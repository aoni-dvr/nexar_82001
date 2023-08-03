/**
 *  @file SvcRegressionTask.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details svc cv regression
 *
 */


#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaDMA_Def.h"

/* svc-framework */
#include "AmbaDSP_Liveview.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

/* svc-shared */
#include "SvcResCfg.h"
#include "SvcBufMap.h"
#include "SvcCvFlow_Comm.h"
#include "SvcSysStat.h"
#include "SvcTask.h"
#include "SvcPlat.h"

#include "SvcCvAlgo.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_RefSeg.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvAppDef.h"
#include "SvcRegressionTask.h"

#include "AmbaPlayer.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_Event_Def.h"
#include "SvcPbkCtrl.h"
#include "RefCmptPlayerImpl.h"
#include "AmbaFS.h"
#include "AmbaCache.h"
#include "AmbaIPC_Regression.h"
#include "AmbaIPC_Stream.h"

#include "cvapi_amba_mask_interface.h"
#include "cv_ambaod_header.h"

#define SVC_LOG_REGRESSION     "SVC_LOG_REGRESSION"

static UINT8 G_PlayMethod = 0U;
static UINT32 G_FrameNum = 0U;
static UINT8 G_PipeNum = 0U;
static UINT32 G_NumOfDecodedPic = 0U;
static UINT8 G_EndOfStream = 0U;

AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s   *pDECStatus;

#define PLAY_NORMAL_MODE    1U
#define PLAY_CMPT_MODE       2U

#define ALGOTYPE_CLASSIFICATION   (0U)
#define ALGOTYPE_OBJDETRCTION     (1U)
#define ALGOTYPE_SEGMENTATION     (2U)

//-- For Linux to execute RTOS command --
#define REGREXEC_CMD_MAXLENTH (128U)
#define REGREXEC_QUEUE_LENTH (32U)
#define REGREXEC_STACK_SIZE  (512U*1024U)
typedef struct {
    UINT32 idx;
} RegrExecMsg_s;

static UINT32 RegExecCmdIdx = 0;
static char RegExecCmdBuf[REGREXEC_QUEUE_LENTH][REGREXEC_CMD_MAXLENTH];
static RegrExecMsg_s RegrExecMsg[REGREXEC_QUEUE_LENTH] = {0};
static AMBA_KAL_MSG_QUEUE_t RegrExecQueue;
static SVC_TASK_CTRL_s RegrExecTask;
static UINT8 RegrExecTaskStack[REGREXEC_STACK_SIZE];
//-- For Linux to execute RTOS command -- end

/**
* Play Method of Reression
* @param [in] Method
*/
void SvcCvRegressionPlayMethod(UINT8 Method)
{
    G_PlayMethod = Method;
}

/**
* Set Play Frame Num
* @param [in] FrameNum
*/
void SvcCvRegressionSetPlayFrameNum(UINT32 FrameNum)
{
    G_FrameNum = FrameNum;
}

/**
* Get Play Frame Num
* @param [out] FrameNum
*/
void SvcCvRegressionGetPlayFrameNum(UINT32 *FrameNum)
{
     *FrameNum = G_FrameNum;
}

/**
* Set Pipe Num
* @param [in] PipeNum
*/
void SvcCvRegressionSetCmptPipeNum(UINT8 PipeNum)
{
     G_PipeNum = PipeNum ;
}

/**
* Get Pipe Num
* @param [out] PipeNum
*/
void SvcCvRegressionGetCmptPipeNum(UINT8 *PipeNum)
{
     *PipeNum = G_PipeNum;
}

/**
* Callback function for VideoDec
* @param [in] pEventData
*/
static UINT32 SvcCvRegressionVideoDecCB(const void *pEventData)
{
    UINT32 RetVal = SVC_OK;
    AmbaMisra_TypeCast(&pDECStatus, &pEventData);
    G_NumOfDecodedPic = pDECStatus->NumOfDecodedPic;
    G_EndOfStream = pDECStatus->EndOfStream;
    return RetVal;
}

