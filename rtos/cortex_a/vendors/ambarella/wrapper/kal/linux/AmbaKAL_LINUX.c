/**
 *  @file AmbaKAL_LINUX.c
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
 *  @details Ambarella KAL (Kernel Abstraction Layer) APIs for Linux
 *
 */
#define _GNU_SOURCE
#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"

#include "AmbaKAL.h"

typedef struct {
    UINT32 ExpireFuncArg;
    UINT32 StartInterval;
    UINT32 PeriodicInterval;
    UINT32 AutoStart;
    AMBA_KAL_MUTEX_t *pMutex;
    AMBA_KAL_TIMER_EXPIRY_f ExpireFunc;
} AMBA_TIMER_SETTING_s;

static inline uint64_t timespec2nsec(struct timespec* ts)
{
    return ts->tv_sec * (uint64_t)1000000000L + ts->tv_nsec;
}

static inline void nsec2timespec(struct timespec* ts, uint64_t nsec)
{
    if(nsec == 0L) {
        ts->tv_sec = 0;
        ts->tv_nsec = 0;
    } else {
        ts->tv_sec = nsec / 1000000000L;
        ts->tv_nsec = (nsec % 1000000000L);
    }
}

UINT32 AmbaKAL_HookStkErrHandler(AMBA_KAL_TASK_STACK_ERROR_HANDLER_f StackErrorHandler)
{
    UINT32 RetVal = KAL_ERR_NONE;

    (void)StackErrorHandler;

    return RetVal;
}

UINT32 AmbaKAL_TaskHookProfileFunc(AMBA_KAL_TASK_t *pTask, AMBA_KAL_TASK_PROFILE_FUNC_f ProfileFunc)
{
    UINT32 RetVal = KAL_ERR_NONE;

    (void)pTask;
    (void)ProfileFunc;

    return RetVal;
}

/**
 *  AmbaKAL_TaskCreate - Create a software task
 *  @param[in] pTask Pointer to a software task control block
 *  @param[in] pTaskName Pointer to the Software Task name
 *  @param[in] TaskPriority Priority of the Software Task. 0 = highest priority
 *  @param[in] EntryFunction Entry function of the Software Task
 *  @param[in] EntryArg The argument attached to entry function
 *  @param[in] pStackBase Pointer to the stack area
 *  @param[in] StackByteSize Stack size in bytes
 *  @param[in] AutoStart The initial thread state after creation
 *  @return error code
 */
UINT32 AmbaKAL_TaskCreate(AMBA_KAL_TASK_t *pTask, char *pTaskName, UINT32 TaskPriority,
                          AMBA_KAL_TASK_ENTRY_f EntryFunction, const void * EntryArg,
                          void *pStackBase, UINT32 StackByteSize, UINT32 AutoStart)
{
    struct sched_param param;
    //preempt & starve low pri tasks, use round-robin for same pri task
    const int policy = SCHED_RR;
    int max_pri_RR = sched_get_priority_max(policy);
    int min_pri_RR = sched_get_priority_min(policy);
    pthread_attr_t attr;
    int Ret = 0;

    pthread_attr_init(&attr);
    if ((pStackBase) && (StackByteSize)) {
        pthread_attr_setstack(&attr, pStackBase, StackByteSize);
    }
#if 0 /*QNX only*/
    if (AutoStart) {
        pthread_attr_setsuspendstate_np(&attr, PTHREAD_CREATE_NOT_SUSPENDED);
    } else {
        pthread_attr_setsuspendstate_np(&attr, PTHREAD_CREATE_SUSPENDED);
    }
#else
    (void)AutoStart;
#endif
    if (pTask == NULL) {
        Ret = KAL_ERR_ARG;
    } else {
        Ret = pthread_create(pTask, &attr, EntryFunction, (void *)EntryArg);
        if (pTaskName != NULL) {
            pthread_setname_np(*pTask, (const char *)pTaskName);
        }
        /*
         * NOTE:
         * The two OS use different range of priority, like these:
         * ThreadX uses:  1 (highest) <-----> 127 (lowest)
         * Linux RR uses: 1 (lowest)  <-----> 99 (highest)
         * (KAL_TASK_PRIORITY_LOWEST = 127 and KAL_TASK_PRIORITY_HIGHEST = 1)
         * Hence, We need to map them for consistency.
         *
         * Because the range of Linux is smaller than the range of ThreadX,
         * the last lower priority which are more than 99 priority
         * will be mapped to lowest priority in Linux RR which is 1.
         *
         * Appendix:
         * There are two categories for Linux scheduling policy:
         * (1) 'Normal' scheduling policies: SCHED_OTHER, SCHED_BATCH and SCHED_IDLE
         * (2) 'Real-time' scheduling policies: SCHED_FIFO and SCHED_RR
         *
         * for normal processes: PR = 20 + NI (NI is nice and ranges from -20 to 19)
         * for real time processes: PR = - 1 - real_time_priority (real_time_priority ranges from 1 to 99)
         *
         */
        if ((INT32)TaskPriority > max_pri_RR) {
            /* Revert priority value to the lowest for Linux RR, ex: 100->1, 101->1 */
            param.sched_priority = min_pri_RR;
        } else if ((INT32)TaskPriority < min_pri_RR) {
            /* Revert priority value to the highest for Linux RR,
             * ex: 0->99 (this may not exist in current setting) */
            param.sched_priority = max_pri_RR;
        } else {
            /* Revert priority value, ex: 1->99, 2->98,..., 98->2, 99->1 */
            param.sched_priority = (max_pri_RR + min_pri_RR) - (INT32)TaskPriority;
        }

        Ret = pthread_setschedparam(*pTask, policy, &param);
    }

    return Ret;
}

