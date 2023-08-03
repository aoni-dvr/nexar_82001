/**
 *  @file SvcTimeLapse.c
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
 *  @details svc time-lapse
 */

#include "AmbaWrap.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_StillCapture.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcDSP.h"
#include "SvcRecMain.h"
#include "SvcTask.h"
#include "SvcUtil.h"
#include "SvcResCfg.h"
#include "SvcTimeLapse.h"

#define SVC_LOG_TLAPSE              "TLASPE"
#define SVC_TLAPSE_CAP_BUF_NUM      (2U * (UINT32)CONFIG_ICAM_DSP_ENC_CORE_NUM)

#define SVC_TLAPSE_CAP_FLG_STREAM   (0xFU)         /* CONFIG_ICAM_MAX_REC_STRM */
#define SVC_TLAPSE_CAP_FLG_LAST     (0x10000U)     /* LAST FRAME */

#define SVC_TLAPSE_CAP_BUF_FREE     (0U)
#define SVC_TLAPSE_CAP_BUF_LOCKED   (1U)

typedef struct {
    UINT8                    Enable;
    UINT32                   CapCount;
    UINT32                   FeedCount;
    UINT32                   EncCount;
    UINT32                   BufStatus[SVC_TLAPSE_CAP_BUF_NUM];
    AMBA_DSP_DATACAP_CFG_s   CapCfg[SVC_TLAPSE_CAP_BUF_NUM];
} SVC_TLAPSE_STR_CTRL_s;

typedef struct {
    /* capture task */
    UINT8                    IsTaskCreate;
    SVC_TASK_CTRL_s          TaskCtrl;
    AMBA_KAL_EVENT_FLAG_t    Flag;
    AMBA_KAL_MUTEX_t         Mutex;
    AMBA_KAL_MSG_QUEUE_t     FeedDataQue;
    ULONG                    CapBufAddr;
    UINT32                   CapBufSize;
    UINT32                   CapInterval;

    UINT32                   CapInfoId[AMBA_DSP_MAX_DATACAP_NUM];
    SVC_TLAPSE_STR_CTRL_s    CapStrInfo[CONFIG_ICAM_MAX_REC_STRM];
    UINT32                   CurTime;
    UINT32                   LastTime;

} SVC_TLAPSE_CTRL_s;

static SVC_TLAPSE_CTRL_s      TimeLapseCtrl GNU_SECTION_NOZEROINIT;
static SVC_TLAPSE_TASK_CFG_s  TaskCfg GNU_SECTION_NOZEROINIT;


static void WaitBufferFree(UINT32 StreamId, UINT32 BufId)
{
    UINT32 Err, LoopU = 1U;

    while (LoopU == 1U) {
        Err = AmbaKAL_MutexTake(&(TimeLapseCtrl.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexTake failed (%u)", Err, 0U);
        }

        if (TimeLapseCtrl.CapStrInfo[StreamId].BufStatus[BufId] == SVC_TLAPSE_CAP_BUF_FREE) {
            LoopU = 0U;
        }

        Err = AmbaKAL_MutexGive(&(TimeLapseCtrl.Mutex));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexGive failed (%u)", Err, 0U);
        }

        if (LoopU == 1U) {
            Err = AmbaKAL_TaskSleep(50U);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_TLAPSE, "WaitBufferFree: AmbaKAL_TaskSleep failed (%u)", Err, 0U);
            }
        }
    }
}


