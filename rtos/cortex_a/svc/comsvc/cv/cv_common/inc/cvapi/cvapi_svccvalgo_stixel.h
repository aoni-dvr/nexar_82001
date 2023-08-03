/**
 *  @file cvapi_svccvalgo_stixel.h
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
 *  @details Implementation of stixel svc cv algo
 *
 */

#ifndef CVAPI_SVCCVALGO_STIXEL_H
#define CVAPI_SVCCVALGO_STIXEL_H

#include "SvcCvAlgo.h"

#define STIXEL_IN_DISPARITY_WIDTH   (1920U)
#define STIXEL_IN_DISPARITY_HEIGHT  (768U)
#define STIXEL_COLUMN_WIDTH         (6U)
#define STIXEL_IN_MAX_DISPARITY     (256U)  // Max possible disparity value, in pixel

#define STIXEL_OUT_VDISPARITY_WIDTH         (256U)
#define STIXEL_OUT_VDISPARITY_PRECISION     (1U)   // 1: 1 pixel, 2: 0.5 pixel, 4: 0.25 pixel
#define STIXEL_IN_VDISPARITY_HEIGHT         (384U)
#define STIXEL_IN_VDISPARITY_START_Y        (384U)

#define STIXEL_TRANS_DISPARITY_WIDTH       (STIXEL_IN_DISPARITY_HEIGHT)
#define STIXEL_TRANS_DISPARITY_HEIGHT      (STIXEL_IN_DISPARITY_WIDTH/STIXEL_COLUMN_WIDTH)

#define STIXEL_OUT_STIXEL_NUM       (STIXEL_IN_DISPARITY_WIDTH/STIXEL_COLUMN_WIDTH)

#define STIXEL_REQUIRED_CV_SIZE_VDISP              (100UL<<10UL)
#define STIXEL_REQUIRED_CV_SIZE_GEN_ESTI_ROAD      (100UL<<10UL)
#define STIXEL_REQUIRED_CV_SIZE_PREPROCESS         (100UL<<10UL)
#define STIXEL_REQUIRED_CV_SIZE_FREE_SPACE         (500UL<<10UL)
#define STIXEL_REQUIRED_CV_SIZE_HEIGHT_SEG         (1500UL<<10UL)

extern SVC_CV_ALGO_OBJ_s StixelVDispAlgoObj;
extern SVC_CV_ALGO_OBJ_s StixelGenEstiRoadAlgoObj;
extern SVC_CV_ALGO_OBJ_s StixelPreprocessAlgoObj;
extern SVC_CV_ALGO_OBJ_s StixelFreeSpaceAlgoObj;
extern SVC_CV_ALGO_OBJ_s StixelHeightSegAlgoObj;

#define STIXEL_ROAD_ESTI_MODE_AUTO       (0U)
// Need to configure ManualDetCfg
#define STIXEL_ROAD_ESTI_MODE_MANUAL     (1U)

#define STIXEL_ROAD_SECTION_NUM          (2U)

typedef struct {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 Width;
    UINT32 Height;
} AMBA_CV_STIXEL_ROI_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 Pitch;
    flexidag_memblk_t Buff;
} AMBA_CV_STIXEL_IMAGE_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 Pitch;
    flexidag_memblk_t Buff; // buffer is in 8.4 format
} AMBA_CV_STIXEL_DISPARITY_s;

typedef struct {
    FLOAT  Bottom;
    FLOAT  M;
    FLOAT  C;
    FLOAT  SinCamPitch;
    FLOAT  CosCamPitch;
} AMBA_CV_STIXEL_ESTI_ROAD_s;

typedef struct {
    FLOAT  ImageV0;
    FLOAT  BaseLine;
    FLOAT  FocalLength;
    FLOAT  DetObjectHeight;  // Object with height > DetObjectHeight is the most discoverable, in m.
    AMBA_CV_STIXEL_ESTI_ROAD_s RoadSection[STIXEL_ROAD_SECTION_NUM];
} AMBA_CV_STIXEL_GEN_ESTI_ROAD_CFG_s;

