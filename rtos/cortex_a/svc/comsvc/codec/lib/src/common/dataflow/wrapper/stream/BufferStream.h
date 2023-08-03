/**
 * @file BufferStream.h
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
#ifndef CONTAINER_BUFFER_STREAM_H
#define CONTAINER_BUFFER_STREAM_H
/**
 * Stream data from/to buffer
 *
 * The File Stream module includes the following functions:
 * 1. Initiate the Buffer Stream module
 * 2. Create uffer streams
 * 3. Delete buffer streams
 */
#include "stream/SvcStream.h"

#define SVC_BUFFER_STREAM_MAX_NUM (16U) /**< The maximum number of handlers held in the Buffer Stream module */

/**
 *  The configuration for initializing a file stream handler
 */
typedef struct {
    UINT32 (*pfnBufCopy)(ULONG Dst, ULONG Src, UINT32 Size);  /**< Function pointer that copy data to the buffer */
} SVC_BUFFER_STREAM_CFG_s;

/**
 *  Initialize the buffer stream module.
 *
 *  @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcBufferStream_Init(void);

/**
 *  Create a buffer stream handler.
 *
 *  @param [in] Config The configuration used to create a buffer stream handler
 *  @param [out] Hdlr The returned handler
 *  @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcBufferStream_Create(const SVC_BUFFER_STREAM_CFG_s *Config, SVC_STREAM_HDLR_s **Hdlr);

/**
 *  Delete a buffer stream handler.
 *
 *  @param [in] Hdlr The handler of a buffer stream being removed
 *  @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcBufferStream_Delete(SVC_STREAM_HDLR_s *Hdlr);

/**
 *  Config the destination buffer of a buffer stream handler
 *
 *  @param [in] Hdlr The buffer stream handler
 *  @param [in] Addr The new destination buffer
 *  @param [in] Size The size of the new destination buffer
 *  @return OK or STREAM_ERR_XXXX
 */
UINT32 SvcBufferStream_ConfigBuffer(SVC_STREAM_HDLR_s *Hdlr, ULONG Addr, UINT32 Size);

#endif
