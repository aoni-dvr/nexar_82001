/**
 * @file AmbaCFS_Stream.h
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
#ifndef AMBACFS_STREAM_H
#define AMBACFS_STREAM_H

#include "AmbaCFS_Util.h"

#define AMBA_CFS_MAX_STREAM  (32U)  /**< Max number of CFS handlers (hard limit : 32, due to the limitation of flag bits) */

/**
 * Bank information
 */
typedef struct AMBA_CFS_SCH_BANK_INFO_s_ {
    UINT64 Pos;                             /**< File position */
    UINT32 Length;                          /**< Length */
    UINT8 *Buffer;                          /**< Buffer */
    struct AMBA_CFS_SCH_BANK_INFO_s_ *Next; /**< Poniter to the next bank */
} AMBA_CFS_SCH_BANK_INFO_s;

/**
 * Asynchronous mode related structure
 */
typedef struct {
    AMBA_CFS_SCH_BANK_INFO_s *BankPool;     /**< A list of available banks */
    AMBA_CFS_SCH_BANK_INFO_s *Bank;         /**< Used bank Buffer (only 1 bank in write mode, could be a list in read mode) */
    UINT32 MaxNumBank;                      /**< Maximum available number of banks  */
    UINT32 NumBank;                         /**< Number of banks used by the stream */
    UINT32 Error;                           /**< Error code during async I/O */
    UINT8 Read;                             /**< 1 - rwBuf is for read, 0 - rwBuf is for write */
} AMBA_CFS_STREAM_ASYNC_DATA_s;

/**
 * CFS Stream structure
 */
typedef struct {
    char Filename[AMBA_CFS_MAX_FILENAME_LENGTH]; /**< File path (never change, no need protection) */
    AMBA_KAL_MUTEX_t Mutex;                 /**< Mutex */
    UINT64 Pos;                             /**< File position */
    UINT64 Length;                          /**< File length */
    UINT64 RealLength;                      /**< The real file length (actually written to file)(sync: only used by caller task, async: only used by sch task) */
    UINT32 AlignMode;                       /**< File alignment mode. (ex. AMBA_CFS_ALIGN_MODE_NONE) */
    UINT32 AlignSize;                       /**< File alignment size(in Byte) (The value should be a multiple of a cluster size.). It's valid when AlignMode is AMBA_CFS_ALIGN_MODE_SIZE or AMBA_CFS_ALIGN_MODE_CONTINUOUS. */
    UINT32 BytesToSync;                     /**< File should be sync after BytesToSync bytes were write (never change, no need protection) */
    UINT32 AccBytesNoSync;                  /**< Accumulated bytes since last sync (sync: only used by caller task, async: only used by sch task) */
    AMBA_FS_FILE *File;                     /**< File handler. (sync: only used by caller task, async: only used by sch task) */
    UINT8 Status;                           /**< File status, AMBA_CFS_STATUS_OPENED_READ/AMBA_CFS_STATUS_OPENED_WRITE */
                                            /**< For async: (1) on I/O error, sch will set AsyncData.Error, and the next fread()/fwrite() will set status as error; */
                                            /**<            (2) on close, it is safe to lock stream's mutex because no more I/O can be issued */
    UINT8 AsyncEnable;                      /**< Asynchronous mode (never change, no need protection) */
    UINT8 LengthGot;                        /**< To indicate if file length is got */
    UINT8 DmaEnable;                        /**< How to copy data in async read/write. 0: Memory copy, 1: DMA copy. (When using DMA on async write with cached buffer, be sure that the data in DRAM is correct.) */
    UINT8 StreamId;                         /**< Stream ID (never change, no need protection) */
} AMBA_CFS_STREAM_s;

#endif
