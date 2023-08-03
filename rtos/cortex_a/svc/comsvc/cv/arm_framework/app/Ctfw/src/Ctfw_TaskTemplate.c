#include "Ctfw_TaskTemplate.h"
#include "AmbaMisraFix.h"

#define PROFILE_INSTANCE_ID (0U)

static CtfwRegDesk_t gRegTaskDesk;

// Upstream task send message to this task
static void UpStreamCallback(const CtfwCbMsg_t *pMsg)
{
    UINT32 RetVal = ARM_OK;
    RetVal |= Ctfw_MsgQueueSend(pMsg->Desc.pToHandle, CTFW_TASK_CMD_UPCB, pMsg);
    if ( RetVal != ARM_OK ) {
        ArmLog_ERR(pMsg->Desc.pFromHandle->CtfwResourceName.Task,
                   "## UpStreamCallback failed", 0U, 0U);
    }
}

// Downstream task send message to this task
static void DownStreamCallback(const CtfwCbMsg_t *pMsg)
{
    UINT32 RetVal = ARM_OK;
    RetVal |= Ctfw_MsgQueueSend(pMsg->Desc.pToHandle, CTFW_TASK_CMD_DNCB, pMsg);
    if ( RetVal != ARM_OK ) {
        ArmLog_ERR(pMsg->Desc.pFromHandle->CtfwResourceName.Task,
                   "## DownStreamCallback failed", 0U, 0U);
    }
}

// This task send message to downstream task
static UINT32 CallbackDownStream(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval;
    Rval = Ctfw_CallbackDownStream(pHandle);
    return Rval;
}

// This task send message to upstream task
static UINT32 CallbackUpStream(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval;
    Rval = Ctfw_CallbackUpStream(pHandle);
    return Rval;
}

static UINT32 CheckInOutReady(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval;
    Rval = Ctfw_CheckInOutReady(pHandle);
    return Rval;
}

