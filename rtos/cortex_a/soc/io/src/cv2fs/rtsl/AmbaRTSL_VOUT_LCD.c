/*
 * Copyright (c) 2020 Ambarella International LP
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
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaRTSL_VOUT.h"
#include "AmbaRTSL_PLL.h"
#if defined(CONFIG_QNX)
#include "AmbaSYS.h"
#include "AmbaCSL_PLL.h"
#endif
#include "AmbaCSL_VOUT.h"

#if defined(CONFIG_QNX)
AMBA_VOUT_DISPLAY_CONFIG_REG_s AmbaVOUT_DisplayConfig[2] __attribute__((aligned(128))) __attribute__((section(".dsp_buf")));
#else
static AMBA_VOUT_DISPLAY_CONFIG_REG_s AmbaVOUT_DisplayConfig[2] __attribute__((aligned(128)));
#endif

#define VOUT_REG_MAX_VALUE   (UINT16)0x3fff  /* 14 bits for vout timing paremeters */

static UINT32 VoutChan = 0U;
static UINT32 BtaEnable = MIPI_DSI_BTA_ENABLE_FROM_RCT;

#ifdef CONFIG_QNX
UINT32 AmbaRTSL_PllGetNumCpuCycleMs(void){
    /* Return a long enough value */
    return 0xFFFF;
}
#endif
/**
 *  BoundaryCheck - Check the boundary for mipi-phy parameters
 *  @param[in] PowerOfTwo The power of 2
 *  @param[in,out] Num The number to be checked
 */
static void BoundaryCheck(UINT32 PowerOfTwo, DOUBLE *pNum)
{
    DOUBLE UpperBound = 0.0;

    if (AmbaWrap_pow(2.0, (DOUBLE) PowerOfTwo, &UpperBound) == ERR_NONE) {
        if (*pNum > (UpperBound - 1.0)) {
            *pNum = (UpperBound - 1.0);
        } else if (*pNum == 0.0) {
            *pNum = 0.0;
        } else {
            *pNum = *pNum - 1.0;
        }
    }
}

/**
 *  VOUT_CalculateMphyConfig - Calculate mipi-phy parameters
 *  @param[in] BitRate MIPI data rate
 */
static void VOUT_CalculateMphyConfig(UINT64 BitRate)
{
    UINT32 RetVal;
    DOUBLE MipiBitRate;
    DOUBLE TxByteClkHS, UI;
    DOUBLE ClkPrepare, ClkPrepareMax, ClkPrepareSec;
    DOUBLE HsLpxMin;
    DOUBLE HsTrailMin, Temp;
    DOUBLE HsPrepareMax, HsPrepareSec;
    DOUBLE HsZeroMin, HsZeroMinSec;
    DOUBLE ClkZeroMinSec, ClkZeroMin;
    DOUBLE ClkTrailMin;
    DOUBLE InitTxMin;

    MipiBitRate = (DOUBLE)BitRate / 1e6;                // in MHz

    /********************************************************
     * TX (TxByteClkHS)
     ********************************************************/
    TxByteClkHS = (DOUBLE)MipiBitRate * 1e6 / 8.0;      // in Hz
    UI = (1.0 / (MipiBitRate * 1e6));                   // in seccond

    // ClkPrepare (use HsPrepare)
    ClkPrepareMax = 95e-9 * TxByteClkHS;
    RetVal = AmbaWrap_floor(ClkPrepareMax + 0.5, &ClkPrepareMax);

    ClkPrepareSec = (38e-9 + 95e-9) / 2.0;
    ClkPrepare = ClkPrepareSec * TxByteClkHS;
    RetVal |= AmbaWrap_floor(ClkPrepare + 0.5, &ClkPrepare);

    // ClkZero  (use 2 x HsZero)
    ClkZeroMinSec = (0.0 != TxByteClkHS) ? (300e-9 - (ClkPrepare / TxByteClkHS)) : 0.0;
    ClkZeroMin = ClkZeroMinSec * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(ClkZeroMin, &ClkZeroMin);

    // ClkTrail
    ClkTrailMin = 60e-9 * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(ClkTrailMin, &ClkTrailMin);
    BoundaryCheck(5, &ClkTrailMin);

    // HsPrepare
    HsPrepareMax = (85e-9 + (6.0 * UI)) * TxByteClkHS;
    RetVal |= AmbaWrap_floor(HsPrepareMax + 0.5, &HsPrepareMax);

    HsPrepareSec = (40e-9 + (4.0 * UI) + 85e-9 + (6.0 * UI)) / 2.0;

    if (HsPrepareMax > ClkPrepareMax) {
        HsPrepareMax = ClkPrepareMax;
    }
    BoundaryCheck(6, &HsPrepareMax);

    // HsZero
    HsZeroMinSec = 145e-9 + (10.0 * UI) - HsPrepareSec;
    HsZeroMin = HsZeroMinSec * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(HsZeroMin, &HsZeroMin);

    if (HsZeroMin < (ClkZeroMin / 2.0)) {
        HsZeroMin = ClkZeroMin / 2.0;
    }
    BoundaryCheck(6, &HsZeroMin);

    // HsTrail
    Temp = 8.0 * UI;
    HsTrailMin = 60e-9 + (4.0 * UI);
    if (HsTrailMin < Temp) {
        HsTrailMin = Temp;
    }
    HsTrailMin = HsTrailMin * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(HsTrailMin, &HsTrailMin);
    BoundaryCheck(5, &HsTrailMin);

    // HsLpx
    HsLpxMin = 50e-9 * TxByteClkHS;
    RetVal |= AmbaWrap_ceil(HsLpxMin, &HsLpxMin);
    BoundaryCheck(5, &HsLpxMin);

    // InitTx
    InitTxMin = 100e-6 * TxByteClkHS / 1024.0;
    RetVal |= AmbaWrap_ceil(InitTxMin, &InitTxMin);
    BoundaryCheck(6, &InitTxMin);

    if (RetVal == ERR_NONE) {
        /* Set the related values */
        if (VoutChan == 0U) {
            AmbaCSL_Vout0MipiDsiSetClkTrail((UINT8)ClkTrailMin);
            AmbaCSL_Vout0MipiDsiSetPrepare((UINT8)HsPrepareMax);
            AmbaCSL_Vout0MipiDsiSetClkPrepare((UINT8)HsPrepareMax);
            AmbaCSL_Vout0MipiDsiSetZero((UINT8)HsZeroMin);
            AmbaCSL_Vout0MipiDsiSetClkZero((UINT8)HsZeroMin);
            AmbaCSL_Vout0MipiDsiSetTrail((UINT8)HsTrailMin);
            AmbaCSL_Vout0MipiDsiSetLpx((UINT8)HsLpxMin);
            AmbaCSL_Vout0MipiDsiSetInitTx((UINT8)InitTxMin);
        } else {
            AmbaCSL_Vout1MipiDsiSetClkTrail((UINT8)ClkTrailMin);
            AmbaCSL_Vout1MipiDsiSetPrepare((UINT8)HsPrepareMax);
            AmbaCSL_Vout1MipiDsiSetClkPrepare((UINT8)HsPrepareMax);
            AmbaCSL_Vout1MipiDsiSetZero((UINT8)HsZeroMin);
            AmbaCSL_Vout1MipiDsiSetClkZero((UINT8)HsZeroMin);
            AmbaCSL_Vout1MipiDsiSetTrail((UINT8)HsTrailMin);
            AmbaCSL_Vout1MipiDsiSetLpx((UINT8)HsLpxMin);
            AmbaCSL_Vout1MipiDsiSetInitTx((UINT8)InitTxMin);
        }
    }
}

/**
 *  Float2Int32 - Convert a floating-point value to an integer
 *  @param[in] WorkFLOAT A floating-point value
 *  @return An integer
 */
