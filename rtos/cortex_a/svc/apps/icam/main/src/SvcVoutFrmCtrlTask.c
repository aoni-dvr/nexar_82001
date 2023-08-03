/**
 *  @file SvcVoutFrmCtrlTask.c
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
 *  @details svc vout frame control task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VOUT.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcDisplay.h"
#include "AmbaDspInt.h"
#include "SvcTask.h"
#include "SvcResCfg.h"
#include "SvcVoutFrmCtrlTask.h"
#include "SvcGui.h"

#include "SvcCvFlow.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"

#define SVC_LOG_VFC_TASK  "VFC"

#define SVC_VFC_TASK_STACK_SIZE         (0x3000U)
#define SVC_LIV_FRM_QUEUE_SIZE          32U
#define SVC_VOUT_FRM_QUEUE_SIZE         16U

#define SVC_VOUT_EVT_FRM_INFO           (0U)
#define SVC_VOUT_EVT_CAP_SEQ            (1U)

/* SvcVFCState */
#define SVC_VFC_STATE_NONE              (0U)
#define SVC_VFC_STATE_IDLE              (1U)
#define SVC_VFC_STATE_START_OSD_SYNC    (2U)

#define SVC_VFC_GUI_LEVEL               (2U)

#define YUV_STREAM_BITS                 (0x80000000U)

typedef struct {
    UINT8                   VoutIdx;
    UINT8                   IsVoutInterlace;
    UINT8                   DlyCount;       /* Number of liveview count to be delayed */
    UINT8                   SyncMode;

    UINT8                   FirstRun;
    UINT32                  YuvStrmId;      /* YUV stream ID of DSP event */
    SVC_LIV_FRM_INFO_s      *pLivFrmQue;    /* Liveview frame queue */
    UINT8                   LivQueWp;       /* Write point of liveview frame queue */
    UINT8                   LivQueRp;       /* Read point of liveview frame queue */
    UINT8                   LivQueErr;
    AMBA_DSP_YUV_IMG_BUF_s  LivYuvInfo;     /* Liveview info */

    AMBA_KAL_MSG_QUEUE_t    VoutQueId;      /* Vout frame queue */
    UINT32                  DspWaitFlag;
    AMBA_DSP_VOUT_VIDEO_CFG_s *pVideo;
    UINT8                   PushLatency;    /* Latency(ms) from vout interrupt to push */
} SVC_VOUT_FRM_CTRL_s;

typedef struct {
    UINT32 Event;
    SVC_LIV_FRM_INFO_s      *pFrmInfo;
    UINT64                  SyncValue;
} SVC_VOUT_FRM_MSG_s;

static SVC_VOUT_FRM_CTRL_s g_VoutFrmCtrl GNU_SECTION_NOZEROINIT;
static UINT8  SvcVFCState = 0U;
static UINT32 SvcVFCDebugEnable = 0U;

static UINT32 VoutFrmCtrl_LivDataRdyHandler(const void *pEventInfo);
static void*  VoutFrmCtrl_TaskEntry(void* EntryArg);
static void   VoutFrmCtrl_Entry(void);

static void   VoutFrmCtrl_IncrementIndex(UINT8 *pIdx, UINT8 Entries);
static void   VoutFrmCtrl_GetPrevIndex(UINT8 CurIdx, UINT8 Entries, UINT8 *pPrevIdx);
static void   VoutFrmCtrl_FlushVoutQueue(UINT32 Remain);
static UINT32 VoutFrmCtrlTask_SearchCapSeg(UINT64 CapSeq, UINT8 *pRp);

static void   VoutFrmCtrlTask_GUISyncInit(UINT32 VoutIdx, UINT32 SyncMode);
static void   VoutFrmCtrlTask_GUIUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
static void   VoutFrmCtrlTask_GUIDraw(UINT32 VoutIdx, UINT32 Level);
static void   VoutFrmCtrlTask_GUIEvent(UINT32 VoutIdx, UINT32 Level, UINT32 EventID, void *pEvent);

static void SVC_VFC_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcVFCDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void SVC_VFC_DBG_UL(const char *pFormat, ULONG Arg1, ULONG Arg2)
{
    if (SvcVFCDebugEnable > 0U) {
        SvcWrap_PrintUL(pFormat, Arg1, Arg2, 0U, 0U, 0U);
    }
}

