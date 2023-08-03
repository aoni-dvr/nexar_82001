#include "Ctfw_TaskUtil.h"
#include "AmbaMisraFix.h"

const char* Ctfw_GetTaskName(const CtfwTaskHandle_t *pHandle)
{
    return &(pHandle->CtfwResourceName.Task[0U]);
}

UINT32 Ctfw_CheckRetVal(const char *pCallerName, const char *pCalleeName, UINT32 RetVal)
{
    if (RetVal != ARM_OK) {
        ArmLog_STR("", "%s Call Function:%s Fail", pCallerName, pCalleeName);
    }
    return RetVal;
}

UINT32 Ctfw_ProcUpStreamCb(CtfwTaskHandle_t *pHandle, const CtfwTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;
    //UINT32 IsFull;
    const CtfwDesc_t *pDesc = &(pMsg->Desc);
    const CtfwFrameMetadata_t *pMetaDataPtr;
    UINT32 Index = 0U;

    //find who callbacks
    Rval |= Ctfw_WhosCall(pHandle, pMsg->Desc.pFromHandle, &Index, FROM_UP_STREAM);

    AmbaMisra_TypeCast(&pMetaDataPtr, &pDesc->Frame.MetaData.pBuffer);

    //ArmLog_STR(TASKNAME, "## ProcUpStreamCb");
    if( (pMetaDataPtr->FrameIndex % pHandle->TaskSetting.ProcInterval) == 0U) {
        Rval = Ctfw_BufferDescPush(pHandle, &(pHandle->InputBuffer[Index]), pDesc); //content is AMBA_CV_IMG_BUF_s
        if(Rval != ARM_OK) {
            //ArmLog_ERR(TASKNAME, "## ProcUpStreamCb : input buffer[Index=%d] full!!", Index);
        }
    } else {
        // discard this frame, callback upstream
        CtfwCbMsg_t Msg;

        Rval |= AmbaWrap_memcpy(&(Msg.Desc), pDesc, sizeof(CtfwDesc_t));
        if ( Rval == ARM_OK ) {
            Msg.Desc.pToHandle = Msg.Desc.pFromHandle;
            Msg.Desc.pFromHandle = pHandle;
            //Msg.Desc.IsLastFrame = 0U;
            Msg.pPrivate = NULL;
            pDesc->pFromHandle->DownStreamCallBack(&Msg);
        }
    }


    return Rval;
}

UINT32 Ctfw_CallbackDownStream(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    CtfwCbMsg_t Msg;
    UINT32 TaskIdx;

    for (TaskIdx = 0U; TaskIdx < pHandle->DownStreamTaskNum; TaskIdx++) {
        if (pHandle->pDownStreamList[TaskIdx] == NULL) {
            continue;
        } else if (pHandle->pDownStreamList[TaskIdx]->UpStreamCallBack != NULL) {
            UINT32 Index;

            // get the latest pushed desc
            if (pHandle->OutputBuffer[TaskIdx].WriteIdx == 0U) {
                Index = pHandle->OutputBuffer[TaskIdx].Depth - 1U;
            } else {
                Index = (pHandle->OutputBuffer[TaskIdx].WriteIdx - 1U);
            }

            Rval |= AmbaWrap_memcpy(&(Msg.Desc), &(pHandle->OutputBuffer[TaskIdx].Desc[Index]), sizeof(CtfwDesc_t));
            if ( Rval == ARM_OK ) {
                Msg.Desc.pFromHandle = pHandle;
                Msg.Desc.pToHandle = pHandle->pDownStreamList[TaskIdx];
                //Msg.Desc.IsLastFrame = 0U;
                Msg.pPrivate = NULL;

                //            ArmLog_STR(TASKNAME, "## call %s", Msg.pDesc->pToHandle->CtfwResourceName.Task);

                pHandle->pDownStreamList[TaskIdx]->UpStreamCallBack(&Msg);
            }
        } else {
            // Nothing to do
        }
    }

    //ArmLog_STR(TASKNAME, "## CallbackDownStream finished", I);
    return Rval;
}

UINT32 Ctfw_CallbackUpStream(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    CtfwCbMsg_t Msg;
    UINT32 TaskIdx;

    for (TaskIdx = 0U; TaskIdx < pHandle->UpStreamTaskNum; TaskIdx++) {
        if (pHandle->pUpStreamList[TaskIdx] == NULL) {
            continue;
        } else if (pHandle->pUpStreamList[TaskIdx]->DownStreamCallBack != NULL) {
            Rval |= AmbaWrap_memcpy(&(Msg.Desc), &(pHandle->InputBuffer[TaskIdx].Desc[pHandle->InputBuffer[TaskIdx].ReadIdx]), sizeof(CtfwDesc_t));
            if ( Rval == ARM_OK ) {
                Msg.Desc.pFromHandle = pHandle;
                Msg.Desc.pToHandle = pHandle->pUpStreamList[TaskIdx];
                //Msg.Desc.IsLastFrame = 0U;
                Msg.pPrivate = NULL;

                //            ArmLog_STR(TASKNAME, "## call %s", Msg.pDesc->pToHandle->CtfwResourceName.Task);

                pHandle->pUpStreamList[TaskIdx]->DownStreamCallBack(&Msg);
            }
        } else {
            // Nothing to do
        }
    }


    //ArmLog_STR(TASKNAME, "## CallbackUpStream finished", I);
    return Rval;
}

