// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 * @file AmbaRTSL_VOUT_TV.c
 *
 * @copyright Copyright (c) 2021 Ambarella International LP
 *
 * @details VOUT TV RTSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaRTSL_VOUT.h"
#include "AmbaCSL_VOUT.h"

#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
#include "AmbaMisraFix.h"
#include "AmbaRTSL_PLL.h"
static AMBA_VOUT_DISPLAY1_CONFIG_REG_s AmbaVOUT_Display1Config __attribute__((aligned(128)));
static UINT32 AmbaVOUT_TveConfigData[128] __attribute__((aligned(128)));
#endif

#ifdef CONFIG_QNX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hw/ambarella_clk.h"
#include "AmbaMisraFix.h"
#include "AmbaSYS.h"
AMBA_VOUT_DISPLAY1_CONFIG_REG_s AmbaVOUT_Display1Config;
UINT32 AmbaVOUT_TveConfigData[128];
#endif

#ifdef CONFIG_LINUX
#include "osal.h"
#include <linux/delay.h>
#define AmbaWrap_memcpy OSAL_memcpy
#define AmbaWrap_memset OSAL_memset
AMBA_VOUT_DISPLAY1_CONFIG_REG_s AmbaVOUT_Display1Config;
UINT32 AmbaVOUT_TveConfigData[128];
#endif

static inline void SetRegValue(const volatile void *pRegAddr, UINT32 RegVal)
{
    UINT32 *pReg;
#ifndef CONFIG_LINUX
    AmbaMisra_TypeCast(&pReg, &pRegAddr);
#else
    OSAL_memcpy(&pReg, &pRegAddr, sizeof(pRegAddr));
#endif
    *pReg = RegVal;
}

/**
 *  VOUT_SetAnalogCsc - Assign color space conversion parameters for analog vout interface
 *  @param[in] pVoutCscData Color space conversion data
 */
static void VOUT_SetAnalogCsc(const UINT16 *pVoutCscData)
{
    AmbaVOUT_Display1Config.AnalogCSC0.CoefA0 = pVoutCscData[0];  /* Coef[0][0] */
    AmbaVOUT_Display1Config.AnalogCSC0.CoefA4 = pVoutCscData[3];  /* Coef[1][0] */
    AmbaVOUT_Display1Config.AnalogCSC1.CoefA8 = pVoutCscData[6];  /* Coef[2][0] */
    AmbaVOUT_Display1Config.AnalogCSC1.ConstB0 = pVoutCscData[9];
    AmbaVOUT_Display1Config.AnalogCSC2.ConstB1 = pVoutCscData[10];
    AmbaVOUT_Display1Config.AnalogCSC2.ConstB2 = pVoutCscData[11];
    AmbaVOUT_Display1Config.AnalogCSC3.Output0ClampLow  = pVoutCscData[12];
    AmbaVOUT_Display1Config.AnalogCSC3.Output0ClampHigh = pVoutCscData[13];
    AmbaVOUT_Display1Config.AnalogCSC4.Output1ClampLow  = pVoutCscData[14];
    AmbaVOUT_Display1Config.AnalogCSC4.Output1ClampHigh = pVoutCscData[15];
    AmbaVOUT_Display1Config.AnalogCSC5.Output2ClampLow  = pVoutCscData[16];
    AmbaVOUT_Display1Config.AnalogCSC5.Output2ClampHigh = pVoutCscData[17];
}

/**
 *  VOUT_SetDigital1Csc - Assign color space conversion parameters for digital vout interface
 *  @param[in] pVoutCscData Color space conversion data
 */
static void VOUT_SetDigital1Csc(const UINT16 *pVoutCscData)
{
    AmbaVOUT_Display1Config.DigitalCSC0.CoefA0 = pVoutCscData[0];
    AmbaVOUT_Display1Config.DigitalCSC0.CoefA1 = pVoutCscData[1];
    AmbaVOUT_Display1Config.DigitalCSC1.CoefA2 = pVoutCscData[2];
    AmbaVOUT_Display1Config.DigitalCSC1.CoefA3 = pVoutCscData[3];
    AmbaVOUT_Display1Config.DigitalCSC2.CoefA4 = pVoutCscData[4];
    AmbaVOUT_Display1Config.DigitalCSC2.CoefA5 = pVoutCscData[5];
    AmbaVOUT_Display1Config.DigitalCSC3.CoefA6 = pVoutCscData[6];
    AmbaVOUT_Display1Config.DigitalCSC3.CoefA7 = pVoutCscData[7];
    AmbaVOUT_Display1Config.DigitalCSC4.CoefA8 = pVoutCscData[8];
    AmbaVOUT_Display1Config.DigitalCSC4.ConstB0 = pVoutCscData[9];
    AmbaVOUT_Display1Config.DigitalCSC5.ConstB1 = pVoutCscData[10];
    AmbaVOUT_Display1Config.DigitalCSC5.ConstB2 = pVoutCscData[11];
    AmbaVOUT_Display1Config.DigitalCSC6.Output0ClampLow  = pVoutCscData[12];
    AmbaVOUT_Display1Config.DigitalCSC6.Output0ClampHigh = pVoutCscData[13];
    AmbaVOUT_Display1Config.DigitalCSC7.Output1ClampLow  = pVoutCscData[14];
    AmbaVOUT_Display1Config.DigitalCSC7.Output1ClampHigh = pVoutCscData[15];
    AmbaVOUT_Display1Config.DigitalCSC8.Output2ClampLow  = pVoutCscData[16];
    AmbaVOUT_Display1Config.DigitalCSC8.Output2ClampHigh = pVoutCscData[17];
}

/**
 *  VOUT_SetHdmiCsc - Assign color space conversion parameters for hdmi vout interface
 *  @param[in] pHdmiCscData Color space conversion data
 */
