/**
 * @file AmbaCFS.h
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
#ifndef AMBACFS_H
#define AMBACFS_H

#include <AmbaErrorCode.h>
#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaFS.h>

/**
 * CFS: Cached file system
 *
 * The CFS module includes the following functions:
 * 1. CFS read/write function
 * 2. CFS seek function
 * 3. CFS sync function
 * 4. Other file system related function
 */

/*
 * Error code
 */
#define CFS_ERR_0000                    (CFS_ERR_BASE       )   /**< Invalid argument */
#define CFS_ERR_0001                    (CFS_ERR_BASE | 0x1U)   /**< Fatal error */
#define CFS_ERR_0002                    (CFS_ERR_BASE | 0x2U)   /**< I/O error */
#define CFS_ERR_0003                    (CFS_ERR_BASE | 0x3U)   /**< A request object is unavailable */
#define CFS_ERR_0004                    (CFS_ERR_BASE | 0x4U)   /**< The search operation has been marked as invalid due to internal resource management. Need to re-start search. */


#define AMBA_CFS_MAX_FILENAME_LENGTH    (64U)               /**< The maximum length of file names */

/*
 * Asynchronous mode parameters
 */
#define AMBA_CFS_MAX_DRIVE_AMOUNT       (26U)               /**< The maximum number of drives */

/*
 * Format type of drives
 */
#define AMBA_CFS_FMT_TYPE_FAT12         (0U)                /**< FAT12 format */
#define AMBA_CFS_FMT_TYPE_FAT16         (1U)                /**< FAT16 format */
#define AMBA_CFS_FMT_TYPE_FAT32         (2U)                /**< FAT32 format */
#define AMBA_CFS_FMT_TYPE_EXFAT         (3U)                /**< EXFAT format */
#define AMBA_CFS_FMT_TYPE_ERR           (0xFFFFFFFFU)       /**< Format error */

#define AMBA_CFS_SUNDAY                 (0U)                /**< Sunday */
#define AMBA_CFS_MONDAY                 (1U)                /**< Monday */
#define AMBA_CFS_TUESDAY                (2U)                /**< Tuesday */
#define AMBA_CFS_WEDNESDAY              (3U)                /**< Wednesday */
#define AMBA_CFS_THURSDAY               (4U)                /**< Thursday */
#define AMBA_CFS_FRIDAY                 (5U)                /**< Friday */
#define AMBA_CFS_SATURDAY               (6U)                /**< Saturday */

/*
 * CFS modes
 */
/*
 * Cache invalidation mode for AmbaCFS_Sync()
 */
#define AMBA_CFS_SYNC_NINVALIDATE       (AMBA_FS_NINVALIDATE)   /**< Flush and not invalidate cache */
#define AMBA_CFS_SYNC_INVALIDATE        (AMBA_FS_INVALIDATE)    /**< Flush and invalidate cache */

/*
 * Seek Origin (the position used as reference for the offset)
 */
#define AMBA_CFS_SEEK_START             (AMBA_FS_SEEK_START)    /**< The beginning of a file (seek to the position relative to start position of file) */
#define AMBA_CFS_SEEK_CUR               (AMBA_FS_SEEK_CUR)      /**< The current position of a file (seek to the position relative to current file position) */
#define AMBA_CFS_SEEK_END               (AMBA_FS_SEEK_END)      /**< The end of a file (seek to the position relative to end position of file) */

/*
 * File attributes
 */
#define AMBA_CFS_ATTR_ALL               (AMBA_FS_ATTR_ALL)      /**< All attributes (for fsfirst function) */

/*
 * File Mode
 */
#define AMBA_CFS_FILE_MODE_READ_ONLY    (0U)                /**< "r" */
#define AMBA_CFS_FILE_MODE_READ_WRITE   (1U)                /**< "r+" */
#define AMBA_CFS_FILE_MODE_WRITE_ONLY   (2U)                /**< "w" */
#define AMBA_CFS_FILE_MODE_WRITE_READ   (3U)                /**< "w+" */
#define AMBA_CFS_FILE_MODE_APPEND_ONLY  (4U)                /**< "a" */
#define AMBA_CFS_FILE_MODE_APPEND_READ  (5U)                /**< "a+" */
#define AMBA_CFS_FILE_MODE_MAX          (6U)                /**< Max file mode */

