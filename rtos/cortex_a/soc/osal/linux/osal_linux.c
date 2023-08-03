/**
 *  @file osal_linux_kernel.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Ambarella KAL (Kernel Abstraction Layer) APIs for ThreadX
 *
 */

#include "osal_linux.h"

/******************************************************************************/
/*                                  AmbaKAL.h                                                                                       */
/******************************************************************************/
UINT32 OSAL_TaskCreate(AMBA_KAL_TASK_t **ppTask, char *pTaskName, UINT32 TaskPriority,
                       AMBA_KAL_TASK_ENTRY_f EntryFunction, void* EntryArg,
                       void *pStackBase, UINT32 StackByteSize, UINT32 AutoStart)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (EntryFunction == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        *ppTask = kthread_run(EntryFunction, EntryArg, pTaskName);
        if (*ppTask == NULL) {
            RetVal = KAL_ERR_CALLER;
        }
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_TaskCreate);

UINT32 OSAL_TaskSetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 CpuBindList)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}
EXPORT_SYMBOL(OSAL_TaskSetSmpAffinity);

UINT32 OSAL_TaskDelete(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    INT32 ret;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        ret = kthread_stop(pTask);
        if (ret != 0) {
            RetVal = KAL_ERR_CALLER;
        }
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_TaskDelete);

UINT32 OSAL_TaskResume(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}
EXPORT_SYMBOL(OSAL_TaskResume);

UINT32 OSAL_TaskSleep(UINT32 NumTicks)
{
    UINT32 RetVal = KAL_ERR_NONE;

    usleep_range(NumTicks*1000,(NumTicks + 1)*1000);
    return RetVal;
}
EXPORT_SYMBOL(OSAL_TaskSleep);

UINT32 OSAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        atomic_set(&pEventFlag->flag, 0);
        init_waitqueue_head(&pEventFlag->wait_queue);
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_EventFlagCreate);

UINT32 OSAL_EventFlagDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_EventFlagDelete);

UINT32 OSAL_EventFlagGet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                         UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut)
{
    UINT32 RetVal = KAL_ERR_NONE;
    INT32 ret;

    if ((pEventFlag == NULL) || (pActualFlags == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        if(AnyOrAll == AMBA_KAL_FLAGS_ALL) {
            ret = wait_event_interruptible_timeout(pEventFlag->wait_queue, ((atomic_read(&pEventFlag->flag) & ReqFlags) == ReqFlags),msecs_to_jiffies(TimeOut));
        } else {
            ret = wait_event_interruptible_timeout(pEventFlag->wait_queue, ((atomic_read(&pEventFlag->flag) & ReqFlags) != 0),msecs_to_jiffies(TimeOut));
        }
        if(ret == 0) {
            RetVal = KAL_ERR_TIMEOUT;
        } else if ( (ret < 0) && (ret != -ERESTARTSYS)) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_NONE;
        }
        *pActualFlags = (atomic_read(&pEventFlag->flag) & ReqFlags);
        if(AutoClear == AMBA_KAL_FLAGS_CLEAR_AUTO) {
            atomic_and(~(*pActualFlags), &pEventFlag->flag);
        }
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_EventFlagGet);

UINT32 OSAL_EventFlagSet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        atomic_or(SetFlags, &pEventFlag->flag);
        wake_up_interruptible(&pEventFlag->wait_queue);
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_EventFlagSet);

UINT32 OSAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        atomic_and(~ClearFlags, &pEventFlag->flag);
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_EventFlagClear);

UINT32 OSAL_MutexCreate(AMBA_KAL_MUTEX_t *pMutex, char * pMutexName)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        mutex_init(pMutex);
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_MutexCreate);

UINT32 OSAL_MutexDelete(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_MutexDelete);

UINT32 OSAL_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    UINT32 RetVal = KAL_ERR_NONE;
    INT32 ret;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        if (TimeOut == AMBA_KAL_WAIT_FOREVER) {
            mutex_lock(pMutex);
        } else {
            ktime_t timeout = ktime_add_ms(ktime_get(), TimeOut);
            do {
                ret = mutex_trylock(pMutex);
                if (ret != 0) {
                    // Got the lock!
                    break;
                }
                usleep_range(1000,2000);
            } while (ktime_compare(timeout, ktime_get()) == 1);
            if(ret == 0) {
                // Did not get the lock
                RetVal = KAL_ERR_CALLER;
            }
        }
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_MutexTake);

