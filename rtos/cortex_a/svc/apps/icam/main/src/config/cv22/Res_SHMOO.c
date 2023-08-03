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

// For Sensor in
#define INPUT_WIDTH    (1920U)
#define INPUT_HEIGHT   (1080U)
#define BITSCMP_WIDTH  (1920U)
#define BITSCMP_HEIGHT (1080U)

// For Memory in
#define Y2Y_BITSCMP_WIDTH  (2560U)
#define Y2Y_BITSCMP_HEIGHT (1600U)


static const SVC_RES_CFG_s g_ResCfg[] = {
{
    // RecMode = '$[REC_1][NUM_1][RECID_0_1]#!'
    // CvMode  = '$[CV_0][NUM_0]#!'
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
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = INPUT_WIDTH, .Height = INPUT_HEIGHT},
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
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [1] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
    },
    /* display stream info */
    .DispNum = 0U, .DispBits = 0x0U,
    .DispStrm = {
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
    // RecMode = '$[REC_0][NUM_0]#!'
    // CvMode  = '$[CV_1][NUM_1]#!'
    .FormatName = "(NO VIN) CV FileIn YUV",
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
            .PyramidBits = 0U, .HierWin = {0, 0},
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 0U, .DispBits = 0x0U,
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
    .StillCfg.EnableStill = 1,
},

{
    // RecMode = '$[REC_1][NUM_1][RECID_0_1]#!'
    // CvMode  = '$[CV_0][NUM_0]#!'
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

{
    // RecMode = '$[REC_1][NUM_1][RECID_0_1]#!'
    // CvMode  = '$[CV_1][NUM_1]#!'

    .FormatName = "(VIN0) IMX577 BITSCMP 2*3MP30 + CV Mode",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0][FOV_1]]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = INPUT_WIDTH, .Height = INPUT_HEIGHT},
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
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [1] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
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
                        .FovId = 0U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, 1920, 1080}
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
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
},

{
    // RecMode = '$[REC_1][NUM_1][RECID_0_1]#!'
    // CvMode  = '$[CV_1][NUM_1]#!'
    .FormatName = "Y2Y BITSCMP 2*2MP30 + CV Mode",
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

    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },

},

{
    // RecMode = '$[REC_0][NUM_0]#!'
    // CvMode  = '$[CV_1][NUM_2]#!'
    .FormatName = "(NO VIN) CV FileIn YUV * 2",
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
            .PyramidBits = 0U, .HierWin = {0, 0},
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 0U, .DispBits = 0x0U,
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
},

#if 0
{
    // RecMode = '$$[REC_1][NUM_2][RECID_0_1][RECID_2_3]#!'
    // CvMode  = '$[CV_0][NUM_0]#!'
    .FormatName = "(VIN0/1) IMX577 BITSCMP 2*3MP30 * 2",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0][FOV_1]#"\
        "$[VIN_1][S_IMX577_0][FOV_2][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = INPUT_WIDTH, .Height = INPUT_HEIGHT},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
        [1] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = INPUT_WIDTH, .Height = INPUT_HEIGHT},
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
        },
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = IMX577_3840_2160_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX577,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [1] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [2] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [3] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
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
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 4U, .RecBits = 0xfU,
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
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .MaxWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .MaxWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}
                    },
                }
            }
        },
    },
},
#endif

{

    // RecMode = '$$[REC_1][NUM_2][RECID_0_1][RECID_0_1_2]#!'
    // CvMode  = '$[CV_0][NUM_0]#!'
    .FormatName = "(VIN0) IMX577 BITSCMP 3*3MP30 ",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0][FOV_1][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = INPUT_WIDTH, .Height = INPUT_HEIGHT},
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
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [1] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [2] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
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
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x7U,
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
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .MaxWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}
                    },
                }
            }
        },
    },
},

{
    // RecMode = '$[REC_1][NUM_2][RECID_0_1][RECID_2_3]#!' or //  RecMode = '$[REC_1][NUM_1][RECID_0_1_2_3]#!'
    // CvMode  = '$[CV_1][NUM_2]#!'

    .FormatName = "(VIN0) IMX577 BITSCMP 4*3MP30 + CV FileIn YUV * 2 ",
    .VinTree = {
        "$[VIN_0][S_IMX577_0][FOV_0][FOV_1][FOV_2][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX577Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = INPUT_WIDTH, .Height = INPUT_HEIGHT},
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
        },
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [1] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [2] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .ActWin = {0},
            .MainWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT},
            .PyramidBits = 0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, }
        },
        [3] = {
            .RawWin = {(UINT16)((INPUT_WIDTH - BITSCMP_WIDTH) / 2U), (UINT16)((INPUT_HEIGHT - BITSCMP_HEIGHT) / 2U), BITSCMP_WIDTH, BITSCMP_HEIGHT},
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
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 4U, .RecBits = 0xfU,
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
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .MaxWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 3, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .MaxWin = {BITSCMP_WIDTH, BITSCMP_HEIGHT}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}, .DstWin = {0, 0, BITSCMP_WIDTH, BITSCMP_HEIGHT}
                    },
                }
            }
        },
    },
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefODObj,
            .CvModeID = REFOD_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_EXT_DATA, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {0, 0, 0, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 0U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_OD,
        },
    },
},

};

#endif /* RES_X_X_H */
