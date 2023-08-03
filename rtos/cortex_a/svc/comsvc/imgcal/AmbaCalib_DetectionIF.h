/**
 *  @file AmbaCalib_DetectionIF.h
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
 */

#ifndef AMBA_CALIB_DETECTION_IF_H
#define AMBA_CALIB_DETECTION_IF_H

#include "AmbaCalib_Def.h"
#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_DetectionDef.h"
#define RECT_CORNER_NUM 4U
#define DET_PATTERN_MAX 6U
#define CHECK_BOARD_MAX_CORNER_NUM (4000U)

#define DET_PATTERN_MODE_CHECKBOARD (0U)
#define DET_PATTERN_MODE_CIRCLE     (1U)

typedef struct {
    DOUBLE PowerFilter;            /**< adjust contrast for image. 0 for disable */
    DOUBLE GammaStrFilter;          /**< adjust Gamma for image. 0 for disable */
} AMBA_CAL_DET_TUNE_s;

typedef struct {
    UINT32 MinContourArea;                     /**< Min Area of contour from detection */
    UINT32 MaxContourArea;                     /**< Max Area of contour from detection */
} AMBA_CAL_DET_FILTER_s;

typedef struct {
    AMBA_CAL_ROI_s Area;               /**< Check region */
    AMBA_CAL_DET_TUNE_s TuneCfg;       /**< Tuning image for detection*/
    AMBA_CAL_DET_FILTER_s FilterCfg;   /**< Filter detection result*/
    UINT32 MaxEpsilon;                 /**< Max for approxPolyDP, Default: 15 */
} AMBA_CAL_DET_REGION_INFO_s;

typedef struct {
    UINT8 *pSrcAddr;
    AMBA_CAL_SIZE_s ImgSize;
} AMBA_CAL_DET_SRC_s;

typedef struct {
    AMBA_CAL_DET_SRC_s Src;
    AMBA_CAL_DET_REGION_INFO_s DetectRegion; /**< Check region */
    UINT32 (*pDebugReport)(UINT8 *pImageAddr, UINT32 Width, UINT32 Height, UINT32 StepId);
    UINT32 DebugId;                             /**< debug flag */
} AMBA_CAL_DET_RECT_DET_CFG_s;

typedef struct {
    AMBA_CAL_DET_SRC_s Src;
    DOUBLE QualityLevel;
    UINT32 MinDistance;
} AMBA_CAL_DET_CHECKBOARD_DET_CFG_s;

typedef struct {
    AMBA_CAL_DET_SRC_s Src;
    UINT32 MinContourLength;
    UINT32 MaxContourLength;
    UINT32 MaskRadius;
} AMBA_CAL_DET_CIRCLE_DET_CFG_s;

typedef struct {
    UINT32 DetPatternType;
    AMBA_CAL_DET_CHECKBOARD_DET_CFG_s CheckBoardCfg;
    AMBA_CAL_DET_CIRCLE_DET_CFG_s CircleCfg;
} AMBA_CAL_DET_CFG_s;

typedef struct {
    DOUBLE StandardDeviation;  //StD of inner contour
} AMBA_CAL_DET_ANALYSIS_s;

typedef struct {
    AMBA_CAL_POINT_DB_2D_s Corners[RECT_CORNER_NUM];
    AMBA_CAL_POINT_DB_2D_s Center;
    AMBA_CAL_DET_ANALYSIS_s RectAnalysis;
} AMBA_CAL_DET_RECT_s;

typedef struct {
    AMBA_CAL_DET_RECT_s DetPattern[DET_PATTERN_MAX];
    UINT32 DetectNum;
} AMBA_CAL_DET_RECT_REPORT_s;

/**
* This API is used to get required size of detection working buffer and result buffer for pattern detection with rectangle calibration chart. 
* @param [out] pSize Returned required total size of working buffer forAmbaCal_DetRectPattern() API and storage for saving detection result.
* @return ErrorCode
*/
UINT32 AmbaCal_DetGetDetRectBufSize(SIZE_t *pSize);

