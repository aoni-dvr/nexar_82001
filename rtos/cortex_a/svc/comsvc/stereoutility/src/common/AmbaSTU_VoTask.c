/**
 *  @file AmbaStu_VoTask.c
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
 *  @details Visual Odometry algo task, wrap up vo software function
 *
 */

#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaSTU_VoTask.h"
#include "cvapi_flexidag_ambavo_cv2.h"
#include "AmbaMisraFix.h"

#define TASKNAME "Vo_Task"
#define PROFILE_PROCESS 0U

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

//static void FlexiDagCallback(void)
//{
//
//}

static UINT32 CallbackDownStream(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    Rval = AppAc_CallbackDownStream(pHandle);

    return Rval;
}

static UINT32 CallbackUpStream(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    Rval = AppAc_CallbackUpStream(pHandle);

    return Rval;
}


static UINT32 CheckInOutReady(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    Rval = AppAc_CheckInOutReady(pHandle);

    return Rval;
}


static UINT32 Process(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AppAcVo_t *pAppAcVo;
    AMBA_VO_FD_HANDLE_s *pVoFdHandler;
    AMBA_CV_FMA_BUF_s *pPreFmaBuf;
    AMBA_CV_FMA_BUF_s *pCurFmaBuf;
    AMBA_CV_FEX_BUF_s *pFexBuf;
    AMBA_CV_VO_BUF_s *pOutBuf;
    UINT32 ReadIdx;
    //UINT32 WriteIdx;
    flexidag_memblk_t *pInfo;
    flexidag_memblk_t *pTemp;
    UINT32 FrameNum;

    //input PrevFma
    (void) ArmStdC_memcpy(&pAppAcVo, &(pHandle->private), sizeof(void*));
    pVoFdHandler = &(pAppAcVo->VoFdHandler);
    pPreFmaBuf = &(pAppAcVo->PreFmaBuf);
    if (pPreFmaBuf->pBuffer != NULL) {
        //Rval |= AmbaCV_UtilityCmaMemClean(pPreFmaBuf);
        //Rval |= AmbaCV_UtilityCmaMemInvalid(pPreFmaBuf);
    }

//TODO: find proper input pin by naming

    //input FexBuf
    ReadIdx = pHandle->InputBuffer[0].ReadIdx;
    FrameNum = pHandle->InputBuffer[0].Desc[ReadIdx].FrameNum;
    pInfo = &(pHandle->InputBuffer[0].Desc[ReadIdx].Info);
    (void) ArmStdC_memcpy(&pFexBuf, &pInfo, sizeof(void*));
//    ArmLog_STR(TASKNAME, "## Process, FEX ReadIdx=%d, pInfo->pBuffer=%x, pInfo->buffer_size=%d",
//            ReadIdx, pInfo->pBuffer, pInfo->buffer_size); //fflush(stdout);
    //Rval |= AmbaCV_UtilityCmaMemClean(pInfo);
    //Rval |= AmbaCV_UtilityCmaMemInvalid(pInfo);

    //input CurrFma
    ReadIdx = pHandle->InputBuffer[1].ReadIdx;
    pInfo = &(pHandle->InputBuffer[1].Desc[ReadIdx].Info);
    (void) ArmStdC_memcpy(&pCurFmaBuf, &pInfo, sizeof(void*));
//    ArmLog_STR(TASKNAME, "## Process, FMA ReadIdx=%d, pInfo->pBuffer=%x, pInfo->buffer_size=%d",
//            ReadIdx, pInfo->pBuffer, pInfo->buffer_size); //fflush(stdout);
    //Rval |= AmbaCV_UtilityCmaMemClean(pInfo);
    //Rval |= AmbaCV_UtilityCmaMemInvalid(pInfo);
    //output OutBuf
    pTemp = &(pHandle->OutBuf[pHandle->OutputBuffer[0].WriteIdx]);
    (void) ArmStdC_memcpy(&pOutBuf, &(pTemp), sizeof(void*));

//    ArmLog_STR(TASKNAME, "## AmbaVO_GetResult FN=%d, pVoFdHandler=%x, pPreFmaBuf=%x, pCurFmaBuf=%x, pFexBuf=%x, pOutBuf=%x",
//            FrameNum, pVoFdHandler, pPreFmaBuf->pBuffer, pCurFmaBuf->pBuffer, pFexBuf->pBuffer, pOutBuf->pBuffer); //fflush(stdout);
    Rval = AmbaVO_GetResult(pVoFdHandler, pPreFmaBuf, pCurFmaBuf, pFexBuf, pOutBuf);

//#define MaxFileNameLen 128U
//    if(0){
//        char VoOutFile[128] = "/tmp/nfs/ac/out/vo/vo_";
//        char FileIndex[32];
//        char *pBuf;
//        UINT32 Size;
//        UINT32 WriteSize;
//
//        sprintf(FileIndex, "%d", FrameNum);
//        ArmStdC_strcat(VoOutFile, MaxFileNameLen, FileIndex);
//        ArmStdC_strcat(VoOutFile, MaxFileNameLen, ".bin");
//        ArmLog_STR(TASKNAME, "## dump vo: %s", VoOutFile);
//
//        pBuf = pOutBuf->pBuffer;
//        Size = pOutBuf->BufferSize;
//        ArmLog_STR(TASKNAME, "## : ArmFIO_Save pBuf=%x, size=%d", pBuf, Size);
//        Rval = ArmFIO_Save(pBuf, Size, VoOutFile, &WriteSize);
//    }

    //push to output buf
    if (Rval == ARM_OK) {
        AppAcDesc_t AppAcDesc;

        AppAcDesc.FrameNum = FrameNum;
        AppAcDesc.pFromHandle = pHandle;
        AppAcDesc.IsLastFrame = pHandle->InputBuffer[0].Desc[ReadIdx].IsLastFrame;
        ArmStdC_memcpy(&(AppAcDesc.Info), pOutBuf, sizeof(flexidag_memblk_t));
        ArmStdC_memcpy(&(AppAcDesc.Metadata), &(pHandle->InputBuffer[0].Desc[ReadIdx].Metadata), sizeof(AppAcMetadata_t));
        Rval = AppAc_BufferDescPushToAllDownStream(pHandle, &AppAcDesc);
    }
    //backup PrevFma
    ArmStdC_memcpy(&(pAppAcVo->PreFmaBuf), pCurFmaBuf, sizeof(AMBA_CV_FMA_BUF_s));



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

    Rval = AppAc_ProcUpStreamCb(pHandle, pMsg);

    return Rval;
}

