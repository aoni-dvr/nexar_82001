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
#include "AmbaHDMI_Def.h"
#include "AmbaRTSL_VOUT.h"
#include "AmbaCSL_VOUT.h"

#ifndef CONFIG_LINUX
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

static AMBA_VOUT_DISPLAY_C_CONFIG_REG_s AmbaVOUT_Display2Config __attribute__((aligned(128)));
static UINT32 AmbaVOUT_TveConfigData[128] __attribute__((aligned(128)));

#else
#include "osal.h"
#include <linux/delay.h>

#define AmbaWrap_memcpy OSAL_memcpy
#define AmbaWrap_memset OSAL_memset

AMBA_VOUT_DISPLAY_C_CONFIG_REG_s AmbaVOUT_Display2Config;
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
    AmbaVOUT_Display2Config.AnalogCSC0.CoefA0 = pVoutCscData[0];  /* Coef[0][0] */
    AmbaVOUT_Display2Config.AnalogCSC0.CoefA4 = pVoutCscData[3];  /* Coef[1][0] */
    AmbaVOUT_Display2Config.AnalogCSC1.CoefA8 = pVoutCscData[6];  /* Coef[2][0] */
    AmbaVOUT_Display2Config.AnalogCSC1.ConstB0 = pVoutCscData[9];
    AmbaVOUT_Display2Config.AnalogCSC2.ConstB1 = pVoutCscData[10];
    AmbaVOUT_Display2Config.AnalogCSC2.ConstB2 = pVoutCscData[11];
    AmbaVOUT_Display2Config.AnalogCSC3.Output0ClampLow  = pVoutCscData[12];
    AmbaVOUT_Display2Config.AnalogCSC3.Output0ClampHigh = pVoutCscData[13];
    AmbaVOUT_Display2Config.AnalogCSC4.Output1ClampLow  = pVoutCscData[14];
    AmbaVOUT_Display2Config.AnalogCSC4.Output1ClampHigh = pVoutCscData[15];
    AmbaVOUT_Display2Config.AnalogCSC5.Output2ClampLow  = pVoutCscData[16];
    AmbaVOUT_Display2Config.AnalogCSC5.Output2ClampHigh = pVoutCscData[17];
}

/**
 *  VOUT_SetHdmiCsc - Assign color space conversion parameters for hdmi vout interface
 *  @param[in] pHdmiCscData Color space conversion data
 */
static void VOUT_SetHdmiCsc(const UINT16 *pHdmiCscData)
{
    AmbaVOUT_Display2Config.HdmiCSC0.CoefA0 = pHdmiCscData[0];
    AmbaVOUT_Display2Config.HdmiCSC0.CoefA1 = pHdmiCscData[1];
    AmbaVOUT_Display2Config.HdmiCSC1.CoefA2 = pHdmiCscData[2];
    AmbaVOUT_Display2Config.HdmiCSC1.CoefA3 = pHdmiCscData[3];
    AmbaVOUT_Display2Config.HdmiCSC2.CoefA4 = pHdmiCscData[4];
    AmbaVOUT_Display2Config.HdmiCSC2.CoefA5 = pHdmiCscData[5];
    AmbaVOUT_Display2Config.HdmiCSC3.CoefA6 = pHdmiCscData[6];
    AmbaVOUT_Display2Config.HdmiCSC3.CoefA7 = pHdmiCscData[7];
    AmbaVOUT_Display2Config.HdmiCSC4.CoefA8 = pHdmiCscData[8];
    AmbaVOUT_Display2Config.HdmiCSC4.ConstB0 = pHdmiCscData[9];
    AmbaVOUT_Display2Config.HdmiCSC5.ConstB1 = pHdmiCscData[10];
    AmbaVOUT_Display2Config.HdmiCSC5.ConstB2 = pHdmiCscData[11];
    AmbaVOUT_Display2Config.HdmiCSC6.Output0ClampLow  = pHdmiCscData[12];
    AmbaVOUT_Display2Config.HdmiCSC6.Output0ClampHigh = pHdmiCscData[13];
    AmbaVOUT_Display2Config.HdmiCSC7.Output1ClampLow  = pHdmiCscData[14];
    AmbaVOUT_Display2Config.HdmiCSC7.Output1ClampHigh = pHdmiCscData[15];
    AmbaVOUT_Display2Config.HdmiCSC8.Output2ClampLow  = pHdmiCscData[16];
    AmbaVOUT_Display2Config.HdmiCSC8.Output2ClampHigh = pHdmiCscData[17];
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
        AmbaVOUT_Display2Config.DispCtrl.FixedFormatSelect = 1U;
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
        AmbaVOUT_Display2Config.DispCtrl.FixedFormatSelect = 3U;
    } else {
        AmbaVOUT_Display2Config.DispCtrl.FixedFormatSelect = 0U;
    }
}

