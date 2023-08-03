/**
 *  @file AmbaIOUtility.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header file for Ambarella IO Utility functions.
 *
 */

#ifndef AMBAIO_UTILITY_H
#define AMBAIO_UTILITY_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#define IO_UTIL_CRC32_INIT_VALUE       (~0U)
#define IO_UTIL_MAX_INT_STR_LEN        (22U)
#define IO_UTIL_MAX_FLOAT_STR_LEN      (50U)
#define IO_UTIL_MAX_STR_TOKEN_NUMBER   (32U)
#define IO_UTIL_MAX_STR_TOKEN_LENGTH   (128U)

// string print functions
UINT32 IO_UtilityStringPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs);
UINT32 IO_UtilityStringPrintUInt64(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT64 *pArgs);
UINT32 IO_UtilityStringPrintStr(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const char * const pArgs[]);

// string append functions
void IO_UtilityStringAppend(char *pBuffer, UINT32 BufferSize, const char *pSource);

// primitive type to string functions
UINT32 IO_UtilityUInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
UINT32 IO_UtilityUInt64ToStr(char *pBuffer, UINT32 BufferSize, UINT64 Value, UINT32 Radix);

// misc string functions
SIZE_t IO_UtilityStringLength(const char *pString);
void   IO_UtilityStringCopy(char *pDest, SIZE_t DestSize, const char *pSource);
INT32  IO_UtilityStringCompare(const char *pString1, const char *pString2, SIZE_t Size);

// memory functions

// CRC32 functions
UINT32 IO_UtilityCrc32(const UINT8 *pBuffer, UINT32 Size);
#ifdef CONFIG_QNX
UINT32 IO_UtilityCrc32Sw(const UINT8 *pBuffer, UINT32 Size);
#endif

// FDT (Flat Device Tree) functions
INT32  IO_UtilityFDTNodeOffsetByCID(INT32 Offset, const char *CompatibleStr);
UINT32 IO_UtilityFDTPropertyU32(INT32 Offset, const char *PropName, UINT32 Index);
UINT32 IO_UtilityFDTPropertyU32Quick(INT32 Offset, const char *CompatibleStr, const char *PropName, UINT32 Index);
UINT64 IO_UtilityFDTPropertyU64Quick(INT32 Offset, const char *CompatibleStr, const char *PropName, UINT32 Index);
INT32  IO_UtilityFDTPHandleOffset(UINT32 PHandle);

// convert UINT32 address <-> void pointer
void  *IO_UtilityU32AddrToPtr(UINT32 Addr);
UINT32 IO_UtilityPtrToU32Addr(const void *Ptr);
UINT64 IO_UtilityPtrToU64Addr(const void *Ptr);
void  *IO_UtilityU64AddrToPtr(UINT64 Addr);

// register read/write functions
UINT32 IO_UtilityRegRead32(UINT64 Address);
void   IO_UtilityRegWrite32(UINT64 Address, UINT32 Value);
void   IO_UtilityRegMaskWrite32(UINT64 Address, UINT32 Value, UINT32 Mask, UINT32 Shift);
UINT32 IO_UtilityRegMaskRead32(UINT64 Address, UINT32 Mask, UINT32 Shift);

#endif
