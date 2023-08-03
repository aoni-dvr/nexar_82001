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
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaIntrinsics.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_DDRC.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_GIC.h"

#include "AmbaCSL_PLL.h"
#include "AmbaCSL_DDRC.h"

#include "AmbaRTSL_CV5XChipVer.h"

typedef struct {
    UINT32  IntProg:    7;
    UINT32  VcoType:    1;  /* 0 = Single-Ended VCO, 1 = Differenial VCO */
    UINT32  Sdiv:       4;
    UINT32  Sout:       4;
    UINT32  Fsdiv:      4;
    UINT32  Fsout:      4;
    UINT32  VcoDiv:     4;
} AMBA_PLL_COEFF_s;

typedef struct {
    UINT32      FspWrIdx;
    UINT32      FspOpIdx;
    UINT32      Mr1;
    UINT32      Mr2;
    UINT32      Clock;
    UINT32      Timing0;
    UINT32      Timing1;
    UINT32      Timing2;
    UINT32      Timing3;
    UINT32      Timing4;
    UINT32      DuelDieTiming;
    UINT32      RefreshTiming;
    UINT32      WdqsTiming;
} AMBA_DDRC_PARA_s;

#define MAX_PLL_VCO_FREQ_CORE_L1    2200000000UL    /* max core PLL VCO frequency for vco range level1 */
#define MAX_PLL_VCO_FREQ_CORE_L2    2700000000UL    /* max core PLL VCO frequency for vco range level2 */
#define MAX_PLL_VCO_FREQ_CORE_L3    3000000000UL    /* max core PLL VCO frequency for vco range level3 */
#define MIN_PLL_VCO_FREQ_CORE_L1    1100000000UL    /* min core PLL VCO frequency */

#define MAX_PLL_VCO_FREQ_HDMI_L1    4570000000ULL   /* max hdmi PLL VCO frequency for vco range level1 */
#define MAX_PLL_VCO_FREQ_HDMI_L2    5400000000ULL   /* max hdmi PLL VCO frequency for vco range level2 */
#define MIN_PLL_VCO_FREQ_HDMI_L1    2470000000ULL   /* min hdmi PLL VCO frequency */

#define COPY_DATA(a, b)  __asm__ __volatile__ ("mov %[des], %[src]" : [des] "=r" (a) : [src] "r" (b))

static AMBA_DDRC_PARA_s DdrcPara = {0U};

static FLOAT AmbaPllVcoFreq[AMBA_NUM_PLL];          /* VCO Frequency (Hz) */

static UINT32 AmbaPllVoutAClkMode = AMBA_PLL_VOUT_LCD_MIPI_DSI;
static UINT32 AmbaPllVoutAClkRatio = 1U;
static UINT32 AmbaPllVoutBClkMode = AMBA_PLL_VOUT_LCD_MIPI_DSI;
static UINT32 AmbaPllVoutBClkRatio = 1U;
static UINT32 AmbaPllVoutCClkMode = AMBA_PLL_VOUT_TV_HDMI;
static UINT32 AmbaPllVoutCClkRatio = 10U;

