/**
 *  @file AmbaStu_MvacTask.c
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
 *  @details Multi View Auto Calibration algo task, wrap up mvac software function
 *
 */

#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaSTU_MvacTask.h"
#include "cvapi_flexidag_ambamvac_cv2.h"
#include "AmbaMisraFix.h"

#define TASKNAME "Mvac_Task"
#define PROFILE_PROCESS 0U
#define PROFILE_SOLVE_END 1U

//extern UINT32 gAllFrameProcDone;

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
    AppAcMvac_t *pAppAcMvac;
    AMBA_MVAC_FD_HANDLE_s *pMvacFdHandler;
    //AMBA_CV_FMA_BUF_s *pPreFmaBuf;
    AMBA_CV_FMA_BUF_s *pCurFmaBuf;
    AMBA_CV_FEX_BUF_s *pFexBuf;
    AMBA_CV_VO_BUF_s *pVOBuf;
    AMBA_CV_META_BUF_s *pInMetaLeft;
    AMBA_CV_META_BUF_s *pInMetaRight;
    AMBA_CV_MVAC_BUF_s *pOutBuf;
    void *pVoid;

    UINT32 ReadIdx;
    //UINT32 WriteIdx;
    flexidag_memblk_t *pInfo;
    flexidag_memblk_t *pTemp;
    UINT32 FrameNum[3];
    AppAcMetadata_t *pAppAcMetadata;

    //input handler
    (void) ArmStdC_memcpy(&pAppAcMvac, &(pHandle->private), sizeof(void*));
    pMvacFdHandler = &(pAppAcMvac->MvacFdHandler);


//    ArmLog_STR(TASKNAME, "## Process, MVAC STATE=%d, MVAC_STATE_SOLVE=%d", pAppAcMvac->MvacAlgoState, MVAC_STATE_SOLVE); //fflush(stdout);
    if((pAppAcMvac->MvacAlgoState == AMBA_CV_MVAC_STATE_SOLVE)) {
        (void)AppAc_ProfilerTimerStart(pHandle, PROFILE_SOLVE_END);
    }

    //input FexBuf
    ReadIdx = pHandle->InputBuffer[0].ReadIdx;
    FrameNum[0] = pHandle->InputBuffer[0].Desc[ReadIdx].FrameNum;
    pInfo = &(pHandle->InputBuffer[0].Desc[ReadIdx].Info);
    pAppAcMetadata = &(pHandle->InputBuffer[0].Desc[ReadIdx].Metadata);
    (void) ArmStdC_memcpy(&pFexBuf, &pInfo, sizeof(void*));
//    ArmLog_STR(TASKNAME, "## Process, FEX ReadIdx=%d, pInfo->pBuffer=%x, pInfo->buffer_size=%d",
//            ReadIdx, pInfo->pBuffer, pInfo->buffer_size); //fflush(stdout);
    //Rval |= AmbaCV_UtilityCmaMemClean(pInfo);
    //Rval |= AmbaCV_UtilityCmaMemInvalid(pInfo);

    //input CurrFma
    ReadIdx = pHandle->InputBuffer[1].ReadIdx;
    FrameNum[1] = pHandle->InputBuffer[1].Desc[ReadIdx].FrameNum;
    pInfo = &(pHandle->InputBuffer[1].Desc[ReadIdx].Info);
    (void) ArmStdC_memcpy(&pCurFmaBuf, &pInfo, sizeof(void*));
//    ArmLog_STR(TASKNAME, "## Process, FMA ReadIdx=%d, pInfo->pBuffer=%x, pInfo->buffer_size=%d",
//            ReadIdx, pInfo->pBuffer, pInfo->buffer_size); //fflush(stdout);
    //Rval |= AmbaCV_UtilityCmaMemClean(pInfo);
    //Rval |= AmbaCV_UtilityCmaMemInvalid(pInfo);

    //input VoBuf
    ReadIdx = pHandle->InputBuffer[2].ReadIdx;
    FrameNum[2] = pHandle->InputBuffer[2].Desc[ReadIdx].FrameNum;
    pInfo = &(pHandle->InputBuffer[2].Desc[ReadIdx].Info);
    (void) ArmStdC_memcpy(&pVOBuf, &pInfo, sizeof(void*));
