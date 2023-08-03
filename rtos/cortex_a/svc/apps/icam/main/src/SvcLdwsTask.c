/**
 *  @file SvcLdwsTask.c
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
 *  @details Svc Ldws Object Detection task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"

/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaCalib_AVMIF.h"
#include "AmbaWS_LDWS.h"
#include "AmbaMisraFix.h"
#include "AmbaSLD_LaneDetect.h"
#include "AmbaRTSL_ORC.h"
#include "AmbaFS.h"
#include "AmbaGDMA_Def.h"
#include "AmbaGDMA.h"
#include "AmbaSYS_Def.h"
#include "AmbaSYS.h"
#include "AmbaDef.h"
#include "AmbaAP_LKA.h"
#include "AmbaSvcWrap.h"

#if defined(CONFIG_BUILD_COMMON_TUNE_CTUNER)
#include "AmbaCT_EmirTunerIF.h"
#endif

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCvCamCtrl.h"
#include "SvcTask.h"

/* svc-shared */
#include "SvcGui.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcOsd.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcVinSrc.h"
#include "AmbaShell.h"
#include "SvcDataGather.h"
#include "SvcCvFlow_RefSeg.h"

/* svc-icam */
#include "SvcCvAppDef.h"
#include "SvcLdwsTask.h"
#include "SvcCalibAdas.h"
#include "SvcCan.h"
#include "SvcWarningIconTask.h"
#include "AmbaWS_FCWS.h"
#include "SvcAdasNotify.h"
#include "RefFlow_AutoCal.h"
#include "SvcAdasAutoCal.h"
#include "RefFlow_LD.h"
#include "SvcUserPref.h"
#include "SvcSegDrawTask.h"

#define SVC_LOG_LDWS_TASK     "LDWS_TASK"
#define SVC_LDWS_QUEUE_SIZE        128U

#define SVC_LDWS_TASK_STACK_SIZE (0x10000)

/* Definition of control Ldws feature */
static UINT32 LdwsInit = 0U;

/* Definition for OSD */
static UINT32 LdwsOnOff = 0U;  /* Enable to draw detection on OSD by default */

static UINT32 LdwsCalibFromNAND = 0U;

static UINT32 g_EnableLKA = 0U;

static UINT32 g_AutoCalEnable = 0U;

static AMBA_KAL_MSG_QUEUE_t    SvcLdwsQueId GNU_SECTION_NOZEROINIT;
static AMBA_CAL_EM_CALC_COORD_CFG_s *CalibData = NULL;
static SVC_LDWS_OUT_s g_LDOut;
static AMBA_SEG_CROP_INFO_s g_SegBufCropInfo;

static UINT32 SvcLdwsTask_IsDecMode(void) {
    UINT32                 RetVal = SVC_OK, i, Src, Err;
    SVC_USER_PREF_s        *pSvcUserPref;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

    AmbaMisra_TouchUnused(&RetVal);

    /* if it's in decode mode */
	Src = 0U;
    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        if (pSvcUserPref->OperationMode == 1U) {
            for (i = 0U; i < pCfg->FovNum; i++) {
                Err = SvcResCfg_GetFovSrc(i, &Src);
                if ((Err == SVC_OK) && (Src != SVC_VIN_SRC_MEM_DEC)) {
                    RetVal = SVC_NG; //Means liveview mode
                }
            }
        }
    }

    return RetVal;
}


static UINT32 GetLDWSResult(RF_LOG_HEADER_s *pHeader) {
    UINT32 DataFmt = pHeader->DataFmt;

    if (DataFmt == RF_LD_DFMT_SR_LANE_OUT) {
        AmbaSvcWrap_MisraMemcpy(&g_LDOut.LaneInfo, pHeader->pDataAddr, sizeof(AMBA_SR_LANE_RECONSTRUCT_INFO_s));
        g_LDOut.LaneInfoIsAssign = 1U;
    }

    if (g_EnableLKA == 1U) {
        if (DataFmt == RF_LD_DFMT_LKA_OUT) {
            AmbaSvcWrap_MisraMemcpy(&g_LDOut.LKAInfo, pHeader->pDataAddr, sizeof(AMBA_AP_LKA_RESULT_s));
            g_LDOut.LKAInfoIsAssign = 1U;
        }
    } else {
        g_LDOut.LKAInfoIsAssign = 0U;
    }

    AmbaMisra_TouchUnused(pHeader);
    return ADAS_ERR_NONE;
}


