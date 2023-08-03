/**
*  @file SvcDataGather.c
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
*  @details svc data gather
*
*/

#include "AmbaMisraFix.h"
#include "AmbaDef.h"
#include "AmbaVIN.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"

#if defined(CONFIG_AMBA_AVB)
#include "NetStack.h"
#include "AvbStack.h"
#endif

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcCan.h"
#include "SvcImg.h"
#include "SvcResCfg.h"
#include "SvcDataGather.h"


#define LOG_DATG            "DATG"

static inline void DATG_DBG(UINT32 DbgLvl, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    #define DATG_DBGLVL_PRN     (0U)

#if DATG_DBGLVL_PRN
    if (0U < (DbgLvl & (UINT32)DATG_DBGLVL_PRN)) {
        SvcLog_DBG(LOG_DATG, pFormat, Arg1, Arg2);
    }
#else
    ULONG Temp = 0U;
    AmbaMisra_TypeCast(&Temp, pFormat);
    AmbaMisra_TouchUnused(&DbgLvl);
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
#endif
}

#define BUFFER_NUM          (640U)
#define STACK_SIZE          (0x3000U)

#if defined(CONFIG_ICAM_RECORD_USED)
#define MAX_ID_NUM          ((UINT32)CONFIG_ICAM_MAX_REC_STRM + 0x00000001U)
#else
#define MAX_ID_NUM          (0x00000001U)
#endif

#define FLG_START_FETCH     (0x00000001U)

#define OVERFLOW_NONE       (0x00000000U)
#define OVERFLOW_SYNC       (0x00000001U)
#define OVERFLOW_DATA       (0x00000002U)

#define AMBA_DATG_RAWQ_MAX   (64U * AMBA_NUM_VIN_CHANNEL)

#define KEEP_NUM             (10U)   /* the number of the data to keep before DataRdIdx */
#define DATG_TIMEOUT         (5000U)

typedef struct {
    UINT16             IsUsed;
    UINT16             TickType;
    UINT32             VinBits;
    UINT32             FovBits;
    UINT32             Flags;
    UINT32             SyncTime;
    UINT32             DataRdIdx;
    AMBA_DATG_READY_CB  pDataReadyCB;
} CTRL_INFO_s;

typedef struct {
    SVC_TASK_CTRL_s          TaskCtrl;

    UINT32                   RawWrIdx;
    AMBA_KAL_MSG_QUEUE_t     RawQueID;
    AMBA_DSP_RAW_DATA_RDY_s  *RawQue[AMBA_DATG_RAWQ_MAX];
    AMBA_KAL_MUTEX_t         Mtx;

    AMBA_DATG_s              *pDataBuf;
    UINT64                   DataCount;
    UINT32                   DataWrIdx;
    UINT32                   DataRdIdx;
} TASK_INFO_s;

