/**
 *  @file AmbaStu_AlgoTaskUtil.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Provide utilities for stereo algo task
 *
 */

#include "SvcCvAlgo.h"
#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaMisraFix.h"


#define TASKNAME &(pHandle->AppAcResourceName.Task[0])

char* AppAc_GetTaskName(AppAcTaskHandle_t *pHandle)
{
    return &(pHandle->AppAcResourceName.Task[0]);
}

UINT32 AppAc_ProcUpStreamCb(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;
    //UINT32 IsFull;
    AppAcDesc_t *pDesc = &(pMsg->Desc);
    UINT32 Index = 0U;

    //find who callbacks
    Rval = AppAc_WhosCall(pHandle, pMsg->Desc.pFromHandle, &Index, FROM_UP_STREAM);


    //ArmLog_STR(TASKNAME, "## ProcUpStreamCb");
    if(pDesc->FrameNum % pHandle->ProcInterval == 0U) {
        Rval = AppAc_BufferDescPush(pHandle, &(pHandle->InputBuffer[Index]), pDesc); //content is AMBA_CV_IMG_BUF_s
        if(Rval != ARM_OK) {
            //ArmLog_ERR(TASKNAME, "## ProcUpStreamCb : input buffer[Index=%d] full!!", Index);
        }
    } else {
        // discard this frame, callback upstream
        AppAcCbMsg_t Msg;

        ArmStdC_memcpy(&(Msg.Desc), pDesc, sizeof(AppAcDesc_t));
        Msg.Desc.pToHandle = Msg.Desc.pFromHandle;
        Msg.Desc.pFromHandle = pHandle;
        Msg.Desc.IsLastFrame = 0U;
        Msg.pPrivate = NULL;
        pDesc->pFromHandle->DownStreamCallBack(&Msg);
    }


    return Rval;
}

UINT32 AppAc_CallbackDownStream(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AppAcCbMsg_t Msg;
    UINT32 I;

    for (I = 0U; I < MAX_DOWN_STREAM_NODES; I++) {
        if (pHandle->pDownStreamList[I] == NULL) {
            continue;
        } else if (pHandle->pDownStreamList[I]->UpStreamCallBack != NULL) {
            UINT32 Index;

            // get the latest pushed desc
            if (pHandle->OutputBuffer[I].WriteIdx == 0U) {
                Index = pHandle->OutputBuffer[I].Depth - 1U;
            } else {
                Index = (pHandle->OutputBuffer[I].WriteIdx - 1U);
            }

            ArmStdC_memcpy(&(Msg.Desc), &(pHandle->OutputBuffer[I].Desc[Index]), sizeof(AppAcDesc_t));
            Msg.Desc.pFromHandle = pHandle;
            Msg.Desc.pToHandle = pHandle->pDownStreamList[I];
            Msg.Desc.IsLastFrame = 0U;
            Msg.pPrivate = NULL;

//            ArmLog_STR(TASKNAME, "## call %s", Msg.pDesc->pToHandle->AppAcResourceName.Task);

            pHandle->pDownStreamList[I]->UpStreamCallBack(&Msg);
        }
    }

    //ArmLog_STR(TASKNAME, "## CallbackDownStream finished", I);
    return Rval;
}

UINT32 AppAc_CallbackUpStream(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AppAcCbMsg_t Msg;
    UINT32 I;

    for (I = 0U; I < MAX_UP_STREAM_NODES; I++) {
        if (pHandle->pUpStreamList[I] == NULL) {
            continue;
        } else if (pHandle->pUpStreamList[I]->DownStreamCallBack != NULL) {
            ArmStdC_memcpy(&(Msg.Desc), &(pHandle->InputBuffer[I].Desc[pHandle->InputBuffer[I].ReadIdx]), sizeof(AppAcDesc_t));
            Msg.Desc.pFromHandle = pHandle;
            Msg.Desc.pToHandle = pHandle->pUpStreamList[I];
            Msg.Desc.IsLastFrame = 0U;
            Msg.pPrivate = NULL;

//            ArmLog_STR(TASKNAME, "## call %s", Msg.pDesc->pToHandle->AppAcResourceName.Task);

            pHandle->pUpStreamList[I]->DownStreamCallBack(&Msg);
        }
    }


    //ArmLog_STR(TASKNAME, "## CallbackUpStream finished", I);
    return Rval;
}

