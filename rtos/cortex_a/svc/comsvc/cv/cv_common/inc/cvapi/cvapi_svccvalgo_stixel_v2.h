/**
 *  @file cvapi_svccvalgo_stixel_v2.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of stixel v2 svc cv algo
 *
 */

#ifndef CVAPI_SVCCVALGO_STIXEL_V2_H
#define CVAPI_SVCCVALGO_STIXEL_V2_H

#include "SvcCvAlgo.h"

#define STIXEL_V2_IN_DISPARITY_WIDTH   (1920U)
#define STIXEL_V2_IN_DISPARITY_HEIGHT  (768U)
#define STIXEL_V2_COLUMN_WIDTH         (12U)

#define STIXEL_V2_MAX_DISPARITY_VAL       (256U)

#define STIXEL_V2_OUT_VDISPARITY_PRECISION     (2U)   // 1: 1 pixel, 2: 0.5 pixel, 4: 0.25 pixel
#define STIXEL_V2_OUT_VDISPARITY_WIDTH         (STIXEL_V2_MAX_DISPARITY_VAL*STIXEL_V2_OUT_VDISPARITY_PRECISION)
#define STIXEL_V2_IN_VDISPARITY_HEIGHT         (480U)
#define STIXEL_V2_VDISPARITY_HOR_SAMPLE_STRIDE (8U)
#define STIXEL_V2_IN_VDISPARITY_START_Y        (STIXEL_V2_IN_DISPARITY_HEIGHT - STIXEL_V2_IN_VDISPARITY_HEIGHT)

#define STIXEL_V2_TRANS_DISPARITY_WIDTH       (STIXEL_V2_IN_DISPARITY_HEIGHT)
#define STIXEL_V2_TRANS_DISPARITY_HEIGHT      (STIXEL_V2_IN_DISPARITY_WIDTH/STIXEL_V2_COLUMN_WIDTH)

#define STIXEL_V2_OUT_STIXEL_NUM       (STIXEL_V2_IN_DISPARITY_WIDTH/STIXEL_V2_COLUMN_WIDTH)

//#define STIXEL_V2_REQUIRED_CV_SIZE_VDISP              (100U<<10U)
#define STIXEL_V2_REQUIRED_CV_SIZE_GEN_ESTI_ROAD      (1UL<<20U)
#define STIXEL_V2_REQUIRED_CV_SIZE_PREPROCESS         (1UL<<20U)
#define STIXEL_V2_REQUIRED_CV_SIZE_FREE_SPACE         (4UL<<20U)
#define STIXEL_V2_REQUIRED_CV_SIZE_DP_FREE_SPACE      (4UL<<20U)
#define STIXEL_V2_REQUIRED_CV_SIZE_HEIGHT_SEG         (6UL<<20U)

extern SVC_CV_ALGO_OBJ_s StixelV2VDispAlgoObj;
extern SVC_CV_ALGO_OBJ_s StixelV2GenEstiRoadAlgoObj;
extern SVC_CV_ALGO_OBJ_s StixelV2PreprocessAlgoObj;
extern SVC_CV_ALGO_OBJ_s StixelV2FreeSpaceAlgoObj;
extern SVC_CV_ALGO_OBJ_s StixelV2HeightSegAlgoObj;
extern SVC_CV_ALGO_OBJ_s StixelV2FreeSpaceDPAlgoObj;


#define STIXEL_V2_ROAD_ESTI_MODE_AUTO       (0U)
// Need to configure ManualDetCfg
#define STIXEL_V2_ROAD_ESTI_MODE_MANUAL     (1U)

#define STIXEL_V2_ROAD_SECTION_NUM          (2U)

#define STIXEL_V2_BOTTOM_DP_HOR_DOWNSAMPLE  (16U)
#define STIXEL_V2_TRANS_DOWNSAMPLE_WIDTH       (STIXEL_V2_TRANS_DISPARITY_WIDTH/STIXEL_V2_BOTTOM_DP_HOR_DOWNSAMPLE)
#define STIXEL_V2_BOTTOM_DP_PROC_HEIGHT     (80U)

typedef struct {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 Width;
    UINT32 Height;
} AMBA_CV_STIXEL_V2_ROI_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 Pitch;
    flexidag_memblk_t Buff;
} AMBA_CV_STIXEL_V2_IMAGE_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 Pitch;
    flexidag_memblk_t Buff; // buffer is in 8.4 format
} AMBA_CV_STIXEL_V2_DISPARITY_s;

