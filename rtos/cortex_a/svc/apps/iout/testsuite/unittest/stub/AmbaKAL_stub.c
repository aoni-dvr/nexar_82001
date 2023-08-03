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
#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaKAL.h"

static UINT32 RetVal_TaskCreate         = KAL_ERR_NONE;
static UINT32 RetVal_TaskResume         = KAL_ERR_NONE;
static UINT32 RetVal_TaskSetSmpAffinity = KAL_ERR_NONE;
static UINT32 RetVal_TaskSleep          = KAL_ERR_NONE;
static UINT32 RetVal_TaskYield          = KAL_ERR_NONE;
static UINT32 RetVal_MutexCreate        = KAL_ERR_NONE;
static UINT32 RetVal_MutexDelete        = KAL_ERR_NONE;
static UINT32 RetVal_MutexTake          = KAL_ERR_NONE;
static UINT32 RetVal_MutexGive          = KAL_ERR_NONE;
static UINT32 RetVal_EventFlagCreate    = KAL_ERR_NONE;
static UINT32 RetVal_EventFlagSet       = KAL_ERR_NONE;
static UINT32 RetVal_EventFlagGet       = KAL_ERR_NONE;
static UINT32 RetVal_EventFlagClear     = KAL_ERR_NONE;
static UINT32 RetVal_SemaphoreCreate    = KAL_ERR_NONE;
static UINT32 RetVal_SemaphoreTake      = KAL_ERR_NONE;
static UINT32 RetVal_SemaphoreGive      = KAL_ERR_NONE;
static UINT32 RetVal_SemaphoreDelete    = KAL_ERR_NONE;
static UINT32 RetVal_GetSysTickCount    = KAL_ERR_NONE;
static UINT32 RetVal_TimerCreate        = KAL_ERR_NONE;
static UINT32 RetVal_TimerChange        = KAL_ERR_NONE;
static UINT32 RetVal_TimerStop          = KAL_ERR_NONE;

void (*fpCB_TaskSleep)(void);
void (*fpCB_MutexCreate)(void);
void (*fpCB_EventFlagGet)(void);
UINT32 (*fpCB_SemaphoreCreate)(void);
void (*fpCB_SemaphoreTake)(void);

void Set_RetVal_TaskCreate(UINT32 RetVal)
{
    RetVal_TaskCreate = RetVal;
}
void Set_RetVal_TaskResume(UINT32 RetVal)
{
    RetVal_TaskResume = RetVal;
}
void Set_RetVal_TaskSetSmpAffinity(UINT32 RetVal)
{
    RetVal_TaskSetSmpAffinity = RetVal;
}
void Set_RetVal_TaskSleep(UINT32 RetVal)
{
    RetVal_TaskSleep = RetVal;
}
void Set_RetVal_TaskYield(UINT32 RetVal)
{
    RetVal_TaskYield = RetVal;
}
void Set_RetVal_MutexCreate(UINT32 RetVal)
{
    RetVal_MutexCreate = RetVal;
}
void Set_RetVal_MutexDelete(UINT32 RetVal)
{
    RetVal_MutexDelete = RetVal;
}
void Set_RetVal_MutexTake(UINT32 RetVal)
{
    RetVal_MutexTake = RetVal;
}
void Set_RetVal_MutexGive(UINT32 RetVal)
{
    RetVal_MutexGive = RetVal;
}
void Set_RetVal_EventFlagCreate(UINT32 RetVal)
{
    RetVal_EventFlagCreate = RetVal;
}
void Set_RetVal_EventFlagSet(UINT32 RetVal)
{
    RetVal_EventFlagSet = RetVal;
}
void Set_RetVal_EventFlagGet(UINT32 RetVal)
{
    RetVal_EventFlagGet = RetVal;
}
void Set_RetVal_EventFlagClear(UINT32 RetVal)
{
    RetVal_EventFlagClear = RetVal;
}
void Set_RetVal_SemaphoreCreate(UINT32 RetVal)
{
    RetVal_SemaphoreCreate = RetVal;
}
void Set_RetVal_SemaphoreTake(UINT32 RetVal)
{
    RetVal_SemaphoreTake = RetVal;
}
void Set_RetVal_SemaphoreGive(UINT32 RetVal)
{
    RetVal_SemaphoreGive = RetVal;
}
void Set_RetVal_SemaphoreDelete(UINT32 RetVal)
{
    RetVal_SemaphoreDelete = RetVal;
}
void Set_RetVal_GetSysTickCount(UINT32 RetVal)
{
    RetVal_GetSysTickCount = RetVal;
}
void Set_RetVal_TimerCreate(UINT32 RetVal)
{
    RetVal_TimerCreate = RetVal;
}
void Set_RetVal_TimerChange(UINT32 RetVal)
{
    RetVal_TimerChange = RetVal;
}
void Set_RetVal_TimerStop(UINT32 RetVal)
{
    RetVal_TimerStop = RetVal;
}

