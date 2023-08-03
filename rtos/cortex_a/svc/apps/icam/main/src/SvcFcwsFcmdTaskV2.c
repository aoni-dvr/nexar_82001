/**
 *  @file SvcFcwsFcmdTaskV2.c
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
 *  @details Svc FCWS and FCMD V2 ADAS
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
#include "AmbaAP_LKA.h"
#include "RefFlow_FC.h"
#include "SvcFcwsFcmdTaskV2.h"
#include "SvcLdwsTaskV2.h"
#include "SvcCan.h"
#include "SvcWarningIconTask.h"
#include "SvcCvCamCtrl.h"
#include "SvcAdasNotify.h"
#include "SvcUserPref.h"
#include "SvcODDrawTask.h"
#include "SvcSegDrawTask.h"

#include "SvcCvAlgo.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "cv_ambaod_header.h"
#include "cvapi_svccvalgo_ambaperception.h"

#define SVC_LOG_FCWS_FCMD_TASK     "FC_TASK"
#define SVC_FCWS_FCMD_QUEUE_SIZE   128U

#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
#define SVC_APP_CV_FOV_NUM_MAX   (4U)
#endif
#define HorizontalLine_Thickness      (3U)
#define SVC_FCWS_FCMD_TASK_STACK_SIZE (0x100000)

static UINT32 g_IsGet1stODresult = 0U;
static UINT32 FcInit = 0U;
static UINT32 FcCalibFromNAND = 0U;
static UINT32 FcOnOff = 0U;  /* Disable to draw detection on OSD by default */
static UINT32 WarningIconOnOff = 0U;  /* Disable WarningIcon display by default */
//static AMBA_KAL_MSG_QUEUE_t  SvcFcQueId GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MSG_QUEUE_t  SvcFc2stageQueId GNU_SECTION_NOZEROINIT;
static AMBA_KAL_MSG_QUEUE_t  SvcFcProcV2QueId GNU_SECTION_NOZEROINIT;

static AMBA_KAL_SEMAPHORE_t SvcFcProcV2Sem GNU_SECTION_NOZEROINIT;
static AMBA_KAL_SEMAPHORE_t SvcFc2stageSem GNU_SECTION_NOZEROINIT;



static SVC_CALIB_ADAS_INFO_GET_s *g_CalibData = NULL;
static AMBA_OD_2DBBX_LIST_s            g_BbxOutOri;
static AMBA_OD_2DBBX_LIST_s            g_BbxOut;
static AMBA_SR_SROBJECT_DATA_s         g_SROut;
static REF_FLOW_FC_OUT_DATA_V2_s       g_FCOut;
static REF_FLOW_FC_SR_IMAGE_3D_MODEL_s g_SrImage3d;

//SR_MODE_HOR = 0, SR_MODE_VER = 1
static UINT32 g_FcV2_SR_mode GNU_SECTION_NOZEROINIT;

#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION
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

static void SvcFc_BbxTransSvcToWs(AMBA_OD_2DBBX_LIST_s *pBbxdataWS, const SVC_FCWS_FCMD_BBX_LIST_V2_s *pList) {
    pBbxdataWS->MsgCode = pList->MsgCode;
    pBbxdataWS->CaptureTime = pList->CaptureTime;
    pBbxdataWS->FrameNum = pList->FrameNum;
    pBbxdataWS->NumBbx = pList->BbxAmount;
    AmbaSvcWrap_MisraMemcpy(pBbxdataWS->Bbx, pList->Bbx, SVC_CV_DETRES_MAX_BBX_NUM * sizeof(SVC_CV_DETRES_BBX_WIN_s));
}
#endif



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
    } else if (DataFmt == RF_FC_DFMT_2DBBX_IN) {
        static AMBA_OD_2DBBX_LIST_s *pAddr;// = (AMBA_OD_2DBBX_LIST_s *)pDataAddr;
        AmbaMisra_TypeCast(&pAddr, &(pHeader->pDataAddr));
        AmbaSvcWrap_MisraMemcpy(&g_BbxOutOri, pAddr, DataSize);
    } else if (DataFmt == RF_FC_DFMT_SR) {
        static AMBA_SR_SROBJECT_DATA_s *pAddr;// = (AMBA_SR_SROBJECT_DATA_s *)pDataAddr;
        AmbaMisra_TypeCast(&pAddr, &(pHeader->pDataAddr));
        AmbaSvcWrap_MisraMemcpy(&g_SROut, pAddr, DataSize);
    } else if (DataFmt == RF_FC_DFMT_FC_OUT_V2) {
        AmbaSvcWrap_MisraMemcpy(&g_FCOut, pHeader->pDataAddr, sizeof(REF_FLOW_FC_OUT_DATA_V2_s));
    } else if (DataFmt == RF_FC_DFMT_FC_SR_IMAGE_3D_OUT) {
        AmbaSvcWrap_MisraMemcpy(&g_SrImage3d, pHeader->pDataAddr, sizeof(REF_FLOW_FC_SR_IMAGE_3D_MODEL_s));
    } else if (DataFmt == RF_FC_DFMT_FCWS_CFG) {
    } else if (DataFmt == RF_FC_DFMT_FCMD_CFG) {
    } else {
        //Do nothing
    }

    AmbaMisra_TouchUnused(&HdrVer);
    AmbaMisra_TouchUnused(&Count);
    AmbaMisra_TouchUnused(&Channel);
    AmbaMisra_TouchUnused(pHeader);

    return ADAS_ERR_NONE;
}

static void SvcFcPcpt3DResult(const void *pCvOut, obj_t **p3DObj)
{
    const amba_odv37_step8_out_t *p3DOut;
    ULONG ObjAddr;

    AmbaMisra_TypeCast(&p3DOut, &pCvOut);
    AmbaMisra_TypeCast(&ObjAddr, &p3DOut);
    ObjAddr = ObjAddr + p3DOut->objects_offset;
    AmbaMisra_TypeCast(&(*p3DObj), &ObjAddr);

//    AmbaPrint_PrintUInt5("Receive 3D total: %d", p3DOut->num_objects, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();

}

