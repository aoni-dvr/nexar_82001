/**
 *  @file SvcRecMain.c
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
 *  @details svc record control
 *
 */

#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaUtility.h"
#include "AmbaDef.h"
#include "AmbaSD.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaSYS.h"
#include "AmbaShell.h"
#include "AmbaCache.h"
#include "AmbaVfs.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaAEncFlow.h"
#include "AmbaDCF.h"
#include "AmbaMux.h"
#include "AmbaPrint.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcUtil.h"
#include "SvcClock.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcTask.h"
#include "SvcResCfg.h"
#include "SvcStgMgr.h"
#include "SvcRecQpCtrl.h"
#include "SvcDrawStopwatch.h"
#include "SvcDataGather.h"
#include "SvcDataCmp.h"
#include "SvcRecBlend.h"
#include "SvcStgMonitor.h"
#include "SvcTimeLapse.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#if defined(CONFIG_ICAM_PLAYBACK_USED)
#include "SvcPbkCtrl.h"
#endif
#if defined(CONFIG_AMBA_RTSP)
#if defined(CONFIG_RTSP_LINUX)
#include "SvcRtspLink.h"
#else
#include "SvcRtsp.h"
#endif
#endif
#include "SvcStillMux.h"
#include "SvcPlat.h"
#if defined(CONFIG_ICAM_ENCRYPTION)
#include "SvcEncrypt.h"
#endif
#include "SvcRecMain.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "platform.h"
#include "custom.h"
#include "app_helper.h"
#include "system/metadata.h"
#include "record/imu_record.h"
#include "record/gnss_record.h"
#include "SvcDrawDateTime.h"
#include "SvcDrawLogo.h"
#include "SvcResCfg.h"
#include "cardv_modules/modules/dqa_test_script/dqa_test_script.h"
#endif

#define LOG_RCM         "RCM"

#define RCM_DBG_PRN     0
static inline void RCM_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if RCM_DBG_PRN
    SvcLog_DBG(LOG_RCM, pFormat, Arg1, Arg2);
#else
    ULONG Temp = 0U;
    AmbaMisra_TypeCast(&Temp, pFormat);
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
#endif
}

#define RCM_NULL_SRC                (0xFFFFFFFFU)
#define RCM_FREE_SPACE_RESERVED     (2U)
#define RCM_PIV_SPACE_RESERVED      (0x2000000U)     /* reserved for piv */
#define RCM_EOS_TMO                 (5000U)

/* control flag */
#define RCM_FLAG_NULL_WRITE         (0x00000001U)
#define RCM_FLAG_AUDIO_EXIST        (0x00000002U)
#if defined(CONFIG_ICAM_AUDIO_USED)
#define RCM_FLAG_AUDIO_START        (0x00000004U)
#endif
#define RCM_FLAG_VIDEO_START_PREP   (0x00000010U)
#define RCM_FLAG_QPCTRL_EXIST       (0x00000020U)
#define RCM_FLAG_BLEND_EXIST        (0x00000040U)
#define RCM_FLAG_RECMON_EXIST       (0x00000080U)
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
#define RCM_FLAG_RECMV_EXIST        (0x00000100U)
#endif
#define RCM_FLAG_BITSMTX_EXIST      (0x00000200U)
#define RCM_FLAG_FWK_LOADED         (0x10000000U)

#define RCM_BITSCALC_OP_ADD         (0x00000001U)
#define RCM_BITSCALC_OP_MINUS       (0x00000002U)
#define RCM_BITSCALC_OP_GET         (0x00000003U)


typedef struct {
    AMBA_REC_MASTER_s  Rma;
    AMBA_REC_SRC_s     RscArr[CONFIG_ICAM_MAX_RSC_PER_STRM];
    AMBA_REC_BOX_s     RbxArr[CONFIG_ICAM_MAX_RBX_PER_STRM];
    AMBA_REC_DST_s     RdtArr[CONFIG_ICAM_MAX_RDT_PER_STRM];
} REC_MAIN_FWK_s;

typedef struct {
    #define REC_MON_TYPE_FSTATUS       (0x00000001U)
    UINT32                 Type;

    #define REC_MON_DATA_SIZE          (0x0000003CU)
    UINT8                  Data[REC_MON_DATA_SIZE];
} GNU_MIN_PADDING REC_MON_QUE_s;

typedef struct {
    UINT32                 NumStrm;
    SVC_REC_FWK_CFG_s      UsrCfg[CONFIG_ICAM_MAX_REC_STRM];

    UINT32                 RcmFlags;
    REC_MAIN_FWK_s         RecFwk[CONFIG_ICAM_MAX_REC_STRM];
    UINT32                 RscIdxMap[CONFIG_ICAM_MAX_REC_STRM][AMBA_RSC_TYPE_NUM];

    UINT32                 StartBits;
    AMBA_KAL_EVENT_FLAG_t  StartFlag;

    UINT32                 NumAEnc;

    UINT32                 QpCtrlPriority;
    UINT32                 QpCtrlCpuBits;

    UINT32                 RecBlendPriority;
    UINT32                 RecBlendCpuBits;
    UINT32                 BlendBits;

    UINT32                 DataBits;
    UINT16                 DataGCtrl[CONFIG_ICAM_MAX_REC_STRM];
    UINT64                 DataGSync[CONFIG_ICAM_MAX_REC_STRM];
    UINT64                 DataGCnt[CONFIG_ICAM_MAX_REC_STRM];
    UINT64                 DataGDiv[CONFIG_ICAM_MAX_REC_STRM];

    UINT64                 TLapseCapTime[CONFIG_ICAM_MAX_REC_STRM];

    /* record monitor task */
    #define REC_MON_FLAG_IDLE       (0x00000001U)
    #define REC_MON_FLAG_ENABLE     (0x10000000U)

    #define REC_MON_QUE_LEN         (0x00000040U)

    SVC_TASK_CTRL_s        RecMonTask;
    AMBA_KAL_EVENT_FLAG_t  RecMonFlag;
    PFN_REC_MON_NOTIFY     pfnRecMonNotify;
    AMBA_KAL_MSG_QUEUE_t   RecMonQueId;
    REC_MON_QUE_s          RecMonQue[REC_MON_QUE_LEN];
    REC_EVENT_NOTIFY       RecEvtCb[CONFIG_ICAM_MAX_REC_STRM][SVC_RCM_EVT_NUM];

    /* video thumbnail callback */
    PFN_REC_CAP_THM        pfnCapThm;

    /* total size of the bitstream datas haven't been copied to RecBox or RecDest */
    UINT32                 BitsDataSize[CONFIG_ICAM_MAX_REC_STRM][AMBA_RSC_TYPE_NUM];
    AMBA_KAL_MUTEX_t       BitsDataMutex;

    /* bitstream data comparison task */
    SVC_DATA_CMP_HDLR_s    BitsCmpHdlr;
    UINT32                 BitsCmpStrmBits;
    UINT32                 BitsCmpPriority;
    UINT32                 BitsCmpCpuBits;

#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
    /* record MV task */
    #define REC_MV_QUE_LEN          (2U * MAX_MV_NUM_PER_STRM * (UINT32)CONFIG_ICAM_MAX_REC_STRM)

    SVC_TASK_CTRL_s        RecMVTask;
    AMBA_KAL_MSG_QUEUE_t   RecMVQueId;
    AMBA_DSP_ENC_MV_RDY_s  RecMVQue[REC_MV_QUE_LEN];
    PFN_REC_MV_NOTIFY      pfnRecMVNotify;
#endif
} REC_MAIN_CTRL_s;

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define RDT_FILE_BUFINFO_NUM    ((UINT32)CONFIG_AMBA_REC_RBX_MP4_BUF_NUM)

typedef struct {
    UINT32   IsValid;
    ULONG    Addr;
    UINT32   Size;
} AMBA_RDT_BUFINFO_s;

typedef struct {
    char                     FileName[AMBA_REC_MAX_FILE_NAME_LEN];
    AMBA_VFS_FILE_s          File;
    UINT32                   Status;
    UINT32                   UnsyncSize;
    UINT32                   FileSyncSize;
    UINT32                   StoreDisable;
    AMBA_RDT_STATIS_s        Statis;

    UINT32                   StorageCheck;
    UINT32                   IsBufFull;
    ULONG                    BufInfoLimit;
    AMBA_RDT_BUFINFO_s       BufInfo[RDT_FILE_BUFINFO_NUM];
    UINT32                   BufInfoIdx;
    AMBA_RDT_FSTATUS_INFO_s  FInfo;
} AMBA_RDT_FILE_PRIV_s;
#endif


static REC_MAIN_CTRL_s  g_RcmCtrl GNU_SECTION_NOZEROINIT;

static UINT32 BitsDataSizeCalc(UINT32 StreamId, UINT32 RscType, UINT32 Size, UINT32 Operation)
{
    UINT32 Rval = 0U, Err;

    Err = AmbaKAL_MutexTake(&(g_RcmCtrl.BitsDataMutex), 5000U);
    if (KAL_ERR_NONE != Err) {
        SvcLog_NG(LOG_RCM, "AmbaKAL_MutexTake SvcRcmBitsCalcMtx failed %u", Err, 0U);
    }

    if (KAL_ERR_NONE == Err) {
        switch (Operation) {
            case RCM_BITSCALC_OP_ADD:
                g_RcmCtrl.BitsDataSize[StreamId][RscType] += Size;
                break;
            case RCM_BITSCALC_OP_MINUS:
                if (Size <= g_RcmCtrl.BitsDataSize[StreamId][RscType]) {
                    g_RcmCtrl.BitsDataSize[StreamId][RscType] -= Size;
                } else {
                    g_RcmCtrl.BitsDataSize[StreamId][RscType] = 0U;
                }
                break;
            case RCM_BITSCALC_OP_GET:
                /* nothing */
                break;
            default:
                SvcLog_NG(LOG_RCM, "unknown BITSCALC OP %u", Operation, 0U);
                break;
        }

        Rval = g_RcmCtrl.BitsDataSize[StreamId][RscType];
    }

    if (KAL_ERR_NONE == Err) {
        Err = AmbaKAL_MutexGive(&(g_RcmCtrl.BitsDataMutex));
        if (KAL_ERR_NONE != Err) {
            SvcLog_NG(LOG_RCM, "AmbaKAL_MutexGive SvcRcmBitsCalcMtx failed %u", Err, 0U);
        }
    }

    return Rval;
}

static UINT32 ProcBitsSizeUpdate(UINT32 StreamId, UINT32 RscType, UINT32 Size)
{
    UINT32 RSize;

    RSize = BitsDataSizeCalc(StreamId, RscType, Size, RCM_BITSCALC_OP_MINUS);

    return RSize;
}

#if defined(CONFIG_ICAM_ENCRYPTION)
static ULONG SearchStartOffset(ULONG BufBase, ULONG BufSize, ULONG DataAddr, ULONG DataSize, UINT32 IsHevc, UINT32 FrameType)
{
    ULONG                i = 0U, j, CmpCnt, Offset = 0xFFFFFFFFU, Addr;
    const UINT8          *pVal, *pStartCode;
    static const UINT8   IdrAvc[5U]    = {0x00U, 0x00U, 0x00U, 0x01U, 0x65U};
    static const UINT8   IdrHevc[4U]   = {0x00U, 0x00U, 0x01U, 0x26U};
    static const UINT8   SliceAvc[5U]  = {0x00U, 0x00U, 0x00U, 0x01U, 0x61U};
    static const UINT8   SliceHevc[4U] = {0x00U, 0x00U, 0x01U, 0x02U};

    if (IsHevc == 0U) {
        if (FrameType == PIC_FRAME_IDR) {
            CmpCnt     = sizeof(IdrAvc);
            pStartCode = IdrAvc;
        } else {
            CmpCnt     = sizeof(SliceAvc);
            pStartCode = SliceAvc;
        }
    } else {
        if (FrameType == PIC_FRAME_IDR) {
            CmpCnt     = sizeof(IdrHevc);
            pStartCode = IdrHevc;
        } else {
            CmpCnt     = sizeof(SliceHevc);
            pStartCode = SliceHevc;
        }
    }

    while ((i + CmpCnt) <= DataSize) {
        for (j = 0U; j < CmpCnt; j++) {
            Addr = (DataAddr + i) + j;
            if (Addr >= (BufBase + BufSize)) {
                Addr -= BufSize;
            }
            AmbaMisra_TypeCast(&pVal, &Addr);

            if (*pVal == pStartCode[j]) {
                /**/
            } else {
                break;
            }
        }

        if (j == CmpCnt) {
            Offset = (i + CmpCnt) - 1U;
            break;
        }

        i++;
    }

    return Offset;
}

