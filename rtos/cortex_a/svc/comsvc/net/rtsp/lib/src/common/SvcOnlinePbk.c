/**
 *  @file SvcOnlinePbk.c
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
 *  @details svc online playback
 *
 */
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaMisraFix.h"
#include "AmbaPrint.h"
#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "SvcOnlinePbk.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaAudio_AENC.h"
#include "SvcRtspLink.h"

#define ONLINE_PBK_TASK_0_PRI               (86U)
#define ONLINE_PBK_TASK_0_CPU_BITS          (0x01U)
#define ONLINE_PBK_STACK_SIZE               (0x2000U)

/* Loop switch on/off */
#define ON                                  1U
#define OFF                                 0U
#define TIME_SCALE_90K                      (90000UL)

#define ONLINE_PBK_VFLG_START_FEED          (0x00000001U)
#define ONLINE_PBK_AFLG_START_FEED          (0x00000002U)
#define ONLINE_VIDEO_HANDLER_PAUSE          (0x00000004U)
#define ONLINE_VIDEO_HANDLER_RESUME         (0x00000008U)
#define ONLINE_AUDIO_HANDLER_PAUSE          (0x00000010U)
#define ONLINE_AUDIO_HANDLER_RESUME         (0x00000020U)

#define SCODE                               ((const UINT8 *)"\x00\x00\x00\x01")
#define SCODE_SIZE                          4U
#define ZEROPADDING                         ((const UINT8 *)"\x00\x00\x00\x00")
#define ZEROPADDING_SIZE                    4U
#define H264_END_OF_SEQUENCE                ((const UINT8 *)"\x00\x00\x00\x01\x0A")
#define H264_END_OF_STREAM                  ((const UINT8 *)"\x00\x00\x00\x01\x0B")
#define HEVC_END_OF_SEQUENCE                ((const UINT8 *)"\x00\x00\x01\x48\x00")
#define HEVC_END_OF_STREAM                  ((const UINT8 *)"\x00\x00\x00\x01\x4A\x00")
#define H264_EOS_SIZE                       5U
#define HEVC_EOS_SIZE                       5U
#define HEVC_EOF_SIZE                       6U
#define NALU_GOP_SIZE                       22U
#define MAX_NALUS_IN_SAMPLE                 10U
#define ALIGN_32                            32U
#define MAX_INSERT_ZERO                     (MAX_NALUS_IN_SAMPLE * ALIGN_32)     /* Each NALUS reserved 32 bytes insert "0" */

#define NULL_PTS                            (0xFFFFFFFFU)
#define UPDATE_MINIMUM                      3U

#define ALIGN_32                            32U

#define ADEC_CH_NUM      2U
#define ADEC_FRAME_SIZE  1024U

/*TEMP DEFINE*/
#define Big2Little_GetBe32 SvcFileOp_GetBe32
#define Big2Little_GetBe16 SvcFileOp_GetBe16
#define Big2Little_GetByte SvcFileOp_GetByte

typedef enum {
    NEXT_FEED_ALIGN32_DISABLE = 0U,
    NEXT_FEED_ALIGN32_ENABLE  = 1U
} NEXT_FEED_ALIGN32_e;

typedef struct _APPLIB_NETFIFO_SPS_PPS_s_ {
    UINT8 Sps[64];  /**< H264 SPS */
    UINT32 SpsLen;  /**< H264 SPS size */
    UINT8 Pps[32];  /**< H264 PPS */
    UINT32 PpsLen;  /**< H264 PPS size */
} APPLIB_NETFIFO_SPS_PPS_s;

typedef struct _APPLIB_NETFIFO_VPS_SPS_PPS_s_ {
    UINT8 Vps[32];  /**< H265 VPS */
    UINT32 VpsLen;  /**< H265 VPS size */
    UINT8 Sps[60];  /**< H265 SPS */
    UINT32 SpsLen;  /**< H265 SPS size */
    UINT8 Pps[24];  /**< H265 PPS */
    UINT32 PpsLen;  /**< H265 PPS size */
} APPLIB_NETFIFO_VPS_SPS_PPS_s;

#define ONLINE_PBK_MSG_NUM      (512U)
#define ONLINE_PBK_VCACHE_SIZE  (1024*1024)
#define ONLINE_PBK_ACACHE_SIZE  (64*1024)

typedef struct {
    UINT32      VFeedEof;
    UINT32      AFeedEof;
    UINT8       IsFeedStoped;
    UINT64      LatestPts;
    UINT32      VFeedIdx;
    UINT32      AFeedIdx;
    ULONG       VFeedPointer;
    ULONG       AFeedPointer;
    UINT64      LatestDts;
    UINT32      DeltaVFrameNum;
    UINT32      DeltaAFrameNum;
    UINT32      VQueueSndSize;
    UINT32      VQueueRcvSize;
    UINT32      AQueueSndSize;
    UINT32      AQueueRcvSize;
    UINT8       VCacheBuf[ONLINE_PBK_VCACHE_SIZE];  // 2MB cache buf for video
    UINT32      VCachedSize;
    UINT32      VCachedFileOff;
    UINT8       *VideoDataPtr;
    UINT8       ACacheBuf[ONLINE_PBK_ACACHE_SIZE];  // 64KB cache buf for audio
    UINT32      ACachedSize;
    UINT32      ACachedFileOff;
    UINT32      FileError;
    UINT32      ForceEos;
} SVC_ONLINE_PBK_AVTSK_Ctr_s;

typedef struct {
    UINT32                      InitFlg;
    AMBA_KAL_EVENT_FLAG_t       PbkCtrFlg;
    AMBA_KAL_MSG_QUEUE_t        VMsgQueue;
    AMBA_DSP_ENC_PIC_RDY_s      VQueueBuf[ONLINE_PBK_MSG_NUM];
    AMBA_KAL_MSG_QUEUE_t        AMsgQueue;
    AMBA_AENC_AUDIO_DESC_s      AQueueBuf[ONLINE_PBK_MSG_NUM];
    SVC_VFS_FILE_s              VideoFile;
    SVC_VFS_FILE_s              AudioFile;
    SVC_DMUX_MOV_INFO_s         DMux;
    AMBA_KAL_MUTEX_t            FileReadMutex;
    AMBA_KAL_MUTEX_t            IpcMutex;
    SVC_ONLINE_PBK_AVTSK_Ctr_s  AvTskCtrl;
} SVC_ONLINE_PBK_Ctr_s;

static UINT8                VideoTskStack[ONLINE_PBK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
static UINT8                AudioTskStack[ONLINE_PBK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
static UINT8                VideoHandlerTskStack[ONLINE_PBK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
static UINT8                AudioHandlerTskStack[ONLINE_PBK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

static SVC_ONLINE_PBK_BUF_s OnlinePbkBuf;
static SVC_ONLINE_PBK_Ctr_s OnlinePbkCtr;
static SVC_TASK_CTRL_s      VideoTskCtrl GNU_SECTION_NOZEROINIT;
static SVC_TASK_CTRL_s      AudioTskCtrl GNU_SECTION_NOZEROINIT;
static SVC_TASK_CTRL_s      VideoHandlerTskCtrl GNU_SECTION_NOZEROINIT;
static SVC_TASK_CTRL_s      AudioHandlerTskCtrl GNU_SECTION_NOZEROINIT;

static UINT32 SvcOnlinePbk_DeInit(VOID);

#if 1
/* temp function */
static UINT32 SvcFileOp_GetBe32(const SVC_VFS_FILE_s *pFile)
{
    UINT8  Val[4];
    UINT32 Rval;

    (VOID)pFile;
    if (OK != AmbaWrap_memcpy(Val, OnlinePbkCtr.AvTskCtrl.VideoDataPtr, 4U)) {
        Rval = ERR_NA;
        Rval = 0xFFFFFFFFU;
        AmbaPrint_PrintStr5("%s, memcpy failed!", __func__, NULL, NULL, NULL, NULL);
    } else {
        OnlinePbkCtr.AvTskCtrl.VideoDataPtr += 4U;
        Rval = 0;
        Rval |= Val[0];
        Rval <<= 8U;
        Rval |= Val[1];
        Rval <<= 8U;
        Rval |= Val[2];
        Rval <<= 8U;
        Rval |= Val[3];
    }

    return Rval;
}
static UINT32 SvcFileOp_GetBe16(const SVC_VFS_FILE_s *pFile)
{
    UINT8  Val[2];
    UINT32 Rval;

    (VOID)pFile;
    if (OK != AmbaWrap_memcpy(Val, OnlinePbkCtr.AvTskCtrl.VideoDataPtr, 2U)) {
        Rval = ERR_NA;
        Rval = 0xFFFFFFFFU;
        AmbaPrint_PrintStr5("%s, memcpy failed!", __func__, NULL, NULL, NULL, NULL);
    } else {
        OnlinePbkCtr.AvTskCtrl.VideoDataPtr += 2U;
        Rval = 0;
        Rval |= Val[0];
        Rval <<= 8U;
        Rval |= Val[1];
    }

    return Rval;
}

static UINT32 SvcFileOp_GetByte(const SVC_VFS_FILE_s *pFile)
{
    UINT8  Val;
    UINT32 Rval;

    (VOID)pFile;
    if (OK != AmbaWrap_memcpy(&Val, OnlinePbkCtr.AvTskCtrl.VideoDataPtr, 1U)) {
        Rval = ERR_NA;
        Rval = 0xFFFFFFFFU;
        AmbaPrint_PrintStr5("%s, memcpy failed!", __func__, NULL, NULL, NULL, NULL);
    } else {
        OnlinePbkCtr.AvTskCtrl.VideoDataPtr += 1U;
        Rval = (UINT32)Val;
    }

    return Rval;
}
#endif

static UINT32 PbkSearchFeedPoint(UINT64 LatestPts, UINT32 *pFeedIdx)
{
    INT32                       low = 0, high;
    UINT8                       Loop = ON;
    UINT32                      PrevIndex = 0U, TgtIdx, tmpidx = 0U, Rval = OK;
    UINT64                      Pts, TempPts;

    /* Find previous near IDR picture by using binary search */
    high = (INT32)OnlinePbkCtr.DMux.VideoSyncSampleTableSize - 1;

    while (Loop == ON) {
        INT32 mid = (low + high) / 2;

        /* Make sure pts range not over than highest value */
        tmpidx = OnlinePbkCtr.DMux.pVideoSyncSampleTable[high] - 1U;
        Pts =  ((UINT64)OnlinePbkCtr.DMux.pVideoSyncSampleTable[high] - 1ULL) * (UINT64)OnlinePbkCtr.DMux.SampleDurationV;
        Pts += (UINT64)OnlinePbkCtr.DMux.VideoFrameContext[tmpidx].FrameOffset;

        /* Change to 90K base */
        Pts *= (UINT64)TIME_SCALE_90K;
        Pts /= (UINT64)OnlinePbkCtr.DMux.TimescaleV;

        if (LatestPts > Pts) {
            PrevIndex = OnlinePbkCtr.DMux.pVideoSyncSampleTable[high] - 1U;
            break;
        }

        tmpidx = OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid] - 1U;
        Pts =  ((UINT64)OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid] - 1ULL) * (UINT64)OnlinePbkCtr.DMux.SampleDurationV;
        Pts += (UINT64)OnlinePbkCtr.DMux.VideoFrameContext[tmpidx].FrameOffset;

        /* Change to 90K base */
        Pts *= (UINT64)TIME_SCALE_90K;
        Pts /= (UINT64)OnlinePbkCtr.DMux.TimescaleV;

        if (LatestPts == Pts) { /* Return preIDR index */
            if (0 == mid) { /* If IDR index = 0 */
                PrevIndex = OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid] - 1U;
            } else {
                PrevIndex = OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid - 1] - 1U;
            }
            Loop = OFF;
        } else if (LatestPts > Pts) { /* IDR Pts smaller than lastPts */
            low = mid + 1;
            /* Compare preIDR index to get the final IDR index */
            TempPts =  ((UINT64)OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid + 1] - 1ULL) * (UINT64)OnlinePbkCtr.DMux.SampleDurationV;
            tmpidx  =  OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid + 1] - 1U;
            TempPts += (UINT64)OnlinePbkCtr.DMux.VideoFrameContext[tmpidx].FrameOffset;

            /* Change to 90K base */
            TempPts *= (UINT64)TIME_SCALE_90K;
            TempPts /= (UINT64)OnlinePbkCtr.DMux.TimescaleV;

            if (TempPts >= LatestPts) { /* If preIDR pts bigger or equal to lastPts, return preIDR index */
                PrevIndex = OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid] - 1U;
                Loop = OFF;
            }
        } else { /* IDR Pts bigger than lastPts */
            high = mid - 1;
            /* Compare preIDR index to get the final index */
            TempPts =  ((UINT64)OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid - 1] - 1ULL) * (UINT64)OnlinePbkCtr.DMux.SampleDurationV;
            tmpidx  =  OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid - 1] - 1U;
            TempPts += (UINT64)OnlinePbkCtr.DMux.VideoFrameContext[tmpidx].FrameOffset;

            /* Change to 90K base */
            TempPts *= TIME_SCALE_90K;
            TempPts /= OnlinePbkCtr.DMux.TimescaleV;

            if (TempPts < LatestPts) {  /* If preIDR pts smaller than lastPts, return preIDR index */
                PrevIndex = OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid-1] - 1U;
                Loop = OFF;
            }

            if (TempPts == LatestPts) { /* If preIDR pts equal to lastPts, return pre preIDR index */
                if (0 == (mid-1)) { /* If (mid-1) is first IDR index (IDR = 0) */
                    PrevIndex = OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid-1] - 1U;
                } else {
                    PrevIndex = OnlinePbkCtr.DMux.pVideoSyncSampleTable[mid-2] - 1U;
                }
                Loop = OFF;
            }
        }
    }

    TgtIdx = PrevIndex;

    {
        char                PrintBuf[64];

        {
            char    *StrBuf = PrintBuf;
            UINT32  BufSize = 64U;
            UINT32  CurStrLen;

            AmbaUtility_StringAppend(StrBuf, BufSize, "PbkSearchFeedPoint: NearIdrIndex = ");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(TgtIdx), 10U);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, ", LatestPts = ");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(LatestPts), 10U);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, "");

        }

        if (0U < AmbaUtility_StringLength(PrintBuf)) {
            AmbaMisra_TouchUnused(&Rval);
            AmbaPrint_PrintStr5("%s, %s", __func__, PrintBuf, NULL, NULL, NULL);
        }
    }

    *pFeedIdx = TgtIdx;
    return Rval;
}

