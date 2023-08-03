/**
 *  @file AmbaCalib_LDCCIF.h
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
#ifndef AMBA_CALIB_LDCC_IF_H
#define AMBA_CALIB_LDCC_IF_H
#include "AmbaCalib_LDCDef.h"
#include "AmbaCalib_OcCbIF.h"

#define AMBA_CAL_LDCC_DATA_VERSION (1U)
#define AMBA_CAL_LDCC_MAX_PLANE_NUM (3U)

typedef struct {
    AMBA_CAL_SENSOR_s Sensor;
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
    AMBA_CAL_LENS_DST_UNIT_e LensDistoUnit;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s RealExpectTbl;
    AMBA_CAL_POINT_DB_2D_s *pSrcMap;
    AMBA_CAL_OCCB_ORGANIZED_CORNER_s DstMap;
    UINT32 PointCount;
} AMBA_CAL_LDCC_CFG_s;

/**
* This API is used to get the working buffer size of the LDC curve. 
* @param [out] pSize LDC curve finder working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_LdccGetBufSize(SIZE_t *pSize);

/**
* This API is used to re-calculate ldc curve base on LDC data. 
* @param [in] pCfg Organizer Config. Please refer to AMBA_CAL_LDCC_CFG_s for more details.
* @param [in] pWorkingBuf .
* @param [out] pOutput Real/Expect 1D curve.  Please refer to AMBA_CAL_LENS_DST_REAL_EXPECT_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_LdccCalibLensDistoCurve(const AMBA_CAL_LDCC_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_LENS_DST_REAL_EXPECT_s *pOutput);

/**
* This API is used to get the working buffer size of Calib1PlaneDistoTbl. 
* @param [out] pSize LDC curve finder working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_LdccGetCal1PlaneBufSize(SIZE_t *pSize);

/**
* This API is used to re-calculate fisheye ldc curve base on LDC data by 1 parallel circle pattern plane.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [in] pCamSpec Camera spec. Please refer to AMBA_CAL_CAM_V2_s for more details.
* @param [in] pCorners corners position and type information. Please refer to AMBA_CAL_DET_CORNER_LIST_s for more details.
* @param [in] pCorner2dMap Corner relationship map. Please refer to AMBA_CAL_CAM_V2_s for more details.
* @param [in] pUsedCorner2dMapArea The valid area of the corner relationship map. Please refer to AMBA_CAL_ROI_s for more details.
* @param [in] RefDotNum Referance dots number to cal optical center. Please refer to UINT32 for more details.
* @param [in] PolyOrder the order of polyfit to calculate distortion table. Please refer to UINT32 for more details.
* @param [in] FocalLength lens focal length, unit pixels. Please refer to DOUBLE for more details.
* @param [out] pLensDisto calculated result for 1D distortion table. Please refer to AMBA_CAL_LENS_DST_REAL_EXPECT_s for more details.
* @param [out] pCorner2OpticalCenterDistReal for all dots distance between raw pos to optical center.  Please refer to DOUBLE for more details.
* @param [out] pCorner2OpticalCenterDistExpect for all dots distance between idea pos to optical center.  Please refer to DOUBLE for more details.
* @param [out] pDistNumber the number of all dots.  Please refer to UINT32 for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_LdccCalib1PlaneDistoTbl( const void *pWorkingBuf,
                                        const AMBA_CAL_CAM_V2_s *pCamSpec,
                                        const AMBA_CAL_DET_CORNER_LIST_s *pCorners,
                                        const INT16 *pCorner2dMap,
                                        const AMBA_CAL_ROI_s *pUsedCorner2dMapArea,
                                        UINT32 RefDotNum,
                                        UINT32 PolyOrder,
                                        DOUBLE FocalLength,
                                        AMBA_CAL_LENS_DST_REAL_EXPECT_s *pLensDisto,
                                        DOUBLE *pCorner2OpticalCenterDistReal,
                                        DOUBLE* pCorner2OpticalCenterDistExpect,
                                        UINT32 *pDistNumber);



/**
* This API is used to get the working buffer size of Calib3PlaneDistoTbl. 
* @param [out] pSize LDC curve finder working buffer size.
* @return ErrorCode
*/
UINT32 AmbaCal_LdccGetCal3PlaneBufSize(SIZE_t *pSize);

/**
* This API is used to re-calculate fisheye ldc curve base on LDC data by 3 circle pattern plane.
* @param [in] pWorkingBuf Working buffer provided by user.
* @param [in] pCamSpec Camera spec. Please refer to AMBA_CAL_CAM_V2_s for more details.
* @param [in] pPreCalDistTbl pre-calculated calibration table. Please refer to AMBA_CAL_LENS_DST_REAL_EXPECT_s for more details.
* @param [in] pCorners corners position and type information. Please refer to AMBA_CAL_DET_CORNER_LIST_s for more details.
* @param [in] pCorner2dMap Corner relationship map. Please refer to AMBA_CAL_CAM_V2_s for more details.
* @param [in] TotalPlaneNum The total planes number for calibrating distortion table. Please refer to UINT32 for more details.
* @param [in] PolyOrder the order of polyfit to calculate distortion table. Please refer to UINT32 for more details.
* @param [in] FocalLength lens focal length, unit pixels. Please refer to DOUBLE for more details.
* @param [in] SeparateStepNum Separate to few step to calculate distortion table. Please refer to UINT32 for more details.
* @param [in] RefMaxAngleDeg Set a reference angle degree to calculate the inner and outer distortion table. Please refer to DOUBLE for more details.
* @param [out] pCalREDistTbl calculated RefMaxAngleDeg inner 1D distortion table. Please refer to AMBA_CAL_LENS_DST_REAL_EXPECT_s for more details.
* @param [out] pCalRADistTbl calculated whole range 1D distortion table.  Please refer to AMBA_CAL_LENS_DST_ANGLE_s for more details.
* @return ErrorCode
*/
UINT32 AmbaCal_LdccCalib3PlaneDistoTbl( const void *pWorkingBuf,
                                        AMBA_CAL_CAM_V2_s *pCamSpec,
                                        const AMBA_CAL_LENS_DST_REAL_EXPECT_s *pPreCalDistTbl,
                                        const AMBA_CAL_DET_CORNER_LIST_s *const pCorners[AMBA_CAL_LDCC_MAX_PLANE_NUM],
                                        const INT16 *const pCorner2dMap[AMBA_CAL_LDCC_MAX_PLANE_NUM],
                                        UINT32 TotalPlaneNum,
                                        UINT32 PolyOrder,
                                        DOUBLE FocalLength,
                                        UINT32 SeparateStepNum,
                                        DOUBLE RefMaxAngleDeg,
                                        AMBA_CAL_LENS_DST_REAL_EXPECT_s *pCalREDistTbl,
                                        AMBA_CAL_LENS_DST_ANGLE_s *pCalRADistTbl);

#endif
