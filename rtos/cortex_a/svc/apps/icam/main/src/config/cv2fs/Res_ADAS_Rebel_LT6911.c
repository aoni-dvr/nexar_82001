/**
 *  @file Res_ADAS_LT6911.c
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

#include "AmbaYuv.h"
#include "AmbaYuv_LT6911.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaFPD_LT9611UXC.h"
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

#ifdef CONFIG_ICAM_VOUTB_OSD_BUF_FHD
#define INSTANCE_MASK_OSD_BASE              (((UINT32)1806U << 16U) | 900U)
#define INSTANCE_MASK_OSD_BASE_OD896X512    (((UINT32)1792U << 16U) | 1024U)
#elif defined CONFIG_ICAM_VOUTB_OSD_BUF_HD
#define INSTANCE_MASK_OSD_BASE              (((UINT32)1204U << 16U) | 604U)
#define INSTANCE_MASK_OSD_BASE_OD896X512    (((UINT32)1194U << 16U) | 682U)
#else
#define INSTANCE_MASK_OSD_BASE              (0U)
#define INSTANCE_MASK_OSD_BASE_OD896X512    (0U)
#endif

static const SVC_RES_CFG_s g_ResCfg[] = {
{   /*LT6911_3840_2160_A30P*/
    .FormatName = "(VIN0) LT6911 3840x1920 (crop 3840x1920) Simple OpenOD + OpenSeg (Pyramid=0x3F)",
    .VinTree = {
        "$[VIN_0][Y_LT6911_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaYuv_LT6911Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 120U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .FixedFrameRate = 1,
        },
    },
    /* sensor info */
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = LT6911_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_DUMMY,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 1920},
            .ActWin = {0},
            .MainWin = {3840, 1920},
            .PyramidBits = 0xF, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 3840U, 1920U}
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
            .CvModeID = REFOD_FDAG_LINUX_OPENOD,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 16, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
            .PrivFlag = 0U, /* Linux CH0 */
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefSegObj,
            .CvModeID = REFSEG_FDAG_LINUX_OPENSEG,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 42, 180, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_SEG,
            .PrivFlag = 1U, /* Linux CH1 */
        },
    }
#endif
},
{   /*LT6911_3840_2160_A30P*/
    .FormatName = "(VIN0) LT6911 3840x1920(crop 3840x1920)AmbaPcpt(2 Scale OD 896x512,KP,3D,Mask,CLF)+AmbaSeg, 1080P30VOUT sync 7.5FPS",
    .VinTree = {
        "$[VIN_0][Y_LT6911_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaYuv_LT6911Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 120U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .FixedFrameRate = 1,
        },
    },
    /* sensor info */
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = LT6911_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_DUMMY,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 1920},
            .ActWin = {0},
            .MainWin = {3840, 1920},
            .PyramidBits = 0x3F, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
            .GuiCfg = {
                .Attribute =  SVC_GUI_ATTR_CUSTOM_RATE,
                .UpdateFrameRate = {.Interlace = 0U, .TimeScale = 8000U, .NumUnitsInTick = 1000U}
            },
#endif
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 3840U, 1920U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_PCPT_OD_RES1_2SC_DET_CLF,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 2U,
                        .Roi = {
                            [0] = {4, 32, 0, 0, 0},
                            [1] = {0, 1472, 628, 0, 0},
                        }
                    },
                },
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
                /* FeederFlag[0:7]: DivisorMode(0x2), ExtSync(0x4). FeederFlag[8:15]: Divisor = 0x3 */
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x406U,
#else
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0U,
#endif
            },
            .Config = {0, INSTANCE_MASK_OSD_BASE_OD896X512, 0, 0},
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
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
{   /*LT6911_3840_2160_A30P*/
    .FormatName = "(VIN0) LT6911 3840x1920(crop 3840x1920)AmbaPcpt(2 Scale OD 896x512,KP,Non-3D,Mask,CLF)+AmbaSeg, 1080P30VOUT sync 7.5FPS",
    .VinTree = {
        "$[VIN_0][Y_LT6911_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaYuv_LT6911Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 120U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .FixedFrameRate = 1,
        },
    },
    /* sensor info */
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = LT6911_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_DUMMY,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 1920},
            .ActWin = {0},
            .MainWin = {3840, 1920},
            .PyramidBits = 0x3F, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
            .GuiCfg = {
                .Attribute =  SVC_GUI_ATTR_CUSTOM_RATE,
                .UpdateFrameRate = {.Interlace = 0U, .TimeScale = 8000U, .NumUnitsInTick = 1000U}
            },
