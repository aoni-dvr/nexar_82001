/**
 *  @file AmbaWS_BSD.h
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
 *  @details Header file of Blind Spot Detection
 *
 */

#ifndef AMBA_WS_BSD_H
#define AMBA_WS_BSD_H

#include "AmbaAdasErrorCode.h"
#include "AmbaSurround.h"

/* event msg */
#define AMBA_WS_BSD_EVENT_NO_WARNING             (0U)    /**< Run BSD proc success and no warning. */
#define AMBA_WS_BSD_EVENT_UNIMPLEMENTED          (1U)    /**< Run BSD proc fail. */
#define AMBA_WS_BSD_EVENT_LV1_LEFT               (2U)    /**< Run BSD proc success and raise level1 warning on left side */
#define AMBA_WS_BSD_EVENT_LV1_RIGHT              (3U)    /**< Run BSD proc success and raise level1 warning on right side */
#define AMBA_WS_BSD_EVENT_LV2_LEFT               (4U)    /**< Run BSD proc success and raise level2 warning on left side */
#define AMBA_WS_BSD_EVENT_LV2_RIGHT              (5U)    /**< Run BSD proc success and raise level2 warning on right side */
#define AMBA_WS_BSD_EVENT_CLS_LV1_LEFT           (6U)    /**< Run BSD proc success and raise level1 and closing vehicle warning on left side */
#define AMBA_WS_BSD_EVENT_CLS_LV1_RIGHT          (7U)    /**< Run BSD proc success and raise level1 and closing vehicle warning on right side */
#define AMBA_WS_BSD_EVENT_CLS_LV2_LEFT           (8U)    /**< Run BSD proc success and raise level2 and closing vehicle warning on left side (inactive at current version) */
#define AMBA_WS_BSD_EVENT_CLS_LV2_RIGHT          (9U)    /**< Run BSD proc success and raise level2 and closing vehicle warning on right side (inactive at current version) */
#define AMBA_WS_BSD_EVENT_UNACTIVE               (10U)   /**< BSD is unactive */

/* constant */
#define TTC_LEVEL                                (10U)   /**< level number of TTC */

#define AMBA_WS_BSD_CHANNEL_LEFT                 (0U)    /**< BSD left channel */
#define AMBA_WS_BSD_CHANNEL_RIGHT                (1U)    /**< BSD right channel */



/* struct define for general */
typedef struct {
    DOUBLE FPS;                                      /**< frame per second */
    DOUBLE DetectionDelay;                           /**< time delay by object detection, units:msec */
    DOUBLE ReconstructionDelay;                      /**< time delay by surrounding reconstruction, units:msec */
    DOUBLE OutputDelay;                              /**< time delay to output, units:msec  */
} AMBA_WS_BSD_SYSTEM_DELAY_s;

typedef struct {
    DOUBLE ActiveSpeed;                              /**< operating speed, units:km/hr */
    DOUBLE Level1_th[2];                             /**< xy vector form original point to target object for level1 warning, units:mm */
    DOUBLE Level2_th[2];                             /**< xy vector form original point to target object for level2 warning, units:mm */
    DOUBLE Off_th[2];                                /**< xy vector form original point to target object for turning off warning, units:mm (Off_th[0] is invalid at current version) */
    DOUBLE RelativeSpeedStride;                      /**< Stride of RelativeSpeed , units:m/s */
    DOUBLE TTC_th[TTC_LEVEL];                        /**< time to collision[N] at Speed=RelativeSpeedStride*N m/s, units:ms; unused for TTC_th[0] = 0.0 */
    DOUBLE Suppress_th;                              /**< x distance from original point for suppression zone, units:mm */
    UINT32 TSClock;                                  /**< time stamp clock (TSClock = TS(cycle) / Time(ms), eg: TSClock = 12288) */
    UINT32 SuppressTargetSpeed;                      /**< Suppress warning when target speed smaller than this value, units: km/hr (set 0xFFFFFFFF to disable suppression) */
} AMBA_WS_BSD_SPEC_s;

