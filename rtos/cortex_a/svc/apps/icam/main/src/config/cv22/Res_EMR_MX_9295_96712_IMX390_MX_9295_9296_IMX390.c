/**
 *  @file Res_EMR_MX_9295_96712_IMX390_MX_9295_9296_IMX390.c
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
#include "AmbaSensor_MAX9295_9296_IMX390.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaVOUT_Def.h"
#include "AmbaSPI_Def.h"
#include "AmbaFPD_T30P61.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_MAXIM_TFT1280768.h"
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#include "AmbaFPD_MAXIM_Dual_TFT1280768.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaFPD_HDMI.h"

#ifdef CONFIG_MAX9295_96712_IMX390_20BIT
#define IMX390_SENSOR_MODE MX01_IMX390_1920_1080_60P_HDR1
#else
#define IMX390_SENSOR_MODE MX01_IMX390_1920_1080_A60P_HDR
#endif

#define T3CH_SIDE_ACT_W 1280
#define T3CH_SIDE_ACT_H 720

#define T3CH_SIDE_MAIN_W 1280
#define T3CH_SIDE_MAIN_H 720

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0)MX96712+3x(MX9295+IMX390) (Cropping) (3VOUT) + (VIN1)MX9296+MX9295+IMX390 1080P30",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 2U,
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
            .TimeoutCfg = {5000U, 100U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 2 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 768)/2, 1280, 768}, /* dzoom crop */
            .MainWin = {1280, 768},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 768)/2, 1280, 768}, /* dzoom crop */
            .MainWin = {1280, 768},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .HierWin = {0, 0},
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
            .DevMode = AMBA_FPD_MAXIM_A60HZ,
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
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            },
            .DevMode = MAXIM_MULTIVIEW_SPLITTER_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0)MX96712+3x(MX9295+IMX390) (Cropping) (HDMI) + (VIN1)MX9296+MX9295+IMX390 1080P30",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 2U,
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
            .TimeoutCfg = {5000U, 100U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 2 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 300, 1920, 480}, /* dzoom crop */
            .MainWin = {1920, 480},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 768)/2, 1280, 768}, /* dzoom crop */
            .MainWin = {1280, 768},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 768)/2, 1280, 768}, /* dzoom crop */
            .MainWin = {1280, 768},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .MainWin = {1920, 1080},
            .HierWin = {0, 0},
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
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {960, 540, 960, 540}
                    },
                }
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0)MX96712+3x(MX9295+IMX390) (Cropping) (HDMI) + (VIN1)MX9296+MX9295+IMX390 1280x768 (LCD)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 2U,
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
            .TimeoutCfg = {5000U, 100U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 2 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0, 270, 1920, 540}, /* dzoom crop */
            .MainWin = {1920, 540},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 768)/2, 1280, 768}, /* dzoom crop */
            .MainWin = {1280, 768},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0, .VideoRotateFlip = 0,
            .pDriver = &AmbaFPD_MAXIM_TFT128Obj,
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            },
            .DevMode = AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = 0,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 540}, .DstWin = {0, 0, 1920, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {960, 540, 960, 540}
                    },
                }
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
    .FormatName = "(VIN0)MX96712+MX9295+IMX390 (Cropping) (HDMI) + (VIN1)MX9296+MX9295+IMX390 1280x768 (LCD)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
        [1] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - 1280)/2, (1080 - 768)/2, 1280, 768}, /* dzoom crop */
            .MainWin = {1280, 768},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0, .VideoRotateFlip = 0,
            .pDriver = &AmbaFPD_MAXIM_TFT128Obj,
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            },
            .DevMode = AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = 0,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0)3x(MX96712+MX9295+IMX390 1280x720)+(VIN1)1x(MX9296+MX9295+IMX390 1280x720) (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 2U,
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
            .TimeoutCfg = {5000U, 100U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 2 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0, .VideoRotateFlip = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, 960, 240}
                    },
                },
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = 0,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 4U,
                .ChanCfg = {
                    [0] = {
                        //.FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 504}
                        .FovId = 0U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, 960, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {960, 0, 960, 540}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {960, 540, 960, 540}
                    },
                }
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
    .FormatName = "(VIN0)2x(MX96712+MX9295+IMX390 1280x720)+(VIN1)1x(MX9296+MX9295+IMX390 1280x720) (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_0][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 2U,
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
            .TimeoutCfg = {5000U, 100U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 2 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0, .VideoRotateFlip = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = 0,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 0, 960, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H}, .DstWin = {960, 540, 960, 540}
                    },
                }
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
    .FormatName = "(VIN0)1x(MX96712+MX9295+IMX390 1280x720)+(VIN1)1x(MX9296+MX9295+IMX390 1280x720) (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 18U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = IMX390_SENSOR_MODE,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
        [1] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0, .VideoRotateFlip = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = 0,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {960, 540, 960, 540}
                    },
                }
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
    .FormatName = "(VIN1)1x(MX9296+MX9295+IMX390 1280x720) (HDMI)",
    .VinTree = {
        "$[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {(1920 - T3CH_SIDE_ACT_W)/2, (1080 - T3CH_SIDE_ACT_H)/2, T3CH_SIDE_ACT_W, T3CH_SIDE_ACT_H}, /* dzoom crop */
            .MainWin = {T3CH_SIDE_MAIN_W, T3CH_SIDE_MAIN_H},
            .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0, .VideoRotateFlip = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = 0,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
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
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
};

#endif /* RES_X_X_H */
