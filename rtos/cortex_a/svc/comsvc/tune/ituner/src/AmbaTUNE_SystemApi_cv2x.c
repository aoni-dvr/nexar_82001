/*
*  @file AmbaTUNE_SystemApi_cv2x.c
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

#include "AmbaTUNE_SystemApi_cv2x.h"

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))

// TBD

#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))
INT32 tuner_strcmp(const char *pString1, const char *pString2)
{
    INT32 value=0;
    UINT32 str_len_s1, str_len_s2;

    str_len_s1 = AmbaUtility_StringLength(pString1);
    str_len_s2 = AmbaUtility_StringLength(pString2);

    if (pString1==NULL) {
        value = 1;
    } else if (pString2==NULL) {
        value = -1;
    } else {
        str_len_s1 = AmbaUtility_StringLength(pString1);
        str_len_s2 = AmbaUtility_StringLength(pString2);
        if (str_len_s1==str_len_s2) {
            value = (INT32)AmbaUtility_StringCompare(pString1, pString2, (SIZE_t)str_len_s1);
        } else if (str_len_s1>str_len_s2) {
            value = 1;
        } else if (str_len_s1<str_len_s2) {
            value = -1;
        } else {
            // misraC
        }
    }

    return value;
}

INT32 tuner_u8ncmp(const UINT8 *pU8_01, const UINT8 *pU8_02, UINT32 size)
{
    INT32 value = 0;
    UINT32 i;

    if (pU8_01==NULL) {
        value = 1;
    } else if (pU8_02==NULL) {
        value = -1;
    } else {
        for (i=0UL; i<size; i++) {
            if (pU8_01[i]==pU8_02[i]) {

            } else {
                value = -1;
                break;
            }
        }
    }

    return value;
}

INT32 tuner_i8ncmp(const INT8 *pU8_01, const INT8 *pU8_02, UINT32 size)
{
    INT32 value = 0;
    UINT32 i;

    if (pU8_01==NULL) {
        value = 1;
    } else if (pU8_02==NULL) {
        value = -1;
    } else {
        for (i=0UL; i<size; i++) {
            if (pU8_01[i]==pU8_02[i]) {

            } else {
                value = -1;
                break;
            }
        }
    }

    return value;
}


INT32 tuner_strncmp(const char *pString1, const char *pString2, INT32 Size)
{
    INT32 value;
    value = (INT32)AmbaUtility_StringCompare(pString1, pString2, (SIZE_t)Size);
    return value;
}

UINT32 tuner_strspn(const char *s1, const char *s2)
{
    UINT32 I, J;

    for (I = 0U; s1[I] != '\0'; I++) {
        for (J = 0U; s2[J] != '\0'; J++) {
            if (s2[J] == s1[I]) {
                break;
            }
        }
        if (s2[J] == '\0') {
            break;
        }
    }
    return I;
}

UINT32 tuner_strcspn( const char * pStr, const char * pKeys )
{
    UINT32 Position1st = AmbaUtility_StringLength(pStr);

    if ((pStr != NULL) && (pKeys != NULL)) {
        UINT32 StrIdx, StrLen = Position1st;
        UINT32 KeyIdx, KeyLen = AmbaUtility_StringLength(pKeys);

        for (StrIdx = 0U; StrIdx < StrLen; StrIdx ++) {
            for (KeyIdx = 0U; KeyIdx < KeyLen; KeyIdx ++) {
                if (pStr[StrIdx] == pKeys[KeyIdx]) {
                    break;
                }
            }

            if (pStr[StrIdx] == pKeys[KeyIdx]) {
                if (Position1st > StrIdx) {
                    Position1st = StrIdx;
                    break;
                }
            }
        }
    }

    return Position1st;
}

UINT32 tuner_strlen(const char *s)
{
    UINT32 value;
    value = AmbaUtility_StringLength(s);
    return value;
}

char* tuner_strtok_r(char *str, const char *delim, char **nextp)
{
    char *ret, *strTemp;
    strTemp = str;

    if (strTemp == NULL) {
        strTemp = *nextp;
    }

    strTemp = &strTemp[tuner_strspn(strTemp, delim)];

    if (*strTemp == '\0') {
        ret = NULL;
    } else {
        ret = strTemp;

        strTemp = &strTemp[tuner_strcspn(strTemp, delim)];

        if (*strTemp != '\0') {
            //*strTemp++ = '\0';
            *strTemp = '\0';
            strTemp = &strTemp[1];
        }
        *nextp = strTemp;
    }
    return ret;
}
static UINT32 tuner_isspace(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if (((cval >= 9U) && (cval <= 13U)) || (cval == 32U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}
static UINT32 tuner_isdigit(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if ((cval >= 48U) && (cval <= 57U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

static UINT32 tuenr_isalpha(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if (((cval >= 65U) && (cval <= 90U)) || ((cval >= 97U) && (cval <= 122U))) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

static UINT32 tuner_isupper(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if ((cval >= 65U) && (cval <= 90U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}


#define TUNER_LONG_MAX    (0x7FFFFFFFU)
#define TUNER_LONG_MIN    (0x80000000U)
INT64 tuner_strtol(const char *nptr, char **endptr, INT32 base)
{
    register const char *s = nptr;
    UINT32 IdxS = 0;
    register LONG acc;
    register INT32 c;
    register LONG cutoff;
    register INT32 neg = 0, any, cutlim;
    UINT32 MisraCBreakFlag = 0U;
    ULONG MisraULTmp;
    LONG MisraLTmp;
    INT32 MisraBase = base;
    do {
        c = (INT32) s[IdxS];
        IdxS++;
    } while (tuner_isspace((char)c) == 1U);
    if (c == (INT32)'-') {
        neg = 1;
        c = (INT32)s[IdxS];
        IdxS++;
    } else if (c == (INT32)'+') {
        c = (INT32)s[IdxS];
        IdxS++;
    } else {
        // Do nothing
    }
    if (((MisraBase == 0) || (MisraBase == 16)) && (c == (INT32)'0') && ((s[IdxS] == 'x') || (s[IdxS] == 'X'))) {
        c = (INT32)s[IdxS + 1U];

        IdxS += 2U;
        MisraBase = 16;
    }

    if (MisraBase == 0) {
        if (c == (INT32)'0') {
            MisraBase = 8;
        } else {
            MisraBase = 10;
        }
    }
    if (neg != 0) {
        MisraLTmp = (LONG)TUNER_LONG_MIN;
        cutoff = -MisraLTmp;
    } else {
        cutoff = (LONG)TUNER_LONG_MAX;
    }
    cutlim = cutoff % (LONG)MisraBase;
    MisraULTmp = (ULONG)cutoff / (ULONG)MisraBase;
    cutoff = (LONG)MisraULTmp;
    acc = 0;
    any = 0;
    for(;;) {
        if (tuner_isdigit((char)c) != 0U) {
            c = c - (INT32)'0';
        } else if (tuenr_isalpha((char)c) != 0U) {
            if (tuner_isupper((char)c) != 0U) {
                c = c - (INT32)'A' + 10;
            } else {
                c = c - (INT32)'a' + 10;
            }
        } else {
            MisraCBreakFlag = 1U;
        }
        if (MisraCBreakFlag == 0U) {
            if (c >= MisraBase) {
                MisraCBreakFlag = 1U;
            } else {
                if ((any < 0) || (acc > cutoff) || ((acc == cutoff) && (c > cutlim))) {
                    any = -1;
                } else {
                    any = 1;
                    acc *= (LONG)MisraBase;
                    acc += (LONG)c;
                }
                c = (INT32)s[IdxS];
                IdxS++;
            }
        }
        if (MisraCBreakFlag != 0U) {
            break;
        }
    }
    if (any < 0) {
        if (neg != 0) {
            acc = (LONG)TUNER_LONG_MIN;
        } else {
            acc = (LONG)TUNER_LONG_MAX;
        }
    } else if (neg != 0) {
        acc = -acc;
    } else {
        // Do nothing
    }
    if (endptr != NULL) {
        if (any == 0) {
            ituner_memcpy(endptr, &s[IdxS - 1U], sizeof(char *));
        } else {
            ituner_memcpy(endptr, nptr, sizeof(char *));
        }

    }
    return (INT64)(acc);
}

UINT64 tuner_strtoul(const char *s, char * const *endptr, INT32 base)
{
    UINT64 value;
    if ((endptr==NULL) || (base==0)) {
        // misraC
    }
    (void)AmbaUtility_StringToUInt64(s, &value);
    return value;
}

char* tuner_strstr(const char *s1, const char *s2)
{
    char* p_value = NULL;

    if ((s1 != NULL) && (s2 != NULL)) {
        UINT32 CmpIdx = 0U, IsHit = 0U;
        UINT32 Str1Idx = 0U, Str2Idx = 0U;

        while ( s1[CmpIdx] != '\0' ) {
            Str1Idx = CmpIdx;
            Str2Idx = 0;

            while ( (s1[Str1Idx] != '\0' ) &&
                    (s2[Str2Idx] != '\0' ) &&
                    (s1[Str1Idx] == s2[Str2Idx]) ) {
                Str1Idx += 1U;
                Str2Idx += 1U;
                IsHit = 1U;
            }

            if ( s2[Str2Idx] != '\0' ) {
                CmpIdx += 1U;
                IsHit = 0U;
            } else {
                break;
            }
        }

        if (IsHit == 1U) {
            const char *p_misra_char = &(s1[CmpIdx]);
            ituner_memcpy(&p_value, &p_misra_char, sizeof(char *));
        }
    }

    return p_value;
}

char* tuner_strrchr(char *pString1, INT32 c)
{
    char *pStr = NULL;
    struct {
        union {
            INT32 Digital;
            char CharVal;
        } Cvt;
    } Int32ToChar;

    Int32ToChar.Cvt.Digital = c;

    if (pString1 != NULL) {
        UINT32 CmpIdx = AmbaUtility_StringLength(pString1);
        char   CmpKey = Int32ToChar.Cvt.CharVal;

        while ( CmpIdx > 0U ) {
            if (pString1[CmpIdx] != CmpKey) {
                CmpIdx -= 1U;
            } else {
                break;
            }
        }

        if (CmpIdx > 0U) {
            pStr = &(pString1[CmpIdx]);
        } else {
            if (CmpIdx == 0U) {
                if (pString1[CmpIdx] == CmpKey) {
                    pStr = &(pString1[0]);
                }
            }
        }
    }

    return pStr;
}

void tuner_strcpy(char *pDest, const char *pSource)
{
    AmbaUtility_StringCopy(pDest, AmbaUtility_StringLength(pSource)+1U, pSource);
}


void tuner_strncpy(char *pDest, const char *pSource, INT32 DestSize)
{
    AmbaUtility_StringCopy(pDest, (SIZE_t)DestSize, pSource);
}

AMBA_FS_FILE* tuner_fopen(const char *FileName, const char *Mode)
{
    AMBA_FS_FILE *Rval = NULL;

    (void)AmbaFS_FileOpen(FileName, Mode, &Rval);

    return Rval;
}

UINT32 tuner_fread(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile)
{
    UINT32 Rval;

    (void)AmbaFS_FileRead(pBuf, Size, Count, pFile, &Rval);

    return Rval;
}

UINT32 tuner_fwrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile)
{
    UINT32 Rval;

    (void)AmbaFS_FileWrite(pBuf, Size, Count, pFile, &Rval);

    return Rval;
}

void tuner_strncat(char *pBuffer, const char *pSource, UINT32 BufferSize)
{
    AmbaUtility_StringAppend(pBuffer, BufferSize, pSource);
}

UINT32 tuner_uint32_to_string(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    UINT32 Rval;
    Rval = AmbaUtility_UInt32ToStr(pBuffer, BufferSize, Value, Radix);
    return Rval;
}

UINT32 tuner_int32_to_string(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix)
{
    UINT32 Rval;
    Rval = AmbaUtility_Int32ToStr(pBuffer, BufferSize, Value, Radix);
    return Rval;
}

UINT32 tuner_double_to_string(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint)
{
    UINT32 Rval;
    Rval = AmbaUtility_DoubleToStr(pBuffer, BufferSize, Value, Afterpoint);
    return Rval;
}

static inline UINT32 is_digit(char c)
{
    static const UINT8 Zero = (UINT8)'0';
    static const UINT8 Nine = (UINT8)'9';
    UINT32 Rval;
    if (((UINT8)c >= Zero) && ((UINT8)c <= Nine)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

static inline DOUBLE U8_To_Double(UINT8 Number)
{
    return (DOUBLE)Number;
}

static inline INT32 U8_To_S32(UINT8 Number)
{
    return (INT32)Number;
}
DOUBLE tuner_atof(const char *s)
{
    DOUBLE a = 0.0;
    INT32 e = 0;
    char c;
    DOUBLE positive = 1.0;
    INT32 sign = 1;
    INT32 i = 0;

    if(*s == '-') {
        positive = -1.0;
        s++;
    } else if (*s == '+') {
        s++;
    } else {
        // Do nothing
    }
    c = *s;
    s++;
    while ((c != '\0') && (is_digit(c) != 0U)) {
        a = (a*10.0) + U8_To_Double((UINT8)c - (UINT8)'0');
        c = *s;
        s++;
    }
    if (c == '.') {
        c = *s;
        s++;
        while ((c != '\0') && (is_digit(c) != 0U)) {
            a = (a*10.0) + U8_To_Double((UINT8)c - (UINT8)'0');
            e = e-1;
            c = *s;
            s++;
        }
    }
    if ((c == 'e') || (c == 'E')) {
        c = *s;
        s++;
        if (c == '+') {
            c = *s;
            s++;
        } else if (c == '-') {
            c = *s;
            s++;
            sign = -1;
        } else {
            // Do nothing
        }
        while (is_digit(c) != 0U) {
            i = (i * 10) + U8_To_S32((UINT8)c - (UINT8)'0');
            c = *s;
            s++;
        }
        e += (i*sign);
    }
    while (e > 0) {
        a *= 10.0;
        e--;
    }
    while (e < 0) {
        a *= 0.1;
        e++;
    }
    a *= positive;

    return a;
}

void tuner_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    UINT32 itn_error_base_u16 = TUNE_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintUInt5((UINT16)itn_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void tuner_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    UINT32 itn_error_base_u16 = TUNE_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintInt5((UINT16)itn_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void tuner_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    UINT32 itn_error_base_u16 = TUNE_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintStr5((UINT16)itn_error_base_u16, pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
}

//coverity rule 4.7 refine.
void ituner_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    if(AmbaWrap_memcpy(pDst, pSrc, num) == ITN_OK) {;}
}

//coverity rule 4.7 refine.
void ituner_memset(void *ptr, INT32 v, SIZE_t n)
{
    if(AmbaWrap_memset(ptr, v, n) == ITN_OK) {;}
}

#else

INT32 tuner_strcmp(const char *pString1, const char *pString2)
{
    INT32 value=0;
    UINT32 str_len_s1, str_len_s2;

    str_len_s1 = AmbaUtility_StringLength(pString1);
    str_len_s2 = AmbaUtility_StringLength(pString2);

    if (pString1==NULL) {
        value = 1;
    } else if (pString2==NULL) {
        value = -1;
    } else {
        str_len_s1 = AmbaUtility_StringLength(pString1);
        str_len_s2 = AmbaUtility_StringLength(pString2);
        if (str_len_s1==str_len_s2) {
            value = (INT32)AmbaUtility_StringCompare(pString1, pString2, (SIZE_t)str_len_s1);
        } else if (str_len_s1>str_len_s2) {
            value = 1;
        } else if (str_len_s1<str_len_s2) {
            value = -1;
        } else {
            // misraC
        }
    }

    return value;
}

INT32 tuner_u8ncmp(const UINT8 *pU8_01, const UINT8 *pU8_02, UINT32 size)
{
    INT32 value = 0;
    UINT32 i;

    if (pU8_01==NULL) {
        value = 1;
    } else if (pU8_02==NULL) {
        value = -1;
    } else {
        for (i=0UL; i<size; i++) {
            if (pU8_01[i]==pU8_02[i]) {

            } else {
                value = -1;
                break;
            }
        }
    }

    return value;
}
#if 0
INT32 tuner_i8ncmp(const INT8 *pU8_01, const INT8 *pU8_02, UINT32 size)
{
    INT32 value = 0;
    UINT32 i;

    if (pU8_01==NULL) {
        value = 1;
    } else if (pU8_02==NULL) {
        value = -1;
    } else {
        for (i=0UL; i<size; i++) {
            if (pU8_01[i]==pU8_02[i]) {

            } else {
                value = -1;
                break;
            }
        }
    }

    return value;
}
#endif

INT32 tuner_strncmp(const char *pString1, const char *pString2, INT32 Size)
{
    INT32 value;
    value = (INT32)AmbaUtility_StringCompare(pString1, pString2, (SIZE_t)Size);
    return value;
}

static UINT32 tuner_strspn(const char *s1, const char *s2)
{
    UINT32 index_I, index_J;

    for (index_I = 0U; s1[index_I] != '\0'; index_I++) {
        for (index_J = 0U; s2[index_J] != '\0'; index_J++) {
            if (s2[index_J] == s1[index_I]) {
                break;
            }
        }
        if (s2[index_J] == '\0') {
            break;
        }
    }
    return index_I;
}

UINT32 tuner_strcspn( const char * pStr, const char * pKeys )
{
    UINT32 Position1st = AmbaUtility_StringLength(pStr);

    if ((pStr != NULL) && (pKeys != NULL)) {
        UINT32 StrIdx, StrLen = Position1st;
        UINT32 KeyIdx, KeyLen = AmbaUtility_StringLength(pKeys);

        for (StrIdx = 0U; StrIdx < StrLen; StrIdx ++) {
            for (KeyIdx = 0U; KeyIdx < KeyLen; KeyIdx ++) {
                if (pStr[StrIdx] == pKeys[KeyIdx]) {
                    break;
                }
            }

            if (pStr[StrIdx] == pKeys[KeyIdx]) {
                if (Position1st > StrIdx) {
                    Position1st = StrIdx;
                    break;
                }
            }
        }
    }

    return Position1st;
}

UINT32 tuner_strlen(const char *s)
{
    UINT32 value;
    value = AmbaUtility_StringLength(s);
    return value;
}

char* tuner_strtok_r(char *str, const char *delim, char **nextp)
{
    char *ret, *strTemp;
    strTemp = str;

    if (strTemp == NULL) {
        strTemp = *nextp;
    }

    strTemp = &strTemp[tuner_strspn(strTemp, delim)];

    if (*strTemp == '\0') {
        ret = NULL;
    } else {
        ret = strTemp;

        strTemp = &strTemp[tuner_strcspn(strTemp, delim)];

        if (*strTemp != '\0') {
            //*strTemp++ = '\0';
            *strTemp = '\0';
            strTemp = &strTemp[1];
        }
        *nextp = strTemp;
    }
    return ret;
}
static UINT32 tuner_isspace(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if (((cval >= 9U) && (cval <= 13U)) || (cval == 32U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}
static UINT32 tuner_isdigit(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if ((cval >= 48U) && (cval <= 57U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

static UINT32 tuenr_isalpha(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if (((cval >= 65U) && (cval <= 90U)) || ((cval >= 97U) && (cval <= 122U))) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

static UINT32 tuner_isupper(char c)
{
    UINT8 cval = (UINT8)c;
    UINT32 Rval;
    if ((cval >= 65U) && (cval <= 90U)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}


#define TUNER_LONG_MAX    (0x7FFFFFFFU)
#define TUNER_LONG_MIN    (0x80000000U)

static inline UINT8 equal_op_char(char compare1, char compare2)
{
    return (compare1==compare2)?1U:0U;
}

static inline UINT8 equal_op_INT32(INT32 compare1, INT32 compare2)
{
    return (compare1==compare2)?1U:0U;
}

static inline UINT8 larger_op_INT32(INT32 compare1, INT32 compare2)
{
    return (compare1>compare2)?1U:0U;
}

static inline UINT8 larger_op_LONG(LONG compare1, LONG compare2)
{
    return (compare1>compare2)?1U:0U;
}

static inline INT32 casting_char_to_int32(char source_char) //For CERT
{
    INT32 dst_int32 = 0;
    ituner_memcpy(&dst_int32, &source_char, sizeof(char));
    return dst_int32;
}

INT64 tuner_strtol(const char *nptr, char **endptr, INT32 base)
{
    register const char *s = nptr;
    UINT32 IdxS = 0;
    register LONG acc;
    register INT32 c;
    register LONG cutoff;
    register INT32 neg = 0, any, cutlim;
    UINT32 MisraCBreakFlag = 0U;
    ULONG MisraULTmp;
    LONG MisraLTmp;
    INT32 MisraBase = base;
    UINT8 bool_result;
    do {
        c = casting_char_to_int32(s[IdxS]);
        IdxS++;
    } while (tuner_isspace((char)c) == 1U);
    if (c == (INT32)'-') {
        neg = 1;
        c = casting_char_to_int32(s[IdxS]);
        IdxS++;
        MisraLTmp = (LONG)TUNER_LONG_MIN;
        cutoff = -MisraLTmp;
    } else if (c == (INT32)'+') {
        c = casting_char_to_int32(s[IdxS]);
        IdxS++;
        cutoff = (LONG)TUNER_LONG_MAX;
    } else {
        // Do nothing
        cutoff = (LONG)TUNER_LONG_MAX;
    }
    //if (((MisraBase == 0) || (MisraBase == 16)) && (c == (INT32)'0') && ((s[IdxS] == 'x') || (s[IdxS] == 'X'))) {
    bool_result = (equal_op_INT32(MisraBase, 0) + equal_op_INT32(MisraBase, 16)) * equal_op_INT32(c, (INT32)'0') * (equal_op_char(s[IdxS], 'x') + equal_op_char(s[IdxS], 'X'));
    if(bool_result!=0U) {
        c = casting_char_to_int32(s[IdxS + 1U]);

        IdxS += 2U;
        MisraBase = 16;
    }

    if (MisraBase == 0) {
        if (c == (INT32)'0') {
            MisraBase = 8;
        } else {
            MisraBase = 10;
        }
    }
    /*
    if (neg != 0) {
        MisraLTmp = (LONG)TUNER_LONG_MIN;
        cutoff = -MisraLTmp;
    } else {
        cutoff = (LONG)TUNER_LONG_MAX;
    }
    */
    cutlim = cutoff % (LONG)MisraBase;
    MisraULTmp = (ULONG)cutoff / (ULONG)MisraBase;
    cutoff = (LONG)MisraULTmp;
    acc = 0;
    any = 0;
    for(;;) {
        if (tuner_isdigit((char)c) != 0U) {
            c = c - (INT32)'0';
        } else if (tuenr_isalpha((char)c) != 0U) {
            if (tuner_isupper((char)c) != 0U) {
                c = c - (INT32)'A' + 10;
            } else {
                c = c - (INT32)'a' + 10;
            }
        } else {
            MisraCBreakFlag = 1U;
        }
        if (MisraCBreakFlag == 0U) {
            if (c >= MisraBase) {
                MisraCBreakFlag = 1U;
            } else {
                //if ((any < 0) || (acc > cutoff) || ((acc == cutoff) && (c > cutlim))) {
                bool_result =  larger_op_LONG(0, any) + larger_op_LONG(acc, cutoff) + (equal_op_INT32(acc, cutoff) * larger_op_INT32(c, cutlim));
                if(bool_result!=0U) {
                    any = -1;
                } else {
                    any = 1;
                    acc *= (LONG)MisraBase;
                    acc += (LONG)c;
                }
                c = casting_char_to_int32(s[IdxS]);
                IdxS++;
            }
        }
        if (MisraCBreakFlag != 0U) {
            break;
        }
    }
    if (any < 0) {
        if (neg != 0) {
            acc = (LONG)TUNER_LONG_MIN;
        } else {
            acc = (LONG)TUNER_LONG_MAX;
        }
    } else if (neg != 0) {
        acc = -acc;
    } else {
        // Do nothing
    }
    if (endptr != NULL) {
        if (any == 0) {
            ituner_memcpy(endptr, &s[IdxS - 1U], sizeof(char *));
        } else {
            ituner_memcpy(endptr, nptr, sizeof(char *));
        }
    }
    return (INT64)(acc);
}

