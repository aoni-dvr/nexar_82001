/**
 *  @file dsp_osal.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details DSP OS abstract layer APIs
 *
 */

#include "dsp_osal.h"
#include "AmbaDSP_EventCtrl.h"
#if defined (CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaFDT.h"  // for misra depress
#endif

typedef struct {
    uint32_t DspCode[DSP_MAX_IRQ_CODE_NUM];
    uint32_t VinSof[DSP_MAX_IRQ_VIN_NUM];
    uint32_t VinEof[DSP_MAX_IRQ_VIN_NUM];
    uint32_t Vout[DSP_MAX_IRQ_VOUT_NUM];
} osal_irq_id_t;
#if !defined (CONFIG_DEVICE_TREE_SUPPORT)
extern const osal_irq_id_t osal_irq_id;
#endif

typedef struct {
    ULONG       Base;       /* Virtual Address */
    ULONG       PhysBase;   /* Physical Address, u32 only */
    uint32_t    size;
    uint32_t    UsedSize;
    uint32_t    is_cached;
} osal_buf_pool_t;

typedef struct {
    ULONG       Base;     /* Virtual Address */
    uint32_t    size;
    uint32_t    pool_id;
} osal_buf_t;

static osal_buf_pool_t osal_buf_pool[DSP_BUF_POOL_NUM] = {0};

static ULONG ADDR_ALIGN_NUM(ULONG Addr, UINT32 Num)
{
    return (Addr + ((Num - (Addr & ((ULONG)Num-1U))) % Num));
}

/******************************************************************************/
/*                                  print                                     */
/******************************************************************************/
UINT32 dsp_osal_get_u64msb(UINT64 U64Val)
{
    return (UINT32)((U64Val >> 32U) & 0xFFFFFFFFU);
}

UINT32 dsp_osal_get_u64lsb(UINT64 U64Val)
{
    return (UINT32)(U64Val & 0xFFFFFFFFU);
}

void dsp_osal_print_flush(void)
{
    AmbaPrint_Flush();
}

void dsp_osal_print_stop_and_flush(void)
{
    AmbaPrint_StopAndFlush();
}

#if 0
void dsp_osal_printS5(const char *fmt,
                      const char *argv0,
                      const char *argv1,
                      const char *argv2,
                      const char *argv3,
                      const char *argv4)
{
    AmbaPrint_PrintStr5(fmt, argv0, argv1, argv2, argv3, argv4);
}
#endif

void dsp_osal_printU5(const char *fmt,
                      uint32_t argv0,
                      uint32_t argv1,
                      uint32_t argv2,
                      uint32_t argv3,
                      uint32_t argv4)
{
    AmbaPrint_PrintUInt5(fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_osal_module_printS5(uint32_t module_id, const char *fmt,
                             const char *argv0, const char *argv1,
                             const char *argv2, const char *argv3,
                             const char *argv4)
{
    AmbaPrint_ModulePrintStr5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_osal_module_printU5(uint32_t module_id, const char *fmt,
                             uint32_t argv0, uint32_t argv1,
                             uint32_t argv2, uint32_t argv3,
                             uint32_t argv4)
{
    AmbaPrint_ModulePrintUInt5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}

#if 0
void dsp_osal_module_printI5(uint32_t module_id, const char *fmt,
                             int32_t argv0, int32_t argv1,
                             int32_t argv2, int32_t argv3,
                             int32_t argv4)
{
    AmbaPrint_ModulePrintInt5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}
#endif

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
#if 0
uint32_t dsp_osal_sem_init(osal_sem_t *sem, char *pName, int32_t pshared, uint32_t value)
{
(void)pshared;
    return AmbaKAL_SemaphoreCreate(sem, pName, value);
}

uint32_t dsp_osal_sem_deinit(osal_sem_t *sem)
{
    return AmbaKAL_SemaphoreDelete(sem);
}

uint32_t dsp_osal_sem_post(osal_sem_t *sem)
{
    return AmbaKAL_SemaphoreGive(sem);
}

uint32_t dsp_osal_sem_wait(osal_sem_t *sem, uint32_t timeout)
{
    return AmbaKAL_SemaphoreTake(sem, timeout);
}
#endif

uint32_t dsp_osal_mutex_init(osal_mutex_t *mutex, char *pName)
{
    return AmbaKAL_MutexCreate(mutex, pName);
}

#if 0
uint32_t dsp_osal_mutex_deinit(osal_mutex_t *mutex)
{
    return AmbaKAL_MutexDelete(mutex);
}
#endif

uint32_t dsp_osal_mutex_lock(osal_mutex_t *mutex, uint32_t timeout)
{
    return AmbaKAL_MutexTake(mutex, timeout);
}

uint32_t dsp_osal_mutex_unlock(osal_mutex_t *mutex)
{
    return AmbaKAL_MutexGive(mutex);
}

uint32_t dsp_osal_eventflag_init(osal_even_t *event, char *pName)
{
    return AmbaKAL_EventFlagCreate(event, pName);
}

uint32_t dsp_osal_eventflag_set(osal_even_t *event, uint32_t flag)
{
    return AmbaKAL_EventFlagSet(event, flag);
}

uint32_t dsp_osal_eventflag_get(osal_even_t *event, uint32_t reqflag, uint32_t all, uint32_t clear, uint32_t *actflag, uint32_t timeout)
{
    return AmbaKAL_EventFlagGet(event, reqflag, all, clear, actflag, timeout);
}

uint32_t dsp_osal_eventflag_clear(osal_even_t *event, uint32_t flag)
{
    return AmbaKAL_EventFlagClear(event, flag);
}

uint32_t dsp_osal_spinlock(const osal_spinlock_t *pSpinlock)
{
    UINT32 *pCpuStatus;

    dsp_osal_typecast(&pCpuStatus, &pSpinlock);
    return AmbaKAL_CriticalSectionEnter(pCpuStatus);
}

uint32_t dsp_osal_spinunlock(const osal_spinlock_t *pSpinlock)
{
    UINT32 CpuStatus;

    (void)dsp_osal_memcpy(&CpuStatus, pSpinlock, sizeof(UINT32));
    return AmbaKAL_CriticalSectionExit(CpuStatus);
}

uint32_t dsp_osal_cmdlock(osal_mutex_t *mutex, const osal_spinlock_t *pSpinlock)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t retcode = 0U;

    retcode = dsp_osal_mutex_lock(mutex, DSP_CMD_LOCK_TIMEOUT);
    if (retcode != DSP_ERR_NONE) {
        dsp_osal_printU5("[ERR] dsp_osal_cmdlock : mtx lock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        retcode = dsp_osal_spinlock(pSpinlock);
        if (retcode != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_cmdlock : spinlock lock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }
    return Rval;
}

uint32_t dsp_osal_cmdunlock(osal_mutex_t *mutex, const osal_spinlock_t *pSpinlock)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t retcode = 0U;

    retcode = dsp_osal_spinunlock(pSpinlock);
    if (retcode != DSP_ERR_NONE) {
        dsp_osal_printU5("[ERR] dsp_osal_cmdunlock : spinunlock unlock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        retcode = dsp_osal_mutex_unlock(mutex);
        if (retcode != DSP_ERR_NONE) {
            dsp_osal_printU5("[ERR] dsp_osal_cmdunlock : mtx unlock fail[0x%X]", retcode, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }
    return Rval;
}

uint32_t dsp_osal_thread_create(osal_thread_t *thread, char *name, uint32_t priority,
                                osal_thread_entry_f entry, const void *arg,
                                void *stack_buff, uint32_t stack_size, uint32_t auto_start)
{
    AMBA_KAL_TASK_ENTRY_f task_entry;
    const void *EntryArg = arg;
(void)arg;
    dsp_osal_typecast(&task_entry, &entry);

    return AmbaKAL_TaskCreate(thread, name, priority, task_entry, EntryArg, stack_buff, stack_size, auto_start);
}

uint32_t dsp_osal_thread_delete(osal_thread_t *thread)
{
    return AmbaKAL_TaskDelete(thread);
}

uint32_t dsp_osal_thread_should_stop(osal_thread_t *thread)
{
    return AmbaKAL_TaskTerminate(thread);
}

uint32_t dsp_osal_thread_set_affinity(osal_thread_t *thread, uint32_t affinity)
{
    return AmbaKAL_TaskSetSmpAffinity(thread, affinity);
}

uint32_t dsp_osal_thread_resume(osal_thread_t *thread)
{
    return AmbaKAL_TaskResume(thread);
}

#if 0
uint32_t dsp_osal_mq_create(osal_mq_t *mq, char *name, uint32_t msg_size, void *msg_q_buf, uint32_t msg_q_buf_size)
{
    return AmbaKAL_MsgQueueCreate(mq, name, msg_size, msg_q_buf, msg_q_buf_size);
}

uint32_t dsp_osal_mq_send(osal_mq_t *mq, void *msg, uint32_t time_out)
{
    return AmbaKAL_MsgQueueSend(mq, msg, time_out);
}

uint32_t dsp_osal_mq_recv(osal_mq_t *mq, void *msg, uint32_t time_out)
{
    return AmbaKAL_MsgQueueReceive(mq, msg, time_out);
}

uint32_t dsp_osal_mq_query(osal_mq_t *mq, osal_mq_info_t *mq_info)
{
    UINT32 Rval = DSP_ERR_NONE;
    AMBA_KAL_MSG_QUEUE_INFO_s MsgQInfo;

    (void)dsp_osal_memset(&MsgQInfo, 0, sizeof(AMBA_KAL_MSG_QUEUE_INFO_s));

    Rval = AmbaKAL_MsgQueueQuery(mq, &MsgQInfo);
    mq_info->PendingNum = MsgQInfo.NumEnqueued;
    mq_info->FreeNum = MsgQInfo.NumAvailable;

    return Rval;
}
#endif

uint32_t dsp_osal_sleep(uint32_t msec)
{
    return AmbaKAL_TaskSleep(msec);
}

uint32_t dsp_osal_get_sys_tick(uint32_t *msec)
{
    return AmbaKAL_GetSysTickCount(msec);
}

#if 0
uint32_t dsp_osal_cache_size_align(uint32_t size)
{
    return ((size) + AMBA_CACHE_LINE_SIZE - 1U) & ~(AMBA_CACHE_LINE_SIZE - 1U);
}

uint32_t dsp_osal_cache_addr_align(uint32_t addr)
{
    return addr & ~(AMBA_CACHE_LINE_SIZE - 1U);
}
#endif

uint32_t dsp_osal_cache_clean(const void *ptr, uint32_t size)
{
    UINT32 Rval;
    ULONG VirtAddr;

    dsp_osal_typecast(&VirtAddr, &ptr);
    Rval = AmbaCache_DataClean(VirtAddr, size);

    return Rval;
}

uint32_t dsp_osal_cache_invalidate(const void *ptr, uint32_t size)
{
    UINT32 Rval;
    ULONG VirtAddr;

    dsp_osal_typecast(&VirtAddr, &ptr);
    Rval = AmbaCache_DataInvalidate(VirtAddr, size);

    return Rval;
}

#if 0
uint32_t dsp_osal_is_cached_addr(const ULONG Virt, uint32_t *pCached)
{
    UINT32 Rval = OK;
(void)Virt;
(void)pCached;
    return Rval;
}
#endif


#ifndef DSP_ATT_CLIENT_ID
#if defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_CV28) || defined (CONFIG_SOC_H32) || defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
#define DSP_ATT_CLIENT_ID   (AMBA_DRAM_CLIENT_ORCCODE)
#else
#define DSP_ATT_CLIENT_ID   (AMBA_DRAM_CLIENT_ORCCODE0)
#endif
#endif
uint32_t dsp_osal_virt2phys(const ULONG Virt, ULONG *pPhys)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pPhys == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_virt2phys : Null addr", 0U, 0U, 0U, 0U, 0U);
    } else if (Virt == 0UL) {
        *pPhys = 0U;
    } else {
#ifdef CONFIG_ARM32
        UINT32 U32Addr;
#else
        ULONG U64Addr;
#endif

#ifdef CONFIG_ARM32
        Rval = AmbaMMU_Virt32ToPhys32((UINT32)Virt, &U32Addr);
        *pPhys = (ULONG)U32Addr;
#else
        Rval = AmbaMMU_VirtToPhys((ULONG)Virt, &U64Addr);
        *pPhys = U64Addr;
#endif
    }

    return Rval;
}

uint32_t dsp_osal_phys2virt(const ULONG Phys, ULONG *pVirt)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pVirt == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_phys2virt : Null addr", 0U, 0U, 0U, 0U, 0U);
    } else if (Phys == 0UL) {
        *pVirt = 0U;
    } else {
#ifdef CONFIG_ARM32
        UINT32 U32Physic = (UINT32)Phys, U32PhysOutAddr = 0U;
#else
        ULONG PhysOutAddr = 0U;
#endif

#ifdef CONFIG_ARM32
        Rval = AmbaMMU_Phys32ToVirt32(U32Physic, &U32PhysOutAddr);
        if (Rval == ERR_NA) {
            U32PhysOutAddr = U32Physic;
        }
        *pVirt = (ULONG)U32PhysOutAddr;
        //dsp_osal_printU5("[P2V] CV 0x%08X -> P 0x%08X --> V 0x%08X", Phys, U32PhysAddr, *pVirt, 0U, 0U);
#else
        Rval = AmbaMMU_PhysToVirt(Phys, &PhysOutAddr);
        if (Rval == ERR_NA) {
            PhysOutAddr = Phys;
        }
        *pVirt = PhysOutAddr;
        //dsp_osal_printU5("[P2V] CV 0x%08X -> P 0x%08X --> V 0x%08X", Phys, PhysAddr, *pVirt, 0U, 0U);
#endif
    }
    return Rval;
}

uint32_t dsp_osal_virt2cli(const ULONG Virt, uint32_t *pCli)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pCli == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_virt2cli : Null addr", 0U, 0U, 0U, 0U, 0U);
    } else if (Virt == 0UL) {
        *pCli = 0U;
    } else {
#ifdef CONFIG_ARM32
        UINT32 U32Addr;
#endif
        ULONG U64Addr, U64CliAddr = 0UL;

#ifdef CONFIG_ARM32
        Rval = AmbaMMU_Virt32ToPhys32((UINT32)Virt, &U32Addr);
#else
        Rval = AmbaMMU_VirtToPhys(Virt, &U64Addr);
#endif

        if (Rval == OK) {
#ifdef CONFIG_ARM32
            U64Addr = (ULONG)U32Addr;
#endif
            /* Phys to cli virt */
            Rval = AmbaMemProt_QueryIntmdAddr(U64Addr, DSP_ATT_CLIENT_ID, &U64CliAddr);
            if (Rval == ERR_NA) {
                U64CliAddr = U64Addr;
            }
            //dsp_osal_printU5("[V2C] V 0x%X%08X -> P 0x%08X -> CV 0x%08X", dsp_osal_get_u64msb(Virt), dsp_osal_get_u64lsb(Virt), dsp_osal_get_u64msb(U64Addr), dsp_osal_get_u64msb(U64Addr), 0U);
            //dsp_osal_printU5("                              -> CV 0x%X%08X", dsp_osal_get_u64msb(U64CliAddr), dsp_osal_get_u64lsb(U64CliAddr), 0U, 0U, 0U);
        } else {
            //dsp_osal_printU5("[V2C]0x%08X Err[0x%08X]", (UINT32)Virt, Rval, 0U, 0U, 0U);
        }

        *pCli = (UINT32)U64CliAddr;
    }

    return Rval;

}

