#include <test_group.h>
#include <AmbaTypes.h>
#include <AmbaIOUtility.h>

UINT32 IO_UtilityPtrToU32Addr(const void *Ptr)
{
    return (UINT32)Ptr;
}

void  *IO_UtilityU32AddrToPtr(UINT32 Addr)
{
    return (void *)Addr;
}

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
            if ((StringLengthDest + StringLengthCopy) <= (BufferSize - 1U)) {
                pBuffer[StringLengthDest + StringLengthCopy] = '\0';
            } else {
                pBuffer[BufferSize - 1U] = '\0';
            }
        }
    }
}

static char num_base16_full[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static char UInt32ToDigit(UINT32 Value)
{
    char cRet = '0';

    if (Value < 16U) {
        cRet = num_base16_full[Value];
    }
    return cRet;
}

UINT32 IO_UtilityInt32ToStr(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix)
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


UINT32 IO_UtilityUInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
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
    if (pSource != NULL) {
        SIZE_t i;
        SIZE_t Length = IO_UtilityStringLength(pSource);
        for (i = 0; i < Length; i++) {
            if (pSource[i] == TargetChar) {
                uRet = 1;
                break;
            }
        }
    }
    return uRet;
}

static UINT32 Digit2Uint32(char Digit, UINT32 Base)
{
    UINT32 uRet = 0xFF, i;
    static const char num_base10[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    static const char num_base16_part[6] = {'a', 'b', 'c', 'd', 'e', 'f'};
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

static UINT32 IO_UtilityStringToUInt32(const char *pString, UINT32 *pValue)
{
    UINT32 uRet = 0;
    if ((pString == NULL) || (pValue == NULL)) {
        uRet = 1;
    } else {
        SIZE_t i, Count, idx;
        UINT32 Mul;
        UINT32 base = 10;
        SIZE_t Length = IO_UtilityStringLength(pString);
        if (Length == 0U) {
            // '' return error
            uRet = 1;
        } else {
            Count = Length;
            if (Length >= 2U) {
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


static void StringPrint_PreProc(const char *pFmtString, UINT32 *idx, UINT32 is_return, char *TmpBuffer, UINT32 BufferSize, UINT32 *width)
{

    while (pFmtString[*idx] != '\0') {
        if ((DoesStringContain("bcdefgilopPrRsuxX%", pFmtString[*idx]) == 1U) || (is_return == 1U)) {
            break;
        } else if ((pFmtString[*idx] >= '1') && (pFmtString[*idx] <= '9')) {
            UINT32 idxStart = *idx;
            UINT32 idxEnd = idxStart;
            UINT32 w;
            for (; (IO_UtilityIsDigit((INT32)pFmtString[idxEnd]) != 0U); idxEnd++) {
                /* make misra happy */
            }

            IO_UtilityStringCopy(TmpBuffer, BufferSize, &pFmtString[idxStart]);
            TmpBuffer[idxEnd - idxStart] = '\0';
            if (IO_UtilityStringToUInt32(TmpBuffer, &w) == 0U) {
                *width = w;
            } else {
                *width = 0;
            }

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

static UINT32 StringPrint_FormatCopyCharI32(char *pBuffer, UINT32 BufferSize, UINT32 *nRetIdx, UINT32 *argIdx, INT32 ArgValue)
{
    UINT32 is_return = 0;

    (*argIdx) = (*argIdx) + 1U;
    if ((ArgValue >= 0) && (ArgValue <= 255)) {
        pBuffer[*nRetIdx] = (char)ArgValue;
    } else {
        pBuffer[*nRetIdx] = ' ';
    }
    (*nRetIdx) = (*nRetIdx) + 1U;
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
            IO_UtilityStringAppend(pBuffer, BufferSize, &tmp[1]);
            *string_length -= 1U;
        } else {
            IO_UtilityStringAppend(pBuffer, BufferSize, &tmp[0]);
        }
    } else {
        IO_UtilityStringAppend(pBuffer, BufferSize, &tmp[0]);
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
        string_length = IO_UtilityInt32ToStr(tmp, BufferSize, ArgValue, 10);
    } else if (base == 16) {
        string_length = IO_UtilityUInt32ToStr(tmp, BufferSize, (UINT32)ArgValue, 16);
    } else {
        string_length = IO_UtilityInt32ToStr(tmp, BufferSize, ArgValue, 10);
    }
    return string_length;
}


/**
* Utility print message that contain int32 var
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] pFmtString format of desired string
* @param [in] Count total number of args
* @param [in] pArgs pointer to int32 args array
* @return nRetIdx string ending mark index, e.g. the string length
*
*/
UINT32 IO_UtilityStringPrintInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const INT32 *pArgs)
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
        while (pFmtString[idx] != '\0') {
            if (is_return == 1U) {
                break;
            }
            if (pFmtString[idx] == '%') {

                idx++;
                width = 0;

                StringPrint_PreProc(pFmtString, &idx, is_return, tmp, 40, &width);

                if (pFmtString[idx] == '%') {
                    is_return = StringPrint_FomratCopyPercent(pBuffer, BufferSize, &nRetIdx);
                } else if (pFmtString[idx] == 's') {
                    /* not supported, just print '(null)' */
                    is_return = StringPrint_FormatCopyNull(pBuffer, BufferSize, &nRetIdx, &argIdx);
                } else if (pFmtString[idx] == 'c') {
                    ArgValue = StringPrintInt32_CheckArg(argIdx, Count, pArgs);
                    is_return = StringPrint_FormatCopyCharI32(pBuffer, BufferSize, &nRetIdx, &argIdx, ArgValue);
                } else {

                    StringPrint_IgnoreFormatLL(pFmtString, &idx);

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

                    }
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

                StringPrint_PreProc(pFmtString, &idx, is_return, tmp, 40, &width);

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

                StringPrint_PreProc(pFmtString, &idx, is_return, tmp, 40, &width);

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
                StringPrint_PreProc(pFmtString, &idx, is_return, tmp, 40, &width);

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

void IO_UtilityRegWrite32(UINT64 Address, UINT32 Value)
{
    UINT32 *ptr_u32 = (UINT32 *)Address;
    *ptr_u32 = Value;
}

UINT32 IO_UtilityRegRead32(UINT64 Address)
{
    UINT32 *ptr_u32 = (UINT32 *)Address;
    return *ptr_u32;
}

#include <AmbaReg_ScratchpadNS.h>
#include <AmbaReg_USB.h>
#include <AmbaCSL_USB.h>
#include <AmbaReg_RCT.h>
#include <string.h>

AMBA_SCRATCHPAD_NS_REG_s AmbaScratchpadNS_Reg;
AMBA_USB_REG_s           AmbaUSB_Reg;
AMBA_RCT_REG_s           AmbaRCT_Reg;

AMBA_USB_REG_s           * pAmbaUSB_Reg = &AmbaUSB_Reg;
AMBA_SCRATCHPAD_NS_REG_s * const pAmbaScratchpadNS_Reg = &AmbaScratchpadNS_Reg;
AMBA_RCT_REG_s           * const pAmbaRCT_Reg;

static UINT32 phandle_rct    = 0x01;
static UINT32 phandle_usbphy = 0x02;

static INT32  offset_rct     = 100;
static INT32  offset_usbphy  = 200;

static UINT32 ehci_pin_0 = 1;
static UINT32 ehci_pin_1 = 2;

UINT32 IO_UtilityFDTPropertyU32(INT32 Offset, const char *PropName, UINT32 Index)
{
    UINT32 uret = 0;
    if (Offset == offset_rct) {
        if (strcmp("reg", PropName) == 0) {
            // get UDC base address
            uret = (UINT32)usb_rct_mem;
        }
    }

    if (Offset == offset_usbphy) {
        if (strcmp("amb,pinmux-ids", PropName) == 0) {
            if (Index == 0U) {
                // get EHCI pin 0
                uret = ehci_pin_0;
            } else if (Index == 1U) {
                // get EHCI pin 1
                uret = ehci_pin_1;
            }
        }
    }

    return uret;
}

UINT32 IO_UtilityFDTPropertyU32Quick(INT32 Offset, const char *CompatibleStr, const char *PropName, UINT32 Index)
{
    UINT32 uret = 0;
    if (strcmp("ambarella,udc", CompatibleStr) == 0) {
        if (strcmp("reg", PropName) == 0) {
            // get UDC base address
            uret = (UINT32)&AmbaUSB_Reg;
        } else if (strcmp("amb,rct-regmap", PropName) == 0) {
            // get phandle of RCT
            uret = phandle_rct;
        }
    }

    if (strcmp("ambarella,ehci", CompatibleStr) == 0) {
        if (strcmp("reg", PropName) == 0) {
            uret = (UINT32)usb_ehci_mem;
        }
    }

    if (strcmp("ambarella,ohci", CompatibleStr) == 0) {
        if (strcmp("reg", PropName) == 0) {
            uret = (UINT32)usb_ohci_mem;
        }
    }

    if (strcmp("ambarella,usbphy", CompatibleStr) == 0) {
        if (strcmp("pinctrl-0", PropName) == 0) {
            uret = phandle_usbphy;
        }
    }

    return uret;
}

INT32  IO_UtilityFDTPHandleOffset(UINT32 PHandle)
{
    INT32 iret = 0;

    if (PHandle == phandle_rct) {
        iret = offset_rct;
    }

    if (PHandle == phandle_usbphy) {
        iret = offset_usbphy;
    }

    return iret;
}