static UINT32 Process(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    const flexidag_memblk_t *pInputData[MAX_UP_STREAM_NODES];
    CtfwFrameMetadata_t *pInputMetaDataPtr[MAX_UP_STREAM_NODES];

    flexidag_memblk_t *pOutputData;
    const flexidag_memblk_t *pOutMetaData;
    CtfwFrameMetadata_t *pOutputMetaDataPtr;

    UINT32 TskIdx;
    UINT32 InputTaskNum;
    UINT32 InputErrorState = 0U;

    if ( pHandle->UpStreamTaskNum != 0U ) {
        InputTaskNum = pHandle->UpStreamTaskNum;
    } else {
        // There is a input for source task actually
        InputTaskNum = 1U;
    }

    // Pass misrac initialization
    for (TskIdx = 0U; TskIdx < (sizeof(pInputMetaDataPtr)/sizeof(void*)); TskIdx++) {
        pInputMetaDataPtr[TskIdx] = NULL;
    }

    // Collect input pointer
    for (TskIdx = 0U; TskIdx < InputTaskNum; TskIdx++) {
        const flexidag_memblk_t *pMetaData;
        UINT32 ReadIdx;

        ReadIdx = pHandle->InputBuffer[TskIdx].ReadIdx;
        pInputData[TskIdx] = &(pHandle->InputBuffer[TskIdx].Desc[ReadIdx].Frame.CtfwData);
        pMetaData = &(pHandle->InputBuffer[TskIdx].Desc[ReadIdx].Frame.MetaData);
        AmbaMisra_TypeCast(&pInputMetaDataPtr[TskIdx], &pMetaData->pBuffer);
    }

    // Clear error state on source
    if ( pHandle->UpStreamTaskNum == 0U ) {
        InputErrorState = 0U;
    } else {
        InputErrorState = pInputMetaDataPtr[0U]->ErrorState;
    }

    // Check if all input are with consistent frame index
    for (TskIdx = 1U; TskIdx < pHandle->UpStreamTaskNum; TskIdx++) {
        InputErrorState |= pInputMetaDataPtr[TskIdx]->ErrorState;
        if ( pInputMetaDataPtr[TskIdx]->FrameIndex != pInputMetaDataPtr[0U]->FrameIndex ) {
            ArmLog_ERR(Ctfw_GetTaskName(pHandle), "## Frame index mismatch", 0U, 0U);
            ArmLog_ERR(Ctfw_GetTaskName(pHandle), "## Frame index[%u] = %u", 0U, pInputMetaDataPtr[0U]->FrameIndex);
            ArmLog_ERR(Ctfw_GetTaskName(pHandle), "## Frame index[%u] = %u", TskIdx, pInputMetaDataPtr[TskIdx]->FrameIndex);
            Rval = ARM_NG;
        }
    }

    // Output pointer
    {
        UINT32 WriteIdx;
        WriteIdx = pHandle->OutputBuffer[0U].WriteIdx;
        pOutputData = &pHandle->OutBuf[WriteIdx];
        pOutMetaData = &pHandle->OutMetadataBuf[WriteIdx];
        AmbaMisra_TypeCast(&pOutputMetaDataPtr, &pOutMetaData->pBuffer);
    }

    if ( Rval == ARM_OK ) {
        // Process metadata
        {
            // Copy metadata
            Rval |= AmbaWrap_memcpy(pOutputMetaDataPtr, pInputMetaDataPtr[0U], sizeof(CtfwFrameMetadata_t));
            pOutputMetaDataPtr->ErrorState = InputErrorState;
        }

        // Process data
        if ( pHandle->AlgTaskObj.pProcess != NULL ) {
            if (pOutputMetaDataPtr->ErrorState == 0U) {
                Rval |= pHandle->AlgTaskObj.pProcess(pInputMetaDataPtr[0U], pInputData, &pHandle->AlgBuffMap, pOutputData);
                if ( Rval != ARM_OK ) {
                    pOutputMetaDataPtr->ErrorState |= ARM_NG;
                    ArmLog_ERR(Ctfw_GetTaskName(pHandle), "## Process fail with return value(%u)", Rval, 0U);
                }
            } //else {
            //ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Warning, Get input ErrorState(%d), skip", pOutputMetaDataPtr->ErrorState, 0U);
            //}
        }

        if ( pHandle->GetProcCbFxn != NULL ) {
            pHandle->GetProcCbFxn(pOutputMetaDataPtr, pOutputData->pBuffer);
        }
    }

    if (pHandle->DownStreamTaskNum != 0U) {
        CtfwDesc_t CtfwDesc;
        Rval |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&CtfwDesc, 0, sizeof(CtfwDesc)));
        CtfwDesc.pFromHandle = pHandle;
        CtfwDesc.Frame.MetaData = *pOutMetaData;
        CtfwDesc.Frame.CtfwData = *pOutputData;

        // Update write index of this task's output buffer
        Rval |= Ctfw_BufferDescPushToAllDownStream(pHandle, &CtfwDesc);
    }

    return Rval;
}

static UINT32 Idle2Ready(const CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    (void) pHandle;

    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## do Idle2Ready, pHandle=%x", pHandle);

    return Rval;
}

static UINT32 Ready2Proc(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## do Ready2Proc, pHandle=%x", pHandle);

    Rval |= Ctfw_ProfilerTimerReset(pHandle, PROFILE_INSTANCE_ID);

    return Rval;
}

// Receive message from upstream task, update write index of this task's input buffer
static UINT32 ProcUpStreamMsg(CtfwTaskHandle_t *pHandle, const CtfwTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;

    if ( pHandle->UpStreamTaskNum != 0U) {
        Rval = Ctfw_ProcUpStreamCb(pHandle, pMsg);
    } else {
        const CtfwDesc_t *pDesc = &(pMsg->Desc);
        Rval = Ctfw_BufferDescPush(pHandle, &(pHandle->InputBuffer[0U]), pDesc);
    }

    return Rval;
}

// Receive message from downstream task, update read index of this task's output buffer
static UINT32 ProcDownStreamMsg(CtfwTaskHandle_t *pHandle, const CtfwTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;

    if ( pHandle->DownStreamTaskNum != 0U) {
        UINT32 Index = 0U;
        // Find who callbacks
        Rval = Ctfw_WhosCall(pHandle, pMsg->Desc.pFromHandle, &Index, FROM_DOWN_STREAM);
        // Update rp of corresponding output buffer
        Rval = Ctfw_BufferUprp(pHandle, &(pHandle->OutputBuffer[Index]));
    } else {
        Rval = Ctfw_BufferUprp(pHandle, &(pHandle->OutputBuffer[0U]));
    }

    return Rval;
}

