/**
 *  @file AmbaUtility.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Ambarella Utility functions.
 *
 */
#include "osal_linux.h"

/******************************************************************************/
/*                                  AmbaUtility.h                                                                                   */
/******************************************************************************/
static char num_base16_full[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static char UInt32ToDigit(UINT32 Value)
{
    char cRet = '0';

    if (Value < 16U) {
        cRet = num_base16_full[Value];
    }
    return cRet;
}

UINT32 OSAL_Int32ToStr(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    INT32  t = Value;
    char *pWorkChar1, *pWorkChar2 = pBuffer, Tmp;
    UINT32 idx1 = 0, idx2 = 0, swap_len;
    UINT32 Base = Radix, ut, uTmp;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (t == 0) {
        pWorkChar2[idx2] = '0';
        idx2++;
        pWorkChar2[idx2] = '\0';         /* attach the null terminator */
        StrLength = 1;
    } else {
        //----------------------------------------------------------
        if ((t < 0) && (Base == 10U)) {
            t = -t;
            pWorkChar2[idx2] = '-';
            idx2++;
            pWorkChar2[idx2 + 1U] = '\0';   /* attach the null terminator */
        }

        ut = (UINT32)t;
        pWorkChar1 = pWorkChar2;
        idx1 = idx2;

        while (ut > 0U) {
            uTmp = ut % Base;
            pWorkChar1[idx1] = UInt32ToDigit(uTmp);
            idx1++;
            if (idx1 >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= Base;
        }
        pWorkChar1[idx1] = '\0';             /* attach the null terminator */
        StrLength = idx1;

        // change the order of the digits
        swap_len = (idx1 - idx2) / 2U;  /* n = lenght of the string / 2 */
        while (swap_len > 0U) {
            if (idx1 > 0U) {
                idx1--;
            }
            Tmp = pWorkChar1[idx1];
            pWorkChar1[idx1] = pWorkChar2[idx2];
            pWorkChar2[idx2] = Tmp;
            idx2++;
            swap_len--;
        }
    }

    /* make sure at least a \0 is added.*/
    pBuffer[BufferSize-1U] = '\0';
    return StrLength;
}
EXPORT_SYMBOL(OSAL_Int32ToStr);

UINT32 OSAL_UInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    UINT32 ut = Value, uTmp;
    char *pWorkChar1, *pWorkChar2 = pBuffer, Tmp;
    UINT32 idx1 = 0, idx2 = 0, swap_len;
    UINT32 Base = Radix;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (ut == 0U) {
        pWorkChar2[idx2] = '0';
        idx2++;
        pWorkChar2[idx2] = '\0';         /* attach the null terminator */
        StrLength = 1;
    } else {
        pWorkChar1 = pWorkChar2;
        idx1 = idx2;

        while (ut > 0U) {
            uTmp = ut % Base;
            pWorkChar1[idx1] = UInt32ToDigit(uTmp);
            idx1++;
            if (idx1 >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= Base;
        }
        pWorkChar1[idx1] = '\0';             /* attach the null terminator */
        StrLength = idx1;

        if (idx1 > 0U) {
            // change the order of the digits
            swap_len = idx1 / 2U;  /* n = lenght of the string / 2 */
            while (swap_len > 0U) {
                if (idx1 > 0U) {
                    idx1--;
                }
                Tmp = pWorkChar1[idx1];
                pWorkChar1[idx1] = pWorkChar2[idx2];
                pWorkChar2[idx2] = Tmp;
                idx2++;
                swap_len--;
            }
        }
    }

    /* make sure at least a \0 is added.*/
    if (BufferSize > 0U) {
        /* make sure at least a \0 is added.*/
        pBuffer[BufferSize-1U] = '\0';
    }
    return StrLength;
}
EXPORT_SYMBOL(OSAL_UInt32ToStr);

UINT32 OSAL_Int64ToStr(char *pBuffer, UINT32 BufferSize, INT64 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    INT64  t = Value;
    char *pWorkChar1, *pWorkChar2 = pBuffer, Tmp;
    UINT32 idx1 = 0, idx2 = 0, swap_len;
    UINT32 Base = Radix;
    UINT64 ut, uTmp;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (t == 0) {
        pWorkChar2[idx2] = '0';
        idx2++;
        pWorkChar2[idx2] = '\0';         /* attach the null terminator */
        StrLength = 1;
    } else {
        //----------------------------------------------------------
        if ((t < 0) && (Base == 10U)) {
            t = -t;
            pWorkChar2[idx2] = '-';
            idx2++;
            pWorkChar2[idx2 + 1U] = '\0';   /* attach the null terminator */
        }

        ut = (UINT64)t;
        pWorkChar1 = pWorkChar2;
        idx1 = idx2;

        while (ut > 0U) {
            uTmp = ut % (UINT64)Base;
            pWorkChar1[idx1] = UInt32ToDigit((UINT32)uTmp);
            idx1++;
            if (idx1 >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= (UINT64)Base;
        }
        pWorkChar1[idx1] = '\0';             /* attach the null terminator */
        StrLength = idx1;

        // change the order of the digits
        swap_len = (idx1 - idx2) / 2U;  /* n = lenght of the string / 2 */
        while (swap_len > 0U) {
            if (idx1 > 0U) {
                idx1--;
            }
            Tmp = pWorkChar1[idx1];
            pWorkChar1[idx1] = pWorkChar2[idx2];
            pWorkChar2[idx2] = Tmp;
            idx2++;
            swap_len--;
        }
    }

    /* make sure at least a \0 is added.*/
    if (BufferSize > 0U) {
        /* make sure at least a \0 is added.*/
        pBuffer[BufferSize-1U] = '\0';
    }

    return StrLength;
}
EXPORT_SYMBOL(OSAL_Int64ToStr);

UINT32 OSAL_UInt64ToStr(char *pBuffer, UINT32 BufferSize, UINT64 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    UINT64 ut = Value, uTmp;
    char *pWorkChar1, *pWorkChar2 = pBuffer, Tmp;
    UINT32 idx1 = 0, idx2 = 0, swap_len;
    UINT32 Base = Radix;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (ut == 0U) {
        pWorkChar2[idx2] = '0';
        idx2++;
        pWorkChar2[idx2] = '\0';         /* attach the null terminator */
        StrLength = 1;
    } else {
        pWorkChar1 = pWorkChar2;
        idx1 = idx2;

        while (ut > 0U) {
            uTmp = ut % (UINT64)Base;
            pWorkChar1[idx1] = UInt32ToDigit((UINT32)uTmp);
            idx1++;
            if (idx1 >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= (UINT64)Base;
        }
        pWorkChar1[idx1] = '\0';             /* attach the null terminator */
        StrLength = idx1;

        // change the order of the digits
        swap_len = idx1 / 2U;  /* n = lenght of the string / 2 */
        while (swap_len > 0U) {
            if (idx1 > 0U) {
                idx1--;
            }
            Tmp = pWorkChar1[idx1];
            pWorkChar1[idx1] = pWorkChar2[idx2];
            pWorkChar2[idx2] = Tmp;
            idx2++;
            swap_len--;
        }
    }

    if (BufferSize > 0U) {
        /* make sure at least a \0 is added.*/
        pBuffer[BufferSize-1U] = '\0';
    }
    return StrLength;
}
EXPORT_SYMBOL(OSAL_UInt64ToStr);

static UINT32 OSAL_IsDigit(INT32 Value)
{
    UINT32 ret = 0;
    if ((Value >= (INT32)'0') && (Value <= (INT32)'9')) {
        ret = 1;
    }
    return ret;
}
static UINT32 DoesStringContain(const char *pSource, char TargetChar)
{
    UINT32 uRet = 0;
    if (pSource != NULL) {
        SIZE_t i;
        SIZE_t Length = OSAL_StringLength(pSource);
        for (i = 0; i < Length; i++) {
            if (pSource[i] == TargetChar) {
                uRet = 1;
                break;
            }
        }
    }
    return uRet;
}

static void StringPrint_PreProc(const char *pFmtString, UINT32 *idx, UINT32 is_return, char *TmpBuffer, UINT32 BufferSize, UINT32 *width)
{

    for (; pFmtString[*idx] != '\0'; ) {
        if ((DoesStringContain("bcdefgilopPrRsuxX%", pFmtString[*idx]) == 1U) || (is_return == 1U)) {
            break;
        } else if ((pFmtString[*idx] >= '1') && (pFmtString[*idx] <= '9')) {
            UINT32 idxStart = *idx;
            UINT32 idxEnd = idxStart;
            UINT32 w;
            for (; (OSAL_IsDigit((INT32)pFmtString[idxEnd]) != 0U); idxEnd++) {
                /* make misra happy */
            }
            //strncpy(tmp, &FmtString[idxStart], idxEnd - idxStart);
            OSAL_StringCopy(TmpBuffer, BufferSize, &pFmtString[idxStart]);
            TmpBuffer[idxEnd - idxStart] = '\0';
            if (OSAL_StringToUInt32(TmpBuffer, &w) == 0U) {
                *width = w;
            } else {
                *width = 0;
            }
            //width = strtoul(tmp, NULL, 0);
            *idx = idxEnd - 1U;
        } else {
            /* Others are not supported */
        }
        *idx += 1U;
    }
}


static void StringPrintUInt32_Padding(char *pBuffer, UINT32 BufferSize, UINT32 width, UINT32 string_length, UINT32 *nRetIdx, UINT32 *is_return)
{
    UINT32 i;
    if (width > string_length) {
        for (i = 0; i < (width - string_length); i++) {
            pBuffer[*nRetIdx] = '0';
            *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
                break;
            }
        }
        if (*is_return == 0U) {
            pBuffer[*nRetIdx] = '\0';
        }
    }
}

static void StringPrintUInt32_ParseArg(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 idx, INT32 *base, UINT32 *nRetIdx, UINT32 *is_return, UINT32 *width)
{
    if ((pFmtString[idx] == 'd') || (pFmtString[idx] == 'i')) {
        *base = 10;
    } else if (pFmtString[idx] == 'u') {
        *base = 10;
    } else if ((pFmtString[idx] == 'x') || (pFmtString[idx] == 'X')) {
        *base = 16;
    } else if((pFmtString[idx] == 'p') || (pFmtString[idx] == 'P')) {
        *base = 16;
        if (pFmtString[idx] == 'p') {
            pBuffer[*nRetIdx] = '0';
            *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
            } else {
                pBuffer[*nRetIdx] = 'x';
                *nRetIdx += 1U;
                if (*nRetIdx == (BufferSize-1U)) {
                    *is_return = 1;
                }
            }
        }
        *width = 8;
    } else if((pFmtString[idx] == 'r') || (pFmtString[idx] == 'R')) {
        *base = 16;
        if (pFmtString[idx] == 'r') {
            pBuffer[*nRetIdx] = '0';
            *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
            } else {
                pBuffer[*nRetIdx] = 'x';
                *nRetIdx += 1U;
                if (*nRetIdx == (BufferSize-1U)) {
                    *is_return = 1;
                }
            }
        }
        *width = 8;
    } else if ((pFmtString[idx] == 'o') || (pFmtString[idx] == 'b')) {
        /* not supported, just use base 10 */
        *base = 10;
    } else {
        /* make misra happy */
    }
}

static UINT32 StringPrintUInt32_CheckArg(UINT32 ArgIdx, UINT32 Count, const UINT32 *pArgs)
{
    UINT32  ArgValue;
    if (ArgIdx >= Count) {
        ArgValue = 0;
    } else {
        ArgValue = pArgs[ArgIdx];
    }
    return ArgValue;
}

static UINT32 StringPrintUInt32_ValueToString(char *tmp, UINT32 BufferSize, INT32 base, UINT32 ArgValue)
{
    UINT32 string_length;
    if (base == 16) {
        string_length = OSAL_UInt32ToStr(tmp, BufferSize, ArgValue, 16);
    } else {
        string_length = OSAL_UInt32ToStr(tmp, BufferSize, ArgValue, 10);
    }
    return string_length;
}

UINT32 OSAL_StringPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs)
{
    char tmp[40];
    INT32 base = 10;
    UINT32 width, string_length;
    UINT32 idx = 0, nRetIdx = 0, argIdx = 0, ArgValue;
    UINT32 is_return = 0;

    if ((pBuffer == NULL) || (pFmtString == NULL) || (pArgs == NULL) || (BufferSize == 0U)) {
        // no action
    } else {
        for (; pFmtString[idx] != '\0';) {
            if (is_return == 1U) {
                break;
            }
            if (pFmtString[idx] == '%') {
                idx++;
                width = 0;

                StringPrint_PreProc(pFmtString, &idx, is_return, tmp, 40, &width);

                if (pFmtString[idx] == '%') {
                    pBuffer[nRetIdx] = '%';
                    nRetIdx++;
                    pBuffer[nRetIdx] = '\0';
                    if (nRetIdx == (BufferSize-1U)) {
                        is_return = 1;
                        continue;
                    }
                } else if (pFmtString[idx] == 's') {
                    /* not supported, just print '(null)' */
                    argIdx++;
                    pBuffer[nRetIdx] = '\0';
                    OSAL_StringAppend(pBuffer, BufferSize, "(null)");
                    if ((nRetIdx + 6U) > (BufferSize - 1U)) {
                        nRetIdx = BufferSize - 1U;
                        is_return = 1;
                        continue;
                    } else {
                        nRetIdx += 6U;
                    }
                } else if (pFmtString[idx] == 'c') {

                    ArgValue = StringPrintUInt32_CheckArg(argIdx, Count, pArgs);

                    argIdx++;
                    pBuffer[nRetIdx] = (char)ArgValue;
                    nRetIdx++;
                    pBuffer[nRetIdx] = '\0';
                    if (nRetIdx == (BufferSize-1U)) {
                        is_return = 1;
                        continue;
                    }
                } else {
                    if (pFmtString[idx] == 'l') {
                        idx++;
                        if (pFmtString[idx] == 'l') {
                            idx++;
                        }
                    }

                    if (DoesStringContain("bdiopPrRxXu", pFmtString[idx]) == 1U) {

                        StringPrintUInt32_ParseArg(pBuffer, BufferSize, pFmtString, idx, &base, &nRetIdx, &is_return, &width);
                        if (is_return == 1U) {
                            continue;
                        }

                        ArgValue = StringPrintUInt32_CheckArg(argIdx, Count, pArgs);

                        argIdx++;
                        pBuffer[nRetIdx] = '\0';

                        string_length = StringPrintUInt32_ValueToString(tmp, 40, base, ArgValue);

                        // padding spaces if needed
                        StringPrintUInt32_Padding(pBuffer, BufferSize, width, string_length, &nRetIdx, &is_return);
                        if (is_return == 1U) {
                            continue;
                        }

                        OSAL_StringAppend(pBuffer, BufferSize, &tmp[0]);

                        if ((nRetIdx + string_length) > (BufferSize-1U)) {
                            nRetIdx = BufferSize-1U;
                            is_return = 1;
                            continue;
                        } else {
                            nRetIdx += string_length;
                        }

                        //if (pFmtString[idx] == 'X') {
                        //strtoupper(d);
                        //}
                    }
                }
                idx++;
            } else {
                pBuffer[nRetIdx] = pFmtString[idx];
                idx++;
                nRetIdx++;
                if (nRetIdx == (BufferSize-1U)) {
                    is_return = 1;
                    continue;
                }
            }
        }
        pBuffer[nRetIdx] = '\0';
    }
    return nRetIdx;
}
EXPORT_SYMBOL(OSAL_StringPrintUInt32);