static void* TimeLapseTask(void* EntryArg)
{
    UINT16                   Count, Idx, i, StreamId, CapInstance[CONFIG_ICAM_MAX_REC_STRM] = {0U};
    UINT32                   Err = SVC_OK, ActualFlags, BufId, CapBits, Bit = 1U;
    AMBA_DSP_DATACAP_CTRL_s  DataCapCtrl[CONFIG_ICAM_MAX_REC_STRM] = {0U};
    AMBA_DSP_EXT_YUV_BUF_s   ExtBuf;
    SVC_TLAPSE_STR_CTRL_s    *pStrInfo;
    SVC_TLAPSE_CTRL_s        *pCtrl = &TimeLapseCtrl;
    const SVC_RES_CFG_s      *pResCfg = SvcResCfg_Get();
    UINT16                   DispNum = (UINT16)pResCfg->DispNum;

    SvcLog_OK(SVC_LOG_TLAPSE, "SvcTimeLapseTask is created", 0U, 0U);
    AmbaMisra_TouchUnused(EntryArg);

    while (Err == SVC_OK) {
        ActualFlags = 0U;
        CapBits     = 0U;
        Count       = 0U;

        Err = AmbaKAL_EventFlagGet(&(pCtrl->Flag),
                                   SVC_TLAPSE_CAP_FLG_STREAM,
                                   AMBA_KAL_FLAGS_ANY,
                                   AMBA_KAL_FLAGS_CLEAR_NONE,
                                   &ActualFlags,
                                   AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_EventFlagGet SVC_TLAPSE_CAP_FLG_STREAM failed (%u)", Err, 0U);
            continue;
        }

        /* capture configuration */
        for (Idx = 0U; Idx < (UINT16)CONFIG_ICAM_MAX_REC_STRM; Idx++) {
            if ((pCtrl->CapStrInfo[Idx].Enable == 1U) && ((ActualFlags & (Bit << (UINT32)Idx)) > 0U)) {
                CapInstance[Count]        = Idx + DispNum;
                DataCapCtrl[Count].CapNum = 1U;
                BufId                     = pCtrl->CapStrInfo[Idx].CapCount % (UINT32)SVC_TLAPSE_CAP_BUF_NUM;

                if (CapInstance[Count] < AMBA_DSP_MAX_DATACAP_NUM) {
                    TimeLapseCtrl.CapInfoId[CapInstance[Count]] = Idx;
                }

                WaitBufferFree(Idx, BufId);

                Err = AmbaDSP_DataCapCfg(CapInstance[Count], &(pCtrl->CapStrInfo[Idx].CapCfg[BufId]));
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_TLAPSE, "AmbaDSP_DataCapCfg failed (%u/%u)", Idx, Err);
                } else {
                    Count++;
                    CapBits = SetBits(CapBits, Bit << (UINT32)Idx);

                    Err = AmbaKAL_MutexTake(&(pCtrl->Mutex), AMBA_KAL_WAIT_FOREVER);
                    if (Err != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexTake failed (%u)", Err, 0U);
                    }

                    pCtrl->CapStrInfo[Idx].BufStatus[BufId] = SVC_TLAPSE_CAP_BUF_LOCKED;

                    Err = AmbaKAL_MutexGive(&(pCtrl->Mutex));
                    if (Err != KAL_ERR_NONE) {
                        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexGive failed (%u)", Err, 0U);
                    }
                }
            }
        }

        /* wait to do capture */
        Err = AmbaKAL_GetSysTickCount(&(pCtrl->CurTime));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_GetSysTickCount failed (%u)", Err, 0U);
        }

        if (pCtrl->LastTime > 0U) {
            if ((pCtrl->CurTime - pCtrl->LastTime) > pCtrl->CapInterval) {
                SvcLog_DBG(SVC_LOG_TLAPSE, "Capture too slow (%u/%u)"
                                         , pCtrl->CurTime - pCtrl->LastTime
                                         , pCtrl->CapInterval);
            } else {
                Err = AmbaKAL_TaskSleep((pCtrl->LastTime + pCtrl->CapInterval) - pCtrl->CurTime);
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_TaskSleep failed (%u)", Err, 0U);
                }

                Err = AmbaKAL_GetSysTickCount(&(pCtrl->CurTime));
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_GetSysTickCount failed (%u)", Err, 0U);
                }
            }
        }

        /* capture control */
        if (Count > 0U) {
            Err = SvcDSP_DataCapCtrl(Count, CapInstance, DataCapCtrl, NULL);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_TLAPSE, "AmbaDSP_DataCapCtrl failed (%u)", Err, 0U);
            } else {
                for (i = 0U; i < Count; i++) {
                    if (CapInstance[i] < AMBA_DSP_MAX_DATACAP_NUM) {
                        if (pCtrl->CapInfoId[CapInstance[i]] < (UINT32)CONFIG_ICAM_MAX_REC_STRM) {
                            pCtrl->CapStrInfo[pCtrl->CapInfoId[CapInstance[i]]].CapCount++;
                        }
                    }
                }

                pCtrl->LastTime = pCtrl->CurTime;
            }
        }

        /* feed yuv data */
        while (CapBits != 0U) {
            Err = AmbaKAL_MsgQueueReceive(&(pCtrl->FeedDataQue), &StreamId, AMBA_KAL_WAIT_FOREVER);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MsgQueueReceive FeedDataQue failed (%u)", Err, 0U);
            }

            ActualFlags = 0U;
            Err = AmbaKAL_EventFlagGet(&(pCtrl->Flag), SVC_TLAPSE_CAP_FLG_STREAM, AMBA_KAL_FLAGS_ANY,
                                    AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_NO_WAIT);
            if (Err != KAL_ERR_NONE) {
                /* nothing */
            }

            AmbaSvcWrap_MisraMemset(&ExtBuf, 0, sizeof(AMBA_DSP_EXT_YUV_BUF_s));

            pStrInfo = &(pCtrl->CapStrInfo[StreamId]);
            BufId    = pStrInfo->FeedCount % (UINT32)SVC_TLAPSE_CAP_BUF_NUM;

            ExtBuf.ExtYuvBuf.DataFmt        = AMBA_DSP_YUV420;
            ExtBuf.ExtYuvBuf.BaseAddrY      = pStrInfo->CapCfg[BufId].DataBuf.BaseAddr;
            ExtBuf.ExtYuvBuf.Pitch          = pStrInfo->CapCfg[BufId].DataBuf.Pitch;
            ExtBuf.ExtYuvBuf.Window.Width   = pStrInfo->CapCfg[BufId].DataBuf.Window.Width;
            ExtBuf.ExtYuvBuf.Window.Height  = pStrInfo->CapCfg[BufId].DataBuf.Window.Height;
            ExtBuf.ExtYuvBuf.Window.OffsetX = pStrInfo->CapCfg[BufId].DataBuf.Window.OffsetX;
            ExtBuf.ExtYuvBuf.Window.OffsetY = pStrInfo->CapCfg[BufId].DataBuf.Window.OffsetY;
            ExtBuf.pExtME0Buf               = NULL;
            ExtBuf.pExtME1Buf               = &(pStrInfo->CapCfg[BufId].AuxDataBuf);

            if (((ActualFlags & (1UL << (UINT32)StreamId)) == 0U)) {
                /* if it's the last frame */
                Err = AmbaKAL_EventFlagSet(&(pCtrl->Flag), SVC_TLAPSE_CAP_FLG_LAST);
                if (Err != KAL_ERR_NONE) {
                    SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_EventFlagSet failed (%u/%u)", SVC_TLAPSE_CAP_FLG_LAST, Err);
                }
            } else {
                Err = AmbaDSP_VideoEncFeedYuvData(1U, &StreamId, &ExtBuf);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_TLAPSE, "AmbaDSP_VideoEncFeedYuvData failed (%u/%u)", StreamId, Err);
                }

                pStrInfo->FeedCount++;
            }

            CapBits = ClearBits(CapBits, Bit << StreamId);
        }
    }

    return NULL;
}

