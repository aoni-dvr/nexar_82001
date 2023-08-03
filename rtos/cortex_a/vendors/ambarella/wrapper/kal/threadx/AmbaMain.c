/**
 *  @file AmbaMain.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details main entry of Ambarella Camera Platform for ThreadX and ThreadX/SMP
 *
 */

#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif
#include "AmbaDef.h"
#include "AmbaIntrinsics.h"

#define AMBA_KAL_SOURCE_CODE

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#ifndef AMBA_CORTEX_A76_H
#include "AmbaCortexA76.h"
#endif
#endif

#include "AmbaKAL.h"
#include "AmbaCache.h"
#include "tx_thread.h"

#include "AmbaSYS.h"
#include "AmbaSYS_Ctrl.h"
#include "AmbaDrvEntry.h"

#include "AmbaRTSL_CPU.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_TMR.h"

#include "AmbaCSL_Scratchpad.h"

#include "AmbaKAL_private.h"

#ifdef CONFIG_XEN_SUPPORT
#include <AmbaXen.h>
#endif
//#define amba_xen_print_int(...)
//#define amba_xen_print(...)

#define AMBA_CORE_INT           AMBA_INT_SGI_ID15

#define AMBA_ERR_NONE           0x0U
#define AMBA_ERR_ARG            0x1U

void interrupt_core(UINT32 TargetCore); /* called from tx_thread_smp_core_preempt.asm */
INT32 main(void);
void AmbaOtherCoreEntry(void);
void AmbaExceptionHandler(UINT32 ExceptionID, ULONG * pSavedRegs);

/*************************************************************************************************/

void interrupt_core(UINT32 TargetCore)
{
#ifdef CONFIG_ENABLE_AMBALINK
    (void)AmbaRTSL_GicSendSGI(AMBA_CORE_INT, AMBA_INT_IRQ, AMBA_INT_SGI2CPU_TARGET_LIST, ((UINT32)0x1U << TargetCore));
#else
    (void)AmbaRTSL_GicSendSGI(AMBA_CORE_INT, AMBA_INT_FIQ, AMBA_INT_SGI2CPU_TARGET_LIST, ((UINT32)0x1U << TargetCore));
#endif
}

static void KAL_SGI_ISR(UINT32 IntID __attribute__((unused)), UINT32 IntArg __attribute__((unused)))
{
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&IntArg);
}

static void KAL_TMR_ISR(UINT32 TimerID __attribute__((unused)), UINT32 UserArg __attribute__((unused)))
{
    /* Defined in tx_timer_interrupt.s */
    void _tx_timer_interrupt(void);

    _tx_timer_interrupt();                      /* trigger the OS Scheduler */

    AmbaMisra_TouchUnused(&TimerID);
    AmbaMisra_TouchUnused(&UserArg);
}

#if defined(CONFIG_SSP_THREADX_NEWLIB) && !defined(VCAST_AMBA)

struct _reent *__getreent(void);
void __malloc_lock(struct _reent *_r __attribute__((unused)));
void __malloc_unlock(struct _reent *_r __attribute__((unused)));

struct _reent AmbaNewlibImpureData[TX_THREAD_SMP_MAX_CORES];
struct _reent *AmbaNewlibImpurePtr[TX_THREAD_SMP_MAX_CORES];

struct _reent *__getreent(void)
{
    UINT32 CpuID = AmbaRTSL_CpuGetCoreID();
    UINT32 RtosCoreId;
    (void)AmbaSYS_GetRtosCoreId(&RtosCoreId);
    return AmbaNewlibImpurePtr[CpuID - RtosCoreId];
}

/*
 * Implementation of Newlib's __malloc_lock and __malloc_unlock
 */
static AMBA_KAL_MUTEX_t AmbaNewlibMutex;

void __malloc_lock(struct _reent *_r __attribute__((unused)))
{
    UINT32 ContextType;

    (void)_r;

    /* This function should not be called in ISR mode */
    (void)AmbaKAL_GetCpuContext(&ContextType);
    if (ContextType == 0U) {
        AmbaAssert();
    }
    (void)AmbaKAL_MutexTake(&AmbaNewlibMutex, KAL_WAIT_FOREVER);
}

void __malloc_unlock(struct _reent *_r __attribute__((unused)))
{
    UINT32 ContextType;

    (void)_r;

    /* This function should not be called in ISR mode */
    (void)AmbaKAL_GetCpuContext(&ContextType);
    if (ContextType == 0U) {
        AmbaAssert();
    }
    (void)AmbaKAL_MutexGive(&AmbaNewlibMutex);
}
#endif /* defined(CONFIG_SSP_THREADX_NEWLIB) && !defined(VCAST_AMBA) */

