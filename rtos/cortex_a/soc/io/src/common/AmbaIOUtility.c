/**
 *  @file AmbaIOUtility.c
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
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
 *  @details Ambarella Utility functions.
 *
 */

#include "AmbaIOUtility.h"
#include "AmbaIOUtility_Crc32Hw.h"
#include <AmbaWrap.h>
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaFDT.h>
#endif

static char UInt32ToDigit(UINT32 Value)
{
    static const char num_base16_full[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char cRet = num_base16_full[Value];
    return cRet;
}

static void string_reverse(char *pBuffer, UINT32 Size)
{
    UINT32 idx;
    UINT32 limit = Size / 2U;
    UINT32 swap_idx;
    char tmp;

    for (idx = 0; idx < limit; idx++) {
        swap_idx = (Size - idx) - 1U;
        tmp = pBuffer[idx];
        pBuffer[idx] = pBuffer[swap_idx];
        pBuffer[swap_idx] = tmp;
    }
}

UINT32 IO_UtilityUInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    UINT32 ut = Value, uTmp;
    UINT32 idx = 0;
    UINT32 Base = Radix;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (ut == 0U) {
        pBuffer[idx] = '0';
        idx++;
        pBuffer[idx] = '\0';         /* attach the null terminator */
        StrLength = 1;
    } else {
        while (ut > 0U) {
            uTmp = ut % Base;
            pBuffer[idx] = UInt32ToDigit(uTmp);
            idx++;
            if (idx >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= Base;
        }
        pBuffer[idx] = '\0';             /* attach the null terminator */
        StrLength = idx;

        // change the order of the digits
        string_reverse(pBuffer, StrLength);
    }

    /* make sure at least a \0 is added.*/
    if (BufferSize > 0U) {
        /* make sure at least a \0 is added.*/
        pBuffer[BufferSize-1U] = '\0';
    }
    return StrLength;
}

UINT32 IO_UtilityUInt64ToStr(char *pBuffer, UINT32 BufferSize, UINT64 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    UINT64 ut = Value, uTmp;
    UINT32 idx = 0;
    UINT32 Base = Radix;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (ut == 0U) {
        pBuffer[idx] = '0';
        idx++;
        pBuffer[idx] = '\0';         /* attach the null terminator */
        StrLength = 1;
    } else {
        while (ut > 0U) {
            uTmp = ut % (UINT64)Base;
            pBuffer[idx] = UInt32ToDigit((UINT32)uTmp);
            idx++;
            if (idx >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= (UINT64)Base;
        }
        pBuffer[idx] = '\0';             /* attach the null terminator */
        StrLength = idx;

        // change the order of the digits
        string_reverse(pBuffer, StrLength);

    }

    if (BufferSize > 0U) {
        /* make sure at least a \0 is added.*/
        pBuffer[BufferSize-1U] = '\0';
    }
    return StrLength;
}

static UINT32 IO_UtilityIsDigit(INT32 Value)
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
    SIZE_t i;
    SIZE_t Length = IO_UtilityStringLength(pSource);
    for (i = 0; i < Length; i++) {
        if (pSource[i] == TargetChar) {
            uRet = 1;
            break;
        }
    }

    return uRet;
}

static UINT32 Digit2Uint32(char Digit)
{
    UINT32 uRet = 0xFF, i;
    static const char num_base10[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    // only support 10-based digit, we only scan 1-8 for 100% branch coverage
    for (i = 1; i <= 8U; i++) {
        if (num_base10[i] == Digit) {
            uRet = i;
            break;
        }
    }

    // if no result, it is 9
    if (uRet == 0xFFU) {
        uRet = 9;
    }
    return uRet;

}

static void IO_UtilityStringToUInt32(const char *pString, UINT32 *pValue)
{
    SIZE_t i, Count, idx;
    UINT32 Mul;
    UINT32 base = 10;
    SIZE_t Length = IO_UtilityStringLength(pString);
    Count = Length;
    // only 10-base number
    if (Count > 10U) {
        // max is '4294967295 = 0xFFFFFFFF'
        *pValue = 0;
    } else {
        *pValue = 0;
        Mul = 1;
        idx = (Length - 1U);
        for (i = 0; i < Count; i++) {
            UINT32 Value = Digit2Uint32(pString[idx]);
            *pValue = *pValue + (Mul * Value);
            idx = idx - 1U;
            Mul = Mul * base;
        }
    }
    return;
}


static void StringPrint_PreProc(const char *pFmtString, UINT32 *idx, char *TmpBuffer, UINT32 BufferSize, UINT32 *width)
{

    while (pFmtString[*idx] != '\0') {
        if (DoesStringContain("bcdefgilopPrRsuxX%", pFmtString[*idx]) == 1U) {
            break;
        } else if ((pFmtString[*idx] >= '1') && (pFmtString[*idx] <= '9')) {
            UINT32 idxStart = *idx;
            UINT32 idxEnd = idxStart;
            UINT32 w;
            for (; (IO_UtilityIsDigit((INT32)(UINT8)pFmtString[idxEnd]) != 0U); idxEnd++) {
                /* make misra happy */
            }

            IO_UtilityStringCopy(TmpBuffer, BufferSize, &pFmtString[idxStart]);
            TmpBuffer[idxEnd - idxStart] = '\0';
            IO_UtilityStringToUInt32(TmpBuffer, &w);
            *width = w;
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
    } else {
        /* not supported, just use base 10 */
        *base = 10;
    }
}

static void StringPrintUInt64_ParseArg(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 idx, INT32 *base, UINT32 *nRetIdx, UINT32 *is_return, UINT32 *width)
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
        *width = 16;
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
        *width = 16;
    } else {
        /* not supported, just use base 10 */
        *base = 10;
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

static UINT64 StringPrintUInt64_CheckArg(UINT32 ArgIdx, UINT32 Count, const UINT64 *pArgs)
{
    UINT64  ArgValue;
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
        string_length = IO_UtilityUInt32ToStr(tmp, BufferSize, ArgValue, 16);
    } else {
        string_length = IO_UtilityUInt32ToStr(tmp, BufferSize, ArgValue, 10);
    }
    return string_length;
}

static UINT32 StringPrintUInt64_ValueToString(char *tmp, UINT32 BufferSize, INT32 base, UINT64 ArgValue)
{
    UINT32 string_length;
    if (base == 16) {
        string_length = IO_UtilityUInt64ToStr(tmp, BufferSize, ArgValue, 16);
    } else {
        string_length = IO_UtilityUInt64ToStr(tmp, BufferSize, ArgValue, 10);
    }
    return string_length;
}

static void StringPrint_IgnoreFormatLL(const char *pFmtString, UINT32 *idx)
{
    if (pFmtString[*idx] == 'l') {
        *idx = *idx + 1U;
        if (pFmtString[*idx] == 'l') {
            *idx = *idx + 1U;
        }
    }
}

static UINT32 StringPrint_FomratCopy(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 *nRetIdx, UINT32 *idx)
{
    UINT32 is_return = 0;

    pBuffer[*nRetIdx] = pFmtString[*idx];
    (*idx) = (*idx) + 1U;
    (*nRetIdx) = (*nRetIdx) + 1U;
    if (*nRetIdx == (BufferSize-1U)) {
        is_return = 1;
    }
    return is_return;
}

static UINT32 StringPrint_FomratCopyPercent(char *pBuffer, UINT32 BufferSize, UINT32 *nRetIdx)
{
    UINT32 is_return = 0;

    pBuffer[*nRetIdx] = '%';
    (*nRetIdx) = *nRetIdx + 1U;
    pBuffer[*nRetIdx] = '\0';
    if (*nRetIdx == (BufferSize-1U)) {
        is_return = 1;
    }

    return is_return;
}

static UINT32 StringPrint_FormatCopyCharU32(char *pBuffer, UINT32 BufferSize, UINT32 *nRetIdx, UINT32 *argIdx, UINT32 ArgValue)
{
    UINT32 is_return = 0;

    (*argIdx) = (*argIdx) + 1U;
    pBuffer[*nRetIdx] = (char)ArgValue;
    (*nRetIdx) = (*nRetIdx) + 1U;
    pBuffer[*nRetIdx] = '\0';
    if (*nRetIdx == (BufferSize-1U)) {
        is_return = 1;
    }
    return is_return;

}

static UINT32 StringPrint_FormatCopyCharU64(char *pBuffer, UINT32 BufferSize, UINT32 *nRetIdx, UINT32 *argIdx, UINT64 ArgValue)
{
    UINT32 is_return = 0;

    (*argIdx) = (*argIdx) + 1U;
    pBuffer[*nRetIdx] = (char)ArgValue;
    (*nRetIdx) = (*nRetIdx) + 1U;
    pBuffer[*nRetIdx] = '\0';
    if (*nRetIdx == (BufferSize-1U)) {
        is_return = 1;
    }
    return is_return;
}

static UINT32 StringPrint_FormatCopyNull(char *pBuffer, UINT32 BufferSize, UINT32 *nRetIdx, UINT32 *argIdx)
{
    UINT32 is_return = 0;

    (*argIdx) = (*argIdx) + 1U;
    pBuffer[*nRetIdx] = '\0';
    IO_UtilityStringAppend(pBuffer, BufferSize, "(null)");
    if ((*nRetIdx + 6U) > (BufferSize - 1U)) {
        *nRetIdx = BufferSize - 1U;
        is_return = 1;
    } else {
        *nRetIdx = *nRetIdx + 6U;
    }
    return is_return;
}


/**
* Utility print message that contain unsigned int32 var
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] pFmtString format of desired string
* @param [in] Count total number of args
* @param [in] pArgs pointer to uint32 args array
* @return nRetIdx string ending mark index, e.g. the string length
*
*/
UINT32 IO_UtilityStringPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs)
{
    char tmp[40];
    INT32 base = 10;
    UINT32 width, string_length;
    UINT32 idx = 0, nRetIdx = 0, argIdx = 0, ArgValue;
    UINT32 is_return = 0;
    SIZE_t max_idx = (UINT32)IO_UtilityStringLength(pFmtString);

    if ((pBuffer == NULL) || (pFmtString == NULL) || (pArgs == NULL) || (BufferSize == 0U)) {
        // no action
    } else {
        while (pFmtString[idx] != '\0') {
            if (is_return == 1U) {
                break;
            }
            if (pFmtString[idx] == '%') {
                idx++;
                width = 0;

                StringPrint_PreProc(pFmtString, &idx, tmp, 40, &width);

                if (pFmtString[idx] == '%') {
                    is_return = StringPrint_FomratCopyPercent(pBuffer, BufferSize, &nRetIdx);
                } else if (pFmtString[idx] == 's') {
                    /* not supported, just print '(null)' */
                    is_return = StringPrint_FormatCopyNull(pBuffer, BufferSize, &nRetIdx, &argIdx);
                } else if (pFmtString[idx] == 'c') {

                    ArgValue = StringPrintUInt32_CheckArg(argIdx, Count, pArgs);
                    is_return = StringPrint_FormatCopyCharU32(pBuffer, BufferSize, &nRetIdx, &argIdx, ArgValue);

                } else {

                    StringPrint_IgnoreFormatLL(pFmtString, &idx);

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

                        IO_UtilityStringAppend(pBuffer, BufferSize, &tmp[0]);

                        if ((nRetIdx + string_length) > (BufferSize-1U)) {
                            nRetIdx = BufferSize-1U;
                            is_return = 1;
                            continue;
                        } else {
                            nRetIdx += string_length;
                        }
                    }
                }

                if (is_return == 1U) {
                    continue;
                }

                if (idx < max_idx) {
                    idx++;
                }

            } else {
                // no format specified, copy 1 character
                is_return = StringPrint_FomratCopy(pBuffer, BufferSize, pFmtString, &nRetIdx, &idx);
            }
        } // end of for loop
        pBuffer[nRetIdx] = '\0';
    }
    return nRetIdx;
}

