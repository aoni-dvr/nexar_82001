/**
 *  @file AmbaStu_SinkTask.c
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
 *  @details Sink task, can be connect to a algo task and receive output from it, callback to high level app.
 *
 */

#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaSTU_SinkTask.h"
#include "cvapi_flexidag_ambafex_cv2.h"
#include "cvapi_flexidag_ambamvac_cv2.h"
#include "AmbaMisraFix.h"

#define TASKNAME "Sink_Task"
#define PROFILE_PROCESS 0U

extern UINT32 gAllFrameProcDone;

static void UpStreamCallback(AppAcCbMsg_t *pMsg)
{
    //ArmLog_STR(TASKNAME, "## send message APPAC_TASK_CMD_UPCB pMsg=%x", pMsg);
    AppAc_MsgQueueSend(pMsg->Desc.pToHandle, APPAC_TASK_CMD_UPCB, pMsg);
}
static void DownStreamCallback(AppAcCbMsg_t *pMsg)
{
    //ArmLog_STR(TASKNAME, "## send message APPAC_TASK_CMD_DNCB pMsg=%x", pMsg);
    AppAc_MsgQueueSend(pMsg->Desc.pToHandle, APPAC_TASK_CMD_DNCB, pMsg);
}


static UINT32 CheckInOutReady(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    UINT32 IsEmpty;
//    UINT32 IsFull;
//    UINT32 i;
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

//    // scan all hooked output are not full
//    if (Rval == ARM_OK) {
//        Rval = ARM_NG;
//        for (i = 0; i < MAX_DOWN_STREAM_NODES; i++) {
//
//
//            if (pHandle->pDownStreamList[i] == NULL) {
//                continue;
//            }
//
//            pBuf = &(pHandle->OutputBuffer[i]);
//            ArmLog_STR(TASKNAME, "## check OutputBuffer[%d] rp=%d, wp=%d", i, pBuf->ReadIdx, pBuf->WriteIdx);
//            Rval = AppAc_IsBufferFull(pHandle, pBuf, &IsFull);
//            if (IsFull == 1U) {
//                Rval = ARM_NG;
//                break;
//            } else {
//                Rval = ARM_OK;
//            }
//        }
//    }

//    if(Rval == ARM_OK) {
//        ArmLog_STR(TASKNAME, "## Input and output are OK");
//    } else {
//        ArmLog_STR(TASKNAME, "## Input and output are not ready");
//    }
    return Rval;
}


//static UINT32 CallbackDownStream(AppAcTaskHandle_t *pHandle)
//{
//    UINT32 Rval = ARM_OK;
//    Rval = AppAc_CallbackDownStream(pHandle);
//    return Rval;
//}

static UINT32 CallbackUpStream(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    Rval = AppAc_CallbackUpStream(pHandle);
    return Rval;
}