#if defined(CONFIG_XEN_SUPPORT)

static UINT32 tx_on_xen_ticks = 0;  /* Ticks (TimerValue) for vtimer */

/*
 * Get the frequency of the vtimer.
 */
static __attribute__((noinline)) UINT32 arm_get_cntfrq(void)
{
    UINT32 cntfrq = 0;

#ifdef CONFIG_ARM32
    __asm__ __volatile__("mrc p15, 0, %0, c14, c0, 0" : : "r" (cntfrq));
#else
    __asm__ __volatile__("mrs %0, cntfrq_el0" : : "r" (cntfrq));
#endif
    return cntfrq;
}

/*
 * Fire next shot for vtimer.
 */
static  __attribute__((noinline)) void tx_on_xen_schedule_arm_vtimer(void)
{
    UINT32 cntv_tval = tx_on_xen_ticks;
    UINT32 cntv_ctl = 0x1U;

    /* Write to Virtual Timer TimerValue register */
    __asm__ __volatile__("isb");

#ifdef CONFIG_ARM32
    /* Write to Virtual Timer TimerValue register */
    __asm__ __volatile__("mcr p15, 0, %0, c14, c3, 0" : : "r" (cntv_tval));
    /* Write to Counter-timer Virtual Timer Control register */
    __asm__ __volatile__("mcr p15, 0, %0, c14, c3, 1" : : "r" (cntv_ctl));
#else
    /* Write to Virtual Timer TimerValue register */
    __asm__ __volatile__("msr cntv_tval_el0, %0" : : "r" (cntv_tval));
    /* Write to Counter-timer Virtual Timer Control register */
    __asm__ __volatile__("msr cntv_ctl_el0, %0" : : "r" (cntv_ctl));
#endif

    (void)cntv_tval;
    (void)cntv_ctl;
    // For MISRA 2012 Rule 2.2, example use __asm__ ("nop")
    {
        volatile UINT32 x = 0;
        (void)x;
    }
}

/*
 * Handle interrupt from vtimer.
 */
static void tx_on_xen_handle_arm_vtimer_interrupt(UINT32 intid, UINT32 arg)
{
    extern void _tx_timer_interrupt(void);
    (void)(intid);
    (void)(arg);

    tx_on_xen_schedule_arm_vtimer();
    _tx_timer_interrupt();
}

/*
 * Setup vtimer for ThreadX scheduler.
 */
static void tx_on_xen_setup_vtimer(void)
{
    AMBA_INT_CONFIG_s irqsetup = {
        .TriggerType = INT_TRIG_HIGH_LEVEL,
        .IrqType = INT_TYPE_IRQ,
        .CpuTargets = 0x1U,
    };

    tx_on_xen_ticks = (arm_get_cntfrq() / (UINT32) TX_TIMER_TICKS_PER_SECOND);
#ifdef CONFIG_SOC_CV2FS
    (void)AmbaRTSL_GicIntConfig(AMBA_INT_PPI_ID027_VIRTUAL_TIMER,
                                &irqsetup,
                                tx_on_xen_handle_arm_vtimer_interrupt,
                                0);
    (void)AmbaRTSL_GicIntEnable(AMBA_INT_PPI_ID027_VIRTUAL_TIMER);
#else
    (void)AmbaRTSL_GicIntConfig(AMBA_INT_PPI_ID27_VIRTUAL_TIMER,
                                &irqsetup,
                                tx_on_xen_handle_arm_vtimer_interrupt,
                                0);
    (void)AmbaRTSL_GicIntEnable(AMBA_INT_PPI_ID27_VIRTUAL_TIMER);
#endif
}

#else

#if defined(CONFIG_XEN_SUPPORT)
static  __attribute__((noinline)) void tx_on_xen_schedule_arm_vtimer(void)
{
    // For MISRA 2012 Rule 2.2, example use __asm__ ("nop")
    volatile UINT32 x = 0;
    (void)x;
}
#endif  // CONFIG_XEN_SUPPORT
#endif  // CONFIG_XEN_SUPPORT

/**
 *  InitRtosCores - Entry function to enable the core executing RTOS
 */
