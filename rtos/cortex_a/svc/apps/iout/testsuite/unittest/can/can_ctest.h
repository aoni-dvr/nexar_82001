#include <threadx/amba_osal.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "AmbaReg_CAN.h"
#include "AmbaCAN.h"
#include "AmbaINT_Def.h"

typedef struct {
    UINT32              ChannelId;
    AMBA_CAN_CONFIG_s   Config;
} AMBA_SHELL_CAN_CTRL_s;

extern AMBA_SHELL_CAN_CTRL_s IoDiag_CanCtrl;

void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

INT32 can_ctest_enable_main(void);
INT32 can_ctest_getinfo_main(void);
INT32 can_ctest_read_main(void);
INT32 can_ctest_write_main(void);
INT32 can_ctest_writefd_main(void);
INT32 can_ctest_disable_main(void);
INT32 can_ctest_isr_main(void);
INT32 can_ctest_getsafestate_main(void);
INT32 can_ctest_setsafestate_main(void);

extern void IO_UtilityRegWrite32(UINT64 Address, UINT32 Value);
extern UINT32 IO_UtilityRegRead32(UINT64 Address);
extern void AmbaDelayCycles(UINT32 Delay);
void AmbaMisra_TypeCast32(void * pNewType, const void * pOldType);
void AmbaMisra_TouchUnused(void * pArg);

UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num);
UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n);

UINT32 AmbaKAL_SemaphoreCreate(AMBA_KAL_SEMAPHORE_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount);
UINT32 AmbaKAL_SemaphoreDelete(AMBA_KAL_SEMAPHORE_t *pSemaphore);
UINT32 AmbaKAL_SemaphoreTake(AMBA_KAL_SEMAPHORE_t *pSemaphore, UINT32 TimeOut);
UINT32 AmbaKAL_SemaphoreGive(AMBA_KAL_SEMAPHORE_t *pSempahore);
UINT32 AmbaKAL_MsgQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, char *pMsgQueueName, UINT32 MsgSize, void *pMsgQueueBuf, UINT32 MsgQueueBufSize);
UINT32 AmbaKAL_MsgQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue);
UINT32 AmbaKAL_MsgQueueSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut);
UINT32 AmbaKAL_MsgQueueReceive(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut);
UINT32 AmbaKAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName);
UINT32 AmbaKAL_EventFlagDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag);
UINT32 AmbaKAL_EventFlagGet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                            UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut);
UINT32 AmbaKAL_EventFlagSet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags);
UINT32 AmbaKAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags);
UINT32 AmbaKAL_MutexCreate(AMBA_KAL_MUTEX_t *pMutex, char * pMutexName);
UINT32 AmbaKAL_MutexDelete(AMBA_KAL_MUTEX_t *pMutex);
UINT32 AmbaKAL_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut);
UINT32 AmbaKAL_MutexGive(AMBA_KAL_MUTEX_t *pMutex);
UINT32 AmbaKAL_TaskCreate(AMBA_KAL_TASK_t *pTask, char *pTaskName, UINT32 TaskPriority,
                          AMBA_KAL_TASK_ENTRY_f EntryFunction, const void * EntryArg,
                          void *pStackBase, UINT32 StackByteSize, UINT32 AutoStart);
UINT32 AmbaKAL_TaskDelete(AMBA_KAL_TASK_t *pTask);
UINT32 AmbaKAL_TaskTerminate(AMBA_KAL_TASK_t *pTask);

UINT32 AmbaCache_DataFlush(ULONG VirtAddr, ULONG Size);
UINT32 AmbaCache_DataInvalidate(ULONG VirtAddr, ULONG Size);

AMBA_INT_ISR_f AmbaRTSL_GicGetISR(UINT32 IntID);
UINT32 AmbaRTSL_GicIntConfig(UINT32 IntID, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg);
UINT32 AmbaRTSL_GicIntEnable(UINT32 IntID);
UINT32 AmbaRTSL_PllGetCanClk(void);

extern can_ctest_isr(UINT32 IntID);
