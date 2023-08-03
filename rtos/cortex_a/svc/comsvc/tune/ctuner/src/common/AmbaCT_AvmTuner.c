/**
 *  @file AmbaCT_AvmTuner.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */
#include "AmbaTypes.h"
#include "AmbaCT_AvmTuner.h"
#include "AmbaCT_Logging.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCT_AvmTunerIF.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

typedef struct {
    AMBA_CAL_AVM_2D_DATA_s Data2D;
    AMBA_CAL_AVM_3D_DATA_s Data3D;
    AMBA_CAL_AVM_MV_DATA_s DataMv;
    AMBA_CAL_AVM_PRE_CHECK_RESULT_s DataPreCheckResult;
    struct {
        DOUBLE RealTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE ExceptTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE ExceptAngleTable[MAX_LENS_DISTO_TBL_LEN];
    } LensTable[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_AVM_3D_PRE_CHECK_V2_s PreCheckData[MAX_PRE_CHECK_VIEW_NUM];
    struct {
        DOUBLE RealTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE ExpectTable[MAX_LENS_DISTO_TBL_LEN];
    } MvPerspLensTbl[4];
} AMBA_CT_AVM_WORKING_BUF_s;

static UINT32 (*pCTAvmCbMsgReciver)(AMBA_CAL_AVM_MSG_TYPE_e Type, AMBA_CAL_AVM_CAM_ID_e CamId,const AMBA_CAL_AVM_MSG_s *pMsg) = NULL;
static UINT32 (*pAvmCbSaveInternalData)(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_AVM_PRE_CAL_DATA_s *pData) = NULL;
static UINT32 (*pAvmCbFeedInternalData)(AMBA_CAL_AVM_CAM_ID_e CamId, AMBA_CAL_AVM_PRE_CAL_DATA_s *pData) = NULL;
static UINT32 (*p2DAvmCbFeedCustomMat)(AMBA_CAL_AVM_CAM_ID_e CamId, DOUBLE *pMat) = NULL;
static UINT32 (*pAvmCbSavePreCheckData)(AMBA_CAL_AVM_CAM_ID_e CamId, const void *pData, UINT32 SaveLength) = NULL;
static UINT32 (*pAvmCbGetPreCheckData)(const char *pFileName, void *pData, UINT32 DataLength) = NULL;
static UINT32 (*pAvmCbFeedOSDMaskFunc)(UINT32 Width, UINT32 Height, const char *TablePath, UINT8 *pTable) = NULL;
static UINT32 (*pAvmCbSaveCarVout4Points)(const AMBA_CAL_POINT_DB_2D_s CarVout4Points[4U]) = NULL;
static UINT32 (*pAvmCbCalibErrorReport)(AMBA_CAL_AVM_CAM_ID_e CamId, UINT32 CalibPointNum, const DOUBLE ReProjectError[]) = NULL;
static AMBA_CT_AVM_WORKING_BUF_s *pCTAvmWorkingBuf = NULL;
static void *pCTAvmCalibWorkingBuf = NULL;
static SIZE_t CTAvmCalibWorkingBufSize = 0;
static AMBA_CT_AVM_USER_SETTING_s CTUserAvmGroupData GNU_SECTION_NOZEROINIT;
static UINT8 AvmTunerValidGroup[AMBA_CT_AVM_TUNER_MAX];

static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static inline void CT_AvmSetGroupValid(AMBA_CT_AVM_TUNER_GROUP_s GroupId)
{
    if (GroupId < AMBA_CT_AVM_TUNER_MAX) {
        AvmTunerValidGroup[GroupId] = 1;
    }
}

void AmbaCT_AvmGetWorkingBufSize(SIZE_t *pSize)
{
    *pSize = sizeof(AMBA_CT_AVM_WORKING_BUF_s);
}

static inline void CT_AvmResetGData2Default(void)
{
    UINT32 i;
    CT_CheckRval(AmbaWrap_memset(&CTUserAvmGroupData, 0x0, sizeof(CTUserAvmGroupData)), "AmbaWrap_memset", __func__);

    CTUserAvmGroupData.AdvView3D.DisplayRotation = 0U;
    CTUserAvmGroupData.View3D.DisplayRotation = 0U;

    CTUserAvmGroupData.AdvView3D.RotationAngle = 0.0;

    CTUserAvmGroupData.System.EnableSavePrecheck = 0U;

    CTUserAvmGroupData.OsdMask.FromFile = 0U;

    CTUserAvmGroupData.MainViewPerspetCfgV1.ProjectionRotationDegreeH = 0.0;
    CTUserAvmGroupData.MainViewPerspetCfgV1.ProjectionRotationDegreeV = 0.0;
    CTUserAvmGroupData.MainViewPerspetCfgV1.ProjectionRotationDegreeF = 0.0;
    CTUserAvmGroupData.MainViewPerspetCfgV1.TotalViewAngleY = 0.0;

    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectUpper.pRealTbl = pCTAvmWorkingBuf->MvPerspLensTbl[0].RealTable;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectUpper.pExpectTbl = pCTAvmWorkingBuf->MvPerspLensTbl[0].ExpectTable;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectUpper.Length = 0U;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectLower.pRealTbl = pCTAvmWorkingBuf->MvPerspLensTbl[1].RealTable;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectLower.pExpectTbl = pCTAvmWorkingBuf->MvPerspLensTbl[1].ExpectTable;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectLower.Length = 0U;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectLeft.pRealTbl = pCTAvmWorkingBuf->MvPerspLensTbl[2].RealTable;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectLeft.pExpectTbl = pCTAvmWorkingBuf->MvPerspLensTbl[2].ExpectTable;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectLeft.Length = 0U;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectRight.pRealTbl = pCTAvmWorkingBuf->MvPerspLensTbl[3].RealTable;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectRight.pExpectTbl = pCTAvmWorkingBuf->MvPerspLensTbl[3].ExpectTable;
    CTUserAvmGroupData.MainViewPerspetCfgV1.LdeRealExpectRight.Length = 0U;

    CTUserAvmGroupData.CalibSetting.EnableRansac = 1U;
    CTUserAvmGroupData.CalibSetting.IterationNum = 0U;
    CTUserAvmGroupData.CalibSetting.AllowedReprojErr = 0.0;

    CTUserAvmGroupData.MainViewEqualDistCfg.TotalViewAngleY = 0.0;
    for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
        CTUserAvmGroupData.Blend.ReferenceCarVoutPosition[i] = 0U;
        CTUserAvmGroupData.Blend.SectorAngle[i] = 0U;
        CTUserAvmGroupData.MinVout[i].Width = 96U;
        CTUserAvmGroupData.MinVout[i].Height = 32U;
        CTUserAvmGroupData.MinRoiSize[i].Width = 0U;
        CTUserAvmGroupData.MinRoiSize[i].Height = 0U;
        CTUserAvmGroupData.Internal[i].ExtrapolatePlaneBoundary = 1U;
        CTUserAvmGroupData.Internal[i].RefineDmaEnable = 1U;
        CTUserAvmGroupData.Internal[i].RefineWaitlineEnable = 1U;
        CTUserAvmGroupData.Internal[i].RefineOutOfRoiEnable = 1U;
        CTUserAvmGroupData.Internal[i].RefineVFlipEnable = 1U;
        CTUserAvmGroupData.Internal[i].RepeatUnusedArea = 1U;
        CTUserAvmGroupData.Internal[i].WaitlineTh = 0U;
        CTUserAvmGroupData.Internal[i].DmaTh = 0U;
        CTUserAvmGroupData.Internal[i].DebugID = 0U;
        CTUserAvmGroupData.CustomCalib2D[i].Enable = 0U;
        CTUserAvmGroupData.Lens[i].pRealTable = &pCTAvmWorkingBuf->LensTable[i].RealTable[0];
        CTUserAvmGroupData.Lens[i].pExceptTable = &pCTAvmWorkingBuf->LensTable[i].ExceptTable[0];
        CTUserAvmGroupData.Lens[i].pExceptAngleTable = &pCTAvmWorkingBuf->LensTable[i].ExceptAngleTable[0];
    }
    AmbaUtility_StringCopy(CTUserAvmGroupData.View3DModelType.ModelType, sizeof(CTUserAvmGroupData.View3DModelType.ModelType), "SIMPLE\0");
}

UINT32 AmbaCT_AvmTunerInit(const AMBA_CT_AVM_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    if (pInitCfg->WorkingBufSize < sizeof(AMBA_CT_AVM_WORKING_BUF_s)) {
        pNextMsg = CT_LogPutStr(Msg, "Working buffer size ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(pInitCfg->WorkingBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, " incorrect, necessary size: ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(sizeof(AMBA_CT_AVM_WORKING_BUF_s)));
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CT_ERR_1;
    } else {
        if(0U != AmbaWrap_memset(AvmTunerValidGroup, 0x0, sizeof(AvmTunerValidGroup))){
            Rval = CT_ERR_1;
        }
        if(0U != AmbaWrap_memcpy(&pCTAvmWorkingBuf, &pInitCfg->pWorkingBuf, sizeof(AMBA_CT_AVM_WORKING_BUF_s *))) {
            Rval = CT_ERR_1;
        }

        pCTAvmCbMsgReciver = pInitCfg->CbMsgReciver;
        pAvmCbFeedInternalData = pInitCfg->CbFeedPreCalData;
        pAvmCbSaveInternalData = pInitCfg->CbSavePreCalData;
        pAvmCbSavePreCheckData = pInitCfg->CbSavePreCheckData;
        pAvmCbGetPreCheckData = pInitCfg->CbGetPreCheckData;
        p2DAvmCbFeedCustomMat = pInitCfg->Cb2DCustomCalibCalib;
        pAvmCbFeedOSDMaskFunc = pInitCfg->CbFeedOSDMaskFunc;
        pCTAvmCalibWorkingBuf = pInitCfg->pCalibWorkingBuf;
        CTAvmCalibWorkingBufSize = pInitCfg->CalibWorkingBufSize;
        pAvmCbSaveCarVout4Points = pInitCfg->CbSaveCarVout4Points;
        pAvmCbCalibErrorReport = pInitCfg->CbCalibErrorReport;
        CT_AvmResetGData2Default();
    }
    return Rval;
}

void AmbaCT_AvmGetSystem(AMBA_CT_AVM_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetSystem(const AMBA_CT_AVM_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_SYSTEM);
}

void AmbaCT_AvmGetCar(  AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.Car, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetCar(const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.Car, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_CAR);
}

void AmbaCT_AvmGet2dViewRange(AMBA_CT_AVM_2D_VIEW_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.View2D, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSet2dViewRange(const AMBA_CT_AVM_2D_VIEW_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.View2D, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_2D_VIEW);
}