static CTRL_INFO_s  CtrlInfo[MAX_ID_NUM] = {0U};
static UINT32       g_TaskExist = 0U;
static TASK_INFO_s  TaskInfo GNU_SECTION_NOZEROINIT;
static AMBA_DATG_s  DataBuffer[BUFFER_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 CalcAbsTimeDiff(UINT32 DataTime, UINT32 SyncTime, UINT32 Overflow)
{
    UINT32 Diff;
    if (Overflow == OVERFLOW_DATA) {
        Diff = DataTime - 0U;
        Diff += ((0xFFFFFFFFU) - SyncTime);
    } else if (Overflow == OVERFLOW_SYNC) {
        Diff = SyncTime - 0U;
        Diff += ((0xFFFFFFFFU) - DataTime);
    } else {
        if (SyncTime > DataTime) {
            Diff = SyncTime - DataTime;
        } else {
            Diff = DataTime - SyncTime;
        }
    }

    return Diff;
}

static UINT32 SyncData(UINT16 CtrlId, UINT16 TickType, UINT32 SyncTime)
{
    #define DATG_DBGL_FD        (0x02U)

    UINT32            i, SchNum, SyncIdx, RdIdx, MinDiff = 0U, TmpDiff;
    UINT32            OverFlow = OVERFLOW_NONE, LatestTime = 0U, FromPrev = 0U;
    const AMBA_DATG_s  *pData;

    RdIdx   = BUFFER_NUM;

    if (g_TaskExist == 1U) {
        /* search from the latest data */
        SyncIdx = TaskInfo.DataWrIdx;

        /* search the data which has min diff (capture time and sync time) */
        SchNum = GetMinValU32(BUFFER_NUM, (UINT32)TaskInfo.DataCount);
        for (i = 0U; i < SchNum; i++) {
            if (SyncIdx == 0U) {
                SyncIdx = (BUFFER_NUM - 1U);
            } else {
                SyncIdx--;
            }
            pData = &(TaskInfo.pDataBuf[SyncIdx]);

            if (i == 0U) {
                LatestTime = (UINT32)pData->CapTime[TickType];
                if (LatestTime < SyncTime) {
                    OverFlow = OVERFLOW_DATA;
                }
                MinDiff = CalcAbsTimeDiff(LatestTime, SyncTime, OverFlow);
                RdIdx = SyncIdx;
                DATG_DBG(DATG_DBGL_FD, "## SyncIdx/FovBits(%u/0x%X)", SyncIdx, pData->FovBits);
                DATG_DBG(DATG_DBGL_FD, "##   SyncTime/LatestTime(%u/%u)", SyncTime, LatestTime);
            } else {
                if (LatestTime < (UINT32)pData->CapTime[TickType]) {
                    if (OverFlow == OVERFLOW_DATA) {
                        OverFlow = OVERFLOW_NONE;
                    } else {
                        OverFlow = OVERFLOW_SYNC;
                    }
                }
                TmpDiff = CalcAbsTimeDiff((UINT32)pData->CapTime[TickType], SyncTime, OverFlow);

                DATG_DBG(DATG_DBGL_FD, "## SyncIdx/FovBits(%u/0x%X)", SyncIdx, pData->FovBits);
                DATG_DBG(DATG_DBGL_FD, "##   MinDiff/TmpDiff(%u/%u)", MinDiff, TmpDiff);
                if (TmpDiff < MinDiff) {
                    MinDiff = TmpDiff;
                    RdIdx = SyncIdx;
                } else {
                    /* min diff is found */
                    break;
                }
            }
        }
    } else {
        if (TaskInfo.DataRdIdx >= KEEP_NUM) {
            SyncIdx = TaskInfo.DataRdIdx - KEEP_NUM;
        } else {
            if (TaskInfo.DataCount <= BUFFER_NUM) {
                SyncIdx = 0U;
            } else {
                SyncIdx = (BUFFER_NUM + TaskInfo.DataRdIdx) - KEEP_NUM;
                FromPrev = 1U;
            }
        }
        SchNum  = GetMinValU32(BUFFER_NUM, (UINT32)TaskInfo.DataCount);

        for (i = 0U; i < SchNum; i++) {
            pData = &(TaskInfo.pDataBuf[SyncIdx]);
            if (pData->CapTime[TICK_TYPE_SYS] >=  SyncTime) {
                RdIdx              = SyncIdx;
                if (((FromPrev == 0U) && (OverFlow == OVERFLOW_NONE)) || ((FromPrev == 1U) && (OverFlow == OVERFLOW_DATA))) {
                    if (TaskInfo.DataRdIdx < (SyncIdx + 1U)) {
                        TaskInfo.DataRdIdx = SyncIdx + 1U;
                    }
                } else if ((FromPrev == 0U) && (OverFlow == OVERFLOW_DATA)) {
                    TaskInfo.DataRdIdx = SyncIdx + 1U;
                } else {
                    /**/
                }

                if (TaskInfo.DataRdIdx >= BUFFER_NUM) {
                    TaskInfo.DataRdIdx -= BUFFER_NUM;
                }
                break;
            }

            SyncIdx ++;
            if (SyncIdx >= BUFFER_NUM) {
                SyncIdx -= BUFFER_NUM;
                OverFlow = OVERFLOW_DATA;
            }

            AmbaMisra_TouchUnused(&OverFlow);
        }
    }

    /* update read pointer to the data we found*/
    if (RdIdx < BUFFER_NUM) {
        // SvcLog_DBG(LOG_DATG, "[datag%u] sync data(%u)", CtrlId, RdIdx);
    } else {
        SvcLog_NG(LOG_DATG, "[datag%u] can't sync data", CtrlId, 0U);
    }

    return RdIdx;
}

static void StoreData(AMBA_DATG_s *pData, const AMBA_DSP_RAW_DATA_RDY_s *pRawData)
{
    UINT32  i, Err;

    /* vin id and capture ticks */
    Err = AmbaKAL_GetSysTickCount(&i);
    if (Err != SVC_OK) {
        SvcLog_NG(LOG_DATG, "AmbaKAL_GetSysTickCount failed(%u)", Err, 0U);
    }

    pData->VinBits                  = ((UINT32)0x01 << pRawData->VinId);
    pData->CapTime[TICK_TYPE_AUDIO] = pRawData->CapPts;
    pData->CapTime[TICK_TYPE_SYS]   = i;
    pData->FovBits                  = 0U;

    AmbaMisra_TouchUnused(&(pData->FovBits));

    /* 3A data */
#if defined(CONFIG_BUILD_IMGFRW_AAA)
    {
        #define DATG_DBGL_WD        (0x01U)

        UINT32               v, Idx, AlgoNum;
        SVC_IMG_ALGO_INFO_s  AlgoInfo[AMBA_DSP_MAX_VIEWZONE_NUM];

        DATG_DBG(DATG_DBGL_WD, "## S VinId/CapSequence(%d/%u)", pRawData->VinId, (UINT32)pRawData->CapSequence);

        AmbaSvcWrap_MisraMemset(AlgoInfo, 0, sizeof(AlgoInfo));
        SvcImg_AlgoInfoGet((UINT32)pRawData->VinId, (UINT32)pRawData->CapSequence, AlgoInfo, &AlgoNum);
        DATG_DBG(DATG_DBGL_WD, "##   CapTime/AlgoNum(%u/%u)", (UINT32)pRawData->CapPts, AlgoNum);

        for (i = 0U; i < AlgoNum; i++) {
            for (v = 0U; v < AMBA_DSP_MAX_VIEWZONE_NUM; v++) {
                if (0U < (AlgoInfo[i].Fov.SelectBits & ((UINT32)0x01U << v))) {
                    pData->AaaData[v].ExposureNum = AlgoInfo[i].ExposureNum;

                    for (Idx = 0U; Idx < 4U; Idx++) {
                        pData->AaaData[v].Ae[Idx].ExposureTime = AlgoInfo[i].Ae[Idx].ExposureTime;
                        pData->AaaData[v].Ae[Idx].Gain         = AlgoInfo[i].Ae[Idx].Gain;
                    }

                    for (Idx = 0U; Idx < 3U; Idx++) {
                        pData->AaaData[v].Awb.Wgc[Idx] = AlgoInfo[i].Awb.Wgc[Idx];
                    }
                }
            }
        }
    }
#endif

    if (pData->FovBits == 0U) {
        UINT32 FovNum = 0U, Bit = 1U;
        UINT32 FovIdx[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};

        Err = SvcResCfg_GetFovIdxsInVinID(pRawData->VinId, FovIdx, &FovNum);
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_DATG, "SvcResCfg_GetFovIdxsInVinID failed(%u)", Err, 0U);
        } else {
            for (i = 0U; i < FovNum; i++) {
                pData->FovBits |= (Bit << FovIdx[i]);
            }
        }
    }

    /* canbus data */