uint32_t dsp_osal_cli2virt(const uint32_t Cli, ULONG *pVirt)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pVirt == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_cli2virt : Null addr", 0U, 0U, 0U, 0U, 0U);
    } else if (Cli == 0UL) {
        *pVirt = 0U;
    } else {
#ifdef CONFIG_ARM32
        UINT32 U32PhysOutAddr = 0U;
#else
        ULONG U64PhysOutAddr = 0U;
#endif
        ULONG U64CliAddr = (ULONG)Cli, U64PhysAddr = 0U;

        Rval = AmbaMemProt_QueryPhysAddr(U64CliAddr, DSP_ATT_CLIENT_ID, &U64PhysAddr);
        if (Rval == ERR_NA) {
            U64PhysAddr = U64CliAddr;
        }

#ifdef CONFIG_ARM32
        Rval = AmbaMMU_Phys32ToVirt32((UINT32)U64PhysAddr, &U32PhysOutAddr);
        if (Rval == ERR_NA) {
            U32PhysOutAddr = (UINT32)U64PhysAddr;
        }
        *pVirt = (ULONG)U32PhysOutAddr;
        //dsp_osal_printU5("[C2V] CV 0x%08X -> P 0x%08X --> V 0x%08X", Cli, PhysAddr, *pVirt, 0U, 0U);
#else
        Rval = AmbaMMU_PhysToVirt(U64PhysAddr, &U64PhysOutAddr);
        if (Rval == ERR_NA) {
            U64PhysOutAddr = U64PhysAddr;
        }
        *pVirt = U64PhysOutAddr;
        //dsp_osal_printU5("[C2V] CV 0x%08X -> P 0x%08X --> V 0x%08X", Cli, PhysAddr, *pVirt, 0U, 0U);
#endif
    }
    return Rval;

}

#if 0
uint32_t dsp_osal_copy_to_user(void *to, const void *from, uint32_t n)
{
    UINT32 Rval = OK;
(void)to;
(void)from;
(void)n;
    return Rval;
}

uint32_t dsp_osal_copy_from_user(void *to, const void *from, uint32_t n)
{
    UINT32 Rval = OK;
(void)to;
(void)from;
(void)n;
    return Rval;
}
#endif

uint32_t dsp_osal_readl_relaxed(const void* reg)
{
    const volatile UINT32 *ptr;

    dsp_osal_typecast(&ptr, &reg);

    return *ptr;
}

void dsp_osal_writel_relaxed(uint32_t val, const void *reg)
{
    volatile UINT32 *ptr;

    dsp_osal_typecast(&ptr, &reg);

    *ptr = val;
}

#if 0
void *dsp_osal_ioremap(uint32_t pa, uint32_t size)
{
    void *ptr = NULL;

(void)pa;
(void)size;

    return ptr;
}

void dsp_osal_iounmap(void* va, uint32_t size)
{
(void)va;
(void)size;
}
#endif

uint32_t dsp_osal_irq_config(uint32_t irq_id, const osal_irq_t *irq_cfg, osal_isr_entry_f hdlr, uint32_t arg)
{
    AMBA_INT_CONFIG_s IntCfg = {0};
    AMBA_INT_ISR_f irq_hdlr;

    dsp_osal_typecast(&irq_hdlr, &hdlr);

    IntCfg.TriggerType = irq_cfg->trigger_type;
    IntCfg.IrqType = irq_cfg->irq_type;
    IntCfg.CpuTargets = (UINT8)irq_cfg->cpu_targets;

    return AmbaINT_Config(irq_id, &IntCfg, irq_hdlr, arg);
}

uint32_t dsp_osal_irq_enable(uint32_t irq_id)
{
    return AmbaINT_Enable(irq_id);
}

uint32_t dsp_osal_irq_disable(uint32_t irq_id)
{
    return AmbaINT_Disable(irq_id);
}

#if !defined (CONFIG_DEVICE_TREE_SUPPORT)
#if defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_H32) || defined (CONFIG_SOC_CV28)
#ifdef AMBA_INT_SPI_ID173_CODE_VDSP0_IRQ
#define IRQ_VDSP0   (AMBA_INT_SPI_ID173_CODE_VDSP0_IRQ)
#else
#define IRQ_VDSP0   (173)
#endif
#ifdef AMBA_INT_SPI_ID174_CODE_VDSP1_IRQ
#define IRQ_VDSP1   (AMBA_INT_SPI_ID174_CODE_VDSP1_IRQ)
#else
#define IRQ_VDSP1   (174)
#endif
#ifdef AMBA_INT_SPI_ID175_CODE_VDSP2_IRQ
#define IRQ_VDSP2   (AMBA_INT_SPI_ID175_CODE_VDSP2_IRQ)
#else
#define IRQ_VDSP2   (175)
#endif
#ifdef AMBA_INT_SPI_ID176_CODE_VDSP3_IRQ
#define IRQ_VDSP3   (AMBA_INT_SPI_ID176_CODE_VDSP3_IRQ)
#else
#define IRQ_VDSP3   (176)
#endif

#if defined (CONFIG_SOC_CV22)
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID120_VOUT_B_INT)
#else
#define IRQ_VOUT1    (120)
#endif

const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};

#elif defined (CONFIG_SOC_CV2)
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID163_PIP2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID163_PIP2_SOF)
#else
#define IRQ_VIN2_SOF    (163)
#endif
#ifdef AMBA_INT_SPI_ID164_PIP2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID164_PIP2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (164)
#endif
#ifdef AMBA_INT_SPI_ID158_PIP3_SOF
#define IRQ_VIN3_SOF    (AMBA_INT_SPI_ID158_PIP3_SOF)
#else
#define IRQ_VIN3_SOF    (158)
#endif
#ifdef AMBA_INT_SPI_ID159_PIP3_LAST_PIXEL
#define IRQ_VIN3_EOF    (AMBA_INT_SPI_ID159_PIP3_LAST_PIXEL)
#else
#define IRQ_VIN3_EOF    (159)
#endif
#ifdef AMBA_INT_SPI_ID153_PIP4_SOF
#define IRQ_VIN4_SOF    (AMBA_INT_SPI_ID153_PIP4_SOF)
#else
#define IRQ_VIN4_SOF    (153)
#endif
#ifdef AMBA_INT_SPI_ID154_PIP4_LAST_PIXEL
#define IRQ_VIN4_EOF    (AMBA_INT_SPI_ID154_PIP4_LAST_PIXEL)
#else
#define IRQ_VIN4_EOF    (154)
#endif
#ifdef AMBA_INT_SPI_ID148_PIP5_SOF
#define IRQ_VIN5_SOF    (AMBA_INT_SPI_ID148_PIP5_SOF)
#else
#define IRQ_VIN5_SOF    (148)
#endif
#ifdef AMBA_INT_SPI_ID149_PIP5_LAST_PIXEL
#define IRQ_VIN5_EOF    (AMBA_INT_SPI_ID149_PIP5_LAST_PIXEL)
#else
#define IRQ_VIN5_EOF    (149)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID120_VOUT_B_INT)
#else
#define IRQ_VOUT1    (120)
#endif
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
        [3U] = IRQ_VIN3_SOF,
        [4U] = IRQ_VIN4_SOF,
        [5U] = IRQ_VIN5_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
        [3U] = IRQ_VIN3_EOF,
        [4U] = IRQ_VIN4_EOF,
        [5U] = IRQ_VIN5_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};

#elif defined (CONFIG_SOC_CV25)
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID147_PIP2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID147_PIP2_SOF)
#else
#define IRQ_VIN2_SOF    (147)
#endif
#ifdef AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (148)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID120_VOUT_B_INT)
#else
#define IRQ_VOUT1    (120)
#endif
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};

#elif defined (CONFIG_SOC_H32)
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID147_PIP2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID147_PIP2_SOF)
#else
#define IRQ_VIN2_SOF    (147)
#endif
#ifdef AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (148)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID120_VOUT_B_INT)
#else
#define IRQ_VOUT1    (120)
#endif
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};
#else /* CV28 */
#ifdef AMBA_INT_SPI_ID143_VIN_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID143_VIN_SOF)
#else
#define IRQ_VIN0_SOF    (143)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID144_VIN_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID168_PIP_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID168_PIP_SOF)
#else
#define IRQ_VIN1_SOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID169_PIP_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID169_PIP_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (169)
#endif
#ifdef AMBA_INT_SPI_ID147_PIP2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID147_PIP2_SOF)
#else
#define IRQ_VIN2_SOF    (147)
#endif
#ifdef AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID148_PIP2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (148)
#endif
#ifdef AMBA_INT_SPI_ID121_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID121_VOUT_A_INT)
#else
#define IRQ_VOUT0    (121)
#endif
const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
    },
};
#endif
#elif defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
#ifdef AMBA_INT_SPI_ID177_CODE_VDSP0_IRQ
#define IRQ_VDSP0   (AMBA_INT_SPI_ID177_CODE_VDSP0_IRQ)
#else
#define IRQ_VDSP0   (177)
#endif
#ifdef AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ
#define IRQ_VDSP1   (AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ)
#else
#define IRQ_VDSP1   (178)
#endif
#ifdef AMBA_INT_SPI_ID179_CODE_VDSP2_IRQ
#define IRQ_VDSP2   (AMBA_INT_SPI_ID179_CODE_VDSP2_IRQ)
#else
#define IRQ_VDSP2   (179)
#endif
#ifdef AMBA_INT_SPI_ID180_CODE_VDSP3_IRQ
#define IRQ_VDSP3   (AMBA_INT_SPI_ID180_CODE_VDSP3_IRQ)
#else
#define IRQ_VDSP3   (180)
#endif

#ifdef AMBA_INT_SPI_ID139_VIN0_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID139_VIN0_SOF)
#else
#define IRQ_VIN0_SOF    (139)
#endif
#ifdef AMBA_INT_SPI_ID141_VIN0_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID141_VIN0_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (141)
#endif
#ifdef AMBA_INT_SPI_ID144_VIN1_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID144_VIN1_SOF)
#else
#define IRQ_VIN1_SOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID146_VIN1_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID146_VIN1_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (146)
#endif
#ifdef AMBA_INT_SPI_ID149_VIN2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID149_VIN2_SOF)
#else
#define IRQ_VIN2_SOF    (149)
#endif
#ifdef AMBA_INT_SPI_ID150_VIN2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID150_VIN2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (150)
#endif
#ifdef AMBA_INT_SPI_ID153_VIN3_SOF
#define IRQ_VIN3_SOF    (AMBA_INT_SPI_ID153_VIN3_SOF)
#else
#define IRQ_VIN3_SOF    (153)
#endif
#ifdef AMBA_INT_SPI_ID155_VIN3_LAST_PIXEL
#define IRQ_VIN3_EOF    (AMBA_INT_SPI_ID155_VIN3_LAST_PIXEL)
#else
#define IRQ_VIN3_EOF    (155)
#endif
#ifdef AMBA_INT_SPI_ID158_VIN4_SOF
#define IRQ_VIN4_SOF    (AMBA_INT_SPI_ID158_VIN4_SOF)
#else
#define IRQ_VIN4_SOF    (158)
#endif
#ifdef AMBA_INT_SPI_ID160_VIN4_LAST_PIXEL
#define IRQ_VIN4_EOF    (AMBA_INT_SPI_ID160_VIN4_LAST_PIXEL)
#else
#define IRQ_VIN4_EOF    (160)
#endif
#ifdef AMBA_INT_SPI_ID174_VIN8_SOF
#define IRQ_VIN5_SOF    (AMBA_INT_SPI_ID174_VIN8_SOF)
#else
#define IRQ_VIN5_SOF    (174)
#endif
#ifdef AMBA_INT_SPI_ID175_VIN8_LAST_PIXEL
#define IRQ_VIN5_EOF    (AMBA_INT_SPI_ID175_VIN8_LAST_PIXEL)
#else
#define IRQ_VIN5_EOF    (175)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID120_VOUT_A_INT)
#else
#define IRQ_VOUT0    (120)
#endif
#ifdef AMBA_INT_SPI_ID119_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID119_VOUT_B_INT)
#else
#define IRQ_VOUT1    (119)
#endif

const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
    },
    .VinSof = {
        [0U] = IRQ_VIN0_SOF,
        [1U] = IRQ_VIN1_SOF,
        [2U] = IRQ_VIN2_SOF,
        [3U] = IRQ_VIN3_SOF,
        [4U] = IRQ_VIN4_SOF,
        [5U] = IRQ_VIN5_SOF,
    },
    .VinEof = {
        [0U] = IRQ_VIN0_EOF,
        [1U] = IRQ_VIN1_EOF,
        [2U] = IRQ_VIN2_EOF,
        [3U] = IRQ_VIN3_EOF,
        [4U] = IRQ_VIN4_EOF,
        [5U] = IRQ_VIN5_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
    },
};
#else /*CV5/CV52*/
#ifdef AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ0
#define IRQ_VDSP0   (AMBA_INT_SPI_ID178_CODE_VDSP1_IRQ0)
#else
#define IRQ_VDSP0   (178)
#endif
#ifdef AMBA_INT_SPI_ID179_CODE_VDSP1_IRQ1
#define IRQ_VDSP1   (AMBA_INT_SPI_ID179_CODE_VDSP1_IRQ1)
#else
#define IRQ_VDSP1   (179)
#endif
#ifdef AMBA_INT_SPI_ID180_CODE_VDSP1_IRQ2
#define IRQ_VDSP2   (AMBA_INT_SPI_ID180_CODE_VDSP1_IRQ2)
#else
#define IRQ_VDSP2   (180)
#endif
#ifdef AMBA_INT_SPI_ID182_CODE_VDSP2_IRQ0
#define IRQ_VDSP3   (AMBA_INT_SPI_ID182_CODE_VDSP2_IRQ0)
#else
#define IRQ_VDSP3   (182)
#endif
#ifdef AMBA_INT_SPI_ID174_CODE_VDSP0_IRQ0
#define IRQ_VDSP4   (AMBA_INT_SPI_ID174_CODE_VDSP0_IRQ0)
#else
#define IRQ_VDSP4   (174)
#endif