static void BitsDataEncrypt(AMBA_RMAC_ENCRYPT_s *pInfo)
{
    UINT32                Err, Rval = SVC_OK;
    SVC_ENCRYPT_PROCESS_s Process;

    AmbaSvcWrap_MisraMemset(&Process, 0, sizeof(Process));

    if ((pInfo->RscType == AMBA_RSC_TYPE_VIDEO)) {
        UINT32          IsHevc = 0U;
        ULONG           Offset, AlignSize, MinSize;
        SVC_ENC_INFO_s  EncInfo;

        SvcEnc_InfoGet(&EncInfo);

        #if defined(AMBA_DSP_ENC_CODING_TYPE_H265)
        if (EncInfo.pStrmCfg[pInfo->StreamId].CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
            IsHevc = 1U;
        }
        #else
        AmbaMisra_TouchUnused(&IsHevc);
        #endif

        /* we only encrypt NAL type IDR */
        Offset = SearchStartOffset(pInfo->SrcBufBase,
                                   pInfo->SrcBufSize,
                                   pInfo->SrcDataAddr,
                                   pInfo->SrcDataSize,
                                   IsHevc,
                                   pInfo->FrameType);
        if (Offset == 0xFFFFFFFFU) {
            SvcLog_NG(LOG_RCM, "SearchStartOffset failed %u", pInfo->FrameType, 0U);
            Rval = SVC_NG;
        }

        #define VIDEO_ENCRYPT_OFFSET     (128U)   /* skip first 128 Bytes for muxer parsing */
        #define VIDEO_ENCRYPT_SIZE       (128U)   /* encryption size */
        #define VIDEO_H264_ALIGN         (32U)    /* muxer will add zero padding for frame alignment, SVC_H264_ALIGN_SIZE */
        #define VIDEO_H265_ALIGN         (1U)     /* muxer will add zero padding for frame alignment */

        Process.ProcType       = SVC_ENCRYPT_PROC_ENC;
        Process.InputBufBase   = pInfo->SrcBufBase;
        Process.InputBufSize   = pInfo->SrcBufSize;
        Process.InputAddr      = pInfo->SrcDataAddr;
        Process.InputTotalSize = pInfo->SrcDataSize;
        Process.InputOffset    = Offset + VIDEO_ENCRYPT_OFFSET;
        Process.InputProcSize  = VIDEO_ENCRYPT_SIZE;

        /* let the output data overwrite the input data */
        Process.OutputBufBase  = Process.InputBufBase;
        Process.OutputBufSize  = Process.InputBufSize;
        Process.OutputAddr     = Process.InputAddr;

        if (IsHevc == 1U) {
            AlignSize = VIDEO_H265_ALIGN;
        } else {
            AlignSize = VIDEO_H264_ALIGN;
        }

        MinSize = ((Process.InputOffset + Process.InputProcSize + (AlignSize - 1U)) / AlignSize) * AlignSize;

        /* only encrypt the frame if its size is enough*/
        if (MinSize >= Process.InputTotalSize) {
            Rval = SVC_NG;
        }

        if (Rval == SVC_OK) {
            Err = SvcEncrypt_Process(&Process);
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcEncrypt_Process failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }
    } else {
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        pInfo->DstBufBase  = Process.OutputBufBase;
        pInfo->DstBufSize  = Process.OutputBufSize;
        pInfo->DstDataAddr = Process.OutputAddr;
        pInfo->DstDataSize = Process.OutputSize;
    } else {
        pInfo->DstBufBase  = pInfo->SrcBufBase;
        pInfo->DstBufSize  = pInfo->SrcBufSize;
        pInfo->DstDataAddr = pInfo->SrcDataAddr;
        pInfo->DstDataSize = pInfo->SrcDataSize;
    }

    if (Rval == SVC_OK) {
        ULONG  AlignStart, AlignSize;

        if ((pInfo->DstDataAddr + pInfo->DstDataSize) > (pInfo->DstBufBase + pInfo->DstBufSize)) {
            AlignStart = pInfo->DstDataAddr & AMBA_CACHE_LINE_MASK;
            AlignSize  = ((((pInfo->DstBufBase + pInfo->DstBufSize) - AlignStart) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK);
            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcPlat_CacheClean failed %u", Err, 0U);
            }

            AlignSize  = (((pInfo->DstDataAddr + pInfo->DstDataSize) - (pInfo->DstBufBase + pInfo->DstBufSize)) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK;
            Err = SvcPlat_CacheClean(pInfo->DstBufBase, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcPlat_CacheClean failed %u", Err, 0U);
            }
        } else {
            AlignStart = pInfo->DstDataAddr & AMBA_CACHE_LINE_MASK;
            AlignSize  = (((pInfo->DstDataAddr + pInfo->DstDataSize) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK) - AlignStart;
            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcPlat_CacheClean failed %u", Err, 0U);
            }
        }
    }
}
#endif

static UINT32 EncStartCB(const void *pEventData)
{
    const AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s  *pEvt;

    AmbaMisra_TypeCast(&pEvt, &pEventData);

    SvcLog_OK(LOG_RCM, "@@ strm(%u) start!!!", pEvt->StreamId, 0U);
    return OK;
}

static UINT32 EncStopCB(const void *pEventData)
{
    const AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s  *pEvt;

    AmbaMisra_TypeCast(&pEvt, &pEventData);

    SvcLog_OK(LOG_RCM, "@@ strm(%u) stop!!!", pEvt->StreamId, 0U);
    return OK;
}

static void BitsCmpSendData(const AMBA_DSP_ENC_PIC_RDY_s *pEvt)
{
    UINT32                  Err, SrcIndex;
    SVC_DATA_CMP_SEND_s     BitsCmp;
    const UINT64            *pPts;
    const REC_MAIN_FWK_s    *pFwk;
    const AMBA_REC_SRC_s    *pRsc;

    pFwk     = &(g_RcmCtrl.RecFwk[pEvt->StreamId]);
    SrcIndex = g_RcmCtrl.RscIdxMap[pEvt->StreamId][AMBA_RSC_TYPE_VIDEO];
    pRsc     = &(pFwk->RscArr[SrcIndex]);

    BitsCmp.CmpId        = (UINT8)pEvt->StreamId;
    BitsCmp.NumData      = 2U;

    pPts = &(pEvt->Pts);
    AmbaMisra_TypeCast(&(BitsCmp.Data[0].DataAddr), &pPts);
    BitsCmp.Data[0].DataSize = (UINT32)sizeof(UINT64);
    AmbaMisra_TypeCast(&(BitsCmp.Data[0].BufBase), &pPts);
    BitsCmp.Data[0].BufSize  = (UINT32)sizeof(UINT64);

    AmbaMisra_TypeCast(&(BitsCmp.Data[1].DataAddr), &(pEvt->StartAddr));
    BitsCmp.Data[1].DataSize = pEvt->PicSize;
    BitsCmp.Data[1].BufBase  = pRsc->UsrCfg.BsBufBase;
    BitsCmp.Data[1].BufSize  = pRsc->UsrCfg.BsBufSize;

    Err = SvcDataCmp_DataSend(&(g_RcmCtrl.BitsCmpHdlr), &BitsCmp);
    if (Err != SVC_OK) {
        SvcLog_NG(LOG_RCM, "SvcDataCmp_DataSend failed %u", Err, 0U);
    }
}

static UINT32 EncDataRdyCB(const void *pEventData)
{
    UINT32                   Rval, SrcIndex, Bits = 0x01U, ActualFlags = 0U, RSize, Err;
    AMBA_DSP_ENC_PIC_RDY_s   *pEvt;
    const REC_MAIN_FWK_s     *pFwk;
    const AMBA_REC_MASTER_s  *pRma;
    const AMBA_REC_SRC_s     *pRsc;

    AmbaMisra_TypeCast(&pEvt, &pEventData);
    pFwk = &(g_RcmCtrl.RecFwk[pEvt->StreamId]);

    SrcIndex = g_RcmCtrl.RscIdxMap[pEvt->StreamId][AMBA_RSC_TYPE_VIDEO];

    if (pEvt->PicSize != AMBA_DSP_ENC_END_MARK) {
        /* avoid cache write back to memory */
        {
            ULONG  AlignStart, AlignSize;

            pRsc = &(pFwk->RscArr[SrcIndex]);

            if ((pEvt->StartAddr + pEvt->PicSize) > (pRsc->UsrCfg.BsBufBase + pRsc->UsrCfg.BsBufSize)) {
                AlignStart = pEvt->StartAddr & AMBA_CACHE_LINE_MASK;
                AlignSize  = (pRsc->UsrCfg.BsBufBase + pRsc->UsrCfg.BsBufSize) - AlignStart;
                Err = SvcPlat_CacheInvalidate(AlignStart, AlignSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(LOG_RCM, "SvcPlat_CacheInvalidate failed %u", Err, 0U);
                }

                AlignSize  = (((pEvt->StartAddr + pEvt->PicSize) - (pRsc->UsrCfg.BsBufBase + pRsc->UsrCfg.BsBufSize)) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK;
                Err = SvcPlat_CacheInvalidate(pRsc->UsrCfg.BsBufBase, AlignSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(LOG_RCM, "SvcPlat_CacheInvalidate failed %u", Err, 0U);
                }
            } else {
                AlignStart = pEvt->StartAddr & AMBA_CACHE_LINE_MASK;
                AlignSize  = (((pEvt->StartAddr + pEvt->PicSize) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK) - AlignStart;
                Err = SvcPlat_CacheInvalidate(AlignStart, AlignSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(LOG_RCM, "SvcPlat_CacheInvalidate failed %u", Err, 0U);
                }
            }
        }

        /* update the total size of bitsdata that haven't been copied */
        RSize = BitsDataSizeCalc(pEvt->StreamId, AMBA_RSC_TYPE_VIDEO, pEvt->PicSize, RCM_BITSCALC_OP_ADD);
        if (RSize == 0U) {
            /* nothing */
        }

        /* bitstream data comparison */
        if (0U < CheckBits(g_RcmCtrl.BitsCmpStrmBits, (Bits << pEvt->StreamId))) {
            BitsCmpSendData(pEvt);
        }
    }

    /* Xcode doesn't give capture time info */
    if (g_RcmCtrl.UsrCfg[pEvt->StreamId].IsXcode == 1U) {
        UINT32  Time;

        pEvt->CaptureTimeStamp = 0U;
        if (AmbaSYS_GetOrcTimer(&Time) == SYS_ERR_NONE) {
            pEvt->CaptureTimeStamp = Time;
        }
    }

#if defined(CONFIG_ICAM_TIMELAPSE_USED)
    /* time-lapse doesn't give capture time info */
    if (g_RcmCtrl.UsrCfg[pEvt->StreamId].IsTimeLapse == 1U) {
        if ((pEvt->TileIdx == (pEvt->NumTile - 1U)) && (pEvt->SliceIdx == (pEvt->NumSlice - 1U))) {
            SvcTimeLapse_EncOneFrameNotify(pEvt->StreamId);
        }
        if ((pEvt->TileIdx == 0U) && (pEvt->SliceIdx == 0U)) {
            pRsc = &(pFwk->RscArr[SrcIndex]);
            g_RcmCtrl.TLapseCapTime[pEvt->StreamId] += (12288000ULL * (UINT64)pRsc->UsrCfg.NumUnitsInTick) / (UINT64)pRsc->UsrCfg.TimeScale;
        }
        pEvt->CaptureTimeStamp = g_RcmCtrl.TLapseCapTime[pEvt->StreamId];
    }
#endif

    if ((0U < CheckBits(g_RcmCtrl.StartBits, (Bits << pEvt->StreamId))) &&
        (SrcIndex != RCM_NULL_SRC)) {
        pRma = &(pFwk->Rma);
        pRsc = &(pFwk->RscArr[SrcIndex]);

        Rval = pRsc->pfnEnque(pRsc, pEvt);
        if (Rval == SVC_OK) {
            pRma->pfnNotify(pRma, pRsc->SrcBit);
        }
    }

    if (0U != CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_VIDEO_START_PREP)) {
        if (KAL_ERR_NONE != AmbaKAL_EventFlagSet(&(g_RcmCtrl.StartFlag), Bits << pEvt->StreamId)) {
            SvcLog_NG(LOG_RCM, "fail to set event flag", 0U, 0U);
        }

        if (g_RcmCtrl.DataGSync[pEvt->StreamId] == 0U) {
            g_RcmCtrl.DataGSync[pEvt->StreamId] = pEvt->CaptureTimeStamp;
        }

        Err = AmbaKAL_EventFlagGet(&(g_RcmCtrl.StartFlag), g_RcmCtrl.StartBits, AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_NO_WAIT);
        if (KAL_ERR_NONE != Err) {
            /* nothing */
        }
        if (g_RcmCtrl.StartBits == CheckBits(g_RcmCtrl.StartBits, ActualFlags)) {
            g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_VIDEO_START_PREP);
            SvcLog_OK(LOG_RCM, "record start successfully(%u)", g_RcmCtrl.StartBits, 0U);
        }
    }

    RCM_DBG("[V%u] data_ready, size(%X)!!!", pEvt->StreamId, pEvt->PicSize);
    RCM_DBG("[V%u] data_ready, tick(%X)!!!", pEvt->StreamId, (UINT32)pEvt->CaptureTimeStamp);
    return OK;
}

static UINT32 EncMVDataCB(const void *pEventData)
{
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
    AMBA_DSP_ENC_MV_RDY_s  MV;
    AMBA_DSP_ENC_MV_RDY_s  *pMV;
    UINT32                 Err;

    AmbaMisra_TypeCast(&pMV, &pEventData);
    if (AmbaKAL_MsgQueueSend(&(g_RcmCtrl.RecMVQueId), pMV, AMBA_KAL_NO_WAIT) != SVC_OK) {
        /* drop first msg and send again */
        Err = AmbaKAL_MsgQueueReceive(&(g_RcmCtrl.RecMVQueId), &MV, AMBA_KAL_NO_WAIT);
        if (KAL_ERR_NONE != Err) {
            /* nothing */
        }
        Err = AmbaKAL_MsgQueueSend(&(g_RcmCtrl.RecMVQueId), pMV, AMBA_KAL_NO_WAIT);
        if (KAL_ERR_NONE != Err) {
            /* nothing */
        }
    }

    RCM_DBG("[V%u] MV_ready, MvBufType(%X)!!!", pMV->StreamId, pMV->MvBufType);
    RCM_DBG("    CapPts/CapSequence(%u/%u)!!!", (UINT32)pMV->CapPts, (UINT32)pMV->CapSequence);
    RCM_DBG("    MvBufAddr/MvBufPitch(0x%X/%u)!!!", pMV->MvBufAddr, pMV->MvBufPitch);
    RCM_DBG("    MvBufWidth/MvBufHeight(%u/%u)!!!", pMV->MvBufWidth, pMV->MvBufHeight);
#else
    AmbaMisra_TouchUnused(&pEventData);
#endif
    return OK;
}

#ifdef CONFIG_ICAM_AUDIO_USED
static UINT32 AEncDataRdyCB(const void *pEventData)
{
    UINT32                   i, Rval, SrcIndex, Bits = 0x01U, RSize, AEncBit;
    AMBA_AENC_AUDIO_DESC_s   *pEvt;
    const REC_MAIN_FWK_s     *pFwk;
    const AMBA_REC_MASTER_s  *pRma;
    const AMBA_REC_SRC_s     *pRsc;
    const SVC_REC_FWK_CFG_s  *pCfg;

    AmbaMisra_TypeCast(&pEvt, &pEventData);

    AEncBit = AmbaAEncFlow_CheckAencId(pEvt->pHdlr);
    if (AEncBit <= (UINT32)CONFIG_AENC_NUM) {
        AEncBit = (Bits << AEncBit);
    } else {
        AEncBit = 0U;
    }

    for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
        pCfg = &(g_RcmCtrl.UsrCfg[i]);
        pFwk = &(g_RcmCtrl.RecFwk[i]);

        if (0U < CheckBits(pCfg->AEncBits, AEncBit)) {
            SrcIndex = g_RcmCtrl.RscIdxMap[i][AMBA_RSC_TYPE_AUDIO];
            if ((0U < CheckBits(g_RcmCtrl.StartBits, (Bits << i))) &&
                (SrcIndex != RCM_NULL_SRC)) {
                pRma = &(pFwk->Rma);
                pRsc = &(pFwk->RscArr[SrcIndex]);

                /* update the total size of bitsdata that haven't been copied */
                if (pEvt->Eos == 0U) {
                    RSize = BitsDataSizeCalc(i, AMBA_RSC_TYPE_AUDIO, pEvt->DataSize, RCM_BITSCALC_OP_ADD);
                    if (RSize == 0U) {
                        /* nothing */
                    }
                }

                Rval = pRsc->pfnEnque(pRsc, pEvt);
                if (Rval == SVC_OK) {
                    pRma->pfnNotify(pRma, pRsc->SrcBit);
                }
            }
        }
    }

    RCM_DBG("[A] audio data_ready, eos/size(%X/%X)!!!", pEvt->Eos, pEvt->DataSize);
    RCM_DBG("[A] audio data_ready, eos/tick(%X/%X)!!!", pEvt->Eos, (UINT32)pEvt->AudioTicks);
    return OK;
}

static void AinCtrl(UINT32 Chan, UINT32 Op)
{
    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbAinCtrl != NULL)) {
        g_pPlatCbEntry->pCbAinCtrl(Chan, Op);
    }
}
#endif

#if defined(CONFIG_ICAM_DATA_GATHER_USED)
static void DataGatherCB(UINT16 CtrlId, AMBA_DATG_s *pDataG)
{
    UINT32                  i, Rval, SrcIndex, Bits = 0x01U, RSize;
    const REC_MAIN_FWK_s    *pFwk;
    const AMBA_REC_MASTER_s  *pRma;
    const AMBA_REC_SRC_s     *pRsc;

    for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
        if (g_RcmCtrl.DataGCtrl[i] == CtrlId) {
            pFwk = &(g_RcmCtrl.RecFwk[i]);

            SrcIndex = g_RcmCtrl.RscIdxMap[i][AMBA_RSC_TYPE_DATA];
            if ((0U < CheckBits(g_RcmCtrl.StartBits, (Bits << i))) &&
                (SrcIndex != RCM_NULL_SRC)) {
                pRma = &(pFwk->Rma);
                pRsc = &(pFwk->RscArr[SrcIndex]);

                if ((g_RcmCtrl.DataGCnt[i] % g_RcmCtrl.DataGDiv[i]) == 0U) {
                    /* update the total size of bitsdata that haven't been copied */
                    RSize = BitsDataSizeCalc(i, AMBA_RSC_TYPE_DATA, 1U, RCM_BITSCALC_OP_ADD);
                    if (RSize == 0U) {
                        /* nothing */
                    }

                    Rval = pRsc->pfnEnque(pRsc, pDataG);
                    if (Rval == SVC_OK) {
                        pRma->pfnNotify(pRma, pRsc->SrcBit);
                    }
                }

                g_RcmCtrl.DataGCnt[i]++;
            }
        }
    }
}

static void DataGatherOpen(UINT32 StartBits)
{
    UINT16    StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];
    UINT32    i, NumStrm, ActualFlags;

    if (0U < StartBits) {
        if (AmbaKAL_EventFlagGet(&(g_RcmCtrl.StartFlag),
                                   StartBits,
                                   AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_NONE,
                                   &ActualFlags,
                                   5000U) == SVC_OK) {

            SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StartBits, &NumStrm, StrmIdxArr);
            for (i = 0U; i < NumStrm; i++) {
                g_RcmCtrl.DataGCnt[StrmIdxArr[i]] = 0U;
                SvcDataGather_StartFetch(g_RcmCtrl.DataGCtrl[StrmIdxArr[i]],
                                         TICK_TYPE_AUDIO,
                                         StrmIdxArr[i],
                                         g_RcmCtrl.DataGSync[StrmIdxArr[i]]);
            }
        } else {
            SvcLog_DBG(LOG_RCM, "fail to wait record start(0x%X)", StartBits, 0U);
        }
    }
}

static void DataGatherClose(UINT32 StopBits)
{
    UINT16                   StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];
    UINT32                   i, c, NumStrm, SrcIndex;
    const SVC_REC_FWK_CFG_s  *pCfg;
    const REC_MAIN_FWK_s     *pFwk;
    const AMBA_REC_MASTER_s  *pRma;
    const AMBA_REC_SRC_s     *pRsc;

    if (0U < StopBits) {
        SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StopBits, &NumStrm, StrmIdxArr);

        for (i = 0U; i < NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[StrmIdxArr[i]]);
            pFwk = &(g_RcmCtrl.RecFwk[StrmIdxArr[i]]);

            for (c = 0U; c < pCfg->NumRsc; c++) {
                if (pCfg->RscCfgArr[c]->SrcType == AMBA_RSC_TYPE_DATA) {
                    AmbaRscData_Control(&(pFwk->RscArr[c]), AMBA_RSC_DATA_DUMMY_EOS, NULL);
                }
            }

            SvcDataGather_StopFetch(g_RcmCtrl.DataGCtrl[StrmIdxArr[i]]);

            SrcIndex = g_RcmCtrl.RscIdxMap[StrmIdxArr[i]][AMBA_RSC_TYPE_DATA];
            if (SrcIndex != RCM_NULL_SRC) {
                pRma = &(pFwk->Rma);
                pRsc = &(pFwk->RscArr[SrcIndex]);

                pRma->pfnNotify(pRma, pRsc->SrcBit);
            }
        }
    }
}
#endif

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static linux_time_s video_split_time[CONFIG_ICAM_MAX_REC_STRM];
void UpdateVideoStartTime(int streamId)
{
    extern void SvcDCF_DashcamUpdateVideoSplitTime(UINT32 streamid, AMBA_RTC_DATE_TIME_s time);
    linux_time_s linux_time;
    AMBA_RTC_DATE_TIME_s tmp;

    if (streamId >= CONFIG_ICAM_MAX_REC_STRM) {
        return;
    }
    app_helper.get_linux_time(&linux_time);
    video_split_time[streamId] = linux_time;
    tmp.Year = linux_time.utc_time.year;
    tmp.Month = linux_time.utc_time.month;
    tmp.Day = linux_time.utc_time.day;
    tmp.Hour = linux_time.utc_time.hour;
    tmp.Minute = linux_time.utc_time.minute;
    tmp.Second = linux_time.utc_time.second;
    SvcDCF_DashcamUpdateVideoSplitTime(streamId, tmp);
    AmbaPrint_PrintUInt5("update time, stream%d %02d:%02d:%02d.%03d", streamId, linux_time.utc_time.hour, linux_time.utc_time.minute, linux_time.utc_time.second, linux_time.usec / 1000);
}

static int currentStreamId = 0;
void UpdateVideoMetadataStreamId(int streamId)
{
    currentStreamId = streamId;
}

