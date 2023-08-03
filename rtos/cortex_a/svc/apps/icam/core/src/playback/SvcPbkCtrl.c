/**
 *  @file SvcPbkCtrl.c
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
 *  @details svc playback control
 *
 */

#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaDCF.h"
#include "AmbaUtility.h"
#include "AmbaRscData.h"
#include "AmbaVfs.h"
#include "AmbaShell.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAudio.h"
#include "AmbaCache.h"
#include "AmbaSensor.h"
#include "AmbaWrap.h"
#include "AmbaGDMA.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcStgMonitor.h"
#include "SvcTask.h"
#include "SvcOsd.h"
#include "SvcWinCalc.h"
#include "SvcClock.h"
#include "SvcRecMain.h"
#include "SvcVinSrc.h"
#include "SvcDataGather.h"
#include "SvcResCfg.h"
#include "SvcLiveview.h"
#include "SvcPlat.h"
#include "SvcIK.h"
#if defined(CONFIG_ICAM_ENCRYPTION)
#include "SvcEncrypt.h"
#endif
#include "SvcPbkCtrl.h"

#define SVC_LOG_PBK_CTRL       "PBKCTRL"
#define PLAYER_MAX_NUM         ((UINT32)CONFIG_AMBA_PLAYER_MAX_NUM)

#if defined(CONFIG_ICAM_ENCRYPTION)
typedef struct {
    /* input */
    ULONG  BufBase;
    ULONG  BufSize;
    ULONG  DataAddr;
    ULONG  DataSize;
    UINT32 EncodeType;
    UINT32 FrameType;
    /* output */
#define PLAYER_MAX_START_SEARCH_NUM (6U)
    UINT32 Num;
    ULONG  Offset[PLAYER_MAX_START_SEARCH_NUM];
    ULONG  Size[PLAYER_MAX_START_SEARCH_NUM];
} PLAYER_SEARCH_STARTCODE_s;
#endif

typedef struct {
    UINT32 PlayerId;
    UINT32 Status;
    void   *pParam;
} PLAYER_MSG_s;

typedef struct {
    UINT32 LastTick;
    UINT32 LastDecNum;
    UINT32 NewDecNum;
    UINT32 FeedNum;
    UINT32 Decstate;
} PLAYER_MONITOR_TRACK_s;

typedef struct {
    UINT32                   Running;
    PLAYER_MONITOR_TRACK_s   Video;
} PLAYER_MONITOR_s;

typedef struct {
#define MSG_MAX_NUM         (512U)
#define STACK_SIZE          (0x4000U)
    SVC_TASK_CTRL_s         Task;
    UINT8                   Stack[STACK_SIZE];

    PLAYER_MSG_s            MsgBuf[MSG_MAX_NUM];
    AMBA_KAL_MSG_QUEUE_t    MsgQue;

    AMBA_KAL_EVENT_FLAG_t   Flag;

    ULONG                   TextRp[PLAYER_MAX_NUM];
    UINT32                  TextFeedCnt[PLAYER_MAX_NUM];

#define MONITOR_MAX_INTERVAL (1000U)
    PLAYER_MONITOR_s        MonStatus[PLAYER_MAX_NUM];
} PLAYER_HDLR_s;

static SVC_PBK_CTRL_INFO_s     PbkCtrlInfo[PLAYER_MAX_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_PLAYER_s           Player[PLAYER_MAX_NUM] GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;
static void                    *pStrHdlr[PLAYER_MAX_NUM] GNU_SECTION_NOZEROINIT;
static PLAYER_HDLR_s           PlayerHdlr GNU_SECTION_NOZEROINIT;
static UINT32                  RunningBits = 0U, TaskCreate = 0U, TextEnable = 1U;
#if defined(CONFIG_ICAM_AUDIO_USED)
static UINT32                  AudioEnable = 1U;
#endif

static char  G_PbkCtrlQueName[32];
static char  G_PbkCtrlFlagName[] = "PbkCtrlFlag";

static void PlayerCB(AMBA_PLAYER_s *pPlayer, UINT32 Status, void *pParam)
{
    PLAYER_MSG_s  Msg;
    UINT32        i, Err;

    AmbaMisra_TouchUnused(pParam);
    AmbaMisra_TouchUnused(pPlayer);

    for (i = 0U; i < PLAYER_MAX_NUM; i++) {
        if (pPlayer == &(Player[i])) {
            Msg.PlayerId = i;
            Msg.Status   = Status;
            Msg.pParam   = pParam;

            Err = AmbaKAL_MsgQueueSend(&(PlayerHdlr.MsgQue), &Msg, AMBA_KAL_NO_WAIT);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_MsgQueueSend failed %u", Err, 0U);
            }

            break;
        }
    }
}

#if defined(CONFIG_ICAM_ENCRYPTION)
static void SearchStartOffset(PLAYER_SEARCH_STARTCODE_s *pSearch)
{
    ULONG                i = 0U, j, CmpCnt, Addr;
    const UINT8          *pVal, *pStartCode;
    static const UINT8   IdrAvc[5U]    = {0x00U, 0x00U, 0x00U, 0x01U, 0x65U};
    static const UINT8   IdrHevc[5U]   = {0x00U, 0x00U, 0x00U, 0x01U, 0x26U};
    static const UINT8   SliceAvc[5U]  = {0x00U, 0x00U, 0x00U, 0x01U, 0x61U};
    static const UINT8   SliceHevc[5U] = {0x00U, 0x00U, 0x00U, 0x01U, 0x02U};

    if ((pSearch->EncodeType == AMBA_FORMAT_MID_H265) || (pSearch->EncodeType == AMBA_FORMAT_MID_HVC)) {
        if (pSearch->FrameType == AMBA_PLAYER_FRAME_TYPE_IDR) {
            CmpCnt     = sizeof(IdrHevc);
            pStartCode = IdrHevc;
        } else {
            CmpCnt     = sizeof(SliceHevc);
            pStartCode = SliceHevc;
        }
    } else {
        if (pSearch->FrameType == AMBA_PLAYER_FRAME_TYPE_IDR) {
            CmpCnt     = sizeof(IdrAvc);
            pStartCode = IdrAvc;
        } else {
            CmpCnt     = sizeof(SliceAvc);
            pStartCode = SliceAvc;
        }
    }

    pSearch->Num = 0U;

    while ((i + CmpCnt) <= pSearch->DataSize) {
        for (j = 0U; j < CmpCnt; j++) {
            Addr = (pSearch->DataAddr + i) + j;
            if (Addr >= (pSearch->BufBase + pSearch->BufSize)) {
                Addr -= pSearch->BufSize;
            }
            AmbaMisra_TypeCast(&pVal, &Addr);

            if (*pVal == pStartCode[j]) {
                /**/
            } else {
                break;
            }
        }

        if (j == CmpCnt) {
            /* find a new start code */
            pSearch->Offset[pSearch->Num] = i;
            pSearch->Num++;
        }

        if (pSearch->Num >= PLAYER_MAX_START_SEARCH_NUM) {
            break;
        }

        i++;
    }

    for (i = 0U; i < (ULONG)pSearch->Num; i++) {
        if (i < (pSearch->Num - 1U)) {
            pSearch->Size[i] = pSearch->Offset[i + 1U] - pSearch->Offset[i];
        } else {
            pSearch->Size[i] = (pSearch->DataSize - pSearch->Offset[i]);
        }
    }
}