/**
 *  AmbaKAL_TaskDelete - Delete the software task
 *  @param[in] pTask Pointer to the software task control block
 *  @return error code
 */
UINT32 AmbaKAL_TaskDelete(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 Ret;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        /* not wait, not care */
        Ret = pthread_detach(*pTask);
        if (Ret != OK) {
        } else {
            /* do nothing */
        }
    }
    return RetVal;
}

UINT32 AmbaKAL_TaskSuspend(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 Ret;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
#if 0   /*QNX only*/
        Ret = ThreadCtlExt_r(getpid(), *pTask, _NTO_TCTL_ONE_THREAD_HOLD, 0);
#else
        /* wait for pthread_exit */
        //Ret = pthread_join(*pTask, NULL);
        /* not wait, not care */
        Ret = pthread_detach(*pTask);
#endif
        if (Ret != OK) {
        } else {
            /* do nothing */
        }
    }
    return RetVal;
}

/**
 *  AmbaKAL_TaskResume - This function resumes or prepares for execution a thread that was previously suspended by either AmbaKAL_TaskSuspend or AmbaKAL_TaskCreate function.
 *  @param[in] pTask Pointer to the software task control block
 *  @return error code
 */
UINT32 AmbaKAL_TaskResume(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 Ret;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
#if 0   /*QNX only*/
        Ret = ThreadCtlExt_r(getpid(), *pTask, _NTO_TCTL_ONE_THREAD_CONT, 0);
#else
        Ret = OK;
#endif
        if (Ret != OK) {
            printf("AmbaKAL_TaskResume failed, Ret = 0x%08x\n", Ret);
        } else {
            /* do nothing */
        }
    }
    return RetVal;
}

UINT32 AmbaKAL_TaskAbortCondSuspend(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;

    (void)pTask;

    return RetVal;
}

/**
 *  AmbaKAL_TaskTerminate - Terminate the software task
 *  @param[in] pTask Pointer to the software task control block
 *  @return error code
 */
UINT32 AmbaKAL_TaskTerminate(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 Ret;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        Ret = pthread_cancel(*pTask);
        if (Ret != OK) {
            //it is normal when return 3 "No such process"
            //printf("AmbaKAL_TaskTerminate failed, Ret = 0x%08x\n", Ret);
        } else {
            /* do nothing */
        }
    }
    return RetVal;
}

UINT32 AmbaKAL_TaskReset(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;

    (void)pTask;

    return RetVal;
}