UINT32 AppAc_BufferInit(AppAcTaskHandle_t *pHandle, UINT32 Depth, UINT32 OutBufSize)

{
    UINT32 Rval = ARM_OK;
    UINT32 i;
    ArmLog_STR(TASKNAME, "## create input buffer", NULL, NULL);

    for (i = 0U; i < MAX_UP_STREAM_NODES; i++) {
        pHandle->InputBuffer[i].Depth = Depth;
        pHandle->InputBuffer[i].ReadIdx = 0U;
        pHandle->InputBuffer[i].WriteIdx = 0U;
    }

    // create output buffer
    ArmLog_STR(TASKNAME, "## create output buffer", NULL, NULL);

    for (i = 0U; i < MAX_DOWN_STREAM_NODES; i++) {
        pHandle->OutputBuffer[i].Depth = Depth;
        pHandle->OutputBuffer[i].ReadIdx = 0U;
        pHandle->OutputBuffer[i].WriteIdx = 0U;
    }
    for (UINT32 I = 0U; I < Depth; I++) {
        Rval = ArmMemPool_Allocate(pHandle->MemPoolId, OutBufSize, &(pHandle->OutBuf[I]));
        //ArmLog_STR(TASKNAME, "## create pHandle->OutBuf[%d]=%x, size=%d", I, pHandle->OutBuf[I].pBuffer, pHandle->OutBuf[I].buffer_size);
    }

    return Rval;
}

UINT32 AppAc_BufferDeinit(AppAcTaskHandle_t *pHandle, UINT32 Depth)
{
    UINT32 Rval = ARM_OK;
    UINT32 i;
    ArmLog_STR(TASKNAME, "## create input buffer", NULL, NULL);

    for (i = 0U; i < MAX_UP_STREAM_NODES; i++) {
        pHandle->InputBuffer[i].Depth = Depth;
        pHandle->InputBuffer[i].ReadIdx = 0U;
        pHandle->InputBuffer[i].WriteIdx = 0U;
    }

    // create output buffer
    ArmLog_STR(TASKNAME, "## create output buffer", NULL, NULL);

    for (i = 0U; i < MAX_DOWN_STREAM_NODES; i++) {
        pHandle->OutputBuffer[i].Depth = Depth;
        pHandle->OutputBuffer[i].ReadIdx = 0U;
        pHandle->OutputBuffer[i].WriteIdx = 0U;
    }
    for (UINT32 I = 0U; I < Depth; I++) {
        Rval = ArmMemPool_Free(pHandle->MemPoolId, &(pHandle->OutBuf[I]));
        //ArmLog_STR(TASKNAME, "## create pHandle->OutBuf[%d]=%x, size=%d", I, pHandle->OutBuf[I].pBuffer, pHandle->OutBuf[I].buffer_size);
    }

    return Rval;
}

UINT32 AppAc_IsBufferEmpty(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf, UINT32 *pIsEmpty)
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

UINT32 AppAc_IsBufferFull(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf, UINT32 *pIsFull)
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

UINT32 AppAc_BufferUpwp(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf)
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

UINT32 AppAc_BufferUprp(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf)
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