static void BitsDataDecrypt(AMBA_PLAYER_s *pPlayer, AMBA_PLAYER_DECRYPT_s *pInfo)
{
    UINT32                      Err, Rval = SVC_OK, i;
    ULONG                       OutputSize = 0U, AlignSize, MinSize;
    SVC_ENCRYPT_PROCESS_s       Process;
    PLAYER_SEARCH_STARTCODE_s   Search;

    AmbaMisra_TouchUnused(pPlayer);
    AmbaSvcWrap_MisraMemset(&Process, 0, sizeof(Process));
    AmbaSvcWrap_MisraMemset(&Search, 0, sizeof(Search));

    if ((pInfo->FrameType == AMBA_PLAYER_FRAME_TYPE_IDR) ||
        (pInfo->FrameType == AMBA_PLAYER_FRAME_TYPE_I) ||
        (pInfo->FrameType == AMBA_PLAYER_FRAME_TYPE_P) ||
        (pInfo->FrameType == AMBA_PLAYER_FRAME_TYPE_B)) {
        #define VIDEO_START_CODE_SIZE      (4U)    /* 0x00 0x00 0x00 0x01 */
        #define VIDEO_DECRYPT_OFFSET       (128U)
        #define VIDEO_DECRYPT_SIZE         (128U)
        #define VIDEO_H264_ALIGN           (32U)    /* muxer will add zero padding for frame alignment, SVC_H264_ALIGN_SIZE */
        #define VIDEO_H265_ALIGN           (1U)     /* muxer will add zero padding for frame alignment */

        Search.BufBase    = pInfo->SrcBufBase;
        Search.BufSize    = pInfo->SrcBufSize;
        Search.DataAddr   = pInfo->SrcDataAddr;
        Search.DataSize   = pInfo->SrcDataSize;
        Search.EncodeType = pPlayer->ExpMov.VideoTrack[0].MediaId;
        Search.FrameType  = pInfo->FrameType;

        SearchStartOffset(&Search);
        if (Search.Num == 0U) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "SearchIdrOffset failed %u", 0U, 0U);
            Rval = SVC_NG;
        }

        if ((pPlayer->ExpMov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_H265) ||
            (pPlayer->ExpMov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_HVC)) {

            AlignSize = VIDEO_H265_ALIGN;
        } else {
            AlignSize = VIDEO_H264_ALIGN;
        }

        for (i = 0U; i < Search.Num; i++) {
            Process.ProcType       = SVC_ENCRYPT_PROC_DEC;
            Process.InputBufBase   = pInfo->SrcBufBase;
            Process.InputBufSize   = pInfo->SrcBufSize;
            if (i == 0U) {
                /* there are other info (sps, pps ....) need to copy before the first idr */
                Process.InputAddr      = pInfo->SrcDataAddr;
                Process.InputOffset    = (Search.Offset[i] + VIDEO_START_CODE_SIZE) + VIDEO_DECRYPT_OFFSET;
            } else {
                Process.InputAddr      = pInfo->SrcDataAddr + Search.Offset[i];
                Process.InputOffset    = VIDEO_START_CODE_SIZE + VIDEO_DECRYPT_OFFSET;
            }
            Process.InputTotalSize = ((pInfo->SrcDataAddr + Search.Offset[i]) + Search.Size[i]) - Process.InputAddr;
            Process.InputProcSize  = VIDEO_DECRYPT_SIZE;

            if (Process.InputAddr >= (Process.InputBufBase + Process.InputBufSize)) {
                Process.InputAddr -= Process.InputBufSize;
            }

            MinSize = ((Process.InputOffset + Process.InputProcSize + (AlignSize - 1U)) / AlignSize) * AlignSize;

            /* only decrypt the frame if its size is enough*/
            if (MinSize >= Process.InputTotalSize) {
                Rval = SVC_NG;
            }

            /* let the output data overwrite the input data */
            Process.OutputBufBase  = pInfo->SrcBufBase;
            Process.OutputBufSize  = pInfo->SrcBufSize;
            Process.OutputAddr     = Process.InputAddr;

            if (Rval == SVC_OK) {
                Err = SvcEncrypt_Process(&Process);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcEncrypt_Process failed %u", Err, 0U);
                    Rval = SVC_NG;
                } else {
                    OutputSize += Process.OutputSize;
                }
            }
        }
    } else {
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        pInfo->DstDataSize = OutputSize;
    } else {
        pInfo->DstDataSize = pInfo->SrcDataSize;
    }

    if (Rval == SVC_OK) {
        ULONG  AlignStart, AlignSize;

        if ((pInfo->SrcDataAddr + pInfo->DstDataSize) > (pInfo->SrcBufBase + pInfo->SrcBufSize)) {
            AlignStart = pInfo->SrcDataAddr & AMBA_CACHE_LINE_MASK;
            AlignSize  = ((((pInfo->SrcBufBase + pInfo->SrcBufSize) - AlignStart) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK);
            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcPlat_CacheClean failed %u", Err, 0U);
            }

            AlignSize  = (((pInfo->SrcDataAddr + pInfo->DstDataSize) - (pInfo->SrcBufBase + pInfo->SrcBufSize)) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK;
            Err = SvcPlat_CacheClean(pInfo->SrcBufBase, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcPlat_CacheClean failed %u", Err, 0U);
            }
        } else {
            AlignStart = pInfo->SrcDataAddr & AMBA_CACHE_LINE_MASK;
            AlignSize  = (((pInfo->SrcDataAddr + pInfo->DstDataSize) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK) - AlignStart;
            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcPlat_CacheClean failed %u", Err, 0U);
            }
        }
    }
}
#endif

