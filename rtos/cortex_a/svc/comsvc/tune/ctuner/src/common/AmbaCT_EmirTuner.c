/**
 *  @file AmbaCT_EmirTuner.c
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
#include "AmbaCT_EmirTuner.h"
#include "AmbaFS.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCT_Logging.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif


typedef struct {
    UINT8 Reserved;
    AMBA_CAL_EM_3IN1_DATA_s Data3in1;
    AMBA_CAL_EM_SV_DATA_s DataSingleView;
    AMBA_CAL_LDC_VIEW_CFG_s DataLdcViewCfg;
    AMBA_CAL_EM_ASP_VIEW_CFG_s DataAspViewCfg;
    AMBA_CAL_EM_CURVED_SURFACE_CFG_s DataCurvedViewCfg;
    struct {
        DOUBLE RealTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE ExceptTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE RealAngleTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE ExceptAngleTable[MAX_LENS_DISTO_TBL_LEN];
    } LensTable[AMBA_CAL_EM_CAM_MAX];
} AMBA_CT_EM_WORKING_BUF_s;

static AMBA_CT_EM_USER_SETTING_s CTUserEmirGroupData GNU_SECTION_NOZEROINIT;
static UINT8 EmTunerValidGroup[AMBA_CT_EM_TUNER_MAX];
static AMBA_CT_EM_WORKING_BUF_s *pEmWorkingBuf = NULL;
static void *pEmCalibWorkingBuf = NULL;
static SIZE_t EmCalibWorkingBufSize = 0;
static UINT32 (*pEmCbMsgReciver)(AMBA_CAL_EM_MSG_TYPE_e Type, AMBA_CAL_EM_CAM_ID_e CamId, const AMBA_CAL_EM_MSG_s *pMsg) = NULL;
static UINT32 (*pEmSvCbMsgReciver)(AMBA_CAL_EM_MSG_TYPE_e Type, const AMBA_CAL_EM_MSG_s *pMsg) = NULL;
static UINT32 (*pEmCbFeedMaskFunc)(UINT32 CamId, UINT32 *pWidth, UINT32 *pHeight, UINT8 *pTable) = NULL;
static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}
static inline void AmbaCT_EmSetGroupValid(AMBA_CT_EM_TUNER_GROUP_s GroupId)
{
    if (GroupId < AMBA_CT_EM_TUNER_MAX) {
        EmTunerValidGroup[GroupId] = 1;
    }
}

void AmbaCT_EmGetSystem(AMBA_CT_EM_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetSystem(const AMBA_CT_EM_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_SYSTEM);
}

void AmbaCT_EmGetCar(AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.Car, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetCar(const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.Car, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_CAR);
}

void AmbaCT_EmGet3in1View(AMBA_CT_EM_3IN1_VIEW_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.View3in1, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSet3in1View(const AMBA_CT_EM_3IN1_VIEW_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.View3in1, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_3IN1_VIEW);
}

void AmbaCT_EmGet3in1Blend(AMBA_CT_EM_3IN1_BLEND_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.Blend3in1, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSet3in1Blend(const AMBA_CT_EM_3IN1_BLEND_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.Blend3in1, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_3IN1_BLEND);
}

void AmbaCT_EmGetSingleView(AMBA_CAL_EM_VIEW_TYPE_e *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.ViewSingleType, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetSingleView(const AMBA_CAL_EM_VIEW_TYPE_e *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.ViewSingleType, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_SINGLE_VIEW);
}

void AmbaCT_EmGetSingleViewAspCfg(AMBA_CT_EM_ASP_POINT_MAP_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.ViewSingleAspCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmGetSingleViewCurvedCfg(AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.ViewSingleCurvedCfg, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetSingleViewAspCfg(const AMBA_CT_EM_ASP_POINT_MAP_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.ViewSingleAspCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_SINGLE_ASP_CFG);
}

void AmbaCT_EmSetSingleViewCurvedCfg(const AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.ViewSingleCurvedCfg, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_SINGLE_CURVED_CFG);
}

void AmbaCT_EmGetVout(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.Vout[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetVout(UINT32 ArrayIndex, const AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.Vout[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_VOUT);
}

void AmbaCT_EmGetMask(UINT32 ArrayIndex, AMBA_CT_EM_MASK_s **pData)
{
    const AMBA_CT_EM_MASK_s *pCTData = &CTUserEmirGroupData.Mask[ArrayIndex];
    CT_CheckRval(AmbaWrap_memcpy(pData, &pCTData, sizeof(AMBA_CT_EM_MASK_s *)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetMask(UINT32 ArrayIndex, const AMBA_CT_EM_MASK_s *pData)
{
    AMBA_CT_EM_MASK_s *pCTData = &CTUserEmirGroupData.Mask[ArrayIndex];
    if (pData != pCTData) {
        CT_CheckRval(AmbaWrap_memcpy(pCTData, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    }
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_MASK);
}

void AmbaCT_EmGetLens(UINT32 ArrayIndex, AMBA_CT_EM_LENS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.Lens[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetLens(UINT32 ArrayIndex, const AMBA_CT_EM_LENS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.Lens[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_LENS);
}

void AmbaCT_EmGetSensor(UINT32 ArrayIndex, AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.Sensor[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetSensor(UINT32 ArrayIndex, const AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.Sensor[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_SENSOR);
}

void AmbaCT_EmGetOpticalCenter(UINT32 ArrayIndex, AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.OpticalCenter[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetOpticalCenter(UINT32 ArrayIndex, const AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.OpticalCenter[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_OPTICAL_CENTER);
}

void AmbaCT_EmGetCamera(UINT32 ArrayIndex, AMBA_CT_EM_CAMERA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.Camera[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetCamera(UINT32 ArrayIndex, const AMBA_CT_EM_CAMERA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.Camera[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_CAMERA);
}

void AmbaCT_EmGetVin(UINT32 ArrayIndex, AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.Vin[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetVin(UINT32 ArrayIndex, const AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.Vin[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_VIN);
}

void AmbaCT_EmGetRoi(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.Roi[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetRoi(UINT32 ArrayIndex, const AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.Roi[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_ROI);
}

void AmbaCT_EmGetTileSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.TileSize[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetTileSize(UINT32 ArrayIndex, const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.TileSize[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_TILE_SIZE);
}

void AmbaCT_EmGetCalibPoints(UINT32 ArrayIndex, AMBA_CT_EM_CALIB_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.CalibPointMap[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetCalibPoints(UINT32 ArrayIndex, const AMBA_CT_EM_CALIB_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.CalibPointMap[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_CALIB_POINTS);
}

void AmbaCT_EmGetAssistancePoints(UINT32 ArrayIndex, AMBA_CT_EM_ASSISTANCE_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.AssistancePointMap[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetAssistancePoints(UINT32 ArrayIndex, const AMBA_CT_EM_ASSISTANCE_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.AssistancePointMap[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_ASSISTANCE_POINTS);
}

void AmbaCT_EmGetOptimize(UINT32 ArrayIndex, UINT8 *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.OptimizeLevel[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetOptimize(UINT32 ArrayIndex, const UINT8 *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.OptimizeLevel[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_OPTIMIZE);
}

void AmbaCT_EmGetInternal(UINT32 ArrayIndex, AMBA_CAL_EM_INTERNAL_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserEmirGroupData.Internal[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_EmSetInternal(UINT32 ArrayIndex, const AMBA_CAL_EM_INTERNAL_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserEmirGroupData.Internal[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_EmSetGroupValid(AMBA_CT_EM_INTERNAL);
}

void AmbaCT_EmGetWorkingBufSize(SIZE_t *pSize)
{
    *pSize = sizeof(AMBA_CT_EM_WORKING_BUF_s);
}

static inline UINT32 CT_EmResetGData2Default(void)
{
    UINT32 Rval;
    UINT32 Index;
    Rval = AmbaWrap_memset(&CTUserEmirGroupData, 0x0, sizeof(CTUserEmirGroupData));
    if(Rval != CT_OK) {
        Rval = CT_ERR_1;
    }

    for (Index = 0; Index < (UINT32)AMBA_CAL_EM_CAM_MAX; Index++) {
        CTUserEmirGroupData.Internal[Index].WaitlineTh = 0;
        CTUserEmirGroupData.Internal[Index].DmaTh = 0;
        CTUserEmirGroupData.Internal[Index].DebugID = 0;
        CTUserEmirGroupData.Internal[Index].ExtrapolatePlaneBoundary = 0;
        CTUserEmirGroupData.Internal[Index].RefineDmaEnable = 0;
        CTUserEmirGroupData.Internal[Index].RefineWaitlineEnable = 0;
        CTUserEmirGroupData.Internal[Index].RefineOutOfRoiEnable = 1;
        CTUserEmirGroupData.Internal[Index].RefineVFlipEnable = 0;
        CTUserEmirGroupData.Internal[Index].RepeatUnusedArea = 0;
        CTUserEmirGroupData.Internal[Index].PointMapHFlipEnable = 0;
        CTUserEmirGroupData.Internal[Index].AutoRoiEnable = 0;
        CTUserEmirGroupData.Lens[Index].pRealTable = &pEmWorkingBuf->LensTable[Index].RealTable[0];
        CTUserEmirGroupData.Lens[Index].pExceptTable = &pEmWorkingBuf->LensTable[Index].ExceptTable[0];
        CTUserEmirGroupData.Lens[Index].pRealAngleTable = &pEmWorkingBuf->LensTable[Index].RealAngleTable[0];
        CTUserEmirGroupData.Lens[Index].pExceptAngleTable = &pEmWorkingBuf->LensTable[Index].ExceptAngleTable[0];
    }
    CTUserEmirGroupData.View3in1.MirrorFlipCfg = EMIR_VIN_FLIP;
    return Rval;
}

static void CT_EmCastVoidPoint2WorkingBuf(void * const *VoidAddr, AMBA_CT_EM_WORKING_BUF_s **WorkBufAddr)
{
    CT_CheckRval(AmbaWrap_memcpy(WorkBufAddr, VoidAddr, sizeof(AMBA_CT_EM_WORKING_BUF_s *)), "AmbaWrap_memcpy", __func__);
}

UINT32 AmbaCT_EmTunerInit(const AMBA_CT_EM_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    if (pInitCfg->WorkingBufSize <  sizeof(AMBA_CT_EM_WORKING_BUF_s)) {
        pNextMsg = CT_LogPutStr(Msg, "Working buffer size ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(pInitCfg->WorkingBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, " incorrect, necessary size: ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(sizeof(AMBA_CT_EM_WORKING_BUF_s)));
        Rval = CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        AmbaPrint_PrintUInt5("[cTuner] Working buffer size  %d incorrect, necessary size: %d", (UINT32)pInitCfg->WorkingBufSize, (UINT32)sizeof(AMBA_CT_EM_WORKING_BUF_s), 0U, 0U, 0U);
        Rval |= CAL_ERR_1;
    } else {
        Rval = AmbaWrap_memset(EmTunerValidGroup, 0x0, sizeof(EmTunerValidGroup));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        CT_EmCastVoidPoint2WorkingBuf(&pInitCfg->pWorkingBuf, &pEmWorkingBuf);
        pEmCbMsgReciver = pInitCfg->CbMsgReciver;
        pEmSvCbMsgReciver = pInitCfg->SvCbMsgReciver;
        pEmCbFeedMaskFunc = pInitCfg->CbFeedMaskFunc;
        pEmCalibWorkingBuf = pInitCfg->pCalibWorkingBuf;
        EmCalibWorkingBufSize = pInitCfg->CalibWorkingBufSize;
        Rval |= CT_EmResetGData2Default();
    }
    return Rval;
}

static AMBA_CT_EM_TYPE_e CT_EmString2Type(const char *pType)
{
    AMBA_CT_EM_TYPE_e Rval;
    if (0 == CT_strcmp(pType, "3IN1")) {
        Rval = AMBA_CT_EM_TYPE_3IN1;
    } else if (0 == CT_strcmp(pType, "SV")) {
        Rval = AMBA_CT_EM_TYPE_SINGLE_VIEW;
    } else {
        Rval = AMBA_CT_EM_TYPE_MAX;
    }
    return Rval;
}

UINT8 AmbaCT_EmGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_EM_TUNER_MAX) ? EmTunerValidGroup[GroupId] : 0U;
}

static const char* AmbaCT_EmGetGroupName(UINT8 GroupId)
{
    const char *Rval = "CALIB_TUNER_UNKNOWN_GROUP";
    static const struct {
        const char *const GroupName;
        AMBA_CT_EM_TUNER_GROUP_s GroupId;
    } GroupLut[] = {
        {"AMBA_CT_EM_SYSTEM", AMBA_CT_EM_SYSTEM},
        {"AMBA_CT_EM_CAR", AMBA_CT_EM_CAR},
        {"AMBA_CT_EM_3IN1_VIEW", AMBA_CT_EM_3IN1_VIEW},
        {"AMBA_CT_EM_SINGLE_VIEW", AMBA_CT_EM_SINGLE_VIEW},
        {"AMBA_CT_EM_SINGLE_ASP_CFG", AMBA_CT_EM_SINGLE_ASP_CFG},
        {"AMBA_CT_EM_3IN1_BLEND", AMBA_CT_EM_3IN1_BLEND},
        {"AMBA_CT_EM_VOUT", AMBA_CT_EM_VOUT},
        {"AMBA_CT_EM_MASK", AMBA_CT_EM_MASK},
        {"AMBA_CT_EM_LENS", AMBA_CT_EM_LENS},
        {"AMBA_CT_EM_SENSOR", AMBA_CT_EM_SENSOR},
        {"AMBA_CT_EM_OPTICAL_CENTER", AMBA_CT_EM_OPTICAL_CENTER},
        {"AMBA_CT_EM_CAMERA", AMBA_CT_EM_CAMERA},
        {"AMBA_CT_EM_VIN", AMBA_CT_EM_VIN},
        {"AMBA_CT_EM_ROI", AMBA_CT_EM_ROI},
        {"AMBA_CT_EM_TILE_SIZE", AMBA_CT_EM_TILE_SIZE},
        {"AMBA_CT_EM_CALIB_POINTS", AMBA_CT_EM_CALIB_POINTS},
        {"AMBA_CT_EM_ASSISTANCE_POINTS", AMBA_CT_EM_ASSISTANCE_POINTS},
        {"AMBA_CT_EM_OPTIMIZE", AMBA_CT_EM_OPTIMIZE},
        {"AMBA_CT_EM_INTERNAL", AMBA_CT_EM_INTERNAL},
    };

    for (UINT32 Index = 0U; Index < (sizeof(GroupLut)/sizeof(GroupLut[0])); Index++) {
        if (GroupId == (UINT8) GroupLut[Index].GroupId) {
            Rval = GroupLut[Index].GroupName;
            break;
        }
    }
    return (const char *const) Rval;
}

static UINT32 CT_EmCheckGroup(const UINT8 *pNecessaryGroup, UINT32 GroupLen)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 Index;
    for (Index = 0U; Index < GroupLen; Index++) {
        if (EmTunerValidGroup[pNecessaryGroup[Index]] == 0U) {
            pNextMsg = CT_LogPutStr(Msg, "Lose Group ");
            pNextMsg = CT_LogPutStr(pNextMsg, AmbaCT_EmGetGroupName(pNecessaryGroup[Index]));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CAL_ERR_1;
        }
    }
    return Rval;
}

static UINT32 CT_EmFeedMask(AMBA_CAL_EM_CAM_ID_e CamId, AMBA_CT_EM_MASK_s *pMaskInfo)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    if (((pMaskInfo->Width * pMaskInfo->Height) > MAX_EM_MASK_TABLE) || ((pMaskInfo->Width * pMaskInfo->Height) == 0U)) {
        AmbaPrint_PrintUInt5("[cTuner] ERROR: Emir Mask size %d x %d (Max %d)", pMaskInfo->Width, pMaskInfo->Height, MAX_EM_MASK_TABLE, 0U, 0U);
        Rval = CAL_ERR_1;
    } else if (pMaskInfo->FromFile != 0U) {
        AMBA_FS_FILE *Fd;
        Fd = CT_fopen(pMaskInfo->TablePath, "rb");
        if (Fd == NULL) {
            pNextMsg = CT_LogPutStr(Msg, "Open file fail: ");
            pNextMsg = CT_LogPutStr(pNextMsg, pMaskInfo->TablePath);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            AmbaPrint_PrintStr5("[cTuner] ERROR: Emir open file fail: ", pMaskInfo->TablePath, NULL, NULL, NULL, NULL);
            Rval |= CAL_ERR_1;
        } else {
            //Note # CT_fread return the number of items successfully read and should be 1 for below line
            UINT32 FsCount = CT_sizeT_to_U32(sizeof(UINT8));
            if (CT_fread(pMaskInfo->Table, pMaskInfo->Width * pMaskInfo->Height, FsCount, Fd) != FsCount) {
                AmbaPrint_PrintStr5("[cTuner] ERROR: Emir read file fail: ", pMaskInfo->TablePath, NULL, NULL, NULL, NULL);
                Rval |= CAL_ERR_1;
            }
            if (CT_fclose(Fd) != OK) {
                Rval |= CAL_ERR_1;
            }
        }
    } else {
        if (pEmCbFeedMaskFunc != NULL) {
            Rval = pEmCbFeedMaskFunc((UINT32)CamId, &pMaskInfo->Width, &pMaskInfo->Height, pMaskInfo->Table);
        } else {
            //TODO #
        }
    }

    return Rval;
}

static UINT32 CT_EmFeedLensSpec(const AMBA_CT_EM_LENS_s *pIn,
    AMBA_CAL_LENS_SPEC_s *pOut,
    AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRealExpectBuf,
    AMBA_CAL_LENS_DST_ANGLE_s *pAngleBuf,
    AMBA_CAL_LENS_DST_FORMULA_s *pFormulaBuf)
{
    UINT32 Rval = CAL_OK;
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
        pAngleBuf->pRealTbl = pIn->pRealAngleTable;
        pAngleBuf->pAngleTbl = pIn->pExceptAngleTable;
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
        Rval |= CAL_ERR_1;
        break;
    }
    return Rval;
}


static inline void CT_EmFeedPointMap(const AMBA_CT_EM_CALIB_POINTS_s *pInCalibPoints,
    const AMBA_CT_EM_ASSISTANCE_POINTS_s *pInAssistancePoints,
    AMBA_CAL_EM_POINT_MAP_s *pOut)
{
    UINT32 Index;
    for (Index = 0; Index < EMIR_CALIB_POINT_NUM; Index++) {
        pOut->CalibPoints[Index].WorldPos.X = pInCalibPoints->WorldPositionX[Index];
        pOut->CalibPoints[Index].WorldPos.Y = pInCalibPoints->WorldPositionY[Index];
        pOut->CalibPoints[Index].WorldPos.Z = pInCalibPoints->WorldPositionZ[Index];
        pOut->CalibPoints[Index].RawPos.X = pInCalibPoints->RawPositionX[Index];
        pOut->CalibPoints[Index].RawPos.Y = pInCalibPoints->RawPositionY[Index];
    }
    for (Index = 0; Index < pInAssistancePoints->Number; Index++) {
        pOut->AssistancePoints[Index].WorldPos.X = pInAssistancePoints->WorldPositionX[Index];
        pOut->AssistancePoints[Index].WorldPos.Y = pInAssistancePoints->WorldPositionY[Index];
        pOut->AssistancePoints[Index].WorldPos.Z = pInAssistancePoints->WorldPositionZ[Index];
        pOut->AssistancePoints[Index].RawPos.X = pInAssistancePoints->RawPositionX[Index];
        pOut->AssistancePoints[Index].RawPos.Y = pInAssistancePoints->RawPositionY[Index];
    }
    pOut->AssistancePointNumber = pInAssistancePoints->Number;

}

static void CT_CastUINT2OptimizeLevel(UINT8 InUint, AMBA_CAL_EM_OPTIMIZE_LEVEL_e *OutEnum)
{
    switch(InUint) {
        case 0U:
            *OutEnum = AMBA_CAL_EM_OPTIMIZE_LV_0;
            break;
        case 1U:
            *OutEnum = AMBA_CAL_EM_OPTIMIZE_LV_1;
            break;
        case 2U:
            *OutEnum = AMBA_CAL_EM_OPTIMIZE_LV_2;
            break;
        case 3U:
            *OutEnum = AMBA_CAL_EM_OPTIMIZE_LV_3;
            break;
        case 4U:
            *OutEnum = AMBA_CAL_EM_OPTIMIZE_LV_MAX;
            break;
        default:
            /* UnSupported Type*/
            *OutEnum = AMBA_CAL_EM_OPTIMIZE_LV_0;
            break;
    }
}