UINT32 AppAc_BufferUprpToAll(AppAcTaskHandle_t *pHandle, UINT32 ToUpDown)
{
    UINT32 Rval = ARM_OK;
    //UINT32 IsEmpty;
    //UINT32 IsFull;
    UINT32 i;
    AppAcBuffer_t *pBuf;

    if (ToUpDown == TO_UP_STREAM) {

        for (i = 0; i < MAX_UP_STREAM_NODES; i++) {
            if (pHandle->pUpStreamList[i] == NULL) {
                continue;
            }
            pBuf = &(pHandle->InputBuffer[i]);
            Rval = AppAc_BufferUprp(pHandle, pBuf);
        }

    } else if (ToUpDown == TO_DOWN_STREAM) {

        for (i = 0; i < MAX_DOWN_STREAM_NODES; i++) {
            if (pHandle->pDownStreamList[i] == NULL) {
                continue;
            }
            pBuf = &(pHandle->OutputBuffer[i]);
            Rval = AppAc_BufferUprp(pHandle, pBuf);
        }

    } else {
        //
    }

    return Rval;
}

UINT32 AppAc_BufferUpwpToAll(AppAcTaskHandle_t *pHandle, UINT32 ToUpDown)
{
    UINT32 Rval = ARM_OK;
    //UINT32 IsEmpty;
    //UINT32 IsFull;
    UINT32 i;
    AppAcBuffer_t *pBuf;

    if (ToUpDown == TO_UP_STREAM) {

        for (i = 0; i < MAX_UP_STREAM_NODES; i++) {
            if (pHandle->pUpStreamList[i] == NULL) {
                continue;
            }
            pBuf = &(pHandle->InputBuffer[i]);
            Rval = AppAc_BufferUpwp(pHandle, pBuf);
        }

    } else if (ToUpDown == TO_DOWN_STREAM) {

        for (i = 0; i < MAX_DOWN_STREAM_NODES; i++) {
            if (pHandle->pDownStreamList[i] == NULL) {
                continue;
            }
            pBuf = &(pHandle->OutputBuffer[i]);
            Rval = AppAc_BufferUpwp(pHandle, pBuf);
        }

    } else {
        //
    }

    return Rval;
}

UINT32 AppAc_BufferDescPush(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf, AppAcDesc_t *pDesc)
{
    UINT32 Rval = ARM_OK;
    UINT32 IsFull;

    if(pBuf == NULL) {
        Rval = ARM_NG;
    }
    (void) AppAc_IsBufferFull(pHandle, pBuf, &IsFull);
    if(IsFull == 1U) {
        ArmLog_ERR(TASKNAME, "## AppAc_BufferDescPush : buffer full!!", 0U, 0U);
        Rval = ARM_NG;
    } else {

        ArmStdC_memcpy(&(pBuf->Desc[pBuf->WriteIdx]), pDesc, sizeof(AppAcDesc_t));
        //ArmLog_STR(TASKNAME, "## Got FrameNum=%d", pBuf->Desc[pBuf->WriteIdx].FrameNum);
        AppAc_BufferUpwp(pHandle, pBuf);
    }

    return Rval;
}

UINT32 AppAc_BufferDescPushToAllDownStream(AppAcTaskHandle_t *pHandle, AppAcDesc_t *pAppAcDesc)
{
    UINT32 Rval = ARM_OK;
    //UINT32 IsEmpty;
    //UINT32 IsFull;
    UINT32 i;
    AppAcBuffer_t *pBuf;
    AppAcDesc_t AppAcDesc;

    AppAcDesc = *pAppAcDesc;
    // scan all hooked output are not full
    if (Rval == ARM_OK) {
        for (i = 0; i < MAX_DOWN_STREAM_NODES; i++) {
            if (pHandle->pDownStreamList[i] == NULL) {
                continue;
            }
            //ArmLog_STR(TASKNAME, "## pushing DownStreamList[%d]", i);
            pBuf = &(pHandle->OutputBuffer[i]);
            AppAcDesc.pToHandle = pHandle->pDownStreamList[i];
            Rval |= AppAc_BufferDescPush(pHandle, pBuf, &AppAcDesc);
        }
    }

    return Rval;
}