static void* PlayerEventHandler(void* EntryArg)
{
    UINT32             LoopU = 1U, Err, Rval, PlayerId, WaitBits = 0U;
    PLAYER_MSG_s       Msg;
    PLAYER_MONITOR_s   *pMon;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&LoopU);

    while (LoopU == 1U) {
        Rval = SVC_OK;
        Err = AmbaKAL_MsgQueueReceive(&(PlayerHdlr.MsgQue), &Msg, MONITOR_MAX_INTERVAL);

        if (Err == SVC_OK) {
            PlayerId = Msg.PlayerId;

            switch (Msg.Status) {
                case AMBA_PLAYER_STATUS_FEED_UPDATE:
                    /* feed update */
#if defined(CONFIG_ICAM_DATA_GATHER_USED)
                    /* feed CAN data */
                    if (Player[PlayerId].ExpMov.TextTrackCount > 0U) {
                        AMBA_PLAYER_BITSBUF_INFO_s   TextInfo = {0};
                        UINT8                        *pTemp = NULL;
                        UINT16                       TextLen;
                        UINT32                       DGStatus, ForceBreak = 0U;
                        ULONG                        RSize = 0U;
                        char                         TempTextBuf[128U];
                        const char                   *pText;
                        AMBA_DATG_s                  Data;

                        AmbaMisra_TouchUnused(&Rval);
                        AmbaMisra_TouchUnused(pTemp);

                        if (Rval == SVC_OK) {
                            Err = AmbaPlayer_Control(&(Player[PlayerId]), AMBA_PLAYER_CTRL_TEXT_BITSINFO_GET, &TextInfo);
                            if (Err != SVC_OK) {
                                Rval = SVC_NG;
                            }
                        }

                        if (Rval == SVC_OK) {
                            if (PlayerHdlr.TextRp[PlayerId] == 0U) {
                                PlayerHdlr.TextRp[PlayerId] = TextInfo.BufBase;
                            }

                            if (PlayerHdlr.TextRp[PlayerId] < TextInfo.Wp) {
                                RSize = TextInfo.Wp - PlayerHdlr.TextRp[PlayerId];
                            }

                            if (PlayerHdlr.TextRp[PlayerId] > TextInfo.Wp) {
                                RSize =  TextInfo.BufSize - (PlayerHdlr.TextRp[PlayerId] - TextInfo.Wp);
                            }
                        }

                        if (Rval == SVC_OK) {
                            while ((RSize > 0U) && (ForceBreak == 0U)) {
                                /* wait data gather buffer available */
                                DGStatus = 0U;
                                SvcDataGather_FileFeedStaus(&DGStatus);
                                if (DGStatus == SVC_DATA_GATHER_FILE_WRITABLE) {
                                    /* get text length */
                                    AmbaMisra_TypeCast(&pTemp, &(PlayerHdlr.TextRp[PlayerId]));
                                    TextLen   = 0U;
                                    TextLen  |= pTemp[0U];
                                    TextLen <<= 8U;
                                    TextLen  |= pTemp[1U];
                                    TextLen  += 2U;

                                    if (RSize < (ULONG)TextLen) {
                                        break;
                                    }

                                    /* transfer text to data */
                                    Err = AmbaWrap_memset(TempTextBuf, 0, sizeof(TempTextBuf));
                                    if (Err != SVC_OK) {
                                        SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memset failed %u", Err, 0U);
                                    }

                                    if ((PlayerHdlr.TextRp[PlayerId] + (UINT32)TextLen) < (TextInfo.BufBase + TextInfo.BufSize)) {
                                        Err = AmbaWrap_memcpy(TempTextBuf, pTemp, TextLen);
                                        if (Err != SVC_OK) {
                                            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memcpy failed %u", Err, 0U);
                                        }
                                    } else {
                                        ULONG CpySize, CpyRSize;

                                        CpySize = (TextInfo.BufBase + TextInfo.BufSize) - PlayerHdlr.TextRp[PlayerId];
                                        Err = AmbaWrap_memcpy(TempTextBuf, pTemp, (UINT32)CpySize);
                                        if (Err != SVC_OK) {
                                            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memcpy failed %u", Err, 0U);
                                        }

                                        CpyRSize = TextLen - CpySize;
                                        AmbaMisra_TypeCast(&pTemp, &(TextInfo.BufBase));
                                        Err = AmbaWrap_memcpy(&(TempTextBuf[CpySize]), pTemp, (UINT32)CpyRSize);
                                        if (Err != SVC_OK) {
                                            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memcpy failed %u", Err, 0U);
                                        }
                                    }
                                    pText = TempTextBuf;
                                    AmbaRscData_TextToCan(pText, &Data);

                                    Data.CapTime[TICK_TYPE_SYS] = PlayerHdlr.TextFeedCnt[PlayerId];
                                    /* feed data */
                                    SvcDataGather_FileFeedData(&Data);
#if 0
                                    SvcLog_DBG(SVC_LOG_PBK_CTRL, "DataFeetCnt %u, CapTime %u", PlayerHdlr.TextFeedCnt[PlayerId], (UINT32)Data.CapTime[TICK_TYPE_AUDIO]);
#endif
                                    /* update pointer and count */
                                    PlayerHdlr.TextFeedCnt[PlayerId]++;
                                    PlayerHdlr.TextRp[PlayerId] += TextLen;
                                    if (PlayerHdlr.TextRp[PlayerId] >= (TextInfo.BufBase + TextInfo.BufSize)) {
                                        PlayerHdlr.TextRp[PlayerId] -= TextInfo.BufSize;
                                    }

                                    RSize -= (ULONG)TextLen;
                                } else {
                                    /* there is no enough data gather buffer now, we will feed it next time */
                                    ForceBreak = 1U;
                                }
                            }
                        }
                    }
#endif
                    break;
                case AMBA_PLAYER_STATUS_FEED_DONE:
                    if (Player[PlayerId].FeedType == AMBA_PLAYER_FTYPE_MOVIE) {
                        UINT32 *pFeedNum;

                        pMon = &(PlayerHdlr.MonStatus[PlayerId]);

                        AmbaMisra_TypeCast(&pFeedNum, &(Msg.pParam));
                        pMon->Video.FeedNum = *pFeedNum;
                    }
                    /* seamless playback */
                    if ((Player[PlayerId].Seamless == 1U) && (PbkCtrlInfo[PlayerId].Loop != SVC_PBK_CTRL_LOOP_NONE)) {
                        UINT32                     FileNum = 1U;
                        SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;
                        char                       FName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];

                        if ((PbkCtrlInfo[PlayerId].Loop == SVC_PBK_CTRL_LOOP_ALL) && (PbkCtrlInfo[PlayerId].FileIndex != SVC_PBK_CTRL_IDX_LATEST)) {
                            PbkCtrlInfo[PlayerId].FileIndex++;
                        }

                        Err = SvcStgMonitor_GetDriveInfo(PbkCtrlInfo[PlayerId].Drive, &DriveInfo);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcStgMonitor_GetDriveInfo failed %u", Err, 0U);
                            Rval = SVC_NG;
                        }

                        if (Rval == SVC_OK) {
                            Err = AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, &FileNum);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaDCF_GetFileAmount failed %u", Err, 0U);
                                FileNum = 1U;
                            }
                            if ((PbkCtrlInfo[PlayerId].FileIndex != SVC_PBK_CTRL_IDX_LATEST) && (PbkCtrlInfo[PlayerId].FileIndex > FileNum)) {
                                PbkCtrlInfo[PlayerId].FileIndex = 1U;
                            }
                        }

                        if (Rval == SVC_OK) {
                            Err = AmbaDCF_GetFileName(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, FName, PbkCtrlInfo[PlayerId].FileIndex);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaDCF_GetFileName failed %u", Err, 0U);
                                Rval = SVC_NG;
                            }
                        }

                        if (Rval == SVC_OK) {
                            Err = AmbaPlayer_Control(&(Player[PlayerId]), AMBA_PLAYER_CTRL_SEAMLESS, FName);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Control failed %u", Err, 0U);
                                Rval = SVC_NG;
                            }
                        }
                    }
                    break;
                case AMBA_PLAYER_STATUS_PLAYER_EOS:
                    if ((Player[PlayerId].Seamless == 0U) && (PbkCtrlInfo[PlayerId].Loop != SVC_PBK_CTRL_LOOP_NONE)) {
                        Err = AmbaKAL_TaskSleep(1000);
                        if (Err != KAL_ERR_NONE) {
                            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_TaskSleep failed %u", Err, 0U);
                        }

                        Err = SvcPbkCtrl_Stop((UINT32)(1UL << PlayerId));
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcPbkCtrl_Stop failed %u", Err, 0U);
                            Rval = SVC_NG;
                        }

                        if (Rval == SVC_OK) {
                            Err = SvcPbkCtrl_TaskDelete(PlayerId);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcPbkCtrl_TaskDelete failed %u", Err, 0U);
                                Rval = SVC_NG;
                            } else {
                                RunningBits |= ((UINT32)1U) << PlayerId;
                            }
                        }

                        if (Rval == SVC_OK) {
                            SVC_PBK_CTRL_CREATE_s Create;

                            Create.FileName  = NULL;
                            Create.FileIndex = PbkCtrlInfo[PlayerId].FileIndex;
                            Create.Loop      = PbkCtrlInfo[PlayerId].Loop;
                            Create.Seamless  = 0U;
                            Create.Speed     = AMBA_PLAYER_SPEED_01X;
                            Create.Direction = AMBA_PLAYER_DIR_FORWARD;
                            Create.StartTime = 0U;

                            if (PbkCtrlInfo[PlayerId].Loop == SVC_PBK_CTRL_LOOP_ALL) {
                                Create.FileIndex++;
                            }

                            Err = SvcPbkCtrl_TaskCreate(PlayerId, &Create);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcPbkCtrl_TaskCreate failed %u", Err, 0U);
                                Rval = SVC_NG;
                            }
                        }

                        if (Rval == SVC_OK) {
                            WaitBits |= ((UINT32)1U) << PlayerId;
                        }

                        if ((Rval == SVC_OK) && (WaitBits == RunningBits)) {
                            /* all player need to be started at the same time*/
                            Err = SvcPbkCtrl_Start(WaitBits);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcPbkCtrl_Start failed %u", Err, 0U);
                                Rval = SVC_NG;
                            } else {
                                WaitBits = 0U;
                            }
                        }
                    }
                    break;
                case AMBA_PLAYER_STATUS_VID_FRAME_INFO:
                    if (PbkCtrlInfo[PlayerId].FeedBits > 0U) {
                        const AMBA_DSP_EVENT_VIDDEC_PIC_EX_s  *pPicInfo;
                        AMBA_DSP_EXT_YUV_BUF_s                ExtYuv[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
                        UINT16                                FovId[AMBA_DSP_MAX_VIEWZONE_NUM] = {0}, FeedNum = 0U;
                        UINT32                                i, Bits = 1U;

                        AmbaMisra_TypeCast(&pPicInfo, &(Msg.pParam));
#if 0
                        /* draw a black band at the video center for the test */
                        {
                            ULONG Addr, AlignStart, AlignSize, Height = 128U;

                            Addr = pPicInfo->YAddr + ((pPicInfo->Height * pPicInfo->Pitch) / 2U);
                            Err = AmbaWrap_memset((void*)Addr, 0, pPicInfo->Pitch * Height);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memset failed %u", Err, 0U);
                            }

                            AlignStart = Addr & AMBA_CACHE_LINE_MASK;
                            AlignSize  = (pPicInfo->Pitch * Height) & AMBA_CACHE_LINE_MASK;
                            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcPlat_CacheClean failed %u", Err, 0U);
                            }


                            Addr = pPicInfo->UVAddr + (((pPicInfo->Height * pPicInfo->Pitch) / 2U) / 2U);
                            Err = AmbaWrap_memset((void*)Addr, 128, pPicInfo->Pitch * (Height/2U));
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memset failed %u", Err, 0U);
                            }

                            AlignStart = Addr & AMBA_CACHE_LINE_MASK;
                            AlignSize  = (pPicInfo->Pitch * (Height/2U)) & AMBA_CACHE_LINE_MASK;
                            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcPlat_CacheClean failed %u", Err, 0U);
                            }
                        }
