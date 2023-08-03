/**
 *  @file Res_SUR_MX_9295_96712_IMX224.c
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
#include "AmbaSensor_MAX9295_96712_IMX224.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaVOUT_Def.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvAppDef.h"
#endif

#ifdef CONFIG_ICAM_EEPROM_USED
#include "AmbaEEPROM.h"
extern AMBA_EEPROM_OBJ_s AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj;
#endif

#define IMX224_IQ    (IQ_SSR_MAX9295_96712_IMX224 | (IMG_NORMAL_MODE << 16))

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX96712+MX9295+IMX224 1920x1080 (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX224_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX224Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 16U, .OffsetY = 8U, .Width = 1280U, .Height = 960U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX224_1280_960_30P,
                .SensorGroup = 0,
                .IQTable = IMX224_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
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
                .Win = {1440U, 1080}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280U, 960}, .DstWin = {0, 0, 1440U, 1080}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1280, .FrameHeight = 960, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 224, 0, 0},
                        }
                    },
                },
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX96712+4x(MX9295+IMX224) SINGLE VIEW ",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX224_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX224_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX224_2][FOV_2]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX224_3][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX224Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 16U, .OffsetY = 8U, .Width = 1280U, .Height = 3840},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 4,
                       .CaptureWindow = {.OffsetX = 16, .OffsetY = 8, .Width = 1280, .Height = 3840}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_IMX224_1280_960_30P,
                .SensorGroup = 0,
                .IQTable = IMX224_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_IMX224_1280_960_30P,
                .SensorGroup = 0,
                .IQTable = IMX224_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_IMX224_1280_960_30P,
                .SensorGroup = 0,
                .IQTable = IMX224_IQ,
            }, /* sensor idx 2 */
            [3] = {
                .SensorMode = MX01_IMX224_1280_960_30P,
                .SensorGroup = 0,
                .IQTable = IMX224_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [1] = {
            .RawWin = {0, 1, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [2] = {
            .RawWin = {0, 2, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [3] = {
            .RawWin = {0, 3, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1440, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 0U, .SrcWin = {0, 0, 1280, 960}, .DstWin = {0, 0, 1440, 1080}, .RotateFlip = AMBA_DSP_ROTATE_0,
                        },
                    },

            },
            .DevMode = HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .DispAltNum = 4, .DispAltIdx = 1,
    .DispAlt = {
        [0] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_HDMI_Obj,
                    .StrmCfg = {
                        .Win = {1440, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                            .ChanCfg = {
                                [0] = {
                                    .FovId = 0U, .SrcWin = {0, 0, 1280, 960}, .DstWin = {0, 0, 1440, 1080}, .RotateFlip = AMBA_DSP_ROTATE_0,
                                },
                            },

                    },
                    .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
                    .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
                },
            },
        },
        [1] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_HDMI_Obj,
                    .StrmCfg = {
                        .Win = {1440, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                            .ChanCfg = {
                                [0] = {
                                    .FovId = 1U, .SrcWin = {0, 0, 1280, 960}, .DstWin = {0, 0, 1440, 1080}, .RotateFlip = AMBA_DSP_ROTATE_0,
                                },
                            },

                    },
                    .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
                    .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
                },
            },
        },
        [2] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_HDMI_Obj,
                    .StrmCfg = {
                        .Win = {1440, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                            .ChanCfg = {
                                [0] = {
                                    .FovId = 2U, .SrcWin = {0, 0, 1280, 960}, .DstWin = {0, 0, 1440, 1080}, .RotateFlip = AMBA_DSP_ROTATE_0,
                                },
                            },

                    },
                    .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
                    .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
                },
            },
        },
        [3] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_HDMI_Obj,
                    .StrmCfg = {
                        .Win = {1440, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                            .ChanCfg = {
                                [0] = {
                                    .FovId = 3U, .SrcWin = {0, 0, 1280, 960}, .DstWin = {0, 0, 1440, 1080}, .RotateFlip = AMBA_DSP_ROTATE_0,
                                },
                            },

                    },
                    .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
                    .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
                },
            },
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1920U}, .MaxWin = {2560U, 1920U}, .NumChan = 4U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280U, 960U}, .DstWin = {0, 0, 1280U, 960U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280U, 960U}, .DstWin = {1280U, 0, 1280U, 960U}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280U, 960U}, .DstWin = {0, 960U, 1280U, 960U}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1280U, 960U}, .DstWin = {1280U, 960U, 1280U, 960U}
                    },
                }
            }
        },
    },
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
                        .FrameWidth = 1280, .FrameHeight = 960, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 224, 0, 0},
                        }
                    },
                },
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX96712+4x(MX9295+IMX224) SUR VIEW ",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX224_0][FOV_0][FOV_4]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX224_1][FOV_1][FOV_5]#\
         $[VIN_0][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX224_2][FOV_2][FOV_6]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX224_3][FOV_3][FOV_7]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX224Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 16U, .OffsetY = 8U, .Width = 1280U, .Height = 3840},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 4,
                       .CaptureWindow = {.OffsetX = 16, .OffsetY = 8, .Width = 1280, .Height = 3840}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_IMX224_1280_960_30P,
                .SensorGroup = 0,
                .IQTable = IMX224_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_IMX224_1280_960_30P,
                .SensorGroup = 0,
                .IQTable = IMX224_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_IMX224_1280_960_30P,
                .SensorGroup = 0,
                .IQTable = IMX224_IQ,
            }, /* sensor idx 2 */
            [3] = {
                .SensorMode = MX01_IMX224_1280_960_30P,
                .SensorGroup = 0,
                .IQTable = IMX224_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 8U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [1] = {
            .RawWin = {0, 1, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [2] = {
            .RawWin = {0, 2, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [3] = {
            .RawWin = {0, 3, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PyramidBits = 0x01, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [4] = {
            .RawWin = {0, 0, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [5] = {
            .RawWin = {0, 1, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [6] = {
            .RawWin = {0, 2, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [7] = {
            .RawWin = {0, 3, 1280, 960},
            .ActWin = {0, 0, 1280, 960},
            .MainWin = {1280, 960},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {512, 704}, .MaxWin = {1280, 720}, .NumChan = 4U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 4U, .SrcWin = {0, 0, 1280, 960}, .DstWin = {0, 0, 512, 224}, .RotateFlip = AMBA_DSP_ROTATE_0,
                        },
                        [1] = {
                            .FovId = 5U, .SrcWin = {0, 0, 1280, 960}, .DstWin = {0, 480, 512, 224}, .RotateFlip = AMBA_DSP_ROTATE_180,
                        },
                        [2] = {
                            .FovId = 6U, .SrcWin = {0, 0, 1280, 960}, .DstWin ={0, 0, 256, 704}, .RotateFlip = AMBA_DSP_ROTATE_270,
                        },
                        [3] = {
                            .FovId = 7U, .SrcWin = {0, 0, 1280, 960}, .DstWin ={256, 0, 256, 704}, .RotateFlip = AMBA_DSP_ROTATE_90,
                        },
                    },
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1920U}, .MaxWin = {2560U, 1920U}, .NumChan = 4U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280U, 960U}, .DstWin = {0, 0, 1280U, 960U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280U, 960U}, .DstWin = {1280U, 0, 1280U, 960U}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280U, 960U}, .DstWin = {0, 960U, 1280U, 960U}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1280U, 960U}, .DstWin = {1280U, 960U, 1280U, 960U}
                    },
                }
            }
        },
    },
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
                        .FrameWidth = 1280, .FrameHeight = 960, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 224, 0, 0},
                        }
                    },
                },
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
}

};

#endif /* RES_X_X_H */

