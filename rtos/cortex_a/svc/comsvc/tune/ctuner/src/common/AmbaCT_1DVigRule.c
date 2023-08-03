/**
 *  @file AmbaCT_1DVigRule.c
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
#include "AmbaCT_Parser.h" // REG_s
#include "AmbaCT_ProcRule.h"
#include "AmbaCT_1DVigRule.h"
#include "AmbaCT_1DVigTuner.h"
#include "AmbaTypes.h"

#define SYSTEM_PARAMS_COUNT (5U)
static PARAM_s CT_SystemParams[SYSTEM_PARAMS_COUNT] = {
    {0, "calib_mode", STRING, 1, NULL, 0},
    {1, "chip_rev", STRING, 1, NULL, 0},
    {2, "output_prefix", STRING, 1, NULL, 0},
    {3, "chan_num", U_32, 1, NULL, 0},
    {4, "sync_max_diff_th", U_32, 1, NULL, 0},
};

#define RAW_INFO_PARAMS_COUNT (15U)
static PARAM_s CT_RawInfoParams[RAW_INFO_PARAMS_COUNT] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
    {4, "pitch", U_32, 1, NULL, 0},
    {5, "resolution", U_8, 1, NULL, 0},
    {6, "from_file", U_8, 1, NULL, 0},
    {7, "path", STRING, 1, NULL, 0},
    {8, "bayer", U_8, 1, NULL, 0},
    {9, "r_black", S_16, 1, NULL, 0},
    {10, "g_r_black", S_16, 1, NULL, 0},
    {11, "g_b_black", S_16, 1, NULL, 0},
    {12, "b_black", S_16, 1, NULL, 0},
    {13, "sensortype", U_32, 1, NULL, 0},
    {14, "irmode", U_32, 1, NULL, 0},
};

#define VIGNETTE_PARAMS_COUNT (19U)
static PARAM_s CT_VignetteParams[VIGNETTE_PARAMS_COUNT] = {
    {0, "vin_start_x", U_32, 1, NULL, 0},
    {1, "vin_start_y", U_32, 1, NULL, 0},
    {2, "vin_width", U_32, 1, NULL, 0},
    {3, "vin_height", U_32, 1, NULL, 0},
    {4, "vin_h_subsample_factor_num", U_8, 1, NULL, 0},
    {5, "vin_h_subsample_factor_den", U_8, 1, NULL, 0},
    {6, "vin_v_subsample_factor_num", U_8, 1, NULL, 0},
    {7, "vin_v_subsample_factor_den", U_8, 1, NULL, 0},
    {8, "raw_num", U_32, 1, NULL, 0},
    {9, "strength", U_32, 1, NULL, 0},
    {10, "chroma_ratio", U_32, 1, NULL, 0},
    {11, "strength_effect_mode", U_8, 1, NULL, 0},
    {12, "sync", U_8, 1, NULL, 0},
    {13, "max_center_x_offset", U_32, 1, NULL, 0},
    {14, "max_center_y_offset", U_32, 1, NULL, 0},
    {15, "center_search_algo", U_32, 1, NULL, 0},
    {16, "center_search_start_x", U_32, 1, NULL, 0},
    {17, "center_search_start_y", U_32, 1, NULL, 0},
    {18, "center_search_radius", U_32, 1, NULL, 0},
};

#define EVALUATION_PARAMS_COUNT (3U)
static PARAM_s CT_EvaluationParams[EVALUATION_PARAMS_COUNT] = {
    {0, "x", U_32, AMBA_CAL_VIG_EVA_MAX_SAMPLE_NUM , NULL, 0},
    {1, "y", U_32, AMBA_CAL_VIG_EVA_MAX_SAMPLE_NUM , NULL, 0},
    {2, "number", U_32, 1, NULL, 0},
};

#define EVALUATION_CALIB_PARAMS_COUNT (13U)
static PARAM_s CT_VigEvaluationCalibParams[EVALUATION_CALIB_PARAMS_COUNT] = {
    {0, "r_center_x", U_32, 1, NULL, 0},
    {1, "r_center_y", U_32, 1, NULL, 0},
    {2, "gr_center_x", U_32, 1, NULL, 0},
    {3, "gr_center_y", U_32, 1, NULL, 0},
    {4, "b_center_x", U_32, 1, NULL, 0},
    {5, "b_center_y", U_32, 1, NULL, 0},
    {6, "gb_center_x", U_32, 1, NULL, 0},
    {7, "gb_center_y", U_32, 1, NULL, 0},
    {8, "radial_coarse", U_32, 1, NULL, 0},
    {9, "radial_coarse_log", U_32, 1, NULL, 0},
    {10, "radial_fine", U_32, 1, NULL, 0},
    {11, "radial_fine_log", U_32, 1, NULL, 0},
    {12, "file_path", STRING, 1, NULL, 0},
};

static void CT_SystemProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_COUNT];
    AMBA_CT_1D_VIG_TUNER_SYSTEM_s Data;
    AmbaCT_1DVigGetSystemInfo(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.ChipRev;
    pField[2] = Data.OutputPrefix;
    pField[3] = &Data.ChanNum;
    pField[4] = &Data.SyncMaxDiffTh;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_1DVigSetSystemInfo(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_RawInfoProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    AMBA_CT_1D_VIG_TUNER_RAW_INFO_s Data;
    UINT32 ArrayIndex = (UINT32)Index / RAW_INFO_PARAMS_COUNT;
    void *pField[RAW_INFO_PARAMS_COUNT];
    AmbaCT_1DVigGetRawInfo(ArrayIndex, &Data);
    pField[0] = &Data.Area.StartX;
    pField[1] = &Data.Area.StartY;
    pField[2] = &Data.Area.Width;
    pField[3] = &Data.Area.Height;
    pField[4] = &Data.Pitch;
    pField[5] = &Data.Resolution;
    pField[6] = &Data.FromFile;
    pField[7] = Data.Path;
    pField[8] = &Data.Bayer;
    pField[9] = &Data.BlackLevel.BlackR;
    pField[10] = &Data.BlackLevel.BlackGr;
    pField[11] = &Data.BlackLevel.BlackGb;
    pField[12] = &Data.BlackLevel.BlackB;
    pField[13] = &Data.SensorType;
    pField[14] = &Data.IrMode;
    CT_ProcParam(pParam, pField[(UINT32)Index % RAW_INFO_PARAMS_COUNT]);
    AmbaCT_1DVigSetRawInfo(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_VignetteProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    AMBA_CT_1D_VIG_TUNER_VIGNETTE_s Data;
    UINT32 ArrayIndex = (UINT32)Index / VIGNETTE_PARAMS_COUNT;
    void *pField[VIGNETTE_PARAMS_COUNT];
    AmbaCT_1DVigGetVignette(ArrayIndex, &Data);
    pField[0] = &Data.VinSensorGeo.StartX;
    pField[1] = &Data.VinSensorGeo.StartY;
    pField[2] = &Data.VinSensorGeo.Width;
    pField[3] = &Data.VinSensorGeo.Height;
    pField[4] = &Data.VinSensorGeo.HSubSample.FactorNum;
    pField[5] = &Data.VinSensorGeo.HSubSample.FactorDen;
    pField[6] = &Data.VinSensorGeo.VSubSample.FactorNum;
    pField[7] = &Data.VinSensorGeo.VSubSample.FactorDen;
    pField[8] = &Data.RawNum;
    pField[9] = &Data.Strength;
    pField[10] = &Data.ChromaRatio;
    pField[11] = &Data.StrengthEffectMode;
    pField[12] = &Data.Sync;
    pField[13] = &Data.MaxCenterXOffset;
    pField[14] = &Data.MaxCenterYOffset;
    pField[15] = &Data.CenterSearchAlgo;
    pField[16] = &Data.CenterSearchStartX;
    pField[17] = &Data.CenterSearchStartY;
    pField[18] = &Data.CenterSearchRadius;
    CT_ProcParam(pParam, pField[(UINT32)Index % VIGNETTE_PARAMS_COUNT]);
    AmbaCT_1DVigSetVignette(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        *pParamStatus |= (0x1ULL << 13);
        *pParamStatus |= (0x1ULL << 14);
        *pParamStatus |= (0x1ULL << 15);
        *pParamStatus |= (0x1ULL << 16);
        *pParamStatus |= (0x1ULL << 17);
        *pParamStatus |= (0x1ULL << 18);
    }
}

static void CT_EvaluationProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    AMBA_CT_1D_VIG_EVALUATION_LOC_s Data;
    UINT32 ArrayIndex = (UINT32)Index / EVALUATION_PARAMS_COUNT;
    void *pField[EVALUATION_PARAMS_COUNT];
    AmbaCT_1DVigGetEvaluationLoc(ArrayIndex, &Data);
    pField[0] = &Data.X[0];
    pField[1] = &Data.Y[0];
    pField[2] = &Data.Number;
    CT_ProcParam(pParam, pField[(UINT32)Index % EVALUATION_PARAMS_COUNT]);
    AmbaCT_1DVigSetEvaluationLoc(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_VigEvaluationCalibProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    AMBA_CT_1D_VIG_EVA_CALIB_DATA_s Data;
    UINT32 ArrayIndex = (UINT32) Index / EVALUATION_CALIB_PARAMS_COUNT;
    void *pField[EVALUATION_CALIB_PARAMS_COUNT];
    AmbaCT_1DVigGetEvaluationCalibData(ArrayIndex, &Data);
    pField[0] = &Data.ModelCenterX_R;
    pField[1] = &Data.ModelCenterY_R;
    pField[2] = &Data.ModelCenterX_Gr;
    pField[3] = &Data.ModelCenterY_Gr;
    pField[4] = &Data.ModelCenterX_B;
    pField[5] = &Data.ModelCenterY_B;
    pField[6] = &Data.ModelCenterX_Gb;
    pField[7] = &Data.ModelCenterY_Gb;
    pField[8] = &Data.RadialCoarse;
    pField[9] = &Data.RadialCoarseLog;
    pField[10] = &Data.RadialBinsFine;
    pField[11] = &Data.RadialBinsFineLog;
    pField[12] = Data.Path;
    CT_ProcParam(pParam, pField[(UINT32)Index % EVALUATION_CALIB_PARAMS_COUNT]);
    AmbaCT_1DVigSetEvaluationCalibData(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

#define VIGNETTE_REGS_COUNT (5U)
static REG_s VignetteRegs[VIGNETTE_REGS_COUNT] = {
    {(INT32)AMBA_CT_1D_VIG_TUNER_SYSTEM_INFO, "system", 1, 0, (INT32) SYSTEM_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_SystemParams, CT_SystemProc, 0},
    {(INT32)AMBA_CT_1D_VIG_TUNER_RAW_INFO, "raw", 4, 0, (INT32) RAW_INFO_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_RawInfoParams, CT_RawInfoProc, 0},
    {(INT32)AMBA_CT_1D_VIG_TUNER_VIGNETTE, "vignette", 4, 0, (INT32) VIGNETTE_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_VignetteParams, CT_VignetteProc, 0},
    {(INT32)AMBA_CT_1D_VIG_TUNER_EVA_LOC, "evaluation_location", 4, 0, (INT32) EVALUATION_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_EvaluationParams, CT_EvaluationProc, 0},
    {(INT32)AMBA_CT_1D_VIG_TUNER_CALIB_INFO, "calib_info", 4, 0, (INT32) EVALUATION_CALIB_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_VigEvaluationCalibParams, CT_VigEvaluationCalibProc, 0},
};

UINT32 AmbaCT_Get1DVignetteRuleInfo(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &VignetteRegs[0];
    RuleInfo->RegCount = VIGNETTE_REGS_COUNT;
    return 0U;
}

UINT32 AmbaCT_Get1DVignetteRuleInfoMisra(Rule_Info_t *RuleInfo);

UINT32 AmbaCT_Get1DVignetteRuleInfoMisra(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &VignetteRegs[0];
    RuleInfo->RegCount = VIGNETTE_REGS_COUNT;

    return 0U;
}