/**
* Utility print message that contain unsigned int64 var
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] pFmtString format of desired string
* @param [in] Count total number of args
* @param [in] pArgs pointer to uint64 args array
* @return nRetIdx string ending mark index, e.g. the string length
*
*/
UINT32 IO_UtilityStringPrintUInt64(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT64 *pArgs)
{
    char tmp[40];
    INT32 base = 10;
    UINT32 width, string_length;
    UINT32 idx = 0, nRetIdx = 0, argIdx = 0;
    UINT64 arg_value;
    UINT32 is_return = 0;
    SIZE_t max_idx = (UINT32)IO_UtilityStringLength(pFmtString);

    if ((pBuffer == NULL) || (pFmtString == NULL) || (pArgs == NULL) || (BufferSize == 0U)) {
        // no action
    } else {
        while (pFmtString[idx] != '\0') {
            if (is_return == 1U) {
                break;
            }
            if (pFmtString[idx] == '%') {
                idx++;
                width = 0;

                StringPrint_PreProc(pFmtString, &idx, tmp, 40, &width);

                if (pFmtString[idx] == '%') {
                    is_return = StringPrint_FomratCopyPercent(pBuffer, BufferSize, &nRetIdx);
                } else if (pFmtString[idx] == 's') {
                    /* not supported, just print '(null)' */
                    is_return = StringPrint_FormatCopyNull(pBuffer, BufferSize, &nRetIdx, &argIdx);
                } else if (pFmtString[idx] == 'c') {

                    arg_value = StringPrintUInt64_CheckArg(argIdx, Count, pArgs);
                    is_return = StringPrint_FormatCopyCharU64(pBuffer, BufferSize, &nRetIdx, &argIdx, arg_value);

                } else {

                    StringPrint_IgnoreFormatLL(pFmtString, &idx);

                    if (DoesStringContain("bdiopPrRxXu", pFmtString[idx]) == 1U) {

                        StringPrintUInt64_ParseArg(pBuffer, BufferSize, pFmtString, idx, &base, &nRetIdx, &is_return, &width);
                        if (is_return == 1U) {
                            continue;
                        }

                        arg_value = StringPrintUInt64_CheckArg(argIdx, Count, pArgs);

                        argIdx++;
                        pBuffer[nRetIdx] = '\0';

                        string_length = StringPrintUInt64_ValueToString(tmp, 40, base, arg_value);

                        // padding spaces if needed
                        StringPrintUInt32_Padding(pBuffer, BufferSize, width, string_length, &nRetIdx, &is_return);
                        if (is_return == 1U) {
                            continue;
                        }

                        IO_UtilityStringAppend(pBuffer, BufferSize, &tmp[0]);

                        if ((nRetIdx + string_length) > (BufferSize-1U)) {
                            nRetIdx = BufferSize-1U;
                            is_return = 1;
                            continue;
                        } else {
                            nRetIdx += string_length;
                        }
                    }
                }

                if (is_return == 1U) {
                    continue;
                }

                if (idx < max_idx) {
                    idx++;
                }

            } else {
                // no format specified, copy 1 character
                is_return = StringPrint_FomratCopy(pBuffer, BufferSize, pFmtString, &nRetIdx, &idx);
            }
        } // end of for loop
        pBuffer[nRetIdx] = '\0';
    }
    return nRetIdx;
}

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

