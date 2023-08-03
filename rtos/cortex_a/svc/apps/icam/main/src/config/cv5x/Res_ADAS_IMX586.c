/**
 *  @file Res_ADAS_IMX586.c
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
#include "AmbaSensor_IMX586.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaFPD_HDMI.h"
#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaHDMI_Def.h"
#include "AmbaDSP_VideoEnc.h"

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
    .FormatName = "(VIN0) IMX586 8KP30 REC",
    .VinTree = {
        "$[VIN_0][S_IMX586_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX586Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX586_7680_4320_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX586,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 7680, 4320},
            .ActWin = {0},
            .MainWin = {7680, 4320},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .WarpLumaWaitLine = 64U, .MaxHorWarpCompensation = 64U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .Win = {7680, 4320}, .MaxWin = {7680, 4320}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 7680, 4320}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) IMX586 8KP30 REC CV (OpenOD + OpenSeg)",
    .VinTree = {
        "$[VIN_0][S_IMX586_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX586Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX586_7680_4320_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX586,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 7680, 4320},
            .ActWin = {0},
            .MainWin = {7680, 4320},
            .PyramidBits = 0x20U, .HierWin = {0U, 0U},
            .PipeCfg = {.PipeMode = SVC_VDOPIPE_DRAMEFCY, .RawCompression = 1U, .WarpLumaWaitLine = 64U, .MaxHorWarpCompensation = 64U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .Win = {7680, 4320}, .MaxWin = {7680, 4320}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 7680, 4320}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 7680, .FrameHeight = 4320, .NumRoi = 1U,
                        .Roi = {
                            [0] = {5, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
#if (CONFIG_ICAM_CLK_LINE == 0x1)
                .SrcRate = 0x3, /* Tuned according to Dram util */
#else
                .SrcRate = 0x6, /* Tuned according to Dram util */
#endif

            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefSegObj,
            .CvModeID = REFSEG_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 7680, .FrameHeight = 4320, .NumRoi = 1U,
                        .Roi = {
                            [0] = {5, 40, 180, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
#if (CONFIG_ICAM_CLK_LINE == 0x1)
                .SrcRate = 0x3, /* Tuned according to Dram util */
#else
                .SrcRate = 0x6, /* Tuned according to Dram util */
#endif
            },
            .CvFlowType = SVC_CV_FLOW_REF_SEG,
        },
    },
