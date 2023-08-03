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
#include "AmbaHDMI_Def.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_RefSeg.h"
#if defined(CONFIG_LINUX)
#include "SvcCvFlow_RemoteApp.h"
#endif
#include "SvcCvAppDef.h"
#endif

#define UHD4K_W  (3840U)
#define UHD4K_H  (2160U)
#define FHD_W    (1920U)
#define FHD_H    (1080U)

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN8) IMX577 4KP30",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1, .BootToRec = 1U,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN8) IMX577 4KP30 (CV FDAG) OpenOD + OpenSeg",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
#if !defined(CONFIG_LINUX)
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
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefSegObj,
            .CvModeID = REFSEG_FDAG_OPEN_NET_1,
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
            .CvFlowType = SVC_CV_FLOW_REF_SEG,
        },
    },
#else
    /* running od + seg on linux may make cpu loading too heavy */
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
#endif
},
{
    .FormatName = "(VIN8) IMX577 4KP30 TimeLapse",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1, .TimeLapse = 1U,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN8) IMX577 4KP60",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A60P,
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
},
{
    .FormatName = "(VIN8) IMX577 4KP30, Dual SD",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
                .UseAuxStg = 1,
            },
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN8) IMX577 4KP30 (CV FDAG Linux) OpenOD",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .PyramidBits = 0x1E, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
                        .FrameWidth = UHD4K_W, .FrameHeight = UHD4K_H, .NumRoi = 0U
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
            .PrivFlag = 0U, /* Linux CH0 */
        },
    }
#endif
},
{
    .FormatName = "(VIN8) IMX577 1080P30 Linear (for Still and Raw Encode)",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920, .Height = 1080},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, FHD_W, FHD_H}
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
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 1, .EnableHDR = 1,
        .NumVin = 1,
        .RawCfg = {
            [8] = {
                .MaxRaw = {.Compressed = 1, .CeNeeded = 1, .FrameNum = 3, .Width = 1920, .Height = 1080,
                           .EffectW = 1920, .EffectH = 1080, .SensorMode = IMX577_1920_1080_A30P},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 1920, 1080, 0, 0, 0},
                },
            },
        }
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{
    .FormatName = "(VIN8) IMX577 4KP30 Linear (for Still)",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920, .Height = 1080},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, FHD_W, FHD_H}
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
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 1, .EnableHDR = 0,
        .NumVin = 1,
        .RawCfg = {
            [8] = {
                .MaxRaw = {.Compressed = 1, .CeNeeded = 0, .FrameNum = 3, .Width = UHD4K_W, .Height = UHD4K_H,
                           .EffectW = UHD4K_W, .EffectH = UHD4K_H, .SensorMode = IMX577_3840_2160_A30P},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, UHD4K_W, UHD4K_H, 0, 0, 0},
                },
            },
        }
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{
    .FormatName = "(VIN8) IMX577 4KP30 + DEC FHD to YUV420 FHD",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#\
         $[M_YUV420_0][FOV_1]#\
         $[DEC_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, UHD4K_W, UHD4K_H},
            .ActWin = {0},
            .MainWin = {UHD4K_W, UHD4K_H},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
        [1] = {
            .RawWin = {0, 0, FHD_W, FHD_H},
            .ActWin = {0},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .FromVirtVin = SVC_RES_FOV_FROM_VIRT_VIN,},
        },
        [2] = {
            .RawWin = {0, 0, FHD_W, FHD_H},
            .MainWin = {FHD_W, FHD_H},
            .DecFeedBits = 2U,    /* feed to MemYuv420 (FOV1) */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, 1920, 1080}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, 960, 540}
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
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            }
        },
    },
},
{
    .FormatName = "(VIN8) IMX577 4KP30 Linear (for Eis evaluation)",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840U, .Height = 2160U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0U,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 3840U, 2160U},
            .ActWin = {0U},
            .MainWin = {3840U, 2160U},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .MaxHorWarpCompensation = 320U},
            .IQTable = 1U << 8U, /* enable eis */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 2160U}, .DstWin = {0U, 0U, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60U, .NumUnitsInTick = 1U},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 2160U}, .MaxWin = {3840U, 2160U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 2160U}, .DstWin = {0U, 0U, 3840U, 2160U}
                    },
                }
            }
        },
    },
    .StillCfg = {
        .EnableStill = 1U, .EnableStillRaw = 1U, .EnableHISO = 1U, .EnableHDR = 0U,
        .NumVin = 1U,
        .RawCfg = {
            [8] = {
                .MaxRaw = {.Compressed = 1U, .CeNeeded = 0U, .FrameNum = 3U, .Width = 3840U, .Height = 2160U,
                           .EffectW = 3840U, .EffectH = 2160U, .SensorMode = IMX577_3840_2160_A30P},
                .ChanMask = 0x1U,
                .ChanWin = {
                    [0] = {0U, 0U, 3840U, 2160U, 0U, 0U, 0U},
                },
            },
        }
    },
    .CvFlowNum = 0U, .CvFlowBits = 0x0U,
},
{
    .FormatName = "(VIN8) IMX577 1080P30 Linear (for Eis evaluation)",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920, .Height = 1080},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
            [0] = {
                .SensorMode = IMX577_1920_1080_A30P,
                .SensorGroup = 0U,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR},
            .IQTable = 1U << 8U, /* enable eis */
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, FHD_W, FHD_H}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60U, .NumUnitsInTick = 1U},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        },
    },
    .StillCfg = {
        .EnableStill = 1U, .EnableStillRaw = 1U, .EnableHISO = 1U, .EnableHDR = 1U,
        .NumVin = 1,
        .RawCfg = {
            [8] = {
                .MaxRaw = {.Compressed = 1U, .CeNeeded = 1U, .FrameNum = 3U, .Width = 1920U, .Height = 1080U,
                           .EffectW = 1920U, .EffectH = 1080U, .SensorMode = IMX577_1920_1080_A30P},
                .ChanMask = 0x1U,
                .ChanWin = {
                    [0] = {0U, 0U, 1920U, 1080U, 0U, 0U, 0U},
                },
            },
        }
    },
    .CvFlowNum = 0U, .CvFlowBits = 0x0U,
},
{
    .FormatName = "(VIN8) IMX577 4KP60 (CV FDAG) OpenOD",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A60P,
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefSegObj,
            .CvModeID = REFSEG_FDAG_OPEN_NET_1,
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
            .CvFlowType = SVC_CV_FLOW_REF_SEG,
        },
    },
