/**
 *  @file amba_osal.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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

#ifndef OSAL_LINUX_H
#define OSAL_LINUX_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/of_irq.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/iav_helper.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/clk.h>
#define AMBA_KAL_CPU_CORE_MASK          TX_THREAD_SMP_CORE_MASK

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef OSAL_IOCTL_H
#include "osal_ioctl.h"
#endif

/******************************************************************************/
/*                                  AmbaKAL.h                                                                                       */
/******************************************************************************/
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
#define KAL_ERR_POSIX                   0xFFFFFFFFU

#define AMBA_KAL_SYS_TICK_MS            1U                                  /* System tick (ms) */

/* QNX allows unprivileged threads to set thread priority as 1~63(by default) */
/* Ambarella modifies this setting to 1~127, 128 is reserved to ISR */
#define KAL_TASK_PRIORITY_HIGHEST       (1U)
#define KAL_TASK_PRIORITY_LOWEST        (127U)

#ifdef CONFIG_SMP_CORE_NUM
#define SMP_CORE_MASK                   (1 << CONFIG_SMP_CORE_NUM - 1)
#else
#define SMP_CORE_MASK                   0x1U
#endif
typedef struct task_struct              AMBA_KAL_TASK_t;                    /* Software Task */
typedef struct semaphore                AMBA_KAL_SEMAPHORE_t;               /* Counting Semaphore */
typedef struct mutex                    AMBA_KAL_MUTEX_t;                   /* Mutex */
typedef struct {
    atomic_t                flag;
    struct wait_queue_head  wait_queue;
} AMBA_KAL_EVENT_FLAG_t;

typedef INT32 (*AMBA_KAL_TASK_ENTRY_f) (void *EntryArg);

#define KAL_WAIT_NEVER                    0x0U
#define KAL_WAIT_FOREVER                  0xFFFFFFFFU

#define KAL_FLAGS_WAIT_ANY              (0U)    /* any of requested eventflags satisfied */
#define KAL_FLAGS_WAIT_ALL              (1U)    /* all the requested eventflags satisfied */
#define KAL_FLAGS_CLEAR_NONE            (0U)    /* do not clear eventflags on satisfaction */
#define KAL_FLAGS_CLEAR_AUTO            (1U)    /* auto clear eventflags on satisfaction */
//
#define AMBA_KAL_TASK_HIGHEST_PRIORITY  KAL_TASK_PRIORITY_HIGHEST
#define AMBA_KAL_TASK_LOWEST_PRIORITY   KAL_TASK_PRIORITY_LOWEST
//
#define KAL_START_NONE                  0x0U
#define KAL_START_AUTO                  0x1U

#define AMBA_KAL_DONT_START             KAL_START_NONE
#define AMBA_KAL_AUTO_START             KAL_START_AUTO
//
#define AMBA_KAL_NO_WAIT                KAL_WAIT_NEVER
#define AMBA_KAL_WAIT_FOREVER           KAL_WAIT_FOREVER

#define AMBA_KAL_FLAGS_ANY              KAL_FLAGS_WAIT_ANY
#define AMBA_KAL_FLAGS_ALL              KAL_FLAGS_WAIT_ALL
#define AMBA_KAL_FLAGS_CLEAR_NONE       KAL_FLAGS_CLEAR_NONE
#define AMBA_KAL_FLAGS_CLEAR_AUTO       KAL_FLAGS_CLEAR_AUTO

#define AMBA_SYS_CLK_VISION             0x4U
#define AMBA_SYS_CLK_AUD_0              0x10U

#define AmbaMisra_TouchUnused(x) (void)(x)

UINT32 OSAL_TaskCreate(AMBA_KAL_TASK_t **ppTask, char *pTaskName, UINT32 TaskPriority,
                       AMBA_KAL_TASK_ENTRY_f EntryFunction, void* EntryArg,
                       void *pStackBase, UINT32 StackByteSize, UINT32 AutoStart);
UINT32 OSAL_TaskSetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 CpuBindList);
UINT32 OSAL_TaskDelete(AMBA_KAL_TASK_t *pTask);
UINT32 OSAL_TaskResume(AMBA_KAL_TASK_t *pTask);
UINT32 OSAL_TaskSleep(UINT32 NumTicks);

