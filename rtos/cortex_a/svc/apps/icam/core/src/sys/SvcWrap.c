/**
*  @file SvcWrap.c
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

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"

/**
* wrap of strstr function
* @param [in] pString1 string1
* @param [in] pString2 string2
* @return result string
*/
char *SvcWrap_strstr(const char *pString1, const char *pString2)
{
    char *pStr = NULL;

    if ((pString1 != NULL) && (pString2 != NULL)) {
        UINT32 CmpIdx = 0U, IsHit = 0U;
        UINT32 Str1Idx = 0U, Str2Idx = 0U;

        while ( pString1[CmpIdx] != '\0' ) {
            Str1Idx = CmpIdx;
            Str2Idx = 0;

            while ( (pString1[Str1Idx] != '\0' ) &&
                    (pString2[Str2Idx] != '\0' ) &&
                    (pString1[Str1Idx] == pString2[Str2Idx]) ) {
                Str1Idx += 1U;
                Str2Idx += 1U;
                IsHit = 1U;
            }

            if ( pString2[Str2Idx] != '\0' ) {
                CmpIdx += 1U;
                IsHit = 0U;
            } else {
                break;
            }
        }

        if (IsHit == 1U) {
            const char *pCStr = &(pString1[CmpIdx]);
            AmbaMisra_TypeCast(&(pStr), &(pCStr));
        }
    }

    return pStr;
}

/**
* wrap of strrchr function
* @param [in] pString1 string
* @param [in] c character to find
* @return result string
*/
char *SvcWrap_strrchr(char *pString1, INT32 c)
{
    char *pStr = NULL;

    if (pString1 != NULL) {
        UINT32 CmpIdx = (UINT32)AmbaUtility_StringLength(pString1);
        char   CmpKey = (char)c;

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
            if (pString1[CmpIdx] == CmpKey) {
                pStr = &(pString1[0]);
            }
        }
    }

    return pStr;
}

/**
* wrap of strspn function
* @param [in] pStr string
* @param [in] pCharSet character set
* @return count
*/
UINT32 SvcWrap_strspn(const char *pStr, const char *pCharSet)
{
    UINT32 Count = 0U;

    if ((pStr != NULL) && (pCharSet != NULL)) {
        UINT32 StartIdx = 0xFFFFFFFFU, EndIdx = 0U, HitCharSet = 0U;
        UINT32 StrIdx,  StrLen = (UINT32)AmbaUtility_StringLength(pStr);
        UINT32 CharIdx, CharSetLen = (UINT32)AmbaUtility_StringLength(pCharSet);

        if ((StrLen > 0U) && (CharSetLen > 0U)) {
            for (StrIdx = 0U; StrIdx < StrLen; StrIdx ++) {
                HitCharSet = 0U;

                for (CharIdx = 0U; CharIdx < CharSetLen; CharIdx ++ ) {
                    if (pStr[StrIdx] == pCharSet[CharIdx]) {
                        HitCharSet = 1U;
                    }
                }

                if (HitCharSet == 1U) {
                    if (StartIdx == 0xFFFFFFFFU) {
                        StartIdx = StrIdx;
                    } else {
                        EndIdx = StrIdx;
                    }
                } else {
                    if (StartIdx != 0xFFFFFFFFU) {
                        break;
                    }
                }
            }

            if (EndIdx >= StartIdx) {
                Count = EndIdx + 1U - StartIdx;
            }
        }
    }

    return Count;
}

