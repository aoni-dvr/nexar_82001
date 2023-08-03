/**
 *  @file AmbaDiag_ThreadX.c
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
 *  @details Diagnostic functions for ThreadX.
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"

#include "AmbaSYS_Ctrl.h"

#define AMBA_KAL_SOURCE_CODE
#include "AmbaKAL.h"
#include "tx_thread.h"
#include "tx_execution_profile.h"

#ifndef AMBA_PROFILE_MAX_THREADS
#define AMBA_PROFILE_MAX_THREADS    (UINT32)TX_MAX_PRIORITIES
#endif  /* AMBA_PROFILE_MAX_THREADS */

typedef struct {
    CHAR    *pName;
    UINT32  CurState;
    ULONG   RunCount;
    ULONG   TimeSlice;
    UINT32  Priority;
    UINT32  PreemptionThreshold;
    TX_THREAD *pNextThread;
    TX_THREAD *pSuspendedThread;
} DIAG_TASK_INFO_s;

typedef struct {
    CHAR *pName;
    ULONG Count;
    TX_THREAD *Owner;
    TX_THREAD *FirstSuspended;
    ULONG SuspendedCount;
    AMBA_KAL_MUTEX_t *pNextMutex;
} DIAG_MTX_INFO_s;

typedef struct {
    CHAR *pName;
    ULONG CurrentValue;
    TX_THREAD *FirstSuspended;
    ULONG SuspendedCount;
    TX_SEMAPHORE *pNextSem;
} DIAG_SEM_INFO_s;

typedef struct {
    CHAR *pName;
    ULONG CurrentFlag;
    TX_THREAD *FirstSuspended;
    ULONG SuspendedCount;
    AMBA_KAL_EVENT_FLAG_t *pNextFlag;
} DIAG_FLG_INFO_s;

typedef struct {
    CHAR *pName;
    ULONG Enqueued;
    ULONG AvailableStorage;
    TX_THREAD *FirstSuspended;
    ULONG SuspendedCount;
    AMBA_KAL_MSG_QUEUE_t *pNextQueue;
} DIAG_MBF_INFO_s;

extern TX_MUTEX                   *_tx_mutex_created_ptr;
extern AMBA_KAL_SEMAPHORE_t       *_tx_semaphore_created_ptr;
extern AMBA_KAL_MSG_QUEUE_t       *_tx_queue_created_ptr;
extern AMBA_KAL_EVENT_FLAG_t      *_tx_event_flags_created_ptr;

extern ULONG _tx_mutex_created_count;
extern ULONG _tx_semaphore_created_count;
extern ULONG _tx_queue_created_count;
extern ULONG _tx_event_flags_created_count;

static void Diag_PrintFormattedInt(AMBA_SYS_LOG_f LogFunc, const char *pFmtString, UINT32 Value, UINT32 Digits)
{
    char StrBuf[64];
    UINT32 ArgsUInt32[2];
    UINT32 StringLength, i;

    ArgsUInt32[0] = Value;

    StringLength = IO_UtilityStringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgsUInt32);
    if (StringLength < Digits) {
        for (i = 0; i < (Digits - StringLength); i++) {
            LogFunc(" ");
        }
    }
    LogFunc(StrBuf);
}

static void Diag_PrintFormattedStr(AMBA_SYS_LOG_f LogFunc, const char *pFmtString, const char *Value)
{
    char StrBuf[64];
    const char *ArgsStr[2];

    ArgsStr[0] = Value;
    (void)IO_UtilityStringPrintStr(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgsStr);
    LogFunc(StrBuf);
}

static UINT32 Diag_TaskGetIndex(const TX_THREAD *pTargetTask)
{
    const TX_THREAD *pTask = _tx_thread_created_ptr;
    ULONG Index;
    UINT32 Found = 0U;

    for (Index = 0; Index < _tx_thread_created_count; Index++) {
        if (pTask == pTargetTask) {
            Found = 1U;
        }
        pTask = pTask->tx_thread_created_next;
    }

    if (Found == 0U) {
        Index = 0xFFFFFFFFU;
    }

    return Index;
}

