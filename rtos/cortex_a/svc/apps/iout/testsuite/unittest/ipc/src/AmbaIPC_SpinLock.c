/**
 * @file AmbaIPC_SpinLock.c
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
 * @details Global Spinlock Implementation
 *
 */
#include "AmbaIntrinsics.h"
#include "AmbaKAL.h"
#include "AmbaLink.h"
#include "AmbaIPC_Lock.h"
#include "AmbaPrint.h"
#include "AmbaLinkPrivate.h"
#include "AmbaLink_core.h"
#include "AmbaWrap.h"

/* all OS use the same size for each spinlock */
typedef struct {
    UINT32       slock;          ///< lock
    char         padding[12];    ///< align to ERG_SIZE
} aspinlock_t;

typedef struct {
    aspinlock_t *lock;           ///< shared memory for the lock
} aspinlock_db;

static aspinlock_db lock_set;
static UINT32 lock_set_size;

#if !defined(GCOVR_ENABLE)
#if defined(CONFIG_ARM64)
#define ASM_DOLOCK __asm__ volatile(                  \
                    /* LL/SC */                       \
                    "1:     ldaxr   %w0, [%1]\n"      \
                    "       cbnz    %w0, 1b\n"        \
                    "       stxr    %w0, %w2, [%1]\n" \
                    "       cbnz    %w0, 1b\n"        \
                    "       nop"                      \
                    : "=&r" (tmp)                     \
                    : "r" (lock), "r" (1)             \
                    : "memory");

#define ASM_UNLOCK __asm__ volatile(            \
                    "       stlr    wzr, [%0]"  \
                    :                           \
                    : "r" (lock)                \
                    : "memory");

static void do_SpinLock(UINT32 *lock)
{
    UINT32 tmp;

    // standard exclusive load/store on lock->lock
    ASM_DOLOCK
}

static void do_SpinUnlock(UINT32 *lock)
{
    // set lock->lock to 0
    ASM_UNLOCK
}
#else //defined(CONFIG_ARM64)
#define ASM_DOLOCK  __asm__ volatile(             \
                    "       pldw    %1\n"         \
                    "1:     ldaex   %0, %1\n"     \
                    "       cmp     %0, #0\n"     \
                    "       strexeq %0, %2, %1\n" \
                    "       cmpeq   %0, #0\n"     \
                    "       bne     1b\n"         \
                    : "=&r" (tmp), "+Q" (*lock)   \
                    : "r" (0x1)                   \
                    : "memory");

#define ASM_UNLOCK __asm__ volatile(          \
                    "       stl     %1, [%0]\n" \
                    :                           \
                    : "r" (lock), "r" (0)       \
                    : "cc");

static void do_SpinLock(UINT32 *lock)
{
    UINT32 tmp = 0U;
    (void)lock;
    (void)tmp;

    // standard exclusive load/store on lock->lock
    ASM_DOLOCK
}

static void do_SpinUnlock(UINT32 *lock)
{
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TouchUnused(lock);
#else
    (void)lock;
#endif
    // set lock->lock to 0
    ASM_UNLOCK
}
#endif //defined(CONFIG_ARM64)
#else  //defined(GCOVR_ENABLE)
extern void do_SpinUnlock(UINT32 *lock);
extern void do_SpinLock(UINT32 *lock);
#endif //defined(GCOVR_ENABLE)

static UINT32 SpinLockInited = 0U;

/**
 * @brief This function is used to assign a spin lock.
 * Note that it does not disable interrupts. It is used exclusively to
 * lock a specified spin lock across dual operating systems. If ISR is
 * anticipated to access a spin lock, the AmbaIPC_SpinLockIrqSave function
 * should be used.
 *
 * @param [in] SpinID Spin lock ID
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SpinUnlock
 * @see AmbaIPC_SpinLockIrqSave
 */
UINT32 AmbaIPC_SpinLock(UINT32 SpinID)
{
    UINT32 ret = 0U;
    IpcDebug("IPC %s(%d) %d", __func__, __LINE__, SpinID);
    if (SpinID >= lock_set_size) {
        ret = IPC_ERR_EINVAL;
    } else if (SpinLockInited != 1U) {
        IpcDebug("IPC %s(%d) %d SpinLockInited %d", __func__, __LINE__, SpinID, SpinLockInited);
        ret = 0U;
    } else {
        do_SpinLock(&lock_set.lock[SpinID].slock);
    }

    return ret;
}


/**
 * @brief This function is used to unlock a global spin lock.
 * Note that it does not disable interrupts. It is used exclusively to
 * unlock a specified spin lock across dual operating systems.
 *
 * @param [in] SpinID Spin lock ID
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SpinUnlockIrqRestore
 */
UINT32 AmbaIPC_SpinUnlock(UINT32 SpinID)
{
    UINT32 ret = 0U;

    IpcDebug("IPC %s(%d) %d", __func__, __LINE__, SpinID);
    if (SpinID >= lock_set_size) {
        ret = IPC_ERR_EINVAL;
    } else if (SpinLockInited != 1U) {
        IpcDebug("IPC %s(%d) %d SpinLockInited %d", __func__, __LINE__, SpinID, SpinLockInited);
        ret = 0U;
    } else {
        do_SpinUnlock(&lock_set.lock[SpinID].slock);
    }

    return ret;
}

/**
 * @brief disable interrupts and exclusively lock a specified spin lock
 *
 * @param [in] lock Spin lock ID
 * @param [in] pFlags The pointer to saved IRQ status
 *
 * @return 0 - OK, others - NG
 */
