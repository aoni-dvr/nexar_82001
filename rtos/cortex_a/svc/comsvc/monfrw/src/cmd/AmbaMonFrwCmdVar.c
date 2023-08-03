/**
 *  @file AmbaMonFrwCmdVar.c
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details AmbaShell Monitor Framework Command Variable
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"

#include "AmbaMonFrwCmdVar.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MONFRW_MEM_INFO_u_*/ {
    char   Char;
    UINT8  Uint8;
} AMBA_MONFRW_MEM_INFO_u;

typedef struct /*_AMBA_MONFRW_MEM_INFO_s_*/ {
    AMBA_MONFRW_MEM_INFO_u    Ctx;
} AMBA_MONFRW_MEM_INFO_s;

static UINT32 AmbaMonFrwCmdVar_PrintFlag = 0xFFFFFFFFU;

/**
 *  sfvar atoi ex
 *  @param[in] pStr pointer to the string
 *  @param[in] Base number base
 *  @return value (INT64)
 *  @note this function is intended for internal use only
 */
static INT64 sfvar_atoi_ex(const char *pStr, INT32 Base)
{
    INT64 RetVal = 0LL;
    INT64 i = 0LL;

    INT64 Sign = 1LL;

    AMBA_MONFRW_MEM_INFO_s MemInfo;

    INT64 Val, BaseVal;
    INT64 StrVal, StrBaseVal;

    const char *pStr2;

    if (pStr != NULL) {
        if (pStr[0] == '-') {
            /* negative */
            Sign = -1LL;
            /* char next */
            pStr2 = &(pStr[1]);
        } else {
            pStr2 = pStr;
        }

        while (pStr2[i] != '\0') {
            /* base char val */
            switch ((UINT8) pStr2[i]) {
                case (UINT8) '0':
                case (UINT8) '1':
                case (UINT8) '2':
                case (UINT8) '3':
                case (UINT8) '4':
                case (UINT8) '5':
                case (UINT8) '6':
                case (UINT8) '7':
                case (UINT8) '8':
                case (UINT8) '9':
                    /* zero char base val */
                    MemInfo.Ctx.Char = '0';
                    StrBaseVal = (INT64) MemInfo.Ctx.Uint8;
                    BaseVal = 0LL;
                    break;
                case (UINT8) 'A':
                case (UINT8) 'B':
                case (UINT8) 'C':
                case (UINT8) 'D':
                case (UINT8) 'E':
                case (UINT8) 'F':
                    /* A char base val */
                    MemInfo.Ctx.Char = 'A';
                    StrBaseVal = (INT64) MemInfo.Ctx.Uint8;
                    BaseVal = 10LL;
                    break;
                case (UINT8) 'a':
                case (UINT8) 'b':
                case (UINT8) 'c':
                case (UINT8) 'd':
                case (UINT8) 'e':
                case (UINT8) 'f':
                    /* a char base val */
                    MemInfo.Ctx.Char = 'a';
                    StrBaseVal = (INT64) MemInfo.Ctx.Uint8;
                    BaseVal = 10LL;
                    break;
                default:
                    MemInfo.Ctx.Char = pStr2[i];
                    StrBaseVal = (INT64) MemInfo.Ctx.Uint8;
                    BaseVal = -1LL;
                    break;
            }
            /* is valid? */
            if (BaseVal >= 0LL) {
                /* num char val */
                MemInfo.Ctx.Char = pStr2[i];
                StrVal = (INT64) MemInfo.Ctx.Uint8;
                /* num diff val */
                Val = StrVal - StrBaseVal;
                /* val accumulated */
                RetVal = (RetVal * Base) + Val + BaseVal;
            }
            /* next */
            i++;
        }
    }

    return (Sign * RetVal);
}

/**
 *  sfvar atou ex
 *  @param[in] pStr pointer to the string
 *  @param[in] Base number base
 *  @return value (INT64)
 *  @note this function is intended for internal use only
 */