/**
* wrap of strcspn function
* @param [in] pStr string
* @param [in] pKeys key
* @return position
*/
UINT32 SvcWrap_strcspn( const char * pStr, const char * pKeys )
{
    UINT32 Position1st = (UINT32)AmbaUtility_StringLength(pStr);

    if ((pStr != NULL) && (pKeys != NULL)) {
        UINT32 StrIdx, StrLen = Position1st;
        UINT32 KeyIdx, KeyLen = (UINT32)AmbaUtility_StringLength(pKeys);

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

static inline UINT32 SvcWrap_IsDigital(char c)
{
    UINT32 IsDigital = 0U;

    if ((c >= '0') && (c <= '9')) {
        IsDigital = 1U;
    }

    return IsDigital;
}

static inline INT32 SvcWrap_CharToInt32(char c)
{
    UINT32 RetVal = 0;
    AmbaMisra_TypeCast(&(RetVal), &(c)); RetVal &= 0xFFU;
    return (INT32)RetVal;
}

/**
* wrap of strtod function
* @param [in] pStr string
* @return result
*/
DOUBLE SvcWrap_strtod(const char *pStr)
{
    static       INT32 maxExponent = 511;
    static const DOUBLE ExpLtb[] = {
        10.0,
        100.0,
        1.0e4,
        1.0e8,
        1.0e16,
        1.0e32,
        1.0e64,
        1.0e128,
        1.0e256
    };
    static INT32 MaxMantissaNum = 18;
    UINT32 ErrCode = 0U;
    DOUBLE Rval = 0.0;
    const INT32 CHAR_0 = 48;

    if (pStr != NULL) {

        INT32  Idx = 0, StartIdx, DecIdx = 0;
        INT32  IsNagtive = 0, ExpIsNagtive = 0;
        INT32  Mantissa = 0, ExpVal = 0;
        DOUBLE Exp = 0.0;

        // Pass front-space char
        {
            while (pStr[Idx] == ' ') {
                 Idx += 1;
            }
        }

        // Check the sign char
        {
            if (pStr[Idx] == '-') {
                IsNagtive = 1;
                Idx += 1;
            } else {
                if (pStr[Idx] == '+') {
                    Idx += 1;
                } else {
                    if ((0U == SvcWrap_IsDigital(pStr[Idx])) && // not digital
                        (pStr[Idx] == '.')) {                   // not decimal point
                        ErrCode = 1U;
                    }
                }
                IsNagtive = 0;
            }
        }

        // Get the start index
        StartIdx = Idx;

        // Get the MANTISSA and Decimal Point
        if (ErrCode == 0U) {
            DecIdx   = -1;
            Mantissa = 0;

            while (pStr[Idx] != '\0') {

                if (0U == SvcWrap_IsDigital(pStr[Idx])) {

                    if ((pStr[Idx] != '.') ||   // not decimal point. maybe 'e', 'E' or '\0'
                        (DecIdx >= 0)) {        // format : '-.' case
                        break;
                    }

                    DecIdx = Mantissa;
                }

                Mantissa ++;
                Idx ++;
            }

            if (Mantissa <= 0) {
                ErrCode = 1;
            }
        }

        // Get the exponent
        if (ErrCode == 0U) {
            INT32 fracExp;

            if (DecIdx < 0) {
                DecIdx = Mantissa;
            } else {
                Mantissa -= 1;      // reject the decimal point from mantissa part
            }

            // The mantissa part is too larger to ignore others.
            if (Mantissa > MaxMantissaNum) {
                fracExp = DecIdx - MaxMantissaNum;
                Mantissa = MaxMantissaNum;
            } else {
                fracExp = DecIdx - Mantissa;
            }

            // Check 'e' or 'E' char
            if ((pStr[Idx] == 'e') || (pStr[Idx] == 'E')) {
                Idx ++;

                if (pStr[Idx] == '-') {
                    ExpIsNagtive = 1;
                    Idx += 1;
                } else {
                    if (pStr[Idx] == '+') {
                        Idx += 1;
                    } else {
                        if (0U == SvcWrap_IsDigital(pStr[Idx])) {
                            ErrCode = 1U;
                        }
                    }
                    ExpIsNagtive = 0;
                }

                if (ErrCode == 0U) {
                    while (1U == SvcWrap_IsDigital(pStr[Idx])) {
                        ExpVal = ( ExpVal * 10 ) + ( SvcWrap_CharToInt32(pStr[Idx]) - CHAR_0 );
                        Idx ++;
                    }
                }
            }

            if (ErrCode == 0U) {
                const DOUBLE *pTbl;

                if (ExpIsNagtive == 1) {
                    ExpVal = fracExp - ExpVal;
                } else {
                    ExpVal = fracExp + ExpVal;
                }

                if (ExpVal < 0) {
                    ExpIsNagtive = 1;
                    ExpVal = -ExpVal;
                } else {
                    ExpIsNagtive = 0;
                }
                if (ExpVal > maxExponent) {
                    ExpVal = maxExponent;
                }

                pTbl = ExpLtb;
                Exp = 1.0;
                while (ExpVal != 0) {
                    if ( ( ExpVal % 2 ) > 0 ) {
                        Exp *= *(pTbl);
                    }
                    pTbl ++;
                    ExpVal /= 2;
                }
            }
        }

        // Calculate the value by MANTISSA part.
        if (ErrCode == 0U) {
            INT32 Frac1 = 0, Frac2 = 0;

            Idx = StartIdx;

            while (Mantissa > 9) {
                if (pStr[Idx] == '.') {
                    Idx ++;
                }

                Frac1 = ( Frac1 * 10 ) + ( SvcWrap_CharToInt32(pStr[Idx]) - CHAR_0 );

                Idx ++;
                Mantissa --;
            }

            while (Mantissa > 0) {
                if (pStr[Idx] == '.') {
                    Idx ++;
                }

                Frac2 = ( Frac2 * 10 ) + ( SvcWrap_CharToInt32(pStr[Idx]) - CHAR_0 );

                Idx ++;
                Mantissa --;
            }

            Rval = ( 1.0e9 * (DOUBLE)(Frac1) ) + (DOUBLE)(Frac2);
            if (IsNagtive == 1) {
                Rval = -Rval;
            }
        }

        // Calculate the value by exponent
        if (ErrCode == 0U) {
            if (Exp > 0.0) {
                if (ExpIsNagtive == 1) {
                    Rval /= Exp;
                } else {
                    Rval *= Exp;
                }
            }
        }
    }

    return Rval;
}

/**
* wrap of strtol function
* @param [in] pStr string
* @param [out] pValue value
* @return 0-OK, 1-NG
*/
UINT32 SvcWrap_strtol(const char *pStr, INT32 *pValue)
{
    char FirstStr;
    UINT32 ValueU32;

    UINT32 RetVal = SVC_OK;

    if (pStr != NULL) {
        FirstStr = pStr[0];
        if (FirstStr == '-') {
            RetVal = SvcWrap_strtoul(&pStr[1], &ValueU32);
            if (SVC_OK == RetVal) {
                *pValue = (INT32) ValueU32 * -1;
            }
        } else {
            RetVal = SvcWrap_strtoul(&pStr[0], &ValueU32);
            if (SVC_OK == RetVal) {
                *pValue = (INT32) ValueU32;
            }
        }
    } else {
        RetVal = SVC_NG;
    }

    if (SVC_OK != RetVal) {
        *pValue = 0;
    }

    return RetVal;
}

typedef void (*SVC_WRAP_PRN_SPECIFIER_f)(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg);
typedef struct {
    UINT32                Enable;
    char                  Specifier;
    char                  Desciption[64U];
    SVC_WRAP_PRN_SPECIFIER_f pProc;
} SVC_WRAP_PRN_SPECIFIER_s;

static UINT32 SvcWrap_SpecifierPtnSearch(const char *pSpecifierStr, const char *pPtnStr);
static UINT32 SvcWrap_SpecifierPaddingNum(const char *pSpecifier, char *pPaddingChar);
static UINT32 SvcWrap_SpecifierAfterPointNum(const char *pSpecifier);
static void   SvcWrap_SpecifierStringCopy(char *pBuf, UINT32 BufSize, const char *pDigitalBuf, char PaddingChar, UINT32 PaddingNum);

static UINT32 SvcWrap_SpecifierInt16ToDecStr(INT16 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize);
static UINT32 SvcWrap_SpecifierInt32ToDecStr(INT32 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize);
static UINT32 SvcWrap_SpecifierInt64ToDecStr(INT64 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize);
static void   SvcWrap_SpecifierINT(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg);

static UINT32 SvcWrap_SpecifierUInt32ToDecStr(UINT32 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize);
static UINT32 SvcWrap_SpecifierUInt64ToDecStr(UINT64 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize);
static void   SvcWrap_SpecifierUINT(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg);

static void   SvcWrap_SpecifierHexLowerCase(char *pBuf);
static UINT32 SvcWrap_SpecifierUInt32ToHexStr(UINT32 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize);
static UINT32 SvcWrap_SpecifierUInt64ToHexStr(UINT64 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize);
static void   SvcWrap_SpecifierHex(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg);
static UINT32 SvcWrap_SpecifierDoubleToDecStr(DOUBLE InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize);
static void   SvcWrap_SpecifierDouble(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg);
static void   SvcWrap_SpecifierChar(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg);
static void   SvcWrap_SpecifierStr(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg);
static void   SvcWrap_SpecifierPoint(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg);

static UINT32 SvcWrap_AddrCompare(const void *pAddr0, const void *pAddr1);
static void   SvcWrap_SpecifierGetStr(char *pFmt, UINT32 *pPreSpecifierLeng, char **pSpecifierStr, UINT32 *pSpecifierLeng, char **pNextStr, SVC_WRAP_PRN_SPECIFIER_s **pSpecifier);
static void   SvcWrap_SpecifierParsing(const char *pSpecifierStr, const SVC_WRAP_PRN_SPECIFIER_s *pSpecifier, SVC_WRAP_ARGV_s *pArgVector, char *pBuf, UINT32 BufSize);
static UINT32 SvcWrap_ArgvProc(char *pStrBuf, UINT32 StrBufSize, const char *pFmt, UINT32 ArgCount, SVC_WRAP_ARGV_s *pArgVector, UINT32 *pStrLen);

#define SVC_WRAP_PRN_SPECIFIER_NUM     (8U)
static SVC_WRAP_PRN_SPECIFIER_s SvcWrapSpecifier[SVC_WRAP_PRN_SPECIFIER_NUM] = {
    { 1U, 'd', "Signed decimal integer"                            , SvcWrap_SpecifierINT,    },
    { 2U, 'u', "Unsigned decimal integer"                          , SvcWrap_SpecifierUINT,   },
    { 3U, 'x', "Unsigned hexadecimal integer"                      , SvcWrap_SpecifierHex,    },
    { 4U, 'X', "Unsigned hexadecimal integer (uppercase)"          , SvcWrap_SpecifierHex,    },
    { 5U, 'f', "Decimal floating point"                            , SvcWrap_SpecifierDouble, },
    { 6U, 'c', "Character"                                         , SvcWrap_SpecifierChar,   },
    { 7U, 's', "String of characters"                              , SvcWrap_SpecifierStr,    },
    { 8U, 'p', "Pointer address"                                   , SvcWrap_SpecifierPoint,  },
};

static UINT32 SvcWrap_SpecifierPtnSearch(const char *pSpecifierStr, const char *pPtnStr)
{
    UINT32 TypeCompResult = 0U;

    if (pSpecifierStr == NULL) {
        // TBD
    } else if (pPtnStr == NULL) {
        // TBD
    } else {
        UINT8 ExitLoop = 0U;
        char *pSpecifierChar;
        const UINT16 *pSpecifierU16;
        const UINT16 *pPtnU16;

        AmbaMisra_TypeCast(&(pPtnU16), &(pPtnStr));

        AmbaMisra_TypeCast(&(pSpecifierChar), &(pSpecifierStr));

        /* CERT STR30-C */
        AmbaMisra_TouchUnused(pSpecifierChar);

        while (*pSpecifierChar != '\0') {

            if (pPtnStr[1] == '\0') {
                if (*pSpecifierChar == pPtnStr[0]) {
                    TypeCompResult = 1U;
                    ExitLoop = 1U;
                }
            } else if ((pPtnStr[2] == '\0') && (pSpecifierChar[1] != '\0')) {
                AmbaMisra_TypeCast(&(pSpecifierU16), &(pSpecifierChar));
                if (*pSpecifierU16 == *pPtnU16) {
                    TypeCompResult = 1U;
                    ExitLoop = 1U;
                }
            } else {
                ExitLoop = 1U;
            }

            if (ExitLoop > 0U) {
                break;
            }

            pSpecifierChar ++;
        }
    }

    return TypeCompResult;
}

static UINT32 SvcWrap_SpecifierPaddingNum(const char *pSpecifier, char *pPaddingChar)
{
    UINT32 RetNum = 0U;

    if (pSpecifier != NULL) {
        UINT8  *pU8;
        UINT32 SpecifierStrLen = (UINT32)AmbaUtility_StringLength(pSpecifier);
        const char  *pSpecifierStr = pSpecifier;
        char   PaddingChar = ' ';
        UINT32 PaddingNum = 0, TmpU32;

        SpecifierStrLen --;
        pSpecifierStr ++;

        if (1U == SvcWrap_IsDigital(*pSpecifierStr)) {
            if (*pSpecifierStr == '0') {
                PaddingChar = '0';
            }
        }

        while (SpecifierStrLen > 1U) {
            if (1U == SvcWrap_IsDigital(*pSpecifierStr)) {
                AmbaMisra_TypeCast(&pU8, &pSpecifierStr);
                TmpU32 = *pU8;
                TmpU32 &= 0xFFU;
                PaddingNum = ( PaddingNum * 10U ) + ( TmpU32 - 48U );
                SpecifierStrLen --;
                pSpecifierStr ++;

                /* CERT STR30-C */
                AmbaMisra_TouchUnused(pU8);
            } else {
                break;
            }
        }

        if (pPaddingChar != NULL) {
            *pPaddingChar = PaddingChar;
        }

        RetNum = PaddingNum;
    }

    return RetNum;
}

static UINT32 SvcWrap_SpecifierAfterPointNum(const char *pSpecifier)
{
    UINT32 RetNum = 0U;

    if (pSpecifier != NULL) {
        UINT8  *pU8;
        const char *pSpecifierStr = pSpecifier;
        const char *pAfterPointStr = NULL;
        UINT32 AfterPointNum = 0, TmpU32;

        while (*pSpecifierStr != '\0') {

            if (*pSpecifierStr == '.') {
                pAfterPointStr = pSpecifierStr;
                break;
            }

            pSpecifierStr ++;
        }

        if (pAfterPointStr != NULL) {
            pAfterPointStr ++;
            while (*pAfterPointStr != '\0') {
                if (1U == SvcWrap_IsDigital(*pAfterPointStr)) {
                    AmbaMisra_TypeCast(&pU8, &pAfterPointStr);
                    TmpU32 = *pU8;
                    TmpU32 &= 0xFFU;
                    AfterPointNum = ( AfterPointNum * 10U ) + ( TmpU32 - 48U );

                    /* CERT STR30-C */
                    AmbaMisra_TouchUnused(pU8);
                } else {
                    break;
                }
                pAfterPointStr ++;
            }
        }

        RetNum = AfterPointNum;
    }

    return RetNum;
}

static void SvcWrap_SpecifierStringCopy(char *pBuf, UINT32 BufSize, const char *pDigitalBuf, char PaddingChar, UINT32 PaddingNum)
{
    if ((pBuf != NULL) && (pDigitalBuf != NULL)) {
        char  *pCurBuf = pBuf;
        UINT32 CurBufSize = BufSize;
        UINT32 CurPaddingNum = PaddingNum;
        UINT32 ExtraSignProc = 0U;
        UINT32 CurDigitalNum = (UINT32)AmbaUtility_StringLength(pDigitalBuf);

        if (CurDigitalNum > 0U) {
            if ((pDigitalBuf[0] == '-') && (PaddingChar != ' ')) {
                ExtraSignProc = 1U;
                if (CurPaddingNum > 0U) {
                    CurPaddingNum -= 1U;
                }
            }

            if (CurDigitalNum <= (BufSize - 1U)) {
                if (CurPaddingNum > CurDigitalNum) {
                    CurPaddingNum -= CurDigitalNum;
                } else {
                    CurPaddingNum = 0U;
                }

                if ((CurPaddingNum + CurDigitalNum) > (BufSize - 1U)) {
                    CurPaddingNum = (CurPaddingNum + CurDigitalNum) - (BufSize - 1U);
                }

                pCurBuf = pBuf;
                CurBufSize = BufSize;
                if (ExtraSignProc > 0U) {
                    *pCurBuf = '-';
                    pCurBuf ++;
                    CurBufSize --;
                }

                while (CurPaddingNum > 0U) {
                    *pCurBuf = PaddingChar;
                    pCurBuf ++;
                    CurBufSize --;
                    CurPaddingNum --;
                }

                if (ExtraSignProc > 0U) {
                    AmbaUtility_StringCopy(pCurBuf, CurBufSize, &(pDigitalBuf[1]));
                } else {
                    AmbaUtility_StringCopy(pCurBuf, CurBufSize, pDigitalBuf);
                }
            }
        }
    }
}

static UINT32 SvcWrap_SpecifierInt16ToDecStr(INT16 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize)
{
    INT32 Int32Val = InputVal;

    return SvcWrap_SpecifierInt32ToDecStr(Int32Val, pSpecifier, pBuf, BufSize);
}

static UINT32 SvcWrap_SpecifierInt32ToDecStr(INT32 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = 0U;

    if (pSpecifier == NULL) {
        RetVal = 2U;
    } else if (pBuf == NULL) {
        RetVal = 3U;
    } else {
        char   DigitalCharBuf[16];
        char   PaddingChar = ' ';
        UINT32 PaddingNum;

        PaddingNum = SvcWrap_SpecifierPaddingNum(pSpecifier, &PaddingChar);

        AmbaSvcWrap_MisraMemset(DigitalCharBuf, 0, sizeof(DigitalCharBuf));

        (void) AmbaUtility_Int32ToStr(DigitalCharBuf, (UINT32)sizeof(DigitalCharBuf), InputVal, 10U);

        SvcWrap_SpecifierStringCopy(pBuf, BufSize, DigitalCharBuf, PaddingChar, PaddingNum);
    }

    return RetVal;
}

static UINT32 SvcWrap_SpecifierInt64ToDecStr(INT64 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = 0U;

    if (pSpecifier == NULL) {
        RetVal = 2U;
    } else if (pBuf == NULL) {
        RetVal = 3U;
    } else {
        char   DigitalCharBuf[32];
        char   PaddingChar = ' ';
        UINT32 PaddingNum;

        PaddingNum = SvcWrap_SpecifierPaddingNum(pSpecifier, &PaddingChar);

        AmbaSvcWrap_MisraMemset(DigitalCharBuf, 0, sizeof(DigitalCharBuf));

        (void) AmbaUtility_Int64ToStr(DigitalCharBuf, (UINT32)sizeof(DigitalCharBuf), InputVal, 10U);

        SvcWrap_SpecifierStringCopy(pBuf, BufSize, DigitalCharBuf, PaddingChar, PaddingNum);
    }

    return RetVal;
}

static void SvcWrap_SpecifierINT(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg)
{
    if (pBuf == NULL) {
        // TBD
    } else if (BufSize == 0U) {
        // TBD
    } else if (pSpecifier == NULL) {
        // TBD
    } else if (pArg == NULL) {
        // TBD
    } else {
        if (0U < SvcWrap_SpecifierPtnSearch(pSpecifier, "ll")) {
            INT64 Int64 = (INT64)(pArg->Uint64);
            (void) SvcWrap_SpecifierInt64ToDecStr(Int64, pSpecifier, pBuf, BufSize);
        } else if (0U < SvcWrap_SpecifierPtnSearch(pSpecifier, "h")) {
            INT16 Int16 = (INT16)(pArg->Uint64);
            (void) SvcWrap_SpecifierInt16ToDecStr(Int16, pSpecifier, pBuf, BufSize);
        } else {
            INT32 Int32 = (INT32)(pArg->Uint64);
            (void) SvcWrap_SpecifierInt32ToDecStr(Int32, pSpecifier, pBuf, BufSize);
        }
    }

    AmbaMisra_TouchUnused(pArg);
}

static UINT32 SvcWrap_SpecifierUInt32ToDecStr(UINT32 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = 0U;

    if (pSpecifier == NULL) {
        RetVal = 2U;
    } else if (pBuf == NULL) {
        RetVal = 3U;
    } else {
        char   DigitalCharBuf[16];
        char   PaddingChar = ' ';
        UINT32 PaddingNum;

        PaddingNum = SvcWrap_SpecifierPaddingNum(pSpecifier, &PaddingChar);

        AmbaSvcWrap_MisraMemset(DigitalCharBuf, 0, sizeof(DigitalCharBuf));

        (void) AmbaUtility_UInt32ToStr(DigitalCharBuf, (UINT32)sizeof(DigitalCharBuf), InputVal, 10U);

        SvcWrap_SpecifierStringCopy(pBuf, BufSize, DigitalCharBuf, PaddingChar, PaddingNum);
    }

    return RetVal;
}

static UINT32 SvcWrap_SpecifierUInt64ToDecStr(UINT64 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = 0U;

    if (pSpecifier == NULL) {
        RetVal = 2U;
    } else if (pBuf == NULL) {
        RetVal = 3U;
    } else {
        char   DigitalCharBuf[32];
        char   PaddingChar = ' ';
        UINT32 PaddingNum;

        PaddingNum = SvcWrap_SpecifierPaddingNum(pSpecifier, &PaddingChar);

        AmbaSvcWrap_MisraMemset(DigitalCharBuf, 0, sizeof(DigitalCharBuf));

        (void) AmbaUtility_UInt64ToStr(DigitalCharBuf, (UINT32)sizeof(DigitalCharBuf), InputVal, 10U);

        SvcWrap_SpecifierStringCopy(pBuf, BufSize, DigitalCharBuf, PaddingChar, PaddingNum);
    }

    return RetVal;
}

static void SvcWrap_SpecifierUINT(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg)
{
    if (pBuf == NULL) {
        // TBD
    } else if (BufSize == 0U) {
        // TBD
    } else if (pSpecifier == NULL) {
        // TBD
    } else if (pArg == NULL) {
        // TBD
    } else {
        if (0U < SvcWrap_SpecifierPtnSearch(pSpecifier, "ll")) {
            (void) SvcWrap_SpecifierUInt64ToDecStr(pArg->Uint64, pSpecifier, pBuf, BufSize);
        } else {
            UINT32 UInt32 = (UINT32)(pArg->Uint64);
            (void) SvcWrap_SpecifierUInt32ToDecStr(UInt32, pSpecifier, pBuf, BufSize);
        }
    }

    AmbaMisra_TouchUnused(pArg);
}

static void SvcWrap_SpecifierHexLowerCase(char *pBuf)
{
    if (pBuf != NULL) {
        if (0U < AmbaUtility_StringLength(pBuf)) {
            char *pCurBuf = pBuf;

            while (*pCurBuf != '\0') {
                if ((*pCurBuf >= 'A') && (*pCurBuf <= 'Z')) {
                    *pCurBuf += 32;
                }
                pCurBuf ++;
            }
        }
    }
}

static UINT32 SvcWrap_SpecifierUInt32ToHexStr(UINT32 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = 0U;

    if (pSpecifier == NULL) {
        RetVal = 2U;
    } else if (pBuf == NULL) {
        RetVal = 3U;
    } else {
        char   DigitalCharBuf[32];
        char   PaddingChar = ' ';
        UINT32 PaddingNum;

        PaddingNum = SvcWrap_SpecifierPaddingNum(pSpecifier, &PaddingChar);

        AmbaSvcWrap_MisraMemset(DigitalCharBuf, 0, sizeof(DigitalCharBuf));

        (void) AmbaUtility_UInt32ToStr(DigitalCharBuf, (UINT32)sizeof(DigitalCharBuf), InputVal, 16U);

        SvcWrap_SpecifierStringCopy(pBuf, BufSize, DigitalCharBuf, PaddingChar, PaddingNum);
    }

    return RetVal;
}

static UINT32 SvcWrap_SpecifierUInt64ToHexStr(UINT64 InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = 0U;

    if (pSpecifier == NULL) {
        RetVal = 2U;
    } else if (pBuf == NULL) {
        RetVal = 3U;
    } else {
        char   DigitalCharBuf[32];
        char   PaddingChar = ' ';
        UINT32 PaddingNum;

        PaddingNum = SvcWrap_SpecifierPaddingNum(pSpecifier, &PaddingChar);

        AmbaSvcWrap_MisraMemset(DigitalCharBuf, 0, sizeof(DigitalCharBuf));

        (void) AmbaUtility_UInt64ToStr(DigitalCharBuf, (UINT32)sizeof(DigitalCharBuf), InputVal, 16U);

        SvcWrap_SpecifierStringCopy(pBuf, BufSize, DigitalCharBuf, PaddingChar, PaddingNum);
    }

    return RetVal;
}

static void SvcWrap_SpecifierHex(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg)
{
    if (pBuf == NULL) {
        // TBD
    } else if (BufSize == 0U) {
        // TBD
    } else if (pSpecifier == NULL) {
        // TBD
    } else if (pArg == NULL) {
        // TBD
    } else {
        if (0U < SvcWrap_SpecifierPtnSearch(pSpecifier, "ll")) {
            (void) SvcWrap_SpecifierUInt64ToHexStr(pArg->Uint64, pSpecifier, pBuf, BufSize);
        } else {
            UINT32 UInt32 = (UINT32)(pArg->Uint64);
            (void) SvcWrap_SpecifierUInt32ToHexStr(UInt32, pSpecifier, pBuf, BufSize);
        }

        if (0U < SvcWrap_SpecifierPtnSearch(pSpecifier, "x")) {
            SvcWrap_SpecifierHexLowerCase(pBuf);
        }
    }

    AmbaMisra_TouchUnused(pArg);
}

static UINT32 SvcWrap_SpecifierDoubleToDecStr(DOUBLE InputVal, const char *pSpecifier, char *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = 0U;

    if (pSpecifier == NULL) {
        RetVal = 2U;
    } else if (pBuf == NULL) {
        RetVal = 3U;
    } else {
        char   DigitalCharBuf[352];
        char   PaddingChar = ' ';
        UINT32 PaddingNum;
        UINT32 AfterPoint;

        PaddingNum = SvcWrap_SpecifierPaddingNum(pSpecifier, &PaddingChar);
        AfterPoint = SvcWrap_SpecifierAfterPointNum(pSpecifier);
        if (AfterPoint == 0U) {
            AfterPoint = 6U;
        }

        AmbaSvcWrap_MisraMemset(DigitalCharBuf, 0, sizeof(DigitalCharBuf));

        (void) AmbaUtility_DoubleToStr(DigitalCharBuf, (UINT32)sizeof(DigitalCharBuf), InputVal, AfterPoint);

        SvcWrap_SpecifierStringCopy(pBuf, BufSize, DigitalCharBuf, PaddingChar, PaddingNum);
    }

    return RetVal;
}

static void SvcWrap_SpecifierDouble(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg)
{
    if (pBuf == NULL) {
        // TBD
    } else if (BufSize == 0U) {
        // TBD
    } else if (pSpecifier == NULL) {
        // TBD
    } else if (pArg == NULL) {
        // TBD
    } else {
        // DOUBLE DoubleVal;
        (void) SvcWrap_SpecifierDoubleToDecStr(pArg->Doubld, pSpecifier, pBuf, BufSize);
    }

    AmbaMisra_TouchUnused(pArg);
}

static void SvcWrap_SpecifierChar(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg)
{
    if (pBuf == NULL) {
        // TBD
    } else if (BufSize == 0U) {
        // TBD
    } else if (pSpecifier == NULL) {
        // TBD
    } else if (pArg == NULL) {
        // TBD
    } else {
        if (BufSize >= 2U) {
            pBuf[0] = pArg->Char;
            pBuf[1] = '\0';
        }
    }

    AmbaMisra_TouchUnused(pArg);
}

static void SvcWrap_SpecifierStr(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg)
{
    if (pBuf == NULL) {
        // TBD
    } else if (BufSize == 0U) {
        // TBD
    } else if (pSpecifier == NULL) {
        // TBD
    } else if (pArg == NULL) {
        // TBD
    } else {
        UINT32 CurStrLen = 0U;
        const char *pChar = pArg->pCStr;

        if (pChar != NULL) {
            while ((*pChar != '\0') && (CurStrLen < BufSize)) {
                pBuf[CurStrLen] = *pChar;
                CurStrLen ++;
                pChar ++;
            }
        }
        pBuf[CurStrLen] = '\0';
    }

    AmbaMisra_TouchUnused(pArg);
}

static void SvcWrap_SpecifierPoint(char *pBuf, UINT32 BufSize, const char *pSpecifier, SVC_WRAP_ARGV_s *pArg)
{
    if (pBuf == NULL) {
        // TBD
    } else if (BufSize == 0U) {
        // TBD
    } else if (pSpecifier == NULL) {
        // TBD
    } else if (pArg == NULL) {
        // TBD
    } else {
        char *pCurBuf = pBuf;
        UINT32 CurBufSize = BufSize;

        if (CurBufSize > 3U) {
            pCurBuf[0] = '0';
            pCurBuf[1] = 'x';
            pCurBuf[2] = '\0';
            pCurBuf = &(pCurBuf[2]);
            CurBufSize -= 2U;

#if defined(CONFIG_ARM32)
            {
                UINT32 UInt32 = 0U;
                if (pArg->pPointer != NULL) {
                    AmbaMisra_TypeCast(&(UInt32), &(pArg->pPointer));
                } else {
                    UInt32 = (UINT32)(pArg->Uint64);
                }
                (void) SvcWrap_SpecifierUInt32ToHexStr(UInt32, "%08X", pCurBuf, CurBufSize);
            }
#else
            {
                UINT64 UInt64 = 0U;
                if (pArg->pPointer != NULL) {
                    AmbaMisra_TypeCast(&(UInt64), &(pArg->pPointer));
                } else {
                    UInt64 = pArg->Uint64;
                }
                (void) SvcWrap_SpecifierUInt64ToHexStr(UInt64, "%016llX", pCurBuf, CurBufSize);
            }
#endif
        }
    }

    AmbaMisra_TouchUnused(pArg);
}

static void SvcWrap_SpecifierParsing(const char *pSpecifierStr, const SVC_WRAP_PRN_SPECIFIER_s *pSpecifier, SVC_WRAP_ARGV_s *pArgVector, char *pBuf, UINT32 BufSize)
{
    if (pSpecifierStr == NULL) {
        // TBD
    } else if (pSpecifier == NULL) {
        // TBD
    } else if (pArgVector == NULL) {
        // TBD
    } else if (pBuf == NULL) {
        // TBD
    } else {
        char *pDstBuf;
        UINT32 DstBufSize, TmpU32;

        TmpU32 = (UINT32)AmbaUtility_StringLength(pBuf);
        pDstBuf = &(pBuf[TmpU32]);
        TmpU32 += 1U;
        if (TmpU32 >= BufSize) {
            DstBufSize = 0U;
        } else {
            DstBufSize = BufSize - TmpU32;
        }

        if ((pSpecifier->Enable > 0U) && (pSpecifier->pProc != NULL) && (DstBufSize > 0U)) {
            (pSpecifier->pProc)(pDstBuf, DstBufSize, pSpecifierStr, pArgVector);
        }

        AmbaMisra_TouchUnused(SvcWrapSpecifier);
    }
}

static UINT32 SvcWrap_AddrCompare(const void *pAddr0, const void *pAddr1)
{
    UINT32 RetVal = 0U;

#ifdef CONFIG_ARM64
    UINT64 Addr0 = 0ULL, Addr1 = 0ULL;
#else
    UINT32 Addr0 = 0U, Addr1 = 0U;
#endif

    AmbaMisra_TypeCast(&Addr0, &pAddr0);
    AmbaMisra_TypeCast(&Addr1, &pAddr1);

    if (Addr0 > Addr1) {
        RetVal = 1U;
    }

    return RetVal;
}

static void SvcWrap_SpecifierGetStr(char *pFmt,           UINT32 *pPreSpecifierLeng,
                                    char **pSpecifierStr, UINT32 *pSpecifierLeng,
                                    char **pNextStr,
                                    SVC_WRAP_PRN_SPECIFIER_s **pSpecifier)
{
    if (pFmt == NULL) {
        // TBD
    } else if (pSpecifierStr == NULL) {
        // TBD
    } else if (pNextStr == NULL) {
        // TBD
    } else if (pSpecifier == NULL) {
        // TBD
    } else if (pPreSpecifierLeng == NULL) {
        // TBD
    } else if (pSpecifierLeng == NULL) {
        // TBD
    } else {
        UINT32 FmtStrIdx = 0U, SpecifierIdx, SpecifierLeng = 0U;
        char *pSpecifyStrBegin = NULL, *pSpecifyStrEnd = NULL;
        SVC_WRAP_PRN_SPECIFIER_s *pCurSpecifier = NULL;

        while (pFmt[FmtStrIdx] != '\0') {
            if (pFmt[FmtStrIdx] == '%') {
                if (pFmt[FmtStrIdx + 1U] == '%') {
                    FmtStrIdx += 1U;
                } else {
                    pSpecifyStrBegin = &(pFmt[FmtStrIdx]);
                    *pPreSpecifierLeng = FmtStrIdx;
                }
            } else {
                if (pSpecifyStrBegin != NULL) {
                    SpecifierLeng ++;
                    for (SpecifierIdx = 0U; SpecifierIdx < SVC_WRAP_PRN_SPECIFIER_NUM; SpecifierIdx ++) {
                        pCurSpecifier = &(SvcWrapSpecifier[SpecifierIdx]);
                        if (pCurSpecifier->Enable > 0U) {
                            if (pCurSpecifier->Specifier == pFmt[FmtStrIdx]) {
                                break;
                            }
                        }
                        pCurSpecifier = NULL;
                    }

                    if (pCurSpecifier != NULL) {
                        pSpecifyStrEnd = &(pFmt[FmtStrIdx]);
                        break;
                    }
                }
            }

            FmtStrIdx ++;
        }

        if ((pSpecifyStrBegin != NULL) && (pSpecifyStrEnd != NULL)) {
            *pSpecifierStr = pSpecifyStrBegin;
            pSpecifyStrEnd ++;
            *pNextStr      = pSpecifyStrEnd;
            *pSpecifier    = pCurSpecifier;
            *pSpecifierLeng = SpecifierLeng + 1U;
        } else {
            *pSpecifierStr = NULL;
            *pNextStr      = NULL;
            *pSpecifier    = NULL;
        }
    }
}

/**
* advanced print proc
* @param [in] pStrBuf pointer to string buffer
* @param [in] StrBufSize size of string buffer
* @param [in] pFmt pointer to desired print format
* @param [in] ArgAddr base of arg
* @param [out] pStrLen final print string length
* @return RetVal 0 ok, !0 error
*
*/
static UINT32 SvcWrap_ArgvProc(char *pStrBuf, UINT32 StrBufSize, const char *pFmt, UINT32 ArgCount, SVC_WRAP_ARGV_s *pArgVector, UINT32 *pStrLen)
{
    UINT32 RetVal = 0U;

    if (pStrBuf == NULL) {
        RetVal = 1U;
    } else if (StrBufSize == 0U) {
        RetVal = 1U;
    } else if (pFmt == NULL) {
        RetVal = 1U;
    } else if (pArgVector == NULL) {
        RetVal = 2U;
    } else {
        char SpecifierStrBuf[16];
        const char *pCurFmt = pFmt;
        char *pCurStr = NULL;
        char *pNextStr = NULL;
        char *pSpecifierStr = NULL;
        UINT32 CurPreSpecifierLeng = 0U;
        UINT32 CurSpecifierLeng = 0U;
        SVC_WRAP_PRN_SPECIFIER_s *pCurSpecifier;
        UINT32 CurArgIdx = 0U;

        AmbaMisra_TypeCast(&(pNextStr), &(pCurFmt));

        AmbaSvcWrap_MisraMemset(pStrBuf, 0, StrBufSize);

        do {
            pCurStr = pNextStr;

            pNextStr      = NULL;
            pSpecifierStr = NULL;
            pCurSpecifier = NULL;

            SvcWrap_SpecifierGetStr(pCurStr, &CurPreSpecifierLeng, &pSpecifierStr, &CurSpecifierLeng, &pNextStr, &pCurSpecifier);

            if ((pSpecifierStr != NULL) &&
                (pNextStr != NULL) &&
                (pCurSpecifier != NULL) &&
                (CurArgIdx < ArgCount)) {

                AmbaSvcWrap_MisraMemset(SpecifierStrBuf, 0, sizeof(SpecifierStrBuf));
                AmbaSvcWrap_MisraMemcpy(SpecifierStrBuf, pSpecifierStr, CurSpecifierLeng);
                SpecifierStrBuf[CurSpecifierLeng] = '\0';

                if (0U < SvcWrap_AddrCompare(pSpecifierStr, pCurStr)) {
                    AmbaSvcWrap_MisraMemcpy(&(pStrBuf[AmbaUtility_StringLength(pStrBuf)]), pCurStr, CurPreSpecifierLeng);
                }
                SvcWrap_SpecifierParsing(SpecifierStrBuf, pCurSpecifier, &(pArgVector[CurArgIdx]), pStrBuf, StrBufSize);
                CurArgIdx ++;

            } else {
                AmbaUtility_StringAppend(pStrBuf, StrBufSize, pCurStr);
                break;
            }

        } while (0U < SvcWrap_AddrCompare(pNextStr, pCurStr));

        if (pStrLen != NULL) {
            *pStrLen = (UINT32)AmbaUtility_StringLength(pStrBuf);
        }
    }

    return RetVal;
}

#if 0
extern void SvcWrap_PrintExample(void);
void SvcWrap_PrintExample(void)
{
    UINT32 Idx, Cnt;
    const char *pModuleStr = "SYS";
    SVC_WRAP_PRINT_s LocalPrint;

    AmbaSvcWrap_MisraMemset(&LocalPrint, 0, sizeof(LocalPrint));

    SVC_WRAP_PRINT " " SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_E
    SVC_WRAP_PRINT "====== Print Specifier ======" SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_E

    Cnt = (UINT32)sizeof(SvcWrapSpecifier) / (UINT32)sizeof(SvcWrapSpecifier[0]);
    for (Idx = 0U; Idx < Cnt; Idx ++) {
        if (SvcWrapSpecifier[Idx].Enable > 0U) {
            SVC_WRAP_PRINT "  %%%c    %s"
                SVC_PRN_ARG_S pModuleStr
                SVC_PRN_ARG_CHAR SvcWrapSpecifier[Idx].Specifier  SVC_PRN_ARG_POST
                SVC_PRN_ARG_CSTR SvcWrapSpecifier[Idx].Desciption SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
        }
    }

    SVC_WRAP_PRINT " " SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_E
    SVC_WRAP_PRINT "====== Print Example ======" SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  Decimal (16 bits) : %%hd      + 2019             => %hd"     SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_INT16  2019              SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  Decimal (16 bits) : %%hd      + -2019            => %hd"     SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_INT16  -2019             SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  Decimal (32 bits) : %%d       + 2019             => %d"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_INT32  2019              SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "                      %%d       + -2019            => %d"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_INT32  -2019             SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "                      %%08d     + 2019             => %08d"    SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_INT32  2019              SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "                      %%u       + 2019             => %u"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_UINT32 2019U             SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "                      %%u       + -2019            => %u"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_UINT32 -2019             SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  Hex     (32 bits) : %%x       + 0xabcd           => %x"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_UINT32 0xabcdU           SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "                      %%X       + 0xabcd           => %X"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_UINT32 0xabcdU           SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  Hex     (64 bits) : %%llx     + 0x201910160451LL => %llx"    SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_UINT64 0x201910160451ULL                  SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  Hex     (64 bits) : %%016llx  + 0x1910160451LL   => %016llx" SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_UINT64 0x1910160451ULL                    SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  DOUBLE  (64 bits) : %%f       + 3.14159          => %f"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_DOUBLE 3.14159                            SVC_PRN_ARG_E
    SVC_WRAP_PRINT "                      %%f       + -3.14159         => %f"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_DOUBLE -3.14159                           SVC_PRN_ARG_E
    SVC_WRAP_PRINT "                      %%8.4f    + 3.14159          => %8.4f"   SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_DOUBLE 3.14159                            SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  Char    ( 8 bits) : %%c       + 'a'              => %c"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_CHAR   'a'               SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  String            : %%s       + \"Just Test!\"     => %s"    SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_CSTR   "Just Test!"      SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  Pointer Address   : %%p       + &Idx             => %p"      SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_CPOINT &Idx              SVC_PRN_ARG_POST SVC_PRN_ARG_E
    SVC_WRAP_PRINT " " SVC_PRN_ARG_S pModuleStr SVC_PRN_ARG_E
}
#endif

static void DBGLog(const char *pModule, const char *pFormat)
{
    AmbaPrint_PrintStr5("[%s|DBG] %s", pModule, pFormat, NULL, NULL, NULL);
}

/**
* wrap of print function
* @param [in] pModule module
* @param [in] pPrint print parameters
* @return none
*/
void SvcWrap_Print(const char *pModule, SVC_WRAP_PRINT_s *pPrint)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (pPrint != NULL) {
        char IntStrBuf[512];

        AmbaSvcWrap_MisraMemset(IntStrBuf, 0, sizeof(IntStrBuf));

        RetVal = SvcWrap_ArgvProc(IntStrBuf, (UINT32)sizeof(IntStrBuf), pPrint->pStrFmt, pPrint->Argc, pPrint->Argv, &PRetVal);
        if ((RetVal == 0U) && (PRetVal > 0U)) {
            if (pPrint->pProc != NULL) {
                (pPrint->pProc)(pModule, IntStrBuf, 0U, 0U);
            } else {
                DBGLog(pModule, IntStrBuf);
            }
        }
    }
}

/**
* wrap of snprintf function
* @param [in] pBuf buffer
* @param [in] BufSize buffer size
* @param [in] pFmt string format
* @return length of string
*/
UINT32 SvcWrap_snprintf(char *pBuf, UINT32 BufSize, SVC_WRAP_SNPRINT_s *pFmt)
{
    UINT32 Length = 0U;

    if ((pFmt != NULL) && (pBuf != NULL) && (BufSize > 0U)) {
        (void) SvcWrap_ArgvProc(pBuf, BufSize, pFmt->pStrFmt, pFmt->Argc, pFmt->Argv, &Length);
    }

    return Length;
}

static void SvcWrapPrnFunc(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if ((pModule != NULL) && (pFormat != NULL)) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, 0U, 0U, 0U);
    }
}

void SvcWrap_PrintUL(const char *pFormat, ULONG Arg1, ULONG Arg2, ULONG Arg3, ULONG Arg4, ULONG Arg5)
{
#define DBG_PRN_NATIVE      (0U)

#if DBG_PRN_NATIVE
    printf(pFmt, x1, x2, x3, x4, x5);
    printf("\n");
#else
    SVC_WRAP_PRINT_s SvcWrapPrn;

    if (AmbaWrap_memset(&SvcWrapPrn, 0, sizeof(SvcWrapPrn)) == OK) {
        SvcWrapPrn.pProc = SvcWrapPrnFunc;
        SvcWrapPrn.pStrFmt = pFormat;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)Arg1; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)Arg2; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)Arg3; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)Arg4; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)Arg5; SvcWrapPrn.Argc ++;

        SvcWrap_Print("", &SvcWrapPrn);
    }
#endif
}


