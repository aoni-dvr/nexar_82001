/**
 *  @file amba_osal.h
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
 *  @details Definitions for Ambarella KAL (Kernel Abstraction Layer) structures
 *
 */

#ifndef AMBA_OSAL_H
#define AMBA_OSAL_H

#include <threadx/tx_api.h>
#define AMBA_KAL_CPU_CORE_MASK          TX_THREAD_SMP_CORE_MASK

#define KAL_ERR_0000                    (KAL_ERR_BASE)              /* Invalid argument */
#define KAL_ERR_0001                    (KAL_ERR_BASE + 0x1U)       /* Invalid caller */
#define KAL_ERR_0002                    (KAL_ERR_BASE + 0x2U)       /* Resource was deleted while thread was suspended */
#define KAL_ERR_0007                    (KAL_ERR_BASE + 0x7U)       /* A timeout occurred */
#define KAL_ERR_000A                    (KAL_ERR_BASE + 0xAU)       /* Message queue is empty */
#define KAL_ERR_000B                    (KAL_ERR_BASE + 0xBU)       /* Message queue is full */
#define KAL_ERR_0011                    (KAL_ERR_BASE + 0x11U)      /* Thread is not in either terminated or completed state */
#define KAL_ERR_0012                    (KAL_ERR_BASE + 0x12U)      /* Thread is not in suspended by AmbaKAL_TaskSuspend or AmbaKAL_TaskCreate function */
#define KAL_ERR_0014                    (KAL_ERR_BASE + 0x14U)      /* Thread is in a terminated or completed state */
#define KAL_ERR_0017                    (KAL_ERR_BASE + 0x17U)      /* A one-shot timer that has already expired */
#define KAL_ERR_001A                    (KAL_ERR_BASE + 0x1AU)      /* Suspension was aborted by another thread, timer, or ISR */
#define KAL_ERR_001B                    (KAL_ERR_BASE + 0x1BU)      /* Thread is not in waiting state */
#define KAL_ERR_001E                    (KAL_ERR_BASE + 0x1EU)      /* Resource is not owned by caller */
#define KAL_ERR_00FF                    (KAL_ERR_BASE + 0xFFU)      /* Feature is not enabled */

#define KAL_ERR_NONE                    OK
#define KAL_ERR_ARG                     KAL_ERR_0000
#define KAL_ERR_CALLER                  KAL_ERR_0001
#define KAL_ERR_DELETED                 KAL_ERR_0002
#define KAL_ERR_TIMEOUT                 KAL_ERR_0007
#define KAL_ERR_QUEUE_EMPTY             KAL_ERR_000A
#define KAL_ERR_QUEUE_FULL              KAL_ERR_000B
#define KAL_ERR_THREAD_ACTIVE           KAL_ERR_0011
#define KAL_ERR_RESUME                  KAL_ERR_0012
#define KAL_ERR_THREAD_DORMANT          KAL_ERR_0014
#define KAL_ERR_ACTIVATE                KAL_ERR_0017
#define KAL_ERR_WAIT_ABORTED            KAL_ERR_001A
#define KAL_ERR_ABORT                   KAL_ERR_001B
#define KAL_ERR_NOT_OWNED               KAL_ERR_001E
#define KAL_ERR_NO_FEATURE              KAL_ERR_00FF

#define AMBA_KAL_SYS_TICK_MS            1U                                  /* System tick (ms) */

#define KAL_TASK_PRIORITY_HIGHEST       (0U)
#define KAL_TASK_PRIORITY_LOWEST        ((UINT32)TX_MAX_PRIORITIES - 1U)

typedef TX_THREAD                       AMBA_KAL_TASK_t;                    /* Software Task */
typedef TX_SEMAPHORE                    AMBA_KAL_SEMAPHORE_t;               /* Counting Semaphore */
typedef TX_MUTEX                        AMBA_KAL_MUTEX_t;                   /* Mutex */
typedef TX_EVENT_FLAGS_GROUP            AMBA_KAL_EVENT_FLAG_t;              /* Event Flags */
typedef TX_QUEUE                        AMBA_KAL_MSG_QUEUE_t;               /* Message Queue */
typedef TX_TIMER                        AMBA_KAL_TIMER_t;                   /* Application Timer */
typedef TX_BYTE_POOL                    AMBA_KAL_BYTE_POOL_t;               /* Memory Byte Pool */
typedef TX_BLOCK_POOL                   AMBA_KAL_BLOCK_POOL_t;              /* Memory Block Pool */