#if defined(CONFIG_ICAM_CANBUS_USED)
#if defined(CONFIG_ICAM_DATA_GATHER_USED)
    {
        AMBA_SR_CANBUS_RAW_DATA_s       RawData;
        AMBA_SR_CANBUS_TRANSFER_DATA_s  XferData;

        SvcCan_GetRawData(&RawData);
        Err = SvcCan_Transfer(&RawData, &XferData);
        if (Err != ADAS_ERR_NONE) {
            SvcLog_NG(LOG_DATG, "SvcCan_Transfer failed(%u)", Err, 0U);
        } else {
            pData->CanBus.TurnLightStatus     = XferData.CANBusTurnLightStatus;
            pData->CanBus.GearStatus          = XferData.CANBusGearStatus;
            pData->CanBus.XferSpeed           = XferData.TransferSpeed;
            pData->CanBus.WheelDir            = XferData.WheelDir;
            pData->CanBus.XferWheelAngle      = XferData.TransferWheelAngle;
            pData->CanBus.XferWheelAngleSpeed = XferData.TransferWheelAngleSpeed;
        }
    }
#endif
#endif

#if defined(CONFIG_AMBA_AVB)
    /* ethernet ptp */
    {
        Err = AmbaAvbStack_GetPhc(0U, &(pData->EthPTP.Sec), &(pData->EthPTP.NanoSec));
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_DATG, "fail to get ptp time", 0U, 0U);
        }
    }
#endif

    TaskInfo.DataCount++;
}