static void SvcLdwsTask_DetectionResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg)
{
    SVC_CV_DETRES_SEG_s SegResult;
    const SVC_CV_DETRES_SEG_s* pSegResult;

    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);

    if (LdwsOnOff == 1U) {
        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pSegResult, &pDetResMsg);
        SegResult.Width       = pSegResult->Width;
        SegResult.Height      = pSegResult->Height;
        SegResult.Pitch       = pSegResult->Pitch;
        SegResult.FrameNum    = pSegResult->FrameNum;
        SegResult.CaptureTime = pSegResult->CaptureTime;
        AmbaMisra_TypeCast(&SegResult.pBuf, &pSegResult->pBuf);

        if (SVC_OK != AmbaKAL_MsgQueueSend(&SvcLdwsQueId, &SegResult, AMBA_KAL_WAIT_FOREVER)) {
            SvcLog_NG(SVC_LOG_LDWS_TASK, "AmbaKAL_MsgQueueSend failed", 0U, 0U);
        } else {
            //Do nothing
        }
    } else {
        //Do nothing
    }
}

static void* SvcLdwsTaskEntry(void* EntryArg)
{
    UINT32 Rval;//CarSpeed,
    SVC_CV_DETRES_SEG_s SegResult;
    REF_FLOW_LD_IN_DATA_s LaneInData;
    AMBA_LDWS_DEPARTURE_INFO_s Dep;
    static AMBA_SR_CANBUS_RAW_DATA_s CanbusRawDataLdws = { 0 };// for adas
    static AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTransData; // canbus transfer data
    REF_FLOW_LD_OUT_DATA_s LaneOutData;
    SvcADAS_NOTIFY_ADAS_PRIORITY_e LdEvent = ADAS_PRIORITY_NONE;

    AmbaMisra_TouchUnused(EntryArg);

    while (AmbaKAL_MsgQueueReceive(&SvcLdwsQueId, &SegResult, AMBA_KAL_WAIT_FOREVER) == SVC_OK) {
        AmbaSvcWrap_MisraMemset(&LaneInData, 0, sizeof(LaneInData));
        AmbaSvcWrap_MisraMemset(&Dep, 0, sizeof(Dep));

        SvcCan_GetRawData(&CanbusRawDataLdws);
        if((SVC_OK == SvcLdwsTask_IsDecMode()) && (CanbusRawDataLdws.FlagValidRawData == 0U)) {
            //Decode mode
            (void)SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)SegResult.FrameNum, &CanbusTransData);
            LaneInData.SrLaneInData.TimeStamp = CanbusTransData.CapTS;
//                AmbaPrint_PrintUInt5("[LD][Decode mode] FrameNum = %d, TransferSpeed = %d, CapTS = %d",
//                        SegResult.FrameNum, CanbusTransData.TransferSpeed, CanbusTransData.CapTS, 0U, 0U);
        } else if ((SVC_NG == SvcLdwsTask_IsDecMode()) && (CanbusRawDataLdws.FlagValidRawData == 1U)){
            //Liveview with CAN bus
            (void)SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)SegResult.CaptureTime, &CanbusTransData);
            LaneInData.SrLaneInData.TimeStamp = SegResult.CaptureTime;