#ifdef AMBA_INT_SPI_ID144_VIN0_SOF
#define IRQ_VIN0_SOF    (AMBA_INT_SPI_ID144_VIN0_SOF)
#else
#define IRQ_VIN0_SOF    (144)
#endif
#ifdef AMBA_INT_SPI_ID146_VIN0_LAST_PIXEL
#define IRQ_VIN0_EOF    (AMBA_INT_SPI_ID146_VIN0_LAST_PIXEL)
#else
#define IRQ_VIN0_EOF    (146)
#endif
#ifdef AMBA_INT_SPI_ID149_VIN1_SOF
#define IRQ_VIN1_SOF    (AMBA_INT_SPI_ID149_VIN1_SOF)
#else
#define IRQ_VIN1_SOF    (149)
#endif
#ifdef AMBA_INT_SPI_ID150_VIN1_LAST_PIXEL
#define IRQ_VIN1_EOF    (AMBA_INT_SPI_ID150_VIN1_LAST_PIXEL)
#else
#define IRQ_VIN1_EOF    (150)
#endif
#ifdef AMBA_INT_SPI_ID153_VIN2_SOF
#define IRQ_VIN2_SOF    (AMBA_INT_SPI_ID153_VIN2_SOF)
#else
#define IRQ_VIN2_SOF    (153)
#endif
#ifdef AMBA_INT_SPI_ID154_VIN2_LAST_PIXEL
#define IRQ_VIN2_EOF    (AMBA_INT_SPI_ID154_VIN2_LAST_PIXEL)
#else
#define IRQ_VIN2_EOF    (154)
#endif
#ifdef AMBA_INT_SPI_ID157_VIN3_SOF
#define IRQ_VIN3_SOF    (AMBA_INT_SPI_ID157_VIN3_SOF)
#else
#define IRQ_VIN3_SOF    (157)
#endif
#ifdef AMBA_INT_SPI_ID158_VIN3_LAST_PIXEL
#define IRQ_VIN3_EOF    (AMBA_INT_SPI_ID158_VIN3_LAST_PIXEL)
#else
#define IRQ_VIN3_EOF    (158)
#endif
#ifdef AMBA_INT_SPI_ID139_VIN4_SOF
#define IRQ_VIN4_SOF    (AMBA_INT_SPI_ID139_VIN4_SOF)
#else
#define IRQ_VIN4_SOF    (139)
#endif
#ifdef AMBA_INT_SPI_ID141_VIN4_LAST_PIXEL
#define IRQ_VIN4_EOF    (AMBA_INT_SPI_ID141_VIN4_LAST_PIXEL)
#else
#define IRQ_VIN4_EOF    (141)
#endif
#ifdef AMBA_INT_SPI_ID163_VIN5_SOF
#define IRQ_VIN5_SOF    (AMBA_INT_SPI_ID163_VIN5_SOF)
#else
#define IRQ_VIN5_SOF    (163)
#endif
#ifdef AMBA_INT_SPI_ID164_VIN5_LAST_PIXEL
#define IRQ_VIN5_EOF    (AMBA_INT_SPI_ID164_VIN5_LAST_PIXEL)
#else
#define IRQ_VIN5_EOF    (164)
#endif
#ifdef AMBA_INT_SPI_ID171_VIN7_SOF
#define IRQ_VIN6_SOF    (AMBA_INT_SPI_ID171_VIN7_SOF)
#else
#define IRQ_VIN6_SOF    (167)
#endif
#ifdef AMBA_INT_SPI_ID168_VIN6_LAST_PIXEL
#define IRQ_VIN6_EOF    (AMBA_INT_SPI_ID168_VIN6_LAST_PIXEL)
#else
#define IRQ_VIN6_EOF    (168)
#endif
#ifdef AMBA_INT_SPI_ID171_VIN7_SOF
#define IRQ_VIN7_SOF    (AMBA_INT_SPI_ID171_VIN7_SOF)
#else
#define IRQ_VIN7_SOF    (171)
#endif
#ifdef AMBA_INT_SPI_ID172_VIN7_LAST_PIXEL
#define IRQ_VIN7_EOF    (AMBA_INT_SPI_ID172_VIN7_LAST_PIXEL)
#else
#define IRQ_VIN7_EOF    (172)
#endif
#ifdef AMBA_INT_SPI_ID209_VIN8_SOF
#define IRQ_VIN8_SOF    (AMBA_INT_SPI_ID209_VIN8_SOF)
#else
#define IRQ_VIN8_SOF    (209)
#endif
#ifdef AMBA_INT_SPI_ID210_VIN8_LAST_PIXEL
#define IRQ_VIN8_EOF    (AMBA_INT_SPI_ID210_VIN8_LAST_PIXEL)
#else
#define IRQ_VIN8_EOF    (210)
#endif
#ifdef AMBA_INT_SPI_ID213_VIN9_SOF
#define IRQ_VIN9_SOF    (AMBA_INT_SPI_ID213_VIN9_SOF)
#else
#define IRQ_VIN9_SOF    (213)
#endif
#ifdef AMBA_INT_SPI_ID214_VIN9_LAST_PIXEL
#define IRQ_VIN9_EOF    (AMBA_INT_SPI_ID214_VIN9_LAST_PIXEL)
#else
#define IRQ_VIN9_EOF    (214)
#endif
#ifdef AMBA_INT_SPI_ID217_VIN10_SOF
#define IRQ_VIN10_SOF    (AMBA_INT_SPI_ID217_VIN10_SOF)
#else
#define IRQ_VIN10_SOF    (217)
#endif
#ifdef AMBA_INT_SPI_ID218_VIN10_LAST_PIXEL
#define IRQ_VIN10_EOF    (AMBA_INT_SPI_ID218_VIN10_LAST_PIXEL)
#else
#define IRQ_VIN10_EOF    (218)
#endif
#ifdef AMBA_INT_SPI_ID221_VIN11_SOF
#define IRQ_VIN11_SOF    (AMBA_INT_SPI_ID221_VIN11_SOF)
#else
#define IRQ_VIN11_SOF    (221)
#endif
#ifdef AMBA_INT_SPI_ID222_VIN11_LAST_PIXEL
#define IRQ_VIN11_EOF    (AMBA_INT_SPI_ID222_VIN11_LAST_PIXEL)
#else
#define IRQ_VIN11_EOF    (222)
#endif
#ifdef AMBA_INT_SPI_ID225_VIN12_SOF
#define IRQ_VIN12_SOF    (AMBA_INT_SPI_ID225_VIN12_SOF)
#else
#define IRQ_VIN12_SOF    (225)
#endif
#ifdef AMBA_INT_SPI_ID226_VIN12_LAST_PIXEL
#define IRQ_VIN12_EOF    (AMBA_INT_SPI_ID226_VIN12_LAST_PIXEL)
#else
#define IRQ_VIN12_EOF    (226)
#endif
#ifdef AMBA_INT_SPI_ID229_VIN13_SOF
#define IRQ_VIN13_SOF    (AMBA_INT_SPI_ID229_VIN13_SOF)
#else
#define IRQ_VIN13_SOF    (229)
#endif
#ifdef AMBA_INT_SPI_ID230_VIN13_LAST_PIXEL
#define IRQ_VIN13_EOF    (AMBA_INT_SPI_ID230_VIN13_LAST_PIXEL)
#else
#define IRQ_VIN13_EOF    (230)
#endif
#ifdef AMBA_INT_SPI_ID120_VOUT_A_INT
#define IRQ_VOUT0    (AMBA_INT_SPI_ID120_VOUT_A_INT)
#else
#define IRQ_VOUT0    (120)
#endif
#ifdef AMBA_INT_SPI_ID119_VOUT_B_INT
#define IRQ_VOUT1    (AMBA_INT_SPI_ID119_VOUT_B_INT)
#else
#define IRQ_VOUT1    (119)
#endif
#ifdef AMBA_INT_SPI_ID241_VOUT_C_INT
#define IRQ_VOUT2    (AMBA_INT_SPI_ID241_VOUT_C_INT)
#else
#define IRQ_VOUT2    (241)
#endif

const osal_irq_id_t osal_irq_id = {
    .DspCode = {
        [0U] = IRQ_VDSP0,
        [1U] = IRQ_VDSP1,
        [2U] = IRQ_VDSP2,
        [3U] = IRQ_VDSP3,
        [4U] = IRQ_VDSP4,
    },
    .VinSof = {
        [0U]  = IRQ_VIN0_SOF,
        [1U]  = IRQ_VIN1_SOF,
        [2U]  = IRQ_VIN2_SOF,
        [3U]  = IRQ_VIN3_SOF,
        [4U]  = IRQ_VIN4_SOF,
        [5U]  = IRQ_VIN5_SOF,
        [6U]  = IRQ_VIN6_SOF,
        [7U]  = IRQ_VIN7_SOF,
        [8U]  = IRQ_VIN8_SOF,
        [9U]  = IRQ_VIN9_SOF,
        [10U] = IRQ_VIN10_SOF,
        [11U] = IRQ_VIN11_SOF,
        [12U] = IRQ_VIN12_SOF,
        [13U] = IRQ_VIN13_SOF,
    },
    .VinEof = {
        [0U]  = IRQ_VIN0_EOF,
        [1U]  = IRQ_VIN1_EOF,
        [2U]  = IRQ_VIN2_EOF,
        [3U]  = IRQ_VIN3_EOF,
        [4U]  = IRQ_VIN4_EOF,
        [5U]  = IRQ_VIN5_EOF,
        [6U]  = IRQ_VIN6_EOF,
        [7U]  = IRQ_VIN7_EOF,
        [8U]  = IRQ_VIN8_EOF,
        [9U]  = IRQ_VIN9_EOF,
        [10U] = IRQ_VIN10_EOF,
        [11U] = IRQ_VIN11_EOF,
        [12U] = IRQ_VIN12_EOF,
        [13U] = IRQ_VIN13_EOF,
    },
    .Vout = {
        [0U] = IRQ_VOUT0,
        [1U] = IRQ_VOUT1,
        [2U] = IRQ_VOUT2,
    },
};
#endif
#else
#define AMBA_INT_OFFSET     (32U)
static osal_irq_id_t osal_irq_id = {0};
#endif

#if defined (CONFIG_DEVICE_TREE_SUPPORT)
#define DTS_DSP_NODE        "/dspdrv"
#define DTS_DSP_VIN_NODE    "/dspdrv/dspvin"
#define DTS_DSP_VOUT_NODE   "/dspdrv/dspvout"

