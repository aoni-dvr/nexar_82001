/**
 *  @file AmbaCalib_VignetteCV2IF.h
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
#ifndef AMBA_CALIB_VIGNETTECV2_IF_H
#define AMBA_CALIB_VIGNETTECV2_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_VignetteCV2Def.h"
#define AMBA_CAL_1D_VIG_DATA_VERSION (1U)


#define RAW14BIT_MAX_VALUE (16383U)  // Note # 14bit
#define CAL_VIG_RESOLUTION_14BIT (14U)

#define RAW_CUT_RATIO (1000U)
#define AMBA_CAL_1D_SENSOR_RGB (0U)
#define AMBA_CAL_1D_SENSOR_RGBIR (1U)


// Note #  External header file
typedef UINT32 (*AMBA_CAL_1D_VIG_CB_GET_RAW_s)(SIZE_t RawBufSize, UINT32 *pPitch, AMBA_CAL_ROI_s *pValidArea, UINT16 *pRaw);
typedef struct {
    UINT32 SensorType;
    AMBA_CAL_1D_VIG_CB_GET_RAW_s GetRawFileCBFunc;
    UINT32 GetRawNum;
    UINT32 Strength;
    UINT32 ChromaRatio;
    UINT8 StrengthEffectMode;
    UINT8 Bayer;
    UINT32 IrMode;            /* 0:Not-IR 1:Mode1 2:Mode2 */
    UINT32 Resolution;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_BLACK_CORRECTION_s BlackLevel;
} AMBA_CAL_1D_VIG_CFG_s;

typedef struct {
    UINT32 SensorType;
    UINT32 Strength;
    UINT32 ChromaRatio;
    UINT8 StrengthEffectMode;
    UINT8 Bayer;
    UINT32 IrMode;
    UINT32 Resolution;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_BLACK_CORRECTION_s BlackLevel;
    // Note # AMBA_CAL_SIZE_s ImageSize;
    UINT32 RawPitch;
    void *pRawBuf;
} AMBA_CAL_1D_VIG_CFG_V1_s;

typedef struct {
    UINT32 SensorType;
    UINT32 Strength;
    UINT32 ChromaRatio;
    UINT8 StrengthEffectMode;
    UINT8 Bayer;
    UINT32 IrMode;
    UINT32 Resolution;
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_BLACK_CORRECTION_s BlackLevel;
    // Note # AMBA_CAL_SIZE_s ImageSize;
    UINT32 RawPitch;
    void *pRawBuf;
    UINT32 MaxCenterXOffset;
    UINT32 MaxCenterYOffset;
} AMBA_CAL_1D_VIG_CFG_V2_s;

typedef struct {
    AMBA_CAL_VIG_MIN_CONR_ERR_CFG_s *pMinCornerErrCfg;
} AMBA_CAL_VIG_CENTER_ALGO_CFG_s;

typedef struct {
    UINT32 SensorType;        /* 0: RGB, 1: RGB-IR */
    UINT32 Strength;          /* 0 ~ 65536U */
    UINT32 ChromaRatio;       /* 0 ~ 65536U */
    UINT8 StrengthEffectMode; /* 0: Default, 1: Keep ratio mode */
    UINT8 Bayer;              /* 0: AMBA_DSP_BAYER_RG, 1: AMBA_DSP_BAYER_BG, 2: AMBA_DSP_BAYER_GR, 3: AMBA_DSP_BAYER_GB */
    UINT32 IrMode;            /* 0: Not-IR, 1: Mode1, 2: Mode2 */
    UINT32 Resolution;        /* Valid bit range per pixel of input RAW */
    AMBA_CAL_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_CAL_BLACK_CORRECTION_s BlackLevel;
    UINT32 RawPitch;
    void *pRawBuf;
    UINT32 MaxCenterXOffset;
    UINT32 MaxCenterYOffset;
    AMBA_CAL_1D_VIG_CENTER_ALGO_e CenterSearchAlgo; /* 0: Default, 1: Center with minimum corner error */
    AMBA_CAL_VIG_CENTER_ALGO_CFG_s CenterSearchCfg; /* Unit is pixel */
} AMBA_CAL_1D_VIG_CFG_V3_s;

typedef struct {
    AMBA_CAL_POINT_UINT_2D_s *pSamplingLocations;
#define AMBA_CAL_VIG_EVA_MAX_SAMPLE_NUM (100U)
    UINT32 SamplingNum;
    void *pRawBuf;
    AMBA_CAL_SIZE_s ImageSize;
    AMBA_CAL_BLACK_CORRECTION_s BlackLevel;
    UINT32 RawBitsNum;
    UINT32 RawPitch;
    UINT32 IrMode;
    UINT8 Bayer;
    UINT32 SensorType;
} AMBA_CAL_1D_VIG_EVA_CFG_s;