static void PerceptionStatusCallback(UINT32 CvFlowChan, UINT32 Event, void *pInfo)
{
#ifdef CONFIG_CV_FLEXIDAG_AMBAPERCEPTION

    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Event);
    AmbaMisra_TouchUnused(&pInfo);

    if (FcOnOff == 1U) {
        if (Event == SVC_CV_STATUS_AMBAOD_PCPT_PREPROC) {
            SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut;
            UINT32 RetVal;

            RetVal = AmbaKAL_SemaphoreTake(&SvcFcProcV2Sem, AMBA_KAL_WAIT_FOREVER);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] PerceptionStatusCallback() SemaphoreTake Error!! %d", __LINE__, 0U);
            }

            /* Already get 1st OD result */
            g_IsGet1stODresult = 1U;

            /* Fill obj index in ObjIdxList. If obj list is not full, fill in a 0xff at the end the list.
               Set the ListUpdate flag updating the list and number of obj.
            */
            {
                static SVC_FCWS_FCMD_BBX_LIST_V2_s FcBbxData;
                UINT32 Rval = 0U;

                AmbaMisra_TypeCast(&pPcptOut, &pInfo);

                FcBbxData.Chan        = CvFlowChan;
                FcBbxData.CvType      = 0U;
                FcBbxData.BbxAmount = pPcptOut->pBbxList->BbxAmount;
                FcBbxData.CaptureTime = pPcptOut->pBbxList->CaptureTime;
                FcBbxData.FrameNum = pPcptOut->pBbxList->FrameNum;
                FcBbxData.MsgCode = pPcptOut->pBbxList->MsgCode;
                FcBbxData.Source = pPcptOut->pBbxList->Source;
                FcBbxData.Bbx =  pPcptOut->pBbxList->Bbx;
                FcBbxData.class_name = pPcptOut->pBbxList->class_name;

                //FC_PreProcessV2()
                {
                    static AMBA_SR_CANBUS_RAW_DATA_s FcV2CanbusRawData = { 0 };// for adas
                    static AMBA_OD_2DBBX_LIST_s BbxdataWS GNU_SECTION_NOZEROINIT;
                    static AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTransData; // canbus transfer data
                    static REF_FLOW_FC_PRE_PROC_IN_V2_s  PreProcIn  GNU_SECTION_NOZEROINIT;
                    static REF_FLOW_FC_PRE_PROC_OUT_V2_s PreProcOut GNU_SECTION_NOZEROINIT;

                    AmbaSvcWrap_MisraMemset(&BbxdataWS, 0, sizeof(BbxdataWS));
                    AmbaSvcWrap_MisraMemset(&PreProcIn, 0, sizeof(REF_FLOW_FC_PRE_PROC_IN_V2_s));

                    if (FcBbxData.Source < SVC_APP_CV_FOV_NUM_MAX) {
                        SvcCan_GetRawData(&FcV2CanbusRawData);
                        if((SVC_OK == SvcFcTask_IsDecMode()) && (FcV2CanbusRawData.FlagValidRawData == 0U)) {
                            //Decode mode
                            Rval = SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)FcBbxData.FrameNum, &CanbusTransData);
                            AmbaMisra_TouchUnused(&Rval);
                            FcBbxData.CaptureTime = CanbusTransData.CapTS;
                            //Showing current speed under decode mode
                            SvcAdasNotify_DecModeDrawSpeed(CanbusTransData.TransferSpeed);
                        } else if ((SVC_NG == SvcFcTask_IsDecMode()) && (FcV2CanbusRawData.FlagValidRawData == 1U)){
                            //Liveview with CAN bus
                            Rval = SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)FcBbxData.CaptureTime, &CanbusTransData);
                            AmbaMisra_TouchUnused(&Rval);
                        } else {
                            //Liveview without CAN bus
                            CanbusTransData.FlagValidTransferData = 1U;
                            CanbusTransData.TransferSpeed = 80.0;
                            CanbusTransData.CANBusTurnLightStatus = AMBA_SR_CANBUS_TURN_LIGHT_N;
                            CanbusTransData.CANBusGearStatus = AMBA_SR_CANBUS_GEAR_D;
                            CanbusTransData.CapTS = pPcptOut->pBbxList->CaptureTime;
                            FcBbxData.CaptureTime = pPcptOut->pBbxList->CaptureTime;
                        }

                        SvcFc_BbxTransSvcToWs(&BbxdataWS, &FcBbxData);
                        AmbaSvcWrap_MisraMemcpy(&PreProcIn.BbxList, &BbxdataWS, sizeof(AMBA_OD_2DBBX_LIST_s));
                        AmbaSvcWrap_MisraMemcpy(&PreProcIn.Canbus, &CanbusTransData, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s));


                        RetVal = RefFlowFC_PreProcessV2(&PreProcIn, &PreProcOut);
                        if (RetVal != SVC_OK) {
                            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] RefFlowFC_PreProcessV2 Error!! %d", __LINE__, 0U);
                        }
#if 0
                        {
                            AMBA_OD_2DBBX_LIST_s* pBbxList;

                            AmbaPrint_PrintUInt5("[SVC_PreProcess] Input 2D Bbox:", 0U, 0U, 0U, 0U, 0U);
                            pBbxList = &PreProcIn.BbxList;
                            for (UINT32 i = 0U; i < pBbxList->NumBbx; i++) {
                                AmbaPrint_PrintUInt5("     Id = %d, Bbox(X,Y,W,H) = (%d, %d, %d, %d)", pBbxList->Bbx[i].ObjectId, pBbxList->Bbx[i].X, pBbxList->Bbx[i].Y, pBbxList->Bbx[i].W, pBbxList->Bbx[i].H);
                            }

                            AmbaPrint_PrintUInt5("[SVC_PreProcess] Output 2D Bbox:", 0U, 0U, 0U, 0U, 0U);
                            pBbxList = &PreProcOut.BbxList;
                            for (UINT32 i = 0U; i < pBbxList->NumBbx; i++) {
                                AmbaPrint_PrintUInt5("     Id = %d, Bbox(X,Y,W,H) = (%d, %d, %d, %d)", pBbxList->Bbx[i].ObjectId, pBbxList->Bbx[i].X, pBbxList->Bbx[i].Y, pBbxList->Bbx[i].W, pBbxList->Bbx[i].H);
                            }

                            AmbaPrint_PrintUInt5("[SVC_PreProcess] Sel object for doing 3D Bbox:", 0U, 0U, 0U, 0U, 0U);
                            for (UINT32 i = 0U; i < PreProcOut.ValidObj.TwoD3DData.ObjNum; i++) {
                                AmbaPrint_PrintUInt5("     Sel Id = %d", PreProcOut.ValidObj.TwoD3DData.ValidID[i], 0U, 0U, 0U, 0U);
                            }

                            AmbaPrint_PrintUInt5("[SVC_PreProcess] Sel object for doing Seg:", 0U, 0U, 0U, 0U, 0U);
                            for (UINT32 i = 0U; i < PreProcOut.ValidObj.SegData.ObjNum; i++) {
                                AmbaPrint_PrintUInt5("     Sel Id = %d", PreProcOut.ValidObj.SegData.ValidID[i], 0U, 0U, 0U, 0U);
                            }
                        }
