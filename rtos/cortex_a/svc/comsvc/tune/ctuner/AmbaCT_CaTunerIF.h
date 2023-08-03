/**
 *  @file AmbaCT_CaTunerIF.h
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
#ifndef AMBA_CT_CA_TUNER_IF_H
#define AMBA_CT_CA_TUNER_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_WrapDef.h"
#include "AmbaCalib_Camera.h"
typedef struct {
    char CalibMode[64];
    char ChipRev[8];
    char OutputPrefix[64];
} AMBA_CT_CA_SYSTEM_s;

typedef struct {
    DOUBLE *pExpecttable;
    DOUBLE *pCaRedTable;
    DOUBLE *pCaBlueTable;
    UINT32  TableLen;
} AMBA_CT_CA_LENS_s;

typedef struct {
    UINT32 Method;
    UINT32 Compensate;
    UINT32 Ratio;
    UINT32 Zoom_step;
} AMBA_CAL_CA_ADJUST_CFG_s;

typedef struct {
    AMBA_CT_CA_SYSTEM_s System;
    AMBA_CT_CA_LENS_s Lens;
    AMBA_CAL_SENSOR_s Sensor;
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
    AMBA_CAL_CA_ADJUST_CFG_s Adjust;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s Vin;
    AMBA_CAL_SIZE_s TileSize;
} AMBA_CT_CA_USER_SETTING_s;

typedef struct {
    AMBA_CAL_CA_METHOD_TYPE_e Method;
    AMBA_CAL_CA_DATA_s *pCalibData;
    AMBA_CAL_CA_SEPARATE_DATA_s *pSeparateCalibData;
} AMBA_CT_CA_CALIB_DATA_s;

const AMBA_CT_CA_USER_SETTING_s* AmbaCT_CaGetUserSetting(void);
UINT32 AmbaCT_CaGetCalibData(AMBA_CT_CA_CALIB_DATA_s *pData);
#endif