/**
* This API is used to get vignette calibration working buffer size.
* @param [out] size Vignette working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigGetWorkingBufSize(SIZE_t *size);

/**
* This API is used to generate vignette tables based on input raw data.
* @param [in] pCfg Vignette configuration. Please refer to AMBA_CAL_1D_VIG_CFG_s for more details.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_1D_VIG_CALIB_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigRaw2VigTbl(const AMBA_CAL_1D_VIG_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_1D_VIG_CALIB_DATA_s *pOutput);

/**
* This API is used to make the brightness consistent for different sensor module.
* @param [in] Num To set Sync table number
* @param [in] Threshold To set the parameter for max brightness difference.
* (Ratio:1024 ~ 8192)
* @param [in] pSrc Please refer to AMBA_CAL_1D_VIG_CALIB_DATA_s for more details.
* @param [out] pDst Returned output data. Please refer to AMBA_CAL_1D_VIG_CALIB_DATA_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigSyncTbl(UINT32 Num, UINT32 Threshold, const AMBA_CAL_1D_VIG_CALIB_DATA_s *pSrc, AMBA_CAL_1D_VIG_CALIB_DATA_s *pDst);

/**
* This API is used to get vignette calibration working buffer size.
* @param [in] pImageSize Raw image size, Please refer to AMBA_CAL_SIZE_s for more details.
* @param [in] BitsNum Valid bit resolution of the raw image.
* @param [out] pBufSize Vignette working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigGetWorkingBufSizeV1(const AMBA_CAL_SIZE_s *pImageSize, UINT32 BitsNum, SIZE_t *pBufSize);

/**
* This API is used to generate vignette tables based on input raw data.
* @param [in] pCfg Vignette configuration. Please refer to AMBA_CAL_1D_VIG_CFG_V1_s for more details.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [in] BufSize Working buffer size.
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_1D_VIG_CALIB_DATA_V1_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigRaw2VigTblV1(const AMBA_CAL_1D_VIG_CFG_V1_s *pCfg, const void *pWorkingBuf, SIZE_t BufSize, AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pOutput);

/**
* This API is used to generate vignette tables based on input raw data.
* @param [in] pCfg Vignette configuration. Please refer to AMBA_CAL_1D_VIG_CFG_V2_s for more details.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [in] BufSize Working buffer size.
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_1D_VIG_CALIB_DATA_V1_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigRaw2VigTblV2(const AMBA_CAL_1D_VIG_CFG_V2_s *pCfg, const void *pWorkingBuf, SIZE_t BufSize, AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pOutput);

/**
* This advanced API of AmbaCal_1DVigRaw2VigTbl is used to generate vignette tables based on input raw data.
* @param [in] pCfg Vignette configuration. Please refer to AMBA_CAL_1D_VIG_CFG_V2_s for more details.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [in] BufSize Working buffer size.
* @param [out] pOutput Returned output settings. Please refer to AMBA_CAL_1D_VIG_CALIB_DATA_V1_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigRaw2VigTblV3(const AMBA_CAL_1D_VIG_CFG_V3_s *pCfg, const void *pWorkingBuf, SIZE_t BufSize, AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pOutput);

/**
* This API is used to make the brightness consistent for different sensor module.
* @param [in] Num To set Sync table number
* @param [in] Threshold To set the parameter for max brightness difference.
* (Ratio:1024 ~ 8192)
* @param [in] pSrc Please refer to AMBA_CAL_1D_VIG_CALIB_DATA_V1_s for more details.
* @param [out] pDst Returned output data. Please refer to AMBA_CAL_1D_VIG_CALIB_DATA_V1_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigSyncTblV1(UINT32 Num, UINT32 Threshold, const AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pSrc, AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pDst);

/**
* This API is used to generate vignette evaluation report based on input raw data.
* @param [in] pRawInfo Vignette configuration. Please refer to AMBA_CAL_1D_VIG_CFG_s for more details.
* @param [in] pCalibData settings. Please refer to AMBA_CAL_1D_VIG_CALIB_DATA_s for more details.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [in] BufSize Working buffer size.
* @param [out] pReport Returned output evaluations. Please refer to AMBA_CAL_1D_VIG_EVA_REPORT_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigEvaluationReport(const AMBA_CAL_1D_VIG_EVA_CFG_s *pRawInfo, const AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pCalibData, const void *pWorkingBuf, SIZE_t BufSize, AMBA_CAL_1D_VIG_EVA_REPORT_s **pReport);

/**
* This API is used to get vignette evaluation working buffer size.
* @param [in] pImageSize Raw image size, Please refer to AMBA_CAL_SIZE_s for more details.
* @param [in] BitsNum Valid bit resolution of the raw image.
* @param [out] pBufSize Vignette evaluation working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_1DVigGetEvaWorkBufSize(const AMBA_CAL_SIZE_s *pImageSize, UINT32 BitsNum, SIZE_t *pBufSize);

#endif