static INT32 StringPrintInt32_CheckArg(UINT32 ArgIdx, UINT32 Count, const INT32 *pArgs)
{
    INT32  ArgValue;
    if (ArgIdx >= Count) {
        ArgValue = 0;
    } else {
        ArgValue = pArgs[ArgIdx];
    }
    return ArgValue;
}

static void StringPrintInt32_Append(char *pBuffer, UINT32 BufferSize, const char *tmp, UINT32 width, UINT32 *string_length)
{
    if (tmp[0] == '-') {
        if (width > *string_length) {
            OSAL_StringAppend(pBuffer, BufferSize, &tmp[1]);
            *string_length -= 1U;
        } else {
            OSAL_StringAppend(pBuffer, BufferSize, &tmp[0]);
        }
    } else {
        OSAL_StringAppend(pBuffer, BufferSize, &tmp[0]);
    }
}

static void StringPrintInt32_ParseArg(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 idx, INT32 *base, UINT32 *nRetIdx, UINT32 *is_return, UINT32 *width)
{
    if ((pFmtString[idx] == 'd') || (pFmtString[idx] == 'i')) {
        *base = -10;
    } else if (pFmtString[idx] == 'u') {
        *base = 10;
    } else if ((pFmtString[idx] == 'x') || (pFmtString[idx] == 'X')) {
        *base = 16;
    } else if((pFmtString[idx] == 'p') || (pFmtString[idx] == 'P')) {
        *base = 16;
        if (pFmtString[idx] == 'p') {
            pBuffer[*nRetIdx] = '0';
            *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
            } else {
                pBuffer[*nRetIdx] = 'x';
                *nRetIdx += 1U;
                if (*nRetIdx == (BufferSize-1U)) {
                    *is_return = 1;
                }
            }
        }
        *width = 8;
    } else if((pFmtString[idx] == 'r') || (pFmtString[idx] == 'R')) {
        *base = 16;
        if (pFmtString[idx] == 'r') {
            pBuffer[*nRetIdx] = '0';
            *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
            } else {
                pBuffer[*nRetIdx] = 'x';
                *nRetIdx += 1U;
                if (*nRetIdx == (BufferSize-1U)) {
                    *is_return = 1;
                }
            }
        }
        *width = 8;
    } else if ((pFmtString[idx] == 'o') || (pFmtString[idx] == 'b')) {
        /* not supported, just use base 10 */
        *base = 10;
    } else {
        /* make misra happy */
    }
}

