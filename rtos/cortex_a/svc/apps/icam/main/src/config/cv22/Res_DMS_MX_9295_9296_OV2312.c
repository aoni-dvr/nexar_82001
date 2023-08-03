/**
 *  @file Res_ADAS_MX_9295_9296_IMX390.c
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
#include "AmbaSensor_MAX9295_9296_OV2312_RGBIR.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
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
#include "SvcCvAppDef.h"
#endif

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX9296+1x(MX9295+OV2312), CAP = 1600 x 1300, P30, RGBIR",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_OV2312_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_OV2312Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1600U, .Height = 1300U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_OV2312_1600_1300_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV2312_TD_RGBIR,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1600, 1280},
            .MainWin = {1600, 1280},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1280, 1024}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 1280, 1024}
                    },
                }
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
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
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 1280}, .MaxWin = {1600, 1280}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 1600, 1280}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
#if 0//defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC_PCPT_KP ,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_MAIN_YUV, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1280, .FrameHeight = 720, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 40, 0, 0},
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
    .FormatName = "(VIN0) MX9296+1x(MX9295+OV2312), CAP = 1600 x 1300, P30, IR ONLY",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_OV2312_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_OV2312Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1600U, .Height = 1300U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_OV2312_1600_1300_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV2312_TD_IR,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1600, 1280},
            .MainWin = {1600, 1280},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1280, 1024}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 1280, 1024}
                    },
                }
            },
            .DevMode = (UINT8) HDMI_VIC_1920_1080_A60P,
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
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 1280}, .MaxWin = {1600, 1280}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 1600, 1280}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
#if 0//defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_FC_PCPT_KP ,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_MAIN_YUV, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1280, .FrameHeight = 720, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 40, 0, 0},
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
    .FormatName = "(VIN0) MX9296+1x(MX9295+OV2312) P30 2xFOVs (RGB/IR)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_OV2312_0][FOV_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_OV2312Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1600U, .Height = 1300U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_OV2312_1600_1300_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV2312_SIMUL_IR,
            },
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1600, 1280},
            .MainWin = {1600, 1280},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U },
            .IQTable = IQ_SSR_OV2312_SIMUL_RGBIR,
        },
        [1] = {
            .RawWin = {0, 0, 1600, 1280},
            .MainWin = {1600, 1280},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 768}, .MaxWin = {1920, 1080}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {960, 0, 960, 768}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 960, 768}
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
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 1280}, .MaxWin = {1600, 1280}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 1600, 1280}
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
                .Win = {1600, 1280}, .MaxWin = {1600, 1280}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 1600, 1280}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},
{
    .FormatName = "(VIN0) MX9296+1x(MX9295+OV2312) P60 (TIME-DIVISION)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_OV2312_0][FOV_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_OV2312Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1600U, .Height = 1300U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_PROG,
                       .ConCatNum = 1U,
                       .IntcNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1600U, .Height = 1300U},
                       .TDNum = 2U,
                }
            },
            .SubChanTDFrmNum = {
                [0] = { 1U, 1U, 0U, 0U, 0U, 0U, 0U, 0U }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_OV2312_1600_1300_A60P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV2312_TD_RGBIR,
                .TimeDivisionIQTable = {
                    IQ_SSR_OV2312_TD_RGBIR,
                    IQ_SSR_OV2312_TD_IR,
                    0, 0, 0, 0, 0, 0
                }
            },
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1600, 1280},
            .MainWin = {1600, 1280},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U },
            .VirtChan = {0x2U, 0x0U}
        },
        [1] = {
            .RawWin = {0, 0, 1600, 1280},
            .MainWin = {1600, 1280},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .MctfDisable = 0U, .MctsDisable = 0U, .LinearCE = 0U, .RawCompression = 0U },
            .VirtChan = {0x2U, 0x100U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 768}, .MaxWin = {1920, 1080}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {960, 0, 960, 768}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 960, 768}
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
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 1280}, .MaxWin = {1600, 1280}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 1600, 1280}
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
                .Win = {1600, 1280}, .MaxWin = {1600, 1280}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1600, 1280}, .DstWin = {0, 0, 1600, 1280}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
},
};

#endif /* RES_X_X_H */
