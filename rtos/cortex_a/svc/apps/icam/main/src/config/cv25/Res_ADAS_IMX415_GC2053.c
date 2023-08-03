#include "AmbaVIN.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX415.h"
#include "AmbaSensor_GC2053.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD_T30P61.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaCVBS.h"
#include "AmbaFPD_CVBS.h"
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
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "../../cardv/app_helper.h"
#endif

#define GRP_FMID_SWITCH     "FMID_SW"

#define IMX415_MAIN_W (3840U)
#define IMX415_MAIN_H (2160U)

#define GC2053_MAIN_W (1280U)
#define GC2053_MAIN_H (720U)

static const SVC_RES_CFG_s g_ResCfg_Imx415_Gc2053[] = {
    {
        .GroupName = GRP_FMID_SWITCH,
        .FormatName = "(VIN0) IMX415 3840x2160 \n\
                       (VIN1) GC2053 1280x720",
        .VinTree = {
            "$[VIN_0][S_IMX415_0][FOV_0]#\
             $[VIN_1][S_GC2053_0][FOV_1]#!"
        },
        /* vin info */
        .VinNum = 2U,
        .VinCfg = {
            [0] = {
                .pDriver = &AmbaSensor_IMX415Obj,
                .SerdesType = SVC_RES_SERDES_TYPE_NONE,
                .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = IMX415_MAIN_W, .Height = IMX415_MAIN_H},
                .TimeoutCfg = {5000U, 100U},
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U},
            },
            [1] = {
                .pDriver = &AmbaSensor_GC2053Obj,
                .SerdesType = SVC_RES_SERDES_TYPE_NONE,
                .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = GC2053_MAIN_W, .Height = GC2053_MAIN_H},
                .TimeoutCfg = {5000U, 100U},
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U},
            },
        },
        .SensorCfg = {
            [0] = { /* sensor on VIN_0 */
                [0] = {
                    .SensorMode = IMX415_3840_2160_30P,
                    .SensorGroup = 0,
                    .IQTable = IQ_SSR_IMX415,
                },   /* sensor idx 0 */
            },
            [1] = { /* sensor on VIN_1 */
                [0] = {
                    .SensorMode = GC2053_1920_1080_30P,
                    .SensorGroup = 0,
                    .IQTable = IQ_SSR_GC2053,
                },   /* sensor idx 1 */
            }
        },
        /* fov info */
        .FovNum = 2U,
        .FovCfg = {
            [0] = {
                .RawWin = {0, 0, IMX415_MAIN_W, IMX415_MAIN_H},
                .ActWin = {0},
                .MainWin = {IMX415_MAIN_W, IMX415_MAIN_H},
                .PyramidBits = 0x00, .HierWin = {0U, 0U},
                .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
            },
            [1] = {
                .RawWin = {0U, 0U, GC2053_MAIN_W, GC2053_MAIN_H},
                .ActWin = {0U},
                .MainWin = {GC2053_MAIN_W, GC2053_MAIN_H},
                .PyramidBits = 0x00, .HierWin = {0x0, 0x0},
                .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
            },
        },
#if 0
        /* display stream info */
        .DispNum = 1U, .DispBits = 0x01U,
        .DispStrm = {
            [0] = {
                .VoutID = 0,
                .pDriver = &AmbaFPD_T30P61Obj,
                .StrmCfg = {
                    .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 0U, .SrcWin = {0, 0, 1280U, 720U}, .DstWin = {0, 0, 960U, 240U}
                        },
                    }
                },
                .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
            }
        },
#endif
        /* record stream info */
        .RecNum = 3U, .RecBits = 0x07U,
        .RecStrm = {
            [0] = {
                .RecSetting = {
                    .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                    .DestBits = SVC_REC_DST_FILE,
                    .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1, .BitRate = 0U,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
                },
                .StrmCfg = {
                    .Win = {3840, 2160}, .MaxWin = {3840, 2160}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 0U, .SrcWin = {0, 0, IMX415_MAIN_W, IMX415_MAIN_H}, .DstWin = {0, 0, 3840, 2160}
                        },
                    }
                }
            },
            [1] = {
                .RecSetting = {
                    .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                    .DestBits = SVC_REC_DST_NET,
                    .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1, .BitRate = 0U,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
                },
                .StrmCfg = {
                    .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 0U, .SrcWin = {0, 0, IMX415_MAIN_W, IMX415_MAIN_H}, .DstWin = {0, 0, 1280U, 720U}
                        },
                    }
                }
            },
            [2] = {
                .RecSetting = {
                    .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                    .DestBits = SVC_REC_DST_FILE | SVC_REC_DST_NET,
                    .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1, .BitRate = 0U,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
                },
                .StrmCfg = {
                    .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 1U, .SrcWin = {0, 0, GC2053_MAIN_W, GC2053_MAIN_H}, .DstWin = {0, 0, 1280U, 720U}
                        },
                    }
                }
            },
            [3] = {
                .RecSetting = {
                    .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                    .DestBits = SVC_REC_DST_NET,
                    .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1, .BitRate = 0U,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
                },
                .StrmCfg = {
                    .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 1U, .SrcWin = {0, 0, GC2053_MAIN_W, GC2053_MAIN_H}, .DstWin = {0, 0, 1280U, 720U}
                        },
                    }
                }
            },
        },
        .StillCfg.EnableStill = 1,
