#include "AmbaCtfw_MvacStixelFlow.h"
#include "AmbaMisraFix.h"

#include "ArmErrCode.h"
#include "AmbaTypes.h"

#include "Ctfw_TaskTemplate.h"

#define FLOW_NAME "MvacStixel"
#define CT_INIT_NO_ALGO_CONFIG           NULL
#define CT_INIT_NO_GET_PROC_CALL_BACK    NULL
#define CT_INIT_NO_FLEXI_BIN             NULL

static CtfwTaskHandle_t gCtfwSourceTaskHandle;
static CtfwTaskHandle_t gCtfwMvacSinkTaskHandle;
static CtfwTaskHandle_t gCtfwStixelSinkTaskHandle;
static CtfwTaskHandle_t gCtfwSpuFexTaskHandle;
static CtfwTaskHandle_t gCtfwFmaTaskHandle;
static CtfwTaskHandle_t gCtfwSpuFusionTaskHandle;
static CtfwTaskHandle_t gCtfwStixelTaskHandle;
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

void AmbaCtfw_MvacStixelProfilerReport(void)
{
    UINT32 Rval = ARM_OK;
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSourceTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwMvacSinkTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwStixelSinkTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSpuFexTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwFmaTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSpuFusionTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwStixelTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwMvacVoTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwMvacAcTaskHandle, 0U);
    (void) Rval;
}

UINT32 AmbaCtfw_MvacStixelProduceFrame(const flexidag_memblk_t *pPicInfo, const CtfwCalibInfo_t *pCalibInfo,
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

UINT32 AmbaCtfw_MvacStixelStop(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskStop(&gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwMvacSinkTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwStixelSinkTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwSpuFexTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwFmaTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwSpuFusionTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwStixelTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwMvacVoTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwMvacAcTaskHandle);

    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwMvacAcTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwMvacVoTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwStixelTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSpuFusionTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwFmaTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSpuFexTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwStixelSinkTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwMvacSinkTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSourceTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_MvacStixelStart(const CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig,
                                const CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig,
                                const CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig,
                                const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig,
                                const CTFW_SPU_FUSION_TASK_CONFIG_t *pSpuFusionTaskConfig,
                                const CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
                                CtfwGetProcResult_t GetMvacResultCb, CtfwGetProcResult_t GetStixelResultCb,
                                flexidag_memblk_t StixelFlexiBinBlk[])
{
    UINT32 Rval = ARM_OK;
    UINT32 OutBuffDepth = CTFW_MAX_BUFFER_DEPTH;

    Rval |= Ctfw_TaskInit(&gCtfwSourceTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskInit(&gCtfwSinkTaskObj,        GetMvacResultCb,                CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwMvacSinkTaskHandle);
    Rval |= Ctfw_TaskInit(&gCtfwSinkTaskObj,        GetStixelResultCb,              CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwStixelSinkTaskHandle);
    Rval |= Ctfw_TaskInit(&gCtfwSpuFexTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  pSpuFexTaskConfig,      CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSpuFexTaskHandle);
    Rval |= Ctfw_TaskInit(&gCtfwFmaTaskObj,         CT_INIT_NO_GET_PROC_CALL_BACK,  pFmaTaskConfig,         CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwFmaTaskHandle);
    Rval |= Ctfw_TaskInit(&gCtfwSpuFusionTaskObj,   CT_INIT_NO_GET_PROC_CALL_BACK,  pSpuFusionTaskConfig,   CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSpuFusionTaskHandle);
    Rval |= Ctfw_TaskInit(&gCtfwStixelTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  pStixelTaskConfig,      StixelFlexiBinBlk,      FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwStixelTaskHandle);
    Rval |= Ctfw_TaskInit(&gCtfwMvacVoTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  pVoTaskConfig,          CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwMvacVoTaskHandle);
    Rval |= Ctfw_TaskInit(&gCtfwMvacAcTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  pAcTaskConfig,          CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwMvacAcTaskHandle);

    Rval |= Ctfw_TaskStart(&gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwMvacSinkTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwStixelSinkTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwSpuFexTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwFmaTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwSpuFusionTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwStixelTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwMvacVoTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwMvacAcTaskHandle);

    // Connect task
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceTaskHandle, &gCtfwSpuFexTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFexTaskHandle, &gCtfwFmaTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFexTaskHandle, &gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwFmaTaskHandle, &gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFexTaskHandle, &gCtfwMvacAcTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwFmaTaskHandle, &gCtfwMvacAcTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwMvacVoTaskHandle, &gCtfwMvacAcTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwMvacAcTaskHandle, &gCtfwMvacSinkTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFexTaskHandle, &gCtfwSpuFusionTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceTaskHandle, &gCtfwSpuFusionTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFusionTaskHandle, &gCtfwStixelTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwStixelTaskHandle, &gCtfwStixelSinkTaskHandle);
    }

    // Debug: check connection
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSourceTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwMvacSinkTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwStixelSinkTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSpuFexTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwFmaTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSpuFusionTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwStixelTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwMvacAcTaskHandle);
    }

    return Rval;
}


