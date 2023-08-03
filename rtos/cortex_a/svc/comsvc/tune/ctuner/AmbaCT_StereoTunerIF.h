/**
 *  @file AmbaCT_StereoTunerIF.h
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
#ifndef AMBA_CT_STEREO_TUNER_IF_H
#define AMBA_CT_STEREO_TUNER_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Camera.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_EmirrorDef.h"
#include "AmbaCalib_WrapDef.h"

typedef struct {
    char CalibMode[10];
    char ChipRev[10];
    char OutputPrefix[64];
    UINT8 CameraNum;
} AMBA_CT_STEREO_SYSTEM_s;

typedef struct {
    UINT32 Enable;
} AMBA_CT_STEREO_WARP_OPTIMIZER_s;

typedef struct {
    char OutputPostfix[64];
} AMBA_CT_STEREO_OUTPUT_CFG_s;

typedef struct {
    AMBA_CAL_LENS_DST_TYPE_e LensDistoType;
    DOUBLE *pRealTable;
    DOUBLE *pExceptTable;
    DOUBLE *pRealAngleTable;
    DOUBLE *pExceptAngleTable;
    UINT32 TableLen;
    DOUBLE RealExpectFormula[5];
    DOUBLE AngleFormula[5];
} AMBA_CT_STEREO_LENS_s;

typedef struct {
    DOUBLE PositionX;
    DOUBLE PositionY;
    DOUBLE PositionZ;
    AMBA_CAL_ROTATION_e RotateType;
} AMBA_CT_STEREO_CAMERA_s;

typedef struct {
    DOUBLE WorldPositionX[EMIR_CALIB_POINT_NUM];
    DOUBLE WorldPositionY[EMIR_CALIB_POINT_NUM];
    DOUBLE WorldPositionZ[EMIR_CALIB_POINT_NUM];
    DOUBLE RawPositionX[EMIR_CALIB_POINT_NUM];
    DOUBLE RawPositionY[EMIR_CALIB_POINT_NUM];
    char CornerMapBinPath[64];
} AMBA_CT_STEREO_CALIB_POINTS_s;

typedef struct {
    UINT32 Number;
    DOUBLE WorldPositionX[MAX_EMIR_ASSISTANCE_POINT_NUM];
    DOUBLE WorldPositionY[MAX_EMIR_ASSISTANCE_POINT_NUM];
    DOUBLE WorldPositionZ[MAX_EMIR_ASSISTANCE_POINT_NUM];
    DOUBLE RawPositionX[MAX_EMIR_ASSISTANCE_POINT_NUM];
    DOUBLE RawPositionY[MAX_EMIR_ASSISTANCE_POINT_NUM];
} AMBA_CT_STEREO_ASSIST_POINTS_s;

typedef struct {
    AMBA_CT_STEREO_SYSTEM_s System;
    AMBA_CT_STEREO_WARP_OPTIMIZER_s WarpOptimizer;
    AMBA_CT_STEREO_OUTPUT_CFG_s OutputCfg[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_EM_CURVED_SURFACE_CFG_s VirtualCamViewCfg[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_ROI_s Vout[AMBA_CAL_EM_CAM_MAX];
    AMBA_CT_STEREO_LENS_s Lens[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_SENSOR_s Sensor[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_POINT_DB_2D_s OpticalCenter[AMBA_CAL_EM_CAM_MAX];
    AMBA_CT_STEREO_CAMERA_s Camera[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s Vin[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_ROI_s Roi[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_SIZE_s TileSize[AMBA_CAL_EM_CAM_MAX];
    AMBA_CT_STEREO_CALIB_POINTS_s CalibPointMap[AMBA_CAL_EM_CAM_MAX];
    AMBA_CT_STEREO_ASSIST_POINTS_s AssistancePointMap[AMBA_CAL_EM_CAM_MAX];
    UINT8 OptimizeLevel[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_EM_INTERNAL_CFG_s Internal[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_SIZE_s Corner2dMapSize[AMBA_CAL_EM_CAM_MAX];
    AMBA_CAL_POINT_INT_2D_s BaseCorner2dIdx[AMBA_CAL_EM_CAM_MAX];
} AMBA_CT_STEREO_USER_SETTING_s;

const AMBA_CT_STEREO_USER_SETTING_s* AmbaCT_StereoGetUserSetting(void);
#endif