/*
 * Status of file
 */
#define AMBA_CFS_STATUS_UNUSED          (0U)                /**< The status indicating that a file is unused */
#define AMBA_CFS_STATUS_OPENED_READ     (1U)                /**< The status indicating that a file is opened to read */
#define AMBA_CFS_STATUS_OPENED_WRITE    (2U)                /**< The status indicating that a file is opened to write */
#define AMBA_CFS_STATUS_CLOSING_READ    (3U)                /**< The status indicating that a file is closing (async read mode only) */
#define AMBA_CFS_STATUS_CLOSING_WRITE   (4U)                /**< The status indicating that a file is closing (async write mode only) */
#define AMBA_CFS_STATUS_ERROR           (5U)                /**< The status indicating that an error occurs in operating a file (async mode only, to block subsequent read/write) */

/*
 * CFS event
 */
#define AMBA_CFS_EVENT_FOPEN            (1U)                /**< The event raised in opening a file. Info: File name and file open mode; The others are dummy. */
#define AMBA_CFS_EVENT_FCLOSE           (2U)                /**< The event raised in closing a file. Info: File name; The others are dummy. */
#define AMBA_CFS_EVENT_CREATE           (3U)                /**< The event raised in creating a file. Info: File name; The others are dummy. */
#define AMBA_CFS_EVENT_REMOVE           (4U)                /**< The event raised in removing a file. Info: File name; The others are dummy. */
#define AMBA_CFS_EVENT_UPDATE           (5U)                /**< The event raised in updating a file. Info: File name; The others are dummy. */
#define AMBA_CFS_EVENT_RMDIR            (6U)                /**< The event raised in removing a directory. Info: Directory name; The others are dummy. */
#define AMBA_CFS_EVENT_MKDIR            (7U)                /**< The event raised in creating a directory. Info: Directory name; The others are dummy. */
#define AMBA_CFS_EVENT_FORMAT           (8U)                /**< The event raised in formatting a drive. Info: Drive name; The others are dummy. */
#define AMBA_CFS_EVENT_IO_ERROR         (9U)                /**< The event raised when an I/O error occurs. Info: File name; The others are dummy. */
#define AMBA_CFS_EVENT_TOO_FRAGMENTED   (10U)               /**< The event raised when the storage is too fragmented. Info: Drive name; The others are dummy. */
#define AMBA_CFS_EVENT_ASYNC_WRITE      (11U)               /**< The event raised in asynchronous file write. Info: CFS File descriptor; The others are dummy. */
#define AMBA_CFS_EVENT_ASYNC_READ       (12U)               /**< The event raised in asynchronous file read. Info: CFS File descriptor; The others are dummy. */
#define AMBA_CFS_EVENT_ASYNC_CLOSE      (13U)               /**< The event raised in asynchronous file close. Info: CFS File descriptor; The others are dummy. */
#define AMBA_CFS_EVENT_MAX              (14U)               /**< Max event */

/*
 * CFS File Alignment Mode
 */
#define AMBA_CFS_ALIGN_MODE_NONE        (0U)                /**< Alignment Mode. No align. The file size is not aligned. */
#define AMBA_CFS_ALIGN_MODE_SIZE        (1U)                /**< Alignment Mode. Size align. The behavior of "file write" is same as "no align", but the file size will be aligned when closing file. */
#define AMBA_CFS_ALIGN_MODE_CONTINUOUS  (2U)                /**< Alignment Mode. Continuous align. Data in storage is continuous, but "file write" might fail when the storage is too fragmented. */
#define AMBA_CFS_ALIGN_MODE_MAX         (3U)                /**< Max Alignment Mode. */

/*
 * CFS DMA threshold
 */
