/**
 *  @file AmbaRTSL_PLL_SetClkFreq.c
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
 *  @details PLL RTSL driver APIs for Setting Clock Frequencies
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaWrap.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_PLL.h"

typedef struct {
    UINT32  IntProg:    7;
    UINT32  VcoType:    1;  /* 0 = Single-Ended VCO, 1 = Differenial VCO */
    UINT32  Sdiv:       4;
    UINT32  Sout:       4;
    UINT32  Fsdiv:      4;
    UINT32  Fsout:      4;
    UINT32  VcoDiv:     4;
} AMBA_PLL_COEFF_s;

#define MAX_PLL_VCO_FREQ_CORE_L1    900000000UL     /* max core PLL VCO frequency for vco range level1 */
#define MAX_PLL_VCO_FREQ_CORE_L2    1200000000UL    /* max core PLL VCO frequency for vco range level2 */
#define MAX_PLL_VCO_FREQ_CORE_L3    1800000000UL    /* max core PLL VCO frequency for vco range level3 */
#define MIN_PLL_VCO_FREQ_CORE_L1    530000000UL     /* min core PLL VCO frequency */

#define MAX_PLL_VCO_FREQ_HDMI_L1    4570000000ULL   /* max hdmi PLL VCO frequency for vco range level1 */
#define MAX_PLL_VCO_FREQ_HDMI_L2    5400000000ULL   /* max hdmi PLL VCO frequency for vco range level2 */
#define MIN_PLL_VCO_FREQ_HDMI_L1    2470000000ULL   /* min hdmi PLL VCO frequency */

static FLOAT AmbaPllVcoFreq[AMBA_NUM_PLL];          /* VCO Frequency (Hz) */

static UINT32 AmbaPllVoutLcdClkMode = AMBA_PLL_VOUT_LCD_MIPI_DSI;
static UINT32 AmbaPllVoutLcdClkRatio = 1U;

static inline void PLL_SetupPllVcoRangeVal(UINT32 PllType, UINT64 VcoFreq, AMBA_PLL_CTRL_REGS_VAL_s *pCurPllCtrlRegsVal)
{
    AMBA_PLL_CTRL3_REG_s RegVal;

    if (AmbaWrap_memcpy(&RegVal, &pCurPllCtrlRegsVal->Ctrl3, 4U) == 0U ) {

        if (PllType == 0U) {
            if (VcoFreq <= MIN_PLL_VCO_FREQ_CORE_L1) {
                RegVal.VcoRange = 0U;
            } else if (VcoFreq <= MAX_PLL_VCO_FREQ_CORE_L1) {
                RegVal.VcoRange = 1U;
            } else if (VcoFreq <= MAX_PLL_VCO_FREQ_CORE_L2) {
                RegVal.VcoRange = 2U;
            } else {
                RegVal.VcoRange = 3U;
            }
        } else {
            if (VcoFreq <= MIN_PLL_VCO_FREQ_HDMI_L1) {
                RegVal.VcoRange = 0U;
            } else if (VcoFreq <= MAX_PLL_VCO_FREQ_HDMI_L1) {
                RegVal.VcoRange = 1U;
            } else if (VcoFreq <= MAX_PLL_VCO_FREQ_HDMI_L2) {
                RegVal.VcoRange = 2U;
            } else {
                RegVal.VcoRange = 3U;
            }
        }
    }

    if (AmbaWrap_memcpy(&pCurPllCtrlRegsVal->Ctrl3, &RegVal, 4U) != 0U) {
        /* For misra-c */
    }
}

static inline void PLL_SetupCorePostDivVal(UINT32 PllOutFreq, UINT32 *pPostScaler)
{
    if (PllOutFreq <= MIN_PLL_VCO_FREQ_CORE_L1) {
        *pPostScaler = (UINT32)(MIN_PLL_VCO_FREQ_CORE_L1 / PllOutFreq);
        *pPostScaler = *pPostScaler + 1U;
    }
}

/**
 *  PLL_CalculateAdjCoeff - Auto-calculate coefficients for the specified pll output frequency value
 *  @param[in] RefFreq Reference clock frequency of the pll
 *  @param[in] PllOutFreq Target pll output frequency
 *  @param[out] pPllCoeff pointer to the pll coefficients
 */
static void PLL_CalculateAdjCoeff(UINT32 RefFreq, UINT32 PllOutFreq, AMBA_PLL_COEFF_s *pPllCoeff)
{
    UINT32 TargetSout, IntProg = 0;
    UINT32 MaxVco = 0;

    if (RefFreq != 0U) {
        if (PllOutFreq > MAX_PLL_VCO_FREQ_CORE_L1) {
            MaxVco = (UINT32)MAX_PLL_VCO_FREQ_CORE_L2;
        } else {
            MaxVco = (UINT32)MAX_PLL_VCO_FREQ_CORE_L1;
        }

        TargetSout = (MaxVco / PllOutFreq) - 1U;
        if (MaxVco < PllOutFreq) {
            pPllCoeff->Sout = 0U;
            pPllCoeff->Sdiv = 0U;
        } else if (TargetSout > 0xfU) {
            pPllCoeff->Sout = 0xfU;
            pPllCoeff->Sdiv = 0xfU;
        } else {
            pPllCoeff->Sdiv = (UINT8)TargetSout;
            pPllCoeff->Sout = (UINT8)TargetSout;
        }

        /* IntProg = PllOutFreq * (Sout + 1) / (Sdiv + 1) - 1 */
        if (PllOutFreq < RefFreq) {
            IntProg = 0U;
        } else {
            IntProg = (PllOutFreq / RefFreq) - 1U;
            if (IntProg > 0x7fU) {
                IntProg = 0x7fU;
            }
        }
        pPllCoeff->IntProg = (UINT8)IntProg;
    }
}

/**
 *  PLL_CalculateAdjCoeffHalf - Auto-calculate coefficients for the specified pll output frequency value
 *  @param[in] RefFreq Reference clock frequency of the pll
 *  @param[in] PllOutFreq Target pll output frequency
 *  @param[out] pPllCoeff pointer to the pll coefficients
 */
static void PLL_CalculateAdjCoeffHalf(UINT32 RefFreq, UINT32 PllOutFreq, AMBA_PLL_COEFF_s *pPllCoeff)
{
    UINT32 RefFreq12 = RefFreq / 2U;
    UINT32 TargetSout, IntProg = 0;
    UINT32 MaxVco = 0;

    if (PllOutFreq > (MAX_PLL_VCO_FREQ_CORE_L2 / 2U)) {
        MaxVco = (UINT32)(MAX_PLL_VCO_FREQ_CORE_L3 / 2U);
    } else if (PllOutFreq > (MAX_PLL_VCO_FREQ_CORE_L1 / 2U)) {
        MaxVco = (UINT32)(MAX_PLL_VCO_FREQ_CORE_L2 / 2U);
    } else {
        MaxVco = (UINT32)(MAX_PLL_VCO_FREQ_CORE_L1 / 2U);
    }

    TargetSout = ((MaxVco / PllOutFreq) * 2U) - 1U;
    if (MaxVco < PllOutFreq) {
        /* PLL out is very high */
        pPllCoeff->Sout = 1U;
        pPllCoeff->Sdiv = 0U;
    } else if (TargetSout > 0xfU) {
        /* PLL out is vrey low */
        pPllCoeff->Sout = 0xfU;
        pPllCoeff->Sdiv = 0x7U;
    } else {
        /* normal PLL out */
        pPllCoeff->Sout = (UINT8)TargetSout;
        pPllCoeff->Sdiv = (UINT8)(((TargetSout + 1U) / 2U) - 1U);
    }

    /* IntProg = PllOutFreq * (Sout + 1) / (Sdiv + 1) - 1 */
    if (PllOutFreq < RefFreq12) {
        IntProg = 0U;
    } else {
        IntProg = (PllOutFreq / RefFreq12) - 1U;
        if (IntProg > 0x7fU) {
            IntProg = 0x7fU;
        }
    }
    pPllCoeff->IntProg = (UINT8)IntProg;
}

