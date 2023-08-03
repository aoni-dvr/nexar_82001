/**
 *  @file AmbaSR_Lane.h
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
 *  @details Header file of the surrounding lane reconstruction
 *
 */
#ifndef AMBA_SR_LANE_H
#define AMBA_SR_LANE_H

#ifndef AMBA_SURROUND_H
#include "AmbaSurround.h"
#endif
#ifndef AMBA_VEHICLES_H
#include "Amba_Vehicles.h"
#endif
#ifndef AMBA_CALIB_EMIRROR_API_H
#include "AmbaCalib_EmirrorDef.h"
#endif
#include "AmbaSEG_BufferDef.h"

/**
* Define for Line ID
*/
#define AMBA_SR_LANE_LINE_LEFT0          (0U)
#define AMBA_SR_LANE_LINE_RIGHT0         (1U)
#define AMBA_SR_LANE_LINE_CENTER         (2U)
#define AMBA_SR_LANE_LINE_MAX_NUM        (3U)

#define AMBA_SR_LANE_EGO                 (0U)
#define AMBA_SR_LANE_LEFT0               (1U)
#define AMBA_SR_LANE_RIGHT0              (2U)
#define AMBA_SR_LANE_MAX_NUM             (3U)

/**
* Define for LineStat in AMBA_SR_LANE_PNT_DATA_s/AMBA_SR_LANE_FUNC_DATA_s
*/
#define AMBA_SR_LANE_LINE_STAT_NONE   (0xFFFFFFFFU)
#define AMBA_SR_LANE_LINE_STAT_VALID  (1U)

/**
* Define for TrackId in AMBA_SR_LANE_FUNC_DATA_s/AMBA_SR_LANE_MEMBER_s
*/
#define AMBA_SR_LANE_TRACK_NONE   (0xFFFFFFFFU)

/** Maximum of line points number */
#define AMBA_SR_MAX_PNT_NUM (32U)

/** The index of configurable parameter */
#define AMBA_SR_LANE_CFG_HOUGH_RADIUS    (0x1U)    /**< define for hough search range of radius, means percentage of segmentation width, format is double, range is [6.25, 20.0] */
#define AMBA_SR_LANE_CFG_CALIBRATION     (0x2U)    /**< calibration data, format is AMBA_CAL_EM_CALC_COORD_CFG_s */

/** The index of V2 configurable parameter */
#define AMBA_SR_LANE_CFG_V2_HOUGH_RADIUS    (0x1U)    /**< define for hough search range of radius, means percentage of segmentation width, format is double, range is [6.25, 20.0] */
#define AMBA_SR_LANE_CFG_V2_CALIB_CFG       (0x2U)    /**< calibration config, format is AMBA_SR_LANE_CALIB_CFG_V2_s */

typedef struct {
    INT32 X;    /**< Point X at source image domain in pixel */
    INT32 Y;    /**< Point Y at source image domain in pixel */
} AMBA_SR_LINE_PNT_2D_s;

typedef struct {
    UINT32 PntNum;                                         /**< Total point number to represent a line */
    AMBA_SR_LINE_PNT_2D_s PntData[AMBA_SR_MAX_PNT_NUM];    /**< Points at image domain */
} AMBA_SR_LINE_PNT_2D_DATA_s;

typedef struct {
    DOUBLE X;    /**< Point X at world domain in mm*/
    DOUBLE Y;    /**< Point Y at world domain in mm*/
    DOUBLE Z;    /**< Point Z at world domain in mm*/
} AMBA_SR_LINE_PNT_3D_s;

typedef struct {
    UINT32 PntNum;                                         /**< Total point number to represent a line */
    AMBA_SR_LINE_PNT_3D_s PntData[AMBA_SR_MAX_PNT_NUM];    /**< Points at world domain */
} AMBA_SR_LINE_PNT_3D_DATA_s;

