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
#include "AmbaSPI_Def.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"


#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_RefSeg.h"
#include "SvcCvFlow_RefOD_SharedFD.h"
#include "SvcCvAppDef.h"
#endif

#define IMX390_IQ    (IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16))

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN8) MX96712+MX9295+IMX390 1920x1080p60 (HDMI)",
    .VinTree = {
        "$[VIN_8][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
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
            .PipeCfg = {.LinearCE = 1U}
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
                .UseAuxStg = 1,
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
},

{
    .FormatName = "(VIN8) 3x1920x1080p60 (HDMI-3IN1)",
    .VinTree = {
        "$[VIN_8][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_8][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_8][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
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
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
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
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.Rotate7Flip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 270, 1920, 540}, .DstWin = {0, 0, 1920, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 540, 960, 540}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
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
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN8, 9) 2x1920x1080p60 (HDMI 2-in-1)",
    .VinTree = {
        "$[VIN_8][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_9][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [9] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [9] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 540}, .MaxWin = {1920, 1080}, .NumChan = 2,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 960, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 0, 960, 540}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
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
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},

{
    .FormatName = "(VIN0, 1, 2) 3x1920x1080p60 (HDMI 3-in-1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_2][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 3U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [1] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [2] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 270, 1920, 540}, .DstWin = {0, 0, 1920, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 540, 960, 540}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
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
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},
{
    .FormatName = "(VIN4, 5, 6) 3x1920x1080p60 (HDMI 3-in-1)",
    .VinTree = {
        "$[VIN_4][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_5][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_6][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 3U,
    .VinCfg = {
        [4] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [5] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [6] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [4] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [5] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [6] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 270, 1920, 540}, .DstWin = {0, 0, 1920, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 540, 960, 540}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
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
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},
{
    .FormatName = "(VIN8, 9, 10) 3x1920x1080p60 (HDMI 3-in-1)",
    .VinTree = {
        "$[VIN_8][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_9][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_10][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 3U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [9] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [10] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [9] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [10] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 270, 1920, 540}, .DstWin = {0, 0, 1920, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 540, 960, 540}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
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
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},

{
    .FormatName = "(VIN11, 12, 13) 3x1920x1080p60 (HDMI 3-in-1)",
    .VinTree = {
        "$[VIN_11][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_12][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_13][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 3U,
    .VinCfg = {
        [11] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [12] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [13] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [11] = { /* sensor on VIN_11 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [12] = { /* sensor on VIN_12 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [13] = { /* sensor on VIN_13 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 270, 1920, 540}, .DstWin = {0, 0, 1920, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 540, 960, 540}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
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
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},

{
    .FormatName = "(VIN0, 4, 8, 11) 4x1920x1080p30 (HDMI 4-in-1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_4][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_8][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_2]#\
         $[VIN_11][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 4U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [4] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [8] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [11] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [4] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [8] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [11] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 4,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 960, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 0, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 540, 960, 540}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 540, 960, 540}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
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
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},

{
    .FormatName = "(VIN0, 1, 2, 4, 5, 6) 6x1920x1080p30 (HDMI 6-in-1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_2][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#\
         $[VIN_4][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#\
         $[VIN_5][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_4]#\
         $[VIN_6][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_5]#!"
    },
    /* vin info */
    .VinNum = 6U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [4] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [5] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
        [6] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [1] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [2] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [4] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [5] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [6] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 6U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [4] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [5] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 720}, .MaxWin = {1920, 1080}, .NumChan = 6,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 640, 360}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={640, 0, 640, 360}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {1280, 0, 640, 360}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 360, 640, 360}
                    },
                    [4] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={640, 360, 640, 360}
                    },
                    [5] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1280, 360, 640, 360}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
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
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
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
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},
{
    .FormatName = "(VIN0, 1, 2, 11, 12, 13) 6x1920x1080p30 (HDMI 6-in-1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_2][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#\
         $[VIN_11][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#\
         $[VIN_12][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_4]#\
         $[VIN_13][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_5]#!"
    },
    /* vin info */
    .VinNum = 6U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [11] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [12] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [13] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [1] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [2] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [11] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [12] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [13] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 6U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [4] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [5] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 720}, .MaxWin = {1920, 1080}, .NumChan = 6,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 640, 360}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={640, 0, 640, 360}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {1280, 0, 640, 360}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 360, 640, 360}
                    },
                    [4] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={640, 360, 640, 360}
                    },
                    [5] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1280, 360, 640, 360}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
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
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},
{
    .FormatName = "(VIN0-2, 4-6, 11-13) 9x1920x1080p30 (HDMI 9-in-1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_2][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#\
         $[VIN_4][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#\
         $[VIN_5][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_4]#\
         $[VIN_6][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_5]#\
         $[VIN_11][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_6]#\
         $[VIN_12][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_7]#\
         $[VIN_13][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_8]#!"
    },
    /* vin info */
    .VinNum = 9U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [4] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [5] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [6] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [11] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [12] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [13] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [1] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [2] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [4] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [5] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [6] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [11] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [12] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [13] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 9U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [4] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [5] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [6] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [7] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [8] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 9,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 640, 360}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={640, 0, 640, 360}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {1280, 0, 640, 360}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 360, 640, 360}
                    },
                    [4] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={640, 360, 640, 360}
                    },
                    [5] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1280, 360, 640, 360}
                    },
                    [6] = {
                        .FovId = 6U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 720, 640, 360}
                    },
                    [7] = {
                        .FovId = 7U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={640, 720, 640, 360}
                    },
                    [8] = {
                        .FovId = 8U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1280, 720, 640, 360}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
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
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},