static void InitRtosCore(UINT32 CoreId)
{
#if !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
    ULONG EntryPoint = CONFIG_FWPROG_SYS_LOADADDR;
#endif

#ifdef CONFIG_ARM64
    (void)CoreId;
#if !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
    (void)AmbaSmcCall(PSCI_CPU_ON_AARCH64, CoreId, EntryPoint, 0x0U, 0x0U);
#endif
#else
    (void)AmbaSmcCallA32(PSCI_CPU_ON_AARCH32, CoreId, EntryPoint, 0x0U, 0x0U);
#endif
    /* stall this core after SMC call */
    AmbaAssert();
}

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define AMBA_AXI_MISC_CTRL_OFFSET   10U
#define AMBA_AXI_RVBAR_ADDR1_OFFSET 26U

#pragma GCC optimize ("O0")
void BootCore1(UINT32 entry)
{
    //UINT32 *pEntryPtr = (UINT32 *)CONFIG_FWPROG_SYS_LOADADDR;
    volatile UINT32 *pAxiConfigReg = (volatile UINT32 *)AMBA_CORTEX_A76_AXI_CONFIG_PHYS_BASE_ADDR;

    //*pEntryPtr = 0x14000000;
    //AmbaCache_DataInvalidate(CONFIG_FWPROG_SYS_LOADADDR, sizeof(UINT32));
    pAxiConfigReg[AMBA_AXI_RVBAR_ADDR1_OFFSET] = (entry >> 0x8U);
    pAxiConfigReg[AMBA_AXI_MISC_CTRL_OFFSET] = pAxiConfigReg[AMBA_AXI_MISC_CTRL_OFFSET] & ~(0x2U<<16U);
}
#pragma GCC push_options
#endif
/**
 *  InitBiosDriver - Basic I/O initializations
 *  @param[in] NumSmpCores Number of SMP cores
 */
#pragma GCC optimize ("O0")
static void InitBiosDriver(UINT8 NumSmpCores)
{
    ULONG  CoreID;
    UINT32 RtosCoreId;
    ULONG  EntryPoint;

    (void)AmbaSYS_GetRtosCoreId(&RtosCoreId);

#ifdef CONFIG_FWPROG_SYS_LOADADDR
    EntryPoint = CONFIG_FWPROG_SYS_LOADADDR;
#else
    EntryPoint = 0x0U;
    AmbaAssert();
#endif

#ifdef CONFIG_XEN_SUPPORT
    if (AmbaXen_is_guest() != 0u) {
        /*
         * If we're running as a guest VM, the hypervisor has already
         * released the cores. We can skip the bootstrapping protocol.
         */
    } else {
#endif
        /* Release the cores for SMP RTOS. */
        for (CoreID = (RtosCoreId + 1U); CoreID < NumSmpCores; CoreID++) {
            /* SMC call for ATF service */
#ifdef CONFIG_ARM64
#if !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
            (void)AmbaSmcCall(PSCI_CPU_ON_AARCH64, CoreID, EntryPoint, 0x0U, 0x0U);
#else
            BootCore1(EntryPoint);
#endif
#else
            (void)AmbaSmcCallA32(PSCI_CPU_ON_AARCH32, CoreID, EntryPoint, 0x0U, 0x0U);
#endif
        }
#ifdef CONFIG_XEN_SUPPORT
    }

    if (AmbaXen_is_guest() != 0u) {
        /* TODO: SMP support on XEN! */
    } else {
#endif
        AmbaCSL_SkpdSetScratchpadData(0U, 0xDEADBEAFU);
        AmbaCSL_SkpdSetScratchpadData(1U, 0xDEADBEAFU);
        AmbaCSL_SkpdSetScratchpadData(2U, 0xDEADBEAFU);
        AmbaCSL_SkpdSetScratchpadData(3U, 0xDEADBEAFU);
#ifdef CONFIG_XEN_SUPPORT
    }
#endif

    /* Bring-up the primary processor. */
    AmbaMMU_SetupPrimary(NumSmpCores);

#if 0
#if defined(CONFIG_XEN_SUPPORT)
    if (AmbaXen_is_guest() != 0u) {
        // TODO: 2nd stage setting for xen
    }
#endif
#endif // 0/1

    AmbaRTSL_PllInit(24000000U);
    AmbaRTSL_GicInit();

#ifdef CONFIG_XEN_SUPPORT
    if (AmbaXen_is_guest() != 0u) {
//        amba_xen_print("FIXME: skipping user call back inits!\n");
        ;
    } else {
#endif
        if (AmbaSysUserCallbacks.UserEntryBefOS != NULL) {
            AmbaSysUserCallbacks.UserEntryBefOS();
        }
#if defined(CONFIG_XEN_SUPPORT)
    }

    if (AmbaXen_is_guest() != 0u) {
        /* Use virtual timer */
        tx_on_xen_setup_vtimer();
    } else {
#endif
        AmbaRTSL_TmrInit();
#if defined(CONFIG_XEN_SUPPORT)
    }

    if (AmbaXen_is_guest() != 0u) {
        /* TODO: SMP support on XEN! */
    } else {
#endif
        /* Make the other cores finish their MMU init */
        AmbaCSL_SkpdSetScratchpadData(0U, 0x00504D53U); /* "SMP" */
        AmbaCSL_SkpdSetScratchpadData(1U, 0x01000000U);
        AmbaCSL_SkpdSetScratchpadData(2U, 0x02000000U);
        AmbaCSL_SkpdSetScratchpadData(3U, 0x03000000U);
        AmbaCSL_SkpdSetScratchpadData(RtosCoreId, 0x00504D53U); /* "SMP" */

        for (CoreID = (RtosCoreId + 1U); CoreID < NumSmpCores; CoreID++) {
            while ((AmbaCSL_SkpdGetScratchpadData(CoreID) & 0xFFFFFFU) == 0x0U) {
                continue;
            }
        }
#if defined(CONFIG_XEN_SUPPORT)
    }
#endif
}
#pragma GCC push_options

