/**
 *  @file Res_ADAS_TI_913_954_OV10635.c
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
#include "AmbaSensor_TI913_954_OV10635.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaHDMI_Def.h"

#define OV10635_SHDR_IQ      (IQ_SSR_TI913_954_OV10635 | (IMG_SENSOR_HDR_MODE_0 << 16))

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) TI954+TI913+OV10635 (SHDR)(HDMI)",
    .VinTree = {
        "$[VIN_0][B_TI954_0][B_TI913_0][S_OV9716_0][FOV_0_BYPASS][M_RAW_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI03_OV10635Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 2560U, .Height = 800U},
            .TimeoutCfg = { 5000U, 100U },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001U},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = TI03_OV10635_1280_800_30P_SHDR,
                .SensorGroup = 0,
                .IQTable = OV10635_SHDR_IQ,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 2560U, 800U},
            .ActWin = {0U},
            .MainWin = {1280U, 800U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = {.LinearCE = LINEAR_CE_FORCE_DISABLE, .PipeMode = SVC_VDOPIPE_MIPIRAW}
        },
        [1] = {
            .RawWin = {0U, 0U, 1280U, 800U},
            .ActWin = {0U},
            .MainWin = {1280U, 800U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .VirtChan ={.Enable = SVC_RES_PIPE_VIRTUAL_CHAN_FROM_UPSTREAM, .Id = 0U},
            .PipeCfg = {.LinearCE = LINEAR_CE_FORCE_DISABLE, .PipeMode = SVC_VDOPIPE_NORMAL, .FromVirtVin = SVC_RES_FOV_FROM_VIRT_VIN},
            
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1280U, 800U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1280U, 800U}, .DstWin = {0U, 0U, 1280U, 800U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1001U},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280U, 800U}, .MaxWin = {1280U, 800U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1280U, 800U}, .DstWin = {0U, 0U, 1280U, 800U}
                    },
                }
            }
        },
    },
},
};

#endif /* RES_X_X_H */

