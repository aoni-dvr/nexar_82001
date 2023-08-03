/**
 *  @file Res_SUR_MX_9295_96712_AR0233.c
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
#include "AmbaDSP_ImageFilter.h"

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

#define AR0233_IQ    (IQ_SSR_MAX9295_9296_AR0233 | (IMG_SENSOR_HDR_MODE_0 << 16))

#define AR0233_LINEAR_IQ    (IQ_SSR_MAX9295_9296_AR0233 | (IMG_NORMAL_MODE << 16))

#define T3CH_SIDE_ACT_W 1280
#define T3CH_SIDE_ACT_H 768

#define T3CH_SIDE_MAIN_W 1280
#define T3CH_SIDE_MAIN_H 768

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX96712+MX9295+AR0233 1920x1080 (HDMI)",
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
                .SensorMode = MX01_AR0233_1920_1080_A30P_MODE6,
                .SensorGroup = 0,
                .IQTable = AR0233_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x30, .HierWin = {0U, 0U},
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
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
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 40, 180, 0, 0},
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
    .FormatName = "(VIN0) MX96712+4x(MX9295+AR0233)  (SUR)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0][FOV_4]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_AR0233_1][FOV_1][FOV_5]#\
         $[VIN_0][B_MAXIM96712_2][B_MAXIM9295_0][S_AR0233_2][FOV_2][FOV_6]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_AR0233_3][FOV_3][FOV_7]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 4320},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 4,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 1920, .Height = 4320}
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
                .SensorMode = MX01_AR0233_1920_1200_30P,
                .SensorGroup = 0,
                .IQTable = AR0233_LINEAR_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_AR0233_1920_1200_30P,
                .SensorGroup = 0,
                .IQTable = AR0233_LINEAR_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_AR0233_1920_1200_30P,
                .SensorGroup = 0,
                .IQTable = AR0233_LINEAR_IQ,
            }, /* sensor idx 2 */
            [3] = {
                .SensorMode = MX01_AR0233_1920_1200_30P,
                .SensorGroup = 0,
                .IQTable = AR0233_LINEAR_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 8U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [3] = {
            .RawWin = {0, 3, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [4] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {320, 140, 1280, 720},
            .MainWin = {1280, 800},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [5] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {320, 140, 1280, 720},
            .MainWin = {1280, 800},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [6] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {320, 140, 1280, 720},
            .MainWin = {1280, 800},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [7] = {
            .RawWin = {0, 3, 1920, 1080},
            .ActWin = {320, 140, 1280, 720},
            .MainWin = {1280, 800},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
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
                .Win = {864, 1056}, .MaxWin = {1920, 1080}, .NumChan = 4U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1280, 800}, .DstWin = {0, 0, 864, 336}, .RotateFlip = AMBA_DSP_ROTATE_0,
                    },
                    [1] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1280, 800}, .DstWin = {0, 720, 864, 336}, .RotateFlip = AMBA_DSP_ROTATE_180,
                    },
                    [2] = {
                        .FovId = 6U, .SrcWin = {0, 0, 1280, 800}, .DstWin ={0, 0, 384, 1056}, .RotateFlip = AMBA_DSP_ROTATE_270,
                    },
                    [3] = {
                        .FovId = 7U, .SrcWin = {0, 0, 1280, 800}, .DstWin ={580, 0, 384, 1056}, .RotateFlip = AMBA_DSP_ROTATE_90,
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
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
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 540}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 960, 540}, .DstWin = {0, 0, 960, 540}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 540}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 960, 540}, .DstWin = {0, 0, 960, 540}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 540}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0, 0, 960, 540}, .DstWin = {0, 0, 960, 540}
                    },
                }
            }
        }
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 40, 180, 0, 0},
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
    .FormatName = "(VIN0) MX96712+4x(MX9295+AR0233)  (NORMAL)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_AR0233_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_2][B_MAXIM9295_0][S_AR0233_2][FOV_2]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_AR0233_3][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 4320},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 4,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 1920, .Height = 4320}
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
                .SensorMode = MX01_AR0233_1920_1200_30P,
                .SensorGroup = 0,
                .IQTable = AR0233_LINEAR_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_AR0233_1920_1200_30P,
                .SensorGroup = 0,
                .IQTable = AR0233_LINEAR_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_AR0233_1920_1200_30P,
                .SensorGroup = 0,
                .IQTable = AR0233_LINEAR_IQ,
            }, /* sensor idx 2 */
            [3] = {
                .SensorMode = MX01_AR0233_1920_1200_30P,
                .SensorGroup = 0,
                .IQTable = AR0233_LINEAR_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 8U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x10, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x10, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x10, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [3] = {
            .RawWin = {0, 3, 1920, 1080},
            .ActWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x10, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
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
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 960, 540},
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {960, 0, 960, 540},
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 540, 960, 540},
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 540, 960, 540},
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
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
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 540}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 960, 540}, .DstWin = {0, 0, 960, 540}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 540}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 960, 540}, .DstWin = {0, 0, 960, 540}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 540}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0, 0, 960, 540}, .DstWin = {0, 0, 960, 540}
                    },
                }
            }
        }
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 40, 180, 0, 0},
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
    .FormatName = "(VIN0) MX96172+(MX9295+AR0233) (HDMI) (CALIB)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_AR0233_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1936U, .Height = 1106U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_AR0233_1920_1080_A30P_MODE6,
                .SensorGroup = 0,
                .IQTable = AR0233_IQ,
            }, /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1936, 1106},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B,}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
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
    },
},
{   /* XCODE RefOD */
    .FormatName = "DEC Pyramid (DEC-Only) (FDAG)",
    .VinTree = {
        "$[DEC_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 0U,
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .PyramidBits = 0x3F, .HierWin = {0, 0},
            .PipeCfg = { 0 }
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
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 40, 180, 0, 0},
                        }
                    },
                },
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
};

#endif /* RES_X_X_H */

