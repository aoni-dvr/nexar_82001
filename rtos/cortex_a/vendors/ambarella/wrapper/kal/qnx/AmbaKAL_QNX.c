/**
 *  @file AmbaKAL_ThreadX.c
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
 *  @details Ambarella KAL (Kernel Abstraction Layer) APIs for ThreadX
 *
 */
#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"

#include "AmbaKAL.h"

typedef struct {
    UINT32 ExpireFuncArg;
    UINT32 StartInterval;
    UINT32 PeriodicInterval;
    UINT32 AutoStart;
    AMBA_KAL_TIMER_EXPIRY_f ExpireFunc;
} AMBA_TIMER_SETTING_s;

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
    pthread_attr_t attr;
    int QnxRet = KAL_ERR_NONE;
    void * ArgTmp;

    if ((pTask == NULL) || (EntryFunction == NULL)) {
        QnxRet = KAL_ERR_ARG;
    } else {
        pthread_attr_init(&attr);
        if ((pStackBase != NULL) && (0x0 != StackByteSize)) {
            pthread_attr_setstack(&attr, pStackBase, StackByteSize);
        }

        if (AutoStart) {
            pthread_attr_setsuspendstate_np(&attr, PTHREAD_CREATE_NOT_SUSPENDED);
        } else {
            pthread_attr_setsuspendstate_np(&attr, PTHREAD_CREATE_SUSPENDED);
        }

        AmbaMisra_TypeCast(&ArgTmp, &EntryArg);
        QnxRet = pthread_create(pTask, &attr, EntryFunction, ArgTmp);
        pthread_setname_np(*pTask, (const char *)pTaskName);

        if (TaskPriority >= 127U) {
            QnxRet = pthread_setschedprio(*pTask, 1U);
        } else if (0x0U == TaskPriority) {
            QnxRet = pthread_setschedprio(*pTask, 127U);
        } else {
            QnxRet = pthread_setschedprio(*pTask, 127U-TaskPriority);
        }
    }

    return QnxRet;
}
/**
 *  AmbaKAL_TaskDelete - Delete the software task
 *  @param[in] pTask Pointer to the software task control block
 *  @return error code
 */
UINT32 AmbaKAL_TaskDelete(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 QnxRet;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        QnxRet = pthread_detach(*pTask);
        if (QnxRet != OK) {
            printf("AmbaKAL_TaskDelete failed, QnxRet = 0x%08x\n",QnxRet);
        } else {
            /* do nothing */
        }
    }
    return RetVal;
}

