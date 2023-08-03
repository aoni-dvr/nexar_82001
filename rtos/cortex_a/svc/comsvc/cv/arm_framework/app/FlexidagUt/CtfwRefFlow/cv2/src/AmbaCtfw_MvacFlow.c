#include "AmbaCtfw_MvacFlow.h"
#include "AmbaMisraFix.h"

#include "ArmErrCode.h"
#include "AmbaTypes.h"

#include "Ctfw_TaskTemplate.h"

#define FLOW_NAME "Mvac"
#define CT_INIT_NO_ALGO_CONFIG           NULL
#define CT_INIT_NO_GET_PROC_CALL_BACK    NULL
#define CT_INIT_NO_FLEXI_BIN             NULL

static CtfwTaskHandle_t gCtfwSourceTaskHandle;
static CtfwTaskHandle_t gCtfwSinkTaskHandle;
static CtfwTaskHandle_t gCtfwFexTaskHandle;
static CtfwTaskHandle_t gCtfwFmaTaskHandle;
static CtfwTaskHandle_t gCtfwMvacVoTaskHandle;
static CtfwTaskHandle_t gCtfwMvacAcTaskHandle;

static UINT32 gMemPoolId = 0U;

static UINT32 CheckNullPtr(const void *ptr)
{
    UINT32 Rval = ARM_OK;
    if (ptr == NULL) {
        Rval = ARM_NG;
    }
    return Rval;
}

void AmbaCtfw_MvacProfilerReport(void)
{
    UINT32 Rval = ARM_OK;
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSourceTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSinkTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwFexTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwFmaTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwMvacVoTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwMvacAcTaskHandle, 0U);
    (void) Rval;
}

UINT32 AmbaCtfw_MvacProduceFrame(const flexidag_memblk_t *pPicInfo, const CtfwCalibInfo_t *pCalibInfo,
                                 UINT32 FrameIndex, CtfwFrame_t *pFrame)
{
    UINT32 Rval = ARM_OK;

    CtfwFrameMetadata_t *pMvacInputMetadata;

    pFrame->CtfwData = *pPicInfo;
    AmbaMisra_TypeCast(&pMvacInputMetadata, &pFrame->MetaData.pBuffer);
    pMvacInputMetadata->CalibInfo = *pCalibInfo;
    pMvacInputMetadata->FrameIndex = FrameIndex;
    Rval |= AmbaKAL_GetSysTickCount(&pMvacInputMetadata->TimeStamp);

    return Rval;
}

