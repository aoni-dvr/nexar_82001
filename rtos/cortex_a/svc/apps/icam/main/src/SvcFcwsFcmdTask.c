/**
 *  @file SvcFcwsFcmdTask.c
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
 *  @details Svc FCWS and FCMD Object Detection task
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
#include "AmbaWS_FCWS.h"
#include "AmbaWS_FCMD.h"
#include "AmbaSLD_LaneDetect.h"
#include "AmbaRTSL_ORC.h"
#include "AmbaFS.h"
#include "AmbaGDMA_Def.h"
#include "AmbaGDMA.h"
#include "AmbaShell.h"
#include "AmbaDef.h"
#include "AmbaSvcWrap.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"

/* svc-shared */
#include "SvcGui.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcOsd.h"
#include "SvcCvFlow.h"
#include "SvcCalibAdas.h"
#include "SvcDataGather.h"
#include "SvcVinSrc.h"
#include "SvcCvFlow_Comm.h"

/* svc-icam */
#include "SvcCvAppDef.h"
//#include "SvcFcwsFcmd.h"
#include "RefFlow_FC.h"
#include "SvcFcwsFcmdTask.h"
#include "SvcCan.h"
#include "SvcWarningIconTask.h"
#include "SvcCvCamCtrl.h"
#include "SvcAdasNotify.h"
#include "SvcUserPref.h"
#include "RefFlow_FC.h"
#include "SvcODDrawTask.h"
#include "SvcSegDrawTask.h"


#define SVC_LOG_FCWS_FCMD_TASK     "FC_TASK"
#define SVC_FCWS_FCMD_QUEUE_SIZE   128U

#define SVC_APP_CV_FOV_NUM_MAX   (4U)
#define HorizontalLine_Thickness      (3U)
#define SVC_FCWS_FCMD_TASK_STACK_SIZE (0x10000)

static UINT32 FcInit = 0U;
static UINT32 FcCalibFromNAND = 0U;
static UINT32 FcOnOff = 0U;  /* Disable to draw detection on OSD by default */
static UINT32 WarningIconOnOff = 0U;  /* Disable WarningIcon display by default */
static AMBA_KAL_MSG_QUEUE_t  SvcFcQueId GNU_SECTION_NOZEROINIT;
static AMBA_CAL_EM_CALC_COORD_CFG_s *CalibData = NULL;
static AMBA_OD_2DBBX_LIST_s    g_BbxOut;
static AMBA_SR_SROBJECT_DATA_s g_SROut;
static REF_FLOW_FC_OUT_DATA_s  g_FCOut;

static UINT32 SvcFcTask_IsDecMode(void) {
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


static UINT32 GetResult(RF_LOG_HEADER_s *pHeader) {
    UINT32 HdrVer = pHeader->HdrVer;
    UINT32 DataFmt = pHeader->DataFmt;
    UINT32 Count = pHeader->Count;
    UINT32 Channel = pHeader->Channel;
    UINT32 DataSize = pHeader->DataSize;

    if (DataFmt == RF_FC_DFMT_2DBBX_OUT) {
        static AMBA_OD_2DBBX_LIST_s *pAddr;// = (AMBA_OD_2DBBX_LIST_s *)pDataAddr;
        AmbaMisra_TypeCast(&pAddr, &(pHeader->pDataAddr));
        AmbaSvcWrap_MisraMemcpy(&g_BbxOut, pAddr, DataSize);
    } else if (DataFmt == RF_FC_DFMT_SR) {
        static AMBA_SR_SROBJECT_DATA_s *pAddr;// = (AMBA_SR_SROBJECT_DATA_s *)pDataAddr;
        AmbaMisra_TypeCast(&pAddr, &(pHeader->pDataAddr));
        AmbaSvcWrap_MisraMemcpy(&g_SROut, pAddr, DataSize);
    } else if (DataFmt == RF_FC_DFMT_FC_OUT) {
        static REF_FLOW_FC_OUT_DATA_s *pAddr;// = (REF_FLOW_FC_OUT_DATA_s *)pDataAddr;
        AmbaMisra_TypeCast(&pAddr, &(pHeader->pDataAddr));
        AmbaSvcWrap_MisraMemcpy(&g_FCOut, pAddr, DataSize);
    } else {
        //Do nothing
    }

    AmbaMisra_TouchUnused(&HdrVer);
    AmbaMisra_TouchUnused(&Count);
    AmbaMisra_TouchUnused(&Channel);
    AmbaMisra_TouchUnused(pHeader);

    return ADAS_ERR_NONE;

}

static void SvcFcTask_DetectionResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg)
{
    SVC_CV_DETRES_BBX_LIST_s *pList;
    SVC_FCWS_FCMD_BBX_LIST_s FcBbxData;

    ULONG  Msg;
    UINT32 ValidDetResMsg = 0U;

    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);

    AmbaMisra_TypeCast(&Msg, pDetResMsg);
    if (Msg == SVC_CV_DETRES_BBX) {
        ValidDetResMsg = 1U;
    } else {
        ValidDetResMsg = 0U;
    }

    if ((FcOnOff == 1U) && (ValidDetResMsg == 1U)) {
        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pList, &pDetResMsg);
        FcBbxData.Chan        = Chan;
        FcBbxData.CvType      = CvType;
        FcBbxData.BbxAmount = pList->BbxAmount;
        FcBbxData.CaptureTime = pList->CaptureTime;
        FcBbxData.FrameNum = pList->FrameNum;
        FcBbxData.MsgCode = pList->MsgCode;
        FcBbxData.Source = pList->Source;
        FcBbxData.Bbx =  pList->Bbx;
        FcBbxData.class_name = pList->class_name;
        if (SVC_OK != AmbaKAL_MsgQueueSend(&SvcFcQueId, &FcBbxData, AMBA_KAL_WAIT_FOREVER)) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "AmbaKAL_MsgQueueSend failed", 0U, 0U);
        } else {
            //Do nothing
        }
    } else {
        //Do nothing
    }
}