/**
 *  PLL_AssignAdjCoeff - Assign coefficients for the specified pll output frequency value
 *  @param[in] PllIdx PLL ID
 *  @param[in] PllCoeff PLL coefficients
 *  @param[in,out] pCurPllCtrlRegsVal A copy of the current pll control register values
 */
static void PLL_AssignAdjCoeff(UINT32 PllIdx, const AMBA_PLL_COEFF_s *PllCoeff, AMBA_PLL_CTRL_REGS_VAL_s *pCurPllCtrlRegsVal)
{
    AMBA_PLL_CTRL_REG_s RegVal;

    if (AmbaWrap_memcpy(&RegVal, &pCurPllCtrlRegsVal->Ctrl, 4U) == 0U) {
        RegVal.Sdiv = (UINT8)PllCoeff->Sdiv;
        RegVal.Sout = (UINT8)PllCoeff->Sout;
        RegVal.IntProg = (UINT8)PllCoeff->IntProg;
    }

    if (AmbaWrap_memcpy(&pCurPllCtrlRegsVal->Ctrl, &RegVal, 4U) == 0U ) {
        AmbaCSL_PllSetCtrlRegVal(PllIdx, pCurPllCtrlRegsVal->Ctrl);
    }
}

/**
 *  PLL_GetNextPllHsDivVal - Get pll high speed divider value
 *  @param[in] DivVal The current divider value
 *  @return the next divider value
 */
static UINT32 PLL_GetNextPllHsDivVal(UINT32 DivVal)
{
    UINT32 NextDivVal;

    switch (DivVal) {
    case 1U:
        NextDivVal = 2U;
        break;
    case 2U:
        NextDivVal = 3U;
        break;
    case 3U:
        NextDivVal = 5U;
        break;
    case 5U:
        NextDivVal = 7U;
        break;
    default:
        NextDivVal = 0U;
        break;
    }

    return NextDivVal;
}

/**
 *  PLL_CalcPllOutDivVal - Get vco to pll_out divider values
 *  @param[in] Ratio The ratio between vco and pll_out frequency
 *  @param[out] pDivSout Normal speed divider value
 *  @param[in,out] pDivFsout High speed divider value
 */
static void PLL_CalcPllOutDivVal(UINT32 Ratio, UINT32 *pDivSout, UINT32 *pDivFsout)
{
    UINT32 DivSout, DivFsout;
    UINT32 NewDivFsout;

    if (Ratio > 0UL) {
        if ((pDivFsout != NULL) && (*pDivFsout != 0UL)) {
            NewDivFsout = *pDivFsout;
        } else {
            NewDivFsout = 1U;
        }

        do {
            DivFsout = NewDivFsout;
            DivSout = Ratio / DivFsout;
            NewDivFsout = PLL_GetNextPllHsDivVal(DivFsout);
        } while ((DivSout > 16U) && (NewDivFsout != 0U));

        if (pDivSout != NULL) {
            if (DivSout > 16U) {
                *pDivSout = 16U;
            } else {
                *pDivSout = DivSout;
            }
        }

        if (pDivFsout != NULL) {
            *pDivFsout = DivFsout;
        }
    }
}

/**
 *  PLL_GetPllHsDivVal - Get divider value of pll control 2 register
 *  @param[in] DivRegVal The divider field of pll control 2 register
 *  @return the actual divider value
 */
static UINT32 PLL_GetPllHsDivVal(UINT32 DivRegVal)
{
    UINT32 DivVal;

    switch (DivRegVal) {
    case 0U:
    case 1U:
    case 2U:
    case 3U:
        DivVal = 1U;
        break;
    case 4U:
        DivVal = 2U;
        break;
    case 5U:
        DivVal = 3U;
        break;
    case 6U:
        DivVal = 5U;
        break;
    case 7U:
        DivVal = 7U;
        break;
    default:
        DivVal = 2U;
        break;
    }

    return DivVal;
}

/**
 *  PLL_GetPllHsDivRegVal - Get pll control 2 register value
 *  @param[in] DivVal The divider value
 *  @return the register value
 */
static UINT32 PLL_GetPllHsDivRegVal(UINT32 DivVal)
{
    UINT32 DivRegVal;

    switch (DivVal) {
    case 1U:
        DivRegVal = 0U;
        break;
    case 2U:
        DivRegVal = 4U;
        break;
    case 3U:
        DivRegVal = 5U;
        break;
    case 5U:
        DivRegVal = 6U;
        break;
    case 7U:
        DivRegVal = 7U;
        break;
    default:
        /* No such divider value */
        DivRegVal = 0U;
        break;
    }

    return DivRegVal;
}

/**
 *  PLL_CalcPllCoeff - Auto-calculate coefficients for the specified pll output frequency value
 *  @param[in] PllIdx PLL ID
 *  @param[in] RefFreq Reference clock frequency of the pll
 *  @param[in] PllOutFreq Target pll output frequency
 *  @param[out] pPllCoeff pointer to the pll coefficients
 */
static void PLL_CalcPllCoeff(UINT32 PllIdx, UINT32 RefFreq, UINT32 PllOutFreq, AMBA_PLL_COEFF_s *pPllCoeff)
{
    UINT32 DivSout = 1U, DivFsout = 1U;
    UINT32 DivSdiv = 2U, VcoRatio;
    UINT32 DivIntProg;

    (void) PllIdx;

    if ((pPllCoeff != NULL) && (RefFreq != 0U)) {
        pPllCoeff->VcoDiv = 0U;
        pPllCoeff->Fsdiv = 0U;

        /* Derive Sout */
        if (pPllCoeff->VcoType == 0U) {
            VcoRatio = (UINT32)(MAX_PLL_VCO_FREQ_CORE_L1 / (UINT64)PllOutFreq);
            PLL_CalcPllOutDivVal(VcoRatio, &DivSout, &DivFsout);
        }

        pPllCoeff->Fsout = (UINT8)PLL_GetPllHsDivRegVal(DivFsout);
        pPllCoeff->Sout = (UINT8)(DivSout - 1U);

        /* Derive Sdiv */
        VcoRatio = (UINT32)(((UINT64)PllOutFreq * (UINT64)DivSout * (UINT64)DivFsout) / (UINT64)RefFreq);
        if (VcoRatio > 128UL) {
            while ((VcoRatio / (DivSdiv - 1UL)) > 128UL) {
                DivSdiv += 1U;
                if (DivSdiv > 16U) {
                    DivSdiv = 16U;
                    break;
                }
            }
        }

        pPllCoeff->Sdiv = (UINT8)(DivSdiv - 1U);

        /* Derive IntProg */
        DivIntProg = VcoRatio / (DivSdiv);
        if (DivIntProg == 0U) {
            pPllCoeff->IntProg = 0U;
        } else if (DivIntProg > 128U) {
            pPllCoeff->IntProg = (UINT8)(127U);
        } else {
            pPllCoeff->IntProg = (UINT8)(DivIntProg - 1U);
        }
    }
}