{
    .FormatName = "(VIN0-2, 4-6, 8-10, 11-13) 12x1920x1080p30 (HDMI 12-in-1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_2][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#\
         $[VIN_4][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#\
         $[VIN_5][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_4]#\
         $[VIN_6][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_5]#\
         $[VIN_8][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_6]#\
         $[VIN_9][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_7]#\
         $[VIN_10][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_8]#\
         $[VIN_11][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_9]#\
         $[VIN_12][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_10]#\
         $[VIN_13][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_11]#!"
    },
    /* vin info */
    .VinNum = 12U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [4] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [5] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [6] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [8] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [9] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [10] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [11] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [12] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [13] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [1] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [2] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [4] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [5] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [6] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [8] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [9] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [10] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [11] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [12] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [13] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 12U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [4] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [5] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [6] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [7] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [8] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [9] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [10] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [11] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U /*, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 12,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 480, 360}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={480, 0, 480, 360}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {960, 0, 480, 360}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1440, 0, 480, 360}
                    },
                    [4] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 360, 480, 360}
                    },
                    [5] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={480, 360, 480, 360}
                    },
                    [6] = {
                        .FovId = 6U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 360, 480, 360}
                    },
                    [7] = {
                        .FovId = 7U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1440, 360, 480, 360}
                    },
                    [8] = {
                        .FovId = 8U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 720, 480, 360}
                    },
                    [9] = {
                        .FovId = 9U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={480, 720, 480, 360}
                    },
                    [10] = {
                        .FovId = 10U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 720, 480, 360}
                    },
                    [11] = {
                        .FovId = 11U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1440, 720, 480, 360}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 12U, .RecBits = 0xFFFU,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
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
                .UseAuxStg = 0,
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [4] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 4, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [5] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 5, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [6] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 6, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 6U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [7] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 7, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 7U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [8] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 8, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 8U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [9] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 9, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 9U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [10] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 10, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 10U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [11] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 11, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 11U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},
{
    .FormatName = "(VIN0-2, 4-6, 8-10, 11-13) 12x1920x1080p30 (HDMI 12-in-1) (Open OD)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_2][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#\
         $[VIN_4][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#\
         $[VIN_5][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_4]#\
         $[VIN_6][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_5]#\
         $[VIN_8][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_6]#\
         $[VIN_9][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_7]#\
         $[VIN_10][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_8]#\
         $[VIN_11][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_9]#\
         $[VIN_12][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_10]#\
         $[VIN_13][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_11]#!"
    },
    /* vin info */
    .VinNum = 12U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [4] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [5] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [6] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [8] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [9] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [10] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [11] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [12] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [13] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [1] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [2] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [4] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [5] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [6] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [8] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [9] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [10] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [11] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [12] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [13] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 12U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [4] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [5] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [6] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [7] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [8] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [9] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [10] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
        [11] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U  /*, .RawCompression = IK_RAW_COMPACT_12B */ }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 12,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 480, 360}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={480, 0, 480, 360}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {960, 0, 480, 360}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1440, 0, 480, 360}
                    },
                    [4] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 360, 480, 360}
                    },
                    [5] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={480, 360, 480, 360}
                    },
                    [6] = {
                        .FovId = 6U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 360, 480, 360}
                    },
                    [7] = {
                        .FovId = 7U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1440, 360, 480, 360}
                    },
                    [8] = {
                        .FovId = 8U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 720, 480, 360}
                    },
                    [9] = {
                        .FovId = 9U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={480, 720, 480, 360}
                    },
                    [10] = {
                        .FovId = 10U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 720, 480, 360}
                    },
                    [11] = {
                        .FovId = 11U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1440, 720, 480, 360}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 12U, .RecBits = 0xFFFU,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
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
                .UseAuxStg = 0,
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [4] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 4, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [5] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 5, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [6] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 6, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 6U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [7] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 7, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 7U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [8] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 8, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 8U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [9] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 9, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 9U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [10] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 10, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 10U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [11] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 11, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 11U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,