UINT32 OSAL_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        mutex_unlock(pMutex);
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_MutexGive);

UINT32 OSAL_SemaphoreCreate(AMBA_KAL_SEMAPHORE_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        sema_init(pSemaphore, InitCount);
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_SemaphoreCreate);

UINT32 OSAL_SemaphoreDelete(AMBA_KAL_SEMAPHORE_t *pSemaphore)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_SemaphoreDelete);

UINT32 OSAL_SemaphoreTake(AMBA_KAL_SEMAPHORE_t *pSemaphore, UINT32 TimeOut)
{
    UINT32 RetVal = KAL_ERR_NONE;
    INT32 ret;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        if (TimeOut == AMBA_KAL_WAIT_FOREVER) {
            down(pSemaphore);
        } else {
            ret = down_timeout(pSemaphore, msecs_to_jiffies(TimeOut));
            if (ret != 0) {
                RetVal = KAL_ERR_CALLER;
            }
        }
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_SemaphoreTake);

UINT32 OSAL_SemaphoreGive(AMBA_KAL_SEMAPHORE_t *pSempahore)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pSempahore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        up(pSempahore);
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_SemaphoreGive);
/******************************************************************************/
/*                                  AmbaPrint.h                                                                                       */
/******************************************************************************/
static UINT32 module_allow_list[ALLOW_LIST_ENTRY_NUMBER];
static DEFINE_MUTEX(Mutex_Print);

void OSAL_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (Arg5) {
        printk(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
    } else if (Arg4) {
        printk(pFmt, Arg1, Arg2, Arg3, Arg4);
    } else if (Arg3) {
        printk(pFmt, Arg1, Arg2, Arg3);
    } else if (Arg2) {
        printk(pFmt, Arg1, Arg2);
    } else if (Arg1) {
        printk(pFmt, Arg1);
    } else {
        printk(pFmt);
    }
}
EXPORT_SYMBOL(OSAL_PrintUInt5);

void OSAL_PrintInt5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    if (Arg5) {
        printk(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
    } else if (Arg4) {
        printk(pFmt, Arg1, Arg2, Arg3, Arg4);
    } else if (Arg3) {
        printk(pFmt, Arg1, Arg2, Arg3);
    } else if (Arg2) {
        printk(pFmt, Arg1, Arg2);
    } else if (Arg1) {
        printk(pFmt, Arg1);
    } else {
        printk(pFmt);
    }
}
EXPORT_SYMBOL(OSAL_PrintInt5);

void OSAL_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    if (pArg5 != NULL ) {
        printk(pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
    } else if (pArg4 != NULL) {
        printk(pFmt, pArg1, pArg2, pArg3, pArg4);
    } else if (pArg3 != NULL) {
        printk(pFmt, pArg1, pArg2, pArg3);
    } else if (pArg2 != NULL) {
        printk(pFmt, pArg1, pArg2);
    } else if (pArg1 != NULL) {
        printk(pFmt, pArg1);
    } else {
        printk(pFmt);
    }
}
EXPORT_SYMBOL(OSAL_PrintStr5);

UINT32 OSAL_ModuleSetAllowList(UINT16 ModuleID, UINT8 Enable)
{
    UINT32 uret = PRINT_ERR_SUCCESS;
    if (Enable > 1U) {
        uret = PRINT_ERR_INVALID_PARAMETER;
    } else {
        mutex_lock(&Mutex_Print);
        UINT16 idx_main = ModuleID / 32U;
        UINT16 shift    = ModuleID % 32U;
        UINT32 value_1  = 1U;
        UINT32 value    = value_1 << shift;

        if (Enable == PRINT_COND_NO) {
            module_allow_list[idx_main] &= ~value;
        } else {
            module_allow_list[idx_main] |= value;
        }
        mutex_unlock(&Mutex_Print);
    }
    return uret;
}
EXPORT_SYMBOL(OSAL_ModuleSetAllowList);