void AmbaCT_AvmGet2dCustomCalib(UINT32 ArrayIndex, AMBA_CT_AVM_2D_CUSTOM_CALIB_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.CustomCalib2D[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSet2dCustomCalib(const UINT32 ArrayIndex, const AMBA_CT_AVM_2D_CUSTOM_CALIB_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.CustomCalib2D[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_2D_CUSTOM_CALIB);
}

void AmbaCT_AvmGet3dViewModelType(AMBA_CT_AVM_3D_VIEW_MODEL_TYPE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.View3DModelType, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSet3dViewModelType(const AMBA_CT_AVM_3D_VIEW_MODEL_TYPE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.View3DModelType, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_3D_VIEW_MODEL_TYPE);
}

void AmbaCT_AvmGet3dView(AMBA_CT_AVM_3D_VIEW_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.View3D, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSet3dView(const AMBA_CT_AVM_3D_VIEW_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.View3D, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_3D_VIEW);
}

void AmbaCT_AvmGet3dAdvView(AMBA_CT_AVM_3D_ADV_VIEW_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.AdvView3D, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSet3dAdvView(const AMBA_CT_AVM_3D_ADV_VIEW_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.AdvView3D, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_3D_ADV_VIEW);
}

void AmbaCT_AvmGet3dWarp(UINT32 ArrayIndex, AMBA_CAL_AVM_3D_WARP_CAL_TYPE_e *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.WarpCalType[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSet3dWarp(const UINT32 ArrayIndex, const AMBA_CAL_AVM_3D_WARP_CAL_TYPE_e *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.WarpCalType[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_3D_WARP);
}
void AmbaCT_AvmGetMainView(UINT8 *pData)
{
    *pData = (UINT8)CTUserAvmGroupData.MainViewType;
}

void AmbaCT_AvmSetMainView(const UINT8 *pData)
{
    static const AMBA_CAL_AVM_VIEW_TYPE_e AVM_TYPE_ENUM_LUT[6] =
    {
       AMBA_CAL_AVM_BIRDVIEW,
       AMBA_CAL_AVM_3DVIEW,
       AMBA_CAL_AVM_MV_PERSPECTIVE,
       AMBA_CAL_AVM_MV_EQUAL_DISTANCE,
       AMBA_CAL_AVM_MV_LDC,
       AMBA_CAL_AVM_MV_FLOOR,
    };

    CTUserAvmGroupData.MainViewType = AVM_TYPE_ENUM_LUT[*pData];
    CT_AvmSetGroupValid(AMBA_CT_AVM_MAIN_VIEW);
}

void AmbaCT_AvmGetMVPerspectiveCfg(AMBA_CAL_AVM_MV_PERSPET_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.MainViewPerspetCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetMVPerspectiveCfg(const AMBA_CAL_AVM_MV_PERSPET_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.MainViewPerspetCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_MAIN_VIEW_PERSPECTIVE);
}

void AmbaCT_AvmGetMVPerspectiveV1Cfg(AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.MainViewPerspetCfgV1, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetMVPerspectiveV1Cfg(const AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.MainViewPerspetCfgV1, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_MAIN_VIEW_PERSPECTIVE);
}


void AmbaCT_AvmGetMVEqualDistCfg(AMBA_CAL_AVM_MV_EQ_DISTANCE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.MainViewEqualDistCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetMVEqualDistCfg(const AMBA_CAL_AVM_MV_EQ_DISTANCE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.MainViewEqualDistCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_MAIN_VIEW_EQUAL_DIST);
}

void AmbaCT_AvmGetMVLdcCfg(AMBA_CAL_AVM_MV_LDC_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.MainViewLdcCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetMVLdcCfg(const AMBA_CAL_AVM_MV_LDC_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.MainViewLdcCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_MAIN_VIEW_LDC);
}

void AmbaCT_AvmGetMVDirection(AMBA_CAL_POINT_DB_3D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.MainViewDirection, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetMVDirection(const AMBA_CAL_POINT_DB_3D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.MainViewDirection, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_MAIN_VIEW_DIRECTION);
}

void AmbaCT_AvmGet3dVirtualCam(AMBA_CAL_VIRTUAL_CAM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.VirtualCam3D, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSet3dVirtualCam(const AMBA_CAL_VIRTUAL_CAM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.VirtualCam3D, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_3D_VITRUAL_CAM);
}

void AmbaCT_AvmGetViewBlendV2(AMBA_CAL_AVM_BLEND_CFG_V2_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.Blend, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetViewBlendV2(const AMBA_CAL_AVM_BLEND_CFG_V2_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.Blend, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_VIEW_BLEND);
}
void AmbaCT_AvmGetOsdMask(AMBA_CT_AVM_OSD_MASK_s **pData)
{
    const AMBA_CT_AVM_OSD_MASK_s *pCTData = &CTUserAvmGroupData.OsdMask;
    CT_CheckRval(AmbaWrap_memcpy(pData, &pCTData, sizeof(AMBA_CT_AVM_OSD_MASK_s *)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetOsdMask(const AMBA_CT_AVM_OSD_MASK_s *pData)
{
    AMBA_CT_AVM_OSD_MASK_s *pCTData = &CTUserAvmGroupData.OsdMask;
    if (pData != pCTData) {
        CT_CheckRval(AmbaWrap_memcpy(pCTData, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    }
    CT_AvmSetGroupValid(AMBA_CT_AVM_OSD_MASK);
}

void AmbaCT_AvmGetLens(UINT32 ArrayIndex, AMBA_CT_AVM_LENS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.Lens[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetLens(const UINT32 ArrayIndex,const AMBA_CT_AVM_LENS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.Lens[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_LENS);
}

void AmbaCT_AvmGetSensor(UINT32 ArrayIndex, AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.Sensor[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetSensor(const UINT32 ArrayIndex,const AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.Sensor[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_SENSOR);
}

void AmbaCT_AvmGetOpticalCenter(UINT32 ArrayIndex, AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.OpticalCenter[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetOpticalCenter(const UINT32 ArrayIndex,const AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.OpticalCenter[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_OPTICAL_CENTER);
}

void AmbaCT_AvmGetCamera(UINT32 ArrayIndex, AMBA_CT_AVM_CAMERA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.Camera[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetCamera(const UINT32 ArrayIndex,const AMBA_CT_AVM_CAMERA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.Camera[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_CAMERA);
}

void AmbaCT_AvmSetVin(const UINT32 ArrayIndex,const AMBA_IK_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.Vin[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmGetVin(UINT32 ArrayIndex, AMBA_IK_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.Vin[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_VIN);
}

void AmbaCT_AvmGetCalibPoints(UINT32 ArrayIndex, AMBA_CT_AVM_CALIB_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.CalibPointMap[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetCalibPoints(const UINT32 ArrayIndex,const AMBA_CT_AVM_CALIB_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.CalibPointMap[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_CALIB_POINTS);
}

void AmbaCT_AvmGetAssistancePoints(UINT32 ArrayIndex, AMBA_CT_AVM_ASSISTANCE_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.AssistancePointMap[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetAssistancePoints(const UINT32 ArrayIndex,const AMBA_CT_AVM_ASSISTANCE_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.AssistancePointMap[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_ASSISTANCE_POINTS);
}

void AmbaCT_AvmGetRoi(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.Roi[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetRoi(const UINT32 ArrayIndex,const AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.Roi[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_ROI);
}

void AmbaCT_AvmGetMainSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.MainSize[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetMainSize(const UINT32 ArrayIndex,const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.MainSize[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_MAIN);
}

void AmbaCT_AvmGetMinVoutSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.MinVout[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetMinVoutSize(const UINT32 ArrayIndex,const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.MinVout[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_MIN_VOUT);
}

void AmbaCT_AvmGetMinRoiSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.MinRoiSize[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetMinRoiSize(const UINT32 ArrayIndex,const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.MinRoiSize[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_MIN_ROI_SIZE);
}

void AmbaCT_AvmGetTileSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.TileSize[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetTileSize(const UINT32 ArrayIndex,const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.TileSize[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_TILE_SIZE);
}

void AmbaCT_AvmGetAuto(UINT32 ArrayIndex, AMBA_CT_AVM_AUTO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.Auto[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetAuto(const UINT32 ArrayIndex,const AMBA_CT_AVM_AUTO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.Auto[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_AUTO);
}

void AmbaCT_AvmGetOptimize(UINT32 ArrayIndex, UINT8 *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.OptimizeLevel[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetOptimize(const UINT32 ArrayIndex,const UINT8 *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.OptimizeLevel[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_OPTIMIZE);
}

void AmbaCT_AvmGetInternal(UINT32 ArrayIndex, AMBA_CAL_AVM_INTERNAL_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.Internal[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetInternal(const UINT32 ArrayIndex,const AMBA_CAL_AVM_INTERNAL_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.Internal[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_INTERNAL);
}

UINT8 AmbaCT_AvmGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_AVM_TUNER_MAX) ? AvmTunerValidGroup[GroupId] : 0U;
}

void AmbaCT_AvmGetPreCheckData(AMBA_CT_AVM_PRE_CHECK_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserAvmGroupData.PreCheckData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetPreCheckData(const AMBA_CT_AVM_PRE_CHECK_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserAvmGroupData.PreCheckData, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_PRECHECKDATA);
}

void AmbaCT_AvmGetCalib(AMBA_CT_AVM_CALIB_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserAvmGroupData.CalibSetting, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_AvmSetCalib(const AMBA_CT_AVM_CALIB_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserAvmGroupData.CalibSetting, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    CT_AvmSetGroupValid(AMBA_CT_AVM_CALIB);
}

static AMBA_CT_AVM_TYPE_e CT_AvmString2Type(const char *pType)
{
    AMBA_CT_AVM_TYPE_e Rval;
    if (0 == CT_strcmp(pType, "2D")) {
        Rval = AMBA_CT_AVM_TYPE_2D;
    } else if (0 == CT_strcmp(pType, "3D")) {
        Rval = AMBA_CT_AVM_TYPE_3D;
    } else if (0 == CT_strcmp(pType, "MV")) {
        Rval = AMBA_CT_AVM_TYPE_MV;
    } else {
        Rval = AMBA_CT_AVM_TYPE_MAX;
    }
    return Rval;
}

static AMBA_CT_AVM_CALIB_MODE_e CT_AvmString2CalibMode(const char *pCalibMode)
{
    AMBA_CT_AVM_CALIB_MODE_e Rval;

    if (0 == CT_strcmp(pCalibMode, "AVM_PRE_CALC")) {
        Rval = AMBA_CT_AVM_CALIB_MODE_PRE_CALC;
    } else if (0 == CT_strcmp(pCalibMode, "AVM_FAST")) {
        Rval = AMBA_CT_AVM_CALIB_MODE_FAST;
    } else if (0 == CT_strcmp(pCalibMode, "AVM_NORMAL")) {
        Rval = AMBA_CT_AVM_CALIB_MODE_NORMAL;
    } else if (0 == CT_strcmp(pCalibMode, "AVM_PRE_CHECK")) {
        Rval = AMBA_CT_AVM_CALIB_MODE_PRE_CHECK;
    } else {
        Rval = AMBA_CT_AVM_CALIB_MODE_NORMAL;
    }
    return Rval;
}

static AMBA_CAL_AVM_3D_VIEW_MODEL_e CT_AvmString2View3DModelType(const char *pViewModelType)
{
    AMBA_CAL_AVM_3D_VIEW_MODEL_e Rval;

    if (0 == CT_strcmp(pViewModelType, "ADVANCE")) {
        Rval = AMBA_CAL_AVM_3D_VIEW_MODEL_ADV;
    } else if (0 == CT_strcmp(pViewModelType, "SIMPLE")) {
        Rval = AMBA_CAL_AVM_3D_VIEW_MODEL_SIM;
    } else {
        Rval = AMBA_CAL_AVM_3D_VIEW_MODEL_SIM;
    }
    return Rval;
}

static const char* CT_AvmGetGroupName(UINT8 GroupId)
{

    static const struct {
        const char *const GroupName;
        AMBA_CT_AVM_TUNER_GROUP_s GroupId;
    } GroupLut[] = {
        {"AMBA_CT_AVM_SYSTEM",AMBA_CT_AVM_SYSTEM},
        {"AMBA_CT_AVM_CAR",AMBA_CT_AVM_CAR},
        {"AMBA_CT_AVM_2D_VIEW",AMBA_CT_AVM_2D_VIEW},
        {"AMBA_CT_AVM_2D_CUSTOM_CALIB",AMBA_CT_AVM_2D_CUSTOM_CALIB},
        {"AMBA_CT_AVM_3D_VIEW_MODEL_TYPE",AMBA_CT_AVM_3D_VIEW_MODEL_TYPE},
        {"AMBA_CT_AVM_3D_VIEW",AMBA_CT_AVM_3D_VIEW},
        {"AMBA_CT_AVM_3D_ADV_VIEW",AMBA_CT_AVM_3D_ADV_VIEW},
        {"AMBA_CT_AVM_3D_VITRUAL_CAM",AMBA_CT_AVM_3D_VITRUAL_CAM},
        {"AMBA_CT_AVM_3D_WARP",AMBA_CT_AVM_3D_WARP},
        {"AMBA_CT_AVM_MAIN_VIEW",AMBA_CT_AVM_MAIN_VIEW},
        {"AMBA_CT_AVM_MAIN_VIEW_PERSPECTIVE",AMBA_CT_AVM_MAIN_VIEW_PERSPECTIVE},
        {"AMBA_CT_AVM_MAIN_VIEW_EQUAL_DIST",AMBA_CT_AVM_MAIN_VIEW_EQUAL_DIST},
        {"AMBA_CT_AVM_MAIN_VIEW_LDC",AMBA_CT_AVM_MAIN_VIEW_LDC},
        {"AMBA_CT_AVM_MAIN_VIEW_DIRECTION",AMBA_CT_AVM_MAIN_VIEW_DIRECTION},
        {"AMBA_CT_AVM_VIEW_BLEND",AMBA_CT_AVM_VIEW_BLEND},
        {"AMBA_CT_AVM_OSD_MASK",AMBA_CT_AVM_OSD_MASK},
        {"AMBA_CT_AVM_LENS",AMBA_CT_AVM_LENS},
        {"AMBA_CT_AVM_SENSOR",AMBA_CT_AVM_SENSOR},
        {"AMBA_CT_AVM_OPTICAL_CENTER",AMBA_CT_AVM_OPTICAL_CENTER},
        {"AMBA_CT_AVM_CAMERA",AMBA_CT_AVM_CAMERA},
        {"AMBA_CT_AVM_VIN",AMBA_CT_AVM_VIN},
        {"AMBA_CT_AVM_CALIB_POINTS",AMBA_CT_AVM_CALIB_POINTS},
        {"AMBA_CT_AVM_ASSISTANCE_POINTS",AMBA_CT_AVM_ASSISTANCE_POINTS},
        {"AMBA_CT_AVM_ROI",AMBA_CT_AVM_ROI},
        {"AMBA_CT_AVM_MAIN",AMBA_CT_AVM_MAIN},
        {"AMBA_CT_AVM_TILE_SIZE",AMBA_CT_AVM_TILE_SIZE},
        {"AMBA_CT_AVM_AUTO",AMBA_CT_AVM_AUTO},
        {"AMBA_CT_AVM_OPTIMIZE",AMBA_CT_AVM_OPTIMIZE},
        {"AMBA_CT_AVM_INTERNAL",AMBA_CT_AVM_INTERNAL},
        {"AMBA_CT_AVM_PRECHECKDATA",AMBA_CT_AVM_PRECHECKDATA},
    };
    const char *Rval = "CALIB_TUNER_UNKNOWN_GROUP";
    UINT32 i;
    for (i = 0; i < (sizeof(GroupLut)/sizeof(GroupLut[0])); i++) {
        if (GroupId == (UINT8)GroupLut[i].GroupId) {
            Rval = GroupLut[i].GroupName;
            break;
        }
    }
    return (const char *const) Rval;
}

static const UINT8 NecessaryGroupPreCheck[] = {
    (UINT8) AMBA_CT_AVM_SYSTEM,
    (UINT8) AMBA_CT_AVM_LENS,
    (UINT8) AMBA_CT_AVM_SENSOR,
    (UINT8) AMBA_CT_AVM_OPTICAL_CENTER,
    (UINT8) AMBA_CT_AVM_CAMERA,
    (UINT8) AMBA_CT_AVM_PRECHECKDATA,
};

static UINT32 CT_AvmCheckGroup(const UINT8 *pNecessaryGroup, UINT32 GroupLen)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 i;

    for (i = 0; i < GroupLen; i++) {
        if (AvmTunerValidGroup[pNecessaryGroup[i]] == 0U) {
            pNextMsg = CT_LogPutStr(Msg, "Lose Group ");
            pNextMsg = CT_LogPutStr(pNextMsg, CT_AvmGetGroupName(pNecessaryGroup[i]));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CT_ERR_1;
        }
    }
    return Rval;
}

static UINT32 CT_AvmCheckOptionalGroup(UINT8 GroupA, UINT8 GroupB)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    if ( (GroupA >= (UINT8)AMBA_CT_AVM_TUNER_MAX) || (GroupB >= (UINT8)AMBA_CT_AVM_TUNER_MAX) ) {
        pNextMsg = CT_LogPutStr(Msg, "Invalid Group ID(");
        pNextMsg = CT_LogPutU8(pNextMsg, GroupA);
        pNextMsg = CT_LogPutStr(pNextMsg, ",");
        pNextMsg = CT_LogPutU8(pNextMsg, GroupB);
        pNextMsg = CT_LogPutStr(pNextMsg, ")");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CT_ERR_1;
    } else {
        if ( (AvmTunerValidGroup[GroupA] == 0U) && (AvmTunerValidGroup[GroupB] == 0U) ) {
            pNextMsg = CT_LogPutStr(Msg, "Lose Group, either");
            pNextMsg = CT_LogPutStr(pNextMsg, CT_AvmGetGroupName(GroupA));
            pNextMsg = CT_LogPutStr(pNextMsg, " or ");
            pNextMsg = CT_LogPutStr(pNextMsg, CT_AvmGetGroupName(GroupB));
            pNextMsg = CT_LogPutStr(pNextMsg, " must be assigned");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval = CT_ERR_1;
        }
    }

    return Rval;
}

static UINT32 CT_AvmFeedLensSpec(const AMBA_CT_AVM_LENS_s *pIn,
    AMBA_CAL_LENS_SPEC_s *pOut,
    AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRealExpectBuf,
    AMBA_CAL_LENS_DST_ANGLE_s *pAngleBuf,
    AMBA_CAL_LENS_DST_FORMULA_s *pFormulaBuf)
{
    UINT32 Rval = CT_OK;
    pOut->LensDistoType = pIn->LensDistoType;
    switch(pIn->LensDistoType) {
    case AMBA_CAL_LD_REAL_EXPECT_TBL:
        pRealExpectBuf->Length = pIn->TableLen;
        pRealExpectBuf->pRealTbl = pIn->pRealTable;
        pRealExpectBuf->pExpectTbl = pIn->pExceptTable;
        pOut->LensDistoSpec.pRealExpect = pRealExpectBuf;
        break;
    case AMBA_CAL_LD_ANGLE_TBL:
        pAngleBuf->Length = pIn->TableLen;
        pAngleBuf->pAngleTbl = pIn->pExceptAngleTable;
        pAngleBuf->pRealTbl = pIn->pRealTable;
        pOut->LensDistoSpec.pAngle = pAngleBuf;
        break;
    case AMBA_CAL_LD_REAL_EXPECT_FORMULA:
        pFormulaBuf->X1 = pIn->RealExpectFormula[0];
        pFormulaBuf->X3 = pIn->RealExpectFormula[1];
        pFormulaBuf->X5 = pIn->RealExpectFormula[2];
        pFormulaBuf->X7 = pIn->RealExpectFormula[3];
        pFormulaBuf->X9 = pIn->RealExpectFormula[4];
        pOut->LensDistoSpec.pRealExpectFormula = pFormulaBuf;
        break;
    case AMBA_CAL_LD_ANGLE_FORMULA:
        pFormulaBuf->X1 = pIn->AngleFormula[0];
        pFormulaBuf->X3 = pIn->AngleFormula[1];
        pFormulaBuf->X5 = pIn->AngleFormula[2];
        pFormulaBuf->X7 = pIn->AngleFormula[3];
        pFormulaBuf->X9 = pIn->AngleFormula[4];
        pOut->LensDistoSpec.pAngleFormula = pFormulaBuf;
        break;
    default:
        Rval |= CT_ERR_1;
        break;
    }
    return Rval;
}


static UINT32 CT_AvmFeedLensSpecV2(const AMBA_CT_AVM_LENS_s *pIn,
    AMBA_CAL_LENS_SPEC_V2_s *pOut,
    AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRealExpectBuf,
    AMBA_CAL_LENS_DST_ANGLE_s *pAngleBuf,
    AMBA_CAL_LENS_DST_FORMULA_s *pFormulaBuf)
{
    UINT32 Rval = CT_OK;
    pOut->LensDistoType = pIn->LensDistoType;
    switch(pIn->LensDistoType) {
    case AMBA_CAL_LD_REAL_EXPECT_TBL:
        pRealExpectBuf->Length = pIn->TableLen;
        pRealExpectBuf->pRealTbl = pIn->pRealTable;
        pRealExpectBuf->pExpectTbl = pIn->pExceptTable;
        pOut->LensDistoSpec.pRealExpect = pRealExpectBuf;
        break;
    case AMBA_CAL_LD_ANGLE_TBL:
        pAngleBuf->Length = pIn->TableLen;
        pAngleBuf->pAngleTbl = pIn->pExceptAngleTable;
        pAngleBuf->pRealTbl = pIn->pRealTable;
        pOut->LensDistoSpec.pAngle = pAngleBuf;
        break;
    case AMBA_CAL_LD_REAL_EXPECT_FORMULA:
        pFormulaBuf->X1 = pIn->RealExpectFormula[0];
        pFormulaBuf->X3 = pIn->RealExpectFormula[1];
        pFormulaBuf->X5 = pIn->RealExpectFormula[2];
        pFormulaBuf->X7 = pIn->RealExpectFormula[3];
        pFormulaBuf->X9 = pIn->RealExpectFormula[4];
        pOut->LensDistoSpec.pRealExpectFormula = pFormulaBuf;
        break;
    case AMBA_CAL_LD_ANGLE_FORMULA:
        pFormulaBuf->X1 = pIn->AngleFormula[0];
        pFormulaBuf->X3 = pIn->AngleFormula[1];
        pFormulaBuf->X5 = pIn->AngleFormula[2];
        pFormulaBuf->X7 = pIn->AngleFormula[3];
        pFormulaBuf->X9 = pIn->AngleFormula[4];
        pOut->LensDistoSpec.pAngleFormula = pFormulaBuf;
        break;
    default:
        Rval |= CT_ERR_1;
        break;
    }
    return Rval;
}

static inline void CT_AvmFeedPointMap(const AMBA_CT_AVM_CALIB_POINTS_s *pInCalibPoints,
    const AMBA_CT_AVM_ASSISTANCE_POINTS_s *pInAssistancePoints,
    AMBA_CAL_AVM_POINT_MAP_s *pOut)
{
    UINT32 i;
    for (i = 0; i < AVM_CALIB_POINT_NUM; i++) {
        pOut->CalibPoints[i].WorldPos.X = pInCalibPoints->WorldPositionX[i];
        pOut->CalibPoints[i].WorldPos.Y = pInCalibPoints->WorldPositionY[i];
        pOut->CalibPoints[i].WorldPos.Z = pInCalibPoints->WorldPositionZ[i];
        pOut->CalibPoints[i].RawPos.X = pInCalibPoints->RawPositionX[i];
        pOut->CalibPoints[i].RawPos.Y = pInCalibPoints->RawPositionY[i];
    }
    for (i = 0; i < pInAssistancePoints->Number; i++) {
        pOut->AssistancePoints[i].WorldPos.X = pInAssistancePoints->WorldPositionX[i];
        pOut->AssistancePoints[i].WorldPos.Y = pInAssistancePoints->WorldPositionY[i];
        pOut->AssistancePoints[i].WorldPos.Z = pInAssistancePoints->WorldPositionZ[i];
        pOut->AssistancePoints[i].RawPos.X = pInAssistancePoints->RawPositionX[i];
        pOut->AssistancePoints[i].RawPos.Y = pInAssistancePoints->RawPositionY[i];
    }
    pOut->AssistancePointNumber = pInAssistancePoints->Number;

}

static UINT32 CT_AvmDumpCalibCfg(const AMBA_CAL_AVM_CALIB_CFG_V3_s *pCfg)
{
    UINT32 Rval;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    pNextMsg = CT_LogPutStr(Msg, " FocalLength: " );
    pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->FocalLength);
    Rval = CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " RansacCfg.Enable: " );
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->RansacCfg.Enable);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " RansacCfg.IterationNum: " );
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->RansacCfg.IterationNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " RansacCfg.AllowedReprojErr: " );
    pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->RansacCfg.AllowedReprojErr);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    return Rval;
}

static UINT32 CT_AvmDump2DCfg(const AMBA_CAL_AVM_2D_CFG_V2_s *pCfg)
{
    UINT32 i, J, Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    pNextMsg = CT_LogPutStr(Msg, "Car.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Car.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "Car.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Car.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.ViewRange.StartX: ");
    pNextMsg = CT_LogPutS32(pNextMsg, pCfg->View.ViewRange.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.ViewRange.StartY: ");
    pNextMsg = CT_LogPutS32(pNextMsg, pCfg->View.ViewRange.StartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.ViewRange.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.ViewRange.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.ViewRange.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.ViewRange.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.DisplayHeight: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.DisplayHeight);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "CarVoutOSD.VoutStartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->CarVoutOSD.VoutStartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "CarVoutOSD.VoutStartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->CarVoutOSD.VoutStartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "CarVoutOSD.Width:: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->CarVoutOSD.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "CarVoutOSD.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->CarVoutOSD.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.Angle[0]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.Angle[0]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.SectorAngle[0]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.SectorAngle[0]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.Width[0]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.Width[0]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.ReferenceCarVoutPosition[0]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.ReferenceCarVoutPosition[0]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.Angle[1]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.Angle[1]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.SectorAngle[1]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.SectorAngle[1]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Width[1]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Width[1]);
    Rval = CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.ReferenceCarVoutPosition[1]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.ReferenceCarVoutPosition[1]);
    Rval = CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Angle[2]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Angle[2]);
    Rval = CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.SectorAngle[2]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.SectorAngle[2]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Width[2]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Width[2]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.ReferenceCarVoutPosition[2]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.ReferenceCarVoutPosition[2]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Angle[3]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Angle[3]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.SectorAngle[3]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.SectorAngle[3]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Width[3]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Width[3]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.ReferenceCarVoutPosition[3]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.ReferenceCarVoutPosition[3]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.AutoVoutOrderEnable:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.AutoVoutOrderEnable);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
        pNextMsg = CT_LogPutStr(Msg, "Cam[" );
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg, "].Cam.Lens.LensDistoType: " );
        pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)pCfg->Cam[i].Cam.Lens.LensDistoType);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cam[" );
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg, "].Cam.Lens.LensDistoSpec.pRealExpect->Length: " );
        pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cam[" );
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg, "].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl:" );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (J = 0; J < pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length; J++) {
            pNextMsg = CT_LogPutDB(Msg,pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl[J]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg, "Cam[" );
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg, "].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl:" );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (J = 0; J < pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length; J++) {
            pNextMsg = CT_LogPutDB(Msg,pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl[J]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.OpticalCenter.X:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.OpticalCenter.X);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.OpticalCenter.Y:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.OpticalCenter.Y );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Pos.X:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Pos.X );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Pos.Y:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Pos.Y );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Pos.Z:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Pos.Z );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Rotation:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  (UINT32)pCfg->Cam[i].Cam.Rotation );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.CellSize:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Sensor.CellSize );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.StartX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Sensor.StartX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.StartY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Sensor.StartY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Sensor.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Sensor.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.StartX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.StartX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.StartY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.StartY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i);
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.HSubSample.FactorDen:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.HSubSample.FactorDen );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.HSubSample.FactorNum:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.HSubSample.FactorNum );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.VSubSample.FactorDen:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.VSubSample.FactorDen );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i);
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.VSubSample.FactorNum:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.VSubSample.FactorNum );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);


        for (J = 0; J < (UINT32)AVM_CALIB_POINT_NUM; J++) {
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].RawPos.X );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].RawPos.Y );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].WorldPos.X );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].WorldPos.Y );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J);
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].WorldPos.Z );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePointNumber:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePointNumber );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (J = 0; J < pCfg->Cam[i].PointMap.AssistancePointNumber; J++) {
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].RawPos.X );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].RawPos.Y );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].WorldPos.X );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].WorldPos.Y );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J);
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].WorldPos.Z );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].ROI.StartX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].ROI.StartX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i);
        pNextMsg = CT_LogPutStr(Msg, " ].ROI.StartY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].ROI.StartY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].ROI.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].ROI.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i);
        pNextMsg = CT_LogPutStr(Msg, " ].ROI.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].ROI.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Main.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Main.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Main.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Main.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].MinVout.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].MinVout.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].MinVout.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].MinVout.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].MinRoiSize.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].MinRoiSize.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].MinRoiSize.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].MinRoiSize.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Tile.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Tile.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Tile.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Tile.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].AutoROI:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].AutoROI );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].AutoFrontEndRotation:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].AutoFrontEndRotation );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].OptimizeLevel:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  (UINT32)pCfg->Cam[i].OptimizeLevel );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.DebugID:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.DebugID );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.RefineDmaEnable:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.RefineDmaEnable );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.RefineWaitlineEnable:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.RefineWaitlineEnable );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.RefineOutOfRoiEnable:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.RefineOutOfRoiEnable );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.RefineVFlipEnable:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.RefineVFlipEnable );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.ExtrapolatePlaneBoundary:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.ExtrapolatePlaneBoundary );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.RepeatUnusedArea:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.RepeatUnusedArea );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.DmaTh:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.DmaTh );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.WaitlineTh:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.WaitlineTh );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].PlugIn.MsgReciverCfg.GridStatus:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].PlugIn.MsgReciverCfg.GridStatus );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].PlugIn.MsgReciverCfg.PatternError:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].PlugIn.MsgReciverCfg.PatternError );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].PlugIn.MsgReciverCfg.Report:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].PlugIn.MsgReciverCfg.Report );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    }
    return Rval;
}

static inline void CT_AvmCalibFeedPlugIn(UINT32 UseCustomCalib, AMBA_CAL_AVM_CALIB_PLUG_IN_s *pPlugIn)
{
    pPlugIn->PlugInMode = 0U;

    pPlugIn->PlugInMode |= (UINT32)AVM_CALIB_PLUGIN_ERROR_REPORT;
    pPlugIn->pCalibErrorReport = pAvmCbCalibErrorReport;

    if ( UseCustomCalib != 0U ) {
        pPlugIn->PlugInMode |= (UINT32)AVM_CALIB_PLUGIN_CUSTOM_CALIB;
        pPlugIn->pInsertCustomCalibMat = p2DAvmCbFeedCustomMat;
    }
    return;
}

static UINT32 CT_AvmCheck3DPreCheckData(void)
{
    UINT32 Rval;
    UINT32 AvmCheckGroupRval = CT_OK;
    UINT32 AvmCheckOptionalGroupRval = CT_OK;

    AMBA_CAL_AVM_PRE_CHK_CAL_DA_V2_s CalibData;

    UINT32 PreCheckVinNum = 0;
    const char *pPreCheckDataPath;
    AMBA_CAL_AVM_3D_PRE_CHECK_V2_s *pPreCheckData;
    AMBA_CAL_AVM_CALIB_DATA_V2_s CalibDataV2[AMBA_CAL_AVM_CAM_MAX];
    UINT32 PreCheckTh = 0;
    UINT32 i;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula[AMBA_CAL_AVM_CAM_MAX];

    SIZE_t Size;
    AMBA_CAL_AVM_POINT_MAP_s PointMap;
    UINT32 (*pGetPreCheckData)(const char *pFileName, void *pData, UINT32 DataLength);
    Rval = AmbaWrap_memset(&CalibData, 0x0, sizeof(CalibData));
    if(Rval != CT_OK) {
        Rval = CT_ERR_1;
    }

    AvmCheckGroupRval = CT_AvmCheckGroup(NecessaryGroupPreCheck, CT_sizeT_to_U32(sizeof(NecessaryGroupPreCheck)/sizeof(NecessaryGroupPreCheck[0])));
    AvmCheckOptionalGroupRval = CT_AvmCheckOptionalGroup((UINT8)AMBA_CT_AVM_CALIB_POINTS, (UINT8)AMBA_CT_AVM_PRECHECKDATA);

    if ( (0U != AvmCheckGroupRval) || (0U != AvmCheckOptionalGroupRval) ) {
        Rval |= CT_ERR_1;
    } else {
        CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
        CT_LOG_MSG_s *pNextMsg = Msg;

        PreCheckVinNum = CTUserAvmGroupData.PreCheckData.Number;
        pPreCheckDataPath = CTUserAvmGroupData.PreCheckData.InputFile;

        pGetPreCheckData = pAvmCbGetPreCheckData;
        pPreCheckData = pCTAvmWorkingBuf->PreCheckData;

        Rval |= AmbaCal_AvmGetPreCheckWorkSize(&Size);

        if (CTAvmCalibWorkingBufSize < Size) {
            pNextMsg = CT_LogPutStr(Msg, "Buf Size: ");
            pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(CTAvmCalibWorkingBufSize));
            pNextMsg = CT_LogPutStr(pNextMsg, " < ");
            pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(Size));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CT_ERR_1;

        } else {
            Rval |= AmbaWrap_memset(&CalibData, 0x0, sizeof(CalibData));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }
            for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
                AMBA_CAL_AVM_CALIB_CFG_V2_s CalibCfg;
                CT_CheckRval(AmbaWrap_memset(&CalibCfg.PlugIn, 0x0, sizeof(AMBA_CAL_AVM_CALIB_PLUG_IN_s)), "AmbaWrap_memset", __func__);
                CT_AvmCalibFeedPlugIn(CTUserAvmGroupData.CustomCalib2D[i].Enable, &CalibCfg.PlugIn);
                if (CalibCfg.PlugIn.PlugInMode != 0U){
                     Rval |= CT_ERR_1;
                }
                Rval |= CT_AvmFeedLensSpecV2(&CTUserAvmGroupData.Lens[i], &CalibData.Cam[i].Lens, &LensSpecRealExpect[i], &LensSpecAngle[i], &LensSpecFormula[i]);
                CalibData.Cam[i].Sensor = CTUserAvmGroupData.Sensor[i];
                CalibData.Cam[i].OpticalCenter = CTUserAvmGroupData.OpticalCenter[i];
                CalibData.Cam[i].Pos.X = CTUserAvmGroupData.Camera[i].PositionX;
                CalibData.Cam[i].Pos.Y = CTUserAvmGroupData.Camera[i].PositionY;
                CalibData.Cam[i].Pos.Z = CTUserAvmGroupData.Camera[i].PositionZ;
                CalibData.Cam[i].Rotation = CTUserAvmGroupData.Camera[i].RotateType;

                CalibCfg.LenSpec = CalibData.Cam[i].Lens;
                CalibCfg.LenPos = CalibData.Cam[i].Pos;
                CalibCfg.OpticalCenter = CalibData.Cam[i].OpticalCenter;
                CalibCfg.FocalLength = CTUserAvmGroupData.Camera[i].FocalLength;
                CalibCfg.CellSize = CalibData.Cam[i].Sensor.CellSize;
                CalibCfg.pPointMap = &PointMap;

                CT_AvmFeedPointMap(&CTUserAvmGroupData.CalibPointMap[i], &CTUserAvmGroupData.AssistancePointMap[i], &PointMap);
                Rval |= AmbaCal_DistortionTblMmToPixelV2(CalibData.Cam[i].Sensor.CellSize, &CalibData.Cam[i].Lens);

                if (CalibData.Cam[i].Lens.LensDistoType == AMBA_CAL_LD_REAL_EXPECT_TBL){
                    CalibCfg.FocalLength = AMBA_CAL_FOCAL_LENGTH_UNKNOWN; //FIXME # Move to init
                    CalibCfg.CellSize = 1.0; //TBC # why?
                }

                Rval |= AmbaCal_GenV2CalibrationInfo(i, pCTAvmCalibWorkingBuf, &CalibCfg, &CalibDataV2[i]);
                Rval |= AmbaWrap_memcpy(CalibData.CalibData[i].Calib2DMatrix, CalibDataV2[i].Calib2DMatrix, sizeof(CalibDataV2[i].Calib2DMatrix));
                CalibData.CalibData[i].Calib2DMatrixZSign = CalibDataV2[i].Calib2DMatrixZSign;
            }

            Rval |= pGetPreCheckData(pPreCheckDataPath, pPreCheckData, sizeof(AMBA_CAL_AVM_3D_PRE_CHECK_DATA_s)*PreCheckVinNum);
            Rval |= AmbaCal_Avm3DPreCheckCalibEnvV2(&CalibData, PreCheckVinNum, CalibDataV2, pPreCheckData, PreCheckTh, pCTAvmCalibWorkingBuf, &pCTAvmWorkingBuf->DataPreCheckResult);
            if (Rval != CT_OK) {
                pNextMsg = CT_LogPutStr(Msg, "call AmbaCal_Avm3DPreCheckCalibEnv fail");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            } else {
                pNextMsg = CT_LogPutStr(Msg, "call AmbaCal_Avm3DPreCheckCalibEnv success");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            }