/**
 *  AmbaRTSL_PllSetFreq - Set PLL frequency
 *  @param[in] PllIdx PLL ID
 *  @param[in] Frequency Target PLL clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetFreq(UINT32 PllIdx, UINT32 Frequency)
{
    AMBA_PLL_CTRL_REGS_VAL_s NewPllCtrlRegsVal;
    AMBA_PLL_COEFF_s PllCoeff;
    UINT32 RefFreq, Prescaler, Postscaler, DivIntProg;
    UINT32 DivVco, DivSdiv, DivFsdiv, DivSout, DivFsout;
    DOUBLE WorkDouble, fIntFreq;
    UINT64 WorkUint64;
    UINT32 WorkUint32;
    UINT32 RetVal = PLL_ERR_NONE;

    if (Frequency == 0U) {
        /* Power Down the PLL (save power) */
        AmbaCSL_PllSetPowerDown(PllIdx);

        AmbaPllVcoFreq[PllIdx] = 0.0F;
        (void)AmbaRTSL_PllSetPllOutFreq(PllIdx, 0.0F);
    } else {
        /* Get current PLL Control register values */
        AmbaCSL_PllGetCtrlRegsVal(PllIdx, &NewPllCtrlRegsVal);

        /* Set default PLL control registers' values */
        NewPllCtrlRegsVal.Ctrl2 = 0x30520000U;
        NewPllCtrlRegsVal.Ctrl3 = 0x00088002U;

        Prescaler = AmbaCSL_PllGetPreScaleRegVal(PllIdx);           /* current Prescaler */
        if (Prescaler == 0xffffffffU) {
            Prescaler = 1U;     /* There is no Prescaler */
        }
        RefFreq = AmbaRTSL_PllGetPllRefFreq(PllIdx) / Prescaler;    /* Reference-Frequency / Prescaler */

        Postscaler = AmbaCSL_PllGetPostScaleRegVal(PllIdx);
        if (Postscaler == 0xffffffffU) {
            Postscaler = 1U;    /* There is no Postscaler */
        } else {
            if (PllIdx != AMBA_PLL_VIDEO2) {
                /* Auto-calculate 4-bit divider value */
                PLL_SetupCorePostDivVal(Frequency, &Postscaler);
                if (Postscaler >= 0x10U) {
                    Postscaler = 0x10U;
                }
            }

            /* postscaler shall not be zero */
            if (Postscaler == 0U) {
                Postscaler = 1U;
            }
        }

        AmbaCSL_PllSetPreScaleRegVal(PllIdx, Prescaler);
        AmbaCSL_PllSetPostScaleRegVal(PllIdx, Postscaler);

        /* Derive IntProg, Sdiv, Sout, VcoDiv, Fsdiv, Fsout */
        PllCoeff.VcoType = 0U;  /* Always use single-ended vco */
        PLL_CalcPllCoeff(PllIdx, RefFreq, Frequency * Postscaler, &PllCoeff);
        /* NOTE: Prepare PllCtrlReg and PllCtrl2Reg values */
        NewPllCtrlRegsVal.Ctrl  &= 0x80f00fdfU;
        NewPllCtrlRegsVal.Ctrl  |= (UINT32)PllCoeff.IntProg << 24UL;
        NewPllCtrlRegsVal.Ctrl  |= (UINT32)PllCoeff.Sout << 16UL;
        NewPllCtrlRegsVal.Ctrl  |= (UINT32)PllCoeff.Sdiv << 12UL;
        NewPllCtrlRegsVal.Ctrl2 &= 0xfffff000U;
        NewPllCtrlRegsVal.Ctrl2 |= (UINT32)PllCoeff.VcoDiv;
        NewPllCtrlRegsVal.Ctrl2 |= (UINT32)PllCoeff.Fsdiv << 4UL;
        NewPllCtrlRegsVal.Ctrl2 |= (UINT32)PllCoeff.Fsout << 8UL;

        DivIntProg = PllCoeff.IntProg + (UINT32)1U;
        DivSdiv  = PllCoeff.Sdiv + (UINT32)1U;
        DivSout  = PllCoeff.Sout + (UINT32)1U;
        DivVco   = PLL_GetPllHsDivVal(PllCoeff.VcoDiv);
        DivFsdiv = PLL_GetPllHsDivVal(PllCoeff.Fsdiv);
        DivFsout = PLL_GetPllHsDivVal(PllCoeff.Fsout);

        /* The range of the integer+fraction is [<int+0> to <int+1>) */
        WorkUint64 = (UINT64)RefFreq * (UINT64)DivVco * (UINT64)DivFsdiv * (UINT64)DivSdiv * ((UINT64)DivIntProg + 1ULL);
        /* NOTE: Prepare PllCtrl3Reg value */
        PLL_SetupPllVcoRangeVal(0U, WorkUint64, &NewPllCtrlRegsVal);

        /* fIntFreq = Integer portion */
        Postscaler *= (DivFsout * DivSout);
        fIntFreq = (DOUBLE)RefFreq * (DOUBLE)DivVco * (DOUBLE)DivFsdiv * (DOUBLE)DivSdiv * (DOUBLE)DivIntProg;
        fIntFreq /= (DOUBLE)Postscaler;
        fIntFreq = fIntFreq + 0.5F;
        WorkUint32 = (UINT32) fIntFreq;
        if (Frequency > WorkUint32) {
            WorkUint32 = Frequency - WorkUint32;
            if (WorkUint32 != 0U) { /* get fraction value */
                WorkDouble = (DOUBLE) WorkUint32 * (DOUBLE)Postscaler;
                WorkDouble /= ((DOUBLE)RefFreq * (DOUBLE)DivSdiv);
                WorkDouble *= (DOUBLE)0x100000000ULL;
                WorkUint32 = (UINT32) WorkDouble;
            }
            NewPllCtrlRegsVal.Fraction = WorkUint32;
        } else {
            NewPllCtrlRegsVal.Fraction = 0U;
        }

        /* Set PLL Control register values */
        AmbaCSL_PllSetCtrlRegsVal(PllIdx, &NewPllCtrlRegsVal);
        (void)AmbaRTSL_PllCalcFreq(PllIdx);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllFineAdjFreq - Fine Adjust PLL frequency
 *  @param[in] PllIdx PLL ID
 *  @param[in] Frequency Target PLL clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllFineAdjFreq(UINT32 PllIdx, UINT32 Frequency)
{
    AMBA_PLL_CTRL_REGS_VAL_s NewPllCtrlRegsVal;
    AMBA_PLL_COEFF_s PllCoeff;
    UINT32 RefFreq, Prescaler, Postscaler, DivIntProg;
    UINT32 DivVco, DivSdiv, DivFsdiv, DivSout, DivFsout;
    DOUBLE WorkDouble, fIntFreq;
    UINT32 WorkUint32;
    UINT32 RetVal = PLL_ERR_NONE;

    if (Frequency == 0U) {
        /* Power Down the PLL (save power) */
        AmbaCSL_PllSetPowerDown(PllIdx);

        AmbaPllVcoFreq[PllIdx] = 0.0F;
        (void)AmbaRTSL_PllSetPllOutFreq(PllIdx, 0.0F);
    } else {
        /* Get current PLL Control register values */
        AmbaCSL_PllGetCtrlRegsVal(PllIdx, &NewPllCtrlRegsVal);

        Prescaler = AmbaCSL_PllGetPreScaleRegVal(PllIdx);           /* current Prescaler */
        if (Prescaler == 0xffffffffU) {
            Prescaler = 1U;     /* There is no Prescaler */
        }
        RefFreq = AmbaRTSL_PllGetPllRefFreq(PllIdx) / Prescaler;    /* Reference-Frequency / Prescaler */

        Postscaler = AmbaCSL_PllGetPostScaleRegVal(PllIdx);
        if (Postscaler == 0xffffffffU) {
            Postscaler = 1U;    /* There is no Postscaler */
        }

        /* Derive IntProg, Sdiv, Sout, VcoDiv, Fsdiv, Fsout */
        PllCoeff.VcoType = 0U;  /* Always use single-ended vco */
        PLL_CalcPllCoeff(PllIdx, RefFreq, Frequency * Postscaler, &PllCoeff);
        /* NOTE: Prepare PllCtrlReg values */
        NewPllCtrlRegsVal.Ctrl  &= 0x80f00fdfU;
        NewPllCtrlRegsVal.Ctrl  |= (UINT32)PllCoeff.IntProg << 24UL;
        NewPllCtrlRegsVal.Ctrl  |= (UINT32)PllCoeff.Sout << 16UL;
        NewPllCtrlRegsVal.Ctrl  |= (UINT32)PllCoeff.Sdiv << 12UL;

        DivIntProg = PllCoeff.IntProg + (UINT32)1U;
        DivSdiv  = PllCoeff.Sdiv + (UINT32)1U;
        DivSout  = PllCoeff.Sout + (UINT32)1U;
        DivVco   = PLL_GetPllHsDivVal(PllCoeff.VcoDiv);
        DivFsdiv = PLL_GetPllHsDivVal(PllCoeff.Fsdiv);
        DivFsout = PLL_GetPllHsDivVal(PllCoeff.Fsout);

        /* fIntFreq = Integer portion */
        Postscaler *= (DivFsout * DivSout);
        fIntFreq = (DOUBLE)RefFreq * (DOUBLE)DivVco * (DOUBLE)DivFsdiv * (DOUBLE)DivSdiv * (DOUBLE)DivIntProg;
        fIntFreq /= (DOUBLE)Postscaler;
        fIntFreq = fIntFreq + 0.5F;
        WorkUint32 = (UINT32) fIntFreq;
        WorkUint32 = Frequency - WorkUint32;
        if (WorkUint32 != 0U) { /* get fraction value */
            WorkDouble = (DOUBLE) WorkUint32 * (DOUBLE)Postscaler;
            WorkDouble /= ((DOUBLE)RefFreq * (DOUBLE)DivSdiv);
            WorkDouble *= (DOUBLE)0x100000000ULL;
            WorkUint32 = (UINT32) WorkDouble;
        }
        NewPllCtrlRegsVal.Fraction = WorkUint32;

        /* Update PLL Control/Fraction register values */
        AmbaCSL_PllSetFracRegVal(PllIdx, NewPllCtrlRegsVal.Fraction);
        AmbaCSL_PllSetCtrlRegVal(PllIdx, NewPllCtrlRegsVal.Ctrl);

        (void)AmbaRTSL_PllCalcFreq(PllIdx);
    }

    return RetVal;
}

static void PLL_SetFixPllCtrl(AMBA_PLL_CTRL_REG_s *pPllCtrl, UINT32 Frequency)
{
    pPllCtrl->Sdiv = 0xfU;
    pPllCtrl->Sout = 0xfU;
    if (Frequency <= 24000000U) {
        pPllCtrl->IntProg = 0x0U;
    } else if (Frequency <= 48000000U) {
        pPllCtrl->IntProg = 0x1U;
    } else {
        pPllCtrl->IntProg = 0x2U;
    }
}

static void PLL_RampPllDecrease(UINT32 PllIdx, UINT32 Frequency, UINT32 *pCurFreq, AMBA_PLL_CTRL_REGS_VAL_s *pPllReg, AMBA_PLL_COEFF_s *pPllCoeff)
{
    UINT32 CurFreqTmp = *pCurFreq;
    UINT32 Postscaler, IntProg, RefFreq = 0U;

    /* get PostScaler value */
    Postscaler = AmbaCSL_PllGetPostScaleRegVal(PllIdx);
    if (Postscaler == 0xffffffffU) {
        Postscaler = 1U;    /* There is no Postscaler */
    }

    IntProg = pPllCoeff->IntProg + (UINT32)1U;
    RefFreq = (UINT32)AmbaRTSL_PllGetPllRefFreq(PllIdx);

    while (((UINT32)CurFreqTmp > Frequency) && (IntProg > 0U)) {
        /* get target Sout/Sdiv/IntPro value */
        if (((Frequency % 24000000U) != 0U) && ((Frequency % 12000000U) == 0U)) {
            PLL_CalculateAdjCoeffHalf(RefFreq, (CurFreqTmp - (RefFreq / 2U)) * Postscaler, pPllCoeff);
        } else {
            PLL_CalculateAdjCoeff(RefFreq, (CurFreqTmp - RefFreq) * Postscaler, pPllCoeff);
        }
        PLL_AssignAdjCoeff(PllIdx, pPllCoeff, pPllReg);
        CurFreqTmp = (UINT32)AmbaRTSL_PllCalcFreq(PllIdx);
        PLL_SetupPllVcoRangeVal(0U, (UINT64)AmbaPllVcoFreq[PllIdx], pPllReg);
        AmbaCSL_PllSetCtrl3Reg(PllIdx, pPllReg->Ctrl3);
        CurFreqTmp = CurFreqTmp / Postscaler;
    }
    *pCurFreq = CurFreqTmp;
}

static void PLL_RampPllIncrease(UINT32 PllIdx, UINT32 Frequency, UINT32 CurFreq, AMBA_PLL_CTRL_REGS_VAL_s *pPllReg, AMBA_PLL_COEFF_s *pPllCoeff)
{
    UINT32 CurFreqTmp = CurFreq;
    UINT32 Postscaler, IntProg, RefFreq = 0U;

    /* get PostScaler value */
    Postscaler = AmbaCSL_PllGetPostScaleRegVal(PllIdx);
    if (Postscaler == 0xffffffffU) {
        Postscaler = 1U;    /* There is no Postscaler */
    }

    IntProg = pPllCoeff->IntProg + (UINT32)1U;
    RefFreq = (UINT32)AmbaRTSL_PllGetPllRefFreq(PllIdx);

    while (((UINT32)CurFreqTmp < Frequency) && (IntProg < 123U)) {
        /* get target Sout/Sdiv/IntPro value */
        if (((Frequency % 24000000U) != 0U) && ((Frequency % 12000000U) == 0U)) {
            PLL_CalculateAdjCoeffHalf(RefFreq, (CurFreqTmp + (RefFreq / 2U)) * Postscaler, pPllCoeff);
        } else {
            PLL_CalculateAdjCoeff(RefFreq, (CurFreqTmp + RefFreq) * Postscaler, pPllCoeff);
        }
        PLL_AssignAdjCoeff(PllIdx, pPllCoeff, pPllReg);
        CurFreqTmp = (UINT32)AmbaRTSL_PllCalcFreq(PllIdx);
        PLL_SetupPllVcoRangeVal(0U, (UINT64)AmbaPllVcoFreq[PllIdx], pPllReg);
        AmbaCSL_PllSetCtrl3Reg(PllIdx, pPllReg->Ctrl3);
        CurFreqTmp = CurFreqTmp / Postscaler;
    }
}

/**
 *  AmbaRTSL_PllAdjFreq - Adjust PLL frequency
 *  @param[in] PllIdx PLL ID
 *  @param[in] Frequency Target PLL clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllAdjFreq(UINT32 PllIdx, UINT32 Frequency)
{
    AMBA_PLL_CTRL_REGS_VAL_s CurPllCtrlRegsVal;
    AMBA_PLL_CTRL_REG_s CurPllCtrlRegVal;
    AMBA_PLL_COEFF_s PllCoeff;
    UINT32 CurFreq, RefFreq = 0U;
    UINT32 Postscaler, FractionalMode = 0U;

    /* get current PLL control registers' values */
    AmbaCSL_PllGetCtrlRegsVal(PllIdx, &CurPllCtrlRegsVal);
    if (AmbaWrap_memcpy(&CurPllCtrlRegVal, &CurPllCtrlRegsVal.Ctrl, 4U) != 0U) {
        /* For misra-c */
    }

    if (Frequency == (UINT32)AmbaRTSL_PllGetPllOutFreq(PllIdx)) {
        /* nothing to do */
    } else {
        if (CurPllCtrlRegVal.FractionEnable == 1U) {
            FractionalMode = 1U;
            AmbaCSL_PllSetFractionMode(PllIdx, 0U);
        }

        if (Frequency < 72000000U) {
            PLL_SetFixPllCtrl(&CurPllCtrlRegVal, Frequency);
            if (AmbaWrap_memcpy(&CurPllCtrlRegsVal.Ctrl, &CurPllCtrlRegVal, 4U) == 0U ) {
                AmbaCSL_PllSetCtrlRegVal(PllIdx, CurPllCtrlRegsVal.Ctrl);
            }
        } else {
            /* get clock stage per intprog */
            RefFreq = (UINT32)AmbaRTSL_PllGetPllRefFreq(PllIdx);

            /* get PostScaler value */
            Postscaler = AmbaCSL_PllGetPostScaleRegVal(PllIdx);
            if (Postscaler == 0xffffffffU) {
                Postscaler = 1U;    /* There is no Postscaler */
            }

            /* set pll_int value */
            CurFreq = (UINT32)AmbaRTSL_PllCalcFreq(PllIdx);
            CurFreq = CurFreq / Postscaler;
            PllCoeff.IntProg = CurPllCtrlRegVal.IntProg;
            PllCoeff.Sdiv = CurPllCtrlRegVal.Sdiv;
            PllCoeff.Sout = CurPllCtrlRegVal.Sout;
            if (CurFreq > Frequency) {
                /* decrease the frequency */
                PLL_RampPllDecrease(PllIdx, Frequency, &CurFreq, &CurPllCtrlRegsVal, &PllCoeff);

                if ((UINT32)CurFreq < Frequency) {
                    /* get target Sout/Sdiv/IntPro value */
                    PLL_CalculateAdjCoeff(RefFreq, (CurFreq + RefFreq) * Postscaler, &PllCoeff);
                    PLL_AssignAdjCoeff(PllIdx, &PllCoeff, &CurPllCtrlRegsVal);
                    PLL_SetupPllVcoRangeVal(0U, (UINT64)AmbaPllVcoFreq[PllIdx], &CurPllCtrlRegsVal);
                    AmbaCSL_PllSetCtrl3Reg(PllIdx, CurPllCtrlRegsVal.Ctrl3);
                }
            } else {
                PLL_RampPllIncrease(PllIdx, Frequency, CurFreq, &CurPllCtrlRegsVal, &PllCoeff);
            }

            PLL_SetupPllVcoRangeVal(0U, (UINT64)AmbaPllVcoFreq[PllIdx], &CurPllCtrlRegsVal);
            AmbaCSL_PllSetCtrl3Reg(PllIdx, CurPllCtrlRegsVal.Ctrl3);

            if (FractionalMode == 1U) {
                AmbaCSL_PllSetFractionMode(PllIdx, 1U);
            }
        }
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllCalcFreq - Get actual PLL clock frequency value
 *  @param[in] PllIdx PLL ID
 *  @return PLL clock frequency
 */
FLOAT AmbaRTSL_PllCalcFreq(UINT32 PllIdx)
{
    /* PLL Output Frequency = (RefFreq/preScaler) * ((IntProg + 1) + Fraction/2^32) * (sdiv + 1)/(sout+1) */
    UINT32 IntProg, Sdiv, Sout, Prescaler, DivFsout;
    DOUBLE WorkDouble, Fraction;
    AMBA_PLL_CTRL_REGS_VAL_s CurPllCtrlRegsVal;
    AMBA_PLL_CTRL_REG_s PllCtrlReg;
    FLOAT RetVal = (FLOAT)0.0F;

    /* get current PLL Control Register Values */
    AmbaCSL_PllGetCtrlRegsVal(PllIdx, &CurPllCtrlRegsVal);
    if (AmbaWrap_memcpy(&PllCtrlReg, &CurPllCtrlRegsVal.Ctrl, 4U) != 0U) {
        /* For miara-c */
    }
    if ((PllCtrlReg.PowerDown != 0x0U) || (PllCtrlReg.ForcePllReset != 0x0U)) {
        RetVal = (FLOAT)0.0F;   /* the PLL is in power save mode, its frequency = 0 */
    } else if (PllCtrlReg.Bypass != 0x0U) {
        RetVal = (FLOAT)AmbaRTSL_PllGetPllRefFreq(PllIdx);
    } else {
        IntProg = PllCtrlReg.IntProg + (UINT32)1U;
        Sdiv = PllCtrlReg.Sdiv + (UINT32)1U;
        Sout = PllCtrlReg.Sout + (UINT32)1U;
        Prescaler = AmbaCSL_PllGetPreScaleRegVal(PllIdx);
        if (Prescaler == 0xffffffffU) {
            Prescaler = 1U;     /* There is no Prescaler */
        }
        DivFsout = PLL_GetPllHsDivVal((CurPllCtrlRegsVal.Ctrl2 >> 8U) & 0xfU);

        WorkDouble = (DOUBLE)AmbaRTSL_PllGetPllRefFreq(PllIdx) * (DOUBLE)Sdiv;   /* = RefFreq * (sdiv + 1) */
        if ((PllCtrlReg.FractionEnable != 0U) && (CurPllCtrlRegsVal.Fraction != 0U)) {
            /* fractional mode */
            /* = RefFreq * (sdiv + 1) * (IntProg + 1 + Fraction) */
            Fraction = (DOUBLE)CurPllCtrlRegsVal.Fraction;
            Fraction /= ((DOUBLE)0x100000000ULL);
            WorkDouble = WorkDouble * ((DOUBLE)Fraction + (DOUBLE)IntProg);
        } else {
            /* integer mode */
            /* = RefFreq * (sdiv + 1) * (IntProg + 1) */
            WorkDouble *= (DOUBLE)IntProg;
        }
        WorkDouble /= (DOUBLE)Prescaler;    /* pllVco =((IntProg+1)+Fraction/2^32) * RefFreq/preScaler)*(sdiv+1) */
        AmbaPllVcoFreq[PllIdx] = (FLOAT) WorkDouble;  /* pllVco Frequency (Hz) */
        WorkDouble /= ((DOUBLE)Sout * (DOUBLE)DivFsout);

        RetVal = ((FLOAT) (WorkDouble + 0.5F));
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetAudioClk - Set audio clock 0 frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetAudioClk(UINT32 Frequency)
{
    UINT32 RefFreq;
    UINT32 IntDiv = AmbaCSL_PllGetPreScaleRegVal(AMBA_PLL_AUDIO);
    UINT32 OriginalClk = AmbaRTSL_PllGetAudioClk();

    RefFreq = AmbaRTSL_PllGetPllRefFreq(AMBA_PLL_AUDIO);
    RefFreq = RefFreq / IntDiv;    /* Reference-Frequency / Prescaler */
    /* reference clock could not over 100Mhz */
    if (RefFreq > 100000000U) {
        IntDiv = ((RefFreq / 100000000U) < 16U) ? ((RefFreq / 100000000U) + 1U) : 16U;
        AmbaCSL_PllSetPreScaleRegVal(AMBA_PLL_AUDIO, IntDiv - 1U);
    }

    AmbaCSL_PllSetFractionMode(AMBA_PLL_AUDIO, 1U);
    (void)AmbaRTSL_PllSetFreq(AMBA_PLL_AUDIO, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_AUDIO);   /* Get Audio (gclk_au) clock frequency */

    if (OriginalClk != AmbaRTSL_PllGetAudioClk()) {
        (void)AmbaRTSL_PllNotifyFreqChanges(AMBA_PLL_AUDIO);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetAudio2Clk - Set audio clock 2 frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetAudio2Clk(UINT32 Frequency)
{
    UINT32 Audio2RefFreq = (UINT32) AmbaRTSL_PllGetAudioClk();
    UINT32 Audio2ClkDivider = GetMaxValU32(GetRoundUpValU32(Audio2RefFreq, Frequency), 1U);

    Audio2ClkDivider = GetMinValU32(Audio2ClkDivider, 0x10U);

    if (Audio2ClkDivider != AmbaCSL_PllGetAudio3Divider()) {
        pAmbaRCT_Reg->Audio3Postscaler.WriteEnable = 0U;
        pAmbaRCT_Reg->Audio3Postscaler.Divider = (UINT8)(Audio2ClkDivider - 1U);
        pAmbaRCT_Reg->Audio3Postscaler.WriteEnable = 1U;
        pAmbaRCT_Reg->Audio3Postscaler.WriteEnable = 0U;
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetAdcClk - Set ADC clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetAdcClk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaRTSL_PllGetAdcClk();
    UINT32 ClkDivd;

    ClkSrc = ClkSrc * AmbaCSL_PllGetAdcDivider() * 2U;
    ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency * 2U), 1U);

    if (ClkDivd != AmbaCSL_PllGetAdcDivider()) {
        AmbaCSL_PllSetAdcDivider((UINT16)ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetCanClk - Set CAN clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetCanClk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaRTSL_PllGetCanClk();
    UINT32 ClkDivd;

    ClkSrc = ClkSrc * AmbaCSL_PllGetCanDivider();
    ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency), 1U);

    if (ClkDivd != AmbaCSL_PllGetCanDivider()) {
        if (ClkDivd > 0xffU) {
            ClkDivd = 0xffU;
        }
        AmbaCSL_PllSetCanDivider(ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetCoreClk - Set Core clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetCoreClk(UINT32 Frequency)
{
    UINT32 OriginalClk = AmbaRTSL_PllGetCoreClk();

    /* NOTE: APB clock shall be faster than GCLK_UART otherwise UART module will be dead. */

    /* Some peripheral IPs are not designed for high speed. Their clock source (gclk_core/gclk_ahb) should be below a specific frequency. */
    if (Frequency >= 466000000U) {
        pAmbaRCT_Reg->SysConfig.PeripheralClkMode = 0U;     /* ENET/NAND/SD/USB use gclk_ahb as clock source */
    }

    /* Do not use AmbaRTSL_PllSetFreq to prevent clock glitch problem */
    (void)AmbaRTSL_PllAdjFreq(AMBA_PLL_CORE, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_CORE);        /* Get Core (pll_out_core) clock frequency */

    /* Maximum the peripheral performance. */
    if (Frequency < 466000000U) {
        pAmbaRCT_Reg->SysConfig.PeripheralClkMode = 1U;     /* ENET/NAND/SD/USB use gclk_core as clock source */
    }

    if (OriginalClk != AmbaRTSL_PllGetCoreClk()) {
        (void)AmbaRTSL_PllNotifyFreqChanges(AMBA_PLL_CORE);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetDebounceClk - Set Debounce clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetDebounceClk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaRTSL_PllGetClkRefFreq();
    UINT32 ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency), 1U);  /* the output frequency won't be higher than the required one */

    if (ClkDivd != AmbaCSL_PllGetDebounceDivider()) {
        AmbaCSL_PllSetDebounceDivider(ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetEthernetClk - Set ethernet clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetEthernetClk(UINT32 Frequency)
{
    UINT32 SoPipClkDivider;

    AmbaCSL_PllSetFractionMode(AMBA_PLL_ENET, 1U);
    (void)AmbaRTSL_PllSetFreq(AMBA_PLL_ENET, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_ENET);    /* Get Ethernet (gclk_enet_clk_rmii) clock frequency */

    if (AmbaCSL_PllGetVin1ClkSrcSel() == 0U) {
        /* set Sensor PIP (gclk_so_pip) postscaler */
        SoPipClkDivider = AmbaCSL_PllGetEnetPostscaler();
        if (SoPipClkDivider != AmbaCSL_PllGetVin1Divider()) {
            pAmbaRCT_Reg->Vin1ClkDivider.WriteEnable = 0U;
            pAmbaRCT_Reg->Vin1ClkDivider.Divider = (UINT8)(SoPipClkDivider - 1U);
            pAmbaRCT_Reg->Vin1ClkDivider.WriteEnable = 1U;
            pAmbaRCT_Reg->Vin1ClkDivider.WriteEnable = 0U;
        }
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetIdspClk - Set IDSP clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetIdspClk(UINT32 Frequency)
{
    UINT32 OriginalClk = AmbaRTSL_PllGetIdspClk();

    /* Do not use AmbaRTSL_PllSetFreq to prevent clock glitch problem */
    (void)AmbaRTSL_PllAdjFreq(AMBA_PLL_IDSP, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_IDSP); /* Get IDSP (pll_out_idsp) clock frequency */

    if (OriginalClk != AmbaRTSL_PllGetIdspClk()) {
        (void)AmbaRTSL_PllNotifyFreqChanges(AMBA_PLL_IDSP);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetIrClk - Set IR clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetIrClk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaRTSL_PllGetClkRefFreq();
    UINT32 ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency), 1U);  /* the output frequency won't be higher than the required one */

    if (ClkDivd != AmbaCSL_PllGetIrDivider()) {
        AmbaCSL_PllSetIrDivider(ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetPwmClk - Set PWM clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetPwmClk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaRTSL_PllGetPwmClk();
    UINT32 ClkDivd;

    ClkSrc = ClkSrc * AmbaCSL_PllGetPwmDivider();
    ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency), 1U);
    if (ClkDivd != AmbaCSL_PllGetPwmDivider()) {
        AmbaCSL_PllSetPwmDivider(ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetSdPllOutClk - Set SD pll output frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSdPllOutClk(UINT32 Frequency)
{
    AmbaCSL_PllSetFractionMode(AMBA_PLL_SD, 0U);
    (void)AmbaRTSL_PllSetFreq(AMBA_PLL_SD, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_SD);  /* Get pll_out_sd clock frequency */

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetSd0Clk - Set SD48 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSd0Clk(UINT32 Frequency)
{
    UINT32 RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
    UINT32 ClkDivider = GetMaxValU32(RefFreq / Frequency, 1U);

    if (0x0U != (RefFreq % Frequency)) {
        ClkDivider += 1U;
    }

    if (ClkDivider != AmbaCSL_PllGetSd0Postscaler()) {
        AmbaCSL_PllSetSd0Postscaler((UINT16)ClkDivider);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetSd1Clk - Set SDIO0 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSd1Clk(UINT32 Frequency)
{
    UINT32 RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
    UINT32 ClkDivider = GetMaxValU32(RefFreq / Frequency, 1U);

    if (ClkDivider != AmbaCSL_PllGetSd1Postscaler()) {
        AmbaCSL_PllSetSd1Postscaler((UINT16)ClkDivider);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetSd2Clk - Set SDIO1 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSd2Clk(UINT32 Frequency)
{
    UINT32 RefFreq = (UINT32)AmbaRTSL_PllGetPllOutFreq(AMBA_PLL_SD);
    UINT32 ClkDivider = GetMaxValU32(RefFreq / Frequency, 1U);

    if (ClkDivider != AmbaCSL_PllGetSd2Postscaler()) {
        AmbaCSL_PllSetSd2Postscaler((UINT16)ClkDivider);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetSensor0Clk - Set Sensor 0 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSensor0Clk(UINT32 Frequency)
{
    UINT32 SoVinClkDivider;

    AmbaCSL_PllSetFractionMode(AMBA_PLL_SENSOR0, 1U);
    (void)AmbaRTSL_PllSetFreq(AMBA_PLL_SENSOR0, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_SENSOR0); /* Get Sensor (gclk_so) clock frequency */

    /* set Sensor VIN (gclk_so_vin) postscaler */
    SoVinClkDivider = AmbaCSL_PllGetSensorPostscaler();
    if (SoVinClkDivider != AmbaCSL_PllGetVin0Divider()) {
        pAmbaRCT_Reg->Vin0ClkDivider.WriteEnable = 0U;
        pAmbaRCT_Reg->Vin0ClkDivider.Divider = (UINT8)(SoVinClkDivider - 1U);
        pAmbaRCT_Reg->Vin0ClkDivider.WriteEnable = 1U;
        pAmbaRCT_Reg->Vin0ClkDivider.WriteEnable = 0U;
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetSpiMasterClk - Set SPI master clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSpiMasterClk(UINT32 Frequency)
{
    UINT32 ClkDivd = 0U;

    if (Frequency == 0U) {
        AmbaCSL_PllDisableSpiMasterClk(1U);
    } else {
        AmbaCSL_PllDisableSpiMasterClk(0U);
        ClkDivd = AmbaRTSL_PllGetSpiMasterClk();
        ClkDivd = GetMaxValU32(ClkDivd * AmbaCSL_PllGetSpiMasterClkDivider() / Frequency, 1U);

        if (ClkDivd != AmbaCSL_PllGetSpiMasterClkDivider()) {
            AmbaCSL_PllSetSpiMasterClkDivider(ClkDivd);
        }
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetSpiSlaveClk - Set SPI slave clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSpiSlaveClk(UINT32 Frequency)
{
    UINT32 ClkDivd = 0U, OriFreq, RetStatus;

    if (Frequency == 0U) {
        AmbaCSL_PllDisableSpiSlaveClk(1U);
        RetStatus = PLL_ERR_NONE;
    } else if (Frequency <= 312500000U) {
        /* The SSI clock from RCT to SSI slave must be less than 312.5MHz. */
        AmbaCSL_PllDisableSpiSlaveClk(0U);
        OriFreq = AmbaRTSL_PllGetSpiSlaveClk();
        OriFreq = OriFreq * AmbaCSL_PllGetSpiSlaveClkDivider();
        ClkDivd = GetMaxValU32(OriFreq / 312500000U, 1U);

        if ((OriFreq / ClkDivd) > 312500000U) {
            ClkDivd = ClkDivd + 1U;
        }

        if (ClkDivd != AmbaCSL_PllGetSpiSlaveClkDivider()) {
            AmbaCSL_PllSetSpiSlaveClkDivider(ClkDivd);
        }

        RetStatus = PLL_ERR_NONE;
    } else {
        RetStatus = PLL_ERR_ARG;
    }

    return RetStatus;
}

/**
 *  AmbaRTSL_PllSetSpiNorClk - Set SPI-NOR clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSpiNorClk(UINT32 Frequency)
{
    UINT32 ClkDivd = 0U;
    UINT32 Tmp;

    if (Frequency == 0U) {
        AmbaCSL_PllDisableSpiNorClk(1U);
    } else {
        AmbaCSL_PllDisableSpiNorClk(0U);
        ClkDivd = AmbaRTSL_PllGetSpiNorClk();
        ClkDivd = GetMaxValU32(ClkDivd * AmbaCSL_PllGetSpiNorClkDivider() / Frequency, 1U);

        Tmp = AmbaRTSL_PllGetSpiNorClk();
        if (0x0U != ((Tmp * AmbaCSL_PllGetSpiNorClkDivider()) % Frequency)) {
            ClkDivd += 1U;
        }

        if (ClkDivd != AmbaCSL_PllGetSpiNorClkDivider()) {
            AmbaCSL_PllSetSpiNorClkDivider(ClkDivd);
        }
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetUartApbClk - Set UART APB clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUartApbClk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaRTSL_PllGetUartApbClk();
    UINT32 ClkDivd;

    ClkSrc = ClkSrc * AmbaCSL_PllGetUartApbDivider();
    ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency), 1U);
    if (ClkDivd != AmbaCSL_PllGetUartApbDivider()) {
        AmbaCSL_PllSetUartApbDivider(ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetUart0Clk - Set UART AHB 0 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUart0Clk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaCSL_PllGetUart0Divider();
    UINT32 ClkDivd;

    ClkSrc = ClkSrc * AmbaCSL_PllGetUart0Divider();
    ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency), 1U);
    if (ClkDivd != AmbaCSL_PllGetUart0Divider()) {
        AmbaCSL_PllSetUart0Divider(ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetUart1Clk - Set UART AHB 1 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUart1Clk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaRTSL_PllGetUart1Clk();
    UINT32 ClkDivd;

    ClkSrc = ClkSrc * AmbaCSL_PllGetUart1Divider();
    ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency), 1U);
    if (ClkDivd != AmbaCSL_PllGetUart1Divider()) {
        AmbaCSL_PllSetUart1Divider(ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetUart2Clk - Set UART AHB 2 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUart2Clk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaCSL_PllGetUart2Divider();
    UINT32 ClkDivd;

    ClkSrc = ClkSrc * AmbaCSL_PllGetUart2Divider();
    ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency), 1);
    if (ClkDivd != AmbaCSL_PllGetUart2Divider()) {
        AmbaCSL_PllSetUart2Divider(ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetUart3Clk - Set UART AHB 3 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetUart3Clk(UINT32 Frequency)
{
    UINT32 ClkSrc = AmbaRTSL_PllGetUart3Clk();
    UINT32 ClkDivd;

    ClkSrc = ClkSrc * AmbaCSL_PllGetUart3Divider();
    ClkDivd = GetMaxValU32(GetRoundUpValU32(ClkSrc, Frequency), 1);
    if (ClkDivd != AmbaCSL_PllGetUart3Divider()) {
        AmbaCSL_PllSetUart3Divider(ClkDivd);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetVisionClk - Set Vision clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVisionClk(UINT32 Frequency)
{
    if (Frequency != AmbaRTSL_PllGetVisionClk()) {
        AmbaCSL_PllSetFractionMode(AMBA_PLL_VISION, 1U);
        (void)AmbaRTSL_PllSetFreq(AMBA_PLL_VISION, Frequency);
        (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_VISION);  /* Get VISION (gclk_vision) clock frequency */
    }
    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetVoutLcdClk - Set VoutLCD clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVoutLcdClk(UINT32 Frequency)
{
    /* set VoutLCD (glck_vo2) */
    AmbaCSL_PllSetFractionMode(AMBA_PLL_VIDEO2, 1U);
    AmbaCSL_PllSetPreScaleRegVal(AMBA_PLL_VIDEO2, 1U);
    AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_VIDEO2, AmbaPllVoutLcdClkRatio);

    /* Clean the bypass select bit at initial */
    pAmbaRCT_Reg->VoutClkSelect.VoutBypassSelect = 0U;

    if (AmbaPllVoutLcdClkMode == AMBA_PLL_VOUT_LCD_CVBS) {
        AmbaCSL_PllSetVoutDisplay0Select(0);    /* Use gclk_vo2 as Display-A clock source */
    } else if ((AmbaPllVoutLcdClkMode == AMBA_PLL_VOUT_LCD_MIPI_DSI) || (AmbaPllVoutLcdClkMode == AMBA_PLL_VOUT_LCD_MIPI_CSI)) {
        AmbaCSL_PllSetVoutDisplay0Select(0U);    /* Use gclk_vo2 as Display-A clock source */
    } else if (AmbaPllVoutLcdClkMode == AMBA_PLL_VOUT_LCD_FPD_LINK) {
        AmbaCSL_PllSetVoutDisplay0Select(0U);    /* Use gclk_vo2 as Display-A clock source */

        /* each data-lane always send 7-bit per pixel (refer to spec) */
        AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_VIDEO2, 7U);
    } else {
        /* for misraC checking, do nothing */
    }

    (void)AmbaRTSL_PllSetFreq(AMBA_PLL_VIDEO2, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_VIDEO2);  /* Get VoutLCD (gclk_vo2) clock frequency */

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetVoutLcdClkHint - Set VOUT-LCD clock ratio hint
 *  @param[in] Type VOUT-LCD interface selection
 *  @param[in] Ratio VOUT-LCD clock ratio selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVoutLcdClkHint(UINT32 Type, UINT32 Ratio)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if ((Type >= AMBA_NUM_PLL_VOUT_LCD_TYPE) || (Ratio == 0U)) {
        RetVal = PLL_ERR_ARG;
    } else {
        AmbaPllVoutLcdClkMode = Type;
        AmbaPllVoutLcdClkRatio = Ratio;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllFineAdjAudioClk - Fine adjust audio clock 0 frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllFineAdjAudioClk(UINT32 Frequency)
{
    UINT32 OriginalClk = AmbaRTSL_PllGetAudioClk();

    (void)AmbaRTSL_PllFineAdjFreq(AMBA_PLL_AUDIO, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_AUDIO);   /* Get Audio (gclk_au) clock frequency */

    if (OriginalClk != AmbaRTSL_PllGetAudioClk()) {
        (void)AmbaRTSL_PllNotifyFreqChanges(AMBA_PLL_AUDIO);
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllFineAdjSensor0Clk - Fine adjust Sensor 0 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllFineAdjSensor0Clk(UINT32 Frequency)
{
    (void)AmbaRTSL_PllFineAdjFreq(AMBA_PLL_SENSOR0, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_SENSOR0); /* Get Sensor (gclk_so) clock frequency */

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllFineAdjEthernetClk - Fine adjust Etnernet clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllFineAdjEthernetClk(UINT32 Frequency)
{
    (void)AmbaRTSL_PllFineAdjFreq(AMBA_PLL_ENET, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_ENET); /* Get Etnernet/Pip (gclk_enet_clk_rmii / gclk_so_pip) clock frequency */

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllFineAdjVoutLcdClk - Fine adjust Vout LCD clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllFineAdjVoutLcdClk(UINT32 Frequency)
{
    (void)AmbaRTSL_PllFineAdjFreq(AMBA_PLL_VIDEO2, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_VIDEO2);  /* Get VoutLCD (gclk_vo2) clock frequency */

    return PLL_ERR_NONE;
}

