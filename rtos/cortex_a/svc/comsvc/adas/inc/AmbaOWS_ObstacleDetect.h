/**
 *  @file AmbaOWS_ObstacleDetect.h
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
 *  @details Definitions & Constants for obstacle warning system API
 *
 */



#ifndef AMBA_OWS_OBSTACLE_DETECT_H
#define AMBA_OWS_OBSTACLE_DETECT_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif
#ifndef AMBA_CALIB_EMIRROR_API_H
#include "AmbaCalib_EmirrorDef.h"
#endif
#ifndef AMBA_CALIB_AVM_API_H
#include "AmbaCalib_AVMIF.h"
#endif
#ifndef AMBA_SURROUND_H
#include "AmbaSurround.h"
#endif

/** Camera channel for input */
#define AMBA_OWS_CHANNEL_FRONT     (0U)
#define AMBA_OWS_CHANNEL_BACK      (1U)
#define AMBA_OWS_CHANNEL_LEFT      (2U)
#define AMBA_OWS_CHANNEL_RIGHT     (3U)
#define AMBA_OWS_CHANNEL_MAX_NUM   (4U)

/** Maximum split number of width */
#define AMBA_OWS_MAX_PATH_NUM (16U)

/** Maximum stage number*/
#define AMBA_OWS_MAX_STAGE_NUM (16U)

/** Maximum of idx list number */
#define AMBA_OWS_MAX_IDX_LIST (32U)


typedef struct {
    UINT8* pBuf;     /**< Buffer address */
    UINT32 Pitch;    /**< Buffer pitch which is the width to change to next image row */
    UINT32 Width;    /**< Image width */
    UINT32 Height;   /**< Image height */
} AMBA_OWS_BUF_INFO_s;

typedef struct {
    UINT32 SrcW;    /**< Source image width */
    UINT32 SrcH;    /**< Source image height */
    UINT32 RoiW;    /**< ROI width at source image */
    UINT32 RoiH;    /**< ROI height at source image */
    UINT32 RoiX;    /**< ROI X offset at source image */
    UINT32 RoiY;    /**< ROI Y offset at source image */
} AMBA_OWS_CROP_INFO_s;

/** Segmentation obstacle detection process input data */
typedef struct {
    AMBA_OWS_BUF_INFO_s* pSegBufInfo;           /**< Segmentation buffer information */
    AMBA_OWS_CROP_INFO_s* pSegBufCropInfo;      /**< Segmentation buffer crop information */
    UINT64 TimeStamp;                           /**< Time stamp of segmentation buffer */
} AMBA_OWS_PROC_INPUT_DATA_s;

typedef struct {
    INT32 IdxNum;                            /**< Valid obstacle index number */
    UINT8 IdxList[AMBA_OWS_MAX_IDX_LIST];    /**< Valid obstacle index in segmentation buffer */
} AMBA_OWS_SEG_IDX_INFO_s;

typedef struct {
    UINT32 DetectRange;                       /**< Detected width of warning zone */
    UINT32 DetectDistanceMax;                 /**< Detected distance of warning zone */
    UINT32 DetectDistanceMin;                 /**< Minimum detected distance of warning zone */
    UINT32 PathNum;                           /**< The split number of detected width */
    UINT32 StageNum;                          /**< Total stage number in detected distance */
    INT32 Sensitivity;                        /**< The dectected sensitivity of obstacle from segmentation*/
    INT32 EnableStabilize;                    /**< The flag for whether enable stabilize */
    INT32 DelayTolerance;                     /**< The maximum tolerance for delay used on stabilize. unit: msec*/
} AMBA_OWS_DETECT_CFG_s;

/** Initial configuration data  */
typedef struct {
    UINT8* pWrkBuf;                                             /**< Working buffer address */
    UINT32 WrkBufSize;                                          /**< Working buffer size */
    UINT32 SegBufImgW;                                          /**< Segmentation image width */
    UINT32 SegBufImgH;                                          /**< Segmentation image height */
    INT32 Clock;                                                /**< System clock in KHz */
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;                        /**< Config for self car spec */
    AMBA_OWS_DETECT_CFG_s DetectCfg[AMBA_OWS_CHANNEL_MAX_NUM];  /**< Detected configs*/
    AMBA_OWS_SEG_IDX_INFO_s ObsIdxInfo;                         /**< Obstacle index in segmentation image */
} AMBA_OWS_CFG_DATA_s;

typedef struct {
    AMBA_CAL_AVM_CALIB_DATA_s CalibData[AMBA_OWS_CHANNEL_MAX_NUM];     /**< AVM calibration data*/
    AMBA_CAL_AVM_PARK_AST_CFG_V1_s Cfg[AMBA_OWS_CHANNEL_MAX_NUM];      /**< AVM parking assistance config V1*/
} AMBA_OWS_CAL_AVM_CFG_s;

typedef struct {
    INT32 X;    /**< Point X at source image domain in pixel */
    INT32 Y;    /**< Point Y at source image domain in pixel */
} AMBA_OWS_PNT_2D_s;

typedef struct {
    DOUBLE X;    /**< Point X at world domain in mm */
    DOUBLE Y;    /**< Point Y at world domain in mm */
    DOUBLE Z;    /**< Point Z at world domain in mm */
} AMBA_OWS_PNT_3D_s;

typedef struct {
    AMBA_OWS_PNT_2D_s Pnt2D[2];    /**< points in 2D*/
    AMBA_OWS_PNT_3D_s Pnt3D[2];    /**< points in 3D*/
} AMBA_OWS_LINESEG_s;

typedef struct {
    UINT32 PathNum;                                                                 /**< The split number of detected width */
    UINT32 StageNum;                                                                /**< Total stage number in detected distance */
    AMBA_OWS_LINESEG_s StageLine[AMBA_OWS_MAX_PATH_NUM][AMBA_OWS_MAX_STAGE_NUM];    /**< Grid points of warning zone , size:PathNum*StageNum */
} AMBA_OWS_GRID_INFO_s;

typedef struct {
    UINT32 PathNum;                                                 /**< The split number of detected width */
    UINT32 StageNum;                                                /**< Total stage number in detected distance */
    UINT32 ObsStage[AMBA_OWS_MAX_PATH_NUM];                         /**< Warning stage of each path*/
    DOUBLE ObsDistance[AMBA_OWS_MAX_PATH_NUM];                      /**< Distance of obstacle of each path */
    AMBA_OWS_LINESEG_s ObsStageLine[AMBA_OWS_MAX_PATH_NUM];         /**< Line segment of warning stage of each path*/
} AMBA_OWS_RESULT_s;


UINT32 AmbaOWS_Process(UINT32 Channel, const AMBA_OWS_PROC_INPUT_DATA_s* pInData, AMBA_OWS_RESULT_s* pResultData);

UINT32 AmbaOWS_GetDefaultConfig(UINT32 SegBufImgW, UINT32 SegBufImgH, AMBA_OWS_CFG_DATA_s* pDefCfgData);

UINT32 AmbaOWS_SetInitConfig(const AMBA_OWS_CFG_DATA_s* pCfgData, const AMBA_OWS_CAL_AVM_CFG_s* pCalCfg);

UINT32 AmbaOWS_GetGridInfo(UINT32 Channel, AMBA_OWS_GRID_INFO_s* pGridData);

#endif