static UINT32 CmdParserIdleState(const CtfwTaskHandle_t *pHandle, const CtfwTaskMsg_t *pMsg, UINT32 *pNextState)
{
    UINT32 Rval = ARM_OK;
    UINT32 Cmd = pMsg->Cmd;

    switch (Cmd) {
    case CTFW_TASK_CMD_INIT:
        Rval = Idle2Ready(pHandle);
        if (Rval == ARM_OK) {
            *pNextState = CTFW_TASK_STATE_READY;
        }
        break;
    default:
        *pNextState = CTFW_TASK_STATE_NA;
        break;
    }
    return Rval;
}

static UINT32 CmdParserReadyState(CtfwTaskHandle_t *pHandle, const CtfwTaskMsg_t *pMsg, UINT32 *pNextState)
{
    UINT32 Rval = ARM_OK;
    UINT32 Cmd = pMsg->Cmd;

    switch (Cmd) {
    case CTFW_TASK_CMD_START:
        Rval = Ready2Proc(pHandle);
        if (Rval == ARM_OK) {
            *pNextState = CTFW_TASK_STATE_PROC;
        }
        break;
    default:
        *pNextState = CTFW_TASK_STATE_NA;
        break;
    }
    return Rval;
}

static UINT32 CmdParserProcFrame(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    UINT32 ProcRval;

    Rval |= Ctfw_ProfilerStart(pHandle, PROFILE_INSTANCE_ID);
    ProcRval = Process(pHandle);
    Rval |= Ctfw_ProfilerStop(pHandle, ProcRval, PROFILE_INSTANCE_ID);

    // Notify upstream node
    Rval |= CallbackUpStream(pHandle);

    // Update read index of this task's input buffer
    if (pHandle->UpStreamTaskNum == 0U) {
        Rval |= Ctfw_BufferUprp(pHandle, &(pHandle->InputBuffer[0U]));
    } else {
        Rval |= Ctfw_BufferUprpToAll(pHandle, TO_UP_STREAM);
    }

    // Notify downstream node
    Rval |= CallbackDownStream(pHandle);

    return Rval;
}

static UINT32 CmdParserProcState(CtfwTaskHandle_t *pHandle, const CtfwTaskMsg_t *pMsg, UINT32 *pNextState)
{
    UINT32 Rval = ARM_OK;
    UINT32 Cmd = pMsg->Cmd;
    CtfwTaskHandle_t *pFromNode;
    UINT32 InOutCheckStat = ARM_NG;
    UINT32 UnknownCmd = 0U;

    AmbaMisra_TypeCast(&pFromNode, &pMsg->pPrivate);

    switch (Cmd) {
    case CTFW_TASK_CMD_UPCB:
        Rval = ProcUpStreamMsg(pHandle, pMsg);
        break;
    case CTFW_TASK_CMD_DNCB:
        Rval = ProcDownStreamMsg(pHandle, pMsg);
        break;
    case CTFW_TASK_CMD_CONNECT_DOWN_STREAM:
        Rval = Ctfw_InsertDownStreamList(pHandle, pFromNode);
        break;
    case CTFW_TASK_CMD_CONNECT_UP_STREAM:
        Rval = Ctfw_InsertUpStreamList(pHandle, pFromNode);
        break;
    case CTFW_TASK_CMD_DISCONNECT_DOWN_STREAM:
        Rval = Ctfw_DeleteStreamList(pHandle, pFromNode, TO_DOWN_STREAM);
        break;
    case CTFW_TASK_CMD_DISCONNECT_UP_STREAM:
        Rval = Ctfw_DeleteStreamList(pHandle, pFromNode, TO_UP_STREAM);
        break;
    default:
        UnknownCmd = 1U;
        break;
    }

    if ( Cmd == CTFW_TASK_CMD_STOP ) {
        *pNextState = CTFW_TASK_STATE_READY;
    } else if ( (Rval == ARM_OK) && (UnknownCmd == 0U) ) {
        *pNextState = CTFW_TASK_STATE_PROC;
    } else {
        *pNextState = CTFW_TASK_STATE_NA;
    }

    if ( Rval == ARM_OK ) {
        InOutCheckStat = CheckInOutReady(pHandle);

        if (InOutCheckStat == ARM_OK) {
            Rval |= CmdParserProcFrame(pHandle);
        }
    }

    return Rval;
}
static UINT32 StateTransitionFunc(CtfwTaskHandle_t *pHandle, const CtfwTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;
    UINT32 Cs = pHandle->CurrState;
    UINT32 Ns = 0U;

    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## StateTransitionFunc, CS=%d, CMD=%d", Cs, Cmd);
    switch(Cs) {
    case CTFW_TASK_STATE_IDLE:
        Rval = CmdParserIdleState(pHandle, pMsg, &Ns);
        break;
    case CTFW_TASK_STATE_READY:
        Rval = CmdParserReadyState(pHandle, pMsg, &Ns);
        break;
    case CTFW_TASK_STATE_PROC:
        Rval = CmdParserProcState(pHandle, pMsg, &Ns);
        break;
    default:
        Rval = ARM_NG;
        break;
    }

    if(Rval == ARM_OK) {
        pHandle->CurrState = Ns;
    }

    return Rval;
}

