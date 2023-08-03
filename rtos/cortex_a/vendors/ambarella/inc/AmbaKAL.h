/**
 *  @file AmbaKAL.h
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
 *  @details Definitions for Ambarella KAL (Kernel Abstraction Layer)
 *
 */

#ifndef AMBA_KAL_H
#define AMBA_KAL_H

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#if defined(CONFIG_QNX)
#include <qnx/amba_osal.h>
#elif defined(CONFIG_LINUX)
#include <linux/amba_osal.h>
#else
#include <threadx/amba_osal.h>
#endif

/*
 * Defined in AmbaKAL_xxx.c
 */
UINT32 AmbaKAL_HookStkErrHandler(AMBA_KAL_TASK_STACK_ERROR_HANDLER_f StackErrorHandler);
UINT32 AmbaKAL_TaskHookProfileFunc(AMBA_KAL_TASK_t *pTask, AMBA_KAL_TASK_PROFILE_FUNC_f ProfileFunc);
UINT32 AmbaKAL_TaskCreate(AMBA_KAL_TASK_t *pTask, char *pTaskName, UINT32 TaskPriority,
                          AMBA_KAL_TASK_ENTRY_f EntryFunction, const void * EntryArg,
                          void *pStackBase, UINT32 StackByteSize, UINT32 AutoStart);
UINT32 AmbaKAL_TaskDelete(AMBA_KAL_TASK_t *pTask);
UINT32 AmbaKAL_TaskSuspend(AMBA_KAL_TASK_t *pTask);
UINT32 AmbaKAL_TaskResume(AMBA_KAL_TASK_t *pTask);
UINT32 AmbaKAL_TaskAbortCondSuspend(AMBA_KAL_TASK_t *pTask);
UINT32 AmbaKAL_TaskTerminate(AMBA_KAL_TASK_t *pTask);
UINT32 AmbaKAL_TaskReset(AMBA_KAL_TASK_t *pTask);
UINT32 AmbaKAL_TaskQuery(AMBA_KAL_TASK_t *pTask, AMBA_KAL_TASK_INFO_s *pTaskInfo);
UINT32 AmbaKAL_TaskIdentify(AMBA_KAL_TASK_t ** pTaskPtr);
UINT32 AmbaKAL_TaskSleep(UINT32 NumTicks);
UINT32 AmbaKAL_TaskYield(void);
UINT32 AmbaKAL_TaskSetUserVal(AMBA_KAL_TASK_t *pTask, UINT32 UserVal);
UINT32 AmbaKAL_TaskGetUserVal(const AMBA_KAL_TASK_t *pTask, UINT32 *pUserVal);
UINT32 AmbaKAL_TaskSetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 CpuBindList);
UINT32 AmbaKAL_TaskGetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 * pCpuBindList);

UINT32 AmbaKAL_GetCpuContext(UINT32 * pContextType);
UINT32 AmbaKAL_GetSysTickCount(UINT32 * pSysTickCount);
UINT32 AmbaKAL_GetSmpCpuID(UINT32 * pCpuID);
UINT32 AmbaKAL_CriticalSectionEnter(UINT32 *pCpuStatus);
UINT32 AmbaKAL_CriticalSectionExit(UINT32 CpuStatus);

UINT32 AmbaKAL_TimerSetSmpAffinity(AMBA_KAL_TIMER_t *pTimer, UINT32 CpuBindList);
UINT32 AmbaKAL_TimerGetSmpAffinity(AMBA_KAL_TIMER_t *pTimer, UINT32 * pCpuBindList);
UINT32 AmbaKAL_TimerCreate(AMBA_KAL_TIMER_t *pTimer, char * pTimerName, AMBA_KAL_TIMER_EXPIRY_f ExpireFunc,
                           UINT32 ExpireFuncArg, UINT32 StartInterval, UINT32 PeriodicInterval, UINT32 AutoStart);
UINT32 AmbaKAL_TimerDelete(AMBA_KAL_TIMER_t *pTimer);
UINT32 AmbaKAL_TimerStart(AMBA_KAL_TIMER_t *pTimer);
UINT32 AmbaKAL_TimerStop(AMBA_KAL_TIMER_t *pTimer);
UINT32 AmbaKAL_TimerChange(AMBA_KAL_TIMER_t *pTimer, UINT32 StartInterval, UINT32 PeriodicInterval, UINT32 AutoStart);
UINT32 AmbaKAL_TimerQuery(AMBA_KAL_TIMER_t *pTimer, AMBA_KAL_TIMER_INFO_s *pTimerInfo);

UINT32 AmbaKAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName);
UINT32 AmbaKAL_EventFlagDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag);
UINT32 AmbaKAL_EventFlagGet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                            UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut);
UINT32 AmbaKAL_EventFlagSet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags);
UINT32 AmbaKAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags);
UINT32 AmbaKAL_EventFlagQuery(AMBA_KAL_EVENT_FLAG_t *pEventFlag, AMBA_KAL_EVENT_FLAG_INFO_s *pEventFlagInfo);

UINT32 AmbaKAL_MutexCreate(AMBA_KAL_MUTEX_t *pMutex, char * pMutexName);
UINT32 AmbaKAL_MutexDelete(AMBA_KAL_MUTEX_t *pMutex);
UINT32 AmbaKAL_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
UINT32 AmbaKAL_MutexGive(AMBA_KAL_MUTEX_t *pMutex);
UINT32 AmbaKAL_MutexQuery(AMBA_KAL_MUTEX_t *pMutex, AMBA_KAL_MUTEX_INFO_s *pMutexInfo);

UINT32 AmbaKAL_SemaphoreCreate(AMBA_KAL_SEMAPHORE_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount);
UINT32 AmbaKAL_SemaphoreDelete(AMBA_KAL_SEMAPHORE_t *pSemaphore);
UINT32 AmbaKAL_SemaphoreTake(AMBA_KAL_SEMAPHORE_t *pSemaphore, UINT32 TimeOut);
UINT32 AmbaKAL_SemaphoreGive(AMBA_KAL_SEMAPHORE_t *pSempahore);
UINT32 AmbaKAL_SemaphoreQuery(AMBA_KAL_SEMAPHORE_t *pSemaphore, AMBA_KAL_SEMAPHORE_INFO_s *pSemaphoreInfo);

UINT32 AmbaKAL_MsgQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, char * pMsgQueueName, UINT32 MsgSize, void * pMsgQueueBuf, UINT32 MsgQueueBufSize);
UINT32 AmbaKAL_MsgQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue);
UINT32 AmbaKAL_MsgQueueFlush(AMBA_KAL_MSG_QUEUE_t *pMsgQueue);
UINT32 AmbaKAL_MsgQueueReceive(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut);
UINT32 AmbaKAL_MsgQueueSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut);
UINT32 AmbaKAL_MsgQueueQuery(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, AMBA_KAL_MSG_QUEUE_INFO_s * pMsgQueueInfo);

#endif /* AMBA_KAL_H */