typedef struct {
    UINT16  MinDisparity;    // Stixel bottom with disparity < MinDisparity will be set to height = 1 pixel, in m.
} AMBA_CV_STIXEL_HEIGHT_SEG_CFG_s;

typedef struct {
    // Horizontal focal length. (unit: pixel)
    DOUBLE Fu;
    // Vertical focal length. (unit: pixel)
    DOUBLE Fv;
    // Optical center u. (unit: pixel)
    DOUBLE U0;
    // Optical center v. (unit: pixel)
    DOUBLE V0;
    // Stereo baseline. (unit: mm)
    DOUBLE Baseline;
} AMBA_CV_STIXEL_CAM_INT_PARAM_s;

typedef struct {
    // Default camera height. (unit: mm)
    DOUBLE Height;
    // Default camera pitch angle. (unit: radian)
    DOUBLE Pitch;
} AMBA_CV_STIXEL_CAM_EXT_PARAM_s;

typedef struct {
    // Horizontal ratio threshold. (range: [0.0 ~ 1.0])
    DOUBLE XRatioThr;
    // Vertical ratio threshold. (range: [0.0 ~ 1.0])
    DOUBLE YRatioThr;
} AMBA_CV_STIXEL_CAM_VDISP_PARAM_s;

typedef struct {
    // Granularity of camera pitch in candidate pool. (unit: degree)
    DOUBLE PitchStride;
    // Granularity of camera height in candidate pool. (unit: m)
    DOUBLE HeightStride;
    // Number of various camera pitch.
    UINT32 PitchSetNum;
    // Number of various camera height.
    UINT32 HeightSetNum;
    // Extrinsic camera parameters.
    AMBA_CV_STIXEL_CAM_EXT_PARAM_s ExtParam;
    // Parameters to assess quality of v disaprity .
    AMBA_CV_STIXEL_CAM_VDISP_PARAM_s VDispParam;
} AMBA_CV_STIXEL_MANUAL_DET_CFG_s;

typedef struct {
    // Run auto mode(STIXEL_ROAD_ESTI_MODE_AUTO ) or manual mode(STIXEL_ROAD_ESTI_MODE_MANUAL).
    UINT16 RoadEstiMode;
    // Height of stixel will be forced to be 1 if disparity at its bottom is smaller than this value. (unit: pixel)
    UINT16 MinDisparity;
    // Object with a height larger than this value is more detectable. (unit: m)
    DOUBLE DetObjectHeight;
    // Intrinsic camera parameters.
    AMBA_CV_STIXEL_CAM_INT_PARAM_s IntParam;
    // Configuration of manual mode.
    AMBA_CV_STIXEL_MANUAL_DET_CFG_s ManualDetCfg;
} AMBA_CV_STIXEL_CFG_s;

typedef struct {
    AMBA_CV_STIXEL_CFG_s AlgCfg;
    AMBA_CV_STIXEL_ROI_s RoiCfg;
} AMBA_CV_STIXEL_CFG_V2_s;

typedef struct {
    // Top v position of stixel. (unit: pixel)
    UINT16 Top;
    // Bottom v position of stixel. (unit: pixel)
    UINT16 Bottom;
    // Average disparity of sitxel. (unit: pixel)
    UINT16 AvgDisparity;
    // Confidence value, 0(low) ~ 65535(high)
    UINT16 Confidence;
} AMBA_CV_STIXEL_s;

typedef struct {
    // Number of stixels.
    UINT32 StixelNum;
    // Width of stixels.
    UINT32 Width;
    // List of stixels.
    AMBA_CV_STIXEL_s StixelList[STIXEL_OUT_STIXEL_NUM];
} AMBA_CV_STIXEL_DET_OUT_s;

#endif //CVAPI_SVCCVALGO_STIXEL_H

