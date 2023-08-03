/**
 *  @file Res_EMR_MX_9295_9296_IMX490.c
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

#include "AmbaVIN.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_IMX490.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaFPD_HDMI.h"

#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#include "AmbaFPD_MAXIM_Define.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvAppDef.h"
#endif

static const SVC_RES_CFG_s g_ResCfg[] = {

{
    .FormatName = "(VIN0) MX9296+MX9295+IMX490 2880x1376P30 SHDR",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX490_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX490Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 2880U, .Height = 1860U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX490_2880_1860_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX490 | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 242U, 2880U, 1376U},
            .ActWin = {0U},
            .MainWin = {2880U, 1376U},
            .PyramidBits = 0x04U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U, .MctfDisable = MCTF_BITS_ON_CMPR, }
        },
    },
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 918}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 2880U, 1376U}, .DstWin = {0U, 0, 1920U, 918}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
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
                .Win = {2880U, 1376U}, .MaxWin = {2880U, 1376U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2880U, 1376U}, .DstWin = {0, 0, 2880U, 1376U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},

{
    .FormatName = "(VIN2) MX9296+MX9295+IMX490 2880x1376P30 SHDR",
    .VinTree = {
        "$[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX490_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX490Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 2880U, .Height = 1860U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX490_2880_1860_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX490 | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 242U, 2880U, 1376U},
            .ActWin = {0U},
            .MainWin = {2880U, 1376U},
            .PyramidBits = 0x04U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U, .MctfDisable = MCTF_BITS_ON_CMPR, }
        },
    },
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 918}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 2880U, 1376U}, .DstWin = {0U, 0, 1920U, 918}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
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
                .Win = {2880U, 1376U}, .MaxWin = {2880U, 1376U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2880U, 1376U}, .DstWin = {0, 0, 2880U, 1376U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},

{
    .FormatName = "(VIN3) MX9296+MX9295+IMX490 2880x1376P30 SHDR",
    .VinTree = {
        "$[VIN_3][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX490_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [3] = {
            .pDriver = &AmbaSensor_MX00_IMX490Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 2880U, .Height = 1860U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [3] = { /* sensor on VIN_3 */
            [0] = {
                .SensorMode = MX00_IMX490_2880_1860_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX490 | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 242U, 2880U, 1376U},
            .ActWin = {0U},
            .MainWin = {2880U, 1376U},
            .PyramidBits = 0x04U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U, .MctfDisable = MCTF_BITS_ON_CMPR, }
        },
    },
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 918}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 2880U, 1376U}, .DstWin = {0U, 0, 1920U, 918}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
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
                .Win = {2880U, 1376U}, .MaxWin = {2880U, 1376U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2880U, 1376U}, .DstWin = {0, 0, 2880U, 1376U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},

{
    .FormatName = "(VIN0/2) MX9296+MX9295+IMX490 2880x1376P30 SHDR CV Running",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX490_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX490_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX490Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 2880U, .Height = 1860U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX490Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 2880U, .Height = 1860U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX490_2880_1860_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX490 | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX490_2880_1860_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX490 | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 242U, 2880U, 1376U},
            .ActWin = {0U},
            .MainWin = {2880U, 1376U},
            .PyramidBits = 0x04U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U, .MctfDisable = MCTF_BITS_ON_CMPR, }
        },
        [1] = {
            .RawWin = {0U, 242U, 2880U, 1376U},
            .ActWin = {0U},
            .MainWin = {2880U, 1376U},
            .PyramidBits = 0x04U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U, .MctfDisable = MCTF_BITS_ON_CMPR, }
        },
    },
    .DispAltNum = 2, .DispAltIdx = 0,
    .DispAlt = {
        [0] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_HDMI_Obj,
                    .StrmCfg = {
                        .Win = {1920U, 918}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 0U, .SrcWin = {0U, 0U, 2880U, 1376U}, .DstWin = {0U, 0, 1920U, 918}
                            },
                        }
                    },
                    .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
                },
            },
        },
        [1] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_HDMI_Obj,
                    .StrmCfg = {
                        .Win = {1920U, 918}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 1U, .SrcWin = {0U, 0U, 2880U, 1376U}, .DstWin = {0U, 0, 1920U, 918}
                            },
                        }
                    },
                    .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
                },
            },
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2880U, 1376U}, .MaxWin = {2880U, 1376U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2880U, 1376U}, .DstWin = {0, 0, 2880U, 1376U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2880U, 1376U}, .MaxWin = {2880U, 1376U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 2880U, 1376U}, .DstWin = {0, 0, 2880U, 1376U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 4, .CvFlowBits = 0xFU,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 24, 0, 0},
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
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 88, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
        [2] = {
            .Chan = 2U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,  
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 24, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 2U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [3] = {
            .Chan = 3U, .pCvObj = &SvcCvFlow_AmbaSegObj,
            .CvModeID = AMBASEG_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,  
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 88, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 3U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    },

#endif
    .StillCfg.EnableStill = 1,
},