static void PLL_ChangeDdrcClock(void)
{
#define SP_CODE_ADDR     0x20e0030000UL

    ULONG Vco;
    ULONG Addr;
    UINT32 Frequency;
    UINT32 NewRegVal;
    volatile UINT32 *pPllCtrlReg[2];
    volatile UINT32 *pPllCtrl2Reg[2];
    volatile UINT32 *pPllCtrl3Reg[2];
    volatile UINT32 *pRctTimerReg;
    UINT32 Sdiv, Sout, IntProg, Fsdiv, Fsout;
    UINT32 ClkDiv2 = 0;
    UINT32 Reg2Bit12 = 0;
    UINT32 NewInt = 0;
    const AMBA_DDRC_PARA_s *pDdrcPara;
    AMBA_DDRC_REG_s *pDdrcReg;

    Addr = (SP_CODE_ADDR + 0x2000UL);
    COPY_DATA(pDdrcPara, Addr);

    Addr = (0x20ed180000UL);
    COPY_DATA(pPllCtrlReg[0U], Addr);
    Addr = (0x20ed180008UL);
    COPY_DATA(pPllCtrl2Reg[0U], Addr);
    Addr = (0x20ed18000cUL);
    COPY_DATA(pPllCtrl3Reg[0U], Addr);
    Addr = (0x20ed190000UL );
    COPY_DATA(pPllCtrlReg[1U], Addr);
    Addr = (0x20ed190008UL);
    COPY_DATA(pPllCtrl2Reg[1U], Addr);
    Addr = (0x20ed19000cUL);
    COPY_DATA(pPllCtrl3Reg[1U], Addr);

    Addr = 0x20ed080254UL;
    COPY_DATA(pRctTimerReg, Addr);

    Addr = (0x1000004000UL);
    COPY_DATA(pDdrcReg, Addr);
    if ((pDdrcPara->Clock/2U) <= 888000000U) {
        pDdrcReg->Timing0 = pDdrcPara->Timing0;
        pDdrcReg->Timing1 = pDdrcPara->Timing1;
        pDdrcReg->Timing2 = pDdrcPara->Timing2;
        pDdrcReg->Timing3 = pDdrcPara->Timing3;
        pDdrcReg->Timing4 = pDdrcPara->Timing4;

        pDdrcReg->DuelDieTiming = pDdrcPara->DuelDieTiming;
        pDdrcReg->RefreshTiming = pDdrcPara->RefreshTiming;
        pDdrcReg->WdqsTiming = pDdrcPara->WdqsTiming;
        pDdrcReg->DllCtrlSbc[0] = 0xe1b811U;
        pDdrcReg->DllCtrlSbc[1] = 0xe1b811U;
        pDdrcReg->DllCtrlSbc[2] = 0xe1b811U;
        pDdrcReg->DllCtrlSbc[3] = 0xe1b811U;
        pDdrcReg->DllCtrlSel1D0 = 0x5f5f5f5fU;
        pDdrcReg->DllCtrlSel1D1 = 0x5f5f5f5fU;
        pDdrcReg->DllCtrlSel0D0 = 0x0f0f0f0fU;
        pDdrcReg->DllCtrlSel0D1 = 0x0f0f0f0fU;
        pDdrcReg->DllCtrlSel2D0 = 0x0f0f0f0fU;
        pDdrcReg->DllCtrlSel2D0 = 0x0f0f0f0fU;
    } else {
        pDdrcReg->Timing0 = pDdrcPara->Timing0;
        pDdrcReg->Timing1 = pDdrcPara->Timing1;
        pDdrcReg->Timing2 = pDdrcPara->Timing2;
        pDdrcReg->Timing3 = pDdrcPara->Timing3;
        pDdrcReg->Timing4 = pDdrcPara->Timing4;

        pDdrcReg->DuelDieTiming = pDdrcPara->DuelDieTiming;
        pDdrcReg->RefreshTiming = pDdrcPara->RefreshTiming;
        pDdrcReg->WdqsTiming = pDdrcPara->WdqsTiming;
        pDdrcReg->DllCtrlSbc[0] = 0x3b811U;
        pDdrcReg->DllCtrlSbc[1] = 0x3b811U;
        pDdrcReg->DllCtrlSbc[2] = 0x3b811U;
        pDdrcReg->DllCtrlSbc[3] = 0x3b811U;
        pDdrcReg->DllCtrlSel1D0 = 0x0f0f0f0fU;
        pDdrcReg->DllCtrlSel1D1 = 0x0f0f0f0fU;
        pDdrcReg->DllCtrlSel0D0 = 0x00000000U;
        pDdrcReg->DllCtrlSel0D1 = 0x00000000U;
        pDdrcReg->DllCtrlSel2D0 = 0x20202020U;
        pDdrcReg->DllCtrlSel2D0 = 0x20202020U;
    }

    Frequency = 24000000U;
    Sdiv = (pPllCtrlReg[0U][0U] >> 12U) & 0xfU;
    Sout = (pPllCtrlReg[0U][0U] >> 16U) & 0xfU;
    IntProg = (pPllCtrlReg[0U][0U] >> 24U) & 0x7fU;
    Fsdiv = (pPllCtrl2Reg[0][0U] >> 9U) & 0x1U;
    Fsout= (pPllCtrl2Reg[0][0U] >> 11U) & 0x1U;
    ClkDiv2 = (pPllCtrl2Reg[0][0U] >> 8U) & 0x1U;
    Reg2Bit12 = (pPllCtrl2Reg[0][0U] >> 12U) & 0x1U;

    if ( 0x0U != Reg2Bit12 ) {
        //pll * 2 = 24000000 * (c2[8]+1) * (c2[9] + 1) * (c1[15:12]+1) * (c1[30:24] + 1)
        Frequency = Frequency >> 1U;
        Frequency = Frequency * (ClkDiv2 + 1U) * (Fsdiv + 1U) * (Sdiv + 1U);
    } else {
        Frequency = Frequency / (Fsout + 1U) / (Sout + 1U) / 2U;
        Frequency = Frequency * (Fsdiv + 1U) * (Sdiv + 1U);
    }
    NewInt = (pDdrcPara->Clock / Frequency) - 1U;

    if (NewInt > IntProg) {
        while (NewInt > IntProg) {
            IntProg = IntProg + 1U;
            NewRegVal = pPllCtrlReg[0U][0U] & (0XffffffU);
            NewRegVal = NewRegVal | (IntProg << 24U);
            pPllCtrlReg[0U][0U] = NewRegVal;
            pPllCtrlReg[0U][0U] = NewRegVal | 0x1U;
            pPllCtrlReg[1U][0U] = NewRegVal;
            pPllCtrlReg[1U][0U] = NewRegVal | 0x1U;
            pRctTimerReg[1U] = 1U;
            pRctTimerReg[1U] = 0U;
            pRctTimerReg[1U] = 2U;
            Frequency = pRctTimerReg[0U];
            while (Frequency < 72000U) {
                pRctTimerReg[1U] = 2U;
                Frequency = pRctTimerReg[0U];
                pRctTimerReg[1U] = 0U;
            }
            pPllCtrlReg[0U][0U] = NewRegVal;
            pPllCtrlReg[1U][0U] = NewRegVal;
        }
        Vco = 24000000UL * NewInt * (ClkDiv2 + 1U) * (Fsdiv + 1U) * (Sdiv + 1U);
        if (Vco < 4500000000UL) {
            pPllCtrl3Reg[0U][0U] = 0xc8000U;
            pPllCtrl3Reg[1U][0U] = 0xc8000U;
        } else if (Vco < 5500000000UL) {
            pPllCtrl3Reg[0U][0U] = 0xc8002U;
            pPllCtrl3Reg[1U][0U] = 0xc8002U;
        } else if (Vco < 6500000000UL) {
            pPllCtrl3Reg[0U][0U] = 0xc8004U;
            pPllCtrl3Reg[1U][0U] = 0xc8004U;
        } else {
            pPllCtrl3Reg[0U][0U] = 0xc8006U;
            pPllCtrl3Reg[1U][0U] = 0xc8006U;
        }
    } else {
        while (NewInt < IntProg) {
            IntProg = IntProg - 1U;
            NewRegVal = pPllCtrlReg[0U][0U] & (0XffffffU);
            NewRegVal = NewRegVal | (IntProg << 24U);
            pPllCtrlReg[0U][0U] = NewRegVal;
            pPllCtrlReg[0U][0U] = NewRegVal | 0x1U;
            pPllCtrlReg[1U][0U] = NewRegVal;
            pPllCtrlReg[1U][0U] = NewRegVal | 0x1U;
            pRctTimerReg[1U] = 1U;
            pRctTimerReg[1U] = 0U;
            pRctTimerReg[1U] = 2U;
            Frequency = pRctTimerReg[0U];
            while (Frequency < 72000U) {
                pRctTimerReg[1U] = 2U;
                Frequency = pRctTimerReg[0U];
                pRctTimerReg[1U] = 0U;
            }
            pPllCtrlReg[0U][0U] = NewRegVal;
            pPllCtrlReg[1U][0U] = NewRegVal;
        }
        Vco = 24000000UL * NewInt * (ClkDiv2 + 1U) * (Fsdiv + 1U) * (Sdiv + 1U);
        if (Vco < 4500000000UL) {
            pPllCtrl3Reg[0U][0U] = 0xc8000U;
            pPllCtrl3Reg[1U][0U] = 0xc8000U;
        } else if (Vco < 5500000000UL) {
            pPllCtrl3Reg[0U][0U] = 0xc8002U;
            pPllCtrl3Reg[1U][0U] = 0xc8002U;
        } else if (Vco < 6500000000UL) {
            pPllCtrl3Reg[0U][0U] = 0xc8004U;
            pPllCtrl3Reg[1U][0U] = 0xc8004U;
        } else {
            pPllCtrl3Reg[0U][0U] = 0xc8006U;
            pPllCtrl3Reg[1U][0U] = 0xc8006U;
        }
    }
}

