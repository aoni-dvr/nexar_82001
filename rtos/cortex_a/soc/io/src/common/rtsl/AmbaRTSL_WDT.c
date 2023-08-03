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

#ifdef CONFIG_QNX
#include <fcntl.h>
#include <hw/ambarella_clk.h>
#include "Amba_wdt.h"
#else
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_PLL.h"
#endif
#include "AmbaRTSL_WDT.h"
#include "AmbaCSL_WDT.h"
#include "AmbaCSL_RCT.h"

#ifndef CONFIG_QNX
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif
#endif


#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static UINT32 AmbaWdtTimeOutVal[AMBA_WDT_CH_NUM];
static AMBA_WDT_ISR_f AmbaWdtTimeOutFunc[AMBA_WDT_CH_NUM];
static UINT32 AmbaWdtTimeOutFuncArg[AMBA_WDT_CH_NUM];
static UINT32 AmbaWdtResetStatus;
#else
static UINT32 AmbaWdtTimeOutVal;
static AMBA_WDT_ISR_f AmbaWdtTimeOutFunc;
static UINT32 AmbaWdtTimeOutFuncArg;
#if defined(CONFIG_SOC_CV2)
static UINT32 AmbaWdtResetStatus;
#endif
#endif

#ifndef CONFIG_QNX
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
AMBA_WDT_REG_s * pAmbaWDT_Reg[AMBA_WDT_CH_NUM];

void AmbaRTSL_Wdt_Init(void)
{
    ULONG base_addr;
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    UINT32 i;
#endif
    base_addr = AMBA_CA53_WDT_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaWDT_Reg[0], &base_addr);
    base_addr = AMBA_CA53_WDT_1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaWDT_Reg[1], &base_addr);
    base_addr = AMBA_CA53_WDT_2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaWDT_Reg[2], &base_addr);
    base_addr = AMBA_CA53_WDT_3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaWDT_Reg[3], &base_addr);
    base_addr = AMBA_CA53_WDT_4_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaWDT_Reg[4], &base_addr);
    base_addr = AMBA_CA53_WDT_5_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaWDT_Reg[5], &base_addr);

    AmbaWdtResetStatus = AmbaCSL_RctGetWdtState();
    if (AmbaWdtResetStatus == 0x3fU) {
        AmbaWdtResetStatus = 0U;
        if (AmbaCSL_RctGetWdtPattern() == AMBA_RCT_RESET_PATTERN) {
            AmbaWdtResetStatus = 0x1U;
        }
    } else {
        AmbaWdtResetStatus ^= 0x3fU;
    }
    AmbaCSL_WdtClearWdtResetStatus();
    AmbaCSL_RctClrWdtPattern();
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    for (i = 0U; i < AMBA_WDT_CH_NUM; i++) {
        base_addr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,wdt", "reg", i * 2U);
        if ( base_addr != 0UL ) {
            AmbaMisra_TypeCast(&pAmbaWDT_Reg[i], &base_addr);
        }
    }
#endif
}
#else
AMBA_WDT_REG_s * pAmbaWDT_Reg;

void AmbaRTSL_Wdt_Init(void)
{
    ULONG base_addr;
    INT32 offset = 0;
    (void) offset;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_WDT_BASE_ADDR;
#else
    base_addr = AMBA_CORTEX_A53_WDT_BASE_ADDR;
#endif
    AmbaMisra_TypeCast(&pAmbaWDT_Reg, &base_addr);

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    base_addr = IO_UtilityFDTPropertyU32Quick(offset, "ambarella,wdt", "reg", 0U);
    if ( base_addr != 0U ) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
        base_addr = base_addr | AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR;
#endif
        AmbaMisra_TypeCast(&pAmbaWDT_Reg, &base_addr);
    }