#ifdef DUMP_UT_RGS_GOLDEN
#define RGS_2D3D_PRECHECK_NUM    4U
#define RGS_MAINVIEW_PRECHECK_NUM    1U
            {
                //Note # rgs_AvmGetPreCheckWorkSize
                ut_dump_rgs_golden_file("AvmGetPreCheckWorkSize_Out.bin", &Size, sizeof(Size));

                //Note # rgs_Avm3DPreCheckCalibEnv
                ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_In1.bin", &CalibData, sizeof(CalibData));
                for(i = 0U; i < AMBA_CAL_AVM_CAM_MAX; i++) {
                    ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_In1.bin", CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                    ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_In1.bin", CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->pRealTbl, CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                    ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_In1.bin", CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->pExpectTbl, CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                }
                ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_In2.bin", &PreCheckVinNum, sizeof(PreCheckVinNum));
                printf("dumping Avm3DPreCheckCalibEnv_In3. %d, %d, %d\n", sizeof(AMBA_CAL_AVM_3D_PRE_CHECK_V2_s), RGS_2D3D_PRECHECK_NUM, PreCheckVinNum);
                ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_In3.bin", pPreCheckData, sizeof(AMBA_CAL_AVM_3D_PRE_CHECK_V2_s) * PreCheckVinNum);
                ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_In4.bin", &PreCheckTh, sizeof(PreCheckTh));
                ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_In5.bin", &CalibDataV2, sizeof(CalibDataV2));
                ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_Out.bin", &pCTAvmWorkingBuf->DataPreCheckResult, sizeof(AMBA_CAL_AVM_PRE_CHECK_RESULT_s));
                ut_dump_rgs_golden_file("Avm3DPreCheckCalibEnv_Out.bin", pCTAvmWorkingBuf->DataPreCheckResult.pPreCheckResults, sizeof(AMBA_CAL_AVM_PRE_CHECK_STATUS_s) * PreCheckVinNum);

            }
#endif

        }
    }
    return Rval;
}