static UINT32 CT_EmDump3in1Cfg(void)
{
    UINT32 i, J, Rval;
    AMBA_CAL_EM_3IN1_CFG_s Cfg;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula[AMBA_CAL_EM_CAM_MAX];
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    Rval = AmbaWrap_memset(&Cfg, 0x0, sizeof(Cfg));
    Rval |= AmbaWrap_memcpy(&Cfg.Car, &CTUserEmirGroupData.Car, sizeof(Cfg.Car));
    if(Rval != CT_OK) {
        Rval = CT_ERR_1;
    }

    Cfg.View.ViewStartPos.X = CTUserEmirGroupData.View3in1.StartPosX;
    Cfg.View.ViewStartPos.Y = CTUserEmirGroupData.View3in1.StartPosY;
    Cfg.View.ViewStartPos.Z = CTUserEmirGroupData.View3in1.StartPosZ;
    Cfg.View.ViewWidth = CTUserEmirGroupData.View3in1.Width;
    Cfg.View.DistanceOfStitchPlane = CTUserEmirGroupData.View3in1.DistanceOfStitchPlane;
    Cfg.View.BackViewScale = CTUserEmirGroupData.View3in1.BackViewScale;
    Cfg.View.BackViewVerShift = CTUserEmirGroupData.View3in1.BackViewVerShift;
    Cfg.View.LeftViewHorShift = CTUserEmirGroupData.View3in1.LeftViewHorShift;
    Cfg.View.RightViewHorShift = CTUserEmirGroupData.View3in1.RightViewHorShift;
    Cfg.View.ValidVoutLowerBound = CTUserEmirGroupData.View3in1.ValidVoutLowerBound;
    Rval |= AmbaWrap_memcpy(&Cfg.View.BlendLeft, &CTUserEmirGroupData.Blend3in1.Left, sizeof(Cfg.View.BlendLeft));
    Rval |= AmbaWrap_memcpy(&Cfg.View.BlendRight, &CTUserEmirGroupData.Blend3in1.Right, sizeof(Cfg.View.BlendRight));
    Rval |= AmbaWrap_memcpy(&Cfg.View.VoutAreaLeft, &CTUserEmirGroupData.Vout[AMBA_CAL_EM_CAM_LEFT],sizeof(Cfg.View.VoutAreaLeft));
    Rval |= AmbaWrap_memcpy(&Cfg.View.VoutAreaRight, &CTUserEmirGroupData.Vout[AMBA_CAL_EM_CAM_RIGHT],sizeof(Cfg.View.VoutAreaRight));
    Rval |= AmbaWrap_memcpy(&Cfg.View.VoutAreaBack, &CTUserEmirGroupData.Vout[AMBA_CAL_EM_CAM_BACK],sizeof(Cfg.View.VoutAreaBack));
    Cfg.View.MaskLeft.Width = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_LEFT].Width;
    Cfg.View.MaskLeft.Height = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_LEFT].Height;
    Cfg.View.MaskLeft.pMaskTbl = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_LEFT].Table;
    Cfg.View.MaskRight.Width = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_RIGHT].Width;
    Cfg.View.MaskRight.Height = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_RIGHT].Height;
    Cfg.View.MaskRight.pMaskTbl = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_RIGHT].Table;
    Cfg.View.MaskBack.Width = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_BACK].Width;
    Cfg.View.MaskBack.Height = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_BACK].Height;
    Cfg.View.MaskBack.pMaskTbl = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_BACK].Table;
    if(Rval != CT_OK) {
        Rval = CT_ERR_1;
    }

    for (i = 0U; i < (UINT32)AMBA_CAL_EM_CAM_MAX; i++) {
        Rval |= CT_EmFeedLensSpec(&CTUserEmirGroupData.Lens[i], &Cfg.Cam[i].Cam.Lens, &LensSpecRealExpect[i], &LensSpecAngle[i], &LensSpecFormula[i]);
        Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Cam.Sensor, &CTUserEmirGroupData.Sensor[i], sizeof(Cfg.Cam[i].Cam.Sensor));
        Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Cam.OpticalCenter, &CTUserEmirGroupData.OpticalCenter[i], sizeof(Cfg.Cam[i].Cam.OpticalCenter));
        Cfg.Cam[i].Cam.Pos.X = CTUserEmirGroupData.Camera[i].PositionX;
        Cfg.Cam[i].Cam.Pos.Y = CTUserEmirGroupData.Camera[i].PositionY;
        Cfg.Cam[i].Cam.Pos.Z = CTUserEmirGroupData.Camera[i].PositionZ;
        Cfg.Cam[i].Cam.Rotation = CTUserEmirGroupData.Camera[i].RotateType;
        Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].VinSensorGeo, &CTUserEmirGroupData.Vin[i], sizeof(Cfg.Cam[i].VinSensorGeo));
        Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].ROI, &CTUserEmirGroupData.Roi[i], sizeof(Cfg.Cam[i].ROI));
        Rval |= AmbaWrap_memcpy(&Cfg.Cam[i].Tile, &CTUserEmirGroupData.TileSize[i], sizeof(Cfg.Cam[i].Tile));
        CT_EmFeedPointMap(&CTUserEmirGroupData.CalibPointMap[i], &CTUserEmirGroupData.AssistancePointMap[i], &Cfg.Cam[i].PointMap);
        CT_CastUINT2OptimizeLevel(CTUserEmirGroupData.OptimizeLevel[i], &Cfg.Cam[i].OptimizeLevel);
        Rval |=  AmbaWrap_memcpy(&Cfg.Cam[i].InternalCfg, &CTUserEmirGroupData.Internal[i], sizeof(Cfg.Cam[i].InternalCfg));

        Cfg.Cam[i].PlugIn.MsgReciverCfg.GridStatus = 1;
        Cfg.Cam[i].PlugIn.MsgReciverCfg.PatternError = 1;
        Cfg.Cam[i].PlugIn.MsgReciverCfg.Report = 1;
        Cfg.Cam[i].PlugIn.MsgReciver = pEmCbMsgReciver;

        Cfg.Cam[i].PlugIn.pPointMapAdjuster = NULL;
        Cfg.Cam[i].PlugIn.PlugInMode = 0;
    }
    if(Rval != CT_OK) {
        Rval = CT_ERR_1;
    }

    pNextMsg = CT_LogPutStr(Msg, "Car.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Car.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "Car.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Car.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.ViewStartPos.X: ");
    pNextMsg = CT_LogPutDB(pNextMsg, Cfg.View.ViewStartPos.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.ViewStartPos.Y: ");
    pNextMsg = CT_LogPutDB(pNextMsg, Cfg.View.ViewStartPos.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.ViewStartPos.Z: ");
    pNextMsg = CT_LogPutDB(pNextMsg, Cfg.View.ViewStartPos.Z);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.ViewWidth: ");
    pNextMsg = CT_LogPutDB(pNextMsg, Cfg.View.ViewWidth);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.DistanceOfStitchPlane: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.DistanceOfStitchPlane);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.BackViewScale: ");
    pNextMsg = CT_LogPutDB(pNextMsg, Cfg.View.BackViewScale);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.BackViewVerShift: ");
    pNextMsg = CT_LogPutDB(pNextMsg, Cfg.View.BackViewVerShift);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.LeftViewHorShift: ");
    pNextMsg = CT_LogPutDB(pNextMsg, Cfg.View.LeftViewHorShift);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.RightViewHorShift: ");
    pNextMsg = CT_LogPutDB(pNextMsg, Cfg.View.RightViewHorShift);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.ValidVoutLowerBound: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.ValidVoutLowerBound);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.BlendLeft.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.BlendLeft.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.BlendLeft.Angle: ");
    pNextMsg = CT_LogPutS32(pNextMsg, Cfg.View.BlendLeft.Angle);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.BlendLeft.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.BlendLeft.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.BlendRight.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.BlendRight.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.BlendRight.Angle: ");
    pNextMsg = CT_LogPutS32(pNextMsg, Cfg.View.BlendRight.Angle);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.BlendRight.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.BlendRight.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.VoutAreaLeft.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.VoutAreaLeft.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.VoutAreaLeft.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.VoutAreaLeft.StartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.VoutAreaLeft.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.VoutAreaLeft.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.VoutAreaLeft.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.VoutAreaLeft.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.VoutAreaRight.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.VoutAreaRight.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.VoutAreaRight.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.VoutAreaRight.StartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.VoutAreaRight.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.VoutAreaRight.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.VoutAreaRight.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.VoutAreaRight.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.MaskLeft.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.MaskLeft.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.MaskLeft.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.MaskLeft.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.MaskLeft.pMaskTbl: 0x");
    pNextMsg = CT_LogPutAddr(pNextMsg, Cfg.View.MaskLeft.pMaskTbl);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.MaskRight.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.MaskRight.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.MaskRight.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.MaskRight.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.MaskRight.pMaskTbl: 0x");
    pNextMsg = CT_LogPutAddr(pNextMsg, Cfg.View.MaskRight.pMaskTbl);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.MaskBack.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.MaskBack.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.MaskBack.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, Cfg.View.MaskBack.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "View.MaskBack.pMaskTbl: 0x");
    pNextMsg = CT_LogPutAddr(pNextMsg, Cfg.View.MaskBack.pMaskTbl);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "View.MirrorFlipCfg: ");
    pNextMsg = CT_LogPutU32(pNextMsg, CTUserEmirGroupData.View3in1.MirrorFlipCfg);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    for (i = 0U; i < (UINT32)AMBA_CAL_EM_CAM_MAX; i++) {
        pNextMsg = CT_LogPutStr(Msg, "Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Lens.LensDistoType: ");
        pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)Cfg.Cam[i].Cam.Lens.LensDistoType);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Lens.LensDistoSpec.pRealExpect->Length: ");
        pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (J = 0; J < Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length; J++) {
            if (Cfg.Cam[i].Cam.Lens.LensDistoUnit == AMBA_CAL_LD_PIXEL) {
                pNextMsg = CT_LogPutDB(Msg, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl[J] * Cfg.Cam[i].Cam.Sensor.CellSize);
            } else {
                pNextMsg = CT_LogPutDB(Msg, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl[J]);
            }
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg, "Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl:");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (J = 0; J < Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->Length; J++) {
            if (Cfg.Cam[i].Cam.Lens.LensDistoUnit == AMBA_CAL_LD_PIXEL) {
                pNextMsg = CT_LogPutDB(Msg, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl[J] * Cfg.Cam[i].Cam.Sensor.CellSize);
            } else {
                pNextMsg = CT_LogPutDB(Msg, Cfg.Cam[i].Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl[J]);
            }
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.OpticalCenter.X: ");
        pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].Cam.OpticalCenter.X);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.OpticalCenter.Y: ");
        pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].Cam.OpticalCenter.Y);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Pos.X: ");
        pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].Cam.Pos.X);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Pos.Y: ");
        pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].Cam.Pos.Y);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Pos.Z: ");
        pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].Cam.Pos.Z);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Rotation: ");
        pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)Cfg.Cam[i].Cam.Rotation);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Sensor.CellSize: ");
        pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].Cam.Sensor.CellSize);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Sensor.StartX: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].Cam.Sensor.StartX);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Sensor.StartY: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].Cam.Sensor.StartY);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Sensor.Width: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].Cam.Sensor.Width);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Cam.Sensor.Height: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].Cam.Sensor.Height);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].VinSensorGeo.StartX: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].VinSensorGeo.StartX);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].VinSensorGeo.StartY: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].VinSensorGeo.StartY);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].VinSensorGeo.Width: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].VinSensorGeo.Width);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].VinSensorGeo.Height: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].VinSensorGeo.Height);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].VinSensorGeo.HSubSample.FactorDen: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].VinSensorGeo.HSubSample.FactorDen);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].VinSensorGeo.HSubSample.FactorNum: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].VinSensorGeo.HSubSample.FactorNum);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].VinSensorGeo.VSubSample.FactorDen: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].VinSensorGeo.VSubSample.FactorDen);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].VinSensorGeo.VSubSample.FactorNum: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].VinSensorGeo.VSubSample.FactorNum);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].ROI.StartX: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].ROI.StartX);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].ROI.StartY: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].ROI.StartY);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].ROI.Width: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].ROI.Width);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].ROI.Height: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].ROI.Height);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Tile.Width: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].Tile.Width);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].Tile.Height: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].Tile.Height);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

        for (J = 0; J < EMIR_CALIB_POINT_NUM; J++) {
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].RawPos.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].RawPos.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].RawPos.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].RawPos.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].WorldPos.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].WorldPos.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.Z: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].WorldPos.Z);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }

        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.AssistancePointNumber: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].PointMap.AssistancePointNumber);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (J = 0; J < Cfg.Cam[i].PointMap.AssistancePointNumber; J++) {
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].RawPos.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].RawPos.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].RawPos.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].RawPos.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].WorldPos.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].WorldPos.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
            pNextMsg = CT_LogPutU32(pNextMsg, i);
            pNextMsg = CT_LogPutStr(pNextMsg, "].PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, J);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.Z: ");
            pNextMsg = CT_LogPutDB(pNextMsg, Cfg.Cam[i].PointMap.CalibPoints[J].WorldPos.Z);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }

        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].OptimizeLevel: ");
        pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)Cfg.Cam[i].OptimizeLevel);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].InternalCfg.DebugID: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].InternalCfg.DebugID);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].InternalCfg.DmaTh: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].InternalCfg.DmaTh);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].InternalCfg.WaitlineTh: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].InternalCfg.WaitlineTh);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].InternalCfg.raw_pos_horflip_enable: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].InternalCfg.PointMapHFlipEnable);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].InternalCfg.out_of_roi_refine_enable: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].InternalCfg.RefineOutOfRoiEnable);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Cfg.Cam[");
        pNextMsg = CT_LogPutU32(pNextMsg, i);
        pNextMsg = CT_LogPutStr(pNextMsg, "].InternalCfg.auto_roi_refine_enable: ");
        pNextMsg = CT_LogPutU32(pNextMsg, Cfg.Cam[i].InternalCfg.AutoRoiEnable);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        //TODO # Echo plugin
    }

    return Rval;
}

