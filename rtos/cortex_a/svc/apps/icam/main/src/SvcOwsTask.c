/**
 *  @file SvcOwsTask.c
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
 *  @details Svc Ows Object Detection task
 *
 */
#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaCalib_AVMIF.h"
#include "AmbaCT_AvmTunerIF.h"
#include "AmbaCalib_EmirrorDef.h"

#include "AmbaOWS_ObstacleDetect.h"
#include "PrepareAVMCalib.h"


#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcOwsTask.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvAppDef.h"
#include "SvcCalibAdas.h"
#include "SvcSurDrawTask.h"


#define SVC_LOG_OWS_TASK     "OWS_TASK"
#define SVC_OWS_QUEUE_SIZE        128U

#define SVC_OWS_TASK_STACK_SIZE (0xF000)

static UINT8 SvcOwsInit = 0U;
static UINT8 OwsCalibLoad = 0U; //Current always 1;
static UINT8 SvcOwsOn = 0U;
static AMBA_KAL_MSG_QUEUE_t    SvcOwsQueId GNU_SECTION_NOZEROINIT;
static AMBA_OWS_CROP_INFO_s SegCropInfo;

static void SvcOwsTask_DetectionResultCallback(UINT32 Chan, UINT32 OutputTag, void *pDetResMsg);

static void* SvcOwsTaskEntry(void* EntryArg)
{
    UINT8 IsTrue = 1U;
    UINT32 Rval;
    static SVC_CV_DETRES_SEG_s SegResult;

    static AMBA_OWS_BUF_INFO_s SegBufInfo;
    static AMBA_OWS_PROC_INPUT_DATA_s OWSIn;
    static AMBA_OWS_RESULT_s OWSOut;

    while (0U != IsTrue) {
        Rval = AmbaKAL_MsgQueueReceive(&SvcOwsQueId, &SegResult, AMBA_KAL_WAIT_FOREVER);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_OWS_TASK, "SvcLdwsQueId MsgQue Receive error !", 0U, 0U);
        } else {
            //Do nothing
        }
        SegBufInfo.Height = SegResult.Height;
        SegBufInfo.Width = SegResult.Width;
        SegBufInfo.Pitch = SegResult.Pitch;
        SegBufInfo.pBuf = SegResult.pBuf;

        OWSIn.TimeStamp = SegResult.CaptureTime;
        OWSIn.pSegBufInfo = &SegBufInfo;
        OWSIn.pSegBufCropInfo = &SegCropInfo;

        Rval = AmbaOWS_Process(SegResult.MsgCode, &OWSIn, &OWSOut);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_OWS_TASK, "AmbaOWS_Process error !", 0U, 0U);
        } else {
            const void *DrawOut;
            const AMBA_OWS_RESULT_s *OutData = &OWSOut;
            AmbaMisra_TypeCast(&DrawOut, &OutData);
            Rval = SvcSurDrawTask_DrawMsg(SVC_OWS_DRAW, DrawOut);
            if (SVC_NG == Rval) {
                SvcLog_NG(SVC_LOG_OWS_TASK, "SvcSurDrawTask_DrawMsg error !", 0U, 0U);
            }
            //AmbaPrint_PrintInt5("PathNum %d ObsStage %d %d %d %d", OWSOut.PathNum, OWSOut.ObsStage[0], OWSOut.ObsStage[1], OWSOut.ObsStage[2], OWSOut.ObsStage[3]);
        }
    }
    AmbaMisra_TouchUnused(EntryArg);

    return NULL;
}

