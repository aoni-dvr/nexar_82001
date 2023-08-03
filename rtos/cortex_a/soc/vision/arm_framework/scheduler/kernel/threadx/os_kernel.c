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
#include "AmbaMMU.h"
#include "AmbaTime.h"
#include "AmbaRTC.h"
#include "AmbaRTSL_DRAMC.h"
#include "os_kernel.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "cache_kernel.h"
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaFDT.h"  // for misra depress
#endif

uint32_t    enable_arm_cache   = 1U;
uint32_t    enable_rtos_cache  = 1U;
uint32_t    enable_orc_cache   = 1U;
uint32_t    enable_log_msg     = 0U;
uint32_t    enable_cma         = 0U;
uint32_t    scheduler_id       = 2U;

static kernel_memblk_t cma_master;
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

    str_pos = krn_snprintf_uint5(&str_line[str_pos], (uint32_t)sizeof(str_line), fmt,
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

    str_pos = krn_snprintf_str5(&str_line[str_pos], (uint32_t)sizeof(str_line), fmt,
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

    str_pos = krn_snprintf_uint5(&str_line[str_pos], (uint32_t)sizeof(str_line), fmt,
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

    str_pos = krn_snprintf_str5(&str_line[str_pos], (uint32_t)sizeof(str_line), fmt,
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
        ret = OSAL_MutexTake(mutex, AMBA_KAL_WAIT_FOREVER);
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
        ret = OSAL_MutexTake(mutex, 1U);
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
        ret = OSAL_MutexGive(mutex);
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
    uint32_t ret;

    if(event == NULL) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_set() : event == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        ret = OSAL_EventFlagSet(event, 0x1);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_SET_FAIL;
            krn_printU5("[ERROR] krn_eventcond_set() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_eventcond_get(keven_t *event, const uint32_t *condition, const uint32_t *target, uint32_t operation, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t actflag;
    uint32_t ret;

    if((event == NULL) || (condition == NULL) || (target == NULL)) {
        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
        krn_printU5("[ERROR] krn_eventcondition_get() : event == NULL or condition == NULL or target == NULL retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        if(operation == (uint32_t)EVENT_OPT_GL) {
            if(*condition <= *target) {
                ret = OSAL_EventFlagGet(event, 0x1U, 0U, 1U, &actflag, timeout);
                if(ret != 0U) {
                    if(ret == KAL_ERR_TIMEOUT) {
                        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_TIMEOUT;
                    } else {
                        retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
                        krn_printU5("[ERROR] krn_eventcondition_get() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
                    }
                }
            }
        } else {
            retcode = ERR_DRV_SCHDR_EVENTFLAG_GET_FAIL;
            krn_printU5("[ERROR] krn_eventcondition_get() : operation %d not support retcode = 0x%x", operation, retcode, 0U, 0U, 0U);
        }
    }
    return retcode;
}

kthread_t *krn_thread_create(kthread_t *thread, kthread_entry_t entry, const void *arg,
                             uint32_t priority, uint32_t affinity, uint32_t stack_size, char *stack_buff,
                             char *name)
{
    uint32_t ret;
    AMBA_KAL_TASK_ENTRY_f tsk_entry;

    (void) arg;
    if((thread == NULL) || (entry == NULL) || (stack_buff == NULL) || (stack_size == 0U)) {
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
                  0U);

        if(ret == 0U) {
            ret = OSAL_TaskSetSmpAffinity(thread, affinity);
        }

        if(ret == 0U) {
            ret = OSAL_TaskResume(thread);
        }

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
    AMBA_KAL_TASK_INFO_s TaskInfo;
    uint32_t CheckCnt = 200U;
    uint32_t Leave = 0U;

    if(thread == NULL) {
        retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
        krn_printU5("[ERROR] krn_thread_delete() : arg invalidate retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
    } else {
        do {
            if (OSAL_TaskQuery(thread, &TaskInfo) == TX_SUCCESS) {
                CheckCnt--;
                if (CheckCnt == 0U) {
                    Leave = 1U;
                }
            } else {
                Leave = 1U;
            }

            if (Leave != 0U) {
                break;
            }
            if( krn_sleep(10U) != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_thread_delete() : krn_sleep fail ", 0U, 0U, 0U, 0U, 0U);
                Leave = 1U;
            }
        } while (TaskInfo.TaskState != TX_COMPLETED);
        ret = OSAL_TaskTerminate(thread);

        if(ret == 0U) {
            ret = OSAL_TaskDelete(thread);
        }

        if(TaskInfo.TaskState != TX_COMPLETED) {
            krn_printU5("[WARNING] krn_thread_delete() : timeout\n", 0, 0, 0, 0, 0);
        }

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
            krn_printU5("[ERROR] krn_thread_delete() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t krn_thread_should_stop(const kthread_t *thread)
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
    uint64_t u64addr = 0UL;
    uint64_t size_align = 0UL,addr_align = 0UL;
    uint64_t pa_start = 0UL,pa_end = 0UL;
    uint32_t ret;
    const void *paddr = NULL;

    (void) ptr;
    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
            krn_printU5("[ERROR] krn_cache_clean() : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
        } else {
            pa_start = krn_v2p(ptr);
            pa_end = pa_start + size - 1U;
            if(pa_start == 0U) {
                retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
                krn_typecast(&u64addr,&ptr);
                krn_printU5("[ERROR] krn_cache_clean() : fail : va 0x%x is in invalid range", (uint32_t)u64addr, 0U, 0U, 0U, 0U);
            } else if(krn_p2v(pa_end) == NULL) {
                retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
                krn_typecast(&u64addr,&ptr);
                krn_printU5("[ERROR] krn_cache_clean() : fail : size 0x%x is in invalid range, va 0x%x", size, (uint32_t)u64addr, 0U, 0U, 0U);
            } else {
                addr_align = krn_cache_addr_align(pa_start);
                size_align = size + (pa_start - addr_align);
                size_align = krn_cache_size_align(size_align);

                paddr = krn_p2v(addr_align);
                krn_typecast(&u64addr,&paddr);
                ret = OSAL_Cache_DataClean((ULONG)u64addr, (ULONG)size_align);
                if(ret != 0U) {
                    retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
                    krn_printU5("[ERROR] krn_cache_clean() : fail : ret = 0x%x addr = 0x%x size = 0x%x", ret, (uint32_t)u64addr, size, 0U, 0U);
                }
            }
        }
    }
    return retcode;
}

uint32_t krn_cache_invalidate(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t u64addr = 0UL;
    uint64_t size_align = 0UL,addr_align = 0UL;
    uint64_t pa_start = 0UL,pa_end = 0UL;
    uint32_t ret;
    const void *paddr = NULL;

    (void) ptr;
    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
            krn_printU5("[ERROR] krn_cache_invalidate() : fail : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            pa_start = krn_v2p(ptr);
            pa_end = pa_start + size - 1U;
            if(pa_start == 0U) {
                retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
                krn_typecast(&u64addr,&ptr);
                krn_printU5("[ERROR] krn_cache_invalidate() : fail : va 0x%x is in invalid range", (uint32_t)u64addr, 0U, 0U, 0U, 0U);
            } else if(krn_p2v(pa_end) == NULL) {
                retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
                krn_typecast(&u64addr,&ptr);
                krn_printU5("[ERROR] krn_cache_invalidate() : fail : size 0x%x is in invalid range, va 0x%x", size, (uint32_t)u64addr, 0U, 0U, 0U);
            } else {
                addr_align = krn_cache_addr_align(pa_start);
                size_align = size + (pa_start - addr_align);
                size_align = krn_cache_size_align(size_align);

                paddr = krn_p2v(addr_align);
                krn_typecast(&u64addr,&paddr);
                ret = OSAL_Cache_DataInvalidate((ULONG)u64addr,(ULONG)size_align);
                if(ret != 0U) {
                    retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
                    krn_printU5("[ERROR] krn_cache_invalidate() : fail : ret = 0x%x addr = 0x%x size = 0x%x", ret, (uint32_t)u64addr, size, 0U, 0U);
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
        ret = OSAL_memcpy(to, from, (SIZE_t)n);

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
        ret = OSAL_memcpy(to, from, (SIZE_t)n);

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
    ULONG va = 0UL;
    void *ptr = NULL;

    (void) size;
    if((pa == 0U) || (size == 0U)) {
        krn_printU5("[ERROR] krn_ioremap() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        if( AmbaMMU_PhysToVirt((ULONG)pa, &va) == ERRCODE_NONE) {
            krn_typecast(&ptr, &va);
        }
    }

    return ptr;
}

void krn_iounmap(const void* va, uint64_t size)
{
    ULONG VirtAddr;
    ULONG PhysAddr;

    (void) size;
    krn_typecast(&VirtAddr, &va);
    if( AmbaMMU_VirtToPhys(VirtAddr, &PhysAddr) == ERRCODE_NONE) {
        krn_printU5("[ERROR] krn_iounmap() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    }
}

uint32_t krn_enable_irq(uint32_t irq_num, kisr_entry_t handler)
{
    uint32_t retcode = ERRCODE_NONE;
    AMBA_INT_CONFIG_s irq_config;
    AMBA_INT_ISR_f isr_entry;
    uint32_t ret;

    if(handler == NULL) {
        retcode = ERR_DRV_SCHDR_IRQ_ENABLE_FAIL;
        krn_printU5("[ERROR] krn_enable_irq() : invalid arg", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* setup ISR for ORC interrupt */
        irq_config.TriggerType  = AMBA_INT_RISING_EDGE_TRIGGER;
        irq_config.IrqType      = AMBA_INT_IRQ;
        irq_config.CpuTargets   = 0x01U;
        krn_typecast(&isr_entry, &handler);
        ret = OSAL_INT_Config(irq_num, &irq_config, isr_entry, 0U);
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

uint32_t krn_cma_alloc(const void *arg, uint64_t owner)
{
    (void) arg;
    (void) owner;
    krn_printU5("[ERROR] krn_cma_alloc : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_ALLOC_FAIL;
}

uint32_t krn_cma_free(const void *arg)
{
    (void) arg;
    krn_printU5("[ERROR] krn_cma_free : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_FREE_FAIL;
}

uint32_t krn_cma_sync_cache(const void *arg)
{
    (void) arg;
    krn_printU5("[ERROR] krn_cma_sync_cache : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_SYNC_FAIL;
}

uint32_t krn_cma_get_usage(const void *arg)
{
    (void) arg;
    krn_printU5("[ERROR] krn_cma_get_usage : not support", 0U, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_CMA_GET_USAGE_FAIL;
}

void * krn_cma_p2v(uint64_t phy, uint32_t *pCacheFlag)
{
    uint64_t offset = 0UL;
    void *retval = NULL;

    if(phy == 0U) {
        krn_printU5("[ERROR] krn_cma_p2v() : phy == 0U \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        if ((phy >= cma_master.buffer_daddr) && (phy < (cma_master.buffer_daddr + cma_master.buffer_size))) {
            offset = phy - cma_master.buffer_daddr;
            retval = &cma_master.pBuffer[offset];
            *pCacheFlag = 1U;
        } else {
            krn_printU5("[ERROR] krn_cma_p2v() : fail: pa (0x%x)\n", phy, 0U, 0U, 0U, 0U);
        }
    }
    return retval;
} /* krn_p2v() */

uint64_t krn_cma_v2p(void *virt, uint32_t *pCacheFlag)
{
    uint64_t  start = 0UL,target = 0UL;
    uint64_t  offset = 0UL;
    uint64_t  retval = 0UL;

    (void) virt;
    if(virt == NULL) {
        krn_printU5("[ERROR] krn_cma_v2p() : virt == NULL \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&target,&virt);
        krn_typecast(&start,&cma_master.pBuffer);
        if (target >= start) {
            offset = (target - start);
            if(offset < cma_master.buffer_size) {
                retval = cma_master.buffer_daddr + offset;
                *pCacheFlag = 1U;
            }
        }

        if(retval == 0U) {
            krn_printU5("[ERROR] krn_cma_v2p() : Unable to find mapping for virt (%x)\n", (uint32_t)target, 0U, 0U, 0U, 0U);
        } /* if (phy is not in [kernel_master, rtos_master]) */
    }

    return retval;
} /* krn_v2p() */

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

    return (uint32_t)len;
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
#if ! defined(CONFIG_DEVICE_TREE_SUPPORT)
extern uint32_t __cv_schdr_start, __cv_schdr_end;
extern uint32_t __cv_sys_start, __cv_sys_end;
extern uint32_t __cv_rtos_user_start, __cv_rtos_user_end;
extern uint32_t __cv_start, __cv_end;
#endif

uint32_t krn_get_cv_range(uint64_t *start, uint64_t *end)
{
    uint32_t retcode = ERRCODE_NONE;
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    const void *fdt;
    int32_t ret, offset, len;
    const char *chr1;
    const uint32_t *ptr1;
    const uint64_t *ptr64;
    const struct fdt_property *prop = NULL;
    uint64_t dtb_addr = 0UL;

    dtb_addr = (uint64_t)CONFIG_DTB_LOADADDR;
    krn_typecast(&fdt, &dtb_addr);

    ret = AmbaFDT_CheckHeader(fdt);
    if (ret != 0) {
        krn_printU5("[ERROR] krn_get_cv_range() : AmbaFDT_CheckHeader ret = %d", (uint32_t)ret, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_FDT_CHECK_HEADER_FAIL;
    } else {
        offset = AmbaFDT_PathOffset(fdt, "scheduler");
        if (offset < 0) {
            krn_printU5("[ERROR] krn_get_cv_range() : invalid fdt offset %d", (uint32_t)offset, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_FDT_PATH_OFFSET_FAIL;
        } else {
            prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_pa", &len);
            if ((prop != NULL) && (len == 4)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr1, &chr1);
                *start = (uint64_t)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            } else if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr64, &chr1);
                *start = AmbaFDT_Fdt64ToCpu(ptr64[0]);
            } else {
                krn_printU5("[ERROR] krn_get_cv_range() : invalid fdt property", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_FDT_GET_PROPERTY_FAIL;
            }

            prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_size", &len);
            if ((prop != NULL) && (len == 4)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr1, &chr1);
                *end = (uint64_t)AmbaFDT_Fdt32ToCpu(ptr1[0]) + *start;
            } else if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr64, &chr1);
                *end = AmbaFDT_Fdt64ToCpu(ptr64[0]) + *start;
            } else {
                krn_printU5("[ERROR] krn_get_cv_range() : invalid fdt property", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_FDT_GET_PROPERTY_FAIL;
            }
        }
    }
#else
    const uint32_t *pU32;
    uint64_t BufAddr = 0UL;

    pU32 = &__cv_start;
    krn_typecast(&BufAddr,&pU32);
    *start = BufAddr;

    pU32 = &__cv_end;
    krn_typecast(&BufAddr,&pU32);
    *end = BufAddr;
#endif

    return retcode;
}

uint32_t krn_get_cv_rtos_range(uint64_t *start, uint64_t *end)
{
    uint32_t retcode = ERRCODE_NONE;
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    const void *fdt;
    int32_t ret, offset, len;
    const char *chr1;
    const uint32_t *ptr1;
    const uint64_t *ptr64;
    const struct fdt_property *prop = NULL;
    uint64_t dtb_addr = 0UL;

    dtb_addr = (uint64_t)CONFIG_DTB_LOADADDR;
    krn_typecast(&fdt, &dtb_addr);

    ret = AmbaFDT_CheckHeader(fdt);
    if (ret != 0) {
        krn_printU5("[ERROR] krn_get_cv_rtos_range() : AmbaFDT_CheckHeader ret = %d", (uint32_t)ret, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_FDT_CHECK_HEADER_FAIL;
    } else {
        offset = AmbaFDT_PathOffset(fdt, "flexidag_rtos");
        if (offset < 0) {
            krn_printU5("[ERROR] krn_get_cv_rtos_range() : invalid fdt offset %d", (uint32_t)offset, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_FDT_PATH_OFFSET_FAIL;
        } else {
            prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
            if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr1, &chr1);
                *start = (uint64_t)AmbaFDT_Fdt32ToCpu(ptr1[0]);
                *end = (uint64_t)AmbaFDT_Fdt32ToCpu(ptr1[1]) + *start;
            } else if ((prop != NULL) && (len == 16)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr64, &chr1);
                *start = AmbaFDT_Fdt64ToCpu(ptr64[0]);
                *end = AmbaFDT_Fdt64ToCpu(ptr64[1]) + *start;
            } else {
                krn_printU5("[ERROR] krn_get_cv_rtos_range() : invalid fdt property", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_FDT_GET_PROPERTY_FAIL;
            }
        }
    }
#else
    const uint32_t *pU32;
    uint64_t BufAddr = 0UL;

    pU32 = &__cv_rtos_user_start;
    krn_typecast(&BufAddr,&pU32);
    *start = BufAddr;

    pU32 = &__cv_rtos_user_end;
    krn_typecast(&BufAddr,&pU32);
    *end = BufAddr;
#endif

    return retcode;
}

uint32_t krn_get_cv_sys_range(uint64_t *start, uint64_t *end)
{
    uint32_t retcode = ERRCODE_NONE;
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    const void *fdt;
    int32_t ret, offset, len;
    const char *chr1;
    const uint32_t *ptr1;
    const uint64_t *ptr64;
    const struct fdt_property *prop = NULL;
    uint64_t dtb_addr = 0UL;

    dtb_addr = (uint64_t)CONFIG_DTB_LOADADDR;
    krn_typecast(&fdt, &dtb_addr);

    ret = AmbaFDT_CheckHeader(fdt);
    if (ret != 0) {
        krn_printU5("[ERROR] krn_get_cv_sys_range() : AmbaFDT_CheckHeader ret = %d", (uint32_t)ret, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_FDT_CHECK_HEADER_FAIL;
    } else {
        offset = AmbaFDT_PathOffset(fdt, "flexidag_sys");
        if (offset < 0) {
            krn_printU5("[ERROR] krn_get_cv_sys_range() : invalid fdt offset %d", (uint32_t)offset, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_FDT_PATH_OFFSET_FAIL;
        } else {
            prop = AmbaFDT_GetProperty(fdt, offset, "reg", &len);
            if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr1, &chr1);
                *start = (uint64_t)AmbaFDT_Fdt32ToCpu(ptr1[0]);
                *end = (uint64_t)AmbaFDT_Fdt32ToCpu(ptr1[1]) + *start;
            } else if ((prop != NULL) && (len == 16)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr64, &chr1);
                *start = AmbaFDT_Fdt64ToCpu(ptr64[0]);
                *end = AmbaFDT_Fdt64ToCpu(ptr64[1]) + *start;
            } else {
                krn_printU5("[ERROR] krn_get_cv_sys_range() : invalid fdt property", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_FDT_GET_PROPERTY_FAIL;
            }
        }
    }
#else
    const uint32_t *pU32;
    uint64_t BufAddr = 0UL;

    pU32 = &__cv_sys_start;
    krn_typecast(&BufAddr,&pU32);
    *start = BufAddr;

    pU32 = &__cv_sys_end;
    krn_typecast(&BufAddr,&pU32);
    *end = BufAddr;
#endif

    return retcode;
}

uint32_t krn_get_cv_schdr_range(uint64_t *start, uint64_t *end)
{
    uint32_t retcode = ERRCODE_NONE;
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    const void *fdt;
    int32_t ret, offset, len;
    const char *chr1;
    const uint32_t *ptr1;
    const uint64_t *ptr64;
    const struct fdt_property *prop = NULL;
    uint64_t dtb_addr = 0UL;

    dtb_addr = (uint64_t)CONFIG_DTB_LOADADDR;
    krn_typecast(&fdt, &dtb_addr);

    ret = AmbaFDT_CheckHeader(fdt);
    if (ret != 0) {
        krn_printU5("[ERROR] krn_get_cv_schdr_range() : AmbaFDT_CheckHeader ret = %d", (uint32_t)ret, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_FDT_CHECK_HEADER_FAIL;
    } else {
        offset = AmbaFDT_PathOffset(fdt, "scheduler");
        if (offset < 0) {
            krn_printU5("[ERROR] krn_get_cv_schdr_range() : invalid fdt offset %d", (uint32_t)offset, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_FDT_PATH_OFFSET_FAIL;
        } else {
            prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_pa", &len);
            if ((prop != NULL) && (len == 4)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr1, &chr1);
                *start = (uint64_t)AmbaFDT_Fdt32ToCpu(ptr1[0]);
            } else if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr64, &chr1);
                *start = AmbaFDT_Fdt64ToCpu(ptr64[0]);
            } else {
                krn_printU5("[ERROR] krn_get_cv_schdr_range() : invalid fdt property", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_FDT_GET_PROPERTY_FAIL;
            }

            prop = AmbaFDT_GetProperty(fdt, offset, "cv_schdr_size", &len);
            if ((prop != NULL) && (len == 4)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr1, &chr1);
                *end = (uint64_t)AmbaFDT_Fdt32ToCpu(ptr1[0]) + *start;
            } else if ((prop != NULL) && (len == 8)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr64, &chr1);
                *end = AmbaFDT_Fdt64ToCpu(ptr64[0]) + *start;
            } else {
                krn_printU5("[ERROR] krn_get_cv_schdr_range() : invalid fdt property", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_FDT_GET_PROPERTY_FAIL;
            }
        }
    }
#else
    const uint32_t *pU32;
    uint64_t BufAddr = 0UL;

    pU32 = &__cv_schdr_start;
    krn_typecast(&BufAddr,&pU32);
    *start = BufAddr;

    pU32 = &__cv_schdr_end;
    krn_typecast(&BufAddr,&pU32);
    *end = BufAddr;
#endif

    return retcode;
}

uint32_t krn_get_att_range(uint64_t *start, uint64_t *end)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t pa_start = 0UL,pa_end = 0UL;
#if (defined (CHIP_CV22) && defined (CONFIG_CV_CONFIG_TX_SDK7)) || (!defined (CONFIG_CV_CONFIG_TX_SDK7) && !(defined (CHIP_CV2A) || defined (CHIP_CV2FS) || defined (CHIP_CV22FS)))
    uint32_t ret;
    uint64_t ca_start = 0UL,ca_end = 0UL;
    ULONG ca_size = 0UL;
    ULONG tmp1 = 0U,tmp2 = 0U;

    retcode = krn_get_cv_range(&pa_start, &pa_end);
    if (retcode == ERRCODE_NONE) {
        ret = AmbaRTSL_DramGetClientInfo(AMBA_DRAM_CLIENT_ORCVP, &tmp1, &ca_size);
        if( (ret == 0U) && (ca_size != 0U) ) {
            ca_start = tmp1;
            ca_end = ca_start + (uint64_t)ca_size;
            krn_module_printU5("krn_get_att_range AMBA_DRAM_CLIENT_ORCVP ca (0x%x) size (0x%x) ", ca_start, ca_size, 0U, 0U, 0U);
            if(AmbaRTSL_DramQueryAttP2V((ULONG)pa_start, AMBA_DRAM_CLIENT_ORCVP, &tmp2) == 0U) {
                *start = tmp2;
                *end = *start + (pa_end - pa_start);
                if( (pa_start < ca_start) || (pa_end > ca_end)) {
                    krn_printU5("[ERROR] krn_get_att_range() : pa (0x%x - 0x%x) is not in ATT range (0x%x - 0x%x)", pa_start, pa_end, ca_start, ca_end, 0U);
                    *start = pa_start;
                    *end = pa_end;
                    retcode = ERR_DRV_SCHDR_GET_ATT_RANGE_FAIL;
                }
            } else {
                krn_printU5("[ERROR] krn_get_att_range() : AmbaRTSL_DramQueryAttP2V fail pa (0x%x - 0x%x) ca (0x%x - 0x%x)", pa_start, pa_end, ca_start, ca_end, 0U);
                *start = pa_start;
                *end = pa_end;
                retcode = ERR_DRV_SCHDR_GET_ATT_RANGE_FAIL;
            }
        } else {
            *start = pa_start;
            *end = pa_end;
        }
    }
#else
    retcode = krn_get_cv_range(&pa_start, &pa_end);
    if (retcode == ERRCODE_NONE) {
        *start = pa_start;
        *end = pa_end;
    }
#endif

    return retcode;
} /* get_att_range() */

uint32_t krn_get_sod_irq(uint32_t *irq_num)
{
    uint32_t retcode = ERRCODE_NONE;
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    const void *fdt;
    int32_t ret, offset, len;
    const char *chr1;
    const uint32_t *ptr1;
    const struct fdt_property *prop = NULL;
    uint64_t dtb_addr = 0UL;

    dtb_addr = CONFIG_DTB_LOADADDR;
    krn_typecast(&fdt, &dtb_addr);

    ret = AmbaFDT_CheckHeader(fdt);
    if (ret != 0) {
        krn_printU5("[ERROR] krn_get_sod_irq() : AmbaFDT_CheckHeader ret = %d", (uint32_t)ret, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_FDT_CHECK_HEADER_FAIL;
    } else {
        offset = AmbaFDT_PathOffset(fdt, "scheduler");
        if (offset < 0) {
            krn_printU5("[ERROR] krn_get_sod_irq() : invalid fdt offset %d", (uint32_t)offset, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_FDT_PATH_OFFSET_FAIL;
        } else {
            prop = AmbaFDT_GetProperty(fdt, offset, "interrupts", &len);
            if ((prop != NULL) && (len > 4)) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr1, &chr1);
                *irq_num = (uint32_t)AmbaFDT_Fdt32ToCpu(ptr1[1]) + 32U + scheduler_id;
            } else {
                krn_printU5("[ERROR] krn_get_sod_irq() : invalid fdt property", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_FDT_GET_PROPERTY_FAIL;
            }
        }
    }
#else
#if defined (CHIP_CV2)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV22)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV25)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV28)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID185_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV2A) || defined (CHIP_CV2FS)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV22A) || defined (CHIP_CV22FS)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID193_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV5) || defined (CHIP_CV52)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID201_VORC_THREAD0_IRQ + scheduler_id;
#elif defined (CHIP_CV6)
    *irq_num = (uint32_t)AMBA_INT_SPI_ID76_MORC_ARM0_IRQ0 + scheduler_id;
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif
#endif

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
    AMBA_RTC_DATE_TIME_s now;
    UINT32 TimeStamp;

    if(id == UNIX_TIME_ID) {
        if(AmbaRTC_GetSysTime(&now) == 0U) {
            if( AmbaTime_UtcDateTime2TimeStamp(&now, &TimeStamp) == 0U) {
                *pSec = TimeStamp;
                *pMsec = 0U;
            } else {
                retcode = ERR_DRV_SCHDR_GET_TIME_FAIL;
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

static uint32_t os_kernel_init = 0U;

uint32_t krn_os_kernel_init(void)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_all_mem_t *mem_all;
    uint64_t start = 0UL, end = 0UL, tmp = 0UL, value = 0UL;
    void *virt;
    visorc_init_params_t *cfg;
    char *src;
    const char *dts;

    mem_all = krn_ambacv_get_mem();
    retcode = krn_get_cv_range(&start, &end);
    if(retcode == ERRCODE_NONE) {
        mem_all->cv_region.base = start;
        value = (end - start);
        mem_all->cv_region.size = value;

        retcode = krn_get_att_range(&start, &end);
        if(retcode == ERRCODE_NONE) {
            mem_all->cv_att_region.base = start;
            value = (end - start);
            mem_all->cv_att_region.size = value;
        } else {
            krn_printU5("[ERROR] krn_os_kernel_init() : fail", 0U, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        retcode = krn_get_cv_schdr_range(&start, &end);
        if(retcode == ERRCODE_NONE) {
            mem_all->all.base   = (uint64_t) krn_p2c(start);
            value = (end - start);
            value = (value - CAVALRY_MEM_LOG_SIZE);
            mem_all->all.size   = value;
            virt = krn_ioremap(start,(end - start));
            if(virt != NULL) {
                krn_mmap_add_master(virt, start, (end - start), enable_arm_cache);

                krn_typecast(&src, &virt);
                dts = &src[SYSINIT_OFFSET];
                krn_typecast(&cfg, &dts);
                krn_ambacv_set_sysinit(cfg);

                mem_all->cma_region.base = krn_p2c(end);
                cma_master.pBuffer = &src[(end - start)];
                tmp = end;
                retcode = krn_get_cv_sys_range(&start, &end);
                if(retcode == ERRCODE_NONE) {
                    value = (end - tmp);
                    mem_all->cma_region.size = value;

                    cma_master.buffer_cacheable = 1U;
                    cma_master.buffer_daddr = krn_c2p(mem_all->cma_region.base);
                    cma_master.buffer_size = mem_all->cma_region.size;
                }
            } else {
                krn_printU5("[ERROR] krn_os_kernel_init() : krn_ioremap fail", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_CORE_MAP_FAIL;
            }
        }
    }

#if defined(CONFIG_AMBALINK_MEM_SIZE)
    if(retcode == ERRCODE_NONE) {
        retcode = krn_get_cv_rtos_range(&start, &end);
        if(retcode == ERRCODE_NONE) {
            if((end - start) != 0UL) {
                mem_all->rtos_region.base = krn_p2c(start);
                value = (end - start);
                mem_all->rtos_region.size = value;

                virt = krn_ioremap(start,(end - start));
                if(virt != NULL) {
                    krn_mmap_add_rtos(virt, start, (end - start), enable_rtos_cache);
                } else {
                    krn_printU5("[ERROR] krn_os_kernel_init() : krn_ioremap fail", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_RTOS_MAP_FAIL;
                }
            } else {
                mem_all->rtos_region.base = 0UL;
                mem_all->rtos_region.size = 0UL;
                krn_mmap_add_rtos(NULL, 0U, 0U, enable_rtos_cache);
            }
        }
    }
#endif
    krn_mmap_add_dsp_data(NULL, 0U, 0U, 1);

    if(retcode == ERRCODE_NONE) {
        os_kernel_init = 1U;
    }

    return retcode;
}

void krn_os_kernel_exit(void)
{
    os_kernel_init = 0U;
}

