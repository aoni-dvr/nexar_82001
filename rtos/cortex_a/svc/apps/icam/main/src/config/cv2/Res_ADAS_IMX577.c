/**
 *  @file Res_ADAS_IMX577.c
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
#include "AmbaSensor_IMX577.h"
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

#define MAIN_W 3840
#define MAIN_H 2160
#define PYRAMID_BITS 0x1E
#define HIER_W 2720
#define HIER_H 1528

#define GRP_FMID_SWITCH     "FMID_SW"

static const SVC_RES_CFG_s g_ResCfg[] = {
{   /*IMX577_3840_2160_30P_HDR NO CV*/
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 4KP30",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 4U, .OffsetY = 0U, .Width = 3840U, .Height = 4636U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577 | (IMG_DSP_HDR_MODE_0 << 16),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - MAIN_W)/2, (2160 - MAIN_H)/2, MAIN_W, MAIN_H},
            .ActWin = {0},
            .MainWin = {MAIN_W, MAIN_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 2U, .HdrBlendHieght = 2160, .LinearCE = 0U, .RawCompression = 1U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,.BootToRec = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,.BootToRec = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{   /*IMX577_3840_2160_30P_HDR FDAG*/
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 4KP30 (CV FDAG) OpenOD",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 4U, .OffsetY = 0U, .Width = 3840U, .Height = 4636U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577 | (IMG_DSP_HDR_MODE_0 << 16),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - MAIN_W)/2, (2160 - MAIN_H)/2, MAIN_W, MAIN_H},
            .ActWin = {0},
            .MainWin = {MAIN_W, MAIN_H},
            .PyramidBits = 0x8, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 2U, .HdrBlendHieght = 2160, .LinearCE = 0U, .RawCompression = 1U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
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
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
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
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 2160, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
#endif
    .StillCfg.EnableStill = 1,
},
{   /*IMX577_3840_2160_30P_HDR FDAG*/
    .FormatName = "(VIN0) IMX577 4KP30 (CV FDAG) AmbaOD(2 Scale) + AmbaSeg",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 4U, .OffsetY = 0U, .Width = 3840U, .Height = 4636U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577 | (IMG_DSP_HDR_MODE_0 << 16),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - MAIN_W)/2, (2160 - MAIN_H)/2, MAIN_W, MAIN_H},
            .ActWin = {0},
            .MainWin = {MAIN_W, MAIN_H},
            .PyramidBits = 0x9, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 2U, .HdrBlendHieght = 2160, .LinearCE = 0U, .RawCompression = 1U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
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
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
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
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 720, 400}
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
            .CvModeID = AMBAOD_FDAG_FC_2SCALE,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 2160, .NumRoi = 2U,
                        .Roi = {
                            [0] = {0, 1280, 760, 0, 0},
                            [1] = {3, 40, 62, 0, 0},
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
                        .FrameWidth = 3840, .FrameHeight = 2160, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 180, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    },