static inline INT32 Float2Int32(FLOAT WorkFLOAT)
{
    INT32 WorkINT;

    if (WorkFLOAT >= 0.0F) {
        WorkINT = (INT32)(WorkFLOAT);
    } else {
        WorkINT = 0 - (INT32)(-WorkFLOAT);
    }
    return WorkINT;
}

/**
 *  VOUT_SetDigital0Csc - Assign color space conversion parameters for digital vout interface
 *  @param[in] pCscMatrix Color space conversion matrix
 */
static void VOUT_SetDigitalCsc(const AMBA_VOUT_CSC_MATRIX_s *pCscMatrix)
{
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC0.CoefA0 = (UINT16)Float2Int32(pCscMatrix->Coef[0][0] * 1024.0F);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC0.CoefA1 = (UINT16)Float2Int32(pCscMatrix->Coef[0][1] * 1024.0F);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC1.CoefA2 = (UINT16)Float2Int32(pCscMatrix->Coef[0][2] * 1024.0F);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC1.CoefA3 = (UINT16)Float2Int32(pCscMatrix->Coef[1][0] * 1024.0F);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC2.CoefA4 = (UINT16)Float2Int32(pCscMatrix->Coef[1][1] * 1024.0F);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC2.CoefA5 = (UINT16)Float2Int32(pCscMatrix->Coef[1][2] * 1024.0F);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC3.CoefA6 = (UINT16)Float2Int32(pCscMatrix->Coef[2][0] * 1024.0F);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC3.CoefA7 = (UINT16)Float2Int32(pCscMatrix->Coef[2][1] * 1024.0F);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC4.CoefA8 = (UINT16)Float2Int32(pCscMatrix->Coef[2][2] * 1024.0F);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC4.ConstB0 = (UINT16)Float2Int32(pCscMatrix->Offset[0]);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC5.ConstB1 = (UINT16)Float2Int32(pCscMatrix->Offset[1]);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC5.ConstB2 = (UINT16)Float2Int32(pCscMatrix->Offset[2]);
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC6.Output0ClampLow  = pCscMatrix->MinVal[0];
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC6.Output0ClampHigh = pCscMatrix->MaxVal[0];
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC7.Output1ClampLow  = pCscMatrix->MinVal[1];
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC7.Output1ClampHigh = pCscMatrix->MaxVal[1];
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC8.Output2ClampLow  = pCscMatrix->MinVal[2];
    AmbaVOUT_DisplayConfig[VoutChan].DigitalCSC8.Output2ClampHigh = pCscMatrix->MaxVal[2];
}

/**
 *  VOUT_SetMipiDsiTiming - Assign video timing parameters for MIPI DSI output
 *  @param[in] pDisplayTiming Video timing parameters
 *  @param[in] BytePerClock Number of bytes per clock cycle
 */
static UINT32 VOUT_SetMipiDsiTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, UINT32 BytePerClock)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 NewActiveStartX, NewActiveStartY;
    UINT16 ActiveStartX = pDisplayTiming->HsyncFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
    UINT16 ActiveStartY = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch;
    UINT8 Byte4Clock, Byte2Clock;
    UINT16 Overhead, TempInt;
    DOUBLE TempDouble;
    UINT16 ActiveWidthBound, ActiveHeightBound;
    UINT16 NewHFrontPorch, NewVFrontPorch;
    UINT16 Htotal = pDisplayTiming->HsyncFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels;
    UINT16 Vtotal = pDisplayTiming->VsyncFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines;

    if (BytePerClock == 3U) {
        Byte4Clock = 1U;
        Byte2Clock = 1U;
    } else {
        Byte4Clock = 2U;
        Byte2Clock = 1U;
    }

    /* Calculate new Hsync front porch */
    TempInt = (UINT16) 6U * Byte4Clock;
    TempInt += (UINT16) 4U * Byte2Clock;
    TempInt += 6U;
    ActiveWidthBound = Htotal - TempInt;

    Overhead = (UINT16) 5U * Byte4Clock;
    Overhead += (UINT16) 2U * Byte2Clock;
    Overhead += 5U;

    TempDouble =  (DOUBLE)ActiveWidthBound - (DOUBLE)pDisplayTiming->ActivePixels;
    TempDouble = TempDouble / 3.0;
    if (AmbaWrap_ceil(TempDouble, &TempDouble) != ERR_NONE) {
        RetVal = VOUT_ERR_UNEXPECTED;
    }

    NewActiveStartX = Overhead + (UINT16)((UINT16)TempDouble * 2U) - 20U;
    NewHFrontPorch = pDisplayTiming->HsyncFrontPorch - (ActiveStartX - NewActiveStartX);

    /* Calculate new Vsync front porch */
    ActiveHeightBound = Vtotal - 6U;
    NewActiveStartY = ActiveHeightBound - pDisplayTiming->ActiveLines;
    NewActiveStartY = NewActiveStartY >> 1U;
    NewActiveStartY += 5U;
    NewVFrontPorch = pDisplayTiming->VsyncFrontPorch - (ActiveStartY - NewActiveStartY);

    /* Fill up and check video timing */
    TempInt = Htotal - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispFrmSize.FrameWidth = TempInt;
    }

    TempInt = NewHFrontPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalHSync.StartColumn = TempInt;
    }

    TempInt = NewHFrontPorch + pDisplayTiming->HsyncPulseWidth;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalHSync.EndColumn = TempInt;
    }

    TempInt = Vtotal - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispFrmSize.FrameHeightFld0 = TempInt;
    }

    TempInt = NewHFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispTopActiveStart.StartColumn = TempInt;
    }

    TempInt = NewVFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming ->VsyncBackPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispTopActiveStart.StartRow = TempInt;
    }

    TempInt = NewHFrontPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispTopActiveEnd.EndColumn = TempInt;
    }

    TempInt = NewVFrontPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispTopActiveEnd.EndRow = TempInt;
    }

    /* Sync */
    AmbaVOUT_DisplayConfig[VoutChan].DigitalVSyncTopStart.StartColumn = AmbaVOUT_DisplayConfig[VoutChan].DigitalHSync.StartColumn;

    TempInt = NewVFrontPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalVSyncTopStart.StartRow = TempInt;
    }
    AmbaVOUT_DisplayConfig[VoutChan].DigitalVSyncTopEnd.EndColumn = AmbaVOUT_DisplayConfig[VoutChan].DigitalHSync.StartColumn;

    TempInt = NewVFrontPorch + pDisplayTiming->VsyncPulseWidth;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalVSyncTopEnd.EndRow = TempInt;
    }

    AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.Interlaced = 0U;

    return RetVal;
}

/**
 *  VOUT_SetMipiCsiTiming - Assign video timing parameters for MIPI CSI output
 *  @param[in] pDisplayTiming Video timing parameters
 *  @param[in] BytePerClock Number of bytes per clock cycle
 */