static UINT32 CT_EmDumpSingleViewCfg(const AMBA_CAL_EM_SV_CFG_s *pCfg)
{
    UINT32 Rval;
    UINT32 Index;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Lens.LensDistoType: ");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pCfg->Cam.Lens.LensDistoType);
    Rval = CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Lens.LensDistoSpec.pRealExpect->Length: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Cam.Lens.LensDistoSpec.pRealExpect->Length);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl:");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (Index = 0; Index < pCfg->Cam.Lens.LensDistoSpec.pRealExpect->Length; Index++) {
        pNextMsg = CT_LogPutDB(Msg, pCfg->Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl[Index]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl:");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (Index = 0; Index < pCfg->Cam.Lens.LensDistoSpec.pRealExpect->Length; Index++) {
        pNextMsg = CT_LogPutDB(Msg, pCfg->Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl[Index]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.OpticalCenter.X: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Cam.OpticalCenter.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.OpticalCenter.Y: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Cam.OpticalCenter.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Pos.X: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Cam.Pos.X);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Pos.Y: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Cam.Pos.Y);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Pos.Z: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Cam.Pos.Z);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Rotation: ");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pCfg->Cam.Rotation);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.CellSize: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Cam.Sensor.CellSize);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Cam.Sensor.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Cam.Sensor.StartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Cam.Sensor.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Cam.Sensor.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);


    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.StartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.HSubSample.FactorDen: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.HSubSample.FactorDen);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.HSubSample.FactorNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.HSubSample.FactorNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.VSubSample.FactorDen: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.VSubSample.FactorDen);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.VSubSample.FactorNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.VSubSample.FactorNum);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->ROI.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->ROI.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->ROI.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->ROI.StartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->ROI.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->ROI.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->ROI.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->ROI.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Type: ");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pCfg->View.Type);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    switch(pCfg->View.Type) {
        case AMBA_CAL_EM_LDC:
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pLDC->LineStraightStrX: ");
            pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.Cfg.pLDC->LineStraightStrX);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pLDC->LineStraightStrY: ");
            pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.Cfg.pLDC->LineStraightStrY);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pLDC->FovWiderStrX: ");
            pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.Cfg.pLDC->FovWiderStrX);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pLDC->FovWiderStrY: ");
            pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.Cfg.pLDC->FovWiderStrY);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pLDC->DistEvenStrX: ");
            pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.Cfg.pLDC->DistEvenStrX);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pLDC->DistEvenStrY: ");
            pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.Cfg.pLDC->DistEvenStrY);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            break;
        case AMBA_CAL_EM_ASPHERIC:
            for (Index = 0U; Index < 6U; Index++) {
                pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg->pAspheric->PointMap[");
                pNextMsg = CT_LogPutU32(pNextMsg, Index);
                pNextMsg = CT_LogPutStr(pNextMsg, "].Src.X: ");
                pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pAspheric->PointMap[Index].Src.X);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg->pAspheric->PointMap[");
                pNextMsg = CT_LogPutU32(pNextMsg, Index);
                pNextMsg = CT_LogPutStr(pNextMsg, "].Src.Y: ");
                pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pAspheric->PointMap[Index].Src.Y);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg->pAspheric->PointMap[");
                pNextMsg = CT_LogPutU32(pNextMsg, Index);
                pNextMsg = CT_LogPutStr(pNextMsg, "].Dst.X: ");
                pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pAspheric->PointMap[Index].Dst.X);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg->pAspheric->PointMap[");
                pNextMsg = CT_LogPutU32(pNextMsg, Index);
                pNextMsg = CT_LogPutStr(pNextMsg, "].Dst.Y: ");
                pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pAspheric->PointMap[Index].Dst.Y);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            }
            break;
        case AMBA_CAL_EM_CURVED_SURFACE:
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.Z: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.Z);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.Z: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.Z);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosCurveLeft.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosCurveLeft.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosCurveLeft.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosCurveLeft.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosCurveRight.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosCurveRight.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosCurveRight.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosCurveRight.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosLineLeft.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosLineLeft.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosLineLeft.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosLineLeft.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosLineRight.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosLineRight.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosLineRight.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosLineRight.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            break;
        case AMBA_CAL_EM_CURVED_ASPHERIC:
            for (Index = 0U; Index < 6U; Index++) {
                pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg->pAspheric->PointMap[");
                pNextMsg = CT_LogPutU32(pNextMsg, Index);
                pNextMsg = CT_LogPutStr(pNextMsg, "].Src.X: ");
                pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pAspheric->PointMap[Index].Src.X);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg->pAspheric->PointMap[");
                pNextMsg = CT_LogPutU32(pNextMsg, Index);
                pNextMsg = CT_LogPutStr(pNextMsg, "].Src.Y: ");
                pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pAspheric->PointMap[Index].Src.Y);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg->pAspheric->PointMap[");
                pNextMsg = CT_LogPutU32(pNextMsg, Index);
                pNextMsg = CT_LogPutStr(pNextMsg, "].Dst.X: ");
                pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pAspheric->PointMap[Index].Dst.X);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
                pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg->pAspheric->PointMap[");
                pNextMsg = CT_LogPutU32(pNextMsg, Index);
                pNextMsg = CT_LogPutStr(pNextMsg, "].Dst.Y: ");
                pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pAspheric->PointMap[Index].Dst.Y);
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            }
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.Z: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.Focus.Z);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.Z: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->VirtualCam.ProjectPlane.Z);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosCurveLeft.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosCurveLeft.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosCurveLeft.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosCurveLeft.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosCurveRight.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosCurveRight.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosCurveRight.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosCurveRight.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosLineLeft.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosLineLeft.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosLineLeft.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosLineLeft.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosLineRight.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosLineRight.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->View.Cfg.pCurvedSurface->WorldPosLineRight.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->View.Cfg.pCurvedSurface->WorldPosLineRight.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            break;
        default:
            /* UnSupported type */
            break;
    }
    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.VoutArea.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.VoutArea.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.VoutArea.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.VoutArea.StartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.VoutArea.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.VoutArea.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.VoutArea.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.VoutArea.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.PlugIn.PlugInMode: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.PlugIn.PlugInMode);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    if ((pCfg->Calibinfo.Type == AMBA_EM_CALIB_4_POINT) && (pCfg->Calibinfo.p4Point != NULL)) {
        for (Index = 0U; Index < EMIR_CALIB_POINT_NUM; Index++) {
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].RawPos.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->CalibPoints[Index].RawPos.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].RawPos.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->CalibPoints[Index].RawPos.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->CalibPoints[Index].WorldPos.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->CalibPoints[Index].WorldPos.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.CalibPoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.Z: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->CalibPoints[Index].WorldPos.Z);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }

        pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.AssistancePointNumber: ");
        pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Calibinfo.p4Point->AssistancePointNumber);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        for (Index = 0U; Index < pCfg->Calibinfo.p4Point->AssistancePointNumber; Index++) {
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].RawPos.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->AssistancePoints[Index].RawPos.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].RawPos.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->AssistancePoints[Index].RawPos.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.X: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->AssistancePoints[Index].WorldPos.X);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.Y: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->AssistancePoints[Index].WorldPos.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            pNextMsg = CT_LogPutStr(Msg, "pCfg->PointMap.AssistancePoints[");
            pNextMsg = CT_LogPutU32(pNextMsg, Index);
            pNextMsg = CT_LogPutStr(pNextMsg, "].WorldPos.Z: ");
            pNextMsg = CT_LogPutDB(pNextMsg, pCfg->Calibinfo.p4Point->AssistancePoints[Index].WorldPos.Z);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
    }
    pNextMsg = CT_LogPutStr(Msg, "pCfg->OptimizeLevel: ");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pCfg->OptimizeLevel);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->InternalCfg.DebugID: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->InternalCfg.DebugID);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->InternalCfg.DmaTh: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->InternalCfg.DmaTh);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->InternalCfg.WaitlineTh: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->InternalCfg.WaitlineTh);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->InternalCfg.PointMapHFlipEnable: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->InternalCfg.PointMapHFlipEnable);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->InternalCfg.RefineOutOfRoiEnable: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->InternalCfg.RefineOutOfRoiEnable);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->InternalCfg.AutoRoiEnable: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->InternalCfg.AutoRoiEnable);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    return Rval;
}

