/**
 *  @file SvcRmgTask.c
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
 *  @details svc RMG (Rear Mirror Guard) task
 *
 */

#include "AmbaTypes.h"

/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"

/* comsvc */
#include "AmbaShell.h"
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaCalib_AVMIF.h"
#include "RefFlow_RMG.h"

/* core */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcResCfg.h"
#include "SvcGui.h"
#include "SvcOsd.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcFlowControl.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcCan.h"
#include "SvcCalibAdas.h"
#include "SvcDataGather.h"

/* svc-icam */
#include "SvcRmgTask.h"
#include "SvcBufMap.h"
#include "SvcODDrawTask.h"
#include "SvcResCfgTask.h"
#include "SvcCvAppDef.h"
#include "SvcLogoDraw.h"

#define SVC_LOG_RMG_TASK     "RMG_TASK"

#define RMG_GUI_LEVEL (7U)

static UINT32 RmgCvFlowRegisterID[SVC_CV_FLOW_CHAN_MAX] = {0};
static void RmgTask_RefODResult(UINT32 Chan, UINT32 CvType, void *pOutput);
static void RmgTask_DetectionResultHandler(UINT32 Chan, void *pDetResMsg);

static void RmgTask_OsdDraw(UINT32 VoutIdx, UINT32 Level);
static void RmgTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);

static void RmgTask_CalculateHorizon(const AMBA_CAL_EM_CALC_COORD_CFG_s *pCoorCfg);
static void RmgTask_CalculateZone(const AMBA_WS_RMG_CONFIG_s* pRMGCfg, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCoorCfg);

static REF_FLOW_RMG_CFG_s RefRmgCfg;
static AMBA_CAL_EM_CALC_COORD_CFG_s CoorCfg;

static UINT16 RmgCatOther = RmgCatNone;
static UINT32 RmgOsdUpdate = 0U;
static UINT32 OsdWidth, OsdHeight;
static UINT32 DefaultActOffsetY;
static INT32  DrawHorizon[2];
static INT32  DrawZoneF[4][2];
static INT32  DrawZoneN[4][2];

static AMBA_OD_2DBBX_LIST_s    BbxOut GNU_SECTION_NOZEROINIT;
static AMBA_SR_SROBJECT_DATA_s SROut  GNU_SECTION_NOZEROINIT;

static UINT32 RmgMode = RmgModeLiveview;

static UINT32 GetLogData(RF_LOG_HEADER_s *pLogHeader)
{
    UINT32 DataFmt = pLogHeader->DataFmt;
    UINT32 DataSize = pLogHeader->DataSize;
    const void* pDataAddr = pLogHeader->pDataAddr;

    AmbaMisra_TouchUnused(pLogHeader);

    if (DataFmt == RF_RMG_DFMT_2DBBX_STBL_OUT) {    /* stablized bbx */
        const AMBA_OD_2DBBX_LIST_s *pAddr; ;

        AmbaMisra_TypeCast(&pAddr, &pDataAddr);
        if (SVC_OK != AmbaWrap_memcpy(&BbxOut, pAddr, DataSize)) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "AmbaWrap_memcpy BbxOut failed", 0U, 0U);
        }
    }

    if (DataFmt == RF_RMG_DFMT_SR) {
        const AMBA_SR_SROBJECT_DATA_s *pAddr ;

        AmbaMisra_TypeCast(&pAddr, &pDataAddr);
        if (SVC_OK != AmbaWrap_memcpy(&SROut, pAddr, DataSize)) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "AmbaWrap_memcpy SROut failed", 0U, 0U);
        }
    }

    return SVC_OK;
}

/**
 * Initialize Rmg Task
 * @return 0-OK 1-NG
 */