#endif

                        for (i = 0U; i < (UINT32)AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
                            if (0U < CheckBits(PbkCtrlInfo[PlayerId].FeedBits, (Bits << i))) {
                                ExtYuv[FeedNum].ExtYuvBuf.DataFmt        = pPicInfo->DataFmt;
                                ExtYuv[FeedNum].ExtYuvBuf.Pitch          = (UINT16)pPicInfo->Pitch;
                                ExtYuv[FeedNum].ExtYuvBuf.Window.Width   = (UINT16)pPicInfo->Width;
                                ExtYuv[FeedNum].ExtYuvBuf.Window.Height  = (UINT16)pPicInfo->Height;
                                ExtYuv[FeedNum].ExtYuvBuf.Window.OffsetX = 0U;
                                ExtYuv[FeedNum].ExtYuvBuf.Window.OffsetY = 0U;
                                ExtYuv[FeedNum].pExtME1Buf               = NULL;
                                ExtYuv[FeedNum].pExtME0Buf               = NULL;
                                ExtYuv[FeedNum].ExtYuvBuf.BaseAddrY      = pPicInfo->YAddr;
                                ExtYuv[FeedNum].ExtYuvBuf.BaseAddrUV     = pPicInfo->UVAddr;

                                FovId[FeedNum] = (UINT16)i;

                                FeedNum++;
                            }
                        }

                        if (FeedNum > 0U) {
                            Err = AmbaDSP_LiveviewFeedYuvData(FeedNum, FovId, ExtYuv);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaDSP_LiveviewFeedYuvData failed %u", Err, 0U);
                            }
                        }
                    }
                    break;
                case AMBA_PLAYER_STATUS_PLAYER_START:
                    if (Player[PlayerId].FeedType == AMBA_PLAYER_FTYPE_MOVIE) {
                        pMon = &(PlayerHdlr.MonStatus[PlayerId]);

                        pMon->Running          = 1U;
                    }
                    break;
                case AMBA_PLAYER_STATUS_VID_STATUS:
                    {
                        const AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s  *pStatus;

                        pMon = &(PlayerHdlr.MonStatus[PlayerId]);

                        if (pMon->Running > 0U) {
                            AmbaMisra_TypeCast(&pStatus, &(Msg.pParam));

                            pMon->Video.Decstate    = pStatus->DecState;
                            pMon->Video.NewDecNum   = pStatus->NumOfDecodedPic;

                            if ((pMon->Video.Decstate == DEC_STATE_IDLE_WITH_LAST_PIC) ||
                                (pMon->Video.Decstate == DEC_STATE_IDLE)) {
                                pMon->Running = 0U;
                            }

                            if (pMon->Video.FeedNum > 0U) {
                                if (pMon->Video.NewDecNum >= pMon->Video.FeedNum) {
                                    pMon->Running = 0U;
                                }
                            }
                        }
                    }
                    break;
                case AMBA_PLAYER_STATUS_PLAYER_STOP:
                    {
                        pMon = &(PlayerHdlr.MonStatus[PlayerId]);

                        AmbaSvcWrap_MisraMemset(pMon, 0, sizeof(PLAYER_MONITOR_s));
                    }
                    break;
                default:
                    /* NOTHING */
                    break;
            }

        }

        /* monitor decode PTS */
        {
            UINT32 NewTick = 0U, Update = 0U;

            for (PlayerId = 0U; PlayerId < PLAYER_MAX_NUM; PlayerId++) {
                pMon = &(PlayerHdlr.MonStatus[PlayerId]);

                if (pMon->Running > 0U) {
                    Err = AmbaKAL_GetSysTickCount(&NewTick);
                    if (Err != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_GetSysTickCount failed %u", Err, 0U);
                    }

                    if (pMon->Video.LastTick > 0U) {
                        if ((NewTick - pMon->Video.LastTick) > (UINT32)MONITOR_MAX_INTERVAL) {
                            if (pMon->Video.LastDecNum == pMon->Video.NewDecNum) {
                                SvcLog_DBG(SVC_LOG_PBK_CTRL, "### There is no DecPTS update over %ums for VDEC%u ###", NewTick - pMon->Video.LastTick, PlayerId);
                            }
                            Update = 1U;
                        }
                    } else {
                        Update = 1U;
                    }

                    if (Update > 0U) {
                        pMon->Video.LastTick    = NewTick;
                        pMon->Video.LastDecNum  = pMon->Video.NewDecNum;
                    }
                }
            }
        }

        AmbaMisra_TouchUnused(&Rval);
        AmbaMisra_TouchUnused(&LoopU);
    }

    return NULL;
}