/**
* This API is used to detect rectangle calibration charts in input image and reported related information of each detected rectangle. 
* Buffer for saving report data was contained in working buffer. That is to say, Pointer pOutCornersAddr will point to a location in pWorkingbuf.
* This API returns error if the input parameter is invalid.
* @param [in] pDetectCalInfo Pattern detection configuration for rectangle calibration chart case. Please refer to AMBA_CAL_DET_RECT_DET_CFG_s for more details. 
* @param [in] pWorkingbuf Working buffer provided by user. Required buffer size can be queried byAmbaCal_DetGetDetRectBufSize(). 
* @param [out] pOutCornersAddr Returned buffer address for saving report data of detection result. Please refer to AMBA_CAL_DET_RECT_REPORT_s for more details. 
* @return ErrorCode
*/
UINT32 AmbaCal_DetRectPattern(const AMBA_CAL_DET_RECT_DET_CFG_s *pDetectCalInfo, const void *pWorkingbuf, AMBA_CAL_DET_RECT_REPORT_s **pOutCornersAddr);

/**
* This API is used to get required size of detection working buffer for checker board pattern detection. 
* @param [out] pSize Returned required total size of working buffer for AmbaCal_DetCheckboardCorners().
*/
void AmbaCal_DetGetPetDetBufSize(SIZE_t *pSize);

/**
* This API is used to detect the corner of checker board. 
* This API returns error if the input parameter is invalid.
* @param [in] pCfg Pattern detection configuration for rectangle calibration chart case. Please refer to AMBA_CAL_DET_CHECKBOARD_DET_CFG_s for more details. 
* @param [in] pWorkingbuf Working buffer provided by user. 
* @param [out] pCorners Output corners position. Please refer to AMBA_CAL_POINT_INT_2D_s for more details.
* @param [out] pCornerNum Output corner number
* @return ErrorCode
*/
UINT32 AmbaCal_DetCheckboardCorners(const AMBA_CAL_DET_CHECKBOARD_DET_CFG_s *pCfg, const void *pWorkingbuf, AMBA_CAL_POINT_INT_2D_s *pCorners, UINT32 *pCornerNum);

/**
* This API is used to find the sub-pixel accurate location of the corner. 
* This API returns error if the input parameter is invalid.
* @param [in] pSrc Source Image. Please refer to AMBA_CAL_DET_SRC_s for more details. 
* @param [in] pCorners The pixel accurate location of the corner. Please refer to AMBA_CAL_POINT_INT_2D_s for more details.
* @param [in] CornerNum Corner number
* @param [out] pSubPixCorners Output sub-pixel accurate location of the corner. Please refer to AMBA_CAL_POINT_DB_2D_s  r more details.
* @return ErrorCode
*/
UINT32 AmbaCal_DetFindAllCornersSubPixel(const AMBA_CAL_DET_SRC_s *pSrc, const AMBA_CAL_POINT_INT_2D_s *pCorners, UINT32 CornerNum, AMBA_CAL_POINT_DB_2D_s *pSubPixCorners);

/**
* This API is used to find the circle corners accurate location of the corner. 
* This API returns error if the input parameter is invalid.
* @param [in] pCfg Source Image. Please refer to AMBA_CAL_DET_CIRCLE_DET_CFG_s for more details. 
* @param [in] pWorkingbuf Working buffer provided by user.
* @param [out] pCorners Output circle corners accurate location of the corner. Please refer to AMBA_CAL_POINT_DB_2D_s  r more details.
* @param [out] pCornerNum Corner number
* @return ErrorCode
*/
UINT32 AmbaCal_DetCircleCorners(const AMBA_CAL_DET_CIRCLE_DET_CFG_s *pCfg, const void *pWorkingbuf, AMBA_CAL_POINT_DB_2D_s *pCorners, UINT32 *pCornerNum);
/**
* This API is used to find the color type of the corners. 
* This API returns error if the input parameter is invalid.
* @param [in] pSrc Source Image. Please refer to AMBA_CAL_YUV_INFO_s for more details. 
* @param [in] pImgSize The size of the source image. Please refer to AMBA_CAL_SIZE_s for more details.
* @param [in,out] pCorners get pos of the corner and record the type. Please refer to AMBA_CAL_DET_CORNER_LIST_s more details.
* @return ErrorCode
*/
UINT32 AmbaCal_DetGetCircleCornersType(const AMBA_CAL_YUV_INFO_s *pSrc, const AMBA_CAL_SIZE_s *pImgSize, const AMBA_CAL_DET_CORNER_LIST_s *pCorners);

#endif //AMBA_CALIB_DETECTION_IF_H
