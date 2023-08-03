
/**
 *  @file AmbaWS_RCTA.h
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
 *  @details Definitions & Constants for rear crossing traffic alert (RCTA) warning system
 *
 */

#ifndef AMBA_WS_RCTA_H
#define AMBA_WS_RCTA_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif

#include "AmbaSR_SegCrossVehicle.h"

/** Define for fisheye camera channel index */
#define AMBA_WS_RCTA_CHANNEL_FRONT      (AMBA_SR_SCV_CHANNEL_FRONT)
#define AMBA_WS_RCTA_CHANNEL_REAR       (AMBA_SR_SCV_CHANNEL_REAR)
#define AMBA_WS_RCTA_CHANNEL_MAX_NUM    (AMBA_SR_SCV_CHANNEL_MAX_NUM)

#define AMBA_WS_RCTA_STATUS_NO_WARNING    (0U)
#define AMBA_WS_RCTA_STATUS_LV1_LEFT      (1U)
#define AMBA_WS_RCTA_STATUS_LV1_RIGHT     (2U)
#define AMBA_WS_RCTA_STATUS_LV2_LEFT      (3U)
#define AMBA_WS_RCTA_STATUS_LV2_RIGHT     (4U)
#define AMBA_WS_RCTA_STATUS_UNACTIVE      (6U)
//#define AMBA_WS_RCTA_STATUS_TOTAL         (7U)

#define AMBA_WS_RCTA_INVALID_OBJECT_ID    (0xFFFFFFFFU)

typedef struct {
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusInfo;  /**< Canbus Information */
    AMBA_SR_SCV_OBJECT_DATA_LIST_s SrInfo;      /**< SR Information: Result of Vehicle Detection */
} AMBA_WS_RCTA_PROC_INPUT_DATA_s;

typedef struct {
    UINT32 WarningStat;                         /**< refer to AMBA_WS_RCTA_STATUS_XXX */
    UINT32 ObjId;                               /**< The warning object's Id (if no target, ObjId == AMBA_WS_RCTA_INVALID_OBJECT_ID) */
    DOUBLE TTC;                                 /**< The time to collision of warning object (in ms)*/
    AMBA_SR_OBJECT_DATA_s  ObjData;             /**< The Info of the warning object */
    AMBA_SR_SCV_OBJECT_2D_DATA_s  ObjData2D;    /**< The 2D Info of the warning object */
} AMBA_WS_RCTA_WARNING_INFO_s;

typedef struct {
    UINT32 TTCThres;               /**< Time to collision (in ms) threshold (only active for TrustZone)*/
    UINT32 ZoneDepth;              /**< Target zone depth in y direction (mm) */
    UINT32 ZoneLength;             /**< Half of target zone length in x direction (mm), give Lv1 warning for target is approaching */
    UINT32 TTCZoneLength;          /**< Half of target zone length in x direction (mm), give Lv1 warning for target is approaching and Lv2 warning for target's TTC < TTCThres */
} ABMA_WS_RCTA_WARN_THRES_s;

typedef struct {
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;              /**< Config for self car spec */
    UINT32 ActiveSpeed;                               /**< Function is active when back speed over than ActiveSpeed (in kph, positive value) */
    INT32 Clock;                                      /**< System clock in KHz */
    UINT32 TotalDelay;                                /**< delay from frame captured to warning send (in ms)*/
    ABMA_WS_RCTA_WARN_THRES_s WarningThreshold;       /**< Config for warning threshold */
} AMBA_WS_RCTA_CFG_DATA_s;


UINT32 AmbaWS_RCTA_Process(UINT32 Channel, const AMBA_WS_RCTA_PROC_INPUT_DATA_s* pProcData, AMBA_WS_RCTA_WARNING_INFO_s* pWarnInfoL, AMBA_WS_RCTA_WARNING_INFO_s* pWarnInfoR);

UINT32 AmbaWS_RCTA_GetDefaultConfig(AMBA_WS_RCTA_CFG_DATA_s* pDefCfgData);

UINT32 AmbaWS_RCTA_SetInitConfig(UINT32 Channel, const AMBA_WS_RCTA_CFG_DATA_s* pCfgData);

#endif