static UINT64 sfvar_atou_ex(const char *pStr, UINT32 Base)
{
    UINT64 RetVal = 0ULL;
    UINT64 i = 0ULL;

    AMBA_MONFRW_MEM_INFO_s MemInfo;

    UINT64 Val, BaseVal;
    UINT64 StrVal, StrBaseVal;

    const char *pStr2;

    if (pStr != NULL) {
        pStr2 = pStr;
        while (pStr2[i] != '\0') {
            /* base char val */
            switch ((UINT8) pStr2[i]) {
                case (UINT8) '0':
                case (UINT8) '1':
                case (UINT8) '2':
                case (UINT8) '3':
                case (UINT8) '4':
                case (UINT8) '5':
                case (UINT8) '6':
                case (UINT8) '7':
                case (UINT8) '8':
                case (UINT8) '9':
                    /* zero char base val */
                    MemInfo.Ctx.Char = '0';
                    StrBaseVal = (UINT64) MemInfo.Ctx.Uint8;
                    BaseVal = 0ULL;
                    break;
                case (UINT8) 'A':
                case (UINT8) 'B':
                case (UINT8) 'C':
                case (UINT8) 'D':
                case (UINT8) 'E':
                case (UINT8) 'F':
                    /* A char base val */
                    MemInfo.Ctx.Char = 'A';
                    StrBaseVal = (UINT64) MemInfo.Ctx.Uint8;
                    BaseVal = 10ULL;
                    break;
                case (UINT8) 'a':
                case (UINT8) 'b':
                case (UINT8) 'c':
                case (UINT8) 'd':
                case (UINT8) 'e':
                case (UINT8) 'f':
                    /* a char base val */
                    MemInfo.Ctx.Char = 'a';
                    StrBaseVal = (UINT64) MemInfo.Ctx.Uint8;
                    BaseVal = 10ULL;
                    break;
                default:
                    MemInfo.Ctx.Char = pStr2[i];
                    StrBaseVal = (UINT64) MemInfo.Ctx.Uint8;
                    BaseVal = 0xFFFFULL;
                    break;
            }
            /* is valid? */
            if (BaseVal != 0xFFFFUL) {
                /* num char val */
                MemInfo.Ctx.Char = pStr2[i];
                StrVal = (UINT64) MemInfo.Ctx.Uint8;
                /* num diff val */
                Val = (StrVal > StrBaseVal) ? (StrVal - StrBaseVal) : 0ULL;
                /* val accumulated */
                RetVal = (RetVal * Base) + Val + BaseVal;
            }
            /* next */
            i++;
        }
    }

    return RetVal;
}

/**
 *  @private
 *  sfvar atoll
 *  @param[in] pStr pinter to the string
 *  @return value (INT64)
 *  @note this function is intended for internal use only
 */
INT64 sfvar_atoll(const char *pStr)
{
    INT64 Var = 0LL;

    switch ((UINT8) pStr[0]) {
        case (UINT8) '$':
            /* */
            break;
        case (UINT8) '0':
            if ((pStr[1] == 'x') ||
                (pStr[1] == 'X')) {
                Var = sfvar_atoi_ex(&(pStr[2]), 16);
            } else {
                Var = sfvar_atoi_ex(pStr, 10);
            }
            break;
        case (UINT8) '-':
        case (UINT8) '1':
        case (UINT8) '2':
        case (UINT8) '3':
        case (UINT8) '4':
        case (UINT8) '5':
        case (UINT8) '6':
        case (UINT8) '7':
        case (UINT8) '8':
        case (UINT8) '9':
            Var = sfvar_atoi_ex(pStr, 10);
            break;
        default:
            /* */
            break;
    }

    return (INT32) Var;
}

/**
 *  @private
 *  sfvar atoi
 *  @param[in] pStr pinter to the string
 *  @return value (INT32)
 *  @note this function is intended for internal use only
 */
INT32 sfvar_atoi(const char *pStr)
{
    return (INT32) sfvar_atoll(pStr);
}

/**
 *  @private
 *  sfvar atoul
 *  @param[in] pStr pinter to the string
 *  @return value (UINT64)
 *  @note this function is intended for internal use only
 */
UINT64 sfvar_atoul(const char *pStr)
{
    UINT64 Var = 0ULL;

    switch ((UINT8) pStr[0]) {
        case (UINT8) '$':
            /* */
            break;
        case (UINT8) '0':
            if ((pStr[1] == 'x') ||
                (pStr[1] == 'X')) {
                Var = sfvar_atou_ex(&(pStr[2]), 16);
            } else {
                Var = sfvar_atou_ex(pStr, 10);
            }
            break;
        case (UINT8) '1':
        case (UINT8) '2':
        case (UINT8) '3':
        case (UINT8) '4':
        case (UINT8) '5':
        case (UINT8) '6':
        case (UINT8) '7':
        case (UINT8) '8':
        case (UINT8) '9':
            Var = sfvar_atou_ex(pStr, 10);
            break;
        default:
            /* */
            break;
    }

    return Var;
}