//                AmbaPrint_PrintUInt5("[LD][Liveview with CAN bus] FrameNum = %d, TransferSpeed = %d, CaptureTime = %d",
//                        SegResult.FrameNum, CanbusTransData.TransferSpeed, SegResult.CaptureTime, 0U, 0U);
        } else {
            //Liveview without CAN bus
//                AmbaPrint_PrintUInt5("[LD]No can bus", 0U, 0U, 0U, 0U, 0U);
            CanbusTransData.TransferSpeed = 0.0;
            LaneInData.SrLaneInData.TimeStamp = SegResult.CaptureTime;
        }

        LaneInData.SrLaneInData.Width       = SegResult.Width;
        LaneInData.SrLaneInData.Height      = SegResult.Height;
        LaneInData.SrLaneInData.Pitch       = SegResult.Pitch;
        AmbaMisra_TypeCast32(&(LaneInData.SrLaneInData.pBuf), &SegResult.pBuf);


        LaneInData.SrLaneInData.CropInfo.SrcW = g_SegBufCropInfo.SrcW;//TBD
        LaneInData.SrLaneInData.CropInfo.SrcH = g_SegBufCropInfo.SrcH;//TBD
        LaneInData.SrLaneInData.CropInfo.RoiX = g_SegBufCropInfo.RoiX;
        LaneInData.SrLaneInData.CropInfo.RoiY = g_SegBufCropInfo.RoiY;
        LaneInData.SrLaneInData.CropInfo.RoiW = g_SegBufCropInfo.RoiW;
        LaneInData.SrLaneInData.CropInfo.RoiH = g_SegBufCropInfo.RoiH;

        AmbaSvcWrap_MisraMemcpy(&(LaneInData.CanbusTrData), &CanbusTransData, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s));

        AmbaSvcWrap_MisraMemset(&LaneOutData, 0, sizeof(LaneOutData));
        Rval = RefFlowLD_Process(&LaneInData, &LaneOutData);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_LDWS_TASK, "RefFlowLD_Process error !", 0U, 0U);
        } else {
            //Do nothing
        }

//        AmbaPrint_PrintUInt5("[V1]Dep.DepartureMsg = %d, DistL = %d  DistR = %d",
//                            LaneOutData.LdwsDepRes.DepartureMsg,
//                            LaneOutData.LdwsDepRes.DistanceLR[AMBA_SR_LANE_LINE_LEFT0],
//                            LaneOutData.LdwsDepRes.DistanceLR[AMBA_SR_LANE_LINE_RIGHT0], 0U, 0U);
        /* Update the distance of LDWS between front wheel to lane boundary (in mm) */
        SvcAdasNotify_LdwsDistRLUpdate(LaneOutData.LdwsDepRes.DistanceLR[AMBA_SR_LANE_LINE_LEFT0], LaneOutData.LdwsDepRes.DistanceLR[AMBA_SR_LANE_LINE_RIGHT0]);
        switch(LaneOutData.LdwsDepRes.DepartureMsg) {
        case AMBA_LDWS_DEPARTURE_NONE:
            /* Reset FcEvent event to none */
            LdEvent = ADAS_PRIORITY_NONE;
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS,   SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA);
            break;
        case AMBA_LDWS_DEPARTURE_LEFT:
            /* Trigger LDWS event and check */
            LdEvent = ADAS_PRIORITY_LDWS;
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS,   SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA);
            break;
        case AMBA_LDWS_DEPARTURE_RIGHT:
            /* Trigger LDWS event and check */
            LdEvent = ADAS_PRIORITY_LDWS;
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS,   SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
            break;
        default:
            SvcLog_NG(SVC_LOG_LDWS_TASK, "LaneOutData.LdwsDepRes.DepartureMsg unknown type!", 0U, 0U);
            break;
        }

        /* Check big ADAS IconShow status */
        LdEvent = SvcAdasNotify_ADAS_IconShow(LdEvent);
        if (LdEvent == ADAS_PRIORITY_LDWS) {
            if(AMBA_LDWS_DEPARTURE_LEFT == LaneOutData.LdwsDepRes.DepartureMsg) {
                (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_2x,   SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW);
                (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L_2x, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
                (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R_2x, SVC_WARN_ICON_FLG_ENA);
            } else if(AMBA_LDWS_DEPARTURE_RIGHT == LaneOutData.LdwsDepRes.DepartureMsg) {
                (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_2x,   SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW);
                (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L_2x, SVC_WARN_ICON_FLG_ENA);
                (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R_2x, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
            } else {
                (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_2x,   SVC_WARN_ICON_FLG_ENA);
                (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L_2x, SVC_WARN_ICON_FLG_ENA);
                (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R_2x, SVC_WARN_ICON_FLG_ENA);
            }
        } else {
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_2x,   SVC_WARN_ICON_FLG_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L_2x, SVC_WARN_ICON_FLG_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R_2x, SVC_WARN_ICON_FLG_ENA);
        }

        //Auto Calibration LaneDetectionData and CanbusData Update
        if (1U == SvcLdwsTask_GetAutoCalStatus()) {
            static SVC_ADAS_AUTOCAL_LD_INFO_s AutoCalInfo;

            if (SVC_OK != AmbaWrap_memcpy(&AutoCalInfo.LaneDetectionData, &(g_LDOut.LaneInfo),
                                          sizeof(AMBA_SR_LANE_RECONSTRUCT_INFO_s))){
                SvcLog_NG(SVC_LOG_LDWS_TASK, "LaneDetectionData memcpy failed", 0U, 0U);
            }

            if (SVC_OK != AmbaWrap_memcpy(&AutoCalInfo.CanbusData, &CanbusTransData,
                                          sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s))){
                SvcLog_NG(SVC_LOG_LDWS_TASK, "CanbusData memcpy failed", 0U, 0U);
            }

            if (SVC_OK != SvcAdasAutoCal_MsgSend(&AutoCalInfo)){
                SvcLog_NG(SVC_LOG_LDWS_TASK, "SvcAdasAutoCal_MsgSend failed", 0U, 0U);
            }
        }

        /* Draw LD detect result */
        {
            AmbaSvcWrap_MisraMemcpy(&g_LDOut.CanbusTrData, &CanbusTransData, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s));
            Rval = SvcSegDrawTask_LdMsg(SVC_LW_DRAW, &g_LDOut);
            if (SVC_NG == Rval) {
                SvcLog_NG(SVC_LOG_LDWS_TASK, "SvcSegDrawTask_LdMsg() error !", 0U, 0U);
            }
        }


    }

    return NULL;
}