#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 12, .CvFlowBits = 0xFFFU,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [2] = {
            .Chan = 2U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 2U,   /* input is FOV_2 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 2U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [3] = {
            .Chan = 3U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 3U,   /* input is FOV_3 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 3U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [4] = {
            .Chan = 4U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 4U,   /* input is FOV_4 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 4U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [5] = {
            .Chan = 5U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 5U,   /* input is FOV_5 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 5U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [6] = {
            .Chan = 6U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 6U,   /* input is FOV_6 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 6U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [7] = {
            .Chan = 7U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 7U,   /* input is FOV_7 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 7U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [8] = {
            .Chan = 8U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 8U,   /* input is FOV_8 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 8U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [9] = {
            .Chan = 9U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 9U,   /* input is FOV_9 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 9U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [10] = {
            .Chan = 10U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 10U,   /* input is FOV_10 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 10U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [11] = {
            .Chan = 11U, .pCvObj = &SvcCvFlow_RefOD_SHFD_Obj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 11U,   /* input is FOV_11 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 11U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
#endif
},

{
    .FormatName = "(VIN0-3, 4-7, 8-10, 11-13) 14x1920x1080p30 (HDMI 14-in-1)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_2][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_2]#\
         $[VIN_4][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#\
         $[VIN_5][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_4]#\
         $[VIN_6][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_5]#\
         $[VIN_8][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_6]#\
         $[VIN_9][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_7]#\
         $[VIN_10][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_8]#\
         $[VIN_11][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_9]#\
         $[VIN_12][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_0][FOV_10]#\
         $[VIN_13][B_MAXIM96712_2][B_MAXIM9295_0][S_IMX390_0][FOV_11]#\
         $[VIN_3][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_0][FOV_12]#\
         $[VIN_7][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_0][FOV_13]#!"
    },
    /* vin info */
    .VinNum = 12U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [3] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [4] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [5] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [6] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [7] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [8] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [9] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [10] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [11] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [12] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
        [13] = {
            .pDriver = &AmbaSensor_MX01_IMX390_VCObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj,
#endif
            .TimeoutCfg = {5000U, 0},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [1] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [2] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [3] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [4] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [5] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [6] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [7] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [8] = { /* sensor on VIN_8 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [9] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [10] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [11] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [12] = { /* sensor on VIN_10 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
        [13] = { /* sensor on VIN_9 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX01_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = (0x02UL << 4UL),
                .IQTable = IMX390_IQ,
            },
        },
    },
    /* fov info */
    .FovNum = 14U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [4] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [5] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [6] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [7] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [8] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [9] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [10] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [11] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [12] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [13] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { /*.RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U ,*/ .LinearCE = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_OFF_DRAMOUT, .WarpLumaWaitLine = 64U, .WarpChromaWaitLine = 64U, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 14,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 480, 360}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={480, 0, 480, 360}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {960, 0, 480, 360}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1440, 0, 480, 360}
                    },
                    [4] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 360, 480, 360}
                    },
                    [5] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={480, 360, 480, 360}
                    },
                    [6] = {
                        .FovId = 6U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 360, 480, 360}
                    },
                    [7] = {
                        .FovId = 7U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1440, 360, 480, 360}
                    },
                    [8] = {
                        .FovId = 8U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={0, 720, 480, 360}
                    },
                    [9] = {
                        .FovId = 9U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={480, 720, 480, 360}
                    },
                    [10] = {
                        .FovId = 10U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={960, 720, 480, 360}
                    },
                    [11] = {
                        .FovId = 11U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1440, 720, 480, 360}
                    },
                    [12] = {
                        .FovId = 12U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={1200, 540, 480, 360}
                    },
                    [13] = {
                        .FovId = 13U, .SrcWin = {0, 0, 1920, 1080}, .DstWin ={240, 540, 480, 360}
                    },
                },
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 14U, .RecBits = 0x3FFFU,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
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
                .UseAuxStg = 0,
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [4] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 4, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [5] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 0,
                .RecId = 5, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 5U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [6] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 6, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 6U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [7] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 7, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 7U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [8] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 8, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 8U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [9] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 9, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 9U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [10] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 10, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 10U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [11] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 11, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 11U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [12] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 12, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 12U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [13] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .UseAuxStg = 1,
                .RecId = 13, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 13U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},
};

#endif /* RES_X_X_H */