/**
 *  @private
 *  sfvar atou
 *  @param[in] pStr pinter to the string
 *  @return value (UINT32)
 *  @note this function is intended for internal use only
 */
UINT32 sfvar_atou(const char *pStr)
{
    return (UINT32) sfvar_atoul(pStr);
}

/**
 *  @private
 *  sfvar string length
 *  @param[in] pStr pinter to the string
 *  @return length
 *  @note this function is intended for internal use only
 */
UINT32 sfvar_strlen(const char *pStr)
{
    UINT32 Length = 0U;

    if (pStr != NULL) {
        while (pStr[Length] != '\0') { Length++; }
    }

    return Length;
}

/**
 *  @private
 *  sfvar string compare
 *  @param[in] pStr1 pinter to the string 1
 *  @param[in] pStr1 pinter to the string 2
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 sfvar_strcmp(const char *pStr1, const char *pStr2)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 Len1;
    UINT32 Len2;

    if ((pStr1 != NULL) &&
        (pStr2 != NULL)) {
        /* string length */
        Len1 = sfvar_strlen(pStr1);
        Len2 = sfvar_strlen(pStr2);
        /* compare */
        if ((Len1 > 0U) &&
            (Len2 > 0U)) {
            if (Len1 == Len2) {
                /* length same */
                for (i = 0U; i < Len1; i++) {
                    /* compare char */
                    if (pStr1[i] != pStr2[i]) {
                        break;
                    }
                }
                if (i != Len1) {
                    /* string different */
                    RetCode = NG_UL;
                }
            } else {
                /* length different */
                RetCode = NG_UL;
            }
        } else {
            /* empty */
            RetCode = NG_UL;
        }
    } else {
        /* string null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  sfvar ultoa
 *  @param[in] Value value to trnasfer
 *  @param[out] pStr pinter to the string
 *  @param[in] Base number base
 *  @param[in] Count number count
 *  @param[in] Flag format flag
 *  @note this function is intended for internal use only
 */
void sfvar_ultoa(UINT64 Value, char *pStr, UINT32 Base, UINT32 Count, UINT32 Flag)
{
    static const char AmbaMonFrwVar_NumStr[] = "0123456789ABCDEF";

    UINT32 i, j;

    UINT64 uValue = Value;

    char iStr[32];
    UINT32 StrCnt = 0U;

    for (i = 0U; i < 18U; i++) {
        iStr[i] = AmbaMonFrwVar_NumStr[uValue%Base];
        StrCnt++;
        uValue = uValue/Base;
        if (uValue == 0U) {
            break;
        }
    }

    if (Count >= StrCnt) {
        /* normal */
        switch (Flag) {
            case (UINT32) SFVAR_LEADING_ZERO:
                /* zero leading */
                for (i = 0U; i < (Count - StrCnt); i++) {
                    pStr[i] = '0';
                }
                /* num */
                for (j = i; j < Count; j++) {
                    pStr[j] = iStr[StrCnt-((j-i)+1U)];
                }
                pStr[j] = '\0';
                break;
            case (UINT32) SFVAR_LEADING_SPACE:
                /* space leading */
                for (i = 0U; i < (Count - StrCnt); i++) {
                    pStr[i] = ' ';
                }
                /* num */
                for (j = i; j < Count; j++) {
                    pStr[j] = iStr[StrCnt-((j-i)+1U)];
                }
                pStr[j] = '\0';
                break;
            case (UINT32) SFVAR_LEADING_NONE:
            default:
                /* num */
                for (i = 0U; i < StrCnt; i++) {
                    pStr[i] = iStr[StrCnt-(i+1U)];
                }
                pStr[i] = '\0';
                break;
        }
    } else {
        /* overflow */
        for (i = 0U; i < Count; i++) {
            pStr[i] = '#';
        }
        pStr[i] = '\0';
    }
}

/**
 *  @private
 *  sfvar utoa
 *  @param[in] Value value to trnasfer
 *  @param[out] pStr pinter to the string
 *  @param[in] Base number base
 *  @param[in] Count number count
 *  @param[in] Flag format flag
 *  @note this function is intended for internal use only
 */
void sfvar_utoa(UINT32 Value, char *pStr, UINT32 Base, UINT32 Count, UINT32 Flag)
{
    sfvar_ultoa((UINT64) Value, pStr, Base, Count, Flag);
}

/**
 *  @private
 *  sfvar lltoa
 *  @param[in] Value value to trnasfer
 *  @param[out] pStr pinter to the string
 *  @param[in] Base number base
 *  @param[in] Count number count
 *  @param[in] Flag format flag
 *  @note this function is intended for internal use only
 */
void sfvar_lltoa( INT64 Value, char *pStr, UINT32 Base, UINT32 Count, UINT32 Flag)
{
     INT64 iValue = Value;
    UINT32 ShiftCnt = 0U;
    UINT64 uValue;

    if (iValue < 0L) {
        pStr[0] = '-';
        ShiftCnt++;
        iValue = (-1)*iValue;
    }

    uValue = (iValue > 0) ? (UINT64) iValue : 0U;

    sfvar_ultoa(uValue, &(pStr[ShiftCnt]), Base, Count-ShiftCnt, Flag);
}

/**
 *  @private
 *  sfvar itoa
 *  @param[in] Value value to trnasfer
 *  @param[out] pStr pinter to the string
 *  @param[in] Base number base
 *  @param[in] Count number count
 *  @param[in] Flag format flag
 *  @note this function is intended for internal use only
 */
void sfvar_itoa( INT32 Value, char *pStr, UINT32 Base, UINT32 Count, UINT32 Flag)
{
    sfvar_lltoa((INT64) Value, pStr, Base, Count, Flag);
}

/**
 *  @private
 *  sfvar print
 *  @param[in] pFmt pointer to the print format
 *  @param[in] Argc pointer to the argument count
 *  @param[in] Argv potiner to the argument string pointer
 *  @note this function is intended for internal use only
 */
void sfvar_print(const char *pFmt, const UINT32 *Argc, const char * const *Argv)
{
    UINT32 i;
    const char *pArgv[5];
    UINT32 ArgCnt;

    if (Argc != NULL) {
        ArgCnt = *Argc;
        /* limitation, may be refined in the future */
        if (ArgCnt > 5U) {
            ArgCnt = 5U;
        }

        pArgv[0] = NULL;
        pArgv[1] = NULL;
        pArgv[2] = NULL;
        pArgv[3] = NULL;
        pArgv[4] = NULL;

        for (i = 0U; i < ArgCnt; i++) {
            pArgv[i] = Argv[i];
        }

        AmbaPrint_PrintStr5(pFmt, pArgv[0], pArgv[1], pArgv[2], pArgv[3], pArgv[4]);
    } else {
        AmbaPrint_PrintStr5(pFmt, NULL, NULL, NULL, NULL, NULL);
    }
}

/**
 *  @private
 *  sfvar print ex
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ul value
 *  @param[in] var_base value number base
 *  @note this function is intended for internal use only
 */
void sfvar_print_ex(const char *pStr, UINT32 var_ul, UINT32 var_base)
{
    char str[11];
    UINT32 Argc = 2U;
    const char *Argv[2] = { pStr, str };

    sfvar_utoa(var_ul, str, var_base, 8U, (UINT32) SFVAR_LEADING_NONE);

    sfvar_print("%s (%s)", &Argc, Argv);
}

/**
 *  @private
 *  sfvar print64 ex
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ull value
 *  @param[in] var_base value number base
 *  @note this function is intended for internal use only
 */
void sfvar_print64_ex(const char *pStr, UINT64 var_ull, UINT32 var_base)
{
    char str[19];
    UINT32 Argc = 2U;
    const char *Argv[2] = { pStr, str };

    sfvar_ultoa(var_ull, str, var_base, 16U, (UINT32) SFVAR_LEADING_NONE);

    sfvar_print("%s (%s)", &Argc, Argv);
}

/**
 *  @private
 *  sfvar print ex2
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ul value0
 *  @param[in] var0_base value0 number base
 *  @param[in] var1_ul value1
 *  @param[in] var1_base value1 number base
 *  @note this function is intended for internal use only
 */
void sfvar_print_ex2(const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base)
{
    char str[2][11];
    UINT32 Argc = 3U;
    const char *Argv[3] = { pStr, str[0], str[1] };

    sfvar_utoa(var0_ul, str[0], var0_base, 8U, (UINT32) SFVAR_LEADING_NONE);
    sfvar_utoa(var1_ul, str[1], var1_base, 8U, (UINT32) SFVAR_LEADING_NONE);

    sfvar_print("%s (%s) = %s", &Argc, Argv);
}

/**
 *  @private
 *  sfvar print64 ex2
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ull value0
 *  @param[in] var0_base value0 number base
 *  @param[in] var1_ull value1
 *  @param[in] var1_base value1 number base
 *  @note this function is intended for internal use only
 */
void sfvar_print64_ex2(const char *pStr, UINT64 var0_ull, UINT32 var0_base, UINT64 var1_ull, UINT32 var1_base)
{
    char str[2][19];
    UINT32 Argc = 3U;
    const char *Argv[3] = { pStr, str[0], str[1] };

    sfvar_ultoa(var0_ull, str[0], var0_base, 16U, (UINT32) SFVAR_LEADING_NONE);
    sfvar_ultoa(var1_ull, str[1], var1_base, 16U, (UINT32) SFVAR_LEADING_NONE);

    sfvar_print("%s (%s) = %s", &Argc, Argv);
}

/**
 *  @private
 *  Amba monitor framework command print ex
 *  @param[in] Id print id
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ul value
 *  @param[in] var_base value number base
 *  @note this function is intended for internal use only
 */
void AmbaMonFrwCmd_PrintEx(UINT32 Id, const char *pStr, UINT32 var_ul, UINT32 var_base)
{
    if ((AmbaMonFrwCmdVar_PrintFlag & Id) > 0U) {
        sfvar_print_ex(pStr, var_ul, var_base);
    }
}

/**
 *  @private
 *  Amba monitor framework command print64 ex
 *  @param[in] Id print id
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ul value
 *  @param[in] var_base value number base
 *  @note this function is intended for internal use only
 */
void AmbaMonFrwCmd_Print64Ex(UINT32 Id, const char *pStr, UINT64 var_ull, UINT32 var_base)
{
    if ((AmbaMonFrwCmdVar_PrintFlag & Id) > 0U) {
        sfvar_print64_ex(pStr, var_ull, var_base);
    }
}

/**
 *  @private
 *  Amba monitor framework command print ex2
 *  @param[in] Id print id
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ul value0
 *  @param[in] var0_base value0 number base
 *  @param[in] var1_ul value1
 *  @param[in] var1_base value1 number base
 *  @note this function is intended for internal use only
 */
void AmbaMonFrwCmd_PrintEx2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base)
{
    if ((AmbaMonFrwCmdVar_PrintFlag & Id) > 0U) {
        sfvar_print_ex2(pStr, var0_ul, var0_base, var1_ul, var1_base);
    }
}

