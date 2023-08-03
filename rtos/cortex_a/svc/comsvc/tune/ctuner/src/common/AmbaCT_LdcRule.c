/**
 *  @file AmbaCT_LdcRule.c
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
#include "AmbaCT_LdcTuner.h"
#include "AmbaCT_LdcRule.h"
#include "AmbaCT_Logging.h"
#define SYSTEM_PARAMS_COUNT (3U)
static PARAM_s CT_LdcSystemParams[SYSTEM_PARAMS_COUNT] = {
    {0, "calib_mode", STRING, 1, NULL, 0},
    {1, "chip_rev", STRING, 1, NULL, 0},
    {2, "output_prefix", STRING, 1, NULL, 0},
};

#define LENS_PARAMS_COUNT (8U)
static PARAM_s CT_LdcLensParams[LENS_PARAMS_COUNT] = {
    {0, "disto_type", U_8, 1, NULL, 0},
    {1, "real_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {2, "expect_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {3, "angle_real_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {4, "expect_real_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {5, "table_length", U_32, 1, NULL, 0},
    {6, "real_expect_formula", S_DB, 5, NULL, 0},
    {7, "angle_formula", S_DB, 5, NULL, 0},
};

#define SENSOR_PARAMS_COUNT (5U)
static PARAM_s CT_LdcSensorParams[SENSOR_PARAMS_COUNT] = {
    {0, "cell_size", S_DB, 1, NULL, 0},
    {1, "start_x", U_32, 1, NULL, 0},
    {2, "start_y", U_32, 1, NULL, 0},
    {3, "width", U_32, 1, NULL, 0},
    {4, "height", U_32, 1, NULL, 0},
};

#define OPTICAL_CENTER_PARAMS_COUNT (2U)
static PARAM_s CT_LdcOpticalCenterParams[OPTICAL_CENTER_PARAMS_COUNT] = {
    {0, "x", S_DB, 1, NULL, 0},
    {1, "y", S_DB, 1, NULL, 0},
};

#define CAMERA_PARAMS_COUNT (4U)
static PARAM_s CT_LdcCameraParams[CAMERA_PARAMS_COUNT] = {
    {0, "pos_x", S_DB, 1, NULL, 0},
    {1, "pos_y", S_DB, 1, NULL, 0},
    {2, "pos_z", S_DB, 1, NULL, 0},
    {3, "rotate_type", U_8, 1, NULL, 0},
};

#define VIN_PARAMS_COUNT (8U)
static PARAM_s CT_LdcVinParams[VIN_PARAMS_COUNT] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
    {4, "h_subsample_factor_num", U_8, 1, NULL, 0},
    {5, "h_subsample_factor_den", U_8, 1, NULL, 0},
    {6, "v_subsample_factor_num", U_8, 1, NULL, 0},
    {7, "v_subsample_factor_den", U_8, 1, NULL, 0},
};

#define TILE_SIZE_PARAMS_COUNT (2U)
static PARAM_s CT_LdcTileSizeParams[TILE_SIZE_PARAMS_COUNT] = {
    {0, "width", U_32, 1, NULL, 0},
    {1, "height", U_32, 1, NULL, 0},
};

#define VIEW_PARAMS_COUNT (6U)
static PARAM_s CT_LdcViewParams[VIEW_PARAMS_COUNT] = {
    {0, "line_straight_str_x", U_8, 1, NULL, 0},
    {1, "line_straight_str_y", U_8, 1, NULL, 0},
    {2, "fov_wider_str_x", U_8, 1, NULL, 0},
    {3, "fov_wider_str_y", U_8, 1, NULL, 0},
    {4, "dist_even_str_x", U_8, 1, NULL, 0},
    {5, "dist_even_str_y", U_8, 1, NULL, 0},
};

static void CT_LdcSystemProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_COUNT];
    AMBA_CT_LDC_SYSTEM_s Data;
    AmbaCT_LdcGetSystem(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.ChipRev;
    pField[2] = Data.OutputPrefix;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdcSetSystem(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_LdcLensProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    void *pField[LENS_PARAMS_COUNT];
    AMBA_CT_LDC_LENS_s Data;
    AmbaCT_LdcGetLens(&Data);
    pField[0] = &Data.LensDistoType;
    pField[1] = Data.pRealTable;
    pField[2] = Data.pExceptTable;
    pField[3] = Data.pRealAngleTable;
    pField[4] = Data.pExceptAngleTable;
    pField[5] = &Data.TableLen;
    pField[6] = Data.RealExpectFormula;
    pField[7] = Data.AngleFormula;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdcSetLens(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
        if (((UINT32) Index % (UINT32)LENS_PARAMS_COUNT) == 0U) {
            if (Data.LensDistoType == AMBA_CAL_LD_REAL_EXPECT_TBL) {
                *pParamStatus |= 0x1ULL << 3U;
                *pParamStatus |= 0x1ULL << 4U;
                *pParamStatus |= 0x1ULL << 6U;
                *pParamStatus |= 0x1ULL << 7U;
            } else if (Data.LensDistoType == AMBA_CAL_LD_ANGLE_TBL) {
                *pParamStatus |= 0x1ULL << 1U;
                *pParamStatus |= 0x1ULL << 2U;
                *pParamStatus |= 0x1ULL << 6U;
                *pParamStatus |= 0x1ULL << 7U;
            } else if (Data.LensDistoType == AMBA_CAL_LD_REAL_EXPECT_FORMULA) {
                *pParamStatus |= 0x1ULL << 1U;
                *pParamStatus |= 0x1ULL << 2U;
                *pParamStatus |= 0x1ULL << 3U;
                *pParamStatus |= 0x1ULL << 4U;
                *pParamStatus |= 0x1ULL << 5U;
                *pParamStatus |= 0x1ULL << 7U;
            } else if (Data.LensDistoType == AMBA_CAL_LD_ANGLE_FORMULA) {
                *pParamStatus |= 0x1ULL << 1U;
                *pParamStatus |= 0x1ULL << 2U;
                *pParamStatus |= 0x1ULL << 3U;
                *pParamStatus |= 0x1ULL << 4U;
                *pParamStatus |= 0x1ULL << 5U;
                *pParamStatus |= 0x1ULL << 7U;
            } else {
                pNextMsg = CT_LogPutStr(Msg, "Not support lens distortion type.");
                (void) CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            }
        }
    }
}

static void CT_LdcSensorProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SENSOR_PARAMS_COUNT];
    AMBA_CAL_SENSOR_s Data;
    AmbaCT_LdcGetSensor(&Data);
    pField[0] = &Data.CellSize;
    pField[1] = &Data.StartX;
    pField[2] = &Data.StartY;
    pField[3] = &Data.Width;
    pField[4] = &Data.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdcSetSensor(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdcOpticalCenterProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OPTICAL_CENTER_PARAMS_COUNT];
    AMBA_CAL_POINT_DB_2D_s Data;
    AmbaCT_LdcGetOpticalCenter(&Data);
    pField[0] = &Data.X;
    pField[1] = &Data.Y;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdcSetOpticalCenter(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdcCameraProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[CAMERA_PARAMS_COUNT];
    AMBA_CT_LDC_CAMERA_s Data;
    AmbaCT_LdcGetCamera(&Data);
    pField[0] = &Data.PositionX;
    pField[1] = &Data.PositionY;
    pField[2] = &Data.PositionZ;
    pField[3] = &Data.RotateType;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdcSetCamera(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdcVinProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIN_PARAMS_COUNT];
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s Data;
    AmbaCT_LdcGetVin(&Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    pField[4] = &Data.HSubSample.FactorNum;
    pField[5] = &Data.HSubSample.FactorDen;
    pField[6] = &Data.VSubSample.FactorNum;
    pField[7] = &Data.VSubSample.FactorDen;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdcSetVin(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdcTileSizeProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[TILE_SIZE_PARAMS_COUNT];
    AMBA_CAL_SIZE_s Data;
    AmbaCT_LdcGetTileSize(&Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdcSetTileSize(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdcViewProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIEW_PARAMS_COUNT];
    AMBA_CAL_LDC_VIEW_CFG_s Data;
    AmbaCT_LdcGetView(&Data);
    pField[0] = &Data.LineStraightStrX;
    pField[1] = &Data.LineStraightStrY;
    pField[2] = &Data.FovWiderStrX;
    pField[3] = &Data.FovWiderStrY;
    pField[4] = &Data.DistEvenStrX;
    pField[5] = &Data.DistEvenStrY;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdcSetView(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

#define LDC_REGS_COUNT (8U)
static REG_s LdcRegs[LDC_REGS_COUNT] = {
    {(INT32)AMBA_CT_LDC_SYSTEM_INFO, "system", 1, 0, (INT32) SYSTEM_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdcSystemParams, CT_LdcSystemProc, 0},
    {(INT32)AMBA_CT_LDC_LENS, "lens", 1, 0, (INT32) LENS_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdcLensParams, CT_LdcLensProc, 0},
    {(INT32)AMBA_CT_LDC_SENSOR, "sensor", 1, 0, (INT32) SENSOR_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdcSensorParams, CT_LdcSensorProc, 0},
    {(INT32)AMBA_CT_LDC_OPTICAL_CENTER, "optical_center", 1, 0, (INT32) OPTICAL_CENTER_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdcOpticalCenterParams, CT_LdcOpticalCenterProc, 0},
    {(INT32)AMBA_CT_LDC_CAMERA, "camera", 1, 0, (INT32) CAMERA_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdcCameraParams, CT_LdcCameraProc, 0},
    {(INT32)AMBA_CT_LDC_VIN, "vin", 1, 0, (INT32) VIN_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdcVinParams, CT_LdcVinProc, 0},
    {(INT32)AMBA_CT_LDC_TILE_SIZE, "tile_size", 1, 0, (INT32) TILE_SIZE_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdcTileSizeParams, CT_LdcTileSizeProc, 0},
    {(INT32)AMBA_CT_LDC_VIEW, "view", 1, 0, (INT32) VIEW_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdcViewParams, CT_LdcViewProc, 0},
};

UINT32 AmbaCT_GetLdcRuleInfo(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &LdcRegs[0];
    RuleInfo->RegCount = LDC_REGS_COUNT;
    return CT_OK;
}

UINT32 AmbaCT_GetLdcRuleInfoMisra(Rule_Info_t *RuleInfo);
UINT32 AmbaCT_GetLdcRuleInfoMisra(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &LdcRegs[0];
    RuleInfo->RegCount = LDC_REGS_COUNT;
    return CT_OK;
}

