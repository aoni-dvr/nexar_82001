/**
 *  @file AmbaWS_LDWS.h
 *
 * Copyright (c) 2018 Ambarella International LP
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
 *  @details Header file of Lane Departure Warning System
 *
 */


#ifndef AMBA_LDWS_H
#define AMBA_LDWS_H

#ifndef AMBA_SR_LANE_H
#include "AmbaSR_Lane.h"
#endif

/**
* Define for DepartureMsg in AMBA_LDWS_DEPARTURE_INFO_s
*/
#define AMBA_LDWS_DEPARTURE_NONE  0U
#define AMBA_LDWS_DEPARTURE_LEFT  1U
#define AMBA_LDWS_DEPARTURE_RIGHT 2U

/**
* Define for CarType in AMBA_LDWS_CFG_DATA_s
*/
#define AMBA_LDWS_PASSENGER_CAR   0U
#define AMBA_LDWS_TRUCK           1U
#define AMBA_LDWS_BUS             2U

typedef struct {
    UINT64 TimeStamp;                             /**< time stamp of detection */
    AMBA_SR_LANE_FUNC_DATA_s LaneFunc;            /**< lane function */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusInfo;    /**< canbus information [optional] */
} AMBA_LDWS_LANE_DATA_s;


/** rules of signs for DistanceLR in AMBA_LDWS_DEPARTURE_INFO_s
*       ^                       ^
*       | LeftWheel  ^          |  RightWheel
*(-)---------->(+)   |    (+)---------->(-)
*       |            |          |
*       |            |          |
*                    |
*(-)---------------------------------------->(+)  rules of signs for DepartureRate in AMBA_LDWS_DEPARTURE_INFO_s
*                    |
*                    |
*/
typedef struct {
    DOUBLE DistanceLR[AMBA_SR_LANE_LINE_MAX_NUM];    /**< Distance between front wheel to lane boundary (in mm) */
    DOUBLE DepartureRate;                            /**< Vehicle approach velocity at right angle to the lane boundary (in m/s) */
    UINT32 DepartureMsg;                             /**< Warning status */
} AMBA_LDWS_DEPARTURE_INFO_s;

typedef struct  {
    UINT32 Clock;                             /**< KHz */
    UINT32 CarType;                           /**< car type, invalid at current version */
    UINT32 WarnSensLv;                        /**< 0~100, 100: high sensitivity */
    UINT32 ActiveSpeed;                       /**< the speed that make warning system work when canbus is valid */
    AMBA_COMPACT_CAR_SPEC_s* pCarBodyInfo;    /**< car body information. Size.Length, FrontTrack and FrontOverhang is necessary */
} AMBA_LDWS_CFG_DATA_s;

UINT32 AmbaWS_LDWSProcess(const AMBA_LDWS_LANE_DATA_s* LaneData, AMBA_LDWS_DEPARTURE_INFO_s* DepRes);

UINT32 AmbaWS_LDWSGetDefaultConfig(AMBA_LDWS_CFG_DATA_s* DefaultCfg);

UINT32 AmbaWS_LDWSSetInitConfig(const AMBA_LDWS_CFG_DATA_s* InitCfg);

#endif