static void SvcFc_BbxTransSvcToWs(AMBA_OD_2DBBX_LIST_s *pBbxdataWS, const SVC_FCWS_FCMD_BBX_LIST_s *pList) {
    pBbxdataWS->MsgCode = pList->MsgCode;
    pBbxdataWS->CaptureTime = pList->CaptureTime;
    pBbxdataWS->FrameNum = pList->FrameNum;
    pBbxdataWS->NumBbx = pList->BbxAmount;
    AmbaSvcWrap_MisraMemcpy(pBbxdataWS->Bbx, pList->Bbx, SVC_CV_DETRES_MAX_BBX_NUM * sizeof(SVC_CV_DETRES_BBX_WIN_s));
}

static UINT32 FcwsWarningCheck(UINT32 FcwsEvent)
{
    static UINT32 CurrentEvent = AMBA_WS_FCWS_EVENT_NO_WARNING;
    static UINT32 FcwsShowCnt = ADAS_WARNING_SHOW_CNT;

    //AmbaWS_FCWS definition is that Red warning value(COL_WARNING = 3U) > Yellow warning value(PRE_WARNING = 2U)
    if (FcwsEvent >= CurrentEvent) {
        FcwsShowCnt = ADAS_WARNING_SHOW_CNT; //Reset ShowCnt if event priority is lower/equal
        CurrentEvent = FcwsEvent;
    }

    if (FcwsShowCnt == 0U) {
        CurrentEvent = AMBA_WS_FCWS_EVENT_NO_WARNING;
    } else {
        FcwsShowCnt--;
    }

    return CurrentEvent;
}

static UINT32 FcmdWarningContinuousCheck(UINT32 IsInit, UINT32 FcmdEvent)
{
#define FCMD_WARNING_CONTINUOUS_CNT 2U
    UINT32 FcmdResult = AMBA_WS_FCMD_EVENT_NO_WARNING;
    static UINT8 IsWarning = 0U;
    static UINT8 FmcdCnt = 0U;
    static UINT8 WarningCountDown = 0U;

    if (1U == IsInit) {
        FcmdResult = AMBA_WS_FCMD_EVENT_NO_WARNING;
        IsWarning = 0U;
        FmcdCnt = 0U;
        WarningCountDown = 0U;
    } else {
        //Do nothing
    }

    if (1U == IsWarning) {
        WarningCountDown--;
        if (0U == WarningCountDown) {
            IsWarning = 0U;
        }
        FcmdResult = AMBA_WS_FCMD_EVENT_WARNING;
    } else { //No IsWarning (IsWarning == 0U)
        if (AMBA_WS_FCMD_EVENT_WARNING == FcmdEvent) {
            FmcdCnt++;
            if (FCMD_WARNING_CONTINUOUS_CNT == FmcdCnt) {
                IsWarning = 1U;
                WarningCountDown = ADAS_WARNING_SHOW_CNT;
            } else {
                //Do nothing
            }
        } else {
            FmcdCnt = 0U;
        }
    }

    return FcmdResult;
}

static void* SvcFcTaskEntry(void* EntryArg)
{
    DOUBLE SafeDistance, TTC;
    SvcADAS_NOTIFY_ADAS_PRIORITY_e FcEvent = ADAS_PRIORITY_NONE;
    SVC_FCWS_FCMD_BBX_LIST_s FcBbxData;
    static AMBA_SR_CANBUS_RAW_DATA_s CanbusRawDataFc = { 0 };// for adas
    static AMBA_OD_2DBBX_LIST_s BbxdataWS GNU_SECTION_NOZEROINIT;
    static AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTransData; // canbus transfer data
    UINT32 FmcdWarning, FcwsWarning;
    static REF_FLOW_FC_IN_DATA_s  FcInData GNU_SECTION_NOZEROINIT;
    static REF_FLOW_FC_OUT_DATA_s FcOutData GNU_SECTION_NOZEROINIT;

    AmbaSvcWrap_MisraMemset(&BbxdataWS, 0, sizeof(BbxdataWS));

    AmbaMisra_TouchUnused(EntryArg);

    while (AmbaKAL_MsgQueueReceive(&SvcFcQueId, &FcBbxData, AMBA_KAL_WAIT_FOREVER) == SVC_OK) {
        if (FcBbxData.Source < SVC_APP_CV_FOV_NUM_MAX) {
            SvcCan_GetRawData(&CanbusRawDataFc);
            if((SVC_OK == SvcFcTask_IsDecMode()) && (CanbusRawDataFc.FlagValidRawData == 0U)) {
                //Decode mode
                (void)SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)FcBbxData.FrameNum, &CanbusTransData);
                FcBbxData.CaptureTime = CanbusTransData.CapTS;
                //Showing current speed under decode mode
                SvcAdasNotify_DecModeDrawSpeed(CanbusTransData.TransferSpeed);
//                AmbaPrint_PrintUInt5("[FC][Decode mode] FrameNum = %d, TransferSpeed = %d, CaptureTime = %d",
//                        FcBbxData.FrameNum, CanbusTransData.TransferSpeed, FcBbxData.CaptureTime, 0U, 0U);
            } else if ((SVC_NG == SvcFcTask_IsDecMode()) && (CanbusRawDataFc.FlagValidRawData == 1U)){
                //Liveview with CAN bus
                (void)SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)FcBbxData.CaptureTime, &CanbusTransData);