static void Diag_TaskShowInfo(UINT32 TaskID, TX_THREAD *pTask, const DIAG_TASK_INFO_s *pTaskInfo, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 AddrStart, AddrEnd;
    UINT32 NextTaskID, SmpCoreSet;

    /* Task Execution State */
    static const char *TaskStateStr[14] = {
        [TX_READY]          = "READY",
        [TX_COMPLETED]      = "COMPLETED",
        [TX_TERMINATED]     = "TERMINATED",
        [TX_SUSPENDED]      = "SUSPENDED",
        [TX_SLEEP]          = "SLEEP",
        [TX_QUEUE_SUSP]     = "QUEUE_SUSP",
        [TX_SEMAPHORE_SUSP] = "SEMAPHORE_SUSP",
        [TX_EVENT_FLAG]     = "EVENT_FLAG",
        [TX_BLOCK_MEMORY]   = "BLOCK_MEMORY",
        [TX_BYTE_MEMORY]    = "BYTE_MEMORY",
        [TX_IO_DRIVER]      = "IO_DRIVER",
        [TX_FILE]           = "FILE",
        [TX_TCP_IP]         = "TCP_IP",
        [TX_MUTEX_SUSP]     = "MUTEX_SUSP",
    };

    /* ID, %3d */
    Diag_PrintFormattedInt(LogFunc, "%d", TaskID, 3U);

    /* Priority, %5d */
    Diag_PrintFormattedInt(LogFunc, "%d", pTaskInfo->Priority, 5U);

    /* PreemptionThreshold, %9d */
    Diag_PrintFormattedInt(LogFunc, "%d", pTaskInfo->PreemptionThreshold, 9U);

    /* RunCount, %7d */
    Diag_PrintFormattedInt(LogFunc, "%d", pTaskInfo->RunCount, 7U);

    /* Task State, %16s */
    Diag_PrintFormattedStr(LogFunc, "%16s", TaskStateStr[pTaskInfo->CurState]);

    /* NextTaskID, %9d */
    NextTaskID = Diag_TaskGetIndex(pTaskInfo->pSuspendedThread);
    if (NextTaskID == 0xFFFFFFFFU) {
        NextTaskID = TaskID;
    }
    Diag_PrintFormattedInt(LogFunc, "%d", NextTaskID, 9U);

    /* Stack Size, %9d */
    Diag_PrintFormattedInt(LogFunc, "%d", pTask->tx_thread_stack_size, 9U);

    /* Stack Peak, %9d */
#ifdef CONFIG_KAL_THREADX_STACK_CHECK
    AmbaMisra_TypeCast32(&AddrStart, &pTask->tx_thread_stack_highest_ptr);
    AmbaMisra_TypeCast32(&AddrEnd, &pTask->tx_thread_stack_end);
    Diag_PrintFormattedInt(LogFunc, "%d", (AddrEnd - AddrStart) + 1U, 9U);
#else
    LogFunc("        0");
#endif

    /* Stack Size Used, %9d */
    AmbaMisra_TypeCast32(&AddrStart, &pTask->tx_thread_stack_ptr);
    AmbaMisra_TypeCast32(&AddrEnd, &pTask->tx_thread_stack_end);
    Diag_PrintFormattedInt(LogFunc, "%d", (AddrEnd - AddrStart) + 1U, 9U);

    /* Stack Start Address, 0x%X */
    AmbaMisra_TypeCast32(&AddrStart, &pTask->tx_thread_stack_start);
    Diag_PrintFormattedInt(LogFunc, "%X", AddrStart, 9U);

    /* Stack End Address, 0x%X */
    AmbaMisra_TypeCast32(&AddrStart, &pTask->tx_thread_stack_end);
    Diag_PrintFormattedInt(LogFunc, "%X", AddrStart, 11U);

    /* Stack Pointer Address, 0x%X */
    AmbaMisra_TypeCast32(&AddrStart, &pTask->tx_thread_stack_ptr);
    Diag_PrintFormattedInt(LogFunc, "%X", AddrStart, 12U);

    /* SMP CoreSet */
    (void)AmbaKAL_TaskGetSmpAffinity(pTask, &SmpCoreSet);
    Diag_PrintFormattedInt(LogFunc, "%X", SmpCoreSet, 9U);

    /* Task Name */
    LogFunc("  ");
    LogFunc(pTaskInfo->pName);
    LogFunc("\n\r");
}

