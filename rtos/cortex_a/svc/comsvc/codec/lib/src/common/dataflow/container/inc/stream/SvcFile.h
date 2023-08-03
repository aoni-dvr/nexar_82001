/**
 * @file SvcFile.h
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
#ifndef CONTAINER_FILE_H
#define CONTAINER_FILE_H
/**
 * Stream data from/to file
 *
 * The File Stream module includes the following functions:
 * 1. Initiate the File Stream module
 * 2. Create file streams
 * 3. Delete file streams
 */

#include "stream/SvcStream.h"

/**
 * The configuration for initializing the File Stream module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the File Stream module */
    UINT32 BufferSize;      /**< The size of the work buffer */
    UINT8 MaxHdlr;          /**< The maximum number of handlers held in the File Stream module */
} SVC_FILE_STREAM_INIT_CFG_s;

/**
 * The configuration for initializing a file stream handler
 */
typedef struct {
    UINT32 Alignment;       /**< The alignment of file size */
    UINT32 BytesToSync;     /**< The number of bytes to sync FAT */
    UINT8 Async;            /**< The parameters of async mode */
    UINT8 *BankBuffer;      /**< The bank buffer. This is dummy when Async = 0. */
    UINT32 BankBufferSize;  /**< The size of the bank buffer. This is dummy when Async = 0. The bank amount will be (BankBufferSize / BankSize). BankBufferSize should be aligned to BankSize. */
} SVC_FILE_STREAM_CFG_s;

/**
 * Get the default configuration for initializing the File Stream module.
 *
 * @param [out] Config The returned configuration
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_GetInitDefaultCfg(SVC_FILE_STREAM_INIT_CFG_s *Config);

/**
 * Get the required buffer size for initializing the File Stream module.
 *
 * @param [in] MaxHdlr The maximum number of handlers held in the File Stream module
 * @param [in] BufferSize The required buffer size
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_GetInitBufferSize(UINT8 MaxHdlr, UINT32 *BufferSize);

/**
 * Initialize the File Stream module.
 *
 * @param [in] Config The configuration used to initialize the File Stream module
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_Init(const SVC_FILE_STREAM_INIT_CFG_s *Config);

/**
 * Get the default configuration of a file stream handler.
 *
 * @param [out] Config The returned configuration
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_GetDefaultCfg(SVC_FILE_STREAM_CFG_s *Config);

/**
 * Create a file stream handler.
 *
 * @param [in] Config The configuration used to create a file stream handler
 * @param [out] Hdlr The returned handler
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_Create(const SVC_FILE_STREAM_CFG_s *Config, SVC_STREAM_HDLR_s **Hdlr);

/**
 * Delete a file stream handler.
 *
 * @param [in] Hdlr The handler of a file stream being removed
 * @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcFileStream_Delete(SVC_STREAM_HDLR_s *Hdlr);

#endif