static UINT32 CT_AvmCheck2DPreCheckData(void)
{
    UINT32 Rval;
    UINT32 AvmCheckGroupRval = CT_OK;
    UINT32 AvmCheckOptionalGroupRval = CT_OK;
    AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s CalibData;
    UINT32 PreCheckVinNum = 0;
    const char *pPreCheckDataPath;
    AMBA_CAL_AVM_2D_PRE_CHECK_DATA_s *pPreCheckData;
    UINT32 PreCheckTh = 0;
    UINT32 i;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula[AMBA_CAL_AVM_CAM_MAX];
    SIZE_t Size;
    AMBA_CAL_AVM_POINT_MAP_s PointMap;

    UINT32 (*pGetPreCheckData)(const char *pFileName, void *pData, UINT32 DataLength);
    AvmCheckGroupRval = CT_AvmCheckGroup(NecessaryGroupPreCheck, CT_sizeT_to_U32(sizeof(NecessaryGroupPreCheck)/sizeof(NecessaryGroupPreCheck[0])));
    AvmCheckOptionalGroupRval =CT_AvmCheckOptionalGroup((UINT8)AMBA_CT_AVM_CALIB_POINTS, (UINT8)AMBA_CT_AVM_PRECHECKDATA);

    if ( (0U != AvmCheckGroupRval) || (0U != AvmCheckOptionalGroupRval) ) {
        Rval = CT_ERR_1;
    } else {
        CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
        CT_LOG_MSG_s *pNextMsg = Msg;
        const AMBA_CAL_AVM_3D_PRE_CHECK_V2_s *pPreCheck;

        PreCheckVinNum = CTUserAvmGroupData.PreCheckData.Number;
        pPreCheckDataPath = CTUserAvmGroupData.PreCheckData.InputFile;

        pGetPreCheckData = pAvmCbGetPreCheckData;

        pPreCheck = pCTAvmWorkingBuf->PreCheckData;
        Rval = AmbaWrap_memcpy(&pPreCheckData,&pPreCheck,sizeof(AMBA_CAL_AVM_2D_PRE_CHECK_DATA_s*));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        Rval |= AmbaCal_AvmGetPreCheckWorkSize(&Size);

        if (CTAvmCalibWorkingBufSize < Size) {
            pNextMsg = CT_LogPutStr(Msg, "Buf Size: ");
            pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(CTAvmCalibWorkingBufSize));
            pNextMsg = CT_LogPutStr(pNextMsg, " < ");
            pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(Size));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CT_ERR_1;
        } else {
            Rval |= AmbaWrap_memset(&CalibData, 0x0, sizeof(CalibData));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }
            for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
                Rval |= CT_AvmFeedLensSpec(&CTUserAvmGroupData.Lens[i], &CalibData.Cam[i].Lens, &LensSpecRealExpect[i], &LensSpecAngle[i], &LensSpecFormula[i]);
                if (CalibData.Cam[i].Lens.LensDistoType != AMBA_CAL_LD_REAL_EXPECT_TBL){
                    pNextMsg = CT_LogPutStr(Msg, "LensDistoType not support: ");
                    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)CalibData.Cam[i].Lens.LensDistoType);
                    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
                    Rval |= CT_ERR_1;
                } else {
                    Rval |= AmbaWrap_memcpy(&CalibData.Cam[i].Sensor, &CTUserAvmGroupData.Sensor[i], sizeof(CalibData.Cam[i].Sensor));
                    Rval |= AmbaWrap_memcpy(&CalibData.Cam[i].OpticalCenter, &CTUserAvmGroupData.OpticalCenter[i], sizeof(CalibData.Cam[i].OpticalCenter));
                    CalibData.Cam[i].Pos.X = CTUserAvmGroupData.Camera[i].PositionX;
                    CalibData.Cam[i].Pos.Y = CTUserAvmGroupData.Camera[i].PositionY;
                    CalibData.Cam[i].Pos.Z = CTUserAvmGroupData.Camera[i].PositionZ;
                    CalibData.Cam[i].Rotation = CTUserAvmGroupData.Camera[i].RotateType;
                    if(Rval != CT_OK) {
                        Rval = CT_ERR_1;
                    }

                    CT_AvmFeedPointMap(&CTUserAvmGroupData.CalibPointMap[i], &CTUserAvmGroupData.AssistancePointMap[i], &PointMap);
                    Rval |= AmbaCal_DistortionTblMmToPixel(CalibData.Cam[i].Sensor.CellSize, &CalibData.Cam[i].Lens);
                    Rval |= AmbaCal_GenCalibrationInfo((INT32)i, CalibData.Cam[i].Lens, CalibData.Cam[i].Pos,
                        CalibData.Cam[i].OpticalCenter, &PointMap, 0, NULL, &CalibData.CalibData[i]);
                }
            }

            if (Rval == CT_OK) {
                Rval |= pGetPreCheckData(pPreCheckDataPath, pPreCheckData, sizeof(AMBA_CAL_AVM_2D_PRE_CHECK_DATA_s)*PreCheckVinNum);
                Rval |= AmbaCal_Avm2DPreCheckCalibEnv(&CalibData, PreCheckVinNum, pPreCheckData, PreCheckTh, pCTAvmCalibWorkingBuf, &pCTAvmWorkingBuf->DataPreCheckResult);
            }

            if (Rval != CT_OK) {
                pNextMsg = CT_LogPutStr(pNextMsg, "call AmbaCal_Avm2DPreCheckCalibEnv fail");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            } else {
                pNextMsg = CT_LogPutStr(pNextMsg, "call AmbaCal_Avm2DPreCheckCalibEnv success");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            }

#ifdef DUMP_UT_RGS_GOLDEN
            {
                //Note # rgs_Avm2DPreCheckCalibEnv
                ut_dump_rgs_golden_file("Avm2DPreCheckCalibEnv_In1.bin", &CalibData, sizeof(AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s));
                for(i = 0U; i < AMBA_CAL_AVM_CAM_MAX; i++) {
                    ut_dump_rgs_golden_file("Avm2DPreCheckCalibEnv_In1.bin", CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                    ut_dump_rgs_golden_file("Avm2DPreCheckCalibEnv_In1.bin", CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->pRealTbl, CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                    ut_dump_rgs_golden_file("Avm2DPreCheckCalibEnv_In1.bin", CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->pExpectTbl, CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                }
                ut_dump_rgs_golden_file("Avm2DPreCheckCalibEnv_In2.bin", &PreCheckVinNum, sizeof(PreCheckVinNum));
                ut_dump_rgs_golden_file("Avm2DPreCheckCalibEnv_In3.bin", pPreCheckData, PreCheckVinNum * sizeof(AMBA_CAL_AVM_2D_PRE_CHECK_DATA_s));
                ut_dump_rgs_golden_file("Avm2DPreCheckCalibEnv_In4.bin", &PreCheckTh, sizeof(PreCheckTh));
                ut_dump_rgs_golden_file("Avm2DPreCheckCalibEnv_Out.bin", &pCTAvmWorkingBuf->DataPreCheckResult, sizeof(AMBA_CAL_AVM_PRE_CHECK_RESULT_s));
                ut_dump_rgs_golden_file("Avm2DPreCheckCalibEnv_Out.bin", pCTAvmWorkingBuf->DataPreCheckResult.pPreCheckResults, PreCheckVinNum * sizeof(AMBA_CAL_AVM_PRE_CHECK_STATUS_s));
            }
#endif
        }
    }
    return Rval;
}

static UINT32 CT_AvmCheckMainViewPreCheckData(void)
{
    UINT32 Rval = CT_OK;
    UINT32 AvmCheckGroupRval = CT_OK;
    UINT32 AvmCheckOptionalGroupRval = CT_OK;

    AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s CalibData;
    UINT32 PreCheckVinNum = 0;
    const char *pPreCheckDataPath;
    AMBA_CAL_AVM_MV_PRE_CHECK_DATA_s *pPreCheckData;
    UINT32 PreCheckTh = 0;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect;
    AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle;
    AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula;
    SIZE_t Size;
    AMBA_CAL_AVM_POINT_MAP_s PointMap;
    UINT32 (*pGetPreCheckData)(const char *pFileName, void *pData, UINT32 DataLength);
    AvmCheckGroupRval = CT_AvmCheckGroup(NecessaryGroupPreCheck, CT_sizeT_to_U32(sizeof(NecessaryGroupPreCheck)/sizeof(NecessaryGroupPreCheck[0])));
    AvmCheckOptionalGroupRval = CT_AvmCheckOptionalGroup((UINT8)AMBA_CT_AVM_CALIB_POINTS, (UINT8)AMBA_CT_AVM_PRECHECKDATA);

    if ( (0U != AvmCheckGroupRval) || (0U != AvmCheckOptionalGroupRval) ) {
        Rval = CT_ERR_1;
    } else {
        const AMBA_CAL_AVM_3D_PRE_CHECK_V2_s *pPreCheck;
        CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
        CT_LOG_MSG_s *pNextMsg = Msg;

        PreCheckVinNum = CTUserAvmGroupData.PreCheckData.Number;
        pPreCheckDataPath = CTUserAvmGroupData.PreCheckData.InputFile;

        pGetPreCheckData = pAvmCbGetPreCheckData;

        pPreCheck = pCTAvmWorkingBuf->PreCheckData;
        Rval |= AmbaWrap_memcpy(&pPreCheckData,&pPreCheck,sizeof(AMBA_CAL_AVM_MV_PRE_CHECK_DATA_s*));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        Rval |= AmbaCal_AvmGetPreCheckWorkSize(&Size);

        if (CTAvmCalibWorkingBufSize < Size) {
            pNextMsg = CT_LogPutStr(Msg, "Buf Size: ");
            pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(CTAvmCalibWorkingBufSize));
            pNextMsg = CT_LogPutStr(pNextMsg, " < ");
            pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(Size));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CT_ERR_1;
        } else {
            Rval |= AmbaWrap_memset(&CalibData, 0x0, sizeof(CalibData));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }

            Rval |= CT_AvmFeedLensSpec(&CTUserAvmGroupData.Lens[0], &CalibData.Cam[0].Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);

            if (CalibData.Cam[0].Lens.LensDistoType != AMBA_CAL_LD_REAL_EXPECT_TBL){
                pNextMsg = CT_LogPutStr(Msg, "LensDistoType not support: ");
                pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)CalibData.Cam[0].Lens.LensDistoType);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
                Rval |= CT_ERR_1;
            } else {

                CalibData.Cam[0].Sensor = CTUserAvmGroupData.Sensor[0];
                CalibData.Cam[0].OpticalCenter = CTUserAvmGroupData.OpticalCenter[0];
                CalibData.Cam[0].Pos.X = CTUserAvmGroupData.Camera[0].PositionX;
                CalibData.Cam[0].Pos.Y = CTUserAvmGroupData.Camera[0].PositionY;
                CalibData.Cam[0].Pos.Z = CTUserAvmGroupData.Camera[0].PositionZ;
                CalibData.Cam[0].Rotation = CTUserAvmGroupData.Camera[0].RotateType;

                CT_AvmFeedPointMap(&CTUserAvmGroupData.CalibPointMap[0], &CTUserAvmGroupData.AssistancePointMap[0], &PointMap);
                Rval |= AmbaCal_DistortionTblMmToPixel(CalibData.Cam[0].Sensor.CellSize, &CalibData.Cam[0].Lens);
                Rval |= AmbaCal_GenCalibrationInfo(0, CalibData.Cam[0].Lens, CalibData.Cam[0].Pos,
                    CalibData.Cam[0].OpticalCenter, &PointMap, 0, NULL, &CalibData.CalibData[0]);

                Rval |= pGetPreCheckData(pPreCheckDataPath, pPreCheckData, sizeof(AMBA_CAL_AVM_MV_PRE_CHECK_DATA_s)*PreCheckVinNum);
                Rval |= AmbaCal_AvmMVPreCheckCalibEnv(&CalibData, PreCheckVinNum, pPreCheckData, PreCheckTh, pCTAvmCalibWorkingBuf, &pCTAvmWorkingBuf->DataPreCheckResult);
            }


            if (Rval != CT_OK) {
                pNextMsg = CT_LogPutStr(pNextMsg, "call AmbaCal_AvmMVPreCheckCalibEnv fail");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            } else {
                pNextMsg = CT_LogPutStr(pNextMsg, "call AmbaCal_AvmMVPreCheckCalibEnv success");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            }

#ifdef DUMP_UT_RGS_GOLDEN
            {
                UINT32 i;

                //Note # rgs_Avm2DPreCheckCalibEnv
                ut_dump_rgs_golden_file("AvmMVPreCheckCalibEnv_In1.bin", &CalibData, sizeof(AMBA_CAL_AVM_PRE_CHECK_CALIB_DATA_s));
                for(i = 0U; i < 1U; i++) {
                    ut_dump_rgs_golden_file("AvmMVPreCheckCalibEnv_In1.bin", CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                    ut_dump_rgs_golden_file("AvmMVPreCheckCalibEnv_In1.bin", CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->pRealTbl, CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                    ut_dump_rgs_golden_file("AvmMVPreCheckCalibEnv_In1.bin", CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->pExpectTbl, CalibData.Cam[i].Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                }
                ut_dump_rgs_golden_file("AvmMVPreCheckCalibEnv_In2.bin", &PreCheckVinNum, sizeof(PreCheckVinNum));
                ut_dump_rgs_golden_file("AvmMVPreCheckCalibEnv_In3.bin", pPreCheckData, PreCheckVinNum * sizeof(AMBA_CAL_AVM_2D_PRE_CHECK_DATA_s));
                ut_dump_rgs_golden_file("AvmMVPreCheckCalibEnv_In4.bin", &PreCheckTh, sizeof(PreCheckTh));
                ut_dump_rgs_golden_file("AvmMVPreCheckCalibEnv_Out.bin", &pCTAvmWorkingBuf->DataPreCheckResult, sizeof(AMBA_CAL_AVM_PRE_CHECK_RESULT_s));
                ut_dump_rgs_golden_file("AvmMVPreCheckCalibEnv_Out.bin", pCTAvmWorkingBuf->DataPreCheckResult.pPreCheckResults, PreCheckVinNum * sizeof(AMBA_CAL_AVM_PRE_CHECK_STATUS_s));
            }
#endif

        }
    }
    return Rval;
}

static inline void CT_Avm2dFeedPlugIn(const char *CalibModeStr, INT32 UseCustomCalib, UINT32 EnableSavePrecheck, AMBA_CAL_AVM_2D_PLUG_IN_s *pPlugIn)
{
    AMBA_CT_AVM_CALIB_MODE_e CalibMode = CT_AvmString2CalibMode(CalibModeStr);
    pPlugIn->MsgReciverCfg.GridStatus = 1;
    pPlugIn->MsgReciverCfg.PatternError = 1;
    pPlugIn->MsgReciverCfg.Report = 1;
    pPlugIn->MsgReciver = pCTAvmCbMsgReciver;

    pPlugIn->PlugInMode = 0;
    pPlugIn->PlugInMode |= (UINT32)AVM_2D_PLUGIN_MODE_MSG_RECEIVER;

    switch (CalibMode) {
        case AMBA_CT_AVM_CALIB_MODE_FAST:
            pPlugIn->PlugInMode |= (UINT32)AVM_2D_PLUGIN_MODE_FEED_INT_DATA;
            pPlugIn->pFeedPreCalData = pAvmCbFeedInternalData;
            break;
        case AMBA_CT_AVM_CALIB_MODE_PRE_CALC:
            pPlugIn->PlugInMode |= (UINT32)AVM_2D_PLUGIN_MODE_SAVE_INT_DATA;
            pPlugIn->pSavePreCalData = pAvmCbSaveInternalData;
            break;
        case AMBA_CT_AVM_CALIB_MODE_NORMAL:
        default:
            //Note # do nothing
            break;
    }

    if ( UseCustomCalib != 0 ) {
        pPlugIn->PlugInMode |= (UINT32)AVM_2D_PLUGIN_MODE_CUSTOM_CALIB;
        pPlugIn->pInsertCustomCalibMat = p2DAvmCbFeedCustomMat;
    }

    if ( EnableSavePrecheck != 0U ) {
        pPlugIn->PlugInMode |= (UINT32)AVM_2D_PLUGIN_MODE_SAVE_PRECHECK;
        pPlugIn->pSavePreCheckData = pAvmCbSavePreCheckData;
    }

    return;
}