void AmbaDiag_KalShowTaskInfoA32(AMBA_SYS_LOG_f LogFunc)
{
    DIAG_TASK_INFO_s DiagTskInfo;
    TX_THREAD *pTask = _tx_thread_created_ptr;
    ULONG  i;
    UINT32 RetVal;

    if (LogFunc != NULL) {
        LogFunc(" ID  PRI  PREEMPT  SCHED            STAT  NEXT_ID  STKSIZE  STKPEAK  STKUSED  STKBASE     STKEND      STKPTR      SMP  NAME            \n\r");
        LogFunc("---------------------------------------------------------------------------------------------------------------------------------------\n\r");

        for (i = 0; i < _tx_thread_created_count; i++) {
            RetVal = tx_thread_info_get(pTask,                            /* Pointer to thread control block */
                                        &DiagTskInfo.pName,               /* Pointer to the thread's name */
                                        &DiagTskInfo.CurState,            /* Pointer to the thread's current execution state */
                                        &DiagTskInfo.RunCount,            /* Pointer to the threads run count */
                                        &DiagTskInfo.Priority,            /* Pointer to the threads priority */
                                        &DiagTskInfo.PreemptionThreshold, /* Pointer to the threads preemption-threshold */
                                        &DiagTskInfo.TimeSlice,           /* Pointer to the threads time-slice */
                                        &DiagTskInfo.pNextThread,         /* Pointer to the next created thread */
                                        &DiagTskInfo.pSuspendedThread);   /* Pointer to the next thread in suspension list */

            if ((RetVal == TX_SUCCESS) && (DiagTskInfo.CurState != TX_TERMINATED)) {
                Diag_TaskShowInfo(i, pTask, &DiagTskInfo, LogFunc);
            }

            pTask = pTask->tx_thread_created_next;
        }
    }
}

static void Diag_MutexShowInfo(UINT32 MtxID, const DIAG_MTX_INFO_s *pMtxInfo, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 i, StrLen;

    /* ID, %3d */
    Diag_PrintFormattedInt(LogFunc, "%d", MtxID, 3U);

    /* Name, %31s */
    LogFunc("  ");
    if (pMtxInfo->pName != NULL) {
        LogFunc(pMtxInfo->pName);
        StrLen = IO_UtilityStringLength(pMtxInfo->pName);
        if (StrLen < 31U) {
            for (i = 0; i < (31U - StrLen); i++) {
                LogFunc(" ");
            }
        }
    } else {
        LogFunc("                               ");
    }

    /* Count, %5d */
    Diag_PrintFormattedInt(LogFunc, "%d", pMtxInfo->Count, 7U);

    /* SuspendCount, %12d */
    Diag_PrintFormattedInt(LogFunc, "%d", pMtxInfo->SuspendedCount, 14U);

    /* if the Mutex owner is available */
    if (pMtxInfo->Owner != NULL) {
        /* Owner, %s */
        if (pMtxInfo->Owner->tx_thread_name == NULL) {
            LogFunc("   ");
        } else {
            Diag_PrintFormattedStr(LogFunc, "  %s", pMtxInfo->Owner->tx_thread_name);
        }
    }

    if (pMtxInfo->FirstSuspended != NULL) {
        /* First task in suspend list, %s */
        if (pMtxInfo->FirstSuspended->tx_thread_name == NULL) {
            LogFunc("   ");
        } else {
            Diag_PrintFormattedStr(LogFunc, "  %s", pMtxInfo->FirstSuspended->tx_thread_name);
        }
    }

    LogFunc("\n\r");
}

