/**
 *  @file Res_ADAS_OV48C40.c
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
#include "AmbaSensor_OV48C40.h"
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

#include ".svc_autogen"

#define GRP_FMID_SWITCH     "FMID_SW"

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 8KP24",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 7680, .Height = 4320},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 24, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            /* sensor idx 0 */
            [0] = {
                .SensorMode = OV48C40_7680_4320_24P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
            },
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
            .PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, .WarpLumaWaitLine = 64U, .MaxHorWarpCompensation = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {754, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 754, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 24, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 24U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 24, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 24U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 24, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 24U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = SVC_CLK_DEF_CORE,
        .IdspClk = SVC_CLK_DEF_IDSP,
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 4KP120",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840, .Height = 2160},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 120, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            /* sensor idx 0 */
            [0] = {
                .SensorMode = OV48C40_3840_2160_120P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
            },
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 2160},
            .ActWin = {0},
            .MainWin = {3840, 2160},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, .MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {754, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 754, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 120, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 120U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = SVC_CLK_DEF_CORE,
        .IdspClk = SVC_CLK_DEF_IDSP,
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 4KP60 (4x3)",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 4032, .Height = 3024},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_4032_3024_60P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 4032, 3024},
            .ActWin = {0},
            .MainWin = {3840, 2880},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, .MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {564, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 564, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840, 2880}, .MaxWin = {3840, 2880}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 3840, 2880}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 960}, .MaxWin = {1280, 960}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 1280, 960}
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
                .Win = {1920, 1440}, .MaxWin = {1920, 1440}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 1920, 1440}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = 480000000,
        .IdspClk = 600000000,
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 4KP60 2FOV EIS (Performance check)",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 4032, .Height = 3024},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_4032_3024_60P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 960, 1008},
            .ActWin = {0},
            .MainWin = {960, 720},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .MctfDisable=MCTF_BITS_ON_CMPR, .MctsDisable=(MCTS_BITS_OFF_DRAMOUT|MCTS_BITS_OFF), .MaxHorWarpCompensation = 64U,},
            .VirtChan = { (SVC_RES_PIPE_VIRTUAL_CHAN |SVC_RES_PIPE_VIRTUAL_CHAN_VIN_DECI), 0x2U},
        },
        [1] = {
            .RawWin = {0, 0, 3968, 2208},
            .ActWin = {0},
            .MainWin = {3840, 2160},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .MctfDisable=MCTF_BITS_ON_CMPR, .MctsDisable=(MCTS_BITS_OFF_DRAMOUT|MCTS_BITS_OFF), .MaxHorWarpCompensation = 62U},
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {754, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 960, 720}, .DstWin = {0, 0, 754, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840, 2160}, .MaxWin = {3840, 2160}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 3840, 2160}
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
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 1280, 720}
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
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 960, 720}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = 264000000,
        .IdspClk = 480000000,
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 4KP30 CV",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 380U, .Width = 3840, .Height = 2160},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_4032_3024_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
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
            .PyramidBits = 0x8U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, .MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {754, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 754, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 1280, 720}
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
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = 168000000,
        .IdspClk = 480000000,
    },
#endif
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
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 4KP30 (4x3)",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 4032, .Height = 3024},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_4032_3024_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 4032, 3024},
            .ActWin = {0},
            .MainWin = {3840, 2880},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, .MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {564, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 564, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840, 2880}, .MaxWin = {3840, 2880}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 3840, 2880}
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
                .Win = {1280, 960}, .MaxWin = {1280, 960}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 1280, 960}
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
                .Win = {1920, 1440}, .MaxWin = {1920, 1440}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 1920, 1440}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = 240000000,
        .IdspClk = 480000000,
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 1080P240 (4x3)",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 16U, .OffsetY = 0U, .Width = 1984, .Height = 1440},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 240, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_2016_1512_240P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1984, 1440},
            .ActWin = {0},
            .MainWin = {1920, 1440},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, .MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {564, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1440}, .DstWin = {0, 0, 564, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 120, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 120U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1440}, .MaxWin = {1920, 1440}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1440}, .DstWin = {0, 0, 1920, 1440}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 960}, .MaxWin = {1280, 960}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1440}, .DstWin = {0, 0, 1280, 960}
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
                .Win = {1920, 1440}, .MaxWin = {1920, 1440}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1440}, .DstWin = {0, 0, 1920, 1440}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = SVC_CLK_DEF_CORE,
        .IdspClk = SVC_CLK_DEF_IDSP,
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 1080P60 (4x3)",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 16U, .OffsetY = 0U, .Width = 1984, .Height = 1512},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_2016_1512_60P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1984, 1512},
            .ActWin = {0},
            .MainWin = {1920, 1440},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, .MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {564, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1440}, .DstWin = {0, 0, 564, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1440}, .MaxWin = {1920, 1440}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1440}, .DstWin = {0, 0, 1920, 1440}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 960}, .MaxWin = {1280, 960}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1440}, .DstWin = {0, 0, 1280, 960}
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
                .Win = {1920, 1440}, .MaxWin = {1920, 1440}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1440}, .DstWin = {0, 0, 1920, 1440}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = SVC_CLK_DEF_CORE,
        .IdspClk = SVC_CLK_DEF_IDSP,
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 1080P30",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920, .Height = 1080},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
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
            .PipeCfg = { .RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {754, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 754, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1280, 720}
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
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = 168000000,
        .IdspClk = 480000000,
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 1080P30 CV",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920, .Height = 1080},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
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
            .PyramidBits = 0x2U, .HierWin = {0U, 0U},
            .PipeCfg = { .RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {754, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 754, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1280, 720}
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
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_RefSegObj,
            .CvModeID = REFSEG_FDAG_OPEN_NET_1,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 40, 180, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 1U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_REF_SEG,
        },
    },
