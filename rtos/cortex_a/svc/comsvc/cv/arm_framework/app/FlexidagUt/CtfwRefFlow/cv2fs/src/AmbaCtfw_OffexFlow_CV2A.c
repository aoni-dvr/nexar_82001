#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "ArmErrCode.h"
#include "AmbaTypes.h"

#include "AmbaCtfw_OffexFlow_CV2A.h"
#include "Ctfw_TaskTemplate.h"
#include "Ctfw_SourceTask.h"
#include "Ctfw_SinkTask.h"
#include "Ctfw_OffexTask_CV2A.h"

#define FLOW_NAME "OffexFlow"

static CtfwTaskHandle_t gCtfwSourceTaskHandle;
static CtfwTaskHandle_t gCtfwOffexCv2aTaskHandle;
static CtfwTaskHandle_t gCtfwSinkTaskHandle;

static UINT32 gMemPoolId = 0U;

void AmbaCtfw_OffexCv2aProfilerReport(void)
{
    (void) Ctfw_ProfilerTimerReport(&gCtfwSourceTaskHandle, 0U);
    (void) Ctfw_ProfilerTimerReport(&gCtfwOffexCv2aTaskHandle, 0U);
    (void) Ctfw_ProfilerTimerReport(&gCtfwSinkTaskHandle, 0U);
}

UINT32 AmbaCtfw_OffexCv2aFeedOneFrame(const CtfwFrame_t *pFrame)
{
    UINT32 Rval = ARM_OK;

    if ( pFrame == NULL ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_OffexCv2aFeedOneFrame sanity check fail", 0U, 0U);
        Rval = ARM_NG;
    }

    if ( Rval == ARM_OK ) {
        CtfwCbMsg_t CbMsg;
        CtfwDesc_t *pDesc = &(CbMsg.Desc);

        pDesc->Frame = *pFrame;
        Rval |= Ctfw_MsgQueueSend(&gCtfwSourceTaskHandle, CTFW_TASK_CMD_UPCB, &CbMsg);

        if (Rval != ARM_OK) {
            ArmLog_ERR(FLOW_NAME, "Ctfw_MsgQueueSend Fail. Ret = %u", Rval, 0U);
        }
    }

    return Rval;
}

UINT32 AmbaCtfw_OffexCv2aProduceFrame(const AMBA_CV_IMG_BUF_s *pImage, UINT32 FrameIndex, CtfwFrame_t *pFrame)
{
    UINT32 Rval = ARM_OK;

    if (pImage == NULL) {
        ArmLog_ERR(FLOW_NAME, "## AmbaCtfw_OffexCv2aProduceFrame(), pImage == NULL", 0, 0);
        Rval = ARM_NG;
    } else {
        flexidag_memblk_t *pInputData = NULL;
        CtfwFrameMetadata_t *pOffexInMeta = NULL;

        /* give input data */
        (void) AmbaWrap_memcpy((void*)&pInputData, (void*)&pImage, sizeof(void*));
        pFrame->CtfwData = *pInputData;

        /* give inpute metadata */
        (void) AmbaWrap_memcpy((void*)&pOffexInMeta, (void*)&pFrame->MetaData.pBuffer, sizeof(void*));
        pOffexInMeta->FrameIndex = FrameIndex;
        Rval = (UINT32) AmbaKAL_GetSysTickCount(&pOffexInMeta->TimeStamp);
    }

    return Rval;
}

