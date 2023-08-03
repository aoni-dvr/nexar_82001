/**
 *  @file AmbaCT_OcRule.c
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
#include "AmbaCT_Parser.h"
#include "AmbaCT_ProcRule.h"
#include "AmbaCT_OcRule.h"
#include "AmbaCT_OcTuner.h"
#include "AmbaTypes.h"
#include "AmbaCT_Logging.h"
#define SYSTEM_PARAMS_COUNT (3U)
static PARAM_s CT_OcSystemParams[SYSTEM_PARAMS_COUNT] = {
    {0, "calib_mode", STRING, 1, NULL, 0},
    {1, "chip_rev", STRING, 1, NULL, 0},
    {2, "output_prefix", STRING, 1, NULL, 0},
};

#define RAW_INFO_PARAMS_COUNT (9U)
static PARAM_s CT_OcRawInfoParams[RAW_INFO_PARAMS_COUNT] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
    {4, "pitch", U_32, 1, NULL, 0},
    {5, "resolution", U_8, 1, NULL, 0},
    {6, "from_file", U_8, 1, NULL, 0},
    {7, "path", STRING, 1, NULL, 0},
    {8, "bayer", U_8, 1, NULL, 0},
};

#define VIN_PARAMS_COUNT (8U)
static PARAM_s CT_OcVinParams[VIN_PARAMS_COUNT] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
    {4, "h_subsample_factor_num", U_8, 1, NULL, 0},
    {5, "h_subsample_factor_den", U_8, 1, NULL, 0},
    {6, "v_subsample_factor_num", U_8, 1, NULL, 0},
    {7, "v_subsample_factor_den", U_8, 1, NULL, 0},
};

#define ELLIPSE_INFO_PARAMS_COUNT (4U)
static PARAM_s CT_OcEllipseInfoParams[ELLIPSE_INFO_PARAMS_COUNT] = {
    {0, "min_radius", U_32, 1, NULL, 0},
    {1, "center_limit_width", U_32, 1, NULL, 0},
    {2, "center_limit_height", U_32, 1, NULL, 0},
    {3, "split_circle", U_8, 1, NULL, 0},
};

static void CT_OcSystemProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_COUNT];
    AMBA_CT_OC_TUNER_SYSTEM_s Data;
    AmbaCT_OcGetSystemInfo(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.ChipRev;
    pField[2] = Data.OutputPrefix;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcSetSystemInfo(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OcRawInfoProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    AMBA_CT_OC_TUNER_RAW_INFO_s Data;
    void *pField[RAW_INFO_PARAMS_COUNT];
    AmbaCT_OcGetRawInfo(&Data);
    pField[0] = &Data.Area.StartX;
    pField[1] = &Data.Area.StartY;
    pField[2] = &Data.Area.Width;
    pField[3] = &Data.Area.Height;
    pField[4] = &Data.Pitch;
    pField[5] = &Data.Resolution;
    pField[6] = &Data.FromFile;
    pField[7] = Data.Path;
    pField[8] = &Data.Bayer;
    CT_ProcParam(pParam, pField[(UINT32)Index % (UINT32)RAW_INFO_PARAMS_COUNT]);
    AmbaCT_OcSetRawInfo(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OcVinProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    AMBA_CT_OC_TUNER_VIN_s Data;
    void *pField[VIN_PARAMS_COUNT];
    AmbaCT_OcGetVin(&Data);

    pField[0] = &Data.VinSensorGeo.StartX;
    pField[1] = &Data.VinSensorGeo.StartY;
    pField[2] = &Data.VinSensorGeo.Width;
    pField[3] = &Data.VinSensorGeo.Height;
    pField[4] = &Data.VinSensorGeo.HSubSample.FactorNum;
    pField[5] = &Data.VinSensorGeo.HSubSample.FactorDen;
    pField[6] = &Data.VinSensorGeo.VSubSample.FactorNum;
    pField[7] = &Data.VinSensorGeo.VSubSample.FactorDen;
    CT_ProcParam(pParam, pField[(UINT32)Index % (UINT32)VIN_PARAMS_COUNT]);
    AmbaCT_OcSetVin(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OcEllipseInfoProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    AMBA_CT_OC_TUNER_ELLIPSE_INFO_s Data;
    void *pField[ELLIPSE_INFO_PARAMS_COUNT];
    AmbaCT_OcGetEllipseInfo(&Data);

    pField[0] = &Data.MinRadius;
    pField[1] = &Data.CenterLimit.Width;
    pField[2] = &Data.CenterLimit.Height;
    pField[3] = &Data.SplitCircle;
    CT_ProcParam(pParam, pField[(UINT32)Index % (UINT32)ELLIPSE_INFO_PARAMS_COUNT]);
    AmbaCT_OcSetEllipseInfo(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

#define OPTICAL_CENTER_REGS_COUNT (4)
static REG_s OpticalCenterRegs[OPTICAL_CENTER_REGS_COUNT] = {
    {(INT32)AMBA_CT_OC_TUNER_SYSTEM_INFO, "system", 1, 0, (INT32) SYSTEM_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OcSystemParams, CT_OcSystemProc, 0},
    {(INT32)AMBA_CT_OC_TUNER_RAW_INFO, "raw", 1, 0, (INT32) RAW_INFO_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OcRawInfoParams, CT_OcRawInfoProc, 0},
    {(INT32)AMBA_CT_OC_TUNER_VIN, "vin", 1, 0, (INT32) VIN_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OcVinParams, CT_OcVinProc, 0},
    {(INT32)AMBA_CT_OC_TUNER_ELLIPSE_INFO, "ellipse", 1, 0, (INT32) ELLIPSE_INFO_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OcEllipseInfoParams, CT_OcEllipseInfoProc, 0},
};

UINT32 AmbaCT_GetOpticalCenterRuleInfo(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &OpticalCenterRegs[0];
    RuleInfo->RegCount = OPTICAL_CENTER_REGS_COUNT;
    return CT_OK;
}

UINT32 AmbaCT_GetOCRuleInfoMisra(Rule_Info_t *RuleInfo);
UINT32 AmbaCT_GetOCRuleInfoMisra(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &OpticalCenterRegs[0];
    RuleInfo->RegCount = OPTICAL_CENTER_REGS_COUNT;
    return CT_OK;
}

