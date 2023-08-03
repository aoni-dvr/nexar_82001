/**
 *  @file AmbaFPD_Y85006.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Control APIs of DZXtech Display FPD panel Y85006 (mipi dsi 1-lane)
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaGPIO.h"
#include "AmbaFPD.h"
#include "AmbaVOUT.h"
#include "AmbaFPD_Y85006.h"

#define AMBA_LCD_BACKLIGHT   GPIO_PIN_41
#define AMBA_LCD_RESET       GPIO_PIN_42

static AMBA_FPD_Y85006_CONFIG_s *pDispConfig;

/*-----------------------------------------------------------------------------------------------*\
 * Y85006 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_Y85006_CONFIG_s Y85006_Config[AMBA_FPD_Y85006_NUM_MODE] = {
    [AMBA_FPD_Y85006_320_480_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_565_1LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 11892108,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 49,
            .HsyncPulseWidth    = 10,
            .HsyncBackPorch     = 21,
            .VsyncFrontPorch    = 9,
            .VsyncPulseWidth    = 3,
            .VsyncBackPorch     = 4,
            .ActivePixels       = 320,
            .ActiveLines        = 480,
        },
        .BlankPacket = {
            .LenHBP     = 28,
            .LenHSA     = 10,
            .LenHFP     = 90,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_Y85006_320_480_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_565_1LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 11904000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 49,
            .HsyncPulseWidth    = 10,
            .HsyncBackPorch     = 21,
            .VsyncFrontPorch    = 9,
            .VsyncPulseWidth    = 3,
            .VsyncBackPorch     = 4,
            .ActivePixels       = 320,
            .ActiveLines        = 480,
        },
        .BlankPacket = {
            .LenHBP     = 28,
            .LenHSA     = 10,
            .LenHFP     = 90,
            .LenBLLP    = 0,
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Y85006_Init
 *
 *  @Description:: Write Y85006 register
 *
 *  @Input      ::
 *      Offset:     Register offset
 *      Data:       Register data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void Y85006_Init(void)
{
    UINT32 Parameter[4];

    /* Positive gamma control (0xE0)*/
    Parameter[0] = 0x080E0400;
    Parameter[1] = 0x79400A17;
    Parameter[2] = 0x0A0E074D;
    Parameter[3] = 0x000F1D1A;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xE0, 16, (UINT8 *)Parameter);

    /* Negative gamma control (0xE1)*/
    Parameter[0] = 0x021F1B00;
    Parameter[1] = 0x34320510;
    Parameter[2] = 0x090A0243;
    Parameter[3] = 0x000F3733;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xE1, 16, (UINT8 *)Parameter);

    /* Power control 1 (0xC0: 2 parameters) */
    Parameter[0] = 0x00001618;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC0, 2, (UINT8 *)Parameter);

    /* Power control 2 (0xC1: 1 parameter): */
    Parameter[0] = 0x41;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC1, 2, (UINT8 *)Parameter);

    /* VCOM control (0xC5) */
    Parameter[0] = 0x00801200;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC5, 3, (UINT8 *)Parameter);

    /* Memory access (0x36) */
    Parameter[0] = 0x48;     //0x48-> Bit7:Page Address Order, Bit3:BGR Order .
    (void)AmbaVout_MipiDsiSendDcsWrite(0x36, 1, (UINT8 *)Parameter);

    /* Interface pixel format (0x3A) */
    Parameter[0] = 0x55;    //0x77->24 bits/pixel, 0x66->18 bits/pixel, 0x55->16 bits/pixel.
    (void)AmbaVout_MipiDsiSendDcsWrite(0x3A, 1, (UINT8 *)Parameter);

    /* Frame rate (0xB1: 60Hz) */
    Parameter[0] = 0x11A1;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xB1, 2, (UINT8 *)Parameter);

    /* Display inversion control (0xB4) */
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xB4, 1, (UINT8 *)Parameter);

    /* Set Image Function */
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xE9, 1, (UINT8 *)Parameter);

    /* Adjust control 3 */
    Parameter[0] = 0x822C51A9U;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xF7, 4, (UINT8 *)Parameter);

    /* Exit sleep mode */
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_EXIT_SLEEP_MODE, 0, NULL);
    (void)AmbaKAL_TaskSleep(120);

    /* Display on */
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_DISPLAY_ON, 0, NULL);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85006Enable
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
static UINT32 FPD_Y85006Enable(void)
{
    UINT32 RetVal = ERR_NONE;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85006Disable
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
static UINT32 FPD_Y85006Disable(void)
{
    UINT32 RetVal = ERR_NONE;

    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_DISPLAY_OFF, 0, NULL);
    (void)AmbaGPIO_SetFuncGPO(AMBA_LCD_BACKLIGHT, AMBA_GPIO_LEVEL_LOW);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85006GetInfo
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
static UINT32 FPD_Y85006GetInfo(AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    if (pDispConfig == NULL) {
        RetVal = ERR_ARG;
    } else {
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85006GetModeInfo
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
static UINT32 FPD_Y85006GetModeInfo(UINT8 mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    if ( (mode >= AMBA_FPD_Y85006_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &Y85006_Config[mode];
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85006Config
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
static UINT32 FPD_Y85006Config(UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 ColorOrder = 0U;

    if (Mode >= AMBA_FPD_Y85006_NUM_MODE) {
        RetVal = ERR_ARG;
    } else {
        /* Reset LCD */
        (void)AmbaGPIO_SetFuncGPO(AMBA_LCD_RESET, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(10);
        (void)AmbaGPIO_SetFuncGPO(AMBA_LCD_RESET, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(10);
        (void)AmbaGPIO_SetFuncGPO(AMBA_LCD_RESET, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(1000);

        pDispConfig = &Y85006_Config[Mode];

        RetVal = AmbaVout_MipiDsiSetBlankPkt(&pDispConfig->BlankPacket);

        if (RetVal == OK) {
            RetVal = AmbaVout_MipiDsiEnable(pDispConfig->OutputMode, ColorOrder,  &pDispConfig->DisplayTiming);

            /* initial sequence for panel Y85006 */
            Y85006_Init();
        }

        /* Turn on backlight */
        (void)AmbaGPIO_SetFuncGPO(AMBA_LCD_BACKLIGHT, AMBA_GPIO_LEVEL_HIGH);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85006SetBacklight
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
static UINT32 FPD_Y85006SetBacklight(UINT32 EnableFlag)
{
    extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
    AmbaUserGPIO_FpdCtrl(EnableFlag);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_Y85006Obj = {
    .FpdEnable          = FPD_Y85006Enable,
    .FpdDisable         = FPD_Y85006Disable,
    .FpdGetInfo         = FPD_Y85006GetInfo,
    .FpdGetModeInfo     = FPD_Y85006GetModeInfo,
    .FpdConfig          = FPD_Y85006Config,
    .FpdSetBacklight    = FPD_Y85006SetBacklight,
    .FpdGetLinkStatus   = NULL,

    .pName = "Y85006"
};