static void VOUT_SetHdmiCsc(const UINT16 *pHdmiCscData)
{
    AmbaVOUT_Display1Config.HdmiCSC0.CoefA0 = pHdmiCscData[0];
    AmbaVOUT_Display1Config.HdmiCSC0.CoefA1 = pHdmiCscData[1];
    AmbaVOUT_Display1Config.HdmiCSC1.CoefA2 = pHdmiCscData[2];
    AmbaVOUT_Display1Config.HdmiCSC1.CoefA3 = pHdmiCscData[3];
    AmbaVOUT_Display1Config.HdmiCSC2.CoefA4 = pHdmiCscData[4];
    AmbaVOUT_Display1Config.HdmiCSC2.CoefA5 = pHdmiCscData[5];
    AmbaVOUT_Display1Config.HdmiCSC3.CoefA6 = pHdmiCscData[6];
    AmbaVOUT_Display1Config.HdmiCSC3.CoefA7 = pHdmiCscData[7];
    AmbaVOUT_Display1Config.HdmiCSC4.CoefA8 = pHdmiCscData[8];
    AmbaVOUT_Display1Config.HdmiCSC4.ConstB0 = pHdmiCscData[9];
    AmbaVOUT_Display1Config.HdmiCSC5.ConstB1 = pHdmiCscData[10];
    AmbaVOUT_Display1Config.HdmiCSC5.ConstB2 = pHdmiCscData[11];
    AmbaVOUT_Display1Config.HdmiCSC6.Output0ClampLow  = pHdmiCscData[12];
    AmbaVOUT_Display1Config.HdmiCSC6.Output0ClampHigh = pHdmiCscData[13];
    AmbaVOUT_Display1Config.HdmiCSC7.Output1ClampLow  = pHdmiCscData[14];
    AmbaVOUT_Display1Config.HdmiCSC7.Output1ClampHigh = pHdmiCscData[15];
    AmbaVOUT_Display1Config.HdmiCSC8.Output2ClampLow  = pHdmiCscData[16];
    AmbaVOUT_Display1Config.HdmiCSC8.Output2ClampHigh = pHdmiCscData[17];
}

/**
 *  VOUT_SetFixedFormat - Set fixed format according to display timing
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_SetFixedFormat(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    /* Use fixed format for 480I and 576I to support pixel repetition features */
    if ((pDisplayTiming->DisplayMethod == 1U) &&
        (pDisplayTiming->HsyncFrontPorch == 38U) &&
        (pDisplayTiming->HsyncPulseWidth == 124U) &&
        (pDisplayTiming->HsyncBackPorch == 114U) &&
        (pDisplayTiming->ActivePixels == 1440U) &&
        (pDisplayTiming->VsyncFrontPorch == 4U) &&
        (pDisplayTiming->VsyncPulseWidth == 3U) &&
        (pDisplayTiming->VsyncBackPorch == 15U) &&
        (pDisplayTiming->ActiveLines == 480U)) {
        /* 480I60 */
        AmbaVOUT_Display1Config.DispCtrl.FixedFormatSelect = 1U;
    } else if ((pDisplayTiming->DisplayMethod == 1U) &&
               (pDisplayTiming->HsyncFrontPorch == 24U) &&
               (pDisplayTiming->HsyncPulseWidth == 126U) &&
               (pDisplayTiming->HsyncBackPorch == 138U) &&
               (pDisplayTiming->ActivePixels == 1440U) &&
               (pDisplayTiming->VsyncFrontPorch == 2U) &&
               (pDisplayTiming->VsyncPulseWidth == 3U) &&
               (pDisplayTiming->VsyncBackPorch == 19U) &&
               (pDisplayTiming->ActiveLines == 576U)) {
        /* 576I50 */
        AmbaVOUT_Display1Config.DispCtrl.FixedFormatSelect = 3U;
    } else {
        AmbaVOUT_Display1Config.DispCtrl.FixedFormatSelect = 0U;
    }
}

