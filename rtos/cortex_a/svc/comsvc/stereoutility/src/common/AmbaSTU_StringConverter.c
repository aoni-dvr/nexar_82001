/**
 *  @file AmbaSTU_StringConverter.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details stereo utility
 *
 */
#include "AmbaTypes.h"
#include "AmbaSTU_ErrNo.h"
#include "AmbaWrap.h"
#include "AmbaSTU_StringConverter.h"
static inline INT32 STU_DBtoS32(DOUBLE Number)
{
    return (INT32) Number;
}

static inline UINT64 STU_DBtoU64(DOUBLE Number)
{
    return (UINT64) Number;
}

static inline UINT32 STU_StrPutChar(UINT8 *pStr, UINT32 StrBufLen, UINT32 Index, const UINT8 C)
{
    UINT32 Rval = STU_OK;
    if (StrBufLen > Index) {
        pStr[Index] = C;
    } else {
        Rval = STU_ERROR_GENERAL;
    }
    return Rval;
}

UINT32 STU_U32toStr(UINT8 *pStr, UINT32 StrBufLen, UINT32 Number, UINT32 *pStrLen)
{
    UINT32 Rval = STU_OK;
    if (Number == 0U) {
        Rval |= STU_StrPutChar(pStr, StrBufLen, 0U, 0x30U/*'0'*/);
        Rval |= STU_StrPutChar(pStr, StrBufLen, 1U, 0x0U/*'\0'*/);
        *pStrLen = 0U;
    } else {
        UINT32 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT32 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT32 CopyOfNumber;
        UINT8 ModResultCharMisra;
        CopyOfNumber = NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (UINT8)ModResultAdd48;
            NumberMisra = NumberMisra / 10U;
            Rval = STU_StrPutChar(pStr, StrBufLen, Length - (Divide + (UINT32)1U), ModResultCharMisra);
            if (Rval != 0U) {
                break;
            }
        }
        Rval = STU_StrPutChar(pStr, StrBufLen, Length, 0x0U/*'\0'*/);
        *pStrLen = Length;
    }
    return Rval;
}


UINT32 STU_U64toStr(UINT8 *pStr, UINT32 StrBufLen, UINT64 Number, UINT32 *pStrLen)
{
    UINT32 Rval = STU_OK;
    if (Number == 0U) {
        Rval |= STU_StrPutChar(pStr, StrBufLen, 0U, 0x30U/*'0'*/);
        Rval |= STU_StrPutChar(pStr, StrBufLen, 1U, 0x0U/*'\0'*/);
        *pStrLen = 0U;
    } else {
        UINT64 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT64 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT64 CopyOfNumber;
        UINT8 ModResultCharMisra;
        CopyOfNumber = NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (UINT8)ModResultAdd48;
            NumberMisra = NumberMisra / 10U;
            Rval = STU_StrPutChar(pStr, StrBufLen, Length - (Divide + (UINT32)1U), ModResultCharMisra);
            if (Rval != 0U) {
                break;
            }
        }
        Rval = STU_StrPutChar(pStr, StrBufLen, Length, 0x0U/*'\0'*/);
        *pStrLen = Length;
    }
    return Rval;
}


UINT32 STU_DBtoStr(UINT8 *pStr, UINT32 StrBufLen, DOUBLE Number, UINT32 *pStrLen)
{
    const UINT32 MAX_ACCURACY = 6U;
    UINT32 Rval = STU_OK;
    if(Number == 0.0) {
        Rval |= STU_StrPutChar(pStr, StrBufLen, 0U, 0x30U/*'0'*/);
        Rval |= STU_StrPutChar(pStr, StrBufLen, 1U, 0x0U/*'\0'*/);
        *pStrLen = 0U;
    } else {
        UINT32 LengthU64 = 0U;
        UINT32 Length = 0U;
        UINT32 IsNegative = 0U;
        UINT64 Decimal;
        UINT32 i;
        UINT32 ZeroNum = 0;
        UINT32 Accuracy;
        DOUBLE PowMsira;
        DOUBLE NumberMisra = Number;
        if(NumberMisra < 0.0) {
            IsNegative = 1U;
            NumberMisra = 0.0 - NumberMisra;
            Length++;
        }
        if(IsNegative == 1U) {
            Rval = STU_StrPutChar(pStr, StrBufLen, 0U, 0x2DU/*'-'*/);
        }
        if (0U != STU_U64toStr(&pStr[Length], StrBufLen - 1U, (UINT64)NumberMisra, &LengthU64)) {
            Rval = STU_ERROR_GENERAL;
        }
        NumberMisra = NumberMisra - (DOUBLE)(STU_DBtoS32(NumberMisra));
        Length = Length + LengthU64;
        if (0U != STU_StrPutChar(pStr, StrBufLen, Length, 0x2EU/*'.'*/)) {
            Rval = STU_ERROR_GENERAL;
        }
        Length++;
        // Note: check usable decimal num:
        Accuracy = StrBufLen - Length - 1U;
        if (Accuracy > MAX_ACCURACY) {
            Accuracy = MAX_ACCURACY;
        }

        if (0U != AmbaWrap_pow(10.0, (DOUBLE)Accuracy + 1.0, &PowMsira)) {
            Rval = STU_ERROR_GENERAL;
        }
        Decimal = STU_DBtoU64(NumberMisra * PowMsira);
        if ((Decimal % 10U) >= 5U){
            Decimal += 5U;
        }
        Decimal /= 10U;
        for (i = 0; i < Accuracy; i++) {
            if (0U != AmbaWrap_pow(10.0, (DOUBLE)i, &PowMsira)) {
                Rval = STU_ERROR_GENERAL;
            }
            if (Decimal >= (UINT64)PowMsira) {
                ZeroNum = Accuracy - 1U - i;
            }
        }
        for (i = 0; i < ZeroNum; i++) {
            pStr[Length] = 0x30U/*'0'*/;
            Length++;
        }
        if (0U != STU_U64toStr(&pStr[Length], StrBufLen - Length, (UINT64)Decimal, &LengthU64)) {
            Rval = STU_ERROR_GENERAL;
        }
        Length += LengthU64;
        *pStrLen = Length;
    }
    return Rval;
}