void UpdateVideoMetadata(unsigned char *data, unsigned int *outLen)
{
    //UINT8 data[512] = {0};
    int index = 8, len = 0;
    UINT32 crc32 = 0;
    metadata_s *metadata = NULL;
    linux_time_s linux_time;
    device_info_s *device_info;

    if (currentStreamId % 2 != 0) {
        *outLen = 0;
        return;
    }
    linux_time = video_split_time[currentStreamId];
    AmbaPrint_PrintUInt5("UpdateVideoMetadata(%d)---%02d:%02d:%02d.%03d", currentStreamId, linux_time.utc_time.hour, linux_time.utc_time.minute, linux_time.utc_time.second, linux_time.usec / 1000);
    device_info = app_helper.get_device_info();
    len = 8;
    data[index] = len;
    index += 1;
    data[index++] = (UINT8)((linux_time.sec & 0xFF000000U) >> 24);
    data[index++] = (UINT8)((linux_time.sec & 0xFF0000U) >> 16);
    data[index++] = (UINT8)((linux_time.sec & 0xFF00U) >> 8);
    data[index++] = (UINT8)(linux_time.sec & 0xFFU);    
    linux_time.usec /= 1000;
    data[index++] = (UINT8)((linux_time.usec & 0xFF000000U) >> 24);
    data[index++] = (UINT8)((linux_time.usec & 0xFF0000U) >> 16);
    data[index++] = (UINT8)((linux_time.usec & 0xFF00U) >> 8);
    data[index++] = (UINT8)(linux_time.usec & 0xFFU);

    metadata_get(&metadata);
    len = AmbaUtility_StringLength((char *)metadata->SerialNumber);
    data[index] = len;
    index += 1;
    if (len > 0) {
        AmbaWrap_memcpy(data + index, metadata->SerialNumber, len);
        index += len;
    }

    len = AmbaUtility_StringLength(device_info->sw_version);
    data[index] = len;
    index += 1;
    AmbaWrap_memcpy(data + index, device_info->sw_version, len);
    index += len;

    len = AmbaUtility_StringLength(device_info->build_time);
    data[index] = len;                        
    index += 1;
    AmbaWrap_memcpy(data + index, device_info->build_time, len);
    index += len;

    crc32 = AmbaUtility_Crc32(data + 8, index - 8);
    data[index++] = (UINT8)((crc32 & 0xFF000000U) >> 24);
    data[index++] = (UINT8)((crc32 & 0xFF0000U) >> 16);
    data[index++] = (UINT8)((crc32 & 0xFF00U) >> 8);
    data[index++] = (UINT8)(crc32 & 0xFFU);

    data[0] = (UINT8)((index & 0xFF000000U) >> 24);
    data[1] = (UINT8)((index & 0xFF0000U) >> 16);
    data[2] = (UINT8)((index & 0xFF00U) >> 8);
    data[3] = (UINT8)(index & 0xFFU);

    data[4] = 'C';
    data[5] = 'U';
    data[6] = 'S';
    data[7] = 'T';

    *outLen = index;
}
#endif

static UINT32 FetchFileName(char *pNameBuf, UINT32 MaxNameLen, UINT32 StreamID)
{
    char                      StgDrive = g_RcmCtrl.UsrCfg[StreamID].Drive;
    UINT32                    Rval;
    SVC_STG_MONI_DRIVE_INFO_s DriveInfo;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    Rval = SvcStgMonitor_GetDriveInfo(StgDrive, &DriveInfo);
    if (Rval == SVC_OK) {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        UINT32                    VinID;
        Rval = SvcResCfg_GetVinIDOfRecIdx(StreamID, &VinID);
        if (Rval == SVC_OK) {
        Rval = AmbaDCF_CreateFileName(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, pNameBuf, VinID, StreamID);
        }
#else
        Rval = AmbaDCF_CreateFileName(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, pNameBuf, 0U, StreamID);
#endif
    }
    
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->PivOnOff == OPTION_ON) {
#if defined(CONFIG_ICAM_STLCAP_USED)
        if (Rval == SVC_OK
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            && (StreamID % 2 == 0)
#endif
            ) {
            UINT32 i, FileNameLen;
            char   VidThmFileName[MAX_FILE_NAME_LEN] = {0};
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            AmbaWrap_memset(VidThmFileName, 0, MAX_FILE_NAME_LEN);
            for (i = (MAX_FILE_NAME_LEN - 1U); i >= 2U; i --) {
                if (pNameBuf[i] == '\\') {
                    AmbaWrap_memcpy(VidThmFileName, pNameBuf, i + 1);
                    AmbaUtility_StringAppend(VidThmFileName, sizeof(VidThmFileName), DCF_THUMB_DIR_NAME);
                    AmbaUtility_StringAppend(VidThmFileName, sizeof(VidThmFileName), "\\");
                    AmbaWrap_memcpy(VidThmFileName + SvcWrap_strlen(VidThmFileName), pNameBuf + i + 1, SvcWrap_strlen(pNameBuf) - (i + 1) - 4);
                    AmbaUtility_StringAppend(VidThmFileName, sizeof(VidThmFileName), "_net.jpg");
                    FileNameLen = SvcWrap_strlen(VidThmFileName);
                    FileNameLen ++;
                    Rval = SvcStillMux_SetVidThmFileName((UINT8)StreamID, VidThmFileName, (UINT8)FileNameLen);
                    rec_dvr_update_thm(rec_dvr_stream_id_to_channel(StreamID), VidThmFileName);
                    SvcLog_OK(LOG_RCM, "set video thm for stream %u", StreamID, 0U);
                    AmbaPrint_PrintStr5(" %s", VidThmFileName, NULL, NULL, NULL, NULL);
                    break;
                }
            }

#else
            AmbaSvcWrap_MisraMemcpy(VidThmFileName, pNameBuf, MAX_FILE_NAME_LEN);
            for (i = (MAX_FILE_NAME_LEN - 1U); i >= 2U; i --) {
                if ((VidThmFileName[i] == '4') &&
                    (VidThmFileName[i - 1U] == 'P') &&
                    (VidThmFileName[i - 2U] == 'M')) {
                    VidThmFileName[i] = 'M';
                    VidThmFileName[i- 1U] = 'H';
                    VidThmFileName[i- 2U] = 'T';
                    FileNameLen = (UINT32)SvcWrap_strlen(VidThmFileName);
                    FileNameLen ++;
                    Rval = SvcStillMux_SetVidThmFileName((UINT8)StreamID, VidThmFileName, (UINT8)FileNameLen);
                    SvcLog_OK(LOG_RCM, "set video thm for stream %u", StreamID, 0);
                    AmbaPrint_PrintStr5(" %s", VidThmFileName, NULL, NULL, NULL, NULL);
                }
            }      
#endif
        }
#endif
    }

#if defined(CONFIG_ICAM_RAWENC_USED)
    if (Rval == SVC_OK) {
        extern void SvcRawEnc_SetStrmCapFileName(UINT32 StreamID, const char *pFileName, UINT32 Length);
        SvcRawEnc_SetStrmCapFileName(StreamID, pNameBuf, (UINT32)SvcWrap_strlen(pNameBuf));
    }
#endif
    AmbaPrint_PrintStr5("fetch %s", pNameBuf, NULL, NULL, NULL, NULL);

    AmbaMisra_TouchUnused(&MaxNameLen);
    return Rval;
}

static UINT32 FetchEventFileName(char *pNameBuf, UINT32 MaxNameLen, UINT32 StreamID)
{
    char                      StgDrive = g_RcmCtrl.UsrCfg[StreamID].Drive;
    UINT32                    Rval;
    SVC_STG_MONI_DRIVE_INFO_s DriveInfo;

    Rval = SvcStgMonitor_GetDriveInfo(StgDrive, &DriveInfo);
    if (Rval == SVC_OK) {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        UINT32                    VinID;
        Rval = SvcResCfg_GetVinIDOfRecIdx(StreamID, &VinID);
        if (Rval == SVC_OK) {
            Rval = AmbaDCF_CreateFileName(DriveInfo.DriveID, 1U, AMBA_DCF_FILE_TYPE_VIDEO, pNameBuf, VinID, StreamID);
        }
#else
        Rval = AmbaDCF_CreateFileName(DriveInfo.DriveID, 1U, AMBA_DCF_FILE_TYPE_VIDEO, pNameBuf, 0U, StreamID);
#endif
    }
    
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#if defined(CONFIG_ICAM_STLCAP_USED)
    if (Rval == SVC_OK
        && (StreamID % 2 == 0)) {
        UINT32 i, FileNameLen;
        char   VidThmFileName[MAX_FILE_NAME_LEN];
        AmbaWrap_memset(VidThmFileName, 0, MAX_FILE_NAME_LEN);
        for (i = (MAX_FILE_NAME_LEN - 1U); i >= 2U; i --) {
            if (pNameBuf[i] == '\\') {
                AmbaWrap_memcpy(VidThmFileName, pNameBuf, i + 1);
                AmbaUtility_StringAppend(VidThmFileName, sizeof(VidThmFileName), DCF_THUMB_DIR_NAME);
                AmbaUtility_StringAppend(VidThmFileName, sizeof(VidThmFileName), "\\");
                AmbaWrap_memcpy(VidThmFileName + SvcWrap_strlen(VidThmFileName), pNameBuf + i + 1, SvcWrap_strlen(pNameBuf) - (i + 1) - 4);
                AmbaUtility_StringAppend(VidThmFileName, sizeof(VidThmFileName), "_net.jpg");
                FileNameLen = SvcWrap_strlen(VidThmFileName);
                FileNameLen ++;
                Rval = SvcStillMux_SetVidThmFileName((UINT8)StreamID, VidThmFileName, (UINT8)FileNameLen);
                rec_dvr_update_thm(rec_dvr_stream_id_to_channel(StreamID), VidThmFileName);
                SvcLog_OK(LOG_RCM, "set sos thm for stream %u", StreamID, 0U);
                AmbaPrint_PrintStr5(" %s", VidThmFileName, NULL, NULL, NULL, NULL);
                break;
            }
        }
    }
#endif
#endif

    AmbaPrint_PrintStr5("fetch %s", pNameBuf, NULL, NULL, NULL, NULL);

    AmbaMisra_TouchUnused(&MaxNameLen);
    return Rval;
}

static UINT32 UpdateFileName(const char *pNameBuf, UINT32 StreamID)
{
    char    StgDrive = g_RcmCtrl.UsrCfg[StreamID].Drive;
    UINT32  Rval = SVC_OK;

    if (AmbaDCF_UpdateFile(pNameBuf) == 0U) {
        Rval = SVC_OK;
    } else {
        Rval = SVC_NG;
    }

    /* trigger storage manager to check free space */
    SvcStgMgr_TrigDelCheck(StgDrive);

    AmbaPrint_PrintStr5("update %s", pNameBuf, NULL, NULL, NULL, NULL);
    return Rval;
}

#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
static UINT64 CalcNeededSpace(UINT32 StreamID)
{
    UINT32                   i, FileSize;
    UINT64                   NeedSize = 0U;
    AMBA_REC_EVAL_s          MiaInfo;
    const SVC_REC_FWK_CFG_s  *pCfg = &(g_RcmCtrl.UsrCfg[StreamID]);

    for (i = 0U; i < pCfg->NumRbx; i++) {
        if (pCfg->RbxCfgArr[i]->BoxType == AMBA_RBX_TYPE_MP4) {
            MiaInfo = pCfg->MiaInfo;
            if (pCfg->RbxCfgArr[i]->AuCoding == AMBA_RSC_AUD_SUBTYPE_AAC) {
                MiaInfo.MiaCfg[AMBA_REC_MIA_AUDIO].BitRate = pCfg->RbxCfgArr[i]->AuBrate;
            }

            if (AmbaMux_EvalFileSize(pCfg->RbxCfgArr[i]->SubType, &(MiaInfo), &FileSize) != SVC_OK) {
                SvcLog_NG(LOG_RCM, "AmbaMux_EvalFileSize failed, (%u)", i, 0U);
            } else {
                NeedSize += FileSize;
                SvcLog_DBG(LOG_RCM, "[%u] eval file size = %u", i, FileSize);
            }
        }
    }
    NeedSize += RCM_PIV_SPACE_RESERVED;
    NeedSize *= RCM_FREE_SPACE_RESERVED;

    return NeedSize;
}
#endif

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define SD_FREE_SPACE_THRESOLD (1024 * 1024 * 1024 * 3UL)
static UINT32 FreeSpaceCheckByDrive(char ChkDrive)
{
    UINT32 Rval = SVC_NG;
    UINT64 SDFreeSize = 0;
    char Drive = 0;

    Drive = ChkDrive;
    SvcLog_OK(LOG_RCM, "FreeSpaceCheckByDrive", 0U, 0U);
    if (tolower(Drive) == tolower(SD_SLOT[0])
        && app_helper.check_sd_exist() == 0) {
        SvcLog_OK(LOG_RCM, "CheckFreeSpace the driver:%c, No sd card or card is bad", Drive, 0U);
        return SVC_NG;
    }
    if (AmbaDCF_IsReady() == 0) {
        SvcLog_OK(LOG_RCM, "CheckFreeSpace the driver:%c, DCF is not ready", Drive, 0U);
        return SVC_NG;
    }
    SvcStgMgr_SetFreeSizeThr(Drive, SD_FREE_SPACE_THRESOLD);
    SvcStgMgr_GetFreeSize(Drive, &SDFreeSize);
    SvcLog_OK(LOG_RCM, "CheckFreeSpace the driver:%c, FreeSize=%dMB", Drive, SDFreeSize / 1024 / 1024);
    if (SDFreeSize < SD_FREE_SPACE_THRESOLD) {
        UINT32 loopFileCount = 0;
        SVC_STG_MONI_DRIVE_INFO_s DriveInfo;
        UINT8 loopOk = 0;

        if (SVC_OK == SvcStgMonitor_GetDriveInfo(Drive, &DriveInfo)) {
            if (SVC_OK == AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, &loopFileCount)) {
                SvcLog_OK(LOG_RCM, "Check Loop FileCount:%d", loopFileCount, 0U);
                if (loopFileCount > 1) {
                    SvcStgMgr_TrigDelCheck(Drive);
                    loopOk = 1;
                }
            }
        }
        if (loopOk == 0) {
            SvcLog_NG(LOG_RCM, "sd free space is too small", 0U, 0U);
        } else {
            Rval = SVC_OK;
        }
    } else {
        Rval = SVC_OK;
    }


    return Rval;
}
#else
static UINT32 FreeSpaceCheckByDrive(char ChkDrive)
{
    UINT32                   i, Rval = SVC_NG;
    UINT64                   FreeSize = 0U, NeedSize = 0U;
    const SVC_REC_FWK_CFG_s  *pCfg;

    SvcStgMgr_GetFreeSize(ChkDrive, &FreeSize);
    if (FreeSize != 0U) {
        /* calc. needed size */
        for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[i]);

            if (pCfg->Drive == ChkDrive) {
                NeedSize += CalcNeededSpace(i);
            }
        }

        /* check free space */
        if (NeedSize < FreeSize) {
            Rval = SvcStgMgr_SetFreeSizeThr(ChkDrive, NeedSize);
        } else {
            SvcLog_NG(LOG_RCM, "## free space is too small, (0x%08X/0x%08X)", (UINT32)FreeSize
                                                                         , (UINT32)NeedSize);
        }
    }

    return Rval;
}
#endif

static UINT32 FreeSpaceCheck(UINT32 StreamID)
{
    return FreeSpaceCheckByDrive(g_RcmCtrl.UsrCfg[StreamID].Drive);
}

