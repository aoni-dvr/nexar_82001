/**
 * @file MemByteOp.h
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
#ifndef CONTAINER_MEMBYTEOP_H
#define CONTAINER_MEMBYTEOP_H

#include "stream/SvcFile.h"
#include "AmbaFS.h"
#include "FormatAPI.h"

/**
 *  Memory operation
 */
typedef struct{
    UINT32 Offset;      /**< r/w offset */
    UINT8 *Base;        /**< ptr of data base */
    UINT32 Size;        /**< data size */
    UINT8 *Buffer;      /**< buffer address to 256 byte buffer,  NULL if no buffer used. */
    UINT64 BufferBase;  /**< pt to buffered data start */
    UINT32 BufferSize;  /**< buffer size */
    SVC_STREAM_HDLR_s *Stream; /**< stream handler */
} SVC_MEM_OP_s;

/**
 * Get current position of memory
 *
 * @param [in] Ctx The context of memory operation
 * @return position
 */
UINT64 SvcFormat_GetMemPos(const SVC_MEM_OP_s *Ctx);

/**
 * Read N bytes data from memory
 *
 * @param [in] Ctx The context of memory operation
 * @param [in] ReadSize The size of readSize
 * @param [in] ReadBuffer The buffer of data
 * @param [out] Count The returned number of elements read
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_ReadMem(SVC_MEM_OP_s *Ctx, UINT32 ReadSize, UINT8 *ReadBuffer, UINT32 *Count);

/**
 * Put N bytes data to memory
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] WriteSize The size of data
 * @param [in] Buffer The data to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_WriteMem(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 WriteSize, const UINT8 *Buffer);

/**
 * Doing memory seek
 *
 * @param [in] Ctx The context of memory operation
 * @param [in] Offset The offset of memory address
 * @param [in] Origin The seek method
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_SeekMem(SVC_MEM_OP_s *Ctx, INT64 Offset, INT32 Origin);

/**
 * Read a byte to Memory
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemByte(SVC_MEM_OP_s *Ctx, UINT8 *Buffer);

/**
 * Read 2 byte to Memory (little endian)
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemLe16(SVC_MEM_OP_s *Ctx, UINT16 *Buffer);

/**
 * Read 4 byte to Memory(little endian)
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemLe32(SVC_MEM_OP_s *Ctx, UINT32 *Buffer);

/**
 * Read 2 byte to Memory(big endian)
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemBe16(SVC_MEM_OP_s *Ctx, UINT16 *Buffer);

/**
 * Read 4 byte to Memory(big endian)
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemBe32(SVC_MEM_OP_s *Ctx, UINT32 *Buffer);

/**
 * Write a byte to Memory
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemByte(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT8 Code);

/**
 * Write 2 bytes to Memory (little endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemLe16(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT16 Code);

/**
 * Write 4 bytes to Memory (little endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemLe32(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 Code);
#if 0
/**
 * Write 8 bytes to Memory (little endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemLe64(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT64 Code);
#endif
/**
 * Write 2 bytes to Memory (Big endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemBe16(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT16 Code);

/**
 * Write 4 bytes to Memory (Big endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemBe32(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 Code);

/**
 * Write 8 bytes to Memory (Big endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemBe64(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT64 Code);

/**
 * Write n bytes 0x00 to Memory
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] PaddingSize the size of empty bytes
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemPadding(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 PaddingSize);

#endif
