/**
 *  @file AmbaSR_SegCrossVehicle.h
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
 *  @details Definitions & Constants for segmentation crossing traffic vehicle detection
 *
 */

#ifndef AMBA_SR_SEG_CROSS_VEHICLE_H
#define AMBA_SR_SEG_CROSS_VEHICLE_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif
#ifndef AMBA_CALIB_AVM_API_H
#include "AmbaCalib_AVMIF.h"
#endif
#ifndef AMBA_SURROUND_H
#include "AmbaSurround.h"
#endif

/** Define for fisheye camera channel index */
#define AMBA_SR_SCV_CHANNEL_FRONT    (0U)
#define AMBA_SR_SCV_CHANNEL_REAR     (1U)
#define AMBA_SR_SCV_CHANNEL_MAX_NUM  (2U)

/** Maximum of idx list number */
#define AMBA_SR_SCV_MAX_IDX_LIST (32U)

#define AMBA_SR_SCV_MAX_MODEL_VT_NUM (8U)
#define AMBA_SR_SCV_MAX_OBJ_NUM (10U)

typedef struct {
    UINT8* pBuf;      /**< Buffer address */
    UINT32 Pitch;     /**< Buffer pitch which is the width to change to next image row */
    UINT32 Width;     /**< Image width */
    UINT32 Height;    /**< Image height */
} AMBA_SR_SCV_BUF_INFO_s;

typedef struct {
    UINT32 SrcW;    /**< Source image width */
    UINT32 SrcH;    /**< Source image height */
    UINT32 RoiW;    /**< ROI width at source image */
    UINT32 RoiH;    /**< ROI height at source image */
    UINT32 RoiX;    /**< ROI X offset at source image */
    UINT32 RoiY;    /**< ROI Y offset at source image */
} AMBA_SR_SCV_CROP_INFO_s;

typedef struct {
    AMBA_SR_SCV_BUF_INFO_s SegBufInfo;              /**< Segmentation buffer information */
    AMBA_SR_SCV_CROP_INFO_s SegBufCropInfo;         /**< Segmentation buffer crop information */
    UINT64 TimeStamp;                               /**< Time stamp of segmentation buffer */
} AMBA_SR_SCV_PROC_INPUT_DATA_s;

typedef struct {
    UINT32 ObjId;                                                       /**< Object index >*/
    UINT32 VerticeStatus;                                               /**< VerticeStatus, one bit means the status of one vertice. 1 is valided */
    AMBA_CAL_POINT_INT_2D_s Vertices[AMBA_SR_SCV_MAX_MODEL_VT_NUM];     /**< Points at source image domain in pixel */
} AMBA_SR_SCV_OBJECT_2D_DATA_s;

typedef struct {
    UINT32  TotalNum;                                                    /**< Total Number of detected Object in captured frame */
    AMBA_SR_OBJECT_DATA_s  SRObject[AMBA_SR_SCV_MAX_OBJ_NUM];            /**< The Info of each detected Object */
    AMBA_SR_SCV_OBJECT_2D_DATA_s  ObjData2D[AMBA_SR_SCV_MAX_OBJ_NUM];    /**< The 2D Info of each detected Object */
} AMBA_SR_SCV_OBJECT_DATA_LIST_s;

typedef struct {
    INT32 IdxNum;                               /**< Valid vehicle index number */
    UINT8 IdxList[AMBA_SR_SCV_MAX_IDX_LIST];    /**< Valid vehicle index in segmentation buffer */
} AMBA_SR_SCV_SEG_IDX_INFO_s;

typedef struct {
    UINT8* pWrkBuf;                               /**< Working buffer address */
    UINT32 WrkBufSize;                            /**< Working buffer size */
    UINT32 SegBufImgW;                            /**< Segmentation image width */
    UINT32 SegBufImgH;                            /**< Segmentation image height */
    INT32 Clock;                                  /**< System clock in KHz */
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;          /**< Config for self car spec */
    AMBA_SR_SCV_SEG_IDX_INFO_s VehicleIdxInfo;    /**< Vehicle index in segmentation image */
} AMBA_SR_SCV_CFG_DATA_s;

typedef struct {
    AMBA_CAL_AVM_CALIB_DATA_s CalibData;     /**< AVM calibration data */
    AMBA_CAL_AVM_PARK_AST_CFG_V1_s Cfg;      /**< AVM parking assistance config V1 */
} AMBA_SR_SCV_CAL_AVM_CFG_s;


UINT32 AmbaSR_SCV_Process(UINT32 Channel, const AMBA_SR_SCV_PROC_INPUT_DATA_s* pInData, AMBA_SR_SCV_OBJECT_DATA_LIST_s* pResultData);

UINT32 AmbaSR_SCV_GetDefaultConfig(UINT32 SegBufImgW, UINT32 SegBufImgH, AMBA_SR_SCV_CFG_DATA_s* pDefCfgData);

UINT32 AmbaSR_SCV_SetInitConfig(UINT32 Channel, const AMBA_SR_SCV_CFG_DATA_s* pCfgData, const AMBA_SR_SCV_CAL_AVM_CFG_s* pCalCfg);

#endif
