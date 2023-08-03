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
#include "AmbaCSL_TMR.h"

/**
 *  TMR_GetCounterRegs - Get timer counter reigsters
 *  @param[in] TimerID Timer ID
 *  @return Timer counter registers
 */
static AMBA_TMR_COUNTER_REG_s * TMR_GetCounterRegs(UINT32 TimerID)
{
    AMBA_TMR_COUNTER_REG_s *pTmrCounterRegs = NULL;
    UINT32 TmrRegIndex = 0U, TempID = TimerID;

    if (TimerID >= AMBA_NUM_TIMER) {
        pTmrCounterRegs = NULL;
    } else {
        /* 10 timers per timer register gruop */
        if (TempID >= 10U) {
            TmrRegIndex = 1U;
            TempID -= 10U;
        }

        if (TempID == AMBA_TIMER0) {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter0;
        } else if (TempID == AMBA_TIMER1) {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter1;
        } else if (TempID == AMBA_TIMER2) {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter2;
        } else if (TempID == AMBA_TIMER3) {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter3;
        } else if (TempID == AMBA_TIMER4) {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter4;
        } else if (TempID == AMBA_TIMER5) {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter5;
        } else if (TempID == AMBA_TIMER6) {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter6;
        } else if (TempID == AMBA_TIMER7) {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter7;
        } else if (TempID == AMBA_TIMER8) {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter8;
        } else {
            pTmrCounterRegs = &pAmbaTMR_Regs[TmrRegIndex]->Counter9;
        }
    }

    return pTmrCounterRegs;
}

/**
 *  AmbaCSL_TmrSetConfig - Enable Timer interrupt when overflow
 *  @param[in] TimerID Timer ID
 *  @param[in] ClkSel Clock source selection
 *  @param[in] IntCtrl Interrupt control
 */
void AmbaCSL_TmrSetConfig(UINT32 TimerID, UINT32 ClkSel, UINT32 IntCtrl)
{
    UINT32 TimerCtrl = 0U;
    UINT32 BitOffset = 0U;

    if (TimerID < AMBA_TIMER8) {
        TimerCtrl = pAmbaTMR_Regs[0]->TimerCtrl0;
        BitOffset = (TimerID << 2U);
    } else if (TimerID < AMBA_TIMER10) {
        TimerCtrl = pAmbaTMR_Regs[0]->TimerCtrl1;
        BitOffset = ((TimerID - 8U) << 2U);
    } else if (TimerID < AMBA_TIMER18) {
        TimerCtrl = pAmbaTMR_Regs[1]->TimerCtrl0;
        BitOffset = ((TimerID - 10U) << 2U);
    } else if (TimerID < AMBA_NUM_TIMER) {
        TimerCtrl = pAmbaTMR_Regs[1]->TimerCtrl1;
        BitOffset = ((TimerID - 18U) << 2U);
    } else {
        /* Shall not happen!! */
    }

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    TimerCtrl &= ~((UINT32)0x7U << (BitOffset + 1U));

    if ((ClkSel == AMBA_TMR_CLK_EXT) && (TimerID < AMBA_TIMER3)) {
        TimerCtrl |= (ClkSel << (BitOffset + 1U));
    } else {
        TimerCtrl |= (ClkSel << (BitOffset + 3U));
    }
#else
    TimerCtrl &= ~((UINT32)0x3U << (BitOffset + 1U));
    TimerCtrl |= (ClkSel << (BitOffset + 1U));
#endif
    TimerCtrl |= (IntCtrl << (BitOffset + 2U));

    if (TimerID < AMBA_TIMER8) {
        pAmbaTMR_Regs[0]->TimerCtrl0 = TimerCtrl;
    } else if (TimerID < AMBA_TIMER10) {
        pAmbaTMR_Regs[0]->TimerCtrl1 = TimerCtrl;
    } else if (TimerID < AMBA_TIMER18) {
        pAmbaTMR_Regs[1]->TimerCtrl0 = TimerCtrl;
    } else if (TimerID < AMBA_NUM_TIMER) {
        pAmbaTMR_Regs[1]->TimerCtrl1 = TimerCtrl;
    } else {
        /* Shall not happen!! */
    }
}

/**
 *  AmbaCSL_TmrSetEnable - Enable/Disable Timer
 *  @param[in] TimerID Timer ID
 *  @param[in] Enable Enable(1)/Disable(0)
 */
void AmbaCSL_TmrSetEnable(UINT32 TimerID, UINT32 Enable)
{
    UINT32 TimerCtrl = 0U;
    UINT32 BitOffset = 0U;

    if (TimerID < AMBA_TIMER8) {
        TimerCtrl = pAmbaTMR_Regs[0]->TimerCtrl0;
        BitOffset = (TimerID << 2U);
    } else if (TimerID < AMBA_TIMER10) {
        TimerCtrl = pAmbaTMR_Regs[0]->TimerCtrl1;
        BitOffset = ((TimerID - 8U) << 2U);
    } else if (TimerID < AMBA_TIMER18) {
        TimerCtrl = pAmbaTMR_Regs[1]->TimerCtrl0;
        BitOffset = ((TimerID - 10U) << 2U);
    } else if (TimerID < AMBA_NUM_TIMER) {
        TimerCtrl = pAmbaTMR_Regs[1]->TimerCtrl1;
        BitOffset = ((TimerID - 18U) << 2U);
    } else {
        /* Shall not happen!! */
    }

    if (Enable != 0U) {
        TimerCtrl |= ((UINT32)0x1U << BitOffset);
    } else {
        TimerCtrl &= ~((UINT32)0x1U << BitOffset);
    }

    if (TimerID < AMBA_TIMER8) {
        pAmbaTMR_Regs[0]->TimerCtrl0 = TimerCtrl;
    } else if (TimerID < AMBA_TIMER10) {
        pAmbaTMR_Regs[0]->TimerCtrl1 = TimerCtrl;
    } else if (TimerID < AMBA_TIMER18) {
        pAmbaTMR_Regs[1]->TimerCtrl0 = TimerCtrl;
    } else if (TimerID < AMBA_NUM_TIMER) {
        pAmbaTMR_Regs[1]->TimerCtrl1 = TimerCtrl;
    } else {
        /* Shall not happen!! */
    }
}

