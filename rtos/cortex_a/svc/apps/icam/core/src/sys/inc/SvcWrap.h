/**
*  @file SvcWrap.h
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
*  @details wrap functions
*
*/

#ifndef SVC_WRAP_H
#define SVC_WRAP_H

#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaSvcWrap.h"

#define SvcWrap_sprintfU32       AmbaUtility_StringPrintUInt32
#define SvcWrap_sprintfI32       AmbaUtility_StringPrintInt32
#define SvcWrap_sprintfStr       AmbaUtility_StringPrintStr

#define SvcWrap_strlen           AmbaUtility_StringLength
#define SvcWrap_strcpy           AmbaUtility_StringCopy

static inline INT32 SvcWrap_strcmp(const char *pStr1, const char *pStr2)
{
    INT32   Rval = -1;
    UINT32  Len1, Len2;

    if ((pStr1 != NULL) && (pStr2 != NULL)) {
        Len1 = (UINT32)AmbaUtility_StringLength(pStr1);
        Len2 = (UINT32)AmbaUtility_StringLength(pStr2);

        if (Len2 < Len1) {
            Rval = 1;
        } else if (Len1 < Len2) {
            Rval = -1;
        } else {
            Rval = AmbaUtility_StringCompare(pStr1, pStr2, Len2);
        }
    }

    return Rval;
}

#define SvcWrap_strtok           AmbaUtility_StringToken
#define SvcWrap_strtoul          AmbaUtility_StringToUInt32
#define SvcWrap_strtoull         AmbaUtility_StringToUInt64
char   *SvcWrap_strstr(const char *pString1, const char *pString2);
char   *SvcWrap_strrchr(char *pString1, INT32 c);
UINT32  SvcWrap_strspn(const char *pStr, const char *pCharSet);
UINT32  SvcWrap_strcspn(const char * pStr, const char * pKeys);
DOUBLE  SvcWrap_strtod(const char *pStr);
UINT32  SvcWrap_strtol(const char *pStr, INT32 *pValue);

typedef struct {
    UINT64 Uint64;
    DOUBLE Doubld;
    char   Char;
    const void *pPointer;
    const char *pCStr;
} SVC_WRAP_ARGV_s;

#define SVC_WRAP_MAX_ARGC (32U)

typedef void (*SVC_WRAP_PRINT_f)(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2);

typedef struct {
    const char            *pStrFmt;
    UINT32                 Argc;
    SVC_WRAP_ARGV_s        Argv[SVC_WRAP_MAX_ARGC];
    SVC_WRAP_PRINT_f       pProc;
} SVC_WRAP_PRINT_s;

void SvcWrap_Print(const char *pModule, SVC_WRAP_PRINT_s *pPrint);
void SvcWrap_PrintUL(const char *pFormat, ULONG Arg1, ULONG Arg2, ULONG Arg3, ULONG Arg4, ULONG Arg5);

#define SVC_WRAP_PRINT { const char *pPrnModuleStr; SVC_WRAP_PRINT_s SvcWrapPrint; if (0U == AmbaWrap_memset(&SvcWrapPrint, 0, sizeof(SvcWrapPrint))) { SvcWrapPrint.pStrFmt =
#define SVC_PRN_ARG_S      ; SvcWrapPrint.Argc --; pPrnModuleStr =
#define SVC_PRN_ARG_PROC   ; SvcWrapPrint.pProc =
#define SVC_PRN_ARG_UINT64 ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].Uint64   =
#define SVC_PRN_ARG_DOUBLE ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].Doubld   =
#define SVC_PRN_ARG_INT16  ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].Uint64   = (UINT64)((UINT16)(
#define SVC_PRN_ARG_INT32  ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].Uint64   = (UINT64)((UINT32)(
#define SVC_PRN_ARG_UINT8  ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].Uint64   = (UINT64)((
#define SVC_PRN_ARG_UINT16 ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].Uint64   = (UINT64)((
#define SVC_PRN_ARG_UINT32 ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].Uint64   = (UINT64)((
#define SVC_PRN_ARG_CHAR   ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].Char     = ((
#define SVC_PRN_ARG_CSTR   ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].pCStr    = ((
#define SVC_PRN_ARG_CPOINT ; SvcWrapPrint.Argc ++; SvcWrapPrint.Argv[SvcWrapPrint.Argc].pPointer = ((
#define SVC_PRN_ARG_POST   ))
#define SVC_PRN_ARG_E      ; SvcWrapPrint.Argc ++; SvcWrap_Print(pPrnModuleStr, &SvcWrapPrint); }}

typedef struct {
    const char            *pStrFmt;
    UINT32                 Argc;
    SVC_WRAP_ARGV_s        Argv[SVC_WRAP_MAX_ARGC];
} SVC_WRAP_SNPRINT_s;

UINT32 SvcWrap_snprintf(char *pBuf, UINT32 BufSize, SVC_WRAP_SNPRINT_s *pFmt);

#define SVC_WRAP_SNPRINT { UINT32 SnPrnVal; char *pPrnBufStr; UINT32 BufSize; UINT32 *pRlen; SVC_WRAP_SNPRINT_s SvcWrapSnPrint; SnPrnVal = AmbaWrap_memset(&SvcWrapSnPrint, 0, sizeof(SvcWrapSnPrint)); if (0U == SnPrnVal) { AmbaMisra_TouchUnused(&SnPrnVal); } SvcWrapSnPrint.pStrFmt =
#define SVC_SNPRN_ARG_S      ; SvcWrapSnPrint.Argc --; pPrnBufStr =
#define SVC_SNPRN_ARG_BSIZE  ; BufSize =
#define SVC_SNPRN_ARG_RLEN   ; pRlen =
#define SVC_SNPRN_ARG_UINT64 ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].Uint64 =
#define SVC_SNPRN_ARG_DOUBLE ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].Doubld =
#define SVC_SNPRN_ARG_INT16  ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].Uint64   = (UINT64)((UINT16)(
#define SVC_SNPRN_ARG_INT32  ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].Uint64   = (UINT64)((UINT32)(
#define SVC_SNPRN_ARG_UINT8  ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].Uint64   = (UINT64)((
#define SVC_SNPRN_ARG_UINT16 ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].Uint64   = (UINT64)((
#define SVC_SNPRN_ARG_UINT32 ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].Uint64   = (UINT64)((
#define SVC_SNPRN_ARG_CHAR   ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].Char     = ((
#define SVC_SNPRN_ARG_CSTR   ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].pCStr    = ((
#define SVC_SNPRN_ARG_CPOINT ; SvcWrapSnPrint.Argc ++; SvcWrapSnPrint.Argv[SvcWrapSnPrint.Argc].pPointer = ((
#define SVC_SNPRN_ARG_POST   ))
#define SVC_SNPRN_ARG_E      ; SvcWrapSnPrint.Argc ++; *pRlen = SvcWrap_snprintf(pPrnBufStr, BufSize, &SvcWrapSnPrint); }

#endif  /* SVC_WRAP_H */
