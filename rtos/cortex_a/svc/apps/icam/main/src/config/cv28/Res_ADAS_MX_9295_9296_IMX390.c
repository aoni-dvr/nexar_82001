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


#include "AmbaVOUT_Def.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaFPD_MAXIM_TFT1280768.h"
#include "AmbaHDMI_Def.h"

#include "AmbaFPD_HDMI.h"
#include "AmbaFPD_LT9611UXC.h"

#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#include "AmbaFPD_MAXIM_Define.h"

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

#ifdef CONFIG_ICAM_EEPROM_USED
#include "AmbaEEPROM.h"
extern AMBA_EEPROM_OBJ_s AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj;
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

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX390 (HDMI)",
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
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
       },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
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
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B }
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
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN1) MX9296+MX9295+IMX390 (HDMI)",
    .VinTree = {
        "$[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
       },
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
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
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
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
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX9296+2x(MX9295+IMX390) (HDMI)",
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
            .CapWin = {.OffsetX = 8U, .OffsetY = 42U, .Width = FHD_W, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 8U, .OffsetY = 42U, .Width = FHD_W, .Height = 2160U}
                }
            },
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, FHD_W, FHD_H},
            .ActWin = {0U},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 1U, FHD_W, FHD_H},
            .ActWin = {0U},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 540U, FHD_W, 540U}
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
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {FHD_W, 2160U}, .MaxWin = {FHD_W, 2160U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, FHD_W, FHD_H}, .DstWin = {0U, 0U, FHD_W, FHD_H}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, FHD_W, FHD_H}, .DstWin = {0U, FHD_H, FHD_W, FHD_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN1) MX9296+2x(MX9295+IMX390) (HDMI)",
    .VinTree = {
        "$[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 42U, .Width = FHD_W, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 8U, .OffsetY = 42U, .Width = FHD_W, .Height = 2160U}
                }
            },
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, FHD_W, FHD_H},
            .ActWin = {0U},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 1U, FHD_W, FHD_H},
            .ActWin = {0U},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 540U, FHD_W, 540U}
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
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {FHD_W, 2160U}, .MaxWin = {FHD_W, 2160U}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, FHD_W, FHD_H}, .DstWin = {0U, 0U, FHD_W, FHD_H}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, FHD_W, FHD_H}, .DstWin = {0U, FHD_H, FHD_W, FHD_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX390 (HDMI)\r\n\
                   (VIN1) MX9296+MX9295+IMX390",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 540U, FHD_W, 540U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 4U, .RecBits = 0x0FU,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {HD_W, HD_H}, .MaxWin = {HD_W, HD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, HD_W, HD_H}
                    },
                }
            }
        },
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 3U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {HD_W, HD_H}, .MaxWin = {HD_W, HD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, HD_W, HD_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX9296+2x(MX9295+IMX390) (HDMI)\r\n\
                   (VIN1) MX9296+MX9295+IMX390",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_0][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 42U, .Width = FHD_W, .Height = 2160U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 8U, .OffsetY = 42U, .Width = FHD_W, .Height = 2160U}
                }
            },
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = FHD_W, .Height = FHD_H},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [2] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0U, 0U, 960U, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {960U, 0U, 960U, 540U}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 540U, FHD_W, 540U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, FHD_W, FHD_H}, .DstWin = {0U, 0U, FHD_W, FHD_H}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, FHD_W, FHD_H}, .DstWin = {0U, 0U, FHD_W, FHD_H}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, FHD_W, FHD_H}, .DstWin = {0U, 0U, FHD_W, FHD_H}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(For still) (VIN0) MX9296+MX9295+IMX390 (HDMI)",
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
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
       },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
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
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B }
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
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 1, .EnableHDR = 1,
        .NumVin = 1,
        .RawCfg = {
            [0] = {
                .MaxRaw = {.Compressed = 0, .CeNeeded = 1, .FrameNum = 3, .Width = 1920, .Height = 1080,
                           .EffectW = 1920, .EffectH = 1080, .SensorMode = MX00_IMX390_1920_1080_A30P_HDR},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 1920, 1080, 0, 0, 0},
                },
            },
        },
    },
},
{
    .FormatName = "(For still) (VIN1) MX9296+MX9295+IMX390 (HDMI)",
    .VinTree = {
        "$[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
       },
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
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
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
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
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 1, .EnableHDR = 1,
        .NumVin = 1,
        .RawCfg = {
            [1] = {
                .MaxRaw = {.Compressed = 0, .CeNeeded = 1, .FrameNum = 3, .Width = 1920, .Height = 1080,
                           .EffectW = 1920, .EffectH = 1080, .SensorMode = MX00_IMX390_1920_1080_A30P_HDR},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 1920, 1080, 0, 0, 0},
                },
            },
        },
    },
},
{
    .FormatName = "(For Still) (VIN0/1) MX9296+MX9295+IMX390 (HDMI)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x0, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 540U, FHD_W, 540U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 1, .EnableHDR = 1,
        .NumVin = 2,
        .RawCfg = {
            [0] = {
                .MaxRaw = {.Compressed = 0, .CeNeeded = 1, .FrameNum = 2, .Width = 1920, .Height = 1080,
                           .EffectW = 1920, .EffectH = 1080, .SensorMode = MX00_IMX390_1920_1080_A30P_HDR},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 1920, 1080, 0, 0, 0},
                },
            },
            [1] = {
                .MaxRaw = {.Compressed = 0, .CeNeeded = 1, .FrameNum = 2, .Width = 1920, .Height = 1080,
                           .EffectW = 1920, .EffectH = 1080, .SensorMode = MX00_IMX390_1920_1080_A30P_HDR},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 1920, 1080, 0, 0, 0},
                },
            },
        }
    },
},
{
    .FormatName = "(For Still) (VIN0) MX9296+2x(MX9295+IMX390) (HDMI)",
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
            .CapWin = {.OffsetX = 8U, .OffsetY = 42U, .Width = FHD_W, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 8U, .OffsetY = 42U, .Width = FHD_W, .Height = 2160U}
                }
            },
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A30P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, FHD_W, FHD_H},
            .ActWin = {0U},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 1U, FHD_W, FHD_H},
            .ActWin = {0U},
            .MainWin = {FHD_W, FHD_H},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = ICAMAQ_TV_VOUT,
            .pDriver = &AmbaFPD_LT9611UXCObj,
            .StrmCfg = {
                .Win = {FHD_W, FHD_H}, .MaxWin = {FHD_W, FHD_H}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 0, FHD_W, 540U}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, FHD_W, FHD_H}, .DstWin = {0, 540U, FHD_W, 540U}
                    },
                }
            },
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A30HZ,
            .FrameRate = {.TimeScale = 30, .NumUnitsInTick = 1},
        },
    },
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 1, .EnableHDR = 1,
        .NumVin = 1,
        .RawCfg = {
            [0] = {
                .MaxRaw = {.Compressed = 0, .CeNeeded = 1, .FrameNum = 2, .Width = 1920, .Height = 2160,
                           .EffectW = 3840, .EffectH = 1080, .SensorMode = MX00_IMX390_1920_1080_A30P_HDR},
                .ChanMask = 0x3,
                .ChanWin = {
                    [0] = {0, 0, 1920, 1080, 0, 0, 0},
                    [1] = {1920, 0, 1920, 1080, 0, 0, 0},
                },
            },
        },
    },
},
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX390 (ZS095)",
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
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
       },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .MainWin = {1920U, 1080U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 1080}, .DstWin = {0, 0, 1920, 480}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
},
{
    .FormatName = "(VIN0)(fov0) 1760 x 448 (ZS095)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 88, .OffsetY = 337, .Width = 1760, .Height = 448},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
       },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin =  {    0,   0, 1760,  448 },
            .ActWin =  {   80,  64, 1600,  320 }, /* dzoom crop */
            .MainWin = {            1600,  320 },
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = { 80, 16, 1440, 288 }, .DstWin = { 0, 0, 1920, 480 }
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
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
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 320}, .MaxWin = {1600, 320}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 320}, .DstWin = {0, 0, 1600, 320}
                    },
                },
            },
        },
    },
},
{
    .FormatName = "(VIN1)(fov1) 1056 x 844 (ZS095)",
    .VinTree = {
        "$[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 440, .OffsetY = 139, .Width = 1056, .Height = 844},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
       },
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin =  {   0,   0, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {48, 38, 864, 692}, .DstWin = {0, 0, 1920, 480}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
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
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
    },
},
{
    .FormatName = "(VIN1)(fov2) 1056 x 844 (ZS095)",
    .VinTree = {
        "$[VIN_1][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 440, .OffsetY = 139, .Width = 1056, .Height = 844},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
       },
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin =  {   0,   0, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {48, 38, 864, 692}, .DstWin = {0, 0, 1920, 480}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
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
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
    },
},
{
    .FormatName = "(VIN1) (fov1/2) (zs095)",
    .VinTree = {
        "$[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 440, .OffsetY = 278, .Width = 1056, .Height = 1688},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 440, .OffsetY = 278, .Width = 1056, .Height = 1688}
                }
            },
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin =  {   0,   0, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
        [1] = {
            .RawWin =  {   0,   1, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {48, 38, 864, 692}, .DstWin = {  0, 0, 960, 480} , .BlendEnable = SVC_RES_BLD_TBL_FROM_ROM,
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {48, 38, 864, 692}, .DstWin = {960, 0, 960, 480} , .BlendEnable = SVC_RES_BLD_TBL_FROM_ROM,
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
    },
},
{
    .FormatName = "(VIN0/1) (fov0/2) (zs095)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_1][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_1][FOV_2]#!"
    },

    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 88, .OffsetY = 337, .Width = 1760, .Height = 448},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 440, .OffsetY = 278, .Width = 1056, .Height = 1688},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 440, .OffsetY = 278, .Width = 1056, .Height = 1688}
                }
            },
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin =  {    0,   0, 1760,  448 },
            .ActWin =  {   80,  64, 1600,  320 }, /* dzoom crop */
            .MainWin = {            1600,  320 },
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
        [1] = {
            .RawWin =  {   0,   0, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
        [2] = {
            .RawWin =  {   0,   1, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {80, 16, 1440, 288}, .DstWin = {     0, 0, 1280, 480},
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {48, 38,  864, 692}, .DstWin = {  1280, 0,  640, 480},
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
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
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 320}, .MaxWin = {1600, 320}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 320}, .DstWin = {0, 0, 1600, 320}
                    },
                },
            },
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
    },
},
{
    .FormatName = "(VIN0/1) (fov0/1) (zs095)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_1][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_1][FOV_2]#!"
    },

    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 88, .OffsetY = 337, .Width = 1760, .Height = 448},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 440, .OffsetY = 278, .Width = 1056, .Height = 1688},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 440, .OffsetY = 278, .Width = 1056, .Height = 1688}
                }
            },
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin =  {    0,   0, 1760,  448 },
            .ActWin =  {   80,  64, 1600,  320 }, /* dzoom crop */
            .MainWin = {            1600,  320 },
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
        [1] = {
            .RawWin =  {   0,   0, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
        [2] = {
            .RawWin =  {   0,   1, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {80, 16, 1440, 288}, .DstWin = { 640, 0, 1280, 480}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {48, 38,  864, 692}, .DstWin = {   0, 0,  640, 480}
                    },
                },
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
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
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 320}, .MaxWin = {1600, 320}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 320}, .DstWin = {0, 0, 1600, 320}
                    },
                },
            },
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
    },
},
{
    .FormatName = "(VIN0/1) (fov0/1/2) (zs095)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_1]#\
         $[VIN_1][B_MAXIM9296_1][B_MAXIM9295_0][S_IMX390_1][FOV_2]#!"
    },

    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 88, .OffsetY = 337, .Width = 1760, .Height = 448},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
        [1] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 440, .OffsetY = 278, .Width = 1056, .Height = 1688},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 440, .OffsetY = 278, .Width = 1056, .Height = 1688}
                }
            },
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
#ifdef CONFIG_ICAM_EEPROM_USED
            .pEEPROMDriver = &AmbaEEPROM_MAX9295_9296_MICRO_24AA256Obj,
