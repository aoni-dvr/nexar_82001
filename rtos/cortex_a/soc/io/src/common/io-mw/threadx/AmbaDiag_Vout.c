/**
 *  @file AmbaDiag_Vout.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details Diagnostic functions for video outputs
 *
 */

#if defined(CONFIG_QNX)
#include "Generic.h"
#include "hw/ambarella_clk.h"
#endif

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"
#include "AmbaKAL.h"

#include "AmbaSYS_Ctrl.h"
#include "AmbaVOUT_Def.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_VOUT.h"
#include "AmbaCSL_VOUT.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

static UINT32 CsiMode = 0U;

#if defined(CONFIG_LINUX) // Note: remove this if io utility for linux is available
#define IO_UtilityStringPrintUInt32 IoDiag_StringPrintUInt32
static UINT32 IoDiag_StringPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs)
{
    AmbaMisra_TouchUnused(&Count);

    return snprintf(pBuffer, BufferSize, pFmtString, *pArgs);
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define IO_UtilityStringPrintUInt64 IoDiag_StringPrintUInt64
static UINT32 IoDiag_StringPrintUInt64(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT64 *pArgs)
{
    AmbaMisra_TouchUnused(&Count);

    return snprintf(pBuffer, BufferSize, pFmtString, *pArgs);
}
#endif
#endif

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SYS_LOG_f LogFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)IO_UtilityStringPrintUInt32(StrBuf, (UINT32)sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    LogFunc(StrBuf);
}
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static void IoDiag_PrintFormattedInt64(const char *pFmtString, UINT64 Value, AMBA_SYS_LOG_f LogFunc)
{
    char StrBuf[64];
    UINT64 ArgUINT64[2];

    ArgUINT64[0] = Value;
    (void)IO_UtilityStringPrintUInt64(StrBuf, (UINT32)sizeof(StrBuf), pFmtString, 1U, ArgUINT64);
    LogFunc(StrBuf);
}
#endif

/**
 *  IoDiag_VoutGetDisp0 - Get Vout display0 info from vout registers
 *  @param[in] pDisplayTiming pointer to video timing
 *  @param[in] pFrameSyncPolarity frame sync signal polarity
 *  @param[in] pLineSyncPolarity line sync signal polarity
 */
static void IoDiag_VoutGetDisp0(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, UINT32 *pFrameSyncPolarity, UINT32 *pLineSyncPolarity)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *const pReg = pAmbaVoutDisplay0_Reg;
#else
    const AMBA_VOUT_DISPLAY0_CONFIG_REG_s *const pReg = pAmbaVoutDisplay0_Reg;
#endif
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */

#if defined(CONFIG_QNX)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTA, &pDisplayTiming->PixelClkFreq);
#else
    (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTLCD, &pDisplayTiming->PixelClkFreq);
#endif
#else
/* Non-QNX */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutAClk();
#else
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutLcdClk();
#endif
#endif

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

    if (pReg->DigitalOutputMode.VSyncPolarity == 0U) {
        *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
    } else {
        *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
    }
    if (pReg->DigitalOutputMode.HSyncPolarity == 0U) {
        *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
    } else {
        *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
    }

    pDisplayTiming->HsyncPulseWidth  = pReg->DigitalHSync.EndColumn;
    pDisplayTiming->HsyncPulseWidth -= pReg->DigitalHSync.StartColumn;
    pDisplayTiming->VsyncPulseWidth  = pReg->DigitalVSyncTopEnd.EndRow;
    pDisplayTiming->VsyncPulseWidth -= pReg->DigitalVSyncTopStart.StartRow;

    if (pReg->DispCtrl.DigitalOutput == 0U) {
        /* No Output */
        pDisplayTiming->HsyncPulseWidth = 0U;
        pDisplayTiming->VsyncPulseWidth = 0U;
    }

#if defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (pReg->DispCtrl.SelectCsi == 1U) {
        CsiMode = 1U;
        /* CSI timing */
        pDisplayTiming->HsyncPulseWidth += 1U;
        pDisplayTiming->VsyncPulseWidth += 1U;

        pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - 10U);
        pDisplayTiming->HsyncFrontPorch = (UINT16)((UINT16)FrameWidth - (pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch));

        pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - 10U);
        pDisplayTiming->VsyncFrontPorch = (UINT16)((UINT16)FrameHeight - (pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch));
    } else {
        CsiMode = 0U;
        /* DSI timing */
        pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pReg->DigitalHSync.EndColumn);
        pDisplayTiming->HsyncFrontPorch = (UINT16)((UINT16)FrameWidth - (pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels));

        pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pReg->DigitalVSyncTopEnd.EndRow);
        pDisplayTiming->VsyncFrontPorch = (UINT16)((UINT16)FrameHeight - (pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines));
    }