static UINT8 is_in_allow_list(UINT16 ModuleID)
{
    UINT16 idx_main = ModuleID / 32U;
    UINT16 shift    = ModuleID % 32U;
    UINT32 value    = (module_allow_list[idx_main] >> shift) & 0x01U;
    UINT8  ret = PRINT_COND_NO;
    if (value != 0U) {
        ret = PRINT_COND_YES;
    }
    return ret;
}

static void osal_printk(const char *fmt, ...)
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

void OSAL_ModulePrintStr5(UINT16 ModuleID, const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    UINT8 is_allowed = is_in_allow_list(ModuleID);
    MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_LOG_BUFFER;

    if (is_allowed == PRINT_COND_YES) {
        message_destination = MESSAGE_TO_BOTH;
    }

    if (message_destination == MESSAGE_TO_BOTH) {
        OSAL_PrintStr5(pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
    } else {

        if (pArg5 != NULL ) {
            osal_printk(pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
        } else if (pArg4 != NULL) {
            osal_printk(pFmt, pArg1, pArg2, pArg3, pArg4);
        } else if (pArg3 != NULL) {
            osal_printk(pFmt, pArg1, pArg2, pArg3);
        } else if (pArg2 != NULL) {
            osal_printk(pFmt, pArg1, pArg2);
        } else if (pArg1 != NULL) {
            osal_printk(pFmt, pArg1);
        } else {
            osal_printk(pFmt);
        }
    }
}
EXPORT_SYMBOL(OSAL_ModulePrintStr5);

void OSAL_ModulePrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    UINT8 is_allowed = is_in_allow_list(ModuleID);
    MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_LOG_BUFFER;

    if (is_allowed == PRINT_COND_YES) {
        message_destination = MESSAGE_TO_BOTH;
    }

    if (message_destination == MESSAGE_TO_BOTH) {
        OSAL_PrintUInt5(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
    } else {
        if (Arg5) {
            osal_printk(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
        } else if (Arg4) {
            osal_printk(pFmt, Arg1, Arg2, Arg3, Arg4);
        } else if (Arg3) {
            osal_printk(pFmt, Arg1, Arg2, Arg3);
        } else if (Arg2) {
            osal_printk(pFmt, Arg1, Arg2);
        } else if (Arg1) {
            osal_printk(pFmt, Arg1);
        } else {
            osal_printk(pFmt);
        }
    }
}
EXPORT_SYMBOL(OSAL_ModulePrintUInt5);

void OSAL_ModulePrintInt5(UINT16 ModuleID, const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    UINT8 is_allowed = is_in_allow_list(ModuleID);
    MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_LOG_BUFFER;

    if (is_allowed == PRINT_COND_YES) {
        message_destination = MESSAGE_TO_BOTH;
    }

    if (message_destination == MESSAGE_TO_BOTH) {
        OSAL_PrintInt5(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
    } else {
        if (Arg5) {
            osal_printk(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
        } else if (Arg4) {
            osal_printk(pFmt, Arg1, Arg2, Arg3, Arg4);
        } else if (Arg3) {
            osal_printk(pFmt, Arg1, Arg2, Arg3);
        } else if (Arg2) {
            osal_printk(pFmt, Arg1, Arg2);
        } else if (Arg1) {
            osal_printk(pFmt, Arg1);
        } else {
            osal_printk(pFmt);
        }
    }
}
EXPORT_SYMBOL(OSAL_ModulePrintInt5);
/******************************************************************************/
/*                                  AmbaWrap.h                                                                                      */
/******************************************************************************/
UINT32 OSAL_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    UINT32 err = 0;

    if ((pDst == NULL) ||
        (pSrc == NULL) ||
        (num == 0U)) {
        err = LIBWRAP_ERR_INVAL;
    } else {
        memcpy(pDst, pSrc, num);
    }

    return err;
}
EXPORT_SYMBOL(OSAL_memcpy);

UINT32 OSAL_memset(void *ptr, INT32 v, SIZE_t n)
{
    UINT32 err = 0;

    if (ptr == NULL) {
        err = LIBWRAP_ERR_INVAL;
    } else {
        memset(ptr, v, n);
    }

    return err;
}
EXPORT_SYMBOL(OSAL_memset);

UINT32 OSAL_memcmp(const void *p1, const void *p2, SIZE_t n, void *pV)
{
    UINT32 err = 0;
    INT32 v;

    if ((p1 == NULL) ||
        (p2 == NULL) ||
        (pV == NULL) ||
        (n == 0U)) {
        err = LIBWRAP_ERR_INVAL;
    } else {
        v = memcmp(p1, p2, n);
        err = OSAL_memcpy(pV, &v, sizeof(v));
    }

    return err;
}
EXPORT_SYMBOL(OSAL_memcmp);
/******************************************************************************/
/*                                  AmbaMisraFix.h                                                                                   */
/******************************************************************************/
void OSAL_TypeCast(void * pNewType, const void * pOldType)
{
    memcpy(pNewType, pOldType, sizeof(void *));
}
EXPORT_SYMBOL(OSAL_TypeCast);

/******************************************************************************/
/*                                  AmbaCache.h                                                                                   */
/******************************************************************************/
UINT32 OSAL_Cache_DataClean(ULONG VirtAddr, ULONG Size)
{
    UINT32 RetVal = CACHE_ERR_NONE;

    void *ptr;

    OSAL_TypeCast(&ptr,&VirtAddr);
    ambcache_clean_range(ptr, Size);
    return RetVal;
}
EXPORT_SYMBOL(OSAL_Cache_DataClean);

UINT32 OSAL_Cache_DataInvalidate(ULONG VirtAddr, ULONG Size)
{
    UINT32 RetVal = CACHE_ERR_NONE;
    void *ptr;

    OSAL_TypeCast(&ptr,&VirtAddr);
    ambcache_inv_range(ptr, Size);
    return RetVal;
}
EXPORT_SYMBOL(OSAL_Cache_DataInvalidate);
/******************************************************************************/
/*                                  AmbaINT.h                                                                                   */
/******************************************************************************/
UINT32 OSAL_INT_Config(UINT32 Irq, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg)
{
    UINT32 RetVal = INT_ERR_NONE;
    UINT32 i,bset = 0U;
    INT32 ret = 0;
    cpumask_t cpumask;

    if (pIntConfig == NULL) {
        RetVal = INT_ERR_ARG;
    } else {
        if(pIntConfig->TriggerType == AMBA_INT_RISING_EDGE_TRIGGER) {
            ret = request_irq(Irq, IntFunc, IRQF_TRIGGER_RISING, NULL, NULL);
        } else {
            ret = request_irq(Irq, IntFunc, IRQF_TRIGGER_HIGH, NULL, NULL);
        }
        for(i = 0U; i < 8U; i++) {
            if(pIntConfig->CpuTargets & (UINT32)(1U << i)) {
                cpumask_set_cpu(i, &cpumask);
                bset = 1U;
            }
        }
        if(bset == 1U) {
            ret = irq_set_affinity_hint(Irq,&cpumask);
        }

        if(ret != 0) {
            RetVal = INT_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}
EXPORT_SYMBOL(OSAL_INT_Config);

UINT32 OSAL_INT_Enable(UINT32 Irq)
{
    UINT32 RetVal = INT_ERR_NONE;

    return RetVal;
}
EXPORT_SYMBOL(OSAL_INT_Enable);

UINT32 OSAL_INT_Disable(UINT32 Irq)
{
    UINT32 RetVal = INT_ERR_NONE;

    free_irq(Irq,NULL);
    return RetVal;
}
EXPORT_SYMBOL(OSAL_INT_Disable);

UINT32 OSAL_GetClkFreq(UINT32 ClkID, UINT32 *pFreq)
{
    UINT32 retcode = 0;
    struct clk *gclk = NULL;

    if(ClkID == AMBA_SYS_CLK_AUD_0) {
        gclk = clk_get_sys(NULL, "gclk_audio");
        if(gclk != NULL) {
            *pFreq = (UINT32) clk_get_rate(gclk);
        } else {
            retcode = 0xFFFFFFFEU;
        }
    } else if(ClkID == AMBA_SYS_CLK_VISION) {
        gclk = clk_get_sys(NULL, "gclk_vision");
        if(gclk != NULL) {
            *pFreq = (UINT32) clk_get_rate(gclk);
        } else {
            retcode = 0xFFFFFFFEU;
        }
    } else {
        retcode = 0xFFFFFFFFU;
    }

    return retcode;
}
EXPORT_SYMBOL(OSAL_GetClkFreq);
