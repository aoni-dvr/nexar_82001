/*
 *  Dummy header when not using standard libraries from toolchain.
 */

//#ifndef CONFIG_CC_USESTD
#ifndef STDLIB_H
#define STDLIB_H

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG
#include <AmbaWrap.h>
#include <AmbaIOUtility.h>

static char num_base16_full[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

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

static UINT32 UtilityStringToUInt32(const char *pString, UINT32 *pValue)
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

inline static int rand(void)
{
    int v;

    (void)AmbaWrap_rand(&v);

    return v;
}

inline static int atoi(const char *nptr)
{
    UINT32 v = 0;

    (void)UtilityStringToUInt32(nptr, &v);

    return (int)v;
}

#endif /* !STDLIB_H */
//#endif