#else
    CsiMode = 0U;
    /* Digital or DSI timing */
    pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pReg->DigitalHSync.EndColumn);
    pDisplayTiming->HsyncFrontPorch = (UINT16)((UINT16)FrameWidth - (pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels));

    pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pReg->DigitalVSyncTopEnd.EndRow);
    pDisplayTiming->VsyncFrontPorch = (UINT16)((UINT16)FrameHeight - (pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines));
#endif
}

#if !defined(CONFIG_SOC_CV28)
/**
 *  IoDiag_VoutGetDisp1 - Get Vout display1 info from vout registers
 *  @param[in] pDisplayTiming pointer to video timing
 *  @param[in] pFrameSyncPolarity frame sync signal polarity
 *  @param[in] pLineSyncPolarity line sync signal polarity
 */
static void IoDiag_VoutGetDisp1(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, UINT32 *pFrameSyncPolarity, UINT32 *pLineSyncPolarity)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *const pReg = pAmbaVoutDisplay1_Reg;
#else
    const AMBA_VOUT_DISPLAY1_CONFIG_REG_s *const pReg = pAmbaVoutDisplay1_Reg;
#endif
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */

#if defined(CONFIG_QNX)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTB, &pDisplayTiming->PixelClkFreq);
#else
    (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTTV, &pDisplayTiming->PixelClkFreq);
#endif
#else
/* Non-QNX */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutBClk();
#else
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutTvClk();
#endif
#endif

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

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (pReg->DispCtrl.DigitalOutput != 0U) {
        if (pReg->DigitalOutputMode.VSyncPolarity == 0U) {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        if (pReg->DigitalOutputMode.HSyncPolarity == 0U) {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        pDisplayTiming->HsyncPulseWidth  = pReg->DigitalHSync.EndColumn;
        pDisplayTiming->HsyncPulseWidth -= pReg->DigitalHSync.StartColumn;
        pDisplayTiming->VsyncPulseWidth  = pReg->DigitalVSyncTopEnd.EndRow;
        pDisplayTiming->VsyncPulseWidth -= pReg->DigitalVSyncTopStart.StartRow;
    } else {
        /* No Output */
        pDisplayTiming->HsyncPulseWidth = 0U;
        pDisplayTiming->VsyncPulseWidth = 0U;
    }
#else
    if (pReg->DispCtrl.HdmiOutput != 0U) {
        if (pReg->HdmiOutputMode.VSyncPolarity == 0U) {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        if (pReg->HdmiOutputMode.HSyncPolarity == 0U) {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        pDisplayTiming->HsyncPulseWidth  = pReg->HdmiHSync.EndColumn;
        pDisplayTiming->HsyncPulseWidth -= pReg->HdmiHSync.StartColumn;
        pDisplayTiming->VsyncPulseWidth  = pReg->HdmiVSyncTopEnd.EndRow;
        pDisplayTiming->VsyncPulseWidth -= pReg->HdmiVSyncTopStart.StartRow;
#if !defined(CONFIG_SOC_H32)
    } else if (pReg->DispCtrl.DigitalOutput != 0U) {
        if (pReg->DigitalOutputMode.VSyncPolarity == 0U) {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        if (pReg->DigitalOutputMode.HSyncPolarity == 0U) {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        pDisplayTiming->HsyncPulseWidth  = pReg->DigitalHSync.EndColumn;
        pDisplayTiming->HsyncPulseWidth -= pReg->DigitalHSync.StartColumn;
        pDisplayTiming->VsyncPulseWidth  = pReg->DigitalVSyncTopEnd.EndRow;
        pDisplayTiming->VsyncPulseWidth -= pReg->DigitalVSyncTopStart.StartRow;
#endif
    } else if (pReg->DispCtrl.AnalogOutput != 0U) {
        if (pReg->AnalogOutputMode.VSyncPolarity == 0U) {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        if (pReg->AnalogOutputMode.HSyncPolarity == 0U) {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        pDisplayTiming->HsyncPulseWidth  = pReg->AnalogHSync.EndColumn;
        pDisplayTiming->HsyncPulseWidth -= pReg->AnalogHSync.StartColumn;
        pDisplayTiming->VsyncPulseWidth  = pReg->AnalogVSyncTopEnd.EndRow;
        pDisplayTiming->VsyncPulseWidth -= pReg->AnalogVSyncTopStart.StartRow;
    } else {
        /* No Output */
        pDisplayTiming->HsyncPulseWidth = 0U;
        pDisplayTiming->VsyncPulseWidth = 0U;
    }
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (pReg->DispCtrl.SelectCsi == 1U) {
        CsiMode = 1U;
        /* CSI timing */
        pDisplayTiming->HsyncPulseWidth += 1U;
        pDisplayTiming->VsyncPulseWidth += 1U;

        pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - 10U);
        pDisplayTiming->HsyncFrontPorch = (UINT16)((UINT16)FrameWidth - (pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch));

        pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - 10U);
        pDisplayTiming->VsyncFrontPorch = (UINT16)((UINT16)FrameHeight - (pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch));
    } else {
        CsiMode = 0U;
        /* DSI timing */
        pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pReg->DigitalHSync.EndColumn);
        pDisplayTiming->HsyncFrontPorch = (UINT16)((UINT16)FrameWidth - (pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels));

        pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pReg->DigitalVSyncTopEnd.EndRow);
        pDisplayTiming->VsyncFrontPorch = (UINT16)((UINT16)FrameHeight - (pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines));
    }
#else
    if (pReg->DispCtrl.HdmiOutput != 0U) {
        CsiMode = 0U;
        /* HDMI timing */
        pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pDisplayTiming->HsyncPulseWidth);
        pDisplayTiming->HsyncFrontPorch = (UINT16)(FrameWidth - (FrameActiveColStart + FrameActiveColWidth));

        pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pDisplayTiming->VsyncPulseWidth);
        pDisplayTiming->VsyncFrontPorch = (UINT16)(FrameHeight - (FrameActiveRowStart + FrameActiveRowHeight));
    } else if (pReg->DispCtrl.AnalogOutput != 0U) {
        CsiMode = 0U;
        /* Analog timing */
        pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pDisplayTiming->HsyncPulseWidth);
        pDisplayTiming->HsyncFrontPorch = (UINT16)(FrameWidth - (FrameActiveColStart + FrameActiveColWidth));

        pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pDisplayTiming->VsyncPulseWidth);
        pDisplayTiming->VsyncFrontPorch = (UINT16)(FrameHeight - (FrameActiveRowStart + FrameActiveRowHeight));
    } else {
        CsiMode = 1U;
         /* CSI timing */
        pDisplayTiming->HsyncPulseWidth += 1U;
        pDisplayTiming->VsyncPulseWidth += 1U;

        pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - 10U);
        pDisplayTiming->HsyncFrontPorch = (UINT16)((UINT16)FrameWidth - (pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch));

        pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - 10U);
        pDisplayTiming->VsyncFrontPorch = (UINT16)((UINT16)FrameHeight - (pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch));
       /* CSI timing */
    }