#endif
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) IMX577 4KP30 (NML EVT)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 2160},
            .ActWin = {0},
            .MainWin = {1280, 720},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits  = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = (SVC_REC_DST_NMLEVT),
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
                .SrcBits  = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = (SVC_REC_DST_NMLEVT),
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) IMX577 4KP30 (EMG EVT)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 2160},
            .ActWin = {0},
            .MainWin = {1280, 720},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1280, 720}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = (SVC_REC_DST_FILE | SVC_REC_DST_EMGEVT),
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
                .DestBits     = (SVC_REC_DST_FILE | SVC_REC_DST_EMGEVT),
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
    .StillCfg.EnableStill = 1,
},
{   /*IMX577_2560_1440_30P_HDR DUPLEX/XCODE */
    .FormatName = "(VIN0) IMX577 4KP30 (DUPLEX/XCODE) (PIP)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#\
         $[DEC_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - 2560)/2, (2160 - 1440)/2, 2560, 1440},
            .ActWin = {0},
            .MainWin = {2560, 1440},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2560, 1440}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2560U, 1440U}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {1920 - 720, 0, 720, 400}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560, 1440}, .MaxWin = {2560, 1440}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2560, 1440}, .DstWin = {0, 0, 2560, 1440}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{   /*IMX577_3840_2160_30P_HDR NET*/
    .FormatName = "(VIN0) IMX577 4KP30 (NET)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 4U, .OffsetY = 0U, .Width = 3840U, .Height = 4636U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577 | (IMG_DSP_HDR_MODE_0 << 16),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - MAIN_W)/2, (2160 - MAIN_H)/2, MAIN_W, MAIN_H},
            .ActWin = {0},
            .MainWin = {MAIN_W, MAIN_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 2U, .HdrBlendHieght = 2160, .LinearCE = 0U, .RawCompression = 1U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
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
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = (SVC_REC_DST_FILE | SVC_REC_DST_NET),
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{   /*IMX577_3840_2160_30P_HDR NO CV TIMELAPSE*/

#define TLPS_MAIN_W 2560
#define TLPS_MAIN_H 1440

    .FormatName = "(VIN0) IMX577 4KP30 (TIMELAPSE)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 4U, .OffsetY = 0U, .Width = 3840U, .Height = 4636U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577 | (IMG_DSP_HDR_MODE_0 << 16),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - TLPS_MAIN_W)/2, (2160 - MAIN_H)/2, TLPS_MAIN_W, TLPS_MAIN_H},
            .ActWin = {0},
            .MainWin = {TLPS_MAIN_W, TLPS_MAIN_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 2U, .HdrBlendHieght = 2160, .LinearCE = 0U, .RawCompression = 1U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, TLPS_MAIN_W, TLPS_MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, TLPS_MAIN_W, TLPS_MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {TLPS_MAIN_W, TLPS_MAIN_H}, .MaxWin = {TLPS_MAIN_W, TLPS_MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, TLPS_MAIN_W, TLPS_MAIN_H}, .DstWin = {0, 0, TLPS_MAIN_W, TLPS_MAIN_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1, .TimeLapse = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, TLPS_MAIN_W, TLPS_MAIN_H}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{   /* (VIN0) IMX577 1080P30 (DUAL DECODE) (PIP) */
    .FormatName = "(VIN0) IMX577 1080P30 (DUAL DECODE) (PIP)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0][FOV_1]#\
         $[DEC_0][FOV_2]#\
         $[DEC_1][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
        [1] = {
            .RawWin = {1920, 1080, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
        [2] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { 0 }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 4U,
                .ChanCfg = {
#define PIP_WIDTH  (896)
#define PIP_HEIGHT (504)
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {((960 - PIP_WIDTH) / 2), 540 + ((540 - PIP_HEIGHT) / 2), PIP_WIDTH, PIP_HEIGHT}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {960 + ((960 - PIP_WIDTH) / 2),((540 - PIP_HEIGHT) / 2), PIP_WIDTH, PIP_HEIGHT}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {960 + ((960 - PIP_WIDTH) / 2), 540 + ((540 - PIP_HEIGHT) / 2), PIP_WIDTH, PIP_HEIGHT}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN0) IMX577 4KP30 (LINEAR)(TIME-DIVISION)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_PROG,
                       .ConCatNum = 1U,
                       .IntcNum = 1U,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 3840U, .Height = 2160U},
                       .TDNum = 2U,
                }
            },
            .SubChanTDFrmNum = {
                [0] = { 1U, 1U, 0U, 0U, 0U, 0U, 0U, 0U }
            },
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
                .TimeDivisionIQTable = {
                    IQ_SSR_IMX577,
                    IQ_SSR_IMX577,
                    0, 0, 0, 0, 0, 0
                }
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - MAIN_W)/2, (2160 - MAIN_H)/2, MAIN_W, MAIN_H},
            .ActWin = {0},
            .MainWin = {MAIN_W, MAIN_H},
            //.PyramidBits = PYRAMID_BITS, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U },
            .VirtChan = {0x2U, 0x0U}
        },
        [1] = {
            .RawWin = {(3840 - MAIN_W)/2, (2160 - MAIN_H)/2, MAIN_W, MAIN_H},
            .ActWin = {0},
            .MainWin = {MAIN_W, MAIN_H},
            //.PyramidBits = PYRAMID_BITS, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U },
            .VirtChan = {0x2U, 0x100U}
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum =1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{   /*IMX577_3840_2160_A30P */
    .FormatName = "(VIN0) IMX577 4KP30 (Linear Mode, CAP=Main=4K) (CV FDAG) OpenOD",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 2160},
            .ActWin = {0},
            .MainWin = {3840, 2160},
            .PyramidBits = 0x8, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 2160, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
#endif
    .StillCfg.EnableStill = 1,
},
{   /*IMX577_3840_2160_A30P */
    .FormatName = "(VIN0) IMX577 4KP30 (Linear Mode, CAP=Main=1080P) (CV FDAG) OpenOD",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 960U, .OffsetY = 540U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
#endif
    .StillCfg.EnableStill = 1,
},
{   /*IMX577_1920_1080_30P DUPLEX/XCODE */
    .FormatName = "DEC Pyramid (DUPLEX) (CV FDAG) OpenOD",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#\
         $[DEC_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - 1920)/2, (2160 - 1080)/2, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2, .HierWin = {0U, 0U},
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {1920 - 720, 0, 720, 400}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
#endif
    .StillCfg.EnableStill = 1,
},
{   /* XCODE RefOD */
    .FormatName = "DEC Pyramid (DEC-Only) (CV FDAG) OpenOD",
    .VinTree = {
        "$[DEC_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 0U,
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x2, .HierWin = {0U, 0U},
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0, 0, 1920U, 1080U}
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 168, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
#endif
    .StillCfg.EnableStill = 1,
},
{   /*IMX577_2560_1440_30P_HDR DUPLEX/XCODE */
    .FormatName = "(VIN0) IMX577 4KP30 (DUPLEX/XCODE)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#\
         $[DEC_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - 2560)/2, (2160 - 1440)/2, 2560, 1440},
            .ActWin = {0},
            .MainWin = {2560, 1440},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2560, 1440}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2560, 1440}, .MaxWin = {2560, 1440}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2560, 1440}, .DstWin = {0, 0, 2560, 1440}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{   /*IMX577_3840_2160_30P_HDR FDAG*/
    .FormatName = "(VIN0) IMX577 4KP30 (CV FDAG Linux) OpenOD",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 4U, .OffsetY = 0U, .Width = 3840U, .Height = 4636U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577 | (IMG_DSP_HDR_MODE_0 << 16),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - MAIN_W)/2, (2160 - MAIN_H)/2, MAIN_W, MAIN_H},
            .ActWin = {0},
            .MainWin = {MAIN_W, MAIN_H},
            .PyramidBits = PYRAMID_BITS, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 2U, .HdrBlendHieght = 2160, .LinearCE = 0U, .RawCompression = 1U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
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
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
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
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
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
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,    /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 2160, .NumRoi = 0U
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
            .PrivFlag = 0U, /* Linux CH0 */
        },
    },
