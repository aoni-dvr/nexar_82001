/**
 *  @file AmbaCT_CaTuner.c
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
#include "AmbaCT_CaTuner.h"
#include "AmbaCalib_Camera.h"
#include "AmbaCalib_CaDef.h"
#include "AmbaCalib_ErrNo.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCT_Logging.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

typedef struct {
    AMBA_CAL_CA_DATA_s CalibData;
    AMBA_CAL_CA_SEPARATE_DATA_s CalibSeparateData;
    DOUBLE CaExpect[MAX_LENS_CATBL_LEN];
    DOUBLE CaRLine[MAX_LENS_CATBL_LEN];
    DOUBLE CaBLine[MAX_LENS_CATBL_LEN];
} AMBA_CT_CA_WORKING_BUF_s;

static UINT8 CaTunerValidGroup[AMBA_CT_CA_MAX];
static AMBA_CT_CA_USER_SETTING_s CTUserCaGroupData GNU_SECTION_NOZEROINIT;
static AMBA_CT_CA_WORKING_BUF_s *pCaWorkingBuf = NULL;
static void *pCaCalibWorkingBuf = NULL;
static SIZE_t CaCalibWorkingBufSize = 0U;

static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static inline void AmbaCT_CaSetGroupValid(AMBA_CT_CA_TUNER_GROUP_e GroupId)
{
    if (GroupId < AMBA_CT_CA_MAX) {
        CaTunerValidGroup[GroupId] = 1U;
    }
}

void AmbaCT_CaGetSystem(AMBA_CT_CA_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserCaGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_CaSetSystem(const AMBA_CT_CA_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserCaGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_CaSetGroupValid(AMBA_CT_CA_SYSTEM_INFO);
}

void AmbaCT_CaGetLens(AMBA_CT_CA_LENS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserCaGroupData.Lens, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_CaSetLens(const AMBA_CT_CA_LENS_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserCaGroupData.Lens, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_CaSetGroupValid(AMBA_CT_CA_LENS);
}

void AmbaCT_CaGetSensor(AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserCaGroupData.Sensor, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_CaSetSensor(const AMBA_CAL_SENSOR_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserCaGroupData.Sensor, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_CaSetGroupValid(AMBA_CT_CA_SENSOR);
}

void AmbaCT_CaGetOpticalCenter(AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserCaGroupData.OpticalCenter, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_CaSetOpticalCenter(const AMBA_CAL_POINT_DB_2D_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserCaGroupData.OpticalCenter, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_CaSetGroupValid(AMBA_CT_CA_OPTICAL_CENTER);
}

void AmbaCT_CaGetVin(AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserCaGroupData.Vin, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_CaSetVin(const AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserCaGroupData.Vin, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_CaSetGroupValid(AMBA_CT_CA_VIN);
}

void AmbaCT_CaGetTileSize(AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserCaGroupData.TileSize, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_CaSetTileSize(const AMBA_CAL_SIZE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserCaGroupData.TileSize, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_CaSetGroupValid(AMBA_CT_CA_TILE_SIZE);
}

void AmbaCT_CaGetAdjust(AMBA_CAL_CA_ADJUST_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserCaGroupData.Adjust, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_CaSetAdjust(const AMBA_CAL_CA_ADJUST_CFG_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserCaGroupData.Adjust, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_CaSetGroupValid(AMBA_CT_CA_ADJUST);
}

void AmbaCT_CaGetWorkingBufSize(SIZE_t *pSize)
{
    *pSize = sizeof(AMBA_CT_CA_WORKING_BUF_s);
}

static inline void CT_CaResetGData2Default(void)
{
    CT_CheckRval(AmbaWrap_memset(&CTUserCaGroupData, 0x0, sizeof(CTUserCaGroupData)), "AmbaWrap_memset", __func__);
    CTUserCaGroupData.Lens.pExpecttable = pCaWorkingBuf->CaExpect;
    CTUserCaGroupData.Lens.pCaRedTable  = pCaWorkingBuf->CaRLine;
    CTUserCaGroupData.Lens.pCaBlueTable = pCaWorkingBuf->CaBLine;
}

UINT32 AmbaCT_CaTunerInit(const AMBA_CT_CA_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    if (pInitCfg->WorkingBufSize < sizeof(AMBA_CT_CA_WORKING_BUF_s)) {
        pNextMsg = CT_LogPutStr(Msg, "Working buffer size");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(pInitCfg->WorkingBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, "incorrect, necessary size:");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(sizeof(AMBA_CT_CA_WORKING_BUF_s)));
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CT_ERR_1;
    } else {
        Rval |= AmbaWrap_memset(CaTunerValidGroup, 0x0, sizeof(CaTunerValidGroup));
        Rval |= AmbaWrap_memcpy(&pCaWorkingBuf, &pInitCfg->pWorkingBuf, sizeof(AMBA_CT_CA_WORKING_BUF_s *));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }
        pCaCalibWorkingBuf = pInitCfg->pCalibWorkingBuf;
        CaCalibWorkingBufSize = pInitCfg->CalibWorkingBufSize;
        CT_CaResetGData2Default();
    }
    return Rval;
}

UINT8 AmbaCT_CaGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_CA_MAX) ? CaTunerValidGroup[GroupId] : 0U;
}

static const char* AmbaCT_CaGetGroupName(AMBA_CT_CA_TUNER_GROUP_e GroupId)
{
#define GROUP_LUT_COUNT (7U)
    static volatile const  struct {
        const char *const GroupName;
        AMBA_CT_CA_TUNER_GROUP_e GroupId;
    } GroupLut[GROUP_LUT_COUNT] = {
            {"AMBA_CT_CA_SYSTEM_INFO", AMBA_CT_CA_SYSTEM_INFO},
            {"AMBA_CT_CA_LENS", AMBA_CT_CA_LENS},
            {"AMBA_CT_CA_SENSOR", AMBA_CT_CA_SENSOR},
            {"AMBA_CT_CA_OPTICAL_CENTER", AMBA_CT_CA_OPTICAL_CENTER},
            {"AMBA_CT_CA_ADJUST", AMBA_CT_CA_ADJUST},
            {"AMBA_CT_CA_VIN", AMBA_CT_CA_VIN},
            {"AMBA_CT_CA_TILE_SIZE", AMBA_CT_CA_TILE_SIZE},
    };
    const char *Rval = "CALIB_TUNER_UNKNOWN_GROUP";
    UINT32 Index;

    for (Index = 0U; Index < GROUP_LUT_COUNT; Index++) {
        if (GroupId == GroupLut[Index].GroupId) {
            Rval = GroupLut[Index].GroupName;
            break;
        }
    }
    return (const char *const) Rval;
}

#define NECESSARY_GROUP_COUNT (7U)
static UINT32 CT_CaCheckGroup(void)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 Index;
    static volatile const AMBA_CT_CA_TUNER_GROUP_e NecessaryGroup[NECESSARY_GROUP_COUNT] = {
    AMBA_CT_CA_SYSTEM_INFO,
    AMBA_CT_CA_LENS,
    AMBA_CT_CA_SENSOR,
    AMBA_CT_CA_OPTICAL_CENTER,
    AMBA_CT_CA_ADJUST,
    AMBA_CT_CA_VIN,
    AMBA_CT_CA_TILE_SIZE,
    };

    for (Index = 0U; Index < NECESSARY_GROUP_COUNT; Index++) {
        if (CaTunerValidGroup[NecessaryGroup[Index]] == 0U) {
            pNextMsg = CT_LogPutStr(Msg, "Lose Group ");
            pNextMsg = CT_LogPutStr(pNextMsg, AmbaCT_CaGetGroupName(NecessaryGroup[Index]));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CT_ERR_1;
        }
    }
    return Rval;
}

static void CT_CaDumpCfg(const AMBA_CAL_CA_CFG_s *pCfg)
{
    UINT32 Rval = CT_OK;
    UINT32 Index;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    pNextMsg = CT_LogPutStr(Msg, "pCfg->CaInfo.pCaTbllnfo.Length:");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.pCaTbllnfo->Length);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "Cfg.CaInfo.pCaTbllnfo->pExpectTbl:");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (Index = 0; Index < pCfg->CaInfo.pCaTbllnfo->Length; Index++) {
        pNextMsg = CT_LogPutDB(Msg, pCfg->CaInfo.pCaTbllnfo->pExpectTbl[Index]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    pNextMsg = CT_LogPutStr(Msg, "Cfg.CaInfo.pCaTbllnfo->pRRealTbl:");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (Index = 0; Index < pCfg->CaInfo.pCaTbllnfo->Length; Index++) {
        pNextMsg = CT_LogPutDB(Msg, pCfg->CaInfo.pCaTbllnfo->pRRealTbl[Index]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    pNextMsg = CT_LogPutStr(Msg, "Cfg.CaInfo.pCaTbllnfo->pBRealTbl:");
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    for (Index = 0; Index < pCfg->CaInfo.pCaTbllnfo->Length; Index++) {
        pNextMsg = CT_LogPutDB(Msg, pCfg->CaInfo.pCaTbllnfo->pBRealTbl[Index]);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    }
    pNextMsg = CT_LogPutStr(Msg, "pCfg->OpticalCenter.X: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.CenterX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->OpticalCenter.Y: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.CenterY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->CompensateRatio: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.CompensateRatio);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Method: ");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)pCfg->CaInfo.Method);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->RRatio: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.RRatio);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Zoom_Step: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.ZoomStep);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);


    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.CellSize: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->CaInfo.Sensor.CellSize);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.StartX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.Sensor.StartX);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.Sensor.StartY);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.Sensor.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Cam.Sensor.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CaInfo.Sensor.Height);
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

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Tile.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Tile.Width);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    pNextMsg = CT_LogPutStr(Msg, "pCfg->Tile.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Tile.Height);
    Rval |= CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);

    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error CT_CaDumpCfg() call CT_LogPackMsg() Fail", NULL, NULL, NULL, NULL, NULL);
    }
}

static AMBA_CAL_CA_METHOD_TYPE_e Cal_UINT32toCaMethodType(UINT32 Method)

{
    UINT32 MethodMisra;

    static volatile const AMBA_CAL_CA_METHOD_TYPE_e Lut[] = {
       AMBA_CAL_CA_AUTO,
       AMBA_CAL_CA_B_ONLY,
       AMBA_CAL_CA_R_ONLY,
       AMBA_CAL_CA_B_TBL_SCALE_TO_R,
       AMBA_CAL_CA_R_TBL_SCALE_TO_B,
       AMBA_CAL_CA_AVERAGE,
       AMBA_CAL_CA_SEPARATE,
    };
    if (Method > (((UINT32)AMBA_CAL_CA_MAX)-1U)) {
        MethodMisra = (((UINT8)AMBA_CAL_CA_MAX)-1U);
    } else {
        MethodMisra = Method;
    }
    return Lut[MethodMisra];
}

UINT32 AmbaCT_CaExecute(void)
{
    UINT32 Rval = CT_OK;
    if (CT_CaCheckGroup() != 0U) {
        Rval = CT_ERR_1;
    } else {
        AMBA_CAL_CA_CFG_s Cfg;
        AMBA_CAL_CA_TBL_s CaTbllnfo;
        SIZE_t Size;
        Rval |= AmbaCal_CaGetBufSize(&Size);
        if (CaCalibWorkingBufSize < Size) {
            Rval |= CAL_ERR_1;
        } else {
            CaTbllnfo.Length = CTUserCaGroupData.Lens.TableLen;
            CaTbllnfo.pExpectTbl = CTUserCaGroupData.Lens.pExpecttable;
            CaTbllnfo.pRRealTbl  = CTUserCaGroupData.Lens.pCaRedTable;
            CaTbllnfo.pBRealTbl  = CTUserCaGroupData.Lens.pCaBlueTable;
            Cfg.CaInfo.pCaTbllnfo = &CaTbllnfo;
            Cfg.CaInfo.pCaTbllnfo->Length = CaTbllnfo.Length;

            if(AmbaWrap_memcpy(&Cfg.CaInfo.Sensor,&CTUserCaGroupData.Sensor, sizeof(Cfg.CaInfo.Sensor)) != CT_OK){
                Rval |= CAL_ERR_1;
            }

            Cfg.CaInfo.CenterX = (UINT32)CTUserCaGroupData.OpticalCenter.X;
            Cfg.CaInfo.CenterY = (UINT32)CTUserCaGroupData.OpticalCenter.Y;

            Cfg.CaInfo.CompensateRatio = CTUserCaGroupData.Adjust.Compensate;
            Cfg.CaInfo.RRatio = CTUserCaGroupData.Adjust.Ratio;
            Cfg.CaInfo.Method = Cal_UINT32toCaMethodType(CTUserCaGroupData.Adjust.Method);
            Cfg.CaInfo.ZoomStep = CTUserCaGroupData.Adjust.Zoom_step;
            Cfg.Tile.Width = CTUserCaGroupData.TileSize.Width;
            Cfg.Tile.Height= CTUserCaGroupData.TileSize.Height;
            if(AmbaWrap_memcpy(&Cfg.VinSensorGeo,&CTUserCaGroupData.Vin, sizeof(Cfg.VinSensorGeo)) != CT_OK){
                Rval |= CAL_ERR_1;
            }
            CT_CaDumpCfg(&Cfg);
            if (Cfg.CaInfo.Method == AMBA_CAL_CA_SEPARATE){
                Rval |= AmbaCal_CaGenSeparatedRBTbl(&Cfg,pCaCalibWorkingBuf,&pCaWorkingBuf->CalibSeparateData);
            } else {
                Rval |= AmbaCal_CaGenCaTbl(&Cfg,pCaCalibWorkingBuf,&pCaWorkingBuf->CalibData);
            }
        }

    }
    return Rval;
}

UINT32 AmbaCT_CaGetCalibData(AMBA_CT_CA_CALIB_DATA_s *pData)
{
    UINT32 Rval = CT_OK;

    pData->Method = Cal_UINT32toCaMethodType(CTUserCaGroupData.Adjust.Method);
    switch(pData->Method) {
        case AMBA_CAL_CA_SEPARATE:
            pData->pSeparateCalibData = &pCaWorkingBuf->CalibSeparateData;
            break;
        default:
            pData->pCalibData = &pCaWorkingBuf->CalibData;
            break;
    }

    return Rval;
}

const AMBA_CT_CA_USER_SETTING_s* AmbaCT_CaGetUserSetting(void)
{
    return (const AMBA_CT_CA_USER_SETTING_s*) &CTUserCaGroupData;
}