UINT32 AmbaCtfw_MvacStixelInit(const CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig,
                               const CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig,
                               const CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig,
                               const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig,
                               const CTFW_SPU_FUSION_TASK_CONFIG_t *pSpuFusionTaskConfig,
                               const CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
                               CtfwGetProcResult_t GetMvacResultCb, CtfwGetProcResult_t GetStixelResultCb,
                               flexidag_memblk_t StixelFlexiBinBlk[],
                               UINT32 PoolFrameNumTobeAlloc, CtfwFrame_t FramePool[])
{
    UINT32 Rval = ARM_OK;
    static flexidag_memblk_t gTotalBuf;
    UINT32 FramePoolSize = 0U;
    UINT32 TskReqSize = 0U;
    UINT32 CVMemSize = 0U;
    UINT32 InputDataSize = 0U;
    UINT32 OutBuffDepth = CTFW_MAX_BUFFER_DEPTH;

    ArmLog_DBG(FLOW_NAME, "## Hello AmbaCtfw_MvacStixelInit!!", 0U, 0U);

    Rval |= CheckNullPtr(pSpuFexTaskConfig);
    Rval |= CheckNullPtr(pFmaTaskConfig);
    Rval |= CheckNullPtr(pVoTaskConfig);
    Rval |= CheckNullPtr(pAcTaskConfig);
    Rval |= CheckNullPtr(pSpuFusionTaskConfig);
    Rval |= CheckNullPtr(pStixelTaskConfig);

    if ( Rval != ARM_OK ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_MvacStixelInit sanity check fail", 0U, 0U);
    }

    if ( PoolFrameNumTobeAlloc < CTFW_MAX_BUFFER_DEPTH ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_MvacStixelInit fail, PoolFrameNumTobeAlloc(%u) < CTFW_MAX_BUFFER_DEPTH(%u)",
                   PoolFrameNumTobeAlloc, CTFW_MAX_BUFFER_DEPTH);
        Rval = ARM_NG;
    }

    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskQuerySize(&gCtfwSourceTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwSinkTaskObj, OutBuffDepth, &TskReqSize);
        // 2 sink nodes
        CVMemSize += (TskReqSize*2U);

        Rval |= Ctfw_TaskQuerySize(&gCtfwSpuFexTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwFmaTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwSpuFusionTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwStixelTaskObj, OutBuffDepth, &TskReqSize);
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
        Rval |= Ctfw_TaskInit(&gCtfwSourceTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSourceTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwSinkTaskObj,        GetMvacResultCb,                CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwMvacSinkTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwSinkTaskObj,        GetStixelResultCb,              CT_INIT_NO_ALGO_CONFIG, CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwStixelSinkTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwSpuFexTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  pSpuFexTaskConfig,      CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSpuFexTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwFmaTaskObj,         CT_INIT_NO_GET_PROC_CALL_BACK,  pFmaTaskConfig,         CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwFmaTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwSpuFusionTaskObj,   CT_INIT_NO_GET_PROC_CALL_BACK,  pSpuFusionTaskConfig,   CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSpuFusionTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwStixelTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  pStixelTaskConfig,      StixelFlexiBinBlk,      FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwStixelTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwMvacVoTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  pVoTaskConfig,          CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwMvacAcTaskObj,      CT_INIT_NO_GET_PROC_CALL_BACK,  pAcTaskConfig,          CT_INIT_NO_FLEXI_BIN,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwMvacAcTaskHandle);
    }

    Rval |= Ctfw_TaskStart(&gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwMvacSinkTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwStixelSinkTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwSpuFexTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwFmaTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwSpuFusionTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwStixelTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwMvacVoTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwMvacAcTaskHandle);

    // Connect task
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceTaskHandle, &gCtfwSpuFexTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFexTaskHandle, &gCtfwFmaTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFexTaskHandle, &gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwFmaTaskHandle, &gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFexTaskHandle, &gCtfwMvacAcTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwFmaTaskHandle, &gCtfwMvacAcTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwMvacVoTaskHandle, &gCtfwMvacAcTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwMvacAcTaskHandle, &gCtfwMvacSinkTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFexTaskHandle, &gCtfwSpuFusionTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceTaskHandle, &gCtfwSpuFusionTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSpuFusionTaskHandle, &gCtfwStixelTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwStixelTaskHandle, &gCtfwStixelSinkTaskHandle);
    }

    // Debug: check connection
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSourceTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwMvacSinkTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwStixelSinkTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSpuFexTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwFmaTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSpuFusionTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwStixelTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwMvacVoTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwMvacAcTaskHandle);
    }

    return Rval;
}

UINT32 AmbaCtfw_MvacStixelFeedOneFrame(const CtfwFrame_t *pFrame)
{
    UINT32 Rval = ARM_OK;

    if ( pFrame == NULL ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_MvacStixelFeedOneFrame sanity check fail", 0U, 0U);
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

UINT32 AmbaCtfw_MvacStixelRestart(const CTFW_SPU_FEX_TASK_CONFIG_t *pSpuFexTaskConfig,
                                  const CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig,
                                  const CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig,
                                  const CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig,
                                  const CTFW_SPU_FUSION_TASK_CONFIG_t *pSpuFusionTaskConfig,
                                  const CTFW_STIXEL_TASK_CONFIG_t *pStixelTaskConfig,
                                  CtfwGetProcResult_t GetMvacResultCb, CtfwGetProcResult_t GetStixelResultCb,
                                  flexidag_memblk_t StixelFlexiBinBlk[])
{
    UINT32 Rval = ARM_OK;

    Rval |= AmbaCtfw_MvacStixelStop();
    Rval |= AmbaCtfw_MvacStixelStart(pSpuFexTaskConfig,
                                     pFmaTaskConfig, pVoTaskConfig, pAcTaskConfig,
                                     pSpuFusionTaskConfig, pStixelTaskConfig,
                                     GetMvacResultCb, GetStixelResultCb, StixelFlexiBinBlk);

    return Rval;
}