static ArmTaskRet_t TaskFunc(ArmTaskEntryArg_t EntryArg)
{
    CtfwTaskMsg_t Msg;
    UINT32 Rval;
    CtfwTaskHandle_t *pHandle;

    AmbaMisra_TouchUnused(EntryArg);

    AmbaMisra_TypeCast(&pHandle, &EntryArg);
    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## task start running, pHandle=%x", pHandle);
    while (pHandle->TaskRun == 1U) {
        // Clear error bit
        Rval = ARM_OK;

        Rval |= ArmMsgQueue_Recv(&pHandle->TaskMsgQ, &Msg);
        //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## recv_msg from %s", pHandle->CtfwResourceName.MsgQ);
        if ( Rval != ARM_OK ) {
            ArmLog_ERR(Ctfw_GetTaskName(pHandle), "## ArmMsgQueue_Recv Error", 0U, 0U);
        }

        if ( Rval == ARM_OK ) {
            Rval |= StateTransitionFunc(pHandle, &Msg);
            if(Rval != ARM_OK) {
                ArmLog_ERR(Ctfw_GetTaskName(pHandle), "## StateTransitionFunc Error", 0U, 0U);
            }
        }
    }
    return ARM_TASK_NULL_RET;
}

static UINT32 RegisterTask(const char *pRegTaskName, UINT32 *pOutInstId)
{
    UINT32 Rval = ARM_OK;
    UINT32 RegTaskIdx;
    UINT32 FoundInStorage = 0U;
    SIZE_t RegTaskNameLen;
    RegTaskNameLen = AmbaUtility_StringLength(pRegTaskName);

    for (RegTaskIdx = 0U; RegTaskIdx < gRegTaskDesk.RegTaskNum; RegTaskIdx++) {
        SIZE_t SavedTaskNameLen;
        const char *pSavedTaskName = gRegTaskDesk.RegTask[RegTaskIdx].TaskName;

        SavedTaskNameLen = AmbaUtility_StringLength(pSavedTaskName);

        if ( SavedTaskNameLen == RegTaskNameLen ) {
            INT32 IfDiff;
            IfDiff = AmbaUtility_StringCompare(pRegTaskName, pSavedTaskName, RegTaskNameLen);
            if ( IfDiff == 0 ) {
                // Found the same task
                UINT32 InstId;
                FoundInStorage = 1U;

                for (InstId = 0U; InstId < MAX_CTFW_TASK_INSTANCE_NUM; InstId++) {
                    if ( gRegTaskDesk.RegTask[RegTaskIdx].RegSlot[InstId] == 0U ) {
                        // Found available slot
                        *pOutInstId = InstId;
                        gRegTaskDesk.RegTask[RegTaskIdx].RegSlot[InstId] = 1U;
                        break;
                    }
                }

                if ( InstId == MAX_CTFW_TASK_INSTANCE_NUM ) {
                    // No available slot
                    ArmLog_ERR("CTFW", "Err, Number of registered instance has reached maximum!", 0U, 0U);
                    Rval = ARM_NG;
                }
                break;
            }
        }
    }

    // If not found in the pool, create new registered task
    if ( FoundInStorage == 0U ) {
        UINT32 NewRegTaskIdx;

        if ( gRegTaskDesk.RegTaskNum >= MAX_CTFW_TASK_NUM ) {
            ArmLog_ERR("CTFW", "Err, Number of registered task has reached maximum!", 0U, 0U);
            Rval = ARM_NG;
        }

        if (Rval == ARM_OK) {
            NewRegTaskIdx = gRegTaskDesk.RegTaskNum;
            *pOutInstId = 0U;
            gRegTaskDesk.RegTask[NewRegTaskIdx].RegSlot[0U] = 1U;
            AmbaUtility_StringCopy(gRegTaskDesk.RegTask[NewRegTaskIdx].TaskName, RegTaskNameLen+1U, pRegTaskName);
            gRegTaskDesk.RegTaskNum += 1U;
        }
    }

    return Rval;
}