UINT32 SvcRmgTask_Init(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if ((pResCfg->UserFlag & SVC_EMR_RMG) > 0U) {
        static UINT32  RmgWorkBufSize;
        static ULONG   RmgWorkBuf;
        static UINT8 *pRmgWorkBuf;
        static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;

        SvcLog_DBG(SVC_LOG_RMG_TASK, "SvcRmgTask_Init starts", 0U, 0U);

        if (pResCfg->DispStrm[0].VoutID == VOUT_IDX_A) {
            RmgMode = RmgModeLiveview;
        } else {
            RmgMode = RmgModeDecode;
        }

        RetVal |= AmbaWrap_memset(&BbxOut, 0, sizeof(BbxOut));
        RetVal |= AmbaWrap_memset(&SROut, 0, sizeof(SROut));
        RetVal |= AmbaWrap_memset(&RefRmgCfg, 0, sizeof(RefRmgCfg));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "AmbaWrap_memset BbxOut/SROut/RefRmgCfg failed", 0U, 0U);
        }

        RetVal = RefFlowRMG_GetDefaultCfg(&RefRmgCfg);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "RefFlowRMG_GetDefaultCfg failed", 0U, 0U);
        }

        /* prepare working buffer */
        RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_RMG_WRK, &RmgWorkBuf, &RmgWorkBufSize);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "SvcBuffer_Request failed", 0U, 0U);
        }
        AmbaMisra_TypeCast(&pRmgWorkBuf, &RmgWorkBuf);
        RefRmgCfg.pWrkBufAddr = pRmgWorkBuf;

        /* prepare calibration data */
        CoorCfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
        RetVal = SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_BACK, &CoorCfg);
        if (SVC_OK == RetVal) {
            RetVal = SvcOsd_GetOsdBufSize(pResCfg->DispStrm[0].VoutID, &OsdWidth, &OsdHeight);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_RMG_TASK, "SvcOsd_GetOsdBufSize failed", 0U, 0U);
            }

            DefaultActOffsetY = pResCfg->FovCfg[0].ActWin.OffsetY;
            RefRmgCfg.CalibCfg = CoorCfg;
            RmgTask_CalculateHorizon(&CoorCfg);
            RmgTask_CalculateZone(&RefRmgCfg.RMGCfg, &CoorCfg);

            SvcGui_Register(pResCfg->DispStrm[0].VoutID, RMG_GUI_LEVEL, "RmgTask", RmgTask_OsdDraw, RmgTask_OsdUpdate);
        } else {
            SvcLog_NG(SVC_LOG_RMG_TASK, "No calibration data! Use \"svc_app emr_cal_load rear\" to load script in SD card.", 0U, 0U);
        }

        RefRmgCfg.NNCat2ObjType[0].NNCat   = 0U;
        RefRmgCfg.NNCat2ObjType[0].ObjType = SR_OBJ_TYPE_PERSON_0;

        RefRmgCfg.NNCat2ObjType[1].NNCat   = 2U;
        RefRmgCfg.NNCat2ObjType[1].ObjType = SR_OBJ_TYPE_SCOOTER_0;
        RefRmgCfg.NNCat2ObjType[2].NNCat   = 3U;
        RefRmgCfg.NNCat2ObjType[2].ObjType = SR_OBJ_TYPE_SCOOTER_0;

        RefRmgCfg.NNCat2ObjType[3].NNCat   = 4U;
        RefRmgCfg.NNCat2ObjType[3].ObjType = SR_OBJ_TYPE_VEHICLE_0;
        RefRmgCfg.NNCat2ObjType[4].NNCat   = 5U;
        RefRmgCfg.NNCat2ObjType[4].ObjType = SR_OBJ_TYPE_VEHICLE_0;

        RefRmgCfg.NNCatNum = 5U;

        RefRmgCfg.LogFp = GetLogData;
        RefRmgCfg.RMGCfg.ApproachLevelThres[0] = 10000.0;
        RefRmgCfg.RMGCfg.ApproachLevelThres[1] =  5000.0;
        RefRmgCfg.RMGCfg.ApproachLevelThres[2] =  2500.0;
        RefRmgCfg.RMGCfg.ApproachLevelThres[3] =  1000.0;
        RefRmgCfg.RMGCfg.ReversingZoneSize[0]  = 10500U;
        RefRmgCfg.RMGCfg.ReversingZoneSize[1]  = 20000U;

        RetVal = RefFlowRMG_Init(&RefRmgCfg);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "RefFlowRMG_Init failed", 0U, 0U);
        }

        /* draw logo */
        if (RmgMode == RmgModeLiveview) {
            SvcLogoDraw_Init(VOUT_IDX_A);
            SvcLogoDraw_Update();
        }

        /* prepare osd color lookup table */
        SvcOsd_SetClut(29U, 0x20BFFF00U);   /* yellow */
        SvcOsd_SetClut(30U, 0x40BFFF00U);
        SvcOsd_SetClut(31U, 0x60BFFF00U);
        SvcOsd_SetClut(32U, 0x80BFFF00U);
        SvcOsd_SetClut(33U, 0x20FF80FFU);   /* purple */
        SvcOsd_SetClut(34U, 0x40FF80FFU);
        SvcOsd_SetClut(35U, 0x60FF80FFU);
        SvcOsd_SetClut(36U, 0x80FF80FFU);

        SvcLog_DBG(SVC_LOG_RMG_TASK, "SvcRmgTask_Init ends", 0U, 0U);
    }

    return RetVal;
}

