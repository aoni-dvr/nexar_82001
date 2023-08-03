/*
*  @file AmbaTUNE_SystemApi_cv5x.h
*
* Copyright (c) 2020 Ambarella International LP
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
*/

#ifndef AMBA_IMG_SYSTEM_API_CV5X_H
#define AMBA_IMG_SYSTEM_API_CV5X_H

#define ITN_VERSION_MAJOR 8200102U

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))

#include "AmbaTypes.h"
#include "RefWrap.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaFS.h"
#include "string.h"

#define DSP_VIN_GET 0

UINT32 tuner_uint32_to_string(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
UINT32 tuner_int32_to_string(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix);
UINT32 tuner_double_to_string(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint);

#define ituner_print_uint32_5(fmt, arg...) AmbaPrint_PrintUInt5(fmt"\n", ##arg)
#define ituner_print_int32_5(fmt, arg...) AmbaPrint_PrintInt5(fmt"\n", ##arg)
#define ituner_print_str_5(fmt, arg...) AmbaPrint_PrintStr5(fmt"\n", ##arg)
#define ituner_memset memset
#define ituner_memcpy memcpy
#define ituner_strcmp strcmp
#define ituner_strncmp strncmp
#define ituner_strspn strspn
#define ituner_strcspn strcspn
#define ituner_strlen strlen
#define ituner_strtok_r strtok_r
#define ituner_strtol strtol
#define ituner_strtoul strtoul
#define ituner_strstr strstr
#define ituner_strrchr strrchr
#define ituner_strcpy strcpy
#define ituner_strncpy strncpy
#define ituner_atof atof
#define ituner_sprintf sprintf
#define ituner_strncat strncat
#define ituner_fopen fopen
#define ituner_fread fread
#define ituner_fwrite fwrite
#define ituner_fclose fclose
#define AmbaCache_DataInvalidate(...)
#define ituner_u32_str tuner_uint32_to_string
#define ituner_s32_str tuner_int32_to_string
#define ituner_double_str tuner_double_to_string

#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaFS.h"

#define DSP_VIN_GET 0

INT32 tuner_strcmp(const char *pString1, const char *pString2);
INT32 tuner_strncmp(const char *pString1, const char *pString2, INT32 Size);
INT32 tuner_u8ncmp(const UINT8 *pU8_01, const UINT8 *pU8_02, UINT32 size);
INT32 tuner_i8ncmp(const INT8 *pU8_01, const INT8 *pU8_02, UINT32 size);
UINT32 tuner_strspn(const char *s1, const char *s2);
UINT32 tuner_strcspn( const char * pStr, const char * pKeys );
UINT32 tuner_strlen(const char *s);
char* tuner_strtok_r(char *str, const char *delim, char **nextp);
INT64 tuner_strtol(const char *nptr, char **endptr, INT32 base);
UINT64 tuner_strtoul(const char *s, char * const *endptr, INT32 base);
char* tuner_strstr(const char *s1, const char *s2);
char* tuner_strrchr(char *pString1, INT32 c);
void tuner_strcpy(char *pDest, const char *pSource);
void tuner_strncpy(char *pDest, const char *pSource, INT32 DestSize);
AMBA_FS_FILE* tuner_fopen(const char *FileName, const char *Mode);
UINT32 tuner_fread(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile);
UINT32 tuner_fwrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile);
DOUBLE tuner_atof(const char *s);
void tuner_strncat(char *pBuffer, const char *pSource, UINT32 BufferSize);
UINT32 tuner_uint32_to_string(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
UINT32 tuner_int32_to_string(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix);
UINT32 tuner_double_to_string(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint);
void tuner_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void tuner_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void tuner_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);

