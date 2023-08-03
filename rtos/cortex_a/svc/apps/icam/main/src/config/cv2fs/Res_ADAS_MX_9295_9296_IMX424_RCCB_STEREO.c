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
#include "AmbaFPD.h"
#include "AmbaFPD_LT9611UXC.h"
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
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (CV FDAG) AmbaOD + AmbaSeg",
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
            .PyramidBits = 0x08, .HierWin = {0x0, 0x0},
            .PipeCfg = {.LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 20, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_AmbaSegObj,
            .CvModeID = AMBASEG_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 80, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB (CV FDAG) AmbaOD(2 Scale) + AmbaSeg",
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
            .PyramidBits = 0x08, .HierWin = {0x0, 0x0},
            .PipeCfg = {.LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC_2SCALE,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 2U,
                        .Roi = {
                            [0] = {0, 1280, 640, 0, 0},
                            [1] = {3, 40, 20, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_AmbaSegObj,
            .CvModeID = AMBASEG_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 80, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
{
    .FormatName = "(VIN1) MX9296+MX9295+IMX424_RCCB (3840x1920p30)",
    .VinTree = {
        "$[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_0 */
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
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB  (VIN1) MX9296+MX9295+IMX424_RCCB DisableMasterSync 0 ",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
            $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
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
        [1] = {
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
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
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
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
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
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB  (VIN1) MX9296+MX9295+IMX424_RCCB For Calibration",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
            .DisableMasterSync = 0U,
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_30P_HDR,
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
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
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
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB  (VIN1) MX9296+MX9295+IMX424_RCCB Apply Warp",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
            .DisableMasterSync = 1U,
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
            .DisableMasterSync = 0U,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX424_3840_1920_30P_HDR,
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
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
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
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB  (VIN1) MX9296+MX9295+IMX424_RCCB",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
        [1] = {
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
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
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
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
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
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB  (VIN1) MX9296+MX9295+IMX424_RCCB (FileIn SPU Fusion SC5)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
        [1] = {
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
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
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
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U }
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
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
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x1U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
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
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 2U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 2U,
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
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB  (VIN1) MX9296+MX9295+IMX424_RCCB (SPU Fusion SC5)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
        [1] = {
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
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
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
            .PyramidBits = 0x1D, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x1U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 0U,
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 1U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB  (VIN1) MX9296+MX9295+IMX424_RCCB (SPU Fusion SC5) + AmbaOD 2 scale",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
        [1] = {
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
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
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
            .PyramidBits = 0x1D, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x1U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 20, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_StereoObj,
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
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 1U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB  (VIN1) MX9296+MX9295+IMX424_RCCB AmbaOD 2 Scale",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX424StereoObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .DisableMasterSync = 0U,
        },
        [1] = {
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
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX424 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
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
            .PyramidBits = 0x1D, .HierWin = {0x0, 0x0},
            .PipeCfg = {.LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = {.LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x1U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
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
            .CvModeID = AMBAOD_FDAG_FC_2SCALE,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 2U,
                        .Roi = {
                            [0] = {0, 1280, 640, 0, 0},
                            [1] = {3, 40, 20, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX424_RCCB  (VIN1) MX9296+MX9295+IMX424_RCCB (SPU Fusion SC5, Stixel)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX424_0][FOV_1]#!"
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
        [1] = {
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
        [1] = { /* sensor on VIN_0 */
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
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
        [1] = {
            .RawWin = {0U, 0U, 3840U, 1920U},
            .ActWin = {0U},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x15, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
            .CalUpdBits = 0x2000, /* b'0010 0000 0000 0000 enable calib ID 13 */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x1U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560U, 1280U}, .MaxWin = {2560U, 1280U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840U, 1920U}, .DstWin = {0, 0, 2560U, 1280U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
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
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 1U,
            },
            .CvFlowType = SVC_CV_FLOW_STEREO,
            .DownstrmChanNum = 1U,
            .DownstrmChan = {1U},
        },
        [1] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_StixelObj,
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
    },
#endif
},
};

#endif /* RES_X_X_H */
