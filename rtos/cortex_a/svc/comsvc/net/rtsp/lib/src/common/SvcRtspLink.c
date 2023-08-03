/**
*  @file SvcRtspLink.c
*
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*
*  @details RTSP Ambalink
*
*/
#if defined(CONFIG_LINUX)
#include <stdlib.h>
#endif

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaLink.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_LU_ExamFrameShareRtsp.h"
#include "AmbaIPC_RpcProg_RT_ExamFrameShareRtsp.h"
#include "AmbaCache.h"
#include "AmbaIPC_RpcProg_LU_Util.h"
#include "AmbaMMU.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaDSP.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Event.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AIN.h"
#include "SvcRtspLink.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaSvcWrap.h"

#define RTSP_STACKSIZE        (0x2000U)
#define RTSP_MAXSTREAMNUM     (4U)

#define RTSP_HANDLER_VID      (0x0000000000000001U)
#define RTSP_HANDLER_AUD      (0x0000000000000002U)
#define RTSP_HANDLER_GSENSOR  (0x0000000000000003U)

#if defined(CONFIG_ARM64) //(__WORDSIZE == 64) || defined(__x86_64__) || (__INTPTR_MAX__ == 9223372036854775807LL)
    #define POINTER_MASK        0xFFFFFFFFFFFFFFFF
#else
    #define POINTER_MASK        0x00000000FFFFFFFF
#endif
#define CAST_TO_32_BITS(value)       (UINT32)((value) & 0x00000000FFFFFFFF)
// 32 bits pointer conver to uint64_t
#define CAST_TO_UINTPRT(ptr)         ((UINT64)((UINT32)(ptr)) & POINTER_MASK)
// uint64_t conver to 32 bits pointer
#define CAST_TO_PTR(uintPtr)         (void*)((UINT32)(((UINT64)(uintPtr)) & POINTER_MASK))

#define NETFIFO_GSENSOR_STREAM_ID (0x5a5a)

typedef enum _AMBA_EXAMFRAMESHARE_CONTROL_CMD_e_ {
    AMBA_EXAMFRAMESHARE_CMD_STARTENC = 1,        /**< Start encode. */
    AMBA_EXAMFRAMESHARE_CMD_STOPENC,             /**< Stop encode. */
} AMBA_EXAMFRAMESHARE_CONTROL_CMD_e;

typedef enum _AMBA_EXAMFRAMESHARE_FRAME_TYPE_e_ {
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_MJPEG_FRAME      = 0,    ///< MJPEG frame type
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_IDR_FRAME        = 1,    ///< Idr frame type
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_I_FRAME          = 2,    ///< I frame type
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_P_FRAME          = 3,    ///< P frame type
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_B_FRAME          = 4,    ///< B frame type
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_JPEG_FRAME       = 5,    ///< jpeg main frame
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_THUMBNAIL_FRAME  = 6,    ///< jpeg thumbnail frame
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_SCREENNAIL_FRAME = 7,    ///< jpeg screennail frame
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_AUDIO_FRAME      = 8,    ///< audio frame
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_UNDEFINED        = 9,    ///< others
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_GSENSOR_FRAME    = 10,
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_EOS              = 255,  ///< eos bits that feed to raw buffer
    AMBA_EXAMFRAMESHARE_FRAME_TYPE_LAST = AMBA_EXAMFRAMESHARE_FRAME_TYPE_EOS
} AMBA_EXAMFRAMESHARE_FRAME_TYPE_e;

typedef struct {
    UINT32  SeqNum;     /**< sequential number of bits buffer */
    UINT64  Pts;        /**< time stamp in ticks */
    UINT8   Type;       /**< data type of the entry. see AMBA_EXAMFRAMESHARE_FRAME_TYPE_e*/
    UINT8   Completed;  /**< if the buffer content a complete entry */
    UINT16  Align;      /** data size alignment (in bytes, align = 2^n, n is a integer )*/
    UINT64  StartAddr;  /**< start address of data */
    UINT32  Size;       /**< real data size */
} AMBA_EXAMFRAMESHARE_FRAME_DESC_s;

typedef enum _AMBA_EXAMFRAMESHARE_MEDIA_TYPE_e_ {
    AMBA_EXAMFRAMESHARE_MEDIA_TYPE_VIDEO = 1,  /**< The type is Video */
    AMBA_EXAMFRAMESHARE_MEDIA_TYPE_AUDIO = 2,  /**< The type is Audio */
    AMBA_EXAMFRAMESHARE_MEDIA_TYPE_TEXT  = 3,  /**< The type is Text */
    AMBA_EXAMFRAMESHARE_MEDIA_TYPE_MAX   = 4   /**< Max value, for check use */
} AMBA_EXAMFRAMESHARE_MEDIA_TYPE_e;

typedef enum {
    AMP_VIDEO_MODE_P               = 0,  /**< Progressive */
    AMP_VIDEO_MODE_I_A_FRM_PER_SMP = 1,  /**< Frame per sample */
    AMP_VIDEO_MODE_I_A_FLD_PER_SMP = 2   /**< Field per sample */
} AMP_VIDEO_MODE_s;

typedef enum {
    #define AMP_FORMAT_MID_A_MASK   0x20U
    #define AMP_FORMAT_MID_T_MASK   0x40U
    AMP_FORMAT_MID_H264  = 0x01,                            /**< H264 media id */
    AMP_FORMAT_MID_AVC   = 0x02,                            /**< AVC media id */
    AMP_FORMAT_MID_MJPEG = 0x03,                            /**< MJPEG media id */
    AMP_FORMAT_MID_H265  = 0x04,                            /**< H265 media id */
    AMP_FORMAT_MID_HVC   = 0x05,                            /**< HVC media id */
    AMP_FORMAT_MID_AAC   = (AMP_FORMAT_MID_A_MASK | 0x01),  /**< AAC media id */
    AMP_FORMAT_MID_PCM   = (AMP_FORMAT_MID_A_MASK | 0x02),  /**< PCM media id */
    AMP_FORMAT_MID_ADPCM = (AMP_FORMAT_MID_A_MASK | 0x03),  /**< ADPCM media id */
    AMP_FORMAT_MID_MP3   = (AMP_FORMAT_MID_A_MASK | 0x04),  /**< MP3 media id */
    AMP_FORMAT_MID_AC3   = (AMP_FORMAT_MID_A_MASK | 0x05),  /**< AC3 media id */
    AMP_FORMAT_MID_WMA   = (AMP_FORMAT_MID_A_MASK | 0x06),  /**< WMA media id */
    AMP_FORMAT_MID_OPUS  = (AMP_FORMAT_MID_A_MASK | 0x07),  /**< OPUS media id */
    AMP_FORMAT_MID_LPCM  = (AMP_FORMAT_MID_A_MASK | 0x08),  /**< LPCM media id */
    AMP_FORMAT_MID_TEXT  = (AMP_FORMAT_MID_T_MASK | 0x01),  /**< Text media id */
    AMP_FORMAT_MID_MP4S  = (AMP_FORMAT_MID_T_MASK | 0x02)   /**< MP4S media id */
} AMP_FORMAT_MID_e;

typedef enum {
    AMP_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX = 1,   /**< The track's type is Video */
    AMP_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MAX_IDX = 32,
    AMP_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MIN_IDX = 33,  /**< The track's type is Audio */
    AMP_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MAX_IDX = 64,
    AMP_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MIN_IDX  = 65,  /**< The track's type is Text */
    AMP_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MAX_IDX  = 96,
} AMP_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_e;