static inline uint32_t dsp_osal_irq_code_id_init(const void *pfdt)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t fdt_ret = 0;
    int32_t offset = 0;
    const struct fdt_property *prop = NULL;
    int32_t len = 0;
    const char *chr1;
    const UINT32 *ptr1;
    uint32_t i, idx;

    fdt_ret = AmbaFDT_PathOffset(pfdt, DTS_DSP_NODE);
    if (fdt_ret < 0) {
        dsp_osal_printU5("[ERR] dsp_osal_irq_code_id_init : AmbaFDT_PathOffset[DSP_NODE] fail[0x%X]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        offset = fdt_ret;
        prop = AmbaFDT_GetProperty(pfdt, offset, "interrupts", &len);
        if ((prop != NULL) && (len > 4)) {
            chr1 = (const char *)&prop->data[0];
            dsp_osal_typecast(&ptr1, &chr1);
            // interrupts = <0 145 0x1 0 146 0x1 0 147 0x1 0 148 0x1>
            for (i = 0U; i < DSP_MAX_IRQ_CODE_NUM; i++) {
                idx = (3U*i) + 1U;
                osal_irq_id.DspCode[i] = AmbaFDT_Fdt32ToCpu(ptr1[idx]);
                osal_irq_id.DspCode[i] += AMBA_INT_OFFSET;
            }
            //dsp_osal_printU5("dsp_osal_irq_code_id_init : Code[%d %d %d %d]", osal_irq_id.DspCode[0U], osal_irq_id.DspCode[1U], osal_irq_id.DspCode[2U], osal_irq_id.DspCode[3U], 0U);
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_irq_code_id_init : AmbaFDT_GetProperty[DSP_NODE] fail", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
}

static inline uint32_t dsp_osal_irq_vin_id_init(const void *pfdt)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t fdt_ret = 0;
    int32_t offset = 0;
    const struct fdt_property *prop = NULL;
    int32_t len = 0;
    const char *chr1;
    const UINT32 *ptr1;
    uint32_t i;
#define NODE_NAME_LEN   (32U)
    char node_name[NODE_NAME_LEN];

    for (i = 0U; i < DSP_MAX_IRQ_VIN_NUM; i++) {
        (void)dsp_osal_str_copy(node_name, NODE_NAME_LEN, DTS_DSP_VIN_NODE);
        (void)dsp_osal_str_append_u32(node_name, NODE_NAME_LEN, i, 10U);
        fdt_ret = AmbaFDT_PathOffset(pfdt, node_name);
        if (fdt_ret < 0) {
            dsp_osal_printU5("[ERR] dsp_osal_irq_vin_id_init : AmbaFDT_PathOffset[DSP_VIN_NODE %d] fail[%d]", i, (uint32_t)fdt_ret, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            offset = fdt_ret;
            prop = AmbaFDT_GetProperty(pfdt, offset, "interrupts", &len);
            if ((prop != NULL) && (len > 4)) {
                chr1 = (const char *)&prop->data[0];
                dsp_osal_typecast(&ptr1, &chr1);
                // interrupts = <0 107 0x1 0 109 0x1>
                osal_irq_id.VinSof[i] = AmbaFDT_Fdt32ToCpu(ptr1[1U]);
                osal_irq_id.VinSof[i] += AMBA_INT_OFFSET;
                osal_irq_id.VinEof[i] = AmbaFDT_Fdt32ToCpu(ptr1[4U]);
                osal_irq_id.VinEof[i] += AMBA_INT_OFFSET;
                //dsp_osal_printU5("dsp_osal_irq_vin_id_init : Vin%d[%d %d]", i, osal_irq_id.VinSof[i], osal_irq_id.VinEof[i], 0U, 0U);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_irq_vin_id_init : AmbaFDT_GetProperty[DSP_VIN_NODE %d] fail", i, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }

        if (Rval != DSP_ERR_NONE) {
            break;
        }
    }

    return Rval;
}

static inline uint32_t dsp_osal_irq_vout_id_init(const void *pfdt)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t fdt_ret = 0;
    int32_t offset = 0;
    const struct fdt_property *prop = NULL;
    int32_t len = 0;
    const char *chr1;
    const UINT32 *ptr1;
    uint32_t i;
#define NODE_NAME_LEN   (32U)
    char node_name[NODE_NAME_LEN];

    for (i = 0U; i < DSP_MAX_IRQ_VOUT_NUM; i++) {
        (void)dsp_osal_str_copy(node_name, NODE_NAME_LEN, DTS_DSP_VOUT_NODE);
        (void)dsp_osal_str_append_u32(node_name, NODE_NAME_LEN, i, 10U);
        fdt_ret = AmbaFDT_PathOffset(pfdt, node_name);
        if (fdt_ret < 0) {
            dsp_osal_printU5("[ERR] dsp_osal_irq_vout_id_init : AmbaFDT_PathOffset[DSP_VOUT_NODE %d] fail[%d]", i, (uint32_t)fdt_ret, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            offset = fdt_ret;
            prop = AmbaFDT_GetProperty(pfdt, offset, "interrupts", &len);
            if ((prop != NULL) && (len > 4)) {
                chr1 = (const char *)&prop->data[0];
                dsp_osal_typecast(&ptr1, &chr1);
                // interrupts = <0 88 0x1>
                osal_irq_id.Vout[i] = AmbaFDT_Fdt32ToCpu(ptr1[1U]);
                osal_irq_id.Vout[i] += AMBA_INT_OFFSET;
                //dsp_osal_printU5("dsp_osal_irq_vout_id_init : Vout%d[%d]", i, osal_irq_id.Vout[i], 0U, 0U, 0U);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_irq_vout_id_init : AmbaFDT_GetProperty[DSP_VOUT_NODE %d] fail", i, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }

        if (Rval != DSP_ERR_NONE) {
            break;
        }
    }

    return Rval;
}

static inline uint32_t dsp_osal_irq_id_init(void)
{
static uint32_t osal_irq_id_inited = 0U;
    uint32_t Rval = DSP_ERR_NONE;

    if (osal_irq_id_inited == 0U) {
#if defined (CONFIG_DTB_LOADADDR)
        int32_t fdt_ret = 0;
        const void *pfdt = NULL;
#if defined (CONFIG_ARM64)
        const UINT64 dtb_addr = (UINT64)CONFIG_DTB_LOADADDR;
#else
        const UINT32 dtb_addr = (UINT32)CONFIG_DTB_LOADADDR;
#endif

        //dsp_osal_printU5("dsp_osal_irq_id_init : dtb addr 0x%X", dtb_addr, 0U, 0U, 0U, 0U);
        dsp_osal_typecast(&pfdt, &dtb_addr);
        fdt_ret = AmbaFDT_CheckHeader(pfdt);
        if (fdt_ret != 0) {
            dsp_osal_printU5("[ERR] dsp_osal_irq_id_init : AmbaFDT_CheckHeader fail[0x%X]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            /* Get CODE interrupt */
            Rval = dsp_osal_irq_code_id_init(pfdt);

            /* Get VIN interrupt */
            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_osal_irq_vin_id_init(pfdt);
            }

            /* Get VOUT interrupt */
            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_osal_irq_vout_id_init(pfdt);
            }
        }

        if (Rval == DSP_ERR_NONE) {
            osal_irq_id_inited = 1U;
        }
#else
        dsp_osal_printU5("[ERR] dsp_osal_irq_id_init : Null dtb address", 0U, 0U, 0U, 0U, 0U);
#endif
    }

    return Rval;
}
#endif

uint32_t dsp_osal_irq_id_query(uint32_t type, uint32_t idx, uint32_t *irq_id)
{
    uint32_t Rval = DSP_ERR_NONE;

#if defined (CONFIG_DEVICE_TREE_SUPPORT)
    Rval = dsp_osal_irq_id_init();
#endif

    if (type >= NUM_DSP_IRQ_TYPE) {
        dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : Invalid type[%d]", type, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else if (irq_id == NULL) {
        dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : Null input", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0000;
    } else {
        if (type == DSP_IRQ_TYPE_VIN_SOF) {
            if (idx >= DSP_MAX_IRQ_VIN_NUM) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : [%d]Invalid idx[%d]", type, idx, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                *irq_id = osal_irq_id.VinSof[idx];
            }
        } else if (type == DSP_IRQ_TYPE_VIN_EOF) {
            if (idx >= DSP_MAX_IRQ_VIN_NUM) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : [%d]Invalid idx[%d]", type, idx, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                *irq_id = osal_irq_id.VinEof[idx];
            }
        } else if (type == DSP_IRQ_TYPE_VOUT) {
            if (idx >= DSP_MAX_IRQ_VOUT_NUM) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : [%d]Invalid idx[%d]", type, idx, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                *irq_id = osal_irq_id.Vout[idx];
            }
        } else {
            if (idx >= DSP_MAX_IRQ_CODE_NUM) {
                dsp_osal_printU5("[ERR] dsp_osal_irq_id_query : [%d]Invalid idx[%d]", type, idx, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            } else {
                *irq_id = osal_irq_id.DspCode[idx];
            }
        }
    }

    return Rval;
}

#if 0
uint32_t dsp_osal_cma_alloc(void *arg, uint64_t owner)
{
(void)arg;
(void)owner;
    dsp_osal_printU5("[ERR] dsp_osal_cma_alloc : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003/*DSP_ERR_CMA_OPS*/;
}

uint32_t dsp_osal_cma_free(void *arg)
{
(void)arg;
    dsp_osal_printU5("[ERR] dsp_osal_cma_free : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003/*DSP_ERR_CMA_OPS*/;
}

uint32_t dsp_osal_cma_sync_cache(void *arg)
{
(void) arg;
    dsp_osal_printU5("[ERR] dsp_osal_cma_sync_cache : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003/*DSP_ERR_CMA_OPS*/;
}

uint32_t dsp_osal_cma_get_usage(void *arg)
{
(void)arg;
    dsp_osal_printU5("[ERR] dsp_osal_cma_get_usage : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003/*DSP_ERR_CMA_OPS*/;
}

uint32_t dsp_osal_cma_p2v(uint32_t phy, uint32_t *pCacheFlag, ULONG *pAddr)
{
(void)phy;
(void)pCacheFlag;
(void)pAddr;
    dsp_osal_printU5("[ERR] dsp_osal_cma_p2v : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003/*DSP_ERR_CMA_OPS*/;
}

uint32_t dsp_osal_cma_v2p(ULONG virt, uint32_t *pCacheFlag)
{
(void)virt;
(void)pCacheFlag;
    dsp_osal_printU5("[ERR] dsp_osal_cma_v2p : not support", 0U, 0U, 0U, 0U, 0U);
    return DSP_ERR_0003/*DSP_ERR_CMA_OPS*/;
}
#endif

/******************************************************************************/
/*                                  DSP                                       */
/******************************************************************************/
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
void dsp_osal_safety_init(void)
{
    const void *pVoid;
#if defined (CONFIG_ARM64)
    UINT64 SmemInitAddr = 0xED05E010U;
    UINT64 SmemInitData = 0x808080U;
#else
    UINT32 SmemInitAddr = 0xED05E010U;
    UINT32 SmemInitData = 0x808080U;
#endif

    /* Init SMEM via Sydney Reader */
    dsp_osal_typecast(&pVoid, &SmemInitAddr);
    dsp_osal_writel_relaxed(SmemInitData, pVoid);
    dsp_osal_printU5("[DSP_INIT]SmemInit 0x%X @ 0x%X", SmemInitAddr, SmemInitData, 0U, 0U, 0U);
}
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
void dsp_osal_orc_set_pc(const osal_ucode_addr_t *pBasePhysAddr, const uint8_t IdspBinShareMode)
#else
void dsp_osal_orc_set_pc(const osal_ucode_addr_t *pBasePhysAddr)
#endif
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (pBasePhysAddr->CodeAddr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(0, (UINT32)pBasePhysAddr->CodeAddr);
    }
    if (pBasePhysAddr->Code1Addr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(1, (UINT32)pBasePhysAddr->Code1Addr);
    }
    if (pBasePhysAddr->Code2Addr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(2, (UINT32)pBasePhysAddr->Code2Addr);
    }
    if (IdspBinShareMode == 1U) {
        if (pBasePhysAddr->Code2Addr != 0U) {
            AmbaCSL_DebugPortSetOrcPC(3, (UINT32)pBasePhysAddr->Code2Addr);
        }
    } else {
        if (pBasePhysAddr->Code3Addr != 0U) {
            AmbaCSL_DebugPortSetOrcPC(3, (UINT32)pBasePhysAddr->Code3Addr);
        }
    }

    if (pBasePhysAddr->MeAddr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(4, (UINT32)pBasePhysAddr->MeAddr);
    }
    if (pBasePhysAddr->Me1Addr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(5, (UINT32)pBasePhysAddr->Me1Addr);
    }

    if (pBasePhysAddr->MdxfAddr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(6, (UINT32)pBasePhysAddr->MdxfAddr);
    }
    if (pBasePhysAddr->Mdxf1Addr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(7, (UINT32)pBasePhysAddr->Mdxf1Addr);
    }
#else
    if (pBasePhysAddr->CodeAddr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(0, (UINT32)pBasePhysAddr->CodeAddr);
    }

    if (pBasePhysAddr->MeAddr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(1, (UINT32)pBasePhysAddr->MeAddr);
    }

    if (pBasePhysAddr->MdxfAddr != 0U) {
        AmbaCSL_DebugPortSetOrcPC(2, (UINT32)pBasePhysAddr->MdxfAddr);
    }
#endif
}

void dsp_osal_orc_enable(UINT32 OrcMask)
{
#define ORCODE_RESET_VALUE  (0xFFU)
#define ORCME_RESET_VALUE   (0xFU)
#define ORCMDXF_RESET_VALUE (0xFU)

    /* Reset DORC/EORC first to prevent CORC trying to access them */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_ME0_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_ME0_IDX/*orcme0*/, ORCME_RESET_VALUE);
    }
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_ME1_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_ME1_IDX/*orcme1*/, ORCME_RESET_VALUE);
    }
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_MDXF0_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_MDXF0_IDX/*orcmdxf0*/, ORCMDXF_RESET_VALUE);
    }
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_MDXF1_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_MDXF1_IDX/*orcmdxf1*/, ORCMDXF_RESET_VALUE);
    }

    /* per ChenHan's suggest on 2021/6/1 */
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_CODE_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_CODE_IDX/*orccode*/, ORCODE_RESET_VALUE);
    }

    if (0U == DSP_GetBit(OrcMask, DSP_ORC_IDSP1_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_IDSP1_IDX/*orcidsp1*/, ORCODE_RESET_VALUE);
    }
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_IDSP0_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_IDSP0_IDX/*orcidsp0*/, ORCODE_RESET_VALUE);
    }
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_VIN_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_VIN_IDX/*orcvin*/, ORCODE_RESET_VALUE);
    }
#else
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_ME0_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_ME0_IDX/*orcme*/, ORCME_RESET_VALUE);
    }
    if (0U == DSP_GetBit(OrcMask, DSP_ORC_MDXF0_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_MDXF0_IDX/*orcmdxf*/, ORCMDXF_RESET_VALUE);
    }

    if (0U == DSP_GetBit(OrcMask, DSP_ORC_CODE_IDX, 1U)) {
        AmbaCSL_DebugPortEnableORC(DSP_ORC_CODE_IDX/*orccode*/, ORCODE_RESET_VALUE);
    }
#endif
}

void dsp_osal_orc_shutdown(void)
{
#define ORCODE_SHUTDOWN_VALUE   (0xFF00U)
#define ORCME_SHUTDOWN_VALUE    (0xF0U)
#define ORCMDXF_SHUTDOWN_VALUE  (0xF0U)

    /* Reset DORC/EORC first to prevent CORC trying to access them */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    AmbaCSL_DebugPortEnableORC(DSP_ORC_ME0_IDX/*orcme0*/, ORCME_SHUTDOWN_VALUE);
    AmbaCSL_DebugPortEnableORC(DSP_ORC_ME1_IDX/*orcme1*/, ORCME_SHUTDOWN_VALUE);
    AmbaCSL_DebugPortEnableORC(DSP_ORC_MDXF0_IDX/*orcmdxf0*/, ORCMDXF_SHUTDOWN_VALUE);
    AmbaCSL_DebugPortEnableORC(DSP_ORC_MDXF1_IDX/*orcmdxf1*/, ORCMDXF_SHUTDOWN_VALUE);

    /* per ChenHan's suggest on 2021/6/1 */
    AmbaCSL_DebugPortEnableORC(DSP_ORC_CODE_IDX/*orccode*/, ORCODE_SHUTDOWN_VALUE);
    AmbaCSL_DebugPortEnableORC(DSP_ORC_IDSP1_IDX/*orcidsp1*/, ORCODE_SHUTDOWN_VALUE);
    AmbaCSL_DebugPortEnableORC(DSP_ORC_IDSP0_IDX/*orcidsp0*/, ORCODE_SHUTDOWN_VALUE);
    AmbaCSL_DebugPortEnableORC(DSP_ORC_VIN_IDX/*orcvin*/, ORCODE_SHUTDOWN_VALUE);
#elif defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_CV28) || defined (CONFIG_SOC_H32)
    AmbaCSL_DebugPortEnableORC(DSP_ORC_ME0_IDX, ORCME_SHUTDOWN_VALUE);
    AmbaCSL_DebugPortEnableORC(DSP_ORC_MDXF0_IDX, ORCMDXF_SHUTDOWN_VALUE);
    AmbaCSL_DebugPortEnableORC(DSP_ORC_CODE_IDX, ORCODE_SHUTDOWN_VALUE);
#else
    // TBD
#endif
}