static UINT32 SvcLdwsTask_GetCalibrationData(void)
{
    UINT32 Rval;
    static UINT32 LdwsCalibInitFlag = 0U;
    static AMBA_CAL_EM_CALC_COORD_CFG_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;

    if (LdwsCalibInitFlag == 0U) {
        Cfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
        Rval = SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_FRONT, &Cfg);
        if(Rval == SVC_OK) {
            SvcLog_OK("SvcFc_GetCalibrationData", "[LD] Get ADAS CAM CALIB from NAND !", 0U, 0U);
            CalibData = &Cfg;
            LdwsCalibInitFlag = 1U;
        }
    }

    return LdwsCalibInitFlag;
}

/**
 *  Init LDWS module
 *  @return error code
 */
#define SQRT2                         ((FLOAT) 1.414213562)
#define SQRT2xSQRT2                   ((FLOAT) 2)
#define SQRT2xSQRT2xSQRT2             ((FLOAT) 2.828427124)
#define SQRT2xSQRT2xSQRT2xSQRT2       ((FLOAT) 4)
#define SQRT2xSQRT2xSQRT2xSQRT2xSQRT2 ((FLOAT) 5.656854248)
UINT32 SvcLdwsTask_Init(void)
{
    UINT32 RetVal;
    static SVC_TASK_CTRL_s     SvcLdwsTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8               SvcLdwsTaskStack[SVC_LDWS_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char                QueueName[] = "SvcLdwsQueue";
    static SVC_CV_DETRES_SEG_s SvcLdwsQue[SVC_LDWS_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    SVC_USER_PREF_s   *pSvcUserPref;
    REF_FLOW_LD_CFG_s RfLaneCfg;
    AMBA_COMPACT_CAR_SPEC_s CarSpec;
    static UINT8 *WrkBufAddr;
    UINT32 Size = 0U;
    UINT32 Freq;
    ULONG  CacheBase;
    UINT32 CacheSize = 0U;

	Freq = 0;
    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED,
                               SMEM_PF0_ID_ADAS_LDWS,
                               &CacheBase,
                               &CacheSize);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_LDWS_TASK, "[FC] SvcBuffer_Request() ADAS_LDWS Error!! RetVal = %d", RetVal, 0U);
    }

    LdwsCalibFromNAND = SvcLdwsTask_GetCalibrationData();

    /* Get Default config */
    RetVal = RefFlowLD_GetDefaultCfg(SVC_LDWS_SEG_WIDTH, SVC_LDWS_SEG_HEIGHT, &Size, &RfLaneCfg);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_LDWS_TASK, "RefFlowLD_GetDefaultCfg fail", 0U, 0U);
    }

    AmbaPrint_PrintUInt5("[LD] working buffer size = %d", Size, 0U, 0U, 0U, 0U);
    if (Size > CacheSize) {
        SvcLog_NG(SVC_LOG_LDWS_TASK, "[LD] Working buffer size Error!! %d", __LINE__, 0U);
    }

    /* Init to 0 */
    AmbaSvcWrap_MisraMemset(&g_LDOut, 0, sizeof(SVC_LDWS_OUT_s));

    AmbaSvcWrap_MisraMemset(&g_SegBufCropInfo, 0, sizeof(g_SegBufCropInfo));
    {/*  Align to calibration data size */
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        UINT32 i;
        UINT32 CvFlowNum = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                    SVC_CV_ROI_INFO_s   RoiInfo;
                    FLOAT StartX, StartY, Width, Height;
                    AmbaSvcWrap_MisraMemset(&RoiInfo, 0, sizeof(RoiInfo));
                    RetVal = SvcCvFlow_Control(i, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_LDWS_TASK, "SvcCvFlow_Control err %d", RetVal, 0U);
                    } else {
                        StartX = (FLOAT)(RoiInfo.Roi[0].StartX);
                        StartY = (FLOAT)(RoiInfo.Roi[0].StartY);
                        Width  = (FLOAT)(RoiInfo.Roi[0].Width);
                        Height = (FLOAT)(RoiInfo.Roi[0].Height);
                        switch(RoiInfo.Roi[0].Index) {
                        case 0:
                            //No action
                            break;
                        case 1:
                            StartX *= SQRT2;
                            StartY *= SQRT2;
                            Width  *= SQRT2;
                            Height *= SQRT2;
                            break;
                        case 2:
                            StartX *= SQRT2xSQRT2;
                            StartY *= SQRT2xSQRT2;
                            Width  *= SQRT2xSQRT2;
                            Height *= SQRT2xSQRT2;
                            break;
                        case 3:
                            StartX *= SQRT2xSQRT2xSQRT2;
                            StartY *= SQRT2xSQRT2xSQRT2;
                            Width  *= SQRT2xSQRT2xSQRT2;
                            Height *= SQRT2xSQRT2xSQRT2;
                            break;
                        case 4:
                            StartX *= SQRT2xSQRT2xSQRT2xSQRT2;
                            StartY *= SQRT2xSQRT2xSQRT2xSQRT2;
                            Width  *= SQRT2xSQRT2xSQRT2xSQRT2;
                            Height *= SQRT2xSQRT2xSQRT2xSQRT2;
                            break;
                        case 5:
                            StartX *= SQRT2xSQRT2xSQRT2xSQRT2xSQRT2;
                            StartY *= SQRT2xSQRT2xSQRT2xSQRT2xSQRT2;
                            Width  *= SQRT2xSQRT2xSQRT2xSQRT2xSQRT2;
                            Height *= SQRT2xSQRT2xSQRT2xSQRT2xSQRT2;
                            break;
                        default:
                            SvcLog_NG(SVC_LOG_LDWS_TASK, "Something wrong, PyramidScale = %d", RoiInfo.Roi[0].Index, 0U);
                            break;

                        }
                        g_SegBufCropInfo.RoiX = (UINT32)StartX;
                        g_SegBufCropInfo.RoiY = (UINT32)StartY;
                        g_SegBufCropInfo.RoiW = (UINT32)Width;
                        g_SegBufCropInfo.RoiH = (UINT32)Height;

                        //Auto Calibration SegRoi Setup
                        {
                            AMBA_CAL_ROI_s SegRoi;

                            SegRoi.Width = g_SegBufCropInfo.RoiW;
                            SegRoi.Height = g_SegBufCropInfo.RoiH;
                            SegRoi.StartX = g_SegBufCropInfo.RoiX;
                            SegRoi.StartY = g_SegBufCropInfo.RoiY;
                            SvcAdasAutoCal_SetupSegRoi(&SegRoi);
                        }

                        SvcLog_OK(SVC_LOG_LDWS_TASK, "[LD][PyramidScale = %d] Align to calibration data size:", RoiInfo.Roi[0].Index, 0U);
                        SvcLog_OK(SVC_LOG_LDWS_TASK, "[LD]StartX = %d, StartX = %d", g_SegBufCropInfo.RoiX, g_SegBufCropInfo.RoiY);
                        SvcLog_OK(SVC_LOG_LDWS_TASK, "[LD]Width = %d, Height = %d", g_SegBufCropInfo.RoiW, g_SegBufCropInfo.RoiH);
                    }
                    break;
                }
            }
        }
    }

    (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_DSP_SYS, &Freq);
    Freq = Freq/1000U;

    /* SLD initial config */
    AmbaSvcWrap_MisraMemset(&RfLaneCfg, 0, sizeof(RfLaneCfg));
    RfLaneCfg.SrLaneCfg.DashLength = 4000;
    RfLaneCfg.SrLaneCfg.DashSpace = 6000;
    RfLaneCfg.SrLaneCfg.SegBufImgW = SVC_LDWS_SEG_WIDTH;
    RfLaneCfg.SrLaneCfg.SegBufImgH = SVC_LDWS_SEG_HEIGHT;
    AmbaMisra_TypeCast32(&(RfLaneCfg.SrLaneCfg.Clock), &Freq);
    RfLaneCfg.SrLaneCfg.LaneMarkIdxInfo.IdxNum = 4;
    RfLaneCfg.SrLaneCfg.LaneMarkIdxInfo.ClassList[0] = SEG_CLASS_CURB;
    RfLaneCfg.SrLaneCfg.LaneMarkIdxInfo.TypeList[0] = AMBA_SEG_TYPE_CURB;
    RfLaneCfg.SrLaneCfg.LaneMarkIdxInfo.ClassList[1] = SEG_CLASS_LANEMARKING_Y;
    RfLaneCfg.SrLaneCfg.LaneMarkIdxInfo.TypeList[1] = AMBA_SEG_TYPE_LANE;
    RfLaneCfg.SrLaneCfg.LaneMarkIdxInfo.ClassList[2] = SEG_CLASS_LANEMARKING_W;
    RfLaneCfg.SrLaneCfg.LaneMarkIdxInfo.TypeList[2] = AMBA_SEG_TYPE_LANE;
    RfLaneCfg.SrLaneCfg.LaneMarkIdxInfo.ClassList[3] = SEG_CLASS_LANEMARKING_R;
    RfLaneCfg.SrLaneCfg.LaneMarkIdxInfo.TypeList[3] = AMBA_SEG_TYPE_LANE;

    /* LDWS Inital config */
    CarSpec.Size.Length = 4486U;
    CarSpec.Size.Width = 1839U;
    CarSpec.FrontOverhang = 902U;
    CarSpec.FrontTrack = 1589U;

    RfLaneCfg.LdwsCfg.Clock = Freq;
    RfLaneCfg.LdwsCfg.pCarBodyInfo = &CarSpec;
    RfLaneCfg.LdwsCfg.WarnSensLv = 30U;

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        RfLaneCfg.LdwsCfg.ActiveSpeed = pSvcUserPref->LdwsActiveSpeed;
    } else {
        RfLaneCfg.LdwsCfg.ActiveSpeed = SVC_LD_WARNING_LOWER_BOUND;
    }
    SvcLog_OK(SVC_LOG_LDWS_TASK, "LDWS ActiveSpeed = %d", RfLaneCfg.LdwsCfg.ActiveSpeed, 0U);

    /* Set calibration config */
    if (LdwsCalibFromNAND == 1U) {
        AmbaSvcWrap_MisraMemcpy(&(RfLaneCfg.CalibCfg), CalibData, sizeof(AMBA_CAL_EM_CALC_COORD_CFG_s));
    }

    RfLaneCfg.LogFp = GetLDWSResult;
    AmbaMisra_TypeCast(&WrkBufAddr, &(CacheBase));
    RetVal = RefFlowLD_Init(&RfLaneCfg, WrkBufAddr);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_LDWS_TASK, "RefFlowLD_Init fail", 0U, 0U);
    }

    AmbaSvcWrap_MisraMemcpy(&g_LDOut.LKAConfigInfo, &RfLaneCfg.LkaCfg, sizeof(AMBA_AP_LKA_CONFIG_s));

    /* create msg queue */
    if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcLdwsQueId,
                                    QueueName,
                                    sizeof(SVC_CV_DETRES_SEG_s),
                                    SvcLdwsQue,
                                    SVC_LDWS_QUEUE_SIZE * sizeof(SVC_CV_DETRES_SEG_s))) {
        SvcLog_NG(SVC_LOG_LDWS_TASK, "SvcLdwsTask_Init: Fail to create msg queue", 0U, 0U);
    } else {
        //Do nothing
    }

    /* Create LDWS task */
    SvcLdwsTaskCtrl.Priority   = 53U;
    SvcLdwsTaskCtrl.EntryFunc  = SvcLdwsTaskEntry;
    SvcLdwsTaskCtrl.pStackBase = SvcLdwsTaskStack;
    SvcLdwsTaskCtrl.StackSize  = SVC_LDWS_TASK_STACK_SIZE;
    SvcLdwsTaskCtrl.CpuBits    = 0x1U;
    SvcLdwsTaskCtrl.EntryArg   = 0x0U;

    RetVal = SvcTask_Create("SvcLdwsTask", &SvcLdwsTaskCtrl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_LDWS_TASK, "SvcLdws_TaskCreate create fail", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
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
                    RetVal |= SvcCvFlow_Register(i, SvcLdwsTask_DetectionResultCallback, &CvFlowRegisterID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_LDWS_TASK, "SvcLdwsTask_Init err. SvcCvFlow_Register failed %d", RetVal, 0U);
                    }
                }
            }
        }
    }

    if (SVC_OK == RetVal) {
        LdwsInit = 1U;
    } else {
        //Do nothing
    }

    return RetVal;
}

