/**
 *  @file SvcPsdTask.c
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
 *  @details Svc Psd Object Detection task
 *
 */
#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaCalib_AVMIF.h"
#include "AmbaCT_AvmTunerIF.h"
#include "AmbaCalib_EmirrorDef.h"

#include "RefFlow_PSD.h"
#include "PrepareAVMCalib.h"


#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcPsdTask.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvAppDef.h"
#include "SvcCalibAdas.h"
#include "SvcSurDrawTask.h"
#include "SvcOsd.h"
#include "SvcGui.h"



#define SVC_LOG_PSD_TASK     "PSD_TASK"
#define SVC_PSD_QUEUE_SIZE        128U

#define SVC_PSD_TASK_STACK_SIZE (0xF000)

static UINT8 SvcPsdInit = 0U;
static UINT8 PsdCalibLoad = 0U; //Current always 1;
static UINT8 SvcPsdOn = 0U;
static AMBA_KAL_MSG_QUEUE_t    SvcPsdQueId GNU_SECTION_NOZEROINIT;
static AMBA_PSD_CROP_INFO_s PsdSegCropInfo;

static void SvcPsdTask_DetectionResultCallback(UINT32 Chan, UINT32 OutputTag, void *pDetResMsg);

static UINT32 GetPSDResult( RF_LOG_HEADER_s *pHeader)
{
    UINT32 DataFmt = pHeader->DataFmt;
    (void) pHeader;

    if (DataFmt == RF_PSD_DFMT_DETECT_OUT) {
        //AmbaPrint_PrintStr5("%s, DataFmt = RF_PSD_DFMT_DETECT_OUT", __func__, NULL, NULL, NULL, NULL);
    }
    return ADAS_ERR_NONE;
}

static void* SvcPsdTaskEntry(void* EntryArg)
{
    UINT8 IsTrue = 1U;
    UINT32 Rval;
    SVC_CV_DETRES_SEG_s SegResult;
    static REF_FLOW_PSD_IN_DATA_s PSDIn;
    static REF_FLOW_PSD_OUT_DATA_s PSDOut;

    while (0U != IsTrue) {
        Rval = AmbaKAL_MsgQueueReceive(&SvcPsdQueId, &SegResult, AMBA_KAL_WAIT_FOREVER);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_PSD_TASK, "SvcLdwsQueId MsgQue Receive error !", 0U, 0U);
        } else {
            //Do nothing
        }

        PSDIn.PsdInData.TimeStamp = SegResult.CaptureTime;
        PSDIn.PsdInData.SegBufInfo.pBuf = SegResult.pBuf;
        PSDIn.PsdInData.SegBufInfo.Width = SegResult.Width;
        PSDIn.PsdInData.SegBufInfo.Height = SegResult.Height;
        PSDIn.PsdInData.SegBufInfo.Pitch = SegResult.Pitch;
        AmbaSvcWrap_MisraMemcpy(&PSDIn.PsdInData.SegBufCropInfo, &PsdSegCropInfo, sizeof(AMBA_PSD_CROP_INFO_s));
        Rval = RefFlowPSD_Process(SegResult.MsgCode, &PSDIn, &PSDOut);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_PSD_TASK, "RefFlowPSD_Process error !", 0U, 0U);
        } else {
            const void *DrawOut;
            const AMBA_PSD_PROC_OUTPUT_DATA_s *OutData = &PSDOut.PsdOutData;
            AmbaMisra_TypeCast(&DrawOut, &OutData);
            Rval = SvcSurDrawTask_DrawMsg(SVC_PSD_DRAW, DrawOut);
            if (SVC_NG == Rval) {
                SvcLog_NG(SVC_LOG_PSD_TASK, "SvcSurDrawTask_DrawMsg error !", 0U, 0U);
            }
            //AmbaPrint_PrintInt5("SpaceNum %d", PSDOut.PsdOutData.SpaceNum, 0U, 0U, 0U, 0U);
        }
    }
    AmbaMisra_TouchUnused(EntryArg);

    return NULL;
}

