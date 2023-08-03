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
#include "AmbaWrap.h"
#include "AmbaDef.h"

#define AMBA_KAL_SOURCE_CODE
#include "AmbaKAL.h"
#include "tx_thread.h"

/**
 *  AmbaKAL_HookStkErrHandler - Register a notification callback for handling thread stack errors
 *  @param[in] StackErrorHandler Callback function for stack error handling
 *  @return error code
 */
UINT32 AmbaKAL_HookStkErrHandler(AMBA_KAL_TASK_STACK_ERROR_HANDLER_f StackErrorHandler)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    VOID (*stack_error_handler)(TX_THREAD *thread_ptr);

    AmbaWrap_memcpy(&stack_error_handler, &StackErrorHandler, sizeof(StackErrorHandler));

    TxRetVal = tx_thread_stack_error_notify(stack_error_handler);
    if (TxRetVal != TX_SUCCESS) {
        RetVal = KAL_ERR_NO_FEATURE;
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskHookProfileFunc - Register an application notification to trace the entry and exit calls of a specific thread
 *  @param[in] pTask A pointer to a thread control block
 *  @param[in] ProfileFunc Function to be called upon thread entry and exit calls
 *  @return error code
 */
UINT32 AmbaKAL_TaskHookProfileFunc(AMBA_KAL_TASK_t *pTask, AMBA_KAL_TASK_PROFILE_FUNC_f ProfileFunc)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    VOID (*thread_entry_exit_notify)(TX_THREAD *thread_ptr, UINT type);

    AmbaWrap_memcpy(&thread_entry_exit_notify, &ProfileFunc, sizeof(ProfileFunc));

    TxRetVal = tx_thread_entry_exit_notify(pTask, thread_entry_exit_notify);
    if (TxRetVal != TX_SUCCESS) {
        RetVal = KAL_ERR_NO_FEATURE;
    }

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
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    void (*TxEntryFunction)(ULONG TxEntryArg);
    ULONG EntryArgTmp;
    UINT32 Priority = TaskPriority;

    if ((pTask == NULL) || (EntryFunction == NULL) || (pStackBase == NULL) || (StackByteSize == 0x0U)) {
        RetVal = KAL_ERR_ARG;
    } else {
        AmbaWrap_memcpy(&TxEntryFunction, &EntryFunction, sizeof(EntryFunction));
        AmbaMisra_TypeCast(&EntryArgTmp, &EntryArg);

        if (TaskPriority > KAL_TASK_PRIORITY_LOWEST) {
            Priority = KAL_TASK_PRIORITY_LOWEST;
        }

        TxRetVal = tx_thread_create(pTask,
                                    pTaskName,
                                    TxEntryFunction,
                                    EntryArgTmp,
                                    pStackBase,
                                    StackByteSize,
                                    Priority,
                                    Priority,
                                    TX_NO_TIME_SLICE,
                                    AutoStart);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskDelete - Delete the software task
 *  @param[in] pTask Pointer to the software task control block
 *  @return error code
 */
UINT32 AmbaKAL_TaskDelete(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        if (tx_thread_identify() == pTask) {
            RetVal = KAL_ERR_CALLER;
        } else {
            TxRetVal = tx_thread_delete(pTask);
            if (TxRetVal == TX_SUCCESS) {
                RetVal = KAL_ERR_NONE;
            } else if (TxRetVal == TX_CALLER_ERROR) {
                RetVal = KAL_ERR_CALLER;
            } else if (TxRetVal == TX_DELETE_ERROR) {
                RetVal = KAL_ERR_THREAD_ACTIVE;
            } else {
                RetVal = KAL_ERR_ARG;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskSuspend - Suspend the software task
 *  @param[in] pTask Pointer to the software task control block
 *  @return error code
 */
UINT32 AmbaKAL_TaskSuspend(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_thread_suspend(pTask);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_THREAD_DORMANT;
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
    UINT32 TxRetVal;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_thread_resume(pTask);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_RESUME;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskAbortCondSuspend - This function aborts sleep or any other conditional suspension of the specified thread.
 *                                 This function does not release explicit suspension that is made by the AmbaKAL_TaskSuspend call.
 *  @param[in] pTask Pointer to the software task control block
 *  @return error code
 */
UINT32 AmbaKAL_TaskAbortCondSuspend(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_thread_wait_abort(pTask);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ABORT;
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
    UINT32 TxRetVal;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_thread_terminate(pTask);
        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskReset - Reset the terminated software task to execute at its entry point.
 *                      The specified thread must be in either terminated or completed state.
 *  @param[in] pTask Pointer to the software task control block
 *  @return error code
 */
UINT32 AmbaKAL_TaskReset(AMBA_KAL_TASK_t *pTask)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_thread_reset(pTask);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_THREAD_ACTIVE;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskQuery - Retrieve the information about the specified software task
 *  @param[in] pTask Pointer to the software task control block
 *  @param[out] pTaskInfo Pointer to the output buffer for the information of the software task
 *  @return error code
 */
UINT32 AmbaKAL_TaskQuery(AMBA_KAL_TASK_t *pTask, AMBA_KAL_TASK_INFO_s *pTaskInfo)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    char *pName;
    UINT32 RunCount, TimeSlice;
    UINT32 CurState, Priority, PreemptionThreshold;
    AMBA_KAL_TASK_t *pNextThread, *pSuspendedThread;

    if ((pTask == NULL) || (pTaskInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_thread_info_get(pTask,
                                      &pName,
                                      &CurState,
                                      &RunCount,
                                      &Priority,
                                      &PreemptionThreshold,
                                      &TimeSlice,
                                      &pNextThread,
                                      &pSuspendedThread);

        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        } else {
            pTaskInfo->TaskState = CurState;    /* current state */
            pTaskInfo->TaskPriority = Priority; /* priority */
            pTaskInfo->pTaskName = pName;       /* pointer to the name of the software task */
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
    UINT32 RetVal = KAL_ERR_NONE;

    if (pTaskPtr == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        *pTaskPtr = tx_thread_identify();
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
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    TxRetVal = tx_thread_sleep(NumTicks);
    if (TxRetVal == TX_WAIT_ABORTED) {
        RetVal = KAL_ERR_WAIT_ABORTED;
    } else if (TxRetVal == TX_CALLER_ERROR) {
        RetVal = KAL_ERR_CALLER;
    } else {
        RetVal = KAL_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskYield - This function is used to relinquish processor control to other ready-to-run threads at the same or higher priority.
 *  @return error code
 */
UINT32 AmbaKAL_TaskYield(void)
{
    tx_thread_relinquish();

    return KAL_ERR_NONE;
}

/**
 *  AmbaKAL_TaskSetUserVal - Set user value
 *  @param[in] pTask Pointer to the software task control block
 *  @param[in] UserVal User Value
 *  @return error code
 */
UINT32 AmbaKAL_TaskSetUserVal(AMBA_KAL_TASK_t *pTask, UINT32 UserVal)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        pTask->UserValue = (UINT32) UserVal;
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskGetUserVal - Get User Value
 *  @param[in] pTask Pointer to the software task control block
 *  @param[out] pUserVal Pointer to the User Value
 *  @return error code
 */
UINT32 AmbaKAL_TaskGetUserVal(const AMBA_KAL_TASK_t *pTask, UINT32 *pUserVal)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if ((pTask == NULL) || (pUserVal == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        *pUserVal = (UINT32) pTask->UserValue;
    }

    return RetVal;
}

/**
 *  AmbaKAL_GetCpuContext - Check if current system state is in ISR or not
 *  @param[out] pContextType Context type. 0 = Interrupt context; otherwise = Non-interrupt context
 *  @return error code
 */
UINT32 AmbaKAL_GetCpuContext(UINT32 * pContextType)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxSystemState;

    if (pContextType == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        /* Determine the caller's context. Mutex locks are only available from initialization and threads.  */
        TxSystemState = TX_THREAD_GET_SYSTEM_STATE();
        if ((TxSystemState == 0U) || (TxSystemState >= TX_INITIALIZE_IN_PROGRESS)) {
            *pContextType = 1U;
        } else {
            *pContextType = 0U;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_GetSysTickCount - Get current system tick counter value
 *  @param[out] pSysTickCount The number of elapsed system ticks.
 *  @return error code
 */
UINT32 AmbaKAL_GetSysTickCount(UINT32 * pSysTickCount)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pSysTickCount == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        *pSysTickCount = ((UINT32) tx_time_get());
    }

    return RetVal;
}

/**
 *  AmbaKAL_GetSmpCpuID - get the cpu ID in SMP system
 *  @param[out] pCpuID The cpu ID.
 *  @return error code
 */
UINT32 AmbaKAL_GetSmpCpuID(UINT32 * pCpuID)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pCpuID == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        *pCpuID = TX_SMP_CORE_ID;
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
    UINT32 TxRetVal;

    if (pTask == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        if ((CpuBindList & (UINT32)TX_THREAD_SMP_CORE_MASK) != 0x0U) {
            TxRetVal = tx_thread_smp_core_exclude(pTask, (~CpuBindList) & (UINT32)TX_THREAD_SMP_CORE_MASK);
        } else {
            /* if no core is assigned, default assign to core0 */
            TxRetVal = tx_thread_smp_core_exclude(pTask, (~0x1U) & (UINT32)TX_THREAD_SMP_CORE_MASK);
        }

        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TaskGetSmpAffinity - This function is used to retrieve the CPU affinity of a thread.
 *  @param[in] pTask Pointer to the software task control block
 *  @param[out] pCpuBindList Pointer to store CPU binding list
 *  @return error code
 */
UINT32 AmbaKAL_TaskGetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 * pCpuBindList)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    UINT32 CpuMap;

    if ((pTask == NULL) || (pCpuBindList == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_thread_smp_core_exclude_get(pTask, &CpuMap);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        } else {
            *pCpuBindList = (UINT32)((~CpuMap) & (UINT32)TX_THREAD_SMP_CORE_MASK);
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TimerSetSmpAffinity - This function enables the binding of a timer to a central processing unit (CPU) or a range of CPUs, so that the timer will execute only on the designated CPU or CPUs rather than any CPU.
 *  @param[in] pTimer Pointer to the timer control block
 *  @param[in] CpuBindList CPU Binding List
 *  @return error code
 */
UINT32 AmbaKAL_TimerSetSmpAffinity(AMBA_KAL_TIMER_t *pTimer, UINT32 CpuBindList)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        if (CpuBindList < (UINT32)TX_THREAD_SMP_MAX_CORES) {
            TxRetVal = tx_timer_smp_core_exclude(pTimer, (UINT32)(~(CpuBindList)) & (UINT32)TX_THREAD_SMP_CORE_MASK);
        } else {
            /* if no core is assigned, default assign to core0 */
            TxRetVal = tx_timer_smp_core_exclude(pTimer, (UINT32)(~0x1U) & (UINT32)TX_THREAD_SMP_CORE_MASK);
        }

        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TimerGetSmpAffinity - This function is used to retrieve the CPU affinity of a thread.
 *  @param[in] pTask Pointer to the software task control block
 *  @param[out] pCpuBindList Pointer to store CPU binding list
 *  @return error code
 */
UINT32 AmbaKAL_TimerGetSmpAffinity(AMBA_KAL_TIMER_t *pTimer, UINT32 * pCpuBindList)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    UINT32 CpuMap;

    if ((pTimer == NULL) || (pCpuBindList == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_timer_smp_core_exclude_get(pTimer, &CpuMap);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        } else {
            *pCpuBindList = (UINT32)((~CpuMap) & (UINT32)TX_THREAD_SMP_CORE_MASK);
        }
    }

    return RetVal;
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
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    void (*TxExpireFunc)(UINT32 TxEntryArg);

    if ((pTimer == NULL) || (ExpireFunc == NULL) ) {
        RetVal = KAL_ERR_ARG;
    } else {
        AmbaWrap_memcpy(&TxExpireFunc, &ExpireFunc, sizeof(ExpireFunc));

        TxRetVal = tx_timer_create(pTimer,
                                   pTimerName,
                                   TxExpireFunc,
                                   ExpireFuncArg,
                                   StartInterval,
                                   PeriodicInterval,
                                   (AutoStart == 1U) ? (TX_AUTO_START) : (TX_DONT_START));

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
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
    UINT32 TxRetVal;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_timer_delete(pTimer);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
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
    UINT32 TxRetVal;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_timer_activate(pTimer);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ACTIVATE;
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
    UINT32 TxRetVal;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_timer_deactivate(pTimer);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TimerChange - The function changes the expiration characteristics of the specified timer.
 *                        The timer must be deactivated prior to calling this function.
 *  @param[in] pTimer Pointer to the timer control block
 *  @param[in] StartInterval Intial ticks for the timer expiration
 *  @param[in] PeriodicInterval Reload ticks for the timer expiration after the 1st. 0 = one-shot
 *  @param[in] AutoStart Automatically start up or not after creation
 *  @return error code
 */
UINT32 AmbaKAL_TimerChange(AMBA_KAL_TIMER_t *pTimer, UINT32 StartInterval, UINT32 PeriodicInterval, UINT32 AutoStart)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pTimer == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        /* Stop the timer first otherwise the changes won't take effect */
        (void)tx_timer_deactivate(pTimer);

        TxRetVal = tx_timer_change(pTimer, StartInterval, PeriodicInterval);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        } else {
            if (AutoStart != 0U) {
                /* Start the timer on desire */
                TxRetVal = tx_timer_activate(pTimer);
                if (TxRetVal != TX_SUCCESS) {
                    RetVal = KAL_ERR_ACTIVATE;
                }
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_TimerQuery - Retrieve the information about the specified software timer
 *  @param[in] pTimer Pointer to the Software timer Control Block
 *  @param[out] pTimerInfo Pointer to the output buffer for the information of the software timer
 *  @return error code
 */
UINT32 AmbaKAL_TimerQuery(AMBA_KAL_TIMER_t *pTimer, AMBA_KAL_TIMER_INFO_s *pTimerInfo)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    char *pName;
    UINT32 CurState;
    UINT32 RemainTicks, PeriodicTicks;
    AMBA_KAL_TIMER_t *pNextTimer;

    if ((pTimer == NULL) || (pTimerInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_timer_info_get(pTimer,
                                     &pName,
                                     &CurState,
                                     &RemainTicks,
                                     &PeriodicTicks,
                                     &pNextTimer);

        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        } else {
            pTimerInfo->pTimerName = pName;
            pTimerInfo->ActiveState = CurState;
            pTimerInfo->RemainInterval = RemainTicks;
            pTimerInfo->PeriodicInterval = PeriodicTicks;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_CriticalSectionEnter - This function is used to enter critical section
 *  @param[in] pCpuStatus The pointer for storing processor status register
 *  @return error code
 */
UINT32 AmbaKAL_CriticalSectionEnter(UINT32 *pCpuStatus)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (pCpuStatus == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        *pCpuStatus = _tx_thread_smp_protect();
    }

    return RetVal;
}

/**
 *  AmbaKAL_CriticalSectionExit - This function is used to exit critical section
 *  @param[in] CpuStatus The backup value of processor status register
 *  @return error code
 */
UINT32 AmbaKAL_CriticalSectionExit(UINT32 CpuStatus)
{
    _tx_thread_smp_unprotect(CpuStatus);

    return KAL_ERR_NONE;
}

/**
 *  AmbaKAL_EventFlagCreate - Create a group of 32 event flags
 *  @param[in] pEventFlag Pointer to an event flags group control block
 *  @param[in] pEventFlagName Pointer to event flags name
 *  @return error code
 */
UINT32 AmbaKAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_event_flags_create(pEventFlag, pEventFlagName);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
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
    UINT32 TxRetVal;

    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_event_flags_delete(pEventFlag);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
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
    UINT32 TxRetVal;
    UINT32 ActualFlags;

    if ((pEventFlag == NULL) || (pActualFlags == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_event_flags_get(pEventFlag, ReqFlags, (((AnyOrAll & 0x1U) << 1U) | (AutoClear & 0x1U)), &ActualFlags, TimeOut);
        if (TxRetVal == TX_SUCCESS) {
            *pActualFlags = (UINT32) ActualFlags;
        } else if (TxRetVal == TX_NO_EVENTS) {
            RetVal = KAL_ERR_TIMEOUT;
        } else if (TxRetVal == TX_DELETED) {
            RetVal = KAL_ERR_DELETED;
        } else if (TxRetVal == TX_WAIT_ABORTED) {
            RetVal = KAL_ERR_WAIT_ABORTED;
        } else {
            RetVal = KAL_ERR_ARG;
        }
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
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_event_flags_set(pEventFlag, SetFlags, TX_OR);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        }
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
    UINT32 TxRetVal;

    if (pEventFlag == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_event_flags_set(pEventFlag, ~(ClearFlags), TX_AND);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        }
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
    UINT32 TxRetVal;
    char *pName;
    UINT32 CurFlags, SuspendedCount;
    AMBA_KAL_TASK_t *pFirstSuspended;
    AMBA_KAL_EVENT_FLAG_t *pNextEventGroup;

    if ((pEventFlag == NULL) || (pEventFlagInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_event_flags_info_get(pEventFlag,          /* Pointer to an event flags group control block */
                                           &pName,              /* Pointer to the event flags group's name */
                                           &CurFlags,           /* Pointer to current set flags */
                                           &pFirstSuspended,    /* Pointer to 1st suspended thread of the event flags group */
                                           &SuspendedCount,     /* Pointer to the number of currently suspended threads */
                                           &pNextEventGroup);   /* Pointer to the next created event flags group */

        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        } else {
            pEventFlagInfo->pEventFlagName = pName;
            pEventFlagInfo->NumWaitTask = (UINT32)SuspendedCount;
            pEventFlagInfo->CurrentFlags = (UINT32)CurFlags;
        }
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
    UINT32 TxRetVal;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_mutex_create(pMutex, pMutexName, TX_INHERIT);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
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
    UINT32 TxRetVal;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_mutex_delete(pMutex);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
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
    UINT32 TxRetVal;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_mutex_get(pMutex, TimeOut);
        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_DELETED) {
            RetVal = KAL_ERR_DELETED;
        } else if (TxRetVal == TX_NOT_AVAILABLE) {
            RetVal = KAL_ERR_TIMEOUT;
        } else if (TxRetVal == TX_WAIT_ABORTED) {
            RetVal = KAL_ERR_WAIT_ABORTED;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
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
    UINT32 TxRetVal;

    if (pMutex == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_mutex_put(pMutex);
        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_NOT_OWNED) {
            RetVal = KAL_ERR_NOT_OWNED;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_MutexQuery - Retrieve the information about the specified Mutex
 *  @param[in] pMutex pointer to the Mutex Control Block
 *  @param[out] pMutexInfo pointer to the output buffer for mutex information
 *  @return error code
 */
UINT32 AmbaKAL_MutexQuery(AMBA_KAL_MUTEX_t *pMutex, AMBA_KAL_MUTEX_INFO_s *pMutexInfo)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    char *pName;
    UINT32 Count, SuspendedCount;
    AMBA_KAL_TASK_t *pOwner;
    AMBA_KAL_TASK_t *pFirstSuspend;
    AMBA_KAL_MUTEX_t *pNextMutex;

    if ((pMutex == NULL) || (pMutexInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_mutex_info_get(pMutex,
                                     &pName,
                                     &Count,
                                     &pOwner,
                                     &pFirstSuspend,
                                     &SuspendedCount,
                                     &pNextMutex);

        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        } else {
            pMutexInfo->pMutexName = pName;
            pMutexInfo->pOwnerTask = pOwner;
            pMutexInfo->NumWaitTask = SuspendedCount;
        }
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
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_semaphore_create(pSemaphore, pSemaphoreName, InitCount);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
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
    UINT32 TxRetVal;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_semaphore_delete(pSemaphore);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
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
    UINT32 TxRetVal;

    if (pSemaphore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_semaphore_get(pSemaphore, TimeOut);
        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_DELETED) {
            RetVal = KAL_ERR_DELETED;
        } else if (TxRetVal == TX_NO_INSTANCE) {
            RetVal = KAL_ERR_TIMEOUT;
        } else if (TxRetVal == TX_WAIT_ABORTED) {
            RetVal = KAL_ERR_WAIT_ABORTED;
        } else {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_SemaphoreGive - Give the counting semaphore
 *  @param[in] pSempahore Pointer to the Semaphore Control Block
 *  @return error code
 */
UINT32 AmbaKAL_SemaphoreGive(AMBA_KAL_SEMAPHORE_t *pSempahore)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;

    if (pSempahore == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_semaphore_put(pSempahore);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaKAL_SemaphoreQuery - The function retrieves information from the specified semaphore
 *  @param[in] pSemaphore Pointer to the Semaphore Control Block
 *  @param[in] pCurCount Pointer to the current semaphore's count value
 *  @return error code
 */
UINT32 AmbaKAL_SemaphoreQuery(AMBA_KAL_SEMAPHORE_t *pSemaphore, AMBA_KAL_SEMAPHORE_INFO_s *pSemaphoreInfo)
{
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    char *pName;
    UINT32 CurCount, SuspendedCount;
    AMBA_KAL_TASK_t *pFirstSuspended;
    AMBA_KAL_SEMAPHORE_t *pNextSem;

    if ((pSemaphore == NULL) || (pSemaphoreInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_semaphore_info_get(pSemaphore,
                                         &pName,
                                         &CurCount,
                                         &pFirstSuspended,
                                         &SuspendedCount,
                                         &pNextSem);

        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        } else {
            pSemaphoreInfo->pSemaphoreName = pName;
            pSemaphoreInfo->CurrentCount = CurCount;
            pSemaphoreInfo->NumWaitTask = SuspendedCount;
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
    UINT32 TxRetVal;
    UINT32 NumWordPerMsg = GetMaxValU32((MsgSize / (sizeof(UINT32))), 1U);

    if ((pMsgQueue == NULL) || (MsgSize == 0U) || (pMsgQueueBuf == NULL) || (MsgQueueBufSize == 0U)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_queue_create(pMsgQueue,
                                   pMsgQueueName,
                                   NumWordPerMsg,
                                   pMsgQueueBuf,
                                   MsgQueueBufSize);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
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
    UINT32 TxRetVal;

    if (pMsgQueue == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_queue_delete(pMsgQueue);

        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_CALLER_ERROR) {
            RetVal = KAL_ERR_CALLER;
        } else {
            RetVal = KAL_ERR_ARG;
        }
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
    UINT32 TxRetVal;

    if (pMsgQueue == NULL) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_queue_flush(pMsgQueue);
        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
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
    UINT32 TxRetVal;

    if ((pMsgQueue == NULL) || (pMsg == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_queue_receive(pMsgQueue, pMsg, TimeOut);
        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_DELETED) {
            RetVal = KAL_ERR_DELETED;
        } else if (TxRetVal == TX_QUEUE_EMPTY) {
            RetVal = KAL_ERR_TIMEOUT;
        } else if (TxRetVal == TX_WAIT_ABORTED) {
            RetVal = KAL_ERR_WAIT_ABORTED;
        } else {
            RetVal = KAL_ERR_ARG;
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
    UINT32 TxRetVal;

    if ((pMsgQueue == NULL) || (pMsg == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_queue_send(pMsgQueue, pMsg, TimeOut);
        if (TxRetVal == TX_SUCCESS) {
            RetVal = KAL_ERR_NONE;
        } else if (TxRetVal == TX_DELETED) {
            RetVal = KAL_ERR_DELETED;
        } else if (TxRetVal == TX_QUEUE_FULL) {
            RetVal = KAL_ERR_TIMEOUT;
        } else if (TxRetVal == TX_WAIT_ABORTED) {
            RetVal = KAL_ERR_WAIT_ABORTED;
        } else {
            RetVal = KAL_ERR_ARG;
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
    UINT32 RetVal = KAL_ERR_NONE;
    UINT32 TxRetVal;
    char *pName;
    UINT32 CurCount, AvailableBuf, SuspendedCount;
    TX_THREAD *pFirstSuspended;
    TX_QUEUE *pNextQueue;

    if ((pMsgQueue == NULL) || (pMsgQueueInfo == NULL)) {
        RetVal = KAL_ERR_ARG;
    } else {
        TxRetVal = tx_queue_info_get(pMsgQueue,
                                     &pName,
                                     &CurCount,
                                     &AvailableBuf,
                                     &pFirstSuspended,
                                     &SuspendedCount,
                                     &pNextQueue);

        if (TxRetVal != TX_SUCCESS) {
            RetVal = KAL_ERR_ARG;
        } else {
            pMsgQueueInfo->pMsgQueueName = pName;
            pMsgQueueInfo->NumEnqueued = CurCount;
            pMsgQueueInfo->NumAvailable = AvailableBuf;
            pMsgQueueInfo->NumWaitTask = SuspendedCount;
        }
    }

    return RetVal;
}
