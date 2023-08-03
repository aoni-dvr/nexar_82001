/**
 *  @file Res_ADAS_K351P.c
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
#include "AmbaSensor_K351P.h"
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

#define MAIN_W 1920
#define MAIN_H 1080
#define PYRAMID_BITS 0x1E
#define HIER_W 2720
#define HIER_H 1528

#define GRP_FMID_SWITCH     "FMID_SW"

static const SVC_RES_CFG_s g_ResCfg_K351P[] = {
{
    .FormatName = "(VIN0) K351P 2000*2000PA30",
    .VinTree = {
        "$[VIN_0][S_K351P_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_K351PObj,
            .SerdesType = SVC_RES_SERDES_TYPE_NONE,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1984, .Height = 1984},
            .TimeoutCfg = {5000U, 100U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .FixedFrameRate = 1,
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_8 */
            [0] = {
                .SensorMode = K351P_2000_2000_30P_10BIT,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_K351P,
            },   /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1984, 1984},
            .ActWin = {0},
            .MainWin = {1984, 1984},
            .PyramidBits = 0x0U, .HierWin = {0U, 0U},
            .PipeCfg = {.RawCompression = 0U, .MctfDisable = MCTF_BITS_ON_CMPR}
			//.PipeCfg = {.RawCompression = 1U, .PipeMode = SVC_VDOPIPE_DRAMEFCY, .MctfDisable = MCTF_BITS_ON_CMPR, .MctsDisable = MCTS_BITS_FORCE_RES, .MaxHorWarpCompensation = 64U, .WarpLumaWaitLine = 64U}
        },
    },
    /* display stream info */
    .DispNum = 0U, .DispBits = 0x00U,
#if 0    
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1984, 1984}, .DstWin = {0, 0, 1920, 1080}
                    },
                }
            },
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60, .NumUnitsInTick = 1},
        },
    },
#endif
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 30, .IdrInterval = 1, 
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1984, 1984}, .MaxWin = {1984, 1984}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1984, 1984}, .DstWin = {0, 0, 1984, 1984}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = SVC_REC_SRC_VIDEO,
                .DestBits = SVC_REC_DST_EMGEVT,
                .RecId = 1, .M = 1, .N = 30, .IdrInterval = 1, 
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {640, 640}, .MaxWin = {640, 640}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1984, 1984}, .DstWin = {0, 0, 640, 640}
                    },
                }
            }
        },
    },
    .StillCfg.EnableStill = 1,
#if 0
    .StillCfg = {
        .EnableStill = 1, .EnableStillRaw = 1, .EnableHISO = 1, .EnableHDR = 1,
        .NumVin = 1,
        .RawCfg = {
            [0] = {
                .MaxRaw = {.Compressed = 1, .CeNeeded = 1, .FrameNum = 3, .Width = 1920, .Height = 1080,
                           .EffectW = 1920, .EffectH = 1080, .SensorMode = K351P_2000_2000_30P_10BIT},
                .ChanMask = 0x1,
                .ChanWin = {
                    [0] = {0, 0, 1920, 1080, 0, 0, 0},
                },
            },
        }
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
#endif
},

};

static SVC_RES_CFG_s g_ResCfg[1];
static void g_RefCfgPreInit(void)
{
     AmbaWrap_memcpy(&g_ResCfg[0], &g_ResCfg_K351P[0], sizeof(SVC_RES_CFG_s)); 
}
#endif /* RES_X_X_H */