void dsp_osal_dbgport_set_sync_cnt(uint32_t sync_cnt_id, uint32_t val)
{
    AmbaCSL_DebugPortSetSyncCounter(sync_cnt_id, val);
}

void dsp_osal_dbgport_set_smem_grp(void)
{
    const void *pVoid;
#if defined (CONFIG_ARM64)
    UINT64 SmemGrpAddr = 0xED05E004U;
    UINT64 SmemGrpData = 0x83000002U; // b[31] for smem grouping
#else
    UINT32 SmemGrpAddr = 0xED05E004U;
    UINT32 SmemGrpData = 0x83000002U; // b[31] for smem grouping
#endif
    dsp_osal_typecast(&pVoid, &SmemGrpAddr);
    dsp_osal_writel_relaxed((UINT32)SmemGrpData, pVoid);
    dsp_osal_printU5("[DSP_INIT]SmemGrp", 0U, 0U, 0U, 0U, 0U);
}

uint32_t dsp_osal_get_orc_timer(void)
{
    return AmbaRTSL_GetOrcTimer();
}

uint32_t dsp_osal_get_dram_page(void)
{
#if defined (CONFIG_ARM64)
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
    const UINT64 DramCAddr = 0xDFFE1004U;
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    const UINT64 DramCAddr = 0x1000000000U;
#else
    const UINT64 DramCAddr = 0xDFFE0804U;
#endif
#else
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
    const UINT32 DramCAddr = 0xDFFE1004U;
#else
    const UINT32 DramCAddr = 0xDFFE0804U;
#endif
#endif
    const void *pVoid;

    dsp_osal_typecast(&pVoid, &DramCAddr);
    return dsp_osal_readl_relaxed(pVoid);
}

void dsp_osal_get_cmd_req_timeout_cfg(uint32_t *pEnable, uint32_t *pTime)
{
    if ((pEnable == NULL) ||
        (pTime == NULL)) {
        dsp_osal_printU5("[ERR] dsp_osal_get_cmd_req_timeout_cfg : Null input", 0U, 0U, 0U, 0U, 0U);
    } else {
#define DSP_CMD_REQ_TIMEOUT_CHECK   (0U) // cmd-req-msg has timeout information, disable assertion
#define DSP_CMD_REQ_TIME            (4U) // sync counter timer in ms. ( only 1 ~ 4 ms allowed )
        *pEnable = DSP_CMD_REQ_TIMEOUT_CHECK;
        *pTime = DSP_CMD_REQ_TIME;
    }
}

static osal_buf_t ucode_bin_buf[UCODE_BIN_NUM] = {0};

#if defined (CONFIG_DEVICE_TREE_SUPPORT)

uint32_t dsp_osal_alloc_ucode_addr(void);
uint32_t dsp_osal_alloc_ucode_addr(void)
{
#if defined (CONFIG_DTB_LOADADDR)
#define DTS_DSP_BIN_NODE        "/reserved-memory/dsp_bin_buf"
    UINT32 Rval = DSP_ERR_NONE;
    INT32 fdt_ret = 0, offset = 0;
    const void *pfdt = NULL;
    INT32 len;
    const char *chr1;
    const UINT32 *ptr1;
    const UINT64 *ptr64;
    const struct fdt_property *prop = NULL;
    ULONG PhysAddr = 0U;
    UINT32 Size = 0U;
    ULONG VirtAddr;
    ULONG BaseStartVirtAddr, StartVirtAddr;
    UINT32 MemOffset = 0U;
    UINT32 UsedSizeOffset = 0U, UsedSize = 0U;
#if defined (CONFIG_ARM64)
    const UINT64 dtb_addr = (UINT64)CONFIG_DTB_LOADADDR;
#else
    const UINT32 dtb_addr = (UINT32)CONFIG_DTB_LOADADDR;
#endif
    static const uint32_t ucode_bin_size[UCODE_BIN_NUM] = {
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        [UCODE_BIN_ORCODE]  = ORCODE_BIN_MAX_SIZE,
        [UCODE_BIN_ORCODE1] = ORCODE1_BIN_MAX_SIZE,
        [UCODE_BIN_ORCODE2] = ORCODE2_BIN_MAX_SIZE,
        [UCODE_BIN_ORCODE3] = ORCODE3_BIN_MAX_SIZE,
        [UCODE_BIN_ORCME]   = ORCME_BIN_MAX_SIZE,
        [UCODE_BIN_ORCME1]  = ORCME_BIN_MAX_SIZE,
        [UCODE_BIN_ORCMD]   = ORCMD_BIN_MAX_SIZE,
        [UCODE_BIN_ORCMD1]  = ORCMD_BIN_MAX_SIZE,
        [UCODE_BIN_DEFAULT] = DEFAULT_BIN_MAX_SIZE,
#else
        [UCODE_BIN_ORCODE]  = ORCODE_BIN_MAX_SIZE,
        [UCODE_BIN_ORCME]   = ORCME_BIN_MAX_SIZE,
        [UCODE_BIN_ORCMD]   = ORCMD_BIN_MAX_SIZE,
        [UCODE_BIN_DEFAULT] = DEFAULT_BIN_MAX_SIZE,
#endif
    };

    //dsp_osal_printU5("dsp_osal_alloc_ucode_addr : dtb addr 0x%X", dtb_addr, 0U, 0U, 0U, 0U);
    dsp_osal_typecast(&pfdt, &dtb_addr);
    fdt_ret = AmbaFDT_CheckHeader(pfdt);
    if (fdt_ret != 0) {
        dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr : AmbaFDT_CheckHeader fail[0x%X]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        fdt_ret = AmbaFDT_PathOffset(pfdt, DTS_DSP_BIN_NODE);
        if (fdt_ret < 0) {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr : AmbaFDT_PathOffset[DTS_DSP_BIN_NODE] fail[%d]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            offset = fdt_ret;
            prop = AmbaFDT_GetProperty(pfdt, offset, "reg", &len);
            if ((prop != NULL) && (len > 0)) {
                chr1 = (const char *)&prop->data[0];
                // reg <base size>
                if (len == 8) {
                    // 32bit per cell
                    dsp_osal_typecast(&ptr1, &chr1);
                    PhysAddr = AmbaFDT_Fdt32ToCpu(ptr1[0U]);
                    Size = AmbaFDT_Fdt32ToCpu(ptr1[1U]);
                } else if (len == 16) {
                    // 64bit per cell
                    dsp_osal_typecast(&ptr64, &chr1);
                    PhysAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0U]);
                    Size = (UINT32)AmbaFDT_Fdt64ToCpu(ptr64[1U]);
                } else {
                    // unknown
                }

                (void)dsp_osal_phys2virt(PhysAddr, &VirtAddr);
                osal_buf_pool[DSP_BUF_POOL_BINARY].Base = VirtAddr;
                osal_buf_pool[DSP_BUF_POOL_BINARY].size = Size;
                osal_buf_pool[DSP_BUF_POOL_BINARY].UsedSize = 0U;
                osal_buf_pool[DSP_BUF_POOL_BINARY].is_cached = 1U;
                osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase = PhysAddr;
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr : AmbaFDT_GetProperty[DTS_DSP_BIN_NODE] fail", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }

        dsp_osal_printU5("[dspdev] FDT mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                          DSP_BUF_POOL_BINARY,
                          dsp_osal_get_u64msb(osal_buf_pool[DSP_BUF_POOL_BINARY].Base),
                          dsp_osal_get_u64lsb(osal_buf_pool[DSP_BUF_POOL_BINARY].Base),
                          dsp_osal_get_u64msb(osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase),
                          dsp_osal_get_u64lsb(osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase));
        dsp_osal_printU5("                         Sz %d Cached %d",
                          osal_buf_pool[DSP_BUF_POOL_BINARY].size,
                          osal_buf_pool[DSP_BUF_POOL_BINARY].is_cached, 0U, 0U, 0U);

        if (Rval == DSP_ERR_NONE) {
            dsp_osal_typecast(&BaseStartVirtAddr, &VirtAddr);
            StartVirtAddr = ADDR_ALIGN_NUM(BaseStartVirtAddr, DSP_BUF_ADDR_ALIGN);
            MemOffset = (UINT32)(StartVirtAddr - BaseStartVirtAddr);
            UsedSizeOffset += MemOffset;

            /* Allocate orcode */
            ucode_bin_buf[UCODE_BIN_ORCODE].Base = StartVirtAddr;
            ucode_bin_buf[UCODE_BIN_ORCODE].size = ucode_bin_size[UCODE_BIN_ORCODE];
            ucode_bin_buf[UCODE_BIN_ORCODE].pool_id = DSP_BUF_POOL_BINARY;
            UsedSize += ucode_bin_size[UCODE_BIN_ORCODE];
            UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            /* Allocate orcode1 */
            if ((UsedSizeOffset + UsedSize) < Size) {
                ucode_bin_buf[UCODE_BIN_ORCODE1].Base = StartVirtAddr + UsedSize;
                ucode_bin_buf[UCODE_BIN_ORCODE1].size = ucode_bin_size[UCODE_BIN_ORCODE1];
                ucode_bin_buf[UCODE_BIN_ORCODE1].pool_id = DSP_BUF_POOL_BINARY;
                UsedSize += ucode_bin_size[UCODE_BIN_ORCODE1];
                UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }

            /* Allocate orcode2 */
            if ((UsedSizeOffset + UsedSize) < Size) {
                ucode_bin_buf[UCODE_BIN_ORCODE2].Base = StartVirtAddr + UsedSize;
                ucode_bin_buf[UCODE_BIN_ORCODE2].size = ucode_bin_size[UCODE_BIN_ORCODE2];
                ucode_bin_buf[UCODE_BIN_ORCODE2].pool_id = DSP_BUF_POOL_BINARY;
                UsedSize += ucode_bin_size[UCODE_BIN_ORCODE2];
                UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }

            /* Allocate orcode3 */
            if ((UsedSizeOffset + UsedSize) < Size) {
                ucode_bin_buf[UCODE_BIN_ORCODE3].Base = StartVirtAddr + UsedSize;
                ucode_bin_buf[UCODE_BIN_ORCODE3].size = ucode_bin_size[UCODE_BIN_ORCODE3];
                ucode_bin_buf[UCODE_BIN_ORCODE3].pool_id = DSP_BUF_POOL_BINARY;
                UsedSize += ucode_bin_size[UCODE_BIN_ORCODE3];
                UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
#endif


            /* Allocate orcme */
            if ((UsedSizeOffset + UsedSize) < Size) {
                ucode_bin_buf[UCODE_BIN_ORCME].Base = StartVirtAddr + UsedSize;
                ucode_bin_buf[UCODE_BIN_ORCME].size = ucode_bin_size[UCODE_BIN_ORCME];
                ucode_bin_buf[UCODE_BIN_ORCME].pool_id = DSP_BUF_POOL_BINARY;
                UsedSize += ucode_bin_size[UCODE_BIN_ORCME];
                UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            /* Allocate orcme1 */
            if ((UsedSizeOffset + UsedSize) < Size) {
                ucode_bin_buf[UCODE_BIN_ORCME1].Base = StartVirtAddr + UsedSize;
                ucode_bin_buf[UCODE_BIN_ORCME1].size = ucode_bin_size[UCODE_BIN_ORCME1];
                ucode_bin_buf[UCODE_BIN_ORCME1].pool_id = DSP_BUF_POOL_BINARY;
                UsedSize += ucode_bin_size[UCODE_BIN_ORCME1];
                UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
#endif

            /* Allocate orcmd */
            if ((Rval == DSP_ERR_NONE) &&
                ((UsedSizeOffset + UsedSize) < Size)) {
                ucode_bin_buf[UCODE_BIN_ORCMD].Base = StartVirtAddr + UsedSize;
                ucode_bin_buf[UCODE_BIN_ORCMD].size = ucode_bin_size[UCODE_BIN_ORCMD];
                ucode_bin_buf[UCODE_BIN_ORCMD].pool_id = DSP_BUF_POOL_BINARY;
                UsedSize += ucode_bin_size[UCODE_BIN_ORCMD];
                UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            /* Allocate orcmd */
            if ((Rval == DSP_ERR_NONE) &&
                ((UsedSizeOffset + UsedSize) < Size)) {
                ucode_bin_buf[UCODE_BIN_ORCMD1].Base = StartVirtAddr + UsedSize;
                ucode_bin_buf[UCODE_BIN_ORCMD1].size = ucode_bin_size[UCODE_BIN_ORCMD1];
                ucode_bin_buf[UCODE_BIN_ORCMD1].pool_id = DSP_BUF_POOL_BINARY;
                UsedSize += ucode_bin_size[UCODE_BIN_ORCMD1];
                UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
#endif

            /* Allocate default binary */
            if ((Rval == DSP_ERR_NONE) &&
                ((UsedSizeOffset + UsedSize) < Size)) {
                ucode_bin_buf[UCODE_BIN_DEFAULT].Base = StartVirtAddr + UsedSize;
                ucode_bin_buf[UCODE_BIN_DEFAULT].size = ucode_bin_size[UCODE_BIN_DEFAULT];
                ucode_bin_buf[UCODE_BIN_DEFAULT].pool_id = DSP_BUF_POOL_BINARY;
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }
    }
#else
    dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr : Null dtb address", 0U, 0U, 0U, 0U, 0U);
#endif
    return Rval;
}

#else
static inline uint32_t dsp_osal_alloc_ucode_addr(void)
{
    UINT32 Rval = 0;
    ULONG phys_addr;
    ULONG AddrStart, AddrEnd;
    extern ULONG __ucode_start, __ucode_end;
    const ULONG *pAddrStart, *pAddrEnd;
    static const uint32_t ucode_bin_size[UCODE_BIN_NUM] = {
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        [UCODE_BIN_ORCODE]  = ORCODE_BIN_MAX_SIZE,
        [UCODE_BIN_ORCODE1] = ORCODE1_BIN_MAX_SIZE,
        [UCODE_BIN_ORCODE2] = ORCODE2_BIN_MAX_SIZE,
        [UCODE_BIN_ORCODE3] = ORCODE3_BIN_MAX_SIZE,
        [UCODE_BIN_ORCME]   = ORCME_BIN_MAX_SIZE,
        [UCODE_BIN_ORCME1]  = ORCME_BIN_MAX_SIZE,
        [UCODE_BIN_ORCMD]   = ORCMD_BIN_MAX_SIZE,
        [UCODE_BIN_ORCMD1]  = ORCMD_BIN_MAX_SIZE,
        [UCODE_BIN_DEFAULT] = DEFAULT_BIN_MAX_SIZE,
#else
        [UCODE_BIN_ORCODE]  = ORCODE_BIN_MAX_SIZE,
        [UCODE_BIN_ORCME]   = ORCME_BIN_MAX_SIZE,
        [UCODE_BIN_ORCMD]   = ORCMD_BIN_MAX_SIZE,
        [UCODE_BIN_DEFAULT] = DEFAULT_BIN_MAX_SIZE,
#endif
    };

    /* update buf_pool */
    pAddrStart = &__ucode_start;
    dsp_osal_typecast(&AddrStart, &pAddrStart);
    pAddrEnd = &__ucode_end;
    dsp_osal_typecast(&AddrEnd, &pAddrEnd);
    dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_BINARY].Base, &pAddrStart);
    osal_buf_pool[DSP_BUF_POOL_BINARY].size = (UINT32)(AddrEnd - AddrStart);
    osal_buf_pool[DSP_BUF_POOL_BINARY].UsedSize = 0U;
    osal_buf_pool[DSP_BUF_POOL_BINARY].is_cached = 0U;
    (void)dsp_osal_virt2phys(osal_buf_pool[DSP_BUF_POOL_BINARY].Base, &phys_addr);
    osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase = phys_addr;
    dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X%X Phys 0x%X Sz %d",
                      DSP_BUF_POOL_BINARY,
                      dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_BINARY].Base),
                      dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_BINARY].Base),
                      osal_buf_pool[DSP_BUF_POOL_BINARY].PhysBase,
                      osal_buf_pool[DSP_BUF_POOL_BINARY].size);

    {
        ULONG VirtAddr = osal_buf_pool[DSP_BUF_POOL_BINARY].Base;
        ULONG BaseStartVirtAddr, StartVirtAddr;
        UINT32 MemOffset = 0U;
        UINT32 UsedSizeOffset = 0U, UsedSize = 0U;
        UINT32 Size = osal_buf_pool[DSP_BUF_POOL_BINARY].size;

        dsp_osal_typecast(&BaseStartVirtAddr, &VirtAddr);
        StartVirtAddr = ADDR_ALIGN_NUM(BaseStartVirtAddr, DSP_BUF_ADDR_ALIGN);
        MemOffset = (UINT32)(StartVirtAddr - BaseStartVirtAddr);
        UsedSizeOffset += MemOffset;

        /* Allocate orcode */
        ucode_bin_buf[UCODE_BIN_ORCODE].Base = StartVirtAddr;
        ucode_bin_buf[UCODE_BIN_ORCODE].size = ucode_bin_size[UCODE_BIN_ORCODE];
        ucode_bin_buf[UCODE_BIN_ORCODE].pool_id = DSP_BUF_POOL_BINARY;
        UsedSize += ucode_bin_size[UCODE_BIN_ORCODE];
        UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        /* Allocate orcode1 */
        if ((UsedSizeOffset + UsedSize) < Size) {
            ucode_bin_buf[UCODE_BIN_ORCODE1].Base = StartVirtAddr + UsedSize;
            ucode_bin_buf[UCODE_BIN_ORCODE1].size = ucode_bin_size[UCODE_BIN_ORCODE1];
            ucode_bin_buf[UCODE_BIN_ORCODE1].pool_id = DSP_BUF_POOL_BINARY;
            UsedSize += ucode_bin_size[UCODE_BIN_ORCODE1];
            UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }

        /* Allocate orcode2 */
        if ((UsedSizeOffset + UsedSize) < Size) {
            ucode_bin_buf[UCODE_BIN_ORCODE2].Base = StartVirtAddr + UsedSize;
            ucode_bin_buf[UCODE_BIN_ORCODE2].size = ucode_bin_size[UCODE_BIN_ORCODE2];
            ucode_bin_buf[UCODE_BIN_ORCODE2].pool_id = DSP_BUF_POOL_BINARY;
            UsedSize += ucode_bin_size[UCODE_BIN_ORCODE2];
            UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }

        /* Allocate orcode3 */
        if ((UsedSizeOffset + UsedSize) < Size) {
            ucode_bin_buf[UCODE_BIN_ORCODE3].Base = StartVirtAddr + UsedSize;
            ucode_bin_buf[UCODE_BIN_ORCODE3].size = ucode_bin_size[UCODE_BIN_ORCODE3];
            ucode_bin_buf[UCODE_BIN_ORCODE3].pool_id = DSP_BUF_POOL_BINARY;
            UsedSize += ucode_bin_size[UCODE_BIN_ORCODE3];
            UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }
#endif


        /* Allocate orcme */
        if ((UsedSizeOffset + UsedSize) < Size) {
            ucode_bin_buf[UCODE_BIN_ORCME].Base = StartVirtAddr + UsedSize;
            ucode_bin_buf[UCODE_BIN_ORCME].size = ucode_bin_size[UCODE_BIN_ORCME];
            ucode_bin_buf[UCODE_BIN_ORCME].pool_id = DSP_BUF_POOL_BINARY;
            UsedSize += ucode_bin_size[UCODE_BIN_ORCME];
            UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        /* Allocate orcme1 */
        if ((UsedSizeOffset + UsedSize) < Size) {
            ucode_bin_buf[UCODE_BIN_ORCME1].Base = StartVirtAddr + UsedSize;
            ucode_bin_buf[UCODE_BIN_ORCME1].size = ucode_bin_size[UCODE_BIN_ORCME1];
            ucode_bin_buf[UCODE_BIN_ORCME1].pool_id = DSP_BUF_POOL_BINARY;
            UsedSize += ucode_bin_size[UCODE_BIN_ORCME1];
            UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }
#endif

        /* Allocate orcmd */
        if ((Rval == DSP_ERR_NONE) &&
            ((UsedSizeOffset + UsedSize) < Size)) {
            ucode_bin_buf[UCODE_BIN_ORCMD].Base = StartVirtAddr + UsedSize;
            ucode_bin_buf[UCODE_BIN_ORCMD].size = ucode_bin_size[UCODE_BIN_ORCMD];
            ucode_bin_buf[UCODE_BIN_ORCMD].pool_id = DSP_BUF_POOL_BINARY;
            UsedSize += ucode_bin_size[UCODE_BIN_ORCMD];
            UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        /* Allocate orcmd */
        if ((Rval == DSP_ERR_NONE) &&
            ((UsedSizeOffset + UsedSize) < Size)) {
            ucode_bin_buf[UCODE_BIN_ORCMD1].Base = StartVirtAddr + UsedSize;
            ucode_bin_buf[UCODE_BIN_ORCMD1].size = ucode_bin_size[UCODE_BIN_ORCMD1];
            ucode_bin_buf[UCODE_BIN_ORCMD1].pool_id = DSP_BUF_POOL_BINARY;
            UsedSize += ucode_bin_size[UCODE_BIN_ORCMD1];
            UsedSize = ALIGN_NUM(UsedSize, DSP_BUF_ADDR_ALIGN);
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }
#endif

        /* Allocate default binary */
        if ((Rval == DSP_ERR_NONE) &&
            ((UsedSizeOffset + UsedSize) < Size)) {
            ucode_bin_buf[UCODE_BIN_DEFAULT].Base = StartVirtAddr + UsedSize;
            ucode_bin_buf[UCODE_BIN_DEFAULT].size = ucode_bin_size[UCODE_BIN_DEFAULT];
            ucode_bin_buf[UCODE_BIN_DEFAULT].pool_id = DSP_BUF_POOL_BINARY;
        } else {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_ucode_addr() : mem shortage %d", __LINE__, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
}
#endif

uint32_t dsp_osal_get_ucode_base_addr(osal_ucode_addr_t *pBase)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pBase == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_ucode_base_addr : Null address", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp binary mem range */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        if ((ucode_bin_buf[UCODE_BIN_ORCODE].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE2].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE3].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_DEFAULT].Base == 0UL))
#else
        if ((ucode_bin_buf[UCODE_BIN_ORCODE].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_DEFAULT].Base == 0UL))
#endif
        {
            Rval = dsp_osal_alloc_ucode_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            pBase->CodeAddr = ucode_bin_buf[UCODE_BIN_ORCODE].Base;
            pBase->MeAddr = ucode_bin_buf[UCODE_BIN_ORCME].Base;
            pBase->MdxfAddr = ucode_bin_buf[UCODE_BIN_ORCMD].Base;
            pBase->DefBinAddr = ucode_bin_buf[UCODE_BIN_DEFAULT].Base;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            pBase->Code1Addr = ucode_bin_buf[UCODE_BIN_ORCODE1].Base;
            pBase->Code2Addr = ucode_bin_buf[UCODE_BIN_ORCODE2].Base;
            pBase->Code3Addr = ucode_bin_buf[UCODE_BIN_ORCODE3].Base;
            pBase->Me1Addr = ucode_bin_buf[UCODE_BIN_ORCME1].Base;
            pBase->Mdxf1Addr = ucode_bin_buf[UCODE_BIN_ORCMD1].Base;
#endif
        }
    }

    return Rval;
}