#define AMBA_CFS_DMA_SIZE_THRESHOLD     (8192U)             /**< DMA copy threshold for cached source (use DMA if data size is larger than threshold, otherwise use memcpy) */

/**
 * Data type of drive information
 */
#define AMBA_CFS_DRIVE_INFO_s           AMBA_FS_DRIVE_INFO_t

/**
 * Data type of file state
 */
#define AMBA_CFS_FILE_INFO_s            AMBA_FS_FILE_INFO_s

/**
 * CFS File descriptor
 */
typedef struct {
    char Filename[AMBA_CFS_MAX_FILENAME_LENGTH];            /**< Full path of a file */
} AMBA_CFS_FILE_s;

/**
 *  Parameters for CFS event
 *
 *  File operation:
 *  1.  AMBA_CFS_EVENT_FOPEN: File name and file open mode; The others are dummy.
 *  2.  AMBA_CFS_EVENT_FCLOSE: File name; The others are dummy.
 *  3.  AMBA_CFS_EVENT_CREATE: File name; The others are dummy.
 *  4.  AMBA_CFS_EVENT_REMOVE: File name; The others are dummy.
 *  5.  AMBA_CFS_EVENT_UPDATE: File name; The others are dummy.
 *  6.  AMBA_CFS_EVENT_RMDIR: Directory name; The others are dummy.
 *  7.  AMBA_CFS_EVENT_MKDIR: Directory name; The others are dummy.
 *  8.  AMBA_CFS_EVENT_FORMAT: Drive name; The others are dummy.
 *  9.  AMBA_CFS_EVENT_IO_ERROR: File name; The others are dummy.
 *  10. AMBA_CFS_EVENT_TOO_FRAGMENTED: Drive name; The others are dummy.
 *
 *  Scheduler task:
 *  11. AMBA_CFS_EVENT_ASYNC_WRITE: CFS File descriptor; The others are dummy.
 *  12. AMBA_CFS_EVENT_ASYNC_READ: CFS File descriptor; The others are dummy.
 *  13. AMBA_CFS_EVENT_ASYNC_CLOSE: CFS File descriptor; The others are dummy.
 */
typedef struct {
    UINT32 Event;                                           /**< CFS event. (ex: AMBA_CFS_EVENT_FOPEN.) */

    /* Parameters for file operations */
    char Name[AMBA_CFS_MAX_FILENAME_LENGTH];                /**< File/Directory/Drive name */
    UINT8 Mode;                                             /**< File open mode */

    /* Parameters for scheduler task */
    AMBA_CFS_FILE_s *File;                                  /**< CFS File descriptor */
} AMBA_CFS_EVENT_INFO_s;

/**
 * The function to check whether the buffer is cached or not
 * Return 1 - Cached, 0 - Non-Cached
 */
typedef UINT32 (*AMBA_CFS_CHECK_CACHED_f)(void *StartAddr, UINT32 Size);

/**
 * The function to clean data cache
 * Return 0 - OK, others - Error
 */
typedef UINT32 (*AMBA_CFS_CACHE_CLEAN_f)(void *StartAddr, UINT32 Size);

/**
 * The function to invalidate data cache
 * Return 0 - OK, others - Error
 */
typedef UINT32 (*AMBA_CFS_CACHE_INVALIDATE_f)(void *StartAddr, UINT32 Size);

/**
 * The function to convert virtual memory address to physical address
 * Return 0 - OK, others - Error
 */
typedef UINT32 (*AMBA_CFS_VIRT_TO_PHYS_f)(void *VirtAddr, void **PhysAddr);

/**
 * The configuration of the CFS module
 */
