/**
 *  @file os_kernel.c
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
 *  @details OS kernel APIs
 *
 */
#include <stdint.h>
#include <sys/time.h>
#include "cavalry_ioctl.h"
#include "os_kernel.h"
#include "ambacv_kal.h"
#include "cache_kernel.h"

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
    int32_t ret;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_set() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        /* important notice: this is a blocking call */
        ret = pthread_mutex_lock(&event->mutex);
        if(ret == 0) {
            ret = pthread_cond_broadcast(&event->cond);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
                krn_printU5("[ERROR] krn_eventcondition_set() : pthread_cond_broadcast fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            }
            pthread_mutex_unlock(&event->mutex);
        } else {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
            krn_printU5("[ERROR] krn_eventcondition_set() : pthread_mutex_lock fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

static uint32_t krn_eventcond_poll(uint32_t *condition, uint32_t *target, uint32_t operation)
{
    uint32_t Ret = 1;

    if(operation == EVENT_OPT_GL) {
        if(*condition > *target) {
            Ret = 0U;
        }
    }

    return Ret;
}

uint32_t krn_eventcond_get(keven_t *event, const uint32_t *condition, const uint32_t *target, uint32_t operation, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;
    struct timespec tm;
    uint64_t time;

    if((event == NULL) || (condition == NULL) || (target == NULL)) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_get() : event == NULL or condition == NULL or target == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = clock_gettime(CLOCK_MONOTONIC, &tm);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
            krn_printU5("[ERROR] krn_eventcondition_get() : clock_gettime fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        } else {
            time = timespec2nsec(&tm);
            time += ((uint64_t)timeout*1000000U);
            nsec2timespec(&tm, time);

            /* lock */
            ret = pthread_mutex_timedlock_monotonic(&event->mutex, &tm);
            if(ret == 0) {
                /* poll */
                while (ret != ETIMEDOUT) {
                    if (0U == krn_eventcond_poll(condition, target, operation)) {
                        break;
                    }
                    /* unlock & wait */
                    ret = pthread_cond_timedwait(&event->cond, &event->mutex, &tm);
                }

                /* unlock */
                pthread_mutex_unlock(&event->mutex);
            } else {
                krn_printU5("[ERROR] krn_eventcondition_get() : pthread_mutex_timedlock_monotonic fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            }

            if (ret == ETIMEDOUT) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT;
            } else if (ret != 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
            }
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

    (void) arg;
    (void) affinity;
    if((thread == NULL) || (entry == NULL)) {
        krn_printU5("[ERROR] krn_thread_create() : arg invalidate ", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&tsk_entry, &entry);
        ret = OSAL_TaskCreate(
                  thread,
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
    return thread;
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
        ret = OSAL_memcpy(to, from, n);

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
        ret = OSAL_memcpy(to, from, n);

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_COPY_FROM_USER_FAIL;
            krn_printU5("[ERROR] krn_copy_from_user() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_readl_relaxed(void* reg)
{
    const volatile uint32_t *ptr;
    uint32_t value = 0U;

    (void) reg;
    if(reg == NULL) {
        krn_printU5("[ERROR] krn_readl_relaxed() : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&ptr,&reg);
        value = *ptr;
    }
    return value;
}

void krn_writel_relaxed(uint32_t val, void* reg)
{
    uint32_t *ptr;

    (void) reg;
    if(reg == NULL) {
        krn_printU5("[ERROR] krn_writel_relaxed() : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&ptr,&reg);
        *(volatile uint32_t*)ptr = val;
    }
}

void *krn_ioremap(uint64_t pa, uint64_t size)
{
    void *ptr = NULL;

    (void) size;
    if((pa == 0U) || (size == 0U)) {
        krn_printU5("[ERROR] krn_ioremap() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = (void *)mmap_device_io(size, pa);
    }
    return ptr;
}

void krn_iounmap(void* va, uint64_t size)
{
    munmap_device_io((_Uintptrt)va, size);
}

kisr_entry_t relay_handler;

uint32_t krn_enable_irq(uint32_t irq_num, kisr_entry_t handler)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    (void) irq_num;
    if(handler == NULL) {
        retcode = ERR_DRV_SCHDR_IRQ_ENABLE_FAIL;
        krn_printU5("[ERROR] krn_enable_irq() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* setup ISR for ORC interrupt */
        relay_handler = handler;
        ret = OSAL_INT_Enable(irq_num);
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

extern kernel_memblk_t cma_master;
uint32_t krn_cma_alloc(void *arg, uint64_t owner)
{
    (void) arg;
    (void) owner;
    krn_printU5("[ERROR] krn_cma_alloc : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
}

uint32_t krn_cma_free(void *arg)
{
    (void) arg;
    krn_printU5("[ERROR] krn_cma_free : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_FREE_FAIL;
}

uint32_t krn_cma_sync_cache(void *arg)
{
    (void) arg;
    krn_printU5("[ERROR] krn_cma_sync_cache : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_SYNC_FAIL;
}

uint32_t krn_cma_get_usage(void *arg)
{
    (void) arg;
    krn_printU5("[ERROR] krn_cma_get_usage : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL;
}

void * krn_cma_p2v(uint64_t phy, uint32_t *pCacheFlag)
{
    uint64_t offset = 0UL;
    void *retval = NULL;

    if ((phy >= cma_master.buffer_daddr) && (phy < (cma_master.buffer_daddr + cma_master.buffer_size))) {
        offset = phy - cma_master.buffer_daddr;
        retval = &cma_master.pBuffer[offset];
        *pCacheFlag = cma_master.buffer_cacheable;
    }
    return retval;
}

uint64_t krn_cma_v2p(void *virt, uint32_t *pCacheFlag)
{
    uint64_t  start = 0UL,end = 0UL,target = 0UL;
    uint64_t  offset = 0UL;
    uint64_t  retval = 0UL;

    krn_typecast(&target,&virt);
    krn_typecast(&start,&cma_master.pBuffer);
    end = (start + cma_master.buffer_size);

    if ((target >= start) && (target < end)) {
        offset = (target - start);
        retval = cma_master.buffer_daddr + offset;
        *pCacheFlag = cma_master.buffer_cacheable;
    }

    return retval;
}
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
#define AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ 185U
#define AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ 193U

uint32_t krn_get_sod_irq(uint32_t *irq_num)
{
    uint32_t retcode = ERRCODE_NONE;

#if defined (CHIP_CV2)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV22)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV25)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV28)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV2A)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV2FS)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV22FS)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV5) || defined (CHIP_CV52)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV6)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID76_MORC_ARM0_IRQ0 + scheduler_id;
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif

    return retcode;
}

uint32_t krn_get_clock(uint32_t id, uint32_t *freq)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    if(id == AUDIO_CLOCK_ID) {
        ret = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, freq);
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
    struct timeval now;

    if(id == UNIX_TIME_ID) {
        if(gettimeofday(&now, NULL) == 0U) {
            *pSec = (uint32_t)(now.tv_sec);
            if(now.tv_usec != 0U) {
                *pMsec = (uint32_t)((now.tv_usec)%1000U);
            } else {
                *pMsec = 0U;
            }
        } else {
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
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
                 uint32_t argv0,
                 uint32_t argv1,
                 uint32_t argv2,
                 uint32_t argv3,
                 uint32_t argv4)
{
    printf("[devcv]:");
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
}

void krn_printS5(const char *fmt,
                 const char *argv0,
                 const char *argv1,
                 const char *argv2,
                 const char *argv3,
                 const char *argv4)
{
    printf("[devcv]:");
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
}

void krn_module_printU5(const char *fmt,
                        uint32_t argv0,
                        uint32_t argv1,
                        uint32_t argv2,
                        uint32_t argv3,
                        uint32_t argv4)
{
    printf("[devcv]:");
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
}

void krn_module_printS5(const char *fmt,
                        const char *argv0,
                        const char *argv1,
                        const char *argv2,
                        const char *argv3,
                        const char *argv4)
{
    printf("[devcv]:");
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
    printf("\n");
}

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
uint32_t krn_sem_init(ksem_t *sem, int32_t pshared, uint32_t value)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_INIT_FAIL;
        krn_printU5("[ERROR] krn_sem_init() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = sem_init( sem, 1, value );
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_SEM_INIT_FAIL;
            krn_printU5("[ERROR] krn_sem_init() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_sem_deinit(ksem_t *sem)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_DEINIT_FAIL;
        krn_printU5("[ERROR] krn_sem_deinit() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = sem_destroy( sem);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_SEM_DEINIT_FAIL;
            krn_printU5("[ERROR] krn_sem_deinit() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_sem_post(ksem_t *sem)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_POST_FAIL;
        krn_printU5("[ERROR] krn_sem_post() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = sem_post( sem);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_SEM_POST_FAIL;
            krn_printU5("[ERROR] krn_sem_post() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_sem_wait(ksem_t *sem, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    struct timespec tm;
    int32_t ret;
    uint64_t time;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
        krn_printU5("[ERROR] krn_sem_wait() : sem == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        if(timeout == OS_KERNEL_WAIT_FOREVER) {
            ret = sem_wait( sem);
        } else {
            ret = clock_gettime(CLOCK_MONOTONIC, &tm);
            if(ret != 0) {
                krn_printU5("[ERROR] krn_sem_wait() : clock_gettime fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            } else {
                time = timespec2nsec(&tm);
                time += ((uint64_t)timeout*1000000U);
                nsec2timespec(&tm, time);
                ret = sem_timedwait_monotonic( sem, &tm);
            }
        }

        if ( ret != 0 ) {
            if(errno == ETIMEDOUT) {
                retcode = ERR_DRV_SCHDR_SEM_WAIT_TIMEOUT;
            } else {
                retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
            }
            krn_printU5("[ERROR] krn_sem_wait() : fail ret = %d errno %d ", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_mutex_init(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
        krn_printU5("[ERROR] krn_mutex_init() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = pthread_mutex_init(mutex, NULL);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
            krn_printU5("[ERROR] krn_mutex_init() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_mutex_deinit(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_DEINIT_FAIL;
        krn_printU5("[ERROR] krn_mutex_deinit() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = pthread_mutex_destroy( mutex);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_MUTEX_DEINIT_FAIL;
            krn_printU5("[ERROR] krn_mutex_deinit() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_mutex_lock(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
        krn_printU5("[ERROR] krn_mutex_lock() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = pthread_mutex_lock( mutex);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
            krn_printU5("[ERROR] krn_mutex_lock() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_mutex_try_lock(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_TRY_LOCK_FAIL;
        krn_printU5("[ERROR] krn_mutex_try_lock() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = pthread_mutex_trylock( mutex);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_MUTEX_TRY_LOCK_FAIL;
            krn_printU5("[ERROR] krn_mutex_try_lock() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_mutex_unlock(kmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
        krn_printU5("[ERROR] krn_mutex_unlock() : mutex == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = pthread_mutex_unlock( mutex);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
            krn_printU5("[ERROR] krn_mutex_unlock() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_eventflag_init(keven_t *event)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
        krn_printU5("[ERROR] krn_eventflag_init() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        pthread_condattr_t attr;

        ret = pthread_mutex_init(&event->mutex, NULL);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
            krn_printU5("[ERROR] krn_eventflag_init() : pthread_mutex_init fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }

        if(ret == 0) {
            ret = pthread_condattr_init(&attr);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
                krn_printU5("[ERROR] krn_eventflag_init() : pthread_condattr_init fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            }
        }

        if(ret == 0) {
            ret = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
                krn_printU5("[ERROR] krn_eventflag_init() : pthread_condattr_setclock fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            }
        }

        if(ret == 0) {
            ret = pthread_cond_init(&event->cond, &attr);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
                krn_printU5("[ERROR] krn_eventflag_init() : pthread_cond_init fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            }
        }
        atomic_set(&event->flag, 0U);
    }
    return retcode;
}

uint32_t krn_eventflag_set(keven_t *event, uint32_t flag)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventflag_set() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        /* important notice: this is a blocking call */
        ret = pthread_mutex_lock(&event->mutex);
        if(ret == 0) {
            atomic_set_value(&event->flag, event->flag | flag);
            ret = pthread_cond_broadcast(&event->cond);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
                krn_printU5("[ERROR] krn_eventflag_set() : pthread_cond_broadcast fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            }
            pthread_mutex_unlock(&event->mutex);
        } else {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
            krn_printU5("[ERROR] krn_eventflag_set() : pthread_mutex_lock fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

static uint32_t krn_eventflag_poll(keven_t *event, uint32_t reqflag, uint32_t AnyOrAll,
                                   uint32_t AutoClear, uint32_t *pActualFlags)
{
    uint32_t Ret;

    *pActualFlags = event->flag;

    /* read */
    if ((AnyOrAll == KAL_FLAGS_WAIT_ANY) && (*pActualFlags & reqflag)) {
        Ret = 0U;
    } else if ((AnyOrAll == KAL_FLAGS_WAIT_ALL) && (reqflag == (*pActualFlags & reqflag))) {
        Ret = 0U;
    } else {
        /* poll failure */
        Ret = KAL_ERR_TIMEOUT;
    }

    /*write */
    if ((Ret == 0U) && (AMBA_KAL_FLAGS_CLEAR_AUTO == AutoClear)) {
        atomic_clr_value(&event->flag, reqflag);
        pthread_cond_broadcast(&event->cond);
    }

    return Ret;
}

uint32_t krn_eventflag_get(keven_t *event, uint32_t reqflag, uint32_t *actflag, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    struct timespec tm;
    int32_t ret;
    uint64_t time;

    if((event == NULL) || (actflag == NULL)) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
        krn_printU5("[ERROR] krn_eventflag_get() : event == NULL or actflag == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = clock_gettime(CLOCK_MONOTONIC, &tm);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
            krn_printU5("[ERROR] krn_eventflag_get() : clock_gettime fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        } else {
            time = timespec2nsec(&tm);
            time += ((uint64_t)timeout*1000000U);
            nsec2timespec(&tm, time);

            /* lock */
            ret = pthread_mutex_timedlock_monotonic(&event->mutex, &tm);
            if(ret == 0) {
                /* poll */
                while (ret != ETIMEDOUT) {
                    if (0U == krn_eventflag_poll(event, reqflag, 0, 1, actflag)) {
                        break;
                    }
                    /* unlock & wait */
                    ret = pthread_cond_timedwait(&event->cond, &event->mutex, &tm);
                }

                /* unlock */
                pthread_mutex_unlock(&event->mutex);
            } else {
                krn_printU5("[ERROR] krn_eventflag_get() : pthread_mutex_timedlock_monotonic fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            }

            if (ret == ETIMEDOUT) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT;
            } else if (ret != 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
            }
        }
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
        /* important notice: this is a blocking call */
        pthread_mutex_lock(&event->mutex);
        atomic_clr_value(&event->flag, flag);
        pthread_cond_broadcast(&event->cond);
        pthread_mutex_unlock(&event->mutex);
    }
    return retcode;
}

uint32_t krn_eventcond_set(keven_t *event)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_set() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        /* important notice: this is a blocking call */
        ret = pthread_mutex_lock(&event->mutex);
        if(ret == 0) {
            ret = pthread_cond_broadcast(&event->cond);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_INIT_FAIL;
                krn_printU5("[ERROR] krn_eventcondition_set() : pthread_cond_broadcast fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            }
            pthread_mutex_unlock(&event->mutex);
        } else {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
            krn_printU5("[ERROR] krn_eventcondition_set() : pthread_mutex_lock fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return retcode;
}

static uint32_t krn_eventcond_poll(uint32_t *condition, uint32_t *target, uint32_t operation)
{
    uint32_t Ret = 1;

    if(operation == EVENT_OPT_GL) {
        if(*condition > *target) {
            Ret = 0U;
        }
    }

    return Ret;
}

uint32_t krn_eventcond_get(keven_t *event, const uint32_t *condition, const uint32_t *target, uint32_t operation, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;
    struct timespec tm;
    uint64_t time;

    if((event == NULL) || (condition == NULL) || (target == NULL)) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_get() : event == NULL or condition == NULL or target == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = clock_gettime(CLOCK_MONOTONIC, &tm);
        if(ret != 0) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
            krn_printU5("[ERROR] krn_eventcondition_get() : clock_gettime fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        } else {
            time = timespec2nsec(&tm);
            time += ((uint64_t)timeout*1000000U);
            nsec2timespec(&tm, time);

            /* lock */
            ret = pthread_mutex_timedlock_monotonic(&event->mutex, &tm);
            if(ret == 0) {
                /* poll */
                while (ret != ETIMEDOUT) {
                    if (0U == krn_eventcond_poll(condition, target, operation)) {
                        break;
                    }
                    /* unlock & wait */
                    ret = pthread_cond_timedwait(&event->cond, &event->mutex, &tm);
                }

                /* unlock */
                pthread_mutex_unlock(&event->mutex);
            } else {
                krn_printU5("[ERROR] krn_eventcondition_get() : pthread_mutex_timedlock_monotonic fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            }

            if (ret == ETIMEDOUT) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT;
            } else if (ret != 0) {
                retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
            }
        }
    }
    return retcode;
}

typedef void*(*kthreadfn)(void *data);
kthread_t *krn_thread_create(kthread_t *thread, kthread_entry_t entry, void *arg,
                             uint32_t priority, uint32_t affinity, uint32_t stack_size, char *stack_buff,
                             char *name)
{
    kthreadfn handle;
    int32_t ret;

    (void) arg;
    if((thread == NULL) || (entry == NULL)) {
        krn_printU5("[ERROR] krn_thread_create() : arg invalidate ", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&handle, &entry);
        ret = pthread_create(thread, NULL, handle, arg);
        if(ret != 0) {
            krn_printU5("[ERROR] krn_thread_create() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
        }
    }
    return thread;
}

uint32_t krn_thread_delete(kthread_t *thread)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(thread == NULL) {
        retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
        krn_printU5("[ERROR] krn_thread_delete() : arg invalidate retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        if (thread != NULL) {
            ret = pthread_join(*thread, NULL);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
                krn_printU5("[ERROR] krn_thread_delete() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
            } else {
                thread = NULL;
            }
        }
    }
    return retcode;
}

uint32_t krn_thread_should_stop(kthread_t *thread)
{
    return 0U;
}

uint32_t krn_sleep(uint32_t msec)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    ret = usleep(msec*1000U);
    if(ret != 0) {
        krn_printU5("[ERROR] krn_thread_delete() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
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

static struct cache_ctrl cache_info;
uint32_t krn_cache_clean(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t offset = 0UL;
    int32_t ret;

    if(retcode == ERRCODE_NONE) {
        if(size != 0U) {
            if(ptr == NULL) {
                retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
                krn_printU5("[ERROR] krn_cache_clean() : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
            } else {
                ret = mem_offset(ptr, NOFD, 1, &offset, 0);
                if(ret != 0) {
                    retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
                    krn_printU5("[ERROR] krn_cache_clean() : mem_offset fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
                } else {
                    CACHE_FLUSH(&cache_info, ptr, offset, size);
                }
            }
        }
    }
    return retcode;
}

uint32_t krn_cache_invalidate(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t offset = 0UL;
    int32_t ret;

    if(retcode == ERRCODE_NONE) {
        if(size != 0U) {
            if(ptr == NULL) {
                retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
                krn_printU5("[ERROR] krn_cache_invalidate() : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
            } else {
                ret = mem_offset(ptr, NOFD, 1, &offset, 0);
                if(ret != 0) {
                    retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
                    krn_printU5("[ERROR] krn_cache_invalidate() : mem_offset fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
                } else {
                    CACHE_INVAL(&cache_info, ptr, offset, size);
                }
            }
        }
    }

    return retcode;
}

uint32_t krn_copy_to_user(void *to, const void *from, uint64_t n)
{
    uint32_t retcode = ERRCODE_NONE;

    if((to == NULL) || (from == NULL)) {
        retcode = ERR_DRV_SCHDR_COPY_TO_USER_FAIL;
        krn_printU5("[ERROR] krn_copy_to_user() : fail retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        memcpy(to, from, n);
    }
    return retcode;
}

uint32_t krn_copy_from_user(void *to, const void *from, uint64_t n)
{
    uint32_t retcode = ERRCODE_NONE;

    if((to == NULL) || (from == NULL)) {
        retcode = ERR_DRV_SCHDR_COPY_FROM_USER_FAIL;
        krn_printU5("[ERROR] krn_copy_from_user() : fail retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        memcpy(to, from, n);
    }
    return retcode;
}

uint32_t krn_readl_relaxed(void* reg)
{
    const volatile uint32_t *ptr;
    uint32_t value = 0U;

    (void) reg;
    if(reg == NULL) {
        krn_printU5("[ERROR] krn_readl_relaxed() : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&ptr,&reg);
        value = *ptr;
    }
    return value;
}

void krn_writel_relaxed(uint32_t val, void* reg)
{
    uint32_t *ptr;

    (void) reg;
    if(reg == NULL) {
        krn_printU5("[ERROR] krn_writel_relaxed() : fail reg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&ptr,&reg);
        *(volatile uint32_t*)ptr = val;
    }
}

void *krn_ioremap(uint64_t pa, uint64_t size)
{
    void *ptr = NULL;

    (void) size;
    if((pa == 0U) || (size == 0U)) {
        krn_printU5("[ERROR] krn_ioremap() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = (void *)mmap_device_io(size, pa);
    }
    return ptr;
}

void krn_iounmap(void* va, uint64_t size)
{
    munmap_device_io((_Uintptrt)va, size);
}

kisr_entry_t relay_handler;

uint32_t krn_enable_irq(uint32_t irq_num, kisr_entry_t handler)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    (void) irq_num;
    if(handler == NULL) {
        retcode = ERR_DRV_SCHDR_IRQ_ENABLE_FAIL;
        krn_printU5("[ERROR] krn_enable_irq() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* setup ISR for ORC interrupt */
        relay_handler = handler;
        InterruptUnmask(irq_num, -1);
    }
    return retcode;
}

uint32_t krn_disable_irq(uint32_t irq_num)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    (void) irq_num;
    ret = InterruptMask(irq_num, -1);
    if(ret != 0) {
        retcode = ERR_DRV_SCHDR_IRQ_DISABLE_FAIL;
        krn_printU5("[ERROR] krn_disable_irq() : fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
    }
    return retcode;
}

extern kernel_memblk_t cma_master;
uint32_t krn_cma_alloc(void *arg, uint64_t owner)
{
    (void) arg;
    (void) owner;
    krn_printU5("[ERROR] krn_cma_alloc : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
}

uint32_t krn_cma_free(void *arg)
{
    (void) arg;
    krn_printU5("[ERROR] krn_cma_free : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_FREE_FAIL;
}

uint32_t krn_cma_sync_cache(void *arg)
{
    (void) arg;
    krn_printU5("[ERROR] krn_cma_sync_cache : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_SYNC_FAIL;
}

uint32_t krn_cma_get_usage(void *arg)
{
    (void) arg;
    krn_printU5("[ERROR] krn_cma_get_usage : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL;
}

void * krn_cma_p2v(uint64_t phy, uint32_t *pCacheFlag)
{
    uint64_t offset = 0UL;
    void *retval = NULL;

    if ((phy >= cma_master.buffer_daddr) && (phy < (cma_master.buffer_daddr + cma_master.buffer_size))) {
        offset = phy - cma_master.buffer_daddr;
        retval = &cma_master.pBuffer[offset];
        *pCacheFlag = cma_master.buffer_cacheable;
    }
    return retval;
}

uint64_t krn_cma_v2p(void *virt, uint32_t *pCacheFlag)
{
    uint64_t  start = 0UL,end = 0UL,target = 0UL;
    uint64_t  offset = 0UL;
    uint64_t  retval = 0UL;

    krn_typecast(&target,&virt);
    krn_typecast(&start,&cma_master.pBuffer);
    end = (start + cma_master.buffer_size);

    if ((target >= start) && (target < end)) {
        offset = (target - start);
        retval = cma_master.buffer_daddr + offset;
        *pCacheFlag = cma_master.buffer_cacheable;
    }

    return retval;
}
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

    len = strlen(str_src);
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
#define AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ 185U
#define AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ 193U

uint32_t krn_get_sod_irq(uint32_t *irq_num)
{
    uint32_t retcode = ERRCODE_NONE;

#if defined (CHIP_CV2)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV22)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV25)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV2A) || defined (CHIP_CV2FS)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV22A) || defined (CHIP_CV22FS)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV28)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV5) || defined (CHIP_CV52)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV6)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID76_MORC_ARM0_IRQ0 + scheduler_id;
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif

    return retcode;
}

uint32_t krn_get_clock(uint32_t id, uint32_t *freq)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    if(id == AUDIO_CLOCK_ID) {
        *freq = 12288000;
#if defined (CHIP_CV6)
    } else if(id == HSM_CLOCK_ID) {
        *freq = 432000000;
    } else if(id == NVP_CLOCK_ID) {
        *freq = 1200000000;
    } else if(id == GVP_CLOCK_ID) {
        *freq = 912000000;
    } else if(id == FEX_CLOCK_ID) {
        *freq = 792000000;
    } else if(id == FMA_CLOCK_ID) {
        *freq = 912000000;
#else
    } else if(id == VISION_CLOCK_ID) {
        *freq = 792000000;
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
    struct timeval now;

    if(id == UNIX_TIME_ID) {
        if(gettimeofday(&now, NULL) == 0U) {
            *pSec = (uint32_t)(now.tv_sec);
            if(now.tv_usec != 0U) {
                *pMsec = (uint32_t)((now.tv_usec)%1000U);
            } else {
                *pMsec = 0U;
            }
        } else {
            retcode = ERR_DRV_SCHDR_GET_TIME_FAIL;
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
    int32_t ret;
    uint32_t retcode = ERRCODE_NONE;

    ret = cache_init(0, &cache_info, NULL);
    if(ret != 0) {
        retcode = ERR_DRV_SCHDR_CACHE_INIT_FAIL;
        krn_printU5("[ERROR] krn_cache_clean() : cache_init fail ret = %d errno %d", ret, errno, 0U, 0U, 0U);
    }
    return retcode;
}

void krn_os_kernel_exit(void)
{

}
#endif
