/**
 * @file BitOp.h
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
#ifndef CONTAINER_BITOP_H
#define CONTAINER_BITOP_H

#include "stream/SvcStream.h"
#include "FormatAPI.h"

/**
 *  Bit Operation
 */
typedef struct{
    UINT8 *BsBuffer;    /**< Buffer */
    UINT32 BsOffset;    /**< Byte Offset */
    UINT32 BsLen;       /**< Length */
    UINT32 BitPos;      /**< Bit position of the current byte */
} SVC_BIT_OP_s;

/**
 * ue(v) decode
 * unsigned integer Exp-Golomb-coded syntax element with the left bit first.
 *
 * @param [in] Ctx The context of bit operation
 * @param [out] Data The returned data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetUe(SVC_BIT_OP_s *Ctx, UINT32 *Data);

/**
 * se(v) decode
 *
 * @param [in] Ctx The context of bit operation
 * @param [out] Data The returned data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetSe(SVC_BIT_OP_s *Ctx, INT32 *Data);

/**
 * Get one bit
 *
 * @param [in] Ctx The context of bit operation
 * @param [out] Data The returned data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Get1Bit(SVC_BIT_OP_s *Ctx, UINT8 *Data);

/**
 * Get "Len" bits
 *
 * @param [in] Ctx The context of bit operation
 * @param [in] Len Number of bits
 * @param [out] Data The returned data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetBit(SVC_BIT_OP_s *Ctx, UINT8 Len, UINT32 *Data);

/**
 * more_data_in_rbsp()
 *
 * @param [in] Ctx The context of bit operation
 * @param [in] Len Number of bits
 * @return Count
 */
INT32 SvcFormat_MoreRbspSata(SVC_BIT_OP_s const *Ctx, UINT32 Len);

/**
 * Initialize the context of bit operation
 *
 * @param [in] Ctx The context of bit operation
 * @param [in] Buffer Pointer to the buffer
 * @param [in] BufferLen Buffer length
 */
void SvcFormat_InitBitBuf(SVC_BIT_OP_s *Ctx, UINT8 *Buffer, UINT32 BufferLen);

#if 0
UINT32 SvcFormat_BitBufEos(const SVC_BIT_OP_s *Ctx);
#endif

#endif

