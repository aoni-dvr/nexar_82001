/**
 *  @file SvcLvFeedExtRawTask.c
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
 *  @details svc liveview feed external raw task
 *
 */

#include "AmbaTypes.h"
#include "AmbaShell.h"
#include "AmbaGDMA.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaDspInt.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcMem.h"
#include "SvcLiveview.h"
#include "SvcResCfg.h"
#include "SvcBuffer.h"

#include "SvcTaskList.h"
#include "SvcBufMap.h"

#include "SvcLvFeedExtYuvTask.h"

#define SVC_LV_FD_EX_DATA_TASK_STACK_SIZE   (0xA000U)
#define SVC_LV_FD_EX_DATA_TASK_BUF_NUM      CONFIG_ICAM_LV_FEED_EXT_YUV_BUF_NUM
#define SVC_LV_FD_EX_DATA_TASK_BUF_SIZE     (0x3FC000) /* 1920x1088x2 */
/* Internal Attribute */
typedef struct {
    UINT8                   TaskEnable;
    SVC_TASK_CTRL_s         FeedTask;
    UINT8                   FeedTaskStack[SVC_LV_FD_EX_DATA_TASK_STACK_SIZE];
    SVC_TASK_CTRL_s         CopyTask;
    UINT8                   CopyTaskStack[SVC_LV_FD_EX_DATA_TASK_STACK_SIZE];
    AMBA_KAL_MSG_QUEUE_t    DataMsg;
    AMBA_DSP_YUV_DATA_RDY_s DataMsgQue[16U];
    UINT32                  BufAddr[SVC_LV_FD_EX_DATA_TASK_BUF_NUM];
    UINT8                   BufWrIdx;
    UINT8                   BufRdIdx;
    UINT32                  ExtDataSrc;
    UINT32                  ExtDataDst;
} SVC_LV_TASK_FEED_EXT_YUV_CTRL_s;

static SVC_LV_TASK_FEED_EXT_YUV_CTRL_s LvFeedExtYuvTaskCtrl GNU_SECTION_NOZEROINIT;

static UINT32 LvFeedExtYuvTask_Ctrl(UINT32 Enable);
static void*  LvFeedExtYuvTask_Entry(void * EntryArg);
static void*  LvFeedExtYuvTask_CopyEntry(void * EntryArg);

#define SVC_LOG_LV_FD_EX_RAW_TASK "LV_FD_EX_RAW_TASK"


UINT32 SvcLvFeedExtYuvTask_Start(void)
{
    static UINT8 Init = 0U;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum;
    UINT32 RetVal = SVC_OK;

    if (Init == 0U) {
        if (SVC_OK != AmbaWrap_memset(&LvFeedExtYuvTaskCtrl, 0, sizeof(LvFeedExtYuvTaskCtrl))) {
            SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "## SvcLiveviewTask_Init() failed with AmbaWrap_memset LvFeedExtYuvTaskCtrl", 0U, 0U);
        }
        Init = 1U;
    }
    if (SVC_OK == SvcResCfg_GetFovIdxsFromMem(FovIdxs, &FovNum)) {
        if (FovNum > 0U) {
            if (SVC_OK == LvFeedExtYuvTask_Ctrl(1U /* Enable */)) {
                /* Do nothing here */
            } else {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl() enable failed", 0U, 0U);
                RetVal = SVC_NG;
            }
        }
    }

    return RetVal;
}

