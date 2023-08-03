/**
 *  @file AmbaCT_BpcTuner.c
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
#include "AmbaCT_BpcTuner.h"
#include "AmbaCalib_StaticBadPixelIF.h"
#include "AmbaCalib_ErrNo.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCT_Logging.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

typedef struct {
    AMBA_CAL_BPC_CALIB_DATA_V1_s CalibData;
} AMBA_CT_BPC_WORKING_BUF_s;

static AMBA_CT_BPC_WORKING_BUF_s *pBpcTunerWorkingBuf;
static AMBA_CT_BPC_USER_SETTING_s CTUserBpcGroupData GNU_SECTION_NOZEROINIT;
static void *pBpcCalibWorkingBuf = NULL;
//#Note: static SIZE_t BpcCalibWorkingBufSize = 0U;
static void *pCTSrcRawBuf = NULL;
static SIZE_t CTSrcRawBufSize = 0U;
static UINT8 BpcTunerValidGroup[AMBA_CT_BPC_TUNER_MAX];
static UINT32 (*pCTFeedRawFunc)(const char *pFileName, UINT16 *pRawBuf, SIZE_t RawBufSize);

static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static inline void AmbaCT_BpcSetGroupValid(AMBA_CT_BPC_TUNER_GROUP_e GroupId)
{
    if (GroupId < AMBA_CT_BPC_TUNER_MAX) {
        BpcTunerValidGroup[GroupId] = 1;
    }
}

static const char* AmbaCT_BpcGetGroupName(UINT8 GroupId)
{
    static const struct {
        const char *const GroupName;
        AMBA_CT_BPC_TUNER_GROUP_e GroupId;
    } GroupLut[AMBA_CT_BPC_TUNER_MAX] = {
        {"AMBA_CT_BPC_TUNER_SYSTEM_INFO", AMBA_CT_BPC_TUNER_SYSTEM_INFO},
        {"AMBA_CT_BPC_TUNER_RAW_INFO", AMBA_CT_BPC_TUNER_RAW_INFO},
        {"AMBA_CT_BPC_TUNER_VIN", AMBA_CT_BPC_TUNER_VIN},
        {"AMBA_CT_BPC_TUNER_BPC", AMBA_CT_BPC_TUNER_BPC},
        {"AMBA_CT_BPC_TUNER_OB", AMBA_CT_BPC_TUNER_OB},
    };
    const char *Rval = "CALIB_TUNER_UNKNOWN_GROUP";
    UINT32 Index;
    for (Index = 0; Index < (sizeof(GroupLut)/sizeof(GroupLut[0])); Index++) {
        if (GroupId == (UINT8)GroupLut[Index].GroupId) {
            Rval = GroupLut[Index].GroupName;
            break;
        }
    }
    return (const char *const) Rval;
}

static UINT32 CT_BpcCheckGroup(void)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    static const UINT8 NecessaryGroup[] = {
        (UINT8)AMBA_CT_BPC_TUNER_SYSTEM_INFO,
        (UINT8)AMBA_CT_BPC_TUNER_RAW_INFO,
        (UINT8)AMBA_CT_BPC_TUNER_VIN,
        (UINT8)AMBA_CT_BPC_TUNER_BPC,
        (UINT8)AMBA_CT_BPC_TUNER_OB,
    };
    UINT32 Index;
    for (Index = 0; Index < (sizeof(NecessaryGroup)/sizeof(NecessaryGroup[0])); Index++) {
        if (BpcTunerValidGroup[NecessaryGroup[Index]] == 0U) {
            pNextMsg = CT_LogPutStr(Msg, "Lose Group");
            pNextMsg = CT_LogPutStr(pNextMsg, AmbaCT_BpcGetGroupName(NecessaryGroup[Index]));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CAL_ERR_1;
        }
    }
    return Rval;
}


void AmbaCT_BpcGetSystemInfo(AMBA_CT_BPC_TUNER_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserBpcGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_BpcSetSystemInfo(const AMBA_CT_BPC_TUNER_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserBpcGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_BpcSetGroupValid(AMBA_CT_BPC_TUNER_SYSTEM_INFO);
}

void AmbaCT_BpcGetRawInfo(AMBA_CT_BPC_TUNER_RAW_INFO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserBpcGroupData.RawInfo, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_BpcSetRawInfo(const AMBA_CT_BPC_TUNER_RAW_INFO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserBpcGroupData.RawInfo, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_BpcSetGroupValid(AMBA_CT_BPC_TUNER_RAW_INFO);
}

void AmbaCT_BpcGetVin(AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserBpcGroupData.Vin, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_BpcSetVin(const AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserBpcGroupData.Vin, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_BpcSetGroupValid(AMBA_CT_BPC_TUNER_VIN);
}

void AmbaCT_BpcGetBpc(AMBA_CT_BPC_TUNER_BPC_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserBpcGroupData.Bpc, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_BpcSetBpc(const AMBA_CT_BPC_TUNER_BPC_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserBpcGroupData.Bpc, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_BpcSetGroupValid(AMBA_CT_BPC_TUNER_BPC);
}

void AmbaCT_BpcGetOb(AMBA_CAL_BPC_OB_INFO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserBpcGroupData.Ob, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_BpcSetOb(const AMBA_CAL_BPC_OB_INFO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserBpcGroupData.Ob, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_BpcSetGroupValid(AMBA_CT_BPC_TUNER_OB);
}

void AmbaCT_BpcGetWorkingBufSize(SIZE_t *pSize)
{
    *pSize = sizeof(AMBA_CT_BPC_WORKING_BUF_s);
}

static inline void CT_BpcResetGData2Default(void)
{
    CT_CheckRval(AmbaWrap_memset(&CTUserBpcGroupData, 0x0, sizeof(CTUserBpcGroupData)), "AmbaWrap_memset", __func__);
}

UINT32 AmbaCT_BpcTunerInit(const AMBA_CT_BPC_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    if (pInitCfg->WorkingBufSize < sizeof(AMBA_CT_BPC_WORKING_BUF_s)) {
        pNextMsg = CT_LogPutStr(Msg, "Working buffer size ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(pInitCfg->WorkingBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, "incorrect, necessary size: ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(sizeof(AMBA_CT_BPC_WORKING_BUF_s)));
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CAL_ERR_1;
    } else {
        CT_CheckRval(AmbaWrap_memset(BpcTunerValidGroup, 0x0, sizeof(BpcTunerValidGroup)), "AmbaWrap_memset", __func__);
        CT_CheckRval(AmbaWrap_memcpy(&pBpcTunerWorkingBuf, &pInitCfg->pWorkingBuf, sizeof(AMBA_CT_BPC_WORKING_BUF_s *)), "AmbaWrap_memcpy", __func__);
        CT_CheckRval(AmbaWrap_memset(pBpcTunerWorkingBuf, 0x0, sizeof(AMBA_CT_BPC_WORKING_BUF_s)), "AmbaWrap_memset", __func__);

        pCTSrcRawBuf = pInitCfg->pSrcRawBuf;
        CTSrcRawBufSize = pInitCfg->SrcRawBufSize;

        pBpcCalibWorkingBuf = pInitCfg->pCalibWorkingBuf;
        //#Note: BpcCalibWorkingBufSize = pInitCfg->WorkingBufSize;

        pCTFeedRawFunc = pInitCfg->FeedRawFunc;
        CT_BpcResetGData2Default();
    }
    return Rval;
}

UINT8 AmbaCT_BpcGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_BPC_TUNER_MAX) ? BpcTunerValidGroup[GroupId] : 0U;
}

static AMBA_CAL_BPC_ALGO_e CT_BpcUint8ToAlgo(UINT8 Algo)
{
    AMBA_CAL_BPC_ALGO_e Rval = AMBA_CAL_BPC_ALGO_MAX;
    static const AMBA_CAL_BPC_ALGO_e Lut[AMBA_CAL_BPC_ALGO_MAX] = {
        AMBA_CAL_BPC_ALGO_HOT_PIXEL,
        AMBA_CAL_BPC_ALGO_DARK_PIXEL,
    };
    if (Algo < (UINT8)AMBA_CAL_BPC_ALGO_MAX) {
        Rval = Lut[Algo];
    }
    return Rval;
}

static AMBA_CAL_BPC_TH_MODE_e CT_BpcUint8ToThMode(UINT8 ThMode)
{
    AMBA_CAL_BPC_TH_MODE_e Rval = AMBA_CAL_BPC_TH_MAX;
    static const AMBA_CAL_BPC_TH_MODE_e Lut[AMBA_CAL_BPC_TH_MAX] = {
        AMBA_CAL_BPC_TH_RELATIVE,
        AMBA_CAL_BPC_TH_DIFFERENCE
    };
    if (ThMode < (UINT8)AMBA_CAL_BPC_TH_MAX) {
        Rval = Lut[ThMode];
    }
    return Rval;
}

static void CT_BpcDumpCfg(const AMBA_CAL_BPC_CFG_V2_s *pCfg)
{
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.StartX:");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.StartX);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.StartY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.StartY);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.Width: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.Width);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.Height: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.Height);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.HSubSample.FactorNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.HSubSample.FactorNum);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.HSubSample.FactorDen: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.HSubSample.FactorDen);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.VSubSample.FactorNum: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.VSubSample.FactorNum);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->VinSensorGeo.VSubSample.FactorDen: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->VinSensorGeo.VSubSample.FactorDen);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Algo: ");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pCfg->Algo);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->ThMode: ");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32) pCfg->ThMode);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->UpperTh: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->UpperTh);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->LowerTh: ");
    pNextMsg = CT_LogPutDB(pNextMsg, pCfg->LowerTh);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->BlockWidth: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->BlockWidth);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->BlockHeight: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->BlockHeight);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->CalibOffsetX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->CalibOffsetX);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->OB.OBEnable: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->OB.OBEnable);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->OB.OBWidth: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->OB.OBWidth);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->OB.OBHeight: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->OB.OBHeight);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->OB.OBOffsetX: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->OB.OBOffsetX);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->OB.OBOffsetY: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->OB.OBOffsetY);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->OB.OBPitch: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->OB.OBPitch);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->SensorType: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->SensorType);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->Bayer: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->Bayer);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
    pNextMsg = CT_LogPutStr(Msg, "pCfg->IrMode: ");
    pNextMsg = CT_LogPutU32(pNextMsg, pCfg->IrMode);
    (void) CT_LogPackMsg(CT_LOG_LEVEL_INFO, Msg, pNextMsg);
}

UINT32 AmbaCT_BpcExecute(void)
{
    UINT32 Rval = CAL_OK;
    if (CT_BpcCheckGroup() != 0UL) {
        Rval |= CAL_ERR_1;
    } else {
        AMBA_CAL_BPC_CFG_V2_s Cfg;
        AMBA_CAL_SIZE_s RawSize;

        CT_CheckRval(AmbaWrap_memcpy(&Cfg.VinSensorGeo, &CTUserBpcGroupData.Vin, sizeof(Cfg.VinSensorGeo)), "AmbaWrap_memcpy", __func__);
        Cfg.Algo = CT_BpcUint8ToAlgo(CTUserBpcGroupData.Bpc.Algo);
        Cfg.ThMode = CT_BpcUint8ToThMode(CTUserBpcGroupData.Bpc.ThMode);
        Cfg.UpperTh = CTUserBpcGroupData.Bpc.UpperTh;
        Cfg.LowerTh = CTUserBpcGroupData.Bpc.LowerTh;
        Cfg.BlockWidth = CTUserBpcGroupData.Bpc.BlockWidth;
        Cfg.BlockHeight = CTUserBpcGroupData.Bpc.BlockHeight;
        Cfg.CalibOffsetX = CTUserBpcGroupData.RawInfo.CalibOffsetX;
        Cfg.Bayer = CTUserBpcGroupData.RawInfo.Bayer;
        Cfg.SensorType = CTUserBpcGroupData.RawInfo.SensorType;
        Cfg.IrMode = CTUserBpcGroupData.RawInfo.IrMode;
        RawSize.Width = CTUserBpcGroupData.RawInfo.Area.Width;
        RawSize.Height = CTUserBpcGroupData.RawInfo.Area.Height;

        CT_CheckRval(AmbaWrap_memcpy(&Cfg.OB, &CTUserBpcGroupData.Ob, sizeof(Cfg.OB)), "AmbaWrap_memcpy", __func__);

        if (CTUserBpcGroupData.RawInfo.FromFile == 1U) {
            if ((pCTSrcRawBuf != NULL) && (CTSrcRawBufSize > 0U)) {
                UINT16 *pInRawBuf = NULL;
                CT_CheckRval(AmbaWrap_memcpy(&pInRawBuf, &pCTSrcRawBuf, sizeof(UINT16 *)), "AmbaWrap_memcpy", __func__);
                Rval |= pCTFeedRawFunc(CTUserBpcGroupData.RawInfo.Path, pInRawBuf, CTSrcRawBufSize);
            } else {
                AmbaPrint_PrintUInt5("AmbaCT_BpcExecute Error: Raw buffer is null.", 0U, 0U, 0U, 0U, 0U);
                Rval |= CAL_ERR_1;
            }
        } else {
            //Note # Not support
            Rval |= CAL_ERR_1;
        }
        CT_BpcDumpCfg(&Cfg);
        Rval |= AmbaCal_BpcRaw2BpcTblV2(&Cfg, pBpcCalibWorkingBuf, pCTSrcRawBuf, &RawSize, CTUserBpcGroupData.RawInfo.Pitch, &pBpcTunerWorkingBuf->CalibData);
    }
    return Rval;
}

UINT32 AmbaCT_BpcGetCalibData(AMBA_CT_BPC_CALIB_DATA_s *pData)
{
    UINT32 Rval = 0U;
    pData->pCalibData = &pBpcTunerWorkingBuf->CalibData;
    return Rval;
}


const AMBA_CT_BPC_USER_SETTING_s* AmbaCT_BpcGetUserSetting(void)
{
    return (const AMBA_CT_BPC_USER_SETTING_s*) &CTUserBpcGroupData;
}
