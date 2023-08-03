/**
 *  @file AmbaStu_FexTask.c
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
 *  @details FEX algo task, wrap up SCA FEX flexidag
 *
 */


#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaSTU_FexTask.h"
#include "cvapi_svccvalgo_ambafex.h"
#include "cvapi_flexidag_ambafex_cv2.h"
#include "AmbaMisraFix.h"

#define TASKNAME "ScaFex_Task"
#define PROFILE_PROCESS 0U

//extern SVC_CV_ALGO_OBJ_s AmbaFexAlgoObj;

static void UpStreamCallback(AppAcCbMsg_t *pMsg)
{
//    ArmLog_STR(TASKNAME, "## send message APPAC_TASK_CMD_UPCB pMsg=%x", pMsg);
    AppAc_MsgQueueSend(pMsg->Desc.pToHandle, APPAC_TASK_CMD_UPCB, pMsg);
}
static void DownStreamCallback(AppAcCbMsg_t *pMsg)
{
//    ArmLog_STR(TASKNAME, "## send message APPAC_TASK_CMD_DNCB pMsg=%x", pMsg);
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
    //UINT32 IsInputEmpty;
    //UINT32 IsOutputFull;
    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    AppAcScaFex_t *pAppAcScaFex;
    UINT32 FrameNum;

    if(pHandle == NULL) {
        Rval = ARM_NG;
    }
//    //check input and output ready
//    if (Rval == ARM_OK) {
//        Rval |= AppAc_IsBufferEmpty(pHandle, &(pHandle->InputBuffer[0]), &IsInputEmpty);
//        Rval |= AppAc_IsBufferFull(pHandle, &(pHandle->OutputBuffer[0]), &IsOutputFull);
//    }
//    if ((Rval == ARM_OK) && ((IsInputEmpty == 1U) || (IsOutputFull == 1U))) {
//        ArmLog_ERR(TASKNAME, "## IO not ready");
//        Rval = ARM_NG;
//    }

    if(Rval == ARM_OK) {
        SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg;
        AMBA_CV_FLEXIDAG_IO_s InBuf;
        AMBA_CV_FLEXIDAG_IO_s OutBuf;
        flexidag_memblk_t *pOutBuf = &(pHandle->OutBuf[pHandle->OutputBuffer[0].WriteIdx]);

        ArmStdC_memcpy(&(pAppAcScaFex), &(pHandle->private), sizeof(void*));

        pScaHandle = &(pAppAcScaFex->ScaHandle);

        //Input PicInfo
        ScaFeedCfg.pIn = &InBuf;
        ScaFeedCfg.pIn->num_of_buf = 1U;
        FrameNum = pHandle->InputBuffer[0].Desc[pHandle->InputBuffer[0].ReadIdx].FrameNum;
        ArmStdC_memcpy(
                &(ScaFeedCfg.pIn->buf[0]),
                &(pHandle->InputBuffer[0].Desc[pHandle->InputBuffer[0].ReadIdx].Info),
                sizeof(flexidag_memblk_t));

        //Output Fma
        ScaFeedCfg.pOut = &OutBuf;
        ScaFeedCfg.pOut->num_of_buf = 1U;
        ArmStdC_memcpy(
                &(ScaFeedCfg.pOut->buf[0]),
                pOutBuf,
                sizeof(flexidag_memblk_t));

//        ArmLog_STR(TASKNAME, "## AmbaFex_GetResult FN=%d, pScaHandle=%x, pPicInfo=%x, pOutBuf=%x",
//                FrameNum, pScaHandle, ScaFeedCfg.pIn->buf[0].pBuffer, ScaFeedCfg.pOut->buf[0].pBuffer); //fflush(stdout);
//        {//just debug
//            memio_source_recv_picinfo_t *pPicInfo;
//            (void) ArmStdC_memcpy(&pPicInfo, &(pHandle->InputBuffer[0].Desc[pHandle->InputBuffer[0].ReadIdx].Info.pBuffer), sizeof(void*));
//            ArmLog_STR(TASKNAME, "## Fexfeed : pPicInfo=%x rpLumaLeft[0]=%x", pPicInfo, pPicInfo->pic_info.rpLumaLeft[0]);
//        }
        Rval = pScaHandle->pAlgoObj->Feed(pScaHandle, &ScaFeedCfg);

//#define MaxFileNameLen 128U
//        if(0) {
//            char FexOutFile[128] = "/tmp/nfs/ac/out/fex/Fex_";
//            char FileIndex[32];
//            char *pBuf;
//            UINT32 Size;
//            UINT32 WriteSize;
//
//            sprintf(FileIndex, "%d", FrameNum);
//            ArmStdC_strcat(FexOutFile, MaxFileNameLen, FileIndex);
//            ArmStdC_strcat(FexOutFile, MaxFileNameLen, ".bin");
//            ArmLog_STR(TASKNAME, "## dump fex: %s", FexOutFile);
//
//            pBuf = pOutBuf->pBuffer;
//            Size = pOutBuf->buffer_size;
//            ArmLog_STR(TASKNAME, "## : ArmFIO_Save pBuf=%x, size=%d", pBuf, Size);
//            Rval = ArmFIO_Save(pBuf, Size, FexOutFile, &WriteSize);
//        }

        //push to output buf
        if (Rval == ARM_OK) {
            AppAcDesc_t AppAcDesc;
            UINT32 ReadIdx = pHandle->InputBuffer[0].ReadIdx;
            AppAcDesc.FrameNum = FrameNum;
            AppAcDesc.pFromHandle = pHandle;
            AppAcDesc.IsLastFrame = pHandle->InputBuffer[0].Desc[ReadIdx].IsLastFrame;
            ArmStdC_memcpy(&(AppAcDesc.Info), pOutBuf, sizeof(flexidag_memblk_t));
            ArmStdC_memcpy(&(AppAcDesc.Metadata), &(pHandle->InputBuffer[0].Desc[ReadIdx].Metadata), sizeof(AppAcMetadata_t));
            Rval = AppAc_BufferDescPushToAllDownStream(pHandle, &AppAcDesc);
        }
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
    //ArmLog_STR(TASKNAME, "## ProcDownStreamCb: outbuffer[%d] uprp");

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

    //ArmLog_STR(TASKNAME, "## StateTransitionFunc, CS=%d, CMD=%d", Cs, Cmd); fflush(stdout);
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


UINT32 AppAc_ScaAmbaFexInit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId, char *FlexiBinFileName)
{
    UINT32 Rval = ARM_OK;
    ARM_TASK_CREATE_ARG_s TaskCreateArg;
    ArmTaskEntryArg_t EntryArg;
    //ArmTask_t *pTask;
    //SVC_CV_ALGO_QUERY_CFG_s QueryCfg;
    flexidag_memblk_t AlgoTaskTotalBuf;
    flexidag_memblk_t PrivateMemblk;
    flexidag_memblk_t AlgoBufMemblk;
    AppAcScaFex_t *pAppAcScaFex;
    SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg;
    SVC_CV_ALGO_CREATE_CFG_s ScaCreatCfg;
    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    flexidag_memblk_t FlexiBin;
    AmbaMisra_TouchUnused(FlexiBinFileName);
    //char TaskName[128];

    ArmStdC_memset(pHandle, 0U, sizeof(AppAcTaskHandle_t));

    pHandle->ProcInterval = 1U;

    pHandle->CurrState = APPAC_TASK_STATE_IDLE;
    pHandle->UpStreamCallBack = &UpStreamCallback;
    pHandle->DownStreamCallBack = &DownStreamCallback;

    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = ArmMemPool_Allocate(MemPoolId, APPAC_SCA_FEX_MEM_SIZE, &AlgoTaskTotalBuf);
        ArmLog_DBG(TASKNAME, "## create mem pool %u", Rval, 0U); //fflush(stdout);
        ArmMemPool_Create(&AlgoTaskTotalBuf, &pHandle->MemPoolId);
    }
    if (Rval == ARM_OK) {
        // assign resource names
        ArmStdC_strcpy(pHandle->AppAcResourceName.MsgQ, MAX_APPAC_NAME_LEN, "ScaFex_TaskMsgQ");
        ArmStdC_strcpy(pHandle->AppAcResourceName.Task, MAX_APPAC_NAME_LEN, "ScaFex_Task");
        ArmStdC_strcpy(pHandle->AppAcResourceName.TaskMtx, MAX_APPAC_NAME_LEN, "ScaFex_TaskMtx");
        ArmStdC_strcpy(pHandle->AppAcResourceName.InBufMtx, MAX_APPAC_NAME_LEN, "ScaFex_InBufMtx");
        ArmStdC_strcpy(pHandle->AppAcResourceName.OutBufMtx, MAX_APPAC_NAME_LEN, "ScaFex_OutBufMtx");
    }

    // alloc private handle
    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## alloc private handle %u", Rval, 0U);
        Rval = ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(AppAcScaFex_t), &PrivateMemblk);
        (void) ArmStdC_memset(PrivateMemblk.pBuffer, 0U, PrivateMemblk.buffer_size);
    }
    if (Rval == ARM_OK) {
        //ArmLog_STR(TASKNAME, "## assign private handle");
        (void) ArmStdC_memcpy(&pAppAcScaFex, &PrivateMemblk.pBuffer, sizeof(void*));
        (void) ArmStdC_memcpy(&(pHandle->private), &pAppAcScaFex, sizeof(void*));

        pScaHandle = &(pAppAcScaFex->ScaHandle);
        pScaHandle->pAlgoObj = &AmbaFexAlgoObj;
//        pHandle->ScaHandle.pAlgoObj = pScaHandle->pAlgoObj;

        //ArmLog_STR(TASKNAME, "## call sca.query", Rval);
        //sca_query
        ScaQueryCfg.pAlgoObj = &AmbaFexAlgoObj;
        Rval = AmbaFexAlgoObj.Query(pScaHandle, &ScaQueryCfg);
        ArmLog_DBG(TASKNAME, "## ScaQueryCfg.TotalReqBufSz=%d, Rval=%d", ScaQueryCfg.TotalReqBufSz, Rval);
        //alloc sca memory and create
        ArmLog_STR(TASKNAME, "## call create sca total buf", NULL, NULL);
        Rval = ArmMemPool_Allocate(pHandle->MemPoolId, ScaQueryCfg.TotalReqBufSz, &AlgoBufMemblk);

        if (Rval == ARM_OK) {

            //ArmLog_STR(TASKNAME, "## call create sca.create"); fflush(stdout);
            ArmStdC_memset(&ScaCreatCfg, 0U, sizeof(ScaCreatCfg));
            ArmStdC_memset(&FlexiBin, 0U, sizeof(FlexiBin));
            ScaCreatCfg.pAlgoBuf = &AlgoBufMemblk;
            ScaCreatCfg.pBin[0] = &FlexiBin;
            ScaCreatCfg.NumFD = 1U;
            pScaHandle->pAlgoCtrl = NULL;
            Rval = AmbaFexAlgoObj.Create(pScaHandle, &ScaCreatCfg);
        }
        //ArmLog_STR(TASKNAME, "## sca created, pScaHandle=%x pScaHandle->pAlgoObj=%x", pScaHandle, pScaHandle->pAlgoObj); fflush(stdout);
        //register callback

    }

    // create Mutex
    ArmLog_DBG(TASKNAME, "## create Mutex %u", Rval, 0U); //fflush(stdout);
    Rval |= ArmMutex_Create(&pHandle->Mutex, &(pHandle->AppAcResourceName.TaskMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->InBufMtx, &(pHandle->AppAcResourceName.InBufMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->OutBufMtx, &(pHandle->AppAcResourceName.OutBufMtx[0]));
    // create message queue
    ArmLog_DBG(TASKNAME, "## create msg queue %u", Rval, 0U); //fflush(stdout);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = AppAc_MsgQueueCreate(pHandle);
        //ArmLog_STR(TASKNAME, "## created msg queue %x", &(pHandle->TaskMsgQ)); fflush(stdout);
    }

    // create input output buffer
    if (Rval == ARM_OK) {
        UINT32 Depth = MAX_BUFFER_DEPTH;
        UINT32 OutBufSize = ScaCreatCfg.OutputSz[0];
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
    ArmLog_DBG(TASKNAME, "## send init command to task %u", Rval, 0U);// fflush(stdout);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_INIT, NULL);
    }

    // wait for ready state
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);

    return Rval;
}


UINT32 AppAc_ScaAmbaFexStart(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_START, NULL);
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_PROC);

    return Rval;
}

UINT32 AppAc_ScaAmbaFexStop(AppAcTaskHandle_t *pHandle)
{
    AmbaMisra_TouchUnused(pHandle);
    return 0U;
}

UINT32 AppAc_ScaAmbaFexDeinit(AppAcTaskHandle_t *pHandle)
{
    AmbaMisra_TouchUnused(pHandle);
    return 0U;
}