UINT32 Ctfw_InBufferInit(CtfwTaskHandle_t *pHandle, UINT32 Depth)
{
    UINT32 Rval = ARM_OK;
    UINT32 BuffIdx;
    ArmLog_STR(Ctfw_GetTaskName(pHandle), "## init input buffer", NULL, NULL);

    for (BuffIdx = 0U; BuffIdx < MAX_UP_STREAM_NODES; BuffIdx++) {
        pHandle->InputBuffer[BuffIdx].Depth = Depth;
        pHandle->InputBuffer[BuffIdx].ReadIdx = 0U;
        pHandle->InputBuffer[BuffIdx].WriteIdx = 0U;
    }

    return Rval;
}

UINT32 Ctfw_InBufferDeinit(CtfwTaskHandle_t *pHandle, UINT32 Depth)
{
    UINT32 Rval = ARM_OK;
    UINT32 BuffIdx;
    ArmLog_STR(Ctfw_GetTaskName(pHandle), "## deinit input buffer", NULL, NULL);

    for (BuffIdx = 0U; BuffIdx < MAX_UP_STREAM_NODES; BuffIdx++) {
        pHandle->InputBuffer[BuffIdx].Depth = Depth;
        pHandle->InputBuffer[BuffIdx].ReadIdx = 0U;
        pHandle->InputBuffer[BuffIdx].WriteIdx = 0U;
    }

    return Rval;
}

UINT32 Ctfw_OutBufferInit(CtfwTaskHandle_t *pHandle, UINT32 Depth)
{
    UINT32 Rval = ARM_OK;
    UINT32 BuffIdx;
    ArmLog_STR(Ctfw_GetTaskName(pHandle), "## init output buffer", NULL, NULL);

    for (BuffIdx = 0U; BuffIdx < MAX_DOWN_STREAM_NODES; BuffIdx++) {
        pHandle->OutputBuffer[BuffIdx].Depth = Depth;
        pHandle->OutputBuffer[BuffIdx].ReadIdx = 0U;
        pHandle->OutputBuffer[BuffIdx].WriteIdx = 0U;
    }

    return Rval;
}

UINT32 Ctfw_OutBufferDeinit(CtfwTaskHandle_t *pHandle, UINT32 Depth)
{
    UINT32 Rval = ARM_OK;
    UINT32 BuffIdx;
    ArmLog_STR(Ctfw_GetTaskName(pHandle), "## deinit output buffer", NULL, NULL);

    for (BuffIdx = 0U; BuffIdx < MAX_DOWN_STREAM_NODES; BuffIdx++) {
        pHandle->OutputBuffer[BuffIdx].Depth = Depth;
        pHandle->OutputBuffer[BuffIdx].ReadIdx = 0U;
        pHandle->OutputBuffer[BuffIdx].WriteIdx = 0U;
    }

    return Rval;
}

UINT32 Ctfw_IsBufferEmpty(CtfwTaskHandle_t *pHandle, const CtfwBuffer_t *pBuf, UINT32 *pIsEmpty)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    if(pBuf == NULL) {
        Rval = ARM_NG;
    }
    *pIsEmpty = 0U;
    if(Rval == ARM_OK) {
        if(pBuf->ReadIdx == pBuf->WriteIdx) {
            *pIsEmpty = 1U;
        }
    }

    return Rval;
}

UINT32 Ctfw_IsBufferFull(CtfwTaskHandle_t *pHandle, const CtfwBuffer_t *pBuf, UINT32 *pIsFull)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    if(pBuf == NULL) {
        Rval = ARM_NG;
    }
    //ArmLog_STR(TASKNAME, "## check IsBufferFull rp=%d, wp=%d", pBuf->ReadIdx, pBuf->WriteIdx);
    *pIsFull = 0U;
    if(Rval == ARM_OK) {
        if(pBuf->ReadIdx == ((pBuf->WriteIdx + 1U) % pBuf->Depth)) {
            *pIsFull = 1U;
        }
    }

    return Rval;
}

UINT32 Ctfw_BufferUpwp(CtfwTaskHandle_t *pHandle, CtfwBuffer_t *pBuf)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    if (pBuf == NULL) {
        Rval = ARM_NG;
    }
    if (Rval == ARM_OK) {
        pBuf->WriteIdx = ((pBuf->WriteIdx + 1U) % pBuf->Depth);
    }

    return Rval;
}

