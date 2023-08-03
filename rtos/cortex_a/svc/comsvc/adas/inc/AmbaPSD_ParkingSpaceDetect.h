/**
 *  @file AmbaPSD_ParkingSpaceDetect.h
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
 *  @details Definitions & Constants for Segmentation lane detection API
 *
 */

#ifndef AMBA_PARKING_SPACE_DETECT_H
#define AMBA_PARKING_SPACE_DETECT_H


#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ADAS_ERROR_CODE_H
#include "AmbaAdasErrorCode.h"
#endif

#ifndef AMBA_CALIB_AVM_API_H
#include "AmbaCalib_AVMIF.h"
#endif


/** Define for fisheye camera channel index */
#define AMBA_PSD_CHANNEL_FRONT                      (0U)
#define AMBA_PSD_CHANNEL_BACK                       (1U)
#define AMBA_PSD_CHANNEL_LEFT                       (2U)
#define AMBA_PSD_CHANNEL_RIGHT                      (3U)
#define AMBA_PSD_CHANNEL_MAX_NUM                    (4U)

/** Define for invalid class index */
#define AMBA_PSD_INVALID_CLASS_IDX         (0xFFFFFFFFU)

/** Maximum of obstacle idx list number */
#define AMBA_PSD_MAX_OBSTACLE_LIST_NUM             (32U)

/** Maximum of parking corner number */
#define AMBA_PSD_MAX_CORNER_NUM                   (128U)

/** Maximum of parking space number */
#define AMBA_PSD_MAX_SPACE_NUM                     (64U)

/** Define for status of parking space */
#define AMBA_PSD_SPACE_STATUS_OCCUPIED              (0U)
#define AMBA_PSD_SPACE_STATUS_AVAILABLE             (1U)

/** Define for corner position in parking space */
#define AMBA_PSD_SPACE_CORNER_CT                    (0U)    /**< Close top corner */
#define AMBA_PSD_SPACE_CORNER_CB                    (1U)    /**< Close Bottom corner */
#define AMBA_PSD_SPACE_CORNER_FB                    (2U)    /**< Far Bottom corner */
#define AMBA_PSD_SPACE_CORNER_FT                    (3U)    /**< Far top corner */
#define AMBA_PSD_SPACE_CORNER_MAX                   (4U)    /**< maximum number of space corners */

/** Maximum of line number in one space */
#define AMBA_PSD_SPACE_LINE_C                       (0U)    /**< close line */
#define AMBA_PSD_SPACE_LINE_T                       (1U)    /**< top line */
#define AMBA_PSD_SPACE_LINE_B                       (2U)    /**< bottom line */
#define AMBA_PSD_SPACE_LINE_MAX                     (3U)    /**< maximum number of space lines  */

/** Maximum of point number in one line */
#define AMBA_PSD_MAX_LINE_PNT_NUM                  (16U)

/** Define for status of parking space corner */
#define AMBA_PSD_CORNER_STATUS_NONE                 (0U)
#define AMBA_PSD_CORNER_STATUS_DETECT               (1U)
#define AMBA_PSD_CORNER_STATUS_ESTIMATE             (2U)


/********************************************************************* */

typedef struct {
    UINT32 BackGroundIdx;                                      /**< Index for background */
    UINT32 RoadIdx;                                            /**< Index for road */
    UINT32 CornerIdx;                                          /**< Index for Corner of parking line */
    UINT32 ParkingLineIdx;                                     /**< Index for parking line  */
    UINT32 ObstacleIdxNum;                                     /**< Total index number for obstacle  */
    UINT32 ObstacleIdxList[AMBA_PSD_MAX_OBSTACLE_LIST_NUM];    /**< Index list for obstacle */
} AMBA_PSD_CLASS_INFO_s;


typedef struct {
    UINT32 Clock;                           /**< System Clock in KHz, time(ms) = TimeStamp/Clock */
    UINT8* pWrkBuf;                         /**< Working buffer address */
    UINT32 WrkBufSize;                      /**< Working buffer size */
    UINT32 SegBufImgW;                      /**< Segmentation image width */
    UINT32 SegBufImgH;                      /**< Segmentation image height */
    UINT32 CarSize[2];                      /**< Self car size, [width,lengh], units:mm */
    AMBA_PSD_CLASS_INFO_s SegClassInfo;     /**< Class information of segmentation buffer */
} AMBA_PSD_CFG_s;

/********************************************************************* */

typedef struct {
    UINT8* pBuf;      /**< Buffer address */
    UINT32 Pitch;     /**< Buffer pitch which is the width to change to next image row */
    UINT32 Width;     /**< Image width */
    UINT32 Height;    /**< Image height */
} AMBA_PSD_BUF_INFO_s;