UINT32 AppAc_CheckInOutReady(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    UINT32 IsEmpty;
    UINT32 IsFull;
    UINT32 i;

    // scan all hooked input are not empty
    Rval = ARM_NG;
    for (i = 0; i < MAX_UP_STREAM_NODES; i++) {
        AppAcBuffer_t *pBuf;

        if (pHandle->pUpStreamList[i] == NULL) {
            continue;
        }

        pBuf = &(pHandle->InputBuffer[i]);
        //ArmLog_STR(TASKNAME, "## check InputBuffer[%d] rp=%d, wp=%d", i, pBuf->ReadIdx, pBuf->WriteIdx);
        Rval = AppAc_IsBufferEmpty(pHandle, pBuf, &IsEmpty);
        if (IsEmpty == 1U) {
            Rval = ARM_NG;
            break;
        } else {
            Rval = ARM_OK;
        }
    }

    // scan all hooked output are not full
    if (Rval == ARM_OK) {
        Rval = ARM_NG;
        for (i = 0; i < MAX_DOWN_STREAM_NODES; i++) {
            AppAcBuffer_t *pBuf;

            if (pHandle->pDownStreamList[i] == NULL) {
                continue;
            }

            pBuf = &(pHandle->OutputBuffer[i]);
            //ArmLog_STR(TASKNAME, "## check OutputBuffer[%d] rp=%d, wp=%d", i, pBuf->ReadIdx, pBuf->WriteIdx);
            Rval = AppAc_IsBufferFull(pHandle, pBuf, &IsFull);
            if (IsFull == 1U) {
                Rval = ARM_NG;
                break;
            } else {
                Rval = ARM_OK;
            }
        }
    }

//    if(Rval == ARM_OK) {
//        ArmLog_STR(TASKNAME, "## Input and output are OK");
//    } else {
//        ArmLog_STR(TASKNAME, "## Input and output are not ready");
//    }
    return Rval;
}

UINT32 AppAc_WaitState(AppAcTaskHandle_t *pHandle, UINT32 State)
{
    ArmMutex_Take(&pHandle->Mutex);
    while (pHandle->CurrState != State) {
        usleep(10000);
    }
    ArmMutex_Give(&pHandle->Mutex);

    return ARM_OK;
}

UINT32 AppAc_MsgQueueCreate(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    Rval = ArmMsgQueue_Create(&(pHandle->TaskMsgQ), &(pHandle->AppAcResourceName.MsgQ[0]), sizeof(AppAcTaskMsg_t), 16U, NULL);
    return Rval;
}

UINT32 AppAc_MsgQueueDelete(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    Rval = ArmMsgQueue_Delete(&(pHandle->TaskMsgQ), &(pHandle->AppAcResourceName.MsgQ[0]));
    return Rval;
}


UINT32 AppAc_MsgQueueSend(AppAcTaskHandle_t *pHandle, UINT32 Cmd, AppAcCbMsg_t *pCbMsg)
{
    UINT32 Rval = ARM_OK;
    AppAcTaskMsg_t Msg;

    ArmStdC_memset(&Msg, 0U, sizeof(Msg));
    Msg.Cmd = Cmd;
    if (pCbMsg != NULL) {
        Msg.Desc = pCbMsg->Desc;
        Msg.pPrivate = pCbMsg->pPrivate;
    }

    ArmMsgQueue_Send(&(pHandle->TaskMsgQ), &Msg);

    return Rval;
}


UINT32 AppAc_CmdConnectUpStream(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pUpStreamNode)
{
    UINT32 Rval = ARM_OK;
    AppAcCbMsg_t CbMsg;

    ArmLog_STR(AppAc_GetTaskName(pHandle), "## Send upstream node %s", AppAc_GetTaskName(pUpStreamNode), NULL);

    CbMsg.pPrivate = pUpStreamNode;
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_CONNECT_UP_STREAM, &CbMsg);

    return Rval;
}

/*
 * UpDown: 0: up, 1: down
 */
