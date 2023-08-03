/**
 *  @file SvcBase64.c
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
 *  @details base 64 encode module
 *
 */

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "SvcBase64.h"

/**
* encode byte array with base64.
* @param [in]  byte array pointer to be encoded.
* @param [in]  byte array length, length of origSigned.
* @param [out]  encoded string.
* @param [in]  output buffer length.
* @return ErrorCode
*/
UINT32 SvcBase64_Encode(char const* pOrigSigned, UINT32 OrigLength, char* pResult, UINT32 MaxLength)
{
    UINT32 Index;
    static const char Base64Char[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    UINT8 const* pOrig = (UINT8 const*)pOrigSigned; // in case any input bytes have the MSB set
    const UINT32 NumOrig24BitValues = OrigLength / 3U;
    UINT8 HavePadding = (OrigLength > (NumOrig24BitValues * 3U)) ? 1U : 0U;
    UINT8 havePadding2 = (OrigLength == ((NumOrig24BitValues * 3U) + 2U)) ? 1U : 0U;
    const UINT32 NumResultBytes = 4U * (NumOrig24BitValues + HavePadding);
    UINT32 RetVal = OK;

    if ((NULL == pOrig) || (NULL == pResult)) {
        RetVal = ERR_NA;
    } else if ((NumResultBytes + 1U) > MaxLength) {
        AmbaPrint_PrintUInt5("SvcBase64_Encode: result buffer(%u) is not enough(%u)", MaxLength, NumResultBytes + 1U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        if (AmbaWrap_memset(pResult, 0, NumResultBytes + 1U)!= 0U) { }
        // Map each full group of 3 input bytes into 4 output base-64 characters:
        for (Index = 0U; Index < NumOrig24BitValues; ++Index) {
            pResult[(4U * Index)     ] = Base64Char[(pOrig[3U * Index] >> 2U) & 0x3FU];
            pResult[(4U * Index) + 1U] = Base64Char[(((pOrig[3U * Index] & 0x3U) << 4U) | (pOrig[(3U * Index) + 1U] >> 4U)) & 0x3FU];
            pResult[(4U * Index) + 2U] = Base64Char[((pOrig[(3U * Index) + 1U] << 2U) | (pOrig[(3U * Index) + 2U] >> 6U)) & 0x3FU];
            pResult[(4U * Index) + 3U] = Base64Char[pOrig[(3U * Index) + 2U] & 0x3FU];
        }

        // Now, take padding into account.  (Note: i == numOrig24BitValues)
        if (0U != HavePadding) {
            pResult[4U * Index] = Base64Char[(pOrig[3U * Index] >> 2U) & 0x3FU];
            if (0U != havePadding2) {
                pResult[(4U * Index) + 1U] = Base64Char[(((pOrig[3U * Index] & 0x3U) << 4U) | (pOrig[(3U * Index) + 1U] >> 4U)) & 0x3FU];
                pResult[(4U * Index) + 2U] = Base64Char[(pOrig[(3U * Index) + 1U] << 2U) & 0x3FU];
            } else {
                pResult[(4U * Index) + 1U] = Base64Char[((pOrig[3U * Index] & 0x3U) << 4U) & 0x3FU];
                pResult[(4U * Index) + 2U] = '=';
            }
            pResult[(4U * Index) + 3U] = '=';
        }

        pResult[NumResultBytes] = '\0';
    }
    return RetVal;
}

