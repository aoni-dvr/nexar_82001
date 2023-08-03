/**
 *  @file AmbaFPD_C101EAN01_0.c
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
 *  @details Control APIs of AUO FPD panel C101EAN01.0
 *
 */

#include "AmbaTypes.h"

#include "AmbaFPD.h"
#include "AmbaVOUT.h"
#include "AmbaFPD_C101EAN01_0.h"

static AMBA_FPD_C101EAN01_CONFIG_s *pDispConfig;

/*-----------------------------------------------------------------------------------------------*\
 * C101EAN01 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_C101EAN01_CONFIG_s C101EAN01_Config[AMBA_FPD_C101EAN01_NUM_MODE] = {
    [AMBA_FPD_C101EAN01_1280_720_60HZ] = {
        .OutputMode     = VOUT_FPD_LINK_MODE_24BIT,
        .BitOrder       = VOUT_FPD_LINK_ORDER_LSB,
        .DisplayTiming    = {
            .PixelClkFreq       = 68573826,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 64,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 134,
            .VsyncFrontPorch    = 1,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 50,
            .ActivePixels       = 1280,
            .ActiveLines        = 720,
        },
        .DataValidPolarity      = VOUT_SIGNAL_ACTIVE_HIGH,
    },
    [AMBA_FPD_C101EAN01_1280_720_A60HZ] = {
        .OutputMode     = VOUT_FPD_LINK_MODE_24BIT,
        .BitOrder       = VOUT_FPD_LINK_ORDER_LSB,
        .DisplayTiming    = {
            .PixelClkFreq       = 68642400,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 64,
            .HsyncPulseWidth    = 2,
            .HsyncBackPorch     = 134,
            .VsyncFrontPorch    = 1,
            .VsyncPulseWidth    = 2,
            .VsyncBackPorch     = 50,
            .ActivePixels       = 1280,
            .ActiveLines        = 720,
        },
        .DataValidPolarity      = VOUT_SIGNAL_ACTIVE_HIGH,
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_C101EAN01Enable
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
static UINT32 FPD_C101EAN01Enable(void)
{
    UINT32 RetVal = ERR_NONE;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_C101EAN01Disable
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
static UINT32 FPD_C101EAN01Disable(void)
{
    UINT32 RetVal = ERR_NONE;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_C101EAN01GetInfo
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
static UINT32 FPD_C101EAN01GetInfo(AMBA_FPD_INFO_s *pInfo)
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

static UINT32 FPD_C101EAN01GetModeInfo(UINT8 mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    if ( (mode >= AMBA_FPD_C101EAN01_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &C101EAN01_Config[mode];
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_C101EAN01Config
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
static UINT32 FPD_C101EAN01Config(UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;

    if (Mode >= AMBA_FPD_C101EAN01_NUM_MODE) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &C101EAN01_Config[Mode];
        RetVal = AmbaVout_FpdLinkEnable(pDispConfig->OutputMode, pDispConfig->BitOrder, pDispConfig->DataValidPolarity, &pDispConfig->DisplayTiming);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_C101EAN01SetBacklight
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
static UINT32 FPD_C101EAN01SetBacklight(UINT32 EnableFlag)
{
    extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
    AmbaUserGPIO_FpdCtrl(EnableFlag);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_C101EAN01Obj = {
    .FpdEnable          = FPD_C101EAN01Enable,
    .FpdDisable         = FPD_C101EAN01Disable,
    .FpdGetInfo         = FPD_C101EAN01GetInfo,
    .FpdGetModeInfo     = FPD_C101EAN01GetModeInfo,
    .FpdConfig          = FPD_C101EAN01Config,
    .FpdSetBacklight    = FPD_C101EAN01SetBacklight,
    .FpdGetLinkStatus   = NULL,

    .pName = "C101EAN01"
};
