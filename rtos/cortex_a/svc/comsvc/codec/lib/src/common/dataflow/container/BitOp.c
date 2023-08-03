/**
 * @file BitOp.c
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

#include "format/SvcFormat.h"
#include "BitOp.h"

static const UINT8 BitMask8[8] = {0xFFU, 0x7FU, 0x3FU, 0x1FU, 0x0FU, 0x07U, 0x03U, 0x01U};

/**
 * ue(v) decode
 * unsigned integer Exp-Golomb-coded syntax element with the left bit first.
 *
 * @param [in] Ctx The context of bit operation
 * @param [out] Data The returned data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetUe(SVC_BIT_OP_s *Ctx, UINT32 *Data)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 lzb = 0U;
    while (Rval == FORMAT_OK) {
        UINT8 b = 0U;
        Rval = SvcFormat_Get1Bit(Ctx, &b);
        if (Rval == FORMAT_OK) {
            if (b == 0U) {
                lzb++;
            } else {
                break;
            }
        }
    }
    if (Rval == FORMAT_OK) {
        if (lzb == 0U) {
            *Data = 0U;
        } else {
            UINT32 TmpU32;
            Rval = SvcFormat_GetBit(Ctx, lzb, &TmpU32);
            if (Rval == FORMAT_OK) {
                *Data = ((UINT32)1U << lzb) - 1U + TmpU32;
            }
        }
    }
    return Rval;
}

/**
 * se(v) decode
 *
 * @param [in] Ctx The context of bit operation
 * @param [out] Data The returned data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetSe(SVC_BIT_OP_s *Ctx, INT32 *Data)
{
    UINT32 Rval;
    UINT32 TmpU32;
    Rval = SvcFormat_GetUe(Ctx, &TmpU32);
    if (Rval == FORMAT_OK) {
        if (TmpU32 <= 1U) {
            *Data = (INT32)TmpU32;
        } else {
            DOUBLE TempData;
            Rval = W2F(AmbaWrap_ceil((DOUBLE)TmpU32/2., &TempData));
            if (Rval == FORMAT_OK) {
                if ((TmpU32 & 1U) == 0U) {
                    TempData = -TempData;
                }
            }
            *Data = (INT32)TempData;
        }
    }
    return Rval;
}

/**
 * Get one bit
 *
 * @param [in] Ctx The context of bit operation
 * @param [out] Data The returned data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Get1Bit(SVC_BIT_OP_s *Ctx, UINT8 *Data)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 Next = Ctx->BitPos + 1U;
    UINT8 TempData = Ctx->BsBuffer[Ctx->BsOffset];
    TempData = TempData & BitMask8[Ctx->BitPos];
    TempData >>= (8U - Next);
    if (Next == 8U) {
        if (Ctx->BsOffset > Ctx->BsLen) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong ptr!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        } else {
            Ctx->BsOffset++;
            Ctx->BitPos = 0;
        }
    } else {
        Ctx->BitPos++;
    }
    *Data = TempData;
    return Rval;
}

/**
 * Get "Len" bits
 *
 * @param [in] Ctx The context of bit operation
 * @param [in] Len Number of bits
 * @param [out] Data The returned data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetBit(SVC_BIT_OP_s *Ctx, UINT8 Len, UINT32 *Data)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 Next = Ctx->BitPos + Len;
    if (Next < 9U) {
        register UINT8 TempData = Ctx->BsBuffer[Ctx->BsOffset];//*(Ctx->BsPtr);
        TempData = TempData & BitMask8[Ctx->BitPos];
        *Data = ((UINT32)TempData >> (8U - Next));
    } else if (Next < 17U) {
        static const UINT16 BitMask16[16] = {
        0xFFFFU, 0x7FFFU, 0x3FFFU, 0x1FFFU, 0x0FFFU, 0x07FFU, 0x03FFU, 0x01FFU,
        0x00FFU, 0x007FU, 0x003FU, 0x001FU, 0x000FU, 0x0007U, 0x0003U, 0x0001U
        };
        register UINT16 TempData = (UINT16)((UINT16)Ctx->BsBuffer[Ctx->BsOffset] << 8U) | (UINT16)Ctx->BsBuffer[Ctx->BsOffset + 1U];
        TempData = TempData & BitMask16[Ctx->BitPos];
        *Data = ((UINT32)TempData >> (16U - Next));
    } else {
        static const UINT32 BitMask32[32] = {
        0xFFFFFFFFU, 0x7FFFFFFFU, 0x3FFFFFFFU, 0x1FFFFFFFU, 0x0FFFFFFFU, 0x07FFFFFFU, 0x03FFFFFFU, 0x01FFFFFFU,
        0x00FFFFFFU, 0x007FFFFFU, 0x003FFFFFU, 0x001FFFFFU, 0x000FFFFFU, 0x0007FFFFU, 0x0003FFFFU, 0x0001FFFFU,
        0x0000FFFFU, 0x00007FFFU, 0x00003FFFU, 0x00001FFFU, 0x00000FFFU, 0x000007FFU, 0x000003FFU, 0x000001FFU,
        0x000000FFU, 0x0000007FU, 0x0000003FU, 0x0000001FU, 0x0000000FU, 0x00000007U, 0x00000003U, 0x00000001U
        };
        register UINT32 TempData = ((UINT32)Ctx->BsBuffer[Ctx->BsOffset] << 24U) | ((UINT32)Ctx->BsBuffer[Ctx->BsOffset + 1U] << 16U) | ((UINT32)Ctx->BsBuffer[Ctx->BsOffset + 2U] << 8U) | (UINT32)Ctx->BsBuffer[Ctx->BsOffset + 2U];
        TempData = TempData & BitMask32[Ctx->BitPos];
        *Data = ((UINT32)TempData >> (32U - Next));
    }
    while (Next >= 8U) {
        if (Ctx->BsOffset > Ctx->BsLen) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong offset!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
            break;
        } else {
            Ctx->BsOffset++;
            Next -= 8U;
        }
    }
    Ctx->BitPos = Next;
    return Rval;
}

/**
 * more_data_in_rbsp()
 *
 * @param [in] Ctx The context of bit operation
 * @param [in] Len Number of bits
 * @return Count
 */
