/**
 * @file AmbaIPC_Mutex.c
 *
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
 *
 * @details Global Mutex Implementation
 *
 */

#include "AmbaIntrinsics.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaLink.h"
#include "AmbaPrint.h"
#include "AmbaLinkPrivate.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaWrap.h"
#include "AmbaLink_core.h"

#define CORE_A9C0           ((UINT8)0x1U)

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define CORE_RC0            ((UINT8)0x04U) // cf. AMBALINK_CORE_GUEST2 defined in Linux
#ifdef CONFIG_CPU_CORTEX_R52
#define CORE_LOCAL          CORE_RC0
#define IRQ_LOCAL           ((UINT32)AXI_SOFT_IRQ0 + (UINT32)AIPC_MUTEX_IRQ_R52)
#else
#define CORE_LOCAL          CORE_A9C0
#define IRQ_LOCAL           ((UINT32)AXI_SOFT_IRQ0 + (UINT32)AIPC_MUTEX_IRQ_RTOS)
#endif // CONFIG_CPU_CORTEX_R52
#else // Not CV2FS
#define CORE_LOCAL          CORE_A9C0
#define IRQ_LOCAL           ((UINT32)AXI_SOFT_IRQ0 + (UINT32)AIPC_MUTEX_IRQ_RTOS)
#endif // CONFIG_SOC_CV2FS

typedef struct {
    UINT32                slock;              ///< Spin lock
    UINT8                 owner;              ///< core that currently owns the mutex
    UINT8                 wait_list;          ///< cores that are waiting for the mutex
    INT8                  padding[10];        ///< align to ERG_SIZE
} amutex_share_t;

typedef struct {
    AMBA_KAL_MUTEX_t      mutex;              ///< local version of mutex
    UINT32                count;              ///< refcount for the mutex
} amutex_local_t;

typedef struct {
    AMBA_KAL_EVENT_FLAG_t wakeup_flags;       ///< flags to wake up waiting tasks
    UINT32                wakeup_mask;        ///< bit-pattern for wakeup flag

    amutex_local_t local[AMBA_IPC_NUM_MUTEX]; ///< local part of the global mutex
    amutex_share_t *share;                    ///< pointer to shared part, which
    // resides on the shared memory
} amutex_db;

static UINT32 MutexInited = 0U;

static amutex_db lock_set __attribute__((section(".bss.noinit")));

static inline UINT32 REMOTE_IS_WAITING(const amutex_share_t *share)
{
    UINT8 core_remote = 0xf;

    core_remote &= ~CORE_LOCAL;
    core_remote &= ~(share->wait_list);

    return core_remote;
}
#if !defined(GCOVR_ENABLE)
// Disable NEON registers usage in ISR
#pragma GCC push_options
#pragma GCC target("general-regs-only")
#endif
static void AipcMutexIsr(UINT32 IntID, UINT32 UserArg)
{
    ULONG AhbSpSwiAClr = AHB_SCRATCHPAD_BASE + AHB_SP_SWI_CLEAR_OFFSET;
    UINT32 *pReg;
    UINT8 errorFlag = 0U;
    UINT32 irq = IntID;
    if (irq >= (UINT32)AXI_SOFT_IRQ0) {
        irq -= (UINT32)AXI_SOFT_IRQ0;
    } else {
        IpcDebug("IPC %s(%d) IntID=0x%08x, AXI_SOFT_IRQ0=0x%08x", __func__, __LINE__, irq, AXI_SOFT_IRQ0);
        errorFlag = 1;
    }

    (void)UserArg;

    if (errorFlag == 0U) {
        /* pReg = (UINT32 *)AhbSpSwiAClr; */
        pReg = NULL;
        if (AmbaWrap_memcpy(&pReg, &AhbSpSwiAClr, sizeof(AhbSpSwiAClr))!= 0U) { }

        *pReg = (UINT32)((UINT32)1U << irq);

        (void)AmbaKAL_EventFlagSet(&lock_set.wakeup_flags, 0xFFFFFFFFU);
    }
}
#if !defined(GCOVR_ENABLE)
#pragma GCC pop_options
#endif
// Disable NEON registers usage in ISR

