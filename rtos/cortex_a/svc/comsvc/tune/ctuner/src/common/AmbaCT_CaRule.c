/**
 *  @file AmbaCT_CaRule.c
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
#include "AmbaCalib_Camera.h"
#include "AmbaCT_CaTuner.h"
#include "AmbaCT_CaRule.h"

#define SYSTEM_PARAMS_COUNT (3)
static PARAM_s CT_CaSystemParams[SYSTEM_PARAMS_COUNT] = {
    {0, "calib_mode", STRING, 1, NULL, 0},
    {1, "chip_rev", STRING, 1, NULL, 0},
    {2, "output_prefix", STRING, 1, NULL, 0},
};

#define LENS_PARAMS_COUNT   (4)
static PARAM_s CT_CaLensParams[LENS_PARAMS_COUNT] = {
    {0, "expect_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {1, "ca_line_r_table",S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {2, "ca_line_b_table",S_DB,MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {3, "table_length", U_32, 1, NULL, 0},
};

#define SENSOR_PARAMS_COUNT (5)
static PARAM_s CT_CaSensorParams[SENSOR_PARAMS_COUNT] = {
    {0, "cell_size", S_DB, 1, NULL, 0},
    {1, "start_x", U_32, 1, NULL, 0},
    {2, "start_y", U_32, 1, NULL, 0},
    {3, "width", U_32, 1, NULL, 0},
    {4, "height", U_32, 1, NULL, 0},
};

#define OPTICAL_CENTER_PARAMS_COUNT (2)
static PARAM_s CT_CaOpticalCenterParams[] = {
    {0, "x", S_DB, 1, NULL, 0},
    {1, "y", S_DB, 1, NULL, 0},
};

#define ADJUST_PARAMS_COUNT (4)
static PARAM_s CT_CaAdjustParams[ADJUST_PARAMS_COUNT] = {
    {0, "method", U_32, 1, NULL, 0},
    {1, "compensate", U_32, 1, NULL, 0},
    {2, "ratio", U_32, 1, NULL, 0},
    {3, "zoom_step", U_32, 1, NULL, 0},
};

#define VIN_PARAMS_COUNT (8)
static PARAM_s CT_CaVinParams[VIN_PARAMS_COUNT] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
    {4, "h_subsample_factor_num", U_8, 1, NULL, 0},
    {5, "h_subsample_factor_den", U_8, 1, NULL, 0},
    {6, "v_subsample_factor_num", U_8, 1, NULL, 0},
    {7, "v_subsample_factor_den", U_8, 1, NULL, 0},
};

#define TILE_SIZE_PARAMS_COUNT (2)
static PARAM_s CT_CaTileSizeParams[TILE_SIZE_PARAMS_COUNT] = {
    {0, "width", U_32, 1, NULL, 0},
    {1, "height", U_32, 1, NULL, 0},
};

static void CT_CaSystemProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_COUNT];
    AMBA_CT_CA_SYSTEM_s Data;
    AmbaCT_CaGetSystem(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.ChipRev;
    pField[2] = Data.OutputPrefix;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_CaSetSystem(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_CaLensProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[LENS_PARAMS_COUNT];
    AMBA_CT_CA_LENS_s Data;
    AmbaCT_CaGetLens(&Data);
    pField[0] = Data.pExpecttable;
    pField[1] = Data.pCaRedTable;
    pField[2] = Data.pCaBlueTable;
    pField[3] = &Data.TableLen;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_CaSetLens(&Data);
    if (pParamStatus != NULL) {
       *pParamStatus |= (0x1ULL << (UINT32)Index);
    }

}


static void CT_CaSensorProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SENSOR_PARAMS_COUNT];
    AMBA_CAL_SENSOR_s Data;
    AmbaCT_CaGetSensor(&Data);
    pField[0] = &Data.CellSize;
    pField[1] = &Data.StartX;
    pField[2] = &Data.StartY;
    pField[3] = &Data.Width;
    pField[4] = &Data.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_CaSetSensor(&Data);
    if (pParamStatus != NULL) {
       *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}


static void CT_CaOpticalCenterProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OPTICAL_CENTER_PARAMS_COUNT];
    AMBA_CAL_POINT_DB_2D_s Data;
    AmbaCT_CaGetOpticalCenter(&Data);
    pField[0] = &Data.X;
    pField[1] = &Data.Y;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_CaSetOpticalCenter(&Data);
    if (pParamStatus != NULL) {
       *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_CaAdjustProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[ADJUST_PARAMS_COUNT];
    AMBA_CAL_CA_ADJUST_CFG_s Data;
    AmbaCT_CaGetAdjust(&Data);
    pField[0] = &Data.Method;
    pField[1] = &Data.Compensate;
    pField[2] = &Data.Ratio;
    pField[3] = &Data.Zoom_step;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_CaSetAdjust(&Data);
    if (pParamStatus != NULL) {
       *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_CaVinProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIN_PARAMS_COUNT];
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s Data;
    AmbaCT_CaGetVin(&Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    pField[4] = &Data.HSubSample.FactorNum;
    pField[5] = &Data.HSubSample.FactorDen;
    pField[6] = &Data.VSubSample.FactorNum;
    pField[7] = &Data.VSubSample.FactorDen;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_CaSetVin(&Data);
    if (pParamStatus != NULL) {
       *pParamStatus |= (0x1ULL << (UINT32)Index);
    }

}

static void CT_CaTileSizeProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[TILE_SIZE_PARAMS_COUNT];
    AMBA_CAL_SIZE_s Data;
    AmbaCT_CaGetTileSize(&Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_CaSetTileSize(&Data);
    if (pParamStatus != NULL) {
       *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

#define CA_REGS_COUNT   (7)
static REG_s CaRegs[CA_REGS_COUNT] = {
    {(INT32)AMBA_CT_CA_SYSTEM_INFO, "system", 1, 0, SYSTEM_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_CaSystemParams, CT_CaSystemProc, 0},
    {(INT32)AMBA_CT_CA_LENS, "lens", 1, 0, LENS_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_CaLensParams, CT_CaLensProc, 0},
    {(INT32)AMBA_CT_CA_SENSOR, "sensor", 1, 0, SENSOR_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_CaSensorParams, CT_CaSensorProc, 0},
    {(INT32)AMBA_CT_CA_OPTICAL_CENTER, "optical_center", 1, 0, OPTICAL_CENTER_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_CaOpticalCenterParams, CT_CaOpticalCenterProc, 0},
    {(INT32)AMBA_CT_CA_ADJUST, "adjust", 1, 0, ADJUST_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_CaAdjustParams, CT_CaAdjustProc, 0},
    {(INT32)AMBA_CT_CA_VIN, "vin", 1, 0, VIN_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_CaVinParams, CT_CaVinProc, 0},
    {(INT32)AMBA_CT_CA_TILE_SIZE, "tile_size", 1, 0, TILE_SIZE_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_CaTileSizeParams, CT_CaTileSizeProc, 0},
};

UINT32 AmbaCT_GetCaRuleInfo(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &CaRegs[0];
    RuleInfo->RegCount = CA_REGS_COUNT;
    return 0U;
}

UINT32 AmbaCT_GetCaRuleInfoMisra(Rule_Info_t *RuleInfo);
UINT32 AmbaCT_GetCaRuleInfoMisra(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &CaRegs[0];
    RuleInfo->RegCount = CA_REGS_COUNT;
    return 0U;
}

