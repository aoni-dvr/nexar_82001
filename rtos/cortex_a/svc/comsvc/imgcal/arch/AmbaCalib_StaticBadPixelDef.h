/**
 *  @file AmbaCalib_StaticBadPixelDef.h
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
#ifndef AMBA_CALIB_STATIC_BAD_PIXEL_DEF_H
#define AMBA_CALIB_STATIC_BAD_PIXEL_DEF_H
#include "AmbaTypes.h"
#include "AmbaCalib_WrapDef.h"
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define AMBA_CAL_BPC_MAX_TBL_SIZE (7680U * 4320U / 8U)
#else
#define AMBA_CAL_BPC_MAX_TBL_SIZE (4000U * 3000U / 8U)
#endif
typedef enum {
    AMBA_CAL_BPC_ALGO_HOT_PIXEL = 0,
    AMBA_CAL_BPC_ALGO_DARK_PIXEL,
    AMBA_CAL_BPC_ALGO_MAX,
} AMBA_CAL_BPC_ALGO_e;

typedef enum {
    AMBA_CAL_BPC_TH_RELATIVE = 0,
    AMBA_CAL_BPC_TH_DIFFERENCE,
    AMBA_CAL_BPC_TH_MAX,
} AMBA_CAL_BPC_TH_MODE_e;

typedef struct {
    UINT32 OBEnable;          /**< enable flag to control OB, enable:1 disable:0 */
    UINT32 OBWidth;           /**< OB width of the sensor mode */
    UINT32 OBHeight;          /**< OB height of the sensor mode */
    UINT32 OBOffsetX;         /**< optical black start offset in x direction */
    UINT32 OBOffsetY;         /**< optical black start offset in y direction */
    UINT32 OBPitch;           /**< OB Pitch >= width, Pitch is related to the internal implementation of IDSP */
} AMBA_CAL_BPC_OB_INFO_s;

typedef struct {
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_BPC_ALGO_e Algo;
    AMBA_CAL_BPC_TH_MODE_e ThMode;
    DOUBLE UpperTh;     /**< max threshold */
    DOUBLE LowerTh;     /**< min threshold */
    UINT32 BlockWidth;  /**< detect block width */
    UINT32 BlockHeight; /**< detect block height */
    AMBA_CAL_BPC_OB_INFO_s OB;
} AMBA_CAL_BPC_CFG_s;

typedef struct {
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_BPC_ALGO_e Algo;      /* Detect HOT bad pixel or DARK bad pixel */
    AMBA_CAL_BPC_TH_MODE_e ThMode; /* Method about calculating final threshold */
    DOUBLE UpperTh;      /**< Range to upper threshold from calculated threshold base. Pixel that value is bigger than final upper threshold will be considered as HOT bad pixel. Don't care when "Algo" is ALGO_DARK_PIXEL */
    DOUBLE LowerTh;      /**< Range to lower threshold from calculated threshold base. Pixel that value is smaller than final lower threshold will be considered as DARK bad pixel. Don't care when "Algo" is ALGO_HOT_PIXEL */
    UINT32 BlockWidth;   /**< Detect block width */
    UINT32 BlockHeight;  /**< Detect block height */
    UINT32 CalibOffsetX; /**< Skip initial lines so that SBP calibrated VIN sensor offset x can be multiple of 8 */
    AMBA_CAL_BPC_OB_INFO_s OB;
} AMBA_CAL_BPC_CFG_V1_s;

typedef struct {
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_BPC_ALGO_e Algo;
    AMBA_CAL_BPC_TH_MODE_e ThMode;
    DOUBLE UpperTh;      /**< max threshold */
    DOUBLE LowerTh;      /**< min threshold */
    UINT32 BlockWidth;   /**< detect block width */
    UINT32 BlockHeight;  /**< detect block height */
    UINT32 CalibOffsetX; /**< Skip initial lines so that SBP calibrated VIN sensor offset x can be multiple of 8 */
    AMBA_CAL_BPC_OB_INFO_s OB;
    UINT32 SensorType;   /**< 0: RGB, 1: RGB-IR */
    UINT8 Bayer;         /**< 0: AMBA_DSP_BAYER_RG, 1: AMBA_DSP_BAYER_BG, 2: AMBA_DSP_BAYER_GR, 3: AMBA_DSP_BAYER_GB */
    UINT32 IrMode;       /**< 0: Not-IR, 1: Mode1, 2: Mode2 */
} AMBA_CAL_BPC_CFG_V2_s;

typedef struct {
    UINT32 Version;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s CalibSensorGeo;
    UINT8 SbpTbl[AMBA_CAL_BPC_MAX_TBL_SIZE];
    UINT32 SbpTblSize;
    UINT32 Count;
} AMBA_CAL_BPC_CALIB_DATA_s;

typedef struct {
    UINT32 Version;                                /**< Algo version */
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s CalibSensorGeo; /**< Sensor geometry information for calibration */
    UINT8 *pSbpTbl;                                /**< SBP calibration table buffer */
    UINT32 SbpTblSize;                             /**< Size of SBP calibration table */
    UINT32 Count;                                  /**< Total detected bad pixel number */
} AMBA_CAL_BPC_CALIB_DATA_V1_s;

#endif