//    ArmLog_STR(TASKNAME, "## Process, VO ReadIdx=%d, pInfo->pBuffer=%x, pInfo->buffer_size=%d",
//                ReadIdx, pInfo->pBuffer, pInfo->buffer_size); //fflush(stdout);
    //Rval |= AmbaCV_UtilityCmaMemClean(pInfo);
    //Rval |= AmbaCV_UtilityCmaMemInvalid(pInfo);

    //read metadata
    //... fread the file we need to workaround ...
    pVoid = &(pAppAcMetadata->CalibInfo.Left);
    ArmStdC_memcpy(&pInMetaLeft, &pVoid, sizeof(void*));
    pVoid = &(pAppAcMetadata->CalibInfo.Right);
    ArmStdC_memcpy(&pInMetaRight, &pVoid, sizeof(void*));
//    ArmLog_STR(TASKNAME, "## Process, pInMetaLeft->pBuffer=%x, size=%d",
//            pInMetaLeft->pBuffer, pInMetaLeft->buffer_size); //fflush(stdout);
//    ArmLog_STR(TASKNAME, "## Process, pInMetaRight->pBuffer=%x, size=%d",
//            pInMetaRight->pBuffer, pInMetaRight->buffer_size); //fflush(stdout);

    //output OutBuf
    pTemp = &(pHandle->OutBuf[pHandle->OutputBuffer[0].WriteIdx]);
    (void) ArmStdC_memcpy(&pOutBuf, &(pTemp), sizeof(void*));
//    ArmLog_STR(TASKNAME, "## Process, MVAC OUT WriteIdx=%d, pInfo->pBuffer=%x, pInfo->buffer_size=%d",
//            pHandle->OutputBuffer[0].WriteIdx, pOutBuf->pBuffer, pOutBuf->BufferSize); //fflush(stdout);

    //run

    if(!(FrameNum[0] == FrameNum[1])) {
        ArmLog_DBG(TASKNAME, "## AmbaMVAC_GetResult FrameNum mismatch [FN0=%d, FN1=%d]", FrameNum[0], FrameNum[1]);
    }
    if(!(FrameNum[1] == FrameNum[2])) {
        ArmLog_DBG(TASKNAME, "## AmbaMVAC_GetResult FrameNum mismatch [FN1=%d, FN2=%d]", FrameNum[1], FrameNum[2]);
    }

//    ArmLog_STR(TASKNAME, "## AmbaMVAC_GetResult FN=%d, pMvacFdHandler=%x, pVOBuf=%x, pCurFmaBuf=%x, pFexBuf=%x, pOutBuf=%x",
//            FrameNum[0], pMvacFdHandler, pVOBuf->pBuffer, pCurFmaBuf->pBuffer, pFexBuf->pBuffer, pOutBuf->pBuffer); //fflush(stdout);
    Rval = AmbaMVAC_GetResult(pMvacFdHandler, pVOBuf, pCurFmaBuf, pFexBuf, pInMetaLeft, pInMetaRight, pOutBuf);

    //push to output buf
    if (Rval == ARM_OK) {
        AppAcDesc_t AppAcDesc;

        AppAcDesc.FrameNum = FrameNum[0];
        AppAcDesc.pFromHandle = pHandle;
        AppAcDesc.IsLastFrame = pHandle->InputBuffer[0].Desc[ReadIdx].IsLastFrame;
        ArmStdC_memcpy(&(AppAcDesc.Info), pOutBuf, sizeof(flexidag_memblk_t));
        ArmStdC_memcpy(&(AppAcDesc.Metadata), &(pHandle->InputBuffer[0].Desc[ReadIdx].Metadata), sizeof(AppAcMetadata_t));
        Rval = AppAc_BufferDescPushToAllDownStream(pHandle, &AppAcDesc);
    }

