/**
 *  @file Res_SUR_LT6911.c
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
 *  @details svc resolution config
 *
 */

#ifndef RES_X_X_H       /* Dir 4.10 */
#define RES_X_X_H

#include "AmbaYuv.h"
#include "AmbaYuv_LT6911.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaFPD_LT9611UXC.h"
#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvAppDef.h"
#endif

static const SVC_RES_CFG_s g_ResCfg[] = {
{   /*LT6911_1920_1080_A30P*/
    .FormatName = "(VIN0) LT6911 1080P30",
    .VinTree = {
        "$[VIN_0][Y_LT6911_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaYuv_LT6911Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .FixedFrameRate = 1,
        },
    },
    /* sensor info */
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = LT6911_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_DUMMY,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920U, 1080U},
            .MainWin = {1280, 720},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x00U,
    .StillCfg.EnableStill = 0,
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaSegObj,
            .CvModeID = AMBASEG_FDAG_AVM,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1280, .FrameHeight = 720, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 104, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }

#endif
},
{   /*LT6911_1920_1080_30P FDAG*/
    .FormatName = "(VIN0) LT6911 1080P30 4FOV Single View(CV FDAG)",
    .VinTree = {
        "$[VIN_0][Y_LT6911_0][FOV_0][FOV_1][FOV_2][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaYuv_LT6911Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
            .FixedFrameRate = 1,
        },
    },
    /* sensor info */
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = LT6911_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_DUMMY,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 960, 540},
            .MainWin = {1280, 720},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
        [1] = {
            .RawWin = {960, 0, 960, 540},
            .MainWin = {1280, 720},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
        [2] = {
            .RawWin = {0, 540, 960, 540},
            .MainWin = {1280, 720},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
        [3] = {
            .RawWin = {960, 540, 960, 540},
            .MainWin = {1280, 720},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .DispAltNum = 4, .DispAltIdx = 1,
    .DispAlt = {
        [0] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_LT9611UXCObj,
                    .StrmCfg = {
                        .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                            .ChanCfg = {
                                [0] = {
                                    .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}, .RotateFlip = AMBA_DSP_ROTATE_0,
                                },
                            },

                    },
                    .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
                    .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
                },
            },
        },
        [1] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_LT9611UXCObj,
                    .StrmCfg = {
                        .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                            .ChanCfg = {
                                [0] = {
                                    .FovId = 1U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}, .RotateFlip = AMBA_DSP_ROTATE_0,
                                },
                            },

                    },
                    .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
                    .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
                },
            },
        },
        [2] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_LT9611UXCObj,
                    .StrmCfg = {
                        .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                            .ChanCfg = {
                                [0] = {
                                    .FovId = 2U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}, .RotateFlip = AMBA_DSP_ROTATE_0,
                                },
                            },

                    },
                    .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
                    .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
                },
            },
        },
        [3] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_LT9611UXCObj,
                    .StrmCfg = {
                        .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                            .ChanCfg = {
                                [0] = {
                                    .FovId = 3U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}, .RotateFlip = AMBA_DSP_ROTATE_0,
                                },
                            },

                    },
                    .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
                    .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
                },
            },
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x00U,
    .StillCfg.EnableStill = 1,
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaSegObj,
            .CvModeID = AMBASEG_FDAG_AVM,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_0 */
                        .FrameWidth = 1280, .FrameHeight = 720, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 104, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
};

#endif /* RES_X_X_H */