static UINT32 UnRegisterTask(const char *pRegTaskName, UINT32 RegTaskInstId)
{
    SIZE_t RegTaskNameLen;
    UINT32 RegTaskIdx;
    UINT32 FoundInStorage = 0U;
    UINT32 Rval = ARM_OK;

    RegTaskNameLen = AmbaUtility_StringLength(pRegTaskName);

    for (RegTaskIdx = 0U; RegTaskIdx < gRegTaskDesk.RegTaskNum; RegTaskIdx++) {
        SIZE_t SavedTaskNameLen;
        const char *pSavedTaskName = gRegTaskDesk.RegTask[RegTaskIdx].TaskName;

        SavedTaskNameLen = AmbaUtility_StringLength(pSavedTaskName);

        if ( SavedTaskNameLen == RegTaskNameLen ) {
            INT32 IfDiff;
            IfDiff = AmbaUtility_StringCompare(pRegTaskName, pSavedTaskName, RegTaskNameLen);
            if ( IfDiff == 0 ) {
                // Found the same task
                // Remove registration
                gRegTaskDesk.RegTask[RegTaskIdx].RegSlot[RegTaskInstId] = 0U;
                FoundInStorage = 1U;
                break;
            }
        }
    }

    if ( FoundInStorage == 0U ) {
        ArmLog_ERR("CTFW", "Err, can't find task in the pool, unregistered fail!", 0U, 0U);
        Rval = ARM_NG;
    }

    return Rval;
}

UINT32 Ctfw_Init(void)
{
    UINT32 Rval = ARM_OK;

    Rval = Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&gRegTaskDesk, 0, sizeof(gRegTaskDesk)));

    return Rval;
}

UINT32 Ctfw_TaskQuerySize(const CtfwTaskObj_t *pAlgTaskObj, UINT32 OutBuffDepth, UINT32 *pTotalSize)
{
    UINT32 Rval = ARM_OK;
    UINT32 MetadataSize = (UINT32)sizeof(CtfwFrameMetadata_t);
    UINT32 TaskStackSize = 0U;
    UINT32 AlgBuffMapSize = 0U;
    UINT32 AlgWorkSize = 0U;
    UINT32 AlgOutputSize = 0U;

    UINT32 TotalSize = 0U;

    if (pAlgTaskObj != NULL) {
        UINT32 TotalOutputSize = 0U;

        if (pAlgTaskObj->pQuerySize != NULL) {
            Rval = pAlgTaskObj->pQuerySize(&TaskStackSize, &AlgBuffMapSize, &AlgWorkSize, &AlgOutputSize);
        }

        TotalSize += CTFW_CV_MEM_ALIGN(TaskStackSize);

        TotalSize += CTFW_CV_MEM_ALIGN(AlgBuffMapSize);

        TotalSize += CTFW_CV_MEM_ALIGN(AlgWorkSize);

        TotalOutputSize = OutBuffDepth*CTFW_CV_MEM_ALIGN(AlgOutputSize);
        TotalSize += TotalOutputSize;

        TotalSize += OutBuffDepth*CTFW_CV_MEM_ALIGN(MetadataSize);
    }

    *pTotalSize = TotalSize;

    return Rval;
}

