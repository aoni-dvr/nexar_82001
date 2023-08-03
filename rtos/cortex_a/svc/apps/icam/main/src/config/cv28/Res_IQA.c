/**
 *  @file Res_IQA.c
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
#include "AmbaSensor_MAX9295_9296_IMX390.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD.h"
#include "AmbaFPD_LT9611UXC.h"
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#include "AmbaFPD_MAXIM_TFT1280768.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaCVBS.h"

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
    .FormatName = "(DK) (IMX577_3840_2160_A30P)",
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
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
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
},
{
    .FormatName = "(DK) (MX00_IMX390_1920_1080_A60P_HDR)",
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
            .DevMode = AMBA_FPD_LT9611_RGB888_1080P_A60HZ,
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
                .RecId = 0U, .M = 1U, .N = 60U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
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
},
};

#endif /* RES_X_X_H */
