// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#include <linux/io.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/clk.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/iav_helper.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif
#include "cavalry_ioctl.h"
#include "os_kernel.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "cavalry_cma.h"
#include "cache_kernel.h"
#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

uint32_t    enable_arm_cache   = 1U;
uint32_t    enable_rtos_cache  = 1U;
uint32_t    enable_orc_cache   = 1U;
uint32_t    enable_log_msg     = 0U;
uint32_t    enable_cma         = 0U;
uint32_t    scheduler_id       = 0U;

#ifdef USE_AMBA_KAL
/******************************************************************************/
/*                                  print                                 */
/******************************************************************************/
#define SCHDR_ERR_0000      (SSP_ERR_BASE | 0x00400000UL)
#define AMBA_SCHDR_PRINT_MODULE_ID      ((UINT16)(SCHDR_ERR_0000 >> 16U))     /**< Module ID for AmbaPrint */

void krn_printU5(const char *fmt,
                 uint64_t argv0,
                 uint64_t argv1,
                 uint64_t argv2,
                 uint64_t argv3,
                 uint64_t argv4)
{
    uint32_t  str_start = 0U, str_end = 0U, str_pos = 0U;
    char str_line[512];

    str_pos = krn_snprintf_uint5(&str_line[str_pos], sizeof(str_line), fmt,
                                 (uint32_t)argv0,
                                 (uint32_t)argv1,
                                 (uint32_t)argv2,
                                 (uint32_t)argv3,
                                 (uint32_t)argv4);

    while ( (str_line[str_end] != '\0') && (str_end <= str_pos) ) {
        if (str_line[str_end] == '\n') {
            str_line[str_end] = '\0';
            OSAL_PrintUInt5(&str_line[str_start], 0U, 0U, 0U, 0U, 0U);
            str_start = str_end + 1U;
        }
        str_end++;
    }

    if(str_start < str_end) {
        OSAL_PrintUInt5(&str_line[str_start], 0U, 0U, 0U, 0U, 0U);
    }

}

void krn_printS5(const char *fmt,
                 const char *argv0,
                 const char *argv1,
                 const char *argv2,
                 const char *argv3,
                 const char *argv4)
{
    uint32_t  str_start = 0U, str_end = 0U, str_pos = 0U;
    char str_line[512];

    str_pos = krn_snprintf_str5(&str_line[str_pos], sizeof(str_line), fmt,
                                argv0,
                                argv1,
                                argv2,
                                argv3,
                                argv4);

    while ( (str_line[str_end] != '\0') && (str_end <= str_pos) ) {
        if (str_line[str_end] == '\n') {
            str_line[str_end] = '\0';
            OSAL_PrintUInt5(&str_line[str_start], 0U, 0U, 0U, 0U, 0U);
            str_start = str_end + 1U;
        }
        str_end++;
    }

    if(str_start < str_end) {
        OSAL_PrintUInt5(&str_line[str_start], 0U, 0U, 0U, 0U, 0U);
    }
}

void krn_module_printU5(const char *fmt,
                        uint64_t argv0,
                        uint64_t argv1,
                        uint64_t argv2,
                        uint64_t argv3,
                        uint64_t argv4)
{
    uint32_t  str_start = 0U, str_end = 0U, str_pos = 0U;
    char str_line[512];

    str_pos = krn_snprintf_uint5(&str_line[str_pos], sizeof(str_line), fmt,
                                 (uint32_t)argv0,
                                 (uint32_t)argv1,
                                 (uint32_t)argv2,
                                 (uint32_t)argv3,
                                 (uint32_t)argv4);

    while ( (str_line[str_end] != '\0') && (str_end <= str_pos) ) {
        if (str_line[str_end] == '\n') {
            str_line[str_end] = '\0';
            OSAL_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID, &str_line[str_start], 0U, 0U, 0U, 0U, 0U);
            str_start = str_end + 1U;
        }
        str_end++;
    }

    if(str_start < str_end) {
        OSAL_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID, &str_line[str_start], 0U, 0U, 0U, 0U, 0U);
    }
}