UINT32 SvcPsdTask_Init(void)
{
    UINT32 Rval = SVC_OK;
    UINT32 WrkBufSize;
    static REF_FLOW_PSD_CFG_s refPsdCfg;
    UINT32 BufSize = 0U;
    ULONG BufBase = 0U;
    static SVC_TASK_CTRL_s     SvcPsdTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8               SvcPsdTaskStack[SVC_PSD_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char                QueueName[] = "SvcPsdQueue";
    static SVC_CV_DETRES_SEG_s SvcPsdQue[SVC_PSD_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    UINT8 *WrkBuf;
    AMBA_CAL_SIZE_s CarSize;

    AmbaSvcWrap_MisraMemset(&CarSize, 0, sizeof(AMBA_CAL_SIZE_s));


    Rval = RefFlowPSD_GetDefaultCfg(PSD_SEG_WIDTH, PSD_SEG_HEIGH, &WrkBufSize, &refPsdCfg);
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_PSD_TASK, "RefFlowPSD_GetDefaultCfg err %d", Rval, 0U);
    }

    if (Rval == SVC_OK) {
        SvcLog_DBG(SVC_LOG_PSD_TASK, "[DEBUG] Working buffer size = %d", WrkBufSize, 0U);
        if (WrkBufSize > SVC_PSD_WRK_BUF_SIZE) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_PSD_TASK, "Error!! too small working buffer size", 0U, 0U);
        }
    }

    if (Rval == SVC_OK) {
        UINT32 i;
        static SVC_CALIB_ADAS_NAND_TABLE_s Cfg;
        SVC_CALIB_ADAS_NAND_TABLE_s *pCfg = &Cfg;
        UINT32 Chan, RetVal;
        static AMBA_CAL_AVM_MV_LDC_CFG_s LdcCfg[PREP_AVM_CAL_CHANNEL_MAX];
        static AMBA_CAL_LENS_DST_REAL_EXPECT_s RealExpect[PREP_AVM_CAL_CHANNEL_MAX];
        static DOUBLE Real[PREP_AVM_CAL_CHANNEL_MAX][MAX_LENS_DISTO_TBL_LEN];
        static DOUBLE Expect[PREP_AVM_CAL_CHANNEL_MAX][MAX_LENS_DISTO_TBL_LEN];

        for (i = 0; i < RF_PSD_CHANNEL_MAX_NUM; i++) {
            Chan = i + SVC_CALIB_ADAS_TYPE_AVM_F;
            AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_CALIB_ADAS_NAND_TABLE_s));
            Cfg.AdasAvmPaCfg.Cfg.pLDC = &LdcCfg[i];
            Cfg.AdasAvmPaCfg.Cam.Lens.LensDistoSpec.pRealExpect = &RealExpect[i];
            RealExpect[i].pRealTbl= &Real[i][0];
            RealExpect[i].pExpectTbl = &Expect[i][0];
            RetVal = SvcCalib_AdasCfgGet(Chan, pCfg);
            if (RetVal == SVC_OK) {
                AmbaSvcWrap_MisraMemcpy(&(refPsdCfg.CalibData[i]), &(Cfg.AdasAvmCalibCfg), sizeof(AMBA_CAL_AVM_CALIB_DATA_s));
                AmbaSvcWrap_MisraMemcpy(&(refPsdCfg.PACfgV1[i]), &(Cfg.AdasAvmPaCfg), sizeof(AMBA_CAL_AVM_PARK_AST_CFG_V1_s));
                AmbaSvcWrap_MisraMemcpy(&CarSize, &(Cfg.AdasAvm2DCfg.Car), sizeof(AMBA_CAL_SIZE_s));
            } else {
                break;
            }
        }
        if (i == RF_PSD_CHANNEL_MAX_NUM) {
            PsdCalibLoad = 1U;
        }

        if (PsdCalibLoad == 1U) {
            refPsdCfg.PsdCfg.Clock = 12288;
            refPsdCfg.PsdCfg.CarSize[0U] = CarSize.Width;
            refPsdCfg.PsdCfg.CarSize[1U] = CarSize.Height;

            refPsdCfg.PsdCfg.SegClassInfo.BackGroundIdx = 0U;
            refPsdCfg.PsdCfg.SegClassInfo.RoadIdx = 1U;
            refPsdCfg.PsdCfg.SegClassInfo.CornerIdx = 5U;
            refPsdCfg.PsdCfg.SegClassInfo.ParkingLineIdx = 2U;
            refPsdCfg.PsdCfg.SegClassInfo.ObstacleIdxNum = 2U;
            refPsdCfg.PsdCfg.SegClassInfo.ObstacleIdxList[0] = 3U;
            refPsdCfg.PsdCfg.SegClassInfo.ObstacleIdxList[1] = 4U;

            refPsdCfg.LogFp = GetPSDResult;


            Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_SUR_CV_PSD, &BufBase, &BufSize);
            if (Rval == SVC_OK) {
                AmbaMisra_TypeCast(&WrkBuf, &BufBase);
                Rval = RefFlowPSD_Init(&refPsdCfg, WrkBuf);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PSD_TASK, "RefFlowPSD_Init failed!!", 0U, 0U);
                }
            }
        }
    }

    if (Rval == SVC_OK) {
        /* create msg queue */
        if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcPsdQueId,
                                        QueueName,
                                        sizeof(SVC_CV_DETRES_SEG_s),
                                        SvcPsdQue,
                                        SVC_PSD_QUEUE_SIZE * sizeof(SVC_CV_DETRES_SEG_s))) {
            SvcLog_NG(SVC_LOG_PSD_TASK, "SvcPsdTask_Init: Fail to create msg queue", 0U, 0U);
        } else {
            //Do nothing
        }

        /* Create LDWS task */
        SvcPsdTaskCtrl.Priority   = 53U;
        SvcPsdTaskCtrl.EntryFunc  = SvcPsdTaskEntry;
        SvcPsdTaskCtrl.pStackBase = SvcPsdTaskStack;
        SvcPsdTaskCtrl.StackSize  = SVC_PSD_TASK_STACK_SIZE;
        SvcPsdTaskCtrl.CpuBits    = 0x1U;
        SvcPsdTaskCtrl.EntryArg   = 0x0U;

        Rval = SvcTask_Create("SvcPsdTask", &SvcPsdTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_PSD_TASK, "SvcPsd_TaskCreate create fail", 0U, 0U);
        }

        if (SVC_OK == Rval) {
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
                        Rval |= SvcCvFlow_Register(i, SvcPsdTask_DetectionResultCallback, &CvFlowRegisterID);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_PSD_TASK, "SvcPsdTask_Init err. SvcCvFlow_Register failed %d", Rval, 0U);
                        } else {
                            PsdSegCropInfo.SrcW = pCvFlow[i].InputCfg.Input[0].FrameWidth;
                            PsdSegCropInfo.SrcH = pCvFlow[i].InputCfg.Input[0].FrameHeight;
                            PsdSegCropInfo.RoiW = 1280U;
                            PsdSegCropInfo.RoiH = 512U;
                            PsdSegCropInfo.RoiX = pCvFlow[i].InputCfg.Input[0].Roi[0].StartX;
                            PsdSegCropInfo.RoiY = pCvFlow[i].InputCfg.Input[0].Roi[0].StartY;
                            SvcPsdInit = 1U;
                        }
                    }
                }
            }
        }

    }
    return Rval;
}

