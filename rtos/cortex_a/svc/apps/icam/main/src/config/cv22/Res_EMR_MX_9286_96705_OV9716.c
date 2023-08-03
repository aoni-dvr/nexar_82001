/**
 *  @file Res_EMR_MX_9286_96705_OV9716.c
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
#include "AmbaSensor_MAX96705_9286_OV9716.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD_T30P61.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX9286+MX96705+OV9716 1ch",
    .VinTree = {
        "$[VIN_0][B_MAXIM9286_0][B_MAXIM96705_0][S_OV9716_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver    = &AmbaSensor_MX02_OV9716Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin     = {.OffsetX = 56U, .OffsetY = 8U, .Width = 1280U, .Height = 960U},
            .FrameRate  = {.TimeScale = 30000, .NumUnitsInTick = 1001},
            .TimeoutCfg = {5000U, 100U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
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
        }
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID  = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win     = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {0U, 0U, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .VoutID  = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win     = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    }
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1080P,
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
                .RecId     = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win     = {1280U, 960U}, .MaxWin = {1280U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {0U, 0U, 1280U, 960U}
                    }
                }
            }
        }
    },
    .StillCfg.EnableStill = 1,
},
#if 0
{
    .FormatName = "(VIN0) MX9286+MX96705+OV9716 3ch",
    .VinTree    = {
        "$[VIN_0][B_MAXIM9286_0][B_MAXIM96705_0][S_OV9716_0][FOV_0]#\
         $[VIN_0][B_MAXIM9286_1][B_MAXIM96705_0][S_OV9716_1][FOV_1]#\
         $[VIN_0][B_MAXIM9286_2][B_MAXIM96705_0][S_OV9716_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver    = &AmbaSensor_MX02_OV9716Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin     = {.OffsetX = 56U, .OffsetY = 24U, .Width = 1280U, .Height = 2880U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option  = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 56, .OffsetY = 24, .Width = 1280, .Height = 2880}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
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
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin      = {0U, 0U, 1280U, 960U},
            .MainWin     = {1280U, 960U},
            .PyramidBits = 0, .HierWin = {0U, 0U},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [1] = {
            .RawWin      = {0, 1U, 1280U, 960U},
            .MainWin     = {1280U, 960U},
            .PyramidBits = 0, .HierWin = {0U, 0U},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [2] = {
            .RawWin      = {0, 2U, 1280U, 960U},
            .MainWin     = {1280U, 960U},
            .PyramidBits = 0, .HierWin = {0U, 0U},
            .PipeCfg     = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID  = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win     = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {0U, 0U, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID  = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win     = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {  0U,   0U,  960U, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {960U,   0U,  960U, 540U}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {  0U, 540U, 1920U, 540U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum  = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits   = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
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
    },
    .StillCfg.EnableStill = 1,
},
#endif
{
    .FormatName = "(VIN0) MX9286+MX96705+OV9716 4ch",
    .VinTree    = {
        "$[VIN_0][B_MAXIM9286_0][B_MAXIM96705_0][S_OV9716_0][FOV_0]#\
         $[VIN_0][B_MAXIM9286_1][B_MAXIM96705_0][S_OV9716_1][FOV_1]#\
         $[VIN_0][B_MAXIM9286_2][B_MAXIM96705_0][S_OV9716_2][FOV_2]#\
         $[VIN_0][B_MAXIM9286_3][B_MAXIM96705_0][S_OV9716_3][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
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
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
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
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID  = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win     = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280U, 960U}, .DstWin = {0U, 0U, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
        [1] = {
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
            .DevMode = (UINT8)HDMI_VIC_1080P,
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
};

#endif /* RES_X_X_H */