UINT32 OSAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName);
UINT32 OSAL_EventFlagDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag);
UINT32 OSAL_EventFlagGet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                         UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut);
UINT32 OSAL_EventFlagSet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags);
UINT32 OSAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags);

UINT32 OSAL_MutexCreate(AMBA_KAL_MUTEX_t *pMutex, char * pMutexName);
UINT32 OSAL_MutexDelete(AMBA_KAL_MUTEX_t *pMutex);
UINT32 OSAL_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
UINT32 OSAL_MutexGive(AMBA_KAL_MUTEX_t *pMutex);

UINT32 OSAL_SemaphoreCreate(AMBA_KAL_SEMAPHORE_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount);
UINT32 OSAL_SemaphoreDelete(AMBA_KAL_SEMAPHORE_t *pSemaphore);
UINT32 OSAL_SemaphoreTake(AMBA_KAL_SEMAPHORE_t *pSemaphore, UINT32 TimeOut);
UINT32 OSAL_SemaphoreGive(AMBA_KAL_SEMAPHORE_t *pSempahore);

/******************************************************************************/
/*                                  AmbaMisraFix.h                                                                                   */
/******************************************************************************/
void OSAL_TypeCast(void * pNewType, const void * pOldType);

/******************************************************************************/
/*                                  AmbaPrint.h                                                                                       */
/******************************************************************************/
/* PRINT error values */
#define PRINT_ERR_START             0x91990000U
#define PRINT_ERR_SUCCESS           (0U)
#define PRINT_ERR_INVALID_PARAMETER (PRINT_ERR_START + 0x01U)
#define PRINT_ERR_RESOURCE_CREATE   (PRINT_ERR_START + 0x02U)
#define PRINT_ERR_MUTEX_LOCK_FAIL   (PRINT_ERR_START + 0x03U)
#define PRINT_ERR_MUTEX_UNLOCK_FAIL (PRINT_ERR_START + 0x04U)

// new APIs
#define PRINT_COND_YES   (1U)
#define PRINT_COND_NO    (0U)

typedef enum {
    MESSAGE_TO_LOG_BUFFER = 0,
    MESSAGE_TO_PRINT_BUFFER,
    MESSAGE_TO_BOTH
} MESSAGE_DESTINATION_e;

#define ALLOW_LIST_ENTRY_NUMBER (65536U/32U)

void OSAL_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void OSAL_PrintInt5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void OSAL_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);

UINT32 OSAL_ModuleSetAllowList(UINT16 ModuleID, UINT8 Enable);
void OSAL_ModulePrintStr5(UINT16 ModuleID, const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);
void OSAL_ModulePrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void OSAL_ModulePrintInt5(UINT16 ModuleID, const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);

/******************************************************************************/
/*                                  AmbaWrap.h                                                                                      */
/******************************************************************************/
/** Invalid argument or parameters. */
#define LIBWRAP_ERR_0000    (LIBWRAP_ERR_BASE)
#define LIBWRAP_ERR_INVAL   LIBWRAP_ERR_0000    //< Invalid argument */

UINT32 OSAL_memcpy(void *pDst, const void *pSrc, SIZE_t num);
UINT32 OSAL_memset(void *ptr, INT32 v, SIZE_t n);
UINT32 OSAL_memcmp(const void *p1, const void *p2, SIZE_t n, void *pV);

/******************************************************************************/
/*                                  AmbaINT.h                                                                                   */
/******************************************************************************/
#define INT_ERR_0000        (INT_ERR_BASE)              /* Invalid argument */
#define INT_ERR_0001        (INT_ERR_BASE + 0x1U)       /* Unable to do concurrency protection */
#define INT_ERR_0002        (INT_ERR_BASE + 0x2U)       /* Target ID has been enabled. Need to do disable first */
#define INT_ERR_00FF        (INT_ERR_BASE + 0XFFU)      /* Unexpected error */

/* INT error values */
#define INT_ERR_NONE        0x0U
#define INT_ERR_ARG         INT_ERR_0000
#define INT_ERR_MUTEX       INT_ERR_0001
#define INT_ERR_ID_CONFLIC  INT_ERR_0002
#define INT_ERR_UNEXPECTED  INT_ERR_00FF