/**
* Callback function for Detection Result CV
* @param [in] Chan
* @param [in] CvType
* @param [in] pDetResMsg
*/
static void SvcCvRegressionODResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg)
{
    SVC_CV_DETRES_BBX_LIST_s *pODResult = (SVC_CV_DETRES_BBX_LIST_s*)pDetResMsg;
    amba_od_out_t *pODout = (amba_od_out_t*)pODResult->pCvOut;
    UINT32 FrameNum = 0U;
    UINT8 PipeNum = 0U;
    UINT32 RetVal = SVC_OK;
    SvcCvRegressionGetPlayFrameNum(&FrameNum);
    SvcPlat_CacheClean((UINT32)pODout, sizeof(amba_od_out_t));
    AmbaIPC_RegrWrite(FrameNum, ALGOTYPE_OBJDETRCTION, (UINT32)pODout, sizeof(amba_od_out_t));

    if ((G_EndOfStream!=0U) && (G_NumOfDecodedPic == (FrameNum+1U)) && (FrameNum!=0U)){
        SvcLog_DBG(SVC_LOG_REGRESSION, "AmbaIPC_RegrStop", 0U, 0U);
        SvcLog_DBG(SVC_LOG_REGRESSION, "RegressionODCallback, G_EndOfStream:%d, G_NumOfDecodedPic:%d", G_EndOfStream, G_NumOfDecodedPic);
        if (SVC_OK != SvcPbkCtrl_Stop(1U)) {
            RetVal = 1U;
        }

        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_REGRESSION, "stop video failed!!", 0U, 0U);
        } else {
            SvcLog_OK(SVC_LOG_REGRESSION, "stop video successfully!!", 0U, 0U);
        }


        if (SVC_OK != SvcPbkCtrl_TaskDelete(0U)) {
            RetVal = 1U;
        }

        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_REGRESSION, "stop delete failed!!", 0U, 0U);
        } else {
            SvcLog_OK(SVC_LOG_REGRESSION, "stop delete successfully!!", 0U, 0U);
        }

        AmbaIPC_RegrStop();
    } else {
        if (G_PlayMethod == PLAY_NORMAL_MODE) {
            SvcPbkCtrl_TrickPlay(1U, AMBA_PLAYER_TRICK_STEP);
        } else if (G_PlayMethod == PLAY_CMPT_MODE) {
            SvcCvRegressionGetCmptPipeNum(&PipeNum);
            RefCmptPlayer_Step(PipeNum);
        } else {
            SvcLog_NG(SVC_LOG_REGRESSION, "Play Method err. ", 0U, 0U);
        }
    }

   FrameNum++;
   SvcCvRegressionSetPlayFrameNum(FrameNum);
   AmbaMisra_TouchUnused(&Chan);
   AmbaMisra_TouchUnused(&CvType);
}

/**
* Callback function for Detection Result CV
* @param [in] Chan
* @param [in] CvType
* @param [in] pDetResMsg
*/
static void SvcCvRegressionSEGResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg)
{
    SVC_CV_DETRES_SEG_s *pSegResult = (SVC_CV_DETRES_SEG_s*)pDetResMsg;
    amba_mask_out_t *pSegMaskOut = (amba_mask_out_t*)pSegResult->pCvOut;
    UINT32 FrameNum = 0;
    UINT8 PipeNum = 0U;
    UINT32 RetVal = SVC_OK;
    SvcCvRegressionGetPlayFrameNum(&FrameNum);
    SvcPlat_CacheClean((UINT32)pSegMaskOut, sizeof(amba_mask_out_t));
    SvcPlat_CacheClean((UINT32)pSegMaskOut->buff_addr, pSegMaskOut->buff_width*pSegMaskOut->buff_height);
    AmbaIPC_RegrWrite(FrameNum, ALGOTYPE_SEGMENTATION, (UINT32)pSegMaskOut, sizeof(amba_mask_out_t));

    if ((G_EndOfStream!=0U) && (G_NumOfDecodedPic == (FrameNum+1U)) && (FrameNum!=0U)){
        SvcLog_DBG(SVC_LOG_REGRESSION, "AmbaIPC_RegrStop", 0U, 0U);
        SvcLog_DBG(SVC_LOG_REGRESSION, "RegressionSEGCallback, G_EndOfStream:%d, G_NumOfDecodedPic:%d", G_EndOfStream, G_NumOfDecodedPic);
        if (SVC_OK != SvcPbkCtrl_Stop(1U)) {
            RetVal = 1U;
        }

        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_REGRESSION, "stop video failed!!", 0U, 0U);
        } else {
            SvcLog_OK(SVC_LOG_REGRESSION, "stop video successfully!!", 0U, 0U);
        }


        if (SVC_OK != SvcPbkCtrl_TaskDelete(0U)) {
            RetVal = 1U;
        }

        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_REGRESSION, "stop delete failed!!", 0U, 0U);
        } else {
            SvcLog_OK(SVC_LOG_REGRESSION, "stop delete successfully!!", 0U, 0U);
        }

        AmbaIPC_RegrStop();
    }else {
        if (G_PlayMethod == PLAY_NORMAL_MODE) {
            SvcPbkCtrl_TrickPlay(1U, AMBA_PLAYER_TRICK_STEP);
        } else if (G_PlayMethod == PLAY_CMPT_MODE) {
            SvcCvRegressionGetCmptPipeNum(&PipeNum);
            RefCmptPlayer_Step(PipeNum);
        } else {
            SvcLog_NG(SVC_LOG_REGRESSION, "Play Method err. ", 0U, 0U);
        }
    }

    FrameNum++;
    SvcCvRegressionSetPlayFrameNum(FrameNum);
    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