static UINT32 YuvCapDataReady(const void *pEventInfo)
{
    UINT32 Err, Rval = SVC_OK;
    const AMBA_DSP_STL_RAW_DATA_INFO_s *pInfo;

    AmbaMisra_TypeCast(&pInfo, &pEventInfo);

    if (pInfo->CapInstance < AMBA_DSP_MAX_DATACAP_NUM) {
        UINT16 CapInfoId = (UINT16)TimeLapseCtrl.CapInfoId[pInfo->CapInstance];
        if (CapInfoId < (UINT16)CONFIG_ICAM_MAX_REC_STRM) {
            Err = AmbaKAL_MsgQueueSend(&(TimeLapseCtrl.FeedDataQue), &CapInfoId, AMBA_KAL_NO_WAIT);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MsgQueueSend failed (%u)", Err, 0U);
            }
        }
    }

    return Rval;
}

/**
* Get Time-lapse config
* @param [in] ppCfg double pointer to config
*/
void SvcTimeLapse_TaskCfgGet(SVC_TLAPSE_TASK_CFG_s **ppCfg)
{
    *ppCfg = &TaskCfg;
}

/**
* Create time-lapse tasks
* @return ErrorCode
*/
UINT32 SvcTimeLapse_TaskCreate(void)
{
    UINT32           Err, i, Rval = SVC_OK;
    static UINT32    IsInit = 0U;
    SVC_TASK_CTRL_s  *pTask;

    AmbaMisra_TouchUnused(&Rval);

    if (IsInit == 0U) {
        TimeLapseCtrl.IsTaskCreate     = 0U;
        IsInit                         = 1U;
    }

    /* create capture task */
    if ((Rval == SVC_OK) && (TimeLapseCtrl.IsTaskCreate == 0U)) {
        AmbaMisra_TouchUnused(&Rval);
        /* create event flag */
        if (Rval == SVC_OK) {
            static char FlagName[] = "SvcTimeLapseFlag";

            Err = AmbaKAL_EventFlagCreate(&(TimeLapseCtrl.Flag), FlagName);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_EventFlagCreate failed (%u)", Err, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaKAL_EventFlagClear(&(TimeLapseCtrl.Flag), 0xFFFFFFFFU);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_EventFlagClear failed (%u)", Err, 0U);
                Rval = SVC_NG;
            }
        }

        /* create mutex */
        if (Rval == SVC_OK) {
            static char FlagName[] = "SvcTimeLapseMutex";

            Err = AmbaKAL_MutexCreate(&(TimeLapseCtrl.Mutex), FlagName);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexCreate failed (%u)", Err, 0U);
                Rval = SVC_NG;
            }
        }

        /* create msg queue */
        if (Rval == SVC_OK) {
#define SVC_TLAPSE_FEED_QUE_NUM (64UL)
            static char   MsgQueName[] = "SvcTimeLapseQue";
            static UINT16 QueBuf[SVC_TLAPSE_FEED_QUE_NUM] GNU_SECTION_NOZEROINIT;

            Err = AmbaKAL_MsgQueueCreate(&(TimeLapseCtrl.FeedDataQue), MsgQueName, (UINT32)sizeof(UINT16),
                                         QueBuf, (UINT32)sizeof(QueBuf));
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MsgQueueCreate failed (%u)", Err, 0U);
                Rval = SVC_NG;
            }
        }

        /* create the task */
        if (Rval == SVC_OK) {
            #define SVC_TLAPSE_CAP_STACK_SIZE (0x8000U)

            static UINT8 CapStack[SVC_TLAPSE_CAP_STACK_SIZE] GNU_SECTION_NOZEROINIT;

            pTask = &TimeLapseCtrl.TaskCtrl;

            pTask->Priority   = TaskCfg.TaskPriority;
            pTask->CpuBits    = TaskCfg.TaskCpuBits;
            pTask->EntryFunc  = TimeLapseTask;
            pTask->EntryArg   = 0U;
            pTask->pStackBase = CapStack;
            pTask->StackSize  = SVC_TLAPSE_CAP_STACK_SIZE;

            Err = SvcTask_Create("TimeLapseTask", pTask);
            if (Err !=SVC_OK) {
                SvcLog_NG(SVC_LOG_TLAPSE, "TimeLapseTask created failed!(%u)", Err, 0U);
                Rval = SVC_NG;
            } else {
                TimeLapseCtrl.IsTaskCreate = 1U;
            }
        }
    }

    for (i = 0U; i < (UINT32)CONFIG_ICAM_MAX_REC_STRM; i++) {
        AmbaSvcWrap_MisraMemset(&(TimeLapseCtrl.CapStrInfo[i]), 0, sizeof(SVC_TLAPSE_STR_CTRL_s));
    }

    return Rval;
}

