/**
 * @file ByteOp.c
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
#include "FormatAPI.h"
#include "ByteOp.h"

/**
 *  Read a byte from stream.
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetByte(SVC_STREAM_HDLR_s *Stream, UINT8 *Buffer)
{
    UINT32 Rval, Count;
    Rval = S2F(Stream->Func->Read(Stream, 1, Buffer, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != 1U) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Read 2 bytes from stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetLe16(SVC_STREAM_HDLR_s *Stream, UINT16 *Buffer)
{
    UINT32 Rval, Count;
    UINT8 Rtn[2];
    Rval = S2F(Stream->Func->Read(Stream, 2, Rtn, &Count));
    if (Rval == FORMAT_OK) {
        if (Count == 2U) {
            *Buffer = ((UINT16)Rtn[0] | ((UINT16)Rtn[1]<<8));
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Read 4 bytes from stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetLe32(SVC_STREAM_HDLR_s *Stream, UINT32 *Buffer)
{
    UINT32 Rval, Count;
    UINT8 Rtn[4];
    Rval = S2F(Stream->Func->Read(Stream, 4, Rtn, &Count));
    if (Rval == FORMAT_OK) {
        if (Count == 4U) {
            *Buffer = ((UINT32)Rtn[0] | ((UINT32)Rtn[1]<<8) | ((UINT32)Rtn[2]<<16) | ((UINT32)Rtn[3]<<24));
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Read 8 bytes from stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetLe64(SVC_STREAM_HDLR_s *Stream, UINT64 *Buffer)
{
    UINT32 Rval, Count;
    UINT8 Rtn[8];
    Rval = S2F(Stream->Func->Read(Stream, 8, Rtn, &Count));
    if (Rval == FORMAT_OK) {
        if (Count == 8U) {
            *Buffer = ((UINT64)Rtn[0] | ((UINT64)Rtn[1]<<8) | ((UINT64)Rtn[2]<<16) | ((UINT64)Rtn[3]<<24)
                    | ((UINT64)Rtn[4]<<32) | ((UINT64)Rtn[5]<<40) | ((UINT64)Rtn[6]<<48) | ((UINT64)Rtn[7]<<56));
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Read 2 bytes from stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetBe16(SVC_STREAM_HDLR_s *Stream, UINT16 *Buffer)
{
    UINT32 Rval, Count;
    UINT8 Rtn[2];
    Rval = S2F(Stream->Func->Read(Stream, 2, Rtn, &Count));
    if (Rval == FORMAT_OK) {
        if (Count == 2U) {
            *Buffer = (((UINT16)Rtn[0]<<8) | (UINT16)Rtn[1]);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Read 4 bytes from stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetBe32(SVC_STREAM_HDLR_s *Stream, UINT32 *Buffer)
{
    UINT32 Rval, Count;
    UINT8 Rtn[4];
    Rval = S2F(Stream->Func->Read(Stream, 4, Rtn, &Count));
    if (Rval == FORMAT_OK) {
        if (Count == 4U) {
            *Buffer = (((UINT32)Rtn[0]<<24) | ((UINT32)Rtn[1]<<16) | ((UINT32)Rtn[2]<<8) | (UINT32)Rtn[3]);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Read 8 bytes from stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetBe64(SVC_STREAM_HDLR_s *Stream, UINT64 *Buffer)
{
    UINT32 Rval, Count;
    UINT8 Rtn[8];
    Rval = S2F(Stream->Func->Read(Stream, 8, Rtn, &Count));
    if (Rval == FORMAT_OK) {
        if (Count == 8U) {
            *Buffer = (((UINT64)Rtn[0]<<56) | ((UINT64)Rtn[1]<<48) | ((UINT64)Rtn[2]<<40) | ((UINT64)Rtn[3]<<32)
                    | ((UINT64)Rtn[4]<<24) | ((UINT64)Rtn[5]<<16) | ((UINT64)Rtn[6]<<8) | (UINT64)Rtn[7]);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Write 1 byte to stream
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutByte(SVC_STREAM_HDLR_s *Stream, UINT8 Code)
{
    UINT32 Rval, Count;
    Rval = S2F(Stream->Func->Write(Stream, 1, &Code, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != 1U) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_IO_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Write 2 bytes to stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutLe16(SVC_STREAM_HDLR_s *Stream, UINT16 Code)
{
    UINT32 Rval, Count;
    UINT8 W[2];
    W[0] = (UINT8)(Code & 0xFFU);
    W[1] = (UINT8)((Code & 0xFF00U) >> 8);
    Rval = S2F(Stream->Func->Write(Stream, 2, W, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != 2U) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_IO_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Write 4 bytes to stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutLe32(SVC_STREAM_HDLR_s *Stream, UINT32 Code)
{
    UINT32 Rval, Count;
    UINT8 W[4];
    W[0] = (UINT8)(Code & 0xFFU);
    W[1] = (UINT8)((Code & 0xFF00U) >> 8);
    W[2] = (UINT8)((Code & 0xFF0000U) >> 16);
    W[3] = (UINT8)((Code & 0xFF000000U) >> 24);
    Rval = S2F(Stream->Func->Write(Stream, 4, W, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != 4U) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_IO_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Write 8 bytes to stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutLe64(SVC_STREAM_HDLR_s *Stream, UINT64 Code)
{
    UINT32 Rval, Count;
    UINT8 W[8];
    W[0] = (UINT8)(Code & 0xFFULL);
    W[1] = (UINT8)((Code & 0xFF00ULL) >> 8);
    W[2] = (UINT8)((Code & 0xFF0000ULL) >> 16);
    W[3] = (UINT8)((Code & 0xFF000000ULL) >> 24);
    W[4] = (UINT8)((Code & 0xFF00000000ULL) >> 32);
    W[5] = (UINT8)((Code & 0xFF0000000000ULL) >> 40);
    W[6] = (UINT8)((Code & 0xFF000000000000ULL) >> 48);
    W[7] = (UINT8)((Code & 0xFF00000000000000ULL) >> 56);
    Rval = S2F(Stream->Func->Write(Stream, 8, W, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != 8U) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_IO_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Write 2 bytes to stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutBe16(SVC_STREAM_HDLR_s *Stream, UINT16 Code)
{
    UINT32 Rval, Count;
    UINT8 W[2];
    W[0] = (UINT8)((Code & 0xFF00U) >> 8);
    W[1] = (UINT8)(Code & 0x00FFU);
    Rval = S2F(Stream->Func->Write(Stream, 2, W, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != 2U) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_IO_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Write 4 bytes to stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutBe32(SVC_STREAM_HDLR_s *Stream, UINT32 Code)
{
    UINT32 Rval, Count;
    UINT8 W[4];
    W[0] = (UINT8)((Code & 0xFF000000U) >> 24);
    W[1] = (UINT8)((Code & 0xFF0000U) >> 16);
    W[2] = (UINT8)((Code & 0xFF00U) >> 8);
    W[3] = (UINT8)(Code & 0xFFU);
    Rval = S2F(Stream->Func->Write(Stream, 4, W, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != 4U) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_IO_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Write 8 bytes to stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutBe64(SVC_STREAM_HDLR_s *Stream, UINT64 Code)
{
    UINT32 Rval, Count;
    UINT8 W[8];
    W[0] = (UINT8)((Code & 0xFF00000000000000ULL) >> 56);
    W[1] = (UINT8)((Code & 0xFF000000000000ULL) >> 48);
    W[2] = (UINT8)((Code & 0xFF0000000000ULL) >> 40);
    W[3] = (UINT8)((Code & 0xFF00000000ULL) >> 32);
    W[4] = (UINT8)((Code & 0xFF000000ULL) >> 24);
    W[5] = (UINT8)((Code & 0xFF0000ULL) >> 16);
    W[6] = (UINT8)((Code & 0xFF00ULL) >> 8);
    W[7] = (UINT8)(Code & 0xFFULL);
    Rval = S2F(Stream->Func->Write(Stream, 8, W, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != 8U) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_IO_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

#define SVC_EMPTY_BUFFER_SIZE 512U   /**< Empty buffer size */
/**
 *  Write N bytes to stream
 *
 *  @param [in] Stream source stream
 *  @param [in] Size the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutPadding(SVC_STREAM_HDLR_s *Stream, UINT32 Size)
{
    UINT32 Rval = FORMAT_OK;
    static UINT8 Empty[SVC_EMPTY_BUFFER_SIZE] = {0};
    UINT32 Rmsz = Size;
    UINT32 Count;
    while (Rmsz > SVC_EMPTY_BUFFER_SIZE) {
        Rval = S2F(Stream->Func->Write(Stream, SVC_EMPTY_BUFFER_SIZE, Empty, &Count));
        if (Rval == FORMAT_OK) {
            if (Count == SVC_EMPTY_BUFFER_SIZE) {
                Rmsz -= SVC_EMPTY_BUFFER_SIZE;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_IO_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        if (Rmsz > 0U) {
            Rval = S2F(Stream->Func->Write(Stream, Rmsz, Empty, &Count));
            if (Rval == FORMAT_OK) {
                if (Count < Rmsz) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_IO_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }
    return Rval;
}
#if 0
UINT16 SvcFormat_Le2Be16(UINT16 Code)
{
    UINT16 Rval;
    Rval  = (Code & 0xFFU) << 8;
    Rval += (Code & 0xFF00U) >> 8;
    return Rval;
}
#endif
/**
 *  Convert little endian to big endian
 *
 *  @param [in] Code the value to convert
 *  @return the resulted value
 */
UINT32 SvcFormat_Le2Be32(UINT32 Code)
{
    UINT32 Rval;
    Rval  = (Code & 0xFFU) << 24;
    Rval += (Code & 0xFF00U) << 8;
    Rval += (Code & 0xFF0000U) >> 8;
    Rval += (Code & 0xFF000000U) >> 24;
    return Rval;
}
#if 0
/**
 *  Convert little endian to big endian
 *
 *  @param [in] Code the value to convert
 *  @return the resulted value
 */
UINT64 SvcFormat_Le2Be64(UINT64 Code)
{
    UINT64 Rval;
    Rval  = (Code & 0xFFULL) << 56;
    Rval += (Code & 0xFF00ULL) << 40;
    Rval += (Code & 0xFF0000ULL) << 24;
    Rval += (Code & 0xFF000000ULL) << 8;
    Rval += (Code & 0xFF00000000ULL) >> 8;
    Rval += (Code & 0xFF0000000000ULL) >> 24;
    Rval += (Code & 0xFF000000000000ULL) >> 40;
    Rval += (Code & 0xFF00000000000000ULL) >> 56;
    return Rval;
}
#endif
