/**
 *  @file AmbaCT_1DVigTunerIF.h
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
#ifndef AMBA_CT_1D_VIG_TUNER_IF_H
#define AMBA_CT_1D_VIG_TUNER_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_WrapDef.h"

#define AMBA_CT_1D_VIG_MAX_FILE_PATH (64)

typedef enum {
    AMBA_CT_VIG_CALIB_MODE_NORMAL = 0,
    AMBA_CT_VIG_CALIB_MODE_EVALUATION,
    AMBA_CT_VIG_CALIB_MODE_MAX,
} AMBA_CT_1D_VIG_CALIB_MODE_e;

typedef struct {
    char CalibMode[64];
    char ChipRev[8];
    char OutputPrefix[64];
    UINT32 ChanNum;
    UINT32 SyncMaxDiffTh;
} AMBA_CT_1D_VIG_TUNER_SYSTEM_s;

typedef struct {
    AMBA_CAL_ROI_s Area;
    UINT32 Pitch;
    UINT32 SensorType;
    UINT32 IrMode;
    UINT8 Resolution;
    UINT8 FromFile;
    UINT8 Bayer;
    char Path[AMBA_CT_1D_VIG_MAX_FILE_PATH];
    AMBA_CAL_BLACK_CORRECTION_s BlackLevel;
} AMBA_CT_1D_VIG_TUNER_RAW_INFO_s;

typedef struct {
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    UINT32 RawNum;
    UINT32 Strength;
    UINT32 ChromaRatio;
    UINT8 StrengthEffectMode;
    UINT8 Sync;
    UINT32 MaxCenterXOffset;
    UINT32 MaxCenterYOffset;
    UINT32 CenterSearchAlgo;
    UINT32 CenterSearchStartX;
    UINT32 CenterSearchStartY;
    UINT32 CenterSearchRadius;
} AMBA_CT_1D_VIG_TUNER_VIGNETTE_s;

typedef struct {
    UINT32 X[AMBA_CAL_VIG_EVA_MAX_SAMPLE_NUM];
    UINT32 Y[AMBA_CAL_VIG_EVA_MAX_SAMPLE_NUM];
    UINT32 Number;
} AMBA_CT_1D_VIG_EVALUATION_LOC_s;

typedef struct {
    UINT32 RadialCoarse;
    UINT32 RadialCoarseLog;
    UINT32 RadialBinsFine;
    UINT32 RadialBinsFineLog;
    UINT32 ModelCenterX_R;
    UINT32 ModelCenterX_Gr;
    UINT32 ModelCenterX_B;
    UINT32 ModelCenterX_Gb;
    UINT32 ModelCenterY_R;
    UINT32 ModelCenterY_Gr;
    UINT32 ModelCenterY_B;
    UINT32 ModelCenterY_Gb;
    char Path[AMBA_CT_1D_VIG_MAX_FILE_PATH];
} AMBA_CT_1D_VIG_EVA_CALIB_DATA_s;

typedef struct {
    AMBA_CT_1D_VIG_TUNER_SYSTEM_s System;
    AMBA_CT_1D_VIG_TUNER_RAW_INFO_s RawInfo[4];
    AMBA_CT_1D_VIG_TUNER_VIGNETTE_s Vignette[4];
    AMBA_CT_1D_VIG_EVALUATION_LOC_s EvaluationLocation[4];
    AMBA_CT_1D_VIG_EVA_CALIB_DATA_s EvaCalibData[4];
} AMBA_CT_1D_VIG_USET_SETTING_s;

typedef struct {
    AMBA_CAL_1D_VIG_CALIB_DATA_s *pCalibData;
} AMBA_CT_1D_VIG_CALIB_DATA_s;

typedef struct {
    AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pCalibData;
} AMBA_CT_1D_VIG_CALIB_DATA_V1_s;

typedef struct {
    AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pCalibData;
    AMBA_CAL_1D_VIG_EVA_REPORT_s *pEvaluationReport;
} AMBA_CT_1D_VIG_CALIB_DATA_V2_s;

const AMBA_CT_1D_VIG_USET_SETTING_s* AmbaCT_1DVigGetUserSetting(void);
UINT32 AmbaCT_1DVigGetCalibData(UINT32 Index, AMBA_CT_1D_VIG_CALIB_DATA_s *pData);
UINT32 AmbaCT_1DVigGetCalibDataV1(UINT32 Index, AMBA_CT_1D_VIG_CALIB_DATA_V1_s *pData);
UINT32 AmbaCT_1DVigGetCalibDataV2(UINT32 Index, AMBA_CT_1D_VIG_CALIB_DATA_V2_s *pData);
#endif
