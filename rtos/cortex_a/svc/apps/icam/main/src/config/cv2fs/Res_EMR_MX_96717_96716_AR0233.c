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
#include "AmbaSensor_MAX96717_96716_AR0233.h"
#include "AmbaSensor_MAX96717_96722_AR0233.h"
//#include "AmbaSensor_MAX9295_96712_AR0233.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaVOUT_Def.h"
#include "AmbaFPD_LT9611UXC.h"

#include "AmbaVOUT_Def.h"
#include "AmbaFPD_HDMI.h"
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
    .FormatName = "(VIN0) MX96716+MX96717+AR0233 1920x576P60 (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96716_0][B_MAXIM96717_0][S_AR0233_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX04_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 576U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
       },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX04_AR0233_1920_576_60P_M6,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96717_96716_AR0233 | (IMG_SENSOR_HDR_MODE_0 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 576},
            .MainWin = {1920, 576},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 576}, .DstWin = {0, 0, 1280, 720}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_720P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 576}, .DstWin = {0, 0, 1280, 720}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_720P_A60HZ,
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
                .RecId = 0U, .M = 1U, .N = 60U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 576U}, .MaxWin = {1920U, 576U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 576U}, .DstWin = {0U, 0U, 1920U, 576U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN1) MX96716+MX96717+AR0233 1920x576P60 (HDMI)",
    .VinTree = {
        "$[VIN_1][B_MAXIM96716_0][B_MAXIM96717_0][S_AR0233_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX04_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 576U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
       },
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX04_AR0233_1920_576_60P_M6,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96717_96716_AR0233 | (IMG_SENSOR_HDR_MODE_0 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 576},
            .MainWin = {1920, 576},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 576}, .DstWin = {0, 0, 1280, 720}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_720P_A60HZ,
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
                .RecId = 0U, .M = 1U, .N = 60U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 576U}, .MaxWin = {1920U, 576U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 576U}, .DstWin = {0U, 0U, 1920U, 576U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN5) MX96716+MX96717+AR0233 1920x576P60 (HDMI)",
    .VinTree = {
        "$[VIN_5][B_MAXIM96716_0][B_MAXIM96717_0][S_AR0233_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [5] = {
            .pDriver = &AmbaSensor_MX04_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 576U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
       },
    },
    .SensorCfg = {
        [5] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX04_AR0233_1920_576_60P_M6,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96717_96716_AR0233 | (IMG_SENSOR_HDR_MODE_0 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 576},
            .MainWin = {1920, 576},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 576}, .DstWin = {0, 0, 1280, 720}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_720P_A60HZ,
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
                .RecId = 0U, .M = 1U, .N = 60U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 576U}, .MaxWin = {1920U, 576U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 576U}, .DstWin = {0U, 0U, 1920U, 576U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0/1/5) MX96716+MX96717+AR0233 1920x576P60 (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM96716_0][B_MAXIM96717_0][S_AR0233_0][FOV_0]#\
         $[VIN_1][B_MAXIM96716_0][B_MAXIM96717_0][S_AR0233_0][FOV_1]#\
         $[VIN_5][B_MAXIM96716_0][B_MAXIM96717_0][S_AR0233_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 3U,
    .VinCfg = {
         [0] = {
             .pDriver = &AmbaSensor_MX04_AR0233Obj,
             .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
             .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 576U},
             .TimeoutCfg = {5000U, 100U},
             .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX04_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 576U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [5] = {
            .pDriver = &AmbaSensor_MX04_AR0233Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 576U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX04_AR0233_1920_576_60P_M6,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96717_96716_AR0233 | (IMG_SENSOR_HDR_MODE_0 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX04_AR0233_1920_576_60P_M6,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96717_96716_AR0233 | (IMG_SENSOR_HDR_MODE_0 << 16),
            }, /* sensor idx 0 */
        },
        [5] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX04_AR0233_1920_576_60P_M6,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96717_96716_AR0233 | (IMG_SENSOR_HDR_MODE_0 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 576},
            .MainWin = {1920, 576},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 576},
            .MainWin = {1920, 576},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U }
        },
        [2] = {
            .RawWin = {0, 0, 1920, 576},
            .MainWin = {1920, 576},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920, 576}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 576}, .DstWin = {0, 0, 1920, 576}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1800, 1080}, .MaxWin = {1920, 1080}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 576}, .DstWin = {0, 0, 1800, 540}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920, 576}, .DstWin = {0, 540, 1800, 540}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#if 0
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 60U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 576U}, .MaxWin = {1920U, 576U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 576U}, .DstWin = {0U, 0U, 1920U, 576U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 60U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 576U}, .MaxWin = {1920U, 576U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 576U}, .DstWin = {0U, 0U, 1920U, 576U}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2U, .M = 1U, .N = 60U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 576U}, .MaxWin = {1920U, 576U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, 1920U, 576U}, .DstWin = {0U, 0U, 1920U, 576U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
#endif
},
};

#endif /* RES_X_X_H */