/**
 * @brief query mutex owner
 *
 * @param [in] id mutex ID
 *
 * @return 0 - NOT owned by local, others - owned by local
 */
UINT32 AmbaIPC_OWNER_IS_LOCAL(UINT32 id)
{
    UINT32 ret = 0U; /* NOT owned */
    const amutex_share_t *share;

    if (id >= AMBA_IPC_NUM_MUTEX) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_MutexTake invalid id %d!", id, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    } else {
        share = &lock_set.share[id];
        if ((share->owner == CORE_LOCAL) || (share->owner == 0U)) {
            ret = 1U; /* owned*/
        }
    }

    return ret;
}

/**
 * @brief This function is used to lock the global mutex.
 *
 * @param [in] MutexID Mutex ID
 * @param [in] Timeout Ticks Timeout value in millisecond
 *
 * @return 0 - OK, others - NG
 */
UINT32 AmbaIPC_MutexTake(UINT32 MutexID, UINT32 Timeout)
{
    UINT32 ret;
    amutex_share_t *share;
    amutex_local_t *local;
    UINT32 flags;

    if (MutexID >= AMBA_IPC_NUM_MUTEX) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_MutexTake invalid id %d!", MutexID, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        ret = IPC_ERR_EINVAL;
    } else if (MutexInited != 1U) {
        IpcDebug("IPC %s(%d) %d %d MutexInited %d", __func__, __LINE__, MutexID, Timeout, MutexInited);
        ret = 0U;
    } else {
        share = &lock_set.share[MutexID];
        local = &lock_set.local[MutexID];

        // lock the local mutex
        ret = AmbaKAL_MutexTake(&local->mutex, Timeout);

        if (ret != 0U) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaKAL_MutexTake error", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            ret = IPC_ERR_ETIMEDOUT;
        } else {
            // Check repeatedly until we become the owner or timeout.
            // Assume nobody long-holds share->slock.
            for(;;) {
                UINT32  dummy, status;

                (void)do_SpinLockIrqSave(&share->slock, &flags);

                if ( (share->owner == 0U) ||
                     ((share->owner == CORE_LOCAL) && (REMOTE_IS_WAITING(share) == 0U)) ) {
                    // set ourself as owner and break out of the waiting loop
                    share->owner = CORE_LOCAL;
                    share->wait_list &= ~CORE_LOCAL;
                    local->count++;
                    (void)do_SpinUnlockIrqRestore(&share->slock, flags);
                    break;
                }
                // add ourself to wait-list
                share->wait_list |= CORE_LOCAL;
                (void)do_SpinUnlockIrqRestore(&share->slock, flags);

                // wait for remote owner to finish
                status = AmbaKAL_EventFlagGet(&lock_set.wakeup_flags,
                                              (UINT32)((UINT32)0x1U << MutexID),
                                              1U, 1U,
                                              &dummy,
                                              Timeout);
                if (status != 0U) {
                    // timeout, no more wait
                    (void)do_SpinLockIrqSave(&share->slock, &flags);
                    share->wait_list &= ~CORE_LOCAL;
                    (void)do_SpinUnlockIrqRestore(&share->slock, flags);
                    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "timeout %d", MutexID, 0U, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                    ret = IPC_ERR_ETIMEDOUT;
                }
            }
        }
    }

    return ret;
}
#if defined(AmbaIPC_MUTEX_EXTRA_SAFETY)
static UINT32 LocalMutex_Owner_Check(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 ret;
    AMBA_KAL_TASK_t *pTask = NULL;
    AMBA_KAL_MUTEX_INFO_s MutexInfo;

    /* get self task id */
    ret = AmbaKAL_TaskIdentify(&pTask);
    if (ret == 0U) {
        /* get mutex owner task id */
        ret = AmbaKAL_MutexQuery(pMutex, &MutexInfo);
    }
    if (ret == 0U) {
        /* error: I am NOT the owner */
        if (MutexInfo.pOwnerTask != pTask) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: non-owner error", __func__, NULL, NULL, NULL, NULL);
            ret = IPC_ERR_EINVAL;
        }
    }

    return ret;
}
#endif
/**
 * @brief This function is used to unlock the global mutex.
 *
 * @param [in] MutexID Mutex ID
 *
 * @return 0 - OK, others - NG
 */
