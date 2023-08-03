#include "AmbaCtfw_StixelV2Flow.h"
#include "AmbaMisraFix.h"

#include "ArmErrCode.h"
#include "AmbaTypes.h"

#include "Ctfw_TaskTemplate.h"

#include "Ctfw_SourceTask.h"
#include "Ctfw_SinkTask.h"
#include "Ctfw_StixelCamPoseTask.h"
#include "Ctfw_StixelGenStixelTask.h"

#define FLOW_NAME "StixelV2"
#define CT_INIT_NO_ALGO_CONFIG           NULL
#define CT_INIT_NO_GET_PROC_CALL_BACK    NULL
#define CT_INIT_NO_FLEXI_BIN             NULL

static CtfwTaskHandle_t gCtfwSourceSpuTaskHandle;
static CtfwTaskHandle_t gCtfwSourceImgTaskHandle;
static CtfwTaskHandle_t gCtfwSinkTaskHandle;
static CtfwTaskHandle_t gCtfwCamPoseTaskHandle;
static CtfwTaskHandle_t gCtfwGenStixelTaskHandle;

static UINT32 gMemPoolId = 0U;

static UINT32 CheckNullPtr(const void *ptr)
{
    UINT32 Rval = ARM_OK;
    if (ptr == NULL) {
        Rval = ARM_NG;
    }
    return Rval;
}

void AmbaCtfw_StixelV2ProfilerReport(void)
{
    UINT32 Rval = ARM_OK;
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSourceSpuTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSourceImgTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwCamPoseTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwGenStixelTaskHandle, 0U);
    Rval |= Ctfw_ProfilerTimerReport(&gCtfwSinkTaskHandle, 0U);
    (void) Rval;
}

static UINT32 StixelV2ProduceFrame(const flexidag_memblk_t *pSpuOut,
                                   UINT32 FrameIndex, CtfwFrame_t *pFrame)
{
    UINT32 Rval = ARM_OK;
    CtfwFrameMetadata_t *pMetadata;

    if (pSpuOut == NULL) {
        Rval = ARM_NG;
    } else {
        pFrame->CtfwData = *pSpuOut;
        AmbaMisra_TypeCast(&pMetadata, &pFrame->MetaData.pBuffer);

        pMetadata->FrameIndex = FrameIndex;
        Rval |= AmbaKAL_GetSysTickCount(&pMetadata->TimeStamp);
    }

    return Rval;
}

UINT32 AmbaCtfw_StixelV2ProduceImgFrame(const flexidag_memblk_t *pPicInfo,
                                        UINT32 FrameIndex, CtfwFrame_t *pFrame)
{
    return StixelV2ProduceFrame(pPicInfo, FrameIndex, pFrame);
}

UINT32 AmbaCtfw_StixelV2ProduceSpuFrame(const flexidag_memblk_t *pSpuOut,
                                        UINT32 FrameIndex, CtfwFrame_t *pFrame)
{
    return StixelV2ProduceFrame(pSpuOut, FrameIndex, pFrame);
}