typedef struct  {
    UINT32 LineStat[AMBA_SR_LANE_LINE_MAX_NUM];                       /**< line status */
    AMBA_SR_LINE_PNT_3D_DATA_s Line3D[AMBA_SR_LANE_LINE_MAX_NUM];     /**< line points in 3d */
} AMBA_SR_LANE_PNT_DATA_s;


typedef struct {
    UINT64 TimeStamp;                             /**< time stamp of result, invalid at current version */
    AMBA_SR_LANE_PNT_DATA_s LanePnt;              /**< lane data */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusInfo;    /**< canbus information, invalid at current version */
} AMBA_SR_LANE_DATA_s;

typedef struct  {
    DOUBLE Coef[5];    /**< line function coef and valid y range */
} AMBA_SR_LINE_FUNC_PARA_s;

typedef struct {
    UINT32 TrackId;                                                  /**< Tracking id */
    UINT32 LineStat[AMBA_SR_LANE_LINE_MAX_NUM];                      /**< line status */
    AMBA_SR_LINE_PNT_3D_DATA_s Line3D[AMBA_SR_LANE_LINE_MAX_NUM];    /**< line points in 3d */
    AMBA_SR_LINE_FUNC_PARA_s LinePara[AMBA_SR_LANE_LINE_MAX_NUM];    /**< line function parameters */
} AMBA_SR_LANE_FUNC_DATA_s;

typedef struct {
    UINT32 TrackId;                                                      /**< Tracking id */
    UINT32 LineStat[AMBA_SR_LANE_LINE_MAX_NUM];                          /**< Line status */
    UINT16 LaneType[AMBA_SR_LANE_LINE_MAX_NUM];                          /**< Lane type */
    AMBA_SR_LINE_PNT_2D_DATA_s Line2D[AMBA_SR_LANE_LINE_MAX_NUM];        /**< Line points in 2d */
    AMBA_SR_LINE_PNT_3D_DATA_s Line3D[AMBA_SR_LANE_LINE_MAX_NUM];        /**< Line points in 3d */
    AMBA_SR_LINE_FUNC_PARA_s LineFuncPara[AMBA_SR_LANE_LINE_MAX_NUM];    /**< Line function parameters */
} AMBA_SR_LANE_MEMBER_s;

typedef struct {
    UINT64 TimeStamp;                                          /**< Time stamp*/
    AMBA_SR_LANE_MEMBER_s LaneMember[AMBA_SR_LANE_MAX_NUM];    /**< Lane information: line state, points, function coefficient */
} AMBA_SR_LANE_RECONSTRUCT_INFO_s;

typedef struct {
    UINT32  Clock;                                /**< Clock Info used to calculate system time from Timetick */
    AMBA_SR_LANE_PNT_DATA_s *BuffAddr;            /**< Pointer to the start point of input Ring Buffer */
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;          /**< SelfCar Spec */
    UINT32  DataRingSz;                           /**< Ring Buffer Size */
} AMBA_SR_LANE_CFG_s;

/** Initial configuration data  */
typedef struct {
    UINT8* pWrkBuf;                               /**< Working buffer address */
    UINT32 WrkBufSize;                            /**< Working buffer size */
    INT32 DashLength;                             /**< Highway dash line length in mm, invalid at current version */
    INT32 DashSpace;                              /**< Highway dash line space in mm, invalid at current version */
    UINT32 SegBufImgW;                            /**< Segmentation image width. If it doesn't match pSegBufInfo->Width, process size will be downscaled to this size. It should be same to the SegBufImgW in AMBA_SR_LANE_QUERY_INFO_V2_s */
    UINT32 SegBufImgH;                            /**< Segmentation image height. If it doesn't match pSegBufInfo->Height, process size will be downscaled to this size. It should be same to the SegBufImgH in AMBA_SR_LANE_QUERY_INFO_V2_s */
    INT32 Clock;                                  /**< System clock in KHz */
    AMBA_SEG_CLASS_INFO LaneMarkIdxInfo;          /**< Lane mark index in segmentation image */
    AMBA_SEG_CLASS_INFO FreeSpaceIdxInfo;         /**< Free space index in segmentation image */
} AMBA_SR_LANE_CFG_V2_s;