static void StringPrintInt32_Padding(char *pBuffer, UINT32 BufferSize, UINT32 width, UINT32 string_length, UINT32 *nRetIdx, UINT32 *is_return, const char *tmp)
{
    UINT32 i;
    if (width > string_length) {
        if (tmp[0] == '-') {
            // should perform -00001, not 0000-1
            pBuffer[*nRetIdx] = '-';
            *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
            }
        }
        if (*is_return == 0U) {
            for (i = 0; i < (width - string_length); i++) {
                pBuffer[*nRetIdx] = '0';
                *nRetIdx += 1U;
                if (*nRetIdx == (BufferSize-1U)) {
                    *is_return = 1;
                    break;
                }
            }
        }
        if (*is_return == 0U) {
            pBuffer[*nRetIdx] = '\0';
        }
    }
}

static UINT32 StringPrintInt32_ValueToString(char *tmp, UINT32 BufferSize, INT32 base, INT32 ArgValue)
{
    UINT32 string_length;
    if (base == -10) {
        string_length = OSAL_Int32ToStr(tmp, BufferSize, ArgValue, 10);
    } else if (base == 16) {
        string_length = OSAL_UInt32ToStr(tmp, BufferSize, (UINT32)ArgValue, 16);
    } else {
        string_length = OSAL_UInt32ToStr(tmp, BufferSize, (UINT32)ArgValue, 10);
    }
    return string_length;
}

