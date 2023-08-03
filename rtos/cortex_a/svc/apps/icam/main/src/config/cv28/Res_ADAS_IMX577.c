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
#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD.h"
#include "AmbaFPD_LT9611UXC.h"
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaCVBS.h"
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

#define UHD8K_W  (7680U)
#define UHD8K_H  (4320U)
#define UHD5K_W  (5120U)
#define UHD5K_H  (2880U)
#define UHD4K_W  (3840U)
#define UHD4K_H  (2160U)
#define QHD6M_W  (3200U)
#define QHD6M_H  (1800U)
#define WQHD_W   (2560U)
#define WQHD_H   (1440U)
#define FHD_W    (1920U)
#define FHD_H    (1080U)
#define HD_W     (1280U)
#define HD_H      (720U)

#define GRP_FMID_SWITCH     "FMID_SW"

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 3840x2160 8MP30 (HDMI)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
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
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, UHD4K_W, UHD4K_H},
            .ActWin = {0},
            .MainWin = {UHD4K_W, UHD4K_H},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1, .BootToRec = 1U,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {UHD4K_W, UHD4K_H}, .MaxWin = {UHD4K_W, UHD4K_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, UHD4K_W, UHD4K_H}
                    },
                }
            }
        },
    },

    .StillCfg.EnableStill = 1,
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 3840x2160 8MP30 (HDMI) OpenOD",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
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
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, UHD4K_W, UHD4K_H},
            .ActWin = {0},
            .MainWin = {UHD4K_W, UHD4K_H},
            .PyramidBits = 0x8U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
                .Win = {UHD4K_W, UHD4K_H}, .MaxWin = {UHD4K_W, UHD4K_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, UHD4K_W, UHD4K_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
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
                        .FrameWidth = UHD4K_W, .FrameHeight = UHD4K_H, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 128, 64, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 3840x2160 8MP30 (HDMI) OpenOD + OpenSeg",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
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
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, UHD4K_W, UHD4K_H},
            .ActWin = {0},
            .MainWin = {UHD4K_W, UHD4K_H},
            .PyramidBits = 0x8U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
                .Win = {UHD4K_W, UHD4K_H}, .MaxWin = {UHD4K_W, UHD4K_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, UHD4K_W, UHD4K_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720U, 400U}, .MaxWin = {720U, 400U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, 720U, 400U}
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
                        .FrameWidth = UHD4K_W, .FrameHeight = UHD4K_H, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 128, 64, 0, 0},
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
                        .FrameWidth = UHD4K_W, .FrameHeight = UHD4K_H, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 180, 0, 0},
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
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 3840x2160 8MP30 (HDMI) HDR",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 4U, .OffsetY = 0U, .Width = UHD4K_W, .Height = 4636U},
            .TimeoutCfg = {5000U, 100U},
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
            .RawWin = {0, 0, UHD4K_W, UHD4K_H},
            .ActWin = {0},
            .MainWin = {UHD4K_W, UHD4K_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 2U, .HdrBlendHieght = 2160U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {UHD4K_W, UHD4K_H}, .MaxWin = {UHD4K_W, UHD4K_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, UHD4K_W, UHD4K_H}
                    },
                }
            }
        },
    },

    .StillCfg.EnableStill = 1,
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 3200x1800 6MP30 (HDMI)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
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
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, UHD4K_W, UHD4K_H},
            .ActWin = {0},
            .MainWin = {QHD6M_W, QHD6M_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = 0U, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, QHD6M_W, QHD6M_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
                .Win = {QHD6M_W, QHD6M_H}, .MaxWin = {QHD6M_W, QHD6M_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, QHD6M_W, QHD6M_H}, .DstWin = {0, 0, QHD6M_W, QHD6M_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720U, 400U}, .MaxWin = {720U, 400U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, QHD6M_W, QHD6M_H}, .DstWin = {0, 0, 720U, 400U}
                    },
                }
            }
        },
    },

    .StillCfg.EnableStill = 1,
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 2560x1440 4MP30 (HDMI)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
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
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, UHD4K_W, UHD4K_H},
            .ActWin = {0},
            .MainWin = {WQHD_W, WQHD_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = 0U, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, WQHD_W, WQHD_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
                .Win = {WQHD_W, WQHD_H}, .MaxWin = {WQHD_W, WQHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, WQHD_W, WQHD_H}, .DstWin = {0, 0, WQHD_W, WQHD_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720U, 400U}, .MaxWin = {720U, 400U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, WQHD_W, WQHD_H}, .DstWin = {0, 0, 720U, 400U}
                    },
                }
            }
        },
    },

    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) IMX577 1920x1080 2MP30 (NET)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = FHD_W, .Height = FHD_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_1920_1080_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, FHD_W, FHD_H},
            .ActWin = {0},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = 0U, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = (SVC_REC_DST_FILE | SVC_REC_DST_NET),
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {720U, 400U}, .MaxWin = {720U, 400U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, 720U, 400U}
                    },
                }
            }
        },
    },
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 1920x1080 2MP30 (HDMI)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = FHD_W, .Height = FHD_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_1920_1080_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, FHD_W, FHD_H},
            .ActWin = {0},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = 0U, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            }
        },
    },

    .StillCfg.EnableStill = 1,
},
#if defined(CONFIG_ENABLE_AMBALINK)
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) IMX577 1920x1080 2MP30 (CV FDAG Linux) OpenOD (HDMI)",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = FHD_W, .Height = FHD_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_1920_1080_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, FHD_W, FHD_H},
            .ActWin = {0},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0x04, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = 0U, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
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
                        .FrameWidth = FHD_W, .FrameHeight = FHD_H, .NumRoi = 0U
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
            .PrivFlag = 0U, /* Linux CH0 */
        },
    },
#endif
},
#endif
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
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
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
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, UHD4K_W, UHD4K_H},
            .ActWin = {0},
            .MainWin = {UHD4K_W, UHD4K_H},
            .PyramidBits = 0x8U, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1440U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 2016, 1512}, .DstWin = {0, 0, 1440U, 1080U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
},
{
#define BITSCMP_WIDTH  (1920U)
#define BITSCMP_HEIGHT (1600U)
    .FormatName = "(VIN0) IMX577 BITSCMP 2*3MP30",
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
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
{
#define Y2Y_BITSCMP_WIDTH  (1920U)
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
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
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
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
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
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
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
