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
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_RefSeg.h"
#include "SvcCvFlow_Rebel_RefOD.h"
#include "SvcCvFlow_Rebel_RefSeg.h"
#include "SvcCvAppDef.h"
#endif

static const SVC_RES_CFG_s g_ResCfg[] = {
{   /*LT6911_3840_2160_A30P*/
    .FormatName = "(VIN0) LT6911 3840x1920 (4K CapWin crop 3840x1920) Rebel OD + Seg(TV HDMI 1080p30) VOUT sync 10FPS",
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
                .UpdateFrameRate = {.Interlace = 0U, .TimeScale = 10000U, .NumUnitsInTick = 1000U}
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
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REBEL_FDAG_LINUX_OPENOD,
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
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
                /* FeederFlag[0:7]: DivisorMode(0x2), ExtSync(0x4). FeederFlag[8:15]: Divisor = 0x3 */
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x306U,
#else
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0U,
#endif
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
            .PrivFlag = 0U, /* Linux CH0 */
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefSegObj,
            .CvModeID = REBEL_FDAG_LINUX_OPENSEG,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 180, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_SEG,
            .PrivFlag = 1U, /* Linux CH1 */
        },
    }
#endif
},
{   /*LT6911_3840_2160_A30P*/
    .FormatName = "(VIN0) LT6911 3840x1920 (4K CapWin crop 3840x1920) Rebel OD + Seg(TV HDMI 1080p60) VOUT sync 10FPS",
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
                .UpdateFrameRate = {.Interlace = 0U, .TimeScale = 10000U, .NumUnitsInTick = 1000U}
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
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REBEL_FDAG_LINUX_OPENOD,
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
#if defined (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE) && (CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE == 0x2)
                /* FeederFlag[0:7]: DivisorMode(0x2), ExtSync(0x4). FeederFlag[8:15]: Divisor = 0x3 */
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x306U,
#else
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0U,
#endif
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
            .PrivFlag = 0U, /* Linux CH0 */
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefSegObj,
            .CvModeID = REBEL_FDAG_LINUX_OPENSEG,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 180, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_SEG,
            .PrivFlag = 1U, /* Linux CH1 */
        },
    }
#endif
},
{
    .FormatName = "DEC-mode (3840x1920p30) Rebel OD + Seg (Pyramid=0x3F)",
    .VinTree = {
        "$[DEC_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 0U,
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840U, 1920U},
            .ActWin = {0},
            .MainWin = {3840U, 1920U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = {.MaxWarpDma = 24U,}
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
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .GuiCfg = {
                .Attribute =  SVC_GUI_ATTR_CUSTOM_RATE,
                .UpdateFrameRate = {.Interlace = 0U, .TimeScale = 10000U, .NumUnitsInTick = 1000U}
            },
        },
    },
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REBEL_FDAG_LINUX_OPENOD,
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
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x306U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
            .PrivFlag = 0U, /* Linux CH0 */
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefSegObj,
            .CvModeID = REBEL_FDAG_LINUX_OPENSEG,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 3840, .FrameHeight = 1920, .NumRoi = 1U,
                        .Roi = {
                            [0] = {3, 40, 180, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0x0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_SEG,
            .PrivFlag = 1U, /* Linux CH1 */
        },
    }
#endif
},
};

#endif /* RES_X_X_H */
