/**
 * @file AmbaCortexA53Fusa.c
 *
 * @copyright Copyright (C) 2020 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights.  This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors.  You may not use,
 * reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed
 * license agreement or nondisclosure agreement with Ambarella, Inc. or
 * its authorized affiliates.  In the absence of such an agreement, you
 * agree to promptly notify and return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AmbaTypes.h>
#include <AmbaDef.h>
#include <AmbaKAL.h>
#include <AmbaWrap.h>
#include <AmbaMisraFix.h>
#include <AmbaIntrinsics.h>
#include <AmbaFusa.h>

// FIXME: The KAL wrapper should provide abstract API for discovering CPUs
#define FUSA_NR_CPUS        CONFIG_KAL_THREADX_SMP_NUM_CORES
#define FUSA_STACK_SIZE     2048
#define FUSA_TASK_PRIORITY  96

/*
 * Task-specific object for running Cortex-A53 self-diagnostic loop.
 */
struct self_diag_obj {
    char name[16];       /* Used by RTOS task */
    UINT32 cpuid;        /* Executing CPU ID */
    UINT32 mask;         /* Bitmask of diagnostics to execute */
    AMBA_KAL_TASK_t task;  /* RTOS task object */
    UINT32 dtti;         /* DTTI in millisecond (1ms == 1 tick) */
    char stack[FUSA_STACK_SIZE];  /* Per-task stack */
};

/*
 * Instantiate the tasks based on the number of online CPUs.
 */
static struct self_diag_obj self_diag_objs[FUSA_NR_CPUS];

/*
 * Exception self-diagnostic provided in secure monitor as SIP service.
 *
 * SSR Work Item: CV2ASDK-8090
 */
static inline UINT32 fusa_cortex_a53_exception_self_diag(void)
{
    UINT32 ret;
#ifdef CONFIG_ARM64
    ret = AmbaSmcCall(0x8200ff81U, 0, 0, 0, 0);
#else
    ret = AmbaSmcCallA32(0x8200ff81U, 0, 0, 0, 0);
#endif
    return ret;
}

/*
 * Comprehensive ARMv8 instructions self-diagnostic provided in secure
 * monitor as SIP service.
 *
 * SSR Work Item: CV2ASDK-8106
 */
static inline UINT32 fusa_cortex_a53_isa_self_diag(void)
{
    UINT32 ret;
#ifdef CONFIG_ARM64
    ret = AmbaSmcCall(0x8200ff82U, 0, 0, 0, 0);
#else
    ret = AmbaSmcCallA32(0x8200ff82U, 0, 0, 0, 0);
#endif
    return ret;
}

/*
 * Configuration registers integrity self-diagnostic provided in secure
 * monitor as SIP service.
 *
 * SSR Work Item: CV2ASDK-8097
 */
static inline UINT32 fusa_cortex_a53_creg_self_diag(void)
{
    UINT32 ret;
#ifdef CONFIG_ARM64
    ret = AmbaSmcCall(0x8200ff83U, 0, 0, 0, 0);
#else
    ret = AmbaSmcCallA32(0x8200ff83U, 0, 0, 0, 0);
#endif
    return ret;
}

/*
 * Branch-prediction self-diagnostics provided in secure monitor as
 * SIP service.
 *
 * SSR Work Item: CV2ASDK-8091
 */
static inline UINT32 fusa_cortex_a53_bp_self_diag(void)
{
    UINT32 ret;
#ifdef CONFIG_ARM64
    ret = AmbaSmcCall(0x8200ff84U, 0, 0, 0, 0);
#else
    ret = AmbaSmcCallA32(0x8200ff84U, 0, 0, 0, 0);
#endif
    return ret;
}

/*
 * L2 cache self-dignostics provided in secure monitor as SIP service.
 *
 * SSR Work Item: CV2ASDK-8092
 */
static inline UINT32 fusa_cortex_a53_l2_self_diag(void)
{
    UINT32 ret;
#ifdef CONFIG_ARM64
    ret = AmbaSmcCall(0x8200ff85U, 0, 0, 0, 0);
#else
    ret = AmbaSmcCallA32(0x8200ff85U, 0, 0, 0, 0);
#endif
    return ret;
}

/*
 * ECC fault-injection self-diagnostics provided in secure monitor as SIP
 * service.
 *
 * SSR Work Item: CV2ASDK-8099
 */
static inline UINT32 fusa_cortex_a53_eccfi_self_diag(void)
{
    UINT32 ret;
#ifdef CONFIG_ARM64
    ret = AmbaSmcCall(0x8200ff86U, 0, 0, 0, 0);
#else
    ret = AmbaSmcCallA32(0x8200ff86U, 0, 0, 0, 0);
#endif
    return ret;
}

/*
 * Periodic execution of Cortex-A53 self-diagnostic.
 *
 * There is no error handling here. The computed results are sent to the
 * safety CPU for verification and to act on when faults are detected. The
 * safety CPU also monitors that the results are computed within allowed
 * deadlines as specified in the DTTI constraint.
 */
