/**
 *  @file Res_EMR_IMX183.c
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
#include "AmbaSensor_IMX183.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD_C101EAN01_0.h"
#include "AmbaHDMI_Def.h"

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) IMX183 (VIN1) IMX183(NO CV)",
    .VinTree = {
        "$[VIN_0][S_IMX183_0][FOV_0]#\
         $[VIN_1][S_IMX183_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX183Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 180U, .OffsetY = 40U, .Width = 5376U, .Height = 3648U},
            .FrameRate = {.TimeScale = 10, .NumUnitsInTick = 1},
        },
        [1] = {
            .pDriver = &AmbaSensor_IMX183Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 180U, .OffsetY = 40U, .Width = 5376U, .Height = 3648U},
            .FrameRate = {.TimeScale = 10, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {  /* sensor idx 0 */
                .SensorMode = IMX183_5472_3648_10P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_IMX183,
            },
        },
        [1] = { /* sensor on VIN_1 */
            [0] = {  /* sensor idx 0 */
                .SensorMode = IMX183_5472_3648_10P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_IMX183,
            },
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 5376U, 3648U},
            .ActWin = {0},
            .MainWin = {5376U, 3648U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
        [1] = {
            .RawWin = {0U, 0U, 5376U, 3648U},
            .ActWin = {0},
            .MainWin = {5376U, 3648U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_C101EAN01Obj,
            .StrmCfg = {
                .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 1280U, 720U}
                    },
                }
            },
            .DevMode = AMBA_FPD_C101EAN01_1280_720_A60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 10U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 10U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {5376U, 3648U}, .MaxWin = {5376U, 3648U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 5376U, 3648U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 10U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 10U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {5376U, 3648U}, .MaxWin = {5376U, 3648U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 5376U, 3648U}
                    },
                }
            }
        },
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1U,
},
{
    .FormatName = "(VIN0) IMX183(1080P CROP)",
    .VinTree = {
        "$[VIN_0][S_IMX183_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX183Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 1908U, .OffsetY = 1324U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 10, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {  /* sensor idx 0 */
                .SensorMode = IMX183_5472_3648_10P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_IMX183,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_C101EAN01Obj,
            .StrmCfg = {
                .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1280U, 720U}
                    },
                }
            },
            .DevMode = AMBA_FPD_C101EAN01_1280_720_A60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 10U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 10U, .NumUnitsInTick = 1U, .Interlace = 0U},
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
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1U,
},
{
    .FormatName = "(VIN0) IMX183 (VIN1) IMX183(TIMELAPSE)",
    .VinTree = {
        "$[VIN_0][S_IMX183_0][FOV_0]#\
         $[VIN_1][S_IMX183_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX183Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 180U, .OffsetY = 40U, .Width = 5376U, .Height = 3648U},
            .FrameRate = {.TimeScale = 10, .NumUnitsInTick = 1},
        },
        [1] = {
            .pDriver = &AmbaSensor_IMX183Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 180U, .OffsetY = 40U, .Width = 5376U, .Height = 3648U},
            .FrameRate = {.TimeScale = 10, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {  /* sensor idx 0 */
                .SensorMode = IMX183_5472_3648_10P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_IMX183,
            },
        },
        [1] = { /* sensor on VIN_1 */
            [0] = {  /* sensor idx 0 */
                .SensorMode = IMX183_5472_3648_10P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_IMX183,
            },
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 5376U, 3648U},
            .ActWin = {0},
            .MainWin = {5376U, 3648U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
        [1] = {
            .RawWin = {0U, 0U, 5376U, 3648U},
            .ActWin = {0},
            .MainWin = {5376U, 3648U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_C101EAN01Obj,
            .StrmCfg = {
                .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 1280U, 720U}
                    },
                }
            },
            .DevMode = AMBA_FPD_C101EAN01_1280_720_A60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 10U, .IdrInterval = 1U, .TimeLapse = 1U,
                .FrameRate = {.TimeScale = 10U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {5376U, 3648U}, .MaxWin = {5376U, 3648U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 5376U, 3648U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 10U, .IdrInterval = 1U, .TimeLapse = 1U,
                .FrameRate = {.TimeScale = 10U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {5376U, 3648U}, .MaxWin = {5376U, 3648U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 5376U, 3648U}
                    },
                }
            }
        },
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
{
    .FormatName = "(VIN0) IMX183(NO CV)",
    .VinTree = {
        "$[VIN_0][S_IMX183_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX183Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 180U, .OffsetY = 40U, .Width = 5376U, .Height = 3648U},
            .FrameRate = {.TimeScale = 10, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {  /* sensor idx 0 */
                .SensorMode = IMX183_5472_3648_10P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_IMX183,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 5376U, 3648U},
            .ActWin = {0},
            .MainWin = {5376U, 3648U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_C101EAN01Obj,
            .StrmCfg = {
                .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 1280U, 720U}
                    },
                }
            },
            .DevMode = AMBA_FPD_C101EAN01_1280_720_A60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_NET,
                .RecId = 0U, .M = 1U, .N = 10U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 10U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 1280U, 720U}
                    },
                }
            }
        },
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1U,
},
{
    .FormatName = "(VIN0) IMX183 (VIN1) IMX183(TV SBS/NO LCD)",
    .VinTree = {
        "$[VIN_0][S_IMX183_0][FOV_0]#\
         $[VIN_1][S_IMX183_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_IMX183Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 180U, .OffsetY = 40U, .Width = 5376U, .Height = 3648U},
            .FrameRate = {.TimeScale = 10, .NumUnitsInTick = 1},
        },
        [1] = {
            .pDriver = &AmbaSensor_IMX183Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 180U, .OffsetY = 40U, .Width = 5376U, .Height = 3648U},
            .FrameRate = {.TimeScale = 10, .NumUnitsInTick = 1},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {  /* sensor idx 0 */
                .SensorMode = IMX183_5472_3648_10P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_IMX183,
            },
        },
        [1] = { /* sensor on VIN_1 */
            [0] = {  /* sensor idx 0 */
                .SensorMode = IMX183_5472_3648_10P,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_IMX183,
            },
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 5376U, 3648U},
            .ActWin = {0},
            .MainWin = {5376U, 3648U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
        [1] = {
            .RawWin = {0U, 0U, 5376U, 3648U},
            .ActWin = {0},
            .MainWin = {5376U, 3648U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = {0}
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 960U, 1080U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {960U, 0U, 960U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A30P,
            .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 10U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 10U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {5376U, 3648U}, .MaxWin = {5376U, 3648U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 5376U, 3648U}, .DstWin = {0U, 0U, 5376U, 3648U}
                    },
                }
            }
        },
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
},
};

#endif /* RES_X_X_H */