#endif
}
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/**
 *  IoDiag_VoutGetDisp2 - Get Vout display2 info from vout registers
 *  @param[in] pDisplayTiming pointer to video timing
 *  @param[in] pFrameSyncPolarity frame sync signal polarity
 *  @param[in] pLineSyncPolarity line sync signal polarity
 */
static void IoDiag_VoutGetDisp2(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, UINT32 *pFrameSyncPolarity, UINT32 *pLineSyncPolarity)
{
    const AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *const pReg = pAmbaVoutDisplay2_Reg;
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */

#if defined(CONFIG_QNX)
    (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTC, &pDisplayTiming->PixelClkFreq);
#else
/* Non-QNX */
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVoutCClk();
#endif

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
        if (pReg->HdmiOutputMode.VSyncPolarity == 0U) {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        if (pReg->HdmiOutputMode.HSyncPolarity == 0U) {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        pDisplayTiming->HsyncPulseWidth  = pReg->HdmiHSync.EndColumn;
        pDisplayTiming->HsyncPulseWidth -= pReg->HdmiHSync.StartColumn;
        pDisplayTiming->VsyncPulseWidth  = pReg->HdmiVSyncTopEnd.EndRow;
        pDisplayTiming->VsyncPulseWidth -= pReg->HdmiVSyncTopStart.StartRow;
    } else if (pReg->DispCtrl.AnalogOutput != 0U) {
        if (pReg->AnalogOutputMode.VSyncPolarity == 0U) {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pFrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
        if (pReg->AnalogOutputMode.HSyncPolarity == 0U) {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        } else {
            *pLineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        }
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
#endif

/**
 *  IoDiag_VoutShowTiming - Draw to the input video timing
 *  @param[in] pDisplayTiming pointer to video timing
 *  @param[in] FrameSyncPolarity frame sync signal polarity
 *  @param[in] LineSyncPolarity line sync signal polarity
 *  @param[in] LogFunc function to print messages on shell task
 */
static void IoDiag_VoutShowTiming(const AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, UINT32 FrameSyncPolarity, UINT32 LineSyncPolarity, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 FrameWidth, FrameHeight;
    UINT64 WorkUINT64;

    if (CsiMode == 1U) {
        FrameWidth = (UINT32)pDisplayTiming->HsyncFrontPorch + (UINT32)pDisplayTiming->HsyncPulseWidth + (UINT32)pDisplayTiming->HsyncBackPorch;
        FrameHeight = (UINT32)pDisplayTiming->VsyncFrontPorch + (UINT32)pDisplayTiming->VsyncPulseWidth + (UINT32)pDisplayTiming->VsyncBackPorch;
    } else {
        FrameWidth = (UINT32)pDisplayTiming->HsyncFrontPorch + (UINT32)pDisplayTiming->HsyncPulseWidth + (UINT32)pDisplayTiming->HsyncBackPorch + (UINT32)pDisplayTiming->ActivePixels;
        FrameHeight = (UINT32)pDisplayTiming->VsyncFrontPorch + (UINT32)pDisplayTiming->VsyncPulseWidth + (UINT32)pDisplayTiming->VsyncBackPorch + (UINT32)pDisplayTiming->ActiveLines;
    }

    LogFunc("[Diag][VOUT] Pixel Clock             ");
    IoDiag_PrintFormattedInt("%u.000 Hz\n", pDisplayTiming->PixelClkFreq, LogFunc);

    WorkUINT64 = pDisplayTiming->PixelClkFreq;
    WorkUINT64 *= 1000LLU;
    WorkUINT64 /= (UINT64)FrameWidth;
    LogFunc("[Diag][VOUT] Line Frequency          ");
    IoDiag_PrintFormattedInt("%d", (UINT32)(WorkUINT64 / 1000LLU), LogFunc);
    LogFunc(".");
    IoDiag_PrintFormattedInt("%3u Hz\n", (UINT32)(WorkUINT64 % 1000LLU), LogFunc);

    WorkUINT64 = pDisplayTiming->PixelClkFreq;
    WorkUINT64 *= 1000LLU;
    WorkUINT64 /= ((UINT64)FrameWidth * (UINT64)FrameHeight);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (pAmbaVoutDisplay2_Reg->HdmiOutputMode.OutputMode == 3U) {
        /* In YUV420 mode, PixelClk will be in half, need to double when obtain Framerate */
        WorkUINT64 *= 2LLU;
    }
#endif
    LogFunc("[Diag][VOUT] Frame Frequency         ");
    IoDiag_PrintFormattedInt("%d", (UINT32)(WorkUINT64 / 1000LLU), LogFunc);
    LogFunc(".");
    IoDiag_PrintFormattedInt("%3u Hz\n", (UINT32)(WorkUINT64 % 1000LLU), LogFunc);

    LogFunc("[Diag][VOUT] H Total Pixels          ");
    IoDiag_PrintFormattedInt("%u dot\n", FrameWidth, LogFunc);
    LogFunc("[Diag][VOUT] H Active Pixels         ");
    IoDiag_PrintFormattedInt("%u dot\n", pDisplayTiming->ActivePixels, LogFunc);
    LogFunc("[Diag][VOUT] H Sync Pixels           ");
    IoDiag_PrintFormattedInt("%u dot\n", pDisplayTiming->HsyncPulseWidth, LogFunc);
    LogFunc("[Diag][VOUT] H Back Porch Pixels     ");
    IoDiag_PrintFormattedInt("%u dot\n", pDisplayTiming->HsyncBackPorch, LogFunc);
    LogFunc("[Diag][VOUT] H Front Porch Pixels    ");
    IoDiag_PrintFormattedInt("%u dot\n", pDisplayTiming->HsyncFrontPorch, LogFunc);
    if (LineSyncPolarity == VOUT_SIGNAL_ACTIVE_HIGH) { /* H sync active-high */
        LogFunc("[Diag][VOUT] H Sync Polarity         Positive\n");
    } else { /* H sync active-low */
        LogFunc("[Diag][VOUT] H Sync Polarity         Negative\n");
    }

    LogFunc("[Diag][VOUT] V Total Lines           ");
    IoDiag_PrintFormattedInt("%u line\n", FrameHeight, LogFunc);
    LogFunc("[Diag][VOUT] V Active TOTAL          ");
    IoDiag_PrintFormattedInt("%u line\n", pDisplayTiming->ActiveLines, LogFunc);
    LogFunc("[Diag][VOUT] V Sync Lines            ");
    IoDiag_PrintFormattedInt("%u line (per field)\n", pDisplayTiming->VsyncPulseWidth, LogFunc);
    LogFunc("[Diag][VOUT] V Back Porch Lines      ");
    IoDiag_PrintFormattedInt("%u line (per field)\n", pDisplayTiming->VsyncBackPorch, LogFunc);
    LogFunc("[Diag][VOUT] V Front Porch Lines     ");
    IoDiag_PrintFormattedInt("%u line (per field)\n", pDisplayTiming->VsyncFrontPorch, LogFunc);
    if (FrameSyncPolarity == VOUT_SIGNAL_ACTIVE_HIGH) { /* V sync active-high */
        LogFunc("[Diag][VOUT] V Sync Polarity         Positive\n");
    } else { /* V sync active-low */
        LogFunc("[Diag][VOUT] V Sync Polarity         Negative\n");
    }

    if (pDisplayTiming->DisplayMethod == 1U) {
        LogFunc("[Diag][VOUT] Interlace               Interlace\n");
    } else {
        LogFunc("[Diag][VOUT] Interlace               NON-Interlace\n");
    }

#if 0
    LogFunc("[Diag][VOUT]           |    :  :     :  :      ");
    IoDiag_PrintFormattedInt("%-4u", FrameWidth, LogFunc);
    LogFunc("                    :\n");
    LogFunc("[Diag][VOUT]           |    :<---------------------------------------->:\n");
    LogFunc("[Diag][VOUT]           |____:  :     :  :______________________________:            ____\n");
    LogFunc("[Diag][VOUT] DataEnable|    |  :     :  |                              |           |\n");
    LogFunc("[Diag][VOUT]           |    |___________|                              |___________|\n");
    LogFunc("[Diag][VOUT]           |    :  :     :  :<---------------------------->:\n");
    LogFunc("[Diag][VOUT]           |    :  :     :  :            ");
    IoDiag_PrintFormattedInt("%-4u", pDisplayTiming->ActivePixels, LogFunc);
    LogFunc("              :\n");
    if (LineSyncPolarity == VOUT_SIGNAL_ACTIVE_HIGH) { /* H sync active-high */
        LogFunc("[Diag][VOUT]           |    :  :_____:  :                              :   _____\n");
        LogFunc("[Diag][VOUT]    H Sync |    :  | ");
        IoDiag_PrintFormattedInt("%-3u", pDisplayTiming->HsyncPulseWidth, LogFunc);
        LogFunc(" |  :                              :  |     |\n");
        LogFunc("[Diag][VOUT]           |_______|     |____________________________________|     |_______\n");
    } else { /* H sync active-low */
        LogFunc("[Diag][VOUT]           |_______:     :____________________________________       _______\n");
        LogFunc("[Diag][VOUT]    H Sync |    :  | ");
        IoDiag_PrintFormattedInt("%-3u", pDisplayTiming->HsyncPulseWidth, LogFunc);
        LogFunc(" |  :                              :  |     |\n");
        LogFunc("[Diag][VOUT]           |    :  |_____|  :                              :  |_____|\n");
    }
    LogFunc("[Diag][VOUT]           |    :  :     :  :                              :\n");
    LogFunc("[Diag][VOUT]           |____:__:_____:__:______________________________:________________\n");
    LogFunc("[Diag][VOUT]                 ");
    IoDiag_PrintFormattedInt("%-3u", pDisplayTiming->HsyncFrontPorch, LogFunc);
    LogFunc("      ");
    IoDiag_PrintFormattedInt("%-3u\n", pDisplayTiming->HsyncBackPorch, LogFunc);


    LogFunc("[Diag][VOUT]           |     :           :           :  ");
    IoDiag_PrintFormattedInt("%-4u", FrameHeight, LogFunc);
    LogFunc("     :                   :\n");
    LogFunc("[Diag][VOUT]           |     :<----------------------------------------------------->:\n");
    LogFunc("[Diag][VOUT]           |_____:           :           :           :___  ____//____  _____\n");
    LogFunc("[Diag][VOUT] DataEnable|     |           :           :           |   ||          ||  :\n");
    LogFunc("[Diag][VOUT]           |     |_____//__________//__________//____|   ||          ||  :\n");
    LogFunc("[Diag][VOUT]           |     :           :           :           :<----------------->:\n");
    LogFunc("[Diag][VOUT]           |     :           :           :           :        ");
    IoDiag_PrintFormattedInt("%-4u", pDisplayTiming->ActiveLines, LogFunc);
    LogFunc("       :\n");
    if (LineSyncPolarity == VOUT_SIGNAL_ACTIVE_HIGH) { /* H sync active-high */
        LogFunc("[Diag][VOUT]           |     :           :           :           :                   :  \n");
        LogFunc("[Diag][VOUT]    H Sync | ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  || \n");
        LogFunc("[Diag][VOUT]           |_||__||__||//||__||__||//||__||__||//||__||__||__||//||__||__||_\n");
    } else { /* H sync active-low */
        LogFunc("[Diag][VOUT]           |_  __: __  //  __: __  //  __: __  //  __: __  __  //  __  __: _\n");
        LogFunc("[Diag][VOUT]    H Sync | ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  || \n");
        LogFunc("[Diag][VOUT]           | ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  || \n");
    }
    if (FrameSyncPolarity == VOUT_SIGNAL_ACTIVE_HIGH) { /* V sync active-high */
        LogFunc("[Diag][VOUT]           |     :           :_____//____:           :                   :\n");
        LogFunc("[Diag][VOUT]    V Sync |     :           |           |           :                   :\n");
        LogFunc("[Diag][VOUT]           |___________//____|           |_____//______________//___________\n");
    } else { /* V sync active-low */
        LogFunc("[Diag][VOUT]           |___________//____:           :_____//______________//___________\n");
        LogFunc("[Diag][VOUT]    V Sync |     :           |           |           :                   :\n");
        LogFunc("[Diag][VOUT]           |     :           |_____//____|           :                   :\n");
    }
    LogFunc("[Diag][VOUT]           |_____:___________:___________:___________:___________________:__\n");
    LogFunc("[Diag][VOUT]                       ");
    IoDiag_PrintFormattedInt("%-3u", pDisplayTiming->VsyncFrontPorch, LogFunc);
    LogFunc("         ");
    IoDiag_PrintFormattedInt("%-3u", pDisplayTiming->VsyncPulseWidth, LogFunc);
    LogFunc("         ");
    IoDiag_PrintFormattedInt("%-3u \n", pDisplayTiming->VsyncBackPorch, LogFunc);
#endif
}

/**
 *  IoDiag_VoutDumpRegs - Dump Vout and PHY registers
 *  @param[in] VoutChan Vout channel
 */
static void IoDiag_VoutDumpRegs(UINT32 VoutChan, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = VOUT_ERR_NONE;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    ULONG PhysicalAddr = AMBA_CORTEX_A76_VOUT_BASE_ADDR;
#else
    ULONG PhysicalAddr = AMBA_CORTEX_A53_VOUT_BASE_ADDR;
#endif
    ULONG BaseAddr, DumpAddr;
    ULONG VoutDispAddr, VoutTopAddr;
    const UINT32* pReg;
    ULONG i;

    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        VoutDispAddr = PhysicalAddr + VOUT_DISPLAY0_REG_OFFSET;
        AmbaMisra_TypeCast(&BaseAddr, &pAmbaVoutDisplay0_Reg);
        LogFunc("VOUT Display0 Registers:\n");
#if !defined(CONFIG_SOC_CV28)
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        VoutDispAddr = PhysicalAddr + VOUT_DISPLAY1_REG_OFFSET;
        AmbaMisra_TypeCast(&BaseAddr, &pAmbaVoutDisplay1_Reg);
        LogFunc("VOUT Display1 Registers:\n");
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (VoutChan == AMBA_VOUT_CHANNEL2) {
        VoutDispAddr = PhysicalAddr + VOUT_DISPLAY2_REG_OFFSET;
        AmbaMisra_TypeCast(&BaseAddr, &pAmbaVoutDisplay2_Reg);
        LogFunc("VOUT Display2 Registers:\n");
#endif
    } else {
        LogFunc("Invalid VOUT Channel!\n");
        RetVal = VOUT_ERR_ARG;
    }

    if (RetVal == VOUT_ERR_NONE) {
        for(i = 0U; i < 128U; i += 4U) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            IoDiag_PrintFormattedInt64("0x%llX:  ", VoutDispAddr + (i * 4U), LogFunc);
#else
            IoDiag_PrintFormattedInt("0x%08X:  ", VoutDispAddr + (i * 4U), LogFunc);
#endif
            DumpAddr = BaseAddr + (i * 4U);
            AmbaMisra_TypeCast(&pReg, &DumpAddr);
            IoDiag_PrintFormattedInt("0x%08X ", *pReg, LogFunc);

            DumpAddr = BaseAddr + ((i + 1U) * 4U);
            AmbaMisra_TypeCast(&pReg, &DumpAddr);
            IoDiag_PrintFormattedInt("0x%08X ", *pReg, LogFunc);

            DumpAddr = BaseAddr + ((i + 2U) * 4U);
            AmbaMisra_TypeCast(&pReg, &DumpAddr);
            IoDiag_PrintFormattedInt("0x%08X ", *pReg, LogFunc);

            DumpAddr = BaseAddr + ((i + 3U) * 4U);
            AmbaMisra_TypeCast(&pReg, &DumpAddr);
            IoDiag_PrintFormattedInt("0x%08X \n", *pReg, LogFunc);
        }

        VoutTopAddr = PhysicalAddr + VOUT_TOP_REG_OFFSET;
        AmbaMisra_TypeCast(&BaseAddr, &pAmbaVoutTop_Reg);
        LogFunc("VOUT Top Registers:\n");

        for(i = 0U; i < 8U; i += 4U) {
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            IoDiag_PrintFormattedInt64("0x%llX:  ", VoutTopAddr + (i * 4U), LogFunc);
#else
            IoDiag_PrintFormattedInt("0x%08X:  ", VoutTopAddr + (i * 4U), LogFunc);
#endif
            DumpAddr = BaseAddr + (i * 4U);
            AmbaMisra_TypeCast(&pReg, &DumpAddr);
            IoDiag_PrintFormattedInt("0x%08X ", *pReg, LogFunc);

            DumpAddr = BaseAddr + ((i + 1U) * 4U);
            AmbaMisra_TypeCast(&pReg, &DumpAddr);
            IoDiag_PrintFormattedInt("0x%08X ", *pReg, LogFunc);

            DumpAddr = BaseAddr + ((i + 2U) * 4U);
            AmbaMisra_TypeCast(&pReg, &DumpAddr);
            IoDiag_PrintFormattedInt("0x%08X ", *pReg, LogFunc);

            DumpAddr = BaseAddr + ((i + 3U) * 4U);
            AmbaMisra_TypeCast(&pReg, &DumpAddr);
            IoDiag_PrintFormattedInt("0x%08X \n", *pReg, LogFunc);
        }
    }
}

/**
 *  IoDiag_VoutShowStatus - Show Vout display status
 *  @param[in] VoutChan Vout channel
 */
static void IoDiag_VoutShowStatus(UINT32 VoutChan, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 StatusData;

    LogFunc("Status:\n");
    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        StatusData = pAmbaVoutDisplay0_Reg->DispStatus;

        IoDiag_PrintFormattedInt(" DIGITAL_UNDERFLOW:  %d\n", ((UINT32)StatusData >> 29U) & 0x1U, LogFunc);
#if defined(CONFIG_SOC_CV28)
        IoDiag_PrintFormattedInt(" ANALOG_UNDERFLOW:   %d\n", ((UINT32)StatusData >> 28U) & 0x1U, LogFunc);
#endif
#if !defined(CONFIG_SOC_H32)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_CV28)
        IoDiag_PrintFormattedInt(" MIPI_FIFO_OVERFLOW: %d\n", ((UINT32)StatusData >> 5U) & 0x1U, LogFunc);
#else
        IoDiag_PrintFormattedInt(" MIPI_FIFO_OVERFLOW: %d\n", ((UINT32)StatusData >> 4U) & 0x1U, LogFunc);
#endif
#endif

#if !defined(CONFIG_SOC_CV28)
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        StatusData = pAmbaVoutDisplay1_Reg->DispStatus;

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        IoDiag_PrintFormattedInt(" DIGITAL_UNDERFLOW:  %d\n", ((UINT32)StatusData >> 29U) & 0x1U, LogFunc);
        IoDiag_PrintFormattedInt(" MIPI_FIFO_OVERFLOW: %d\n", ((UINT32)StatusData >> 5U) & 0x1U, LogFunc);
#else
        IoDiag_PrintFormattedInt(" ANALOG_UNDERFLOW:   %d\n", ((UINT32)StatusData >> 28U) & 0x1U, LogFunc);
        IoDiag_PrintFormattedInt(" HDMI_UNDERFLOW:     %d\n", ((UINT32)StatusData >> 27U) & 0x1U, LogFunc);
#endif
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (VoutChan == AMBA_VOUT_CHANNEL2) {
        StatusData = pAmbaVoutDisplay2_Reg->DispStatus;

        IoDiag_PrintFormattedInt(" ANALOG_UNDERFLOW:   %d\n", ((UINT32)StatusData >> 28U) & 0x1U, LogFunc);
        IoDiag_PrintFormattedInt(" HDMI_UNDERFLOW:     %d\n", ((UINT32)StatusData >> 27U) & 0x1U, LogFunc);
#endif
    } else {
        /* Do nothing */
    }
}

/**
 *  AmbaDiag_VoutShowInfo - Show Vout display 0/1 info
 *  @param[in] VoutChan Vout channel
 *  @return error code
 */
UINT32 AmbaDiag_VoutShowInfo(UINT32 VoutChan, AMBA_SYS_LOG_f LogFunc)
{
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming = {0};
    UINT32 FrameSyncPolarity = 0U, LineSyncPolarity = 0U;
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        IoDiag_VoutGetDisp0(&DisplayTiming, &FrameSyncPolarity, &LineSyncPolarity);
#if !defined(CONFIG_SOC_CV28)
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        IoDiag_VoutGetDisp1(&DisplayTiming, &FrameSyncPolarity, &LineSyncPolarity);
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (VoutChan == AMBA_VOUT_CHANNEL2) {
        IoDiag_VoutGetDisp2(&DisplayTiming, &FrameSyncPolarity, &LineSyncPolarity);
#endif
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    if (RetVal == VOUT_ERR_NONE) {
        IoDiag_VoutShowTiming(&DisplayTiming, FrameSyncPolarity, LineSyncPolarity, LogFunc);
        LogFunc("---------------------------------------------------------\n");
        IoDiag_VoutShowStatus(VoutChan, LogFunc);
        LogFunc("---------------------------------------------------------\n");
        IoDiag_VoutDumpRegs(VoutChan, LogFunc);
    }

    return RetVal;
}

/**
 *  AmbaDiag_VoutGetMmioAddr - Get Vout mmio base address
 *  @param[in] MmioID specify which sub-module is interested
 *  @param[out] pMmioAddr base address of the sub-module
 *  @param[out] pMmioSize range of the mmio addresses
 *  @return error code
 */
UINT32 AmbaDiag_VoutGetMmioAddr(UINT32 MmioID, ULONG *pMmioAddr, UINT32 *pMmioSize)
{
    UINT32 RetVal = VOUT_ERR_NONE;

    if (MmioID == VOUT_MMIO_DISP0) {
        AmbaMisra_TypeCast(pMmioAddr, &pAmbaVoutDisplay0_Reg);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        *pMmioSize = (UINT32)(sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s));
#else
        *pMmioSize = (UINT32)(sizeof(AMBA_VOUT_DISPLAY0_CONFIG_REG_s));
#endif
#if !defined(CONFIG_SOC_CV28)
    } else if (MmioID == VOUT_MMIO_DISP1) {
        AmbaMisra_TypeCast(pMmioAddr, &pAmbaVoutDisplay1_Reg);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        *pMmioSize = (UINT32)(sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s));
#else
        *pMmioSize = (UINT32)(sizeof(AMBA_VOUT_DISPLAY1_CONFIG_REG_s));
#endif
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (MmioID == VOUT_MMIO_DISP2) {
        AmbaMisra_TypeCast(pMmioAddr, &pAmbaVoutDisplay2_Reg);
        *pMmioSize = (UINT32)(sizeof(AMBA_VOUT_DISPLAY_C_CONFIG_REG_s));
#endif
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaDiag_VoutMipiTestPattern - Enable MIPI test pattern output
 *  @return error code
 */
UINT32 AmbaDiag_VoutMipiTestPattern(void)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    RetVal = AmbaRTSL_VoutMipiTestPattern();

    return RetVal;
#else
    /* Not supported API */
    return VOUT_ERR_INVALID_API;
#endif
}

/**
 *  AmbaDiag_VoutConfigThrDbgPort - Configure Vout display through debug port
 *  @param[in] VoutChan Target vout channel
 *  @return error code
 */
UINT32 AmbaDiag_VoutConfigThrDbgPort(UINT32 VoutChan)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 RetVal = VOUT_ERR_NONE;

    if (VoutChan == AMBA_VOUT_CHANNEL0) {
        RetVal = AmbaRTSL_VoutConfigThrDbgPort(VoutChan, pAmbaVoutDisplay0_Reg);
    } else if (VoutChan == AMBA_VOUT_CHANNEL1) {
        RetVal = AmbaRTSL_VoutConfigThrDbgPort(VoutChan, pAmbaVoutDisplay1_Reg);
    } else if (VoutChan == AMBA_VOUT_CHANNEL2) {
        RetVal = AmbaRTSL_VoutConfigThrDbgPort(VoutChan, pAmbaVoutDisplay2_Reg);
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
#else
    /* Not supported API */
    (void)VoutChan;
    return VOUT_ERR_INVALID_API;
#endif
}
