/**
 *  @file AmbaCT_OcCbRule.c
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
#include "AmbaCT_OcCbRule.h"
#include "AmbaCT_OcCbTuner.h"
#include "AmbaCalib_OcCbIF.h"
#include "AmbaTypes.h"
#include "AmbaCT_Logging.h"
#define SYSTEM_PARAMS_COUNT (4U)
static PARAM_s CT_OccbSystemParams[SYSTEM_PARAMS_COUNT] = {
    {0, "calib_mode", STRING, 1, NULL, 0},
    {1, "chip_rev", STRING, 1, NULL, 0},
    {2, "output_prefix", STRING, 1, NULL, 0},
    {3, "lens_type", STRING, 1, NULL, 0},
};

#define SRC_PARAMS_COUNT (3)
static PARAM_s CT_OccbSrcParams[SRC_PARAMS_COUNT] = {
    {0, "file_path", STRING, 1, NULL, 0},
    {1, "image_width", U_32, 1, NULL, 0},
    {2, "image_height", U_32, 1, NULL, 0},
};

#define PAT_DET_PARAMS_COUNT (2)
static PARAM_s CT_OccbPatDetParams[PAT_DET_PARAMS_COUNT] = {
    {0, "quality_level", S_DB, 1, NULL, 0},
    {1, "min_distance", U_32, 1, NULL, 0},
};

#define PAT_DET_PAT_TYPE_PARAMS_COUNT (1)
static PARAM_s CT_OccbPatDetPatTypeParams[PAT_DET_PAT_TYPE_PARAMS_COUNT] = {
    {0, "pattern_type", U_32, 1, NULL, 0},
};

#define PAT_DET_CIRCLE_PARAMS_COUNT (3)
static PARAM_s CT_OccbPatDetCircleParams[PAT_DET_CIRCLE_PARAMS_COUNT] = {
    {0, "min_perimeter", S_DB, 1, NULL, 0},
    {1, "max_perimeter", S_DB, 1, NULL, 0},
    {2, "min_distance", S_DB, 1, NULL, 0},
};

#define ORGANIZE_CFG_PARAMS_COUNT (4)
static PARAM_s CT_OccbOrganizeCfgParams[ORGANIZE_CFG_PARAMS_COUNT] = {
    {0, "estimate_tolerance", S_DB, 1, NULL, 0},
    {1, "center_search_radius", U_32, 1, NULL, 0},
    {2, "center_neighbors_search_radius", U_32, 1, NULL, 0},
    {3, "search_radius", U_32, 1, NULL, 0},
};

#define CAMERA_PARAMS_COUNT (4)
static PARAM_s CT_OccbCameraParams[CAMERA_PARAMS_COUNT] = {
    {0, "real_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {1, "expect_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {2, "table_length", U_32, 1, NULL, 0},
    {3, "cell_size", S_DB, 1, NULL, 0},
};

#define OPTICAL_CENTER_PARAMS_COUNT (2)
static PARAM_s CT_OccbOpticalCenterParams[OPTICAL_CENTER_PARAMS_COUNT] = {
    {0, "x", S_DB, 1, NULL, 0},
    {1, "y", S_DB, 1, NULL, 0},
};

#define SEARCH_RANGE_PARAMS_COUNT (2)
static PARAM_s CT_OccbSearchRangeParams[SEARCH_RANGE_PARAMS_COUNT] = {
    {0, "width", U_32, 1, NULL, 0},
    {1, "height", U_32, 1, NULL, 0},
};

#define WORLD_MAP_PARAMS_COUNT (6)
static PARAM_s CT_OccbWorld3DMapParams[WORLD_MAP_PARAMS_COUNT] = {
    {0, "output_3D_map_file", U_32, 1, NULL, 0},
    {1, "base_point_x", S_DB, 1, NULL, 0},
    {2, "base_point_y", S_DB, 1, NULL, 0},
    {3, "base_point_z", S_DB, 1, NULL, 0},
    {4, "horizontal_grid_space", S_DB, 1, NULL, 0},
    {5, "vertical_grid_space", S_DB, 1, NULL, 0},
};

#define OPTIMIZED_GRID_NUM_PARAMS_COUNT (4)
static PARAM_s CT_OccbOptimizedGridNumParams[OPTIMIZED_GRID_NUM_PARAMS_COUNT] = {
    {0, "width", U_32, 1, NULL, 0},
    {1, "height", U_32, 1, NULL, 0},
    {2, "min_width", U_32, 1, NULL, 0},
    {3, "min_height", U_32, 1, NULL, 0},
};

static void CT_OccbSystemProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_COUNT];
    AMBA_CT_OCCB_TUNER_SYSTEM_s Data;
    AmbaCT_OcCbGetSystemInfo(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.ChipRev;
    pField[2] = Data.OutputPrefix;
    pField[3] = Data.LensType;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetSystemInfo(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        // Note # LensType default NONFISHEYE
        *pParamStatus |= (0x1ULL << 3U);
    }
}

static void CT_OccbSrcProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SRC_PARAMS_COUNT];
    AMBA_CT_OCCB_TUNER_SRC_s Data;
    AmbaCT_OcCbGetSrcInfo(&Data);
    pField[0] = Data.FilePath;
    pField[1] = &Data.ImgSize.Width;
    pField[2] = &Data.ImgSize.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetSrcInfo(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OccbPatDetProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[PAT_DET_PARAMS_COUNT];
    AMBA_CT_OCCB_TUENR_PAT_DET_CHK_s Data;
    AmbaCT_OcCbGetPatDetCfg(&Data);
    pField[0] = &Data.QualityLevel;
    pField[1] = &Data.MinDistance;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetPatDetCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OccbPatDetTypeProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[PAT_DET_PAT_TYPE_PARAMS_COUNT];
    UINT32 PatternType;
    AmbaCT_OcCbGetPatTypeCfg(&PatternType);
    pField[0] = &PatternType;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetPatTypeCfg(&PatternType);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OccbPatDetCircleProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[PAT_DET_CIRCLE_PARAMS_COUNT];
    AMBA_CT_OCCB_TUENR_PAT_DET_CIR_s Data;
    AmbaCT_OcCbGetPatDetCircleCfg(&Data);
    pField[0] = &Data.MinPerimeter;
    pField[1] = &Data.MaxPerimeter;
    pField[2] = &Data.MinDistance;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetPatDetCircleCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OccbOrganizeCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[ORGANIZE_CFG_PARAMS_COUNT];
    AMBA_CAL_OCCB_ORGANIZE_CFG_s Data;
    AmbaCT_OcCbGetOrganizeCornerCfg(&Data);
    pField[0] = &Data.EstToleranceRate;
    pField[1] = &Data.CenterSearchRadius;
    pField[2] = &Data.CenterNeighborsSearchRadius;
    pField[3] = &Data.SearchRadius;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetOrganizeCornerCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OccbCameraProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[CAMERA_PARAMS_COUNT];
    AMBA_CT_OCCB_CAMERA_s Data;
    AmbaCT_OcCbGetCamera(&Data);
    pField[0] = Data.pRealTable;
    pField[1] = Data.pExceptTable;
    pField[2] = &Data.TableLen;
    pField[3] = &Data.CellSize;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetCamera(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OccbOpticalCenterProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OPTICAL_CENTER_PARAMS_COUNT];
    AMBA_CAL_POINT_DB_2D_s Data;
    AmbaCT_OcCbGetOpticalCenter(&Data);
    pField[0] = &Data.X;
    pField[1] = &Data.Y;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetOpticalCenter(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OccbSearchRangeProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SEARCH_RANGE_PARAMS_COUNT];
    AMBA_CAL_SIZE_s Data;
    AmbaCT_OcCbGetSearchRange(&Data);
    pField[0] = &Data.Width;
    pField[1] = &Data.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetSearchRange(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OccbWorldPtMapProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[WORLD_MAP_PARAMS_COUNT];
    AMBA_CT_OCCB_TUENR_World_Map_s Data;
    AmbaCT_OcCbGetWorldMapCfg(&Data);
    pField[0] = &Data.Output3DWorldMap;
    pField[1] = &Data.BasePtPos.X;
    pField[2] = &Data.BasePtPos.Y;
    pField[3] = &Data.BasePtPos.Z;
    pField[4] = &Data.HorizontalGridSpace;
    pField[5] = &Data.VerticalGridSpace;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetWorldMapCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_OccbOptimizedGridNumProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OPTIMIZED_GRID_NUM_PARAMS_COUNT];
    AMBA_CT_OCCB_USED_GRID_SIZE_s Data;
    AmbaCT_OcCbGetOptimizedGridNum(&Data);

    pField[0] = &Data.MaxSize.Width;
    pField[1] = &Data.MaxSize.Height;
    pField[2] = &Data.MinSize.Width;
    pField[3] = &Data.MinSize.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_OcCbSetOptimizedGridNum(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        *pParamStatus |= (0x1ULL << 2U);
        *pParamStatus |= (0x1ULL << 3U);
    }
}


#define OCCB_REGS_COUNT (11)
static REG_s OcCbRegs[OCCB_REGS_COUNT] = {
    {(INT32)AMBA_CT_OCCB_TUNER_SYSTEM_INFO, "system", 1, 0, (INT32) SYSTEM_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbSystemParams, CT_OccbSystemProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_SRC_INFO, "source", 1, 0, (INT32) SRC_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbSrcParams, CT_OccbSrcProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_PAT_TYPE, "pattern_detect_type", 1, 0, (INT32) PAT_DET_PAT_TYPE_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbPatDetPatTypeParams, CT_OccbPatDetTypeProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_PAT_DET, "pattern_detect_cfg", 1, 0, (INT32) PAT_DET_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbPatDetParams, CT_OccbPatDetProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_PAT_CIRCLE_DET, "pattern_detect_circle_cfg", 1, 0, (INT32) PAT_DET_CIRCLE_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbPatDetCircleParams, CT_OccbPatDetCircleProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_ORGANIZE_CORNER, "organize_corner_cfg", 1, 0, (INT32) ORGANIZE_CFG_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbOrganizeCfgParams, CT_OccbOrganizeCfgProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_CAMERA, "camera", 1, 0, (INT32)CAMERA_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbCameraParams, CT_OccbCameraProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_OPTICAL_CENTER, "ideal_optical_center", 1, 0, (INT32)OPTICAL_CENTER_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbOpticalCenterParams, CT_OccbOpticalCenterProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_SEARCH_RANGE, "search_range", 1, 0, (INT32)SEARCH_RANGE_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbSearchRangeParams, CT_OccbSearchRangeProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_WORLD_MAP, "world_map", 1, 0, (INT32)WORLD_MAP_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbWorld3DMapParams, CT_OccbWorldPtMapProc, 0},
    {(INT32)AMBA_CT_OCCB_TUNER_OPTIMIZED_GRID_NUM, "optimized_grid_num", 1, 0, (INT32)OPTIMIZED_GRID_NUM_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_OccbOptimizedGridNumParams, CT_OccbOptimizedGridNumProc, 0},
};

UINT32 AmbaCT_GetOcCbRuleInfo(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &OcCbRegs[0];
    RuleInfo->RegCount = OCCB_REGS_COUNT;
    return CT_OK;
}

UINT32 CT_GetOcCbRuleInfoMisra(Rule_Info_t *RuleInfo);
UINT32 CT_GetOcCbRuleInfoMisra(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &OcCbRegs[0];
    RuleInfo->RegCount = OCCB_REGS_COUNT;
    return CT_OK;
}

