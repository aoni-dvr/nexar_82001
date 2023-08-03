/**
 *  @file Res_EMR_MX_9295_96712_IMX390.c
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

#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_96712_IMX390.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaVOUT_Def.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#include "AmbaFPD_MAXIM_Dual_TFT1280768.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvAppDef.h"
#endif

#ifdef CONFIG_MAX9295_96712_IMX390_20BIT
#define IMX390_SENSOR_MODE MX01_IMX390_1920_1080_60P_HDR1
#else
#define IMX390_SENSOR_MODE MX01_IMX390_1920_1080_A60P_HDR
#endif

#define IMX390_IQ    (IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16))

#define T3CH_SIDE_ACT_W 1280
#define T3CH_SIDE_ACT_H 768

#define T3CH_SIDE_MAIN_W 1280
#define T3CH_SIDE_MAIN_H 768

#define T3CH_SIDE_V36_MAIN_W 1024
#define T3CH_SIDE_V36_MAIN_H 576

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX96712+MX9295+IMX390 (ZS-1VOUT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1936_1106_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "3VIN-1VOUT with BSD",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 54U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 54, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .PipeMode = SVC_VDOPIPE_DRAMEFCY, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B , .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_DZOOM }
        },
        [1] = {
            .RawWin = {(1920 - T3CH_SIDE_V36_MAIN_W)/2, ((1080 - T3CH_SIDE_V36_MAIN_H)/2) + 1, T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .PipeMode = SVC_VDOPIPE_DRAMEFCY, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B , .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV }
        },
        [2] = {
            .RawWin = {(1920 - T3CH_SIDE_V36_MAIN_W)/2, ((1080 - T3CH_SIDE_V36_MAIN_H)/2) + 2, T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .PipeMode = SVC_VDOPIPE_DRAMEFCY, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B , .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {102, 58, 820, 460}, .DstWin = {1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {102, 58, 820, 460}, .DstWin = {0, 0, 1280, 768}
                    },
                },
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits =  (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}, .MaxWin = {T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits =  (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}, .MaxWin = {T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_SMALL,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_MAIN_YUV, .StrmId = 1U,   /* input is FOV_0 */
                        .FrameWidth = 1024, .FrameHeight = 512, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 64, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_SMALL,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_MAIN_YUV, .StrmId = 2U,   /* input is FOV_0 */
                        .FrameWidth = 1024, .FrameHeight = 512, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 64, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
    },
#endif
    .UserFlag = SVC_EMR_BSD,
},
{
    .FormatName = "(VIN0) MX96712+3x(MX9295+IMX390) (Cropping) (3VOUT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 54U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 54, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {(1920 - T3CH_SIDE_ACT_W)/2, ((1080 - T3CH_SIDE_ACT_H)/2) + 1, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {(1920 - T3CH_SIDE_ACT_W)/2, ((1080 - T3CH_SIDE_ACT_H)/2) + 2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, 1280, 768}
                    },
                },
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX96712+3x(MX9295+IMX390) (AmbaOD) (3VOUT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 54U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 54, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {(1920 - T3CH_SIDE_ACT_W)/2, ((1080 - T3CH_SIDE_ACT_H)/2) + 1, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {(1920 - T3CH_SIDE_ACT_W)/2, ((1080 - T3CH_SIDE_ACT_H)/2) + 2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, 1280, 768}
                    },
                },
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
#if 0//defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_AMBA_OD_FDAG,
            .CvModeID = AMBAOD_FDAG_EMR_1280x768_L0_1,
            .FovInputNum = 1U,
            .FovInput = {
                [0] = 1U,   /* input is FOV_1 */
            },
        },
        [1] = {
            .CvFlowType = SVC_CV_FLOW_AMBA_OD_FDAG,
            .CvModeID = AMBAOD_FDAG_EMR_1280x768_L0_1,
            .FovInputNum = 1U,
            .FovInput = {
                [0] = 2U,   /* input is FOV_2 */
            },
        },
    },
#endif
},
{
    .FormatName = "(VIN1) MX96712+3x(MX9295+IMX390)",
    .VinTree = {
        "$[VIN_1][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_1][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 54U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 54, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = {.SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {(1920 - T3CH_SIDE_ACT_W)/2, ((1080 - T3CH_SIDE_ACT_H)/2) + 1, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x1, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {(1920 - T3CH_SIDE_ACT_W)/2, ((1080 - T3CH_SIDE_ACT_H)/2) + 2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x1, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, 1280, 768}
                    },
                },
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX96172+3x(MX9295+IMX390) (DOWN-SCALE) (HDMI-3IN1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 54U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 54, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, /* downscale */
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, /* downscale */
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 504}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {960, 504, 960, 576}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin ={0, 504, 960, 576}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX96712+3x(MX9295+IMX390)(CROPPING)(AmbaOD)(HDMI-3IN1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 54U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 54, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {(1920 - T3CH_SIDE_ACT_W)/2, ((1080 - T3CH_SIDE_ACT_H)/2) + 1, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {(1920 - T3CH_SIDE_ACT_W)/2, ((1080 - T3CH_SIDE_ACT_H)/2) + 2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 504}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {960, 504, 960, 576}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin ={0, 504, 960, 576}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
#if 0//defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_AMBA_OD_FDAG,
            .CvModeID = AMBAOD_FDAG_EMR_1280x768_L0_1,
            .FovInputNum = 1U,
            .FovInput = {
                [0] = 1U,   /* input is FOV_1 */
            },
        },
        [1] = {
            .CvFlowType = SVC_CV_FLOW_AMBA_OD_FDAG,
            .CvModeID = AMBAOD_FDAG_EMR_1280x768_L0_1,
            .FovInputNum = 1U,
            .FovInput = {
                [0] = 2U,   /* input is FOV_2 */
            },
        },
    },
