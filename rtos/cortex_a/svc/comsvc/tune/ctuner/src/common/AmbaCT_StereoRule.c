/**
 *  @file AmbaCT_StereoRule.c
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
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaCT_StereoTuner.h"
#include "AmbaCT_StereoRule.h"

#define SYSTEM_PARAMS_NUM (4U)
static PARAM_s CT_StereoSystemParams[SYSTEM_PARAMS_NUM] = {
    {0, "calib_mode", STRING, 1U, NULL, 0},
    {1, "chip_rev", STRING, 1U, NULL, 0U},
    {2, "output_prefix", STRING, 1U, NULL, 0U},
    {3, "camera_num", U_8, 1U, NULL, 0U},
};

#define WARP_OPTIMIZER_NUM (1U)
static PARAM_s CT_StereoWarpOptimizerParams[WARP_OPTIMIZER_NUM] = {
    {0, "enable", U_32, 1, NULL, 0},
};

#define OUTPUT_CFG_PARAMS_NUM (1U)
static PARAM_s CT_StereoOutputCfgParams[OUTPUT_CFG_PARAMS_NUM] = {
    {0, "postfix", STRING, 1U, NULL, 0},
};

#define VIRTUAL_CAM_PARAMS_NUM (6U)
static PARAM_s CT_StereoVirtulaCamParams[VIRTUAL_CAM_PARAMS_NUM] = {
    {0, "focus_x", S_DB, 1, NULL, 0},
    {1, "focus_y", S_DB, 1, NULL, 0},
    {2, "focus_z", S_DB, 1, NULL, 0},
    {3, "project_plane_center_x", S_DB, 1, NULL, 0},
    {4, "project_plane_center_y", S_DB, 1, NULL, 0},
    {5, "project_plane_center_z", S_DB, 1, NULL, 0},
};

#define VOUT_PARAMS_NUM (4U)
static PARAM_s CT_StereoVoutParams[VOUT_PARAMS_NUM] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
};

#define LENS_PARAMS_NUM (8U)
static PARAM_s CT_StereoLensParams[LENS_PARAMS_NUM] = {
    {0, "disto_type", U_8, 1, NULL, 0},
    {1, "real_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {2, "expect_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {3, "angle_real_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {4, "expect_real_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {5, "table_length", U_32, 1, NULL, 0},
    {6, "real_expect_formula", S_DB, 5, NULL, 0},
    {7, "angle_formula", S_DB, 5, NULL, 0},
};

#define SENSOR_PARAMS_NUM (5U)
static PARAM_s CT_StereoSensorParams[SENSOR_PARAMS_NUM] = {
    {0, "cell_size", S_DB, 1, NULL, 0},
    {1, "start_x", U_32, 1, NULL, 0},
    {2, "start_y", U_32, 1, NULL, 0},
    {3, "width", U_32, 1, NULL, 0},
    {4, "height", U_32, 1, NULL, 0},
};

#define OPTICAL_CENTER_PARAMS_NUM (2U)
static PARAM_s CT_StereoOpticalCenterParams[OPTICAL_CENTER_PARAMS_NUM] = {
    {0, "x", S_DB, 1, NULL, 0},
    {1, "y", S_DB, 1, NULL, 0},
};

#define CAMERA_PARAMS_NUM (4U)
static PARAM_s CT_StereoCameraParams[CAMERA_PARAMS_NUM] = {
    {0, "pos_x", S_DB, 1, NULL, 0},
    {1, "pos_y", S_DB, 1, NULL, 0},
    {2, "pos_z", S_DB, 1, NULL, 0},
    {3, "rotate_type", U_8, 1, NULL, 0},
};
#define VIN_PARAMS_NUM (8U)
static PARAM_s CT_StereoVinParams[VIN_PARAMS_NUM] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
    {4, "h_subsample_factor_num", U_8, 1, NULL, 0},
    {5, "h_subsample_factor_den", U_8, 1, NULL, 0},
    {6, "v_subsample_factor_num", U_8, 1, NULL, 0},
    {7, "v_subsample_factor_den", U_8, 1, NULL, 0},
};

#define ROI_PARAMS_NUM (4U)
static PARAM_s CT_StereoRoiParams[ROI_PARAMS_NUM] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
};

#define TILE_SIZE_PARAMS_NUM (2U)
static PARAM_s CT_StereoTileSizeParams[TILE_SIZE_PARAMS_NUM] = {
    {0, "width", U_32, 1, NULL, 0},
    {1, "height", U_32, 1, NULL, 0},
};

#define CALIB_POINTS_PARAMS_NUM (6U)
static PARAM_s CT_StereoCalibPointsParams[CALIB_POINTS_PARAMS_NUM] = {
    {0, "world_pos_x", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
    {1, "world_pos_y", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
    {2, "world_pos_z", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
    {3, "raw_pos_x", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
    {4, "raw_pos_y", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
    {5, "corner_map_path", STRING, 1U, NULL, 0U},
};

#define ASSISTANCE_POINTS_PARAMS_NUM (6U)
static PARAM_s CT_StereoAssistancePointsParams[] = {
    {0, "number", U_32, 1, NULL, 0},
    {1, "world_pos_x", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
    {2, "world_pos_y", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
    {3, "world_pos_z", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
    {4, "raw_pos_x", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
    {5, "raw_pos_y", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
};

#define OPTIMIZE_PARAMS_NUM (1U)
static PARAM_s CT_StereoOptimizeParams[OPTIMIZE_PARAMS_NUM] = {
    {0, "level", U_8, 1, NULL, 0},
};

#define INTERNAL_PARAMS_NUM (4U)
static PARAM_s CT_StereoInternalParams[INTERNAL_PARAMS_NUM] = {
    {0, "debug_id", U_32, 1, NULL, 0},
    {1, "dma_th", U_32, 1, NULL, 0},
    {2, "out_of_roi_refine_enable", U_32, 1, NULL, 0},
    {3, "auto_roi_refine_enable", U_32, 1, NULL, 0},
};

static void CT_StereoOutputCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OUTPUT_CFG_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / OUTPUT_CFG_PARAMS_NUM;
    AMBA_CT_STEREO_OUTPUT_CFG_s Data;
    AmbaCT_StereoGetOutputCfg(ArrayIndex, &Data);
    pField[0] = Data.OutputPostfix;
    CT_ProcParam(pParam, pField[(UINT32) Index % OUTPUT_CFG_PARAMS_NUM]);
    AmbaCT_StereoSetOutputCfg(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_StereoSystemProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_NUM];
    AMBA_CT_STEREO_SYSTEM_s Data;
    AmbaCT_StereoGetSystem(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.ChipRev;
    pField[2] = Data.OutputPrefix;
    pField[3] = &Data.CameraNum;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_StereoSetSystem(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_StereoWarpOptimizerProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[WARP_OPTIMIZER_NUM];
    AMBA_CT_STEREO_WARP_OPTIMIZER_s Data;
    AmbaCT_StereoGetWarpOptimizer(&Data);
                pField[0] = &Data.Enable;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_StereoSetWarpOptimizer(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_StereoVirtualCamProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIRTUAL_CAM_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / VIRTUAL_CAM_PARAMS_NUM;
    AMBA_CAL_EM_CURVED_SURFACE_CFG_s Data;
    AmbaCT_StereoGetVirtulaCam(ArrayIndex, &Data);
    pField[0] = &Data.VirtualCam.Focus.X;
    pField[1] = &Data.VirtualCam.Focus.Y;
    pField[2] = &Data.VirtualCam.Focus.Z;
    pField[3] = &Data.VirtualCam.ProjectPlane.X;
    pField[4] = &Data.VirtualCam.ProjectPlane.Y;
    pField[5] = &Data.VirtualCam.ProjectPlane.Z;
    CT_ProcParam(pParam, pField[(UINT32) Index % VIRTUAL_CAM_PARAMS_NUM]);
    AmbaCT_StereoSetVirtulaCam(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_StereoVoutProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VOUT_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / VOUT_PARAMS_NUM;
    AMBA_CAL_ROI_s Data;
    AmbaCT_StereoGetVout(ArrayIndex, &Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32) Index % VOUT_PARAMS_NUM]);
    AmbaCT_StereoSetVout(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }

}

static void CT_StereoLensProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[LENS_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / LENS_PARAMS_NUM;
    AMBA_CT_STEREO_LENS_s Data;
    AmbaCT_StereoGetLens(ArrayIndex, &Data);
    pField[0] = &Data.LensDistoType;
    pField[1] = Data.pRealTable;
    pField[2] = Data.pExceptTable;
    pField[3] = Data.pRealAngleTable;
    pField[4] = Data.pExceptAngleTable;
    pField[5] = &Data.TableLen;
    pField[6] = Data.RealExpectFormula;
    pField[7] = Data.AngleFormula;
    CT_ProcParam(pParam, pField[(UINT32) Index % LENS_PARAMS_NUM]);
    AmbaCT_StereoSetLens(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
        if (((UINT32) Index % LENS_PARAMS_NUM) == 0U) {
            if (Data.LensDistoType == AMBA_CAL_LD_REAL_EXPECT_TBL) {
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 3U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 4U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 6U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 7U);
            } else if (Data.LensDistoType == AMBA_CAL_LD_ANGLE_TBL) {
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 1U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 2U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 6U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 7U);
            } else if (Data.LensDistoType == AMBA_CAL_LD_REAL_EXPECT_FORMULA) {
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 1U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 2U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 3U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 4U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 5U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 7U);
            } else if (Data.LensDistoType == AMBA_CAL_LD_ANGLE_FORMULA) {
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 1U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 2U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 3U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 4U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 5U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 7U);
            } else {
                /* Unsupport type */
            }
        }
    }
}

