/**
 *  @file AmbaCT_AvmTunerIF.h
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
#ifndef AMBA_CT_AVM_TUNER_IF_H
#define AMBA_CT_AVM_TUNER_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Camera.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_AVMDef.h"
#include "AmbaCalib_WrapDef.h"

typedef enum {
    AMBA_CT_AVM_TYPE_2D = 0,
    AMBA_CT_AVM_TYPE_3D,
    AMBA_CT_AVM_TYPE_MV,
    AMBA_CT_AVM_TYPE_MAX,
} AMBA_CT_AVM_TYPE_e;

typedef enum {
    AMBA_CT_AVM_CALIB_MODE_NORMAL = 0,
    AMBA_CT_AVM_CALIB_MODE_PRE_CALC,
    AMBA_CT_AVM_CALIB_MODE_FAST,
    AMBA_CT_AVM_CALIB_MODE_PRE_CHECK,
    AMBA_CT_AVM_CALIB_MODE_MAX,
} AMBA_CT_AVM_CALIB_MODE_e;

typedef struct {
    char CalibMode[16];
    char Type[10];
    char ChipRev[10];
    char OutputPrefix[64];
    UINT32 EnableSavePrecheck;
} AMBA_CT_AVM_SYSTEM_s;

typedef struct {
    UINT32 DisplayHeight;
    AMBA_CAL_AVM_VIEW_LAYOUT_s CropRange;
    UINT32 AutoVoutOrderEnable;
} AMBA_CT_AVM_2D_VIEW_s;

typedef struct {
    UINT32 Enable;
    DOUBLE CalibMatrix[9];
} AMBA_CT_AVM_2D_CUSTOM_CALIB_s;

typedef struct {
    UINT32 FloorRangeX;
    UINT32 FloorRangeY;
    DOUBLE CenterPositionX;
    DOUBLE CenterPositionY;
    UINT32 Distance;
    UINT32 Height;
    UINT32 DisplayWidth;
    UINT32 DisplayHeight;
    UINT32 DisplayRotation;
    UINT32 AutoVoutOrderEnable;
} AMBA_CT_AVM_3D_VIEW_s;

typedef struct {
    UINT32 TopFrontRange;
    UINT32 TopBackRange;
    UINT32 TopLeftRange;
    UINT32 TopRightRange;
    UINT32 TopFrontLeftRadiusX;
    UINT32 TopFrontLeftRadiusY;
    UINT32 TopFrontRightRadiusX;
    UINT32 TopFrontRightRadiusY;
    UINT32 TopBackLeftRadiusX;
    UINT32 TopBackLeftRadiusY;
    UINT32 TopBackRightRadiusX;
    UINT32 TopBackRightRadiusY;
    UINT32 TopHeight;
    UINT32 BottomFrontRange;
    UINT32 BottomBackRange;
    UINT32 BottomLeftRange;
    UINT32 BottomRightRange;
    UINT32 BottomFrontLeftRadiusX;
    UINT32 BottomFrontLeftRadiusY;
    UINT32 BottomFrontRightRadiusX;
    UINT32 BottomFrontRightRadiusY;
    UINT32 BottomBackLeftRadiusX;
    UINT32 BottomBackLeftRadiusY;
    UINT32 BottomBackRightRadiusX;
    UINT32 BottomBackRightRadiusY;
    UINT32 TopToBottomRadiusH;
    UINT32 TopToBottomRadiusV;
    UINT32 BottomCornerIntervalNum;
    UINT32 PillarIntervalNum;
    DOUBLE CenterPositionX;
    DOUBLE CenterPositionY;
    DOUBLE RotationAngle;
    UINT32 DisplayWidth;
    UINT32 DisplayHeight;
    UINT32 DisplayRotation;
    UINT32 AutoVoutOrderEnable;
} AMBA_CT_AVM_3D_ADV_VIEW_s;

typedef struct {
    char ModelType[16];
} AMBA_CT_AVM_3D_VIEW_MODEL_TYPE_s;

typedef struct {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 Width;
    UINT32 Height;
    UINT32 FromFile;
    char TablePath[64];
    UINT8 Table[AVM_BLEND_MAX_SIZE];
} AMBA_CT_AVM_OSD_MASK_s;

typedef struct {
    AMBA_CAL_LENS_DST_TYPE_e LensDistoType;
    DOUBLE *pRealTable;
    DOUBLE *pExceptTable;
    DOUBLE *pExceptAngleTable;
    UINT32 TableLen;
    DOUBLE RealExpectFormula[5];
    DOUBLE AngleFormula[5];
} AMBA_CT_AVM_LENS_s;

typedef struct {
    DOUBLE PositionX;
    DOUBLE PositionY;
    DOUBLE PositionZ;
    AMBA_CAL_ROTATION_e RotateType;
    DOUBLE FocalLength;
} AMBA_CT_AVM_CAMERA_s;

typedef struct {
    DOUBLE WorldPositionX[AVM_CALIB_POINT_NUM];
    DOUBLE WorldPositionY[AVM_CALIB_POINT_NUM];
    DOUBLE WorldPositionZ[AVM_CALIB_POINT_NUM];
    DOUBLE RawPositionX[AVM_CALIB_POINT_NUM];
    DOUBLE RawPositionY[AVM_CALIB_POINT_NUM];
} AMBA_CT_AVM_CALIB_POINTS_s;

typedef struct {
    UINT32 Number;
    DOUBLE WorldPositionX[MAX_AVM_ASSISTANCE_POINT_NUM];
    DOUBLE WorldPositionY[MAX_AVM_ASSISTANCE_POINT_NUM];
    DOUBLE WorldPositionZ[MAX_AVM_ASSISTANCE_POINT_NUM];
    DOUBLE RawPositionX[MAX_AVM_ASSISTANCE_POINT_NUM];
    DOUBLE RawPositionY[MAX_AVM_ASSISTANCE_POINT_NUM];
} AMBA_CT_AVM_ASSISTANCE_POINTS_s;

typedef struct {
    UINT32 RoiEnable;
    UINT32 FrontEndRotationEnable;
} AMBA_CT_AVM_AUTO_s;

typedef struct {
    UINT32 Number;
    char InputFile[64];
} AMBA_CT_AVM_PRE_CHECK_s;

typedef struct {
    UINT32 EnableRansac;
    UINT32 IterationNum;
    DOUBLE AllowedReprojErr;
} AMBA_CT_AVM_CALIB_s;

typedef struct {
    AMBA_CT_AVM_SYSTEM_s System;
    AMBA_CAL_SIZE_s Car;
    AMBA_CT_AVM_2D_VIEW_s View2D;
    AMBA_CT_AVM_2D_CUSTOM_CALIB_s CustomCalib2D[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CT_AVM_3D_VIEW_MODEL_TYPE_s View3DModelType;
    AMBA_CT_AVM_3D_VIEW_s View3D;
    AMBA_CT_AVM_3D_ADV_VIEW_s AdvView3D;
    AMBA_CAL_AVM_3D_WARP_CAL_TYPE_e WarpCalType[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_VIRTUAL_CAM_s VirtualCam3D;
    AMBA_CAL_AVM_VIEW_TYPE_e MainViewType;
    AMBA_CAL_AVM_MV_PERSPET_CFG_s MainViewPerspetCfg;
    AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s MainViewPerspetCfgV1;
    AMBA_CAL_AVM_MV_EQ_DISTANCE_s MainViewEqualDistCfg;
    AMBA_CAL_AVM_MV_LDC_CFG_s MainViewLdcCfg;
    AMBA_CAL_POINT_DB_3D_s MainViewDirection;
    AMBA_CAL_AVM_BLEND_CFG_V2_s Blend;
    AMBA_CT_AVM_OSD_MASK_s OsdMask;
    AMBA_CT_AVM_LENS_s Lens[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_SENSOR_s Sensor[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_POINT_DB_2D_s OpticalCenter[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CT_AVM_CAMERA_s Camera[AMBA_CAL_AVM_CAM_MAX];
    AMBA_IK_VIN_SENSOR_GEOMETRY_s Vin[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CT_AVM_CALIB_POINTS_s CalibPointMap[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CT_AVM_ASSISTANCE_POINTS_s AssistancePointMap[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_ROI_s Roi[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_SIZE_s MainSize[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_SIZE_s MinVout[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_SIZE_s MinRoiSize[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_SIZE_s TileSize[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CT_AVM_AUTO_s Auto[AMBA_CAL_AVM_CAM_MAX];
    UINT8 OptimizeLevel[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CAL_AVM_INTERNAL_CFG_s Internal[AMBA_CAL_AVM_CAM_MAX];
    AMBA_CT_AVM_PRE_CHECK_s PreCheckData;
    AMBA_CT_AVM_CALIB_s CalibSetting;
} AMBA_CT_AVM_USER_SETTING_s;

typedef struct {
    AMBA_CT_AVM_TYPE_e Type;
    AMBA_CT_AVM_CALIB_MODE_e Mode;
    AMBA_CAL_AVM_2D_DATA_s *pData2D;
    AMBA_CAL_AVM_3D_DATA_s *pData3D;
    AMBA_CAL_AVM_MV_DATA_s *pDataMv;
    AMBA_CAL_AVM_PRE_CHECK_RESULT_s *pDataPreCheck;
} AMBA_CT_AVM_CALIB_DATA_s;
const AMBA_CT_AVM_USER_SETTING_s* AmbaCT_AvmGetUserSetting(void);
UINT32 AmbaCT_AvmGetCalibData(AMBA_CT_AVM_CALIB_DATA_s *pData);
#endif