typedef struct {
    UINT32                            IsInit;
    UINT32                            Status;
    UINT32                            LivePlaying;
    UINT32                            GSensorLivePlaying;
    CLIENT_ID_t                       ClientID;
    UINT32                            StreamNum;
    SVC_RTSP_VIR2PHY_f                pVir2Phy;
    SVC_RTSP_AUD_HDLR2ID_f            pAudHdlr2Id;

#define RTSPLINK_FLG_SERVERDONE  (1U)
    AMBA_KAL_EVENT_FLAG_t             Flag;

#if defined(CONFIG_LINUX)
    AMBA_KAL_MSG_QUEUE_t              InfoResQue;
    AMBA_KAL_MSG_QUEUE_t              InfoReqQue;
    AMBA_KAL_MSG_QUEUE_t              FrameReqQue;
    AMBA_KAL_TASK_t                   InfoTask;
#endif
} SVC_FSRTSP_INFO_s;

typedef struct {
    UINT32       MediaType;
    UINT32       RecStreamId;
    UINT32       TrackType;

    SVC_EXAMFRAMESHARE_VID_CONFIG_s  VConfig;
    SVC_EXAMFRAMESHARE_AUD_CONFIG_s  AConfig;
} SVC_FSRTSP_TRACK_s;

typedef struct {
    UINT32                            IsUsed;
    const char                        *Name;
    UINT32                            RecStreamId;
    UINT32                            IsAudioExist;
    #define RTSPLINK_MAX_TRACK_NUM  (32U)
    SVC_FSRTSP_TRACK_s                Track[RTSPLINK_MAX_TRACK_NUM];
    UINT32                            TrackNum;

} SVC_FSRTSP_STREAM_s;

#if defined(CONFIG_LINUX)
#define RTSPINFO_QUE_OUTSIZE  (1536U)
#define RTSPINFO_QUE_INPSIZE  (256U)
#define RTSPINFO_QUE_MAXNUM   (32U)

/* request msg */
typedef struct RTSPInfo_ReqQue_s {
    int           Type;  /* _RT_EXAMFRAMESHARERTSP_FUNC_e_ */
    unsigned char Input[RTSPINFO_QUE_INPSIZE];
} RTSPInfo_ReqQue_t;

/* response msg */
typedef struct {
    int           Type;  /* _RT_EXAMFRAMESHARERTSP_FUNC_e_ */
    int           Status;
    unsigned char Output[RTSPINFO_QUE_OUTSIZE];
} RTSPInfo_ResQue_t;

#define RTSPFRAME_QUE_OUTSIZE (512U)
#define RTSPFRAME_QUE_MAXNUM  (128U)

typedef struct {
    int           Type;  /* _LU_EXAMFRAMESHARERTSP_FUNC_e_ */
    unsigned char Output[RTSPFRAME_QUE_OUTSIZE];
} RTSPFrame_ResQue_t;

#endif

static SVC_FSRTSP_INFO_s     MainInfo = {0U};
static SVC_FSRTSP_STREAM_s   StreamInfo[RTSP_MAXSTREAMNUM] = {0};

static const char *StreamName[RTSP_MAXSTREAMNUM] =
{
    "live-internal-main",
    "live-internal",
    "live-external",
    "live-external-sub",
};

static UINT32 CreatRPCClnt(void);
static UINT32 RegisterRpc(void);
static UINT32 StartRtspServer(void);
static void   RT_ExamFrameShareRtsp_GetMediaStreamIDList_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
static void   RT_ExamFrameShareRtsp_GetMediaInfo_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
static void   RT_ExamFrameShareRtsp_GetEncInfo_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
static void   RT_ExamFrameShareRtsp_Pb_OP_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

static UINT32 GetRTSPStreamID(UINT32 RecStreamId)
{
    UINT32 i = 0, RTSPStreamID = 0U, found = 0;

    /* now we only use one rtsp stream for all record streams */
    //AmbaMisra_TouchUnused(&RecStreamId);
    //for (i = 0U; i < RTSP_MAXSTREAMNUM; i++) {
        //if (StreamInfo[i].RecStreamId == RecStreamId) {
            //RTSPStreamID = i;
            //break;
        //}
    //}
    for (i = 0U; i < RTSP_MAXSTREAMNUM; i++) {
        if (StreamInfo[i].IsUsed && StreamInfo[i].RecStreamId == RecStreamId) {
            RTSPStreamID = i;
            found = 1;
            break;
        }
    }
    if (found == 0) {
        for (i = 0U; i < RTSP_MAXSTREAMNUM; i++) {
            if (StreamInfo[i].IsUsed == 0U) {
                found = 1;
                RTSPStreamID = i;
                break;
            }
        }
    }

    return (found == 1) ? RTSPStreamID : RTSP_MAXSTREAMNUM;
}

