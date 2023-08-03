/**
 *  @file AmbaB8CSL_PLL.h
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
 *  @details Definitions & Constants for B8 PLL control APIs
 *
 */

#ifndef B8_CSL_PLL_H
#define B8_CSL_PLL_H

#include "AmbaB8Reg_RCT.h"

/*-----------------------------------------------------------------------------------------------*\
 * Macro Definitions
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
* Defined in AmbaB8_MmioBase.asm
\*-----------------------------------------------------------------------------------------------*/
extern B8_RCT_REG_s *const pAmbaB8_PllReg;

typedef struct {
    B8_PLL_CTRL_REG_s         Ctrl;           /* Control Register */
    B8_PLL_CTRL2_REG_s        Ctrl2;          /* Control Register-2 */
    B8_PLL_CTRL3_REG_s        Ctrl3;          /* Control Register-3 */
    UINT32                    Fraction;       /* Fraction */
} B8_PLL_CTRL_REGS_VAL_s;

UINT32 AmbaCSL_B8PllGetPostScaleRegVal(UINT32 ChipID, UINT32 PllIdx);
UINT32 AmbaCSL_B8PllSetPostScaleRegVal(UINT32 ChipID, UINT32 PllIdx, UINT32 DivVal);

UINT32 AmbaCSL_B8PllGetCtrlRegsVal(UINT32 ChipID, UINT32 PllIdx, B8_PLL_CTRL_REGS_VAL_s *pPllCtrlRegsVal);
UINT32 AmbaCSL_B8PllSetCtrlRegsVal(UINT32 ChipID, UINT32 PllIdx, const B8_PLL_CTRL_REGS_VAL_s *pCurRegsVal, B8_PLL_CTRL_REGS_VAL_s *pNewRegsVal);
UINT32 AmbaCSL_B8PllPowerDown(UINT32 ChipID, UINT32 PllIdx);
UINT32 AmbaCSL_B8PllFractionalMode(UINT32 ChipID, UINT32 PllIdx, UINT8 UseFractional);
UINT32 AmbaCSL_B8PllCheckLockStatus(UINT32 ChipID, UINT32 PllIdx);
UINT32 AmbaCSL_B8PllRescale(UINT32 ChipID, UINT32 PllIdx, UINT32 RefFreq, INT32 FrequencyDiff);
void AmbaCSL_B8PllSetPllCtrl(UINT32 ChipID, UINT32 PllIdx, const UINT32 *pPllCtrlRegsVal);
void AmbaCSL_B8PllSetPllCtrl2(UINT32 ChipID, UINT32 PllIdx, const UINT32 *pPllCtrlRegsVal);
void AmbaCSL_B8PllSetPllCtrl3(UINT32 ChipID, UINT32 PllIdx, const UINT32 *pPllCtrlRegsVal);
void AmbaCSL_B8PllSetSwphyDiv(UINT32 ChipID, UINT32 Divider);

#endif /* B8_CSL_PLL_H*/
