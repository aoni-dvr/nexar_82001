/**
 *  @file AmbaCT_SystemApi.c
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
#include "AmbaCT_SystemApi.h"

static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

UINT32 CT_U64_to_U32(UINT64 Number)
{
    UINT32 Rval = 0U;
    UINT32 SizeU32 = 0U;
    Rval |= AmbaWrap_memcpy(&SizeU32, &Number, sizeof(UINT32));
    if (Rval != 0U) {
        SizeU32 = 0U;
    }
    return SizeU32;
}

UINT32 CT_sizeT_to_U32(SIZE_t Size)
{
    UINT32 Rval = 0U;
    UINT32 SizeU32 = 0U;
    Rval |= AmbaWrap_memcpy(&SizeU32, &Size, sizeof(UINT32));
    if (Rval != 0U) {
        SizeU32 = 1U;
    }
    return SizeU32;
}

INT32 CT_strcmp(const char *pString1, const char *pString2) {
    INT32 value=0;
    UINT32 str_len_s1, str_len_s2;

    str_len_s1 = CT_sizeT_to_U32(AmbaUtility_StringLength(pString1));
    str_len_s2 = CT_sizeT_to_U32(AmbaUtility_StringLength(pString2));

    if (pString1==NULL) {
        value = 1;
    } else if (pString2==NULL) {
        value = -1;
    } else {
        str_len_s1 = CT_sizeT_to_U32(AmbaUtility_StringLength(pString1));
        str_len_s2 = CT_sizeT_to_U32(AmbaUtility_StringLength(pString2));
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

INT32 CT_strncmp(const char *pString1, const char *pString2, INT32 Size) {
    INT32 value;
    value = (INT32)AmbaUtility_StringCompare(pString1, pString2, (SIZE_t)Size);
    return value;
}

UINT32 CT_strspn(const char *s1, const char *s2)
{
    UINT32 i, J;

    for (i = 0U; s1[i] != '\0'; i++) {
        for (J = 0U; s2[J] != '\0'; J++) {
            if (s2[J] == s1[i]) {
                break;
            }
        }
        if (s2[J] == '\0') {
            break;
        }
    }
    return i;
}

UINT32 CT_strcspn( const char * pStr, const char * pKeys )
{
    UINT32 Position1st = CT_sizeT_to_U32(AmbaUtility_StringLength(pStr));

    if ((pStr != NULL) && (pKeys != NULL)) {
        UINT32 StrIdx, StrLen = Position1st;
        UINT32 KeyIdx, KeyLen = CT_sizeT_to_U32(AmbaUtility_StringLength(pKeys));

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

UINT32 CT_strlen(const char *s) {
    UINT32 value;
    value = CT_sizeT_to_U32(AmbaUtility_StringLength(s));
    return value;
}

char* CT_strtok_r(char *str, const char *delim, char **nextp) {
    char *ret, *strTemp;
    strTemp = str;

    if (strTemp == NULL)
    {
        strTemp = *nextp;
    }

    strTemp = &strTemp[CT_strspn(strTemp, delim)];

    if (*strTemp == '\0')
    {
        ret = NULL;
    } else {
        ret = strTemp;

        strTemp = &strTemp[CT_strcspn(strTemp, delim)];

        if (*strTemp != '\0')
        {
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

static inline UINT32 CT_GetSign(const char *nptr,INT32 *pNeg, INT32 *pC_tmp, INT32 *pMisraBase)
{
    register const char *s = nptr;
    register INT32 c;
    UINT32 IdxS = 0;

    do {
        c = (INT32)((UINT8)s[IdxS]);
        IdxS++;
    } while (tuner_isspace((char)c) == 1U);

    if (c == (INT32)'-') {
        *pNeg = 1;
        c = (INT32)((UINT8)s[IdxS]);
        IdxS++;
    } else if (c == (INT32)'+') {
        c = (INT32)((UINT8)s[IdxS]);
        IdxS++;
    } else {
        // Do nothing
    }

    if (((*pMisraBase == 0) || (*pMisraBase == 16)) && (c == (INT32)'0') && ((s[IdxS] == 'x') || (s[IdxS] == 'X'))) {
        c = (INT32)((UINT8)s[IdxS + 1U]);

        IdxS += 2U;
        *pMisraBase = 16;
    }
    *pC_tmp = c;
    return IdxS;
}

#define TUNER_LONG_MAX    (0x7FFFFFFFU)
#define TUNER_LONG_MIN    (0x80000000U)
INT64 CT_strtol(const char *nptr, char **endptr, INT32 base)
{
    register const char *s = nptr;
    UINT32 IdxS = 0;
    register LONG acc;
    register INT32 c;
    register LONG cutoff;
    register INT32 neg = 0, any, cutlim;
    INT32 Neg_tmp = 0;
    INT32 C_tmp;
    UINT32 MisraCBreakFlag = 0U;
    ULONG MisraULTmp;
    LONG MisraLTmp;
    INT32 MisraBase = base;

    IdxS = CT_GetSign(nptr, &Neg_tmp, &C_tmp, &MisraBase);
    neg = Neg_tmp;
    c = C_tmp;
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
    cutlim = (INT32)(cutoff % (LONG)MisraBase);
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
                c = (INT32)((UINT8)s[IdxS]);
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
            CT_CheckRval(AmbaWrap_memcpy(endptr, &s[IdxS - 1U], sizeof(char *)), "AmbaWrap_memcpy", __func__);
        } else {
            CT_CheckRval(AmbaWrap_memcpy(endptr, nptr, sizeof(char *)), "AmbaWrap_memcpy", __func__);
        }

    }
    return (INT64)(acc);
}

UINT64 CT_strtoul(const char *s, char * const *endptr, INT32 base) {
    UINT64 value = 0U;
    if ((endptr==NULL) || (base==0)) {
        // misraC
    }
    (void)AmbaUtility_StringToUInt64(s, &value);
    return value;
}

char* CT_strstr(const char *s1, const char *s2) {
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
            CT_CheckRval(AmbaWrap_memcpy(&p_value, &p_misra_char, sizeof(char *)), "AmbaWrap_memcpy", __func__);
        }
    }

    return p_value;
}

char* CT_strrchr(char *pString1, INT32 c)
{
    char *pStr = NULL;
    INT32 S32CvtChar = c;

    if (pString1 != NULL) {
        UINT32 CmpIdx = CT_sizeT_to_U32(AmbaUtility_StringLength(pString1));
        char   CmpKey;
        CT_CheckRval(AmbaWrap_memcpy(&CmpKey, &S32CvtChar, sizeof(CmpKey)), "AmbaWrap_memcpy", __func__);
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
            // Note # CmpIdx = 0U
            if (pString1[CmpIdx] == CmpKey) {
                pStr = &(pString1[0]);
            }

        }
    }

    return pStr;
}

void CT_strcpy(char *pDest, const char *pSource) {
    AmbaUtility_StringCopy(pDest, AmbaUtility_StringLength(pSource)+1U, pSource);
}


void CT_strncpy(char *pDest, const char *pSource, INT32 DestSize) {
    AmbaUtility_StringCopy(pDest, (SIZE_t)DestSize, pSource);
}

AMBA_FS_FILE* CT_fopen(const char *FileName, const char *Mode)
{
    AMBA_FS_FILE *Rval = NULL;

    (void)AmbaFS_FileOpen(FileName, Mode, &Rval);

    return Rval;
}

UINT32 CT_fread(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile)
{
    UINT32 Rval;

    (void)AmbaFS_FileRead(pBuf, Size, Count, pFile, &Rval);

    return Rval;
}

UINT32 CT_fwrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile)
{
    UINT32 Rval;

    (void)AmbaFS_FileWrite(pBuf, Size, Count, pFile, &Rval);

    return Rval;
}

void CT_strcat(char *pBuffer, const char *pSource, UINT32 BufferSize)
{
    AmbaUtility_StringAppend(pBuffer, BufferSize, pSource);
}

UINT32 CT_uint32_to_string(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    UINT32 Rval;
    Rval = AmbaUtility_UInt32ToStr(pBuffer, BufferSize, Value, Radix);
    return Rval;
}

UINT32 CT_int32_to_string(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix)
{
    UINT32 Rval;
    Rval = AmbaUtility_Int32ToStr(pBuffer, BufferSize, Value, Radix);
    return Rval;
}

UINT32 CT_double_to_string(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint)
{
    UINT32 Rval;
    Rval = AmbaUtility_DoubleToStr(pBuffer, BufferSize, Value, Afterpoint);
    return Rval;
}

static inline UINT32 CT_isdigit(char c)
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

static inline DOUBLE CT_U8ToDouble(UINT8 Number)
{
    return (DOUBLE)Number;
}

static inline INT32 CT_U8ToS32(UINT8 Number)
{
    return (INT32)Number;
}
DOUBLE CT_atof(const char *s)
{
    DOUBLE a = 0.0;
    INT32 e = 0;
    char c;
    DOUBLE positive = 1.0;
    INT32 sign = 1;
    INT32 i = 0;
    const char *pS = s;
    if(*pS == '-'){
        positive = -1.0;
        pS++;
    } else if (*pS == '+') {
        pS++;
    } else {
        // Do nothing
    }
    c = *pS;
    pS++;
    while ((c != '\0') && (CT_isdigit(c) != 0U)) {
        a = (a*10.0) + CT_U8ToDouble((UINT8)c - (UINT8)'0');
        c = *pS;
        pS++;
    }
    if (c == '.') {
        c = *pS;
        pS++;
        while ((c != '\0') && (CT_isdigit(c) != 0U)) {
            a = (a*10.0) + CT_U8ToDouble((UINT8)c - (UINT8)'0');
            e = e-1;
            c = *pS;
            pS++;
        }
    }
    if ((c == 'e') || (c == 'E')) {
        c = *pS;
        pS++;
        if (c == '+') {
            c = *pS;
            pS++;
        } else if (c == '-') {
            c = *pS;
            pS++;
            sign = -1;
        } else {
            // Do nothing
        }
        while (CT_isdigit(c) != 0U) {
            i = (i * 10) + CT_U8ToS32((UINT8)c - (UINT8)'0');
            c = *pS;
            pS++;
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