static UINT32 Ctfw_TaskAllocMem(UINT32 MemPoolId, CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    UINT32 MetadataSize = (UINT32)sizeof(CtfwFrameMetadata_t);
    UINT32 TaskStackSize = 0U;
    UINT32 AlgBuffMapSize = 0U;
    UINT32 AlgWorkSize = 0U;
    UINT32 SingleOutputSize = 0U;

    UINT32 TotalOutputSize = 0U;

    if (pHandle->AlgTaskObj.pQuerySize != NULL) {
        Rval |= pHandle->AlgTaskObj.pQuerySize(&TaskStackSize, &AlgBuffMapSize, &AlgWorkSize, &SingleOutputSize);
    }

    // Create memory pool for the whole task
    if (Rval == ARM_OK) {
        UINT32 TskAllocSize = 0U;

        TskAllocSize += CTFW_CV_MEM_ALIGN(AlgBuffMapSize);

        TskAllocSize += CTFW_CV_MEM_ALIGN(AlgWorkSize);

        TotalOutputSize = pHandle->OutBuffDepth*CTFW_CV_MEM_ALIGN(SingleOutputSize);
        TskAllocSize += TotalOutputSize;

        TskAllocSize += pHandle->OutBuffDepth*CTFW_CV_MEM_ALIGN(MetadataSize);

        TskAllocSize += CTFW_CV_MEM_ALIGN(TaskStackSize);

        ArmLog_DBG(Ctfw_GetTaskName(pHandle), "alloc MemPoolId = %d", MemPoolId, 0);
        Rval |= Cftw_MemPool_Allocate(MemPoolId, TskAllocSize, &pHandle->AlgoTaskTotalBuf);
        ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Create mem pool, TskAllocSize = %u, Rval = %u", TskAllocSize, Rval); //fflush(stdout);
        Rval |= ArmMemPool_Create(&pHandle->AlgoTaskTotalBuf, &pHandle->MemPoolId);
    }

    // Partition buffer map
    if (Rval == ARM_OK) {
        if ( AlgWorkSize != 0U ) {
            Rval = Cftw_MemPool_Allocate(pHandle->MemPoolId, AlgBuffMapSize, &pHandle->AlgBuffMap);
        }
    }

    // Partition output buffer
    if (Rval == ARM_OK) {
        if ( SingleOutputSize != 0U ) {
            UINT32 OutBuffIdx;
            for (OutBuffIdx = 0U; OutBuffIdx < pHandle->OutBuffDepth; OutBuffIdx++) {
                Rval = Cftw_MemPool_Allocate(pHandle->MemPoolId, SingleOutputSize, &(pHandle->OutBuf[OutBuffIdx]));
            }
        }
    }

    // Partition metadata buffer
    if (Rval == ARM_OK) {
        UINT32 OutBuffIdx;
        for (OutBuffIdx = 0U; OutBuffIdx < pHandle->OutBuffDepth; OutBuffIdx++) {
            Rval = Cftw_MemPool_Allocate(pHandle->MemPoolId, MetadataSize, &(pHandle->OutMetadataBuf[OutBuffIdx]));
        }
    }

    // Initialize input/output buffer
    if (Rval == ARM_OK) {
        Rval = Ctfw_InBufferInit(pHandle, CTFW_MAX_BUFFER_DEPTH);
        Rval = Ctfw_OutBufferInit(pHandle, pHandle->OutBuffDepth);
    }

    // Create task
    if (Rval == ARM_OK) {
        pHandle->Task.Ctrl.StackSz = TaskStackSize;
        Rval = Cftw_MemPool_Allocate(pHandle->MemPoolId, pHandle->Task.Ctrl.StackSz, &(pHandle->Task.StackBuf));
    }

    return Rval;
}