UINT32 Ctfw_BufferUprp(CtfwTaskHandle_t *pHandle, CtfwBuffer_t *pBuf)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    if (pBuf == NULL) {
        Rval = ARM_NG;
    }
    if (Rval == ARM_OK) {
        //ArmLog_STR(TASKNAME, "## check InputBuffer rp=%d, wp=%d", pBuf->ReadIdx, pBuf->WriteIdx);
        pBuf->ReadIdx = ((pBuf->ReadIdx + 1U) % pBuf->Depth);
    }

    return Rval;
}

UINT32 Ctfw_BufferUprpToAll(CtfwTaskHandle_t *pHandle, UINT32 ToUpDown)
{
    UINT32 Rval = ARM_OK;
    //UINT32 IsEmpty;
    //UINT32 IsFull;
    UINT32 i;
    CtfwBuffer_t *pBuf;

    if (ToUpDown == TO_UP_STREAM) {

        for (i = 0; i < pHandle->UpStreamTaskNum; i++) {
            if (pHandle->pUpStreamList[i] == NULL) {
                continue;
            }
            pBuf = &(pHandle->InputBuffer[i]);
            Rval = Ctfw_BufferUprp(pHandle, pBuf);
        }

    } else if (ToUpDown == TO_DOWN_STREAM) {

        for (i = 0; i < pHandle->DownStreamTaskNum; i++) {
            if (pHandle->pDownStreamList[i] == NULL) {
                continue;
            }
            pBuf = &(pHandle->OutputBuffer[i]);
            Rval = Ctfw_BufferUprp(pHandle, pBuf);
        }

    } else {
        //
    }

    return Rval;
}
#if 0
UINT32 Ctfw_BufferUpwpToAll(CtfwTaskHandle_t *pHandle, UINT32 ToUpDown)
{
    UINT32 Rval = ARM_OK;
    //UINT32 IsEmpty;
    //UINT32 IsFull;
    UINT32 i;
    CtfwBuffer_t *pBuf;

    if (ToUpDown == TO_UP_STREAM) {

        for (i = 0U; i < pHandle->UpStreamTaskNum; i++) {
            if (pHandle->pUpStreamList[i] == NULL) {
                continue;
            }
            pBuf = &(pHandle->InputBuffer[i]);
            Rval = Ctfw_BufferUpwp(pHandle, pBuf);
        }

    } else if (ToUpDown == TO_DOWN_STREAM) {

        for (i = 0U; i < pHandle->DownStreamTaskNum; i++) {
            if (pHandle->pDownStreamList[i] == NULL) {
                continue;
            }
            pBuf = &(pHandle->OutputBuffer[i]);
            Rval = Ctfw_BufferUpwp(pHandle, pBuf);
        }

    } else {
        //
    }

    return Rval;
}
#endif
UINT32 Ctfw_BufferDescPush(CtfwTaskHandle_t *pHandle, CtfwBuffer_t *pBuf, const CtfwDesc_t *pDesc)
{
    UINT32 Rval = ARM_OK;
    UINT32 IsFull;

    if(pBuf == NULL) {
        Rval = ARM_NG;
    }
    Rval |= Ctfw_IsBufferFull(pHandle, pBuf, &IsFull);
    if(IsFull == 1U) {
        ArmLog_ERR(Ctfw_GetTaskName(pHandle), "## Ctfw_BufferDescPush : buffer full!!", 0U, 0U);
        Rval = ARM_NG;
    } else {
        Rval |= AmbaWrap_memcpy(&(pBuf->Desc[pBuf->WriteIdx]), pDesc, sizeof(CtfwDesc_t));
        //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Got FrameNum=%d", pBuf->Desc[pBuf->WriteIdx].FrameNum);
        Rval |= Ctfw_BufferUpwp(pHandle, pBuf);
    }

    return Rval;
}

UINT32 Ctfw_BufferDescPushToAllDownStream(CtfwTaskHandle_t *pHandle, const CtfwDesc_t *pCtfwDesc)
{
    UINT32 Rval = ARM_OK;
    //UINT32 IsEmpty;
    //UINT32 IsFull;
    UINT32 TaskIdx;
    CtfwBuffer_t *pBuf;
    CtfwDesc_t CtfwDesc;

    CtfwDesc = *pCtfwDesc;
    // scan all hooked output are not full
    for (TaskIdx = 0U; TaskIdx < pHandle->DownStreamTaskNum; TaskIdx++) {
        if (pHandle->pDownStreamList[TaskIdx] == NULL) {
            continue;
        }
        //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## pushing DownStreamList[%d]", i);
        pBuf = &(pHandle->OutputBuffer[TaskIdx]);
        CtfwDesc.pToHandle = pHandle->pDownStreamList[TaskIdx];
        Rval |= Ctfw_BufferDescPush(pHandle, pBuf, &CtfwDesc);
    }

    return Rval;
}