void AmbaDiag_KalShowMutexInfoA32(AMBA_SYS_LOG_f LogFunc)
{
    DIAG_MTX_INFO_s DiagMtxInfo;
    AMBA_KAL_MUTEX_t *pMutex = _tx_mutex_created_ptr;
    ULONG i, RetVal;

    if (LogFunc != NULL) {
        LogFunc("---------------------------------\n\r");
        LogFunc("*******  Total ");
        Diag_PrintFormattedInt(LogFunc, "%d", _tx_mutex_created_count, 3U);
        LogFunc(" Mutex  *******\n\r");
        LogFunc("---------------------------------\n\r");
        LogFunc(" ID  NAME                             Count  SuspendCount  Owner                TaskWaitOn\n\r");

        for (i = 0; i < _tx_mutex_created_count; i ++) {
            RetVal = tx_mutex_info_get(pMutex,
                                       &DiagMtxInfo.pName,
                                       &DiagMtxInfo.Count,
                                       &DiagMtxInfo.Owner,
                                       &DiagMtxInfo.FirstSuspended,
                                       &DiagMtxInfo.SuspendedCount,
                                       &DiagMtxInfo.pNextMutex);
            if (RetVal == TX_SUCCESS) {
                Diag_MutexShowInfo(i, &DiagMtxInfo, LogFunc);
            }

            pMutex = pMutex->tx_mutex_created_next;
        }
    }
}

static void Diag_SemaphoreShowInfo(UINT32 SemID, const DIAG_SEM_INFO_s *pSemInfo, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 i, StrLen;

    /* ID, %3d */
    Diag_PrintFormattedInt(LogFunc, "%d", SemID, 3U);

    /* CurrentValue, %8d */
    Diag_PrintFormattedInt(LogFunc, "%d", pSemInfo->CurrentValue, 10U);

    /* Name, %31s */
    LogFunc("  ");
    if (pSemInfo->pName != NULL) {
        LogFunc(pSemInfo->pName);
        StrLen = IO_UtilityStringLength(pSemInfo->pName);
        if (StrLen < 31U) {
            for (i = 0; i < (31U - StrLen); i++) {
                LogFunc(" ");
            }
        }
    } else {
        LogFunc("                               ");
    }

    /* SuspendCount, %12d */
    Diag_PrintFormattedInt(LogFunc, "%d", pSemInfo->SuspendedCount, 14U);

    /* if the Semaphore FirstSuspended is available */
    if (pSemInfo->FirstSuspended != NULL) {
        if (pSemInfo->FirstSuspended->tx_thread_name == NULL) {
            LogFunc("   ");
        } else {
            Diag_PrintFormattedStr(LogFunc, "  %s", pSemInfo->FirstSuspended->tx_thread_name);
        }
    }

    LogFunc("\n\r");
}

void AmbaDiag_KalShowSemaphoreInfoA32(AMBA_SYS_LOG_f LogFunc)
{
    DIAG_SEM_INFO_s DiagSemInfo;
    AMBA_KAL_SEMAPHORE_t *pSem = _tx_semaphore_created_ptr;
    ULONG i, RetVal;

    if (LogFunc != NULL) {
        LogFunc("---------------------------------\n\r");
        LogFunc("*****  Total ");
        Diag_PrintFormattedInt(LogFunc, "%d", _tx_semaphore_created_count, 3U);
        LogFunc(" Semaphore  *****\n\r");
        LogFunc("---------------------------------\n\r");
        LogFunc(" ID     Count  NAME                             SuspendCount  TaskWaitOn\n\r");

        for (i = 0; i < _tx_semaphore_created_count; i ++) {
            RetVal = tx_semaphore_info_get(pSem,
                                           &DiagSemInfo.pName,
                                           &DiagSemInfo.CurrentValue,
                                           &DiagSemInfo.FirstSuspended,
                                           &DiagSemInfo.SuspendedCount,
                                           &DiagSemInfo.pNextSem);
            if (RetVal == TX_SUCCESS) {
                Diag_SemaphoreShowInfo(i, &DiagSemInfo, LogFunc);
            }

            pSem = pSem->tx_semaphore_created_next;
        }
    }
}