#endif
},
{
    .FormatName = "(VIN8) IMX577 4KP30 (NET)",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
                .DestBits = (SVC_REC_DST_FILE),
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
                .DestBits = (SVC_REC_DST_NET | SVC_REC_DST_FILE),
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
{
    .FormatName = "(VIN8) IMX577 4KP30 Dual EncEngine",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
            .FixedFrameRate = 1,
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1, .VEngineId = 0U,
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
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1, .VEngineId = 1U,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN8) IMX577 4KP30 (NET) DUAL RTSP STREAMS",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
            .FixedFrameRate = 1,
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
                .DestBits = SVC_REC_DST_NET,
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
                .DestBits = SVC_REC_DST_NET,
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
},
{
    .FormatName = "(VIN8) IMX577 4KP30 (CV FDAG Linux) Remote App",
    .VinTree = {
        "$[VIN_8][S_IMX577_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [8] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = UHD4K_W, .Height = UHD4K_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [8] = { /* sensor on VIN_8 */
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
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, UHD4K_W, UHD4K_H}, .DstWin = {0, 0, FHD_W, FHD_H}
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
#if defined(CONFIG_LINUX)
            /* Pure Linux */
            .Chan = 0U, .pCvObj = &SvcCvFlow_RemoteAppObj,
            .CvModeID = REMOTE_APP_FLEXI_IO_OSDBUF,
#else
            /* Dual-OS */
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_LINUX_OSDBUF,
#endif
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
    }
#endif
}
};

#endif /* RES_X_X_H */