static UINT32 CT_Avm2dExecute(void)
{
    UINT32 Rval;
    UINT32 AvmCheckGroupRval = CT_OK;
    UINT32 AvmCheckOptionalGroupRval = CT_OK;
    static const UINT8 NecessaryGroup2d[] = {
        (UINT8) AMBA_CT_AVM_SYSTEM,
        (UINT8) AMBA_CT_AVM_CAR,
        (UINT8) AMBA_CT_AVM_2D_VIEW,
        (UINT8) AMBA_CT_AVM_VIEW_BLEND,
        (UINT8) AMBA_CT_AVM_LENS,
        (UINT8) AMBA_CT_AVM_SENSOR,
        (UINT8) AMBA_CT_AVM_OPTICAL_CENTER,
        (UINT8) AMBA_CT_AVM_CAMERA,
        (UINT8) AMBA_CT_AVM_VIN,
        (UINT8) AMBA_CT_AVM_ASSISTANCE_POINTS,
        (UINT8) AMBA_CT_AVM_ROI,
        (UINT8) AMBA_CT_AVM_MAIN,
        (UINT8) AMBA_CT_AVM_TILE_SIZE,
        (UINT8) AMBA_CT_AVM_AUTO,
        (UINT8) AMBA_CT_AVM_OPTIMIZE,
    };
    AvmCheckGroupRval = CT_AvmCheckGroup(NecessaryGroup2d, CT_sizeT_to_U32(sizeof(NecessaryGroup2d)/sizeof(NecessaryGroup2d[0])));
    AvmCheckOptionalGroupRval = CT_AvmCheckOptionalGroup((UINT8)AMBA_CT_AVM_CALIB_POINTS, (UINT8)AMBA_CT_AVM_2D_CUSTOM_CALIB);

    if ( (0U != AvmCheckGroupRval) || (0U != AvmCheckOptionalGroupRval) ) {
        Rval = CT_ERR_1;
    } else {
        UINT32 i;
        AMBA_CAL_AVM_CALIB_DATA_s CalibrationData[AMBA_CAL_AVM_CAM_MAX];
        AMBA_CAL_AVM_2D_CFG_V2_s Cfg;

        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect[AMBA_CAL_AVM_CAM_MAX];
        AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle[AMBA_CAL_AVM_CAM_MAX];
        AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula[AMBA_CAL_AVM_CAM_MAX];
        SIZE_t Size;
        UINT32 LoadCarOsdMask;
        AMBA_CAL_LENS_SPEC_s LensSpec;

        Rval = AmbaCal_AvmGetBirdViewWorkSize(&Size);

        if (CTAvmCalibWorkingBufSize < Size) {
            Rval |= CT_ERR_1;
        } else {
            static const AMBA_CAL_AVM_OPTIMIZE_LEVEL_e PTIMIZE_LEVEL_ENUM_LUT[4] =
                {   AMBA_CAL_AVM_OPTIMIZE_LV_0, //Note #  without optimize
                    AMBA_CAL_AVM_OPTIMIZE_LV_1, //Note #  optimize OC
                    AMBA_CAL_AVM_OPTIMIZE_LV_2, //Note #  optimize H matrix
                    AMBA_CAL_AVM_OPTIMIZE_LV_3, //Note #  optimize OC + H matrix
                };
            Rval |= AmbaWrap_memset(&Cfg, 0x0, sizeof(Cfg));
            Rval |= AmbaWrap_memcpy(&Cfg.Car, &CTUserAvmGroupData.Car, sizeof(Cfg.Car));
            Rval |= AmbaWrap_memcpy(&Cfg.View.ViewRange, &CTUserAvmGroupData.View2D.CropRange, sizeof(Cfg.View.ViewRange));
            Rval |= AmbaWrap_memcpy(&Cfg.View.Blend, &CTUserAvmGroupData.Blend, sizeof(Cfg.View.Blend));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }

            Cfg.View.AutoVoutOrderEnable = CTUserAvmGroupData.View2D.AutoVoutOrderEnable;
            Cfg.View.DisplayHeight = CTUserAvmGroupData.View2D.DisplayHeight;
            LoadCarOsdMask = 0U;
            for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
                if ( (Cfg.View.Blend.ReferenceCarVoutPosition[i] == 1U) && (LoadCarOsdMask == 0U) ) {
                    if ( CTUserAvmGroupData.OsdMask.FromFile == 1U ) {
                        Rval |= pAvmCbFeedOSDMaskFunc(CTUserAvmGroupData.OsdMask.Width, CTUserAvmGroupData.OsdMask.Height, CTUserAvmGroupData.OsdMask.TablePath, CTUserAvmGroupData.OsdMask.Table);
                    } else {
                        //Note #  Custom Cb
                    }

                    LoadCarOsdMask = 1U;
                    Cfg.CarVoutOSD.Pattern = CTUserAvmGroupData.OsdMask.Table;
                    Cfg.CarVoutOSD.VoutStartX = CTUserAvmGroupData.OsdMask.StartX;
                    Cfg.CarVoutOSD.VoutStartY = CTUserAvmGroupData.OsdMask.StartY;
                    Cfg.CarVoutOSD.Width = CTUserAvmGroupData.OsdMask.Width;
                    Cfg.CarVoutOSD.Height = CTUserAvmGroupData.OsdMask.Height;
                }
                Rval |= CT_AvmFeedLensSpecV2(&CTUserAvmGroupData.Lens[i], &Cfg.Cam[i].Cam.Lens, &LensSpecRealExpect[i], &LensSpecAngle[i], &LensSpecFormula[i]);
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Cam.Sensor, &CTUserAvmGroupData.Sensor[i], sizeof(Cfg.Cam[i].Cam.Sensor));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Cam.OpticalCenter, &CTUserAvmGroupData.OpticalCenter[i], sizeof(Cfg.Cam[i].Cam.OpticalCenter));
                Cfg.Cam[i].Cam.Pos.X = CTUserAvmGroupData.Camera[i].PositionX;
                Cfg.Cam[i].Cam.Pos.Y = CTUserAvmGroupData.Camera[i].PositionY;
                Cfg.Cam[i].Cam.Pos.Z = CTUserAvmGroupData.Camera[i].PositionZ;
                Cfg.Cam[i].Cam.Rotation = CTUserAvmGroupData.Camera[i].RotateType;
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].VinSensorGeo, &CTUserAvmGroupData.Vin[i], sizeof(Cfg.Cam[i].VinSensorGeo));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].ROI, &CTUserAvmGroupData.Roi[i], sizeof(Cfg.Cam[i].ROI));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Main, &CTUserAvmGroupData.MainSize[i], sizeof(Cfg.Cam[i].Main));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Tile, &CTUserAvmGroupData.TileSize[i], sizeof(Cfg.Cam[i].Tile));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].MinVout, &CTUserAvmGroupData.MinVout[i], sizeof(Cfg.Cam[i].MinVout));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].MinRoiSize, &CTUserAvmGroupData.MinRoiSize[i], sizeof(Cfg.Cam[i].MinRoiSize));
                CT_AvmFeedPointMap(&CTUserAvmGroupData.CalibPointMap[i], &CTUserAvmGroupData.AssistancePointMap[i], &Cfg.Cam[i].PointMap);
                Cfg.Cam[i].AutoROI = CTUserAvmGroupData.Auto[i].RoiEnable;
                Cfg.Cam[i].AutoFrontEndRotation = CTUserAvmGroupData.Auto[i].FrontEndRotationEnable;
                Cfg.Cam[i].OptimizeLevel = PTIMIZE_LEVEL_ENUM_LUT[CTUserAvmGroupData.OptimizeLevel[i]];
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].InternalCfg, &CTUserAvmGroupData.Internal[i], sizeof(Cfg.Cam[i].InternalCfg));
                if(Rval != CT_OK) {
                    Rval = CT_ERR_1;
                }

                CT_Avm2dFeedPlugIn(CTUserAvmGroupData.System.CalibMode, (INT32)CTUserAvmGroupData.CustomCalib2D[i].Enable, CTUserAvmGroupData.System.EnableSavePrecheck, &Cfg.Cam[i].PlugIn);
            }
            Rval |= CT_AvmDump2DCfg(&Cfg);

#ifdef DUMP_UT_RGS_GOLDEN
            extern ut_dump_rgs_golden_file(char *Filename, void *Bin, SIZE_t Size);
            {
                UINT32 i = AMBA_CAL_AVM_CAM_MAX - 1U;

                //Note # dump when it's still in mm
                ut_dump_rgs_golden_file("DistortionTblMmToPixel_In2.bin", &Cfg.Cam[i].Cam.Lens, sizeof(AMBA_CAL_LENS_SPEC_V2_s));
                ut_dump_rgs_golden_file("DistortionTblMmToPixel_In2.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                ut_dump_rgs_golden_file("DistortionTblMmToPixel_In2.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                ut_dump_rgs_golden_file("DistortionTblMmToPixel_In2.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
            }
#endif

            //Note #  Calibration process
            for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
                INT32 UseCustomCalibMatrix = 0;

                Rval |= AmbaCal_DistortionTblMmToPixelV2(Cfg.Cam[i].Cam.Sensor.CellSize, &Cfg.Cam[i].Cam.Lens);

                if ( (Cfg.Cam[i].PlugIn.PlugInMode & (UINT32)AVM_2D_PLUGIN_MODE_CUSTOM_CALIB) != 0U ) {
                    UseCustomCalibMatrix = 1;
                }

                {

                    LensSpec.LensDistoType = Cfg.Cam[i].Cam.Lens.LensDistoType;
                    LensSpec.LensDistoUnit = Cfg.Cam[i].Cam.Lens.LensDistoUnit;
                    LensSpec.LensDistoSpec.pRealExpect = Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect;
                    LensSpec.LensDistoSpec.pAngle = Cfg.Cam[i].Cam.Lens.LensDistoSpec.pAngle;
                    LensSpec.LensDistoSpec.pRealExpectFormula = Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpectFormula;
                    LensSpec.LensDistoSpec.pAngleFormula = Cfg.Cam[i].Cam.Lens.LensDistoSpec.pAngleFormula;
                    Rval |= AmbaCal_GenCalibrationInfo((INT32)i, LensSpec, Cfg.Cam[i].Cam.Pos,
                        Cfg.Cam[i].Cam.OpticalCenter, &Cfg.Cam[i].PointMap, UseCustomCalibMatrix, Cfg.Cam[i].PlugIn.pInsertCustomCalibMat, &CalibrationData[i]);

                }
            }

            Rval |= AmbaWrap_memset(&pCTAvmWorkingBuf->Data2D, 0, sizeof(pCTAvmWorkingBuf->Data2D));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }

            Rval |= AmbaCal_AvmGenBirdViewTblV2(CalibrationData, &Cfg, pCTAvmCalibWorkingBuf, &pCTAvmWorkingBuf->Data2D);

#ifdef DUMP_UT_RGS_GOLDEN
            extern ut_dump_rgs_golden_file(char *Filename, void *Bin, SIZE_t Size);
            {
                UINT32 i = AMBA_CAL_AVM_CAM_MAX - 1U;
                SIZE_t Max2DPreCalSize;

                //Note # rgs_AvmGetBirdViewWorkSize
                ut_dump_rgs_golden_file("AvmGetBirdViewWorkSize_Out.bin", &Size, sizeof(Size));

                //Note # rgs_DistortionTblMmToPixel
                ut_dump_rgs_golden_file("DistortionTblMmToPixel_In1.bin", &Cfg.Cam[i].Cam.Sensor.CellSize, sizeof(DOUBLE));
                ut_dump_rgs_golden_file("DistortionTblMmToPixel_Out.bin", &Cfg.Cam[i].Cam.Lens, sizeof(AMBA_CAL_LENS_SPEC_V2_s));
                ut_dump_rgs_golden_file("DistortionTblMmToPixel_Out.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                ut_dump_rgs_golden_file("DistortionTblMmToPixel_Out.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                ut_dump_rgs_golden_file("DistortionTblMmToPixel_Out.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));

                //Note # rgs_AvmGenBirdViewTbl
                ut_dump_rgs_golden_file("AvmGenBirdViewTbl_In1.bin", &CalibrationData[0], sizeof(CalibrationData));
                ut_dump_rgs_golden_file("AvmGenBirdViewTbl_In2.bin", &Cfg, sizeof(AMBA_CAL_AVM_2D_CFG_V2_s));
                for(i = 0U; i < AMBA_CAL_AVM_CAM_MAX; i++) {
                    ut_dump_rgs_golden_file("AvmGenBirdViewTbl_In2.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                    ut_dump_rgs_golden_file("AvmGenBirdViewTbl_In2.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                    ut_dump_rgs_golden_file("AvmGenBirdViewTbl_In2.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                }
                ut_dump_rgs_golden_file("AvmGenBirdViewTbl_Out.bin", &pCTAvmWorkingBuf->Data2D, sizeof(AMBA_CAL_AVM_2D_DATA_s));

                //Note # rgs_AvmGetMax2DPreCalSize
                AmbaCal_AvmGetMax2DPreCalSize(&Max2DPreCalSize);
                ut_dump_rgs_golden_file("AvmGetMax2DPreCalSize_Out.bin", &Max2DPreCalSize, sizeof(Max2DPreCalSize));
            }
#endif

        }
    }
    return Rval;
}

static UINT32 CT_AvmDump3DCfg(const AMBA_CAL_AVM_3D_CFG_V2_s *pCfg)
{
    UINT32 i, J, Rval;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    pNextMsg = CT_LogPutStr(Msg, " Car.Width:" );
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Car.Width);
    Rval = CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " Car.Height:" );
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Car.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Type: ");
    pNextMsg = CT_LogPutStr(pNextMsg, ((pCfg->View.ModelCfg.Type==AMBA_CAL_AVM_3D_VIEW_MODEL_ADV)?"ADVANCE":"SIMPLE"));
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    if ( pCfg->View.ModelCfg.Type == AMBA_CAL_AVM_3D_VIEW_MODEL_SIM ) {
        pNextMsg = CT_LogPutStr(Msg, " View.FloorViewRangeX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.SimpleModelCfg.FloorViewRangeX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.FloorViewRangeY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.SimpleModelCfg.FloorViewRangeY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.ViewCenter.X:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.ModelCfg.SimpleModelCfg.ViewCenter.X );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.ViewCenter.Y:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.ModelCfg.SimpleModelCfg.ViewCenter.Y );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.ViewDistance:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.SimpleModelCfg.ViewDistance );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.ViewHeight:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.SimpleModelCfg.ViewHeight );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    } else if ( pCfg->View.ModelCfg.Type == AMBA_CAL_AVM_3D_VIEW_MODEL_ADV ) {
        pNextMsg = CT_LogPutStr(Msg, " View.TopFrontRange:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopFrontRange );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopBackRange:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopBackRange );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopLeftRange:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopLeftRange );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopRightRange:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopRightRange );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopFrontLeftRadiusX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopFrontLeftRadiusX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopFrontLeftRadiusY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopFrontLeftRadiusY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopFrontRightRadiusX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopFrontRightRadiusX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopFrontRightRadiusY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopFrontRightRadiusY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopBackLeftRadiusX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopBackLeftRadiusX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopBackLeftRadiusY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopBackLeftRadiusY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopBackRightRadiusX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopBackRightRadiusX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopBackRightRadiusY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopBackRightRadiusY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopHeight:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopHeight );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomFrontRange:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomFrontRange );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomBackRange:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomBackRange );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomLeftRange:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomLeftRange );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomRightRange:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomRightRange );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomFrontLeftRadiusX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomFrontLeftRadiusX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomFrontLeftRadiusY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomFrontLeftRadiusY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomFrontRightRadiusX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomFrontRightRadiusX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomFrontRightRadiusY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomFrontRightRadiusY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomBackLeftRadiusX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomBackLeftRadiusX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomBackLeftRadiusY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomBackLeftRadiusY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomBackRightRadiusX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomBackRightRadiusX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomBackRightRadiusY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomBackRightRadiusY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopToBottomRadiusH:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopToBottomRadiusH );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.TopToBottomRadiusV:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.TopToBottomRadiusV );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.BottomCornerIntervalNum:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.BottomCornerIntervalNum );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.PillarIntervalNum:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.PillarIntervalNum );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.ViewCenter.X:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.ViewCenter.X );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.ViewCenter.Y:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.ViewCenter.Y );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " View.RotationAngle:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.ModelCfg.AdvanceModelCfg.RotationAngle );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);


    }else {
        //Note # do nothing
    }

    pNextMsg = CT_LogPutStr(Msg, " View.DisplayWidth:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.DisplayWidth );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.DisplayHeight:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.DisplayHeight );
    Rval |= CT_LogPackMsg((UINT32)CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.DisplayRotation:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,(UINT32)pCfg->View.DisplayRotation );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.VirtualCam.Focus.X:  " );
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.VirtualCam.Focus.X );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.VirtualCam.Focus.Y:  " );
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.VirtualCam.Focus.Y );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.VirtualCam.Focus.Z:  " );
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.VirtualCam.Focus.Z );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.VirtualCam.ProjectPlane.X:  " );
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.VirtualCam.ProjectPlane.X );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.VirtualCam.ProjectPlane.Y:  " );
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.VirtualCam.ProjectPlane.Y );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.VirtualCam.ProjectPlane.Z:  " );
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.VirtualCam.ProjectPlane.Z );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.VirtualCam.ProjectPlaneShift.X:  " );
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.VirtualCam.ProjectPlaneShift.X );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.VirtualCam.ProjectPlaneShift.Y:  " );
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.VirtualCam.ProjectPlaneShift.Y );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " CarVoutOSD.VoutStartX:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->CarVoutOSD.VoutStartX );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " CarVoutOSD.VoutStartY:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->CarVoutOSD.VoutStartY );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " CarVoutOSD.Width:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->CarVoutOSD.Width );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " CarVoutOSD.Height:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->CarVoutOSD.Height );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Angle[0]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Angle[0] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.SectorAngle[0]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.SectorAngle[0]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Width[0]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Width[0] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.ReferenceCarVoutPosition[0]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.ReferenceCarVoutPosition[0] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Angle[1]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Angle[1] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.SectorAngle[1]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.SectorAngle[1]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Width[1]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Width[1] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.ReferenceCarVoutPosition[1]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.ReferenceCarVoutPosition[1] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Angle[2]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Angle[2] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.SectorAngle[2]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.SectorAngle[2]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Width[2]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Width[2] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.ReferenceCarVoutPosition[2]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.ReferenceCarVoutPosition[2] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Angle[3]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Angle[3] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.Blend.SectorAngle[3]: ");
    pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->View.Blend.SectorAngle[3]);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.Width[3]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.Width[3] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.Blend.ReferenceCarVoutPosition[3]:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Blend.ReferenceCarVoutPosition[3] );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, " View.AutoVoutOrderEnable:  " );
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.AutoVoutOrderEnable );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);


    for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
        pNextMsg = CT_LogPutStr(Msg, " Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Lens.LensDistoType:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  (UINT32)pCfg->Cam[i].Cam.Lens.LensDistoType );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

        switch(pCfg->Cam[i].Cam.Lens.LensDistoType) {
            case AMBA_CAL_LD_REAL_EXPECT_TBL:
                pNextMsg = CT_LogPutStr(Msg, " Cam[ " );
                pNextMsg = CT_LogPutU32(pNextMsg,  i );
                pNextMsg = CT_LogPutStr(Msg, " ].Cam.Lens.LensDistoSpec.pRealExpect->Length:  " );
                pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length );
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " Cam[ " );
                pNextMsg = CT_LogPutU32(pNextMsg, i );
                pNextMsg = CT_LogPutStr(Msg, " ].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl: " );
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                for (J = 0; J < pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length; J++) {
                    pNextMsg = CT_LogPutDB(Msg,pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl[J]);
                    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                }
                pNextMsg = CT_LogPutStr(Msg, " Cam[ " );
                pNextMsg = CT_LogPutU32(pNextMsg, i );
                pNextMsg = CT_LogPutStr(Msg, " ].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl: " );
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                for (J = 0; J < pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length; J++) {
                    pNextMsg = CT_LogPutDB(Msg,pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl[J]);
                    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                }
                break;
            case AMBA_CAL_LD_ANGLE_TBL:
                pNextMsg = CT_LogPutStr(Msg, " Cam[ " );
                pNextMsg = CT_LogPutU32(pNextMsg,  i );
                pNextMsg = CT_LogPutStr(Msg, " ].Cam.Lens.LensDistoSpec.pAngle->Length:  " );
                pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngle->Length );
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " Cam[ " );
                pNextMsg = CT_LogPutU32(pNextMsg, i );
                pNextMsg = CT_LogPutStr(Msg, " ].Cam.Lens.LensDistoSpec.pAngle->pAngleTbl: " );
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                for (J = 0; J < pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngle->Length; J++) {
                    pNextMsg = CT_LogPutDB(Msg,pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngle->pAngleTbl[J]);
                    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                }
                pNextMsg = CT_LogPutStr(Msg, " Cam[ " );
                pNextMsg = CT_LogPutU32(pNextMsg, i );
                pNextMsg = CT_LogPutStr(Msg, " ].Cam.Lens.LensDistoSpec.pAngle->pRealTbl: " );
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                for (J = 0; J < pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngle->Length; J++) {
                    pNextMsg = CT_LogPutDB(Msg,pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngle->pRealTbl[J]);
                    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                }
                break;
            case AMBA_CAL_LD_REAL_EXPECT_FORMULA:
                pNextMsg = CT_LogPutStr(Msg, " Cam[ " );
                pNextMsg = CT_LogPutU32(pNextMsg, i );
                pNextMsg = CT_LogPutStr(Msg, " ].Cam.Lens.LensDistoSpec.pRealExpectFormula: " );
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X1 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpectFormula->X1);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X3 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpectFormula->X3 );
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X5 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpectFormula->X5);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X7 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpectFormula->X7);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X9 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pRealExpectFormula->X9);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                break;
            case AMBA_CAL_LD_ANGLE_FORMULA:
                pNextMsg = CT_LogPutStr(Msg, " Cam[ " );
                pNextMsg = CT_LogPutU32(pNextMsg, i );
                pNextMsg = CT_LogPutStr(Msg, " ].Cam.Lens.LensDistoSpec.pAngleFormula: " );
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X1 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngleFormula->X1);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X3 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngleFormula->X3);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X5 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngleFormula->X5);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X7 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngleFormula->X7);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, " X9 = " );
                pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Lens.LensDistoSpec.pAngleFormula->X9);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                break;
            default:
                //Note # do nothing
                break;
        }
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.OpticalCenter.X:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.OpticalCenter.X );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.OpticalCenter.Y:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.OpticalCenter.Y );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Pos.X:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Pos.X );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Pos.Y:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Pos.Y );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Pos.Z:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Pos.Z );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Rotation:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  (UINT32)pCfg->Cam[i].Cam.Rotation );
        Rval |= CT_LogPackMsg((UINT32)CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.CellSize:  " );
        pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].Cam.Sensor.CellSize );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.StartX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Sensor.StartX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.StartY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Sensor.StartY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Sensor.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Cam.Sensor.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Cam.Sensor.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.StartX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.StartX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.StartY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.StartY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.HSubSample.FactorDen:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.HSubSample.FactorDen );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.HSubSample.FactorNum:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.HSubSample.FactorNum );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.VSubSample.FactorDen:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.VSubSample.FactorDen );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].VinSensorGeo.VSubSample.FactorNum:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].VinSensorGeo.VSubSample.FactorNum );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

        for (J = 0; J < (UINT32)AVM_CALIB_POINT_NUM; J++) {
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].RawPos.X );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].RawPos.Y );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].WorldPos.X );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].WorldPos.Y );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.CalibPoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.CalibPoints[J].WorldPos.Z );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

        }
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePointNumber:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePointNumber );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (J = 0; J < pCfg->Cam[i].PointMap.AssistancePointNumber; J++) {
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].RawPos.X );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].RawPos.Y );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].WorldPos.X );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].WorldPos.Y );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  i );
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutU32(pNextMsg,  J);
            pNextMsg = CT_LogPutStr(Msg, " ].PointMap.AssistancePoints[ " );
            pNextMsg = CT_LogPutDB(pNextMsg,  pCfg->Cam[i].PointMap.AssistancePoints[J].WorldPos.Z );
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].ROI.StartX:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].ROI.StartX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].ROI.StartY:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].ROI.StartY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].ROI.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].ROI.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].ROI.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].ROI.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Main.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Main.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Main.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Main.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].MinVout.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].MinVout.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].MinVout.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].MinVout.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].MinRoiSize.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].MinRoiSize.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].MinRoiSize.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].MinRoiSize.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Tile.Width:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Tile.Width );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].Tile.Height:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].Tile.Height );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].AutoROI:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].AutoROI );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].AutoFrontEndRotation:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].AutoFrontEndRotation );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].OptimizeLevel:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  (UINT32)pCfg->Cam[i].OptimizeLevel);
        Rval |= CT_LogPackMsg((UINT32)CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.DebugID:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.DebugID );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.DmaTh:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.DmaTh );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].InternalCfg.WaitlineTh:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].InternalCfg.WaitlineTh );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].PlugIn.MsgReciverCfg.GridStatus:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].PlugIn.MsgReciverCfg.GridStatus );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].PlugIn.MsgReciverCfg.PatternError:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].PlugIn.MsgReciverCfg.PatternError );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].PlugIn.MsgReciverCfg.Report:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  pCfg->Cam[i].PlugIn.MsgReciverCfg.Report );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, " pCfg->Cam[ " );
        pNextMsg = CT_LogPutU32(pNextMsg,  i );
        pNextMsg = CT_LogPutStr(Msg, " ].WarpCalType:  " );
        pNextMsg = CT_LogPutU32(pNextMsg,  (UINT32)pCfg->Cam[i].WarpCalType );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    }
    return Rval;
}

