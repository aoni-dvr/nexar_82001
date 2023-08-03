/**
 *  @file AmbaCalib_Camera.h
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
#ifndef AMBA_CALIB_CAMERA_H
#define AMBA_CALIB_CAMERA_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"

#define CAL_BAYER_CHANNEL_EE  (0U)
#define CAL_BAYER_CHANNEL_EO  (1U)
#define CAL_BAYER_CHANNEL_OE  (2U)
#define CAL_BAYER_CHANNEL_OO  (3U)
#define CAL_BAYER_CHANNEL_MAX (4U)

#define MAX_LENS_DISTO_TBL_LEN (64U)
#define AMBA_CAL_FOCAL_LENGTH_UNKNOWN (0.0)
typedef enum {
    AMBA_CAL_LD_REAL_EXPECT_TBL = 0,
    AMBA_CAL_LD_ANGLE_TBL,
    AMBA_CAL_LD_REAL_EXPECT_FORMULA,
    AMBA_CAL_LD_ANGLE_FORMULA,
    AMBA_CAL_LD_USER_PINHO_DEF,
    AMBA_CAL_LD_USER_ANGLE_DEF
} AMBA_CAL_LENS_DST_TYPE_e;

typedef enum {
    AMBA_CAL_LD_MM = 0,
    AMBA_CAL_LD_PIXEL
} AMBA_CAL_LENS_DST_UNIT_e;

typedef enum {
    LENS_SPEC_PLUGIN_CUSTOM_LDC_FUNC = 0x01,
    LENS_SPEC_PLUGIN_CUSTOM_LDE_FUNC = 0x02,
} AMBA_CAL_LENS_SPEC_PLUGIN_MODE_e;


typedef struct {
    UINT32 Length;
    DOUBLE *pRealTbl;
    DOUBLE *pExpectTbl;
} AMBA_CAL_LENS_DST_REAL_EXPECT_s;

typedef struct {
    UINT32 Length;
    DOUBLE *pAngleTbl;
    DOUBLE *pRealTbl;
} AMBA_CAL_LENS_DST_ANGLE_s;

typedef struct {
    DOUBLE X9;
    DOUBLE X7;
    DOUBLE X5;
    DOUBLE X3;
    DOUBLE X1;
} AMBA_CAL_LENS_DST_FORMULA_s;

typedef struct {
    UINT32 (*pLDCFunc)(const AMBA_CAL_POINT_DB_2D_s *pInPoint, AMBA_CAL_POINT_DB_2D_s *pOutPoint);
    UINT32 (*pLDEFunc)(const AMBA_CAL_POINT_DB_2D_s *pInPoint, AMBA_CAL_POINT_DB_2D_s *pOutPoint);
} AMBA_CAL_LENS_DST_USER_PINHO_s;

typedef struct {
    UINT32 (*pLDCFunc)(const AMBA_CAL_SPHERICAL_POINT_DB_s *pInPoint, AMBA_CAL_SPHERICAL_POINT_DB_s *pOutPoint);
    UINT32 (*pLDEFunc)(const AMBA_CAL_SPHERICAL_POINT_DB_s *pInPoint, AMBA_CAL_SPHERICAL_POINT_DB_s *pOutPoint);
} AMBA_CAL_LENS_DST_USER_ANGLE_s;

typedef struct {
    AMBA_CAL_LENS_DST_TYPE_e LensDistoType;
    AMBA_CAL_LENS_DST_UNIT_e LensDistoUnit;
    struct {
        AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRealExpect;
        AMBA_CAL_LENS_DST_ANGLE_s *pAngle;
        AMBA_CAL_LENS_DST_FORMULA_s *pRealExpectFormula;
        AMBA_CAL_LENS_DST_FORMULA_s *pAngleFormula;
        AMBA_CAL_LENS_DST_USER_PINHO_s *pUserDefPinhoFunc;
        AMBA_CAL_LENS_DST_USER_ANGLE_s *pUserDefAngleFunc;
    } LensDistoSpec;
} AMBA_CAL_LENS_SPEC_V2_s;

typedef struct {
    AMBA_CAL_LENS_DST_TYPE_e LensDistoType;
    AMBA_CAL_LENS_DST_UNIT_e LensDistoUnit;
    struct {
        AMBA_CAL_LENS_DST_REAL_EXPECT_s *pRealExpect;
        AMBA_CAL_LENS_DST_ANGLE_s *pAngle;
        AMBA_CAL_LENS_DST_FORMULA_s *pRealExpectFormula;
        AMBA_CAL_LENS_DST_FORMULA_s *pAngleFormula;
    } LensDistoSpec;
} AMBA_CAL_LENS_SPEC_s;

typedef struct {
    DOUBLE CellSize;
    UINT32 StartX;
    UINT32 StartY;
    UINT32 Width;
    UINT32 Height;
} AMBA_CAL_SENSOR_s;

typedef struct {
    AMBA_CAL_LENS_SPEC_s Lens;
    AMBA_CAL_SENSOR_s Sensor;
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
    AMBA_CAL_POINT_DB_3D_s Pos;
    AMBA_CAL_ROTATION_e Rotation;
} AMBA_CAL_CAM_s;

typedef struct {
    AMBA_CAL_LENS_SPEC_V2_s Lens;
    AMBA_CAL_SENSOR_s Sensor;
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
    AMBA_CAL_POINT_DB_3D_s Pos;
    AMBA_CAL_ROTATION_e Rotation;
} AMBA_CAL_CAM_V2_s;

typedef struct {
    AMBA_CAL_LENS_SPEC_V2_s Lens;
    AMBA_CAL_SENSOR_s Sensor;
    AMBA_CAL_POINT_DB_2D_s OpticalCenter; /* Unit: pixel */
    AMBA_CAL_POINT_DB_3D_s Pos;           /* Unit: mm    */
    AMBA_CAL_ROTATION_e Rotation;
    AMBA_CAL_POINT_DB_2D_s FocalLength;   /* Unit: mm    */
} AMBA_CAL_CAM_V3_s;

typedef struct {
    AMBA_CAL_POINT_DB_3D_s Focus;
    AMBA_CAL_POINT_DB_3D_s ProjectPlane;
    AMBA_CAL_POINT_DB_2D_s ProjectPlaneShift;
} AMBA_CAL_VIRTUAL_CAM_s;

typedef struct {
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
    AMBA_CAL_SIZE_s Size;
    DOUBLE Angle;
} AMBA_CAL_ELLIPSE_INFO_s;

typedef struct {
    DOUBLE Yaw;    /* Nose left or right about an axis drawn from top to bottom*/
    DOUBLE Pitch;  /* Nose up or down about an axis running from the pilot's left to right */
    DOUBLE Roll;   /* Rotation about an axis drawn through the body of the vehicle from tail to nose */
} AMBA_CAL_ROTATION_ANGLE_3D_s;

#endif