/**
 * Start Rmg Task
 * @return 0-OK 1-NG
 */
UINT32 SvcRmgTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if ((pResCfg->UserFlag & SVC_EMR_RMG) > 0U) {
        UINT32 i;

        UINT32 CvFlowNum  = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        SvcLog_DBG(SVC_LOG_RMG_TASK, "SvcRmgTask_Start starts", 0U, 0U);

        RetVal = SvcODDrawTask_Enable(0U);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "SvcODDrawTask_Enable(0) failed", 0U, 0U);
        }

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    RetVal = SvcCvFlow_Register(i, RmgTask_RefODResult, &RmgCvFlowRegisterID[i]);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_RMG_TASK, "SvcCvFlow_Register failed", 0U, 0U);
                    }
                }
            }
        }

        SvcLog_DBG(SVC_LOG_RMG_TASK, "SvcRmgTask_Start ends", 0U, 0U);
    }

    return RetVal;
}

/**
 * Stop Rmg Task
 * @return 0-OK 1-NG
 */
UINT32 SvcRmgTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if ((pResCfg->UserFlag & SVC_EMR_RMG) > 0U) {
        UINT32 i;

        UINT32 CvFlowNum  = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        RetVal = SvcODDrawTask_Enable(1U);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "SvcODDrawTask_Enable(1) failed", 0U, 0U);
        }

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    RetVal = SvcCvFlow_Unregister(i, RmgCvFlowRegisterID[i]);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_RMG_TASK, "SvcCvFlow_Unregister failed %d", RetVal, 0U);
                    }
                }
            }
        }
    }

    return RetVal;
}