void AmbaDelayCycles(UINT32 Val)
{

}

UINT32 AmbaKAL_TaskCreate(AMBA_KAL_TASK_t *pTask, char *pTaskName, UINT32 TaskPriority,
                          AMBA_KAL_TASK_ENTRY_f EntryFunction, const void * EntryArg,
                          void *pStackBase, UINT32 StackByteSize, UINT32 AutoStart)
{
    return RetVal_TaskCreate;
}

UINT32 AmbaKAL_TaskResume(AMBA_KAL_TASK_t *pTask)
{
    return RetVal_TaskResume;
}

UINT32 AmbaKAL_TaskSetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 CpuBindList)
{
    return RetVal_TaskSetSmpAffinity;
}

UINT32 AmbaKAL_TaskSleep(UINT32 NumTicks)
{
    UINT32 Ret = RetVal_TaskSleep;
    if (fpCB_TaskSleep != NULL) {
        fpCB_TaskSleep();
    }
    return Ret;
}

UINT32 AmbaKAL_TaskYield(void)
{
    return RetVal_TaskYield;
}

UINT32 AmbaKAL_MutexCreate(AMBA_KAL_MUTEX_t *pMutex, char * pMutexName)
{
    UINT32 Ret = RetVal_MutexCreate;
    if (fpCB_MutexCreate != NULL) {
        fpCB_MutexCreate();
    }
    return Ret;
}

UINT32 AmbaKAL_MutexDelete(AMBA_KAL_MUTEX_t *pMutex)
{
    return RetVal_MutexDelete;
}

UINT32 AmbaKAL_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    return RetVal_MutexTake;
}

UINT32 AmbaKAL_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    return RetVal_MutexGive;
}

UINT32 AmbaKAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName)
{
    return RetVal_EventFlagCreate;
}

UINT32 AmbaKAL_EventFlagSet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags)
{
    return RetVal_EventFlagSet;
}

UINT32 AmbaKAL_EventFlagGet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                            UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut)
{
    UINT32 Ret = RetVal_EventFlagGet;
    if (fpCB_EventFlagGet != NULL) {
        fpCB_EventFlagGet();
    }
    return Ret;
}

UINT32 AmbaKAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags)
{
    return RetVal_EventFlagClear;
}

UINT32 AmbaKAL_SemaphoreCreate(AMBA_KAL_SEMAPHORE_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount)
{
    UINT32 Ret = RetVal_SemaphoreCreate;
    if (fpCB_SemaphoreCreate != NULL) {
        Ret = fpCB_SemaphoreCreate();
    }
    return Ret;
}

UINT32 AmbaKAL_SemaphoreTake(AMBA_KAL_SEMAPHORE_t *pSemaphore, UINT32 TimeOut)
{
    UINT32 Ret = RetVal_SemaphoreTake;
    if (fpCB_SemaphoreTake != NULL) {
        fpCB_SemaphoreTake();
    }
    return Ret;
}

UINT32 AmbaKAL_SemaphoreGive(AMBA_KAL_SEMAPHORE_t *pSempahore)
{
    return RetVal_SemaphoreGive;
}

UINT32 AmbaKAL_SemaphoreDelete(AMBA_KAL_SEMAPHORE_t *pSemaphore)
{
    return RetVal_SemaphoreDelete;
}

UINT32 AmbaKAL_GetSysTickCount(UINT32 * pSysTickCount)
{
    return RetVal_GetSysTickCount;
}

UINT32 AmbaKAL_TimerCreate(AMBA_KAL_TIMER_t *pTimer, char * pTimerName, AMBA_KAL_TIMER_EXPIRY_f ExpireFunc,
                           UINT32 ExpireFuncArg, UINT32 StartInterval, UINT32 PeriodicInterval, UINT32 AutoStart)
{
    return RetVal_TimerCreate;
}

UINT32 AmbaKAL_TimerChange(AMBA_KAL_TIMER_t *pTimer, UINT32 StartInterval, UINT32 PeriodicInterval, UINT32 AutoStart)
{
    return RetVal_TimerChange;
}

UINT32 AmbaKAL_TimerStop(AMBA_KAL_TIMER_t *pTimer)
{
    return RetVal_TimerStop;
}

void AmbaAssert(void)
{
}

