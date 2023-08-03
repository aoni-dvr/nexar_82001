/**
 *  @file RefFlow_AutoCal.h
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
 *
 *  @details Inline function definitions
 *
 */
#ifndef REFFLOW_AUTOCAL_H_
#define REFFLOW_AUTOCAL_H_

#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaWrap.h>
#include <AmbaMisraFix.h>
#include <AmbaSurround.h>
#include <AmbaSR_Lane.h>
#include <AmbaCalib_EmirrorAutoCalibIF.h>

/** Define for target number of lane line pair collecting */
#define MAX_LANE_DATA_COLLECT_NUM 500U
/** After first auto calibration finished, calculate new calibration data when calling times of AutoCal handler exceed this define value */
#define RUN_AUTOCAL_PER_CALL_TIMES 900U


/** Refer to AMBA_AUTOCAL_CALIB_INFO_s */
#define RF_AC_DFMT_CAL_INFO_V000           (0x00000100U)            /**< 0x00000100 to 0x000001FF: Reserved for AMBA_AUTOCAL_CALIB_INFO_s */
#define RF_AC_DFMT_CAL_INFO                (RF_FUNC_AC | RF_AC_DFMT_CAL_INFO_V000)

typedef struct {
    AMBA_CAL_ROTATION_ANGLE_3D_s CamRotStatus;                     /**< Rotation status of current camera */
    AMBA_CAL_WORLD_RAW_POINT_s CalibPoints[EMIR_CALIB_POINT_NUM];  /**< Calibration points which fit current camera status */
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibData;                        /**< Calibration data which fit current camera status */
} AMBA_AUTOCAL_CALIB_INFO_s;

typedef struct {
#define AUTOCAL_HDLR_DATA_COLLECT 0U
#define AUTOCAL_HDLR_CALIB_SUCCEED 1U
#define AUTOCAL_HDLR_CALIB_FAIL 2U
    UINT32 Status;                            /**< Report working status of handler */
    DOUBLE DataCollectRatio;                  /**< Report progress rate to start (next) auto calibration algorithm */
    AMBA_AUTOCAL_CALIB_INFO_s AutoCalibInfo;  /**< Returned valid auto calibration result if status is AUTOCAL_HDLR_CALIB_SUCCEED */
} AMBA_AUTOCAL_HDLR_OUT_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;                                               /**< Camera information */
    DOUBLE FocalLength;                                               /**< Focal length of camera lens. Unit is mm */
    AMBA_CAL_ROI_s RawRoi;                                            /**< Information of region of interesting(ROI) on raw data */
    AMBA_CAL_ROI_s VoutView;                                          /**< Information of vout view or view image */
    AMBA_CAL_ROI_s SegRoi;                                            /**< Information of segment data */
    AMBA_CAL_WORLD_RAW_POINT_s DefCalibPoints[EMIR_CALIB_POINT_NUM];  /**< Default calibration points data */
    AMBA_SR_LANE_RECONSTRUCT_INFO_s *pLaneDetectionData;              /**< Current lane detection data */
    AMBA_AUTOCAL_CALIB_INFO_s CurrCalInfo;                            /**< Optional. Assign result of last auto calibration result or memset to all zero */
    AMBA_SR_CANBUS_TRANSFER_DATA_s *pCanbusData;                      /**< Optional. Car can-bus data. It would help auto calibration to get better result */
    AMBA_CAL_ROTATION_ANGLE_3D_s CompenDegThreshold;                  /**< Optional. All zero to use default value. Desired maximum compensative degree in every auto calibration */
} AMBA_AUTOCAL_HDLR_CFG_s;

void AmbaAutoCal_RefHdlrWorkBufSize(SIZE_t *pSize);

UINT32 AmbaAutoCal_RefHandler(const AMBA_AUTOCAL_HDLR_CFG_s *pCfg, const void *pWorkingBuf, AMBA_AUTOCAL_HDLR_OUT_s *pOutput);

#endif /* REFFLOW_AUTOCAL_H_ */