uint32_t dsp_osal_get_ucode_end_addr(osal_ucode_addr_t *pEnd)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (pEnd == NULL) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_ucode_end_addr : Null address", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp binary mem range */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        if ((ucode_bin_buf[UCODE_BIN_ORCODE].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE2].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCODE3].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD1].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_DEFAULT].Base == 0UL))
#else
        if ((ucode_bin_buf[UCODE_BIN_ORCODE].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCME].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_ORCMD].Base == 0UL) ||
            (ucode_bin_buf[UCODE_BIN_DEFAULT].Base == 0UL))
#endif
        {
            Rval = dsp_osal_alloc_ucode_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            pEnd->CodeAddr = ucode_bin_buf[UCODE_BIN_ORCODE].Base + ucode_bin_buf[UCODE_BIN_ORCODE].size;
            pEnd->MeAddr = ucode_bin_buf[UCODE_BIN_ORCME].Base + ucode_bin_buf[UCODE_BIN_ORCME].size;
            pEnd->MdxfAddr = ucode_bin_buf[UCODE_BIN_ORCMD].Base + ucode_bin_buf[UCODE_BIN_ORCMD].size;
            pEnd->DefBinAddr = ucode_bin_buf[UCODE_BIN_DEFAULT].Base + ucode_bin_buf[UCODE_BIN_DEFAULT].size;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            pEnd->Code1Addr = ucode_bin_buf[UCODE_BIN_ORCODE1].Base + ucode_bin_buf[UCODE_BIN_ORCODE1].size;
            pEnd->Code2Addr = ucode_bin_buf[UCODE_BIN_ORCODE2].Base + ucode_bin_buf[UCODE_BIN_ORCODE2].size;
            pEnd->Code3Addr = ucode_bin_buf[UCODE_BIN_ORCODE3].Base + ucode_bin_buf[UCODE_BIN_ORCODE3].size;
            pEnd->Me1Addr = ucode_bin_buf[UCODE_BIN_ORCME1].Base + ucode_bin_buf[UCODE_BIN_ORCME1].size;
            pEnd->Mdxf1Addr = ucode_bin_buf[UCODE_BIN_ORCMD1].Base + ucode_bin_buf[UCODE_BIN_ORCMD1].size;
#endif
        }
    }

    return Rval;
}

/* allocate dsp protocol buffer */
static inline uint32_t dsp_osal_alloc_prot_addr(void)
{
#if defined (CONFIG_DEVICE_TREE_SUPPORT)
    UINT32 Rval = OK;

#if defined (CONFIG_DTB_LOADADDR)
#define DTS_DSP_PROT_NODE        "/reserved-memory/dsp_prot_buf"
    int32_t fdt_ret = 0, offset = 0;
    const void *pfdt = NULL;
    INT32 len;
    const char *chr1;
    const UINT32 *ptr1;
    const UINT64 *ptr64;
    const struct fdt_property *prop = NULL;
    ULONG PhysAddr = 0U;
    UINT32 Size = 0U;
    ULONG VirtAddr;
#if defined (CONFIG_ARM64)
    const UINT64 dtb_addr = (UINT64)CONFIG_DTB_LOADADDR;
#else
    const UINT32 dtb_addr = (UINT32)CONFIG_DTB_LOADADDR;
#endif

    //dsp_osal_printU5("dsp_osal_alloc_prot_addr : dtb addr 0x%X", dtb_addr, 0U, 0U, 0U, 0U);
    dsp_osal_typecast(&pfdt, &dtb_addr);
    fdt_ret = AmbaFDT_CheckHeader(pfdt);
    if (fdt_ret != 0) {
        dsp_osal_printU5("[ERR] dsp_osal_alloc_prot_addr : AmbaFDT_CheckHeader fail[0x%X]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        fdt_ret = AmbaFDT_PathOffset(pfdt, DTS_DSP_PROT_NODE);
        if (fdt_ret < 0) {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_prot_addr : AmbaFDT_PathOffset[DTS_DSP_PROT_NODE] fail[%d]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            offset = fdt_ret;
            prop = AmbaFDT_GetProperty(pfdt, offset, "reg", &len);
            if ((prop != NULL) && (len > 0)) {
                chr1 = (const char *)&prop->data[0];
                // reg <base size>
                if (len == 8) {
                    // 32bit per cell
                    dsp_osal_typecast(&ptr1, &chr1);
                    PhysAddr = AmbaFDT_Fdt32ToCpu(ptr1[0U]);
                    Size = AmbaFDT_Fdt32ToCpu(ptr1[1U]);
                } else if (len == 16) {
                    // 64bit per cell
                    dsp_osal_typecast(&ptr64, &chr1);
                    PhysAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0U]);
                    Size = (UINT32)AmbaFDT_Fdt64ToCpu(ptr64[1U]);
                } else {
                    // unknown
                }
                (void)dsp_osal_phys2virt(PhysAddr, &VirtAddr);
                osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base = VirtAddr;
                osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size = Size;
                osal_buf_pool[DSP_BUF_POOL_PROTOCOL].UsedSize = 0U;
                osal_buf_pool[DSP_BUF_POOL_PROTOCOL].is_cached = 0U;
                osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase = PhysAddr;
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_prot_addr : AmbaFDT_GetProperty[DTS_DSP_PROT_NODE] fail", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }

        dsp_osal_printU5("[dspdev] FDT mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                          DSP_BUF_POOL_PROTOCOL,
                          dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base),
                          dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base),
                          dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase),
                          dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase));
        dsp_osal_printU5("                         Sz %d Cached %d",
                          osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size,
                          osal_buf_pool[DSP_BUF_POOL_PROTOCOL].is_cached, 0U, 0U, 0U);
    }