static UINT32 LivRawDatatRdy(const void *pEventData)
{
    UINT32                          Err, MtxTake = 0U;
    AMBA_DSP_RAW_DATA_RDY_s         *pRawData;
    static AMBA_DSP_RAW_DATA_RDY_s  RawBuffer[AMBA_DATG_RAWQ_MAX] GNU_SECTION_NOZEROINIT;

    Err = AmbaKAL_MutexTake(&(TaskInfo.Mtx), DATG_TIMEOUT);
    if (Err != KAL_ERR_NONE) {
        SvcLog_NG(LOG_DATG, "AmbaKAL_MutexTake failed %u", Err, 0U);
    } else {
        MtxTake = 1U;
    }

    if (MtxTake == 1U) {
        if (TaskInfo.RawWrIdx < AMBA_DATG_RAWQ_MAX) {
            pRawData = &(RawBuffer[TaskInfo.RawWrIdx]);

            Err = AmbaWrap_memcpy(pRawData, pEventData, sizeof(AMBA_DSP_RAW_DATA_RDY_s));
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_DATG, "AmbaWrap_memcpy failed", 0U, 0U);
            }

            if (SVC_OK != AmbaKAL_MsgQueueSend(&(TaskInfo.RawQueID), &pRawData, AMBA_KAL_NO_WAIT)) {
                SvcLog_NG(LOG_DATG, "fail to enqueue", 0U, 0U);
            }

            TaskInfo.RawWrIdx++;
            if (TaskInfo.RawWrIdx >= AMBA_DATG_RAWQ_MAX) {
                TaskInfo.RawWrIdx -= AMBA_DATG_RAWQ_MAX;
            }
        } else {
            SvcLog_NG(LOG_DATG, "Invalid RawWrIdx %u/%u", TaskInfo.RawWrIdx, AMBA_DATG_RAWQ_MAX);
        }
    }

    if (MtxTake == 1U) {
        Err = AmbaKAL_MutexGive(&(TaskInfo.Mtx));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DATG, "AmbaKAL_MutexGive failed %u", Err, 0U);
        }
    }

    return SVC_OK;
}

static void* DataGather_TaskEntry(void* EntryArg)
{
    UINT32                   Rval = SVC_OK;
    UINT16                   CtrlId;
    TASK_INFO_s              *pTaskInfo;
    AMBA_DSP_RAW_DATA_RDY_s  *pRawData = NULL;
    AMBA_DATG_s              *pData;
    CTRL_INFO_s              *pCtrl;
    const ULONG              *pArg;
    ULONG                    Arg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(pRawData);

    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Arg = (*pArg);
    AmbaMisra_TypeCast(&pTaskInfo, &Arg);

    SvcLog_OK(LOG_DATG, "DataGather_TaskEntry created", 0U, 0U);

    while (Rval == SVC_OK) {
        Rval = AmbaKAL_MsgQueueReceive(&(pTaskInfo->RawQueID), &pRawData, AMBA_KAL_WAIT_FOREVER);
        if (Rval != SVC_OK) {
            SvcLog_NG(LOG_DATG, "AmbaKAL_MsgQueueReceive failed", 0U, 0U);
            continue;
        }

        /* store data to ring buffer */
        pData = &(TaskInfo.pDataBuf[TaskInfo.DataWrIdx]);
        StoreData(pData, pRawData);

        /* update write pointer */
        TaskInfo.DataWrIdx++;
        if (TaskInfo.DataWrIdx >= BUFFER_NUM) {
            TaskInfo.DataWrIdx -= BUFFER_NUM;
        }

        /* fetch start */
        for (CtrlId = 0U; CtrlId < (UINT16)MAX_ID_NUM; CtrlId++) {
            pCtrl = &(CtrlInfo[CtrlId]);
            if (pCtrl->IsUsed == 0U) {
                continue;
            }

            if (0U < (pCtrl->Flags & FLG_START_FETCH)) {
                if (pCtrl->DataRdIdx < BUFFER_NUM) {
                    pData = &(TaskInfo.pDataBuf[pCtrl->DataRdIdx]);

                    /* give data */
                    if (0U < (pData->FovBits & pCtrl->FovBits)) {
                        pCtrl->pDataReadyCB(CtrlId, pData);
                    }

                    /* update read pointer */
                    pCtrl->DataRdIdx++;
                    if (pCtrl->DataRdIdx >= BUFFER_NUM) {
                        pCtrl->DataRdIdx -= BUFFER_NUM;
                    }
                }
            }
        }
    }

    return NULL;
}


