/**
 *  @file AmbaCT_LdccTunerIF.h
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

#ifndef AMBA_CT_LDCC_TUNER_IF_H
#define AMBA_CT_LDCC_TUNER_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_OcCbDef.h"
#include "AmbaCalib_LDCCIF.h"
typedef struct {
    char CalibMode[10];
    char ChipRev[10];
    char OutputPrefix[64];
    char LensType[64];
} AMBA_CT_LDCC_SYSTEM_s;

typedef struct {
    char SinglePlanePatternYFilePath[64];
    char SinglePlanePatternUVFilePath[64];
    char MultiPlanePatternYFilePath[64];
    char MultiPlanePatternUVFilePath[64];
    UINT32 IsYUV420;
    AMBA_CAL_SIZE_s ImageSize;
} AMBA_CT_LDCC_SRC_s;

typedef struct {
    DOUBLE QualityLevel;
    UINT32 MinDistance;
} AMBA_CT_LDCC_PAT_DET_CHK_s;

typedef struct {
    UINT32 MinPerimeter;
    UINT32 MaxPerimeter;
    UINT32 MinDistance;
} AMBA_CT_LDCC_PAT_DET_CIR_s;

typedef struct {
    DOUBLE RealTable[MAX_LENS_DISTO_TBL_LEN];
    DOUBLE ExceptTable[MAX_LENS_DISTO_TBL_LEN];
    UINT32 TableLen;
    DOUBLE CellSize;
    UINT32 DistTableUnit;
    DOUBLE FocalLength;
} AMBA_CT_LDCC_CAMERA_s;

typedef struct {
    AMBA_CAL_POINT_DB_2D_s IdealPos;
    AMBA_CAL_SIZE_s SearchRange;
} AMBA_CT_LDCC_OPTICAL_CENTER_s;

typedef struct {
    AMBA_CAL_OCCB_CIRCLE_GRID_PATTERN_s LayoutCfg;
    char FilePath[64];
} AMBA_CT_LDCC_PAT_LAYOUT_CFG_s;

typedef struct {
    UINT32 RefDotNum;
    UINT32 PolyOrder;
} AMBA_CT_LDCC_SP_CAL_DISTOR_CFG_s;

typedef struct {
    UINT32 SeparateStepNum;
    DOUBLE RefMaxAngleDeg;
    UINT32 PolyOrder;
    UINT32 TotalPlane;
} AMBA_CT_LDCC_MP_CAL_DISTOR_CFG_s;

typedef struct {
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s OnePlaneRETbl;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s MultiPlaneRETbl;
    AMBA_CAL_LENS_DST_ANGLE_s RealAngleTbl;
} AMBA_CT_LDCC_CALIB_DATA_s;


typedef struct {
    AMBA_CT_LDCC_SYSTEM_s System;
    AMBA_CT_LDCC_SRC_s SrcInfo;
    // AMBA_CT_LDCC_PAT_DET_CHK_s SinglePlaneDetectionCfg;
    // AMBA_CT_LDCC_PAT_DET_CHK_s MultiPlaneDetectionCfg;
    AMBA_CT_LDCC_PAT_DET_CIR_s SinglePlaneCircleDetCfg;
    AMBA_CT_LDCC_PAT_DET_CIR_s MultiPlaneCircleDetCfg;
    AMBA_CAL_ROI_s MultiPlaneDetCenterROI;
    AMBA_CAL_OCCB_ORGANIZE_CFG_s SinglePlaneOrganizeCornerCfg;
    AMBA_CAL_OCCB_ORGANIZE_CFG_s MultiPlaneOrganizeCornerCfg[AMBA_CAL_LDCC_MAX_PLANE_NUM];
    AMBA_CT_LDCC_CAMERA_s Camera;
    AMBA_CT_LDCC_OPTICAL_CENTER_s OpticalCenter;
    AMBA_CT_LDCC_PAT_LAYOUT_CFG_s PatternLayout[AMBA_CAL_LDCC_MAX_PLANE_NUM];
    AMBA_CAL_OCCB_USED_GRID_SIZE_s UsedOCGridSize;
    AMBA_CT_LDCC_SP_CAL_DISTOR_CFG_s SPCalDistorCfg;
    AMBA_CT_LDCC_MP_CAL_DISTOR_CFG_s MPCalDistorCfg;
} AMBA_CT_LDCC_USER_SETTING_s;

typedef struct {
    void *pWorkingBuf;
    SIZE_t WorkingBufSize;
    UINT32 (*pFeedSinglePlaneSrcFunc)(const AMBA_CAL_SIZE_s *pImgSize, const AMBA_CAL_YUV_INFO_s *pYUVInfo);
    UINT32 (*pDrawSinglePlaneOrgResult)(const AMBA_CAL_POINT_DB_2D_s *pCorners, const AMBA_CAL_OCCB_ORGANIZED_CORNER_s *pOrganizedCorners, UINT8 *pYImg);
    UINT32 (*pFeedMultiPlaneSrcFunc)(const AMBA_CAL_SIZE_s *pImgSize, const AMBA_CAL_YUV_INFO_s *pYUVInfo);
    UINT32 (*pDrawMultiPlaneDetResult)(const AMBA_CAL_DET_CORNER_LIST_s *pCornerList, UINT8 *pYImg);
    UINT32 (*pFeedMultiPlanePatInfo)(UINT32 PlaneIdx, AMBA_CAL_OCCB_CIRLCE_INFO_s *pPatInfo);
    UINT32 (*pDrawMultiPlaneDetResultByPlane)(const AMBA_CAL_DET_CORNER_LIST_s *pCornersList0, const AMBA_CAL_DET_CORNER_LIST_s *pCornersList1, const AMBA_CAL_DET_CORNER_LIST_s *pCornersList2, UINT8 *pYImg);
    UINT32 (*pDrawMultiPlaneOrgResult)(UINT32 PlaneIdx, const AMBA_CAL_POINT_DB_2D_s *pCorners, const AMBA_CAL_OCCB_ORGANIZED_CORNER_s *pOrganizedCorners, UINT8 *pYImg);
} AMBA_CT_LDCC_INIT_CFG_s;
UINT32 AmbaCT_LdccGetCalibData(AMBA_CT_LDCC_CALIB_DATA_s *pLdccData);
const AMBA_CT_LDCC_USER_SETTING_s* AmbaCT_LdccGetUserSetting(void);
#endif
