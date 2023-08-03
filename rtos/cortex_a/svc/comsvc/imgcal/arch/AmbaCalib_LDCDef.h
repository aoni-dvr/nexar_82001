/**
 *  @file AmbaCalib_LDCCDef.h
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
#ifndef AMBA_CALIB_LDC_DEF_H
#define AMBA_CALIB_LDC_DEF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Camera.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_WarpDef.h"
typedef struct {
    UINT8 LineStraightStrX;
    UINT8 LineStraightStrY;
    UINT8 FovWiderStrX; // TBC: view range degrees ?
    UINT8 FovWiderStrY;
    UINT8 DistEvenStrX;
    UINT8 DistEvenStrY;
} AMBA_CAL_LDC_VIEW_CFG_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_LDC_VIEW_CFG_s View;
    AMBA_CAL_SIZE_s Tile;
} AMBA_CAL_LDC_CFG_s;

typedef struct {
    AMBA_CAL_CAM_V2_s Cam;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_LDC_VIEW_CFG_s View;
    AMBA_CAL_SIZE_s Tile;
} AMBA_CAL_LDC_CFG_V2_s;


typedef struct {
    UINT32 Version;
    AMBA_CAL_WARP_CALIB_DATA_s WarpTbl;
} AMBA_CAL_LDC_DATA_s;

#endif