static UINT32 InitFreeSpaceCheck(UINT32 NumStrm, UINT16 *pStrmIdxArr)
{
    UINT32                   Rval = SVC_OK;
    char                     StgDrives[SVC_STG_DRIVE_NUM];
    UINT32                   i, s, StgNum = 0U;
    const SVC_REC_FWK_CFG_s  *pCfg;
    const REC_MAIN_FWK_s     *pFwk;

    if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE) == 0U) {
        AmbaSvcWrap_MisraMemset(StgDrives, 0, sizeof(StgDrives));

        /* check storage drive used */
        for (i = 0U; i < NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[pStrmIdxArr[i]]);
            pFwk = &(g_RcmCtrl.RecFwk[pStrmIdxArr[i]]);

            for (s = 0U; s < pCfg->NumRdt; s++) {
                if (pFwk->RdtArr[s].UsrCfg.DstType != AMBA_RDT_TYPE_NET) {
                    break;
                }
            }

            if (s != pCfg->NumRdt) {
                for (s = 0U; s < SVC_STG_DRIVE_NUM; s++) {
                    if (StgDrives[s] == pCfg->Drive) {
                        break;
                    }
                }

                if (s == SVC_STG_DRIVE_NUM) {
                    if (StgNum < SVC_STG_DRIVE_NUM) {
                        StgDrives[StgNum] = pCfg->Drive;
                        StgNum++;
                    } else {
                        Rval = SVC_NG;
                        SvcLog_NG(LOG_RCM, "## SVC_STG_DRIVE_NUM < StgNum", 0U, 0U);
                        break;
                    }
                }
            }
        }

        if (Rval == SVC_OK) {
            for (s = 0U; s < StgNum; s++) {
                Rval = FreeSpaceCheckByDrive(StgDrives[s]);
                if (Rval != SVC_OK) {
                    break;
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&NumStrm);
    AmbaMisra_TouchUnused(pStrmIdxArr);

    return Rval;
}

static UINT32 FStatusNotify(const AMBA_RDT_FSTATUS_INFO_s *pInfo)
{
    UINT32         Rval = SVC_OK, Err, Reserved = REC_MON_DATA_SIZE;
    static UINT32  InfoSize = (UINT32)sizeof(AMBA_RDT_FSTATUS_INFO_s);
    UINT8          *pData;
    REC_MON_QUE_s  Msg;

    if (InfoSize <= Reserved) {
        Msg.Type = REC_MON_TYPE_FSTATUS;
        pData    = Msg.Data;

        AmbaSvcWrap_MisraMemcpy(pData, pInfo, InfoSize);

        Err = AmbaKAL_MsgQueueSend(&(g_RcmCtrl.RecMonQueId), &Msg, AMBA_KAL_NO_WAIT);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_RCM, "AmbaKAL_MsgQueueSend failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    } else {
        SvcLog_NG(LOG_RCM, "REC_MON_DATA_SIZE is too small, (%u/%u)", InfoSize, Reserved);
        Rval = SVC_NG;
    }

    return Rval;
}

static void CodecClockCtrl(UINT32 Ctrl)
{
#if defined(AMBA_DSP_ENC_CODING_TYPE_H265)
    UINT32          i, IsHevcExist = 0U;
    SVC_ENC_INFO_s  EncInfo;

    if (0U < Ctrl) {
        SvcEnc_InfoGet(&EncInfo);
        for (i = 0U; i < *(EncInfo.pNumStrm); i++) {
            if (EncInfo.pStrmCfg[i].CodingFmt == AMBA_DSP_ENC_CODING_TYPE_H265) {
                IsHevcExist = 1U;
                break;
            }
        }

        /* enable feature clock */
        if (0U < IsHevcExist) {
            SvcClock_FeatureCtrl(1U, (SVC_CLK_FEA_BIT_CODEC | SVC_CLK_FEA_BIT_HEVC));
        } else {
            SvcClock_FeatureCtrl(1U, SVC_CLK_FEA_BIT_CODEC);
        }
    } else {
        /* disable feature clock */
        SvcClock_FeatureCtrl(0U, (SVC_CLK_FEA_BIT_CODEC | SVC_CLK_FEA_BIT_HEVC));
    }
#else
    if (0U < Ctrl) {
        /* enable feature clock */
        SvcClock_FeatureCtrl(1U, SVC_CLK_FEA_BIT_CODEC);
    } else {
        /* disable feature clock */
        SvcClock_FeatureCtrl(0U, SVC_CLK_FEA_BIT_CODEC);
    }
#endif
}

#ifdef CONFIG_AMBA_RTSP
static void RTSPSetup(UINT32 StreamID)
{
#if defined(CONFIG_RTSP_LINUX)
        SVC_ENC_INFO_s                   VEncInfo;
        SVC_EXAMFRAMESHARE_VID_CONFIG_s  VidConfig = {0};

        SvcEnc_InfoGet(&VEncInfo);

        VidConfig.StreamId = StreamID;
        AmbaSvcWrap_MisraMemcpy(&(VidConfig.VidInfo), &(VEncInfo.pStrmCfg[StreamID]), sizeof(AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s));
#if defined(CONFIG_ICAM_AUDIO_USED)
        {
            const SVC_REC_FWK_CFG_s           *pCfg;
            AMBA_AENC_FLOW_INFO_s             AEncInfo;
            SVC_EXAMFRAMESHARE_AUD_CONFIG_s   AudConfig = {0};
            UINT32                            SetAudio = 0U, i, Bit = 1U;

            pCfg = &(g_RcmCtrl.UsrCfg[StreamID]);

            AmbaAEncFlow_InfoGet(&AEncInfo);

            for (i = 0U; i < (UINT32)CONFIG_AENC_NUM; i++) {
                if (((Bit << i) & pCfg->AEncBits) > 0U) {
                    SetAudio = 1U;

                    AudConfig.StreamId    = i;
                    AmbaMisra_TypeCast(&(AudConfig.BitsBufBase), &(AEncInfo.pAEncInfo->pAencBsInfo.pHead));
                    AudConfig.BitsBufSize = AEncInfo.pAEncInfo->pAencBsInfo.Size;

                    if (*(AEncInfo.pAEncType) == AMBA_AUDIO_TYPE_PCM) {
                        AudConfig.Format = RTSP_LINK_AUD_PCM;
                    } else if (*(AEncInfo.pAEncType) == AMBA_AUDIO_TYPE_AAC) {
                        AudConfig.Format = RTSP_LINK_AUD_AAC;
                    } else {
                        SvcLog_NG(LOG_RCM, "Unknown audio type (%u)", AEncInfo.pAEncSetup->Type, 0U);
                        SetAudio = 0U;
                    }
                    AmbaSvcWrap_MisraMemcpy(&(AudConfig.AudInfo), &(AEncInfo.pAEncInfo->AencCreateInfo), sizeof(AMBA_AUDIO_ENC_CREATE_INFO_s));
                }
            }
            if (SetAudio == 1U) {
                SvcRtspLink_SetInfo(StreamID, &VidConfig, &AudConfig, SvcBuffer_Vir2Phys, AmbaAEncFlow_CheckAencId);
            } else {
                SvcRtspLink_SetInfo(StreamID, &VidConfig, NULL, SvcBuffer_Vir2Phys, AmbaAEncFlow_CheckAencId);
            }
        }
#else
        SvcRtspLink_SetInfo(StreamID, &VidConfig, NULL, SvcBuffer_Vir2Phys, NULL);
#endif
#endif

#if !defined(CONFIG_RTSP_LINUX)
        {
            SVC_ENC_INFO_s  VEncInfo;

            SvcEnc_InfoGet(&VEncInfo);
#if defined(CONFIG_ICAM_AUDIO_USED)
            {
                AMBA_AENC_FLOW_INFO_s   AEncInfo;
                SVC_RTSP_AUD_CONFIG_s   AudConfig;
                UINT32                  SetAudio = 1U;

                AmbaAEncFlow_InfoGet(&AEncInfo);

                if (*(AEncInfo.pAEncType) == AMBA_AUDIO_TYPE_PCM) {
                    AudConfig.Format = SVC_RTSP_AUD_PCM;
                } else if (*(AEncInfo.pAEncType) == AMBA_AUDIO_TYPE_AAC) {
                    AudConfig.Format = SVC_RTSP_AUD_AAC;
                } else {
                    SvcLog_NG(LOG_RCM, "Unknown audio type (%u)", *(AEncInfo.pAEncType), 0U);
                    SetAudio = 0U;
                }

                if (SetAudio == 1U) {
                    AudConfig.pEncInfo   = &(AEncInfo.pAEncInfo->AencCreateInfo);
                    AudConfig.pBsBufBase = (UINT8*)AEncInfo.pAEncInfo->pAencBsInfo.pHead;
                    AudConfig.BsBufSize  = AEncInfo.pAEncInfo->pAencBsInfo.Size;

                    SvcRtsp_SetInfo(StreamID, &(VEncInfo.pStrmCfg[StreamID]), &AudConfig);
                } else {
                    SvcRtsp_SetInfo(StreamID, &(VEncInfo.pStrmCfg[StreamID]), NULL);
                }
            }
#else
            SvcRtsp_SetInfo(StreamID, &(VEncInfo.pStrmCfg[StreamID]), NULL);
#endif
            AmbaPrint_PrintUInt5("RTSPSetup 1 StreamID %d", StreamID, 0U, 0U, 0U, 0U);

        }
#endif
    AmbaMisra_TouchUnused(&StreamID);
}
#endif

static UINT32 BankSyncSizeCalc(UINT32 StreamID)
{
    UINT32          BitRate, SyncSize;
    SVC_ENC_INFO_s  EncInfo;

    SvcEnc_InfoGet(&EncInfo);
    BitRate  = EncInfo.pStrmCfg[StreamID].EncConfig.BitRateCfg.BitRate;
    SyncSize = (BitRate * (UINT32)CONFIG_ICAM_REC_SYNC_TIME);
    SyncSize = GetRoundUpValU32(SyncSize, 8U);
    /* To disperse sync timing if some streams with same bit-rate */
    SyncSize += (StreamID * (UINT32)CONFIG_AMBA_VFS_BANK_SIZE);

    SyncSize  = GetRoundUpValU32(SyncSize, (UINT32)CONFIG_AMBA_VFS_BANK_SIZE);
    SyncSize *= (UINT32)CONFIG_AMBA_VFS_BANK_SIZE;

    SvcLog_DBG(LOG_RCM, "[%u] bank sync size(%u)", StreamID, SyncSize);
    return SyncSize;
}

static void* RecMonTask(void* EntryArg)
{
    #define REC_MON_EXEC_INTERVAL       (100U)
    #define REC_MON_WARN_PERCENTAGE     (85U)

    static char  FlagName[] = "SvcRecMonFlag";
    static char  QueName[]  = "SvcRecMonQue";

    UINT16                   StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];
    UINT32                   i, c, StartBits, NumStrm, Err;
    UINT32                   VUnProcSize[CONFIG_ICAM_DSP_ENC_ENG_NUM] = {0}, AUnProcSize;
    UINT32                   VBitsBufSize[CONFIG_ICAM_DSP_ENC_ENG_NUM] = {0}, ABitsBufSize;
    #if defined(CONFIG_ICAM_DATA_GATHER_USED)
    UINT32                   DUnProcSize, DBitsBufSize;
    #endif
    UINT32                   MaxQue = 0U, IsValid, IsWarn, ActualFlags, BufStatus = 0U;
    UINT32                   EngId, Bit = 1U, EngNum = (UINT32)CONFIG_ICAM_DSP_ENC_ENG_NUM;
    AMBA_RSC_STATIS_s         Statis = {0};
    const SVC_REC_FWK_CFG_s  *pCfg;
    const REC_MAIN_FWK_s     *pFwk;
    const AMBA_REC_SRC_s     *pRsc;
    const AMBA_REC_DST_s     *pRdt;
    SVC_ENC_INFO_s           EncInfo = {0};
    REC_MON_QUE_s            Msg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&Bit);
    AmbaMisra_TouchUnused(&EngNum);

    if (AmbaKAL_EventFlagCreate(&(g_RcmCtrl.RecMonFlag), FlagName) == SVC_OK) {
        Err = AmbaKAL_EventFlagSet(&(g_RcmCtrl.RecMonFlag), REC_MON_FLAG_IDLE);
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_RCM, "AmbaKAL_EventFlagSet REC_MON_FLAG_IDLE failed %u", Err, 0U);
        }
    } else {
        SvcLog_NG(LOG_RCM, "create flag failed!(%u)", 0U, 0U);
    }

    Err = AmbaKAL_MsgQueueCreate(&(g_RcmCtrl.RecMonQueId),
                                QueName,
                                (UINT32)sizeof(REC_MON_QUE_s),
                                &(g_RcmCtrl.RecMonQue[0]),
                                (UINT32)sizeof(g_RcmCtrl.RecMonQue));
    if (Err != SVC_OK) {
        SvcLog_NG(LOG_RCM, "Fail to create monitor queue %u", Err, 0U);
    }

    while (Err == SVC_OK) {
        Err = AmbaKAL_EventFlagGet(&(g_RcmCtrl.RecMonFlag),
                                   REC_MON_FLAG_ENABLE,
                                   AMBA_KAL_FLAGS_ALL,
                                   AMBA_KAL_FLAGS_CLEAR_NONE,
                                   &ActualFlags,
                                   AMBA_KAL_WAIT_FOREVER);
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_RCM, "AmbaKAL_EventFlagGet failed %u", Err, 0U);
            continue;
        }

        Err = AmbaKAL_EventFlagClear(&(g_RcmCtrl.RecMonFlag), REC_MON_FLAG_IDLE);
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_RCM, "AmbaKAL_EventFlagClear REC_MON_FLAG_IDLE failed %u", Err, 0U);
            continue;
        }

        StartBits = g_RcmCtrl.StartBits;
        SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StartBits, &NumStrm, StrmIdxArr);

        SvcEnc_InfoGet(&EncInfo);

        AmbaSvcWrap_MisraMemset(VUnProcSize, 0, sizeof(VUnProcSize));
        AmbaSvcWrap_MisraMemset(VBitsBufSize, 0, sizeof(VBitsBufSize));

        AUnProcSize    = 0U;
        ABitsBufSize   = 0U;

        #if defined(CONFIG_ICAM_DATA_GATHER_USED)
        DUnProcSize    = 0U;
        DBitsBufSize   = 0U;
        #endif

        IsWarn = 0U;
        for (i = 0U; i < NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[StrmIdxArr[i]]);
            pFwk = &(g_RcmCtrl.RecFwk[StrmIdxArr[i]]);

            for (c = 0U; c < pCfg->NumRsc; c++) {
                pRsc = &(pFwk->RscArr[c]);
                IsValid        = 0U;
                switch (pRsc->UsrCfg.SrcType) {
                case AMBA_RSC_TYPE_VIDEO:
                    MaxQue = CONFIG_AMBA_REC_MAX_VRSC_QUELEN;
                    AmbaRscVA_Control(pRsc, AMBA_RSC_VA_STATIS_GET, &Statis);

                    /* the encoders under the same engine will use the same bitstream buffer */
                    for (EngId = 0U; EngId < EngNum; EngId++) {
                    #if CONFIG_ICAM_DSP_ENC_ENG_NUM > 1
                        if ((EncInfo.pStrmCfg[i].Affinity & (Bit << EngId)) > 0U) {
                    #else
                        {
                    #endif
                            VUnProcSize[EngId]  += BitsDataSizeCalc(i, AMBA_RSC_TYPE_VIDEO, 0U, RCM_BITSCALC_OP_GET);
                            VBitsBufSize[EngId]  = pRsc->UsrCfg.BsBufSize;
                            IsValid              = 1U;
                        }
                    }
                    break;
                case AMBA_RSC_TYPE_AUDIO:
                    MaxQue = CONFIG_AMBA_REC_MAX_ARSC_QUELEN;
                    AmbaRscVA_Control(pRsc, AMBA_RSC_VA_STATIS_GET, &Statis);
                    AUnProcSize    = BitsDataSizeCalc(i, AMBA_RSC_TYPE_AUDIO, 0U, RCM_BITSCALC_OP_GET);
                    ABitsBufSize   = pRsc->UsrCfg.BsBufSize;  /* all rec stream use the same audio bits buf */
                    IsValid        = 1U;
                    break;
                case AMBA_RSC_TYPE_DATA:
                #if defined(CONFIG_ICAM_DATA_GATHER_USED)
                    MaxQue = CONFIG_AMBA_REC_MAX_DRSC_QUELEN;
                    AmbaRscData_Control(pRsc, AMBA_RSC_DATA_STATIS_GET, &Statis);
                    DUnProcSize   += BitsDataSizeCalc(i, AMBA_RSC_TYPE_DATA, 0U, RCM_BITSCALC_OP_GET);
                    DBitsBufSize  += AMBA_RBXC_MP4_TEXT_BUF_SIZE;  /* each stream has its own text buf */
                    IsValid        = 1U;
                #endif
                    break;
                default:
                    /* do nothing */
                    break;
                }

                if (0U < IsValid) {
                    /* monitor queue status and report if queue > 85% queue size */
                    if ((0U < Statis.QueLen) && (Statis.QueLen >= ((MaxQue * (UINT32)REC_MON_WARN_PERCENTAGE) / 100U))) {
                        IsWarn = 1U;

                        SvcLog_DBG(LOG_RCM, "\n", 0U, 0U);
                        SvcLog_DBG(LOG_RCM, "## stream%u_src%u", StrmIdxArr[i]
                                                               , pRsc->UsrCfg.SrcType);
                        SvcLog_DBG(LOG_RCM, "## SrcQue %u/%u", Statis.QueLen, MaxQue);
                        SvcLog_DBG(LOG_RCM, "\n", 0U, 0U);
                    }
                }
            }

            if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE) == 0U) {
                for (c = 0U; c < pCfg->NumRdt; c++) {
                    pRdt = &(pFwk->RdtArr[c]);
                    switch (pRdt->UsrCfg.DstType) {
                    case AMBA_RDT_TYPE_FILE:
                        AmbaRdtFile_Control(pRdt, AMBA_RDTC_FILE_BUF_STATUS, &BufStatus);
                        if (BufStatus != 0U) {
                            IsWarn = 1U;
                            SvcLog_DBG(LOG_RCM, "RdtFile Buf is fulled", 0U, 0U);
                        }
                        break;
                    default:
                        /* do nothing */
                        break;
                    }
                }
            }

            if (0U < IsWarn) {
                break;
            }
        }

        /* bitstream data overwritten check */
        {
            for (EngId = 0U; EngId < EngNum; EngId++) {
                if (VUnProcSize[EngId] > 0U) {
                    if (VUnProcSize[EngId] >= VBitsBufSize[EngId]) {
                        IsWarn = 1U;

                        SvcLog_DBG(LOG_RCM, "\n", 0U, 0U);
                        SvcLog_DBG(LOG_RCM, "## video eng%u", EngId, 0U);
                        SvcLog_DBG(LOG_RCM, "##   bits_unhandled(0x%X)", VUnProcSize[EngId], 0U);
                        SvcLog_DBG(LOG_RCM, "##   bits_bufsize  (0x%X)", VBitsBufSize[EngId], 0U);
                        SvcLog_DBG(LOG_RCM, "\n", 0U, 0U);
                    }
                }
            }

            if (AUnProcSize > 0U) {
                if (AUnProcSize >= ABitsBufSize) {
                    IsWarn = 1U;

                    SvcLog_DBG(LOG_RCM, "\n", 0U, 0U);
                    SvcLog_DBG(LOG_RCM, "## audio", 0U, 0U);
                    SvcLog_DBG(LOG_RCM, "##   bits_unhandled(0x%X)", AUnProcSize, 0U);
                    SvcLog_DBG(LOG_RCM, "##   bits_bufsize  (0x%X)", ABitsBufSize, 0U);
                    SvcLog_DBG(LOG_RCM, "\n", 0U, 0U);
                }
            }

            #if defined(CONFIG_ICAM_DATA_GATHER_USED)
            if (DUnProcSize > 0U) {
                if (DUnProcSize >= DBitsBufSize) {
                    IsWarn = 1U;

                    SvcLog_DBG(LOG_RCM, "\n", 0U, 0U);
                    SvcLog_DBG(LOG_RCM, "## data", 0U, 0U);
                    SvcLog_DBG(LOG_RCM, "##   bits_unhandled(0x%X)", DUnProcSize, 0U);
                    SvcLog_DBG(LOG_RCM, "##   bits_bufsize  (0x%X)", DBitsBufSize, 0U);
                    SvcLog_DBG(LOG_RCM, "\n", 0U, 0U);
                }
            }
            #endif
        }

        Err = AmbaKAL_EventFlagSet(&(g_RcmCtrl.RecMonFlag), REC_MON_FLAG_IDLE);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_RCM, "AmbaKAL_EventFlagSet REC_MON_FLAG_IDLE failed %u", Err, 0U);
            continue;
        }

        if (0U < IsWarn) {
            if (g_RcmCtrl.pfnRecMonNotify != NULL) {
                g_RcmCtrl.pfnRecMonNotify(SVC_REC_MON_NCODE_QUEUE_EXCEP, g_RcmCtrl.StartBits);
            }
        }

        if (AmbaKAL_MsgQueueReceive(&(g_RcmCtrl.RecMonQueId),
                                    &Msg,
                                    REC_MON_EXEC_INTERVAL) == KAL_ERR_NONE) {
            switch (Msg.Type) {
            case REC_MON_TYPE_FSTATUS:
                {
                    const AMBA_RDT_FSTATUS_INFO_s *pFInfo;
                    const UINT8                   *pData = Msg.Data;
                    UINT32                        RecId;

                    AmbaMisra_TypeCast(&pFInfo, &pData);
                    RecId = pFInfo->RecStrmId;

                    if (RecId < (UINT32)CONFIG_ICAM_MAX_REC_STRM) {
                        if (g_RcmCtrl.RecEvtCb[RecId][SVC_RCM_EVT_FSTATUS] != NULL) {
                            g_RcmCtrl.RecEvtCb[RecId][SVC_RCM_EVT_FSTATUS](SVC_RCM_EVT_FSTATUS, pData);
                        }
                    }
                }
                break;
            default:
            /**/
                break;
            }
        }
    }

    return NULL;
}