/**
 *  VOUT_SetDisplay1Timing - Assign video timing parameters for display devices
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_SetDisplay1Timing(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT16 Htotal, Vtotal, RegVal;

    Htotal = pDisplayTiming->HsyncFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels;
    if (pDisplayTiming->DisplayMethod == 0U) {
        Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines;
    } else {
        Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch;
        Vtotal *= 2U;
        Vtotal = Vtotal + 1U + pDisplayTiming->ActiveLines;
    }

    /* Fill up the video timing */
    RegVal = Htotal - 1U;
    AmbaVOUT_Display1Config.DispFrmSize.FrameWidth = RegVal;

    if (pDisplayTiming->DisplayMethod == 0U) {
        /* 0 = Progressive scan */
        RegVal = Vtotal - 1U;
        AmbaVOUT_Display1Config.DispFrmSize.FrameHeightFld0 = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
        AmbaVOUT_Display1Config.DispTopActiveStart.StartColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch;
        AmbaVOUT_Display1Config.DispTopActiveStart.StartRow = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U;
        AmbaVOUT_Display1Config.DispTopActiveEnd.EndColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines - 1U;
        AmbaVOUT_Display1Config.DispTopActiveEnd.EndRow = RegVal;

        /* Sync */
        if (AmbaVOUT_Display1Config.DispCtrl.HdmiOutput != 0U) {
            AmbaVOUT_Display1Config.HdmiHSync.StartColumn = 0U;

            RegVal = AmbaVOUT_Display1Config.HdmiHSync.StartColumn + pDisplayTiming->HsyncPulseWidth;
            AmbaVOUT_Display1Config.HdmiHSync.EndColumn = RegVal;

            AmbaVOUT_Display1Config.HdmiVSyncTopStart.StartColumn = AmbaVOUT_Display1Config.HdmiHSync.StartColumn;
            AmbaVOUT_Display1Config.HdmiVSyncTopStart.StartRow = 0U;
            AmbaVOUT_Display1Config.HdmiVSyncTopEnd.EndColumn = AmbaVOUT_Display1Config.HdmiVSyncTopStart.StartColumn;

            RegVal = AmbaVOUT_Display1Config.HdmiVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            AmbaVOUT_Display1Config.HdmiVSyncTopEnd.EndRow = RegVal;
        }
        if (AmbaVOUT_Display1Config.DispCtrl.DigitalOutput != 0U) {
            AmbaVOUT_Display1Config.DigitalHSync.StartColumn = 0U;

            RegVal = AmbaVOUT_Display1Config.DigitalHSync.StartColumn + pDisplayTiming->HsyncPulseWidth;
            AmbaVOUT_Display1Config.DigitalHSync.EndColumn = RegVal;

            AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartColumn = AmbaVOUT_Display1Config.DigitalHSync.StartColumn;
            AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartRow = 0U;
            AmbaVOUT_Display1Config.DigitalVSyncTopEnd.EndColumn = AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartColumn;

            RegVal = AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            AmbaVOUT_Display1Config.DigitalVSyncTopEnd.EndRow = RegVal;
        }

        AmbaVOUT_Display1Config.DispCtrl.Interlaced = 0U;
    } else {
        /* 1 = Interlaced scan */
        RegVal = (Vtotal >> 1U) - 1U;
        AmbaVOUT_Display1Config.DispFrmSize.FrameHeightFld0 = RegVal;

        RegVal = AmbaVOUT_Display1Config.DispFrmSize.FrameHeightFld0 + 1U;
        AmbaVOUT_Display1Config.DispFldHeight.FrameHeightFld1 = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
        AmbaVOUT_Display1Config.DispTopActiveStart.StartColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch;
        AmbaVOUT_Display1Config.DispTopActiveStart.StartRow = RegVal;

        RegVal = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U;
        AmbaVOUT_Display1Config.DispTopActiveEnd.EndColumn = RegVal;

        RegVal = pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + (pDisplayTiming->ActiveLines >> 1U) - 1U;
        AmbaVOUT_Display1Config.DispTopActiveEnd.EndRow = RegVal;

        AmbaVOUT_Display1Config.DispBtmActiveStart.StartColumn = AmbaVOUT_Display1Config.DispTopActiveStart.StartColumn;

        RegVal = AmbaVOUT_Display1Config.DispTopActiveStart.StartRow + 1U;
        AmbaVOUT_Display1Config.DispBtmActiveStart.StartRow = RegVal;

        AmbaVOUT_Display1Config.DispBtmActiveEnd.EndColumn = AmbaVOUT_Display1Config.DispTopActiveEnd.EndColumn;

        RegVal = AmbaVOUT_Display1Config.DispTopActiveEnd.EndRow + 1U;
        AmbaVOUT_Display1Config.DispBtmActiveEnd.EndRow = RegVal;

        /* Sync */
        if (AmbaVOUT_Display1Config.DispCtrl.HdmiOutput != 0U) {
            AmbaVOUT_Display1Config.HdmiHSync.StartColumn = 0U;

            RegVal = AmbaVOUT_Display1Config.HdmiHSync.StartColumn + pDisplayTiming->HsyncPulseWidth;
            AmbaVOUT_Display1Config.HdmiHSync.EndColumn = RegVal;

            AmbaVOUT_Display1Config.HdmiVSyncTopStart.StartColumn = AmbaVOUT_Display1Config.HdmiHSync.StartColumn;
            AmbaVOUT_Display1Config.HdmiVSyncTopStart.StartRow = 0U;
            AmbaVOUT_Display1Config.HdmiVSyncTopEnd.EndColumn = AmbaVOUT_Display1Config.HdmiVSyncTopStart.StartColumn;

            RegVal = AmbaVOUT_Display1Config.HdmiVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            AmbaVOUT_Display1Config.HdmiVSyncTopEnd.EndRow = RegVal;

            RegVal = AmbaVOUT_Display1Config.HdmiHSync.StartColumn + (Htotal >> 1U);
            AmbaVOUT_Display1Config.HdmiVSyncBtmStart.StartColumn = RegVal;

            AmbaVOUT_Display1Config.HdmiVSyncBtmStart.StartRow = 0U;
            AmbaVOUT_Display1Config.HdmiVSyncBtmEnd.EndColumn = AmbaVOUT_Display1Config.HdmiVSyncBtmStart.StartColumn;

            RegVal = AmbaVOUT_Display1Config.HdmiVSyncBtmStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            AmbaVOUT_Display1Config.HdmiVSyncBtmEnd.EndRow = RegVal;
        }
        if (AmbaVOUT_Display1Config.DispCtrl.DigitalOutput != 0U) {
            AmbaVOUT_Display1Config.DigitalHSync.StartColumn = 0U;

            RegVal = AmbaVOUT_Display1Config.DigitalHSync.StartColumn + pDisplayTiming->HsyncPulseWidth;
            AmbaVOUT_Display1Config.DigitalHSync.EndColumn = RegVal;

            AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartColumn = AmbaVOUT_Display1Config.DigitalHSync.StartColumn;
            AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartRow = 0U;
            AmbaVOUT_Display1Config.DigitalVSyncTopEnd.EndColumn = AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartColumn;

            RegVal = AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            AmbaVOUT_Display1Config.DigitalVSyncTopEnd.EndRow = RegVal;

            RegVal = AmbaVOUT_Display1Config.DigitalHSync.StartColumn + (Htotal >> 1U);
            AmbaVOUT_Display1Config.DigitalVSyncBtmStart.StartColumn = RegVal;

            AmbaVOUT_Display1Config.DigitalVSyncBtmStart.StartRow = 0U;
            AmbaVOUT_Display1Config.DigitalVSyncBtmEnd.EndColumn = AmbaVOUT_Display1Config.DigitalVSyncBtmStart.StartColumn;

            RegVal = AmbaVOUT_Display1Config.DigitalVSyncBtmStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            AmbaVOUT_Display1Config.DigitalVSyncBtmEnd.EndRow = RegVal;
        }

        AmbaVOUT_Display1Config.DispCtrl.Interlaced = 1U;
    }

    VOUT_SetFixedFormat(pDisplayTiming);
}

/**
 *  VOUT_SetMipiDsiTiming - Assign video timing parameters for MIPI DSI output
 *  @param[in] pDisplayTiming Video timing parameters
 *  @param[in] BytePerClock Number of bytes per clock cycle
 */
static void VOUT_SetDisplayMipiCsiTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT16 RegVal;
    /* MIPI CSI-2 spec */
    /* The active area is included in the sync area */
    /* The sync width must be greater than the active width */
    /* |----SyncBackPorch---|-----SyncPulseWidth-----|---SyncFrontPorch----| */
    /* |--------------------|-10-|--ActiveArea--|--------------------------| */

    /* Fill up the video timing */
    RegVal = pDisplayTiming->HsyncBackPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncFrontPorch - 1U;
    AmbaVOUT_Display1Config.DispFrmSize.FrameWidth = RegVal;

    RegVal = pDisplayTiming->VsyncBackPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncFrontPorch - 1U;
    AmbaVOUT_Display1Config.DispFrmSize.FrameHeightFld0 = RegVal;

    RegVal = pDisplayTiming->HsyncBackPorch + 10U;
    AmbaVOUT_Display1Config.DispTopActiveStart.StartColumn = RegVal;

    RegVal = pDisplayTiming ->VsyncBackPorch + 10U;
    AmbaVOUT_Display1Config.DispTopActiveStart.StartRow = RegVal;

    RegVal = pDisplayTiming->HsyncBackPorch + 10U + pDisplayTiming->ActivePixels - 1U;
    AmbaVOUT_Display1Config.DispTopActiveEnd.EndColumn = RegVal;

    RegVal = pDisplayTiming->VsyncBackPorch + 10U + pDisplayTiming->ActiveLines - 1U;
    AmbaVOUT_Display1Config.DispTopActiveEnd.EndRow = RegVal;

    /* Sync */
    AmbaVOUT_Display1Config.DigitalHSync.StartColumn = pDisplayTiming->HsyncBackPorch;

    RegVal = pDisplayTiming->HsyncBackPorch + pDisplayTiming->HsyncPulseWidth - 1U;
    AmbaVOUT_Display1Config.DigitalHSync.EndColumn = RegVal;

    AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartColumn = pDisplayTiming->HsyncBackPorch;
    AmbaVOUT_Display1Config.DigitalVSyncTopStart.StartRow = pDisplayTiming->VsyncBackPorch;

    RegVal = pDisplayTiming->HsyncBackPorch + pDisplayTiming->HsyncPulseWidth - 1U;
    AmbaVOUT_Display1Config.DigitalVSyncTopEnd.EndColumn = RegVal;

    RegVal = pDisplayTiming->VsyncBackPorch + pDisplayTiming->VsyncPulseWidth - 1U;
    AmbaVOUT_Display1Config.DigitalVSyncTopEnd.EndRow = RegVal;

    /* MIPI CSI has no interlaced format */
    AmbaVOUT_Display1Config.DispCtrl.Interlaced = 0U;
}