static UINT32 ReadVideoData(const UINT8 *pFrom, UINT32 ReadSize, NEXT_FEED_ALIGN32_e Option)
{
    UINT32                      Align32Check = 0U, InsertZeroCnt = 0U;
    UINT32                      Left, FeedPointRemain, TempReadSize = 0U;
    UINT32                      Rval = OK;
    ULONG                       pLimitUL, pFromUL;
    void                        *FeedPointer, *pBase;
    const void                  *pTempFrom;

    AmbaMisra_TypeCast(&pBase, &OnlinePbkBuf.VBuffAddr);
    pLimitUL = OnlinePbkBuf.VBuffAddr + OnlinePbkBuf.VBuffSize - 1U;

    if (Option == NEXT_FEED_ALIGN32_DISABLE) {
        if (pLimitUL <= (OnlinePbkCtr.AvTskCtrl.VFeedPointer + ReadSize - 1U)) {
            Left = (pLimitUL - OnlinePbkCtr.AvTskCtrl.VFeedPointer) + 1U;
            if (NULL == pFrom) {
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
                if (Left && OK != AmbaWrap_memcpy(FeedPointer, OnlinePbkCtr.AvTskCtrl.VideoDataPtr, Left)) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 1 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                } else {
                    OnlinePbkCtr.AvTskCtrl.VideoDataPtr += Left;
                }

                Left = ReadSize - Left;
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkBuf.VBuffAddr));
                if (Left && OK != AmbaWrap_memcpy(FeedPointer, OnlinePbkCtr.AvTskCtrl.VideoDataPtr, Left)) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 2 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                } else {
                    OnlinePbkCtr.AvTskCtrl.VideoDataPtr += Left;
                }
            } else {
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
                if (OK != AmbaWrap_memcpy(FeedPointer, pFrom, Left)) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 3 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                }

                AmbaMisra_TypeCast(&pFromUL, &pFrom);
                pFromUL += Left;
                Left = ReadSize - Left;
                AmbaMisra_TypeCast(&pTempFrom, &pFromUL);

                if (OK != AmbaWrap_memcpy(pBase, pTempFrom, Left)) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 4 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            OnlinePbkCtr.AvTskCtrl.VFeedPointer = (OnlinePbkBuf.VBuffAddr + Left);
        } else {
            if (NULL == pFrom) {
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
                if (OK != AmbaWrap_memcpy(FeedPointer, OnlinePbkCtr.AvTskCtrl.VideoDataPtr, ReadSize)) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 5 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                } else {
                    OnlinePbkCtr.AvTskCtrl.VideoDataPtr += ReadSize;
                }
            } else {
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
                if (OK != AmbaWrap_memcpy(FeedPointer, pFrom, ReadSize)) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 6 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            OnlinePbkCtr.AvTskCtrl.VFeedPointer += ReadSize;
        }
    } else if ((Option == NEXT_FEED_ALIGN32_ENABLE) && (NULL != pFrom)) {
        /* Evaluate space remaining from VFeedPointer to pLimit */
        FeedPointRemain = (pLimitUL - OnlinePbkCtr.AvTskCtrl.VFeedPointer) + 1U;

        /* If remaining space less than ReadSize                      *\
         * 1. Insert 0 form here to pLimit                            *
         * 2. Insert "0" from pBase, let next feed pointer            *
        \*    (VFeedPointer) align32                          */
        if (FeedPointRemain < ReadSize) {
            AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
            if (OK != AmbaWrap_memset(FeedPointer, 0, FeedPointRemain)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, 1 memset failed!", __func__, NULL, NULL, NULL, NULL);
            }
            OnlinePbkCtr.AvTskCtrl.VFeedPointer = OnlinePbkBuf.VBuffAddr;

            if (ReadSize > ALIGN_32) {
                TempReadSize = ReadSize & (ALIGN_32 - 1U);
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
                if (OK != AmbaWrap_memset(FeedPointer, 0, (ALIGN_32 - TempReadSize))) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 2 memset failed!", __func__, NULL, NULL, NULL, NULL);
                }
                OnlinePbkCtr.AvTskCtrl.VFeedPointer += (ALIGN_32 - TempReadSize);
            } else {
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
                if (OK != AmbaWrap_memset(FeedPointer, 0, (ALIGN_32 - ReadSize))) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 3 memset failed!", __func__, NULL, NULL, NULL, NULL);
                }
                OnlinePbkCtr.AvTskCtrl.VFeedPointer += (ALIGN_32 - ReadSize);
            }
            AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
            if (OK != AmbaWrap_memcpy(FeedPointer, pFrom, ReadSize)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, 7 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
            }
            OnlinePbkCtr.AvTskCtrl.VFeedPointer += ReadSize;

        /* If remaining space equal to ReadSize, insert data and assign VFeedPointer to pBase */
        } else if (FeedPointRemain == ReadSize) {
            AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
            if (OK != AmbaWrap_memcpy(FeedPointer, pFrom, ReadSize)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, 8 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
            }
            OnlinePbkCtr.AvTskCtrl.VFeedPointer = OnlinePbkBuf.VBuffAddr;

        /* If remaining space enough, evaluate how many 0 to insert */
        } else {
            Align32Check = OnlinePbkCtr.AvTskCtrl.VFeedPointer & (ALIGN_32 - 1U);

            if (ReadSize > ALIGN_32) {
                TempReadSize = ReadSize & (ALIGN_32 - 1U);
            } else {
                TempReadSize = ReadSize;
            }

            if (Align32Check > (ALIGN_32 - TempReadSize)) {
                InsertZeroCnt = ((ALIGN_32 + ALIGN_32 - TempReadSize) - Align32Check);
            } else if (Align32Check == (ALIGN_32 - TempReadSize)) {
                InsertZeroCnt = 0U;
            } else {
                InsertZeroCnt = (ALIGN_32 - TempReadSize) - Align32Check;
            }

            if (FeedPointRemain < (InsertZeroCnt + ReadSize)) {
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
                if (OK != AmbaWrap_memset(FeedPointer, 0, FeedPointRemain)) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 4 memset failed!", __func__, NULL, NULL, NULL, NULL);
                }
                OnlinePbkCtr.AvTskCtrl.VFeedPointer = OnlinePbkBuf.VBuffAddr;

                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
                if (OK != AmbaWrap_memset(FeedPointer, 0, (ALIGN_32 - TempReadSize))) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 5 memset failed!", __func__, NULL, NULL, NULL, NULL);
                }
                OnlinePbkCtr.AvTskCtrl.VFeedPointer += (ALIGN_32 - TempReadSize);
            } else {
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
                if (OK != AmbaWrap_memset(FeedPointer, 0, InsertZeroCnt)) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 6 memset failed!", __func__, NULL, NULL, NULL, NULL);
                }
                OnlinePbkCtr.AvTskCtrl.VFeedPointer += InsertZeroCnt;
            }
            AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.VFeedPointer));
            if (OK != AmbaWrap_memcpy(FeedPointer, pFrom, ReadSize)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, 9 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
            }
            OnlinePbkCtr.AvTskCtrl.VFeedPointer += ReadSize;
        }
    } else {
        AmbaPrint_PrintStr5("%s, Unknown option! ALIGN32_ENABLE is only for pointer of source buffer", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_NA;
    }

    return Rval;
}