UINT32 Ctfw_CheckInOutReady(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    UINT32 IsEmpty;
    UINT32 IsFull;
    UINT32 i;
    UINT32 InputTaskNum;

    if ( pHandle->UpStreamTaskNum != 0U ) {
        InputTaskNum = pHandle->UpStreamTaskNum;
    } else {
        // There is a input for source task actually
        InputTaskNum = 1U;
    }

    Rval = ARM_NG;
    // scan all hooked input are not empty
    for (i = 0U; i < InputTaskNum; i++) {
        const CtfwBuffer_t *pBuf;

        pBuf = &(pHandle->InputBuffer[i]);
        //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## check InputBuffer[%d] rp=%d, wp=%d", i, pBuf->ReadIdx, pBuf->WriteIdx);
        Rval = Ctfw_IsBufferEmpty(pHandle, pBuf, &IsEmpty);
        if (IsEmpty == 1U) {
            Rval = ARM_NG;
            break;
        } else {
            Rval = ARM_OK;
        }
    }

    // scan all hooked output are not full
    if (Rval == ARM_OK) {
        if ( pHandle->DownStreamTaskNum != 0U ) {
            Rval = ARM_NG;
            for (i = 0; i < pHandle->DownStreamTaskNum; i++) {
                const CtfwBuffer_t *pBuf;

                if (pHandle->pDownStreamList[i] == NULL) {
                    continue;
                }

                pBuf = &(pHandle->OutputBuffer[i]);
                //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## check OutputBuffer[%d] rp=%d, wp=%d", i, pBuf->ReadIdx, pBuf->WriteIdx);
                Rval = Ctfw_IsBufferFull(pHandle, pBuf, &IsFull);
                if (IsFull == 1U) {
                    Rval = ARM_NG;
                    break;
                } else {
                    Rval = ARM_OK;
                }
            }
        }
    }

//    if(Rval == ARM_OK) {
//        ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Input and output are OK");
//    } else {
//        ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Input and output are not ready");
//    }
    return Rval;
}

UINT32 Ctfw_WaitState(CtfwTaskHandle_t *pHandle, UINT32 State)
{
    UINT32 Rval = ARM_OK;
    Rval |= ArmMutex_Take(&pHandle->TskMutex);
    while (pHandle->CurrState != State) {
        //usleep(10000);
        Rval |= AmbaKAL_TaskSleep(1U);
    }
    Rval |= ArmMutex_Give(&pHandle->TskMutex);

    return Rval;
}

UINT32 Ctfw_MsgQueueCreate(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    Rval = ArmMsgQueue_Create(&(pHandle->TaskMsgQ), &(pHandle->CtfwResourceName.MsgQ[0U]),
                              (UINT32)sizeof(CtfwTaskMsg_t), MAX_MSG_BUFFER_DEPTH, &(pHandle->TaskMsgQBuffer[0U]));
    return Rval;
}

UINT32 Ctfw_MsgQueueDelete(CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    Rval = ArmMsgQueue_Delete(&(pHandle->TaskMsgQ), &(pHandle->CtfwResourceName.MsgQ[0U]));
    return Rval;
}


UINT32 Ctfw_MsgQueueSend(CtfwTaskHandle_t *pHandle, UINT32 Cmd, const CtfwCbMsg_t *pCbMsg)
{
    UINT32 Rval = ARM_OK;
    CtfwTaskMsg_t Msg;

    Rval |= AmbaWrap_memset(&Msg, 0, sizeof(Msg));
    Msg.Cmd = Cmd;
    if (pCbMsg != NULL) {
        Msg.Desc = pCbMsg->Desc;
        Msg.pPrivate = pCbMsg->pPrivate;
    }

    Rval |= ArmMsgQueue_Send(&(pHandle->TaskMsgQ), &Msg);

    return Rval;
}


UINT32 Ctfw_CmdConnectUpStream(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pUpStreamNode)
{
    UINT32 Rval = ARM_OK;
    CtfwCbMsg_t CbMsg;

    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Send upstream node %s", Ctfw_GetTaskName(pUpStreamNode), NULL);

    Rval |= AmbaWrap_memset(&CbMsg, 0, sizeof(CbMsg));
    CbMsg.pPrivate = pUpStreamNode;
    Rval |= Ctfw_MsgQueueSend(pHandle, CTFW_TASK_CMD_CONNECT_UP_STREAM, &CbMsg);

    return Rval;
}

/*
 * UpDown: 0: up, 1: down
 */
UINT32 Ctfw_CmdDisconnectStream(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pNode, UINT32 UpDown)
{
    UINT32 Rval = ARM_OK;
    CtfwCbMsg_t CbMsg;

    Rval |= AmbaWrap_memset(&CbMsg, 0, sizeof(CbMsg));
    CbMsg.pPrivate = pNode;

    if (UpDown == TO_UP_STREAM) {
        //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## send upstream disconnect, node %s", Ctfw_GetTaskName(pNode), NULL);
        Rval |= Ctfw_MsgQueueSend(pHandle, CTFW_TASK_CMD_DISCONNECT_UP_STREAM, &CbMsg);
    } else {
        //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## send downstream disconnect, node %s", Ctfw_GetTaskName(pNode), NULL);
        Rval |= Ctfw_MsgQueueSend(pHandle, CTFW_TASK_CMD_DISCONNECT_DOWN_STREAM, &CbMsg);
    }

    return Rval;
}