UINT32 Ctfw_TaskInit(const CtfwTaskObj_t *pAlgTaskObj, CtfwGetProcResult_t pGetProcCbFxn,
                     const void *pAlgoConfig, flexidag_memblk_t FlexiBinBlk[], const char* FlowName,
                     UINT32 MemPoolId, UINT32 OutBuffDepth, CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    ARM_TASK_CREATE_ARG_s TaskCreateArg;
    ArmTaskEntryArg_t EntryArg;

    Rval |= AmbaWrap_memset(pHandle->RegTaskName, 0, sizeof(pHandle->RegTaskName));

    Rval |= AmbaWrap_memset(pHandle, 0, sizeof(CtfwTaskHandle_t));
    Rval |= AmbaWrap_memcpy(&pHandle->AlgTaskObj, pAlgTaskObj, sizeof(pHandle->AlgTaskObj));

    pHandle->GetProcCbFxn = pGetProcCbFxn;

    pHandle->CurrState = CTFW_TASK_STATE_IDLE;
    pHandle->UpStreamCallBack = &UpStreamCallback;
    pHandle->DownStreamCallBack = &DownStreamCallback;

    // Setup task name
    if (pHandle->AlgTaskObj.pSetName != NULL) {
        Rval = pHandle->AlgTaskObj.pSetName(pHandle->RegTaskName);
        if ( Rval != ARM_OK ) {
            ArmLog_ERR(pHandle->RegTaskName, "SetName failed with return (%d)", Rval, 0U);
        }
    } else {
        ArmLog_ERR(pHandle->RegTaskName, "Err, pHandle->AlgTaskObj.pSetName == NULL", Rval, 0U);
        Rval = ARM_NG;
    }

    if (Rval == ARM_OK) {
        Rval = RegisterTask(pHandle->RegTaskName, &pHandle->InstId);
    }

    // Assign resource names
    if (Rval == ARM_OK) {
        char TaskNameAddInst[MAX_CTFW_NAME_LEN];

        Rval |= AmbaWrap_memset(TaskNameAddInst, 0, sizeof(TaskNameAddInst));
        AmbaUtility_StringAppend(TaskNameAddInst, MAX_CTFW_NAME_LEN, pHandle->RegTaskName);
        // Append instance index to differentiate the same task with multiple instances
        {
            char NumStrBuff[16U];
            UINT32 RetStrLen;
            RetStrLen = AmbaUtility_UInt32ToStr(NumStrBuff, (UINT32)sizeof(NumStrBuff), pHandle->InstId, 10U);
            AmbaUtility_StringAppend(TaskNameAddInst, MAX_CTFW_NAME_LEN, "_");
            AmbaUtility_StringAppend(TaskNameAddInst, MAX_CTFW_NAME_LEN, NumStrBuff);
            (void) RetStrLen;
        }

        AmbaUtility_StringAppend(pHandle->CtfwResourceName.Task, MAX_CTFW_NAME_LEN, FlowName);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.Task, MAX_CTFW_NAME_LEN, "_");
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.Task, MAX_CTFW_NAME_LEN, TaskNameAddInst);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.Task, MAX_CTFW_NAME_LEN, "_Task");

        AmbaUtility_StringAppend(pHandle->CtfwResourceName.MsgQ, MAX_CTFW_NAME_LEN, FlowName);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.MsgQ, MAX_CTFW_NAME_LEN, "_");
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.MsgQ, MAX_CTFW_NAME_LEN, TaskNameAddInst);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.MsgQ, MAX_CTFW_NAME_LEN, "_TaskMsgQ");

        AmbaUtility_StringAppend(pHandle->CtfwResourceName.TaskMtx, MAX_CTFW_NAME_LEN, FlowName);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.TaskMtx, MAX_CTFW_NAME_LEN, "_");
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.TaskMtx, MAX_CTFW_NAME_LEN, TaskNameAddInst);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.TaskMtx, MAX_CTFW_NAME_LEN, "_TaskMtx");

        AmbaUtility_StringAppend(pHandle->CtfwResourceName.InBufMtx, MAX_CTFW_NAME_LEN, FlowName);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.InBufMtx, MAX_CTFW_NAME_LEN, "_");
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.InBufMtx, MAX_CTFW_NAME_LEN, TaskNameAddInst);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.InBufMtx, MAX_CTFW_NAME_LEN, "_InBufMtx");

        AmbaUtility_StringAppend(pHandle->CtfwResourceName.OutBufMtx, MAX_CTFW_NAME_LEN, FlowName);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.OutBufMtx, MAX_CTFW_NAME_LEN, "_");
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.OutBufMtx, MAX_CTFW_NAME_LEN, TaskNameAddInst);
        AmbaUtility_StringAppend(pHandle->CtfwResourceName.OutBufMtx, MAX_CTFW_NAME_LEN, "_OutBufMtx");
    }

    if ( (OutBuffDepth > 0U) && (OutBuffDepth <= CTFW_MAX_BUFFER_DEPTH) ) {
        pHandle->OutBuffDepth = OutBuffDepth;
    } else {
        ArmLog_ERR(Ctfw_GetTaskName(pHandle), "Error, OutBuffDepth = %d, but it must be in range [1, %d]", OutBuffDepth, CTFW_MAX_BUFFER_DEPTH);
        Rval = ARM_NG;
    }

    Rval |= Ctfw_TaskAllocMem(MemPoolId, pHandle);

    // Run custom alg init process
    if (pHandle->AlgTaskObj.pInit != NULL) {
        Rval |= pHandle->AlgTaskObj.pInit(pAlgoConfig, FlexiBinBlk, pHandle->MemPoolId, &pHandle->AlgBuffMap, &pHandle->TaskSetting);
        if ( Rval != ARM_OK ) {
            ArmLog_ERR(Ctfw_GetTaskName(pHandle), "Init failed with return (%d)", Rval, 0U);
        }
    }

    // Create Mutex
    Rval |= ArmMutex_Create(&pHandle->TskMutex, &(pHandle->CtfwResourceName.TaskMtx[0U]));
    Rval |= ArmMutex_Create(&pHandle->InBufMtx, &(pHandle->CtfwResourceName.InBufMtx[0U]));
    Rval |= ArmMutex_Create(&pHandle->OutBufMtx, &(pHandle->CtfwResourceName.OutBufMtx[0U]));

    // Create message queue
    if (Rval == ARM_OK) {
        Rval = Ctfw_MsgQueueCreate(pHandle);
    }

    if (Rval == ARM_OK) {
        AmbaMisra_TypeCast(&EntryArg, &pHandle);
        TaskCreateArg.TaskPriority = pHandle->TaskSetting.TaskPriority;
        TaskCreateArg.EntryFunction = &TaskFunc;
        TaskCreateArg.EntryArg = EntryArg;
        TaskCreateArg.pStackBase = pHandle->Task.StackBuf.pBuffer;
        TaskCreateArg.StackByteSize = pHandle->Task.Ctrl.StackSz;
        TaskCreateArg.CoreSel = pHandle->TaskSetting.TaskCoreSel;
        pHandle->TaskRun = 1U;
        Rval = ArmTask_Create(&(pHandle->Task.Tsk), pHandle->CtfwResourceName.Task, &TaskCreateArg);
    }

    // send init command to task
    if (Rval == ARM_OK) {
        Rval = Ctfw_MsgQueueSend(pHandle, CTFW_TASK_CMD_INIT, NULL);
    }

    // wait for ready state
    Rval |= Ctfw_WaitState(pHandle, CTFW_TASK_STATE_READY);

    return Rval;
}