UINT32 AmbaIPC_MutexGive(UINT32 MutexID)
{
    UINT32 ret = 0U;
    amutex_share_t *share;
    amutex_local_t *local;
    UINT32 flags;

    if (MutexID >= AMBA_IPC_NUM_MUTEX) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_MutexGive invalid id %d\n", MutexID, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        ret = IPC_ERR_EINVAL;
    } else if (MutexInited != 1U) {
        IpcDebug("IPC %s(%d) %d MutexInited %d", __func__, __LINE__, MutexID, MutexInited);
        ret = 0U;
    } else {
        share = &lock_set.share[MutexID];
        local = &lock_set.local[MutexID];

        if (share->owner != CORE_LOCAL) {
            /* non-owner is normal during boot-up when ambalink and SD card init at the same time
             *   AmbaIPC_SDLock() AmbaLink_Enable=0
             *   AmbaIPC_SDUnlock() AmbaLink_Enable=1
             */
            IpcDebug("IPC %s(%d) %d non-owner", __func__, __LINE__, MutexID);
            ret = IPC_ERR_EINVAL;
#if defined(AmbaIPC_MUTEX_EXTRA_SAFETY)
        } else if (LocalMutex_Owner_Check(&(local->mutex)) != 0U) {
            IpcDebug("IPC %s(%d) %d LocalMutex_Owner_Check error", __func__, __LINE__, MutexID);
            ret = IPC_ERR_EINVAL;
#endif
        } else {
            (void)do_SpinLockIrqSave(&share->slock, &flags);
            (local->count)--;
            if ((local->count) == 0U) {
                // We are done with the mutx, now let other waiting EUs to grab it
                share->owner = 0U;
                if (share->wait_list != 0U) {
                    ULONG AhbSpSwiAddr = AHB_SCRATCHPAD_BASE + AHB_SP_SWI_SET_OFFSET;
                    //ULONG *pReg; this will not work
                    UINT32 *pReg;

                    /* pReg = (UINT32 *)AhbSpSwiAddr; */
                    pReg = NULL;
                    if (AmbaWrap_memcpy(&pReg, &AhbSpSwiAddr, sizeof(AhbSpSwiAddr))!= 0U) { }

                    // kick remote waiting core(s)
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#ifdef CONFIG_CPU_CORTEX_R52
                    /* R52 kick A53C0 */
                    if (0U != (share->wait_list & CORE_A9C0)) {
                        *pReg = (UINT32)((UINT32)1U << (UINT32)AIPC_MUTEX_IRQ_RTOS);
                    }
#else
                    /* A53C0 kick R52 */
                    if (0U != (share->wait_list & CORE_RC0)) {
                        *pReg = (UINT32)((UINT32)1U << (UINT32)AIPC_MUTEX_IRQ_R52);
                    }
#endif // CONFIG_CPU_CORTEX_R52
                    /* A53C0 kick Linux */
                    else {
                        *pReg = (UINT32)((UINT32)1U << (UINT32)AIPC_MUTEX_IRQ_LINUX);
                    }
#else // CONFIG_SOC_CV2FS
                    /* A53C0 kick Linux */
                    *pReg = (UINT32)((UINT32)1U << (UINT32)AIPC_MUTEX_IRQ_LINUX);
#endif // CONFIG_SOC_CV2FS
                }
            }
            (void)do_SpinUnlockIrqRestore(&share->slock, flags);

            // unlock local mutex
            ret = AmbaKAL_MutexGive(&local->mutex);
            if (ret != 0U) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaKAL_MutexGive error", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
            }
        }
    }

    return ret;
}

/**
 * @brief mutex module initialization
 *
 * @return 0 - OK, others - NG
 */
