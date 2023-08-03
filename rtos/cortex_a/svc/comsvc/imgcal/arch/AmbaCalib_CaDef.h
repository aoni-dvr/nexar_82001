/**
 *  @file AmbaCalib_CaDef.h
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
#ifndef AMBA_CALIB_CA_DEF_H
#define AMBA_CALIB_CA_DEF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Camera.h"
#include "AmbaCalib_Def.h"

#define MAX_LENS_CATBL_LEN (64U)
#define CA_MAX_GRID_WIDTH  (64U)
#define CA_MAX_GRID_HEIGHT (96U)
#define CA_MAX_GRID_SIZE (CA_MAX_GRID_WIDTH*CA_MAX_GRID_HEIGHT)

typedef enum {
   AMBA_CAL_CA_AUTO = 0,
   AMBA_CAL_CA_B_ONLY,
   AMBA_CAL_CA_R_ONLY,
   AMBA_CAL_CA_B_TBL_SCALE_TO_R,
   AMBA_CAL_CA_R_TBL_SCALE_TO_B,
   AMBA_CAL_CA_AVERAGE,
   AMBA_CAL_CA_SEPARATE,
   AMBA_CAL_CA_MAX,
} AMBA_CAL_CA_METHOD_TYPE_e;

typedef struct {
   DOUBLE RHor;
   DOUBLE RVer;
   DOUBLE BHor;
   DOUBLE BVer;
} AMBA_CAL_CA_OFFSET_POINT_s;

typedef struct {
   UINT32 Length;
   DOUBLE *pRRealTbl;
   DOUBLE *pBRealTbl;
   DOUBLE *pExpectTbl;
} AMBA_CAL_CA_TBL_s;

typedef struct {
   AMBA_CAL_SENSOR_s Sensor;
   AMBA_CAL_CA_TBL_s *pCaTbllnfo;
   UINT32 CenterX;
   UINT32 CenterY;
   UINT32 CompensateRatio;
   UINT32 RRatio;
   UINT32 ZoomStep;
   AMBA_CAL_CA_METHOD_TYPE_e Method;
} AMBA_CAL_CA_INFO_s;

typedef struct {
   AMBA_CAL_CA_INFO_s CaInfo;
   AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
   AMBA_CAL_SIZE_s Tile;
} AMBA_CAL_CA_CFG_s;

typedef struct {
   UINT32  Version;
   AMBA_CAL_VIN_SENSOR_GEOMETRY_s  CalibSensorGeo;
   UINT32  HorGridNum;
   UINT32  VerGridNum;
   UINT32  TileWidthExp;       /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
   UINT32  TileHeightExp;      /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
   INT16   RScaleFactor;
   INT16   BScaleFactor;
   AMBA_CAL_GRID_POINT_s *pCaTbl;
} AMBA_CAL_CA_DATA_s;

typedef struct {
   UINT32  Version;
   AMBA_CAL_VIN_SENSOR_GEOMETRY_s  CalibSensorGeo;
   UINT32  HorGridNum;
   UINT32  VerGridNum;
   UINT32  TileWidthExp;       /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
   UINT32  TileHeightExp;      /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
   INT16   RScaleFactor;
   INT16   BScaleFactor;
   AMBA_CAL_GRID_POINT_s *pRCaTbl;
   AMBA_CAL_GRID_POINT_s *pBCaTbl;
} AMBA_CAL_CA_SEPARATE_DATA_s;


#endif
