/**
 * @file MemByteOp.c
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
#include "MemByteOp.h"
#include "format/SvcFormat.h"

/**
 * Get current position of memory
 *
 * @param [in] Ctx The context of memory operation
 * @return position
 */
UINT64 SvcFormat_GetMemPos(const SVC_MEM_OP_s *Ctx)
{
    return (UINT64)Ctx->Offset;
}

/**
 * Get total size of memory
 *
 * @param [in] Ctx The context of memory operation
 * @return size
 */
static UINT64 SvcFormat_GetMemLength(const SVC_MEM_OP_s *Ctx)
{
    return (UINT64)Ctx->Size;
}

/**
 * Read N bytes data from memory
 *
 * @param [in] Ctx The context of memory operation
 * @param [in] ReadSize The size of readSize
 * @param [in] ReadBuffer The buffer of data
 * @param [out] Count The returned number of elements read
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_ReadMem(SVC_MEM_OP_s *Ctx, UINT32 ReadSize, UINT8 *ReadBuffer, UINT32 *Count)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 TempCount = 0U;
    UINT8 *TmpBuffer = ReadBuffer;
    if (Ctx->Buffer == NULL) {
        if (Ctx->Base != NULL) {
            TempCount = (ReadSize > Ctx->Size) ? Ctx->Size : ReadSize;
            /* read */
            Rval = W2F(AmbaWrap_memcpy(ReadBuffer, &Ctx->Base[Ctx->Offset], TempCount));
            Ctx->Offset += TempCount;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Null pointer!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        const UINT64 Offset = (UINT64)Ctx->BufferSize - 1ULL;
        const UINT64 Align = 0xFFFFFFFFFFFFFFFFULL - Offset;  // trick: let buffer_size always be power of 2
        const UINT64 Size = SvcFormat_GetMemLength(Ctx);
        UINT64 CurPos = SvcFormat_GetMemPos(Ctx);
        UINT32 RemainSize = ReadSize;
        while ((RemainSize > 0U) && (CurPos < Size)) {
//            AmbaPrint_PrintUInt5("%u %u", (CurPos & Align), (Ctx->BufferBase & Align), 0U, 0U, 0U);
            if ((CurPos & Align) != (Ctx->BufferBase & Align)) {
                //re-read data to buffer
                UINT64 TempPos;
                UINT32 r = 0;
                Rval = S2F(Ctx->Stream->Func->GetPos(Ctx->Stream, &TempPos));
                if (Rval == FORMAT_OK) {
                    Ctx->BufferBase = (CurPos & Align);
                    Rval = S2F(Ctx->Stream->Func->Seek(Ctx->Stream, (INT64)Ctx->BufferBase, SVC_STREAM_SEEK_START));
                    if (Rval == FORMAT_OK) {
                        Rval = S2F(Ctx->Stream->Func->Read(Ctx->Stream, Ctx->BufferSize, Ctx->Buffer, &r));
                        if (Rval == FORMAT_OK) {
                            Rval = S2F(Ctx->Stream->Func->Seek(Ctx->Stream, (INT64)TempPos, SVC_STREAM_SEEK_START));
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read fail!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetPos fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
            if (Rval == FORMAT_OK) {
                UINT64 BufIndex = CurPos & Offset;
                const UINT8 *Cur = &Ctx->Buffer[BufIndex];
                UINT32 Len = Ctx->BufferSize - (UINT32)BufIndex;
//                AmbaPrint_PrintUInt5("Size %u CurPos %u Len %u ReadSize %u", Size, CurPos, Len, ReadSize, 0U);
                if (Len > RemainSize) {
                    Len = RemainSize;
                }
                if (Len > ((UINT32)Size - (UINT32)CurPos)) {
                    Len = (UINT32)Size - (UINT32)CurPos;
                }
//                AmbaPrint_PrintUInt5("Len %u Ctx->BufferSize %u Index %u", Len, Ctx->BufferSize, BufIndex, 0U, 0U);
                Rval = W2F(AmbaWrap_memcpy(TmpBuffer, Cur, Len));
                TmpBuffer = &TmpBuffer[Len];
                Ctx->Offset += Len;
                RemainSize -= Len;
                CurPos += Len;
                TempCount += Len;
            }
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    *Count = TempCount;
    return Rval;
}

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
UINT32 SvcFormat_WriteMem(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 WriteSize, const UINT8 *Buffer)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    UINT32 Len = WriteSize;
    if (Buffer != NULL) {
        if (*Offset < Size){
            if ((*Offset + Len) > Size) {
                Len = Size - *Offset;
            }
            Rval = W2F(AmbaWrap_memcpy(&Base[*Offset], Buffer, Len));
            (*Offset) += Len;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong Ptr!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Null Ptr!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Doing memory seek
 *
 * @param [in] Ctx The context of memory operation
 * @param [in] Offset The offset of memory address
 * @param [in] Origin The seek method
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_SeekMem(SVC_MEM_OP_s *Ctx, INT64 Offset, INT32 Origin)
{
    UINT32 Rval = FORMAT_OK;
    INT64 TempOffI64 = 0;
    switch (Origin) {
    case SVC_STREAM_SEEK_START:
        TempOffI64 = Offset;
        break;
    case SVC_STREAM_SEEK_CUR:
        TempOffI64 = (INT64)Ctx->Offset + Offset;
        break;
    case SVC_STREAM_SEEK_END:
        TempOffI64 = (INT64)Ctx->Size + Offset;
        break;
    default:
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong Origin!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
        break;
    }
    if (Rval == FORMAT_OK) {
        if (TempOffI64 > (INT64)Ctx->Size) {
            Ctx->Offset = Ctx->Size;
        } else if (TempOffI64 < 0) {
            Ctx->Offset = 0U;
        } else {
            Ctx->Offset = (UINT32) TempOffI64;
        }
    }
    return Rval;
}

/**
 * Read a byte to Memory
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemByte(SVC_MEM_OP_s *Ctx, UINT8 *Buffer)
{
    UINT32 Rval;
    UINT8 Rtn[1] = {0};
    UINT32 Count;
    Rval = SvcFormat_ReadMem(Ctx, 1, Rtn, &Count);
    if (Rval == FORMAT_OK) {
        if (Count == 1U) {
            *Buffer = Rtn[0];
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Not enough bytes to read!!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    }
    return Rval;
}

/**
 * Read 2 byte to Memory (little endian)
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemLe16(SVC_MEM_OP_s *Ctx, UINT16 *Buffer)
{
    UINT32 Rval;
    UINT8 Rtn[2] = {0};
    UINT32 Count;
    Rval = SvcFormat_ReadMem(Ctx, 2, Rtn, &Count);
    if (Rval == FORMAT_OK) {
        if (Count == 2U) {
            *Buffer = ((UINT16)Rtn[0] | ((UINT16)Rtn[1]<<8));
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Not enough bytes to read!!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    }
    return Rval;
}

/**
 * Read 4 byte to Memory(little endian)
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemLe32(SVC_MEM_OP_s *Ctx, UINT32 *Buffer)
{
    UINT32 Rval;
    UINT8 Rtn[4] = {0};
    UINT32 Count;
    Rval = SvcFormat_ReadMem(Ctx, 4, Rtn, &Count);
    if (Rval == FORMAT_OK) {
        if (Count == 4U) {
            *Buffer = ((UINT32)Rtn[0] | ((UINT32)Rtn[1]<<8) | ((UINT32)Rtn[2]<<16) | ((UINT32)Rtn[3]<<24));
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Not enough bytes to read!!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    }
    return Rval;
}

/**
 * Read 2 byte to Memory(big endian)
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemBe16(SVC_MEM_OP_s *Ctx, UINT16 *Buffer)
{
    UINT32 Rval;
    UINT8 Rtn[2] = {0};
    UINT32 Count;
    Rval = SvcFormat_ReadMem(Ctx, 2, Rtn, &Count);
    if (Rval == FORMAT_OK) {
        if (Count == 2U) {
            *Buffer = (((UINT16)Rtn[0]<<8) | (UINT16)Rtn[1]);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Not enough bytes to read!!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    }
    return Rval;
}

/**
 * Read 4 byte to Memory(big endian)
 *
 * @param [in] Ctx mem buffer param
 * @param [out] Buffer the buffer to get data
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetMemBe32(SVC_MEM_OP_s *Ctx, UINT32 *Buffer)
{
    UINT32 Rval;
    UINT8 Rtn[4] = {0};
    UINT32 Count;
    Rval = SvcFormat_ReadMem(Ctx, 4, Rtn, &Count);
    if (Rval == FORMAT_OK) {
        if (Count == 4U) {
            *Buffer = (((UINT32)Rtn[0]<<24) | ((UINT32)Rtn[1]<<16) | ((UINT32)Rtn[2]<<8) | (UINT32)Rtn[3]);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Not enough bytes to read!!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    }
    return Rval;
}

/**
 * Write a byte to Memory
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemByte(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT8 Code)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if ((*Offset + 1U) <= Size) {
        UINT8 W[1];
        W[0] = (UINT8)(Code & 0xFFU);
        Base[*Offset] = W[0];
        (*Offset)++;
        Rval = FORMAT_OK;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong Ptr!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write 2 bytes to Memory (little endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemLe16(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT16 Code)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if ((*Offset + 2U) <= Size) {
        UINT8 W[2];
        W[0] = (UINT8)(Code & 0xFFU);
        W[1] = (UINT8)((Code & 0xFF00U)>>8);
        Rval = W2F(AmbaWrap_memcpy(&Base[*Offset], W, 2));
        (*Offset) += 2U;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong Ptr!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write 4 bytes to Memory (little endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemLe32(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 Code)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if ((*Offset + 4U) <= Size) {
        UINT8 W[4];
        W[0] = (UINT8)(Code & 0xFFU);
        W[1] = (UINT8)((Code & 0xFF00U)>>8);
        W[2] = (UINT8)((Code & 0xFF0000U)>>16);
        W[3] = (UINT8)((Code & 0xFF000000U)>>24);
        Rval = W2F(AmbaWrap_memcpy(&Base[*Offset], W, 4));
        (*Offset) += 4U;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong Ptr!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}
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
UINT32 SvcFormat_PutMemLe64(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT64 Code)
{
    UINT32 Rval;
    Rval = SvcFormat_PutMemLe32(Offset, Base, Size, (UINT32)Code);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutMemLe32(Offset, Base, Size, 0);
    }
    return Rval;
}
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
UINT32 SvcFormat_PutMemBe16(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT16 Code)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if ((*Offset + 2U) <= Size) {
        UINT8 W[2];
        W[0] = (UINT8)((Code & 0xFF00U) >> 8);
        W[1] = (UINT8)(Code & 0x00FFU);
        Rval = W2F(AmbaWrap_memcpy(&Base[*Offset], W, 2));
        (*Offset) += 2U;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong Ptr!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write 4 bytes to Memory (Big endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemBe32(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 Code)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if ((*Offset + 4U) <= Size) {
        UINT8 W[4];
        W[0] = (UINT8)((Code & 0xFF000000U) >> 24);
        W[1] = (UINT8)((Code & 0xFF0000U) >> 16);
        W[2] = (UINT8)((Code & 0xFF00U) >> 8);
        W[3] = (UINT8)(Code & 0xFFU);
        Rval = W2F(AmbaWrap_memcpy(&Base[*Offset], W, 4));
        (*Offset) += 4U;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong Ptr!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Write 8 bytes to Memory (Big endian)
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] Code the value to write
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemBe64(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT64 Code)
{
    UINT32 Rval;
    Rval = SvcFormat_PutMemBe32(Offset, Base, Size, 0);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutMemBe32(Offset, Base, Size, (UINT32)Code);
    }
    return Rval;
}

/**
 * Write n bytes 0x00 to Memory
 *
 * @param [in] Offset buffer offset
 * @param [in] Base base address
 * @param [in] Size buffer size
 * @param [in] PaddingSize the size of empty bytes
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutMemPadding(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 PaddingSize)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if ((*Offset + PaddingSize) <= Size) {
        Rval = W2F(AmbaWrap_memset(&Base[*Offset], 0, PaddingSize));
        (*Offset) += PaddingSize;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong Ptr!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