UINT32 AmbaKAL_TaskQuery(AMBA_KAL_TASK_t *pTask, AMBA_KAL_TASK_INFO_s *pTaskInfo)
{
    UINT32 RetVal = KAL_ERR_NONE;
    char Name[64];
    struct sched_param TaskParam = {0};
    int i = 0U, policy;

    if ((pTask == NULL) || (pTaskInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        pthread_getschedparam(*pTask, &policy, &TaskParam);
        pTaskInfo->TaskPriority = TaskParam.sched_priority; /* priority */
        pthread_getname_np(*pTask, &Name[0], 64U);
        if (pTaskInfo->pTaskName != NULL) {
            for (i = 0; i < 64; i++) {
                pTaskInfo->pTaskName[i] = Name[i];
                if (Name[i] == '\0') {
                    break;
                }
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskIdentify - This function is used to get the control block address of the executing thread.
 *  @param[out] pTaskPtr A pointer to the pointer of a thread control block.
 *  @return error code
 */
UINT32 AmbaKAL_TaskIdentify(AMBA_KAL_TASK_t ** pTaskPtr)
{
    UINT32 RetVal;

    /* WARN: pthread_self() returns int, not pointer */
    if ((pTaskPtr == NULL) || *pTaskPtr == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        **pTaskPtr = pthread_self();
        RetVal = KAL_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskSleep - This function causes the calling thread to suspend for the specified number of system ticks.
 *                      If 0 is specified, the function returns immediately
 *  @param[in] NumTicks The number of timer ticks(ms) to suspend the task
 *  @return error code
 */
UINT32 AmbaKAL_TaskSleep(UINT32 NumTicks)
{
    UINT32 sec = NumTicks / 1000;
    UINT32 msec = NumTicks % 1000;
    struct timespec Duration;

    Duration.tv_sec = sec;
    Duration.tv_nsec = msec * 1000 * 1000;
    thrd_sleep(&Duration, NULL);

    return KAL_ERR_NONE;
}

UINT32 AmbaKAL_TaskYield(void)
{
    (void)sched_yield();

    return KAL_ERR_NONE;
}

//
///**
// *  AmbaKAL_GetCpuContext - Check if current system state is in ISR or not
// *  @param[out] pContextType Context type. 0 = Interrupt context; otherwise = Non-interrupt context
// *  @return error code
// */
//UINT32 AmbaKAL_GetCpuContext(UINT32 * pContextType)
//{
//    UINT32 RetVal = KAL_ERR_NONE;
//    ULONG TxSystemState;
//
//    if (pContextType == NULL) {
//        RetVal = KAL_ERR_ARG;
//    } else {
//        /* Determine the caller's context. Mutex locks are only available from initialization and threads.  */
//        TxSystemState = TX_THREAD_GET_SYSTEM_STATE();
//        if ((TxSystemState == 0U) || (TxSystemState >= TX_INITIALIZE_IN_PROGRESS)) {
//            *pContextType = 1U;
//        } else {
//            *pContextType = 0U;
//        }
//    }
//
//    return RetVal;
//}
//
/**
 *  AmbaKAL_GetSysTickCount - Get current system tick counter value
 *  @param[out] pSysTickCount The number of elapsed system ticks.
 *  @return error code
 */
UINT32 AmbaKAL_GetSysTickCount(UINT32 * pSysTickCount)
{
    UINT32 RetVal = 0;
    struct timespec tp;

    if (pSysTickCount == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        RetVal = clock_gettime(CLOCK_MONOTONIC, &tp);
        if (RetVal == 0) {
            *pSysTickCount = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
        } else {
            *pSysTickCount = 0;
        }
    }

    return RetVal;
}

UINT32 AmbaKAL_GetSmpCpuID(UINT32 * pCpuID)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pCpuID == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        *pCpuID = sched_getcpu();
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskSetSmpAffinity - This function enables the binding of a thread to a central processing unit (CPU) or a range of CPUs, so that the thread will execute only on the designated CPU or CPUs rather than any CPU.
 *  @param[in] pTask Pointer to the software task control block
 *  @param[in] CpuBindList CPU Binding List
 *  @return error code
 */
UINT32 AmbaKAL_TaskSetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 CpuBindList)
{
    UINT32 RetVal = KAL_ERR_NONE, CoreMask;
    INT32 ret;
    INT32 i;
    cpu_set_t cpuset;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        CoreMask = (CpuBindList & SMP_CORE_MASK);
        if (CoreMask == 0U) {
            CoreMask = 0x01U;
        }

        CPU_ZERO(&cpuset);
        for (i = 0; i < (INT32)CONFIG_SMP_CORE_NUM; i++) {
            if (CoreMask & (1<<i)) {
                CPU_SET(i, &cpuset);
            }
        }
        ret = pthread_setaffinity_np(*pTask, sizeof(cpu_set_t), &cpuset);
        if (ret != 0) {
            RetVal = KAL_ERR_CALLER;
        }
    }

    return RetVal;
}
//
UINT32 AmbaKAL_TaskGetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 * pCpuBindList)
{
    UINT32 RetVal = KAL_ERR_NONE;
    INT32 ret;
    INT32 i;
    cpu_set_t cpuset;

    if ((pTask == NULL) || (pCpuBindList == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        CPU_ZERO(&cpuset);
        ret = pthread_getaffinity_np(*pTask, sizeof(cpu_set_t), &cpuset);
        if (ret != 0) {
            RetVal = KAL_ERR_CALLER;
        } else {
            for (i = 0; i < 4; i++) {
                if (CPU_ISSET(i, &cpuset)) {
                    *pCpuBindList |= (1<<i);
                }
            }
        }
    }

    return RetVal;
}

///**
// *  AmbaKAL_TimerSetSmpAffinity - This function enables the binding of a timer to a central processing unit (CPU) or a range of CPUs, so that the timer will execute only on the designated CPU or CPUs rather than any CPU.
// *  @param[in] pTimer Pointer to the timer control block
// *  @param[in] CpuBindList CPU Binding List
// *  @return error code
// */
//UINT32 AmbaKAL_TimerSetSmpAffinity(AMBA_KAL_TIMER_t *pTimer, UINT32 CpuBindList)
//{
//    UINT32 RetVal = KAL_ERR_NONE;
//    UINT32 TxRetVal;
//
//    if (pTimer == NULL) {
//        RetVal = KAL_ERR_ARG;
//    } else {
//        if (CpuBindList < (UINT32)TX_THREAD_SMP_MAX_CORES) {
//            TxRetVal = tx_timer_smp_core_exclude(pTimer, (ULONG)(~(CpuBindList)) & (UINT32)TX_THREAD_SMP_CORE_MASK);
//        } else {
//            /* if no core is assigned, default assign to core0 */
//            TxRetVal = tx_timer_smp_core_exclude(pTimer, (ULONG)(~0x1U) & (UINT32)TX_THREAD_SMP_CORE_MASK);
//        }
//
//        if (TxRetVal != TX_SUCCESS) {
//            RetVal = KAL_ERR_ARG;
//        }
//    }
//
//    return RetVal;
//}
//
///**
// *  AmbaKAL_TimerGetSmpAffinity - This function is used to retrieve the CPU affinity of a thread.
// *  @param[in] pTask Pointer to the software task control block
// *  @param[out] pCpuBindList Pointer to store CPU binding list
// *  @return error code
// */
//UINT32 AmbaKAL_TimerGetSmpAffinity(AMBA_KAL_TIMER_t *pTimer, UINT32 * pCpuBindList)
//{
//    UINT32 RetVal = KAL_ERR_NONE;
//    UINT32 TxRetVal;
//    ULONG CpuMap;
//
//    if ((pTimer == NULL) || (pCpuBindList == NULL)) {
//        RetVal = KAL_ERR_ARG;
//    } else {
//        TxRetVal = tx_timer_smp_core_exclude_get(pTimer, &CpuMap);
//        if (TxRetVal != TX_SUCCESS) {
//            RetVal = KAL_ERR_ARG;
//        } else {
//            *pCpuBindList = (UINT32)((~CpuMap) & (UINT32)TX_THREAD_SMP_CORE_MASK);
//        }
//    }
//
//    return RetVal;
//}

static void *KAL_TimerEntry(void* argv)
{
    AMBA_TIMER_SETTING_s TimerSetting = {0U};
    AMBA_TIMER_SETTING_s *pTimerSetting = (AMBA_TIMER_SETTING_s *)argv;
    UINT32 uSec = 0U;

    if (pTimerSetting != NULL) {
        TimerSetting.ExpireFunc = pTimerSetting->ExpireFunc;
        TimerSetting.ExpireFuncArg = pTimerSetting->ExpireFuncArg;
        TimerSetting.PeriodicInterval = pTimerSetting->PeriodicInterval;
        TimerSetting.StartInterval = pTimerSetting->StartInterval;
        TimerSetting.AutoStart = pTimerSetting->AutoStart;
        TimerSetting.pMutex = pTimerSetting->pMutex;

        uSec = TimerSetting.StartInterval * 1000;
        usleep(uSec);
        if (pthread_mutex_trylock(TimerSetting.pMutex) == 0) {
            TimerSetting.ExpireFunc(TimerSetting.ExpireFuncArg);
            pthread_mutex_unlock(TimerSetting.pMutex);
        }

        while (1) {
            if (0x0U != TimerSetting.PeriodicInterval) {
                uSec = TimerSetting.PeriodicInterval * 1000;
                usleep(uSec);
                if (pthread_mutex_trylock(TimerSetting.pMutex) == 0) {
                    TimerSetting.ExpireFunc(TimerSetting.ExpireFuncArg);
                    pthread_mutex_unlock(TimerSetting.pMutex);
                }
            } else {
                /*do not wait for AmbaKAL_TimerDelete*/
                pthread_exit(NULL);
            }
        }
    }
    return NULL;
}

/**
 *  AmbaKAL_TimerCreate - Create an application timer
 *  @param[in] pTimer Pointer to the timer control block
 *  @param[in] pTimerName Pointer to the Timer name
 *  @param[in] ExpireFunc Application function to call when the timer expires
 *  @param[in] ExpireFuncArg Argument attached to the timer expiration functio
 *  @param[in] StartInterval Intial ticks for the timer expiration
 *  @param[in] PeriodicInterval Reload ticks for the timer expiration after the 1st. 0 = one-shot
 *  @param[in] AutoStart Automatically start up or not after creation
 *  @return error code
 */
UINT32 AmbaKAL_TimerCreate(AMBA_KAL_TIMER_t *pTimer, char * pTimerName, AMBA_KAL_TIMER_EXPIRY_f ExpireFunc,
                           UINT32 ExpireFuncArg, UINT32 StartInterval, UINT32 PeriodicInterval, UINT32 AutoStart)
{
    AMBA_TIMER_SETTING_s TimerSetting;
    int Ret = KAL_ERR_NONE;

    if (pTimer == NULL) {
        Ret = KAL_ERR_ARG;
    } else {
        /* create mutex */
        pthread_mutex_init(&pTimer->mutex, NULL);
        TimerSetting.pMutex = &pTimer->mutex;
        TimerSetting.ExpireFunc = ExpireFunc;
        TimerSetting.ExpireFuncArg = ExpireFuncArg;
        TimerSetting.PeriodicInterval = PeriodicInterval;
        TimerSetting.StartInterval = StartInterval;
        TimerSetting.AutoStart = AutoStart;

        /* do not run ExpireFunc */
        if (AutoStart == 0U) {
            pthread_mutex_lock(&pTimer->mutex);
        }

        /* create pthread */
        Ret = pthread_create(&pTimer->th, NULL, KAL_TimerEntry, &TimerSetting);
        /* wait 1ms for KAL_TimerEntry to copy TimerSetting */
        usleep(1000);

        if (Ret == OK) {
            if (pTimerName != NULL) {
                pthread_setname_np(pTimer->th, (const char *)pTimerName);
            }
        } else {
            printf("AmbaKAL_TimerCreate failed, error = %d\n", Ret);
            Ret = KAL_ERR_POSIX;
        }
    }

    return Ret;
}
/**
 *  AmbaKAL_TimerDelete - Delete an application timer
 *  @param[in] pTimer Pointer to the timer control block
 *  @return error code
 */
UINT32 AmbaKAL_TimerDelete(AMBA_KAL_TIMER_t *pTimer)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        /* ignore err, one-shot timer already exit */
        (void) pthread_detach(pTimer->th);
    }

    return RetVal;
}

/**
 *  AmbaKAL_TimerStart - This function activates the specified timer
 *  @param[in] pTimer Pointer to the timer control block
 *  @return error code
 */
UINT32 AmbaKAL_TimerStart(AMBA_KAL_TIMER_t *pTimer)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        /* run ExpireFunc */
        (void)pthread_mutex_unlock(&pTimer->mutex);
    }
    return RetVal;
}


/**
 *  AmbaKAL_TimerStop - Stop the application timer
 *  @param[in] pTimer Pointer to the timer control block
 *  @return error code
 */
UINT32 AmbaKAL_TimerStop(AMBA_KAL_TIMER_t *pTimer)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        /* do not run ExpireFunc */
        RetVal = pthread_mutex_lock(&pTimer->mutex);
    }
    return RetVal;
}