#endif
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = 168000000,
        .IdspClk = 480000000,
    },
#endif
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 4KP30 (4x3) DCG HDR",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 32U, .OffsetY = 24U, .Width = 3968, .Height = 2976},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_4032_3024_30P_DCG_HDR,
                .SensorGroup = 0,
                .IQTable =  (IQ_SSR_OV48C40| (IMG_SENSOR_HDR_MODE_0 << 16)),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3968, 2976},
            .ActWin = {0},
            .MainWin = {3840, 2880},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, .LinearCE=1U},
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {564, 424}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 564, 424}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {3840, 2880}, .MaxWin = {3840, 2880}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 3840, 2880}
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
                .Win = {1280, 960}, .MaxWin = {1280, 960}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 1280, 960}
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
                .Win = {1920, 1440}, .MaxWin = {1920, 1440}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2880}, .DstWin = {0, 0, 1920, 1440}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = SVC_CLK_DEF_CORE,
        .IdspClk = SVC_CLK_DEF_IDSP,
    },
#endif
},
{
    .FormatName = "(VIN0) OV48C40 1080P30 - STL CAP (3840x2160)",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920, .Height = 1080},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
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
            .PipeCfg = {0}
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
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
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
            [0] = {
                .MaxRaw = {.Compressed = 0, .CeNeeded = 0, .FrameNum = 3, .Width = 3840, .Height = 2160,
                           .EffectW = 3840, .EffectH = 2160, .SensorMode = OV48C40_3840_2160_30P},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 3840, 2160, 0, 0, 0},
                },
            },
        }
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = SVC_CLK_DEF_CORE,
        .IdspClk = SVC_CLK_DEF_IDSP,
    },
#endif
},
{
    .FormatName = "(VIN0) OV48C40 1080P30 - Timelapse",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920, .Height = 1080},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_OV48C40,
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
            .PipeCfg = {.MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, }
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
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },

    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1, .TimeLapse = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1, .TimeLapse = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1280, 720}
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
                .MaxRaw = {.Compressed = 0, .CeNeeded = 0, .FrameNum = 3, .Width = 3840, .Height = 2160,
                           .EffectW = 3840, .EffectH = 2160, .SensorMode = OV48C40_3840_2160_120P},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 3840, 2160, 1, 1920, 1080},
                },
            },
        },
        .Qview = {
            .Enable = 1,
            .NumQview = 1,
            .Win = {
                [0] = { .VoutId = 1U, .Width = 1920, .Height = 1080 },
            },
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = SVC_CLK_DEF_CORE,
        .IdspClk = SVC_CLK_DEF_IDSP,
    },
#endif
},
{
    .FormatName = "(VIN0) OV48C40 8K DEC",
    .VinTree = {
        "$[DEC_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 0U,
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 7680, 4320},
            .ActWin = {0},
            .MainWin = {7680, 4320},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_OFF, .MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 7680, 4320}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
},
{
    .FormatName = "(VIN0) OV48C40 4K DEC",
    .VinTree = {
        "$[DEC_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 0U,
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 2160},
            .ActWin = {0},
            .MainWin = {3840, 2160},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = { .RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_OFF, .MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
},
{
    .GroupName = GRP_FMID_SWITCH,
    .FormatName = "(VIN0) OV48C40 1080P30 [AF]",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920, .Height = 1080},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = OV48C40_1920_1080_30P,
                .SensorGroup = 1 << 14,
                .IQTable = IQ_SSR_OV48C40,
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
            .PipeCfg = { .RawCompression = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, }
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
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
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
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1280, 720}
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
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = 168000000,
        .IdspClk = 480000000,
    },
#endif
},
{
    .FormatName = "(VIN0) OV48C40 4KP120 FOV-0 EIS, FOV-1 HDS",
    .VinTree = {
        "$[VIN_0][S_OV48C40_0][FOV_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_OV48C40Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 3840, .Height = 2160},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 120, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            /* sensor idx 0 */
            [0] = {
                .SensorMode = OV48C40_3840_2160_120P,
                .SensorGroup = 1 << 15,
                .IQTable = IQ_SSR_OV48C40,
            },
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 3840, 2160},
            .ActWin = {0},
            .MainWin = {3840, 2160},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_NORMAL, .MctfDisable = MCTF_BITS_ON_CMPR, .MaxHorWarpCompensation = 320U},
            .IQTable = (1U << 8U) /* eis enable */
        },
        [1] = {
            .RawWin = {0, 0, 960, 720},
            .ActWin = {0},
            .MainWin = {960, 544},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_NORMAL, .MctfDisable = MCTF_BITS_ON_CMPR, .MaxHorWarpCompensation = 32U},
            .VirtChan = {(SVC_RES_PIPE_VIRTUAL_CHAN_VIN_DECI | SVC_RES_PIPE_VIRTUAL_CHAN_INPUT_MUX_SEL), 0x4U, .InputMuxSelCfg = 0x40001003, .InputSubsample = (4U << 16U) | 3U },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {960U, 544U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 960, 544}, .DstWin = {0, 0, 960U, 544U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1280_720_A30P,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 120, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 120U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 120, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 120U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 720}, .MaxWin = {1280, 720}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 1280, 720}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 30, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 3840, 2160}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            }
        },
    },
#if defined(SVC_CLK_UPDATE_CORE)
    .ClkCfg = {
        .CoreClk = SVC_CLK_DEF_CORE,
        .IdspClk = SVC_CLK_DEF_IDSP,
    },
#endif
},
};
#endif
