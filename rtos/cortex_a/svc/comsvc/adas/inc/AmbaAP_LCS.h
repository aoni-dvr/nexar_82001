/**
 *  @file AmbaAP_LCS.h
 *
 * Copyright (c) 2021 Ambarella International LP
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
 *  @details Header file of the Lane Change Suggestion
 *
 */


#ifndef AMBA_AP_LCS_H
#define AMBA_AP_LCS_H


#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif
#ifndef AMBA_SURROUND_H
#include "AmbaSurround.h"
#endif
#ifndef AMBA_SR_LANE_H
#include "AmbaSR_Lane.h"
#endif
#ifndef AMBA_AP_ACC_H
#include "AmbaAP_ACC.h"
#endif

#define AMBA_LCS_PATH_STAT_NONE   (0xFFFFFFFFU)
#define AMBA_LCS_PATH_STAT_VALID  (1U)

#define AMBA_LCS_PATH_SELECTION_EGO    (AMBA_SR_LANE_EGO)
#define AMBA_LCS_PATH_SELECTION_LEFT   (AMBA_SR_LANE_LEFT0)
#define AMBA_LCS_PATH_SELECTION_RIGHT  (AMBA_SR_LANE_RIGHT0)

/** Define for intervension mode in configuration */
#define AMBA_LCS_MODE_AUTOMATIC       (0U)
#define AMBA_LCS_MODE_SEMIAUTOMATIC   (1U)

#define AMBA_LCS_TARGET_ID_NONE    (0xFFFFFFFFU)

/** Define for status in AMBA_AP_LCS_STEER_INFO_s */
#define AMBA_LCS_STEER_STATUS_NO_PATH             (0U)    /**< No path information */
#define AMBA_LCS_STEER_STATUS_UNACTIVE            (1U)    /**< Self speed is under active speed */
#define AMBA_LCS_STEER_STATUS_UNDER_STEERING      (2U)    /**< Steering angle is over than accept angle */
#define AMBA_LCS_STEER_STATUS_NEUTRAL_STEERING    (3U)    /**< Neutral steering status */

/** Define for status in AMBA_AP_LCS_GAS_BRAKE_INFO_s */
#define AMBA_LCS_ACC_UNACTIVE                     (0x00U)     /**< ACC unactive */
#define AMBA_LCS_ACC_ACTIVE                       (0x01U)     /**< ACC active */
#define AMBA_LCS_ACC_STATIC_MODE                  (0x0000U)   /**< ACC static mode */
#define AMBA_LCS_ACC_SPEED_MODE                   (0x0100U)   /**< ACC speed mode */
#define AMBA_LCS_ACC_FOLLOW_MODE                  (0x0200U)   /**< ACC follow mode */

#define AMBA_LCS_ACC_ACTIVE_STATIC                (AMBA_LCS_ACC_ACTIVE + AMBA_LCS_ACC_STATIC_MODE)
#define AMBA_LCS_ACC_ACTIVE_SPEED                 (AMBA_LCS_ACC_ACTIVE + AMBA_LCS_ACC_SPEED_MODE)
#define AMBA_LCS_ACC_ACTIVE_FOLLOW                (AMBA_LCS_ACC_ACTIVE + AMBA_LCS_ACC_FOLLOW_MODE)

typedef struct {
    UINT8 White;
    UINT8 Yellow;
    UINT8 Red;
    UINT8 Blue;
} AMBA_AP_LCS_LINE_COLOR_SEG_IDX_s;

typedef struct {
    UINT8 Solid;
    UINT8 Dash;
    UINT8 DoubleSolid; /* includes solid-dashed and dashed-solid */
    UINT8 DashDouble;
} AMBA_AP_LCS_LINE_TYPE_SEG_IDX_s;

typedef struct {
    AMBA_VEHICLE_PHYSICAL_CTRL_s PhySpec;             /**< Physical control spec */
    DOUBLE AcceptSteerAng[AMBA_VEHICLE_MAX_SPEED];    /**< Accept turning angle of steering in degree with speed (kph) index */
    UINT32 IntervensionStrength;                      /**< Value between 0~5. 0 for week intervention, and 5 for strong intervention */
    UINT32 ActiveSpeed;                               /**< System will active when speed overthan ActiveSpeed in kph*/
} AMBA_AP_LCS_STEER_CFG_s;

typedef struct {
    DOUBLE ActiveSpeed;                               /**< Operating speed of subject car, ISO rule: > 18; unit: km/hr */
    DOUBLE MaxAcceleration;                           /**< Max acceleration of subject car (positive value), ISO rule: (0.0, 2.0]; unit: m/s^2 */
    DOUBLE MinAcceleration;                           /**< Min acceleration(Max deceleration) of subject car (negative value), ISO rule: [-3.5, 0.0); unit: m/s^2 */
    DOUBLE TimeGap;                                   /**< Time gap with target car at follow mode, able runtime setting, ISO suggest [1.5, 2.2] and at least 0.8; unit: sec */
    DOUBLE SetSpeed;                                  /**< Set speed (max speed at any mode), it must more than ActiveSpeed and 25 kph, able runtime setting; unit: km/hr */
} AMBA_AP_LCS_ACCELERATION_CFG_s;

