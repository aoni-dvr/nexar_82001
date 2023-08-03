#include "can_ctest.h"


void IO_UtilityRegWrite32(UINT64 Address, UINT32 Value)
{

}

UINT32 IO_UtilityRegRead32(UINT64 Address)
{
    static UINT32 value = 0;
    return (++value) % 2; // for AmbaCSL_RngIsBusy() to change done value;
}


void AmbaDelayCycles(UINT32 Delay)
{

}

void AmbaMisra_TouchUnused(void * pArg)
{

}

void AmbaMisra_TypeCast32(void * pNewType, const void * pOldType)
{
    memcpy(pNewType, pOldType, 4);

    return;
}

void AmbaMisra_TypeCast64(void * pNewType, const void * pOldType)
{
    memcpy(pNewType, pOldType, 8);

    return;
}

UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    memcpy(pDst, pSrc, num);
    return 0;
}

UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n)
{
    return 0;
}

UINT32 AmbaKAL_SemaphoreDelete(AMBA_KAL_SEMAPHORE_t *pSemaphore)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}

static UINT32 CanSemaStatus = 0;

void CTest_SetCanSemaStatus(UINT32 flag)
{
    CanSemaStatus = flag;
}

UINT32 AmbaKAL_SemaphoreTake(AMBA_KAL_SEMAPHORE_t *pSemaphore, UINT32 TimeOut)
{
    UINT32 RetVal;

    if (CanSemaStatus == 0) {
        RetVal = KAL_ERR_NONE;
    } else {
        RetVal = 0xFF;
    }

    return RetVal;
}

UINT32 AmbaKAL_SemaphoreGive(AMBA_KAL_SEMAPHORE_t *pSempahore)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}

UINT32 AmbaKAL_MsgQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}

UINT32 AmbaKAL_MsgQueueSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}

static UINT32 CanMqRxFb = 0;

void CTestHw_SetCanMqRxFb(UINT32 flag)
{
    CanMqRxFb = flag;
}

UINT32 AmbaKAL_MsgQueueReceive(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, void *pMsg, UINT32 TimeOut)
{
    UINT32 RetVal;

    if (CanMqRxFb == 0) {
        RetVal = 0xFF;
    } else {
        UINT32 *ptr =  (UINT32 *)pMsg;
        ptr[0] = 0; //CanCh
        ptr[1] = 0; //MsgNo
        RetVal = KAL_ERR_NONE;
    }


    return RetVal;
}

static UINT32 CanResFlag = 0;

void CTest_SetCanResFlag(UINT32 flag)
{
    CanResFlag = flag;
}

static UINT32 CanResIdx = 0;

UINT32 AmbaKAL_EventFlagCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag, char * pEventFlagName)
{
    UINT32 RetVal;

    if (CanResFlag == 0) {
        RetVal = KAL_ERR_NONE;
    } else {
        CanResIdx++;
        if (CanResIdx == CanResFlag) {
            RetVal = 0xFF;
        } else {
            RetVal = KAL_ERR_NONE;
        }
    }

    return RetVal;
}

UINT32 AmbaKAL_EventFlagSet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 SetFlags)
{
    UINT32 RetVal;

    if (CanResFlag == 0) {
        RetVal = KAL_ERR_NONE;
    } else {
        CanResIdx++;
        if (CanResIdx == CanResFlag) {
            RetVal = 0xFF;
        } else {
            RetVal = KAL_ERR_NONE;
        }
    }

    return RetVal;
}

UINT32 AmbaKAL_MutexCreate(AMBA_KAL_MUTEX_t *pMutex, char * pMutexName)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (CanResFlag == 0) {
        RetVal = KAL_ERR_NONE;
    } else {
        CanResIdx++;
        if (CanResIdx == CanResFlag) {
            RetVal = 0xFF;
        } else {
            RetVal = KAL_ERR_NONE;
        }
    }

    return RetVal;
}

UINT32 AmbaKAL_SemaphoreCreate(AMBA_KAL_SEMAPHORE_t *pSemaphore, char * pSemaphoreName, UINT32 InitCount)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (CanResFlag == 0) {
        RetVal = KAL_ERR_NONE;
    } else {
        CanResIdx++;
        if (CanResIdx == CanResFlag) {
            RetVal = 0xFF;
        } else {
            RetVal = KAL_ERR_NONE;
        }
    }

    return RetVal;
}