static UINT32 VOUT_SetMipiCsiTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT16 TempInt;

    /* MIPI CSI-2 spec */
    /* The active area is included in the sync area */
    /* The sync width must be greater than the active width */
    /* |----SyncBackPorch---|-----SyncPulseWidth-----|---SyncFrontPorch----| */
    /* |--------------------|-10-|--ActiveArea--|--------------------------| */


    /* Fill up and check video timing */
    TempInt = pDisplayTiming->HsyncBackPorch + pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncFrontPorch - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispFrmSize.FrameWidth = TempInt;
    }

    TempInt = pDisplayTiming->VsyncBackPorch + pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncFrontPorch - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispFrmSize.FrameHeightFld0 = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch + 10U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispTopActiveStart.StartColumn = TempInt;
    }

    TempInt = pDisplayTiming ->VsyncBackPorch + 10U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispTopActiveStart.StartRow = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch + 10U + pDisplayTiming->ActivePixels - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispTopActiveEnd.EndColumn = TempInt;
    }

    TempInt = pDisplayTiming->VsyncBackPorch + 10U + pDisplayTiming->ActiveLines - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DispTopActiveEnd.EndRow = TempInt;
    }

    /* Sync */
    TempInt = pDisplayTiming->HsyncBackPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalHSync.StartColumn = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch + pDisplayTiming->HsyncPulseWidth - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalHSync.EndColumn = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalVSyncTopStart.StartColumn = TempInt;
    }

    TempInt = pDisplayTiming->VsyncBackPorch;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalVSyncTopStart.StartRow = TempInt;
    }

    TempInt = pDisplayTiming->HsyncBackPorch + pDisplayTiming->HsyncPulseWidth - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalVSyncTopEnd.EndColumn = TempInt;
    }

    TempInt = pDisplayTiming->VsyncBackPorch + pDisplayTiming->VsyncPulseWidth - 1U;
    if (TempInt > VOUT_REG_MAX_VALUE) {
        RetVal |= VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].DigitalVSyncTopEnd.EndRow = TempInt;
    }

    /* MIPI CSI has no interlaced format */
    AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.Interlaced = 0U;

    return RetVal;
}

/**
 *  VOUT_GetDisplay0Timing - Get video timing configuration
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_GetDisplay0Timing(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *const pReg = pAmbaVoutDisplay0_Reg;
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameHeight; /* V-total */

#if defined(CONFIG_QNX)
    AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTA, &pDisplayTiming->PixelClkFreq);
#else
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVout0Clk();
#endif
    /* Get Vout Display0 timing from Vout registers */
    pDisplayTiming->DisplayMethod = 0U;
    FrameHeight = pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U;

    FrameActiveColStart = pReg->DispTopActiveStart.StartColumn;
    FrameActiveColWidth = (pReg->DispTopActiveEnd.EndColumn + (UINT32)1U) - FrameActiveColStart;

    FrameActiveRowStart = pReg->DispTopActiveStart.StartRow;
    FrameActiveRowHeight = (pReg->DispTopActiveEnd.EndRow + (UINT32)1U) - FrameActiveRowStart;

    pDisplayTiming->ActiveLines = (UINT16)FrameActiveRowHeight;
    pDisplayTiming->ActivePixels = (UINT16)FrameActiveColWidth;

    pDisplayTiming->HsyncPulseWidth  = pReg->DigitalHSync.EndColumn;
    pDisplayTiming->HsyncPulseWidth -= pReg->DigitalHSync.StartColumn;
    pDisplayTiming->VsyncPulseWidth  = pReg->DigitalVSyncTopEnd.EndRow;
    pDisplayTiming->VsyncPulseWidth -= pReg->DigitalVSyncTopStart.StartRow;

    pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pReg->DigitalHSync.EndColumn);
    pDisplayTiming->HsyncFrontPorch = (UINT16)(pReg->DigitalHSync.StartColumn);

    pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pDisplayTiming->VsyncPulseWidth);
    pDisplayTiming->VsyncFrontPorch = (UINT16)(FrameHeight - (FrameActiveRowStart + FrameActiveRowHeight));
}

/**
 *  VOUT_GetDisplay1Timing - Get video timing configuration
 *  @param[in] pDisplayTiming Video timing parameters
 */
static void VOUT_GetDisplay1Timing(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *const pReg = pAmbaVoutDisplay1_Reg;
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */

#if defined(CONFIG_QNX)
    AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTA, &pDisplayTiming->PixelClkFreq);
#else
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVout1Clk();
#endif
    /* Get Vout Display1 timing from Vout registers */
    pDisplayTiming->DisplayMethod = 0U;
    FrameWidth = pReg->DispFrmSize.FrameWidth + (UINT32)1U;
    FrameHeight = pReg->DispFrmSize.FrameHeightFld0 + (UINT32)1U;

    FrameActiveColStart = pReg->DispTopActiveStart.StartColumn;
    FrameActiveColWidth = (pReg->DispTopActiveEnd.EndColumn + (UINT32)1U) - FrameActiveColStart;

    FrameActiveRowStart = pReg->DispTopActiveStart.StartRow;
    FrameActiveRowHeight = (pReg->DispTopActiveEnd.EndRow + (UINT32)1U) - FrameActiveRowStart;

    pDisplayTiming->ActiveLines = (UINT16)FrameActiveRowHeight;
    pDisplayTiming->ActivePixels = (UINT16)FrameActiveColWidth;

    pDisplayTiming->HsyncPulseWidth  = pReg->DigitalHSync.EndColumn;
    pDisplayTiming->HsyncPulseWidth -= pReg->DigitalHSync.StartColumn;
    pDisplayTiming->VsyncPulseWidth  = pReg->DigitalVSyncTopEnd.EndRow;
    pDisplayTiming->VsyncPulseWidth -= pReg->DigitalVSyncTopStart.StartRow;

    pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pDisplayTiming->HsyncPulseWidth);
    pDisplayTiming->HsyncFrontPorch = (UINT16)(FrameWidth - (FrameActiveColStart + FrameActiveColWidth));

    pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pDisplayTiming->VsyncPulseWidth);
    pDisplayTiming->VsyncFrontPorch = (UINT16)(FrameHeight - (FrameActiveRowStart + FrameActiveRowHeight));
}

/**
 *  VOUT_GetMipiBytePerClk - Get MIPI DSI byte per clock infomation
 *  @param[in] MipiDsiRawMode MIPI DSI raw mode
 *  @return Byte Per Clock
 */
static UINT32 VOUT_GetMipiDsiBytePerClk(UINT32 MipiDsiRawMode)
{
    UINT32 BytePerClock = 0U;

    switch (MipiDsiRawMode) {
    case VOUT_MIPI_DSI_MODE_422_1LANE:
    case VOUT_MIPI_DSI_MODE_565_1LANE:
    case VOUT_MIPI_DSI_MODE_422_2LANE:
    case VOUT_MIPI_DSI_MODE_565_2LANE:
    case VOUT_MIPI_DSI_MODE_422_4LANE:
    case VOUT_MIPI_DSI_MODE_565_4LANE:
        BytePerClock = 2U;
        break;
    case VOUT_MIPI_DSI_MODE_666_1LANE:
    case VOUT_MIPI_DSI_MODE_888_1LANE:
    case VOUT_MIPI_DSI_MODE_666_2LANE:
    case VOUT_MIPI_DSI_MODE_888_2LANE:
    case VOUT_MIPI_DSI_MODE_666_4LANE:
    case VOUT_MIPI_DSI_MODE_888_4LANE:
    default:
        BytePerClock = 3U;
        break;
    };

    return BytePerClock;
}

/**
 *  VOUT_GetMipiLaneNum - Get MIPI DSI lane num information
 *  @param[in] MipiDsiRawMode MIPI DSI raw mode
 *  @return Lane number
 */
static UINT32 VOUT_GetMipiDsiLaneNum(UINT32 MipiDsiRawMode)
{
    UINT32 NumMipiLane = 0U;

    switch (MipiDsiRawMode) {
    case VOUT_MIPI_DSI_MODE_422_1LANE:
    case VOUT_MIPI_DSI_MODE_565_1LANE:
    case VOUT_MIPI_DSI_MODE_666_1LANE:
    case VOUT_MIPI_DSI_MODE_888_1LANE:
        NumMipiLane = 1U;
        break;
    case VOUT_MIPI_DSI_MODE_422_2LANE:
    case VOUT_MIPI_DSI_MODE_565_2LANE:
    case VOUT_MIPI_DSI_MODE_666_2LANE:
    case VOUT_MIPI_DSI_MODE_888_2LANE:
        NumMipiLane = 2U;
        break;
    case VOUT_MIPI_DSI_MODE_422_4LANE:
    case VOUT_MIPI_DSI_MODE_565_4LANE:
    case VOUT_MIPI_DSI_MODE_666_4LANE:
    case VOUT_MIPI_DSI_MODE_888_4LANE:
    default:
        NumMipiLane = 4U;
        break;
    };

    return NumMipiLane;
}