/**
 *  VOUT_SetDisplay2Timing - Assign video timing parameters for display devices
 *  @param[in] HdmiMode Video pixel format
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_SetDisplay2Timing(UINT32 HdmiMode, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT16 Htotal, Vtotal;

    Htotal = pDisplayTiming->HsyncFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels;
    if (pDisplayTiming->DisplayMethod == 0U) {
        Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines;
    } else {
        Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch;
        Vtotal *= 2U;
        Vtotal = Vtotal + 1U + pDisplayTiming->ActiveLines;
    }

    /* Fill up the video timing */
    AmbaVOUT_Display2Config.DispFrmSize.FrameWidth = Htotal - 1U;
    if ((HdmiMode == HDMI_PIXEL_FORMAT_YCC_420) && ((AmbaVOUT_Display2Config.DispFrmSize.FrameWidth % 2U) != 0U)) {
        /* FrameWidth should be even in YUV420 mode */
        AmbaVOUT_Display2Config.DispFrmSize.FrameWidth -= 1U;
    }

    if (pDisplayTiming->DisplayMethod == 0U) {
        /* 0 = Progressive scan */
        AmbaVOUT_Display2Config.DispFrmSize.FrameHeightFld0 = Vtotal - 1U;
        AmbaVOUT_Display2Config.DispTopActiveStart.StartColumn = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
        AmbaVOUT_Display2Config.DispTopActiveStart.StartRow = pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch;
        AmbaVOUT_Display2Config.DispTopActiveEnd.EndColumn = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U;
        AmbaVOUT_Display2Config.DispTopActiveEnd.EndRow = pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines - 1U;

        /* Sync */
        if (AmbaVOUT_Display2Config.DispCtrl.HdmiOutput != 0U) {
            AmbaVOUT_Display2Config.HdmiHSync.StartColumn = 0U;
            AmbaVOUT_Display2Config.HdmiHSync.EndColumn = AmbaVOUT_Display2Config.HdmiHSync.StartColumn + pDisplayTiming->HsyncPulseWidth;
            AmbaVOUT_Display2Config.HdmiVSyncTopStart.StartColumn = AmbaVOUT_Display2Config.HdmiHSync.StartColumn;
            AmbaVOUT_Display2Config.HdmiVSyncTopStart.StartRow = 0U;
            AmbaVOUT_Display2Config.HdmiVSyncTopEnd.EndColumn = AmbaVOUT_Display2Config.HdmiVSyncTopStart.StartColumn;
            AmbaVOUT_Display2Config.HdmiVSyncTopEnd.EndRow = AmbaVOUT_Display2Config.HdmiVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
        }

        AmbaVOUT_Display2Config.DispCtrl.Interlaced = 0U;

    } else {
        /* 1 = Interlaced scan */
        AmbaVOUT_Display2Config.DispFrmSize.FrameHeightFld0 = (Vtotal >> 1U) - 1U;
        AmbaVOUT_Display2Config.DispFldHeight.FrameHeightFld1 = AmbaVOUT_Display2Config.DispFrmSize.FrameHeightFld0 + 1U;
        AmbaVOUT_Display2Config.DispTopActiveStart.StartColumn = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
        AmbaVOUT_Display2Config.DispTopActiveStart.StartRow = pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch;
        AmbaVOUT_Display2Config.DispTopActiveEnd.EndColumn = pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U;
        AmbaVOUT_Display2Config.DispTopActiveEnd.EndRow = pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + (pDisplayTiming->ActiveLines >> 1U) - 1U;
        AmbaVOUT_Display2Config.DispBtmActiveStart.StartColumn = AmbaVOUT_Display2Config.DispTopActiveStart.StartColumn;
        AmbaVOUT_Display2Config.DispBtmActiveStart.StartRow = AmbaVOUT_Display2Config.DispTopActiveStart.StartRow + 1U;
        AmbaVOUT_Display2Config.DispBtmActiveEnd.EndColumn = AmbaVOUT_Display2Config.DispTopActiveEnd.EndColumn;
        AmbaVOUT_Display2Config.DispBtmActiveEnd.EndRow = AmbaVOUT_Display2Config.DispTopActiveEnd.EndRow + 1U;

        /* Sync */
        if (AmbaVOUT_Display2Config.DispCtrl.HdmiOutput != 0U) {
            AmbaVOUT_Display2Config.HdmiHSync.StartColumn = 0U;
            AmbaVOUT_Display2Config.HdmiHSync.EndColumn = AmbaVOUT_Display2Config.HdmiHSync.StartColumn + pDisplayTiming->HsyncPulseWidth;
            AmbaVOUT_Display2Config.HdmiVSyncTopStart.StartColumn = AmbaVOUT_Display2Config.HdmiHSync.StartColumn;
            AmbaVOUT_Display2Config.HdmiVSyncTopStart.StartRow = 0U;
            AmbaVOUT_Display2Config.HdmiVSyncTopEnd.EndColumn = AmbaVOUT_Display2Config.HdmiVSyncTopStart.StartColumn;
            AmbaVOUT_Display2Config.HdmiVSyncTopEnd.EndRow = AmbaVOUT_Display2Config.HdmiVSyncTopStart.StartRow + pDisplayTiming->VsyncPulseWidth;
            AmbaVOUT_Display2Config.HdmiVSyncBtmStart.StartColumn = AmbaVOUT_Display2Config.HdmiHSync.StartColumn + (Htotal >> 1U);
            AmbaVOUT_Display2Config.HdmiVSyncBtmStart.StartRow = 0U;
            AmbaVOUT_Display2Config.HdmiVSyncBtmEnd.EndColumn = AmbaVOUT_Display2Config.HdmiVSyncBtmStart.StartColumn;
            AmbaVOUT_Display2Config.HdmiVSyncBtmEnd.EndRow = AmbaVOUT_Display2Config.HdmiVSyncBtmStart.StartRow + pDisplayTiming->VsyncPulseWidth;
        }

        AmbaVOUT_Display2Config.DispCtrl.Interlaced = 1U;
    }

    VOUT_SetFixedFormat(pDisplayTiming);
}