UINT32 AmbaKAL_MsgQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, char *pMsgQueueName, UINT32 MsgSize, void *pMsgQueueBuf, UINT32 MsgQueueBufSize)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (CanResFlag == 0) {
        RetVal = KAL_ERR_NONE;
    } else {
        CanResIdx++;
        if (CanResIdx == CanResFlag) {
            RetVal = 0xFF;
        } else {
            RetVal = KAL_ERR_NONE;
        }
    }

    return RetVal;
}

UINT32 AmbaKAL_EventFlagDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}

static UINT32 CanTxFlag = 0;

void CTest_SetCanTxBufStatus(UINT32 flag)
{
    CanTxFlag = flag;
}

UINT32 AmbaKAL_EventFlagGet(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 AnyOrAll,
                            UINT32 AutoClear, UINT32 *pActualFlags, UINT32 TimeOut)
{
    UINT32 RetVal = KAL_ERR_NONE;

    if (CanTxFlag == 0) {
        *pActualFlags = 0x6; // Available Message Buffers
    } else if (CanTxFlag == 1) {
        *pActualFlags = 0x0; // No available buffer
    } else {
        RetVal = 0xFF;
    }

    return RetVal;
}



UINT32 AmbaKAL_EventFlagClear(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ClearFlags)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}



UINT32 AmbaKAL_MutexDelete(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}

static UINT32 CanMutexStatus = 0;

void CTest_SetCanMutexStatus(UINT32 flag)
{
    CanMutexStatus = flag;
}

UINT32 AmbaKAL_MutexTake(AMBA_KAL_MUTEX_t *pMutex, UINT32 TimeOut)
{
    UINT32 RetVal;

    if (CanMutexStatus == 0) {
        RetVal = KAL_ERR_NONE;
    } else {
        RetVal = 0xFF;
    }

    return RetVal;
}

UINT32 AmbaKAL_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    UINT32 RetVal = KAL_ERR_NONE;

    return RetVal;
}

UINT32 AmbaCache_DataFlush(ULONG VirtAddr, ULONG Size)
{
    return 0;
}

static UINT32 CanDmaMsgCtrl = 0;

void CTestHw_SetCanCanDmaMsgCtrl(UINT32 flag)
{
    CanDmaMsgCtrl  = flag;
}

UINT32 AmbaCache_DataInvalidate(ULONG VirtAddr, ULONG Size)
{
    // Rx DMA transfer is done. Simulate data invalidate.
    ///AMBA_CAN_DMA_MESSAGE_s *ptr;
    UINT32 *Ptr = (UINT32 *)VirtAddr;
    //pDmaMessage->MsgCtrl & CAN_MBC_REG_EDL_MASK
    Ptr[1] = CanDmaMsgCtrl;

    return 0;
}

static UINT32 CanGicFb = 0;

void CTestHw_SetCanGicFb(UINT32 flag)
{
    CanGicFb = flag;
}


AMBA_INT_ISR_f AmbaRTSL_GicGetISR(UINT32 IntID)
{
    AMBA_INT_ISR_f IsrEntry;

    switch (CanGicFb) {
    case 0:
    case 1:
    case 2:
    case 3:
        IsrEntry = NULL;
        break;
    default:
        IsrEntry = (AMBA_INT_ISR_f)0x0FFFFFFF;
        break;
    }

    return IsrEntry;
}

UINT32 AmbaRTSL_GicIntConfig(UINT32 IntID, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg)
{
    UINT32 RetVal;

    switch (CanGicFb) {
    case 0:
    case 1:
    case 4:
    case 5:
        RetVal = INT_ERR_NONE;
        break;
    default:
        RetVal = 0xFF;
        break;
    }

    return RetVal;
}

UINT32 AmbaRTSL_GicIntEnable(UINT32 IntID)
{
    UINT32 RetVal = INT_ERR_NONE;

    switch (CanGicFb) {
    case 0:
    case 2:
    case 4:
    case 6:
        RetVal = INT_ERR_NONE;
        break;
    default:
        RetVal = 0xFF;
        break;
    }

    return RetVal;
}

static UINT32 CanPllFb = 0;

void CTestHw_SetCanPllFb(UINT32 flag)
{
    CanPllFb = flag;
}

UINT32 AmbaRTSL_PllGetCanClk(void)
{
    UINT32 freq;

    if (CanPllFb == 0U) {
        freq = 0;
    } else {
        freq = 24000000;
    }

    return freq;
}