void AmbaRTSL_VoutChannelSelect(UINT32 VoutChannel)
{
    VoutChan = VoutChannel;
}

/**
 *  AmbaRTSL_Vout0InitDispConfig - Zero initialize vout display section
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0InitDispConfig(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&AmbaVOUT_DisplayConfig[0], 0, sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s));
    if (RetVal != 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        RetVal = VOUT_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1InitDispConfig - Zero initialize vout display section
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1InitDispConfig(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(&AmbaVOUT_DisplayConfig[1], 0, sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s));
    if (RetVal != 0U) {
        RetVal = VOUT_ERR_UNEXPECTED;
    } else {
        RetVal = VOUT_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0GetDispConfigAddr - Get display configuration parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0GetDispConfigAddr(ULONG *pVirtAddr)
{
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *pVout0DispCfg;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pVout0DispCfg = &AmbaVOUT_DisplayConfig[0];
        AmbaMisra_TypeCast(&VirtAddr, &pVout0DispCfg);
        *pVirtAddr = VirtAddr;
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
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *pVout1DispCfg;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pVout1DispCfg = &AmbaVOUT_DisplayConfig[1];
        AmbaMisra_TypeCast(&VirtAddr, &pVout1DispCfg);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0GetDigitalCscAddr - Get digital csc parameter address
 *  @param[out] pVirtAddr A virtual address
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0GetDigitalCscAddr(ULONG *pVirtAddr)
{
    volatile const AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s *pCscMatrix;
    ULONG VirtAddr = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (pVirtAddr == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        pCscMatrix = &AmbaVOUT_DisplayConfig[0].DigitalCSC0;
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
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
        pCscMatrix = &AmbaVOUT_DisplayConfig[1].DigitalCSC0;
        AmbaMisra_TypeCast(&VirtAddr, &pCscMatrix);
        *pVirtAddr = VirtAddr;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0SetVinVoutSync - Enable/Disable vin-vout sync
 *  @param[in] EnableFlag Enable flag
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0SetVinVoutSync(UINT32 EnableFlag)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (EnableFlag > 1U) {
        RetVal = VOUT_ERR_ARG;
    } else {
        AmbaVOUT_DisplayConfig[0].DispCtrl.VinVoutSync = (UINT8)EnableFlag;
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
        AmbaVOUT_DisplayConfig[1].DispCtrl.VinVoutSync = (UINT8)EnableFlag;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiEnable - Enable MIPI-DSI display
 *  @param[in] MipiDsiMode MIPI DSI mode
 *  @param[in] ColorOrder Pixel bits transmission order
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiEnable(UINT32 MipiDsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 ClkRatio, BytePerClock, MipiLaneNum;
    UINT8 NumMipiLane;
    UINT32 MipiDsiRawMode = AmbaVout_MipiGetRawMode(MipiDsiMode);
#if defined(CONFIG_QNX)
    UINT32 ActualFreq;
#endif

    AmbaMisra_TouchUnused(&ColorOrder);

    if (pDisplayTiming == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        BytePerClock = VOUT_GetMipiDsiBytePerClk(MipiDsiRawMode);
        MipiLaneNum = VOUT_GetMipiDsiLaneNum(MipiDsiRawMode);
        ClkRatio = BytePerClock * 8U / MipiLaneNum;

        NumMipiLane = (UINT8)MipiLaneNum - 1U;
        AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.NumMipiLane = NumMipiLane;

#if defined(CONFIG_QNX)
        if (VoutChan == 0U) {
            AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_VIDEO_A, ClkRatio);
            AmbaSYS_SetIoClkFreq(AMBA_CLK_VOUTA, pDisplayTiming->PixelClkFreq, &ActualFreq);
        } else {
            AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_VIDEO_B, ClkRatio);
            AmbaSYS_SetIoClkFreq(AMBA_CLK_VOUTB, pDisplayTiming->PixelClkFreq, &ActualFreq);
        }
#else
        if (VoutChan == 0U) {
            (void)AmbaRTSL_PllSetVout0ClkHint(AMBA_PLL_VOUT_MIPI_DSI, ClkRatio);
            (void)AmbaRTSL_PllSetVout0Clk(pDisplayTiming->PixelClkFreq);
        } else {
            (void)AmbaRTSL_PllSetVout1ClkHint(AMBA_PLL_VOUT_MIPI_DSI, ClkRatio);
            (void)AmbaRTSL_PllSetVout1Clk(pDisplayTiming->PixelClkFreq);
        }
#endif
        /* Set MIPI PHY according to bit rate */
        VOUT_CalculateMphyConfig((UINT64)pDisplayTiming->PixelClkFreq * ClkRatio);

        /* Fill up the control */
        AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.FlipMode = 0U;
        AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.DigitalOutput = 1U;
        AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.FixedFormatSelect = 0U;
        AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.SelectCsi = 0U;

        /* According to Vout output mode definition */
        if ((MipiDsiRawMode == VOUT_MIPI_DSI_MODE_422_1LANE) ||
            (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_422_2LANE) ||
            (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_422_4LANE)) {
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.OutputMode = 11U;
        } else if ((MipiDsiRawMode == VOUT_MIPI_DSI_MODE_565_1LANE) ||
                   (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_565_2LANE) ||
                   (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_565_4LANE)) {
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.OutputMode = 12U;
        } else if ((MipiDsiRawMode == VOUT_MIPI_DSI_MODE_666_1LANE) ||
                   (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_666_2LANE) ||
                   (MipiDsiRawMode == VOUT_MIPI_DSI_MODE_666_4LANE)) {
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.OutputMode = 13U;
        } else {
            /* 888 1/2/4 lane */
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.OutputMode = 10U;
        }

        /* MIPI DSI related setting */
        AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiLineSync = 1U;
        AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiLineCount = 0U;
        AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiFrameCount = 0U;
        AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiLineDataFrameBlank = 0U;
        AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiLineSyncFrameBlank = 1U;
        AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiEccOrder = 0U;

        /* (TBD) */
        AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.MipiSyncEndEnable = 1U;

        /* MIPI Eotp Mode */
        if (AmbaVout_MipiGetEotpMode(MipiDsiMode) != 0U) {
            AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.MipiEotpEnable = 1U;
        } else {
            AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.MipiEotpEnable = 0U;
        }

        /* Fill up the video timing */
        RetVal = VOUT_SetMipiDsiTiming(pDisplayTiming, BytePerClock);

        /* MIPI D-PHY related setting */
        (void)AmbaRTSL_VoutMipiDsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_UP, AmbaVout_MipiGetDphyClkMode(MipiDsiMode));

        if (VoutChan == 0U) {
            AmbaCSL_Vout0SetMipiLineSync(1);
            AmbaCSL_Vout0SetMipiLineCount(0);
            AmbaCSL_Vout0SetMipiFrameCount(0);
            AmbaCSL_Vout0SetMipiLineDataBlank(0);
            AmbaCSL_Vout0SetMipiLineSyncBlank(1);
            AmbaCSL_Vout0SetMipiEccOrder(0);

            AmbaCSL_Vout0EnableDigitalOutput();

            /* Choose one mipi output mode as default */
            AmbaCSL_Vout0SetDigitalOutputMode(10);
        } else {
            AmbaCSL_Vout1SetMipiLineSync(1);
            AmbaCSL_Vout1SetMipiLineCount(0);
            AmbaCSL_Vout1SetMipiFrameCount(0);
            AmbaCSL_Vout1SetMipiLineDataBlank(0);
            AmbaCSL_Vout1SetMipiLineSyncBlank(1);
            AmbaCSL_Vout1SetMipiEccOrder(0);

            AmbaCSL_Vout1EnableDigitalOutput();

            /* Choose one mipi output mode as default */
            AmbaCSL_Vout1SetDigitalOutputMode(10);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiSetPhyCtrl - Set PHY control parameter
 *  @param[in] ParamID PHY parameter ID
 *  @param[in] ParamVal PHY parameter value
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    UINT32 RetVal;

    switch (ParamID) {
    case VOUT_PHY_MIPI_DPHY_POWER_UP:
        if (VoutChan == 0U) {
            /* Reset digital phy */
            /* DSI_CTRL_REG2[1]=1 (soft reset) */
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x17000002);

            /* Pre-init for send mipi dsi command */
            /* Default PHY value */
            AmbaCSL_Vout0MipiDsiSetCtrl0(0x31620313);
            AmbaCSL_Vout0MipiDsiSetCtrl1(0x00000007);
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x1700001f);
            AmbaCSL_Vout0MipiDsiSetAuxEn(1);
            AmbaCSL_Vout0MipiDsiSetPreEn(1);
            /* Default pre-emphasis */
            AmbaCSL_Vout0MipiDsiSetPre(1);
            AmbaCSL_Vout0MipiDsiSetTxMode(MIPI_DSI_PHY_MODE_DSI);

            /* MIPI DSI clock mode */
            AmbaCSL_Vout0MipiDsiSetClkMode(ParamVal);
        } else {
            /* Reset digital phy */
            /* DSI_CTRL_REG2[1]=1 (soft reset) */
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x17000002);

            /* Pre-init for send mipi dsi command */
            /* Default PHY value */
            AmbaCSL_Vout1MipiDsiSetCtrl0(0x31620313);
            AmbaCSL_Vout1MipiDsiSetCtrl1(0x00000007);
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x1700001f);
            AmbaCSL_Vout1MipiDsiSetAuxEn(1);
            AmbaCSL_Vout1MipiDsiSetPreEn(1);
            /* Default pre-emphasis */
            AmbaCSL_Vout1MipiDsiSetPre(1);
            AmbaCSL_Vout1MipiDsiSetTxMode(MIPI_DSI_PHY_MODE_DSI);

            /* MIPI DSI clock mode */
            AmbaCSL_Vout1MipiDsiSetClkMode(ParamVal);
        }

        RetVal = VOUT_ERR_NONE;
        break;
    case VOUT_PHY_MIPI_DPHY_POWER_DOWN:
        if (VoutChan == 0U) {
            /* Disable analog phy */
            /* DSI_CTRL_REG0[1:0] = 0 (this turns of internal clock divider) */
            /* DSI_CTRL_REG0[8]=0 (this turns of analog phy's bias) */
            /* DSI_CTRL_REG2[0] = 0 (this turn off clock channel) */
            /* DSI_CTRL_REG2[7:4] = 0 (this turn off data channel) */
            AmbaCSL_Vout0MipiDsiSetCtrl0(0x61300210);
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x17000000);

            /* Reset digital phy */
            /* DSI_CTRL_REG2[1]=1 (soft reset) */
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x17000002);
        } else {
            /* Disable analog phy */
            /* DSI_CTRL_REG0[1:0] = 0 (this turns of internal clock divider) */
            /* DSI_CTRL_REG0[8]=0 (this turns of analog phy's bias) */
            /* DSI_CTRL_REG2[0] = 0 (this turn off clock channel) */
            /* DSI_CTRL_REG2[7:4] = 0 (this turn off data channel) */
            AmbaCSL_Vout1MipiDsiSetCtrl0(0x61300210);
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x17000000);

            /* Reset digital phy */
            /* DSI_CTRL_REG2[1]=1 (soft reset) */
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x17000002);
        }
        RetVal = VOUT_ERR_NONE;
        break;
    default:
        RetVal = VOUT_ERR_ARG;
        break;
    };

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiGetStatus - Get MIPI DSI video interface related configurations
 *  @param[out] pMipiDsiConfig MIPI-DSI-specific vout configuration
 *  @param[out] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiGetStatus(AMBA_VOUT_MIPI_DSI_CONFIG_s *pMipiDsiConfig, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 ModeOffset, OutputMode;

    if (pMipiDsiConfig != NULL) {
        if (VoutChan == 0U) {
            ModeOffset = pAmbaVoutDisplay0_Reg->DispCtrl.NumMipiLane;
            OutputMode = pAmbaVoutDisplay0_Reg->DigitalOutputMode.OutputMode;
        } else {
            ModeOffset = pAmbaVoutDisplay1_Reg->DispCtrl.NumMipiLane;
            OutputMode = pAmbaVoutDisplay1_Reg->DigitalOutputMode.OutputMode;
        }

        if (OutputMode == 11U) {
            pMipiDsiConfig->MipiDsiMode = ModeOffset;
        } else if (OutputMode == 12U) {
            pMipiDsiConfig->MipiDsiMode = VOUT_MIPI_DSI_MODE_565_1LANE + ModeOffset;
        } else if (OutputMode == 13U) {
            pMipiDsiConfig->MipiDsiMode = VOUT_MIPI_DSI_MODE_666_1LANE + ModeOffset;
        } else if (OutputMode == 10U) {
            pMipiDsiConfig->MipiDsiMode = VOUT_MIPI_DSI_MODE_888_1LANE + ModeOffset;
        } else {
            RetVal = VOUT_ERR_PROTOCOL;
        }

        if (RetVal == VOUT_ERR_NONE) {
            pMipiDsiConfig->ColorOrder = 0U;
        }
    }

    if (pDisplayTiming != NULL) {
        if (VoutChan == 0U) {
            VOUT_GetDisplay0Timing(pDisplayTiming);
        } else {
            VOUT_GetDisplay1Timing(pDisplayTiming);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiSetCsc - Set CSC matrix as register format
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] MipiDsiMode MIPI DSI mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiSetCsc(const AMBA_VOUT_CSC_MATRIX_s *pCscMatrix, UINT32 MipiDsiMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetDigitalCsc(pCscMatrix);
    AmbaMisra_TouchUnused(&MipiDsiMode);

    return RetVal;
}

/**
 *  VOUT_MipiDsiCommandSetup - Setup one MIPI DSI command
 *  @param[in] DataType Packet data type
 *  @param[in] NumParam Number of parameters
 *  @param[in] pParam Pointer to parameters
 */
static void VOUT_MipiDsiCommandSetup(UINT32 DataType, UINT32 NumParam, const UINT8 *pParam)
{
    UINT32 CtrlRegVal = 0x0U;
    UINT32 RegVal, i;
    UINT8 WorkByte[16];

    if (pParam != NULL) {
        /* Up to 16 payload bytes */
        for (i = 0U; i < 16U; i++) {
            if (i < NumParam) {
                WorkByte[i] = pParam[i];
            } else {
                WorkByte[i] = 0x00U;
            }
        }

        /* Write back payload bytes */
        RegVal = WorkByte[3];
        RegVal = (RegVal << 8U) | WorkByte[2];
        RegVal = (RegVal << 8U) | WorkByte[1];
        RegVal = (RegVal << 8U) | WorkByte[0];
        if (VoutChan == 0U) {
            AmbaCSL_Mipi0SetCommandParam0(RegVal);
        } else {
            AmbaCSL_Mipi1SetCommandParam0(RegVal);
        }
        RegVal = WorkByte[7];
        RegVal = (RegVal << 8U) | WorkByte[6];
        RegVal = (RegVal << 8U) | WorkByte[5];
        RegVal = (RegVal << 8U) | WorkByte[4];
        if (VoutChan == 0U) {
            AmbaCSL_Mipi0SetCommandParam1(RegVal);
        } else {
            AmbaCSL_Mipi1SetCommandParam1(RegVal);
        }
        RegVal = WorkByte[11];
        RegVal = (RegVal << 8U) | WorkByte[10];
        RegVal = (RegVal << 8U) | WorkByte[9];
        RegVal = (RegVal << 8U) | WorkByte[8];
        if (VoutChan == 0U) {
            AmbaCSL_Mipi0SetCommandParam2(RegVal);
        } else {
            AmbaCSL_Mipi1SetCommandParam2(RegVal);
        }
        RegVal = WorkByte[15];
        RegVal = (RegVal << 8U) | WorkByte[14];
        RegVal = (RegVal << 8U) | WorkByte[13];
        RegVal = (RegVal << 8U) | WorkByte[12];
        if (VoutChan == 0U) {
            AmbaCSL_Mipi0SetCommandParam3(RegVal);
        } else {
            AmbaCSL_Mipi1SetCommandParam3(RegVal);
        }
    }

    /* Set command control */
    if (DataType == MIPI_DSI_PKT_GENERIC_NONIMANGE_DATA) {
        CtrlRegVal = 0x00040000U;
    } else if (DataType == MIPI_DSI_PKT_DCS_LONGWRITE) {
        CtrlRegVal = 0x00080000U;
    } else {
        CtrlRegVal = 0x00000000U;
    }

    if (VoutChan == 0U) {
        AmbaCSL_Mipi0SetCommandCtrl(CtrlRegVal);
    } else {
        AmbaCSL_Mipi1SetCommandCtrl(CtrlRegVal);
    }
}

/**
 *  AmbaRTSL_VoutMipiDsiDcsWrite - Send one DCS command packet
 *  @param[in] DcsCmd DCS command ID
 *  @param[in] NumParam Number of parameters
 *  @param[in] pParam Pointer to parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiDcsWrite(UINT32 DcsCmd, UINT32 NumParam, const UINT8 *pParam)
{
    AMBA_MIPI_DCS_SHORT_COMMAND_HEADER_s ShortCmdHeader;
    AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s LongCmdHeader;
    UINT8 WordCount0;
    UINT32 RegVal = 0U, RetVal = VOUT_ERR_NONE;

    if (NumParam < 2U) {
        /* Fill up header */
        AmbaMisra_TypeCast32(&ShortCmdHeader, &RegVal);
        ShortCmdHeader.CommandType = (UINT8)DcsCmd;
        if ((NumParam != 0U) && (pParam != NULL)) {
            ShortCmdHeader.DataType = MIPI_DSI_PKT_DCS_SHORTWRITE1;
            ShortCmdHeader.Parameter = *pParam;
        } else {
            ShortCmdHeader.DataType = MIPI_DSI_PKT_DCS_SHORTWRITE0;
        }
        AmbaMisra_TypeCast32(&RegVal, &ShortCmdHeader);
        if (VoutChan == 0U) {
            AmbaCSL_Mipi0SetCommandHeader(RegVal);
        } else {
            AmbaCSL_Mipi1SetCommandHeader(RegVal);
        }

        /* Setup Mipi command */
        VOUT_MipiDsiCommandSetup(ShortCmdHeader.DataType, NumParam, NULL);
    } else {
        /* Fill up header */
        AmbaMisra_TypeCast32(&LongCmdHeader, &RegVal);
        LongCmdHeader.DataType = MIPI_DSI_PKT_DCS_LONGWRITE;
        LongCmdHeader.CommandType = (UINT8)DcsCmd;
        WordCount0 = (UINT8)NumParam + (UINT8)1U;
        LongCmdHeader.WordCount0 = WordCount0;    /* WordCount0 = send byte + 1 */
        AmbaMisra_TypeCast32(&RegVal, &LongCmdHeader);
        if (VoutChan == 0U) {
            AmbaCSL_Mipi0SetCommandHeader(RegVal);
        } else {
            AmbaCSL_Mipi1SetCommandHeader(RegVal);
        }

        /* Setup Mipi command */
        VOUT_MipiDsiCommandSetup(MIPI_DSI_PKT_DCS_LONGWRITE, NumParam, pParam);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiNormalWrite - Send one normal packet
 *  @param[in] NumParam Number of parameters
 *  @param[in] pParam Pointer to parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiNormalWrite(UINT32 NumParam, const UINT8 *pParam)
{
    AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s LongCmdHeader;
    UINT32 RegVal = 0U, RetVal = VOUT_ERR_NONE;

    /* Fill up header */
    AmbaMisra_TypeCast32(&LongCmdHeader, &RegVal);
    LongCmdHeader.DataType = MIPI_DSI_PKT_GENERIC_NONIMANGE_DATA;
    LongCmdHeader.WordCount0 = (UINT8)NumParam; /* WordCount0 = send byte */
    AmbaMisra_TypeCast32(&RegVal, &LongCmdHeader);
    if (VoutChan == 0U) {
        AmbaCSL_Mipi0SetCommandHeader(RegVal);
    } else {
        AmbaCSL_Mipi1SetCommandHeader(RegVal);
    }

    /* Setup Mipi command */
    VOUT_MipiDsiCommandSetup(MIPI_DSI_PKT_GENERIC_NONIMANGE_DATA, NumParam, pParam);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiSetBlankPkt - Setup blank packets
 *  @param[in] pMipiDsiBlankPktCtrl Blank packets control
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiSetBlankPkt(const AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s *pMipiDsiBlankPktCtrl)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* Payload bytes */
    if (pMipiDsiBlankPktCtrl->LenHBP > 0U) {
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl0.PayloadBytes0 = (UINT16)pMipiDsiBlankPktCtrl->LenHBP;
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl0.UseBlank = 1;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl0.UseBlank = 0;
    }

    if (pMipiDsiBlankPktCtrl->LenHSA > 0U) {
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl0.PayloadBytes1 = (UINT16)pMipiDsiBlankPktCtrl->LenHSA;
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl0.SyncUseBlank = 1;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl0.SyncUseBlank = 0;
    }

    if (pMipiDsiBlankPktCtrl->LenHFP > 0U) {
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl1.PayloadBytes0 = (UINT16)pMipiDsiBlankPktCtrl->LenHFP;
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl1.UseBlank = 1;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl1.UseBlank = 0;
    }

    if (pMipiDsiBlankPktCtrl->LenBLLP > 0U) {
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl1.PayloadBytes1 = (UINT16)pMipiDsiBlankPktCtrl->LenBLLP;
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl1.SyncUseBlank = 1;
    } else {
        AmbaVOUT_DisplayConfig[VoutChan].MipiBlankCtrl1.SyncUseBlank = 0;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiSetBtaSrc - Set BTA enable source
 *  @param[in] BtaEnSrc BTA enable source
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiSetBtaSrc(UINT32 BtaEnSrc)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (BtaEnSrc >= NUM_MIPI_DSI_BTA_ENABLE) {
        RetVal = VOUT_ERR_ARG;
    } else {
        BtaEnable = BtaEnSrc;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiDsiGetAck - Get ack data with BTA
 *  @param[out] pAckData pointer to received ack data
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiDsiGetAckData(UINT32 *pAckData)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    /* Trigger BTA flow, need read commands to be sent before BTA flow. */
    if (VoutChan == 0U) {
        AmbaCSL_Vout0MipiDsiObsvSel(5);

        /* Tigger BTA */
        if (BtaEnable == MIPI_DSI_BTA_ENABLE_FROM_RCT) {
            AmbaCSL_Vout0MipiDsiBtaEnable(1);
            AmbaDelayCycles(AmbaRTSL_PllGetNumCpuCycleMs() << 4U);
        }

        /* Clear overflow */
        AmbaCSL_Vout0ClearStatus();

        /* Get ack data */
        AmbaCSL_Vout0MipiDsiBtaEnable(0);
        AmbaDelayCycles(AmbaRTSL_PllGetNumCpuCycleMs() << 4U);
        *pAckData = AmbaCSL_Vout0MipiDsiGetAckData();

        if (*pAckData != 0x84U) {
            /* Trigger dsi rct reset */
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x9700001fU);
            AmbaDelayCycles(AmbaRTSL_PllGetNumCpuCycleMs() << 4U);
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x1700001f);
        }

        /* Clear received data */
        AmbaCSL_Vout0MipiDsiBtaClrData(1);
        AmbaDelayCycles(AmbaRTSL_PllGetNumCpuCycleMs() << 4U);
        AmbaCSL_Vout0MipiDsiBtaClrData(0);
    } else {
        AmbaCSL_Vout1MipiDsiObsvSel(5);

        /* Tigger BTA */
        if (BtaEnable == MIPI_DSI_BTA_ENABLE_FROM_RCT) {
            AmbaCSL_Vout1MipiDsiBtaEnable(1);
            AmbaDelayCycles(AmbaRTSL_PllGetNumCpuCycleMs() << 4U);
        }

        /* Clear overflow */
        AmbaCSL_Vout1ClearStatus();

        /* Get ack data */
        AmbaCSL_Vout1MipiDsiBtaEnable(0);
        AmbaDelayCycles(AmbaRTSL_PllGetNumCpuCycleMs() << 4U);
        *pAckData = AmbaCSL_Vout1MipiDsiGetAckData();

        if (*pAckData != 0x84U) {
            /* Trigger dsi rct reset */
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x9700001fU);
            AmbaDelayCycles(AmbaRTSL_PllGetNumCpuCycleMs() << 4U);
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x1700001f);
        }

        /* Clear received data */
        AmbaCSL_Vout1MipiDsiBtaClrData(1);
        AmbaDelayCycles(AmbaRTSL_PllGetNumCpuCycleMs() << 4U);
        AmbaCSL_Vout1MipiDsiBtaClrData(0);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiEnable - Enable MIPI-CSI display
 *  @param[in] MipiCsiMode HDMI pixel format
 *  @param[in] ColorOrder Pixel bits transmission order
 *  @param[in] pDisplayTiming Video timing parameters
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiEnable(UINT32 MipiCsiMode, UINT32 ColorOrder, const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    UINT32 ClkRatio;
    UINT32 MipiCsiRawMode = AmbaVout_MipiGetRawMode(MipiCsiMode);
    UINT32 CsiBypassEn = AmbaVout_MipiCsiGetBypass(MipiCsiMode);
#if defined(CONFIG_QNX)
    UINT32 ActualFreq;
#endif

    AmbaMisra_TouchUnused(&ColorOrder);
    if (pDisplayTiming == NULL) {
        RetVal = VOUT_ERR_ARG;
    } else {
        if (MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_1LANE) {
            ClkRatio = 16U / 1U;
        } else if (MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_2LANE) {
            ClkRatio = 16U / 2U;
        } else {
            ClkRatio = 16U / 4U;
        }

#if defined(CONFIG_QNX)
        if (VoutChan == 0U) {
            AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_VIDEO_A, ClkRatio);
            AmbaSYS_SetIoClkFreq(AMBA_CLK_VOUTA, pDisplayTiming->PixelClkFreq, &ActualFreq);
        } else {
            AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_VIDEO_B, ClkRatio);
            AmbaSYS_SetIoClkFreq(AMBA_CLK_VOUTB, pDisplayTiming->PixelClkFreq, &ActualFreq);
        }
#else
        if (VoutChan == 0U) {
            (void)AmbaRTSL_PllSetVout0ClkHint(AMBA_PLL_VOUT_MIPI_CSI, ClkRatio);
            (void)AmbaRTSL_PllSetVout0Clk(pDisplayTiming->PixelClkFreq);
        } else {
            (void)AmbaRTSL_PllSetVout1ClkHint(AMBA_PLL_VOUT_MIPI_CSI, ClkRatio);
            (void)AmbaRTSL_PllSetVout1Clk(pDisplayTiming->PixelClkFreq);
        }
#endif
        /* Set MIPI PHY according to bit rate */
        VOUT_CalculateMphyConfig((UINT64)pDisplayTiming->PixelClkFreq * ClkRatio);

        if (CsiBypassEn != 0U) {
            /* Default to use continuous clock for bypass mode */
            (void)AmbaRTSL_VoutMipiCsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_UP, VOUT_MIPI_DPHY_CONT_CLK);

            /* Enter CSI bypass */
            if (VoutChan == 0U) {
                AmbaCSL_Vout0MipiCsiBypassEnable(1);
            } else {
                AmbaCSL_Vout1MipiCsiBypassEnable(1);
            }
        } else {
            /* Fill up the control */
            AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.FlipMode = 0U;
            AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.DigitalOutput = 1U;
            AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.FixedFormatSelect = 0U;
            AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.SelectCsi = 1U;

            /* According to Vout output mode definition */
            if (MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_1LANE) {
                AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.NumMipiLane = 0U;
            } else if (MipiCsiRawMode == VOUT_MIPI_CSI_MODE_422_2LANE) {
                AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.NumMipiLane = 1U;
            } else {
                AmbaVOUT_DisplayConfig[VoutChan].DispCtrl.NumMipiLane = 3U;
            }
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.OutputMode = 11U;

            /* Enable frame number increments by 1 for every Frame Sync packet (from 0x1 to 0xffff) by default */
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiLineSync = 0U;
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiLineCount = 0U;
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiFrameCount = 1U;
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiLineDataFrameBlank = 0U;
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiLineSyncFrameBlank = 0U;
            AmbaVOUT_DisplayConfig[VoutChan].DigitalOutputMode.MipiEccOrder = 0U;

            /* Fill up the video timing */
            RetVal = VOUT_SetMipiCsiTiming(pDisplayTiming);

            /* MIPI D-PHY related setting */
            if (AmbaVout_MipiGetDphyClkMode(MipiCsiMode) != 0U) {
                (void)AmbaRTSL_VoutMipiCsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_UP, VOUT_MIPI_DPHY_CONT_CLK);
            } else {
                (void)AmbaRTSL_VoutMipiCsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_UP, VOUT_MIPI_DPHY_NONCONT_CLK);
            }
        }
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
        if (VoutChan == 0U) {
            /* Reset digital phy */
            /* DSI_CTRL_REG2[1]=1 (soft reset) */
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x17000002);

            /* Pre-init for send mipi command */
            /* Default PHY value */
            AmbaCSL_Vout0MipiDsiSetCtrl0(0x61300313);
            AmbaCSL_Vout0MipiDsiSetCtrl1(0x00000007);
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x9700001fU);
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x1700001f);
            AmbaCSL_Vout0MipiDsiSetAuxEn(1);
            AmbaCSL_Vout0MipiDsiSetPreEn(1);
            /* Default pre-emphasis */
            AmbaCSL_Vout0MipiDsiSetPre(1);
            AmbaCSL_Vout0MipiDsiSetTxMode(MIPI_DSI_PHY_MODE_CSI);

            /* MIPI continuous mode */
            if (ParamVal == VOUT_MIPI_DPHY_CONT_CLK) {
                AmbaCSL_Vout0MipiDsiSetClkMode(1U);
            } else {
                AmbaCSL_Vout0MipiDsiSetClkMode(0U);
            }
        } else {
            /* Reset digital phy */
            /* DSI_CTRL_REG2[1]=1 (soft reset) */
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x17000002);

            /* Pre-init for send mipi command */
            /* Default PHY value */
            AmbaCSL_Vout1MipiDsiSetCtrl0(0x61300313);
            AmbaCSL_Vout1MipiDsiSetCtrl1(0x00000007);
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x9700001fU);
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x1700001f);
            AmbaCSL_Vout1MipiDsiSetAuxEn(1);
            AmbaCSL_Vout1MipiDsiSetPreEn(1);
            /* Default pre-emphasis */
            AmbaCSL_Vout1MipiDsiSetPre(1);
            AmbaCSL_Vout1MipiDsiSetTxMode(MIPI_DSI_PHY_MODE_CSI);

            /* MIPI continuous mode */
            if (ParamVal == VOUT_MIPI_DPHY_CONT_CLK) {
                AmbaCSL_Vout1MipiDsiSetClkMode(1U);
            } else {
                AmbaCSL_Vout1MipiDsiSetClkMode(0U);
            }
        }
        RetVal = VOUT_ERR_NONE;
        break;
    case VOUT_PHY_MIPI_DPHY_POWER_DOWN:
        if (VoutChan == 0U) {
            /* Disable analog phy */
            /* DSI_CTRL_REG0[1:0] = 0 (this turns of internal clock divider) */
            /* DSI_CTRL_REG0[8]=0 (this turns of analog phy's bias) */
            /* DSI_CTRL_REG2[0] = 0 (this turn off clock channel) */
            /* DSI_CTRL_REG2[7:4] = 0 (this turn off data channel) */
            AmbaCSL_Vout0MipiDsiSetCtrl0(0x61300210);
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x17000000);

            /* Reset digital phy */
            /* DSI_CTRL_REG2[1]=1 (soft reset) */
            AmbaCSL_Vout0MipiDsiSetCtrl2(0x17000002);
        } else {
            /* Disable analog phy */
            /* DSI_CTRL_REG0[1:0] = 0 (this turns of internal clock divider) */
            /* DSI_CTRL_REG0[8]=0 (this turns of analog phy's bias) */
            /* DSI_CTRL_REG2[0] = 0 (this turn off clock channel) */
            /* DSI_CTRL_REG2[7:4] = 0 (this turn off data channel) */
            AmbaCSL_Vout1MipiDsiSetCtrl0(0x61300210);
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x17000000);

            /* Reset digital phy */
            /* DSI_CTRL_REG2[1]=1 (soft reset) */
            AmbaCSL_Vout1MipiDsiSetCtrl2(0x17000002);
        }
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
    UINT32 NumMipiLane, OutputMode;

    if (pMipiCsiConfig != NULL) {
        if (VoutChan == 0U) {
            NumMipiLane = pAmbaVoutDisplay0_Reg->DispCtrl.NumMipiLane;
            OutputMode = pAmbaVoutDisplay0_Reg->DigitalOutputMode.OutputMode;
        } else {
            NumMipiLane = pAmbaVoutDisplay1_Reg->DispCtrl.NumMipiLane;
            OutputMode = pAmbaVoutDisplay1_Reg->DigitalOutputMode.OutputMode;
        }

        if (OutputMode != 11U) {
            RetVal = VOUT_ERR_PROTOCOL;
        } else {
            if (NumMipiLane == 0U) {
                pMipiCsiConfig->MipiCsiMode = VOUT_MIPI_CSI_MODE_422_1LANE;
            } else if (NumMipiLane == 1U) {
                pMipiCsiConfig->MipiCsiMode = VOUT_MIPI_CSI_MODE_422_2LANE;
            } else {
                pMipiCsiConfig->MipiCsiMode = VOUT_MIPI_CSI_MODE_422_4LANE;
            }
            pMipiCsiConfig->ColorOrder = 0U;
        }
    }

    if (pDisplayTiming != NULL) {
        if (VoutChan == 0U) {
            VOUT_GetDisplay0Timing(pDisplayTiming);
        } else {
            VOUT_GetDisplay1Timing(pDisplayTiming);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiSetCsc - Set CSC matrix as register format
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] MipiCsiMode MIPI CSI mode for reference
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiSetCsc(const AMBA_VOUT_CSC_MATRIX_s *pCscMatrix, UINT32 MipiCsiMode)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    VOUT_SetDigitalCsc(pCscMatrix);
    AmbaMisra_TouchUnused(&MipiCsiMode);

    return RetVal;
}