static void DuplexDispUpdate(const AMBA_PLAYER_s *pPlayer)
{
    UINT32               DecFov = 0U, FovSrc, DecID = 0U;
    UINT32               i, j, Err, Rval = SVC_NG;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

    if (PbkCtrlInfo[pPlayer->PlayerId].DspMode == SVC_PBK_CTRL_MODE_DPX) {
        Rval = SVC_OK;
    }

    /* search DecFov */
    if (Rval == SVC_OK) {
        Rval = SVC_NG;
        for (i = 0U; i < pCfg->FovNum; i++) {
            FovSrc = 0U;
            Err = SvcResCfg_GetFovSrc(i, &FovSrc);
            if ((Err == SVC_OK) && (FovSrc == SVC_VIN_SRC_MEM_DEC)) {
                Err = SvcResCfg_GetDecIDOfFovIdx(i, &DecID);
                if ((Err == SVC_OK) && (DecID == pPlayer->PlayerId)) {
                    DecFov = i;
                    Rval   = SVC_OK;
                    break;
                }
            }
        }
    }

    /* update ik setting */
    if (Rval == SVC_OK) {
        AMBA_IK_MODE_CFG_s ImgMode = {0};

        ImgMode.ContextId = DecFov;

        {
            AMBA_IK_WINDOW_SIZE_INFO_s  WindowSizeInfo = {0};

            Err = AmbaIK_GetWindowSizeInfo(&ImgMode, &WindowSizeInfo);
            if (SVC_OK != Err) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaIK_GetWindowSizeInfo failed %u", Err, 0U);
            }

            WindowSizeInfo.VinSensor.StartX = 0U;
            WindowSizeInfo.VinSensor.StartY = 0U;
            WindowSizeInfo.VinSensor.Width  = pPlayer->ExpMov.VideoTrack[0].Width;
            WindowSizeInfo.VinSensor.Height = pPlayer->ExpMov.VideoTrack[0].Height;
            WindowSizeInfo.Main.Width       = pPlayer->ExpMov.VideoTrack[0].Width;
            WindowSizeInfo.Main.Height      = pPlayer->ExpMov.VideoTrack[0].Height;
            Err = AmbaIK_SetWindowSizeInfo(&ImgMode, &WindowSizeInfo);
            if (Err != IK_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaIK_SetWindowSizeInfo failed %u", Err, 0U);
            }
        }

        {
            UINT64                           AttachedRawSeq = 0ULL;
            AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s  IsoCfgCtrl;
            AMBA_IK_EXECUTE_CONTAINER_s      CfgExecInfo;

            AmbaSvcWrap_MisraMemset(&IsoCfgCtrl, 0, sizeof(IsoCfgCtrl));
            AmbaSvcWrap_MisraMemset(&CfgExecInfo, 0, sizeof(CfgExecInfo));
            Err = AmbaIK_ExecuteConfig(&ImgMode, &CfgExecInfo);
            if (SVC_OK != Err) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaIK_ExecuteConfig failed", Err, 0U);
            }

            IsoCfgCtrl.ViewZoneId = (UINT16)ImgMode.ContextId;
            IsoCfgCtrl.CfgIndex   = CfgExecInfo.IkId;
            IsoCfgCtrl.CtxIndex   = ImgMode.ContextId;
            AmbaMisra_TypeCast(&IsoCfgCtrl.CfgAddress, &CfgExecInfo.pIsoCfg);
            Err = AmbaDSP_LiveviewUpdateIsoCfg(1, &IsoCfgCtrl, &AttachedRawSeq);
            if (SVC_OK != Err) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaDSP_LiveviewUpdateIsoCfg failed", Err, 0U);
                Rval = SVC_NG;
            }
        }

    }

    /* update display stream config */
    if (Rval == SVC_OK) {
        SVC_LIV_INFO_s LivInfo;

        SvcLiveview_InfoGet(&LivInfo);

        for (i = 0U; i < pCfg->DispNum; i++) {
            for (j = 0U; j < pCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                if (pCfg->DispStrm[i].StrmCfg.ChanCfg[j].FovId == DecFov) {
                    LivInfo.pStrmCfg[i].pChanCfg[j].ROI.OffsetX = 0U;
                    LivInfo.pStrmCfg[i].pChanCfg[j].ROI.OffsetY = 0U;
                    LivInfo.pStrmCfg[i].pChanCfg[j].ROI.Width   = pPlayer->ExpMov.VideoTrack[0].Width;
                    LivInfo.pStrmCfg[i].pChanCfg[j].ROI.Height  = pPlayer->ExpMov.VideoTrack[0].Height;
                }
            }
        }
        SvcLiveview_Update();
    }
}

#if defined(CONFIG_ICAM_AUDIO_USED)
static void AoutCtrl(UINT32 Chan, UINT32 Op)
{
    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbAoutCtrl != NULL)) {
        g_pPlatCbEntry->pCbAoutCtrl(Chan, Op);
    }
}

static void ADrvMuteCtrl(UINT32 Chan, UINT32 Op)
{
    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbADrvCtrl != NULL)) {
        if (Op == 1U) {
            g_pPlatCbEntry->pCbADrvCtrl(Chan, SVC_PLAT_ADRV_MUTE_ON);
        } else {
            g_pPlatCbEntry->pCbADrvCtrl(Chan, SVC_PLAT_ADRV_MUTE_OFF);
        }
    }
}
#endif

/**
* Get playback info
* @param [in]  PlayerId player id
* @param [in]  ppInfo double pointer to SVC_PBK_CTRL_INFO_s
*/
void SvcPbkCtrl_InfoGet(UINT32 PlayerId, SVC_PBK_CTRL_INFO_s **ppInfo)
{
    if (PlayerId >= PLAYER_MAX_NUM) {
        SvcLog_NG(SVC_LOG_PBK_CTRL, "PlayerId(%u) is invalid!", PlayerId, 0U);
    } else {
        *ppInfo = &(PbkCtrlInfo[PlayerId]);
    }
}

