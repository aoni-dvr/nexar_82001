/**
 *  @file Res_DMS_MX_IMX390_MX_AR0144_MX_AR0239.c
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
#include "AmbaSensor_MAX96707_9286_AR0144.h"
#include "AmbaSensor_MAX9295_9296_AR0239_RGBIR.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaSPI_Def.h"
#include "AmbaVOUT_Def.h"
#include "AmbaFPD_T30P61.h"
#include "AmbaFPD_MAXIM_TFT1280768.h"
#include "AmbaFPD_MAXIM_Define.h"
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
#include "SvcCvAppDef.h"
#endif

#define IMX390_MAIN_WIDTH   (1920U)
#define IMX390_MAIN_HEIGHT  (1080U)

#define IMX390_PREV_WIDTH   (1920U)
#define IMX390_PREV_HEIGHT  (1080U)

#define AR0144_MAIN_WIDTH   (1280U)
#define AR0144_MAIN_HEIGHT  (720U)

#define AR0144_PREV_WIDTH   (640U)
#define AR0144_PREV_HEIGHT  (360U)

#define AR0239_MAIN_WIDTH   (960U)
#define AR0239_MAIN_HEIGHT  (540U)

#define AR0239_PREV_WIDTH   (640U)
#define AR0239_PREV_HEIGHT  (360U)

#define DUPLEX_PREV_WIDTH   (640U)
#define DUPLEX_PREV_HEIGHT  (360U)

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = "(VIN0) MAX9295_9296_IMX390 (VIN1) MAX96707_9286_AR0144 (VIN2) MAX9295_9296_AR0239 (DUPLEX)",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_IMX390_0][FOV_2]#\
         $[VIN_1][B_MAXIM9286_0][B_MAXIM96707_0][S_AR0144_0][FOV_1]#\
         $[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_AR0239_0][FOV_0]#\
         $[DEC_0][FOV_3]#!"
    },
    /* vin info */
    .VinNum = 3U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 21U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {.Boot = 1000U, .Liveview = 1000U},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX03_AR0144Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1280U, .Height = 720U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {.Boot = 1000U, .Liveview = 1000U},
        },
        [2] = {
            .pDriver = &AmbaSensor_MX00_AR0239Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {.Boot = 1000U, .Liveview = 1000U},
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
            [0] = { /* sensor idx 0 */
                .SensorMode = MX03_AR0144_1280_720_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96707_9286_AR0144,
            },
        },
        [2] = { /* sensor on VIN_2 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_AR0239_1920_1080_2LANE_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0239,
            },
        },
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [2] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {IMX390_MAIN_WIDTH, IMX390_MAIN_HEIGHT},
            .PyramidBits = 0x12, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0U, 0U, 1280U, 720U},
            .ActWin = {0U, 0U, 0U, 0U},
            .MainWin = {AR0144_MAIN_WIDTH, AR0144_MAIN_HEIGHT},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {AR0239_MAIN_WIDTH, AR0239_MAIN_HEIGHT},
            .PyramidBits = 0, .HierWin = {   0,   0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [3] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { 0 }
        },
    },
    /* display stream info */
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 4U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0U, 180U, IMX390_MAIN_WIDTH, 720U}, .DstWin = {0U, 0U, 1920U, 720U},
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, AR0144_MAIN_WIDTH, AR0144_MAIN_HEIGHT}, .DstWin = {0U, 720U , AR0144_PREV_WIDTH, AR0144_PREV_HEIGHT}
                    },
                    [2] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, AR0239_MAIN_WIDTH, AR0239_MAIN_HEIGHT}, .DstWin = {640U, 720U, AR0239_PREV_WIDTH, AR0239_PREV_HEIGHT}
                    },
                    [3] = {
                        .FovId = 3U, .SrcWin = {0, 0, 1920U, 1080U}, .DstWin = {1280U, 720U, DUPLEX_PREV_WIDTH, DUPLEX_PREV_HEIGHT}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
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
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {432, 368}, .MaxWin = {432, 368}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, AR0239_MAIN_WIDTH, AR0239_MAIN_HEIGHT}, .DstWin = {0, 0, 432, 368}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {300, 300}, .MaxWin = {300, 300}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, AR0144_MAIN_WIDTH, AR0144_MAIN_HEIGHT}, .DstWin = {0, 0, 300, 300}
                    },
                }
            }
        },
    },
    .CvFlowNum = 2, .CvFlowBits = 0x3U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_REF_OD_FDAG,
            .CvModeID = REFOD_FDAG_LINUX_CH0_FACEDETECT,
            .FovInputNum = 1U,
            .FovInput = {
                [0] = 0x80000003U,   /* input is Record YUV stream 2 */
            },
        },
        [1] = {
            .CvFlowType = SVC_CV_FLOW_REF_OD_FDAG,
            .CvModeID = REFOD_FDAG_LINUX_CH1_BODYDETECT,
            .FovInputNum = 1U,
            .FovInput = {
                [0] = 0x80000002U,   /* input is Record YUV stream 1 */
            },
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MX9296+MX9295+AR0239 1080p30 2LANE + (VIN1) MX9286+MX96707+AR0144 720p30 1LANE",
    .VinTree = {
        "$[VIN_0][B_MAXIM9296_0][B_MAXIM9295_0][S_AR0239_0][FOV_0]#\
         $[VIN_1][B_MAXIM9286_0][B_MAXIM96707_0][S_AR0144_0][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_MX00_AR0239Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .pDriver = &AmbaSensor_MX03_AR0144Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1280U, .Height = 720U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_AR0239_1920_1080_2LANE_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0239,
            },
        },
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX03_AR0144_1280_720_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96707_9286_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {   0,   0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
        [1] = {
            .RawWin = {0U, 0U, 1280U, 720U},
            .ActWin = {0U, 0U, 0U, 0U},
            .MainWin = {1280U, 720U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_MAXIM_TFT128Obj,
            .StrmCfg = {
                .Win = {1280U, 768U}, .MaxWin = {1280U, 768U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1280U, 768U}
                    },
                }
            },
            .DevMode = AMBA_FPD_MAXIM_A60HZ,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1280U, 720U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U, .BitRate = 20000000U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280U, 720U}, .MaxWin = {1280U, 720U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280U, 720U}, .DstWin = {0U, 0U, 1280U, 720U}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U, .BitRate = 20000000U,
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
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN0) MAX9295_9296_IMX390",
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
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        }
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
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { 0 }
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
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
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
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN1) MAX96707_9286_AR0144",
    .VinTree = {
        "$[VIN_1][B_MAXIM9286_0][B_MAXIM96707_0][S_AR0144_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX03_AR0144Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1280U, .Height = 720U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        }
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX03_AR0144_1280_720_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX96707_9286_AR0144,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 0U, 1280U, 720U},
            .ActWin = {0U, 0U, 0U, 0U},
            .MainWin = {1280U, 720U},
            .PyramidBits = 0x0, .HierWin = {0x0, 0x0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
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
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280, 720}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1000U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1280, 720}, .DstWin = {0U, 0U, 1280, 720}
                    },
                }
            }
        },
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = "(VIN2) MAX9295_9296_AR0239",
    .VinTree = {
        "$[VIN_2][B_MAXIM9296_0][B_MAXIM9295_0][S_AR0239_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [2] = {
            .pDriver = &AmbaSensor_MX00_AR0239Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 1080U},
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1000},
        },
    },
    .SensorCfg = {
        [2] = { /* sensor on VIN_2 */
            [0] = { /* sensor idx 0 */
                .SensorMode = MX00_AR0239_1920_1080_2LANE_A30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_MAX9295_9296_AR0239,
            },
        },
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0},
            .MainWin = {1920, 1080},
            .PyramidBits = 0, .HierWin = {   0,   0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U, .RawCompression = 0U }
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
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
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
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
};

#endif /* RES_X_X_H */