/**
 *  InitRtosDriver - Driver initializations
 */
static void InitRtosDriver(void)
{
    /*
     *  Initialization for RTOS service support
     */
#if defined(CONFIG_XEN_SUPPORT)
    if (AmbaXen_is_guest() != 0u) {
//        amba_xen_print("FIXME: skipping AmbaDrvEntry()\n");
        {
            /*
                [2] = AmbaDMA_DrvEntry,
                [4] = AmbaGPIO_DrvEntry,
                [5] = AmbaUART_DrvEntry,
                [8] = AmbaINT_DrvEntry,
                [14] = AmbaSYS_DrvEntry,
                [15] = AmbaTMR_DrvEntry,
            */
            extern UINT32 AmbaUART_DrvEntry(void);
            extern UINT32 AmbaINT_DrvEntry(void);

            (void)AmbaUART_DrvEntry();
            (void)AmbaINT_DrvEntry();
        }
    } else {
#endif
        AmbaDrvEntry();
#if defined(CONFIG_XEN_SUPPORT)
    }
#endif

    if (AmbaSysUserCallbacks.UserEntryAftOS != NULL) {
        AmbaSysUserCallbacks.UserEntryAftOS();
    }
}

/* For Misra-C checking, should be defined as macro in reent.h */
#ifndef _REENT_INIT_PTR
void _REENT_INIT_PTR(struct _reent *name);
#endif

/**
 *  main - Entry function for the primary processor
 */