UINT32 Ctfw_CmdConnectDownStream(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;
    CtfwCbMsg_t CbMsg;

    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Send downstream node %s", Ctfw_GetTaskName(pDownStreamNode), NULL);

    Rval |= AmbaWrap_memset(&CbMsg, 0, sizeof(CbMsg));
    CbMsg.pPrivate = pDownStreamNode;
    Rval |= Ctfw_MsgQueueSend(pHandle, CTFW_TASK_CMD_CONNECT_DOWN_STREAM, &CbMsg);

    return Rval;
}

UINT32 Ctfw_InsertDownStreamList(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;
    UINT32 TaskIdx;

    for (TaskIdx = 0U; TaskIdx < MAX_DOWN_STREAM_NODES; TaskIdx++) {
        if (pHandle->pDownStreamList[TaskIdx] == NULL) {
            pHandle->pDownStreamList[TaskIdx] = pDownStreamNode;
            pHandle->DownStreamTaskNum++;
            //ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## DownStreamList[%d] is available", I, 0U);
            //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Hooked downstream node %s", Ctfw_GetTaskName(pDownStreamNode), NULL);
            break;
        }
    }
    if (TaskIdx == MAX_DOWN_STREAM_NODES) {
        //error, list is full
        Rval = ARM_NG;
    }

    return Rval;
}


UINT32 Ctfw_InsertUpStreamList(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pUpStreamNode)
{
    UINT32 Rval = ARM_OK;
    UINT32 TaskIdx;

    for (TaskIdx = 0U; TaskIdx < MAX_UP_STREAM_NODES; TaskIdx++) {
        if (pHandle->pUpStreamList[TaskIdx] == NULL) {
            pHandle->pUpStreamList[TaskIdx] = pUpStreamNode;
            pHandle->UpStreamTaskNum++;
            //ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## UpStreamList[%d] is available", I, 0U);
            //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Hooked upstream node %s", Ctfw_GetTaskName(pUpStreamNode), NULL);
            break;
        }
    }
    if (TaskIdx == MAX_UP_STREAM_NODES) {
        //error, list is full
        Rval = ARM_NG;
    }

    return Rval;
}

UINT32 Ctfw_DeleteStreamList(CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pNode, UINT32 UpDown)
{
    UINT32 Rval = ARM_OK;
    UINT32 TaskIdx;

    if (UpDown == TO_UP_STREAM) {
        for (TaskIdx = 0U; TaskIdx < MAX_UP_STREAM_NODES; TaskIdx++) {
            if (pHandle->pUpStreamList[TaskIdx] == pNode) {
                pHandle->pUpStreamList[TaskIdx] = NULL;
                pHandle->UpStreamTaskNum--;
                //ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## found target upstream node at UpStreamList[%d]", I, 0U);
                //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## delete upstream node %s", Ctfw_GetTaskName(pNode), NULL);
                break;
            }
        }
        if (TaskIdx == MAX_UP_STREAM_NODES) {
            //error, list is full
            Rval = ARM_NG;
        }
    } else {
        for (TaskIdx = 0U; TaskIdx < MAX_DOWN_STREAM_NODES; TaskIdx++) {
            if (pHandle->pDownStreamList[TaskIdx] == pNode) {
                pHandle->pDownStreamList[TaskIdx] = NULL;
                pHandle->DownStreamTaskNum--;
                //ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## found target downstream node at DownStreamList[%d]", I, 0U);
                //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## delete downstream node %s", Ctfw_GetTaskName(pNode), NULL);
                break;
            }
        }
        if (TaskIdx == MAX_DOWN_STREAM_NODES) {
            //error, list is full
            Rval = ARM_NG;
        }
    }
    return Rval;
}


UINT32 Ctfw_TaskConnect(CtfwTaskHandle_t *pUpStreamNode, CtfwTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;
    //UINT32 I;

    Rval |= Ctfw_InsertUpStreamList(pDownStreamNode, pUpStreamNode);
    Rval |= Ctfw_InsertDownStreamList(pUpStreamNode, pDownStreamNode);

    // Hook upstream to downstream

//    if (Rval == ARM_OK) {
//        for (I = 0U; I < MAX_UP_STREAM_NODES; I++) {
//            if (pDownStreamNode->pUpStreamList[I] == NULL) {
//                pDownStreamNode->pUpStreamList[I] = pUpStreamNode;
//                //ArmLog_STR("Ctfw", "## Hooked upstream node %s(%x) to downstream node %s(%x) at List[%d]", Ctfw_GetTaskName(pUpStreamNode), pUpStreamNode, Ctfw_GetTaskName(pDownStreamNode), pDownStreamNode, I);
//                break;
//            }
//        }
//        if (I == MAX_UP_STREAM_NODES) {
//            //error, list is full
//            Rval = ARM_NG;
//        }
//    }


    // Hook downstream to upstream
//    if (Rval == ARM_OK) {
//        for (I = 0U; I < MAX_DOWN_STREAM_NODES; I++) {
//            if (pUpStreamNode->pDownStreamList[I] == NULL) {
//                pUpStreamNode->pDownStreamList[I] = pDownStreamNode;
//                //ArmLog_STR("Ctfw", "## Hooked downstream node %s(%x) to upstream node %s(%x) at List[%d]", Ctfw_GetTaskName(pDownStreamNode), pDownStreamNode, Ctfw_GetTaskName(pUpStreamNode), pUpStreamNode, I);
//                break;
//            }
//        }
//        if (I == MAX_DOWN_STREAM_NODES) {
//            //error, list is full
//            Rval = ARM_NG;
//        }
//    }

    return Rval;
}