UINT32 AmbaKAL_TaskSuspend(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 QnxRet;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        QnxRet = ThreadCtlExt_r(getpid(), *pTask, _NTO_TCTL_ONE_THREAD_HOLD, 0);
        if (QnxRet != OK) {
            printf("AmbaKAL_TaskSuspend failed, QnxRet = 0x%08x\n",QnxRet);
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
    UINT32 QnxRet;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        QnxRet = ThreadCtlExt_r(getpid(), *pTask, _NTO_TCTL_ONE_THREAD_CONT, 0);
        if (QnxRet != OK) {
            printf("AmbaKAL_TaskResume failed, QnxRet = 0x%08x\n",QnxRet);
        } else {
            /* do nothing */
        }
    }
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
    UINT32 QnxRet;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        QnxRet = pthread_cancel(*pTask);
        if (QnxRet != OK) {
            //it is normal when return 3 "No such process"
            //printf("AmbaKAL_TaskTerminate failed, QnxRet = 0x%08x\n",QnxRet);
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
    int i = 0U;

    if ((pTask == NULL) || (pTaskInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        pthread_getschedparam(*pTask, NULL, &TaskParam);
        pTaskInfo->TaskPriority = TaskParam.sched_priority; /* priority */
        pthread_getname_np(*pTask, &Name[0], 64U);       /* pointer to the name of the software task */
        for (i = 0; i < 64; i++) {
            pTaskInfo->pTaskName[i] = Name[i];
            if (Name[i] == '\0') {
                break;
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
    if ((pTaskPtr == NULL) || (*pTaskPtr == NULL)) {
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
    (void)SchedYield();

    return KAL_ERR_NONE;
}

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
        *pCpuID = SchedGetCpuNum();
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
    UINT32 RetVal = KAL_ERR_NONE;
    int QnxRet;
    pid_t pid = 0;
    UINT32 CoreList = CpuBindList;
    void * MaskTmp = NULL;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        pid = getpid();
        procmgr_ability(pid, PROCMGR_AID_XTHREAD_THREADCTL | PROCMGR_AID_EOL);
        if ((CpuBindList & (UINT32)SMP_CORE_MASK) != 0x0U) {
            AmbaMisra_TypeCast(&MaskTmp, &CoreList);
            QnxRet = ThreadCtlExt_r(pid, *pTask, _NTO_TCTL_RUNMASK, MaskTmp);
        } else {
            /* if no core is assigned, default assign to core0 */
            CoreList = 0x1U;
            AmbaMisra_TypeCast(&MaskTmp, &CoreList);
            QnxRet = ThreadCtlExt_r(pid, *pTask, _NTO_TCTL_RUNMASK, MaskTmp);
        }

        if (QnxRet != 0) {
            //FIXME, need to check error status
            printf("AmbaKAL_TaskSetSmpAffinity failed QnxRet = 0x%08x\n",QnxRet);
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}
//
UINT32 AmbaKAL_TaskGetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 * pCpuBindList)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 CpuMap = 0x0;
    pid_t pid = 0;
    int QnxRet;

    if ((pTask == NULL) || (pCpuBindList == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        pid = getpid();
        QnxRet = ThreadCtlExt_r(pid, *pTask, _NTO_TCTL_RUNMASK_GET_AND_SET, &CpuMap);
        if (QnxRet != 0) {
            printf("AmbaKAL_TaskGetSmpAffinity failed QnxRet = 0x%08x\n",QnxRet);
            RetVal = KAL_ERR_ARG;
        } else {
            *pCpuBindList = CpuMap;
        }
    }

    return RetVal;
}

static void *KAL_SuspendThread(void* argv)
{
    AMBA_KAL_TASK_t * pTid = (AMBA_KAL_TASK_t *)argv;

    ThreadCtlExt_r(getpid(), *pTid, _NTO_TCTL_ONE_THREAD_HOLD, 0);
    pthread_exit(NULL);
}

static void *KAL_TimerEntry(void* argv)
{
    AMBA_TIMER_SETTING_s TimerSetting = {0U};
    AMBA_TIMER_SETTING_s *pTimerSetting = (AMBA_TIMER_SETTING_s *)argv;
    pthread_attr_t attr;
    UINT32 uSec = 0U;
    UINT32 Sec = 0U;
    UINT32 Tid = pthread_self();

    TimerSetting.ExpireFunc = pTimerSetting->ExpireFunc;
    TimerSetting.ExpireFuncArg = pTimerSetting->ExpireFuncArg;
    TimerSetting.PeriodicInterval = pTimerSetting->PeriodicInterval;
    TimerSetting.StartInterval = pTimerSetting->StartInterval;
    TimerSetting.AutoStart = pTimerSetting->AutoStart;

    if (TimerSetting.AutoStart != KAL_START_AUTO) {
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(NULL, &attr, KAL_SuspendThread, &Tid) != OK) {
            printf("%s %d, ThreadCtl_r(%d)\n", __FUNCTION__, __LINE__, Tid);
        } else {
            usleep(100);
        }
    } else {
        /* do nothing */
    }

    uSec = TimerSetting.StartInterval % 1000;
    uSec = uSec * 1000;
    Sec = TimerSetting.StartInterval / 1000;

    usleep(uSec);
    sleep(Sec);
    TimerSetting.ExpireFunc(TimerSetting.ExpireFuncArg);

    if (0x0U != TimerSetting.PeriodicInterval) {
        uSec = TimerSetting.PeriodicInterval % 1000;
        uSec = uSec * 1000;
        Sec = TimerSetting.PeriodicInterval / 1000;
        while (1) {
            usleep(uSec);
            sleep(Sec);
            TimerSetting.ExpireFunc(TimerSetting.ExpireFuncArg);
        }
    } else {
        pthread_exit(NULL);
    }
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
    AMBA_TIMER_SETTING_s TimerSetting = {0U};
    INT32 QnxRet = KAL_ERR_NONE;
    UINT32 RetVal = KAL_ERR_NONE;

    if ((pTimer == NULL) || (ExpireFunc == NULL) ) {
        RetVal = KAL_ERR_ARG;
    } else {
        TimerSetting.ExpireFunc = ExpireFunc;
        TimerSetting.ExpireFuncArg = ExpireFuncArg;
        TimerSetting.PeriodicInterval = PeriodicInterval;
        TimerSetting.StartInterval = StartInterval;
        TimerSetting.AutoStart = AutoStart;

        QnxRet = pthread_create(pTimer, NULL, KAL_TimerEntry, &TimerSetting);

        if (QnxRet == OK) {
            if (pTimerName != NULL) {
                pthread_setname_np(*pTimer, (const char *)pTimerName);
            }
        } else {
            printf("AmbaKAL_TimerCreate failed, error = %d\n", QnxRet);
            RetVal = KAL_ERR_POSIX;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TimerDelete - Delete an application timer
 *  @param[in] pTimer Pointer to the timer control block
 *  @return error code
 */
UINT32 AmbaKAL_TimerDelete(AMBA_KAL_TIMER_t *pTimer)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 QnxRet;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        QnxRet = pthread_cancel(*pTimer);
        if (QnxRet != OK) {
            printf("AmbaKAL_TimerDelete failed, QnxRet = 0x%08x\n",QnxRet);
        } else {
            QnxRet = pthread_detach(*pTimer);
            if (KAL_ERR_NONE != QnxRet) {
                RetVal = KAL_ERR_POSIX;
                printf("AmbaKAL_TimerDelete failed, error = %d\n", QnxRet);
            } else {
                /* do nothing */
            }
        }
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
    UINT32 QnxRet;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        QnxRet = ThreadCtlExt_r(getpid(), *pTimer, _NTO_TCTL_ONE_THREAD_CONT, 0);
        if (QnxRet != OK) {
            printf("AmbaKAL_TimerStart failed, QnxRet = 0x%08x\n",QnxRet);
        } else {
            /* do nothing */
        }
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
    UINT32 QnxRet;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        QnxRet = ThreadCtlExt_r(getpid(), *pTimer, _NTO_TCTL_ONE_THREAD_HOLD, 0);
        if (QnxRet != OK) {
            printf("AmbaKAL_TimerStop failed, QnxRet = 0x%08x\n",QnxRet);
        } else {
            /* do nothing */
        }
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
//
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

    if (pEventFlag != NULL) {
        (void)pEventFlagName;
        pthread_mutex_init(&pEventFlag->mutex, NULL);

        pthread_condattr_init(&attr);
        pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        pthread_cond_init(&pEventFlag->cond, &attr);

        atomic_set(&pEventFlag->flag, 0U);
    } else {
        RetVal = KAL_ERR_ARG;
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

    if (pEventFlag != NULL) {
        pthread_mutex_destroy(&pEventFlag->mutex);
        pthread_cond_destroy(&pEventFlag->cond);
    } else {
        RetVal = KAL_ERR_ARG;
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
        atomic_clr_value(&pEventFlag->flag, ReqFlags);
        pthread_cond_broadcast(&pEventFlag->cond);
    }

    return Ret;
}

static UINT32 EventFlagTimedPoll(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                                 UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut)
{
    struct timespec ts;
    uint64_t time;
    UINT32 Ret = KAL_ERR_TIMEOUT;
    INT32 QnxRet = EOK;

    clock_gettime(CLOCK_MONOTONIC, &ts);
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
    QnxRet = pthread_mutex_timedlock_monotonic(&pEventFlag->mutex, &ts);
    if (QnxRet == EOK) {
        /* poll */
        while (QnxRet != ETIMEDOUT) {
            if (0U == EventFlagPoll(pEventFlag, ReqFlags, AnyOrAll, AutoClear, pActualFlags)) {
                Ret = KAL_ERR_NONE;
                break;
            }
            /* unlock & wait */
            QnxRet = pthread_cond_timedwait(&pEventFlag->cond, &pEventFlag->mutex, &ts);
        }

        /* unlock */
        pthread_mutex_unlock(&pEventFlag->mutex);
    } else {
        if (QnxRet == ETIMEDOUT) {
            fprintf(stderr,"%s %d, mutex lock failed\n",__FUNCTION__, __LINE__);
        }
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
    UINT32 RetVal = KAL_ERR_NONE;

    if ((pEventFlag != NULL) && (pActualFlags != NULL)) {
    if (TimeOut == AMBA_KAL_NO_WAIT) {
        /* single poll */
            RetVal = EventFlagPoll(pEventFlag, ReqFlags, AnyOrAll, AutoClear, pActualFlags);
    } else if (TimeOut == AMBA_KAL_WAIT_FOREVER) {
        for (;;) {
                RetVal = EventFlagTimedPoll(pEventFlag, ReqFlags, AnyOrAll, AutoClear, pActualFlags, TimeOut);
                if (RetVal == OK) {
                break;
            }
        }
    } else {
        /* timeout specified */
            RetVal = EventFlagTimedPoll(pEventFlag, ReqFlags, AnyOrAll, AutoClear, pActualFlags, TimeOut);
        }
    } else {
        RetVal = KAL_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaKAL_EventFlagSet - The function sets event flags in an event flags group
 *  @param[in] pEventFlag Pointer to the Event Flags Group Control Block
 *  @param[in] SetFlags Specify the event flags to give
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagSet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags)
{
    INT32 Ret = EOK;
    UINT32 RetVal = KAL_ERR_NONE;

    if (pEventFlag != NULL) {
    /* important notice: this is a blocking call */
        Ret = pthread_mutex_lock(&pEventFlag->mutex);
        if (Ret == EOK) {
    atomic_set_value(&pEventFlag->flag, pEventFlag->flag | SetFlags);
    pthread_cond_broadcast(&pEventFlag->cond);
    pthread_mutex_unlock(&pEventFlag->mutex);
        } else {
            fprintf(stderr,"%s %d, pEventFlag->mutex lock failed\n",__FUNCTION__, __LINE__);
            RetVal = KAL_ERR_ARG;
        }
    } else {
        RetVal = KAL_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaKAL_EventFlagClear - Clear event flags to the event flags group. All suspended threads whose event flags request is now satisfied are resumed.
 *  @param[in] pEventFlag Pointer to the Event Flags Group Control Block
 *  @param[in] ClearFlags Specify the event flags to clear
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pEventFlag != NULL) {
    /* important notice: this is a blocking call */
    pthread_mutex_lock(&pEventFlag->mutex);
    atomic_clr_value(&pEventFlag->flag, ClearFlags);
    pthread_cond_broadcast(&pEventFlag->cond);
    pthread_mutex_unlock(&pEventFlag->mutex);
    } else {
        RetVal = KAL_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaKAL_EventFlagQuery - Retrieve the information about the specified Event Flag
 *  @param[in] pEventFlag Pointer to the Event Flags Group Control Block
 *  @param[out] pEventFlagInfo Pointer to the info structure of the Event Flags Group
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagQuery(AMBA_KAL_EVENT_FLAG_t *pEventFlag, AMBA_KAL_EVENT_FLAG_INFO_s *pEventFlagInfo)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if ((pEventFlag != NULL) && (pEventFlagInfo != NULL)) {
    pEventFlagInfo->CurrentFlags = pEventFlag->flag;
    } else {
        RetVal = KAL_ERR_ARG;
    }

    return RetVal;
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
    INT32 QnxRet = 0;
    pthread_mutexattr_t attr;

    (void)pMutexName;

    if (pMutex != NULL) {
    QnxRet = pthread_mutexattr_init(&attr);
    if (QnxRet == OK) {
        QnxRet = pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_ENABLE);
        if (QnxRet == OK) {
            QnxRet = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
            if (QnxRet == OK) {
                QnxRet = pthread_mutex_init(pMutex, &attr);
            }
        }
    }

    if (QnxRet != OK) {
        RetVal = KAL_ERR_POSIX;
    }
    } else {
        RetVal = KAL_ERR_ARG;
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
    INT32 QnxRet = 0;

    if (pMutex != NULL) {
        QnxRet = pthread_mutex_destroy(pMutex);
        if (QnxRet != OK) {
            RetVal = KAL_ERR_POSIX;
        }
    } else {
        RetVal = KAL_ERR_ARG;
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
    struct timespec abs_timeout;
    uint64_t time;
    INT32 QnxRet;
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMutex != NULL) {
    clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
    time = timespec2nsec(&abs_timeout);
    time += ((uint64_t)TimeOut*1000000U);
    nsec2timespec(&abs_timeout, time);

        QnxRet = pthread_mutex_timedlock_monotonic(pMutex, &abs_timeout);
        if (QnxRet != OK) {
            RetVal = KAL_ERR_POSIX;
        }
    } else {
        RetVal = KAL_ERR_ARG;
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
    INT32 QnxRet;
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMutex != NULL) {
        QnxRet = pthread_mutex_unlock(pMutex);
        if (QnxRet != OK) {
            RetVal = KAL_ERR_POSIX;
        }
    } else {
        RetVal = KAL_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaKAL_SemaphoreCreate - This function creates a counting semaphore for inter-thread synchronization
 *  @param[in] pSemaphore Pointer to a Semaphore Control Block
 *  @param[in] pSemaphoreName Pointer to the name string of a semaphore
 *  @param[in] InitCount Initial count for this semaphore
 *  @return error code
 */
UINT32 AmbaKAL_SemaphoreCreate(AMBA_KAL_SEMAPHORE_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount)
{
    INT32 QnxRet;
    UINT32 RetVal = KAL_ERR_NONE;

    (void)pSemaphoreName;
    if (pSemaphore != NULL) {
        QnxRet = sem_init(pSemaphore, 1, InitCount);
        if (QnxRet != OK) {
            RetVal = KAL_ERR_POSIX;
        }
    } else {
        RetVal = KAL_ERR_ARG;
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
    INT32 QnxRet;
    UINT32 RetVal = KAL_ERR_NONE;

    if (pSemaphore != NULL) {
        QnxRet = sem_destroy(pSemaphore);
        if (QnxRet != OK) {
            RetVal = KAL_ERR_POSIX;
        }
    } else {
        RetVal = KAL_ERR_ARG;
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
    struct timespec abs_timeout;
    uint64_t time;

    INT32 QnxRet;
    UINT32 RetVal = KAL_ERR_NONE;

    if (pSemaphore != NULL) {
    clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
    time = timespec2nsec(&abs_timeout);
    time += ((uint64_t)TimeOut*1000000U);
    nsec2timespec(&abs_timeout, time);

        QnxRet = sem_timedwait_monotonic(pSemaphore, &abs_timeout);
        if (QnxRet != OK) {
            RetVal = KAL_ERR_POSIX;
        }
    } else {
        RetVal = KAL_ERR_ARG;
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
    INT32 QnxRet;
    UINT32 RetVal = KAL_ERR_NONE;

    if (pSemaphore != NULL) {
        QnxRet = sem_post(pSemaphore);
        if (QnxRet != OK) {
            RetVal = KAL_ERR_POSIX;
        }
    } else {
        RetVal = KAL_ERR_ARG;
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
    struct mq_attr attrs;
    UINT32 RetVal = KAL_ERR_NONE;

    if ((pMsgQueue == NULL) || (MsgSize == 0U) || (MsgQueueBufSize == 0U)) {
        RetVal = KAL_ERR_ARG;
    } else {
    (void)pMsgQueueBuf;
        memset(&attrs, 0, sizeof(attrs));
    attrs.mq_maxmsg = MsgQueueBufSize/MsgSize;
    attrs.mq_msgsize = MsgSize;

#if 0
    /* do not unlink so when pid1 terminated, pid2 still gets it */
    (void)mq_unlink(pMsgQueueName);
#endif
    /* mq_open returns a message queue descriptor */
    *pMsgQueue = mq_open(pMsgQueueName, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, &attrs);
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
    INT32 QnxRet;
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMsgQueue != NULL) {
        QnxRet = mq_close(*pMsgQueue);
        if (QnxRet != OK) {
            RetVal = KAL_ERR_POSIX;
        }
    } else {
        RetVal = KAL_ERR_ARG;
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
    struct timespec abs_timeout;
    struct mq_attr attr;
    void *pMsg;
    UINT32 RetVal = KAL_ERR_NONE;

    if (pMsgQueue != NULL) {
        mq_getattr(*pMsgQueue, &attr);

    clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
        while (RetVal == 0) {
        if (NULL == (pMsg = malloc(attr.mq_msgsize))) {
                RetVal = KAL_ERR_NO_FEATURE;
            continue;
        }
        /* returns number of bytes in the received message */
        if (-1 == mq_timedreceive_monotonic(*pMsgQueue, pMsg, attr.mq_msgsize, NULL, &abs_timeout)) {
                RetVal = KAL_ERR_NO_FEATURE;
            free(pMsg);
            continue;
        }
        free(pMsg);
    }

    mq_getattr(*pMsgQueue, &attr);
    if (attr.mq_curmsgs == 0) {
            RetVal = KAL_ERR_NONE;
        } else {
            RetVal = KAL_ERR_NO_FEATURE;
        }
    } else {
        RetVal = KAL_ERR_ARG;
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
    struct timespec abs_timeout;
    uint64_t time;
    INT32 Ret;
    struct mq_attr attr;
    UINT32 RetVal = KAL_ERR_NONE;

    if ((pMsgQueue == NULL) || (pMsg == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
    Ret = mq_getattr(*pMsgQueue, &attr);
        if (KAL_ERR_NONE == Ret) {
        clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
        time = timespec2nsec(&abs_timeout);
        time += ((uint64_t)TimeOut*1000000U);
        nsec2timespec(&abs_timeout, time);


        /* returns number of bytes in the received message */
        Ret = mq_timedreceive_monotonic(*pMsgQueue, pMsg, attr.mq_msgsize, NULL, &abs_timeout);
    }

        if (-1 == Ret) {
            RetVal = KAL_ERR_POSIX;
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
    struct timespec abs_timeout;
    uint64_t time;
    struct mq_attr attr;
    INT32 Ret;
    UINT32 RetVal = KAL_ERR_NONE;

    if ((pMsgQueue == NULL) || (pMsg == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
    Ret = mq_getattr(*pMsgQueue, &attr);
        if (KAL_ERR_NONE == Ret) {
        clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
        time = timespec2nsec(&abs_timeout);
        time += ((uint64_t)TimeOut*1000000U);
        nsec2timespec(&abs_timeout, time);

        /* hard code msg_prio to 0 */
        Ret = mq_timedsend_monotonic(*pMsgQueue, pMsg, attr.mq_msgsize, 0, &abs_timeout);
    }

        if (-1 == Ret) {
            RetVal = KAL_ERR_POSIX;
        }
    }

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
    struct mq_attr attr;
    INT32 Ret;
    UINT32 RetVal = KAL_ERR_NONE;

    if ((pMsgQueue == NULL) || (pMsgQueueInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
    Ret = mq_getattr(*pMsgQueue, &attr);
    if (0 == Ret) {
        pMsgQueueInfo->NumEnqueued = attr.mq_curmsgs;
        pMsgQueueInfo->NumAvailable = attr.mq_maxmsg - attr.mq_curmsgs;
        } else {
            RetVal = KAL_ERR_POSIX;
        }
    }

    return RetVal;
}

INT32 AmbaKAL_Open(const char * pathname, UINT32 flags)
{
    INT32 QnxRet;

    if (pathname != NULL) {
        QnxRet = open(pathname, flags);
    } else {
        QnxRet = -1;
    }

    return QnxRet;
}

UINT32 AmbaKAL_Close(INT32 fd)
{
    return close(fd);
}

UINT32 AmbaKAL_Ioctl(INT32 fd, UINT32 request, void *data, UINT32 DataSize)
{
    UINT32 Ret = KAL_ERR_NONE;

    if (0x0 == devctl(fd, request, data, DataSize, NULL)) {
        Ret = KAL_ERR_POSIX;
    }

    return Ret;
}


/**
 *  AmbaKAL_Mmap - Map a memory region into a process's address space
 */
void *AmbaKAL_Mmap(void *start, SIZE_t length, INT64 prot, INT64 flags, INT32 fd, UINT32 offsize)
{
    void * pRet = mmap64(start, length, prot, flags, fd, offsize);

    if (MAP_FAILED == pRet) {
        pRet = NULL;
    }

    return pRet;
}

UINT32 AmbaKAL_Unmap(void *start, SIZE_t length)
{
    return munmap(start, length);
}
