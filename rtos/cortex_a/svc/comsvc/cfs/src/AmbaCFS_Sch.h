/**
 * @file AmbaCFS_Sch.h
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
#ifndef AMBACFS_SCH_H
#define AMBACFS_SCH_H

#include "AmbaCFS_Util.h"
#include "AmbaCFS_Stream.h"

#define AMBA_CFS_SCH_CMD_AMOUNT_DEFAULT      (32U)       /**< Default number of commands */
#define AMBA_CFS_SCH_CMD_AMOUNT_MIN          (2U)        /**< Minimum number of commands */
#define AMBA_CFS_SCH_BANK_SIZE_DEFAULT       (524288U)   /**< Default size of banks (512k) */
#define AMBA_CFS_SCH_BANK_PER_FILE_DEFAULT   (2U)        /**< Default number of banks per file */
#define AMBA_CFS_FIO_ALIGNED_SIZE            (131072U)   /**< The alignment size of FIO (128K bytes) */

/*
 * APIs
 */

/**
 *  Get required buffer size
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] CmdAmount           The number of commands
 *  @return The required buffer size
 */
UINT32 AmbaCFS_SchGetInitBufferSize(UINT32 CmdAmount);

/**
 *  Get default config for scheduling module.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Config              The config record
 */
void AmbaCFS_SchGetInitDefaultCfg(AMBA_CFS_INIT_CFG_s *Config);

/**
 *  Initialize scheduling module.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Buffer              Buffer address
 *  @param [in] BufferSize          The size of the work buffer
 *  @param [in] CmdAmount           The number of commands
 *  @param [in] BankSize            Bank size
 *  @param [in] CacheEnable         FileInfo cache is enabled or not
 *  @param [in] CheckCached         The callback function to check whether the buffer is cached or not
 *  @param [in] CacheClean          The callback function to clean data cache
 *  @param [in] CacheInvalidate     The callback function to invalidate data cache
 *  @param [in] VirtToPhys          The callback function to convert virtual memory address to physical address
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchInit(UINT8 *Buffer, UINT32 BufferSize, UINT32 CmdAmount, UINT32 BankSize, UINT8 CacheEnable,
    AMBA_CFS_CHECK_CACHED_f CheckCached, AMBA_CFS_CACHE_CLEAN_f CacheClean, AMBA_CFS_CACHE_INVALIDATE_f CacheInvalidate, AMBA_CFS_VIRT_TO_PHYS_f VirtToPhys);

/**
 *  Lock scheduler
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchLock(void);

/**
 *  Unlock scheduler
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchUnlock(void);

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
UINT32 AmbaCFS_SchAsyncfread(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess);

/**
 *  Synchronous fread.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Buffer              Buffer address
 *  @param [in] Size                Size of a record
 *  @param [in] Count               Number of records
 *  @param [in] Stream              The CFS stream
 *  @param [out] NumSuccess         The returned number of records read
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchSyncfread(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess);

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
UINT32 AmbaCFS_SchAsyncfwrite(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess);

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
UINT32 AmbaCFS_SchSyncfwrite(UINT8 *Buffer, UINT32 Size, UINT32 Count, AMBA_CFS_STREAM_s *Stream, UINT32 *NumSuccess);

/**
 *  Asynchrounou FileSync.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchAsyncFileSync(AMBA_CFS_STREAM_s *Stream);

/**
 *  Synchrounou FileSync.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchSyncFileSync(AMBA_CFS_STREAM_s *Stream);

/**
 *  Asynchronous fappend
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Size                The size
 *  @param [out] NumSuccess         The returned size (bytes) that is appended
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchAsyncfappend(AMBA_CFS_STREAM_s *Stream, UINT64 Size, UINT64 *NumSuccess);

/**
 *  Synchronous fappend
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @param [in] Size                The size
 *  @param [out] NumSuccess         The returned size (bytes) that is appended
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchSyncfappend(AMBA_CFS_STREAM_s *Stream, UINT64 Size, UINT64 *NumSuccess);

/**
 *  Synchronous fclose.
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchSyncfclose(AMBA_CFS_STREAM_s *Stream);

/**
 *  Asynchronous fclose.
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] Stream              The CFS stream
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchAsyncfclose(AMBA_CFS_STREAM_s *Stream);

/**
 *  Get cached data size on a drive
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in]  Drive              Drive name (from A to Z)
 *  @param [out] CachedSize         Cached data size
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SchCachedFileDataSize(char Drive, UINT64 *CachedSize);

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
UINT32 AmbaCFS_SetAsyncData(UINT8 StreamId, UINT8 Mode, UINT8 *BankBuffer, UINT32 BankBufferSize, UINT32 BankSize);

/**
 *  Get max bank number
 *  I/O Error handling for User: None. Won't return CFS_ERR_FS.
 *  @param [in] StreamId            The stream ID
 *  @return The max bank number
 */
UINT32 AmbaCFS_GetMaxNumBank(UINT8 StreamId);

#endif
