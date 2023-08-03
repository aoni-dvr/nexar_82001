/**
 *  @file AmbaWS_RMG.h
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
 *  @details Definitions & Constants for rear mirror guard
 *
 */

#ifndef AMBA_WS_RMG_H
#define AMBA_WS_RMG_H


#include "AmbaAdasErrorCode.h"
#include "AmbaTypes.h"
#include "AmbaSurround.h"

#define AMBA_WS_RMG_MAX_OBJ_NUM (AMBA_OD_2DBBX_MAX_BBX_NUM)

#define AMBA_WS_RMG_LOC_UNDEFINED        (0x00U)
#define AMBA_WS_RMG_LOC_EGO              (0x10U)
#define AMBA_WS_RMG_LOC_LEFT0            (0x20U)
#define AMBA_WS_RMG_LOC_RIGHT0           (0x30U)
#define AMBA_WS_RMG_LOC_REVERSE          (0x40U)

#define AMBA_WS_RMG_LOC_NEAREST          (0x01U)
#define AMBA_WS_RMG_LOC_EGO_NEAREST      (AMBA_WS_RMG_LOC_EGO + AMBA_WS_RMG_LOC_NEAREST)
#define AMBA_WS_RMG_LOC_LEFT0_NEAREST    (AMBA_WS_RMG_LOC_LEFT0 + AMBA_WS_RMG_LOC_NEAREST)
#define AMBA_WS_RMG_LOC_RIGHT0_NEAREST   (AMBA_WS_RMG_LOC_RIGHT0 + AMBA_WS_RMG_LOC_NEAREST)
#define AMBA_WS_RMG_LOC_REVERSE_NEAREST  (AMBA_WS_RMG_LOC_REVERSE + AMBA_WS_RMG_LOC_NEAREST)

/** Direction information: */
/** INACTIVATE: there is no subject speed inforamtion */
/** UNDEFINED: object location is undefined */
/** SMAE: object is same direction (speed > 20kph) */
/** OPPOSITE: object is opposite direction or speed <= 20kph */
#define AMBA_WS_RMG_DIR_INACTIVATE      (0U)
#define AMBA_WS_RMG_DIR_UNDEFINED       (1U)
#define AMBA_WS_RMG_DIR_SAME            (2U)
#define AMBA_WS_RMG_DIR_OPPOSITE        (3U)

#define AMBA_WS_RMG_APPROACH_LEVEL_NUM    (4U)


typedef struct {
    UINT32 ZoneDistance;                                         /**< Zone distance from subject tail, unit in mm*/
    UINT32 LaneWidth;                                            /**< Lane width, unit in mm*/
    UINT32 Clock;                                                /**< System Clock, time(ms) = TimeStamp/Clock, unit in KHz */
    DOUBLE CarSize[2];                                           /**< self car size [width, lengh], units in mm */
    UINT32 ReversingZoneSize[2];                                 /**< Zone size [width, length] when reversing, unit in mm, length is starting from subject tail */
    DOUBLE ApproachLevelThres[AMBA_WS_RMG_APPROACH_LEVEL_NUM];   /**< Approaching level in time to collision , unit in msec. example: {8000.0, 4000.0, 0.0, 0.0} */
} AMBA_WS_RMG_CONFIG_s;

typedef struct {
    AMBA_OD_2DBBX_LIST_s *pBbxList;                 /**< OD Bbox list */
    AMBA_SR_ROI_s *pODRoi;                          /**< OD Roi (coordinate need to be the same as pBbxList) */
    AMBA_SR_SROBJECT_DATA_s *pSurrData;             /**< SR object data */
    AMBA_SR_CANBUS_TRANSFER_DATA_s *pCANBusData;    /**< Canbus data, TransferSpeed and CANBusGearStatus is required */
} AMBA_WS_RMG_PROC_IN_s;

typedef struct {
    UINT32 Id;              /**< Object id. only valid if tracker applied */
    UINT32 Cat;             /**< Object category */
    UINT32 CapTS;           /**< Capture time stamp */
    DOUBLE Distance[2U];    /**< Object position [x, y] in mm, distance between subject center to target front center */
    UINT32 LaneLocation;    /**< Object location, refer to AMBA_WS_RMG_LOC_XXX */
    UINT32 Direction;       /**< Object Direction , refer to AMBA_WS_RMG_DIR_XXX */
    UINT32 ApproachLevel;   /**< Approach level in TTC, example: Lv0 for keep away or TTC>8000.0, Lv1 for 8000.0>=TTC>4000.0, Lv2 TTC<=4000.0 when ApproachLevelThres set as {8000.0, 4000.0, 0.0, 0.0}*/
} AMBA_WS_RMG_OBJECT_DATA_s;

typedef struct {
    UINT32 ObjNum;                                                 /**< object number */
    AMBA_WS_RMG_OBJECT_DATA_s ObjData[AMBA_WS_RMG_MAX_OBJ_NUM];    /**< Object data */
} AMBA_WS_RMG_WARN_OBJS_s;


UINT32 AmbaWS_RMG_GetDefaultConfig(AMBA_WS_RMG_CONFIG_s* pCfg);

UINT32 AmbaWS_RMG_Init(const AMBA_WS_RMG_CONFIG_s* pCfg);

UINT32 AmbaWS_RMG_Process(const AMBA_WS_RMG_PROC_IN_s* pIn, AMBA_WS_RMG_WARN_OBJS_s* pOut);



#endif