UINT32 OSAL_StringPrintInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const INT32 *pArgs)
{
    char tmp[40];
    INT32 base = 10;
    UINT32 width, string_length;
    UINT32 idx = 0, nRetIdx = 0, argIdx = 0;
    INT32  ArgValue;
    UINT32 is_return = 0;

    if ((pBuffer == NULL) || (pFmtString == NULL) || (pArgs == NULL) || (BufferSize == 0U)) {
        // no action
    } else {
        for (; pFmtString[idx] != '\0';) {
            if (is_return == 1U) {
                break;
            }
            if (pFmtString[idx] == '%') {

                idx++;
                width = 0;

                StringPrint_PreProc(pFmtString, &idx, is_return, tmp, 40, &width);

                if (pFmtString[idx] == '%') {
                    pBuffer[nRetIdx] = '%';
                    nRetIdx++;
                    pBuffer[nRetIdx] = '\0';
                    if (nRetIdx == (BufferSize-1U)) {
                        is_return = 1;
                        continue;
                    }
                } else if (pFmtString[idx] == 's') {
                    /* not supported, just print '(null)' */
                    argIdx++;
                    pBuffer[nRetIdx] = '\0';
                    OSAL_StringAppend(pBuffer, BufferSize, "(null)");
                    if ((nRetIdx + 6U) > (BufferSize - 1U)) {
                        nRetIdx = BufferSize - 1U;
                        is_return = 1;
                        continue;
                    } else {
                        nRetIdx += 6U;
                    }
                } else if (pFmtString[idx] == 'c') {
                    ArgValue = StringPrintInt32_CheckArg(argIdx, Count, pArgs);
                    argIdx++;
                    pBuffer[nRetIdx] = (char)ArgValue;
                    nRetIdx++;
                    pBuffer[nRetIdx] = '\0';
                    if (nRetIdx == (BufferSize-1U)) {
                        is_return = 1;
                        continue;
                    }
                } else {
                    if (pFmtString[idx] == 'l') {
                        idx++;
                        if (pFmtString[idx] == 'l') {
                            idx++;
                        }
                    }
                    if (DoesStringContain("bdiopPrRxXu", pFmtString[idx]) == 1U) {
                        StringPrintInt32_ParseArg(pBuffer, BufferSize, pFmtString, idx, &base, &nRetIdx, &is_return, &width);
                        if (is_return == 1U) {
                            continue;
                        }

                        ArgValue = StringPrintInt32_CheckArg(argIdx, Count, pArgs);
                        argIdx++;
                        pBuffer[nRetIdx] = '\0';

                        // convert INT32 value to String
                        string_length = StringPrintInt32_ValueToString(tmp, 40, base, ArgValue);

                        // padding spaces if needed
                        StringPrintInt32_Padding(pBuffer, BufferSize, width, string_length, &nRetIdx, &is_return, tmp);
                        if (is_return == 1U) {
                            continue;
                        }

                        // append string
                        StringPrintInt32_Append(pBuffer, BufferSize, tmp, width, &string_length);

                        if ((nRetIdx + string_length) > (BufferSize-1U)) {
                            nRetIdx = BufferSize-1U;
                            is_return = 1;
                            continue;
                        } else {
                            nRetIdx += string_length;
                        }

                        //if (pFmtString[idx] == 'X') {
                        //strtoupper(d);
                        //}
                    }
                }
                idx++;
            } else {
                pBuffer[nRetIdx] = pFmtString[idx];
                idx++;
                nRetIdx++;
                if (nRetIdx == (BufferSize-1U)) {
                    is_return = 1;
                    continue;
                }
            }
        }
        pBuffer[nRetIdx] = '\0';
    }
    return nRetIdx;
}
EXPORT_SYMBOL(OSAL_StringPrintInt32);