UINT32 SvcLvFeedExtYuvTask_Stop(void)
{
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum;
    UINT32 RetVal = SVC_NG;

    if (SVC_OK == SvcResCfg_GetFovIdxsFromMem(FovIdxs, &FovNum)) {
        if (FovNum > 0U) {
            if (SVC_OK == LvFeedExtYuvTask_Ctrl(0U /* Disable */)) {
                /* Do nothing here */
            } else {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl() disable failed", 0U, 0U);
                RetVal = SVC_NG;
            }
        }
    }

    return RetVal;
}
static UINT32 LivYuvDataRdy(const void *pEventData)
{
    const AMBA_DSP_YUV_DATA_RDY_s  *pYuvRdy = NULL;
    UINT32 FovIdx = 0U;
    UINT8 MatchStreamID = 0U;

    AmbaMisra_TypeCast32(&(pYuvRdy), &(pEventData));

    if (pYuvRdy != NULL) {
        /* If user is specified with stream, i.e., bit[31] is 1 */
        if ((LvFeedExtYuvTaskCtrl.ExtDataSrc & 0x80000000U) > 0U) {
            /* Only ViewZoneId representing stream ID and matching user's setting is needed */
            if ((pYuvRdy->ViewZoneId & LvFeedExtYuvTaskCtrl.ExtDataSrc) > 0U) {
                MatchStreamID = 1U;
            }
        /* Is user is specified target as FovIdx, i.e., bit[31] is 0 */
        } else {
            /* Only ViewZoneId represent FovIdx is needed */
            if ((pYuvRdy->ViewZoneId & 0x80000000U) == 0U) {
                /* Fetch FovIdx */
                FovIdx = pYuvRdy->ViewZoneId & ((UINT32) 0xFU);
                /* User can specify multiple FOV in bit-wise presentation */
                if ((LvFeedExtYuvTaskCtrl.ExtDataSrc & (((UINT32) 1U) << FovIdx)) > 0U) {
                    MatchStreamID = 1U;
                }
            }
        }

        if (MatchStreamID == 1U) {
            AMBA_DSP_YUV_DATA_RDY_s YuvEvent;
            YuvEvent.Buffer.DataFmt        = pYuvRdy->Buffer.DataFmt;
            YuvEvent.Buffer.BaseAddrY      = pYuvRdy->Buffer.BaseAddrY;
            YuvEvent.Buffer.BaseAddrUV     = pYuvRdy->Buffer.BaseAddrUV;
            YuvEvent.Buffer.Pitch          = pYuvRdy->Buffer.Pitch;
            YuvEvent.Buffer.Window.Width   = pYuvRdy->Buffer.Window.Width;
            YuvEvent.Buffer.Window.Height  = pYuvRdy->Buffer.Window.Height;
            YuvEvent.Buffer.Window.OffsetX = pYuvRdy->Buffer.Window.OffsetX;
            YuvEvent.Buffer.Window.OffsetY = pYuvRdy->Buffer.Window.OffsetY;
            YuvEvent.CapPts = pYuvRdy->CapPts;
            YuvEvent.CapSequence = pYuvRdy->CapSequence;
            YuvEvent.ViewZoneId = pYuvRdy->ViewZoneId;
            YuvEvent.YuvPts = pYuvRdy->YuvPts;

            if (KAL_ERR_NONE != AmbaKAL_MsgQueueSend(&LvFeedExtYuvTaskCtrl.DataMsg, &YuvEvent, AMBA_KAL_WAIT_FOREVER)) {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "AmbaKAL_MsgQueueSend failed!", 0U, 0U);
            }
        }
    }

    return OK;
}

