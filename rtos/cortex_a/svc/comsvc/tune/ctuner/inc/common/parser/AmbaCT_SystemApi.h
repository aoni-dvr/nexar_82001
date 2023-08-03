/**
 *  @file AmbaCT_SystemApi.h
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
#ifndef AMBA_CT_SYSTEM_API_H
#define AMBA_CT_SYSTEM_API_H
#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaFS.h"

#define TUNE_OK             (0UL)
#define TUNE_ERROR_GENERAL  (TUNE_ERR_BASE)

UINT32 CT_U64_to_U32(UINT64 Number);
UINT32 CT_sizeT_to_U32(SIZE_t Size);
INT32 CT_strcmp(const char *pString1, const char *pString2);
INT32 CT_strncmp(const char *pString1, const char *pString2, INT32 Size);
UINT32 CT_strspn(const char *s1, const char *s2);
UINT32 CT_strcspn( const char * pStr, const char * pKeys );
UINT32 CT_strlen(const char *s);
char* CT_strtok_r(char *str, const char *delim, char **nextp);
INT64 CT_strtol(const char *nptr, char **endptr, INT32 base);
UINT64 CT_strtoul(const char *s, char * const *endptr, INT32 base);
char* CT_strstr(const char *s1, const char *s2);
char* CT_strrchr(char *pString1, INT32 c);
void CT_strcpy(char *pDest, const char *pSource);
void CT_strncpy(char *pDest, const char *pSource, INT32 DestSize);
AMBA_FS_FILE* CT_fopen(const char *FileName, const char *Mode);
UINT32 CT_fread(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile);
UINT32 CT_fwrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile);
DOUBLE CT_atof(const char *s);
void CT_strcat(char *pBuffer, const char *pSource, UINT32 BufferSize);
UINT32 CT_uint32_to_string(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
UINT32 CT_int32_to_string(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix);
UINT32 CT_double_to_string(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint);

#define CT_memset AmbaWrap_memset
#define CT_memcpy AmbaWrap_memcpy
#define CT_fclose AmbaFS_FileClose

#define DC_I    99999L
#define DC_U    99999UL
#define DC_U64  99999ULL
#define DC_S    NULL

#endif