UINT32 AmbaCtfw_StixelV2Init(const CTFW_STIXEL_CAM_POSE_TASK_CONFIG_t *pStixelv2CamPoseTaskConfig,
                             const CTFW_STIXEL_GEN_STIXEL_TASK_CONFIG_t *pStixelv2GenStixelTaskConfig,
                             CtfwGetProcResult_t GetStixelResultCb,
                             flexidag_memblk_t StixelFlexiBinBlk[], UINT32 PoolFrameNumTobeAlloc,
                             CtfwFrame_t SpuFramePool[], CtfwFrame_t ImgFramePool[])
{
    UINT32 Rval = ARM_OK;
    static flexidag_memblk_t gTotalBuf;
    UINT32 FramePoolSize = 0U;
    UINT32 TskReqSize = 0U;
    UINT32 CVMemSize = 0U;
    UINT32 InputDataSize = 0U;
    UINT32 OutBuffDepth = CTFW_MAX_BUFFER_DEPTH;

    ArmLog_DBG(FLOW_NAME, "## Hello AmbaCtfw_StixelV2Init!!", 0U, 0U);

    Rval |= CheckNullPtr(pStixelv2CamPoseTaskConfig);
    Rval |= CheckNullPtr(pStixelv2GenStixelTaskConfig);

    if ( Rval != ARM_OK ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_MvacStixelInit sanity check fail", 0U, 0U);
    }

    if ( PoolFrameNumTobeAlloc < CTFW_MAX_BUFFER_DEPTH ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_StixelV2Init fail, PoolFrameNumTobeAlloc(%u) < CTFW_MAX_BUFFER_DEPTH(%u)",
                   PoolFrameNumTobeAlloc, CTFW_MAX_BUFFER_DEPTH);
        Rval = ARM_NG;
    }

    if (Rval == ARM_OK) {

        Rval |= Ctfw_TaskQuerySize(&gCtfwSourceTaskObj, OutBuffDepth, &TskReqSize);
        // 2 source nodes
        CVMemSize += (TskReqSize*2U);

        Rval |= Ctfw_TaskQuerySize(&gCtfwStixelCamPoseTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwStixelGenStixelTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwSinkTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_GetSourceFramePoolSize(InputDataSize, PoolFrameNumTobeAlloc, &FramePoolSize);
        // 2 Frame pools
        CVMemSize += (FramePoolSize*2U);

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
        Rval = Ctfw_AllocSourceFramePool(InputDataSize, gMemPoolId, SpuFramePool, PoolFrameNumTobeAlloc);
        Rval = Ctfw_AllocSourceFramePool(InputDataSize, gMemPoolId, ImgFramePool, PoolFrameNumTobeAlloc);
    }

    // CTFW initialization
    if (Rval == ARM_OK) {
        Rval |= Ctfw_Init();
    }

    // Task initialization
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskInit(&gCtfwSourceTaskObj,          CT_INIT_NO_GET_PROC_CALL_BACK,  CT_INIT_NO_ALGO_CONFIG,         CT_INIT_NO_FLEXI_BIN, FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSourceSpuTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwSourceTaskObj,          CT_INIT_NO_GET_PROC_CALL_BACK,  CT_INIT_NO_ALGO_CONFIG,         CT_INIT_NO_FLEXI_BIN, FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSourceImgTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwStixelCamPoseTaskObj,   CT_INIT_NO_GET_PROC_CALL_BACK,  pStixelv2CamPoseTaskConfig,     CT_INIT_NO_FLEXI_BIN, FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwCamPoseTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwStixelGenStixelTaskObj, CT_INIT_NO_GET_PROC_CALL_BACK,  pStixelv2GenStixelTaskConfig,   StixelFlexiBinBlk,    FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwGenStixelTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwSinkTaskObj,            GetStixelResultCb,              CT_INIT_NO_ALGO_CONFIG,         CT_INIT_NO_FLEXI_BIN, FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSinkTaskHandle);
    }

    Rval |= AmbaCtfw_StixelV2Start();

    // Connect task
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceSpuTaskHandle, &gCtfwGenStixelTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceSpuTaskHandle, &gCtfwCamPoseTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwCamPoseTaskHandle, &gCtfwGenStixelTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceImgTaskHandle, &gCtfwGenStixelTaskHandle);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwGenStixelTaskHandle, &gCtfwSinkTaskHandle);
    }

    // Debug: check connection
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSourceSpuTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSourceImgTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwCamPoseTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwGenStixelTaskHandle);
        Rval |= Ctfw_TaskCheckConnection(&gCtfwSinkTaskHandle);
    }

    return Rval;

}

static UINT32 StixelV2FeedOneFrame(CtfwTaskHandle_t *pSourceTaskHandle,
                                   const CtfwFrame_t *pFrame)
{
    UINT32 Rval = ARM_OK;

    if ( pFrame == NULL ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_StixelV2FeedOneFrame sanity check fail", 0U, 0U);
        Rval = ARM_NG;
    }

    if ( Rval == ARM_OK ) {
        CtfwCbMsg_t CbMsg;
        CtfwDesc_t *pDesc = &(CbMsg.Desc);

        pDesc->Frame = *pFrame;

        Rval = Ctfw_MsgQueueSend(pSourceTaskHandle, CTFW_TASK_CMD_UPCB, &CbMsg);

        if (Rval != ARM_OK) {
            ArmLog_ERR(FLOW_NAME, "Ctfw_MsgQueueSend Fail. Ret = %u", Rval, 0U);
        }
    }

    return Rval;
}

UINT32 AmbaCtfw_StixelV2FeedOneSpuFrame(const CtfwFrame_t *pFrame)
{
    return StixelV2FeedOneFrame(&gCtfwSourceSpuTaskHandle, pFrame);
}

UINT32 AmbaCtfw_StixelV2FeedOneImgFrame(const CtfwFrame_t *pFrame)
{
    return StixelV2FeedOneFrame(&gCtfwSourceImgTaskHandle, pFrame);
}

UINT32 AmbaCtfw_StixelV2Stop(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskStop(&gCtfwSourceSpuTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwSourceImgTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwCamPoseTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwGenStixelTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwSinkTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_StixelV2DeInit(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSourceSpuTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSourceImgTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwCamPoseTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwGenStixelTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSinkTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_StixelV2Start(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskStart(&gCtfwSourceSpuTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwSourceImgTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwCamPoseTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwGenStixelTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwSinkTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_StixelV2Restart(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= AmbaCtfw_StixelV2Stop();
    Rval |= AmbaCtfw_StixelV2Start();

    return Rval;
}