UINT32 AmbaIPC_MutexInit(void)
{
    UINT32 ret = 0U, i;
    AMBA_INT_CONFIG_s IntConfig;
    static char IpcMutex[AMBA_IPC_NUM_MUTEX][4];

    if (AmbaWrap_memset(&lock_set, 0, sizeof(lock_set))!= 0U) { }
    if((sizeof(amutex_share_t) * AMBA_IPC_NUM_MUTEX) > AmbaLinkCtrl.AmbaIpcMutexBufSize) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "(sizeof(amutex_share_t) * AMBA_IPC_NUM_MUTEX)(%d) > AmbaLinkCtrl.AmbaIpcMutexBufSize(%d)", (sizeof(amutex_share_t) * AMBA_IPC_NUM_MUTEX), AmbaLinkCtrl.AmbaIpcMutexBufSize, 0U, 0U, 0U);
        AmbaPrint_Flush();
        ret = IPC_ERR_EINVAL;
    } else {
        const void *tmp;
        char flagname[8] = "LockSet";
        // init shared part of aipc mutex memory
#ifdef AMBA_MISRA_FIX_H
        lock_set.share = NULL;
        if (AmbaWrap_memcpy(&lock_set.share, &AmbaLinkCtrl.AmbaIpcMutexBufAddr, sizeof(AmbaLinkCtrl.AmbaIpcMutexBufAddr))!= 0U) { }
#else
        lock_set.share = (amutex_share_t *) AmbaLinkCtrl.AmbaIpcMutexBufAddr;
#endif
        (void)lock_set.share->padding;

#if defined(CONFIG_AMBALINK_RPMSG_ROLE_MAST)
        IpcDebug("IPC %s(%d) memset(%p, %d, 0x%lx)", __func__, __LINE__, lock_set.share, 0, sizeof(amutex_share_t)*AMBA_IPC_NUM_MUTEX);
        if (AmbaWrap_memset(lock_set.share, 0, sizeof(amutex_share_t)*AMBA_IPC_NUM_MUTEX)!= 0U) { }
#else
#if !defined(CONFIG_CPU_CORTEX_R52) && !defined(CONFIG_FWPROG_R52SYS_ENABLE)
        /* legacy rtos + linux */
        if (1U == AmbaLinkCtrl.AmbaLinkMachineRev) {
            IpcDebug("IPC %s(%d) memset(%p, %d, 0x%lx)", __func__, __LINE__, lock_set.share, 0, sizeof(amutex_share_t)*AMBA_IPC_NUM_MUTEX);
            if (AmbaWrap_memset(lock_set.share, 0, sizeof(amutex_share_t)*AMBA_IPC_NUM_MUTEX)!= 0U) { }
        }
#endif
#endif

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&tmp, &lock_set.share);
#else
        tmp = (const void *)lock_set.share;
#endif
        IpcCacheClean(tmp, sizeof(amutex_share_t)*AMBA_IPC_NUM_MUTEX);

        // init local part of aipc mutex memory
        for (i = 0U; i < AMBA_IPC_NUM_MUTEX; i++) {
            IpcMutex[i][0] = 'I';
            IpcMutex[i][1] = 'P';
            IpcMutex[i][2] = 'C';
            IpcMutex[i][3] = '\0';

            lock_set.local[i].count = 0;

            (void)AmbaKAL_MutexCreate(&lock_set.local[i].mutex, &IpcMutex[i][0]);
        }

        // setup event flags
        (void)AmbaKAL_EventFlagCreate(&lock_set.wakeup_flags, flagname);
        lock_set.wakeup_mask = 0;

        IntConfig.TriggerType = AMBA_INT_HIGH_LEVEL_TRIGGER;     /* Sensitivity type */
        IntConfig.IrqType = AMBA_INT_IRQ;                       /* Interrupt type: IRQ or FIQ */

        IntConfig.CpuTargets  = AmbaLinkCtrl.AmbaLinkRunTarget;     /* Target cores: Core-0 */

        // setup wakeup interrupt
        (void)AmbaRTSL_GicIntConfig(IRQ_LOCAL, &IntConfig, AipcMutexIsr, 0U);
        (void)AmbaRTSL_GicIntEnable(IRQ_LOCAL);
    }

    if (ret == 0U) {
        MutexInited = 1U;
    }
    return ret;
}
