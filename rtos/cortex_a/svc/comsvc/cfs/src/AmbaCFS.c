/**
 * @file AmbaCFS.c
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
#include "AmbaCFS_Search.h"
#include "AmbaCFS_ActiveDta.h"
#include <AmbaPrint.h>

#define AMBA_CFS_EVENT_QUEUE_FLAG_ADD          (1U)          /**< EventQueue Flag: A new event is added to EventQueue. */
#define AMBA_CFS_EVENT_QUEUE_SIZE_DEFAULT      (1024U)       /**< Default size of event queue */
#define AMBA_CFS_FILE_ALIGNMENT_MODE_DEFAULT   (AMBA_CFS_ALIGN_MODE_NONE) /**< Default file alignment mode */
#define AMBA_CFS_FILE_ALIGNMENT_SIZE_DEFAULT   (0U)          /**< Default file alignment. (The value should be a multiple of a cluster size.) */
#define AMBA_CFS_FILE_ALIGNMENT_MAX            (104857600U)  /**< Maximum file alignment. (The value should be a multiple of a cluster size.) */
#define AMBA_CFS_FILE_ALIGNMENT_MIN            (512U)        /**< Minimum file alignment. Set by the minimum possible cluster size. */
#define AMBA_CFS_BYTE_TO_SYNC_DEFAULT          (0U)          /**< Default byte to sync. */
#define AMBA_CFS_BYTE_TO_SYNC_MIN              (0U)          /**< Minimum byte to sync. */
#define AMBA_CFS_BYTE_TO_SYNC_MAX              (104857600U)  /**< Maximum byte to sync. */

/**
 * CFS Event Queue
 */
typedef struct {
    AMBA_KAL_MUTEX_t EventMutex;                        /**< CFS event queue mutex. */
    AMBA_KAL_EVENT_FLAG_t QueueFlag;                    /**< EventQueue flag. Set the flag when a new event is added to EventQueue. */
    AMBA_CFS_EVENT_INFO_s *EventQueue;                  /**< An array of events. */
    UINT32 EventQueueSize;                              /**< The size of event queue. */
    UINT32 EventQueueNum;                               /**< The number of events in EventQueue. */
} AMBA_CFS_EVENT_QUEUE_s;

/**
 * CFS module core
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex;                             /**< CFS core mutex */
    AMBA_KAL_MUTEX_t FileOpenMutex;                     /**< CFS mutex for file open */
    AMBA_CFS_STREAM_s StreamTbl[AMBA_CFS_MAX_STREAM];   /**< Stream table */
    UINT8 CacheEnable;                                  /**< Enable CFS info cache or not */
    UINT32 LastClosedStreamId;                          /**< The last closed StreamId of StreamTbl. Used in FileOpen to avoid opening a file that is just closed. Set to AMBA_CFS_MAX_STREAM if no file has been closed. */
    UINT32 CmdAmount;                                   /**< CFS command number */
    UINT32 BankSize;                                    /**< CFS bank size */
    AMBA_CFS_EVENT_QUEUE_s EventCfg;                    /**< CFS Event Queue config */
    AMBA_CFS_ACTIVE_DTA_POOL_s ActiveDtaPool;           /**< Active DTA Pool */
} AMBA_CFS_s;

/**
 * CFS init or not
 */
static UINT8 g_AmbaCFS_Init = 0U;

/**
 * CFS module core
 */
static AMBA_CFS_s g_AmbaCFS GNU_SECTION_NOZEROINIT;

static UINT32 AmbaCFS_GetDriveClusterSize(char Drive, UINT32 *ClusterSize);

/**
 *  Create CFS streams' mutex
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 AmbaCFS_CreateMutex(void)
{
    UINT32 Ret;
    static char CfsMutex[] = "CfsMutex";
    Ret = CFS_K2C(AmbaKAL_MutexCreate(&g_AmbaCFS.Mutex, CfsMutex));
    if (Ret == CFS_OK) {
        static char CfsFileOpenMutex[] = "CfsOpenMutex";
        Ret = CFS_K2C(AmbaKAL_MutexCreate(&g_AmbaCFS.FileOpenMutex, CfsFileOpenMutex));
        if (Ret == CFS_OK) {
            static char CfsEventMutex[] = "CfsEventMutex";
            Ret = CFS_K2C(AmbaKAL_MutexCreate(&g_AmbaCFS.EventCfg.EventMutex, CfsEventMutex));
            if (Ret == CFS_OK) {
                static char CfsStreamMutex[] = "CfsStreamMutex";
                for (UINT32 i = 0U; i < AMBA_CFS_MAX_STREAM; i++) {
                    Ret = CFS_K2C(AmbaKAL_MutexCreate(&g_AmbaCFS.StreamTbl[i].Mutex, CfsStreamMutex));
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_CreateMutex: [ERROR] Failed to create Stream Mutex %u", i, 0, 0, 0, 0);
                        break;
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexCreate CfsEventMutex fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexCreate FileOpenMutex fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexCreate fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Create CFS flag
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static inline UINT32 AmbaCFS_CreateFlag(void)
{
    UINT32 Ret;
    static char CfsEventQueueFlag[] = "CfsEventQueueFlag";
    Ret = CFS_K2C(AmbaKAL_EventFlagCreate(&g_AmbaCFS.EventCfg.QueueFlag, CfsEventQueueFlag));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagCreate fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Lock CFS mutex
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_Lock(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexTake(&g_AmbaCFS.Mutex, AMBA_CFS_TIMEOUT_MUTEX));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Unlock CFS mutex
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_Unlock(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexGive(&g_AmbaCFS.Mutex));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Lock a CFS stream
 *  @param [in] Stream              CFS stream handler
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_LockStream(AMBA_CFS_STREAM_s *Stream)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexTake(&Stream->Mutex, AMBA_CFS_TIMEOUT_MUTEX));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_LockStream: [ERROR] AmbaKAL_MutexTake fail Stream[%u]", Stream->StreamId, 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Unlock a CFS stream
 *  @param [in] Stream              CFS stream handler
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_UnlockStream(AMBA_CFS_STREAM_s *Stream)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexGive(&Stream->Mutex));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_UnlockStream: [ERROR] AmbaKAL_MutexGive fail Stream[%u]", Stream->StreamId, 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Lock CFS FileOpen mutex
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_LockFileOpen(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexTake(&g_AmbaCFS.FileOpenMutex, AMBA_CFS_TIMEOUT_MUTEX));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Unlock CFS FileOpen mutex
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_UnlockFileOpen(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexGive(&g_AmbaCFS.FileOpenMutex));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Lock CFS Event Queue mutex
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_LockEventQueue(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexTake(&g_AmbaCFS.EventCfg.EventMutex, AMBA_CFS_TIMEOUT_MUTEX));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Unlock CFS Event Queue mutex
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_UnlockEventQueue(void)
{
    UINT32 Ret;
    Ret = CFS_K2C(AmbaKAL_MutexGive(&g_AmbaCFS.EventCfg.EventMutex));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get the required bank amount for the file parameter.
 *  @param [in]  FileParam          CFS file parameter
 *  @param [out] BankAmount         The returned bank amount
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetBankAmount(const AMBA_CFS_FILE_PARAM_s *FileParam, UINT32 *BankAmount)
{
    UINT32 Ret = CFS_OK;
    if (FileParam->AsyncEnable != 0U) {
        /* No need to check (g_AmpCfs.BankSize != 0U). It's already checked in AmpCFS_SchInit. */
        if ((FileParam->BankBufferSize % g_AmbaCFS.BankSize) == 0U) {
            *BankAmount = FileParam->BankBufferSize / g_AmbaCFS.BankSize;
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_GetBankAmount: [ERROR] BankBufferSize %u is not aligned to BankSize %u.", FileParam->BankBufferSize, g_AmbaCFS.BankSize, 0, 0, 0);
            Ret = CFS_ERR_API;
        }
    } else {
        *BankAmount = 0U;
    }
    return Ret;
}

/**
 *  Cache data clean
 *  @param [in] StartAddr           The buffer address
 *  @param [in] Size                The data size. Should be cache line aligned.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_DataClean(void *StartAddr, UINT32 Size)
{
    UINT32 Ret;
    ULONG BufferUL;
    AmbaMisra_TypeCast(&BufferUL, &StartAddr);
    Ret = CFS_CACHE2C(AmbaCache_DataClean(BufferUL, (ULONG)Size));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCache_DataClean fail", __func__, NULL, NULL, NULL, NULL);
    }
    (void)StartAddr;
    return Ret;
}

/**
 *  Cache data invalidate
 *  @param [in] StartAddr           The buffer address
 *  @param [in] Size                The data size. Should be cache line aligned.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_DataInvalidate(void *StartAddr, UINT32 Size)
{
    UINT32 Ret;
    ULONG BufferUL;
    AmbaMisra_TypeCast(&BufferUL, &StartAddr);
    Ret = CFS_CACHE2C(AmbaCache_DataInvalidate(BufferUL, (ULONG)Size));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCache_DataInvalidate fail", __func__, NULL, NULL, NULL, NULL);
    }
    (void)StartAddr;
    return Ret;
}

/**
 *  Convert virtual memory address to physical address
 *  @param [in] VirtAddr            The virtual address
 *  @param [in] PhysAddr            The physical address
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_VirtToPhys(void *VirtAddr, void **PhysAddr)
{
    UINT32 Ret = CFS_OK;
    // Currently, this is a default function for ThreadX and QNX which accept virtual address in AmbaGDMA_BlockCopy.
    // So the virtual address don't need to be changed.
    // If AmbaGDMA_BlockCopy accepts physical address in the future, the implementation of this function may change.
    *PhysAddr = VirtAddr;
    return Ret;
}

/**
 *  Check if there is enough free banks for async read
 *  @param [in]  FileParam          CFS file parameter
 *  @return 1 - TRUE, 0 - FALSE
 */
static UINT8 EnoughBank(const AMBA_CFS_FILE_PARAM_s *FileParam)
{
    UINT8 Ret = 1U;
    if (FileParam->AsyncEnable != 0U) {
        UINT32 BankAmount = 0U;
        if (AmbaCFS_GetBankAmount(FileParam, &BankAmount) == CFS_OK) {
            if (BankAmount == 0U) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] There must be at least one bank for async mode", __func__, NULL, NULL, NULL, NULL);
                Ret = 0U;
            } else if ((FileParam->Mode != AMBA_CFS_FILE_MODE_WRITE_ONLY) && (FileParam->Mode != AMBA_CFS_FILE_MODE_APPEND_ONLY)) {
                /* Async read */
                if (BankAmount < 2U) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] There must be at least two banks for read mode", __func__, NULL, NULL, NULL, NULL);
                    Ret = 0U;
                }
            } else {
                /* Do nothing */
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_GetBankAmount fail", __func__, NULL, NULL, NULL, NULL);
            Ret = 0U;
        }
    }
    return Ret;
}

/**
 *  Check if the file can support file size alignment
 *  @param [in]  FileParam          CFS file parameter
 *  @return 1 - TRUE, 0 - FALSE
 */
static UINT8 IsValidAlignment(const AMBA_CFS_FILE_PARAM_s *FileParam)
{
    UINT8 Ret = 1U;

    /* Check AlignMode */
    if (FileParam->AlignMode >= AMBA_CFS_ALIGN_MODE_MAX) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidAlignment: [ERROR] AlignMode %u is not supported", FileParam->AlignMode, 0U, 0U, 0U, 0U);
        Ret = 0U;
    }

    /* Check AlignSize */
    if (Ret != 0U) {
        if (FileParam->AlignMode == AMBA_CFS_ALIGN_MODE_NONE) {
            if (FileParam->AlignSize != 0U) {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidAlignment: [ERROR] AlignMode %u AlignSize %u is not supported", FileParam->AlignMode, FileParam->AlignSize, 0U, 0U, 0U);
                Ret = 0U;
            }
        } else { /* AMBA_CFS_ALIGN_MODE_SIZE or AMBA_CFS_ALIGN_MODE_CONTINUOUS */
            if ((FileParam->AlignSize < AMBA_CFS_FILE_ALIGNMENT_MIN) || (FileParam->AlignSize > AMBA_CFS_FILE_ALIGNMENT_MAX)) {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidAlignment: [ERROR] AlignMode %u AlignSize %u is not supported", FileParam->AlignMode, FileParam->AlignSize, 0U, 0U, 0U);
                Ret = 0U;
            } else {
                if ((FileParam->AlignSize % AMBA_CFS_FILE_ALIGNMENT_MIN) != 0U) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidAlignment: [ERROR] AlignSize %u is not a multiple of %u", FileParam->AlignSize, AMBA_CFS_FILE_ALIGNMENT_MIN, 0U, 0U, 0U);
                    Ret = 0U;
                }
            }
        }
    }

    /* Check Mode */
    /* Check AlignSize should be multiple of ClusterSize */
    if (Ret != 0U) {
        if (FileParam->AlignMode != AMBA_CFS_ALIGN_MODE_NONE) {
            if (FileParam->Mode != AMBA_CFS_FILE_MODE_READ_WRITE) {
                char Drive;
                if (AmbaCFS_UtilGetDrive(FileParam->Filename, &Drive) == CFS_OK) {
                    UINT32 ClusterSize;
                    // TODO Handle I/O Error. Return error code. Otherwise AmbaCFS_FileOpen_ParamCheck would return CFS_ERR_ARG instead of CFS_ERR_FS.
                    if (AmbaCFS_GetDriveClusterSize(Drive, &ClusterSize) == CFS_OK) {
                        if ((FileParam->AlignSize % ClusterSize) == 0U) {
                            /* Do nothing */
                        } else {
                            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidAlignment: [ERROR] Invalid alignment value. ClusterSize: %d, AlignSize: %d", ClusterSize, FileParam->AlignSize, 0, 0, 0);
                            Ret = 0U;
                        }
                    } else {
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidAlignment: [ERROR] Failed to get the cluster size of drive %c", (UINT8) Drive, 0, 0, 0, 0);
                        Ret = 0U;
                    }
                } else {
                    Ret = 0U;
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] File AlignMode cannot be used with read-write mode", __func__, NULL, NULL, NULL, NULL);
                Ret = 0U;
            }
        }
    }
    return Ret;
}

/**
 *  Check if the BytesToSync is valid
 *  @param [in]  BytesToSync        The number of bytes to invoke FileSync
 *  @return 1 - TRUE, 0 - FALSE
 */
static UINT8 IsValidBytesToSync(UINT32 BytesToSync)
{
    UINT8 Ret = 1U;

#if (AMBA_CFS_BYTE_TO_SYNC_MIN > 0U)
    if ((BytesToSync >= AMBA_CFS_BYTE_TO_SYNC_MIN) && (BytesToSync <= AMBA_CFS_BYTE_TO_SYNC_MAX)) {
        /* Do nothing */
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidBytesToSync: [ERROR] BytesToSync %u is not supported", BytesToSync, 0, 0, 0, 0);
        Ret = 0U;
    }
#else
    if (BytesToSync <= AMBA_CFS_BYTE_TO_SYNC_MAX) {
        /* Do nothing */
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidBytesToSync: [ERROR] BytesToSync %u is not supported", BytesToSync, 0, 0, 0, 0);
        Ret = 0U;
    }
#endif

    return Ret;
}

/**
 *  Check if the file parameters are valid
 *  @param [in]  FileParam          CFS file parameter
 *  @return 1 - TRUE, 0 - FALSE
 */
