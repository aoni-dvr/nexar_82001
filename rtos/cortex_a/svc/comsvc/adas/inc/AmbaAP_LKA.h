/**
 *  @file AmbaAP_LKA.h
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
 *  @details Header file of the Lane Keeping Assistance
 *
 */


#ifndef AMBA_AP_LKA_H
#define AMBA_AP_LKA_H


#include "AmbaTypes.h"
#include "AmbaAdasErrorCode.h"
#include "Amba_Vehicles.h"
#include "AmbaSR_Lane.h"


/** Define for status in AMBA_AP_LKA_PROC_RESULT_s */
#define AMBA_AP_LKA_STATUS_NO_PATH             (0U)    /**< No path information from SR  */
#define AMBA_AP_LKA_STATUS_UNACTIVE            (1U)    /**< Self speed is over than active speed */
#define AMBA_AP_LKA_STATUS_UNDER_STEERING      (2U)    /**< Steering angle is over than accept angle */
#define AMBA_AP_LKA_STATUS_NEUTRAL_STEERING    (3U)    /**< Neutral steering status */

/** Define for max vehicle speed in kph */
#define AMBA_AP_LKA_MAX_SPEED              (256U)


/** Define for SSG profile data number */
#define AMBA_AP_LKA_SSG_DATA_NUM           (128U)


typedef struct {
    UINT32 DataNum;                                           /**< Profile data number */
    DOUBLE LateralAcc[AMBA_AP_LKA_SSG_DATA_NUM];              /**< Lateral accerlation in m/sec^2 */
    DOUBLE SteeringAngle[AMBA_AP_LKA_SSG_DATA_NUM];           /**< Steering angle in degree */
} AMBA_AP_LKA_SSG_PROFILE_s;


typedef struct {
    AMBA_AP_LKA_SSG_PROFILE_s SSGProfile;                  /**< Self steering gradient profile, steering wheel angle to lateral accerlation */
    DOUBLE SteeringGearRatio[AMBA_AP_LKA_MAX_SPEED];       /**< The ratio of steering wheel angle to tyre angle with speed (kph) index, e.g., 360:24 degree, ratio = 15.0 */
    DOUBLE MaxSteeringAngle;                               /**< maximum turning angle of steering wheel in degree, e.g., 2.5*360 = 900 deg */
    DOUBLE AcceptSteeringAngle[AMBA_AP_LKA_MAX_SPEED];     /**< Accept turning angle of steering wheel in degree with speed (kph) index */
} AMBA_AP_LKA_STEERING_SPEC_s;


typedef struct {
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;              /**< Self car spec */
    AMBA_AP_LKA_STEERING_SPEC_s SteeringSpec;         /**< Steering related spec */
    UINT32 ActiveSpeed;                               /**< System will active when speed overthan ActiveSpeed in kph*/
    UINT32 IntervensionStrength;                      /**< Value between 0~5. 0 for week intervention, and 5 for strong intervention */
    UINT32 Clock;                                     /**< The clock of time stamp in cycle per second */
} AMBA_AP_LKA_CONFIG_s;



typedef struct {
    UINT32 Id;                                  /**< Lane Id  */
    UINT32 Status;                              /**< System status (refer to AMBA_AP_LKA_STATUS_XXXX)  */
    DOUBLE SteeringWheelAngle;                  /**< Required steering wheel angle in degree, positive for counteer-clockwise */
    DOUBLE SuggestSpeed;                        /**< Suggested speed for stablize driving in kph, invalid at this version */
    AMBA_SR_LINE_PNT_3D_DATA_s PredictivePath;      /**< Predictive path in mm */
} AMBA_AP_LKA_STEERING_INFO_s;

typedef struct {
    UINT32 TimeStamp;                                                   /**< time stamp in cycles */
    AMBA_AP_LKA_STEERING_INFO_s SteeringInfo[AMBA_SR_LANE_MAX_NUM];     /**< Steering Information for each lane */
} AMBA_AP_LKA_RESULT_s;


UINT32 AmbaAP_LKA_GetDefaultConfig(AMBA_AP_LKA_CONFIG_s* pCfg);

UINT32 AmbaAP_LKA_Init(const AMBA_AP_LKA_CONFIG_s* pCfg);

UINT32 AmbaAP_LKA_Process(const AMBA_SR_LANE_RECONSTRUCT_INFO_s* pSrLane, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanBus, AMBA_AP_LKA_RESULT_s* pRes);


#endif
