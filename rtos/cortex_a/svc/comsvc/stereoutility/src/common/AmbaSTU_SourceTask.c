/**
 *  @file AmbaStu_SourceTask.c
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
 *  @details Source task, can be connect to a algo task and send input data to it
 *
 */


#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaSTU_SourceTask.h"
#include "cvapi_flexidag_ambafex_cv2.h"
#include "AmbaMisraFix.h"

#define TASKNAME "Source_Task"

#define PROFILE_PROCESS 0U

static void ToUpStreamCallback(AppAcCbMsg_t *pMsg)
{
    //ArmLog_STR(TASKNAME, "## GotDownStreamCallback");
    AppAc_MsgQueueSend(pMsg->Desc.pToHandle, APPAC_TASK_CMD_UPCB, NULL);
}
static void ToDownStreamCallback(AppAcCbMsg_t *pMsg)
{
    //ArmLog_STR(TASKNAME, "## GotDownStreamCallback");
    AppAc_MsgQueueSend(pMsg->Desc.pToHandle, APPAC_TASK_CMD_DNCB, NULL);
}

static UINT32 CallbackDownStream(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    Rval = AppAc_CallbackDownStream(pHandle);

    return Rval;
}


static UINT32 CheckInOutReady(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    UINT32 IsEmpty;
    UINT32 IsFull;
    UINT32 i;
    AppAcBuffer_t *pBuf;

    // scan all hooked input are not empty
    Rval = ARM_NG;
    pBuf = &(pHandle->InputBuffer[0]);
    //ArmLog_STR(TASKNAME, "## check InputBuffer[%d] rp=%d, wp=%d", i, pBuf->ReadIdx, pBuf->WriteIdx);
    Rval = AppAc_IsBufferEmpty(pHandle, pBuf, &IsEmpty);
    if (IsEmpty == 1U) {
        Rval = ARM_NG;
    } else {
        Rval = ARM_OK;
    }

    // scan all hooked output are not full
    if (Rval == ARM_OK) {
        Rval = ARM_NG;
        for (i = 0; i < MAX_DOWN_STREAM_NODES; i++) {


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

    if(Rval == ARM_OK) {
        //ArmLog_STR(TASKNAME, "## Input and output are OK");
    } else {
        //ArmLog_STR(TASKNAME, "## Input and output are not ready");
    }
    return Rval;
}



static UINT32 Process(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AppAcSource_t *pAppAcSource;
    //UINT32 Length, ActualLoadSize;
    //flexidag_memblk_t *pOutBufDesc;
    UINT32 ReadIdx;
    //UINT32 WriteIdx;
    AppAcDesc_t *pInputDesc;

    ArmStdC_memcpy(&(pAppAcSource), &(pHandle->private), sizeof(void*));

    //get one frame
    ReadIdx = pHandle->InputBuffer[0].ReadIdx;
    pInputDesc = &(pHandle->InputBuffer[0].Desc[ReadIdx]);

    //push to output buf
    if (Rval == ARM_OK) {
        AppAcDesc_t AppAcDesc;
        //UINT32 ReadIdx = pHandle->InputBuffer[0].ReadIdx;

        AppAcDesc.FrameNum = pInputDesc->FrameNum;
        AppAcDesc.pFromHandle = pHandle;
        AppAcDesc.IsLastFrame = pInputDesc->IsLastFrame;
        ArmStdC_memcpy(&(AppAcDesc.Info), &(pInputDesc->Info), sizeof(flexidag_memblk_t));
        ArmStdC_memcpy(&(AppAcDesc.Metadata), &(pInputDesc->Metadata), sizeof(AppAcMetadata_t));

        Rval = AppAc_BufferDescPushToAllDownStream(pHandle, &AppAcDesc);
    }

    return Rval;
}

static UINT32 Idle2Ready(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    //ArmLog_STR(TASKNAME, "## do Idle2Ready, pHandle=%x", pHandle);

    return Rval;
}

static UINT32 Ready2Proc(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AppAcSource_t *pAppAcSource;

    //ArmLog_STR(TASKNAME, "## do Ready2Proc, pHandle=%x", pHandle);

    (void) ArmStdC_memcpy(&pAppAcSource, &(pHandle->private), sizeof(void*));
    //ArmLog_STR(TASKNAME, "## initial %s, %d",pAppAcFileIn->FileName[0], pAppAcFileIn->FileIndex);

    (void)AppAc_ProfilerTimerReset(pHandle, PROFILE_PROCESS);

    return Rval;
}

static UINT32 ProcUpStreamCb(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;
    AppAcDesc_t *pDesc = &(pMsg->Desc);
    //ArmLog_STR(TASKNAME, "## do ProcUpStreamCb, pHandle=%x", pHandle);
    Rval = AppAc_BufferDescPush(pHandle, &(pHandle->InputBuffer[0]), pDesc);

    return Rval;
}

static UINT32 ProcDownStreamCb(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pMsg);
    //ArmLog_STR(TASKNAME, "## do ProcDownStreamCb, pHandle=%x", pHandle);
    // update rp of out desc
    Rval = AppAc_BufferUprp(pHandle, &(pHandle->OutputBuffer[0]));

    return Rval;
}

static UINT32 ProcFlexiDoneCb(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);
    // update rp of input desc

    // update wp of output desc

    // callback to upstream

    // callback to downstream

    return Rval;
}

