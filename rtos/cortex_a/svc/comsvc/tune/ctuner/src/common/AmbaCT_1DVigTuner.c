/**
 *  @file AmbaCT_1DVigTuner.c
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
#include "AmbaCT_1DVigTuner.h"
#include "AmbaCT_1DVigTunerIF.h"
#include "AmbaCalib_VignetteCV2IF.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCT_Logging.h"
#include "AmbaCT_1DVigTunerIF.h"
#include "AmbaMisraFix.h"

#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

typedef struct {
    UINT32 ChanIdx;
    AMBA_CAL_1D_VIG_CALIB_DATA_s CalibData[4];
    AMBA_CAL_1D_VIG_CALIB_DATA_V1_s CalibDataV1[4];
    AMBA_CAL_1D_VIG_CALIB_DATA_V1_s VigSyncSrc[4];
    AMBA_CAL_1D_VIG_CALIB_DATA_V1_s VigSyncDst[4];
    AMBA_CAL_POINT_UINT_2D_s EvaluationLocations[4][AMBA_CAL_VIG_EVA_MAX_SAMPLE_NUM];
    AMBA_CAL_1D_VIG_EVA_REPORT_s EvaluationReport[4][AMBA_CAL_VIG_EVA_MAX_SAMPLE_NUM];
} AMBA_CT_1D_VIG_WORKING_BUF_s;

static AMBA_CT_1D_VIG_WORKING_BUF_s *pCTVigWorkingBuf;
static void *pCTVigCalibWorkingBuf = NULL;
static SIZE_t CTVigCalibWorkingBufSize = 0U;
static UINT8 VigTunerValidGroup[AMBA_CT_1D_VIG_TUNER_MAX];
static UINT32 (*pCTFeedRawFuncV1)(UINT32 CamId, void *pRawBuf, SIZE_t RawBufSize) = NULL;
static UINT32 (*pCTFeedVigGainTbl)(UINT32 CamId, AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pCalibData) = NULL;
static void *pCTSrcRawBuf = NULL;
static SIZE_t CTSrcRawBufSize = 0U;
static AMBA_CT_1D_VIG_USET_SETTING_s CTUserVigGroupData GNU_SECTION_NOZEROINIT;
static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}
static inline void CT_1DVigResetGData2Default(void)
{
    CT_CheckRval(AmbaWrap_memset(&CTUserVigGroupData, 0x0, sizeof(CTUserVigGroupData)), "AmbaWrap_memset", __func__);
}

void AmbaCT_1DVigGetWorkingBufSize(SIZE_t *pSize)
{
    *pSize = sizeof(AMBA_CT_1D_VIG_WORKING_BUF_s);
}

UINT32 AmbaCT_1DVigGetActiveChanIdx(void)
{
    return pCTVigWorkingBuf->ChanIdx;
}

static void CT_1DVigSetActiveChanIdx(UINT32 ChanIdx)
{
    pCTVigWorkingBuf->ChanIdx = ChanIdx;
}

UINT32 AmbaCT_1DVigTunerInit(const AMBA_CT_1D_VIG_INIT_CFG_s *pInitCfg)
{
    UINT32 Rval = CT_OK;
    UINT32 i;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    CT_1DVigResetGData2Default();
    if (pInitCfg->WorkingBufSize < sizeof(AMBA_CT_1D_VIG_WORKING_BUF_s)) {
        pNextMsg = CT_LogPutStr(Msg, "Working buffer size");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(pInitCfg->WorkingBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, "incorrect, necessary size:");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(sizeof(AMBA_CT_1D_VIG_WORKING_BUF_s)));
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CT_ERR_1;
    } else {
        CT_CheckRval(AmbaWrap_memset(VigTunerValidGroup, 0x0, sizeof(VigTunerValidGroup)), "AmbaWrap_memset", __func__);
        CT_CheckRval(AmbaWrap_memcpy(&pCTVigWorkingBuf, &pInitCfg->pWorkingBuf, sizeof(AMBA_CT_1D_VIG_WORKING_BUF_s *)), "AmbaWrap_memcpy", __func__);
        pCTVigCalibWorkingBuf = pInitCfg->pCalibWorkingBuf;
        CTVigCalibWorkingBufSize = pInitCfg->CalibWorkingBufSize;
        pCTFeedRawFuncV1 = pInitCfg->pFeedRawFuncV1;
        pCTFeedVigGainTbl = pInitCfg->pFeedGainTbl;
        pCTSrcRawBuf = pInitCfg->pSrcRawBuf;
        CTSrcRawBufSize = pInitCfg->SrcRawBufSize;
        for (i = 0U; i < 4U; i++) {
            CTUserVigGroupData.Vignette[i].MaxCenterXOffset = 9999U;
            CTUserVigGroupData.Vignette[i].MaxCenterYOffset = 9999U;
        }
    }
    return Rval;
}

static inline void AmbaCT_1DVigSetGroupValid(AMBA_CT_1D_VIG_TUNER_GROUP_e GroupId)
{
    if ((UINT32)GroupId < (UINT32)AMBA_CT_1D_VIG_TUNER_MAX) {
        VigTunerValidGroup[(UINT32)GroupId] = 1U;
    }
}

UINT8 AmbaCT_1DVigGetGroupStatus(UINT8 GroupId)
{
    return (GroupId < (UINT8)AMBA_CT_1D_VIG_TUNER_MAX) ? VigTunerValidGroup[GroupId] : 0U;
}

void AmbaCT_1DVigGetSystemInfo(AMBA_CT_1D_VIG_TUNER_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserVigGroupData.System, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_1DVigSetSystemInfo(const AMBA_CT_1D_VIG_TUNER_SYSTEM_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserVigGroupData.System, pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_1DVigSetGroupValid(AMBA_CT_1D_VIG_TUNER_SYSTEM_INFO);
}

void AmbaCT_1DVigGetRawInfo(UINT32 ArrayIndex, AMBA_CT_1D_VIG_TUNER_RAW_INFO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserVigGroupData.RawInfo[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_1DVigSetRawInfo(UINT32 ArrayIndex, const AMBA_CT_1D_VIG_TUNER_RAW_INFO_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserVigGroupData.RawInfo[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_1DVigSetGroupValid(AMBA_CT_1D_VIG_TUNER_RAW_INFO);
}

void AmbaCT_1DVigGetVignette(UINT32 ArrayIndex, AMBA_CT_1D_VIG_TUNER_VIGNETTE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserVigGroupData.Vignette[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_1DVigSetVignette(UINT32 ArrayIndex, const AMBA_CT_1D_VIG_TUNER_VIGNETTE_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserVigGroupData.Vignette[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_1DVigSetGroupValid(AMBA_CT_1D_VIG_TUNER_VIGNETTE);
}

void AmbaCT_1DVigGetEvaluationLoc(UINT32 ArrayIndex, AMBA_CT_1D_VIG_EVALUATION_LOC_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserVigGroupData.EvaluationLocation[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_1DVigSetEvaluationLoc(UINT32 ArrayIndex, const AMBA_CT_1D_VIG_EVALUATION_LOC_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserVigGroupData.EvaluationLocation[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_1DVigSetGroupValid(AMBA_CT_1D_VIG_TUNER_EVA_LOC);
}

void AmbaCT_1DVigGetEvaluationCalibData(UINT32 ArrayIndex, AMBA_CT_1D_VIG_EVA_CALIB_DATA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(pData, &CTUserVigGroupData.EvaCalibData[ArrayIndex], sizeof(*pData)), "AmbaWrap_memcpy", __func__);
}

void AmbaCT_1DVigSetEvaluationCalibData(UINT32 ArrayIndex, const AMBA_CT_1D_VIG_EVA_CALIB_DATA_s *pData)
{
    CT_CheckRval(AmbaWrap_memcpy(&CTUserVigGroupData.EvaCalibData[ArrayIndex], pData, sizeof(*pData)), "AmbaWrap_memcpy", __func__);
    AmbaCT_1DVigSetGroupValid(AMBA_CT_1D_VIG_TUNER_CALIB_INFO);
}


static const char* AmbaCT_1DVigGetGroupName(AMBA_CT_1D_VIG_TUNER_GROUP_e GroupId)
{
#define GROUP_LUT_COUNT (5)
    static volatile const struct {
        const char *const GroupName;
        AMBA_CT_1D_VIG_TUNER_GROUP_e GroupId;
    } GroupLut[GROUP_LUT_COUNT] = {
            {"AMBA_CT_1D_VIG_TUNER_SYSTEM_INFO", AMBA_CT_1D_VIG_TUNER_SYSTEM_INFO},
            {"AMBA_CT_1D_VIG_TUNER_RAW_INFO", AMBA_CT_1D_VIG_TUNER_RAW_INFO},
            {"AMBA_CT_1D_VIG_TUNER_VIGNETTE", AMBA_CT_1D_VIG_TUNER_VIGNETTE},
            {"AMBA_CT_1D_VIG_TUNER_EVA_LOC", AMBA_CT_1D_VIG_TUNER_EVA_LOC},
            {"AMBA_CT_1D_VIG_TUNER_CALIB_INFO", AMBA_CT_1D_VIG_TUNER_CALIB_INFO},
    };
    const char *Rval = "CALIB_TUNER_UNKNOWN_GROUP";
    INT32 i;

    for (i = 0; i < GROUP_LUT_COUNT; i++) {
        if (GroupId == GroupLut[i].GroupId) {
            Rval = GroupLut[i].GroupName;
            break;
        }
    }
    return (const char *const) Rval;
}

static UINT32 CT_1DVigCalibCheckGroup(const AMBA_CT_1D_VIG_TUNER_GROUP_e * const pNecessaryGroup, UINT32 GroupCount)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 i;
    for (i = 0U; i < GroupCount; i++) {
        if (VigTunerValidGroup[pNecessaryGroup[i]] == 0U) {
            pNextMsg = CT_LogPutStr(Msg, "Loss Group");
            pNextMsg = CT_LogPutStr(pNextMsg, AmbaCT_1DVigGetGroupName(pNecessaryGroup[i]));
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_DEBUG, Msg, pNextMsg);
            Rval |= CT_ERR_1;
        }
    }
    return Rval;
}




static inline UINT32 CT_1DVigSync(void)
{
    UINT32 i;
    UINT32 SyncNum = 0U;
    UINT32 Rval = CT_OK;
    UINT32 VigSyncDataIdx = 0U;
    for (i = 0; i < CTUserVigGroupData.System.ChanNum; i++) {
        if (CTUserVigGroupData.Vignette[i].Sync != 0U) {
            Rval |= AmbaWrap_memcpy(&pCTVigWorkingBuf->VigSyncSrc[SyncNum], &pCTVigWorkingBuf->CalibDataV1[i], sizeof(pCTVigWorkingBuf->VigSyncSrc[SyncNum]));
            SyncNum++;
        }
    }
    if (SyncNum != 0U) {
        Rval |= AmbaCal_1DVigSyncTblV1(SyncNum, CTUserVigGroupData.System.SyncMaxDiffTh, pCTVigWorkingBuf->VigSyncSrc, pCTVigWorkingBuf->VigSyncDst);
        for (i = 0; i < CTUserVigGroupData.System.ChanNum; i++) {
            if (CTUserVigGroupData.Vignette[i].Sync != 0U) {
                Rval |= AmbaWrap_memcpy(&pCTVigWorkingBuf->CalibDataV1[i], &pCTVigWorkingBuf->VigSyncDst[VigSyncDataIdx], sizeof(pCTVigWorkingBuf->CalibDataV1[i]));
                VigSyncDataIdx++;
            }
        }
    }
    if(Rval != CT_OK) {
        Rval = CT_ERR_1;
    }
    return Rval;
}

#define VIG_CALIB_MODE_EVALUATION "EVALUATION"
#define VIG_CALIB_MODE_NORMAL "VIG"

static AMBA_CT_1D_VIG_CALIB_MODE_e CT_VigString2CalibMode(const char *pCalibMode)
{
    AMBA_CT_1D_VIG_CALIB_MODE_e Rval;

    if (0 == CT_strcmp(pCalibMode, VIG_CALIB_MODE_EVALUATION)) {
        Rval = AMBA_CT_VIG_CALIB_MODE_EVALUATION;
    } else if (0 == CT_strcmp(pCalibMode, VIG_CALIB_MODE_NORMAL)) {
        Rval = AMBA_CT_VIG_CALIB_MODE_NORMAL;
    } else {
        Rval = AMBA_CT_VIG_CALIB_MODE_MAX;
    }
    return Rval;
}

static UINT32 CT_1DVigCalibExecute(void)
{
    UINT32 Rval = CT_OK;
#define NECESSARY_VIG_CALIB_GROUP_COUNT (3)
    static const AMBA_CT_1D_VIG_TUNER_GROUP_e NecessaryCalibGroup[NECESSARY_VIG_CALIB_GROUP_COUNT] = {
        AMBA_CT_1D_VIG_TUNER_SYSTEM_INFO,
        AMBA_CT_1D_VIG_TUNER_RAW_INFO,
        AMBA_CT_1D_VIG_TUNER_VIGNETTE,
    };
    if (CT_1DVigCalibCheckGroup(NecessaryCalibGroup, NECESSARY_VIG_CALIB_GROUP_COUNT) != 0U) {
        Rval = CT_ERR_1;
    } else {
        UINT32 i;
        AMBA_CAL_1D_VIG_CFG_V3_s Cfg;
        AMBA_CAL_VIG_MIN_CONR_ERR_CFG_s MinCornerErrAlgoCfg;

        for (i = 0; i < CTUserVigGroupData.System.ChanNum; i++) {
            CT_1DVigSetActiveChanIdx(i);

            Cfg.Strength = CTUserVigGroupData.Vignette[i].Strength;
            Cfg.ChromaRatio = CTUserVigGroupData.Vignette[i].ChromaRatio;
            Cfg.StrengthEffectMode = CTUserVigGroupData.Vignette[i].StrengthEffectMode;
            Cfg.MaxCenterXOffset = CTUserVigGroupData.Vignette[i].MaxCenterXOffset;
            Cfg.MaxCenterYOffset = CTUserVigGroupData.Vignette[i].MaxCenterYOffset;
            Cfg.Bayer = CTUserVigGroupData.RawInfo[i].Bayer;
            Cfg.VinSensorGeo = CTUserVigGroupData.Vignette[i].VinSensorGeo;
            Cfg.BlackLevel = CTUserVigGroupData.RawInfo[i].BlackLevel;
            Cfg.SensorType = CTUserVigGroupData.RawInfo[i].SensorType;
            Cfg.IrMode = CTUserVigGroupData.RawInfo[i].IrMode;
            Cfg.Resolution = (UINT32)CTUserVigGroupData.RawInfo[i].Resolution;



            switch (CTUserVigGroupData.Vignette[i].CenterSearchAlgo) {
                case 0U:
                    Cfg.CenterSearchAlgo = AMBA_CAL_VIG_DEFAULT_ALGO;
                    Cfg.CenterSearchCfg.pMinCornerErrCfg = NULL;
                    break;
                case 1U:
                    Cfg.CenterSearchAlgo = AMBA_CAL_VIG_MIN_CORNER_ERR_ALGO;
                    MinCornerErrAlgoCfg.SearchCenter.X = (DOUBLE)CTUserVigGroupData.Vignette[i].CenterSearchStartX;
                    MinCornerErrAlgoCfg.SearchCenter.Y = (DOUBLE)CTUserVigGroupData.Vignette[i].CenterSearchStartY;
                    MinCornerErrAlgoCfg.SearchRadius = (DOUBLE)CTUserVigGroupData.Vignette[i].CenterSearchRadius;
                    Cfg.CenterSearchCfg.pMinCornerErrCfg = &MinCornerErrAlgoCfg;
                    break;
                default:
                    AmbaPrint_PrintUInt5("[VIG1D] Warning: Invalid algorithm ID %d, set default", CTUserVigGroupData.Vignette[i].CenterSearchAlgo, 0U, 0U, 0U, 0U);
                    Cfg.CenterSearchAlgo = AMBA_CAL_VIG_DEFAULT_ALGO;
                    Cfg.CenterSearchCfg.pMinCornerErrCfg = NULL;
                    break;
            }

            if (CTUserVigGroupData.RawInfo[i].Pitch == CTUserVigGroupData.Vignette[i].VinSensorGeo.Width) {
                // Workaround auto fix
                if (Cfg.Resolution <= 16U) {
                    Cfg.RawPitch = CTUserVigGroupData.Vignette[i].VinSensorGeo.Width * CT_sizeT_to_U32(sizeof(UINT16));
                } else {
                    Cfg.RawPitch = CTUserVigGroupData.Vignette[i].VinSensorGeo.Width * CT_sizeT_to_U32(sizeof(UINT32));
                }
            } else {
                Cfg.RawPitch = CTUserVigGroupData.RawInfo[i].Pitch;
            }
            Cfg.pRawBuf = pCTSrcRawBuf;
            if (pCTFeedRawFuncV1 != NULL) {
                Rval |= pCTFeedRawFuncV1(i, pCTSrcRawBuf, CTSrcRawBufSize);
            }

            Rval |= AmbaCal_1DVigRaw2VigTblV3(&Cfg, pCTVigCalibWorkingBuf, CTVigCalibWorkingBufSize, &pCTVigWorkingBuf->CalibDataV1[i]);
        }
        Rval |= CT_1DVigSync();
    }
    return Rval;
}

static void CT_1DVigDumpEvaInput(const AMBA_CAL_1D_VIG_EVA_CFG_s *pRawInfo, const AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pCalibData)
{
    UINT32 i;
    for (i = 0U; i < pRawInfo->SamplingNum; i++) {
        AmbaPrint_PrintUInt5("pSamplingLocations[%u]: %u %u\n", i, pRawInfo->pSamplingLocations[i].X, pRawInfo->pSamplingLocations[i].Y, 0, 0);
    }
    AmbaPrint_PrintUInt5("SamplingNum: %d\n", pRawInfo->SamplingNum, 0, 0, 0, 0);
    //AmbaPrint_PrintUint5("pRawBuf: %d\n", pRawInfo->pRawBuf, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("ImageSize: %u %u\n", pRawInfo->ImageSize.Width, pRawInfo->ImageSize.Height, 0, 0, 0);
    AmbaPrint_PrintInt5("BlackLevel: %d %d %d %d\n", pRawInfo->BlackLevel.BlackR, pRawInfo->BlackLevel.BlackGr, pRawInfo->BlackLevel.BlackB, pRawInfo->BlackLevel.BlackGb, 0);
    AmbaPrint_PrintUInt5("RawBitsNum: %d\n", pRawInfo->RawBitsNum, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("RawPitch: %d\n", pRawInfo->RawPitch, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("IrMode: %d\n", pRawInfo->IrMode, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("Bayer: %d\n", pRawInfo->Bayer, 0, 0, 0, 0);

    AmbaPrint_PrintUInt5("Version: %d", pCalibData->Version, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("CalibSensorGeo.StartX: %d", pCalibData->CalibSensorGeo.StartX, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("CalibSensorGeo.StartY: %d", pCalibData->CalibSensorGeo.StartY, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("CalibSensorGeo.Width: %d", pCalibData->CalibSensorGeo.Width, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("CalibSensorGeo.Height: %d", pCalibData->CalibSensorGeo.Height, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("CalibSensorGeo.HSubSample.FactorNum: %d", pCalibData->CalibSensorGeo.HSubSample.FactorNum, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("CalibSensorGeo.HSubSample.FactorDen: %d", pCalibData->CalibSensorGeo.HSubSample.FactorDen, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("CalibSensorGeo.VSubSample.FactorNum: %d", pCalibData->CalibSensorGeo.VSubSample.FactorNum, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("CalibSensorGeo.VSubSample.FactorDen: %d", pCalibData->CalibSensorGeo.VSubSample.FactorDen, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("RadialCoarse: %d", pCalibData->RadialCoarse, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("RadialCoarseLog: %d", pCalibData->RadialCoarseLog, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("RadialBinsFine: %d", pCalibData->RadialBinsFine, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("RadialBinsFineLog: %d", pCalibData->RadialBinsFineLog, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("ModelCenterX_R: %d", pCalibData->ModelCenterX_R, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("ModelCenterX_Gr: %d", pCalibData->ModelCenterX_Gr, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("ModelCenterX_B: %d", pCalibData->ModelCenterX_B, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("ModelCenterX_Gb: %d", pCalibData->ModelCenterX_Gb, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("ModelCenterY_R: %d", pCalibData->ModelCenterY_R, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("ModelCenterY_Gr: %d", pCalibData->ModelCenterY_Gr, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("ModelCenterY_B: %d", pCalibData->ModelCenterY_B, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("ModelCenterY_Gb: %d", pCalibData->ModelCenterY_Gb, 0, 0, 0, 0);
    //AmbaPrint_PrintUInt5("VigGainTbl_R[4][128]: %d", pCalibData->VigGainTbl_R[4][128], 0, 0, 0, 0);
    //AmbaPrint_PrintUInt5("VigGainTbl_Gr[4][128]: %d", pCalibData->VigGainTbl_Gr[4][128], 0, 0, 0, 0);
    //AmbaPrint_PrintUInt5("VigGainTbl_B[4][128]: %d", pCalibData->VigGainTbl_B[4][128], 0, 0, 0, 0);
    //AmbaPrint_PrintUInt5("VigGainTbl_Gb[4][128]: %d", pCalibData->VigGainTbl_Gb[4][128], 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("SyncCalInfo: %d %d %d %d", pCalibData->SyncCalInfo[0], pCalibData->SyncCalInfo[1], pCalibData->SyncCalInfo[2], pCalibData->SyncCalInfo[3], 0);
}
#if 0
static void CT_1DVigDumpEvaOutput(UINT32 SamplingNum, const AMBA_CAL_1D_VIG_EVA_REPORT_s *pReport)
{
    UINT32 i;
    for (i = 0U; i < SamplingNum; i++) {
        AmbaPrint_PrintUInt5("[%d]", i, 0, 0, 0, 0);
        AmbaPrint_PrintUInt5("R: %u.%u, Gr: %u.%u",
                (UINT32)pReport[i].Avg9x9R, (UINT32)((pReport[i].Avg9x9R - (DOUBLE)((UINT32)pReport[i].Avg9x9R)) * 10000.0),
                (UINT32)pReport[i].Avg9x9Gr, (UINT32)((pReport[i].Avg9x9Gr - (DOUBLE)((UINT32)pReport[i].Avg9x9Gr)) * 10000.0),
                0);
        AmbaPrint_PrintUInt5("B: %u.%u, Gb: %u.%u",
                (UINT32)pReport[i].Avg9x9B, (UINT32)((pReport[i].Avg9x9B - (DOUBLE)((UINT32)pReport[i].Avg9x9B)) * 10000.0),
                (UINT32)pReport[i].Avg9x9Gb, (UINT32)((pReport[i].Avg9x9Gb - (DOUBLE)((UINT32)pReport[i].Avg9x9Gb)) * 10000.0),
                0);
    }
}
#endif
static UINT32 CT_1DVigEvaluationExecute(void)
{
    UINT32 Rval = CT_OK;
#define NECESSARY_VIG_EVA_GROUP_COUNT (4)
    static const AMBA_CT_1D_VIG_TUNER_GROUP_e NecessaryEvaluationGroup[NECESSARY_VIG_EVA_GROUP_COUNT] = {
        AMBA_CT_1D_VIG_TUNER_SYSTEM_INFO,
        AMBA_CT_1D_VIG_TUNER_RAW_INFO,
        AMBA_CT_1D_VIG_TUNER_EVA_LOC,
        AMBA_CT_1D_VIG_TUNER_CALIB_INFO,
    };
    if (CT_1DVigCalibCheckGroup(NecessaryEvaluationGroup, NECESSARY_VIG_EVA_GROUP_COUNT) != 0U) {
        Rval = CT_ERR_1;
    } else {
        UINT32 i, J;
        AMBA_CAL_1D_VIG_EVA_REPORT_s *pReport;
        AMBA_CAL_1D_VIG_EVA_CFG_s RawInfo;
        AMBA_CAL_1D_VIG_CALIB_DATA_V1_s CalibData;

        for (i = 0U; i < CTUserVigGroupData.System.ChanNum; i++) {
            if (pCTFeedRawFuncV1 != NULL) {
                Rval |= pCTFeedRawFuncV1(i, pCTSrcRawBuf, CTSrcRawBufSize);
            } else {
                AmbaPrint_PrintStr5("%s() pCTFeedRawFuncV1 is NULL", __func__, NULL, NULL, NULL, NULL);
                Rval |= CT_ERR_1;
            }
            RawInfo.SensorType = 0U;
            RawInfo.SamplingNum = CTUserVigGroupData.EvaluationLocation[i].Number;
            RawInfo.pSamplingLocations = pCTVigWorkingBuf->EvaluationLocations[i];
            for (J = 0U; J < RawInfo.SamplingNum; J++) {
                RawInfo.pSamplingLocations[J].X = CTUserVigGroupData.EvaluationLocation[i].X[J];
                RawInfo.pSamplingLocations[J].Y = CTUserVigGroupData.EvaluationLocation[i].Y[J];
            }
            RawInfo.pRawBuf = pCTSrcRawBuf;
            RawInfo.ImageSize.Width = CTUserVigGroupData.RawInfo[i].Area.Width;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.Width;
            RawInfo.ImageSize.Height = CTUserVigGroupData.RawInfo[i].Area.Height;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.Height;
            RawInfo.BlackLevel = CTUserVigGroupData.RawInfo[i].BlackLevel;
            RawInfo.RawBitsNum = CTUserVigGroupData.RawInfo[i].Resolution;
            RawInfo.RawPitch = CTUserVigGroupData.RawInfo[i].Pitch;
            RawInfo.IrMode = CTUserVigGroupData.RawInfo[i].IrMode;
            RawInfo.Bayer = CTUserVigGroupData.RawInfo[i].Bayer;

            CalibData.Version = 0U;
            CalibData.CalibSensorGeo.StartX = CTUserVigGroupData.RawInfo[i].Area.StartX;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.StartX;
            CalibData.CalibSensorGeo.StartY = CTUserVigGroupData.RawInfo[i].Area.StartY;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.StartY;
            CalibData.CalibSensorGeo.Width = CTUserVigGroupData.RawInfo[i].Area.Width;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.Width;
            CalibData.CalibSensorGeo.Height = CTUserVigGroupData.RawInfo[i].Area.Height;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.Height;
            CalibData.CalibSensorGeo.HSubSample.FactorDen = 1U;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.HSubSample.FactorDen;
            CalibData.CalibSensorGeo.HSubSample.FactorNum = 1U;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.HSubSample.FactorNum;
            CalibData.CalibSensorGeo.VSubSample.FactorDen = 1U;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.VSubSample.FactorDen;
            CalibData.CalibSensorGeo.VSubSample.FactorNum = 1U;//CTUserVigGroupData.EvaCalibData[i].VigCalibData.CalibSensorGeo.VSubSample.FactorNum;
            CalibData.RadialCoarse = CTUserVigGroupData.EvaCalibData[i].RadialCoarse;
            CalibData.RadialCoarseLog = CTUserVigGroupData.EvaCalibData[i].RadialCoarseLog;
            CalibData.RadialBinsFine = CTUserVigGroupData.EvaCalibData[i].RadialBinsFine;
            CalibData.RadialBinsFineLog = CTUserVigGroupData.EvaCalibData[i].RadialBinsFineLog;
            CalibData.ModelCenterX_R = CTUserVigGroupData.EvaCalibData[i].ModelCenterX_R;
            CalibData.ModelCenterY_R = CTUserVigGroupData.EvaCalibData[i].ModelCenterY_R;
            CalibData.ModelCenterX_Gr = CTUserVigGroupData.EvaCalibData[i].ModelCenterX_Gr;
            CalibData.ModelCenterY_Gr = CTUserVigGroupData.EvaCalibData[i].ModelCenterY_Gr;
            CalibData.ModelCenterX_B = CTUserVigGroupData.EvaCalibData[i].ModelCenterX_B;
            CalibData.ModelCenterY_B = CTUserVigGroupData.EvaCalibData[i].ModelCenterY_B;
            CalibData.ModelCenterX_Gb = CTUserVigGroupData.EvaCalibData[i].ModelCenterX_Gb;
            CalibData.ModelCenterY_Gb = CTUserVigGroupData.EvaCalibData[i].ModelCenterY_Gb;

            if (pCTFeedVigGainTbl != NULL) {
                Rval |= pCTFeedVigGainTbl(i, &CalibData);
            } else {
                AmbaPrint_PrintStr5("%s() pCTFeedVigGainTbl is NULL", __func__, NULL, NULL, NULL, NULL);
                Rval |= CT_ERR_1;
            }
            CalibData.SyncCalInfo[0] = 0U;
            CalibData.SyncCalInfo[1] = 0U;
            CalibData.SyncCalInfo[2] = 0U;
            CalibData.SyncCalInfo[3] = 0U;

            CT_1DVigDumpEvaInput(&RawInfo, &CalibData);
            Rval |= AmbaCal_1DVigEvaluationReport(&RawInfo, &CalibData, pCTVigCalibWorkingBuf, CTVigCalibWorkingBufSize, &pReport);
#if 0
            CT_1DVigDumpEvaOutput(RawInfo.SamplingNum, pReport);
#endif
            Rval |= AmbaWrap_memcpy(pCTVigWorkingBuf->EvaluationReport[i], pReport, sizeof(AMBA_CAL_1D_VIG_EVA_REPORT_s) * RawInfo.SamplingNum);

        }
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

    }
    return Rval;
}

UINT32 AmbaCT_1DVigExecute(void)
{
    AMBA_CT_1D_VIG_CALIB_MODE_e CalibMode = CT_VigString2CalibMode(CTUserVigGroupData.System.CalibMode);
    UINT32 Rval = CT_OK;

    if (CalibMode == AMBA_CT_VIG_CALIB_MODE_NORMAL){
        Rval |= CT_1DVigCalibExecute();
    } else if (CalibMode == AMBA_CT_VIG_CALIB_MODE_EVALUATION) {
        Rval |= CT_1DVigEvaluationExecute();
    } else {
        Rval |= CT_ERR_1;
    }

    return Rval;
}

UINT32 AmbaCT_1DVigGetCalibData(UINT32 Index, AMBA_CT_1D_VIG_CALIB_DATA_s *pData)
{
    AmbaPrint_PrintStr5("Error, %s function is not support any more, please use the new function AmbaCT_1DVigGetCalibDataV2", __func__, NULL, NULL, NULL, NULL);
    AmbaMisra_TouchUnused(&Index);
    AmbaMisra_TouchUnused(pData);
    return CT_ERR_1;
}

UINT32 AmbaCT_1DVigGetCalibDataV1(UINT32 Index, AMBA_CT_1D_VIG_CALIB_DATA_V1_s *pData)
{
    UINT32 Rval = CT_OK;
    if (Index < 4U) {
        pData->pCalibData = &pCTVigWorkingBuf->CalibDataV1[Index];

    } else {
        Rval = CT_ERR_1;
    }
    return Rval;
}

UINT32 AmbaCT_1DVigGetCalibDataV2(UINT32 Index, AMBA_CT_1D_VIG_CALIB_DATA_V2_s *pData)
{
    UINT32 Rval = CT_OK;
    if (Index < 4U) {
        AMBA_CT_1D_VIG_CALIB_MODE_e CalibMode = CT_VigString2CalibMode(CTUserVigGroupData.System.CalibMode);
        if (CalibMode == AMBA_CT_VIG_CALIB_MODE_NORMAL) {
            pData->pCalibData = &pCTVigWorkingBuf->CalibDataV1[Index];
            pData->pEvaluationReport = NULL;
        } else if (CalibMode == AMBA_CT_VIG_CALIB_MODE_EVALUATION) {
            pData->pCalibData = NULL;
            pData->pEvaluationReport = &pCTVigWorkingBuf->EvaluationReport[Index][0U];
        } else {
            Rval = CT_ERR_1;
        }
    } else {
        Rval = CT_ERR_1;
    }
    return Rval;
}

const AMBA_CT_1D_VIG_USET_SETTING_s* AmbaCT_1DVigGetUserSetting(void)
{
    return (const AMBA_CT_1D_VIG_USET_SETTING_s*)&CTUserVigGroupData;
}