#else
    dsp_osal_printU5("[ERR] dsp_osal_alloc_prot_addr : Null dtb address", 0U, 0U, 0U, 0U, 0U);
#endif
    return Rval;
#else
    UINT32 Rval = OK;
    ULONG phys_addr;
    extern ULONG __dsp_buf_start, __dsp_buf_end;
    const ULONG *pAddrStart, *pAddrEnd;
    ULONG AddrStart, AddrEnd;

    /* update buf_pool */
    pAddrStart = &__dsp_buf_start;
    dsp_osal_typecast(&AddrStart, &pAddrStart);
    pAddrEnd = &__dsp_buf_end;
    dsp_osal_typecast(&AddrEnd, &pAddrEnd);
    dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base, &pAddrStart);
    osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size = (UINT32)(AddrEnd - AddrStart);
    osal_buf_pool[DSP_BUF_POOL_PROTOCOL].UsedSize = 0U;
    osal_buf_pool[DSP_BUF_POOL_PROTOCOL].is_cached = 0U;
    (void)dsp_osal_virt2phys(osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base, &phys_addr);
    osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase = phys_addr;

    dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X Phys 0x%X Sz %d Cached %d",
                      DSP_BUF_POOL_PROTOCOL,
                      (UINT32)osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base,
                      osal_buf_pool[DSP_BUF_POOL_PROTOCOL].PhysBase,
                      osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size,
                      osal_buf_pool[DSP_BUF_POOL_PROTOCOL].is_cached);

    return Rval;
#endif
}

UINT32 dsp_osal_alloc_prot_buf(ULONG *pBase, UINT32 ReqSize, UINT32 Align)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 NeededSize;
    ULONG Offset, CurBase;

    if ((pBase == NULL) || (ReqSize == 0U)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_prot_buf : Null address or Zero size", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp protocol mem range */
        if (osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base == 0UL) {
            Rval = dsp_osal_alloc_prot_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            CurBase = osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base + osal_buf_pool[DSP_BUF_POOL_PROTOCOL].UsedSize;
            Offset = ADDR_ALIGN_NUM(CurBase, Align) - CurBase;
            NeededSize = (UINT32)Offset + ReqSize;
            if (((CurBase + NeededSize) - osal_buf_pool[DSP_BUF_POOL_PROTOCOL].Base) > osal_buf_pool[DSP_BUF_POOL_PROTOCOL].size) {
                Rval = DSP_ERR_0003/*DSP_ERR_MEM_ALLOC*/;
            } else {
                *pBase = CurBase + Offset;

                /* Update pool */
                osal_buf_pool[DSP_BUF_POOL_PROTOCOL].UsedSize += NeededSize;
            }
        }
    }

    return Rval;
}

/* allocate dsp statistic buffer */
static inline uint32_t dsp_osal_alloc_stat_addr(void)
{
    UINT32 Rval = OK;
#if defined (CONFIG_DEVICE_TREE_SUPPORT)
#if defined (CONFIG_DTB_LOADADDR)
#define DTS_DSP_PROT_CA_NODE        "/reserved-memory/dsp_prot_cache_buf"
    int32_t fdt_ret = 0, offset = 0;
    const void *pfdt = NULL;
    INT32 len;
    const char *chr1;
    const UINT32 *ptr1;
    const UINT64 *ptr64;
    const struct fdt_property *prop = NULL;
    ULONG PhysAddr = 0U;
    UINT32 Size = 0U;
    ULONG VirtAddr;
#if defined (CONFIG_ARM64)
    const UINT64 dtb_addr = (UINT64)CONFIG_DTB_LOADADDR;
#else
    const UINT32 dtb_addr = (UINT32)CONFIG_DTB_LOADADDR;
#endif

    //dsp_osal_printU5("dsp_osal_alloc_stat_addr : dtb addr 0x%X", dtb_addr, 0U, 0U, 0U, 0U);
    dsp_osal_typecast(&pfdt, &dtb_addr);
    fdt_ret = AmbaFDT_CheckHeader(pfdt);
    if (fdt_ret != 0) {
        dsp_osal_printU5("[ERR] dsp_osal_alloc_stat_addr : AmbaFDT_CheckHeader fail[0x%X]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        fdt_ret = AmbaFDT_PathOffset(pfdt, DTS_DSP_PROT_CA_NODE);
        if (fdt_ret < 0) {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_stat_addr : AmbaFDT_PathOffset[DTS_DSP_PROT_CA_NODE] fail[%d]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            offset = fdt_ret;
            prop = AmbaFDT_GetProperty(pfdt, offset, "reg", &len);
            if ((prop != NULL) && (len > 0)) {
                chr1 = (const char *)&prop->data[0];
                // reg <base size>
                if (len == 8) {
                    // 32bit per cell
                    dsp_osal_typecast(&ptr1, &chr1);
                    PhysAddr = AmbaFDT_Fdt32ToCpu(ptr1[0U]);
                    Size = AmbaFDT_Fdt32ToCpu(ptr1[1U]);
                } else if (len == 16) {
                    // 64bit per cell
                    dsp_osal_typecast(&ptr64, &chr1);
                    PhysAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0U]);
                    Size = (UINT32)AmbaFDT_Fdt64ToCpu(ptr64[1U]);
                } else {
                    // unknown
                }
                (void)dsp_osal_phys2virt(PhysAddr, &VirtAddr);
                osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base = VirtAddr;
                osal_buf_pool[DSP_BUF_POOL_STATISTIC].size = Size;
                osal_buf_pool[DSP_BUF_POOL_STATISTIC].UsedSize = 0U;
                osal_buf_pool[DSP_BUF_POOL_STATISTIC].is_cached = 1U;
                osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase = PhysAddr;
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_stat_addr : AmbaFDT_GetProperty[DTS_DSP_PROT_CA_NODE] fail", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }

        dsp_osal_printU5("[dspdev] FDT mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                          DSP_BUF_POOL_STATISTIC,
                          dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base),
                          dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base),
                          dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase),
                          dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase));
        dsp_osal_printU5("                         Sz %d Cached %d",
                          osal_buf_pool[DSP_BUF_POOL_STATISTIC].size,
                          osal_buf_pool[DSP_BUF_POOL_STATISTIC].is_cached, 0U, 0U, 0U);

    }
#else
    dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : Null dtb address", 0U, 0U, 0U, 0U, 0U);
#endif

#else
    ULONG phys_addr;
    extern ULONG __dsp_cache_buf_start, __dsp_cache_buf_end;
    const ULONG *pAddrStart, *pAddrEnd;
    ULONG AddrStart, AddrEnd;

    /* update buf_pool */
    pAddrStart = &__dsp_cache_buf_start;
    dsp_osal_typecast(&AddrStart, &pAddrStart);
    pAddrEnd = &__dsp_cache_buf_end;
    dsp_osal_typecast(&AddrEnd, &pAddrEnd);
    dsp_osal_typecast(&osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base, &pAddrStart);
    osal_buf_pool[DSP_BUF_POOL_STATISTIC].size = (UINT32)(AddrEnd - AddrStart);
    osal_buf_pool[DSP_BUF_POOL_STATISTIC].UsedSize = 0U;
    osal_buf_pool[DSP_BUF_POOL_STATISTIC].is_cached = 1U;
    (void)dsp_osal_virt2phys(osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base, &phys_addr);
    osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase = phys_addr;

    dsp_osal_printU5("[dspdev] mempool[%d] Virt 0x%X Phys 0x%X Sz %d Cached %d",
                      DSP_BUF_POOL_STATISTIC,
                      (UINT32)osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base,
                      osal_buf_pool[DSP_BUF_POOL_STATISTIC].PhysBase,
                      osal_buf_pool[DSP_BUF_POOL_STATISTIC].size,
                      osal_buf_pool[DSP_BUF_POOL_STATISTIC].is_cached);

#endif
    return Rval;
}

UINT32 dsp_osal_get_stat_buf(ULONG *pBase, UINT32 *pSize)
{
    UINT32 Rval = DSP_ERR_NONE;

    if ((pBase == NULL) || (pSize == NULL)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_stat_buf : Null address or size", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp protocol mem range */
        if (osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base == 0UL) {
            Rval = dsp_osal_alloc_stat_addr();
        }
        if (Rval == DSP_ERR_NONE) {
            *pBase = osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base;
            *pSize = osal_buf_pool[DSP_BUF_POOL_STATISTIC].size;
        }
    }
    return Rval;
}

UINT32 dsp_osal_alloc_stat_buf(ULONG *pBase, UINT32 ReqSize, UINT32 Align)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 NeededSize;
    ULONG Offset, CurBase;

    if ((pBase == NULL) || (ReqSize == 0U)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_alloc_stat_buf : Null address or Zero size", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp statistic mem range */
        if (osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base == 0UL) {
            Rval = dsp_osal_alloc_stat_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            CurBase = osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base + osal_buf_pool[DSP_BUF_POOL_STATISTIC].UsedSize;
            Offset = ADDR_ALIGN_NUM(CurBase, Align) - CurBase;
            NeededSize = (UINT32)(Offset + ReqSize);
            if (((CurBase + NeededSize) - osal_buf_pool[DSP_BUF_POOL_STATISTIC].Base) > osal_buf_pool[DSP_BUF_POOL_STATISTIC].size) {
                Rval = DSP_ERR_0003/*DSP_ERR_MEM_ALLOC*/;
            } else {
                *pBase = CurBase + Offset;

                /* Update pool */
                osal_buf_pool[DSP_BUF_POOL_STATISTIC].UsedSize += NeededSize;
            }
        }
    }

    return Rval;
}

#if defined (CONFIG_DEVICE_TREE_SUPPORT)
/* allocate dsp data buffer */
static inline uint32_t dsp_osal_alloc_data_addr(void)
{
    UINT32 Rval = OK;

#if defined (CONFIG_DTB_LOADADDR)
#define DTS_DSP_DATA_NODE        "/reserved-memory/dsp_data_buf"
    int32_t fdt_ret = 0, offset = 0;
    const void *pfdt = NULL;
    INT32 len;
    const char *chr1;
    const UINT32 *ptr1;
    const UINT64 *ptr64;
    const struct fdt_property *prop = NULL;
    ULONG PhysAddr = 0U;
    UINT32 Size = 0U;
    ULONG VirtAddr;
#if defined (CONFIG_ARM64)
    const UINT64 dtb_addr = (UINT64)CONFIG_DTB_LOADADDR;
#else
    const UINT32 dtb_addr = (UINT32)CONFIG_DTB_LOADADDR;
#endif

    //dsp_osal_printU5("dsp_osal_alloc_data_addr : dtb addr 0x%X", dtb_addr, 0U, 0U, 0U, 0U);
    dsp_osal_typecast(&pfdt, &dtb_addr);
    fdt_ret = AmbaFDT_CheckHeader(pfdt);
    if (fdt_ret != 0) {
        dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : AmbaFDT_CheckHeader fail[0x%X]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        fdt_ret = AmbaFDT_PathOffset(pfdt, DTS_DSP_DATA_NODE);
        if (fdt_ret < 0) {
            dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : AmbaFDT_PathOffset[DTS_DSP_DATA_NODE] fail[%d]", (uint32_t)fdt_ret, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        } else {
            offset = fdt_ret;
            prop = AmbaFDT_GetProperty(pfdt, offset, "reg", &len);
            if ((prop != NULL) && (len > 0)) {
                chr1 = (const char *)&prop->data[0];
                // reg <base size>
                if (len == 8) {
                    // 32bit per cell
                    dsp_osal_typecast(&ptr1, &chr1);
                    PhysAddr = AmbaFDT_Fdt32ToCpu(ptr1[0U]);
                    Size = AmbaFDT_Fdt32ToCpu(ptr1[1U]);
                } else if (len == 16) {
                    // 64bit per cell
                    dsp_osal_typecast(&ptr64, &chr1);
                    PhysAddr = (ULONG)AmbaFDT_Fdt64ToCpu(ptr64[0U]);
                    Size = (UINT32)AmbaFDT_Fdt64ToCpu(ptr64[1U]);
                } else {
                    // unknown
                }
                (void)dsp_osal_phys2virt(PhysAddr, &VirtAddr);
                osal_buf_pool[DSP_BUF_POOL_DATA].Base = VirtAddr;
                osal_buf_pool[DSP_BUF_POOL_DATA].size = Size;
                osal_buf_pool[DSP_BUF_POOL_DATA].UsedSize = 0U;
                osal_buf_pool[DSP_BUF_POOL_DATA].is_cached = 1U;
                osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase = PhysAddr;
            } else {
                dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : AmbaFDT_GetProperty[DTS_DSP_DATA_NODE] fail", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }

        dsp_osal_printU5("[dspdev] FDT mempool[%d] Virt 0x%X%08X Phys 0x%X%08X",
                          DSP_BUF_POOL_DATA,
                          dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].Base),
                          dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].Base),
                          dsp_osal_get_u64msb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase),
                          dsp_osal_get_u64lsb((UINT64)osal_buf_pool[DSP_BUF_POOL_DATA].PhysBase));
        dsp_osal_printU5("                         Sz %d Cached %d",
                          osal_buf_pool[DSP_BUF_POOL_DATA].size,
                          osal_buf_pool[DSP_BUF_POOL_DATA].is_cached, 0U, 0U, 0U);
    }
#else
    dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : Null dtb address", 0U, 0U, 0U, 0U, 0U);
#endif
    return Rval;
}
#else
static inline uint32_t dsp_osal_alloc_data_addr(void)
{
    UINT32 Rval = DSP_ERR_0008;

    dsp_osal_printU5("[ERR] dsp_osal_alloc_data_addr : Shall from App", 0U, 0U, 0U, 0U, 0U);

    return Rval;
}
#endif

