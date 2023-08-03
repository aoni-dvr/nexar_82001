/**
 *  @file AmbaFPD_LT9611UXC.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Control APIs of LONTIUM LT9611UXC (MIPI DSI -> HDMI)
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaFPD.h"
#include "AmbaVOUT.h"
#include "AmbaFPD_LT9611UXC.h"

static AMBA_FPD_LT9611UXC_CONFIG_s *pDispConfig;

/*-----------------------------------------------------------------------------------------------*\
 * LT9611UXC video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_LT9611UXC_CONFIG_s LT9611UXC_Config[AMBA_FPD_LT9611_SERDES_NUM_MODE] = {
    [AMBA_FPD_LT9611_YUV422_720P_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_422_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 74175824U,
            .Htotal         = 1650U,
            .Vtotal         = 750U,
            .ActivePixels   = 1280U,
            .ActiveLines    = 720U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_RGB888_720P_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 74175824U,
            .Htotal         = 1650U,
            .Vtotal         = 750U,
            .ActivePixels   = 1280U,
            .ActiveLines    = 720U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_YUV422_1080P_30HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_422_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 74175824U,
            .Htotal         = 2200U,
            .Vtotal         = 1125U,
            .ActivePixels   = 1920U,
            .ActiveLines    = 1080U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_RGB888_1080P_30HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 74175824U,
            .Htotal         = 2200U,
            .Vtotal         = 1125U,
            .ActivePixels   = 1920U,
            .ActiveLines    = 1080U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_YUV422_1080P_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_422_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 148351648U,
            .Htotal         = 2200U,
            .Vtotal         = 1125U,
            .ActivePixels   = 1920U,
            .ActiveLines    = 1080U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_RGB888_1080P_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 148351648U,
            .Htotal         = 2200U,
            .Vtotal         = 1125U,
            .ActivePixels   = 1920U,
            .ActiveLines    = 1080U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_YUV422_2160P_30HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_422_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 296703297U,
            .Htotal         = 4400U,
            .Vtotal         = 2250U,
            .ActivePixels   = 3840U,
            .ActiveLines    = 2160U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_RGB888_2160P_30HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 296703297U,
            .Htotal         = 4400U,
            .Vtotal         = 2250U,
            .ActivePixels   = 3840U,
            .ActiveLines    = 2160U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_YUV422_720P_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_422_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 74250000U,
            .Htotal         = 1650U,
            .Vtotal         = 750U,
            .ActivePixels   = 1280U,
            .ActiveLines    = 720U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_RGB888_720P_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 74250000U,
            .Htotal         = 1650U,
            .Vtotal         = 750U,
            .ActivePixels   = 1280U,
            .ActiveLines    = 720U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_YUV422_1080P_A30HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_422_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 74250000U,
            .Htotal         = 2200U,
            .Vtotal         = 1125U,
            .ActivePixels   = 1920U,
            .ActiveLines    = 1080U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_RGB888_1080P_A30HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 74250000U,
            .Htotal         = 2200U,
            .Vtotal         = 1125U,
            .ActivePixels   = 1920U,
            .ActiveLines    = 1080U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_YUV422_1080P_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_422_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 148500000U,
            .Htotal         = 2200U,
            .Vtotal         = 1125U,
            .ActivePixels   = 1920U,
            .ActiveLines    = 1080U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_RGB888_1080P_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 148500000U,
            .Htotal         = 2200U,
            .Vtotal         = 1125U,
            .ActivePixels   = 1920U,
            .ActiveLines    = 1080U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_YUV422_2160P_A30HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_422_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 297000000U,
            .Htotal         = 4400U,
            .Vtotal         = 2250U,
            .ActivePixels   = 3840U,
            .ActiveLines    = 2160U,
            .Interlace      = 0U,
        },
    },
    [AMBA_FPD_LT9611_RGB888_2160P_A30HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .FrameTiming    = {
            .PixelClkFreq   = 297000000U,
            .Htotal         = 4400U,
            .Vtotal         = 2250U,
            .ActivePixels   = 3840U,
            .ActiveLines    = 2160U,
            .Interlace      = 0U,
        },
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_LT9611UXCEnable
 *
 *  @Description:: Enable FPD panel device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_LT9611UXCEnable(void)
{
    UINT32 RetVal = ERR_NONE;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_LT9611UXCDisable
 *
 *  @Description:: Disable FPD panel device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_LT9611UXCDisable(void)
{
    UINT32 RetVal;

    RetVal = AmbaVout_MipiDsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_DOWN, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_LT9611UXCGetInfo
 *
 *  @Description:: Get vout configuration for current FPD display mode
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pInfo:      pointer to FPD display mode info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_LT9611UXCGetInfo(AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 Framerate;

    if ((pDispConfig == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pInfo->Width = pDispConfig->FrameTiming.ActivePixels;
        pInfo->Height = pDispConfig->FrameTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;

        Framerate = pDispConfig->FrameTiming.PixelClkFreq / ((UINT32)pDispConfig->FrameTiming.Htotal * (UINT32)pDispConfig->FrameTiming.Vtotal);

        if (Framerate == 60U) {
            pInfo->FrameRate.TimeScale = 60U;
            pInfo->FrameRate.NumUnitsInTick = 1U;
            pInfo->FrameRate.Interlace = 0U;
        } else if (Framerate == 59U) {
            /* 59.94Hz */
            pInfo->FrameRate.TimeScale = 60000U;
            pInfo->FrameRate.NumUnitsInTick = 1001U;
            pInfo->FrameRate.Interlace = 0U;
        } else if (Framerate == 30U) {
            /* 30Hz */
            pInfo->FrameRate.TimeScale = 30U;
            pInfo->FrameRate.NumUnitsInTick = 1U;
            pInfo->FrameRate.Interlace = 0U;
        } else {
            /* 29.97Hz */
            pInfo->FrameRate.TimeScale = 30000U;
            pInfo->FrameRate.NumUnitsInTick = 1001U;
            pInfo->FrameRate.Interlace = 0U;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_LT9611UXCGetModeInfo
 *
 *  @Description:: Get vout configuration for specific mode
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pInfo:      pointer to FPD display mode info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_LT9611UXCGetModeInfo(UINT8 mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 Framerate;

    if ( (mode >= (UINT8)AMBA_FPD_LT9611_SERDES_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &LT9611UXC_Config[mode];
        pInfo->Width = pDispConfig->FrameTiming.ActivePixels;
        pInfo->Height = pDispConfig->FrameTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;

        Framerate = pDispConfig->FrameTiming.PixelClkFreq / ((UINT32)pDispConfig->FrameTiming.Htotal * (UINT32)pDispConfig->FrameTiming.Vtotal);

        if (Framerate == 60U) {
            pInfo->FrameRate.TimeScale = 60U;
            pInfo->FrameRate.NumUnitsInTick = 1U;
            pInfo->FrameRate.Interlace = 0U;
        } else if (Framerate == 59U) {
            /* 59.94Hz */
            pInfo->FrameRate.TimeScale = 60000U;
            pInfo->FrameRate.NumUnitsInTick = 1001U;
            pInfo->FrameRate.Interlace = 0U;
        } else if (Framerate == 30U) {
            /* 30Hz */
            pInfo->FrameRate.TimeScale = 30U;
            pInfo->FrameRate.NumUnitsInTick = 1U;
            pInfo->FrameRate.Interlace = 0U;
        } else {
            /* 29.97Hz */
            pInfo->FrameRate.TimeScale = 30000U;
            pInfo->FrameRate.NumUnitsInTick = 1001U;
            pInfo->FrameRate.Interlace = 0U;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_LT9611UXCConfig
 *
 *  @Description:: Configure FPD display mode
 *
 *  @Input      ::
 *      pFpdConfig: configuration of FPD display mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_LT9611UXCConfig(UINT8 Mode)
{
    UINT32 RetVal;
    UINT32 ColorOrder = 0U;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming = {0};
    AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s BlankPacket = {0};

    if (Mode >= AMBA_FPD_LT9611_SERDES_NUM_MODE) {
        RetVal = ERR_ARG;
    } else {
        RetVal = ERR_NONE;
        pDispConfig = &LT9611UXC_Config[Mode];
    }

    /* Calculate MIPI DSI Timing */
    if (RetVal == (UINT32)ERR_NONE) {
        RetVal = AmbaVout_MipiDsiCalculateTiming((UINT8)pDispConfig->OutputMode, &pDispConfig->FrameTiming, &DisplayTiming, &BlankPacket);
    }

    //Config. MIPI DSI BLANK PACKET
    if (RetVal == (UINT32)ERR_NONE) {
        RetVal = AmbaVout_MipiDsiSetBlankPkt(&BlankPacket);
    }

    //Enable MIPI DSI
    if (RetVal == (UINT32)ERR_NONE) {
        RetVal = AmbaVout_MipiDsiEnable(pDispConfig->OutputMode, ColorOrder, &DisplayTiming);
    }

    //Set MIPI DSI CSC
    if (RetVal == (UINT32)ERR_NONE) {
        if (pDispConfig->OutputMode == VOUT_MIPI_DSI_MODE_422_4LANE) {
            //CSC_TYPE_IDENTITY
            AMBA_VOUT_CSC_MATRIX_s CscIdentity = {
                .Coef = {
                    [0] = { 1.00000f, 0.00000f, 0.00000f },
                    [1] = { 0.00000f, 1.00000f, 0.00000f },
                    [2] = { 0.00000f, 0.00000f, 1.00000f },
                },
                .Offset = { [0] = 0.00000f, [1] = 0.00000f, [2] = 0.00000f },
                .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
                .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
            };
            RetVal = AmbaVout_MipiDsiSetCscMatrix(&CscIdentity, 0);
        } else {
            if (pDispConfig->FrameTiming.ActiveLines >= 720U) {
                //709 YCC(Full) to RGB(Full)
                AMBA_VOUT_CSC_MATRIX_s CscBt709YccFull2GbrFull = {
                    .Coef = {
                        [0] = {  1.00000f, -0.18711f, -0.46908f },
                        [1] = {  1.00000f,  1.85788f,  0.00000f },
                        [2] = {  1.00000f,  0.00000f,  1.57503f },
                    },
                    .Offset = { [0] =   83.99f, [1] = -237.81f, [2] = -201.60f },
                    .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
                    .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
                };
                RetVal = AmbaVout_MipiDsiSetCscMatrix(&CscBt709YccFull2GbrFull, 0);
            } else {
                //601 YCC(Limited) to RGB(Full) 601
                AMBA_VOUT_CSC_MATRIX_s CscBt601YccLIimit2GbrFull = {
                    .Coef = {
                        [0] = {  1.16438f, -0.39200f, -0.81300f },
                        [1] = {  1.16438f,  2.01700f,  0.00000f },
                        [2] = {  1.16438f,  0.00000f,  1.59600f },
                    },
                    .Offset = { [0] =  135.61f, [1] = -276.81f, [2] = -222.92f },
                    .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
                    .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
                };
                RetVal = AmbaVout_MipiDsiSetCscMatrix(&CscBt601YccLIimit2GbrFull, 0);
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_LT9611UXCBacklight
 *
 *  @Description:: Turn FPD Backlight On/Off
 *
 *  @Input      ::
 *      EnableFlag: 1 = Backlight On, 0 = Backlight Off
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_LT9611UXCSetBacklight(UINT32 EnableFlag)
{
    extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
    AmbaUserGPIO_FpdCtrl(EnableFlag);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_LT9611UXCObj = {
    .FpdEnable          = FPD_LT9611UXCEnable,
    .FpdDisable         = FPD_LT9611UXCDisable,
    .FpdGetInfo         = FPD_LT9611UXCGetInfo,
    .FpdGetModeInfo     = FPD_LT9611UXCGetModeInfo,
    .FpdConfig          = FPD_LT9611UXCConfig,
    .FpdSetBacklight    = FPD_LT9611UXCSetBacklight,
    .FpdGetLinkStatus   = NULL,

    .pName = "LT9611UXC"
};
