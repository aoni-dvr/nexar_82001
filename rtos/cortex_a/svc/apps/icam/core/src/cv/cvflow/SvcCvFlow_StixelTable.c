/**
 *  @file SvcCvFlow_StixelTable.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Table of FlexiDAG based Stixel application
 *
 */

#include "AmbaTypes.h"
#include "AmbaDSP_Liveview.h"

#include "SvcCvAlgo.h"

#include "SvcCvFlowProfile.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_Stixel.h"

#include "cvapi_svccvalgo_stixel.h"

#define STIXEL_ROAD_ESTI_MODE              (STIXEL_ROAD_ESTI_MODE_MANUAL) /* STIXEL_ROAD_ESTI_MODE_AUTO or STIXEL_ROAD_ESTI_MODE_MANUAL*/

//#define UT_CFG_FOCAL_LENGTH_U              (1327.1875)
//#define UT_CFG_FOCAL_LENGTH_V              (1327.1875)
//#define UT_CFG_OPTICAL_CENTER_X            (917.0)
//#define UT_CFG_OPTICAL_CENTER_Y            (489.3125)
//#define UT_CFG_BASELINE                    (0.3003000020980835)

#define UT_CFG_FOCAL_LENGTH_U              (1590.0)
#define UT_CFG_FOCAL_LENGTH_V              (1590.0)
#define UT_CFG_OPTICAL_CENTER_X            (960.0)
#define UT_CFG_OPTICAL_CENTER_Y            (384.0)
#define UT_CFG_BASELINE                    (0.24969)


#define UT_CFG_DEFAULT_CAMERA_HEIGH        (1.520587)
#define UT_CFG_DEFAULT_CAMERA_PITCH        (0.073589)

#define UT_CFG_ROAD_SEARCH_PITCH_STRIDE    (0.25)
#define UT_CFG_ROAD_SEARCH_HEIGHT_STRIDE   (0.02)
#define UT_CFG_ROAD_SEARCH_PITCH_NUM       (64U)
#define UT_CFG_ROAD_SEARCH_HEIGHT_NUM      (10U)

#define UT_CFG_VDISP_X_THR                 (0.2)
#define UT_CFG_VDISP_Y_THR                 (0.15)

#define UT_CFG_DET_OBJECT_HEIGHT           (1.0)
#define UT_CFG_MIN_DISPARITY               (6U)

extern AMBA_CV_STIXEL_CFG_s StixelCfgDefault;

AMBA_CV_STIXEL_CFG_s StixelCfgDefault = {
    .RoadEstiMode = STIXEL_ROAD_ESTI_MODE,
    .IntParam = {
        .Fu       = UT_CFG_FOCAL_LENGTH_U,
        .Fv       = UT_CFG_FOCAL_LENGTH_V,
        .U0       = UT_CFG_OPTICAL_CENTER_X,
        .V0       = UT_CFG_OPTICAL_CENTER_Y,
        .Baseline = UT_CFG_BASELINE,
    },
    .ManualDetCfg = {
        .PitchStride  = UT_CFG_ROAD_SEARCH_PITCH_STRIDE,
        .HeightStride = UT_CFG_ROAD_SEARCH_HEIGHT_STRIDE,
        .PitchSetNum  = UT_CFG_ROAD_SEARCH_PITCH_NUM,
        .HeightSetNum = UT_CFG_ROAD_SEARCH_HEIGHT_NUM,
        .ExtParam = {
#ifdef STIXEL_ROAD_ESTI_MODE_MANUAL
            .Height = UT_CFG_DEFAULT_CAMERA_HEIGH,
            .Pitch  = UT_CFG_DEFAULT_CAMERA_PITCH,
#else
            .Height = 0U,
            .Pitch  = 0U,
#endif
        },
        .VDispParam = {
            .XRatioThr = UT_CFG_VDISP_X_THR,
            .YRatioThr = UT_CFG_VDISP_Y_THR,
        }
    },
    .MinDisparity        = UT_CFG_MIN_DISPARITY,
    .DetObjectHeight     = UT_CFG_DET_OBJECT_HEIGHT,
};

const SVC_CV_FLOW_STIXEL_MODE_INFO_s SvcCvFlow_Stixel_ModeInfo[STIXEL_NUM_MODE] = {
    [STIXEL_MODE_1] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 5,
            .AlgoInfo = {
                [0] = {
                    .pAlgoObj = &StixelVDispAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "stixel_v_disparity.bin"
                    },
                    .NumLcsBin = 0,
                },
                [1] = {
                    .pAlgoObj = &StixelGenEstiRoadAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "stixel_gen_esti_road.bin"
                    },
                    .NumLcsBin = 0,
                },
                [2] = {
                    .pAlgoObj = &StixelPreprocessAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "stixel_preprocess.bin"
                    },
                    .NumLcsBin = 0,
                },
                [3] = {
                    .pAlgoObj = &StixelFreeSpaceAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "stixel_free_space.bin"
                    },
                    .NumLcsBin = 0,
                },
                [4] = {
                    .pAlgoObj = &StixelHeightSegAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "stixel_height_seg.bin"
                    },
                    .NumLcsBin = 0,
                },
            }
        },
        .Cfg = {
            .pStixelCfg = &StixelCfgDefault,
            .VDispOffsetY = 384,
        },
        .OutputType = STIXEL_OUTPUT_TYPE_STIXEL,
    },
};