UINT32 Ctfw_TaskStart(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_WaitState(pHandle, CTFW_TASK_STATE_READY);
    Rval |= Ctfw_MsgQueueSend(pHandle, CTFW_TASK_CMD_START, NULL);
    Rval |= Ctfw_WaitState(pHandle, CTFW_TASK_STATE_PROC);

    return Rval;
}

UINT32 Ctfw_TaskStop(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_WaitState(pHandle, CTFW_TASK_STATE_PROC);
    Rval |= Ctfw_MsgQueueSend(pHandle, CTFW_TASK_CMD_STOP, NULL);
    Rval |= Ctfw_WaitState(pHandle, CTFW_TASK_STATE_READY);

    return Rval;
}

UINT32 Ctfw_TaskDeinit(UINT32 MemPoolId, CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Ctfw_TaskDeinit start", 0U, 0U);

    Rval |= Ctfw_WaitState(pHandle, CTFW_TASK_STATE_READY);

    if (pHandle->AlgTaskObj.pDeInit != NULL) {
        Rval = pHandle->AlgTaskObj.pDeInit(pHandle->MemPoolId, &pHandle->AlgBuffMap);
    }

    if (Rval == ARM_OK) {
        Rval = ArmMemPool_Free(pHandle->MemPoolId, &pHandle->AlgBuffMap);
    }

    if (Rval == ARM_OK) {
        Rval |= ArmTask_Delete(&(pHandle->Task.Tsk));
        Rval |= ArmMemPool_Free(pHandle->MemPoolId, &(pHandle->Task.StackBuf));
    }

    if (Rval == ARM_OK) {
        Rval |= Ctfw_InBufferDeinit(pHandle, CTFW_MAX_BUFFER_DEPTH);
        Rval |= Ctfw_OutBufferDeinit(pHandle, pHandle->OutBuffDepth);
    }

    if (Rval == ARM_OK) {
        Rval |= Ctfw_MsgQueueDelete(pHandle);
    }

    if (Rval == ARM_OK) {
        Rval |= ArmMutex_Delete(&pHandle->TskMutex);
        Rval |= ArmMutex_Delete(&pHandle->InBufMtx);
        Rval |= ArmMutex_Delete(&pHandle->OutBufMtx);
    }

    if (Rval == ARM_OK) {
        Rval |= ArmMemPool_Delete(pHandle->MemPoolId);
        Rval |= ArmMemPool_Free(MemPoolId, &pHandle->AlgoTaskTotalBuf);
    }

    if (Rval == ARM_OK) {
        Rval |= UnRegisterTask(pHandle->RegTaskName, pHandle->InstId);
    }

    ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Ctfw_TaskDeinit done", 0U, 0U);

    return Rval;
}
