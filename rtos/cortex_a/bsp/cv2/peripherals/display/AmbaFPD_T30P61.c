/**
 *  @file AmbaFPD_T30P61.c
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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
 *  @details Control APIs of Shenzhen RZW Display FPD panel T30P61
 *
 */

#include "AmbaTypes.h"

#include "AmbaSPI.h"
#include "AmbaFPD.h"
#include "AmbaVOUT.h"
#include "AmbaFPD_T30P61.h"

static AMBA_FPD_T30P61_CONFIG_s *pDispConfig;

/*-----------------------------------------------------------------------------------------------*\
 * T30P61 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_T30P61_CONFIG_s T30P61_Config[AMBA_FPD_T30P61_NUM_MODE] = {
    [AMBA_FPD_T30P61_960_240_60HZ] = {
        .ScreenMode     = AMBA_FPD_T30P61_SCREEN_MODE_WIDE,
        .OutputMode     = VOUT_RGB_MODE_888_SINGLE,
        .EvenLineColor  = VOUT_RGB_MODE_ORDER_RGB,
        .OddLineColor   = VOUT_RGB_MODE_ORDER_GBR,
        .DisplayTiming    = {
            .PixelClkFreq       = 26948572,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 686,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 68,
            .VsyncFrontPorch    = 1,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 19,
            .ActivePixels       = 960,
            .ActiveLines        = 240,
        },
        .DataLatch      = {
            .ExtClkSampleEdge       = VOUT_SIGNAL_ACTIVE_HIGH,
            .ExtLineSyncPolarity    = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtFrameSyncPolarity   = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtDataValidPolarity   = VOUT_SIGNAL_ACTIVE_HIGH,
        }
    },
    [AMBA_FPD_T30P61_960_240_50HZ] = {
        .ScreenMode     = AMBA_FPD_T30P61_SCREEN_MODE_WIDE,
        .OutputMode     = VOUT_RGB_MODE_888_SINGLE,
        .EvenLineColor  = VOUT_RGB_MODE_ORDER_RGB,
        .OddLineColor   = VOUT_RGB_MODE_ORDER_GBR,
        .DisplayTiming    = {
            .PixelClkFreq       = 22500000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 686,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 68,
            .VsyncFrontPorch    = 1,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 19,
            .ActivePixels       = 960,
            .ActiveLines        = 240,
        },
        .DataLatch      = {
            .ExtClkSampleEdge       = VOUT_SIGNAL_ACTIVE_HIGH,
            .ExtLineSyncPolarity    = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtFrameSyncPolarity   = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtDataValidPolarity   = VOUT_SIGNAL_ACTIVE_HIGH,
        }
    },
    [AMBA_FPD_T30P61_960_240_A60HZ] = {
        .ScreenMode     = AMBA_FPD_T30P61_SCREEN_MODE_WIDE,
        .OutputMode     = VOUT_RGB_MODE_888_SINGLE,
        .EvenLineColor  = VOUT_RGB_MODE_ORDER_RGB,
        .OddLineColor   = VOUT_RGB_MODE_ORDER_GBR,
        .DisplayTiming    = {
            .PixelClkFreq       = 26975520,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 686,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 68,
            .VsyncFrontPorch    = 1,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 19,
            .ActivePixels       = 960,
            .ActiveLines        = 240,
        },
        .DataLatch      = {
            .ExtClkSampleEdge       = VOUT_SIGNAL_ACTIVE_HIGH,
            .ExtLineSyncPolarity    = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtFrameSyncPolarity   = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtDataValidPolarity   = VOUT_SIGNAL_ACTIVE_HIGH,
        }
    },
    [AMBA_FPD_T30P61_960_240_A30HZ] = {
        .ScreenMode     = AMBA_FPD_T30P61_SCREEN_MODE_WIDE,
        .OutputMode     = VOUT_RGB_MODE_888_SINGLE,
        .EvenLineColor  = VOUT_RGB_MODE_ORDER_RGB,
        .OddLineColor   = VOUT_RGB_MODE_ORDER_GBR,
        .DisplayTiming    = {
            .PixelClkFreq       = 13487760,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 686,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 68,
            .VsyncFrontPorch    = 1,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 19,
            .ActivePixels       = 960,
            .ActiveLines        = 240,
        },
        .DataLatch      = {
            .ExtClkSampleEdge       = VOUT_SIGNAL_ACTIVE_HIGH,
            .ExtLineSyncPolarity    = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtFrameSyncPolarity   = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtDataValidPolarity   = VOUT_SIGNAL_ACTIVE_HIGH,
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: T30P61_Write
 *
 *  @Description:: Write T30P61 register
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
static UINT32 T30P61_Write(UINT16 Offset, UINT16 Data)
{
    UINT16 SpiCmd = (UINT16) (Offset << 8U) | (Data & 0xFFU);
    AMBA_FPD_T30P61_CTRL_s T30P61_Ctrl;

    T30P61_Ctrl.SpiChanNo = AMBA_SPI_MASTER3;
    T30P61_Ctrl.SpiSlaveMask = 0x1;

    T30P61_Ctrl.SpiConfig.BaudRate = 500000;
    T30P61_Ctrl.SpiConfig.CsPolarity = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW;
    T30P61_Ctrl.SpiConfig.ClkMode = AMBA_SPI_CPOL_HIGH_CPHA_HIGH;
    T30P61_Ctrl.SpiConfig.ClkStretch = 0;
    T30P61_Ctrl.SpiConfig.DataFrameSize = 16;
    T30P61_Ctrl.SpiConfig.FrameBitOrder = AMBA_SPI_TRANSFER_MSB_FIRST;

    return AmbaSPI_MasterTransferD16(T30P61_Ctrl.SpiChanNo, T30P61_Ctrl.SpiSlaveMask, &T30P61_Ctrl.SpiConfig, 1U, &SpiCmd, NULL, NULL, 500U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_T30P61Enable
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
static UINT32 FPD_T30P61Enable(void)
{
    UINT32 RetVal;

    /*-----------------------------------------------------------------------*\
     * Configure FPD registers by SPI control interface.
    \*-----------------------------------------------------------------------*/
    RetVal = T30P61_Write(0x05U, 0x1CU);        /* Reset all registers */

    if (RetVal == ERR_NONE) {
        (void)T30P61_Write(0x05U, 0x5CU);       /* Normal operation */
        (void)T30P61_Write(0x2BU, 0x01U);       /* Set to normal mode */
        RetVal = T30P61_Write(0x03U, 0x20U);    /* Set brightness to 0x20 */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_T30P61Disable
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
static UINT32 FPD_T30P61Disable(void)
{
    return T30P61_Write(0x2BU, (UINT16) 0x00U); /* Enter standby mode */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_T30P61GetInfo
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
static UINT32 FPD_T30P61GetInfo(AMBA_FPD_INFO_s *pInfo)
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

static UINT32 FPD_T30P61GetModeInfo(UINT8 mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    if ( (mode >= AMBA_FPD_T30P61_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &T30P61_Config[mode];
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_T30P61Config
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
static UINT32 FPD_T30P61Config(UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 ColorOrder = 0U;

    if (Mode >= AMBA_FPD_T30P61_NUM_MODE) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &T30P61_Config[Mode];
        ColorOrder = ((UINT32)pDispConfig->OddLineColor << 16U) | (UINT32)pDispConfig->EvenLineColor;
        RetVal = AmbaVout_DigiRgbEnable(pDispConfig->OutputMode, ColorOrder,  &pDispConfig->DataLatch, &pDispConfig->DisplayTiming);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_T30P61SetBacklight
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
static UINT32 FPD_T30P61SetBacklight(UINT32 EnableFlag)
{
    extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
    AmbaUserGPIO_FpdCtrl(EnableFlag);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_T30P61Obj = {
    .FpdEnable          = FPD_T30P61Enable,
    .FpdDisable         = FPD_T30P61Disable,
    .FpdGetInfo         = FPD_T30P61GetInfo,
    .FpdGetModeInfo     = FPD_T30P61GetModeInfo,
    .FpdConfig          = FPD_T30P61Config,
    .FpdSetBacklight    = FPD_T30P61SetBacklight,
    .FpdGetLinkStatus   = NULL,

    .pName = "T30P61"
};