/**
 *  AmbaRTSL_VoutMipiCsiSetMaxFrame - Set mipi csi maxinum frame count
 *  @param[in] MaxFrameCount Maxinum frame count
 *  @return error code
 */
UINT32 AmbaRTSL_VoutMipiCsiSetMaxFrame(UINT32 MaxFrameCount)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChan == 0U) {
        AmbaCSL_Vout0SetMipiMaxFrame(MaxFrameCount);
    } else {
        AmbaCSL_Vout1SetMipiMaxFrame(MaxFrameCount);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0SetTimeout - Set Timeout for vout0
 *  @param[in] Timeout Timeout value
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0SetTimeout(UINT32 Timeout)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaCSL_Vout0SetTimeout(Timeout);

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1SetTimeout - Set Timeout for vout1
 *  @param[in] Timeout Timeout value
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1SetTimeout(UINT32 Timeout)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaCSL_Vout1SetTimeout(Timeout);

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0CheckFrameFrozen - Check vout0 frame is frozen or not
 *  @param[out] pStatus Frame status
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0CheckFrameFrozen(UINT32 *pStatus)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    static UINT32 LastLumaChecksum = 0U;
    static UINT32 LastChromaChecksum = 0U;
    UINT32 CurrentLumaChecksum, CurrentChromaChecksum;

    CurrentLumaChecksum = AmbaCSL_Vout0GetLumaChecksum();
    CurrentChromaChecksum = AmbaCSL_Vout0GetChromaChecksum();

    if ((LastLumaChecksum == CurrentLumaChecksum) && (LastChromaChecksum == CurrentChromaChecksum)) {
        /* Frame is frozen */
        *pStatus = 1U;
    } else {
        *pStatus = 0U;
    }

    LastLumaChecksum = CurrentLumaChecksum;
    LastChromaChecksum = CurrentChromaChecksum;

    return RetVal;
}