void SvcVoutFrmCtrlTask_DebugEnable(UINT32 DebugEnable)
{
    SvcVFCDebugEnable = (UINT8)DebugEnable;
    SvcLog_OK(SVC_LOG_VFC_TASK, "SvcVFCDebugEnable = %d", SvcVFCDebugEnable, 0U);
}

/**
 *  Init Vout Frame Control.
 *  @return error code
 */
UINT32 SvcVoutFrmCtrlTask_Init(void)
{
    AmbaSvcWrap_MisraMemset(&g_VoutFrmCtrl, 0, sizeof(SVC_VOUT_FRM_CTRL_s));

    return SVC_OK;
}

/**
 *  Configure Vout Frame Control.
 *  @param[in] pCfg Vout frame rate control configuration
 *  @return error code
 */
UINT32 SvcVoutFrmCtrlTask_Config(const SVC_VOUT_FRM_CTRL_CFG_s *pCfg)
{
    UINT32               RetVal = SVC_OK;
    const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();
    UINT32               YuvStrmIdx;
    UINT32               FrameTime;

    if (NULL != pCfg) {
        YuvStrmIdx = pCfg->YuvStrmIdx;
        if (YuvStrmIdx < pResCfg->DispNum) {
            g_VoutFrmCtrl.VoutIdx         = (UINT8)pResCfg->DispStrm[YuvStrmIdx].VoutID;
            g_VoutFrmCtrl.IsVoutInterlace = pResCfg->DispStrm[YuvStrmIdx].FrameRate.Interlace;
            g_VoutFrmCtrl.YuvStrmId       = (YUV_STREAM_BITS | YuvStrmIdx);
            g_VoutFrmCtrl.DlyCount        = pCfg->DlyCount;
            g_VoutFrmCtrl.SyncMode        = pCfg->SyncMode;

            if (g_VoutFrmCtrl.VoutIdx == VOUT_IDX_B) {
#if defined(SVC_DSP_VOUT1_INT)
                g_VoutFrmCtrl.DspWaitFlag = SVC_DSP_VOUT1_INT;
            } else {
#endif
                g_VoutFrmCtrl.DspWaitFlag = SVC_DSP_VOUT0_INT;
            }

            FrameTime = (pResCfg->DispStrm[YuvStrmIdx].FrameRate.NumUnitsInTick * 1000U) / pResCfg->DispStrm[YuvStrmIdx].FrameRate.TimeScale;
            g_VoutFrmCtrl.PushLatency = (UINT8)(FrameTime / 3U);

            AmbaPrint_PrintUInt5("[VFC] SvcVoutFrmCtrlTask_Config VOUT%d, SyncMode= %d DlyCount=%d, PushLatency=%d",
                g_VoutFrmCtrl.VoutIdx, g_VoutFrmCtrl.SyncMode, g_VoutFrmCtrl.DlyCount, g_VoutFrmCtrl.PushLatency, 0U);
        } else {
            SvcLog_NG(SVC_LOG_VFC_TASK, "Invalid YuvStrmIdx=%d", YuvStrmIdx, 0U);
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Start Vout Frame Control Task.
 *  @return error code
 */
UINT32 SvcVoutFrmCtrlTask_Start(void)
{
    UINT32 RetVal;
    UINT32 EnableFlag = 0U;
    SVC_VOUT_FRM_CTRL_s *pCtrl = &g_VoutFrmCtrl;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    static SVC_TASK_CTRL_s TaskCtrl;
    static char MsgQueName[] = "VoutFrmQue";
    static UINT8 FrmCtrlTaskStack[SVC_VFC_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_LIV_FRM_INFO_s LivFrmQueue[SVC_LIV_FRM_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_VOUT_FRM_MSG_s VoutFrmQueue[SVC_VOUT_FRM_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;

    pCtrl->pLivFrmQue = LivFrmQueue;

    RetVal = AmbaKAL_MsgQueueCreate(&pCtrl->VoutQueId,
                                    MsgQueName,
                                    (UINT32)sizeof(SVC_VOUT_FRM_MSG_s),
                                    VoutFrmQueue,
                                    SVC_VOUT_FRM_QUEUE_SIZE * (UINT32)sizeof(SVC_VOUT_FRM_MSG_s));

    if (KAL_ERR_NONE == RetVal) {
        if (pCtrl->SyncMode == SVC_VFC_TASK_CONT_DELAY) {
            /* The sync mode uses vout signal to update VOUT display */
            AmbaSvcWrap_MisraMemset(&TaskCtrl, 0, sizeof(TaskCtrl));
            TaskCtrl.Priority   = 40;
            TaskCtrl.EntryFunc  = VoutFrmCtrl_TaskEntry;
            TaskCtrl.EntryArg   = 0U;
            TaskCtrl.pStackBase = &FrmCtrlTaskStack[0];
            TaskCtrl.StackSize  = SVC_VFC_TASK_STACK_SIZE;
            TaskCtrl.CpuBits    = 0x01U;
            RetVal = SvcTask_Create("SvcVoutFrmCtrlTask", &TaskCtrl);
            if (SVC_OK == RetVal) {
                EnableFlag = 1U;
            } else {
                SvcLog_NG(SVC_LOG_VFC_TASK, "fail to SvcTask_Create", 0U, 0U);
            }
        } else {
            /* Other sync modes require sync info from CV */
            for (UINT32 i = 0U; i < CvFlowNum; i++) {
                if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                    if (pCvFlow[i].InputCfg.EnableFeeder != 0U) {
                        VoutFrmCtrlTask_GUISyncInit(g_VoutFrmCtrl.VoutIdx, (UINT32)g_VoutFrmCtrl.SyncMode);
                        EnableFlag = 1U;
                        break;
                    }
                }
            }
        }

        if (EnableFlag == 1U) {
            RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, VoutFrmCtrl_LivDataRdyHandler);
            if (SVC_OK == RetVal) {
                SvcVFCState = SVC_VFC_STATE_IDLE;
            } else {
                SvcLog_NG(SVC_LOG_VFC_TASK, "fail to register LV_YUV_DATA_RDY event", 0U, 0U);
            }
        } else {
            SvcLog_DBG(SVC_LOG_VFC_TASK, "SvcVoutFrmCtrlTask is not enabled", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_VFC_TASK, "fail to create VoutFrmQue", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Control interface of Vout Frame Control Task.
 *  @param[in] pCmd string of command
 *  @param[in] pInfo information of the command
 *  @return error code
 */
UINT32 SvcVoutFrmCtrlTask_Ctrl(const char *pCmd, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    SVC_VOUT_FRM_CTRL_s *pCtrl = &g_VoutFrmCtrl;
    SVC_VOUT_FRM_MSG_s Msg = {0U};
    const UINT64 *pSyncValue;

    AmbaMisra_TouchUnused(pInfo);

    if (SvcVFCState != SVC_VFC_STATE_NONE) {
        if (0 == SvcWrap_strcmp(SVC_VFC_TASK_CMD_CAP_SEQ, pCmd)) {
            if ((pCtrl->SyncMode == SVC_VFC_TASK_SYNC_CAP_SEQ_AND_OSD) ||
                (pCtrl->SyncMode == SVC_VFC_TASK_SYNC_CAP_SEQ_AND_GUI_TASK)) {
                AmbaMisra_TypeCast(&pSyncValue, &pInfo);

                Msg.Event     = SVC_VOUT_EVT_CAP_SEQ;
                Msg.pFrmInfo  = NULL;
                Msg.SyncValue = *pSyncValue;
                RetVal = AmbaKAL_MsgQueueSend(&pCtrl->VoutQueId, &Msg, AMBA_KAL_NO_WAIT);
                if (RetVal == KAL_ERR_NONE) {
                    SvcVFCState = SVC_VFC_STATE_START_OSD_SYNC;
                    SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Put CapSeq(%u) to VoutQ", (UINT32)Msg.SyncValue, 0U);
                } else {
                    SvcLog_NG(SVC_LOG_VFC_TASK, "Send VoutQue error(0x%x)", RetVal, 0U);
                }
            }
        } else if (0 == SvcWrap_strcmp(SVC_VFC_TASK_CMD_VOUT_UPDATE, pCmd)) {
            VoutFrmCtrl_Entry();
        } else {
            /* Do nothing */
        }
    } else {
        SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Not Started", 0U, 0U);
    }

    return RetVal;
}

static UINT32 VoutFrmCtrl_LivDataRdyHandler(const void *pEventInfo)
{
    UINT32 RetVal = SVC_OK;
    SVC_VOUT_FRM_CTRL_s *pCtrl = &g_VoutFrmCtrl;
    const AMBA_DSP_YUV_DATA_RDY_s *pYuvRdy;
    SVC_VOUT_FRM_MSG_s Msg = {0U};
    SVC_LIV_FRM_INFO_s *pLivFrmInfo;

    AmbaMisra_TypeCast(&pYuvRdy, &pEventInfo);

    if ((pYuvRdy->ViewZoneId == pCtrl->YuvStrmId) && (pCtrl->LivQueErr < 10U)) {
        if (NULL != pCtrl->pLivFrmQue) {
            if (pCtrl->FirstRun == 0U) {
                AmbaSvcWrap_MisraMemcpy(&pCtrl->LivYuvInfo, &pYuvRdy->Buffer, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                pCtrl->FirstRun = 1;
            }

            /* Write to LivFrmQue */
            pLivFrmInfo = &pCtrl->pLivFrmQue[pCtrl->LivQueWp];
            pLivFrmInfo->CapPts     = pYuvRdy->CapPts;
            pLivFrmInfo->BaseAddrY  = pYuvRdy->Buffer.BaseAddrY;
            pLivFrmInfo->BaseAddrUV = pYuvRdy->Buffer.BaseAddrUV;
            pLivFrmInfo->CapSequence= pYuvRdy->CapSequence;

            SVC_VFC_DBG_UL("[VFC] Write to LivQ[%d], Y:0x%x", (ULONG)pCtrl->LivQueWp, pLivFrmInfo->BaseAddrY);
            SVC_VFC_DBG_UL("[VFC] Write to LivQ[%d], CapSeg:%u", (ULONG)pCtrl->LivQueWp, (ULONG)pLivFrmInfo->CapSequence);

            VoutFrmCtrl_IncrementIndex(&pCtrl->LivQueWp, SVC_LIV_FRM_QUEUE_SIZE);

            if (pCtrl->SyncMode == SVC_VFC_TASK_CONT_DELAY) {
                /* Retrieve from LivFrmQue and push to VoutQue with delay */
                if (pCtrl->DlyCount == 0U) {
                    Msg.Event     = SVC_VOUT_EVT_FRM_INFO;
                    Msg.pFrmInfo  = &pCtrl->pLivFrmQue[pCtrl->LivQueRp];
                    Msg.SyncValue = 0U;
                    RetVal = AmbaKAL_MsgQueueSend(&pCtrl->VoutQueId, &Msg, AMBA_KAL_NO_WAIT);
                    SVC_VFC_DBG_UL("[VFC] Push LivQ[%d] to VoutQ, Y:0x%x", (ULONG)pCtrl->LivQueRp, pCtrl->pLivFrmQue[pCtrl->LivQueRp].BaseAddrY);

                    if (RetVal == KAL_ERR_NONE) {
                        VoutFrmCtrl_IncrementIndex(&pCtrl->LivQueRp, SVC_LIV_FRM_QUEUE_SIZE);
                    } else {
                        SvcLog_NG(SVC_LOG_VFC_TASK, "Send VoutQue error(0x%x)", RetVal, 0U);
                        pCtrl->LivQueErr++;
                    }
                } else {
                    pCtrl->DlyCount--;
                }
            }

        } else {
            SvcLog_NG(SVC_LOG_VFC_TASK, "pLivFrmQue = NULL", 0U, 0U);
        }
    }

    return SVC_OK;
}

static void* VoutFrmCtrl_TaskEntry(void* EntryArg)
{
    UINT32 RetVal, ActualFlag;
    const SVC_VOUT_FRM_CTRL_s *pCtrl = &g_VoutFrmCtrl;
    static AMBA_DSP_INT_PORT_s IntPort;

    AmbaMisra_TouchUnused(EntryArg);
    SvcLog_DBG(SVC_LOG_VFC_TASK, "VoutFrmCtrl_TaskEntry start", 0U, 0U);

    /* open dsp int port */
    RetVal = AmbaDspInt_Open(&IntPort);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_VFC_TASK, "fail to open dsp int port", 0U, 0U);
    }

    while (SVC_OK == RetVal) {
        RetVal = AmbaDspInt_Take(&IntPort, pCtrl->DspWaitFlag, &ActualFlag, 0xFFFFU);

        if (RetVal == SVC_OK) {
            VoutFrmCtrl_Entry();
        } else {
            SvcLog_NG(SVC_LOG_VFC_TASK, "AmbaDspInt_Take failed(%u)", RetVal, 0U);
        }
    }

    SvcLog_DBG(SVC_LOG_VFC_TASK, "VoutFrmCtrlTaskEntry[%d] exit", 0U, 0U);

    return NULL;
}

static void VoutFrmCtrl_Entry(void)
{
    UINT32 RetVal;
    SVC_VOUT_FRM_CTRL_s *pCtrl = &g_VoutFrmCtrl;
    SVC_DISP_INFO_s DispInfo;
    UINT32 i, NumDisp;
    SVC_VOUT_FRM_MSG_s Msg = {0U};
    AMBA_KAL_MSG_QUEUE_INFO_s QueueInfo;
    const SVC_LIV_FRM_INFO_s *pLivFrmInfo;
    UINT64  RawSeq = 0ULL;
    UINT64  TargetCapSeq;
    UINT8 Rp = 0U;

    SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Recv vout sig", 0U, 0U);

    if (NULL == pCtrl->pVideo) {
        /* Check if the DispCfg exists */
        SvcDisplay_InfoGet(&DispInfo);
        NumDisp = *(DispInfo.pNumDisp);
        for (i = 0; i < NumDisp; i++) {
            if (DispInfo.pDispCfg[i].VoutID == pCtrl->VoutIdx) {
                pCtrl->pVideo = &(DispInfo.pDispCfg[i].VideoCfg);
                SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Found DispCfg[%d]", i, 0U);
                break;
            }
        }
    }

    if (NULL != pCtrl->pVideo) {
        RetVal = AmbaKAL_MsgQueueReceive(&pCtrl->VoutQueId, &Msg, AMBA_KAL_NO_WAIT);
        if (KAL_ERR_NONE == RetVal) {
            SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Recv vout event(0x%x)", Msg.Event, 0U);

            pLivFrmInfo = NULL;
            if (Msg.Event == SVC_VOUT_EVT_CAP_SEQ) {
                /* Peek the latest */
                if (KAL_ERR_NONE == AmbaKAL_MsgQueueQuery(&pCtrl->VoutQueId, &QueueInfo)) {
                    SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Enqueued(%u)", QueueInfo.NumEnqueued, 0U);
                    if (QueueInfo.NumEnqueued > 0U) {
                        for (i = 0U; i < QueueInfo.NumEnqueued; i++) {
                            if (KAL_ERR_NONE != AmbaKAL_MsgQueueReceive(&pCtrl->VoutQueId, &Msg, AMBA_KAL_NO_WAIT)) {
                                SvcLog_NG(SVC_LOG_VFC_TASK, "MsgQueueReceive failed", 0U, 0U);
                            }
                        }
                    }
                } else {
                    SvcLog_NG(SVC_LOG_VFC_TASK, "MsgQueueQuery failed", 0U, 0U);
                }

                TargetCapSeq = Msg.SyncValue - (UINT64)pCtrl->DlyCount;
                //SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Search CapSeg:%u  Sync:%u", (UINT32)TargetCapSeq, (UINT32)Msg.SyncValue);

                if (SVC_OK == VoutFrmCtrlTask_SearchCapSeg(TargetCapSeq, &Rp)) {
                    pLivFrmInfo = &pCtrl->pLivFrmQue[Rp];
                    SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Take LivQ[%d], CapSeg:%u", Rp, (UINT32)pLivFrmInfo->CapSequence);
                } else {
                    SvcLog_DBG(SVC_LOG_VFC_TASK, "Cannot find CapSeq(%u)", (UINT32)TargetCapSeq, 0U);
                }
            } else if (Msg.Event == SVC_VOUT_EVT_FRM_INFO) {
                pLivFrmInfo = Msg.pFrmInfo;

                VoutFrmCtrl_FlushVoutQueue(0U);
            } else {
                SvcLog_NG(SVC_LOG_VFC_TASK, "Invalid Event (%u)", Msg.Event, 0U);
            }

            if (pLivFrmInfo != NULL) {
                //RetVal = AmbaKAL_TaskSleep(g_VoutFrmCtrl.PushLatency);
                //if (RetVal != SVC_OK) {
                //    SvcLog_NG(SVC_LOG_VFC_TASK, "AmbaKAL_TaskSleep failed %u", RetVal, 0U);
                //}

                pCtrl->pVideo->VideoSource                  = VOUT_SOURCE_DEFAULT_IMAGE;
                pCtrl->pVideo->DefaultImgConfig.Pitch       = pCtrl->LivYuvInfo.Pitch;
                pCtrl->pVideo->DefaultImgConfig.DataFormat  = pCtrl->LivYuvInfo.DataFmt;
                pCtrl->pVideo->DefaultImgConfig.BaseAddrY   = pLivFrmInfo->BaseAddrY;
                pCtrl->pVideo->DefaultImgConfig.BaseAddrUV  = pLivFrmInfo->BaseAddrUV;
                pCtrl->pVideo->DefaultImgConfig.FieldRepeat = 0;

                RetVal |= AmbaDSP_VoutVideoConfig(pCtrl->VoutIdx, pCtrl->pVideo);
                RetVal |= AmbaDSP_VoutVideoCtrl(pCtrl->VoutIdx, 1, 0, &RawSeq);
                SVC_VFC_DBG_UL("[VFC] Set Vout: Y:0x%x UV:0x%x", pLivFrmInfo->BaseAddrY, pLivFrmInfo->BaseAddrUV);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_VFC_TASK, "Vout ctrl failed", 0U, 0U);
                }
            }
        } else if (KAL_ERR_TIMEOUT == RetVal) {
            /* TX_QUEUE_EMPTY: Do nothing. */
            SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Queue empty", 0U, 0U);
        } else {
            SvcLog_NG(SVC_LOG_VFC_TASK, "MsgQueueReceive failed(%u)", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_VFC_TASK, "Can't find DispCfg", 0U, 0U);
    }
}

static void VoutFrmCtrl_IncrementIndex(UINT8 *pIdx, UINT8 Entries)
{
    UINT8 NewIdx = *pIdx;

    NewIdx++;
    if (NewIdx >= Entries) {
        NewIdx = 0;
    }
    *pIdx = NewIdx;
}

static void VoutFrmCtrl_GetPrevIndex(UINT8 CurIdx, UINT8 Entries, UINT8 *pPrevIdx)
{
    if (CurIdx == 0U) {
        *pPrevIdx = Entries - 1U;
    } else {
        *pPrevIdx = CurIdx - 1U;
    }
}

static void VoutFrmCtrl_FlushVoutQueue(UINT32 Remain)
{
    SVC_VOUT_FRM_CTRL_s *pCtrl = &g_VoutFrmCtrl;
    AMBA_KAL_MSG_QUEUE_INFO_s QueueInfo;
    SVC_VOUT_FRM_MSG_s TmpMsg;

    /* Flush but remain one msg if any */
    if (KAL_ERR_NONE == AmbaKAL_MsgQueueQuery(&pCtrl->VoutQueId, &QueueInfo)) {
        SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Enqueued(%u)", QueueInfo.NumEnqueued, 0U);
        if (QueueInfo.NumEnqueued > Remain) {
            for (UINT32 i = 0; i < (QueueInfo.NumEnqueued - Remain); i++) {
                if (KAL_ERR_NONE != AmbaKAL_MsgQueueReceive(&pCtrl->VoutQueId, &TmpMsg, AMBA_KAL_NO_WAIT)) {
                    SvcLog_NG(SVC_LOG_VFC_TASK, "MsgQueueReceive failed", 0U, 0U);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_VFC_TASK, "MsgQueueQuery failed", 0U, 0U);
    }
}


static UINT32 VoutFrmCtrlTask_SearchCapSeg(UINT64 CapSeq, UINT8 *pRp)
{
    UINT32 RetVal = SVC_OK;
    SVC_VOUT_FRM_CTRL_s *pCtrl = &g_VoutFrmCtrl;
    UINT8 ChkIdx;
    UINT32 Found = 0U;

    ChkIdx = pCtrl->LivQueWp;

    for (UINT32 i = 0U; i < SVC_LIV_FRM_QUEUE_SIZE; i++) {
        VoutFrmCtrl_GetPrevIndex(ChkIdx, SVC_LIV_FRM_QUEUE_SIZE, &ChkIdx);

        //SVC_VFC_DBG(SVC_LOG_VFC_TASK, "CapSeq[%u] = %u", ChkIdx, (UINT32)pCtrl->pLivFrmQue[ChkIdx].CapSequence);
        if (CapSeq == pCtrl->pLivFrmQue[ChkIdx].CapSequence) {
            *pRp = ChkIdx;
            pCtrl->LivQueRp = ChkIdx;
            VoutFrmCtrl_IncrementIndex(&pCtrl->LivQueRp, SVC_LIV_FRM_QUEUE_SIZE);
            Found = 1U;
            break;
        }
    }

    if (Found == 0U) {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void VoutFrmCtrlTask_GUISyncInit(UINT32 VoutIdx, UINT32 SyncMode)
{
    if ((SyncMode == SVC_VFC_TASK_SYNC_CAP_SEQ_AND_OSD) ||
        (SyncMode == SVC_VFC_TASK_SYNC_CAP_SEQ_AND_GUI_TASK)) {
         /* The sync mode uses OSD flush signal to update VOUT display */
        SvcGui_EventRegister(VoutIdx, SVC_VFC_GUI_LEVEL, VoutFrmCtrlTask_GUIEvent);
    }

    if (SyncMode == SVC_VFC_TASK_SYNC_CAP_SEQ_AND_GUI_TASK) {
        /* The sync mode uses GUI update signal to triggere image feeder */
        SvcGui_Register(VoutIdx, SVC_VFC_GUI_LEVEL, "VFC", VoutFrmCtrlTask_GUIDraw, VoutFrmCtrlTask_GUIUpdate);
    }
}

/* Feed data at GUI update event */
static void VoutFrmCtrlTask_GUIUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    #define VFC_TARGET_FEEDER       (0U)
    UINT32 RetVal;

    if (SvcVFCState != SVC_VFC_STATE_NONE) {
        if (VoutIdx == g_VoutFrmCtrl.VoutIdx) {
            if (g_VoutFrmCtrl.SyncMode == SVC_VFC_TASK_SYNC_CAP_SEQ_AND_GUI_TASK) {
                SVC_VFC_DBG(SVC_LOG_VFC_TASK, "GUI update: Send FSync", 0U, 0U);

                RetVal = SvcImgFeeder_Ctrl(VFC_TARGET_FEEDER, SVC_IMG_FEEDER_EXT_SYNC, NULL);
                if (KAL_ERR_NONE != RetVal) {
                    SvcLog_NG(SVC_LOG_VFC_TASK, "SvcImgFeeder_Ctrl failed(0x%x)", RetVal, 0U);
                }
            } else {
                /* Do nothing */
            }

            *pUpdate = 0U;
        }
    } else {
        SVC_VFC_DBG(SVC_LOG_VFC_TASK, "Not Started", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&Level);
}

static void VoutFrmCtrlTask_GUIDraw(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
}

/* Update VOUT when OSD is flushed */
static void VoutFrmCtrlTask_GUIEvent(UINT32 VoutIdx, UINT32 Level, UINT32 EventID, void *pEvent)
{
    if (SvcVFCState == SVC_VFC_STATE_START_OSD_SYNC) {
        if ((VoutIdx == g_VoutFrmCtrl.VoutIdx) && (EventID == SVC_GUI_EVENT_OSD_FLUSH)) {
            if ((g_VoutFrmCtrl.SyncMode == SVC_VFC_TASK_SYNC_CAP_SEQ_AND_GUI_TASK) ||
                (g_VoutFrmCtrl.SyncMode == SVC_VFC_TASK_SYNC_CAP_SEQ_AND_OSD)) {
                SVC_VFC_DBG(SVC_LOG_VFC_TASK, "GUI flush", 0U, 0U);
                if (SVC_OK != SvcVoutFrmCtrlTask_Ctrl(SVC_VFC_TASK_CMD_VOUT_UPDATE, NULL)) {
                    SvcLog_NG(SVC_LOG_VFC_TASK, "update vout failed", 0U, 0U);
                }
            }
        }
    } else {
        SVC_VFC_DBG(SVC_LOG_VFC_TASK, "No sync signal yet", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&Level);
    AmbaMisra_TouchUnused(pEvent);
}