typedef struct {
    UINT32 StatusUpdate;                             /**< StatusUpdate, 0: invalid, 1: updated */
    UINT32 ObjectID;                                 /**< object ID */
    UINT32 ObjCat;                                   /**< object category */
    UINT32 CapTS;                                    /**< Capture time */
    DOUBLE Coordinate[3];                            /**< world coordinate of target vehicle, units:mm */
    DOUBLE RelativeSpeed;                            /**< relative speed of target object, units: km/hr */
    DOUBLE RelativeDirection[2];                     /**< relative direction of target object in unit vector, [x,y] */
    DOUBLE RealSpeed;                                /**< real speed of target object, units: km/hr */
    DOUBLE RealDirection[2];                         /**< real direction of target object in unit vector, [x,y] */
    DOUBLE CarSize[3];                               /**< target car size, [width,lengh,height], units:mm */
    DOUBLE TTC;                                      /**< time to collision, units: msec */
} AMBA_WS_BSD_OBJECT_STATUS_s;


/* struct define for V1 */
typedef struct {
    AMBA_WS_BSD_SYSTEM_DELAY_s Delay;                /**< delay of system (invalid at current version) */
    AMBA_WS_BSD_SPEC_s BSDSpec;                      /**< BSD spec */
    DOUBLE CarSize[2];                               /**< self car size, [width,lengh], units:mm */
} AMBA_WS_BSD_DATA_s;

typedef struct {
    UINT32 Event;                                    /**< event */
    AMBA_WS_BSD_OBJECT_STATUS_s ObjectStatus;        /**< status of Object cause event */
} AMBA_WS_BSD_STATUS_s;


/* struct define for V2 */
typedef struct {
    DOUBLE FPS;                                      /**< Frame rate */
    UINT32 WrkBufSize;                               /**< Working buffer size */
} AMBA_WS_BSD_QUERY_CFG_V2_s;

typedef struct {
    UINT32 Channel;                                  /**< BSD channel; Refer to AMBA_WS_BSD_CHANNEL_XXXXXX */
    DOUBLE CarSize[2];                               /**< self car size, [width,lengh], units:mm */
    AMBA_SR_ROI_s ODRoi;                             /**< OD Roi */
    AMBA_WS_BSD_SPEC_s BSDSpec;                      /**< BSD spec */
} AMBA_WS_BSD_INIT_CFG_V2_s;

typedef struct {
    UINT32 Channel;                                  /**< BSD channel; Refer to AMBA_WS_BSD_CHANNEL_XXXXXX */
    AMBA_SR_CANBUS_TRANSFER_DATA_s *pCANBusData;     /**< Canbus data */
    AMBA_OD_2DBBX_LIST_s *pBbxList;                  /**< Bbx list data */
    AMBA_SR_SROBJECT_DATA_s *pSurrData;              /**< SR object data */
} AMBA_WS_BSD_PROC_INPUT_DATA_V2_s;

typedef struct {
    UINT32 Event;                                    /**< Event */
    AMBA_WS_BSD_OBJECT_STATUS_s ObjectStatus;        /**< Status of Object cause event */
} AMBA_WS_BSD_PROC_OUTPUT_DATA_V2_s;



UINT32 AmbaWS_BSD_GetDefaultConfig(AMBA_WS_BSD_DATA_s *pBSDConfig);
UINT32 AmbaWS_BSD_SetConfig(const AMBA_WS_BSD_SYSTEM_DELAY_s *pDelay, const AMBA_WS_BSD_SPEC_s *pBSDSpec, const DOUBLE *pCarSize, AMBA_WS_BSD_DATA_s *pBSDConfig);
UINT32 AmbaWS_BSD_Init(const AMBA_WS_BSD_DATA_s *pBSDConfig);
UINT32 AmbaWS_BSD_Proc(const AMBA_SR_SROBJECT_DATA_s *pSurrData, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCANBusData, const AMBA_WS_BSD_DATA_s *pBSDConfig, AMBA_WS_BSD_STATUS_s *pStatusLeft, AMBA_WS_BSD_STATUS_s *pStatusRight);

UINT32 AmbaWS_BSD_QueryV2(AMBA_WS_BSD_QUERY_CFG_V2_s *pQueryCfg);
UINT32 AmbaWS_BSD_GetDefaultConfigV2(AMBA_WS_BSD_INIT_CFG_V2_s *pInitCfg);
UINT32 AmbaWS_BSD_InitV2(const AMBA_WS_BSD_INIT_CFG_V2_s *pInitCfg, UINT8 *pWrkBufAddr);
UINT32 AmbaWS_BSD_ProcessV2(const AMBA_WS_BSD_PROC_INPUT_DATA_V2_s *pIn, AMBA_WS_BSD_PROC_OUTPUT_DATA_V2_s *pOut);

#endif //AMBA_WS_BSD_H
