/**
 *  @file Res_EMR_TI_953_954_AR0233.c
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
#include "AmbaSensor_MAX9295_96712_AR0233.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaVOUT_Def.h"
#include "AmbaFPD_LT9611UXC.h"

#include "AmbaVOUT_Def.h"
#include "AmbaFPD_LT9611UXC.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#include "AmbaFPD_MAXIM_TFT1280768.h"
#include "AmbaDSP_ImageFilter.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvAppDef.h"
#endif


#define AR0233_IQ    (IQ_SSR_MAX9295_96712_AR0233 | (IMG_SENSOR_HDR_MODE_0 << 16))

#define T3CH_SIDE_V36_MAIN_W 1024
#define T3CH_SIDE_V36_MAIN_H 576

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX96712+MX9295+AR0233 1920x1080p60 (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                },
            },
            .DevMode = (UINT8) AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
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
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX96712+3x(MX9295+AR0233)(HDMI-3IN1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_AR0233_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_AR0233_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
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
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .MainWin = {1024, 576},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .MainWin = {1024, 576},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 504},
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1024, 576}, .DstWin = {960, 504, 960, 576},
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1024, 576}, .DstWin = {0, 504, 960, 576},
                    },
                },
            },
            .DevMode = (UINT8) AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
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
                .SrcBits = SVC_REC_SRC_VIDEO,
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1024, 576}, .MaxWin = {1024, 576}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1024, 576}, .DstWin = {0, 0, 1024, 576}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1024, 576}, .MaxWin = {1024, 576}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1024, 576}, .DstWin = {0, 0, 1024, 576}
                    },
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "3VIN-1VOUT with BSD",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_AR0233_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_AR0233_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
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
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_DZOOM },
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H},
            .PyramidBits = 0x1, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H},
            .PyramidBits = 0x1, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
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
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {150, 58, 724, 460}, .DstWin = {1496, 0, 424, 270}, .BlendEnable = SVC_RES_BLD_TBL_FROM_ROM, .BlendTable = { .RomFileName = "Bld_Tbl_Left_BSD.bmp", .Win = {0, 0, 424, 270} },
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {150, 58, 724, 460}, .DstWin ={0, 0, 424, 270}, .BlendEnable = SVC_RES_BLD_TBL_FROM_ROM, .BlendTable = { .RomFileName = "Bld_Tbl_Right_BSD.bmp", .Win = {0, 0, 424, 270} },
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_SINGLE_A_A60HZ,
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_0 */
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 2U,   /* input is FOV_0 */
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
    .StillCfg.EnableStill = 0,
    .UserFlag = SVC_EMR_BSD,
},
{
    .FormatName = "3VIN-3VOUT with BSD",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_AR0233_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_AR0233_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
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
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_DZOOM }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H},
            .PyramidBits = 0x1, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {160, 90, 1600, 900},
            .MainWin = {T3CH_SIDE_V36_MAIN_W, T3CH_SIDE_V36_MAIN_H},
            .PyramidBits = 0x1, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV }
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
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_TFT128Obj,
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
            .DevMode = (UINT8)AMBA_FPD_MAXIM_DUAL_A60HZ,
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_0 */
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 2U,   /* input is FOV_0 */
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
    .StillCfg.EnableStill = 0,
    .UserFlag = SVC_EMR_BSD,
},
{
    .FormatName = "3VIN-1VOUT dynamic stitching",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_2]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_AR0233_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_AR0233_2][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_EEPROM_MAX9295_96712_MICRO_24AA256
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL) | ((UINT32) 1U << 16U),
                .IQTable = AR0233_IQ | (AR0233_IQ << 8U),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL) | ((UINT32) 1U << 16U),
                .IQTable = AR0233_IQ | (AR0233_IQ << 8U),
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL) | ((UINT32) 1U << 16U),
                .IQTable = AR0233_IQ | (AR0233_IQ << 8U),
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1024, 512},
            .PyramidBits = 0x1, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0xC000, /* b'1100 0000 0000 0000 enable calib ID 14,15 */
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .MainWin = {1024, 512},
            .PyramidBits = 0x1, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0xC000, /* b'1100 0000 0000 0000 enable calib ID 14,15 */
        },
        [0] = {
            .RawWin = {0, 2, 1920, 1080},
            .MainWin = {1024, 512},
            .PyramidBits = 0x1, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0xC000, /* b'1100 0000 0000 0000 enable calib ID 14,15 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1024, 512}, .DstWin = {480, 0, 960, 480}, .BlendEnable = SVC_RES_BLD_TBL_FROM_CALIB,
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1024, 512}, .DstWin = {960, 0, 960, 480}, .Margin = {0, 0, 0, 0}
                    },
                    [2] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1024, 512}, .DstWin = {0, 0, 960, 480}, .Margin = {0, 0, 0, 0}
                    },
                },
            },
            .DevMode = (UINT8) AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1024, 512}, .MaxWin = {1024, 512}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1024, 512}, .DstWin = {0, 0, 1024, 512}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_SMALL,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 2U,   /* input is FOV_2 */
                        .FrameWidth = 1024, .FrameHeight = 512, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
    },