#endif
#if defined(CONFIG_SOC_CV2)
    AmbaWdtResetStatus = AmbaCSL_RctGetWdtState();
    if (AmbaWdtResetStatus == 0x1U) {
        AmbaWdtResetStatus = 0U;
    } else {
        AmbaWdtResetStatus ^= 0x1U;
    }
    AmbaCSL_WdtClearWdtResetStatus();
#endif
}
#endif //CONFIG_SOC_CV2FS
#else

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
void AmbaRTSL_Wdt_Init(void)
{
    AmbaWdtResetStatus = AmbaCSL_RctGetWdtState();
    if (AmbaWdtResetStatus == 0x3fU) {
        AmbaWdtResetStatus = 0U;
        if (AmbaCSL_RctGetWdtPattern() == AMBA_RCT_RESET_PATTERN) {
            AmbaWdtResetStatus = 0x1U;
        }
    } else {
        AmbaWdtResetStatus ^= 0x3fU;
    }
    AmbaCSL_WdtClearWdtResetStatus();
    AmbaCSL_RctClrWdtPattern();
}
#else
void AmbaRTSL_Wdt_Init(void)
{
    AmbaWdtResetStatus = AmbaCSL_RctGetWdtState();
    if (AmbaWdtResetStatus == 0x1U) {
        AmbaWdtResetStatus = 0U;
    } else {
        AmbaWdtResetStatus ^= 0x1U;
    }
    AmbaCSL_WdtClearWdtResetStatus();
}
#endif
#endif

/**
 *  WDT_ISR - ISR for watchdog timer timeout
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument attached to the ISR
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
#ifdef CONFIG_QNX
static void *WDT_ISR(void* argv)
{
    struct sigevent event;
    int iid;

    (void) argv;
    SIGEV_INTR_INIT(&event);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    iid = InterruptAttachEvent(84, &event, 0U);
    InterruptUnmask(84, iid);

    InterruptWait(0, NULL);
    if (AmbaWdtTimeOutFunc[0U] != NULL) {
        AmbaWdtTimeOutFunc[0U](AmbaWdtTimeOutFuncArg[0U]);
    }
#else
    iid = InterruptAttachEvent(74, &event, 0U);
    InterruptUnmask(74, iid);

    InterruptWait(0, NULL);
    if (AmbaWdtTimeOutFunc != NULL) {
        AmbaWdtTimeOutFunc(AmbaWdtTimeOutFuncArg);
    }
#endif

    pthread_exit(NULL);
}
#else
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
static void WDT_ISR(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);

    if (AmbaWdtTimeOutFunc[0U] != NULL) {
        AmbaWdtTimeOutFunc[0U](IsrArg);
    }
}

static void WDT_ISR1(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);

    if (AmbaWdtTimeOutFunc[1U] != NULL) {
        AmbaWdtTimeOutFunc[1U](IsrArg);
    }
}

static void WDT_ISR2(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);

    if (AmbaWdtTimeOutFunc[2U] != NULL) {
        AmbaWdtTimeOutFunc[2U](IsrArg);
    }
}

static void WDT_ISR3(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);

    if (AmbaWdtTimeOutFunc[3U] != NULL) {
        AmbaWdtTimeOutFunc[3U](IsrArg);
    }
}

static void WDT_ISR4(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);

    if (AmbaWdtTimeOutFunc[4U] != NULL) {
        AmbaWdtTimeOutFunc[4U](IsrArg);
    }
}

static void WDT_ISR5(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);

    if (AmbaWdtTimeOutFunc[5U] != NULL) {
        AmbaWdtTimeOutFunc[5U](IsrArg);
    }
}
#else
static void WDT_ISR(UINT32 IntID, UINT32 IsrArg)
{
    AmbaMisra_TouchUnused(&IntID);

    if (AmbaWdtTimeOutFunc != NULL) {
        AmbaWdtTimeOutFunc(IsrArg);
    }
}
#endif // CONFIG_SOC_CV2FS
#endif
#pragma GCC pop_options

/**
 *  WDT_SetCountDownVal - Set WDT countdown value
 *  @param[in] CountDown  Timer countdown value (in APB bus clk)
 */