/**
 *  @private
 *  Amba monitor framework command print64 ex2
 *  @param[in] Id print id
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ull value0
 *  @param[in] var0_base value0 number base
 *  @param[in] var1_ull value1
 *  @param[in] var1_base value1 number base
 *  @note this function is intended for internal use only
 */
void AmbaMonFrwCmd_Print64Ex2(UINT32 Id, const char *pStr, UINT64 var0_ull, UINT32 var0_base, UINT64 var1_ull, UINT32 var1_base)
{
    if ((AmbaMonFrwCmdVar_PrintFlag & Id) > 0U) {
        sfvar_print64_ex2(pStr, var0_ull, var0_base, var1_ull, var1_base);
    }
}

/**
 *  @private
 *  Amba monitor framework command print
 *  @param[in] Id print id
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ul value
 *  @note this function is intended for internal use only
 */
void AmbaMonFrwCmd_Print(UINT32 Id, const char *pStr, UINT32 var_ul)
{
    if ((AmbaMonFrwCmdVar_PrintFlag & Id) > 0U) {
        sfvar_print_ex(pStr, var_ul, 10U);
    }
}

/**
 *  @private
 *  Amba monitor framework command print2
 *  @param[in] Id print id
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ul value0
 *  @param[in] var1_ul value1
 *  @note this function is intended for internal use only
 */
void AmbaMonFrwCmd_Print2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var1_ul)
{
    if ((AmbaMonFrwCmdVar_PrintFlag & Id) > 0U) {
        sfvar_print_ex2(pStr, var0_ul, 10U, var1_ul, 10U);
    }
}

/**
 *  @private
 *  Amba monitor framework command print
 *  @param[in] Id print id
 *  @param[in] pStr pointer to the string
 *  @note this function is intended for internal use only
 */
void AmbaMonFrwCmd_PrintStr(UINT32 Id, const char *pStr)
{
    if ((AmbaMonFrwCmdVar_PrintFlag & Id) > 0U) {
        sfvar_print(pStr, NULL, NULL);
    }
}

/**
 *  @private
 *  Amba monitor framework command print enable
 *  @param[in] Flag print flag
 *  @note this function is intended for internal use only
 */
void AmbaMonFrwCmd_PrintEnable(UINT32 Flag)
{
    AmbaMonFrwCmdVar_PrintFlag = Flag;
}