INT32 SvcFormat_MoreRbspSata(SVC_BIT_OP_s const *Ctx, UINT32 Len)
{
    INT32 Count = 1;
    UINT32 ByteOffset = Ctx->BsOffset;

    /* there is more until we're in the last byte */
    if (ByteOffset >= (Len - 1U)) {
        UINT32 BitOffset = Ctx->BitPos;                 /* bit from start of byte */
        UINT8 CurByte  = Ctx->BsBuffer[Ctx->BsOffset];
        UINT8 CtrlBit = 0;                  /* control bit for current bit posision */
        /* read one bit */
        CurByte = CurByte & BitMask8[BitOffset];
        CtrlBit = (CurByte >> (7U - BitOffset));
        BitOffset++;
        /* a stop bit has to be one */
        if (CtrlBit != 0U) {
            Count = 0;
            while ((BitOffset != 8U) && (Count == 0)) {
                UINT32 CountTmp;
                CurByte = CurByte & BitMask8[BitOffset];
                CtrlBit = (CurByte >> (7U - BitOffset));
                CountTmp = (UINT32)Count | (UINT32)CtrlBit; /* for rule 10.8 */
                Count = (INT32) CountTmp;
                BitOffset++;
            }
        }
    }
    return Count;
}

#if 0
UINT32 SvcFormat_BitBufEos(const SVC_BIT_OP_s *Ctx)
{
    UINT32 Rval;
    if (Ctx->BsOffset >= (Ctx->BsLen - 1U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}
#endif

/**
 * Initialize the context of bit operation
 *
 * @param [in] Ctx The context of bit operation
 * @param [in] Buffer Pointer to the buffer
 * @param [in] BufferLen Buffer length
 */
void SvcFormat_InitBitBuf(SVC_BIT_OP_s *Ctx, UINT8 *Buffer, UINT32 BufferLen)
{
    Ctx->BsBuffer = Buffer;
    Ctx->BsOffset = 0U;
    Ctx->BsLen = BufferLen;
    Ctx->BitPos = 0U;
}

