/**
 *  @file AmbaFPD_ZS095BH3001A8H3_BII.c
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
 *  @details Control APIs of Shenzhen Zhongshen Optoelectronics Co. Ltd FPD panel ZS095BH3001A8H3-BII
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaGPIO.h"
#include "AmbaSPI.h"
#include "AmbaFPD.h"
#include "AmbaVOUT.h"
#include "AmbaFPD_ZS095BH3001A8H3_BII.h"

static AMBA_FPD_ZS095BH3001A8H3_CONFIG_s *pDispConfig;

#define LCD_STANDBY_PIN     GPIO_PIN_37 /* Active-Low */
#define LCD_RESET_PIN       GPIO_PIN_38 /* Active-Low */

/*-----------------------------------------------------------------------------------------------*\
 * ZS095BH3001A8H3 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_ZS095BH3001A8H3_CONFIG_s ZS095BH3001A8H3_Config[AMBA_FPD_ZS095BH_NUM_MODE] = {
    [AMBA_FPD_ZS095BH_1920_480_60HZ] = {
        .OutputMode     = VOUT_FPD_LINK_MODE_24BIT,
        .BitOrder       = VOUT_FPD_LINK_ORDER_LSB,
        .DisplayTiming    = {
            .PixelClkFreq       = 63360000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 67,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 13,
            .VsyncFrontPorch    = 13,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 33,
            .ActivePixels       = 1920,
            .ActiveLines        = 480,
        },
        .DataValidPolarity      = VOUT_SIGNAL_ACTIVE_HIGH,
    },
    [AMBA_FPD_ZS095BH_1920_480_A60HZ] = {
        .OutputMode     = VOUT_FPD_LINK_MODE_24BIT,
        .BitOrder       = VOUT_FPD_LINK_ORDER_LSB,
        .DisplayTiming    = {
            .PixelClkFreq       = 63423360,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 67,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 13,
            .VsyncFrontPorch    = 13,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 33,
            .ActivePixels       = 1920,
            .ActiveLines        = 480,
        },
        .DataValidPolarity      = VOUT_SIGNAL_ACTIVE_HIGH,
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ZS095BH3001A8H3_Write
 *
 *  @Description:: Write ZS095BH3001A8H3 register
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
static UINT32 ZS095BH3001A8H3_Write(UINT16 Offset, UINT16 Data)
{
    UINT16 SpiCmd = (UINT16) (Offset << 8U) | (Data & 0xFFU);
    AMBA_FPD_ZS095BH3001A8H3_CTRL_s ZS095BH3001A8H3_Ctrl;

    ZS095BH3001A8H3_Ctrl.SpiChanNo = AMBA_SPI_MASTER0;
    ZS095BH3001A8H3_Ctrl.SpiSlaveMask = 0x1;

    ZS095BH3001A8H3_Ctrl.SpiConfig.BaudRate = 1000000;
    ZS095BH3001A8H3_Ctrl.SpiConfig.CsPolarity = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW;
    ZS095BH3001A8H3_Ctrl.SpiConfig.ClkMode = AMBA_SPI_CPOL_HIGH_CPHA_HIGH;
    ZS095BH3001A8H3_Ctrl.SpiConfig.ClkStretch = 0;
    ZS095BH3001A8H3_Ctrl.SpiConfig.DataFrameSize = 16;
    ZS095BH3001A8H3_Ctrl.SpiConfig.FrameBitOrder = AMBA_SPI_TRANSFER_MSB_FIRST;

    return AmbaSPI_MasterTransferD16(ZS095BH3001A8H3_Ctrl.SpiChanNo, ZS095BH3001A8H3_Ctrl.SpiSlaveMask, &ZS095BH3001A8H3_Ctrl.SpiConfig, 1U, &SpiCmd, NULL, NULL, 500U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_ZS095BH3001A8H3Enable
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
static UINT32 FPD_ZS095BH3001A8H3Enable(void)
{
    UINT32 RetVal;

    (void)AmbaGPIO_SetFuncGPO(LCD_RESET_PIN, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(10);
    (void)AmbaGPIO_SetFuncGPO(LCD_STANDBY_PIN, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(100);

    /* Force LVDS data format as JEIDA */
    RetVal = ZS095BH3001A8H3_Write(0x00, 0x07);

    if (RetVal == ERR_NONE) {
        RetVal = ZS095BH3001A8H3_Write(0x01, 0x00);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_ZS095BH3001A8H3Disable
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
static UINT32 FPD_ZS095BH3001A8H3Disable(void)
{
    UINT32 RetVal;

    RetVal = AmbaGPIO_SetFuncGPO(LCD_STANDBY_PIN, AMBA_GPIO_LEVEL_HIGH);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_ZS095BH3001A8H3GetInfo
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
static UINT32 FPD_ZS095BH3001A8H3GetInfo(AMBA_FPD_INFO_s *pInfo)
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

static UINT32 FPD_ZS095BH3001A8H3GetModeInfo(UINT8 mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    if ( (mode >= AMBA_FPD_ZS095BH_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &ZS095BH3001A8H3_Config[mode];
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_ZS095BH3001A8H3Config
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
static UINT32 FPD_ZS095BH3001A8H3Config(UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;

    if (Mode >= AMBA_FPD_ZS095BH_NUM_MODE) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &ZS095BH3001A8H3_Config[Mode];
        RetVal = AmbaVout_FpdLinkEnable(pDispConfig->OutputMode, pDispConfig->BitOrder, pDispConfig->DataValidPolarity, &pDispConfig->DisplayTiming);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_ZS095BH3001A8H3SetBacklight
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
static UINT32 FPD_ZS095BH3001A8H3SetBacklight(UINT32 EnableFlag)
{
    extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
    AmbaUserGPIO_FpdCtrl(EnableFlag);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_ZS095BH3001A8H3Obj = {
    .FpdEnable          = FPD_ZS095BH3001A8H3Enable,
    .FpdDisable         = FPD_ZS095BH3001A8H3Disable,
    .FpdGetInfo         = FPD_ZS095BH3001A8H3GetInfo,
    .FpdGetModeInfo     = FPD_ZS095BH3001A8H3GetModeInfo,
    .FpdConfig          = FPD_ZS095BH3001A8H3Config,
    .FpdSetBacklight    = FPD_ZS095BH3001A8H3SetBacklight,
    .FpdGetLinkStatus   = NULL,

    .pName = "ZS095BH3001A8H3"
};