/**
 *  AmbaCSL_TmrSetCurrentVal - Set timer current value
 *  @param[in] TimerID Timer ID
 *  @param[in] Val Current value
 */
void AmbaCSL_TmrSetCurrentVal(UINT32 TimerID, UINT32 Val)
{
    AMBA_TMR_COUNTER_REG_s *pTmrCounterRegs = TMR_GetCounterRegs(TimerID);

    if (pTmrCounterRegs != NULL) {
        pTmrCounterRegs->Status = Val;
    }
}

/**
 *  AmbaCSL_TmrSetReloadVal - Set timer reload value
 *  @param[in] TimerID Timer ID
 *  @param[in] Val Reload value
 */
void AmbaCSL_TmrSetReloadVal(UINT32 TimerID, UINT32 Val)
{
    AMBA_TMR_COUNTER_REG_s *pTmrCounterRegs = TMR_GetCounterRegs(TimerID);

    if (pTmrCounterRegs != NULL) {
        pTmrCounterRegs->Reload = Val;
    }
}

/**
 *  AmbaCSL_TmrSetMatch0Val - Set timer first match value
 *  @param[in] TimerID Timer ID
 *  @param[in] Val The 1st match value
 */
void AmbaCSL_TmrSetMatch0Val(UINT32 TimerID, UINT32 Val)
{
    AMBA_TMR_COUNTER_REG_s *pTmrCounterRegs = TMR_GetCounterRegs(TimerID);

    if (pTmrCounterRegs != NULL) {
        pTmrCounterRegs->FirstMatch = Val;
    }
}

/**
 *  AmbaCSL_TmrSetMatch1Val - Set timer second match value
 *  @param[in] TimerID Timer ID
 *  @param[in] Val The 2nd match value
 */
void AmbaCSL_TmrSetMatch1Val(UINT32 TimerID, UINT32 Val)
{
    AMBA_TMR_COUNTER_REG_s *pTmrCounterRegs = TMR_GetCounterRegs(TimerID);

    if (pTmrCounterRegs != NULL) {
        pTmrCounterRegs->SecondMatch = Val;
    }
}

/**
 *  AmbaCSL_TmrGetEnable - Get Timer Enable/Disable state
 *  @param[in] TimerID Timer ID
 *  @return 1 if enabled, otherwise 0
 */
UINT32 AmbaCSL_TmrGetEnable(UINT32 TimerID)
{
    UINT32 TimerCtrl = 0U;
    UINT32 BitOffset = 0U;

    if (TimerID < AMBA_TIMER8) {
        TimerCtrl = pAmbaTMR_Regs[0]->TimerCtrl0;
        BitOffset = (TimerID << 2U);
    } else if (TimerID < AMBA_TIMER10) {
        TimerCtrl = pAmbaTMR_Regs[0]->TimerCtrl1;
        BitOffset = ((TimerID - 8U) << 2U);
    } else if (TimerID < AMBA_TIMER18) {
        TimerCtrl = pAmbaTMR_Regs[1]->TimerCtrl0;
        BitOffset = ((TimerID - 10U) << 2U);
    } else if (TimerID < AMBA_NUM_TIMER) {
        TimerCtrl = pAmbaTMR_Regs[1]->TimerCtrl1;
        BitOffset = ((TimerID - 18U) << 2U);
    } else {
        /* Shall not happen!! */
    }

    return ((TimerCtrl >> BitOffset) & 0x1U);
}

/**
 *  AmbaCSL_TmrGetCurrentVal - Get timer current value
 *  @param[in] TimerID Timer ID
 *  @return Timer current value
 */
UINT32 AmbaCSL_TmrGetCurrentVal(UINT32 TimerID)
{
    const AMBA_TMR_COUNTER_REG_s *pTmrCounterRegs = TMR_GetCounterRegs(TimerID);
    UINT32 Val = 0U;

    if (pTmrCounterRegs != NULL) {
        Val = pTmrCounterRegs->Status;
    }

    return Val;
}

/**
 *  AmbaCSL_TmrGetReloadVal - Get timer reload value
 *  @param[in] TimerID Timer ID
 *  @return Timer reload value
 */
UINT32 AmbaCSL_TmrGetReloadVal(UINT32 TimerID)
{
    const AMBA_TMR_COUNTER_REG_s *pTmrCounterRegs = TMR_GetCounterRegs(TimerID);
    UINT32 Val = 0U;

    if (pTmrCounterRegs != NULL) {
        Val = pTmrCounterRegs->Reload;
    }

    return Val;
}