static void PLL_CalNewDramPara(UINT32 Frequency)
{
    static UINT32 DramOpFspInd = 1U;
    static UINT32 DramWrFspInd = 1U;
    UINT32 ParaIndex = 0U;
    const UINT32 tWR[7] = {2U, 3U, 4U, 5U, 6U, 7U, 7U};
    const UINT32 tWL[7] = {4U, 4U, 4U, 5U, 6U, 6U, 7U};
    const UINT32 tRTP[7] = {2U, 3U, 4U, 5U, 6U, 7U, 7U};
    const UINT32 Timing0[7] = {0x210e0e07U, 0x2c13130aU, 0x3717170dU, 0x431c1c0fU, 0x4e212112U, 0x59262615U, 0x59262615U};
    const UINT32 Timing1[7] = {0x0e0732U, 0x130742U, 0x170951U, 0x1c0b62U, 0x210d72U, 0x261082U, 0x261082U};
    const UINT32 Timing2[7] = {0x200b0d00U, 0x2b0b1300U, 0x360b1700U, 0x400d1b00U, 0x4b0f1f00U, 0x56112100U, 0x56112100U};
    const UINT32 Timing3[7] = {0x100707U, 0x160a0aU, 0x1b0c0cU, 0x210f0fU, 0x271212U, 0x2c1515U, 0x2c1515U};
    const UINT32 Timing4[7] = {0x2471212U, 0x3872212U, 0x4872212U, 0x5c73212U, 0x6c73212U, 0x7874212U, 0x7874212U};
    const UINT32 DuelDieTiming[7] = {0x250d1112U, 0x250b1214U, 0x280b1314U, 0x2c0a1416U, 0x2f0a1516U, 0x33091618U, 0x33091618U};
    const UINT32 RefreshTiming[7] = {0x3f9c04bU, 0x5f2564bU, 0x7eaec4bU, 0x9e3804bU, 0xbdc184bU, 0xddcae4bU, 0xfdcae4bU};
    const UINT32 Wdqs[7] = {0x1905U, 0x1905U, 0x1B05U, 0x1D05U, 0x2107U, 0x2407U, 0x2407U};

    DdrcPara.FspWrIdx = (DramWrFspInd << 0x6U) | ((DramWrFspInd^1U) << 7U);
    DdrcPara.FspOpIdx = (DramWrFspInd << 0x6U) | ((DramOpFspInd) << 7U);

    if (Frequency > 533000000U) {
        ParaIndex = (Frequency - 533000000U) / 266000000U;
        if (ParaIndex >= 7U) {
            ParaIndex = 6U;
        }

        if (AmbaCSL_DdrcGetDramType(0U) != 1U) {
            DdrcPara.Clock = Frequency << 1U;
        } else {
            DdrcPara.Clock = Frequency;
        }
        DdrcPara.Mr2 = (0X9f020000U) | (tWL[ParaIndex] << 3U) | tRTP[ParaIndex];
        DdrcPara.Mr1 = (0X9f010004U) | (tWR[ParaIndex] << 0x4U);
        DdrcPara.Timing0 = Timing0[ParaIndex];
        DdrcPara.Timing1 = Timing1[ParaIndex];
        DdrcPara.Timing2 = Timing2[ParaIndex];
        DdrcPara.Timing3 = Timing3[ParaIndex];
        DdrcPara.Timing4 = Timing4[ParaIndex];
        DdrcPara.RefreshTiming = RefreshTiming[ParaIndex];
        DdrcPara.WdqsTiming = Wdqs[ParaIndex];
        DdrcPara.DuelDieTiming = DuelDieTiming[ParaIndex];
    } else {
        DdrcPara.Mr2 = 0x9f020052U;
        DdrcPara.Mr1 = 0x9f010024U;
        DdrcPara.Timing0 = 0x16090905U;
        DdrcPara.Timing1 = 0x0e090720U;
        DdrcPara.Timing2 = 0x150b0ddeU;
        DdrcPara.Timing3 = 0x000b0707U;
        DdrcPara.Timing4 = 0x6243a288U;
        DdrcPara.RefreshTiming = 0x04192e4bU;
        DdrcPara.WdqsTiming = 0x00001905U;
        DdrcPara.DuelDieTiming = 0x1f1f1f1fU;
    }

    DramWrFspInd = DramWrFspInd ^ 1U;
    DramOpFspInd = DramWrFspInd;
}

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
 *  PLL_GetHdmiRegVals - Get HDMI PLL value for specific clock frequency
 *  @param[in] Frequency:    TMDS clock frequency
 */