//                AmbaPrint_PrintUInt5("[FC][Liveview with CAN bus] FrameNum = %d, TransferSpeed = %d, CaptureTime = %d",
//                FcBbxData.FrameNum, CanbusTransData.TransferSpeed, FcBbxData.CaptureTime, 0U, 0U);
            } else {
                //Liveview without CAN bus
                static UINT32 FakePTS = 0U;
//                AmbaPrint_PrintUInt5("[FC]No can bus", 0U, 0U, 0U, 0U, 0U);
                /* Fake CAN bus data. E.g., FCMD:2; FCWS: 90 */
                FakePTS++;
                CanbusTransData.CapTS = FakePTS;
                FcInData.CanbusTransData.FlagValidTransferData = 0U;
                FcInData.CanbusTransData.TransferSpeed = 2.0;
//                FcInData.CanbusTransData.TransferSpeed = 90;

            }

            SvcFc_BbxTransSvcToWs(&BbxdataWS, &FcBbxData);
            AmbaSvcWrap_MisraMemcpy(&FcInData.CanbusTransData, &CanbusTransData, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s));
            AmbaSvcWrap_MisraMemcpy(&FcInData.BbxList, &BbxdataWS, sizeof(AMBA_OD_2DBBX_LIST_s));

            /* Fake CAN bus data. E.g., FCMD:2; FCWS: 90 */
//            FcInData.CanbusTransData.FlagValidTransferData = 1U;
//            FcInData.CanbusTransData.TransferSpeed = 2.0;
//            FcInData.CanbusTransData.TransferSpeed = 90.0;
            if (RefFlowFC_Process(&FcInData, &FcOutData) == OK) {
                /* Debug */
//                if (SvcFcwsOutput.Event == 2 || SvcFcwsOutput.Event == 3) {
//                    SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "FCWS Event = %d, SafeDistance = %u", SvcFcwsOutput.Event, SvcFcwsOutput.ObjectStatus.SafeDistance);
//                    SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "SafeDistance  = %d, Coordinate[1] = %u",
//                                                       SvcFcwsOutput.ObjectStatus.SafeDistance, SvcFcwsOutput.ObjectStatus.Coordinate[1]);
//                }
//                if (2 == SvcFcmdOutput.Event) {
//                    SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "FCMD Event = %d", SvcFcmdOutput.Event, 0U);
//                }
//                SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "FCWS: %d   FCMD: %d", FcOutData.WsFcwsStat.Event, FcOutData.WsFcmdStat.Event);

                /* FCWS Result id WarningIconOnOff flag enable */
                if(1U == WarningIconOnOff) {
                    FcwsWarning = FcwsWarningCheck(FcOutData.WsFcwsStat.Event);


                    switch(FcwsWarning) {
                    case AMBA_WS_FCWS_EVENT_PRE_WARNING://Yellow warning
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2, SVC_WARN_ICON_FLG_ENA);

                        /* Trigger FCWS_YELLOW event and check */
                        FcEvent = ADAS_PRIORITY_FCWS_YELLOW;
                        break;
                    case AMBA_WS_FCWS_EVENT_COL_WARNING://Red warning
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1, SVC_WARN_ICON_FLG_ENA);

                        /* Trigger FCWS_YELLOW event and check */
                        FcEvent = ADAS_PRIORITY_FCWS_RED;
                        break;
                    default:

                        /* Reset FcEvent event to none */
                        FcEvent = ADAS_PRIORITY_NONE;
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1, SVC_WARN_ICON_FLG_ENA);
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2, SVC_WARN_ICON_FLG_ENA);
                        break;
                    }

                    /* Check big ADAS IconShow status */
                    FcEvent = SvcAdasNotify_ADAS_IconShow(FcEvent);
                    if (FcEvent == ADAS_PRIORITY_FCWS_RED) {
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2_2x, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1_2x, SVC_WARN_ICON_FLG_ENA);
                    } else if (FcEvent == ADAS_PRIORITY_FCWS_YELLOW) {
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1_2x, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2_2x, SVC_WARN_ICON_FLG_ENA);
                    } else {
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1_2x, SVC_WARN_ICON_FLG_ENA);
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2_2x, SVC_WARN_ICON_FLG_ENA);
                    }


                    TTC          = 0.0;  //Reset TTC
                    SafeDistance = 0.0;  //Reset SafetyDist
                    if (1U == FcOutData.WsFcwsStat.ObjectStatus.StatusUpdate) {
                        TTC = (FcOutData.WsFcwsStat.ObjectStatus.TTC); //UINT: msec
                        SafeDistance = (FcOutData.WsFcwsStat.ObjectStatus.SafeDistance / 1000.0); //UINT: mm to m
                    }

                    //Update SafetyDist
                    SvcAdasNotify_FcwsSafeDistUpdate(SafeDistance);

                    //Update TTC
                    SvcAdasNotify_FcwsTtcUpdate(TTC);

                    FmcdWarning = FcmdWarningContinuousCheck(0U, FcOutData.WsFcmdStat.Event);

                    /* FCMD Result*/
                    switch(FmcdWarning) {
                    case AMBA_WS_FCMD_EVENT_WARNING:
                        /* Trigger FCMD event and check */
                        FcEvent = ADAS_PRIORITY_FCMD;
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
                        break;
                    default:
                        /* Reset FcEvent event to none */
                        FcEvent = ADAS_PRIORITY_NONE;
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD, SVC_WARN_ICON_FLG_ENA);
                        break;
                    }

                    /* Check big ADAS IconShow status */
                    FcEvent = SvcAdasNotify_ADAS_IconShow(FcEvent);
                    if (FcEvent == ADAS_PRIORITY_FCMD) {
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD_2x, SVC_WARN_ICON_FLG_ENA | SVC_WARN_ICON_FLG_SHOW | SVC_WANR_ICON_FLG_ANI);
                    } else {
                        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD_2x, SVC_WARN_ICON_FLG_ENA);
                    }
                }

                /* Draw FC Bbx results */
                {
                    static SVC_FC_DRAW_INFO_s FCinfo;

                    FCinfo.Chan               = FcBbxData.Chan;
                    FCinfo.CvType             = FcBbxData.CvType;
                    FCinfo.BbxOut.MsgCode     = FcBbxData.MsgCode;
                    FCinfo.BbxOut.Source      = FcBbxData.Source;
                    FCinfo.BbxOut.class_name  = FcBbxData.class_name;
                    FCinfo.BbxOut.CaptureTime = g_BbxOut.CaptureTime;
                    FCinfo.BbxOut.FrameNum    = g_BbxOut.FrameNum;
                    FCinfo.BbxOut.BbxAmount   = g_BbxOut.NumBbx;
                    if (g_BbxOut.NumBbx > 0U) {
                        AmbaSvcWrap_MisraMemcpy(FCinfo.BbxOut.Bbx, g_BbxOut.Bbx, (sizeof(AMBA_OD_2DBBX_s)*g_BbxOut.NumBbx));
                    } else {
                        AmbaSvcWrap_MisraMemset(FCinfo.BbxOut.Bbx, 0, (sizeof(AMBA_OD_2DBBX_s)*SVC_CV_DETRES_MAX_BBX_NUM));
                    }
                    AmbaSvcWrap_MisraMemcpy(&(FCinfo.SROut), &g_SROut, sizeof(AMBA_SR_SROBJECT_DATA_s));
                    AmbaSvcWrap_MisraMemcpy(&(FCinfo.FCOut), &g_FCOut, sizeof(REF_FLOW_FC_OUT_DATA_s));

                    /* No need to high light target vehicle if WarningIconOnOff disable */
                    if(0U == WarningIconOnOff) {
                        FCinfo.FCOut.WsFcwsStat.ObjectStatus.StatusUpdate = 0U;
                    }

                    if (SvcODDrawTask_Msg(SVC_FC_DRAW, &FCinfo) == SVC_NG) {
                        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcODDrawTask_Msg() error !", 0U, 0U);
                    }
                }
            }

        }
    }

    return NULL;
}