#if 0
    AMBA_FS_FILE *pFile;
    UINT32  Rval = SVC_OK, Count = 0U;

    Rval = AmbaFS_FileOpen("c:\\a.bin", "w+", &pFile);
    if (Rval == SVC_OK){
        Rval = AmbaFS_FileWrite(pSegResult->pBuf, pSegResult->Width* pSegResult->Height, 1U, pFile, &Count);
        if (Rval == SVC_OK){
            Rval = AmbaFS_FileClose(pFile);
            if (Rval == SVC_OK){
                SvcLog_OK(SVC_LOG_REGRESSION, "Save SegOut Ok. w: %d, h: %d", pSegResult->Width, pSegResult->Height);
            } else {
                SvcLog_NG(SVC_LOG_REGRESSION, "File Close Err. ", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_REGRESSION, "File Write Err. ", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_REGRESSION, "File Open Err. ", 0U, 0U);
    }
#endif

}

/**
* Cc Regression init
* @return ErrorCode
*/
UINT32 SvcCvRegression_Init(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 i;
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ) {
                UINT32 CvFlowRegisterID = 0U;
                RetVal |= SvcCvFlow_Register(i, SvcCvRegressionODResultCallback, &CvFlowRegisterID);
            } else if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG) ||
                (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG) ) {
                UINT32 CvFlowRegisterID = 0U;
                RetVal |= SvcCvFlow_Register(i, SvcCvRegressionSEGResultCallback, &CvFlowRegisterID);
            } else {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_REGRESSION, "CvFlowType:%d, not support! ", pCvFlow[i].CvFlowType, 0U);
            }

            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REGRESSION, "SvcCvRegression_Init err. ", 0U, 0U);
            } else {
                SvcLog_OK(SVC_LOG_REGRESSION, "SvcCvRegression_Init Success ", 0U, 0U);
            }
        }
    }

    RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DEC_STATUS, SvcCvRegressionVideoDecCB);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_REGRESSION, "AmbaDSP_EventHandlerRegister err. ", 0U, 0U);
    }

    return RetVal;
}


//-- For Linux to execute RTOS command --
static void* RegressionExecTaskEntry(void* EntryArg)
{
    UINT32 Rval;
    RegrExecMsg_s Req;
    extern UINT32 AmbaShell_ExecCommand(char *pCmdLine);

    AmbaMisra_TouchUnused(EntryArg);
    for(;;) {
        Rval = AmbaKAL_MsgQueueReceive(&RegrExecQueue, &Req, AMBA_KAL_WAIT_FOREVER);
        if (KAL_ERR_NONE == Rval) {
            AmbaPrint_PrintStr5("[dequeue] %s", RegExecCmdBuf[Req.idx], NULL, NULL, NULL, NULL);
            Rval = AmbaShell_ExecCommand(RegExecCmdBuf[Req.idx]);
        }
    }

    return NULL;
}

void SvcCvRegression_ExecInit(void)
{
    UINT32 Rval;

    SvcLog_DBG(SVC_LOG_REGRESSION, "SvcCvRegression_ExecInit start",0,0);

    Rval = AmbaKAL_MsgQueueCreate(&RegrExecQueue,
                                  "RegrExecQueue",
                                  sizeof(RegrExecMsg_s),
                                  &RegrExecMsg,
                                  REGREXEC_QUEUE_LENTH * sizeof(RegrExecMsg_s));
    if (KAL_ERR_NONE != Rval) {
        SvcLog_NG(SVC_LOG_REGRESSION, "SvcCvRegression_ExecInit: AmbaKAL_MsgQueueCreate() fail. %u", Rval, 0);
    } else {
        RegrExecTask.Priority   = 125;
        RegrExecTask.EntryFunc  = RegressionExecTaskEntry;
        RegrExecTask.EntryArg   = 0;
        RegrExecTask.pStackBase = &RegrExecTaskStack;
        RegrExecTask.StackSize  = REGREXEC_STACK_SIZE;
        RegrExecTask.CpuBits    = 0x01U;
        Rval = SvcTask_Create("RegrExecTask", &RegrExecTask);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_REGRESSION, "SvcCvRegression_ExecInit: SvcTask_Create() fail. %u", Rval, 0);
        }
    }
}

void SvcCvRegression_ExecAddCmd(char *Cmd)
{
    UINT32 Rval;
    RegrExecMsg_s Msg;
    char *pCmdBuf;

    AmbaPrint_PrintStr5("[enqueue] %s", Cmd, NULL, NULL, NULL, NULL);

    pCmdBuf = &RegExecCmdBuf[RegExecCmdIdx][0];

    SvcWrap_strcpy(pCmdBuf, REGREXEC_CMD_MAXLENTH, Cmd);
    Msg.idx = RegExecCmdIdx;

    RegExecCmdIdx++;
    if (RegExecCmdIdx >= REGREXEC_QUEUE_LENTH) {
        RegExecCmdIdx = 0;
    }

    Rval = AmbaKAL_MsgQueueSend(&RegrExecQueue, &Msg, AMBA_KAL_NO_WAIT);
    if (KAL_ERR_NONE != Rval) {
        SvcLog_NG(SVC_LOG_REGRESSION, "SvcCvRegression_ExecAddCmd: AmbaKAL_MsgQueueSend() fail. %u", Rval, 0);
    }
}
//-- For Linux to execute RTOS command -- end

