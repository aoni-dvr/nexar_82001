/**
 *  @file AmbaMonVar.c
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
 *  @details Amba Monitor Variable
 *
 */

#include "AmbaTypes.h"
#include "AmbaPrint.h"

#include "AmbaMonVar.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

static UINT32 AmbaMonVar_PrintFlag = 0xFFFFFFFFU;

/**
 *  @private
 *  svar string length
 *  @param[in] pString pointer to the string
 *  @return string length
 *  @note this function is intended for internal use only
 */
UINT32 svar_strlen(const char *pString)
{
    UINT32 Length = 0U;

    if (pString != NULL) {
        while (pString[Length] != '\0') { Length++; }
    }

    return Length;
}

/**
 *  @private
 *  svar string compare
 *  @param[in] pString1 pointer to the string 1
 *  @param[in] pString2 pointer to the string 2
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 svar_strcmp(const char *pString1, const char *pString2)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 Length1;
    UINT32 Length2;

    if ((pString1 != NULL) &&
        (pString2 != NULL)) {
        /* string length */
        Length1 = svar_strlen(pString1);
        Length2 = svar_strlen(pString2);
        /* compare */
        if ((Length1 > 0U) && (Length2 > 0U)) {
            if (Length1 == Length2) {
                /* length same */
                for (i = 0; i < Length1; i++) {
                    /* compare char */
                    if (pString1[i] != pString2[i]) {
                        break;
                    }
                }
                if (i != Length1) {
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
 *  svar string cat
 *  @param[in,out] pString1 pointer to the des string
 *  @param[in] pString2 pointer to the src string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 svar_strcat(char *pString1, const char *pString2)
{
    UINT32 RetCode = OK_UL;
    UINT32 i;

    UINT32 Length1;
    UINT32 Length2;

    if ((pString1 != NULL) &&
        (pString2 != NULL)) {
        /* string length */
        Length1 = svar_strlen(pString1);
        Length2 = svar_strlen(pString2);
        /* cat */
        for (i = 0U; i < Length2; i++) {
            pString1[Length1+i] = pString2[i];
        }
        pString1[Length1+i] = '\0';
    } else {
        /* null */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  svar ultoa
 *  @param[in] Vaule value to be transfered
 *  @param[in] pString pointer to the string buffer
 *  @param[in] Base number base
 *  @param[in] Count char count
 *  @param[in] Flag string flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
void svar_ultoa(UINT64 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag)
{
    static const char ImageVar_NumString[] = "0123456789ABCDEF";

    UINT32 i, j;

    UINT64 uValue = Value;

    char iString[32];
    UINT32 StringCnt = 0U;

    for (i = 0U; i < 18U; i++) {
        iString[i] = ImageVar_NumString[uValue%Base];
        StringCnt++;
        uValue = uValue/Base;
        if (uValue == 0U) {
            break;
        }
    }

    if (Count >= StringCnt) {
        /* normal */
        switch (Flag) {
            case (UINT32) SVAR_LEADING_ZERO:
                /* zero leading */
                for (i = 0U; i < (Count - StringCnt); i++) {
                    pString[i] = '0';
                }
                /* num */
                for (j = i; j < Count; j++) {
                    pString[j] = iString[StringCnt-((j-i)+1U)];
                }
                pString[j] = '\0';
                break;
            case (UINT32) SVAR_LEADING_SPACE:
                /* space leading */
                for (i = 0U; i < (Count - StringCnt); i++) {
                    pString[i] = ' ';
                }
                /* num */
                for (j = i; j < Count; j++) {
                    pString[j] = iString[StringCnt-((j-i)+1U)];
                }
                pString[j] = '\0';
                break;
            case (UINT32) SVAR_LEADING_NONE:
            default:
                /* num */
                for (i = 0U; i < StringCnt; i++) {
                    pString[i] = iString[StringCnt-(i+1U)];
                }
                pString[i] = '\0';
                break;
        }
    } else {
        /* overflow */
        for (i = 0U; i < Count; i++) {
            pString[i] = '#';
        }
        pString[i] = '\0';
    }
}

/**
 *  @private
 *  svar utoa
 *  @param[in] Value value to be transfered
 *  @param[in] pString pointer to the string buffer
 *  @param[in] Base number base
 *  @param[in] Count char count
 *  @param[in] Flag string flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
void svar_utoa(UINT32 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag)
{
    svar_ultoa((UINT64) Value, pString, Base, Count, Flag);
}

/**
 *  @private
 *  svar lltoa
 *  @param[in] Value value to be transfered
 *  @param[in] pString pointer to the string buffer
 *  @param[in] Base number base
 *  @param[in] Count char count
 *  @param[in] Flag string flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
void svar_lltoa( INT64 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag)
{
     INT64 iValue = Value;
    UINT32 ShiftCnt = 0U;
    UINT64 uValue;

    if (iValue < 0L) {
        pString[0] = '-';
        ShiftCnt++;
        iValue = (-1)*iValue;
    }

    uValue = (iValue > 0L) ? (UINT64) iValue : 0U;

    svar_ultoa(uValue, &(pString[ShiftCnt]), Base, Count-ShiftCnt, Flag);
}

/**
 *  @private
 *  svar itoa
 *  @param[in] Value value to be transfered
 *  @param[in] pString pointer to the string buffer
 *  @param[in] Base number base
 *  @param[in] Count char count
 *  @param[in] Flag string flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
void svar_itoa( INT32 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag)
{
    svar_lltoa((INT64) Value, pString, Base, Count, Flag);
}

/**
 *  @private
 *  svar print
 *  @param[in] pFmt print format
 *  @param[in] Argc pointer to the arg count
 *  @param[in] Argv pointer to the arg context pointer
 *  @note this function is intended for internal use only
 */
void svar_print(const char *pFmt, const UINT32 *Argc, const char * const *Argv)
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
 *  svar print ex
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ul value
 *  @param[in] var_base munber base
 *  @note this function is intended for internal use only
 */
void svar_print_ex(const char *pStr, UINT32 var_ul, UINT32 var_base)
{
    char str[11];
    UINT32 Argc = 2U;
    const char *Argv[2] = { pStr, str };

    svar_utoa(var_ul, str, var_base, 8U, (UINT32) SVAR_LEADING_NONE);

    svar_print("%s (%s)", &Argc, Argv);
}

/**
 *  @private
 *  svar print64 ex
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ull value
 *  @param[in] var_base munber base
 *  @note this function is intended for internal use only
 */
void svar_print64_ex(const char *pStr, UINT64 var_ull, UINT32 var_base)
{
    char str[19];
    UINT32 Argc = 2U;
    const char *Argv[2] = { pStr, str };

    svar_ultoa(var_ull, str, var_base, 16U, (UINT32) SVAR_LEADING_NONE);

    svar_print("%s (%s)", &Argc, Argv);
}

/**
 *  @private
 *  svar print ex2
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ul value 0
 *  @param[in] var0_base munber base for value 0
 *  @param[in] var1_ul value 1
 *  @param[in] var1_base number base for value 1
 *  @note this function is intended for internal use only
 */
void svar_print_ex2(const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base)
{
    char str[2][11];
    UINT32 Argc = 3U;
    const char *Argv[3] = { pStr, str[0], str[1] };

    svar_utoa(var0_ul, str[0], var0_base, 8U, (UINT32) SVAR_LEADING_NONE);
    svar_utoa(var1_ul, str[1], var1_base, 8U, (UINT32) SVAR_LEADING_NONE);

    svar_print("%s (%s) = %s", &Argc, Argv);
}

/**
 *  @private
 *  svar print64 ex2
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ull value 0
 *  @param[in] var0_base munber base for value 0
 *  @param[in] var1_ull value 1
 *  @param[in] var1_base number base for value 1
 *  @note this function is intended for internal use only
 */
void svar_print64_ex2(const char *pStr, UINT64 var0_ull, UINT32 var0_base, UINT64 var1_ull, UINT32 var1_base)
{
    char str[2][19];
    UINT32 Argc = 3U;
    const char *Argv[3] = { pStr, str[0], str[1] };

    svar_ultoa(var0_ull, str[0], var0_base, 16U, (UINT32) SVAR_LEADING_NONE);
    svar_ultoa(var1_ull, str[1], var1_base, 16U, (UINT32) SVAR_LEADING_NONE);

    svar_print("%s (%s) = %s", &Argc, Argv);
}

/**
 *  @private
 *  Amba monitor print ex
 *  @param[in] Id print context id
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ul value
 *  @param[in] var_base munber base for value
 *  @note this function is intended for internal use only
 */
void AmbaMonPrintEx(UINT32 Id, const char *pStr, UINT32 var_ul, UINT32 var_base)
{
    if ((AmbaMonVar_PrintFlag & Id) > 0U) {
        svar_print_ex(pStr, var_ul, var_base);
    }
}

/**
 *  @private
 *  Amba monitor print64 ex
 *  @param[in] Id print context id
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ull value
 *  @param[in] var_base munber base for value
 *  @note this function is intended for internal use only
 */
void AmbaMonPrint64Ex(UINT32 Id, const char *pStr, UINT64 var_ull, UINT32 var_base)
{
    if ((AmbaMonVar_PrintFlag & Id) > 0U) {
        svar_print64_ex(pStr, var_ull, var_base);
    }
}

/**
 *  @private
 *  Amba monitor print ex2
 *  @param[in] Id print context id
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ul value 0
 *  @param[in] var0_base munber base for value 0
 *  @param[in] var1_ul value 1
 *  @param[in] var1_base number base for value 1
 *  @note this function is intended for internal use only
 */
void AmbaMonPrintEx2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base)
{
    if ((AmbaMonVar_PrintFlag & Id) > 0U) {
        svar_print_ex2(pStr, var0_ul, var0_base, var1_ul, var1_base);
    }
}

/**
 *  @private
 *  Amba monitor print64 ex2
 *  @param[in] Id print context id
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ull value 0
 *  @param[in] var0_base munber base for value 0
 *  @param[in] var1_ull value 1
 *  @param[in] var1_base number base for value 1
 *  @note this function is intended for internal use only
 */
void AmbaMonPrint64Ex2(UINT32 Id, const char *pStr, UINT64 var0_ull, UINT32 var0_base, UINT64 var1_ull, UINT32 var1_base)
{
    if ((AmbaMonVar_PrintFlag & Id) > 0U) {
        svar_print64_ex2(pStr, var0_ull, var0_base, var1_ull, var1_base);
    }
}

/**
 *  @private
 *  Amba monitor print
 *  @param[in] Id print context id
 *  @param[in] pStr pointer to the string
 *  @param[in] var_ul value
 *  @note this function is intended for internal use only
 */
void AmbaMonPrint(UINT32 Id, const char *pStr, UINT32 var_ul)
{
    if ((AmbaMonVar_PrintFlag & Id) > 0U) {
        svar_print_ex(pStr, var_ul, 10U);
    }
}

/**
 *  @private
 *  Amba monitor print 2
 *  @param[in] Id print context id
 *  @param[in] pStr pointer to the string
 *  @param[in] var0_ul value 0
 *  @param[in] var1_ul value 1
 *  @note this function is intended for internal use only
 */
void AmbaMonPrint2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var1_ul)
{
    if ((AmbaMonVar_PrintFlag & Id) > 0U) {
        svar_print_ex2(pStr, var0_ul, 10U, var1_ul, 10U);
    }
}

/**
 *  @private
 *  Amba monitor print str
 *  @param[in] Id print context id
 *  @param[in] pStr pointer to the string
 *  @note this function is intended for internal use only
 */
void AmbaMonPrintStr(UINT32 Id, const char *pStr)
{
    if ((AmbaMonVar_PrintFlag & Id) > 0U) {
        svar_print(pStr, NULL, NULL);
    }
}

/**
 *  @private
 *  Amba monitor print enable
 *  @param[in] Flag print context flag
 *  @note this function is intended for internal use only
 */
void AmbaMonPrint_Enable(UINT32 Flag)
{
    AmbaMonVar_PrintFlag = Flag;
}

/**
 *  @private
 *  Amba monitor print flush
 *  @note this function is intended for internal use only
 */
void AmbaMonPrint_Flush(void)
{
    AmbaPrint_Flush();
}
