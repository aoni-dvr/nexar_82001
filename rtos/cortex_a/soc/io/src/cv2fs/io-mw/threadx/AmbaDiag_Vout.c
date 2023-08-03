/**
 *  @file AmbaDiag_Vout.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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

#include "AmbaSYS_Ctrl.h"
#include "AmbaVOUT_Ctrl.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_VOUT.h"


static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SYS_LOG_f LogFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)IO_UtilityStringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    LogFunc(StrBuf);
}

/**
 *  IoDiag_VoutGetDisp0 - Get Vout display0 info from vout registers
 *  @param[in] pDisplayTiming pointer to video timing
 *  @param[in] pFrameSyncPolarity frame sync signal polarity
 *  @param[in] pLineSyncPolarity line sync signal polarity
 */
static void IoDiag_VoutGetDisp0(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, UINT32 *pFrameSyncPolarity, UINT32 *pLineSyncPolarity)
{
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *const pReg = pAmbaVoutDisplay0_Reg;
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */

#if defined(CONFIG_QNX)
    (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTA, &pDisplayTiming->PixelClkFreq);
#else
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVout0Clk();
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

    pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pReg->DigitalHSync.EndColumn);
    pDisplayTiming->HsyncFrontPorch = (UINT16)((UINT16)FrameWidth - (pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels));

    pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pReg->DigitalVSyncTopEnd.EndRow);
    pDisplayTiming->VsyncFrontPorch = (UINT16)((UINT16)FrameHeight - (pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines));
}

/**
 *  IoDiag_VoutGetDisp1 - Get Vout display1 info from vout registers
 *  @param[in] pDisplayTiming pointer to video timing
 *  @param[in] pFrameSyncPolarity frame sync signal polarity
 *  @param[in] pLineSyncPolarity line sync signal polarity
 */
static void IoDiag_VoutGetDisp1(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming, UINT32 *pFrameSyncPolarity, UINT32 *pLineSyncPolarity)
{
    const AMBA_VOUT_DISPLAY_CONFIG_REG_s *const pReg = pAmbaVoutDisplay1_Reg;
    UINT32 FrameActiveColStart, FrameActiveColWidth, FrameActiveRowStart, FrameActiveRowHeight;
    UINT32 FrameWidth, FrameHeight; /* H-total and V-total */

#if defined(CONFIG_QNX)
    (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_VOUTB, &pDisplayTiming->PixelClkFreq);
#else
    pDisplayTiming->PixelClkFreq = AmbaRTSL_PllGetVout1Clk();
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

    pDisplayTiming->HsyncBackPorch = (UINT16)(FrameActiveColStart - pReg->DigitalHSync.EndColumn);
    pDisplayTiming->HsyncFrontPorch = (UINT16)((UINT16)FrameWidth - (pDisplayTiming->HsyncPulseWidth + pDisplayTiming->HsyncBackPorch + pDisplayTiming->ActivePixels));

    pDisplayTiming->VsyncBackPorch = (UINT16)(FrameActiveRowStart - pReg->DigitalVSyncTopEnd.EndRow);
    pDisplayTiming->VsyncFrontPorch = (UINT16)((UINT16)FrameHeight - (pDisplayTiming->VsyncPulseWidth + pDisplayTiming->VsyncBackPorch + pDisplayTiming->ActiveLines));
}

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

    FrameWidth = (UINT32)pDisplayTiming->HsyncFrontPorch + (UINT32)pDisplayTiming->HsyncPulseWidth + (UINT32)pDisplayTiming->HsyncBackPorch + (UINT32)pDisplayTiming->ActivePixels;
    FrameHeight = (UINT32)pDisplayTiming->VsyncFrontPorch + (UINT32)pDisplayTiming->VsyncPulseWidth + (UINT32)pDisplayTiming->VsyncBackPorch + (UINT32)pDisplayTiming->ActiveLines;

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

    if (VoutChan == 0U) {
        IoDiag_VoutGetDisp0(&DisplayTiming, &FrameSyncPolarity, &LineSyncPolarity);
    } else if (VoutChan == 1U) {
        IoDiag_VoutGetDisp1(&DisplayTiming, &FrameSyncPolarity, &LineSyncPolarity);
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    if (RetVal == VOUT_ERR_NONE) {
        IoDiag_VoutShowTiming(&DisplayTiming, FrameSyncPolarity, LineSyncPolarity, LogFunc);
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
        *pMmioSize = sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s);
    } else if (MmioID == VOUT_MMIO_DISP1) {
        AmbaMisra_TypeCast(pMmioAddr, &pAmbaVoutDisplay1_Reg);
        *pMmioSize = sizeof(AMBA_VOUT_DISPLAY_CONFIG_REG_s);
    } else {
        RetVal = VOUT_ERR_ARG;
    }

    return RetVal;
}