typedef void (*AMBA_KAL_TASK_STACK_ERROR_HANDLER_f)(AMBA_KAL_TASK_t * pTask);
typedef void (*AMBA_KAL_TASK_PROFILE_FUNC_f)(AMBA_KAL_TASK_t *pTask, UINT32 EntryOrExit);
typedef void *(*AMBA_KAL_TASK_ENTRY_f) (void *EntryArg);
typedef void (*AMBA_KAL_TIMER_EXPIRY_f) (UINT32 EntryArg);

#define KAL_START_NONE                  TX_DONT_START
#define KAL_START_AUTO                  TX_AUTO_START

#define KAL_WAIT_NEVER                  TX_NO_WAIT
#define KAL_WAIT_FOREVER                TX_WAIT_FOREVER

#define KAL_FLAGS_WAIT_ANY              (0U)    /* any of requested eventflags satisfied */
#define KAL_FLAGS_WAIT_ALL              (1U)    /* all the requested eventflags satisfied */
#define KAL_FLAGS_CLEAR_NONE            (0U)    /* do not clear eventflags on satisfaction */
#define KAL_FLAGS_CLEAR_AUTO            (1U)    /* auto clear eventflags on satisfaction */

/* Legacy definitions */
#define AMBA_KAL_TASK_HIGHEST_PRIORITY  KAL_TASK_PRIORITY_HIGHEST
#define AMBA_KAL_TASK_LOWEST_PRIORITY   KAL_TASK_PRIORITY_LOWEST

#define AMBA_KAL_DONT_START             KAL_START_NONE
#define AMBA_KAL_AUTO_START             KAL_START_AUTO

#define AMBA_KAL_NO_WAIT                KAL_WAIT_NEVER
#define AMBA_KAL_WAIT_FOREVER           KAL_WAIT_FOREVER

#define AMBA_KAL_FLAGS_ANY              KAL_FLAGS_WAIT_ANY
#define AMBA_KAL_FLAGS_ALL              KAL_FLAGS_WAIT_ALL
#define AMBA_KAL_FLAGS_CLEAR_NONE       KAL_FLAGS_CLEAR_NONE
#define AMBA_KAL_FLAGS_CLEAR_AUTO       KAL_FLAGS_CLEAR_AUTO

/*
 * Info structure definition
 */
typedef struct {
    char    *pTaskName;                 /* pointer to the name of the software task */
    UINT32  TaskPriority;               /* Scheduling priority of the thread */
    UINT32  TaskState;                  /* Thread execution state */
} AMBA_KAL_TASK_INFO_s;

typedef struct {
    char    *pTimerName;                /* pointer to the name of the software task */
    UINT32  ActiveState;                /* current state */
    UINT32  RemainInterval;             /* priority */
    UINT32  PeriodicInterval;           /* number of ticks for all timer expirations after the first */
} AMBA_KAL_TIMER_INFO_s;

typedef struct {
    char    *pEventFlagName;            /* pointer to an event flags group name string */
    UINT32  CurrentFlags;               /* current set flags in the event flags group */
    UINT32  NumWaitTask;                /* the number of threads currently suspended on this event flags group */
} AMBA_KAL_EVENT_FLAG_INFO_s;

typedef struct {
    char            *pMutexName;        /* pointer to a mutex name string */
    AMBA_KAL_TASK_t *pOwnerTask;        /* pointer to the owning thread's pointer */
    UINT32          NumWaitTask;        /* the number of threads currently suspended on this mutex */
} AMBA_KAL_MUTEX_INFO_s;

typedef struct {
    char    *pSemaphoreName;            /* A pointer to a semaphore name string */
    UINT32  CurrentCount;               /* current semaphore's count */
    UINT32  NumWaitTask;                /* The number of threads currently suspended on this semaphore */
} AMBA_KAL_SEMAPHORE_INFO_s;

typedef struct {
    char    *pMsgQueueName;             /* A pointer to a message queue name string */
    UINT32  NumEnqueued;                /* The number of messages currently in the queue */
    UINT32  NumAvailable;               /* The number of messages the queue currently has space for */
    UINT32  NumWaitTask;                /* The number of threads currently suspended on this message queue */
} AMBA_KAL_MSG_QUEUE_INFO_s;

#endif