UINT64 tuner_strtoul(const char *s, char * const *endptr, INT32 base)
{
    UINT64 value = 0u;
    if ((endptr==NULL) || (base==0)) {
        // misraC
    }
    (void)AmbaUtility_StringToUInt64(s, &value);
    return value;
}

char* tuner_strstr(const char *s1, const char *s2)
{
    char* p_value = NULL;

    if ((s1 != NULL) && (s2 != NULL)) {
        UINT32 CmpIdx = 0U, IsHit = 0U;
        UINT32 Str1Idx = 0U, Str2Idx = 0U;

        while ( s1[CmpIdx] != '\0' ) {
            Str1Idx = CmpIdx;
            Str2Idx = 0;

            while ( (s1[Str1Idx] != '\0' ) &&
                    (s2[Str2Idx] != '\0' ) &&
                    (s1[Str1Idx] == s2[Str2Idx]) ) {
                Str1Idx += 1U;
                Str2Idx += 1U;
                IsHit = 1U;
            }

            if ( s2[Str2Idx] != '\0' ) {
                CmpIdx += 1U;
                IsHit = 0U;
            } else {
                break;
            }
        }

        if (IsHit == 1U) {
            const char *p_misra_char = &(s1[CmpIdx]);
            ituner_memcpy(&p_value, &p_misra_char, sizeof(char *));
        }
    }

    return p_value;
}