static inline void CT_Avm3dFeedPlugIn(const char *CalibModeStr, INT32 UseCustomCalib, UINT32 EnableSavePrecheck, AMBA_CAL_AVM_3D_PLUG_IN_V2_s *pPlugIn)
{
    AMBA_CT_AVM_CALIB_MODE_e CalibMode = CT_AvmString2CalibMode(CalibModeStr);
    pPlugIn->MsgReciverCfg.GridStatus = 1;
    pPlugIn->MsgReciverCfg.PatternError = 1;
    pPlugIn->MsgReciverCfg.Report = 1;
    pPlugIn->MsgReciver = pCTAvmCbMsgReciver;

    pPlugIn->PlugInMode = 0;
    pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_MSG_RECEIVER;
    pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_SAVE_CAR_VOUT;
    pPlugIn->pSaveCarVout4Points = pAvmCbSaveCarVout4Points;

    switch (CalibMode) {
        case AMBA_CT_AVM_CALIB_MODE_FAST:
            pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_FEED_INT_DATA;
            pPlugIn->pFeedPreCalData = pAvmCbFeedInternalData;
            break;
        case AMBA_CT_AVM_CALIB_MODE_PRE_CALC:
            pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_SAVE_INT_DATA;
            pPlugIn->pSavePreCalData = pAvmCbSaveInternalData;
            break;
        case AMBA_CT_AVM_CALIB_MODE_NORMAL:
        default:
            //Note # do nothing
            break;
    }

    if ( UseCustomCalib != 0 ) {
        pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_CUSTOM_CALIB;
        pPlugIn->pInsertCustomCalibMat = p2DAvmCbFeedCustomMat;
    }

    if ( EnableSavePrecheck != 0U ) {
        pPlugIn->PlugInMode |= (UINT32)AVM_3D_PLUGIN_MODE_SAVE_PRECHECK;
        pPlugIn->pSavePreCheckData = pAvmCbSavePreCheckData;
    }

    return;
}