UINT32 AmbaKAL_TimerChange(AMBA_KAL_TIMER_t *pTimer, UINT32 StartInterval, UINT32 PeriodicInterval, UINT32 AutoStart)
{
    AmbaMisra_TouchUnused(&pTimer);
    AmbaMisra_TouchUnused(&StartInterval);
    AmbaMisra_TouchUnused(&PeriodicInterval);
    AmbaMisra_TouchUnused(&AutoStart);

    return KAL_ERR_NONE;
}

UINT32 AmbaKAL_TimerQuery(AMBA_KAL_TIMER_t *pTimer, AMBA_KAL_TIMER_INFO_s *pTimerInfo)
{
    AmbaMisra_TouchUnused(&pTimer);
    AmbaMisra_TouchUnused(&pTimerInfo);

    return KAL_ERR_NONE;
}

///**
// *  AmbaKAL_CriticalSectionEnter - This function is used to enter critical section
// *  @param[in] pCpuStatus The pointer for storing processor status register
// *  @return error code
// */
//UINT32 AmbaKAL_CriticalSectionEnter(UINT32 *pCpuStatus)
//{
//    UINT32 RetVal = KAL_ERR_NONE;
//
//    if (pCpuStatus == NULL) {
//        RetVal = KAL_ERR_ARG;
//    } else {
//        *pCpuStatus = _tx_thread_smp_protect();
//    }
//
//    return RetVal;
//}
//
///**
// *  AmbaKAL_CriticalSectionExit - This function is used to exit critical section
// *  @param[in] CpuStatus The backup value of processor status register
// *  @return error code
// */
//UINT32 AmbaKAL_CriticalSectionExit(UINT32 CpuStatus)
//{
//    _tx_thread_smp_unprotect(CpuStatus);
//
//    return KAL_ERR_NONE;
//}