static UINT32 SvcFc_GetCalibrationData(void)
{
    UINT32 Rval;
    static UINT32 FcwsFcmdCalibInitFlag = 0U;
    static AMBA_CAL_EM_CALC_COORD_CFG_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;

    if (FcwsFcmdCalibInitFlag == 0U) {
        Cfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
        Rval = SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_FRONT, &Cfg);
        if(Rval == SVC_OK) {
            SvcLog_OK("SvcFc_GetCalibrationData", "[FC] Get ADAS CAM CALIB from NAND ! Ver: ", SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT), 0U);
            CalibData = &Cfg;
            if (SVC_CALIB_ADAS_VERSION == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                FcwsFcmdCalibInitFlag = 1U;
            }
        }
    }

    return FcwsFcmdCalibInitFlag;
}

static UINT32 GetHorizontalLine(const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalCfg, UINT32 pntX[2], UINT32 pntY[2]) {
    UINT32 retVal = 0U;
    UINT32 srcW = pCalCfg->VoutArea.Width;
    UINT32 srcH = pCalCfg->VoutArea.Height;
    UINT32 ofsX = pCalCfg->VoutArea.StartX;
    UINT32 ofsY = pCalCfg->VoutArea.StartY;
    UINT32 x, y;

    x = ofsX;
	y = 0U;
    (void)AmbaCal_EmFindHorizonPosition(pCalCfg, x, ofsY + (srcH/2U), (srcH/2U), &y);
    pntX[0U] = x;
    pntY[0U] = y;

    if (y > (ofsY + srcH - 1U)) {
        retVal = 1U;
    }

    if (retVal == 0U) {
        x = ofsX+srcW -1U;
        (void)AmbaCal_EmFindHorizonPosition(pCalCfg, x, ofsY + (srcH/2U), (srcH/2U), &y);
        pntX[1U] = x;
        pntY[1U] = y;

        if (y > (ofsY + srcH - 1U)) {
            retVal = 1U;
        }
    }

    return retVal;
}

