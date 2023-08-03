/**
 *  @file AmbaCT_OcTuner.c
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
#include "AmbaCT_OcTuner.h"
#include "AmbaFS.h"
#include "AmbaCalib_OcIF.h"
#include "AmbaCalib_ErrNo.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCT_Logging.h"
#include "AmbaCT_OcTunerIF.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

typedef struct {
    UINT16 RawBuf[AMBA_CT_OC_MAX_RAW_SIZE];
    AMBA_CAL_OC_CALIB_DATA_s CalibData;
} AMBA_CT_OC_WORKING_BUF_s;

static AMBA_CT_OC_WORKING_BUF_s *pOcWorkingBuf;
static void *pOcCalibWorkingBuf = NULL;

static UINT8 OcTunerValidGroup[AMBA_CT_OC_TUNER_MAX];
static AMBA_CT_OC_USER_SETTING_s CTUserOcGroupData GNU_SECTION_NOZEROINIT;
static UINT32 (*pCTOcFeedRawFunc)(SIZE_t RawBufSize, UINT16 *pRaw);

static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static inline void CT_OcResetGData2Default(void)
{
    CT_CheckRval(AmbaWrap_memset(&CTUserOcGroupData, 0x0, sizeof(CTUserOcGroupData)), "AmbaWrap_memset", __func__);
}

void AmbaCT_OcGetWorkingBufSize(SIZE_t *pSize)
{
    *pSize = sizeof(AMBA_CT_OC_WORKING_BUF_s);
}

UINT32 AmbaCT_OcTunerInit(const AMBA_CT_OC_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    if (pInitCfg->WorkingBufSize < sizeof(AMBA_CT_OC_WORKING_BUF_s)) {
        pNextMsg = CT_LogPutStr(Msg, "Working buffer size ");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(pInitCfg->WorkingBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, "incorrect, necessary size:");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(sizeof(AMBA_CT_OC_WORKING_BUF_s)));
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CAL_ERR_1;
    } else {
        Rval |= AmbaWrap_memset(OcTunerValidGroup, 0x0, sizeof(OcTunerValidGroup));
        Rval |= AmbaWrap_memcpy(&pOcWorkingBuf, &pInitCfg->pWorkingBuf, sizeof(AMBA_CT_OC_WORKING_BUF_s *));
        if(Rval != CAL_OK) {
            Rval = CAL_ERR_2;
        }
        pOcCalibWorkingBuf = pInitCfg->pCalibWorkingBuf;
        pCTOcFeedRawFunc = pInitCfg->CbFeedRawBuf;
        CT_OcResetGData2Default();
    }
    return Rval;
}

static inline void AmbaCT_OcSetGroupValid(AMBA_CT_OC_TUNER_GROUP_e GroupId)
{
    if (GroupId < AMBA_CT_OC_TUNER_MAX) {
        OcTunerValidGroup[GroupId] = 1U;
    }
}

UINT8 AmbaCT_OcGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_OC_TUNER_MAX) ? OcTunerValidGroup[GroupId] : 0U;
}

void AmbaCT_OcGetSystemInfo(AMBA_CT_OC_TUNER_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOcGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcSetSystemInfo(const AMBA_CT_OC_TUNER_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOcGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_OcSetGroupValid(AMBA_CT_OC_TUNER_SYSTEM_INFO);
}

void AmbaCT_OcGetRawInfo(AMBA_CT_OC_TUNER_RAW_INFO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOcGroupData.RawInfo, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcSetRawInfo(const AMBA_CT_OC_TUNER_RAW_INFO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOcGroupData.RawInfo, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_OcSetGroupValid(AMBA_CT_OC_TUNER_RAW_INFO);
}

void AmbaCT_OcGetVin(AMBA_CT_OC_TUNER_VIN_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOcGroupData.Vin, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcSetVin(const AMBA_CT_OC_TUNER_VIN_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOcGroupData.Vin, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_OcSetGroupValid(AMBA_CT_OC_TUNER_VIN);
}

void AmbaCT_OcGetEllipseInfo(AMBA_CT_OC_TUNER_ELLIPSE_INFO_s*pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserOcGroupData.EllipseInfo, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_OcSetEllipseInfo(const AMBA_CT_OC_TUNER_ELLIPSE_INFO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserOcGroupData.EllipseInfo, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_OcSetGroupValid(AMBA_CT_OC_TUNER_ELLIPSE_INFO);
}

static const char* AmbaCT_OcGetGroupName(UINT8 GroupId)
{
    static const struct {
        const char *const GroupName;
        AMBA_CT_OC_TUNER_GROUP_e GroupId;
    } GroupLut[] = {
        {"AMBA_CT_OC_TUNER_SYSTEM_INFO", AMBA_CT_OC_TUNER_SYSTEM_INFO},
        {"AMBA_CT_OC_TUNER_RAW_INFO", AMBA_CT_OC_TUNER_RAW_INFO},
        {"AMBA_CT_OC_TUNER_VIN", AMBA_CT_OC_TUNER_VIN},
        {"AMBA_CT_OC_TUNER_ELLIPSE_INFO", AMBA_CT_OC_TUNER_ELLIPSE_INFO},
    };
    const char *Rval = "CALIB_TUNER_UNKNOWN_GROUP";
    UINT32 Index;
    for (Index = 0U; Index < (sizeof(GroupLut)/sizeof(GroupLut[0])); Index++) {
        if (GroupId == (UINT8)GroupLut[Index].GroupId) {
            Rval = GroupLut[Index].GroupName;
            break;
        }
    }
    return (const char *const) Rval;
}

static UINT32 AmbaCT_OcCheckGroup(void)
{
    UINT32 Rval = CAL_OK;
    static const UINT8 NecessaryGroup[] = {
        (UINT8)AMBA_CT_OC_TUNER_SYSTEM_INFO,
        (UINT8)AMBA_CT_OC_TUNER_RAW_INFO,
        (UINT8)AMBA_CT_OC_TUNER_VIN,
        (UINT8)AMBA_CT_OC_TUNER_ELLIPSE_INFO,
    };
    UINT32 Index;
    for (Index = 0U; Index < (sizeof(NecessaryGroup)/sizeof(NecessaryGroup[0])); Index++) {
        if (OcTunerValidGroup[NecessaryGroup[Index]] == 0U) {
            AmbaPrint_PrintStr5("Lose Group %s", AmbaCT_OcGetGroupName(NecessaryGroup[Index]), DC_S, DC_S, DC_S, DC_S);
            Rval |= CAL_ERR_1;
        }
    }
    return Rval;
}

UINT32 AmbaCT_OcExecute(void)
{
    UINT32 Rval = CAL_OK;
    if (AmbaCT_OcCheckGroup() != 0UL) {
        Rval |= CAL_ERR_1;
    } else {
        AMBA_CAL_OC_CFG_s Cfg;
        Cfg.pRaw = pOcWorkingBuf->RawBuf;
        Cfg.Bayer = CTUserOcGroupData.RawInfo.Bayer;
        Cfg.VinSensorGeo = CTUserOcGroupData.Vin.VinSensorGeo;
        Cfg.MinRadius = CTUserOcGroupData.EllipseInfo.MinRadius;
        Cfg.CenterLimit = CTUserOcGroupData.EllipseInfo.CenterLimit;
        Cfg.SplitCircle = CTUserOcGroupData.EllipseInfo.SplitCircle;
        if (CTUserOcGroupData.RawInfo.FromFile == 1U) {
            Rval |= pCTOcFeedRawFunc(AMBA_CT_OC_MAX_RAW_SIZE*sizeof(UINT16), pOcWorkingBuf->RawBuf);
        } else {
            //Note # Not support
            Rval |= CAL_ERR_1;
        }
        Rval |= AmbaCal_OcFinder(&Cfg, pOcCalibWorkingBuf, &pOcWorkingBuf->CalibData);
    }
    return Rval;
}

UINT32 AmbaCT_OcGetCalibData(AMBA_CT_OC_CALIB_DATA_s *pData)
{
    UINT32 Rval = 0;
    pData->pCalibData = &pOcWorkingBuf->CalibData;
    return Rval;
}

const AMBA_CT_OC_USER_SETTING_s* AmbaCT_OcGetUserSetting(void)
{
    return (const AMBA_CT_OC_USER_SETTING_s*) &CTUserOcGroupData;
}
