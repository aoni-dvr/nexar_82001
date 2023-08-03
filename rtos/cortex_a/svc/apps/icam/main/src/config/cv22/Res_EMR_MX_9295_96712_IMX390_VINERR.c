/**
 *  @file Res_EMR_MX_9295_96712_IMX390_VINERR.c
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
#include "AmbaSensor_MAX9295_96712_IMX390.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaDSP_ImageFilter.h"

#include "AmbaVOUT_Def.h"
#include "AmbaSPI_Def.h"
#include "AmbaFPD_T30P61.h"
#ifdef CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#endif
#ifdef CONFIG_FPD_MAXIM_DUAL_TFT1280768
#include "AmbaFPD_MAXIM_Dual_TFT1280768.h"
#endif
#include "AmbaFPD_MAXIM_Define.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD_HDMI.h"

#define IMX390_IQ (IQ_SSR_MAX9295_96712_IMX390_24 | (IMG_SENSOR_HDR_MODE_1 << 16))

#if defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII) && defined(CONFIG_FPD_MAXIM_DUAL_TFT1280768)
#define SVC_RES_CFG_3VOUT
#elif defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII)
#define SVC_RES_CFG_2VOUT
#else
#define SVC_RES_CFG_1VOUT
#endif

static const SVC_RES_CFG_s g_ResCfg[] = {
{
#ifdef SVC_RES_CFG_3VOUT
    .FormatName = "(VIN1) MX96712+3x(MX9295+IMX390)(3VOUT)",
#elif defined(SVC_RES_CFG_2VOUT)
    .FormatName = "(VIN1) MX96712+3x(MX9295+IMX390)(MAXIM+HDMI2IN1)",
#else
    .FormatName = "(VIN1) MX96712+3x(MX9295+IMX390)(HDMI3IN1)",
#endif
    .VinTree = {
        "$[VIN_1][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#\
         $[VIN_1][B_MAXIM96712_3][B_MAXIM9295_0][S_IMX390_2][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 24U, .Width = 1920U, .Height = 3240U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 3,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 24, .Width = 1920, .Height = 3240}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
            [2] = {
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 2 */
        }
    },
    /* fov info */
    .FovNum = 3U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {0}, /* dzoom crop */
            .MainWin = {1280, 768},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [2] = {
            .RawWin = {0, 2, 1920, 1080},
            .ActWin = {0}, /* dzoom crop */
            .MainWin = {1280, 768},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
#ifdef SVC_RES_CFG_3VOUT
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {1280, 0, 1280, 768}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 1280, 768}
                    },
                },
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#elif defined(SVC_RES_CFG_2VOUT)
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {960, 0, 960, 1080}
                    },
                    [1] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 960, 1080}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#else
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 3U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 504}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {960, 504, 960, 576}
                    },
                    [2] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 504, 960, 576}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#endif
    /* record stream info */
    .RecNum = 3U, .RecBits = 0x07U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            }
        },
        [2] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 2, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            }
        }
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .CvFlow = { {0} },
    .StillCfg.EnableStill = 1,
},
{
#ifdef SVC_RES_CFG_3VOUT
    .FormatName = "(VIN1) MX96712+2x(MX9295+IMX390)(3VOUT)",
#elif defined(SVC_RES_CFG_2VOUT)
    .FormatName = "(VIN1) MX96712+2x(MX9295+IMX390)(MAXIM+HDMI)",
#else
    .FormatName = "(VIN1) MX96712+2x(MX9295+IMX390)(HDMI2IN1)",
#endif
    .VinTree = {
        "$[VIN_1][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#\
         $[VIN_1][B_MAXIM96712_1][B_MAXIM9295_0][S_IMX390_1][FOV_1]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
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
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
            [1] = {
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 1 */
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
        [1] = {
            .RawWin = {0, 1, 1920, 1080},
            .ActWin = {0}, /* dzoom crop */
            .MainWin = {1280, 768},
            .PyramidBits = 0x0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
#ifdef SVC_RES_CFG_3VOUT
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {1280, 0, 1280, 768}
                    },
                },
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#elif defined(SVC_RES_CFG_2VOUT)
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {960, 0, 960, 1080}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#else
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 2U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 504}
                    },
                    [1] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {960, 504, 960, 576}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#endif
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
        [1] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1280, 768}, .MaxWin = {1280, 768}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0, 0, 1280, 768}, .DstWin = {0, 0, 1280, 768}
                    },
                }
            }
        },
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .CvFlow = { {0} },
    .StillCfg.EnableStill = 1,
},
{
#ifdef SVC_RES_CFG_3VOUT
    .FormatName = "(VIN1) MX96712+1x(MX9295+IMX390)(3VOUT)",
#elif defined(SVC_RES_CFG_2VOUT)
    .FormatName = "(VIN1) MX96712+1x(MX9295+IMX390)(MAXIM+HDMI)",
#else
    .FormatName = "(VIN1) MX96712+1x(MX9295+IMX390)(HDMI)",
#endif
    .VinTree = {
        "$[VIN_1][B_MAXIM96712_0][B_MAXIM9295_0][S_IMX390_0][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [1] = {
            .pDriver = &AmbaSensor_MX01_IMX390Obj,
            .SerdesType = SVC_RES_SERDES_TYPE_MAXIM,
            .CapWin = {.OffsetX = 8U, .OffsetY = 8U, .Width = 1920U, .Height = 1080U},
            .SubChanNum = 1,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0, .Index = 0},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 1,
                       .CaptureWindow = {.OffsetX = 8, .OffsetY = 8, .Width = 1920, .Height = 1080}
                }
            },
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
            .TimeoutCfg = {5000U, 100U},
        }
    },
    .SensorCfg = {
        [1] = { /* sensor on VIN_1 */
            [0] = {
                .SensorMode = MX01_IMX390_1920_1080_A60P_HDR,
                .SensorGroup = 0,
                .IQTable = IMX390_IQ,
            }, /* sensor idx 0 */
        }
    },
    /* fov info */
    .FovNum = 1U,
    .FovCfg = {
        [0] = {
            .RawWin = {0, 0, 1920, 1080},
            .ActWin = {0}, /* dzoom crop */
            .MainWin = {1920, 480},
            .PyramidBits = 0, .HierWin = {0, 0},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR, .RawCompression = IK_RAW_COMPACT_12B }
        },
    },
    /* display stream info */
