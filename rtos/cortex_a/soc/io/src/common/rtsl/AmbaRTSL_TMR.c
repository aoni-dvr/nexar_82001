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
#include "AmbaMisraFix.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_TMR.h"
#include "AmbaCSL_TMR.h"

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define AMBA_NUM_AVAIL_TIMER AMBA_R52_TIMER_OFFSET
#else
#define AMBA_NUM_AVAIL_TIMER AMBA_NUM_TIMER
#endif

typedef struct {
    UINT32  TimerFreq;
    UINT32  NumPeriodicTick;
    UINT32  NumRefClkTick;          /* Number of reference clock cycles to form a actual timer tick */
    UINT32  ExpireCount;
    UINT32  UserIntFuncArg;
    AMBA_TMR_ISR_f  UserIntFunc;    /* The table of line ISRs */
} AMBA_TMR_CTRL_s;

typedef struct {
    UINT32  IntID;                  /* Interrupt ID */
    AMBA_INT_ISR_f pISR;            /* pointer to the group ISR */
} AMBA_TMR_SYS_CTRL_s;

static void TMR_ISR0(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR1(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR2(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR3(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR4(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR5(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR6(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR7(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR8(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR9(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR10(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR11(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR12(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR13(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR14(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR15(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR16(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR17(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR18(UINT32 IntID, UINT32 IsrArg);
static void TMR_ISR19(UINT32 IntID, UINT32 IsrArg);

static AMBA_TMR_SYS_CTRL_s AmbaTmrSysCtrl[AMBA_NUM_TIMER] = {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    { .IntID = AMBA_INT_SPI_ID054_TIMER0,   .pISR = TMR_ISR0},
    { .IntID = AMBA_INT_SPI_ID055_TIMER1,   .pISR = TMR_ISR1},
    { .IntID = AMBA_INT_SPI_ID056_TIMER2,   .pISR = TMR_ISR2},
    { .IntID = AMBA_INT_SPI_ID057_TIMER3,   .pISR = TMR_ISR3},
    { .IntID = AMBA_INT_SPI_ID058_TIMER4,   .pISR = TMR_ISR4},
    { .IntID = AMBA_INT_SPI_ID059_TIMER5,   .pISR = TMR_ISR5},
    { .IntID = AMBA_INT_SPI_ID060_TIMER6,   .pISR = TMR_ISR6},
    { .IntID = AMBA_INT_SPI_ID061_TIMER7,   .pISR = TMR_ISR7},
    { .IntID = AMBA_INT_SPI_ID062_TIMER8,   .pISR = TMR_ISR8},
    { .IntID = AMBA_INT_SPI_ID063_TIMER9,   .pISR = TMR_ISR9},
    { .IntID = AMBA_INT_SPI_ID064_TIMER10,  .pISR = TMR_ISR10},
    { .IntID = AMBA_INT_SPI_ID065_TIMER11,  .pISR = TMR_ISR11},
    { .IntID = AMBA_INT_SPI_ID066_TIMER12,  .pISR = TMR_ISR12},
    { .IntID = AMBA_INT_SPI_ID067_TIMER13,  .pISR = TMR_ISR13},
    { .IntID = AMBA_INT_SPI_ID068_TIMER14,  .pISR = TMR_ISR14},
    { .IntID = AMBA_INT_SPI_ID069_TIMER15,  .pISR = TMR_ISR15},
    { .IntID = AMBA_INT_SPI_ID070_TIMER16,  .pISR = TMR_ISR16},
    { .IntID = AMBA_INT_SPI_ID071_TIMER17,  .pISR = TMR_ISR17},
    { .IntID = AMBA_INT_SPI_ID072_TIMER18,  .pISR = TMR_ISR18},
    { .IntID = AMBA_INT_SPI_ID073_TIMER19,  .pISR = TMR_ISR19},
#else
    { .IntID = AMBA_INT_SPI_ID54_TIMER0,   .pISR = TMR_ISR0},
    { .IntID = AMBA_INT_SPI_ID55_TIMER1,   .pISR = TMR_ISR1},
    { .IntID = AMBA_INT_SPI_ID56_TIMER2,   .pISR = TMR_ISR2},
    { .IntID = AMBA_INT_SPI_ID57_TIMER3,   .pISR = TMR_ISR3},
    { .IntID = AMBA_INT_SPI_ID58_TIMER4,   .pISR = TMR_ISR4},
    { .IntID = AMBA_INT_SPI_ID59_TIMER5,   .pISR = TMR_ISR5},
    { .IntID = AMBA_INT_SPI_ID60_TIMER6,   .pISR = TMR_ISR6},
    { .IntID = AMBA_INT_SPI_ID61_TIMER7,   .pISR = TMR_ISR7},
    { .IntID = AMBA_INT_SPI_ID62_TIMER8,   .pISR = TMR_ISR8},
    { .IntID = AMBA_INT_SPI_ID63_TIMER9,   .pISR = TMR_ISR9},
    { .IntID = AMBA_INT_SPI_ID64_TIMER10,  .pISR = TMR_ISR10},
    { .IntID = AMBA_INT_SPI_ID65_TIMER11,  .pISR = TMR_ISR11},
    { .IntID = AMBA_INT_SPI_ID66_TIMER12,  .pISR = TMR_ISR12},
    { .IntID = AMBA_INT_SPI_ID67_TIMER13,  .pISR = TMR_ISR13},
    { .IntID = AMBA_INT_SPI_ID68_TIMER14,  .pISR = TMR_ISR14},
    { .IntID = AMBA_INT_SPI_ID69_TIMER15,  .pISR = TMR_ISR15},
    { .IntID = AMBA_INT_SPI_ID70_TIMER16,  .pISR = TMR_ISR16},
    { .IntID = AMBA_INT_SPI_ID71_TIMER17,  .pISR = TMR_ISR17},
    { .IntID = AMBA_INT_SPI_ID72_TIMER18,  .pISR = TMR_ISR18},
    { .IntID = AMBA_INT_SPI_ID73_TIMER19,  .pISR = TMR_ISR19},
#endif
};

static AMBA_TMR_CTRL_s AmbaTmrCtrl[AMBA_NUM_TIMER];

#pragma GCC push_options
#pragma GCC target("general-regs-only")
/**
 *  TMR_IntHandler - ISR for all Timers
 *  @param[in] TimerID Timer ID
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_IntHandler(UINT32 TimerID, UINT32 IntID, UINT32 IsrArg)
{
    AMBA_TMR_CTRL_s *pTimer = &AmbaTmrCtrl[TimerID];

    if (TimerID < AMBA_NUM_AVAIL_TIMER) {
        pTimer->ExpireCount++;

        if (pTimer->UserIntFunc != NULL) {
            pTimer->UserIntFunc(TimerID, pTimer->UserIntFuncArg);
        }

        /* release the timer if it is stopped */
        if (AmbaCSL_TmrGetReloadVal(TimerID) == 0U) {
            AmbaCSL_TmrSetEnable(TimerID, 0U);
        }
    } else {
        AmbaMisra_TouchUnused(&IntID);
        AmbaMisra_TouchUnused(&IsrArg);
    }
}

/**
 *  Timer_ISR0 - Interrupt handler for Timer 0
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR0(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER0, IntID, IsrArg);
}

/**
 *  Timer_ISR1 - Interrupt handler for Timer 1.
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR1(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER1, IntID, IsrArg);
}

/**
 *  Timer_ISR2 - Interrupt handler for Timer 2
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR2(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER2, IntID, IsrArg);
}

/**
 *  Timer_ISR3 - Interrupt handler for Timer 3
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR3(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER3, IntID, IsrArg);
}

/**
 *  Timer_ISR4 - Interrupt handler for Timer 4
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR4(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER4, IntID, IsrArg);
}

/**
 *  Timer_ISR5 - Interrupt handler for Timer 5
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR5(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER5, IntID, IsrArg);
}

/**
 *  Timer_ISR6 - Interrupt handler for Timer 6
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR6(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER6, IntID, IsrArg);
}

/**
 *  Timer_ISR7 - Interrupt handler for Timer 7
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR7(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER7, IntID, IsrArg);
}

/**
 *  Timer_ISR8 - Interrupt handler for Timer 8
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR8(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER8, IntID, IsrArg);
}

/**
 *  Timer_ISR9 - Interrupt handler for Timer 9
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR9(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER9, IntID, IsrArg);
}

/**
 *  Timer_ISR10 - Interrupt handler for Timer 10
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR10(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER10, IntID, IsrArg);
}

/**
 *  Timer_ISR11 - Interrupt handler for Timer 11
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR11(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER11, IntID, IsrArg);
}

/**
 *  Timer_ISR12 - Interrupt handler for Timer 12
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR12(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER12, IntID, IsrArg);
}

/**
 *  Timer_ISR13 - Interrupt handler for Timer 13
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR13(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER13, IntID, IsrArg);
}

/**
 *  Timer_ISR14 - Interrupt handler for Timer 14
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR14(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER14, IntID, IsrArg);
}

/**
 *  Timer_ISR15 - Interrupt handler for Timer 15
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR15(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER15, IntID, IsrArg);
}

/**
 *  Timer_ISR16 - Interrupt handler for Timer 16
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR16(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER16, IntID, IsrArg);
}

/**
 *  Timer_ISR17 - Interrupt handler for Timer 17
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR17(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER17, IntID, IsrArg);
}

/**
 *  Timer_ISR18 - Interrupt handler for Timer 18
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR18(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER18, IntID, IsrArg);
}

/**
 *  Timer_ISR19 - Interrupt handler for Timer 19: Reserved for OS ticks.
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
static void TMR_ISR19(UINT32 IntID, UINT32 IsrArg)
{
    TMR_IntHandler(AMBA_TIMER19, IntID, IsrArg);
}
#pragma GCC pop_options

/**
 *  AmbaRTSL_TmrInit - Initialize the timers
 */
void AmbaRTSL_TmrInit(void)
{
    AMBA_INT_CONFIG_s IntConfig = {
        .TriggerType = INT_TRIG_RISING_EDGE,
        .IrqType = INT_TYPE_IRQ,
        .CpuTargets = 0x1U
    };
    UINT32 i, IntID;

    /* Program default values into timer registers */
    for (i = 0U; i < AMBA_NUM_AVAIL_TIMER; i++)  {
        AmbaCSL_TmrSetEnable(i, 0U);
        AmbaCSL_TmrSetConfig(i, AMBA_TMR_CLK_APB, AMBA_TMR_INT_ENABLE);

        /* Program default values into timer registers */
        AmbaCSL_TmrSetCurrentVal(i, 0xffffffffU);
        AmbaCSL_TmrSetReloadVal(i, 0x0U);
        AmbaCSL_TmrSetMatch0Val(i, 0x0U);
        AmbaCSL_TmrSetMatch1Val(i, 0x0U);

        IntID = AmbaTmrSysCtrl[i].IntID;
        (void)AmbaRTSL_GicIntDisable(IntID);
        (void)AmbaRTSL_GicIntConfig(IntID, &IntConfig, AmbaTmrSysCtrl[i].pISR, 0U);
    }
}

/**
 *  AmbaRTSL_TmrHookTimeOutHandler - Hook timer expiration function
 *  @param[in] TimerID Timer ID
 *  @param[in] ExpireFunc Timer expiration function
 *  @param[in] ExpireFuncArg Optional argument attached to timer expiration function
 *  @return error code
 */
UINT32 AmbaRTSL_TmrHookTimeOutHandler(UINT32 TimerID, AMBA_TMR_ISR_f ExpireFunc, UINT32 ExpireFuncArg)
{
    UINT32 RetVal = TMR_ERR_NONE;
    AMBA_TMR_CTRL_s *pTimer = &AmbaTmrCtrl[TimerID];

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer->UserIntFunc = ExpireFunc;
        pTimer->UserIntFuncArg = ExpireFuncArg;

        if (ExpireFunc != NULL) {
            (void)AmbaRTSL_GicIntEnable(AmbaTmrSysCtrl[TimerID].IntID);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrConfig - Configure a timer
 *  @param[in] TimerID Timer ID
 *  @param[in] TimerFreq Desired timer frequency
 *  @param[in] NumPeriodicTick Interval between two timer expiration (in number of ticks)
 *  @return error code
 */
UINT32 AmbaRTSL_TmrConfig(UINT32 TimerID, UINT32 TimerFreq, UINT32 NumPeriodicTick)
{
    UINT32 RetVal = TMR_ERR_NONE;
    AMBA_TMR_CTRL_s *pTimer = &AmbaTmrCtrl[TimerID];
    UINT32 RefFreq;

    if ((TimerID >= AMBA_NUM_AVAIL_TIMER) || (TimerFreq == 0U)) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer->TimerFreq = TimerFreq;
        pTimer->NumPeriodicTick = NumPeriodicTick;

        RefFreq = AmbaRTSL_PllGetApbClk();
        pTimer->NumRefClkTick = RefFreq / TimerFreq;

        if (NumPeriodicTick == 0U) {
            /* NOTE: For cyclic timer, interrupt could be ignored because we can have continuous timestamp values by its tick count. */
            /* NOTE: For non-cyclic timer, interrupt is necessary because its tick count is only meaningful before it reaches zero. */
            (void)AmbaRTSL_GicIntEnable(AmbaTmrSysCtrl[TimerID].IntID);
            AmbaCSL_TmrSetReloadVal(TimerID, 0U);
        } else {
            if (NumPeriodicTick > (0xffffffffU / pTimer->NumRefClkTick)) {
                /* hardware counter width is 32-bit */
                RetVal = TMR_ERR_NA;
            } else {
                /* NOTE: If RefFreq is not a multiple of TimerFreq, the accuracy of the timer will be low. */
                AmbaCSL_TmrSetReloadVal(TimerID, (NumPeriodicTick * pTimer->NumRefClkTick) - 1U);
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrStart - Start a timer
 *  @param[in] TimerID Timer ID
 *  @param[in] NumRemainTick Interval before the next timer expiration (in number of ticks)
 *  @return error code
 */
UINT32 AmbaRTSL_TmrStart(UINT32 TimerID, UINT32 NumRemainTick)
{
    UINT32 RetVal = TMR_ERR_NONE;
    AMBA_TMR_CTRL_s *pTimer;

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer = &AmbaTmrCtrl[TimerID];
        pTimer->ExpireCount = 0U;

        if (NumRemainTick > (0xffffffffU / pTimer->NumRefClkTick)) {
            /* hardware counter width is 32-bit */
            RetVal = TMR_ERR_NA;
        } else {
            /* NOTE: If RefFreq is not a multiple of TimerFreq, the accuracy of the timer will be low. */
            AmbaCSL_TmrSetCurrentVal(TimerID, (NumRemainTick * pTimer->NumRefClkTick));
            AmbaCSL_TmrSetEnable(TimerID, 1U);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrStop - Stop a timer
 *  @param[in] TimerID Timer ID
 *  @return error code
 */
UINT32 AmbaRTSL_TmrStop(UINT32 TimerID)
{
    UINT32 RetVal = TMR_ERR_NONE;

    if (TimerID >= AMBA_NUM_AVAIL_TIMER) {
        RetVal = TMR_ERR_ARG;
    } else {
        (void)AmbaRTSL_GicIntDisable(AmbaTmrSysCtrl[TimerID].IntID);
        AmbaCSL_TmrSetEnable(TimerID, 0U);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrShowTickCount - Show the current tick count value
 *  @param[in] TimerID Timer ID
 *  @param[out] pTimeLeft Number of the remaining ticks before expiration
 *  @return error code
 */
UINT32 AmbaRTSL_TmrShowTickCount(UINT32 TimerID, UINT32 * pTimeLeft)
{
    UINT32 RetVal = TMR_ERR_NONE;
    const AMBA_TMR_CTRL_s *pTimer;

    if ((TimerID >= AMBA_NUM_AVAIL_TIMER) || (pTimeLeft == NULL)) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer = &AmbaTmrCtrl[TimerID];

        /* Don't do rounding otherwise it might cause external clock ticking inversion */
        *pTimeLeft = AmbaCSL_TmrGetCurrentVal(TimerID) / pTimer->NumRefClkTick;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrGetInfo - Get timer information
 *  @param[in] TimerID Timer ID
 *  @param[out] pTimerInfo Timer information
 *  @return error code
 */
UINT32 AmbaRTSL_TmrGetInfo(UINT32 TimerID, AMBA_TMR_INFO_s * pTimerInfo)
{
    UINT32 RetVal = TMR_ERR_NONE;
    const AMBA_TMR_CTRL_s *pTimer;

    if ((TimerID >= AMBA_NUM_AVAIL_TIMER) || (pTimerInfo == NULL)) {
        RetVal = TMR_ERR_ARG;
    } else {
        pTimer = &AmbaTmrCtrl[TimerID];

        /* Assume the reference clock is from gclk_apb */
        /* TODO: external clock source */
        pTimerInfo->SysFreq = AmbaRTSL_PllGetApbClk();
        pTimerInfo->TimerFreq = pTimer->TimerFreq;
        pTimerInfo->PeriodicInterval = pTimer->NumPeriodicTick;
        pTimerInfo->ExpireCount = pTimer->ExpireCount;
        pTimerInfo->State = AmbaCSL_TmrGetEnable(TimerID);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_TmrCoreFreqChg - Reconfigure timer based on the new clock frequency
 */
void AmbaRTSL_TmrCoreFreqChg(void)
{
#if !defined(CONFIG_LINUX)
    const AMBA_TMR_SYS_CTRL_s *pTimerCtrl;
    AMBA_TMR_CTRL_s *pTimer;
    AMBA_INT_INFO_s IntInfo;
    UINT32 RefFreq;
    UINT32 i;

    for (i = 0U; i < AMBA_NUM_AVAIL_TIMER; i++) {
        pTimerCtrl = &AmbaTmrSysCtrl[i];
        (void)AmbaRTSL_GicGetIntInfo(pTimerCtrl->IntID, &IntInfo);
        (void)AmbaRTSL_GicIntDisable(pTimerCtrl->IntID);
        AmbaCSL_TmrSetEnable(i, 0U);

        pTimer = &AmbaTmrCtrl[i];
        if (pTimer->NumPeriodicTick != 0U) {
            if (IntInfo.IrqEnable != 0U) {
                (void)AmbaRTSL_GicIntEnable(pTimerCtrl->IntID);
            }

            RefFreq = AmbaRTSL_PllGetApbClk();

            pTimer->NumRefClkTick = RefFreq / pTimer->TimerFreq;
            /* NOTE: If RefFreq is not a multiple of TimerFreq, the accuracy of the timer will be low. */
            AmbaCSL_TmrSetReloadVal(i, (pTimer->NumPeriodicTick * pTimer->NumRefClkTick) - 1U);
            AmbaCSL_TmrSetCurrentVal(i, AmbaCSL_TmrGetCurrentVal(i) * pTimer->NumRefClkTick);
            AmbaCSL_TmrSetEnable(i, 1U);
        }
    }
#endif
}

/**
 *  AmbaRTSL_TmrGetOsTimerId - Get timer id reserved for RTOS
 *  @param[out] pTimerID Timer ID
 *  @return error code
 */
UINT32 AmbaRTSL_TmrGetOsTimerId(UINT32 *pTimerID)
{
    UINT32 RetVal = TMR_ERR_NONE;

    if (pTimerID == NULL) {
        RetVal = TMR_ERR_ARG;
    } else {
        *pTimerID = AMBA_TIMER_SYS_TICK;
    }

    return RetVal;
}