static UINT32 LvFeedExtYuvTask_Ctrl(UINT32 Enable)
{
    ULONG  BufAddr;
    UINT32 BufSize;
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    static char MsgQueName[] = "CopyYuvDataQueue";

    SvcLog_DBG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl(%u) start", Enable, 0U);

    if (Enable > 0U) {
        if (LvFeedExtYuvTaskCtrl.TaskEnable == 0U) {
            /* Request the buffer for external data buffer */
            RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_LV_FEED_EXT_YUV, &BufAddr, &BufSize);
            if (SVC_OK == RetVal) {
                UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM], FovNum;
                for (UINT32 Idx = 0U; Idx < (UINT32) SVC_LV_FD_EX_DATA_TASK_BUF_NUM; Idx++) {
                    LvFeedExtYuvTaskCtrl.BufAddr[Idx] = BufAddr;
                    BufAddr = BufAddr + (UINT32) sizeof(AMBA_DSP_YUV_DATA_RDY_s) + (UINT32) SVC_LV_FD_EX_DATA_TASK_BUF_SIZE;
                }
                /* Identify which FovIdx needs be feeded and FovIdx/stream will put data to Extdata buf, only single FOV is supported now*/
                if (SVC_OK == SvcResCfg_GetFovIdxsFromMem(FovIdxs, &FovNum)) {
                    for (UINT32 Idx = 0U; Idx < FovNum ;Idx++) {
                        UINT32 FovIdx = FovIdxs[Idx];
                        LvFeedExtYuvTaskCtrl.ExtDataSrc = pCfg->FovCfg[FovIdx].PipeCfg.VirtVinSrc;
                        LvFeedExtYuvTaskCtrl.ExtDataDst = FovIdx;
                        break;
                    }
                }
            }
            RetVal = AmbaKAL_MsgQueueCreate(&LvFeedExtYuvTaskCtrl.DataMsg,
                                            MsgQueName,
                                            sizeof(AMBA_DSP_YUV_DATA_RDY_s),
                                            LvFeedExtYuvTaskCtrl.DataMsgQue,
                                            (16U * sizeof(AMBA_DSP_YUV_DATA_RDY_s)));
            if (SVC_OK == RetVal) {
                /* Create the external data feeding task */
                LvFeedExtYuvTaskCtrl.FeedTask.Priority   = SVC_LV_FD_EX_YUV_TASK_PRI;
                LvFeedExtYuvTaskCtrl.FeedTask.EntryFunc  = LvFeedExtYuvTask_Entry;
                LvFeedExtYuvTaskCtrl.FeedTask.pStackBase = LvFeedExtYuvTaskCtrl.FeedTaskStack;
                LvFeedExtYuvTaskCtrl.FeedTask.EntryArg   = 0U;
                LvFeedExtYuvTaskCtrl.FeedTask.StackSize  = SVC_LV_FD_EX_DATA_TASK_STACK_SIZE;
                LvFeedExtYuvTaskCtrl.FeedTask.CpuBits    = SVC_LV_FD_EX_YUV_TASK_CPU_BITS;

                RetVal = SvcTask_Create("SvcLiveviewFeedExtDataTask", &LvFeedExtYuvTaskCtrl.FeedTask);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl() err, feed task create failed with 0x%x", RetVal, 0U);
                }

                /* Create the external data copy task */
                LvFeedExtYuvTaskCtrl.CopyTask.Priority   = SVC_LV_FD_EX_YUV_CP_TASK_PRI;
                LvFeedExtYuvTaskCtrl.CopyTask.EntryFunc  = LvFeedExtYuvTask_CopyEntry;
                LvFeedExtYuvTaskCtrl.CopyTask.pStackBase = LvFeedExtYuvTaskCtrl.CopyTaskStack;
                LvFeedExtYuvTaskCtrl.CopyTask.EntryArg   = 0U;
                LvFeedExtYuvTaskCtrl.CopyTask.StackSize  = SVC_LV_FD_EX_DATA_TASK_STACK_SIZE;
                LvFeedExtYuvTaskCtrl.CopyTask.CpuBits    = SVC_LV_FD_EX_YUV_CP_TASK_CPU_BITS;

                RetVal = SvcTask_Create("SvcLiveviewCopyExtDataTask", &LvFeedExtYuvTaskCtrl.CopyTask);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl() err, copy task create failed with 0x%x", RetVal, 0U);
                }
            }

            if (SVC_OK != AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, LivYuvDataRdy)) {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "fail to register LV_YUV_RDY event", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl() err, task has been created", 0U, 0U);
            RetVal = SVC_NG;
        }

    } else {
        if (LvFeedExtYuvTaskCtrl.TaskEnable != 0U) {
            RetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, LivYuvDataRdy);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "fail to unregister LV_YUV_RDY event", 0U, 0U);
            }
            /* Destroy the external data feeding task */
            RetVal = SvcTask_Destroy(&LvFeedExtYuvTaskCtrl.FeedTask);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl() err, feed task destroy failed with 0x%x", RetVal, 0U);
            }
            RetVal = SvcTask_Destroy(&LvFeedExtYuvTaskCtrl.CopyTask);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl() err, copy task destroy failed with 0x%x", RetVal, 0U);
            }
            RetVal = AmbaKAL_MsgQueueDelete(&LvFeedExtYuvTaskCtrl.DataMsg);
        } else {
            SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl() err, task doesn't exist", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    SvcLog_DBG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Ctrl(%u) Done", Enable, 0U);

    return RetVal;
}