UINT32 Ctfw_ConnectUpCheck(const CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pUpStreamNode)
{
    UINT32 Rval = ARM_NG;
    UINT32 TaskIdx;
    UINT32 Found = 0U;

    while(Found == 0U) {
        for (TaskIdx = 0U; TaskIdx < MAX_UP_STREAM_NODES; TaskIdx++) {
            if (pHandle->pUpStreamList[TaskIdx] == pUpStreamNode) {
                Found = 1U;
                Rval = ARM_OK;
                //ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Found target upstream node at UpStreamList[%d]", I, 0U);
                //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Checked upstream node %s", Ctfw_GetTaskName(pUpStreamNode), NULL);
                break;
            }
        }
        //usleep(30000);
        Rval |= AmbaKAL_TaskSleep(1U);
    }

    return Rval;
}

UINT32 Ctfw_ConnectDownCheck(const CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_NG;
    UINT32 TaskIdx;
    UINT32 Found = 0U;

    while(Found == 0U) {
        for (TaskIdx = 0U; TaskIdx < MAX_UP_STREAM_NODES; TaskIdx++) {
            if (pHandle->pDownStreamList[TaskIdx] == pDownStreamNode) {
                Found = 1U;
                Rval = ARM_OK;
                //ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Found target downstream node at DownStreamList[%d]", I, 0U);
                //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Checked downstream node %s", Ctfw_GetTaskName(pDownStreamNode), NULL);
                break;
            }
        }
        //usleep(30000);
        Rval |= AmbaKAL_TaskSleep(1U);
    }

    return Rval;
}

UINT32 Ctfw_DisconnectCheck(const CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pNode, UINT32 UpDown)
{
    UINT32 Rval = ARM_OK;
    UINT32 TaskIdx;
    UINT32 Found = 1U;

    if (UpDown == TO_UP_STREAM) {
        while (Found == 1U) {
            for (TaskIdx = 0U; TaskIdx < MAX_UP_STREAM_NODES; TaskIdx++) {
                if (pHandle->pUpStreamList[TaskIdx] == pNode) {
                    Found = 1U;
                    //ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Found target upstream node at UpStreamList[%d]", I, 0U);
                    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Found upstream node %s", Ctfw_GetTaskName(pNode), NULL);
                    break;
                } else {
                    Found = 0U;
                    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Cannot find upstream node %s in UpStreamList", Ctfw_GetTaskName(pNode), NULL);
                }
            }
            //usleep(30000);
            Rval |= AmbaKAL_TaskSleep(1U);
        }
    } else {
        while (Found == 1U) {
            for (TaskIdx = 0U; TaskIdx < MAX_DOWN_STREAM_NODES; TaskIdx++) {
                if (pHandle->pDownStreamList[TaskIdx] == pNode) {
                    Found = 1U;
                    //ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Found target downstream node at DownStreamList[%d]", I, 0U);
                    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Found downstream node %s", Ctfw_GetTaskName(pNode), NULL);
                    break;
                } else {
                    Found = 0U;
                    //ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Cannot find downstream node %s in DownStreamList", Ctfw_GetTaskName(pNode), NULL);
                }
            }
            //usleep(30000);
            Rval |= AmbaKAL_TaskSleep(1U);
        }
    }


    return Rval;
}


UINT32 Ctfw_TaskConnect_TF(CtfwTaskHandle_t *pUpStreamNode, CtfwTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_CmdConnectUpStream(pDownStreamNode, pUpStreamNode);
    Rval |= Ctfw_ConnectUpCheck(pDownStreamNode, pUpStreamNode);
    Rval |= Ctfw_CmdConnectDownStream(pUpStreamNode, pDownStreamNode);
    Rval |= Ctfw_ConnectDownCheck(pUpStreamNode, pDownStreamNode);

    return Rval;
}

UINT32 Ctfw_TaskDisconnect_TF(CtfwTaskHandle_t *pUpStreamNode, CtfwTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;

    Rval |= Ctfw_CmdDisconnectStream(pDownStreamNode, pUpStreamNode, TO_UP_STREAM);
    Rval |= Ctfw_DisconnectCheck(pDownStreamNode, pUpStreamNode, TO_UP_STREAM);
    Rval |= Ctfw_CmdDisconnectStream(pUpStreamNode, pDownStreamNode, TO_DOWN_STREAM);
    Rval |= Ctfw_DisconnectCheck(pUpStreamNode, pDownStreamNode, TO_DOWN_STREAM);

    return Rval;
}