#endif
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
        },
        [1] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX00_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_IMX390 | (IMG_SENSOR_HDR_MODE_1 << 16),
            }, /* sensor idx 1 */
        },
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin =  {    0,   0, 1760,  448 },
            .ActWin =  {   80,  64, 1600,  320 }, /* dzoom crop */
            .MainWin = {            1600,  320 },
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
        [1] = {
            .RawWin =  {   0,   0, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
        [2] = {
            .RawWin =  {   0,   1, 1056,  844 },
            .ActWin =  {  48,  38,  960,  768 }, /* dzoom crop */
            .MainWin = { 960, 768},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .RawCompression = IK_RAW_COMPACT_12B, .ViewCtrlOperation = SVC_RES_VIEW_CTRL_OP_PREV },
        },
    },
    .DispAltNum = 6, .DispAltIdx = 1,
    .DispAlt = {
        [0] = {
            /* display stream info */
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 0,
                    .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
                    .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
                    .StrmCfg = {
                        .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 0U, .SrcWin = { 80, 16, 1440, 288 }, .DstWin = { 0, 0, 1920, 480 }
                            },
                        },
                    },
                    .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
                    .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
                },
            },
        },
        [1] = {
            /* display stream info */
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 0,
                    .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
                    .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
                    .StrmCfg = {
                        .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 3U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 0U, .SrcWin = {80, 16, 1440, 288}, .DstWin = {   0, 0, 1920, 480},
                            },
                            [1] = {
                                .FovId = 1U, .SrcWin = {48, 38,  864, 692}, .DstWin = {   0, 0,  424, 480},
                            },
                            [2] = {
                                .FovId = 2U, .SrcWin = {48, 38,  864, 692}, .DstWin = {1496, 0,  424, 480},
                            },
                        },
                    },
                    .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
                    .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
                },
            },
        },
        [2] = {
            /* display stream info */
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 0,
                    .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
                    .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
                    .StrmCfg = {
                        .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 3U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 0U, .SrcWin = {80, 16, 1440, 288}, .DstWin = {   0, 0, 1920, 480},
                            },
                            [1] = {
                                .FovId = 1U, .SrcWin = {48, 38,  864, 692}, .DstWin = {   0, 0,  424, 270},
                            },
                            [2] = {
                                .FovId = 2U, .SrcWin = {48, 38,  864, 692}, .DstWin = {1496, 0,  424, 270},
                            },
                        },
                    },
                    .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
                    .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
                },
            },
        },
        [3] = {
            /* display stream info */
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 0,
                    .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
                    .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
                    .StrmCfg = {
                        .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 3U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 0U, .SrcWin = {80, 16, 1440, 288}, .DstWin = {   0, 0, 1920, 480},
                            },
                            [1] = {
                                .FovId = 1U, .SrcWin = {48, 38,  864, 692}, .DstWin = {   0, 0,  424, 270}, .BlendEnable = SVC_RES_BLD_TBL_FROM_ROM, .BlendTable = { .RomFileName = "Bld_Tbl_Right_BSD.bmp", .Win = {0, 0, 424, 270} },
                            },
                            [2] = {
                                .FovId = 2U, .SrcWin = {48, 38,  864, 692}, .DstWin = {1496, 0,  424, 270}, .BlendEnable = SVC_RES_BLD_TBL_FROM_ROM, .BlendTable = { .RomFileName = "Bld_Tbl_Left_BSD.bmp", .Win = {0, 0, 424, 270} },
                            },
                        },
                    },
                    .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
                    .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
                },
            },
        },
        [4] = {
            /* display stream info */
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 0,
                    .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
                    .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
                    .StrmCfg = {
                        .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 2U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 0U, .SrcWin = {80, 16, 1440, 288}, .DstWin = { 640, 0, 1280, 480}
                            },
                            [1] = {
                                .FovId = 1U, .SrcWin = {48, 38,  864, 692}, .DstWin = {   0, 0,  640, 480}
                            },
                        },
                    },
                    .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
                    .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
                },
            },
        },
        [5] = {
            /* display stream info */
            .DispNum = 1U, .DispBits = 0x01U,
            .DispStrm = {
                [0] = {
                    .VoutID = 0,
                    .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
                    .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
                    .StrmCfg = {
                        .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 2U,
                        .ChanCfg = {
                            [0] = {
                                .FovId = 0U, .SrcWin = {80, 16, 1440, 288}, .DstWin = {     0, 0, 1280, 480},
                            },
                            [1] = {
                                .FovId = 2U, .SrcWin = {48, 38,  864, 692}, .DstWin = {  1280, 0,  640, 480},
                            },
                        },
                    },
                    .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ,
                    .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
                },
            },
        },
    },
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1600, 320}, .MaxWin = {1600, 320}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1600, 320}, .DstWin = {0, 0, 1600, 320}
                    },
                },
            },
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 2U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {960, 768}, .MaxWin = {960, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 960, 768}, .DstWin = {0, 0, 960, 768}
                    },
                },
            },
        },
    },
},
};

#endif /* RES_X_X_H */