static void* LvFeedExtYuvTask_Entry(void* EntryArg)
{
    const ULONG *pArg;
    UINT32 Rval, WaitFlag = AMBA_DSP_VIN0_SOF, ActualFlags = 0U;
    UINT16 FovIdx = (UINT16) LvFeedExtYuvTaskCtrl.ExtDataDst;
    UINT8  RdIdx;
    UINT8  WrIdx;

    UINT32 BaseAddr, Size;
    AMBA_DSP_EXT_YUV_BUF_s ExtBuf = {0};

    const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    /* Get the input address to pointer pArg */
    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    WaitFlag = AMBA_DSP_VIN0_SOF | AMBA_DSP_VIN0_EOF;

    while (*pArg == 0U) {
        Rval = AmbaDSP_MainWaitVinInterrupt(WaitFlag, &ActualFlags, 10000U);

        if ((Rval == SVC_OK) && ((ActualFlags & AMBA_DSP_VIN0_SOF) > 0U)) {
            RdIdx = LvFeedExtYuvTaskCtrl.BufRdIdx;
            WrIdx = LvFeedExtYuvTaskCtrl.BufWrIdx;
            if ((RdIdx - WrIdx) == 0U) {
                SvcLog_DBG(SVC_LOG_LV_FD_EX_RAW_TASK, "No avaliable yuv. Skip to feed Yuv", 0U, 0U);
                continue;
            }
            BaseAddr = LvFeedExtYuvTaskCtrl.BufAddr[RdIdx];
            (void) AmbaMisra_TypeCast32(&pYuvInfo, &BaseAddr);
            BaseAddr += sizeof(AMBA_DSP_YUV_DATA_RDY_s);
            Size = (UINT32) pYuvInfo->Buffer.Pitch * (UINT32) pYuvInfo->Buffer.Window.Height;
            ExtBuf.ExtYuvBuf.DataFmt        = pYuvInfo->Buffer.DataFmt;
            ExtBuf.ExtYuvBuf.BaseAddrY      = BaseAddr;
            ExtBuf.ExtYuvBuf.BaseAddrUV     = BaseAddr + Size;
            ExtBuf.ExtYuvBuf.Pitch          = pYuvInfo->Buffer.Pitch;
            ExtBuf.ExtYuvBuf.Window.Width   = pCfg->FovCfg[FovIdx].RawWin.Width;
            ExtBuf.ExtYuvBuf.Window.Height  = pCfg->FovCfg[FovIdx].RawWin.Height;
            ExtBuf.ExtYuvBuf.Window.OffsetX = pCfg->FovCfg[FovIdx].RawWin.OffsetX;
            ExtBuf.ExtYuvBuf.Window.OffsetY = pCfg->FovCfg[FovIdx].RawWin.OffsetY;
            ExtBuf.pExtME0Buf               = NULL;
            ExtBuf.pExtME1Buf               = NULL;

            if (SVC_OK != AmbaDSP_LiveviewFeedYuvData(1, &FovIdx, &ExtBuf)) {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_Entry err. AmbaDSP_LiveviewFeedYuvData failed.", 0U, 0U);
            }
            LvFeedExtYuvTaskCtrl.BufRdIdx = (LvFeedExtYuvTaskCtrl.BufRdIdx + 1U) % (UINT8) SVC_LV_FD_EX_DATA_TASK_BUF_NUM;
        }
    }

    return NULL;
}