UINT32 AmbaCtfw_MvacInit(const CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig,
                         const CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig,
                         const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig,
                         CtfwGetProcResult_t GetMvacResultCb,
                         UINT32 PoolFrameNumTobeAlloc,
                         CtfwFrame_t FramePool[])
{
    UINT32 Rval = ARM_OK;
    static flexidag_memblk_t gTotalBuf;
    UINT32 FramePoolSize = 0U;
    UINT32 TskReqSize = 0U;
    UINT32 CVMemSize = 0U;
    UINT32 InputDataSize = 0U;
    UINT32 OutBuffDepth = CTFW_MAX_BUFFER_DEPTH;

    ArmLog_DBG(FLOW_NAME, "## Hello AmbaCtfw_MvacInit!!", 0U, 0U);

    Rval |= CheckNullPtr(pFmaTaskConfig);
    Rval |= CheckNullPtr(pVoTaskConfig);
    Rval |= CheckNullPtr(pAcTaskConfig);

    if ( Rval != ARM_OK ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_MvacStixelInit sanity check fail", 0U, 0U);
    }

    if ( PoolFrameNumTobeAlloc < CTFW_MAX_BUFFER_DEPTH ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_MvacInit fail, PoolFrameNumTobeAlloc(%u) < CTFW_MAX_BUFFER_DEPTH(%u)",
                   PoolFrameNumTobeAlloc, CTFW_MAX_BUFFER_DEPTH);
        Rval = ARM_NG;
    }

    if (Rval == ARM_OK) {

        Rval |= Ctfw_TaskQuerySize(&gCtfwSourceTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwSinkTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwFexTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwFmaTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwMvacVoTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwMvacAcTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_GetSourceFramePoolSize(InputDataSize, PoolFrameNumTobeAlloc, &FramePoolSize);
        CVMemSize += FramePoolSize;

        //ArmLog_ERR(FLOW_NAME, "## CVMemSize = %d", CVMemSize, 0U);

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
        Rval |= Ctfw_TaskInit(&gCtfwSourceTaskObj,  CT_INIT_NO_GET_PROC_CALL_BACK,  CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSourceTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwSinkTaskObj,    GetMvacResultCb,                CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSinkTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwFexTaskObj,     CT_INIT_NO_GET_PROC_CALL_BACK,  CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwFexTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwFmaTaskObj,     CT_INIT_NO_GET_PROC_CALL_BACK,  pFmaTaskConfig,         CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwFmaTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwMvacVoTaskObj,  CT_INIT_NO_GET_PROC_CALL_BACK,  pVoTaskConfig,          CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwMvacAcTaskObj,  CT_INIT_NO_GET_PROC_CALL_BACK,  pAcTaskConfig,          CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwMvacAcTaskHandle);
    }

    Rval |= Ctfw_TaskStart(&gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwSinkTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwFexTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwFmaTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwMvacVoTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwMvacAcTaskHandle);

    // Connect task
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceTaskHandle, &gCtfwFexTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwFexTaskHandle, &gCtfwFmaTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwFexTaskHandle, &gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwFmaTaskHandle, &gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwFexTaskHandle, &gCtfwMvacAcTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwFmaTaskHandle, &gCtfwMvacAcTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwMvacVoTaskHandle, &gCtfwMvacAcTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwMvacAcTaskHandle, &gCtfwSinkTaskHandle);
    }

    // Debug: check connection
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSourceTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSinkTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwFexTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwFmaTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwMvacAcTaskHandle);
    }

    return Rval;
}

UINT32 AmbaCtfw_MvacFeedOneFrame(const CtfwFrame_t *pFrame)
{
    UINT32 Rval = ARM_OK;

    if ( pFrame == NULL ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_MvacFeedOneFrame sanity check fail", 0U, 0U);
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

UINT32 AmbaCtfw_MvacStop(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskDisconnect_TF(&gCtfwFexTaskHandle, &gCtfwMvacAcTaskHandle);
    Rval |= Ctfw_TaskDisconnect_TF(&gCtfwFmaTaskHandle, &gCtfwMvacAcTaskHandle);
    Rval |= Ctfw_TaskDisconnect_TF(&gCtfwMvacVoTaskHandle, &gCtfwMvacAcTaskHandle);
    Rval |= Ctfw_TaskDisconnect_TF(&gCtfwMvacAcTaskHandle, &gCtfwSinkTaskHandle);

    Rval |= Ctfw_TaskStop(&gCtfwMvacAcTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwMvacAcTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_MvacStart(const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig)
{
    UINT32 Rval = ARM_OK;
    UINT32 OutBuffDepth = gCtfwMvacAcTaskHandle.OutBuffDepth;

    Rval |= Ctfw_TaskInit(&gCtfwMvacAcTaskObj,  CT_INIT_NO_GET_PROC_CALL_BACK,  &pAcTaskConfig,          CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwMvacAcTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwMvacAcTaskHandle);

    Rval |= Ctfw_TaskConnect_TF(&gCtfwFexTaskHandle, &gCtfwMvacAcTaskHandle);
    Rval |= Ctfw_TaskConnect_TF(&gCtfwFmaTaskHandle, &gCtfwMvacAcTaskHandle);
    Rval |= Ctfw_TaskConnect_TF(&gCtfwMvacVoTaskHandle, &gCtfwMvacAcTaskHandle);
    Rval |= Ctfw_TaskConnect_TF(&gCtfwMvacAcTaskHandle, &gCtfwSinkTaskHandle);

    return Rval;

}

UINT32 AmbaCtfw_MvacRestart(const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig)
{
    UINT32 Rval = ARM_OK;

    Rval |= AmbaCtfw_MvacStop();
    Rval |= AmbaCtfw_MvacStart(pAcTaskConfig);

    return Rval;
}