void krn_module_printS5(const char *fmt,
                        const char *argv0,
                        const char *argv1,
                        const char *argv2,
                        const char *argv3,
                        const char *argv4)
{
    uint32_t  str_start = 0U, str_end = 0U, str_pos = 0U;
    char str_line[512];

    str_pos = krn_snprintf_str5(&str_line[str_pos], sizeof(str_line), fmt,
                                argv0,
                                argv1,
                                argv2,
                                argv3,
                                argv4);

    while ( (str_line[str_end] != '\0') && (str_end <= str_pos) ) {
        if (str_line[str_end] == '\n') {
            str_line[str_end] = '\0';
            OSAL_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID, &str_line[str_start], 0U, 0U, 0U, 0U, 0U);
            str_start = str_end + 1U;
        }
        str_end++;
    }

    if(str_start < str_end) {
        OSAL_ModulePrintUInt5(AMBA_SCHDR_PRINT_MODULE_ID, &str_line[str_start], 0U, 0U, 0U, 0U, 0U);
    }
}

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
uint32_t krn_sem_init(ksem_t *sem, int32_t pshared, uint32_t value)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_INIT_FAIL;
        krn_printU5("[ERROR] krn_sem_init() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        (void) pshared;
        ret = OSAL_SemaphoreCreate(sem, NULL, value);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_SEM_INIT_FAIL;
            krn_printU5("[ERROR] krn_sem_init() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_sem_deinit(ksem_t *sem)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_DEINIT_FAIL;
        krn_printU5("[ERROR] krn_sem_deinit() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_SemaphoreDelete(sem);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_SEM_DEINIT_FAIL;
            krn_printU5("[ERROR] krn_sem_deinit() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_sem_post(ksem_t *sem)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_POST_FAIL;
        krn_printU5("[ERROR] krn_sem_post() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_SemaphoreGive(sem);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_SEM_POST_FAIL;
            krn_printU5("[ERROR] krn_sem_post() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_sem_wait(ksem_t *sem, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
        krn_printU5("[ERROR] krn_sem_wait() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_SemaphoreTake(sem, timeout);
        if(ret != 0U) {
            if(ret == KAL_ERR_TIMEOUT) {
                retcode = ERR_DRV_SCHDR_SEM_WAIT_TIMEOUT;
            } else {
                retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
            }
            krn_printU5("[ERROR] krn_sem_wait() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_mutex_init(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
        krn_printU5("[ERROR] krn_mutex_init() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_MutexCreate(mutex, NULL);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
            krn_printU5("[ERROR] krn_mutex_init() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_mutex_deinit(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_DEINIT_FAIL;
        krn_printU5("[ERROR] krn_mutex_deinit() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_MutexDelete(mutex);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_MUTEX_DEINIT_FAIL;
            krn_printU5("[ERROR] krn_mutex_deinit() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_mutex_lock(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
        krn_printU5("[ERROR] krn_mutex_lock() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_MutexTake( mutex, AMBA_KAL_WAIT_FOREVER);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
            krn_printU5("[ERROR] krn_mutex_lock() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_mutex_try_lock(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_TRY_LOCK_FAIL;
        krn_printU5("[ERROR] krn_mutex_try_lock() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_MutexTake( mutex, 1U);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_MUTEX_TRY_LOCK_FAIL;
        }
    }
    return retcode;
}

uint32_t krn_mutex_unlock(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
        krn_printU5("[ERROR] krn_mutex_unlock() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_MutexGive( mutex);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
            krn_printU5("[ERROR] krn_mutex_unlock() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_eventflag_init(keven_t *event)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
        krn_printU5("[ERROR] krn_eventflag_init() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_EventFlagCreate(event, NULL);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
            krn_printU5("[ERROR] krn_eventflag_init() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_eventflag_set(keven_t *event, uint32_t flag)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventflag_set() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_EventFlagSet(event, flag);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
            krn_printU5("[ERROR] krn_eventflag_set() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_eventflag_get(keven_t *event, uint32_t reqflag, uint32_t *actflag, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if((event == NULL) || (actflag == NULL)) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
        krn_printU5("[ERROR] krn_eventflag_get() : event == NULL or actflag == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_EventFlagGet(event, reqflag, 0, 1, actflag, timeout);
        if(ret != 0U) {
            if(ret == KAL_ERR_TIMEOUT) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT;
            } else {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
                krn_printU5("[ERROR] krn_eventflag_get() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
            }
        }
    }
    return retcode;
}

uint32_t krn_eventflag_clear(keven_t *event, uint32_t flag)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventflag_clear() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_EventFlagClear(event, flag);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
            krn_printU5("[ERROR] krn_eventflag_clear() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_eventcond_set(keven_t *event)
{
    uint32_t retcode = ERRCODE_NONE;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_set() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        wake_up_interruptible(&event->wait_queue);
    }
    return retcode;
}

uint32_t krn_eventcond_get(keven_t *event, const uint32_t *condition, const uint32_t *target, uint32_t operation, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if((event == NULL) || (condition == NULL) || (target == NULL)) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_get() : event == NULL or condition == NULL or target == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        if(operation == EVENT_OPT_GL) {
            ret = wait_event_interruptible_timeout(event->wait_queue, (*condition > *target),msecs_to_jiffies(timeout));
            if(ret == 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT;
            } else if ( (ret < 0) && (ret != -ERESTARTSYS)) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
                krn_printU5("[ERROR] krn_eventcondition_get() : fail ret = %d", ret, 0U, 0U, 0U, 0U);
            } else {
                retcode = ERRCODE_NONE;
            }
        } else {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
            krn_printU5("[ERROR] krn_eventcondition_get() : operation %d not support retcode = 0x%x", operation, retcode, 0U, 0U, 0U);
        }
    }
    return retcode;
}

kthread_t *krn_thread_create(kthread_t *thread, kthread_entry_t entry, void *arg,
                             uint32_t priority, uint32_t affinity, uint32_t stack_size, char *stack_buff,
                             char *name)
{
    uint32_t ret;
    AMBA_KAL_TASK_ENTRY_f tsk_entry;
    kthread_t *ret_thread = NULL;

    (void) arg;
    if((thread == NULL) || (entry == NULL)) {
        krn_printU5("[ERROR] krn_thread_create() : arg invalidate ", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&tsk_entry, &entry);
        ret = OSAL_TaskCreate(
                  &ret_thread,
                  name,
                  priority,
                  (AMBA_KAL_TASK_ENTRY_f)tsk_entry,
                  arg,
                  stack_buff,
                  stack_size,
                  1U);

        if(ret != 0U) {
            krn_printU5("[ERROR] krn_thread_create() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return ret_thread;
}

uint32_t krn_thread_delete(kthread_t *thread)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(thread == NULL) {
        retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
        krn_printU5("[ERROR] krn_thread_delete() : arg invalidate retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_TaskDelete(thread);

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
            krn_printU5("[ERROR] krn_thread_delete() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_thread_should_stop(kthread_t *thread)
{
    (void) thread;
    return ERRCODE_NONE;
}

uint32_t krn_sleep(uint32_t msec)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    ret = OSAL_TaskSleep(msec);
    if(ret != 0U) {
        krn_printU5("[ERROR] krn_sleep() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_THREAD_SLEEP_FAIL;
    }
    return retcode;
}

uint64_t krn_cache_size_align(uint64_t size)
{
    return ((size) + ARM_CACHELINE_SIZE - 1UL) & ~(ARM_CACHELINE_SIZE - 1UL);
}

uint64_t krn_cache_addr_align(uint64_t addr)
{
    return addr & ~(ARM_CACHELINE_SIZE - 1UL);
}

uint32_t krn_cache_clean(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t virt_addr = 0UL;
    uint32_t ret;

    if(retcode == ERRCODE_NONE) {
        if(size != 0U) {
            if(ptr == NULL) {
                retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
                krn_printU5("[ERROR] krn_cache_clean() : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
            } else {
                krn_typecast(&virt_addr, &ptr);
                ret = OSAL_Cache_DataClean(virt_addr, size);
                if(ret != 0U) {
                    retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
                    krn_printU5("[ERROR] krn_cache_clean() :  fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
    return retcode;
}

uint32_t krn_cache_invalidate(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t virt_addr = 0UL;
    uint32_t ret;

    if(retcode == ERRCODE_NONE) {
        if(size != 0U) {
            if(ptr == NULL) {
                retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
                krn_printU5("[ERROR] krn_cache_invalidate() : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
            } else {
                krn_typecast(&virt_addr, &ptr);
                ret = OSAL_Cache_DataInvalidate(virt_addr, size);
                if(ret != 0U) {
                    retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
                    krn_printU5("[ERROR] krn_cache_invalidate() :  fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    return retcode;
}

uint32_t krn_copy_to_user(void *to, const void *from, uint64_t n)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if((to == NULL) || (from == NULL)) {
        retcode = ERR_DRV_SCHDR_COPY_TO_USER_FAIL;
        krn_printU5("[ERROR] krn_copy_to_user() : fail retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = copy_to_user(to, from, n);

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_COPY_TO_USER_FAIL;
            krn_printU5("[ERROR] krn_copy_to_user() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_copy_from_user(void *to, const void *from, uint64_t n)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if((to == NULL) || (from == NULL)) {
        retcode = ERR_DRV_SCHDR_COPY_FROM_USER_FAIL;
        krn_printU5("[ERROR] krn_copy_to_user() : fail retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = copy_from_user(to, from, n);

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_COPY_FROM_USER_FAIL;
            krn_printU5("[ERROR] krn_copy_from_user() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_readl_relaxed(void* reg)
{
    uint32_t value = 0U;

    (void) reg;
    if(reg == NULL) {
        krn_printU5("[ERROR] krn_readl_relaxed() : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        value = readl(reg);
    }
    return value;
}

void krn_writel_relaxed(uint32_t val, void* reg)
{
    (void) reg;
    if(reg == NULL) {
        krn_printU5("[ERROR] krn_writel_relaxed() : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        writel_relaxed(val, reg);
    }
}

void *krn_ioremap(uint64_t pa, uint64_t size)
{
    void *ptr = NULL;

    (void) size;
    if((pa == 0U) || (size == 0U)) {
        krn_printU5("[ERROR] krn_ioremap() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = ioremap(pa, size);
    }

    return ptr;
}

void krn_iounmap(void* va, uint64_t size)
{
    (void) size;
    iounmap(va);
}

static kisr_entry_t relay_handler = NULL;
static irqreturn_t krn_isr_dispatch(int irq, void *arg)
{
    relay_handler(irq, arg);
    return IRQ_HANDLED;
}

uint32_t krn_enable_irq(uint32_t irq_num, kisr_entry_t handler)
{
    uint32_t retcode = ERRCODE_NONE;
    AMBA_INT_CONFIG_s irq_config;
    uint32_t ret;

    if(handler == NULL) {
        retcode = ERR_DRV_SCHDR_IRQ_ENABLE_FAIL;
        krn_printU5("[ERROR] krn_enable_irq() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* setup ISR for ORC interrupt */
        irq_config.TriggerType  = AMBA_INT_RISING_EDGE_TRIGGER;
        irq_config.IrqType      = AMBA_INT_IRQ;
        irq_config.CpuTargets   = 0x01U;
        /* setup ISR for ORC interrupt */
        relay_handler = handler;
        ret = OSAL_INT_Config(irq_num, &irq_config, krn_isr_dispatch, 0U);
        if(ret == 0U) {
            ret = OSAL_INT_Enable(irq_num);
        }

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_IRQ_ENABLE_FAIL;
            krn_printU5("[ERROR] krn_enable_irq() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_disable_irq(uint32_t irq_num)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    ret = OSAL_INT_Disable(irq_num);
    if(ret != 0U) {
        retcode = ERR_DRV_SCHDR_IRQ_DISABLE_FAIL;
        krn_printU5("[ERROR] krn_disable_irq() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
    }
    return retcode;
}

#if defined(ENABLE_AMBA_MAL)
uint32_t krn_cma_alloc(void *arg, uint64_t owner)
{
    uint32_t retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;

    (void)arg;
    (void)owner;
    krn_printU5("[ERROR] krn_cma_alloc() : not support", 0U, 0U, 0U, 0U, 0U);

    return retcode;
}

uint32_t krn_cma_free(void *arg)
{
    uint32_t retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;

    (void)arg;
    krn_printU5("[ERROR] krn_cma_free() : not support", 0U, 0U, 0U, 0U, 0U);

    return retcode;
}

uint32_t krn_cma_sync_cache(void *arg)
{
    uint32_t retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;

    (void)arg;
    krn_printU5("[ERROR] krn_cma_sync_cache() : not support", 0U, 0U, 0U, 0U, 0U);

    return retcode;
}

uint32_t krn_cma_get_usage(void *arg)
{
    uint32_t retcode = ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL;

    (void)arg;
    krn_printU5("[ERROR] krn_cma_get_usage() : not support", 0U, 0U, 0U, 0U, 0U);

    return retcode;
}

void * krn_cma_p2v(uint64_t phy, uint32_t *pCacheFlag)
{
    void *vpBuffer = NULL;

    if(AmbaMAL_Phys2Virt(AMBA_MAL_ID_CV_SYS, phy, AMBA_MAL_ATTRI_CACHE, &vpBuffer) != 0U) {
        krn_printU5("[ERROR] krn_cma_p2v() : fail phy 0x%x \n", phy, 0U, 0U, 0U, 0U);
    } else {
        *pCacheFlag = 1U;
    }

    return vpBuffer;
}

uint64_t krn_cma_v2p(void *virt, uint32_t *pCacheFlag)
{
    uint64_t  physaddr = 0UL;

    if(AmbaMAL_Virt2Phys(AMBA_MAL_ID_CV_SYS, virt, &physaddr) != 0U) {
        krn_printU5("[ERROR] krn_cma_v2p() : fail \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        *pCacheFlag = 1U;
    }

    return physaddr;
}
#else
extern struct ambarella_cavalry  cavalry_support;
uint32_t krn_cma_alloc(void *arg, uint64_t owner)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
        krn_printU5("[ERROR] krn_cma_alloc() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        struct cavalry_mem *mem;
        krn_typecast(&mem, &arg);
        ret = cavalry_cma_alloc(&cavalry_support, mem, owner);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
            krn_printU5("[ERROR] krn_cma_alloc() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_cma_free(void *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;
        krn_printU5("[ERROR] krn_cma_free() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        struct cavalry_mem *mem;
        krn_typecast(&mem, &arg);
        ret = cavalry_cma_free(&cavalry_support, mem);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;
            krn_printU5("[ERROR] krn_cma_free() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_cma_sync_cache(void *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
        krn_printU5("[ERROR] krn_cma_sync_cache() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        struct cavalry_cache_mem *cache;
        krn_typecast(&cache, &arg);
        ret = cavalry_cma_sync_cache(&cavalry_support, cache);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
            krn_printU5("[ERROR] krn_cma_sync_cache() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_cma_get_usage(void *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL;
        krn_printU5("[ERROR] krn_cma_get_usage() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        struct cavalry_usage_mem *usage;
        krn_typecast(&usage, &arg);
        ret = cavalry_cma_get_usage(&cavalry_support, usage);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL;
            krn_printU5("[ERROR] krn_cma_get_usage() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

void * krn_cma_p2v(uint64_t phy, uint32_t *pCacheFlag)
{
    int32_t ret;
    void *vpBuffer = NULL;

    if(pCacheFlag == NULL) {
        krn_printU5("[ERROR] krn_cma_p2v() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = cavalry_cma_p2v(&cavalry_support, &vpBuffer, phy, pCacheFlag);
        if(ret != 0U) {
            krn_printU5("[ERROR] krn_cma_p2v() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
            cavalry_cma_dump_range(&cavalry_support);
            dump_stack();
        }
    }
    return vpBuffer;
}

uint64_t krn_cma_v2p(void *virt, uint32_t *pCacheFlag)
{
    int32_t ret;
    uint64_t  physaddr = 0UL;

    if(pCacheFlag == NULL) {
        krn_printU5("[ERROR] krn_cma_v2p() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = cavalry_cma_v2p(&cavalry_support, &physaddr, virt, pCacheFlag);
        if(ret != 0U) {
            krn_printU5("[ERROR] krn_cma_v2p() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
            cavalry_cma_dump_range(&cavalry_support);
            dump_stack();
        }
    }
    return physaddr;
}
#endif
/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
int32_t krn_strcmp(const char *s1, const char *s2)
{
    return OSAL_StringCompare(s1, s2, OSAL_StringLength(s1));
}

uint32_t krn_sprintf_str(char *str, const char *str_src)
{
    size_t len;

    len = OSAL_StringLength(str_src);
    OSAL_StringCopy(str, len + 1U, str_src);

    return len;
}

uint32_t krn_snprintf_uint1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0)
{
    uint32_t rval;

    rval = OSAL_StringPrintUInt32(
               str, strbuf_size, format, 1, &arg0);

    return rval;
}

uint32_t krn_snprintf_uint2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1)
{
    uint32_t rval;
    uint32_t args[2];

    args[0] = arg0;
    args[1] = arg1;

    rval = OSAL_StringPrintUInt32(
               str, strbuf_size, format, 2, args);

    return rval;
}

uint32_t krn_snprintf_uint3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2)
{
    uint32_t rval;
    uint32_t args[3];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;

    rval = OSAL_StringPrintUInt32(
               str, strbuf_size, format, 3, args);

    return rval;
}

uint32_t krn_snprintf_uint4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3)
{
    uint32_t rval;
    uint32_t args[4];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;

    rval = OSAL_StringPrintUInt32(
               str, strbuf_size, format, 4, args);

    return rval;
}

uint32_t krn_snprintf_uint5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4)
{
    uint32_t rval;
    uint32_t args[5];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;

    rval = OSAL_StringPrintUInt32(
               str, strbuf_size, format, 5, args);

    return rval;
}

uint32_t krn_snprintf_str1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0)
{
    uint32_t rval;

    rval = OSAL_StringPrintStr(
               str, strbuf_size, format, 1, &arg0);

    return rval;
}

uint32_t krn_snprintf_str2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1)
{
    const char *args[2];

    args[0] = arg0;
    args[1] = arg1;

    return OSAL_StringPrintStr(str, strbuf_size, format, 2, args);
}

uint32_t krn_snprintf_str3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2)
{
    const char *args[3];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;

    return OSAL_StringPrintStr(str, strbuf_size, format, 3, args);
}

uint32_t krn_snprintf_str4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3)
{
    const char *args[4];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;

    return OSAL_StringPrintStr(str, strbuf_size, format, 4, args);
}

uint32_t krn_snprintf_str5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3,
    const char *arg4)
{
    const char *args[5];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;

    return OSAL_StringPrintStr(str, strbuf_size, format, 5, args);
}

/******************************************************************************/
/*                                  Resource                                 */
/******************************************************************************/
uint32_t krn_get_sod_irq(uint32_t *irq_num)
{
    uint32_t retcode = ERRCODE_NONE;
    extern uint32_t ambacv_irq;

    *irq_num = ambacv_irq;
    return retcode;
}

uint32_t krn_get_clock(uint32_t id, uint32_t *freq)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    if(id == AUDIO_CLOCK_ID) {
        ret = OSAL_GetClkFreq(AMBA_SYS_CLK_AUD_0, freq);
#if defined (CHIP_CV6)
    } else if(id == HSM_CLOCK_ID) {
        ret = OSAL_GetClkFreq(AMBA_SYS_CLK_HSM, freq);
    } else if(id == NVP_CLOCK_ID) {
        ret = OSAL_GetClkFreq(AMBA_SYS_CLK_VISION_NVP, freq);
    } else if(id == GVP_CLOCK_ID) {
        ret = OSAL_GetClkFreq(AMBA_SYS_CLK_VISION_GVP, freq);
    } else if(id == FEX_CLOCK_ID) {
        ret = OSAL_GetClkFreq(AMBA_SYS_CLK_FEX, freq);
    } else if(id == FMA_CLOCK_ID) {
        ret = OSAL_GetClkFreq(AMBA_SYS_CLK_FMA, freq);
#else
    } else if(id == VISION_CLOCK_ID) {
        ret = OSAL_GetClkFreq(AMBA_SYS_CLK_VISION, freq);
#endif
    } else {
        retcode = ERR_DRV_SCHDR_CLOCK_ID_OUT_OF_RANGE;
    }

    if(ret != ERRCODE_NONE) {
        retcode = ERR_DRV_SCHDR_GET_CLOCK_FAIL;
        krn_printU5("[ERROR] krn_get_clock() : id 0x%x fail ret 0x%x", id, ret, 0U, 0U, 0U);
    }
    return retcode;
}

uint32_t krn_get_time(uint32_t id, uint32_t *pSec, uint32_t *pMsec)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t now;

    if(id == UNIX_TIME_ID) {
        now = ktime_to_ms(ktime_get_real());
        if(now != 0U) {
            *pSec = (uint32_t)(now/1000U);
            *pMsec = (uint32_t)(now%1000U);
        }  else {
            *pSec = 0U;
            *pMsec = 0U;
        }
    } else {
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    }

    if(retcode != ERRCODE_NONE) {
        krn_printU5("[ERROR] krn_get_time() : id 0x%x fail ret 0x%x", id, retcode, 0U, 0U, 0U);
    }

    return retcode;
}

uint32_t krn_os_kernel_init(void)
{
    uint32_t retcode = ERRCODE_NONE;

    return retcode;
}

void krn_os_kernel_exit(void)
{

}

#else
/******************************************************************************/
/*                                  print                                 */
/******************************************************************************/
void krn_printU5(const char *fmt,
                 uint64_t argv0,
                 uint64_t argv1,
                 uint64_t argv2,
                 uint64_t argv3,
                 uint64_t argv4)
{
    printk(fmt, (uint32_t)argv0, (uint32_t)argv1, (uint32_t)argv2, (uint32_t)argv3, (uint32_t)argv4);
}

void krn_printS5(const char *fmt,
                 const char *argv0,
                 const char *argv1,
                 const char *argv2,
                 const char *argv3,
                 const char *argv4)
{
    printk(fmt, argv0, argv1, argv2, argv3, argv4);
}

static void krn_module_printk(const char *fmt, ...)
{
    va_list args;
    struct va_format vaf;

    BUG_ON(!fmt);

    va_start(args, fmt);

    vaf.fmt = fmt;
    vaf.va = &args;

    printk(KERN_DEBUG "%pV", &vaf);

    va_end(args);
}

void krn_module_printU5(const char *fmt,
                        uint64_t argv0,
                        uint64_t argv1,
                        uint64_t argv2,
                        uint64_t argv3,
                        uint64_t argv4)
{
    krn_module_printk(fmt, argv0, argv1, argv2, argv3, argv4);
}

void krn_module_printS5(const char *fmt,
                        const char *argv0,
                        const char *argv1,
                        const char *argv2,
                        const char *argv3,
                        const char *argv4)
{
    krn_module_printk(fmt, argv0, argv1, argv2, argv3, argv4);
}

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
uint32_t krn_sem_init(ksem_t *sem, int32_t pshared, uint32_t value)
{
    uint32_t retcode = ERRCODE_NONE;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_INIT_FAIL;
        krn_printU5("[ERROR] krn_sem_init() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        (void) pshared;
        sema_init(sem, value);
    }
    return retcode;
}

uint32_t krn_sem_deinit(ksem_t *sem)
{
    uint32_t retcode = ERRCODE_NONE;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_DEINIT_FAIL;
        krn_printU5("[ERROR] krn_sem_deinit() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }
    return retcode;
}

uint32_t krn_sem_post(ksem_t *sem)
{
    uint32_t retcode = ERRCODE_NONE;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_POST_FAIL;
        krn_printU5("[ERROR] krn_sem_post() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        up(sem);
    }
    return retcode;
}

uint32_t krn_sem_wait(ksem_t *sem, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t jiff;
    int32_t ret = 0;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
        krn_printU5("[ERROR] krn_sem_wait() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        if(timeout == OS_KERNEL_WAIT_FOREVER) {
            down(sem);
        } else {
            jiff = msecs_to_jiffies(timeout);
            ret = down_timeout(sem, jiff);
        }
    }

    if(ret != 0) {
        if(ret == -ETIME) {
            retcode = ERR_DRV_SCHDR_SEM_WAIT_TIMEOUT;
        } else {
            retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
        }
        krn_printU5("[ERROR] krn_sem_wait() : down fail ret = %d", ret, 0U, 0U, 0U, 0U);
    }
    return retcode;
}

uint32_t krn_mutex_init(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
        krn_printU5("[ERROR] krn_mutex_init() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        mutex_init(mutex);
    }
    return retcode;
}

uint32_t krn_mutex_deinit(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) mutex;
    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_DEINIT_FAIL;
        krn_printU5("[ERROR] krn_mutex_deinit() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {

    }
    return retcode;
}

uint32_t krn_mutex_lock(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
        krn_printU5("[ERROR] krn_mutex_lock() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        mutex_lock(mutex);
    }
    return retcode;
}

uint32_t krn_mutex_try_lock(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    int ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_TRY_LOCK_FAIL;
        krn_printU5("[ERROR] krn_mutex_try_lock() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = mutex_trylock(mutex);
        if(ret == 0) {
            retcode = ERR_DRV_SCHDR_MUTEX_TRY_LOCK_FAIL;
        }
    }
    return retcode;
}

uint32_t krn_mutex_unlock(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
        krn_printU5("[ERROR] krn_mutex_unlock() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        mutex_unlock(mutex);
    }
    return retcode;
}

uint32_t krn_eventflag_init(keven_t *event)
{
    uint32_t retcode = ERRCODE_NONE;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
        krn_printU5("[ERROR] krn_eventflag_init() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        atomic_set(&event->flag, 0);
        init_waitqueue_head(&event->wait_queue);
    }
    return retcode;
}

uint32_t krn_eventflag_set(keven_t *event, uint32_t flag)
{
    uint32_t retcode = ERRCODE_NONE;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventflag_set() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        atomic_or(flag, &event->flag);
        wake_up_interruptible(&event->wait_queue);
    }
    return retcode;
}

uint32_t krn_eventflag_get(keven_t *event, uint32_t reqflag, uint32_t *actflag, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if((event == NULL) || (actflag == NULL)) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
        krn_printU5("[ERROR] krn_eventflag_get() : event == NULL or actflag == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = wait_event_interruptible_timeout(event->wait_queue, ((atomic_read(&event->flag) & reqflag) != 0),msecs_to_jiffies(timeout));
        if(ret == 0) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT;
        } else if ( ret < 0) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
        } else {
            retcode = ERRCODE_NONE;
        }
        *actflag = atomic_read(&event->flag);
        atomic_and(~reqflag, &event->flag);
    }
    return retcode;
}

uint32_t krn_eventflag_clear(keven_t *event, uint32_t flag)
{
    uint32_t retcode = ERRCODE_NONE;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventflag_clear() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        atomic_and(~flag, &event->flag);
    }
    return retcode;
}

uint32_t krn_eventcond_set(keven_t *event)
{
    uint32_t retcode = ERRCODE_NONE;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_set() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        wake_up_interruptible(&event->wait_queue);
    }
    return retcode;
}

uint32_t krn_eventcond_get(keven_t *event, const uint32_t *condition, const uint32_t *target, uint32_t operation, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if((event == NULL) || (condition == NULL) || (target == NULL)) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_get() : event == NULL or condition == NULL or target == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        if(operation == EVENT_OPT_GL) {
            ret = wait_event_interruptible_timeout(event->wait_queue, (*condition > *target),msecs_to_jiffies(timeout));
            if(ret == 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT;
            } else if ( (ret < 0) && (ret != -ERESTARTSYS)) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
                krn_printU5("[ERROR] krn_eventcondition_get() : fail ret = %d", ret, 0U, 0U, 0U, 0U);
            } else {
                retcode = ERRCODE_NONE;
            }
        } else {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
            krn_printU5("[ERROR] krn_eventcondition_get() : operation %d not support retcode = 0x%x", operation, retcode, 0U, 0U, 0U);
        }
    }
    return retcode;
}

typedef int (*kthreadfn)(void *data);
kthread_t *krn_thread_create(kthread_t *thread, kthread_entry_t entry, void *arg,
                             uint32_t priority, uint32_t affinity, uint32_t stack_size, char *stack_buff,
                             char *name)
{
    kthreadfn handle;
    kthread_t *ret_thread = NULL;

    (void) arg;
    if((thread == NULL) || (entry == NULL) || (stack_buff == NULL) || (stack_size == 0U)) {
        krn_printU5("[ERROR] krn_thread_create() : arg invalidate ", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&handle, &entry);
        ret_thread = kthread_run(handle, arg, name);
        if(ret_thread == NULL) {
            krn_printU5("[ERROR] krn_thread_create() : fail to alloc thread\n", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return ret_thread;
}

uint32_t krn_thread_delete(kthread_t *thread)
{
    uint32_t retcode = ERRCODE_NONE;

    if(thread == NULL) {
        retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
        krn_printU5("[ERROR] krn_thread_delete() : arg invalidate retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        kthread_stop(thread);
    }
    return retcode;
}

uint32_t krn_thread_should_stop(kthread_t *thread)
{
    return kthread_should_stop();
}

uint32_t krn_sleep(uint32_t msec)
{
    uint32_t retcode = ERRCODE_NONE;

    msleep(msec);
    return retcode;
}

uint64_t krn_cache_size_align(uint64_t size)
{
    return ((size) + ARM_CACHELINE_SIZE - 1UL) & ~(ARM_CACHELINE_SIZE - 1UL);
}

uint64_t krn_cache_addr_align(uint64_t addr)
{
    return addr & ~(ARM_CACHELINE_SIZE - 1UL);
}

uint32_t krn_cache_clean(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) ptr;
    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
            krn_printU5("[ERROR] krn_cache_clean() : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
        } else {
            ambcache_clean_range(ptr, size);
        }
    }
    return retcode;
}

uint32_t krn_cache_invalidate(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) ptr;
    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
            krn_printU5("[ERROR] krn_cache_invalidate() : fail : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            ambcache_inv_range(ptr, size);
        }
    }

    return retcode;
}

uint32_t krn_copy_to_user(void *to, const void *from, uint64_t n)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if((to == NULL) || (from == NULL)) {
        retcode = ERR_DRV_SCHDR_COPY_TO_USER_FAIL;
        krn_printU5("[ERROR] krn_copy_to_user() : fail retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = copy_to_user(to, from, n);

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_COPY_TO_USER_FAIL;
            krn_printU5("[ERROR] krn_copy_to_user() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_copy_from_user(void *to, const void *from, uint64_t n)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if((to == NULL) || (from == NULL)) {
        retcode = ERR_DRV_SCHDR_COPY_FROM_USER_FAIL;
        krn_printU5("[ERROR] krn_copy_to_user() : fail retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = copy_from_user(to, from, n);

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_COPY_FROM_USER_FAIL;
            krn_printU5("[ERROR] krn_copy_from_user() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_readl_relaxed(void* reg)
{
    uint32_t value = 0U;

    (void) reg;
    if(reg == NULL) {
        krn_printU5("[ERROR] krn_readl_relaxed() : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        value = readl(reg);
    }
    return value;
}

void krn_writel_relaxed(uint32_t val, void* reg)
{
    (void) reg;
    if(reg == NULL) {
        krn_printU5("[ERROR] krn_writel_relaxed() : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        writel_relaxed(val, reg);
    }
}

void *krn_ioremap(uint64_t pa, uint64_t size)
{
    void *ptr = NULL;

    (void) size;
    if((pa == 0U) || (size == 0U)) {
        krn_printU5("[ERROR] krn_ioremap() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = ioremap(pa, size);
    }

    return ptr;
}

void krn_iounmap(void* va, uint64_t size)
{
    (void) size;
    iounmap(va);
}

static kisr_entry_t relay_handler;
static irqreturn_t krn_isr_dispatch(int irq, void *arg)
{
    relay_handler(irq, arg);
    return IRQ_HANDLED;
}

uint32_t krn_enable_irq(uint32_t irq_num, kisr_entry_t handler)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(handler == NULL) {
        retcode = ERR_DRV_SCHDR_IRQ_ENABLE_FAIL;
        krn_printU5("[ERROR] krn_enable_irq() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* setup ISR for ORC interrupt */
        relay_handler = handler;
        ret = request_irq(irq_num, krn_isr_dispatch,
                          IRQF_TRIGGER_RISING,
                          "cv_scheduler", NULL);

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_IRQ_ENABLE_FAIL;
            krn_printU5("[ERROR] krn_enable_irq() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_disable_irq(uint32_t irq_num)
{
    uint32_t retcode = ERRCODE_NONE;

    free_irq(irq_num, NULL);
    return retcode;
}

#if defined(ENABLE_AMBA_MAL)
uint32_t krn_cma_alloc(void *arg, uint64_t owner)
{
    uint32_t retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;

    (void)arg;
    (void)owner;
    krn_printU5("[ERROR] krn_cma_alloc() : not support", 0U, 0U, 0U, 0U, 0U);

    return retcode;
}

uint32_t krn_cma_free(void *arg)
{
    uint32_t retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;

    (void)arg;
    krn_printU5("[ERROR] krn_cma_free() : not support", 0U, 0U, 0U, 0U, 0U);

    return retcode;
}

uint32_t krn_cma_sync_cache(void *arg)
{
    uint32_t retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;

    (void)arg;
    krn_printU5("[ERROR] krn_cma_sync_cache() : not support", 0U, 0U, 0U, 0U, 0U);

    return retcode;
}

uint32_t krn_cma_get_usage(void *arg)
{
    uint32_t retcode = ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL;

    (void)arg;
    krn_printU5("[ERROR] krn_cma_get_usage() : not support", 0U, 0U, 0U, 0U, 0U);

    return retcode;
}

void * krn_cma_p2v(uint64_t phy, uint32_t *pCacheFlag)
{
    void *vpBuffer = NULL;

    if(AmbaMAL_Phys2Virt(AMBA_MAL_ID_CV_SYS, phy, AMBA_MAL_ATTRI_CACHE, &vpBuffer) != 0U) {
        krn_printU5("[ERROR] krn_cma_p2v() : fail phy 0x%x \n", phy, 0U, 0U, 0U, 0U);
    } else {
        *pCacheFlag = 1U;
    }

    return vpBuffer;
}

uint64_t krn_cma_v2p(void *virt, uint32_t *pCacheFlag)
{
    uint64_t  physaddr = 0UL;

    if(AmbaMAL_Virt2Phys(AMBA_MAL_ID_CV_SYS, virt, &physaddr) != 0U) {
        krn_printU5("[ERROR] krn_cma_v2p() : fail \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        *pCacheFlag = 1U;
    }

    return physaddr;
}
#else
extern struct ambarella_cavalry  cavalry_support;
uint32_t krn_cma_alloc(void *arg, uint64_t owner)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
        krn_printU5("[ERROR] krn_cma_alloc() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = cavalry_cma_alloc(&cavalry_support, arg, owner);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
            krn_printU5("[ERROR] krn_cma_alloc() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_cma_free(void *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;
        krn_printU5("[ERROR] krn_cma_free() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = cavalry_cma_free(&cavalry_support, arg);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_FREE_FAIL;
            krn_printU5("[ERROR] krn_cma_free() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_cma_sync_cache(void *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
        krn_printU5("[ERROR] krn_cma_sync_cache() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = cavalry_cma_sync_cache(&cavalry_support, arg);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_SYNC_FAIL;
            krn_printU5("[ERROR] krn_cma_sync_cache() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_cma_get_usage(void *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL;
        krn_printU5("[ERROR] krn_cma_get_usage() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = cavalry_cma_get_usage(&cavalry_support, arg);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL;
            krn_printU5("[ERROR] krn_cma_get_usage() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

void* krn_cma_p2v(uint64_t phy, uint32_t *pCacheFlag)
{
    int32_t ret;
    void *vpBuffer = NULL;

    if(pCacheFlag == NULL) {
        krn_printU5("[ERROR] krn_cma_p2v() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = cavalry_cma_p2v(&cavalry_support, &vpBuffer, phy, pCacheFlag);
        if(ret != 0U) {
            krn_printU5("[ERROR] krn_cma_p2v() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
            cavalry_cma_dump_range(&cavalry_support);
            dump_stack();
        }
    }
    return vpBuffer;
}

uint64_t krn_cma_v2p(void *virt, uint32_t *pCacheFlag)
{
    int32_t ret;
    uint64_t physaddr = 0UL;

    if(pCacheFlag == NULL) {
        krn_printU5("[ERROR] krn_cma_v2p() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = cavalry_cma_v2p(&cavalry_support, &physaddr, virt, pCacheFlag);
        if(ret != 0U) {
            krn_printU5("[ERROR] krn_cma_v2p() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
            cavalry_cma_dump_range(&cavalry_support);
            dump_stack();
        }
    }
    return physaddr;
}
#endif
/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
int32_t krn_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

uint32_t krn_sprintf_str(char *str, const char *str_src)
{
    size_t len;

    len = strlen(str);
    strncpy(str, str_src, len + 1U);

    return len;
}

uint32_t krn_snprintf_str1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0)
{
    return krn_snprintf_str5(str, strbuf_size, format, arg0, NULL, NULL, NULL, NULL);
}

uint32_t krn_snprintf_str2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1)
{
    return krn_snprintf_str5(str, strbuf_size, format, arg0, arg1, NULL, NULL, NULL);
}

uint32_t krn_snprintf_str3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2)
{
    return krn_snprintf_str5(str, strbuf_size, format, arg0, arg1, arg2, NULL, NULL);
}

uint32_t krn_snprintf_str4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3)
{
    return krn_snprintf_str5(str, strbuf_size, format, arg0, arg1, arg2, arg3, NULL);
}

uint32_t krn_snprintf_str5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3,
    const char *arg4)
{
    uint32_t idx = 0,cnt = 0;
    int32_t ret = 0;

    if ( format == NULL ) {
        // no action
    } else {
        for (; format[idx] != '\0';) {
            if (format[idx] == '%') {
                cnt++;
            }
            idx++;
        }
        if(cnt == 5) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2, arg3, arg4);
        } else if(cnt == 4) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2, arg3);
        } else if(cnt == 3) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2);
        } else if(cnt == 2) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1);
        } else if(cnt == 1) {
            ret = snprintf(str, strbuf_size, format, arg0);
        } else {
            ret = snprintf(str, strbuf_size, format);
        }
    }

    if(ret >= 0) {
        return ret;
    } else {
        return 0;
    }

}

uint32_t krn_snprintf_uint1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0)
{
    return krn_snprintf_uint5(str, strbuf_size, format, arg0, 0U, 0U, 0U, 0U);
}

uint32_t krn_snprintf_uint2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1)
{
    return krn_snprintf_uint5(str, strbuf_size, format, arg0, arg1, 0U, 0U, 0U);
}

uint32_t krn_snprintf_uint3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2)
{
    return krn_snprintf_uint5(str, strbuf_size, format, arg0, arg1, arg2, 0U, 0U);
}

uint32_t krn_snprintf_uint4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3)
{
    return krn_snprintf_uint5(str, strbuf_size, format, arg0, arg1, arg2, arg3, 0U);
}

uint32_t krn_snprintf_uint5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4)
{
    uint32_t idx = 0,cnt = 0;
    int32_t ret = 0;

    if ( format == NULL ) {
        // no action
    } else {
        for (; format[idx] != '\0';) {
            if (format[idx] == '%') {
                cnt++;
            }
            idx++;
        }
        if(cnt == 5) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2, arg3, arg4);
        } else if(cnt == 4) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2, arg3);
        } else if(cnt == 3) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2);
        } else if(cnt == 2) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1);
        } else if(cnt == 1) {
            ret = snprintf(str, strbuf_size, format, arg0);
        } else {
            ret = snprintf(str, strbuf_size, format);
        }
    }

    if(ret >= 0) {
        return ret;
    } else {
        return 0;
    }

}
/******************************************************************************/
/*                                  Resource                                 */
/******************************************************************************/
uint32_t krn_get_sod_irq(uint32_t *irq_num)
{
    uint32_t retcode = ERRCODE_NONE;
    extern uint32_t ambacv_irq;

    *irq_num = ambacv_irq;
    return retcode;
}

uint32_t krn_get_clock(uint32_t id, uint32_t *freq)
{
    uint32_t retcode = ERRCODE_NONE;
    struct clk *gclk = NULL;

    if(id == AUDIO_CLOCK_ID) {
        gclk = clk_get_sys(NULL, "gclk_audio");
#if defined (CHIP_CV6)
    } else if(id == HSM_CLOCK_ID) {
        gclk = clk_get_sys(NULL, "gclk_hsm");
    } else if(id == NVP_CLOCK_ID) {
        gclk = clk_get_sys(NULL, "gclk_nvp");
    } else if(id == GVP_CLOCK_ID) {
        gclk = clk_get_sys(NULL, "gclk_gvp");
    } else if(id == FEX_CLOCK_ID) {
        gclk = clk_get_sys(NULL, "gclk_fex");
    } else if(id == FMA_CLOCK_ID) {
        gclk = clk_get_sys(NULL, "gclk_fma");
#else
    } else if(id == VISION_CLOCK_ID) {
        gclk = clk_get_sys(NULL, "gclk_vision");
#endif
    } else {
        retcode = ERR_DRV_SCHDR_CLOCK_ID_OUT_OF_RANGE;
    }

    if(retcode == ERRCODE_NONE) {
        if(gclk != NULL) {
            *freq = (uint32_t)clk_get_rate(gclk);
        } else {
            retcode = ERR_DRV_SCHDR_GET_CLOCK_FAIL;
            krn_printU5("[ERROR] krn_get_clock() : id 0x%x fail", id, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_get_time(uint32_t id, uint32_t *pSec, uint32_t *pMsec)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t now;

    if(id == UNIX_TIME_ID) {
        now = ktime_to_ms(ktime_get_real());
        if(now != 0U) {
            *pSec = (uint32_t)(now/1000U);
            *pMsec = (uint32_t)(now%1000U);
        } else {
            *pSec = 0U;
            *pMsec = 0U;
        }
    } else {
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    }

    if(retcode != ERRCODE_NONE) {
        krn_printU5("[ERROR] krn_get_time() : id 0x%x fail ret 0x%x", id, retcode, 0U, 0U, 0U);
    }

    return retcode;
}

uint32_t krn_os_kernel_init(void)
{
    uint32_t retcode = ERRCODE_NONE;

    return retcode;
}

void krn_os_kernel_exit(void)
{

}
#endif