#endif




                        if (pPcptOut->PcptData[AMBANET_OD37_3D].Enable == 1U) {
                            UINT32 i, FCPre_idx;
                            UINT8 Count3D;
                            SVC_CV_PERCEPTION_DATA_s *pPcptData = NULL;
                            const AMBA_OD_2DBBX_LIST_s* pBbxList;

                            Count3D = 0;
                            pBbxList = &PreProcOut.BbxList;
                            for(FCPre_idx = 0; FCPre_idx < PreProcOut.ValidObj.TwoD3DData.ObjNum; FCPre_idx++) {
                                for (i = 0U ; i < FcBbxData.BbxAmount; i++) {
                                    /* Prepare the list for sub networks (3D). */
                                    pPcptData = &pPcptOut->PcptData[AMBANET_OD37_3D];
                                    /* Hit ObjectId */
                                    if (PreProcOut.ValidObj.TwoD3DData.ValidID[FCPre_idx] == pBbxList->Bbx[i].ObjectId) {
                                        pPcptData->pObjIdxList[Count3D] = (UINT8) i;
                                        Count3D++;
                                        break;
                                    }
                                }
                            }


                            if (pPcptData != NULL) {
                                for(UINT8 ReOrgObjId = 0U; ReOrgObjId < Count3D; ReOrgObjId++) {
                                    UINT32 ReOrdIdx = pPcptData->pObjIdxList[ReOrgObjId];
                                    PreProcOut.ValidObj.TwoD3DData.ValidID[ReOrgObjId] = pBbxList->Bbx[ReOrdIdx].ObjectId;
                                }
                            }
                            pPcptData = &pPcptOut->PcptData[AMBANET_OD37_3D];
                            pPcptData->pObjIdxList[Count3D] = 0xFF;
                            pPcptData->NumObj = Count3D;
                            pPcptData->ListUpdate = 1U;
                        }

                        {
                            static SVC_FC_PROC_IN_DATA_V2_s FCprocInV2;
                            FCprocInV2.pPreProcIn  = &PreProcIn;
                            FCprocInV2.pPreProcOut = &PreProcOut;

                            if (SVC_OK != AmbaKAL_MsgQueueSend(&SvcFcProcV2QueId, &FCprocInV2, AMBA_KAL_WAIT_FOREVER)) {
                                SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "AmbaKAL_MsgQueueSend SvcFcProcV2QueId failed", 0U, 0U);
                            } else {
                                //Do nothing
                            }
                        }
                    }
                }
            }
        }
    }
#endif
    AmbaMisra_TouchUnused(&CvFlowChan);
    AmbaMisra_TouchUnused(&Event);
    AmbaMisra_TouchUnused(pInfo);
}

static void DetectionResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg)
{
    if ((FcOnOff == 1U) &&(1U == g_IsGet1stODresult)) {
        static SVC_FC_PROC_2Stage_INFO_s Info_2stage;
        UINT32 RetVal;

        RetVal = AmbaKAL_SemaphoreTake(&SvcFc2stageSem, AMBA_KAL_WAIT_FOREVER);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] DetectionResultCallback() SemaphoreTake Error!! %d", __LINE__, 0U);
        }
        AmbaMisra_TypeCast(&Info_2stage.pPcptOut, &pDetResMsg);
        Info_2stage.Chan   = Chan;
        Info_2stage.CvType = CvType;

        if (SVC_OK != AmbaKAL_MsgQueueSend(&SvcFc2stageQueId, &Info_2stage, AMBA_KAL_WAIT_FOREVER)) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "AmbaKAL_MsgQueueSend SvcFc2stageQueId failed", 0U, 0U);
        } else {
            //Do nothing
        }
    }

    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TouchUnused(pDetResMsg);
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