#endif
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) IMX577 4KP30 Linear (for Still)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 2160},
            .ActWin = {0},
            .MainWin = {3840, 2160},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0, .HdrBlendHieght = 0, .LinearCE = 0U, .RawCompression = 1 }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 1, .EnableHDR = 0,
        .NumVin = 1,
        .RawCfg = {
            [0] = {
                .MaxRaw = {.Compressed = 1, .CeNeeded = 0, .FrameNum = 3, .Width = 4032, .Height = 3024,
                           .EffectW = 4032, .EffectH = 3024, .SensorMode = IMX577_4056_3040_30P},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 4032, 3024, 0, 0, 0},
                },
            },
        }
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{   /*IMX577_2028_1520_30P NO CV*/
    .FormatName = "(VIN0) IMX577 2016x1512 Linear 2x binning (for Still)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 6U, .OffsetY = 4U, .Width = 2016, .Height = 1512},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_2028_1520_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 2016, 1512},
            .ActWin = {0},
            .MainWin = {2016, 1512},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0, .HdrBlendHieght = 0, .LinearCE = 0U, .RawCompression = 1 }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {720U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2016, 1512}, .DstWin = {0, 0, 720U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1440U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2016, 1512}, .DstWin = {0, 0, 1440U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {2016, 1512}, .MaxWin = {2016, 1512}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2016, 1512}, .DstWin = {0, 0, 2016, 1512}
                    },
                }
            }
        },
    },
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 1, .EnableHDR = 0,
        .NumVin = 1,
        .RawCfg = {
            [0] = {
                .MaxRaw = {.Compressed = 1, .CeNeeded = 0, .FrameNum = 3, .Width = 4032, .Height = 3024,
                           .EffectW = 4032, .EffectH = 3024, .SensorMode = IMX577_4056_3040_30P},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 4032, 3024, 0, 0, 0},
                },
            },
        }
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{   /*IMX577_3840_2160_30P_HDR NO CV*/
    .FormatName = "(VIN0) IMX577 4KP30 HDR (for Still)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 4U, .OffsetY = 0U, .Width = 3840U, .Height = 4636U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577 | (IMG_DSP_HDR_MODE_0 << 16),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - MAIN_W)/2, (2160 - MAIN_H)/2, MAIN_W, MAIN_H},
            .ActWin = {0},
            .MainWin = {MAIN_W, MAIN_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 2U, .HdrBlendHieght = 2160, .LinearCE = 0U, .RawCompression = 1U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
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
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 0, .EnableHDR = 1,
        .NumVin = 1,
        .RawCfg = {
            [0] = {
                .MaxRaw = {.Compressed = 1, .CeNeeded = 1, .FrameNum = 3, .Width = 3840, .Height = 4636,
                           .EffectW = 3840, .EffectH = 2160, .SensorMode = IMX577_3840_2160_30P_HDR},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 3840, 2160, 0, 0, 0},
                },
            },
        }
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{
#define BITSCMP_WIDTH  (2560U)
#define BITSCMP_HEIGHT (1600U)
    .FormatName = "(VIN0) IMX577 BITSCMP 2*4MP30",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0][FOV_1]]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {(UINT16)((3840U - BITSCMP_WIDTH) / 2U), (UINT16)((2160U - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [1] = {
            .RawWin = {(UINT16)((3840U - BITSCMP_WIDTH) / 2U), (UINT16)((2160U - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_A60HZ,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .MaxWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .MaxWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}
                    },
                }
            }
        },
    },
},