#if defined(SVC_RES_CFG_3VOUT)
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_MAXIM_Dual_TFT128Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {2560, 768}, .MaxWin = {2560, 768}, .NumChan = 0U,
                .ChanCfg = { [0] = { 0 }, },
            },
            .DevMode = (UINT8)MAXIM_MULTIVIEW_SPLITTER_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#elif defined(SVC_RES_CFG_2VOUT)
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0,
            .pDriver = &AmbaFPD_MAXIM_ZS095Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            },
            .DevMode = (UINT8)AMBA_FPD_MAXIM_A60HZ | (UINT8)0x80, // reseting maxim serdes is not necessary for boot
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
        [1] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 0U,
                .ChanCfg = { [0] = { 0 }, },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#else
    .DispNum = 1U, .DispBits = 0x01U,
    .DispStrm = {
        [0] = {
            .VoutID = 1,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .VideoRotateFlip = AMBA_DSP_ROTATE_0_HORZ_FLIP,
            .StrmCfg = {
                .Win = {1920, 1080}, .MaxWin = {1920, 1080}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 504}
                    },
                },
            },
            .DevMode = (UINT8)HDMI_VIC_1920_1080_A60P,
            .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000},
        },
    },
#endif
    /* record stream info */
    .RecNum = 1U, .RecBits = 0x01U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0, .M = 1, .N = 60, .IdrInterval = 1,
                .FrameRate = {.TimeScale = 60000, .NumUnitsInTick = 1000, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920, 480}, .MaxWin = {1920, 480}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0, 0, 1920, 480}, .DstWin = {0, 0, 1920, 480}
                    },
                }
            }
        },
    },
    .CvFlowNum = 0, .CvFlowBits = 0x0U,
    .CvFlow = { {0} },
    .StillCfg.EnableStill = 1,
},
};

#endif /* RES_X_X_H */