/**
 *  VOUT_GetDisplay2Timing - Get video timing configuration
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_GetDisplay2Timing(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    const AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *const pReg = pAmbaVoutDisplay2_Reg;
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */

    /* Get Vout DisplayC timing from Vout registers */
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
 *  AmbaRTSL_Vout2InitDispConfig - Zero initialize vout display section
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2InitDispConfig(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&AmbaVOUT_Display2Config, 0, sizeof(AmbaVOUT_Display2Config));
    if (RetVal != 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        RetVal = VOUT_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2InitTvEncConfig - Zero initialize vout dve section
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2InitTvEncConfig(void)
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
 *  AmbaRTSL_Vout2GetDispConfigAddr - Get display configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetDispConfigAddr(ULONG *pVirtAddr)
{
    const AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *pVout2DispCfg;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pVout2DispCfg = &AmbaVOUT_Display2Config;
#ifndef CONFIG_LINUX
        AmbaMisra_TypeCast(&VirtAddr, &pVout2DispCfg);
#else
        OSAL_memcpy(&VirtAddr, &pVout2DispCfg, sizeof(pVout2DispCfg));
#endif
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2GetTvEncAddr - Get tv encoder configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetTvEncAddr(ULONG *pVirtAddr)
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
        OSAL_memcpy(&VirtAddr, &pTvEncCfg, sizeof(pTvEncCfg));
#endif
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2GetAnalogCscAddr - Get analog csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetAnalogCscAddr(ULONG *pVirtAddr)
{
    volatile const AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display2Config.AnalogCSC0;
#ifndef CONFIG_LINUX
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
#else
        OSAL_memcpy(&VirtAddr, &pCscMatrix, sizeof(pCscMatrix));
#endif
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2GetHdmiCscAddr - Get hdmi csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetHdmiCscAddr(ULONG *pVirtAddr)
{
    volatile const AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_Display2Config.HdmiCSC0;
#ifndef CONFIG_LINUX
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
#else
        OSAL_memcpy(&VirtAddr, &pCscMatrix, sizeof(pCscMatrix));
#endif
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2SetVinVoutSync - Enable/Disable vin-vout sync
 *  @param[in] EnableFlag Enable flag
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2SetVinVoutSync(UINT32 EnableFlag)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        AmbaVOUT_Display2Config.DispCtrl.VinVoutSyncEnable = (UINT8)EnableFlag;
    }

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
#ifndef CONFIG_LINUX
    VOUT_SetAnalogCsc(pVoutCscData);
#endif
    AmbaMisra_TouchUnused(&AnalogMode);

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

    /* Vout Top settings */
    pAmbaVoutTop_Reg->EnableSection1.Enable = 0U;    /* Disable clocks for Display_b and Mixer_b */
    pAmbaVoutTop_Reg->EnableSection2.Enable = 2U;    /* Enable mixer_b drive display_c */

    SetRegValue(&AmbaVOUT_Display2Config.DispCtrl, pDisplayCvbsConfig->DispCtrl);
    SetRegValue(&AmbaVOUT_Display2Config.DispFrmSize, pDisplayCvbsConfig->DispFrmSize);
    SetRegValue(&AmbaVOUT_Display2Config.DispFldHeight, pDisplayCvbsConfig->DispFldHeight);
    SetRegValue(&AmbaVOUT_Display2Config.DispTopActiveStart, pDisplayCvbsConfig->DispTopActiveStart);
    SetRegValue(&AmbaVOUT_Display2Config.DispTopActiveEnd, pDisplayCvbsConfig->DispTopActiveEnd);
    SetRegValue(&AmbaVOUT_Display2Config.DispBtmActiveStart, pDisplayCvbsConfig->DispBtmActiveStart);
    SetRegValue(&AmbaVOUT_Display2Config.DispBtmActiveEnd, pDisplayCvbsConfig->DispBtmActiveEnd);
    SetRegValue(&AmbaVOUT_Display2Config.DispBackgroundColor, pDisplayCvbsConfig->DispBackgroundColor);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogOutputMode, pDisplayCvbsConfig->AnalogOutputMode);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogHSync, pDisplayCvbsConfig->AnalogHSync);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogVSyncTopStart, pDisplayCvbsConfig->AnalogVSyncTopStart);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogVSyncTopEnd, pDisplayCvbsConfig->AnalogVSyncTopEnd);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogVSyncBtmStart, pDisplayCvbsConfig->AnalogVSyncBtmStart);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogVSyncBtmEnd, pDisplayCvbsConfig->AnalogVSyncBtmEnd);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogCSC0, pDisplayCvbsConfig->AnalogCSC0);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogCSC1, pDisplayCvbsConfig->AnalogCSC1);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogCSC2, pDisplayCvbsConfig->AnalogCSC2);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogCSC3, pDisplayCvbsConfig->AnalogCSC3);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogCSC4, pDisplayCvbsConfig->AnalogCSC4);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogCSC5, pDisplayCvbsConfig->AnalogCSC5);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogCSC6, pDisplayCvbsConfig->AnalogCSC6);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogCSC7, pDisplayCvbsConfig->AnalogCSC7);
    SetRegValue(&AmbaVOUT_Display2Config.AnalogCSC8, pDisplayCvbsConfig->AnalogCSC8);

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

    /* Config Display2 through debugport as default setting */
    SetRegValue(&pAmbaVoutDisplay2_Reg->DispCtrl, pDisplayCvbsConfig->DispCtrl);
    SetRegValue(&pAmbaVoutDisplay2_Reg->DispFrmSize, pDisplayCvbsConfig->DispFrmSize);
    SetRegValue(&pAmbaVoutDisplay2_Reg->DispFldHeight, pDisplayCvbsConfig->DispFldHeight);
    SetRegValue(&pAmbaVoutDisplay2_Reg->DispTopActiveStart, pDisplayCvbsConfig->DispTopActiveStart);
    SetRegValue(&pAmbaVoutDisplay2_Reg->DispTopActiveEnd, pDisplayCvbsConfig->DispTopActiveEnd);
    SetRegValue(&pAmbaVoutDisplay2_Reg->DispBtmActiveStart, pDisplayCvbsConfig->DispBtmActiveStart);
    SetRegValue(&pAmbaVoutDisplay2_Reg->DispBtmActiveEnd, pDisplayCvbsConfig->DispBtmActiveEnd);
    SetRegValue(&pAmbaVoutDisplay2_Reg->DispBackgroundColor, pDisplayCvbsConfig->DispBackgroundColor);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogOutputMode, pDisplayCvbsConfig->AnalogOutputMode);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogHSync, pDisplayCvbsConfig->AnalogHSync);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogVSyncTopStart, pDisplayCvbsConfig->AnalogVSyncTopStart);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogVSyncTopEnd, pDisplayCvbsConfig->AnalogVSyncTopEnd);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogVSyncBtmStart, pDisplayCvbsConfig->AnalogVSyncBtmStart);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogVSyncBtmEnd, pDisplayCvbsConfig->AnalogVSyncBtmEnd);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogCSC0, pDisplayCvbsConfig->AnalogCSC0);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogCSC1, pDisplayCvbsConfig->AnalogCSC1);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogCSC2, pDisplayCvbsConfig->AnalogCSC2);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogCSC3, pDisplayCvbsConfig->AnalogCSC3);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogCSC4, pDisplayCvbsConfig->AnalogCSC4);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogCSC5, pDisplayCvbsConfig->AnalogCSC5);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogCSC6, pDisplayCvbsConfig->AnalogCSC6);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogCSC7, pDisplayCvbsConfig->AnalogCSC7);
    SetRegValue(&pAmbaVoutDisplay2_Reg->AnalogCSC8, pDisplayCvbsConfig->AnalogCSC8);

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
        AmbaVOUT_Display2Config.AnalogCSC6.CoefA4 = (UINT16)Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTOFFSET) {
        AmbaVOUT_Display2Config.AnalogCSC7.ConstB1 = (UINT16)Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMIN) {
        AmbaVOUT_Display2Config.AnalogCSC8.Output1ClampLow = (UINT16)Param;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMAX) {
        AmbaVOUT_Display2Config.AnalogCSC8.Output1ClampHigh = (UINT16)Param;
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
        RetValue = (UINT32)AmbaVOUT_Display2Config.AnalogCSC6.CoefA4;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTOFFSET) {
        RetValue = (UINT32)AmbaVOUT_Display2Config.AnalogCSC7.ConstB1;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMIN) {
        RetValue = (UINT32)AmbaVOUT_Display2Config.AnalogCSC8.Output1ClampLow;
    } else if (IreIndex == VOUT_CVBS_IRE_PARAM_OUTMAX) {
        RetValue = (UINT32)AmbaVOUT_Display2Config.AnalogCSC8.Output1ClampHigh;
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
 *  AmbaRTSL_VoutCvbsPowerCtrl - Enable/Disable DAC power
 *  @param[in] EnableFlag Enable flag
 *  @return error code
 */
UINT32 AmbaRTSL_VoutCvbsPowerCtrl(UINT32 EnableFlag)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag == 0U) {
        AmbaCSL_VoutDacPowerDown();
        AmbaCSL_VoutDacBistEnable();
    } else {
        AmbaCSL_VoutDacPowerOn();
        AmbaCSL_VoutDacBistDisable();
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

    if (HdmiMode >= VOUT_NUM_HDMI_MODE) {
        RetVal = VOUT_ERR_ARG;
    } else {

        /* Vout Top settings */
        pAmbaVoutTop_Reg->EnableSection1.Enable = 0U;    /* Disable clocks for Display_b and Mixer_b */
        pAmbaVoutTop_Reg->EnableSection2.Enable = 2U;    /* Enable mixer_b drive display_c */

        AmbaVOUT_Display2Config.DispCtrl.HdmiOutput = 1U;
        if (HdmiMode == VOUT_HDMI_MODE_RGB888_24BIT) {
            AmbaVOUT_Display2Config.HdmiOutputMode.OutputMode = 1U;
        } else if (HdmiMode == VOUT_HDMI_MODE_YCC444_24BIT) {
            AmbaVOUT_Display2Config.HdmiOutputMode.OutputMode = 0U;
        } else if (HdmiMode == VOUT_HDMI_MODE_YCC420_24BIT) {
            AmbaVOUT_Display2Config.HdmiOutputMode.OutputMode = 3U;
            AmbaVOUT_Display2Config.InputSyncCounterCtrl.SyncEnable = 0U;
            AmbaVOUT_Display2Config.InputConfig.ColorRes = 1U;
        } else {  /* HdmiMode == VOUT_HDMI_MODE_YCC422_24BIT */
            AmbaVOUT_Display2Config.HdmiOutputMode.OutputMode = 2U;
        }

        if (FrameSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            AmbaVOUT_Display2Config.HdmiOutputMode.VSyncPolarity = 0U;
        } else {
            AmbaVOUT_Display2Config.HdmiOutputMode.VSyncPolarity = 1U;
        }
        if (LineSyncPolarity == VOUT_SIGNAL_ACTIVE_LOW) {
            AmbaVOUT_Display2Config.HdmiOutputMode.HSyncPolarity = 0U;
        } else {
            AmbaVOUT_Display2Config.HdmiOutputMode.HSyncPolarity = 1U;
        }

        /* Fill up the video timing */
        if (pDisplayTiming != NULL) {
            VOUT_SetDisplay2Timing(HdmiMode, pDisplayTiming);
        }
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
        HdmiOutputMode = pAmbaVoutDisplay2_Reg->HdmiOutputMode;
        if (HdmiOutputMode.OutputMode == 1U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_RGB888_24BIT;
        } else if (HdmiOutputMode.OutputMode == 0U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_YCC444_24BIT;
        } else if (HdmiOutputMode.OutputMode == 2U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_YCC422_24BIT;
        } else if (HdmiOutputMode.OutputMode == 3U) {
            pHdmiConfig->HdmiMode = VOUT_HDMI_MODE_YCC420_24BIT;
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
        VOUT_GetDisplay2Timing(pDisplayTiming);
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
 *  AmbaRTSL_Vout2SetTimeout - Set Timeout for vout1
 *  @param[in] Timeout Timeout value
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2SetTimeout(UINT32 Timeout)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaCSL_Vout2SetTimeout(Timeout);

    return RetVal;
}

/**
 *  AmbaRTSL_Vout2GetStatus - Get vout2 status
 *  @param[out] pStatus Vout2 status
 *  @return error code
 */
UINT32 AmbaRTSL_Vout2GetStatus(UINT32 *pStatus)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    *pStatus = AmbaCSL_Vout2GetStatus();

    return RetVal;
}