#endif
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 3840U, 1920U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_PCPT_OD_RES1_2SC_DET_CLF,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 2U,
                        .Roi = {
                            [0] = {4, 32, 0, 0, 0},
                            [1] = {0, 1472, 628, 0, 0},
                        }
                    },
                },
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
                /* FeederFlag[0:7]: DivisorMode(0x2), ExtSync(0x4). FeederFlag[8:15]: Divisor = 0x3 */
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x406U,
#else
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0U,
#endif
            },
            .Config = {0x37B, INSTANCE_MASK_OSD_BASE_OD896X512, 0, 0},
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
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
{   /*LT6911_3840_2160_A30P*/
    .FormatName = "(VIN0) LT6911 3840x1920(crop 3840x1920)AmbaPcpt(2 Scale OD 896x512,KP,3D,Mask,CLF)+AmbaSeg, 1080P60VOUT sync 7.5FPS",
    .VinTree = {
        "$[VIN_0][Y_LT6911_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaYuv_LT6911Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 120U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .FixedFrameRate = 1,
        },
    },
    /* sensor info */
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = LT6911_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_DUMMY,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 1920},
            .ActWin = {0},
            .MainWin = {3840, 1920},
            .PyramidBits = 0x3F, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
            .GuiCfg = {
                .Attribute =  SVC_GUI_ATTR_CUSTOM_RATE,
                .UpdateFrameRate = {.Interlace = 0U, .TimeScale = 8000U, .NumUnitsInTick = 1000U}
            },
#endif
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 3840U, 1920U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_PCPT_OD_RES1_2SC_DET_CLF,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 2U,
                        .Roi = {
                            [0] = {4, 32, 0, 0, 0},
                            [1] = {0, 1472, 628, 0, 0},
                        }
                    },
                },
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
                /* FeederFlag[0:7]: DivisorMode(0x2), ExtSync(0x4). FeederFlag[8:15]: Divisor = 0x3 */
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x406U,
#else
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0U,
#endif
            },
            .Config = {0, INSTANCE_MASK_OSD_BASE_OD896X512, 0, 0},
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
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
{   /*LT6911_3840_2160_A30P*/
    .FormatName = "(VIN0) LT6911 3840x1920(crop 3840x1920)AmbaPcpt(2 Scale OD 896x512,KP,Non-3D,Mask,CLF)+AmbaSeg, 1080P60VOUT sync 7.5FPS",
    .VinTree = {
        "$[VIN_0][Y_LT6911_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaYuv_LT6911Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 120U, .Width = 3840U, .Height = 1920U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .FixedFrameRate = 1,
        },
    },
    /* sensor info */
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = LT6911_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_DUMMY,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 1920},
            .ActWin = {0},
            .MainWin = {3840, 1920},
            .PyramidBits = 0x3F, .HierWin = {0, 0},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 960U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 1920U, 960U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
            .GuiCfg = {
                .Attribute =  SVC_GUI_ATTR_CUSTOM_RATE,
                .UpdateFrameRate = {.Interlace = 0U, .TimeScale = 8000U, .NumUnitsInTick = 1000U}
            },
#endif
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840U, 1920U}, .MaxWin = {3840U, 1920U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 3840U, 1920U}, .DstWin = {0U, 0U, 3840U, 1920U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 0,
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_PCPT_OD_RES1_2SC_DET_CLF,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 2U,
                        .Roi = {
                            [0] = {4, 32, 0, 0, 0},
                            [1] = {0, 1472, 628, 0, 0},
                        }
                    },
                },
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
                /* FeederFlag[0:7]: DivisorMode(0x2), ExtSync(0x4). FeederFlag[8:15]: Divisor = 0x3 */
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x406U,
#else
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0U,
#endif
            },
            .Config = {0x37B, INSTANCE_MASK_OSD_BASE_OD896X512, 0, 0},
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
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    }
#endif
},
};

#endif /* RES_X_X_H */