static UINT32 CmdParserIdleState(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg, UINT32 *pNextState)
{
    UINT32 Rval = ARM_OK;
    UINT32 Cmd = pMsg->Cmd;

    switch (Cmd) {
        case APPAC_TASK_CMD_INIT:
            Rval = Idle2Ready(pHandle);
            if (Rval == ARM_OK) {
                *pNextState = APPAC_TASK_STATE_READY;
            }
            break;
        default:
            *pNextState = APPAC_TASK_STATE_NA;
    }
    return Rval;
}

static UINT32 CmdParserReadyState(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg, UINT32 *pNextState)
{
    UINT32 Rval = ARM_OK;
    UINT32 Cmd = pMsg->Cmd;

    switch (Cmd) {
        case APPAC_TASK_CMD_START:
            Rval = Ready2Proc(pHandle);
            if (Rval == ARM_OK) {
                *pNextState = APPAC_TASK_STATE_PROC;
            }
            break;
        default:
            *pNextState = APPAC_TASK_STATE_NA;
    }
    return Rval;
}


static UINT32 CmdParserProcState(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg, UINT32 *pNextState)
{
    UINT32 Rval = ARM_OK;
    UINT32 Cmd = pMsg->Cmd;
//    UINT32 IsFull;

    switch (Cmd) {
        case APPAC_TASK_CMD_UPCB:
        Rval = ProcUpStreamCb(pHandle, pMsg);
        if (Rval == ARM_OK) {
            *pNextState = APPAC_TASK_STATE_PROC;
        }
        break;

        case APPAC_TASK_CMD_DNCB:
            Rval = ProcDownStreamCb(pHandle, pMsg);
            if (Rval == ARM_OK) {
                *pNextState = APPAC_TASK_STATE_PROC;
            }

            Rval = ARM_NG; //do nothing when DNCB

            break;
        case APPAC_TASK_CMD_FLEXI_DONE_CB:
            Rval = ProcFlexiDoneCb(pHandle);
            if (Rval == ARM_OK) {
                *pNextState = APPAC_TASK_STATE_PROC;
            }
            break;
        case APPAC_TASK_CMD_CONNECT_DOWN_STREAM:
            Rval = AppAc_InsertDownStreamList(pHandle, pMsg->pPrivate);
            if (Rval == ARM_OK) {
                *pNextState = APPAC_TASK_STATE_PROC;
            }
            break;
        case APPAC_TASK_CMD_CONNECT_UP_STREAM:
            Rval = AppAc_InsertUpStreamList(pHandle, pMsg->pPrivate);
            if (Rval == ARM_OK) {
                *pNextState = APPAC_TASK_STATE_PROC;
            }
            break;
        case APPAC_TASK_CMD_DISCONNECT_DOWN_STREAM:
            Rval = AppAc_DeleteStreamList(pHandle, pMsg->pPrivate, TO_DOWN_STREAM);
            if (Rval == ARM_OK) {
                *pNextState = APPAC_TASK_STATE_PROC;
            }
            break;
        case APPAC_TASK_CMD_DISCONNECT_UP_STREAM:
            Rval = AppAc_DeleteStreamList(pHandle, pMsg->pPrivate, TO_UP_STREAM);
            if (Rval == ARM_OK) {
                *pNextState = APPAC_TASK_STATE_PROC;
            }
            break;
        default:
            *pNextState = APPAC_TASK_STATE_NA;
    }


    if (Rval == ARM_OK) {
        Rval = CheckInOutReady(pHandle);
    }

    if (Rval == ARM_OK) {
        //UINT64 Time;

        (void)AppAc_ProfilerTimerStart(pHandle, PROFILE_PROCESS);
        Rval = Process(pHandle);
        (void)AppAc_ProfilerTimerStop(pHandle, PROFILE_PROCESS);
    }

    if (Rval == ARM_OK) {

        //Rval = CallbackUpStream(pHandle);
        Rval |= CallbackDownStream(pHandle);

        Rval = AppAc_BufferUprp(pHandle, &(pHandle->InputBuffer[0]));
        //Rval = AppAc_BufferUpwp(pHandle, &(pHandle->OutputBuffer[0]));

    }



    return Rval;
}