#define UTIL_VSL_FCWS_LANE_WIDTH                (3750.0)
#define UTIL_VSL_FCWS_WARNING_ZONE_TOUCH_FLAG   (0U)
static UINT32 FCWS_GetWarnZonePnts_TtcBase( const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalCfg,
    const AMBA_WS_FCWS_DATA_s *pFcwsCfg,
    const AMBA_CAL_ROI_s *pDrawRoi,
    INT32 Zone1X[4], INT32 Zone1Y[4])
{
    /**
    * Get FCWS ttc bitmap_datasize warning zone1 and zone2 points
    * @param [in] pCalCfg    Calibration config for corresponding channel
    * @param [in] pFcwsCfg   FCWS config
    * @param [in] pDrawRoi   Change coordinate to roi which is crop from Vout, NULL for roi is the same as Vout
    * @param [out] Zone1X    4 Zone1 x points (0 for left close point, 1 for left far point, 2 for right far point, 3 for right close point)
    * @param [out] Zone1Y    4 Zone1 y points (0 for left close point, 1 for left far point, 2 for right far point, 3 for right close point)
    * @return 0 for success, 1 for failed
    */

    UINT32 rVal = 0U;
    AMBA_CAL_POINT_DB_3D_s pnt3D[4];
    AMBA_CAL_POINT_DB_2D_s pnt2D[4];

    INT32 srcW = (INT32)pCalCfg->VoutArea.Width;  /**< not working for VoutArea.StartX and VoutArea.StartY */
    INT32 srcH = (INT32)pCalCfg->VoutArea.Height;
    DOUBLE srcWDbl = (DOUBLE)srcW;
    DOUBLE srcHDbl = (DOUBLE)srcH;

    INT32 roiW;
    INT32 roiH;
    INT32 roiX;
    INT32 roiY;

    DOUBLE FarDist = 100000.0; /**< mm */

    if (pDrawRoi == NULL) {
        roiW = srcW;
        roiH = srcH;
        roiX = 0;
        roiY = 0;
    } else {
        roiW = (INT32)pDrawRoi->Width;
        roiH = (INT32)pDrawRoi->Height;
        roiX = (INT32)pDrawRoi->StartX;
        roiY = (INT32)pDrawRoi->StartY;
    }

    if (((roiX + roiW) > srcW) || ((roiY + roiH) > srcH)) {
        AmbaPrint_PrintStr5("%s, Error!! Forbidden pDrawRoi setting.", __func__, NULL, NULL, NULL, NULL);
        rVal = 1U;
    }

    AmbaPrint_PrintUInt5("vout=[%d,%d,%d,%d]",
            pCalCfg->VoutArea.StartX, pCalCfg->VoutArea.StartY, pCalCfg->VoutArea.Width, pCalCfg->VoutArea.Height, 0U);

    /** For Zone 1 */
    if (rVal == 0U) {
#if UTIL_VSL_FCWS_WARNING_ZONE_TOUCH_FLAG == 0U
        pnt3D[0].X = -UTIL_VSL_FCWS_LANE_WIDTH / 3.0;     /**< left, close */
        pnt3D[0].Y = (pFcwsCfg->CarSize[1] / 2.0) + 5000.0;
        pnt3D[0].Z = 0.0;
        pnt3D[1].X = -UTIL_VSL_FCWS_LANE_WIDTH / 3.0;     /**< left, far */
        pnt3D[1].Y = FarDist;
        pnt3D[1].Z = 0.0;
        pnt3D[2].X = UTIL_VSL_FCWS_LANE_WIDTH / 3.0;      /**< right, far */
        pnt3D[2].Y = FarDist;
        pnt3D[2].Z = 0.0;
        pnt3D[3].X = UTIL_VSL_FCWS_LANE_WIDTH / 3.0;      /**< right, close */
        pnt3D[3].Y = (pFcwsCfg->CarSize[1] / 2.0) + 5000.0;
        pnt3D[3].Z = 0.0;
#else
        pnt3D[0].X = -UTIL_VSL_FCWS_WARNING_WIDTH_OFFSET;     /**< left, close */
        pnt3D[0].Y = (pFcwsCfg->CarSize[1] / 2.0) + 5000.0;
        pnt3D[0].Z = 0.0;
        pnt3D[1].X = -UTIL_VSL_FCWS_WARNING_WIDTH_OFFSET;     /**< left, far */
        pnt3D[1].Y = FarDist;
        pnt3D[1].Z = 0.0;
        pnt3D[2].X = UTIL_VSL_FCWS_WARNING_WIDTH_OFFSET;      /**< right, far */
        pnt3D[2].Y = FarDist;
        pnt3D[2].Z = 0.0;
        pnt3D[3].X = UTIL_VSL_FCWS_WARNING_WIDTH_OFFSET;      /**< right, close */
        pnt3D[3].Y = (pFcwsCfg->CarSize[1] / 2.0) + 5000.0;
        pnt3D[3].Z = 0.0;
#endif

        for (UINT32 i = 0; i < 4U; i++) {
            if (rVal == 0U) {
                rVal = AmbaCal_EmConvPtWorldToImgPlane(pCalCfg, &pnt3D[i], &pnt2D[i]);
                if (rVal == CAL_OK) {
                    rVal = 0U;
                    AmbaPrint_PrintInt5("Point [%d]: 3D[x,y] -> [%d,%d], 2D[x,y] -> [%d,%d]",
                            (INT32)i, (INT32)pnt3D[i].X, (INT32)pnt3D[i].Y, (INT32)pnt2D[i].X, (INT32)pnt2D[i].Y);
                    if ((pnt2D[i].X < 0.0) || (pnt2D[i].Y < 0.0) || (pnt2D[i].X > (srcWDbl-1.0)) || (pnt2D[i].Y > (srcHDbl-1.0))) {
                        rVal = 1U;
                        AmbaPrint_PrintStr5("%s, get zone1 pnt failed", __func__, NULL, NULL, NULL, NULL);
                    }

                    if (rVal == 0U) {
                        Zone1X[i] = (INT32)pnt2D[i].X - roiX;
                        Zone1Y[i] = (INT32)pnt2D[i].Y - roiY;
                        if ((Zone1X[i] < 0) || (Zone1Y[i] < 0) || (Zone1X[i] > (roiW - 1)) || (Zone1Y[i] > (roiH - 1))) {
                            AmbaPrint_PrintInt5("WSGetWarnZonePnts, Warning!! (Zone1X[%d] = %d) or (Zone1Y[%d] = %d) is out of drawing roi.", (INT32)i, Zone1X[i], (INT32)i, Zone1Y[i], 0);
                        }
                    }

                } else {
                    AmbaPrint_PrintStr5("%s, AmbaCal_EmConvPtWorldToImgPlane failed", __func__, NULL, NULL, NULL, NULL);
                    rVal = 1U;
                }
            }
        }
    }

    return rVal;
}

/**
 *  Init FC module
 *  @return error code
 */
