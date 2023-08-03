/**
 *  @file AmbaAP_ACC.h
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
 *  @details Header file of the Adaptive Cruise Control
 *
 */

#ifndef AMBA_AP_ACC_H
#define AMBA_AP_ACC_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif
#ifndef AMBA_SURROUND_H
#include "AmbaSurround.h"
#endif
#ifndef AMBA_WS_FCWS_H
#include "AmbaWS_FCWS.h"
#endif

/** Define for status in AMBA_AP_ACC_STATUS_s */
#define AMBA_AP_ACC_EVENT_UNKNOWN          (0U)
#define AMBA_AP_ACC_EVENT_UNACTIVE         (1U)
#define AMBA_AP_ACC_EVENT_SPEED_MODE       (2U)
#define AMBA_AP_ACC_EVENT_FOLLOW_MODE      (3U)

/** Define runtime config */
#define AMBA_AP_ACC_CFG_SET_SPEED          (0U)        /**< Runtime config SetSpeed in AMBA_AP_ACC_CONFIG_s, format DOUBLE */
#define AMBA_AP_ACC_CFG_TIME_GAP           (1U)        /**< Runtime config TimeGap in AMBA_AP_ACC_CONFIG_s, format DOUBLE */

typedef struct {
    UINT32 Event;                                      /**< ACC mode, refer to AMBA_AP_ACC_EVENT_xxx */
    DOUBLE FollowDist;                                 /**< Following distance with the target car; unit: mm */
    DOUBLE TargetSpeed;                                /**< Target car speed; unit: km/hr */
    DOUBLE RequiredSpeed;                              /**< Subject car required speed; unit: km/hr */
    DOUBLE RequiredAcceleration;                       /**< Subject car required acceleration; unit: m/s^2 */
} AMBA_AP_ACC_STATUS_s;

typedef struct {
    DOUBLE ActiveSpeed;                                /**< Operating speed of subject car, ISO rule: > 18; unit: km/hr */
    DOUBLE MaxAcceleration;                            /**< Max acceleration of subject car (positive value), ISO rule: (0.0, 2.0]; unit: m/s^2 */
    DOUBLE MinAcceleration;                            /**< Min acceleration(Max deceleration) of subject car (negative value), ISO rule: [-3.5, 0.0); unit: m/s^2 */
} AMBA_AP_ACC_SPEC_s;

typedef struct {
    AMBA_AP_ACC_SPEC_s ACCSpec;                        /**< ACC spec */
    UINT32 Clock;                                      /**< System Clock in KHz, time(ms) = TimeStamp/Clock */
    DOUBLE CarSize[2];                                 /**< Subject car size, [width,lengh]; unit: mm */
    DOUBLE TimeGap;                                    /**< Time gap with target car at follow mode, able runtime setting, ISO suggest [1.5, 2.2] and at least 0.8; unit: sec */
    DOUBLE SetSpeed;                                   /**< Set speed (max speed at any mode), it must more than ActiveSpeed and 25 kph, able runtime setting; unit: km/hr */
} AMBA_AP_ACC_CONFIG_s;

typedef struct {
    AMBA_SR_CANBUS_TRANSFER_DATA_s *pCANBusData;       /**< Canbus data */
    AMBA_WS_FCWS_OBJECT_STATUS_s *pFCWSObjOut;         /**< FCWS object status output */
} AMBA_AP_ACC_PROC_INPUT_DATA_s;

UINT32 AmbaAP_ACC_GetDefaultConfig(AMBA_AP_ACC_CONFIG_s *pACCConfig);

UINT32 AmbaAP_ACC_Init(const AMBA_AP_ACC_CONFIG_s *pACCConfig);

UINT32 AmbaAP_ACC_Process(const AMBA_AP_ACC_PROC_INPUT_DATA_s *pACCIn, AMBA_AP_ACC_STATUS_s *pAccOut);

UINT32 AmbaAP_ACC_SetConfig(UINT32 ConfigId, const void* pConfigVal);

UINT32 AmbaAP_ACC_GetConfig(UINT32 ConfigId, void* pConfigVal);

#endif