UINT32 Ctfw_TaskCheckConnection(const CtfwTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    UINT32 TaskIdx;

    ArmLog_DBG(Ctfw_GetTaskName(pHandle), "Number of upstream node: (%d)", pHandle->UpStreamTaskNum, 0U);
    for (TaskIdx = 0U; TaskIdx < pHandle->UpStreamTaskNum; TaskIdx++) {
        ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Up index(%d)", TaskIdx, 0U);
        ArmLog_STR(Ctfw_GetTaskName(pHandle), "## %s", Ctfw_GetTaskName(pHandle->pUpStreamList[TaskIdx]), NULL);
    }

    ArmLog_DBG(Ctfw_GetTaskName(pHandle), "Number of downstream node: (%d)", pHandle->DownStreamTaskNum, 0U);
    for (TaskIdx = 0U; TaskIdx < pHandle->DownStreamTaskNum; TaskIdx++) {
        ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Down index(%d)", TaskIdx, 0U);
        ArmLog_STR(Ctfw_GetTaskName(pHandle), "## %s", Ctfw_GetTaskName(pHandle->pDownStreamList[TaskIdx]), NULL);
    }

    return Rval;
}

UINT32 Ctfw_WhosCall(const CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pFromHandle, UINT32 *pIndex, UINT32 FromUpDown)
{
    UINT32 Rval = ARM_NG;
    UINT32 TaskIdx;

    if (FromUpDown == FROM_UP_STREAM) {

        for (TaskIdx = 0U; TaskIdx < pHandle->UpStreamTaskNum; TaskIdx++) {
            if (pHandle->pUpStreamList[TaskIdx] == pFromHandle) {
                *pIndex = TaskIdx;
                Rval = ARM_OK;
                break;
            }
        }
        if(Rval == ARM_NG) {
            ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Cannot find %s in %s's upstream list", pFromHandle->CtfwResourceName.Task, pHandle->CtfwResourceName.Task);
        }
    } else if (FromUpDown == FROM_DOWN_STREAM) {

        for (TaskIdx = 0U; TaskIdx < pHandle->DownStreamTaskNum; TaskIdx++) {
            if (pHandle->pDownStreamList[TaskIdx] == pFromHandle) {
                *pIndex = TaskIdx;
                Rval = ARM_OK;
                break;
            }
        }
        if(Rval == ARM_NG) {
            ArmLog_STR(Ctfw_GetTaskName(pHandle), "## Cannot find %s in %s's downstream list", pFromHandle->CtfwResourceName.Task, pHandle->CtfwResourceName.Task);
        }
    } else {
        //
    }

    return Rval;
}

UINT32 Ctfw_ProfilerGetTime(UINT64 *pTimeUs)
{
    UINT32 Rval = ARM_OK;
    //struct  timeval Tv;
    UINT32 SysTickCount;
    //UINT64 Diff;

    //Rval =gettimeofday(&Tv,NULL);
    //*pTimeUs = Tv.tv_sec * 1000000U + Tv.tv_usec;

    Rval |= AmbaKAL_GetSysTickCount(&SysTickCount);
    *pTimeUs = SysTickCount;
    return Rval;
}

UINT32 Ctfw_ProfilerTimerReset(CtfwTaskHandle_t *pHandle, UINT32 ProfileId)
{
    UINT32 Rval = ARM_OK;
    CtfwProfiler_t *pProfiler = &(pHandle->Profiler[ProfileId]);

    pProfiler->StartTimeUs = 0U;
    pProfiler->AccuCount = 0U;
    pProfiler->AccuTimeUs = 0U;
    pProfiler->MaxTimeDiff = 0U;
    pProfiler->MinTimeDiff = 0xFFFFFFFFU;
    return Rval;
}

UINT32 Ctfw_ProfilerStart(CtfwTaskHandle_t *pHandle, UINT32 ProfileId)
{
    UINT32 Rval = ARM_OK;
    CtfwProfiler_t *pProfiler = &(pHandle->Profiler[ProfileId]);

    Rval |= Ctfw_ProfilerGetTime(&(pProfiler->StartTimeUs));

    //ArmLog_STR(TASKNAME, "## Profiler[%d] START %10llu us", ProfileId, pProfiler->StartTimeUs);

    return Rval;
}