#define ituner_print_uint32_5 AmbaPrint_PrintUInt5
#define ituner_print_int32_5 AmbaPrint_PrintInt5
#define ituner_print_str_5 AmbaPrint_PrintStr5
#define ituner_print_flush AmbaPrint_Flush
void ituner_memset(void *ptr, INT32 v, SIZE_t n);
void ituner_memcpy(void *pDst, const void *pSrc, SIZE_t num);
#define ituner_fclose AmbaFS_FileClose
#define ituner_strcmp tuner_strcmp
#define ituner_strncmp tuner_strncmp
#define ituner_u8ncmp tuner_u8ncmp
#define ituner_i8ncmp tuner_i8ncmp
#define ituner_strspn tuner_strspn
#define ituner_strcspn tuner_strcspn
#define ituner_strlen tuner_strlen
#define ituner_strtok_r tuner_strtok_r
#define ituner_strtol tuner_strtol
#define ituner_strtoul tuner_strtoul
#define ituner_strstr tuner_strstr
#define ituner_strrchr tuner_strrchr
#define ituner_strcpy tuner_strcpy
#define ituner_strncpy tuner_strncpy
#define ituner_fopen tuner_fopen
#define ituner_fread tuner_fread
#define ituner_fwrite tuner_fwrite
#define ituner_atof tuner_atof
#define ituner_strncat tuner_strncat
#define ituner_u32_str tuner_uint32_to_string
#define ituner_s32_str tuner_int32_to_string
#define ituner_double_str tuner_double_to_string

#else

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaFS.h"

#define DSP_VIN_GET 1

INT32 tuner_strcmp(const char *pString1, const char *pString2);
INT32 tuner_strncmp(const char *pString1, const char *pString2, INT32 Size);
INT32 tuner_u8ncmp(const UINT8 *pU8_01, const UINT8 *pU8_02, UINT32 size);
INT32 tuner_i8ncmp(const INT8 *pU8_01, const INT8 *pU8_02, UINT32 size);
UINT32 tuner_strspn(const char *s1, const char *s2);
UINT32 tuner_strcspn( const char * pStr, const char * pKeys );
UINT32 tuner_strlen(const char *s);
char* tuner_strtok_r(char *str, const char *delim, char **nextp);
INT64 tuner_strtol(const char *nptr, char **endptr, INT32 base);
UINT64 tuner_strtoul(const char *s, char * const *endptr, INT32 base);
char* tuner_strstr(const char *s1, const char *s2);
char* tuner_strrchr(char *pString1, INT32 c);
void tuner_strcpy(char *pDest, const char *pSource);
void tuner_strncpy(char *pDest, const char *pSource, INT32 DestSize);
AMBA_FS_FILE* tuner_fopen(const char *FileName, const char *Mode);
UINT32 tuner_fread(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile);
UINT32 tuner_fwrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile);
DOUBLE tuner_atof(const char *s);
void tuner_strncat(char *pBuffer, const char *pSource, UINT32 BufferSize);
UINT32 tuner_uint32_to_string(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
UINT32 tuner_int32_to_string(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix);
UINT32 tuner_double_to_string(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint);
void tuner_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void tuner_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void tuner_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);

#define ituner_print_uint32_5 AmbaPrint_PrintUInt5
#define ituner_print_int32_5 AmbaPrint_PrintInt5
#define ituner_print_str_5 AmbaPrint_PrintStr5
#define ituner_print_flush AmbaPrint_Flush
void ituner_memset(void *ptr, INT32 v, SIZE_t n);
void ituner_memcpy(void *pDst, const void *pSrc, SIZE_t num);
#define ituner_fclose AmbaFS_FileClose
#define ituner_strcmp tuner_strcmp
#define ituner_strncmp tuner_strncmp
#define ituner_u8ncmp tuner_u8ncmp
#define ituner_i8ncmp tuner_i8ncmp
#define ituner_strspn tuner_strspn
#define ituner_strcspn tuner_strcspn
#define ituner_strlen tuner_strlen
#define ituner_strtok_r tuner_strtok_r
#define ituner_strtol tuner_strtol
#define ituner_strtoul tuner_strtoul
#define ituner_strstr tuner_strstr
#define ituner_strrchr tuner_strrchr
#define ituner_strcpy tuner_strcpy
#define ituner_strncpy tuner_strncpy
#define ituner_fopen tuner_fopen
#define ituner_fread tuner_fread
#define ituner_fwrite tuner_fwrite
#define ituner_atof tuner_atof
#define ituner_strncat tuner_strncat
#define ituner_u32_str tuner_uint32_to_string
#define ituner_s32_str tuner_int32_to_string
#define ituner_double_str tuner_double_to_string

#endif

#define DC_I    ((INT32)99999L)
#define DC_U    99999U
#define DC_U64  99999UL
#define DC_S    NULL

typedef unsigned long UINTPTR;

#endif  //  AMBA_IMG_SYSTEM_API_H