#if defined(CONFIG_BUILD_CV)
        .CvFlowNum = 0, .CvFlowBits = 0x00U,
        .CvFlow = {
            [0] = {
                .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
                .CvModeID = REFOD_FDAG_LINUX_SSD,
                .PrivFlag = 0U,
                .InputCfg = {
                    .InputNum = 1U,
                    .Input = {
                        [0] = {
                            .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                            .FrameWidth = IMX415_MAIN_W, .FrameHeight = IMX415_MAIN_H, .NumRoi = 0U
                        },
                    },
                    .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
                },
                .CvFlowType = SVC_CV_FLOW_REF_OD,
            },
            [1] = {
                .Chan = 1U, .pCvObj = &SvcCvFlow_RefODObj,
                .CvModeID = REFOD_FDAG_LINUX_SSD,
                .PrivFlag = 1U,
                .InputCfg = {
                    .InputNum = 1U,
                    .Input = {
                        [0] = {
                            .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 1U,   /* input is FOV_1 */
                            .FrameWidth = 1280U, .FrameHeight = 720U, .NumRoi = 0U
                        },
                    },
                    .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
                },
                .CvFlowType = SVC_CV_FLOW_REF_OD,
            },
        }
#endif
    }
};

static const SVC_RES_CFG_s g_ResCfg_Imx415[] = {
    {
        .GroupName = GRP_FMID_SWITCH,
        .FormatName = "(VIN0) IMX415 3840x2160",
        .VinTree = {
            "$[VIN_0][S_IMX415_0][FOV_0]#!"
        },
        /* vin info */
        .VinNum = 1U,
        .VinCfg = {
            [0] = {
                .pDriver = &AmbaSensor_IMX415Obj,
                .SerdesType = SVC_RES_SERDES_TYPE_NONE,
                .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = IMX415_MAIN_W, .Height = IMX415_MAIN_H},
                .TimeoutCfg = {5000U, 100U},
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U},
            },
        },
        .SensorCfg = {
            [0] = { /* sensor on VIN_0 */
                [0] = {
                    .SensorMode = IMX415_3840_2160_30P,
                    .SensorGroup = 0,
                    .IQTable = IQ_SSR_IMX415,
                },   /* sensor idx 0 */
            }
        },
        /* fov info */
        .FovNum = 1U,
        .FovCfg = {
            [0] = {
                .RawWin = {0, 0, IMX415_MAIN_W, IMX415_MAIN_H},
                .ActWin = {0},
                .MainWin = {IMX415_MAIN_W, IMX415_MAIN_H},
                .PyramidBits = 0x00, .HierWin = {0U, 0U},
                .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
            },
        },
#if 0
        /* display stream info */
        .DispNum = 1U, .DispBits = 0x01U,
        .DispStrm = {
            [0] = {
                .VoutID = 0,
                .pDriver = &AmbaFPD_T30P61Obj,
                .StrmCfg = {
                    .Win = {960U, 240U}, .MaxWin = {960U, 240U}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 0U, .SrcWin = {0, 0, 1280U, 720U}, .DstWin = {0, 0, 960U, 240U}
                        },
                    }
                },
                .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
            }
        },
#endif
        /* record stream info */
        .RecNum = 2U, .RecBits = 0x03U,
        .RecStrm = {
            [0] = {
                .RecSetting = {
                    .SrcBits = (SVC_REC_SRC_VIDEO),
                    .DestBits = SVC_REC_DST_FILE,
                    .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1, .BitRate = 0U,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
                },
                .StrmCfg = {
                    .Win = {3840U, 2160U}, .MaxWin = {3840U, 2160U}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 0U, .SrcWin = {0, 0, IMX415_MAIN_W, IMX415_MAIN_H}, .DstWin = {0, 0, 3840U, 2160U}
                        },
                    }
                }
            },
            [1] = {
                .RecSetting = {
                    .SrcBits = (SVC_REC_SRC_VIDEO),
                    .DestBits = SVC_REC_DST_NET,
                    .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1, .BitRate = 0U,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
                },
                .StrmCfg = {
                    .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 0U, .SrcWin = {0, 0, IMX415_MAIN_W, IMX415_MAIN_H}, .DstWin = {0, 0, 1280U, 720U}
                        },
                    }
                }
            },
        },
        .StillCfg.EnableStill = 1,
