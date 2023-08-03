/**
 *  @file AmbaCT_BpcTunerIF.h
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
#ifndef AMBA_CT_BPC_TUNER_IF_H
#define AMBA_CT_BPC_TUNER_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_WrapDef.h"

#define AMBA_CT_BPC_MAX_FILE_PATH (64U)
typedef struct {
    char CalibMode[64];
    char ChipRev[8];
    char OutputPrefix[64];
} AMBA_CT_BPC_TUNER_SYSTEM_s;

typedef struct {
    AMBA_CAL_ROI_s Area;
    UINT32 Pitch;
    UINT8 FromFile;
    char Path[AMBA_CT_BPC_MAX_FILE_PATH];
    UINT32 CalibOffsetX;
    UINT8 Bayer;
    UINT32 SensorType;
    UINT32 IrMode;
} AMBA_CT_BPC_TUNER_RAW_INFO_s;

typedef struct {
    UINT8 Algo;
    UINT8 ThMode;
    DOUBLE UpperTh; /**< max threshold */
    DOUBLE LowerTh; /**< min threshold */
    UINT32 BlockWidth; /**< detect block width */
    UINT32 BlockHeight; /**< detect block height */
} AMBA_CT_BPC_TUNER_BPC_s;


typedef struct {
    AMBA_CT_BPC_TUNER_SYSTEM_s System;
    AMBA_CT_BPC_TUNER_RAW_INFO_s RawInfo;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s Vin;
    AMBA_CT_BPC_TUNER_BPC_s Bpc;
    AMBA_CAL_BPC_OB_INFO_s Ob;
} AMBA_CT_BPC_USER_SETTING_s;

typedef struct {
    AMBA_CAL_BPC_CALIB_DATA_V1_s *pCalibData;
} AMBA_CT_BPC_CALIB_DATA_s;

const AMBA_CT_BPC_USER_SETTING_s* AmbaCT_BpcGetUserSetting(void);
UINT32 AmbaCT_BpcGetCalibData(AMBA_CT_BPC_CALIB_DATA_s *pData);
#endif