static UINT32 StateTransitionFunc(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;
    UINT32 Cs = pHandle->CurrState;
    UINT32 Ns;
    //UINT32 Cmd = pMsg->Cmd;

    //ArmLog_STR(TASKNAME, "## StateTransitionFunc, CS=%d, CMD=%d", Cs, Cmd);
    switch(Cs) {
        case APPAC_TASK_STATE_IDLE:
            Rval = CmdParserIdleState(pHandle, pMsg, &Ns);
        break;
        case APPAC_TASK_STATE_READY:
            Rval = CmdParserReadyState(pHandle, pMsg, &Ns);
        break;
        case APPAC_TASK_STATE_PROC:
            Rval = CmdParserProcState(pHandle, pMsg, &Ns);
        break;
        default:
            Rval = ARM_NG;
    }

    if(Rval == ARM_OK) {
        pHandle->CurrState = Ns;
    }

    return Rval;
}

static ArmTaskRet_t TaskFunc(ArmTaskEntryArg_t EntryArg)
{
    AppAcTaskMsg_t Msg;
    //UINT32 Cmd;
    UINT32 Rval = ARM_OK;
    AppAcTaskHandle_t *pHandle;

    ArmStdC_memcpy(&pHandle, &EntryArg, sizeof(void*));
    //ArmLog_STR(TASKNAME, "## task start running, pHandle=%x", pHandle);
    for(;;) {
        //ArmLog_STR(TASKNAME, "## recv_msg from %s", pHandle->AppAcResourceName.MsgQ);
        ArmMsgQueue_Recv(&pHandle->TaskMsgQ, &Msg);
        Rval |= StateTransitionFunc(pHandle, &Msg);
        if(Rval != ARM_OK) {
            // report error to main task
        }
    }

    return 0U;
}