static void* SvcFcTaskEntryV2(void* EntryArg)
{
    DOUBLE SafeDistance, TTC;
    UINT8  IsTrue = 1U;
    UINT32 Rval, RetVal = SVC_NG;
    static SVC_FC_PROC_IN_DATA_V2_s  FCpreprocInV2;
    static SVC_FC_PROC_2Stage_INFO_s FCproc2stage;
    UINT32 TwoD3DDataObjNum;
    static REF_FLOW_FC_IN_DATA_V2_s  FcInDataV2;
    static REF_FLOW_FC_OUT_DATA_V2_s FcOutDataV2;
    static REF_FLOW_FC_NN_DATA_s NNData;
    static REF_FLOW_FC_NN_INFO_s ObjNNInfoList[FC_IN_DATA_V2_3D_MAX];
    const AMBA_OD_2DBBX_LIST_s* pOutBbxList;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&IsTrue);
    while (0U != IsTrue) {
        Rval = AmbaKAL_MsgQueueReceive(&SvcFcProcV2QueId, &FCpreprocInV2, AMBA_KAL_WAIT_FOREVER);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcQueId SvcFcProcV2QueId MsgQue Receive error !", 0U, 0U);
        } else {
            //Do nothing
        }

        Rval = AmbaKAL_MsgQueueReceive(&SvcFc2stageQueId, &FCproc2stage, AMBA_KAL_WAIT_FOREVER);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcQueId SvcFc2stageQueId MsgQue Receive error !", 0U, 0U);
        } else {
            //Do nothing
        }
        TwoD3DDataObjNum = FCpreprocInV2.pPreProcOut->ValidObj.TwoD3DData.ObjNum;

        {
            obj_t *p3DObj = NULL;

            if (FCproc2stage.pPcptOut->PcptData[AMBANET_OD37_3D].Enable == 1U) {//Protect that SvcFcPcpt3DResult() called when OD37_3D is enable
                if (TwoD3DDataObjNum > 0U) {
                    //Get/Parse 2-stage info from CB
                    SvcFcPcpt3DResult(FCproc2stage.pPcptOut->PcptData[AMBANET_OD37_3D].pCvOut, &p3DObj);
                    if (p3DObj == NULL) {
                        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "Error! 3DObj is NULL!!", 0U, 0U);
                        AmbaPrint_Flush();
                        continue;
                    } else {
                        //Do nothing
                    }
                }
            }

            AmbaSvcWrap_MisraMemcpy(&FcInDataV2.BbxList, &FCpreprocInV2.pPreProcOut->BbxList, sizeof(AMBA_OD_2DBBX_LIST_s));
            AmbaSvcWrap_MisraMemcpy(&FcInDataV2.CanbusTransData, &FCpreprocInV2.pPreProcIn->Canbus, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s));
            SvcLdwsTaskV2_GetLanInfo(&FcInDataV2.SRLaneInfo);
            AmbaSvcWrap_MisraMemset(&NNData, 0, sizeof(REF_FLOW_FC_NN_DATA_s));
            AmbaSvcWrap_MisraMemset(ObjNNInfoList, 0, (sizeof(REF_FLOW_FC_NN_INFO_s)*FC_IN_DATA_V2_3D_MAX));
            FcInDataV2.pNNData = &NNData;
            NNData.pObjNNInfoList = ObjNNInfoList;


            pOutBbxList = &FCpreprocInV2.pPreProcOut->BbxList;
            NNData.ObjNum = pOutBbxList->NumBbx;

            //If OD37_3D Enable == 0 means no Call-back 3D bbx data
            if (FCproc2stage.pPcptOut->PcptData[AMBANET_OD37_3D].Enable == 0U) {
                NNData.ObjNum = 0U; //No 3D bbx, fill 0
            }

            for (UINT32 Idx2D = 0U; Idx2D < NNData.ObjNum; Idx2D++) {//Scan each preprocess of PreProcOut->BbxList->NumBbx
                UINT32 ObjId = 0U;
                ObjId = pOutBbxList->Bbx[Idx2D].ObjectId;
                ObjNNInfoList[Idx2D].ObjId = ObjId;
                ObjNNInfoList[Idx2D].ObjNNDataFlag = 0U;
                /* ObjNNDataFlag:
                 * 0x0(00) => NO SEG and OD;
                 * 0x1(01) => Have SEG;
                 * 0x2(10) => Have OD;
                 * 0x3(11) => Have SEG and OD;
                */

                //Only search when ObjId != 0
                if (0U != ObjId) {
                    UINT8  Find3DIdx = 0U;
//                    UINT8  FindSegIdx = 0U;
//                    UINT32 IdxSEG = 0U;
                    UINT32 Idx3D = 0U;

                    //Search table have 3D ID?
                    for (Idx3D = 0U; Idx3D < TwoD3DDataObjNum; Idx3D++) {
                        //Find it!
                        if (ObjId == FCpreprocInV2.pPreProcOut->ValidObj.TwoD3DData.ValidID[Idx3D]) {
                            Find3DIdx = 1U;
                            break;
                        }
                    }

                    //Search table have SEG ID?
//                    for (IdxSEG = 0U; IdxSEG < FCpreprocInV2.pPreProcOut->ValidObj.SegData.ObjNum; IdxSEG++) {
//                        if (ObjId == FCpreprocInV2.pPreProcOut->ValidObj.SegData.ValidID[IdxSEG]) {
//                            FindSegIdx = 1U;
//                            break;
//                        }
//                    }

                    /* Find 3D bbx idx */
                    if (0U != Find3DIdx) {

                        ObjNNInfoList[Idx2D].ObjNNDataFlag |= 2U;
                        AmbaSvcWrap_MisraMemcpy(&ObjNNInfoList[Idx2D].ObjNN3DInfo, &p3DObj[Idx3D], sizeof(AMBA_OD_3DBBX_s));
                    } else {
                        //Do nothing
                    }

                    /* Find SEG bbx idx */
//                    if (0U != FindSegIdx) {
//                        ObjNNInfoList[Idx2D].ObjNNDataFlag |= 1U;
//                    } else {
//                        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "Error! Not find SEG idx !", 0U, 0U);
//                    }
                    //Hard code
                    AmbaSvcWrap_MisraMemset(&ObjNNInfoList[Idx2D].ObjNNSegInfo, 0, sizeof(AMBA_SR_OBJ_SEG_INFO));
                }

            }

            Rval = RefFlowFC_ProcessV2(&FcInDataV2, &FcOutDataV2);

            /* Draw FC 2D/3D Bbx results */
            if(Rval == SVC_OK) {
                static SVC_FC_DRAW_INFO_V2_s FCinfo;

                FCinfo.Chan               = FCproc2stage.Chan;
                FCinfo.CvType             = FCproc2stage.CvType;
                FCinfo.BbxOut.MsgCode     = FCproc2stage.pPcptOut->MsgCode;
                FCinfo.BbxOut.Source      = FCproc2stage.pPcptOut->pBbxList->Source;
                FCinfo.BbxOut.class_name  = FCproc2stage.pPcptOut->pBbxList->class_name;
                if (g_FcV2_SR_mode == RF_FC_SR_MODE_HOR) {
                    FCinfo.BbxOut.CaptureTime = g_BbxOut.CaptureTime;
                    FCinfo.BbxOut.FrameNum    = g_BbxOut.FrameNum;
                    FCinfo.BbxOut.BbxAmount   = g_BbxOut.NumBbx;
                    if (g_BbxOut.NumBbx > 0U) {
                        AmbaSvcWrap_MisraMemcpy(FCinfo.BbxOut.Bbx, g_BbxOut.Bbx, (sizeof(AMBA_OD_2DBBX_s)*g_BbxOut.NumBbx));
                    } else {
                        AmbaSvcWrap_MisraMemset(FCinfo.BbxOut.Bbx, 0, (sizeof(AMBA_OD_2DBBX_s)*SVC_CV_DETRES_MAX_BBX_NUM));
                    }
                } else if (g_FcV2_SR_mode == RF_FC_SR_MODE_VER) {
                    FCinfo.BbxOut.CaptureTime = g_BbxOutOri.CaptureTime;
                    FCinfo.BbxOut.FrameNum    = g_BbxOutOri.FrameNum;
                    FCinfo.BbxOut.BbxAmount   = g_BbxOutOri.NumBbx;
                    if (g_BbxOutOri.NumBbx > 0U) {
                        AmbaSvcWrap_MisraMemcpy(FCinfo.BbxOut.Bbx, g_BbxOutOri.Bbx, (sizeof(AMBA_OD_2DBBX_s)*g_BbxOutOri.NumBbx));
                    } else {
                        AmbaSvcWrap_MisraMemset(FCinfo.BbxOut.Bbx, 0, (sizeof(AMBA_OD_2DBBX_s)*SVC_CV_DETRES_MAX_BBX_NUM));
                    }
                } else {
                    SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] g_FcV2_SR_mode error, g_FcV2_SR_mode = %d", g_FcV2_SR_mode, 0U);
                }
                AmbaSvcWrap_MisraMemcpy(&(FCinfo.SROut), &g_SROut, sizeof(AMBA_SR_SROBJECT_DATA_s));
                AmbaSvcWrap_MisraMemcpy(&(FCinfo.FCOut), &g_FCOut, sizeof(REF_FLOW_FC_OUT_DATA_s));
                AmbaSvcWrap_MisraMemcpy(&(FCinfo.PcptOut), FCproc2stage.pPcptOut, sizeof(SVC_CV_PERCEPTION_OUTPUT_s));

                //Update WarningZon info from FC module
                SvcAdasNotify_UpdateFCWarningZone(&FCinfo.FCOut.WsFcwsStat.SubjectLaneInfo);
                if (g_SrImage3d.TotalNum > 0U) {
                    AmbaSvcWrap_MisraMemcpy(&FCinfo.Info3Dbbx, &g_SrImage3d, sizeof(REF_FLOW_FC_SR_IMAGE_3D_MODEL_s));
                } else {
                    AmbaSvcWrap_MisraMemset(&FCinfo.Info3Dbbx, 0, sizeof(REF_FLOW_FC_SR_IMAGE_3D_MODEL_s));
                }

                /* No need to high light target vehicle if WarningIconOnOff disable */
                if(0U == WarningIconOnOff) {
                    FCinfo.FCOut.WsFcwsStat.ObjectStatus.StatusUpdate = 0U;
                } else {
//                    SVC_WRAP_PRINT "FCinfo.FCOut.ApAccStat.RequiredSpeed %5f"
//                    SVC_PRN_ARG_S "RequiredSpeed"
//                    SVC_PRN_ARG_PROC SvcLog_OK
//                    SVC_PRN_ARG_DOUBLE  FCinfo.FCOut.ApAccStat.RequiredSpeed
//                    SVC_PRN_ARG_E
                    /* Update ACC RequiredSpeed */
                    SvcAdasNotify_SetACCRequiredSpeed(FCinfo.FCOut.ApAccStat.RequiredSpeed);

//                    SVC_WRAP_PRINT "FCinfo.FCOut.ApAccStat.RequiredAcceleration %5f"
//                    SVC_PRN_ARG_S "RequiredAcceleration"
//                    SVC_PRN_ARG_PROC SvcLog_OK
//                    SVC_PRN_ARG_DOUBLE  FCinfo.FCOut.ApAccStat.RequiredAcceleration
//                    SVC_PRN_ARG_E
                    /* Update ACC Acceleration */
                    SvcAdasNotify_SetACCAcceleration(FCinfo.FCOut.ApAccStat.RequiredAcceleration);
                }

                Rval = SvcODDrawTask_Msg(SVC_FC_PCPT_DRAW, &FCinfo);
                if (SVC_NG == Rval) {
                    SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcODDrawTask_Msg() error !", 0U, 0U);
                } else {
                    //Do nothing
                }

                if(1U == WarningIconOnOff) {
                    SvcADAS_NOTIFY_ADAS_PRIORITY_e FcEvent = ADAS_PRIORITY_NONE;
                    UINT32 FmcdWarning, FcwsWarning;

                    /* FCWS Result*/
                    FcwsWarning = FcwsWarningCheck(FCinfo.FCOut.WsFcwsStat.Event);
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
                    if (1U == FCinfo.FCOut.WsFcwsStat.ObjectStatus.StatusUpdate) {
                        TTC = (FCinfo.FCOut.WsFcwsStat.ObjectStatus.TTC); //UINT: msec
                        SafeDistance = (FCinfo.FCOut.WsFcwsStat.ObjectStatus.SafeDistance / 1000.0); //UINT: mm to m
                    }

                    //Update SafetyDist
                    SvcAdasNotify_FcwsSafeDistUpdate(SafeDistance);

                    //Update TTC
                    SvcAdasNotify_FcwsTtcUpdate(TTC);


                    /* FCMD Result*/
                    FmcdWarning = FcmdWarningContinuousCheck(0U, FCinfo.FCOut.WsFcmdStat.Event);
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


            }
        }

        RetVal = AmbaKAL_SemaphoreGive(&SvcFcProcV2Sem);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] SvcFcTaskEntry() SemaphoreGive SvcFcProcV2Sem Error!! %d", __LINE__, 0U);
        }

        RetVal = AmbaKAL_SemaphoreGive(&SvcFc2stageSem);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] SvcFcTaskEntry() SemaphoreGive SvcFc2stageSem Error!! %d", __LINE__, 0U);
        }
    }

    return NULL;
}

