/**
 *  @file AmbaCSL_ECRU.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Chip Support Library for ECRU
 *
 */

#include "AmbaTypes.h"
#include "AmbaCSL_ECRU.h"


/**
 *
 *  @param [IN] VectorIdx
 *  @param [IN] Mask
 */
void AmbaCSL_ECRUSetOutputMask(UINT32 VectorIdx, UINT32 Mask)
{
    if (VectorIdx < AMBA_NUM_ECRU_ARRAY) {
        pAmbaECRU_Reg->OutputMaskArray[VectorIdx] = Mask;
    }
}

UINT32 AmbaCSL_ECRUGetOutputMask(UINT32 VectorIdx)
{
    UINT32 uret = 0;
    if (VectorIdx < AMBA_NUM_ECRU_ARRAY) {
        uret = pAmbaECRU_Reg->OutputMaskArray[VectorIdx];
    }
    return uret;
}

/**
 *
 *  @param [IN] VectorIdx
 *  @param [IN] Mask
 */
void AmbaCSL_ECRUSetInputMask(UINT32 VectorIdx, UINT32 Mask)
{
    if (VectorIdx < AMBA_NUM_ECRU_ARRAY) {
        pAmbaECRU_Reg->InputMaskArray[VectorIdx] = Mask;
    }
}

UINT32 AmbaCSL_ECRUGetInputMask(UINT32 VectorIdx)
{
    UINT32 uret = 0;
    if (VectorIdx < AMBA_NUM_ECRU_ARRAY) {
        uret = pAmbaECRU_Reg->InputMaskArray[VectorIdx];
    }
    return uret;
}


/**
 *
 *  @param [IN] VectorIdx
 *  @param [IN] Mask
 */
void AmbaCSL_ECRUSetClearError(UINT32 VectorIdx, UINT32 Value)
{
    if (VectorIdx < AMBA_NUM_ECRU_ARRAY) {
        pAmbaECRU_Reg->ErrorClearArray[VectorIdx] = Value;
    }
}

/**
 *
 *  @param [IN] VectorIdx
 *  @return
 */
UINT32 AmbaCSL_ECRUGetError(UINT32 VectorIdx)
{
    UINT32 uret = 0U;
    if (VectorIdx < AMBA_NUM_ECRU_ARRAY) {
        uret = pAmbaECRU_Reg->ErrorLogArray[VectorIdx];
    }
    return uret;
}