static UINT32 CT_Em3in1Execute(void)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    static const UINT8 NecessaryGroup3in1[] = {
        (UINT8)AMBA_CT_EM_SYSTEM,
        (UINT8)AMBA_CT_EM_CAR,
        (UINT8)AMBA_CT_EM_3IN1_VIEW,
        (UINT8)AMBA_CT_EM_3IN1_BLEND,
        (UINT8)AMBA_CT_EM_VOUT,
        (UINT8)AMBA_CT_EM_MASK,
        (UINT8)AMBA_CT_EM_LENS,
        (UINT8)AMBA_CT_EM_SENSOR,
        (UINT8)AMBA_CT_EM_OPTICAL_CENTER,
        (UINT8)AMBA_CT_EM_CAMERA,
        (UINT8)AMBA_CT_EM_VIN,
        (UINT8)AMBA_CT_EM_ROI,
        (UINT8)AMBA_CT_EM_TILE_SIZE,
        (UINT8)AMBA_CT_EM_CALIB_POINTS,
        (UINT8)AMBA_CT_EM_ASSISTANCE_POINTS,
        (UINT8)AMBA_CT_EM_OPTIMIZE,
        (UINT8)AMBA_CT_EM_INTERNAL,
    };

    if (CAL_OK != CT_EmCheckGroup(NecessaryGroup3in1, CT_sizeT_to_U32(sizeof(NecessaryGroup3in1)/sizeof(NecessaryGroup3in1[0])))) {
        Rval = CAL_ERR_1;
    } else {
        UINT32 Index;
        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect[AMBA_CAL_EM_CAM_MAX];
        AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle[AMBA_CAL_EM_CAM_MAX];
        AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula[AMBA_CAL_EM_CAM_MAX];
        SIZE_t Size, CalibInfoSize, GenTableSize;
        Rval |= AmbaCal_EmGetCalibInfoBufSize(&CalibInfoSize);
        Rval |= AmbaCal_EmGet3in1VBufSize(&GenTableSize);
        Size = (CalibInfoSize > GenTableSize)? CalibInfoSize: GenTableSize;
        if (EmCalibWorkingBufSize < Size) {
            pNextMsg = CT_LogPutStr(Msg, "Buf Size: ");
            pNextMsg = CT_LogPutU64(pNextMsg, EmCalibWorkingBufSize);
            pNextMsg = CT_LogPutStr(pNextMsg, " < ");
            pNextMsg = CT_LogPutU64(pNextMsg, Size);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            AmbaPrint_PrintUInt5("[cTuner] ERROR: 3in1-Emir curr buf size %d < Max(%d, %d)", CT_sizeT_to_U32(EmCalibWorkingBufSize), CT_sizeT_to_U32(CalibInfoSize), CT_sizeT_to_U32(GenTableSize), 0U, 0U);
            Rval |= CAL_ERR_1;
        } else {
            AMBA_CAL_EM3IN1_GEN_TBL_CFG_V1_s CfgV1;
            Rval |= AmbaWrap_memset(&CfgV1, 0x0, sizeof(CfgV1));

            /* Fill in CfgV1 */
            Rval |= AmbaWrap_memcpy(&CfgV1.Car, &CTUserEmirGroupData.Car, sizeof(CfgV1.Car));
            CfgV1.View.ViewStartPos.X = CTUserEmirGroupData.View3in1.StartPosX;
            CfgV1.View.ViewStartPos.Y = CTUserEmirGroupData.View3in1.StartPosY;
            CfgV1.View.ViewStartPos.Z = CTUserEmirGroupData.View3in1.StartPosZ;
            CfgV1.View.ViewWidth = CTUserEmirGroupData.View3in1.Width;
            CfgV1.View.DistanceOfStitchPlane = CTUserEmirGroupData.View3in1.DistanceOfStitchPlane;
            CfgV1.View.BackViewScale = CTUserEmirGroupData.View3in1.BackViewScale;
            CfgV1.View.BackViewVerShift = CTUserEmirGroupData.View3in1.BackViewVerShift;
            CfgV1.View.LeftViewHorShift = CTUserEmirGroupData.View3in1.LeftViewHorShift;
            CfgV1.View.RightViewHorShift = CTUserEmirGroupData.View3in1.RightViewHorShift;
            CfgV1.View.ValidVoutLowerBound = CTUserEmirGroupData.View3in1.ValidVoutLowerBound;
            CfgV1.View.MirrorFlipCfg = CTUserEmirGroupData.View3in1.MirrorFlipCfg;
            Rval |= AmbaWrap_memcpy(&CfgV1.View.BlendLeft, &CTUserEmirGroupData.Blend3in1.Left, sizeof(CfgV1.View.BlendLeft));
            Rval |= AmbaWrap_memcpy(&CfgV1.View.BlendRight, &CTUserEmirGroupData.Blend3in1.Right, sizeof(CfgV1.View.BlendRight));
            Rval |= AmbaWrap_memcpy(&CfgV1.View.VoutAreaLeft, &CTUserEmirGroupData.Vout[AMBA_CAL_EM_CAM_LEFT],sizeof(CfgV1.View.VoutAreaLeft));
            Rval |= AmbaWrap_memcpy(&CfgV1.View.VoutAreaRight, &CTUserEmirGroupData.Vout[AMBA_CAL_EM_CAM_RIGHT],sizeof(CfgV1.View.VoutAreaRight));
            Rval |= AmbaWrap_memcpy(&CfgV1.View.VoutAreaBack, &CTUserEmirGroupData.Vout[AMBA_CAL_EM_CAM_BACK],sizeof(CfgV1.View.VoutAreaBack));
            CfgV1.View.MaskLeft.Width = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_LEFT].Width;
            CfgV1.View.MaskLeft.Height = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_LEFT].Height;
            CfgV1.View.MaskLeft.pMaskTbl = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_LEFT].Table;
            Rval |= CT_EmFeedMask(AMBA_CAL_EM_CAM_LEFT, &CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_LEFT]);
            CfgV1.View.MaskRight.Width = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_RIGHT].Width;
            CfgV1.View.MaskRight.Height = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_RIGHT].Height;
            CfgV1.View.MaskRight.pMaskTbl = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_RIGHT].Table;
            Rval |= CT_EmFeedMask(AMBA_CAL_EM_CAM_RIGHT, &CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_RIGHT]);
            CfgV1.View.MaskBack.Width = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_BACK].Width;
            CfgV1.View.MaskBack.Height = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_BACK].Height;
            CfgV1.View.MaskBack.pMaskTbl = CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_BACK].Table;
            Rval |= CT_EmFeedMask(AMBA_CAL_EM_CAM_BACK, &CTUserEmirGroupData.Mask[AMBA_CAL_EM_CAM_BACK]);

            if (Rval != CAL_OK) {
                AmbaPrint_PrintUInt5("[cTuner] ERROR: 3in1-Emir FeedMask fail", 0U, 0U, 0U, 0U, 0U);
                Rval = CAL_OK; //Note # Ignore and continue
            }

            for (Index = 0U; Index < (UINT32)AMBA_CAL_EM_CAM_MAX; Index++) {
                Rval |= AmbaWrap_memcpy(&CfgV1.Cam[Index].Cam.Sensor, &CTUserEmirGroupData.Sensor[Index], sizeof(CfgV1.Cam[Index].Cam.Sensor));
                Rval |= AmbaWrap_memcpy(&CfgV1.Cam[Index].Cam.OpticalCenter, &CTUserEmirGroupData.OpticalCenter[Index], sizeof(CfgV1.Cam[Index].Cam.OpticalCenter));
                CfgV1.Cam[Index].Cam.Pos.X = CTUserEmirGroupData.Camera[Index].PositionX;
                CfgV1.Cam[Index].Cam.Pos.Y = CTUserEmirGroupData.Camera[Index].PositionY;
                CfgV1.Cam[Index].Cam.Pos.Z = CTUserEmirGroupData.Camera[Index].PositionZ;
                CfgV1.Cam[Index].Cam.Rotation = CTUserEmirGroupData.Camera[Index].RotateType;
                Rval |= CT_EmFeedLensSpec(&CTUserEmirGroupData.Lens[Index], &CfgV1.Cam[Index].Cam.Lens, &LensSpecRealExpect[Index], &LensSpecAngle[Index], &LensSpecFormula[Index]);
                if ((CfgV1.Cam[Index].Cam.Lens.LensDistoType == AMBA_CAL_LD_REAL_EXPECT_TBL) && (CfgV1.Cam[Index].Cam.Lens.LensDistoUnit != AMBA_CAL_LD_PIXEL)) {
                    Rval |= AmbaCal_DistortionTblMmToPixel(CfgV1.Cam[Index].Cam.Sensor.CellSize, &CfgV1.Cam[Index].Cam.Lens);
                    CfgV1.Cam[Index].Cam.Lens.LensDistoUnit = AMBA_CAL_LD_PIXEL;
                }
                Rval |= AmbaWrap_memcpy(&CfgV1.Cam[Index].VinSensorGeo, &CTUserEmirGroupData.Vin[Index], sizeof(CfgV1.Cam[Index].VinSensorGeo));
                Rval |= AmbaWrap_memcpy(&CfgV1.Cam[Index].ROI, &CTUserEmirGroupData.Roi[Index], sizeof(CfgV1.Cam[Index].ROI));
                Rval |= AmbaWrap_memcpy(&CfgV1.Cam[Index].Tile, &CTUserEmirGroupData.TileSize[Index], sizeof(CfgV1.Cam[Index].Tile));
                Rval |= AmbaWrap_memcpy(&CfgV1.Cam[Index].InternalCfg, &CTUserEmirGroupData.Internal[Index], sizeof(CfgV1.Cam[Index].InternalCfg));

                CfgV1.Cam[Index].PlugIn.MsgReciverCfg.GridStatus = 1;
                CfgV1.Cam[Index].PlugIn.MsgReciverCfg.PatternError = 1;
                CfgV1.Cam[Index].PlugIn.MsgReciverCfg.Report = 1;
                CfgV1.Cam[Index].PlugIn.MsgReciver = pEmCbMsgReciver;

                CfgV1.Cam[Index].PlugIn.pPointMapAdjuster = NULL; //TODO #
                CfgV1.Cam[Index].PlugIn.PlugInMode = 0; //TODO #
            }
            if(Rval != CT_OK) {
                Rval = CT_ERR_1;
            }

            /* Calculate calibration info of each channel */
            for (UINT32 ChId = 0U; ChId < (UINT32)AMBA_CAL_EM_CAM_MAX; ChId ++) {
                AMBA_CAL_EM_CALIB_INFO_CFG_s CalibInfoCfg;
                AMBA_CAL_EM_POINT_MAP_s CurrPointMap;
                const AMBA_CAL_ROI_s *TargetVoutArea = NULL;
                AMBA_CAL_EM_CALIB_INFO_DATA_s HomoMatrixSet;

                Rval |= AmbaWrap_memset(&CurrPointMap, 0, sizeof(CurrPointMap));
                CT_EmFeedPointMap(&CTUserEmirGroupData.CalibPointMap[ChId], &CTUserEmirGroupData.AssistancePointMap[ChId], &CurrPointMap);

                Rval |= AmbaCal_EmGet3in1VoutAreaInfo(ChId, &CfgV1.View, &TargetVoutArea);
                Rval |= AmbaWrap_memcpy(&CalibInfoCfg.VoutArea, TargetVoutArea, sizeof(AMBA_CAL_ROI_s));

                Rval |= AmbaWrap_memcpy(&CalibInfoCfg.Cam, &CfgV1.Cam[ChId].Cam, sizeof(AMBA_CAL_CAM_s));
                CalibInfoCfg.Type = AMBA_CAL_EM_3IN1;
                CalibInfoCfg.Calibinfo.Type = AMBA_EM_CALIB_4_POINT;
                CalibInfoCfg.Calibinfo.p4Point = &CurrPointMap;
                CalibInfoCfg.Calibinfo.pMultiPoint = NULL;
                CT_CastUINT2OptimizeLevel(CTUserEmirGroupData.OptimizeLevel[ChId], &CalibInfoCfg.OptimizeLevel);
                CalibInfoCfg.PointMapHFlipEnable = CTUserEmirGroupData.Internal[ChId].PointMapHFlipEnable;
                CalibInfoCfg.PlugIn.MsgReciverCfg.GridStatus = 1;
                CalibInfoCfg.PlugIn.MsgReciverCfg.PatternError = 1;
                CalibInfoCfg.PlugIn.MsgReciverCfg.Report = 1;
                CalibInfoCfg.PlugIn.MsgReciver = pEmSvCbMsgReciver;
                CalibInfoCfg.PlugIn.PlugInMode = 0; //TODO #

                /* Calculate Homo Matrix */
                Rval |= AmbaWrap_memset(&HomoMatrixSet, 0, sizeof(AMBA_CAL_EM_CALIB_INFO_DATA_s));
                Rval |= AmbaCal_EmGenCalibInfo(&CalibInfoCfg, pEmCalibWorkingBuf, &HomoMatrixSet);

                /* Assign Data for GenWarp Step */
                Rval |= AmbaWrap_memcpy(&CfgV1.Cam[ChId].Cam.OpticalCenter, &HomoMatrixSet.OpticalCenter, sizeof(AMBA_CAL_POINT_DB_2D_s));
                Rval |= AmbaWrap_memcpy(&CfgV1.Cam[ChId].CalibDataWorld2Raw, &HomoMatrixSet.CalibDataWorld2Raw, sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s));
            }

            if (Rval == CAL_OK) {
                Rval = AmbaCal_EmGen3in1VV1(&CfgV1, pEmCalibWorkingBuf, &pEmWorkingBuf->Data3in1);
            } else {
                AmbaPrint_PrintUInt5("[cTuner] ERROR: 3in1-Emir AmbaCal_EmGenCalibInfo fail", 0U, 0U, 0U, 0U, 0U);
            }
        }
        if (Rval != CAL_OK) {
            Rval |= CT_EmDump3in1Cfg();
        }
    }
    return Rval;
}

