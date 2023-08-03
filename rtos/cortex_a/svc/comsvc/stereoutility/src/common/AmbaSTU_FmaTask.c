/**
 *  @file AmbaStu_FmaTask.c
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
 *  @details FMA algo task, wrap up SCA FMA flexidag
 *
 */

#include "stdio.h"
#include "AmbaSTU_AlgoTaskUtil.h"
#include "AmbaSTU_FmaTask.h"
#include "cvapi_svccvalgo_ambafma.h"
#include "cvapi_flexidag_ambafma_cv2.h"
#include "AmbaMisraFix.h"

#define TASKNAME "ScaFma_Task"
#define PROFILE_PROCESS 0U

extern SVC_CV_ALGO_OBJ_s AmbaFmaAlgoObj;

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


static void Config_AmbaFmaMvac(AMBA_CV_FMA_MVAC_CFG_s *pCfg, const UINT32 ScaleId){
    UINT32 i;

    //set fma default config
    for(i=0;i<MAX_HALF_OCTAVES; ++i){
        pCfg->ScaleCfg[i].Mode = 0U;
    }
    pCfg->ScaleCfg[ScaleId].Mode = (UINT8)CV_FMA_ENABLE_ALL;
    pCfg->ScaleCfg[ScaleId].StereoMatch.SearchRadiusX = 1U;
    pCfg->ScaleCfg[ScaleId].StereoMatch.SearchRadiusY = 1U;
    pCfg->ScaleCfg[ScaleId].StereoMatch.DistanceThUp = 45.7f;
    pCfg->ScaleCfg[ScaleId].StereoMatch.DistanceThDown = 45.7f;
    pCfg->ScaleCfg[ScaleId].StereoMatch.MatchScoreTh = 50U;

    pCfg->ScaleCfg[ScaleId].TemporalMatch.SearchRadiusX = 1U;
    pCfg->ScaleCfg[ScaleId].TemporalMatch.SearchRadiusY = 2U;
    pCfg->ScaleCfg[ScaleId].TemporalMatch.MatchScoreTh = 50U;
}


