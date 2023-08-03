/**
 *  @file Res_ADAS_MX_9295_9296_IMX390.c
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
#include "AmbaSensor_MAX9295_9296_IMX390.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD.h"
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

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX390 (1936*1106p60) ASIL",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 1,
                       .CaptureWindow = {.OffsetX = 8U, .OffsetY = 8U, .Width = 1920, .Height = 1080}
                }
            },
#if defined(AMBA_DSP_VIN_CAP_OPT_EMBD) && defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
            .EmbChanCfg = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_EMBD,
                       .IntcNum = 1,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 1936, .Height = 2}
            },
#endif
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },

    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0U, 0U,  1920U, 1080U}
                    },
                }
            }
        },
    },
},

{
    .FormatName = "(VIN0) 2xMX9296+MX9295+IMX390 (1936*1106p60) ASIL",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 16U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 16, .Width = 1920, .Height = 2160}
                }
            },
#if defined(AMBA_DSP_VIN_CAP_OPT_EMBD) && defined(CONFIG_MX00_IMX390_ASIL_ENABLED)

            .EmbChanCfg = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_EMBD,
                       .IntcNum = 1,
                       .CaptureWindow = {.OffsetX = 0, .OffsetY = 0, .Width = 1936, .Height = 3}
            },
#endif
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
    },

    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {1920U, 540}, .MaxWin = {1920U, 1080U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 960U, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {960U, 0U, 960U, 540U}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
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
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {0U, 0U,  1920U, 1080U}
                    },
                }
            }
        },
    },
},
};

#endif /* RES_X_X_H */