char* tuner_strrchr(char *pString1, INT32 c)
{
    char *pStr = NULL;
    //struct { union { INT32 Digital; char CharVal; } Cvt; } Int32ToChar; // Coverity is not happy about union

    //Int32ToChar.Cvt.Digital = c;

    if (pString1 != NULL) {
        UINT32 CmpIdx = AmbaUtility_StringLength(pString1);
        //char   CmpKey = Int32ToChar.Cvt.CharVal;
        char CmpKey;
        ituner_memcpy(&CmpKey, &c, sizeof(char));

        while ( CmpIdx > 0U ) {
            if (pString1[CmpIdx] != CmpKey) {
                CmpIdx -= 1U;
            } else {
                break;
            }
        }

        if (CmpIdx > 0U) {
            pStr = &(pString1[CmpIdx]);
        } else {
            //if (CmpIdx == 0U) { // Coverity
            if (pString1[CmpIdx] == CmpKey) {
                pStr = &(pString1[0]);
            }
            //}
        }
    }

    return pStr;
}

void tuner_strcpy(char *pDest, const char *pSource)
{
    AmbaUtility_StringCopy(pDest, AmbaUtility_StringLength(pSource)+1U, pSource);
}


void tuner_strncpy(char *pDest, const char *pSource, INT32 DestSize)
{
    AmbaUtility_StringCopy(pDest, (SIZE_t)DestSize, pSource);
}