//    ArmLog_STR(TASKNAME, "## Process, MVAC STATE=%d, MVAC_STATE_SOLVE=%d", pAppAcMvac->MvacAlgoState, MVAC_STATE_SOLVE);
    if((pAppAcMvac->MvacAlgoState == AMBA_CV_MVAC_STATE_SOLVE)) {
        (void)AppAc_ProfilerTimerStop(pHandle, PROFILE_SOLVE_END);
    }


    {
        AMBA_CV_MVAC_DATA_s *pOutData = pOutBuf->pBuffer;
//
//        switch (pOutData->State) {
//            case MVAC_STATE_RESET:
//                //printf("[ut]state: RESET\n");
//                break;
//            case MVAC_STATE_WAIT:
//                //printf("[ut]state: WAIT\n");
//                break;
//            case MVAC_STATE_ACCUMULATION:
//                printf("[ut]state: ACCUMULATION\n");
//                if(pOutData->WarpInfoValid == 1){
//                    printf("[ut] generate warp table at MVAC_STATE_ACCUMULATION\n");
//                }
//                break;
//            case MVAC_STATE_SOLVE:
//                printf("[ut]state: SOLVE\n");
//                if(pOutData->WarpInfoValid == 1){
//                    UINT32 WriteSize;
//
//                    printf("[ut] generate warp table at MVAC_STATE_SOLVE\n");
//                    Rval = ArmFIO_Save(&(pOutData->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                            sizeof(pOutData->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                            "/tmp/nfs/ac/out/warp_left.bin",
//                            &WriteSize);
//                }
//                break;
//            case MVAC_STATE_END:
//                printf("[ut]state: END\n");
//                if(pOutData->WarpInfoValid == 1){
//                    UINT32 WriteSize;
//
//                    printf("[ut] generate warp table at MVAC_STATE_END\n");
//                    Rval = ArmFIO_Save(&(pOutData->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                            sizeof(pOutData->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                            "/tmp/nfs/ac/out/warp_left.bin",
//                            &WriteSize);
//
//                }
//
//                gAllFrameProcDone = 1U;
//                break;
//            case MVAC_STATE_SKIP:
//                //printf("[ut]state: SKIP\n");
//                if(pOutData->WarpInfoValid == 1){
//                    printf("[ut] generate warp table at MVAC_STATE_SKIP\n");
//                }
//                break;
//            default:
//                printf("[ut]state: ERROR\n");
//        }
//
//

        pAppAcMvac->MvacAlgoState = pOutData->State;
    }




    return Rval;
}

static UINT32 Idle2Ready(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    ArmLog_DBG(TASKNAME, "## do Idle2Ready", 0U, 0U);

    return Rval;
}

static UINT32 Ready2Proc(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AppAcMvac_t *pAppAcMvac;

    (void) ArmStdC_memcpy(&pAppAcMvac, &(pHandle->private), sizeof(void*));
    ArmLog_DBG(TASKNAME, "## do Ready2Proc", 0U, 0U);

    (void)AppAc_ProfilerTimerReset(pHandle, PROFILE_PROCESS);
    (void)AppAc_ProfilerTimerReset(pHandle, PROFILE_SOLVE_END);
    pAppAcMvac->MvacAlgoState = AMBA_CV_MVAC_STATE_RESET;
    return Rval;
}


