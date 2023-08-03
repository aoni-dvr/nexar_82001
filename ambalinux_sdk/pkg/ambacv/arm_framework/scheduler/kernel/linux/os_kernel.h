/**
 *  @file os_kernel.h
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
 *  @details Definitions & Constants for OS kernel
 *
 */

#ifndef  OS_KERNEL_H
#define  OS_KERNEL_H

#ifdef USE_AMBA_KAL
#include "osal.h"
#include "ambint.h"
#else
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/of_irq.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#endif
#include "cvtask_errno.h"

#ifndef USE_AMBA_KAL
#define GNU_SECTION_NOZEROINIT
#endif

#define KRX_THREAD_PRIO          29U
#define KRX_STACK_SIZE           8192U

extern uint32_t    enable_arm_cache;
extern uint32_t    enable_rtos_cache;
extern uint32_t    enable_orc_cache;
extern uint32_t    enable_log_msg;
extern uint32_t    enable_cma;
extern uint32_t    scheduler_id;

/******************************************************************************/
/*                                  time                                      */
/******************************************************************************/
#define UNIX_TIME_ID                     0x00000000U


/******************************************************************************/
/*                                  clock                                     */
/******************************************************************************/
#define AUDIO_CLOCK_ID                   0x00000001U
#define VISION_CLOCK_ID                  0x00000002U
#define HSM_CLOCK_ID                     0x00000003U
#define NVP_CLOCK_ID                     0x00000004U
#define GVP_CLOCK_ID                     0x00000005U
#define FEX_CLOCK_ID                     0x00000006U
#define FMA_CLOCK_ID                     0x00000007U

/******************************************************************************/
/*                                  timeout                                 */
/******************************************************************************/
#define OS_KERNEL_WAIT_FOREVER          0xFFFFFFFFU
#define TOKEN_WAIT_TIMEOUT              10000U
#define WORK_QUEUE_WAIT_TIMEOUT         10000U
#define GLOBAL_LOCK_WAIT_TIMEOUT        10000U

/******************************************************************************/
/*                                  event                                     */
/******************************************************************************/
#define EVENT_OPT_GL                    0x0

/******************************************************************************/
/*                                  print                                 */
/******************************************************************************/
void krn_printS5(
    const char *fmt,
    const char *argv0,
    const char *argv1,
    const char *argv2,
    const char *argv3,
    const char *argv4);

void krn_printU5(
    const char *fmt,
    uint64_t argv0,
    uint64_t argv1,
    uint64_t argv2,
    uint64_t argv3,
    uint64_t argv4);

void krn_module_printS5(
    const char *fmt,
    const char *argv0,
    const char *argv1,
    const char *argv2,
    const char *argv3,
    const char *argv4);

void krn_module_printU5(
    const char *fmt,
    uint64_t argv0,
    uint64_t argv1,
    uint64_t argv2,
    uint64_t argv3,
    uint64_t argv4);

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
typedef uint32_t (*kthread_entry_t)(void *arg);
typedef void (*kisr_entry_t)(uint32_t irq, void *arg);
#ifdef USE_AMBA_KAL
typedef AMBA_KAL_SEMAPHORE_t    ksem_t;
typedef AMBA_KAL_MUTEX_t        kmutex_t;
typedef AMBA_KAL_TASK_t         kthread_t;
typedef AMBA_KAL_EVENT_FLAG_t   keven_t;
#else
typedef struct semaphore        ksem_t;
typedef struct mutex            kmutex_t;
typedef struct task_struct      kthread_t;
typedef struct {
    atomic_t                flag;
    struct wait_queue_head  wait_queue;
} keven_t;
#endif

uint32_t krn_sem_init(ksem_t *sem, int32_t pshared, uint32_t value);
uint32_t krn_sem_deinit(ksem_t *sem);
uint32_t krn_sem_post(ksem_t *sem);
uint32_t krn_sem_wait(ksem_t *sem, uint32_t timeout);
uint32_t krn_mutex_init(kmutex_t *mutex);
uint32_t krn_mutex_deinit(kmutex_t *mutex);
uint32_t krn_mutex_lock(kmutex_t *mutex);
uint32_t krn_mutex_try_lock(kmutex_t *mutex);
uint32_t krn_mutex_unlock(kmutex_t *mutex);
uint32_t krn_eventflag_init(keven_t *event);
uint32_t krn_eventflag_set(keven_t *event, uint32_t flag);
uint32_t krn_eventflag_get(keven_t *event, uint32_t reqflag, uint32_t *actflag, uint32_t timeout);
uint32_t krn_eventflag_clear(keven_t *event, uint32_t flag);
uint32_t krn_eventcond_set(keven_t *event);
uint32_t krn_eventcond_get(keven_t *event, const uint32_t *condition, const uint32_t *target, uint32_t operation, uint32_t timeout);

kthread_t *krn_thread_create(
    kthread_t *thread,
    kthread_entry_t entry,
    void *arg,
    uint32_t priority,
    uint32_t affinity,
    uint32_t stack_size,
    char *stack_buff,
    char *name);
