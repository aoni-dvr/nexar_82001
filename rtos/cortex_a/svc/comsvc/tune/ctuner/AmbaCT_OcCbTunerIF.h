/**
 *  @file AmbaCT_OcCbTunerIF.h
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
#ifndef AMBA_CT_OCCB_TUNER_IF_H
#define AMBA_CT_OCCB_TUNER_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_WrapDef.h"
#include "AmbaCalib_OcCbDef.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_OcCbIF.h"
#define CT_OCCB_MAX_FILE_PATH (128)
#define CT_OCCB_MAX_IMG_SIZE (3840U*2160U)
#define CT_OCCB_LENS_NONFISHEYE "NONFISHEYE"
#define CT_OCCB_LENS_FISHEYE "FISHEYE"

typedef struct {
    DOUBLE *pRealTable;
    DOUBLE *pExceptTable;
    UINT32 TableLen;
} AMBA_CT_OCCB_LENS_s;

typedef struct {
    DOUBLE *pRealTable;
    DOUBLE *pExceptTable;
    UINT32 TableLen;
    DOUBLE CellSize;
} AMBA_CT_OCCB_CAMERA_s;

typedef struct {
    char FilePath[CT_OCCB_MAX_FILE_PATH];
    AMBA_CAL_SIZE_s ImgSize;
} AMBA_CT_OCCB_TUNER_SRC_s;

typedef struct {
    char CalibMode[10];
    char ChipRev[10];
    char OutputPrefix[64];
    char LensType[64];
} AMBA_CT_OCCB_TUNER_SYSTEM_s;

typedef struct {
    DOUBLE QualityLevel;
    UINT32 MinDistance;
} AMBA_CT_OCCB_TUENR_PAT_DET_CHK_s;

typedef struct {
    DOUBLE MinPerimeter;
    DOUBLE MaxPerimeter;
    DOUBLE MinDistance;
} AMBA_CT_OCCB_TUENR_PAT_DET_CIR_s;

typedef struct {
    UINT32 PatternType;
    AMBA_CT_OCCB_TUENR_PAT_DET_CHK_s CheckBoardCfg;
    AMBA_CT_OCCB_TUENR_PAT_DET_CIR_s CircleCfg;
} AMBA_CT_OCCB_TUENR_PAT_DET_s;

typedef struct {
    UINT32 Output3DWorldMap;
    AMBA_CAL_POINT_DB_3D_s BasePtPos;
    DOUBLE HorizontalGridSpace;
    DOUBLE VerticalGridSpace;
} AMBA_CT_OCCB_TUENR_World_Map_s;

typedef struct {
    AMBA_CAL_SIZE_s MaxSize;
    AMBA_CAL_SIZE_s MinSize;
} AMBA_CT_OCCB_USED_GRID_SIZE_s;

typedef struct {
    AMBA_CT_OCCB_TUNER_SYSTEM_s System;
    AMBA_CT_OCCB_TUNER_SRC_s SrcInfo;
    AMBA_CT_OCCB_TUENR_PAT_DET_s DetectionCfg;
    AMBA_CAL_OCCB_ORGANIZE_CFG_s OrganizeCornerCfg;
    AMBA_CT_OCCB_CAMERA_s Camera;
    AMBA_CAL_POINT_DB_2D_s IdealOpticalCenter;
    AMBA_CAL_SIZE_s SearchRange;
    AMBA_CT_OCCB_TUENR_World_Map_s WorldMapCfg;
    AMBA_CT_OCCB_USED_GRID_SIZE_s OptimizedGridNum;
} AMBA_CT_OCCB_USER_SETTING_s;

typedef struct {
    AMBA_CAL_POINT_DB_2D_s *pSubPixCorners;
    UINT32 CornerNum;
    AMBA_CAL_OCCB_ORGANIZED_CORNER_s *pOrganizedCorners;
    AMBA_CAL_POINT_DB_2D_s *pOpticalCenterResult;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRefinedLensSpecRealExpect;
    AMBA_CAL_POINT_INT_2D_s BaseCornerIdx;
    AMBA_CAL_WORLD_RAW_POINT_s *pCornerPairs;
    AMBA_CAL_SIZE_s CornerPairsImgSize;
} AMBA_CT_OCCB_CALIB_DATA_s;


const AMBA_CT_OCCB_USER_SETTING_s* AmbaCT_OcCbGetUserSetting(void);
UINT32 AmbaCT_OcCbGetCalibData(AMBA_CT_OCCB_CALIB_DATA_s *pData);
#endif
