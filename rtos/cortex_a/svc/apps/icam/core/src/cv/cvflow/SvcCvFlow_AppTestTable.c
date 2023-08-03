/**
 *  @file SvcCvFlow_AppTestTable.c
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
 *  @details Table of FlexiDAG based Application Test
 *
 */

#include "AmbaTypes.h"
#include "AmbaDSP_Liveview.h"

#include "SvcCvAlgo.h"
//#include "cvapi_flexidag_ref_util.h"
#include "cvapi_amba_mask_interface.h"
#include "cvapi_svccvalgo_openseg.h"
#include "cvapi_svccvalgo_apptest.h"

#include "SvcCvFlowProfile.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AppTest.h"

extern SVC_CV_FLOW_APPTEST_MASK_CFG_s AppTest_Mask_320x128;

SVC_CV_FLOW_APPTEST_MASK_CFG_s AppTest_Mask_320x128 = {
    .BufWidth       = 320U,
    .BufHeight      = 128U
};

const SVC_CV_FLOW_APPTEST_MODE_INFO_s SvcCvFlow_AppTest_ModeInfo[APPTEST_NUM_MODE] = {
    [APPTEST_OPEN_SEG_1] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
                    .pAlgoObj = &AppTestAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_openseg.bin"
                    },
                    .NumLcsBin = 0,
                }
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = APPTEST_OPEN_NET_WIDTH, .NetHeight = APPTEST_OPEN_NET_HEIGHT,
                .pMaskCfg = &AppTest_Mask_320x128
            }
        },
        .OutputType = APPTEST_OUTPUT_TYPE_MASK_OUT,
        .OutputBufIdx = 1,
    },
    [APPTEST_AAA_ALGO] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
                    .pAlgoObj = &AppTestAlgoObj,
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_imgproc.bin"
                    },
                    .NumLcsBin = 0,
                }
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = 0U, .NetHeight = 0U,
                .pMaskCfg = NULL
            }
        },
        .OutputType = APPTEST_OUTPUT_TYPE_AAA_ALGO,
        .OutputBufIdx = 0,
        .FlowCtrl = APPTEST_FLOW_AAA_ALGO
    },
};