#if defined(CONFIG_BUILD_CV)
        .CvFlowNum = 0, .CvFlowBits = 0x0U,
        .CvFlow = {
            [0] = {
                .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
                .CvModeID = REFOD_FDAG_LINUX_SSD,
                .InputCfg = {
                    .InputNum = 1U,
                    .Input = {
                        [0] = {
                            .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,	/* input is FOV_0 */
                            .FrameWidth = IMX415_MAIN_W, .FrameHeight = IMX415_MAIN_H, .NumRoi = 0U
                        },
                    },
                    .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
                },
                .CvFlowType = SVC_CV_FLOW_REF_OD,
            },
        }
#endif
    }
};

static const SVC_RES_CFG_s g_ResCfg_Gc2053[] = {
    {
        .GroupName = GRP_FMID_SWITCH,
        .FormatName = "(VIN1) GC2053 1920x1080",
        .VinTree = {
            "$[VIN_1][S_GC2053_0][FOV_0]#!"
        },
        /* vin info */
        .VinNum = 1U,
        .VinCfg = {
            [1] = {
                .pDriver = &AmbaSensor_GC2053Obj,
                .SerdesType = SVC_RES_SERDES_TYPE_NONE,
                .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 1080U},
                .TimeoutCfg = {5000U, 100U},
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U},
            },
        },
        .SensorCfg = {
            [1] = { /* sensor on VIN_0 */
                [0] = {
                    .SensorMode = GC2053_1920_1080_30P,
                    .SensorGroup = 0,
                    .IQTable = IQ_SSR_GC2053,
                },   /* sensor idx 0 */
            }
        },
        /* fov info */
        .FovNum = 1U,
        .FovCfg = {
            [0] = {
                .RawWin = {0, 0, 1920U, 1080U},
                .ActWin = {0},
                .MainWin = {1920U, 1080U},
                .PyramidBits = 0x00, .HierWin = {0U, 0U},
                .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
            },
        },
        /* record stream info */
        .RecNum = 2U, .RecBits = 0x03U,
        .RecStrm = {
            [0] = {
                .RecSetting = {
                    .SrcBits = (SVC_REC_SRC_VIDEO),
                    .DestBits = SVC_REC_DST_FILE,
                    .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1, .BitRate = 0U,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
                },
                .StrmCfg = {
                    .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0, 0, 1280U, 720U}
                        },
                    }
                }
            },
            [1] = {
                .RecSetting = {
                    .SrcBits = (SVC_REC_SRC_VIDEO),
                    .DestBits = SVC_REC_DST_NET,
                    .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1, .BitRate = 0U,
                    .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
                },
                .StrmCfg = {
                    .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                    .ChanCfg = {
                        [0] = {
                            .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0, 0, 1280U, 720U}
                        },
                    }
                }
            },
        },
        .StillCfg.EnableStill = 1,
#if 0//defined(CONFIG_BUILD_CV)
        .CvFlowNum = 0, .CvFlowBits = 0x0U,
        .CvFlow = {
            [0] = {
                .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
                .CvModeID = REFOD_FDAG_LINUX_SSD,
                .InputCfg = {
                    .InputNum = 1U,
                    .Input = {
                        [0] = {
                            .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,	/* input is FOV_0 */
                            .FrameWidth = 3840U, .FrameHeight = 2160U, .NumRoi = 0U
                        },
                    },
                    .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
                },
                .CvFlowType = SVC_CV_FLOW_REF_OD,
            },
        }
#endif
    }
};

static SVC_RES_CFG_s g_ResCfg[1];
static void g_RefCfgPreInit(void)
{
    if (app_helper.internal_camera_enabled && app_helper.external_camera_enabled == 0) {
        AmbaWrap_memcpy(&g_ResCfg[0], &g_ResCfg_Imx415[0], sizeof(SVC_RES_CFG_s));
    } else if (app_helper.internal_camera_enabled == 0 && app_helper.external_camera_enabled) {
        AmbaWrap_memcpy(&g_ResCfg[0], &g_ResCfg_Gc2053[0], sizeof(SVC_RES_CFG_s));
    } else {
        AmbaWrap_memcpy(&g_ResCfg[0], &g_ResCfg_Imx415_Gc2053[0], sizeof(SVC_RES_CFG_s));
    }
}