static UINT32 SvcFc_GetCalibrationData(void)
{
    UINT32 Rval;
    static UINT32 FcwsFcmdCalibInitFlag = 0U;
    static SVC_CALIB_ADAS_INFO_GET_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
    static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface;

    if (FcwsFcmdCalibInitFlag == 0U) {
        Cfg.AdasCfgV1.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
        Cfg.AdasCfgV1.TransCfg.pCurvedSurface = &CurvedSurface;
        Rval = SvcCalib_AdasCfgGetV1(SVC_CALIB_ADAS_TYPE_FRONT, &Cfg);
        if(Rval == SVC_OK) {
            SvcLog_OK("SvcFc_GetCalibrationData", "[FC][V2] Get ADAS CAM CALIB from NAND !Ver: ", SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT), 0U);
            g_CalibData = &Cfg;
            if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                FcwsFcmdCalibInitFlag = 1U;
            }
        }
    }

    return FcwsFcmdCalibInitFlag;
}

static UINT32 GetHorizontalLine(const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCalCfg, UINT32 pntX[2], UINT32 pntY[2]) {
    UINT32 retVal = 0U;
    UINT32 srcW = pCalCfg->VoutArea.Width;
    UINT32 srcH = pCalCfg->VoutArea.Height;
    UINT32 ofsX = pCalCfg->VoutArea.StartX;
    UINT32 ofsY = pCalCfg->VoutArea.StartY;
    UINT32 x, y;
    UINT32 Rval;

    x = ofsX;
	y = 0U;
    Rval = AmbaCal_EmFindHorizonPositionV1(pCalCfg, x, ofsY + (srcH/2U), (srcH/2U), &y);
    AmbaMisra_TouchUnused(&Rval);
    pntX[0U] = x;
    pntY[0U] = y;

    if (y > (ofsY + srcH - 1U)) {
        retVal = 1U;
    }

    if (retVal == 0U) {
        x = ofsX+srcW -1U;
        Rval = AmbaCal_EmFindHorizonPositionV1(pCalCfg, x, ofsY + (srcH/2U), (srcH/2U), &y);
        AmbaMisra_TouchUnused(&Rval);
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
static UINT32 FCWS_GetWarnZonePnts_TtcBase(const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCalCfg,
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
                rVal = AmbaCal_EmConvPtWorldToImgV1(pCalCfg, &pnt3D[i], &pnt2D[i]);
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
                    AmbaPrint_PrintStr5("%s, AmbaCal_EmConvPtWorldToImgV1 failed", __func__, NULL, NULL, NULL, NULL);
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
UINT32 SvcFcTaskV2_Init(void)
{
    UINT32 RetVal = ADAS_ERR_NA;
    static char                     SvcFc2stageName[] = "SvcFc2stageQueue";
    static char                     SvcFcQueueProcV2Name[] = "SvcFcQueueProcV2";
    static UINT8                    SvcFcTaskStack[SVC_FCWS_FCMD_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_TASK_CTRL_s          SvcFcTaskCtrl                                 GNU_SECTION_NOZEROINIT;
    static SVC_FCWS_FCMD_BBX_LIST_V2_s SvcFc2stageQue[SVC_FCWS_FCMD_QUEUE_SIZE]            GNU_SECTION_NOZEROINIT;
    static SVC_FCWS_FCMD_BBX_LIST_V2_s SvcFcQueProcV2[SVC_FCWS_FCMD_QUEUE_SIZE]            GNU_SECTION_NOZEROINIT;
    UINT32 Size = 0U;
    static REF_FLOW_FC_CFG_V2_s RfFcCfg;
    static UINT8 *WrkBufAddr;
    static SVC_USER_PREF_s        *pSvcUserPref;
    static UINT32                 FcwsMode = AMBA_WS_FCWS_MODE_POSITION;
    static UINT32                 PntX[2], PntY[2];
    SVC_CV_DISP_OSD_CMD_s         HorizontalInfo;
    ULONG   CacheBase;
    UINT32  CacheSize = 0U;

    /* Reset HorizontalLine info */
    AmbaSvcWrap_MisraMemset(&HorizontalInfo, 0, sizeof(SVC_CV_DISP_OSD_CMD_s));
#if defined(CONFIG_ICAM_REBEL_USAGE)
    HorizontalInfo.Channel = VOUT_IDX_A;
#else
    HorizontalInfo.Channel = VOUT_IDX_B;
#endif
    RetVal = SvcAdasNotify_HorizontalLineUpdate(&HorizontalInfo);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] SvcAdasNotify_HorizontalLineUpdate() Error!! %d", __LINE__, 0U);
    }

    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED,
                               SMEM_PF0_ID_ADAS_FC,
                               &CacheBase,
                               &CacheSize);
    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] SvcBuffer_Request() ADAS_FC Error!! RetVal = %d", RetVal, 0U);
    }

    FcCalibFromNAND = SvcFc_GetCalibrationData();
    if (FcCalibFromNAND == 1U){
        RetVal = RefFlowFC_GetDefaultCfgV2(&Size, &RfFcCfg);
        if (RetVal != ADAS_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, RefFlowFC_GetDefaultCfgV2 failed", __func__, NULL, NULL, NULL, NULL);
        }

        AmbaPrint_PrintUInt5("[FC][V2] working buffer size = %d", Size, 0U, 0U, 0U, 0U);
        if (Size > CacheSize) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] Working buffer size Error!! %d", __LINE__, 0U);
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
                            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2][PyramidScale = %d] Align to calibration data size:", RoiInfo.Roi[0].Index, 0U);
                            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2]StartX = %d, StartX = %d", RfFcCfg.ODRoi.StartX, RfFcCfg.ODRoi.StartY);
                            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2]Width = %d, Height = %d", RfFcCfg.ODRoi.Width, RfFcCfg.ODRoi.Height);
                            break;
                        }
                    }
                }
            }

            Lut[0].NNCat = 0U;    Lut[0].ObjType = SR_OBJ_TYPE_PERSON_0;
            Lut[1].NNCat = 2U;    Lut[1].ObjType = SR_OBJ_TYPE_SCOOTER_0;
            Lut[2].NNCat = 3U;    Lut[2].ObjType = SR_OBJ_TYPE_SCOOTER_0;
            Lut[3].NNCat = 4U;    Lut[3].ObjType = SR_OBJ_TYPE_VEHICLE_0;
            Lut[4].NNCat = 5U;    Lut[4].ObjType = SR_OBJ_TYPE_VEHICLE_0;
            RfFcCfg.NNCat2ObjTypeLut.pLut = Lut;
            RfFcCfg.NNCat2ObjTypeLut.LutLength = 5U;
        }
    }

    /** ----------------------------------------------------------------- */
    /** Cfg for SR                                                        */
    /** ----------------------------------------------------------------- */
    /* Init FC if system have CAM position calibration data */
    if ((RetVal == ADAS_ERR_NONE) && (FcCalibFromNAND == 1U)){
        DOUBLE FocallengthUnitCellSize;

        AmbaSvcWrap_MisraMemcpy(&(RfFcCfg.CalibCfg), &(g_CalibData->AdasCfgV1), sizeof(AMBA_CAL_EM_CALC_COORD_CFG_V1_s));
        AmbaSvcWrap_MisraMemcpy(&(RfFcCfg.CalibInfo), &(g_CalibData->AdasInfoDataV1), sizeof(AMBA_CAL_EM_CALIB_INFO_DATA_V1_s));
        RfFcCfg.LogFp = GetResult;

        if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {//TTC mode or position mode
            FcwsMode = pSvcUserPref->FcwsMode;
        } else {
            FcwsMode = AMBA_WS_FCWS_MODE_POSITION;
        }
        if (FcwsMode == AMBA_WS_FCWS_MODE_TTC) {
            RfFcCfg.WsFcwsCfg.FcwsMode = AMBA_WS_FCWS_MODE_TTC;
            RfFcCfg.WsFcwsCfg.FCWSSpec.RelativeSpeedStride = (DOUBLE)SVC_FC_RELATIVE_SPEED_STRIDE_V2;
            for (UINT32 i = 0U; i < TTC_LEVEL; i++) {
                RfFcCfg.WsFcwsCfg.FCWSSpec.TTC_th[i] = (DOUBLE)SVC_FC_TTC_M_V2;
            }
            for (UINT32 i = 0U; i < AMBA_WS_FCWS_MAX_SPEED; i++) {
                RfFcCfg.WsFcwsCfg.FCWSSpec.Position_th[i] = (DOUBLE)i*0.1;
            }
            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "FCWS_MODE: TTC mode", 0U, 0U);
        } else if (FcwsMode == AMBA_WS_FCWS_MODE_POSITION) {
            SvcLog_OK(SVC_LOG_FCWS_FCMD_TASK, "FCWS_MODE: POSITION mode", 0U, 0U);
        } else {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "Something wrong of WS_FCWS_MODE, mode = %d", FcwsMode, 0U);
        }


        RfFcCfg.WsFcwsCfg.Clock = 12288U;
        RfFcCfg.WsFcwsCfg.FCWSSpec.ActiveSpeed = (DOUBLE)SVC_FC_WARNING_LOWER_BOUND_V2;
        RfFcCfg.WsFcwsCfg.FCWSSpec.PrewarnToColwarnRatio = 1.25; //SVC_FC_TTC_M_V2*PrewarnToColwarnRatio => Yellow alare upper bound

        /* Save FocalLength and cell size into pref */
        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcUserPref_Get() failed!!", 0U, 0U);
        }

        FocallengthUnitCellSize = (pSvcUserPref->CalibFocalLength/pSvcUserPref->CalibCellSize);
        SVC_WRAP_PRINT "SvcFcTaskV2_Init() FocalLength = %5f (mm), cell_size = %5f, Focallength(Unit: CellSize) = %7.3f"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_OK
        SVC_PRN_ARG_DOUBLE  pSvcUserPref->CalibFocalLength
        SVC_PRN_ARG_DOUBLE  pSvcUserPref->CalibCellSize
        SVC_PRN_ARG_DOUBLE  FocallengthUnitCellSize
        SVC_PRN_ARG_E

        RfFcCfg.FocalLength.X = FocallengthUnitCellSize;
        RfFcCfg.FocalLength.Y = FocallengthUnitCellSize;
        RfFcCfg.NNAlgoSelect  = pSvcUserPref->FcV2_SR_mode;
        g_FcV2_SR_mode = pSvcUserPref->FcV2_SR_mode;
        ArmLog_OK(SVC_LOG_FCWS_FCMD_TASK, "g_FcV2_SR_mode = %d", g_FcV2_SR_mode, 0U);

        //Vertical mode need to do this change
        //-> Other data is setup under RefFlowFC_GetDefaultCfgV2() by default
        if (g_FcV2_SR_mode == RF_FC_SR_MODE_VER) {
            RfFcCfg.MaxImprovNum.SegPerson0Num = FC_IN_DATA_V2_3D_MAX;
            RfFcCfg.MaxImprovNum.SegScooter0Num = FC_IN_DATA_V2_3D_MAX;
            RfFcCfg.MaxImprovNum.ThreeDScooter0Num = FC_IN_DATA_V2_3D_MAX;
            RfFcCfg.MaxImprovNum.SegObjNum = FC_IN_DATA_V2_3D_MAX;
            RfFcCfg.MaxImprovNum.SegPerson0Num = FC_IN_DATA_V2_3D_MAX;
            RfFcCfg.MaxImprovNum.SegScooter0Num = FC_IN_DATA_V2_3D_MAX;
            RfFcCfg.MaxImprovNum.SegVehicle0Num = FC_IN_DATA_V2_3D_MAX;
            RfFcCfg.MaxImprovNum.ThreeDObjNum = FC_IN_DATA_V2_3D_MAX;
            RfFcCfg.MaxImprovNum.ThreeDPerson0Num = 0U;
            RfFcCfg.MaxImprovNum.ThreeDScooter0Num = FC_IN_DATA_V2_3D_MAX;
            RfFcCfg.MaxImprovNum.ThreeDVehicle0Num = FC_IN_DATA_V2_3D_MAX;

        } else {//RF_FC_SR_MODE_HOR
            RfFcCfg.MaxImprovNum.SegPerson0Num = 0U;
            RfFcCfg.MaxImprovNum.SegScooter0Num = 0U;
            RfFcCfg.MaxImprovNum.ThreeDScooter0Num = 0U;
            RfFcCfg.MaxImprovNum.SegObjNum = 32U;
            RfFcCfg.MaxImprovNum.SegPerson0Num = 0U;
            RfFcCfg.MaxImprovNum.SegScooter0Num = 0U;
            RfFcCfg.MaxImprovNum.SegVehicle0Num = 32U;
            RfFcCfg.MaxImprovNum.ThreeDObjNum = 32U;
            RfFcCfg.MaxImprovNum.ThreeDPerson0Num = 0U;
            RfFcCfg.MaxImprovNum.ThreeDScooter0Num = 0U;
            RfFcCfg.MaxImprovNum.ThreeDVehicle0Num = 32U;
        }
        /* ACC purpose */
        RfFcCfg.ApAccCfg.ACCSpec.ActiveSpeed = FC_ACC_ACTIVESPEED;
        RfFcCfg.ApAccCfg.ACCSpec.MaxAcceleration = FC_MAX_ACCELERATION;
        RfFcCfg.ApAccCfg.ACCSpec.MinAcceleration = FC_MIN_ACCELERATION;
        RfFcCfg.ApAccCfg.SetSpeed = 100.0;
        RfFcCfg.ApAccCfg.TimeGap = 1.7;

        SVC_WRAP_PRINT "[ACC] ActiveSpeed = %3.1f, MaxAcceleration = %3.1f, MinAcceleration = %3.1f, SetSpeed = %5.1f, TimeGap= %3.1f"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_OK
        SVC_PRN_ARG_DOUBLE  RfFcCfg.ApAccCfg.ACCSpec.ActiveSpeed
        SVC_PRN_ARG_DOUBLE  RfFcCfg.ApAccCfg.ACCSpec.MaxAcceleration
        SVC_PRN_ARG_DOUBLE  RfFcCfg.ApAccCfg.ACCSpec.MinAcceleration
        SVC_PRN_ARG_DOUBLE  RfFcCfg.ApAccCfg.SetSpeed
        SVC_PRN_ARG_DOUBLE  RfFcCfg.ApAccCfg.TimeGap
        SVC_PRN_ARG_E


        RfFcCfg.ApLcsCfg.Clock = 12288U;
        RfFcCfg.ApLcsCfg.OdRoi.Width = RfFcCfg.ODRoi.Width;
        RfFcCfg.ApLcsCfg.OdRoi.Height = RfFcCfg.ODRoi.Height;
        RfFcCfg.ApLcsCfg.OdRoi.StartX = RfFcCfg.ODRoi.StartX;
        RfFcCfg.ApLcsCfg.OdRoi.StartY = RfFcCfg.ODRoi.StartY ;

        RfFcCfg.ApLcsCfg.LineColorIdx.White = 1U;
        RfFcCfg.ApLcsCfg.LineColorIdx.Yellow = 2U;
        RfFcCfg.ApLcsCfg.LineColorIdx.Red = 3U;
        RfFcCfg.ApLcsCfg.LineColorIdx.Blue = 4U;
        RfFcCfg.ApLcsCfg.LineTypeIdx.Solid = 1U;
        RfFcCfg.ApLcsCfg.LineTypeIdx.Dash = 2U;
        RfFcCfg.ApLcsCfg.LineTypeIdx.DoubleSolid = 3U;
        RfFcCfg.ApLcsCfg.LineTypeIdx.DashDouble = 4U;
        RfFcCfg.ApLcsCfg.IntervensionMode = AMBA_LCS_MODE_SEMIAUTOMATIC;