static UINT32 CT_Avm3dExecute(void)
{
    UINT32 Rval = CT_OK;
    UINT32 AvmCheckGroupRval = CT_OK;
    UINT32 AvmCheckOptionalGroup3DRval = CT_OK;
    UINT32 AvmCheckOptionalGroup2DRval = CT_OK;
    static const UINT8 NecessaryGroup3d[] = {
        (UINT8) AMBA_CT_AVM_SYSTEM,
        (UINT8) AMBA_CT_AVM_CAR,
        (UINT8) AMBA_CT_AVM_3D_VITRUAL_CAM,
        (UINT8) AMBA_CT_AVM_3D_WARP,
        (UINT8) AMBA_CT_AVM_VIEW_BLEND,
        (UINT8) AMBA_CT_AVM_LENS,
        (UINT8) AMBA_CT_AVM_SENSOR,
        (UINT8) AMBA_CT_AVM_OPTICAL_CENTER,
        (UINT8) AMBA_CT_AVM_CAMERA,
        (UINT8) AMBA_CT_AVM_VIN,
        (UINT8) AMBA_CT_AVM_ASSISTANCE_POINTS,
        (UINT8) AMBA_CT_AVM_ROI,
        (UINT8) AMBA_CT_AVM_MAIN,
        (UINT8) AMBA_CT_AVM_TILE_SIZE,
        (UINT8) AMBA_CT_AVM_AUTO,
        (UINT8) AMBA_CT_AVM_OPTIMIZE,
    };
    AvmCheckGroupRval = CT_AvmCheckGroup(NecessaryGroup3d, CT_sizeT_to_U32(sizeof(NecessaryGroup3d)/sizeof(NecessaryGroup3d[0])));
    AvmCheckOptionalGroup3DRval = CT_AvmCheckOptionalGroup((UINT8)AMBA_CT_AVM_3D_VIEW, (UINT8)AMBA_CT_AVM_3D_ADV_VIEW);
    AvmCheckOptionalGroup2DRval = CT_AvmCheckOptionalGroup((UINT8)AMBA_CT_AVM_CALIB_POINTS, (UINT8)AMBA_CT_AVM_2D_CUSTOM_CALIB);

    if ((0U != AvmCheckGroupRval ) || (0U != AvmCheckOptionalGroup3DRval) || (0U != AvmCheckOptionalGroup2DRval) ) {
        Rval = CT_ERR_1;
    } else {
        UINT32 i;
        AMBA_CAL_AVM_3D_CFG_V2_s Cfg;
        AMBA_CAL_AVM_CALIB_DATA_V2_s CalibrationData[AMBA_CAL_AVM_CAM_MAX];
        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect[AMBA_CAL_AVM_CAM_MAX];
        AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle[AMBA_CAL_AVM_CAM_MAX];
        AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula[AMBA_CAL_AVM_CAM_MAX];
        SIZE_t Size;
        UINT32 LoadCarOsdMask;
        Rval |= AmbaCal_AvmGet3DWorkSize(&Size);
        if (CTAvmCalibWorkingBufSize < Size) {
            Rval |= CT_ERR_1;
        } else {
            static const AMBA_CAL_ROTATION_e CAL_ROTATION_ENUM_LUT[4] =
                {   AMBA_CAL_ROTATE_0,
                    AMBA_CAL_ROTATE_90,
                    AMBA_CAL_ROTATE_180,
                    AMBA_CAL_ROTATE_270,
                };

            static const AMBA_CAL_AVM_OPTIMIZE_LEVEL_e AVM_OPTIMIZE_LEVEL_LUT[5] =
                {   AMBA_CAL_AVM_OPTIMIZE_LV_0,
                    AMBA_CAL_AVM_OPTIMIZE_LV_1, //Note # optimize OC
                    AMBA_CAL_AVM_OPTIMIZE_LV_2, //Note # optimize H matrix
                    AMBA_CAL_AVM_OPTIMIZE_LV_3, //Note # optimize OC + H matrix
                    AMBA_CAL_AVM_OPTIMIZE_LV_MAX
                };
            Rval |= AmbaWrap_memset(&Cfg, 0x0, sizeof(Cfg));
            Rval |= AmbaWrap_memcpy(&Cfg.Car, &CTUserAvmGroupData.Car, sizeof(Cfg.Car));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }

            Cfg.View.ModelCfg.Type = CT_AvmString2View3DModelType(CTUserAvmGroupData.View3DModelType.ModelType);

            if ( Cfg.View.ModelCfg.Type == AMBA_CAL_AVM_3D_VIEW_MODEL_SIM ) {
                Cfg.View.ModelCfg.SimpleModelCfg.FloorViewRangeX = CTUserAvmGroupData.View3D.FloorRangeX;
                Cfg.View.ModelCfg.SimpleModelCfg.FloorViewRangeY = CTUserAvmGroupData.View3D.FloorRangeY;
                Cfg.View.ModelCfg.SimpleModelCfg.ViewCenter.X = CTUserAvmGroupData.View3D.CenterPositionX;
                Cfg.View.ModelCfg.SimpleModelCfg.ViewCenter.Y = CTUserAvmGroupData.View3D.CenterPositionY;
                Cfg.View.ModelCfg.SimpleModelCfg.ViewDistance = CTUserAvmGroupData.View3D.Distance;
                Cfg.View.ModelCfg.SimpleModelCfg.ViewHeight = CTUserAvmGroupData.View3D.Height;

                Cfg.View.DisplayWidth = CTUserAvmGroupData.View3D.DisplayWidth;
                Cfg.View.DisplayHeight = CTUserAvmGroupData.View3D.DisplayHeight;
                Cfg.View.DisplayRotation = CAL_ROTATION_ENUM_LUT[CTUserAvmGroupData.View3D.DisplayRotation];
                Cfg.View.AutoVoutOrderEnable = CTUserAvmGroupData.View3D.AutoVoutOrderEnable;
            } else if ( Cfg.View.ModelCfg.Type == AMBA_CAL_AVM_3D_VIEW_MODEL_ADV ) {
                Cfg.View.ModelCfg.AdvanceModelCfg.TopFrontRange = CTUserAvmGroupData.AdvView3D.TopFrontRange;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopBackRange = CTUserAvmGroupData.AdvView3D.TopBackRange;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopLeftRange = CTUserAvmGroupData.AdvView3D.TopLeftRange;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopRightRange = CTUserAvmGroupData.AdvView3D.TopRightRange;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopFrontLeftRadiusX = CTUserAvmGroupData.AdvView3D.TopFrontLeftRadiusX;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopFrontLeftRadiusY = CTUserAvmGroupData.AdvView3D.TopFrontLeftRadiusY;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopFrontRightRadiusX = CTUserAvmGroupData.AdvView3D.TopFrontRightRadiusX;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopFrontRightRadiusY = CTUserAvmGroupData.AdvView3D.TopFrontRightRadiusY;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopBackLeftRadiusX = CTUserAvmGroupData.AdvView3D.TopBackLeftRadiusX;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopBackLeftRadiusY = CTUserAvmGroupData.AdvView3D.TopBackLeftRadiusY;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopBackRightRadiusX = CTUserAvmGroupData.AdvView3D.TopBackRightRadiusX;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopBackRightRadiusY = CTUserAvmGroupData.AdvView3D.TopBackRightRadiusY;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopHeight = CTUserAvmGroupData.AdvView3D.TopHeight;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomFrontRange = CTUserAvmGroupData.AdvView3D.BottomFrontRange;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomBackRange = CTUserAvmGroupData.AdvView3D.BottomBackRange;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomLeftRange = CTUserAvmGroupData.AdvView3D.BottomLeftRange;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomRightRange = CTUserAvmGroupData.AdvView3D.BottomRightRange;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomFrontLeftRadiusX = CTUserAvmGroupData.AdvView3D.BottomFrontLeftRadiusX;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomFrontLeftRadiusY = CTUserAvmGroupData.AdvView3D.BottomFrontLeftRadiusY;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomFrontRightRadiusX = CTUserAvmGroupData.AdvView3D.BottomFrontRightRadiusX;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomFrontRightRadiusY = CTUserAvmGroupData.AdvView3D.BottomFrontRightRadiusY;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomBackLeftRadiusX = CTUserAvmGroupData.AdvView3D.BottomBackLeftRadiusX;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomBackLeftRadiusY = CTUserAvmGroupData.AdvView3D.BottomBackLeftRadiusY;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomBackRightRadiusX = CTUserAvmGroupData.AdvView3D.BottomBackRightRadiusX;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomBackRightRadiusY = CTUserAvmGroupData.AdvView3D.BottomBackRightRadiusY;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopToBottomRadiusH = CTUserAvmGroupData.AdvView3D.TopToBottomRadiusH;
                Cfg.View.ModelCfg.AdvanceModelCfg.TopToBottomRadiusV = CTUserAvmGroupData.AdvView3D.TopToBottomRadiusV;
                Cfg.View.ModelCfg.AdvanceModelCfg.BottomCornerIntervalNum = CTUserAvmGroupData.AdvView3D.BottomCornerIntervalNum;
                Cfg.View.ModelCfg.AdvanceModelCfg.PillarIntervalNum = CTUserAvmGroupData.AdvView3D.PillarIntervalNum;
                Cfg.View.ModelCfg.AdvanceModelCfg.ViewCenter.X = CTUserAvmGroupData.AdvView3D.CenterPositionX;
                Cfg.View.ModelCfg.AdvanceModelCfg.ViewCenter.Y = CTUserAvmGroupData.AdvView3D.CenterPositionY;
                Cfg.View.ModelCfg.AdvanceModelCfg.RotationAngle = CTUserAvmGroupData.AdvView3D.RotationAngle;

                Cfg.View.DisplayWidth = CTUserAvmGroupData.AdvView3D.DisplayWidth;
                Cfg.View.DisplayHeight = CTUserAvmGroupData.AdvView3D.DisplayHeight;
                Cfg.View.DisplayRotation = CAL_ROTATION_ENUM_LUT[CTUserAvmGroupData.AdvView3D.DisplayRotation];
                Cfg.View.AutoVoutOrderEnable = CTUserAvmGroupData.AdvView3D.AutoVoutOrderEnable;
            }else {
                //Note # do nothing
            }
            Rval |= AmbaWrap_memcpy(&Cfg.View.VirtualCam, &CTUserAvmGroupData.VirtualCam3D, sizeof(Cfg.View.VirtualCam));
            Rval |= AmbaWrap_memcpy(&Cfg.View.Blend, &CTUserAvmGroupData.Blend, sizeof(Cfg.View.Blend));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }

            LoadCarOsdMask = 0U;
            for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
                if ( (Cfg.View.Blend.ReferenceCarVoutPosition[i] == 1U) && (LoadCarOsdMask == 0U) ) {
                    if ( CTUserAvmGroupData.OsdMask.FromFile == 1U ) {
                        Rval |= pAvmCbFeedOSDMaskFunc(CTUserAvmGroupData.OsdMask.Width, CTUserAvmGroupData.OsdMask.Height, CTUserAvmGroupData.OsdMask.TablePath, CTUserAvmGroupData.OsdMask.Table);
                    } else {
                        //Note # Custom Cb
                    }
                    LoadCarOsdMask = 1U;
                    Cfg.CarVoutOSD.Pattern = CTUserAvmGroupData.OsdMask.Table;
                    Cfg.CarVoutOSD.VoutStartX = CTUserAvmGroupData.OsdMask.StartX;
                    Cfg.CarVoutOSD.VoutStartY = CTUserAvmGroupData.OsdMask.StartY;
                    Cfg.CarVoutOSD.Width = CTUserAvmGroupData.OsdMask.Width;
                    Cfg.CarVoutOSD.Height = CTUserAvmGroupData.OsdMask.Height;
                }
                Rval |= CT_AvmFeedLensSpecV2(&CTUserAvmGroupData.Lens[i], &Cfg.Cam[i].Cam.Lens, &LensSpecRealExpect[i], &LensSpecAngle[i], &LensSpecFormula[i]);

                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Cam.Sensor, &CTUserAvmGroupData.Sensor[i], sizeof(Cfg.Cam[i].Cam.Sensor));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Cam.OpticalCenter, &CTUserAvmGroupData.OpticalCenter[i], sizeof(Cfg.Cam[i].Cam.OpticalCenter));
                Cfg.Cam[i].Cam.Pos.X = CTUserAvmGroupData.Camera[i].PositionX;
                Cfg.Cam[i].Cam.Pos.Y = CTUserAvmGroupData.Camera[i].PositionY;
                Cfg.Cam[i].Cam.Pos.Z = CTUserAvmGroupData.Camera[i].PositionZ;
                Cfg.Cam[i].Cam.Rotation = CTUserAvmGroupData.Camera[i].RotateType;
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].VinSensorGeo, &CTUserAvmGroupData.Vin[i], sizeof(Cfg.Cam[i].VinSensorGeo));
                CT_AvmFeedPointMap(&CTUserAvmGroupData.CalibPointMap[i], &CTUserAvmGroupData.AssistancePointMap[i], &Cfg.Cam[i].PointMap);
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].ROI, &CTUserAvmGroupData.Roi[i], sizeof(Cfg.Cam[i].ROI));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Main, &CTUserAvmGroupData.MainSize[i], sizeof(Cfg.Cam[i].Main));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Tile, &CTUserAvmGroupData.TileSize[i], sizeof(Cfg.Cam[i].Tile));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].MinVout, &CTUserAvmGroupData.MinVout[i], sizeof(Cfg.Cam[i].MinVout));
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].MinRoiSize, &CTUserAvmGroupData.MinRoiSize[i], sizeof(Cfg.Cam[i].MinRoiSize));
                Cfg.Cam[i].AutoROI = CTUserAvmGroupData.Auto[i].RoiEnable;
                Cfg.Cam[i].AutoFrontEndRotation = CTUserAvmGroupData.Auto[i].FrontEndRotationEnable;
                Cfg.Cam[i].OptimizeLevel = AVM_OPTIMIZE_LEVEL_LUT[CTUserAvmGroupData.OptimizeLevel[i]];
                Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].InternalCfg, &CTUserAvmGroupData.Internal[i], sizeof(Cfg.Cam[i].InternalCfg));
                if(Rval != CT_OK) {
                    Rval = CT_ERR_1;
                }
                Cfg.Cam[i].WarpCalType = CTUserAvmGroupData.WarpCalType[i];

                CT_Avm3dFeedPlugIn(CTUserAvmGroupData.System.CalibMode, (INT32)CTUserAvmGroupData.CustomCalib2D[i].Enable, CTUserAvmGroupData.System.EnableSavePrecheck, &Cfg.Cam[i].PlugIn);
            }

            Rval |= CT_AvmDump3DCfg(&Cfg);

            //Note # Calibration process
            for (i = 0; i < (UINT32)AMBA_CAL_AVM_CAM_MAX; i++) {
                AMBA_CAL_AVM_CALIB_CFG_V3_s CalibCfg;
                CalibCfg.LenPos = Cfg.Cam[i].Cam.Pos;
                CalibCfg.OpticalCenter = Cfg.Cam[i].Cam.OpticalCenter;
                CalibCfg.FocalLength = CTUserAvmGroupData.Camera[i].FocalLength;
                CalibCfg.CellSize = Cfg.Cam[i].Cam.Sensor.CellSize;
                CalibCfg.pPointMap = &Cfg.Cam[i].PointMap;
                CalibCfg.LenSpec = Cfg.Cam[i].Cam.Lens;

                CT_AvmCalibFeedPlugIn(CTUserAvmGroupData.CustomCalib2D[i].Enable, &CalibCfg.PlugIn);

                Rval |= AmbaCal_DistortionTblMmToPixelV2(Cfg.Cam[i].Cam.Sensor.CellSize, &Cfg.Cam[i].Cam.Lens);

                CalibCfg.RansacCfg.Enable = CTUserAvmGroupData.CalibSetting.EnableRansac;
                CalibCfg.RansacCfg.IterationNum = CTUserAvmGroupData.CalibSetting.IterationNum;
                CalibCfg.RansacCfg.AllowedReprojErr = CTUserAvmGroupData.CalibSetting.AllowedReprojErr;

                Rval |= CT_AvmDumpCalibCfg(&CalibCfg);
                Rval |= AmbaCal_GenV3CalibrationInfo(i, pCTAvmCalibWorkingBuf, &CalibCfg, &CalibrationData[i]);
#ifdef DUMP_UT_RGS_GOLDEN
                {
                    //Note # rgs_GenCalibrationInfo
                    ut_dump_rgs_golden_file("GenCalibrationInfo_In1.bin", &i, sizeof(i));
                    ut_dump_rgs_golden_file("GenCalibrationInfo_In2.bin", &CalibCfg, sizeof(CalibCfg));
                    ut_dump_rgs_golden_file("GenCalibrationInfo_In2.bin", CalibCfg.LenSpec.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                    ut_dump_rgs_golden_file("GenCalibrationInfo_In2.bin", CalibCfg.LenSpec.LensDistoSpec.pRealExpect->pRealTbl, CalibCfg.LenSpec.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                    ut_dump_rgs_golden_file("GenCalibrationInfo_In2.bin", CalibCfg.LenSpec.LensDistoSpec.pRealExpect->pExpectTbl, CalibCfg.LenSpec.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                    ut_dump_rgs_golden_file("GenCalibrationInfo_In2.bin", CalibCfg.pPointMap, sizeof(AMBA_CAL_AVM_POINT_MAP_s));
                    ut_dump_rgs_golden_file("GenCalibrationInfo_Out.bin", &CalibrationData[i], sizeof(AMBA_CAL_AVM_CALIB_DATA_V2_s));
                }
#endif
            }

            Rval |= AmbaWrap_memset(&pCTAvmWorkingBuf->Data3D, 0, sizeof(pCTAvmWorkingBuf->Data3D));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }

            Rval |= AmbaCal_AvmGen3DViewTblV2(CalibrationData, &Cfg, pCTAvmCalibWorkingBuf, &pCTAvmWorkingBuf->Data3D);

#ifdef DUMP_UT_RGS_GOLDEN
            {
                SIZE_t Max3DPreCalSize;

                ut_dump_rgs_golden_file("AvmGet3DWorkSize_Out.bin", &Size, sizeof(Size));

                ut_dump_rgs_golden_file("AvmGen3DViewTbl_In1.bin", &CalibrationData, sizeof(CalibrationData));
                ut_dump_rgs_golden_file("AvmGen3DViewTbl_In2.bin", &Cfg, sizeof(Cfg));
                for (i = 0U; i < AMBA_CAL_AVM_CAM_MAX; i++) {
                    ut_dump_rgs_golden_file("AvmGen3DViewTbl_In2.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                    ut_dump_rgs_golden_file("AvmGen3DViewTbl_In2.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                    ut_dump_rgs_golden_file("AvmGen3DViewTbl_In2.bin", Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length * sizeof(DOUBLE));
                }
                ut_dump_rgs_golden_file("AvmGen3DViewTbl_Out.bin", &(pCTAvmWorkingBuf->Data3D), sizeof(AMBA_CAL_AVM_3D_DATA_s));

                AmbaCal_AvmGetMax3DPreCalSize(&Max3DPreCalSize);
                ut_dump_rgs_golden_file("AvmGetMax3DPreCalSize_Out.bin", &Max3DPreCalSize, sizeof(Max3DPreCalSize));
            }
#endif
        }
    }
    return Rval;
}