UINT32 IO_UtilityStringPrintStr(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const char * const pArgs[])
{
    char tmp[40];
    const char *ArgValue;
    UINT32 width, string_length;
    UINT32 idx = 0, nRetIdx = 0, argIdx = 0;
    UINT32 is_return = 0;

    if ((pBuffer == NULL) || (pFmtString == NULL) || (pArgs == NULL) || (BufferSize == 0U)) {
        // no action
    } else {
        while (pFmtString[idx] != '\0') {
            if (is_return == 1U) {
                break;
            }
            if (pFmtString[idx] == '%') {
                idx++;
                width = 0;
                StringPrint_PreProc(pFmtString, &idx, tmp, 40, &width);

                if (pFmtString[idx] == '%') {
                    is_return = StringPrint_FomratCopyPercent(pBuffer, BufferSize, &nRetIdx);
                } else if (pFmtString[idx] == 's') {
                    if (argIdx >= Count) {
                        ArgValue = "";
                    } else {
                        ArgValue = pArgs[argIdx];
                    }
                    pBuffer[nRetIdx] = '\0';
                    string_length = 0;
                    if (ArgValue != NULL) {
                        string_length = (UINT32)IO_UtilityStringLength(ArgValue);
                        StringPrintStr_Padding(pBuffer, BufferSize, width, string_length, &nRetIdx, &is_return);
                        if (is_return == 1U) {
                            continue;
                        }
                        IO_UtilityStringAppend(pBuffer, BufferSize, ArgValue);
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
                    StringPrint_IgnoreFormatLL(pFmtString, &idx);
                }

                if (is_return == 1U) {
                    continue;
                }
                idx++;
            } else {
                // no format specified, copy 1 character
                is_return = StringPrint_FomratCopy(pBuffer, BufferSize, pFmtString, &nRetIdx, &idx);
            }
        } // end of for loop
        pBuffer[nRetIdx] = '\0';

    }

    return nRetIdx;
}

#ifdef CONFIG_QNX
// no hardware CRC32 support for QNX
#else
#ifdef CONFIG_ARM32
static UINT32 crc32_arm32_hw(UINT32 *crc, const UINT8 *p, UINT32 len)
{
    UINT32 length = len;
    SIZE_t index = 0;

    for(;;) {
        if((length/sizeof(UINT32)) > 0U) {
            UINT32 value = ((UINT32)p[index+3U] << 24U) |
                           ((UINT32)p[index+2U] << 16U) |
                           ((UINT32)p[index+1U] << 8U)  |
                           ((UINT32)p[index]);
            IO_UTIL_CRC32W(crc, value);
            index += sizeof(UINT32);
            length -= sizeof(UINT32);
        } else {
            break;
        }
    }

    if ((length/sizeof(UINT16)) > 0U) {
        UINT16 value = ((UINT16)p[index+1U] << 8U)  |
                       ((UINT16)p[index]);
        IO_UTIL_CRC32H(crc, value);
        index += sizeof(UINT16);
        length -= sizeof(UINT16);
    }

    if ((length/sizeof(UINT8)) > 0U) {
        IO_UTIL_CRC32B(crc, p[index]);
    }
    return *crc;
}
#else
static UINT32 crc32_arm64_hw(UINT32 *crc, const UINT8 *p, UINT32 len)
{
    UINT32 length = len;
    SIZE_t index = 0;

    for(;;) {
        if((length/sizeof(UINT64)) > 0U) {
            UINT64 value = ((UINT64)p[index+7U] << 56U) |
                           ((UINT64)p[index+6U] << 48U) |
                           ((UINT64)p[index+5U] << 40U) |
                           ((UINT64)p[index+4U] << 32U) |
                           ((UINT64)p[index+3U] << 24U) |
                           ((UINT64)p[index+2U] << 16U) |
                           ((UINT64)p[index+1U] << 8U)  |
                           ((UINT64)p[index]);
            IO_UTIL_CRC32X(crc,value);
            index += sizeof(UINT64);
            length -= (UINT32)sizeof(UINT64);
        } else {
            break;
        }
    }
    if ((length/sizeof(UINT32)) > 0U) {
        UINT32 value = ((UINT32)p[index+3U] << 24U) |
                       ((UINT32)p[index+2U] << 16U) |
                       ((UINT32)p[index+1U] << 8U)  |
                       ((UINT32)p[index]);
        IO_UTIL_CRC32W(crc, value);
        index += sizeof(UINT32);
        length -= (UINT32)sizeof(UINT32);
    }

    if ((length/sizeof(UINT16)) > 0U) {
        UINT16 value = ((UINT16)p[index+1U] << 8U)  |
                       ((UINT16)p[index]);
        IO_UTIL_CRC32H(crc, value);
        index += sizeof(UINT16);
        length -= (UINT32)sizeof(UINT16);
    }

    if ((length/sizeof(UINT8)) > 0U) {
        IO_UTIL_CRC32B(crc, p[index]);
    }
    return *crc;
}
#endif
#endif // #ifdef CONFIG_QNX


UINT32 IO_UtilityCrc32(const UINT8 *pBuffer, UINT32 Size)
{
#ifdef CONFIG_QNX
    // no hardware CRC32 support for QNX
    return IO_UtilityCrc32Sw(pBuffer, Size);
#else
    return IO_UtilityCrc32Hw(pBuffer, Size);
#endif
}

#ifdef CONFIG_QNX
// no hardware CRC32 support for QNX

/*-----------------------------------------------------------------------------------------------*\
 *  CRC32 table.
\*-----------------------------------------------------------------------------------------------*/
/*-
 *  COPYRIGHT (C) 1986 Gary S. Brown.  You may use this program, or
 *  code or tables extracted from it, as desired without restriction.
 */
static UINT32 IO_Crc32Table[] = {
    0x00000000U, 0x77073096U, 0xee0e612cU, 0x990951baU, 0x076dc419U, 0x706af48fU,
    0xe963a535U, 0x9e6495a3U, 0x0edb8832U, 0x79dcb8a4U, 0xe0d5e91eU, 0x97d2d988U,
    0x09b64c2bU, 0x7eb17cbdU, 0xe7b82d07U, 0x90bf1d91U, 0x1db71064U, 0x6ab020f2U,
    0xf3b97148U, 0x84be41deU, 0x1adad47dU, 0x6ddde4ebU, 0xf4d4b551U, 0x83d385c7U,
    0x136c9856U, 0x646ba8c0U, 0xfd62f97aU, 0x8a65c9ecU, 0x14015c4fU, 0x63066cd9U,
    0xfa0f3d63U, 0x8d080df5U, 0x3b6e20c8U, 0x4c69105eU, 0xd56041e4U, 0xa2677172U,
    0x3c03e4d1U, 0x4b04d447U, 0xd20d85fdU, 0xa50ab56bU, 0x35b5a8faU, 0x42b2986cU,
    0xdbbbc9d6U, 0xacbcf940U, 0x32d86ce3U, 0x45df5c75U, 0xdcd60dcfU, 0xabd13d59U,
    0x26d930acU, 0x51de003aU, 0xc8d75180U, 0xbfd06116U, 0x21b4f4b5U, 0x56b3c423U,
    0xcfba9599U, 0xb8bda50fU, 0x2802b89eU, 0x5f058808U, 0xc60cd9b2U, 0xb10be924U,
    0x2f6f7c87U, 0x58684c11U, 0xc1611dabU, 0xb6662d3dU, 0x76dc4190U, 0x01db7106U,
    0x98d220bcU, 0xefd5102aU, 0x71b18589U, 0x06b6b51fU, 0x9fbfe4a5U, 0xe8b8d433U,
    0x7807c9a2U, 0x0f00f934U, 0x9609a88eU, 0xe10e9818U, 0x7f6a0dbbU, 0x086d3d2dU,
    0x91646c97U, 0xe6635c01U, 0x6b6b51f4U, 0x1c6c6162U, 0x856530d8U, 0xf262004eU,
    0x6c0695edU, 0x1b01a57bU, 0x8208f4c1U, 0xf50fc457U, 0x65b0d9c6U, 0x12b7e950U,
    0x8bbeb8eaU, 0xfcb9887cU, 0x62dd1ddfU, 0x15da2d49U, 0x8cd37cf3U, 0xfbd44c65U,
    0x4db26158U, 0x3ab551ceU, 0xa3bc0074U, 0xd4bb30e2U, 0x4adfa541U, 0x3dd895d7U,
    0xa4d1c46dU, 0xd3d6f4fbU, 0x4369e96aU, 0x346ed9fcU, 0xad678846U, 0xda60b8d0U,
    0x44042d73U, 0x33031de5U, 0xaa0a4c5fU, 0xdd0d7cc9U, 0x5005713cU, 0x270241aaU,
    0xbe0b1010U, 0xc90c2086U, 0x5768b525U, 0x206f85b3U, 0xb966d409U, 0xce61e49fU,
    0x5edef90eU, 0x29d9c998U, 0xb0d09822U, 0xc7d7a8b4U, 0x59b33d17U, 0x2eb40d81U,
    0xb7bd5c3bU, 0xc0ba6cadU, 0xedb88320U, 0x9abfb3b6U, 0x03b6e20cU, 0x74b1d29aU,
    0xead54739U, 0x9dd277afU, 0x04db2615U, 0x73dc1683U, 0xe3630b12U, 0x94643b84U,
    0x0d6d6a3eU, 0x7a6a5aa8U, 0xe40ecf0bU, 0x9309ff9dU, 0x0a00ae27U, 0x7d079eb1U,
    0xf00f9344U, 0x8708a3d2U, 0x1e01f268U, 0x6906c2feU, 0xf762575dU, 0x806567cbU,
    0x196c3671U, 0x6e6b06e7U, 0xfed41b76U, 0x89d32be0U, 0x10da7a5aU, 0x67dd4accU,
    0xf9b9df6fU, 0x8ebeeff9U, 0x17b7be43U, 0x60b08ed5U, 0xd6d6a3e8U, 0xa1d1937eU,
    0x38d8c2c4U, 0x4fdff252U, 0xd1bb67f1U, 0xa6bc5767U, 0x3fb506ddU, 0x48b2364bU,
    0xd80d2bdaU, 0xaf0a1b4cU, 0x36034af6U, 0x41047a60U, 0xdf60efc3U, 0xa867df55U,
    0x316e8eefU, 0x4669be79U, 0xcb61b38cU, 0xbc66831aU, 0x256fd2a0U, 0x5268e236U,
    0xcc0c7795U, 0xbb0b4703U, 0x220216b9U, 0x5505262fU, 0xc5ba3bbeU, 0xb2bd0b28U,
    0x2bb45a92U, 0x5cb36a04U, 0xc2d7ffa7U, 0xb5d0cf31U, 0x2cd99e8bU, 0x5bdeae1dU,
    0x9b64c2b0U, 0xec63f226U, 0x756aa39cU, 0x026d930aU, 0x9c0906a9U, 0xeb0e363fU,
    0x72076785U, 0x05005713U, 0x95bf4a82U, 0xe2b87a14U, 0x7bb12baeU, 0x0cb61b38U,
    0x92d28e9bU, 0xe5d5be0dU, 0x7cdcefb7U, 0x0bdbdf21U, 0x86d3d2d4U, 0xf1d4e242U,
    0x68ddb3f8U, 0x1fda836eU, 0x81be16cdU, 0xf6b9265bU, 0x6fb077e1U, 0x18b74777U,
    0x88085ae6U, 0xff0f6a70U, 0x66063bcaU, 0x11010b5cU, 0x8f659effU, 0xf862ae69U,
    0x616bffd3U, 0x166ccf45U, 0xa00ae278U, 0xd70dd2eeU, 0x4e048354U, 0x3903b3c2U,
    0xa7672661U, 0xd06016f7U, 0x4969474dU, 0x3e6e77dbU, 0xaed16a4aU, 0xd9d65adcU,
    0x40df0b66U, 0x37d83bf0U, 0xa9bcae53U, 0xdebb9ec5U, 0x47b2cf7fU, 0x30b5ffe9U,
    0xbdbdf21cU, 0xcabac28aU, 0x53b39330U, 0x24b4a3a6U, 0xbad03605U, 0xcdd70693U,
    0x54de5729U, 0x23d967bfU, 0xb3667a2eU, 0xc4614ab8U, 0x5d681b02U, 0x2a6f2b94U,
    0xb40bbe37U, 0xc30c8ea1U, 0x5a05df1bU, 0x2d02ef8dU
};


UINT32 IO_UtilityCrc32Sw(const UINT8 *pBuffer, UINT32 Size)
{
    const UINT8 *p;
    UINT32 Crc;
    UINT32 idx;
    UINT32 count = Size;

    p = pBuffer;
    Crc = ~0U;

    while (count > 0U) {
        idx = (Crc ^ (UINT32)*p) & 0x0FFU;
        Crc = IO_Crc32Table[idx] ^ (Crc >> 8U);
        p++;
        count--;
    }

    return Crc ^ ~0U;
}

#else
UINT32 IO_UtilityCrc32Hw(const UINT8 *pBuffer, UINT32 Size)
{
    const UINT8 *p;
    UINT32 Crc;

    p = pBuffer;
    Crc = ~0U;
#ifdef CONFIG_ARM32
    Crc = crc32_arm32_hw(&Crc, p, Size);
#else
    Crc = crc32_arm64_hw(&Crc, p, Size);
#endif
    return Crc ^ ~0U;
}
#endif


void IO_UtilityStringCopy(char *pDest, SIZE_t DestSize, const char *pSource)
{
    SIZE_t CopyLength;

    if ((pDest == NULL) || (pSource == NULL) || (DestSize == 0U)) {
        // no action
    } else {
        SIZE_t i;
        CopyLength = IO_UtilityStringLength(pSource);
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

SIZE_t IO_UtilityStringLength(const char *pString)
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

INT32 IO_UtilityStringCompare(const char *pString1, const char *pString2, SIZE_t Size)
{
    SIZE_t StringLength1, StringLength2;
    INT32 nRet = 0;
    if ((pString1 == NULL) || (pString2 == NULL) || (Size == 0U)) {
        // return no equal
        nRet = -1;
    } else {
        SIZE_t i;
        StringLength1 = IO_UtilityStringLength(pString1);
        StringLength2 = IO_UtilityStringLength(pString2);
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

void IO_UtilityStringAppend(char *pBuffer, UINT32 BufferSize, const char *pSource)
{
    if ((pBuffer == NULL) || (pSource == NULL) || (BufferSize == 0U)) {
        // no action here
    } else {
        UINT32 StringLengthDest = (UINT32)IO_UtilityStringLength(pBuffer);
        UINT32 StringLengthCopy = (UINT32)IO_UtilityStringLength(pSource);
        UINT32 i;

        if ((StringLengthCopy >= (BufferSize - 1U)) || (StringLengthDest >= (BufferSize - 1U))) {
            // buffer overflow, no action here
        } else {
            if ((StringLengthDest + StringLengthCopy) > (BufferSize - 1U)) {
                StringLengthCopy = (BufferSize - StringLengthDest) - 1U;
            }
            for (i = 0; i < StringLengthCopy; i++) {
                pBuffer[i + StringLengthDest] = pSource[i];
            }
            pBuffer[StringLengthDest + StringLengthCopy] = '\0';
        }
    }
}

void  *IO_UtilityU32AddrToPtr(UINT32 Addr)
{
    void *ptr_ret;
#if defined(CONFIG_ARM32)
    if (AmbaWrap_memcpy(&ptr_ret, &Addr, sizeof(UINT32)) != 0U) {
        // action TBD
    }
    return ptr_ret;
#else
    UINT64 u64_addr = (UINT64)Addr;
    if (AmbaWrap_memcpy(&ptr_ret, &u64_addr, sizeof(UINT64))!= 0U) {
        // action TBD
    }
    return ptr_ret;
#endif
}

void  *IO_UtilityU64AddrToPtr(UINT64 Addr)
{
    void *ptr;

#if defined(CONFIG_ARM32)
    UINT32 u32_addr = (UINT32)Addr;
    if (AmbaWrap_memcpy(&ptr, &u32_addr, sizeof(void *)) != 0U) {
        // action TBD
    }
#else
    if (AmbaWrap_memcpy(&ptr, &Addr, sizeof(void *)) != 0U) {
        // action TBD
    }
#endif
    return ptr;

}

UINT32 IO_UtilityPtrToU32Addr(const void *Ptr)
{
    UINT32 u32_addr_ret;
#if defined(CONFIG_ARM32)
    if (AmbaWrap_memcpy(&u32_addr_ret, &Ptr, sizeof(UINT32)) != 0U) {
        // action TBD
    }
    return u32_addr_ret;
#else
    UINT64 u64_addr;
    if (AmbaWrap_memcpy(&u64_addr, &Ptr, sizeof(UINT64))!= 0U) {
        // action TBD
    }
    u32_addr_ret = (UINT32)u64_addr;
    return u32_addr_ret;
#endif
}

UINT64 IO_UtilityPtrToU64Addr(const void *Ptr)
{
    UINT64 u64_addr_ret;
#if defined(CONFIG_ARM32)
    UINT32 u32_addr;
    if (AmbaWrap_memcpy(&u32_addr, &Ptr, sizeof(UINT32)) != 0U) {
        // action TBD
    }
    u64_addr_ret = u32_addr;
    return u64_addr_ret;
#else
    if (AmbaWrap_memcpy(&u64_addr_ret, &Ptr, sizeof(UINT64))!= 0U) {
        // action TBD
    }
    return u64_addr_ret;
#endif
}

void IO_UtilityRegWrite32(UINT64 Address, UINT32 Value)
{
    volatile UINT32 *uint32_ptr;
    const void *vptr = IO_UtilityU64AddrToPtr(Address);
    if (AmbaWrap_memcpy(&uint32_ptr, &vptr, sizeof(void *)) == 0U) {
        *uint32_ptr = Value;
    }
}

UINT32 IO_UtilityRegRead32(UINT64 Address)
{
    const volatile UINT32 *uint32_ptr;
    const void *vptr = IO_UtilityU64AddrToPtr(Address);
    UINT32 uret = 0;
    if (AmbaWrap_memcpy(&uint32_ptr, &vptr, sizeof(void *)) == 0U) {
        uret = *uint32_ptr;
    }
    return uret;
}

void IO_UtilityRegMaskWrite32(UINT64 Address, UINT32 Value, UINT32 Mask, UINT32 Shift)
{
    UINT32 reg_value = IO_UtilityRegRead32(Address);
    UINT32 x = (Value << Shift ) & Mask;
    reg_value = (reg_value & (~Mask)) | x;
    IO_UtilityRegWrite32(Address, reg_value);
    return;
}

UINT32 IO_UtilityRegMaskRead32(UINT64 Address, UINT32 Mask, UINT32 Shift)
{
    UINT32 value = IO_UtilityRegRead32(Address);
    return (value & Mask) >> Shift;
}


#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#if defined(CONFIG_ARM32)
static UINT32 dtb_addr = (UINT32)CONFIG_DTB_LOADADDR;
#else
static UINT64 dtb_addr = (UINT64)CONFIG_DTB_LOADADDR;
#endif
static void *fdt_ptr = NULL;
/**
 * Get the Node offset of the 'compatible' string inside FDT
 * @param Offset only find nodes after this offset
 * @param CompatibleStr 'compatible' string to match against
 * @return Offfset of the 'compatible' string inside FDT
 * @retval <=0 Failed
*/
INT32 IO_UtilityFDTNodeOffsetByCID(INT32 Offset, const char *CompatibleStr)
{
    INT32 nret;
    INT32 func_nret;

    if (fdt_ptr == NULL) {
        if (AmbaWrap_memcpy(&fdt_ptr, &dtb_addr, sizeof(dtb_addr)) != 0U) {
            // action TBD
        }
    }

    func_nret = AmbaFDT_CheckHeader(fdt_ptr);
    if (func_nret == 0) {
        nret = AmbaFDT_NodeOffsetByCompatible(fdt_ptr, Offset, CompatibleStr);
        // to pass coverity misra-c check
        // need to check nret value
        if (nret <= 0) {
            nret = 0;
        }
    } else {
        nret = 0;
    }
    return nret;
}

/**
 * Get the property value from the Node offset inside FDT
 * @param Offset  Only find nodes after this offset
 * @param PropName 'property' string to match against
 * @param Index Index in the property value array
 * @return UINT32 Value of the property. 0 when not found.
*/
UINT32 IO_UtilityFDTPropertyU32(INT32 Offset, const char *PropName, UINT32 Index)
{
    INT32 func_nret;
    INT32 len;
    UINT32 uret = 0;
    const struct fdt_property *prop;

    if (fdt_ptr == NULL) {
        if (AmbaWrap_memcpy(&fdt_ptr, &dtb_addr, sizeof(dtb_addr)) != 0U) {
            // action TBD
        }
    }

    func_nret = AmbaFDT_CheckHeader(fdt_ptr);

    if (func_nret == 0) {
        prop = AmbaFDT_GetProperty(fdt_ptr, Offset, PropName, &len);
        if ((prop != NULL) && (len > 0) && ((UINT32)len > (Index * 4UL))) {
            const UINT32 *u32_ptr;
            const char *char_ptr = &prop->data[Index * 4UL];
            if (AmbaWrap_memcpy(&u32_ptr, &char_ptr, sizeof(UINT32 *)) == 0U) {
                // FDT is big-endian format, need to covert to little-endian
                uret = fdt32_to_cpu(*u32_ptr);
            }
        }
    }
    return uret;
}

/**
 * Quick get the offset from the PHandle index inside FDT
 * @param PHandle PHandle index value
 * @return INT32 Offset of the PHandle. 0 when not found.
*/
INT32 IO_UtilityFDTPHandleOffset(UINT32 PHandle)
{
    INT32 func_nret;
    INT32 nret = 0;

    if (fdt_ptr == NULL) {
        if (AmbaWrap_memcpy(&fdt_ptr, &dtb_addr, sizeof(dtb_addr)) != 0U) {
            // action TBD
        }
    }

    func_nret = AmbaFDT_CheckHeader(fdt_ptr);

    if (func_nret == 0) {
        nret = AmbaFDT_NodeOffsetByPhandle(fdt_ptr, PHandle);
    }
    return nret;
}

/**
 * Quick get the property value from the Node offset inside FDT
 * @param Offset  Only find nodes after this offset
 * @param CompatibleStr 'compatible' string to match against
 * @param PropName 'property' string to match against
 * @param Index Index in the property value array (by size of UINT32)
 * @return UINT32 Value of the property. 0 when not found.
*/
UINT32 IO_UtilityFDTPropertyU32Quick(INT32 Offset, const char *CompatibleStr, const char *PropName, UINT32 Index)
{
    INT32 node_offset;
    INT32 len;
    UINT32 uret = 0;
    UINT32 ulen;
    const struct fdt_property *prop;

    if (fdt_ptr == NULL) {
        if (AmbaWrap_memcpy(&fdt_ptr, &dtb_addr, sizeof(dtb_addr)) != 0U) {
            // action TBD
        }
    }

    node_offset = IO_UtilityFDTNodeOffsetByCID(Offset, CompatibleStr);
    if (node_offset > 0) {
        prop = AmbaFDT_GetProperty(fdt_ptr, node_offset, PropName, &len);
        ulen = (UINT32)len;
        if ((prop != NULL) && (ulen > 0U)) {
            if (ulen >= ((Index + 1U) * 4U)) {
                const UINT32 *u32_ptr;
                const char *char_ptr = &prop->data[Index * 4UL];
                if (AmbaWrap_memcpy(&u32_ptr, &char_ptr, sizeof(UINT32 *)) == 0U) {
                    // FDT is big-endian format, need to covert to little-endian
                    uret = fdt32_to_cpu(*u32_ptr);
                }
            }
        }
    }
    return uret;
}

/**
 * Quick get the property value from the Node offset inside FDT
 * @param Offset  Only find nodes after this offset
 * @param CompatibleStr 'compatible' string to match against
 * @param PropName 'property' string to match against
 * @param Index Index in the property value array (by size of UINT64)
 * @return UINT64 Value of the property. 0 when not found.
*/
UINT64 IO_UtilityFDTPropertyU64Quick(INT32 Offset, const char *CompatibleStr, const char *PropName, UINT32 Index)
{
    INT32  node_offset;
    INT32  len;
    UINT32 ulen;
    UINT64 uret = 0;
    const struct fdt_property *prop;

    if (fdt_ptr == NULL) {
        if (AmbaWrap_memcpy(&fdt_ptr, &dtb_addr, sizeof(dtb_addr)) != 0U) {
            // action TBD
        }
    }

    node_offset = IO_UtilityFDTNodeOffsetByCID(Offset, CompatibleStr);
    if (node_offset > 0) {
        prop = AmbaFDT_GetProperty(fdt_ptr, node_offset, PropName, &len);
        ulen = (UINT32)len;
        if ((prop != NULL) && (ulen > 0U)) {
            if (ulen >= ((Index+1U) * 8U)) {
                const UINT64 *u64_ptr;
                const char *char_ptr = &prop->data[Index * 8UL];
                if (AmbaWrap_memcpy(&u64_ptr, &char_ptr, sizeof(UINT64 *)) == 0U) {
                    // FDT is big-endian format, need to covert to little-endian
                    uret = fdt64_to_cpu(*u64_ptr);
                }
            }
        }
    }
    return uret;
}

#endif
