/**
 *  @file SvcCvFlow_StereoTable.c
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
 *  @details Table of MEMIO based Ambarella Object Detection
 *
 */

#include "AmbaTypes.h"
#include "AmbaDSP_Liveview.h"

#include "SvcCvAlgo.h"
//#include "cvapi_svccvalgo_stereo.h"


#include "SvcCvFlowProfile.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_Stereo.h"
#include "cvapi_svccvalgo_ambaspufex.h"
#include "cvapi_svccvalgo_ambaspufusion.h"
#include "cvapi_svccvalgo_linux.h"


#if defined(CONFIG_SOC_CV2)
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_flexidag_ambaspufex_def_cv2.h"
#include "cvapi_flexidag_ambaspufusion_cv2.h"
#include "cvapi_flexidag_ambastereoerr_cv2.h"
#endif
#if defined(CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_cv2fs.h"
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"
#include "cvapi_flexidag_ambaspufusion_cv2fs.h"
#include "cvapi_flexidag_ambastereoerr_cv2fs.h"
#include "cvapi_flexidag_ambaoffex_cv2fs.h"
#endif

extern SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L0_1[1];
extern SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L0_UT_4K[1];
extern SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L2_1[1];
extern SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L02_1[2];
extern SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L024_1[3];
extern SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L024_1_SC5[3];
extern SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L024_2_SC5[3];
extern SVC_CV_FLOW_STEREO_FEX_SCALE_INFO_s Stereo_Fex_L0_1[1];
extern SVC_CV_FLOW_STEREO_FEX_SCALE_INFO_s Stereo_Fex_L0_UT_4K[1];
extern SVC_CV_FLOW_STEREO_FUS_INFO_s Stereo_Fusion_SC5[1];

SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L0_1[1] = {
    [0] = {
        .ScaleIdx  = 0,
        .Enable    = 1,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 0, .RoiHeight = 0,
        .FusionDisparity = 0,
    }
};

SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L0_UT_4K[1] = {
    [0] = {
        .ScaleIdx  = 0,
        .Enable    = 1,
        .RoiEnable = 1, .StartCol = 1280, .StartRow = 580, .RoiWidth = 1280, .RoiHeight = 720,
        .FusionDisparity = 1,
    }
};

SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L2_1[1] = {
    [0] = {
        .ScaleIdx  = 2,
        .Enable    = 1,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 0, .RoiHeight = 0,
        .FusionDisparity = 0,
    }
};

SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L02_1[2] = {
    [0] = {
        .ScaleIdx  = 0,
        .Enable    = 1,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 0, .RoiHeight = 0,
        .FusionDisparity = 0,
    },
    [1] = {
        .ScaleIdx  = 2,
        .Enable    = 1,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 0, .RoiHeight = 0,
        .FusionDisparity = 0,
    }
};
SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L024_1[3] = {
    [0] = {
        .ScaleIdx  = 0,
        .Enable    = 1,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 0, .RoiHeight = 0,
        .FusionDisparity = 1,
    },
    [1] = {
        .ScaleIdx  = 2,
        .Enable    = 1,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 0, .RoiHeight = 0,
        .FusionDisparity = 0,
    },
    [2] = {
        .ScaleIdx  = 4,
        .Enable    = 1,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 0, .RoiHeight = 0,
        .FusionDisparity = 0,
    }
};


SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L024_1_SC5[3] = {
    [0] = {
        .ScaleIdx  = 0,
        .Enable    = 1,
        .RoiEnable = 1, .StartCol = 1280, .StartRow = 580, .RoiWidth = 1280, .RoiHeight = 720,
        .FusionDisparity = 1,
    },
    [1] = {
        .ScaleIdx  = 2,
        .Enable    = 1,
        .RoiEnable = 1, .StartCol = 320, .StartRow = 170, .RoiWidth = 1280, .RoiHeight = 720,
        .FusionDisparity = 0,
    },
    [2] = {
        .ScaleIdx  = 4,
        .Enable    = 1,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 960, .RoiHeight = 480,
        .FusionDisparity = 0,
    }
};

SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s Stereo_Spu_L024_2_SC5[3] = {
    [0] = {
        .ScaleIdx  = 0,
        .Enable    = 1,
        .RoiEnable = 1, .StartCol = 0, .StartRow = 0, .RoiWidth = 1280, .RoiHeight = 720,
        .FusionDisparity = 1,
    },
    [1] = {
        .ScaleIdx  = 2,
        .Enable    = 1,
        .RoiEnable = 1, .StartCol = 0, .StartRow = 0, .RoiWidth = 1280, .RoiHeight = 720,
        .FusionDisparity = 0,
    },
    [2] = {
        .ScaleIdx  = 4,
        .Enable    = 1,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 960, .RoiHeight = 480,
        .FusionDisparity = 0,
    }
};

SVC_CV_FLOW_STEREO_FEX_SCALE_INFO_s Stereo_Fex_L0_1[1] = {
    [0] = {
        .ScaleIdx  = 0,
        .Enable    = (UINT8)CV_FEX_ENABLE_ALL,
        .RoiEnable = 0, .StartCol = 0, .StartRow = 0, .RoiWidth = 0, .RoiHeight = 0,
        .SecondaryRoiColOffset = 0, .SecondaryRoiRowOffset = 0,
    }
};