/**
* create of data gather task
* @param [in] Priority task priority
* @param [in] CpuBits task cpu bits
* @return 0-OK, 1-NG
*/
UINT32 SvcDataGather_Create(UINT32 Priority, UINT32 CpuBits)
{
    static char        DataGQue[] = "DataGQue";
    static char        DataGMtx[] = "DataGMtx";
    static UINT8       TaskStack[STACK_SIZE] GNU_SECTION_NOZEROINIT;

    UINT32       Rval = SVC_OK, Err;
    TASK_INFO_s  *pInfo = &TaskInfo;

    if (g_TaskExist != 1U) {
        AmbaSvcWrap_MisraMemset(pInfo, 0, sizeof(TASK_INFO_s));
        Err = AmbaWrap_memset(DataBuffer, 0, sizeof(DataBuffer));
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_DATG, "AmbaWrap_memset failed(%u)", Err, 0U);
        }

        /* buffer boundaries */
        pInfo->pDataBuf  = &(DataBuffer[0U]);
        pInfo->DataCount = 0U;
        pInfo->DataWrIdx = 0U;

        pInfo->RawWrIdx  = 0U;

        Err = AmbaKAL_MsgQueueCreate(&(pInfo->RawQueID), DataGQue, (UINT32)sizeof(AMBA_DSP_RAW_DATA_RDY_s*),
                                     &(pInfo->RawQue[0U]), (UINT32)sizeof(pInfo->RawQue));
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(LOG_DATG, "fail to create queue", 0U, 0U);
            Rval = SVC_NG;
        }

        /* create mutex */
        if (Rval == SVC_OK) {
            Err = AmbaKAL_MutexCreate(&(pInfo->Mtx), DataGMtx);
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(LOG_DATG, "AmbaKAL_MutexCreate failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        /* create task */
        if (Rval == SVC_OK) {
            pInfo->TaskCtrl.Priority    = Priority;
            pInfo->TaskCtrl.EntryFunc   = DataGather_TaskEntry;
            pInfo->TaskCtrl.pStackBase  = TaskStack;
            pInfo->TaskCtrl.StackSize   = STACK_SIZE;
            pInfo->TaskCtrl.CpuBits     = CpuBits;
            AmbaMisra_TypeCast(&(pInfo->TaskCtrl.EntryArg), &pInfo);

            Err = SvcTask_Create("SvcDataGather", &(pInfo->TaskCtrl));
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_DATG, "SvcTask_Create failed(%u)", Err, 0U);
                Rval = SVC_NG;
            } else {
                g_TaskExist = 1U;
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, LivRawDatatRdy);
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_DATG, "fail to register LV_RAW_RDY event", 0U, 0U);
                Rval = SVC_NG;
            }
        }

    } else {
        SvcLog_DBG(LOG_DATG, "SvcDataGather task is already existed ", 0U, 0U);
    }

    return Rval;
}

/**
* destroy of data gather task
* @return 0-OK, 1-NG
*/
UINT32 SvcDataGather_Destroy(void)
{
    UINT32 Rval = SVC_OK, Err;

    if (g_TaskExist == 1U) {
        Err = SvcTask_Destroy(&(TaskInfo.TaskCtrl));
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_DATG, "SvcTask_Destroy failed(%u)", Err, 0U);
            Rval = SVC_NG;
        }

        if (Rval == SVC_OK) {
            Err = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, LivRawDatatRdy);
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_DATG, "fail to unregister LV_RAW_RDY event", 0U, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaKAL_MsgQueueDelete(&(TaskInfo.RawQueID));
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(LOG_DATG, "fail to delete queue", 0U, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            Err = AmbaKAL_MutexDelete(&(TaskInfo.Mtx));
            if (Err != KAL_ERR_NONE) {
                SvcLog_NG(LOG_DATG, "AmbaKAL_MutexDelete failed %u", Err, 0U);
                Rval = SVC_NG;
            }
        }

        Err = AmbaWrap_memset(CtrlInfo, 0, MAX_ID_NUM * sizeof(CTRL_INFO_s));
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_DATG, "AmbaWrap_memset(%u)", Err, 0U);
            Rval = SVC_NG;
        }

        if (Rval == SVC_OK) {
            g_TaskExist = 0U;
        }

    } else {
        SvcLog_DBG(LOG_DATG, "The task is not existed", 0U, 0U);
    }

    return Rval;
}