/**
* Delete time-lapse tasks
* @return ErrorCode
*/
UINT32 SvcTimeLapse_TaskDelete(void)
{
    UINT32 Err, Rval = SVC_OK;

    if (TimeLapseCtrl.IsTaskCreate == 1U) {
        Err = SvcTask_Destroy(&TimeLapseCtrl.TaskCtrl);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_TLAPSE, "TimeLapseTask destroy failed!(%u)", Err, 0U);
            Rval = SVC_NG;
        } else {
            TimeLapseCtrl.IsTaskCreate = 0U;
        }

        Err = AmbaKAL_EventFlagDelete(&(TimeLapseCtrl.Flag));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_EventFlagDelete failed (%u)", Err, 0U);
            Rval = SVC_NG;
        }

        Err = AmbaKAL_MsgQueueDelete(&(TimeLapseCtrl.FeedDataQue));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MsgQueueDelete failed (%u)", Err, 0U);
            Rval = SVC_NG;
        }

        Err = AmbaKAL_MutexDelete(&(TimeLapseCtrl.Mutex));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexDelete failed (%u)", Err, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;
}

/**
* Start time-lapse
* @param [in] RecStreamBits rec stream bits
*/
void SvcTimeLapse_Start(UINT32 RecStreamBits)
{
    UINT32 i, Err, ActualFlags = 0U;

    Err = AmbaKAL_EventFlagGet(&(TimeLapseCtrl.Flag), SVC_TLAPSE_CAP_FLG_STREAM, AMBA_KAL_FLAGS_ANY,
                                   AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_NO_WAIT);
    if (Err != KAL_ERR_NONE) {
        /* nothing */
    }

    /* register yuv capture handler */
    Err = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STL_RAW_RDY, YuvCapDataReady);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaDSP_EventHandlerRegister fail, return %u", Err, 0);
    }

    /* reset some parameters */
    if ((ActualFlags & SVC_TLAPSE_CAP_FLG_STREAM) == 0U) {
        TimeLapseCtrl.LastTime = 0U;
        TimeLapseCtrl.CurTime  = 0U;
    }

    for (i = 0U; i < (UINT32)CONFIG_ICAM_MAX_REC_STRM; i++) {
        if ((RecStreamBits & (1UL << i)) > 0UL) {
            TimeLapseCtrl.CapStrInfo[i].FeedCount = 0U;
            TimeLapseCtrl.CapStrInfo[i].CapCount  = 0U;
            TimeLapseCtrl.CapStrInfo[i].EncCount  = 0U;
        }
    }

    Err = AmbaKAL_EventFlagSet(&(TimeLapseCtrl.Flag), RecStreamBits & SVC_TLAPSE_CAP_FLG_STREAM);
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_EventFlagSet failed (%u/%u)", RecStreamBits, Err);
    }

    Err = AmbaKAL_EventFlagClear(&(TimeLapseCtrl.Flag), SVC_TLAPSE_CAP_FLG_LAST);
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_EventFlagClear failed (%u/%u)", SVC_TLAPSE_CAP_FLG_LAST, Err);
    }

}

