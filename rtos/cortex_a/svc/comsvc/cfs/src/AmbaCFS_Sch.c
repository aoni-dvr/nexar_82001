/**
 * @file AmbaCFS_Sch.c
 *
 * Copyright (c) 2019 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "AmbaCFS_Util.h"
#include "AmbaCFS_Sch.h"
#include "AmbaCFS_Cache.h"
#include <AmbaCache.h>
#include <AmbaPrint.h>
#include <AmbaGDMA.h>

#define AMBA_CFS_SCH_DEBUG      (0)     /**< Enable debug log */

/*
 *   Constants and Global variables definition
 */
#if 0
#define AMBA_CFS_SCH_SLOW_TEST           /**< Enable slow test */
#endif

#define AMBA_CFS_FLAG_CMD_CREATE         (0x00000100U)       /**< Event flag for command create */
#define AMBA_CFS_FLAG_BANK_RELEASE       (0x01000000U)       /**< Event flag for bank release (no need to shift) */
#define AMBA_CFS_DMA_NC_SIZE_THRESHOLD   (512U)              /**< DMA copy threshold for non-cached source (use DMA if data size is larger than threshold, otherwise use memcpy) */
#define AMBA_CFS_DMA_TIMEOUT             (10000U)            /**< DMA timeout (in ms) */

/**
 * Format of commands
 */
typedef struct AMBA_CFS_SCH_CMD_INFO_s_ {
    AMBA_CFS_STREAM_s *Stream;                              /**< Stream address of the command */
    AMBA_CFS_SCH_BANK_INFO_s *Bank;                         /**< Bank Data */
    struct AMBA_CFS_SCH_CMD_INFO_s_ *Next;                  /**< Point to the next command */
    UINT8 StreamId;                                         /**< Stream ID */
    UINT32 Event;                                           /**< CFS event of the command (ex: AMBA_CFS_EVENT_ASYNC_WRITE) */
    UINT64 Length;                                          /**< The stream length once the command is created */
} AMBA_CFS_SCH_CMD_INFO_s;

/**
 * Task structure of schedulers
 */
typedef struct {
    AMBA_CFS_SCH_CMD_INFO_s *Cmd;                           /**< The current working command of the task (Only one command). It came from CmdList. */
    AMBA_CFS_SCH_CMD_INFO_s *CmdList;                       /**< To link commands assigned to the task (It's a list of commands waiting to be processed) */
} AMBA_CFS_SCH_TASK_INFO_s;

/**
 * Global variables
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;                                         /**< Mutex of scheduler module (protect BankAvail, CmdAvail, Stream->AsyncData, Task->CmdList) */
    AMBA_KAL_EVENT_FLAG_t SchFlg;                                   /**< Event flag of scheduler module */
    AMBA_KAL_EVENT_FLAG_t AwdFlg;                                   /**< Async write done*/
    AMBA_KAL_EVENT_FLAG_t ArdFlg;                                   /**< Async read done*/
    AMBA_CFS_SCH_TASK_INFO_s Task;                                  /**< Scheduler task info */
    UINT32 BankSize;                                                /**< Storing the size of a bank */
    AMBA_CFS_SCH_BANK_INFO_s *BankAvail;                            /**< To link available bank data */
    AMBA_CFS_SCH_CMD_INFO_s *CmdAvail;                              /**< To link available command records */
    AMBA_CFS_STREAM_ASYNC_DATA_s AsyncData[AMBA_CFS_MAX_STREAM];    /**< Stream's asynchronous mode related data */
    UINT8 CacheEnable;                                              /**< To enable the cache function of CFS */
    AMBA_CFS_CHECK_CACHED_f CheckCached;                            /**< The callback function to check whether the buffer is cached or not */
    AMBA_CFS_CACHE_CLEAN_f CacheClean;                              /**< The callback function to clean data cache */
    AMBA_CFS_CACHE_INVALIDATE_f CacheInvalidate;                    /**< The callback function to invalidate data cache */
    AMBA_CFS_VIRT_TO_PHYS_f VirtToPhys;                             /**< The callback function to convert virtual memory address to physical address */
} AMBA_CFS_SCH_s;

/**
 * GDMA copy info
 */
typedef struct {
    UINT32 DataSize;            /**< Data size */
    UINT8 *Source;              /**< Source */
    UINT8 *Destination;         /**< Destination */
    UINT32 DmaDataSize;         /**< DMA Data size (Should be aligned) */
    UINT8 *DmaSource;           /**< DMA Source (Should be aligned) */
    UINT8 *DmaDestination;      /**< DMA Destination (Should be aligned) */
} AMBA_CFS_COPY_INFO_s;

static AMBA_CFS_SCH_s g_AmbaCFSSch GNU_SECTION_NOZEROINIT;   /**< CFS scheduler core */

/*
 *   Prototypes
 */
static UINT32 AmbaCFS_SchCopySetSource(AMBA_CFS_COPY_INFO_s *CopyInfo, UINT8 *Src);
static UINT32 AmbaCFS_SchCopySetDestination(AMBA_CFS_COPY_INFO_s *CopyInfo, UINT8 *Dst, UINT32 Size, const AMBA_CFS_STREAM_s *Stream);
static UINT32 AmbaCFS_SchCopyMemory(const AMBA_CFS_COPY_INFO_s *CopyInfo, const AMBA_CFS_STREAM_s *Stream);

/*
 * Internal utility functions
 */

