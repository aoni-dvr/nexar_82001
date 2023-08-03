/**
 *  @file AmbaUtility.c
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

#include "AmbaUtility.h"
#include "AmbaUtility_Crc32Hw.h"

#define USB_WRAP_FUNC
#ifdef USB_WRAP_FUNC
#include <AmbaWrap.h>
#include <AmbaMisraFix.h>
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  CRC32 table.
\*-----------------------------------------------------------------------------------------------*/
/*-
 *  COPYRIGHT (C) 1986 Gary S. Brown.  You may use this program, or
 *  code or tables extracted from it, as desired without restriction.
 */
static UINT32 AmbaCrc32Table[] = {
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

static char num_base16_full[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static char UInt32ToDigit(UINT32 Value)
{
    char cRet = '0';

    if (Value < 16U) {
        cRet = num_base16_full[Value];
    }
    return cRet;
}

/**
* Utility convert int32 to string
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] Value desired value need to be converted to string
* @param [in] Radix radix of input int32 number, 16 or 10
* @return StrLength string length of conver result
*
*/
UINT32 AmbaUtility_Int32ToStr(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix)
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

/**
* Utility convert unsigned int32 to string
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] Value desired value need to be converted to string
* @param [in] Radix radix of input int32 number, 16 or 10
* @return StrLength string length of conver result
*
*/
UINT32 AmbaUtility_UInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
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

static INT64 PowerX(INT32 Base, INT32 X)
{
    INT64 result = 1;
    INT32 idx = X;
    while(idx > 0) {
        result *= Base;
        idx--;
    }
    return result;
}

/**
* Utility convert float to string
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] Value desired float value need to be converted to string
* @param [in] Afterpoint how many digit need to be showed after dot
* @return StrLength string length of conver result
*
*/
UINT32 AmbaUtility_FloatToStr(char *pBuffer,  UINT32 BufferSize, FLOAT Value, UINT32 Afterpoint)
{
    // Extract integer part
    INT64 ipart = (INT64)Value;
    UINT64 uint64_value;
    UINT32 string_length = 0, tmp_len;
    FLOAT fpart;
    DOUBLE dpart;
    char  uint32_string[UTIL_MAX_INT_STR_LEN];

    // Extract floating part
    fpart = Value - (FLOAT)ipart;
    dpart = fpart;

    // convert integer part to string
    if (ipart == 0) {
        // There is no -0, handle it manually.
        if (BufferSize >= 2U) {
            pBuffer[0] = '\0';
            if (Value < 0.0) {
                AmbaUtility_StringAppend(pBuffer, BufferSize, "-");
                string_length++;
            }
            AmbaUtility_StringAppend(pBuffer, BufferSize, "0");
            string_length++;
        }
    } else{
        string_length = AmbaUtility_Int64ToStr(pBuffer, BufferSize, ipart, 10);
    }


    if (string_length < (BufferSize - 1U)) {
        // check for display option after point
        if (Afterpoint != 0U) {

            pBuffer[string_length] = '.';  // add dot
            string_length++;
            pBuffer[string_length] = '\0';
            if (string_length < (BufferSize - 1U)) {
                // Get the value of fraction part upto given no.
                // of points after dot. The third parameter is needed
                // to handle cases like 233.007
                if (Afterpoint >= 10U) {
                    dpart = dpart * (DOUBLE)PowerX(10, (INT32)Afterpoint);
                    if (dpart < 0.0) {
                        dpart *= (DOUBLE)-1;
                    }
                    uint64_value = (UINT64)dpart;
                } else {
                    fpart = fpart * (FLOAT)PowerX(10, (INT32)Afterpoint);
                    if (fpart < 0.0) {
                        fpart *= (FLOAT)-1;
                    }
                    uint64_value = (UINT64)fpart;
                }
                tmp_len = AmbaUtility_UInt64ToStr(uint32_string, UTIL_MAX_INT_STR_LEN, uint64_value, 10);
                if (tmp_len < Afterpoint) {
                    // add '0'
                    UINT32 i, count;
                    count = Afterpoint - tmp_len;
                    if ((string_length + count) < BufferSize) {
                        for (i = 0; i < count; i++) {
                            pBuffer[string_length] = '0';
                            string_length++;
                        }
                        pBuffer[string_length] = '\0';
                    }
                }
                AmbaUtility_StringAppend(pBuffer, BufferSize, uint32_string);
                if ((string_length + tmp_len) < BufferSize) {
                    string_length += tmp_len;
                }
            }
        }
    }
    return string_length;
}

/**
* Utility convert double to string
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] Value desired float value need to be converted to string
* @param [in] Afterpoint how many digit need to be showed after dot
* @return StrLength string length of conver result
*
*/
UINT32 AmbaUtility_DoubleToStr(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint)
{
    // Extract integer part
    INT64 ipart = (INT64)Value;
    UINT64 uint64_value;
    UINT32 string_length = 0, tmp_len;
    DOUBLE fpart;
    char   uint32_string[UTIL_MAX_INT_STR_LEN];

    // Extract floating part
    fpart = Value - (DOUBLE)ipart;

    // convert integer part to string
    if (ipart == 0) {
        // There is no -0, handle it manually.
        if (BufferSize >= 2U) {
            pBuffer[0] = '\0';
            if (Value < 0.0) {
                AmbaUtility_StringAppend(pBuffer, BufferSize, "-");
                string_length++;
            }
            AmbaUtility_StringAppend(pBuffer, BufferSize, "0");
            string_length++;
        }
    } else{
        string_length = AmbaUtility_Int64ToStr(pBuffer, BufferSize, ipart, 10);
    }

    if (string_length < (BufferSize - 1U)) {
        // check for display option after point
        if (Afterpoint != 0U) {
            pBuffer[string_length] = '.';  // add dot
            string_length++;
            pBuffer[string_length] = '\0';
            if (string_length < (BufferSize - 1U)) {
                // Get the value of fraction part upto given no.
                // of points after dot. The third parameter is needed
                // to handle cases like 233.007
                fpart = fpart * (DOUBLE)PowerX(10, (INT32)Afterpoint);
                if (fpart < 0.0) {
                    fpart *= (DOUBLE)-1;
                }
                uint64_value = (UINT64)fpart;
                tmp_len = AmbaUtility_UInt64ToStr(uint32_string, UTIL_MAX_INT_STR_LEN, uint64_value, 10);
                if (tmp_len < Afterpoint) {
                    // add '0'
                    UINT32 i, count;
                    count = Afterpoint - tmp_len;
                    if ((string_length + count) < BufferSize) {
                        for (i = 0; i < count; i++) {
                            pBuffer[string_length] = '0';
                            string_length++;
                        }
                        pBuffer[string_length] = '\0';
                    }
                }
                AmbaUtility_StringAppend(pBuffer, BufferSize, uint32_string);
                if ((string_length + tmp_len) < BufferSize) {
                    string_length += tmp_len;
                }
            }
        }
    }
    return string_length;
}

/**
* Utility convert int64 to string
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] Value desired float value need to be converted to string
* @param [in] Radix radix of input int64 num, 16 or 10
* @return StrLength string length of conver result
*
*/
UINT32 AmbaUtility_Int64ToStr(char *pBuffer, UINT32 BufferSize, INT64 Value, UINT32 Radix)
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

/**
* Utility convert unsigned int64 to string
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] Value desired float value need to be converted to string
* @param [in] Radix radix of input unsigned int64 num, 16 or 10
* @return StrLength string length of conver result
*
*/
UINT32 AmbaUtility_UInt64ToStr(char *pBuffer, UINT32 BufferSize, UINT64 Value, UINT32 Radix)
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

static UINT32 AmbaUtility_IsDigit(char Value)
{
    UINT32 ret = 0;
    if ((Value >= '0') && (Value <= '9')) {
        ret = 1;
    }
    return ret;
}
static UINT32 DoesStringContain(const char *pSource, char TargetChar)
{
    UINT32 uRet = 0;
    if (pSource != NULL) {
        SIZE_t i;
        SIZE_t Length = AmbaUtility_StringLength(pSource);
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

    while (pFmtString[*idx] != '\0') {
        if ((DoesStringContain("bcdefgilopPrRsuxX%", pFmtString[*idx]) == 1U) || (is_return == 1U)) {
            break;
        } else if ((pFmtString[*idx] >= '1') && (pFmtString[*idx] <= '9')) {
            UINT32 idxStart = *idx;
            UINT32 idxEnd = idxStart;
            UINT32 w;
            for (; (AmbaUtility_IsDigit(pFmtString[idxEnd]) != 0U); idxEnd++) {
                /* make misra happy */
            }

            AmbaUtility_StringCopy(TmpBuffer, BufferSize, &pFmtString[idxStart]);
            TmpBuffer[idxEnd - idxStart] = '\0';
            if (AmbaUtility_StringToUInt32(TmpBuffer, &w) == 0U) {
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
            pBuffer[*nRetIdx] = '0'; *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
            } else {
                pBuffer[*nRetIdx] = 'x'; *nRetIdx += 1U;
                if (*nRetIdx == (BufferSize-1U)) {
                    *is_return = 1;
                }
            }
        }
        *width = 8;
    } else if((pFmtString[idx] == 'r') || (pFmtString[idx] == 'R')) {
        *base = 16;
        if (pFmtString[idx] == 'r') {
            pBuffer[*nRetIdx] = '0'; *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
            } else {
                pBuffer[*nRetIdx] = 'x'; *nRetIdx += 1U;
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
        string_length = AmbaUtility_UInt32ToStr(tmp, BufferSize, ArgValue, 16);
    } else {
        string_length = AmbaUtility_UInt32ToStr(tmp, BufferSize, ArgValue, 10);
    }
    return string_length;
}

static UINT32 StringPrintUInt64_ValueToString(char *tmp, UINT32 BufferSize, INT32 base, UINT64 ArgValue)
{
    UINT32 string_length;
    if (base == 16) {
        string_length = AmbaUtility_UInt64ToStr(tmp, BufferSize, ArgValue, 16);
    } else {
        string_length = AmbaUtility_UInt64ToStr(tmp, BufferSize, ArgValue, 10);
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
    if (ArgValue <= 255U) {
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

static UINT32 StringPrint_FormatCopyCharU64(char *pBuffer, UINT32 BufferSize, UINT32 *nRetIdx, UINT32 *argIdx, UINT64 ArgValue)
{
    UINT32 is_return = 0;

    (*argIdx) = (*argIdx) + 1U;
    if (ArgValue <= 255U) {
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

static UINT32 StringPrint_FormatCopyNull(char *pBuffer, UINT32 BufferSize, UINT32 *nRetIdx, UINT32 *argIdx)
{
    UINT32 is_return = 0;

    (*argIdx) = (*argIdx) + 1U;
    pBuffer[*nRetIdx] = '\0';
    AmbaUtility_StringAppend(pBuffer, BufferSize, "(null)");
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
UINT32 AmbaUtility_StringPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs)
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

                        AmbaUtility_StringAppend(pBuffer, BufferSize, &tmp[0]);

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
UINT32 AmbaUtility_StringPrintUInt64(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT64 *pArgs)
{
    char tmp[40];
    INT32 base = 10;
    UINT32 width, string_length;
    UINT32 idx = 0, nRetIdx = 0, argIdx = 0;
    UINT64 ArgValue;
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

                    ArgValue = StringPrintUInt64_CheckArg(argIdx, Count, pArgs);
                    is_return = StringPrint_FormatCopyCharU64(pBuffer, BufferSize, &nRetIdx, &argIdx, ArgValue);

                } else {

                    StringPrint_IgnoreFormatLL(pFmtString, &idx);

                    if (DoesStringContain("bdiopPrRxXu", pFmtString[idx]) == 1U) {

                        StringPrintUInt32_ParseArg(pBuffer, BufferSize, pFmtString, idx, &base, &nRetIdx, &is_return, &width);
                        if (is_return == 1U) {
                            continue;
                        }

                        ArgValue = StringPrintUInt64_CheckArg(argIdx, Count, pArgs);

                        argIdx++;
                        pBuffer[nRetIdx] = '\0';

                        string_length = StringPrintUInt64_ValueToString(tmp, 40, base, ArgValue);

                        // padding spaces if needed
                        StringPrintUInt32_Padding(pBuffer, BufferSize, width, string_length, &nRetIdx, &is_return);
                        if (is_return == 1U) {
                            continue;
                        }

                        AmbaUtility_StringAppend(pBuffer, BufferSize, &tmp[0]);

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
            AmbaUtility_StringAppend(pBuffer, BufferSize, &tmp[1]);
            *string_length -= 1U;
        } else {
            AmbaUtility_StringAppend(pBuffer, BufferSize, &tmp[0]);
        }
    } else {
        AmbaUtility_StringAppend(pBuffer, BufferSize, &tmp[0]);
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
            pBuffer[*nRetIdx] = '0'; *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
            } else {
                pBuffer[*nRetIdx] = 'x'; *nRetIdx += 1U;
                if (*nRetIdx == (BufferSize-1U)) {
                    *is_return = 1;
                }
            }
        }
        *width = 8;
    } else if((pFmtString[idx] == 'r') || (pFmtString[idx] == 'R')) {
        *base = 16;
        if (pFmtString[idx] == 'r') {
            pBuffer[*nRetIdx] = '0'; *nRetIdx += 1U;
            if (*nRetIdx == (BufferSize-1U)) {
                *is_return = 1;
            } else {
                pBuffer[*nRetIdx] = 'x'; *nRetIdx += 1U;
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
        string_length = AmbaUtility_Int32ToStr(tmp, BufferSize, ArgValue, 10);
    } else if (base == 16) {
        string_length = AmbaUtility_UInt32ToStr(tmp, BufferSize, (UINT32)ArgValue, 16);
    } else {
        string_length = AmbaUtility_Int32ToStr(tmp, BufferSize, ArgValue, 10);
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
UINT32 AmbaUtility_StringPrintInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const INT32 *pArgs)
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

/**
* Utility print message that contain string
* @param [out] pBuffer pointer to output string buffer
* @param [in] BufferSize size of output string buffer
* @param [in] pFmtString format of desired string
* @param [in] Count total number of args
* @param [in] pArgs const pointer to string
* @return nRetIdx string ending mark index, e.g. the string length
*
*/
UINT32 AmbaUtility_StringPrintStr(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const char * const pArgs[])
{
    char tmp[40];
    const char *ArgValue;
    UINT32 width, string_length;
    UINT32 idx = 0, nRetIdx = 0, argIdx = 0;
    UINT32 is_return = 0;

    if ((pBuffer == NULL) || (pFmtString == NULL) || (pArgs == NULL) || (BufferSize == 0U)) {
        // no action
    } else {
        while(pFmtString[idx] != '\0') {
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
                        string_length = (UINT32)AmbaUtility_StringLength(ArgValue);
                        StringPrintStr_Padding(pBuffer, BufferSize, width, string_length, &nRetIdx, &is_return);
                        if (is_return == 1U) {
                           continue;
                        }
                        AmbaUtility_StringAppend(pBuffer, BufferSize, ArgValue);
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

#ifdef CONFIG_ARM32
static UINT32 crc32_arm32_hw(UINT32 *crc, const UINT8 *p, UINT32 len)
{
    UINT32 length = len;
    UINT32 index = 0;

    for(;;) {
        if((length/sizeof(UINT32)) > 0U) {
            UINT32 value = ((UINT32)p[index+3U] << 24U) |
                           ((UINT32)p[index+2U] << 16U) |
                           ((UINT32)p[index+1U] << 8U)  |
                           ((UINT32)p[index]);
            CRC32W(crc, value);
            index += sizeof(UINT32);
            length -= sizeof(UINT32);
        } else {
            break;
        }
    }

    if ((length/sizeof(UINT16)) > 0U) {
        UINT16 value = ((UINT16)p[index+1U] << 8U)  |
                       ((UINT16)p[index]);
        CRC32H(crc, value);
        index += sizeof(UINT16);
        length -= sizeof(UINT16);
    }

    if ((length/sizeof(UINT8)) > 0U) {
        CRC32B(crc, p[index]);
    }
    return *crc;
}
#else
static UINT32 crc32_arm64_hw(UINT32 *crc, const UINT8 *p, UINT32 len)
{
    SIZE_t length = len;
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
            CRC32X(crc,value);
            index += sizeof(UINT64);
            length -= sizeof(UINT64);
        } else {
            break;
        }
    }
    if ((length/sizeof(UINT32)) > 0U) {
        UINT32 value = ((UINT32)p[index+3U] << 24U) |
                       ((UINT32)p[index+2U] << 16U) |
                       ((UINT32)p[index+1U] << 8U)  |
                       ((UINT32)p[index]);
        CRC32W(crc, value);
        index += sizeof(UINT32);
        length -= sizeof(UINT32);
    }

    if ((length/sizeof(UINT16)) > 0U) {
        UINT16 value = ((UINT16)p[index+1U] << 8U)  |
                       ((UINT16)p[index]);
        CRC32H(crc, value);
        index += sizeof(UINT16);
        length -= sizeof(UINT16);
    }

    if ((length/sizeof(UINT8)) > 0U) {
        CRC32B(crc, p[index]);
    }
    return *crc;
}
#endif

/**
* Utility add CRC32
* @param [in] pBuffer pointer to data buffer that need to add CRC
* @param [in] Size size of buffer
* @param [in] Crc CRC32 init value
* @return calculated CRC32 result
*
*/
UINT32 AmbaUtility_Crc32Add(const UINT8 *pBuffer, UINT32 Size, UINT32 Crc)
{
    return AmbaUtility_Crc32AddHw(pBuffer, Size, Crc);
}

/**
* Utility add CRC32 by hw
* @param [in] pBuffer pointer to data buffer that need to add CRC
* @param [in] Size size of buffer
* @param [in] Crc CRC32 init value
* @return calculated CRC32 result
*
*/
UINT32 AmbaUtility_Crc32AddHw(const UINT8 *pBuffer, UINT32 Size, UINT32 Crc)
{
    const UINT8 *p;
    UINT32 crcX = Crc;

    p = pBuffer;
#ifdef CONFIG_ARM32
    return crc32_arm32_hw(&crcX, p, Size);
#else
    return crc32_arm64_hw(&crcX, p, Size);
#endif

}

/**
* Utility add CRC32 by sw
* @param [in] pBuffer pointer to data buffer that need to add CRC
* @param [in] Size size of buffer
* @param [in] Crc CRC32 init value
* @return calculated CRC32 result
*
*/
UINT32 AmbaUtility_Crc32AddSw(const UINT8 *pBuffer, UINT32 Size, UINT32 Crc)
{
    const UINT8 *p;
    UINT32 idx;
    UINT32 crcX = Crc;
    UINT32 count = Size;

    p = pBuffer;
    while (count > 0U) {
        idx = (crcX ^ (UINT32)*p) & 0x0FFU;
        crcX = AmbaCrc32Table[idx] ^ (crcX >> 8U);
        p++;
        count--;
    }

    return crcX;
}

/**
* Utility CRC32
* @param [in] pBuffer pointer to data buffer that need to add CRC
* @param [in] Size size of buffer
* @return calculated CRC32 result
*
*/
UINT32 AmbaUtility_Crc32(const UINT8 *pBuffer, UINT32 Size)
{
    UINT32 Result = 0x0U;

    if (pBuffer != NULL) {
        Result = AmbaUtility_Crc32Hw(pBuffer, Size);
    }

    return Result;
}

/**
* Utility CRC32 by hw
* @param [in] pBuffer pointer to data buffer that need to add CRC
* @param [in] Size size of buffer
* @return xor calculated CRC32 result
*
*/
UINT32 AmbaUtility_Crc32Hw(const UINT8 *pBuffer, UINT32 Size)
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

/**
* Utility CRC32 by sw
* @param [in] pBuffer pointer to data buffer that need to add CRC
* @param [in] Size size of buffer
* @return xor calculated CRC32 result
*
*/
UINT32 AmbaUtility_Crc32Sw(const UINT8 *pBuffer, UINT32 Size)
{
    const UINT8 *p;
    UINT32 Crc;
    UINT32 idx;
    UINT32 count = Size;

    p = pBuffer;
    Crc = ~0U;

    while (count > 0U) {
        idx = (Crc ^ (UINT32)*p) & 0x0FFU;
        Crc = AmbaCrc32Table[idx] ^ (Crc >> 8U);
        p++;
        count--;
    }

    return Crc ^ ~0U;
}

/**
* Utility CRC32 finalize
* @param [in] LastValue last value of checked data
* @return xor result of last value and CRC32 init value
*
*/
UINT32 AmbaUtility_Crc32Finalize(UINT32 LastValue)
{
    return (LastValue ^ AMBA_CRC32_INIT_VALUE);
}

/**
* Utility CRC32 finalize by sw
* @param [in] LastValue last value of checked data
* @return xor result of last value and CRC32 init value
*
*/
UINT32 AmbaUtility_Crc32FinalizeSw(UINT32 LastValue)
{
    return AmbaUtility_Crc32Finalize(LastValue);
}

/**
* Utility string copy
* @param [in] pDest pointer to copy destination buffer
* @param [in] DestSize size of destination buffer
* @param [in] pSource pointer to copy source buffer
* @return none
*
*/
void AmbaUtility_StringCopy(char *pDest, SIZE_t DestSize, const char *pSource)
{
    if ((pDest == NULL) || (pSource == NULL) || (DestSize == 0U)) {
        // no action
    } else {
        SIZE_t i = 0;
        while ((pSource[i] != '\0') && (i < (DestSize - 1U))) {
            pDest[i] = pSource[i];
            i++;
        }

        if (i >= DestSize) {
            pDest[DestSize - 1U] = '\0';
        } else {
            pDest[i] = '\0';
        }
    }
}

/**
* Utility string length
* @param [in] pString pointer to target string
* @return Length string length of input string
*
*/
SIZE_t AmbaUtility_StringLength(const char *pString)
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

/**
* Utility string compare
* @param [in] pString1 compare target string 1
* @param [in] pString2 compare target string 2
* @param [in] Size of compare length
* @return nRet comapre result, 0 match, -1 not matching
*
*/
INT32 AmbaUtility_StringCompare(const char *pString1, const char *pString2, SIZE_t Size)
{
    SIZE_t StringLength1, StringLength2;
    INT32 nRet = 0;
    if ((pString1 == NULL) || (pString2 == NULL) || (Size == 0U)) {
        // return no equal
        nRet = -1;
    } else {
        SIZE_t i;
        StringLength1 = AmbaUtility_StringLength(pString1);
        StringLength2 = AmbaUtility_StringLength(pString2);
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

/**
* Utility string append
* @param [in] pBuffer poiter to origin string buffer
* @param [in] BufferSize size of origin string buffer
* @param [in] pSource pointer to string to be copied
* @return none
*
*/
void AmbaUtility_StringAppend(char *pBuffer, UINT32 BufferSize, const char *pSource)
{
    SIZE_t buffer_size = BufferSize;
    if ((pBuffer == NULL) || (pSource == NULL) || (buffer_size == 0U)) {
        // no action here
    } else {
        SIZE_t StringLengthDest = AmbaUtility_StringLength(pBuffer);
        SIZE_t StringLengthCopy = AmbaUtility_StringLength(pSource);
        SIZE_t i;

        if ((StringLengthCopy >= (buffer_size - 1U)) || (StringLengthDest >= (buffer_size - 1U))) {
            // buffer overflow, no action here
        } else {
            if ((StringLengthDest + StringLengthCopy) > (buffer_size - 1U)) {
                StringLengthCopy = (buffer_size - StringLengthDest) - 1U;
            }
            for (i = 0; i < StringLengthCopy; i++) {
                pBuffer[i + StringLengthDest] = pSource[i];
            }
            if ((StringLengthDest + StringLengthCopy) <= (buffer_size - 1U)) {
                pBuffer[StringLengthDest + StringLengthCopy] = '\0';
            } else {
                pBuffer[buffer_size - 1U] = '\0';
            }
        }
    }
}

/**
* Utility string append int32
* @param [in] pBuffer poiter to origin string buffer
* @param [in] BufferSize size of origin string buffer
* @param [in] Value int32 value need to be added into string
* @param [in] Radix radix of input int32 number, 16 or 10
* @return none
*
*/
void AmbaUtility_StringAppendInt32(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix)
{
    char IntString[UTIL_MAX_INT_STR_LEN];

    if ((pBuffer != NULL) && (BufferSize > 0U)) {
        UINT32 Length = AmbaUtility_Int32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Value, Radix);
        if (Length > 0U) {
            AmbaUtility_StringAppend(pBuffer, BufferSize, IntString);
        }
    }
}

/**
* Utility string append unsigned int32
* @param [in] pBuffer poiter to origin string buffer
* @param [in] BufferSize size of origin string buffer
* @param [in] Value uint32 value need to be added into string
* @param [in] Radix radix of input uint32 number, 16 or 10
* @return none
*
*/
void AmbaUtility_StringAppendUInt32(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    char IntString[UTIL_MAX_INT_STR_LEN];

    if (pBuffer != NULL) {
        UINT32 Length = AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Value, Radix);
        if (Length > 0U) {
            AmbaUtility_StringAppend(pBuffer, BufferSize, IntString);
        }
    }
}

/**
* Utility string append float
* @param [in] pBuffer poiter to origin string buffer
* @param [in] BufferSize size of origin string buffer
* @param [in] Value float value need to be added into string
* @param [in] AfterPoint how many digit need to be showed after dot
* @return none
*
*/
void AmbaUtility_StringAppendFloat(char *pBuffer, UINT32 BufferSize, FLOAT Value, UINT32 AfterPoint)
{
    char IntString[UTIL_MAX_FLOAT_STR_LEN];

    if (pBuffer != NULL) {
        UINT32 Length = AmbaUtility_FloatToStr(IntString, UTIL_MAX_FLOAT_STR_LEN, Value, AfterPoint);
        if (Length > 0U) {
            AmbaUtility_StringAppend(pBuffer, BufferSize, IntString);
        }
    }
}

/**
* Utility string append double
* @param [in] pBuffer poiter to origin string buffer
* @param [in] BufferSize size of origin string buffer
* @param [in] Value double value need to be added into string
* @param [in] AfterPoint how many digit need to be showed after dot
* @return none
*
*/
void AmbaUtility_StringAppendDouble(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 AfterPoint)
{
    char IntString[UTIL_MAX_FLOAT_STR_LEN];

    if (pBuffer != NULL) {
        UINT32 Length = AmbaUtility_DoubleToStr(IntString, UTIL_MAX_FLOAT_STR_LEN, Value, AfterPoint);
        if (Length > 0U) {
            AmbaUtility_StringAppend(pBuffer, BufferSize, IntString);
        }
    }
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

/**
* Utility string to unsigned int32
* @param [in] pString source string need to be converted to unsigned int32
* @param [out] pValue unsigned int32 conver result
* @return uRet 0 OK, 1 error
*
*/
UINT32 AmbaUtility_StringToUInt32(const char *pString, UINT32 *pValue)
{
    UINT32 uRet = 0;
    SIZE_t Length = AmbaUtility_StringLength(pString);
    if ((pString == NULL) || (pValue == NULL) || (Length == 0U)) {
        uRet = 1;
    } else {
        SIZE_t i, Count, idx;
        UINT32 base = 10;
        UINT32 Mul;
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
    return uRet;
}

/**
* Utility string to unsigned int64
* @param [in] pString source string need to be converted to unsigned int64
* @param [out] pValue unsigned int64 conver result
* @return uRet 0 OK, 1 error
*
*/
UINT32 AmbaUtility_StringToUInt64(const char *pString, UINT64 *pValue)
{
    UINT32 uRet = 0;
    SIZE_t Length = AmbaUtility_StringLength(pString);
    if ((pString == NULL) || (pValue == NULL) || (Length == 0U)) {
        uRet = 1;
    } else {
        SIZE_t i, Count, idx;
        UINT64 Mul;
        UINT32 base = 10;
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
    return uRet;
}

/**
* Utility memory set char
* @param [in] pBuffer pointer of target buffer
* @param [in] Value specified char need to be filled in buffer
* @param [in] Size buffer size
* @return none
*
*/
void AmbaUtility_MemorySetChar(char *pBuffer, char Value, UINT32 Size)
{
#ifdef USB_WRAP_FUNC
    INT32 value_to_set = (INT32)(UINT8)Value;
    void *ptr_to_set;
    AmbaMisra_TypeCast(&ptr_to_set, &pBuffer);
    AmbaMisra_TouchUnused(pBuffer);
    if (AmbaWrap_memset(ptr_to_set, value_to_set, Size) != 0U) {
        // action TBD
    }
#else
    char *ptr = pBuffer;
    UINT32 Count = Size / sizeof(char);

    if (ptr != NULL) {
        while(Count != 0U) {
            *ptr =  Value;
            Count = Count - 1U;
            ptr++;
        }
    }
#endif
    return;
}

/**
* Utility memory set unsigned int8
* @param [in] pBuffer pointer of target buffer
* @param [in] Value specified unsigned int8 need to be filled in buffer
* @param [in] Size buffer size
* @return none
*
*/
void AmbaUtility_MemorySetU8(UINT8 *pBuffer, UINT8 Value, UINT32 Size)
{
#ifdef USB_WRAP_FUNC
    INT32 value_to_set = (INT32)Value;
    void *ptr_to_set;
    AmbaMisra_TypeCast(&ptr_to_set, &pBuffer);
    AmbaMisra_TouchUnused(pBuffer);
    if (AmbaWrap_memset(ptr_to_set, value_to_set, Size) != 0U) {
        // action TBD
    }
#else
    UINT8 *ptr = pBuffer;
    UINT32 Count = Size / sizeof(UINT8);

    if (ptr != NULL) {
        while(Count != 0U) {
            *ptr =  Value;
            Count = Count - 1U;
            ptr++;
        }
    }
#endif
    return;
}

/**
* Utility memory set unsigned int32
* @param [in] pBuffer pointer of target buffer
* @param [in] Value specified unsigned int32 need to be filled in buffer
* @param [in] Size buffer size
* @return none
*
*/
void AmbaUtility_MemorySetU32(UINT32 *pBuffer, UINT32 Value, UINT32 Size)
{
#ifdef USB_WRAP_FUNC
    INT32 value_to_set = (INT32)Value;
    void *ptr_to_set;
    AmbaMisra_TypeCast(&ptr_to_set, &pBuffer);
    AmbaMisra_TouchUnused(pBuffer);
    if (AmbaWrap_memset(ptr_to_set, value_to_set, Size) != 0U) {
        // action TBD
    }
#else
    UINT32 *ptr = pBuffer;
    UINT32 Count = Size / sizeof(UINT32);

    if (ptr != NULL) {
        while(Count != 0U) {
            *ptr =  Value;
            Count = Count - 1U;
            ptr++;
        }
    }
#endif
    return;
}

/**
* Utility strtok
* @param [in] pString pointer of target string
* @param [in] Delimiter word to be the split key
* @param [out] Args char array to save token
* @param [out] ArgCount total number of token
* @return uRet 0 ok, 0xFFFFFFFFU error
*
*/
UINT32 AmbaUtility_StringToken(const char *pString, char Delimiter, char Args[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH], UINT32 *ArgCount)
{
    UINT32 uRet = 0;
    SIZE_t Idx;
    SIZE_t Length;
    UINT32 ArgPos = 0;
    UINT32 Count = 0;

    if ((pString == NULL) || (Args == NULL) || (ArgCount == NULL)) {
        uRet = 0xFFFFFFFFU;
    } else {
        Length = AmbaUtility_StringLength(pString);
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