/**
 *  AmbaRTSL_Vout1CheckFrameFrozen - Check vout1 frame is frozen or not
 *  @param[out] pStatus Frame status
 *  @return error code
 */
UINT32 AmbaRTSL_Vout1CheckFrameFrozen(UINT32 *pStatus)
{
    UINT32 RetVal = VOUT_ERR_NONE;
    static UINT32 LastLumaChecksum = 0U;
    static UINT32 LastChromaChecksum = 0U;
    UINT32 CurrentLumaChecksum, CurrentChromaChecksum;

    CurrentLumaChecksum = AmbaCSL_Vout1GetLumaChecksum();
    CurrentChromaChecksum = AmbaCSL_Vout1GetChromaChecksum();

    if ((LastLumaChecksum == CurrentLumaChecksum) && (LastChromaChecksum == CurrentChromaChecksum)) {
        /* Frame is frozen */
        *pStatus = 1U;
    } else {
        *pStatus = 0U;
    }

    LastLumaChecksum = CurrentLumaChecksum;
    LastChromaChecksum = CurrentChromaChecksum;

    return RetVal;
}

/**
 *  AmbaRTSL_Vout0GetStatus - Get vout0 status
 *  @param[out] pStatus Vout0 status
 *  @return error code
 */
UINT32 AmbaRTSL_Vout0GetStatus(UINT32 *pStatus)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    *pStatus = AmbaCSL_Vout0GetStatus();

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

/**
 *  AmbaRTSL_VoutGetTopLevelStatus - Get vout top level status
 *  @param[out] pStatus Vout top level status
 *  @return error code
 */
UINT32 AmbaRTSL_VoutGetTopLevelStatus(UINT32 *pStatus)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    *pStatus = AmbaCSL_VoutGetErrors();

    return RetVal;
}

/**
 *  AmbaRTSL_VoutSetTopLevelMask - Set vout top level status mask
 *  @param[in] Mask Vout top level status mask
 *  @return error code
 */
UINT32 AmbaRTSL_VoutSetTopLevelMask(UINT32 Mask)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    AmbaCSL_VoutSetErrorMask(Mask);

    return RetVal;
}
