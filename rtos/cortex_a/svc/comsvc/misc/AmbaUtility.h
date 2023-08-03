/**
 *  @file AmbaUtility.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @details Header file for Ambarella Utility functions.
 *
 */

#ifndef AMBA_UTILITY_H
#define AMBA_UTILITY_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#define AMBA_CRC32_INIT_VALUE       (~0U)
#define UTIL_MAX_INT_STR_LEN        (22U)
#define UTIL_MAX_FLOAT_STR_LEN      (50U)
#define UTIL_MAX_STR_TOKEN_NUMBER   (32U)
#define UTIL_MAX_STR_TOKEN_LENGTH   (128U)

// string print functions
UINT32 AmbaUtility_StringPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs);
UINT32 AmbaUtility_StringPrintUInt64(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT64 *pArgs);
UINT32 AmbaUtility_StringPrintInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const INT32 *pArgs);
UINT32 AmbaUtility_StringPrintStr(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const char * const pArgs[]);

// string append functions
void AmbaUtility_StringAppend(char *pBuffer, UINT32 BufferSize, const char *pSource);
void AmbaUtility_StringAppendInt32(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix);
void AmbaUtility_StringAppendUInt32(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
void AmbaUtility_StringAppendFloat(char *pBuffer, UINT32 BufferSize, FLOAT Value, UINT32 AfterPoint);
void AmbaUtility_StringAppendDouble(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 AfterPoint);

// primitive type to string functions
UINT32 AmbaUtility_FloatToStr(char *pBuffer,  UINT32 BufferSize, FLOAT Value, UINT32 Afterpoint);
UINT32 AmbaUtility_DoubleToStr(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint);
UINT32 AmbaUtility_Int32ToStr(char *pBuffer,  UINT32 BufferSize, INT32 Value,  UINT32 Radix);
UINT32 AmbaUtility_Int64ToStr(char *pBuffer,  UINT32 BufferSize, INT64 Value,  UINT32 Radix);
UINT32 AmbaUtility_UInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
UINT32 AmbaUtility_UInt64ToStr(char *pBuffer, UINT32 BufferSize, UINT64 Value, UINT32 Radix);

// misc string functions
SIZE_t AmbaUtility_StringLength(const char *pString);
void   AmbaUtility_StringCopy(char *pDest, SIZE_t DestSize, const char *pSource);
INT32  AmbaUtility_StringCompare(const char *pString1, const char *pString2, SIZE_t Size);
UINT32 AmbaUtility_StringToken(const char *pString, char Delimiter, char Args[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH], UINT32 *ArgCount);
UINT32 AmbaUtility_StringToUInt32(const char *pString, UINT32 *pValue);
UINT32 AmbaUtility_StringToUInt64(const char *pString, UINT64 *pValue);

// memory functions
void AmbaUtility_MemorySetChar(char *pBuffer, char Value, UINT32 Size);
void AmbaUtility_MemorySetU8(UINT8 *pBuffer, UINT8 Value, UINT32 Size);
void AmbaUtility_MemorySetU32(UINT32 *pBuffer, UINT32 Value, UINT32 Size);

// CRC32 functions
UINT32 AmbaUtility_Crc32(const UINT8 *pBuffer, UINT32 Size);
UINT32 AmbaUtility_Crc32Add(const UINT8 *pBuffer, UINT32 Size, UINT32 Crc);
UINT32 AmbaUtility_Crc32Finalize(UINT32 LastValue);
UINT32 AmbaUtility_Crc32Sw(const UINT8 *pBuffer, UINT32 Size);
UINT32 AmbaUtility_Crc32AddSw(const UINT8 *pBuffer, UINT32 Size, UINT32 Crc);
UINT32 AmbaUtility_Crc32FinalizeSw(UINT32 LastValue);

// Decompress
UINT32 AmbaUtility_GZIPDecompress(UINT8 *pInputBuffer, UINT32 InputBufferSize, UINT8 *pOutputBuffer, UINT32 OutputBufferSize);

#endif