typedef struct {
    UINT32 SrcW;    /**< Source image width */
    UINT32 SrcH;    /**< Source image height */
    UINT32 RoiW;    /**< ROI width at source image */
    UINT32 RoiH;    /**< ROI height at source image */
    UINT32 RoiX;    /**< ROI X offset at source image */
    UINT32 RoiY;    /**< ROI Y offset at source image */
} AMBA_PSD_CROP_INFO_s;

typedef struct {
    AMBA_PSD_BUF_INFO_s SegBufInfo;         /**< Segmentation buffer information*/
    AMBA_PSD_CROP_INFO_s SegBufCropInfo;    /**< Crop information of segmentation buffer */
    UINT32 TimeStamp;                       /**< Time stamp of segmentation buffer */
} AMBA_PSD_PROC_INPUT_DATA_s;

/********************************************************************* */

typedef struct {
    DOUBLE X;    /**< Point X at world domain in mm */
    DOUBLE Y;    /**< Point Y at world domain in mm */
    DOUBLE Z;    /**< Point Z at world domain in mm */
} AMBA_PSD_PNT_3D_s;

typedef struct {
    INT32 X;    /**< Point X at source image domain in pixel */
    INT32 Y;    /**< Point Y at source image domain in pixel */
} AMBA_PSD_PNT_2D_s;

typedef struct {
    UINT32 PntNum;                                         /**< Line points number */
    AMBA_PSD_PNT_2D_s Pnt2D[AMBA_PSD_MAX_LINE_PNT_NUM];    /**< Line points position at Source image domain */
    AMBA_PSD_PNT_3D_s Pnt3D[AMBA_PSD_MAX_LINE_PNT_NUM];    /**< Line points position at world domain */
} AMBA_PSD_LINE_INFO_s;

typedef struct {
    UINT32 Status;                                          /**< Status of this parking space */
    UINT32 Degree;                                          /**< Angle of parking space in degree (0 for parallel to car, 90 for vertical to car)*/
    UINT32 Width;                                           /**< Width of parking space */
    UINT32 Length;                                          /**< Length of parking space */
    UINT32 CornerID[AMBA_PSD_SPACE_CORNER_MAX];             /**< Corner ID */
    UINT32 CornerUID[AMBA_PSD_SPACE_CORNER_MAX];            /**< Corner UID (preserved for Tracker) */
    AMBA_PSD_LINE_INFO_s Line[AMBA_PSD_SPACE_LINE_MAX];       /**< Parking space line */
} AMBA_PSD_PARKING_SPACE_INFO_s;


typedef struct {
    UINT32 ID;                       /**< Corner ID */
    AMBA_PSD_PNT_2D_s Pnt2D;         /**< Corner positin at source image domain */
    AMBA_PSD_PNT_3D_s Pnt3D;         /**< Corner positin at world domain */
    DOUBLE LineAngle[4];             /**< Line angle connected to the corner in PI, invalid when value > PI */
    UINT32 UID;                      /**< cross ch UID (preserved for Tracker) */
    UINT32 CornerStatus;             /**< detect : 1 / estimate : 2 */
} AMBA_PSD_PARKING_CORNER_INFO_s;

typedef struct {
    UINT32 Channel;                                                    /**< Fisheye camera channel index, please refer to AMBA_PSD_CHANNEL_XXX */
    UINT32 CornerNum;                                                  /**< Number of Detected parking corner */
    AMBA_PSD_PARKING_CORNER_INFO_s Corner[AMBA_PSD_MAX_CORNER_NUM];    /**< Parking corner information */
    UINT32 SpaceNum;                                                   /**< Number of Detected parking space */
    AMBA_PSD_PARKING_SPACE_INFO_s Space[AMBA_PSD_MAX_SPACE_NUM];       /**< Parking space information */
} AMBA_PSD_PROC_OUTPUT_DATA_s;

/********************************************************************* */

UINT32 AmbaPSD_GetDefaultConfig(UINT32 SegBufImgW, UINT32 SegBufImgH, AMBA_PSD_CFG_s* pPsdCfg);

UINT32 AmbaPSD_SetInitConfig(const AMBA_PSD_CFG_s* pPsdCfg);

UINT32 AmbaPSD_Process(UINT32 Channel, const AMBA_PSD_PROC_INPUT_DATA_s* pPsdInData,
                       const AMBA_CAL_AVM_CALIB_DATA_s* pCalData,
                       const AMBA_CAL_AVM_PARK_AST_CFG_V1_s* pPACfgV1,
                       AMBA_PSD_PROC_OUTPUT_DATA_s *pPsdOutData);

#endif