//        RfFcCfg.ApLcsCfg.SteerCfg.ActiveSpeed = 0U;
//        RfFcCfg.ApLcsCfg.AccelerationCfg.ActiveSpeed = 18.0;
//        RfFcCfg.ApLcsCfg.AccelerationCfg.MaxAcceleration = 2.0;
//        RfFcCfg.ApLcsCfg.AccelerationCfg.MinAcceleration = -3.5;
//        RfFcCfg.ApLcsCfg.AccelerationCfg.SetSpeed = 100.0;
//        RfFcCfg.ApLcsCfg.AccelerationCfg.TimeGap = 1.7;

        AmbaMisra_TypeCast(&WrkBufAddr, &(CacheBase));
        RetVal = RefFlowFC_InitV2(&RfFcCfg, WrkBufAddr);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "RefFlowFC_Init NG %d", __LINE__, 0U);
        } else {
            //Do nothing
        }

        /* Draw Horizontal Line */
        {
            static UINT32 OsdBufWidth, OsdBufHeight;

#if defined(CONFIG_ICAM_REBEL_USAGE)
            HorizontalInfo.Channel = VOUT_IDX_A;
#else
            HorizontalInfo.Channel = VOUT_IDX_B;
#endif
            RetVal = SvcOsd_GetOsdBufSize(HorizontalInfo.Channel, &OsdBufWidth, &OsdBufHeight);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcTaskV2_Init() SvcOsd_GetOsdBufSize return %u", RetVal, 0U);
            }

            RetVal = GetHorizontalLine(&(g_CalibData->AdasCfgV1), PntX, PntY);
            if (RetVal == 0U) {
                for (UINT32 i = 0U; i < 2U; i++) {
                    PntX[i] = (PntX[i] - g_CalibData->AdasCfgV1.VoutArea.StartX) * OsdBufWidth / g_CalibData->AdasCfgV1.VoutArea.Width ;
                    PntY[i] = (PntY[i] - g_CalibData->AdasCfgV1.VoutArea.StartY) * OsdBufHeight / g_CalibData->AdasCfgV1.VoutArea.Height;
                }
                HorizontalInfo.X1 = ((OsdBufWidth  - PntX[0]) > HorizontalLine_Thickness)?(PntX[0] + HorizontalLine_Thickness):(PntX[0] - HorizontalLine_Thickness);
                HorizontalInfo.X2 = ((OsdBufWidth  - PntX[1]) > HorizontalLine_Thickness)?(PntX[1]):(PntX[1] - HorizontalLine_Thickness);
                HorizontalInfo.Y1 = ((OsdBufHeight - PntY[0]) > HorizontalLine_Thickness)?(PntY[0]):(PntY[0] - HorizontalLine_Thickness);
                HorizontalInfo.Y2 = ((OsdBufHeight - PntY[1]) > HorizontalLine_Thickness)?(PntY[1]):(PntY[1] - HorizontalLine_Thickness);
                HorizontalInfo.Color = 255U;
                HorizontalInfo.Thickness = HorizontalLine_Thickness;
                HorizontalInfo.WarningLevel = 0U;
                RetVal = SvcAdasNotify_HorizontalLineUpdate(&HorizontalInfo);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[FC][V2] SvcAdasNotify_HorizontalLineUpdate() Error!! %d", __LINE__, 0U);
                }
            }

            /* Calib Horizontal Line(Y) update */
            RetVal = SvcAdasNotify_SetCalibHorizontal(0U, ((HorizontalInfo.Y1 + HorizontalInfo.Y2)/2U));
            AmbaMisra_TouchUnused(&RetVal);
        }
    } else {
        /* Calib Horizontal Line(Y) update */
        RetVal = SvcAdasNotify_SetCalibHorizontal(0U, 0U);
        AmbaMisra_TouchUnused(&RetVal);
    }

    /* create msg queue */
    if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcFc2stageQueId,
                                         SvcFc2stageName,
                                         sizeof(SVC_FC_PROC_2Stage_INFO_s),
                                         SvcFc2stageQue,
                                         SVC_FCWS_FCMD_QUEUE_SIZE * sizeof(SVC_FC_PROC_2Stage_INFO_s))) {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcwsFcmdTask_Init: Fail to create SvcFc2stageQueId msg queue", 0U, 0U);
    } else {
        //Do nothing
    }

    if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcFcProcV2QueId,
                                         SvcFcQueueProcV2Name,
                                         sizeof(SVC_FC_PROC_IN_DATA_V2_s),
                                         SvcFcQueProcV2,
                                         SVC_FCWS_FCMD_QUEUE_SIZE * sizeof(SVC_FC_PROC_IN_DATA_V2_s))) {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcwsFcmdTask_Init: Fail to create SvcFcProcV2QueId msg queue", 0U, 0U);
    } else {
        //Do nothing
    }

    if (SVC_OK != AmbaKAL_SemaphoreCreate(&SvcFcProcV2Sem, NULL, 1U)){
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcwsFcmdTask_Init: Fail to create SvcFcProcV2Sem", 0U, 0U);
    }
    if (SVC_OK != AmbaKAL_SemaphoreCreate(&SvcFc2stageSem, NULL, 1U)){
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcwsFcmdTask_Init: Fail to create SvcFc2stageSem", 0U, 0U);
    }


    /* Create FC task */
    SvcFcTaskCtrl.Priority   = 53;
    SvcFcTaskCtrl.EntryFunc  = SvcFcTaskEntryV2;
    SvcFcTaskCtrl.pStackBase = SvcFcTaskStack;
    SvcFcTaskCtrl.StackSize  = SVC_FCWS_FCMD_TASK_STACK_SIZE;
    SvcFcTaskCtrl.CpuBits    = 0x1U;
    SvcFcTaskCtrl.EntryArg   = 0x0U;

    RetVal = SvcTask_Create("SvcFcTaskV2", &SvcFcTaskCtrl);
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
                    RetVal |= SvcCvFlow_Register(i, DetectionResultCallback, &CvFlowRegisterID);
                    RetVal |= SvcCvFlow_StatusEventRegister(i, PerceptionStatusCallback, &CvFlowRegisterID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "SvcFcwsFcmdTask_Init err. SvcCvFlow_Register/StatusEventRegister failed %d", RetVal, 0U);
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

        RetVal = FCWS_GetWarnZonePnts_TtcBase(&(g_CalibData->AdasCfgV1), &RfFcCfg.WsFcwsCfg, NULL, Zone1X, Zone1Y);
        SvcAdasNotify_UpdateFCInitInfo(&RfFcCfg.WsFcwsCfg);
        AmbaMisra_TouchUnused(&RetVal);
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
UINT32 SvcFcTaskV2_Start(void)
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
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[V2] SvcFcTaskV2_Start() Abort ADAS-FCWS/FCMD enable due to calib data not in NAND", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Start warning icon display
 */