AMBA_FS_FILE* tuner_fopen(const char *FileName, const char *Mode)
{
    AMBA_FS_FILE *Rval = NULL;

    (void)AmbaFS_FileOpen(FileName, Mode, &Rval);

    return Rval;
}

UINT32 tuner_fread(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile)
{
    UINT32 Rval;

    (void)AmbaFS_FileRead(pBuf, Size, Count, pFile, &Rval);

    return Rval;
}

UINT32 tuner_fwrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile)
{
    UINT32 Rval;

    (void)AmbaFS_FileWrite(pBuf, Size, Count, pFile, &Rval);

    return Rval;
}

void tuner_strncat(char *pBuffer, const char *pSource, UINT32 BufferSize)
{
    AmbaUtility_StringAppend(pBuffer, BufferSize, pSource);
}

UINT32 tuner_uint32_to_string(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    UINT32 Rval;
    Rval = AmbaUtility_UInt32ToStr(pBuffer, BufferSize, Value, Radix);
    return Rval;
}

UINT32 tuner_int32_to_string(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix)
{
    UINT32 Rval;
    Rval = AmbaUtility_Int32ToStr(pBuffer, BufferSize, Value, Radix);
    return Rval;
}

UINT32 tuner_double_to_string(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint)
{
    UINT32 Rval;
    Rval = AmbaUtility_DoubleToStr(pBuffer, BufferSize, Value, Afterpoint);
    return Rval;
}

