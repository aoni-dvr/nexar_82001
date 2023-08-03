#include <test_group.h>
#include <AmbaTypes.h>
#include <AmbaKAL.h>

UINT32 kal_flag_task_create = 0;
UINT32 kal_flag_task_delete = 0;
UINT32 kal_flag_task_suspend = 0;
UINT32 kal_flag_task_resume = 0;
UINT32 kal_flag_task_sleep = 0;
UINT32 kal_flag_task_set_affinity = 0;
UINT32 kal_flag_task_identify = 0;
UINT32 kal_flag_task_query = 0;
UINT32 kal_flag_task_terminate = 0;
UINT32 kal_flag_task_state = 0;

UINT32 AmbaKAL_TaskSleep(UINT32 NumTicks)
{
    if (kal_flag_task_sleep == 0) {
        return 0;
    } else {
        return kal_flag_task_sleep;
    }
}

UINT32 AmbaKAL_TaskSetSmpAffinity(AMBA_KAL_TASK_t * pTask, UINT32 CpuBindList)
{
    if (kal_flag_task_set_affinity == 0) {
        return 0;
    } else {
        return kal_flag_task_set_affinity;
    }
}

UINT32 AmbaKAL_TaskSuspend(AMBA_KAL_TASK_t *pTask)
{
    if (kal_flag_task_suspend == 0) {
        return 0;
    } else {
        return kal_flag_task_suspend;
    }
}

UINT32 AmbaKAL_TaskResume(AMBA_KAL_TASK_t *pTask)
{
    if (kal_flag_task_resume == 0) {
        return 0;
    } else {
        return kal_flag_task_resume;
    }
}

UINT32 AmbaKAL_TaskCreate(AMBA_KAL_TASK_t *pTask, char *pTaskName, UINT32 TaskPriority,
                          AMBA_KAL_TASK_ENTRY_f EntryFunction, const void * EntryArg,
                          void *pStackBase, UINT32 StackByteSize, UINT32 AutoStart)
{
    if (kal_flag_task_create == 0) {
        return 0;
    } else {
        return kal_flag_task_create;
    }
}

UINT32 AmbaKAL_TaskDelete(AMBA_KAL_TASK_t *pTask)
{
    if (kal_flag_task_delete == 0) {
        return 0;
    } else {
        return kal_flag_task_delete;
    }
}

UINT32 AmbaKAL_TaskIdentify(AMBA_KAL_TASK_t ** pTaskPtr)
{
    if (kal_flag_task_identify == 0) {
        return 0;
    } else {
        return kal_flag_task_identify;
    }
}

UINT32 AmbaKAL_TaskQuery(AMBA_KAL_TASK_t *pTask, AMBA_KAL_TASK_INFO_s *pTaskInfo)
{
    if (kal_flag_task_query == 0) {
        pTaskInfo->TaskState = kal_flag_task_state;
        return 0;
    } else {
        return kal_flag_task_query;
    }
}

UINT32 AmbaKAL_TaskTerminate(AMBA_KAL_TASK_t *pTask)
{
    if (kal_flag_task_terminate == 0) {
        return 0;
    } else {
        return kal_flag_task_terminate;
    }
}

UINT32 kal_flag_mutex_create = 0;
UINT32 kal_flag_mutex_delete = 0;
UINT32 kal_flag_mutex_take = 0;
UINT32 kal_flag_mutex_give = 0;
const UINT32 KAL_MTX_ID = 0x1234AAAA;

UINT32 AmbaKAL_MutexCreate(AMBA_KAL_MUTEX_t *pMutex, char * pMutexName)
{
    if (kal_flag_mutex_create == 0) {
        if (pMutex != NULL) {
            pMutex->tx_mutex_id = KAL_MTX_ID;
        }
        return 0;
    } else {
        return kal_flag_mutex_create;
    }
}

UINT32 AmbaKAL_MutexDelete(AMBA_KAL_MUTEX_t *pMutex)
{
    if (kal_flag_mutex_delete == 0) {
        return 0;
    } else {
        return kal_flag_mutex_delete;
    }
}

UINT32 AmbaKAL_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    if (kal_flag_mutex_take == 0) {
        return 0;
    } else {
        return kal_flag_mutex_take;
    }
}

UINT32 AmbaKAL_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    if (kal_flag_mutex_give == 0) {
        return 0;
    } else {
        return kal_flag_mutex_give;
    }
}

UINT32 kal_flag_semaphore_create = 0;
UINT32 kal_flag_semaphore_delete = 0;
UINT32 kal_flag_semaphore_take = 0;
UINT32 kal_flag_semaphore_give = 0;
UINT32 kal_flag_semaphore_query = 0;

