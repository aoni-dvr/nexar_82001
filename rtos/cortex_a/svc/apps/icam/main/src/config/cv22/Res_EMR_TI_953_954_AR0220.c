/**
 *  @file Res_EMR_TI_953_954_AR0220.c
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
#include "AmbaSensor_TI953_954_AR0220.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD_T30P61.h"
#include "AmbaHDMI_Def.h"


static const SVC_RES_CFG_s g_ResCfg[] = {
{   /*AR0239_1920_1080_30P_HDR */
    .FormatName = "(VIN0) TI954+TI953+AR0220 940P44 HDR",
    .VinTree = {
        "$[VIN_0][B_TI954_0][B_TI953_0][S_AR0220_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI00_AR0220Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 4U, .Width = 1824U, .Height = 940U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = TI00_AR0220_1828_948_44P_HDR,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_954_AR0220 | (IMG_SENSOR_HDR_MODE_2 << 16U),
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1824U, 940U},
            .ActWin = {0},
            .MainWin = {1824U, 940U},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_14B }
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
                        .FovId = 0U, .SrcWin = {0, 0, 1824U, 940U}, .DstWin = {0, 0, 960U, 240U}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1824U, 940U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1824U, 940U}, .DstWin = {0, 0, 1824U, 940U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
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
                .FrameRate = {.TimeScale = 44U, .NumUnitsInTick = 1U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1824U, 940U}, .MaxWin = {1824U, 940U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1824U, 940U}, .DstWin = {0, 0, 1824U, 940U}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
},
};

#endif /* RES_X_X_H */
