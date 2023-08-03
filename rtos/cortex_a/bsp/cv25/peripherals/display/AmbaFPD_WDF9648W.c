/**
 *  @file AmbaFPD_WDF9648W.c
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
 *  @details Control APIs of Wintek 4:3 FPD panel WD-F9648W
 *
 */

#include "AmbaTypes.h"

#include "AmbaSPI.h"
#include "AmbaFPD.h"
#include "AmbaVOUT.h"

#include "AmbaFPD_WDF9648W.h"

static AMBA_FPD_WDF9648W_CONFIG_s *pDispConfig;
/*-----------------------------------------------------------------------------------------------*\
 * Global instance for FPD info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_WDF9648W_CTRL_s WdF9648w_Ctrl = {
    .SpiChanNo = AMBA_SPI_MASTER3,
    .SpiSlaveMask = 0x1,
    .SpiConfig = {
        .BaudRate       = 500000,                           /* Transfer BaudRate in Hz */
        .CsPolarity     = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,  /* Slave select polarity */
        .ClkMode        = AMBA_SPI_CPOL_LOW_CPHA_LOW,       /* SPI Protocol mode */
        .ClkStretch     = 0,                                /* Stretch the duration (us) from the active edge of chip select to the first edge of clock */
        .DataFrameSize  = 16,                               /* Data Frame Size in Bit */
        .FrameBitOrder  = AMBA_SPI_TRANSFER_MSB_FIRST,      /* Bit transfer order */
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * WDF9648W video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_WDF9648W_CONFIG_s WdF9648w_Config[AMBA_FPD_WDF9648W_NUM_MODE] = {
    [AMBA_FPD_WDF9648W_960_480_60HZ] = {
        .ScreenMode     = AMBA_FPD_WDF9648W_SCREEN_MODE_WIDE,
        .OutputMode     = VOUT_RGB_MODE_888_SINGLE,             /* output format */
        .ColorOrder     = VOUT_RGB_MODE_ORDER_RGB,              /* color order */
        .DisplayTiming    = {
            .PixelClkFreq       = 36000000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 152,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 30,
            .VsyncFrontPorch    = 3,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 40,
            .ActivePixels       = 960,
            .ActiveLines        = 480,
        },
        .DataLatch      = {
            .ExtClkSampleEdge       = VOUT_SIGNAL_EDGE_LOW_TO_HIGH,
            .ExtLineSyncPolarity    = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtFrameSyncPolarity   = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtDataValidPolarity   = VOUT_SIGNAL_ACTIVE_HIGH,
        }
    },
    [AMBA_FPD_WDF9648W_960_480_50HZ] = {
        .ScreenMode     = AMBA_FPD_WDF9648W_SCREEN_MODE_WIDE,
        .OutputMode     = VOUT_RGB_MODE_888_SINGLE,             /* output format */
        .ColorOrder     = VOUT_RGB_MODE_ORDER_RGB,              /* color order */
        .DisplayTiming    = {
            .PixelClkFreq       = 30030000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 152,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 30,
            .VsyncFrontPorch    = 3,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 40,
            .ActivePixels       = 960,
            .ActiveLines        = 480
        },
        .DataLatch      = {
            .ExtClkSampleEdge       = VOUT_SIGNAL_EDGE_LOW_TO_HIGH,
            .ExtLineSyncPolarity    = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtFrameSyncPolarity   = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtDataValidPolarity   = VOUT_SIGNAL_ACTIVE_HIGH,
        }
    },
    [AMBA_FPD_WDF9648W_960_480_A60HZ] = {
        .ScreenMode     = AMBA_FPD_WDF9648W_SCREEN_MODE_WIDE,
        .OutputMode     = VOUT_RGB_MODE_888_SINGLE,             /* output format */
        .ColorOrder     = VOUT_RGB_MODE_ORDER_RGB,              /* color order */
        .DisplayTiming    = {
            .PixelClkFreq       = 36036000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 152,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 30,
            .VsyncFrontPorch    = 3,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 40,
            .ActivePixels       = 960,
            .ActiveLines        = 480,
        },
        .DataLatch      = {
            .ExtClkSampleEdge       = VOUT_SIGNAL_EDGE_LOW_TO_HIGH,
            .ExtLineSyncPolarity    = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtFrameSyncPolarity   = VOUT_SIGNAL_ACTIVE_LOW,
            .ExtDataValidPolarity   = VOUT_SIGNAL_ACTIVE_HIGH,
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WdF9648w_Write
 *
 *  @Description:: Write WD-F9648W register
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
static UINT32 WdF9648w_Write(UINT16 Offset, UINT16 Data)
{
    UINT16 SpiCmd = (UINT16)(Offset << 9U) | (Data & 0xFFU);

    return AmbaSPI_MasterTransferD16(WdF9648w_Ctrl.SpiChanNo, WdF9648w_Ctrl.SpiSlaveMask, &WdF9648w_Ctrl.SpiConfig, 1U, &SpiCmd, NULL, NULL, 500U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WdF9648w_SetInputControl
 *
 *  @Description:: Select DE/Sync(HV) mode
 *
 *  @Input      ::
 *      InputCtrl:  Data input mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 WdF9648w_SetInputControl(UINT8 InputCtrl)
{
    const AMBA_VOUT_DATA_LATCH_CONFIG_s *pDataLatch = &pDispConfig->DataLatch;
    UINT16 RegVal = 0x00U;

    if (pDispConfig->ScreenMode == AMBA_FPD_WDF9648W_SCREEN_MODE_NARROW) {
        RegVal |= 0x80U;
    }

    if (InputCtrl == AMBA_FPD_WDF9648W_CTRL_DE) {
        RegVal |= 0x10U;
        if (pDataLatch->ExtDataValidPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            RegVal |= 0x08U;
        }
    } else if (InputCtrl == AMBA_FPD_WDF9648W_CTRL_HV_SYNC) {
        if (pDataLatch->ExtFrameSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            RegVal |= 0x04U;
        }
        if (pDataLatch->ExtLineSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            RegVal |= 0x02U;
        }
    } else {
        /* For MisraC */
    }

    if (pDataLatch->ExtClkSampleEdge == VOUT_SIGNAL_EDGE_HIGH_TO_LOW) {
        RegVal |= 0x01U;
    }

    return WdF9648w_Write(0x02U, RegVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: WdF9648w_SetInputFormat
 *
 *  @Description:: Set input format
 *
 *  @Input      ::
 *      InputFormat:    Input format selection
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 WdF9648w_SetInputFormat(UINT8 InputFormat)
{
    UINT16 RegVal = 0x03U;

    RegVal |= ((UINT16) InputFormat << 4U);

    return WdF9648w_Write(0x03U, RegVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Wdf9648wEnable
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
static UINT32 FPD_Wdf9648wEnable(void)
{
    const UINT16 *pSpiCmdBuf;
    UINT32 NumSpiCmd;
    UINT32 RetVal = ERR_NONE;

    /*-----------------------------------------------------------------------------------------------*\
    * WDF9648W video mode configuration
    \*-----------------------------------------------------------------------------------------------*/
    const UINT16 WdF9648w_DefaultCtrl[] = {
        0x2280U, 0x0C1DU, 0x0E27U, 0x280EU, 0x2621U, 0x2A03U, 0x2C05U, 0x2E29U, 0x3023U, 0x3200U
    };

    const UINT16 WdF9648w_DefaultGamma[] = {
        0x4219U, 0x441DU, 0x460CU, 0x4805U, 0x4A03U, 0x4C03U, 0x4E03U, 0x5005U, 0x5207U, 0x5403U, 0x5602U, 0x580FU,
        0x5A1CU, 0x5C1DU, 0x5E0EU, 0x6005U, 0x6205U, 0x6404U, 0x6604U, 0x6805U, 0x6A07U, 0x6C05U, 0x6E02U, 0x7000U
    };

    /*-----------------------------------------------------------------------*\
     * Configure FPD registers by SPI control interface.
    \*-----------------------------------------------------------------------*/
    (void)WdF9648w_Write(0x05U, 0U);       /* GRB=0 : Global Reset */

    (void)WdF9648w_SetInputFormat(AMBA_FPD_WDF9648W_8BIT_RGB_DA_MODE);

    pSpiCmdBuf = WdF9648w_DefaultCtrl;
    NumSpiCmd = sizeof(WdF9648w_DefaultCtrl) / sizeof(UINT16);

    if (AmbaSPI_MasterTransferD16(WdF9648w_Ctrl.SpiChanNo, WdF9648w_Ctrl.SpiSlaveMask, &WdF9648w_Ctrl.SpiConfig, NumSpiCmd, pSpiCmdBuf, NULL, NULL, 800U) != ERR_NONE) {
        RetVal = ERR_NA;
    } else {
        pSpiCmdBuf = WdF9648w_DefaultGamma;
        NumSpiCmd = sizeof(WdF9648w_DefaultGamma) / sizeof(UINT16);
        (void)AmbaSPI_MasterTransferD16(WdF9648w_Ctrl.SpiChanNo, WdF9648w_Ctrl.SpiSlaveMask, &WdF9648w_Ctrl.SpiConfig, NumSpiCmd, pSpiCmdBuf, NULL, NULL, 800U);

        (void)WdF9648w_SetInputControl(AMBA_FPD_WDF9648W_CTRL_DE);
        (void)WdF9648w_Write(0x01U, (UINT16) 0x01U);        /* Operating mode */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Wdf9648wDisable
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
static UINT32 FPD_Wdf9648wDisable(void)
{
    return WdF9648w_Write(0x01U, (UINT16) 0x00U); /* Standby mode */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Wdf9648wGetInfo
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
static UINT32 FPD_Wdf9648wGetInfo(AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    if (pDispConfig == NULL) {
        RetVal = ERR_ARG;
    } else {
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 4U;
        pInfo->AspectRatio.Y = 3U;
    }
    return RetVal;
}

static UINT32 FPD_Wdf9648wGetModeInfo(UINT8 mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    if ( (mode >= AMBA_FPD_WDF9648W_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &WdF9648w_Config[mode];
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 4U;
        pInfo->AspectRatio.Y = 3U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Wdf9648wConfig
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
static UINT32 FPD_Wdf9648wConfig(UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 ColorOrder = 0;

    if (Mode >= AMBA_FPD_WDF9648W_NUM_MODE) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &WdF9648w_Config[Mode];
        ColorOrder = ((UINT32)pDispConfig->ColorOrder << 16) | (UINT32)pDispConfig->ColorOrder;
        RetVal = AmbaVout_DigiRgbEnable(pDispConfig->OutputMode, ColorOrder,  &pDispConfig->DataLatch, &pDispConfig->DisplayTiming);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Wdf9648wSetBacklight
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
static UINT32 FPD_Wdf9648wSetBacklight(UINT32 EnableFlag)
{
    extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
    AmbaUserGPIO_FpdCtrl(EnableFlag);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_WdF9648wObj = {
    .FpdEnable          = FPD_Wdf9648wEnable,
    .FpdDisable         = FPD_Wdf9648wDisable,
    .FpdGetInfo         = FPD_Wdf9648wGetInfo,
    .FpdGetModeInfo     = FPD_Wdf9648wGetModeInfo,
    .FpdConfig          = FPD_Wdf9648wConfig,
    .FpdSetBacklight    = FPD_Wdf9648wSetBacklight,
    .FpdGetLinkStatus   = NULL,

    .pName = "WDF9648W"
};