static void WDT_SetCountDownVal(UINT32 ChId, UINT32 CountDown)
{
#ifdef CONFIG_QNX
    int fd, err;
    clk_freq_t clk_freq;
    UINT32 WdtClkFreq = 0U;

    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager. Use default clocks", __FUNCTION__);
    } else {
        clk_freq.id = AMBA_CLK_APB;

        /* Get UART0 control clock */
        err = devctl(fd, DCMD_CLOCK_GET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
        if (err) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: GET devctl failed: %s", __FUNCTION__, strerror(errno));
        }

        if (clk_freq.freq == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: /dev/clock: Invalid frequency (-1)", __FUNCTION__);
        } else {
            /* Set real frequency */
            WdtClkFreq = clk_freq.freq;
        }

        close(fd);
    }
#else
    UINT32 WdtClkFreq = AmbaRTSL_PllGetApbClk();
#endif

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaCSL_WdtSetReload(pAmbaWDT_Reg[ChId], CountDown * (WdtClkFreq / 1000U));
    AmbaCSL_WdtSetRestart(pAmbaWDT_Reg[ChId]);
#else
    (void) ChId;
    AmbaCSL_WdtSetReload(CountDown * (WdtClkFreq / 1000U));
    AmbaCSL_WdtSetRestart();
#endif
}

/**
 *  AmbaRTSL_WdtHookTimeOutHandler - The function registers an interrupt handler on watchdog timer expiry.
 *  @param[in] ExpireFunc WDT timeout handler
 *  @param[in] ExpireFuncArg Optional argument attached to WDT timeout handler
 *  @return error code
 */
UINT32 AmbaRTSL_WdtHookTimeOutHandler(AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg)
{
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaWdtTimeOutFunc[0U] = ExpireFunc;
    AmbaWdtTimeOutFuncArg[0U] = ExpireFuncArg;
#else
    AmbaWdtTimeOutFunc = ExpireFunc;
    AmbaWdtTimeOutFuncArg = ExpireFuncArg;
#endif

    return WDT_ERR_NONE;
}


#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/**
 *  AmbaRTSL_WdtHookHandler - The function registers an interrupt handler on watchdog timer expiry.
 *  @param[in] ChId WDT channel id
 *  @param[in] ExpireFunc WDT timeout handler
 *  @param[in] ExpireFuncArg Optional argument attached to WDT timeout handler
 *  @return error code
 */
UINT32 AmbaRTSL_WdtHookHandler(UINT32 ChId, AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg)
{
    AmbaWdtTimeOutFunc[ChId] = ExpireFunc;
    AmbaWdtTimeOutFuncArg[ChId] = ExpireFuncArg;

    return WDT_ERR_NONE;
}

#endif

/**
 *  AmbaRTSL_WdtStart - Start the WDT.
 *  @param[in] CountDown Timer countdown value
 *  @param[in] ResetIrqPulseWidth Pulse width on WDT timeout
 *  @return error code
 */
