/**
 *  @file AmbaWS_FCMD.h
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
 *  @details Header file of Front Car Moving Detection
 *
 */

#ifndef AMBA_WS_FCMD_H
#define AMBA_WS_FCMD_H

#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif
#ifndef AMBA_SURROUND_H
#include "AmbaSurround.h"
#endif

/* event msg */
#define AMBA_WS_FCMD_EVENT_NO_WARNING           (0U)    /**< Run FCMD proc success and no warning. */
#define AMBA_WS_FCMD_EVENT_UNIMPLEMENTED        (1U)    /**< Run FCMD proc fail. */
#define AMBA_WS_FCMD_EVENT_WARNING              (2U)    /**< Run FCMD proc success and raise warning */
#define AMBA_WS_FCMD_EVENT_UNACTIVE             (3U)    /**< FCMD is unactive */

/* struct define */
typedef struct {
    UINT32 StatusUpdate;                             /**< StatusUpdate, 0: invalid, 1: updated  */
    UINT32 ObjectID;                                 /**< object ID */
    UINT32 ObjCat;                                   /**< object category */
    UINT32 CapTS;                                    /**< Capture time */
    DOUBLE Coordinate[3];                            /**< world coordinate of target vehicle, units:mm */
    DOUBLE RelativeSpeed;                            /**< relative speed of target object, units: km/hr */
    DOUBLE RelativeDirection[2];                     /**< relative direction of target object in unit vector [x,y] */
    DOUBLE RealSpeed;                                /**< real speed of target object, invalid at current version, units: km/hr */
    DOUBLE RealDirection[2];                         /**< real direction of target object in unit vector [x,y], invalid at current version */
    DOUBLE CarSize[3];                               /**< target car size, [width,lengh,height], units:mm */
} AMBA_WS_FCMD_OBJECT_STATUS_s;

typedef struct {
    UINT32 Event;                                    /**< event */
    AMBA_WS_FCMD_OBJECT_STATUS_s ObjectStatus;       /**< status of Object cause event */
} AMBA_WS_FCMD_STATUS_s;

typedef struct {
    DOUBLE FPS;                                      /**< frame per second */
    DOUBLE DetectionDelay;                           /**< time delay by object detection, units:msec */
    DOUBLE ReconstructionDelay;                      /**< time delay by surrounding reconstruction, units:msec */
    DOUBLE OutputDelay;                              /**< time delay to output, units:msec  */
} AMBA_WS_FCMD_SYSTEM_DELAY_s;

typedef struct {
    DOUBLE Distance[2];                              /**< thresh distance, [x,y] , units:mm */
    DOUBLE ActiveSpeed;                              /**< max operating speed, units:km/hr */
} AMBA_WS_FCMD_SPEC_s;

typedef struct {
    UINT32 Clock;                                    /**< System Clock in KHz, time(ms) = TimeStamp/Clock */
    AMBA_WS_FCMD_SYSTEM_DELAY_s Delay;               /**< delay of system */
    AMBA_WS_FCMD_SPEC_s FCMDSpec;                    /**< FCMD spec */
    DOUBLE CarSize[2];                               /**< self car size, [width,lengh], units:mm */
} AMBA_WS_FCMD_DATA_s;

UINT32 AmbaWS_FCMD_SetWarningStable (UINT32 ActiveWarningLatency, UINT32 WarningLatency);

UINT32 AmbaWS_FCMD_GetDefaultConfig (AMBA_WS_FCMD_DATA_s *pFCMDConfig);

UINT32 AmbaWS_FCMD_SetConfig (const AMBA_WS_FCMD_SYSTEM_DELAY_s *pDelay, const AMBA_WS_FCMD_SPEC_s *pFCMDSpec, const DOUBLE *pCarSize, AMBA_WS_FCMD_DATA_s *pFCMDConfig);

UINT32 AmbaWS_FCMD_Init (const AMBA_WS_FCMD_DATA_s *pFCMDConfig);

UINT32 AmbaWS_FCMD_Proc (const AMBA_SR_SROBJECT_DATA_s *pSurrData, const AMBA_SR_CANBUS_TRANSFER_DATA_s *pCANBusData, const AMBA_WS_FCMD_DATA_s *pFCMDConfig, AMBA_WS_FCMD_STATUS_s *pStatus);

#endif //AMBA_WS_FCMD_H