/**
 *  VOUT_GetDisplay1Timing - Get video timing configuration
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_GetDisplay1Timing(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    const AMBA_VOUT_DISPLAY1_CONFIG_REG_s *const pReg = pAmbaVoutDisplay1_Reg;
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */

#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutTvClk();
#endif
    /* Get Vout Display0 timing from Vout registers */
    if (pReg->DispCtrl.Interlaced == 0U) {
        /* Progressive scan */
        pDisplayTiming->DisplayMethod = 0U;
        FrameWidth = pReg->DispFrmSize.FrameWidth + (UINT32)1U;
        FrameHeight = pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U;
    } else {
        /* Interlaced scan */
        pDisplayTiming->DisplayMethod = 1U;
        FrameWidth = pReg->DispFrmSize.FrameWidth + (UINT32)1U;
        FrameHeight = (pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U) << 1U;
    }

    FrameActiveColStart = pReg->DispTopActiveStart.StartColumn;
    FrameActiveColWidth = (pReg->DispTopActiveEnd.EndColumn + (UINT32)1U) - FrameActiveColStart;

    FrameActiveRowStart = pReg->DispTopActiveStart.StartRow;
    FrameActiveRowHeight = (pReg->DispTopActiveEnd.EndRow + (UINT32)1U) - FrameActiveRowStart;

    pDisplayTiming->ActiveLines = (UINT16)FrameActiveRowHeight;
    pDisplayTiming->ActivePixels = (UINT16)FrameActiveColWidth;

    if (pReg->DispCtrl.HdmiOutput != 0U) {
        pDisplayTiming->HsyncPulseWidth  = pReg->HdmiHSync.EndColumn;
        pDisplayTiming->HsyncPulseWidth -= pReg->HdmiHSync.StartColumn;
        pDisplayTiming->VsyncPulseWidth  = pReg->HdmiVSyncTopEnd.EndRow;
        pDisplayTiming->VsyncPulseWidth -= pReg->HdmiVSyncTopStart.StartRow;
    } else if (pReg->DispCtrl.DigitalOutput != 0U) {
        pDisplayTiming->HsyncPulseWidth  = pReg->DigitalHSync.EndColumn;
        pDisplayTiming->HsyncPulseWidth -= pReg->DigitalHSync.StartColumn;
        pDisplayTiming->VsyncPulseWidth  = pReg->DigitalVSyncTopEnd.EndRow;
        pDisplayTiming->VsyncPulseWidth -= pReg->DigitalVSyncTopStart.StartRow;
    } else if (pReg->DispCtrl.AnalogOutput != 0U) {
        pDisplayTiming->HsyncPulseWidth  = pReg->AnalogHSync.EndColumn;
        pDisplayTiming->HsyncPulseWidth -= pReg->AnalogHSync.StartColumn;
        pDisplayTiming->VsyncPulseWidth  = pReg->AnalogVSyncTopEnd.EndRow;
        pDisplayTiming->VsyncPulseWidth -= pReg->AnalogVSyncTopStart.StartRow;
    } else {
        /* No Output */
        pDisplayTiming->HsyncPulseWidth = 0U;
        pDisplayTiming->VsyncPulseWidth = 0U;
    }

    pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pDisplayTiming->HsyncPulseWidth);
    pDisplayTiming->HsyncFrontPorch = (UINT16)(FrameWidth - (FrameActiveColStart + FrameActiveColWidth));

    pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pDisplayTiming->VsyncPulseWidth);
    pDisplayTiming->VsyncFrontPorch = (UINT16)(FrameHeight - (FrameActiveRowStart + FrameActiveRowHeight));
}