{   /*IMX577_3840_2160_30P_HDR NO CV*/
    .FormatName = "(VIN0) IMX577 4KP30 (Rotation)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 4U, .OffsetY = 0U, .Width = 3840U, .Height = 4636U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577 | (IMG_DSP_HDR_MODE_0 << 16),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {(3840 - MAIN_W)/2, (2160 - MAIN_H)/2, MAIN_W, MAIN_H},
            .ActWin = {0},
            .MainWin = {MAIN_W, MAIN_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 2U, .HdrBlendHieght = 2160, .LinearCE = 0U, .RawCompression = 1U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {608, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 608U, 1080U}, .RotateFlip = AMBA_DSP_ROTATE_90,
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001},
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
                .Win = {MAIN_W, MAIN_H}, .MaxWin = {MAIN_W, MAIN_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, MAIN_W, MAIN_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720, 400}, .MaxWin = {720, 400}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, MAIN_W, MAIN_H}, .DstWin = {0, 0, 720, 400}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{
#define Y2Y_BITSCMP_WIDTH  (2560U)
#define Y2Y_BITSCMP_HEIGHT (1600U)
    .FormatName = "Y2Y BITSCMP 2*2MP30",
    .VinTree = {
        "$[M_YUV420_0][FOV_0]#\
         $[M_YUV420_1][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 0U,
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .FromVirtVin = SVC_RES_FOV_FROM_VIRT_VIN,},
        },
        [1] = {
            .RawWin = {0, 0, Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .FromVirtVin = ((UINT32)SVC_RES_FOV_FROM_VIRT_VIN | 1UL),},
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 540U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}, .DstWin = {0U, 0U, 960U, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}, .DstWin = {960, 0, 960U, 540U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
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
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}, .MaxWin = {Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}, .DstWin = {0, 0, Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}
                    },
                }
            },
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}, .MaxWin = {Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}, .DstWin = {0, 0, Y2Y_BITSCMP_WIDTH, Y2Y_BITSCMP_HEIGHT}
                    },
                }
            },
        },
    },
},
};

#endif /* RES_X_X_H */
