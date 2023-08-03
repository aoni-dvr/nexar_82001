/*
 *  @file AmbaCT_AvmRule.c
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
#include "AmbaCT_AvmTuner.h"
#include "AmbaCT_AvmRule.h"


#define SYSTEM_PARAMS_NUM (5U)
static PARAM_s CT_AvmSystemParams[SYSTEM_PARAMS_NUM] = {
    {0, "calib_mode", STRING, 1U, NULL, 0U},
    {1, "type", STRING, 1U, NULL, 0U},
    {2, "chip_rev", STRING, 1U, NULL, 0U},
    {3, "output_prefix", STRING, 1U, NULL, 0U},
    {4, "enable_save_precheck", U_32, 1U, NULL, 0U},
};

#define INTERNAL_PARAMS_NUM (9U)
static PARAM_s CT_AvmInternalParams[INTERNAL_PARAMS_NUM] = {
    {0, "debug_id", U_32, 1U, NULL, 0U},
    {1, "wait_line_th", U_32, 1U, NULL, 0U},
    {2, "dma_th", U_32, 1U, NULL, 0U},
    {3, "refine_dma_enable", U_32, 1U, NULL, 0U},
    {4, "refine_waitline_enable", U_32, 1U, NULL, 0U},
    {5, "refine_out_of_roi_enable", U_32, 1U, NULL, 0U},
    {6, "refine_vflip_enable", U_32, 1U, NULL, 0U},
    {7, "extrapolate_plane_boundary", U_32, 1U, NULL, 0U},
    {8, "repeat_unused_area", U_32, 1U, NULL, 0U},
};

#define CAR_PARAMS_NUM (2U)
static PARAM_s CT_AvmCarParams[CAR_PARAMS_NUM] = {
    {0, "width", U_32, 1U, NULL, 0U},
    {1, "height", U_32, 1U, NULL, 0U},
};

#define VIEW_2D_PARAMS_NUM (6U)
static PARAM_s CT_Avm2dViewParams[VIEW_2D_PARAMS_NUM] = {
    {0, "crop_range_start_x", S_32, 1U, NULL, 0U},
    {1, "crop_range_start_y", S_32, 1U, NULL, 0U},
    {2, "crop_width", U_32, 1U, NULL, 0U},
    {3, "crop_height", U_32, 1U, NULL, 0U},
    {4, "display_height", U_32, 1U, NULL, 0U},
    {5, "auto_vout_order_enable", U_32, 1U, NULL, 0U},
};

#define CUSTOMER_2D_CALIB_PARAMS_NUM (2U)
static PARAM_s CT_Avm2dCustomCalibParams[CUSTOMER_2D_CALIB_PARAMS_NUM] = {
    {0, "enable", U_32, 1U, NULL, 0U},
    {1, "calib_matrix", S_DB, 9U, NULL, 0U},
};

#define VIEW_3D_MODEL_TYPE_NUM (1U)
static PARAM_s CT_Avm3dViewModelType[VIEW_3D_MODEL_TYPE_NUM] = {
    {0, "type", STRING, 1U, NULL, 0U},
};

#define VIEW_3D_PARAMS_NUM (10U)
static PARAM_s CT_Avm3dViewParams[VIEW_3D_PARAMS_NUM] = {
    {0, "floor_range_x", U_32, 1U, NULL, 0U},
    {1, "floor_range_y", U_32, 1U, NULL, 0U},
    {2, "center_pos_x", S_DB, 1U, NULL, 0U},
    {3, "center_pos_y", S_DB, 1U, NULL, 0U},
    {4, "distance", U_32, 1U, NULL, 0U},
    {5, "height", U_32, 1U, NULL, 0U},
    {6, "display_width", U_32, 1U, NULL, 0U},
    {7, "display_height", U_32, 1U, NULL, 0U},
    {8, "display_rotation", U_32, 1U, NULL, 0U},
    {9, "auto_vout_order_enable", U_32, 1U, NULL, 0U},
};

#define ADVANCED_3D_VIEW_PARAMS_NUM (36U)
static PARAM_s CT_Avm3dAdvancedViewParams[ADVANCED_3D_VIEW_PARAMS_NUM] = {
    {0, "top_front_range", U_32, 1U, NULL, 0U},
    {1, "top_back_range", U_32, 1U, NULL, 0U},
    {2, "top_left_range", U_32, 1U, NULL, 0U},
    {3, "top_right_range", U_32, 1U, NULL, 0U},
    {4, "top_front_left_radius_x", U_32, 1U, NULL, 0U},
    {5, "top_front_left_radius_y", U_32, 1U, NULL, 0U},
    {6, "top_front_right_radius_x", U_32, 1U, NULL, 0U},
    {7, "top_front_right_radius_y", U_32, 1U, NULL, 0U},
    {8, "top_back_left_radius_x", U_32, 1U, NULL, 0U},
    {9, "top_back_left_radius_y", U_32, 1U, NULL, 0U},
    {10, "top_back_right_radius_x", U_32, 1U, NULL, 0U},
    {11, "top_back_right_radius_y", U_32, 1U, NULL, 0U},
    {12, "top_height", U_32, 1U, NULL, 0U},
    {13, "bottom_front_range", U_32, 1U, NULL, 0U},
    {14, "bottom_back_range", U_32, 1U, NULL, 0U},
    {15, "bottom_left_range", U_32, 1U, NULL, 0U},
    {16, "bottom_right_range", U_32, 1U, NULL, 0U},
    {17, "bottom_front_left_radius_x", U_32, 1U, NULL, 0U},
    {18, "bottom_front_left_radius_y", U_32, 1U, NULL, 0U},
    {19, "bottom_front_right_radius_x", U_32, 1U, NULL, 0U},
    {20, "bottom_front_right_radius_y", U_32, 1U, NULL, 0U},
    {21, "bottom_back_left_radius_x", U_32, 1U, NULL, 0U},
    {22, "bottom_back_left_radius_y", U_32, 1U, NULL, 0U},
    {23, "bottom_back_right_radius_x", U_32, 1U, NULL, 0U},
    {24, "bottom_back_right_radius_y", U_32, 1U, NULL, 0U},
    {25, "top_bottom_radius_h", U_32, 1U, NULL, 0U},
    {26, "top_bottom_radius_v", U_32, 1U, NULL, 0U},
    {27, "bottom_corner_precision_num", U_32, 1U, NULL, 0U},
    {28, "pillar_precision_num", U_32, 1U, NULL, 0U},
    {29, "center_pos_x", S_DB, 1U, NULL, 0U},
    {30, "center_pos_y", S_DB, 1U, NULL, 0U},
    {31, "rotation_angle", S_DB, 1U, NULL, 0U},
    {32, "display_width", U_32, 1U, NULL, 0U},
    {33, "display_height", U_32, 1U, NULL, 0U},
    {34, "display_rotation", U_32, 1U, NULL, 0U},
    {35, "auto_vout_order_enable", U_32, 1U, NULL, 0U},
};

#define WARP_3D_PARAMS_NUM (1U)
static PARAM_s CT_Avm3dWarpParams[WARP_3D_PARAMS_NUM] = {
    {0, "cal_type", U_8, 1U, NULL, 0U},
};

#define MAIN_VIEW_PARAMS_NUM (1U)
static PARAM_s CT_AvmMainViewParams[MAIN_VIEW_PARAMS_NUM] = {
    {0, "type", U_8, 1U, NULL, 0U},
};

#define MAIN_VIEW_PERSPECTIBVE_CFG_PARAMS_NUM (23U)
static PARAM_s CT_AvmMainViewPerspectiveCfgParams[MAIN_VIEW_PERSPECTIBVE_CFG_PARAMS_NUM] = {
    {0, "start_degrees_x", S_DB, 1U, NULL, 0U},
    {1, "start_degrees_y", S_DB, 1U, NULL, 0U},
    {2, "total_view_angle", S_DB, 1U, NULL, 0U},
    {3, "total_view_angle_y", S_DB, 1U, NULL, 0U},
    {4, "display_width", U_32, 1U, NULL, 0U},
    {5, "display_height", U_32, 1U, NULL, 0U},
    {6, "projection_rotation_degree_h", S_DB, 1U, NULL, 0U},
    {7, "projection_rotation_degree_v", S_DB, 1U, NULL, 0U},
    {8, "projection_rotation_degree_f", S_DB, 1U, NULL, 0U},
    {9, "lde_real_table_upper", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {10, "lde_expect_table_upper", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {11, "lde_table_length_upper", U_32, 1U, NULL, 0U},
    {12, "lde_real_table_lower", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {13, "lde_expect_table_lower", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {14, "lde_table_length_lower", U_32, 1U, NULL, 0U},
    {15, "lde_real_table_left", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {16, "lde_expect_table_left", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {17, "lde_table_length_left", U_32, 1U, NULL, 0U},
    {18, "lde_real_table_right", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {19, "lde_expect_table_right", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {20, "lde_table_length_right", U_32, 1U, NULL, 0U},
    {21, "lde_center_x", S_DB, 1U, NULL, 0U},
    {22, "lde_center_y", S_DB, 1U, NULL, 0U},
};

#define MAIN_VIEW_EQUALDISTANCE_CFG_PARAMS_NUM (6U)
static PARAM_s CT_AvmMainViewEqualDistanceCfgParams[MAIN_VIEW_EQUALDISTANCE_CFG_PARAMS_NUM] = {
    {0, "start_degrees_x", S_DB, 1U, NULL, 0U},
    {1, "start_degrees_y", S_DB, 1U, NULL, 0U},
    {2, "total_view_angle", S_DB, 1U, NULL, 0U},
    {3, "total_view_angle_y", S_DB, 1U, NULL, 0U},
    {4, "display_width", U_32, 1U, NULL, 0U},
    {5, "display_height", U_32, 1U, NULL, 0U},
};

#define MAIN_VIEW_LDC_CFG_PARAMS_NUM (1U)
static PARAM_s CT_AvmMainViewLdcCfgParams[MAIN_VIEW_LDC_CFG_PARAMS_NUM] = {
    {0, "compensate_ratio", U_32, 1U, NULL, 0U},
};

#define MAIN_VIEW_DIRECTION_PARAMS_NUM (3U)
static PARAM_s CT_AvmMainViewDirectionParams[MAIN_VIEW_DIRECTION_PARAMS_NUM] = {
    {0, "x", S_DB, 1U, NULL, 0U},
    {1, "y", S_DB, 1U, NULL, 0U},
    {2, "z", S_DB, 1U, NULL, 0U},
};

#define VIRTUAL_3D_CAM_PARAMS_NUM (8U)
static PARAM_s CT_Avm3dVirtualCamParams[VIRTUAL_3D_CAM_PARAMS_NUM] = {
    {0, "focus_x", S_DB, 1U, NULL, 0U},
    {1, "focus_y", S_DB, 1U, NULL, 0U},
    {2, "focus_z", S_DB, 1U, NULL, 0U},
    {3, "project_plane_x", S_DB, 1U, NULL, 0U},
    {4, "project_plane_y", S_DB, 1U, NULL, 0U},
    {5, "project_plane_z", S_DB, 1U, NULL, 0U},
    {6, "project_plane_shift_x", S_DB, 1U, NULL, 0U},
    {7, "project_plane_shift_y", S_DB, 1U, NULL, 0U},
};

#define VIEW_BLEND_PARAMS_NUM (4U)
static PARAM_s CT_AvmViewBlendParams[VIEW_BLEND_PARAMS_NUM] = {
    {0, "angle", U_32, 1U, NULL, 0U},
    {1, "width", U_32, 1U, NULL, 0U},
    {2, "use_car_mask", U_32, 1U, NULL, 0U},
    {3, "sector_angle", U_32, 1U, NULL, 0U},
};

#define OSD_MASK_PARAMS_NUM (6U)
static PARAM_s CT_AvmOsdMaskParams[OSD_MASK_PARAMS_NUM] = {
    {0, "start_x", U_32, 1U, NULL, 0U},
    {1, "start_y", U_32, 1U, NULL, 0U},
    {2, "width", U_32, 1U, NULL, 0U},
    {3, "height", U_32, 1U, NULL, 0U},
    {4, "table_path", STRING, 1U, NULL, 0U},
    {5, "from_file", U_32, 1U, NULL, 0U},
};

#define LENS_PARAMS_NUM (7U)
static PARAM_s CT_AvmLensParams[LENS_PARAMS_NUM] = {
    {0, "disto_type", U_8, 1U, NULL, 0U},
    {1, "real_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {2, "expect_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {3, "expect_angle_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0U},
    {4, "table_length", U_32, 1U, NULL, 0U},
    {5, "real_expect_formula", S_DB, 5U, NULL, 0U},
    {6, "angle_formula", S_DB, 5U, NULL, 0U},
};

#define SENSOR_PARAMS_NUM (5U)
static PARAM_s CT_AvmSensorParams[SENSOR_PARAMS_NUM] = {
    {0, "cell_size", S_DB, 1U, NULL, 0U},
    {1, "start_x", U_32, 1U, NULL, 0U},
    {2, "start_y", U_32, 1U, NULL, 0U},
    {3, "width", U_32, 1U, NULL, 0U},
    {4, "height", U_32, 1U, NULL, 0U},
};

#define OPTICAL_CENTER_PARAMS_NUM (2U)
static PARAM_s CT_AvmOpticalCenterParams[OPTICAL_CENTER_PARAMS_NUM] = {
    {0, "x", S_DB, 1U, NULL, 0U},
    {1, "y", S_DB, 1U, NULL, 0U},
};

#define CAMERA_PARAMS_NUM (5U)
static PARAM_s CT_AvmCameraParams[CAMERA_PARAMS_NUM] = {
    {0, "pos_x", S_DB, 1U, NULL, 0U},
    {1, "pos_y", S_DB, 1U, NULL, 0U},
    {2, "pos_z", S_DB, 1U, NULL, 0U},
    {3, "rotate_type", U_8, 1U, NULL, 0U},
    {4, "focal_length", S_DB, 1U, NULL, 0U},
};

#define VIN_PARAMS_NUM (8U)
static PARAM_s CT_AvmVinParams[VIN_PARAMS_NUM] = {
    {0, "start_x", U_32, 1U, NULL, 0U},
    {1, "start_y", U_32, 1U, NULL, 0U},
    {2, "width", U_32, 1U, NULL, 0U},
    {3, "height", U_32, 1U, NULL, 0U},
    {4, "h_subsample_factor_num", U_8, 1U, NULL, 0U},
    {5, "h_subsample_factor_den", U_8, 1U, NULL, 0U},
    {6, "v_subsample_factor_num", U_8, 1U, NULL, 0U},
    {7, "v_subsample_factor_den", U_8, 1U, NULL, 0U},
};

#define CALIB_POINTS_PARAMS_NUM (5U)
static PARAM_s CT_AvmCalibPointsParams[CALIB_POINTS_PARAMS_NUM] = {
    {0, "world_pos_x", S_DB, AVM_CALIB_POINT_NUM, NULL, 0U},
    {1, "world_pos_y", S_DB, AVM_CALIB_POINT_NUM, NULL, 0U},
    {2, "world_pos_z", S_DB, AVM_CALIB_POINT_NUM, NULL, 0U},
    {3, "raw_pos_x", S_DB, AVM_CALIB_POINT_NUM, NULL, 0U},
    {4, "raw_pos_y", S_DB, AVM_CALIB_POINT_NUM, NULL, 0U},
};

#define ASSISTANCE_POINTS_PARAMS_NUM (6U)
static PARAM_s CT_AvmAssistancePointsParams[ASSISTANCE_POINTS_PARAMS_NUM] = {
    {0, "number", U_32, 1U, NULL, 0U},
    {1, "world_pos_x", S_DB, MAX_AVM_ASSISTANCE_POINT_NUM, NULL, 0U},
    {2, "world_pos_y", S_DB, MAX_AVM_ASSISTANCE_POINT_NUM, NULL, 0U},
    {3, "world_pos_z", S_DB, MAX_AVM_ASSISTANCE_POINT_NUM, NULL, 0U},
    {4, "raw_pos_x", S_DB, MAX_AVM_ASSISTANCE_POINT_NUM, NULL, 0U},
    {5, "raw_pos_y", S_DB, MAX_AVM_ASSISTANCE_POINT_NUM, NULL, 0U},
};

#define ROI_PARAMS_NUM (4U)
static PARAM_s CT_AvmRoiParams[ROI_PARAMS_NUM] = {
    {0, "start_x", U_32, 1U, NULL, 0U},
    {1, "start_y", U_32, 1U, NULL, 0U},
    {2, "width", U_32, 1U, NULL, 0U},
    {3, "height", U_32, 1U, NULL, 0U},
};

#define MAIN_PARAMS_NUM (2U)
static PARAM_s CT_AvmMainParams[MAIN_PARAMS_NUM] = {
    {0, "width", U_32, 1U, NULL, 0U},
    {1, "height", U_32, 1U, NULL, 0U},
};

#define MIN_VOUT_PARAMS_NUM (2U)
static PARAM_s CT_AvmMinVoutParams[MIN_VOUT_PARAMS_NUM] = {
    {0, "width", U_32, 1U, NULL, 0U},
    {1, "height", U_32, 1U, NULL, 0U},
};

#define MIN_ROI_SIZE_PARAMS_NUM (2U)
static PARAM_s CT_AvmMinRoiSizeParams[MIN_ROI_SIZE_PARAMS_NUM] = {
    {0, "width", U_32, 1U, NULL, 0U},
    {1, "height", U_32, 1U, NULL, 0U},
};

#define TILE_SIZE_PARAMS_NUM (2U)
static PARAM_s CT_AvmTileSizeParams[TILE_SIZE_PARAMS_NUM] = {
    {0, "width", U_32, 1U, NULL, 0U},
    {1, "height", U_32, 1U, NULL, 0U},
};

#define AUTO_PARAMS_NUM (2U)
static PARAM_s CT_AvmAutoParams[AUTO_PARAMS_NUM] = {
    {0, "roi_enable", U_32, 1U, NULL, 0U},
    {1, "front_end_rotation_enable", U_32, 1U, NULL, 0U},
};

#define OPTIMIZE_PARAMS_NUM (1U)
static PARAM_s CT_AvmOptimizeParams[OPTIMIZE_PARAMS_NUM] = {
    {0, "level", U_8, 1U, NULL, 0U},
};

#define PRECHECK_PARAMS_NUM (2U)
static PARAM_s CT_AvmPreCheckParams[PRECHECK_PARAMS_NUM] = {
    {0, "number", U_32, 1U, NULL, 0U},
    {1, "data_path", STRING, 1U, NULL, 0U},
};

#define CALIB_PARAMS_NUM (3U)
static PARAM_s CT_AvmCalibParams[CALIB_PARAMS_NUM] = {
    {0, "enable_ransac", U_32, 1U, NULL, 0U},
    {1, "iteration_num", U_32, 1U, NULL, 0U},
    {2, "allowed_reproj_err", S_DB, 1U, NULL, 0U},
};

static void CT_AvmSystemProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_NUM];
    AMBA_CT_AVM_SYSTEM_s Data;
    AmbaCT_AvmGetSystem(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.Type;
    pField[2] = Data.ChipRev;
    pField[3] = Data.OutputPrefix;
    pField[4] = &Data.EnableSavePrecheck;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetSystem(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        *pParamStatus |= (0x1ULL << 4);
    }
}

static void CT_AvmCarProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[CAR_PARAMS_NUM];
    AMBA_CAL_SIZE_s Data;
    AmbaCT_AvmGetCar(&Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetCar(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_Avm2dViewProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[VIEW_2D_PARAMS_NUM];
    AMBA_CT_AVM_2D_VIEW_s Data;
    AmbaCT_AvmGet2dViewRange(&Data);
    pField[0] = &Data.CropRange.StartX;
    pField[1] = &Data.CropRange.StartY;
    pField[2] = &Data.CropRange.Width;
    pField[3] = &Data.CropRange.Height;
    pField[4] = &Data.DisplayHeight;
    pField[5] = &Data.AutoVoutOrderEnable;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSet2dViewRange(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_Avm2dCustomCalibProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[CUSTOMER_2D_CALIB_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / CUSTOMER_2D_CALIB_PARAMS_NUM;
    AMBA_CT_AVM_2D_CUSTOM_CALIB_s Data;
    AmbaCT_AvmGet2dCustomCalib(ArrayIndex, &Data);
    pField[0] = &Data.Enable;
    pField[1] = Data.CalibMatrix;
    CT_ProcParam(pParam, pField[(UINT32)Index % CUSTOMER_2D_CALIB_PARAMS_NUM]);
    AmbaCT_AvmSet2dCustomCalib(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmViewBlendProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[(VIEW_BLEND_PARAMS_NUM * (UINT32)AMBA_CAL_AVM_BlEND_CORNER_MAX)];
    AMBA_CAL_AVM_BLEND_CFG_V2_s Data;
    AmbaCT_AvmGetViewBlendV2(&Data);
    pField[0] = &Data.Angle[0];
    pField[1] = &Data.Width[0];
    pField[2] = &Data.ReferenceCarVoutPosition[0];
    pField[3] = &Data.SectorAngle[0];
    pField[4] = &Data.Angle[1];
    pField[5] = &Data.Width[1];
    pField[6] = &Data.ReferenceCarVoutPosition[1];
    pField[7] = &Data.SectorAngle[1];
    pField[8] = &Data.Angle[2];
    pField[9] = &Data.Width[2];
    pField[10] = &Data.ReferenceCarVoutPosition[2];
    pField[11] = &Data.SectorAngle[2];
    pField[12] = &Data.Angle[3];
    pField[13] = &Data.Width[3];
    pField[14] = &Data.ReferenceCarVoutPosition[3];
    pField[15] = &Data.SectorAngle[3];
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetViewBlendV2(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        //Note # ReferenceCarVoutPosition is optional
        *pParamStatus |= (0x1ULL << 2);
        *pParamStatus |= (0x1ULL << 6);
        *pParamStatus |= (0x1ULL << 10);
        *pParamStatus |= (0x1ULL << 14);
        //Note # SectorAngle is optional
        *pParamStatus |= (0x1ULL << 3);
        *pParamStatus |= (0x1ULL << 7);
        *pParamStatus |= (0x1ULL << 11);
        *pParamStatus |= (0x1ULL << 15);
    }
}

static void CT_Avm3dViewModelTypeProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[VIEW_3D_MODEL_TYPE_NUM];
    AMBA_CT_AVM_3D_VIEW_MODEL_TYPE_s Data;
    AmbaCT_AvmGet3dViewModelType(&Data);
    pField[0] = Data.ModelType;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSet3dViewModelType(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_Avm3dViewProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[VIEW_3D_PARAMS_NUM];
    AMBA_CT_AVM_3D_VIEW_s Data;
    AmbaCT_AvmGet3dView(&Data);
    pField[0] = &Data.FloorRangeX;
    pField[1] = &Data.FloorRangeY;
    pField[2] = &Data.CenterPositionX;
    pField[3] = &Data.CenterPositionY;
    pField[4] = &Data.Distance;
    pField[5] = &Data.Height;
    pField[6] = &Data.DisplayWidth;
    pField[7] = &Data.DisplayHeight;
    pField[8] = &Data.DisplayRotation;
    pField[9] = &Data.AutoVoutOrderEnable;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSet3dView(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);

        //Note # Display rotation is optional
        *pParamStatus |= (0x1ULL << 8);
    }
}

static void CT_Avm3dAdvViewProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[ADVANCED_3D_VIEW_PARAMS_NUM];
    AMBA_CT_AVM_3D_ADV_VIEW_s Data;
    AmbaCT_AvmGet3dAdvView(&Data);
    pField[0] = &Data.TopFrontRange;
    pField[1] = &Data.TopBackRange;
    pField[2] = &Data.TopLeftRange;
    pField[3] = &Data.TopRightRange;
    pField[4] = &Data.TopFrontLeftRadiusX;
    pField[5] = &Data.TopFrontLeftRadiusY;
    pField[6] = &Data.TopFrontRightRadiusX;
    pField[7] = &Data.TopFrontRightRadiusY;
    pField[8] = &Data.TopBackLeftRadiusX;
    pField[9] = &Data.TopBackLeftRadiusY;
    pField[10] = &Data.TopBackRightRadiusX;
    pField[11] = &Data.TopBackRightRadiusY;
    pField[12] = &Data.TopHeight;
    pField[13] = &Data.BottomFrontRange;
    pField[14] = &Data.BottomBackRange;
    pField[15] = &Data.BottomLeftRange;
    pField[16] = &Data.BottomRightRange;
    pField[17] = &Data.BottomFrontLeftRadiusX;
    pField[18] = &Data.BottomFrontLeftRadiusY;
    pField[19] = &Data.BottomFrontRightRadiusX;
    pField[20] = &Data.BottomFrontRightRadiusY;
    pField[21] = &Data.BottomBackLeftRadiusX;
    pField[22] = &Data.BottomBackLeftRadiusY;
    pField[23] = &Data.BottomBackRightRadiusX;
    pField[24] = &Data.BottomBackRightRadiusY;
    pField[25] = &Data.TopToBottomRadiusH;
    pField[26] = &Data.TopToBottomRadiusV;
    pField[27] = &Data.BottomCornerIntervalNum;
    pField[28] = &Data.PillarIntervalNum;
    pField[29] = &Data.CenterPositionX;
    pField[30] = &Data.CenterPositionY;
    pField[31] = &Data.RotationAngle;
    pField[32] = &Data.DisplayWidth;
    pField[33] = &Data.DisplayHeight;
    pField[34] = &Data.DisplayRotation;
    pField[35] = &Data.AutoVoutOrderEnable;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSet3dAdvView(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        //Note # Model rotation is optional
        *pParamStatus |= (0x1ULL << 31);
        //Note # Display rotation is optional
        *pParamStatus |= (0x1ULL << 34);
    }
}

static void CT_Avm3dWarpProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[WARP_3D_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / WARP_3D_PARAMS_NUM;
    AMBA_CAL_AVM_3D_WARP_CAL_TYPE_e Data;
    AmbaCT_AvmGet3dWarp(ArrayIndex, &Data);
    pField[0] = &Data;
    CT_ProcParam(pParam, pField[(UINT32)Index % WARP_3D_PARAMS_NUM]);
    AmbaCT_AvmSet3dWarp(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}
static void CT_AvmMainViewProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[MAIN_PARAMS_NUM];
    UINT8 Data;

    pField[0] = &Data;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetMainView(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmMainViewPerspectiveCfgProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[MAIN_VIEW_PERSPECTIBVE_CFG_PARAMS_NUM];
    AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s Data;
    AmbaCT_AvmGetMVPerspectiveV1Cfg(&Data);
    pField[0] = &Data.StartDegreesX;
    pField[1] = &Data.StartDegreesY;
    pField[2] = &Data.TotalViewAngleX;
    pField[3] = &Data.TotalViewAngleY;
    pField[4] = &Data.DisplayWidth;
    pField[5] = &Data.DisplayHeight;
    pField[6] = &Data.ProjectionRotationDegreeH;
    pField[7] = &Data.ProjectionRotationDegreeV;
    pField[8] = &Data.ProjectionRotationDegreeF;
    pField[9] = Data.LdeRealExpectUpper.pRealTbl;
    pField[10] = Data.LdeRealExpectUpper.pExpectTbl;
    pField[11] = &Data.LdeRealExpectUpper.Length;
    pField[12] = Data.LdeRealExpectLower.pRealTbl;
    pField[13] = Data.LdeRealExpectLower.pExpectTbl;
    pField[14] = &Data.LdeRealExpectLower.Length;
    pField[15] = Data.LdeRealExpectLeft.pRealTbl;
    pField[16] = Data.LdeRealExpectLeft.pExpectTbl;
    pField[17] = &Data.LdeRealExpectLeft.Length;
    pField[18] = Data.LdeRealExpectRight.pRealTbl;
    pField[19] = Data.LdeRealExpectRight.pExpectTbl;
    pField[20] = &Data.LdeRealExpectRight.Length;
    pField[21] = &Data.LdeOpticalCenter.X;
    pField[22] = &Data.LdeOpticalCenter.Y;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetMVPerspectiveV1Cfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);

        //Note # TotalViewAngleY is optional
        *pParamStatus |= (0x1ULL << 3);

        //Note # Rotation are optional
        *pParamStatus |= (0x1ULL << 6);
        *pParamStatus |= (0x1ULL << 7);
        *pParamStatus |= (0x1ULL << 8);

         //Note # Real/Expect table is optional
        *pParamStatus |= (0x1ULL << 9);
        *pParamStatus |= (0x1ULL << 10);
        *pParamStatus |= (0x1ULL << 11);
        *pParamStatus |= (0x1ULL << 12);
        *pParamStatus |= (0x1ULL << 13);
        *pParamStatus |= (0x1ULL << 14);
        *pParamStatus |= (0x1ULL << 15);
        *pParamStatus |= (0x1ULL << 16);
        *pParamStatus |= (0x1ULL << 17);
        *pParamStatus |= (0x1ULL << 18);
        *pParamStatus |= (0x1ULL << 19);
        *pParamStatus |= (0x1ULL << 20);
        *pParamStatus |= (0x1ULL << 21);
        *pParamStatus |= (0x1ULL << 22);
    }
}

static void CT_AvmMainViewEqDistanceCfgProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[MAIN_VIEW_EQUALDISTANCE_CFG_PARAMS_NUM];
    AMBA_CAL_AVM_MV_EQ_DISTANCE_s Data;
    AmbaCT_AvmGetMVEqualDistCfg(&Data);
    pField[0] = &Data.StartDegreesX;
    pField[1] = &Data.StartDegreesY;
    pField[2] = &Data.TotalViewAngleX;
    pField[3] = &Data.TotalViewAngleY;
    pField[4] = &Data.DisplayWidth;
    pField[5] = &Data.DisplayHeight;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetMVEqualDistCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);

        //Note # TotalViewAngleY is optional
        *pParamStatus |= (0x1ULL << 6);
    }
}

static void CT_AvmMainViewLdcCfgProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[MAIN_VIEW_LDC_CFG_PARAMS_NUM];
    AMBA_CAL_AVM_MV_LDC_CFG_s Data;
    AmbaCT_AvmGetMVLdcCfg(&Data);
    pField[0] = &Data.CompensateRatio;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetMVLdcCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmMainViewDirectionProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[MAIN_VIEW_DIRECTION_PARAMS_NUM];
    AMBA_CAL_POINT_DB_3D_s Data;
    AmbaCT_AvmGetMVDirection(&Data);
    pField[0] = &Data.X;
    pField[1] = &Data.Y;
    pField[2] = &Data.Z;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetMVDirection(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_Avm3dVirtualCamProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[VIRTUAL_3D_CAM_PARAMS_NUM];
    AMBA_CAL_VIRTUAL_CAM_s Data;
    AmbaCT_AvmGet3dVirtualCam(&Data);
    pField[0] = &Data.Focus.X;
    pField[1] = &Data.Focus.Y;
    pField[2] = &Data.Focus.Z;
    pField[3] = &Data.ProjectPlane.X;
    pField[4] = &Data.ProjectPlane.Y;
    pField[5] = &Data.ProjectPlane.Z;
    pField[6] = &Data.ProjectPlaneShift.X;
    pField[7] = &Data.ProjectPlaneShift.Y;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSet3dVirtualCam(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);

        //Note # Virtual plane shift is optional
        *pParamStatus |= (0x1ULL << 6);
        *pParamStatus |= (0x1ULL << 7);
    }
}

static void CT_AvmOsdMaskProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[OSD_MASK_PARAMS_NUM];
    AMBA_CT_AVM_OSD_MASK_s *pData = NULL;
    AmbaCT_AvmGetOsdMask(&pData);
    if (pData != NULL) {
        pField[0] = &pData->StartX;
        pField[1] = &pData->StartY;
        pField[2] = &pData->Width;
        pField[3] = &pData->Height;
        pField[4] = pData->TablePath;
        pField[5] = &pData->FromFile;
        CT_ProcParam(pParam, pField[(UINT32)Index % OSD_MASK_PARAMS_NUM]);
        AmbaCT_AvmSetOsdMask(pData);
        if (pParamStatus != NULL) {
            *pParamStatus |= (0x1ULL << (UINT32)Index);
        }
    }
}

static void CT_AvmLensProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[LENS_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / LENS_PARAMS_NUM;
    AMBA_CT_AVM_LENS_s Data;
    AmbaCT_AvmGetLens(ArrayIndex, &Data);
    pField[0] = &Data.LensDistoType;
    pField[1] = Data.pRealTable;
    pField[2] = Data.pExceptTable;
    pField[3] = Data.pExceptAngleTable;
    pField[4] = &Data.TableLen;
    pField[5] = Data.RealExpectFormula;
    pField[6] = Data.AngleFormula;
    CT_ProcParam(pParam, pField[(UINT32)Index % LENS_PARAMS_NUM]);
    AmbaCT_AvmSetLens(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        if (((UINT32)Index % LENS_PARAMS_NUM) == 0U) {
            if (Data.LensDistoType == AMBA_CAL_LD_REAL_EXPECT_TBL) {
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 3U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 5U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 6U);
            } else if (Data.LensDistoType == AMBA_CAL_LD_ANGLE_TBL) {
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 2U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 5U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 6U);
            } else if (Data.LensDistoType == AMBA_CAL_LD_REAL_EXPECT_FORMULA) {
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 1U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 2U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 3U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 4U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 6U);
            } else if (Data.LensDistoType == AMBA_CAL_LD_ANGLE_FORMULA) {
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 1U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 2U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 3U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 4U);
                *pParamStatus |= 0x1ULL << ((ArrayIndex * LENS_PARAMS_NUM) + 5U);
            } else {
                //Note # do nothing
            }
        }
    }
}

static void CT_AvmSensorProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[SENSOR_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / SENSOR_PARAMS_NUM;
    AMBA_CAL_SENSOR_s Data;
    AmbaCT_AvmGetSensor(ArrayIndex, &Data);
    pField[0] = &Data.CellSize;
    pField[1] = &Data.StartX;
    pField[2] = &Data.StartY;
    pField[3] = &Data.Width;
    pField[4] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32)Index % SENSOR_PARAMS_NUM]);
    AmbaCT_AvmSetSensor(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OpticalCenterProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[OPTICAL_CENTER_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / OPTICAL_CENTER_PARAMS_NUM;
    AMBA_CAL_POINT_DB_2D_s Data;
    AmbaCT_AvmGetOpticalCenter(ArrayIndex, &Data);
    pField[0] = &Data.X;
    pField[1] = &Data.Y;
    CT_ProcParam(pParam, pField[(UINT32)Index % OPTICAL_CENTER_PARAMS_NUM]);
    AmbaCT_AvmSetOpticalCenter(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmCameraProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[CAMERA_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / CAMERA_PARAMS_NUM;
    AMBA_CT_AVM_CAMERA_s Data;
    AmbaCT_AvmGetCamera(ArrayIndex, &Data);
    pField[0] = &Data.PositionX;
    pField[1] = &Data.PositionY;
    pField[2] = &Data.PositionZ;
    pField[3] = &Data.RotateType;
    pField[4] = &Data.FocalLength;
    CT_ProcParam(pParam, pField[(UINT32)Index % CAMERA_PARAMS_NUM]);
    AmbaCT_AvmSetCamera(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        //Note # FocalLength is an optional parameter
        *pParamStatus |= (0x1ULL << (4U + (ArrayIndex * CAMERA_PARAMS_NUM)));
    }
}

static void CT_AvmVinProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[VIN_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / VIN_PARAMS_NUM;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s Data;
    AmbaCT_AvmGetVin(ArrayIndex, &Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    pField[4] = &Data.HSubSample.FactorNum;
    pField[5] = &Data.HSubSample.FactorDen;
    pField[6] = &Data.VSubSample.FactorNum;
    pField[7] = &Data.VSubSample.FactorDen;
    CT_ProcParam(pParam, pField[(UINT32)Index % VIN_PARAMS_NUM]);
    AmbaCT_AvmSetVin(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmCalibPointsProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[CALIB_POINTS_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / CALIB_POINTS_PARAMS_NUM;
    AMBA_CT_AVM_CALIB_POINTS_s Data;
    AmbaCT_AvmGetCalibPoints(ArrayIndex, &Data);
    pField[0] = &Data.WorldPositionX[0];
    pField[1] = &Data.WorldPositionY[0];
    pField[2] = &Data.WorldPositionZ[0];
    pField[3] = &Data.RawPositionX[0];
    pField[4] = &Data.RawPositionY[0];
    CT_ProcParam(pParam, pField[(UINT32)Index % CALIB_POINTS_PARAMS_NUM]);
    AmbaCT_AvmSetCalibPoints(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmAssistancePointsProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[ASSISTANCE_POINTS_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / ASSISTANCE_POINTS_PARAMS_NUM;
    AMBA_CT_AVM_ASSISTANCE_POINTS_s Data;
    AmbaCT_AvmGetAssistancePoints(ArrayIndex, &Data);
    pField[0] = &Data.Number;
    pField[1] = &Data.WorldPositionX[0];
    pField[2] = &Data.WorldPositionY[0];
    pField[3] = &Data.WorldPositionZ[0];
    pField[4] = &Data.RawPositionX[0];
    pField[5] = &Data.RawPositionY[0];
    CT_ProcParam(pParam, pField[(UINT32)Index % ASSISTANCE_POINTS_PARAMS_NUM]);
    AmbaCT_AvmSetAssistancePoints(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        if ((((UINT32)Index % ASSISTANCE_POINTS_PARAMS_NUM) == 0U) && (Data.Number == 0U)) {
            *pParamStatus |= (0x1ULL << (1U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
            *pParamStatus |= (0x1ULL << (2U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
            *pParamStatus |= (0x1ULL << (3U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
            *pParamStatus |= (0x1ULL << (4U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
            *pParamStatus |= (0x1ULL << (5U + (ArrayIndex * ASSISTANCE_POINTS_PARAMS_NUM)));
        }
    }
}

static void CT_AvmRoiProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[ROI_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / ROI_PARAMS_NUM;
    AMBA_CAL_ROI_s Data;
    AmbaCT_AvmGetRoi(ArrayIndex, &Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32)Index % ROI_PARAMS_NUM]);
    AmbaCT_AvmSetRoi(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmMainProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[MAIN_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / MAIN_PARAMS_NUM;
    AMBA_CAL_SIZE_s Data;
    AmbaCT_AvmGetMainSize(ArrayIndex, &Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32)Index % MAIN_PARAMS_NUM]);
    AmbaCT_AvmSetMainSize(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmMinVoutProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[MIN_VOUT_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / MIN_VOUT_PARAMS_NUM;
    AMBA_CAL_SIZE_s Data;
    AmbaCT_AvmGetMinVoutSize(ArrayIndex, &Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32)Index % MIN_VOUT_PARAMS_NUM]);
    AmbaCT_AvmSetMinVoutSize(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmMinRoiSizeProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[MIN_ROI_SIZE_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / MIN_ROI_SIZE_PARAMS_NUM;
    AMBA_CAL_SIZE_s Data;
    AmbaCT_AvmGetMinRoiSize(ArrayIndex, &Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32)Index % MIN_ROI_SIZE_PARAMS_NUM]);
    AmbaCT_AvmSetMinRoiSize(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmTileSizeProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[TILE_SIZE_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / TILE_SIZE_PARAMS_NUM;
    AMBA_CAL_SIZE_s Data;
    AmbaCT_AvmGetTileSize(ArrayIndex, &Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[(UINT32)Index % TILE_SIZE_PARAMS_NUM]);
    AmbaCT_AvmSetTileSize(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmAutoProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[AUTO_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / AUTO_PARAMS_NUM;
    AMBA_CT_AVM_AUTO_s Data;
    AmbaCT_AvmGetAuto(ArrayIndex, &Data);
    pField[0] = &Data.RoiEnable;
    pField[1] = &Data.FrontEndRotationEnable;
    CT_ProcParam(pParam, pField[(UINT32)Index % AUTO_PARAMS_NUM]);
    AmbaCT_AvmSetAuto(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmOptimizeProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[OPTIMIZE_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / OPTIMIZE_PARAMS_NUM;
    UINT8 Data;
    AmbaCT_AvmGetOptimize(ArrayIndex, &Data);
    pField[0] = &Data;
    CT_ProcParam(pParam, pField[(UINT32)Index % OPTIMIZE_PARAMS_NUM]);
    AmbaCT_AvmSetOptimize(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmInternalProc(INT32 Index, UINT64 *pParamStatus,const  PARAM_s *pParam)
{
    void *pField[INTERNAL_PARAMS_NUM];
    UINT32 ArrayIndex = (UINT32)Index / INTERNAL_PARAMS_NUM;
    AMBA_CAL_AVM_INTERNAL_CFG_s Data;
    AmbaCT_AvmGetInternal(ArrayIndex, &Data);
    pField[0] = &Data.DebugID;
    pField[1] = &Data.WaitlineTh;
    pField[2] = &Data.DmaTh;
    pField[3] = &Data.RefineDmaEnable;
    pField[4] = &Data.RefineWaitlineEnable;
    pField[5] = &Data.RefineOutOfRoiEnable;
    pField[6] = &Data.RefineVFlipEnable;
    pField[7] = &Data.ExtrapolatePlaneBoundary;
    pField[8] = &Data.RepeatUnusedArea;
    CT_ProcParam(pParam, pField[(UINT32)Index % INTERNAL_PARAMS_NUM]);
    AmbaCT_AvmSetInternal(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmPreCheckDataProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[PRECHECK_PARAMS_NUM];
    AMBA_CT_AVM_PRE_CHECK_s Data;
    AmbaCT_AvmGetPreCheckData(&Data);
    pField[0] = &Data.Number;
    pField[1] = Data.InputFile;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetPreCheckData(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_AvmCalibDataProc(INT32 Index, UINT64 *pParamStatus,const PARAM_s *pParam)
{
    void *pField[CALIB_PARAMS_NUM];
    AMBA_CT_AVM_CALIB_s Data;
    AmbaCT_AvmGetCalib(&Data);
    pField[0] = &Data.EnableRansac;
    pField[1] = &Data.IterationNum;
    pField[2] = &Data.AllowedReprojErr;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_AvmSetCalib(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

#define AMBA_CAL_AVM_CAM_NUM_MAX ((INT32)AMBA_CAL_AVM_CAM_MAX)
#define AVM_REGS_NUM (33U)
static REG_s AvmRegs[AVM_REGS_NUM] = {
    {(UINT32)AMBA_CT_AVM_SYSTEM, "system", 1, 0, (INT32) SYSTEM_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmSystemParams, CT_AvmSystemProc, 0U},
    {(UINT32)AMBA_CT_AVM_CAR, "car", 1, 0, (INT32) CAR_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmCarParams, CT_AvmCarProc, 0U},
    {(UINT32)AMBA_CT_AVM_2D_VIEW, "2d_view", 1, 0, (INT32) VIEW_2D_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_Avm2dViewParams, CT_Avm2dViewProc, 0U},
    {(UINT32)AMBA_CT_AVM_2D_CUSTOM_CALIB, "2d_custom_calib", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) CUSTOMER_2D_CALIB_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_Avm2dCustomCalibParams, CT_Avm2dCustomCalibProc, 0U},
    {(UINT32)AMBA_CT_AVM_3D_VIEW_MODEL_TYPE, "3d_view_model_type", 1, 0, (INT32) VIEW_3D_MODEL_TYPE_NUM, PARAM_STATUS_DEFAULT, CT_Avm3dViewModelType, CT_Avm3dViewModelTypeProc, 0U},
    {(UINT32)AMBA_CT_AVM_3D_VIEW, "3d_view", 1, 0, (INT32) VIEW_3D_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_Avm3dViewParams, CT_Avm3dViewProc, 0U},
    {(UINT32)AMBA_CT_AVM_3D_ADV_VIEW, "3d_adv_view", 1, 0, (INT32) ADVANCED_3D_VIEW_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_Avm3dAdvancedViewParams, CT_Avm3dAdvViewProc, 0U},
    {(UINT32)AMBA_CT_AVM_3D_VITRUAL_CAM, "3d_vitrual_camera", 1, 0, (INT32) VIRTUAL_3D_CAM_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_Avm3dVirtualCamParams, CT_Avm3dVirtualCamProc, 0U},
    {(UINT32)AMBA_CT_AVM_3D_WARP, "3d_warp", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) WARP_3D_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_Avm3dWarpParams, CT_Avm3dWarpProc, 0U},
    {(UINT32)AMBA_CT_AVM_MAIN_VIEW, "main_view", 1, 0, (INT32) MAIN_VIEW_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmMainViewParams, CT_AvmMainViewProc, 0U},
    {(UINT32)AMBA_CT_AVM_MAIN_VIEW_PERSPECTIVE, "main_view_perspective_cfg", 1, 0, (INT32) MAIN_VIEW_PERSPECTIBVE_CFG_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmMainViewPerspectiveCfgParams, CT_AvmMainViewPerspectiveCfgProc, 0U},
    {(UINT32)AMBA_CT_AVM_MAIN_VIEW_EQUAL_DIST, "main_view_equal_distance_cfg", 1, 0, (INT32) MAIN_VIEW_EQUALDISTANCE_CFG_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmMainViewEqualDistanceCfgParams, CT_AvmMainViewEqDistanceCfgProc, 0U},
    {(UINT32)AMBA_CT_AVM_MAIN_VIEW_LDC, "main_view_ldc_cfg", 1, 0, (INT32) MAIN_VIEW_LDC_CFG_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmMainViewLdcCfgParams, CT_AvmMainViewLdcCfgProc, 0U},
    {(UINT32)AMBA_CT_AVM_MAIN_VIEW_DIRECTION, "direction", 1, 0, (INT32) MAIN_VIEW_DIRECTION_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmMainViewDirectionParams, CT_AvmMainViewDirectionProc, 0U},
    {(UINT32)AMBA_CT_AVM_VIEW_BLEND, "view_blend", (INT32)AMBA_CAL_AVM_BlEND_CORNER_MAX, 0, (INT32) VIEW_BLEND_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmViewBlendParams, CT_AvmViewBlendProc, 0U},
    {(UINT32)AMBA_CT_AVM_OSD_MASK, "osd_mask", 1, 0, (INT32) OSD_MASK_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmOsdMaskParams, CT_AvmOsdMaskProc, 0U},
    {(UINT32)AMBA_CT_AVM_LENS, "lens", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) LENS_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmLensParams, CT_AvmLensProc, 0U},
    {(UINT32)AMBA_CT_AVM_SENSOR, "sensor", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) SENSOR_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmSensorParams, CT_AvmSensorProc, 0U},
    {(UINT32)AMBA_CT_AVM_OPTICAL_CENTER, "optical_center", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) OPTICAL_CENTER_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmOpticalCenterParams, CT_OpticalCenterProc, 0U},
    {(UINT32)AMBA_CT_AVM_CAMERA, "camera", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) CAMERA_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmCameraParams, CT_AvmCameraProc, 0U},
    {(UINT32)AMBA_CT_AVM_VIN, "vin", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) VIN_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmVinParams, CT_AvmVinProc, 0U},
    {(UINT32)AMBA_CT_AVM_CALIB_POINTS, "calib_points", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) CALIB_POINTS_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmCalibPointsParams, CT_AvmCalibPointsProc, 0U},
    {(UINT32)AMBA_CT_AVM_ASSISTANCE_POINTS, "assistance_points", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) ASSISTANCE_POINTS_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmAssistancePointsParams, CT_AvmAssistancePointsProc, 0U},
    {(UINT32)AMBA_CT_AVM_ROI, "roi", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) ROI_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmRoiParams, CT_AvmRoiProc, 0U},
    {(UINT32)AMBA_CT_AVM_MAIN, "main", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) MAIN_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmMainParams, CT_AvmMainProc, 0U},
    {(UINT32)AMBA_CT_AVM_MAIN, "min_vout", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) MIN_VOUT_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmMinVoutParams, CT_AvmMinVoutProc, 0U},
    {(UINT32)AMBA_CT_AVM_MIN_ROI_SIZE, "min_roi", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) MIN_ROI_SIZE_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmMinRoiSizeParams, CT_AvmMinRoiSizeProc, 0U},
    {(UINT32)AMBA_CT_AVM_TILE_SIZE, "tile_size", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) TILE_SIZE_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmTileSizeParams, CT_AvmTileSizeProc, 0U},
    {(UINT32)AMBA_CT_AVM_AUTO, "auto", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) AUTO_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmAutoParams, CT_AvmAutoProc, 0U},
    {(UINT32)AMBA_CT_AVM_OPTIMIZE, "optimize", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) OPTIMIZE_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmOptimizeParams, CT_AvmOptimizeProc, 0U},
    {(UINT32)AMBA_CT_AVM_INTERNAL, "internal", AMBA_CAL_AVM_CAM_NUM_MAX, 0, (INT32) INTERNAL_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmInternalParams, CT_AvmInternalProc, ATTRIBUTE_HIDE},
    {(UINT32)AMBA_CT_AVM_PRECHECKDATA, "precheck", 1, 0, (INT32) PRECHECK_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmPreCheckParams, CT_AvmPreCheckDataProc, 0U},
    {(UINT32)AMBA_CT_AVM_CALIB, "calib", 1, 0, (INT32) CALIB_PARAMS_NUM, PARAM_STATUS_DEFAULT, CT_AvmCalibParams, CT_AvmCalibDataProc, 0U},
};

UINT32 AmbaCT_GetAvmRuleInfo(Rule_Info_t *pRuleInfo)
{
    pRuleInfo->RegList = &AvmRegs[0U];
    pRuleInfo->RegCount = (UINT8)AVM_REGS_NUM;
    return 0U;
}

UINT32 CT_GetAvmRuleInfoMisra(Rule_Info_t *pRuleInfo);
UINT32 CT_GetAvmRuleInfoMisra(Rule_Info_t *pRuleInfo)
{
    pRuleInfo->RegList = &AvmRegs[0];
    pRuleInfo->RegCount = (UINT8)AVM_REGS_NUM;
    return 0U;
}