static void RecMonTaskCreate(PFN_REC_MON_NOTIFY pfnNotify)
{
    #define REC_MON_STACK_SIZE      (0x4600U)

    UINT32        Rval = SVC_OK, i, j;
    static UINT8  RecMonStack[REC_MON_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_RECMON_EXIST) == 0U) {
        /* task create */
        g_RcmCtrl.RecMonTask.EntryFunc  = RecMonTask;
        g_RcmCtrl.RecMonTask.pStackBase = RecMonStack;
        g_RcmCtrl.RecMonTask.StackSize  = REC_MON_STACK_SIZE;

        for (i = 0U; i < (UINT32)CONFIG_ICAM_MAX_REC_STRM; i++) {
            for (j = 0U; j < SVC_RCM_EVT_NUM; j++) {
                g_RcmCtrl.RecEvtCb[i][j] = NULL;
            }
        }

        Rval = SvcTask_Create("SvcRecMonTask", &(g_RcmCtrl.RecMonTask));
        if (Rval == SVC_OK) {
            g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_RECMON_EXIST);
            SvcLog_DBG(LOG_RCM, "SvcRecMonTask is created", 0U, 0U);
        } else {
            SvcLog_NG(LOG_RCM, "SvcRecMonTask created failed!(%u)", Rval, 0U);
        }
    }

    g_RcmCtrl.pfnRecMonNotify = pfnNotify;
}

static void RecMonTaskDestroy(void)
{
    UINT32  Rval = SVC_OK, ActualFlags;

    if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_RECMON_EXIST)) {
        if (AmbaKAL_EventFlagGet(&(g_RcmCtrl.RecMonFlag),
                                REC_MON_FLAG_IDLE,
                                AMBA_KAL_FLAGS_ALL,
                                AMBA_KAL_FLAGS_CLEAR_NONE,
                                &ActualFlags,
                                5000U) == SVC_OK) {
            /* task delete */
            Rval = SvcTask_Destroy(&(g_RcmCtrl.RecMonTask));
            if (Rval != SVC_OK) {
                SvcLog_NG(LOG_RCM, "delete task failed!(%u)", Rval, 0U);
            }

            /* flag delete */
            Rval = AmbaKAL_EventFlagDelete(&(g_RcmCtrl.RecMonFlag));
            if (Rval == SVC_OK) {
                g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_RECMON_EXIST);
                SvcLog_DBG(LOG_RCM, "SvcRecMonTask is destroyed", 0U, 0U);
            } else {
                SvcLog_NG(LOG_RCM, "delete flag failed!(%u)", Rval, 0U);
            }

            Rval = AmbaKAL_MsgQueueDelete(&(g_RcmCtrl.RecMonQueId));
            if (Rval != SVC_OK) {
                SvcLog_NG(LOG_RCM, "delete que failed!(%u)", 0U, 0U);
            }
        } else {
            SvcLog_NG(LOG_RCM, "fail to wait event flag", 0U, 0U);
        }
    }
}

#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
static void* RecMVTask(void* EntryArg)
{
    #define HEVC_CTB_GRID_SIZE      (32U)

    UINT32                   i, Err = SVC_OK, AlignSize;
    UINT32                   CtbColNum[CONFIG_ICAM_MAX_REC_STRM];
    ULONG                    AlignStart;
    const SVC_REC_FWK_CFG_s  *pCfg;
    AMBA_DSP_ENC_MV_RDY_s    MV;
    SVC_ENC_MV_DATA_s        MvData;

    AmbaMisra_TouchUnused(EntryArg);

    AmbaSvcWrap_MisraMemset(CtbColNum, 0, sizeof(CtbColNum));
    for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
        pCfg = &(g_RcmCtrl.UsrCfg[i]);
        CtbColNum[i] = GetAlignedValU32(pCfg->EncWin.Width, HEVC_CTB_GRID_SIZE) >> 5U;
    }

    AmbaSvcWrap_MisraMemset(&MvData, 0, sizeof(SVC_ENC_MV_DATA_s));
    while (Err == SVC_OK) {
        Err = AmbaKAL_MsgQueueReceive(&(g_RcmCtrl.RecMVQueId), &MV, AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_RCM, "AmbaKAL_MsgQueueReceive AMBA_KAL_WAIT_FOREVER failed %u", Err, 0U);
            continue;
        }
        if (((UINT32)CONFIG_ICAM_MAX_REC_STRM <= MV.StreamId)
            || (MV.MvBufAddr == 0U)
            || (2U <= MV.MvBufType)) {
            continue;
        }

        if (MV.MvBufType == 0U) {
            MvData.StreamId      = MV.StreamId;
            MvData.CtbColNum     = CtbColNum[MV.StreamId];
            MvData.CtbRowNum     = MV.MvBufHeight;
            MvData.IntraBufPitch = MV.MvBufPitch;
            MvData.IntraBufBase  = MV.MvBufAddr;

            AlignStart = MV.MvBufAddr & AMBA_CACHE_LINE_MASK;
            AlignSize  = (MV.MvBufAddr + (MvData.IntraBufPitch * MvData.CtbRowNum)) - AlignStart;
            AlignSize  = (AlignSize + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK;
            if (SvcPlat_CacheInvalidate(AlignStart, AlignSize) != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcPlat_CacheInvalidate failed %u", 0U, 0U);
            }

            RCM_DBG("StreamId(%u)", MvData.StreamId, 0U);
            RCM_DBG(" CtbColNum(%u)", MvData.CtbColNum, 0U);
            RCM_DBG(" CtbRowNum(%u)", MvData.CtbRowNum, 0U);
            RCM_DBG(" IntraBufPitch(%u)", MvData.IntraBufPitch, 0U);
            RCM_DBG(" IntraBufBase(0x%X)", MvData.IntraBufBase, 0U);
        }

        if (MV.MvBufType == 1U) {
            MvData.MVBufPitch = MV.MvBufPitch;
            MvData.MVBufBase  = MV.MvBufAddr;

            AlignStart = MV.MvBufAddr & AMBA_CACHE_LINE_MASK;
            AlignSize  = (MV.MvBufAddr + (MvData.MVBufPitch * MvData.CtbRowNum)) - AlignStart;
            AlignSize  = (AlignSize + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK;
            if (SvcPlat_CacheInvalidate(AlignStart, AlignSize) != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcPlat_CacheInvalidate failed %u", 0U, 0U);
            }

            RCM_DBG(" MVBufPitch(%u)", MvData.MVBufPitch, 0U);
            RCM_DBG(" MVBufBase(0x%X)", MvData.MVBufBase, 0U);

            if (g_RcmCtrl.pfnRecMVNotify != NULL) {
                RCM_DBG(" MVNotify_S ...", 0U, 0U);
                g_RcmCtrl.pfnRecMVNotify(&MvData);
                RCM_DBG(" MVNotify_E", 0U, 0U);
            }
        }
    }

    return NULL;
}

static void RecMVTaskCreate(void)
{
    #define REC_MV_STACK_SIZE      (0x1000U)

    static char   RecMVQueName[] = "RecMVQue";
    static UINT8  RecMVStack[REC_MV_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    UINT32  Rval = SVC_OK;

    if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_RECMV_EXIST) == 0U) {
        if (AmbaKAL_MsgQueueCreate(&(g_RcmCtrl.RecMVQueId),
                                   RecMVQueName,
                                   sizeof(AMBA_DSP_ENC_MV_RDY_s),
                                   g_RcmCtrl.RecMVQue,
                                   sizeof(g_RcmCtrl.RecMVQue)) != SVC_OK) {
            SvcLog_NG(LOG_RCM, "Fail to create MV queue", 0U, 0U);
        }

        /* task create */
        g_RcmCtrl.RecMVTask.EntryFunc  = RecMVTask;
        g_RcmCtrl.RecMVTask.pStackBase = RecMVStack;
        g_RcmCtrl.RecMVTask.StackSize  = REC_MV_STACK_SIZE;

        Rval = SvcTask_Create("SvcRecMVTask", &(g_RcmCtrl.RecMVTask));
        if (Rval == SVC_OK) {
            g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_RECMV_EXIST);
            SvcLog_DBG(LOG_RCM, "SvcRecMVTask is created", 0U, 0U);
        } else {
            SvcLog_NG(LOG_RCM, "SvcRecMVTask created failed!(%u)", Rval, 0U);
        }
    }
}

static void RecMVTaskDestroy(void)
{
    UINT32  Rval = SVC_OK;

    if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_RECMV_EXIST)) {
        /* task delete */
        Rval = SvcTask_Destroy(&(g_RcmCtrl.RecMVTask));
        if (Rval != SVC_OK) {
            SvcLog_NG(LOG_RCM, "delete task failed!(%u)", Rval, 0U);
        }

        Rval = AmbaKAL_MsgQueueDelete(&(g_RcmCtrl.RecMVQueId));
        if (Rval != SVC_OK) {
            SvcLog_NG(LOG_RCM, "fail to destroy MV queue", 0U, 0U);
        }

        g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_RECMV_EXIST);
        SvcLog_DBG(LOG_RCM, "SvcRecMVTask is destroyed", 0U, 0U);
    }
}
#endif

/**
* get record framework config
* @param [out] pInfo record info
*/
void SvcRecMain_InfoGet(SVC_REC_MAIN_INFO_s *pInfo)
{
    pInfo->pNumStrm          = &(g_RcmCtrl.NumStrm);
    pInfo->pFwkStrmCfg       = g_RcmCtrl.UsrCfg;
    pInfo->pQpCtrlPriority   = &(g_RcmCtrl.QpCtrlPriority);
    pInfo->pQpCtrlCpuBits    = &(g_RcmCtrl.QpCtrlCpuBits);
    pInfo->pRecBlendPriority = &(g_RcmCtrl.RecBlendPriority);
    pInfo->pRecBlendCpuBits  = &(g_RcmCtrl.RecBlendCpuBits);
    pInfo->pRecMonPriority   = &(g_RcmCtrl.RecMonTask.Priority);
    pInfo->pRecMonCpuBits    = &(g_RcmCtrl.RecMonTask.CpuBits);
    pInfo->pNumAEnc          = &(g_RcmCtrl.NumAEnc);
    pInfo->pBitsCmpPriority  = &(g_RcmCtrl.BitsCmpPriority);
    pInfo->pBitsCmpCpuBits   = &(g_RcmCtrl.BitsCmpCpuBits);
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
    pInfo->pRecMVPriority    = &(g_RcmCtrl.RecMVTask.Priority);
    pInfo->pRecMVCpuBits     = &(g_RcmCtrl.RecMVTask.CpuBits);
    pInfo->ppfnMVNotify      = &(g_RcmCtrl.pfnRecMVNotify);
#endif
}

/**
* record init
*/
void SvcRecMain_Init(void)
{
    UINT32             i, j;
    SVC_ENC_EVTCB_s    EncCBs;
    SVC_REC_FWK_CFG_s  *pCfg;
    REC_MAIN_FWK_s     *pFwk;
    static char        StartFlagName[32U] = "RecStartFlag";

    /* reset */
    AmbaSvcWrap_MisraMemset(&g_RcmCtrl, 0, sizeof(REC_MAIN_CTRL_s));

    for (i = 0U; i < (UINT32)CONFIG_ICAM_MAX_REC_STRM; i++) {
        pCfg = &(g_RcmCtrl.UsrCfg[i]);
        pFwk = &(g_RcmCtrl.RecFwk[i]);

        pCfg->RmaCfg = &(pFwk->Rma.UsrCfg);

        for (j = 0U; j < (UINT32)CONFIG_ICAM_MAX_RSC_PER_STRM; j++) {
            pCfg->RscCfgArr[j] = &(pFwk->RscArr[j].UsrCfg);
        }

        for (j = 0U; j < (UINT32)CONFIG_ICAM_MAX_RBX_PER_STRM; j++) {
            pCfg->RbxCfgArr[j] = &(pFwk->RbxArr[j].UsrCfg);
        }

        for (j = 0U; j < (UINT32)CONFIG_ICAM_MAX_RDT_PER_STRM; j++) {
            pCfg->RdtCfgArr[j] = &(pFwk->RdtArr[j].UsrCfg);
        }
    }

    AmbaSvcWrap_MisraMemset(g_RcmCtrl.RscIdxMap, 0xFF, sizeof(g_RcmCtrl.RscIdxMap));

    /* hook video encoder callback functions */
    EncCBs.pCBStart   = EncStartCB;
    EncCBs.pCBStop    = EncStopCB;
    EncCBs.pCBDataRdy = EncDataRdyCB;
    EncCBs.pCBMVData  = EncMVDataCB;
    SvcEnc_HookEvent(1U, &EncCBs);

#ifdef CONFIG_ICAM_AUDIO_USED
    /* hook audio encoder callback */
    AmbaAEncFlow_CBDataRdyInit();
    for (i = 0U; i < (UINT32)(CONFIG_AENC_NUM); i++) {
        AmbaAEncFlow_HookEvent(i, 1U, AEncDataRdyCB);
    }
#endif

    if (KAL_ERR_NONE != AmbaKAL_EventFlagCreate(&(g_RcmCtrl.StartFlag), StartFlagName)) {
        SvcLog_NG(LOG_RCM, "fail to create event flag", 0U, 0U);
    }
    if (KAL_ERR_NONE != AmbaKAL_EventFlagClear(&(g_RcmCtrl.StartFlag), 0xFFFFFFFFU)) {
        SvcLog_NG(LOG_RCM, "fail to clear event flag", 0U, 0U);
    }
}