static void StringPrintStr_Padding(char *pBuffer, UINT32 BufferSize, UINT32 width, UINT32 string_length, UINT32 *nRetIdx, UINT32 *is_return)
{
    UINT32 i;
    if (width > string_length) {
        for (i = 0; i < (width - string_length); i++) {
            pBuffer[*nRetIdx] = ' ';
            *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
                break;
            }
        }
        if (*is_return == 0U) {
            pBuffer[*nRetIdx] = '\0';
        }
    }
}

UINT32 OSAL_StringPrintStr(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const char * const pArgs[])
{
    char tmp[40];
    const char *ArgValue;
    UINT32 width, string_length;
    UINT32 idx = 0, nRetIdx = 0, argIdx = 0;
    UINT32 is_return = 0;

    if ((pBuffer == NULL) || (pFmtString == NULL) || (pArgs == NULL) || (BufferSize == 0U)) {
        // no action
    } else {
        for (; pFmtString[idx] != '\0';) {
            if (is_return == 1U) {
                break;
            }
            if (pFmtString[idx] == '%') {
                idx++;
                width = 0;
                StringPrint_PreProc(pFmtString, &idx, is_return, tmp, 40, &width);

                if (pFmtString[idx] == '%') {
                    pBuffer[nRetIdx] = '%';
                    nRetIdx++;
                    pBuffer[nRetIdx] = '\0';
                    if (nRetIdx == (BufferSize-1U)) {
                        is_return = 1;
                        continue;
                    }
                } else if (pFmtString[idx] == 's') {
                    if (argIdx >= Count) {
                        ArgValue = "";
                    } else {
                        ArgValue = pArgs[argIdx];
                    }
                    pBuffer[nRetIdx] = '\0';
                    string_length = 0;
                    if (ArgValue != NULL) {
                        string_length = OSAL_StringLength(ArgValue);
                        StringPrintStr_Padding(pBuffer, BufferSize, width, string_length, &nRetIdx, &is_return);
                        if (is_return == 1U) {
                            continue;
                        }
                        OSAL_StringAppend(pBuffer, BufferSize, ArgValue);
                    }
                    if ((nRetIdx + string_length) > (BufferSize-1U)) {
                        nRetIdx = BufferSize-1U;
                        is_return = 1;
                        continue;
                    } else {
                        nRetIdx += string_length;
                    }
                    argIdx++;
                } else if (pFmtString[idx] == 'c') {
                    /* not supported */
                } else {
                    if (pFmtString[idx] == 'l') {
                        idx++;
                        if (pFmtString[idx] == 'l') {
                            idx++;
                        }
                    }
                }
                idx++;
            } else {
                pBuffer[nRetIdx] = pFmtString[idx];
                idx++;
                nRetIdx++;
                if (nRetIdx == (BufferSize-1U)) {
                    is_return = 1;
                    continue;
                }
            }
        }
        pBuffer[nRetIdx] = '\0';

    }

    return nRetIdx;
}
EXPORT_SYMBOL(OSAL_StringPrintStr);

