/**
 *  @file AmbaCalib_Def.h
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
#ifndef AMBA_CALIB_DEF_H
#define AMBA_CALIB_DEF_H
#include "AmbaTypes.h"
#include "AmbaCalib_WrapDef.h"
#define BLEND_ALPHA_ITSELF (255U)
#define CALIB_MAX_RAW_WIDTH (4000U)
#define CALIB_MAX_RAW_HEIGHT (3000U)
#define CALIB_MAX_RAW_SIZE (CALIB_MAX_RAW_WIDTH  * CALIB_MAX_RAW_HEIGHT)

// Note # support 8k bayer diagonalline length
#define CALIB_VIG_MAX_RAW_WIDTH (4480U)

typedef enum {
    AMBA_CAL_TBL_VALID = 0,
    AMBA_CAL_TBL_INVALID,
} AMBA_CAL_TBL_STATUS_e;

typedef enum {
    AMBA_CAL_ROTATE_0 = 0,
    AMBA_CAL_ROTATE_90,
    AMBA_CAL_ROTATE_180,
    AMBA_CAL_ROTATE_270,
} AMBA_CAL_ROTATION_e;

typedef struct {
    DOUBLE X;
    DOUBLE Y;
} AMBA_CAL_POINT_DB_2D_s;

typedef struct {
    INT32 X;
    INT32 Y;
} AMBA_CAL_POINT_INT_2D_s;

typedef struct {
    UINT32 X;
    UINT32 Y;
} AMBA_CAL_POINT_UINT_2D_s;

typedef struct {
    DOUBLE X;
    DOUBLE Y;
    DOUBLE Z;
} AMBA_CAL_POINT_DB_3D_s;

typedef struct {
    INT32 X;
    INT32 Y;
    INT32 Z;
} AMBA_CAL_POINT_INT_3D_s;

typedef struct {
    UINT32 Is16BitsRawBuf;
    void *pRawBuf;
} AMBA_CAL_RAW_BUF_s;


typedef struct {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 Width;
    UINT32 Height;
} AMBA_CAL_ROI_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
} AMBA_CAL_SIZE_s;

typedef struct {
    DOUBLE Width;
    DOUBLE Height;
} AMBA_CAL_SIZE_DB_s;

typedef struct {
    AMBA_CAL_POINT_DB_3D_s WorldPos;
    AMBA_CAL_POINT_DB_2D_s RawPos;
} AMBA_CAL_WORLD_RAW_POINT_s;

typedef struct {
    DOUBLE RadialDist;
    DOUBLE PolarAngle;
    DOUBLE AzimuthAngle;
} AMBA_CAL_SPHERICAL_POINT_DB_s;

typedef struct {
    UINT32 Top;
    UINT32 Bottom;
    UINT32 Left;
    UINT32 Right;
} AMBA_CAL_U32_4_AXIS_SIZE_s;

typedef struct {
    DOUBLE Min;
    DOUBLE Max;
} AMBA_CAL_DB_RANGE_s;

typedef enum {
    AMBA_CAL_YUV_420 = 0,
    AMBA_CAL_YUV_422,
} AMBA_CAL_YUV_TYPE_e;

typedef struct {
    AMBA_CAL_YUV_TYPE_e Format;
    UINT8 *pY;
    UINT8 *pUV;
} AMBA_CAL_YUV_INFO_s;

typedef struct {
    UINT8 R;
    UINT8 G;
    UINT8 B;
} AMBA_CAL_RGB_s;

typedef struct {
    UINT8 Y;
    UINT8 U;
    UINT8 V;
} AMBA_CAL_YUV_s;
#endif