static UINT32 CreatRPCClnt(void)
{
    UINT32 Rval = OK;

#if defined(CONFIG_LINUX)
    {
        UINT32 Err;
        static char  QueName[]  = "RTSPFrame_RequestQue";

        Err = AmbaKAL_MsgQueueCreate(&(MainInfo.FrameReqQue), QueName, (UINT32)sizeof(RTSPInfo_ReqQue_t), NULL, (UINT32)RTSPINFO_QUE_MAXNUM * (UINT32)sizeof(RTSPInfo_ReqQue_t));
        if (Err != 0) {
            AmbaPrint_PrintUInt5("AmbaKAL_MsgQueueCreate RTSPInfo_RequestQue failed %u", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }
    }
#else
    {
        MainInfo.ClientID = AmbaIPC_ClientCreate((INT32)LU_EXAMFRAMESHARERTSP_HOST,
                                                 (INT32)LU_EXAMFRAMESHARERTSP_PROG_ID,
                                                 (INT32)LU_EXAMFRAMESHARERTSP_VER);

        if(MainInfo.ClientID == NULL) {
            Rval = ERR_NA;
        }
    }
#endif

    return Rval;
}

#if defined(CONFIG_LINUX)
static void* RTSPInfo_Task(void* EntryArg)
{
    UINT32                Err, Loop = 1U, SendMsg;
    UINT8                 *pBuf;
    RTSPInfo_ReqQue_t     ReqMsg;
    RTSPInfo_ResQue_t     ResMsg;
    AMBA_IPC_SVC_RESULT_s TempRst;

    pBuf = &(ResMsg.Output[0]);
    AmbaMisra_TypeCast(&(TempRst.pResult), &(pBuf));
    AmbaMisra_TouchUnused(EntryArg);

    while (Loop == 1U) {
        SendMsg = 0U;
        Err = AmbaKAL_MsgQueueReceive(&(MainInfo.InfoReqQue), &ReqMsg, AMBA_KAL_WAIT_FOREVER);
        if (Err != 0) {
            AmbaPrint_PrintUInt5("AmbaKAL_MsgQueueReceive InfoReqQue failed %u", Err, 0U, 0U, 0U, 0U);
        } else {
            switch(ReqMsg.Type) {
            case RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIASTREAMID:
                RT_ExamFrameShareRtsp_GetMediaStreamIDList_Svc(ReqMsg.Input, &TempRst);
                ResMsg.Type   = RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIASTREAMID;
                ResMsg.Status = TempRst.Status;
                SendMsg = 1U;
                break;
            case RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIAINFO:
                RT_ExamFrameShareRtsp_GetMediaInfo_Svc(ReqMsg.Input, &TempRst);
                ResMsg.Type   = RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIAINFO;
                ResMsg.Status = TempRst.Status;
                SendMsg = 1U;
                break;
            case RT_EXAMFRAMESHARERTSP_FUNC_GETENCINFO:
                RT_ExamFrameShareRtsp_GetEncInfo_Svc(ReqMsg.Input, &TempRst);
                ResMsg.Type   = RT_EXAMFRAMESHARERTSP_FUNC_GETENCINFO;
                ResMsg.Status = TempRst.Status;
                SendMsg = 1U;
                break;
            case RT_EXAMFRAMESHARERTSP_FUNC_PLAYBACK_OP:
                RT_ExamFrameShareRtsp_Pb_OP_Svc(ReqMsg.Input, &TempRst);
                ResMsg.Type   = RT_EXAMFRAMESHARERTSP_FUNC_PLAYBACK_OP;
                ResMsg.Status = TempRst.Status;
                SendMsg = 1U;
                break;
            case RT_EXAMFRAMESHARERTSP_FUNC_REPORTSTATUS:
                /* nothing */
                break;
            default:
                /* nothing */
                break;
            }
        }

        if (SendMsg > 0U) {
            Err = AmbaKAL_MsgQueueSend(&(MainInfo.InfoResQue), &ResMsg, 5000U);
            if (Err != 0) {
                AmbaPrint_PrintUInt5("AmbaKAL_MsgQueueSend InfoResQue failed %u", Err, 0U, 0U, 0U, 0U);
            }
        }
    }

    return NULL;
}
#endif

static UINT32 RegisterRpc(void)
{
    UINT32 Rval = OK;

#if defined(CONFIG_LINUX)
    {
        UINT32 Err;

        {
            static char  QueName[]  = "RTSPInfo_RequestQue";
            Err = AmbaKAL_MsgQueueCreate(&(MainInfo.InfoReqQue), QueName, (UINT32)sizeof(RTSPInfo_ReqQue_t), NULL, (UINT32)RTSPINFO_QUE_MAXNUM * (UINT32)sizeof(RTSPInfo_ReqQue_t));
            if (Err != 0) {
                AmbaPrint_PrintUInt5("AmbaKAL_MsgQueueCreate RTSPInfo_RequestQue failed %u", Err, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (Rval == OK) {
            static char  QueName[]  = "RTSPInfo_ResponseQue";
            Err = AmbaKAL_MsgQueueCreate(&(MainInfo.InfoResQue), QueName, (UINT32)sizeof(RTSPInfo_ResQue_t), NULL, (UINT32)RTSPINFO_QUE_MAXNUM * (UINT32)sizeof(RTSPInfo_ResQue_t));
            if (Err != 0) {
                AmbaPrint_PrintUInt5("AmbaKAL_MsgQueueCreate RTSPInfo_ResponseQue failed %u", Err, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (Rval == OK) {
        #define RTSPInfoStackSize (0x3000U)
            static char   TaskName[] = "RTSPInfoTask";
            static UINT8  TaskStack[RTSPInfoStackSize];

            Err = AmbaKAL_TaskCreate(&(MainInfo.InfoTask),
                                     TaskName,
                                     30U,
                                     RTSPInfo_Task,
                                     NULL,
                                     TaskStack,
                                     RTSPInfoStackSize,
                                     0);
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_PrintUInt5("AmbaKAL_TaskCreate RTSPInfoTask failed %u", Err, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (Rval == OK) {
            Err = AmbaKAL_TaskSetSmpAffinity(&(MainInfo.InfoTask), 0x1U);
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_PrintUInt5("AmbaKAL_TaskSetSmpAffinity RTSPInfoTask failed %u", Err, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (Rval == OK) {
            Err = AmbaKAL_TaskResume(&(MainInfo.InfoTask));
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_PrintUInt5("AmbaKAL_TaskResume RTSPInfoTask failed %u", Err, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }
    }
#else
    {
        INT32                 Err;
        UINT32                Num;
        static UINT8          SvcRtspStack[RTSP_STACKSIZE] GNU_SECTION_NOZEROINIT;
        AMBA_IPC_PROG_INFO_s  Info;
        AMBA_IPC_PROC_s       Proc[RT_EXAMFRAMESHARERTSP_FUNC_AMOUNT - 1U];

        Info.ProcNum   = (UINT32)RT_EXAMFRAMESHARERTSP_FUNC_AMOUNT - 1U;
        Info.pProcInfo = Proc;

        Num = (UINT32)RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIASTREAMID - 1U;
        Info.pProcInfo[Num].Mode = AMBA_IPC_SYNCHRONOUS;
        Info.pProcInfo[Num].Proc = RT_ExamFrameShareRtsp_GetMediaStreamIDList_Svc;

        Num = (UINT32)RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIAINFO - 1U;
        Info.pProcInfo[Num].Mode = AMBA_IPC_SYNCHRONOUS;
        Info.pProcInfo[Num].Proc = RT_ExamFrameShareRtsp_GetMediaInfo_Svc;

        Num = (UINT32)RT_EXAMFRAMESHARERTSP_FUNC_GETENCINFO - 1U;
        Info.pProcInfo[Num].Mode = AMBA_IPC_SYNCHRONOUS;
        Info.pProcInfo[Num].Proc = RT_ExamFrameShareRtsp_GetEncInfo_Svc;

        Num = (UINT32)RT_EXAMFRAMESHARERTSP_FUNC_PLAYBACK_OP - 1U;
        Info.pProcInfo[Num].Mode = AMBA_IPC_SYNCHRONOUS;
        Info.pProcInfo[Num].Proc = RT_ExamFrameShareRtsp_Pb_OP_Svc;

        Err = AmbaIPC_SvcRegister(RT_EXAMFRAMESHARERTSP_PROG_ID, RT_EXAMFRAMESHARERTSP_VER, RT_EXAMFRAMESHARERTSP_NAME,
                                  85U, SvcRtspStack, RTSP_STACKSIZE, &Info, 1);

        if (Err != 0) {
            AmbaPrint_PrintUInt5("AmbaIPC_SvcRegister failed", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }
    }
#endif
    return Rval;
}

static UINT32 StartRtspServer(void)
{
    UINT32 Rval = OK;

#if defined(CONFIG_LINUX)
    {
        int Err;

        Err = system("/usr/bin/AmbaRTSPServer_FrameShare &");
        if (Err != 0) {
            printf("modprobe AmbaRTSPServer_FrameShare fail %d.\n", Err);
            Rval = ERR_NA;
        }
    }
#else
    {
        AMBA_RPC_PROG_EXEC_ARG_s  *pExec;
        CLIENT_ID_t               ID;
        AMBA_IPC_REPLY_STATUS_e   Status;
        UINT32                    Err;
        static char               SrcStr[] = "/usr/bin/AmbaRTSPServer_FrameShare &";
        const char                *pSrcStr = SrcStr;
        INT32                     Output[EXEC_OUTPUT_SIZE];

        ID = AmbaIPC_ClientCreate((INT32)AMBA_RPC_PROG_LU_UTIL_HOST, AMBA_RPC_PROG_LU_UTIL_PROG_ID, AMBA_RPC_PROG_LU_UTIL_VER);
        AmbaMisra_TypeCast(&pExec, &pSrcStr);

        Status = AmbaRpcProg_Util_Exec2_Clnt(pExec, Output, ID);
        if (Status != AMBA_IPC_REPLY_SUCCESS) {
            AmbaPrint_PrintUInt5("AmbaRpcProg_Util_Exec1_Clnt status %u", Status, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        Err = (UINT32)AmbaIPC_ClientDestroy(ID);
        if (Err != 0U) {
            AmbaPrint_PrintUInt5("AmbaIPC_ClientDestroy failed(%u)", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }
    }
#endif

    return Rval;
}

static void RT_ExamFrameShareRtsp_GetMediaStreamIDList_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    UINT32                                   i;
    RT_EXAMFRAMESHARERTSP_STREAMITEM_LIST_s  *pInfo;

    AmbaMisra_TouchUnused(pArg);

    AmbaMisra_TypeCast(&pInfo, &(pRet->pResult));

    pInfo->Amount = (INT32)MainInfo.StreamNum;
    pRet->Status  = AMBA_IPC_REPLY_SUCCESS;
    pRet->Length  = sizeof(RT_EXAMFRAMESHARERTSP_STREAMITEM_LIST_s);
    pRet->Mode    = AMBA_IPC_SYNCHRONOUS;
    AmbaPrint_PrintUInt5("GetMediaStreamIDList_Svc", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < MainInfo.StreamNum; i++) {
        AmbaUtility_StringCopy(pInfo->StreamItemList[i].Name, RT_EXAMFRAMESHARERTSP_MAX_STREAMNAME_LENGTH, StreamInfo[i].Name);
        pInfo->StreamItemList[i].Active = 1U;
        pInfo->StreamItemList[i].StrmId = StreamInfo[i].RecStreamId;
        AmbaPrint_PrintUInt5("RTSP Stream%u: strm# %d", i, StreamInfo[i].RecStreamId, 0U, 0U, 0U);
        AmbaPrint_PrintStr5("              %s", pInfo->StreamItemList[i].Name, NULL, NULL, NULL, NULL);
    }
}

static void RT_ExamFrameShareRtsp_GetMediaInfo_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    UINT32                                  *pStreamId, i, Err;
    RT_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s  *pInfo;
    SVC_FSRTSP_STREAM_s                     *pStream;
    SVC_FSRTSP_TRACK_s                      *pTrack;
    UINT32                                  StreamId = RTSP_MAXSTREAMNUM;

    AmbaMisra_TypeCast(&pStreamId, &pArg);
    AmbaMisra_TypeCast(&pInfo, &(pRet->pResult));

    for (i = 0; i < MainInfo.StreamNum; i++) {
        if (*pStreamId == StreamInfo[i].RecStreamId) {
            StreamId = i;
            break;
        }
    }
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    AmbaPrint_PrintUInt5("GetMediaInfo_Svc: strm# %d", *pStreamId, 0U, 0U, 0U, 0U);

    if (StreamId < MainInfo.StreamNum) {
        pStream = &(StreamInfo[StreamId]);

        pInfo->nTrack = pStream->TrackNum;

        for (i = 0U; i < pStream->TrackNum; i++) {
            pTrack = &(pStream->Track[i]);

            if (pTrack->MediaType == AMBA_EXAMFRAMESHARE_MEDIA_TYPE_VIDEO) {
                AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pVideo = &(pTrack->VConfig.VidInfo);

                pInfo->Track[i].nTrackType    = (UINT8)pTrack->TrackType;
                pInfo->Track[i].nTimeScale    = pVideo->FrameRate.TimeScale;
                pInfo->Track[i].nTimePerFrame = pVideo->FrameRate.NumUnitsInTick;
                pInfo->Track[i].nInitDelay    = 0U;
                pInfo->Track[i].hCodec        = 1U;
                pInfo->Track[i].pBufferBase   = pVideo->EncConfig.BitsBufAddr;
                pInfo->Track[i].pBufferLimit  = pVideo->EncConfig.BitsBufAddr + pVideo->EncConfig.BitsBufSize;

                pInfo->Track[i].Info.Video.nGOPSize          = pVideo->EncConfig.GOPCfg.N * pVideo->EncConfig.GOPCfg.IdrInterval;
                pInfo->Track[i].Info.Video.nTrickRecDen      = 1UL;
                pInfo->Track[i].Info.Video.nTrickRecNum      = (pVideo->FrameRate.TimeScale / ((pVideo->FrameRate.Interlace + 1U) * pVideo->FrameRate.NumUnitsInTick)) / 30U;
                pInfo->Track[i].Info.Video.nCodecTimeScale   = pVideo->FrameRate.TimeScale;
                pInfo->Track[i].Info.Video.nWidth            = pVideo->Window.Width;
                pInfo->Track[i].Info.Video.nHeight           = pVideo->Window.Height;
                pInfo->Track[i].Info.Video.nM                = pVideo->EncConfig.GOPCfg.M;
                pInfo->Track[i].Info.Video.nN                = pVideo->EncConfig.GOPCfg.N;
                pInfo->Track[i].Info.Video.nIRCycle          = pVideo->EncConfig.IntraRefreshCtrl.IntraRefreshCycle;
                pInfo->Track[i].Info.Video.nRecoveryFrameCnt = 0U;
                pInfo->Track[i].Info.Video.bDefault          = 1U;
                pInfo->Track[i].Info.Video.bClosedGOP        = 0U;

                if (pVideo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H264) {
                    pInfo->Track[i].nMediaId      = (UINT32)AMP_FORMAT_MID_AVC;
            #if defined(AMBA_DSP_ENC_CODING_TYPE_H265)
                } else if (pVideo->CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
                    pInfo->Track[i].nMediaId      = AMP_FORMAT_MID_H265;
            #endif
                } else {
                    AmbaPrint_PrintUInt5("Invalid coding format(%u)", pVideo->CodingFmt, 0U, 0U, 0U, 0U);
                    pRet->Status = AMBA_IPC_REPLY_PARA_INVALID;
                }

                if (pVideo->FrameRate.Interlace == 0U) {
                    pInfo->Track[i].Info.Video.nMode  = (UINT8)AMP_VIDEO_MODE_P;
                } else {
                    pInfo->Track[i].Info.Video.nMode  = (UINT8)AMP_VIDEO_MODE_I_A_FLD_PER_SMP;
                }

                if (pInfo->Track[i].Info.Video.nIRCycle > 0U) {
                    pInfo->Track[i].Info.Video.nN                = 0xFF;
                    pInfo->Track[i].Info.Video.nRecoveryFrameCnt = pInfo->Track[i].Info.Video.nIRCycle - 1U;
                }
            } else if (pTrack->MediaType == AMBA_EXAMFRAMESHARE_MEDIA_TYPE_AUDIO) {
                SVC_EXAMFRAMESHARE_AUD_CONFIG_s *pAudio = &(pTrack->AConfig);

                pInfo->Track[i].nTimeScale     = pAudio->AudInfo.SampleFreq;
                pInfo->Track[i].nTimePerFrame  = pAudio->AudInfo.FrameSize;
                pInfo->Track[i].hCodec         = 1ULL;
                pInfo->Track[i].pBufferBase    = pAudio->BitsBufBase;
                pInfo->Track[i].pBufferLimit   = pAudio->BitsBufBase + pAudio->BitsBufSize;
                pInfo->Track[i].nTrackType     = (UINT8)pTrack->TrackType;

                pInfo->Track[i].Info.Audio.nSampleRate    = pAudio->AudInfo.SampleFreq;
                pInfo->Track[i].Info.Audio.bDefault       = 1U;
                pInfo->Track[i].Info.Audio.nChannels      = (UINT8)pAudio->AudInfo.ChannelNum;
                pInfo->Track[i].Info.Audio.nBitsPerSample = (UINT8)pAudio->AudInfo.SampleResolution;

                if (pAudio->Format == RTSP_LINK_AUD_AAC) {
                    pInfo->Track[i].nMediaId      = (UINT32)AMP_FORMAT_MID_AAC;
                } else if (pAudio->Format == RTSP_LINK_AUD_PCM) {
                    pInfo->Track[i].nMediaId      = (UINT32)AMP_FORMAT_MID_PCM;
                } else {
                    AmbaPrint_PrintUInt5("Invalid audio coding format(%u)", pAudio->Format, 0U, 0U, 0U, 0U);
                    pRet->Status = AMBA_IPC_REPLY_PARA_INVALID;
                }

            } else {
                /* nothing */
            }
        }
    } else {
        AmbaPrint_PrintUInt5("Invalid RTSP StreamId(%u)", StreamId, 0U, 0U, 0U, 0U);
        pRet->Status = AMBA_IPC_REPLY_PARA_INVALID;
    }

    Err = AmbaKAL_EventFlagSet(&(MainInfo.Flag), RTSPLINK_FLG_SERVERDONE);
    if (Err != KAL_ERR_NONE) {
        AmbaPrint_PrintUInt5("AmbaKAL_EventFlagSet failed(%u)", Err, 0U, 0U, 0U, 0U);
    }

    pRet->Length  = sizeof(RT_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s);
    pRet->Mode    = AMBA_IPC_SYNCHRONOUS;
}

unsigned char gsensor_buffer[64] = {0};
static void RT_ExamFrameShareRtsp_GetEncInfo_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s  *pInfo;
    SVC_FSRTSP_STREAM_s                  *pStream;
    UINT32                               *pStreamId;
    UINT32                               Err, i;
    ULONG                                PhyAddr;

    AmbaMisra_TouchUnused(&pArg);
    AmbaMisra_TypeCast(&pInfo, &(pRet->pResult));
    AmbaMisra_TypeCast(&pStreamId, &pArg);

    AmbaPrint_PrintUInt5("GetEncInfo_Svc: strm# 0x%x", *pStreamId, 0U, 0U, 0U, 0U);
    if (*pStreamId == NETFIFO_GSENSOR_STREAM_ID) {
        UINT32 ShareAddr = (UINT32)&gsensor_buffer[0];
        Err = MainInfo.pVir2Phy(ShareAddr, &PhyAddr);
        if (Err == MMU_ERR_NONE) {
            pInfo->buf_base_gsensor = (UINT64)ShareAddr;
            pInfo->buf_base_phy_gsensor = (UINT64)PhyAddr;
            pInfo->buf_size_gsensor = sizeof(gsensor_buffer);
            pInfo->gsensor_hndlr = RTSP_HANDLER_GSENSOR;
        }
    } else {
        if (*pStreamId < MainInfo.StreamNum) {
            SVC_EXAMFRAMESHARE_VID_CONFIG_s  *pVidCfg = NULL;
            SVC_EXAMFRAMESHARE_AUD_CONFIG_s  *pAudCfg = NULL;

            pStream = &(StreamInfo[*pStreamId]);

            for (i = 0U; i < pStream->TrackNum; i++) {
                if (pStream->Track[i].MediaType == AMBA_EXAMFRAMESHARE_MEDIA_TYPE_VIDEO) {
                    pVidCfg = &(pStream->Track[i].VConfig);
                }
                if (pStream->Track[i].MediaType == AMBA_EXAMFRAMESHARE_MEDIA_TYPE_AUDIO) {
                    pAudCfg = &(pStream->Track[i].AConfig);
                }
            }

            if (pVidCfg != NULL) {
                Err = MainInfo.pVir2Phy(pVidCfg->VidInfo.EncConfig.BitsBufAddr, &PhyAddr);
                if (Err != KAL_ERR_NONE) {
                    AmbaPrint_PrintUInt5("pVir2Phy failed(%u)", Err, 0U, 0U, 0U, 0U);
                    pRet->Status = AMBA_IPC_REPLY_PARA_INVALID;
                }

                pInfo->buf_base_v     = pVidCfg->VidInfo.EncConfig.BitsBufAddr;
                pInfo->buf_base_phy_v = PhyAddr;
                pInfo->buf_size_v     = pVidCfg->VidInfo.EncConfig.BitsBufSize;
                pInfo->v_hndlr        = RTSP_HANDLER_VID;
            }

            if (pAudCfg != NULL) {
                Err = MainInfo.pVir2Phy(pAudCfg->BitsBufBase, &PhyAddr);
                if (Err != KAL_ERR_NONE) {
                    AmbaPrint_PrintUInt5("pVir2Phy failed(%u)", Err, 0U, 0U, 0U, 0U);
                    pRet->Status = AMBA_IPC_REPLY_PARA_INVALID;
                }

                pInfo->buf_base_a     = pAudCfg->BitsBufBase;
                pInfo->buf_base_phy_a = PhyAddr;
                pInfo->buf_size_a     = pAudCfg->BitsBufSize;
                pInfo->a_hndlr        = RTSP_HANDLER_AUD;
            }
        } else {
            AmbaPrint_PrintUInt5("Invalid RTSP StreamId(%u)", *pStreamId, 0U, 0U, 0U, 0U);
            pRet->Status = AMBA_IPC_REPLY_PARA_INVALID;
        }
    }

    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Length = sizeof(RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s);
    pRet->Mode   = AMBA_IPC_SYNCHRONOUS;
}

static UINT32 RtspPlaybackCB(void *hdlr, UINT32 event, void* info)
{
    int Rval = OK;
    (void)hdlr;
    (void)event;
    (void)info;

    switch(event) {
    case RTSP_LIVE_PLAY:
        MainInfo.LivePlaying++;        
        AmbaPrint_PrintUInt5("RtspPlaybackCB, RTSP_LIVE_PLAY(%d)", MainInfo.LivePlaying, 0U, 0U, 0U, 0U);
        break;
    case RTSP_LIVE_STOP:
        if (MainInfo.LivePlaying == 0) {
            MainInfo.LivePlaying = 0;
        } else {
            MainInfo.LivePlaying -= 1;
        }
        AmbaPrint_PrintUInt5("RtspPlaybackCB, RTSP_LIVE_STOP(%d)", MainInfo.LivePlaying, 0U, 0U, 0U, 0U);
        //MainInfo.LivePlaying = 0U;
        break;
    case RTSP_PLAYBACK_GSENSOR_PLAY:
        AmbaPrint_PrintStr5("%s, RTSP_PLAYBACK_GSENSOR_PLAY", __func__, NULL, NULL, NULL, NULL);
        MainInfo.GSensorLivePlaying = 1U;
        break;
    case RTSP_PLAYBACK_GSENSOR_STOP:
        AmbaPrint_PrintStr5("%s, RTSP_PLAYBACK_GSENSOR_STOP", __func__, NULL, NULL, NULL, NULL);
        MainInfo.GSensorLivePlaying = 0U;
        break;
    default:
        break;
    }

    return Rval;
}

static void RT_ExamFrameShareRtsp_Pb_OP_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int                             rval = 0;
    UINT32                          op = 0;
    RT_NETFIFO_PLAYBACK_OP_PARAM_s  *pRtosOpParam = NULL, *pLuOpParam = NULL;
    PLAYBACK_OP_INFO_s  localParam;

    AmbaMisra_TypeCast32(&pLuOpParam, &pArg);

    AmbaWrap_memcpy((char*)localParam.inParam, (char*)pLuOpParam->Param, sizeof(localParam.inParam));

    op = pLuOpParam->OP;

    pRtosOpParam = (RT_NETFIFO_PLAYBACK_OP_PARAM_s *)(pRet->pResult);
    AmbaWrap_memset(pRtosOpParam, 0x0, sizeof(RT_NETFIFO_PLAYBACK_OP_PARAM_s));

    localParam.resParam = pRtosOpParam->Param;

    rval = -1;
    {
        void    *pHdlr = (void*)CAST_TO_PTR(pLuOpParam->pHdlr);
        rval = RtspPlaybackCB(pHdlr, op, (void*)(&localParam));

        pRtosOpParam->OP = rval;
    }

    if (OK != rval) {
        pRet->Status = AMBA_IPC_REPLY_PARA_INVALID;
    } else {
        pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    }

    pRet->Length = sizeof(RT_NETFIFO_PLAYBACK_OP_PARAM_s);
    pRet->Mode   = AMBA_IPC_SYNCHRONOUS;
}

/**
* create ambalink rtsp client
* @return OK/ERR_NA.
*/
UINT32 SvcRtspLink_Create(void)
{
    UINT32 Err, Rval = OK;

    if (MainInfo.Status == 1U) {
        AmbaPrint_PrintUInt5("FrameShare_Init Already inited!", 0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaMisra_TouchUnused(&Rval);

        /* create flag */
        if (Rval == OK) {
            static char Name[] = "RtspLinkFlag";

            Err = AmbaKAL_EventFlagCreate(&(MainInfo.Flag), Name);
            if (Err != OK) {
                AmbaPrint_PrintUInt5("AmbaKAL_EventFlagCreate failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (Rval == OK) {
            Err = AmbaKAL_EventFlagClear(&(MainInfo.Flag), 0xFFFFFFFFU);
            if (Err != OK) {
                AmbaPrint_PrintUInt5("AmbaKAL_EventFlagClear failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

    #if defined(CONFIG_LINUX)
        if (Rval == OK) {
            Err = CreatRPCClnt();
            if (Err != OK) {
                AmbaPrint_PrintUInt5("CreatRPCClnt failed", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }
    #endif

        if (Rval == OK) {
            Err = RegisterRpc();
            if (Err != OK) {
                AmbaPrint_PrintUInt5("RegisterRpc failed", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (Rval == OK) {
            Err = StartRtspServer();
            if (Err != OK) {
                AmbaPrint_PrintUInt5("StartRtspServer failed", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        /* we should start rtsp server to execute "AmbaRTSPServer_FrameShare" on linux first,  */
        /* and only create RPC after "AmbaRTSPServer_FrameShare" return (only for dualOS)      */

        if (Rval == OK) {
            UINT32 Flags;

            Err = AmbaKAL_EventFlagGet(&(MainInfo.Flag), RTSPLINK_FLG_SERVERDONE, KAL_FLAGS_WAIT_ALL,
                                       AMBA_KAL_FLAGS_CLEAR_NONE, &Flags, 10000U);
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_PrintUInt5("AmbaKAL_EventFlagGet failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

    #if !defined(CONFIG_LINUX)
        if (Rval == OK) {
            Err = CreatRPCClnt();
            if (Err != OK) {
                AmbaPrint_PrintUInt5("CreatRPCClnt failed", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }
    #endif

        if (Rval == OK) {
            MainInfo.Status = 1U;
        }
    }

    return Rval;
}

/**
* control rtsp start/stop
* @param [in]  record stream id
* @param [in]  RTSP_LINK_CMD_START/RTSP_LINK_CMD_STOP
* @param [in]  No/Yes(0/1)
* @return OK/ERR_NA.
*/
UINT32 SvcRtspLink_Command(UINT32 RecStreamId, UINT32 Cmd)
{
    UINT32                               Rval = OK, Out, Err, i;
    AMBA_IPC_REPLY_STATUS_e              Status;
    LU_EXAMFRAMESHARERTSP_CONTROL_ARG_s  Input;
    extern int app_util_check_external_camera_enabled(void);

    AmbaMisra_TouchUnused(&Err);
    AmbaMisra_TouchUnused(&Status);
    AmbaMisra_TouchUnused(&Out);
    AmbaMisra_TouchUnused(&RecStreamId);

    if (MainInfo.Status == 0U) {
        AmbaPrint_PrintUInt5("RPCClnt not created!", 0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    }

    if (Rval == OK) {
        if (Cmd == RTSP_LINK_CMD_START) {
            Input.Cmd = (UINT32)AMBA_EXAMFRAMESHARE_CMD_STARTENC;
            for (i = 0U; i < RTSP_MAXSTREAMNUM; i++) {
                if ((StreamInfo[i].IsUsed == 1U) && (StreamInfo[i].RecStreamId == RecStreamId)) {
                    StreamInfo[i].IsAudioExist = 1;
                }
            }
        } else if (Cmd == RTSP_LINK_CMD_STOP) {
            Input.Cmd = (UINT32)AMBA_EXAMFRAMESHARE_CMD_STOPENC;
        } else {
            AmbaPrint_PrintUInt5("Invalid cmd type%u", Cmd, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }
    }

    if (Rval == OK) {
#if defined(CONFIG_LINUX)
        RTSPFrame_ResQue_t  ResMsg;

        ResMsg.Type = LU_EXAMFRAMESHARERTSP_FUNC_CONTROLEVENT;
        Err = AmbaWrap_memcpy(ResMsg.Output, &Input, sizeof(LU_EXAMFRAMESHARERTSP_CONTROL_ARG_s));
        AmbaMisra_TouchUnused(&Err);

        Err = AmbaKAL_MsgQueueSend(&(MainInfo.FrameReqQue), &ResMsg, LU_EXAMFRAMESHARERTSP_DEFULT_TIMEOUT);
        if (Err != 0) {
            AmbaPrint_PrintUInt5("AmbaKAL_MsgQueueSend FrameReqQue failed %u", Err, 0U, 0U, 0U, 0U);
        }
#else
        if (!(RecStreamId == 2 && app_util_check_external_camera_enabled() == 0)) {
            Status = AmbaIPC_ClientCall(MainInfo.ClientID, (INT32)LU_EXAMFRAMESHARERTSP_FUNC_CONTROLEVENT,
                                        &Input, (INT32)sizeof(LU_EXAMFRAMESHARERTSP_CONTROL_ARG_s),
                                        &Out, (INT32)sizeof(Out), (UINT32)LU_EXAMFRAMESHARERTSP_DEFULT_TIMEOUT);
            
            if (Status != AMBA_IPC_REPLY_SUCCESS) {
                // AmbaPrint_PrintUInt5("AmbaIPC_ClientCall failed(%u)", (UINT32)Status, 0U, 0U, 0U, 0U);
                // Rval = ERR_NA;
            }
        }
#endif
    }

    if (Rval == OK) {
        if (Cmd == RTSP_LINK_CMD_STOP) {
            MainInfo.StreamNum--;
            for (i = 0U; i < RTSP_MAXSTREAMNUM; i++) {
                if (StreamInfo[i].RecStreamId == RecStreamId) {
                    StreamInfo[i].IsUsed = 0U;
                    break;
                }
            }
        }
    }

    return Rval;
}

UINT32 SvcRtspLink_GSensorNotify(void *pDesc)
{
    UINT32 Rval = OK, Err, Out;
    LU_EXAMFRAMESHARERTSP_FRAME_DESC_s      Desc;
    AMBA_IPC_REPLY_STATUS_e                 Status;
    LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s  Input;
    const GSENSOR_DESC_s                    *pGSensor;
    UINT32                                  Addr;

    AmbaMisra_TypeCast(&pGSensor, &pDesc);
    AmbaMisra_TypeCast(&(Addr), &(pGSensor->pBufAddr));

    Desc.SeqNum    = ((UINT32)pGSensor->EncodedSamples) / 1;
    Desc.Type      = (UINT8)AMBA_EXAMFRAMESHARE_FRAME_TYPE_GSENSOR_FRAME;
    Desc.StartAddr = (UINT64)Addr;
    Desc.Size      = pGSensor->DataSize;
    Desc.StrmId    = NETFIFO_GSENSOR_STREAM_ID;

    if (MainInfo.Status == 0U) {
        AmbaPrint_PrintUInt5("RPCClnt not created!", 0U, 0U, 0U, 0U, 0U);
        Rval = ERR_NA;
    }
    Input.hndlr = RTSP_LINK_NOTIFY_GSENSOR;
    Err = AmbaWrap_memcpy(&Input.InfoPtr, &Desc, sizeof(LU_EXAMFRAMESHARERTSP_FRAME_DESC_s));
    AmbaMisra_TouchUnused(&Err);

    if ((Rval == OK) && (1U == MainInfo.GSensorLivePlaying)) {
        Status = AmbaIPC_ClientCall(MainInfo.ClientID, (INT32)LU_EXAMFRAMESHARERTSP_FUNC_GSENSOR_FRAMEEVENT,
                                    &Input, sizeof(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s),
                                    &Out, sizeof(Out), LU_EXAMFRAMESHARERTSP_DEFULT_TIMEOUT);
        if (Status != AMBA_IPC_REPLY_SUCCESS) {
            AmbaPrint_PrintUInt5("AmbaIPC_ClientCall failed(%u)", (UINT32)Status, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }
    }

    return 0;
}

/**
* notify ambalink new source info
* @param [in]  record stream id
* @param [in]  RTSP_LINK_NOTIFY_VID/RTSP_LINK_NOTIFY_AUD
* @param [in]  source descriptor
* @return OK/ERR_NA.
*/
UINT32 SvcRtspLink_Notify(UINT32 RecStreamId, UINT32 Type, void *pDesc)
{
    UINT32                                  Rval = OK, Err, Out, i, StreamId = 0U;
    LU_EXAMFRAMESHARERTSP_FRAME_DESC_s      Desc;
    AMBA_IPC_REPLY_STATUS_e                 Status;
    LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s  Input;
    SVC_FSRTSP_STREAM_s                     *pStream = NULL;
    SVC_FSRTSP_TRACK_s                      *pTrack;

    AmbaMisra_TouchUnused(&Status);
    AmbaMisra_TouchUnused(&Out);

    StreamId = GetRTSPStreamID(RecStreamId);
    if (StreamId < RTSP_MAXSTREAMNUM) {
        pStream = &(StreamInfo[StreamId]);
    } else {
        Rval = ERR_NA;
    }

    if (pStream != NULL) {
        if (Type == RTSP_LINK_NOTIFY_VID) {
            const AMBA_DSP_ENC_PIC_RDY_s *pVideo;

            AmbaMisra_TypeCast(&pVideo, &pDesc);

            pTrack = NULL;

            for (i = 0U; i < pStream->TrackNum; i++) {
                if ((pStream->Track[i].MediaType == AMBA_EXAMFRAMESHARE_MEDIA_TYPE_VIDEO) &&
                    (pStream->Track[i].VConfig.StreamId == pVideo->StreamId)) {

                    pTrack = &(pStream->Track[i]);
                }
            }

            if (pTrack != NULL) {
                Desc.TrackType = pTrack->TrackType;

                if (pVideo->PicSize == AMBA_DSP_ENC_END_MARK) {
                    Desc.Type = (UINT8)AMBA_EXAMFRAMESHARE_FRAME_TYPE_EOS;
                } else {
                    switch(pVideo->FrameType) {
                    case PIC_FRAME_IDR:
                        Desc.Type = (UINT8)AMBA_EXAMFRAMESHARE_FRAME_TYPE_IDR_FRAME;
                        break;
                    case PIC_FRAME_I:
                        Desc.Type = (UINT8)AMBA_EXAMFRAMESHARE_FRAME_TYPE_I_FRAME;
                        break;
                    case PIC_FRAME_P:
                        Desc.Type = (UINT8)AMBA_EXAMFRAMESHARE_FRAME_TYPE_P_FRAME;
                        break;
                    case PIC_FRAME_B:
                        Desc.Type = (UINT8)AMBA_EXAMFRAMESHARE_FRAME_TYPE_B_FRAME;
                        break;
                    case PIC_FRAME_JPG:
                        Desc.Type = (UINT8)AMBA_EXAMFRAMESHARE_FRAME_TYPE_MJPEG_FRAME;
                        break;
                    default:
                        AmbaPrint_PrintUInt5("Unknown frame type(%u)", pVideo->FrameType, 0U, 0U, 0U, 0U);
                        Rval = ERR_NA;
                        break;
                    }
                }

                if (pVideo->SliceIdx == (pVideo->NumSlice - 1U)) {
                    Desc.Completed = 1U;
                } else {
                    Desc.Completed = 0U;
                }

                Desc.SeqNum    = (UINT32)pVideo->FrmNo;
                Desc.Pts       = pVideo->Pts;
                Desc.StartAddr = (UINT64)pVideo->StartAddr;
                Desc.Size      = pVideo->PicSize;
                Desc.Align     = 32U;
                Desc.StrmId = RecStreamId;
            } else {
                AmbaPrint_PrintUInt5("The video source is not registered (%u)", pVideo->StreamId, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }

        } else if (Type == RTSP_LINK_NOTIFY_AUD) {
            const AMBA_AENC_AUDIO_DESC_s        *pAudio;
            ULONG                               Addr;

            AmbaMisra_TypeCast(&pAudio, &pDesc);
            AmbaMisra_TypeCast(&(Addr), &(pAudio->pBufAddr));

            pTrack = NULL;

            for (i = 0U; i < pStream->TrackNum; i++) {
                if ((pStream->Track[i].MediaType == AMBA_EXAMFRAMESHARE_MEDIA_TYPE_AUDIO) &&
                    (pStream->Track[i].AConfig.StreamId == MainInfo.pAudHdlr2Id(pAudio->pHdlr))) {

                    pTrack = &(pStream->Track[i]);
                }
            }

            if (pTrack != NULL) {
                Desc.TrackType = pTrack->TrackType;
                Desc.Pts       = pAudio->EncodedSamples;
                Desc.SeqNum    = (UINT32)pAudio->EncodedSamples / pTrack->AConfig.AudInfo.FrameSize;
                Desc.StartAddr = Addr;
                Desc.Size      = pAudio->DataSize;
                Desc.Align     = 32U;
                Desc.Completed = 1U;
                Desc.Type      = (UINT8)AMBA_EXAMFRAMESHARE_FRAME_TYPE_AUDIO_FRAME;
                Desc.StrmId = RecStreamId;
            } else {
                AmbaPrint_PrintUInt5("The audio source is not registered ", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }

        } else {
            AmbaPrint_PrintUInt5("Unknown data type(%u)", Type, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        if (MainInfo.Status == 0U) {
            AmbaPrint_PrintUInt5("RPCClnt not created!", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        Input.hndlr = Type;
        Err = AmbaWrap_memcpy(&Input.InfoPtr, &Desc, sizeof(LU_EXAMFRAMESHARERTSP_FRAME_DESC_s));
        AmbaMisra_TouchUnused(&Err);

        if ((Rval == OK) && (MainInfo.LivePlaying > 0)) {
    #if defined(CONFIG_LINUX)
            RTSPFrame_ResQue_t  ResMsg;

            ResMsg.Type = LU_EXAMFRAMESHARERTSP_FUNC_FRAMEEVENT;
            Err = AmbaWrap_memcpy(ResMsg.Output, &Input, sizeof(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s));
            AmbaMisra_TouchUnused(&Err);

            Err = AmbaKAL_MsgQueueSend(&(MainInfo.FrameReqQue), &ResMsg, LU_EXAMFRAMESHARERTSP_DEFULT_TIMEOUT);
            if (Err != 0) {
                AmbaPrint_PrintUInt5("AmbaKAL_MsgQueueSend FrameReqQue failed %u", Err, 0U, 0U, 0U, 0U);
            }
    #else
            Status = AmbaIPC_ClientCall(MainInfo.ClientID, (INT32)LU_EXAMFRAMESHARERTSP_FUNC_FRAMEEVENT,
                                        &Input, sizeof(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s),
                                        &Out, sizeof(Out), LU_EXAMFRAMESHARERTSP_DEFULT_TIMEOUT);

            if (Status != AMBA_IPC_REPLY_SUCCESS) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                AmbaPrint_PrintUInt5("AmbaIPC_ClientCall failed(%u)", (UINT32)Status, 0U, 0U, 0U, 0U);
#endif
                Rval = ERR_NA;
            }
    #endif
        }
    }

    return Rval;
}

/**
* Set rtsp info
* @param [in]  record stream id
* @param [in]  video info
* @param [in]  audio info
*/
void SvcRtspLink_SetInfo(UINT32 RecStreamId,
                         SVC_EXAMFRAMESHARE_VID_CONFIG_s  *pVidCfg,
                         SVC_EXAMFRAMESHARE_AUD_CONFIG_s  *pAudCfg,
                         SVC_RTSP_VIR2PHY_f               pVir2Phy,
                         SVC_RTSP_AUD_HDLR2ID_f           pAudHdlr2Id)
{
    UINT32                 Err, StreamId, i;
    SVC_FSRTSP_STREAM_s    *pStream = NULL;
    SVC_FSRTSP_TRACK_s     *pTrack;

    if (MainInfo.IsInit == 0U) {
        Err = AmbaWrap_memset(&MainInfo, 0, sizeof(MainInfo));
        AmbaMisra_TouchUnused(&Err);

        Err = AmbaWrap_memset(StreamInfo, 0, sizeof(StreamInfo));
        AmbaMisra_TouchUnused(&Err);
        for (i = 0U; i < RTSP_MAXSTREAMNUM; i++) {
            StreamInfo[i].IsUsed = 0U;
        }
        MainInfo.IsInit       = 1U;
        MainInfo.pVir2Phy     = pVir2Phy;
        MainInfo.pAudHdlr2Id  = pAudHdlr2Id;
    }

    StreamId = GetRTSPStreamID(RecStreamId);
    if (StreamId < RTSP_MAXSTREAMNUM) {
        pStream = &(StreamInfo[StreamId]);
    }
    if (pStream != NULL) {
        if (pStream->IsUsed == 0U) {
            pStream->IsUsed = 1U;
            pStream->Name   = StreamName[RecStreamId];
            pStream->RecStreamId       = RecStreamId;
            MainInfo.StreamNum++;
        }
        if (pVidCfg != NULL) {
            UINT32 NewTrack = 1U, VidNum = 0U;

            for (i = 0U; i < pStream->TrackNum; i++) {
                if ((pStream->Track[i].MediaType == (UINT32)AMBA_EXAMFRAMESHARE_MEDIA_TYPE_VIDEO)) {
                    VidNum++;
                    if (pStream->Track[i].VConfig.StreamId == pVidCfg->StreamId) {
                        pTrack = &(pStream->Track[i]);

                        Err = AmbaWrap_memcpy(&(pTrack->VConfig), pVidCfg, sizeof(SVC_EXAMFRAMESHARE_VID_CONFIG_s));
                        AmbaMisra_TouchUnused(&Err);
                        NewTrack = 0U;
                        break;
                    }
                }
            }

            if (NewTrack > 0U) {
                pTrack = &(pStream->Track[pStream->TrackNum]);

                pTrack->MediaType   = (UINT32)AMBA_EXAMFRAMESHARE_MEDIA_TYPE_VIDEO;
                pTrack->RecStreamId = RecStreamId;
                pTrack->TrackType   = (UINT32)AMP_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX + VidNum;

                Err = AmbaWrap_memcpy(&(pTrack->VConfig), pVidCfg, sizeof(SVC_EXAMFRAMESHARE_VID_CONFIG_s));
                AmbaMisra_TouchUnused(&Err);

                pStream->TrackNum++;
            }
        }

        if (pAudCfg != NULL) {
            UINT32 NewTrack = 1U, AudNum = 0U;

            for (i = 0U; i < pStream->TrackNum; i++) {
                if ((pStream->Track[i].MediaType == (UINT32)AMBA_EXAMFRAMESHARE_MEDIA_TYPE_AUDIO)) {
                    AudNum++;
                    if (pStream->Track[i].AConfig.StreamId == pAudCfg->StreamId) {
                        pTrack = &(pStream->Track[i]);

                        Err = AmbaWrap_memcpy(&(pTrack->AConfig), pAudCfg, sizeof(SVC_EXAMFRAMESHARE_AUD_CONFIG_s));
                        AmbaMisra_TouchUnused(&Err);
                        NewTrack = 0U;
                        break;
                    }
                }
            }

            if (NewTrack > 0U) {
                pTrack = &(pStream->Track[pStream->TrackNum]);

                pTrack->MediaType   = (UINT32)AMBA_EXAMFRAMESHARE_MEDIA_TYPE_AUDIO;
                pTrack->RecStreamId = RecStreamId;
                pTrack->TrackType   = (UINT32)AMP_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MIN_IDX + AudNum;

                Err = AmbaWrap_memcpy(&(pTrack->AConfig), pAudCfg, sizeof(SVC_EXAMFRAMESHARE_AUD_CONFIG_s));
                AmbaMisra_TouchUnused(&Err);

                pStream->TrackNum++;
            }
        }
    }
}