UINT32 AmbaCtfw_OffexCv2aInit(const AMBA_CV_SPUFEX_CFG_s *pSpufexCfg,
                              CtfwGetProcResult_t GetResultCb, flexidag_memblk_t FlexiBinBlk, UINT32 PoolFrameNumTobeAlloc, CtfwFrame_t FramePool[])
{
    UINT32 Rval = ARM_OK;
    static flexidag_memblk_t gTotalBuf = {NULL, 0U, 0U, 0U, 0U, 0U};
    UINT32 FramePoolSize = 0U;
    UINT32 TskReqSize = 0U;
    UINT32 CVMemSize = 0U;
    UINT32 InputDataSize = 0;
    UINT32 OutBuffDepth = CTFW_MAX_BUFFER_DEPTH;

    ArmLog_DBG(FLOW_NAME, "## Hello AmbaCtfw_OffexCv2aInit!!", 0U, 0U);

    if ( (pSpufexCfg == NULL) || (GetResultCb == NULL) || (gTotalBuf.pBuffer != NULL)) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_OffexCv2aInit sanity check fail", 0U, 0U);
        Rval = ARM_NG;
    }

    if ( PoolFrameNumTobeAlloc < CTFW_MAX_BUFFER_DEPTH ) {
        ArmLog_ERR(FLOW_NAME, "AmbaCtfw_OffexCv2aInit fail, PoolFrameNumTobeAlloc(%u) < CTFW_MAX_BUFFER_DEPTH(%u)",
                   PoolFrameNumTobeAlloc, CTFW_MAX_BUFFER_DEPTH);
        Rval = ARM_NG;
    }

    if (Rval == ARM_OK) {

        Rval |= Ctfw_TaskQuerySize(&gCtfwSourceTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwOffexCv2akObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        Rval |= Ctfw_TaskQuerySize(&gCtfwSinkTaskObj, OutBuffDepth, &TskReqSize);
        CVMemSize += TskReqSize;

        /* Input buffer pool */
        Rval |= Ctfw_GetSourceFramePoolSize(InputDataSize, PoolFrameNumTobeAlloc, &FramePoolSize);
        CVMemSize += FramePoolSize;

        Rval |= AmbaCV_UtilityCmaMemAlloc(CVMemSize, 1U, &gTotalBuf);
        if (NULL == gTotalBuf.pBuffer) {
            ArmLog_ERR(FLOW_NAME, "## Can't allocate %u from CmaMem", CVMemSize, 0U);
        }

        Rval |= ArmMemPool_Create(&gTotalBuf, &gMemPoolId);
        (void) AmbaWrap_memset(gTotalBuf.pBuffer, 0, gTotalBuf.buffer_size);
    }

    // Allocate input frame pool
    if (Rval == ARM_OK) {
        Rval |= Ctfw_AllocSourceFramePool(InputDataSize, gMemPoolId, FramePool, PoolFrameNumTobeAlloc);
    }

    // CTFW initialization
    if (Rval == ARM_OK) {
        Rval |= Ctfw_Init();
    }

    // Task initialization
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskInit(&gCtfwSourceTaskObj,  NULL,           NULL,       NULL,           FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSourceTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwOffexCv2akObj,  NULL,           pSpufexCfg, &FlexiBinBlk,   FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwOffexCv2aTaskHandle);
        Rval |= Ctfw_TaskInit(&gCtfwSinkTaskObj,    GetResultCb,    NULL,       NULL,           FLOW_NAME, gMemPoolId, OutBuffDepth, &gCtfwSinkTaskHandle);
    }

    if (Rval == ARM_OK) {
        Rval |= AmbaCtfw_OffexCv2aStart();
    }

    // Connect task
    if (Rval == ARM_OK) {
        Rval |= Ctfw_TaskConnect_TF(&gCtfwSourceTaskHandle, &gCtfwOffexCv2aTaskHandle);
        (void) AmbaKAL_TaskSleep(100U);
        Rval |= Ctfw_TaskConnect_TF(&gCtfwOffexCv2aTaskHandle, &gCtfwSinkTaskHandle);
        (void) AmbaKAL_TaskSleep(100U);
    }

    // Debug: check connection
    if (Rval == ARM_OK) {
        (void) Ctfw_TaskCheckConnection(&gCtfwSourceTaskHandle);
        (void) Ctfw_TaskCheckConnection(&gCtfwOffexCv2aTaskHandle);
        (void) Ctfw_TaskCheckConnection(&gCtfwSinkTaskHandle);
    }

    return Rval;
}

UINT32 AmbaCtfw_OffexCv2aStop(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskStop(&gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwOffexCv2aTaskHandle);
    Rval |= Ctfw_TaskStop(&gCtfwSinkTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_OffexCv2aDeInit(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwOffexCv2aTaskHandle);
    Rval |= Ctfw_TaskDeinit(gMemPoolId, &gCtfwSinkTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_OffexCv2aStart(void)
{
    /* ready to proc state */
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_TaskStart(&gCtfwSourceTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwOffexCv2aTaskHandle);
    Rval |= Ctfw_TaskStart(&gCtfwSinkTaskHandle);

    return Rval;
}

UINT32 AmbaCtfw_OffexCv2aRestart(void)
{
    UINT32 Rval = ARM_OK;

    Rval |= AmbaCtfw_OffexCv2aStop();
    Rval |= AmbaCtfw_OffexCv2aStart();

    return Rval;
}

