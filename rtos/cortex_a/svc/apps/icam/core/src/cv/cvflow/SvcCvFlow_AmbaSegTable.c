/**
 *  @file SvcCvFlow_AmbaSegTable.c
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
 *  @details Table of FlexiDAG based Ambarella Segmentation
 *
 */

#include "AmbaTypes.h"
#include "AmbaDSP_Liveview.h"

#include "SvcCvAlgo.h"
//#include "cvapi_flexidag_ref_util.h"
#include "cvapi_amba_mask_interface.h"
#include "cvapi_svccvalgo_ambaseg.h"

#include "SvcCvFlowProfile.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaSeg.h"

extern SVC_CV_FLOW_AMBASEG_MASK_CFG_s AmbaSeg_Mask_1280x512;
extern SVC_CV_FLOW_AMBASEG_MASK_CFG_s AmbaSeg_Mask_768x448;

SVC_CV_FLOW_AMBASEG_MASK_CFG_s AmbaSeg_Mask_1280x512 = {
    .BufWidth       = 1280U,
    .BufHeight      = 512U
};

SVC_CV_FLOW_AMBASEG_MASK_CFG_s AmbaSeg_Mask_768x448 = {
    .BufWidth       = 768U,
    .BufHeight      = 448U
};

const SVC_CV_FLOW_AMBASEG_MODE_INFO_s SvcCvFlow_AmbaSeg_ModeInfo[AMBASEG_NUM_MODE] = {
    [AMBASEG_FDAG_FC] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBASEGFC
                    .pAlgoObj = &AmbaSegAlgoObj,
#else
   #ifdef  CONFIG_CV_FLEXIDAG_AMBASEG39FC
                    .pAlgoObj = &AmbaSeg39AlgoObj,
   #else
                    .pAlgoObj = NULL,
   #endif
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaseg_fc.bin"
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaseg_fc.lcs"
                    },
                }
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBASEG_FC_NET_WIDTH, .NetHeight = AMBASEG_FC_NET_HEIGHT,
                .pMaskCfg = &AmbaSeg_Mask_1280x512
            }
        }
    },
    [AMBASEG_FDAG_FC_RES1] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBASEGFC
                    .pAlgoObj = &AmbaSegAlgoObj,
#else
   #ifdef  CONFIG_CV_FLEXIDAG_AMBASEG39FC
                    .pAlgoObj = &AmbaSeg39AlgoObj,
   #else
                    .pAlgoObj = NULL,
   #endif
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexibin_ambaseg_fc_768x448.bin"
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaseg_fc.lcs"
                    },
                }
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBASEG_FC_NET_WIDTH_RES1, .NetHeight = AMBASEG_FC_NET_HEIGHT_RES1,
                .pMaskCfg = &AmbaSeg_Mask_768x448
            }
        }
    },
    [AMBASEG_FDAG_AVM] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_CV_FLEXIDAG_AMBASEGAVM
                    .pAlgoObj = &AmbaSegAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 1, .FlexiBinBits = 0x1,
                    .FlexiBinList = {
                        [0] = "flexidag_ambaseg37_avm.bin"
                    },
                    .NumLcsBin = 1,
                    .LcsBinList = {
                        [0] = "flexidag_ambaseg37_avm.lcs"
                    },
                }
            }
        },
        .Cfg = {
            [0] = {
                .NetWidth = AMBASEG_FC_NET_WIDTH, .NetHeight = AMBASEG_FC_NET_HEIGHT,
                .pMaskCfg = &AmbaSeg_Mask_1280x512

            }
        }
    },
};