static UINT8 IsValidFileOpenParams(const AMBA_CFS_FILE_PARAM_s *FileParam)
{
    UINT8 Ret = 0U;
    if (AmbaCFS_UtilIsValidPath(FileParam->Filename) != 0U) { /* Check file path */
        if (FileParam->Mode < AMBA_CFS_FILE_MODE_MAX) {
            if ((FileParam->Mode != AMBA_CFS_FILE_MODE_APPEND_ONLY) && (FileParam->Mode != AMBA_CFS_FILE_MODE_APPEND_READ)) {
                if (IsValidAlignment(FileParam) != 0U) {
                    if (IsValidBytesToSync(FileParam->BytesToSync) != 0U) {
                        if (FileParam->AsyncEnable != 0U) {
                            /* Async */
                            if (AmbaCFS_UtilIsAlignedAddr(FileParam->BankBuffer, (UINT32)AMBA_CACHE_LINE_SIZE) != 0U) {
                                if (AmbaCFS_UtilIsAlignedU32(FileParam->BankBufferSize, (UINT32)AMBA_CACHE_LINE_SIZE) != 0U) {
                                    if (EnoughBank(FileParam) != 0U) {
                                        Ret = 1U;
                                    } else {
                                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not enough bank", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidFileOpenParams: [ERROR] BankBufferSize %u not aligned", FileParam->BankBufferSize, 0, 0, 0, 0);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] BankBuffer not aligned", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            /* Sync */
                            Ret = 1U;
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not valid BytesToSync", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not valid Alignment", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Append mode is not ready", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "IsValidFileOpenParams: [ERROR] Invalid file mode %u", FileParam->Mode, 0, 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath failed. %s", __func__, FileParam->Filename, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check if the file handler is locked by a previous handler
 *  @param [in] Stream              The previous stream
 *  @param [in] FileName            The filename to open
 *  @param [in] Mode                The open mode
 *  @return 1 - TRUE, 0 - FALSE
 */
static inline UINT8 IsMultiOpenConflict(const AMBA_CFS_STREAM_s *Stream, const char *FileName, UINT8 Mode)
{
    /* must be called with stream's mutex locked */
    UINT8 Ret = 0U;
    if (Stream->Status != AMBA_CFS_STATUS_UNUSED) {
        if (Stream->Status != AMBA_CFS_STATUS_ERROR) {
            if (Stream->Status != AMBA_CFS_STATUS_OPENED_READ) {
                if (AmbaCFS_UtilStringCompare(Stream->Filename, FileName) == 0) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] The file %s is already opened to write", __func__, FileName, NULL, NULL, NULL);
                    Ret = 1U;
                }
            } else {
                if (Mode != AMBA_CFS_FILE_MODE_READ_ONLY) {
                    if (AmbaCFS_UtilStringCompare(Stream->Filename, FileName) == 0) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] The file %s is already opened with read-only mode", __func__, FileName, NULL, NULL, NULL);
                        Ret = 1U;
                    }
                }
            }
        } else {
            if (AmbaCFS_UtilStringCompare(Stream->Filename, FileName) == 0) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] The file %s has error status", __func__, FileName, NULL, NULL, NULL);
                Ret = 1U;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] The file %s is unused", __func__, FileName, NULL, NULL, NULL);
        Ret = 1U;
    }
    return Ret;
}

/**
 *  Check free stream
 *  @param [in] Stream              CFS stream handler
 *  @param [in] FileName            The filename to open
 *  @param [in] Mode                The open mode
 *  @param [out] BankUsed           The required bank number
 *  @param [out] FreeHdlr           The free stream
 *  @return 0 - OK, others - Error
 */
static UINT32 CheckFreeStreamImpl(AMBA_CFS_STREAM_s *Stream, const char *FileName, UINT8 Mode, UINT32 *BankUsed, AMBA_CFS_STREAM_s **FreeHdlr)
{
    UINT32 Ret = CFS_OK;
    *BankUsed = 0U;
    if (Stream->Status == AMBA_CFS_STATUS_UNUSED) {
        if (Stream->File == NULL) {
            if (*FreeHdlr == NULL) {
                *FreeHdlr = Stream; /* Candidate found */
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] File is not NULL", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        if (IsMultiOpenConflict(Stream, FileName, Mode) == 0U) {
            if (Stream->AsyncEnable != 0U) {
                if ((Stream->Status == AMBA_CFS_STATUS_OPENED_READ) || (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE)) {
                    *BankUsed = AmbaCFS_GetMaxNumBank(Stream->StreamId);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Multi-open conflict", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    }
    return Ret;
}

/**
 *  Check free stream
 *  @param [in] Stream              CFS stream handler
 *  @param [in] FileName            The filename to open
 *  @param [in] Mode                The open mode
 *  @param [out] BankUsed           The required bank number
 *  @param [out] FreeHdlr           The free stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CheckFreeStream(AMBA_CFS_STREAM_s *Stream, const char *FileName, UINT8 Mode, UINT32 *BankUsed, AMBA_CFS_STREAM_s **FreeHdlr)
{
    UINT32 Ret;
    Ret = AmbaCFS_LockStream(Stream);
    if (Ret == CFS_OK) {
        Ret = CheckFreeStreamImpl(Stream, FileName, Mode, BankUsed, FreeHdlr);
        if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
            Ret = CFS_ERR_FATAL;
        }
    }
    return Ret;
}

/**
 *  Get a free stream and lock its mutex
 *  @param [in]  FileName           The filename to open
 *  @param [in]  Mode               The open mode
 *  @param [in]  BankAmount         The required bank number
 *  @param [out] FreeStream         The returned free stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 GetFreeStream(const char *FileName, UINT8 Mode, UINT32 BankAmount, AMBA_CFS_STREAM_s **FreeStream)
{
    UINT32 Ret = CFS_ERR_API;
    AMBA_CFS_STREAM_s *StreamRet = NULL;
    UINT32 StreamId = g_AmbaCFS.LastClosedStreamId; /* Searh stream from "LastClosedStreamId + 1" */
    UINT32 BankUsed = 0U;
    UINT32 OpenedStream = 0U; /* Number of streams that is opened in Async mode */
    UINT32 CmdAmountMin;      /* The minimum required number of CmdAmount */
    /*
     * Add protection to duplicate FileClose
     * If APP calls FileClose twice and then FileOpen immediately, the sequence might be "FileClose -> FileOpen -> FileClose" due to context switch.
     * If the handler given by FileClose is the same handler that is just closed by FileClose, the handler will be closed unexpectedly after the second FileClose.
     * So it will be safer to avoid opening the handler that is just closed.
     * Note that the method cannot prevent the issue completely.
     * We need APP to make sure that FileClose is called only once for each handler.
     */
    for (UINT32 i = 0U; i < AMBA_CFS_MAX_STREAM; i++) {
        UINT32 StreamBankUsed = 0U;
        AMBA_CFS_STREAM_s *StreamHdlr;

        StreamId++;
        if (StreamId >= AMBA_CFS_MAX_STREAM) {
            StreamId = 0U;
        }

        StreamHdlr = &g_AmbaCFS.StreamTbl[StreamId];

        Ret = CheckFreeStream(StreamHdlr, FileName, Mode, &StreamBankUsed, &StreamRet);
        if (Ret == CFS_OK) {
            if (StreamBankUsed != 0U) {
                BankUsed += StreamBankUsed;
                /*
                 * A Stream in Async mode must has more than 1 Bank
                 * So increase OpenedStream only when StreamBankUsed > 0
                 * Ignore the Streams in sync mode
                 */
                OpenedStream++;
            }
        } else {
            break;
        }
    }

    /*
     * Check CmdAmount
     * The sufficient number of CmdAmount is:
     *     CmdAmount = BankAmount + StreamAmount.
     * For example, assume that each stream has 4 banks and there're 9 streams. The CmdAmount can be calculated:
     *     BankAmount = 4 * 9
     *     CmdAmount = (4 * 9) + 9 = 45
     * The case that we need the most commands:
     *     1. All Async Read/Write commands: Each bank needs one command, so the number of commands is "BankAmount"
     *     2. All streams close at the same time: Each stream needs one command, so the number of commands is "StreamAmount"
     * After the new stream is opened:
     *     1. Number of Bank = BankUsed + BankAmount
     *     2. Number of Stream = OpenedStream + 1U
     *     So CmdAmount should be larger than ((BankUsed + BankAmount) + (OpenedStream + 1U))
     */
    CmdAmountMin = BankUsed + BankAmount + OpenedStream + 1U;
    if (CmdAmountMin > g_AmbaCFS.CmdAmount) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "GetFreeStream: [ERROR] CmdAmount should be at least %u. BankUsed: %u, BankRequested: %u, OpenedStream: %u, CmdAmount: %u", CmdAmountMin, BankUsed, BankAmount, OpenedStream, g_AmbaCFS.CmdAmount);
        Ret = CFS_ERR_ARG;
    }

    if (Ret == CFS_OK) {
        /* Lock candidate's mutex */
        Ret = AmbaCFS_LockStream(StreamRet);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckFreeStream fail", __func__, NULL, NULL, NULL, NULL);
    }

    /* Set return value */
    if (Ret == CFS_OK) {
        *FreeStream = StreamRet;
    }
    return Ret;
}

/**
 *  Update the file length of a CFS stream
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]  Stream             The CFS stream
 *  @param [out] FileLength         The returned file length
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 GetFileLength(const AMBA_CFS_STREAM_s *Stream, UINT64 *FileLength)
{
    /* must be called with stream's mutex locked */
    /* always be called before async I/O starts */
    UINT32 Ret = CFS_OK;
    AMBA_FS_FILE *File = Stream->File;
    if (File != NULL) {
        UINT64 CurPos = 0ULL;
        Ret = CFS_F2C(AmbaFS_FileTell(File, &CurPos));
        if (Ret == CFS_OK) {
            Ret = CFS_F2C(AmbaFS_FileSeek(File, 0LL, AMBA_FS_SEEK_END));
            if (Ret == CFS_OK) {
                Ret = CFS_F2C(AmbaFS_FileTell(File, FileLength));
                if (Ret == CFS_OK) {
                    Ret = CFS_F2C(AmbaFS_FileSeek(File, (INT64) CurPos, AMBA_FS_SEEK_START));
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "GetFileLength: [ERROR] AmbaFS_FileSeek fail. Offset:%u Origin:%u. ErrNum: %p", (UINT32)CurPos, AMBA_FS_SEEK_START, AmbaCFS_UtilGetAmbaFSError(), 0, 0);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "GetFileLength: [ERROR] AmbaFS_FileTell fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "GetFileLength: [ERROR] AmbaFS_FileSeek fail. Offset:0 Origin:%u. ErrNum: %p", AMBA_FS_SEEK_END, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "GetFileLength: [ERROR] AmbaFS_FileTell fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] File stream is corrupted", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  Initiate the file length of a CFS stream
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 InitStreamLength(AMBA_CFS_STREAM_s *Stream)
{
    /* must be called with stream's mutex locked */
    UINT32 Ret = CFS_OK;
    if (Stream->LengthGot == 0U) {
        UINT64 FileLength = 0ULL;
        Ret = GetFileLength(Stream, &FileLength);
        if (Ret == CFS_OK) {
            Stream->Length = FileLength;
            Stream->RealLength = Stream->Length;
            Stream->LengthGot = 1U;
        }
    }
    return Ret;
}

/**
 *  Add an event to event queue.
 *  @param [in] EventInfo           Event info.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_AddEventImpl(const AMBA_CFS_EVENT_INFO_s *EventInfo)
{
    UINT32 Ret = CFS_ERR_API;
    if (g_AmbaCFS.EventCfg.EventQueueNum >= g_AmbaCFS.EventCfg.EventQueueSize) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] EventQueue is full", __func__, NULL, NULL, NULL, NULL);
    } else {
        /* Copy event info to EventQueue */
        g_AmbaCFS.EventCfg.EventQueue[g_AmbaCFS.EventCfg.EventQueueNum] = *EventInfo;
        g_AmbaCFS.EventCfg.EventQueueNum++;
        Ret = CFS_K2C(AmbaKAL_EventFlagSet(&g_AmbaCFS.EventCfg.QueueFlag, AMBA_CFS_EVENT_QUEUE_FLAG_ADD));
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagSet fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Add an event to event queue.
 *  @param [in] EventInfo           Event info.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_AddEvent(const AMBA_CFS_EVENT_INFO_s *EventInfo)
{
    UINT32 Ret;
    /* This function is for internal use. No need to check parameters. */
    /* If it's needed to use AmbaCFS_Lock or AmbaCFS_LockStream instead of AmbaCFS_LockEventQueue, make sure there's no risk of deadlock. */
    Ret = AmbaCFS_LockEventQueue();
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_AddEventImpl(EventInfo);

        if (CFS_SET_RETURN(&Ret, AmbaCFS_UnlockEventQueue()) != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockEventQueue fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockEventQueue fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get all CFS events.
 *  @param [in]  Size               Size of EventInfo.
 *  @param [out] EventInfo          Event info buffer.
 *  @param [out] EventNum           Event number.
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetEventImpl(UINT32 Size, AMBA_CFS_EVENT_INFO_s *EventInfo, UINT32 *EventNum)
{
    UINT32 Ret;

    if (g_AmbaCFS.EventCfg.EventQueueNum > Size) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_GetEventImpl: [ERROR] Not enough Size %u EventQueueNum %u", Size, g_AmbaCFS.EventCfg.EventQueueNum, 0U, 0U, 0U);
        Ret = CFS_ERR_ARG;
    } else {
        /* Clear event because there might be new events between AmbaKAL_EventFlagGet() and AmbaCFS_LockEventQueue() in AmbaCFS_GetEvent(). */
        Ret = CFS_K2C(AmbaKAL_EventFlagClear(&g_AmbaCFS.EventCfg.QueueFlag, AMBA_CFS_EVENT_QUEUE_FLAG_ADD));
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagClear fail", __func__, NULL, NULL, NULL, NULL);
        }

        if (Ret == CFS_OK) {
            /* Return EventNum */
            *EventNum = g_AmbaCFS.EventCfg.EventQueueNum;
            /* Return EventInfo. Copy EventQueue to EventInfo. */
            for (UINT32 i = 0U; i < g_AmbaCFS.EventCfg.EventQueueNum; ++i) {
                EventInfo[i] = g_AmbaCFS.EventCfg.EventQueue[i];
            }
            /* Clear EventQueue */
            g_AmbaCFS.EventCfg.EventQueueNum = 0U;
        }
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in]  Size               Size of EventInfo.
 *  @param [in]  EventInfo          Event info buffer.
 *  @param [in]  EventNum           Event number.
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetEvent_ParamCheck(UINT32 Size, const AMBA_CFS_EVENT_INFO_s *EventInfo, const UINT32 *EventNum)
{
    UINT32 Ret = CFS_ERR_ARG;

    if (g_AmbaCFS_Init != 0U) {
        if (EventInfo != NULL) {
            if (EventNum != NULL) {
                if (Size > 0U) {
                    Ret = CFS_OK;
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_GetEvent_ParamCheck: [ERROR] Invalid Size %u", Size, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] EventNum is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] EventInfo is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Get all CFS events.
 *  @param [in]  Size               Size of EventInfo.
 *  @param [in]  Timeout            Timeout value in ms.
 *  @param [out] EventInfo          Event info buffer.
 *  @param [out] EventNum           Event number.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_GetEvent(UINT32 Size, UINT32 Timeout, AMBA_CFS_EVENT_INFO_s *EventInfo, UINT32 *EventNum)
{
    UINT32 Ret;
    Ret = AmbaCFS_GetEvent_ParamCheck(Size, EventInfo, EventNum);
    if (Ret == CFS_OK) {
        UINT32 KalRet;
        UINT32 ActualFlag = 0U;
        /* Wait flag out side of the critical section to avoid dead lock */
        KalRet = AmbaKAL_EventFlagGet(&g_AmbaCFS.EventCfg.QueueFlag, AMBA_CFS_EVENT_QUEUE_FLAG_ADD, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlag, Timeout);
        if (KalRet == KAL_ERR_TIMEOUT) {
            /* No event available. Return OK. */
            *EventNum = 0U;
        } else if (KalRet != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaKAL_EventFlagGet fail", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_K2C(KalRet);
        } else {
            /* If it's needed to use AmbaCFS_Lock or AmbaCFS_LockStream instead of AmbaCFS_LockEventQueue, make sure there's no risk of deadlock. */
            Ret = AmbaCFS_LockEventQueue();
            if (Ret == CFS_OK) {
                Ret = AmbaCFS_GetEventImpl(Size, EventInfo, EventNum);

                if (CFS_SET_RETURN(&Ret, AmbaCFS_UnlockEventQueue()) != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockEventQueue fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockEventQueue fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }
    return Ret;
}

/*
 * APIs called before initialization
 */

/**
 *  Check parameters.
 *  @param [in] Config              The configuration used to initialize the CFS module
 *  @param [in] Size                The required buffer size
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetInitBufferSize_ParamCheck(const AMBA_CFS_INIT_CFG_s *Config, const UINT32 *Size)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (Config != NULL) {
        if (Config->EventQueueSize > 0U) {
            if ((Config->BankSize > 0U) && (Config->CmdAmount > 0U)) {
                if ((Config->CacheEnable == 0U) || ((Config->CacheMaxDirNum > 1U) && (Config->CacheMaxFileNum > 1U))) {
                    if (Size != NULL) {
                        Ret = CFS_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Size is NULL", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_GetRequiredBufferSize_ParamCheck: [ERROR] CacheEnable %u CacheMaxDirNum %u CacheMaxFileNum %u", Config->CacheEnable, Config->CacheMaxDirNum, Config->CacheMaxFileNum, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_GetRequiredBufferSize_ParamCheck: [ERROR] BankSize %u CmdAmount %u", Config->BankSize, Config->CmdAmount, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_GetRequiredBufferSize_ParamCheck: [ERROR] Invalid EventQueueSize %u", Config->EventQueueSize, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Config is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get the required buffer size for initializing the CFS module.
 *  @param [in]  Config             The configuration used to initialize the CFS module
 *  @param [out] BufferSize         Required buffer size
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaCFS_GetInitBufferSize(const AMBA_CFS_INIT_CFG_s *Config, UINT32 *BufferSize)
{
    UINT32 Ret;
    /* Check Parameters */
    Ret = AmbaCFS_GetInitBufferSize_ParamCheck(Config, BufferSize);
    if (Ret == CFS_OK) {
        /* Buffer size for Scheduler */
        UINT32 TotalBufferSize = AmbaCFS_SchGetInitBufferSize(Config->CmdAmount);
        /* Buffer size for Cache */
        if (Config->CacheEnable != 0U) {
            TotalBufferSize += AmbaCFS_CacheGetInitBufferSize(Config->CacheMaxDirNum, Config->CacheMaxFileNum );
        }
        /* Buffer size for ActiveDta */
        TotalBufferSize += AmbaCFS_ActiveDtaGetInitBufferSize(Config->MaxSearch);
        /* Buffer size for EventQueue */
        TotalBufferSize += Config->EventQueueSize * (UINT32)sizeof(AMBA_CFS_EVENT_INFO_s);
        /* Set return value */
        *BufferSize = GetAlignedValU32(TotalBufferSize, (UINT32)AMBA_CACHE_LINE_SIZE);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] Config              The returned configuration of the CFS module
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetInitDefaultCfg_ParamCheck(const AMBA_CFS_INIT_CFG_s *Config)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (Config != NULL) {
        Ret = CFS_OK;
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Config is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get the default configuration for initializing the CFS module.
 *  @param [out] Config             The returned configuration of the CFS module
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaCFS_GetInitDefaultCfg(AMBA_CFS_INIT_CFG_s *Config)
{
    UINT32 Ret;
    Ret = AmbaCFS_GetInitDefaultCfg_ParamCheck(Config);
    if (Ret == CFS_OK) {
        Config->Buffer = NULL;
        AmbaCFS_SchGetInitDefaultCfg(Config);
        AmbaCFS_CacheGetInitDefaultCfg(Config);
        AmbaCFS_ActiveDtaGetInitDefaultCfg(Config);
        Config->EventQueueSize = AMBA_CFS_EVENT_QUEUE_SIZE_DEFAULT;
        Config->CheckCached = NULL;
        Config->CacheClean = NULL; /* Call AmbaCache_DataClean */
        Config->CacheInvalidate = NULL; /* Call AmbaCache_DataInvalidate */
        Config->VirtToPhys = NULL; /* No conversion. Return virtual address. */
        Ret = AmbaCFS_GetInitBufferSize(Config, &Config->BufferSize);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_GetInitBufferSize fail", __func__, NULL, NULL, NULL, NULL);
            if (Ret == CFS_ERR_ARG) {
                /* Error is caused by default config, not by user. So return API error. */
                Ret = CFS_ERR_API;
            }
        }
    }
    return Ret;
}

/**
 *  Check initialization config
 *  @param [in] Config              The configuration of the CFS module
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_Init_ParamCheck(const AMBA_CFS_INIT_CFG_s *Config)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init == 0U) {
        if (Config != NULL) {
            /*
             * Just print a warning if EventQueueSize is small. (It might work.)
             * EventQueueSize should be at least 1. It will be checked in AmbaCFS_GetInitBufferSize().
             */
            if (Config->EventQueueSize < Config->CmdAmount) {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_Init_Check: EventQueueSize %u less than CmdAmount %u", Config->EventQueueSize, Config->CmdAmount, 0U, 0U, 0U);
            }

            if (Config->CheckCached != NULL) { /* CacheClean, CacheInvalidate, and VirtToPhys can be NULL */
                if (Config->Buffer != NULL) {
                    if (AmbaCFS_UtilIsAlignedAddr(Config->Buffer, (UINT32)AMBA_CACHE_LINE_SIZE) != 0U) {
                        UINT32 Size = 0U;
                        Ret = AmbaCFS_GetInitBufferSize(Config, &Size);
                        if (Ret == CFS_OK) {
                            if (Config->BufferSize == Size) {
#if (AMBA_CFS_MAX_STREAM > 32U) /* due to flag limit */
                                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_Init_Check: [ERROR] AMBA_CFS_MAX_STREAM %u not supported", AMBA_CFS_MAX_STREAM, 0, 0, 0, 0);
                                Ret = CFS_ERR_API;
#endif
                            } else {
                                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_Init_Check: [ERROR] BufferSize %u not expected", Config->BufferSize, 0, 0, 0, 0);
                                Ret = CFS_ERR_ARG;
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_GetInitBufferSize fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Buffer not aligned", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Buffer is NULL", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckCached is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Config is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Already init", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Initialize the CFS module.
 *  @param [in] Config              The configuration used to initialize the CFS module
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_Init(const AMBA_CFS_INIT_CFG_s *Config)
{
    UINT32 Ret;
    /* Check parameters */
    Ret = AmbaCFS_Init_ParamCheck(Config);
    if (Ret == CFS_OK) {
        UINT8 *Buffer = Config->Buffer;
        UINT32 SchBufferSize = AmbaCFS_SchGetInitBufferSize(Config->CmdAmount);
        AMBA_CFS_CACHE_CLEAN_f CacheClean;
        AMBA_CFS_CACHE_INVALIDATE_f CacheInvalidate;
        AMBA_CFS_VIRT_TO_PHYS_f VirtToPhys;
        AmbaUtility_MemorySetU8((UINT8 *) &g_AmbaCFS, 0, (UINT32)sizeof(AMBA_CFS_s));
        /* Clear memory */
        AmbaUtility_MemorySetU8(Buffer, 0, Config->BufferSize);
        /* Initialize Scheduler module */
        if (Config->CacheClean != NULL) {
            CacheClean = Config->CacheClean;
        } else {
            // NULL. Call AmbaCache_DataClean by default.
            CacheClean = AmbaCFS_DataClean;
        }
        if (Config->CacheInvalidate != NULL) {
            CacheInvalidate = Config->CacheInvalidate;
        } else {
            // NULL. Call AmbaCache_DataInvalidate by default.
            CacheInvalidate = AmbaCFS_DataInvalidate;
        }
        if (Config->VirtToPhys != NULL) {
            VirtToPhys = Config->VirtToPhys;
        } else {
            // NULL. Call AmbaCFS_VirtToPhys by default.
            VirtToPhys = AmbaCFS_VirtToPhys;
        }
        Ret = AmbaCFS_SchInit(Buffer, SchBufferSize, Config->CmdAmount, Config->BankSize, Config->CacheEnable, Config->CheckCached, CacheClean, CacheInvalidate, VirtToPhys);
        if (Ret == CFS_OK) {
            Buffer = &Buffer[SchBufferSize];
        }
        /* Initialize Cache module */
        if (Ret == CFS_OK) {
            if (Config->CacheEnable != 0U) {
                UINT32 CacheBufferSize = AmbaCFS_CacheGetInitBufferSize(Config->CacheMaxDirNum, Config->CacheMaxFileNum);
                Ret = AmbaCFS_CacheInit(Buffer, CacheBufferSize, Config->CacheMaxDirNum, Config->CacheMaxFileNum);
                if (Ret == CFS_OK) {
                    Buffer = &Buffer[CacheBufferSize];
                }
            }
        }
        /* Initialize Active DTA module */
        if (Ret == CFS_OK) {
            UINT32 ActiveDtaBufferSize = AmbaCFS_ActiveDtaGetInitBufferSize(Config->MaxSearch);
            Ret = AmbaCFS_ActiveDtaInit(Buffer, ActiveDtaBufferSize, Config->MaxSearch, &g_AmbaCFS.ActiveDtaPool);
            if (Ret == CFS_OK) {
                Buffer = &Buffer[ActiveDtaBufferSize];
            }
        }
        /* Initialize Event Queue module */
        if (Ret == CFS_OK) {
            UINT32 EventQueueBufferSize = Config->EventQueueSize * (UINT32)sizeof(AMBA_CFS_EVENT_INFO_s);
            AmbaMisra_TypeCast(&g_AmbaCFS.EventCfg.EventQueue, &Buffer);
            g_AmbaCFS.EventCfg.EventQueueSize = Config->EventQueueSize;
            g_AmbaCFS.EventCfg.EventQueueNum = 0U;
            Buffer = &Buffer[EventQueueBufferSize];
        }
        if (Ret == CFS_OK) {
            /* Initialize global variables */
            g_AmbaCFS.CacheEnable = Config->CacheEnable;
            g_AmbaCFS.CmdAmount = Config->CmdAmount; /* Set the max number of command. */
            g_AmbaCFS.BankSize = Config->BankSize;
            for (UINT32 i = 0U; i < AMBA_CFS_MAX_STREAM; i++) {
                g_AmbaCFS.StreamTbl[i].StreamId = (UINT8) i;
            }
            g_AmbaCFS.LastClosedStreamId = AMBA_CFS_MAX_STREAM; /* Set illegal value indicating that no file has been closed so far. */
            /* Create stream mutex */
            Ret = AmbaCFS_CreateMutex();

            if (Ret == CFS_OK) {
                Ret = AmbaCFS_CreateFlag();
            }

            if (Ret == CFS_OK) {
                g_AmbaCFS_Init = 1U;
            }
        }

        AmbaMisra_TouchUnused(&Buffer);
    }
    return Ret;
}

/*
 * Stream level APIs
 */

UINT8 AmbaCFS_IsValidCfsStream(const AMBA_CFS_FILE_s *File)
{
    UINT8 Ret = 0U;
    if (File != NULL) {
        const AMBA_CFS_STREAM_s *StreamBase = g_AmbaCFS.StreamTbl;
        ULONG StreamUL;
        ULONG StreamBaseUL;
        AmbaMisra_TypeCast(&StreamUL, &File);
        AmbaMisra_TypeCast(&StreamBaseUL, &StreamBase);
        if (StreamUL >= StreamBaseUL) {
            // Code Coverage: for illegal handler
            if (((StreamUL - StreamBaseUL) / sizeof(AMBA_CFS_STREAM_s)) < AMBA_CFS_MAX_STREAM) {
                if (((StreamUL - StreamBaseUL) % sizeof(AMBA_CFS_STREAM_s)) == 0U) {
                    // Code Coverage: for illegal handler
                    Ret = 1U;
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Address not aligned", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Address too large", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Address too small", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] File is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check if the drive is supported
 *  This function is used to check parameters, so return CFS_ERR_ARG when failed.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, others - Error
 */
static UINT32 CheckDriveName(char Drive)
{
    UINT32 Ret;
    UINT32 DriveId;
    Ret = AmbaCFS_UtilGetDriveNum(Drive, &DriveId);
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] FileParam           The returned file parameters
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetFileParam_ParamCheck(const AMBA_CFS_FILE_PARAM_s *FileParam)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (FileParam != NULL) {
        Ret = CFS_OK;
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileParam is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get default file parameters for opening a file descriptor.
 *  @param [out] FileParam          The returned file parameters
 *  @return 0 - OK, others - Error
 */
UINT32 AmbaCFS_GetFileParam(AMBA_CFS_FILE_PARAM_s *FileParam)
{
    UINT32 Ret;
    Ret = AmbaCFS_GetFileParam_ParamCheck(FileParam);
    if (Ret == CFS_OK) {
        AmbaUtility_MemorySetU8((UINT8 *) FileParam, 0, (UINT32)sizeof(AMBA_CFS_FILE_PARAM_s));
        FileParam->BankBuffer = NULL;
        FileParam->BankBufferSize = AMBA_CFS_SCH_BANK_SIZE_DEFAULT * AMBA_CFS_SCH_BANK_PER_FILE_DEFAULT;
        FileParam->AlignMode = AMBA_CFS_FILE_ALIGNMENT_MODE_DEFAULT;
        FileParam->AlignSize = AMBA_CFS_FILE_ALIGNMENT_SIZE_DEFAULT;
        FileParam->BytesToSync = AMBA_CFS_BYTE_TO_SYNC_DEFAULT;
        FileParam->Mode = AMBA_CFS_FILE_MODE_READ_ONLY;
        FileParam->AsyncEnable = 0U;
        FileParam->DmaEnable = 0U; /* Use memory copy as default */
    }
    return Ret;
}

/**
 *  Set CFS file parameters for stream open
 *  @param [in] Stream              The CFS stream
 *  @param [in] FileParam           The file parameter structure
 *  @return 0 - OK, others - Error
 */
static UINT32 SetStreamParameters(AMBA_CFS_STREAM_s *Stream, const AMBA_CFS_FILE_PARAM_s *FileParam)
{
    UINT32 Ret;
    char drive;
    Ret = AmbaCFS_UtilGetDrive(FileParam->Filename, &drive);
    if (Ret == CFS_OK) {
        UINT32 DriveNum;
        Ret = AmbaCFS_UtilGetDriveNum(drive, &DriveNum);
        if (Ret == CFS_OK) {
            /* Set stream parameters */
            AmbaCFS_UtilCopyFileName(Stream->Filename, FileParam->Filename);
            Stream->Pos = 0ULL;
            Stream->AlignMode = FileParam->AlignMode;
            Stream->AlignSize = FileParam->AlignSize;
            Stream->BytesToSync = FileParam->BytesToSync;
            Stream->AccBytesNoSync = 0U;
            Stream->LengthGot = 0U;
            Stream->AsyncEnable = FileParam->AsyncEnable;
            Stream->DmaEnable = FileParam->DmaEnable;
            if (Stream->AsyncEnable != 0U) {
                Ret = AmbaCFS_SetAsyncData(Stream->StreamId, FileParam->Mode, FileParam->BankBuffer, FileParam->BankBufferSize, g_AmbaCFS.BankSize);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "SetStreamParameters: [ERROR] AmbaCFS_SetAsyncData failed", 0, 0, 0, 0, 0);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilGetDriveNum fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Convert file mode to string
 *  @param [in]  Mode               File mode (ex: AMBA_CFS_FILE_MODE_READ_ONLY)
 *  @param [out] ModeStr            File mode string for FS input
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_ModeToStr(UINT8 Mode, const char **ModeStr)
{
    UINT32 Ret = CFS_OK;

    switch (Mode) {
    case AMBA_CFS_FILE_MODE_READ_ONLY:
        *ModeStr = "r";
        break;
    case AMBA_CFS_FILE_MODE_READ_WRITE:
        *ModeStr = "r+";
        break;
    case AMBA_CFS_FILE_MODE_WRITE_ONLY:
        *ModeStr = "w";
        break;
    case AMBA_CFS_FILE_MODE_WRITE_READ:
        *ModeStr = "w+";
        break;
    case AMBA_CFS_FILE_MODE_APPEND_ONLY:
        *ModeStr = "a";
        break;
    case AMBA_CFS_FILE_MODE_APPEND_READ:
        *ModeStr = "a+";
        break;
    default:
        /* Mode should be checked already, so return API error */
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_ModeToStr: [ERROR] Illegal mode %u", Mode, 0U, 0U, 0U, 0U);
        Ret = CFS_ERR_API;
        break;
    }

    return Ret;
}

/**
 *  Check whether new file is created when file open
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]  FileName           File name
 *  @param [in]  Mode               File mode
 *  @param [out] IsFileCreate       Whether new file is created. 0: File already exists. 1: New file created.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileOpen_CheckFileCreate(const char * const FileName, UINT8 Mode, UINT8 *IsFileCreate)
{
    UINT32 Ret = CFS_OK;

    /*
     * Check whether the file exists before FileOpen. To determine whether to invoke AMBA_CFS_EVENT_CREATE event.
     * So only need to check the modes that could create file.
     * Don't use AmbaCFS_GetFileInfoImpl() to check because it locks all streams to check file status (which is not needed here).
     */
    if ((Mode == AMBA_CFS_FILE_MODE_WRITE_ONLY) || (Mode == AMBA_CFS_FILE_MODE_WRITE_READ)) {
        /*
         * Since GetFreeStream() passed and this is in write mode, we can assume that "FileName" is not opened.
         * Unless someone opens "FileName" by AmbaFS_FileOpen directly. (This should not happen.)
         * So "GetFileInfo" can be used to check whether the file exists.
         * Possible cases:
         * 1. File existing and not opened.
         * 2. File not existing and not opened.
         * 3. File existing and opened by AmbaFS directly.
         */
        AMBA_CFS_FILE_INFO_s Info = {0};
        /* Get file info */
        if (g_AmbaCFS.CacheEnable != 0U) {
            Ret = AmbaCFS_CacheGetFileInfo(FileName, &Info);
        } else {
            Ret = CFS_F2C(AmbaFS_GetFileInfo(FileName, &Info));
        }

        if (Ret == CFS_OK) {
            /* Case 1: File existing and not opened. */
            *IsFileCreate = 0U;
        } else if (Ret == CFS_ERR_OBJ_UNAVAILABLE) { /* No such file or directory */
            /* Case 2: File not existing and not opened. */
            /* Return OK. */
            *IsFileCreate = 1U;
            Ret = CFS_OK;
        } else {
            /* Case 3. File existing and opened by AmbaFS directly. Cannot handle this case. Return error. */
            /* Case 4. I/O error occurred. */
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileOpenImpl: [ERROR] Failed to get file info. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
        }
    } else {
        *IsFileCreate = 0U;
    }

    return Ret;
}

/**
 *  Invoke callback when file open
 *  @param [in] FileName            File name
 *  @param [in] Mode                File mode
 *  @param [in] IsFileCreate        Whether a new file is created
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileOpenCallback(const char * const FileName, UINT8 Mode, UINT8 IsFileCreate)
{
    UINT32 Ret;
    AMBA_CFS_EVENT_INFO_s Param;
    Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
    } else {
        AmbaCFS_UtilCopyFileName(Param.Name, FileName);
        Param.Mode = Mode;
    }
    /* Trigger CREATE first, and then FOPEN. */
    /* When the file does not exist, invoke AMBA_CFS_EVENT_CREATE event. */
    if ((Ret == CFS_OK) && (IsFileCreate != 0U)) {
        Param.Event = AMBA_CFS_EVENT_CREATE;
        Ret = AmbaCFS_AddEvent(&Param);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent AMBA_CFS_EVENT_CREATE fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Ret == CFS_OK) {
        Param.Event = AMBA_CFS_EVENT_FOPEN;
        Ret = AmbaCFS_AddEvent(&Param);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent AMBA_CFS_EVENT_FOPEN fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Open a CFS stream
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileParam           The parameters used to open a file descriptor
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileOpenImpl(const AMBA_CFS_FILE_PARAM_s *FileParam, AMBA_CFS_STREAM_s *Stream)
{
    UINT32 Ret;
    const char *FileAccessModeStr = NULL;
    const char * const FileName = FileParam->Filename;
    const UINT8 Mode = FileParam->Mode;
    UINT8 IsFileCreate = 0U; /* 0: File already exists. 1: New file created. */

    /* Convert file mode to string */
    Ret = AmbaCFS_ModeToStr(Mode, &FileAccessModeStr);

    if (Ret == CFS_OK) {
        /* Check whether the file exists before FileOpen. To determine whether to invoke AMBA_CFS_EVENT_CREATE event. */
        Ret = AmbaCFS_FileOpen_CheckFileCreate(FileName, Mode, &IsFileCreate);
    }

    if (Ret == CFS_OK) {
        AMBA_FS_FILE *File = NULL;
        Ret = CFS_F2C(AmbaFS_FileOpen(FileName, FileAccessModeStr, &File));
        if (Ret == CFS_OK) {
            /* If write mode, call FileSync to ensure the file exists in FAT. */
            if (Mode >= AMBA_CFS_FILE_MODE_WRITE_ONLY) {
                /* could be new file, force sync */
                Ret = CFS_F2C(AmbaFS_FileSync(File));
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileOpenImpl: [ERROR] AmbaFS_FileSync fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                }
            }
            if (Ret == CFS_OK) {
                Ret = SetStreamParameters(Stream, FileParam);
                if (Ret == CFS_OK) {
                    /* Call callback function */
                    Ret = AmbaCFS_FileOpenCallback(FileName, Mode, IsFileCreate);
                    if (Ret == CFS_OK) {
                        Stream->Status = (Mode == AMBA_CFS_FILE_MODE_READ_ONLY) ? AMBA_CFS_STATUS_OPENED_READ : AMBA_CFS_STATUS_OPENED_WRITE;
                        Stream->File = File;
                    }
                }
            }
            if (Ret != CFS_OK) {
                if (CFS_F2C(AmbaFS_FileClose(File)) == CFS_OK) {
                    if (Mode >= AMBA_CFS_FILE_MODE_WRITE_ONLY) {
                        /* delete file on error, but could be NG on append mode... */
                        if (CFS_F2C(AmbaFS_Remove(FileName)) != CFS_OK) {
                            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileOpenImpl: [ERROR] AmbaFS_Remove fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileOpenImpl: [ERROR] AmbaFS_FileClose fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                }
            }
            /* Update Cache Status */
            if ((Ret == CFS_OK) && (Mode >= AMBA_CFS_FILE_MODE_READ_WRITE)) {
                if (g_AmbaCFS.CacheEnable != 0U) {
                    Ret = AmbaCFS_CacheRemove(FileParam->Filename);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileOpenImpl: [ERROR] AmbaFS_FileOpen fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
        }
    }
    return Ret;
}

/**
 *  Check file open config
 *  @param [in] FileParam           The parameters used to open a file descriptor
 *  @param [in] File                The returned file descriptor
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileOpen_ParamCheck(const AMBA_CFS_FILE_PARAM_s *FileParam, AMBA_CFS_FILE_s * const *File)
{
    UINT32 Ret = CFS_ERR_ARG;

    if (g_AmbaCFS_Init != 0U) {
        if (FileParam != NULL) {
            if (File != NULL) {
                if (IsValidFileOpenParams(FileParam) != 0U) {
                    Ret = CFS_OK;
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] IsValidFileOpenParams fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] File is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileParam is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Open a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileParam           The parameters used to open a file descriptor
 *  @param [out] File               The returned file descriptor
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileOpen(const AMBA_CFS_FILE_PARAM_s *FileParam, AMBA_CFS_FILE_s **File)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileOpen_ParamCheck(FileParam, File);
    if (Ret == CFS_OK) {
        /*
         * Lock g_AmbaCFS.Mutex to avoid that multiple tasks open the same file at the same time and then cause conflict.
         * TODO: Sometimes FileOpen could be time consuming. Find a way to prevent conflict and no need to lock all streams in FileOpen.
         * Consider the below case without the protection of AmbaCFS_Lock:
         * 1.  Assume that there're totally 2 Streams in g_AmbaCFS.StreamTbl. All streams are already used for open file.
         * 2.  Task1 and Task2 are openning file with the same filename in Write Only mode. (One of the tasks should get error.)
         *     Both tasks run GetFreeStream() at the same time. (Task1 is ahead of Task2.)
         * 3.  Stream1 is not locked. Stream2 is locked by Task3.
         * 4.  Task1 checks Stream1. Waiting for Stream2.
         * 5.  Stream1 is locked by Task4.
         * 6.  Task2 is waiting for Stream1.
         * 7.  Stream1 is closed and unlocked.
         * 8.  Task2 gets Stream1 as free stream. Waiting for Stream2.
         * 9.  Stream2 is closed and unlocked.
         * 10. Task2 locks Stream2. Check Stream2 and passed.
         * 11. Task1 gets Stream2 as free stream and passed.
         * ==> Issue: Both tasks passed. The conflict was not detected !!
         */
        Ret = AmbaCFS_LockFileOpen();
        if (Ret == CFS_OK) {
            UINT32 BankAmount = 0U;
            Ret = AmbaCFS_GetBankAmount(FileParam, &BankAmount);
            if (Ret == CFS_OK) {
                AMBA_CFS_STREAM_s *Stream = NULL;
                Ret = GetFreeStream(FileParam->Filename, FileParam->Mode, (FileParam->AsyncEnable != 0U) ? BankAmount : 0U, &Stream);
                if (Ret == CFS_OK) {
                    /* Stream's mutex has been locked */
                    Ret = AmbaCFS_FileOpenImpl(FileParam, Stream);
                    if (Ret == CFS_OK) {
                        AMBA_CFS_FILE_s *FileHdlr;
                        AmbaMisra_TypeCast(&FileHdlr, &Stream);
                        *File = FileHdlr;
                    }
                    if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {  /* unlock Stream's mutex */
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                        Ret = CFS_ERR_FATAL;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] GetFreeStream fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_GetBankAmount fail", __func__, NULL, NULL, NULL, NULL);
            }

            if (AmbaCFS_UnlockFileOpen() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockFileOpen fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockFileOpen fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Close a CFS stream.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileCloseImpl(AMBA_CFS_STREAM_s *Stream)
{
    /* Close the file */
    if (Stream->Status == AMBA_CFS_STATUS_OPENED_READ) {
        Stream->Status = AMBA_CFS_STATUS_CLOSING_READ;
    } else if (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE) {
        Stream->Status = AMBA_CFS_STATUS_CLOSING_WRITE;
    } else {
        /* Do nothing */
    }
    /* Update LastClosedStreamId */
    g_AmbaCFS.LastClosedStreamId = Stream->StreamId;
    return (Stream->AsyncEnable != 0U) ? AmbaCFS_SchAsyncfclose(Stream) : AmbaCFS_SchSyncfclose(Stream);
}

/**
 *  Check parameters.
 *  @param [in] File                The file descriptor
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileClose_ParamCheck(const AMBA_CFS_FILE_s *File)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (AmbaCFS_IsValidCfsStream(File) != 0U) {
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Close a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] File                The file descriptor
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileClose(AMBA_CFS_FILE_s *File)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileClose_ParamCheck(File);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s *Stream;
        /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            if ((Stream->Status == AMBA_CFS_STATUS_OPENED_READ) || (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE) || (Stream->Status == AMBA_CFS_STATUS_ERROR)) {
                Ret = AmbaCFS_FileCloseImpl(Stream);
            } else {
                /*
                 * File is closing or already closed
                 * Return I/O Error indicating that this error is recoverable
                 */
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileClose: [ERROR] Incorrect file status %u. File is closing or already closed.", Stream->Status, 0U, 0U, 0U, 0U);
                Ret = CFS_ERR_FS;
            }
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  Read from a CFS stream.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Buffer              The buffer in which data elements are stored
 *  @param [in] Size                The size of each data element being read
 *  @param [in] Count               The number of the data elements
 *  @param [in] Stream              The CFS stream
 *  @param [out] NumSuccess         The returned number of elements read
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileReadImpl(void *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess)
{
    UINT32 Ret;
    Ret = InitStreamLength(Stream);
    if (Ret == CFS_OK) {
        UINT8 *BufferAddr;
        UINT32 ReadCount = 0U;
        AmbaMisra_TypeCast(&BufferAddr, &Buffer);
        if (Stream->AsyncEnable != 0U) {
            Ret = AmbaCFS_SchAsyncfread(BufferAddr, Size, Count, Stream, &ReadCount);
        } else {
            Ret = AmbaCFS_SchSyncfread(BufferAddr, Size, Count, Stream, &ReadCount);
        }
        if (Ret != CFS_ERR_FATAL) {
            /* Part of the data might be done. So set the return value even if the function failed. */
            Stream->Pos += (UINT64) Size * ReadCount;
            /* Set return value even when error occurs */
            *NumSuccess = ReadCount;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] InitStreamLength fail", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Buffer);
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] Buffer              The buffer in which data elements are stored
 *  @param [in] Size                The size of each data element being read
 *  @param [in] Count               The number of the data elements
 *  @param [in] File                The file descriptor
 *  @param [in] NumSuccess          The returned number of elements read
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileRead_ParamCheck(const void *Buffer, UINT32 Size, UINT32 Count, const AMBA_CFS_FILE_s *File, const UINT32 *NumSuccess)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (Buffer != NULL) {
            if (NumSuccess != NULL) {
                if (Size != 0U) {
                    if (Count != 0U) {
                        if (AmbaCFS_IsValidCfsStream(File) != 0U) {
                            Ret = CFS_OK;
                        } else {
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Count is 0", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Size is 0", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] NumSuccess is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Buffer is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Read data elements from a file, and stores them in a buffer.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Buffer              The buffer in which data elements are stored
 *  @param [in] Size                The size of each data element being read
 *  @param [in] Count               The number of the data elements
 *  @param [in] File                The file descriptor
 *  @param [out] NumSuccess         The returned number of elements read
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileRead(void *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_FILE_s *File, UINT32 *NumSuccess)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileRead_ParamCheck(Buffer, Size, Count, File, NumSuccess);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s *Stream;   /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            if ((Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE) || (Stream->Status == AMBA_CFS_STATUS_OPENED_READ)) {
                /* Read data from file */
                Ret = AmbaCFS_FileReadImpl(Buffer, Size, Count, Stream, NumSuccess);
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileRead: [ERROR] Incorrect file status %u", Stream->Status, 0, 0, 0, 0);
                Ret = CFS_ERR_API;
            }
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  Write data to a CFS stream.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Buffer              The buffer from which data elements are read
 *  @param [in] Size                The size of each data element being written
 *  @param [in] Count               The number of the data elements
 *  @param [in] Stream              The CFS stream
 *  @param [out] NumSuccess         The returned number of elements written
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileWriteImpl(const void *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess)
{
    UINT32 Ret;
    Ret = InitStreamLength(Stream);
    if (Ret == CFS_OK) {
        const UINT64 DataSize = (UINT64) Size * Count;
        UINT8 *BufferAddr;
        UINT32 WriteCount = 0U;
        /* Update pos and length */
        if ((Stream->Pos + DataSize) > Stream->Length) {
            Stream->Length = Stream->Pos + DataSize;
            if (((Stream->AlignMode == AMBA_CFS_ALIGN_MODE_SIZE) || (Stream->AlignMode == AMBA_CFS_ALIGN_MODE_CONTINUOUS)) && (Stream->AlignSize != 0U)) {
                const UINT64 AlignSize = Stream->AlignSize;
                /* Align Length to AlighSize. Do not use GetAlignedValU64 because AlignSize might not be power of 2. */
                if (AlignSize != 0U) {
                    Stream->Length = ((Stream->Length + AlignSize - 1ULL) / AlignSize) * AlignSize;
                }
            }
        }
        /* Process fwrite command */
        AmbaMisra_TypeCast(&BufferAddr, &Buffer);
        if (Stream->AsyncEnable != 0U) {
            Ret = AmbaCFS_SchAsyncfwrite(BufferAddr, Size, Count, Stream, &WriteCount);
        } else {
            Ret = AmbaCFS_SchSyncfwrite(BufferAddr, Size, Count, Stream, &WriteCount);
        }
        if (Ret != CFS_ERR_FATAL) {
            /* Part of the data might be done. So set the return value even if the function failed. */
            Stream->Pos += (UINT64) Size * WriteCount;
            /* Set return value even when error occurs */
            *NumSuccess = WriteCount;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] InitStreamLength fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] Buffer              The buffer from which data elements are read
 *  @param [in] Size                The size of each data element being written
 *  @param [in] Count               The number of the data elements
 *  @param [in] File                The file descriptor
 *  @param [in] NumSuccess          The returned number of elements written
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileWrite_ParamCheck(const void *Buffer, UINT32 Size, UINT32 Count, const AMBA_CFS_FILE_s *File, const UINT32 *NumSuccess)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (Buffer != NULL) {
            if (NumSuccess != NULL) {
                if (Size != 0U) {
                    if (Count != 0U) {
                        if (AmbaCFS_IsValidCfsStream(File) != 0U) {
                            Ret = CFS_OK;
                        } else {
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Count is 0", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Size is 0", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] NumSuccess is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Buffer is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Write data elements from a buffer to a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Buffer              The buffer from which data elements are read
 *  @param [in] Size                The size of each data element being written
 *  @param [in] Count               The number of the data elements
 *  @param [in] File                The file descriptor
 *  @param [out] NumSuccess         The returned number of elements written
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileWrite(void *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_FILE_s *File, UINT32 *NumSuccess)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileWrite_ParamCheck(Buffer, Size, Count, File, NumSuccess);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s *Stream;
        /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            if (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE) {
                /* Write data */
                Ret = AmbaCFS_FileWriteImpl(Buffer, Size, Count, Stream, NumSuccess);
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileWrite: [ERROR] Incorrect file status %u", Stream->Status, 0, 0, 0, 0);
                Ret = CFS_ERR_API;
            }
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(Buffer);
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  Seek a CFS stream.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Offset              The number of bytes to offset from the origin position
 *  @param [in] Origin              The position used as reference for the offset
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileSeekImpl(AMBA_CFS_STREAM_s *Stream, INT64 Offset, INT32 Origin)
{
    UINT32 Ret = CFS_OK;
    if ((Origin == AMBA_CFS_SEEK_START) || (Origin == AMBA_CFS_SEEK_CUR) || (Origin == AMBA_CFS_SEEK_END)) {
        INT64 FileOffset = Offset;
        /* Evaluate new position */
        switch (Origin) {
        case AMBA_CFS_SEEK_CUR:
            FileOffset += (INT64) Stream->Pos;
            break;
        case AMBA_CFS_SEEK_END:
            Ret = InitStreamLength(Stream);
            if (Ret == CFS_OK) {
                FileOffset += (INT64) Stream->Length;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] InitStreamLength fail", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        default:
            /* Do nothing */
            break;
        }
        if (Ret == CFS_OK) {
            if (FileOffset < 0) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Incorrect position", __func__, NULL, NULL, NULL, NULL);
                /* FileOffset is calculated from user input 'Offset' and 'Origin'. So return ARG error. */
                Ret = CFS_ERR_ARG;
            } else {
                Stream->Pos = (UINT64) FileOffset;
            }
        }
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileSeekImpl: [ERROR] Origin %u not supported", (UINT32) Origin, 0, 0, 0, 0);
        /* Not expected. Should be checked already. */
        Ret = CFS_ERR_API;
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] File                The file descriptor
 *  @param [in] Origin              The position used as reference for the offset
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileSeek_ParamCheck(const AMBA_CFS_FILE_s *File, INT32 Origin)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if ((Origin == AMBA_CFS_SEEK_START) || (Origin == AMBA_CFS_SEEK_CUR) || (Origin == AMBA_CFS_SEEK_END)) {
            if (AmbaCFS_IsValidCfsStream(File) != 0U) {
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileSeek_ParamCheck: [ERROR] Origin %u not supported", (UINT32) Origin, 0, 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Move file I/O pointer.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] File                The file descriptor
 *  @param [in] Offset              The number of bytes to offset from the origin position
 *  @param [in] Origin              The position used as reference for the offset
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileSeek(AMBA_CFS_FILE_s *File, INT64 Offset, INT32 Origin)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileSeek_ParamCheck(File, Origin);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s *Stream;
        /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            if ((Stream->Status == AMBA_CFS_STATUS_OPENED_READ) || (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE)) {
                Ret = AmbaCFS_FileSeekImpl(Stream, Offset, Origin);
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileSeek: [ERROR] Incorrect file status %u", Stream->Status, 0, 0, 0, 0);
                Ret = CFS_ERR_API;
            }
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  The implementation of getting the current position of a CFS stream.
 *  @param [in] Stream              The CFS stream
 *  @param [out] FilePos            The returned position of the file
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileTellImpl(const AMBA_CFS_STREAM_s *Stream, UINT64 *FilePos)
{
    *FilePos = Stream->Pos;
    return CFS_OK;
}

/**
 *  Check parameters.
 *  @param [in] File                The file descriptor
 *  @param [in] FilePos             The returned position of the file
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileTell_ParamCheck(const AMBA_CFS_FILE_s *File, const UINT64 *FilePos)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (FilePos != NULL) {
            if (AmbaCFS_IsValidCfsStream(File) != 0U) {
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FilePos is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get current file I/O pointer.
 *  @param [in] File                The file descriptor
 *  @param [out] FilePos            The returned position of the file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileTell(AMBA_CFS_FILE_s *File, UINT64 *FilePos)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileTell_ParamCheck(File, FilePos);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s *Stream;
        /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            if ((Stream->Status == AMBA_CFS_STATUS_OPENED_READ) || (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE) || (Stream->Status == AMBA_CFS_STATUS_ERROR)) {
                Ret = AmbaCFS_FileTellImpl(Stream, FilePos);
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileTell: [ERROR] Incorrect file status %u", Stream->Status, 0, 0, 0, 0);
                Ret = CFS_ERR_API;
            }
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  Sync a CFS stream.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileSyncImpl(AMBA_CFS_STREAM_s *Stream)
{
    UINT32 Ret = CFS_OK;
    if (Stream->Status != AMBA_CFS_STATUS_OPENED_WRITE) {
        /* Do nothing */
    } else { /* Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE */
        /* Sync the File */
        if (Stream->AsyncEnable != 0U) {
            Ret = AmbaCFS_SchAsyncFileSync(Stream);
        } else {
            Ret = AmbaCFS_SchSyncFileSync(Stream);
        }
        if ((Ret == CFS_OK) && (g_AmbaCFS.CacheEnable != 0U)) {
            Ret = AmbaCFS_CacheRemove(Stream->Filename);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] File                The file descriptor
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileSync_ParamCheck(const AMBA_CFS_FILE_s *File)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (AmbaCFS_IsValidCfsStream(File) != 0U) {
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Flush all data in the cache for the specified file to media.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] File                The file descriptor
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileSync(AMBA_CFS_FILE_s *File)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileSync_ParamCheck(File);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s *Stream;
        /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            if ((Stream->Status == AMBA_CFS_STATUS_OPENED_READ) || (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE)) {
                Ret = AmbaCFS_FileSyncImpl(Stream);
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileSync: [ERROR] Incorrect file status %u", Stream->Status, 0, 0, 0, 0);
                Ret = CFS_ERR_API;
            }
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  Get the length of a CFS stream.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [out] FileLen            The returned length of the file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileGetLenImpl(AMBA_CFS_STREAM_s *Stream, UINT64 *FileLen)
{
    UINT32 Ret;
    if (Stream->Status == AMBA_CFS_STATUS_ERROR) {
        Stream->LengthGot = 0U;
    }
    Ret = InitStreamLength(Stream);
    if (Ret == CFS_OK) {
        *FileLen = Stream->Length;
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] InitStreamLength fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] File                The file descriptor
 *  @param [in] FileLen             The returned length of the file
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileGetLen_ParamCheck(const AMBA_CFS_FILE_s *File, const UINT64 *FileLen)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (FileLen != NULL) {
            if (AmbaCFS_IsValidCfsStream(File) != 0U) {
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileLen is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get the length of a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] File                The file descriptor
 *  @param [out] FileLen            The returned length of the file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileGetLen(AMBA_CFS_FILE_s *File, UINT64 *FileLen)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileGetLen_ParamCheck(File, FileLen);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s *Stream;
        /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            if ((Stream->Status == AMBA_CFS_STATUS_OPENED_READ) || (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE) || (Stream->Status == AMBA_CFS_STATUS_ERROR)) {
                Ret = AmbaCFS_FileGetLenImpl(Stream, FileLen);
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileGetLen: [ERROR] Incorrect file status %u", Stream->Status, 0, 0, 0, 0);
                Ret = CFS_ERR_API;
            }
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  The implementation of adding consecutive clusters of the specified size to the end of a stream.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Size                The size of the area to be added (bytes)
 *  @param [out] NumSuccess         The returned size (bytes) of the added area
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_ClusterAppendImpl(AMBA_CFS_STREAM_s *Stream, UINT64 Size, UINT64 *NumSuccess)
{
    UINT32 Ret;
    if (Stream->AsyncEnable != 0U) {
        Ret = AmbaCFS_SchAsyncfappend(Stream, Size, NumSuccess);
    } else {
        Ret = AmbaCFS_SchSyncfappend(Stream, Size, NumSuccess);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] File                The file descriptor
 *  @param [in] Size                The size of the area to be added (bytes)
 *  @param [in] NumSuccess          The returned size (bytes) of the added area
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_ClusterAppend_ParamCheck(const AMBA_CFS_FILE_s *File, UINT64 Size, const UINT64 *NumSuccess)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (Size > 0ULL) {
            if (NumSuccess != NULL) {
                if (AmbaCFS_IsValidCfsStream(File) != 0U) {
                    Ret = CFS_OK;
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] NumSuccess is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Size is 0", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Add consecutive clusters of a specified size to the end of a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] File                The file descriptor
 *  @param [in] Size                The size of the area to be added (bytes)
 *  @param [out] NumSuccess         The returned size (bytes) of the added area
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ClusterAppend(AMBA_CFS_FILE_s *File, UINT64 Size, UINT64 *NumSuccess)
{
    UINT32 Ret;
    Ret = AmbaCFS_ClusterAppend_ParamCheck(File, Size, NumSuccess);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s *Stream;
        /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            if (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE) {
                Ret = AmbaCFS_ClusterAppendImpl(Stream, Size, NumSuccess);
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_ClusterAppend: [ERROR] Incorrect file status %u", Stream->Status, 0, 0, 0, 0);
                Ret = CFS_ERR_API;
            }
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  Check if a CFS stream reaches EOF.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [out] IsEOF              The returned result of EOF. 1 - The EOF has been reached, 0 - The EOF has not been reached.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileEofImpl(AMBA_CFS_STREAM_s *Stream, UINT32 *IsEOF)
{
    UINT32 Ret;
    Ret = InitStreamLength(Stream);
    if (Ret == CFS_OK) {
        /* Set return value */
        if (Stream->Pos == Stream->Length) {
            *IsEOF = 1U;
        } else {
            *IsEOF = 0U;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] InitStreamLength fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] File                The file descriptor
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileEof_ParamCheck(const AMBA_CFS_FILE_s *File)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (AmbaCFS_IsValidCfsStream(File) != 0U) {
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check the end of file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] File                The file descriptor
 *  @return 1 - The EOF has been reached, 0 - The EOF has not been reached, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileEof(AMBA_CFS_FILE_s *File)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileEof_ParamCheck(File);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s * Stream;
        /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            if ((Stream->Status == AMBA_CFS_STATUS_OPENED_READ) || (Stream->Status == AMBA_CFS_STATUS_OPENED_WRITE) || (Stream->Status == AMBA_CFS_STATUS_ERROR)) {
                UINT32 IsEOF = 0U;
                Ret = AmbaCFS_FileEofImpl(Stream, &IsEOF);
                if (Ret == CFS_OK) {
                    /* Success. Return 1 (The EOF has been reached) or 0 (The EOF has not been reached). */
                    Ret = IsEOF;
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileEof: [ERROR] Incorrect file status %u", Stream->Status, 0, 0, 0, 0);
                Ret = CFS_ERR_API;
            }
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/**
 *  Getting the status of a file.
 *  @param [in] Stream              The CFS stream
 *  @param [in] FileName            The file path used to get the file status
 *  @param [out] FileStatus         File status
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_GetFileStatus(AMBA_CFS_STREAM_s *Stream, const char *FileName, UINT32 *FileStatus)
{
    UINT32 Ret;
    Ret = AmbaCFS_LockStream(Stream);
    if (Ret == CFS_OK) {
        if (Stream->Status != AMBA_CFS_STATUS_UNUSED) {
            if (AmbaCFS_UtilStringCompare(FileName, Stream->Filename) == 0) {
                *FileStatus = Stream->Status;
            }
        }
        Ret = CFS_OK;
        if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
            *FileStatus = AMBA_CFS_STATUS_ERROR;
            Ret = CFS_ERR_FATAL;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        *FileStatus = AMBA_CFS_STATUS_ERROR;
    }
    return Ret;
}

/**
 *  The implementation of getting the status of a file.
 *  @param [in] FileName            The file path used to get the file status
 *  @param [out] Status             The returned status of the file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FStatusImpl(const char *FileName, UINT32 *Status)
{
    UINT32 Ret = CFS_OK;
    *Status = AMBA_CFS_STATUS_UNUSED;
    /* File path should be checked before calling this function */
    for (UINT32 i = 0U; i < AMBA_CFS_MAX_STREAM; i++) {
        AMBA_CFS_STREAM_s *Stream = &g_AmbaCFS.StreamTbl[i];
        Ret = AmbaCFS_GetFileStatus(Stream, FileName, Status);
        if ((Ret != CFS_OK) || (*Status != AMBA_CFS_STATUS_UNUSED)) {
            break;
        }
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] FileName            The file path used to get the file status
 *  @param [in] Status              The returned status of the file
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileStatus_ParamCheck(const char *FileName, const UINT32 *Status)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (Status != NULL) {
            if (FileName != NULL) {
                if (AmbaCFS_UtilIsValidPath(FileName) != 0U) { /* Check file path */
                    Ret = CFS_OK;
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. %s", __func__, FileName, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileName is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Status is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get file or directory information.
 *  @param [in] FileName            The file path used to get the file status
 *  @param [out] Status             The returned status of the file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileStatus(const char *FileName, UINT32 *Status)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileStatus_ParamCheck(FileName, Status);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_FStatusImpl(FileName, Status);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get file status. %s", __func__, FileName, NULL, NULL, NULL);
            }
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                *Status = AMBA_CFS_STATUS_ERROR;
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of removing a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            The path of a file being removed.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_RemoveImpl(const char *FileName)
{
    UINT32 Ret;
    UINT32 FileStatus = AMBA_CFS_STATUS_ERROR;
    Ret = AmbaCFS_FStatusImpl(FileName, &FileStatus);
    if (Ret == CFS_OK) {
        if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check file status */
            Ret = CFS_F2C(AmbaFS_Remove(FileName)); /* Remove file */
            if (Ret == CFS_OK) {
                /* Update Cache Status */
                if (g_AmbaCFS.CacheEnable != 0U) {
                    Ret = AmbaCFS_CacheRemove(FileName);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                /* Trigger REMOVE event */
                if (Ret == CFS_OK) {
                    AMBA_CFS_EVENT_INFO_s Param;
                    Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        Param.Event = AMBA_CFS_EVENT_REMOVE;
                        AmbaCFS_UtilCopyFileName(Param.Name, FileName);
                        Ret = AmbaCFS_AddEvent(&Param);
                        if (Ret != CFS_OK) {
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_RemoveImpl: [ERROR] AmbaFS_Remove fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_RemoveImpl: [ERROR] File is opened. Statue %u", FileStatus, 0, 0, 0, 0);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get file status. %s", __func__, FileName, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] FileName            The path of a file being removed.
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_Remove_ParamCheck(const char *FileName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (FileName != NULL) {
            if (AmbaCFS_UtilIsValidPath(FileName) != 0U) { /* Check file path */
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. %s", __func__, FileName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Delete a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            The path of a file being removed.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_Remove(const char *FileName)
{
    UINT32 Ret;
    Ret = AmbaCFS_Remove_ParamCheck(FileName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_RemoveImpl(FileName);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of moving a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] SrcName             The path of a source file
 *  @param [in] DstName             The path of a destination file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_MoveImpl(const char *SrcName, const char *DstName)
{
    UINT32 Ret;
    UINT32 FileStatus = AMBA_CFS_STATUS_ERROR;
    Ret = AmbaCFS_FStatusImpl(SrcName, &FileStatus);
    if (Ret == CFS_OK) {
        if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check source file */
            Ret = AmbaCFS_FStatusImpl(DstName, &FileStatus);
            if (Ret == CFS_OK) {
                if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check destination file */
                    Ret = CFS_F2C(AmbaFS_Move(SrcName, DstName)); /* Move the file */
                    if (Ret == CFS_OK) {
                        /* Update Cache Status */
                        if (g_AmbaCFS.CacheEnable != 0U) {
                            Ret = AmbaCFS_CacheRemove(SrcName);
                            if (Ret == CFS_OK) {
                                Ret = AmbaCFS_CacheRemove(DstName);
                                if (Ret != CFS_OK) {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail. DstName %s", __func__, DstName, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail. SrcName %s", __func__, SrcName, NULL, NULL, NULL);
                            }
                        }
                        /* Trigger REMOVE and CREATE event */
                        if (Ret == CFS_OK) {
                            AMBA_CFS_EVENT_INFO_s Param;
                            Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
                            if (Ret != CFS_OK) {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                            } else {
                                Param.Event = AMBA_CFS_EVENT_REMOVE;
                                AmbaCFS_UtilCopyFileName(Param.Name, SrcName);
                                Ret = AmbaCFS_AddEvent(&Param);
                                if (Ret == CFS_OK) {
                                    Param.Event = AMBA_CFS_EVENT_CREATE;
                                    AmbaCFS_UtilCopyFileName(Param.Name, DstName);
                                    Ret = AmbaCFS_AddEvent(&Param);
                                    if (Ret != CFS_OK) {
                                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent AMBA_CFS_EVENT_CREATE fail", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent AMBA_CFS_EVENT_REMOVE fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "AmbaCFS_MoveImpl: [ERROR] AmbaFS_Move SrcPath:%s DstPath:%s", SrcName, DstName, NULL, NULL, NULL);
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_MoveImpl: [ERROR] AmbaFS_Move fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Destination file is opened", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_API;
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get Destination file status. %s", __func__, DstName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Source file is opened", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get Source file status. %s", __func__, SrcName, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] SrcName             The path of a source file
 *  @param [in] DstName             The path of a destination file
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_Move_ParamCheck(const char *SrcName, const char *DstName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (SrcName != NULL) {
            if (AmbaCFS_UtilIsValidPath(SrcName) != 0U) { /* Check file path */
                if (DstName != NULL) {
                    if (AmbaCFS_UtilIsValidPath(DstName) != 0U) { /* Check file path */
                        Ret = CFS_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. DstName: %s", __func__, DstName, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] DstName is NULL", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. SrcName: %s", __func__, SrcName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SrcName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Move a source file to a destination location.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] SrcName             The path of a source file
 *  @param [in] DstName             The path of a destination file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_Move(const char *SrcName, const char *DstName)
{
    UINT32 Ret;
    Ret = AmbaCFS_Move_ParamCheck(SrcName, DstName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_MoveImpl(SrcName, DstName);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of getting file info of a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Name                The file path used to obtain its state in the file system
 *  @param [out] Info               The returned status of the file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_GetFileInfoImpl(const char *Name, AMBA_CFS_FILE_INFO_s *Info)
{
    UINT32 Ret;
    UINT32 FileStatus = AMBA_CFS_STATUS_ERROR;
    Ret = AmbaCFS_FStatusImpl(Name, &FileStatus);
    if (Ret == CFS_OK) {
        if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check if the file is opened */
            if (g_AmbaCFS.CacheEnable != 0U) { /* Get fileName status */
                Ret = AmbaCFS_CacheGetFileInfo(Name, Info);
            } else {
                Ret = CFS_F2C(AmbaFS_GetFileInfo(Name, Info));
                if (Ret == CFS_OK) {
                    /* Do nothing */
                } else if (Ret == CFS_ERR_OBJ_UNAVAILABLE) { /* No such file or directory */
                    /* The file does not exist */
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_GetFileInfoImpl: [ERROR] AmbaFS_GetFileInfo fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] File is opened", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get file status. %s", __func__, Name, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] Name                The file path used to obtain its state in the file system
 *  @param [in] Info                The returned status of the file
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetFileInfo_ParamCheck(const char *Name, const AMBA_CFS_FILE_INFO_s *Info)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (Name != NULL) {
            if (Info != NULL) {
                if (AmbaCFS_UtilIsValidPath(Name) != 0U) { /* Check file path */
                    Ret = CFS_OK;
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. %s", __func__, Name, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Info is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Name is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get the status of a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Name                The file path used to obtain its state in the file system
 *  @param [out] Info               The returned status of the file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_GetFileInfo(const char *Name, AMBA_CFS_FILE_INFO_s *Info)
{
    UINT32 Ret;
    Ret = AmbaCFS_GetFileInfo_ParamCheck(Name, Info);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_GetFileInfoImpl(Name, Info);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of changing the attribute of a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            File path
 *  @param [in] Attr                File attributes
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_ChangeFileModeImpl(const char *FileName, UINT32 Attr)
{
    UINT32 Ret;
    UINT32 FileStatus = AMBA_CFS_STATUS_ERROR;
    Ret = AmbaCFS_FStatusImpl(FileName, &FileStatus);
    if (Ret == CFS_OK) {
        if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check file status */
            Ret = CFS_F2C(AmbaFS_ChangeFileMode(FileName, Attr)); /* Change file attribute */
            if (Ret == CFS_OK) {
                /* Update Cache Status */
                if (g_AmbaCFS.CacheEnable != 0U) {
                    Ret = AmbaCFS_CacheRemove(FileName);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                /* Trigger UPDATE event */
                if (Ret == CFS_OK) {
                    AMBA_CFS_EVENT_INFO_s Param;
                    Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        Param.Event = AMBA_CFS_EVENT_UPDATE;
                        AmbaCFS_UtilCopyFileName(Param.Name, FileName);
                        Ret = AmbaCFS_AddEvent(&Param);
                        if (Ret != CFS_OK) {
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "AmbaCFS_ChangeFileModeImpl: [ERROR] AmbaFS_ChangeFileMode Path:%s", FileName, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_ChangeFileModeImpl: [ERROR] AmbaFS_ChangeFileMode fail. Attr:%u. ErrNum: %p", Attr, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] File is opened", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get file status. %s", __func__, FileName, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] FileName            File path
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_ChangeFileMode_ParamCheck(const char *FileName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (FileName != NULL) {
            if (AmbaCFS_UtilIsValidPath(FileName) != 0U) { /* Check file path */
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. %s", __func__, FileName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Change the attributes of a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            File path
 *  @param [in] Attr                File attributes
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ChangeFileMode(const char *FileName, UINT32 Attr)
{
    UINT32 Ret;
    Ret = AmbaCFS_ChangeFileMode_ParamCheck(FileName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_ChangeFileModeImpl(FileName, Attr);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementaiton of gettin the first directory entry of a directory.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]  Name               The directory path being searched
 *  @param [in]  Attr               Directory attributes
 *  @param [out] FileSearch         Address of a directory entry record
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileSearchFirstImpl(const char *Name, UINT8 Attr, AMBA_CFS_DTA_s *FileSearch)
{
    UINT32 Ret = CFS_OK;
    AMBA_CFS_ACTIVE_DTA_s *ActiveDta;

    /* Allocate ActiveDta */
    ActiveDta = AmbaCFS_ActiveDtaAlloc(&g_AmbaCFS.ActiveDtaPool, FileSearch);
    if (ActiveDta == NULL) {
        Ret = CFS_ERR_API;
    } else {
        /* Name should be checked already */
        if (g_AmbaCFS.CacheEnable != 0U) {
            Ret = AmbaCFS_CacheFirstDirEnt(Name, Attr, ActiveDta);
        } else {
            Ret = AmbaCFS_SearchFileFirst(Name, Attr, &ActiveDta->FsSearch);
            if (Ret == CFS_OK) {
                Ret = AmbaCFS_SearchCopyDtaToSearch(FileSearch, &ActiveDta->FsSearch);
            }
        }

        /* When NG, release ActiveDta */
        if (Ret != CFS_OK) {
            if (AmbaCFS_ActiveDtaRelease(&g_AmbaCFS.ActiveDtaPool, ActiveDta) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_ActiveDtaRelease fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in]  Name               The directory path being searched
 *  @param [in]  FileSearch         The returned directory entry
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileSearchFirst_ParamCheck(const char *Name, const AMBA_CFS_DTA_s *FileSearch)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (Name != NULL) {
            if (FileSearch != NULL) {
                if (AmbaCFS_UtilIsValidRegExp(Name) != 0U) { /* Check file path */
                    Ret = CFS_OK;
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidRegExp fail. %s", __func__, Name, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileSearch is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Name is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Search files and return the first matched result.
 *  Once AmbaCFS_FileSearchFirst() is called, CFS will allocate a search cache entry and will not release it until the serach is finalized, i.e., AmbaCFS_FileSearchNext() returns CFS_ERR_0003.
 *  For a directory, there can be only one active search at one time, i.e., the previous one would be discarded.
 *  The number of search cache entries is limited, so the number of unfinalized search operation is also limited.
 *  Therefore, for each AmbaCFS_FileSearchFirst(), it is suggested to immediately call AmbaCFS_FileSearchNext() to finalize the current search ASAP.
 *  For example, to recursively search all the subfolders, please apply breadth-first search rather than depth-first search.
 *  @param [in]  Name               The directory path being searched
 *  @param [in]  Attr               Directory attributes
 *  @param [out] FileSearch         The returned directory entry
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileSearchFirst(const char *Name, UINT8 Attr, AMBA_CFS_DTA_s *FileSearch)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileSearchFirst_ParamCheck(Name, FileSearch);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_FileSearchFirstImpl(Name, Attr, FileSearch);
            if (AmbaCFS_Unlock() != CFS_OK) {
                Ret = CFS_ERR_FATAL;
            }
        }
    }
    return Ret;
}

/**
 *  The implemenation of getting the next directory entry.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [out] FileSearch         Address of a directory entry record
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileSearchNextImpl(AMBA_CFS_DTA_s *FileSearch)
{
    UINT32 Ret = CFS_OK;
    AMBA_CFS_ACTIVE_DTA_s *ActiveDta;

    /* FileSearch should be found in ActiveDta */
    ActiveDta = AmbaCFS_ActiveDtaGetFromDta(&g_AmbaCFS.ActiveDtaPool, FileSearch);
    if (ActiveDta == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Search is not found", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_ARG;
    } else {
        if (g_AmbaCFS.CacheEnable != 0U) {
            Ret = AmbaCFS_CacheNextDirEnt(ActiveDta);
        } else {
            Ret = AmbaCFS_SearchFileNext(&ActiveDta->FsSearch);
            if (Ret == CFS_OK) {
                Ret = AmbaCFS_SearchCopyDtaToSearch(FileSearch, &ActiveDta->FsSearch);
            }
        }
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] FileSearch          The returned directory entry
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileSearchNext_ParamCheck(const AMBA_CFS_DTA_s *FileSearch)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (FileSearch != NULL) {
            if (FileSearch->SearchInfo != NULL) {
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SearchInfo is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileSearch is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Search files and return the next matched result.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [out] FileSearch         The returned directory entry
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileSearchNext(AMBA_CFS_DTA_s *FileSearch)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileSearchNext_ParamCheck(FileSearch);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_FileSearchNextImpl(FileSearch);
            if (AmbaCFS_Unlock() != CFS_OK) {
                Ret = CFS_ERR_FATAL;
            }
        }
    }
    return Ret;
}

/**
 *  The implemenation of finishing the file search.
 *  @param [in] FileSearch          Address of a directory entry record
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileSearchFinishImpl(AMBA_CFS_DTA_s *FileSearch)
{
    UINT32 Ret = CFS_OK;
    AMBA_CFS_ACTIVE_DTA_s *ActiveDta;

    /* FileSearch should be found in ActiveDta */
    ActiveDta = AmbaCFS_ActiveDtaGetFromDta(&g_AmbaCFS.ActiveDtaPool, FileSearch);
    if (ActiveDta == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Search is not found", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_ARG;
    } else {
        if (g_AmbaCFS.CacheEnable != 0U) {
            Ret = AmbaCFS_CacheFinishDirEnt(ActiveDta);
        } else {
            Ret = AmbaCFS_SearchFileFinish(&ActiveDta->FsSearch);
        }

        /* Release ActiveDta */
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_ActiveDtaRelease(&g_AmbaCFS.ActiveDtaPool, ActiveDta);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_ActiveDtaRelease fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }
    AmbaMisra_TouchUnused(FileSearch);

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] FileSearch          The returned directory entry
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileSearchFinish_ParamCheck(const AMBA_CFS_DTA_s *FileSearch)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (FileSearch != NULL) {
            if (FileSearch->SearchInfo != NULL) {
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SearchInfo is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileSearch is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Finish the file search.
 *  Suggest CFS to release the resource for search
 *  @param [in] FileSearch          Address of a directory entry record
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileSearchFinish(AMBA_CFS_DTA_s *FileSearch)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileSearchFinish_ParamCheck(FileSearch);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_FileSearchFinishImpl(FileSearch);
            if (AmbaCFS_Unlock() != CFS_OK) {
                Ret = CFS_ERR_FATAL;
            }
        }
    }
    return Ret;
}

/**
 *  The implementation of combining two files.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] HeadFileName        The path of the base file
 *  @param [in] TailFileName        The path of the add file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileCombineImpl(const char *HeadFileName, const char *TailFileName)
{
    UINT32 Ret;
    UINT32 FileStatus = AMBA_CFS_STATUS_ERROR;
    Ret = AmbaCFS_FStatusImpl(HeadFileName, &FileStatus);
    if (Ret == CFS_OK) {
        if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check source file */
            Ret = AmbaCFS_FStatusImpl(TailFileName, &FileStatus);
            if (Ret == CFS_OK) {
                if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check destination file */
                    Ret = CFS_F2C(AmbaFS_FileCombine(HeadFileName, TailFileName)); /* Combine files */
                    if (Ret == CFS_OK) {
                        /* Update Cache Status */
                        if (g_AmbaCFS.CacheEnable != 0U) {
                            Ret = AmbaCFS_CacheRemove(HeadFileName);
                            if (Ret == CFS_OK) {
                                Ret = AmbaCFS_CacheRemove(TailFileName);
                                if (Ret != CFS_OK) {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail. TailFileName %s", __func__, TailFileName, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail. HeadFileName %s", __func__, HeadFileName, NULL, NULL, NULL);
                            }
                        }
                        /* Trigger UPDATE and REMOVE event */
                        if (Ret == CFS_OK) {
                            AMBA_CFS_EVENT_INFO_s Param;
                            Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
                            if (Ret == CFS_OK) {
                                Param.Event = AMBA_CFS_EVENT_UPDATE;
                                AmbaCFS_UtilCopyFileName(Param.Name, HeadFileName);
                                Ret = AmbaCFS_AddEvent(&Param);
                                if (Ret == CFS_OK) {
                                    Param.Event = AMBA_CFS_EVENT_REMOVE;
                                    AmbaCFS_UtilCopyFileName(Param.Name, TailFileName);
                                    Ret = AmbaCFS_AddEvent(&Param);
                                    if (Ret != CFS_OK) {
                                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent AMBA_CFS_EVENT_REMOVE fail", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent AMBA_CFS_EVENT_UPDATE fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "AmbaCFS_FileCombineImpl: [ERROR] AmbaFS_FileCombine HeadFN:%s TailFN:%s", HeadFileName, TailFileName, NULL, NULL, NULL);
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileCombineImpl: [ERROR] AmbaFS_FileCombine fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Tail file is opened", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_API;
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get Tail file status. %s", __func__, TailFileName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Head file is opened", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get Head file status. %s", __func__, HeadFileName, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] HeadFileName        The path of the base file
 *  @param [in] TailFileName        The path of the add file
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileCombine_ParamCheck(const char *HeadFileName, const char *TailFileName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (HeadFileName != NULL) {
            if (TailFileName != NULL) {
                if (AmbaCFS_UtilIsValidPath(HeadFileName) != 0U) { /* Check file path */
                    if (AmbaCFS_UtilIsValidPath(TailFileName) != 0U) { /* Check file path */
                        Ret = CFS_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. TailFileName: %s", __func__, TailFileName, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. HeadFileName: %s", __func__, HeadFileName, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] TailFileName is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] HeadFileName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Combine two files into one file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] HeadFileName        The path of the base file
 *  @param [in] TailFileName        The path of the add file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileCombine(const char *HeadFileName, const char *TailFileName)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileCombine_ParamCheck(HeadFileName, TailFileName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_FileCombineImpl(HeadFileName, TailFileName);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of dividing a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] SrcFileName         The path of a file being divided
 *  @param [in] NewFileName         The path of a file being created after division
 *  @param [in] Offset              Byte offset from the beginning of the original file to a division location
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileDivideImpl(const char *SrcFileName, const char *NewFileName, UINT64 Offset)
{
    UINT32 Ret;
    UINT32 FileStatus = AMBA_CFS_STATUS_ERROR;
    Ret = AmbaCFS_FStatusImpl(SrcFileName, &FileStatus);
    if (Ret == CFS_OK) {
        if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check source file */
            Ret = AmbaCFS_FStatusImpl(NewFileName, &FileStatus);
            if (Ret == CFS_OK) {
                if (FileStatus == AMBA_CFS_STATUS_UNUSED)  /* Check destination file */{
                    Ret = CFS_F2C(AmbaFS_FileDivide(SrcFileName, NewFileName, Offset)); /* Divide the file */
                    if (Ret == CFS_OK) {
                        /* Update Cache Status */
                        if (g_AmbaCFS.CacheEnable != 0U) {
                            Ret = AmbaCFS_CacheRemove(SrcFileName);
                            if (Ret == CFS_OK) {
                                Ret = AmbaCFS_CacheRemove(NewFileName);
                                if (Ret != CFS_OK) {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail. NewFileName %s", __func__, NewFileName, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail. SrcFileName %s", __func__, SrcFileName, NULL, NULL, NULL);
                            }
                        }
                        /* Trigger UPDATE and CREATE event */
                        if (Ret == CFS_OK) {
                            AMBA_CFS_EVENT_INFO_s Param;
                            Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
                            if (Ret == CFS_OK) {
                                Param.Event = AMBA_CFS_EVENT_UPDATE;
                                AmbaCFS_UtilCopyFileName(Param.Name, SrcFileName);
                                Ret = AmbaCFS_AddEvent(&Param);
                                if (Ret == CFS_OK) {
                                    Param.Event = AMBA_CFS_EVENT_CREATE;
                                    AmbaCFS_UtilCopyFileName(Param.Name, NewFileName);
                                    Ret = AmbaCFS_AddEvent(&Param);
                                    if (Ret != CFS_OK) {
                                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent AMBA_CFS_EVENT_CREATE fail", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent AMBA_CFS_EVENT_UPDATE fail", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "AmbaCFS_FileDivideImpl: [ERROR] AmbaFS_FileDivide SrcFN:%s NewFN:%s", SrcFileName, NewFileName, NULL, NULL, NULL);
                        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileDivideImpl: [ERROR] AmbaFS_FileDivide fail. Offset:%u. ErrNum: %p", (UINT32) Offset, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] New file is opened", __func__, NULL, NULL, NULL, NULL);
                    Ret = CFS_ERR_API;
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get New file status. %s", __func__, NewFileName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Source file is opened", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get Source file status. %s", __func__, SrcFileName, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] SrcFileName         The path of a file being divided
 *  @param [in] NewFileName         The path of a file being created after division
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileDivide_ParamCheck(const char *SrcFileName, const char *NewFileName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (SrcFileName != NULL) {
            if (NewFileName != NULL) {
                if (AmbaCFS_UtilIsValidPath(SrcFileName) != 0U) { /* Check file path */
                    if (AmbaCFS_UtilIsValidPath(NewFileName) != 0U) { /* Check file path */
                        Ret = CFS_OK;
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. NewFileName: %s", __func__, NewFileName, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. SrcFileName: %s", __func__, SrcFileName, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] NewFileName is NULL", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SrcFileName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Divide a file into two files.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] SrcFileName         The path of a file being divided
 *  @param [in] NewFileName         The path of a file being created after division
 *  @param [in] Offset              Byte offset from the beginning of the original file to a division location
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileDivide(const char *SrcFileName, const char *NewFileName, UINT64 Offset)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileDivide_ParamCheck(SrcFileName, NewFileName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_FileDivideImpl(SrcFileName, NewFileName, Offset);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of inserting clusters to a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            File path
 *  @param [in] Offset              The offset from the beginning of the file to a specified location to insert clusters
 *  @param [in] Number              The number of clusters being inserted
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_ClusterInsertImpl(const char *FileName, UINT32 Offset, UINT32 Number)
{
    UINT32 Ret;
    UINT32 FileStatus = AMBA_CFS_STATUS_ERROR;
    Ret = AmbaCFS_FStatusImpl(FileName, &FileStatus);
    if (Ret == CFS_OK) {
        if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check file status */
            Ret = CFS_F2C(AmbaFS_ClusterInsert(FileName, Offset, Number)); /* Insert clusters to the file */
            if (Ret == CFS_OK) {
                /* Update Cache Status */
                if (g_AmbaCFS.CacheEnable != 0U) {
                    Ret = AmbaCFS_CacheRemove(FileName);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                /* Trigger UPDATE event */
                if (Ret == CFS_OK) {
                    AMBA_CFS_EVENT_INFO_s Param;
                    Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
                    if (Ret == CFS_OK) {
                        Param.Event = AMBA_CFS_EVENT_UPDATE;
                        AmbaCFS_UtilCopyFileName(Param.Name, FileName);
                        Ret = AmbaCFS_AddEvent(&Param);
                        if (Ret != CFS_OK) {
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_ClusterInsertImpl: [ERROR] AmbaFS_ClusterInsert fail. ErrNum: %p Insert: %u", AmbaCFS_UtilGetAmbaFSError(), Number, 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] File is opened", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get file status. %s", __func__, FileName, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] FileName            File path
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_ClusterInsert_ParamCheck(const char *FileName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (FileName != NULL) {
            if (AmbaCFS_UtilIsValidPath(FileName) != 0U) { /* Check file path */
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. %s", __func__, FileName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Allocate and insert clusters to a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            File path
 *  @param [in] Offset              The offset from the beginning of the file to a specified location to insert clusters
 *  @param [in] Number              The number of clusters being inserted
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ClusterInsert(const char *FileName, UINT32 Offset, UINT32 Number)
{
    UINT32 Ret;
    Ret = AmbaCFS_ClusterInsert_ParamCheck(FileName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_ClusterInsertImpl(FileName, Offset, Number);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of deleting clusters from a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            File path
 *  @param [in] Offset              The offset from the beginning of the file to a specified location to delete clusters
 *  @param [in] Number              The number of clusters being deleted
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_ClusterDeleteImpl(const char *FileName, UINT32 Offset, UINT32 Number)
{
    UINT32 Ret;
    UINT32 FileStatus = AMBA_CFS_STATUS_ERROR;
    Ret = AmbaCFS_FStatusImpl(FileName, &FileStatus);
    if (Ret == CFS_OK) {
        if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check file status */
            Ret = CFS_F2C(AmbaFS_ClusterDelete(FileName, Offset, Number)); /* Delete clusters from the file */
            if (Ret == CFS_OK) {
                /* Update Cache Status */
                if (g_AmbaCFS.CacheEnable != 0U) {
                    Ret = AmbaCFS_CacheRemove(FileName);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                /* Trigger UPDATE event */
                if (Ret == CFS_OK) {
                    AMBA_CFS_EVENT_INFO_s Param;
                    Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
                    if (Ret == CFS_OK) {
                        Param.Event = AMBA_CFS_EVENT_UPDATE;
                        AmbaCFS_UtilCopyFileName(Param.Name, FileName);
                        Ret = AmbaCFS_AddEvent(&Param);
                        if (Ret != CFS_OK) {
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_ClusterInsertImpl: [ERROR] AmbaFS_ClusterDelete fail. ErrNum: %p Insert: %u", AmbaCFS_UtilGetAmbaFSError(), Number, 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] File is opened", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get file status. %s", __func__, FileName, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] FileName            File path
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_ClusterDelete_ParamCheck(const char *FileName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (FileName != NULL) {
            if (AmbaCFS_UtilIsValidPath(FileName) != 0U) { /* Check file path */
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. %s", __func__, FileName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Delete clusters from a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            File path
 *  @param [in] Offset              The offset from the beginning of the file to a specified location to delete clusters
 *  @param [in] Number              The number of clusters being deleted
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ClusterDelete(const char *FileName, UINT32 Offset, UINT32 Number)
{
    UINT32 Ret;
    Ret = AmbaCFS_ClusterDelete_ParamCheck(FileName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_ClusterDeleteImpl(FileName, Offset, Number);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Truncate a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            File path
 *  @param [in] Offset              The offset to set the new end of file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FileTruncateImpl(const char *FileName, UINT64 Offset)
{
    UINT32 Ret;
    UINT32 FileStatus = AMBA_CFS_STATUS_ERROR;
    Ret = AmbaCFS_FStatusImpl(FileName, &FileStatus);
    if (Ret == CFS_OK) {
        if (FileStatus == AMBA_CFS_STATUS_UNUSED) { /* Check file status */
            Ret = CFS_F2C(AmbaFS_FileTruncate(FileName, Offset));
            if (Ret == CFS_OK) {
                /* Update Cache Status */
                if (g_AmbaCFS.CacheEnable != 0U) {
                    Ret = AmbaCFS_CacheRemove(FileName);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                /* Trigger UPDATE event */
                if (Ret == CFS_OK) {
                    AMBA_CFS_EVENT_INFO_s Param;
                    Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
                    if (Ret == CFS_OK) {
                        Param.Event = AMBA_CFS_EVENT_UPDATE;
                        AmbaCFS_UtilCopyFileName(Param.Name, FileName);
                        Ret = AmbaCFS_AddEvent(&Param);
                        if (Ret != CFS_OK) {
                            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "AmbaCFS_FileTruncateImpl: [ERROR] AmbaFS_FileTruncate FileName:%s", FileName, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FileTruncateImpl: [ERROR] AmbaFS_FileTruncate fail. Offset:%u. ErrNum: %p", (UINT32) Offset, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "%s: [ERROR] File %s is opened", __func__, FileName, NULL, NULL, NULL);
            Ret = CFS_ERR_API;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Failed to get file status. %s", __func__, FileName, NULL, NULL, NULL);
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] FileName            File path
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_FileTruncate_ParamCheck(const char *FileName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (FileName != NULL) {
            if (AmbaCFS_UtilIsValidPath(FileName) != 0U) { /* Check file path */
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. %s", __func__, FileName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Truncate a file.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] FileName            File path
 *  @param [in] Offset              The offset to set the new end of file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_FileTruncate(const char *FileName, UINT64 Offset)
{
    UINT32 Ret;
    Ret = AmbaCFS_FileTruncate_ParamCheck(FileName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_FileTruncateImpl(FileName, Offset);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of making a new directory.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] DirName             The path of a directory being created
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_MakeDirImpl(const char *DirName)
{
    UINT32 Ret;
    /* DirName is checked already */
    Ret = CFS_F2C(AmbaFS_MakeDir(DirName)); /* Make directory */
    if (Ret == CFS_OK) {
        /* Update Cache Status */
        if (g_AmbaCFS.CacheEnable != 0U) {
            Ret = AmbaCFS_CacheRemove(DirName);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
        /* Trigger MKDIR event */
        if (Ret == CFS_OK) {
            AMBA_CFS_EVENT_INFO_s Param;
            Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
            if (Ret == CFS_OK) {
                Param.Event = AMBA_CFS_EVENT_MKDIR;
                AmbaCFS_UtilCopyFileName(Param.Name, DirName);
                Ret = AmbaCFS_AddEvent(&Param);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "AmbaCFS_MakeDirImpl: [ERROR] AmbaFS_MakeDir Path:%s", DirName, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_MakeDirImpl: [ERROR] AmbaFS_MakeDir fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] DirName             The path of a directory being created
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_MakeDir_ParamCheck(const char *DirName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (DirName != NULL) {
            if (AmbaCFS_UtilIsValidPath(DirName) != 0U) { /* Check file path */
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. %s", __func__, DirName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] DirName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Create a new directory.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] DirName             The path of a directory being created
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_MakeDir(const char *DirName)
{
    UINT32 Ret;
    Ret = AmbaCFS_MakeDir_ParamCheck(DirName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_MakeDirImpl(DirName);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of removing a directory.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] DirName             The path of a directory being deleted
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_RemoveDirImpl(const char *DirName)
{
    UINT32 Ret;
    /* DirName is checked already */
    Ret = CFS_F2C(AmbaFS_RemoveDir(DirName)); /* Remove directory */
    if (Ret == CFS_OK) {
        /* Update Cache Status */
        if (g_AmbaCFS.CacheEnable != 0U) {
            Ret = AmbaCFS_CacheRemove(DirName);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_CacheRemove fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
        /* Trigger RMDIR event */
        if (Ret == CFS_OK) {
            AMBA_CFS_EVENT_INFO_s Param;
            Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
            if (Ret == CFS_OK) {
                Param.Event = AMBA_CFS_EVENT_RMDIR;
                AmbaCFS_UtilCopyFileName(Param.Name, DirName);
                Ret = AmbaCFS_AddEvent(&Param);
                if (Ret != CFS_OK) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "AmbaCFS_RemoveDirImpl: [ERROR] AmbaFS_RemoveDir Path:%s", DirName, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_RemoveDirImpl: [ERROR] AmbaFS_RemoveDir fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] DirName             The path of a directory being deleted
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_RemoveDir_ParamCheck(const char *DirName)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (DirName != NULL) {
            if (AmbaCFS_UtilIsValidPath(DirName) != 0U) { /* Check file path */
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UtilIsValidPath fail. %s", __func__, DirName, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] DirName is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Delete a directory.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] DirName             The path of a directory being deleted
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_RemoveDir(const char *DirName)
{
    UINT32 Ret;
    Ret = AmbaCFS_RemoveDir_ParamCheck(DirName);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            Ret = AmbaCFS_RemoveDirImpl(DirName);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] ErrCode             The last FS error code
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetError_ParamCheck(const UINT32 *ErrCode)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (ErrCode != NULL) {
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ErrCode is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get the last FS error code
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [out] ErrCode            The last FS error code
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_GetError(UINT32 *ErrCode)
{
    UINT32 Ret;
    Ret = AmbaCFS_GetError_ParamCheck(ErrCode);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            *ErrCode = AmbaCFS_UtilGetAmbaFSError();
            Ret = CFS_OK;
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] File                The file descriptor
 *  @param [in] ErrCode             The FS error code of a file
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetFileError_ParamCheck(const AMBA_CFS_FILE_s *File, const UINT32 *ErrCode)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (ErrCode != NULL) {
            if (AmbaCFS_IsValidCfsStream(File) != 0U) {
                Ret = CFS_OK;
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_IsValidCfsStream fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ErrCode is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get the FS error code of a file
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] File                The file descriptor
 *  @param [out] ErrCode            The FS error code of a file
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_GetFileError(AMBA_CFS_FILE_s *File, UINT32 *ErrCode)
{
    UINT32 Ret;
    Ret = AmbaCFS_GetFileError_ParamCheck(File, ErrCode);
    if (Ret == CFS_OK) {
        AMBA_CFS_STREAM_s *Stream;
        /* Convert file handler to internal stream handler */
        AmbaMisra_TypeCast(&Stream, &File);
        Ret = AmbaCFS_LockStream(Stream);
        if (Ret == CFS_OK) {
            *ErrCode = AmbaFS_GetFileError(Stream->File);
            Ret = CFS_OK;
            if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(File);
    return Ret;
}

/*
 * Drive level
 */

/**
 *  The implementation of getting device information
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] DriveInfo          The returned information of device capacity
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_GetDriveInfoImpl(char Drive, AMBA_CFS_DRIVE_INFO_s *DriveInfo)
{
    UINT32 Ret;
    if (g_AmbaCFS.CacheEnable != 0U) {
        Ret = AmbaCFS_CacheGetDriveInfo(Drive, DriveInfo);
    } else {
        Ret = CFS_F2C(AmbaFS_GetDriveInfo(Drive, DriveInfo));
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_GetDriveInfoImpl: [ERROR] AmbaFS_GetDriveInfo fail. Drive:%c. ErrNum: %p", (UINT8) Drive, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
        }
    }

    return Ret;
}

/**
 *  Check parameters.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [in]  DriveInfo          The returned information of device capacity
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetDriveInfo_ParamCheck(char Drive, const AMBA_CFS_DRIVE_INFO_s *DriveInfo)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (DriveInfo != NULL) {
            Ret = CheckDriveName(Drive);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckDriveName fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] DriveInfo is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get device capacity (i.e., the information of a drive).
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] DriveInfo          The returned information of device capacity
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_GetDriveInfo(char Drive, AMBA_CFS_DRIVE_INFO_s *DriveInfo)
{
    UINT32 Ret;
    Ret = AmbaCFS_GetDriveInfo_ParamCheck(Drive, DriveInfo);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            char DriveName = AmbaCFS_UtilToUpper(Drive);
            Ret = AmbaCFS_GetDriveInfoImpl(DriveName, DriveInfo);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  This is an internal function
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  Get cluster size of a drive
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [in]  ClusterSize        Cluster size
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_GetDriveClusterSize(char Drive, UINT32 *ClusterSize)
{
    UINT32 Ret;
    Ret = AmbaCFS_Lock();
    if (Ret == CFS_OK) {
        AMBA_CFS_DRIVE_INFO_s Info = {0};
        Ret = AmbaCFS_GetDriveInfoImpl(Drive, &Info);
        if (Ret == CFS_OK) {
            /* Set return value */
            *ClusterSize = Info.BytesPerSector * Info.SectorsPerCluster;
        }
        if (AmbaCFS_Unlock() != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_FATAL;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [in]  SizeByte           The returned size of cached data
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_GetCachedDataSize_ParamCheck(char Drive, const UINT64 *SizeByte)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if (SizeByte != NULL) {
            Ret = CheckDriveName(Drive);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckDriveName fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] SizeByte is NULL", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Get the cached data size of a drive.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] SizeByte           The returned size of cached data
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_GetCachedDataSize(char Drive, UINT64 *SizeByte)
{
    UINT32 Ret;
    Ret = AmbaCFS_GetCachedDataSize_ParamCheck(Drive, SizeByte);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            char DriveName = AmbaCFS_UtilToUpper(Drive);
            Ret = AmbaCFS_SchCachedFileDataSize(DriveName, SizeByte);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Check if the drive in the stream is idle
 *  @param [in] Stream              The CFS stream
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CheckStreamDriveIdle(AMBA_CFS_STREAM_s *Stream, char Drive)
{
    UINT32 Ret;
    Ret = AmbaCFS_LockStream(Stream);
    if (Ret == CFS_OK) {
        if (Stream->Status != AMBA_CFS_STATUS_UNUSED) {
            char DriveName;
            Ret = AmbaCFS_UtilGetDrive(Stream->Filename, &DriveName);
            if (Ret == CFS_OK) {
                if (DriveName == Drive) {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] [%s] has not been closed", __func__, Stream->Filename, NULL, NULL, NULL);
                    Ret = CFS_ERR_API;
                }
            }
        }
        if (AmbaCFS_UnlockStream(Stream) != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_UnlockStream fail", __func__, NULL, NULL, NULL, NULL);
            Ret = CFS_ERR_FATAL;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_LockStream fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check if a drive is idle
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CheckDriveIdleImpl(char Drive)
{
    UINT32 Ret = CFS_OK;
    for (UINT32 i = 0U; i < AMBA_CFS_MAX_STREAM; i++) {
        AMBA_CFS_STREAM_s *Stream = &g_AmbaCFS.StreamTbl[i];
        Ret = CheckStreamDriveIdle(Stream, Drive);
        if (Ret != CFS_OK) {
            break;
        }
    }
    return Ret;
}

/**
 *  Check if a drive is idle
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 CheckDriveIdle(char Drive)
{
    UINT32 Ret;
    Ret = AmbaCFS_SchLock();
    if (Ret == CFS_OK) {
        Ret = CheckDriveIdleImpl(Drive);
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
 *  The implementation of formating a drive.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_FormatImpl(char Drive)
{
    UINT32 Ret = CheckDriveIdle(Drive);    /* drive must be idle */
    if (Ret == CFS_OK) {
        Ret = CFS_F2C(AmbaFS_Format(Drive)); /* Format driveName */
        if (Ret == CFS_OK) {
            /* Update Cache Status */
            if (g_AmbaCFS.CacheEnable != 0U) {
                Ret = AmbaCFS_CacheClearDrive(Drive);
            }
            /* Trigger FORMAT event */
            if (Ret == CFS_OK) {
                AMBA_CFS_EVENT_INFO_s Param;
                Ret = CFS_WRAP2C(AmbaWrap_memset(&Param, 0, sizeof(Param)));
                if (Ret == CFS_OK) {
                    Param.Event = AMBA_CFS_EVENT_FORMAT;
                    Param.Name[0] = Drive;
                    Param.Name[1] = '\0';
                    Ret = AmbaCFS_AddEvent(&Param);
                    if (Ret != CFS_OK) {
                        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_AddEvent fail", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaWrap_memset fail", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_FormatImpl: [ERROR] AmbaFS_Format fail. Drive:%c. ErrNum:%p", (UINT8) Drive, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckDriveIdle fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_Format_ParamCheck(char Drive)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        Ret = CheckDriveName(Drive);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckDriveName fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Format a drive.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_Format(char Drive)
{
    UINT32 Ret;
    /* After format, the drive will be unmounted, so need to mount again. */
    Ret = AmbaCFS_Format_ParamCheck(Drive);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            char DriveName = AmbaCFS_UtilToUpper(Drive);
            Ret = AmbaCFS_FormatImpl(DriveName);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of synchronizing a drive.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @param [in] Mode                Invalidation mode (In AMBA_CFS_SYNC_NINVALIDATE mode, caches will not be invalidated; however, in AMBA_CFS_SYNC_INVALIDATE mode, caches will be invalidated.)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SyncImpl(char Drive, INT32 Mode)
{
    UINT32 Ret;
    /* TODO: also sync all opened streams */
    Ret = CFS_F2C(AmbaFS_Sync(Drive, Mode));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SyncImpl: [ERROR] AmbaFS_Sync fail. Drive:%c. ErrNum:%p", (UINT8) Drive, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @param [in] Mode                Invalidation mode (In AMBA_CFS_SYNC_NINVALIDATE mode, caches will not be invalidated; however, in AMBA_CFS_SYNC_INVALIDATE mode, caches will be invalidated.)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_Sync_ParamCheck(char Drive, INT32 Mode)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        if ((Mode == (INT32)AMBA_CFS_SYNC_NINVALIDATE) || (Mode == (INT32)AMBA_CFS_SYNC_INVALIDATE)) {
            Ret = CheckDriveName(Drive);
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckDriveName fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_Sync_ParamCheck: [ERROR] Mode %d is not supported", Mode, 0, 0, 0, 0);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Write all cached data for a specified drive back to the underlying device.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @param [in] Mode                Invalidation mode (In AMBA_CFS_SYNC_NINVALIDATE mode, caches will not be invalidated; however, in AMBA_CFS_SYNC_INVALIDATE mode, caches will be invalidated.)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_Sync(char Drive, INT32 Mode)
{
    UINT32 Ret;
    Ret = AmbaCFS_Sync_ParamCheck(Drive, Mode);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            char DriveName = AmbaCFS_UtilToUpper(Drive);
            Ret = AmbaCFS_SyncImpl(DriveName, Mode);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_Mount_ParamCheck(char Drive)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        Ret = CheckDriveName(Drive);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckDriveName fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Mount a drive
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_Mount(char Drive)
{
    UINT32 Ret;
    Ret = AmbaCFS_Mount_ParamCheck(Drive);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            char DriveName = AmbaCFS_UtilToUpper(Drive);
            Ret = CFS_F2C(AmbaFS_Mount(DriveName));
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_Mount: [ERROR] AmbaFS_Mount fail. Drive:%c. ErrNum:%p", (UINT8) Drive, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
            }
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/**
 *  The implementation of unmounting a drive
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_UnMountImpl(char Drive)
{
    UINT32 Ret = CheckDriveIdle(Drive);    /* Drive must be idle */
    if (Ret == CFS_OK) {
        /* Update Cache Status */
        if (g_AmbaCFS.CacheEnable != 0U) {
            Ret = AmbaCFS_CacheClearDrive(Drive);
        }
        /* Unmount the drive */
        if (Ret == CFS_OK) {
            Ret = CFS_F2C(AmbaFS_UnMount(Drive));
            if (Ret != CFS_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_UnMountImpl: [ERROR] AmbaFS_UnMount fail. Drive:%c. ErrNum:%p", (UINT8) Drive, AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckDriveIdle fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Check parameters.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_UnMount_ParamCheck(char Drive)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        Ret = CheckDriveName(Drive);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckDriveName fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Unmount a drive
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_UnMount(char Drive)
{
    UINT32 Ret;
    Ret = AmbaCFS_UnMount_ParamCheck(Drive);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            char DriveName = AmbaCFS_UtilToUpper(Drive);
            Ret = AmbaCFS_UnMountImpl(DriveName);
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

/*
 * Cache Operations
 */

/**
 *  Check parameters.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_ClearCache_ParamCheck(char Drive)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (g_AmbaCFS_Init != 0U) {
        Ret = CheckDriveName(Drive);
        if (Ret != CFS_OK) {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] CheckDriveName fail", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Not initialized", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Clear drive caches which include file states, directory entries, and drive information.
 *  @param [in] Drive               Drive name (from A to Z)
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ClearCache(char Drive)
{
    UINT32 Ret;
    Ret = AmbaCFS_ClearCache_ParamCheck(Drive);
    if (Ret == CFS_OK) {
        Ret = AmbaCFS_Lock();
        if (Ret == CFS_OK) {
            if (g_AmbaCFS.CacheEnable != 0U) {
                char DriveName = AmbaCFS_UtilToUpper(Drive);
                Ret = AmbaCFS_CacheClearDrive(DriveName);
            }
            if (AmbaCFS_Unlock() != CFS_OK) {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Unlock fail", __func__, NULL, NULL, NULL, NULL);
                Ret = CFS_ERR_FATAL;
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] AmbaCFS_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