#endif
},
{
    .FormatName = "(VIN0) MX96712+MX9295+IMX390 1920x480 (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1936_1106_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920U, 480U}, .DstWin = {0, 0, 1920U, 480U}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX96712+MX9295+IMX390 1920x1080 (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1936_1106_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            //.ActWin = {0, 300, 1920, 480},  dzoom crop
            .MainWin = {1920, 1080},
            //.PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = 0,
            .StrmCfg = {
                .Win = {1920U, 1080}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080}, .DstWin = {0, 0, 1920U, 1080}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX96712+2x(MX9295+IMX390) (DZOOM-CROPPING) (HDMI-2IN1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 36U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 36, .Width = 1920, .Height = 2160}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 540}, /* dzoom crop */
            .MainWin = {1920, 540},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {0, 300, 1920, 540}, /* dzoom crop */
            .MainWin = {1920, 540},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 2,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 540}, .DstWin = {0, 0, 1920, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 540}, .DstWin ={0, 540, 1920, 540}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 540}, .MaxWin = {1920, 540}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 540}, .DstWin = {0, 0, 1920, 540}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 540}, .MaxWin = {1920, 540}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 540}, .DstWin = {0, 0, 1920, 540}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX96712+4x(MX9295+IMX390) (DZOOM-CROPPING) (HDMI-4IN1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_2][FOV_2]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_3][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 72U, .Width = 1920U, .Height = 4320},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 4,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 72, .Width = 1920, .Height = 4320}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 2 */
            [3] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 720)/2, 1280, 720}, /* dzoom crop */
            .MainWin = {1280, 720},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 720)/2, 1280, 720}, /* dzoom crop */
            .MainWin = {1280, 720},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 720)/2, 1280, 720}, /* dzoom crop */
            .MainWin = {1280, 720},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [3] = {
            .RawWin = {0, 3, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 720)/2, 1280, 720}, /* dzoom crop */
            .MainWin = {1280, 720},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 4U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 960, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {960, 0, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 720}, .DstWin ={0, 540, 960, 540}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1280, 720}, .DstWin ={960, 540, 960, 540}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 4U, .RecBits = 0x0fU,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 3, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX96712+2x(MX9295+IMX390) (DZOOM-CROPPING) (2VOUT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 36U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 36, .Width = 1920, .Height = 2160}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x1, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x1, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .MaxWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX96712+MX9295+IMX390 (AmbaOD) (ZS-1VOUT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1936_1106_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
#if 0//defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_AMBA_OD_FDAG,
            .CvModeID = AMBAOD_FDAG_EMR_1920x480_L0_1,
            .FovInputNum = 1U,
            .FovInput = {
                [0] = 0U,   /* input is FOV_0 */
            },
        },
    },
#endif
},
{
    .FormatName = "2VIN-2VOUT with BSD(Main:1600x900 CV:1024x512)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 16U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 16, .Width = 1920, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {1600, 900},
            .PyramidBits = 0x2,
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {1600, 900},
            .PyramidBits = 0x2,
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {160, 90, 1280, 720}, .DstWin = {1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {160, 90, 1280, 720}, .DstWin = {0, 0, 1280, 768}
                    },
                },
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#if 1
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits =  (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {900, 900}, .MaxWin = {1600, 900}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 900}, .DstWin = {0, 0, 1600, 900}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits =  (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 900}, .MaxWin = {1600, 900}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1600, 900}, .DstWin = {0, 0, 1600, 900}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
#endif
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_SMALL,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1024, .FrameHeight = 512, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 54, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_SMALL,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 1024, .FrameHeight = 512, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 54, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
    },
#endif
    .UserFlag = SVC_EMR_BSD,
},
{
    .FormatName = "2VIN-2VOUT with BSD(Main:1600x900 CV:896x512)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 16U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 16, .Width = 1920, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {1600, 900},
            .PyramidBits = 0x2,
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {1600, 900},
            .PyramidBits = 0x2,
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {160, 90, 1280, 720}, .DstWin = {1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {160, 90, 1280, 720}, .DstWin = {0, 0, 1280, 768}
                    },
                },
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#if 1
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits =  (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {900, 900}, .MaxWin = {1600, 900}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 900}, .DstWin = {0, 0, 1600, 900}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits =  (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 900}, .MaxWin = {1600, 900}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1600, 900}, .DstWin = {0, 0, 1600, 900}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
#endif
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_896x512,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 896, .FrameHeight = 512, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 118, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_896x512,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 896, .FrameHeight = 512, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 118, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
    },
#endif
    .UserFlag = SVC_EMR_BSD,
},
};

#endif /* RES_X_X_H */
