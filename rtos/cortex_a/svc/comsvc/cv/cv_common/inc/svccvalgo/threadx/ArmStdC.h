/**
*  @file ArmStdC.h
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
*   @details The standard C lib
*
*/

#ifndef ARM_STD_C_H
#define ARM_STD_C_H

#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "ArmLog.h"

#define ARM_LOG_STDC    "STDC"

/******************************************************************************
 *  functions in <stdio.h>
 ******************************************************************************/

#define ArmStdC_sprintfU32       AmbaUtility_StringPrintUInt32
#define ArmStdC_sprintfI32       AmbaUtility_StringPrintInt32
#define ArmStdC_sprintfStr       AmbaUtility_StringPrintStr

/******************************************************************************
 *  functions in <string.h>
 ******************************************************************************/

#define ArmStdC_memcmp           AmbaWrap_memcmp

#define ArmStdC_strtok           AmbaUtility_StringToken
#define ArmStdC_strtoul          AmbaUtility_StringToUInt32
#define ArmStdC_strtoull         AmbaUtility_StringToUInt64

#define ArmStdC_strlen           AmbaUtility_StringLength
#define ArmStdC_strcpy           AmbaUtility_StringCopy
#define ArmStdC_strcat           AmbaUtility_StringAppend

static inline void ArmStdC_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    UINT32 Rval = 0;

    Rval = AmbaWrap_memcpy(pDst, pSrc, num);
    if (0U != Rval) {
        ArmLog_ERR(ARM_LOG_STDC, "## AmbaWrap_memcpy() fail (Rval = 0x%x)", Rval, 0U);
    }
}

static inline void ArmStdC_memset(void *ptr, INT32 v, SIZE_t n)
{
    UINT32 Rval = 0;

    Rval = AmbaWrap_memset(ptr, v, n);
    if (0U != Rval) {
        ArmLog_ERR(ARM_LOG_STDC, "## AmbaWrap_memset() fail (Rval = 0x%x)", Rval, 0U);
    }
}

static inline INT32 ArmStdC_strcmp(const char *pStr1, const char *pStr2)
{
    INT32   Rval = 0;
    UINT32  Len1 = 0, Len2 = 0;

    if ((pStr1 != NULL) && (pStr2 != NULL)) {
        Len1 = ArmStdC_strlen(pStr1);
        Len2 = ArmStdC_strlen(pStr2);

        if (Len2 < Len1) {
            Rval = 1;
        } else if (Len1 < Len2) {
            Rval = -1;
        } else {
            Rval = AmbaUtility_StringCompare(pStr1, pStr2, Len2);
        }
    }

    return Rval;
}

/******************************************************************************
 *  functions in <math.h>
 ******************************************************************************/

static inline DOUBLE ArmStdC_pow(DOUBLE base, DOUBLE exponent)
{
    DOUBLE Rval = 0.0f;
    UINT32 Ret = 0;

    Ret = AmbaWrap_pow(base, exponent, &Rval);
    if (0U != Ret) {
        ArmLog_ERR(ARM_LOG_STDC, "## AmbaWrap_pow() fail (Rval = 0x%x)", Ret, 0U);
    }

    return Rval;
}

static inline DOUBLE ArmStdC_floor(DOUBLE x)
{
    DOUBLE Rval = 0.0f;
    UINT32 Ret = 0;

    Ret = AmbaWrap_floor(x, &Rval);
    if (0U != Ret) {
        ArmLog_ERR(ARM_LOG_STDC, "## ArmStdC_floor() fail (Rval = 0x%x)", Ret, 0U);
    }

    return Rval;
}

static inline DOUBLE ArmStdC_ceil(DOUBLE x)
{
    DOUBLE Rval = 0.0f;
    UINT32 Ret = 0;

    Ret = AmbaWrap_ceil(x, &Rval);
    if (0U != Ret) {
        ArmLog_ERR(ARM_LOG_STDC, "## ArmStdC_ceil() fail (Rval = 0x%x)", Ret, 0U);
    }

    return Rval;
}

#endif  /* ARM_STD_C_H */