static void Diag_EventFlagsShowInfo(UINT32 FlgID, const DIAG_FLG_INFO_s *pFlgInfo, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 i, StrLen;

    /* ID, %3d */
    Diag_PrintFormattedInt(LogFunc, "%d", FlgID, 3U);

    /* CurrentFlag, 0x%08X */
    LogFunc("  ");
    Diag_PrintFormattedInt(LogFunc, "%p", pFlgInfo->CurrentFlag, 10U);

    /* Name, %31s */
    LogFunc("  ");
    if (pFlgInfo->pName != NULL) {
        LogFunc(pFlgInfo->pName);
        StrLen = IO_UtilityStringLength(pFlgInfo->pName);
        if (StrLen < 31U) {
            for (i = 0; i < (31U - StrLen); i++) {
                LogFunc(" ");
            }
        }
    } else {
        LogFunc("                               ");
    }

    /* SuspendedCount, %12d */
    Diag_PrintFormattedInt(LogFunc, "%d", pFlgInfo->SuspendedCount, 14U);

    if (pFlgInfo->FirstSuspended != NULL) {
        LogFunc("  ");
        if (pFlgInfo->FirstSuspended->tx_thread_name == NULL) {
            LogFunc(" ");
        } else {
            LogFunc(pFlgInfo->FirstSuspended->tx_thread_name);
        }
    }

    LogFunc("\n\r");
}

void AmbaDiag_KalShowEventFlagsInfoA32(AMBA_SYS_LOG_f LogFunc)
{
    DIAG_FLG_INFO_s DiagFlgInfo;
    AMBA_KAL_EVENT_FLAG_t *pEventFlags = _tx_event_flags_created_ptr;
    ULONG i, RetVal;

    if (LogFunc != NULL) {
        LogFunc("-------------------------------------\n\r");
        LogFunc("*****    Total ");
        Diag_PrintFormattedInt(LogFunc, "%d", _tx_event_flags_created_count, 3U);
        LogFunc(" EventFlags   *****\n\r");
        LogFunc("-------------------------------------\n\r");
        LogFunc(" ID  EventFlags  NAME                             SuspendCount  TaskWaitOn\n\r");

        for (i = 0; i < _tx_event_flags_created_count; i ++) {
            RetVal = tx_event_flags_info_get(pEventFlags,
                                             &DiagFlgInfo.pName,
                                             &DiagFlgInfo.CurrentFlag,
                                             &DiagFlgInfo.FirstSuspended,
                                             &DiagFlgInfo.SuspendedCount,
                                             &DiagFlgInfo.pNextFlag);

            if (RetVal == TX_SUCCESS) {
                Diag_EventFlagsShowInfo(i, &DiagFlgInfo, LogFunc);
            }

            pEventFlags = pEventFlags->tx_event_flags_group_created_next;
        }
    }
}

static void Diag_MsgQueueShowInfo(UINT32 FlgID, const DIAG_MBF_INFO_s *pMbfInfo, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 i, StrLen;

    /* ID, %3d */
    Diag_PrintFormattedInt(LogFunc, "%d", FlgID, 3U);

    /* Enqueued, %8d */
    Diag_PrintFormattedInt(LogFunc, "%d", pMbfInfo->Enqueued, 10U);

    /* AvailableStorage, %3d */
    Diag_PrintFormattedInt(LogFunc, "%d", pMbfInfo->AvailableStorage, 7U);

    /* Name, %4s */
    LogFunc("  ");
    if (pMbfInfo->pName != NULL) {
        LogFunc(pMbfInfo->pName);
        StrLen = IO_UtilityStringLength(pMbfInfo->pName);
        if (StrLen < 31U) {
            for (i = 0; i < (31U - StrLen); i++) {
                LogFunc(" ");
            }
        }
    } else {
        LogFunc("                               ");
    }

    if (pMbfInfo->FirstSuspended != NULL) {
        LogFunc("  ");
        if (pMbfInfo->FirstSuspended->tx_thread_name == NULL) {
            LogFunc(" ");
        } else {
            LogFunc(pMbfInfo->FirstSuspended->tx_thread_name);
        }
    }

    LogFunc("\n\r");
}

