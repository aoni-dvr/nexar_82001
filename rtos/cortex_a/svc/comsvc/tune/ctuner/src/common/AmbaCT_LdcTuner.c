/**
 *  @file AmbaCT_LdcTuner.c
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
#include "AmbaCT_LdcTuner.h"
#include "AmbaCalib_Camera.h"
#include "AmbaCalib_LDCDef.h"
#include "AmbaCalib_LDCIF.h"
#include "AmbaCalib_ErrNo.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCT_Logging.h"
#include "AmbaCT_LdcTunerIF.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

typedef struct {
    AMBA_CAL_LDC_DATA_s CalibData;
    struct {
        DOUBLE RealTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE ExceptTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE RealAngleTable[MAX_LENS_DISTO_TBL_LEN];
        DOUBLE ExceptAngleTable[MAX_LENS_DISTO_TBL_LEN];
    } LensTable;
} AMBA_CT_LDC_WORKING_BUF_s;
static UINT8 LdcTunerValidGroup[AMBA_CT_LDC_MAX];
static AMBA_CT_LDC_WORKING_BUF_s *pLdcWorkingBuf = NULL;
static void *pLdcCalibWorkingBuf = NULL;
static SIZE_t LdcCalibWorkingBufSize = 0;
static AMBA_CT_LDC_USER_SETTING_s CTUserLdcGroupData GNU_SECTION_NOZEROINIT;
static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}
static inline void AmbaCT_LdcSetGroupValid(AMBA_CT_LDC_TUNER_GROUP_e GroupId)
{
    if (GroupId < AMBA_CT_LDC_MAX) {
        LdcTunerValidGroup[GroupId] = 1;
    }
}

void AmbaCT_LdcGetSystem(AMBA_CT_LDC_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserLdcGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdcSetSystem(const AMBA_CT_LDC_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserLdcGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_LdcSetGroupValid(AMBA_CT_LDC_SYSTEM_INFO);
}

void AmbaCT_LdcGetLens(AMBA_CT_LDC_LENS_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdcGroupData.Lens, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdcSetLens(const AMBA_CT_LDC_LENS_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdcGroupData.Lens, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_LdcSetGroupValid(AMBA_CT_LDC_LENS);
}

void AmbaCT_LdcGetSensor(AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdcGroupData.Sensor, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdcSetSensor(const AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdcGroupData.Sensor, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_LdcSetGroupValid(AMBA_CT_LDC_SENSOR);
}

void AmbaCT_LdcGetOpticalCenter(AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdcGroupData.OpticalCenter, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdcSetOpticalCenter(const AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdcGroupData.OpticalCenter, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_LdcSetGroupValid(AMBA_CT_LDC_OPTICAL_CENTER);
}

void AmbaCT_LdcGetCamera(AMBA_CT_LDC_CAMERA_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdcGroupData.Camera, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdcSetCamera(const AMBA_CT_LDC_CAMERA_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdcGroupData.Camera, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_LdcSetGroupValid(AMBA_CT_LDC_CAMERA);
}

void AmbaCT_LdcGetVin(AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdcGroupData.Vin, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdcSetVin(const AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdcGroupData.Vin, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_LdcSetGroupValid(AMBA_CT_LDC_VIN);
}

void AmbaCT_LdcGetTileSize(AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdcGroupData.TileSize, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdcSetTileSize(const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdcGroupData.TileSize, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_LdcSetGroupValid(AMBA_CT_LDC_TILE_SIZE);
}

void AmbaCT_LdcGetView(AMBA_CAL_LDC_VIEW_CFG_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(pData, &CTUserLdcGroupData.View, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_LdcSetView(const AMBA_CAL_LDC_VIEW_CFG_s *pData)
{
    CT_CheckRval( AmbaWrap_memcpy(&CTUserLdcGroupData.View, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_LdcSetGroupValid(AMBA_CT_LDC_VIEW);
}

void AmbaCT_LdcGetWorkingBufSize(SIZE_t *pSize)
{
    *pSize = sizeof(AMBA_CT_LDC_WORKING_BUF_s);
}

static inline void CT_LdcResetGData2Default(void)
{
    CT_CheckRval(AmbaWrap_memset(&CTUserLdcGroupData, 0x0, sizeof(CTUserLdcGroupData)), "AmbaWrap_memset", __func__);
    CTUserLdcGroupData.Lens.pRealTable = pLdcWorkingBuf->LensTable.RealTable;
    CTUserLdcGroupData.Lens.pExceptTable = pLdcWorkingBuf->LensTable.ExceptTable;
    CTUserLdcGroupData.Lens.pRealAngleTable = pLdcWorkingBuf->LensTable.RealAngleTable;
    CTUserLdcGroupData.Lens.pExceptAngleTable = pLdcWorkingBuf->LensTable.ExceptAngleTable;
}

UINT32 AmbaCT_LdcTunerInit(const AMBA_CT_LDC_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    if (pInitCfg->WorkingBufSize < sizeof(AMBA_CT_LDC_WORKING_BUF_s)) {
        pNextMsg = CT_LogPutStr(Msg, "Working buffer size ");
        pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pInitCfg->WorkingBufSize);
        pNextMsg = CT_LogPutStr(pNextMsg, "incorrect, necessary size:");
        pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) sizeof(AMBA_CT_LDC_WORKING_BUF_s));
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CAL_ERR_1;
    } else {
        Rval |= AmbaWrap_memset(LdcTunerValidGroup, 0x0, sizeof(LdcTunerValidGroup));
        Rval |= AmbaWrap_memcpy(&pLdcWorkingBuf, &pInitCfg->pWorkingBuf, sizeof(AMBA_CT_LDC_WORKING_BUF_s *));
        if(Rval != CAL_OK) {
            Rval = CAL_ERR_2;
        }
        pLdcCalibWorkingBuf = pInitCfg->pCalibWorkingBuf;
        LdcCalibWorkingBufSize = pInitCfg->CalibWorkingBufSize;
        CT_LdcResetGData2Default();
    }
    return Rval;
}

UINT8 AmbaCT_LdcGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_LDC_MAX) ? LdcTunerValidGroup[GroupId] : 0U;
}

static const char* AmbaCT_LdcGetGroupName(UINT8 GroupId)
{
    static const struct {
        const char *const GroupName;
        AMBA_CT_LDC_TUNER_GROUP_e GroupId;
    } GroupLut[] = {
        {"AMBA_CT_LDC_SYSTEM_INFO", AMBA_CT_LDC_SYSTEM_INFO},
        {"AMBA_CT_LDC_LENS", AMBA_CT_LDC_LENS},
        {"AMBA_CT_LDC_SENSOR", AMBA_CT_LDC_SENSOR},
        {"AMBA_CT_LDC_OPTICAL_CENTER", AMBA_CT_LDC_OPTICAL_CENTER},
        {"AMBA_CT_LDC_CAMERA", AMBA_CT_LDC_CAMERA},
        {"AMBA_CT_LDC_VIN", AMBA_CT_LDC_VIN},
        {"AMBA_CT_LDC_TILE_SIZE", AMBA_CT_LDC_TILE_SIZE},
        {"AMBA_CT_LDC_VIEW", AMBA_CT_LDC_VIEW},
    };
    const char *Rval = "CALIB_TUNER_UNKNOWN_GROUP";
    UINT32 Index;
    for (Index = 0; Index < (sizeof(GroupLut)/sizeof(GroupLut[0])); Index++) {
        if (GroupId == (UINT8)GroupLut[Index].GroupId) {
            Rval = GroupLut[Index].GroupName;
            break;
        }
    }
    return (const char * const ) Rval;
}

static UINT32 CT_LdcCheckGroup(const UINT8 *pNecessaryGroup, UINT32 GroupLen)
{
    UINT32 Rval = CAL_OK;
    UINT32 Index;
    for (Index = 0; Index < GroupLen; Index++) {
        if (LdcTunerValidGroup[pNecessaryGroup[Index]] == 0U) {
            AmbaPrint_PrintStr5("Lose Group %s", AmbaCT_LdcGetGroupName(pNecessaryGroup[Index]), DC_S, DC_S, DC_S, DC_S);
            Rval |= CAL_ERR_1;
        }
    }
    return Rval;
}

static UINT32 CT_LdcFeedLensSpec(const AMBA_CT_LDC_LENS_s *pIn,
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

static UINT32 CT_LdcDumpCfg(const AMBA_CAL_LDC_CFG_s *pCfg)
{
    UINT32 Rval;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 Index;
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

    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.LineStraightStrX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.LineStraightStrX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.LineStraightStrY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.LineStraightStrY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.FovWiderStrX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.FovWiderStrX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.FovWiderStrY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.FovWiderStrY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.DistEvenStrX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.DistEvenStrX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->View.DistEvenStrY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->View.DistEvenStrY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Tile.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Tile.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Tile.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Tile.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    return Rval;
}

UINT32 AmbaCT_LdcExecute(void)
{
    UINT32 Rval = CAL_OK;
    static const UINT8 NecessaryGroup[] = {
        (UINT8)AMBA_CT_LDC_SYSTEM_INFO,
        (UINT8)AMBA_CT_LDC_LENS,
        (UINT8)AMBA_CT_LDC_SENSOR,
        (UINT8)AMBA_CT_LDC_OPTICAL_CENTER,
        (UINT8)AMBA_CT_LDC_VIN,
        (UINT8)AMBA_CT_LDC_TILE_SIZE,
        (UINT8)AMBA_CT_LDC_VIEW,
    };

    if ((0UL != CT_LdcCheckGroup(NecessaryGroup, CT_sizeT_to_U32(sizeof(NecessaryGroup)/sizeof(NecessaryGroup[0]))))) {
        Rval |= CAL_ERR_1;
    } else {
        CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
        CT_LOG_MSG_s *pNextMsg = Msg;
        AMBA_CAL_LDC_CFG_s Cfg;
        AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect;
        AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle;
        AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula;
        SIZE_t Size;
        Rval |= AmbaCal_LdcGetBufSize(&Size);
        Cfg.Cam.Lens.LensDistoSpec.pRealExpect = NULL;
        if (LdcCalibWorkingBufSize < Size) {
            pNextMsg = CT_LogPutStr(Msg, "Buf Size:");
            pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(LdcCalibWorkingBufSize));
            pNextMsg = CT_LogPutStr(pNextMsg, "<");
            pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(Size));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CAL_ERR_1;
        } else {
            Rval |= CT_LdcFeedLensSpec(&CTUserLdcGroupData.Lens, &Cfg.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);
            Cfg.Cam.Lens.LensDistoUnit = AMBA_CAL_LD_MM;
            Rval |= AmbaWrap_memcpy(&Cfg.Cam.Sensor, &CTUserLdcGroupData.Sensor, sizeof(Cfg.Cam.Sensor));
            Rval |= AmbaWrap_memcpy(&Cfg.Cam.OpticalCenter, &CTUserLdcGroupData.OpticalCenter, sizeof(Cfg.Cam.OpticalCenter));
            Cfg.Cam.Pos.X = CTUserLdcGroupData.Camera.PositionX;
            Cfg.Cam.Pos.Y = CTUserLdcGroupData.Camera.PositionY;
            Cfg.Cam.Pos.Z = CTUserLdcGroupData.Camera.PositionZ;
            Cfg.Cam.Rotation = CTUserLdcGroupData.Camera.RotateType;
            Rval |= AmbaWrap_memcpy(&Cfg.VinSensorGeo, &CTUserLdcGroupData.Vin, sizeof(Cfg.VinSensorGeo));
            Rval |= AmbaWrap_memcpy(&Cfg.View, &CTUserLdcGroupData.View, sizeof(Cfg.View));
            Rval |= AmbaWrap_memcpy(&Cfg.Tile, &CTUserLdcGroupData.TileSize, sizeof(Cfg.Tile));
            Rval |= AmbaWrap_memset(&pLdcWorkingBuf->CalibData, 0x0, sizeof(pLdcWorkingBuf->CalibData));
            if(Rval != CAL_OK) {
                Rval = CAL_ERR_2;
            }
            Rval |= CT_LdcDumpCfg(&Cfg);
            Rval |= AmbaCal_LdcGenWarp((const AMBA_CAL_LDC_CFG_s*)&Cfg, pLdcCalibWorkingBuf, &pLdcWorkingBuf->CalibData);
        }
    }
    return Rval;
}

UINT32 AmbaCT_LdcGetCalibData(AMBA_CT_LDC_CALIB_DATA_s *pData)
{
    UINT32 Rval = 0;
    pData->pCalibData = &pLdcWorkingBuf->CalibData;
    return Rval;
}

const AMBA_CT_LDC_USER_SETTING_s* AmbaCT_LdcGetUserSetting(void)
{
    return (const AMBA_CT_LDC_USER_SETTING_s*) &CTUserLdcGroupData;
}