typedef struct {
    UINT32 Clock;                                     /**< The clock of time stamp in cycle per second */
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;              /**< Self car spec */
    AMBA_SR_ROI_s OdRoi;                              /**< OD Roi (coordinate need to be the same as pBbxList) */
    AMBA_AP_LCS_LINE_COLOR_SEG_IDX_s LineColorIdx;    /**< Segmentation color buffer index for specific element */
    AMBA_AP_LCS_LINE_TYPE_SEG_IDX_s LineTypeIdx;      /**< Segmentation type buffer index for specific element */
    AMBA_AP_LCS_STEER_CFG_s SteerCfg;                 /**< Steering-related config */
    AMBA_AP_LCS_ACCELERATION_CFG_s AccelerationCfg;   /**< Acceleration-related config */
    UINT32 IntervensionMode;                          /**< Intervension mode, please refer to AMBA_LCS_MODE_XXX */
    UINT32 ActiveSpeed;                               /**< System will active when speed over ActiveSpeed in kph, invalid at this version  */
} AMBA_AP_LCS_CONFIG_s;

typedef struct {
    AMBA_CAL_EM_CALC_COORD_CFG_V1_s CalibCfg;         /**< Calibration config, please refer to AmbaCalib_EmirrorDef.h */
    AMBA_CAL_EM_CALIB_INFO_DATA_V1_s CalibInfo;       /**< For supporting NN-3D data related work. Advance data for calibration. Generated by AmbaCal_EmGenCalibInfoV1() */
    AMBA_CAL_POINT_DB_2D_s FocalLength;               /**< For supporting NN-3D data related work. Focal length of lens; unit: cellsize */
} AMBA_AP_LCS_CALIB_CONFIG_s;

typedef struct {
    AMBA_SR_LANE_RECONSTRUCT_INFO_s* pSrLaneInfo;     /**< SR lane information */
    AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanBusData;      /**< Canbus data */
    AMBA_SR_SROBJECT_DATA_s* pSrObjData;              /**< SR object data */
    AMBA_OD_2DBBX_LIST_s* pBbxList;                   /**< Bounding box list data */
} AMBA_AP_LCS_PROC_INPUT_DATA_s;

typedef struct {
    DOUBLE xCoef[5];                                  /**< X coefficient of parametric equation */
    DOUBLE yCoef[5];                                  /**< Y coefficient of parametric equation */
} AMBA_AP_LCS_PARAMETRIC_EQUATION_s;

typedef struct {
    UINT32 PathStat;                                  /**< Path is valid or not, please refer to AMBA_LCS_PATH_STAT_XXX */
    AMBA_SR_LINE_PNT_2D_DATA_s PathPoints2D;          /**< Path points for visualization */
    AMBA_SR_LINE_PNT_3D_DATA_s PathPoints3D;          /**< 3D path points */
    AMBA_AP_LCS_PARAMETRIC_EQUATION_s PathEqParam;    /**< Parametric equation to descript lane switch path */
} AMBA_AP_LCS_PATH_DATA_s;

typedef struct {
    UINT32 Status;                                    /**< Steering status, please refer to AMBA_LCS_STEER_STATUS_XXX */
    DOUBLE SteerAng;                                  /**< Required steering angle in degree, positive for counteer-clockwise */
    DOUBLE MaxSteerAng;                               /**< Maximum turning angle of steering in degree at once, e.g., 2.5*360 = 900 deg, invalid at this version */
    DOUBLE MaxSteerAngRate;                           /**< Maximun steering angle velocity at once, invalid at this version */
} AMBA_AP_LCS_STEER_INFO_s;

typedef struct {
    UINT32 Status;                                    /**< Status, please refer to AMBA_LCS_ACC_UNACTIVE or AMBA_LCS_ACC_ACTIVE_XXX */
    DOUBLE GasPedalDepth;                             /**< Gas pedal depth in range 0~1 */
    DOUBLE BrakePedalDepth;                           /**< Brake pedal depth in range 0~1 */
    DOUBLE FollowDist;                                /**< Following distance with the target car; unit: mm */
    DOUBLE TargetSpeed;                               /**< Target car speed; unit: km/hr */
    DOUBLE RequiredSpeed;                             /**< Subject car required speed; unit: km/hr */
    DOUBLE RequiredAcceleration;                      /**< Subject car required acceleration; unit: m/s^2 */
} AMBA_AP_LCS_GAS_BRAKE_INFO_s;

typedef struct {
    UINT32 TimeStamp;                                         /**< time stamp in cycles */
    UINT8 LaneChangePracticality[AMBA_SR_LANE_MAX_NUM];       /**< use quantity degree to descibe how it possible to change lane */
    AMBA_AP_LCS_PATH_DATA_s PathData[AMBA_SR_LANE_MAX_NUM];   /**< path description */
    UINT32 PathDecision;                                      /**< the suggestion path, please refer to AMBA_LCS_PATH_SELECTION_XXX */
    UINT32 TargetID;                                          /**< the index of the target car on the suggested path */
    AMBA_AP_LCS_STEER_INFO_s SteerInfo;                       /**< the steering information for the selected path */
    AMBA_AP_LCS_GAS_BRAKE_INFO_s GasBrakeInfo;                /**< the acceleration information for the selected path */
} AMBA_AP_LCS_RESULT_s;

UINT32 AmbaAP_LCS_GetDefaultConfig(AMBA_AP_LCS_CONFIG_s *pLcsCfg);

UINT32 AmbaAP_LCS_Init(const AMBA_AP_LCS_CONFIG_s *pLcsCfg, const AMBA_AP_LCS_CALIB_CONFIG_s* pCalCfg);

UINT32 AmbaAP_LCS_Process(const AMBA_AP_LCS_PROC_INPUT_DATA_s *pLcsIn, AMBA_AP_LCS_RESULT_s *pLcsOut);


#endif
