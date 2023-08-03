/**
 *  @file AmbaCalib_VignetteCV2Def.h
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
#ifndef AMBA_CALIB_VIGNETTE_CV2_DEF_H
#define AMBA_CALIB_VIGNETTE_CV2_DEF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_WrapDef.h"
#define VIGNETTE_DEFAULTMODE (0U)
#define VIGNETTE_KEEPRATIOMODE (1U)
#define VIGNETTE_MAX_ELLIPSE_SAMPLE (256U * 1024U)

typedef enum {
    AMBA_CAL_VIG_DEFAULT_ALGO = 0,
    AMBA_CAL_VIG_MIN_CORNER_ERR_ALGO
} AMBA_CAL_1D_VIG_CENTER_ALGO_e;

typedef struct {
    UINT32 Version;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s CalibSensorGeo;
    UINT32 RadialCoarse;
    UINT32 RadialCoarseLog;
    UINT32 RadialBinsFine;
    UINT32 RadialBinsFineLog;
    UINT32 ModelCenterX_R;  // 4 individual bayer components optical center x, relative to calib window.
    UINT32 ModelCenterX_Gr;
    UINT32 ModelCenterX_B;
    UINT32 ModelCenterX_Gb;

    UINT32 ModelCenterY_R;  // 4 individual bayer components optical center Y, relative to calib window.
    UINT32 ModelCenterY_Gr;
    UINT32 ModelCenterY_B;
    UINT32 ModelCenterY_Gb;
    UINT32 VigGainTbl_R[4][128];
    UINT32 VigGainTbl_Gr[4][128];
    UINT32 VigGainTbl_B[4][128];
    UINT32 VigGainTbl_Gb[4][128];
    UINT16 SyncCalInfo[4];  //0:R CH ,1:Ge CH,2:B CH ,3:Go CH
} AMBA_CAL_1D_VIG_CALIB_DATA_s;


typedef struct {
    UINT32 Version;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s CalibSensorGeo;
    UINT32 RadialCoarse;
    UINT32 RadialCoarseLog;
    UINT32 RadialBinsFine;
    UINT32 RadialBinsFineLog;
    UINT32 ModelCenterX_R;  // 4 individual bayer components optical center x, relative to calib window.
    UINT32 ModelCenterX_Gr;
    UINT32 ModelCenterX_B;
    UINT32 ModelCenterX_Gb;
    UINT32 ModelCenterY_R;  // 4 individual bayer components optical center Y, relative to calib window.
    UINT32 ModelCenterY_Gr;
    UINT32 ModelCenterY_B;
    UINT32 ModelCenterY_Gb;
    UINT32 VigGainTbl_R[4][128];
    UINT32 VigGainTbl_Gr[4][128];
    UINT32 VigGainTbl_B[4][128];
    UINT32 VigGainTbl_Gb[4][128];
    UINT32 SyncCalInfo[4];  //0:R CH ,1:Ge CH,2:B CH ,3:Go CH
} AMBA_CAL_1D_VIG_CALIB_DATA_V1_s;

typedef struct {
    DOUBLE Avg9x9R;
    DOUBLE Avg9x9Gr;
    DOUBLE Avg9x9B;
    DOUBLE Avg9x9Gb;
} AMBA_CAL_1D_VIG_EVA_REPORT_s;

//Configure of search center with minimum corner error
typedef struct {
    AMBA_CAL_POINT_DB_2D_s SearchCenter;
    DOUBLE SearchRadius;
} AMBA_CAL_VIG_MIN_CONR_ERR_CFG_s;

#endif