static UINT32 Process(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    UINT32 IsInputEmpty;

    //UINT32 IsOutputFull;
    //SVC_CV_ALGO_HANDLE_s *pScaHandle;
    AppAcSink_t *pAppAcSink;
    AppAcDesc_t *pDesc;
    AMBA_CV_MVAC_DATA_s *pInData;

    //ArmLog_STR(TASKNAME, "## Process");

    if(pHandle == NULL) {
        Rval = ARM_NG;
    }
    //check input and output ready
    if (Rval == ARM_OK) {
        Rval |= AppAc_IsBufferEmpty(pHandle, &(pHandle->InputBuffer[0]), &IsInputEmpty);
    }
    if ((Rval == ARM_OK) && (IsInputEmpty == 1U)) {
        //ArmLog_STR(TASKNAME, "## IO not ready");
        Rval = ARM_NG;
    }

    (void) ArmStdC_memcpy(&pAppAcSink, &(pHandle->private), sizeof(void*));

    pDesc = &(pHandle->InputBuffer[0].Desc[pHandle->InputBuffer[0].ReadIdx]);
    ArmStdC_memcpy(&pInData, &(pDesc->Info.pBuffer), sizeof(void*));

    if (pAppAcSink->AcGetResultCb != NULL) {
        ArmLog_U64(TASKNAME, "## Callback AcGetResultCb=0x%llx, data=0x%llx", (UINT64)pAppAcSink->AcGetResultCb, (UINT64)pInData);
        pAppAcSink->AcGetResultCb(pInData);
    }

    if(pInData->State == AMBA_CV_MVAC_STATE_END) {
        //gAllFrameProcDone = 1U;
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

    //ArmLog_STR(TASKNAME, "## do Ready2Proc, pHandle=%x", pHandle);

    (void)AppAc_ProfilerTimerReset(pHandle, PROFILE_PROCESS);

    return Rval;
}

static UINT32 ProcUpStreamCb(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;

    //ArmLog_STR(TASKNAME, "## ProcUpStreamCb");

    Rval = AppAc_BufferDescPush(pHandle, &(pHandle->InputBuffer[0]), &(pMsg->Desc));
    if(Rval != ARM_OK) {
        //ArmLog_ERR(TASKNAME, "## ProcUpStreamCb : input buffer full!!");
    }

    return Rval;
}

static UINT32 ProcDownStreamCb(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pMsg);

    // update rp of out desc
    Rval = AppAc_BufferUprp(pHandle, &(pHandle->OutputBuffer[0]));

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
            break;
//        case APPAC_TASK_CMD_FLEXI_DONE_CB:
//            Rval = ProcFlexiDoneCb(pHandle);
//            if (Rval == ARM_OK) {
//                *pNextState = APPAC_TASK_STATE_PROC;
//            }
//            break;
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
        (void)AppAc_ProfilerTimerStart(pHandle, PROFILE_PROCESS);
        Rval = Process(pHandle);
        (void)AppAc_ProfilerTimerStop(pHandle, PROFILE_PROCESS);
    }


    if (Rval == ARM_OK) {

        Rval = CallbackUpStream(pHandle);
        //Rval |= CallbackDownStream(pHandle);

        Rval = AppAc_BufferUprp(pHandle, &(pHandle->InputBuffer[0]));
        //Rval = AppAc_BufferUpwp(&(pHandle->OutputBuffer[0]));

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

UINT32 AppAc_SinkTaskInit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId, AcGetResultCb_t AcGetResultCb)
{
    UINT32 Rval = ARM_OK;
    ARM_TASK_CREATE_ARG_s TaskCreateArg;
    ArmTaskEntryArg_t EntryArg;
    flexidag_memblk_t AlgoTaskTotalBuf;
    flexidag_memblk_t AppAcSinkMemblk;
    AppAcSink_t *pAppAcSink;

    //char TaskName[128];

    ArmStdC_memset(pHandle, 0U, sizeof(AppAcTaskHandle_t));

    pHandle->CurrState = APPAC_TASK_STATE_IDLE;
    pHandle->UpStreamCallBack = &UpStreamCallback;
    pHandle->DownStreamCallBack = &DownStreamCallback;

    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = ArmMemPool_Allocate(MemPoolId, APPAC_SINK_MEM_SIZE, &AlgoTaskTotalBuf);
        ArmMemPool_Create(&AlgoTaskTotalBuf, &pHandle->MemPoolId);
    }

    ArmStdC_strcpy(pHandle->AppAcResourceName.MsgQ     , MAX_APPAC_NAME_LEN, "Sink_TaskMsgQ");
    ArmStdC_strcpy(pHandle->AppAcResourceName.Task     , MAX_APPAC_NAME_LEN, "Sink_Task");
    ArmStdC_strcpy(pHandle->AppAcResourceName.TaskMtx  , MAX_APPAC_NAME_LEN, "Sink_TaskMtx");
    ArmStdC_strcpy(pHandle->AppAcResourceName.InBufMtx , MAX_APPAC_NAME_LEN, "Sink_InBufMtx");
    ArmStdC_strcpy(pHandle->AppAcResourceName.OutBufMtx, MAX_APPAC_NAME_LEN, "Sink_OutBufMtx");


    // alloc FileOut private handle
    //ArmLog_STR(TASK_NAME, "## alloc private handle %u",Rval);
    Rval = ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(pAppAcSink), &AppAcSinkMemblk);
    if (Rval == ARM_OK) {
        (void) ArmStdC_memcpy(&pAppAcSink, &AppAcSinkMemblk.pBuffer, sizeof(void*));
        pAppAcSink->AcGetResultCb = AcGetResultCb;
        ArmLog_U64(TASKNAME, "## Callback AcGetResultCb=0x%llx", (UINT64)pAppAcSink->AcGetResultCb, (UINT64)0U); //fflush(stdout);
        (void) ArmStdC_memcpy(&(pHandle->private), &pAppAcSink, sizeof(void*));
    }

    // create Mutex
    Rval |= ArmMutex_Create(&pHandle->Mutex, &(pHandle->AppAcResourceName.TaskMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->InBufMtx, &(pHandle->AppAcResourceName.InBufMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->OutBufMtx, &(pHandle->AppAcResourceName.OutBufMtx[0]));

    // create message queue
    //ArmLog_STR("AppAc_FileOut", "## create msg queue %u",Rval);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = AppAc_MsgQueueCreate(pHandle);

    }

    // create input buffer
    pHandle->InputBuffer[0].Depth = MAX_BUFFER_DEPTH;
    pHandle->InputBuffer[0].ReadIdx = 0U;
    pHandle->InputBuffer[0].WriteIdx = 0U;

    // create output buffer

    // create task
    //ArmLog_STR("AppAc_FileOut", "## create task %u",Rval);
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
    //ArmLog_STR("AppAc_FileOut", "## send init command to task %u", Rval);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_INIT, NULL);
    }

    // wait for ready state
    if (Rval == ARM_OK) {
        AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    }


    return Rval;
}


UINT32 AppAc_SinkTaskStart(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_START, NULL);
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_PROC);

    return Rval;
}

UINT32 AppAc_SinkTaskStop(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    return Rval;
}

UINT32 AppAc_SinkTaskDeinit(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    return Rval;
}