{
    .FormatName = "(VIN0/2/3) MX9296+MX9295+IMX490 2880x1376P30 SHDR CV Running ",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX490_0][FOV_0]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX490_0][FOV_1]#\
         $[VIN_3][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX490_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 3U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX490Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 2880U, .Height = 1860U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_IMX490Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 2880U, .Height = 1860U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
        [3] = {
            .pDriver = &AmbaSensor_MX00_IMX490Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 2880U, .Height = 1860U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX490_2880_1860_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX490 | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        },
        [2] = { /* sensor on VIN_2 */
            [0] = {
                .SensorMode = MX00_IMX490_2880_1860_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX490 | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        },
        [3] = { /* sensor on VIN_3 */
            [0] = {
                .SensorMode = MX00_IMX490_2880_1860_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX490 | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 242U, 2880U, 1376U},
            .ActWin = {0U},
            .MainWin = {2880U, 1376U},
            .PyramidBits = 0x04U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U, .MctfDisable = MCTF_BITS_ON_CMPR, }
        },
        [1] = {
            .RawWin = {0U, 242U, 2880U, 1376U},
            .ActWin = {0U},
            .MainWin = {2880U, 1376U},
            .PyramidBits = 0x04U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U, .MctfDisable = MCTF_BITS_ON_CMPR, }
        },
        [2] = {
            .RawWin = {0U, 242U, 2880U, 1376U},
            .ActWin = {0U},
            .MainWin = {2880U, 1376U},
            .PyramidBits = 0x04U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = 0U, .MctfDisable = MCTF_BITS_ON_CMPR,  }
        },
    },
    .DispAltNum = 3, .DispAltIdx = 0,
    .DispAlt = {
        [0] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_HDMI_Obj,
                    .StrmCfg = {
                        .Win = {1920U, 918}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 0U, .SrcWin = {0U, 0U, 2880U, 1376U}, .DstWin = {0U, 0, 1920U, 918}
                            },
                        }
                    },
                    .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
                },
            },
        },
        [1] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_HDMI_Obj,
                    .StrmCfg = {
                        .Win = {1920U, 918}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 1U, .SrcWin = {0U, 0U, 2880U, 1376U}, .DstWin = {0U, 0, 1920U, 918}
                            },
                        }
                    },
                    .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
                },
            },
        },
        [2] = {
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 1,
                    .pDriver = &AmbaFPD_HDMI_Obj,
                    .StrmCfg = {
                        .Win = {1920U, 918}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 2U, .SrcWin = {0U, 0U, 2880U, 1376U}, .DstWin = {0U, 0, 1920U, 918}
                            },
                        }
                    },
                    .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
                },
            },
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2880U, 1376U}, .MaxWin = {2880U, 1376U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2880U, 1376U}, .DstWin = {0, 0, 2880U, 1376U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2880U, 1376U}, .MaxWin = {2880U, 1376U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 2880U, 1376U}, .DstWin = {0, 0, 2880U, 1376U}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2880U, 1376U}, .MaxWin = {2880U, 1376U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 2880U, 1376U}, .DstWin = {0, 0, 2880U, 1376U}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 6, .CvFlowBits = 0x3FU,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 24, 0, 0},
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
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 88, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
        [2] = {
            .Chan = 2U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,  
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 24, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 2U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [3] = {
            .Chan = 3U, .pCvObj = &SvcCvFlow_AmbaSegObj,
            .CvModeID = AMBASEG_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,  
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 88, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 3U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
        [4] = {
            .Chan = 4U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 2U,   
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 24, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 4U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [5] = {
            .Chan = 5U, .pCvObj = &SvcCvFlow_AmbaSegObj,
            .CvModeID = AMBASEG_FDAG_FC,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 2U, 
                        .FrameWidth = 2880, .FrameHeight = 1376, .NumRoi = 1U,
                        .Roi = {
                            [0] = {2, 80, 88, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 5U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    },

#endif
    .StillCfg.EnableStill = 1,
},


};