UINT32 dsp_osal_get_data_buf(ULONG *pBase, UINT32 *pSize, UINT32 *pIsCached)
{
    UINT32 Rval = DSP_ERR_NONE;

    if ((pBase == NULL) || (pSize == NULL) || (pIsCached == NULL)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_data_buf : Null input", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp data mem range */
        if (osal_buf_pool[DSP_BUF_POOL_DATA].Base == 0UL) {
            Rval = dsp_osal_alloc_data_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            *pBase = osal_buf_pool[DSP_BUF_POOL_DATA].Base;
            *pSize = osal_buf_pool[DSP_BUF_POOL_DATA].size;
            *pIsCached = osal_buf_pool[DSP_BUF_POOL_DATA].is_cached;
        }
    }

    return Rval;
}

UINT32 dsp_osal_get_bin_buf(ULONG *pBase, UINT32 *pSize, UINT32 *pIsCached)
{
    UINT32 Rval = DSP_ERR_NONE;

    if ((pBase == NULL) || (pSize == NULL) || (pIsCached == NULL)) {
        Rval = DSP_ERR_0000;
        dsp_osal_printU5("[ERR] dsp_osal_get_bin_buf : Null input", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Get dsp log mem range */
        if (osal_buf_pool[DSP_BUF_POOL_BINARY].Base == 0UL) {
            Rval = dsp_osal_alloc_ucode_addr();
        }

        if (Rval == DSP_ERR_NONE) {
            *pBase = osal_buf_pool[DSP_BUF_POOL_BINARY].Base;
            *pSize = osal_buf_pool[DSP_BUF_POOL_BINARY].size;
            *pIsCached = osal_buf_pool[DSP_BUF_POOL_BINARY].is_cached;
        }
    }

    return Rval;
}

#if 0
UINT32 dsp_osal_alloc_add_buf_addr(UINT32 Type, ULONG phys_addr, UINT32 size, UINT32 IsCached)
{
    UINT32 Rval = OK;
(void)Type;
(void)phys_addr;
(void)size;
(void)IsCached;
    return Rval;
}

/******************************************************************************/
/*                                  DebugPort                                 */
/******************************************************************************/

void dsp_osal_delay_cycles(UINT32 Delay)
{
    AmbaDelayCycles(Delay);
}
#endif

void dsp_osal_dbgport_get_vin_enable_status(UINT32 VinId, UINT32 *pEnable)
{
    *pEnable = AmbaCSL_VinIsEnabled(VinId);
}

UINT32 dsp_osal_dbgport_get_clk_enable_status(UINT32 ClkId, UINT32 *pEnable)
{
    return AmbaSYS_CheckFeature(ClkId, pEnable);
}

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
UINT32 dsp_osal_dbgport_get_reset_status_info(const UINT32 Idx, UINT32 *pAddr, UINT32 *pDefValue)
{
    UINT32 Rval = DSP_ERR_NONE;
    /*
        idsp        0   0xED1CFFC0  0x0
        Idsp-core   1   0xED1CFFC4  0x0
        smem        2   0xED05E00C  0xBEA8
        vdsp        3   0xED020308  0xBABEFACE
        Vout0-mixer 4   0xE0008214  0x7F
        Vout0-core  5   0xE000837C  0x7F
        Vout0-vo    6   0xE0008380  0xFF
        Vout0-byte  7   0xE0008384  0x1FF
        Vout1-mixer 4   0xE0008514  0x7F
        Vout1-core  5   0xE000867C  0x7F
        Vout1-vo    6   0xE0008680  0xFF
        Vout1-byte  7   0xE0008684  0x1FF
    */

    static const UINT32 DspResetStatusAddrTable[DSP_RESET_STATUS_NUM + DSP_RESET_STATUS_VOUT_MIXER] = {
            [DSP_RESET_STATUS_IDSP]                                     = 0xED1CFFC0U,
            [DSP_RESET_STATUS_IDSP_CORE]                                = 0xED1CFFC4U,
            [DSP_RESET_STATUS_SMEM]                                     = 0xED05E00CU,
            [DSP_RESET_STATUS_VDSP]                                     = 0xED020308U,
            [DSP_RESET_STATUS_VOUT_MIXER]                               = 0xE0008214U,
            [DSP_RESET_STATUS_VOUT_CORE]                                = 0xE000837CU,
            [DSP_RESET_STATUS_VOUT_VO]                                  = 0xE0008380U,
            [DSP_RESET_STATUS_VOUT_BYTE]                                = 0xE0008384U,
            [DSP_RESET_STATUS_VOUT_MIXER + DSP_RESET_STATUS_VOUT_MIXER] = 0xE0008514U,
            [DSP_RESET_STATUS_VOUT_CORE  + DSP_RESET_STATUS_VOUT_MIXER] = 0xE000867CU,
            [DSP_RESET_STATUS_VOUT_VO    + DSP_RESET_STATUS_VOUT_MIXER] = 0xE0008680U,
            [DSP_RESET_STATUS_VOUT_BYTE  + DSP_RESET_STATUS_VOUT_MIXER] = 0xE0008684U,
    };

    static const UINT32 DspResetStatusDefValTable[DSP_RESET_STATUS_NUM + DSP_RESET_STATUS_VOUT_MIXER] = {
            [DSP_RESET_STATUS_IDSP]                                     = 0x0U,
            [DSP_RESET_STATUS_IDSP_CORE]                                = 0x0U,
            [DSP_RESET_STATUS_SMEM]                                     = 0xBEA8U,
            [DSP_RESET_STATUS_VDSP]                                     = 0xBABEFACEU,
            [DSP_RESET_STATUS_VOUT_MIXER]                               = 0x7FU,
            [DSP_RESET_STATUS_VOUT_CORE]                                = 0x7FU,
            [DSP_RESET_STATUS_VOUT_VO]                                  = 0xFFU,
            [DSP_RESET_STATUS_VOUT_BYTE]                                = 0x1FFU,
            [DSP_RESET_STATUS_VOUT_MIXER + DSP_RESET_STATUS_VOUT_MIXER] = 0x7FU,
            [DSP_RESET_STATUS_VOUT_CORE  + DSP_RESET_STATUS_VOUT_MIXER] = 0x7FU,
            [DSP_RESET_STATUS_VOUT_VO    + DSP_RESET_STATUS_VOUT_MIXER] = 0xFFU,
            [DSP_RESET_STATUS_VOUT_BYTE  + DSP_RESET_STATUS_VOUT_MIXER] = 0x1FFU,
    };

    if (Idx < (DSP_RESET_STATUS_NUM + DSP_RESET_STATUS_VOUT_MIXER)) {
        *pAddr = DspResetStatusAddrTable[Idx];
        *pDefValue = DspResetStatusDefValTable[Idx];
    } else {
        Rval = DSP_ERR_0000;
    }
    return Rval;
}
#endif

/******************************************************************************/
/*                                  Event                                     */
/******************************************************************************/
UINT32 dsp_osal_evnt_hdlr_cfg(UINT32 EvntId, UINT32 MaxNum, AMBA_DSP_EVENT_HANDLER_f *pEvntHdlrs)
{
    return DSP_EventHandlerConfig((UINT16)EvntId, LL_EVENT_HANDLER, MaxNum, pEvntHdlrs);
}

UINT32 dsp_osal_evnt_hdlr_reg(UINT32 EvntId, UINT32 Reg, AMBA_DSP_EVENT_HANDLER_f EvntHdlr)
{
    UINT32 Rval = DSP_ERR_NONE;

    if (Reg == 0U) {
        Rval = DSP_UnRegisterEventHandler((UINT16)EvntId, LL_EVENT_HANDLER, EvntHdlr);
    } else {
        Rval = DSP_RegisterEventHandler((UINT16)EvntId, LL_EVENT_HANDLER, EvntHdlr);
    }

    return Rval;
}

UINT32 dsp_osal_evnt_hdlr_reset(UINT32 EvntId)
{
    return DSP_EventHandlerReset((UINT16)EvntId, LL_EVENT_HANDLER);
}

UINT32 dsp_osal_exec_evnt_user_cb(UINT32 HdlrNum, const AMBA_DSP_EVENT_HANDLER_f *pHdlr, UINT32 EvntId, const void *pEvntInfo, const UINT16 EventBufIdx)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i;

(void)EvntId;
    for (i = 0U; i < HdlrNum; i++) {
        if (pHdlr[i] != NULL) {
            (void)(pHdlr[i])(pEvntInfo);
        }
    }
    (void)DSP_ReleaseEventInfoBuf(EventBufIdx);

    return Rval;
}

#if 0
UINT32 dsp_osal_fetch_evnt_data(const void *pEvntData)
{
    UINT32 Rval = DSP_ERR_NONE;
(void)pEvntData;
    return Rval;
}
#endif

/******************************************************************************/
/*                                  AAA transfer                              */
/******************************************************************************/
UINT32 dsp_osal_aaa_transfer(const void *pEvntData)
{
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    extern UINT32 TransferCFAAaaStatData(const void *pData) GNU_WEAK_SYMBOL;
    extern UINT32 TransferPGAaaStatData(const void *pData) GNU_WEAK_SYMBOL;
    extern UINT32 TransCFAHistStatData(const void *pData) GNU_WEAK_SYMBOL;
    UINT32 (*TrasferCfa)(const void *In) = TransferCFAAaaStatData;
    UINT32 (*TrasferPg)(const void *In) = TransferPGAaaStatData;
    UINT32 (*TrasferHist)(const void *In) = TransCFAHistStatData;
    UINT32 Rval = DSP_ERR_NONE;
    const AMBA_DSP_EVENT_3A_TRANSFER_s *pAaaTransfer = NULL;

    dsp_osal_typecast(&pAaaTransfer, &pEvntData);
    if (pAaaTransfer->Type == DSP_3A_VIDEO) {
        if (TrasferCfa == NULL) {
            dsp_osal_printU5("Null TransferCFAAaaStatData", 0U, 0U, 0U, 0U, 0U);
        } else {
            Rval = TransferCFAAaaStatData(pEvntData);
        }
    } else if (pAaaTransfer->Type == DSP_3A_VIDEO_PG) {
        if (TrasferPg == NULL) {
            dsp_osal_printU5("Null TransferPGAaaStatData", 0U, 0U, 0U, 0U, 0U);
        } else {
            Rval = TransferPGAaaStatData(pEvntData);
        }
    } else if ((pAaaTransfer->Type == DSP_HIST_VIDEO) ||
               (pAaaTransfer->Type == DSP_HIST_VIDEO_RGBIR)) {
        if (TrasferHist == NULL) {
            dsp_osal_printU5("Null TransCFAHistStatData", 0U, 0U, 0U, 0U, 0U);
        } else {
            Rval = TransCFAHistStatData(pEvntData);
        }
    } else {
        // TBD
    }

    return Rval;
#else
    extern UINT32 AmbaIK_TransferCFAAaaStatData(const void *In) GNU_WEAK_SYMBOL;
    extern UINT32 AmbaIK_TransferPGAaaStatData(const void *In) GNU_WEAK_SYMBOL;
    extern UINT32 AmbaIK_TransCFAHistStatData(const void *In) GNU_WEAK_SYMBOL;
    UINT32 (*IK_TransferCfa)(const void *In) = AmbaIK_TransferCFAAaaStatData;
    UINT32 (*IK_TransferPg)(const void *In) = AmbaIK_TransferPGAaaStatData;
    UINT32 (*IK_TransferHist)(const void *In) = AmbaIK_TransCFAHistStatData;

    UINT32 Rval = DSP_ERR_NONE;
    const AMBA_DSP_EVENT_3A_TRANSFER_s *pAaaTransfer;

    dsp_osal_typecast(&pAaaTransfer, &pEvntData);
    if (pAaaTransfer->Type == DSP_3A_VIDEO) {
        if (IK_TransferCfa == NULL) {
            dsp_osal_printU5("Null AmbaIK_TransferCFAAaaStatData", 0U, 0U, 0U, 0U, 0U);
        } else {
            Rval = AmbaIK_TransferCFAAaaStatData(pEvntData);
        }
    } else if (pAaaTransfer->Type == DSP_3A_VIDEO_PG) {
        if (IK_TransferPg == NULL) {
            dsp_osal_printU5("Null AmbaIK_TransferPGAaaStatData", 0U, 0U, 0U, 0U, 0U);
        } else {
            Rval = AmbaIK_TransferPGAaaStatData(pEvntData);
        }
    } else if ((pAaaTransfer->Type == DSP_HIST_VIDEO) ||
               (pAaaTransfer->Type == DSP_HIST_VIDEO_RGBIR)) {
        if (IK_TransferHist == NULL) {
            dsp_osal_printU5("Null AmbaIK_TransCFAHistStatData", 0U, 0U, 0U, 0U, 0U);
        } else {
            Rval = AmbaIK_TransCFAHistStatData(pEvntData);
        }
    } else {
        // DO NOTHING
    }

    return Rval;
#endif
}

/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
#if 0
uint32_t dsp_osal_str_to_u32(const char *pString, uint32_t *pVal)
{
    return AmbaUtility_StringToUInt32(pString, pVal);
}
#endif

uint32_t dsp_osal_u32_to_str(char *pString, uint32_t str_len, uint32_t val, uint32_t radix)
{
    return AmbaUtility_UInt32ToStr(pString, str_len, val, radix);
}

uint32_t dsp_osal_str_print_u32(char *pBuffer, uint32_t BufferSize, const char *pFmtString, uint32_t Count, const uint32_t *pArgs)
{
    return AmbaUtility_StringPrintUInt32(pBuffer, BufferSize, pFmtString, Count, pArgs);
}

uint32_t dsp_osal_str_append(char *pBuffer, uint32_t BufferSize, const char *pSource)
{
    uint32_t retcode = DSP_ERR_NONE;

    AmbaUtility_StringAppend(pBuffer, BufferSize, pSource);

    return retcode;
}

uint32_t dsp_osal_str_append_u32(char *pBuffer, uint32_t BufferSize, uint32_t Value, uint32_t Radix)
{
    uint32_t retcode = DSP_ERR_NONE;

    AmbaUtility_StringAppendUInt32(pBuffer, BufferSize, Value, Radix);

    return retcode;
}

uint32_t dsp_osal_str_copy(char *pBuffer, uint32_t BufferSize, const char *pSource)
{
    uint32_t retcode = DSP_ERR_NONE;

    AmbaUtility_StringCopy(pBuffer, BufferSize, pSource);

    return retcode;
}

/******************************************************************************/
/*                                  Resource                                 */
/******************************************************************************/
#if 0
uint32_t dsp_osal_kernel_init(void)
{
    UINT32 Rval = OK;

    return Rval;
}

void dsp_osal_kernel_exit(void)
{

}
#endif