/**
 *  Lock scheduler
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchLock(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexTake(&g_AmbaCFSSch.Mutex, AMBA_CFS_TIMEOUT_MUTEX));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Unlock scheduler
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchUnlock(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexGive(&g_AmbaCFSSch.Mutex));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Give a scheduler event
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Event               The event
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchGiveEvent(UINT32 Event)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_EventFlagSet(&g_AmbaCFSSch.SchFlg, Event));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagSet fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Clear a scheduler event
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Event               The event
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchClearEvent(UINT32 Event)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_EventFlagClear(&g_AmbaCFSSch.SchFlg, Event));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagClear fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Take a scheduler event
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Event               The event
 *  @param [in] Timeout             The timeout
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchTakeEvent(UINT32 Event, UINT32 Timeout)
{
    UINT32 Ret;
    UINT32 Flag;
    Ret = CFS_K2C(AmbaKAL_EventFlagGet(&g_AmbaCFSSch.SchFlg, Event, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &Flag, Timeout));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagGet fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Give async write done event
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchGiveAWriteDone(UINT8 StreamId)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_EventFlagSet(&g_AmbaCFSSch.AwdFlg, (UINT32) 0x00000001U << StreamId));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagSet fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Take async write done event
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchTakeAWriteDone(UINT8 StreamId)
{
    UINT32 Ret;
    UINT32 Flag;
    Ret = CFS_K2C(AmbaKAL_EventFlagGet(&g_AmbaCFSSch.AwdFlg, (UINT32) 0x00000001U << StreamId, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &Flag, AMBA_CFS_TIMEOUT_EVTFLAG));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagGet fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Clear async write done event
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchClearAWriteDone(UINT8 StreamId)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_EventFlagClear(&g_AmbaCFSSch.AwdFlg, (UINT32) 0x00000001U << StreamId));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagClear fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Give async write done event
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchGiveAReadDone(UINT8 StreamId)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_EventFlagSet(&g_AmbaCFSSch.ArdFlg, (UINT32) 0x00000001U << StreamId));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagSet fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Take async read done event
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchTakeAReadDone(UINT8 StreamId)
{
    UINT32 Ret;
    UINT32 Flag;
    Ret = CFS_K2C(AmbaKAL_EventFlagGet(&g_AmbaCFSSch.ArdFlg, (UINT32) 0x00000001U << StreamId, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &Flag, AMBA_CFS_TIMEOUT_EVTFLAG));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagGet fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Clear async read done event
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchClearAReadDone(UINT8 StreamId)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_EventFlagClear(&g_AmbaCFSSch.ArdFlg, (UINT32) 0x00000001U << StreamId));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagClear fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Cache data clean
 *  @param [in] Buffer              The buffer address
 *  @param [in] Size                The data size. Should be cache line aligned.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchDataClean(UINT8 *Buffer, UINT32 Size)
{
    UINT32 Ret = CFS_OK;
    ULONG BufferUL;
    UINT32 IsCached;
    AmbaMisra_TypeCast(&BufferUL, &Buffer);
    IsCached = g_AmbaCFSSch.CheckCached(Buffer, 1U); /* Set Size = 1 to check the address specifically */
    if (IsCached != 0U) {
        Ret = g_AmbaCFSSch.CacheClean(Buffer, Size);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CacheClean fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Cache data invalidate
 *  @param [in] Buffer              The buffer address
 *  @param [in] Size                The data size. Should be cache line aligned.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchDataInvalidate(UINT8 *Buffer, UINT32 Size)
{
    UINT32 Ret = CFS_OK;
    ULONG BufferUL;
    UINT32 IsCached;
    AmbaMisra_TypeCast(&BufferUL, &Buffer);
    IsCached = g_AmbaCFSSch.CheckCached(Buffer, 1U); /* Set Size = 1 to check the address specifically */
    if (IsCached != 0U) {
        Ret = g_AmbaCFSSch.CacheInvalidate(Buffer, Size);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CacheInvalidate fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Convert virtual memory address to physical address
 *  @param [in] VirtAddr            The virtual address
 *  @param [in] PhysAddr            The physical address
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_SchVirtToPhys(UINT8 *VirtAddr, UINT8 **PhysAddr)
{
    UINT32 Ret;
    void *Phys = NULL;
    Ret = g_AmbaCFSSch.VirtToPhys(VirtAddr, &Phys);
    if (Ret == OK) {
        UINT8 *PhysAddrU8 = NULL;
        AmbaMisra_TypeCast(&PhysAddrU8, &Phys);
        *PhysAddr = PhysAddrU8;
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] VirtToPhys fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Create a bank pool for the stream.
 *  Move some banks from g_AmbaCFSSch.BankAvail to AsyncData->BankPool.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @param [in] BankBuffer          The bank buffer
 *  @param [in] BankBufferSize      The size of bank buffer
 *  @param [in] BankSize            The size of a bank
 *  @return 0 - OK, others - Error
 */
static UINT32 CreateBankPool(UINT8 StreamId, UINT8 *BankBuffer, UINT32 BankBufferSize, UINT32 BankSize)
{
    /* must be called with sch's mutex locked */
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    UINT32 Ret = CFS_OK;
    if (AsyncData->MaxNumBank != 0U) {
        if ((BankBuffer != NULL) && ((BankBufferSize / BankSize) == AsyncData->MaxNumBank)) {
            AMBA_CFS_SCH_BANK_INFO_s *PrevBank = NULL;
            AMBA_CFS_SCH_BANK_INFO_s *Bank = g_AmbaCFSSch.BankAvail;
            UINT8 *BankBufferAddr = BankBuffer;
            /* Request "MaxNumBank" banks from BankAvail */
            for (UINT32 i = 0U; i < AsyncData->MaxNumBank; ++i) {
                if (Bank == NULL) {
                    /* In AmbaCFS_SchInit, BankAmount should be sufficient that this case should never happen. */
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CreateBankPool: [ERROR] No available bank", 0, 0, 0, 0, 0);
                    Ret = CFS_ERR_API;
                    break;
                }
                Bank->Buffer = BankBufferAddr;
                /* BankBufferAddr += BankSize; */
                BankBufferAddr = &BankBufferAddr[BankSize];
                PrevBank = Bank;
                Bank = Bank->Next;
            }

            if (Ret == CFS_OK) {
                /* Move the first "MaxNumBank" banks from BankAvail to AsyncData->BankPool */
                AsyncData->BankPool = g_AmbaCFSSch.BankAvail;
                g_AmbaCFSSch.BankAvail = Bank;
                /* PrevBank is the last bank in AsyncData->BankPool */
                /*
                 * There's no need to check (PrevBank != NULL)
                 * PrevBank is not NULL for the following reasons:
                 * 1. AsyncData->MaxNumBank != 0U
                 *     So the previous "for loop" must run at least once and execute 'PrevBank = Bank'
                 * 2. In "for loop", Bank != NULL, so PrevBank != NULL after 'PrevBank = Bank'
                 *     Otherwise Ret won't be CFS_OK
                 */
                PrevBank->Next = NULL;
            }
        } else {
            ULONG BufferUL = 0U;
            AmbaMisra_TypeCast(&BufferUL, &BankBuffer);
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CreateBankPool: [ERROR] BankBuffer %p BankBufferSize %u BankSize %u MaxNumBank %u", (UINT32)BufferUL, BankBufferSize, BankSize, AsyncData->MaxNumBank, 0);
            Ret = CFS_ERR_API; /* Parameters should be checked already. If there's an error return API error. */
        }
    }
    return Ret;
}

/**
 *  Delete the bank pool of the stream.
 *  Move all the banks from AsyncData->BankPool to g_AmbaCFSSch.BankAvail.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, others - Error
 */
static UINT32 DeleteBankPool(UINT8 StreamId)
{
    /* must be called with sch's mutex locked */
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    UINT32 Ret = CFS_OK;

    if (AsyncData->MaxNumBank != 0U) {
        AMBA_CFS_SCH_BANK_INFO_s *Bank = AsyncData->BankPool;

        if (Bank == NULL) {
            /* Unexpected error. There should be banks in BankPool when MaxNumBank != 0. */
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] BankPool is empty", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        } else {
            UINT32 BankCount = 1U;
            /* Find the last bank in AsyncData->BankPool */
            while (Bank->Next != NULL) {
                Bank = Bank->Next;
                BankCount++;
            }
            /* Check BankCount */
            if (BankCount != AsyncData->MaxNumBank) {
                /* Unexpected error. */
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "DeleteBankPool: [ERROR] Unexpected number of banks %u. (Should be %u)", BankCount, AsyncData->MaxNumBank, 0U, 0U, 0U);
                Ret = CFS_ERR_API;
            }
            /* Still release all the banks even if BankCount is unexpected */
            /* Move all the banks in BankPool to the front of BankAvail */
            Bank->Next = g_AmbaCFSSch.BankAvail;
            g_AmbaCFSSch.BankAvail = AsyncData->BankPool;
            AsyncData->BankPool = NULL;
        }
    }
    return Ret;
}

/**
 *  Check if scheduler met I/O eror
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] AsyncData           The async data
 *  @return 0 - OK, others - Error
 */
static inline UINT32 CheckScheduler(const AMBA_CFS_STREAM_ASYNC_DATA_s *AsyncData)
{
    UINT32 Ret = AsyncData->Error;
    /* Note that AsyncData->Error could be CFS_ERR_FS, so user needs to handle I/0 error. */
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] A previous I/O error is detected", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Wait a free bank available
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 WaitFreeBank(UINT8 StreamId)
{
    /* must be called with sch's mutex locked*/
    UINT32 Ret = CFS_OK;
    const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    /*
     * Take care of concurrent access. Consider the following case:
     *     If there're 2 tasks both calling WaitFreeBank() which is called by AmbaCFS_FileWrite(),
     *     it's possible that both tasks get AMBA_CFS_FLAG_BANK_RELEASE because the flag is not cleared.
     *     But actually there could be only one bank available.
     *     So it's possible that AmbaCFS_SchTakeEvent returns OK but AsyncData->BankPool is NULL.
     *     So make sure that 'AsyncData->BankPool != NULL' and mutex locked before function returns.
     *
     * TODO: Distinguish AMBA_CFS_FLAG_BANK_RELEASE of different Stream. Consider the following case:
     *     1. Task1 is waiting for a free bank from Stream1. And Task2 is waiting for a free bank from Stream2.
     *     2. Stream1 releases a bank and triggers AMBA_CFS_FLAG_BANK_RELEASE.
     *     3. Task2 takes the event and finds that Stream2 has no free bank. So clear AMBA_CFS_FLAG_BANK_RELEASE.
     *     4. Although Stream1 has a free bank, Task1 is blocked because AMBA_CFS_FLAG_BANK_RELEASE is cleared.
     */
    while ((AsyncData->BankPool == NULL) || (AsyncData->NumBank >= AsyncData->MaxNumBank)) {
        /*
         * Clear AMBA_CFS_FLAG_BANK_RELEASE every time
         * The only reason of entering the loop again is that AmbaCFS_SchTakeEvent returns OK but there's no free bank in AsyncData.
         * So clear the flag to avoid busy waiting.
         */
        Ret = AmbaCFS_SchClearEvent(AMBA_CFS_FLAG_BANK_RELEASE);
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_SchUnlock();
            if (Ret == CFS_OK) {
                Ret = AmbaCFS_SchTakeEvent(AMBA_CFS_FLAG_BANK_RELEASE, AMBA_CFS_TIMEOUT_EVTFLAG);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchTakeEvent fail", __func__, NULL, NULL, NULL, NULL);
                }
                if (Ret != CFS_ERR_FATAL) {
                    if (CFS_SET_RETURN(&Ret, AmbaCFS_SchLock()) != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Ret == CFS_OK) {
                    // Code Coverage: normal
                    /*
                     * I/O Error handling:
                     *     The previous process don't encounter I/O error. Start considering I/O error from now on.
                     *     If there's I/O error, the way of error handling for Scheduler:
                     *         1. Async Write Command: Don't Write Bank but release Cmd/Bank. Avoid adding further Async Write Command.
                     *         2. Async Read  Command: Don't Read  Bank but release Cmd/Bank. Avoid adding further Async Read Command.
                     *         3. Async Close Command: Release Cmd/Bank as usual. Async Close Command must be processed.
                     *     Note that this function is only used by Async Read/Write, so user should not allocate Bank when getting I/O error.
                     *     If there's I/O error, break the loop and let user release all the resources (including cmd, bank, ...).
                     */

                    /*
                     * Check if any error happened during the previous async I/O
                     * AllocateBank() has made sure that there's no error before calling this function.
                     * But there could be new errors when waiting AMBA_CFS_FLAG_BANK_RELEASE.
                     */
                    Ret = CheckScheduler(AsyncData);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchClearEvent fail", __func__, NULL, NULL, NULL, NULL);
        }

        /*
         * When there's I/O error, stop waiting and return error because WaitFreeBank is only used in Async Read/Write.
         * In Async Read/Write, the process should be stopped when getting I/O error.
         */
        if (Ret != CFS_OK) {
            // Code Coverage (Yellow): Error in the previous process. For example, AmbaCFS_SchClearEvent failed.
            // TODO: 1. If the error is caused by AmbaKAL_EventFlagGet timeout, continue the loop. Need to add a CFS error code for "timeout".
            // TODO: 2. Reduce 'timeout' so that the loop can break quickly when there's error in Sch Task.
            break;
        }
    }
    return Ret;
}

/**
 *  Allocate a new bank.
 *  Remvoe a Bank from AsyncData->BankPool, and return the Bank.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @param [out] Bank               The allocated bank
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AllocateBank(UINT8 StreamId, AMBA_CFS_SCH_BANK_INFO_s **Bank)
{
    /* must be called with sch's mutex locked */
    UINT32 Ret;
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    /*
     * I/O Error handling:
     *     Start considering I/O error from now on.
     *     If there's I/O error, the way of error handling for Scheduler:
     *         1. Async Write Command: Don't Write Bank but release Cmd/Bank. Avoid adding further Async Write Command.
     *         2. Async Read  Command: Don't Read  Bank but release Cmd/Bank. Avoid adding further Async Read Command.
     *         3. Async Close Command: Release Cmd/Bank as usual. Async Close Command must be processed.
     *     As a result, this function only needs to consider Read/Write and should not allocate Bank when getting I/O error.
     */

    /* Check if any error happened during the previous async I/O */
    Ret = CheckScheduler(AsyncData);
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
    }

    if (Ret == CFS_OK) {
        /*
         * In Read mode, Bank is always released before alloc, so there's no need to wait free bank. (Please refer to StartAsyncRead and AmbaCFS_SchAsyncfreadImpl.)
         * In Write mode, we might need to wait Bank when writing a large data. (Please refer to AmbaCFS_SchAsyncfwriteImpl)
         * It's possible when data size > (AsyncData->MaxNumBank * g_AmbaCFSSch.BankSize)
         * To simplify the code, wait free bank regardless of Read/Write mode.
         */
        Ret = WaitFreeBank(StreamId);
        if (Ret == CFS_OK) {
            AMBA_CFS_SCH_BANK_INFO_s * const RetBank = AsyncData->BankPool;
            if (RetBank != NULL) {
                AsyncData->BankPool = RetBank->Next;
                AsyncData->NumBank++;
                /* Set return value */
                *Bank = RetBank;
            } else {
                /* Should not happen. WaitFreeBank returns OK but BankPool is NULL. */
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] BankPool is NULL", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] WaitFreeBank fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Release a bank.
 *  Insert the input Bank into AsyncData->BankPool.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @param [in] Bank                The bank
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 ReleaseBank(UINT8 StreamId, AMBA_CFS_SCH_BANK_INFO_s *Bank)
{
    /* must be called with sch's mutex locked */
    UINT32 Ret = CFS_ERR_API;
    if (Bank != NULL) {
        AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
        Bank->Next = AsyncData->BankPool;
        AsyncData->BankPool = Bank;
        AsyncData->NumBank--;
        Ret = AmbaCFS_SchGiveEvent(AMBA_CFS_FLAG_BANK_RELEASE);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchGiveEvent fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Bank is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Allocate a new command.
 *  Remvoe a Cmd from g_AmbaCFSSch.CmdAvail, and return the Cmd.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Event               The CFS event
 *  @param [in] Stream              The CFS stream (only keep the pointer, to let scheduler detemine to use it or not)
 *  @param [in] StreamId            The stream ID
 *  @param [in] Length              The stream length
 *  @param [out] CmdInfo            The returned address of a free command slot
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AllocateCmd(UINT32 Event, AMBA_CFS_STREAM_s *Stream, UINT8 StreamId, UINT64 Length, AMBA_CFS_SCH_CMD_INFO_s **CmdInfo)
{
    /* must be called with sch's mutex locked */

    /*
     * The sufficient number of CmdAmount is:
     *     CmdAmount = BankAmount + AMBA_CFS_MAX_STREAM.
     * The case that we need the most commands:
     *     1. All Async Read/Write commands: Each bank needs one command, so the number of commands is "BankAmount"
     *     2. All streams close at the same time: Each stream needs one command, so the number of commands is "AMBA_CFS_MAX_STREAM"
     */
    UINT32 Ret = CFS_OK;
    /*
     * Check free command
     * Do not wait for free command. User should make sure that CmdAmount is always large enough.
     */
    if (g_AmbaCFSSch.CmdAvail != NULL) {
        AMBA_CFS_SCH_CMD_INFO_s *Cmd = g_AmbaCFSSch.CmdAvail;
        Cmd->Event = Event;
        Cmd->Stream = Stream;
        Cmd->StreamId = StreamId;
        Cmd->Length = Length;
        g_AmbaCFSSch.CmdAvail = Cmd->Next;
        /* Set return value */
        *CmdInfo = Cmd;
    } else {
        /*
         * Should not happen.
         * Maybe user didn't set CmdAmount large enough.
         * Or the Cmd is not released properly.
         */
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] No free Cmd. Please increase CmdAmount in AmbaCFS_Init()", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  Push a command to the command queue of a task
 *  Assign Bank to Cmd->Bank. Then append Cmd to the tail of Task->CmdList.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Cmd                 The command
 *  @param [in] Bank                The bank
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 PushCmd(AMBA_CFS_SCH_CMD_INFO_s *Cmd, AMBA_CFS_SCH_BANK_INFO_s *Bank)
{
    /* must be called with sch's mutex locked */
    UINT32 Ret;
    AMBA_CFS_SCH_TASK_INFO_s * const Task = &g_AmbaCFSSch.Task;
    AMBA_CFS_SCH_CMD_INFO_s *TmpCmd = Task->CmdList;
    Cmd->Bank = Bank;
    if (TmpCmd == NULL) {
        /* no command, set as the head */
        Task->CmdList = Cmd;
        Cmd->Next = NULL;
    } else {
        /* Append to the tail */
        while (TmpCmd->Next != NULL) {
            TmpCmd = TmpCmd->Next;
        }
        TmpCmd->Next = Cmd;
        Cmd->Next = NULL;
    }
    Ret = AmbaCFS_SchGiveEvent(AMBA_CFS_FLAG_CMD_CREATE);
    if (Ret == CFS_OK) {
        AMBA_CFS_EVENT_INFO_s Param;
        Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
        if (Ret == CFS_OK) {
            Param.Event = Cmd->Event;
            AmbaMisra_TypeCast(&Param.File, &Cmd->Stream);
            Ret = AmbaCFS_AddEvent(&Param);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchGiveEvent fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Release a command.
 *  Insert the input Cmd into g_AmbaCFSSch.CmdAvail.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Cmd                 The command
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 ReleaseCmd(AMBA_CFS_SCH_CMD_INFO_s *Cmd)
{
    /* must be called with sch's mutex locked */
    UINT32 Ret = CFS_OK;
    Cmd->Next = g_AmbaCFSSch.CmdAvail;
    g_AmbaCFSSch.CmdAvail = Cmd;
    return Ret;
}

/**
 *  Flush the read buffer of a stream.
 *  Remove all the banks from AsyncData->Bank, and put them into AsyncData->BankPool.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 FlushReadBuffer(UINT8 StreamId)
{
    /* must be called with sch's mutex locked */
    UINT32 Ret = CFS_OK;
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    AMBA_CFS_SCH_BANK_INFO_s *Bank = AsyncData->Bank;
    while (Bank != NULL) {
        AMBA_CFS_SCH_BANK_INFO_s *Temp = Bank;
        Bank = Bank->Next;
        Ret = ReleaseBank(StreamId, Temp);
        if (Ret != CFS_OK) {
            /* fatal error, no error handling */
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseBank fail", __func__, NULL, NULL, NULL, NULL);
            break;
        }
    }
    AsyncData->Bank = NULL;
    return Ret;
}

/**
 *  Wait async read done
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. User can keep waiting AsyncReadDone because CmdProcess_Read will set AReadDone flag during I/O Error.
 *  @param [in] StreamId            The stream ID
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchWaitAsyncReadDone(UINT8 StreamId)
{
    UINT32 Ret;

    /* g_AmbaCFSSch.Mutex should be locked */

    Ret = AmbaCFS_SchClearAReadDone(StreamId);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_SchUnlock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_SchTakeAReadDone(StreamId);
            if (Ret != CFS_ERR_FATAL) {
                if (AmbaCFS_SchLock() != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_FATAL;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
        }
        /* Check error during waiting. */
        if (Ret == CFS_OK) {
            const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
            /*
             * I/O Error handling:
             *     The previous process don't encounter I/O error. Start considering I/O error from now on.
             *     If there's I/O error, user can keep waiting AReadDone because Sch Task will release resources
             *     and call AmbaCFS_SchGiveAReadDone during I/O Error.
             */

            /* Check if any error happened during previous async I/O */
            Ret = CheckScheduler(AsyncData);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchClearAReadDone fail", __func__, NULL, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Remove all read commands
 *  I/O Error handling for User:
 *      FlushBuffer = 0: None. Won't return CFS_ERR_FS.
 *      FlushBuffer = 1: Need to handle CFS_ERR_FS. In this case, Bank and Cmd are already released.
 *  @param [in] StreamId            The stream ID
 *  @param [in] FlushBuffer         Also flush buffer
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 FlushReadCmd(UINT8 StreamId, UINT8 FlushBuffer)
{
    /* must be called with sch's mutex locked */
    UINT32 Ret = CFS_OK;
    AMBA_CFS_SCH_TASK_INFO_s * const Task = &g_AmbaCFSSch.Task;
    AMBA_CFS_SCH_CMD_INFO_s *Cmd = Task->CmdList;
    AMBA_CFS_SCH_CMD_INFO_s *Prev = NULL;    /* the previous command that is not from the stream */
    AMBA_CFS_SCH_CMD_INFO_s *Next;
    /* remove stream's commands in pending queue */
    while (Cmd != NULL) {
        if ((Cmd->StreamId == StreamId) && (Cmd->Event == AMBA_CFS_EVENT_ASYNC_READ)) {
            Next = Cmd->Next;
            if (Cmd->Bank != NULL) {
                Ret = ReleaseBank(StreamId, Cmd->Bank);
                if (Ret == CFS_OK) {
                    Ret = ReleaseCmd(Cmd);
                    if (Ret == CFS_OK) {
                        Cmd = Next;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseCmd fail", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseBank fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Bank is NULL", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_API;
            }

            if (Ret != CFS_OK) {
                /* fatal error, no error handling */
                break;
            }
        } else {
            if (Prev == NULL) {
                Task->CmdList = Cmd;    /* new head found */
            } else {
                Prev->Next = Cmd;
            }
            Prev = Cmd;
            Cmd = Cmd->Next;
        }
    }
    if (Prev == NULL) {
        Task->CmdList = NULL;   /* no command */
    } else {
        Prev->Next = NULL;
    }

    if (Ret == CFS_OK) {
        if (FlushBuffer != 0U) {
            /* must wait current read done */
            Cmd = Task->Cmd;
            /*
             * I/O Error handling:
             *     The previous process don't encounter I/O error. Start considering I/O error from now on.
             *     If there's I/O error, still need to call FlushReadBuffer.
             */

            /* check active read cmd */
            if (Cmd != NULL) {
                if ((Cmd->StreamId == StreamId) && (Cmd->Event == AMBA_CFS_EVENT_ASYNC_READ)) {
                    Ret = AmbaCFS_SchWaitAsyncReadDone(StreamId);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchWaitAsyncReadDone fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            }

            /*
             * Task->Cmd should be NULL at this point.
             * In CmdProcess_Read(), AmbaCFS_SchGiveAReadDone() is called with Sch's mutex locked.
             * Task->Cmd will be set to NULL in AmbaCFS_ProcessEventImpl().
             * Then the Sch's mutex will be unlocked in AmbaCFS_ProcessEvent().
             * That's when AmbaCFS_SchWaitAsyncReadDone() get the Sch's mutex again.
             */
            if (Task->Cmd != NULL) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Task->Cmd is not released", __func__, NULL, NULL, NULL, NULL);
                (void)CFS_SET_RETURN(&Ret, CFS_ERR_API);
            }

            /* flush read buffer */
            if ((Ret == CFS_OK) || (Ret == CFS_ERR_FS)) { /* Release CFS resourses even if CFS_ERR_FS occurred. */
                /*
                 * Afrer AmbaCFS_SchWaitAsyncReadDone returns, that means CmdProcess_ReadImpl is done.
                 * Consider the following cases:
                 *     1. Success: The Task->Cmd->Bank has been moved to AsyncData->Bank
                 *     2. I/O error: The Task->Cmd->Bank has been moved to AsyncData->BankPool
                 *     In either cases, we should call FlushReadBuffer() so that all the Banks are moved to AsyncData->BankPool.
                 */
                if (CFS_SET_RETURN(&Ret, FlushReadBuffer(StreamId)) != CFS_OK) {
                    /* fatal error, no error handling */
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FlushReadBuffer fail", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
    return Ret;
}

/**
 *  Adjust file pointer
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] File                The file
 *  @param [in] Pos                 The position
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AdjustFilePointer(AMBA_FS_FILE *File, UINT64 Pos)
{
    /* should not lock sch's mutex */
    UINT32 Ret;
    UINT64 CurPos = 0ULL;
    /* TODO: add file read/write position to stream to avoid ftell()? (or maybe just fseek, no need to ftell()?) */
    Ret = CFS_F2C(AmbaFS_FileTell(File, &CurPos));
    if (Ret == CFS_OK) {
        if (Pos != CurPos) {
            /* adjust file pointer */
            Ret = CFS_F2C(AmbaFS_FileSeek(File, (INT64) Pos, AMBA_FS_SEEK_START));
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AdjustFilePointer: [ERROR] AmbaFS_FileSeek fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
            }
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AdjustFilePointer: [ERROR] AmbaFS_FileTell fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Align file size
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Stream->RealLength could be incorrect. File position might be changed.
 *      Possible cases:
 *      1. Sync  FileWrite: In AmbaCFS_SchSyncfwrite(). APP should close file when getting CFS_ERR_FS.
 *      2. Async FileWrite: In CmdProcess_Write(). Release all the resources (Cmd, Bank, ...) and Close file.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Length              File length
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AlignFile(AMBA_CFS_STREAM_s *Stream, UINT64 Length)
{
    /* it is safe to use stream: File, FileName, RealLength, AlignMode, AlignSize (sync: protected by stream's mutex, async: only used by sch) */
    UINT32 Ret = CFS_OK;
    if ((Stream->AlignMode == AMBA_CFS_ALIGN_MODE_CONTINUOUS) && (Stream->AlignSize != 0U) && (Length > Stream->RealLength)) {
        const UINT64 Len = Length - Stream->RealLength;
        if ((Len % Stream->AlignSize) == 0U) {
            AMBA_FS_FILE *File = Stream->File;
            UINT64 AppendedSize = 0ULL;
            /*
             * I/O Error handling:
             *     The previous process don't encounter I/O error. Start considering I/O error from now on.
             *     If there's I/O error, return immediately.
             *     Stream->RealLength could be incorrect after function return. File position might be changed.
             */

            Ret = CFS_F2C(AmbaFS_ClusterAppend(File, Len, &AppendedSize));
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AlignFile: [ERROR] AmbaFS_ClusterAppend fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
            }

            if (Ret == CFS_OK) {
                Stream->RealLength += AppendedSize;
                Ret = CFS_F2C(AmbaFS_FileSeek(File, (INT64)Stream->RealLength - 1, AMBA_FS_SEEK_START));
                if (Ret == CFS_OK) {
                    UINT8 Buffer = 0U;
                    UINT32 WriteCount = 0U;
                    /* Write a byte of dummy data at the end */
                    Ret = CFS_F2C(AmbaFS_FileWrite(&Buffer, 1, 1, File, &WriteCount));
                    if (Ret == CFS_OK) {
                        if (WriteCount != 1U) {
                            /*
                                WriteCount is not expected when "Ret == CFS_OK".
                                It's possible that I/O error occurred but AmbaFS_FileWrite return OK.
                                So use AmbaFS_GetError to determine error code.
                                If AmbaFS_GetError also returns OK, then return CFS_ERR_FATAL because the root cause is unknown.
                             */
                            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AlignFile: [ERROR] AmbaFS_FileWrite fail. WriteCount unexpected: %u ErrNum: %p", WriteCount, AmbaCFS_UtilGetAmbaFSError(), 0U, 0U, 0U);
                            Ret = CFS_F2C(AMBA_FS_ERR_API); /* Input AMBA_FS_ERR_API to run AmbaFS_GetError and convert it to CFS error */
                            if (Ret == CFS_OK) {
                                /* Should never happen. So return fatal error. */
                                Ret = CFS_ERR_FATAL;
                            }
                        } else if (AppendedSize < Len) {
                            char DriveName;
                            /* too fragmented */
                            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AlignFile: [ERROR] Too fragmented. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0U, 0U, 0U, 0U);
                            if (AmbaCFS_UtilGetDrive(Stream->Filename, &DriveName) == CFS_OK) {
                                AMBA_CFS_EVENT_INFO_s Param;
                                if (CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param))) != CFS_OK) {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                                } else {
                                    Param.Event = AMBA_CFS_EVENT_TOO_FRAGMENTED;
                                    Param.Name[0] = DriveName;
                                    Param.Name[1] = '\0';
                                    if (AmbaCFS_AddEvent(&Param) != CFS_OK) {
                                        /* not a critical event, skip error handling */
                                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                                    }
                                }
                            }
                            Ret = CFS_ERR_FATAL;
                        } else {
                            /* Success. Do nothing. */
                        }
                        if (Ret == CFS_OK) {
                            /* after each alignment, RealLength should be the same as the specified length */
                            Stream->RealLength = Length;
                        }
                    } else {
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AlignFile: [ERROR] AmbaFS_FileWrite fail. WriteCount: %u ErrNum: %p", WriteCount, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AlignFile: [ERROR] AmbaFS_FileSeek fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AlignFile: [ERROR] Len %u AlignSize %u", (UINT32) Len, Stream->AlignSize, 0, 0, 0);
            Ret = CFS_ERR_API;
        }
    } else {
        /* Do nothing. Return success. */
    }
    return Ret;
}

/**
 *  Sync file
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] DataSize            Data size
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 SyncFile(AMBA_CFS_STREAM_s *Stream, UINT32 DataSize)
{
    /* should not lock sch's mutex to avoid blocking other streams */
    /* it is safe to use stream: File, FileName, BytesToSync, AccBytesToSync (sync: protected by stream's mutex, async: only used by sch) */
    UINT32 Ret = CFS_OK;
    /* Check bytes to sync */
    if (Stream->BytesToSync > 0U) {
        Stream->AccBytesNoSync += DataSize;
        if (Stream->AccBytesNoSync >= Stream->BytesToSync) {
            /*
             * I/O Error handling:
             *     The previous process don't encounter I/O error. Start considering I/O error from now on.
             *     If there's I/O error, return immediately.
             */

            Ret = CFS_F2C(AmbaFS_FileSync(Stream->File));
            if (Ret == CFS_OK) {
                Stream->AccBytesNoSync = 0U;
                /*
                 * After FileSync, the file info should be changed. So remove cache to update file info.
                 * If there's I/O error, the cache will be handled in CloseFile. No need to remove cache here.
                 */
                if (g_AmbaCFSSch.CacheEnable != 0U) {
                    Ret = AmbaCFS_CacheRemove(Stream->Filename);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "SyncFile: [ERROR] AmbaFS_FileSync fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
            }
        }
    }
    return Ret;
}

/**
 *  The implementation of processing a fwrite command.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *      Possible cases:
 *      1. Sync  FileWrite: In AmbaCFS_SchSyncfwrite(). APP should close file when getting CFS_ERR_FS.
 *      2. Async FileWrite: In CmdProcess_Write(). Release all the resources (Cmd, Bank, ...) and Close file.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Bank                The bank
 *  @param [in] Length              The expected file length
 *  @param [out] IoTime             Total I/O time
 *  @param [out] WriteSize          The returned size of written records
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 WriteBank(AMBA_CFS_STREAM_s *Stream, const AMBA_CFS_SCH_BANK_INFO_s *Bank, UINT64 Length, UINT32 *IoTime, UINT32 *WriteSize)
{
    /* should not lock sch's mutex to avoid blocking other streams */
    /* it is safe to use stream: File */
    UINT32 Ret = CFS_ERR_API;
    if (Bank != NULL) {
        if (Bank->Length > 0U) {
            /*
             * I/O Error handling:
             *     The previous process don't encounter I/O error. Start considering I/O error from now on.
             *     If there's I/O error, return immediately.
             */

            /* align File first, then adjust file pointer (better than adjust and then align, can reduce fseek) */
            Ret = AlignFile(Stream, Length);
            if (Ret == CFS_OK) {
                Ret = AdjustFilePointer(Stream->File, Bank->Pos);
                if (Ret == CFS_OK) {
                    UINT32 Start;
                    /* Writing */
                    Ret = CFS_K2C(AmbaKAL_GetSysTickCount(&Start));
                    if (Ret == CFS_OK) {
                        UINT32 WriteCount;
                        UINT32 DataSize = Bank->Length;
                        Ret = CFS_F2C(AmbaFS_FileWrite(Bank->Buffer, 1, DataSize, Stream->File, &WriteCount));
#ifdef AMBA_CFS_SCH_SLOW_TEST
                        (void)AmbaKAL_TaskSleep(200);
#endif
                        if (Ret == CFS_OK) {
                            UINT32 NextPos = (UINT32)Bank->Pos + WriteCount;
                            UINT32 End;
                            if (WriteCount != DataSize) {
                                /* WriteCount is not expected when "Ret == CFS_OK". Return OK and let user handle this case. */
                                /* It's possible that I/O error occurred but AmbaFS_FileWrite return OK. */
                                /*AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "WriteBank: [ERROR] WriteCount unexpected. DataSize: %u. Written size: %u. ErrNum: %p", DataSize, WriteCount, AmbaCFS_UtilGetAmbaFSError(), 0, 0);*/
                            }
                            if (Stream->RealLength < NextPos) {
                                /* Update RealLength in case that an error byte might be written in CloseFile() in AMBA_CFS_ALIGN_MODE_SIZE mode. */
                                Stream->RealLength = NextPos;
                            }
                            Ret = CFS_K2C(AmbaKAL_GetSysTickCount(&End));
                            if (Ret == CFS_OK) {
                                /* Check bytes to sync */
                                Ret = SyncFile(Stream, WriteCount);
                                if (Ret == CFS_OK) {
                                    if (End >= Start) {
                                        *IoTime = End - Start;
                                    } else {
                                        *IoTime = 0xFFFFFFFFU - (Start - End - 1U);  /* to avoid overflow */
                                    }
                                    *WriteSize = WriteCount;
                                } else {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SyncFile fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_GetSysTickCount fail", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "WriteBank: [ERROR] AmbaFS_FileWrite fail. DataSize: %u. Written size: %u. ErrNum: %p", DataSize, WriteCount, AmbaCFS_UtilGetAmbaFSError(), 0, 0);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_GetSysTickCount fail", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AdjustFilePointer fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AlignFile fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "WriteBank: [ERROR] Length %u", Bank->Length, 0, 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Bank is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  The implementation of processing a fwrite command.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Release all the resources (Cmd, Bank, ...) and Close file.
 *  @param [in] Cmd                 The command
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CmdProcess_WriteImpl(const AMBA_CFS_SCH_CMD_INFO_s *Cmd)
{
    /* must be called with sch's mutex locked */
    /* it is safe to use stream: StreamId, Filename */
    UINT32 Ret;
    /* unlock sch's mutex because fwrite is slow */
    Ret = AmbaCFS_SchUnlock();
    if (Ret == CFS_OK) {
        const AMBA_CFS_SCH_BANK_INFO_s * const Bank = Cmd->Bank;
        UINT32 IoTime = 0U;
        UINT32 WriteSize;
        /*
         * I/O Error handling:
         *     The previous process don't encounter I/O error. Start considering I/O error from now on.
         *     Make sure AmbaCFS_SchLock is called when there's I/O error.
         */
        Ret = WriteBank(Cmd->Stream, Bank, Cmd->Length, &IoTime, &WriteSize);
        if (Ret == CFS_OK) {
            if (Bank->Length != WriteSize) {
                /*
                    WriteSize is unexpected.
                    If SD card is removed when writing file, AmbaFS_FileWrite returns OK but AmbaFS_GetError returns PF_ERR_EIO.
                    So use AmbaFS_GetError to determine error code.
                    If AmbaFS_GetError also returns OK, then return CFS_ERR_FATAL because the root cause is unknown.
                    NOTE: This error handling is only in async write because user can't get WriteSize.
                          In sync write, user need to handle this case.
                 */
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CmdProcess_WriteImpl: [ERROR] WriteSize unexpected. DataSize: %u. WriteSize: %u. ErrNum: %p", Bank->Length, WriteSize, AmbaCFS_UtilGetAmbaFSError(), 0, 0);
                Ret = CFS_F2C(AMBA_FS_ERR_API); /* Input AMBA_FS_ERR_API to run AmbaFS_GetError and convert it to CFS error */
                if (Ret == CFS_OK) {
                    /* Should never happen. So return fatal error. */
                    Ret = CFS_ERR_FATAL;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] WriteBank fail", __func__, NULL, NULL, NULL, NULL);
        }

        /* Lock Mutex regardless of previous error */
        if (CFS_SET_RETURN(&Ret, AmbaCFS_SchLock()) != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Process a fwrite command.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Release Cmd and Close file. Note that Cmd->Bank is already released.
 *  @param [in] Cmd                 The command
 *  @param [in] IsDiscarded         Whether the command is discarded. 0 - Normal code flow, 1 - Skip I/O and only release resources
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CmdProcess_Write(const AMBA_CFS_SCH_CMD_INFO_s *Cmd, UINT32 IsDiscarded)
{
    /* must be called with sch's mutex locked */
    /* it is safe to use stream: StreamId */
    UINT32 Ret;
    const UINT8 StreamId = Cmd->Stream->StreamId;
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    /*
     * I/O Error handling:
     *     If I/O error already happened (i.e. CheckScheduler failed), there's no point in writing data so skip CmdProcess_WriteImpl.
     *     If OK or I/O error, release Cmd->Bank.
     *     Always call AmbaCFS_SchGiveAWriteDone to avoid blocking WaitPendingWrite.
     *     Update AsyncData->Error.
     */
    Ret = CheckScheduler(AsyncData);
    if (Ret == CFS_OK) {
        if (IsDiscarded == 0U) {
            Ret = CmdProcess_WriteImpl(Cmd);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
    }
    /*
     * Release Bank
     *     If FileWrite succeeded, put the bank in AsyncData->Bank.
     *     If FileWrite got I/O error, release the Cmd->Bank. Put the bank in AsyncData->BankPool.
     *     If FileWrite got other errors, it's not recoverable. Do nothing and return error.
     *
     * Release Bank if the event is discarded. (IsDiscarded == 1U)
     *     In this case, (Ret == CFS_OK) should pass, so ReleaseBank() should be executed.
     */
    if ((Ret == CFS_OK) || (Ret == CFS_ERR_FS)) {
        if (CFS_SET_RETURN(&Ret, ReleaseBank(StreamId, Cmd->Bank)) != CFS_OK) {
            /* fatal error, no error handling */
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseBank fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Give event in any condition (including fatal error) to avoid blocking other functions forever. */
    if (CFS_SET_RETURN(&Ret, AmbaCFS_SchGiveAWriteDone(StreamId)) != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchGiveAWriteDone fail", __func__, NULL, NULL, NULL, NULL);
    }
    /* Let AsyncData->Error save the most critical error so far */
    if (Ret != CFS_OK) {
        (void)CFS_SET_RETURN(&AsyncData->Error, Ret);
    }
    return Ret;
}

/**
 *  Read data to a bank
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *      Possible cases:
 *      1. Sync  FileRead: In AmbaCFS_SchSyncfread(). APP should close file when getting CFS_ERR_FS.
 *      2. Async FileRead: In CmdProcess_Read(). Release all the resources (Cmd, Bank, ...) and Close file.
 *  @param [in] Bank                The bank
 *  @param [in] File                The file
 *  @param [out] ReadSize           The returned size of read records
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 ReadBank(const AMBA_CFS_SCH_BANK_INFO_s *Bank, AMBA_FS_FILE *File, UINT32 *ReadSize)
{
    /* should not lock sch's mutex to avoid blocking other streams */
    UINT32 Ret = CFS_ERR_API;
    if (Bank != NULL) {
        if (Bank->Length > 0U) {
            /*
             * I/O Error handling:
             *     Return immediately when there's I/O error.
             */

            /* adjust file pointer */
            Ret = AdjustFilePointer(File, Bank->Pos);
            if (Ret == CFS_OK) {
                UINT32 ReadCount;
                /* Reading */
                /* AmbaSD will handle the overwriting of cache flush, CFS need not deliver a cache-line aligned buffer to AmbaFS_FileRead. */
                Ret = CFS_F2C(AmbaFS_FileRead(Bank->Buffer, 1, Bank->Length, File, &ReadCount));
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "ReadBank: [ERROR] AmbaFS_FileRead fail. DataSize: %u, Read size: %u. ErrNum: %p", Bank->Length, ReadCount, AmbaCFS_UtilGetAmbaFSError(), 0, 0);
                }
                /* Don't return error when (ReadCount != Bank->Length). It's possible when overread. */
#ifdef AMBA_CFS_SCH_SLOW_TEST
                (void)AmbaKAL_TaskSleep(200);
#endif
                *ReadSize = ReadCount;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AdjustFilePointer fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "ReadBank: [ERROR] Length %u", Bank->Length, 0, 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Bank is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  The implementation of processing a fread command.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Release all the resources (Cmd, ...) and Close file.
 *  @param [in] Cmd                 The command
 *  @param [in] IsDiscarded         Whether the command is discarded. 0 - Normal code flow, 1 - Skip I/O and only release resources
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CmdProcess_ReadImpl(const AMBA_CFS_SCH_CMD_INFO_s *Cmd, UINT32 IsDiscarded)
{
    /* must be called with sch's mutex locked (but is is safe to use Stream: StreamId, File) */
    UINT32 Ret;
    if (IsDiscarded == 0U) {
        /* unlock sch's mutex because fread is slow */
        Ret = AmbaCFS_SchUnlock();
        if (Ret == CFS_OK) {
            const AMBA_CFS_STREAM_s * const Stream = Cmd->Stream;
            const UINT8 StreamId = Stream->StreamId;
            UINT32 ReadSize = 0U;
            AMBA_CFS_SCH_BANK_INFO_s * const Bank = Cmd->Bank;
            AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
            /*
             * I/O Error handling:
             *     The previous process don't encounter I/O error. Start considering I/O error from now on.
             *     Make sure AmbaCFS_SchLock is called when there's I/O error.
             */

            /* it is safe to use Stream->File because it is only used by sch task */
            Ret = ReadBank(Bank, Stream->File, &ReadSize);
            if (Ret == CFS_OK) {
                /*
                    Usually Bank->Length is BankSize. If it's the last bank, the Bank->Length is adjusted to match file length.
                    So ReadSize should be equal to Bank->Length. If they're different, there could be some errors during I/O.
                 */
                if (Bank->Length != ReadSize) {
                    /*
                     * ReadSize is unexpected.
                     * There's a chance that AmbaFS_FileRead failed but return OK. However, we can get the error code by AmbaFS_GetError().
                     * So use AmbaFS_GetError to determine error code.
                     * If AmbaFS_GetError also returns OK, then return CFS_ERR_FATAL because the root cause is unknown.
                     *
                     * NOTE: In size-align mode, since the file is only aligned on file close, the Stream->Length is not the actual file size before file close.
                     * In the following code flow, it's possible to get this error (but actually it's not an error):
                     * 1. Open in AMBA_CFS_FILE_MODE_WRITE_READ with size-align
                     * 2. Write some data
                     * 3. Seek to front
                     * 4. Read to end
                     * ==> Might get (Bank->Length != ReadSize) in the last bank
                     * Since this is not a practical case and the error handling will be complicated, ignore the special case for now.
                     * TODO: Do not return error in this special case.
                     */
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CmdProcess_ReadImpl: [ERROR] ReadSize unexpected. DataSize: %u. ReadSize: %u. ErrNum: %p. Pos: %u", Bank->Length, ReadSize, AmbaCFS_UtilGetAmbaFSError(), (UINT32)Bank->Pos, 0U);
                    Ret = CFS_F2C(AMBA_FS_ERR_API); /* Input AMBA_FS_ERR_API to run AmbaFS_GetError and convert it to CFS error */
                    if (Ret == CFS_OK) {
                        /* Should never happen. So return fatal error. */
                        Ret = CFS_ERR_FATAL;
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReadBank fail", __func__, NULL, NULL, NULL, NULL);
            }

            /* Lock Mutex regardless of previous error */
            if (CFS_SET_RETURN(&Ret, AmbaCFS_SchLock()) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
            }

            /* Clean cache for GDMA copy */
            if (Ret == CFS_OK) {
                if ((ReadSize != 0U) && (Stream->DmaEnable == 1U)) {
                    ULONG ReadSizeAlign = AmbaCFS_GetAlignedValUL(ReadSize, AMBA_CACHE_LINE_SIZE); /* No need to worry about overflow because bank size is already aligned. */
                    /*
                     * For async read by GDMA, source is written by FS read, need to clean cache.
                     *     [Improve efficiency]
                     *     Actually, AmbaFS_FileRead copies most of the data to bank via DMA.
                     *     So in most cases, CFS does not need to clean the cache again.
                     *     Example 1: If data size is not aligned, the tail of the data might be stored in FS internal buffer and copied to Bank by ARM.
                     *     Example 2: If file offset is not aligned, the head of the data might be stored in FS internal buffer and copied to Bank by ARM.
                     *     Only the portion of data copied by ARM need to clean cache.
                     *     We can avoid redundant cache clean and improve the efficiency, as long as we know the internal buffering mechanism of file system.
                     * For async write by GDMA, source is user buffer, it should be taken cared of by user.
                     * In video recording, source is written by uCode, so DRAM data is correct and doesn't need to clean cache.
                     */
                    Ret = AmbaCFS_SchDataClean(Bank->Buffer, (UINT32)ReadSizeAlign);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchDataClean fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            }

            /*
                If FileRead succeeded, put the bank in AsyncData->Bank.
                If FileRead got I/O error, release the bank. Put the bank in AsyncData->BankPool. Will be done in CmdProcess_Read().
                If FileRead got other errors, it's not recoverable. Do nothing and return error.
             */
            if (Ret == CFS_OK) {
                AMBA_CFS_SCH_BANK_INFO_s *Tail = AsyncData->Bank;
                /* append the Bank to Stream->AsyncData.Bank */
                Bank->Next = NULL;
                if (Tail != NULL) {
                    while (Tail->Next != NULL) {
                        Tail = Tail->Next;
                    }
                    Tail->Next = Bank;
                } else {
                    AsyncData->Bank = Bank;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        const AMBA_CFS_STREAM_s * const Stream = Cmd->Stream;
        const UINT8 StreamId = Stream->StreamId;
        AMBA_CFS_SCH_BANK_INFO_s * const Bank = Cmd->Bank;
        AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
        AMBA_CFS_SCH_BANK_INFO_s *Tail = AsyncData->Bank;
        /*
         * When the event is discarded, still need to add the Bank to AsyncData even if the content is not correct.
         * Otherwise AmbaCFS_SchAsyncfreadImpl might wait forever.
         */

        /* append the Bank to Stream->AsyncData.Bank */
        Bank->Next = NULL;
        if (Tail != NULL) {
            while (Tail->Next != NULL) {
                Tail = Tail->Next;
            }
            Tail->Next = Bank;
        } else {
            AsyncData->Bank = Bank;
        }
        Ret = CFS_OK;
    }
    return Ret;
}

/**
 *  Process a fread command.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Release Cmd and Close file. Note that Cmd->Bank is already released.
 *  @param [in] Cmd                 The command
 *  @param [in] IsDiscarded         Whether the command is discarded. 0 - Normal code flow, 1 - Skip I/O and only release resources
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CmdProcess_Read(const AMBA_CFS_SCH_CMD_INFO_s *Cmd, UINT32 IsDiscarded)
{
    /* must be called with sch's mutex locked (but it is safe to use Cmd->Stream->StreamId) */
    UINT32 Ret;
    const UINT8 StreamId = Cmd->Stream->StreamId;
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    /*
     * I/O Error handling:
     *     If I/O error already happened (i.e. CheckScheduler failed), there's no point in reading data so skip CmdProcess_ReadImpl.
     *     If I/O error, release Cmd->Bank.
     *     Always call AmbaCFS_SchGiveAReadDone to avoid blocking WaitPendingWrite.
     *     Update AsyncData->Error.
     */
    Ret = CheckScheduler(AsyncData);
    if (Ret == CFS_OK) {
        Ret = CmdProcess_ReadImpl(Cmd, IsDiscarded);
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
    }
    /*
     * Release Bank if there's I/O error.
     *     If FileRead succeeded, put the bank in AsyncData->Bank. It's done in CmdProcess_ReadImpl.
     *     If FileRead got I/O error, release the Cmd->Bank. Put the bank in AsyncData->BankPool.
     *     If FileRead got other errors, it's not recoverable. Do nothing and return error.
     */
    if (Ret == CFS_ERR_FS) {
        if (CFS_SET_RETURN(&Ret, ReleaseBank(StreamId, Cmd->Bank)) != CFS_OK) {
            /* fatal error, no error handling */
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseBank fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Give event in any condition (including fatal error) to avoid blocking other functions forever. */
    if (CFS_SET_RETURN(&Ret, AmbaCFS_SchGiveAReadDone(StreamId)) != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchGiveAReadDone fail", __func__, NULL, NULL, NULL, NULL);
    }
    /* Let AsyncData->Error save the most critical error so far */
    if (Ret != CFS_OK) {
        (void)CFS_SET_RETURN(&AsyncData->Error, Ret);
    }
    return Ret;
}

/**
 *  The implementation of processing a fclose command.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CloseFile_FsClose(AMBA_CFS_STREAM_s *Stream)
{
    /* must be called with stream's mutex locked (to protect Status) */
    /* should not lock sch's mutex to avoid blocking other streams */
    UINT32 Ret = CFS_OK;

    if ((Stream->AlignMode == AMBA_CFS_ALIGN_MODE_SIZE) && (Stream->AlignSize != 0U) && (Stream->Length > Stream->RealLength)) {
        Ret = CFS_F2C(AmbaFS_FileSeek(Stream->File, (INT64)Stream->Length - 1, AMBA_FS_SEEK_START));
        if (Ret == CFS_OK) {
            UINT8 Buffer = 0U;
            UINT32 WriteCount = 0U;
            /* Write a byte of dummy data at the end */
            Ret = CFS_F2C(AmbaFS_FileWrite(&Buffer, 1, 1, Stream->File, &WriteCount));
            if (Ret == CFS_OK) {
                if (WriteCount != 1U) {
                    /*
                        WriteCount is not expected when "Ret == CFS_OK".
                        It's possible that I/O error occurred but AmbaFS_FileWrite return OK.
                        So use AmbaFS_GetError to determine error code.
                        If AmbaFS_GetError also returns OK, then return CFS_ERR_FATAL because the root cause is unknown.
                     */
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CloseFile: [ERROR] AmbaFS_FileWrite fail. WriteCount unexpected: %u ErrNum: %p", WriteCount, AmbaCFS_UtilGetAmbaFSError(), 0U, 0U, 0U);
                    Ret = CFS_F2C(AMBA_FS_ERR_API); /* Input AMBA_FS_ERR_API to run AmbaFS_GetError and convert it to CFS error */
                    if (Ret == CFS_OK) {
                        /* Should never happen. So return fatal error. */
                        Ret = CFS_ERR_FATAL;
                    }
                } else {
                    /* Success. Do nothing. */
                }
                if (Ret == CFS_OK) {
                    /* after each alignment, RealLength should be the same as the specified length */
                    Stream->RealLength = Stream->Length;
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CloseFile: [ERROR] AmbaFS_FileWrite fail. WriteCount: %u ErrNum: %p", WriteCount, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CloseFile: [ERROR] AmbaFS_FileSeek fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
        }
    }
    /* no need to unlock stream's mutex because stream can do nothing now */
    if (Ret == CFS_OK) {
        Ret = CFS_F2C(AmbaFS_FileClose(Stream->File));
        if ((Ret != CFS_OK) && (Ret != CFS_ERR_FS)) {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CloseFile: [ERROR] AmbaFS_FileClose fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
        }
    }
#ifdef AMBA_CFS_SCH_SLOW_TEST
    (void)AmbaKAL_TaskSleep(2000);
#endif

    return Ret;
}

/**
 *  The implementation of processing a fclose command and release resources.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] IsDiscarded         Whether the command is discarded. 0 - Normal code flow, 1 - Skip I/O and only release resources
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CloseFile(AMBA_CFS_STREAM_s *Stream, UINT32 IsDiscarded)
{
    /* must be called with stream's mutex locked (to protect Status) */
    /* should not lock sch's mutex to avoid blocking other streams */
    UINT32 Ret = CFS_OK;

    /*
     * I/O Error handling:
     *     In the following process, do not return immediately when getting I/O error.
     *     Instead, release as much resource as possible.
     *     So that the system can be recovered.
     */

    if (IsDiscarded == 0U) {
        Ret = CloseFile_FsClose(Stream);
    }

    /*
     * If the file is modified, remove cache to update file info.
     * If any error happened, the 'file info' cannot be trusted any more. Also remove cache.
     *
     * Theoretically, we should remove 'file info' but keep 'file search' because the file is still in FAT even if there's error.
     * However, the code will be complicated to distinguish 'file info' and 'file search'.
     * So we just call AmbaCFS_CacheRemove to remove all information.
     */
    if (Ret != CFS_ERR_FATAL) {
        /* If the file is not changed (read only) and there's no error, don't remove cache. */
        if ((Stream->Status != AMBA_CFS_STATUS_CLOSING_READ) || (Ret != CFS_OK)) {
            if (g_AmbaCFSSch.CacheEnable != 0U) {
                if (CFS_SET_RETURN(&Ret, AmbaCFS_CacheRemove(Stream->Filename)) != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    }

    /* Make sure all the resources are released */
    if ((Ret == CFS_OK) || (Ret == CFS_ERR_FS)) {
        const UINT8 StreamId = Stream->StreamId;
        const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
        if (AsyncData->NumBank != 0U) {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CloseFile: [ERROR] Remain %u Bank(s)", AsyncData->NumBank, 0U, 0U, 0U, 0U);
            Ret = CFS_ERR_API;
        }
    }

    /* Release all the pending Cmd/Bank */
    if ((Ret == CFS_OK) || (Ret == CFS_ERR_FS)) { /* Release CFS resourses even if CFS_ERR_FS occurred. */
        /*
         * I/O Error handling:
         *     1. Sync  FileClose: Do nothing because Cmd/Bank is not used
         *     2. ASync FileClose: Make sure to release all Cmd/Bank of this Stream
         *         Possible locations of Bank:
         *         1. Cmd->Bank : Search all the Cmd of the stream and call ReleaseBank().
         *         2. AsyncData->Bank : FlushReadBuffer() is already called in CmdProcess_Close(). To release all the banks in AsyncData->Bank back to AsyncData->BankPool.
         *         3. AsyncData->BankPool : Last step, call DeleteBankPool() to release all the banks in AsyncData->BankPool back to g_AmbaCFSSch.BankAvail.
         *         Possible locations of Cmd:
         *         1. Task->Cmd
         *         2. Task->CmdList
         *         When I/O Error, APP should call AmbaCFS_FileClose.
         *         Let SchTask process all the remain commands, skip ReadBank/WriteBank in the process but still release the resources.
         *         After Close command is done, the resources should all be released.
         */

        if (Stream->AsyncEnable != 0U) {
            if (CFS_SET_RETURN(&Ret, DeleteBankPool(Stream->StreamId)) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] DeleteBankPool fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }

    if ((Ret == CFS_OK) || (Ret == CFS_ERR_FS)) { /* Release CFS resourses even if CFS_ERR_FS occurred. */
        Stream->File = NULL;
        Stream->Status = AMBA_CFS_STATUS_UNUSED;
    } else {
        /*
            Stream was not closed normally. It can no longer be used.
            Set the Sataus to AMBA_CFS_STATUS_ERROR so that the stream will be skipped by GetFreeStream.
         */
        Stream->Status = AMBA_CFS_STATUS_ERROR;
    }

    /*
     * Trigger AMBA_CFS_EVENT_FCLOSE event after Stream->Status is set (meaning that CFS Hdlr is actually closed)
     */
    if ((Ret == CFS_OK) || (Ret == CFS_ERR_FS)) { /* Release CFS resourses even if CFS_ERR_FS occurred. */
        /* Trigger FCLOSE event */
        AMBA_CFS_EVENT_INFO_s Param;
        if (CFS_SET_RETURN(&Ret, CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)))) != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
        } else {
            Param.Event = AMBA_CFS_EVENT_FCLOSE;
            AmbaCFS_UtilCopyFileName(Param.Name, Stream->Filename);
            if (CFS_SET_RETURN(&Ret, AmbaCFS_AddEvent(&Param)) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }

    return Ret;
}

/**
 *  Process a fclose command.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Release Cmd and Close file.
 *  @param [in] Cmd                 The command
 *  @param [in] IsDiscarded         Whether the command is discarded. 0 - Normal code flow, 1 - Skip I/O and only release resources
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CmdProcess_Close(const AMBA_CFS_SCH_CMD_INFO_s *Cmd, UINT32 IsDiscarded)
{
    /* must be called with sch's mutex locked (but it is safe to use Stream->StreamId) */
    UINT32 Ret = CFS_OK;
    AMBA_CFS_STREAM_s * const Stream = Cmd->Stream;
    const UINT8 StreamId = Stream->StreamId;
    const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    if (AsyncData->Read != 0U) {  /* read mode, flush read buffer here */
        /* AmbaCFS_SchAsyncfclose() does not flush read buffer because to speed up async close (not to wait pending read command there) */
        Ret = FlushReadBuffer(StreamId);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FlushReadBuffer fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == CFS_OK) {
        const UINT32 Error = AsyncData->Error; /* Save error code before unlocking mutex */
        /* unlock sch's mutex because fclose is slow, must let other stream do async I/O */
        Ret = AmbaCFS_SchUnlock();
        if (Ret == CFS_OK) {
            /* it should be the last operation of this stream, no one can lock stream anymore, so it is safe to lock stream's mutex */
            Ret = CFS_K2C(AmbaKAL_MutexTake(&Stream->Mutex, AMBA_CFS_TIMEOUT_MUTEX));
            if (Ret == CFS_OK) {
                if (Error != CFS_OK) {
                    /* previous I/O error */
                    Stream->Status = AMBA_CFS_STATUS_ERROR;
                }

                /*
                 * I/O Error handling:
                 *     The previous process don't encounter I/O error. Start considering I/O error from now on.
                 *     If there's I/O error, still need to unlock mutex.
                 */

                Ret = CloseFile(Stream, IsDiscarded);
                if (CFS_SET_RETURN(&Ret, CFS_K2C(AmbaKAL_MutexGive(&Stream->Mutex))) != CFS_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CmdProcess_Close: [ERROR] AmbaKAL_MutexGive fail. Stream %u", Stream->StreamId, 0, 0, 0, 0);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CmdProcess_Close: [ERROR] AmbaKAL_MutexTake fail. Stream %u", Stream->StreamId, 0, 0, 0, 0);
            }
            if (Ret != CFS_ERR_FATAL) {
                if (CFS_SET_RETURN(&Ret, AmbaCFS_SchLock()) != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Process a command.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Release Cmd and Close file.
 *  @param [in] Cmd                 The command
 *  @param [in] IsDiscarded         Whether the command is discarded. 0 - Normal code flow, 1 - Skip I/O and only release resources
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CmdProcess(const AMBA_CFS_SCH_CMD_INFO_s *Cmd, UINT32 IsDiscarded)
{
    /* must be called with sch's mutex locked */
    /* do not lock stream's mutex to avoid dead lock (except close) */
    UINT32 Ret = CFS_ERR_API; /* Parameters should be checked already. If there's an error return API error. */
    if (Cmd->Stream != NULL) {
        if ((Cmd->Bank != NULL) || ((Cmd->Event != AMBA_CFS_EVENT_ASYNC_WRITE) && (Cmd->Event != AMBA_CFS_EVENT_ASYNC_READ))) {
            /*
             * I/O Error handling:
             *     Start considering I/O error from now on.
             *     If there's I/O error, invoke AMBA_CFS_EVENT_IO_ERROR event.
             */

            switch (Cmd->Event) {
            case AMBA_CFS_EVENT_ASYNC_WRITE:
                Ret = CmdProcess_Write(Cmd, IsDiscarded);
                break;
            case AMBA_CFS_EVENT_ASYNC_READ:
                Ret = CmdProcess_Read(Cmd, IsDiscarded);
                break;
            case AMBA_CFS_EVENT_ASYNC_CLOSE:
                Ret = CmdProcess_Close(Cmd, IsDiscarded);
                break;
            default:
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CmdProcess: [ERROR] Event %u not expected", Cmd->Event, 0, 0, 0, 0);
                break;
            }
            if (Ret == CFS_ERR_FS) {
                AMBA_CFS_EVENT_INFO_s Param;
                /* No need to handle the I/O errors that cause CFS_ERR_FATAL. */
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CmdProcess: [ERROR] Event %u failed", Cmd->Event, 0, 0, 0, 0);
                if (CFS_SET_RETURN(&Ret, CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)))) != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                } else {
                    Param.Event = AMBA_CFS_EVENT_IO_ERROR;
                    AmbaCFS_UtilCopyFileName(Param.Name, Cmd->Stream->Filename);
                    if (CFS_SET_RETURN(&Ret, AmbaCFS_AddEvent(&Param)) != CFS_OK) {
                        /* not a critical event, skip error handling */
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "CmdProcess: [ERROR] Event %u. Bank is NULL", Cmd->Event, 0, 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Stream is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get the command in command queue.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] File                CFS File descriptor
 *  @param [in] Event               CFS scheduler event. (ex: AMBA_CFS_EVENT_ASYNC_WRITE.)
 *  @param [out] Cmd                CFS command
 *  @return 0 - OK, others - Error
 */
static UINT32 GetCmd(const AMBA_CFS_FILE_s *File, UINT32 Event, AMBA_CFS_SCH_CMD_INFO_s **Cmd)
{
    /* must be called with sch's mutex locked */
    UINT32 Ret = CFS_ERR_ARG; /* When user process events in wrong sequence, this function might fail. So return ARG error. */
    AMBA_CFS_SCH_CMD_INFO_s * TempCmd = g_AmbaCFSSch.Task.CmdList;
    AMBA_CFS_SCH_CMD_INFO_s * PrevCmd = NULL;
    /* Search the commands in CmdList, remove and return the first match of File & Event. */
    while (TempCmd != NULL) {
        const AMBA_CFS_FILE_s *TempFile;
        AmbaMisra_TypeCast(&TempFile, &TempCmd->Stream);
        if (TempFile == File) {
            /* It should be the first command in the same stream. Otherwise the command sequence is wrong. */
            if (TempCmd->Event == Event) {
                /* Command is found. Remove it from CmdList. */
                if (PrevCmd != NULL) {
                    // Code Coverage: not the first command
                    PrevCmd->Next = TempCmd->Next;
                } else {
                    /* PrevCmd is NULL. TempCmd is the first one in CmdList. */
                    g_AmbaCFSSch.Task.CmdList = TempCmd->Next;
                }
                /* Return command */
                *Cmd = TempCmd;
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "GetCmd: [ERROR] Event %u not expected. Should be %u", Event, TempCmd->Event, 0, 0, 0);
            }
            break; /* End the loop */
        }
        PrevCmd = TempCmd;
        TempCmd = TempCmd->Next;
    }
    if (*Cmd == NULL) {
        ULONG FileUL;
        AmbaMisra_TypeCast(&FileUL, &File);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "GetCmd: [ERROR] Command not found. File %p Event %u", (UINT32)FileUL, Event, 0, 0, 0);
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  Process CFS event
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Close file.
 *  @param [in] File                CFS File descriptor
 *  @param [in] Event               CFS scheduler event. (ex: AMBA_CFS_EVENT_ASYNC_WRITE.)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_ProcessEventImpl(AMBA_CFS_FILE_s *File, UINT32 Event)
{
    UINT32 Ret;
    AMBA_CFS_SCH_CMD_INFO_s *Cmd = NULL;
    /* Search command */
    Ret = GetCmd(File, Event, &Cmd);
    if (Ret == CFS_OK) {
        // TODO: Check g_AmbaCFSSch.Task.Cmd should be NULL
        g_AmbaCFSSch.Task.Cmd = Cmd;
        /*
         * I/O Error handling:
         *     The previous process don't encounter I/O error. Start considering I/O error from now on.
         *     Make sure ReleaseCmd is called when there's I/O error.
         */

        /* Process command */
        Ret = CmdProcess(Cmd, 0U);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CmdProcess fail", __func__, NULL, NULL, NULL, NULL);
        }
        /* Release command */
        if (Ret != CFS_ERR_FATAL) {
            if (CFS_SET_RETURN(&Ret, ReleaseCmd(Cmd)) != CFS_OK) {
                /* fatal error, no error handling */
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseCmd fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
        g_AmbaCFSSch.Task.Cmd = NULL;
    } else {
        /* fatal error, no error handling */
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] GetCmd fail", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  Check parameters.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] File                CFS File descriptor
 *  @param [in] Event               CFS scheduler event. (ex: AMBA_CFS_EVENT_ASYNC_WRITE.)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_ProcessEvent_ParamCheck(const AMBA_CFS_FILE_s *File, UINT32 Event)
{
    UINT32 Ret = CFS_ERR_ARG;
    if ((Event == AMBA_CFS_EVENT_ASYNC_WRITE) || (Event == AMBA_CFS_EVENT_ASYNC_READ) || (Event == AMBA_CFS_EVENT_ASYNC_CLOSE)) {
        if (AmbaCFS_IsValidCfsStream(File) != 0U) {
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_ProcessEvent_ParamCheck: [ERROR] Event %u not expected", Event, 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Process CFS event.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Close file.
 *  @param [in] File                CFS File descriptor
 *  @param [in] Event               CFS scheduler event. (ex: AMBA_CFS_EVENT_ASYNC_WRITE.)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ProcessEvent(AMBA_CFS_FILE_s *File, UINT32 Event)
{
    UINT32 Ret;
    Ret = AmbaCFS_ProcessEvent_ParamCheck(File, Event);
    if (Ret == CFS_OK) {
        /* Get Scheduler Mutex */
        Ret = AmbaCFS_SchLock();
        if (Ret == CFS_OK) {
            /*
             * I/O Error handling:
             *     The previous process don't encounter I/O error. Start considering I/O error from now on.
             *     Make sure AmbaCFS_SchUnlock is called when there's I/O error.
             */

            Ret = AmbaCFS_ProcessEventImpl(File, Event);
            if (CFS_SET_RETURN(&Ret, AmbaCFS_SchUnlock()) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Discard CFS event
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Close file.
 *  @param [in] File                CFS File descriptor
 *  @param [in] Event               CFS scheduler event. (ex: AMBA_CFS_EVENT_ASYNC_WRITE.)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_DiscardEventImpl(AMBA_CFS_FILE_s *File, UINT32 Event)
{
    UINT32 Ret;
    AMBA_CFS_SCH_CMD_INFO_s *Cmd = NULL;
    /* Search command */
    Ret = GetCmd(File, Event, &Cmd);
    if (Ret == CFS_OK) {
        // TODO: Check g_AmbaCFSSch.Task.Cmd should be NULL
        g_AmbaCFSSch.Task.Cmd = Cmd;
        /*
         * I/O Error handling:
         *     The previous process don't encounter I/O error. Start considering I/O error from now on.
         *     Make sure ReleaseCmd is called when there's I/O error.
         */

        /* Discard command */
        Ret = CmdProcess(Cmd, 1U);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CmdProcess fail", __func__, NULL, NULL, NULL, NULL);
        }
        /* Release command */
        if (Ret != CFS_ERR_FATAL) {
            if (CFS_SET_RETURN(&Ret, ReleaseCmd(Cmd)) != CFS_OK) {
                /* fatal error, no error handling */
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseCmd fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
        g_AmbaCFSSch.Task.Cmd = NULL;
    } else {
        /* fatal error, no error handling */
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] GetCmd fail", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  Check parameters.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] File                CFS File descriptor
 *  @param [in] Event               CFS scheduler event. (ex: AMBA_CFS_EVENT_ASYNC_WRITE.)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_DiscardEvent_ParamCheck(const AMBA_CFS_FILE_s *File, UINT32 Event)
{
    UINT32 Ret = CFS_ERR_ARG;
    if ((Event == AMBA_CFS_EVENT_ASYNC_WRITE) || (Event == AMBA_CFS_EVENT_ASYNC_READ) || (Event == AMBA_CFS_EVENT_ASYNC_CLOSE)) {
        if (AmbaCFS_IsValidCfsStream(File) != 0U) {
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_DiscardEvent_ParamCheck: [ERROR] Event %u not expected", Event, 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Discard CFS event.
 *  When emergency stop, user can call AmbaCFS_DiscardEvent instead of AmbaCFS_ProcessEvent for AMBA_CFS_EVENT_ASYNC_WRITE and AMBA_CFS_EVENT_ASYNC_READ.
 *  The read/write will be skipped to stop quickly but the data might be corrupted.
 *  It's still suggested to call AmbaCFS_ProcessEvent for AMBA_CFS_EVENT_ASYNC_CLOSE to close file correctly.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Close file.
 *  @param [in] File                CFS File descriptor
 *  @param [in] Event               CFS scheduler event. (ex: AMBA_CFS_EVENT_ASYNC_WRITE.)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_DiscardEvent(AMBA_CFS_FILE_s *File, UINT32 Event)
{
    UINT32 Ret;
    Ret = AmbaCFS_DiscardEvent_ParamCheck(File, Event);
    if (Ret == CFS_OK) {
        /* Get Scheduler Mutex */
        Ret = AmbaCFS_SchLock();
        if (Ret == CFS_OK) {
            /*
             * I/O Error handling:
             *     The previous process don't encounter I/O error. Start considering I/O error from now on.
             *     Make sure AmbaCFS_SchUnlock is called when there's I/O error.
             */

            Ret = AmbaCFS_DiscardEventImpl(File, Event);
            if (CFS_SET_RETURN(&Ret, AmbaCFS_SchUnlock()) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Add a fwrite command to command list.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AddWriteCmd(AMBA_CFS_STREAM_s *Stream)
{
    /* must be called with sch/stream mutex locked */
    /* always be called after Async->Error is checked, no need to check again */
    UINT32 Ret;
    const UINT8 StreamId = Stream->StreamId;
    AMBA_CFS_SCH_CMD_INFO_s *Cmd = NULL;
    Ret = AllocateCmd(AMBA_CFS_EVENT_ASYNC_WRITE, Stream, StreamId, Stream->Length, &Cmd);
    if (Ret == CFS_OK) {
        if (Stream->Status != AMBA_CFS_STATUS_ERROR) {
            AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
            /* Insert command */
            Ret = PushCmd(Cmd, AsyncData->Bank);
            if (Ret == CFS_OK) {
                /* set active bank as NULL */
                AsyncData->Bank = NULL;
            } else {
                /* fatal error, no error handling */
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] PushCmd fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AddWriteCmd: [ERROR] Status %u", Stream->Status, 0, 0, 0, 0);
            Ret = CFS_ERR_API;
        }
    } else {
        /* fatal error, no error handling */
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AllocateCmd fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Add a fread command to command list. (could fail, must have error handler)
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Cmd and Bank are not allocated when there's I/O Error.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Pos                 The read position
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AddReadCmd(AMBA_CFS_STREAM_s *Stream, UINT64 Pos)
{
    /* must be called with sch/stream mutex locked */
    /* always be called after Async->Error is checked, no need to check again */
    UINT32 Ret;
    const UINT8 StreamId = Stream->StreamId;
    const UINT64 Length = Stream->Length;
    AMBA_CFS_SCH_CMD_INFO_s *Cmd = NULL;
    Ret = AllocateCmd(AMBA_CFS_EVENT_ASYNC_READ, Stream, StreamId, Length, &Cmd);
    if (Ret == CFS_OK) {
        if (Stream->Status != AMBA_CFS_STATUS_ERROR) {
            AMBA_CFS_SCH_BANK_INFO_s *Bank;
            /*
             * I/O Error handling:
             *     Start considering I/O error from now on.
             *     The only function that could return I/O Error is AllocateBank.
             *     If there's I/O error, release Cmd and return error.
             */

            Ret = AllocateBank(StreamId, &Bank);
            if (Ret == CFS_OK) {
                Bank->Pos = Pos;
                Bank->Length = g_AmbaCFSSch.BankSize;
                if ((Bank->Pos + Bank->Length) > Length) {
                    UINT64 LengthTemp = Length - Bank->Pos;
                    Bank->Length = (UINT32) LengthTemp;
                }
                /* Insert command */
                Ret = PushCmd(Cmd, Bank);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] PushCmd fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                /* allocate bank failed, release cmd on error */
                if (ReleaseCmd(Cmd) != CFS_OK) {
                    /* fatal error, ignore error handling */
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseCmd fail", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_FATAL;
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AddReadCmd: [ERROR] Status %u", Stream->Status, 0, 0, 0, 0);
            Ret = CFS_ERR_API;
        }
    } else {
        /* fatal error, ignore error handling */
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AllocateCmd fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Add a fclose command to command list (could fail, must have error handler)
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AddCloseCmd(AMBA_CFS_STREAM_s *Stream)
{
    /* must be called with sch/stream mutex locked */
    UINT32 Ret;
    AMBA_CFS_SCH_CMD_INFO_s *Cmd = NULL;
    Ret = AllocateCmd(AMBA_CFS_EVENT_ASYNC_CLOSE, Stream, Stream->StreamId, Stream->Length, &Cmd);
    if (Ret == CFS_OK) {
        /* Insert command */
        Ret = PushCmd(Cmd, NULL);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] PushCmd fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AllocateCmd fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/*
 * Asynchronous mode implementation
 */

/*
 * Scheduler APIs
 */

UINT32 AmbaCFS_SchGetInitBufferSize(UINT32 CmdAmount)
{
    return (CmdAmount * ((UINT32)sizeof(AMBA_CFS_SCH_BANK_INFO_s) + (UINT32)sizeof(AMBA_CFS_SCH_CMD_INFO_s)));
}

/**
 *  Get default config for scheduling module.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Config              The config record
 */
void AmbaCFS_SchGetInitDefaultCfg(AMBA_CFS_INIT_CFG_s *Config)
{
    Config->CmdAmount    = AMBA_CFS_SCH_CMD_AMOUNT_DEFAULT;
    Config->BankSize     = AMBA_CFS_SCH_BANK_SIZE_DEFAULT;
}

/**
 *  Create scheuler flags
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CreateFlags(void)
{
    UINT32 Ret;
    static char CfsSchFlag[16] = "CfsSchFlag";
    Ret = CFS_K2C(AmbaKAL_EventFlagCreate(&g_AmbaCFSSch.SchFlg, CfsSchFlag));
    if (Ret == CFS_OK) {
        static char CfsAwdFlag[16] = "CfsAwdFlag";
        Ret = CFS_K2C(AmbaKAL_EventFlagCreate(&g_AmbaCFSSch.AwdFlg, CfsAwdFlag));
        if (Ret == CFS_OK) {
            static char CfsArdFlag[16] = "CfsArdFlag";
            Ret = CFS_K2C(AmbaKAL_EventFlagCreate(&g_AmbaCFSSch.ArdFlg, CfsArdFlag));
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Can not create ArdFlg", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Can not create AwdFlg", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Can not create SchFlg", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check initialization config.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Buffer              Buffer address
 *  @param [in] BufferSize          Buffer size
 *  @param [in] CmdAmount           The command amount
 *  @param [in] BankSize            The bank size
 *  @param [in] CheckCached         The callback function to check whether the buffer is cached or not
 *  @param [in] CacheClean          The callback function to clean data cache
 *  @param [in] CacheInvalidate     The callback function to invalidate data cache
 *  @param [in] VirtToPhys          The callback function to convert virtual memory address to physical address
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_SchInit_ParamCheck(const UINT8 *Buffer, UINT32 BufferSize, UINT32 CmdAmount, UINT32 BankSize,
    AMBA_CFS_CHECK_CACHED_f CheckCached, AMBA_CFS_CACHE_CLEAN_f CacheClean, AMBA_CFS_CACHE_INVALIDATE_f CacheInvalidate, AMBA_CFS_VIRT_TO_PHYS_f VirtToPhys)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (Buffer != NULL) {
        if (CmdAmount >= AMBA_CFS_SCH_CMD_AMOUNT_MIN) {
            if ((BankSize > 0U) && ((BankSize % AMBA_CACHE_LINE_SIZE) == 0U)) {
                if (BufferSize == AmbaCFS_SchGetInitBufferSize(CmdAmount)) {
                    if (CheckCached != NULL) {
                        if (CacheClean != NULL) {
                            if (CacheInvalidate != NULL) {
                                if (VirtToPhys != NULL) {
                                    Ret = CFS_OK;
                                } else {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] VirtToPhys is NULL", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CacheInvalidate is NULL", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CacheClean is NULL", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckCached is NULL", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] BufferSize is incorrect", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchInit_Check: [ERROR] BankSize %u is invalid", BankSize, 0, 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchInit_Check: [ERROR] CmdAmount %u is not supported", CmdAmount, 0, 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Buffer is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

UINT32 AmbaCFS_SchInit(UINT8 *Buffer, UINT32 BufferSize, UINT32 CmdAmount, UINT32 BankSize, UINT8 CacheEnable,
    AMBA_CFS_CHECK_CACHED_f CheckCached, AMBA_CFS_CACHE_CLEAN_f CacheClean, AMBA_CFS_CACHE_INVALIDATE_f CacheInvalidate, AMBA_CFS_VIRT_TO_PHYS_f VirtToPhys)
{
    static UINT8 g_AmbaCFSSchInit = 0U;
    UINT32 Ret;
    Ret = AmbaCFS_SchInit_ParamCheck(Buffer, BufferSize, CmdAmount, BankSize, CheckCached, CacheClean, CacheInvalidate, VirtToPhys);
    if (Ret == CFS_OK) {
        if (g_AmbaCFSSchInit == 0U) {
            g_AmbaCFSSchInit = 1U;
            Ret = CFS_WRAP2C(AmbaWrap_memset(&g_AmbaCFSSch, 0, sizeof(g_AmbaCFSSch)));
            if (Ret == CFS_OK) {
                /* Initialize Schduler */
                static char CfsSchMutex[16] = "CfsSchMutex";
                Ret = CFS_K2C(AmbaKAL_MutexCreate(&g_AmbaCFSSch.Mutex, CfsSchMutex));
                if (Ret == CFS_OK) {
                    Ret = CreateFlags();
                    if (Ret == CFS_OK) {
                        UINT32 i;
                        /*
                         * The sufficient number of CmdAmount is:
                         *     CmdAmount = BankAmount + AMBA_CFS_MAX_STREAM.
                         * The case that we need the most commands:
                         *     1. All Async Read/Write commands: Each bank needs one command, so the number of commands is "BankAmount"
                         *     2. All streams close at the same time: Each stream needs one command, so the number of commands is "AMBA_CFS_MAX_STREAM"
                         *
                         * Set sufficient number of BankAmount to make sure that Bank info is always enough.
                         *     (Number of Bank info) >= CmdAmount - AMBA_CFS_MAX_STREAM
                         */
                        UINT8 *BufferTemp = Buffer;
                        /* Assign bank data */
                        g_AmbaCFSSch.BankSize = BankSize;
                        /* Assign and set bank info */
                        AmbaMisra_TypeCast(&g_AmbaCFSSch.BankAvail, &BufferTemp);
                        for (i = 0U; i < (CmdAmount - 1U); i++) {
                            g_AmbaCFSSch.BankAvail[i].Buffer = NULL;
                            g_AmbaCFSSch.BankAvail[i].Next = &g_AmbaCFSSch.BankAvail[i + 1U];
                        }
                        g_AmbaCFSSch.BankAvail[i].Buffer = NULL;
                        g_AmbaCFSSch.BankAvail[i].Next = NULL;
                        /* BufferTemp += BankAmount * sizeof(AMBA_CFS_SCH_BANK_INFO_s) */
                        BufferTemp = &BufferTemp[CmdAmount * sizeof(AMBA_CFS_SCH_BANK_INFO_s)];
                        /* Assign and set Cmd buffer */
                        AmbaMisra_TypeCast(&g_AmbaCFSSch.CmdAvail, &BufferTemp);
                        for (i = 0U; i < (CmdAmount - 1U); i++) {
                            g_AmbaCFSSch.CmdAvail[i].Next = &g_AmbaCFSSch.CmdAvail[i + 1U];
                        }
                        g_AmbaCFSSch.CmdAvail[i].Next = NULL;
                        /* NOTE: Update BufferTemp if it's used in the further process */
                        /* BufferTemp += (UINT32)CmdAmount * sizeof(AMBA_CFS_SCH_CMD_INFO_s); */
                        /* Set cache enable */
                        g_AmbaCFSSch.CacheEnable = CacheEnable;
                        g_AmbaCFSSch.CheckCached = CheckCached;
                        g_AmbaCFSSch.CacheClean = CacheClean;
                        g_AmbaCFSSch.CacheInvalidate = CacheInvalidate;
                        g_AmbaCFSSch.VirtToPhys = VirtToPhys;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexCreate fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }
    return Ret;
}

/**
 *  The implemenation of synchronous fread.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. APP should close file when getting CFS_ERR_FS.
 *  @param [in] Buffer              Buffer address
 *  @param [in] Size                Size of a record
 *  @param [in] Count               Number of records
 *  @param [in] Stream              The CFS stream
 *  @param [out] Result             Number of records that are read
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchSyncfreadImpl(UINT8 *Buffer, UINT32 Size, UINT32 Count, const AMBA_CFS_STREAM_s *Stream, UINT32 *Result)
{
    /* must be called with stream's mutex locked */
    UINT32 Ret = CFS_OK;
    UINT64 ByteLeft = (UINT64) Size * Count;
    UINT64 CurPos = Stream->Pos;
    UINT8 *BufferTemp = Buffer;
    /* While has data to read */
    while (ByteLeft > 0ULL) {
        UINT32 Cnt = 0U;
        UINT32 Len = g_AmbaCFSSch.BankSize;
        AMBA_CFS_SCH_BANK_INFO_s Bank;
        UINT8 IsBreakLoop = 0U;
        /* Get count per read */
        if (ByteLeft < (UINT64) Len) {
            Len = (UINT32) ByteLeft;
        }
        /* Prepare command */
        Bank.Buffer = BufferTemp;
        Bank.Pos = CurPos;
        Bank.Length = Len;
        Bank.Next = NULL;

        /*
         * I/O Error handling:
         *     The previous process don't encounter I/O error. Start considering I/O error from now on.
         *     If there's I/O error, return immediately.
         *     'Bank' is a local variable. Don't release it.
         */

        /* Read data */
        Ret = ReadBank(&Bank, Stream->File, &Cnt);
        if (Ret == CFS_OK) {
            ByteLeft -= Cnt;
            BufferTemp = &BufferTemp[Cnt]; /* BufferTemp += Cnt */
            CurPos += Cnt;
            if (Cnt != Len) {
                /* Check EOF. ByteLeft may not be zero (due to overread). */
                if (CurPos != Stream->Length) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "AmbaCFS_SchSyncfreadImpl: [ERROR] FileName: %s", Stream->Filename, NULL, NULL, NULL, NULL);
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchSyncfreadImpl: [ERROR] Read Size: %u, Cur: %u, Len: %u, ErrNum: %p", Cnt, (UINT32)CurPos, (UINT32)Stream->Length, AmbaCFS_UtilGetAmbaFSError(), 0);
                    Ret = CFS_ERR_FS;
                }
                IsBreakLoop = 1U;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReadBank fail", __func__, NULL, NULL, NULL, NULL);
        }

        if ((Ret != CFS_OK) || (IsBreakLoop != 0U)) {
            break;
        }
    }
    /* Set return value */
    if (Ret != CFS_ERR_FATAL) {
        UINT64 LeftCount = (ByteLeft + Size - 1ULL) / Size;
        *Result = Count - (UINT32) LeftCount;
    }
    return Ret;
}

/**
 *  Synchronous fread.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. APP should close file when getting CFS_ERR_FS.
 *  @param [in] Buffer              Buffer address
 *  @param [in] Size                Size of a record
 *  @param [in] Count               Number of records
 *  @param [in] Stream              The CFS stream
 *  @param [out] NumSuccess         The returned number of records read
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchSyncfread(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchSyncfreadImpl(Buffer, Size, Count, Stream, NumSuccess);
    if (Ret != CFS_OK) {
        Stream->Status = AMBA_CFS_STATUS_ERROR;
    }
    return Ret;
}

/**
 *  Read data from bank.
 *  Read from current available banks. Won't wait for pending read commands.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Buffer              Buffer address
 *  @param [in] Pos                 File position
 *  @param [in] Length              Data length
 *  @param [out] Result             Number of records that are read
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 ReadFromBank(const AMBA_CFS_STREAM_s *Stream, UINT8 *Buffer, UINT64 Pos, UINT64 Length, UINT64 *Result)
{
    /* must be called with stream/sch mutex locked */
    UINT32 Ret = CFS_OK;
    UINT64 Count = 0ULL;
    const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[Stream->StreamId];
    const AMBA_CFS_SCH_BANK_INFO_s *Bank = AsyncData->Bank;
    UINT64 RemainLength = Length;
    UINT64 CurPos = Pos;

    /* check bank list, read available (bank data is connected) */
    while (Bank != NULL) {
        UINT8 IsBreakLoop = 0U;
        if (Bank->Length > 0U) {
            if (Bank->Buffer != NULL) {
                if (Bank->Pos > CurPos) {
                    /*
                     * Could only happen at the first bank
                     * It happens after the following steps:
                     * 1. Read some data
                     * 2. Seek to a position prior to the first bank
                     * 3. Read data
                     * In this case, ReadFromBank() will get a return value = OK and Result = 0.
                     * User will handle this case in AmbaCFS_SchAsyncfreadImpl(), so break the loop from here.
                     */
                    if (Bank != AsyncData->Bank) {
                        // Code Coverage: data not consecutive, or memory corruption
                        /*
                         * It happens in the following conditions (which is unexpected):
                         * 1. PrevBank->Pos < CurPos
                         * 2. (PrevBank->Pos + PrevBank->Length) <= CurPos
                         * 3. Bank->Pos > CurPos
                         * So the data is not consecutive between PrevBank and Bank.
                         */
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Bank inconsistent", __func__, NULL, NULL, NULL, NULL);
                        Ret = CFS_ERR_API;
                    }
                    IsBreakLoop = 1U;
                } else {
                    if ((Bank->Pos + Bank->Length) > CurPos) {
                        // Code Coverage (Yellow): The required data is found in Bank
                        UINT32 Len = (UINT32)(Bank->Pos + Bank->Length - CurPos);
                        if ((UINT64) Len > RemainLength) {
                            // Code Coverage (Yellow): will not read the whole bank
                            Len = (UINT32) RemainLength;
                        }
                        {
                            /* AmbaWrap_memcpy(Buffer + Count, Bank->Buffer + (CurPos - Bank->Pos), Len) */
                            UINT8 *DstBuffer = &Buffer[Count];
                            UINT64 SrcOffset = CurPos - Bank->Pos;
                            UINT8 *SrcBuffer = &Bank->Buffer[SrcOffset];
                            AMBA_CFS_COPY_INFO_s CopyInfo = {0};

                            Ret = AmbaCFS_SchCopySetSource(&CopyInfo, SrcBuffer);
                            if (Ret == CFS_OK) {
                                Ret = AmbaCFS_SchCopySetDestination(&CopyInfo, DstBuffer, Len, Stream);
                                if (Ret == CFS_OK) {
                                    Ret = AmbaCFS_SchCopyMemory(&CopyInfo, Stream);
                                    if (Ret != CFS_OK) {
                                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchCopyMemory fail", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchCopySetDestination fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchCopySetSource fail", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                        if (Ret == CFS_OK) {
                            RemainLength -= Len;
                            Count += Len;
                            CurPos += Len;
                            if ((RemainLength == 0ULL) || (CurPos == Stream->Length)) {
                                // Code Coverage (Yellow): read full or reach EOS
                                /* read full or reach EOS, break */
                                IsBreakLoop = 1U;
                            }
                        }
                    }
                    if ((Ret == CFS_OK) && (IsBreakLoop == 0U)) {
                        Bank = Bank->Next;
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Buffer is NULL", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_API;
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "ReadFromBankImpl: [ERROR] Length %u", Bank->Length, 0, 0, 0, 0);
            Ret = CFS_ERR_API;
        }

        if ((Ret != CFS_OK) || (IsBreakLoop != 0U)) {
            // Code Coverage (Yellow): error or done or need to read from prior position
            break;
        }
    }
    /* Set return value */
    if (Ret == CFS_OK) { /* Theoretically, should also allow Ret == CFS_ERR_FS. However, Ret won't be CFS_ERR_FS in this function, so skip the check. */
        *Result = Count;
    }
    return Ret;
}

/**
 *  Check if there's a pending bank to READ at postion Pos
 *  If Sch Task is waiting to READ data at Pos, then return pending. Otherwise, it's not pending.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @param [in] Pos                 File position
 *  @param [out] IsPending          Bank is pending or not
 *  @return 0 - OK, others - Error
 */
static UINT32 BankIsPending(UINT8 StreamId, UINT64 Pos, UINT8 *IsPending)
{
    /* must be called with sch mutex locked */
    UINT32 Ret = CFS_OK;
    UINT8 Pending = 0U;
    const AMBA_CFS_SCH_TASK_INFO_s * const Task = &g_AmbaCFSSch.Task;
    const AMBA_CFS_SCH_CMD_INFO_s *Cmd = Task->Cmd;
    if (Cmd != NULL) {
        if ((Cmd->StreamId == StreamId) && (Cmd->Event == AMBA_CFS_EVENT_ASYNC_READ)) {
            const AMBA_CFS_SCH_BANK_INFO_s *Bank = Cmd->Bank;
            if (Bank != NULL) {
                if ((Bank->Pos <= Pos) && ((Bank->Pos + Bank->Length) > Pos)) {
                    Pending = 1U;
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Bank is NULL", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_API;
            }
        }
    }
    if (Pending == 0U) {
        Cmd = Task->CmdList;
        while (Cmd != NULL) {
            UINT8 IsBreakLoop = 0U;
            if ((Cmd->StreamId == StreamId) && (Cmd->Event == AMBA_CFS_EVENT_ASYNC_READ)) {
                const AMBA_CFS_SCH_BANK_INFO_s *Bank = Cmd->Bank;
                if (Bank != NULL) {
                    if ((Bank->Pos <= Pos) && ((Bank->Pos + Bank->Length) > Pos)) {
                        Pending = 1U;
                        IsBreakLoop = 1U;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Bank is NULL", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_API;
                    IsBreakLoop = 1U;
                }
            }

            if (IsBreakLoop != 0U) {
                break;
            }
            Cmd = Cmd->Next;
        }
    }
    /* Set return value */
    *IsPending = Pending;
    return Ret;
}

/**
 *  Wait pending write commands
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. In this case, there're no more pending write commands.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 WaitPendingWrite(AMBA_CFS_STREAM_s *Stream)
{
    /* must be called with sch/stream mutex locked */
    /* always be called after Async->Error is checked, no need to check again */
    UINT32 Ret = CFS_OK;
    const UINT8 StreamId = Stream->StreamId;
    const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    if (AsyncData->Bank != NULL) {
        Ret = AddWriteCmd(Stream);
    }
    if (Ret == CFS_OK) {
        for ( ; ; ) { /* Infinite loop */
            const AMBA_CFS_SCH_TASK_INFO_s * const Task = &g_AmbaCFSSch.Task;
            const AMBA_CFS_SCH_CMD_INFO_s *Cmd = Task->Cmd;
            /*
             * Check Task->Cmd and all the commands in Task->CmdList for write commands of StreamId.
             * If the command is found, Cmd is not NULL.
             * Otherwise, Cmd is NULL and we can stop waiting.
             */
            if ((Cmd == NULL) || (Cmd->StreamId != StreamId)) {
                /* no current command, or current command is not for stream, check pending commands */
                Cmd = Task->CmdList;
                while (Cmd != NULL) {
                    if (Cmd->StreamId == StreamId) {
                        /* skip the check on Event, because they should be write commands */
                        break;
                    }
                    Cmd = Cmd->Next;
                }
            }
            /* If Cmd is found, wait AWriteDone. */
            if (Cmd != NULL) {
                Ret = AmbaCFS_SchClearAWriteDone(StreamId);
                if (Ret == CFS_OK) {
                    Ret = AmbaCFS_SchUnlock();
                    if (Ret == CFS_OK) {
                        Ret = AmbaCFS_SchTakeAWriteDone(StreamId);
                        if (Ret != CFS_ERR_FATAL) {
                            if (AmbaCFS_SchLock() != CFS_OK) {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
                                Ret = CFS_ERR_FATAL;
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                /* No pending write command */
            }

            /* Check error during waiting. */
            if (Ret == CFS_OK) {
                /*
                 * I/O Error handling:
                 *     The previous process don't encounter I/O error. Start considering I/O error from now on.
                 *     If there's I/O error, keep waiting AWriteDone because Sch Task will release resources
                 *     and call AmbaCFS_SchGiveAWriteDone during I/O Error.
                 *     In the end, the new command added in AddWriteCmd will also be consumed.
                 */

                /* Check if any error happened during previous async I/O */
                Ret = CheckScheduler(AsyncData);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
                }
            }

            /*
             * Break the loop in either of the following cases:
             * 1. No more pending write commands for StreamId
             * 2. Got serious error (except I/O Error) while waiting
             */
            if ((Cmd == NULL) ||
                ((Ret != CFS_OK) && (Ret != CFS_ERR_FS))) {
                break;
            }
        }
    } else {
        /* fatal error, no error handling */
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AddWriteCmd fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Wait pending read commands
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. In this case, the data at position Pos is not ready.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Pos                 Stream Position
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 WaitPendingRead(const AMBA_CFS_STREAM_s *Stream, UINT64 Pos)
{
    /* must be called with sch/stream mutex locked */
    UINT32 Ret = CFS_OK;
    UINT8 IsPending = 0U;
    /* data not complete, wait pending read commands */
    do {
        const UINT8 StreamId = Stream->StreamId;
        /*
         * I/O Error handling:
         *     Start considering I/O error from now on.
         *     WaitPendingRead is only used by AmbaCFS_SchAsyncfreadImpl while waiting Sch Task to read data in Banks.
         *     If there's I/O error, there's no point reading the data, so return error immediately.
         */

        Ret = AmbaCFS_SchWaitAsyncReadDone(StreamId);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchWaitAsyncReadDone fail", __func__, NULL, NULL, NULL, NULL);
        } else {
            Ret = BankIsPending(StreamId, Pos, &IsPending);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] BankIsPending fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        if (Ret != CFS_OK) {
            break;
        }
    } while (IsPending != 0U);
    return Ret;
}

/**
 *  Start new asynchronous read session
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. Some Read commands might be sent to Sch Task. Let Sch Task consume the rest commands.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Pos                 The read position
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 StartAsyncRead(AMBA_CFS_STREAM_s *Stream, UINT64 Pos)
{
    /* must be called with sch/stream mutex locked */
    /* always be called after Async->Error is checked, no need to check again */
    UINT32 Ret = CFS_ERR_API;
    if (Stream->Status != AMBA_CFS_STATUS_ERROR) {
        const UINT8 StreamId = Stream->StreamId;
        /*
         * I/O Error handling:
         *     Start considering I/O error from now on.
         *     If there's I/O error from FlushReadCmd, return immediately.
         *     If there's I/O error from AddReadCmd, return immediately. Let Sch Task consume the rest commands because the event callback is already called.
         */

        /* remove all banks and all pending commands */
        /* Might return CFS_ERR_FS because FlushBuffer = 1 */
        Ret = FlushReadCmd(StreamId, 1U);
        if (Ret == CFS_OK) {
            const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
            UINT64 TempPos = Pos;
            /* send cmd to read data to bank */
            for (UINT32 i = 0U; i < AsyncData->MaxNumBank; i++) {
                UINT8 IsBreakLoop = 0U;
                Ret = AddReadCmd(Stream, TempPos);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AddReadCmd fail", __func__, NULL, NULL, NULL, NULL);
                    IsBreakLoop = 1U;
                } else {
                    TempPos += g_AmbaCFSSch.BankSize;
                    if (TempPos >= Stream->Length) {
                        IsBreakLoop = 1U;
                    }
                }

                if (IsBreakLoop == 1U) {
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FlushReadCmd fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "StartAsyncRead: [ERROR] Status %u", Stream->Status, 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Switch to asynchronous read
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. The stream might still be in Write mode. There might be pending Write Cmds.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 SwitchToAsyncRead(AMBA_CFS_STREAM_s *Stream)
{
    /* must be called with sch/stream mutex locked */
    UINT32 Ret;
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[Stream->StreamId];
    /*
     * I/O Error handling:
     *     Start considering I/O error from now on.
     *     WaitPendingRead is only used by AmbaCFS_SchAsyncfreadImpl.
     *     If there's I/O error, there's no point reading the data, so return error immediately.
     *     If there's I/O error from CheckScheduler, the stream might still be in Write mode. There might be pending Write Cmds.
     *     If there's I/O error from WaitPendingWrite, the stream might still be in Write mode. There will be no more pending Write Cmds.
     */

    /* check if any error happens during previous async I/O */
    Ret = CheckScheduler(AsyncData);
    if (Ret == CFS_OK) {
        if (AsyncData->Read == 0U) {
            Ret = WaitPendingWrite(Stream);
            if (Ret == CFS_OK) {
                AsyncData->Read = 1U;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] WaitPendingWrite fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Prefetch the next bank when needed.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] CurPos              Current position of read
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 PrefetchBank(AMBA_CFS_STREAM_s *Stream, UINT64 CurPos)
{
    /* called with stream/sch mutex locked */
    UINT32 Ret = CFS_OK;
    const UINT8 StreamId = Stream->StreamId;
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    const AMBA_CFS_SCH_BANK_INFO_s *Bank = AsyncData->Bank;
    UINT32 BankCount;

    /* Get the last bank and count the current number of banks */
    /* No need to check (Bank != NULL) because ReadFromBank succeeded */
    BankCount = 1U;
    while (Bank->Next != NULL) {
        Bank = Bank->Next;
        BankCount++;
    }
    /*
     * Decide whether to prefetch the next bank.
     * The current rule is to prefetch the next bank under the following conditions:
     * 1. All banks are used. (No pending read command.)
     *    Check this condition by (AsyncData->MaxNumBank == BankCount)
     * 2. CurPos has reached the last bank.
     *    Check this condition by (Bank->Pos <= CurPos)
     *    This condition can ba changed. For example, when CurPos passes 50% of the banks.
     *
     * Code Coverage: Consider the following cases:
     * 1. There exists pending read command. CurPos has not reached the last bank.
     *    Could happen at the start of a read session. If ByteLeft != 0, wait pending read command.
     * 2. There exists pending read command. CurPos has reached the last bank.
     *    Could happen at the start of a read session. If ByteLeft != 0, wait pending read command.
     * 3. There's no pending read commands.  CurPos has not reached the last bank.
     *    Could happen if read is completed (ByteLeft == 0). Do nothing and return.
     * 4. There's no pending read commands.  CurPos has reached the last bank.
     *    Could happen after reading the second last bank. Prefetch next bank. If ByteLeft != 0, wait pending read command.
     * As a result, we only need to prefetch bank when (Bank->Pos <= CurPos) && (AsyncData->MaxNumBank == BankCount).
     */
    if ((Bank->Pos <= CurPos) && (AsyncData->MaxNumBank == BankCount)) {
        AMBA_CFS_SCH_BANK_INFO_s * const TmpBank = AsyncData->Bank;
        AsyncData->Bank = TmpBank->Next;
        Ret = ReleaseBank(StreamId, TmpBank);
        if (Ret == CFS_OK) {
            /* prefetch the next bank */
            UINT64 Pos = Bank->Pos + Bank->Length;
            if (Pos < Stream->Length) {
                Ret = AddReadCmd(Stream, Pos);
                if (Ret != CFS_OK) {
                    /* fatal error, no error handling */
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AddReadCmd fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                // Code Coverage (Yellow): Possible when the last bank has reached EOS (Pos == Stream->Length). No need to prefetch bank.
            }
        } else {
            /* fatal error, no error handling */
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseBank fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Ret;
}

/**
 *  The implementation of asynchronous fread.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. There might be some Cmds remained. Let Sch Task consume the rest commands.
 *  @param [in] Buffer              Buffer address
 *  @param [in] Size                Size of a record
 *  @param [in] Count               Number of records
 *  @param [in] Stream              The CFS stream
 *  @param [out] Result             Number of records that are readed
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchAsyncfreadImpl(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *Result)
{
    /* called with stream/sch mutex locked */
    UINT32 Ret;

    /*
     * I/O Error handling:
     *     Start considering I/O error from now on.
     *     If there's I/O error from SwitchToAsyncRead, return immediately. The stream might still be in Write mode. There might be pending Write Cmds.
     *     If there's I/O error from AddReadCmd, return immediately. Cmd and Bank are no allocated.
     *     If there's I/O error from StartAsyncRead, return immediately. Some Read commands might be sent to Sch Task. Let Sch Task consume the rest commands.
     *     If there's I/O error from WaitPendingRead, return immediately because Sch Task won't read data any more.
     */

    Ret = SwitchToAsyncRead(Stream);
    if (Ret == CFS_OK) {
        const UINT8 StreamId = Stream->StreamId;
        const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
        if (AsyncData->MaxNumBank > 1U) {
            UINT64 ByteLeft = (UINT64) Size * Count;
            UINT64 CurPos = Stream->Pos;
            UINT8 *BufferTemp = Buffer;
            while (CurPos < Stream->Length) {
                UINT64 Len;
                /* Read data from current available banks. Won't wait for pending read commands. */
                Ret = ReadFromBank(Stream, BufferTemp, CurPos, ByteLeft, &Len);
                if (Ret == CFS_OK) {
                    if (Len > 0ULL) {
                        ByteLeft -= Len;
                        BufferTemp = &BufferTemp[Len]; /* BufferTemp += Len */
                        CurPos += Len;
                        /*
                         * Some data is read from current available banks.
                         * Decide whether to prefetch the next bank.
                         */
                        if (CurPos < Stream->Length) {
                            /*
                             * Not EOF
                             * Prefetch the next bank when needed.
                             */
                            Ret = PrefetchBank(Stream, CurPos);
                        } else {
                            /*
                             * Reach EOF.
                             * There's no data to prefetch, and no pending read command.
                             * So set ByteLeft = 0 in order to break the loop and return.
                             * It possible that ByteLeft != 0 at this point if (Size * Count) is beyond EOF.
                             */
                            ByteLeft = 0U;
                        }
                    } else {
                        UINT8 IsPending = 1U;
                        /*
                         * No data is read from current available banks.
                         * Two possible reasons:
                         * 1. New data is pending.
                         *    Need to wait pending read commands.
                         * 2. CurPos is beyond the region of current banks.
                         *    Start new async read, so new data must be pending.
                         *
                         * Decide whether to wait pending read commands or start a new async read session.
                         * In either cases, wait pending read commands afterwards.
                         */
                        Ret = BankIsPending(StreamId, CurPos, &IsPending);
                        if (Ret == CFS_OK) {
                            if (IsPending == 0U) {
                                // Code Coverage: No pending bank at CurPos. Need to start a new async read session.
                                Ret = StartAsyncRead(Stream, CurPos);
                                if (Ret != CFS_OK) {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] StartAsyncRead fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                        } else {
                            /* fatal error, no error handling */
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] BankIsPending fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    /* If data not complete, wait pending read commands. */
                    if ((Ret == CFS_OK) && (ByteLeft != 0ULL)) {
                        Ret = WaitPendingRead(Stream, CurPos);
                        if (Ret != CFS_OK) {
                            /* fatal error, no error handling */
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] WaitPendingRead fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                } else {
                    /* fatal error, no error handling */
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReadFromBank fail", __func__, NULL, NULL, NULL, NULL);
                }

                /*
                 * Break the loop if:
                 * 1. There's error, or
                 * 2. Read complete
                 */
                if ((Ret != CFS_OK) || (ByteLeft == 0ULL)) {
                    break;
                }
            }

            /* Set return value */
            if ((Ret == CFS_OK) || (Ret == CFS_ERR_FS)) {
                UINT64 LeftCount = (ByteLeft + Size - 1ULL) / Size;
                *Result = Count - (UINT32) LeftCount;
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchAsyncfreadImpl: [ERROR] MaxNumBank %u", AsyncData->MaxNumBank, 0, 0, 0, 0);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SwitchToAsyncRead fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Asynchronous fread.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. There might be some Cmds remained. Let Sch Task consume the rest commands.
 *  @param [in] Buffer              Buffer address
 *  @param [in] Size                Size of a record
 *  @param [in] Count               Number of records
 *  @param [in] Stream              The CFS stream
 *  @param [out] NumSuccess         The returned number of records read
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchAsyncfread(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchLock();
    if (Ret == CFS_OK) {
        /*
         * I/O Error handling:
         *     Start considering I/O error from now on.
         *     If there's I/O error, return immediately.
         */

        Ret = AmbaCFS_SchAsyncfreadImpl(Buffer, Size, Count, Stream, NumSuccess);
        if (Ret != CFS_OK) {
            Stream->Status = AMBA_CFS_STATUS_ERROR;
        }
        if (AmbaCFS_SchUnlock() != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_FATAL;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Switch to asynchronous write
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. The stream might still be in Read mode. There might be pending Read Cmds.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 SwitchToAsyncWrite(AMBA_CFS_STREAM_s *Stream)
{
    /* must be called with sch/stream mutex locked */
    UINT32 Ret;
    const UINT8 StreamId = Stream->StreamId;
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];

    /*
     * I/O Error handling:
     *     Start considering I/O error from now on.
     *     If there's I/O error from CheckScheduler, return immediately. The stream might be in Read mode. There might be pending Read Cmds.
     *     If there's I/O error from FlushReadCmd, return immediately. The stream is still in Read mode but all the pending Read Cmds are cleared.
     */

    /* check if any error happens during previous async I/O */
    Ret = CheckScheduler(AsyncData);
    if (Ret == CFS_OK) {
        if (AsyncData->Read != 0U) {
            /* Async Read */

            /* from read to write, clear all pending read commands */
            /* Might return CFS_ERR_FS because FlushBuffer = 1 */
            Ret = FlushReadCmd(StreamId, 1U);
            if (Ret == CFS_OK) {
                AsyncData->Read = 0U;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FlushReadCmd fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            /* Async Write */

            /* check if write pointer has been moved */
            const AMBA_CFS_SCH_BANK_INFO_s *Bank = AsyncData->Bank;
            UINT64 CurPos = Stream->Pos;
            if (Bank != NULL) {
                if ((CurPos < Bank->Pos) || (CurPos > (Bank->Pos + Bank->Length))) {
                    Ret = AddWriteCmd(Stream);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AddWriteCmd fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get async bank.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. In this case, no need to release resource.
 *  @param [in] StreamId            The stream ID
 *  @param [in] CurPos              The current position
 *  @param [out] Bank               The allocated bank
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 SchAsyncfwrite_GetBank(UINT8 StreamId, UINT64 CurPos, AMBA_CFS_SCH_BANK_INFO_s **Bank)
{
    UINT32 Ret = CFS_OK;
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];

    if (AsyncData->Bank != NULL) {
        /*
         * When AsyncData->Bank is not NULL in write mode, there's only one Bank.
         * The Bank is not full, so the data is not written to file system.
         * Any new data will be written to this Bank.
         */

        /* Return write buffer */
        *Bank = AsyncData->Bank;
    } else {
        AMBA_CFS_SCH_BANK_INFO_s *RetBank;
        /*
         * I/O Error handling:
         *     Start considering I/O error from now on.
         *     If there's I/O error, return immediately. No Bank is allocated.
         */

        /* Allocate new buffer */
        Ret = AllocateBank(StreamId, &RetBank);
        if (Ret == CFS_OK) {
            AsyncData->Bank = RetBank;
            RetBank->Pos = CurPos;
            RetBank->Length = 0U;

            *Bank = RetBank;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AllocateBank fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of asynchronous fwrite.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Buffer              Buffer address
 *  @param [in] Size                Size of a record
 *  @param [in] Count               Number of records
 *  @param [in] Stream              The CFS stream
 *  @param [out] Result             Number of records that are written
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchAsyncfwriteImpl(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *Result)
{
    /* called with stream/sch mutex locked */
    UINT32 Ret;

    /*
     * I/O Error handling:
     *     Start considering I/O error from now on.
     *     If there's I/O error from SwitchToAsyncWrite, return immediately. The stream might still be in Read mode.
     *     If there's I/O error from SchAsyncfwrite_GetBank, return immediately. Let Sch Task consume the reset commnads.
     */

    Ret = SwitchToAsyncWrite(Stream);
    if (Ret == CFS_OK) {
        AMBA_CFS_COPY_INFO_s CopyInfo = {0};
        UINT64 ByteLeft = (UINT64) Size * Count;

        Ret = AmbaCFS_SchCopySetSource(&CopyInfo, Buffer);
        if (Ret == CFS_OK) {
            UINT64 CurPos = Stream->Pos;
            /* process data */
            while (ByteLeft > 0ULL) {
                const UINT8 StreamId = Stream->StreamId;
                AMBA_CFS_SCH_BANK_INFO_s *Bank;
                /*
                 * Get a free Bank.
                 * CheckScheduler is called in the process.
                 * If there's I/O error, return immediately.
                 */
                Ret = SchAsyncfwrite_GetBank(StreamId, CurPos, &Bank);
                if (Ret == CFS_OK) {
                    if ((CurPos >= Bank->Pos) && (CurPos < (Bank->Pos + g_AmbaCFSSch.BankSize))) {
                        UINT64 Len;
                        /* copy data to write buffer */
                        UINT64 OffsetTemp = CurPos - Bank->Pos;
                        UINT32 Offset = (UINT32) OffsetTemp;
                        /* Since (CurPos < (Bank->Pos + g_AmbaCFSSch.BankSize)), no need to consider the negativity of BankLeft */
                        UINT32 BankLeft = g_AmbaCFSSch.BankSize - Offset;
                        /* If Bank->Pos is not FIO aligned, force to flush the buffer ASAP (by adjusting BankLeft) */
                        if ((Bank->Pos % AMBA_CFS_FIO_ALIGNED_SIZE) != 0ULL) {
                            // Code Coverage (Yellow): write from an unaligned address
                            UINT64 CurPosRemainder = CurPos % AMBA_CFS_FIO_ALIGNED_SIZE;
                            /* the start position of this bank is not 128KB aligned, force flush ASAP */
                            UINT32 FioLeft = AMBA_CFS_FIO_ALIGNED_SIZE - (UINT32) CurPosRemainder;
                            /*
                             * Code Coverage: It's possible that (FioLeft >= BankLeft) under the following conditions:
                             * 1. (Bank->Pos + g_AmbaCFSSch.BankSize) is not FIO aligned
                             *    Usually g_AmbaCFSSch.BankSize is aligned. When Bank->Pos is not aligned, (Bank->Pos + g_AmbaCFSSch.BankSize) is also not aligned.
                             * 2. CurPos is close to (Bank->Pos + g_AmbaCFSSch.BankSize)
                             *    So that (CurPos + FioLeft) >= (Bank->Pos + g_AmbaCFSSch.BankSize), which is equivalent to (FioLeft >= BankLeft)
                             */
                            if (FioLeft < BankLeft) {
                                BankLeft = FioLeft;
#if AMBA_CFS_SCH_DEBUG
                                if (ByteLeft > BankLeft) {
                                    // AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchAsyncfwriteImpl: Force flush, Pos:%u CurPos:%u ByteLeft:%u => BankLeft:%u", (UINT32)g_AmbaCFSSch.AsyncData[StreamId].Bank->Pos, (UINT32)CurPos, (UINT32)ByteLeft, BankLeft);
                                }
#endif
                            }
                        }
                        Len = BankLeft;
                        if (ByteLeft < Len) {
                            Len = ByteLeft;
                        }
                        {
                            /* (void) AmbaWrap_memcpy(Bank->Buffer + Offset, CopyInfo.Source, (UINT32) Len) */
                            UINT8 *DstBuffer = &Bank->Buffer[Offset];
                            Ret = AmbaCFS_SchCopySetDestination(&CopyInfo, DstBuffer, (UINT32) Len, Stream);
                            if (Ret == CFS_OK) {
                                Ret = AmbaCFS_SchCopyMemory(&CopyInfo, Stream);
                                if (Ret != CFS_OK) {
                                    /* fatal error, no error handling */
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchCopyMemory fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchCopySetDestination fail", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                        if (Ret == CFS_OK) {
                            CopyInfo.Source = &CopyInfo.Source[Len]; /* SrcBuffer += Len */
                            /* Update length of write buffer */
                            if ((Offset + (UINT32) Len) > Bank->Length) {
                                // Code Coverage (Yellow): Happens when writing new data into bank
                                Bank->Length = Offset + (UINT32) Len;
                            }
                            /* Flush write buffer if it is full */
                            if (Len == BankLeft) {
                                Ret = AddWriteCmd(Stream);
                                if (Ret != CFS_OK) {
                                    /* fatal error, no error handling */
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AddWriteCmd fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            }

                            if (Ret == CFS_OK) {
                                /* Update pointers */
                                ByteLeft -= Len;
                                /* Buffer += Len */
                                CurPos += Len;
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchAsyncfwriteImpl: [ERROR] CurPos %u not supported", (UINT32) CurPos, 0, 0, 0, 0);
                        Ret = CFS_ERR_API;
                    }
                } else {
                    /* fatal error, no error handling */
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SchAsyncfwrite_GetBank fail", __func__, NULL, NULL, NULL, NULL);
                }

                if (Ret != CFS_OK) {
                    break;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchCopySetSource fail", __func__, NULL, NULL, NULL, NULL);
        }
        /* Set return value */
        if (Ret != CFS_ERR_FATAL) {
            UINT64 LeftCount = (ByteLeft + Size - 1ULL) / Size;
            *Result = Count - (UINT32) LeftCount;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SwitchToAsyncWrite fail", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Buffer);
    return Ret;
}

/**
 *  Asynchronous fwrite.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Buffer              Buffer address
 *  @param [in] Size                Size of a record
 *  @param [in] Count               Number of records
 *  @param [in] Stream              The CFS stream
 *  @param [out] NumSuccess         The returned number of records written
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchAsyncfwrite(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_SchAsyncfwriteImpl(Buffer, Size, Count, Stream, NumSuccess);
        if (Ret != CFS_OK) {
            Stream->Status = AMBA_CFS_STATUS_ERROR;
        }
        if (CFS_SET_RETURN(&Ret, AmbaCFS_SchUnlock()) != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  The implementation of synchronous fwrite.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS. APP should close file when getting CFS_ERR_FS.
 *  @param [in] Buffer              Buffer address
 *  @param [in] Size                Size of a record
 *  @param [in] Count               Number of records
 *  @param [in] Stream              The CFS stream
 *  @param [out] Result             Number of records that are written
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchSyncfwriteImpl(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *Result)
{
    /* called with stream's mutex locked */
    UINT32 Ret = CFS_OK;
    UINT64 ByteLeft = (UINT64) Size * Count;
    UINT64 CurPos = Stream->Pos;
    UINT8 *BufferTemp = Buffer;

    /* While has data to send */
    while (ByteLeft > 0ULL) {
        AMBA_CFS_SCH_BANK_INFO_s Bank;
        UINT32 Len;
        UINT32 WrittenLen = 0U;
        UINT32 IoTime = 0U;
        /* Get count per write */
        UINT32 BankLeft = g_AmbaCFSSch.BankSize;
        if ((CurPos % AMBA_CFS_FIO_ALIGNED_SIZE) != 0ULL) {
            UINT64 CurPosRemainder = CurPos % AMBA_CFS_FIO_ALIGNED_SIZE;
            /* the start position of this bank is not 128KB aligned, force flush ASAP */
            UINT32 FioLeft = AMBA_CFS_FIO_ALIGNED_SIZE - (UINT32) CurPosRemainder;
            if (FioLeft < BankLeft) {
                BankLeft = FioLeft;
#if AMBA_CFS_SCH_DEBUG
                if (ByteLeft > BankLeft) {
                    // AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchSyncfwriteImpl: Force flush, Pos:%u CurPos:%u ByteLeft:%u => BankLeft:%u", (UINT32)Bank.Pos, (UINT32)CurPos, (UINT32)ByteLeft, BankLeft);
                }
#endif
            }
        }
        Len = BankLeft;
        if (ByteLeft < Len) {
            Len = (UINT32) ByteLeft;
        }
        /* Prepare bank */
        Bank.Buffer = BufferTemp;
        Bank.Pos = CurPos;
        Bank.Length = Len;
        Bank.Next = NULL;

        /*
         * I/O Error handling:
         *     The previous process don't encounter I/O error. Start considering I/O error from now on.
         *     If there's I/O error, return immediately.
         *     'Bank' is a local variable. Don't release it.
         */

        /* Write data */
        Ret = WriteBank(Stream, &Bank, Stream->Length, &IoTime, &WrittenLen);
        ByteLeft -= WrittenLen;
        BufferTemp = &BufferTemp[WrittenLen]; /* BufferTemp += WrittenLen */
        CurPos += WrittenLen;

        /*
         * If WrittenLen is not expected, break the loop anyway.
         * There's a chance that WriteBank returns OK but WrittenLen is less than Bank.Length.
         * However, there's no need to change Ret to I/O error because APP can detect it by 'Result'.
         * Let APP decides it's error or not.
         */
        if ((Ret != CFS_OK) || (Bank.Length != WrittenLen)) {
            break;
        }
    }
    /* Set return value */
    if (Ret != CFS_ERR_FATAL) {
        UINT64 LeftCount = (ByteLeft + Size - 1U) / Size;
        *Result = Count - (UINT32) LeftCount;
    }
    return Ret;
}

/**
 *  Synchronous fwrite.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Buffer              Buffer address
 *  @param [in] Size                Size of a record
 *  @param [in] Count               Number of records
 *  @param [in] Stream              The CFS stream
 *  @param [out] NumSuccess         The returned number of records written
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchSyncfwrite(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchSyncfwriteImpl(Buffer, Size, Count, Stream, NumSuccess);
    if (Ret != CFS_OK) {
        Stream->Status = AMBA_CFS_STATUS_ERROR;
    }
    return Ret;
}

/**
 *  Asynchrounou FileSync.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchAsyncFileSyncImpl(AMBA_CFS_STREAM_s *Stream)
{
    /* must be called with stream/sch mutex locked */
    UINT32 Ret;
    const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[Stream->StreamId];

    /*
     * I/O Error handling:
     *     Start considering I/O error from now on.
     *     If there's I/O error, return immediately.
     */

    /* check if any error happens during previous async I/O */
    Ret = CheckScheduler(AsyncData);
    if (Ret == CFS_OK) {
        Ret = WaitPendingWrite(Stream);
        if (Ret == CFS_OK) {
            Ret = CFS_F2C(AmbaFS_FileSync(Stream->File));
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchAsyncFileSyncImpl: [ERROR] AmbaFS_FileSync fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] WaitPendingWrite fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
    }
    Stream->AccBytesNoSync = 0U;
    return Ret;
}

/**
 *  Asynchrounou FileSync.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchAsyncFileSync(AMBA_CFS_STREAM_s *Stream)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_SchAsyncFileSyncImpl(Stream);
        if (Ret != CFS_OK) {
            Stream->Status = AMBA_CFS_STATUS_ERROR;
        }
        if (AmbaCFS_SchUnlock() != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_FATAL;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  The implementation of synchrounou FileSync.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchSyncFileSyncImpl(AMBA_CFS_STREAM_s *Stream)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = CFS_F2C(AmbaFS_FileSync(Stream->File));
    if (Ret == CFS_OK) {
        Stream->AccBytesNoSync = 0U;
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchSyncFileSyncImpl: [ERROR] AmbaFS_FileSync fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Synchrounou FileSync.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchSyncFileSync(AMBA_CFS_STREAM_s *Stream)
{
    /* called with Stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchSyncFileSyncImpl(Stream);
    if (Ret != CFS_OK) {
        Stream->Status = AMBA_CFS_STATUS_ERROR;
    }
    return Ret;
}

/**
 *  The implementation of asynchronous fappend
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Size                The size
 *  @param [out] Result             The size that is appended
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchAsyncfappendImpl(AMBA_CFS_STREAM_s *Stream, UINT64 Size, UINT64 *Result)
{
    /* called with stream/sch mutex locked */
    UINT32 Ret;
    const AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[Stream->StreamId];
    if (AsyncData->Read == 0U) {
        /*
         * I/O Error handling:
         *     Start considering I/O error from now on.
         *     If there's I/O error, return immediately.
         */

        /* check if any error happens during previous async I/O */
        Ret = CheckScheduler(AsyncData);
        if (Ret == CFS_OK) {
            Ret = WaitPendingWrite(Stream);
            if (Ret == CFS_OK) {
                Ret = CFS_F2C(AmbaFS_ClusterAppend(Stream->File, Size, Result));
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchAsyncfappendImpl: [ERROR] AmbaFS_ClusterAppend fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] WaitPendingWrite fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchAsyncfappendImpl: [ERROR] Read %u", AsyncData->Read, 0, 0, 0, 0);
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  Asynchronous fappend
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Size                The size
 *  @param [out] NumSuccess         The returned size (bytes) that is appended
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchAsyncfappend(AMBA_CFS_STREAM_s *Stream, UINT64 Size, UINT64 *NumSuccess)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_SchAsyncfappendImpl(Stream, Size, NumSuccess);
        if (Ret != CFS_OK) {
            Stream->Status = AMBA_CFS_STATUS_ERROR;
        }
        if (AmbaCFS_SchUnlock() != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_FATAL;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  The implementation of synchronous fappend
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Size                The size
 *  @param [out] Result             The size that is appended
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchSyncfappendImpl(const AMBA_CFS_STREAM_s *Stream, UINT64 Size, UINT64 *Result)
{
    UINT32 Ret;
    /* called with stream's mutex locked */
    /* No need to set status to error if AmbaFS_fappend() failed, the file still can be used */
    Ret = CFS_F2C(AmbaFS_ClusterAppend(Stream->File, Size, Result));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SchSyncfappendImpl: [ERROR] AmbaFS_ClusterAppend fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Synchronous fappend
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Size                The size
 *  @param [out] NumSuccess         The returned size (bytes) that is appended
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchSyncfappend(AMBA_CFS_STREAM_s *Stream, UINT64 Size, UINT64 *NumSuccess)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchSyncfappendImpl(Stream, Size, NumSuccess);
    if (Ret != CFS_OK) {
        Stream->Status = AMBA_CFS_STATUS_ERROR;
    }
    return Ret;
}

/**
 *  The implementation of asynchronous fclose.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchAsyncfcloseImpl(AMBA_CFS_STREAM_s *Stream)
{
    /* called with stream/sch mutex locked */
    UINT32 Ret = CFS_OK;
    const UINT8 StreamId = Stream->StreamId;
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];

    /*
     * I/O Error handling:
     *     Start considering I/O error from now on.
     *     The functions that could return I/O error:
     *     1. CheckScheduler
     *         Doesn't change Ret
     *     2. FlushReadCmd
     *         Won't return CFS_ERR_FS because FlushBuffer = 0
     *     In conclusion, this function will not return CFS_ERR_FS.
     */

    /* check if any error happens during previous async I/O */
    if (CheckScheduler(AsyncData) != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckScheduler fail", __func__, NULL, NULL, NULL, NULL);
        Stream->Status = AMBA_CFS_STATUS_ERROR;
    }
    if (AsyncData->Read != 0U) {
        /* Async Read */

        /* remove pending read cmds */
        /* to speed up close, not to wait the active read command, let sch flush buffer */
        /* Won't return CFS_ERR_FS because FlushBuffer = 0 */
        Ret = FlushReadCmd(StreamId, 0U);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FlushReadCmd fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        /* Async Write */

        /* Check remained data */
        if (AsyncData->Bank != NULL) {
            /* Write the remained data only when there's no error (including I/O Error). */
            if (Stream->Status != AMBA_CFS_STATUS_ERROR) {
                /* submit the current write buffer */
                Ret = AddWriteCmd(Stream);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AddWriteCmd fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                /* flush the current write buffer */
                Ret = ReleaseBank(StreamId, AsyncData->Bank);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ReleaseBank fail", __func__, NULL, NULL, NULL, NULL);
                }
                AsyncData->Bank = NULL;
            }
        }
    }
    /*
     * Close command should be sent when there's I/O error.
     * Theoretically, Ret won't be CFS_ERR_FS.
     * Add a check here just in case.
     */
    if ((Ret == CFS_OK) || (Ret == CFS_ERR_FS)) {
        if (CFS_SET_RETURN(&Ret, AddCloseCmd(Stream)) != CFS_OK) {
            /* fatal error, no error handling */
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AddCloseCmd fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Asynchronous fclose.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchAsyncfclose(AMBA_CFS_STREAM_s *Stream)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_SchAsyncfcloseImpl(Stream);
        if (AmbaCFS_SchUnlock() != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_FATAL;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  The implementation of synchronous fclose.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchSyncfcloseImpl(AMBA_CFS_STREAM_s *Stream)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = CloseFile(Stream, 0U);
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CloseFile fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Synchronous fclose.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchSyncfclose(AMBA_CFS_STREAM_s *Stream)
{
    /* called with stream's mutex locked */
    UINT32 Ret;
    Ret = AmbaCFS_SchSyncfcloseImpl(Stream);
    /*
     * I/O Error handling:
     *     Do not set Stream->Status to AMBA_CFS_STATUS_ERROR.
     *     Otherwise the Stream will be skipped by GetFreeStream and can no longer be used again. (Not recoverable)
     */
    return Ret;
}

/**
 *  The implementation of getting cached data size on a drive
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] CachedSize         Cached data size
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_SchCachedFileDataSizeImpl(char Drive, UINT64 *CachedSize)
{
    UINT64 SizeByte = 0ULL;
    const AMBA_CFS_SCH_TASK_INFO_s * const Task = &g_AmbaCFSSch.Task;
    const AMBA_CFS_SCH_CMD_INFO_s *Cmd = Task->Cmd;
    char DriveUpper = AmbaCFS_UtilToUpper(Drive);
    if ((Cmd != NULL) && (Cmd->Event == AMBA_CFS_EVENT_ASYNC_WRITE)) {
        if (AmbaCFS_UtilToUpper(Cmd->Stream->Filename[0]) == DriveUpper) {
            SizeByte += Cmd->Bank->Length;
        }
    }
    Cmd = Task->CmdList;
    while (Cmd != NULL) {
        if (Cmd->Event == AMBA_CFS_EVENT_ASYNC_WRITE) {
            if (AmbaCFS_UtilToUpper(Cmd->Stream->Filename[0]) == DriveUpper) {
                SizeByte += Cmd->Bank->Length;
            }
        }
        Cmd = Cmd->Next;
    }
    /* Set return value */
    *CachedSize = SizeByte;
    return CFS_OK;
}

/**
 *  Get cached data size on a drive
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] CachedSize         Cached data size
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchCachedFileDataSize(char Drive, UINT64 *CachedSize)
{
    UINT32 Ret;
    Ret = AmbaCFS_SchLock();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_SchCachedFileDataSizeImpl(Drive, CachedSize);
        if (AmbaCFS_SchUnlock() != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchUnlock fail", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_FATAL;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_SchLock fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Set async data of a stream
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @param [in] Mode                The open mode
 *  @param [in] BankBuffer          The bank buffer
 *  @param [in] BankBufferSize      The size of bank buffer
 *  @param [in] BankSize            The size of a bank
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaCFS_SetAsyncData(UINT8 StreamId, UINT8 Mode, UINT8 *BankBuffer, UINT32 BankBufferSize, UINT32 BankSize)
{
    AMBA_CFS_STREAM_ASYNC_DATA_s * const AsyncData = &g_AmbaCFSSch.AsyncData[StreamId];
    AsyncData->MaxNumBank = BankBufferSize / BankSize;
    AsyncData->NumBank = 0U;
    AsyncData->Read = (Mode == AMBA_CFS_FILE_MODE_READ_ONLY) ? 1U : 0U;
    AsyncData->Bank = NULL;
    AsyncData->Error = CFS_OK;
    return CreateBankPool(StreamId, BankBuffer, BankBufferSize, BankSize);
}

/**
 *  Get max bank number
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return The max bank number
 */
UINT32 AmbaCFS_GetMaxNumBank(UINT8 StreamId)
{
    /* must be called with stream's mutex locked */
    return g_AmbaCFSSch.AsyncData[StreamId].MaxNumBank;
}

/**
 *  GDMA alignment
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] CopyInfo            Memory copy information
 *  @param [in] Dst                 Destination
 *  @param [in] Src                 Source
 *  @param [in] Size                The size
 *  @param [in] DmaThres            DMA copy threshold
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchGdmaAlignment(AMBA_CFS_COPY_INFO_s *CopyInfo, UINT8 *Dst, UINT8 *Src, UINT32 Size, UINT32 DmaThres)
{
    UINT32 Ret = CFS_OK;
    UINT32 DataSize = Size;
    UINT8 *Source = Src;
    UINT8 *Destination = Dst;
    ULONG DstUL;
    ULONG DstAlignedUL;
    UINT32 MaxAlignment = (DmaThres > AMBA_GDMA_MAX_WIDTH) ? (DmaThres) : (AMBA_GDMA_MAX_WIDTH);
    UINT32 TailSize;
    UINT32 Remain;

    /* Align Dst to AMBA_CACHE_LINE_SIZE */
    AmbaMisra_TypeCast(&DstUL, &Dst);
    DstAlignedUL = AmbaCFS_GetAlignedValUL(DstUL, AMBA_CACHE_LINE_SIZE);
    if (DstUL != DstAlignedUL) {
        ULONG Offset = DstAlignedUL - DstUL;
        Ret = CFS_WRAP2C(AmbaWrap_memcpy(Destination, Source, Offset));
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memcpy fail", __func__, NULL, NULL, NULL, NULL);
        } else {
            Source      = &Source[Offset];      /* Source += Offset; */
            Destination = &Destination[Offset]; /* Destination += Offset; */
            DataSize -= (UINT32)Offset;
        }
    }

    if (Ret == CFS_OK) {
        /* Get TailSize. Align DataSize to AMBA_CACHE_LINE_SIZE */
        TailSize = DataSize & ((UINT32)AMBA_CACHE_LINE_SIZE - 1U); /* TailSize = DataSize % AMBA_CACHE_LINE_SIZE */
        DataSize -= TailSize;

        /*
         * Some examples of different DmaThres
         * 1. DmaThres = 65536
         *    Remain = DataSize % 65536
         *    (1) Remain : memcpy (Combine Remain with TailSize)
         *    (2) Others : GDMA copy (DataSize - Remain)
         * 2. DmaThres = 8192
         *    Remain = DataSize % 8192
         *    (1) Remain : memcpy (Combine Remain with TailSize)
         *    (2) Others : GDMA copy (DataSize - Remain)
         * 3. DmaThres = 512
         *    Remain = DataSize % 8192 (Do NOT mod 512)
         *    (1) If Remain >= 512 : GDMA copy
         *        If Remain <  512 : memcpy (Combine Remain with TailSize)
         *    (2) Others : GDMA copy
         */
        Remain = DataSize & (MaxAlignment - 1U); /* Remain = DataSize % MaxAlignment */
        if (Remain < DmaThres) {
            TailSize += Remain;
            DataSize -= Remain;
        }

        if (TailSize != 0U) {
            Ret = CFS_WRAP2C(AmbaWrap_memcpy(&Destination[DataSize], &Source[DataSize], TailSize));
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memcpy fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        if (Ret == CFS_OK) {
            CopyInfo->DmaSource = Source;
            CopyInfo->DmaDestination = Destination;
            CopyInfo->DmaDataSize = DataSize;
        }
    }
    return Ret;
}

/**
 *  Set GDMA source
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] CopyInfo            Memory copy information
 *  @param [in] Src                 Source
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchCopySetSource(AMBA_CFS_COPY_INFO_s *CopyInfo, UINT8 *Src)
{
    UINT32 Ret = CFS_OK;
    CopyInfo->Source = Src;
    /* For async read by GDMA, source is written by FS read, need to clean cache. It's already done in CmdProcess_ReadImpl. */
    /* For async write by GDMA, source is user buffer, it should be taken cared of by user. */
    return Ret;
}

/**
 *  Set GDMA destination
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] CopyInfo            Memory copy information
 *  @param [in] Dst                 Destination
 *  @param [in] Size                The size
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchCopySetDestination(AMBA_CFS_COPY_INFO_s *CopyInfo, UINT8 *Dst, UINT32 Size, const AMBA_CFS_STREAM_s *Stream)
{
    UINT32 Ret = CFS_OK;

    /* AmbaCFS_SchCopySetSource should be called first */

    /* CopyInfo->Source is already set in AmbaCFS_SchCopySetSource */
    CopyInfo->Destination = Dst;
    CopyInfo->DataSize = Size;
    /* Only check AMBA_CFS_DMA_NC_SIZE_THRESHOLD because AMBA_CFS_DMA_NC_SIZE_THRESHOLD should be smaller than AMBA_CFS_DMA_SIZE_THRESHOLD. */
    if ((Stream->DmaEnable == 1U) && (Size >= AMBA_CFS_DMA_NC_SIZE_THRESHOLD)) {
        UINT32 SrcIsCached;
        UINT32 DmaThres;
        SrcIsCached = g_AmbaCFSSch.CheckCached(CopyInfo->Source, 1U); /* Set Size = 1 to check the address specifically */
        if (SrcIsCached != 0U) {
            DmaThres = AMBA_CFS_DMA_SIZE_THRESHOLD;
        } else {
            DmaThres = AMBA_CFS_DMA_NC_SIZE_THRESHOLD;
        }
        /* it could corrupt the data ahead or behind buffer */
        /* for async write, it is safe because destination is bank, and bank must be cache_line aligned */
        /* for async read, it could be dangerous becase source is user buffer, so use AmbaCFS_SchGdmaAlignment() to protect */
        Ret = AmbaCFS_SchGdmaAlignment(CopyInfo, Dst, CopyInfo->Source, Size, DmaThres);

        /*
         * It's possible that DmaDataSize == 0.
         * Meaning that all data are already copied in AmbaCFS_SchCopySetDestination.
         */
        if ((Ret == CFS_OK) && (CopyInfo->DmaDataSize > 0U)) {
            /* Cache invalidate */
            /* For async read, destination is user buffer, invalidate the cache for user. */
            /* For async write, destination is used for FS write. FS will clean cache before file write, so it's needed to invalidate the cache. */
            /* DmaDestination and DmaDataSize should be aligned after AmbaCFS_SchGdmaAlignment */
            Ret = AmbaCFS_SchDataInvalidate(CopyInfo->DmaDestination, CopyInfo->DmaDataSize);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchDataInvalidate fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }
    return Ret;
}

/**
 *  The implementaiton of GDMA copy
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] DmaDestination      DMA destination address (aligned)
 *  @param [in] DmaSource           DMA source address (aligned)
 *  @param [in] DmaDataSize         DMA data size (aligned)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchGdmaMemcpyImpl(UINT8 *DmaDestination, UINT8 *DmaSource, UINT32 DmaDataSize)
{
    UINT32 Ret;
    UINT32 Rest = DmaDataSize;
    UINT32 BlockHeight = DmaDataSize / AMBA_GDMA_MAX_WIDTH;
    UINT8 *Src = NULL;
    UINT8 *Dst = NULL;

    Ret = AmbaCFS_SchVirtToPhys(DmaSource, &Src);
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchVirtToPhys(Src) fail", __func__, NULL, NULL, NULL, NULL);
    } else {
        Ret = AmbaCFS_SchVirtToPhys(DmaDestination, &Dst);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchVirtToPhys(Dst) fail", __func__, NULL, NULL, NULL, NULL);
        } else {
#if defined(CONFIG_SVC_APPS_ICAM) && defined(CONFIG_THREADX)
            Ret = AmbaCFS_SchDataClean(DmaSource, DmaDataSize);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchDataClean fail", __func__, NULL, NULL, NULL, NULL);
            }
#endif
            while (Rest > 0U) {
                UINT32 DataSize;
                AMBA_GDMA_BLOCK_s GdmaBlock;
                GdmaBlock.PixelFormat = AMBA_GDMA_8_BIT;
                GdmaBlock.pSrcImg = Src;
                GdmaBlock.pDstImg = Dst;
                if (BlockHeight > AMBA_GDMA_MAX_HEIGHT) {
                    GdmaBlock.SrcRowStride = AMBA_GDMA_MAX_WIDTH;
                    GdmaBlock.DstRowStride = GdmaBlock.SrcRowStride;
                    GdmaBlock.BltWidth = AMBA_GDMA_MAX_WIDTH;
                    GdmaBlock.BltHeight = AMBA_GDMA_MAX_HEIGHT;
                } else if (BlockHeight > 0U) {
                    GdmaBlock.SrcRowStride = AMBA_GDMA_MAX_WIDTH;
                    GdmaBlock.DstRowStride = GdmaBlock.SrcRowStride;
                    GdmaBlock.BltWidth = AMBA_GDMA_MAX_WIDTH;
                    GdmaBlock.BltHeight = BlockHeight;
                } else {
                    GdmaBlock.SrcRowStride = Rest;
                    GdmaBlock.DstRowStride = GdmaBlock.SrcRowStride;
                    GdmaBlock.BltWidth = Rest;
                    GdmaBlock.BltHeight = 1U;
                }
                DataSize = GdmaBlock.BltWidth * GdmaBlock.BltHeight; /* Pixel size = 1 for AMBA_GDMA_8_BIT fomat. */
                Ret = CFS_G2C(AmbaGDMA_BlockCopy(&GdmaBlock, NULL, 0U, AMBA_CFS_DMA_TIMEOUT));
                if (Ret != OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaGDMA_BlockCopy fail", __func__, NULL, NULL, NULL, NULL);
                }

                if (Ret != CFS_OK) {
                    break;
                }
                Rest -= DataSize;
                BlockHeight -= GdmaBlock.BltHeight; /* Might become negative in the last loop but it's harmless. */
                Src = &Src[DataSize];
                Dst = &Dst[DataSize];
            }

            if (Ret == CFS_OK) {
                Ret = CFS_G2C(AmbaGDMA_WaitAllCompletion(AMBA_CFS_DMA_TIMEOUT));
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaGDMA_WaitCompletion fail", __func__, NULL, NULL, NULL, NULL);
                }
            }
#if defined(CONFIG_SVC_APPS_ICAM) && defined(CONFIG_THREADX)
            if (Ret == CFS_OK) {
                Ret = AmbaCFS_SchDataInvalidate(DmaDestination, DmaDataSize);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: AmbaCFS_SchDataInvalidate fail", __func__, NULL, NULL, NULL, NULL);
                }
            }
#endif
        }
    }

    return Ret;
}

/**
 *  Copy memory
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] CopyInfo            Memory copy information
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SchCopyMemory(const AMBA_CFS_COPY_INFO_s *CopyInfo, const AMBA_CFS_STREAM_s *Stream)
{
    UINT32 Ret = CFS_OK;
    /* Only check AMBA_CFS_DMA_NC_SIZE_THRESHOLD because AMBA_CFS_DMA_NC_SIZE_THRESHOLD should be smaller than AMBA_CFS_DMA_SIZE_THRESHOLD. */
    if ((Stream->DmaEnable == 1U) && (CopyInfo->DataSize >= AMBA_CFS_DMA_NC_SIZE_THRESHOLD)) {
        /*
         * It's possible that DmaDataSize == 0.
         * Meaning that all data are already copied in AmbaCFS_SchCopySetDestination.
         */
        if (CopyInfo->DmaDataSize > 0U) {
            Ret = AmbaCFS_SchGdmaMemcpyImpl(CopyInfo->DmaDestination, CopyInfo->DmaSource, CopyInfo->DmaDataSize);
        }
    } else {
        Ret = CFS_WRAP2C(AmbaWrap_memcpy(CopyInfo->Destination, CopyInfo->Source, CopyInfo->DataSize));
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memcpy fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