static void PLL_GetHdmiRegVals(UINT32 Frequency, UINT32 *pClkSelect, AMBA_PLL_CTRL_REGS_VAL_s *pHdmiRegVals, AMBA_PLL_CTRL_REGS_VAL_s *pHdmi2RegVals)
{
    if ((Frequency >= 296000000U) && (Frequency <= 297000000U)) {
        /* TMDS frequency = 297000000 */
        *pClkSelect = 0x001dU;

        pHdmiRegVals->Ctrl = 0x7a100009U;
        pHdmiRegVals->Ctrl2 = 0x20520100U;
        pHdmiRegVals->Ctrl3 = 0x000c800cU;
        pHdmiRegVals->Fraction = 0xc0000000U;

        /* LCPLL power down (HDMILC_CTRL_REG[5] = 1) */
        pHdmi2RegVals->Ctrl = 0xdeadbeafU;
        pHdmi2RegVals->Ctrl2 = 0xdeadbeafU;
        pHdmi2RegVals->Ctrl3 = 0xdeadbeafU;
        pHdmi2RegVals->Fraction = 0xdeadbeafU;

    } else if ((Frequency >= 300000000U) && (Frequency < 400000000U)) {
        /* TMDS frequency = 371250000 */
        *pClkSelect = 0x011dU;

        pHdmiRegVals->Ctrl = 0x4c000009U;
        pHdmiRegVals->Ctrl2 = 0x30521040U;
        pHdmiRegVals->Ctrl3 = 0x00088006U;
        pHdmiRegVals->Fraction = 0x58000000U;

        /* LCPLL power down (HDMILC_CTRL_REG[5] = 1) */
        pHdmi2RegVals->Ctrl = 0xdeadbeafU;
        pHdmi2RegVals->Ctrl2 = 0xdeadbeafU;
        pHdmi2RegVals->Ctrl3 = 0xdeadbeafU;
        pHdmi2RegVals->Fraction = 0xdeadbeafU;

    } else if (Frequency >= 400000000U) {
        /* TMDS frequency = 495000000 */
        *pClkSelect = 0x011dU;

        pHdmiRegVals->Ctrl = 0x36104000U;
        pHdmiRegVals->Ctrl2 = 0x30521050U;
        pHdmiRegVals->Ctrl3 = 0x00088006U;
        pHdmiRegVals->Fraction = 0x00088006U;

        /* LCPLL power down (HDMILC_CTRL_REG[5] = 1) */
        pHdmi2RegVals->Ctrl = 0xdeadbeafU;
        pHdmi2RegVals->Ctrl2 = 0xdeadbeafU;
        pHdmi2RegVals->Ctrl3 = 0xdeadbeafU;
        pHdmi2RegVals->Fraction = 0xdeadbeafU;

    } else {
        /* Shall not happen! Set default TMDS frequency = 297000000 */
        *pClkSelect = 0x001dU;

        pHdmiRegVals->Ctrl = 0x7a100009U;
        pHdmiRegVals->Ctrl2 = 0x20520100U;
        pHdmiRegVals->Ctrl3 = 0x000c800cU;
        pHdmiRegVals->Fraction = 0xc0000000U;

        /* LCPLL power down (HDMILC_CTRL_REG[5] = 1) */
        pHdmi2RegVals->Ctrl = 0xdeadbeafU;
        pHdmi2RegVals->Ctrl2 = 0xdeadbeafU;
        pHdmi2RegVals->Ctrl3 = 0xdeadbeafU;
        pHdmi2RegVals->Fraction = 0xdeadbeafU;

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
 *  PLL_CalcPllCoeff - Auto-calculate coefficients for the specified pll output frequency value
 *  @param[in] PllIdx PLL ID
 *  @param[in] RefFreq Reference clock frequency of the pll
 *  @param[in] PllOutFreq Target pll output frequency
 *  @param[out] pPllCoeff pointer to the pll coefficients
 */
static void PLL_CalcPllCoeff(UINT32 PllIdx, UINT32 RefFreq, UINT32 PllOutFreq, AMBA_PLL_COEFF_s *pPllCoeff)
{
    UINT32 DivSout = 1U, DivFsout = 1U;
    UINT32 DivSdiv = 2U;
    UINT32 DivIntProg;

    if ((pPllCoeff != NULL) && (RefFreq != 0U)) {
        pPllCoeff->VcoDiv = 0U;
        pPllCoeff->Fsdiv = 0U;

        /* Derive Sout */
        if (pPllCoeff->VcoType == 0U) {
            if ((PllIdx == AMBA_PLL_HDMI) || (PllIdx == AMBA_PLL_HDMILC)) {
                DivSout = (UINT32)((MAX_PLL_VCO_FREQ_HDMI_L1 >> 1ULL) / (UINT64)PllOutFreq);
                DivFsout = 2U;
                } else {
                DivSout = (UINT32)(MAX_PLL_VCO_FREQ_CORE_L1 / (UINT64)PllOutFreq);
                DivFsout = 1U;
            }
            if (DivSout == 0U) {
                DivSout = 1U;
            }
        }
        pPllCoeff->Fsout = (UINT8)(DivFsout - 1U);
        pPllCoeff->Sout = (UINT8)(DivSout - 1U);

        /* Derive Sdiv */
        DivIntProg = (UINT32)(((UINT64)PllOutFreq * (UINT64)DivSout) / (UINT64)RefFreq);
        if (DivIntProg > 128UL) {
            while ((DivIntProg / (DivSdiv - 1UL)) > 128UL) {
                DivSdiv += 1U;
                if (DivSdiv > 16U) {
                    DivSdiv = 16U;
                    break;
                }
            }
        }

        pPllCoeff->Sdiv = (UINT8)(DivSdiv - 1U);

        /* Derive IntProg */
        DivIntProg = DivIntProg / (DivSdiv);
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
    UINT32 DivSdiv, DivSout, DivFsout;
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
            if ((PllIdx != AMBA_PLL_VIDEOA) && (PllIdx != AMBA_PLL_HDMI)) {
                /* Auto-calculate 4-bit divider value */
                PLL_SetupCorePostDivVal(Frequency, &Postscaler);

#ifdef AMBA_PLL_FOURFOLD_GCLK_SO_VIN
                /* to make gclk_so_vin = 4 x clk_si, postscaler is required to be even number. */
                if ((PllIdx == AMBA_PLL_SENSOR0) || (PllIdx == AMBA_PLL_SENSOR2)) {
                    if ((Postscaler % 4U) != 0U) {
                        Postscaler = ((Postscaler / 4U) + 1U) * 4U;
                    }
                }
#endif
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
        NewPllCtrlRegsVal.Ctrl2 |= (UINT32)PllCoeff.Fsout << 8UL;

        DivIntProg = PllCoeff.IntProg + (UINT32)1U;
        DivSdiv  = PllCoeff.Sdiv + (UINT32)1U;
        DivSout  = PllCoeff.Sout + (UINT32)1U;
        DivFsout = PllCoeff.Fsout + (UINT32)1U;

        /* Calculate VCO range */
        WorkUint64 = (UINT64)RefFreq * (UINT64)DivFsout * (UINT64)DivSdiv * ((UINT64)DivIntProg + 1ULL);
        /* NOTE: Prepare PllCtrl3Reg value */
        if ((PllIdx == AMBA_PLL_HDMI) || (PllIdx == AMBA_PLL_HDMILC)) {
            PLL_SetupPllVcoRangeVal(1U, WorkUint64, &NewPllCtrlRegsVal);
        } else {
            PLL_SetupPllVcoRangeVal(0U, WorkUint64, &NewPllCtrlRegsVal);
        }

        /* The range of the integer+fraction is [<int+0> to <int+1>) */
        /* fIntFreq = Integer portion */
        Postscaler *= DivSout;
        fIntFreq = (DOUBLE)RefFreq * (DOUBLE)DivSdiv * (DOUBLE)DivIntProg;
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
    UINT32 DivSdiv, DivSout;
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

        /* fIntFreq = Integer portion */
        Postscaler *= (DivSout);
        fIntFreq = (DOUBLE)RefFreq *(DOUBLE)DivSdiv * (DOUBLE)DivIntProg;
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
        }  else {
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
    UINT32 IntProg, Sdiv, Sout, Prescaler;
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
        WorkDouble /= (DOUBLE)Sout;

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

void AmbaRTSL_PllSetDdrcClk(UINT32 Frequency)
{
    const UINT32 volatile *pDramCode;
    UINT32 volatile *pScratchpadCode;
    UINT64 Addr;
    void (*AliasRunSpCode)(ULONG StackAddr) = NULL;
    UINT32 i;

    PLL_CalNewDramPara(Frequency);
    /* copy SR function */
    COPY_DATA(pDramCode, ambarella_dram_clockchang);
    Addr = SP_CODE_ADDR;
    AmbaMisra_TypeCast(&pScratchpadCode, &Addr);
    /* copy code to scratchpad */
    for (i = 0; i < 1024U; i++) {
        pScratchpadCode[i] = pDramCode[i];
    }
    AmbaMisra_TypeCast(&AliasRunSpCode, &Addr);

    COPY_DATA(pDramCode, PLL_ChangeDdrcClock);
    Addr = SP_CODE_ADDR + 0x1000UL;
    AmbaMisra_TypeCast(&pScratchpadCode, &Addr);
    /* copy code to scratchpad */
    for (i = 0; i < 1024U; i++) {
        pScratchpadCode[i] = pDramCode[i];
    }

    COPY_DATA(pDramCode, &DdrcPara);
    Addr = SP_CODE_ADDR + 0x2000UL;
    AmbaMisra_TypeCast(&pScratchpadCode, &Addr);
    /* copy code to scratchpad */
    for (i = 0; i < 128U; i++) {
        pScratchpadCode[i] = pDramCode[i];
    }

    AmbaRTSL_GicIntGlobalDisable();
    AmbaMMU_Disable();
    AMBA_DSB();
    AMBA_ISB();

    /* jump to scratchpad */
    AliasRunSpCode(SP_CODE_ADDR + 0x3000UL);
    AmbaMMU_Enable();

    AmbaRTSL_GicIntGlobalEnable();
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
 *  AmbaRTSL_PllSetDsuClk - Set DSU clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetDsuClk(UINT32 Frequency)
{
    if (Frequency != AmbaRTSL_PllGetDsuClk()) {
        AmbaCSL_PllSetFractionMode(AMBA_PLL_DSU, 0U);
        (void)AmbaRTSL_PllSetFreq(AMBA_PLL_DSU, Frequency);
        (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_DSU);  /* Get DSU (gclk_dsu) clock frequency */
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
    AmbaCSL_PllSetFractionMode(AMBA_PLL_ENET, 1U);
    (void)AmbaRTSL_PllSetFreq(AMBA_PLL_ENET, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_ENET);    /* Get Ethernet (gclk_enet_clk_rmii) clock frequency */

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
 *  AmbaRTSL_PllSetIdspvClk - Set IDSPV clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetIdspvClk(UINT32 Frequency)
{
    UINT32 OriginalClk = AmbaRTSL_PllGetIdspClk();

    /* Do not use AmbaRTSL_PllSetFreq to prevent clock glitch problem */
    (void)AmbaRTSL_PllAdjFreq(AMBA_PLL_IDSPV, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_IDSPV); /* Get IDSP (pll_out_idsp) clock frequency */

    if (OriginalClk != AmbaRTSL_PllGetIdspvClk()) {
        (void)AmbaRTSL_PllNotifyFreqChanges(AMBA_PLL_IDSPV);
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
 *  AmbaRTSL_PllSetNandClk - Set NAND clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetNandClk(UINT32 Frequency)
{
    AmbaCSL_PllSetFractionMode(AMBA_PLL_NAND, 0U);
    (void)AmbaRTSL_PllSetFreq(AMBA_PLL_NAND, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_NAND);    /* Get NAND (gclk_nand) clock frequency */

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

    /* set Sensor VIN4 (gclk_so_vin5) postscaler */
#ifdef AMBA_PLL_FOURFOLD_GCLK_SO_VIN
    /* Due to CPHY requirement( (gclk_so_vin5 > idspv_clk/32) && (gclk_so_vin5 > symbol rate / 7 / 5.5),
       configure gclk_so_vin5 = 4 x clk_si by default */
    SoVinClkDivider = AmbaCSL_PllGetSensorPostscaler() / 4U;
#else
    SoVinClkDivider = AmbaCSL_PllGetSensorPostscaler();
#endif
    if (SoVinClkDivider != AmbaCSL_PllGetVin4Divider()) {
        pAmbaRCT_Reg->Vin4ClkDivider.WriteEnable = 0U;
        pAmbaRCT_Reg->Vin4ClkDivider.Divider = (UINT8)(SoVinClkDivider - 1U);
        pAmbaRCT_Reg->Vin4ClkDivider.WriteEnable = 1U;
        pAmbaRCT_Reg->Vin4ClkDivider.WriteEnable = 0U;
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetSensor1Clk - Set Sensor 1 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSensor1Clk(UINT32 Frequency)
{
    UINT32 SoVinClkDivider;

    AmbaCSL_PllSetFractionMode(AMBA_PLL_SENSOR2, 1U);
    (void)AmbaRTSL_PllSetFreq(AMBA_PLL_SENSOR2, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_SENSOR2); /* Get Sensor2 (gclk_so2) clock frequency */

    /* set Sensor VIN0 (gclk_so_vin1) postscaler */
#ifdef AMBA_PLL_FOURFOLD_GCLK_SO_VIN
    /* Due to CPHY requirement( (gclk_so_vin1 > idspv_clk/32) && (gclk_so_vin1 > symbol rate / 7 / 5.5),
       configure gclk_so_vin1 = 4 x clk_si by default */
    SoVinClkDivider = AmbaCSL_PllGetSensor2Postscaler() / 4U;

#else
    SoVinClkDivider = AmbaCSL_PllGetSensor2Postscaler();
#endif
    if (SoVinClkDivider != AmbaCSL_PllGetVin0Divider()) {
        pAmbaRCT_Reg->Vin0ClkDivider.WriteEnable = 0U;
        pAmbaRCT_Reg->Vin0ClkDivider.Divider = (UINT8)(SoVinClkDivider - 1U);
        pAmbaRCT_Reg->Vin0ClkDivider.WriteEnable = 1U;
        pAmbaRCT_Reg->Vin0ClkDivider.WriteEnable = 0U;
    }

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllSetSlvsEcClk - Set Slvs-Ec clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetSlvsEcClk(UINT32 Frequency)
{
    AmbaCSL_PllSetFractionMode(AMBA_PLL_SLVSEC, 1U);
    (void)AmbaRTSL_PllSetFreq(AMBA_PLL_SLVSEC, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_SLVSEC);    /* Get Slvs-Ec (pll_out_slvsec / rct_slvsecphy_cfgclk) clock frequency */

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
 *  AmbaRTSL_PllSetVoutAClk - Set VoutA clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVoutAClk(UINT32 Frequency)
{
    UINT32 RetStatus = PLL_ERR_NONE;

    /* Set VoutA (gclk_vo_a) */
    AmbaCSL_PllSetFractionMode(AMBA_PLL_VIDEOA, 1U);
    AmbaCSL_PllSetPreScaleRegVal(AMBA_PLL_VIDEOA, 1U);
    AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_VIDEOA, AmbaPllVoutAClkRatio);
    AmbaCSL_PllSetVoutARefClkSelect(0);

    if ((AmbaPllVoutAClkMode == AMBA_PLL_VOUT_LCD_MIPI_DSI) || (AmbaPllVoutAClkMode == AMBA_PLL_VOUT_LCD_MIPI_CSI)) {
        AmbaCSL_PllSetDsiAClkSelect(0U);
        AmbaCSL_PllSetVoutDisplayASelect(0U);    /* Use gclk_vo_a as Display-A clock source */
    } else if (AmbaPllVoutAClkMode == AMBA_PLL_VOUT_LCD_FPD_LINK) {
        AmbaCSL_PllSetDsiAClkSelect(0U);
        AmbaCSL_PllSetVoutDisplayASelect(0U);    /* Use gclk_vo_a as Display-A clock source */

        /* each data-lane always send 7-bit per pixel (refer to spec) */
        AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_VIDEOA, 7U);
    } else {
        RetStatus = PLL_ERR_IMPL;
    }

    if (RetStatus == PLL_ERR_NONE) {
        (void)AmbaRTSL_PllSetFreq(AMBA_PLL_VIDEOA, Frequency);
        (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_VIDEOA);  /* Get VOUTA (gclk_vo_a) clock frequency */
    }

    return RetStatus;
}

/**
 *  AmbaRTSL_PllSetVoutBClk - Set VoutB clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVoutBClk(UINT32 Frequency)
{
    UINT32 RetStatus = PLL_ERR_NONE;
    UINT64 PllOutFreq = (UINT64) Frequency;

    /* Set VoutB (gclk_vo_b) */
    AmbaCSL_PllSetFractionMode(AMBA_PLL_HDMI, 1U);
    AmbaCSL_PllSetPreScaleRegVal(AMBA_PLL_HDMI, 1U);
    AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_HDMI, AmbaPllVoutBClkRatio);

    /* Clean the bypass select bit at initial */
    pAmbaRCT_Reg->VoutClkSelect.VoutBypassSelect = 0U;
    pAmbaRCT_Reg->VoutClkSelect.VoutBRefClkSelect = 0U;

    if ((AmbaPllVoutBClkMode == AMBA_PLL_VOUT_LCD_MIPI_DSI) || (AmbaPllVoutBClkMode == AMBA_PLL_VOUT_LCD_MIPI_CSI)) {
        AmbaCSL_PllSetDsiBClkSelect(0U);
        AmbaCSL_PllSetVoutDisplayBSelect(0U);    /* Use gclk_hdmi as Display-B clock source */
    } else if (AmbaPllVoutBClkMode == AMBA_PLL_VOUT_LCD_FPD_LINK) {
        AmbaCSL_PllSetDsiBClkSelect(0U);
        AmbaCSL_PllSetVoutDisplayBSelect(0U);    /* Use gclk_hdmi as Display-B clock source */

        /* each data-lane always send 7-bit per pixel (refer to spec) */
        AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_HDMI, 7U);
    } else {
        RetStatus = PLL_ERR_IMPL;
    }

    if (RetStatus == PLL_ERR_NONE) {
        PllOutFreq = (UINT64) Frequency * AmbaPllVoutBClkRatio;

        /* Cannot take clock > 3GHz, otherwise output clock will become unstable */
        if (PllOutFreq < 3000000000U) {
            (void)AmbaRTSL_PllSetFreq(AMBA_PLL_HDMILC, 0U);
            (void)AmbaRTSL_PllSetFreq(AMBA_PLL_HDMI, Frequency);
        } else {
            RetStatus = PLL_ERR_IMPL;
        }

        /* Need to enable postscaler after PLL_HDMI_CTRL_REGs were enabled */
        AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_HDMI, AmbaPllVoutBClkRatio);

        (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_HDMI);  /* Get VOUTB (gclk_vo_b) clock frequency */
    }

    return RetStatus;
}

/**
 *  AmbaRTSL_PllSetVoutCClk - Set VOUT HDMI PLL.  It will be 1/10 or 1/40 HDMI PHY freq when HDMI is used.
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVoutCClk(UINT32 Frequency)
{
    /* Set VoutC (pll_out_hdmi) */
    UINT32 ClkSelect = 0x001dU, RetStatus = PLL_ERR_NONE;
    UINT64 PllOutFreq = (UINT64) Frequency;
    AMBA_PLL_CTRL_REGS_VAL_s NewHdmiRegsVal, NewHdmi2RegsVal;
    UINT32 ChipID = AmbaRTSL_CV5XChipVerGet();

    if ((AmbaPllVoutCClkMode == AMBA_PLL_VOUT_TV_HDMI) || (AmbaPllVoutCClkMode == AMBA_PLL_VOUT_TV_CVBS)) {
        AmbaCSL_PllSetVoutDisplayCSelect(2U);    /* Use clk160d as Display-C clock source */
        AmbaCSL_PllSetHdmiUsePhyClkVo(1U);       /* 1 - use (gclk_hdmi_phy / 10) for gclk_vo */

        /*
         * Set HDMI PLL to be always 10x
         */
        PllOutFreq = (UINT64) Frequency * 10U;
    } else {
        RetStatus = PLL_ERR_IMPL;
    }

    if (RetStatus == PLL_ERR_NONE) {
        (void)AmbaRTSL_PllSetVoutCClkConfig(AMBA_PLL_HDMI_CLK_RCT);

        AmbaCSL_PllSetFractionMode(AMBA_PLL_HDMI, 1U);      /* 1 - HDMI PLL use fractional mode */
        AmbaCSL_PllSetPostScaleRegVal(AMBA_PLL_HDMI, 1U);   /* VOUTC does not use postscaler, set to 1 */

        if (AmbaPllVoutCClkMode != AMBA_PLL_VOUT_TV_HDMI) {
            /* Non-HDMI display. Use HDMI PLL only */
            AmbaCSL_PllSetHdmiPhyPllSrcSelect(1U);          /* 1 - use HDMI PLL for gclk_hdmi_phy */
            AmbaCSL_PllSetHdmiPllCascade(0U);               /* 0 - HDMI PLL supplies clock to HDMI2 PLL */

            /* Power Down the PLL (save power) */
            (void)AmbaRTSL_PllSetFreq(AMBA_PLL_HDMILC, 0U);
            (void)AmbaRTSL_PllSetFreq(AMBA_PLL_HDMI, (UINT32)PllOutFreq);
        } else {
            /* HDMI display */
            if (Frequency <= 200000000U) {       /* CASE A. Frequency <= 200000000 */
                /* Use HDMI PLL only */
                AmbaCSL_PllSetHdmiPhyPllSrcSelect(1U);      /* 1 - use HDMI PLL for gclk_hdmi_phy */
                AmbaCSL_PllSetHdmiPllCascade(0U);           /* 0 - HDMI PLL supplies clock to HDMI2 PLL */

                /* Power Down the PLL (save power) */
                (void)AmbaRTSL_PllSetFreq(AMBA_PLL_HDMILC, 0U);
                (void)AmbaRTSL_PllSetFreq(AMBA_PLL_HDMI, (UINT32)PllOutFreq);

                /* apply a proper HDMI phy setting */
                pAmbaRCT_Reg->HdmiPhyCtrl.Data = 0x18820240U;
                pAmbaRCT_Reg->HdmiPhyCtrl.Data = 0x18820641U;
            } else if (Frequency < 550000000U) { /* CASE B. 200000000 < Frequency < 550000000 */
                /* Use Ring Pll with hardcoded pll setting */
                (void)AmbaRTSL_PllSetFreq(AMBA_PLL_HDMI, 0U);
                (void)AmbaRTSL_PllSetFreq(AMBA_PLL_HDMILC, 0U);

                PLL_GetHdmiRegVals(Frequency, &ClkSelect, &NewHdmiRegsVal, &NewHdmi2RegsVal);
                pAmbaRCT_Reg->HdmiClkSelect.Data = ClkSelect;

                AmbaCSL_PllSetCtrlRegsVal(AMBA_PLL_HDMI, &NewHdmiRegsVal);
                AmbaCSL_PllSetCtrlRegsVal(AMBA_PLL_HDMILC, &NewHdmi2RegsVal);

                /* apply a proper HDMI phy setting */
                pAmbaRCT_Reg->HdmiPhyCtrl.Data = 0x18810ff0U;
                pAmbaRCT_Reg->HdmiPhyCtrl.Data = 0x18810ff1U;
            } else {                            /* CASE C. 550000000 <= Frequency */
                /* hardcoded pll value, TMDS frequency = 594000000 */
                pAmbaRCT_Reg->T2vCtrl.PowerDown = 0U;       /* Need to use bias voltage from T2v */

                if (ChipID == CV5X_CHIP_VER_A1) {
                    NewHdmiRegsVal.Ctrl = 0x14071000U;
                    NewHdmiRegsVal.Ctrl2 = 0x00520300U;
                    NewHdmiRegsVal.Ctrl3 = 0x000c8000U;
                    NewHdmiRegsVal.Fraction = 0xc0000000U;
                    NewHdmi2RegsVal.Ctrl = 0x3c800008U;
                    NewHdmi2RegsVal.Ctrl2 = 0x020a1300U;
                    NewHdmi2RegsVal.Ctrl3 = 0x0fcd4020U;
                    NewHdmi2RegsVal.Fraction = 0xe0000000U;

                    /* Using pll2(LCPLL) only */
                    pAmbaRCT_Reg->HdmiClkSelect.Data = 0x8d15U;

                    AmbaCSL_PllSetCtrlRegsVal(AMBA_PLL_HDMI, &NewHdmiRegsVal);
                    AmbaCSL_PllSetCtrlRegsVal(AMBA_PLL_HDMILC, &NewHdmi2RegsVal);
                    pAmbaRCT_Reg->HdmiLcPllCtrl4 = 0x0001d891U;

                    /* apply a proper HDMI phy setting */
                    pAmbaRCT_Reg->HdmiPhyCtrl.Data = 0x0f400ff0U;
                    pAmbaRCT_Reg->HdmiPhyCtrl.Data = 0x0f400ff1U;

                } else {
                    NewHdmiRegsVal.Ctrl = 0x11000000U;
                    NewHdmiRegsVal.Ctrl2 = 0x20521300U;
                    NewHdmiRegsVal.Ctrl3 = 0x000c400cU;
                    NewHdmiRegsVal.Fraction = 0U;
                    NewHdmi2RegsVal.Ctrl = 0x660e0008U;
                    NewHdmi2RegsVal.Ctrl2 = 0x020a0900U;
                    NewHdmi2RegsVal.Ctrl3 = 0x01cd4020U;
                    NewHdmi2RegsVal.Fraction = 0x20000000U;

                    /* Using both pll1 and pll2(LCPLL) */
                    pAmbaRCT_Reg->HdmiClkSelect.Data = 0x8d1dU;

                    /* Config PLL Ctrl Registers */
                    AmbaCSL_PllSetCtrlRegsVal(AMBA_PLL_HDMI, &NewHdmiRegsVal);
                    AmbaCSL_PllSetCtrlRegsVal(AMBA_PLL_HDMILC, &NewHdmi2RegsVal);

                    /* apply a proper HDMI phy setting */
                    pAmbaRCT_Reg->HdmiPhyCtrl.Data = 0x0f200ff0U;
                    pAmbaRCT_Reg->HdmiPhyCtrl.Data = 0x0f400ff1U;
                }
            }
        }

        /* Power cycle HDMI PHY */
        AmbaCSL_PllSetHdmiPhyPowerDown(0U);
        AmbaDelayCycles(AmbaCSL_PllGetNumCpuCycleUs() << 4UL);
        AmbaCSL_PllSetHdmiPhyPowerDown(1U);
        AmbaDelayCycles(AmbaCSL_PllGetNumCpuCycleUs() << 4UL);

        if (Frequency <= 200000000U) {
            (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_HDMI);    /* Calculate HDMI PLL clock frequency */
            (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_HDMILC);   /* Calculate HDMILC PLL clock frequency */
        } else {
            /* Store HDMI/HDMILC PLL clock frequency directly */
            if ((Frequency >= 296000000U) && (Frequency <= 297000000U)) {
                /* TMDS frequency = 297000000 */
                (void)AmbaRTSL_PllSetPllOutFreq(AMBA_PLL_HDMI, (FLOAT)297000000U);
                (void)AmbaRTSL_PllSetPllOutFreq(AMBA_PLL_HDMILC, (FLOAT)297000000U);

            } else if ((Frequency >= 300000000U) && (Frequency < 400000000U)) {
                /* TMDS frequency = 371250000 */
                (void)AmbaRTSL_PllSetPllOutFreq(AMBA_PLL_HDMI, (FLOAT)371250000U);
                (void)AmbaRTSL_PllSetPllOutFreq(AMBA_PLL_HDMILC, (FLOAT)371250000U);

            } else if (Frequency >= 550000000U) {
                /* TMDS frequency = 594000000 */
                (void)AmbaRTSL_PllSetPllOutFreq(AMBA_PLL_HDMI, (FLOAT)594000000U);
                (void)AmbaRTSL_PllSetPllOutFreq(AMBA_PLL_HDMILC, (FLOAT)594000000U);

            } else {
                RetStatus = PLL_ERR_ARG;
            }
        }
    }

    return RetStatus;
}

/**
 *  AmbaRTSL_PllSetVoutAClkHint - Set VOUT-A clock ratio hint
 *  @param[in] Type VOUT-LCD interface selection
 *  @param[in] Ratio VOUT-LCD clock ratio selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVoutAClkHint(UINT32 Type, UINT32 Ratio)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if ((Type >= AMBA_NUM_PLL_VOUT_LCD_TYPE) || (Ratio == 0U)) {
        RetVal = PLL_ERR_ARG;
    } else {
        AmbaPllVoutAClkMode = Type;
        AmbaPllVoutAClkRatio = Ratio;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetVoutBClkHint - Set VOUT-B clock ratio hint
 *  @param[in] Type VOUT-LCD interface selection
 *  @param[in] Ratio VOUT-LCD clock ratio selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVoutBClkHint(UINT32 Type, UINT32 Ratio)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if ((Type >= AMBA_NUM_PLL_VOUT_LCD_TYPE) || (Ratio == 0U)) {
        RetVal = PLL_ERR_ARG;
    } else {
        AmbaPllVoutBClkMode = Type;
        AmbaPllVoutBClkRatio = Ratio;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PllSetVoutCClkHint - Set VOUT-C clock ratio hint
 *  @param[in] Type VOUT-TV interface selection
 *  @param[in] Ratio VOUT-TV clock ratio selection
 *  @return error code
 */
UINT32 AmbaRTSL_PllSetVoutCClkHint(UINT32 Type, UINT32 Ratio)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if ((Type >= AMBA_NUM_PLL_VOUT_TV_TYPE) || (Ratio == 0U)) {
        RetVal = PLL_ERR_ARG;
    } else {
        AmbaPllVoutCClkMode = Type;
        AmbaPllVoutCClkRatio = Ratio;
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
 *  AmbaRTSL_PllFineAdjSensor1Clk - Fine adjust Sensor 1 clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllFineAdjSensor1Clk(UINT32 Frequency)
{
    (void)AmbaRTSL_PllFineAdjFreq(AMBA_PLL_SENSOR2, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_SENSOR2); /* Get Sensor2 (gclk_so2) clock frequency */

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
 *  AmbaRTSL_PllFineAdjVoutAClk - Fine adjust VoutA clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllFineAdjVoutAClk(UINT32 Frequency)
{
    (void)AmbaRTSL_PllFineAdjFreq(AMBA_PLL_VIDEOA, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_VIDEOA);  /* Get VOUTA (gclk_vo_a) clock frequency */

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllFineAdjVoutBClk - Fine adjust VoutB clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllFineAdjVoutBClk(UINT32 Frequency)
{
    (void)AmbaRTSL_PllFineAdjFreq(AMBA_PLL_HDMI, Frequency);
    (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_HDMI);  /* Get VOUTB (gclk_vo_b) clock frequency */

    return PLL_ERR_NONE;
}

/**
 *  AmbaRTSL_PllFineAdjVoutCClk - Fine adjust VoutC clock frequency
 *  @param[in] Frequency Target clock frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PllFineAdjVoutCClk(UINT32 Frequency)
{
    UINT32 RetVal = PLL_ERR_NONE;

    if (Frequency <= 200000000U) {
        (void)AmbaRTSL_PllFineAdjFreq(AMBA_PLL_HDMI, Frequency * 10U);
        (void)AmbaRTSL_PllCalcPllOutFreq(AMBA_PLL_HDMI);  /* Get VOUTC (pll_hdmi) clock frequency */
    } else {
        /* Not support */
        RetVal = PLL_ERR_ARG;
    }

    return RetVal;
}