/**
* load record framework
*/
void SvcRecMain_FwkLoad(PFN_REC_MON_NOTIFY pfnMonNotify, PFN_REC_CAP_THM pfnCapThm)
{
    UINT32             i, c, Rval = SVC_OK, Type, SubType, SrcBits, BoxBits, InBoxBits;
    UINT32             IsAudioExist = 0U;
    SVC_REC_FWK_CFG_s  *pCfg;
    REC_MAIN_FWK_s     *pFwk;
    SVC_ENC_INFO_s     EncInfo;

    if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_FWK_LOADED) == 0U) {
        g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_FWK_LOADED);

        g_RcmCtrl.DataBits = 0U;
        for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[i]);
            pFwk = &(g_RcmCtrl.RecFwk[i]);

            /* create record source */
            for (c = 0U; c < pCfg->NumRsc; c++) {
                Type = pCfg->RscCfgArr[c]->SrcType;

                switch (Type) {
                case AMBA_RSC_TYPE_VIDEO:
                case AMBA_RSC_TYPE_AUDIO:
                    Rval = AmbaRscVA_Create(&(pFwk->RscArr[c]));
                    if (Type == AMBA_RSC_TYPE_AUDIO) {
                        IsAudioExist = 1U;
                    }
                    break;
                case AMBA_RSC_TYPE_DATA:
#if defined(CONFIG_ICAM_DATA_GATHER_USED)
                    Rval = AmbaRscData_Create(&(pFwk->RscArr[c]));
                    if (Rval == SVC_OK) {
                        Rval = SvcDataGather_Register(&(g_RcmCtrl.DataGCtrl[i]), DataGatherCB);
                        g_RcmCtrl.DataBits |= ((UINT32)0x01U << i);
                    }

                    /* data gather frame rate is equal to vin capture frame rate, we may need to skip
                       some frames if (vin frame rate !=  video encode frame rate) */
                    {
                        const SVC_RES_CFG_s    *pResCfg = SvcResCfg_Get();
                        const AMBA_REC_SRC_s   *pRsc = &(pFwk->RscArr[c]);
                        UINT32                 VinBits = 0U, j;
                        UINT64                 Div = 1U;

                        SvcResCfg_GetVinBitsOfRecIdx(i, &VinBits);

                        for (j = 0U; j < AMBA_DSP_MAX_VIN_NUM; j++) {
                            if ((VinBits & (1UL << j)) != 0U) {
                                Div  = (UINT64)(pResCfg->VinCfg[j].FrameRate.TimeScale) * (UINT64)(pRsc->UsrCfg.NumUnitsInTick);
                                Div /= (UINT64)(pResCfg->VinCfg[j].FrameRate.NumUnitsInTick);
                                Div /= (UINT64)(pRsc->UsrCfg.TimeScale);

                                break;
                            }
                        }

                        g_RcmCtrl.DataGDiv[i] = Div;

                        SvcLog_DBG(LOG_RCM, "DataGDiv (%u/%u)", i, (UINT32)g_RcmCtrl.DataGDiv[i]);
                    }
#endif
                    break;
                default:
                    /* unknown source type */
                    Rval = SVC_NG;
                    break;
                }

                if (Rval == SVC_OK) {
                    g_RcmCtrl.RscIdxMap[i][Type] = c;
                } else {
                    SvcLog_NG(LOG_RCM, "fail to create source", 0U, 0U);
                }
            }

            /* create record box */
            for (c = 0U; c < pCfg->NumRbx; c++) {
                Type = pCfg->RbxCfgArr[c]->BoxType;
                if (Type == AMBA_RBX_TYPE_MP4) {
                    pCfg->RbxCfgArr[c]->MiaInfo = pCfg->MiaInfo;

                    Rval = AmbaRbxMP4_Create(&(pFwk->RbxArr[c]));
                } else {
                    /* unknown box type */
                    Rval = SVC_NG;
                }

                if (Rval != SVC_OK) {
                    SvcLog_NG(LOG_RCM, "fail to create box", 0U, 0U);
                }
            }

            /* create record destination */
            if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE) == 0U) {
                for (c = 0U; c < pCfg->NumRdt; c++) {
                    Type    = pCfg->RdtCfgArr[c]->DstType;
                    SubType = pCfg->RdtCfgArr[c]->SubType;
                    if (Type == AMBA_RDT_TYPE_FILE) {
                        pCfg->RdtCfgArr[c]->StreamID        = i;
                        pCfg->RdtCfgArr[c]->pfnFetchFName   = FetchFileName;
                        pCfg->RdtCfgArr[c]->pfnUpdateFName  = UpdateFileName;
                        pCfg->RdtCfgArr[c]->pfnCheckStorage = FreeSpaceCheck;
                        pCfg->RdtCfgArr[c]->pfnFStatusNotify= FStatusNotify;
                        Rval = AmbaRdtFile_Create(&(pFwk->RdtArr[c]));

                    } else if ((Type == AMBA_RDT_TYPE_EVT) && (SubType == AMBA_RDT_STYPE_NML)){
                        pCfg->RdtCfgArr[c]->StreamID        = i;
                        pCfg->RdtCfgArr[c]->pfnFetchFName   = FetchEventFileName;
                        pCfg->RdtCfgArr[c]->pfnUpdateFName  = UpdateFileName;
                        pCfg->RdtCfgArr[c]->pfnCheckStorage = FreeSpaceCheck;
                        pCfg->RdtCfgArr[c]->pfnFStatusNotify= FStatusNotify;
                        Rval = AmbaRdtNmlEvt_Create(&(pFwk->RdtArr[c]));

                    } else if ((Type == AMBA_RDT_TYPE_EVT) && (SubType == AMBA_RDT_STYPE_EMG)){
                        pCfg->RdtCfgArr[c]->StreamID        = i;
                        pCfg->RdtCfgArr[c]->pfnFetchFName   = FetchEventFileName;
                        pCfg->RdtCfgArr[c]->pfnUpdateFName  = UpdateFileName;
                        pCfg->RdtCfgArr[c]->pfnCheckStorage = FreeSpaceCheck;
                        pCfg->RdtCfgArr[c]->pfnFStatusNotify= FStatusNotify;
                        Rval = AmbaRdtEmgEvt_Create(&(pFwk->RdtArr[c]));

                    } else if (Type == AMBA_RDT_TYPE_NET) {
                        pCfg->RdtCfgArr[c]->StreamID        = i;
                        Rval = AmbaRdtNet_Create(&(pFwk->RdtArr[c]));
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                        #if defined(CONFIG_AMBA_RTSP) && defined(CONFIG_RTSP_LINUX)
                        RTSPSetup(i);
                        #endif
#endif
                    } else {
                        SvcLog_NG(LOG_RCM, "Unknown destination type", 0U, 0U);
                        Rval = SVC_NG;
                    }

                    if (Rval != SVC_OK) {
                        SvcLog_NG(LOG_RCM, "fail to create destination", 0U, 0U);
                    }
                }
            }

            SrcBits   = 0U;
            BoxBits   = 0U;
            InBoxBits = 0U;
            /* create record master */
            if (0U < pCfg->NumRsc) {
                pCfg->RmaCfg->pfnProcBitsSizeUpdate = ProcBitsSizeUpdate;
                pCfg->RmaCfg->StreamId              = i;

                #if defined(CONFIG_ICAM_ENCRYPTION)
                pCfg->RmaCfg->pfnEncrypt = BitsDataEncrypt;
                #else
                pCfg->RmaCfg->pfnEncrypt = NULL;
                #endif

                Rval = AmbaRecMaster_Create(&(pFwk->Rma), pCfg->RmaPriority, pCfg->RmaCpuBits);

                if (Rval == SVC_OK) {
                    /* set split time */
                    AmbaRecMaster_Control(&(pFwk->Rma),
                                        AMBA_RMAC_SPLIT_TIME,
                                        &(pCfg->MiaInfo.FileSplitTime));

                    /* hook record source */
                    for (c = 0U; c < pCfg->NumRsc; c++) {
                        AmbaRecMaster_SrcHook(&(pFwk->Rma), &(pFwk->RscArr[c]));
                        SrcBits |= pFwk->RscArr[c].SrcBit;
                    }

                    /* hook record box */
                    for (c = 0U; c < pCfg->NumRbx; c++) {
                        /* assign source bits to box */
                        pFwk->RbxArr[c].UsrCfg.InSrcBits = SrcBits;

                        AmbaRecMaster_BoxHook(&(pFwk->Rma), &(pFwk->RbxArr[c]));
                        BoxBits |= pFwk->RbxArr[c].BoxBit;
                    }

                    /* hook record destination */
                    if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE) == 0U) {
                        for (c = 0U; c < pCfg->NumRdt; c++) {
                            /* assign source/box bits to destinations */
                            if ((0U < BoxBits) && (pFwk->RdtArr[c].UsrCfg.DstType != AMBA_RDT_TYPE_NET)
                                            && (pFwk->RdtArr[c].UsrCfg.SubType != AMBA_RDT_STYPE_EMG)) {
                                pFwk->RdtArr[c].UsrCfg.InSrcBits = 0U;
                                pFwk->RdtArr[c].UsrCfg.InBoxBits = BoxBits;
                            } else {
                                pFwk->RdtArr[c].UsrCfg.InSrcBits = SrcBits;
                                pFwk->RdtArr[c].UsrCfg.InBoxBits = 0U;
                            }
                            InBoxBits = SetBits(InBoxBits, pFwk->RdtArr[c].UsrCfg.InBoxBits);

                            AmbaRecMaster_DstHook(&(pFwk->Rma), &(pFwk->RdtArr[c]));
                        }
                    }

                    /* clear SrcBits of record box if it has no destination */
                    if (InBoxBits == 0U) {
                        for (c = 0U; c < pCfg->NumRbx; c++) {
                            /* assign source bits to box */
                            pFwk->RbxArr[c].UsrCfg.InSrcBits = 0U;
                        }
                    }
                } else {
                    SvcLog_NG(LOG_RCM, "fail to create master", 0U, 0U);
                }
            }
        }

        if (g_RcmCtrl.NumStrm > 0U) {
            /* check if there is audio source */
            if (0U < IsAudioExist) {
                g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_AUDIO_EXIST);
            }


            /* create qp ctrl task */
            SvcEnc_InfoGet(&EncInfo);
            for (i = 0U; i < *(EncInfo.pNumStrm); i++) {
#ifndef CONFIG_ICAM_PROJECT_SHMOO
                if (EncInfo.pStrmCfg[i].EncConfig.BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_SMART_VBR) {
                    if (SVC_OK == SvcRecQpCtrlCreate(g_RcmCtrl.QpCtrlPriority, g_RcmCtrl.QpCtrlCpuBits)) {
                        g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_QPCTRL_EXIST);
                        break;
                    }
                }
#endif
            }


            /*create blend task */
            if (SVC_OK == SvcRecBlend_Create(g_RcmCtrl.RecBlendPriority, g_RcmCtrl.RecBlendCpuBits)) {
                g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_BLEND_EXIST);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                SvcDrawDateTime_Init();
                SvcDrawLogo_Init();
#else
                SvcDrawStopwatch_Init();
#endif

            }

            /* create mutex for bitsdata caculation */
            {
                static char RcmMtx[] = "SvcRcmBitsCalcMtx";

                Rval = AmbaKAL_MutexCreate(&(g_RcmCtrl.BitsDataMutex), RcmMtx);
                if (SVC_OK != Rval) {
                    SvcLog_NG(LOG_RCM, "AmbaKAL_MutexCreate SvcRcmBitsCalcMtx failed %u", Rval, 0U);
                } else {
                    g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_BITSMTX_EXIST);
                }
            }

#if defined(CONFIG_ICAM_TIMELAPSE_USED)
            /* Create time-lapse tasks */
            for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
                pCfg = &(g_RcmCtrl.UsrCfg[i]);
                if (pCfg->IsTimeLapse > 0U) {
                    Rval = SvcTimeLapse_TaskCreate();
                    if (SVC_OK != Rval) {
                        SvcLog_NG(LOG_RCM, "SvcTimeLapse_TaskCreate failed", 0U, 0U);
                    }
                    break;
                }
            }
#endif
        }

#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
        /* create record MV task */
        RecMVTaskCreate();
#endif
        Rval = AmbaMux_Init();
        if (Rval != SVC_OK) {
            SvcLog_NG(LOG_RCM, "fail to init muxer", 0U, 0U);
        }
    } else {
        SvcLog_DBG(LOG_RCM, "resouces are already loaded", 0U, 0U);
    }

    /* create record monitor task */
    RecMonTaskCreate(pfnMonNotify);

    /* install video thumbnail capture CB */
    g_RcmCtrl.pfnCapThm = pfnCapThm;
}

/**
* unload record framework
*/
void SvcRecMain_FwkUnload(void)
{
    UINT32                   Rval, i, c;
    const AMBA_REC_SRC_s     *pRsc;
    const AMBA_REC_BOX_s     *pRbx;
    const AMBA_REC_DST_s     *pRdt;
    const SVC_REC_FWK_CFG_s  *pCfg;
    const REC_MAIN_FWK_s     *pFwk;

    if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_FWK_LOADED)) {
        g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_FWK_LOADED);

        for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[i]);
            pFwk = &(g_RcmCtrl.RecFwk[i]);

            /* destroy record master */
            if (0U < pCfg->NumRsc) {
                Rval = AmbaRecMaster_Destroy(&(pFwk->Rma));
                if (Rval != SVC_OK) {
                    SvcLog_NG(LOG_RCM, "fail to destroy master", 0U, 0U);
                }

                /* destroy record source */
                for (c = 0U; c < pCfg->NumRsc; c++) {
                    pRsc = &(pFwk->RscArr[c]);

                    switch (pRsc->UsrCfg.SrcType) {
                    case AMBA_RSC_TYPE_VIDEO:
                    case AMBA_RSC_TYPE_AUDIO:
                        Rval = AmbaRscVA_Destroy(pRsc);
                        if (Rval != SVC_OK) {
                            SvcLog_NG(LOG_RCM, "fail to destroy source", 0U, 0U);
                        }
                        break;
                    case AMBA_RSC_TYPE_DATA:
#if defined(CONFIG_ICAM_DATA_GATHER_USED)
                        Rval = AmbaRscData_Destroy(pRsc);
                        if (Rval == SVC_OK) {
                            Rval = SvcDataGather_UnRegister(g_RcmCtrl.DataGCtrl[i]);
                        }

                        if (Rval != SVC_OK) {
                            SvcLog_NG(LOG_RCM, "fail to destroy source", 0U, 0U);
                        }
#endif
                        break;
                    default:
                        /* unknown source type */
                        break;
                    }
                }

                /* destroy record box */
                for (c = 0U; c < pCfg->NumRbx; c++) {
                    pRbx = &(pFwk->RbxArr[c]);

                    if (pRbx->UsrCfg.BoxType == AMBA_RBX_TYPE_MP4) {
                        Rval = AmbaRbxMP4_Destroy(pRbx);
                        if (Rval != SVC_OK) {
                            SvcLog_NG(LOG_RCM, "fail to destroy box", 0U, 0U);
                        }
                    }
                }

                if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE) == 0U) {
                    /* destroy record destination */
                    for (c = 0U; c < pCfg->NumRdt; c++) {
                        pRdt = &(pFwk->RdtArr[c]);

                        if (pRdt->UsrCfg.DstType == AMBA_RDT_TYPE_FILE) {
                            Rval = AmbaRdtFile_Destroy(pRdt);
                        } else if ((pRdt->UsrCfg.DstType == AMBA_RDT_TYPE_EVT) &&
                                (pRdt->UsrCfg.SubType == AMBA_RDT_STYPE_NML)) {
                            Rval = AmbaRdtNmlEvt_Destroy(pRdt);
                        } else if ((pRdt->UsrCfg.DstType == AMBA_RDT_TYPE_EVT) &&
                                (pRdt->UsrCfg.SubType == AMBA_RDT_STYPE_EMG)) {
                            Rval = AmbaRdtEmgEvt_Destroy(pRdt);
                        } else if (pRdt->UsrCfg.DstType == AMBA_RDT_TYPE_NET) {
                            Rval = AmbaRdtNet_Destroy(pRdt);
                        } else {
                            SvcLog_NG(LOG_RCM, "Unknown destination type", 0U, 0U);
                            Rval = SVC_NG;
                        }

                        if (Rval != SVC_OK) {
                            SvcLog_NG(LOG_RCM, "fail to destroy destination", 0U, 0U);
                        }
                    }
                }
            }
        }

        /* destroy qp ctrl task */
        if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_QPCTRL_EXIST)) {
            if (SVC_OK == SvcRecQpCtrlDestroy()) {
                g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_QPCTRL_EXIST);
            }
        }

        /* destroy blend task */
        if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_BLEND_EXIST)) {
            if (SVC_OK == SvcRecBlend_Destroy()) {
                g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_BLEND_EXIST);
            }
        }

        /* destroy record monitor task */
        RecMonTaskDestroy();

        /* destroy BitsDataMutex */
        if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_BITSMTX_EXIST)) {
            if (SVC_OK == AmbaKAL_MutexDelete(&(g_RcmCtrl.BitsDataMutex))) {
                g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_BITSMTX_EXIST);
            }
        }

#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
        /* destroy record MV task */
        RecMVTaskDestroy();
#endif


#if defined(CONFIG_ICAM_TIMELAPSE_USED)
        /* destroy time-lapse tasks */
        for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[i]);
            if (pCfg->IsTimeLapse > 0U) {
                Rval = SvcTimeLapse_TaskDelete();
                if (SVC_OK != Rval) {
                    SvcLog_NG(LOG_RCM, "SvcTimeLapse_TaskDelete failed", 0U, 0U);
                }
                break;
            }
        }
#endif

        /* check if there is audio source */
        g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_AUDIO_EXIST);

    } else {
        SvcLog_DBG(LOG_RCM, "nothing to unload", 0U, 0U);
    }
}

/**
* record start
* @param [in] StartBits stream bits
* @param [in] IsBootToRec boot to record (0: No, 1: Yes)
*/
void SvcRecMain_Start(UINT32 StartBits, UINT32 IsBootToRec)
{
    UINT16                             StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];
    UINT32                             i, c, Rval, NumStrm, TLapse = 0U;
    UINT32                             BankSyncSize, VThmBits, Err;
    AMBA_DSP_VIDEO_ENC_START_CONFIG_s  StaArr[CONFIG_ICAM_MAX_REC_STRM];
    const SVC_REC_FWK_CFG_s            *pCfg;
    REC_MAIN_FWK_s                     *pFwk;

    if (StartBits & 0x1) {
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_BUSY;
    }
    if (StartBits & 0x2) {
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].preview_status = REC_STATUS_BUSY;
    }
    if (StartBits & 0x4) {
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_BUSY;
    }
    if (StartBits & 0x8) {
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].preview_status = REC_STATUS_BUSY;
    }
  
    AmbaMisra_TouchUnused(&Rval);
    if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_FWK_LOADED)) {
        Rval = SVC_OK;
        AmbaMisra_TouchUnused(&Rval);
        SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StartBits, &NumStrm, StrmIdxArr);

        if (g_RcmCtrl.StartBits == 0U) {
            if (IsBootToRec > 0U) {
                Rval = SVC_OK;
            } else {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                if (tolower(REC_DEST_DISK[0]) == tolower(SD_SLOT[0]) && app_helper.check_sd_exist())
#endif
                {
                    Rval = InitFreeSpaceCheck(NumStrm, StrmIdxArr);
                }
            }

            if (Rval == SVC_OK) {
                /* enable codec clock */
                CodecClockCtrl(1U);

                /* enable record monitor task */
                Err = AmbaKAL_EventFlagSet(&(g_RcmCtrl.RecMonFlag), REC_MON_FLAG_ENABLE);
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(LOG_RCM, "AmbaKAL_EventFlagSet REC_MON_FLAG_ENABLE failed %u", Err, 0U);
                }
            }
        }

        if (Rval == SVC_OK) {
            VThmBits = 0;
            for (i = 0U; i < NumStrm; i++) {
                pCfg = &(g_RcmCtrl.UsrCfg[StrmIdxArr[i]]);
                pFwk = &(g_RcmCtrl.RecFwk[StrmIdxArr[i]]);

                /* reset BitsDataSize */
                for (c = 0U; c < (UINT32)AMBA_RSC_TYPE_NUM; c++) {
                    g_RcmCtrl.BitsDataSize[StrmIdxArr[i]][c] = 0U;
                }

                if (0U < pCfg->NumRsc) {
                    for (c = 0U; c < pCfg->NumRsc; c++) {
                        switch (pFwk->RscArr[c].UsrCfg.SrcType) {
                        case AMBA_RSC_TYPE_VIDEO:
                        case AMBA_RSC_TYPE_AUDIO:
                            AmbaRscVA_Control(&(pFwk->RscArr[c]), AMBA_RSC_VA_RESET, NULL);
                            break;
                        case AMBA_RSC_TYPE_DATA:
#if defined(CONFIG_ICAM_DATA_GATHER_USED)
                            AmbaRscData_Control(&(pFwk->RscArr[c]), AMBA_RSC_DATA_RESET, NULL);
                            g_RcmCtrl.DataGSync[StrmIdxArr[i]] = 0U;
#endif
                            break;
                        default:
                            /* do nothing */
                            break;
                        }
                    }

                    for (c = 0U; c < pCfg->NumRbx; c++) {
                        AmbaRbxMP4_Control(&(pFwk->RbxArr[c]), AMBA_RBXC_MP4_HEADRESET, NULL);
                    }

                    if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE) == 0U) {
                        for (c = 0U; c < pCfg->NumRdt; c++) {
                            if (pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_FILE) {
                                VThmBits     = SetBits(VThmBits, ((UINT32)0x01U << StrmIdxArr[i]));

                                BankSyncSize = BankSyncSizeCalc(StrmIdxArr[i]);

                                if (IsBootToRec > 0U) {
                                    pFwk->RdtArr[c].UsrCfg.BootToRec = 1U;
                                } else {
                                    pFwk->RdtArr[c].UsrCfg.BootToRec = 0U;
                                }

                                AmbaRdtFile_Control(&(pFwk->RdtArr[c]), AMBA_RDTC_FILE_OPEN, &BankSyncSize);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                                {
                                    extern void UpdateVideoStartTime(int streamId);
                                    AMBA_RDT_FILE_PRIV_s *pPriv = (AMBA_RDT_FILE_PRIV_s *)(pFwk->RdtArr[c].PrivData);
                                    UpdateVideoStartTime(pFwk->RdtArr[c].UsrCfg.StreamID);
                                    imu_record_control(IMU_RECORD_FILE_OPEN, pFwk->RdtArr[c].UsrCfg.StreamID, pPriv->FileName, NULL);
                                    gnss_record_control(GNSS_RECORD_FILE_OPEN, pPriv->FileName, NULL);
                                }
#endif
                            } else if ((pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_EVT) &&
                                    (pFwk->RdtArr[c].UsrCfg.SubType == AMBA_RDT_STYPE_NML)) {
                                AmbaRdtNmlEvt_Control(&(pFwk->RdtArr[c]), AMBA_RDTC_NMLEVT_OPEN, NULL);
                            } else if ((pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_EVT) &&
                                    (pFwk->RdtArr[c].UsrCfg.SubType == AMBA_RDT_STYPE_EMG)) {

                                BankSyncSize = BankSyncSizeCalc(StrmIdxArr[i]);

                                AmbaRdtEmgEvt_Control(&(pFwk->RdtArr[c]), AMBA_RDTC_EMGEVT_OPEN, &BankSyncSize);
                            } else if (pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_NET) {
                                if (pFwk->RdtArr[c].UsrCfg.SubType == AMBA_RDT_STYPE_RTSP) {
#if defined(CONFIG_AMBA_RTSP)
                                    RTSPSetup(StrmIdxArr[i]);
#endif
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                                    AmbaRdtNet_Control(&(pFwk->RdtArr[c]), AMBA_RDT_NET_RTSP_START, NULL);
#endif
                                } else {
                                    SvcLog_NG(LOG_RCM, "Unknown destination subtype", 0U, 0U);
                                }
                            } else {
                                SvcLog_NG(LOG_RCM, "Unknown destination type", 0U, 0U);
                            }

                        }
                    }
                    AmbaRecMaster_Control(&(pFwk->Rma), AMBA_RMAC_GO, NULL);
                }
            }

            /* set start flag before starting encoders */
            g_RcmCtrl.StartBits = SetBits(g_RcmCtrl.StartBits, StartBits);

