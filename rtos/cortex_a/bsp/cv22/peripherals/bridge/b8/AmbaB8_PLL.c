/**
 *  @file AmbaB8_PLL.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details B8 PLL APIs
 *
 */
#include "AmbaB8.h"

#include "AmbaB8_Communicate.h"
#include "AmbaB8_PLL.h"

#include "AmbaB8CSL_PLL.h"
#include "AmbaB8CSL_Merger.h"

#ifdef BUILT_IN_SERDES
#include "AmbaCSL_VIN.h"
#endif

#define MAX_PLL_VCO_FREQ        1500000000U /* Max PLL VCO frequency */
/* #define MIN_PLL_VCO_FREQ        800000000U  */ /* MIN PLL VCO frequency */

// #define B8NSWPLL_DEBUG //comment this when not build B8N swpll debug msg
#define B8NSWPLL_SAMPLE_NUM     5U
#define B8NSWPLL_TRIGGER_PERIOD 3U          /* unit: # of interrupts */

/* Event details */
#define B8NSWPLL_FLG_ENABLED    0x001U      /* enable B8N software pll */
#define B8NSWPLL_FLG_TRIGGED    0x002U      /* trigger B8N software pll */
#define B8NSWPLL_FLG_VIN0       0x010U      /* VIN0 B8N software pll */
#define B8NSWPLL_FLG_PIP1       0x020U      /* PIP1 B8N software pll */
#define B8NSWPLL_FLG_PIP2       0x040U      /* PIP2 B8N software pll */
#define B8NSWPLL_FLG_PIP3       0x080U      /* PIP3 B8N software pll */
#define B8NSWPLL_FLG_PIP4       0x100U      /* PIP4 B8N software pll */
#define B8NSWPLL_FLG_PIP5       0x200U      /* PIP5 B8N software pll */

typedef struct {
    UINT32  IntProg:    7;
    UINT32  Sdiv:       4;
    UINT32  Sout:       4;
} B8_PLL_COEFF_s;

typedef struct {
    UINT8                   InitDone;       /* Initial flag */
    AMBA_KAL_MUTEX_t        Mutex;          /* Mutex */
    AMBA_KAL_EVENT_FLAG_t   EventFlag;      /* Event Flags */
    UINT32                  DiffThreshold[B8_NUM_SOC_VIN_CHANNEL]; /* DiffThreshold time in us */
} B8_PLL_SWPLL_CTRL_s;

/* B8N software pll control */
static B8_PLL_SWPLL_CTRL_s B8NSwPllCtrl = {0};

static inline void SwapValU32(UINT32 *Val1, UINT32 *Val2)
{
    UINT32 ValTmp;

    ValTmp = *Val1;
    *Val1 = *Val2;
    *Val2 = ValTmp;
}

static void BubbleSortU32(UINT32 *Array, INT32 Size)
{
    INT32 i, j;
    for (i = 0; i < Size; i++) {
        for (j = 1; j < (Size - i); j++) {
            if (Array[j] < Array[j - 1]) {
                SwapValU32(&Array[j], &Array[j - 1]);
            }
        }
    }
}