uint32_t krn_thread_delete(kthread_t *thread);
uint32_t krn_thread_should_stop(kthread_t *thread);

uint32_t krn_sleep(uint32_t msec);

uint64_t krn_cache_addr_align(uint64_t addr);
uint64_t krn_cache_size_align(uint64_t size);
uint32_t krn_cache_clean(void *ptr, uint64_t size);
uint32_t krn_cache_invalidate(void *ptr, uint64_t size);

uint32_t krn_readl_relaxed(void* reg);
void krn_writel_relaxed(uint32_t val, void* reg);

uint32_t krn_copy_to_user(void *to, const void *from, uint64_t n);
uint32_t krn_copy_from_user(void *to, const void *from, uint64_t n);

void *krn_ioremap(uint64_t pa, uint64_t size);
void krn_iounmap(void* va, uint64_t size);

uint32_t krn_enable_irq(uint32_t irq_num, kisr_entry_t handler);
uint32_t krn_disable_irq(uint32_t irq_num);

uint32_t krn_cma_alloc(void *arg, uint64_t owner);
uint32_t krn_cma_free(void *arg);
uint32_t krn_cma_sync_cache(void *arg);
uint32_t krn_cma_get_usage(void *arg);

void * krn_cma_p2v(uint64_t phy, uint32_t *pCacheFlag);
uint64_t krn_cma_v2p(void *virt, uint32_t *pCacheFlag);

/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
#ifdef USE_AMBA_KAL
static inline void krn_typecast(void * pNewType, const void * pOldType)
{
    if(pNewType == NULL) {
        krn_printS5("[ERROR] krn_typecast : fun %s", __func__, NULL, NULL, NULL, NULL);
        krn_printU5("[ERROR] krn_typecast : line %d", __LINE__, 0U, 0U, 0U, 0U);
        krn_printU5("[ERROR] pNewType == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else if(pOldType == NULL) {
        krn_printS5("[ERROR] krn_typecast : fun %s", __func__, NULL, NULL, NULL, NULL);
        krn_printU5("[ERROR] krn_typecast : line %d", __LINE__, 0U, 0U, 0U, 0U);
        krn_printU5("[ERROR] pOldType == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        OSAL_TypeCast(pNewType, pOldType);
    }
}

static inline void krn_unused(void *ptr)
{
    OSAL_TouchUnused(ptr);
}

static inline uint32_t krn_memcpy(void *pDst, const void *pSrc, uint64_t num)
{
    uint32_t retcode = ERRCODE_NONE;

    if( OSAL_memcpy(pDst, pSrc, num) != ERRCODE_NONE) {
        retcode = ERR_DRV_SCHDR_MEMCPY_FAIL;
    }

    return retcode;
}

static inline uint32_t krn_memset(void *ptr, int32_t v, uint64_t n)
{
    uint32_t retcode = ERRCODE_NONE;

    if( OSAL_memset(ptr, v, n) != ERRCODE_NONE) {
        retcode = ERR_DRV_SCHDR_MEMSET_FAIL;
    }

    return retcode;
}
#else
static inline void krn_typecast(void * pNewType, const void * pOldType)
{
    if(pNewType == NULL) {
        printk("[ERROR] krn_typecast : %s %d : pNewType == NULL \n", __FUNCTION__, __LINE__);
    } else if(pOldType == NULL) {
        printk("[ERROR] krn_typecast : %s %d : pOldType == NULL \n", __FUNCTION__, __LINE__);
    } else {
        memcpy(pNewType, pOldType, sizeof(void *));
    }
}

static inline void krn_unused(void *ptr)
{
    (void)ptr;
}

static inline uint32_t krn_memcpy(void *pDst, const void *pSrc, uint64_t num)
{
    memcpy(pDst, pSrc, num);
    return 0U;
}

static inline uint32_t krn_memset(void *ptr, int32_t v, uint64_t n)
{
    memset(ptr, v, n);
    return 0U;
}
#endif
int32_t krn_strcmp(const char *s1, const char *s2);

uint32_t krn_sprintf_str(char *str, const char *str_src);

uint32_t krn_snprintf_uint1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0);

uint32_t krn_snprintf_uint2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1);

uint32_t krn_snprintf_uint3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2);

uint32_t krn_snprintf_uint4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3);

uint32_t krn_snprintf_uint5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4);

uint32_t krn_snprintf_str1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0);

uint32_t krn_snprintf_str2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1);

uint32_t krn_snprintf_str3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2);

uint32_t krn_snprintf_str4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3);

uint32_t krn_snprintf_str5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3,
    const char *arg4);

/******************************************************************************/
/*                                  Resource                                 */
/******************************************************************************/
uint32_t krn_get_sod_irq(uint32_t *irq_num);
uint32_t krn_get_clock(uint32_t id, uint32_t *freq);
uint32_t krn_get_time(uint32_t id, uint32_t *pSec, uint32_t *pMsec);
uint32_t krn_os_kernel_init(void);
void krn_os_kernel_exit(void);

#endif  //OS_KERNEL_H
