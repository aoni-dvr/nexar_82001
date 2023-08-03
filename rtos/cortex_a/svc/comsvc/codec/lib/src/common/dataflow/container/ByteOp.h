/**
 * @file ByteOp.h
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
#ifndef CONTAINER_BYTEOP_H
#define CONTAINER_BYTEOP_H

#include "stream/SvcStream.h"
#include "FormatAPI.h"

/**
 *  Read a byte from stream.
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetByte(SVC_STREAM_HDLR_s *Stream, UINT8 *Buffer);

/**
 *  Read 2 bytes from stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetLe16(SVC_STREAM_HDLR_s *Stream, UINT16 *Buffer);

/**
 *  Read 4 bytes from stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetLe32(SVC_STREAM_HDLR_s *Stream, UINT32 *Buffer);

/**
 *  Read 8 bytes from stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetLe64(SVC_STREAM_HDLR_s *Stream, UINT64 *Buffer);

/**
 *  Read 2 bytes from stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetBe16(SVC_STREAM_HDLR_s *Stream, UINT16 *Buffer);

/**
 *  Read 4 bytes from stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetBe32(SVC_STREAM_HDLR_s *Stream, UINT32 *Buffer);

/**
 *  Read 8 bytes from stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [out] Buffer the buffer
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetBe64(SVC_STREAM_HDLR_s *Stream, UINT64 *Buffer);

/**
 *  Write 1 byte to stream
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutByte(SVC_STREAM_HDLR_s *Stream, UINT8 Code);

/**
 *  Write 2 bytes to stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutLe16(SVC_STREAM_HDLR_s *Stream, UINT16 Code);

/**
 *  Write 4 bytes to stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutLe32(SVC_STREAM_HDLR_s *Stream, UINT32 Code);

/**
 *  Write 8 bytes to stream (little endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutLe64(SVC_STREAM_HDLR_s *Stream, UINT64 Code);

/**
 *  Write 2 bytes to stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutBe16(SVC_STREAM_HDLR_s *Stream, UINT16 Code);

/**
 *  Write 4 bytes to stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutBe32(SVC_STREAM_HDLR_s *Stream, UINT32 Code);

/**
 *  Write 8 bytes to stream (big endian)
 *
 *  @param [in] Stream source stream
 *  @param [in] Code the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutBe64(SVC_STREAM_HDLR_s *Stream, UINT64 Code);

/**
 *  Write N bytes to stream
 *
 *  @param [in] Stream source stream
 *  @param [in] Size the value to write
 *  @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutPadding(SVC_STREAM_HDLR_s *Stream, UINT32 Size);
#if 0
/**
 *  Convert little endian to big endian
 *
 *  @param [in] Code the value to convert
 *  @return the resulted value
 */
UINT16 SvcFormat_Le2Be16(UINT16 Code);
#endif
/**
 *  Convert little endian to big endian
 *
 *  @param [in] Code the value to convert
 *  @return the resulted value
 */
UINT32 SvcFormat_Le2Be32(UINT32 Code);
#if 0
/**
 *  Convert little endian to big endian
 *
 *  @param [in] Code the value to convert
 *  @return the resulted value
 */
UINT64 SvcFormat_Le2Be64(UINT64 Code);
#endif
#endif

