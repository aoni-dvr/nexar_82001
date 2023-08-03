/**
 *  @file AmbaWS_FCWS.h
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
 *  @details Header file of Front Collision Warning System
 *
 */

#ifndef AMBA_WS_FCWS_H
#define AMBA_WS_FCWS_H

#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif
#ifndef AMBA_SURROUND_H
#include "AmbaSurround.h"
#endif
#include "AmbaSR_Lane.h"

#define AMBA_WS_FCWS_MODE_POSITION              (0U)    /**< Warning when (obj position < position threshold) */
#define AMBA_WS_FCWS_MODE_TTC                   (1U)    /**< Warning when (obj ttc < ttc threshold) or (obj position < urgent position threshold) */

/* event msg */
#define AMBA_WS_FCWS_EVENT_NO_WARNING           (0U)    /**< Run FCWS proc success and no warning */
#define AMBA_WS_FCWS_EVENT_UNIMPLEMENTED        (1U)    /**< Run FCWS proc fail */
#define AMBA_WS_FCWS_EVENT_PRE_WARNING          (2U)    /**< Run FCWS proc success and raise preliminary collision warning */
#define AMBA_WS_FCWS_EVENT_COL_WARNING          (3U)    /**< Run FCWS proc success and raise collision warning */
#define AMBA_WS_FCWS_EVENT_UNACTIVE             (4U)    /**< FCWS is unactive */

/* constant */
#define TTC_LEVEL                               (10U)   /**< level number of TTC */
#define AMBA_WS_FCWS_MAX_SPEED                  (256U)  /**< Self car max speed (kph) */


/* struct define */
typedef struct {
    UINT32 StatusUpdate;                            /**< StatusUpdate, 0: invalid, 1: updated */
    UINT32 ObjectID;                                /**< object ID */
    UINT32 ObjCat;                                  /**< object category */
    UINT32 CapTS;                                   /**< Capture time */
    DOUBLE Coordinate[3];                           /**< world coordinate of target vehicle; Coordinate[0]: (Xmin+Xmax)/2, Coordinate[1]: Ymin, Coordinate[2]: 0, units:mm */
    DOUBLE RelativeSpeed;                           /**< relative speed of target object, units: km/hr */
    DOUBLE RelativeDirection[2];                    /**< relative direction of target object in unit vector, [x,y] */
    DOUBLE RealSpeed;                               /**< real speed of target object, units: km/hr */
    DOUBLE RealDirection[2];                        /**< real direction of target object in unit vector, [x,y] */
    DOUBLE CarSize[3];                              /**< target car size, [width,lengh,height], units:mm */
    DOUBLE TTCRaw;                                  /**< Raw time to collision, units: msec */
    DOUBLE TTC;                                     /**< Refined time to collision, units: msec */
    DOUBLE SafeDistance;                            /**< min safe distance, units: mm */
    UINT8 FlagRelativeSpeed;                        /**< relative speed flag; 1 for valid, 0 for invalid */
    UINT8 FlagRealSpeed;                            /**< real speed flag; 1 for valid, 0 for invalid */
} AMBA_WS_FCWS_OBJECT_STATUS_s;

typedef struct {
    UINT32 Event;                                   /**< event */
    AMBA_WS_FCWS_OBJECT_STATUS_s ObjectStatus;      /**< status of object cause event */
    AMBA_SR_LANE_MEMBER_s SubjectLaneInfo;          /**< SR lane member information */
} AMBA_WS_FCWS_STATUS_s;

typedef struct {
    DOUBLE FPS;                                     /**< frame per second */
    DOUBLE DetectionDelay;                          /**< time delay by object detection, units:msec */
    DOUBLE ReconstructionDelay;                     /**< time delay by surrounding reconstruction, units:msec */
    DOUBLE OutputDelay;                             /**< time delay to output, units:msec  */
} AMBA_WS_FCWS_SYSTEM_DELAY_s;

typedef struct {
    DOUBLE Position_th[AMBA_WS_FCWS_MAX_SPEED];     /**< Distance threshold for position/ttc mode; if close position mechanism in ttc mode -> set Position_th to 0.0 (units: m) */
    DOUBLE RelativeSpeedStride;                     /**< Stride of RelativeSpeed, units:m/s */
    DOUBLE TTC_th[TTC_LEVEL];                       /**< time to collision[N] at Speed=RelativeSpeedStride*N m/s, units:msec */
    DOUBLE DriverResponse;                          /**< driver response time, units:msec (invalid at current version) */
    DOUBLE MaxDeceleration;                         /**< max deceleration of subject car, units:m/s2 (invalid at current version) */
    DOUBLE ActiveSpeed;                             /**< operating speed, units:km/hr */
    DOUBLE PrewarnToColwarnRatio;                   /**< Pre-warning to Col-warning ratio, set 0.0~10.0 is valid, no pre-warning when value <= 1.0 */
} AMBA_WS_FCWS_SPEC_s;

typedef struct {
    UINT32 FcwsMode;                                /**< Method to give warning, please refer to AMBA_WS_FCWS_MODE_XXX */
    UINT32 Clock;                                   /**< System Clock in KHz, time(ms) = TimeStamp/Clock */
    AMBA_WS_FCWS_SYSTEM_DELAY_s Delay;              /**< delay of system */
    AMBA_WS_FCWS_SPEC_s FCWSSpec;                   /**< FCWS spec */
    DOUBLE CarSize[2];                              /**< self car size, [width,lengh], units:mm */
} AMBA_WS_FCWS_DATA_s;

typedef struct {
    AMBA_OD_2DBBX_LIST_s *pBbxList;                 /**< Bbx list data */
    AMBA_SR_SROBJECT_DATA_s *pSurrData;             /**< SR object data */
    AMBA_SR_CANBUS_TRANSFER_DATA_s *pCANBusData;    /**< Canbus data */
    AMBA_SR_ROI_s *pODRoi;                          /**< OD Roi (coordinate need to be the same as pBbxList) */
    AMBA_WS_FCWS_DATA_s *pFCWSConfig;               /**< WS configs setting */
    AMBA_SR_LANE_RECONSTRUCT_INFO_s *pSRLaneInfo;   /**< SR lane reconstruction information (optional) */
} AMBA_WS_FCWS_PROC_INPUT_DATA_s;

UINT32 AmbaWS_FCWS_SetWarningStable (UINT32 Latency);

UINT32 AmbaWS_FCWS_GetDefaultConfig (AMBA_WS_FCWS_DATA_s *pFCWSConfig);

UINT32 AmbaWS_FCWS_SetConfig (const AMBA_WS_FCWS_SYSTEM_DELAY_s *pDelay, const AMBA_WS_FCWS_SPEC_s *pFCWSSpec, const DOUBLE *pCarSize, AMBA_WS_FCWS_DATA_s *pFCWSConfig);

UINT32 AmbaWS_FCWS_Init (const AMBA_WS_FCWS_DATA_s *pFCWSConfig);

UINT32 AmbaWS_FCWS_Proc (const AMBA_SR_SROBJECT_DATA_s *pSurrData, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCANBusData, const AMBA_WS_FCWS_DATA_s *pFCWSConfig, AMBA_WS_FCWS_STATUS_s *pStatus);  /**< run FCWS process and output status */

UINT32 AmbaWS_FCWS_ProcessV2 (const AMBA_WS_FCWS_PROC_INPUT_DATA_s *pIn, AMBA_WS_FCWS_STATUS_s *pStatus);  /**< run FCWS process and output status */

#endif //AMBA_WS_FCWS_H
