/**
 *  @file AmbaCT_StereoTuner.c
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
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
#include "AmbaTypes.h"
#include "AmbaCT_StereoTuner.h"
#include "AmbaFS.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCT_Logging.h"
#include "AmbaCalib_StereoIF.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

typedef struct {
    AMBA_CAL_EM_SV_DATA_s DataSingleView[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_EM_CURVED_SURFACE_CFG_s DataCurvedViewCfg;
    struct {
        DOUBLE RealTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE ExceptTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE RealAngleTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE ExceptAngleTable[MAX_LENS_DISTO_TBL_LEN];
    } LensTable[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_EM_POINT_MAP_s NormalPointMapCfg[AMBA_CAL_ST_CAM_NUM];
    AMBA_CAL_EM_MULTI_POINT_MAP_s MultiPointMapCfg[AMBA_CAL_ST_CAM_NUM];
    AMBA_CAL_WORLD_RAW_POINT_s MultiPointMapCfgCornerPairs[AMBA_CAL_ST_CAM_NUM][AMBA_CAL_ST_MAX_CORNER_NUM];
} AMBA_CT_STEREO_WORKING_BUF_s;

static AMBA_CT_STEREO_USER_SETTING_s CTUserStereoGroupData GNU_SECTION_NOZEROINIT;
static UINT8 StereoTunerValidGroup[AMBA_CT_STEREO_TUNER_MAX];
static AMBA_CT_STEREO_WORKING_BUF_s *pStereoWorkingBuf = NULL;
static void *pStereoCalibWorkingBuf = NULL;
static SIZE_t StereoCalibWorkingBufSize = 0;

static UINT32 (*pStereoSvCbMsgReciver)(AMBA_CAL_EM_MSG_TYPE_e Type, const AMBA_CAL_EM_MSG_s *pMsg) = NULL;
static UINT32 (*pStereoSvCbFeedCornerMap)(const char *FilePath, AMBA_CAL_SIZE_s *pCorner2dMapSize,
        AMBA_CAL_POINT_INT_2D_s *pBaseCorner2dIdx, AMBA_CAL_EM_MULTI_POINT_MAP_s *pMultiPointSet) = NULL;

static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static inline void AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_TUNER_GROUP_s GroupId)
{
    if (GroupId < AMBA_CT_STEREO_TUNER_MAX) {
        StereoTunerValidGroup[GroupId] = 1;
    }
}

void AmbaCT_StereoGetOutputCfg(UINT32 ArrayIndex, AMBA_CT_STEREO_OUTPUT_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.OutputCfg[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetOutputCfg(UINT32 ArrayIndex, const AMBA_CT_STEREO_OUTPUT_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.OutputCfg[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_OUTPUT_CFG);
}

void AmbaCT_StereoGetSystem(AMBA_CT_STEREO_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetSystem(const AMBA_CT_STEREO_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_SYSTEM);
}

void AmbaCT_StereoGetWarpOptimizer(AMBA_CT_STEREO_WARP_OPTIMIZER_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.WarpOptimizer, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetWarpOptimizer(const AMBA_CT_STEREO_WARP_OPTIMIZER_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.WarpOptimizer, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_WARP_OPTIMIZER);
}

void AmbaCT_StereoGetVirtulaCam(UINT32 ArrayIndex, AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.VirtualCamViewCfg[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetVirtulaCam(UINT32 ArrayIndex, const AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.VirtualCamViewCfg[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_VIRTUAL_CAM);
}

void AmbaCT_StereoGetVout(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.Vout[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetVout(UINT32 ArrayIndex, const AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.Vout[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_VOUT);
}

void AmbaCT_StereoGetLens(UINT32 ArrayIndex, AMBA_CT_STEREO_LENS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.Lens[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetLens(UINT32 ArrayIndex, const AMBA_CT_STEREO_LENS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.Lens[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_LENS);
}

void AmbaCT_StereoGetSensor(UINT32 ArrayIndex, AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.Sensor[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetSensor(UINT32 ArrayIndex, const AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.Sensor[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_SENSOR);
}

void AmbaCT_StereoGetOpticalCenter(UINT32 ArrayIndex, AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.OpticalCenter[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetOpticalCenter(UINT32 ArrayIndex, const AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.OpticalCenter[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_OPTICAL_CENTER);
}

void AmbaCT_StereoGetCamera(UINT32 ArrayIndex, AMBA_CT_STEREO_CAMERA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.Camera[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetCamera(UINT32 ArrayIndex, const AMBA_CT_STEREO_CAMERA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.Camera[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_CAMERA);
}

void AmbaCT_StereoGetVin(UINT32 ArrayIndex, AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.Vin[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetVin(UINT32 ArrayIndex, const AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.Vin[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_VIN);
}

void AmbaCT_StereoGetRoi(UINT32 ArrayIndex, AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.Roi[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetRoi(UINT32 ArrayIndex, const AMBA_CAL_ROI_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.Roi[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_ROI);
}

void AmbaCT_StereoGetTileSize(UINT32 ArrayIndex, AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.TileSize[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetTileSize(UINT32 ArrayIndex, const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.TileSize[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_TILE_SIZE);
}

void AmbaCT_StereoGetCalibPoints(UINT32 ArrayIndex, AMBA_CT_STEREO_CALIB_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.CalibPointMap[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetCalibPoints(UINT32 ArrayIndex, const AMBA_CT_STEREO_CALIB_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.CalibPointMap[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_CALIB_POINTS);
}

void AmbaCT_StereoGetAssistPoints(UINT32 ArrayIndex, AMBA_CT_STEREO_ASSIST_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.AssistancePointMap[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetAssistPoints(UINT32 ArrayIndex, const AMBA_CT_STEREO_ASSIST_POINTS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.AssistancePointMap[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_ASSIST_POINTS);
}

void AmbaCT_StereoGetOptimize(UINT32 ArrayIndex, UINT8 *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.OptimizeLevel[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetOptimize(UINT32 ArrayIndex, const UINT8 *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.OptimizeLevel[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_OPTIMIZE);
}

void AmbaCT_StereoGetInternal(UINT32 ArrayIndex, AMBA_CAL_EM_INTERNAL_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserStereoGroupData.Internal[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_StereoSetInternal(UINT32 ArrayIndex, const AMBA_CAL_EM_INTERNAL_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserStereoGroupData.Internal[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_StereoSetGroupValid(AMBA_CT_STEREO_INTERNAL);
}

void AmbaCT_StereoGetWorkingBufSize(SIZE_t *pSize)
{
    *pSize = sizeof(AMBA_CT_STEREO_WORKING_BUF_s);
}

static inline void AmbaCT_StereoResetGData2Default(void)
{
    UINT32 Index;
    CT_CheckRval(AmbaWrap_memset(&CTUserStereoGroupData, 0x0, sizeof(CTUserStereoGroupData)), "AmbaWrap_memset", __func__);

    CTUserStereoGroupData.WarpOptimizer.Enable = 0;

    for (Index = 0U; Index < (UINT32)AMBA_CAL_EM_CAM_MAX; Index++) {
        CTUserStereoGroupData.Internal[Index].WaitlineTh = 0;
        CTUserStereoGroupData.Internal[Index].DmaTh = 0;
        CTUserStereoGroupData.Internal[Index].DebugID = 0;
        CTUserStereoGroupData.Internal[Index].ExtrapolatePlaneBoundary = 0;
        CTUserStereoGroupData.Internal[Index].RefineDmaEnable = 0;
        CTUserStereoGroupData.Internal[Index].RefineWaitlineEnable = 0;
        CTUserStereoGroupData.Internal[Index].RefineOutOfRoiEnable = 1;
        CTUserStereoGroupData.Internal[Index].RefineVFlipEnable = 0;
        CTUserStereoGroupData.Internal[Index].RepeatUnusedArea = 0;
        CTUserStereoGroupData.Internal[Index].PointMapHFlipEnable = 0;
        CTUserStereoGroupData.Internal[Index].AutoRoiEnable = 0;
        CTUserStereoGroupData.Lens[Index].pRealTable = &pStereoWorkingBuf->LensTable[Index].RealTable[0];
        CTUserStereoGroupData.Lens[Index].pExceptTable = &pStereoWorkingBuf->LensTable[Index].ExceptTable[0];
        CTUserStereoGroupData.Lens[Index].pRealAngleTable = &pStereoWorkingBuf->LensTable[Index].RealAngleTable[0];
        CTUserStereoGroupData.Lens[Index].pExceptAngleTable = &pStereoWorkingBuf->LensTable[Index].ExceptAngleTable[0];
        CTUserStereoGroupData.CalibPointMap[Index].CornerMapBinPath[0] = '\0';
    }
}

static void AmbaCT_StereoCastVoidPoint2WorkingBuf(void * const *VoidAddr, AMBA_CT_STEREO_WORKING_BUF_s **WorkBufAddr)
{
    CT_CheckRval(AmbaWrap_memcpy(WorkBufAddr, VoidAddr, sizeof(AMBA_CT_STEREO_WORKING_BUF_s *)), "AmbaWrap_memcpy", __func__);
}

UINT32 AmbaCT_StereoTunerInit(const AMBA_CT_STEREO_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    if (pInitCfg->WorkingBufSize <  sizeof(AMBA_CT_STEREO_WORKING_BUF_s)) {

        pNextMsg = CT_LogPutStr(Msg, "Working buffer size ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(pInitCfg->WorkingBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, " incorrect, necessary size: ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(sizeof(AMBA_CT_STEREO_WORKING_BUF_s)));
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CAL_ERR_1;
    } else {
        if(AmbaWrap_memset(StereoTunerValidGroup, 0x0, sizeof(StereoTunerValidGroup)) != CAL_OK){
            Rval = CAL_ERR_1;
        }

        AmbaCT_StereoCastVoidPoint2WorkingBuf(&pInitCfg->pWorkingBuf, &pStereoWorkingBuf);
        pStereoSvCbMsgReciver = pInitCfg->SvCbMsgReciver;
        pStereoSvCbFeedCornerMap = pInitCfg->SvCbMsgFeedCornerMap;
        pStereoCalibWorkingBuf = pInitCfg->pCalibWorkingBuf;
        StereoCalibWorkingBufSize = pInitCfg->CalibWorkingBufSize;
        AmbaCT_StereoResetGData2Default();
    }
    return Rval;
}

UINT8 AmbaCT_StereoGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_STEREO_TUNER_MAX) ? StereoTunerValidGroup[GroupId] : 0U;
}

static const char* AmbaCT_StereoGetGroupName(UINT8 GroupId)
{
    const char *Rval = "CALIB_TUNER_UNKNOWN_GROUP";
    static const struct {
        const char *const GroupName;
        AMBA_CT_STEREO_TUNER_GROUP_s GroupId;
    } GroupLut[] = {
        {"AMBA_CT_STEREO_OUTPUT_CFG", AMBA_CT_STEREO_OUTPUT_CFG},
        {"AMBA_CT_STEREO_SYSTEM", AMBA_CT_STEREO_SYSTEM},
        {"AMBA_CT_STEREO_VIRTUAL_CAM", AMBA_CT_STEREO_VIRTUAL_CAM},
        {"AMBA_CT_STEREO_VOUT", AMBA_CT_STEREO_VOUT},
        {"AMBA_CT_STEREO_LENS", AMBA_CT_STEREO_LENS},
        {"AMBA_CT_STEREO_SENSOR", AMBA_CT_STEREO_SENSOR},
        {"AMBA_CT_STEREO_OPTICAL_CENTER", AMBA_CT_STEREO_OPTICAL_CENTER},
        {"AMBA_CT_STEREO_CAMERA", AMBA_CT_STEREO_CAMERA},
        {"AMBA_CT_STEREO_VIN", AMBA_CT_STEREO_VIN},
        {"AMBA_CT_STEREO_ROI", AMBA_CT_STEREO_ROI},
        {"AMBA_CT_STEREO_TILE_SIZE", AMBA_CT_STEREO_TILE_SIZE},
        {"AMBA_CT_STEREO_CALIB_POINTS", AMBA_CT_STEREO_CALIB_POINTS},
        {"AMBA_CT_STEREO_ASSIST_POINTS", AMBA_CT_STEREO_ASSIST_POINTS},
        {"AMBA_CT_STEREO_OPTIMIZE", AMBA_CT_STEREO_OPTIMIZE},
        {"AMBA_CT_STEREO_INTERNAL", AMBA_CT_STEREO_INTERNAL},
    };

    for (UINT32 Index = 0U; Index < (sizeof(GroupLut)/sizeof(GroupLut[0])); Index++) {
        if (GroupId == (UINT8) GroupLut[Index].GroupId) {
            Rval = GroupLut[Index].GroupName;
            break;
        }
    }
    return (const char *const) Rval;
}

static UINT32 AmbaCT_StereoCheckGroup(const UINT8 *pNecessaryGroup, UINT32 GroupLen)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 Index;
    for (Index = 0U; Index < GroupLen; Index++) {
        if (StereoTunerValidGroup[pNecessaryGroup[Index]] == 0U) {
            pNextMsg = CT_LogPutStr(Msg, "Lose Group ");
            pNextMsg = CT_LogPutStr(pNextMsg, AmbaCT_StereoGetGroupName(pNecessaryGroup[Index]));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CAL_ERR_1;
        }
    }
    return Rval;
}

static UINT32 AmbaCT_StereoFeedLensSpec(const AMBA_CT_STEREO_LENS_s *pIn,
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

static UINT32 AmbaCT_StereoFeedPointMap(const AMBA_CT_STEREO_CALIB_POINTS_s *pInCalibPoints,
    const AMBA_CT_STEREO_ASSIST_POINTS_s *pInAssistancePoints,
    AMBA_CAL_EM_CALIB_POINT_INFO_s *pOutCalibinfo,
    AMBA_CAL_SIZE_s *pCorner2dMapSize,
    AMBA_CAL_POINT_INT_2D_s *pBaseCorner2dIdx)
{
    UINT32 RetVal = CAL_OK;

    if (pInCalibPoints->CornerMapBinPath[0] != '\0') {
        pOutCalibinfo->Type = AMBA_EM_CALIB_MULTI_POINT;

        RetVal = pStereoSvCbFeedCornerMap(pInCalibPoints->CornerMapBinPath, pCorner2dMapSize,
            pBaseCorner2dIdx, pOutCalibinfo->pMultiPoint);
    } else {
        UINT32 Index;

        pOutCalibinfo->Type = AMBA_EM_CALIB_4_POINT;

        for (Index = 0U; Index < EMIR_CALIB_POINT_NUM; Index++) {
            pOutCalibinfo->p4Point->CalibPoints[Index].WorldPos.X = pInCalibPoints->WorldPositionX[Index];
            pOutCalibinfo->p4Point->CalibPoints[Index].WorldPos.Y = pInCalibPoints->WorldPositionY[Index];
            pOutCalibinfo->p4Point->CalibPoints[Index].WorldPos.Z = pInCalibPoints->WorldPositionZ[Index];
            pOutCalibinfo->p4Point->CalibPoints[Index].RawPos.X = pInCalibPoints->RawPositionX[Index];
            pOutCalibinfo->p4Point->CalibPoints[Index].RawPos.Y = pInCalibPoints->RawPositionY[Index];
        }

        for (Index = 0U; Index < pInAssistancePoints->Number; Index++) {
            pOutCalibinfo->p4Point->AssistancePoints[Index].WorldPos.X = pInAssistancePoints->WorldPositionX[Index];
            pOutCalibinfo->p4Point->AssistancePoints[Index].WorldPos.Y = pInAssistancePoints->WorldPositionY[Index];
            pOutCalibinfo->p4Point->AssistancePoints[Index].WorldPos.Z = pInAssistancePoints->WorldPositionZ[Index];
            pOutCalibinfo->p4Point->AssistancePoints[Index].RawPos.X = pInAssistancePoints->RawPositionX[Index];
            pOutCalibinfo->p4Point->AssistancePoints[Index].RawPos.Y = pInAssistancePoints->RawPositionY[Index];
        }

        pOutCalibinfo->p4Point->AssistancePointNumber = pInAssistancePoints->Number;
    }

    return RetVal;
}
#if 0
static inline void AmbaCT_StereoFeedCorner2DPairs(const AMBA_CT_STEREO_CALIB_POINTS_s *pInCalibPoints,
    const AMBA_CT_STEREO_ASSIST_POINTS_s *pInAssistancePoints,
    AMBA_CAL_EM_POINT_MAP_s *pOut)
{
    UINT32 Index;
    for (Index = 0U; Index < EMIR_CALIB_POINT_NUM; Index++) {
        pOut->CalibPoints[Index].WorldPos.X = pInCalibPoints->WorldPositionX[Index];
        pOut->CalibPoints[Index].WorldPos.Y = pInCalibPoints->WorldPositionY[Index];
        pOut->CalibPoints[Index].WorldPos.Z = pInCalibPoints->WorldPositionZ[Index];
        pOut->CalibPoints[Index].RawPos.X = pInCalibPoints->RawPositionX[Index];
        pOut->CalibPoints[Index].RawPos.Y = pInCalibPoints->RawPositionY[Index];
    }
    for (Index = 0U; Index < pInAssistancePoints->Number; Index++) {
        pOut->AssistancePoints[Index].WorldPos.X = pInAssistancePoints->WorldPositionX[Index];
        pOut->AssistancePoints[Index].WorldPos.Y = pInAssistancePoints->WorldPositionY[Index];
        pOut->AssistancePoints[Index].WorldPos.Z = pInAssistancePoints->WorldPositionZ[Index];
        pOut->AssistancePoints[Index].RawPos.X = pInAssistancePoints->RawPositionX[Index];
        pOut->AssistancePoints[Index].RawPos.Y = pInAssistancePoints->RawPositionY[Index];
    }
    pOut->AssistancePointNumber = pInAssistancePoints->Number;

}
#endif
static UINT32 AmbaCT_StereoDumpSingleViewCfg(const AMBA_CAL_EM_SV_CFG_s *pCfg)
{
    UINT32 Rval = CT_OK;
    UINT32 Index;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = NULL;

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Lens.LensDistoType: ");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pCfg->Cam.Lens.LensDistoType);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Lens.LensDistoSpec.pRealExpect->Length: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Cam.Lens.LensDistoSpec.pRealExpect->Length);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl:");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (Index = 0U; Index < pCfg->Cam.Lens.LensDistoSpec.pRealExpect->Length; Index++) {
        pNextMsg = CT_LogPutDB(Msg, pCfg->Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl[Index]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl:");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (Index = 0U; Index < pCfg->Cam.Lens.LensDistoSpec.pRealExpect->Length; Index++) {
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

static void CT_StereoCastUINT2OptimizeLevel(UINT8 InUint, AMBA_CAL_EM_OPTIMIZE_LEVEL_e *OutEnum)
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

#if 0
static inline UINT32 AmbaCT_StereoFeedAsphericPointMap(const AMBA_CT_STEREO_ASP_POINT_MAP_s *pInAspPointMap, AMBA_CAL_EM_ASP_VIEW_CFG_s *pOut)
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
#endif
static inline UINT32 AmbaCT_StereoFeedVirtualCamCurvedPlane(const AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pInCurved, AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pOut)
{
    UINT32 Rval = CAL_OK;
    if ((pInCurved == NULL) || (pOut == NULL)) {
        Rval |= CAL_ERR_1;
    } else {
        CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
        CT_LOG_MSG_s *pNextMsg = NULL;
        if(CAL_OK != AmbaWrap_memcpy(pOut, pInCurved, sizeof(AMBA_CAL_EM_CURVED_SURFACE_CFG_s))) {
            Rval = CAL_ERR_1;
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

static UINT32 AmbaCT_StereoSingleViewExecute(UINT8 CameraId)
{
    UINT32 Rval = CAL_OK;
    SIZE_t Size;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg;
    static const UINT8 NecessaryGroupSV[] = {
        (UINT8)AMBA_CT_STEREO_SYSTEM,
        (UINT8)AMBA_CT_STEREO_OUTPUT_CFG,
        (UINT8)AMBA_CT_STEREO_VIRTUAL_CAM,
        (UINT8)AMBA_CT_STEREO_VOUT,
        (UINT8)AMBA_CT_STEREO_LENS,
        (UINT8)AMBA_CT_STEREO_SENSOR,
        (UINT8)AMBA_CT_STEREO_OPTICAL_CENTER,
        (UINT8)AMBA_CT_STEREO_CAMERA,
        (UINT8)AMBA_CT_STEREO_VIN,
        (UINT8)AMBA_CT_STEREO_ROI,
        (UINT8)AMBA_CT_STEREO_TILE_SIZE,
        (UINT8)AMBA_CT_STEREO_OPTIMIZE,
        (UINT8)AMBA_CT_STEREO_INTERNAL,
    };

    Rval |= AmbaCal_EmGetSingleViewBufSize(&Size);
    if (CAL_OK != AmbaCT_StereoCheckGroup(NecessaryGroupSV, CT_sizeT_to_U32(sizeof(NecessaryGroupSV)/sizeof(NecessaryGroupSV[0])))) {
        Rval = CAL_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "ERROR: AmbaCT_StereoCheckGroup Fail");
        pNextMsg = CT_LogPutStr(pNextMsg, ".");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else if (StereoCalibWorkingBufSize < Size) {
        Rval = CAL_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "ERROR: Stereo Buf Size: ");
        pNextMsg = CT_LogPutU64(pNextMsg, StereoCalibWorkingBufSize);
        pNextMsg = CT_LogPutStr(pNextMsg, " < ");
        pNextMsg = CT_LogPutU64(pNextMsg, Size);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else if (CameraId >= (UINT8)AMBA_CAL_ST_CAM_NUM) {
        Rval = CAL_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "ERROR: Invalid Camera Id ");
        pNextMsg = CT_LogPutU32(pNextMsg, CameraId);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        AMBA_CAL_EM_SV_CFG_s Cfg;
        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect;
        AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle;
        AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula;
        pNextMsg = CT_LogPutStr(Msg, "AmbaCT_StereoSingleViewExecute Camera Id ");
        pNextMsg = CT_LogPutU32(pNextMsg, CameraId);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

        Rval |= AmbaWrap_memset(&Cfg, 0x0, sizeof(Cfg));
        Rval |= AmbaCT_StereoFeedLensSpec(&CTUserStereoGroupData.Lens[CameraId], &Cfg.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);
        Rval |= AmbaWrap_memcpy(&Cfg.Cam.Sensor, &CTUserStereoGroupData.Sensor[CameraId], sizeof(Cfg.Cam.Sensor));
        Rval |= AmbaWrap_memcpy(&Cfg.Cam.OpticalCenter, &CTUserStereoGroupData.OpticalCenter[CameraId], sizeof(Cfg.Cam.OpticalCenter));
        Cfg.Cam.Pos.X = CTUserStereoGroupData.Camera[CameraId].PositionX;
        Cfg.Cam.Pos.Y = CTUserStereoGroupData.Camera[CameraId].PositionY;
        Cfg.Cam.Pos.Z = CTUserStereoGroupData.Camera[CameraId].PositionZ;
        Cfg.Cam.Rotation = AMBA_CAL_ROTATE_0;

        Rval |= AmbaWrap_memcpy(&Cfg.VinSensorGeo, &CTUserStereoGroupData.Vin[CameraId], sizeof(Cfg.VinSensorGeo));
        if(Rval != CAL_OK){
            Rval = CAL_ERR_2;
        }
        Cfg.View.Type = AMBA_CAL_EM_CURVED_SURFACE;

        Cfg.Calibinfo.p4Point = &pStereoWorkingBuf->NormalPointMapCfg[CameraId];
        Cfg.Calibinfo.pMultiPoint = &pStereoWorkingBuf->MultiPointMapCfg[CameraId];
        Cfg.Calibinfo.pMultiPoint->pMultiCalibPoints = pStereoWorkingBuf->MultiPointMapCfgCornerPairs[CameraId];

        Rval |= AmbaCT_StereoFeedPointMap(&CTUserStereoGroupData.CalibPointMap[CameraId], &CTUserStereoGroupData.AssistancePointMap[CameraId],
            &Cfg.Calibinfo, &CTUserStereoGroupData.Corner2dMapSize[CameraId], &CTUserStereoGroupData.BaseCorner2dIdx[CameraId]);

        Cfg.View.Cfg.pCurvedSurface = &pStereoWorkingBuf->DataCurvedViewCfg;
        Rval |= AmbaCT_StereoFeedVirtualCamCurvedPlane(&CTUserStereoGroupData.VirtualCamViewCfg[CameraId], Cfg.View.Cfg.pCurvedSurface);
        {
            DOUBLE CurvedWorldPosLineY = 0.0;

            if (CTUserStereoGroupData.CalibPointMap[CameraId].CornerMapBinPath[0] != '\0') {
                UINT32 PtIdx;

                for (PtIdx = 0U; PtIdx < (CTUserStereoGroupData.Corner2dMapSize[CameraId].Width*CTUserStereoGroupData.Corner2dMapSize[CameraId].Height); PtIdx++) {
                    AMBA_CAL_WORLD_RAW_POINT_s ReadPt;

                    ReadPt = Cfg.Calibinfo.pMultiPoint->pMultiCalibPoints[PtIdx];
                    if ((ReadPt.RawPos.X == 0.0) && (ReadPt.RawPos.Y == 0.0)) {
                        continue;
                    } else {
                        CurvedWorldPosLineY = ReadPt.WorldPos.Y;
                        break;
                    }
                }
            } else {
                CurvedWorldPosLineY = Cfg.Calibinfo.p4Point->CalibPoints[0U].WorldPos.Y;
            }

            Cfg.View.Cfg.pCurvedSurface->WorldPosCurveLeft.X = 2200000.0;
            Cfg.View.Cfg.pCurvedSurface->WorldPosCurveLeft.Y = CurvedWorldPosLineY + 500.0;
            Cfg.View.Cfg.pCurvedSurface->WorldPosCurveRight.X = -2200000.0;
            Cfg.View.Cfg.pCurvedSurface->WorldPosCurveRight.Y = CurvedWorldPosLineY + 500.0;
            Cfg.View.Cfg.pCurvedSurface->WorldPosLineLeft.X = 2000000.0;
            Cfg.View.Cfg.pCurvedSurface->WorldPosLineLeft.Y = CurvedWorldPosLineY;
            Cfg.View.Cfg.pCurvedSurface->WorldPosLineRight.X = -2000000.0;
            Cfg.View.Cfg.pCurvedSurface->WorldPosLineRight.Y = CurvedWorldPosLineY;
        }

        Cfg.View.PlugIn.PlugInMode = 0U;
        Cfg.View.PlugIn.MsgReciverCfg.GridStatus = 1U;
        Cfg.View.PlugIn.MsgReciverCfg.Report = 1U;
        if ((Cfg.View.Type == AMBA_CAL_EM_CURVED_SURFACE) || (Cfg.View.Type == AMBA_CAL_EM_CURVED_ASPHERIC)) {
            Cfg.View.PlugIn.MsgReciverCfg.PatternError = 1U;
        }
        Cfg.View.PlugIn.MsgReciver = pStereoSvCbMsgReciver;

        CT_StereoCastUINT2OptimizeLevel(CTUserStereoGroupData.OptimizeLevel[CameraId], &Cfg.OptimizeLevel);
        Rval |= AmbaWrap_memcpy(&Cfg.View.VoutArea, &CTUserStereoGroupData.Vout[CameraId], sizeof(Cfg.View.VoutArea));
        Rval |= AmbaWrap_memcpy(&Cfg.Tile, &CTUserStereoGroupData.TileSize[CameraId], sizeof(Cfg.Tile));
        Rval |= AmbaWrap_memcpy(&Cfg.ROI, &CTUserStereoGroupData.Roi[CameraId], sizeof(Cfg.ROI));
        Rval |= AmbaWrap_memcpy(&Cfg.InternalCfg, &CTUserStereoGroupData.Internal[CameraId], sizeof(Cfg.InternalCfg));
        Rval |= AmbaCT_StereoDumpSingleViewCfg(&Cfg);

        Rval |= AmbaWrap_memset(&pStereoWorkingBuf->DataSingleView[CameraId], 0, sizeof(AMBA_CAL_EM_SV_DATA_s));
        Rval |= AmbaCal_EmGenSingleView(&Cfg, pStereoCalibWorkingBuf, &pStereoWorkingBuf->DataSingleView[CameraId]);
        if(Rval != CAL_OK) {
            Rval = CAL_ERR_2;
        }
    }
    return Rval;
}

static UINT32 AmbaCT_StereoWarpOptimizeExecute(void)
{
    UINT32 Rval = CAL_OK;
    AMBA_CAL_ST_REFINE_CFG_s Cfg;
    UINT32 CameraId;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    DOUBLE DistanceToWall = 0.0;

    AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect[AMBA_CAL_ST_CAM_NUM];
    AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle[AMBA_CAL_ST_CAM_NUM];
    AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula[AMBA_CAL_ST_CAM_NUM];

    for (CameraId = 0U; CameraId < AMBA_CAL_ST_CAM_NUM; CameraId++) {
        if (CTUserStereoGroupData.CalibPointMap[CameraId].CornerMapBinPath[0] == '\0') {
            Rval = CAL_ERR_1;
            break;
        } else {
            UINT32 PtIdx;

            Cfg.pCornerPairs[CameraId] = pStereoWorkingBuf->MultiPointMapCfgCornerPairs[CameraId];
            Cfg.Corner2dMapSize[CameraId] = CTUserStereoGroupData.Corner2dMapSize[CameraId];
            Cfg.BaseCorner2DIdx[CameraId] = CTUserStereoGroupData.BaseCorner2dIdx[CameraId];
            for (PtIdx = 0U; PtIdx < (Cfg.Corner2dMapSize[CameraId].Width*Cfg.Corner2dMapSize[CameraId].Height); PtIdx++) {
                AMBA_CAL_WORLD_RAW_POINT_s ReadPt;
                ReadPt = Cfg.pCornerPairs[CameraId][PtIdx];
                if ((ReadPt.RawPos.X == 0.0) && (ReadPt.RawPos.Y == 0.0)) {
                    continue;
                } else {
                    DistanceToWall = -ReadPt.WorldPos.Y;
                    break;
                }
            }
        }

        //Note # setting camera virtual cam vout setting
        Cfg.VirtualCamViewCfg[CameraId].VirtualCam = CTUserStereoGroupData.VirtualCamViewCfg[CameraId].VirtualCam;
        {
            Cfg.VirtualCamViewCfg[CameraId].WorldPosCurveLeft.X = 2200000.0;
            Cfg.VirtualCamViewCfg[CameraId].WorldPosCurveLeft.Y = -DistanceToWall + 500.0;
            Cfg.VirtualCamViewCfg[CameraId].WorldPosCurveRight.X = -2200000.0;
            Cfg.VirtualCamViewCfg[CameraId].WorldPosCurveRight.Y = -DistanceToWall + 500.0;
            Cfg.VirtualCamViewCfg[CameraId].WorldPosLineLeft.X = 2000000.0;
            Cfg.VirtualCamViewCfg[CameraId].WorldPosLineLeft.Y = -DistanceToWall;
            Cfg.VirtualCamViewCfg[CameraId].WorldPosLineRight.X = -2000000.0;
            Cfg.VirtualCamViewCfg[CameraId].WorldPosLineRight.Y = -DistanceToWall;
        }

        Cfg.Vout[CameraId] = CTUserStereoGroupData.Vout[CameraId];
        Rval |= AmbaCT_StereoFeedLensSpec(&CTUserStereoGroupData.Lens[CameraId], &Cfg.Cam[CameraId].Lens,
            &LensSpecRealExpect[CameraId], &LensSpecAngle[CameraId], &LensSpecFormula[CameraId]);
        Rval |= AmbaWrap_memcpy(&Cfg.Cam[CameraId].Sensor, &CTUserStereoGroupData.Sensor[CameraId], sizeof(Cfg.Cam[CameraId].Sensor));
        Rval |= AmbaWrap_memcpy(&Cfg.Cam[CameraId].OpticalCenter, &CTUserStereoGroupData.OpticalCenter[CameraId], sizeof(Cfg.Cam[CameraId].OpticalCenter));
        Cfg.Cam[CameraId].Pos.X = CTUserStereoGroupData.Camera[CameraId].PositionX;
        Cfg.Cam[CameraId].Pos.Y = CTUserStereoGroupData.Camera[CameraId].PositionY;
        Cfg.Cam[CameraId].Pos.Z = CTUserStereoGroupData.Camera[CameraId].PositionZ;
        Cfg.Cam[CameraId].Rotation = AMBA_CAL_ROTATE_0;

        Cfg.RawSize[CameraId].Width = CTUserStereoGroupData.Vin[CameraId].Width;
        Cfg.RawSize[CameraId].Height = CTUserStereoGroupData.Vin[CameraId].Height;

        //Note # From result of Emirror caib
        Rval |= AmbaWrap_memcpy(Cfg.CalibMatrix[CameraId], pStereoWorkingBuf->DataSingleView[CameraId].CalibMatrix,
            sizeof(Cfg.CalibMatrix[CameraId]));
    }

    if ( Rval == CAL_OK ) {


        //Note # Process
#if 1
        //Note # Removed after Stereo works in SDK8  191023
        AmbaPrint_PrintUInt5("[cTuner] ERROR: AmbaCT_StereoWarpOptimizeExecute is blocked", 0U, 0U, 0U, 0U, 0U);
#else
        DOUBLE BaseLine;
        Cfg.HorGridNum = pStereoWorkingBuf->DataSingleView[AMBA_CAL_ST_CAM_LEFT].WarpTbl.HorGridNum;
        Cfg.VerGridNum = pStereoWorkingBuf->DataSingleView[AMBA_CAL_ST_CAM_LEFT].WarpTbl.VerGridNum;
        Cfg.TileWidthExp = pStereoWorkingBuf->DataSingleView[AMBA_CAL_ST_CAM_LEFT].WarpTbl.TileHeightExp;
        Cfg.TileHeightExp = pStereoWorkingBuf->DataSingleView[AMBA_CAL_ST_CAM_LEFT].WarpTbl.TileHeightExp;

        BaseLine = CTUserStereoGroupData.Camera[AMBA_CAL_ST_CAM_LEFT].PositionX - CTUserStereoGroupData.Camera[AMBA_CAL_ST_CAM_RIGHT].PositionX;
        BaseLine = ((BaseLine<0.0)?(-BaseLine):BaseLine);

        Cfg.DistanceToWall = DistanceToWall;
        Cfg.BaseLine = BaseLine;
        Rval |= AmbaCal_StereoRefineTbl(pStereoCalibWorkingBuf, &Cfg,
            pStereoWorkingBuf->DataSingleView[AMBA_CAL_ST_CAM_LEFT].WarpTbl.WarpVector);
#endif
    } else {
        pNextMsg = CT_LogPutStr(Msg, "Feed corner 2D map bin file failed");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    }

    return Rval;
}

UINT32 AmbaCT_StereoExecute(void)
{
    UINT32 Rval = CAL_OK;
    UINT8 CameraId = 0U;
    do {
        Rval = AmbaCT_StereoSingleViewExecute(CameraId);
        CameraId ++;
    } while(CameraId < CTUserStereoGroupData.System.CameraNum);

    if ( CTUserStereoGroupData.WarpOptimizer.Enable != 0U ) {
        Rval |= AmbaCT_StereoWarpOptimizeExecute();
    }

    return Rval;
}

UINT32 AmbaCT_StereoGetCalibData(UINT32 CameraId, AMBA_CT_STEREO_CALIB_DATA_s *pData)
{
    UINT32 Rval = CAL_OK;
    pData->pDataSingleView = &pStereoWorkingBuf->DataSingleView[CameraId];
    return Rval;
}

const AMBA_CT_STEREO_USER_SETTING_s * AmbaCT_StereoGetUserSetting(void)
{
    return (const AMBA_CT_STEREO_USER_SETTING_s *)&CTUserStereoGroupData;
}
#endif
