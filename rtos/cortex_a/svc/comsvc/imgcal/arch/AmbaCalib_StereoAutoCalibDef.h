/**
 *  @file AmbaCalib_StereoAutoCalibDef.h
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
#ifndef AMBA_CALIB_STEREO_AUTOCALIB_DEF_H
#define AMBA_CALIB_STEREO_AUTOCALIB_DEF_H
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)

#include "AmbaCalib_Camera.h"
#include "AmbaCalib_WarpDef.h"
#include "cvapi_fma_interface.h"

#define STAC_MODE_STATIC 0U
#define STAC_MODE_MOVING 1U

#define STAC_LUT_WIDTH 128U
#define STAC_LUT_HEIGHT 96U
#define STAC_LUT_SIZE (STAC_LUT_WIDTH*STAC_LUT_HEIGHT*2U) // 2 u,v
#define STAC_DIFFERENTIAL_LUT_FILE_HEADER_SIZE 28U
#define STAC_DIFFERENTIAL_LUT_FILE_TABLE_SIZE (STAC_LUT_WIDTH*STAC_LUT_HEIGHT*4U*2U*4U) // 2 u,v ; 4 diff; 4 float (calibration tool format)

/**
 * @brief The projection model allowed.
 */
typedef enum {
    PROJECTION_MODEL_PINHOLE = 0,
    PROJECTION_MODEL_SPHERICAL = 1
} AMBA_CAL_WARP_PROJECTION_MODEL_e;

/**
 * @brief The calibration of the stereo camera.
 */
typedef struct {
    AMBA_CAL_WARP_PROJECTION_MODEL_e ProjectionModel; ///< The projection model of the stereo camera.
    DOUBLE              ProjectionModelParams[8]; ///< The intrinsic parameters of the stereo camera.
    DOUBLE              Baseline; ///< The baseline in meters of the stereo camera.
    DOUBLE              Camera2bodyOrientation[4]; ///< A quaternion representing the master camera orientation expressed in the body reference system.
    DOUBLE              Camera2bodyPosition[3]; ///< A vector representing the master camera position in meters expressed in the body reference system.
} AMBA_CAL_STEREO_CALIB_INFO_s;

typedef struct {
    UINT32 AutocalibMode;
    //external
    AMBA_CAL_STEREO_CALIB_INFO_s CalibrationInfo; ///< The camera calibration of the stereo camera, consistent with the selected half octave.
    UINT32 MaxIterations; ///< The maximum number of iterations that the internal minimizator can complete.
    UINT32 MinNumFeatures; ///< The minimum number of features in order to start the minimization.
    UINT32 MinNumBuckets; ///< The minimum number of full buckets.
    UINT8 ScaleId; ///< The id of the half octave used for extracting features.

    //internal
    UINT8 Reserved;
    DOUBLE Th1Table[4]; ///< A threshold table which indirectly defines, for each external iteration, the maximum weight for a given feature.
    DOUBLE Th2Table[4]; ///< A threshold table which indirectly defines, for each external iteration, the minimum weight for a given feature.
    INT8 HeaderLeft[STAC_DIFFERENTIAL_LUT_FILE_HEADER_SIZE]; ///< The header of the left differential LUT.
    INT8 TableLeft[STAC_DIFFERENTIAL_LUT_FILE_TABLE_SIZE]; ///< The left differential LUT.
    INT8 HeaderRight[STAC_DIFFERENTIAL_LUT_FILE_HEADER_SIZE]; ///< The header of the right differential LUT.
    INT8 TableRight[STAC_DIFFERENTIAL_LUT_FILE_TABLE_SIZE]; ///< The right differential LUT.
} AMBA_CAL_STEREO_AUTOCALIB_SETTING_s;

typedef struct {
    AMBA_CAL_STEREO_AUTOCALIB_SETTING_s Setting;
    void *pWorkingBuf;
    fma_output_t *pFma;
    fex_out_t *pFex;
} AMBA_CAL_STEREO_AUTOCALIB_CFG_s;

typedef struct {
    void *pInternal;
    UINT32 Version;
    UINT32 AutoCalibStatus;                //0: invalid warp table 1: valid warp table
    AMBA_CAL_WARP_CALIB_DATA_s WarpTbl[2]; //0: left 1: right
} AMBA_CAL_STEREO_AUTOCALIB_DATA_s;

#endif
#endif