#define SQRT2                         ((FLOAT) 1.414213562)
#define SQRT2xSQRT2                   ((FLOAT) 2)
#define SQRT2xSQRT2xSQRT2             ((FLOAT) 2.828427124)
#define SQRT2xSQRT2xSQRT2xSQRT2       ((FLOAT) 4)
#define SQRT2xSQRT2xSQRT2xSQRT2xSQRT2 ((FLOAT) 5.656854248)
UINT32 SvcFcTask_Init(void)
{
    UINT32 RetVal = ADAS_ERR_NA;
    static char                     SvcFcQueueName[] = "SvcFcQueue";
    static UINT8                    SvcFcTaskStack[SVC_FCWS_FCMD_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_TASK_CTRL_s          SvcFcTaskCtrl                                 GNU_SECTION_NOZEROINIT;
    static SVC_FCWS_FCMD_BBX_LIST_s SvcFcQue[SVC_FCWS_FCMD_QUEUE_SIZE]            GNU_SECTION_NOZEROINIT;
    UINT32 Size = 0U;
    static REF_FLOW_FC_CFG_s RfFcCfg;
    static UINT8 *WrkBufAddr;
    static SVC_USER_PREF_s        *pSvcUserPref;
    static UINT32                 FcwsMode = AMBA_WS_FCWS_MODE_POSITION;
    static UINT32                 PntX[2], PntY[2];
    SVC_CV_DISP_OSD_CMD_s         HorizontalInfo;
    ULONG   CacheBase;
    UINT32  CacheSize = 0U;

    /* Reset HorizontalLine info */
    AmbaSvcWrap_MisraMemset(&HorizontalInfo, 0, sizeof(SVC_CV_DISP_OSD_CMD_s));
    HorizontalInfo.Channel = VOUT_IDX_B;
    RetVal = SvcAdasNotify_HorizontalLineUpdate(&HorizontalInfo);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC] SvcAdasNotify_HorizontalLineUpdate() Error!! %d", __LINE__, 0U);
    }

    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED,
                               SMEM_PF0_ID_ADAS_FC,
                               &CacheBase,
                               &CacheSize);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC] SvcBuffer_Request() ADAS_FC Error!! RetVal = %d", RetVal, 0U);
    }

    FcCalibFromNAND = SvcFc_GetCalibrationData();
    if (FcCalibFromNAND == 1U){
        RetVal = RefFlowFC_GetDefaultCfg(&Size, &RfFcCfg);
        if (RetVal != ADAS_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, RefFlowFC_GetDefaultCfg failed", __func__, NULL, NULL, NULL, NULL);
        }

        AmbaPrint_PrintUInt5("[FC] working buffer size = %d", Size, 0U, 0U, 0U, 0U);
        if (Size > CacheSize) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC] Working buffer size Error!! %d", __LINE__, 0U);
        }


        /* Fit both AmbaOD 3.5 and AmbaOD 3.6 class */
        {
            static AMBA_SR_NN_CAT_TO_OBJ_TYPE_s Lut[12];

            {/*  Align to calibration data size */
                const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
                UINT32 i;
                UINT32 CvFlowNum   = pResCfg->CvFlowNum;
                UINT32 CvFlowBits  = pResCfg->CvFlowBits;
                const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

                for (i = 0U; i < CvFlowNum; i++) {
                    if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                        if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                            (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                            (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                            (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                            SVC_CV_ROI_INFO_s   RoiInfo;
                            FLOAT StartX, StartY, Width, Height;
                            AmbaSvcWrap_MisraMemset(&RoiInfo, 0, sizeof(RoiInfo));
                            RetVal = SvcCvFlow_Control(i, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo);
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
                                SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "Something wrong, PyramidScale = %d", RoiInfo.Roi[0].Index, 0U);
                                break;

                            }
                            RfFcCfg.ODRoi.StartX = GetAlignedValU32((UINT32)StartX, 8U);
                            RfFcCfg.ODRoi.StartY = GetAlignedValU32((UINT32)StartY, 8U);
                            RfFcCfg.ODRoi.Width  = GetAlignedValU32((UINT32)Width, 8U);
                            RfFcCfg.ODRoi.Height = GetAlignedValU32((UINT32)Height, 8U);
                            if (SVC_OK != RetVal) {
                                SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcCvFlow_Control err %d", RetVal, 0U);
                            }
                            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "[FC][PyramidScale = %d] Align to calibration data size:", RoiInfo.Roi[0].Index, 0U);
                            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "[FC]StartX = %d, StartX = %d", RfFcCfg.ODRoi.StartX, RfFcCfg.ODRoi.StartY);
                            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "[FC]Width = %d, Height = %d", RfFcCfg.ODRoi.Width, RfFcCfg.ODRoi.Height);
                            break;
                        }
                    }
                }
            }

            AmbaSvcWrap_MisraMemset(RfFcCfg.ObjSize, 0, sizeof(AMBA_SR_OBJECT_SIZE_s)*RF_FC_MAX_NN_CAT_NUM);
            RfFcCfg.ClassNum = 13U;
            // Car
            RfFcCfg.ObjSize[1].Height = 1500U;
            RfFcCfg.ObjSize[1].Length = 4600U;
            RfFcCfg.ObjSize[1].Width = 1800U;
            // Truck
            RfFcCfg.ObjSize[2].Height = 4200U;
            RfFcCfg.ObjSize[2].Length = 10700U;
            RfFcCfg.ObjSize[2].Width = 2700U;
            // Bus
            RfFcCfg.ObjSize[3].Height = 2900U;
            RfFcCfg.ObjSize[3].Length = 8500U;
            RfFcCfg.ObjSize[3].Width = 2300U;
            // Suv
            RfFcCfg.ObjSize[4].Height = 1800U;
            RfFcCfg.ObjSize[4].Length = 5000U;
            RfFcCfg.ObjSize[4].Width = 2000U;
            // Train
            RfFcCfg.ObjSize[5].Height = 4230U;
            RfFcCfg.ObjSize[5].Length = 20300U;
            RfFcCfg.ObjSize[5].Width = 2890U;
            // Person
            RfFcCfg.ObjSize[6].Height = 1750U;
            RfFcCfg.ObjSize[6].Length = 500U;
            RfFcCfg.ObjSize[6].Width = 600U;
            // Cyclist
            RfFcCfg.ObjSize[7].Height = 1600U;
            RfFcCfg.ObjSize[7].Length = 3000U;
            RfFcCfg.ObjSize[7].Width = 700U;
            // Green/ yellow/ red/ sign/ others
            for(UINT32 i = 8U; i < RfFcCfg.ClassNum; i++){
                RfFcCfg.ObjSize[i].Height = 1000U;
                RfFcCfg.ObjSize[i].Length = 1000U;
                RfFcCfg.ObjSize[i].Width = 1000U;
            }

            Lut[0].NNCat = 1U;    Lut[0].ObjType = SR_OBJ_TYPE_VEHICLE_0;
            Lut[1].NNCat = 2U;    Lut[1].ObjType = SR_OBJ_TYPE_VEHICLE_0;
            Lut[2].NNCat = 3U;    Lut[2].ObjType = SR_OBJ_TYPE_VEHICLE_0;
            Lut[3].NNCat = 4U;    Lut[3].ObjType = SR_OBJ_TYPE_VEHICLE_0;
            Lut[4].NNCat = 6U;    Lut[4].ObjType = SR_OBJ_TYPE_PERSON_0;
            Lut[5].NNCat = 7U;    Lut[5].ObjType = SR_OBJ_TYPE_SCOOTER_0;

            RfFcCfg.NNCat2ObjTypeLut.pLut = Lut;
            RfFcCfg.NNCat2ObjTypeLut.LutLength = 6U;
        }
    }

    /** ----------------------------------------------------------------- */
    /** Cfg for SR                                                        */
    /** ----------------------------------------------------------------- */
    /* Init FC if system have CAM position calibration data */
    if ((RetVal == ADAS_ERR_NONE) && (FcCalibFromNAND == 1U)){
        AmbaSvcWrap_MisraMemcpy(&(RfFcCfg.CalibCfg), CalibData, sizeof(AMBA_CAL_EM_CALC_COORD_CFG_s));
        RfFcCfg.LogFp = GetResult;

        if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {//TTC mode or position mode
            FcwsMode = pSvcUserPref->FcwsMode;
        } else {
            FcwsMode = AMBA_WS_FCWS_MODE_POSITION;
        }
        if (FcwsMode == AMBA_WS_FCWS_MODE_TTC) {
            RfFcCfg.WsFcwsCfg.FcwsMode = AMBA_WS_FCWS_MODE_TTC;
            RfFcCfg.WsFcwsCfg.Clock = 12288U;
            RfFcCfg.WsFcwsCfg.FCWSSpec.RelativeSpeedStride = (DOUBLE)SVC_FC_RELATIVE_SPEED_STRIDE;
            for (UINT32 i = 0U; i < TTC_LEVEL; i++) {
                RfFcCfg.WsFcwsCfg.FCWSSpec.TTC_th[i] = (DOUBLE)SVC_FC_TTC_M;
            }
            for (UINT32 i = 0U; i < AMBA_WS_FCWS_MAX_SPEED; i++) {
                RfFcCfg.WsFcwsCfg.FCWSSpec.Position_th[i] = (DOUBLE)i*0.1;
            }
            RfFcCfg.WsFcwsCfg.FCWSSpec.ActiveSpeed = (DOUBLE)SVC_FC_WARNING_LOWER_BOUND;
            RfFcCfg.WsFcwsCfg.FCWSSpec.PrewarnToColwarnRatio = 1.125;

            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "FCWS_MODE: TTC mode", 0U, 0U);
        } else if (FcwsMode == AMBA_WS_FCWS_MODE_POSITION) {
            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "FCWS_MODE: POSITION mode", 0U, 0U);
        } else {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "Something wrong of WS_FCWS_MODE, mode = %d", FcwsMode, 0U);
        }
        AmbaMisra_TypeCast(&WrkBufAddr, &(CacheBase));
        RetVal = RefFlowFC_Init(&RfFcCfg, WrkBufAddr);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "RefFlowFC_Init NG %d", __LINE__, 0U);
        } else {
            //Do nothing
        }

        /* Draw Horizontal Line */
        {
            static UINT32 OsdBufWidth, OsdBufHeight;

            RetVal = SvcOsd_GetOsdBufSize(1U, &OsdBufWidth, &OsdBufHeight);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcTask_Init() SvcOsd_GetOsdBufSize return %u", RetVal, 0U);
            }

            RetVal = GetHorizontalLine(CalibData, PntX, PntY);
            if (RetVal == 0U) {
                for (UINT32 i = 0U; i < 2U; i++) {
                    PntX[i] = (PntX[i] - CalibData->VoutArea.StartX) * OsdBufWidth / CalibData->VoutArea.Width ;
                    PntY[i] = (PntY[i] - CalibData->VoutArea.StartY) * OsdBufHeight / CalibData->VoutArea.Height;
                }

                HorizontalInfo.Channel = VOUT_IDX_B;

                HorizontalInfo.X1 = ((OsdBufWidth  - PntX[0]) > HorizontalLine_Thickness)?(PntX[0] + HorizontalLine_Thickness):(PntX[0] - HorizontalLine_Thickness);
                HorizontalInfo.X2 = ((OsdBufWidth  - PntX[1]) > HorizontalLine_Thickness)?(PntX[1]):(PntX[1] - HorizontalLine_Thickness);
                HorizontalInfo.Y1 = ((OsdBufHeight - PntY[0]) > HorizontalLine_Thickness)?(PntY[0]):(PntY[0] - HorizontalLine_Thickness);
                HorizontalInfo.Y2 = ((OsdBufHeight - PntY[1]) > HorizontalLine_Thickness)?(PntY[1]):(PntY[1] - HorizontalLine_Thickness);
                HorizontalInfo.Color = 255U;
                HorizontalInfo.Thickness = HorizontalLine_Thickness;
                HorizontalInfo.WarningLevel = 0U;
                RetVal = SvcAdasNotify_HorizontalLineUpdate(&HorizontalInfo);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC] SvcAdasNotify_HorizontalLineUpdate() Error!! %d", __LINE__, 0U);
                }
            }
        }
    }

    /* create msg queue */
    if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcFcQueId,
                                         SvcFcQueueName,
                                         sizeof(SVC_FCWS_FCMD_BBX_LIST_s),
                                         SvcFcQue,
                                         SVC_FCWS_FCMD_QUEUE_SIZE * sizeof(SVC_FCWS_FCMD_BBX_LIST_s))) {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcwsFcmdTask_Init: Fail to create msg queue", 0U, 0U);
    } else {
        //Do nothing
    }

    /* Create FC task */
    SvcFcTaskCtrl.Priority   = 53;
    SvcFcTaskCtrl.EntryFunc  = SvcFcTaskEntry;
    SvcFcTaskCtrl.pStackBase = SvcFcTaskStack;
    SvcFcTaskCtrl.StackSize  = SVC_FCWS_FCMD_TASK_STACK_SIZE;
    SvcFcTaskCtrl.CpuBits    = 0x1U;
    SvcFcTaskCtrl.EntryArg   = 0x0U;

    RetVal = SvcTask_Create("SvcFcTask", &SvcFcTaskCtrl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFc_TaskCreate create fail", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        UINT32 i;
        UINT32 CvFlowNum = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    UINT32 CvFlowRegisterID = 0U;
                    RetVal |= SvcCvFlow_Register(i, SvcFcTask_DetectionResultCallback, &CvFlowRegisterID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcwsFcmdTask_Init err. SvcCvFlow_Register failed %d", RetVal, 0U);
                    }
                }
            }
        }
    }

    if (1U == FcCalibFromNAND) {
        INT32 Zone1X[4], Zone1Y[4];
        const SVC_GUI_CANVAS_s *pCanvas;
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        UINT32 CvFlowNum = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
        UINT32 RatioX, RatioY, i, FovIdx = 0U;

        pCanvas = SvcSegDrawTask_GetCanvasROIinfo();

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                    FovIdx = pCvFlow[i].InputCfg.Input[0].StrmId;
                }
            }
        }

        RatioX = ((UINT32)pResCfg->FovCfg[FovIdx].MainWin.Width) /(pCanvas->Width);
        RatioY = ((UINT32)pResCfg->FovCfg[FovIdx].MainWin.Height)/(pCanvas->Height);

        (void)FCWS_GetWarnZonePnts_TtcBase(CalibData, &RfFcCfg.WsFcwsCfg, NULL, Zone1X, Zone1Y);
        for (i = 0U; i< 4U; i++) {
            Zone1X[i] = (((Zone1X[i]) / (INT32)RatioX) + (INT32)pCanvas->StartX);
            Zone1Y[i] = (((Zone1Y[i]) /(INT32)RatioY) + (INT32)pCanvas->StartY);
            AmbaPrint_PrintUInt5("Zone1X[%d] = %d, Zone1Y[%d] = %d",
                                 i,
                                 (UINT32)Zone1X[i],
                                 i,
                                 (UINT32)Zone1Y[i],
                                 0U);
        }

        SvcAdasNotify_WarningZoneEnable(0U);
        SvcAdasNotify_WarningZoneUpdate(Zone1X, Zone1Y, sizeof(Zone1X), sizeof(Zone1Y));
    }

    if (SVC_OK == RetVal) {
        FcInit = 1U;
    } else {
        //Do nothing
    }

    return RetVal;
}