typedef struct {
    UINT8 *Buffer;                                          /**< The work buffer of the CFS module */
    UINT32 BufferSize;                                      /**< The size of the work buffer */
    /* The parameters for CFS scheduler */
    UINT32 BankSize;                                        /**< The size of a bank */
    UINT32 CmdAmount;                                       /**< The number of commands */
    /* The parameters for CFS cache */
    UINT8  CacheEnable;                                     /**< The flag used to enable or disable the cache function of the CFS module (If the value is TRUE, the cache function will be enabled.) */
    UINT32 CacheMaxDirNum;                                  /**< The maximum number of cached directories */
    UINT32 CacheMaxFileNum;                                 /**< The maximum number of cached files */
    /* The parameter for CFS event queue */
    UINT32 EventQueueSize;                                  /**< The size of event queue */
    /* The parameter for CFS active DTA */
    UINT32 MaxSearch;                                       /**< The max unfinalized search. See AmbaCFS_FileSearchFirst() */

    /* Callback function */
    AMBA_CFS_CHECK_CACHED_f CheckCached;                    /**< The callback function to check whether the buffer is cached or not */
    AMBA_CFS_CACHE_CLEAN_f CacheClean;                      /**< The callback function to clean data cache. NULL - Call AmbaCache_DataClean by default. */
    AMBA_CFS_CACHE_INVALIDATE_f CacheInvalidate;            /**< The callback function to invalidate data cache. NULL - Call AmbaCache_DataInvalidate by default. */
    AMBA_CFS_VIRT_TO_PHYS_f VirtToPhys;                     /**< The callback function to convert virtual memory address to physical address. It's used when DMA APIs require physical address. NULL - Call default function. */
} AMBA_CFS_INIT_CFG_s;

/**
 * Parameters for opening a file
 */
typedef struct {
    char Filename[AMBA_CFS_MAX_FILENAME_LENGTH];            /**< File name */
    UINT8 Mode;                                             /**< File open mode (ex: AMBA_CFS_FILE_MODE_READ_ONLY.) */

    /* Parameters for async I/O mode */
    UINT8 AsyncEnable;                                      /**< The flag to enable async mode (Indicate if a file is in asynchronous mode.) */
    UINT8 DmaEnable;                                        /**< How to copy data in async read/write. 0: Memory copy, 1: DMA copy. (When using DMA on async write with cached buffer and the data size >= AMBA_CFS_DMA_SIZE_THRESHOLD, be sure that the data in DRAM is correct.) */
    UINT8 *BankBuffer;                                      /**< The bank buffer */
    UINT32 BankBufferSize;                                  /**< The size of the bank buffer. The bank amount will be (BankBufferSize / BankSize). BankBufferSize should be aligned to BankSize. */

    /* Parameters for writing a file */
    UINT32 AlignMode;                                       /**< File alignment mode. (ex. AMBA_CFS_ALIGN_MODE_NONE) */
    UINT32 AlignSize;                                       /**< File alignment size (The value should be a multiple of a cluster size.). It's valid when AlignMode is AMBA_CFS_ALIGN_MODE_SIZE or AMBA_CFS_ALIGN_MODE_CONTINUOUS. */
    UINT32 BytesToSync;                                     /**< The number of bytes that the CFS would invoke FileSync() one time (A file should be synced after BytesToSync bytes have been written.) */
} AMBA_CFS_FILE_PARAM_s;

/**
 * Data type of directory entry
 */
typedef struct {
    UINT64 FileSize;                                /**< File size in bytes */
    UINT16 Date;                                    /**< Modification date */
    UINT16 Time;                                    /**< Modification time */
    UINT32 Attribute;                               /**< File attributes */
    char FileName[AMBA_CFS_MAX_FILENAME_LENGTH];    /**< Short file name */
    void *SearchInfo;                               /**< Internal search info */
} AMBA_CFS_DTA_s;

/*
 * CFS APIs
 */
UINT32 AmbaCFS_GetInitBufferSize(const AMBA_CFS_INIT_CFG_s *Config, UINT32 *BufferSize);

UINT32 AmbaCFS_GetInitDefaultCfg(AMBA_CFS_INIT_CFG_s *Config);

UINT32 AmbaCFS_Init(const AMBA_CFS_INIT_CFG_s *Config);

UINT32 AmbaCFS_GetEvent(UINT32 Size, UINT32 Timeout, AMBA_CFS_EVENT_INFO_s *EventInfo, UINT32 *EventNum);