void OSAL_StringCopy(char *pDest, SIZE_t DestSize, const char *pSource)
{
    SIZE_t CopyLength;

    if ((pDest == NULL) || (pSource == NULL) || (DestSize == 0U)) {
        // no action
    } else {
        SIZE_t i;
        CopyLength = OSAL_StringLength(pSource);
        if (CopyLength > (DestSize - 1U)) {
            // overflow condition
            CopyLength = (DestSize - 1U);
        }
        for (i = 0; i < CopyLength; i++) {
            pDest[i] = pSource[i];
        }
        pDest[CopyLength] = '\0';
    }
}
EXPORT_SYMBOL(OSAL_StringCopy);

SIZE_t OSAL_StringLength(const char *pString)
{
    SIZE_t Length = 0;
    if (pString == NULL) {
        Length = 0;
    } else {
        while (pString[Length] != '\0') {
            Length++;
        }
    }
    return Length;
}
EXPORT_SYMBOL(OSAL_StringLength);

INT32 OSAL_StringCompare(const char *pString1, const char *pString2, SIZE_t Size)
{
    SIZE_t StringLength1, StringLength2;
    INT32 nRet = 0;
    if ((pString1 == NULL) || (pString2 == NULL) || (Size == 0U)) {
        // return no equal
        nRet = -1;
    } else {
        SIZE_t i;
        StringLength1 = OSAL_StringLength(pString1);
        StringLength2 = OSAL_StringLength(pString2);
        if ((Size > StringLength1) || (Size > StringLength2)) {
            nRet = -1;
        } else {
            for (i = 0; i < Size; i++) {
                if (pString1[i] != pString2[i]) {
                    if (pString1[i] < pString2[i]) {
                        nRet = -1;
                    } else {
                        nRet = 1;
                    }
                    break;
                }
            }
        }
    }
    return nRet;
}
EXPORT_SYMBOL(OSAL_StringCompare);

