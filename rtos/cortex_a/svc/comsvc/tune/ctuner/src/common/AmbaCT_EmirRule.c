/**
 *  @file AmbaCT_EmirRule.c
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
#include "AmbaCT_EmirTuner.h"
#include "AmbaCT_EmirRule.h"
#define SYSTEM_PARAMS_NUM (4U)
static PARAM_s CT_EmSystemParams[SYSTEM_PARAMS_NUM] = {
    {0, "calib_mode", STRING, 1U, NULL, 0},
    {1, "type", STRING, 1U, NULL, 0U},
    {2, "chip_rev", STRING, 1U, NULL, 0U},
    {3, "output_prefix", STRING, 1U, NULL, 0U},
};
#define CAR_PARAMS_NUM (2U)
static PARAM_s CT_EmCarParams[CAR_PARAMS_NUM] = {
    {0, "width", U_32, 1U, NULL, 0U},
    {1, "height", U_32, 1U, NULL, 0U},
};

#define VIEW_3IN1_PARAMS_NUM (12U)
static PARAM_s CT_EmView3in1Params[VIEW_3IN1_PARAMS_NUM] = {
    {0, "start_pos_x", S_DB, 1, NULL, 0},
    {1, "start_pos_y", S_DB, 1, NULL, 0},
    {2, "start_pos_z", S_DB, 1, NULL, 0},
    {3, "width", S_DB, 1, NULL, 0},
    {4, "distance_of_stitch_plane", U_32, 1, NULL,0},
    {5, "back_scale", S_DB, 1, NULL, 0},
    {6, "back_hor_shift", S_DB, 1, NULL, 0},
    {7, "back_ver_shift", S_DB, 1, NULL, 0},
    {8, "left_hor_shift", S_DB, 1, NULL, 0},
    {9, "right_hor_shift", S_DB, 1, NULL, 0},
    {10, "valid_vout_lower_bound", U_32, 1, NULL, 0},
    {11, "mirror_flip_cfg", U_32, 1, NULL, 0},
};

#define VIEW_SINGLE_PARAMS_NUM (1U)
static PARAM_s CT_EmViewSingleParams[VIEW_SINGLE_PARAMS_NUM] = {
    {0, "type", U_8, 1, NULL, 0},
};

#define VIEW_SINGLE_ASP_CFG_PARAMS_NUM (4U)
static PARAM_s CT_EmViewSingleAspCfgParams[] = {
    {0, "point_src_x", S_DB, 6, NULL, 0},
    {1, "point_src_y", S_DB, 6, NULL, 0},
    {2, "point_dst_x", S_DB, 6, NULL, 0},
    {3, "point_dst_y", S_DB, 6, NULL, 0},
};

#define VIEW_SINGLE_CURVED_CFG_PARAMS_NUM (14U)
static PARAM_s CT_EmViewSingleCurvedCfgParams[VIEW_SINGLE_CURVED_CFG_PARAMS_NUM] = {
    {0, "focus_x", S_DB, 1, NULL, 0},
    {1, "focus_y", S_DB, 1, NULL, 0},
    {2, "focus_z", S_DB, 1, NULL, 0},
    {3, "project_plane_center_x", S_DB, 1, NULL, 0},
    {4, "project_plane_center_y", S_DB, 1, NULL, 0},
    {5, "project_plane_center_z", S_DB, 1, NULL, 0},
    {6, "world_pos_curve_left_x", S_DB, 1, NULL, 0},
    {7, "world_pos_curve_left_y", S_DB, 1, NULL, 0},
    {8, "world_pos_curve_right_x", S_DB, 1, NULL, 0},
    {9, "world_pos_curve_right_y", S_DB, 1, NULL, 0},
    {10, "world_pos_line_left_x", S_DB, 1, NULL, 0},
    {11, "world_pos_line_left_y", S_DB, 1, NULL, 0},
    {12, "world_pos_line_right_x", S_DB, 1, NULL, 0},
    {13, "world_pos_line_right_y", S_DB, 1, NULL, 0},
};

#define VIEW_BLEND_PARAMS_NUM (6U)
static PARAM_s CT_EmViewBlendParams[VIEW_BLEND_PARAMS_NUM] = {
    {0, "left_start_x", U_32, 1, NULL,0},
    {1, "left_angle", S_32, 1, NULL, 0},
    {2, "left_width", U_32, 1, NULL, 0},
    {3, "right_start_x", U_32, 1, NULL,0},
    {4, "right_angle", S_32, 1, NULL, 0},
    {5, "right_width", U_32, 1, NULL, 0},
};

#define VOUT_PARAMS_NUM (4U)
static PARAM_s CT_EmVoutParams[VOUT_PARAMS_NUM] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
};

#define MASK_PARAMS_NUM (4U)
static PARAM_s CT_EmMaskParams[] = {
    {0, "width", U_32, 1, NULL, 0},
    {1, "height", U_32, 1, NULL, 0},
    {2, "table_path", STRING, 1, NULL, 0},
    {3, "from_file", U_32, 1, NULL, 0},
};

#define LENS_PARAMS_NUM (8U)
static PARAM_s CT_EmLensParams[LENS_PARAMS_NUM] = {
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
static PARAM_s CT_EmSensorParams[SENSOR_PARAMS_NUM] = {
    {0, "cell_size", S_DB, 1, NULL, 0},
    {1, "start_x", U_32, 1, NULL, 0},
    {2, "start_y", U_32, 1, NULL, 0},
    {3, "width", U_32, 1, NULL, 0},
    {4, "height", U_32, 1, NULL, 0},
};

#define OPTICAL_CENTER_PARAMS_NUM (2U)
static PARAM_s CT_EmOpticalCenterParams[OPTICAL_CENTER_PARAMS_NUM] = {
    {0, "x", S_DB, 1, NULL, 0},
    {1, "y", S_DB, 1, NULL, 0},
};

#define CAMERA_PARAMS_NUM (4U)
static PARAM_s CT_EmCameraParams[CAMERA_PARAMS_NUM] = {
    {0, "pos_x", S_DB, 1, NULL, 0},
    {1, "pos_y", S_DB, 1, NULL, 0},
    {2, "pos_z", S_DB, 1, NULL, 0},
    {3, "rotate_type", U_8, 1, NULL, 0},
};
#define VIN_PARAMS_NUM (8U)
static PARAM_s CT_EmVinParams[VIN_PARAMS_NUM] = {
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
static PARAM_s CT_EmRoiParams[ROI_PARAMS_NUM] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
};

#define TILE_SIZE_PARAMS_NUM (2U)
static PARAM_s CT_EmTileSizeParams[TILE_SIZE_PARAMS_NUM] = {
    {0, "width", U_32, 1, NULL, 0},
    {1, "height", U_32, 1, NULL, 0},
};

#define CALIB_POINTS_PARAMS_NUM (5U)
static PARAM_s CT_EmCalibPointsParams[CALIB_POINTS_PARAMS_NUM] = {
    {0, "world_pos_x", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
    {1, "world_pos_y", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
    {2, "world_pos_z", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
    {3, "raw_pos_x", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
    {4, "raw_pos_y", S_DB, EMIR_CALIB_POINT_NUM, NULL, 0},
};

#define ASSISTANCE_POINTS_PARAMS_NUM (6U)
static PARAM_s CT_EmAssistancePointsParams[] = {
    {0, "number", U_32, 1, NULL, 0},
    {1, "world_pos_x", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
    {2, "world_pos_y", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
    {3, "world_pos_z", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
    {4, "raw_pos_x", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
    {5, "raw_pos_y", S_DB, MAX_EMIR_ASSISTANCE_POINT_NUM, NULL, 0},
};

#define OPTIMIZE_PARAMS_NUM (1U)
static PARAM_s CT_EmOptimizeParams[OPTIMIZE_PARAMS_NUM] = {
    {0, "level", U_8, 1, NULL, 0},
};

#define INTERNAL_PARAMS_NUM (6U)
static PARAM_s CT_EmInternalParams[INTERNAL_PARAMS_NUM] = {
    {0, "debug_id", U_32, 1, NULL, 0},
    {1, "wait_line_th", U_32, 1, NULL, 0},
    {2, "dma_th", U_32, 1, NULL, 0},
    {3, "raw_pos_horflip_enable", U_32, 1, NULL, 0},
    {4, "out_of_roi_refine_enable", U_32, 1, NULL, 0},
    {5, "auto_roi_refine_enable", U_32, 1, NULL, 0},
};

static void CT_EmSystemProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_NUM];
    AMBA_CT_EM_SYSTEM_s Data;
    AmbaCT_EmGetSystem(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.Type;
    pField[2] = Data.ChipRev;
    pField[3] = Data.OutputPrefix;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_EmSetSystem(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}


static void CT_EmCarProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[CAR_PARAMS_NUM];
    AMBA_CAL_SIZE_s Data;
    AmbaCT_EmGetCar(&Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_EmSetCar(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmView3in1Proc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIEW_3IN1_PARAMS_NUM];
    AMBA_CT_EM_3IN1_VIEW_s Data;
    AmbaCT_EmGet3in1View(&Data);
    pField[0] = &Data.StartPosX;
    pField[1] = &Data.StartPosY;
    pField[2] = &Data.StartPosZ;
    pField[3] = &Data.Width;
    pField[4] = &Data.DistanceOfStitchPlane;
    pField[5] = &Data.BackViewScale;
    pField[6] = &Data.BackViewHorShift;
    pField[7] = &Data.BackViewVerShift;
    pField[8] = &Data.LeftViewHorShift;
    pField[9] = &Data.RightViewHorShift;
    pField[10] = &Data.ValidVoutLowerBound;
    pField[11] = &Data.MirrorFlipCfg;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_EmSet3in1View(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        *pParamStatus |= (0x1ULL << (((UINT32)Index * VIEW_3IN1_PARAMS_NUM) + 11U));
    }
}

static void CT_EmViewSingleProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIEW_SINGLE_PARAMS_NUM];
    AMBA_CAL_EM_VIEW_TYPE_e Data;
    AmbaCT_EmGetSingleView(&Data);
    pField[0] = &Data;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_EmSetSingleView(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmViewSingleAspCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIEW_SINGLE_ASP_CFG_PARAMS_NUM];
    AMBA_CT_EM_ASP_POINT_MAP_s Data;
    AmbaCT_EmGetSingleViewAspCfg(&Data);
    pField[0] = &Data.SrcX[0];
    pField[1] = &Data.SrcY[0];
    pField[2] = &Data.DstX[0];
    pField[3] = &Data.DstY[0];
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_EmSetSingleViewAspCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmViewSingleCurvedCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIEW_SINGLE_CURVED_CFG_PARAMS_NUM];
    AMBA_CAL_EM_CURVED_SURFACE_CFG_s Data;
    AmbaCT_EmGetSingleViewCurvedCfg(&Data);
    pField[0] = &Data.VirtualCam.Focus.X;
    pField[1] = &Data.VirtualCam.Focus.Y;
    pField[2] = &Data.VirtualCam.Focus.Z;
    pField[3] = &Data.VirtualCam.ProjectPlane.X;
    pField[4] = &Data.VirtualCam.ProjectPlane.Y;
    pField[5] = &Data.VirtualCam.ProjectPlane.Z;
    pField[6] = &Data.WorldPosCurveLeft.X;
    pField[7] = &Data.WorldPosCurveLeft.Y;
    pField[8] = &Data.WorldPosCurveRight.X;
    pField[9] = &Data.WorldPosCurveRight.Y;
    pField[10] = &Data.WorldPosLineLeft.X;
    pField[11] = &Data.WorldPosLineLeft.Y;
    pField[12] = &Data.WorldPosLineRight.X;
    pField[13] = &Data.WorldPosLineRight.Y;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_EmSetSingleViewCurvedCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmView3in1BlendProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIEW_BLEND_PARAMS_NUM];
    AMBA_CT_EM_3IN1_BLEND_s Data;
    AmbaCT_EmGet3in1Blend(&Data);
    pField[0] = &Data.Left.StartX;
    pField[1] = &Data.Left.Angle;
    pField[2] = &Data.Left.Width;
    pField[3] = &Data.Right.StartX;
    pField[4] = &Data.Right.Angle;
    pField[5] = &Data.Right.Width;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_EmSet3in1Blend(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmVoutProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VOUT_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / VOUT_PARAMS_NUM;
    AMBA_CAL_ROI_s Data;
    AmbaCT_EmGetVout(ArrayIndex, &Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32) Index % VOUT_PARAMS_NUM]);
    AmbaCT_EmSetVout(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }

}

static void CT_EmMaskProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[MASK_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / MASK_PARAMS_NUM;
    AMBA_CT_EM_MASK_s *pData = NULL;
    AmbaCT_EmGetMask(ArrayIndex, &pData);
    if (pData != NULL) {
        pField[0] = &pData->Width;
        pField[1] = &pData->Height;
        pField[2] = pData->TablePath;
        pField[3] = &pData->FromFile;
        CT_ProcParam(pParam, pField[(UINT32) Index % MASK_PARAMS_NUM]);
        AmbaCT_EmSetMask(ArrayIndex, pData);
        if (pParamStatus != NULL) {
            *pParamStatus |= (0x1ULL << (UINT32) Index);
        }
    }
}

static void CT_EmLensProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[LENS_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / LENS_PARAMS_NUM;
    AMBA_CT_EM_LENS_s Data;
    AmbaCT_EmGetLens(ArrayIndex, &Data);
    pField[0] = &Data.LensDistoType;
    pField[1] = Data.pRealTable;
    pField[2] = Data.pExceptTable;
    pField[3] = Data.pRealAngleTable;
    pField[4] = Data.pExceptAngleTable;
    pField[5] = &Data.TableLen;
    pField[6] = Data.RealExpectFormula;
    pField[7] = Data.AngleFormula;
    CT_ProcParam(pParam, pField[(UINT32) Index % LENS_PARAMS_NUM]);
    AmbaCT_EmSetLens(ArrayIndex, &Data);
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

static void CT_EmSensorProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SENSOR_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / SENSOR_PARAMS_NUM;
    AMBA_CAL_SENSOR_s Data;
    AmbaCT_EmGetSensor(ArrayIndex, &Data);
    pField[0] = &Data.CellSize;
    pField[1] = &Data.StartX;
    pField[2] = &Data.StartY;
    pField[3] = &Data.Width;
    pField[4] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32) Index % SENSOR_PARAMS_NUM]);
    AmbaCT_EmSetSensor(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmOpticalCenterProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OPTICAL_CENTER_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / OPTICAL_CENTER_PARAMS_NUM;
    AMBA_CAL_POINT_DB_2D_s Data;
    AmbaCT_EmGetOpticalCenter(ArrayIndex, &Data);
    pField[0] = &Data.X;
    pField[1] = &Data.Y;
    CT_ProcParam(pParam, pField[(UINT32) Index % OPTICAL_CENTER_PARAMS_NUM]);
    AmbaCT_EmSetOpticalCenter(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmCameraProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[CAMERA_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / CAMERA_PARAMS_NUM;
    AMBA_CT_EM_CAMERA_s Data;
    AmbaCT_EmGetCamera(ArrayIndex, &Data);
    pField[0] = &Data.PositionX;
    pField[1] = &Data.PositionY;
    pField[2] = &Data.PositionZ;
    pField[3] = &Data.RotateType;
    CT_ProcParam(pParam, pField[(UINT32) Index % CAMERA_PARAMS_NUM]);
    AmbaCT_EmSetCamera(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmVinProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[VIN_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / VIN_PARAMS_NUM;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s Data;
    AmbaCT_EmGetVin(ArrayIndex, &Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    pField[4] = &Data.HSubSample.FactorNum;
    pField[5] = &Data.HSubSample.FactorDen;
    pField[6] = &Data.VSubSample.FactorNum;
    pField[7] = &Data.VSubSample.FactorDen;
    CT_ProcParam(pParam, pField[(UINT32) Index % VIN_PARAMS_NUM]);
    AmbaCT_EmSetVin(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }

}

static void CT_EmRoiProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[ROI_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / ROI_PARAMS_NUM;
    AMBA_CAL_ROI_s Data;
    AmbaCT_EmGetRoi(ArrayIndex, &Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32) Index % ROI_PARAMS_NUM]);
    AmbaCT_EmSetRoi(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }

}

static void CT_EmTileSizeProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[TILE_SIZE_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / TILE_SIZE_PARAMS_NUM;
    AMBA_CAL_SIZE_s Data;
    AmbaCT_EmGetTileSize(ArrayIndex, &Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32) Index % TILE_SIZE_PARAMS_NUM]);
    AmbaCT_EmSetTileSize(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmCalibPointsProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[CALIB_POINTS_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / CALIB_POINTS_PARAMS_NUM;
    AMBA_CT_EM_CALIB_POINTS_s Data;
    AmbaCT_EmGetCalibPoints(ArrayIndex, &Data);
    pField[0] = &Data.WorldPositionX[0];
    pField[1] = &Data.WorldPositionY[0];
    pField[2] = &Data.WorldPositionZ[0];
    pField[3] = &Data.RawPositionX[0];
    pField[4] = &Data.RawPositionY[0];
    CT_ProcParam(pParam, pField[(UINT32) Index % CALIB_POINTS_PARAMS_NUM]);
    AmbaCT_EmSetCalibPoints(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_EmAssistancePointsProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[ASSISTANCE_POINTS_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / ASSISTANCE_POINTS_PARAMS_NUM;
    AMBA_CT_EM_ASSISTANCE_POINTS_s Data;
    AmbaCT_EmGetAssistancePoints(ArrayIndex, &Data);
    pField[0] = &Data.Number;
    pField[1] = &Data.WorldPositionX[0];
    pField[2] = &Data.WorldPositionY[0];
    pField[3] = &Data.WorldPositionZ[0];
    pField[4] = &Data.RawPositionX[0];
    pField[5] = &Data.RawPositionY[0];
    CT_ProcParam(pParam, pField[(UINT32) Index % ASSISTANCE_POINTS_PARAMS_NUM]);
    AmbaCT_EmSetAssistancePoints(ArrayIndex, &Data);
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

static void CT_EmOptimizeProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OPTIMIZE_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / OPTIMIZE_PARAMS_NUM;
    UINT8 Data;
    AmbaCT_EmGetOptimize(ArrayIndex, &Data);
    pField[0] = &Data;
    CT_ProcParam(pParam, pField[(UINT32) Index % OPTIMIZE_PARAMS_NUM]);
    AmbaCT_EmSetOptimize(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }

}

static void CT_EmInternalProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[INTERNAL_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32) Index / INTERNAL_PARAMS_NUM;
    AMBA_CAL_EM_INTERNAL_CFG_s Data;
    AmbaCT_EmGetInternal(ArrayIndex, &Data);
    pField[0] = &Data.DebugID;
    pField[1] = &Data.WaitlineTh;
    pField[2] = &Data.DmaTh;
    pField[3] = &Data.PointMapHFlipEnable;
    pField[4] = &Data.RefineOutOfRoiEnable;
    pField[5] = &Data.AutoRoiEnable;
    CT_ProcParam(pParam, pField[(UINT32) Index % INTERNAL_PARAMS_NUM]);
    AmbaCT_EmSetInternal(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
        if (((UINT32) Index % INTERNAL_PARAMS_NUM) == 0U) {
            *pParamStatus |= (0x1ULL << ((ArrayIndex*INTERNAL_PARAMS_NUM) + 3U)); //Note # Skip Check
            *pParamStatus |= (0x1ULL << ((ArrayIndex*INTERNAL_PARAMS_NUM) + 4U)); //Note # Skip Check
            *pParamStatus |= (0x1ULL << ((ArrayIndex*INTERNAL_PARAMS_NUM) + 5U)); //Note # Skip Check
        }
    }
}

#define AMBA_CAL_EMIR_CAM_MAX ((INT32)AMBA_CAL_EM_CAM_MAX)
#define EM_REGS_NUM (20U)
static REG_s EmRegs[EM_REGS_NUM] = {
    {(UINT32)AMBA_CT_EM_SYSTEM, "system", 1, 0, (INT32) SYSTEM_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmSystemParams, CT_EmSystemProc, 0U},
    {(UINT32)AMBA_CT_EM_CAR, "car", 1, 0, (INT32) CAR_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmCarParams, CT_EmCarProc, 0U},
    {(UINT32)AMBA_CT_EM_3IN1_VIEW, "view_3_in_1", 1, 0, (INT32) VIEW_3IN1_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmView3in1Params, CT_EmView3in1Proc, 0U},
    {(UINT32)AMBA_CT_EM_SINGLE_VIEW, "view_single", 1, 0, (INT32) VIEW_SINGLE_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmViewSingleParams, CT_EmViewSingleProc, 0U},
    {(UINT32)AMBA_CT_EM_SINGLE_ASP_CFG, "view_single_aspheric", 1, 0, (INT32) VIEW_SINGLE_ASP_CFG_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmViewSingleAspCfgParams, CT_EmViewSingleAspCfgProc, 0U},
    {(UINT32)AMBA_CT_EM_SINGLE_CURVED_CFG, "view_single_virtualcam", 1, 0, (INT32) VIEW_SINGLE_CURVED_CFG_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmViewSingleCurvedCfgParams, CT_EmViewSingleCurvedCfgProc, 0U},
    {(UINT32)AMBA_CT_EM_3IN1_BLEND, "blend_3_in_1", 1, 0, (INT32) VIEW_BLEND_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmViewBlendParams, CT_EmView3in1BlendProc, 0U},
    {(UINT32)AMBA_CT_EM_VOUT, "vout", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) VOUT_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmVoutParams, CT_EmVoutProc, 0U},
    {(UINT32)AMBA_CT_EM_MASK, "mask", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) MASK_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmMaskParams, CT_EmMaskProc, 0U},
    {(UINT32)AMBA_CT_EM_LENS, "lens", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) LENS_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmLensParams, CT_EmLensProc, 0U},
    {(UINT32)AMBA_CT_EM_SENSOR, "sensor", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) SENSOR_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmSensorParams, CT_EmSensorProc, 0U},
    {(UINT32)AMBA_CT_EM_OPTICAL_CENTER, "optical_center", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) OPTICAL_CENTER_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmOpticalCenterParams, CT_EmOpticalCenterProc, 0U},
    {(UINT32)AMBA_CT_EM_CAMERA, "camera", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) CAMERA_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmCameraParams, CT_EmCameraProc, 0U},
    {(UINT32)AMBA_CT_EM_VIN, "vin", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) VIN_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmVinParams, CT_EmVinProc, 0U},
    {(UINT32)AMBA_CT_EM_ROI, "roi", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) ROI_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmRoiParams, CT_EmRoiProc, 0U},
    {(UINT32)AMBA_CT_EM_TILE_SIZE, "tile_size", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) TILE_SIZE_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmTileSizeParams, CT_EmTileSizeProc, 0U},
    {(UINT32)AMBA_CT_EM_CALIB_POINTS, "calib_points", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) CALIB_POINTS_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmCalibPointsParams, CT_EmCalibPointsProc, 0U},
    {(UINT32)AMBA_CT_EM_ASSISTANCE_POINTS, "assistance_points", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) ASSISTANCE_POINTS_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmAssistancePointsParams, CT_EmAssistancePointsProc, 0U},
    {(UINT32)AMBA_CT_EM_OPTIMIZE, "optimize", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) OPTIMIZE_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmOptimizeParams, CT_EmOptimizeProc, 0U},
    {(UINT32)AMBA_CT_EM_INTERNAL, "internal", AMBA_CAL_EMIR_CAM_MAX, 0, (INT32) INTERNAL_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_EmInternalParams, CT_EmInternalProc, ATTRIBUTE_HIDE},
};

UINT32 AmbaCT_GetEmRuleInfo(Rule_Info_t *pRuleInfo)
{
    pRuleInfo->RegList = &EmRegs[0U];
    pRuleInfo->RegCount = (UINT8)EM_REGS_NUM;
    return 0U;
}

UINT32 AmbaCT_GetEmRuleInfoMisra(Rule_Info_t *pRuleInfo);
UINT32 AmbaCT_GetEmRuleInfoMisra(Rule_Info_t *pRuleInfo)
{
    pRuleInfo->RegList = &EmRegs[0];
    pRuleInfo->RegCount = (UINT8)EM_REGS_NUM;
    return 0U;
}