static inline UINT32 is_digit(char c)
{
    static const UINT8 Zero = (UINT8)'0';
    static const UINT8 Nine = (UINT8)'9';
    UINT32 Rval;
    if (((UINT8)c >= Zero) && ((UINT8)c <= Nine)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

static inline DOUBLE U8_To_Double(UINT8 Number)
{
    return (DOUBLE)Number;
}

static inline INT32 U8_To_S32(UINT8 Number)
{
    return (INT32)Number;
}
DOUBLE tuner_atof(const char *s)
{
    DOUBLE a = 0.0;
    INT32 e = 0;
    char c;
    const char *misra_s;
    DOUBLE positive = 1.0;
    INT32 sign = 1;
    INT32 i = 0;

    ituner_memcpy(&misra_s, &s, sizeof(char*));

    if(*misra_s == '-') {
        positive = -1.0;
        misra_s++;
    } else if (*misra_s == '+') {
        misra_s++;
    } else {
        // Do nothing
    }
    c = *misra_s;
    misra_s++;
    while ((c != '\0') && (is_digit(c) != 0U)) {
        a = (a*10.0) + U8_To_Double((UINT8)c - (UINT8)'0');
        c = *misra_s;
        misra_s++;
    }
    if (c == '.') {
        c = *misra_s;
        misra_s++;
        while ((c != '\0') && (is_digit(c) != 0U)) {
            a = (a*10.0) + U8_To_Double((UINT8)c - (UINT8)'0');
            e = e-1;
            c = *misra_s;
            misra_s++;
        }
    }
    if ((c == 'e') || (c == 'E')) {
        c = *misra_s;
        misra_s++;
        if (c == '+') {
            c = *misra_s;
            misra_s++;
        } else if (c == '-') {
            c = *misra_s;
            misra_s++;
            sign = -1;
        } else {
            // Do nothing
        }
        while (is_digit(c) != 0U) {
            i = (i * 10) + U8_To_S32((UINT8)c - (UINT8)'0');
            c = *misra_s;
            misra_s++;
        }
        e += (i*sign);
    }
    while (e > 0) {
        a *= 10.0;
        e--;
    }
    while (e < 0) {
        a *= 0.1;
        e++;
    }
    a *= positive;

    return a;
}
#if 0
void tuner_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    UINT32 itn_error_base_u16 = TUNE_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintUInt5((UINT16)itn_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void tuner_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    UINT32 itn_error_base_u16 = TUNE_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintInt5((UINT16)itn_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void tuner_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    UINT32 itn_error_base_u16 = TUNE_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintStr5((UINT16)itn_error_base_u16, pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
}
#endif
//coverity rule 4.7 refine.
void ituner_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    if(AmbaWrap_memcpy(pDst, pSrc, num) == ITN_OK) {;}
}

//coverity rule 4.7 refine.
void ituner_memset(void *ptr, INT32 v, SIZE_t n)
{
    if(AmbaWrap_memset(ptr, v, n) == ITN_OK) {;}
}

#endif