/**
 *  AmbaRTSL_Vout1InitDispConfig - Zero initialize vout display section
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1InitDispConfig(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&AmbaVOUT_Display1Config, 0, sizeof(AmbaVOUT_Display1Config));
    if (RetVal != 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        RetVal = VOUT_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1InitTvEncConfig - Zero initialize vout dve section
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1InitTvEncConfig(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(AmbaVOUT_TveConfigData, 0, sizeof(AmbaVOUT_TveConfigData));
    if (RetVal != 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        RetVal = VOUT_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1GetDispConfigAddr - Get display configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetDispConfigAddr(ULONG *pVirtAddr)
{
    const AMBA_VOUT_DISPLAY1_CONFIG_REG_s *pVout1DispCfg;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pVout1DispCfg = &AmbaVOUT_Display1Config;
#ifndef CONFIG_LINUX
        AmbaMisra_TypeCast(&VirtAddr, &pVout1DispCfg);
#else
        AmbaWrap_memcpy(&VirtAddr, &pVout1DispCfg, sizeof(pVout1DispCfg));
#endif
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1GetTvEncAddr - Get tv encoder configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetTvEncAddr(ULONG *pVirtAddr)
{
    const UINT32 *pTvEncCfg;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pTvEncCfg = AmbaVOUT_TveConfigData;
#ifndef CONFIG_LINUX
        AmbaMisra_TypeCast(&VirtAddr, &pTvEncCfg);
#else
        AmbaWrap_memcpy(&VirtAddr, &pTvEncCfg, sizeof(pTvEncCfg));
#endif
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1GetAnalogCscAddr - Get analog csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetAnalogCscAddr(ULONG *pVirtAddr)
{
    volatile const AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display1Config.AnalogCSC0;
#ifndef CONFIG_LINUX
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
#else
        AmbaWrap_memcpy(&VirtAddr, &pCscMatrix, sizeof(pCscMatrix));
#endif
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1GetDigitalCscAddr - Get digital csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetDigitalCscAddr(ULONG *pVirtAddr)
{
    volatile const AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display1Config.DigitalCSC0;
#ifndef CONFIG_LINUX
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
#else
        AmbaWrap_memcpy(&VirtAddr, &pCscMatrix, sizeof(pCscMatrix));
#endif
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1GetHdmiCscAddr - Get hdmi csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetHdmiCscAddr(ULONG *pVirtAddr)
{
    volatile const AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display1Config.HdmiCSC0;
#ifndef CONFIG_LINUX
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
#else
        AmbaWrap_memcpy(&VirtAddr, &pCscMatrix, sizeof(pCscMatrix));
#endif
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1SetVinVoutSync - Enable/Disable vin-vout sync
 *  @param[in] EnableFlag Enable flag
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1SetVinVoutSync(UINT32 EnableFlag)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        AmbaVOUT_Display1Config.DispCtrl.VinVoutSync = (UINT8)EnableFlag;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutCvbsEnable - Enable CVBS output
 *  @param[in] pDisplayCvbsConfig CVBS configuration
 *  @param[in] pTveConfig TV encoder configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VoutCvbsEnable(const AMBA_VOUT_DISPLAY_CVBS_CONFIG_REG_s *pDisplayCvbsConfig, const AMBA_VOUT_TVENC_CONTROL_REG_s *pTveConfig)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    SetRegValue(&AmbaVOUT_Display1Config.DispCtrl, pDisplayCvbsConfig->DispCtrl);
    SetRegValue(&AmbaVOUT_Display1Config.DispFrmSize, pDisplayCvbsConfig->DispFrmSize);
    SetRegValue(&AmbaVOUT_Display1Config.DispFldHeight, pDisplayCvbsConfig->DispFldHeight);
    SetRegValue(&AmbaVOUT_Display1Config.DispTopActiveStart, pDisplayCvbsConfig->DispTopActiveStart);
    SetRegValue(&AmbaVOUT_Display1Config.DispTopActiveEnd, pDisplayCvbsConfig->DispTopActiveEnd);
    SetRegValue(&AmbaVOUT_Display1Config.DispBtmActiveStart, pDisplayCvbsConfig->DispBtmActiveStart);
    SetRegValue(&AmbaVOUT_Display1Config.DispBtmActiveEnd, pDisplayCvbsConfig->DispBtmActiveEnd);
    SetRegValue(&AmbaVOUT_Display1Config.DispBackgroundColor, pDisplayCvbsConfig->DispBackgroundColor);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogOutputMode, pDisplayCvbsConfig->AnalogOutputMode);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogHSync, pDisplayCvbsConfig->AnalogHSync);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogVSyncTopStart, pDisplayCvbsConfig->AnalogVSyncTopStart);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogVSyncTopEnd, pDisplayCvbsConfig->AnalogVSyncTopEnd);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogVSyncBtmStart, pDisplayCvbsConfig->AnalogVSyncBtmStart);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogVSyncBtmEnd, pDisplayCvbsConfig->AnalogVSyncBtmEnd);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogCSC0, pDisplayCvbsConfig->AnalogCSC0);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogCSC1, pDisplayCvbsConfig->AnalogCSC1);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogCSC2, pDisplayCvbsConfig->AnalogCSC2);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogCSC3, pDisplayCvbsConfig->AnalogCSC3);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogCSC4, pDisplayCvbsConfig->AnalogCSC4);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogCSC5, pDisplayCvbsConfig->AnalogCSC5);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogCSC6, pDisplayCvbsConfig->AnalogCSC6);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogCSC7, pDisplayCvbsConfig->AnalogCSC7);
    SetRegValue(&AmbaVOUT_Display1Config.AnalogCSC8, pDisplayCvbsConfig->AnalogCSC8);

    /* clear TV encoder configuration data */
    (void)AmbaWrap_memset(AmbaVOUT_TveConfigData, 0, sizeof(AmbaVOUT_TveConfigData));

    AmbaVOUT_TveConfigData[32] = pTveConfig->TvEncReg32;
    AmbaVOUT_TveConfigData[33] = pTveConfig->TvEncReg33;
    AmbaVOUT_TveConfigData[34] = pTveConfig->TvEncReg34;
    AmbaVOUT_TveConfigData[35] = pTveConfig->TvEncReg35;
    AmbaVOUT_TveConfigData[36] = pTveConfig->TvEncReg36;
    AmbaVOUT_TveConfigData[37] = pTveConfig->TvEncReg37;
    AmbaVOUT_TveConfigData[38] = pTveConfig->TvEncReg38;
    AmbaVOUT_TveConfigData[39] = pTveConfig->TvEncReg39;
    AmbaVOUT_TveConfigData[40] = pTveConfig->TvEncReg40;
    AmbaVOUT_TveConfigData[42] = pTveConfig->TvEncReg42;
    AmbaVOUT_TveConfigData[43] = pTveConfig->TvEncReg43;
    AmbaVOUT_TveConfigData[44] = pTveConfig->TvEncReg44;
    AmbaVOUT_TveConfigData[45] = pTveConfig->TvEncReg45;
    AmbaVOUT_TveConfigData[46] = pTveConfig->TvEncReg46;
    AmbaVOUT_TveConfigData[47] = pTveConfig->TvEncReg47;
    AmbaVOUT_TveConfigData[50] = pTveConfig->TvEncReg50;
    AmbaVOUT_TveConfigData[51] = pTveConfig->TvEncReg51;
    AmbaVOUT_TveConfigData[52] = pTveConfig->TvEncReg52;
    AmbaVOUT_TveConfigData[56] = pTveConfig->TvEncReg56;
    AmbaVOUT_TveConfigData[57] = pTveConfig->TvEncReg57;
    AmbaVOUT_TveConfigData[58] = pTveConfig->TvEncReg58;
    AmbaVOUT_TveConfigData[59] = pTveConfig->TvEncReg59;
    AmbaVOUT_TveConfigData[60] = pTveConfig->TvEncReg60;
    AmbaVOUT_TveConfigData[61] = pTveConfig->TvEncReg61;
    AmbaVOUT_TveConfigData[62] = pTveConfig->TvEncReg62;
    AmbaVOUT_TveConfigData[65] = pTveConfig->TvEncReg65;
    AmbaVOUT_TveConfigData[66] = pTveConfig->TvEncReg66;
    AmbaVOUT_TveConfigData[67] = pTveConfig->TvEncReg67;
    AmbaVOUT_TveConfigData[68] = pTveConfig->TvEncReg68;
    AmbaVOUT_TveConfigData[69] = pTveConfig->TvEncReg69;
    AmbaVOUT_TveConfigData[96] = pTveConfig->TvEncReg96;
    AmbaVOUT_TveConfigData[97] = pTveConfig->TvEncReg97;
    AmbaVOUT_TveConfigData[99] = pTveConfig->TvEncReg99;
    AmbaVOUT_TveConfigData[120] = pTveConfig->TvEncReg120;
    AmbaVOUT_TveConfigData[121] = pTveConfig->TvEncReg121;

    /* Config Display1 through debugport as default setting */
    SetRegValue(&pAmbaVoutDisplay1_Reg->DispCtrl, pDisplayCvbsConfig->DispCtrl);
    SetRegValue(&pAmbaVoutDisplay1_Reg->DispFrmSize, pDisplayCvbsConfig->DispFrmSize);
    SetRegValue(&pAmbaVoutDisplay1_Reg->DispFldHeight, pDisplayCvbsConfig->DispFldHeight);
    SetRegValue(&pAmbaVoutDisplay1_Reg->DispTopActiveStart, pDisplayCvbsConfig->DispTopActiveStart);
    SetRegValue(&pAmbaVoutDisplay1_Reg->DispTopActiveEnd, pDisplayCvbsConfig->DispTopActiveEnd);
    SetRegValue(&pAmbaVoutDisplay1_Reg->DispBtmActiveStart, pDisplayCvbsConfig->DispBtmActiveStart);
    SetRegValue(&pAmbaVoutDisplay1_Reg->DispBtmActiveEnd, pDisplayCvbsConfig->DispBtmActiveEnd);
    SetRegValue(&pAmbaVoutDisplay1_Reg->DispBackgroundColor, pDisplayCvbsConfig->DispBackgroundColor);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogOutputMode, pDisplayCvbsConfig->AnalogOutputMode);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogHSync, pDisplayCvbsConfig->AnalogHSync);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogVSyncTopStart, pDisplayCvbsConfig->AnalogVSyncTopStart);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogVSyncTopEnd, pDisplayCvbsConfig->AnalogVSyncTopEnd);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogVSyncBtmStart, pDisplayCvbsConfig->AnalogVSyncBtmStart);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogVSyncBtmEnd, pDisplayCvbsConfig->AnalogVSyncBtmEnd);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogCSC0, pDisplayCvbsConfig->AnalogCSC0);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogCSC1, pDisplayCvbsConfig->AnalogCSC1);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogCSC2, pDisplayCvbsConfig->AnalogCSC2);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogCSC3, pDisplayCvbsConfig->AnalogCSC3);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogCSC4, pDisplayCvbsConfig->AnalogCSC4);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogCSC5, pDisplayCvbsConfig->AnalogCSC5);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogCSC6, pDisplayCvbsConfig->AnalogCSC6);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogCSC7, pDisplayCvbsConfig->AnalogCSC7);
    SetRegValue(&pAmbaVoutDisplay1_Reg->AnalogCSC8, pDisplayCvbsConfig->AnalogCSC8);

    /* Config TvEnc through debugport as default setting */
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg32, pTveConfig->TvEncReg32);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg33, pTveConfig->TvEncReg33);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg34, pTveConfig->TvEncReg34);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg35, pTveConfig->TvEncReg35);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg36, pTveConfig->TvEncReg36);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg37, pTveConfig->TvEncReg37);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg38, pTveConfig->TvEncReg38);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg39, pTveConfig->TvEncReg39);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg40, pTveConfig->TvEncReg40);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg42, pTveConfig->TvEncReg42);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg43, pTveConfig->TvEncReg43);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg44, pTveConfig->TvEncReg44);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg45, pTveConfig->TvEncReg45);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg46, pTveConfig->TvEncReg46);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg47, pTveConfig->TvEncReg47);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg50, pTveConfig->TvEncReg50);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg51, pTveConfig->TvEncReg51);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg52, pTveConfig->TvEncReg52);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg56, pTveConfig->TvEncReg56);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg57, pTveConfig->TvEncReg57);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg58, pTveConfig->TvEncReg58);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg59, pTveConfig->TvEncReg59);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg60, pTveConfig->TvEncReg60);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg61, pTveConfig->TvEncReg61);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg62, pTveConfig->TvEncReg62);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg65, pTveConfig->TvEncReg65);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg66, pTveConfig->TvEncReg66);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg67, pTveConfig->TvEncReg67);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg68, pTveConfig->TvEncReg68);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg69, pTveConfig->TvEncReg69);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg120, pTveConfig->TvEncReg120);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg121, pTveConfig->TvEncReg121);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutCvbsSetIreParam - Set CVBS IRE parameter
 *  @param[in] IreIndex IRE parameter ID
 *  @param[in] Param IRE parameter value
 *  @return error code
 */