/**
 *  AmbaKAL_EventFlagCreate - Create a group of 32 event flags
 *  @param[in] pEventFlag Pointer to an event flags group control block
 *  @param[in] pEventFlagName Pointer to event flags name
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName)
{
    UINT32 RetVal = KAL_ERR_NONE;
    pthread_condattr_t attr;

    (void)pEventFlagName;
    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        pthread_mutex_init(&pEventFlag->mutex, NULL);

        pthread_condattr_init(&attr);
        pthread_cond_init(&pEventFlag->cond, &attr);

        pEventFlag->flag = 0U;
    }

    return RetVal;
}

/**
 *  AmbaKAL_EventFlagDelete - Delete the event flags group
 *  @param[in] pEventFlag Pointer to the Event Flags Group Control Block
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        pthread_mutex_destroy(&pEventFlag->mutex);
        pthread_cond_destroy(&pEventFlag->cond);
    }
    return RetVal;
}

static UINT32 EventFlagPoll(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                            UINT32 AutoClear, UINT32 *pActualFlags)
{
    UINT32 Ret;

    *pActualFlags = pEventFlag->flag;

    /* read */
    if ((AnyOrAll == KAL_FLAGS_WAIT_ANY) && (*pActualFlags & ReqFlags)) {
        Ret = 0U;
    } else if ((AnyOrAll == KAL_FLAGS_WAIT_ALL) && (ReqFlags == (*pActualFlags & ReqFlags))) {
        Ret = 0U;
    } else {
        /* poll failure */
        Ret = KAL_ERR_TIMEOUT;
    }

    /*write */
    if ((Ret == 0U) && (AMBA_KAL_FLAGS_CLEAR_AUTO == AutoClear)) {
        /* flag & ~ReqFlags */
        __sync_and_and_fetch(&pEventFlag->flag, ~ReqFlags);
        pthread_cond_broadcast(&pEventFlag->cond);
    }

    return Ret;
}

static UINT32 EventFlagTimedPoll(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                                 UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut)
{
    struct timespec ts;
    uint64_t time;
    UINT32 _Ret = 0U, Ret = KAL_ERR_TIMEOUT;

    clock_gettime(CLOCK_REALTIME, &ts);
    time = timespec2nsec(&ts);

    /* set specific timeout */
    if (TimeOut != AMBA_KAL_WAIT_FOREVER) {
        time += ((uint64_t)TimeOut*1000000U);
        nsec2timespec(&ts, time);
    } else {
        /* forever: 1000ms loop iteration */
        ts.tv_sec += 1;
    }

    /* lock */
    if (pthread_mutex_timedlock(&pEventFlag->mutex, &ts) == 0) {
        /* poll */
        while (_Ret != ETIMEDOUT) {
            if (0U == EventFlagPoll(pEventFlag, ReqFlags, AnyOrAll, AutoClear, pActualFlags)) {
                Ret = KAL_ERR_NONE;
                break;
            }
            /* unlock & wait */
            _Ret = pthread_cond_timedwait(&pEventFlag->cond, &pEventFlag->mutex, &ts);
        }

        /* unlock */
        pthread_mutex_unlock(&pEventFlag->mutex);
    }

    return Ret;
}

/**
 *  AmbaKAL_EventFlagGet - Take event flags from the event flags group
 *  @param[in] pEventFlag Pointer to the Event Flags Group Control Block
 *  @param[in] ReqFlags A bit pattern indicating which bits to check
 *  @param[in] AnyOrAll Specifies whether all or any of the requested event flags are required
 *  @param[in] AutoClear Clear event flags that satisfy the request
 *  @param[in] TimeOut Timeout value in ms (millisecond)
 *  @param[out] pActualFlags Actual event flags
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagGet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                            UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut)
{
    UINT32 Ret = KAL_ERR_NONE;

    if ((pEventFlag == NULL) || (pActualFlags == NULL)) {
        Ret = KAL_ERR_ARG;
    } else {
        if (TimeOut == AMBA_KAL_NO_WAIT) {
            /* single poll */
            Ret = EventFlagPoll(pEventFlag, ReqFlags, AnyOrAll, AutoClear, pActualFlags);
        } else if (TimeOut == AMBA_KAL_WAIT_FOREVER) {
            for (;;) {
                Ret = EventFlagTimedPoll(pEventFlag, ReqFlags, AnyOrAll, AutoClear, pActualFlags, TimeOut);
                if (Ret == 0U) {
                    break;
                }
            }
        } else {
            /* timeout specified */
            Ret = EventFlagTimedPoll(pEventFlag, ReqFlags, AnyOrAll, AutoClear, pActualFlags, TimeOut);
        }
    }

    return Ret;
}