#ifdef CONFIG_ICAM_AUDIO_USED
            if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_AUDIO_EXIST)) {
                if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_AUDIO_START) == 0U) {
                    g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_AUDIO_START);
                    AmbaAEncFlow_Start(AinCtrl, 1U, g_RcmCtrl.NumAEnc);
                }
            }
#endif

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            for (i = 0U; i < NumStrm; i++) {
                pCfg = &(g_RcmCtrl.UsrCfg[StrmIdxArr[i]]);
                pFwk = &(g_RcmCtrl.RecFwk[StrmIdxArr[i]]);
                for (c = 0U; c < pCfg->NumRdt; c++) {
                    if (pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_NET) {
                        if (pFwk->RdtArr[c].UsrCfg.SubType == AMBA_RDT_STYPE_RTSP) {
                            AmbaRdtNet_Control(&(pFwk->RdtArr[c]), AMBA_RDT_NET_RTSP_START, NULL);
                        }
                    }
                }
            }
#endif
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
            /* do video thumbnail capture */
            if ((g_RcmCtrl.pfnCapThm != NULL) && (IsBootToRec == 0U)) {
                g_RcmCtrl.pfnCapThm(VThmBits);
            }
#endif
            /* rec blend task start */
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            SvcRecBlend_Start(StartBits & 0x0F);
#else
            SvcDrawStopwatch_TimerReset(g_RcmCtrl.BlendBits & StartBits);
            SvcRecBlend_Start(g_RcmCtrl.BlendBits & StartBits);
#endif
            /* record start */
            SvcEnc_Config();
            g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_VIDEO_START_PREP);

            AmbaSvcWrap_MisraMemset(StaArr, 0, sizeof(StaArr));
            SvcEnc_Start((UINT16)NumStrm, StrmIdxArr, StaArr);

#if defined(CONFIG_ICAM_DATA_GATHER_USED)
            /* open data gather */
            if (0U < g_RcmCtrl.DataBits) {
                DataGatherOpen((StartBits & g_RcmCtrl.DataBits));
            }
#endif

            /* check if timelapse */
            for (i = 0U; i < NumStrm; i++) {
                g_RcmCtrl.TLapseCapTime[StrmIdxArr[i]] = 0U;
                pCfg = &(g_RcmCtrl.UsrCfg[StrmIdxArr[i]]);
                if (pCfg->IsTimeLapse == 1U) {
                    TLapse = 1U;

                }
            }

            if (TLapse == 1U) {
#if defined(CONFIG_ICAM_TIMELAPSE_USED)
                SvcTimeLapse_Start(StartBits);
#endif
            } else {
                /* no dynamic QP control if timelpase */
                if (g_RcmCtrl.BitsCmpStrmBits == 0U) {
                    SvcRecQpCtrlStart(StartBits);
                }
            }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            rec_dvr_record_started_handler(StartBits);
#endif

        }
    } else {
        SvcLog_DBG(LOG_RCM, "resouces aren't loaded", 0U, 0U);
    }

    if (StartBits & 0x01) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status != REC_STATUS_RECORD) {
            rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_IDLE;
        }
    }
    if (StartBits & 0x02) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].preview_status != REC_STATUS_VF) {
            rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].preview_status = REC_STATUS_IDLE;
        }
    }
    if (StartBits & 0x04) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status != REC_STATUS_RECORD) {
            rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_IDLE;
        }
    }
    if (StartBits & 0x08) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].preview_status != REC_STATUS_VF) {
            rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].preview_status = REC_STATUS_IDLE;
        }
    }
}

/**
* record stop
* @param [in] StopBits stream bits
* @param [in] IsEmgStop Yes/No (1U/0U)
*/
void SvcRecMain_Stop(UINT32 StopBits, UINT32 IsEmgStop)
{
    UINT8                    StopOptArr[CONFIG_ICAM_MAX_REC_STRM] = {0};
    UINT16                   StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];
    UINT32                   i, c, NumStrm, StartBits, TimeOut, TLapse = 0U, Xcode = 0U, Pbk = 0U, Err, StreamBits = StopBits;
    const SVC_REC_FWK_CFG_s  *pCfg;
    const REC_MAIN_FWK_s     *pFwk;
    
    if (StopBits & 0x1) {
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_BUSY;
    }
    if (StopBits & 0x4) {
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_BUSY;
    }

    StreamBits &= g_RcmCtrl.StartBits;
    if ((IsEmgStop == 0U) && (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_VIDEO_START_PREP))) {
        SvcLog_DBG(LOG_RCM, "encoders are still starting up", 0U, 0U);
    } else if (0U < StreamBits) {
        SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StreamBits, &NumStrm, StrmIdxArr);
        StartBits = ClearBits(g_RcmCtrl.StartBits, StreamBits);

        if (KAL_ERR_NONE != AmbaKAL_EventFlagClear(&(g_RcmCtrl.StartFlag), StreamBits)) {
            SvcLog_NG(LOG_RCM, "fail to clear event flag", 0U, 0U);
        }

        for (i = 0U; i < NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[StrmIdxArr[i]]);
            if (pCfg->IsTimeLapse == 1U) {
                TLapse = 1U;
            }
            if (pCfg->IsXcode == 1U) {
                Xcode = 1U;
            }
        }

        if (IsEmgStop == 1U) {
            for (i = 0U; i < NumStrm; i++) {
                pCfg = &(g_RcmCtrl.UsrCfg[StrmIdxArr[i]]);
                pFwk = &(g_RcmCtrl.RecFwk[StrmIdxArr[i]]);

                if (0U < pCfg->NumRsc) {
                    if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE) == 0U) {
                        for (c = 0U; c < pCfg->NumRdt; c++) {
                            if (pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_FILE) {
                                AmbaRdtFile_Control(&(pFwk->RdtArr[c]), AMBA_RDTC_FILE_STOP, NULL);
                            }
                        }
                    }
                }
            }
        }

#if defined(CONFIG_ICAM_PLAYBACK_USED)
        {
            if (SVC_OK != AmbaPlayer_Control(NULL, AMBA_PLAYER_CTRL_INFO_GET, &Pbk)) {
                SvcLog_NG(LOG_RCM, "fail to get player bits", 0U, 0U);
            }
        }
#else
        AmbaMisra_TouchUnused(&Pbk);
#endif

        if (TLapse == 1U) {
#if defined(CONFIG_ICAM_TIMELAPSE_USED)
            SvcTimeLapse_Stop(StreamBits);
#endif
        } else {
            SvcRecQpCtrlStop(StreamBits);
        }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        SvcRecBlend_Stop(StopBits & 0x0F);
#else
        SvcRecBlend_Stop(g_RcmCtrl.BlendBits & StreamBits);
#endif
        for (i = 0U; i < NumStrm; i++) {
            StopOptArr[i] = AMBA_DSP_VIDEO_ENC_STOP_IMMEDIATELY;
        }

        SvcEnc_Stop((UINT16)NumStrm, StrmIdxArr, StopOptArr);

        /* stop audio and text after video eos */
        for (i = 0U; i < NumStrm; i++) {
            TimeOut = RCM_EOS_TMO;
            pFwk    = &(g_RcmCtrl.RecFwk[StrmIdxArr[i]]);
            AmbaRecMaster_Control(&(pFwk->Rma), AMBA_RMAC_WAIT_VEOS, &TimeOut);
        }

        /* close data gather */
#if defined(CONFIG_ICAM_DATA_GATHER_USED)
        if (0U < g_RcmCtrl.DataBits) {
            DataGatherClose((StreamBits & g_RcmCtrl.DataBits));
        }
#endif

#ifdef CONFIG_ICAM_AUDIO_USED
        if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_AUDIO_EXIST)) {
            if (StartBits == 0U) {
                if (0U < CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_AUDIO_START)) {
                    g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_AUDIO_START);
                    AmbaAEncFlow_Stop(AinCtrl, 1U, g_RcmCtrl.NumAEnc);
                }
            } else {
                for (i = 0U; i < NumStrm; i++) {
                    pCfg = &(g_RcmCtrl.UsrCfg[StrmIdxArr[i]]);
                    pFwk = &(g_RcmCtrl.RecFwk[StrmIdxArr[i]]);

                    /* insert dummy eos for audio if all streams aren't closed */
                    for (c = 0U; c < pCfg->NumRsc; c++) {
                        if (pCfg->RscCfgArr[c]->SrcType == AMBA_RSC_TYPE_AUDIO) {
                            AmbaRscVA_Control(&(pFwk->RscArr[c]), AMBA_RSC_VA_DUMMY_EOS, NULL);
                        }
                    }
                }
            }
        }
#endif

        /* close file */
        TimeOut = RCM_EOS_TMO;
        for (i = 0U; i < NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[StrmIdxArr[i]]);
            pFwk = &(g_RcmCtrl.RecFwk[StrmIdxArr[i]]);

            if (0U < pCfg->NumRsc) {
                /* check eos */
                AmbaRecMaster_Control(&(pFwk->Rma), AMBA_RMAC_HALT, &TimeOut);

                if (CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE) == 0U) {
                    /* close file */
                    for (c = 0U; c < pCfg->NumRdt; c++) {
                        if (pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_FILE) {
                            AmbaRdtFile_Control(&(pFwk->RdtArr[c]), AMBA_RDTC_FILE_CLOSE, NULL);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                            {
                                AMBA_RDT_FILE_PRIV_s *pPriv = (AMBA_RDT_FILE_PRIV_s *)pFwk->RdtArr[c].PrivData;
                                AmbaRdtFile_Control(&(pFwk->RdtArr[c]), AMBA_RDTC_FILE_CLOSE, NULL);
                                imu_record_control(IMU_RECORD_FILE_CLOSE, pFwk->RdtArr[c].UsrCfg.StreamID, pPriv->FileName, NULL);
                                gnss_record_control(GNSS_RECORD_FILE_CLOSE, pPriv->FileName, NULL);
                            }
#endif
                        } else if ((pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_EVT) &&
                                (pFwk->RdtArr[c].UsrCfg.SubType == AMBA_RDT_STYPE_NML)) {
                            AmbaRdtNmlEvt_Control(&(pFwk->RdtArr[c]), AMBA_RDTC_NMLEVT_CLOSE, NULL);
                        } else if ((pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_EVT) &&
                                (pFwk->RdtArr[c].UsrCfg.SubType == AMBA_RDT_STYPE_EMG)) {
                            AmbaRdtEmgEvt_Control(&(pFwk->RdtArr[c]), AMBA_RDTC_EMGEVT_CLOSE, NULL);
                        } else if (pFwk->RdtArr[c].UsrCfg.DstType == AMBA_RDT_TYPE_NET) {
                            if (pFwk->RdtArr[c].UsrCfg.SubType == AMBA_RDT_STYPE_RTSP) {
                                AmbaRdtNet_Control(&(pFwk->RdtArr[c]), AMBA_RDT_NET_RTSP_STOP, NULL);
                            } else {
                                SvcLog_NG(LOG_RCM, "Unknown destination subtype", 0U, 0U);
                            }
                        } else {
                            SvcLog_NG(LOG_RCM, "Unknown destination type", 0U, 0U);
                        }
                    }
                }
            }
        }

        g_RcmCtrl.StartBits = StartBits;
        if ((StartBits == 0U) && (Xcode == 0U) && (Pbk == 0U)) {
            /* disable codec clock */
            CodecClockCtrl(0U);
        }

        if (StartBits == 0U) {
            /* disable record monitor task */
            Err = AmbaKAL_EventFlagClear(&(g_RcmCtrl.RecMonFlag), REC_MON_FLAG_ENABLE);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(LOG_RCM, "AmbaKAL_EventFlagClear REC_MON_FLAG_ENABLE failed %u", Err, 0U);
            }
        }

        SvcLog_OK(LOG_RCM, "record stop successfully(%u)", StreamBits, 0U);
    } else {
        SvcLog_DBG(LOG_RCM, "encoders are in idle", 0U, 0U);
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    rec_dvr_record_stopped_handler(StopBits);
#endif
    
    if (StopBits & 0x1) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status != REC_STATUS_IDLE) {
            rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_RECORD;
        }
    }
    if (StopBits & 0x4) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status != REC_STATUS_IDLE) {
            rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_RECORD;
        }
    }

}