static inline UINT32 CT_EmFeedAsphericPointMap(const AMBA_CT_EM_ASP_POINT_MAP_s *pInAspPointMap, AMBA_CAL_EM_ASP_VIEW_CFG_s *pOut)
{
    UINT32 Rval = CAL_OK;
    UINT32 Index;
    if ((pInAspPointMap == NULL) || (pOut == NULL)) {
        Rval |= CAL_ERR_1;
    } else {
        CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
        CT_LOG_MSG_s *pNextMsg = Msg;
        for (Index = 0U; Index < 6U; Index++) {
            pOut->PointMap[Index].Src.X = pInAspPointMap->SrcX[Index];
            pOut->PointMap[Index].Src.Y = pInAspPointMap->SrcY[Index];
            pOut->PointMap[Index].Dst.X = pInAspPointMap->DstX[Index];
            pOut->PointMap[Index].Dst.Y = pInAspPointMap->DstY[Index];
            pNextMsg = CT_LogPutDB(Msg, pOut->PointMap[Index].Src.X);
            pNextMsg = CT_LogPutStr(pNextMsg, " ");
            pNextMsg = CT_LogPutDB(pNextMsg, pOut->PointMap[Index].Src.Y);
            pNextMsg = CT_LogPutStr(pNextMsg, " ");
            pNextMsg = CT_LogPutDB(pNextMsg, pOut->PointMap[Index].Dst.X);
            pNextMsg = CT_LogPutStr(pNextMsg, " ");
            pNextMsg = CT_LogPutDB(pNextMsg, pOut->PointMap[Index].Dst.Y);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        }
    }
    return Rval;
}

