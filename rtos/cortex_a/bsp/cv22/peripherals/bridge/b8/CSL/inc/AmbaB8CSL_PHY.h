/**
 *  @file AmbaB8CSL_PHY.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Definitions & Constants for B8 PHY control APIs
 *
 */

#ifndef B8_CSL_PHY_H
#define B8_CSL_PHY_H

#include "AmbaB8Reg_PHY.h"

/*-----------------------------------------------------------------------------------------------*\
 * Macro Definitions
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
* Defined in AmbaB8_MmioBase.asm
\*-----------------------------------------------------------------------------------------------*/
extern B8_PHY_REG_s *const pAmbaB8_PhyReg;

void AmbaB8CSL_PHYPowerUpLSTx(UINT32 ChipID);
void AmbaB8CSL_PHYPowerDownLSTx(UINT32 ChipID);
void AmbaB8CSL_PHYSetLSTx(UINT32 ChipID, UINT32 Current);
void AmbaB8CSL_PHYSetCdrOutSelect(UINT32 ChipID, UINT32 CdrOutSelect);
void AmbaB8CSL_PHYSetDfe(UINT32 ChipID, UINT32 Ctrl18Reg);
void AmbaB8CSL_PHYSetCtle(UINT32 ChipID, UINT32 Ctrl21Reg);

UINT32 AmbaB8CSL_PHYGetCdrStatus(UINT32 ChipID);
UINT32 AmbaB8CSL_PHYGetRxStatus(UINT32 ChipID);

void AmbaB8CSL_PHYResetHSTxAfe(UINT32 ChipID);
void AmbaB8CSL_PHYResetHSTxDigital(UINT32 ChipID);
void AmbaB8CSL_PHYReleaseHSTxAfe(UINT32 ChipID);
void AmbaB8CSL_PHYReleaseHSTxDigital(UINT32 ChipID);
void AmbaB8CSL_PHYPowerDown(UINT32 ChipID);

#endif /* B8_CSL_PHY_H*/