/**
 *  Start FC module
 *  @return error code
 */
UINT32 SvcFcTask_Start(void)
{
    UINT32 RetVal;

    if (FcCalibFromNAND == 1U) {
        if (FcInit == 1U) {
            FcOnOff = 1U;
            RetVal = SVC_OK;
        } else {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "FCWS and FCMD module not init yet. Cannot start", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[V1] SvcFcTask_Start() Abort ADAS-FCWS/FCMD enable due to calib data not in NAND", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Start warning icon display
 */
UINT32 SvcFcTask_Stop(void)
{
    UINT32 RetVal;

    if (FcInit == 1U) {
        FcOnOff = 0U;
        WarningIconOnOff = 0U;
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "FCWS and FCMD module not init yet. Cannot stop", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Stop warning icon display
 */
void SvcFcTask_WarningIcanEnable(void)
{
    if (FcCalibFromNAND == 1U) {
        if ((FcInit == 1U) && (FcOnOff == 1U)) {
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1_2x, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2_2x, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD_2x, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            (void) FcmdWarningContinuousCheck(1U, AMBA_WS_FCMD_EVENT_NO_WARNING);
            WarningIconOnOff = 1U;
        } else {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "FC module not init yet(%d) or FC disable right now(%d). Cannot start", FcInit, FcOnOff);
        }
    } else {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[V1] SvcFcTask_WarningIcanEnable() Abort ADAS-FCWS/FCMD enable due to calib data not in NAND", 0U, 0U);
    }
}

/**
 *  Stop FC module
 */
void SvcFcTask_WarningIcanDisable(void)
{
    if (FcInit == 1U) {
        WarningIconOnOff = 0U;
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1, SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1_2x, SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2, SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2_2x, SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD, SVC_WARN_ICON_FLG_ENA);
        (void) SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD_2x, SVC_WARN_ICON_FLG_ENA);
        SvcAdasNotify_FcwsSafeDistUpdate(0.0);
    } else {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "FCWS and FCMD module not init yet. Cannot stop", 0U, 0U);
    }
}

/**
 *  Get current status of FC module
 *  @param[in] pEnable current FC module statue (On or Off)
 *  @return error code
 */
UINT32 SvcFcTask_GetStatus(UINT32 *pEnable)
{
    UINT32 RetVal;

    if (FcInit == 1U) {
        *pEnable = FcOnOff;
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "FCWS and FCMD module not init yet. Cannot get status", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}