static inline UINT32 CT_EmFeedVirtualCamCurvedPlane(const AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pInCurved, AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pOut)
{
    UINT32 Rval = CAL_OK;
    if ((pInCurved == NULL) || (pOut == NULL)) {
        Rval |= CAL_ERR_1;
    } else {
        CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
        CT_LOG_MSG_s *pNextMsg = Msg;
        Rval |= AmbaWrap_memcpy(pOut, pInCurved, sizeof(AMBA_CAL_EM_CURVED_SURFACE_CFG_s));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        pNextMsg = CT_LogPutStr(Msg, "Focus: ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->VirtualCam.Focus.X);
        pNextMsg = CT_LogPutStr(pNextMsg, " ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->VirtualCam.Focus.Y);
        pNextMsg = CT_LogPutStr(pNextMsg, " ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->VirtualCam.Focus.Z);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Project Plane: ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->VirtualCam.ProjectPlane.X);
        pNextMsg = CT_LogPutStr(pNextMsg, " ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->VirtualCam.ProjectPlane.Y);
        pNextMsg = CT_LogPutStr(pNextMsg, " ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->VirtualCam.ProjectPlane.Z);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
        pNextMsg = CT_LogPutStr(Msg, "Curve Plane: (");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->WorldPosCurveLeft.X);
        pNextMsg = CT_LogPutStr(pNextMsg, ", ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->WorldPosCurveLeft.Y);
        pNextMsg = CT_LogPutStr(pNextMsg, ") (");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->WorldPosCurveRight.X);
        pNextMsg = CT_LogPutStr(pNextMsg, ", ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->WorldPosCurveRight.Y);
        pNextMsg = CT_LogPutStr(pNextMsg, ") (");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->WorldPosLineLeft.X);
        pNextMsg = CT_LogPutStr(pNextMsg, ", ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->WorldPosLineLeft.Y);
        pNextMsg = CT_LogPutStr(pNextMsg, ") (");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->WorldPosLineRight.X);
        pNextMsg = CT_LogPutStr(pNextMsg, ", ");
        pNextMsg = CT_LogPutDB(pNextMsg, pOut->WorldPosLineRight.Y);
        pNextMsg = CT_LogPutStr(pNextMsg, ")");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    return Rval;
}

static UINT32 CT_EmSingleViewExecute(void)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    static const UINT8 NecessaryGroupSV[] = {
        (UINT8)AMBA_CT_EM_SYSTEM,
        (UINT8)AMBA_CT_EM_SINGLE_VIEW,
        (UINT8)AMBA_CT_EM_VOUT,
        (UINT8)AMBA_CT_EM_LENS,
        (UINT8)AMBA_CT_EM_SENSOR,
        (UINT8)AMBA_CT_EM_OPTICAL_CENTER,
        (UINT8)AMBA_CT_EM_CAMERA,
        (UINT8)AMBA_CT_EM_VIN,
        (UINT8)AMBA_CT_EM_ROI,
        (UINT8)AMBA_CT_EM_TILE_SIZE,
        (UINT8)AMBA_CT_EM_OPTIMIZE,
        (UINT8)AMBA_CT_EM_INTERNAL,
    };

    if (CAL_OK != CT_EmCheckGroup(NecessaryGroupSV, CT_sizeT_to_U32(sizeof(NecessaryGroupSV)/sizeof(NecessaryGroupSV[0])))) {
        Rval = CAL_ERR_1;
    } else {
        AMBA_CAL_EM_SV_CFG_s Cfg;
        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect;
        AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle;
        AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula;
        AMBA_CAL_EM_POINT_MAP_s CalibPointsMap;
        Rval |= AmbaWrap_memset(&Cfg, 0x0, sizeof(Cfg));
        Rval |= CT_EmFeedLensSpec(&CTUserEmirGroupData.Lens[0U], &Cfg.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);
        Rval |= AmbaWrap_memcpy(&Cfg.Cam.Sensor, &CTUserEmirGroupData.Sensor[0U], sizeof(Cfg.Cam.Sensor));
        Rval |= AmbaWrap_memcpy(&Cfg.Cam.OpticalCenter, &CTUserEmirGroupData.OpticalCenter[0U], sizeof(Cfg.Cam.OpticalCenter));
        Cfg.Cam.Pos.X = CTUserEmirGroupData.Camera[0U].PositionX;
        Cfg.Cam.Pos.Y = CTUserEmirGroupData.Camera[0U].PositionY;
        Cfg.Cam.Pos.Z = CTUserEmirGroupData.Camera[0U].PositionZ;
        Cfg.Cam.Rotation = CTUserEmirGroupData.Camera[0U].RotateType;
        Rval |= AmbaWrap_memcpy(&Cfg.VinSensorGeo, &CTUserEmirGroupData.Vin[0U], sizeof(Cfg.VinSensorGeo));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        Cfg.View.Type = CTUserEmirGroupData.ViewSingleType;
        switch (Cfg.View.Type) {
        case AMBA_CAL_EM_LDC:
            Cfg.View.Cfg.pLDC = &pEmWorkingBuf->DataLdcViewCfg;
            pEmWorkingBuf->DataLdcViewCfg.FovWiderStrX = 0U;
            pEmWorkingBuf->DataLdcViewCfg.FovWiderStrY = 0U;
            pEmWorkingBuf->DataLdcViewCfg.DistEvenStrX = 100U;
            pEmWorkingBuf->DataLdcViewCfg.DistEvenStrY = 100U;
            pEmWorkingBuf->DataLdcViewCfg.LineStraightStrX = 100U;
            pEmWorkingBuf->DataLdcViewCfg.LineStraightStrY = 100U;
            //FIXME # Not allow user cfg yet.
            break;
        case AMBA_CAL_EM_ASPHERIC:
            Cfg.View.Cfg.pAspheric = &pEmWorkingBuf->DataAspViewCfg;
            //FIXME # Feed PointMap of AspCfg
            Rval |= CT_EmFeedAsphericPointMap(&CTUserEmirGroupData.ViewSingleAspCfg, Cfg.View.Cfg.pAspheric);
            break;
        case AMBA_CAL_EM_CURVED_SURFACE:
            Cfg.View.Cfg.pCurvedSurface = &pEmWorkingBuf->DataCurvedViewCfg;
            //FIXME # Feed PointMap of AspCfg
            Rval |= CT_EmFeedVirtualCamCurvedPlane(&CTUserEmirGroupData.ViewSingleCurvedCfg, Cfg.View.Cfg.pCurvedSurface);
            break;
        case AMBA_CAL_EM_CURVED_ASPHERIC:
            Cfg.View.Cfg.pAspheric = &pEmWorkingBuf->DataAspViewCfg;
            //FIXME # Feed PointMap of AspCfg
            Rval |= CT_EmFeedAsphericPointMap(&CTUserEmirGroupData.ViewSingleAspCfg, Cfg.View.Cfg.pAspheric);
            Cfg.View.Cfg.pCurvedSurface = &pEmWorkingBuf->DataCurvedViewCfg;
            //FIXME # Feed PointMap of AspCfg
            Rval |= CT_EmFeedVirtualCamCurvedPlane(&CTUserEmirGroupData.ViewSingleCurvedCfg, Cfg.View.Cfg.pCurvedSurface);
            break;
        default:
            pNextMsg = CT_LogPutStr(Msg, "Invalid View.Type ");
            pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) Cfg.View.Type);
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
            Rval |= CAL_ERR_1;
            break;
        }
        Rval |= AmbaWrap_memcpy(&Cfg.View.VoutArea, &CTUserEmirGroupData.Vout[0U], sizeof(Cfg.View.VoutArea));
        Cfg.View.PlugIn.PlugInMode = 0U; // TODO #
        Cfg.View.PlugIn.MsgReciverCfg.GridStatus = 1U;
        Cfg.View.PlugIn.MsgReciverCfg.Report = 1U;
        if ((Cfg.View.Type == AMBA_CAL_EM_CURVED_SURFACE) || (Cfg.View.Type == AMBA_CAL_EM_CURVED_ASPHERIC)) {
            Cfg.View.PlugIn.MsgReciverCfg.PatternError = 1U;
        }
        Cfg.View.PlugIn.MsgReciver = pEmSvCbMsgReciver;

        Rval |= AmbaWrap_memset(&CalibPointsMap, 0x0, sizeof(CalibPointsMap));
        CT_EmFeedPointMap(&CTUserEmirGroupData.CalibPointMap[0U], &CTUserEmirGroupData.AssistancePointMap[0U], &CalibPointsMap);
        Cfg.Calibinfo.p4Point = &CalibPointsMap;
        Cfg.Calibinfo.Type = AMBA_EM_CALIB_4_POINT;

        Rval |= AmbaWrap_memcpy(&Cfg.Tile, &CTUserEmirGroupData.TileSize[0U], sizeof(Cfg.Tile));
        Rval |= AmbaWrap_memcpy(&Cfg.ROI, &CTUserEmirGroupData.Roi[0], sizeof(Cfg.ROI));
        CT_CastUINT2OptimizeLevel(CTUserEmirGroupData.OptimizeLevel[0U], &Cfg.OptimizeLevel);
        Rval |= AmbaWrap_memcpy(&Cfg.InternalCfg, &CTUserEmirGroupData.Internal[0U], sizeof(Cfg.InternalCfg));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        Rval |= CT_EmDumpSingleViewCfg(&Cfg);
        Rval |= AmbaCal_EmGenSingleView(&Cfg, pEmCalibWorkingBuf, &pEmWorkingBuf->DataSingleView);
    }
    return Rval;
}

UINT32 AmbaCT_EmExecute(void)
{
    UINT32 Rval = CAL_OK;
    AMBA_CT_EM_TYPE_e Type = CT_EmString2Type(CTUserEmirGroupData.System.Type);

    switch (Type) {
    case AMBA_CT_EM_TYPE_3IN1:
        Rval |= CT_Em3in1Execute();
        break;
    case AMBA_CT_EM_TYPE_SINGLE_VIEW:
        Rval |= CT_EmSingleViewExecute();
        break;
    default:
        Rval |= CAL_ERR_1;
        break;
    }
    return Rval;
}

UINT32 AmbaCT_EmGetCalibData(AMBA_CT_EM_CALIB_DATA_s *pData)
{
    AMBA_CT_EM_TYPE_e Type = CT_EmString2Type(CTUserEmirGroupData.System.Type);
    UINT32 Rval = CAL_OK;
    pData->Type = Type;
    switch(Type) {
        case AMBA_CT_EM_TYPE_3IN1:
            pData->pData3in1 = &pEmWorkingBuf->Data3in1;
            break;
        case AMBA_CT_EM_TYPE_SINGLE_VIEW:
            pData->pDataSingleView = &pEmWorkingBuf->DataSingleView;
            break;
        default:
            Rval = CAL_ERR_1;
            break;
    }
    return Rval;
}

const AMBA_CT_EM_USER_SETTING_s * AmbaCT_EmirGetUserSetting(void)
{
    return (const AMBA_CT_EM_USER_SETTING_s *)&CTUserEmirGroupData;
}