typedef struct {
    AMBA_CAL_EM_CALC_COORD_CFG_V1_s CalibCfg;     /**< Calibration config, please refer to AmbaCalib_EmirrorDef.h */
    AMBA_CAL_EM_CALIB_INFO_DATA_V1_s CalibInfo;   /**< For supporting NN-3D data related work. Advance data for calibration. Generated by AmbaCal_EmGenCalibInfoV1() */
    AMBA_CAL_POINT_DB_2D_s FocalLength;           /**< For supporting NN-3D data related work. Focal length of lens; unit: cellsize */
} AMBA_SR_LANE_CALIB_CFG_V2_s;

typedef struct {
    UINT32 SegBufImgW;                            /**< Segmentation image width. If it doesn't match pSegBufInfo->Width, process size will be downscaled to this size. */
    UINT32 SegBufImgH;                            /**< Segmentation image height. If it doesn't match pSegBufInfo->Height, process size will be downscaled to this size. */
} AMBA_SR_LANE_QUERY_INFO_V2_s;

typedef struct {
    AMBA_SEG_BUF_INFO_s* pSegBufCategory;         /**< Segmentation category buffer */
    AMBA_SEG_BUF_INFO_s* pSegBufColor;            /**< Segmentation color buffer */
    AMBA_SEG_BUF_INFO_s* pSegBufType;             /**< Segmentation type buffer */
    AMBA_SEG_BUF_INFO_s* pSegBufInstance;         /**< Segmentation instance buffer */
    AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusInfo;  /**< Canbus information */
} AMBA_SR_LANE_PROC_IN_V2_s;

UINT32 AmbaSR_GetLaneFunction(const AMBA_SR_LANE_DATA_s* LaneData, AMBA_SR_LANE_FUNC_DATA_s* LaneFunc);
UINT32 AmbaSR_LaneSetInitConfig(const AMBA_SR_LANE_CFG_s* InitCfg);


UINT32 AmbaSR_Lane_GetDefaultConfig(UINT32 SegBufImgW, UINT32 SegBufImgH, AMBA_SR_LANE_CFG_V2_s* pDefCfgData);
UINT32 AmbaSR_Lane_SetInitConfig(const AMBA_SR_LANE_CFG_V2_s* pCfgData, const AMBA_CAL_EM_CALC_COORD_CFG_s* pCalCfg);
UINT32 AmbaSR_MultiLaneReconstruction(const AMBA_SEG_BUF_INFO_s* pInData, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusInfo, AMBA_SR_LANE_RECONSTRUCT_INFO_s* pLaneInfo);
UINT32 AmbaSR_Lane_GetConfig(UINT8 CfgId, void* pCfgVal);
UINT32 AmbaSR_Lane_SetConfig(UINT8 CfgId, const void* pCfgVal);


UINT32 AmbaSR_Lane_QueryV2(const AMBA_SR_LANE_QUERY_INFO_V2_s* pQrInfo, UINT32* pWrkBufSize);
UINT32 AmbaSR_Lane_GetDefaultCfgV2(AMBA_SR_LANE_CFG_V2_s* pDefCfgData);
UINT32 AmbaSR_Lane_InitV2(const AMBA_SR_LANE_CFG_V2_s* pCfgData, const AMBA_SR_LANE_CALIB_CFG_V2_s* pCalCfg);
UINT32 AmbaSR_Lane_GetConfigV2(UINT8 CfgId, void* pCfgVal);
UINT32 AmbaSR_Lane_SetConfigV2(UINT8 CfgId, const void* pCfgVal);
UINT32 AmbaSR_Lane_ProcessV2(const AMBA_SR_LANE_PROC_IN_V2_s* pInData, AMBA_SR_LANE_RECONSTRUCT_INFO_s* pLaneInfo);

#endif