/**
* Stop time-lapse
* @param [in] RecStreamBits rec stream bits
*/
void SvcTimeLapse_Stop(UINT32 RecStreamBits)
{
    UINT32 Err, ActualFlags = 0U, i, j, Bit = 1U;

    Err = AmbaKAL_EventFlagClear(&(TimeLapseCtrl.Flag), RecStreamBits & SVC_TLAPSE_CAP_FLG_STREAM);
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_EventFlagSet failed (%u/%u)", RecStreamBits, Err);
    }

    Err = AmbaKAL_EventFlagGet(&(TimeLapseCtrl.Flag), SVC_TLAPSE_CAP_FLG_LAST, AMBA_KAL_FLAGS_ANY,
                                AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, TimeLapseCtrl.CapInterval + 1000U);
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_EventFlagGet SVC_TLAPSE_CAP_FLG_LAST failed (%u)", Err, 0U);
    }

    if ((ActualFlags & SVC_TLAPSE_CAP_FLG_LAST) == 0U) {
        SvcLog_DBG(SVC_LOG_TLAPSE, "Time-lapse hasn't sent the last frame", 0U, 0U);
    }

    Err = AmbaKAL_MutexTake(&(TimeLapseCtrl.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexTake failed (%u)", Err, 0U);
    }

    for (i = 0U; i < (UINT32)CONFIG_ICAM_MAX_REC_STRM; i++) {
        if (0U < CheckBits((Bit << i), RecStreamBits)) {
            for (j = 0U; j < SVC_TLAPSE_CAP_BUF_NUM; j++) {
                TimeLapseCtrl.CapStrInfo[i].BufStatus[j] = SVC_TLAPSE_CAP_BUF_FREE;
            }
        }
    }

    Err = AmbaKAL_MutexGive(&(TimeLapseCtrl.Mutex));
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexGive failed (%u)", Err, 0U);
    }

    /* unregister yuv capture handler */
    Err = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STL_RAW_RDY, YuvCapDataReady);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaDSP_EventHandlerUnRegister fail, return %u", Err, 0);
    }
}

