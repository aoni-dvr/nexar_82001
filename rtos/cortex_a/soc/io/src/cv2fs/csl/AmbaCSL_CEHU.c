/**
 *  @file AmbaCSL_CEHU.c
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
 *  @details Chip Support Library for CEHU
 *
 */

#include <AmbaTypes.h>
#include <AmbaCSL_CEHU.h>

#ifndef AMBA_REG_CEHU_H
#include <AmbaReg_CEHU.h>
#endif
/**
 * Set safety mode
*/
void AmbaCSL_CEHUSetSafetyMode(UINT32 InstanceIdx, UINT32 GroupIdx, UINT32 SafetyMode)
{
    if ((InstanceIdx < AMBA_NUM_CEHU_INSTANCES) && (GroupIdx < 10U)) {
        pAmbaCEHU_Reg[InstanceIdx]->ModeArray[GroupIdx] = SafetyMode;
    }
}

UINT32 AmbaCSL_CEHUGetSafetyMode(UINT32 InstanceIdx, UINT32 GroupIdx)
{
    UINT32 uret = 0;
    if ((InstanceIdx < AMBA_NUM_CEHU_INSTANCES) && (GroupIdx < 10U)) {
        uret = pAmbaCEHU_Reg[InstanceIdx]->ModeArray[GroupIdx];
    }
    return uret;
}


UINT32 AmbaCSL_CEHUGetBitMask(UINT32 InstanceIdx, UINT32 GroupIdx)
{
    UINT32 uret = 0;
    if ((InstanceIdx < AMBA_NUM_CEHU_INSTANCES) && (GroupIdx < 5U)) {
        uret = pAmbaCEHU_Reg[InstanceIdx]->ErrorBitMaskArray[GroupIdx];
    }
    return uret;
}


/**
 * Set mask for error bits
*/
void AmbaCSL_CEHUSetBitMask(UINT32 InstanceIdx, UINT32 GroupIdx, UINT32 Mask)
{
    if ((InstanceIdx < AMBA_NUM_CEHU_INSTANCES) && (GroupIdx < 5U)) {
        pAmbaCEHU_Reg[InstanceIdx]->ErrorBitMaskArray[GroupIdx] = Mask;
    }
}

/**
 * Clear error bits
*/
void AmbaCSL_CEHUClearBitVector(UINT32 InstanceIdx, UINT32 GroupIdx)
{
    if ((InstanceIdx < AMBA_NUM_CEHU_INSTANCES) && (GroupIdx < 5U)) {
        UINT32 value = pAmbaCEHU_Reg[InstanceIdx]->ErrorBitVectorArray[GroupIdx];
        pAmbaCEHU_Reg[InstanceIdx]->ErrorBitVectorArray[GroupIdx] = value;
    }
}

/**
 * Get error bits
*/
UINT32 AmbaCSL_CEHUGetBitVector(UINT32 InstanceIdx, UINT32 GroupIdx)
{
    UINT32 uret = 0;
    if ((InstanceIdx < AMBA_NUM_CEHU_INSTANCES) && (GroupIdx < 5U)) {
        uret = pAmbaCEHU_Reg[InstanceIdx]->ErrorBitVectorArray[GroupIdx];
    }
    return uret;
}

UINT32 AmbaCSL_CEHUGetFirstErrorID(UINT32 InstanceIdx)
{
    return pAmbaCEHU_Reg[InstanceIdx]->ErrorID.ID;
}

void   AmbaCSL_CEHUClearFirstErrorID(UINT32 InstanceIdx)
{
    pAmbaCEHU_Reg[InstanceIdx]->ErrorID.ID = 0;
}


/**
 * Set Safety Check Done for CEHU instance
*/
void AmbaCSL_CEHUSetCheckDone(UINT32 InstanceIdx)
{
    if (InstanceIdx < AMBA_NUM_CEHU_INSTANCES) {
        pAmbaCEHU_Reg[InstanceIdx]->SafetyCheck.CheckDone = 1U;
    }
}