/**
 *  AmbaKAL_EventFlagSet - The function sets event flags in an event flags group
 *  @param[in] pEventFlag Pointer to the Event Flags Group Control Block
 *  @param[in] SetFlags Specify the event flags to give
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagSet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags)
{
    UINT32 Ret = KAL_ERR_NONE;

    if (pEventFlag == NULL) {
        Ret = KAL_ERR_ARG;
    } else {
        /* important notice: this is a blocking call */
        pthread_mutex_lock(&pEventFlag->mutex);
        /* flag | SetFlags */
        __sync_or_and_fetch(&pEventFlag->flag, SetFlags);
        pthread_cond_broadcast(&pEventFlag->cond);
        pthread_mutex_unlock(&pEventFlag->mutex);
    }

    return Ret;
}

/**
 *  AmbaKAL_EventFlagClear - Clear event flags to the event flags group. All suspended threads whose event flags request is now satisfied are resumed.
 *  @param[in] pEventFlag Pointer to the Event Flags Group Control Block
 *  @param[in] ClearFlags Specify the event flags to clear
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags)
{
    UINT32 Ret = KAL_ERR_NONE;

    if (pEventFlag == NULL) {
        Ret = KAL_ERR_ARG;
    } else {
        /* important notice: this is a blocking call */
        pthread_mutex_lock(&pEventFlag->mutex);
        /* flag & ~ClearFlags */
        __sync_and_and_fetch(&pEventFlag->flag, ~ClearFlags);
        pthread_cond_broadcast(&pEventFlag->cond);
        pthread_mutex_unlock(&pEventFlag->mutex);
    }

    return Ret;
}

/**
 *  AmbaKAL_EventFlagQuery - Retrieve the information about the specified Event Flag
 *  @param[in] pEventFlag Pointer to the Event Flags Group Control Block
 *  @param[out] pEventFlagInfo Pointer to the info structure of the Event Flags Group
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagQuery(AMBA_KAL_EVENT_FLAG_t *pEventFlag, AMBA_KAL_EVENT_FLAG_INFO_s *pEventFlagInfo)
{
    UINT32 Ret = KAL_ERR_NONE;

    if ((pEventFlagInfo == NULL) || (pEventFlag == NULL)) {
        Ret = KAL_ERR_ARG;
    } else {
        pEventFlagInfo->CurrentFlags = pEventFlag->flag;
    }

    return Ret;
}

/**
 *  AmbaKAL_MutexCreate - Create a mutex
 *  @param[in] pMutex Pointer to a Mutex control block
 *  @param[in] pMutexName Pointer to a Mutex name
 *  @return error code
 */
UINT32 AmbaKAL_MutexCreate(AMBA_KAL_MUTEX_t *pMutex, char * pMutexName)
{
    UINT32 RetVal = KAL_ERR_NONE;
    INT32 Ret = 0;
    pthread_mutexattr_t attr;

    (void)pMutexName;

    if (pMutex == NULL) {
        Ret = KAL_ERR_ARG;
    } else {
        Ret = pthread_mutexattr_init(&attr);
        if (Ret == OK) {
            Ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
            if (Ret == OK) {
                Ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
                if (Ret == OK) {
                    Ret = pthread_mutex_init(pMutex, &attr);
                }
            }
        }

        if (Ret != OK) {
            RetVal = KAL_ERR_POSIX;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_MutexDelete - The function deletes the specified mutex
 *  @param[in] pMutex Pointer to the Mutex Control Block
 *  @return error code
 */
UINT32 AmbaKAL_MutexDelete(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        pthread_mutex_destroy(pMutex);
    }

    return RetVal;
}

/**
 *  AmbaKAL_MutexTake - Take the mutex
 *  @param[in] pMutex Pointer to the Mutex Control Block
 *  @param[in] TimeOut TimeOut value in ms (millisecond)
 *  @return error code
 */
UINT32 AmbaKAL_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    UINT32 RetVal = KAL_ERR_NONE;
    struct timespec abs_timeout;
    uint64_t time;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        clock_gettime(CLOCK_REALTIME, &abs_timeout);
        time = timespec2nsec(&abs_timeout);
        time += ((uint64_t)TimeOut*1000000U);
        nsec2timespec(&abs_timeout, time);
        RetVal = pthread_mutex_timedlock(pMutex, &abs_timeout);
    }

    return RetVal;
}

/**
 *  AmbaKAL_MutexGive - Give the mutex
 *  @param[in] pMutex Pointer to the Mutex Control Block
 *  @return error code
 */
UINT32 AmbaKAL_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        RetVal = pthread_mutex_unlock(pMutex);
    }

    return RetVal;
}

///**
// *  AmbaKAL_MutexQuery - Retrieve the information about the specified Mutex
// *  @param[in] pMutex pointer to the Mutex Control Block
// *  @param[out] pMutexInfo pointer to the output buffer for mutex information
// *  @return error code
// */
//UINT32 AmbaKAL_MutexQuery(AMBA_KAL_MUTEX_t *pMutex, AMBA_KAL_MUTEX_INFO_s *pMutexInfo)
//{
//    UINT32 RetVal = KAL_ERR_NONE;
//    UINT32 TxRetVal;
//    char *pName;
//    ULONG Count, SuspendedCount;
//    AMBA_KAL_TASK_t *pOwner;
//    AMBA_KAL_TASK_t *pFirstSuspend;
//    AMBA_KAL_MUTEX_t *pNextMutex;
//
//    if ((pMutex == NULL) || (pMutexInfo == NULL)) {
//        RetVal = KAL_ERR_ARG;
//    } else {
//        TxRetVal = tx_mutex_info_get(pMutex,
//                                     &pName,
//                                     &Count,
//                                     &pOwner,
//                                     &pFirstSuspend,
//                                     &SuspendedCount,
//                                     &pNextMutex);
//
//        if (TxRetVal != TX_SUCCESS) {
//            RetVal = KAL_ERR_ARG;
//        } else {
//            pMutexInfo->pMutexName = pName;
//            pMutexInfo->pOwnerTask = pOwner;
//            pMutexInfo->NumWaitTask = SuspendedCount;
//        }
//    }
//
//    return RetVal;
//}