UINT32 AppAc_CmdDisconnectStream(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pNode, UINT32 UpDown)
{
    UINT32 Rval = ARM_OK;
    AppAcCbMsg_t CbMsg;

    CbMsg.pPrivate = pNode;

    if (UpDown == TO_UP_STREAM) {
        ArmLog_STR(AppAc_GetTaskName(pHandle), "## send upstream disconnect, node %s", AppAc_GetTaskName(pNode), NULL);
        AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_DISCONNECT_UP_STREAM, &CbMsg);
    } else {
        ArmLog_STR(AppAc_GetTaskName(pHandle), "## send downstream disconnect, node %s", AppAc_GetTaskName(pNode), NULL);
        AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_DISCONNECT_DOWN_STREAM, &CbMsg);
    }

    return Rval;
}


UINT32 AppAc_CmdConnectDownStream(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;
    AppAcCbMsg_t CbMsg;

    ArmLog_STR(AppAc_GetTaskName(pHandle), "## Send downstream node %s", AppAc_GetTaskName(pDownStreamNode), NULL);

    CbMsg.pPrivate = pDownStreamNode;
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_CONNECT_DOWN_STREAM, &CbMsg);

    return Rval;
}

UINT32 AppAc_InsertDownStreamList(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;
    UINT32 I;

    for (I = 0U; I < MAX_DOWN_STREAM_NODES; I++) {
        if (pHandle->pDownStreamList[I] == NULL) {
            pHandle->pDownStreamList[I] = pDownStreamNode;
            ArmLog_DBG(AppAc_GetTaskName(pHandle), "## DownStreamList[%d] is available", I, 0U);
            ArmLog_STR(AppAc_GetTaskName(pHandle), "## Hooked downstream node %s", AppAc_GetTaskName(pDownStreamNode), NULL);
            break;
        }
    }
    if (I == MAX_DOWN_STREAM_NODES) {
        //error, list is full
        Rval = ARM_NG;
    }

    return Rval;
}


UINT32 AppAc_InsertUpStreamList(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pUpStreamNode)
{
    UINT32 Rval = ARM_OK;
    UINT32 I;

    if (Rval == ARM_OK) {
        for (I = 0U; I < MAX_UP_STREAM_NODES; I++) {
            if (pHandle->pUpStreamList[I] == NULL) {
                pHandle->pUpStreamList[I] = pUpStreamNode;
                ArmLog_DBG(AppAc_GetTaskName(pHandle), "## UpStreamList[%d] is available", I, 0U);
                ArmLog_STR(AppAc_GetTaskName(pHandle), "## Hooked upstream node %s", AppAc_GetTaskName(pUpStreamNode), NULL);
                break;
            }
        }
        if (I == MAX_UP_STREAM_NODES) {
            //error, list is full
            Rval = ARM_NG;
        }
    }

    return Rval;
}

UINT32 AppAc_DeleteStreamList(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pNode, UINT32 UpDown)
{
    UINT32 Rval = ARM_OK;
    UINT32 I;

    if (UpDown == TO_UP_STREAM) {
        for (I = 0U; I < MAX_UP_STREAM_NODES; I++) {
            if (pHandle->pUpStreamList[I] == pNode) {
                pHandle->pUpStreamList[I] = NULL;
                ArmLog_DBG(AppAc_GetTaskName(pHandle), "## found target upstream node at UpStreamList[%d]", I, 0U);
                ArmLog_STR(AppAc_GetTaskName(pHandle), "## delete upstream node %s", AppAc_GetTaskName(pNode), NULL);
                break;
            }
        }
        if (I == MAX_UP_STREAM_NODES) {
            //error, list is full
            Rval = ARM_NG;
        }
    } else {
        for (I = 0U; I < MAX_DOWN_STREAM_NODES; I++) {
            if (pHandle->pDownStreamList[I] == pNode) {
                pHandle->pDownStreamList[I] = NULL;
                ArmLog_DBG(AppAc_GetTaskName(pHandle), "## found target downstream node at DownStreamList[%d]", I, 0U);
                ArmLog_STR(AppAc_GetTaskName(pHandle), "## delete downstream node %s", AppAc_GetTaskName(pNode), NULL);
                break;
            }
        }
        if (I == MAX_DOWN_STREAM_NODES) {
            //error, list is full
            Rval = ARM_NG;
        }
    }
    return Rval;
}