static UINT32 ProcDownStreamCb(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg)
{
    UINT32 Rval = ARM_OK;
    UINT32 Index;

    //find who callbacks
    Rval = AppAc_WhosCall(pHandle, pMsg->Desc.pFromHandle, &Index, FROM_DOWN_STREAM);

    // update rp of corresponding output buffer
    Rval = AppAc_BufferUprp(pHandle, &(pHandle->OutputBuffer[Index]));

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
        (void)AppAc_ProfilerTimerStart(pHandle, PROFILE_PROCESS);
        Rval = Process(pHandle);
        (void)AppAc_ProfilerTimerStop(pHandle, PROFILE_PROCESS);
    }

    if (Rval == ARM_OK) {
        Rval = CallbackUpStream(pHandle);
        Rval |= AppAc_BufferUprpToAll(pHandle, TO_UP_STREAM);
        Rval |= CallbackDownStream(pHandle);
        //Rval |= AppAc_BufferUpwpToAll(pHandle, TO_DOWN_STREAM); //already update in process
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


UINT32 AppAc_AmbaVoInit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId, flexidag_memblk_t *pInitMetaRightBuf, AMBA_CV_VO_CFG_s *pVoCfg)
{
    UINT32 Rval = ARM_OK;
    ARM_TASK_CREATE_ARG_s TaskCreateArg;
    ArmTaskEntryArg_t EntryArg;
    //ArmTask_t *pTask;
    flexidag_memblk_t AlgoTaskTotalBuf;
    flexidag_memblk_t PrivateMemblk;
    //flexidag_memblk_t AlgoBufMemblk;
    AppAcVo_t *pAppAcVo;


    flexidag_memblk_t FlexiDagBinBuf;
    uint32_t FlexiDagBinSz = 0;
    uint32_t StateBufSz;
    uint32_t TempBufSz;
    uint32_t OutputBufNum;
    uint32_t OutputBufSz;
    AMBA_CV_VO_CFG_s *pVoConfig;
    AMBA_VO_FD_HANDLE_s *pVoFdHandler;
    flexidag_memblk_t StateBuf;
    flexidag_memblk_t TempBuf;
    //AMBA_CV_FEX_BUF_s FexBuf;
    //AMBA_CV_FMA_BUF_s CurFmaBuf;
    //AMBA_CV_VO_BUF_s OutBuf;


    //char TaskName[128];

    ArmStdC_memset(pHandle, 0U, sizeof(AppAcTaskHandle_t));

    pHandle->ProcInterval = 1U;

    pHandle->CurrState = APPAC_TASK_STATE_IDLE;
    pHandle->UpStreamCallBack = &UpStreamCallback;
    pHandle->DownStreamCallBack = &DownStreamCallback;

    // create memory pool for the whole task
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = ArmMemPool_Allocate(MemPoolId, APPAC_VO_MEM_SIZE, &AlgoTaskTotalBuf);
        ArmLog_DBG(TASKNAME, "## create mem pool %u", Rval, 0U); //fflush(stdout);
        ArmMemPool_Create(&AlgoTaskTotalBuf, &pHandle->MemPoolId);
    }

    // assign resource names
    if (Rval == ARM_OK) {
        ArmStdC_strcpy(pHandle->AppAcResourceName.MsgQ, MAX_APPAC_NAME_LEN, "Vo_TaskMsgQ");
        ArmStdC_strcpy(pHandle->AppAcResourceName.Task, MAX_APPAC_NAME_LEN, "Vo_Task");
        ArmStdC_strcpy(pHandle->AppAcResourceName.TaskMtx, MAX_APPAC_NAME_LEN, "Vo_TaskMtx");
        ArmStdC_strcpy(pHandle->AppAcResourceName.InBufMtx, MAX_APPAC_NAME_LEN, "Vo_InBufMtx");
        ArmStdC_strcpy(pHandle->AppAcResourceName.OutBufMtx, MAX_APPAC_NAME_LEN, "Vo_OutBufMtx");
    }

    // alloc private handle
    if (Rval == ARM_OK) {
//        ArmLog_STR(TASKNAME, "## alloc private handle %u", Rval);
        Rval = ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(AppAcVo_t), &PrivateMemblk);
    }

    if (Rval == ARM_OK) {
//        ArmLog_STR(TASKNAME, "## assign private handle", Rval);
        (void) ArmStdC_memcpy(&pAppAcVo, &PrivateMemblk.pBuffer, sizeof(void*));
        (void) ArmStdC_memcpy(&(pHandle->private), &pAppAcVo, sizeof(void*));
        pVoConfig = &(pAppAcVo->VoConfig);
        pVoFdHandler = &(pAppAcVo->VoFdHandler);
//        ArmLog_STR(TASKNAME, "## pVoFdHandler=%x", pVoFdHandler);
        (void)ArmStdC_memset(&(pAppAcVo->PreFmaBuf), 0U, sizeof(AMBA_CV_FMA_BUF_s));
    }

    if (Rval == ARM_OK) {
        Rval = AmbaVO_Open(&FlexiDagBinBuf, FlexiDagBinSz, &StateBufSz, &TempBufSz, &OutputBufNum, &OutputBufSz, pVoFdHandler);
    }

    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## AmbaVO_Open StateBufSz:%d TempBufSz:%d\n", StateBufSz, TempBufSz);
        ArmLog_DBG(TASKNAME, "## AmbaVO_Open OutputBufNum:%d OutputBufSz:%d\n", OutputBufNum, OutputBufSz);
        Rval |= ArmMemPool_Allocate(pHandle->MemPoolId, StateBufSz, &StateBuf);
        //Rval |= ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(AMBA_CV_VO_DATA_s), &OutBuf);
        Rval |= AmbaVO_Init(pVoFdHandler, &StateBuf, &TempBuf);
    }

    if (Rval == ARM_OK) {
        ArmStdC_memcpy(pVoConfig, pVoCfg, sizeof(AMBA_CV_VO_CFG_s));
        //pVoConfig->pInMetaRight = pInitMetaRightBuf;
        ArmStdC_memcpy(&(pVoConfig->pInMetaRight), &(pInitMetaRightBuf), sizeof(void*));
        Rval = AmbaVO_Cfg(pVoFdHandler, pVoConfig);
    }

    // create Mutex