static void CT_StereoSensorProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SENSOR_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / SENSOR_PARAMS_NUM;
    AMBA_CAL_SENSOR_s Data;
    AmbaCT_StereoGetSensor(ArrayIndex, &Data);
    pField[0] = &Data.CellSize;
    pField[1] = &Data.StartX;
    pField[2] = &Data.StartY;
    pField[3] = &Data.Width;
    pField[4] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32) Index % SENSOR_PARAMS_NUM]);
    AmbaCT_StereoSetSensor(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_StereoOpticalCenterProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OPTICAL_CENTER_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / OPTICAL_CENTER_PARAMS_NUM;
    AMBA_CAL_POINT_DB_2D_s Data;
    AmbaCT_StereoGetOpticalCenter(ArrayIndex, &Data);
    pField[0] = &Data.X;
    pField[1] = &Data.Y;
    CT_ProcParam(pParam, pField[(UINT32) Index % OPTICAL_CENTER_PARAMS_NUM]);
    AmbaCT_StereoSetOpticalCenter(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_StereoCameraProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[CAMERA_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / CAMERA_PARAMS_NUM;
    AMBA_CT_STEREO_CAMERA_s Data;
    AmbaCT_StereoGetCamera(ArrayIndex, &Data);
    pField[0] = &Data.PositionX;
    pField[1] = &Data.PositionY;
    pField[2] = &Data.PositionZ;
    pField[3] = &Data.RotateType;
    CT_ProcParam(pParam, pField[(UINT32) Index % CAMERA_PARAMS_NUM]);
    AmbaCT_StereoSetCamera(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_StereoVinProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIN_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / VIN_PARAMS_NUM;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s Data;
    AmbaCT_StereoGetVin(ArrayIndex, &Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    pField[4] = &Data.HSubSample.FactorNum;
    pField[5] = &Data.HSubSample.FactorDen;
    pField[6] = &Data.VSubSample.FactorNum;
    pField[7] = &Data.VSubSample.FactorDen;
    CT_ProcParam(pParam, pField[(UINT32) Index % VIN_PARAMS_NUM]);
    AmbaCT_StereoSetVin(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }

}

static void CT_StereoRoiProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[ROI_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / ROI_PARAMS_NUM;
    AMBA_CAL_ROI_s Data;
    AmbaCT_StereoGetRoi(ArrayIndex, &Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32) Index % ROI_PARAMS_NUM]);
    AmbaCT_StereoSetRoi(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }

}

static void CT_StereoTileSizeProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[TILE_SIZE_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / TILE_SIZE_PARAMS_NUM;
    AMBA_CAL_SIZE_s Data;
    AmbaCT_StereoGetTileSize(ArrayIndex, &Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32) Index % TILE_SIZE_PARAMS_NUM]);
    AmbaCT_StereoSetTileSize(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_StereoCalibPointsProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[CALIB_POINTS_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / CALIB_POINTS_PARAMS_NUM;
    AMBA_CT_STEREO_CALIB_POINTS_s Data;
    AmbaCT_StereoGetCalibPoints(ArrayIndex, &Data);
    pField[0] = &Data.WorldPositionX[0];
    pField[1] = &Data.WorldPositionY[0];
    pField[2] = &Data.WorldPositionZ[0];
    pField[3] = &Data.RawPositionX[0];
    pField[4] = &Data.RawPositionY[0];
    pField[5] = &Data.CornerMapBinPath;
    CT_ProcParam(pParam, pField[(UINT32) Index % CALIB_POINTS_PARAMS_NUM]);
    AmbaCT_StereoSetCalibPoints(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_StereoAssistancePointsProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[ASSISTANCE_POINTS_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / ASSISTANCE_POINTS_PARAMS_NUM;
    AMBA_CT_STEREO_ASSIST_POINTS_s Data;
    AmbaCT_StereoGetAssistPoints(ArrayIndex, &Data);
    pField[0] = &Data.Number;
    pField[1] = &Data.WorldPositionX[0];
    pField[2] = &Data.WorldPositionY[0];
    pField[3] = &Data.WorldPositionZ[0];
    pField[4] = &Data.RawPositionX[0];
    pField[5] = &Data.RawPositionY[0];
    CT_ProcParam(pParam, pField[(UINT32) Index % ASSISTANCE_POINTS_PARAMS_NUM]);
    AmbaCT_StereoSetAssistPoints(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
        if ((((UINT32) Index % ASSISTANCE_POINTS_PARAMS_NUM) == 0U) && (Data.Number == 0U)) {
            *pParamStatus |= (0x1ULL << (1U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
            *pParamStatus |= (0x1ULL << (2U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
            *pParamStatus |= (0x1ULL << (3U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
            *pParamStatus |= (0x1ULL << (4U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
            *pParamStatus |= (0x1ULL << (5U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
        }
    }
}

static void CT_StereoOptimizeProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OPTIMIZE_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / OPTIMIZE_PARAMS_NUM;
    UINT8 Data;
    AmbaCT_StereoGetOptimize(ArrayIndex, &Data);
    pField[0] = &Data;
    CT_ProcParam(pParam, pField[(UINT32) Index % OPTIMIZE_PARAMS_NUM]);
    AmbaCT_StereoSetOptimize(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }

}

static void CT_StereoInternalProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[INTERNAL_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / INTERNAL_PARAMS_NUM;
    AMBA_CAL_EM_INTERNAL_CFG_s Data;
    AmbaCT_StereoGetInternal(ArrayIndex, &Data);
    pField[0] = &Data.DebugID;
    pField[1] = &Data.DmaTh;
    pField[2] = &Data.RefineOutOfRoiEnable;
    pField[3] = &Data.AutoRoiEnable;
    CT_ProcParam(pParam, pField[(UINT32) Index % INTERNAL_PARAMS_NUM]);
    AmbaCT_StereoSetInternal(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
        if (((UINT32) Index % INTERNAL_PARAMS_NUM) == 0U) {
            *pParamStatus |= (0x1ULL << ((ArrayIndex*INTERNAL_PARAMS_NUM) + 1U)); //Note # Skip Check
            *pParamStatus |= (0x1ULL << ((ArrayIndex*INTERNAL_PARAMS_NUM) + 2U)); //Note # Skip Check
            *pParamStatus |= (0x1ULL << ((ArrayIndex*INTERNAL_PARAMS_NUM) + 3U)); //Note # Skip Check
        }
    }
}

#define AMBA_CAL_STEREO_CAM_MAX ((INT32)AMBA_CAL_EM_CAM_MAX)
#define STEREO_REGS_NUM (16U)
static REG_s StereoRegs[STEREO_REGS_NUM] = {
    {(UINT32)AMBA_CT_STEREO_SYSTEM, "system", 1, 0, (INT32) SYSTEM_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoSystemParams, CT_StereoSystemProc, 0U},
    {(UINT32)AMBA_CT_STEREO_WARP_OPTIMIZER, "warp_optimizer", 1, 0, (INT32) WARP_OPTIMIZER_NUM, PARAM_STATUS_DEFAULT, CT_StereoWarpOptimizerParams, CT_StereoWarpOptimizerProc, 0U},
    {(UINT32)AMBA_CT_STEREO_OUTPUT_CFG, "output", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) OUTPUT_CFG_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoOutputCfgParams, CT_StereoOutputCfgProc, 0U},
    {(UINT32)AMBA_CT_STEREO_VIRTUAL_CAM, "virtualcam", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) VIRTUAL_CAM_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoVirtulaCamParams, CT_StereoVirtualCamProc, 0U},
    {(UINT32)AMBA_CT_STEREO_VOUT, "vout", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) VOUT_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoVoutParams, CT_StereoVoutProc, 0U},
    {(UINT32)AMBA_CT_STEREO_LENS, "lens", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) LENS_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoLensParams, CT_StereoLensProc, 0U},
    {(UINT32)AMBA_CT_STEREO_SENSOR, "sensor", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) SENSOR_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoSensorParams, CT_StereoSensorProc, 0U},
    {(UINT32)AMBA_CT_STEREO_OPTICAL_CENTER, "optical_center", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) OPTICAL_CENTER_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoOpticalCenterParams, CT_StereoOpticalCenterProc, 0U},
    {(UINT32)AMBA_CT_STEREO_CAMERA, "camera", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) CAMERA_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoCameraParams, CT_StereoCameraProc, 0U},
    {(UINT32)AMBA_CT_STEREO_VIN, "vin", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) VIN_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoVinParams, CT_StereoVinProc, 0U},
    {(UINT32)AMBA_CT_STEREO_ROI, "roi", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) ROI_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoRoiParams, CT_StereoRoiProc, 0U},
    {(UINT32)AMBA_CT_STEREO_TILE_SIZE, "tile_size", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) TILE_SIZE_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoTileSizeParams, CT_StereoTileSizeProc, 0U},
    {(UINT32)AMBA_CT_STEREO_CALIB_POINTS, "calib_points", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) CALIB_POINTS_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoCalibPointsParams, CT_StereoCalibPointsProc, 0U},
    {(UINT32)AMBA_CT_STEREO_ASSIST_POINTS, "assistance_points", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) ASSISTANCE_POINTS_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoAssistancePointsParams, CT_StereoAssistancePointsProc, 0U},
    {(UINT32)AMBA_CT_STEREO_OPTIMIZE, "optimize", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) OPTIMIZE_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoOptimizeParams, CT_StereoOptimizeProc, 0U},
    {(UINT32)AMBA_CT_STEREO_INTERNAL, "internal", AMBA_CAL_STEREO_CAM_MAX, 0, (INT32) INTERNAL_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_StereoInternalParams, CT_StereoInternalProc, ATTRIBUTE_HIDE},
};

UINT32 AmbaCT_GetStereoRuleInfo(Rule_Info_t *pRuleInfo)
{
    pRuleInfo->RegList = &StereoRegs[0U];
    pRuleInfo->RegCount = (UINT8)STEREO_REGS_NUM;
    return 0U;
}

UINT32 AmbaCT_GetStereoRuleInfoMisra(Rule_Info_t *pRuleInfo);
UINT32 AmbaCT_GetStereoRuleInfoMisra(Rule_Info_t *pRuleInfo)
{
    pRuleInfo->RegList = &StereoRegs[0];
    pRuleInfo->RegCount = (UINT8)STEREO_REGS_NUM;
    return 0U;
}
