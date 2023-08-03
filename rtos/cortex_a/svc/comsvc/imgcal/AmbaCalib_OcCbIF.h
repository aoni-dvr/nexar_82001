/**
 *  @file AmbaCalib_OcCbIF.h
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
#ifndef AMBA_CALIB_OCCB_IF_H
#define AMBA_CALIB_OCCB_IF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_Camera.h"
#include "AmbaCalib_OcCbDef.h"
#include "AmbaCalib_DetectionDef.h"


typedef struct {
    INT16 *pCorner2dMap;
    AMBA_CAL_ROI_s Corner2dMapValidArea;
} AMBA_CAL_OCCB_ORGANIZED_CORNER_s;

/**
* This API is used to get checkerboard organizer working buffer size.��
* @param [out] pSize Checkerboard organizer working buffer size.
*/
void AmbaCal_OcCbGetOrganizeBufSize(SIZE_t *pSize);

/**
* This API is used to get separate lines working buffer size.��
* @param [out] pSize separate lines working buffer size.
*/
void AmbaCal_OcCbSeparateLinesBufSize(SIZE_t *pSize);

/**
* This API is used to organize the relationship of each corner.��
* @param [in] pCfg Organizer Config. Please refer to AMBA_CAL_OCCB_ORGANIZE_CFG_s more detail.
* @param [in] pCorners Checkerboard Corners position. Please refer to AMBA_CAL_DET_CORNER_LIST_s more detail.
* @param [in] pImgSize Checkerboard Image Size. Please refer to AMBA_CAL_SIZE_sr more detail.
* @param [in] pCalibBoard User reference calibration board. Please refer to AMBA_CAL_OCCB_CALIB_BOARD_s more detail.
* @param [in] pWorkBuf Working buffer provided by user.
* @param [out] pOrganizedCorners Organized Corners Information. Please refer to AMBA_CAL_OCCB_ORGANIZED_CORNER_sr more detail.
* @return ErrorCode
*/
UINT32 AmbaCal_OcCbOrganizeCornersNook(const AMBA_CAL_OCCB_ORGANIZE_CFG_s *pCfg,
        const AMBA_CAL_DET_CORNER_LIST_s *pCorners,
        const AMBA_CAL_SIZE_s *pImgSize,
        const AMBA_CAL_OCCB_CALIB_BOARD_s *pCalibBoard,
        const void *pWorkBuf,
        AMBA_CAL_OCCB_ORGANIZED_CORNER_s *pOrganizedCorners);

/**
* This API is used to organize the relationship of each corner.��
* @param [in] pCfg Organizer Config. Please refer to AMBA_CAL_OCCB_ORGANIZE_CFG_s more detail.
* @param [in] pCorners Checkerboard Corners position. Please refer to AMBA_CAL_POINT_DB_2D_sr more detail.
* @param [in] CornerNum Corner Number.
* @param [in] pImgSize Checkerboard Image Size. Please refer to AMBA_CAL_SIZE_sr more detail.
* @param [in] pWorkBuf Working buffer provided by user.
* @param [out] pOrganizedCorners Organized Corners Information. Please refer to AMBA_CAL_OCCB_ORGANIZED_CORNER_sr more detail.
* @return ErrorCode
*/
UINT32 AmbaCal_OcCbOrganizeCorners(const AMBA_CAL_OCCB_ORGANIZE_CFG_s *pCfg,
        AMBA_CAL_POINT_DB_2D_s *pCorners,
        UINT32 CornerNum,
        const AMBA_CAL_SIZE_s *pImgSize,
        const void *pWorkBuf,
        AMBA_CAL_OCCB_ORGANIZED_CORNER_s *pOrganizedCorners);

/**
* This API is used to get the working buffer size of the checkerboard optical center finder.��
* @param [out] pSize Checkerboard optical center finder working buffer size.
*/
void AmbaCal_OcCbGetOCFinderBufSize(SIZE_t *pSize);

/**
* This API is used to find the optical center based on the checkerboard corner position.��
* @param [in] pCamSpec Camera spec. Please refer to AMBA_CAL_CAM_sr more detail.
* @param [in] pSearchRange The optical center search range. Please refer to AMBA_CAL_SIZE_sr more detail.
* @param [in] pCorners Checkerboard Corners Position. Please refer to AMBA_CAL_POINT_DB_2D_sr more detail.
* @param [in] pCorner2dMap The 100x100 Corner relationship map.
* @param [in] pCorner2dMapValidArea The valid area of the corner relationship map. Please refer to AMBA_CAL_ROI_sr more detail.
* @param [in] pWorkBuf Working buffer provided by user.
* @param [out] pOpticalCenter Optical center position. Please refer to AMBA_CAL_POINT_DB_2D_sr more detail.
* @return ErrorCode
*/
UINT32 AmbaCal_OcCbOpticalCenterFinder(const AMBA_CAL_CAM_s *pCamSpec,
        const AMBA_CAL_SIZE_s *pSearchRange,
        const AMBA_CAL_POINT_DB_2D_s *pCorners,
        const INT16 *pCorner2dMap,
        const AMBA_CAL_ROI_s *pCorner2dMapValidArea,
        const void *pWorkBuf,
        AMBA_CAL_POINT_DB_2D_s *pOpticalCenter);