typedef struct {
    FLOAT  Bottom;
    FLOAT  InvM;
    FLOAT  C;
    FLOAT  SinCamPitch;
    FLOAT  CosCamPitch;
} AMBA_CV_STIXEL_V2_ESTI_ROAD_s;

typedef struct {
    FLOAT  ImageV0;
    FLOAT  BaseLine;
    FLOAT  FocalLength;
    FLOAT  DetObjectHeight;  // Object with height > DetObjectHeight is the most discoverable, in pixel.
    AMBA_CV_STIXEL_V2_ESTI_ROAD_s RoadSection[STIXEL_V2_ROAD_SECTION_NUM];
    FLOAT  PlaneCoef[3U]; // Z = Coef[0]X + Coef[1]Y+ Coef[2]
    FLOAT  FusePlaneCoefStartX;
} AMBA_CV_STIXEL_V2_GEN_ESTI_ROAD_CFG_s;

typedef struct {
    UINT16 ObjectWeighting;
    UINT16 GroundWeighting;
    UINT16 GroundToler;
} AMBA_CV_STIXEL_V2_FREE_SPACE_CFG_s;

typedef struct {
    UINT16  MinDisparity;    // Stixel bottom with disparity < MinDisparity will be set to height = 1 pixel, in m.
} AMBA_CV_STIXEL_V2_HEIGHT_SEG_CFG_s;

typedef struct {
    DOUBLE  Fu;         // horizontal focal length, in pixel
    DOUBLE  Fv;         // vertical focal length, in pixel
    DOUBLE  U0;         // optical center x, in pixel
    DOUBLE  V0;         // optical center y, in pixel
    DOUBLE  Baseline;   // stereo baseline, in mm
} AMBA_CV_STIXEL_V2_CAM_INT_PARAM_s;

typedef struct {
    DOUBLE  Height;     // default camera height, in mm
    DOUBLE  Pitch;      // default camera pitch angle, in radian
} AMBA_CV_STIXEL_V2_CAM_EXT_PARAM_s;

typedef struct {
    DOUBLE XRatioThr;  // horizontal ratio threshold
    DOUBLE YRatioThr;  // vertical ratio threshold
} AMBA_CV_STIXEL_V2_CAM_VDISP_PARAM_s;

typedef struct {
    DOUBLE PitchStride; // in degree
    DOUBLE HeightStride; // in meter
    UINT32 PitchSetNum;
    UINT32 HeightSetNum;
    AMBA_CV_STIXEL_V2_CAM_EXT_PARAM_s ExtParam;
    AMBA_CV_STIXEL_V2_CAM_VDISP_PARAM_s VDispParam;
    DOUBLE CamHeightLowerBnd;
    UINT32 UseMultiLineFit;
} AMBA_CV_STIXEL_V2_MANUAL_DET_CFG_s;

typedef struct {
    UINT16 RoadEstiMode;
    UINT16 MinDisparity;        //
    UINT16 DetObjectHeight;     // In pixel
    UINT16 GroundWeighting;     //
    UINT16 ObjectWeighting;     //
    UINT16 Tolerance;           // Lower this value to lift sensitivity
    UINT16 InvalidHWDisparityVal;
    AMBA_CV_STIXEL_V2_CAM_INT_PARAM_s IntParam;
    AMBA_CV_STIXEL_V2_MANUAL_DET_CFG_s ManualDetCfg;
} AMBA_CV_STIXEL_V2_ALG_CFG_s;

typedef struct {
    AMBA_CV_STIXEL_V2_ALG_CFG_s AlgCfg;
    AMBA_CV_STIXEL_V2_ROI_s     RoiCfg;
} AMBA_CV_STIXEL_V2_CFG_s;

typedef struct {
    UINT16 Top;             //Top position of stixel
    UINT16 Bottom;          //Bottom position of stixel
    UINT16 AvgDisparity;    //Average disparity of sitxel, unit: pixel
    UINT16 Confidence;      //Confidence value, 0(low) ~ 65535(high)
} AMBA_CV_STIXEL_V2_s;

typedef struct {
    UINT32 StixelNum;       //Number of stixels
    UINT32 Width;           //Width of stixels
    AMBA_CV_STIXEL_V2_s StixelList[STIXEL_V2_OUT_STIXEL_NUM];
} AMBA_CV_STIXEL_V2_DET_OUT_s;

#endif //CVAPI_SVCCVALGO_STIXEL_V2_H