//    ArmLog_STR(TASKNAME, "## create Mutex %u",Rval);
    Rval |= ArmMutex_Create(&pHandle->Mutex, &(pHandle->AppAcResourceName.TaskMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->InBufMtx, &(pHandle->AppAcResourceName.InBufMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->OutBufMtx, &(pHandle->AppAcResourceName.OutBufMtx[0]));

    // create message queue
//    ArmLog_STR(TASKNAME, "## create msg queue %u",Rval);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = AppAc_MsgQueueCreate(pHandle);
//        ArmLog_STR(TASKNAME, "## created msg queue %x", &(pHandle->TaskMsgQ));
    }

    // create input output buffer
    if (Rval == ARM_OK) {
        UINT32 Depth = MAX_BUFFER_DEPTH;
        UINT32 OutBufSize = sizeof(AMBA_CV_VO_DATA_s);
        Rval = AppAc_BufferInit(pHandle, Depth, OutBufSize);
    }

    // create task
    ArmLog_DBG(TASKNAME, "## create task %u", Rval, 0U);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        pHandle->Task.Ctrl.StackSz = 8192U;
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
//    ArmLog_STR(TASKNAME, "## send init command to task %u", Rval);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_INIT, NULL);
    }

    // wait for ready state
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);

    return Rval;
}


UINT32 AppAc_AmbaVoStart(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_START, NULL);
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_PROC);

    return Rval;
}

UINT32 AppAc_AmbaVoStop(AppAcTaskHandle_t *pHandle)
{
    AmbaMisra_TouchUnused(pHandle);

    return 0U;
}

UINT32 AppAc_AmbaVoDeinit(AppAcTaskHandle_t *pHandle)
{
    AmbaMisra_TouchUnused(pHandle);

    return 0U;
}

