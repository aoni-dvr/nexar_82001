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
#include "AmbaSensor_MAX9295_9296_IMX390_RCCB.h"
#include "AmbaIQParamHandlerSample.h"

#include "AmbaVOUT_Def.h"
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaFPD_MAXIM_TFT1280768.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_RefSeg.h"
#include "SvcCvFlow_Stereo.h"
#include "SvcCvAppDef.h"
#endif

//#define TEST_MULTIPLE_SCALE

#ifdef CONFIG_ICAM_VOUTB_OSD_BUF_FHD
#define INSTANCE_MASK_OSD_BASE      (((UINT32)1806U << 16U) | 900U)
#elif defined CONFIG_ICAM_VOUTB_OSD_BUF_HD
#define INSTANCE_MASK_OSD_BASE      (((UINT32)1204U << 16U) | 604U)
#else
#define INSTANCE_MASK_OSD_BASE      (0U)
#endif


static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MX9296+MX9295+IMX390_RCCB (CV FDAG) AmbaPcpt(KP, 3D, Mask, Classifier) + AmbaSeg\n",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390CObj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = {
                .SensorMode = MX00_IMX390C_1920_1080_30P_HDR,
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
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
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
            .DevMode = HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1000U},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO | SVC_REC_SRC_DATA),
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
#if defined(CONFIG_BUILD_CV)
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .Chan = 0U, .pCvObj = &SvcCvFlow_AmbaODObj,
            .CvModeID = AMBAOD_FDAG_PCPT_OD_DET_CLF,
            .InputCfg = {
                .InputNum = 1U,
                .Input = {
                    [0] = {
                        .DataSrc = SVC_CV_DATA_SRC_PYRAMID, .StrmId = 0U,   /* input is FOV_0 */
                        .FrameWidth = 1920, .FrameHeight = 1080, .NumRoi = 1U,
                        .Roi = {
                            [0] = {1, 40, 62, 0, 0},
                        }
                    },
                },
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .Config = {0, INSTANCE_MASK_OSD_BASE, 0, 0},
            .CvFlowType = SVC_CV_FLOW_AMBA_OD,
        },
        [1] = {
            .Chan = 1U, .pCvObj = &SvcCvFlow_AmbaSegObj,
            .CvModeID = AMBASEG_FDAG_FC,
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
                .EnableFeeder = 1U, .FeederID = 0U, .FeederFlag = 0U,
            },
            .CvFlowType = SVC_CV_FLOW_AMBA_SEG,
        },
    },
#endif
    .StillCfg.EnableStill = 1,
},
};

#endif /* RES_X_X_H */