static UINT32 Process(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;
    //UINT32 IsInputEmpty;
    //UINT32 IsOutputFull;
    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    AppAcScaFma_t *pAppAcScaFma;
    UINT32 FrameNum;
    UINT32 ReadIdx;
    AppAcBuffer_t *pInDescBuf;
    AppAcDesc_t *pInDesc;

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

        ArmStdC_memcpy(&(pAppAcScaFma), &(pHandle->private), sizeof(void*));

        pScaHandle = &(pAppAcScaFma->ScaHandle);

        // Input Fex
        pInDescBuf = &(pHandle->InputBuffer[0]);
        ReadIdx = pInDescBuf->ReadIdx;
        pInDesc = &(pInDescBuf->Desc[ReadIdx]);

        ScaFeedCfg.pIn = &InBuf;
        ScaFeedCfg.pIn->num_of_buf = 1U;
        FrameNum = pInDesc->FrameNum;
        ArmStdC_memcpy(
                &(ScaFeedCfg.pIn->buf[0]),
                &(pInDesc->Info),
                sizeof(flexidag_memblk_t));
        //Rval |= AmbaCV_UtilityCmaMemClean(&(ScaFeedCfg.pIn->buf[0]));
        //Rval |= AmbaCV_UtilityCmaMemInvalid(&(ScaFeedCfg.pIn->buf[0]));
        // Output Fma
        ScaFeedCfg.pOut = &OutBuf;
        ScaFeedCfg.pOut->num_of_buf = 1U;
        ArmStdC_memcpy(
                &(ScaFeedCfg.pOut->buf[0]),
                &(pAppAcScaFma->FmaOut),
                sizeof(flexidag_memblk_t));

        // Run
//        ArmLog_STR(TASKNAME, "## AmbaFma_GetResult FN=%d, pScaHandle=%x, pFexBuf=%x, pOutBuf=%x",
//                FrameNum, pScaHandle, ScaFeedCfg.pIn->buf[0].pBuffer, ScaFeedCfg.pOut->buf[0].pBuffer); //fflush(stdout);
        Rval |= pScaHandle->pAlgoObj->Feed(pScaHandle, &ScaFeedCfg);

        // collect output from the one FmaOut
        pOutBuf = &(pHandle->OutBuf[pHandle->OutputBuffer[0].WriteIdx]);
        AmbaCV_UtilityCmaMemInvalid(&(ScaFeedCfg.pOut->buf[0]));
        ArmStdC_memcpy(pOutBuf->pBuffer, ScaFeedCfg.pOut->buf[0].pBuffer, ScaFeedCfg.pOut->buf[0].buffer_size);
//        ArmLog_STR(TASKNAME, "## AmbaFma_GetResult load output to %x from %x, size %d",
//                pOutBuf->pBuffer, ScaFeedCfg.pOut->buf[0].pBuffer, ScaFeedCfg.pOut->buf[0].buffer_size);

#define MaxFileNameLen 128U
#define FILE_PATH "/sd0/scale2_image/8_1/"
        if (0) {
            char FmaOutFile[128] = FILE_PATH"out/fma/Fma_";
            char FexOutFile[128] = FILE_PATH"out/fma/Fex_";
            char FileIndex[32];
            char *pBuf;
            UINT32 Size;
            UINT32 WriteSize;

            sprintf(FileIndex, "%d", FrameNum);
            ArmStdC_strcat(FmaOutFile, MaxFileNameLen, FileIndex);
            ArmStdC_strcat(FmaOutFile, MaxFileNameLen, ".bin");
            ArmLog_STR(TASKNAME, "## dump fma: %s", FmaOutFile, NULL);

            pBuf = pOutBuf->pBuffer;
            Size = pOutBuf->buffer_size;
            ArmLog_U64(TASKNAME, "## : ArmFIO_Save pBuf=%llx, size=%llu", (UINT64)pBuf, (UINT64)Size);
            Rval = ArmFIO_Save(pBuf, Size, FmaOutFile, &WriteSize);

            sprintf(FileIndex, "%d", FrameNum);
            ArmStdC_strcat(FexOutFile, MaxFileNameLen, FileIndex);
            ArmStdC_strcat(FexOutFile, MaxFileNameLen, ".bin");
            ArmLog_STR(TASKNAME, "## dump fex: %s", FexOutFile, NULL);

            pBuf = ScaFeedCfg.pIn->buf[0].pBuffer;
            Size = ScaFeedCfg.pIn->buf[0].buffer_size;
            ArmLog_U64(TASKNAME, "## : ArmFIO_Save pBuf=%llx, size=%llu", (UINT64)pBuf, (UINT64)Size);
            Rval = ArmFIO_Save(pBuf, Size, FexOutFile, &WriteSize);

        }


        //push to output buf
        if (Rval == ARM_OK) {
            AppAcDesc_t AppAcDesc;

            AppAcDesc.FrameNum = FrameNum;
            AppAcDesc.pFromHandle = pHandle;
            AppAcDesc.IsLastFrame = pInDesc->IsLastFrame;
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

//if(pMsg->pDesc != NULL) {
//    ArmLog_STR(TASKNAME, "## caller: %s, pMsg->pDesc=%x", pMsg->pDesc->pFromHandle->AppAcResourceName.Task, pMsg->pDesc); //fflush(stdout);
//}
    //find who callbacks
    Rval = AppAc_WhosCall(pHandle, pMsg->Desc.pFromHandle, &Index, FROM_DOWN_STREAM);

    // update rp of corresponding output buffer
    //if (Rval == ARM_OK) {
        Rval = AppAc_BufferUprp(pHandle, &(pHandle->OutputBuffer[Index]));
    //}

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

UINT32 AppAc_ScaAmbaFmaInit(AppAcTaskHandle_t *pHandle, UINT32 MemPoolId, char *FlexiBinFileName, UINT32 ScaleId)
{
    UINT32 Rval = ARM_OK;
    ARM_TASK_CREATE_ARG_s TaskCreateArg;
    ArmTaskEntryArg_t EntryArg;
    //ArmTask_t *pTask;
    //SVC_CV_ALGO_QUERY_CFG_s QueryCfg;
    flexidag_memblk_t AlgoTaskTotalBuf;
    flexidag_memblk_t PrivateMemblk;
    flexidag_memblk_t AlgoBufMemblk;
    AppAcScaFma_t *pAppAcScaFma;
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
        Rval = ArmMemPool_Allocate(MemPoolId, APPAC_SCA_FMA_MEM_SIZE, &AlgoTaskTotalBuf);
        ArmLog_DBG(TASKNAME, "## create mem pool %u", Rval, 0U); fflush(stdout);
        ArmMemPool_Create(&AlgoTaskTotalBuf, &pHandle->MemPoolId);
    }

    if (Rval == ARM_OK) {
        // assign resource names
        ArmStdC_strcpy(pHandle->AppAcResourceName.MsgQ, MAX_APPAC_NAME_LEN, "ScaFma_TaskMsgQ");
        ArmStdC_strcpy(pHandle->AppAcResourceName.Task, MAX_APPAC_NAME_LEN, "ScaFma_Task");
        ArmStdC_strcpy(pHandle->AppAcResourceName.TaskMtx, MAX_APPAC_NAME_LEN, "ScaFma_TaskMtx");
        ArmStdC_strcpy(pHandle->AppAcResourceName.InBufMtx, MAX_APPAC_NAME_LEN, "ScaFma_InBufMtx");
        ArmStdC_strcpy(pHandle->AppAcResourceName.OutBufMtx, MAX_APPAC_NAME_LEN, "ScaFma_OutBufMtx");
    }

    // alloc private handle
    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## alloc private handle", 0U, 0U);
        Rval = ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(AppAcScaFma_t), &PrivateMemblk);
        (void) ArmStdC_memset(PrivateMemblk.pBuffer, 0U, PrivateMemblk.buffer_size);
    }
    if (Rval == ARM_OK) {
        ArmLog_DBG(TASKNAME, "## assign private handle", 0U, 0U);
        (void) ArmStdC_memcpy(&pAppAcScaFma, &PrivateMemblk.pBuffer, sizeof(void*));
        (void) ArmStdC_memcpy(&(pHandle->private), &pAppAcScaFma, sizeof(void*));

        pScaHandle = &(pAppAcScaFma->ScaHandle);
        ArmStdC_memset(pScaHandle, 0U, sizeof(SVC_CV_ALGO_HANDLE_s));
        pScaHandle->pAlgoObj = &AmbaFmaAlgoObj;
//        pHandle->ScaHandle.pAlgoObj = pScaHandle->pAlgoObj;

        //ArmLog_STR(TASKNAME, "## call sca.query", Rval);
        //sca_query
        ScaQueryCfg.pAlgoObj = &AmbaFmaAlgoObj;
        Rval = AmbaFmaAlgoObj.Query(pScaHandle, &ScaQueryCfg);
        ArmLog_DBG(TASKNAME, "## ScaQueryCfg.TotalReqBufSz=%d, Rval=%d", ScaQueryCfg.TotalReqBufSz, Rval);
        //alloc sca memory and create
        //ArmLog_STR(TASKNAME, "## call create sca total buf", Rval);
        Rval = ArmMemPool_Allocate(pHandle->MemPoolId, ScaQueryCfg.TotalReqBufSz, &AlgoBufMemblk);

        if (Rval == ARM_OK) {

            //ArmLog_STR(TASKNAME, "## call create sca.create", Rval);
            //ArmLog_STR(TASKNAME, "## memset ScaCreatCfg %llx, size %llu", &ScaCreatCfg, sizeof(ScaCreatCfg));
            ArmStdC_memset(&ScaCreatCfg, 0U, sizeof(ScaCreatCfg));
            ArmStdC_memset(&FlexiBin, 0U, sizeof(FlexiBin));
            pScaHandle->pAlgoCtrl = NULL;
            ScaCreatCfg.pAlgoBuf = &AlgoBufMemblk;
            ScaCreatCfg.pBin[0] = &FlexiBin;
            ScaCreatCfg.NumFD = 1U;
            Rval = AmbaFmaAlgoObj.Create(pScaHandle, &ScaCreatCfg);
            //ArmLog_STR(TASKNAME, "## call create sca.create", Rval);
        }
        //ArmLog_STR(TASKNAME, "## sca created, pScaHandle=%x pScaHandle->pAlgoObj=%x", pScaHandle, pScaHandle->pAlgoObj);
        if (Rval == ARM_OK) {
            SVC_CV_ALGO_CTRL_CFG_s ScaCtrlCfg;
            AMBA_CV_FMA_MVAC_CFG_s FmaCfg;

            Config_AmbaFmaMvac(&FmaCfg, ScaleId);
            ScaCtrlCfg.CtrlType = AMBA_FMA_CONTROL_TYPE_MVAC;
            ScaCtrlCfg.pCtrlParam = &FmaCfg;
            AmbaFmaAlgoObj.Control(pScaHandle, &ScaCtrlCfg);
        }
        if (Rval == ARM_OK) {
            UINT32 OutBufSize = ScaCreatCfg.OutputSz[0];

            // create one for fma feed use, and then copy it to outbuf
            Rval |= ArmMemPool_Allocate(pHandle->MemPoolId, OutBufSize, &(pAppAcScaFma->FmaOut));
        }
        //register callback
    }

    // create Mutex
    ArmLog_DBG(TASKNAME, "## create Mutex %u", Rval, 0U);
    Rval |= ArmMutex_Create(&pHandle->Mutex, &(pHandle->AppAcResourceName.TaskMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->InBufMtx, &(pHandle->AppAcResourceName.InBufMtx[0]));
    Rval |= ArmMutex_Create(&pHandle->OutBufMtx, &(pHandle->AppAcResourceName.OutBufMtx[0]));

    // create message queue
    ArmLog_DBG(TASKNAME, "## create msg queue %u", Rval, 0U);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        Rval = AppAc_MsgQueueCreate(pHandle);
        //ArmLog_STR(TASKNAME, "## created msg queue %x", &(pHandle->TaskMsgQ));
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
    ArmLog_DBG(TASKNAME, "## send init command to task %u", Rval, 0U);
    if ((Rval == ARM_OK) && (pHandle != NULL)) {
        AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_INIT, NULL);
    }

    // wait for ready state
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);

    return Rval;
}


UINT32 AppAc_ScaAmbaFmaStart(AppAcTaskHandle_t *pHandle)
{
    UINT32 Rval = ARM_OK;

    AppAc_WaitState(pHandle, APPAC_TASK_STATE_READY);
    AppAc_MsgQueueSend(pHandle, APPAC_TASK_CMD_START, NULL);
    AppAc_WaitState(pHandle, APPAC_TASK_STATE_PROC);

    return Rval;
}

UINT32 AppAc_ScaAmbaFmaStop(AppAcTaskHandle_t *pHandle)
{
    AmbaMisra_TouchUnused(pHandle);
    return 0U;
}

UINT32 AppAc_ScaAmbaFmaDeinit(AppAcTaskHandle_t *pHandle)
{
    AmbaMisra_TouchUnused(pHandle);
    return 0U;
}

