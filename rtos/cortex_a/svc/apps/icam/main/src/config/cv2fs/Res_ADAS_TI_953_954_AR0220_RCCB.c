/**
 *  @file Res_ADAS_TI_953_954_AR0220_RCCB.c
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
#include "AmbaVIN.h"
#include "AmbaSensor.h"
#include "AmbaSensor_TI953_954_AR0220_RCCB.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD.h"
#include "AmbaFPD_LT9611UXC.h"
#include "AmbaDSP_ImageFilter.h"
#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_RefSeg.h"
#include "SvcCvAppDef.h"
#endif

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) TI954+TI953+AR0220_RCCB 940P44 HDR",
    .VinTree = {
        "$[VIN_0][B_TI954_0][B_TI953_0][S_AR0220_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI01_AR0220Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1792U, .Height = 944U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = TI01_AR0220_1828_948_44P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_954_AR0220_RCCB | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1792U, 944U},
            .ActWin = {0},
            .MainWin = {1792U, 944U},
            .PyramidBits = 0x3F, .HierWin = {0, 0}, .PyramidDeciRate = 2,
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 1012U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1920U, 1012U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
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
                .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1792U, 944U}, .MaxWin = {1792U, 944U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1792U, 944U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{
    .FormatName = "(VIN0) TI954+TI953+AR0220_RCCB 940P30 HDR",
    .VinTree = {
        "$[VIN_0][B_TI954_0][B_TI953_0][S_AR0220_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI01_AR0220Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1792U, .Height = 944U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = TI01_AR0220_1828_948_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_954_AR0220_RCCB | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1792U, 944U},
            .ActWin = {0},
            .MainWin = {1792U, 944U},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 1012U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1920U, 1012U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_60HZ,
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
                .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1792U, 944U}, .MaxWin = {1792U, 944U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1792U, 944U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{
    .FormatName = "(VIN0) TI954+TI953+AR0220_RCCB 940P30 HDR OpenOD Linux",
    .VinTree = {
        "$[VIN_0][B_TI954_0][B_TI953_0][S_AR0220_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI01_AR0220Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1792U, .Height = 944U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = TI01_AR0220_1828_948_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_954_AR0220_RCCB | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1792U, 944U},
            .ActWin = {0},
            .MainWin = {1792U, 944U},
            .PyramidBits = 0x3F, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 1012U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1920U, 1012U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_60HZ,
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
                .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1792U, 944U}, .MaxWin = {1792U, 944U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1792U, 944U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_LINUX_SSD,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1792U, .FrameHeight = 944U, .NumRoi = 0U
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    }
#endif
},
{
    .FormatName = "(VIN0) TI954+TI953+AR0220_RCCB 940P30 HDR RefLinux",
    .VinTree = {
        "$[VIN_0][B_TI954_0][B_TI953_0][S_AR0220_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI01_AR0220Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1792U, .Height = 944U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = TI01_AR0220_1828_948_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_954_AR0220_RCCB | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1792U, 944U},
            .ActWin = {0},
            .MainWin = {1792U, 944U},
            .PyramidBits = 0x3F, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U, .MainY12Out = 1U  },
            .VirtChan = {0x8U, 0x0U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 1012U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1920U, 1012U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_60HZ,
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
                .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1792U, 944U}, .MaxWin = {1792U, 944U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1792U, 944U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
#if 0
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefLinuxObj,
            .CvModeID = REFLNX_FDAG_LINUX_1,
            .InputCfg = {
                .InputNum = 3U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PROC_RAW, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 0U, .FrameHeight = 0U, .NumRoi = 0U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                    [1] = {
                        .DataSrc = SVC_CV_DATA_SRC_MAIN_Y12, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 0U, .FrameHeight = 0U, .NumRoi = 0U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                    [2] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 0U, .FrameHeight = 0U, .NumRoi = 0U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },

    }
#endif
#endif
},
{
    .FormatName = "(VIN0) TI954+TI953+AR0220_RCCB 940P44 HDR (TimeDiv)",
    .VinTree = {
        "$[VIN_0][B_TI954_0][B_TI953_0][S_AR0220_0][FOV_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI01_AR0220Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1792U, .Height = 944U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_PROG,
                       .ConCatNum = 1U,
                       .IntcNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1792U, .Height = 944U},
                       .TDNum = 2U,
                }
            },
            .SubChanTDFrmNum = {
                    [0] = { 1U, 1U, 0U, 0U, 0U, 0U, 0U, 0U }
            },
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = TI01_AR0220_1828_948_44P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_954_AR0220_RCCB | (IMG_SENSOR_HDR_MODE_2 << 16U),
                .TimeDivisionIQTable = {
                    IQ_SSR_TI953_954_AR0220_RCCB | (IMG_SENSOR_HDR_MODE_2 << 16U),
                    IQ_SSR_TI953_954_AR0220_RCCB | (IMG_SENSOR_HDR_MODE_2 << 16U),
                    0, 0, 0, 0, 0, 0
                }
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1792U, 944U},
            .ActWin = {0},
            .MainWin = {1792U, 944U},
            .PyramidBits = 0x3F, .HierWin = {0, 0}, .PyramidFlag = SVC_RES_PYRAMID_DOWNSCALE_2X,
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U, .MainY12Out = 1U },
            .VirtChan = {0x2U, 0x0U}
        },
        [1] = {
            .RawWin = {0, 0, 1792U, 944U},
            .ActWin = {0},
            .MainWin = {1792U, 944U},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = 0U },
            .VirtChan = {0xAU, 0x100U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 1012U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1920U, 1012U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
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
                .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1792U, 944U}, .MaxWin = {1792U, 944U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1792U, 944U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1792U, 944U}, .MaxWin = {1792U, 944U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1792U, 944U}, .DstWin = {0, 0, 1792U, 944U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
};

#endif /* RES_X_X_H */