/**
* This API is used to calculate image position and real world coordinate information.��
* @param [in] BaseCornerWorldPos Position of red dot on stereo calibration board. (In world coordination). Please refer to AMBA_CAL_POINT_DB_3D_s for more details.��
* @param [in] pSrcImg Source image.
* @param [in] SrcImgSize Image Size. Please refer to AMBA_CAL_SIZE_sr more detail.
* @param [in] pCorners Detected corners position on image. (In image coordination) (Unit: pixel). Please refer to AMBA_CAL_POINT_DB_2D_sr more detail.
* @param [in] CornerNum Number of corners
* @param [in] pCorner2dMap 2D index map of corners.
* @param [in] pCorner2dMapValidArea Start position and size of 2D index map.Please refer to AMBA_CAL_ROI_sr more detail.
* @param [in] HorizontalGirdSpace Horizontal gap between each dot on calibration board. (In world coordination) (Unit: mm)
* @param [in] VerticalGirdSpace Vertical gap between each dot on calibration board. (In world coordination) (Unit: mm)
* @param [out] pBaseCorner2DIdx Output detected 2D index of red dot in output world/image corresponding corner pairs.Please refer to AMBA_CAL_POINT_INT_2D_s for more details.
* @param [out] pCorner2dPairs Output world/image corresponding corner pairs, which are arranged as 2D map.Refer to AMBA_CAL_WORLD_RAW_POINT_sr more information.��
* @param [out] pCorner2dPairsSize Output size of world/image corresponding corner pairs.Please refer to AMBA_CAL_SIZE_s for more details.��
* @return ErrorCode
*/
UINT32 AmbaCal_OcCbCalculateWorldInfo(
    const AMBA_CAL_POINT_DB_3D_s BaseCornerWorldPos,
    const UINT8 *pSrcImg,
    AMBA_CAL_SIZE_s SrcImgSize,
    const AMBA_CAL_POINT_DB_2D_s *pCorners,
    UINT32 CornerNum,
    const INT16 *pCorner2dMap,
    const AMBA_CAL_ROI_s *pCorner2dMapValidArea,
    DOUBLE HorizontalGirdSpace,
    DOUBLE VerticalGirdSpace,
    AMBA_CAL_POINT_INT_2D_s *pBaseCorner2DIdx,
    AMBA_CAL_WORLD_RAW_POINT_s *pCorner2dPairs,
    AMBA_CAL_SIZE_s *pCorner2dPairsSize);

/**
* This API is used to find the optical center based on the checkerboard corner position.��
* @param [in] pCamSpec Camera spec. Please refer to AMBA_CAL_CAM_sr more detail.
* @param [in] pSearchRange The optical center search range. Please refer to AMBA_CAL_SIZE_sr more detail.
* @param [in] pCorners Checkerboard Corners Position. Please refer to AMBA_CAL_POINT_DB_2D_sr more detail.
* @param [in] pCorner2dMap The 100x100 Corner relationship map.
* @param [in] pCorner2dMapValidArea The valid area of the corner relationship map. Please refer to AMBA_CAL_ROI_sr more detail.
* @param [in] pUsedOCGridSize The used grid size of the Corner2dMap. Please refer to AMBA_CAL_OCCB_USED_GRID_SIZE_s for more detail.
* @param [in] pWorkBuf Working buffer provided by user.
* @param [out] pOpticalCenter Optical center position. Please refer to AMBA_CAL_POINT_DB_2D_sr more detail.
* @return ErrorCode
*/
UINT32 AmbaCal_OcCbOpticalCenterFindV1(const AMBA_CAL_CAM_s *pCamSpec,
        const AMBA_CAL_SIZE_s *pSearchRange,
        const AMBA_CAL_POINT_DB_2D_s *pCorners,
        const INT16 *pCorner2dMap,
        const AMBA_CAL_ROI_s *pCorner2dMapValidArea,
        const AMBA_CAL_OCCB_USED_GRID_SIZE_s *pUsedOCGridSize,
        const void *pWorkBuf,
        AMBA_CAL_POINT_DB_2D_s *pOpticalCenter);

/**
* This API is used to get the corners type and classifier to each plane.��
* @param [in] pWorkBuf Working buffer provided by user.
* @param [in] pSrc Source image Please refer to AMBA_CAL_YUV_INFO_s more detail.
* @param [in] pImgSize Image Size. Please refer to AMBA_CAL_SIZE_sr more detail.
* @param [in] pCenterSearchROI set a range to search the center red points. Please refer to AMBA_CAL_ROI_s more detail.
* @param [in,out] pCorners input corners position and ouput the corners type information. Please refer to AMBA_CAL_DET_CORNER_LIST_s for more details.
* @param [out] pTopLeftCorners the TopLeft plane corners information. Please refer to AMBA_CAL_DET_CORNER_LIST_s for more details.
* @param [out] pTopRightCorners the TopRight plane corners information. Please refer to AMBA_CAL_DET_CORNER_LIST_s for more details.
* @param [out] pBottomCorners the Bottom plane corners information. Please refer to AMBA_CAL_DET_CORNER_LIST_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_OcCbCornerTypePlaneClass(
        const void *pWorkBuf,
        const AMBA_CAL_YUV_INFO_s *pSrc,
        const AMBA_CAL_SIZE_s *pImgSize,
        const AMBA_CAL_ROI_s *pCenterSearchROI,
        const AMBA_CAL_DET_CORNER_LIST_s *pCorners,
        const AMBA_CAL_DET_CORNER_LIST_s *pTopLeftCorners,
        const AMBA_CAL_DET_CORNER_LIST_s *pTopRightCorners,
        const AMBA_CAL_DET_CORNER_LIST_s *pBottomCorners);


#endif