void OSAL_StringAppend(char *pBuffer, UINT32 BufferSize, const char *pSource)
{
    if ((pBuffer == NULL) || (pSource == NULL) || (BufferSize == 0U)) {
        // no action here
    } else {
        SIZE_t StringLengthDest = OSAL_StringLength(pBuffer);
        SIZE_t StringLengthCopy = OSAL_StringLength(pSource);
        SIZE_t i;

        if (StringLengthCopy >= (BufferSize - 1U)) {
            // buffer overflow, no action here
        } else {
            if ((StringLengthDest + StringLengthCopy) > (BufferSize - 1U)) {
                StringLengthCopy = (BufferSize - StringLengthDest) - 1U;
            }
            for (i = 0; i < StringLengthCopy; i++) {
                pBuffer[i + StringLengthDest] = pSource[i];
            }
            if ((StringLengthDest + StringLengthCopy) <= (BufferSize - 1U)) {
                pBuffer[StringLengthDest + StringLengthCopy] = '\0';
            } else {
                pBuffer[BufferSize - 1U] = '\0';
            }
        }
    }
}
EXPORT_SYMBOL(OSAL_StringAppend);

void OSAL_StringAppendInt32(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix)
{
    char IntString[UTIL_MAX_INT_STR_LEN];

    if ((pBuffer != NULL) && (BufferSize > 0U)) {
        UINT32 Length = OSAL_Int32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Value, Radix);
        if (Length > 0U) {
            OSAL_StringAppend(pBuffer, BufferSize, IntString);
        }
    }
}
EXPORT_SYMBOL(OSAL_StringAppendInt32);

void OSAL_StringAppendUInt32(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    char IntString[UTIL_MAX_INT_STR_LEN];

    if (pBuffer != NULL) {
        UINT32 Length = OSAL_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Value, Radix);
        if (Length > 0U) {
            OSAL_StringAppend(pBuffer, BufferSize, IntString);
        }
    }
}
EXPORT_SYMBOL(OSAL_StringAppendUInt32);