#endif
    .UserFlag = SVC_EMR_STITCH,
},
{   /* XCODE RefOD */
#define DEC_WIDTH  T3CH_SIDE_V36_MAIN_W
#define DEC_HEIGHT T3CH_SIDE_V36_MAIN_H
    .FormatName = "DEC Pyramid (DEC-Only)(HDMI out)(FDAG)",
    .VinTree = {
        "$[DEC_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 0U,
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, DEC_WIDTH, DEC_HEIGHT},
            .ActWin = {0},
            .MainWin = {DEC_WIDTH, DEC_HEIGHT},
            .PyramidBits = 0x1,
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, DEC_WIDTH, DEC_HEIGHT}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8) AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
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
                            [0] = {0, 0, 64, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
    },
#endif
    .UserFlag = SVC_EMR_BSD,
},
{
    .FormatName = "(VIN0) 3x1920x1080p60 (Full FOV)(3VOUT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_AR0233_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_AR0233_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
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
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 480}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 0, 1280, 768}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_DUAL_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO  | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
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
                .SrcBits =  (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits =  (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                },
            },
        },
    },
},
{
    .FormatName = "1VIN-1VOUT",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 24, 1920, 724}, /* dzoom crop */
            .MainWin = {1920, 724},
            .PyramidBits = 0x2, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_DZOOM }
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
                        .FovId = 0U, .SrcWin = {0, 122, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 724}, .MaxWin = {1920, 724}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 724}, .DstWin = {0, 0, 1920, 724}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_SMALL,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 724, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
    },
#endif
    .UserFlag = SVC_EMR_POSITION,
},
{
    .FormatName = "1VIN-1VOUT RMG (Rear Mirror Guard)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_AR0233_1920_1080_A60P_MODE6,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = AR0233_IQ,
            }, /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 24, 1920, 724}, /* dzoom crop */
            .MainWin = {1920, 724},
            .PyramidBits = 0x2, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_DZOOM }
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
                        .FovId = 0U, .SrcWin = {0, 122, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 724}, .MaxWin = {1920, 724}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 724}, .DstWin = {0, 0, 1920, 724}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_SMALL,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 724, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
    },
#endif
    .UserFlag = SVC_EMR_RMG,
},
{
    .FormatName = "DEC RMG (DEC-Only)(HDMI out)(FDAG)",
    .VinTree = {
        "$[DEC_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 0U,
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 720},
            .ActWin = {0},
            .MainWin = {1920, 720},
            .PyramidBits = 0x2,
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 720}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 720}, .DstWin = {0, 0, 1920, 720}
                    },
                }
            },
            .DevMode = (UINT8) AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_EMR_SMALL,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 720, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
    },
#endif
    .UserFlag = SVC_EMR_RMG,
},
};

#endif /* RES_X_X_H */
