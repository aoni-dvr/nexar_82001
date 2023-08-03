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

#include "string.h"
#include "math.h"
#include "AmbaTypes.h"

/******************************************************************************
 *  functions in <string.h>
 ******************************************************************************/

#define ArmStdC_memcpy             memcpy
#define ArmStdC_memcmp             memcmp
#define ArmStdC_memset             memset

#define ArmStdC_strlen             strlen
#define ArmStdC_strcpy(dst,n,src)  strncpy(dst,src,n)

static inline void ArmStdC_strcat(char *pBuffer, UINT32 BufferSize, const char *pSource)
{
    if ((pBuffer == NULL) || (pSource == NULL) || (BufferSize == 0U)) {
        // no action here
    } else {
        SIZE_t StringLengthDest = ArmStdC_strlen(pBuffer);
        SIZE_t StringLengthCopy = ArmStdC_strlen(pSource);
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

/******************************************************************************
 *  functions in <math.h>
 ******************************************************************************/

#define ArmStdC_pow              pow
#define ArmStdC_floor            floor
#define ArmStdC_ceil             ceil

#endif  /* ARM_STD_C_H */
