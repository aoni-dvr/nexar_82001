/**
 *  @file Res_EMR_TI_AR0231_MIPI.c
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
#include "AmbaSensor_TI953_960_AR0231_MIPI.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaVOUT_Def.h"
#include "AmbaSPI_Def.h"
#include "AmbaFPD_T30P61.h"
#include "AmbaHDMI_Def.h"

static const SVC_RES_CFG_s g_ResCfg[] = {
{
    .FormatName = {
        "2FOV Vout1:FOV1 Stereo Fusion 0+2"
    },
    .VinTree = {
        "$[VIN_2][B_TI960_0][B_TI953_0][S_AR0231_0][FOV_1]#\
         $[VIN_2][B_TI960_1][B_TI953_0][S_AR0231_1][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [2] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [2] = { /* sensor on VIN_2 */
            [0] = { /* sensor idx 0 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        }
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        }
    },
    /* record stream info */
    .RecNum = 2U, .RecBits = 0x03U,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        }
    },
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_STEREO,
            .CvModeID = SVC_STEREO_MODE_FUSION_0_2,
            .FovInputNum = 2U,
            .FovInput = {
                [0] = 0U,   /* input is FOV_0 */
                [1] = 1U,   /* input is FOV_1 */
            },
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = {
        "4FOV Vout1:FOV1 Stereo Fusion 0+2"
    },
   .VinTree = {
        "$[VIN_2][B_TI960_0][B_TI953_0][S_AR0231_0][FOV_1]#\
         $[VIN_2][B_TI960_1][B_TI953_0][S_AR0231_1][FOV_0]#\
         $[VIN_0][B_TI960_2][B_TI953_0][S_AR0231_2][FOV_3]#\
         $[VIN_0][B_TI960_3][B_TI953_0][S_AR0231_3][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
        [2] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [2] = { /* sensor idx 2 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [3] = { /* sensor idx 3 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        },
        [2] = { /* sensor on VIN_2 */
            [0] = { /* sensor idx 0 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 2,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 2,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        }
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [2] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [3] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        }
    },
    /* record stream info */
    .RecNum = 4U, .RecBits = 0x0FU,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 2U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 3U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        }
    },
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_STEREO,
            .CvModeID = SVC_STEREO_MODE_FUSION_0_2,
            .FovInputNum = 4U,
            .FovInput = {
                [0] = 0U,   /* input is FOV_0 */
                [1] = 1U,   /* input is FOV_1 */
                [2] = 2U,   /* input is FOV_2 */
                [3] = 3U,   /* input is FOV_3 */
            },
        },
    },
    .StillCfg.EnableStill = 1,
},
{
    .FormatName = {
        "4FOV Vout1:FOV3 Stereo Fusion 0+2"
    },
   .VinTree = {
        "$[VIN_2][B_TI960_0][B_TI953_0][S_AR0231_0][FOV_1]#\
         $[VIN_2][B_TI960_1][B_TI953_0][S_AR0231_1][FOV_0]#\
         $[VIN_0][B_TI960_2][B_TI953_0][S_AR0231_2][FOV_3]#\
         $[VIN_0][B_TI960_3][B_TI953_0][S_AR0231_3][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
        [2] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [2] = { /* sensor idx 2 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [3] = { /* sensor idx 3 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 1,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        },
        [2] = { /* sensor on VIN_2 */
            [0] = { /* sensor idx 0 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 2,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = TI00_AR0231_1920_1080_30P_HDR,
                .SensorGroup = 2,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        }
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [2] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
        [3] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 1U, .MctfDisable = MCTF_BITS_ON_CMPR }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        }
    },
    /* record stream info */
    .RecNum = 4U, .RecBits = 0x0FU,
    .RecStrm = {
        [0] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 0U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 1U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 2U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
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
        [3] = {
            .RecSetting = {
                .SrcBits = (SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO),
                .DestBits     = SVC_REC_DST_FILE,
                .RecId = 3U, .M = 1U, .N = 30U, .IdrInterval = 1U,
                .FrameRate = {.TimeScale = 30000U, .NumUnitsInTick = 1001U, .Interlace = 0U},
            },
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            }
        }
    },
    .CvFlowNum = 1, .CvFlowBits = 0x1U,
    .CvFlow = {
        [0] = {
            .CvFlowType = SVC_CV_FLOW_STEREO,
            .CvModeID = SVC_STEREO_MODE_FUSION_0_2,
            .FovInputNum = 4U,
            .FovInput = {
                [0] = 0U,   /* input is FOV_0 */
                [1] = 1U,   /* input is FOV_1 */
                [2] = 2U,   /* input is FOV_2 */
                [3] = 3U,   /* input is FOV_3 */
            },
        },
    },
    .StillCfg.EnableStill = 1,
},
{ /* FOR VIGNETTE, FOV0 VOUT TO TV */
    .FormatName = {
        "2FOV Vout1:FOV0 for vignette"
    },
    .VinTree = {
        "$[VIN_2][B_TI960_0][B_TI953_0][S_AR0231_0][FOV_1]#\
         $[VIN_2][B_TI960_1][B_TI953_0][S_AR0231_1][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [2] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [2] = { /* sensor on VIN_2 */
            [0] = { /* sensor idx 0 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        }
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
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
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        }
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .CvFlowNum = 0U, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
{ /* FOR VIGNETTE, FOV1 VOUT TO TV */
    .FormatName = {
        "2FOV Vout1:FOV1 for vignette"
    },
    .VinTree = {
        "$[VIN_2][B_TI960_0][B_TI953_0][S_AR0231_0][FOV_1]#\
         $[VIN_2][B_TI960_1][B_TI953_0][S_AR0231_1][FOV_0]#!"
    },
    /* vin info */
    .VinNum = 1U,
    .VinCfg = {
        [2] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [2] = { /* sensor on VIN_2 */
            [0] = { /* sensor idx 0 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        }
    },
    /* fov info */
    .FovNum = 2U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        }
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 0U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 1U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        }
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .CvFlowNum = 0U, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
{ /* FOR VIGNETTE, FOV2 VOUT TO TV */
    .FormatName = {
        "4FOV Vout1:FOV2 for vignette"
    },
   .VinTree = {
        "$[VIN_2][B_TI960_0][B_TI953_0][S_AR0231_0][FOV_1]#\
         $[VIN_2][B_TI960_1][B_TI953_0][S_AR0231_1][FOV_0]#\
         $[VIN_0][B_TI960_2][B_TI953_0][S_AR0231_2][FOV_3]#\
         $[VIN_0][B_TI960_3][B_TI953_0][S_AR0231_3][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
        [2] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [2] = { /* sensor idx 2 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [3] = { /* sensor idx 3 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        },
        [2] = { /* sensor on VIN_2 */
            [0] = { /* sensor idx 0 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        }
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [2] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [3] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        }
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .CvFlowNum = 0U, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
{ /* FOR VIGNETTE, FOV3 VOUT TO TV */
    .FormatName = {
        "4FOV Vout1:FOV3 for vignette"
    },
   .VinTree = {
        "$[VIN_2][B_TI960_0][B_TI953_0][S_AR0231_0][FOV_1]#\
         $[VIN_2][B_TI960_1][B_TI953_0][S_AR0231_1][FOV_0]#\
         $[VIN_0][B_TI960_2][B_TI953_0][S_AR0231_2][FOV_3]#\
         $[VIN_0][B_TI960_3][B_TI953_0][S_AR0231_3][FOV_2]#!"
    },
    /* vin info */
    .VinNum = 2U,
    .VinCfg = {
        [0] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        },
        [2] = {
            .pDriver = &AmbaSensor_TI_AR0231MIPIObj,
            .SerdesType = SVC_RES_SERDES_TYPE_TI,
            .CapWin = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U},
            .SubChanNum = 1U,
            .SubChanCfg = {
                [0] = {.SubChan = {.IsVirtual = 0U, .Index = 0U},
                       .Option = AMBA_DSP_VIN_CAP_OPT_INTC,
                       .IntcNum = 2U,
                       .ConCatNum = 1U,
                       .CaptureWindow = {.OffsetX = 0U, .OffsetY = 0U, .Width = 1920U, .Height = 2160U}
                }
            },
            .FrameRate = {.TimeScale = 30000, .NumUnitsInTick = 1001},
        }
    },
    .SensorCfg = {
        [0] = { /* sensor on VIN_0 */
            [2] = { /* sensor idx 2 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [3] = { /* sensor idx 3 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        },
        [2] = { /* sensor on VIN_2 */
            [0] = { /* sensor idx 0 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            },
            [1] = { /* sensor idx 1 */
                .SensorMode = TI00_AR0231_1920_1080_30P,
                .SensorGroup = 0,
                .IQTable = IQ_SSR_TI953_960_AR0231,
            }
        }
    },
    /* fov info */
    .FovNum = 4U,
    .FovCfg = {
        [0] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [1] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [2] = {
            .RawWin = {0U, 1U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
        [3] = {
            .RawWin = {0U, 0U, 1920U, 1080U},
            .ActWin = {0U},
            .MainWin = {1920U, 1080U},
            .PyramidBits = 0x3F, .HierWin = {0U, 0U},
            .PipeCfg = { .RotateFlip = 0U, .HdrExposureNum = 0U, .HdrBlendHieght = 0U, .LinearCE = 0U }
        },
    },
    /* display stream info */
    .DispNum = 2U, .DispBits = 0x03U,
    .DispStrm = {
        [0] = {
            .VoutID = 0U,
            .pDriver = &AmbaFPD_T30P61Obj,
            .StrmCfg = {
                .Win = {960, 240}, .MaxWin = {960, 240}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 2U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 960, 240}
                    },
                }
            },
            .DevMode = AMBA_FPD_T30P61_960_240_60HZ,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        },
        [1] = {
            .VoutID = 1U,
            .pDriver = &AmbaFPD_HDMI_Obj,
            .StrmCfg = {
                .Win = {1920U, 1080U}, .MaxWin = {1920U, 1080U}, .NumChan = 1U,
                .ChanCfg = {
                    [0] = {
                        .FovId = 3U, .SrcWin = {0U, 0U, 1920U, 1080U}, .DstWin = {0U, 0U, 1920U, 1080U}
                    },
                }
            },
            .DevMode = HDMI_VIC_1080P,
            .FrameRate = {.TimeScale = 60000U, .NumUnitsInTick = 1001U},
        }
    },
    /* record stream info */
    .RecNum = 0U, .RecBits = 0x0U,
    .CvFlowNum = 0U, .CvFlowBits = 0x0U,
    .StillCfg.EnableStill = 1,
},
};

#endif /* RES_X_X_H */