UINT32 SvcFcTaskV2_Stop(void)
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
void SvcFcTaskV2_WarningIcanEnable(void)
{
    UINT32 Rval = 0U;

    AmbaMisra_TouchUnused(&Rval);
    if (FcCalibFromNAND == 1U) {
        if ((FcInit == 1U) && (FcOnOff == 1U)) {
            Rval  = SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1_2x, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2_2x, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD_2x, SVC_WARN_ICON_FLG_ENA | SVC_WANR_ICON_FLG_ANI_ENA);
            Rval |= FcmdWarningContinuousCheck(1U, AMBA_WS_FCMD_EVENT_NO_WARNING);
            AmbaMisra_TouchUnused(&Rval);
            WarningIconOnOff = 1U;
        } else {
            SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "FC module not init yet(%d) or FC disable right now(%d). Cannot start", FcInit, FcOnOff);
        }
    } else {
        SvcLog_NG(SVC_LOG_FCWS_FCMD_TASK, "[V2] SvcFcTaskV2_WarningIcanEnable() Abort ADAS-FCWS/FCMD enable due to calib data not in NAND", 0U, 0U);
    }
}

/**
 *  Stop FC module
 */
void SvcFcTaskV2_WarningIcanDisable(void)
{
    UINT32 Rval = 0U;

    AmbaMisra_TouchUnused(&Rval);
    if (FcInit == 1U) {
        WarningIconOnOff = 0U;
        Rval  = SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1, SVC_WARN_ICON_FLG_ENA);
        Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R1_2x, SVC_WARN_ICON_FLG_ENA);
        Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2, SVC_WARN_ICON_FLG_ENA);
        Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCWS_R2_2x, SVC_WARN_ICON_FLG_ENA);
        Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD, SVC_WARN_ICON_FLG_ENA);
        Rval |= SvcWarnIconTask_Update(SVC_WARN_ICON_FCMD_2x, SVC_WARN_ICON_FLG_ENA);
        AmbaMisra_TouchUnused(&Rval);
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
UINT32 SvcFcTaskV2_GetStatus(UINT32 *pEnable)
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
