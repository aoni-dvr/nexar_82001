#include "AmbaCtfw_StixelFlow.h"
#include "AmbaMisraFix.h"

#include "ArmErrCode.h"
#include "AmbaTypes.h"

#include "Ctfw_TaskTemplate.h"

#include "Ctfw_SourceTask.h"
#include "Ctfw_SinkTask.h"
#include "Ctfw_StixelTask.h"

#define FLOW_NAME "Stixel"
#define CT_INIT_NO_ALGO_CONFIG           NULL
#define CT_INIT_NO_GET_PROC_CALL_BACK    NULL
#define CT_INIT_NO_FLEXI_BIN             NULL

static CtfwTaskHandle_t gCtfwSourceTaskHandle;
static CtfwTaskHandle_t gCtfwSinkTaskHandle;
static CtfwTaskHandle_t gCtfwStixelTaskHandle;

static UINT32 gMemPoolId = 0U;

static UINT32 CheckNullPtr(const void *ptr)
{
    UINT32 Rval = ARM_OK;
    if (ptr == NULL) {
        Rval = ARM_NG;
    }
    return Rval;
}

void AmbaCtfw_StixelProfilerReport(void)
{
    UINT32 Rval = ARM_OK;
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSourceTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwStixelTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSinkTaskHandle, 0U);
    (void) Rval;
}

UINT32 AmbaCtfw_StixelProduceFrame(const flexidag_memblk_t *pSpuOut,
                                   UINT32 FrameIndex, CtfwFrame_t *pFrame)
{
    UINT32 Rval = ARM_OK;
    CtfwFrameMetadata_t *pStixelInputMetadata;

    if (pSpuOut == NULL) {
        Rval = ARM_NG;
    } else {
        pFrame->CtfwData = *pSpuOut;
        AmbaMisra_TypeCast(&pStixelInputMetadata, &pFrame->MetaData.pBuffer);

        pStixelInputMetadata->FrameIndex = FrameIndex;
        Rval |= AmbaKAL_GetSysTickCount(&pStixelInputMetadata->TimeStamp);
    }

    return Rval;
}

UINT32 AmbaCtfw_StixelInit(const CTFW_STIXEL_TASK_CONFIG_t *pStixelCfg,
                           CtfwGetProcResult_t GetResultCb, flexidag_memblk_t StixelFlexiBinBlk[], UINT32 PoolFrameNumTobeAlloc, CtfwFrame_t FramePool[])
{
    UINT32 Rval = ARM_OK;
    static flexidag_memblk_t gTotalBuf;
    UINT32 FramePoolSize = 0U;
    UINT32 TskReqSize = 0U;
    UINT32 CVMemSize = 0U;
    UINT32 InputDataSize = 0U;
    UINT32 OutBuffDepth = CTFW_MAX_BUFFER_DEPTH;

    ArmLog_DBG(FLOW_NAME, "## Hello AmbaCtfw_StixelInit!!", 0U, 0U);

    Rval |= CheckNullPtr(pStixelCfg);

    if ( Rval != ARM_OK ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_MvacStixelInit sanity check fail", 0U, 0U);
    }

    if ( PoolFrameNumTobeAlloc < CTFW_MAX_BUFFER_DEPTH ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_StixelInit fail, PoolFrameNumTobeAlloc(%u) < CTFW_MAX_BUFFER_DEPTH(%u)",
                   PoolFrameNumTobeAlloc, CTFW_MAX_BUFFER_DEPTH);
        Rval = ARM_NG;
    }

    if (Rval == ARM_OK) {

        Rval |= Ctfw_TaskQuerySize(&gCtfwSourceTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwStixelTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwSinkTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_GetSourceFramePoolSize(InputDataSize, PoolFrameNumTobeAlloc, &FramePoolSize);
        CVMemSize += FramePoolSize;

        Rval |= AmbaCV_UtilityCmaMemAlloc(CVMemSize, 1U, &gTotalBuf);
        if (Rval != 0U) {
            ArmLog_ERR(FLOW_NAME, "## Can't allocate %u from CmaMem", CVMemSize, 0U);
        } else {
            Rval |= ArmMemPool_Create(&gTotalBuf, &gMemPoolId);
            Rval |= AmbaWrap_memset(gTotalBuf.pBuffer, 0, gTotalBuf.buffer_size);
        }
    }

    // Allocate input frame pool
    if (Rval == ARM_OK) {
        Rval = Ctfw_AllocSourceFramePool(InputDataSize, gMemPoolId, FramePool, PoolFrameNumTobeAlloc);
    }

    // CTFW initialization
    if (Rval == ARM_OK) {
        Rval |= Ctfw_Init();
    }

    // Task initialization
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskInit(&gCtfwSourceTaskObj,          CT_INIT_NO_GET_PROC_CALL_BACK,  CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN, FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSourceTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwStixelTaskObj,          CT_INIT_NO_GET_PROC_CALL_BACK,  pStixelCfg,             StixelFlexiBinBlk,    FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwStixelTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwSinkTaskObj,            GetResultCb,                    CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN, FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSinkTaskHandle);
    }

    Rval |= AmbaCtfw_StixelStart();

    // Connect task
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceTaskHandle, &gCtfwStixelTaskHandle);
        Rval |= AmbaKAL_TaskSleep(100);

        Rval |= Ctfw_TaskConnect_TF(&gCtfwStixelTaskHandle, &gCtfwSinkTaskHandle);
        Rval |= AmbaKAL_TaskSleep(100);
    }

    // Debug: check connection
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSourceTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwStixelTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSinkTaskHandle);
    }

    return Rval;

}

UINT32 AmbaCtfw_StixelFeedOneFrame(const CtfwFrame_t *pFrame)
{
    UINT32 Rval = ARM_OK;

    if ( pFrame == NULL ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_StixelFeedOneFrame sanity check fail", 0U, 0U);
        Rval = ARM_NG;
    }

    if ( Rval == ARM_OK ) {
        CtfwCbMsg_t CbMsg;
        CtfwDesc_t *pDesc = &(CbMsg.Desc);

        pDesc->Frame = *pFrame;

        Rval = Ctfw_MsgQueueSend(&gCtfwSourceTaskHandle, CTFW_TASK_CMD_UPCB, &CbMsg);

        if (Rval != ARM_OK) {
            ArmLog_ERR(FLOW_NAME, "Ctfw_MsgQueueSend Fail. Ret = %u", Rval, 0U);
        }
    }

    return Rval;
}

UINT32 AmbaCtfw_StixelStop(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskStop(&gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwStixelTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwSinkTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_StixelDeInit(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwStixelTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSinkTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_StixelStart(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskStart(&gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwStixelTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwSinkTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_StixelRestart(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= AmbaCtfw_StixelStop();
    Rval |= AmbaCtfw_StixelStart();

    return Rval;
}