void AmbaDiag_KalShowMsgQueueInfoA32(AMBA_SYS_LOG_f LogFunc)
{
    DIAG_MBF_INFO_s DiagMbfInfo;
    AMBA_KAL_MSG_QUEUE_t *pMsgQueue = _tx_queue_created_ptr;
    ULONG i, RetVal;

    if (LogFunc != NULL) {
        LogFunc("------------------------------------\n\r");
        LogFunc("*****    Total ");
        Diag_PrintFormattedInt(LogFunc, "%d", _tx_queue_created_count, 3U);
        LogFunc(" MsgQueue    *****\n\r");
        LogFunc("------------------------------------\n\r");
        LogFunc(" ID  Enqueued  Space  NAME                             TaskWaitOn\n\r");

        for (i = 0; i < _tx_queue_created_count; i ++) {
            RetVal = tx_queue_info_get(pMsgQueue,
                                       &DiagMbfInfo.pName,
                                       &DiagMbfInfo.Enqueued,
                                       &DiagMbfInfo.AvailableStorage,
                                       &DiagMbfInfo.FirstSuspended,
                                       &DiagMbfInfo.SuspendedCount,
                                       &DiagMbfInfo.pNextQueue);
            if (RetVal == TX_SUCCESS) {
                Diag_MsgQueueShowInfo(i, &DiagMbfInfo, LogFunc);
            }

            pMsgQueue = pMsgQueue->tx_queue_created_next;
        }
    }
}

void AmbaDiag_KalResetCpuLoadInfo(void)
{
    AmbaSysProfile_Init();
}