static void* LvFeedExtYuvTask_CopyEntry(void* EntryArg)
{
    const ULONG *pArg;
    UINT8 WrIdx = 0U;
    void *pTempBuf;
    void *pTempBufDst, *pTempBufSrc;
    UINT32 BaseAddr = 0U;
    UINT32 Size;
    void *pYuvEvent;

    AMBA_DSP_YUV_DATA_RDY_s YuvEvent;
    AMBA_GDMA_BLOCK_s GdmaCfg;

    /* Get the input address to pointer pArg */
    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    while (*pArg == 0U) {

        if (SVC_OK != AmbaKAL_MsgQueueReceive(&LvFeedExtYuvTaskCtrl.DataMsg, &YuvEvent, AMBA_KAL_WAIT_FOREVER)) {
            SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_CopyEntry err. AmbaKAL_MsgQueueReceive failed", 0U, 0U);
            continue;
        }

        WrIdx = LvFeedExtYuvTaskCtrl.BufWrIdx;
        BaseAddr = LvFeedExtYuvTaskCtrl.BufAddr[WrIdx];
        (void) AmbaMisra_TypeCast32(&pYuvEvent, &BaseAddr);

        if (SVC_OK == AmbaWrap_memcpy(pYuvEvent, &YuvEvent, sizeof(AMBA_DSP_YUV_DATA_RDY_s))) {
            BaseAddr += sizeof(AMBA_DSP_YUV_DATA_RDY_s);
            /* Y data */
            (void) AmbaMisra_TypeCast32(&pTempBuf, &BaseAddr); /* pTempBuf = (UINT8 *) BaseAddr;*/
            Size = (UINT32) YuvEvent.Buffer.Pitch * (UINT32) YuvEvent.Buffer.Window.Height;
            pTempBufDst = pTempBuf;
            (void) AmbaMisra_TypeCast32(&pTempBufSrc, &YuvEvent.Buffer.BaseAddrY); /* pTempBufSrc = (UINT8 *)YuvEvent.Buffer.BaseAddrY;*/

            GdmaCfg.pSrcImg = pTempBufSrc;
            GdmaCfg.pDstImg = pTempBufDst;
            GdmaCfg.SrcRowStride = YuvEvent.Buffer.Pitch;
            GdmaCfg.DstRowStride = YuvEvent.Buffer.Pitch;
            GdmaCfg.BltWidth  = YuvEvent.Buffer.Window.Width;
            GdmaCfg.BltHeight = YuvEvent.Buffer.Window.Height;
            GdmaCfg.PixelFormat = AMBA_GDMA_8_BIT;
            if (SVC_OK != AmbaGDMA_BlockCopy(&GdmaCfg, NULL, 0U, 500U)) {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_CopyEntry err. AmbaGDMA_BlockCopy failed", 0U, 0U);
            }

            /* UV data */
            BaseAddr = BaseAddr + Size;
            (void) AmbaMisra_TypeCast32(&pTempBuf, &BaseAddr); /* pTempBuf = (UINT8 *) BaseAddr;*/
            pTempBufDst = pTempBuf;
            (void) AmbaMisra_TypeCast32(&pTempBufSrc, &YuvEvent.Buffer.BaseAddrUV); /*pTempBufSrc = (UINT8 *)YuvEvent.Buffer.BaseAddrUV; */

            GdmaCfg.pSrcImg = pTempBufSrc;
            GdmaCfg.pDstImg = pTempBufDst;
            GdmaCfg.SrcRowStride = YuvEvent.Buffer.Pitch;
            GdmaCfg.DstRowStride = YuvEvent.Buffer.Pitch;
            GdmaCfg.BltWidth  = YuvEvent.Buffer.Window.Width;
            GdmaCfg.BltHeight = YuvEvent.Buffer.Window.Height;
            GdmaCfg.BltHeight = GdmaCfg.BltHeight >> (UINT32) 1U;
            GdmaCfg.PixelFormat = AMBA_GDMA_8_BIT;
            if (SVC_OK != AmbaGDMA_BlockCopy(&GdmaCfg, NULL, 0U, 500U)) {
                SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "LvFeedExtYuvTask_CopyEntry err. AmbaGDMA_BlockCopy failed", 0U, 0U);
            }

            LvFeedExtYuvTaskCtrl.BufWrIdx = (LvFeedExtYuvTaskCtrl.BufWrIdx + 1U) % (UINT8) SVC_LV_FD_EX_DATA_TASK_BUF_NUM;
        }
    }

    return NULL;
}

UINT32 SvcLvFeedExtYuvTask_ExtYuvMemSizeQuery(UINT32 *pWorkSize)
{
    UINT32 RetVal = SVC_OK;
    UINT32 Size = 0U;

    if (pWorkSize == NULL) {
        SvcLog_NG(SVC_LOG_LV_FD_EX_RAW_TASK, "SvcLiveviewTask_ExtDataMemSizeQuery err. Input pWorkSize should not be NULL", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (SVC_OK == RetVal) {
        Size = (sizeof(AMBA_DSP_YUV_DATA_RDY_s) + (UINT32) SVC_LV_FD_EX_DATA_TASK_BUF_SIZE) * (UINT32) SVC_LV_FD_EX_DATA_TASK_BUF_NUM;
        *pWorkSize = Size;
    }

    return RetVal;
}