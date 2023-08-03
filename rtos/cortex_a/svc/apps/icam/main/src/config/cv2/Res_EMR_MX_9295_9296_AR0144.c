/**
 *  @file Res_EMR_MX_9295_9296_AR0144.c
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
#include "AmbaSensor_LIMAX9295_9296_AR0144.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Stereo.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD_T30P61.h"
#include "AmbaHDMI_Def.h"

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
    .FormatName = {
        "3FOV Vout1:FOV1 Stereo Fusion 0+2"
    },
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_AR0144_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_1][S_AR0144_1][FOV_1]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_2][S_AR0144_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_LIMAXIM_AR0144Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_PROG,
                       .IntcNum = 1U,
                       .ConCatNum = 3U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [2] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {1280, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [2] = {
            .RawWin = {2560, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0, .HierWin = {   0,   0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_STEREO,
            .CvModeID = SVC_STEREO_MODE_FUSION_0_2,
            .FovInputNum = 2U,
            .FovInput = {
                [0] = 0U,   /* input is FOV_0 */
                [1] = 1U,   /* input is FOV_1 */
            },
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = {
        "3FOV Vout1:FOV2 Stereo Fusion 0+2"
    },
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_AR0144_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_1][S_AR0144_1][FOV_1]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_2][S_AR0144_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_LIMAXIM_AR0144Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_PROG,
                       .IntcNum = 1U,
                       .ConCatNum = 3U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [2] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {1280, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [2] = {
            .RawWin = {2560, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0, .HierWin = {   0,   0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_STEREO,
            .CvModeID = SVC_STEREO_MODE_FUSION_0_2,
            .FovInputNum = 2U,
            .FovInput = {
                [0] = 0U,   /* input is FOV_0 */
                [1] = 1U,   /* input is FOV_1 */
            },
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = {
        "3FOV Vout1:FOV1 Stereo FEX_SP"
    },
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_AR0144_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_1][S_AR0144_1][FOV_1]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_2][S_AR0144_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_LIMAXIM_AR0144Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_PROG,
                       .IntcNum = 1U,
                       .ConCatNum = 3U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [2] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {1280, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [2] = {
            .RawWin = {2560, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0, .HierWin = {   0,   0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_STEREO,
            .CvModeID = SVC_STEREO_MODE_FEX_SP,
            .FovInputNum = 2U,
            .FovInput = {
                [0] = 0U,   /* input is FOV_0 */
                [1] = 1U,   /* input is FOV_1 */
            },
        },
    },
    .StillCfg.EnableStill = 1,
},
{ /* FOR VIGNETTE, FOV0 VOUT TO TV */
    .FormatName = {
        "3FOV Vout1:FOV0 for vignette"
    },
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_AR0144_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_1][S_AR0144_1][FOV_1]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_2][S_AR0144_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_LIMAXIM_AR0144Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_PROG,
                       .IntcNum = 1U,
                       .ConCatNum = 3U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [2] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {1280, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [2] = {
            .RawWin = {2560, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0, .HierWin = {   0,   0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .CvFlowNum = 0U, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
{ /* FOR VIGNETTE, FOV1 VOUT TO TV */
    .FormatName = {
        "3FOV Vout1:FOV1 for vignette"
    },
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_AR0144_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_1][S_AR0144_1][FOV_1]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_2][S_AR0144_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_LIMAXIM_AR0144Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_PROG,
                       .IntcNum = 1U,
                       .ConCatNum = 3U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [2] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {1280, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [2] = {
            .RawWin = {2560, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0, .HierWin = {   0,   0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960, 240}
                    },
                }
            }
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .CvFlowNum = 0U, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
{ /* FOR VIGNETTE, FOV2 VOUT TO TV */
    .FormatName = {
        "3FOV Vout1:FOV2 for vignette"
    },
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_AR0144_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_1][S_AR0144_1][FOV_1]#\
         $[VIN_0][B_MAXIM9296_0][B_MAXIM9295_2][S_AR0144_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_LIMAXIM_AR0144Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_PROG,
                       .IntcNum = 1U,
                       .ConCatNum = 3U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 720U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
            [2] = { /* sensor idx 1 */
                .SensorMode = MAXIM_AR0144_1280_720_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {1280, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [2] = {
            .RawWin = {2560, 0, 1280, 720},
            .MainWin = {1280, 720},
            .PyramidBits = 0, .HierWin = {   0,   0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .CvFlowNum = 0U, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
};

#endif /* RES_X_X_H */