void AmbaDiag_KalShowCpuLoadInfoA32(AMBA_SYS_LOG_f LogFunc)
{
    extern UINT32 SysElapsedTimeStart;
    static UINT32 DiagThreadTime[AMBA_PROFILE_MAX_THREADS];
    TX_THREAD *pTask;
    DIAG_TASK_INFO_s TaskInfo;
    ULONG TaskTime, IdleTime[4], IsrTime[4], TotalTaskTime[4];
    UINT32 ElapsedTime;
    UINT64 WorkUINT64;
    UINT32 i, RetVal;
    UINT32 CpuId = 0U;

    if (_tx_thread_created_count <= AMBA_PROFILE_MAX_THREADS) {
        pTask = _tx_thread_created_ptr;
        for (i = 0; i < _tx_thread_created_count; i++) {
            (void)_tx_execution_thread_time_get(pTask, &TaskTime);
            DiagThreadTime[i] = TaskTime;

            pTask = pTask->tx_thread_created_next;
        }

#if TX_THREAD_SMP_MAX_CORES > 0
        (void)_tx_execution_core_thread_total_time_get(0, &TotalTaskTime[0]);
        (void)_tx_execution_core_idle_time_get(0, &IdleTime[0]);
        (void)_tx_execution_core_isr_time_get(0, &IsrTime[0]);
#endif
#if TX_THREAD_SMP_MAX_CORES > 1
        (void)_tx_execution_core_thread_total_time_get(1U, &TotalTaskTime[1]);
        (void)_tx_execution_core_idle_time_get(1U, &IdleTime[1]);
        (void)_tx_execution_core_isr_time_get(1U, &IsrTime[1]);
#endif
#if TX_THREAD_SMP_MAX_CORES > 2
        (void)_tx_execution_core_thread_total_time_get(2U, &TotalTaskTime[2]);
        (void)_tx_execution_core_idle_time_get(2U, &IdleTime[2]);
        (void)_tx_execution_core_isr_time_get(2U, &IsrTime[2]);
#endif
#if TX_THREAD_SMP_MAX_CORES > 3
        (void)_tx_execution_core_thread_total_time_get(3U, &TotalTaskTime[3]);
        (void)_tx_execution_core_idle_time_get(3U, &IdleTime[3]);
        (void)_tx_execution_core_isr_time_get(3U, &IsrTime[3]);
#endif

        (void)AmbaKAL_GetSmpCpuID(&CpuId);
        ElapsedTime = AmbaSysProfile_GetTimeStamp(CpuId);
        ElapsedTime -= SysElapsedTimeStart;
        ElapsedTime ++; /* always round-up */

        LogFunc("----------------------------------------------------\n");
        LogFunc(" Elapsed time: ");
        Diag_PrintFormattedInt(LogFunc, "%u", ElapsedTime, 10U);
        LogFunc("  microseconds\n");
        LogFunc("----------------------------------------------------\n");
        LogFunc(" ID       TIME    PERCENT  TASK_NAME\n");
        LogFunc("----------------------------------------------------\n");

        pTask = _tx_thread_created_ptr;
        for (i = 0; i < _tx_thread_created_count; i++) {
            RetVal = tx_thread_info_get(pTask,
                                        &TaskInfo.pName,
                                        &TaskInfo.CurState,
                                        &TaskInfo.RunCount,
                                        &TaskInfo.Priority,
                                        &TaskInfo.PreemptionThreshold,
                                        &TaskInfo.TimeSlice,
                                        &TaskInfo.pNextThread,
                                        &TaskInfo.pSuspendedThread);

            if ((RetVal == TX_SUCCESS) && (TaskInfo.CurState != TX_TERMINATED)) {
                Diag_PrintFormattedInt(LogFunc, "%d", i, 3U);
                WorkUINT64 = (UINT64)DiagThreadTime[i];
                Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)WorkUINT64, 11U);
                WorkUINT64 *= 1000LLU;
                WorkUINT64 /= (UINT64)ElapsedTime;
                Diag_PrintFormattedInt(LogFunc, "%d", (UINT32)(WorkUINT64 / 10LLU), 8U);
                LogFunc(".");
                Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)(WorkUINT64 % 10LLU), 1U);
                LogFunc("%  ");
                LogFunc(TaskInfo.pName);
                LogFunc("\n");
            }

            pTask = pTask->tx_thread_created_next;
        }

        LogFunc("----------------------------------------------------\n");
        for (i = 0; i < (UINT32)TX_THREAD_SMP_MAX_CORES; i ++) {
            LogFunc(" Core-");
            Diag_PrintFormattedInt(LogFunc, "%u", i, 1U);
            LogFunc(":\n");
            LogFunc("----------------------------------------------------\n");

            LogFunc("    ");
            WorkUINT64 = (UINT64)IsrTime[i];
            Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)WorkUINT64, 10U);
            WorkUINT64 *= 1000LLU;
            WorkUINT64 /= (UINT64)ElapsedTime;
            Diag_PrintFormattedInt(LogFunc, "%d", (UINT32)(WorkUINT64 / 10LLU), 8U);
            LogFunc(".");
            Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)(WorkUINT64 % 10LLU), 1U);
            LogFunc("%  Interrupt Service Routine\n");

            LogFunc("    ");
            WorkUINT64 = (UINT64)TotalTaskTime[i];
            Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)WorkUINT64, 10U);
            WorkUINT64 *= 1000LLU;
            WorkUINT64 /= (UINT64)ElapsedTime;
            Diag_PrintFormattedInt(LogFunc, "%d", (UINT32)(WorkUINT64 / 10LLU), 8U);
            LogFunc(".");
            Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)(WorkUINT64 % 10LLU), 1U);
            LogFunc("%  Task\n");

            LogFunc("    ");
            if ((TotalTaskTime[i] == 0x0U) && (IsrTime[i] == 0x0U)) {
                IdleTime[i] = ElapsedTime;
            }
            WorkUINT64 = (UINT64)IdleTime[i];
            Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)WorkUINT64, 10U);
            WorkUINT64 *= 1000LLU;
            WorkUINT64 /= (UINT64)ElapsedTime;
            Diag_PrintFormattedInt(LogFunc, "%d", (UINT32)(WorkUINT64 / 10LLU), 8U);
            LogFunc(".");
            Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)(WorkUINT64 % 10LLU), 1U);
            LogFunc("%  Idle\n");

            LogFunc("    ");
            if (((ULONG)ElapsedTime) > (TotalTaskTime[i] + IsrTime[i] + IdleTime[i])) {
                WorkUINT64 = (UINT64)ElapsedTime;
                WorkUINT64 -= (UINT64)TotalTaskTime[i];
                WorkUINT64 -= (UINT64)IsrTime[i];
                WorkUINT64 -= (UINT64)IdleTime[i];
            } else {
                WorkUINT64 = (UINT64)0LLU;
            }
            Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)WorkUINT64, 10U);
            WorkUINT64 *= 1000LLU;
            WorkUINT64 /= (UINT64)ElapsedTime;
            Diag_PrintFormattedInt(LogFunc, "%d", (UINT32)(WorkUINT64 / 10LLU), 8U);
            LogFunc(".");
            Diag_PrintFormattedInt(LogFunc, "%u", (UINT32)(WorkUINT64 % 10LLU), 1U);
            LogFunc("%  Overhead\n");

            LogFunc("----------------------------------------------------\n");
        }
    }
}