/**
* record main control
* @param [in] ParamType parameter type
* @param [in] NumStrm number of stream
* @param [in] pStreamIdx stream index
* @param [in] pNewVal pointer to new value
*/
void SvcRecMain_Control(UINT32 ParamType,
                        UINT16 NumStrm,
                        const UINT16 *pStreamIdx,
                        const void *pNewVal)
{
    UINT32                   i, c, SrcType = AMBA_RSC_TYPE_NUM, Bits = 0x01U, Num;
    const SVC_REC_FWK_CFG_s  *pCfg;
    REC_MAIN_FWK_s           *pFwk;
    AMBA_RSC_STATIS_s         *pRscStatis = NULL;
    UINT32                   *pVal;

    switch (ParamType) {
    case SVC_RCM_PMT_NLWR_SET:
        AmbaMisra_TypeCast(&pVal, &pNewVal);
        if (0U < (*pVal)) {
            g_RcmCtrl.RcmFlags = SetBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE);
        } else {
            g_RcmCtrl.RcmFlags = ClearBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE);
        }
        break;
    case SVC_RCM_PMT_NLWR_GET:
        AmbaMisra_TypeCast(&pVal, &pNewVal);
        (*pVal) = CheckBits(g_RcmCtrl.RcmFlags, RCM_FLAG_NULL_WRITE);
        break;
    case SVC_RCM_RBX_IN:
        AmbaMisra_TypeCast(&pVal, &pNewVal);
        for (i = 0; i < NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[pStreamIdx[i]]);
            pFwk = &(g_RcmCtrl.RecFwk[pStreamIdx[i]]);

            for (c = 0U; c < pCfg->NumRbx; c++) {
                pFwk->RbxArr[c].UsrCfg.InSrcBits = (*pVal);
            }
        }
        break;
    case SVC_RCM_RDT_SRCIN:
        AmbaMisra_TypeCast(&pVal, &pNewVal);
        for (i = 0; i < NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[pStreamIdx[i]]);
            pFwk = &(g_RcmCtrl.RecFwk[pStreamIdx[i]]);

            for (c = 0U; c < pCfg->NumRdt; c++) {
                pFwk->RdtArr[c].UsrCfg.InSrcBits = (*pVal);
                pFwk->RdtArr[c].UsrCfg.InBoxBits = 0U;
            }
        }
        break;
    case SVC_RCM_RDT_BOXIN:
        AmbaMisra_TypeCast(&pVal, &pNewVal);
        for (i = 0; i < NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[pStreamIdx[i]]);
            pFwk = &(g_RcmCtrl.RecFwk[pStreamIdx[i]]);

            for (c = 0U; c < pCfg->NumRdt; c++) {
                pFwk->RdtArr[c].UsrCfg.InSrcBits = 0U;
                pFwk->RdtArr[c].UsrCfg.InBoxBits = (*pVal);
            }
        }
        break;
    case SVC_RCM_PMT_VDCODING:
    case SVC_RCM_PMT_AUCODING:
    case SVC_RCM_PMT_DACODING:
        if (ParamType == SVC_RCM_PMT_VDCODING) {
            SrcType = AMBA_RSC_TYPE_VIDEO;
            Num = NumStrm;
        } else if (ParamType == SVC_RCM_PMT_AUCODING) {
            SrcType = AMBA_RSC_TYPE_AUDIO;
            Num = g_RcmCtrl.NumStrm;
        } else {
            SrcType = AMBA_RSC_TYPE_DATA;
            Num = NumStrm;
        }

        AmbaMisra_TypeCast(&pVal, &pNewVal);

        for (i = 0; i < Num; i++) {
            if (ParamType == SVC_RCM_PMT_VDCODING) {
                pCfg = &(g_RcmCtrl.UsrCfg[pStreamIdx[i]]);
                pFwk = &(g_RcmCtrl.RecFwk[pStreamIdx[i]]);
            } else if (ParamType == SVC_RCM_PMT_AUCODING) {
                pCfg = &(g_RcmCtrl.UsrCfg[i]);
                pFwk = &(g_RcmCtrl.RecFwk[i]);
            } else {
                pCfg = &(g_RcmCtrl.UsrCfg[i]);
                pFwk = &(g_RcmCtrl.RecFwk[i]);
            }

            AmbaMisra_TouchUnused(&SrcType);

            for (c = 0U; c < pCfg->NumRsc; c++) {
                if (pFwk->RscArr[c].UsrCfg.SrcType == SrcType) {
                    pFwk->RscArr[c].UsrCfg.SubType = (*pVal);
                }
            }

            for (c = 0U; c < pCfg->NumRbx; c++) {
                switch (SrcType) {
                case AMBA_RSC_TYPE_VIDEO:
                    pFwk->RbxArr[c].UsrCfg.VdCoding = (*pVal);
                    AmbaRbxMP4_Control(&(pFwk->RbxArr[c]), AMBA_RBXC_MP4_VDCODING, pVal);
                    break;
                case AMBA_RSC_TYPE_AUDIO:
                    if (pFwk->RbxArr[c].UsrCfg.AuCoding != AMBA_RSC_AUD_SUBTYPE_NONE) {
                        pFwk->RbxArr[c].UsrCfg.AuCoding = (*pVal);
                        AmbaRbxMP4_Control(&(pFwk->RbxArr[c]), AMBA_RBXC_MP4_AUCODING, pVal);
                    }
                    break;
                case AMBA_RSC_TYPE_DATA:
                default:
                    /* do nothing */
                    break;
                }
            }
        }
        break;
    case SVC_RCM_GET_VID_STATIS:
    case SVC_RCM_GET_AUD_STATIS:
    case SVC_RCM_GET_DAT_STATIS:
        if (ParamType == SVC_RCM_GET_VID_STATIS) {
            SrcType = AMBA_RSC_TYPE_VIDEO;
        } else if (ParamType == SVC_RCM_GET_AUD_STATIS) {
            SrcType = AMBA_RSC_TYPE_AUDIO;
        } else {
            SrcType = AMBA_RSC_TYPE_DATA;
        }

        AmbaMisra_TypeCast(&pRscStatis, &pNewVal);
        for (i = 0U; i < NumStrm; i++) {
            pCfg = &(g_RcmCtrl.UsrCfg[pStreamIdx[i]]);
            pFwk = &(g_RcmCtrl.RecFwk[pStreamIdx[i]]);

            AmbaMisra_TouchUnused(&SrcType);

            if (0U < CheckBits(g_RcmCtrl.StartBits, (Bits << pStreamIdx[i]))) {
                for (c = 0U; c < pCfg->NumRsc; c++) {
                    if (pFwk->RscArr[c].UsrCfg.SrcType == SrcType) {
                        switch (SrcType) {
                        case AMBA_RSC_TYPE_VIDEO:
                        case AMBA_RSC_TYPE_AUDIO:
                            AmbaRscVA_Control(&(pFwk->RscArr[c]),
                                              AMBA_RSC_VA_STATIS_GET,
                                              &pRscStatis[i]);
                            break;
                        case AMBA_RSC_TYPE_DATA:
                            AmbaRscData_Control(&(pFwk->RscArr[c]),
                                                AMBA_RSC_DATA_STATIS_GET,
                                                &pRscStatis[i]);
                            break;
                        default:
                            /* do nothing */
                            break;
                        }
                        break;
                    }
                }
            }
        }
        break;
    case SVC_RCM_GET_ENC_STATUS:
        AmbaMisra_TypeCast(&pVal, &pNewVal);
        (*pVal) = g_RcmCtrl.StartBits;
        break;
    case SVC_RCM_SET_BLEND:
        AmbaMisra_TypeCast(&pVal, &pNewVal);
        g_RcmCtrl.BlendBits = *pVal;
        break;
    case SVC_RCM_SET_SCH_VIDOFF:
        {
            AMBA_RMAC_SCHOFF_PARAM  SchOff;

            AmbaMisra_TypeCast(&pVal, &pNewVal);

            for (i = 0U; i < NumStrm; i++) {
                pCfg = &(g_RcmCtrl.UsrCfg[pStreamIdx[i]]);
                pFwk = &(g_RcmCtrl.RecFwk[pStreamIdx[i]]);

                for (c = 0U; c < pCfg->NumRsc; c++) {
                    if (pFwk->RscArr[c].UsrCfg.SrcType == AMBA_RSC_TYPE_VIDEO) {
                        SchOff.SrcBits        = pFwk->RscArr[c].SrcBit;
                        SchOff.SchFrameOffset = *pVal;
                        AmbaRecMaster_Control(&(pFwk->Rma), AMBA_RMAC_SCH_OFFSET, &SchOff);
                        break;
                    }
                }
            }
        }
        break;
    case SVC_RCM_BITSCMP_ON:
        if (g_RcmCtrl.BitsCmpStrmBits == 0U) {
            UINT32                      Err, Bit = 1U;
            SVC_DATA_CMP_TASK_CREATE_s  Create;

            Create.CmpNum       = (UINT8)NumStrm;
            Create.DataNum      = 2U;
            Create.TaskPriority = g_RcmCtrl.BitsCmpPriority;
            Create.TaskCpuBits  = g_RcmCtrl.BitsCmpCpuBits;

            for (i = 0; i < NumStrm; i++) {
                g_RcmCtrl.BitsCmpStrmBits = SetBits(g_RcmCtrl.BitsCmpStrmBits, (Bit << (UINT32)(pStreamIdx[i])));
            }

            Err = SvcDataCmp_Create(&(g_RcmCtrl.BitsCmpHdlr), &Create);
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcDataCmp_Create failed %u", Err, 0U);
            }
        }
        break;
    case SVC_RCM_BITSCMP_OFF:
        if (g_RcmCtrl.BitsCmpStrmBits != 0U) {
            UINT32                      Err;

            Err = SvcDataCmp_Delete(&(g_RcmCtrl.BitsCmpHdlr));
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcDataCmp_Delete failed %u", Err, 0U);
            } else {
                g_RcmCtrl.BitsCmpStrmBits = 0U;
            }
        }
        break;
    case SVC_RCM_BITSCMP_INFO:
        if (g_RcmCtrl.BitsCmpStrmBits != 0U) {
            UINT32                      Err, *pValue;
            SVC_DATA_CMP_REPORT_s       Report = {0};
            Err = SvcDataCmp_ReportGet(&(g_RcmCtrl.BitsCmpHdlr), &Report);
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcDataCmp_ReportGet failed %u", Err, 0U);
            } else {
                SvcLog_OK(LOG_RCM, "Video encode bitstream comparison: ErrCount %u, CmpCount %u", (UINT32)Report.ErrCount, (UINT32)Report.CmpCount);
            }

            if (pNewVal != NULL){
                AmbaMisra_TypeCast(&pValue, &pNewVal);
                if(Report.ErrCount != 0U){
                    *pValue = 1U;
                    SvcRecMain_Stop(g_RcmCtrl.StartBits,0U);
                    Err = SvcDataCmp_Delete(&(g_RcmCtrl.BitsCmpHdlr));
                    if (Err != SVC_OK) {
                        SvcLog_NG(LOG_RCM, "SvcDataCmp_Delete failed %u", Err, 0U);
                    } else {
                        g_RcmCtrl.BitsCmpStrmBits = 0U;
                    }
                } else {
                    *pValue = 0U;
                }
            }
        }
        break;
    case SVC_RCM_SHMOO_CTRL:
        if (g_RcmCtrl.BitsCmpStrmBits != 0U) {
            UINT32                      Err, *pValue, IsStop=0U;
            SVC_DATA_CMP_REPORT_s       Report;
            static UINT32 prevCount = 0U, zeroCount = 0U;

            Err = SvcDataCmp_ReportGet(&(g_RcmCtrl.BitsCmpHdlr), &Report);
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_RCM, "SvcDataCmp_ReportGet failed %u", Err, 0U);
            } else {
                if(pNewVal != NULL){
                    AmbaMisra_TypeCast(&pValue, &pNewVal);

                    if((UINT32)Report.CmpCount == 0U){
                        zeroCount = 1U;
                    }

                    if((zeroCount > 0U) || ( (prevCount > 0U) && (prevCount == (UINT32)Report.CmpCount) )){
                        *pValue |= 1U;
                        IsStop = 1U;
                        SvcLog_NG(LOG_RCM, "Dsp should assert now", 0U, 0U);
                    } else {
                        prevCount = (UINT32)Report.CmpCount;
                        *pValue = (Report.ErrCount == 0U) ? (*pValue | 0U) : (*pValue | 1U);
                    }
                }

                if((Report.ErrCount == 1U) || (IsStop == 1U) || (NumStrm == 0U)){
                    SvcRecMain_Stop(g_RcmCtrl.StartBits,0U);
                    Err = SvcDataCmp_Delete(&(g_RcmCtrl.BitsCmpHdlr));
                    if (Err != SVC_OK) {
                        SvcLog_NG(LOG_RCM, "SvcDataCmp_Delete failed %u", Err, 0U);
                    } else {
                        g_RcmCtrl.BitsCmpStrmBits = 0U;
                    }
                    SvcLog_OK(LOG_RCM, "Stop Video encode and bitstream comparison", 0U, 0U);
                }

                //SvcLog_OK(LOG_RCM, "Video encode bitstream comparison: ErrCount %u, CmpCount %u", (UINT32)Report.ErrCount, (UINT32)Report.CmpCount);

            }
        }
        break;

    default:
        /* do nothing */
        break;
    }
}

/**
* register record event callback
* @param [in] StreamBits record stream bits
* @param [in] Event event type
* @param [in] pfnEvtNotify callback function
*/
void SvcRecMain_EventCbRegister(UINT32 StreamBits, UINT32 Event, REC_EVENT_NOTIFY pfnEvtNotify)
{
    UINT32  NumStrm, i;
    UINT16  StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];

    SvcUtil_BitsToArr(CONFIG_ICAM_MAX_REC_STRM, StreamBits, &NumStrm, StrmIdxArr);
    if (Event < SVC_RCM_EVT_NUM) {
        for (i = 0U; i < NumStrm; i++) {
             g_RcmCtrl.RecEvtCb[StrmIdxArr[i]][Event] = pfnEvtNotify;
        }
    }
}

/**
* record dump
*/
void SvcRecMain_Dump(void)
{
    UINT32                   i, c;
    const SVC_REC_FWK_CFG_s  *pCfg;
    const REC_MAIN_FWK_s     *pFwk;
    const AMBA_REC_SRC_s      *pRsc;
    const AMBA_REC_BOX_s      *pRbx;
    const AMBA_REC_DST_s      *pRdt;
    const AMBA_REC_EVAL_s     *pMia;

    AmbaPrint_PrintStr5("## dump record main", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_PrintUInt5("num_strm: %u", g_RcmCtrl.NumStrm, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("flags   : 0x%08X", g_RcmCtrl.RcmFlags, 0U, 0U, 0U, 0U);

    AmbaPrint_PrintUInt5("[media]", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
        pMia = &(g_RcmCtrl.UsrCfg[i].MiaInfo);

        AmbaPrint_PrintUInt5("  [strm_%u]", i, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("    file_split_time   : %u", pMia->FileSplitTime, 0U, 0U, 0U, 0U);
        for (c = 0U; c < AMBA_REC_MIA_MAX; c++) {
            if (pMia->MiaCfg[c].BitRate == 0U) {
                continue;
            }

            AmbaPrint_PrintUInt5("    (track%u)", c, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("    bitrate: %u", pMia->MiaCfg[c].BitRate, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("    frate  : %u", pMia->MiaCfg[c].FrameRate, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("    ftick  : %u", pMia->MiaCfg[c].FrameTick, 0U, 0U, 0U, 0U);
            if (c == AMBA_REC_MIA_VIDEO) {
                AmbaPrint_PrintUInt5("    m      : %u", pMia->MiaCfg[c].RsvCfg[MIA_RSV_VIDEO_M], 0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("    n      : %u", pMia->MiaCfg[c].RsvCfg[MIA_RSV_VIDEO_N], 0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("    idr_it : %u", pMia->MiaCfg[c].RsvCfg[MIA_RSV_VIDEO_IDRIT], 0U, 0U, 0U, 0U);
            }
        }
    }

    AmbaPrint_PrintUInt5("[rsc/rbx/rdt num]", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
        pCfg = &(g_RcmCtrl.UsrCfg[i]);
        pFwk = &(g_RcmCtrl.RecFwk[i]);
        AmbaPrint_PrintUInt5("  [strm_%u]: %u/%u/%u", i, pCfg->NumRsc,
                                                         pCfg->NumRbx,
                                                         pCfg->NumRdt, 0U);
    }

    AmbaPrint_PrintUInt5("[rsc]", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
        pCfg = &(g_RcmCtrl.UsrCfg[i]);
        pFwk = &(g_RcmCtrl.RecFwk[i]);

        AmbaPrint_PrintUInt5("  [strm_%u]", i, 0U, 0U, 0U, 0U);
        for (c = 0U; c < pCfg->NumRsc; c++) {
            pRsc = &(pFwk->RscArr[c]);

            AmbaPrint_PrintUInt5("    [src_0x%X]", pRsc->SrcBit, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      src/sub type: %u/%u", pRsc->UsrCfg.SrcType,
                                                              pRsc->UsrCfg.SubType, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      mem_base/size: 0x%X/0x%X", (UINT32)pRsc->UsrCfg.MemBase,
                                                                   pRsc->UsrCfg.MemSize, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      bs_base/size: 0x%X/0x%X", (UINT32)pRsc->UsrCfg.BsBufBase,
                                                                  pRsc->UsrCfg.BsBufSize, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      scale/tick: %u/%u", pRsc->UsrCfg.TimeScale,
                                                            pRsc->UsrCfg.NumUnitsInTick, 0U, 0U, 0U);
        }
    }

    AmbaPrint_PrintUInt5("[rbx]", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
        pCfg = &(g_RcmCtrl.UsrCfg[i]);
        pFwk = &(g_RcmCtrl.RecFwk[i]);


        AmbaPrint_PrintUInt5("  [strm_%u]", i, 0U, 0U, 0U, 0U);
        for (c = 0U; c < pCfg->NumRbx; c++) {
            pRbx = &(pFwk->RbxArr[c]);

            AmbaPrint_PrintUInt5("    [box_0x%X]", pRbx->BoxBit, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      box/sub type: %u/%u", pRbx->UsrCfg.BoxType,
                                                              pRbx->UsrCfg.SubType, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      mem_base/size: 0x%X/0x%X", (UINT32)pRbx->UsrCfg.MemBase,
                                                                   pRbx->UsrCfg.MemSize, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      in_src_bits: 0x%X", pRbx->UsrCfg.InSrcBits, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("       [video]", 0U, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        coding: %u", pRbx->UsrCfg.VdCoding, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        w/h: %u/%u", pRbx->UsrCfg.VdWidth,
                                                       pRbx->UsrCfg.VdHeight, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        timescale: %u", pRbx->UsrCfg.VdTimeScale,
                                                          0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("       [audio]", 0U, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        coding: %u", pRbx->UsrCfg.AuCoding, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        volume: %u", pRbx->UsrCfg.AuVolume, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        bits_per_sample: %u", pRbx->UsrCfg.AuWBitsPerSample,
                                                                0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        channel: %u", pRbx->UsrCfg.AuChannels, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        bitrate: %u", pRbx->UsrCfg.AuBrate, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        sample_freq: %u", pRbx->UsrCfg.AuSample, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("        format: %u", pRbx->UsrCfg.AuFormat, 0U, 0U, 0U, 0U);
        }
    }

    AmbaPrint_PrintUInt5("[rdt]", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < g_RcmCtrl.NumStrm; i++) {
        pCfg = &(g_RcmCtrl.UsrCfg[i]);
        pFwk = &(g_RcmCtrl.RecFwk[i]);

        AmbaPrint_PrintUInt5("  [strm_%u]", i, 0U, 0U, 0U, 0U);
        for (c = 0U; c < pCfg->NumRdt; c++) {
            pRdt = &(pFwk->RdtArr[c]);

            AmbaPrint_PrintUInt5("    [dst_0x%X]", c, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      dst/sub type: %u/%u", pRdt->UsrCfg.DstType,
                                                              pRdt->UsrCfg.SubType, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      mem_base/size: 0x%X/0x%X", (UINT32)pRdt->UsrCfg.MemBase,
                                                                   pRdt->UsrCfg.MemSize, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      in_src_bits: 0x%X", pRdt->UsrCfg.InSrcBits, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("      in_box_bits: 0x%X", pRdt->UsrCfg.InBoxBits, 0U, 0U, 0U, 0U);
        }
    }
}
