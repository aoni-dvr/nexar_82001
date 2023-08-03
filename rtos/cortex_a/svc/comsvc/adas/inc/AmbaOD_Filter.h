/**
 * @file AmbaOD_Filter.h
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
 *  @details AmbaOD_Filter
 *
 */

#ifndef AMBA_OD_FILTER_H
#define AMBA_OD_FILTER_H

#include "AmbaTypes.h"
#include "AmbaErrorCode.h"
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaOD_2DBbx.h"

#define FILTER_MAX_OBSRV_NUM           (AMBA_OD_2DBBX_MAX_BBX_NUM)
#define AMBA_OD_FILTER_RINGBUF_NUM     (5U)                           /**< The 2D bounding box ring buffer size for filter */

/* MODE */
#define FILTER_MODE_NONE               (0U)                           /**< Disable filter */
#define FILTER_MODE_FCWS               (1U)                           /**< FCWS filter mode */
#define FILTER_MODE_BSD_LEFT           (2U)                           /**< BSD left filter mode */
#define FILTER_MODE_BSD_RIGHT          (3U)                           /**< BSD right filter mode */
#define FILTER_MODE_EMIR_BACK          (4U)                           /**< EMIR back filter mode */
#define FILTER_MODE_EMIR_LEFT          (5U)                           /**< EMIR left filter mode */
#define FILTER_MODE_EMIR_RIGHT         (6U)                           /**< EMIR right filter mode, AKA RMG mode */
#define FILTER_MODE_EMIR_FRONT         (7U)                           /**< EMIR front filter mode */
#define FILTER_MODE_MAX                (8U)

/* Output data */
// <FCWS mode>
#define FILTER_FCWS_OUT_NUM            (4U)                          /**< The array size of output data */
#define FILTER_FCWS_OUT_X              (0U)                          /**< The filtered 2D bounding box x offset in current frame (T) */
#define FILTER_FCWS_OUT_Y              (1U)                          /**< The filtered 2D bounding box y offset in current frame (T) */
#define FILTER_FCWS_OUT_W              (2U)                          /**< The filtered 2D bounding box width in current frame (T) */
#define FILTER_FCWS_OUT_H              (3U)                          /**< The filtered 2D bounding box height in current frame (T) */
// <RMG mode>
#define FILTER_RMG_OUT_NUM            (4U)                          /**< The array size of output data */
#define FILTER_RMG_OUT_X              (0U)                          /**< The filtered 2D bounding box x offset in current frame (T) */
#define FILTER_RMG_OUT_Y              (1U)                          /**< The filtered 2D bounding box y offset in current frame (T) */
#define FILTER_RMG_OUT_W              (2U)                          /**< The filtered 2D bounding box width in current frame (T) */
#define FILTER_RMG_OUT_H              (3U)                          /**< The filtered 2D bounding box height in current frame (T) */
// <BSD mode>
#define FILTER_BSD_OUT_NUM             (4U)                          /**< The array size of output data */
#define FILTER_BSD_OUT_X               (0U)                          /**< The filtered 2D bounding box x offset in current frame (T) */
#define FILTER_BSD_OUT_Y               (1U)                          /**< The filtered 2D bounding box y offset in current frame (T) */
#define FILTER_BSD_OUT_W               (2U)                          /**< The filtered 2D bounding box width in current frame (T) */
#define FILTER_BSD_OUT_H               (3U)                          /**< The filtered 2D bounding box height in current frame (T) */


/* Observed data list */
typedef struct {
    void *ObsrvData[FILTER_MAX_OBSRV_NUM];
} AMBA_FILTER_OBSRV_LIST_s;

/**
* Get filter working buffer size.
* @param [in] Mode: Filter mode.
* @param [out] pSize: Filter working buffer size.
* @return ErrorCode
*/
UINT32 AmbaOD_GetFilterBufSize(const UINT32 Mode, SIZE_t *pSize);

/**
* Initialize filter working buffer.
* @param [in] pWorkingBuf: Filter working buffer.
* @param [in] Mode: Filter mode.
* @param [in] pCalibData: Camera calibration data.
* @return ErrorCode
*/
UINT32 AmbaOD_InitFilterWkgbuf(const void *pWorkingBuf, const UINT32 Mode, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalibData);

/**
* Get filter ring buffer size.
* @param [in] Mode: Filter mode.
* @param [out] pSize: Filter ring buffer size.
* @return ErrorCode
*/
UINT32 AmbaOD_GetOdRingBufSize(const UINT32 Mode, SIZE_t *pSize);

/**
* Initialize filter ring buffer.
* @param [in] pOdRingBuf: Filter ring buffer.
* @param [in] Mode: Filter mode.
* @return ErrorCode
*/
UINT32 AmbaOD_InitOdRingBuf(const void* pOdRingBuf, const UINT32 Mode);

/**
* Get observed data buffer size.
* @param [in] Mode: Filter mode.
* @param [out] pSize: Observed data buffer size.
* @return ErrorCode
*/
UINT32 AmbaOD_GetObsrvBufSize(const UINT32 Mode, SIZE_t *pSize);

/**
* Initialize observed data buffer.
* @param [in] pObsrvBuf: Observed data buffer.
* @param [in] Mode: Filter mode.
* @return ErrorCode
*/
UINT32 AmbaOD_InitObsrvBuf(const void* pObsrvBuf, const UINT32 Mode);

/**
* Uninitialize the filter moduel.
* @param [in] Mode: Filter mode.
* @return ErrorCode
*/
UINT32 AmbaOD_UninitFilter(UINT32 Mode);

/**
* Set filter active distance.
* @param [in] UserDist: User defined distance (Unit: millimeter).
* @param [in] Mode: Filter mode.
* @return ErrorCode
*/
UINT32 AmbaOD_SetFilterDist(const DOUBLE UserDist, const UINT32 Mode);

/**
* Bounding box ring buffer processing before executing "AmbaOD_BbxProc"
* @param [in] pInBbxList: Bounding box list.
* @param [in] Mode: Filter mode.
* @param [out] pOutObsrvList: Observed data list.
* @return ErrorCode
*/
UINT32 AmbaOD_RingProc(const AMBA_OD_2DBBX_LIST_s *pInBbxList, const UINT32 Mode, AMBA_FILTER_OBSRV_LIST_s *pOutObsrvList);

/**
* Bounding box processing for ADAS features.
* @param [in] pObsrvData: Temporal information of bounding box.
* @param [in] Mode: Filter mode.
* @param [out] pOutData: Processed bounding box data.
* @return ErrorCode
*/
UINT32 AmbaOD_BbxProc(const void *pObsrvData, const UINT32 Mode, DOUBLE *pOutData);

/**
* Initialize filter working buffer.
* @param [in] Mode: Filter mode.
* @param [in] pCalibData: Camera calibration data.
* @return ErrorCode
*/
UINT32 AmbaOD_UpdateFilterCalCfg(UINT32 Mode, const AMBA_CAL_EM_CALC_COORD_CFG_s *pCalibData);

#endif //AMBA_OD_FILTER_H