UINT32 AmbaKAL_SemaphoreCreate(AMBA_KAL_SEMAPHORE_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount)
{
    if (kal_flag_semaphore_create == 0) {
        return 0;
    } else {
        return kal_flag_semaphore_create;
    }
}
UINT32 AmbaKAL_SemaphoreDelete(AMBA_KAL_SEMAPHORE_t *pSemaphore)
{
    if (kal_flag_semaphore_delete == 0) {
        return 0;
    } else {
        return kal_flag_semaphore_delete;
    }
}
UINT32 AmbaKAL_SemaphoreTake(AMBA_KAL_SEMAPHORE_t *pSemaphore, UINT32 TimeOut)
{
    if (kal_flag_semaphore_take == 0) {
        return 0;
    } else {
        return kal_flag_semaphore_take;
    }
}
UINT32 AmbaKAL_SemaphoreGive(AMBA_KAL_SEMAPHORE_t *pSempahore)
{
    if (kal_flag_semaphore_give == 0) {
        return 0;
    } else {
        return kal_flag_semaphore_give;
    }
}
UINT32 AmbaKAL_SemaphoreQuery(AMBA_KAL_SEMAPHORE_t *pSemaphore, AMBA_KAL_SEMAPHORE_INFO_s *pSemaphoreInfo)
{
    if (kal_flag_semaphore_query == 0) {
        return 0;
    } else {
        return kal_flag_semaphore_query;
    }
}

UINT32 kal_flag_queue_create = 0;
UINT32 kal_flag_queue_delete = 0;
UINT32 kal_flag_queue_flush  = 0;
UINT32 kal_flag_queue_recv   = 0;
UINT32 kal_flag_queue_send   = 0;
UINT32 kal_flag_queue_query  = 0;

UINT32 AmbaKAL_MsgQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, char * pMsgQueueName, UINT32 MsgSize, void * pMsgQueueBuf, UINT32 MsgQueueBufSize)
{
    if (kal_flag_queue_create == 0) {
        return 0;
    } else {
        return kal_flag_queue_create;
    }
}
UINT32 AmbaKAL_MsgQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue)
{
    if (kal_flag_queue_delete == 0) {
        return 0;
    } else {
        return kal_flag_queue_delete;
    }
}
UINT32 AmbaKAL_MsgQueueFlush(AMBA_KAL_MSG_QUEUE_t *pMsgQueue)
{
    if (kal_flag_queue_flush == 0) {
        return 0;
    } else {
        return kal_flag_queue_flush;
    }
}
UINT32 AmbaKAL_MsgQueueReceive(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut)
{
    if (kal_flag_queue_recv == 0) {
        return 0;
    } else {
        return kal_flag_queue_recv;
    }
}
UINT32 AmbaKAL_MsgQueueSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut)
{
    if (kal_flag_queue_send == 0) {
        return 0;
    } else {
        return kal_flag_queue_send;
    }
}

UINT32 AmbaKAL_MsgQueueQuery(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, AMBA_KAL_MSG_QUEUE_INFO_s * pMsgQueueInfo)
{
    if (kal_flag_queue_query == 0) {
        return 0;
    } else {
        return kal_flag_queue_query;
    }
}

UINT32 kal_flag_event_flag_create = 0;
UINT32 kal_flag_event_flag_delete = 0;
UINT32 kal_flag_event_flag_get    = 0;
UINT32 kal_flag_event_flag_set    = 0;
UINT32 kal_flag_event_flag_clear  = 0;

UINT32 AmbaKAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName)
{
    if (kal_flag_event_flag_create == 0U) {
        return 0;
    } else {
        return kal_flag_event_flag_create;
    }
}
UINT32 AmbaKAL_EventFlagDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag)
{
    if (kal_flag_event_flag_delete == 0U) {
        return 0;
    } else {
        return kal_flag_event_flag_delete;
    }
}
UINT32 AmbaKAL_EventFlagGet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                            UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut)
{
    if (kal_flag_event_flag_get == 0U) {
        return 0;
    } else {
        return kal_flag_event_flag_get;
    }
}
UINT32 AmbaKAL_EventFlagSet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags)
{
    if (kal_flag_event_flag_set == 0U) {
        return 0;
    } else {
        return kal_flag_event_flag_set;
    }
}
UINT32 AmbaKAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags)
{
    if (kal_flag_event_flag_clear == 0U) {
        return 0;
    } else {
        return kal_flag_event_flag_clear;
    }
}