UINT32 Ctfw_ProfilerStop(CtfwTaskHandle_t *pHandle, UINT32 ProcStat, UINT32 ProfileId)
{
    UINT32 Rval = ARM_OK;
    UINT64 End;
    UINT64 Diff;
    CtfwProfiler_t *pProfiler = &(pHandle->Profiler[ProfileId]);

    Rval |= Ctfw_ProfilerGetTime(&End);

    Diff = End - pProfiler->StartTimeUs;
    pProfiler->AccuTimeUs += Diff;
    pProfiler->AccuCount += 1U;

    if(Diff > pProfiler->MaxTimeDiff) {
        pProfiler->MaxTimeDiff = (UINT32)Diff;
    }

    if(Diff < pProfiler->MinTimeDiff) {
        pProfiler->MinTimeDiff = (UINT32)Diff;
    }

    if ( ProcStat != ARM_OK ) {
        pProfiler->ErrorCount += 1U;
    }

    //ArmLog_STR(TASKNAME, "## Profiler[%d] END %10llu, DIFF %10llu us", ProfileId, End, Diff);

    return Rval;
}

UINT32 Ctfw_ProfilerTimerReport(const CtfwTaskHandle_t *pHandle, UINT32 ProfileId)
{
    UINT32 Rval = ARM_OK;
    UINT64 Average;
    const CtfwProfiler_t *pProfiler = &(pHandle->Profiler[ProfileId]);

    if (pProfiler->AccuCount > 0U) {
        Average = pProfiler->AccuTimeUs / pProfiler->AccuCount;
        ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Profile[%d]: Average     %d", ProfileId, (UINT32)Average);
        ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Profile[%d]: MaxTimeDiff %d", ProfileId, (UINT32)pProfiler->MaxTimeDiff);
        ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Profile[%d]: MinTimeDiff %d", ProfileId, (UINT32)pProfiler->MinTimeDiff);
        ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Profile[%d]: AccuCount   %d", ProfileId, (UINT32)pProfiler->AccuCount);
        ArmLog_DBG(Ctfw_GetTaskName(pHandle), "## Profile[%d]: ErrorCount  %d", ProfileId, (UINT32)pProfiler->ErrorCount);
    }

    return Rval;
}

UINT32 Ctfw_ShiftFlexiMemblkBuff(const flexidag_memblk_t *pIn, ULONG ShiftSize, flexidag_memblk_t *pOut)
{
    UINT32 Rval = ARM_OK;
    UINT32 ShiftSizeU32 = (UINT32)ShiftSize;
    ULONG AlignedShiftSize = CTFW_CV_MEM_ALIGN(ShiftSizeU32);

    if ( AlignedShiftSize != ShiftSize ) {
        Rval = ARM_NG;
    }

    if ( Rval == ARM_OK ) {

        *pOut = *pIn;
        pOut->pBuffer = &pOut->pBuffer[ShiftSize];
        pOut->buffer_daddr = pOut->buffer_daddr + ShiftSize;
        pOut->buffer_caddr = pOut->buffer_caddr + ShiftSize;

        pOut->buffer_size -= (UINT32)ShiftSize;
    }

    return Rval;
}

UINT32 Ctfw_GetSourceFramePoolSize(UINT32 SrcDataSize, UINT32 PoolFrameNum, UINT32 *pFramePoolSize)
{
    UINT32 Rval = ARM_OK;
    UINT32 SingleFrameSize = 0U;

    SingleFrameSize += CTFW_CV_MEM_ALIGN(SrcDataSize);
    SingleFrameSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(CtfwFrameMetadata_t));

    *pFramePoolSize = SingleFrameSize*PoolFrameNum;

    return Rval;
}

extern uint64_t ambacv_p2c(uint64_t pa);

UINT32 Cftw_MemPool_Allocate(UINT32 PoolId, UINT32 ReqSize, flexidag_memblk_t *pOutBuf)
{
    UINT32 RetVal = ARM_OK;
    UINT64 CAddr;

    // Temp workaround, this API doesn't set caddr
    RetVal |= ArmMemPool_Allocate(PoolId, ReqSize, pOutBuf);
    CAddr = ambacv_p2c(pOutBuf->buffer_daddr);
    pOutBuf->buffer_caddr = (ULONG)CAddr;
    if(pOutBuf->buffer_caddr == 0U) {
        ArmLog_ERR("Ctfw_TaskUtil", "ambacv_p2c fail", 0U, 0U);
        RetVal |= ARM_NG;
    }
    return RetVal;
}

UINT32 Ctfw_AllocSourceFramePool(UINT32 SrcDataSize, UINT32 MemPoolID, CtfwFrame_t FramePool[], UINT32 PoolFrameNum)
{
    UINT32 Rval = ARM_OK;
    UINT32 BuffIndex;

    for (BuffIndex = 0U; BuffIndex < PoolFrameNum; BuffIndex++) {
        if ( SrcDataSize != 0U ) {
            Rval |= Cftw_MemPool_Allocate(MemPoolID, SrcDataSize, &FramePool[BuffIndex].CtfwData);
        }
        Rval |= Cftw_MemPool_Allocate(MemPoolID, (UINT32)sizeof(CtfwFrameMetadata_t), &FramePool[BuffIndex].MetaData);
        if ( Rval != ARM_OK ) {
            ArmLog_ERR("Ctfw_TaskUtil", "Allocate source data [%d] fail", BuffIndex, 0U);
        }
    }

    return Rval;
}