#endif
},
{
    .FormatName = "(VIN0) IMX586 8KP30 2-FOV on HDMI",
    .VinTree = {
        "$[VIN_0][S_IMX586_0][FOV_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX586Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX586_7680_4320_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX586,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 2160},
            // .ActWin = {0, 0, 3840, 2160},
            .MainWin = {3840, 2160},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .MaxHorWarpCompensation = 64U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
        [1] = {
            .RawWin = {0, 1080, 3840, 2160},
            // .ActWin = {0, 2160, 3840, 2160},
            .MainWin = {3840, 2160},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .MaxHorWarpCompensation = 64U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 540}, .MaxWin = {1920, 1080}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 960, 540}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {960, 0, 960, 540}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .Win = {3840, 2160}, .MaxWin = {3840, 2160}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 3840, 2160}
                    },
                }
            }
        },
    },
},
{
    #define DECODE_W    (1920U)
    #define DECODE_H    (1080U)

    .FormatName = "(DUPLEX) DEC Only (FHD)",
    .VinTree = {
        "$[DEC_0][FOV_0]#!"
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, DECODE_W, DECODE_H},
            .MainWin = {DECODE_W, DECODE_H},
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, DECODE_W, DECODE_H}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, DECODE_W, DECODE_H}, .DstWin = {0, 0, 1280U, 720U}
                    },
                }
            }
        },
    },
},
#if 0
{
    .FormatName = "(DUPLEX) Liveview (FHD) + DEC (FHD)",
    .VinTree = {
        "$[VIN_0][S_IMX586_0][FOV_0]#!"
/*        "$[VIN_0][S_IMX586_0][FOV_0]#\
           $[DEC_0][FOV_1]#!"*/
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX586Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX586_7680_4320_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX586,
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    //.FovNum = 2U,
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            //.RawWin = {2880, 1620, 1920, 1080},
            .RawWin = {1920, 1080, 3840, 2160},
            .ActWin = {0},
            .MainWin = {3840, 2160},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
        //[1] = {
        //    .RawWin = {0, 0, 1920, 1080},
        //    .ActWin = {0},
        //    .MainWin = {1920, 1080},
        //    .PyramidBits = 0, .HierWin = {0, 0},
        //    .PipeCfg = { 0 }
        //},
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    //.RecNum = 2U, .RecBits = 0x03U,
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
                .Win = {3840, 2160}, .MaxWin = {3840, 2160}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 3840, 2160}
                    },
                }
            }
        },
        //[1] = {
        //    .RecSetting = {
        //        .SrcBits = (SVC_REC_SRC_VIDEO),
        //        .DestBits     = SVC_REC_DST_FILE,
        //        .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
        //        .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
        //    },
        //    .StrmCfg = {
        //        .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
        //        .ChanCfg = {
        //            [0] = {
        //                .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
        //            },
        //        }
        //    }
        //},
    },
},
#endif
{
    .FormatName = "(VIN0) IMX586 8KP30, 4KP30 VOUT (VOUT Cropped)",
    .VinTree = {
        "$[VIN_0][S_IMX586_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX586Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX586_7680_4320_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX586,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 7680, 4320},
            .ActWin = {0},
            .MainWin = {7680, 4320},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .WarpLumaWaitLine = 64U, .MaxHorWarpCompensation = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {3840, 2160}, .MaxWin = {3840, 2160}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {1920, 1080, 3840, 2160}, .DstWin = {0, 0, 3840, 2160}
                    },
                }
            },
            .DevMode = HDMI_VIC_3840_2160_A30P_WIDE,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
},
{
    .FormatName = "(VIN0) IMX586 8KP30, 8KP30 VOUT",
    .VinTree = {
        "$[VIN_0][S_IMX586_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX586Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX586_7680_4320_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX586,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 7680, 4320},
            .ActWin = {0},
            .MainWin = {7680, 4320},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .WarpLumaWaitLine = 64U, .MaxHorWarpCompensation = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {7680, 4320}, .MaxWin = {7680, 4320}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 7680, 4320}
                    },
                }
            },
            .DevMode = HDMI_VIC_7680_4320_30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
},
{
    .FormatName = "(VIN0) IMX586 8KP30, 4KP30 VOUT (VOUT Scaled)",
    .VinTree = {
        "$[VIN_0][S_IMX586_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX586Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX586_7680_4320_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX586,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 7680, 4320},
            .ActWin = {0},
            .MainWin = {7680, 4320},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .WarpLumaWaitLine = 64U, .MaxHorWarpCompensation = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {3840, 2160}, .MaxWin = {3840, 2160}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 3840, 2160}
                    },
                }
            },
            .DevMode = HDMI_VIC_3840_2160_A30P_WIDE,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
},
{
    .FormatName = "(VIN0) IMX586 8KP30 Liv CV (OpenOD + OpenSeg)",
    .VinTree = {
        "$[VIN_0][S_IMX586_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX586Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX586_7680_4320_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX586,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 7680, 4320},
            .ActWin = {0},
            .MainWin = {7680, 4320},
            .PyramidBits = 0x20U, .HierWin = {0U, 0U},
            .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .WarpLumaWaitLine = 64U, .MaxHorWarpCompensation = 64U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 7680, .FrameHeight = 4320, .NumRoi = 1U,
                        .Roi = {
                            [0] = {5, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefSegObj,
            .CvModeID = REFSEG_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 7680, .FrameHeight = 4320, .NumRoi = 1U,
                        .Roi = {
                            [0] = {5, 40, 180, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_SEG,
        },
    },
#endif
},
{
    .FormatName = "(VIN0) IMX586 8KP30 EIS",
    .VinTree = {
        "$[VIN_0][S_IMX586_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX586Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX586_7680_4320_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX586,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 7680, 4320},
            .ActWin = {0},
            .MainWin = {7680, 4320},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            // not support record if MaxHorWarpCompensation 480, check more in 8KP24 mode
            .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .MaxHorWarpCompensation = 480U, .MctfDisable = MCTF_BITS_ON_CMPR},
            // if need record, MaxHorWarpCompensation 288
            // .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .MaxHorWarpCompensation = 288U, .MctfDisable = MCTF_BITS_ON_CMPR},
            // disable mctf and mcts: MaxHorWarpCompensation 384
            // .PipeCfg = {.PipeMode = SVC_VDOPIPE_NORMAL, .RawCompression = 1U, .MaxHorWarpCompensation = 384U, .MctfDisable = MCTF_BITS_OFF, .MctsDisable = MCTS_BITS_OFF},
            .IQTable = 1U << 8U, /* enable eis */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .Win = {7680, 4320}, .MaxWin = {7680, 4320}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 7680, 4320}
                    },
                }
            }
        },
    },
},
};

#endif /* RES_X_X_H */