static UINT32 Proc2Ready(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    AmbaMisra_TouchUnused(pHandle);

    ArmLog_DBG(TASKNAME, "## do Proc2Ready", 0U, 0U);

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

    *pNextState = pHandle->CurrState;
    switch (Cmd) {
        case APPAC_TASK_CMD_STOP:
            Rval = Proc2Ready(pHandle);
            if (Rval == ARM_OK) {
                *pNextState = APPAC_TASK_STATE_READY;
            }
            break;
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

    Rval = ARM_OK;

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


UINT32 AppAc_AmbaMvacInit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId, flexidag_memblk_t *pInitMetaLeftBuf, AMBA_CV_MVAC_VIEW_INFO_s *pMvacViewInfo)
{
    UINT32 Rval = ARM_OK;
    ARM_TASK_CREATE_ARG_s TaskCreateArg;
    ArmTaskEntryArg_t EntryArg;
    //ArmTask_t *pTask;
    //flexidag_memblk_t AlgoBufMemblk;
    AppAcMvac_t *pAppAcMvac;
    //char TaskName[128];

    //int i;

    flexidag_memblk_t FlexiDagBinBuf;
    uint32_t FlexiDagBinSz = 0;
    uint32_t StateBufSz;
    uint32_t TempBufSz;
    uint32_t OutputBufNum;
    uint32_t OutputBufSz;
    AMBA_CV_MVAC_CFG_s *pMvacConfig;
    AMBA_MVAC_FD_HANDLE_s *pMvacHandler;
    //flexidag_memblk_t StateBuf;
    flexidag_memblk_t TempBuf;

    //AMBA_CV_VO_BUF_s VOBuf;
    //AMBA_CV_FMA_BUF_s FmaBuf;
    //AMBA_CV_FEX_BUF_s FexBuf;
    //flexidag_memblk_t MetaLeftBuf, MetaRightBuf;
    //AMBA_CV_MVAC_BUF_s OutBuf;

    ArmStdC_memset(pHandle, 0U, sizeof(AppAcTaskHandle_t));

    pHandle->ProcInterval = 1U;

    pHandle->CurrState = APPAC_TASK_STATE_IDLE;
    pHandle->UpStreamCallBack = &UpStreamCallback;
    pHandle->DownStreamCallBack = &DownStreamCallback;

    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = ArmMemPool_Allocate(MemPoolId, APPAC_MVAC_MEM_SIZE, &pHandle->AlgoTaskTotalBuf);
        ArmLog_DBG(TASKNAME, "## create mem pool %u", Rval, 0U); //fflush(stdout);
        ArmMemPool_Create(&pHandle->AlgoTaskTotalBuf, &pHandle->MemPoolId);
    }

    if (Rval == ARM_OK) {
        // assign resource names
        ArmStdC_strcpy(pHandle->AppAcResourceName.MsgQ, MAX_APPAC_NAME_LEN, "Mvac_TaskMsgQ");
        ArmStdC_strcpy(pHandle->AppAcResourceName.Task, MAX_APPAC_NAME_LEN, "Mvac_Task");
        ArmStdC_strcpy(pHandle->AppAcResourceName.TaskMtx, MAX_APPAC_NAME_LEN, "Mvac_TaskMtx");
        ArmStdC_strcpy(pHandle->AppAcResourceName.InBufMtx, MAX_APPAC_NAME_LEN, "Mvac_InBufMtx");
        ArmStdC_strcpy(pHandle->AppAcResourceName.OutBufMtx, MAX_APPAC_NAME_LEN, "Mvac_OutBufMtx");
    }

    // alloc private handle
    if (Rval == ARM_OK) {
        //ArmLog_STR(TASKNAME, "## alloc private handle %u", Rval);
        Rval = ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(AppAcMvac_t), &pHandle->AppAcPrivateMemblk);
    }
    if (Rval == ARM_OK) {
        //ArmLog_STR(TASKNAME, "## assign private handle", Rval);
        (void) ArmStdC_memcpy(&pAppAcMvac, &(pHandle->AppAcPrivateMemblk.pBuffer), sizeof(void*));
        (void) ArmStdC_memcpy(&(pHandle->private), &pAppAcMvac, sizeof(void*));
        pMvacConfig = &(pAppAcMvac->MvacConfig);
        pMvacHandler = &(pAppAcMvac->MvacFdHandler);
    }

    if (Rval == ARM_OK) {
        AmbaMVAC_Open(&FlexiDagBinBuf, FlexiDagBinSz, &StateBufSz, &TempBufSz, &OutputBufNum, &OutputBufSz, pMvacHandler);
    }

    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## AmbaMVAC_Open StateBufSz:%d TempBufSz:%d\n", StateBufSz, TempBufSz);
        ArmLog_DBG(TASKNAME, "## AmbaMVAC_Open OutputBufNum:%d OutputBufSz:%d\n", OutputBufNum, OutputBufSz);
        Rval |= ArmMemPool_Allocate(pHandle->MemPoolId, StateBufSz, &pHandle->StateBuf);
        //Rval |= ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(AMBA_CV_VO_DATA_s), &OutBuf);
        Rval |= AmbaMVAC_Init(pMvacHandler, &pHandle->StateBuf, &TempBuf);
    }

    if (Rval == ARM_OK) {

        //sc5
//        pMvacConfig->View.WarpTableInfo.HorizontalGridNumber = 128;
//        pMvacConfig->View.WarpTableInfo.VerticalGridNumber = 96;
//        pMvacConfig->View.WarpTableInfo.TileWidthQ16 = 2097152;
//        pMvacConfig->View.WarpTableInfo.TileHeightQ16 = 2097152;
//        pMvacConfig->View.ScaleId = 0;
        ArmStdC_memcpy(&(pMvacConfig->View), pMvacViewInfo, sizeof(AMBA_CV_MVAC_VIEW_INFO_s));
        //pMvacConfig->pInMetaLeft = pInitMetaLeftBuf;
        ArmStdC_memcpy(&(pMvacConfig->pInMetaLeft), &pInitMetaLeftBuf, sizeof(void*));
        pMvacConfig->MaxIterations = 100;
        pMvacConfig->MinEpipolarScoreThreshold = 0;
        pMvacConfig->MaxFrameSpeedThreshold = 0.5;
        pMvacConfig->MinNumDepthThreshold = 200;
        pMvacConfig->InhibitedFrameSlotSize = 300;
        pMvacConfig->Mode = AMBA_CV_MVAC_MODE_CONFIG;
        ArmLog_DBG(TASKNAME, "## ScaleId = %d", pMvacConfig->View.ScaleId, 0U);
        ArmLog_DBG(TASKNAME, "## WarpTableInfo.HorizontalGridNumber = %d", pMvacConfig->View.WarpTableInfo.HorizontalGridNumber, 0U);
        ArmLog_DBG(TASKNAME, "## WarpTableInfo.VerticalGridNumber   = %d", pMvacConfig->View.WarpTableInfo.VerticalGridNumber, 0U);
        ArmLog_DBG(TASKNAME, "## WarpTableInfo.TileWidthQ16         = %d", pMvacConfig->View.WarpTableInfo.TileWidthQ16, 0U);
        ArmLog_DBG(TASKNAME, "## WarpTableInfo.TileHeightQ16        = %d", pMvacConfig->View.WarpTableInfo.TileHeightQ16, 0U);

        AmbaMVAC_Cfg(pMvacHandler, pMvacConfig);

//        pMvacConfig->Mode = AMBA_CV_MVAC_MODE_CONTROL;
//        pMvacConfig->ControlState = AMBA_CV_MVAC_CONTROL_START;
//        AmbaMVAC_Cfg(pMvacHandler, pMvacConfig);
    }

    // create Mutex
    //ArmLog_STR(TASKNAME, "## create Mutex %u",Rval);
    Rval |= ArmMutex_Create(&pHandle->Mutex, &(pHandle->AppAcResourceName.TaskMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->InBufMtx, &(pHandle->AppAcResourceName.InBufMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->OutBufMtx, &(pHandle->AppAcResourceName.OutBufMtx[0]));

    // create message queue
    //ArmLog_STR(TASKNAME, "## create msg queue %u",Rval);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = AppAc_MsgQueueCreate(pHandle);
        //ArmLog_STR(TASKNAME, "## created msg queue %x", &(pHandle->TaskMsgQ));
    }

    // create input output buffer
    if (Rval == ARM_OK) {
        UINT32 Depth = MAX_BUFFER_DEPTH;
        UINT32 OutBufSize = sizeof(AMBA_CV_MVAC_DATA_s);
        Rval = AppAc_BufferInit(pHandle, Depth, OutBufSize);
    }

    // create task
    //ArmLog_STR(TASKNAME, "## create task %u",Rval);
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
    //ArmLog_STR(TASKNAME, "## send init command to task %u", Rval);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_INIT, NULL);
    }

    // wait for ready state
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);

    return Rval;
}