static UINT32 CT_AvmDumpMVCfg(const AMBA_CAL_AVM_MV_CFG_V2_s *pCfg)
{
    UINT32 i, Rval;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    pNextMsg = CT_LogPutStr(Msg,"Cam.Cam.Lens.LensDistoType: ");
    pNextMsg = CT_LogPutU32(pNextMsg,(UINT32)pCfg->Cam.Cam.Lens.LensDistoType );
    Rval = CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"Cam.Cam.Lens.LensDistoSpec.pRealExpect->Length: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.Cam.Lens.LensDistoSpec.pRealExpect->Length );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"Cam.Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl:");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    for (i = 0; i < pCfg->Cam.Cam.Lens.LensDistoSpec.pRealExpect->Length; i++) {
        pNextMsg = CT_LogPutDB(Msg,pCfg->Cam.Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl[i]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    pNextMsg = CT_LogPutStr(Msg,"Cam.Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl:");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (i = 0; i < pCfg->Cam.Cam.Lens.LensDistoSpec.pRealExpect->Length; i++) {
        pNextMsg = CT_LogPutDB(Msg,pCfg->Cam.Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl[i]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.OpticalCenter.X: ");
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->Cam.Cam.OpticalCenter.X );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.OpticalCenter.Y: ");
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->Cam.Cam.OpticalCenter.Y );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.Pos.X: ");
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->Cam.Cam.Pos.X );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.Pos.Y: ");
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->Cam.Cam.Pos.Y );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.Pos.Z: ");
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->Cam.Cam.Pos.Z );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.Rotation: ");
    pNextMsg = CT_LogPutU32(pNextMsg,(UINT32)pCfg->Cam.Cam.Rotation );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.Sensor.CellSize: ");
    pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->Cam.Cam.Sensor.CellSize );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.Sensor.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.Cam.Sensor.StartX );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.Sensor.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.Cam.Sensor.StartY );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.Sensor.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.Cam.Sensor.Width );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Cam.Sensor.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.Cam.Sensor.Height );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.VinSensorGeo.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.VinSensorGeo.StartX );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.VinSensorGeo.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.VinSensorGeo.StartY );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.VinSensorGeo.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.VinSensorGeo.Width );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.VinSensorGeo.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.VinSensorGeo.Height );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.VinSensorGeo.HSubSample.FactorDen: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.VinSensorGeo.HSubSample.FactorDen );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.VinSensorGeo.HSubSample.FactorNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.VinSensorGeo.HSubSample.FactorNum );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.VinSensorGeo.VSubSample.FactorDen: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.VinSensorGeo.VSubSample.FactorDen );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.VinSensorGeo.VSubSample.FactorNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.VinSensorGeo.VSubSample.FactorNum );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (i = 0; i < AVM_CALIB_POINT_NUM; i++) {
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].RawPos.X: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].RawPos.X);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].RawPos.Y: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].RawPos.Y);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].WorldPos.X: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].WorldPos.X);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].WorldPos.Y: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].WorldPos.Y);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].WorldPos.Z: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].WorldPos.Z);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.AssistancePointNumber: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.PointMap.AssistancePointNumber );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (i = 0; i < pCfg->Cam.PointMap.AssistancePointNumber; i++) {
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].RawPos.X: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].RawPos.X);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].RawPos.Y: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].RawPos.Y);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].WorldPos.X: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].WorldPos.X);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].WorldPos.Y: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].WorldPos.Y);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PointMap.CalibPoints[");
        pNextMsg = CT_LogPutU32(pNextMsg,i);
        pNextMsg = CT_LogPutStr(Msg,"].WorldPos.Z: ");
        pNextMsg = CT_LogPutDB(pNextMsg,pCfg->Cam.PointMap.CalibPoints[i].WorldPos.Z);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.ROI.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.ROI.StartX );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.ROI.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.ROI.StartY );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.ROI.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.ROI.Width );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.ROI.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.ROI.Height );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Tile.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.Tile.Width );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.Tile.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.Tile.Height );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.OptimizeLevel: ");
    pNextMsg = CT_LogPutU32(pNextMsg,(UINT32)pCfg->Cam.OptimizeLevel );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.InternalCfg.DebugID: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.InternalCfg.DebugID );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.InternalCfg.DmaTh: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.InternalCfg.DmaTh );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.InternalCfg.WaitlineTh: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.InternalCfg.WaitlineTh );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg,"pCfg->Cam.PlugIn.PlugInMode: ");
    pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->Cam.PlugIn.PlugInMode );
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Type: ");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)pCfg->View.Type);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    switch(pCfg->View.Type) {
    case AMBA_CAL_AVM_MV_PERSPECTIVE:
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->StartDegreesX: ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pPerspective->StartDegreesX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->StartDegreesY: ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pPerspective->StartDegreesY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->TotalViewAngleX: ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pPerspective->TotalViewAngleX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->TotalViewAngleY: ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pPerspective->TotalViewAngleY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->DisplayWidth: ");
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Cfg.pPerspective->DisplayWidth );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->DisplayHeight: ");
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Cfg.pPerspective->DisplayHeight );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->ProjectionRotationDegreeH: ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pPerspective->ProjectionRotationDegreeH );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->ProjectionRotationDegreeV: ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pPerspective->ProjectionRotationDegreeV );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->ProjectionRotationDegreeF: ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pPerspective->ProjectionRotationDegreeF );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectUpper.Length: ");
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Cfg.pPerspective->LdeRealExpectUpper.Length );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectUpper.pRealTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (i = 0; i < pCfg->View.Cfg.pPerspective->LdeRealExpectUpper.Length; i++) {
            pNextMsg = CT_LogPutDB(Msg, pCfg->View.Cfg.pPerspective->LdeRealExpectUpper.pRealTbl[i]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectUpper.pExpectTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (i = 0; i < pCfg->View.Cfg.pPerspective->LdeRealExpectUpper.Length; i++) {
            pNextMsg = CT_LogPutDB(Msg, pCfg->View.Cfg.pPerspective->LdeRealExpectUpper.pExpectTbl[i]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectLower.Length: ");
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Cfg.pPerspective->LdeRealExpectLower.Length );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectLower.pRealTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (i = 0; i < pCfg->View.Cfg.pPerspective->LdeRealExpectLower.Length; i++) {
            pNextMsg = CT_LogPutDB(Msg, pCfg->View.Cfg.pPerspective->LdeRealExpectLower.pRealTbl[i]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectLower.pExpectTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (i = 0; i < pCfg->View.Cfg.pPerspective->LdeRealExpectLower.Length; i++) {
            pNextMsg = CT_LogPutDB(Msg, pCfg->View.Cfg.pPerspective->LdeRealExpectLower.pExpectTbl[i]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectLeft.Length: ");
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Cfg.pPerspective->LdeRealExpectLeft.Length );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectLeft.pRealTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (i = 0; i < pCfg->View.Cfg.pPerspective->LdeRealExpectLeft.Length; i++) {
            pNextMsg = CT_LogPutDB(Msg, pCfg->View.Cfg.pPerspective->LdeRealExpectLeft.pRealTbl[i]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectLeft.pExpectTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (i = 0; i < pCfg->View.Cfg.pPerspective->LdeRealExpectLeft.Length; i++) {
            pNextMsg = CT_LogPutDB(Msg, pCfg->View.Cfg.pPerspective->LdeRealExpectLeft.pExpectTbl[i]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectRight.Length: ");
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Cfg.pPerspective->LdeRealExpectRight.Length );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectRight.pRealTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (i = 0; i < pCfg->View.Cfg.pPerspective->LdeRealExpectRight.Length; i++) {
            pNextMsg = CT_LogPutDB(Msg, pCfg->View.Cfg.pPerspective->LdeRealExpectRight.pRealTbl[i]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeRealExpectRight.pExpectTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (i = 0; i < pCfg->View.Cfg.pPerspective->LdeRealExpectRight.Length; i++) {
            pNextMsg = CT_LogPutDB(Msg, pCfg->View.Cfg.pPerspective->LdeRealExpectRight.pExpectTbl[i]);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeOpticalCenter.X: ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pPerspective->LdeOpticalCenter.X );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg,"pCfg->View.Cfg.pPerspective->LdeOpticalCenter.Y: ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pPerspective->LdeOpticalCenter.Y );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        break;
    case AMBA_CAL_AVM_MV_EQUAL_DISTANCE:
        pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pEqualDistance->StartDegreesX:  ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pEqualDistance->StartDegreesX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pEqualDistance->StartDegreesY:  ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pEqualDistance->StartDegreesY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pEqualDistance->TotalViewAngleX:  ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pEqualDistance->TotalViewAngleX );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pEqualDistance->TotalViewAngleY:  ");
        pNextMsg = CT_LogPutDB(pNextMsg,   pCfg->View.Cfg.pEqualDistance->TotalViewAngleY );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pEqualDistance->DisplayWidth:  ");
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Cfg.pEqualDistance->DisplayWidth );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pEqualDistance->DisplayHeight:  ");
        pNextMsg = CT_LogPutU32(pNextMsg,   pCfg->View.Cfg.pEqualDistance->DisplayHeight );
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        break;
    case AMBA_CAL_AVM_MV_LDC:
        pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pLDC->CompensateRatio: ");
        pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.Cfg.pLDC->CompensateRatio);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        break;
    case AMBA_CAL_AVM_MV_FLOOR:
    default:
        //Note # don nothing
        break;
    }
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Direction.X: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Cam.Direction.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Direction.Y: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Cam.Direction.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Direction.Z: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Cam.Direction.Z);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    return Rval;
}

static inline void CT_AvmMainviewFeedPlugIn(INT32 UseCustomCalib, UINT32 EnableSavePrecheck, AMBA_CAL_AVM_MV_PLUG_IN_s *pPlugIn)
{
    pPlugIn->MsgReciver = pCTAvmCbMsgReciver;

    pPlugIn->PlugInMode = 0U;
    pPlugIn->PlugInMode |= (UINT32)AVM_MV_PLUGIN_MODE_MSG_RECEIVER;

    if ( UseCustomCalib != 0 ) {
        pPlugIn->PlugInMode |= (UINT32)AVM_MV_PLUGIN_MODE_CUSTOM_CALIB;
        pPlugIn->pInsertCustomCalibMat = p2DAvmCbFeedCustomMat;
    }

    if ( EnableSavePrecheck != 0U ) {
        pPlugIn->PlugInMode |= (UINT32)AVM_MV_PLUGIN_MODE_SAVE_PRECHECK;
        pPlugIn->pSavePreCheckData = pAvmCbSavePreCheckData;
    }
    return;
}

static UINT32 CT_AvmMainviewExecute(void)
{
    UINT32 Rval;
    UINT32 AvmCheckGroupRval = CT_OK;
    UINT32 AvmCheckOptionalGroupRval = CT_OK;
    static const UINT8 NecessaryGroupMainView[] = {
        (UINT8) AMBA_CT_AVM_SYSTEM,
        (UINT8) AMBA_CT_AVM_MAIN_VIEW,
        (UINT8) AMBA_CT_AVM_LENS,
        (UINT8) AMBA_CT_AVM_SENSOR,
        (UINT8) AMBA_CT_AVM_OPTICAL_CENTER,
        (UINT8) AMBA_CT_AVM_CAMERA,
        (UINT8) AMBA_CT_AVM_VIN,
        (UINT8) AMBA_CT_AVM_ROI,
        (UINT8) AMBA_CT_AVM_TILE_SIZE,

    };
    AvmCheckGroupRval = CT_AvmCheckGroup(NecessaryGroupMainView, CT_sizeT_to_U32(sizeof(NecessaryGroupMainView)/sizeof(NecessaryGroupMainView[0])));
    AvmCheckOptionalGroupRval = CT_AvmCheckOptionalGroup((UINT8)AMBA_CT_AVM_CALIB_POINTS, (UINT8)AMBA_CT_AVM_2D_CUSTOM_CALIB);
    if ( (0U != AvmCheckGroupRval) || (0U != AvmCheckOptionalGroupRval)) {
        Rval = CT_ERR_1;
    } else {
        AMBA_CAL_AVM_MV_CFG_V2_s Cfg = { // Note # To fix Coverity MISAR C
            .View = {
                .Type = AMBA_CAL_AVM_BIRDVIEW,
                .Cfg = {
                    .pPerspective = NULL,
                    .pEqualDistance = NULL,
                    .pLDC = NULL,
                    .pFloor = NULL,
                },
            },
            .Cam = {
                .Cam = {
                    .Lens = {
                        .LensDistoType = AMBA_CAL_LD_REAL_EXPECT_TBL,
                        .LensDistoUnit = AMBA_CAL_LD_MM,
                        .LensDistoSpec = {
                            .pRealExpect = NULL,
                            .pAngle = NULL,
                            .pRealExpectFormula = NULL,
                            .pAngleFormula = NULL,
                            .pUserDefPinhoFunc = NULL,
                            .pUserDefAngleFunc = NULL,
                        },
                    },
                    .Sensor = {
                        .CellSize = 0.0,
                        .StartX = 0U,
                        .StartY = 0U,
                        .Width = 0U,
                        .Height = 0U,
                    },
                },
            },
        };
        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect;
        AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle;
        AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula;
        AMBA_CAL_AVM_CALIB_DATA_s CalibrationData[1];
        SIZE_t Size;
        static const AMBA_CAL_AVM_OPTIMIZE_LEVEL_e AVM_OPTIMIZE_LEVEL_LUT[5] =
            {   AMBA_CAL_AVM_OPTIMIZE_LV_0,
                AMBA_CAL_AVM_OPTIMIZE_LV_1, //Note # optimize OC
                AMBA_CAL_AVM_OPTIMIZE_LV_2, //Note # optimize H matrix
                AMBA_CAL_AVM_OPTIMIZE_LV_3, //Note # optimize OC + H matrix
                AMBA_CAL_AVM_OPTIMIZE_LV_MAX
            };

        CT_CheckRval(AmbaWrap_memset(CalibrationData[0].Calib2DMatrix, 0x0, sizeof(DOUBLE)*9U), "AmbaWrap_memset", __func__);
        CalibrationData[0].Calib2DMatrixZSign = 0;
        Rval = AmbaCal_AvmGetMainViewWorkSize(&Size);

        if (CTAvmCalibWorkingBufSize < Size) {
            Rval |= CT_ERR_1;
        } else {
            Rval |= AmbaWrap_memset(&Cfg, 0x0, sizeof(Cfg));
            Rval |= CT_AvmFeedLensSpecV2(&CTUserAvmGroupData.Lens[0], &Cfg.Cam.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);
            Cfg.Cam.Cam.Sensor = CTUserAvmGroupData.Sensor[0];
            Cfg.Cam.Cam.OpticalCenter = CTUserAvmGroupData.OpticalCenter[0];
            Cfg.Cam.Cam.Pos.X = CTUserAvmGroupData.Camera[0].PositionX;
            Cfg.Cam.Cam.Pos.Y = CTUserAvmGroupData.Camera[0].PositionY;
            Cfg.Cam.Cam.Pos.Z = CTUserAvmGroupData.Camera[0].PositionZ;
            Cfg.Cam.Cam.Rotation = CTUserAvmGroupData.Camera[0].RotateType;
            Rval |= AmbaWrap_memcpy(&Cfg.Cam.VinSensorGeo, &CTUserAvmGroupData.Vin[0], sizeof(Cfg.Cam.VinSensorGeo));
            CT_AvmFeedPointMap(&CTUserAvmGroupData.CalibPointMap[0], &CTUserAvmGroupData.AssistancePointMap[0], &Cfg.Cam.PointMap);
            Rval |= AmbaWrap_memcpy(&Cfg.Cam.ROI, &CTUserAvmGroupData.Roi[0], sizeof(Cfg.Cam.ROI));
            Rval |= AmbaWrap_memcpy(&Cfg.Cam.Tile, &CTUserAvmGroupData.TileSize[0], sizeof(Cfg.Cam.Tile));
            Cfg.Cam.OptimizeLevel = AVM_OPTIMIZE_LEVEL_LUT[CTUserAvmGroupData.OptimizeLevel[0]];
            Rval |= AmbaWrap_memcpy(&Cfg.Cam.InternalCfg, &CTUserAvmGroupData.Internal[0], sizeof(Cfg.Cam.InternalCfg));
            Rval |= AmbaWrap_memcpy(&Cfg.Cam.Direction, &CTUserAvmGroupData.MainViewDirection, sizeof(Cfg.Cam.Direction));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }
            Cfg.View.Type = CTUserAvmGroupData.MainViewType;
            switch(Cfg.View.Type) {
                case AMBA_CAL_AVM_MV_PERSPECTIVE:
                    Cfg.View.Cfg.pPerspective = &CTUserAvmGroupData.MainViewPerspetCfgV1;
                    break;
                case AMBA_CAL_AVM_MV_EQUAL_DISTANCE:
                    Cfg.View.Cfg.pEqualDistance= &CTUserAvmGroupData.MainViewEqualDistCfg;
                    break;
                case AMBA_CAL_AVM_MV_LDC:
                    Cfg.View.Cfg.pLDC = &CTUserAvmGroupData.MainViewLdcCfg;
                    break;
                case AMBA_CAL_AVM_MV_FLOOR:
                default:
                    Rval |= CT_ERR_1;
                    break;
            }

            CT_AvmMainviewFeedPlugIn((INT32)CTUserAvmGroupData.CustomCalib2D[0].Enable, CTUserAvmGroupData.System.EnableSavePrecheck, &Cfg.Cam.PlugIn);

            Rval |= CT_AvmDumpMVCfg(&Cfg);

            //Note # Calibration process
            {
                INT32 UseCustomCalibMatrix = 0;

                Rval |= AmbaCal_DistortionTblMmToPixelV2(Cfg.Cam.Cam.Sensor.CellSize, &Cfg.Cam.Cam.Lens);

                if ( (Cfg.Cam.PlugIn.PlugInMode & (UINT32)AVM_MV_PLUGIN_MODE_CUSTOM_CALIB) != 0U ) {
                    UseCustomCalibMatrix = 1;
                }
                {
                    AMBA_CAL_LENS_SPEC_s LensSpec;
                    LensSpec.LensDistoType = Cfg.Cam.Cam.Lens.LensDistoType;
                    LensSpec.LensDistoUnit = Cfg.Cam.Cam.Lens.LensDistoUnit;
                    LensSpec.LensDistoSpec.pRealExpect = Cfg.Cam.Cam.Lens.LensDistoSpec.pRealExpect;
                    LensSpec.LensDistoSpec.pAngle = Cfg.Cam.Cam.Lens.LensDistoSpec.pAngle;
                    LensSpec.LensDistoSpec.pRealExpectFormula = Cfg.Cam.Cam.Lens.LensDistoSpec.pRealExpectFormula;
                    LensSpec.LensDistoSpec.pAngleFormula = Cfg.Cam.Cam.Lens.LensDistoSpec.pAngleFormula;
                    Rval |= AmbaCal_GenCalibrationInfo(0, LensSpec, Cfg.Cam.Cam.Pos,
                        Cfg.Cam.Cam.OpticalCenter, &Cfg.Cam.PointMap, UseCustomCalibMatrix, Cfg.Cam.PlugIn.pInsertCustomCalibMat, &CalibrationData[0]);
                }
            }

            Rval |= AmbaWrap_memset(&pCTAvmWorkingBuf->DataMv, 0, sizeof(pCTAvmWorkingBuf->DataMv));
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }

            Rval |= AmbaCal_AvmGenMainViewTblV2(CalibrationData, &Cfg, pCTAvmCalibWorkingBuf, &pCTAvmWorkingBuf->DataMv);
#ifdef DUMP_UT_RGS_GOLDEN
            {
                //Note # rgs_AvmGetMainViewWorkSize
                ut_dump_rgs_golden_file("AvmGetMainViewWorkSize_Out.bin", &Size, sizeof(Size));

                //Note # rgs_AvmGenMainViewTbl
                ut_dump_rgs_golden_file("AvmGenMainViewTbl_In1.bin", &CalibrationData[0], sizeof(CalibrationData[0]));
                ut_dump_rgs_golden_file("AvmGenMainViewTbl_In2.bin", &Cfg, sizeof(AMBA_CAL_AVM_MV_CFG_V2_s));
                ut_dump_rgs_golden_file("AvmGenMainViewTbl_In2.bin", Cfg.View.Cfg.pPerspective, sizeof(AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s));
                ut_dump_rgs_golden_file("AvmGenMainViewTbl_In2.bin", Cfg.Cam.Cam.Lens.LensDistoSpec.pRealExpect, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                ut_dump_rgs_golden_file("AvmGenMainViewTbl_In2.bin", Cfg.Cam.Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl, sizeof(DOUBLE) * Cfg.Cam.Cam.Lens.LensDistoSpec.pRealExpect->Length);
                ut_dump_rgs_golden_file("AvmGenMainViewTbl_In2.bin", Cfg.Cam.Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl, sizeof(DOUBLE) * Cfg.Cam.Cam.Lens.LensDistoSpec.pRealExpect->Length);
                ut_dump_rgs_golden_file("AvmGenMainViewTbl_Out.bin", &pCTAvmWorkingBuf->DataMv, sizeof(AMBA_CAL_AVM_MV_DATA_s));
            }
#endif
        }
    }

    return Rval;
}

UINT32 AmbaCT_AvmExecute(void)
{
    AMBA_CT_AVM_TYPE_e Type = CT_AvmString2Type(CTUserAvmGroupData.System.Type);
    AMBA_CT_AVM_CALIB_MODE_e CalibMode = CT_AvmString2CalibMode(CTUserAvmGroupData.System.CalibMode);
    UINT32 Rval = CT_OK;

    if(CalibMode == AMBA_CT_AVM_CALIB_MODE_PRE_CHECK){
        switch (Type) {
            case AMBA_CT_AVM_TYPE_2D:
                Rval = CT_AvmCheck2DPreCheckData();
                break;
            case AMBA_CT_AVM_TYPE_3D:
                Rval = CT_AvmCheck3DPreCheckData();
                break;
            case AMBA_CT_AVM_TYPE_MV:
                Rval = CT_AvmCheckMainViewPreCheckData();
                break;
            default:
                Rval = CT_ERR_1;
                break;
        }
    } else {
        switch (Type) {
            case AMBA_CT_AVM_TYPE_2D:
                Rval = CT_Avm2dExecute();
                break;
            case AMBA_CT_AVM_TYPE_3D:
                Rval = CT_Avm3dExecute();
                break;
            case AMBA_CT_AVM_TYPE_MV:
                Rval = CT_AvmMainviewExecute();
                break;
            default:
                Rval = CT_ERR_1;
                break;
        }
    }

    return Rval;
}

static UINT32 CT_AvmGetCalibTables(AMBA_CT_AVM_CALIB_DATA_s *pData)
{
    AMBA_CT_AVM_TYPE_e Type = CT_AvmString2Type(CTUserAvmGroupData.System.Type);
    UINT32 Rval = CT_OK;

    pData->Type = Type;
    switch(Type) {
        case AMBA_CT_AVM_TYPE_2D:
            pData->pData2D = &pCTAvmWorkingBuf->Data2D;
            break;
        case AMBA_CT_AVM_TYPE_3D:
            pData->pData3D = &pCTAvmWorkingBuf->Data3D;
            break;
        case AMBA_CT_AVM_TYPE_MV:
            pData->pDataMv = &pCTAvmWorkingBuf->DataMv;
            break;
        default:
            Rval = CT_ERR_1;
            break;
    }
    return Rval;
}

static UINT32 CT_AvmGetCalibPreCheckData(AMBA_CT_AVM_CALIB_DATA_s *pData)
{
    UINT32 Rval = CT_OK;
    pData->pDataPreCheck = &pCTAvmWorkingBuf->DataPreCheckResult;
    return Rval;
}

UINT32 AmbaCT_AvmGetCalibData(AMBA_CT_AVM_CALIB_DATA_s *pData)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_AVM_CALIB_MODE_e CalibMode = CT_AvmString2CalibMode(CTUserAvmGroupData.System.CalibMode);

    pData->Mode = CalibMode;

    switch(CalibMode) {
        case AMBA_CT_AVM_CALIB_MODE_PRE_CHECK:
            Rval |= CT_AvmGetCalibPreCheckData(pData);
            break;
        case AMBA_CT_AVM_CALIB_MODE_NORMAL:
        case AMBA_CT_AVM_CALIB_MODE_PRE_CALC:
        case AMBA_CT_AVM_CALIB_MODE_FAST:
            Rval |= CT_AvmGetCalibTables(pData);
            break;
        default:
            Rval = CT_ERR_1;
            break;
    }
    return Rval;
}

const AMBA_CT_AVM_USER_SETTING_s * AmbaCT_AvmGetUserSetting(void)
{
    return (const AMBA_CT_AVM_USER_SETTING_s *)&CTUserAvmGroupData;
}