UINT32 do_SpinLockIrqSave(UINT32 *lock, UINT32 *pFlags)
{
    *pFlags = AMBA_get_interrupt_state();
    AMBA_disable_interrupt();

    do_SpinLock(lock);

    return 0;
}

/**
 * @brief This function is used to assign a spin lock.
 * It is used when ISR is anticipated to access a spin lock. It will disable
 * interrupts and save the IRQ status.
 *
 * @param [in] SpinID Spin lock ID
 * @param [in] pFlags The pointer to saved IRQ status
 *
 * @return 0 - OK, others - NG
 */
UINT32 AmbaIPC_SpinLockIrqSave(UINT32 SpinID, UINT32 *pFlags)
{
    UINT32 ret = 0U;

    if ((SpinID >= lock_set_size) || (pFlags == NULL)) {
        ret = IPC_ERR_EINVAL;
    } else if (SpinLockInited != 1U) {
        IpcDebug("IPC %s(%d) %d SpinLockInited %d", __func__, __LINE__, SpinID, SpinLockInited);
        ret = 0U;
    } else {
        (void)do_SpinLockIrqSave(&lock_set.lock[SpinID].slock, pFlags);
    }
    return ret;
}

/**
 * @brief enable interrupts and unlock a specified spin lock
 *
 * @param [in] lock Spin lock ID
 * @param [in] Flags saved IRQ status
 *
 * @return 0 - OK, others - NG
 */
UINT32 do_SpinUnlockIrqRestore(UINT32 *lock, UINT32 Flags)
{
    do_SpinUnlock(lock);
    AMBA_set_interrupt_state(Flags);

    return 0;
}

/**
 * @brief This function is used to unlock a global spin lock.
 * It is used when ISR is anticipated to access a spin lock. It will restore the
 * IRQ status.
 *
 * @param [in] SpinID Spin lock ID
 * @param [in] Flags IRQ status to be restored
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SpinLockIrqSave
 */
UINT32 AmbaIPC_SpinUnlockIrqRestore(UINT32 SpinID, UINT32 Flags)
{
    UINT32 ret = 0U;

    if (SpinID >= lock_set_size) {
        ret = IPC_ERR_EINVAL;
    } else if (SpinLockInited != 1U) {
        IpcDebug("IPC %s(%d) %d SpinLockInited %d", __func__, __LINE__, SpinID, SpinLockInited);
        ret = 0U;
    } else {
        (void)do_SpinUnlockIrqRestore(&lock_set.lock[SpinID].slock, Flags);
    }

    return ret;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_SpinInit
 *
 *  @Description::  Spinlock module init
 *
\*----------------------------------------------------------------------------*/
UINT32 AmbaIPC_SpinInit(void)
{
    UINT32 ret = 0U;
    void *ptr;

#ifdef AMBA_MISRA_FIX_H
    lock_set.lock = NULL;
    if (AmbaWrap_memcpy(&lock_set.lock, &AmbaLinkCtrl.AmbaIpcSpinLockBufAddr, sizeof(AmbaLinkCtrl.AmbaIpcSpinLockBufAddr))!= 0U) { }
#else
    lock_set.lock = (aspinlock_t *)AmbaLinkCtrl.AmbaIpcSpinLockBufAddr;
#endif
    (void)lock_set.lock->padding;

    lock_set_size = AmbaLinkCtrl.AmbaIpcSpinLockBufSize / sizeof(aspinlock_t);

    /* Reserve one spinlock space for BCH NAND controller workaround. */
    lock_set_size -= 1U;

    IpcDebug("IPC %s(%d) addr %p size %d", __func__, __LINE__, lock_set.lock, lock_set_size);

    if(AMBA_IPC_NUM_SPINLOCK > lock_set_size) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AMBA_IPC_NUM_SPINLOCK too big", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        ret = IPC_ERR_EINVAL;
    } else {
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&ptr, &lock_set.lock);
#else
        ptr = (void *)lock_set.lock;
#endif
        (void)ptr;

#if defined(CONFIG_AMBALINK_RPMSG_ROLE_MAST)
        {
            extern UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n);
            IpcDebug("IPC %s(%d) memset(%p, %d, 0x%lx)", __func__, __LINE__, ptr, 0, (SIZE_t)lock_set_size * sizeof(aspinlock_t));
            if (AmbaWrap_memset(ptr, 0, (SIZE_t)lock_set_size * sizeof(aspinlock_t))!= 0U) { }
        }
#else
#if !defined(CONFIG_CPU_CORTEX_R52) && !defined(CONFIG_FWPROG_R52SYS_ENABLE)
        /* legacy rtos + linux */
        if (1U == AmbaLinkCtrl.AmbaLinkMachineRev) {
            extern UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n);
            IpcDebug("IPC %s(%d) memset(%p, %d, 0x%lx)", __func__, __LINE__, ptr, 0, (SIZE_t)lock_set_size * sizeof(aspinlock_t));
            if (AmbaWrap_memset(ptr, 0, (SIZE_t)lock_set_size * sizeof(aspinlock_t))!= 0U) { }
        }
#endif
#endif

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&ptr, &lock_set.lock);
#else
        ptr = (void *)lock_set.lock;
#endif

        IpcCacheClean(ptr, lock_set_size * sizeof(aspinlock_t));

        SpinLockInited = 1U;
    }

    return ret;
}