/**
* Do time-lapse configuration
* @param [in] pUsrCfg time-lapse capture configuration
*/
void SvcTimeLapse_CapCfg(const SVC_TLAPSE_CAP_CFG_s *pUsrCfg)
{
    ULONG                  BaseAddr;
    UINT32                 Err = SVC_OK, BufUnitSize;
    UINT32                 RemainSize;
    UINT16                 BufPitch, i, j;
    SVC_TLAPSE_STR_CTRL_s  *pStrInfo;
    AMBA_DSP_DATACAP_CFG_s *pCapCfg;
    const SVC_RES_CFG_s    *pResCfg = SvcResCfg_Get();

    TimeLapseCtrl.CapBufAddr  = TaskCfg.CapBufAddr;
    TimeLapseCtrl.CapBufSize  = TaskCfg.CapBufSize;
    TimeLapseCtrl.CapInterval = pUsrCfg->CapInterval;

    BaseAddr   = TimeLapseCtrl.CapBufAddr;
    RemainSize = TimeLapseCtrl.CapBufSize;

    AmbaSvcWrap_MisraMemset(TimeLapseCtrl.CapInfoId, 255, sizeof(TimeLapseCtrl.CapInfoId));

    for (i = 0U; i < (UINT16)CONFIG_ICAM_MAX_REC_STRM; i++) {
        pStrInfo = &(TimeLapseCtrl.CapStrInfo[i]);

        if (((pUsrCfg->RecStreamBits) & (1UL << i)) > 0UL) {
            pStrInfo->Enable = 1U;
        } else {
            pStrInfo->Enable = 0U;
        }

        for (j = 0U; j < SVC_TLAPSE_CAP_BUF_NUM; j++) {
            pCapCfg  = &(pStrInfo->CapCfg[j]);
            AmbaSvcWrap_MisraMemset(pCapCfg, 0, sizeof(AMBA_DSP_DATACAP_CFG_s));

            pCapCfg->CapDataType   = DSP_DATACAP_TYPE_VIDEO_YUV;
            pCapCfg->Index         = i;
            pCapCfg->AllocType     = ALLOC_EXTERNAL_CYCLIC;
            pCapCfg->BufNum        = 1U;
            pCapCfg->OverFlowCtrl  = 1U;
            pCapCfg->AuxDataNeeded = 0U;

            /* assign cap buffer */
            if (pStrInfo->Enable == 1U) {
                /* yuv buffer */
                if (Err == SVC_OK) {
                    Err = AmbaDSP_CalcStillYuvExtBufSize(i + (UINT16)pResCfg->DispNum, DSP_DATACAP_BUFTYPE_YUV, &BufPitch, &BufUnitSize);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaDSP_CalcStillYuvExtBufSize DSP_DATACAP_BUFTYPE_YUV failed %u", Err, 0U);
                    }
                    BufUnitSize = GetAlignedValU32(BufUnitSize, 64U);

                    if (RemainSize < (BufUnitSize)) {
                        Err = SVC_NG;
                        SvcLog_NG(SVC_LOG_TLAPSE, "SvcTimeLapse CapYUV Buf is not enough (%u/%u)", BufUnitSize, RemainSize);
                    } else {
                        pCapCfg->DataBuf.BaseAddr       = BaseAddr;
                        pCapCfg->DataBuf.Pitch          = BufPitch;
                        pCapCfg->DataBuf.Window.Width   = pResCfg->RecStrm[i].StrmCfg.Win.Width;
                        pCapCfg->DataBuf.Window.Height  = pResCfg->RecStrm[i].StrmCfg.Win.Height;
                        pCapCfg->DataBuf.Window.OffsetX = 0U;
                        pCapCfg->DataBuf.Window.OffsetY = 0U;

                        BaseAddr    += BufUnitSize;
                        RemainSize  -= BufUnitSize;

                        // SvcLog_DBG(SVC_LOG_TLAPSE, "CapYUV Buf(%u/%u)", i, j);
                        // SvcLog_DBG(SVC_LOG_TLAPSE, "          (%u/%u)", pCapCfg->DataBuf.BaseAddr, BufUnitSize);
                    }

                }

                /* aux buffer */
                if (Err == SVC_OK) {
                    Err = AmbaDSP_CalcStillYuvExtBufSize(i + (UINT16)pResCfg->DispNum, DSP_DATACAP_BUFTYPE_AUX, &BufPitch, &BufUnitSize);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaDSP_CalcStillYuvExtBufSize DSP_DATACAP_BUFTYPE_AUX failed %u", Err, 0U);
                    }
                    BufUnitSize = GetAlignedValU32(BufUnitSize, 64U);

                    if (RemainSize < (BufUnitSize)) {
                        Err = SVC_NG;
                        SvcLog_NG(SVC_LOG_TLAPSE, "SvcTimeLapse CapAUX Buf is not enough (%u/%u)", BufUnitSize, RemainSize);
                    } else {
                        pCapCfg->AuxDataBuf.BaseAddr       = BaseAddr;
                        pCapCfg->AuxDataBuf.Pitch          = BufPitch;
                        pCapCfg->AuxDataBuf.Window.Width   = pResCfg->RecStrm[i].StrmCfg.Win.Width;
                        pCapCfg->AuxDataBuf.Window.Height  = pResCfg->RecStrm[i].StrmCfg.Win.Height;
                        pCapCfg->AuxDataBuf.Window.OffsetX = 0U;
                        pCapCfg->AuxDataBuf.Window.OffsetY = 0U;

                        BaseAddr    += BufUnitSize;
                        RemainSize  -= BufUnitSize;

                        // SvcLog_DBG(SVC_LOG_TLAPSE, "CapAUX Buf(%u/%u)", i, j);
                        // SvcLog_DBG(SVC_LOG_TLAPSE, "          (%u/%u)", pCapCfg->AuxDataBuf.BaseAddr, BufUnitSize);
                    }
                }
            }
        }
    }

    SvcLog_DBG(SVC_LOG_TLAPSE, "RemainSize = 0x%X", RemainSize, 0U);
}

/**
* To update buffer status once the frame is encoded done
* @param [in] StreamId Encode stream Id
*/
void SvcTimeLapse_EncOneFrameNotify(UINT32 StreamId)
{
    UINT32 BufId, Err;

    BufId = TimeLapseCtrl.CapStrInfo[StreamId].EncCount % SVC_TLAPSE_CAP_BUF_NUM;

    Err = AmbaKAL_MutexTake(&(TimeLapseCtrl.Mutex), AMBA_KAL_WAIT_FOREVER);
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexTake failed (%u)", Err, 0U);
    }

    TimeLapseCtrl.CapStrInfo[StreamId].BufStatus[BufId] = SVC_TLAPSE_CAP_BUF_FREE;

    Err = AmbaKAL_MutexGive(&(TimeLapseCtrl.Mutex));
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(SVC_LOG_TLAPSE, "AmbaKAL_MutexGive failed (%u)", Err, 0U);
    }

    TimeLapseCtrl.CapStrInfo[StreamId].EncCount++;
}