#define INT_TRIG_HIGH_LEVEL             (0U)          /* High level sensitive */
#define INT_TRIG_RISING_EDGE            (1U)          /* Rising-edge-triggered */

/* Legacy definitions */
#define AMBA_INT_HIGH_LEVEL_TRIGGER     INT_TRIG_HIGH_LEVEL
#define AMBA_INT_RISING_EDGE_TRIGGER    INT_TRIG_RISING_EDGE

#define INT_TYPE_IRQ                    (0U)          /* IRQ Interrupt */
#define INT_TYPE_FIQ                    (1U)          /* FIQ Interrupt */

#define AMBA_INT_IRQ                    INT_TYPE_IRQ
#define AMBA_INT_FIQ                    INT_TYPE_FIQ

typedef irqreturn_t (*AMBA_INT_ISR_f)(INT32 IntID, void * UserArg);

typedef struct {
    UINT32  TriggerType;                /* Sensitivity type */
    UINT32  IrqType;                    /* Interrupt type: IRQ or FIQ */
    UINT8   CpuTargets;                 /* Target cores */
} AMBA_INT_CONFIG_s;

UINT32 OSAL_INT_Config(UINT32 Irq, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg);
UINT32 OSAL_INT_Enable(UINT32 Irq);
UINT32 OSAL_INT_Disable(UINT32 Irq);

/******************************************************************************/
/*                                  AmbaCache.h                                                                                   */
/******************************************************************************/
UINT32 OSAL_Cache_DataClean(ULONG VirtAddr, ULONG Size);
UINT32 OSAL_Cache_DataInvalidate(ULONG VirtAddr, ULONG Size);

/******************************************************************************/
/*                                  AmbaUtility.h                                                                                   */
/******************************************************************************/
#define UTIL_MAX_INT_STR_LEN        (22U)
#define UTIL_MAX_FLOAT_STR_LEN      (50U)
#define UTIL_MAX_STR_TOKEN_NUMBER   (32U)
#define UTIL_MAX_STR_TOKEN_LENGTH   (128U)

#define OSAL_TouchUnused(x)

// string print functions
UINT32 OSAL_StringPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs);
UINT32 OSAL_StringPrintInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const INT32 *pArgs);
UINT32 OSAL_StringPrintStr(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const char * const pArgs[]);

// string append functions
void OSAL_StringAppend(char *pBuffer, UINT32 BufferSize, const char *pSource);
void OSAL_StringAppendInt32(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix);
void OSAL_StringAppendUInt32(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
void OSAL_StringAppendFloat(char *pBuffer, UINT32 BufferSize, FLOAT Value, UINT32 AfterPoint);
void OSAL_StringAppendDouble(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 AfterPoint);

// primitive type to string functions
UINT32 OSAL_FloatToStr(char *pBuffer,  UINT32 BufferSize, FLOAT Value, UINT32 Afterpoint);
UINT32 OSAL_DoubleToStr(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint);
UINT32 OSAL_Int32ToStr(char *pBuffer,  UINT32 BufferSize, INT32 Value,  UINT32 Radix);
UINT32 OSAL_Int64ToStr(char *pBuffer,  UINT32 BufferSize, INT64 Value,  UINT32 Radix);
UINT32 OSAL_UInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
UINT32 OSAL_UInt64ToStr(char *pBuffer, UINT32 BufferSize, UINT64 Value, UINT32 Radix);

// misc string functions
SIZE_t OSAL_StringLength(const char *pString);
void   OSAL_StringCopy(char *pDest, SIZE_t DestSize, const char *pSource);
INT32  OSAL_StringCompare(const char *pString1, const char *pString2, SIZE_t Size);
UINT32 OSAL_StringToken(const char *pString, char Delimiter, char Args[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH], UINT32 *ArgCount);
UINT32 OSAL_StringToUInt32(const char *pString, UINT32 *pValue);
UINT32 OSAL_StringToUInt64(const char *pString, UINT64 *pValue);

UINT32 OSAL_GetClkFreq(UINT32 ClkID, UINT32 * pFreq);

#endif