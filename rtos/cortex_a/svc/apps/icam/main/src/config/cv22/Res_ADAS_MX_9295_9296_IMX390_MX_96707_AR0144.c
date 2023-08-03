/**
 *  @file Res_ADAS_MX_9295_9296_IMX390_MX_96707_AR0144.c
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
#include "AmbaSensor_MAX9295_9296_IMX390.h"
#include "AmbaSensor_MAX96707_9286_AR0144.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD_T30P61.h"
#include "AmbaFPD_MAXIM_TFT1280768.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX9296+2x(9295+IMX390) (HDMI)\n\
                   (VIN1) MX9286+96707+AR0144)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_1][B_MAXIM9286_0][B_MAXIM96707_0][S_AR0144_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 42U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 8U, .OffsetY = 42U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX03_AR0144Obj,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1280U, .Height = 720U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR2,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
                }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR2,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX03_AR0144_1280_720_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96707_9286_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [2] = {
            .RawWin = {0U, 0U, 1280U, 720U},
            .ActWin = {0U, 0U, 0U, 0U},
            .MainWin = {1280U, 720U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX9296+2x(9295+IMX390) (MX-TFT1280768)\n\
                   (VIN1) MX9286+96707+AR0144)   (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_1][B_MAXIM9286_0][B_MAXIM96707_0][S_AR0144_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 42U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 8U, .OffsetY = 42U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX03_AR0144Obj,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1280U, .Height = 720U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR2,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
                }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR2,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX03_AR0144_1280_720_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96707_9286_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [2] = {
            .RawWin = {0U, 0U, 1280U, 720U},
            .ActWin = {0U, 0U, 0U, 0U},
            .MainWin = {1280U, 720U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_MAXIM_TFT128Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
};

#endif /* RES_X_X_H */