/**
 *  AmbaKAL_SemaphoreCreate - This function creates a counting semaphore for inter-thread synchronization
 *  @param[in] pSemaphore Pointer to a Semaphore Control Block
 *  @param[in] pSemaphoreName Pointer to the name string of a semaphore
 *  @param[in] InitCount Initial count for this semaphore
 *  @return error code
 */
UINT32 AmbaKAL_SemaphoreCreate(AMBA_KAL_SEMAPHORE_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount)
{
    UINT32 RetVal = KAL_ERR_NONE;
    (void)pSemaphoreName;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        /* always shared between processes via a shared mapping */
        RetVal = sem_init(pSemaphore, 1, InitCount);
    }

    return RetVal;
}

/**
 *  AmbaKAL_SemaphoreDelete - Delete the counting semaphore
 *  @param[in] pSemaphore Pointer to a Semaphore Control Block
 *  @return error code
 */
UINT32 AmbaKAL_SemaphoreDelete(AMBA_KAL_SEMAPHORE_t *pSemaphore)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        RetVal = sem_destroy(pSemaphore);
    }

    return RetVal;
}

/**
 *  AmbaKAL_SemaphoreTake - Take the counting semaphore
 *  @param[in] pSemaphore Pointer to the Semaphore Control Block
 *  @param[in] TimeOut Timeout value in ms (millisecond)
 *  @return error code
 */
UINT32 AmbaKAL_SemaphoreTake(AMBA_KAL_SEMAPHORE_t *pSemaphore, UINT32 TimeOut)
{
    UINT32 RetVal = KAL_ERR_NONE;
    struct timespec abs_timeout;
    uint64_t time;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        clock_gettime(CLOCK_REALTIME, &abs_timeout);
        time = timespec2nsec(&abs_timeout);
        time += ((uint64_t)TimeOut*1000000U);
        nsec2timespec(&abs_timeout, time);
        RetVal = sem_timedwait(pSemaphore, &abs_timeout);
    }

    return RetVal;
}

/**
 *  AmbaKAL_SemaphoreGive - Give the counting semaphore
 *  @param[in] pSempahore Pointer to the Semaphore Control Block
 *  @return error code
 */
UINT32 AmbaKAL_SemaphoreGive(AMBA_KAL_SEMAPHORE_t *pSemaphore)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        RetVal = sem_post(pSemaphore);
    }

    return RetVal;
}

/**
 *  AmbaKAL_SemaphoreQuery - The function retrieves information from the specified semaphore
 *  @param[in] pSemaphore Pointer to the Semaphore Control Block
 *  @param[in] pSemaphoreInfo Pointer to the semaphore info
 *  @return error code
 */