INT32 main(void)
{
    UINT32 CpuID = AmbaRTSL_CpuGetCoreID();
    UINT32 RtosCoreId = 0U;
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 TmrFreq, TmrTicks, OsTimerId = AMBA_NUM_TIMER;

#if defined(CONFIG_XEN_SUPPORT)
    if (AmbaXen_is_guest() != 0u) {
#ifdef CONFIG_ARM32
        // Work-around potential bug in TX code to get CPU ID
        // Double-check the implementation of AmbaRTSL_CpuGetCoreID()!
        UINT32 mpidr = 0x0U;
        __asm__ __volatile__("mrc p15, 0, %0, cr0, cr0, 5" : : "r" (mpidr));
        CpuID &= 0x3u;
        (void)mpidr;
#else
        // TODO
#endif
    }
#endif

    (void)AmbaSYS_GetRtosCoreId(&RtosCoreId);
    /* Driver initialization before OS is launching */
    if (CpuID == RtosCoreId) {
        /* Init the RtosCore */
        InitBiosDriver(TX_THREAD_SMP_MAX_CORES);
    } else if (CpuID == 0U) {
        /* Core0 is not the core executing RTOS, and it is used to wake the RtosCore up */
        InitRtosCore(RtosCoreId);
    } else {
        /* should not happened */
        AmbaAssert();
    }

#if !(defined(CONFIG_THREADX) && defined(CONFIG_ARM64))
    _tx_initilaize_rtos_core_id(RtosCoreId);
#endif
#if defined(CONFIG_XEN_SUPPORT)
    if (AmbaXen_is_guest() != 0u) {
        /* Kick off virtual timer */
        tx_on_xen_schedule_arm_vtimer();
    } else {
#endif
        /* Set OS timer */
        TmrFreq = 1000000U;
        TmrTicks = TmrFreq / (UINT32)TX_TIMER_TICKS_PER_SECOND;
        (void)AmbaRTSL_TmrGetOsTimerId(&OsTimerId);
        (void)AmbaRTSL_TmrHookTimeOutHandler(OsTimerId, KAL_TMR_ISR, 0U);
        (void)AmbaRTSL_TmrConfig(OsTimerId, TmrFreq, TmrTicks);
        (void)AmbaRTSL_TmrStart(OsTimerId, TmrTicks);
#if defined(CONFIG_XEN_SUPPORT)
    }
#endif

#if defined(CONFIG_KAL_THREADX_PROFILE)
    /* Set system profiler */
    AmbaSysProfile_Init();
#endif

    /* Register SGI service for ThreadX SMP communication */
    IntConfig.TriggerType = AMBA_INT_HIGH_LEVEL_TRIGGER;
    IntConfig.IrqType = AMBA_INT_FIQ;
    IntConfig.CpuTargets = 1U;
    (void)AmbaRTSL_GicIntConfig(AMBA_CORE_INT, &IntConfig, KAL_SGI_ISR, 0U);

#if defined(CONFIG_SSP_THREADX_NEWLIB) && !defined(VCAST_AMBA)

#if TX_THREAD_SMP_MAX_CORES > 0
    AmbaNewlibImpurePtr[0] = &AmbaNewlibImpureData[0];
    _REENT_INIT_PTR(AmbaNewlibImpurePtr[0]);
#endif
#if TX_THREAD_SMP_MAX_CORES > 1
    AmbaNewlibImpurePtr[1] = &AmbaNewlibImpureData[1];
    _REENT_INIT_PTR(AmbaNewlibImpurePtr[1]);
#endif
#if TX_THREAD_SMP_MAX_CORES > 2
    AmbaNewlibImpurePtr[2] = &AmbaNewlibImpureData[2];
    _REENT_INIT_PTR(AmbaNewlibImpurePtr[2]);
#endif
#if TX_THREAD_SMP_MAX_CORES > 3
    AmbaNewlibImpurePtr[3] = &AmbaNewlibImpureData[3];
    _REENT_INIT_PTR(AmbaNewlibImpurePtr[3]);
#endif

#endif /* defined(CONFIG_SSP_THREADX_NEWLIB) && !defined(VCAST_AMBA) */

    /*
     *  Enter ThreadX kernel. The API tx_kernel_enter doesn't return. IRQ will be enabled in tx_thread_schedule.s
     */
    tx_kernel_enter();

    /* Shall not happen!! */
    AmbaAssert();

    return 0;
}

/**
 *  EnterRTOS - Try to enter RTOS
 *  @param[in] CpuID CPU cluster and core id
 *  @return error code
 */