static UINT32 Digit2Uint32(char Digit, UINT32 Base)
{
    UINT32 uRet = 0xFF, i;
    static char num_base10[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    static char num_base16_part[6] = {'a', 'b', 'c', 'd', 'e', 'f'};
    if (Base == 10U) {
        for (i = 0; i < 10U; i++) {
            if (num_base10[i] == Digit) {
                uRet = i;
                break;
            }
        }
    } else if (Base == 16U) {
        for (i = 0; i < 16U; i++) {
            if (num_base16_full[i] == Digit) {
                uRet = i;
                break;
            }
        }

        if (uRet == 0xFFU) {
            // no found, try matching num_base16_part
            for (i = 0; i < 6U; i++) {
                if (num_base16_part[i] == Digit) {
                    uRet = i + 10U;
                    break;
                }
            }
        }
    } else {
        // make vcase happy
    }
    return uRet;

}

UINT32 OSAL_StringToUInt32(const char *pString, UINT32 *pValue)
{
    UINT32 uRet = 0;
    if ((pString == NULL) || (pValue == NULL)) {
        uRet = 1;
    } else {
        SIZE_t i, Count, idx, Mul;
        UINT32 base = 10;
        SIZE_t Length = OSAL_StringLength(pString);
        if (Length == 0U) {
            // '' return error
            uRet = 1;
        } else {
            Count = Length;
            if (Length > 2U) {
                if (((pString[0] == '0') && (pString[1] == 'x')) || ((pString[0] == '0') && (pString[1] == 'X'))) {
                    base = 16;
                    Count = Count - 2U;
                }
            }
            if ((base == 16U) && (Count == 0U)) {
                // '0x' return error
                uRet = 1;
            } else if ((base == 16U) && (Count > 8U)) {
                // '0xABCD12345678' return error
                uRet = 1;
            } else if ((base == 10U) && (Count > 10U)) {
                // max is '4294967295 = 0xFFFFFFFF' return error
                uRet = 1;
            } else {
                *pValue = 0;
                Mul = 1;
                idx = (Length - 1U);
                for (i = 0; i < Count; i++) {
                    UINT32 Value = Digit2Uint32(pString[idx], base);
                    if (Value == 0xFFU) {
                        // no digit
                        uRet = 1;
                        *pValue = 0;
                        break;
                    } else {
                        *pValue = *pValue + (Mul * Value);
                    }
                    idx = idx - 1U;
                    Mul = Mul * base;
                }
            }
        }
    }
    return uRet;
}
EXPORT_SYMBOL(OSAL_StringToUInt32);

UINT32 OSAL_StringToUInt64(const char *pString, UINT64 *pValue)
{
    UINT32 uRet = 0;
    if ((pString == NULL) || (pValue == NULL)) {
        uRet = 1;
    } else {
        SIZE_t i, Count, idx;
        UINT64 Mul;
        UINT32 base = 10;
        SIZE_t Length = OSAL_StringLength(pString);
        if (Length == 0U) {
            // '' return error
            uRet = 1;
        } else {
            Count = Length;
            if (Length > 2U) {
                if (((pString[0] == '0') && (pString[1] == 'x')) || ((pString[0] == '0') && (pString[1] == 'X'))) {
                    base = 16;
                    Count = Count - 2U;
                }
            }
            if ((base == 16U) && (Count == 0U)) {
                // '0x' return error
                uRet = 1;
            } else if ((base == 16U) && (Count > 16U)) {
                // '0xABCD1234ABCD1234AA' return error
                uRet = 1;
            } else if ((base == 10U) && (Count > 20U)) {
                // max is '18446744073709551615 = 0xFFFFFFFFFFFFFFFF' return error
                uRet = 1;
            } else {
                *pValue = 0;
                Mul = 1;
                idx = (Length - 1U);
                for (i = 0; i < Count; i++) {
                    UINT32 Value = Digit2Uint32(pString[idx], base);
                    if (Value == 0xFFU) {
                        // no digit
                        uRet = 1;
                        *pValue = 0;
                        break;
                    } else {
                        *pValue = *pValue + (Mul * Value);
                    }
                    idx = idx - 1U;
                    Mul = Mul * base;
                }
            }
        }
    }
    return uRet;
}
EXPORT_SYMBOL(OSAL_StringToUInt64);

void OSAL_MemorySetChar(char *pBuffer, char Value, UINT32 Size)
{
    char *ptr = pBuffer;
    UINT32 Count = Size / sizeof(char);

    if (ptr != NULL) {
        while(Count != 0U) {
            *ptr =  Value;
            Count = Count - 1U;
            ptr++;
        }
    }
    return;
}
EXPORT_SYMBOL(OSAL_MemorySetChar);

void OSAL_MemorySetU8(UINT8 *pBuffer, UINT8 Value, UINT32 Size)
{
    UINT8 *ptr = pBuffer;
    UINT32 Count = Size / sizeof(UINT8);

    if (ptr != NULL) {
        while(Count != 0U) {
            *ptr =  Value;
            Count = Count - 1U;
            ptr++;
        }
    }
    return;
}
EXPORT_SYMBOL(OSAL_MemorySetU8);

void OSAL_MemorySetU32(UINT32 *pBuffer, UINT32 Value, UINT32 Size)
{
    UINT32 *ptr = pBuffer;
    UINT32 Count = Size / sizeof(UINT32);

    if (ptr != NULL) {
        while(Count != 0U) {
            *ptr =  Value;
            Count = Count - 1U;
            ptr++;
        }
    }
    return;
}
EXPORT_SYMBOL(OSAL_MemorySetU32);

UINT32 OSAL_StringToken(const char *pString, char Delimiter, char Args[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH], UINT32 *ArgCount)
{
    UINT32 uRet = 0;
    UINT32 Idx;
    UINT32 Length;
    UINT32 ArgPos = 0;
    UINT32 Count = 0;

    if ((pString == NULL) || (Args == NULL) || (ArgCount == NULL)) {
        uRet = 0xFFFFFFFFU;
    } else {
        Length = OSAL_StringLength(pString);
        for (Idx = 0; Idx < Length; Idx++) {
            if ((Count >= (UTIL_MAX_STR_TOKEN_NUMBER)) || (ArgPos >= (UTIL_MAX_STR_TOKEN_LENGTH))) {
                uRet = 0xFFFFFFFFU;
                break;
            }
            if (pString[Idx] == Delimiter) {
                // match
                Args[Count][ArgPos] = '\0';
                if (ArgPos != 0U) {
                    Count++;
                    ArgPos = 0;
                }
            } else {
                // no match
                Args[Count][ArgPos] = pString[Idx];
                ArgPos++;
            }
        }
        if ((Count < UTIL_MAX_STR_TOKEN_NUMBER) && (ArgPos < UTIL_MAX_STR_TOKEN_LENGTH)) {
            if (ArgPos == 0U) {
                // last token is ' '
                if (Count > 0U) {
                    Count = Count - 1U;
                }
            } else {
                Args[Count][ArgPos] = '\0';
            }
        }

        *ArgCount = (Count+1U);

    }
    return uRet;
}
EXPORT_SYMBOL(OSAL_StringToken);