static void RmgTask_DetectionResultHandler(UINT32 Chan, void *pDetResMsg)
{
    UINT32 RetVal;
    const  SVC_CV_DETRES_BBX_LIST_s *pList;
    static REF_FLOW_RMG_INPUT_DATA_s  RmgIn  GNU_SECTION_NOZEROINIT;
    static REF_FLOW_RMG_OUTPUT_DATA_s RmgOut GNU_SECTION_NOZEROINIT;
    static AMBA_SR_CANBUS_RAW_DATA_s  RmgCanbusRawData GNU_SECTION_NOZEROINIT;
    static SVC_RMG_DRAW_INFO_s        RmgDrawInfo GNU_SECTION_NOZEROINIT;
    static AMBA_OD_2DBBX_LIST_s       RmgDrawList GNU_SECTION_NOZEROINIT;

    (void) pDetResMsg;
    AmbaMisra_TypeCast(&pList, &pDetResMsg);

    /* prepare bbx list */
    RmgIn.BbxList.MsgCode = pList->MsgCode;
    RmgIn.BbxList.CaptureTime = pList->CaptureTime;
    RmgIn.BbxList.FrameNum = pList->FrameNum;
    RmgIn.BbxList.NumBbx = pList->BbxAmount;
    if (SVC_OK != AmbaWrap_memcpy(RmgIn.BbxList.Bbx, pList->Bbx, SVC_CV_DETRES_MAX_BBX_NUM * sizeof(SVC_CV_DETRES_BBX_WIN_s))) {
        SvcLog_NG(SVC_LOG_RMG_TASK, "AmbaWrap_memcpy RmgIn.BbxList.Bbx failed", 0U, 0U);
    }

    /* prepare canbus data */
    SvcCan_GetRawData(&RmgCanbusRawData);
    if (RmgCanbusRawData.FlagValidRawData == 0U) {
        if (RmgMode == RmgModeDecode) {
            RetVal = SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)pList->FrameNum, &RmgIn.CanbusTrData);
            if (SVC_OK == RetVal) {
                RmgIn.BbxList.CaptureTime = RmgIn.CanbusTrData.CapTS;
            }
        } else {
            RmgIn.CanbusTrData.FlagValidTransferData = 0U;
            RmgIn.CanbusTrData.CapTS = pList->CaptureTime;
        }
    } else {
        RetVal = SvcDataGather_GetCanData(TICK_TYPE_AUDIO, (UINT64)pList->CaptureTime, &RmgIn.CanbusTrData);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "SvcDataGather_GetCanData failed", 0U, 0U);
        }
    }

    RetVal = RefFlowRMG_Process(&RmgIn, &RmgOut);
    if (RetVal != ADAS_ERR_NONE) {
        // SvcLog_NG(SVC_LOG_RMG_TASK, "RefFlowRMG_Process failed", 0U, 0U);
    }

    {
        UINT32 BbxIdx;
        if (SVC_OK != AmbaWrap_memcpy(&RmgDrawList, &BbxOut, sizeof(AMBA_OD_2DBBX_LIST_s))) {
            SvcLog_NG(SVC_LOG_RMG_TASK, "AmbaWrap_memcpy RmgDrawList failed", 0U, 0U);
        }
        for (BbxIdx = 0U; BbxIdx < RmgDrawList.NumBbx; BbxIdx++) {
            RmgDrawList.Bbx[BbxIdx].Cat = RmgCatOther;
        }
    }

    {
        UINT32 ObjIdx, BbxIdx;

        for (ObjIdx = 0U; ObjIdx < RmgOut.RMGOut.ObjNum; ObjIdx++) {
            for (BbxIdx = 0U; BbxIdx < BbxOut.NumBbx; BbxIdx++) {
                if (RmgOut.RMGOut.ObjData[ObjIdx].Id == BbxOut.Bbx[BbxIdx].ObjectId) {
                    if ((BbxOut.Bbx[BbxIdx].Cat == 0U) && (RmgCanbusRawData.CANBusGearStatus != AMBA_SR_CANBUS_GEAR_R)) { /* refer to SvcCvFlow_AmbaOD.c for Cat id */
                        RmgDrawList.Bbx[BbxIdx].Cat = RmgCatOther;
                    } else {
                        if (RmgOut.RMGOut.ObjData[ObjIdx].LaneLocation != AMBA_WS_RMG_LOC_UNDEFINED) {
                            if (RmgOut.RMGOut.ObjData[ObjIdx].Direction == AMBA_WS_RMG_DIR_SAME) {
                                if (RmgOut.RMGOut.ObjData[ObjIdx].ApproachLevel > 0U) {
                                    if ((RmgOut.RMGOut.ObjData[ObjIdx].LaneLocation & 0xF0U) == AMBA_WS_RMG_LOC_EGO) {
                                        RmgDrawList.Bbx[BbxIdx].Cat = (UINT16) (RmgCatEgo + RmgOut.RMGOut.ObjData[ObjIdx].ApproachLevel);
                                    } else if ((RmgOut.RMGOut.ObjData[ObjIdx].LaneLocation & 0xF0U) == AMBA_WS_RMG_LOC_LEFT0) {
                                        RmgDrawList.Bbx[BbxIdx].Cat = (UINT16) (RmgCatSide + RmgOut.RMGOut.ObjData[ObjIdx].ApproachLevel);
                                    } else if ((RmgOut.RMGOut.ObjData[ObjIdx].LaneLocation & 0xF0U) == AMBA_WS_RMG_LOC_RIGHT0) {
                                        RmgDrawList.Bbx[BbxIdx].Cat = (UINT16) (RmgCatSide + RmgOut.RMGOut.ObjData[ObjIdx].ApproachLevel);
                                    } else if ((RmgOut.RMGOut.ObjData[ObjIdx].LaneLocation & 0xF0U) == AMBA_WS_RMG_LOC_REVERSE) {
                                        RmgDrawList.Bbx[BbxIdx].Cat = (UINT16) (RmgCatEgo + RmgOut.RMGOut.ObjData[ObjIdx].ApproachLevel);
                                    } else {
                                        RmgDrawList.Bbx[BbxIdx].Cat = RmgCatOther;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    RmgDrawInfo.Chan       = Chan;
    RmgDrawInfo.Class_name = pList->class_name;
    RmgDrawInfo.FovIdx     = pList->Source;
    RmgDrawInfo.pBbx       = &RmgDrawList;
    RmgDrawInfo.pSRData    = &SROut;
    RetVal = SvcODDrawTask_Msg(SVC_RMG_DRAW, &RmgDrawInfo);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_RMG_TASK, "SvcODDrawTask_Msg failed", 0U, 0U);
    }

    RmgOsdUpdate = 1U;
}

static void RmgTask_RefODResult(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    AmbaMisra_TouchUnused(&CvType);

    RmgTask_DetectionResultHandler(Chan, pOutput);
}

static void RmgTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (RmgOsdUpdate == 1U) {
        RmgOsdUpdate = 0U;
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static void RmgTask_OsdDraw(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    RmgTask_CalculateHorizon(&CoorCfg);
    RmgTask_CalculateZone(&RefRmgCfg.RMGCfg, &CoorCfg);

    if ((DrawHorizon[0] >= 0) && (DrawHorizon[0] < (INT32) OsdWidth) && (DrawHorizon[1] >= 0) && (DrawHorizon[1] < (INT32) OsdHeight)) {
        /* draw horizon */
        RetVal |= SvcOsd_DrawLine(pResCfg->DispStrm[0].VoutID, 0U, (UINT32) DrawHorizon[1], (OsdWidth - 1U), (UINT32) DrawHorizon[0], 1U, RmgColorZone);
    }

    if ((DrawZoneN[0][0] >= 0) && (DrawZoneN[0][0] < (INT32) OsdWidth)  && (DrawZoneN[1][0] >= 0) && (DrawZoneN[1][0] < (INT32) OsdWidth)  &&
        (DrawZoneN[2][0] >= 0) && (DrawZoneN[2][0] < (INT32) OsdWidth)  && (DrawZoneN[3][0] >= 0) && (DrawZoneN[3][0] < (INT32) OsdWidth)  &&
        (DrawZoneN[0][1] >= 0) && (DrawZoneN[0][1] < (INT32) OsdHeight) && (DrawZoneN[1][1] >= 0) && (DrawZoneN[1][1] < (INT32) OsdHeight) &&
        (DrawZoneN[2][1] >= 0) && (DrawZoneN[2][1] < (INT32) OsdHeight) && (DrawZoneN[3][1] >= 0) && (DrawZoneN[3][1] < (INT32) OsdHeight) &&
        (DrawZoneF[0][0] >= 0) && (DrawZoneF[0][0] < (INT32) OsdWidth)  && (DrawZoneF[1][0] >= 0) && (DrawZoneF[1][0] < (INT32) OsdWidth)  &&
        (DrawZoneF[2][0] >= 0) && (DrawZoneF[2][0] < (INT32) OsdWidth)  && (DrawZoneF[3][0] >= 0) && (DrawZoneF[3][0] < (INT32) OsdWidth)  &&
        (DrawZoneF[0][1] >= 0) && (DrawZoneF[0][1] < (INT32) OsdHeight) && (DrawZoneF[1][1] >= 0) && (DrawZoneF[1][1] < (INT32) OsdHeight) &&
        (DrawZoneF[2][1] >= 0) && (DrawZoneF[2][1] < (INT32) OsdHeight) && (DrawZoneF[3][1] >= 0) && (DrawZoneF[3][1] < (INT32) OsdHeight)) {
        /* draw 4 lines for 3 car lanes */
        RetVal |= SvcOsd_DrawLine(pResCfg->DispStrm[0].VoutID, (UINT32) DrawZoneN[0][0], (UINT32) DrawZoneN[0][1], (UINT32) DrawZoneF[0][0], (UINT32) DrawZoneF[0][1], 1U, RmgColorZone);
        RetVal |= SvcOsd_DrawLine(pResCfg->DispStrm[0].VoutID, (UINT32) DrawZoneN[1][0], (UINT32) DrawZoneN[1][1], (UINT32) DrawZoneF[1][0], (UINT32) DrawZoneF[1][1], 1U, RmgColorZone);
        RetVal |= SvcOsd_DrawLine(pResCfg->DispStrm[0].VoutID, (UINT32) DrawZoneN[2][0], (UINT32) DrawZoneN[2][1], (UINT32) DrawZoneF[2][0], (UINT32) DrawZoneF[2][1], 1U, RmgColorZone);
        RetVal |= SvcOsd_DrawLine(pResCfg->DispStrm[0].VoutID, (UINT32) DrawZoneN[3][0], (UINT32) DrawZoneN[3][1], (UINT32) DrawZoneF[3][0], (UINT32) DrawZoneF[3][1], 1U, RmgColorZone);

        /* draw zone distance */
        RetVal |= SvcOsd_DrawLine(pResCfg->DispStrm[0].VoutID, (UINT32) DrawZoneF[0][0], (UINT32) DrawZoneF[0][1], (UINT32) DrawZoneF[3][0], (UINT32) DrawZoneF[3][1], 1U, RmgColorZone);
    }

    if (SVC_OK != RetVal) {
        // SvcLog_NG(SVC_LOG_RMG_TASK, "SvcOsd_DrawLine failed", 0U, 0U);
    }
}

static void RmgTask_CalculateHorizon(const AMBA_CAL_EM_CALC_COORD_CFG_s *pCoorCfg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DrawHorizonPos[2];
    UINT32 MainWidth, MainHeight;
    UINT32 OsdOffsetY;
    INT32  ActOffsetY;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    MainWidth  = pResCfg->FovCfg[0].MainWin.Width;
    MainHeight = pResCfg->FovCfg[0].MainWin.Height;

    RetVal |= AmbaCal_EmFindHorizonPosition(pCoorCfg, 0U, (MainHeight / 2U), (MainHeight / 2U), &DrawHorizonPos[0]);
    RetVal |= AmbaCal_EmFindHorizonPosition(pCoorCfg, MainWidth, (MainHeight / 2U), (MainHeight / 2U), &DrawHorizonPos[1]);
    if (RetVal != CAL_OK) {
        SvcLog_NG(SVC_LOG_RMG_TASK, "AmbaCal_EmFindHorizonPosition failed", 0U, 0U);
    }

    /* transfer from Main to Osd */
    if (RmgMode == RmgModeDecode) {
        OsdOffsetY = ((UINT32) pResCfg->DispStrm[0].StrmCfg.MaxWin.Height - (UINT32) pResCfg->DispStrm[0].StrmCfg.Win.Height) / 2U;
        DrawHorizon[0] = ((INT32) DrawHorizonPos[0] + (INT32) OsdOffsetY) / 2;
        DrawHorizon[1] = ((INT32) DrawHorizonPos[1] + (INT32) OsdOffsetY) / 2;
    } else {
        OsdOffsetY = pResCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetY;
        ActOffsetY = (INT32) DefaultActOffsetY - (INT32) pResCfg->FovCfg[0].ActWin.OffsetY;
        DrawHorizon[0] = ((INT32) DrawHorizonPos[0] - (INT32) OsdOffsetY) + ActOffsetY;
        DrawHorizon[1] = ((INT32) DrawHorizonPos[1] - (INT32) OsdOffsetY) + ActOffsetY;
    }
}

static void RmgTask_CalculateZone(const AMBA_WS_RMG_CONFIG_s* pRMGCfg, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCoorCfg)
{
    UINT32 RetVal;
    INT32  SrcHeight, ShiftX;
    INT32  MainWidth, MainHeight;
    INT32  ActOffsetY, OsdOffsetY;
    UINT32 LaneIdx;
    DOUBLE LaneWidth = (DOUBLE) pRMGCfg->LaneWidth;
    DOUBLE LaneX[4];
    DOUBLE FarY, NearY;
    AMBA_CAL_POINT_DB_3D_s Pnt3D;
    AMBA_CAL_POINT_DB_2D_s Pnt2D;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    SrcHeight = (INT32) pResCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.Height;

    MainWidth  = (INT32) pResCfg->FovCfg[0].MainWin.Width;
    MainHeight = (INT32) pResCfg->FovCfg[0].MainWin.Height;

    LaneX[0] = -1.5 * LaneWidth;
    LaneX[1] = -0.5 * LaneWidth;
    LaneX[2] =  0.5 * LaneWidth;
    LaneX[3] =  1.5 * LaneWidth;

    FarY = (-pRMGCfg->CarSize[1]/2.0) - (DOUBLE)pRMGCfg->ZoneDistance;
    for (LaneIdx = 0; LaneIdx < 4U; LaneIdx++) {
        /* find far point */
        Pnt3D.X = LaneX[LaneIdx];
        Pnt3D.Y = FarY;
        Pnt3D.Z = 0.0;
        RetVal = AmbaCal_EmConvPtWorldToImgPlane(pCoorCfg, &Pnt3D, &Pnt2D);
        DrawZoneF[LaneIdx][0] = (INT32) Pnt2D.X;
        DrawZoneF[LaneIdx][1] = (INT32) Pnt2D.Y;
        if (CAL_OK == RetVal) {
            if ((DrawZoneF[LaneIdx][0] < 0) || (DrawZoneF[LaneIdx][0] > (MainWidth - 1)) || (DrawZoneF[LaneIdx][1] < 0) || (DrawZoneF[LaneIdx][1] > (MainHeight - 1))) {
                RetVal = CAL_ERR_0;
            }
        }

        /* find near point */
        if (CAL_OK == RetVal) {
            NearY = (-pRMGCfg->CarSize[1]/2.0) - 2000.0;
            while (NearY > FarY) {
                Pnt3D.X = LaneX[LaneIdx];
                Pnt3D.Y = NearY;
                Pnt3D.Z = 0.0;
                RetVal = AmbaCal_EmConvPtWorldToImgPlane(pCoorCfg, &Pnt3D, &Pnt2D);
                DrawZoneN[LaneIdx][0] = (INT32) Pnt2D.X;
                DrawZoneN[LaneIdx][1] = (INT32) Pnt2D.Y;
                if (CAL_OK == RetVal) {
                    if ((DrawZoneN[LaneIdx][0] < 0) || (DrawZoneN[LaneIdx][0] > (MainWidth - 1)) || (DrawZoneN[LaneIdx][1] < 0) || (DrawZoneN[LaneIdx][1] > (MainHeight - 1))) {
                        RetVal = CAL_ERR_0;
                    }
                }

                if (CAL_OK == RetVal) {
                    break;
                } else {
                    NearY = NearY - 500.0;
                }
            }
        }

        if (CAL_OK == RetVal) {
            /* transfer from Main to Osd and flip X-axis*/
            if (RmgMode == RmgModeDecode) {
                OsdOffsetY = ((INT32) pResCfg->DispStrm[0].StrmCfg.MaxWin.Height - (INT32) pResCfg->DispStrm[0].StrmCfg.Win.Height) / 2;
                DrawZoneN[LaneIdx][0] = ((MainWidth - 1) - DrawZoneN[LaneIdx][0]) / 2;
                DrawZoneN[LaneIdx][1] = (DrawZoneN[LaneIdx][1] + OsdOffsetY) / 2;
                DrawZoneF[LaneIdx][0] = ((MainWidth - 1) - DrawZoneF[LaneIdx][0]) / 2;
                DrawZoneF[LaneIdx][1] = (DrawZoneF[LaneIdx][1] + OsdOffsetY) / 2;
            } else {
                /*
                    Main: 1920x724, Vout: 1920x480
                    -----------------------------------------
                    |                                       |   122
                    |---------------------------------------|
                    |                                       |
                    |                                       |   480
                    |                                       |
                    |---------------------------------------|
                    |                Zone C                 |   122
                    -----------------------------------------
                */
                OsdOffsetY = (INT32) pResCfg->DispStrm[0].StrmCfg.ChanCfg[0].SrcWin.OffsetY;
                ActOffsetY = (INT32) DefaultActOffsetY - (INT32) pResCfg->FovCfg[0].ActWin.OffsetY;

                /* shift near point if it is in Zone C */
                if (DrawZoneN[LaneIdx][1] >= (SrcHeight + OsdOffsetY)) {
                    INT32 MarginY = (SrcHeight + OsdOffsetY) - 1;
                    INT32 DiffY = DrawZoneN[LaneIdx][1] - MarginY;
                    ShiftX = (DiffY * (DrawZoneF[LaneIdx][0] - DrawZoneN[LaneIdx][0])) / (DrawZoneN[LaneIdx][1] - DrawZoneF[LaneIdx][1]);
                    DrawZoneN[LaneIdx][0] += ShiftX;
                    DrawZoneN[LaneIdx][1] -= DiffY;
                }

                DrawZoneN[LaneIdx][0] = (MainWidth - 1) - DrawZoneN[LaneIdx][0];
                DrawZoneN[LaneIdx][1] = (DrawZoneN[LaneIdx][1] - OsdOffsetY) + ActOffsetY;
                DrawZoneF[LaneIdx][0] = (MainWidth - 1) - DrawZoneF[LaneIdx][0];
                DrawZoneF[LaneIdx][1] = (DrawZoneF[LaneIdx][1] - OsdOffsetY) + ActOffsetY;

                /* tilt */
                ShiftX = (ActOffsetY * (DrawZoneF[LaneIdx][0] - DrawZoneN[LaneIdx][0])) / (DrawZoneN[LaneIdx][1] - DrawZoneF[LaneIdx][1]);
                if (((DrawZoneN[LaneIdx][0] + ShiftX) >= 0) && ((DrawZoneN[LaneIdx][0] + ShiftX) < MainWidth)) {
                    DrawZoneN[LaneIdx][0] += ShiftX;
                    DrawZoneN[LaneIdx][1] -= ActOffsetY;
                }
            }
        }
    }
}

/**
 * Set Rmg Task Debug Bbx
 * @param[in] Level Level of debug bbx
 * @return None
 */
void SvcRmgTask_SetDebugBbx(UINT32 Level)
{
    if (Level != 0U) {
        RmgCatOther = RmgCatDebug;
    } else {
        RmgCatOther = RmgCatNone;
    }
}

/**
 * Control Rmg Task Osd
 * @param[in] Enable enable or disable osd
 * @return None
 */
void SvcRmgTask_OsdEnable(UINT32 Enable)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    static UINT32 OsdStatus = 1U;

    if ((Enable == 1U) && (OsdStatus == 0U)) {
        SvcGui_Register(pResCfg->DispStrm[0].VoutID, RMG_GUI_LEVEL, "RmgTask", RmgTask_OsdDraw, RmgTask_OsdUpdate);
        OsdStatus = 1U;
    } else if ((Enable == 0U) && (OsdStatus == 1U)) {
        SvcGui_Unregister(pResCfg->DispStrm[0].VoutID, RMG_GUI_LEVEL);
        OsdStatus = 0U;
    } else {
        /* do nothing */
    }
}