/**
* Create player
* @param [in]  PlayerId player id
* @param [in]  pCreate create parameters
* @return ErrorCode
*/
UINT32 SvcPbkCtrl_TaskCreate(UINT32 PlayerId, const SVC_PBK_CTRL_CREATE_s *pCreate)
{
    UINT32                     Rval = SVC_OK, Index = 1U, Err, FileAmount = 0U;
    char                       FName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(FName);

    if (PlayerId < PLAYER_MAX_NUM) {
        /* pass misra rule */
        AmbaMisra_TouchUnused(&PlayerId);

        if (Rval == SVC_OK) {
            if (pCreate->FileName != NULL) {
                AmbaUtility_StringCopy(FName, sizeof(FName), pCreate->FileName);
            } else {
                Err = SvcStgMonitor_GetDriveInfo(PbkCtrlInfo[PlayerId].Drive, &DriveInfo);
                if (Err != SVC_OK) {
                    Rval = SVC_NG;
                }

                if (Rval == SVC_OK) {
                    Index = pCreate->FileIndex;

                    Err = AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, &FileAmount);
                    if (Err != SVC_OK) {
                        Index = 1U;
                    }

                    if (Index == SVC_PBK_CTRL_IDX_LATEST) {
                        Index = FileAmount;
                    }

                    if (Index > FileAmount) {
                        Index = 1U;
                    }

                    Err = AmbaDCF_GetFileName(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, FName, Index);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaDCF_GetFileName failed %u", Err, 0U);
                        Rval = SVC_NG;
                    }
                }

            }
        }

        if (Rval == SVC_OK) {
            PbkCtrlInfo[PlayerId].FileIndex = Index;
            PbkCtrlInfo[PlayerId].Loop      = pCreate->Loop;
            PbkCtrlInfo[PlayerId].pPlayer   = &(Player[PlayerId]);

            Err = AmbaWrap_memset(PbkCtrlInfo[PlayerId].pPlayer, 0, sizeof(AMBA_PLAYER_s));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memset failed %u", Err, 0U);
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaStreamWrap_GetHdlr(&(pStrHdlr[PlayerId]));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcStream_GetHdlr failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if ((Rval == SVC_OK) && (TaskCreate == 0U)) {
            ULONG  Num;

            AmbaMisra_TypeCast(&Num, &(PlayerHdlr.MsgQue));
            if (0U < AmbaUtility_UInt32ToStr(G_PbkCtrlQueName, 32U, (UINT32)Num, 16U)) {
                AmbaUtility_StringAppend(G_PbkCtrlQueName, 32, "PbkCtrlQue");
            }

            Err = AmbaKAL_MsgQueueCreate(&(PlayerHdlr.MsgQue),
                                         G_PbkCtrlQueName,
                                         (UINT32)sizeof(PLAYER_MSG_s),
                                         PlayerHdlr.MsgBuf,
                                         (UINT32)sizeof(PlayerHdlr.MsgBuf));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_MsgQueueCreate failed %u", Err, 0U);
                Rval = SVC_NG;
            }

            if (Rval == SVC_OK) {
                Err = AmbaKAL_EventFlagCreate(&(PlayerHdlr.Flag), G_PbkCtrlFlagName);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_EventFlagCreate failed %u", Err, 0U);
                    Rval = SVC_NG;
                }
            }

            if (Rval == SVC_OK) {
                Err = AmbaKAL_EventFlagClear(&(PlayerHdlr.Flag), 0xFFFFFFFFU);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
                }
            }

            if (Rval == SVC_OK) {
                Err = AmbaWrap_memset(PlayerHdlr.MonStatus, 0, sizeof(PlayerHdlr.MonStatus));
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memset failed %u", Err, 0U);
                }
            }

            if (Rval == SVC_OK) {
                PlayerHdlr.Task.Priority   = PbkCtrlInfo[0].PbkTaskInfo.TaskPriority;
                PlayerHdlr.Task.EntryFunc  = PlayerEventHandler;
                PlayerHdlr.Task.EntryArg   = 0U;
                PlayerHdlr.Task.pStackBase = PlayerHdlr.Stack;
                PlayerHdlr.Task.StackSize  = STACK_SIZE;
                PlayerHdlr.Task.CpuBits    = PbkCtrlInfo[0].PbkTaskInfo.TaskCpuBits;

                Err = SvcTask_Create("PlayerHdlr", &(PlayerHdlr.Task));
                if (SVC_OK != Err) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcTask_Create failed : PlayerHdlr %u", Err, 0U);
                    Rval = SVC_NG;
                }
            }

            if (Rval == SVC_OK) {
                TaskCreate = 1U;
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaPlayer_Parse(&(Player[PlayerId]), pStrHdlr[PlayerId], FName, AMBA_PLAYER_MTYPE_MOVIE);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Parse failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            AMBA_PLAYER_CREATE_s Create;

            Create.FeedType        = AMBA_PLAYER_FTYPE_MOVIE;
            Create.Speed           = pCreate->Speed;
            Create.Direction       = pCreate->Direction;
            Create.StartTime       = pCreate->StartTime;
            Create.PreloadSize     = 5000U;
            Create.Seamless        = pCreate->Seamless;
            Create.pStreamHdlr     = pStrHdlr[PlayerId];
            Create.pFunc           = PlayerCB;
#if defined(CONFIG_ICAM_ENCRYPTION)
            Create.pDecryptFunc    = BitsDataDecrypt;
#else
            Create.pDecryptFunc    = NULL;
#endif

            /* video */
            Create.Video.Enable    = 1U;
            Create.Video.TrackId   = 0U;

            /* audio */
            Create.Audio.Enable    = 0U;
            Create.Audio.TrackId   = 0U;
            Create.AudioCodecIndex = 0U;
            Player[PlayerId].pCbAoutCtrl = NULL;
            Player[PlayerId].pCbMuteCtrl = NULL;
#if defined(CONFIG_ICAM_AUDIO_USED)
            if ((AudioEnable == 1U) && (PlayerId == 0U) && (Player[PlayerId].ExpMov.AudioTrackCount > 0U)) {
                Create.Audio.Enable    = 1U;
                Create.Audio.TrackId   = (UINT32)Player[PlayerId].ExpMov.VideoTrackCount;
                Create.AudioCodecIndex = CONFIG_ICAM_I2S_CHANNEL;
                Player[PlayerId].pCbAoutCtrl = AoutCtrl;
                Player[PlayerId].pCbMuteCtrl = ADrvMuteCtrl;
            }
#endif

            /* text */
            if ((TextEnable == 1U) && (PlayerId == 0U) && (Player[PlayerId].ExpMov.TextTrackCount > 0U)) {
                Create.Text.Enable    = 1U;
                Create.Text.TrackId   = (UINT32)Player[PlayerId].ExpMov.VideoTrackCount + (UINT32)Player[PlayerId].ExpMov.AudioTrackCount;
            } else {
                Create.Text.Enable    = 0U;
                Create.Text.TrackId   = 0U;
            }

            if ((Create.Speed != 0U) || (Create.Direction != AMBA_PLAYER_DIR_FORWARD)) {
                Create.Audio.Enable   = 0U;
                Create.Text.Enable    = 0U;
            }

            Err = AmbaPlayer_Create(&(Player[PlayerId]), &Create);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Create failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

#if defined(CONFIG_ICAM_DATA_GATHER_USED)
        if (Rval == SVC_OK) {
            static UINT32 IsInit = 0U;

            SvcDataGather_FileFeedReset();
            PlayerHdlr.TextRp[PlayerId] = 0U;

            if (IsInit == 0U) {
                Err = AmbaWrap_memset(PlayerHdlr.TextFeedCnt, 0, sizeof(PlayerHdlr.TextFeedCnt));
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memset failed %u", Err, 0U);
                }
                IsInit = 1U;
            }
        }
#endif

#if !defined(CONFIG_ICAM_DSP_DPX_PB_ONLY)
        if (Rval == SVC_OK) {
            AMBA_PLAYER_DISP_CTRL_s Disp;

            Err = AmbaWrap_memcpy(&Disp, &(Player[PlayerId].Disp), sizeof(AMBA_PLAYER_DISP_CTRL_s));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memcpy failed %u", Err, 0U);
            }

            Err = AmbaPlayer_Control(&(Player[PlayerId]), AMBA_PLAYER_CTRL_DISP_CTRL, &Disp);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Control failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }
#endif
    } else {
        SvcLog_NG(SVC_LOG_PBK_CTRL, "Invalid PlayerId %u", PlayerId, 0U);
        Rval = SVC_NG;
    }


    return Rval;
}

/**
* Start playback
* @param [in]  PlayerBits player bits
* @return ErrorCode
*/
UINT32 SvcPbkCtrl_Start(UINT32 PlayerBits)
{
    UINT32                           Rval = SVC_OK, PlayerId, PlayerNum = 0U, Err, Bits = 0U;
    AMBA_PLAYER_s                    *PlayerArr[PLAYER_MAX_NUM];

    AmbaMisra_TouchUnused(&Rval);

    for (PlayerId = 0U; PlayerId < PLAYER_MAX_NUM; PlayerId++) {
        if (((((UINT32)1U) << PlayerId) & (PlayerBits)) != 0U) {
            PlayerArr[PlayerNum] = &(Player[PlayerId]);
            Bits = Bits | (((UINT32)1U) << PlayerId);
            PlayerNum++;
            DuplexDispUpdate(&(Player[PlayerId]));
        }
    }

    if ((SVC_OK == Rval) && (PlayerNum > 0U)) {
        SvcClock_FeatureCtrl(1U, (SVC_CLK_FEA_BIT_CODEC | SVC_CLK_FEA_BIT_HEVC));

        Err = AmbaPlayer_Start((UINT16)PlayerNum, PlayerArr);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Start failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    if ((SVC_OK == Rval) && (PlayerNum > 0U)) {
        RunningBits = RunningBits | Bits;
    }

    return Rval;
}

/**
* Stop playback
* @param [in]  PlayerBits player bits
* @return ErrorCode
*/
UINT32 SvcPbkCtrl_Stop(UINT32 PlayerBits)
{
    UINT32                           Rval = SVC_OK, PlayerId, PlayerNum = 0U, Err, Bits = 0U;
    AMBA_PLAYER_s                    *PlayerArr[PLAYER_MAX_NUM];

    AmbaMisra_TouchUnused(&Rval);

    for (PlayerId = 0U; PlayerId < PLAYER_MAX_NUM; PlayerId++) {
        if ((((UINT32)1U << PlayerId) & (PlayerBits)) != 0U) {
            PlayerArr[PlayerNum] = &(Player[PlayerId]);
            Bits = Bits | ((UINT32)1U << PlayerId);
            PlayerNum++;
        }
    }

    if ((SVC_OK == Rval) && (PlayerNum > 0U)) {

        Err = AmbaPlayer_Stop((UINT16)PlayerNum, PlayerArr);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Start failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    if ((SVC_OK == Rval) && (PlayerNum > 0U)) {
        RunningBits = RunningBits & (~Bits);
    }

    return Rval;
}

/**
* Delete playback task
* @param [in]  PlayerId player id
* @return ErrorCode
*/
UINT32 SvcPbkCtrl_TaskDelete(UINT32 PlayerId)
{
    UINT32                  Rval = SVC_OK, Err;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == SVC_OK) {
        if (PlayerId >= PLAYER_MAX_NUM) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "Invalid PlayerId %u", PlayerId, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Err = AmbaPlayer_Delete(&(Player[PlayerId]));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Delete failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        UINT32 RecBits = 0U;

        AmbaMisra_TouchUnused(&RecBits);

#if defined(CONFIG_ICAM_RECORD_USED)
        SvcRecMain_Control(SVC_RCM_GET_ENC_STATUS, 0U, NULL, &RecBits);
#endif

        if ((RunningBits == 0U) && (RecBits == 0U)) {
            SvcClock_FeatureCtrl(0U, (SVC_CLK_FEA_BIT_CODEC | SVC_CLK_FEA_BIT_HEVC));
        }
    }

#if defined(CONFIG_ICAM_ENABLE_REGRESSION_FLOW)
    if (PlayerId < PLAYER_MAX_NUM) {
        Err = AmbaStreamWrap_DeleteHdlr((pStrHdlr[PlayerId]));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaStreamWrap_DeleteHdlr failed %u", Err, 0U);
        }
    }
#endif

#if 0
    /* now we only create handler task once */
    if ((Rval == SVC_OK) && (TaskCreate == 1U) && (RunningBits == 0U)) {

        Err = SvcTask_Destroy(&(PlayerHdlr.Task));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcTask_Destroy failed %u", Err, 0U);
            Rval = SVC_NG;
        }

        if (Rval == SVC_OK) {
            Err = AmbaKAL_EventFlagDelete(&(PlayerHdlr.Flag));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_EventFlagDelete failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaKAL_MsgQueueDelete(&(PlayerHdlr.MsgQue));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_MsgQueueDelete failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            TaskCreate = 0U;
        }
    }
#endif

    return Rval;
}

/**
* Playback trickplay
* @param [in]  PlayerBits player bits
* @param [in]  Operation trick play operation
* @return ErrorCode
*/
UINT32 SvcPbkCtrl_TrickPlay(UINT32 PlayerBits, UINT32 Operation)
{
    UINT32                           Rval = SVC_OK, PlayerId, PlayerNum = 0U, Err, Bits;
    AMBA_PLAYER_s                    *PlayerArr[PLAYER_MAX_NUM];

    AmbaMisra_TouchUnused(&Rval);

    Bits = PlayerBits & RunningBits;

    for (PlayerId = 0U; PlayerId < PLAYER_MAX_NUM; PlayerId++) {
        if (((1UL << PlayerId) & (Bits)) != 0U) {
            PlayerArr[PlayerNum] = &(Player[PlayerId]);
            PlayerNum++;
        }
    }

    if ((SVC_OK == Rval) && (PlayerNum > 0U)) {
        Err = AmbaPlayer_TrickPlay((UINT16)PlayerNum, PlayerArr, Operation);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_TrickPlay failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    /* stop monitor for pause status */
    if ((SVC_OK == Rval) && (PlayerNum > 0U)) {
        PLAYER_MONITOR_s *pMon;

        for (PlayerId = 0U; PlayerId < PlayerNum; PlayerId++) {
            pMon = &(PlayerHdlr.MonStatus[PlayerId]);

            if (Operation == AMBA_PLAYER_TRICK_PAUSE) {
                pMon->Running        = 0U;
            }

            if (Operation == AMBA_PLAYER_TRICK_RESUME) {
                pMon->Video.LastTick = 0U;
                pMon->Running        = 1U;
            }
        }
    }

    return Rval;
}

/**
* Playback zoom and rotate
* @param [in]  PlayerId player id
* @param [in]  pDisp Vout bits
* @return ErrorCode
*/
UINT32 SvcPbkCtrl_PostCtrl(UINT32 PlayerId, AMBA_PLAYER_DISP_CTRL_s *pDisp)
{
    UINT32 Err, Rval = SVC_OK;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == SVC_OK) {
        if (PlayerId >= PLAYER_MAX_NUM) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "Invalid PlayerId %u", PlayerId, 0U);
            Rval = SVC_NG;
        }
    }

#if !defined(CONFIG_ICAM_DSP_DPX_PB_ONLY)
    if (Rval == SVC_OK) {
        Err = AmbaPlayer_Control(&(Player[PlayerId]), AMBA_PLAYER_CTRL_DISP_CTRL, pDisp);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Control failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }
#else
    AmbaMisra_TouchUnused(&pDisp);
    AmbaMisra_TouchUnused(pDisp);
    AmbaMisra_TouchUnused(&Err);