UINT32 AmbaRTSL_WdtStart(UINT32 CountDown, UINT32 ResetIrqPulseWidth)
{
#if defined(CONFIG_QNX)
    UINT32 ExpireAction;

    /* Disable watch dog timer first */
    (void)AmbaRTSL_WdtStop();

    #if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    if (AmbaWdtTimeOutFunc[0U] != NULL) {
    #else
    if (AmbaWdtTimeOutFunc != NULL) {
    #endif
        ExpireAction = WDT_ACT_IRQ;
        pthread_create(NULL, NULL, WDT_ISR, 0);
    } else {
        ExpireAction = WDT_ACT_SYS_RESET;
        #if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        (void)InterruptMask(84, -1);
        #else
        (void)InterruptMask(74, -1);
        #endif
    }
#else /* CONFIG_QNX */

    #if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    UINT32 IntID = AMBA_INT_SPI_ID084_WDT;
    #elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 IntID = AMBA_INT_SPI_ID75_WATCHDOG_TIMER;
    #else
    UINT32 IntID = AMBA_INT_SPI_ID74_WATCHDOG_TIMER;
    #endif
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 ExpireAction;

    /* Disable watch dog timer first */
    (void)AmbaRTSL_WdtStop();

    #if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    ExpireAction = WDT_ACT_EXT;
    if (AmbaWdtTimeOutFunc[0U] != NULL) {
    #else
    
    #if defined(CONFIG_SOC_CV5) && !defined(CONFIG_WDT_PIN_NON)
    ExpireAction = WDT_ACT_EXT;
    #else
    ExpireAction = WDT_ACT_SYS_RESET;
    #endif

    if (AmbaWdtTimeOutFunc != NULL) {
    #endif
        ExpireAction = WDT_ACT_IRQ;

        /* Hook WDT ISR */
        IntConfig.TriggerType = INT_TRIG_RISING_EDGE;
        IntConfig.IrqType = INT_TYPE_FIQ;
        IntConfig.CpuTargets = 1U;
        #if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        (void)AmbaRTSL_GicIntConfig(IntID, &IntConfig, WDT_ISR, AmbaWdtTimeOutFuncArg[0]);
        #else
        (void)AmbaRTSL_GicIntConfig(IntID, &IntConfig, WDT_ISR, AmbaWdtTimeOutFuncArg);
        #endif
        (void)AmbaRTSL_GicIntEnable(IntID);
    }
#endif /* CONFIG_QNX */

    #if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaWdtTimeOutVal[0] = CountDown;
    WDT_SetCountDownVal(0U, CountDown);
    AmbaCSL_WdtSetResetWidth(pAmbaWDT_Reg[0], 0xFFFFFFFFU);
    AmbaCSL_WdtSetTimeOutAction(ExpireAction);
    #else
    AmbaWdtTimeOutVal = CountDown;
    WDT_SetCountDownVal(0U, CountDown);
    AmbaCSL_WdtSetResetWidth(0xFFFFFFFFU);
    AmbaCSL_WdtSetTimeOutAction(ExpireAction);
    #endif

    if (0U == ResetIrqPulseWidth) {
        /* misra-c*/
    }
    return WDT_ERR_NONE;
}


#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/**
 *  AmbaRTSL_WdtStartByChId - Start the specified WDT.
 *  @param[in] ChId WDT channel id
 *  @param[in] CountDown Timer countdown value
 *  @param[in] ResetIrqPulseWidth Pulse width on WDT timeout
 *  @return error code
 */
UINT32 AmbaRTSL_WdtStartByChId(UINT32 ChId, UINT32 CountDown, UINT32 ResetIrqPulseWidth)
{
#ifndef CONFIG_QNX
    UINT32 IntID[AMBA_WDT_CH_NUM];
    AMBA_INT_ISR_f pISR[AMBA_WDT_CH_NUM];
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 ExpireAction;

    IntID[0] = AMBA_INT_SPI_ID084_WDT;
    IntID[1] = AMBA_INT_SPI_ID169_WDT_1;
    IntID[2] = AMBA_INT_SPI_ID170_WDT_2;
    IntID[3] = AMBA_INT_SPI_ID171_WDT_3;
    IntID[4] = AMBA_INT_SPI_ID172_WDT_4;
    IntID[5] = AMBA_INT_SPI_ID168_WDT_5;

    pISR[0] = WDT_ISR;
    pISR[1] = WDT_ISR1;
    pISR[2] = WDT_ISR2;
    pISR[3] = WDT_ISR3;
    pISR[4] = WDT_ISR4;
    pISR[5] = WDT_ISR5;

    /* Disable watch dog timer first */
    (void)AmbaRTSL_WdtStopByChId(ChId);

    if (AmbaWdtTimeOutFunc[ChId] != NULL) {
        ExpireAction = WDT_ACT_IRQ;

        /* Hook WDT ISR */
        IntConfig.TriggerType = INT_TRIG_RISING_EDGE;
        IntConfig.IrqType = INT_TYPE_FIQ;
        IntConfig.CpuTargets = 1U;
        (void)AmbaRTSL_GicIntConfig(IntID[ChId], &IntConfig, pISR[ChId], AmbaWdtTimeOutFuncArg[ChId]);
        (void)AmbaRTSL_GicIntEnable(IntID[ChId]);
    } else {
        ExpireAction = WDT_ACT_EXT;
        (void)AmbaRTSL_GicIntDisable(IntID[ChId]);
    }
    AmbaWdtTimeOutVal[ChId] = CountDown;

    AmbaCSL_WdtSetReload(pAmbaWDT_Reg[ChId], CountDown * (AmbaRTSL_PllGetApbClk() / 1000U));
    AmbaCSL_WdtSetRestart(pAmbaWDT_Reg[ChId]);
    if (ExpireAction == WDT_ACT_EXT) {
        AmbaCSL_WdtSetResetWidth(pAmbaWDT_Reg[ChId], 0x1000);
    } else {
        AmbaCSL_WdtSetResetWidth(pAmbaWDT_Reg[ChId], ResetIrqPulseWidth);
    }
    AmbaCSL_WdtSetAction(ChId, ExpireAction);
#else
    UINT32 ExpireAction;

    /* Disable watch dog timer first */
    (void)AmbaRTSL_WdtStopByChId(ChId);

    if (AmbaWdtTimeOutFunc[ChId] != NULL) {
        ExpireAction = WDT_ACT_IRQ;
    } else {
        ExpireAction = WDT_ACT_EXT;
    }
    AmbaWdtTimeOutVal[ChId] = CountDown;

    WDT_SetCountDownVal(ChId, CountDown);
    if (ExpireAction == WDT_ACT_EXT) {
        AmbaCSL_WdtSetResetWidth(pAmbaWDT_Reg[ChId], 0x1000);
    } else {
        AmbaCSL_WdtSetResetWidth(pAmbaWDT_Reg[ChId], ResetIrqPulseWidth);
    }
    AmbaCSL_WdtSetAction(ChId, ExpireAction);
#endif

    return WDT_ERR_NONE;
}


#endif

/**
 *  AmbaRTSL_WdtRestart - Feed the WDT.
 *  @return error code
 */
UINT32 AmbaRTSL_WdtFeed(void)
{

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    AmbaCSL_WdtSetRestart(pAmbaWDT_Reg[0U]);
#else
    AmbaCSL_WdtSetRestart();
#endif

    return WDT_ERR_NONE;
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/**
 *  AmbaRTSL_WdtFeedByChId - Feed the specified WDT.
 *  @param[in] ChId WDT channel id
 *  @return error code
 */
UINT32 AmbaRTSL_WdtFeedByChId(UINT32 ChId)
{
    AmbaCSL_WdtSetRestart(pAmbaWDT_Reg[ChId]);

    return WDT_ERR_NONE;
}

/**
 *  AmbaRTSL_WdtStop - Stop the WDT.
 *  @return error code
 */
UINT32 AmbaRTSL_WdtStop(void)
{
    AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[0U], 0U);

    return WDT_ERR_NONE;
}

/**
 *  AmbaRTSL_WdtStopByChId - Stop the specified WDT.
 *  @param[in] ChId WDT channel id
 *  @return error code
 */
UINT32 AmbaRTSL_WdtStopByChId(UINT32 ChId)
{
    AmbaCSL_WdtSetCtrlReg(pAmbaWDT_Reg[ChId], 0U);

    return WDT_ERR_NONE;
}

/**
 *  AmbaRTSL_WdtGetStatusById - Check if timeout has occurred
 *  @param[in] ChId WDT channel id
 *  @return WDT timeout state
 */
UINT32 AmbaRTSL_WdtGetStatusById(UINT32 ChId)
{
    UINT32 RetVal;

    if ((AmbaCSL_RctGetWdtState() & ((UINT32)0x1U << ChId)) == 0U) {
        RetVal = 1U;
    } else {
        RetVal = 0U;
    }

    return RetVal;
}
#else
UINT32 AmbaRTSL_WdtStop(void)
{
    AmbaCSL_WdtSetCtrlReg(0U);

    return WDT_ERR_NONE;
}
#endif

/**
 *  AmbaRTSL_WdtGetTimeOutStatus - Check if timeout has occurred
 *  @return WDT timeout state
 */
UINT32 AmbaRTSL_WdtGetTimeOutStatus(void)
{
    UINT32 RetVal;

    /* FIXME, we need to check all the WDT status here */
    if ((AmbaCSL_RctGetWdtState() & 0x1U) == 0U) {
        RetVal = 1U;
    } else {
        RetVal = 0U;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_WdtClearTimeOutStatus - Clear TimeoutSts which is used to indicate if interrupt has occurred
 */
void AmbaRTSL_WdtClearTimeOutStatus(void)
{
    AmbaCSL_WdtClearWdtResetStatus();
}

/**
 *  AmbaRTSL_WdtGetTimeOutAction - Get Watchdog Timer Mode
 *  @return WDT timeout action
 */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
UINT32 AmbaRTSL_WdtGetTimeOutAction(UINT32 ChId)
{
    UINT32 RegVal, ExpireAction;

    RegVal = AmbaCSL_WdtGetCtrlReg(ChId);
    if ((RegVal & 0x2U) != 0U) {
        ExpireAction = WDT_ACT_SYS_RESET;
    } else if ((RegVal & 0x4U) != 0U) {
        ExpireAction = WDT_ACT_IRQ;
    } else if ((RegVal & 0x8U) != 0U) {
        ExpireAction = WDT_ACT_EXT;
    } else {
        ExpireAction = WDT_ACT_NONE;
    }

    return ExpireAction;
}

/**
 *  AmbaRTSL_WdtCoreFreqChg - Reconfigure the timer base on the new APB clock
 */
void AmbaRTSL_WdtCoreFreqChg(UINT32 ChId)
{
    UINT32 RegVal;

    RegVal = AmbaCSL_WdtGetCtrlReg(ChId);
    if ((RegVal & 0x1U) != 0U) {
        WDT_SetCountDownVal(ChId, AmbaWdtTimeOutVal[ChId]);
    }
}

UINT32 AmbaRTSL_WdtSetPattern(void)
{
    AmbaCSL_RctSetWdtPattern();

    return WDT_ERR_NONE;
}

#else
UINT32 AmbaRTSL_WdtGetTimeOutAction(void)
{
    UINT32 RegVal, ExpireAction;

    RegVal = AmbaCSL_WdtGetCtrlReg();
    if ((RegVal & 0x2U) != 0U) {
        ExpireAction = WDT_ACT_SYS_RESET;
    } else if ((RegVal & 0x4U) != 0U) {
        ExpireAction = WDT_ACT_IRQ;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if ((RegVal & 0x8U) != 0U) {
        ExpireAction = WDT_ACT_EXT;
#endif
    } else {
        ExpireAction = WDT_ACT_NONE;
    }

    return ExpireAction;
}

/**
 *  AmbaRTSL_WdtCoreFreqChg - Reconfigure the timer base on the new APB clock
 */
void AmbaRTSL_WdtCoreFreqChg(void)
{
    UINT32 RegVal;

    RegVal = AmbaCSL_WdtGetCtrlReg();
    if ((RegVal & 0x1U) != 0U) {
        WDT_SetCountDownVal(0U, AmbaWdtTimeOutVal);
    }
}
#endif

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV2)
UINT32 AmbaRTSL_WdtGetResetStatus(void)
{
    return AmbaWdtResetStatus;
}
#endif