UINT32 AppAc_TaskConnect(AppAcTaskHandle_t *pUpStreamNode, AppAcTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;
    //UINT32 I;

    AppAc_InsertUpStreamList(pDownStreamNode, pUpStreamNode);
    AppAc_InsertDownStreamList(pUpStreamNode, pDownStreamNode);

    // Hook upstream to downstream

//    if (Rval == ARM_OK) {
//        for (I = 0U; I < MAX_UP_STREAM_NODES; I++) {
//            if (pDownStreamNode->pUpStreamList[I] == NULL) {
//                pDownStreamNode->pUpStreamList[I] = pUpStreamNode;
//                //ArmLog_STR("AppAc", "## Hooked upstream node %s(%x) to downstream node %s(%x) at List[%d]", AppAc_GetTaskName(pUpStreamNode), pUpStreamNode, AppAc_GetTaskName(pDownStreamNode), pDownStreamNode, I);
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
//                //ArmLog_STR("AppAc", "## Hooked downstream node %s(%x) to upstream node %s(%x) at List[%d]", AppAc_GetTaskName(pDownStreamNode), pDownStreamNode, AppAc_GetTaskName(pUpStreamNode), pUpStreamNode, I);
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

UINT32 AppAc_ConnectUpCheck(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pUpStreamNode)
{
    UINT32 Rval = ARM_NG;
    UINT32 I;
    UINT32 Found = 0U;

    while(Found == 0U) {
        for (I = 0U; I < MAX_UP_STREAM_NODES; I++) {
            if (pHandle->pUpStreamList[I] == pUpStreamNode) {
                Found = 1U;
                Rval = ARM_OK;
                ArmLog_DBG(AppAc_GetTaskName(pHandle), "## Found target upstream node at UpStreamList[%d]", I, 0U);
                ArmLog_STR(AppAc_GetTaskName(pHandle), "## Checked upstream node %s", AppAc_GetTaskName(pUpStreamNode), NULL);
                break;
            }
        }
        usleep(30000);
    }

    return Rval;
}

UINT32 AppAc_ConnectDownCheck(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_NG;
    UINT32 I;
    UINT32 Found = 0U;

    while(Found == 0U) {
        for (I = 0U; I < MAX_UP_STREAM_NODES; I++) {
            if (pHandle->pDownStreamList[I] == pDownStreamNode) {
                Found = 1U;
                Rval = ARM_OK;
                ArmLog_DBG(AppAc_GetTaskName(pHandle), "## Found target downstream node at DownStreamList[%d]", I, 0U);
                ArmLog_STR(AppAc_GetTaskName(pHandle), "## Checked downstream node %s", AppAc_GetTaskName(pDownStreamNode), NULL);
                break;
            }
        }
        usleep(30000);
    }

    return Rval;
}

UINT32 AppAc_DisconnectCheck(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pNode, UINT32 UpDown)
{
    UINT32 Rval = ARM_OK;
    UINT32 I;
    UINT32 Found = 0U;

    if (UpDown == TO_UP_STREAM) {
        while (Found == 1U) {
            for (I = 0U; I < MAX_UP_STREAM_NODES; I++) {
                if (pHandle->pUpStreamList[I] == pNode) {
                    Found = 1U;
                    ArmLog_DBG(AppAc_GetTaskName(pHandle), "## Found target upstream node at UpStreamList[%d]", I, 0U);
                    ArmLog_STR(AppAc_GetTaskName(pHandle), "## Found upstream node %s", AppAc_GetTaskName(pNode), NULL);
                    break;
                } else {
                    Found = 0U;
                    ArmLog_STR(AppAc_GetTaskName(pHandle), "## Cannot find upstream node %s in UpStreamList", AppAc_GetTaskName(pNode), NULL);
                }
            }
            usleep(30000);
        }
    } else {
        while (Found == 1U) {
            for (I = 0U; I < MAX_DOWN_STREAM_NODES; I++) {
                if (pHandle->pDownStreamList[I] == pNode) {
                    Found = 1U;
                    ArmLog_DBG(AppAc_GetTaskName(pHandle), "## Found target downstream node at DownStreamList[%d]", I, 0U);
                    ArmLog_STR(AppAc_GetTaskName(pHandle), "## Found downstream node %s", AppAc_GetTaskName(pNode), NULL);
                    break;
                } else {
                    Found = 0U;
                    ArmLog_STR(AppAc_GetTaskName(pHandle), "## Cannot find downstream node %s in DownStreamList", AppAc_GetTaskName(pNode), NULL);
                }
            }
            usleep(30000);
        }
    }


    return Rval;
}


UINT32 AppAc_TaskConnect_TF(AppAcTaskHandle_t *pUpStreamNode, AppAcTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;

    Rval |= AppAc_CmdConnectUpStream(pDownStreamNode, pUpStreamNode);
    Rval |= AppAc_ConnectUpCheck(pDownStreamNode, pUpStreamNode);
    Rval |= AppAc_CmdConnectDownStream(pUpStreamNode, pDownStreamNode);
    Rval |= AppAc_ConnectDownCheck(pUpStreamNode, pDownStreamNode);

    return Rval;
}

UINT32 AppAc_TaskDisconnect_TF(AppAcTaskHandle_t *pUpStreamNode, AppAcTaskHandle_t *pDownStreamNode)
{
    UINT32 Rval = ARM_OK;

    Rval |= AppAc_CmdDisconnectStream(pDownStreamNode, pUpStreamNode, TO_UP_STREAM);
    Rval |= AppAc_DisconnectCheck(pDownStreamNode, pUpStreamNode, TO_UP_STREAM);
    Rval |= AppAc_CmdDisconnectStream(pUpStreamNode, pDownStreamNode, TO_DOWN_STREAM);
    Rval |= AppAc_DisconnectCheck(pUpStreamNode, pDownStreamNode, TO_DOWN_STREAM);

    return Rval;
}


UINT32 AppAc_WhosCall(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pFromHandle, UINT32 *pIndex, UINT32 FromUpDown)
{
    UINT32 Rval = ARM_NG;
    UINT32 i;

    if (FromUpDown == FROM_UP_STREAM) {

        for (i = 0; i < MAX_UP_STREAM_NODES; i++) {
            if (pHandle->pUpStreamList[i] == pFromHandle) {
                *pIndex = i;
                Rval = ARM_OK;
                break;
            }
        }
        if(Rval == ARM_NG) {
//            ArmLog_STR(TASKNAME, "## Cannot find %s in %s's upstream list", pFromHandle->AppAcResourceName.Task, pHandle->AppAcResourceName.Task);
        }
    } else if (FromUpDown == FROM_DOWN_STREAM) {

        for (i = 0; i < MAX_DOWN_STREAM_NODES; i++) {
            if (pHandle->pDownStreamList[i] == pFromHandle) {
                *pIndex = i;
                Rval = ARM_OK;
                break;
            }
        }
        if(Rval == ARM_NG) {
//            ArmLog_STR(TASKNAME, "## Cannot find %s in %s's downstream list", pFromHandle->AppAcResourceName.Task, pHandle->AppAcResourceName.Task);
        }
    } else {
        //
    }



    return Rval;
}

UINT32 AppAc_ProfilerGetTime(UINT64 *pTimeUs)
{
    UINT32 Rval = ARM_OK;
    struct  timeval Tv;
    //UINT64 Diff;

    Rval =gettimeofday(&Tv,NULL);
    *pTimeUs = Tv.tv_sec * 1000000U + Tv.tv_usec;

    return Rval;
}

UINT32 AppAc_ProfilerTimerReset(AppAcTaskHandle_t *pHandle, UINT32 ProfileId)
{
    UINT32 Rval = ARM_OK;
    AppAcProfiler_t *pProfiler = &(pHandle->Profiler[ProfileId]);

    pProfiler->StartTimeUs = 0U;
    pProfiler->AccuCount = 0U;
    pProfiler->AccuTimeUs = 0U;
    pProfiler->MaxTimeDiff = 0U;
    pProfiler->MinTimeDiff = 0xFFFFFFFFU;
    return Rval;
}

UINT32 AppAc_ProfilerTimerStart(AppAcTaskHandle_t *pHandle, UINT32 ProfileId)
{
    UINT32 Rval = ARM_OK;
    AppAcProfiler_t *pProfiler = &(pHandle->Profiler[ProfileId]);

    AppAc_ProfilerGetTime(&(pProfiler->StartTimeUs));

    //ArmLog_STR(TASKNAME, "## Profiler[%d] START %10llu us", ProfileId, pProfiler->StartTimeUs);

    return Rval;
}

UINT32 AppAc_ProfilerTimerStop(AppAcTaskHandle_t *pHandle, UINT32 ProfileId)
{
    UINT32 Rval = ARM_OK;
    UINT64 End;
    UINT64 Diff;
    AppAcProfiler_t *pProfiler = &(pHandle->Profiler[ProfileId]);

    AppAc_ProfilerGetTime(&End);

    Diff = End - pProfiler->StartTimeUs;
    pProfiler->AccuTimeUs += Diff;
    pProfiler->AccuCount += 1U;

    if(Diff > pProfiler->MaxTimeDiff) {
        pProfiler->MaxTimeDiff = Diff;
    }

    if(Diff < pProfiler->MinTimeDiff) {
        pProfiler->MinTimeDiff = Diff;
    }

    //ArmLog_STR(TASKNAME, "## Profiler[%d] END %10llu, DIFF %10llu us", ProfileId, End, Diff);

    return Rval;
}

UINT32 AppAc_ProfilerTimerReport(AppAcTaskHandle_t *pHandle, UINT32 ProfileId)
{
    UINT32 Rval = ARM_OK;
    UINT64 Average;
    AppAcProfiler_t *pProfiler = &(pHandle->Profiler[ProfileId]);

    if (pProfiler->AccuCount > 0U) {
        Average = pProfiler->AccuTimeUs / pProfiler->AccuCount;
        ArmLog_DBG(TASKNAME, "## Profile[%d]: Average     %d", ProfileId, (UINT32)Average);
        ArmLog_DBG(TASKNAME, "## Profile[%d]: MaxTimeDiff %d", ProfileId, (UINT32)pProfiler->MaxTimeDiff);
        ArmLog_DBG(TASKNAME, "## Profile[%d]: MinTimeDiff %d", ProfileId, (UINT32)pProfiler->MinTimeDiff);
        ArmLog_DBG(TASKNAME, "## Profile[%d]: AccuCount   %d", ProfileId, (UINT32)pProfiler->AccuCount);
    }
    return Rval;
}

UINT32 AppAc_SetFrameNum(AppAcPicInfo_t *pPicInfo, UINT32 FrameNum)
{
    UINT32 Rval = ARM_OK;
    memio_source_recv_picinfo_t *pInfo;

    ArmStdC_memcpy(&pInfo, &(pPicInfo->PicInfo.pBuffer), sizeof(void*));
    pInfo->pic_info.frame_num = FrameNum;


    return Rval;
}

UINT32 AppAc_GetFrameNum(AppAcPicInfo_t *pPicInfo, UINT32 *pFrameNum)
{
    UINT32 Rval = ARM_OK;
    memio_source_recv_picinfo_t *pInfo;

    ArmStdC_memcpy(&pInfo, &(pPicInfo->PicInfo.pBuffer), sizeof(void*));
    *pFrameNum = pInfo->pic_info.frame_num;


    return Rval;
}
