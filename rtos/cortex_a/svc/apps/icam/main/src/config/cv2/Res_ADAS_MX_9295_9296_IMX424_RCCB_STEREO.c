/**
 *  @file Res_ADAS_MX_9295_9296_IMX424_RCCB.c
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
#include "AmbaSensor_MAX9295_9296_IMX424_RCCB.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaVOUT_Def.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD.h"
#include "AmbaFPD_HDMI.h"
#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_RefSeg.h"
#include "SvcCvFlow_Stereo.h"
#include "SvcCvFlow_Stixel.h"
#include "SvcCvFlow_StereoAC.h"
#include "SvcCvFlow_LinuxStereo.h"


#include "SvcCvAppDef.h"
#endif


static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) Apply warp",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30)",
    .VinTree = {
        "$[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [2] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN4) MX9296+MX9295+IMX424_RCCB (3840x1920p30)",
    .VinTree = {
        "$[VIN_4][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [4] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [4] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
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
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 960U, 480U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {960U, 0U, 960U, 480U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) For Calibration",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 960U, 480U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {960U, 0U, 960U, 480U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) Apply Warp",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 960U, 480U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {960U, 0U, 960U, 480U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (FileIn SPUFEX 1080P UT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 960U, 480U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {960U, 0U, 960U, 480U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StereoObj,
            .CvModeID = STEREO_SCA_SP_FE_UT_1080P,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (FileIn SPUFEX 4K UT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 960U, 480U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {960U, 0U, 960U, 480U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StereoObj,
            .CvModeID = STEREO_SCA_SP_FE_UT_4K,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (FileIn Fusion 02 UT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 960U, 480U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {960U, 0U, 960U, 480U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StereoObj,
            .CvModeID = STEREO_SCA_FS_02_UT,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (FileIn Fusion 024 UT)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 960U, 480U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {960U, 0U, 960U, 480U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StereoObj,
            .CvModeID = STEREO_SCA_FS_024_UT,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN4) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (Filein - SPU Fusion)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_4][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [4] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [4] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
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
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StereoObj,
            .CvModeID = STEREO_SCA_SP_FS_3840x1920_L024_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (SPU Fusion)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 15, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 15000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 15, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 15000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_ICAM_CV_LINUX_STEREO_APP)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_LinuxStereoObj,
            .CvModeID = LINUX_STEREO_SCA_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,    /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,    /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .SrcRate = 2,
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0,
            },
            .CvFlowType = SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL,
            .PrivFlag = 0U, /* Linux CH0 */
        }
    },
#else
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StereoObj,
            .CvModeID = STEREO_SCA_SP_FS_3840x1920_L024_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (AutoCalib)(Linux/QNX)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x4, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x4, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 960U, 480U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {960U, 0U, 960U, 480U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
                .BitRate = 50000000,
            },
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 1920U, 960U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
                .BitRate = 50000000,
            },
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 1920U, 960U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_ICAM_CV_LINUX_STEREO_APP)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_LinuxStereoObj,
            .CvModeID = LINUX_STEREO_SCA_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                     [0] = {
                         .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,    /* input is FOV_0 */
                         .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                     },
                     [1] = {
                         .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,    /* input is FOV_1 */
                         .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                     },
                 },
                 .SrcRate = 1,
                 .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0,
             },
             .CvFlowType = SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL,
             .PrivFlag = 0U, /* Linux CH0 */
         }
    },
#elif defined(CONFIG_ICAM_CV_STEREO_AUTO_CALIB)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StereoACObj,
            .CvModeID = STEREO_AC_MODE_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 0, 0, 1920, 960},
                        }
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .SrcRate = 2,
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x1,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO_AC,
            //.CvAppFlag = SVC_CV_APP_LOAD_TEST_DATA,
        }
    },
#endif
    .StillCfg = {
    .EnableStill = 1,
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) FHD REC, No Warp",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 480U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 960U, 480U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {960U, 0U, 960U, 480U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
                .BitRate = 50000000,
            },
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 1920U, 960U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
                .BitRate = 50000000,
            },
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 1920U, 960U}
                    },
                }
            }
        },
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (SPU Fusion, AutoCalib)(QNX)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 1920U, 960U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 1920U, 960U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_ICAM_CV_STEREO_AUTO_CALIB)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StereoObj,
            .CvModeID = STEREO_SCA_SP_FS_3840x1920_L024_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x1U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_StereoACObj,
            .CvModeID = STEREO_AC_MODE_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 0, 0, 1920, 960},
                        }
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .SrcRate = 2,
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0x1U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO_AC,
        }
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (SPU Fusion, Stixel)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 15, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 15000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 15, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 15000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 3840U, 1920U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_ICAM_CV_LINUX_STEREO_APP)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_LinuxStereoObj,
            .CvModeID = LINUX_STEREO_SCA_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .SrcRate = 1,
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0,
            },
            .CvFlowType = SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL,
            .PrivFlag = 0U, /* Linux CH0 */
        }
    },
#elif defined(CONFIG_ICAM_CV_STIXEL)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StereoObj,
            .CvModeID = STEREO_SCA_SP_FS_3840x1920_L024_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x1U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
            .DownstrmChanNum = 1U,
            .DownstrmChan = {1U},
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_StixelObj,
            .CvModeID = STIXEL_MODE_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 1U,   /* input is FOV_0 */
                        .FrameWidth = 0, .FrameHeight = 0, .NumRoi = 0U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_STIXEL,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (VIN2) MX9296+MX9295+IMX424_RCCB (3840x1920p30) (Linux AC STIXEL)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 1U,
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_A30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x14, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x14, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
                .BitRate = 50000000,
            },
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 1920U, 960U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
                .BitRate = 50000000,
            },
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 960U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 1920U, 960U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_ICAM_CV_LINUX_STEREO_APP)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_LinuxStereoObj,
            .CvModeID = LINUX_STEREO_SCA_1,
            .InputCfg = {
                .InputNum = 2U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .SrcRate = 2,
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0,
            },
            .CvFlowType = SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL,
            .PrivFlag = 0U, /* Linux CH0 */
        }
    },
#endif
    .StillCfg = {
        .EnableStill = 1,
    },
},
};

#endif /* RES_X_X_H */
