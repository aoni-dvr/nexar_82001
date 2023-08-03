/**
 *  @file AmbaCT_LdccRule.c
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
#include "AmbaCT_LdccRule.h"
#include "AmbaCT_LdccTuner.h"
#include "AmbaTypes.h"
#include "AmbaCT_Logging.h"
#define SYSTEM_PARAMS_COUNT (4U)
static PARAM_s CT_LdccSystemParams[SYSTEM_PARAMS_COUNT] = {
    {0, "calib_mode", STRING, 1, NULL, 0},
    {1, "chip_rev", STRING, 1, NULL, 0},
    {2, "output_prefix", STRING, 1, NULL, 0},
    {3, "lens_type", STRING, 1, NULL, 0},
};

#define SRC_PARAMS_COUNT (7U)
static PARAM_s CT_LdccSrcParams[SRC_PARAMS_COUNT] = {
    {0, "single_plane_pattern_y_file_path", STRING, 1, NULL, 0},
    {1, "single_plane_pattern_uv_file_path", STRING, 1, NULL, 0},
    {2, "multi_plane_pattern_y_file_path", STRING, 1, NULL, 0},
    {3, "multi_plane_pattern_uv_file_path", STRING, 1, NULL, 0},
    {4, "is_yuv_420", U_32, 1, NULL, 0},
    {5, "image_width", U_32, 1, NULL, 0},
    {6, "image_height", U_32, 1, NULL, 0},
};

#define PAT_DET_CIRCLE_PARAMS_COUNT (3U)
static PARAM_s CT_LdccPatDetCircleParams[PAT_DET_CIRCLE_PARAMS_COUNT] = {
    {0, "min_perimeter", U_32, 1, NULL, 0},
    {1, "max_perimeter", U_32, 1, NULL, 0},
    {2, "min_distance", U_32, 1, NULL, 0},
};

#define PAT_DET_CENTER_ROI_PARAMS_COUNT (4U)
static PARAM_s CT_LdccPatDetCenterROIParams[PAT_DET_CENTER_ROI_PARAMS_COUNT] = {
    {0, "start_x", U_32, 1, NULL, 0},
    {1, "start_y", U_32, 1, NULL, 0},
    {2, "width", U_32, 1, NULL, 0},
    {3, "height", U_32, 1, NULL, 0},
};
#define ORGANIZE_CFG_PARAMS_COUNT (4U)
static PARAM_s CT_LdccOrganizeCfgParams[ORGANIZE_CFG_PARAMS_COUNT] = {
    {0, "estimate_tolerance", S_DB, 1, NULL, 0},
    {1, "center_search_radius", U_32, 1, NULL, 0},
    {2, "center_neighbors_search_radius", U_32, 1, NULL, 0},
    {3, "search_radius", U_32, 1, NULL, 0},
};

#define CAMERA_PARAMS_COUNT (6U)
static PARAM_s CT_LdccCameraParams[CAMERA_PARAMS_COUNT] = {
    {0, "real_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {1, "expect_table", S_DB, MAX_LENS_DISTO_TBL_LEN, NULL, 0},
    {2, "table_length", U_32, 1, NULL, 0},
    {3, "cell_size", S_DB, 1, NULL, 0},
    {4, "dist_table_unit", U_32, 1, NULL, 0},
    {5, "focallength_mm", S_DB, 1, NULL, 0},
};

#define OPTICAL_CENTER_PARAMS_COUNT (4U)
static PARAM_s CT_LdccOpticalCenterParams[OPTICAL_CENTER_PARAMS_COUNT] = {
    {0, "ideal_x", S_DB, 1, NULL, 0},
    {1, "ideal_y", S_DB, 1, NULL, 0},
    {2, "search_width", U_32, 1, NULL, 0},
    {3, "search_height", U_32, 1, NULL, 0},
};

#define MP_PAT_LAYOUT_CFG_PARAMS_COUNT (7U)
static PARAM_s CT_LdccMPPatLayoutCfgParams[MP_PAT_LAYOUT_CFG_PARAMS_COUNT] = {
    {0, "circle_info_path", STRING, 1, NULL, 0},
    {1, "circle_row_num", U_32, 1, NULL, 0},
    {2, "circle_col_num", U_32, 1, NULL, 0},
    {3, "center_to_center_distance", U_32, 1, NULL, 0},
    {4, "image_rotation", U_32, 1, NULL, 0},
    {5, "start_x", S_32, 1, NULL, 0},
    {6, "start_y", S_32, 1, NULL, 0},
};

#define SP_USED_OC_RANGE_CFG_PARAMS_COUNT (8U)
static PARAM_s CT_LdccSPUsedOCRangeCfgParams[SP_USED_OC_RANGE_CFG_PARAMS_COUNT] = {
    {0, "min_top", U_32, 1, NULL, 0},
    {1, "min_bottom", U_32, 1, NULL, 0},
    {2, "min_left", U_32, 1, NULL, 0},
    {3, "min_right", U_32, 1, NULL, 0},
    {4, "max_top", U_32, 1, NULL, 0},
    {5, "max_bottom", U_32, 1, NULL, 0},
    {6, "max_left", U_32, 1, NULL, 0},
    {7, "max_right", U_32, 1, NULL, 0},
};

#define SP_CAL_DISTOR_CFG_PARAMS_COUNT (2U)
static PARAM_s CT_LdccSPCalDistorCfgParams[SP_CAL_DISTOR_CFG_PARAMS_COUNT] = {
    {0, "ref_dot_num", U_32, 1, NULL, 0},
    {1, "polyfit_order", U_32, 1, NULL, 0},
};

#define MP_CAL_DISTOR_CFG_PARAMS_COUNT (4U)
static PARAM_s CT_LdccMPCalDistorCfgParams[MP_CAL_DISTOR_CFG_PARAMS_COUNT] = {
    {0, "separate_cal_step_num", U_32, 1, NULL, 0},
    {1, "ref_max_angle_deg", S_DB, 1, NULL, 0},
    {2, "polyfit_order", U_32, 1, NULL, 0},
    {3, "plane_num", U_32, 1, NULL, 0},
};

static void CT_LdccSystemProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SYSTEM_PARAMS_COUNT];
    AMBA_CT_LDCC_SYSTEM_s Data;
    AmbaCT_LdccGetSystem(&Data);
    pField[0] = Data.CalibMode;
    pField[1] = Data.ChipRev;
    pField[2] = Data.OutputPrefix;
    pField[3] = Data.LensType;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetSystem(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
        // Note # LensType default NONFISHEYE
        *pParamStatus |= (0x1ULL << 3U);
    }
}

static void CT_LdccSrcProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SRC_PARAMS_COUNT];
    AMBA_CT_LDCC_SRC_s Data;
    AmbaCT_LdccGetSrcInfo(&Data);
    pField[0] = Data.SinglePlanePatternYFilePath;
    pField[1] = Data.SinglePlanePatternUVFilePath;
    pField[2] = Data.MultiPlanePatternYFilePath;
    pField[3] = Data.MultiPlanePatternUVFilePath;
    pField[4] = &Data.IsYUV420;
    pField[5] = &Data.ImageSize.Width;
    pField[6] = &Data.ImageSize.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetSrcInfo(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdccSPPatDetCircleProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[PAT_DET_CIRCLE_PARAMS_COUNT];
    AMBA_CT_LDCC_PAT_DET_CIR_s Data;
    AmbaCT_LdccGetSinglePatDetCircleCfg(&Data);
    pField[0] = &Data.MinPerimeter;
    pField[1] = &Data.MaxPerimeter;
    pField[2] = &Data.MinDistance;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetSinglePatDetCircleCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdccMPPatDetCircleProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[PAT_DET_CIRCLE_PARAMS_COUNT];
    AMBA_CT_LDCC_PAT_DET_CIR_s Data;
    AmbaCT_LdccGetMultiPatDetCircleCfg(&Data);
    pField[0] = &Data.MinPerimeter;
    pField[1] = &Data.MaxPerimeter;
    pField[2] = &Data.MinDistance;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetMultiPatDetCircleCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdccPatDetCenterROIProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[PAT_DET_CENTER_ROI_PARAMS_COUNT];
    AMBA_CAL_ROI_s Data;
    AmbaCT_LdccGetMultiPatDetCenterROI(&Data);
    pField[0] = &Data.StartX;
    pField[1] = &Data.StartY;
    pField[2] = &Data.Width;
    pField[3] = &Data.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetMultiPatDetCenterROI(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32) Index);
    }
}

static void CT_LdccSPOrganizeCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[ORGANIZE_CFG_PARAMS_COUNT];
    AMBA_CAL_OCCB_ORGANIZE_CFG_s Data;
    AmbaCT_LdccGetSinglePlaneOrganizeCornerCfg(&Data);
    pField[0] = &Data.EstToleranceRate;
    pField[1] = &Data.CenterSearchRadius;
    pField[2] = &Data.CenterNeighborsSearchRadius;
    pField[3] = &Data.SearchRadius;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetSinglePlaneOrganizeCornerCfg(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdccMPOrganizeCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[ORGANIZE_CFG_PARAMS_COUNT];
    UINT32 ArrayIndex = (UINT32)Index / ORGANIZE_CFG_PARAMS_COUNT;
    AMBA_CAL_OCCB_ORGANIZE_CFG_s Data;
    AmbaCT_LdccGetMultiPlaneOrganizeCornerCfg(ArrayIndex, &Data);
    pField[0] = &Data.EstToleranceRate;
    pField[1] = &Data.CenterSearchRadius;
    pField[2] = &Data.CenterNeighborsSearchRadius;
    pField[3] = &Data.SearchRadius;
    CT_ProcParam(pParam, pField[((UINT32)Index % ORGANIZE_CFG_PARAMS_COUNT)]);
    AmbaCT_LdccSetMultiPlaneOrganizeCornerCfg(ArrayIndex, &Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdccCameraProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[CAMERA_PARAMS_COUNT];
    AMBA_CT_LDCC_CAMERA_s Data;
    AmbaCT_LdccGetCamera(&Data);
    pField[0] = Data.RealTable;
    pField[1] = Data.ExceptTable;
    pField[2] = &Data.TableLen;
    pField[3] = &Data.CellSize;
    pField[4] = &Data.DistTableUnit;
    pField[5] = &Data.FocalLength;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetCamera(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdccOpticalCenterProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[OPTICAL_CENTER_PARAMS_COUNT];
    AMBA_CT_LDCC_OPTICAL_CENTER_s Data;
    AmbaCT_LdccGetOpticalCenter(&Data);
    pField[0] = &Data.IdealPos.X;
    pField[1] = &Data.IdealPos.Y;
    pField[2] = &Data.SearchRange.Width;
    pField[3] = &Data.SearchRange.Height;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetOpticalCenter(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdccMPPatLayoutCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[MP_PAT_LAYOUT_CFG_PARAMS_COUNT];
    UINT32 ArrayIndex = (UINT32)Index / MP_PAT_LAYOUT_CFG_PARAMS_COUNT;
    AMBA_CT_LDCC_PAT_LAYOUT_CFG_s *pData = NULL;
    AmbaCT_LdccGetPatternLayoutCfg(ArrayIndex, &pData);
    if (pData != NULL) {
        pField[0] = &pData->FilePath;
        pField[1] = &pData->LayoutCfg.RowNum;
        pField[2] = &pData->LayoutCfg.ColNum;
        pField[3] = &pData->LayoutCfg.Center2CenterDistance;
        pField[4] = &pData->LayoutCfg.ImgRotate;
        pField[5] = &pData->LayoutCfg.StartPos.X;
        pField[6] = &pData->LayoutCfg.StartPos.Y;
        CT_ProcParam(pParam, pField[((UINT32)Index % MP_PAT_LAYOUT_CFG_PARAMS_COUNT)]);
        AmbaCT_LdccSetPatternLayoutCfg(ArrayIndex, pData);
        if (pParamStatus != NULL) {
            *pParamStatus |= (0x1ULL << (UINT32)Index);
        }
    }
}

static void CT_LdccSPUsedOCRangeCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SP_USED_OC_RANGE_CFG_PARAMS_COUNT];
    AMBA_CAL_OCCB_USED_GRID_SIZE_s Data;
    AmbaCT_LdccGetSPOCUsedRange(&Data);
    pField[0] = &Data.MinSize.Top;
    pField[1] = &Data.MinSize.Bottom;
    pField[2] = &Data.MinSize.Left;
    pField[3] = &Data.MinSize.Right;
    pField[4] = &Data.MaxSize.Top;
    pField[5] = &Data.MaxSize.Bottom;
    pField[6] = &Data.MaxSize.Left;
    pField[7] = &Data.MaxSize.Right;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetSPOCUsedRange(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdccSPCalDistorCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[SP_CAL_DISTOR_CFG_PARAMS_COUNT];
    AMBA_CT_LDCC_SP_CAL_DISTOR_CFG_s Data;
    AmbaCT_LdccGetSPCalDistor(&Data);
    pField[0] = &Data.RefDotNum;
    pField[1] = &Data.PolyOrder;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetSPCalDistor(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

static void CT_LdccMPCalDistorCfgProc(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam)
{
    void *pField[MP_CAL_DISTOR_CFG_PARAMS_COUNT];
    AMBA_CT_LDCC_MP_CAL_DISTOR_CFG_s Data;
    AmbaCT_LdccGetMPCalDistor(&Data);
    pField[0] = &Data.SeparateStepNum;
    pField[1] = &Data.RefMaxAngleDeg;
    pField[2] = &Data.PolyOrder;
    pField[3] = &Data.TotalPlane;
    CT_ProcParam(pParam, pField[Index]);
    AmbaCT_LdccSetMPCalDistor(&Data);
    if (pParamStatus != NULL) {
        *pParamStatus |= (0x1ULL << (UINT32)Index);
    }
}

#define LDCC_REGS_COUNT (13)

static REG_s LdccRegs[LDCC_REGS_COUNT] = {
    {(INT32)AMBA_CT_LDCC_TUNER_SYSTEM_INFO, "system", 1, 0, (INT32) SYSTEM_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccSystemParams, CT_LdccSystemProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_SRC_INFO, "source", 1, 0, (INT32) SRC_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccSrcParams, CT_LdccSrcProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_SP_PAT_CIRCLE_DET, "single_plane_pattern_detect_circle_cfg", 1, 0, (INT32) PAT_DET_CIRCLE_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccPatDetCircleParams, CT_LdccSPPatDetCircleProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_MP_PAT_CIRCLE_DET, "multi_plane_pattern_detect_circle_cfg", 1, 0, (INT32) PAT_DET_CIRCLE_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccPatDetCircleParams, CT_LdccMPPatDetCircleProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_MP_PAT_SEPARATE, "multi_plane_pattern_detect_center_roi", 1, 0, (INT32) PAT_DET_CENTER_ROI_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccPatDetCenterROIParams, CT_LdccPatDetCenterROIProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_SP_ORGANIZE_CORNER, "single_plane_organize_corner_cfg", 1, 0, (INT32) ORGANIZE_CFG_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccOrganizeCfgParams, CT_LdccSPOrganizeCfgProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_MP_ORGANIZE_CORNER, "multi_plane_organize_corner_cfg", (INT32)AMBA_CAL_LDCC_MAX_PLANE_NUM, 0, (INT32) ORGANIZE_CFG_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccOrganizeCfgParams, CT_LdccMPOrganizeCfgProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_CAMERA, "camera", 1, 0, (INT32)CAMERA_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccCameraParams, CT_LdccCameraProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_OPTICAL_CENTER, "optical_center", 1, 0, (INT32)OPTICAL_CENTER_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccOpticalCenterParams, CT_LdccOpticalCenterProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_MP_PAT_LAYOUT_CFG, "multi_plane_pattern_layout_cfg", (INT32)AMBA_CAL_LDCC_MAX_PLANE_NUM, 0, (INT32)MP_PAT_LAYOUT_CFG_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccMPPatLayoutCfgParams, CT_LdccMPPatLayoutCfgProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_SP_OC_USED_RANGE, "single_plane_used_oc_grid_range", 1, 0, (INT32)SP_USED_OC_RANGE_CFG_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccSPUsedOCRangeCfgParams, CT_LdccSPUsedOCRangeCfgProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_SP_CAL_DISTOR_CFG, "single_plane_cal_distortion_tbl_cfg", 1, 0, (INT32)SP_CAL_DISTOR_CFG_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccSPCalDistorCfgParams, CT_LdccSPCalDistorCfgProc, 0},
    {(INT32)AMBA_CT_LDCC_TUNER_MP_CAL_DISTOR_CFG, "multi_plane_cal_distortion_tbl_cfg", 1, 0, (INT32)MP_CAL_DISTOR_CFG_PARAMS_COUNT, PARAM_STATUS_DEFAULT, CT_LdccMPCalDistorCfgParams, CT_LdccMPCalDistorCfgProc, 0},
};

UINT32 AmbaCT_GetLdccRuleInfo(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &LdccRegs[0];
    RuleInfo->RegCount = LDCC_REGS_COUNT;
    return CT_OK;
}

UINT32 AmbaCT_GetLdccRuleInfoMisra(Rule_Info_t *RuleInfo);
UINT32 AmbaCT_GetLdccRuleInfoMisra(Rule_Info_t *RuleInfo)
{
    RuleInfo->RegList = &LdccRegs[0];
    RuleInfo->RegCount = LDCC_REGS_COUNT;
    return CT_OK;
}