static UINT32 InstGopHeader(UINT8 SkipFirstI, UINT8 SkipLastI, UINT64 Pts, \
                                UINT32 *VFrameSize)
{
    UINT8                       GopHeader[NALU_GOP_SIZE], reserved = 1U;
    UINT16                      high, low;
    UINT32                      TimeScale, NumUnitsInTick, Rval = OK;
    const SVC_DMUX_MOV_INFO_s   *pMov = &(OnlinePbkCtr.DMux);

    if (OK != AmbaWrap_memset(GopHeader, 0, (SIZE_t)NALU_GOP_SIZE)) {
        Rval = ERR_NA;
    }

    if (SVC_DMUX_VID_FORMAT_H264 == pMov->VideoFormat) {
        /* start code, byte 0~3 */
        GopHeader[0] = 0x00U;
        GopHeader[1] = 0x00U;
        GopHeader[2] = 0x00U;
        GopHeader[3] = 0x01U;
        /* NAL header, byte 4 */
        GopHeader[4] = 0x7AU;
        /* Version main, byte 5 */
        GopHeader[5] = 0x01U;
        /* Version sub, byte 6 */
        GopHeader[6] = 0x01U;
    } else if (SVC_DMUX_VID_FORMAT_HEVC == pMov->VideoFormat) {
        /* start code, byte 0~3 */
        GopHeader[0] = 0x00U;
        GopHeader[1] = 0x00U;
        GopHeader[2] = 0x01U;
        GopHeader[3] = 0x34U;
        /* NAL header, byte 4 */
        GopHeader[4] = 0x00U;
        /* Version main, byte 5 */
        GopHeader[5] = 0x01U;
        /* Version sub, byte 6 */
        GopHeader[6] = 0x01U;
    } else {
        AmbaPrint_PrintStr5("%s, Unknown video format", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_NA;
    }

    /* SkipFirstI, byte 7 (bit 7) */
    GopHeader[7] = (UINT8)(SkipFirstI << 7U);
    /* SkipLastI, byte 7 (bit 6) */
    GopHeader[7] |= (UINT8)(SkipLastI << 6U);

    TimeScale      = OnlinePbkCtr.DMux.TimescaleV;
    NumUnitsInTick = OnlinePbkCtr.DMux.SampleDurationV;

    /* Sample duration       */
    /*   high 16 bits        */
    /*     byte 7  (bit 5~0) */
    /*     byte 8            */
    /*     byte 9  (bit 7~6) */
    /*   low 16 bits         */
    /*     byte 9  (bit 4~0) */
    /*     byte 10           */
    /*     byte 11 (bit 7~5) */
    high = (UINT16)((NumUnitsInTick >> 16U) & 0xFFFFU);
    low  = (UINT16)(NumUnitsInTick & 0xFFFFU);
    GopHeader[7]  |= (UINT8)(high >> 10U);
    GopHeader[8]  =  (UINT8)(high >> 2U);
    GopHeader[9]  =  (UINT8)(high << 6U);
    GopHeader[9]  |= (UINT8)(reserved << 5U);
    GopHeader[9]  |= (UINT8)(low >> 11U);
    GopHeader[10] =  (UINT8)(low >> 3U);
    GopHeader[11] =  (UINT8)(low << 5U);
    GopHeader[11] |= (UINT8)(reserved << 4U);
    /* TimeScale              */
    /*   high 16 bits         */
    /*     byte 11  (bit 3~0) */
    /*     byte 12            */
    /*     byte 13  (bit 7~4) */
    /*   low 16 bits          */
    /*     byte 13  (bit 2~0) */
    /*     byte 14            */
    /*     byte 15  (bit 7~3) */
    high = (UINT16)((TimeScale >> 16U) & 0xFFFFU);
    low  = (UINT16)(TimeScale & 0xFFFFU);
    GopHeader[11] |= (UINT8)(high >> 12U);
    GopHeader[12] =  (UINT8)(high >> 4U);
    GopHeader[13] =  (UINT8)(high << 4U);
    GopHeader[13] |= (UINT8)(reserved << 3U);
    GopHeader[13] |= (UINT8)(low >> 13U);
    GopHeader[14] =  (UINT8)(low >> 5U);
    GopHeader[15] =  (UINT8)(low << 3U);
    GopHeader[15] |= (UINT8)(reserved << 2U);
    /* PTS                    */
    /*   high 16 bits         */
    /*     byte 15  (bit 1~0) */
    /*     byte 16            */
    /*     byte 17  (bit 7~2) */
    /*   low 16 bits          */
    /*     byte 17  (bit 0)   */
    /*     byte 18            */
    /*     byte 19  (bit 7~1) */
    high = (UINT16)((Pts >> 16) & 0xFFFFU);
    low  = (UINT16)(Pts & 0xFFFFU);
    GopHeader[15] |= (UINT8)(high >> 14U);
    GopHeader[16] =  (UINT8)(high >> 6U);
    GopHeader[17] =  (UINT8)(high << 2U);
    GopHeader[17] |= (UINT8)(reserved << 1U);
    GopHeader[17] |= (UINT8)(low >> 15U);
    GopHeader[18] =  (UINT8)(low >> 7U);
    GopHeader[19] =  (UINT8)(low << 1U);
    GopHeader[19] |= (UINT8)(reserved);
    /* N, byte 20 */
    GopHeader[20] = (UINT8)pMov->N;
    /* M, byte 21 (bit 7~4) */
    GopHeader[21] = (UINT8)((pMov->M << 4) & 0xF0U);

    if (OK != ReadVideoData(GopHeader, NALU_GOP_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
        AmbaPrint_PrintStr5("%s, Read Video Data failed!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_NA;
    } else {
        *VFrameSize += NALU_GOP_SIZE;
    }

    return Rval;
}

static UINT32 InstSpsPps(UINT32 *VFrameSize)
{
    UINT32                      Rval = OK;

    if (OnlinePbkCtr.DMux.VideoFormat == SVC_DMUX_VID_FORMAT_HEVC) {

        if (0U != OnlinePbkCtr.DMux.Vps_Sps_Pps.VpsSize) {
            /* Feed start code */
            if (OK != ReadVideoData(SCODE, SCODE_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read Start Code failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += SCODE_SIZE;
            }
            /* Feed SPS */
            if (OK != ReadVideoData(OnlinePbkCtr.DMux.Vps_Sps_Pps.Vps,
                                        OnlinePbkCtr.DMux.Vps_Sps_Pps.VpsSize, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read SPS failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += OnlinePbkCtr.DMux.Vps_Sps_Pps.VpsSize;
            }
        }

        if (0U != OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize) {
            /* Feed start code */
            if (OK != ReadVideoData(SCODE, SCODE_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read Start Code failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += SCODE_SIZE;
            }

            /* Feed SPS */
            if (OK != ReadVideoData(OnlinePbkCtr.DMux.Vps_Sps_Pps.Sps,
                                        OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read SPS failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize;
            }
        }

        if (0U != OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize) {
            /* Feed start code */
            if (OK != ReadVideoData(SCODE, SCODE_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read Start Code failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += SCODE_SIZE;
            }
            /* Feed PPS */
            if (OK != ReadVideoData(OnlinePbkCtr.DMux.Vps_Sps_Pps.Pps,
                                        OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read PPS failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize;
            }
        }
    } else if (OnlinePbkCtr.DMux.VideoFormat == SVC_DMUX_VID_FORMAT_H264) {
        if (0U != OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize) {
            /* Feed start code */
            if (OK != ReadVideoData(SCODE, SCODE_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read Start Code failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += SCODE_SIZE;
            }

            /* Feed SPS */
            if (OK != ReadVideoData(OnlinePbkCtr.DMux.Vps_Sps_Pps.Sps,
                                        OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read SPS failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize;
            }
        }

        if (0U != OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize) {
            /* Feed start code */
            if (OK != ReadVideoData(SCODE, SCODE_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read Start Code failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += SCODE_SIZE;
            }

            /* Feed PPS */
            if (OK != ReadVideoData(OnlinePbkCtr.DMux.Vps_Sps_Pps.Pps,
                                        OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize, NEXT_FEED_ALIGN32_DISABLE)) {
                Rval = ERR_NA;
                AmbaPrint_PrintStr5("%s, Read PPS failed!", __func__, NULL, NULL, NULL, NULL);
            } else {
                *VFrameSize += OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s, Unknown video format!", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_NA;
    }

    return Rval;
}

static void SyncAudio(UINT64 StartPts)
{
    UINT64 Tmp;
    UINT32 AStartPTS;

    Tmp = StartPts;
    Tmp *= OnlinePbkCtr.DMux.TimescaleA;
    Tmp /= TIME_SCALE_90K;
    Tmp /= OnlinePbkCtr.DMux.SampleDurationA;

    /* Setup AFeedIdx */
    AmbaMisra_TypeCast(&OnlinePbkCtr.AvTskCtrl.AFeedIdx, &Tmp);
    AmbaMisra_TypeCast(&AStartPTS, &StartPts);

    AmbaPrint_PrintUInt5("SyncAudio: TimescaleA %d, SampleDurationA %d, AStartPTS %d, AFeedIdx %d", \
        OnlinePbkCtr.DMux.TimescaleA, OnlinePbkCtr.DMux.SampleDurationA, AStartPTS, OnlinePbkCtr.AvTskCtrl.AFeedIdx, 0U);
}

static UINT32 CalcVideoFreeSpace(VOID)
{
    UINT32  FreeSpace = 0U, Size = 0U;

    if (OnlinePbkCtr.AvTskCtrl.VQueueRcvSize <= OnlinePbkCtr.AvTskCtrl.VQueueSndSize) {
        FreeSpace = OnlinePbkCtr.AvTskCtrl.VQueueSndSize - OnlinePbkCtr.AvTskCtrl.VQueueRcvSize;
        SvcOnlinePbk_QueryVBufSize(&Size);
        FreeSpace = Size - FreeSpace;
    } else {
        AmbaPrint_PrintUInt5("[CalcVideoFreeSpace]: VQueueRcvSize %d is larger than VQueueSndSize %d", \
                            OnlinePbkCtr.AvTskCtrl.VQueueRcvSize, OnlinePbkCtr.AvTskCtrl.VQueueSndSize, 0U, 0U, 0U);
        FreeSpace = 0U;
    }

    return FreeSpace;
}

static UINT32 CalcAudioFreeSpace(VOID)
{
    UINT32  FreeSpace = 0U, Size = 0U;

    if (OnlinePbkCtr.AvTskCtrl.AQueueRcvSize <= OnlinePbkCtr.AvTskCtrl.AQueueSndSize) {
        FreeSpace = OnlinePbkCtr.AvTskCtrl.AQueueSndSize - OnlinePbkCtr.AvTskCtrl.AQueueRcvSize;
        SvcOnlinePbk_QueryABufSize(&Size);
        FreeSpace = Size - FreeSpace;
    } else {
        AmbaPrint_PrintUInt5("[CalcAudioFreeSpace]: AQueueRcvSize %d is larger than AQueueSndSize %d", \
                            OnlinePbkCtr.AvTskCtrl.AQueueRcvSize, OnlinePbkCtr.AvTskCtrl.AQueueSndSize, 0U, 0U, 0U);
        FreeSpace = 0U;
    }

    return FreeSpace;
}

static void VideoPbkTaskEntry(UINT32 EntryArg)
{
    UINT32                          Rval = OK, LoopFlg = 1U;
    UINT32                          ActualFlags, IsPreload = 1U, FreeSpace = 0U;
    UINT32                          NaluCnt = 0U, SkipFirstB = 0U, FwdSkip = 0U, FeedCnt = 0U;
    UINT32                          SamType, SamSize, SamOffset, SamIdx;
    UINT32                          NaluLen, Tmp, MaxPicSize = 0U;
    UINT64                          Pts;
    ULONG                           VFrameAddr = 0U;
    UINT32                          VFrameSize = 0U, VFramePts = 0U;
    UINT32                          VCachedFileStart = 0U, VCachedFileEnd = 0U, NumSuccess = 0U;
    AMBA_DSP_ENC_PIC_RDY_s          VDesc;

    (VOID)EntryArg;

    ActualFlags = 0U;
    if (KAL_ERR_NONE != AmbaKAL_EventFlagGet(&(OnlinePbkCtr.PbkCtrFlg), \
                                            ONLINE_PBK_VFLG_START_FEED, \
                                            AMBA_KAL_FLAGS_ANY, \
                                            AMBA_KAL_FLAGS_CLEAR_AUTO, \
                                            &ActualFlags, AMBA_KAL_WAIT_FOREVER)) {
        AmbaPrint_PrintStr5("%s, Get Event flag ONLINE_PBK_VFLG_START_FEED failed!", __func__, NULL, NULL, NULL, NULL);
    }

    OnlinePbkCtr.AvTskCtrl.VFeedPointer = OnlinePbkBuf.VBuffAddr;

    while ((OK == Rval) && (1U == LoopFlg)) {
        {
            FwdSkip = 0U;
            /* (1) Get current type, size and offset of sample */
            SamIdx    = OnlinePbkCtr.AvTskCtrl.VFeedIdx;
            SamSize   = OnlinePbkCtr.DMux.VideoFrameContext[SamIdx].FrameSize;
            SamOffset = OnlinePbkCtr.DMux.VideoFrameContext[SamIdx].FileOffset;
            SamType   = OnlinePbkCtr.DMux.VideoFrameContext[SamIdx].FrameType;

            /* Calculate worst case picture size, assume (MAX_NALUS_IN_SAMPLE) NALUs in sample */
            if (SVC_DMUX_VID_FORMAT_HEVC == OnlinePbkCtr.DMux.VideoFormat) {
                MaxPicSize = NALU_GOP_SIZE
                             + (SCODE_SIZE + OnlinePbkCtr.DMux.Vps_Sps_Pps.VpsSize)
                             + (SCODE_SIZE + OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize)
                             + (SCODE_SIZE + OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize)
                             + (MAX_NALUS_IN_SAMPLE * SCODE_SIZE)
                             + SamSize
                             + HEVC_EOF_SIZE
                             + ZEROPADDING_SIZE
                             + MAX_INSERT_ZERO;
            } else if (SVC_DMUX_VID_FORMAT_H264 == OnlinePbkCtr.DMux.VideoFormat) {
                MaxPicSize = NALU_GOP_SIZE
                             + (SCODE_SIZE + OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize)
                             + (SCODE_SIZE + OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize)
                             + (MAX_NALUS_IN_SAMPLE * SCODE_SIZE)
                             + SamSize
                             + H264_EOS_SIZE
                             + ZEROPADDING_SIZE
                             + MAX_INSERT_ZERO;
            } else {
                AmbaPrint_PrintStr5("%s, Unknown video format", __func__, NULL, NULL, NULL, NULL);
                Rval = ERR_NA;
            }

            /* Make sure if there is enough space to feed */
            FreeSpace = CalcVideoFreeSpace();
            if (FreeSpace < MaxPicSize) {
                AmbaKAL_TaskSleep(100);
                continue;
            }

            /* Check frame type */
            if ((SamType != SVC_DMUX_VID_FRAME_IDR) &&
                (SamType != SVC_DMUX_VID_FRAME_I) &&
                (SamType != SVC_DMUX_VID_FRAME_P) &&
                (SamType != SVC_DMUX_VID_FRAME_B)) {
                AmbaPrint_PrintStr5("%s, unknow sample type", __func__, NULL, NULL, NULL, NULL);
            }

            if (1U < OnlinePbkCtr.DMux.M) {
                /* If the first feed picture is I at preload stage, skip B pictures after it until */
                /* next P picture Because they belong to previous GOP. (Ex: IBBP..., remove BB)    */
                if ((1U == IsPreload) && (0U == FeedCnt) && (SamType == SVC_DMUX_VID_FRAME_I)) {
                    SkipFirstB = 1U;
                }

                if (1U == SkipFirstB) {
                    if (SamType == SVC_DMUX_VID_FRAME_P) {
                        SkipFirstB = 0U;
                    }
                    if (SamType == SVC_DMUX_VID_FRAME_B) {
                        FwdSkip = 1U;
                    }
                }
            }

            /* (4) If picture isn't skipped, feed it to bit buffer */
            if (0U == FwdSkip) {
                VFrameAddr = OnlinePbkCtr.AvTskCtrl.VFeedPointer;
                VFramePts = OnlinePbkCtr.AvTskCtrl.LatestDts + (UINT64)(OnlinePbkCtr.DMux.VideoFrameContext[SamIdx].FrameOffset);
                VFrameSize = 0;

                /* Rules to feed GOP and SPS/PPS before pictures              */
                /*    (a) IDR picture                                         */
                /*    (b) The first feed picture is I and is at preload stage */
                if ((SamType == SVC_DMUX_VID_FRAME_IDR) ||
                    ((1U == IsPreload) && (0U == FeedCnt) && (SamType == SVC_DMUX_VID_FRAME_I))) {
                    Pts = OnlinePbkCtr.AvTskCtrl.LatestDts + (UINT64)(OnlinePbkCtr.DMux.VideoFrameContext[SamIdx].FrameOffset);
                    /* Change to 90K base */
                    Pts *= (UINT64)TIME_SCALE_90K;
                    Pts /= (UINT64)OnlinePbkCtr.DMux.TimescaleV;

                    if (OK != InstGopHeader(0, 0, Pts, &VFrameSize)) {
                        AmbaPrint_PrintStr5("%s, Insert Gop Header failed!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ERR_NA;
                    }
                    if (OK != InstSpsPps(&VFrameSize)) {
                        AmbaPrint_PrintStr5("%s, Insert SPS PPS failed!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ERR_NA;
                    }
                    AmbaPrint_PrintStr5("%s, [GOP(%llu) SPS/PPS]", __func__, NULL, NULL, NULL, NULL);
                }

                {
                    VCachedFileStart = OnlinePbkCtr.AvTskCtrl.VCachedFileOff;
                    VCachedFileEnd = OnlinePbkCtr.AvTskCtrl.VCachedFileOff + OnlinePbkCtr.AvTskCtrl.VCachedSize;
                    if ((SamOffset>=VCachedFileStart) && ((SamOffset+SamSize)<=VCachedFileEnd)) {
                        // Do nothing
                    } else {
                        if (SamSize >= ONLINE_PBK_VCACHE_SIZE) {
                            AmbaPrint_PrintUInt5("SamSize %d is larger than vcache size %d", SamSize, \
                                                ONLINE_PBK_VCACHE_SIZE, 0U, 0U, 0U);
                        }

                        if (AMBA_FS_ERR_NA != SvcVFS_Seek(&(OnlinePbkCtr.VideoFile), (INT64)SamOffset, PF_SEEK_SET)) {
                            AmbaPrint_PrintStr5("%s, Seek to Feed Video failed!", __func__, NULL, NULL, NULL, NULL);
                            Rval = ERR_NA;
                        } else {
                            AmbaKAL_MutexTake(&(OnlinePbkCtr.FileReadMutex), TX_WAIT_FOREVER);
                            (void)SvcVFS_Read(OnlinePbkCtr.AvTskCtrl.VCacheBuf, 1U, ONLINE_PBK_VCACHE_SIZE, \
                                            &(OnlinePbkCtr.VideoFile), &NumSuccess);
                            if (NumSuccess != ONLINE_PBK_VCACHE_SIZE) {
                                if (1U == OnlinePbkCtr.AvTskCtrl.FileError) {
                                    OnlinePbkCtr.AvTskCtrl.ForceEos = 1U;
                                    OnlinePbkCtr.AvTskCtrl.FileError = 0U;
                                    LoopFlg = 0U;
                                }
                                AmbaPrint_PrintUInt5("Feed VCacheBuf with size %d", NumSuccess, 0U, 0U, 0U, 0U);
                                OnlinePbkCtr.AvTskCtrl.FileError = 1U;
                            }
                            AmbaKAL_MutexGive(&(OnlinePbkCtr.FileReadMutex));
                            OnlinePbkCtr.AvTskCtrl.VCachedFileOff = SamOffset;
                            OnlinePbkCtr.AvTskCtrl.VCachedSize = NumSuccess;
                        }
                    }

                    OnlinePbkCtr.AvTskCtrl.VideoDataPtr = OnlinePbkCtr.AvTskCtrl.VCacheBuf + (SamOffset-OnlinePbkCtr.AvTskCtrl.VCachedFileOff);
                }
                /* Start to feed video */
                while (0U != SamSize) {
                    if (1U == OnlinePbkCtr.DMux.NALULenSize) {
                        NaluLen = Big2Little_GetByte(&(OnlinePbkCtr.VideoFile));
                    } else if (2U == OnlinePbkCtr.DMux.NALULenSize) {
                        NaluLen = Big2Little_GetBe16(&(OnlinePbkCtr.VideoFile));
                    } else {
                        NaluLen = Big2Little_GetBe32(&(OnlinePbkCtr.VideoFile));
                    }

                    /* Feed start code */
                    if (OK != ReadVideoData(SCODE, SCODE_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
                        AmbaPrint_PrintStr5("%s, Read Start Code failed!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ERR_NA;
                    } else {
                        VFrameSize += SCODE_SIZE;
                    }

                    /* Feed NALU */
                    if (OK != ReadVideoData(NULL, NaluLen, NEXT_FEED_ALIGN32_DISABLE)) {
                        AmbaPrint_PrintStr5("%s, Read NALU failed!", __func__, NULL, NULL, NULL, NULL);
                        Rval = ERR_NA;
                    } else {
                        VFrameSize += NaluLen;
                    }

                    Tmp = OnlinePbkCtr.DMux.NALULenSize + NaluLen;
                    if (Tmp <= SamSize) {
                        SamSize -= Tmp;
                    } else {
                        AmbaPrint_PrintUInt5("[FwdFeedVideo]: something wrong", 0U, 0U, 0U, 0U, 0U);
                        AmbaPrint_PrintUInt5("NALULenSize = %u, NaluLen = %u", OnlinePbkCtr.DMux.NALULenSize, NaluLen, 0U, 0U, 0U);
                        AmbaPrint_PrintUInt5("SamSize = %u, FeedCnt = %u", SamSize, FeedCnt, 0U, 0U, 0U);
                    }

                    NaluCnt++;
                }

                /* Check NALUs in sample */
                if (MAX_NALUS_IN_SAMPLE < NaluCnt) {
                    AmbaPrint_PrintUInt5("NALUs(%u) in sample(%u) is bigger than assumption", NaluCnt, SamIdx, 0U, 0U, 0U);
                }
                NaluCnt = 0U;

                /* Feed zero padding for CAVLC workaround */
                if (OK != ReadVideoData(ZEROPADDING, ZEROPADDING_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
                    AmbaPrint_PrintUInt5("Read zero padding failed!", 0U, 0U, 0U, 0U, 0U);
                    Rval = ERR_NA;
                } else {
                    VFrameSize += ZEROPADDING_SIZE;
                }
            }
            OnlinePbkCtr.AvTskCtrl.LatestDts += OnlinePbkCtr.DMux.SampleDurationV;
            OnlinePbkCtr.AvTskCtrl.VFeedIdx++;
            FeedCnt++;

            /* If file end */
            if (OnlinePbkCtr.AvTskCtrl.VFeedIdx == OnlinePbkCtr.DMux.VideoFrameTotalNumberCount) {
                if (SVC_DMUX_VID_FORMAT_H264 == OnlinePbkCtr.DMux.VideoFormat) {
                    /* Feed H264_END_OF_STREAM */
                    if (OK != ReadVideoData(H264_END_OF_STREAM, H264_EOS_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
                       AmbaPrint_PrintUInt5("Insert Eos Failed", 0U, 0U, 0U, 0U, 0U);
                       Rval = ERR_NA;
                    } else {
                        VFrameSize += H264_EOS_SIZE;
                    }
                    AmbaPrint_PrintUInt5("FeedVideo [H264][EOF]", 0U, 0U, 0U, 0U, 0U);
                } else if (SVC_DMUX_VID_FORMAT_HEVC == OnlinePbkCtr.DMux.VideoFormat) {
                    /* Feed HEVC_END_OF_STREAM */
                    if (OK != ReadVideoData(HEVC_END_OF_STREAM, HEVC_EOF_SIZE, NEXT_FEED_ALIGN32_DISABLE)) {
                       AmbaPrint_PrintUInt5("Insert Eos Failed", 0U, 0U, 0U, 0U, 0U);
                       Rval = ERR_NA;
                    } else {
                        VFrameSize += HEVC_EOF_SIZE;
                    }
                    AmbaPrint_PrintUInt5("FeedVideo [HEVC][EOF]", 0U, 0U, 0U, 0U, 0U);
                } else {
                    AmbaPrint_PrintUInt5("Unknown video format!!", 0U, 0U, 0U, 0U, 0U);
                    Rval = ERR_NA;
                }
                OnlinePbkCtr.AvTskCtrl.VFeedEof = 1;
                LoopFlg = 0U;
            }
            AmbaWrap_memset(&VDesc, 0, sizeof(VDesc));
            VDesc.Pts = OnlinePbkCtr.AvTskCtrl.LatestDts;
            VDesc.StartAddr = VFrameAddr;
            VDesc.FrameType = SamType;

            if ((1U == OnlinePbkCtr.AvTskCtrl.VFeedEof) || (1U == OnlinePbkCtr.AvTskCtrl.ForceEos)) {
                VDesc.PicSize = AMBA_DSP_ENC_END_MARK;
            } else {
                VDesc.PicSize = VFrameSize;
            }

            VDesc.FrmNo = OnlinePbkCtr.AvTskCtrl.VFeedIdx-1;

            Rval = AmbaKAL_MsgQueueSend(&(OnlinePbkCtr.VMsgQueue), &VDesc, AMBA_KAL_WAIT_FOREVER);
            if (KAL_ERR_NONE != Rval) {
                AmbaPrint_PrintUInt5("[VideoPbkTaskEntry] Fail to send msg queue(%u)", Rval,
                    0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            } else {
                if (1U != OnlinePbkCtr.AvTskCtrl.VFeedEof) {
                    OnlinePbkCtr.AvTskCtrl.VQueueSndSize += VFrameSize;
                }
            }
        }
    }
}

static void AudioPbkTaskEntry(UINT32 EntryArg)
{
    UINT32                      Rval = OK, LoopFlg = 1U, ActualFlags;
    UINT32                      SamSize, SamOffset, FreeSpace = 0U;
    UINT32                      ReadSize = 0U;
    UINT32                      Left, NumSuccess = 0U;
    ULONG                       pLimitUL, AFrameAddr;
    void                        *FeedPointer, *pBase;
    UINT32                      ACachedFileStart = 0U, ACachedFileEnd = 0U;
    UINT8                       *AudioDataPtr = NULL;
    AMBA_AENC_AUDIO_DESC_s      ADesc;

    (VOID)EntryArg;
    OnlinePbkCtr.AvTskCtrl.AFeedPointer = OnlinePbkBuf.ABuffAddr;

    ActualFlags = 0U;
    if (KAL_ERR_NONE != AmbaKAL_EventFlagGet(&(OnlinePbkCtr.PbkCtrFlg), \
                                            ONLINE_PBK_AFLG_START_FEED, \
                                            AMBA_KAL_FLAGS_ANY, \
                                            AMBA_KAL_FLAGS_CLEAR_AUTO, \
                                            &ActualFlags, AMBA_KAL_WAIT_FOREVER)) {
        AmbaPrint_PrintStr5("%s, Get Event flag ONLINE_PBK_AFLG_START_FEED failed!", __func__, NULL, NULL, NULL, NULL);
    }

    OnlinePbkCtr.AvTskCtrl.AFeedPointer = OnlinePbkBuf.ABuffAddr;

    while ((OK == Rval) && (1U == LoopFlg)) {
        SamSize   = OnlinePbkCtr.DMux.AudioFrameContext[OnlinePbkCtr.AvTskCtrl.AFeedIdx].FrameSize;
        SamOffset = OnlinePbkCtr.DMux.AudioFrameContext[OnlinePbkCtr.AvTskCtrl.AFeedIdx].FileOffset;

        /* Make sure if there is enough space to feed */
        FreeSpace = CalcAudioFreeSpace();
        if (FreeSpace < SamSize) {
            AmbaKAL_TaskSleep(100);
            continue;
        }

        {
            ACachedFileStart = OnlinePbkCtr.AvTskCtrl.ACachedFileOff;
            ACachedFileEnd = OnlinePbkCtr.AvTskCtrl.ACachedFileOff + OnlinePbkCtr.AvTskCtrl.ACachedSize;
            if ((SamOffset>=ACachedFileStart) && ((SamOffset+SamSize)<=ACachedFileEnd)) {
                // Do nothing
            } else {
                if (AMBA_FS_ERR_NA != AmbaFS_FileSeek(OnlinePbkCtr.AudioFile.pFile, (INT64)SamOffset, PF_SEEK_SET)) {
                    AmbaPrint_PrintStr5("%s, Seek to Feed Audio failed!", __func__, NULL, NULL, NULL, NULL);
                    Rval = ERR_NA;
                } else {
                    AmbaKAL_MutexTake(&(OnlinePbkCtr.FileReadMutex), TX_WAIT_FOREVER);
                    (void)AmbaFS_FileRead(OnlinePbkCtr.AvTskCtrl.ACacheBuf, 1, ONLINE_PBK_ACACHE_SIZE, \
                                    OnlinePbkCtr.AudioFile.pFile, &NumSuccess);
                    if (NumSuccess != ONLINE_PBK_ACACHE_SIZE) {
                        AmbaPrint_PrintUInt5("Feed ACacheBuf with size %d", NumSuccess, 0U, 0U, 0U, 0U);
                    }
                    AmbaKAL_MutexGive(&(OnlinePbkCtr.FileReadMutex));

                    OnlinePbkCtr.AvTskCtrl.ACachedFileOff = SamOffset;
                    OnlinePbkCtr.AvTskCtrl.ACachedSize = NumSuccess;
                }
            }
            AudioDataPtr = OnlinePbkCtr.AvTskCtrl.ACacheBuf + (SamOffset-OnlinePbkCtr.AvTskCtrl.ACachedFileOff);
        }

        AFrameAddr = OnlinePbkCtr.AvTskCtrl.AFeedPointer;

        {
            AmbaMisra_TypeCast(&pBase, &OnlinePbkBuf.ABuffAddr);
            pLimitUL = OnlinePbkBuf.ABuffAddr + OnlinePbkBuf.ABuffSize - 1U;
            ReadSize = OnlinePbkCtr.DMux.AudioFrameContext[OnlinePbkCtr.AvTskCtrl.AFeedIdx].FrameSize;

            if (pLimitUL <= (OnlinePbkCtr.AvTskCtrl.AFeedPointer + ReadSize - 1U)) {
                Left = (pLimitUL - OnlinePbkCtr.AvTskCtrl.AFeedPointer) + 1U;
                {
                    AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.AFeedPointer));
                    if (Left && OK != AmbaWrap_memcpy(FeedPointer, AudioDataPtr, Left)) {
                        Rval = ERR_NA;
                        AmbaPrint_PrintStr5("%s, 1 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        AudioDataPtr += Left;
                    }

                    Left = ReadSize - Left;
                    AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkBuf.ABuffAddr));
                    if (Left && OK != AmbaWrap_memcpy(FeedPointer, AudioDataPtr, Left)) {
                        Rval = ERR_NA;
                        AmbaPrint_PrintStr5("%s, 2 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        AudioDataPtr += Left;
                    }
                }
                OnlinePbkCtr.AvTskCtrl.AFeedPointer = (OnlinePbkBuf.ABuffAddr + Left);
            } else {
                AmbaMisra_TypeCast(&FeedPointer, &(OnlinePbkCtr.AvTskCtrl.AFeedPointer));
                if (OK != AmbaWrap_memcpy(FeedPointer, AudioDataPtr, ReadSize)) {
                    Rval = ERR_NA;
                    AmbaPrint_PrintStr5("%s, 3 memcpy failed!", __func__, NULL, NULL, NULL, NULL);
                } else {
                    AudioDataPtr += ReadSize;
                }

                OnlinePbkCtr.AvTskCtrl.AFeedPointer += ReadSize;
            }
        }

        if (AmbaWrap_memset(&ADesc, 0, sizeof(ADesc))!= 0U) { }

        ADesc.EncodedSamples = 1024 *(OnlinePbkCtr.AvTskCtrl.AFeedIdx);
        AmbaMisra_TypeCast(&(ADesc.pBufAddr), &AFrameAddr);
        ADesc.DataSize = SamSize;

        Rval = AmbaKAL_MsgQueueSend(&(OnlinePbkCtr.AMsgQueue), &ADesc, AMBA_KAL_WAIT_FOREVER);
        if (KAL_ERR_NONE != Rval) {
            AmbaPrint_PrintUInt5("[AudioPbkTaskEntry] Fail to send msg queue(%u)", Rval,
                0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        } else {
            OnlinePbkCtr.AvTskCtrl.AQueueSndSize += SamSize;
        }

        OnlinePbkCtr.AvTskCtrl.AFeedIdx++;

        /* If file end */
        if ((OnlinePbkCtr.AvTskCtrl.AFeedIdx == OnlinePbkCtr.DMux.AudioFrameTotalNumberCount) || (1U == OnlinePbkCtr.AvTskCtrl.ForceEos)) {
            OnlinePbkCtr.AvTskCtrl.AFeedEof = 1;
            LoopFlg = 0U;

            ADesc.DataSize = AMBA_DSP_ENC_END_MARK;
            ADesc.Eos = 1U;
            Rval = AmbaKAL_MsgQueueSend(&(OnlinePbkCtr.AMsgQueue), &ADesc, AMBA_KAL_WAIT_FOREVER);
            if (KAL_ERR_NONE != Rval) {
                AmbaPrint_PrintUInt5("[AudioPbkTaskEntry] Fail to send msg queue(%u)", Rval,
                    0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }
    }
}

static void VideoPbkHandler(UINT32 EntryArg)
{
    UINT32                          Rval = OK, LoopFlg = 1U, ActualFlags = 0U;
    AMBA_DSP_ENC_PIC_RDY_s          VDesc;
    UINT32                          StartTime = 0U, EndTime = 0U, DiffTime = 0U;
    UINT32                          DeltaFrameTime = 0U, PauseFlg = 0U;

    (VOID)EntryArg;
    while (1U == LoopFlg) {
        ActualFlags = 0U;
        AmbaKAL_EventFlagGet(&(OnlinePbkCtr.PbkCtrFlg), \
                            ONLINE_VIDEO_HANDLER_PAUSE | ONLINE_VIDEO_HANDLER_RESUME, \
                            AMBA_KAL_FLAGS_ANY, \
                            AMBA_KAL_FLAGS_CLEAR_AUTO, \
                            &ActualFlags, AMBA_KAL_NO_WAIT);

        if (0U != (ActualFlags & ONLINE_VIDEO_HANDLER_PAUSE)) {
            PauseFlg = 1U;
            AmbaKAL_TaskSleep(10);
            continue;
        }

        if (0U != (ActualFlags & ONLINE_VIDEO_HANDLER_RESUME)) {
            PauseFlg = 0U;
            OnlinePbkCtr.AvTskCtrl.DeltaVFrameNum = 0U;
            AmbaPrint_PrintUInt5("[VideoPbkHandler] DeltaVFrameNum", 0U,
                                0U, 0U, 0U, 0U);
        }

        if (1U == PauseFlg) {
            AmbaKAL_TaskSleep(10);
            continue;
        }

        if (AmbaWrap_memset(&VDesc, 0, sizeof(AMBA_DSP_ENC_PIC_RDY_s))!= 0U) { }
        Rval = AmbaKAL_MsgQueueReceive(&(OnlinePbkCtr.VMsgQueue), &VDesc, AMBA_KAL_WAIT_FOREVER);
        if (KAL_ERR_NONE != Rval) {
            AmbaPrint_PrintUInt5("[VideoPbkHandler] Fail to receive msg queue(%u)", Rval,
                                0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        } else {
            OnlinePbkCtr.AvTskCtrl.DeltaVFrameNum++;
            DeltaFrameTime = (UINT64)1000*OnlinePbkCtr.AvTskCtrl.DeltaVFrameNum*OnlinePbkCtr.DMux.SampleDurationV/OnlinePbkCtr.DMux.TimescaleV;

            AmbaKAL_MutexTake(&(OnlinePbkCtr.IpcMutex), TX_WAIT_FOREVER);
            Rval = SvcRtspLink_PbNotify(0, RTSP_LINK_NOTIFY_VID, &VDesc);
            AmbaKAL_MutexGive(&OnlinePbkCtr.IpcMutex);

            if (OK == Rval) {
                if (AMBA_DSP_ENC_END_MARK != VDesc.PicSize) {
                    OnlinePbkCtr.AvTskCtrl.VQueueRcvSize += VDesc.PicSize;
                }

                if (1U == OnlinePbkCtr.AvTskCtrl.DeltaVFrameNum) {
                    (VOID)AmbaKAL_GetSysTickCount(&StartTime);
                    AmbaPrint_PrintUInt5("VideoPbkHandler Start In System Time %d ms", StartTime, 0U, 0U, 0U, 0U);
                }

                {
                    (VOID)AmbaKAL_GetSysTickCount(&EndTime);
                    DiffTime = EndTime - StartTime;

                    if (DeltaFrameTime > DiffTime) {
                        AmbaKAL_TaskSleep(DeltaFrameTime-DiffTime);
                    } else {
                        AmbaPrint_PrintUInt5("VideoPbkHandler exception ExpectedTime %dms, Frame Number= %d, RealTime %d, Slow %dms, StartTime %dms", \
                        DeltaFrameTime, OnlinePbkCtr.AvTskCtrl.DeltaVFrameNum, DiffTime, DiffTime - DeltaFrameTime, StartTime);
                    }
                }
            }
        }
    }
}

static void AudioPbkHandler(UINT32 EntryArg)
{
    UINT32                          Rval = OK, LoopFlg = 1U, ActualFlags = 0U;
    AMBA_AENC_AUDIO_DESC_s          ADesc;
    UINT32                          StartTime = 0U, EndTime = 0U, DiffTime = 0U;
    UINT32                          DeltaFrameTime = 0U, PauseFlg = 0U;

    (VOID)EntryArg;
    while (1U == LoopFlg) {
        ActualFlags = 0U;
        AmbaKAL_EventFlagGet(&(OnlinePbkCtr.PbkCtrFlg), \
                            ONLINE_AUDIO_HANDLER_PAUSE | ONLINE_AUDIO_HANDLER_RESUME, \
                            AMBA_KAL_FLAGS_ANY, \
                            AMBA_KAL_FLAGS_CLEAR_AUTO, \
                            &ActualFlags, AMBA_KAL_NO_WAIT);

        if (0U != (ActualFlags & ONLINE_AUDIO_HANDLER_PAUSE)) {
            PauseFlg = 1U;
            AmbaKAL_TaskSleep(10);
            continue;
        }

        if (0U != (ActualFlags & ONLINE_AUDIO_HANDLER_RESUME)) {
            OnlinePbkCtr.AvTskCtrl.DeltaAFrameNum = 0U;
            PauseFlg = 0U;
        }

        if (1U == PauseFlg) {
            AmbaKAL_TaskSleep(10);
            continue;
        }

        if (AmbaWrap_memset(&ADesc, 0, sizeof(AMBA_AENC_AUDIO_DESC_s))!= 0U) { }
        Rval = AmbaKAL_MsgQueueReceive(&(OnlinePbkCtr.AMsgQueue), &ADesc, AMBA_KAL_WAIT_FOREVER);
        if (KAL_ERR_NONE != Rval) {
            AmbaPrint_PrintUInt5("[AudioPbkHandler] Fail to receive msg queue(%u)", Rval,
                                0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        } else {
            OnlinePbkCtr.AvTskCtrl.DeltaAFrameNum++;
            DeltaFrameTime = (UINT64)1000*OnlinePbkCtr.AvTskCtrl.DeltaAFrameNum*OnlinePbkCtr.DMux.SampleDurationA/OnlinePbkCtr.DMux.TimescaleA;
            AmbaKAL_MutexTake(&(OnlinePbkCtr.IpcMutex), TX_WAIT_FOREVER);
            Rval = SvcRtspLink_PbNotify(0, RTSP_LINK_NOTIFY_AUD, &ADesc);
            AmbaKAL_MutexGive(&OnlinePbkCtr.IpcMutex);

            if (OK == Rval) {
                OnlinePbkCtr.AvTskCtrl.AQueueRcvSize += ADesc.DataSize;

                if (1U == OnlinePbkCtr.AvTskCtrl.DeltaAFrameNum) {
                    (VOID)AmbaKAL_GetSysTickCount(&StartTime);
                    AmbaPrint_PrintUInt5("AudioPbkHandler Start In System Time %d ms", StartTime, 0U, 0U, 0U, 0U);
                }

                {
                    (VOID)AmbaKAL_GetSysTickCount(&EndTime);
                    DiffTime = EndTime - StartTime;

                    if (DeltaFrameTime > DiffTime) {
                        AmbaKAL_TaskSleep(DeltaFrameTime-DiffTime);
                    } else if (DeltaFrameTime < DiffTime){
                        AmbaPrint_PrintUInt5("AudioPbkHandler exception ExpectedTime %dms, Frame Number= %d, RealTime %d, Slow %dms", \
                        DeltaFrameTime, OnlinePbkCtr.AvTskCtrl.DeltaAFrameNum, DiffTime, DiffTime - DeltaFrameTime, 0U);
                    }
                }
            }
        }
    }
}

UINT32 SvcOnlinePbk_Init(char *FileName)
{
    UINT32                      Rval = OK, Err;
    SVC_DMUX_BUF_INFO_s         DMuxBufInfo;

    if (1U == OnlinePbkCtr.InitFlg) {
        AmbaPrint_PrintStr5("%s, SvcOnlinePbk_Init has already been inited", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_NA;
    } else {
        {
            DMuxBufInfo.BufAddr = OnlinePbkBuf.DemuxBuffAddr;
            DMuxBufInfo.BufSize = OnlinePbkBuf.DemuxBuffSize;

            if (OK != SvcVFS_Open(FileName, "r", &(OnlinePbkCtr.VideoFile))) {
                AmbaPrint_PrintStr5("%s, Fail to Open file: %s", __func__, FileName, NULL, NULL, NULL);
                Rval = ERR_NA;
            } else {
                (void)SvcVFS_Open(FileName, "r", &(OnlinePbkCtr.AudioFile));
                if (OnlinePbkCtr.AudioFile.pFile == NULL) {
                    AmbaPrint_PrintStr5("%s, Fail to Open AudioFile file: %s", __func__, FileName, NULL, NULL, NULL);
                    Rval = ERR_NA;
                }
            }

            if (Rval == OK) {
                AmbaWrap_memset(&(OnlinePbkCtr.DMux), 0, sizeof(SVC_DMUX_MOV_INFO_s));

                Err = SvcMp4Par_ParseMovie(&(OnlinePbkCtr.VideoFile), SVC_DMUX_CONT_FORMAT_MP4, &DMuxBufInfo, &(OnlinePbkCtr.DMux));
                if (Err != OK) {
                    AmbaPrint_PrintStr5("%s, Fail to ParseMovie", __func__, NULL, NULL, NULL, NULL);
                    Rval = ERR_NA;
                }
            }
        }

        {
            if (KAL_ERR_NONE != AmbaKAL_EventFlagCreate(&(OnlinePbkCtr.PbkCtrFlg), "OnlinePbkCtrlFlg")) {
                AmbaPrint_PrintStr5("%s, Fail to create OnlinePbkCtrlFlg", __func__, NULL, NULL, NULL, NULL);
                Rval = ERR_NA;
            }

            if (KAL_ERR_NONE != AmbaKAL_EventFlagClear(&(OnlinePbkCtr.PbkCtrFlg), 0xffffffffU)) {
                AmbaPrint_PrintStr5("%s, Fail to Clear OnlinePbkCtrlFlgV", __func__, NULL, NULL, NULL, NULL);
                Rval = ERR_NA;
            }
        }

        /* create mutex to avoid task deleted when file reading. */
        Err = AmbaKAL_MutexCreate(&(OnlinePbkCtr.FileReadMutex),"FileReadMutex");
        if (KAL_ERR_NONE != Err) {
            AmbaPrint_PrintUInt5("Fail to create FileReadMutex(%u)", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        /* create mutex to protect ipc process. */
        Err = AmbaKAL_MutexCreate(&(OnlinePbkCtr.IpcMutex),"IpcMutex");
        if (KAL_ERR_NONE != Err) {
            AmbaPrint_PrintUInt5("Fail to create IpcMutex(%u)", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        /* create msg queue */
        Err = AmbaKAL_MsgQueueCreate(&(OnlinePbkCtr.VMsgQueue), "OnlinePbkMsgV", sizeof(AMBA_DSP_ENC_PIC_RDY_s),
                                     OnlinePbkCtr.VQueueBuf, ONLINE_PBK_MSG_NUM * sizeof(AMBA_DSP_ENC_PIC_RDY_s));
        if (KAL_ERR_NONE != Err) {
            AmbaPrint_PrintUInt5("Fail to create OnlinePbkMsgV(%u)", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        Err = AmbaKAL_MsgQueueFlush(&(OnlinePbkCtr.VMsgQueue));
        if (KAL_ERR_NONE != Err) {
            AmbaPrint_PrintUInt5("Fail to flush OnlinePbkMsgV(%u)", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        Err = AmbaKAL_MsgQueueCreate(&(OnlinePbkCtr.AMsgQueue), "OnlinePbkMsgA", sizeof(AMBA_AENC_AUDIO_DESC_s),
                                     OnlinePbkCtr.AQueueBuf, ONLINE_PBK_MSG_NUM * sizeof(AMBA_AENC_AUDIO_DESC_s));
        if (KAL_ERR_NONE != Err) {
            AmbaPrint_PrintUInt5("Fail to create OnlinePbkMsgA(%u)", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        Err = AmbaKAL_MsgQueueFlush(&(OnlinePbkCtr.AMsgQueue));
        if (KAL_ERR_NONE != Err) {
            AmbaPrint_PrintUInt5("Fail to flush OnlinePbkMsgA(%u)", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        if (OnlinePbkCtr.DMux.VideoTrackExist)
        {
            VideoTskCtrl.Priority   = ONLINE_PBK_TASK_0_PRI;
            VideoTskCtrl.EntryFunc  = VideoPbkTaskEntry;
            VideoTskCtrl.pStackBase = VideoTskStack;
            VideoTskCtrl.StackSize  = ONLINE_PBK_STACK_SIZE;
            VideoTskCtrl.CpuBits    = ONLINE_PBK_TASK_0_CPU_BITS;
            VideoTskCtrl.EntryArg   = 0x0U;

            Rval = SvcTask_Create("OnlineVPbkTask", &VideoTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("OnlineVPbkTask create failed(%u)", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OnlinePbkCtr.DMux.AudioTrackExist)
        {
            AudioTskCtrl.Priority   = ONLINE_PBK_TASK_0_PRI;
            AudioTskCtrl.EntryFunc  = AudioPbkTaskEntry;
            AudioTskCtrl.pStackBase = AudioTskStack;
            AudioTskCtrl.StackSize  = ONLINE_PBK_STACK_SIZE;
            AudioTskCtrl.CpuBits    = ONLINE_PBK_TASK_0_CPU_BITS;
            AudioTskCtrl.EntryArg   = 0x0U;

            Rval = SvcTask_Create("OnlineAPbkTask", &AudioTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("OnlineAPbkTask create failed(%u)", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OnlinePbkCtr.DMux.VideoTrackExist)
        {
            VideoHandlerTskCtrl.Priority   = ONLINE_PBK_TASK_0_PRI;
            VideoHandlerTskCtrl.EntryFunc  = VideoPbkHandler;
            VideoHandlerTskCtrl.pStackBase = VideoHandlerTskStack;
            VideoHandlerTskCtrl.StackSize  = ONLINE_PBK_STACK_SIZE;
            VideoHandlerTskCtrl.CpuBits    = ONLINE_PBK_TASK_0_CPU_BITS;
            VideoHandlerTskCtrl.EntryArg   = 0x0U;

            Rval = SvcTask_Create("OnlineVPbkHandler", &VideoHandlerTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("OnlineVPbkHandler create failed(%u)", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OnlinePbkCtr.DMux.AudioTrackExist)
        {
           AudioHandlerTskCtrl.Priority   = ONLINE_PBK_TASK_0_PRI;
           AudioHandlerTskCtrl.EntryFunc  = AudioPbkHandler;
           AudioHandlerTskCtrl.pStackBase = AudioHandlerTskStack;
           AudioHandlerTskCtrl.StackSize  = ONLINE_PBK_STACK_SIZE;
           AudioHandlerTskCtrl.CpuBits    = ONLINE_PBK_TASK_0_CPU_BITS;
           AudioHandlerTskCtrl.EntryArg   = 0x0U;

           Rval = SvcTask_Create("OnlineAPbkHandler", &AudioHandlerTskCtrl);
           if (Rval != OK) {
               AmbaPrint_PrintUInt5("OnlineAPbkHandler create failed(%u)", Rval, 0U, 0U, 0U, 0U);
               Rval = ERR_NA;
           }
        }

        if (Rval == OK) {
            OnlinePbkCtr.InitFlg = 1U;
        } else {
            OnlinePbkCtr.InitFlg = 0U;
            AmbaPrint_PrintUInt5("OnlinePbk_Init Failed.clear InitFlg.", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return Rval;
}

static UINT32 SvcOnlinePbk_DeInit(VOID)
{
    UINT32  Rval = OK;

    if (OnlinePbkCtr.InitFlg == 1U) {
        AmbaKAL_MutexTake(&OnlinePbkCtr.FileReadMutex, TX_WAIT_FOREVER);
        AmbaKAL_MutexTake(&OnlinePbkCtr.IpcMutex, TX_WAIT_FOREVER);

        if (OnlinePbkCtr.DMux.VideoTrackExist) {
            Rval = SvcTask_Destroy(&VideoTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("failed to destroy VideoTskCtrl", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OnlinePbkCtr.DMux.AudioTrackExist) {
            Rval = SvcTask_Destroy(&AudioTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("failed to destroy AudioTskCtrl", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OnlinePbkCtr.DMux.VideoTrackExist) {
            Rval = SvcTask_Destroy(&VideoHandlerTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("failed to destroy VideoHandlerTskCtrl", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OnlinePbkCtr.DMux.AudioTrackExist) {
            Rval = SvcTask_Destroy(&AudioHandlerTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("failed to destroy AudioHandlerTskCtrl", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OK != SvcVFS_Close(&(OnlinePbkCtr.VideoFile))) {
            AmbaPrint_PrintUInt5("Close VideoFile failed!", 0U, 0U, 0U, 0U, 0U);
        }

        Rval = AmbaFS_FileClose(OnlinePbkCtr.AudioFile.pFile);
        if (Rval != AMBA_FS_ERR_NA) {
            AmbaPrint_PrintUInt5("Close AudioFile failed!", 0U, 0U, 0U, 0U, 0U);
        }

        if (OK != AmbaKAL_EventFlagDelete(&(OnlinePbkCtr.PbkCtrFlg))) {
            AmbaPrint_PrintUInt5("failed to delete PbkCtrFlg", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        Rval = AmbaKAL_MsgQueueDelete(&(OnlinePbkCtr.VMsgQueue));
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintUInt5("failed to delete VMsgQueue", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        Rval = AmbaKAL_MsgQueueDelete(&(OnlinePbkCtr.AMsgQueue));
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintUInt5("failed to delete AMsgQueue", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        AmbaKAL_MutexGive(&OnlinePbkCtr.FileReadMutex);
        Rval = AmbaKAL_MutexDelete(&(OnlinePbkCtr.FileReadMutex));
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintUInt5("failed to delete FileReadMutex", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }
        AmbaKAL_MutexGive(&OnlinePbkCtr.IpcMutex);
        Rval = AmbaKAL_MutexDelete(&(OnlinePbkCtr.IpcMutex));
        if (Rval != KAL_ERR_NONE) {
            AmbaPrint_PrintUInt5("failed to delete IpcMutex", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        if (AmbaWrap_memset(&OnlinePbkCtr, 0, sizeof(SVC_ONLINE_PBK_Ctr_s))!= 0U) { }
    }

    return Rval;
}

static UINT32 PbkSeekReset(VOID)
{
    UINT32  Rval = OK;

    if (OnlinePbkCtr.InitFlg == 1U) {
        AmbaKAL_MutexTake(&(OnlinePbkCtr.FileReadMutex), TX_WAIT_FOREVER);
        AmbaKAL_MutexTake(&(OnlinePbkCtr.IpcMutex), TX_WAIT_FOREVER);
        if (OnlinePbkCtr.DMux.VideoTrackExist) {
            Rval = SvcTask_Destroy(&VideoTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("failed to destroy VideoTskCtrl", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }

            Rval = SvcTask_Destroy(&VideoHandlerTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("failed to destroy VideoHandlerTskCtrl", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }


        if (OnlinePbkCtr.DMux.AudioTrackExist) {
            Rval = SvcTask_Destroy(&AudioTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("failed to destroy AudioTskCtrl", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }

            Rval = SvcTask_Destroy(&AudioHandlerTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("failed to destroy AudioHandlerTskCtrl", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }
        AmbaKAL_MutexGive(&(OnlinePbkCtr.FileReadMutex));
        AmbaKAL_MutexGive(&(OnlinePbkCtr.IpcMutex));
        if (AmbaWrap_memset(&(OnlinePbkCtr.AvTskCtrl), 0, sizeof(SVC_ONLINE_PBK_AVTSK_Ctr_s))!= 0U) { }
    }

    return Rval;
}

UINT32 PbkSeekStart(VOID)
{
    UINT32  Rval = OK, Err;

    if (1U == OnlinePbkCtr.InitFlg) {
        if (KAL_ERR_NONE != AmbaKAL_EventFlagClear(&(OnlinePbkCtr.PbkCtrFlg), 0xffffffffU)) {
            AmbaPrint_PrintStr5("%s, Fail to Clear OnlinePbkCtrlFlgV", __func__, NULL, NULL, NULL, NULL);
            Rval = ERR_NA;
        }

        Err = AmbaKAL_MsgQueueFlush(&(OnlinePbkCtr.VMsgQueue));
        if (KAL_ERR_NONE != Err) {
            AmbaPrint_PrintUInt5("Fail to flush OnlinePbkMsgV(%u)", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        Err = AmbaKAL_MsgQueueFlush(&(OnlinePbkCtr.AMsgQueue));
        if (KAL_ERR_NONE != Err) {
            AmbaPrint_PrintUInt5("Fail to flush OnlinePbkMsgA(%u)", Err, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }

        if (OnlinePbkCtr.DMux.VideoTrackExist)
        {
            VideoTskCtrl.Priority   = ONLINE_PBK_TASK_0_PRI;
            VideoTskCtrl.EntryFunc  = VideoPbkTaskEntry;
            VideoTskCtrl.pStackBase = VideoTskStack;
            VideoTskCtrl.StackSize  = ONLINE_PBK_STACK_SIZE;
            VideoTskCtrl.CpuBits    = ONLINE_PBK_TASK_0_CPU_BITS;
            VideoTskCtrl.EntryArg   = 0x0U;

            Rval = SvcTask_Create("OnlineVPbkTask", &VideoTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("OnlineVPbkTask create failed(%u)", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OnlinePbkCtr.DMux.AudioTrackExist)
        {
            AudioTskCtrl.Priority   = ONLINE_PBK_TASK_0_PRI;
            AudioTskCtrl.EntryFunc  = AudioPbkTaskEntry;
            AudioTskCtrl.pStackBase = AudioTskStack;
            AudioTskCtrl.StackSize  = ONLINE_PBK_STACK_SIZE;
            AudioTskCtrl.CpuBits    = ONLINE_PBK_TASK_0_CPU_BITS;
            AudioTskCtrl.EntryArg   = 0x0U;

            Rval = SvcTask_Create("OnlineAPbkTask", &AudioTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("OnlineAPbkTask create failed(%u)", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OnlinePbkCtr.DMux.VideoTrackExist)
        {
            VideoHandlerTskCtrl.Priority   = ONLINE_PBK_TASK_0_PRI;
            VideoHandlerTskCtrl.EntryFunc  = VideoPbkHandler;
            VideoHandlerTskCtrl.pStackBase = VideoHandlerTskStack;
            VideoHandlerTskCtrl.StackSize  = ONLINE_PBK_STACK_SIZE;
            VideoHandlerTskCtrl.CpuBits    = ONLINE_PBK_TASK_0_CPU_BITS;
            VideoHandlerTskCtrl.EntryArg   = 0x0U;

            Rval = SvcTask_Create("OnlineVPbkHandler", &VideoHandlerTskCtrl);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("OnlineVPbkHandler create failed(%u)", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_NA;
            }
        }

        if (OnlinePbkCtr.DMux.AudioTrackExist)
        {
           AudioHandlerTskCtrl.Priority   = ONLINE_PBK_TASK_0_PRI;
           AudioHandlerTskCtrl.EntryFunc  = AudioPbkHandler;
           AudioHandlerTskCtrl.pStackBase = AudioHandlerTskStack;
           AudioHandlerTskCtrl.StackSize  = ONLINE_PBK_STACK_SIZE;
           AudioHandlerTskCtrl.CpuBits    = ONLINE_PBK_TASK_0_CPU_BITS;
           AudioHandlerTskCtrl.EntryArg   = 0x0U;

           Rval = SvcTask_Create("OnlineAPbkHandler", &AudioHandlerTskCtrl);
           if (Rval != OK) {
               AmbaPrint_PrintUInt5("OnlineAPbkHandler create failed(%u)", Rval, 0U, 0U, 0U, 0U);
               Rval = ERR_NA;
           }
        }

        if (KAL_ERR_NONE != AmbaKAL_EventFlagSet(&(OnlinePbkCtr.PbkCtrFlg), \
                            ONLINE_PBK_VFLG_START_FEED | ONLINE_PBK_AFLG_START_FEED)) {
          Rval = ERR_NA;
          AmbaPrint_PrintStr5("%s, Give Event flag ONLINE_PBK_VFLG_START_FEED failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Rval;
}

void SvcOnlinePbk_SetBufInfo(SVC_ONLINE_PBK_BUF_s *pOnlinePbkBuf)
{
    UINT8 *pBuf = NULL;

    if (AmbaWrap_memset(&OnlinePbkCtr, 0, sizeof(SVC_ONLINE_PBK_Ctr_s))!= 0U) { }
    if (AmbaWrap_memset(&OnlinePbkBuf, 0, sizeof(SVC_ONLINE_PBK_BUF_s))!= 0U) { }
    if (AmbaWrap_memcpy(&OnlinePbkBuf, pOnlinePbkBuf, sizeof(SVC_ONLINE_PBK_BUF_s))!= 0U) { }

    AmbaMisra_TypeCast(&pBuf, &(OnlinePbkBuf.DemuxBuffAddr));
    if (AmbaWrap_memset(pBuf, 0, OnlinePbkBuf.DemuxBuffSize)!= 0U) { }

    AmbaMisra_TypeCast(&pBuf, &(OnlinePbkBuf.VBuffAddr));
    if (AmbaWrap_memset(pBuf, 0, OnlinePbkBuf.VBuffSize)!= 0U) { }

    AmbaMisra_TypeCast(&pBuf, &(OnlinePbkBuf.ABuffAddr));
    if (AmbaWrap_memset(pBuf, 0, OnlinePbkBuf.ABuffSize)!= 0U) { }

    AmbaPrint_PrintUInt5("SetBufInfo, VBuffAddr 0x%x, VBuffSize %d, ABuffAddr 0x%x, ABuffSize %d", \
                        OnlinePbkBuf.VBuffAddr, OnlinePbkBuf.VBuffSize, OnlinePbkBuf.ABuffAddr, OnlinePbkBuf.ABuffSize, 0U);
}

SVC_ONLINE_PBK_BUF_s* SvcOnlinePbk_GetBufInfo(void)
{
    return &OnlinePbkBuf;
}

void SvcOnlinePbk_QueryVBufSize(UINT32 *BufferSize)
{
    *BufferSize = 0x00900000; // CONFIG_SVC_DSP_DEC_BS_SIZE
}

void SvcOnlinePbk_QueryABufSize(UINT32 *BufferSize)
{
    *BufferSize = ADEC_FRAME_SIZE * ADEC_CH_NUM * 4U * 16U;
}

UINT32 SvcOnlinePbk_Op(UINT32 OpCmd, UINT32 Param)
{
    UINT32  Rval = OK, SeekTime = 0U;

    (VOID)Param;
    if (0U == OnlinePbkCtr.InitFlg) {
        AmbaPrint_PrintStr5("%s, OnlinePbkCtr isn't initialized", __func__, NULL, NULL, NULL, NULL);
        Rval = ERR_NA;
    } else {
        switch(OpCmd){
        case ONLINE_PBK_OP_PLAY:
            if (0U == Param) {
                if (KAL_ERR_NONE != AmbaKAL_EventFlagSet(&(OnlinePbkCtr.PbkCtrFlg), \
                                    ONLINE_PBK_VFLG_START_FEED | ONLINE_PBK_AFLG_START_FEED)) {
                  Rval = ERR_NA;
                  AmbaPrint_PrintStr5("%s, Give Event flag ONLINE_PBK_VFLG_START_FEED failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                SeekTime = ((UINT64)(Param))*TIME_SCALE_90K/1000;
                (VOID)PbkSeekReset();
                OnlinePbkCtr.AvTskCtrl.LatestPts = SeekTime;

                /* To find feeding point, search previous near IDR picture first */
                {
                    if (OK != PbkSearchFeedPoint(OnlinePbkCtr.AvTskCtrl.LatestPts, &(OnlinePbkCtr.AvTskCtrl.VFeedIdx))) {
                        AmbaPrint_PrintStr5("%s, Search feed point failed!!", __func__, NULL, NULL, NULL, NULL);
                        OnlinePbkCtr.AvTskCtrl.LatestPts = 0ULL;
                    } else {
                        OnlinePbkCtr.AvTskCtrl.LatestDts = (UINT64)(OnlinePbkCtr.DMux.SampleDurationV)*(OnlinePbkCtr.AvTskCtrl.VFeedIdx);
                        SyncAudio(OnlinePbkCtr.AvTskCtrl.LatestPts);

                        (VOID)PbkSeekStart();
                    }
                }
            }
             break;
        case ONLINE_PBK_OP_STOP:
            AmbaPrint_PrintStr5("%s, ONLINE_PBK_OP_MSG_STOP", __func__, NULL, NULL, NULL, NULL);
            (VOID)SvcOnlinePbk_DeInit();
             break;
        case ONLINE_PBK_OP_PAUSE:
            if (KAL_ERR_NONE != AmbaKAL_EventFlagSet(&(OnlinePbkCtr.PbkCtrFlg), \
                                ONLINE_VIDEO_HANDLER_PAUSE | ONLINE_AUDIO_HANDLER_PAUSE)) {
              Rval = ERR_NA;
              AmbaPrint_PrintStr5("%s, Give Event flag ONLINE_VIDEO_HANDLER_PAUSE failed!", __func__, NULL, NULL, NULL, NULL);
            }
             break;
        case ONLINE_PBK_OP_RESUME:
            if (KAL_ERR_NONE != AmbaKAL_EventFlagSet(&(OnlinePbkCtr.PbkCtrFlg), \
                                ONLINE_VIDEO_HANDLER_RESUME | ONLINE_AUDIO_HANDLER_RESUME)) {
              Rval = ERR_NA;
              AmbaPrint_PrintStr5("%s, Give Event flag ONLINE_VIDEO_HANDLER_RESUME failed!", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        default:
             break;
        }
    }

    return Rval;
}

SVC_DMUX_MOV_INFO_s* SvcOnlinePbk_GetDmuxInfo(VOID)
{
    SVC_DMUX_MOV_INFO_s *pDmuxMovInfo = NULL;
    if (1U == OnlinePbkCtr.InitFlg) {
        pDmuxMovInfo = &(OnlinePbkCtr.DMux);
    }

    return pDmuxMovInfo;
}

UINT32 SvcOnlinePbk_GetSPSAndPPS(char *filename, char *resParam)
{
    int ReturnValue = OK;
    UINT32 SpsLen = 0;
    UINT32 PpsLen = 0;
    UINT32 VpsLen = 0;

    if ((!filename) || (!resParam)) {
        AmbaPrint_PrintStr5("%s, <GetSPSAndPPS> Invalid param", __func__, NULL, NULL, NULL, NULL);
        ReturnValue = ERR_NA;
    } else {
        if (1U == OnlinePbkCtr.InitFlg) {
            if (OnlinePbkCtr.DMux.VideoFormat == SVC_DMUX_VID_FORMAT_HEVC) {
                APPLIB_NETFIFO_VPS_SPS_PPS_s *DecCfgHVC = (APPLIB_NETFIFO_VPS_SPS_PPS_s *) resParam;
                AmbaPrint_PrintStr5("<GetSPSAndPPS> HEVC clip", NULL, NULL, NULL, NULL, NULL);
                VpsLen = (unsigned int) OnlinePbkCtr.DMux.Vps_Sps_Pps.VpsSize;
                SpsLen = (unsigned int) OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize;
                PpsLen = (unsigned int) OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize;
                if ((SpsLen > sizeof(DecCfgHVC->Sps)) ||
                    (PpsLen > sizeof(DecCfgHVC->Pps)) ||
                    (VpsLen > sizeof(DecCfgHVC->Vps))) {
                    AmbaPrint_PrintUInt5("<GetSPSAndPPS> buffer size is not enough. SpsLen(%d , %d) PpsLen (%d, %d)", \
                                        SpsLen, sizeof(DecCfgHVC->Sps),\
                                        PpsLen, sizeof(DecCfgHVC->Pps), 0U);
                    AmbaPrint_PrintUInt5("<GetSPSAndPPS> buffer size is not enough. VpsLen (%d, %d)", \
                                        VpsLen, sizeof(DecCfgHVC->Vps),\
                                        0U, 0U, 0U);
                    ReturnValue = ERR_NA;
                } else {
                    DecCfgHVC->VpsLen = VpsLen;
                    DecCfgHVC->SpsLen = SpsLen;
                    DecCfgHVC->PpsLen = PpsLen;
                    AmbaWrap_memcpy(DecCfgHVC->Vps, OnlinePbkCtr.DMux.Vps_Sps_Pps.Vps, VpsLen);
                    AmbaWrap_memcpy(DecCfgHVC->Sps, OnlinePbkCtr.DMux.Vps_Sps_Pps.Sps, SpsLen);
                    AmbaWrap_memcpy(DecCfgHVC->Pps, OnlinePbkCtr.DMux.Vps_Sps_Pps.Pps, PpsLen);
                    ReturnValue = OK;
                }
            } else if (OnlinePbkCtr.DMux.VideoFormat == SVC_DMUX_VID_FORMAT_H264) {
                APPLIB_NETFIFO_SPS_PPS_s *DecCfgAVC = (APPLIB_NETFIFO_SPS_PPS_s *) resParam;
                AmbaPrint_PrintUInt5("<GetSPSAndPPS> AVC clip", 0U, 0U, 0U, 0U, 0U);
                {
                    SpsLen = (unsigned int) OnlinePbkCtr.DMux.Vps_Sps_Pps.SpsSize;
                    PpsLen = (unsigned int) OnlinePbkCtr.DMux.Vps_Sps_Pps.PpsSize;
                    if ((SpsLen > sizeof(DecCfgAVC->Sps)) ||
                        (PpsLen > sizeof(DecCfgAVC->Pps))) {
                        AmbaPrint_PrintUInt5("<GetSPSAndPPS> buffer size is not enough. SpsLen(%d , %d) PpsLen (%d, %d)", \
                                            SpsLen, sizeof(DecCfgAVC->Sps),\
                                            PpsLen, sizeof(DecCfgAVC->Pps), 0U);
                        ReturnValue = ERR_NA;
                    } else {
                        DecCfgAVC->SpsLen = SpsLen;
                        DecCfgAVC->PpsLen = PpsLen;
                        AmbaWrap_memcpy(DecCfgAVC->Sps, OnlinePbkCtr.DMux.Vps_Sps_Pps.Sps, SpsLen);
                        AmbaWrap_memcpy(DecCfgAVC->Pps, OnlinePbkCtr.DMux.Vps_Sps_Pps.Pps, PpsLen);
                        ReturnValue = OK;
                    }
                }
            } else {
                AmbaPrint_PrintStr5("%s, Unknown video format!", __func__, NULL, NULL, NULL, NULL);
                ReturnValue = ERR_NA;
            }
        } else {
            AmbaPrint_PrintStr5("%s, <GetSPSAndPPS> OnlinePbkCtr.InitFlg isn't 1", __func__, NULL, NULL, NULL, NULL);
            ReturnValue = ERR_NA;
        }
    }

    return ReturnValue;
}