UINT32 AmbaKAL_SemaphoreQuery(AMBA_KAL_SEMAPHORE_t *pSemaphore, AMBA_KAL_SEMAPHORE_INFO_s *pSemaphoreInfo)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    INT32 CurCount;

    if ((pSemaphore == NULL) || (pSemaphoreInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = sem_getvalue(pSemaphore, &CurCount);

        if (TxRetVal != 0U) {
            RetVal = KAL_ERR_ARG;
        } else {
            pSemaphoreInfo->CurrentCount = CurCount;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_MsgQueueCreate - This function creates a message queue that is typically used for inter-thread communication
 *  @param[in] pMsgQueue Pointer to a message queue Control Block
 *  @param[in] pMsgQueueName pointer to the name string of a message queue
 *  @param[in] MsgSize The size of each message in the queue
 *  @param[in] pMsgQueueBuf Starting address of the message queue
 *  @param[in] MsgQueueBufSize Total number of bytes available for the message queue
 *  @return error code
 */
UINT32 AmbaKAL_MsgQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, char *pMsgQueueName, UINT32 MsgSize, void *pMsgQueueBuf, UINT32 MsgQueueBufSize)
{
    UINT32 RetVal = KAL_ERR_NONE;
    struct mq_attr attrs;
    char name[256] = "/";

    (void)pMsgQueueBuf;

    if ((pMsgQueue == NULL) || (pMsgQueueName == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        memset(&attrs, 0, sizeof attrs);
        attrs.mq_maxmsg = MsgQueueBufSize/MsgSize;
        attrs.mq_msgsize = MsgSize;
        strcat(name, pMsgQueueName);

#if 1
        /* unlink it first for process recovery */
        (void)mq_unlink(name);
#endif
        /* mq_open returns a message queue descriptor */
        *pMsgQueue = mq_open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, &attrs);
        /* call mq_setattr() later to change mq_attr */

        if (*pMsgQueue == -1) {
            RetVal = KAL_ERR_POSIX;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_MsgQueueDelete - Delete the message queue
 *  @param[in] pMsgQueue Pointer to the message queue Control Block
 *  @return error code
 */
UINT32 AmbaKAL_MsgQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMsgQueue == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        RetVal = mq_close(*pMsgQueue);
    }

    return RetVal;
}

/**
 *  AmbaKAL_MsgQueueFlush - Flush the message queue
 *  @param[in] pMsgQueue Pointer to the message queue Control Block
 *  @return error code
 */
UINT32 AmbaKAL_MsgQueueFlush(AMBA_KAL_MSG_QUEUE_t *pMsgQueue)
{
    UINT32 RetVal = KAL_ERR_NONE;
    struct timespec abs_timeout;
    struct mq_attr attr;
    void *pMsg;
    INT32 iRet;

    if (pMsgQueue == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        iRet = mq_getattr(*pMsgQueue, &attr);

        clock_gettime(CLOCK_REALTIME, &abs_timeout);
        while (iRet == 0) {
            if (NULL == (pMsg = malloc(attr.mq_msgsize))) {
                iRet = KAL_ERR_NO_FEATURE;
                continue;
            }
            /* iReturns number of bytes in the received message */
            if (-1 == mq_timedreceive(*pMsgQueue, pMsg, attr.mq_msgsize, NULL, &abs_timeout)) {
                iRet = KAL_ERR_NO_FEATURE;
                free(pMsg);
                continue;
            }
            free(pMsg);
        }

        mq_getattr(*pMsgQueue, &attr);
        if (attr.mq_curmsgs != 0) {
            RetVal = KAL_ERR_NO_FEATURE;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_MsgQueueReceive - Receive a message from the message queue
 *  @param[in] pMsgQueue Pointer to the message queue Control Block
 *  @param[in] pMsg Location of where to put the message
 *  @param[in] TimeOut Timeout value in ms (millisecond)
 *  @return error code
 */
UINT32 AmbaKAL_MsgQueueReceive(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut)
{
    UINT32 RetVal = KAL_ERR_NONE;
    struct timespec abs_timeout;
    uint64_t time;
    INT32 iRet;
    struct mq_attr attr;

    if ((pMsgQueue == NULL) || (pMsg == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        iRet = mq_getattr(*pMsgQueue, &attr);
        if (0 == iRet) {
            clock_gettime(CLOCK_REALTIME, &abs_timeout);
            time = timespec2nsec(&abs_timeout);
            time += ((uint64_t)TimeOut*1000000U);
            nsec2timespec(&abs_timeout, time);

            /* returns number of bytes in the received message */
            iRet = mq_timedreceive(*pMsgQueue, pMsg, attr.mq_msgsize, NULL, &abs_timeout);
        }

        if (iRet == -1) {
            RetVal = KAL_ERR_TIMEOUT;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_MsgQueueSend - Send a message from the message queue
 *  @param[in] pMsgQueue Pointer to the message queue Control Block
 *  @param[in] pMsg Pointer to the message
 *  @param[in] TimeOut Timeout value in ms (millisecond)
 *  @return error code
 */
UINT32 AmbaKAL_MsgQueueSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut)
{
    UINT32 RetVal = KAL_ERR_NONE;
    struct timespec abs_timeout;
    uint64_t time;
    struct mq_attr attr;
    INT32 iRet;

    if ((pMsgQueue == NULL) || (pMsg == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        iRet = mq_getattr(*pMsgQueue, &attr);
        if (0 == iRet) {
            clock_gettime(CLOCK_REALTIME, &abs_timeout);
            time = timespec2nsec(&abs_timeout);
            time += ((uint64_t)TimeOut*1000000U);
            nsec2timespec(&abs_timeout, time);

            /* hard code msg_prio to 0 */
            iRet = mq_timedsend(*pMsgQueue, pMsg, attr.mq_msgsize, 0, &abs_timeout);
        }

        if (iRet == -1) {
            RetVal = KAL_ERR_TIMEOUT;
        }
    }

    /* hard code msg_prio to 0 */
    return RetVal;
}

/**
 *  AmbaKAL_MsgQueueQuery - Query the number of messages currently in the queue
 *  @param[in] pMsgQueue Pointer to the message queue Control Block
 *  @param[out] pMsgQueueInfo A pointer to the output buffer for message queue information
 *  @return error code
 */
UINT32 AmbaKAL_MsgQueueQuery(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, AMBA_KAL_MSG_QUEUE_INFO_s * pMsgQueueInfo)
{
    UINT32 RetVal = KAL_ERR_NONE;
    struct mq_attr attr;
    INT32 iRet;

    if ((pMsgQueue == NULL) || (pMsgQueueInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        iRet = mq_getattr(*pMsgQueue, &attr);
        if (0 == iRet) {
            pMsgQueueInfo->NumEnqueued = attr.mq_curmsgs;
            pMsgQueueInfo->NumAvailable = attr.mq_maxmsg - attr.mq_curmsgs;
        } else {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}

INT32 AmbaKAL_Open(const char * pathname, UINT32 flags)
{
    INT32 iRet;

    if (pathname == NULL) {
        iRet = -1;
    } else {
        iRet = open(pathname, flags);
    }

    return iRet;
}

UINT32 AmbaKAL_Close(INT32 fd)
{
    return close(fd);
}

UINT32 AmbaKAL_Ioctl(INT32 fd, UINT32 request, void *data, UINT32 DataSize)
{
    UINT32 RetVal = KAL_ERR_NONE;

    (void) DataSize;
    if (0x0 == ioctl(fd, request, data)) {
        RetVal = KAL_ERR_POSIX;
    }

    return RetVal;
}


/**
 *  AmbaKAL_Mmap - Map a memory region into a process's address space
 */
void *AmbaKAL_Mmap(void *start, SIZE_t length, INT64 prot, INT64 flags, INT32 fd, UINT32 offsize)
{
    void * pRet = NULL;

    if (start != NULL) {
        pRet = mmap(start, length, prot, flags, fd, offsize);

        if (MAP_FAILED == pRet) {
            pRet = NULL;
        }
    }

    return pRet;
}

UINT32 AmbaKAL_Unmap(void *start, SIZE_t length)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (start == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        RetVal = munmap(start, length);
    }

    return RetVal;
}

void AmbaAssert(void)
{
    assert(0);
}