UINT32 AppAc_SourceTaskInit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId)
{
    UINT32 Rval = ARM_OK;
    ARM_TASK_CREATE_ARG_s TaskCreateArg;
    ArmTaskEntryArg_t EntryArg;
    //ArmTask_t *pTask;
    //SVC_CV_ALGO_QUERY_CFG_s QueryCfg;
    //flexidag_memblk_t AlgoTaskTotalBuf;
    //flexidag_memblk_t AppAcSourceMemblk;
    AppAcSource_t *pAppAcSource;

    //char TaskName[128];

    ArmStdC_memset(pHandle, 0U, sizeof(AppAcTaskHandle_t));

    pHandle->CurrState = APPAC_TASK_STATE_IDLE;
    pHandle->UpStreamCallBack = &ToUpStreamCallback;
    pHandle->DownStreamCallBack = &ToDownStreamCallback;

    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = ArmMemPool_Allocate(MemPoolId, APPAC_SOURCE_MEM_SIZE, &(pHandle->AlgoTaskTotalBuf));
        ArmMemPool_Create(&(pHandle->AlgoTaskTotalBuf), &pHandle->MemPoolId);
    }

    ArmStdC_strcpy(pHandle->AppAcResourceName.MsgQ     , MAX_APPAC_NAME_LEN, "Source_TaskMsgQ");
    ArmStdC_strcpy(pHandle->AppAcResourceName.Task     , MAX_APPAC_NAME_LEN, "Source_Task");
    ArmStdC_strcpy(pHandle->AppAcResourceName.TaskMtx  , MAX_APPAC_NAME_LEN, "Source_TaskMtx");
    ArmStdC_strcpy(pHandle->AppAcResourceName.InBufMtx , MAX_APPAC_NAME_LEN, "Source_InBufMtx");
    ArmStdC_strcpy(pHandle->AppAcResourceName.OutBufMtx, MAX_APPAC_NAME_LEN, "Source_OutBufMtx");


    // alloc private handle
    Rval = ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(AppAcSource_t), &(pHandle->AppAcPrivateMemblk));
    if (Rval == ARM_OK) {
        (void) ArmStdC_memcpy(&pAppAcSource, &(pHandle->AppAcPrivateMemblk.pBuffer), sizeof(void*));
        (void) ArmStdC_memcpy(&(pHandle->private), &pAppAcSource, sizeof(void*));
    }

    // create Mutex
    Rval |= ArmMutex_Create(&pHandle->Mutex, &(pHandle->AppAcResourceName.TaskMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->InBufMtx, &(pHandle->AppAcResourceName.InBufMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->OutBufMtx, &(pHandle->AppAcResourceName.OutBufMtx[0]));
    // create message queue
    //ArmLog_STR("AppAc_FileIn", "## create msg queue %u",Rval);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = AppAc_MsgQueueCreate(pHandle);

    }

    // create input output buffer
    if (Rval == ARM_OK) {
        UINT32 Depth = MAX_BUFFER_DEPTH;
        UINT32 OutBufSize = sizeof(memio_source_recv_picinfo_t);
        Rval = AppAc_BufferInit(pHandle, Depth, OutBufSize);
    }

    // create task
    //ArmLog_STR("AppAc_FileIn", "## create task %u",Rval);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        pHandle->Task.Ctrl.StackSz = 4096U;
        Rval = ArmMemPool_Allocate(pHandle->MemPoolId, pHandle->Task.Ctrl.StackSz, &(pHandle->Task.StackBuf));
    }
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        (void) ArmStdC_memcpy(&EntryArg, &pHandle, sizeof(void*));
        TaskCreateArg.TaskPriority = 50U;
        TaskCreateArg.EntryFunction = &TaskFunc;
        TaskCreateArg.EntryArg = EntryArg;
        TaskCreateArg.pStackBase = pHandle->Task.StackBuf.pBuffer;
        TaskCreateArg.StackByteSize = pHandle->Task.Ctrl.StackSz;
        TaskCreateArg.CoreSel = pHandle->Task.Ctrl.CoreSel;
        ArmTask_Create(&(pHandle->Task.Tsk), pHandle->AppAcResourceName.Task, &TaskCreateArg);
    }

    // send init command to task
    //ArmLog_STR("AppAc_FileIn", "## send init command to task %u", Rval);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_INIT, NULL);
    }

    // wait for ready state
    if (Rval == ARM_OK) {
        AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    }


    return Rval;
}


UINT32 AppAc_SourceTaskStart(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_START, NULL);
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_PROC);

    return Rval;
}

UINT32 AppAc_SourceTaskStop(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_PROC);
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_STOP, NULL);
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    return Rval;
}

UINT32 AppAc_SourceTaskDeinit(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    return Rval;
}

UINT32 AppAc_SourceTaskFeedOneFrame(AppAcTaskHandle_t *pHandle, AppAcPicInfo_t *pPicInfo, AppAcMetadata_t *pMetadata)
{
    UINT32 Rval = ARM_OK;
    AppAcCbMsg_t CbMsg;
    AppAcDesc_t *pDesc = &(CbMsg.Desc);
    UINT32 FrameNum;

    Rval = AppAc_GetFrameNum(pPicInfo, &FrameNum);
    ArmStdC_memcpy(&(pDesc->Info), &(pPicInfo->PicInfo), sizeof(flexidag_memblk_t));
    ArmStdC_memcpy(&(pDesc->Metadata), pMetadata, sizeof(AppAcMetadata_t));
    pDesc->FrameNum = FrameNum;

    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_UPCB, &CbMsg);

    return Rval;
}