UINT32 AppAc_AmbaMvacStart(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_START, NULL);
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_PROC);

    return Rval;
}

UINT32 AppAc_AmbaMvacStop(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacStop", 0U, 0U); //fflush(stdout);
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_PROC);
    ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacStop: got proc state", 0U, 0U); //fflush(stdout);
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_STOP, NULL);
    ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacStop: snet stop command", 0U, 0U); //fflush(stdout);
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacStop: got ready state", 0U, 0U); //fflush(stdout);

    return Rval;
}

UINT32 AppAc_AmbaMvacDeinit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId)
{
    UINT32 Rval = ARM_OK;

    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);

    // delete task
    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacDeinit: delete task", 0U, 0U); //fflush(stdout);
        Rval |= ArmTask_Delete(&(pHandle->Task.Tsk));
        Rval |= ArmMemPool_Free(pHandle->MemPoolId, &(pHandle->Task.StackBuf));
    }
    // delete buffer
    if (Rval == ARM_OK) {
        UINT32 Depth = MAX_BUFFER_DEPTH;

        ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacDeinit: delete buffer", 0U, 0U); //fflush(stdout);
        Rval |= AppAc_BufferDeinit(pHandle, Depth);
    }
    // delete message queue
    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacDeinit: delete msg queue", 0U, 0U); //fflush(stdout);
        Rval |= AppAc_MsgQueueDelete(pHandle);
    }

    // delete Mutex
    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacDeinit: delete mutex", 0U, 0U); //fflush(stdout);
        Rval |= ArmMutex_Delete(&pHandle->Mutex);
        Rval |= ArmMutex_Delete(&pHandle->InBufMtx);
        Rval |= ArmMutex_Delete(&pHandle->OutBufMtx);
    }

    // delete algo
    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacDeinit: delete algo StateBuf", 0U, 0U); //fflush(stdout);
        Rval |= ArmMemPool_Free(pHandle->MemPoolId, &pHandle->StateBuf);
    }

    // delete private
    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacDeinit: delete algo private", 0U, 0U); //fflush(stdout);
        Rval |= ArmMemPool_Free(pHandle->MemPoolId, &pHandle->AppAcPrivateMemblk);
    }

    // delete AlgoTaskTotalBuf
    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## AppAc_AmbaMvacDeinit: delete algo AlgoTaskTotalBuf", 0U, 0U); //fflush(stdout);
        Rval |= ArmMemPool_Delete(pHandle->MemPoolId);
        Rval |= ArmMemPool_Free(MemPoolId, &pHandle->AlgoTaskTotalBuf);
    }

    return 0U;
}