UINT32 AmbaCFS_ProcessEvent(AMBA_CFS_FILE_s *File, UINT32 Event);

UINT32 AmbaCFS_DiscardEvent(AMBA_CFS_FILE_s *File, UINT32 Event);

/*
 * File level
 */
UINT32 AmbaCFS_GetFileParam(AMBA_CFS_FILE_PARAM_s *FileParam);

UINT32 AmbaCFS_FileOpen(const AMBA_CFS_FILE_PARAM_s *FileParam, AMBA_CFS_FILE_s **File);

UINT32 AmbaCFS_FileClose(AMBA_CFS_FILE_s *File);

UINT32 AmbaCFS_FileRead(void *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_FILE_s *File, UINT32 *NumSuccess);

UINT32 AmbaCFS_FileWrite(void *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_FILE_s *File, UINT32 *NumSuccess);

UINT32 AmbaCFS_FileSeek(AMBA_CFS_FILE_s *File, INT64 Offset, INT32 Origin);

UINT32 AmbaCFS_FileTell(AMBA_CFS_FILE_s *File, UINT64 *FilePos);

UINT32 AmbaCFS_FileSync(AMBA_CFS_FILE_s *File);

UINT32 AmbaCFS_FileGetLen(AMBA_CFS_FILE_s *File, UINT64 *FileLen);

UINT32 AmbaCFS_ClusterAppend(AMBA_CFS_FILE_s *File, UINT64 Size, UINT64 *NumSuccess);

UINT32 AmbaCFS_FileEof(AMBA_CFS_FILE_s *File);

/*
 * File level
 * Most of the APIs require full paths of directory and file names.
 */
UINT32 AmbaCFS_FileStatus(const char *FileName, UINT32 *Status);

UINT32 AmbaCFS_Remove(const char *FileName);

UINT32 AmbaCFS_Move(const char *SrcName, const char *DstName);

UINT32 AmbaCFS_GetFileInfo(const char *Name, AMBA_CFS_FILE_INFO_s *Info);

UINT32 AmbaCFS_ChangeFileMode(const char *FileName, UINT32 Attr);

UINT32 AmbaCFS_FileSearchFirst(const char *Name, UINT8 Attr, AMBA_CFS_DTA_s *FileSearch);

UINT32 AmbaCFS_FileSearchNext(AMBA_CFS_DTA_s *FileSearch);

UINT32 AmbaCFS_FileSearchFinish(AMBA_CFS_DTA_s *FileSearch);

UINT32 AmbaCFS_FileCombine(const char *HeadFileName, const char *TailFileName);

UINT32 AmbaCFS_FileDivide(const char *SrcFileName, const char *NewFileName, UINT64 Offset);

UINT32 AmbaCFS_ClusterInsert(const char *FileName, UINT32 Offset, UINT32 Number);

UINT32 AmbaCFS_ClusterDelete(const char *FileName, UINT32 Offset, UINT32 Number);

UINT32 AmbaCFS_FileTruncate(const char *FileName, UINT64 Offset);

UINT32 AmbaCFS_MakeDir(const char *DirName);

UINT32 AmbaCFS_RemoveDir(const char *DirName);

UINT32 AmbaCFS_GetError(UINT32 *ErrCode);

UINT32 AmbaCFS_GetFileError(AMBA_CFS_FILE_s *File, UINT32 *ErrCode);

/*
 * Drive level
 */
UINT32 AmbaCFS_GetDriveInfo(char Drive, AMBA_CFS_DRIVE_INFO_s *DriveInfo);

UINT32 AmbaCFS_GetCachedDataSize(char Drive, UINT64 *SizeByte);

UINT32 AmbaCFS_Format(char Drive);

UINT32 AmbaCFS_Sync(char Drive, INT32 Mode);

UINT32 AmbaCFS_Mount(char Drive);

UINT32 AmbaCFS_UnMount(char Drive);

/*
 * Cache Operations
 */
UINT32 AmbaCFS_ClearCache(char Drive);

#endif
