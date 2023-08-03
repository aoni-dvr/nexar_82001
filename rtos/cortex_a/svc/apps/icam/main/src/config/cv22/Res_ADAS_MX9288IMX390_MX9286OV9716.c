/**
 *  @file Res_ADAS_MX9288IMX390_MX9286OV9716.c
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
#include "AmbaSensor_MAX9295_9288_IMX390.h"
#include "AmbaSensor_MAX96705_9286_OV9716.h"
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
#include "SvcCvFlow_RefOD.h"
#include "SvcCvAppDef.h"
#endif

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0)MX9288+MX9295+IMX390 (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9288_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver    = &AmbaSensor_MX04_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin     = {.OffsetX = 8U, .OffsetY = 8U, .Width = 1920U, .Height = 1080U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate  = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode  = MX04_IMX390_1920_1080_30P_HDR,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX9295_9288_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin      = {0, 0, 1920, 1080},
            .MainWin     = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
#if 0
        [0] = {
            .VoutID = 0, .VideoRotateFlip = 0,
            .pDriver = &AmbaFPD_MAXIM_TFT128Obj,
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            },
            .DevMode = AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
#endif
        [0] = {
            .VoutID  = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0,
            .StrmCfg = {
                .Win     = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8) HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits   = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits  = SVC_REC_DST_FILE,
                .RecId     = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN1)MX9286+MX96705+OV9716 (HDMI)",
    .VinTree    = {
        "$[VIN_1][B_MAXIM9286_0][B_MAXIM96705_0][S_OV9716_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver    = &AmbaSensor_MX02_OV9716Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin     = {.OffsetX = 56U, .OffsetY = 8U, .Width = 1280U, .Height = 960U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate  = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin      = {0U, 0U, 1280U, 960U},
            .MainWin     = {1280U, 960U},
            .PyramidBits = 0, .HierWin = {0U, 0U},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID  = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win     = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {  0U,   0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode   = (UINT8) HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
    },
    /* record stream info */
    .RecNum  = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits   = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits  = SVC_REC_DST_FILE,
                .RecId     = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280U, 960U}, .MaxWin = {1280U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {0U, 0U, 1280U, 960U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN1)MX9286+4x(MX96705+OV9716) (HDMI)",
    .VinTree    = {
        "$[VIN_1][B_MAXIM9286_0][B_MAXIM96705_0][S_OV9716_0][FOV_0]#\
         $[VIN_1][B_MAXIM9286_1][B_MAXIM96705_0][S_OV9716_1][FOV_1]#\
         $[VIN_1][B_MAXIM9286_2][B_MAXIM96705_0][S_OV9716_2][FOV_2]#\
         $[VIN_1][B_MAXIM9286_3][B_MAXIM96705_0][S_OV9716_3][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver    = &AmbaSensor_MX02_OV9716Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin     = {.OffsetX = 56U, .OffsetY = 32U, .Width = 1280U, .Height = 3840U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option  = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 4,
                       .CaptureWindow = {.OffsetX = 56, .OffsetY = 32, .Width = 1280, .Height = 3840}
                }
            },
            .TimeoutCfg = {5000U, 200U},
            .FrameRate  = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
            [2] = { /* sensor idx 2 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
            [3] = { /* sensor idx 3 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
        },
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin      = {0U, 0U, 1280U, 960U},
            .MainWin     = {1280U, 960U},
            .PyramidBits = 0, .HierWin = {0U, 0U},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [1] = {
            .RawWin      = {0U, 1U, 1280U, 960U},
            .MainWin     = {1280U, 960U},
            .PyramidBits = 0, .HierWin = {0U, 0U},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [2] = {
            .RawWin      = {0U, 2U, 1280U, 960U},
            .MainWin     = {1280U, 960U},
            .PyramidBits = 0, .HierWin = {0U, 0U},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [3] = {
            .RawWin      = {0U, 3U, 1280U, 960U},
            .MainWin     = {1280U, 960U},
            .PyramidBits = 0, .HierWin = {0U, 0U},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID  = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win     = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 4U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {  0U,   0U, 960U, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {  0U, 540U, 960U, 540U}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {960U,   0U, 960U, 540U}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {960U, 540U, 960U, 540U}
                    },
                }
            },
            .DevMode   = (UINT8) HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
    },
    /* record stream info */
    .RecNum  = 4U, .RecBits = 0x0FU,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits   = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits  = SVC_REC_DST_FILE,
                .RecId     = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280U, 960U}, .MaxWin = {1280U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {0U, 0U, 1280U, 960U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits   = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits  = SVC_REC_DST_FILE,
                .RecId     = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280U, 960U}, .MaxWin = {1280U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {0U, 0U, 1280U, 960U}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits   = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits  = SVC_REC_DST_FILE,
                .RecId     = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280U, 960U}, .MaxWin = {1280U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {0U, 0U, 1280U, 960U}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits   = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits  = SVC_REC_DST_FILE,
                .RecId     = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280U, 960U}, .MaxWin = {1280U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {0U, 0U, 1280U, 960U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0)MX9288+MX9295+IMX390 (X) + (VIN1)MX9286+4x(MX96705+OV9716) (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9288_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9286_0][B_MAXIM96705_0][S_OV9716_0][FOV_1]#\
         $[VIN_1][B_MAXIM9286_1][B_MAXIM96705_0][S_OV9716_1][FOV_2]#\
         $[VIN_1][B_MAXIM9286_2][B_MAXIM96705_0][S_OV9716_2][FOV_3]#\
         $[VIN_1][B_MAXIM9286_3][B_MAXIM96705_0][S_OV9716_3][FOV_4]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX04_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 1920U, .Height = 1080U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .pDriver    = &AmbaSensor_MX02_OV9716Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin     = {.OffsetX = 56U, .OffsetY = 512U, .Width = 1280U, .Height = 2880U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option  = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 4,
                       .CaptureWindow = {.OffsetX = 56, .OffsetY = 512, .Width = 1280, .Height = 2880}
                }
            },
            .TimeoutCfg = {5000U, 200U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode  = MX04_IMX390_1920_1080_30P_HDR,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX9295_9288_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
            [2] = { /* sensor idx 2 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
            [3] = { /* sensor idx 3 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
        },
    },
    /* fov info */
    .FovNum = 5U,
    .FovCfg = {
        [0] = {
            .RawWin      = {0, 0, 1920, 1080},
            .MainWin     = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin      = {0, 0, 1280, 720},
            .MainWin     = {1280, 720},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [2] = {
            .RawWin      = {0, 1, 1280, 720},
            .MainWin     = {1280, 720},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [3] = {
            .RawWin      = {0, 2, 1280, 720},
            .MainWin     = {1280, 720},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [4] = {
            .RawWin      = {0, 3, 1280, 720},
            .MainWin     = {1280, 720},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
#if 0
        [0] = {
            .VoutID = 0, .VideoRotateFlip = 0,
            .pDriver = &AmbaFPD_MAXIM_TFT128Obj,
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            },
            .DevMode = AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
#endif
        [0] = {
            .VoutID  = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0,
            .StrmCfg = {
                .Win     = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 4U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {  0,   0, 960, 540}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {  0, 540, 960, 540}
                    },
                    [2] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {960,   0, 960, 540}
                    },
                    [3] = {
                        .FovId = 4U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {960, 540, 960, 540}
                    },
                }
            },
            .DevMode = (UINT8) HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x1U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits   = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits  = SVC_REC_DST_FILE,
                .RecId     = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win     = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
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
    .FormatName = "(VIN0)MX9288+MX9295+IMX390 (HDMI) + (VIN1)MX9286+4x(MX96705+OV9716) (X)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9288_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9286_0][B_MAXIM96705_0][S_OV9716_0][FOV_1]#\
         $[VIN_1][B_MAXIM9286_1][B_MAXIM96705_0][S_OV9716_1][FOV_2]#\
         $[VIN_1][B_MAXIM9286_2][B_MAXIM96705_0][S_OV9716_2][FOV_3]#\
         $[VIN_1][B_MAXIM9286_3][B_MAXIM96705_0][S_OV9716_3][FOV_4]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX04_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 1920U, .Height = 1080U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .pDriver    = &AmbaSensor_MX02_OV9716Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin     = {.OffsetX = 56U, .OffsetY = 512U, .Width = 1280U, .Height = 2880U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option  = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 4,
                       .CaptureWindow = {.OffsetX = 56, .OffsetY = 512, .Width = 1280, .Height = 2880}
                }
            },
            .TimeoutCfg = {5000U, 200U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode  = MX04_IMX390_1920_1080_30P_HDR,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX9295_9288_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            },
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
            [2] = { /* sensor idx 2 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
            [3] = { /* sensor idx 3 */
                .SensorMode  = MX02_OV9716_1392_976_30P,
                .SensorGroup = 0,
                .IQTable     = IQ_SSR_MAX96705_9286_OV9716,
            },
        },
    },
    /* fov info */
    .FovNum = 5U,
    .FovCfg = {
        [0] = {
            .RawWin      = {0, 0, 1920, 1080},
            .MainWin     = {1920, 1080},
            .PyramidBits = 0x2U, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin      = {0, 0, 1280, 720},
            .MainWin     = {1280, 720},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [2] = {
            .RawWin      = {0, 1, 1280, 720},
            .MainWin     = {1280, 720},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [3] = {
            .RawWin      = {0, 2, 1280, 720},
            .MainWin     = {1280, 720},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [4] = {
            .RawWin      = {0, 3, 1280, 720},
            .MainWin     = {1280, 720},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
#if 0
        [0] = {
            .VoutID = 0, .VideoRotateFlip = 0,
            .pDriver = &AmbaFPD_MAXIM_TFT128Obj,
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            },
            .DevMode = AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
#endif
        [0] = {
            .VoutID  = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0,
            .StrmCfg = {
                .Win     = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = (UINT8) HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x1U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits   = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits  = SVC_REC_DST_FILE,
                .RecId     = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win     = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_REF_OD_FDAG,
            .CvModeID = REFOD_FDAG_OPEN_1920x1080_L1_1,
            .FovInputNum = 1U,
            .FovInput = {
                [0] = 0U,   /* input is FOV_0 */
            },
        }
    },
    .StillCfg.EnableStill = 1,
},
};

#endif /* RES_X_X_H */