UINT32 SvcOwsTask_Init(void)
{
    UINT32 Rval = SVC_OK;
    static AMBA_OWS_CFG_DATA_s OwsInitCfg;
    static AMBA_OWS_CAL_AVM_CFG_s OWSCalibCfg;
    static SVC_TASK_CTRL_s     SvcOwsTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8               SvcOwsTaskStack[SVC_OWS_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char                QueueName[] = "SvcOwsQueue";
    static SVC_CV_DETRES_SEG_s SvcOwsQue[SVC_OWS_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 BufSize = 0U;
    ULONG BufBase = 0U;
    AMBA_CAL_SIZE_s CarSize;

    AmbaSvcWrap_MisraMemset(&OwsInitCfg, 0, sizeof(AMBA_OWS_CFG_DATA_s));
    AmbaSvcWrap_MisraMemset(&OWSCalibCfg, 0, sizeof(AMBA_OWS_CAL_AVM_CFG_s));
    AmbaSvcWrap_MisraMemset(&CarSize, 0, sizeof(AMBA_CAL_SIZE_s));

    Rval = AmbaOWS_GetDefaultConfig(OWS_SEG_WIDTH, OWS_SEG_HEIGH, &OwsInitCfg);
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_OWS_TASK, "AmbaOWS_GetDefaultConfig err %d", Rval, 0U);
    } else {
        UINT32 i;
        static SVC_CALIB_ADAS_NAND_TABLE_s Cfg;
        SVC_CALIB_ADAS_NAND_TABLE_s *pCfg = &Cfg;
        UINT32 Chan, RetVal;
        static AMBA_CAL_AVM_MV_LDC_CFG_s LdcCfg[AMBA_CAL_AVM_CAM_MAX];
        static AMBA_CAL_LENS_DST_REAL_EXPECT_s RealExpect[AMBA_CAL_AVM_CAM_MAX];
        static DOUBLE Real[AMBA_CAL_AVM_CAM_MAX][MAX_LENS_DISTO_TBL_LEN];
        static DOUBLE Expect[AMBA_CAL_AVM_CAM_MAX][MAX_LENS_DISTO_TBL_LEN];

        for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
            Chan = i + SVC_CALIB_ADAS_TYPE_AVM_F;
            AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_CALIB_ADAS_NAND_TABLE_s));
            Cfg.AdasAvmPaCfg.Cfg.pLDC = &LdcCfg[i];
            Cfg.AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect = &RealExpect[i];
            RealExpect[i].pRealTbl= &Real[i][0];
            RealExpect[i].pExpectTbl = &Expect[i][0];
            RetVal = SvcCalib_AdasCfgGet(Chan, pCfg);
            if (RetVal == SVC_OK) {
                AmbaSvcWrap_MisraMemcpy(&(OWSCalibCfg.CalibData[i]), &(Cfg.AdasAvmCalibCfg), sizeof(AMBA_CAL_AVM_CALIB_DATA_s));
                AmbaSvcWrap_MisraMemcpy(&(OWSCalibCfg.Cfg[i]), &(Cfg.AdasAvmPaCfg), sizeof(AMBA_CAL_AVM_PARK_AST_CFG_V1_s));
                AmbaSvcWrap_MisraMemcpy(&CarSize, &(Cfg.AdasAvm2DCfg.Car), sizeof(AMBA_CAL_SIZE_s));
            } else {
                break;
            }
        }
        if (i == (UINT32)AMBA_CAL_AVM_CAM_MAX) {
            OwsCalibLoad = 1U;
        }

        if (OwsCalibLoad == 1U) {
            Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_SUR_CV_OWS, &BufBase, &BufSize);
            if ((BufSize >= OwsInitCfg.WrkBufSize) && (Rval == SVC_OK)) {
                AmbaMisra_TypeCast(&OwsInitCfg.pWrkBuf, &BufBase);
                OwsInitCfg.Clock = 12288;
                OwsInitCfg.SelfCarSpec.Size.Width = CarSize.Width;
                OwsInitCfg.SelfCarSpec.Size.Length = CarSize.Height;
                OwsInitCfg.ObsIdxInfo.IdxNum = 3;//Can adjust
                OwsInitCfg.ObsIdxInfo.IdxList[0] = SEG_CLASS_BACKGROUND;
                OwsInitCfg.ObsIdxInfo.IdxList[1] = SEG_CLASS_OBSTACLE;
                OwsInitCfg.ObsIdxInfo.IdxList[2] = SEG_CLASS_POLE;
                Rval = AmbaOWS_SetInitConfig(&OwsInitCfg, &OWSCalibCfg);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_OWS_TASK, "AmbaOWS_SetInitConfig failed!!", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_OWS_TASK, "SvcBuffer_Request failed or buffer size not enough!! size %d rval = %d", OwsInitCfg.WrkBufSize, Rval);
            }
        }
    }

    if (Rval == SVC_OK) {
        /* create msg queue */
        if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcOwsQueId,
                                        QueueName,
                                        sizeof(SVC_CV_DETRES_SEG_s),
                                        SvcOwsQue,
                                        SVC_OWS_QUEUE_SIZE * sizeof(SVC_CV_DETRES_SEG_s))) {
            SvcLog_NG(SVC_LOG_OWS_TASK, "SvcOwsTask_Init: Fail to create msg queue", 0U, 0U);
        } else {
            //Do nothing
        }

        /* Create LDWS task */
        SvcOwsTaskCtrl.Priority   = 54U;
        SvcOwsTaskCtrl.EntryFunc  = SvcOwsTaskEntry;
        SvcOwsTaskCtrl.pStackBase = SvcOwsTaskStack;
        SvcOwsTaskCtrl.StackSize  = SVC_OWS_TASK_STACK_SIZE;
        SvcOwsTaskCtrl.CpuBits    = 0x1U;
        SvcOwsTaskCtrl.EntryArg   = 0x0U;

        Rval = SvcTask_Create("SvcOwsTask", &SvcOwsTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_OWS_TASK, "SvcOws_TaskCreate create fail", 0U, 0U);
        } else {
            const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
            UINT32 i;
            UINT32 CvFlowNum = pResCfg->CvFlowNum;
            UINT32 CvFlowBits = pResCfg->CvFlowBits;
            const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

            for (i = 0U; i < CvFlowNum; i++) {
                if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                    if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG_FDAG) ||
                        (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                        UINT32 CvFlowRegisterID = 0U;
                        Rval |= SvcCvFlow_Register(i, SvcOwsTask_DetectionResultCallback, &CvFlowRegisterID);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_OWS_TASK, "SvcOwsTask_Init err. SvcCvFlow_Register failed %d", Rval, 0U);
                        } else {
                            SegCropInfo.SrcW = pCvFlow[i].InputCfg.Input[0].FrameWidth;
                            SegCropInfo.SrcH = pCvFlow[i].InputCfg.Input[0].FrameHeight;
                            SegCropInfo.RoiW = 1280U;
                            SegCropInfo.RoiH = 512U;
                            SegCropInfo.RoiX = pCvFlow[i].InputCfg.Input[0].Roi[0].StartX;
                            SegCropInfo.RoiY = pCvFlow[i].InputCfg.Input[0].Roi[0].StartY;
                            SvcOwsInit = 1U;
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

UINT32 SvcOwsTask_Start(void)
{
    UINT32 RetVal;

    if (OwsCalibLoad == 1U) {
        if (SvcOwsInit == 1U) {
            //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS, SVC_WARN_ICON_FLG_ENA);
            //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            SvcOwsOn = 1U;
            RetVal = SVC_OK;
        } else {
            SvcLog_NG(SVC_LOG_OWS_TASK, "OWS module not init yet. Cannot start", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_OWS_TASK, "Abort ADAS-OWS enable due to calib data not in NAND", 0U, 0U);
        RetVal = SVC_NG;
    }
    return RetVal;
}

UINT32 SvcOwsTask_Stop(void)
{
    UINT32 RetVal;

    if (SvcOwsInit == 1U) {
        SvcOwsOn = 0U;
        //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS,   SVC_WARN_ICON_FLG_ENA);
        //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA);
        //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA);
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_OWS_TASK, "OWS module not init yet. Cannot start", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void SvcOwsTask_DetectionResultCallback(UINT32 Chan, UINT32 OutputTag, void *pDetResMsg)
{
    SVC_CV_DETRES_SEG_s SegResult;
    const SVC_CV_DETRES_SEG_s* pSegResult;
    UINT32 FovIdx = 0U, OwsChan = (UINT32)AMBA_CAL_AVM_CAM_BACK;
    UINT8 OwsEnable = 0U;

    (void)pDetResMsg;
    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&OutputTag);

    if (SvcOwsOn == 1U) {
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        FovIdx = pCvFlow[Chan].InputCfg.Input[0].StrmId;

        if ((pResCfg->FovNum > 1U) && (FovIdx == 1U)) {
            OwsEnable = 1U;
            OwsChan = FovIdx;
        } else if (pResCfg->FovNum == 1U) {
            OwsEnable = 1U;
            OwsChan = (UINT32)AMBA_CAL_AVM_CAM_BACK;
        } else {
            OwsEnable = 0U;
        }

        if (OwsEnable == 1U) {
            AmbaMisra_TypeCast(&pSegResult, &pDetResMsg);
            SegResult.Width       = pSegResult->Width;
            SegResult.Height      = pSegResult->Height;
            SegResult.Pitch       = pSegResult->Pitch;
            SegResult.FrameNum    = pSegResult->FrameNum;
            SegResult.CaptureTime = pSegResult->CaptureTime;
            SegResult.MsgCode = OwsChan;
            AmbaMisra_TypeCast(&SegResult.pBuf, &pSegResult->pBuf);

            if (SVC_OK != AmbaKAL_MsgQueueSend(&SvcOwsQueId, &SegResult, AMBA_KAL_WAIT_FOREVER)) {
                SvcLog_NG(SVC_LOG_OWS_TASK, "AmbaKAL_MsgQueueSend failed", 0U, 0U);
            } else {
                //Do nothing
            }
        }
    } else {
        //Do nothing
    }
}