static UINT32 B8_GetStreamTimeStamp(UINT32 ChipID, UINT32 *pTimeStampInput0, UINT32 *pTimeStampInput1)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 DataBuf32[4];

    if ((pTimeStampInput0 != NULL) && (pTimeStampInput1 != NULL)) {
        /* get B8AN merger timestamp of input 0 & 1 */
        if (AmbaB8_RegRead((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), & pAmbaB8_MergerReg->PllCounter0, 1, B8_DATA_WIDTH_32BIT, 4, DataBuf32) == B8_ERR_NONE) {
            *pTimeStampInput0 = DataBuf32[0];
            *pTimeStampInput1 = DataBuf32[2];
            RetVal = B8_ERR_COMMUNICATE;
        }
    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_GetPllTable
 *
 *  @Description:: Get corresponding PLL table
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          B8_CLK_DOMAIN_CTRL_s* : Table Address
\*-----------------------------------------------------------------------------------------------*/
static B8_CLK_DOMAIN_CTRL_s* B8_GetPllTable(UINT32 ChipID)
{
    static B8_CLK_DOMAIN_CTRL_s AmbaB8_ClkDomainCtrl[B8_NUM_CHANNEL * B8_MAX_NUM_B8_ON_CHAN] = { 0U };

    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = &AmbaB8_ClkDomainCtrl[0];
    UINT32 CtrlIdx = AmbaB8_GetCtrlIndex(ChipID);

    if (CtrlIdx < (B8_NUM_CHANNEL * B8_MAX_NUM_B8_ON_CHAN)) {
        pClkDomainCtrl = &AmbaB8_ClkDomainCtrl[CtrlIdx];

    } else {
        AmbaB8_PrintUInt5("[B8_GetPllTable] B8 ChipID not supported", 0U, 0U, 0U, 0U, 0U);
    }

    return pClkDomainCtrl;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_CalculatePllCoeff
 *
 *  @Description:: Calculate the suitable PLL coefficients
 *
 *  @Input      ::
 *      RefFreq:    Reference Clock Frequency
 *      PllOutFreq: PLL Output Frequency
 *
 *  @Output     ::
 *      pPllCoeff:  Pointer to the PLL Coefficients
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_CalculatePllCoeff(UINT32 RefFreq, UINT32 PllOutFreq, B8_PLL_COEFF_s *pPllCoeff)
{
    UINT32 IntProgTmp;
    DOUBLE FloorVal;

    pPllCoeff->Sdiv = 0U;

    if (PllOutFreq < MAX_PLL_VCO_FREQ) {
        pPllCoeff->Sout = (UINT8)(MAX_PLL_VCO_FREQ / PllOutFreq) - 1U;
    } else {
        pPllCoeff->Sout = 0U;
    }

    IntProgTmp = PllOutFreq;
    IntProgTmp = IntProgTmp * ((UINT32)pPllCoeff->Sout + 1U);
    (void) AmbaB8_Wrap_floor(((DOUBLE)IntProgTmp / (DOUBLE)RefFreq) + 0.5, &FloorVal);
    IntProgTmp = (UINT32)FloorVal;
    pPllCoeff->IntProg = (UINT8)(IntProgTmp - 1U);

    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_InitPllOutFreq
 *
 *  @Description:: Initialize B8N/B8F/B8D PLL output frequency
 *
 *  @Input      ::
 *      ChipID:         B8 chip id
 *      pClkDomainCtrl: The clock domain control of B8N/B8F/B8D chip
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_InitPllOutFreq(B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl)
{
    UINT32 ClkRefFreq       = pClkDomainCtrl->ClkRefFreq;
    UINT32 PllRefFreq       = pClkDomainCtrl->PllRefFreq[B8_PLL_CORE];
    UINT32 *PllOutClkFreq   = pClkDomainCtrl->PllOutClkFreq;
    FLOAT  *fPllOutClkFreq  = pClkDomainCtrl->fPllOutClkFreq;

    /* Reset Initialzied Setting */
    fPllOutClkFreq[B8_PLL_CORE]     = (FLOAT) PllRefFreq * (FLOAT)(0x19U + 1U) / 2.0f;   /* 312 Mhz */
    fPllOutClkFreq[B8_PLL_MPHY_TX0] = (FLOAT) ClkRefFreq * 2.0f * (FLOAT)(0xfU + 1U);       /* 2304 Mhz */
    fPllOutClkFreq[B8_PLL_MPHY_RX]  = (FLOAT) ClkRefFreq * 2.0f * (FLOAT)(0xfU + 1U);       /* 2304 Mhz */
    fPllOutClkFreq[B8_PLL_SENSOR]   = (FLOAT) PllRefFreq * 18.5625f / 6.0f;     /* 74.25 Mhz */
    fPllOutClkFreq[B8_PLL_VIDEO]    = (FLOAT) PllRefFreq * 31.25f;              /* 750 Mhz */

    PllOutClkFreq[B8_PLL_CORE]      = (UINT32) fPllOutClkFreq[B8_PLL_CORE];
    PllOutClkFreq[B8_PLL_MPHY_TX0]  = (UINT32) fPllOutClkFreq[B8_PLL_MPHY_TX0];
    PllOutClkFreq[B8_PLL_MPHY_RX]   = (UINT32) fPllOutClkFreq[B8_PLL_MPHY_RX];
    PllOutClkFreq[B8_PLL_SENSOR]    = (UINT32) fPllOutClkFreq[B8_PLL_SENSOR];
    PllOutClkFreq[B8_PLL_VIDEO]     = (UINT32) fPllOutClkFreq[B8_PLL_VIDEO];

    pClkDomainCtrl->CorePllClkCtrl.CoreFreq     = pClkDomainCtrl->PllOutClkFreq[B8_PLL_CORE];       /* gclk_core */
    pClkDomainCtrl->CorePllClkCtrl.IdspFreq     = pClkDomainCtrl->PllOutClkFreq[B8_PLL_CORE];       /* gclk_idsp */
    /* TBC: scaler_core_swphy_reg(0x14)[7:0] or [9:0] = 0x11A */
    pClkDomainCtrl->CorePllClkCtrl.SwphyFreq    = pClkDomainCtrl->PllOutClkFreq[B8_PLL_CORE] / 20U; /* gclk_swphy */
    /* pClkDomainCtrl->PllOutClkFreq[B8_PLL_CORE] / 26U; *//* gclk_swphy */
    pClkDomainCtrl->CorePllClkCtrl.SpiFreq      = pClkDomainCtrl->CorePllClkCtrl.CoreFreq / 4U;     /* gclk_ssi */
    /* TBC: gclk_so_vin?? master_sync */
    pClkDomainCtrl->SensorPllClkCtrl.SensorFreq = pClkDomainCtrl->PllOutClkFreq[B8_PLL_SENSOR];     /* glck_si */
    pClkDomainCtrl->SensorPllClkCtrl.Video1Freq = pClkDomainCtrl->PllOutClkFreq[B8_PLL_SENSOR];      /* gclk_vo_dsi1 */
    pClkDomainCtrl->VideoPllClkCtrl.Video0Freq  = pClkDomainCtrl->PllOutClkFreq[B8_PLL_VIDEO];      /* gclk_vo_dsi0 */
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_PllSetFreq
 *
 *  @Description:: Set PLL frequency
 *
 *  @Input      ::
 *      ChipID:         B8 chip id
 *      PllIdx:         Index of the PLL
 *      Frequency:      PLL Clock frequency
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_PllSetFreq(UINT32 ChipID, UINT32 PllIdx, UINT32 Frequency)
{
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = B8_GetPllTable(ChipID);
    B8_PLL_CTRL_REGS_VAL_s CurPllCtrlRegsVal, NewPllCtrlRegsVal;
    UINT32 RefFreq, IntProg, Sdiv, Sout, PostScaler;
    DOUBLE WorkDouble, fIntFreq;
    B8_PLL_COEFF_s PllCoeff;
    INT32 WorkINT32;
    UINT32 RegVal = 0;
    UINT32 RetVal = B8_ERR_NONE;

    if (Frequency == 0U) {
        /* Power Down the PLL (save power) */
        (void) AmbaCSL_B8PllPowerDown(ChipID, PllIdx);

        pClkDomainCtrl->fPllOutClkFreq[PllIdx]   = 0.0f;  /* Frequency (Hz) */
        pClkDomainCtrl->PllOutClkFreq[PllIdx]    = 0U;    /* Frequency (Hz) */

        RetVal = B8_ERR_NONE;
    } else {
        /* Get Reference Frequency (Todo: Prescaler) */
        RefFreq = pClkDomainCtrl->PllRefFreq[PllIdx];

        /* Get current PLL control values */
        (void) AmbaCSL_B8PllGetCtrlRegsVal(ChipID, PllIdx, &CurPllCtrlRegsVal);

        /* Get current PostScaler values */
        PostScaler = AmbaCSL_B8PllGetPostScaleRegVal(ChipID, PllIdx);
        if (PostScaler == 0xffffffffU) {
            PostScaler = 1U; /* There is no Postscaler */
        } else {
            if (PllIdx == B8_PLL_SENSOR) {
                /* Make sure vco frequency is higher than gclk_so_vin requirement */
                PostScaler = (125000000U + (Frequency - 1U)) / Frequency;
            } else {
                /* Todo: Make sure vco frequency is higher than MIN_PLL_VCO_FREQ */
                PostScaler = 1U;
            }
        }

        /* Program post-scaler (if required) */
        (void) AmbaCSL_B8PllSetPostScaleRegVal(ChipID, PllIdx, PostScaler);

        /* Calculate New Ctrl Regs */
        (void) AmbaB8_Wrap_memcpy(&NewPllCtrlRegsVal, &CurPllCtrlRegsVal, sizeof(B8_PLL_CTRL_REGS_VAL_s));

        (void) B8_CalculatePllCoeff(RefFreq, Frequency * PostScaler, &PllCoeff);

        IntProg = (UINT32)PllCoeff.IntProg + 1U;
        Sdiv = (UINT32)PllCoeff.Sdiv + 1U;
        Sout = (UINT32)PllCoeff.Sout + 1U;

        /* Use Fraction mode */
        NewPllCtrlRegsVal.Ctrl.IntProg = (UINT8)IntProg - 1U;
        NewPllCtrlRegsVal.Ctrl.Sdiv = (UINT8)Sdiv - 1U;
        NewPllCtrlRegsVal.Ctrl.Sout = (UINT8)Sout - 1U;
        NewPllCtrlRegsVal.Ctrl.PowerDown = 0U;
        /* fIntFreq = Integer portion */
        PostScaler *= Sout;
        fIntFreq = (DOUBLE) RefFreq * (DOUBLE) IntProg * (DOUBLE) Sdiv;
        fIntFreq /= (DOUBLE) PostScaler;
        fIntFreq = fIntFreq + 0.5f;
        WorkINT32 = (INT32) fIntFreq;
        WorkINT32 = (INT32) Frequency - WorkINT32;
        if (WorkINT32 != 0) {
            WorkDouble = (DOUBLE) WorkINT32 * (DOUBLE) PostScaler;
            WorkDouble /= (DOUBLE) RefFreq * (DOUBLE) Sdiv;
            WorkDouble *= (DOUBLE) 0x100000000ULL;   /* Fraction */
            WorkINT32 = (INT32) WorkDouble;
        }
        NewPllCtrlRegsVal.Fraction = (UINT32) WorkINT32;

        /* Reduce the current source to decrease jitter for Vout */
        if (PllIdx == B8_PLL_VIDEO) {
            RegVal = 0x00710000U;
            (void) AmbaB8_Wrap_memcpy(&NewPllCtrlRegsVal.Ctrl2, &RegVal, sizeof(UINT32));
        } else {
            RegVal = 0x00770000U;
            (void) AmbaB8_Wrap_memcpy(&NewPllCtrlRegsVal.Ctrl2, &RegVal, sizeof(UINT32));
        }

        RegVal = 0x00068102U;
        (void) AmbaB8_Wrap_memcpy(&NewPllCtrlRegsVal.Ctrl3, &RegVal, sizeof(UINT32));

        /* Set PLL Control register values */
        RetVal = AmbaCSL_B8PllSetCtrlRegsVal(ChipID, PllIdx, &CurPllCtrlRegsVal, &NewPllCtrlRegsVal);

        pClkDomainCtrl->PllOutClkFreq[PllIdx]  = Frequency * PostScaler;    /* Frequency (Hz) */
        pClkDomainCtrl->fPllOutClkFreq[PllIdx] = (FLOAT)pClkDomainCtrl->PllOutClkFreq[PllIdx];
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllInit
 *
 *  @Description:: B8 PLL initializations
 *
 *  @Input      ::
 *      ChipID:     B8 chip id (user can only control with one chip)
 *      ClkRefFreq: The Frequency of the PLL Reference Clock
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllInit(UINT32 ChipID, UINT32 ClkRefFreq)
{
    UINT32 i, j;
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl;

    if (AmbaB8_GetSubChipCount(ChipID) > 1U) {
        for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
            if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
                pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
                pClkDomainCtrl->ClkRefFreq = ClkRefFreq;
                for (j = 0U; j < AMBA_NUM_B8_PLL; j ++) {
                    pClkDomainCtrl->PllRefFreq[j] = ClkRefFreq / 3U; /* poc_clk_ref_sel = 1 */
                }
                B8_InitPllOutFreq(pClkDomainCtrl);
            }
        }
    } else {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
        pClkDomainCtrl->ClkRefFreq = ClkRefFreq;
        for (j = 0U; j < AMBA_NUM_B8_PLL; j ++) {
            pClkDomainCtrl->PllRefFreq[j] = ClkRefFreq / 3U; /* poc_clk_ref_sel = 1 */
        }
        B8_InitPllOutFreq(pClkDomainCtrl);
    }

    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSetCoreClk
 *
 *  @Description:: Set Core clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      Frequency: Clock frequency
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSetCoreClk(UINT32 ChipID, UINT32 Frequency)
{
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl;
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 DataBuf32;
    UINT32 i;

    if (Frequency == 240000000U) {
        DataBuf32 = 0x1d020000U;
        AmbaCSL_B8PllSetPllCtrl(ChipID, B8_PLL_CORE, &DataBuf32);

        if (AmbaB8_GetSubChipCount(ChipID) > 1U) {
            for (i = 0; i < B8_MAX_NUM_SUBCHIP; i++) {
                if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
                    pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
                    pClkDomainCtrl->PllOutClkFreq[B8_PLL_CORE]  = 240000000U;
                    pClkDomainCtrl->fPllOutClkFreq[B8_PLL_CORE] = 240000000.0f;
                    pClkDomainCtrl->CorePllClkCtrl.CoreFreq     = 240000000U;
                }
            }
        } else {
            pClkDomainCtrl = B8_GetPllTable(ChipID);
            pClkDomainCtrl->PllOutClkFreq[B8_PLL_CORE]  = 240000000U;
            pClkDomainCtrl->fPllOutClkFreq[B8_PLL_CORE] = 240000000.0f;
            pClkDomainCtrl->CorePllClkCtrl.CoreFreq     = 240000000U;
        }
    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSetSwphyClk
 *
 *  @Description:: Set Swphy clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      Frequency: Clock frequency
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSetSwphyClk(UINT32 ChipID, UINT32 Frequency)
{
    /* LS bit rate = gclk_swphy / 2 */
    /* LS clk = gclk_swphy = clk_core / scaler_swphy_div */
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl;
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 RefFreq, SwPhyScaler, DivVal = 0U;
    UINT32 i;

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
#ifdef BUILT_IN_SERDES
        RefFreq = 24000000; /* clk_ref = 24Mhz */
        if (Frequency != 0U ) {
            /* [7:2] ¡V ls_txphy_clk_div */
            if (RefFreq >= Frequency) DivVal = (RefFreq / Frequency) & 0x3fU;
            else DivVal = 1U;

            AmbaCSL_VinSetSerdesLsTxPhyCtrl(DivVal << 2);
            pClkDomainCtrl->CorePllClkCtrl.SwphyFreq = RefFreq / DivVal;
        } else {
            RetVal = B8_ERR_ARG;
        }
#endif
    } else {
        if (AmbaB8_GetSubChipCount(ChipID) > 1U) {
            for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
                if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
                    pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));

                    /* out_clk = in_clk / scaler_swphy_div */
                    RefFreq = pClkDomainCtrl->PllOutClkFreq[B8_PLL_CORE];

                    if (Frequency != 0U ) {
                        /* [9:0] ¡V scaler_swphy_div */
                        if (RefFreq >= Frequency) {
                            DivVal = ((RefFreq / Frequency) & (UINT32)0x3ffU);
                        } else {
                            DivVal = 1U;
                        }
                        /* FIXMES: B6A [16]: clk_swphy_sel=1 (B8 don't care)*/
                        SwPhyScaler = (UINT32)0x10000U | DivVal;

                        AmbaCSL_B8PllSetSwphyDiv(((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), SwPhyScaler);
                        pClkDomainCtrl->CorePllClkCtrl.SwphyFreq = RefFreq / DivVal;
                    } else {
                        RetVal = B8_ERR_ARG;
                    }
                }
            }
        } else {
            pClkDomainCtrl = B8_GetPllTable(ChipID);

            /* out_clk = in_clk / scaler_swphy_div */
            RefFreq = pClkDomainCtrl->PllOutClkFreq[B8_PLL_CORE];

            if (Frequency != 0U ) {
                /* [9:0] ¡V scaler_swphy_div */
                if (RefFreq >= Frequency) {
                    DivVal = (RefFreq / Frequency) & (UINT32)0x3ffU;
                } else {
                    DivVal = 1U;
                }
                /* FIXMES: B6A [16]: clk_swphy_sel=1 (B8 don't care)*/
                SwPhyScaler = (UINT32) 0x10000U | DivVal;

                AmbaCSL_B8PllSetSwphyDiv(ChipID, SwPhyScaler);
                pClkDomainCtrl->CorePllClkCtrl.SwphyFreq = RefFreq / DivVal;
            } else {
                RetVal = B8_ERR_ARG;
            }

        }
    }
    AmbaB8_PrintUInt5("R0014 0x%x", DivVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSetMphyTx0Clk
 *
 *  @Description:: Set MphyTx0 clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      SerDesRate: Clock frequency
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSetMphyTx0Clk(UINT32 ChipID, UINT32 SerDesRate)
{
    typedef struct {
        UINT32 Frequency;
        UINT8 Intiger;
        UINT8 VcoRange;
        UINT8 ResCtrl;
    } B8_MPHY_PLL_CLK_PARAM_s;

    UINT32 i, RegVal;
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl;
    B8_PLL_CTRL_REG_s MPhyTx0PllCtrl;
    B8_PLL_MPHY_CTRL2_REG_s MPhyTx0PllCtrl2;
    B8_PLL_MPHY_CTRL3_REG_s MPhyTx0PllCtrl3;

    /* pll_acm_en = 0, single-end vco
     * pll_vco_res_ctrl = 2, vco_range:
     * 11: 5.77G < freq < 7.0G
     * 10: 4.8G < freq < 6.03G
     * 01: 4.4G < freq < 5.5G

     * pll_vco_res_ctrl = 1, vco_range:
     * 11: 3.5G < freq < 4.7G
     * 10: 2.8G < freq < 3.83G
     * 01: 2.53G < freq < 3.5G

     * pll_vco_res_ctrl = 0, vco_range:
     * 11: 2.78G < freq < 3.77G
     * 10: 2.19G < freq < 3.01G
     * 01: 1.97G < freq < 2.73G
     */
    static const B8_MPHY_PLL_CLK_PARAM_s PllParam[B8_NUM_SERDES_RATE] = {
        [B8_SERDES_RATE_2P3G]  = { .Frequency = 2304U, .Intiger = 0x0f, .VcoRange = 2, .ResCtrl = 0,},/* 10: 2.19G < freq < 3.01G, res_ctrl = 0 */
        [B8_SERDES_RATE_2P59G] = { .Frequency = 2592U, .Intiger = 0x11, .VcoRange = 2, .ResCtrl = 0,},/* 10: 2.19G < freq < 3.01G, res_ctrl = 0 */
        [B8_SERDES_RATE_3P02G] = { .Frequency = 3024U, .Intiger = 0x14, .VcoRange = 3, .ResCtrl = 0,},/* 11: 2.78G < freq < 3.77G, res_ctrl = 0 */
        [B8_SERDES_RATE_3P45G] = { .Frequency = 3456U, .Intiger = 0x17, .VcoRange = 3, .ResCtrl = 0,},/* 11: 2.78G < freq < 3.77G, res_ctrl = 0 */
        [B8_SERDES_RATE_3P60G] = { .Frequency = 3600U, .Intiger = 0x18, .VcoRange = 3, .ResCtrl = 1,},/* 11: 3.5G < freq < 4.7G, res_ctrl = 1 */
        [B8_SERDES_RATE_3P74G] = { .Frequency = 3744U, .Intiger = 0x19, .VcoRange = 3, .ResCtrl = 1,},/* 11: 3.5G < freq < 4.7G, res_ctrl = 1 */
        [B8_SERDES_RATE_3P88G] = { .Frequency = 3888U, .Intiger = 0x1a, .VcoRange = 3, .ResCtrl = 1,},/* 11: 3.5G < freq < 4.7G, res_ctrl = 1 */
        [B8_SERDES_RATE_4P03G] = { .Frequency = 4032U, .Intiger = 0x1b, .VcoRange = 3, .ResCtrl = 1,},/* 11: 3.5G < freq < 4.7G, res_ctrl = 1 */
        [B8_SERDES_RATE_4P17G] = { .Frequency = 4176U, .Intiger = 0x1c, .VcoRange = 3, .ResCtrl = 1,},/* 11: 3.5G < freq < 4.7G, res_ctrl = 1 */
        [B8_SERDES_RATE_4P32G] = { .Frequency = 4320U, .Intiger = 0x1d, .VcoRange = 3, .ResCtrl = 1,},/* 11: 3.5G < freq < 4.7G, res_ctrl = 1 */
        [B8_SERDES_RATE_4P46G] = { .Frequency = 4464U, .Intiger = 0x1e, .VcoRange = 3, .ResCtrl = 1,},/* 11: 3.5G < freq < 4.7G, res_ctrl = 1 */
        [B8_SERDES_RATE_5P04G] = { .Frequency = 5040U, .Intiger = 0x22, .VcoRange = 2, .ResCtrl = 2,},/* 10: 4.8G < freq < 6.03G, pll_vco_res_ctrl = 2 */
        [B8_SERDES_RATE_5P47G] = { .Frequency = 5472U, .Intiger = 0x25, .VcoRange = 2, .ResCtrl = 2,},/* 10: 4.8G < freq < 6.03G, pll_vco_res_ctrl = 2 */
        [B8_SERDES_RATE_5P76G] = { .Frequency = 5760U, .Intiger = 0x27, .VcoRange = 2, .ResCtrl = 2,},/* 10: 4.8G < freq < 6.03G, pll_vco_res_ctrl = 2 */
        [B8_SERDES_RATE_6P04G] = { .Frequency = 6048U, .Intiger = 0x29, .VcoRange = 3, .ResCtrl = 2,},/* 11: 5.77G < freq < 7.0G, pll_vco_res_ctrl = 2 */
    };

    RegVal = 0x00400000;
    (void) AmbaB8_Wrap_memcpy(&MPhyTx0PllCtrl, &RegVal, sizeof(UINT32));
    RegVal = 0x30571200;    /* PassPllDivHS[12] == 1, pll_out = vco_freq */
    (void) AmbaB8_Wrap_memcpy(&MPhyTx0PllCtrl2, &RegVal, sizeof(UINT32));
    RegVal = 0x00028000;
    (void) AmbaB8_Wrap_memcpy(&MPhyTx0PllCtrl3, &RegVal, sizeof(UINT32));

    /* vco_clamp = 1 */
    MPhyTx0PllCtrl3.VcoClamp = 1U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->MPhyTx0PllCtrl3, 0, B8_DATA_WIDTH_32BIT, 1, &MPhyTx0PllCtrl3);

    MPhyTx0PllCtrl3.VcoResCtrl = PllParam[SerDesRate].ResCtrl;
    MPhyTx0PllCtrl3.VcoRange = PllParam[SerDesRate].VcoRange;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->MPhyTx0PllCtrl3, 0, B8_DATA_WIDTH_32BIT, 1, &MPhyTx0PllCtrl3);

    /* single-end vco */
    MPhyTx0PllCtrl2.VcoType = 0;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->MPhyTx0PllCtrl2, 0, B8_DATA_WIDTH_32BIT, 1, &MPhyTx0PllCtrl2);

    /* integer program */
    MPhyTx0PllCtrl.IntProg = PllParam[SerDesRate].Intiger;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->MPhyTx0PllCtrl, 0, B8_DATA_WIDTH_32BIT, 1, &MPhyTx0PllCtrl);

    /* div2_8 = 0, div2_9 = 1, div2_11 = 0 */
    MPhyTx0PllCtrl2.Div2_8 = 0U;
    MPhyTx0PllCtrl2.Div2_9 = 1U;
    MPhyTx0PllCtrl2.Div2_11 = 0U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->MPhyTx0PllCtrl2, 0, B8_DATA_WIDTH_32BIT, 1, &MPhyTx0PllCtrl2);

    /* vco_halt = 1, rst_l = 1 */
    MPhyTx0PllCtrl.Sdiv = 0U;
    MPhyTx0PllCtrl.Sout = 0U;
    MPhyTx0PllCtrl.HaltVCO = 1U;
    MPhyTx0PllCtrl.ForcePllReset = 1U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->MPhyTx0PllCtrl, 0, B8_DATA_WIDTH_32BIT, 1, &MPhyTx0PllCtrl);

    /* After > 15 cycles of reference clock */
    (void) AmbaB8_KAL_TaskSleep(2);

    /* vco_halt = 0 */
    MPhyTx0PllCtrl.HaltVCO = 0U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->MPhyTx0PllCtrl, 0, B8_DATA_WIDTH_32BIT, 1, &MPhyTx0PllCtrl);

    /* After 6 cycles of reference clock */
    (void) AmbaB8_KAL_TaskSleep(2);

    /* rst_l = 0 */
    MPhyTx0PllCtrl.ForcePllReset = 0U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->MPhyTx0PllCtrl, 0, B8_DATA_WIDTH_32BIT, 1, &MPhyTx0PllCtrl);

    /* After 6 cycles of reference clock */
    (void) AmbaB8_KAL_TaskSleep(2);

    /* vco_clamp = 0 */
    MPhyTx0PllCtrl3.VcoClamp = 0U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PllReg->MPhyTx0PllCtrl3, 0, B8_DATA_WIDTH_32BIT, 1, &MPhyTx0PllCtrl3);

    if (AmbaB8_GetSubChipCount(ChipID) > 1U) {
        for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
            if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
                pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
                pClkDomainCtrl->PllOutClkFreq[B8_PLL_MPHY_TX0]  = PllParam[SerDesRate].Frequency * 1000000U;
                pClkDomainCtrl->fPllOutClkFreq[B8_PLL_MPHY_TX0] = (FLOAT)PllParam[SerDesRate].Frequency * 1000000.0f;
            }
        }
    } else {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
        pClkDomainCtrl->PllOutClkFreq[B8_PLL_MPHY_TX0]  = PllParam[SerDesRate].Frequency * 1000000U;
        pClkDomainCtrl->fPllOutClkFreq[B8_PLL_MPHY_TX0] = (FLOAT)PllParam[SerDesRate].Frequency * 1000000.0f;
    }
    return B8_ERR_NONE;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSetMphyRxClk
 *
 *  @Description:: Set MphyRx clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      SerDesRate: Clock frequency
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSetMphyRxClk(UINT32 ChipID, UINT32 SerDesRate)
{
    AmbaB8_Misra_TouchUnused(&ChipID);
    AmbaB8_Misra_TouchUnused(&SerDesRate);

    /* unnecessary in DCO mode */
    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSetSensorClk
 *
 *  @Description:: Set Sensor clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      Frequency: Clock frequency
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSetSensorClk(UINT32 ChipID, UINT32 Frequency)
{
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl;
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 i;

    if (AmbaB8_GetSubChipCount(ChipID) > 1U) {
        for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
            if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
                pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));

                (void) AmbaCSL_B8PllFractionalMode(((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), B8_PLL_SENSOR, 1);
                RetVal = B8_PllSetFreq(((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), B8_PLL_SENSOR, Frequency);
                pClkDomainCtrl->SensorPllClkCtrl.SensorFreq = Frequency;
            }
        }
    } else {
        pClkDomainCtrl = B8_GetPllTable(ChipID);

        (void) AmbaCSL_B8PllFractionalMode(ChipID, B8_PLL_SENSOR, 1);
        RetVal = B8_PllSetFreq(ChipID, B8_PLL_SENSOR, Frequency);
        pClkDomainCtrl->SensorPllClkCtrl.SensorFreq = Frequency;

    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSetVout0Clk
 *
 *  @Description:: Set Vout 0 clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      Frequency: Clock frequency
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSetVout0Clk(UINT32 ChipID, UINT32 Frequency)
{
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl;
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 i;

    if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
        if (AmbaB8_GetSubChipCount(ChipID) > 1U) {
            for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
                if ((ChipID & ((UINT32)0x1 << i)) != 0U) {
                    pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
                    (void) AmbaCSL_B8PllFractionalMode(((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), B8_PLL_VIDEO, 1);
                    RetVal = B8_PllSetFreq(((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), B8_PLL_VIDEO, Frequency);
                    pClkDomainCtrl->VideoPllClkCtrl.Video0Freq = Frequency;
                }
            }
        } else {
            pClkDomainCtrl = B8_GetPllTable(ChipID);
            (void) AmbaCSL_B8PllFractionalMode(ChipID, B8_PLL_VIDEO, 1);
            RetVal = B8_PllSetFreq(ChipID, B8_PLL_VIDEO, Frequency);
            pClkDomainCtrl->VideoPllClkCtrl.Video0Freq = Frequency;
        }
    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSetVout1Clk
 *
 *  @Description:: Set Vout 1 clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *      Frequency: Clock frequency
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSetVout1Clk(UINT32 ChipID, UINT32 Frequency)
{
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = B8_GetPllTable(ChipID);
    UINT32 RetVal = B8_ERR_NONE;

    if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
        /* Vout1 clock source is from sensor pll */
        (void) AmbaCSL_B8PllFractionalMode(ChipID, B8_PLL_SENSOR, 1);
        RetVal = B8_PllSetFreq(ChipID, B8_PLL_SENSOR, Frequency);

        pClkDomainCtrl->SensorPllClkCtrl.Video1Freq = Frequency;
    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllRescaleSensorClk
 *
 *  @Description:: Rescale sensor clock frequency
 *
 *  @Input      ::
 *      ChipID:        B8 chip id (user can only control with one chip)
 *      FrequencyDiff: Frequency difference
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllRescaleSensorClk(UINT32 ChipID, INT32 FrequencyDiff)
{
    UINT32 i;
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 PllRefFreq;
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl;
    INT32 TargetFreq;

    if (AmbaB8_GetSubChipCount(ChipID) > 1U) {
        for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
            if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
                pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));

                PllRefFreq = pClkDomainCtrl->PllRefFreq[B8_PLL_SENSOR];
                RetVal = AmbaCSL_B8PllRescale(((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), B8_PLL_SENSOR, PllRefFreq, FrequencyDiff);
                TargetFreq = (INT32)pClkDomainCtrl->SensorPllClkCtrl.SensorFreq + FrequencyDiff;
                AmbaMisra_TypeCast32(&pClkDomainCtrl->SensorPllClkCtrl.SensorFreq, &TargetFreq);
            }
        }
    } else {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
        PllRefFreq = pClkDomainCtrl->PllRefFreq[B8_PLL_SENSOR];
        RetVal = AmbaCSL_B8PllRescale(ChipID, B8_PLL_SENSOR, PllRefFreq, FrequencyDiff);
        TargetFreq = (INT32)pClkDomainCtrl->SensorPllClkCtrl.SensorFreq + FrequencyDiff;
        AmbaMisra_TypeCast32(&pClkDomainCtrl->SensorPllClkCtrl.SensorFreq, &TargetFreq);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllGetCoreClk
 *
 *  @Description:: Get Core Clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Clock frequency
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllGetCoreClk(UINT32 ChipID)
{
    const B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = NULL;
    UINT32 i;

    for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
        if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
            pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
            break;
        }
    }
    if (pClkDomainCtrl == NULL) {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
    }

    return pClkDomainCtrl->CorePllClkCtrl.CoreFreq;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllGetIdspClk
 *
 *  @Description:: Get Idsp Clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Clock frequency
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllGetIdspClk(UINT32 ChipID)
{
    const B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = NULL;
    UINT32 i;

    for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
        if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
            pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
            break;
        }
    }
    if (pClkDomainCtrl == NULL) {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
    }

    return pClkDomainCtrl->CorePllClkCtrl.IdspFreq;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllGetSwphyClk
 *
 *  @Description:: Get Swphy Clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Clock frequency
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllGetSwphyClk(UINT32 ChipID)
{
    const B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = NULL;
    UINT32 i;

    for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
        if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
            pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
            break;
        }
    }
    if (pClkDomainCtrl == NULL) {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
    }

    return pClkDomainCtrl->CorePllClkCtrl.SwphyFreq;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllGetSpiClk
 *
 *  @Description:: Get Spi Clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Clock frequency
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllGetSpiClk(UINT32 ChipID)
{
    const B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = NULL;
    UINT32 i;

    for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
        if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
            pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
            break;
        }
    }
    if (pClkDomainCtrl == NULL) {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
    }

    return pClkDomainCtrl->CorePllClkCtrl.SpiFreq;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllGetSensorClk
 *
 *  @Description:: Get Sensor Clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Clock frequency
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllGetSensorClk(UINT32 ChipID)
{
    const B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = NULL;
    UINT32 i;

    for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
        if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
            pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
            break;
        }
    }
    if (pClkDomainCtrl == NULL) {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
    }

    return pClkDomainCtrl->SensorPllClkCtrl.SensorFreq;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllGetVout0Clk
 *
 *  @Description:: Get Vout 0 Clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Clock frequency
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllGetVout0Clk(UINT32 ChipID)
{
    const B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = NULL;
    UINT32 i;

    for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
        if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
            pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
            break;
        }
    }
    if (pClkDomainCtrl == NULL) {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
    }

    return pClkDomainCtrl->VideoPllClkCtrl.Video0Freq;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllGetVout1Clk
 *
 *  @Description:: Get Vout 1 Clock frequency
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Clock frequency
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllGetVout1Clk(UINT32 ChipID)
{
    const B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl = NULL;
    UINT32 i;

    for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
        if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
            pClkDomainCtrl = B8_GetPllTable((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
            break;
        }
    }
    if (pClkDomainCtrl == NULL) {
        pClkDomainCtrl = B8_GetPllTable(ChipID);
    }

    return pClkDomainCtrl->SensorPllClkCtrl.Video1Freq;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllGetPwmClkRef
 *
 *  @Description:: Get Reference Clock frequency of PWM
 *
 *  @Input      ::
 *      ChipID:    B8 chip id (user can only control with one chip)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Clock frequency
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllGetPwmClkRef(UINT32 ChipID)
{
    UINT32 DataBuf32 = 0U;
    UINT32 PwmSrcClk;
    UINT32 i;

    /* TBD: Use clk_ref_sel (POC bit 14) to determine the clk_ref is 72MHz or 24MHz */
    if (AmbaB8_GetSubChipCount(ChipID) > 1U) {
        for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
            if ((ChipID & ((UINT32)0x1U << i)) != 0U) {
                (void) AmbaB8_RegRead(((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), & pAmbaB8_PllReg->SysConfig, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
                break;
            }
        }
    } else {
        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_PllReg->SysConfig, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
    }

    if ((DataBuf32 & (UINT32)0x00004000U) == 0x00004000U) {
        PwmSrcClk = 72000000U;
    } else {
        PwmSrcClk = 24000000U;
    }

    return PwmSrcClk;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllInit
 *
 *  @Description:: Software PLL initialization
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *        INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSwPllInit(void)
{
    UINT32 RetVal = B8_ERR_NONE;
    static char AmbaB8SwPllMutexName[17] = "AmbaB8SwPllMutex";
    static char AmbaB8SwPllEventFlagName[21] = "AmbaB8SwPllEventFlag";

    if (B8NSwPllCtrl.InitDone == 0U) {
        if (AmbaB8_KAL_MutexCreate(&B8NSwPllCtrl.Mutex, AmbaB8SwPllMutexName) == OK) {
            if (AmbaB8_KAL_EventFlagCreate(&B8NSwPllCtrl.EventFlag, AmbaB8SwPllEventFlagName) == OK) {
                B8NSwPllCtrl.InitDone = 1U;
                RetVal = B8_ERR_NONE;
            } else {
                RetVal = B8_ERR_UNEXPECTED;
            }
        } else {
            RetVal = B8_ERR_MUTEX;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllConfig
 *
 *  @Description:: Software PLL configuration
 *
 *  @Input      ::
 *      DiffThresholdInMs: Time Difference Threashold of different input on B8N in microsecond
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSwPllConfig(UINT32 ChipID, UINT32 DiffThresholdInUs)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 VinID;

    if (B8NSwPllCtrl.InitDone != 0U) {
        if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
            for (VinID = 0U; VinID < B8_NUM_SOC_VIN_CHANNEL; VinID ++) {
                if ((ChipID & ((UINT32)1U << (VinID + 4U))) != 0U) {
                    break;
                }
            }
            if (VinID < B8_NUM_SOC_VIN_CHANNEL) {
                B8NSwPllCtrl.DiffThreshold[VinID] = DiffThresholdInUs;
            } else {
                RetVal = B8_ERR_ARG;
            }
        } else {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllEnable
 *
 *  @Description:: Enable Software PLL
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSwPllEnable(void)
{
    UINT32 RetVal = B8_ERR_NONE;

    if (B8NSwPllCtrl.InitDone != 0U) {
        RetVal = AmbaB8_KAL_EventFlagSet(&B8NSwPllCtrl.EventFlag, B8NSWPLL_FLG_ENABLED);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllHandler
 *
 *  @Description:: Software PLL Handler
 *
 *  @Input      :: none
 *      Timeout: Timeout value for event get in ms (millisecond)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSwPllHandler(UINT32 Timeout)
{
    /* History records. Continously monitor 5 samples to get the trend */
    static UINT32 Counter[B8_NUM_SOC_VIN_CHANNEL] = {0};
    static UINT32 TimeStampDiffRecord_0[B8_NUM_SOC_VIN_CHANNEL][B8NSWPLL_SAMPLE_NUM] = {0};
    static UINT32 TimeStampDiffRecord_1[B8_NUM_SOC_VIN_CHANNEL][B8NSWPLL_SAMPLE_NUM] = {0};
    static INT32 DirectionRecord[B8_NUM_SOC_VIN_CHANNEL][B8NSWPLL_SAMPLE_NUM] = {0};
    /* Save last time's timestamp */
    static UINT32 PreTimeStamp[B8_NUM_SOC_VIN_CHANNEL][B8_NUM_VIN_CHANNEL] = {0};

    UINT32 GapThreshold = 1500;
    UINT32 ChipID, i;
    UINT32 ActualFlags, VinEventFlag;   /* Actual event flags */

    /* Timestamp records of 2 input: input_0(stream0)(master) & input_1(stream2)(slave) */
    UINT32 CurTimeStamp[B8_NUM_SOC_VIN_CHANNEL][B8_NUM_VIN_CHANNEL];

    /* Timestamp differences compared to last time's */
    UINT32 TimeStampDiff_0[B8_NUM_SOC_VIN_CHANNEL], TimeStampDiff_1[B8_NUM_SOC_VIN_CHANNEL];

    /* Direction of input_0 & input_1: (+) means input_1 is faster, (-) means input_1 is slower */
    INT32 Direction[B8_NUM_SOC_VIN_CHANNEL];
    UINT32 RetVal = B8_ERR_NONE;

    if (B8NSwPllCtrl.InitDone == 0U) {
        RetVal = B8_ERR_INVALID_API;
    } else {
        if (AmbaB8_KAL_EventFlagGet(&B8NSwPllCtrl.EventFlag,
                                    B8NSWPLL_FLG_ENABLED | B8NSWPLL_FLG_TRIGGED,
                                    1U, 0U,
                                    &ActualFlags,
                                    Timeout) != OK) {
            RetVal = B8_ERR_MUTEX;
        } else {
            /* For each B8N on different VIN */
            for (i = 0U; i < B8_NUM_SOC_VIN_CHANNEL; i++) {
                UINT32 *pCounter;

                VinEventFlag = (((UINT32)0x1U << i) << 4U);
                if ((ActualFlags & VinEventFlag) == 0U) {
                    continue;
                }

                /* get ChipID of target VIN */
                ChipID = VinEventFlag | (B8_SUB_CHIP_ID_B8F0 | B8_SUB_CHIP_ID_B8F1);
                pCounter = &Counter[i];

                /* Unit: pll_core 240mhz */
                if (B8NSwPllCtrl.DiffThreshold[i] != 0U) {
                    GapThreshold = B8NSwPllCtrl.DiffThreshold[i] * 240U;
                }
                (void) B8_GetStreamTimeStamp((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CurTimeStamp[i][0], &CurTimeStamp[i][1]);

                /* Check connection status of master channel */
                TimeStampDiff_0[i] = (UINT32) AmbaB8_GetAbsValU64((INT64)CurTimeStamp[i][0] - (INT64)PreTimeStamp[i][0]);
                if (TimeStampDiff_0[i] == 0U) {
                    AmbaB8_PrintUInt5("> Master TimeStamp halted! <", 0U, 0U, 0U, 0U, 0U);
                    *pCounter = 0U;
                    (void) AmbaB8_Wrap_memset(PreTimeStamp[i], 0, sizeof(UINT32) * 2U);
                    (void) AmbaKAL_EventFlagClear(&B8NSwPllCtrl.EventFlag, VinEventFlag);
                    continue;
                }

                /* Check connection status of slave channels */
                TimeStampDiff_1[i] = (UINT32) AmbaB8_GetAbsValU64((INT64)CurTimeStamp[i][1] - (INT64)PreTimeStamp[i][1]);
                if (TimeStampDiff_1[i] == 0U) {
                    AmbaB8_PrintUInt5("> Slave TimeStamp halted! <", 0U, 0U, 0U, 0U, 0U);
                    *pCounter = 0U;
                    (void) AmbaB8_Wrap_memset(PreTimeStamp[i], 0, sizeof(UINT32) * 2U);
                    (void) AmbaKAL_EventFlagClear(&B8NSwPllCtrl.EventFlag, VinEventFlag);
                    continue;
                }

                /* Gap of master and slave has exceeded pre-defined threshold */
                Direction[i] = (INT32)((INT64)CurTimeStamp[i][1] - (INT64)CurTimeStamp[i][0]);
                if (AmbaB8_GetAbsValU32(Direction[i]) > GapThreshold) {

                    DirectionRecord[i][*pCounter] = Direction[i];
                    TimeStampDiffRecord_0[i][*pCounter] = TimeStampDiff_0[i];
                    TimeStampDiffRecord_1[i][*pCounter] = TimeStampDiff_1[i];

                    if (*pCounter == (B8NSWPLL_SAMPLE_NUM - 1U)) {
                        INT32 DirectionTrend;
                        if (DirectionRecord[i][B8NSWPLL_SAMPLE_NUM - 1U] > DirectionRecord[i][0]) {
                            /* slave is gradually faster than master */
                            DirectionTrend  = 1;
                        } else if (DirectionRecord[i][B8NSWPLL_SAMPLE_NUM - 1U] < DirectionRecord[i][0]) {
                            /* slave is gradually slower than master */
                            DirectionTrend  = -1;
                        } else {
                            DirectionTrend = 0;
                        }

                        /* Do not adjust pll if opposite direction of trend and fact */
                        if (!(((DirectionTrend < 0) && (Direction[i] > 0)) ||
                              ((DirectionTrend > 0) && (Direction[i] < 0)) ||
                              ((DirectionTrend == 0) && (Direction[i] == 0)))) {
                            DOUBLE DiffSumFrac;
                            DOUBLE DiffTmp;
                            INT32 FrequencyDiff;

                            /* Calulate new fraction */
                            BubbleSortU32(TimeStampDiffRecord_0[i], (INT32)B8NSWPLL_SAMPLE_NUM);
                            BubbleSortU32(TimeStampDiffRecord_1[i], (INT32)B8NSWPLL_SAMPLE_NUM);
                            DiffSumFrac = ((DOUBLE) TimeStampDiffRecord_0[i][B8NSWPLL_SAMPLE_NUM >> 1U] / (DOUBLE) TimeStampDiffRecord_1[i][B8NSWPLL_SAMPLE_NUM >> 1U]);

                            /* Get master sensor clock */
                            DiffTmp = (DOUBLE)1.0 - DiffSumFrac;
                            DiffTmp *= (DOUBLE) AmbaB8_PllGetSensorClk(ChipID & (B8_SUB_CHIP_ID_B8F0 | B8_MAIN_CHIP_ID_B8NF_MASK));
                            FrequencyDiff = (INT32) DiffTmp;

                            /* Print msg */
#ifdef B8NSWPLL_DEBUG
                            AmbaB8_PrintUInt5("[B8NSWPLL] Vin#%d TimeStamp Diff_0 = %d, Diff_1 = %d", \
                                              i, \
                                              TimeStampDiffRecord_0[i][B8NSWPLL_SAMPLE_NUM >> 1], \
                                              TimeStampDiffRecord_1[i][B8NSWPLL_SAMPLE_NUM >> 1], \
                                              0U, 0U);
                            AmbaB8_PrintInt5("[B8NSWPLL] Frequency Difference = %d", \
                                             FrequencyDiff, 0U, 0U, 0U, 0U);
#endif
                            (void) AmbaB8_KAL_MutexTake(&B8NSwPllCtrl.Mutex, AMBAB8_KAL_WAIT_FOREVER);
                            (void) AmbaB8_PllRescaleSensorClk((ChipID & (B8_SUB_CHIP_ID_B8F1 | B8_MAIN_CHIP_ID_B8NF_MASK)), FrequencyDiff);
                            (void) AmbaB8_KAL_MutexGive(&B8NSwPllCtrl.Mutex);
                        }

                        /* Reset counter */
                        *pCounter = 0U;

                    } else {
                        /* Adjust pll  */
                        (*pCounter)++;
                    }

                } else if (*pCounter > 0U) {
                    (*pCounter)--;
                } else {
                    AmbaB8_Misra_TouchUnused(pCounter);
                }

                /* Update PreTimeStamp */
                (void) AmbaB8_Wrap_memcpy(PreTimeStamp[i], CurTimeStamp[i], sizeof(UINT32) * 2U);

                /* Clear vin event flag */
                (void) AmbaKAL_EventFlagClear(&B8NSwPllCtrl.EventFlag, VinEventFlag);
            }
            (void) AmbaKAL_EventFlagClear(&B8NSwPllCtrl.EventFlag, B8NSWPLL_FLG_TRIGGED);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllVin0CallBackFunc
 *
 *  @Description:: User shall hook this callback function as VIN ISR of Ambarella SoC
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_PllSwPllVin0CallBackFunc(void)
{
    static UINT32 Counter = 0U;

    if (B8NSwPllCtrl.InitDone != 0U) {
        Counter++;
        if (Counter == B8NSWPLL_TRIGGER_PERIOD) {
            (void) AmbaB8_KAL_EventFlagSet(&B8NSwPllCtrl.EventFlag, (B8NSWPLL_FLG_TRIGGED | B8NSWPLL_FLG_VIN0));
            Counter = 0U;
        }
    }
    return ;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllVin1CallBackFunc
 *
 *  @Description:: User shall hook this callback function as VIN ISR of Ambarella SoC
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_PllSwPllVin1CallBackFunc(void)
{
    static UINT32 Counter = 0U;

    if (B8NSwPllCtrl.InitDone != 0U) {
        Counter++;
        if (Counter == B8NSWPLL_TRIGGER_PERIOD) {
            (void) AmbaB8_KAL_EventFlagSet(&B8NSwPllCtrl.EventFlag, (B8NSWPLL_FLG_TRIGGED | B8NSWPLL_FLG_PIP1));
            Counter = 0U;
        }
    }
    return ;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllVin2CallBackFunc
 *
 *  @Description:: User shall hook this callback function as VIN ISR of Ambarella SoC
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_PllSwPllVin2CallBackFunc(void)
{
    static UINT32 Counter = 0U;

    if (B8NSwPllCtrl.InitDone != 0U) {
        Counter++;
        if (Counter == B8NSWPLL_TRIGGER_PERIOD) {
            (void) AmbaB8_KAL_EventFlagSet(&B8NSwPllCtrl.EventFlag, (B8NSWPLL_FLG_TRIGGED | B8NSWPLL_FLG_PIP2));
            Counter = 0U;
        }
    }
    return ;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllVin3CallBackFunc
 *
 *  @Description:: User shall hook this callback function as VIN ISR of Ambarella SoC
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_PllSwPllVin3CallBackFunc(void)
{
    static UINT32 Counter = 0U;

    if (B8NSwPllCtrl.InitDone != 0U) {
        Counter++;
        if (Counter == B8NSWPLL_TRIGGER_PERIOD) {
            (void) AmbaB8_KAL_EventFlagSet(&B8NSwPllCtrl.EventFlag, (B8NSWPLL_FLG_TRIGGED | B8NSWPLL_FLG_PIP3));
            Counter = 0U;
        }
    }
    return ;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllVin4CallBackFunc
 *
 *  @Description:: User shall hook this callback function as VIN ISR of Ambarella SoC
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_PllSwPllVin4CallBackFunc(void)
{
    static UINT32 Counter = 0U;

    if (B8NSwPllCtrl.InitDone != 0U) {
        Counter++;
        if (Counter == B8NSWPLL_TRIGGER_PERIOD) {
            (void) AmbaB8_KAL_EventFlagSet(&B8NSwPllCtrl.EventFlag, (B8NSWPLL_FLG_TRIGGED | B8NSWPLL_FLG_PIP4));
            Counter = 0U;
        }
    }
    return ;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSwPllVin5CallBackFunc
 *
 *  @Description:: User shall hook this callback function as VIN ISR of Ambarella SoC
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_PllSwPllVin5CallBackFunc(void)
{
    static UINT32 Counter = 0U;

    if (B8NSwPllCtrl.InitDone != 0U) {
        Counter++;
        if (Counter == B8NSWPLL_TRIGGER_PERIOD) {
            (void) AmbaB8_KAL_EventFlagSet(&B8NSwPllCtrl.EventFlag, (B8NSWPLL_FLG_TRIGGED | B8NSWPLL_FLG_PIP5));
            Counter = 0U;
        }
    }
    return ;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSetVoutClkHint
 *
 *  @Description:: Set VOUT clock ratio hint
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSetVoutClkHint(UINT32 ChipID, UINT32 Channel, UINT8 Ratio)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_CG_VOUT_REG_s CgVout = {0};

    CgVout.Div = Ratio;
    if (Channel == 0U) {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->CgVout0), 0, B8_DATA_WIDTH_32BIT, 1, &CgVout);
    } else if (Channel == 1U) {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->CgVout1), 0, B8_DATA_WIDTH_32BIT, 1, &CgVout);
    } else {
        RetVal = B8_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PllSetVoutClkSrc
 *
 *  @Description:: Set VOUT clock ratio hint
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllSetVoutClkSrc(UINT32 ChipID, UINT32 Channel, UINT8 ClkSrc)
{
    UINT32 RetVal = B8_ERR_NONE;

    if (Channel == 0U) {
        B8_PLL_VO_PRE_SCALER_REG_s  VideoPllPreScaler;

        (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_PllReg->VideoPllPreScaler), 0, B8_DATA_WIDTH_32BIT, 1, &VideoPllPreScaler);
        VideoPllPreScaler.VoRefSel = ClkSrc;
        VideoPllPreScaler.PreEnable = 0;
        VideoPllPreScaler.PreEnable = 1;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->VideoPllPreScaler), 0, B8_DATA_WIDTH_32BIT, 1, &VideoPllPreScaler);
    } else if (Channel == 1U) {
        B8_PLL_SENSOR_PRE_SCALER_REG_s  SensorPllPreScaler;

        (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_PllReg->SensorPllPreScaler), 0, B8_DATA_WIDTH_32BIT, 1, &SensorPllPreScaler);
        SensorPllPreScaler.SensorRefSel = ClkSrc;
        SensorPllPreScaler.PreEnable = 0;
        SensorPllPreScaler.PreEnable = 1;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SensorPllPreScaler), 0, B8_DATA_WIDTH_32BIT, 1, &SensorPllPreScaler);
    } else {
        RetVal = B8_ERR_UNEXPECTED;
    }

    return RetVal;
}

UINT32 AmbaB8_PllSetVout0RefFreq(UINT32 ChipID, UINT32 ClkRefFreq)
{
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl;

    pClkDomainCtrl = B8_GetPllTable(ChipID);
    pClkDomainCtrl->PllRefFreq[B8_PLL_VIDEO] = ClkRefFreq;

    return B8_ERR_NONE;
}

UINT32 AmbaB8_PllSetVout1RefFreq(UINT32 ChipID, UINT32 ClkRefFreq)
{
    B8_CLK_DOMAIN_CTRL_s *pClkDomainCtrl;

    pClkDomainCtrl = B8_GetPllTable(ChipID);
    pClkDomainCtrl->PllRefFreq[B8_PLL_SENSOR] = ClkRefFreq;

    return B8_ERR_NONE;
}