SVC_CV_FLOW_STEREO_FEX_SCALE_INFO_s Stereo_Fex_L0_UT_4K[1] = {
    [0] = {
        .ScaleIdx  = 0,
        .Enable    = (UINT8)CV_FEX_ENABLE_ALL,
        .RoiEnable = 1, .StartCol = 1280, .StartRow = 580, .RoiWidth = 1280, .RoiHeight = 720,
        .SecondaryRoiColOffset = 0, .SecondaryRoiRowOffset = 0,
    }
};


SVC_CV_FLOW_STEREO_FUS_INFO_s Stereo_Fusion_SC5[1] = {
    [0] = {
        .ScaleIdx  = 2,
    }
};

const SVC_CV_FLOW_STEREO_MODE_INFO_s SvcCvFlow_Stereo_ModeInfo[STEREO_NUM_MODE] = {
    [STEREO_SCA_SP_FE_1] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
                    .pAlgoObj = &AmbaSpuFexAlgoObj,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                },
            }
        },
        .SpuCfg = {
            .NumScale = 0,
            .pScaleInfo = NULL,
        },
        .FexCfg = {
            .NumScale = 0,
            .pScaleInfo = NULL,
        },
        .FusCfg = {
            .NumScale = 0,
            .pFusInfo = NULL,
        },
        .OutputType = STEREO_OUT_TYPE_SPU_FEX,
        .EnableFusion = 0,
    },
    [STEREO_SCA_SP_FS_3840x1920_L024_1] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 2,
            .AlgoInfo = {
                [0] = {
                    .pAlgoObj = &AmbaSpuFexAlgoObj,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                },
                [1] = {
                    .pAlgoObj = &AmbaSpuFusionAlgoObj,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                }
            }
        },
        .SpuCfg = {
            .NumScale = 3,
            .pScaleInfo = Stereo_Spu_L024_1_SC5,
        },
        .FexCfg = {
            .NumScale = 0,
            .pScaleInfo = NULL,
        },
        .FusCfg = {
            .NumScale = 3,
            .pFusInfo = Stereo_Fusion_SC5,
        },
        .OutputType = STEREO_OUT_TYPE_SPU_FEX,
        .EnableFusion = 1,
    },
    [STEREO_SCA_SP_FE_UT_1080P] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
                    .pAlgoObj = &AmbaSpuFexAlgoObj,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                },
            }
        },
        .SpuCfg = {
            .NumScale = 1,
            .pScaleInfo = Stereo_Spu_L0_1,
        },
        .FexCfg = {
            .NumScale = 1,
            .pScaleInfo = Stereo_Fex_L0_1,
        },
        .FusCfg = {
            .NumScale = 0,
            .pFusInfo = NULL,
        },
        .OutputType = STEREO_OUT_TYPE_SPU_FEX,
        .EnableFusion = 0,
    },
    [STEREO_SCA_SP_FE_UT_4K] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
                    .pAlgoObj = &AmbaSpuFexAlgoObj,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                },
            }
        },
        .SpuCfg = {
            .NumScale = 1,
            .pScaleInfo = Stereo_Spu_L0_UT_4K,
        },
        .FexCfg = {
            .NumScale = 1,
            .pScaleInfo = Stereo_Fex_L0_UT_4K,
        },
        .FusCfg = {
            .NumScale = 0,
            .pFusInfo = NULL,
        },
        .OutputType = STEREO_OUT_TYPE_SPU_FEX,
        .EnableFusion = 0,
    },
    [STEREO_SCA_FS_02_UT] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 2,
            .AlgoInfo = {
                [0] = {
                    .pAlgoObj = &AmbaSpuFexAlgoObj,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                },
                [1] = {
                    .pAlgoObj = &AmbaSpuFusionAlgoObj,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                }
            }
        },
        .SpuCfg = {
            .NumScale = 0,
            .pScaleInfo = NULL,
        },
        .FexCfg = {
            .NumScale = 0,
            .pScaleInfo = NULL,
        },
        .FusCfg = {
            .NumScale = 2,
            .pFusInfo = NULL,
        },
        .OutputType = STEREO_OUT_TYPE_SPU_FEX,
        .EnableFusion = 1,
    },
    [STEREO_SCA_FS_024_UT] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG,
        .AlgoGrp = {
            .NumAlgo = 2,
            .AlgoInfo = {
                [0] = {
                    .pAlgoObj = &AmbaSpuFexAlgoObj,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                },
                [1] = {
                    .pAlgoObj = &AmbaSpuFusionAlgoObj,
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                }
            }
        },
        .SpuCfg = {
            .NumScale = 0,
            .pScaleInfo = NULL,
        },
        .FexCfg = {
            .NumScale = 0,
            .pScaleInfo = NULL,
        },
        .FusCfg = {
            .NumScale = 3,
            .pFusInfo = NULL,
        },
        .OutputType = STEREO_OUT_TYPE_SPU_FEX,
        .EnableFusion = 1,
    },
    [STEREO_SCA_LINUX_1] = {
        .ProfID = SVC_CV_FLOW_PROF_FLEXIDAG_LINUX,
        .AlgoGrp = {
            .NumAlgo = 1,
            .AlgoInfo = {
                [0] = {
#ifdef CONFIG_AMBALINK_BOOT_OS
                    .pAlgoObj = &LinuxAlgoObj,
#else
                    .pAlgoObj = NULL,
#endif
                    .NumFlexiBin = 0, .FlexiBinBits = 0x0,
                    .NumLcsBin = 0,
                    .NumExtBin = 0,
                },
            }
        },
        .EnableFusion = 1,
    },
};

