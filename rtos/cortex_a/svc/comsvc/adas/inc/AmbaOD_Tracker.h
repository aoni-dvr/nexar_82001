/**
 * @file AmbaOD_Tracker.h
 *
 * Copyright (c) 2019 Ambarella International LP
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
 *  @details AmbaOD_Tracker
 *
 */

#ifndef AMBA_OD_TRACKER_H
#define AMBA_OD_TRACKER_H

#include "AmbaTypes.h"
#include "AmbaErrorCode.h"
#include "AmbaSR_ObjType.h"
#include "AmbaOD_2DBbx.h"

typedef struct {
    AMBA_OD_2DBBX_s ObjInfo;                                         /**< Object information */
    UINT32 Counter;                                                  /**< Counter of the same object ID. For example, objID_100 shows up first time, then counter = 1; objID_100 shows up second times, then counter = 2 */
    UINT32 Repeat;                                                   /**< Repeated times of the same object ID. If the previous existing objID lost in current frame, we copy it from previous frame and assign "repeat = 1" */
    UINT32 BestScore;                                                /**< The highest score of the same object ID */
} AMBA_OD_TRACKER_s;

typedef struct {
    AMBA_OD_TRACKER_s Trackers[AMBA_OD_2DBBX_MAX_BBX_NUM];           /**< Tracker list */
} AMBA_OD_TRACKER_LIST_s;

/**
* Create tracker handler.
* @param [in] CamPos: Camera position.
* @param [in] pWorkingBuf: Handler working buffer.
* @param [out] pTkHdlr: Tracker handler.
* @return ErrorCode
*/
UINT32 AmbaOD_CreateTrackerHandler(UINT32 CamPos, const void *pWorkingBuf, void **pTkHdlr);

/**
* Create tracker handler.
* @param [in] CamPos: Camera position.
* @param [in] AppType: Application type.
* @param [in] pWorkingBuf: Handler working buffer.
* @param [out] pTkHdlr: Tracker handler.
* @return ErrorCode
*/
UINT32 AmbaOD_CreateTrackerHandlerV1(UINT32 CamPos, UINT32 AppType, const void *pWorkingBuf, void **pTkHdlr);

/**
* Destroy tracker handler.
* @param [in] pTkHdlr: Tracker handler.
* @return ErrorCode
*/
UINT32 AmbaOD_DestroyTrackerHandlerV1(const void *pTkHdlr);

/**
* Get tracker working buffer size.
* @param [out] pSize: Handler working buffer size.
* @return ErrorCode
*/
UINT32 AmbaOD_GetTrackerBufSize(SIZE_t *pSize);

/**
* Allocate the ID to the detected objects.
* @param [in] pTkHdlr: Tracker handler.
* @param [in] pInODList: Detected objects list. NOTE: The list will be allocated ID
* @param [in] Threshold: Threshold of tracker ranging from 0.0 ~ 1.0. NOTE: 0.5 is recommended
* @param [out] pOutTrackerList: Detailed information of detected objects.
* @return ErrorCode
*/
UINT32 AmbaOD_Tracker(const void *pTkHdlr, AMBA_OD_2DBBX_LIST_s *pInODList, const DOUBLE Threshold, AMBA_OD_TRACKER_LIST_s *pOutTrackerList);

#endif //AMBA_OD_TRACKER_H