static UINT32 EnterRTOS(UINT32 CpuID)
{
    UINT32 SKPD, RetVal = AMBA_ERR_NONE;

    if (CpuID < (UINT32)TX_THREAD_SMP_MAX_CORES) {
        AmbaMMU_SetupNonPrimaryPreSCU();

        /* Wait for the primary core MMU init */
        /* wait for Core0 to set the Flag of Release All Other Cores */
        do {
            SKPD = AmbaCSL_SkpdGetScratchpadData(CpuID) & 0xDEADBEAFU;
        } while (SKPD == 0xDEADBEAFU);

        /* Bring-up non-primary processor */
        AmbaMMU_SetupNonPrimaryPostSCU();
        AmbaRTSL_GicInitCorePrivate();                  /* GIC Initializations for Core Banking (Private) registers */

        /* Notify the primary core that it is safely to enable interrupts */
        SKPD |= 0x00504D53U; /* "SMP" */
        AmbaCSL_SkpdSetScratchpadData(CpuID, SKPD);

        _tx_thread_smp_initialize_wait();   /* Call ThreadX additional core wait. */
    } else {
        RetVal = AMBA_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaOtherCoreEntry - Entry function for the non-primary processors
 */
void AmbaOtherCoreEntry(void)
{
    UINT32 CpuID = AmbaRTSL_CpuGetCoreID();

//    amba_xen_print_int(CpuID);
//    amba_xen_print(" joined!\n");

    (void)EnterRTOS(CpuID);

    /* Shall not happen!! */
    AmbaAssert();
}

/**
 *  AmbaExceptionHandler - Exception Handler
 *  @param[in] ExceptionID Exception ID
 *  @param[in] pSavedRegs Saved cpu registers
 */
void AmbaExceptionHandler(UINT32 ExceptionID, ULONG * pSavedRegs)
{
    if (AmbaSysUserCallbacks.UserExceptionHandler != NULL) {
        AmbaSysUserCallbacks.UserExceptionHandler(ExceptionID, pSavedRegs);
    }

#if defined(CONFIG_XEN_SUPPORT)
    if (AmbaXen_is_guest() != 0u) {
        //amba_xen_print("Shutting down VM due to crash!\n");
        for (volatile UINT32 delay = 0; delay < 0x0fffffffu; delay++) { ; }
        AmbaXen_crash();
    }
#endif

    /* Shall not happen!! */
    AmbaAssert();
}

/**
 *  AmbaStackErrorHandler - Default stack error handler
 *  @param[in] pTask Pointer to the Software Task Control Block
 */
static void AmbaStackErrorHandler(AMBA_KAL_TASK_t *pTask __attribute__((unused)))
{
    AmbaMisra_TouchUnused(pTask);

    AmbaAssert();
}

/**
 *  AmbaIdleTaskEntry - Idle task entry
 *  @param[in] EntryArg Argument of the task entry
 */
static void * AmbaIdleTaskEntry(const void * EntryArg)
{
    UINT32 MaxIdleCycleTime;

#if defined(CONFIG_SSP_THREADX_NEWLIB) && !defined(VCAST_AMBA)
    static char AmbaNewlibMutexName[16] = "AmbaNewlibMutex";

    /* Create a mutex to implement __malloc_lock and __malloc_unlock for newlib */
    (void)AmbaKAL_MutexCreate(&AmbaNewlibMutex, AmbaNewlibMutexName);
#endif

    /* Register default stack overflow handler */
    (void)AmbaKAL_HookStkErrHandler(AmbaStackErrorHandler);

    /* Driver Initializations */
    InitRtosDriver();

    while (AmbaKAL_TaskYield() == KAL_ERR_NONE) {
        /* invoke user provided idle function */
        if (AmbaSysUserCallbacks.UserIdleCallback != NULL) {
            AmbaSysUserCallbacks.UserIdleCallback();
        }
        (void)AmbaSYS_GetMaxIdleCycleTime(&MaxIdleCycleTime);
        (void)AmbaKAL_TaskSleep(MaxIdleCycleTime);
    }

    AmbaMisra_TouchUnused(&EntryArg);

    return NULL;
}

/**
 *  tx_application_define - Create the root task
 *  @param[in] first_unused_memory pointer to the first unused memory region
 */
void tx_application_define(void *first_unused_memory __attribute__((unused)))
{
    static AMBA_KAL_TASK_t AmbaIdleTask GNU_SECTION_NOZEROINIT;
    static char AmbaIdleTaskName[16] = "AmbaIdleTask";
    static UINT8 AmbaIdleTaskStack[8192] GNU_SECTION_NOZEROINIT;
    static UINT32 AmbaIdleTaskStackSize = sizeof(AmbaIdleTaskStack);
    UINT32 RetVal;
    void *(*TmpFunction)(const void * TxEntryArg);
    void *(*EntryFunction) (void *EntryArg);

    TmpFunction = AmbaIdleTaskEntry;
    (void)AmbaWrap_memcpy(&EntryFunction, &TmpFunction, sizeof(EntryFunction));
    /* Create the root task */
    RetVal = AmbaKAL_TaskCreate(&AmbaIdleTask,                      /* pTask */
                                AmbaIdleTaskName,                   /* pTaskName */
                                KAL_TASK_PRIORITY_LOWEST - 1U,      /* Priority */
                                EntryFunction,
                                NULL,                               /* EntryArg */
                                & (AmbaIdleTaskStack[0]),           /* pStackBase */
                                AmbaIdleTaskStackSize,              /* StackByteSize */
                                0U);                                /* AutoStart */

    if (RetVal == AMBA_ERR_NONE) {
        (void)AmbaKAL_TaskSetSmpAffinity(&AmbaIdleTask, 0x1U);
        (void)AmbaKAL_TaskResume(&AmbaIdleTask);
    }

    AmbaMisra_TouchUnused(first_unused_memory);
}

void amba_reclaim_reent(void *ptr)
{
    /* Note: suppose TX_GLOBAL_VFP_ENABLE is not defined. */
#ifdef CONFIG_CC_USESTD
    _reclaim_reent(ptr);
#else
    (void)ptr;
#endif
}