/**
 *  Start LWDS module
 *  @return error code
 */
UINT32 SvcLdwsTask_Start(void)
{
    UINT32 RetVal;

    if (LdwsCalibFromNAND == 1U) {
        if (LdwsInit == 1U) {
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS, SVC_WARN_ICON_FLG_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_2x, SVC_WARN_ICON_FLG_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L_2x, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R_2x, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            LdwsOnOff = 1U;
            RetVal = SVC_OK;
        } else {
            SvcLog_NG(SVC_LOG_LDWS_TASK, "Ldws module not init yet. Cannot start", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_LDWS_TASK, "Abort ADAS-LDWS enable due to calib data not in NAND", 0U, 0U);
        RetVal = SVC_NG;
    }


    return RetVal;
}

/**
 *  Stop LWDS module
 *  @return error code
 */
UINT32 SvcLdwsTask_Stop(void)
{
    UINT32 RetVal;

    if (LdwsInit == 1U) {
        LdwsOnOff = 0U;
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS,   SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L, SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R, SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_2x,   SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_L_2x, SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_LDWS_R_2x, SVC_WARN_ICON_FLG_ENA);
        SvcAdasNotify_LdwsDistRLUpdate(0.0, 0.0);
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_LDWS_TASK, "Ldws module not init yet. Cannot stop", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Get current status of LDWS module
 *  @param[in] pEnable current LDWS module statue (On or Off)
 *  @return error code
 */
UINT32 SvcLdwsTask_GetStatus(UINT32 *pEnable)
{
    UINT32 RetVal;

    if (LdwsInit == 1U) {
        *pEnable = LdwsOnOff;
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_LDWS_TASK, "Ldws module not init yet. Cannot get status", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Enable / Disable LKA
 *  @param[in] Enable (1U), Disable (0U)
 */
void SvcLdwsTask_EnableLKA(UINT32 Enable)
{
    g_EnableLKA =  Enable;
}

/**
 *  Enable LKA debug info
 *  @param[in] Enable (1U), Disable (0U)
 */
void SvcLdwsTask_LKADebugEnable(UINT32 Enable)
{
    g_LDOut.LKADebugFlag = Enable;
}

/**
 *  Enable Auto Calibration flow
 *  @param[in] Enable/Disable
 */
void SvcLdwsTask_EnableAutoCal(UINT32 Enable)
{
    g_AutoCalEnable = Enable;
}

/**
 *  Get Auto Calibration status (Enable or disable)
 */
UINT32 SvcLdwsTask_GetAutoCalStatus(void)
{
    return g_AutoCalEnable;
}