/**
* register of data gather callback function
* @param [out] pCtrlId id of control
* @param [in] pDataReadyCB callback function of user
* @return 0-OK, 1-NG
*/
UINT32 SvcDataGather_Register(UINT16 *pCtrlId, AMBA_DATG_READY_CB pDataReadyCB)
{
    UINT32 Rval = SVC_OK, i, IsFind = 0U;

    for (i = 0U; i < MAX_ID_NUM; i++) {
        if (CtrlInfo[i].IsUsed == 0U) {
            CtrlInfo[i].IsUsed        = 1U;
            CtrlInfo[i].pDataReadyCB  = pDataReadyCB;
            *pCtrlId   = (UINT16)i;
            IsFind     = 1U;
            break;
        }
    }

    if (IsFind == 0U) {
        SvcLog_NG(LOG_DATG, "No available control Id", 0U, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

/**
* un-register of data gather callback function
* @param [in] CtrlId id of control
* @return 0-OK, 1-NG
*/
UINT32 SvcDataGather_UnRegister(UINT16 CtrlId)
{
    UINT32 Rval = SVC_OK, Err;

    if (CtrlId < (UINT16)MAX_ID_NUM) {
        if (CtrlInfo[CtrlId].IsUsed == 1U) {
            Err = AmbaWrap_memset(&(CtrlInfo[CtrlId]), 0, sizeof(CTRL_INFO_s));
            if (Err != SVC_OK) {
                SvcLog_NG(LOG_DATG, "AmbaWrap_memset failed(%u)", Err, 0U);
                Rval = SVC_NG;
            }
        }
    } else {
        SvcLog_NG(LOG_DATG, "invalid CtrlId(%u)", CtrlId, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

/**
* start of data fetch
* @param [in] CtrlId id of control
* @param [in] TickType time format of search
* @param [in] RecStrmIdx index of record stream
* @param [in] SyncTime sync time of fist data
* @return none
*/
void SvcDataGather_StartFetch(UINT16 CtrlId, UINT16 TickType, UINT32 RecStrmIdx, UINT64 SyncTime)
{
    UINT32  VinBits, FovBits, RdIdx, Sync, i, Bit = 1U;

    if (CtrlId < (UINT16)MAX_ID_NUM) {
        if (CtrlInfo[CtrlId].IsUsed == 1U) {
            if (TickType < TICK_TYPE_MAX_NUM) {
                Sync = (UINT32)((UINT64)0xFFFFFFFFUL & SyncTime);
                RdIdx = SyncData(CtrlId, TickType, Sync);
                if (RdIdx < BUFFER_NUM) {
                    SvcResCfg_GetVinBitsOfRecIdx(RecStrmIdx, &VinBits);
                    SvcResCfg_GetFovBitsOfRecIdx(RecStrmIdx, &FovBits);

                    /* for the RecStream which is combined from multi-FOV, we only use
                       one of the FOVs to represent for this CtrlId*/
                    for (i = 0U; i < (UINT32)AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
                        if ((FovBits & (Bit << i)) > 0U) {
                            FovBits = Bit << i;
                            break;
                        }
                    }

                    CtrlInfo[CtrlId].DataRdIdx = RdIdx;
                    CtrlInfo[CtrlId].VinBits   = VinBits;
                    CtrlInfo[CtrlId].FovBits   = FovBits;
                    CtrlInfo[CtrlId].TickType  = TickType;
                    CtrlInfo[CtrlId].SyncTime  = Sync;
                    CtrlInfo[CtrlId].Flags |= FLG_START_FETCH;
                } else {
                    SvcLog_NG(LOG_DATG, "fail to sync(%u)", TickType, 0U);
                }
            } else {
                SvcLog_NG(LOG_DATG, "Invalid TickType(%u)", TickType, 0U);
            }
        } else {
            SvcLog_NG(LOG_DATG, "CtrlId(%u) is not registered", CtrlId, 0U);
        }
    } else {
        SvcLog_NG(LOG_DATG, "invalid CtrlId(%u)", CtrlId, 0U);
    }
}

/**
* stop of data fetch
* @param [in] CtrlId id of control
* @return none
*/
void SvcDataGather_StopFetch(UINT16 CtrlId)
{
    if (CtrlId < (UINT16)MAX_ID_NUM) {
        if (CtrlInfo[CtrlId].IsUsed == 1U) {
            CtrlInfo[CtrlId].Flags = 0U;
        } else {
            SvcLog_NG(LOG_DATG, "CtrlId(%u) is not registered", CtrlId, 0U);
        }
    } else {
        SvcLog_NG(LOG_DATG, "invalid CtrlId(%u)", CtrlId, 0U);
    }
}

/**
* status dump of data gather
* @param [in] PrintFunc print function of shell
* @return none
*/
void SvcDataGather_Dump(AMBA_SHELL_PRINT_f PrintFunc)
{
    char               StrBuf[256] = {'\0'};
    UINT32             i, Rval, BufLen = 256U;
    const CTRL_INFO_s  *pCtrl;
    const AMBA_DATG_s   *pData;

    PrintFunc("## data gather information ##\n");
    SVC_WRAP_SNPRINT            "data wr_idx/count(%u/%llu)\n\n"
        SVC_SNPRN_ARG_S         StrBuf
        SVC_SNPRN_ARG_UINT32    TaskInfo.DataWrIdx SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_UINT64    TaskInfo.DataCount
        SVC_SNPRN_ARG_BSIZE     BufLen
        SVC_SNPRN_ARG_RLEN      &Rval
        SVC_SNPRN_ARG_E
    PrintFunc(StrBuf);

    PrintFunc("[register]\n");
    for (i = 0U; i < MAX_ID_NUM; i++) {
        pCtrl = &(CtrlInfo[i]);
        if (pCtrl->IsUsed == 0U) {
            continue;
        }

        SVC_WRAP_SNPRINT            " (c%d)\n  vin_bits/fov_bits/sync_time(0x%X/0x%X/%u)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    i               SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    pCtrl->VinBits  SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    pCtrl->FovBits  SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    pCtrl->SyncTime SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);

        SVC_WRAP_SNPRINT            "  flags/rd_idx(0x%X/%u)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    pCtrl->Flags     SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    pCtrl->DataRdIdx SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);
    }

    PrintFunc("[latest data]\n");
    i = TaskInfo.DataWrIdx;
    if (i == 0U) {
        i = (BUFFER_NUM - 1U);
    } else {
        i--;
    }
    pData = &(TaskInfo.pDataBuf[i]);

    SVC_WRAP_SNPRINT            " idx/vin_bits/fov_bits/tm0/tm1(%u/0x%X/0x%X/%llu/%llu)\n"
        SVC_SNPRN_ARG_S         StrBuf
        SVC_SNPRN_ARG_UINT32    i                 SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_UINT32    pData->VinBits    SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_UINT32    pData->FovBits    SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_UINT64    pData->CapTime[0]
        SVC_SNPRN_ARG_UINT64    pData->CapTime[1]
        SVC_SNPRN_ARG_BSIZE     BufLen
        SVC_SNPRN_ARG_RLEN      &Rval
        SVC_SNPRN_ARG_E
    PrintFunc(StrBuf);

    /* dump 3A */
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        if ((pData->FovBits & ((UINT32)0x01U << i)) == 0U) {
            continue;
        }

        SVC_WRAP_SNPRINT            "  (fov%u) exp_num/ae0/ae1(%u/%.3f/%.3f)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    i                                    SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    pData->AaaData[i].ExposureNum        SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_DOUBLE    pData->AaaData[i].Ae[0].ExposureTime
            SVC_SNPRN_ARG_DOUBLE    pData->AaaData[i].Ae[1].ExposureTime
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);
    }

    /* dump canbus */
    SVC_WRAP_SNPRINT            "  (canbus) L:%u G:%u XS:%.4f WD:%u XWA:%.4f XWAS:%.4f\n"
        SVC_SNPRN_ARG_S         StrBuf
        SVC_SNPRN_ARG_UINT8     pData->CanBus.TurnLightStatus     SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_UINT8     pData->CanBus.GearStatus          SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_DOUBLE    pData->CanBus.XferSpeed
        SVC_SNPRN_ARG_UINT32    pData->CanBus.WheelDir            SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_DOUBLE    pData->CanBus.XferWheelAngle
        SVC_SNPRN_ARG_DOUBLE    pData->CanBus.XferWheelAngleSpeed
        SVC_SNPRN_ARG_BSIZE     BufLen
        SVC_SNPRN_ARG_RLEN      &Rval
        SVC_SNPRN_ARG_E
    PrintFunc(StrBuf);

    /* dump ethernet PTP */
    SVC_WRAP_SNPRINT            "  (eth_ptp) S:%u NS:%u\n"
        SVC_SNPRN_ARG_S         StrBuf
        SVC_SNPRN_ARG_UINT32    pData->EthPTP.Sec     SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_UINT32    pData->EthPTP.NanoSec SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_BSIZE     BufLen
        SVC_SNPRN_ARG_RLEN      &Rval
        SVC_SNPRN_ARG_E
    PrintFunc(StrBuf);
}

#if defined(CONFIG_BUILD_AMBA_ADAS)
/**
* get data by time
* @param [in] TickType type of time
* @param [in] SyncTime time to sync
* @param [out] pData data
* @return 0-OK, 1-NG
*/
UINT32 SvcDataGather_GetCanData(UINT16 TickType, UINT64 SyncTime, AMBA_SR_CANBUS_TRANSFER_DATA_s *pData)
{
    UINT32      Rval = SVC_NG, Sync, RdIdx;
    UINT16      TType = TickType;

    if ((TickType <= TICK_TYPE_SYS) && (g_TaskExist == 1U)) {
        Rval  = SVC_OK;
    } else if ((g_TaskExist == 0U)) {
        TType = TICK_TYPE_SYS;
        Rval  = SVC_OK;
    } else {
        SvcLog_DBG(LOG_DATG, "TickType not matched (%u)", TickType, 0U);
    }

    if (Rval == SVC_OK) {
        Sync = (UINT32)((UINT64)0xFFFFFFFFUL & SyncTime);
        RdIdx = SyncData((UINT16)MAX_ID_NUM, TType, Sync);

        if (RdIdx < BUFFER_NUM) {
            pData->FlagValidTransferData    = 1U;
            pData->CapTS                    = (UINT32)((UINT64)TaskInfo.pDataBuf[RdIdx].CapTime[TICK_TYPE_AUDIO]);
            pData->CANBusTurnLightStatus    = TaskInfo.pDataBuf[RdIdx].CanBus.TurnLightStatus;
            pData->CANBusGearStatus         = TaskInfo.pDataBuf[RdIdx].CanBus.GearStatus;
            pData->TransferSpeed            = TaskInfo.pDataBuf[RdIdx].CanBus.XferSpeed;
            pData->WheelDir                 = TaskInfo.pDataBuf[RdIdx].CanBus.WheelDir;
            pData->TransferWheelAngle       = TaskInfo.pDataBuf[RdIdx].CanBus.XferWheelAngle;
            pData->TransferWheelAngleSpeed  = TaskInfo.pDataBuf[RdIdx].CanBus.XferWheelAngleSpeed;
        } else {
            SvcLog_DBG(LOG_DATG, "Cannot find the data (%u)", RdIdx, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;
}
#endif

/**
* reset of file feed mode
* @return none
*/
void SvcDataGather_FileFeedReset(void)
{
    UINT32 Err;

    if (g_TaskExist == 0U) {
        Err = AmbaWrap_memset(DataBuffer, 0, sizeof(DataBuffer));
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_DATG, "AmbaWrap_memset failed(%u)", Err, 0U);
        }

        TaskInfo.pDataBuf  = &(DataBuffer[0U]);
        TaskInfo.DataCount = 0U;
        TaskInfo.DataWrIdx = 0U;
        TaskInfo.DataRdIdx = 0U;
    }
}

/**
* status of file feed mode
* @param [out] pStatus status
* @return none
*/
void SvcDataGather_FileFeedStaus(UINT32 *pStatus)
{
    UINT32 Status = 0U;

    if (g_TaskExist == 0U) {
        Status = SVC_DATA_GATHER_FILE_WRITABLE;

        if (TaskInfo.DataWrIdx > TaskInfo.DataRdIdx) {
            if (KEEP_NUM > TaskInfo.DataRdIdx) {
                if (((BUFFER_NUM + TaskInfo.DataRdIdx) - KEEP_NUM) <= TaskInfo.DataWrIdx) {
                    Status = 0U;
                }
            }
        }

        if (TaskInfo.DataWrIdx < TaskInfo.DataRdIdx) {
            if (KEEP_NUM >= TaskInfo.DataRdIdx) {
                Status = 0U;
            } else {
                if ((TaskInfo.DataRdIdx - KEEP_NUM) <= TaskInfo.DataWrIdx) {
                    Status = 0U;
                }
            }
        }
    }

    *pStatus = Status;
}

/**
* input of file feed mode
* @param [in] pData input data
* @return none
*/
void SvcDataGather_FileFeedData(const AMBA_DATG_s *pData)
{
    UINT32 Err;

    if (g_TaskExist == 0U) {
        Err = AmbaWrap_memcpy(&(TaskInfo.pDataBuf[TaskInfo.DataWrIdx]), pData, sizeof(AMBA_DATG_s));
        if (Err != SVC_OK) {
            SvcLog_NG(LOG_DATG, "AmbaWrap_memcpy failed", 0U, 0U);
        }

        TaskInfo.DataCount ++;
        TaskInfo.DataWrIdx ++;
        if (TaskInfo.DataWrIdx >= BUFFER_NUM) {
            TaskInfo.DataWrIdx -= BUFFER_NUM;
        }
    }
}