UINT32 SvcPsdTask_Start(void)
{
    UINT32 RetVal;

    if (PsdCalibLoad == 1U) {
        if (SvcPsdInit == 1U) {
            //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS, SVC_WARN_ICON_FLG_ENA);
            //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            SvcPsdOn = 1U;
            RetVal = SVC_OK;
        } else {
            SvcLog_NG(SVC_LOG_PSD_TASK, "PSD module not init yet. Cannot start", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_PSD_TASK, "Abort ADAS-PSD enable due to calib data not in NAND", 0U, 0U);
        RetVal = SVC_NG;
    }
    return RetVal;
}

UINT32 SvcPsdTask_Stop(void)
{
    UINT32 RetVal;

    if (SvcPsdInit == 1U) {
        SvcPsdOn = 0U;
        //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS,   SVC_WARN_ICON_FLG_ENA);
        //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA);
        //(void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA);
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_PSD_TASK, "PSD module not init yet. Cannot start", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void SvcPsdTask_DetectionResultCallback(UINT32 Chan, UINT32 OutputTag, void *pDetResMsg)
{
    SVC_CV_DETRES_SEG_s SegResult;
    const SVC_CV_DETRES_SEG_s* pSegResult;
    UINT32 FovIdx = 0U, PsdChan = RF_PSD_CHANNEL_RIGHT;
    UINT8 PsdEnable = 0U;

    (void) pDetResMsg;
    AmbaMisra_TouchUnused(&OutputTag);
    AmbaMisra_TouchUnused(&Chan);

    if (SvcPsdOn == 1U) {
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        FovIdx = pCvFlow[Chan].InputCfg.Input[0].StrmId;

        if ((pResCfg->FovNum > 1U) && ((FovIdx == 2U) || (FovIdx == 3U))) {
            PsdEnable = 1U;
            PsdChan = FovIdx;
        } else if (pResCfg->FovNum == 1U) {
            PsdEnable = 0U;
            PsdChan = RF_PSD_CHANNEL_RIGHT;
        } else {
            PsdEnable = 0U;
        }

        if (PsdEnable == 1U) {
            AmbaMisra_TypeCast(&pSegResult, &pDetResMsg);
            SegResult.Width       = pSegResult->Width;
            SegResult.Height      = pSegResult->Height;
            SegResult.Pitch       = pSegResult->Pitch;
            SegResult.FrameNum    = pSegResult->FrameNum;
            SegResult.CaptureTime = pSegResult->CaptureTime;
            SegResult.MsgCode = PsdChan;
            AmbaMisra_TypeCast(&SegResult.pBuf, &pSegResult->pBuf);

            if (SVC_OK != AmbaKAL_MsgQueueSend(&SvcPsdQueId, &SegResult, AMBA_KAL_WAIT_FOREVER)) {
                SvcLog_NG(SVC_LOG_PSD_TASK, "AmbaKAL_MsgQueueSend failed", 0U, 0U);
            } else {
                //Do nothing
            }
        }
    } else {
        //Do nothing
    }
}