#endif
    return Rval;
}

/**
* Decode one frame
* @param [in]  PlayerId decoder bits
* @param [in]  pInput decode info
* @return ErrorCode
*/
UINT32 SvcPbkCtrl_DecOneFrame(UINT32 PlayerId, const SVC_PBK_CTRL_DEC_FRAME_s *pInput)
{
    UINT32                     Rval = SVC_OK, Index = 1U, Err;
    char                       FName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(FName);

    if (PlayerId >= PLAYER_MAX_NUM) {
        SvcLog_NG(SVC_LOG_PBK_CTRL, "Invalid PlayerId %u", PlayerId, 0U);
        Rval = SVC_NG;
    } else {
        if (Rval == SVC_OK) {
            if (pInput->FileName != NULL) {
                AmbaUtility_StringCopy(FName, sizeof(FName), pInput->FileName);
            } else {
                Err = SvcStgMonitor_GetDriveInfo(PbkCtrlInfo[PlayerId].Drive, &DriveInfo);
                if (Err != SVC_OK) {
                    Rval = SVC_NG;
                }

                if (Rval == SVC_OK) {
                    Index = pInput->FileIndex;

                    if (Index == SVC_PBK_CTRL_IDX_LATEST) {
                        /* Get latest file index */
                        Err = AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, &Index);
                        if (Err != SVC_OK) {
                            Index = 1U;
                        }
                    }

                    Err = AmbaDCF_GetFileName(DriveInfo.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, FName, Index);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaDCF_GetFileName failed %u", Err, 0U);
                        Rval = SVC_NG;
                    }
                }
            }
        }

        if (Rval == SVC_OK) {
            PbkCtrlInfo[PlayerId].FileIndex = Index;
            PbkCtrlInfo[PlayerId].pPlayer   = &(Player[PlayerId]);

            Err = AmbaWrap_memset(&(Player[PlayerId]), 0, sizeof(AMBA_PLAYER_s));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memset failed %u", Err, 0U);
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaStreamWrap_GetHdlr(&(pStrHdlr[PlayerId]));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcStream_GetHdlr failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if ((Rval == SVC_OK) && (TaskCreate == 0U)) {
            ULONG  Num;

            AmbaMisra_TypeCast(&Num, &(PlayerHdlr.MsgQue));
            if (0U < AmbaUtility_UInt32ToStr(G_PbkCtrlQueName, 32U, (UINT32)Num, 16U)) {
                AmbaUtility_StringAppend(G_PbkCtrlQueName, 32, "PbkCtrlQue");
            }

            Err = AmbaKAL_MsgQueueCreate(&(PlayerHdlr.MsgQue),
                                         G_PbkCtrlQueName,
                                         (UINT32)sizeof(PLAYER_MSG_s),
                                         PlayerHdlr.MsgBuf,
                                         (UINT32)sizeof(PlayerHdlr.MsgBuf));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_MsgQueueCreate failed %u", Err, 0U);
                Rval = SVC_NG;
            }

            if (Rval == SVC_OK) {
                Err = AmbaKAL_EventFlagCreate(&(PlayerHdlr.Flag), G_PbkCtrlFlagName);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_EventFlagCreate failed %u", Err, 0U);
                    Rval = SVC_NG;
                }
            }

            if (Rval == SVC_OK) {
                Err = AmbaKAL_EventFlagClear(&(PlayerHdlr.Flag), 0xFFFFFFFFU);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
                }
            }

            if (Rval == SVC_OK) {
                Err = AmbaWrap_memset(PlayerHdlr.MonStatus, 0, sizeof(PlayerHdlr.MonStatus));
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaWrap_memset failed %u", Err, 0U);
                }
            }

            if (Rval == SVC_OK) {
                PlayerHdlr.Task.Priority   = PbkCtrlInfo[0].PbkTaskInfo.TaskPriority;
                PlayerHdlr.Task.EntryFunc  = PlayerEventHandler;
                PlayerHdlr.Task.EntryArg   = 0U;
                PlayerHdlr.Task.pStackBase = PlayerHdlr.Stack;
                PlayerHdlr.Task.StackSize  = STACK_SIZE;
                PlayerHdlr.Task.CpuBits    = PbkCtrlInfo[0].PbkTaskInfo.TaskCpuBits;

                Err = SvcTask_Create("PlayerHdlr", &(PlayerHdlr.Task));
                if (SVC_OK != Err) {
                    SvcLog_NG(SVC_LOG_PBK_CTRL, "SvcTask_Create failed : PlayerHdlr %u", Err, 0U);
                    Rval = SVC_NG;
                }
            }

            if (Rval == SVC_OK) {
                TaskCreate = 1U;
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaPlayer_Parse(&(Player[PlayerId]), pStrHdlr[PlayerId], FName, AMBA_PLAYER_MTYPE_MOVIE);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Parse failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            AMBA_PLAYER_CREATE_s Create;

            Create.FeedType       = AMBA_PLAYER_FTYPE_FRAME;
            Create.Speed          = AMBA_PLAYER_SPEED_01X;
            Create.Direction      = AMBA_PLAYER_DIR_FORWARD;
            Create.StartTime      = 0U;
            Create.Video.Enable   = 1U;
            Create.Video.TrackId  = 0U;
            Create.Audio.Enable   = 0U;
            Create.Audio.TrackId  = 1U;
            Create.Text.Enable    = 0U;
            Create.Text.TrackId   = 2U;
            Create.Seamless       = 0U;
            Create.pStreamHdlr    = pStrHdlr[PlayerId];
            Create.pFunc          = PlayerCB;
#if defined(CONFIG_ICAM_ENCRYPTION)
            Create.pDecryptFunc    = BitsDataDecrypt;
#else
            Create.pDecryptFunc    = NULL;
#endif

            Err = AmbaPlayer_Create(&(Player[PlayerId]), &Create);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_Create failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            AMBA_DSP_STLDEC_STATUS_s  Info;

            SvcClock_FeatureCtrl(1U, (SVC_CLK_FEA_BIT_CODEC | SVC_CLK_FEA_BIT_HEVC));

            Err = AmbaPlayer_DecOneFrame(&(Player[PlayerId]), &Info);
            if (Err != 0U) {
                SvcLog_NG(SVC_LOG_PBK_CTRL, "AmbaPlayer_DecOneFrame failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaPlayer_Delete(&(Player[PlayerId]));
            if (Err != 0U) {
                SvcLog_NG("CMDPBK", "AmbaPlayer_Delete failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }
    }

    return Rval;
}

/**
* enable or disable track, should be called before SvcPbkCtrl_TaskCreate
* @param [in]  TrackType
* @param [in]  Enable 0: disable, 1: enable
*/
void SvcPbkCtrl_TrackCtrl(UINT32 TrackType, UINT32 Enable)
{
    if (TrackType == SVC_PBK_CTRL_TRACK_AUDIO) {
        /* audio control */
#if defined(CONFIG_ICAM_AUDIO_USED)
        if (Enable == 0U) {
            AudioEnable = 0U;
        } else {
            AudioEnable = 1U;
        }
#endif
    } else if (TrackType == SVC_PBK_CTRL_TRACK_TEXT) {
        if (Enable == 0U) {
            TextEnable  = 0U;
        } else {
            TextEnable  = 1U;
        }
    } else {
        SvcLog_NG("CMDPBK", "Unknown track type %u", TrackType, 0U);
    }
}

