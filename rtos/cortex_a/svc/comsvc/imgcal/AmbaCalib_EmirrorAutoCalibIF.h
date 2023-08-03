/**
 *  @file AmbaCalib_EmirrorAutoCalibIF.h
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
#ifndef AMBA_CALIB_EMIRROR_AUTOCALIB_API_H
#define AMBA_CALIB_EMIRROR_AUTOCALIB_API_H
#include "AmbaCalib_EmirrorDef.h"
#include "AmbaCalib_ErrNo.h"

#define AMBA_CAL_EM_LANE_MAX_PNT_NUM (32U) /** Maximum of line points number */
typedef struct {
    UINT32 PntNum;                                                /**< Total point number to represent a line */
    AMBA_CAL_POINT_INT_2D_s Pnt2D[AMBA_CAL_EM_LANE_MAX_PNT_NUM];  /**< Points at image domain */
} AMBA_CAL_EM_LANE_LINE_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    DOUBLE FocalLength;
    AMBA_CAL_ROI_s VoutView;
    AMBA_CAL_EM_MULTI_POINT_MAP_s CalibPoints; //At least 4 sets
    /* Lane line data should scale to size of vout view domain */
    AMBA_CAL_EM_LANE_LINE_s LeftLaneLine; // position on bird's eye view
    AMBA_CAL_EM_LANE_LINE_s RightLaneLine; // position on bird's eye view
    AMBA_CAL_ROTATION_ANGLE_3D_s MaxCompensateAngleDeg;
    /* Info for converting coordinate from Vout to World */
    AMBA_CAL_ROI_s Roi;
    AMBA_CAL_EM_CURVED_SURFACE_CFG_s *pBirdviewCfg;
    /* Info for converting coordinate from Raw to AssistPlane(World) */
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
} AMBA_CAL_EM_CAM_SKEW_DET_CFG_s;

typedef struct {
    AMBA_CAL_EM_MULTI_POINT_MAP_s SkewCalibPoints;
    AMBA_CAL_ROTATION_ANGLE_3D_s DetectSkewAngleDeg;
    AMBA_CAL_ROTATION_ANGLE_3D_s CompesateAngleDeg;
    AMBA_CAL_EM_CAM_CALIB_DATA_s SkewCalibData;    /* Matrix mapping raw and world */
    AMBA_CAL_ROTATION_ANGLE_3D_s SkewCamRotStatus; /* Rotation status of current camera. Unit is degree */
} AMBA_CAL_EM_CAM_SKEW_FIX_s;

typedef struct {
    DOUBLE CoeffA;
    DOUBLE CoeffB;
} AMBA_CAL_EM_LINE_2D_EQ_s; /* Ax + B = y */

typedef struct {
    UINT32 LoopTimes;
    DOUBLE InlierDistThreshold;
    UINT32 MinInlierSampleNum;
} AMBA_CAL_EM_RANSAC_CFG_s; /* Ax + B = y */

typedef struct {
    DOUBLE MaxNode2LineDist;
    AMBA_CAL_ROI_s ValidVanishPtRegion;
} AMBA_CAL_LANE_DIRSTR_CHK_s;

typedef enum { //Lane source
    AMBA_CAL_LANE_IMAGE = 0,
    AMBA_CAL_LANE_WORLD
} AMBA_CAL_LANE_LINE_TYPE_e;

typedef struct {
    AMBA_CAL_LANE_LINE_TYPE_e Type;
    AMBA_CAL_EM_LANE_LINE_s *pDataLeft;
    AMBA_CAL_EM_LANE_LINE_s *pDataRight;
    UINT32 PairNum;
} AMBA_CAL_LANE_LINE_DATA_s;

typedef struct {
    UINT32 RetryTimes;
    AMBA_CAL_EM_RANSAC_CFG_s *pRansacCfg;
} AMBA_CAL_LANE_LINE_REFINE_ALGO_s;

typedef struct {
    AMBA_CAL_CAM_s Cam;
    DOUBLE FocalLength;
    AMBA_CAL_ROI_s Roi;
    AMBA_CAL_ROI_s VoutView;
    AMBA_CAL_LANE_LINE_DATA_s LaneLineData;
    AMBA_CAL_EM_MULTI_POINT_MAP_s *pCalibPoints;        /* Optional. Assign NULL for default */
    AMBA_CAL_LANE_LINE_REFINE_ALGO_s DataRefineAlgo;    /* Optional. Assign NULL for default */
    AMBA_CAL_ROTATION_ANGLE_3D_s MaxCompensateAngleDeg; /* Optional. Assign 0 for default */
} AMBA_CAL_EM_AUTOCAL_CFG_s;

UINT32 AmbaCal_EmGetCamSkewDetBufSize(SIZE_t *pSize);
UINT32 AmbaCal_EmCamSkewDetect(const AMBA_CAL_EM_CAM_SKEW_DET_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_CAM_SKEW_FIX_s *pFixedCalibInfo);

UINT32 AmbaCal_EmRansacSamplePtToLine(const AMBA_CAL_POINT_DB_2D_s *pSampleData, UINT32 SampleNum, const AMBA_CAL_EM_RANSAC_CFG_s *pRansacCfg, AMBA_CAL_EM_LINE_2D_EQ_s *pLine);

UINT32 AmbaCal_EmGetImgToFloorMatrix(const AMBA_CAL_EM_CALC_COORD_CFG_s *pCfg, DOUBLE *pMatrix);

UINT32 AmbaCal_EmGetCamAutoCalBufSize(UINT32 TotalLaneLinePairNum, SIZE_t *pSize);

UINT32 AmbaCal_EmCamAutoCalYawPitch(const AMBA_CAL_EM_AUTOCAL_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EM_CAM_SKEW_FIX_s *pOutput);

UINT32 AmbaCal_EmCalcCamStatus(const AMBA_CAL_CAM_s *pCam,
                               const AMBA_CAL_EM_MULTI_POINT_MAP_s *pCalibPoints,
                               DOUBLE FocalLengthMM,
                               const void *pWorkingBuf,
                               AMBA_CAL_POINT_DB_3D_s *pCalculatedCamPos,
                               AMBA_CAL_ROTATION_ANGLE_3D_s *pCalculatedCamRotDeg);

UINT32 AmbaCal_EmGetCamRotateBase(const AMBA_CAL_POINT_DB_3D_s *pCamPos, AMBA_CAL_ROTATION_ANGLE_3D_s *pCamRotBase);
UINT32 AmbaCal_EmIsLaneDirStraight(const AMBA_CAL_LANE_DIRSTR_CHK_s *pChkCfg, const AMBA_CAL_EM_LANE_LINE_s *pLaneLeftLine, const AMBA_CAL_EM_LANE_LINE_s *pLaneRightLine, UINT32 *pChkResult);
UINT32 AmbaCal_EmGetVanishPtByRotStat(const AMBA_CAL_ROI_s *pVoutSize, const AMBA_CAL_ROTATION_ANGLE_3D_s *pCamRotStatus, const AMBA_CAL_POINT_DB_3D_s *pCamPos, DOUBLE FocalLengthPixel, AMBA_CAL_POINT_DB_2D_s *pVanishPtPos);
UINT32 AmbaCal_EmValidVanishPtArea(const AMBA_CAL_POINT_DB_2D_s *pIdealVanishPtPos, const AMBA_CAL_ROTATION_ANGLE_3D_s *pSkewDegTolerance, const AMBA_CAL_ROI_s *pVoutSize, DOUBLE FocalLengthPixel, AMBA_CAL_ROI_s *pValidVanishPtArea);
#endif