static void *periodic_self_diag_task(void *args)
{
    const struct self_diag_obj *obj;

    AmbaMisra_TypeCast32(&obj, &args);
    (void)(args);

    while (obj != NULL) {
        if ((obj->mask & FUSA_RUN_EXCEPTION) != 0U) {
            (void)fusa_cortex_a53_exception_self_diag();
        }
        if ((obj->mask & FUSA_RUN_ISA) != 0U) {
            (void)fusa_cortex_a53_isa_self_diag();
        }
        if ((obj->mask & FUSA_RUN_CREG) != 0U) {
            (void)fusa_cortex_a53_creg_self_diag();
        }
        if ((obj->mask & FUSA_RUN_BP) != 0U) {
            (void)fusa_cortex_a53_bp_self_diag();
        }
        if ((obj->mask & FUSA_RUN_L2) != 0U) {
            (void)fusa_cortex_a53_l2_self_diag();
        }
        if ((obj->mask & FUSA_RUN_ECCFI) != 0U) {
            (void)fusa_cortex_a53_eccfi_self_diag();
        }

        if ( AmbaKAL_TaskSleep(obj->dtti) != 0U ) {
            break;
        }
    }

    return NULL;
}

/*
 * Initialize Cortex-A53 FuSa periodic self-diagnostics.
 */
void AmbaCortexA53FusaInit(UINT32 mask, UINT32 dtti)
{
    UINT32 RetVal;
    UINT32 nr_cpus = FUSA_NR_CPUS;

    RetVal = AmbaWrap_memset(self_diag_objs, 0x0, sizeof(self_diag_objs));
    if (RetVal != 0U) {
        AmbaAssert();
    }

    for (UINT32 cpuid = 0U; cpuid < nr_cpus; cpuid++) {
        /* Setup the task object */
        struct self_diag_obj *obj = &self_diag_objs[cpuid];

        obj->name[0] = 'F';
        obj->name[1] = 'u';
        obj->name[2] = 'S';
        obj->name[3] = 'a';
        obj->name[4] = 'T';
        obj->name[5] = 'a';
        obj->name[6] = 's';
        obj->name[7] = 'k';
        obj->name[8] = '\0';

        obj->cpuid = cpuid;

        /* All cores run exception and instruction-set diags */
        obj->mask = FUSA_RUN_EXCEPTION | FUSA_RUN_ISA;
        if (obj->cpuid == 0U) {
            /* Only core0 runs these self-diags */
            obj->mask |= (FUSA_RUN_CREG |
                          FUSA_RUN_BP |
                          FUSA_RUN_L2 |
                          FUSA_RUN_ECCFI);
        }
        /* Now override with user-specified mask */
        obj->mask &= mask;

        /* Set the DTTI variable from user-specified value */
        obj->dtti = dtti;

        /* Create a task */
        RetVal = AmbaKAL_TaskCreate(&obj->task,
                                    obj->name,
                                    FUSA_TASK_PRIORITY,
                                    periodic_self_diag_task,
                                    obj,
                                    obj->stack,
                                    FUSA_STACK_SIZE,
                                    0U);
        if (RetVal != KAL_ERR_NONE) {
            AmbaAssert();
        }

        /* Bind to one CPU core */
        RetVal = AmbaKAL_TaskSetSmpAffinity(&obj->task,
                                            ((UINT32) 0x1U) << cpuid);
        if (RetVal != KAL_ERR_NONE) {
            AmbaAssert();
        }

        /* Kick off the task */
        RetVal = AmbaKAL_TaskResume(&obj->task);
        if (RetVal != KAL_ERR_NONE) {
            AmbaAssert();
        }
    }
}

/*
 * Enable diags with mask bitmap.
 */
void AmbaCortexA53FusaEnable(UINT32 mask)
{
    UINT32 cpuid;
    UINT32 nr_cpus = FUSA_NR_CPUS;

    for (cpuid = 0U; cpuid < nr_cpus; cpuid++) {
        struct self_diag_obj *obj = &self_diag_objs[cpuid];
        UINT32 newmask = obj->mask;

        newmask |= mask;
        newmask &= 0x3fU;  /* Sanity check on range of newmask */

#if (FUSA_NR_CPUS > 1)
        if (cpuid != 0U) {
            /* Enforce affinity of diags that must run on primary core */
            newmask &= ~(FUSA_RUN_CREG |
                         FUSA_RUN_BP |
                         FUSA_RUN_L2 |
                         FUSA_RUN_ECCFI);
        }
#endif

        obj->mask = newmask;  /* Update */
    }
}

/*
 * Disable diags with mask bitmap.
 */
void AmbaCortexA53FusaDisable(UINT32 mask)
{
    UINT32 cpuid;
    UINT32 nr_cpus = FUSA_NR_CPUS;

    for (cpuid = 0; cpuid < nr_cpus; cpuid++) {
        struct self_diag_obj *obj = &self_diag_objs[cpuid];
        obj->mask &= ~mask;  /* Update */
    }
}

/*
 * Fault Injection.
 */
void AmbaCortexA53FusaInject(UINT32 mask)
{
    UINT32 ret;
#ifdef CONFIG_ARM64
    ret = AmbaSmcCall(0x8200ff87U, mask, 0, 0, 0);
#else
    ret = AmbaSmcCallA32(0x8200ff87U, mask, 0, 0, 0);
#endif
    (void)(ret);
}

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