UINT32 AmbaRTSL_VoutCvbsSetIreParam(UINT32 IreIndex, UINT32 Param)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (IreIndex == VOUT_CVBS_IRE_PARAM_CLAMP) {
        AmbaVOUT_TveConfigData[44] = Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_BLACK) {
        AmbaVOUT_TveConfigData[42] = Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_BLANK) {
        AmbaVOUT_TveConfigData[43] = Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_SYNC) {
        AmbaVOUT_TveConfigData[45] = Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTGAIN) {
        AmbaVOUT_Display1Config.AnalogCSC6.CoefA4 = (UINT16)Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTOFFSET) {
        AmbaVOUT_Display1Config.AnalogCSC7.ConstB1 = (UINT16)Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMIN) {
        AmbaVOUT_Display1Config.AnalogCSC8.Output1ClampLow = (UINT16)Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMAX) {
        AmbaVOUT_Display1Config.AnalogCSC8.Output1ClampHigh = (UINT16)Param;
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutCvbsGetIreParam - Get CVBS IRE parameter
 *  @param[in] IreIndex IRE parameter ID
 *  @return IRE parameter value
 */
UINT32 AmbaRTSL_VoutCvbsGetIreParam(UINT32 IreIndex)
{
    UINT32 RetValue = 0;

    if (IreIndex == VOUT_CVBS_IRE_PARAM_CLAMP) {
        RetValue = AmbaVOUT_TveConfigData[44];
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_BLACK) {
        RetValue = AmbaVOUT_TveConfigData[42];
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_BLANK) {
        RetValue = AmbaVOUT_TveConfigData[43];
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_SYNC) {
        RetValue = AmbaVOUT_TveConfigData[45];
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTGAIN) {
        RetValue = (UINT32)AmbaVOUT_Display1Config.AnalogCSC6.CoefA4;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTOFFSET) {
        RetValue = (UINT32)AmbaVOUT_Display1Config.AnalogCSC7.ConstB1;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMIN) {
        RetValue = (UINT32)AmbaVOUT_Display1Config.AnalogCSC8.Output1ClampLow;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMAX) {
        RetValue = (UINT32)AmbaVOUT_Display1Config.AnalogCSC8.Output1ClampHigh;
    } else {
        /* else */
    }

    return RetValue;
}

/**
 *  AmbaRTSL_VoutCvbsEnableColorBar - Enable color bar pattern generation
 *  @param[in] EnableFlag 1 = Enable; 0 = Disable color bar pattern generation
 *  @param[in] pTveConfig TV encoder configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VoutCvbsEnableColorBar(UINT32 EnableFlag, const AMBA_VOUT_TVENC_CONTROL_REG_s *pTveConfig)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 TvEncReg46Data, TvEncReg56Data;

    if (EnableFlag == 0U) {
        /* Update ucode buffer settings */
        AmbaVOUT_TveConfigData[46] &= ~((UINT32)1U << 5U);
        AmbaVOUT_TveConfigData[56] &= ~((UINT32)1U << 4U);
        AmbaVOUT_TveConfigData[56] |= (UINT32)1U << 3U;
        /* Update settings with debugport */
        TvEncReg46Data = pTveConfig->TvEncReg46 & ~((UINT32)1U << 5U);
        TvEncReg56Data = pTveConfig->TvEncReg56 & ~((UINT32)1U << 4U);
        TvEncReg56Data |= (UINT32)1U << 3U;
    } else {
        /* Update ucode buffer settings */
        AmbaVOUT_TveConfigData[46] |= (UINT32)1U << 5U;
        AmbaVOUT_TveConfigData[56] |= (UINT32)1U << 4U;
        AmbaVOUT_TveConfigData[56] &= ~((UINT32)1U << 3U);
        /* Update settings with debugport */
        TvEncReg46Data = pTveConfig->TvEncReg46 | ((UINT32)1U << 5U);
        TvEncReg56Data = pTveConfig->TvEncReg56 | ((UINT32)1U << 4U);
        TvEncReg56Data &= ~((UINT32)1U << 3U);
    }

    /* Config through debugport */
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg46, TvEncReg46Data);
    SetRegValue(&pAmbaVoutTvEnc_Reg->TvEncReg56, TvEncReg56Data);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutAnalogSetCsc - Set CSC matrix as register format
 *  @param[in] pVoutCscData Color space conversion data
 *  @param[in] AnalogMode Analog mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutAnalogSetCsc(const UINT16 *pVoutCscData, UINT32 AnalogMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetAnalogCsc(pVoutCscData);
    AmbaMisra_TouchUnused(&AnalogMode);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutCvbsPowerCtrl - Enable/Disable DAC power
 *  @param[in] EnableFlag Enable flag
 *  @return error code
 */
UINT32 AmbaRTSL_VoutCvbsPowerCtrl(UINT32 EnableFlag)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag == 0U) {
        AmbaCSL_VoutDacPowerDown();
    } else {
        AmbaCSL_VoutDacPowerOn();
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutHdmiEnable - Enable HDMI display
 *  @param[in] HdmiMode HDMI pixel format
 *  @param[in] FrameSyncPolarity Frame sync signal polarity
 *  @param[in] LineSyncPolarity Line sync signal polarity
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutHdmiEnable(UINT32 HdmiMode, UINT32 FrameSyncPolarity, UINT32 LineSyncPolarity, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if ((HdmiMode >= VOUT_NUM_HDMI_MODE) || (pDisplayTiming == NULL)) {
        RetVal = VOUT_ERR_ARG;
    } else {
        AmbaVOUT_Display1Config.DispCtrl.HdmiOutput = 1U;
        if (HdmiMode == VOUT_HDMI_MODE_RGB888_24BIT) {
            AmbaVOUT_Display1Config.HdmiOutputMode.OutputMode = 1U;
        } else if (HdmiMode == VOUT_HDMI_MODE_YCC444_24BIT) {
            AmbaVOUT_Display1Config.HdmiOutputMode.OutputMode = 0U;
        } else {
            AmbaVOUT_Display1Config.HdmiOutputMode.OutputMode = 2U;
        }

        if (FrameSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            AmbaVOUT_Display1Config.HdmiOutputMode.VSyncPolarity = 0U;
        } else {
            AmbaVOUT_Display1Config.HdmiOutputMode.VSyncPolarity = 1U;
        }
        if (LineSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            AmbaVOUT_Display1Config.HdmiOutputMode.HSyncPolarity = 0U;
        } else {
            AmbaVOUT_Display1Config.HdmiOutputMode.HSyncPolarity = 1U;
        }

        /* Fill up the video timing */
        VOUT_SetDisplay1Timing(pDisplayTiming);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutHdmiGetStatus - Get HDMI related configurations
 *  @param[out] pHdmiConfig HDMI-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutHdmiGetStatus(AMBA_VOUT_HDMI_CONFIG_s *pHdmiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    AMBA_VOUTD_HDMI_OUTPUT_MODE_REG_s HdmiOutputMode;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pHdmiConfig != NULL) {
        HdmiOutputMode = pAmbaVoutDisplay1_Reg->HdmiOutputMode;
        if (HdmiOutputMode.OutputMode == 1U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_RGB888_24BIT;
        } else if (HdmiOutputMode.OutputMode == 0U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_YCC444_24BIT;
        } else if (HdmiOutputMode.OutputMode == 2U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_YCC422_24BIT;
        } else {
            RetVal = VOUT_ERR_PROTOCOL;
        }

        if (RetVal == VOUT_ERR_NONE) {
            if (HdmiOutputMode.HSyncPolarity == 0U) {
                pHdmiConfig->LineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pHdmiConfig->LineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
            if (HdmiOutputMode.VSyncPolarity == 0U) {
                pHdmiConfig->FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
            } else {
                pHdmiConfig->FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
            }
        }
    }

    if (pDisplayTiming != NULL) {
        VOUT_GetDisplay1Timing(pDisplayTiming);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutHdmiSetCsc - Set CSC matrix as register format
 *  @param[in] pHdmiCscData Color space conversion data
 *  @param[in] HdmiMode HDMI mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutHdmiSetCsc(const UINT16 *pHdmiCscData, UINT32 HdmiMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetHdmiCsc(pHdmiCscData);
    AmbaMisra_TouchUnused(&HdmiMode);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutConfigMipiCsiPhy - Configure MIPI PHY
 *  @param[in] pVoutMipiTiming Pointer to MIPI timing configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VoutConfigMipiCsiPhy(const AMBA_VOUT_MIPI_TIMING_PARAM_s *pVoutMipiTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVoutMipiTiming == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        AmbaCSL_VoutMipiDsiSetTrail(pVoutMipiTiming->HsTrail);
        AmbaCSL_VoutMipiDsiSetClkTrail(pVoutMipiTiming->ClkTrail);
        AmbaCSL_VoutMipiDsiSetPrepare(pVoutMipiTiming->HsPrepare);
        AmbaCSL_VoutMipiDsiSetClkPrepare(pVoutMipiTiming->ClkPrepare);
        AmbaCSL_VoutMipiDsiSetZero(pVoutMipiTiming->HsZero);
        AmbaCSL_VoutMipiDsiSetClkZero(pVoutMipiTiming->ClkZero);
        AmbaCSL_VoutMipiDsiSetLpx(pVoutMipiTiming->HsLpx);
        AmbaCSL_VoutMipiDsiSetInitTx(pVoutMipiTiming->InitTx);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiEnable - Enable MIPI-CSI display
 *  @param[in] MipiCsiMode HDMI pixel format
 *  @param[in] ColorOrder Pixel bits transmission order
 *  @param[in] MipiLaneNum MIPI lane count
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiEnable(UINT32 MipiCsiMode, UINT32 ColorOrder, UINT32 MipiLaneNum, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 MipiCsiRawMode = AmbaVout_MipiGetRawMode(MipiCsiMode);
    UINT8 NumMipiLane;

    AmbaMisra_TouchUnused(&ColorOrder);

    if (pDisplayTiming == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        /* Fill up the control */
        NumMipiLane = (UINT8)MipiLaneNum - 1U;
        AmbaVOUT_Display1Config.DispCtrl.NumMipiLane = NumMipiLane;
        AmbaVOUT_Display1Config.DispCtrl.FlipMode = 0U;
        AmbaVOUT_Display1Config.DispCtrl.DigitalOutput = 1U;
        AmbaVOUT_Display1Config.DispCtrl.FixedFormatSelect = 0U;

        /* According to Vout output mode definition */
        if ((MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_1LANE) ||
            (MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_2LANE) ||
            (MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_4LANE)) {
            AmbaVOUT_Display1Config.DigitalOutputMode.OutputMode = 11U;
        } else {
            /* RAW8 1/2/4 lane */
            AmbaVOUT_Display1Config.DigitalOutputMode.OutputMode = 9U;
        }

        /* Enable frame number increments by 1 for every Frame Sync packet (from 0x1 to 0xffff) by default */
        AmbaVOUT_Display1Config.DigitalOutputMode.MipiLineSync = 0U;
        AmbaVOUT_Display1Config.DigitalOutputMode.MipiLineCount = 0U;
        AmbaVOUT_Display1Config.DigitalOutputMode.MipiFrameCount = 1U;
        AmbaVOUT_Display1Config.DigitalOutputMode.MipiLineDataFrameBlank = 0U;
        AmbaVOUT_Display1Config.DigitalOutputMode.MipiLineSyncFrameBlank = 0U;
        AmbaVOUT_Display1Config.DigitalOutputMode.MipiEccOrder = 0U;

        /* Fill up the video timing */
        VOUT_SetDisplayMipiCsiTiming(pDisplayTiming);

        /* MIPI D-PHY related setting */
        if (AmbaVout_MipiGetDphyClkMode(MipiCsiMode) != 0U) {
            (void)AmbaRTSL_VoutMipiCsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_UP, VOUT_MIPI_DPHY_CONT_CLK);
        } else {
            (void)AmbaRTSL_VoutMipiCsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_UP, VOUT_MIPI_DPHY_NONCONT_CLK);
        }

        //Set to 1 when CSI is running
        pAmbaVoutTop_Reg->EnableSection1.MipiSelectCsi = 1U;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiSetPhyCtrl - Set PHY control parameter
 *  @param[in] ParamID PHY parameter ID
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    UINT32 RetVal;

    switch (ParamID) {
    case VOUT_PHY_MIPI_DPHY_POWER_UP:
        /* Reset digital phy */
        /* DSI_CTRL_REG2[1]=1 (soft reset) */
        AmbaCSL_VoutMipiDsiSetCtrl2(0x17000002);

        /* Pre-init for send mipi command */
        /* Default PHY value */
        AmbaCSL_VoutMipiDsiSetCtrl0(0x61300313);
        AmbaCSL_VoutMipiDsiSetCtrl1(0x00000007);
        AmbaCSL_VoutMipiDsiSetCtrl2(0x170000f1);
        AmbaCSL_VoutMipiDsiSetAuxCtrl(0x00000013);

        /* MIPI continuous mode */
        if (ParamVal == VOUT_MIPI_DPHY_CONT_CLK) {
            AmbaCSL_VoutMipiDsiSetClkMode(1U);
        } else {
            AmbaCSL_VoutMipiDsiSetClkMode(0U);
        }

        RetVal = VOUT_ERR_NONE;
        break;
    case VOUT_PHY_MIPI_DPHY_POWER_DOWN:
        /* Disable analog phy */
        /* DSI_CTRL_REG0[1:0] = 0 (this turns of internal clock divider) */
        /* DSI_CTRL_REG0[8]=0 (this turns of analog phy's bias) */
        /* DSI_CTRL_REG2[0] = 0 (this turn off clock channel) */
        /* DSI_CTRL_REG2[7:4] = 0 (this turn off data channel) */
        AmbaCSL_VoutMipiDsiSetCtrl0(0x61300210);
        AmbaCSL_VoutMipiDsiSetCtrl2(0x17000000);

        /* Reset digital phy */
        /* DSI_CTRL_REG2[1]=1 (soft reset) */
        AmbaCSL_VoutMipiDsiSetCtrl2(0x17000002);

        RetVal = VOUT_ERR_NONE;
        break;
    default:
        RetVal = VOUT_ERR_ARG;
        break;
    };

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiGetStatus - Get MIPI CSI related configurations
 *  @param[out] pMipiCsiConfig MIPI-CSI-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiGetStatus(AMBA_VOUT_MIPI_CSI_CONFIG_s *pMipiCsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 ModeOffset;

    if (pMipiCsiConfig != NULL) {
        ModeOffset = pAmbaVoutDisplay1_Reg->DispCtrl.NumMipiLane;

        if (pAmbaVoutDisplay1_Reg->DigitalOutputMode.OutputMode == 11U) {
            pMipiCsiConfig->MipiCsiMode = ModeOffset;
        } else if (pAmbaVoutDisplay1_Reg->DigitalOutputMode.OutputMode == 9U) {
            pMipiCsiConfig->MipiCsiMode = VOUT_MIPI_CSI_MODE_RAW8_1LANE + ModeOffset;
        } else {
            RetVal = VOUT_ERR_PROTOCOL;
        }

        if (RetVal == VOUT_ERR_NONE) {
            pMipiCsiConfig->ColorOrder = 0U;
        }
    }

    if (pDisplayTiming != NULL) {
        VOUT_GetDisplay1Timing(pDisplayTiming);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiSetCsc - Set CSC matrix as register format
 *  @param[in] pVoutCscData Color space conversion data
 *  @param[in] MipiCsiMode MIPI CSI mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiSetCsc(const UINT16 *pVoutCscData, UINT32 MipiCsiMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetDigital1Csc(pVoutCscData);
    AmbaMisra_TouchUnused(&MipiCsiMode);

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1GetStatus - Get vout1 status
 *  @param[out] pStatus Vout1 status
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1GetStatus(UINT32 *pStatus)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    *pStatus = AmbaCSL_Vout1GetStatus();

    return RetVal;
}
