/**
 *  @file AmbaB8CSL_PHY.c
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
 *  @details B8 PHY CSL Device Driver
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_IoMap.h"   //to be removed

#include "AmbaB8CSL_PHY.h"

void AmbaB8CSL_PHYPowerUpLSTx(UINT32 ChipID)
{
    B8_RCT_MPHY_CTRL26_REG_s RctMphyCtrl26Reg;

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl26Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl26Reg);
    RctMphyCtrl26Reg.PdbLstxRxch0 = 1U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl26Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl26Reg);
}

void AmbaB8CSL_PHYPowerDownLSTx(UINT32 ChipID)
{
    B8_RCT_MPHY_CTRL26_REG_s RctMphyCtrl26Reg;

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl26Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl26Reg);
    RctMphyCtrl26Reg.PdbLstxRxch0 = 0U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl26Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl26Reg);
}

void AmbaB8CSL_PHYPowerDown(UINT32 ChipID)
{
    B8_RCT_MPHY_CTRL26_REG_s RctMphyCtrl26Reg;
    B8_RCT_MPHY_CTRL11_REG_s RctMphyCtrl11Reg;
    B8_RCT_MPHY_CTRL13_REG_s RctMphyCtrl13Reg;
    B8_RCT_MPHY_CTRL16_REG_s RctMphyCtrl16Reg;
    B8_RCT_MPHY_CTRL23_REG_s RctMphyCtrl23Reg;
    B8_RCT_MPHY_CTRL25_REG_s RctMphyCtrl25Reg;
    B8_RCT_MPHY_CTRL31_REG_s RctMphyCtrl31Reg;

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl26Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl26Reg);
    RctMphyCtrl26Reg.PdbLstxRxch0 = 0U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl26Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl26Reg);

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl11Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl11Reg);
    RctMphyCtrl11Reg.DcoPdRxch0 = 1U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl11Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl11Reg);

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl13Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl13Reg);
    RctMphyCtrl13Reg.PdeyeRxch0 = 1U;
    RctMphyCtrl13Reg.PderrRxch0 = 1U;
    RctMphyCtrl13Reg.PdDesRxch0 = 1U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl13Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl13Reg);

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl16Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl16Reg);
    RctMphyCtrl16Reg.PdSlicerRxch0 = 1U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl16Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl16Reg);

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl23Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl23Reg);
    RctMphyCtrl23Reg.PdCalibRxch0 = 1U;
    RctMphyCtrl23Reg.PdCtleRxch0 = 1U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl23Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl23Reg);

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl25Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl25Reg);
    RctMphyCtrl25Reg.PdEqRxch0 = 1U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl25Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl25Reg);

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl31Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl31Reg);
    RctMphyCtrl31Reg.TxDrvPdTxch0 = 1U;
    RctMphyCtrl31Reg.PdbLsRxTxch0 = 0U;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl31Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl31Reg);



}

void AmbaB8CSL_PHYSetLSTx(UINT32 ChipID, UINT32 Current)
{
    B8_RCT_MPHY_CTRL26_REG_s RctMphyCtrl26Reg;

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl26Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl26Reg);
    RctMphyCtrl26Reg.LsRRxch0 = (UINT8)(Current & 0xfU);
    RctMphyCtrl26Reg.LsIbRxch0 = (UINT8)((Current >> 4U) & 0xfU);
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl26Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl26Reg);
}

void AmbaB8CSL_PHYSetCdrOutSelect(UINT32 ChipID, UINT32 CdrOutSelect)
{
    B8_RCT_MPHY_CTRL27_REG_s RctMphyCtrl27Reg = {0};

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl27Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl27Reg);
    RctMphyCtrl27Reg.CdrOutSelectRxch0 = (UINT8)(CdrOutSelect & 0x3fU);
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl27Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl27Reg);
}

UINT32 AmbaB8CSL_PHYGetCdrStatus(UINT32 ChipID)
{
    UINT32 MphyRctObsv2Reg = 0U;

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->MphyRctObsv2Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &MphyRctObsv2Reg);
    return MphyRctObsv2Reg;
}


UINT32 AmbaB8CSL_PHYGetRxStatus(UINT32 ChipID)
{
    B8_MPHY_RCT_OBSV5_REG_s MphyRctObsv5Reg = {0};

    (void) AmbaB8_RegRead(ChipID, &pAmbaB8_PhyReg->MphyRctObsv5Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &MphyRctObsv5Reg);
    return (MphyRctObsv5Reg.MphyRxStateRxch0);
}

void AmbaB8CSL_PHYSetCtle(UINT32 ChipID, UINT32 Ctrl21Reg)
{
    UINT32 RctMphyCtrl21Reg = Ctrl21Reg;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl21Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl21Reg);
}

void AmbaB8CSL_PHYSetDfe(UINT32 ChipID, UINT32 Ctrl18Reg)
{
    UINT32 RctMphyCtrl18Reg = Ctrl18Reg;
    (void) AmbaB8_RegWrite(ChipID, &pAmbaB8_PhyReg->RctMphyCtrl18Reg, 0U, B8_DATA_WIDTH_32BIT, 1U, &RctMphyCtrl18Reg);
}

void AmbaB8CSL_PHYResetHSTxAfe(UINT32 ChipID)
{
    UINT32 Data;
    Data = 0x09008000U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //rst tx afe
}

void AmbaB8CSL_PHYResetHSTxDigital(UINT32 ChipID)
{
    UINT32 Data;
    Data = 0x09008000U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //rst tx dig
}

void AmbaB8CSL_PHYReleaseHSTxAfe(UINT32 ChipID)
{
    UINT32 Data;
    Data = 0x09008002U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //rst tx afe
}

void AmbaB8CSL_PHYReleaseHSTxDigital(UINT32 ChipID)
{
    UINT32 Data;
    Data = 0x09008006U;
    (void) AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5284U, 0, B8_DATA_WIDTH_32BIT, 1, &Data);                                  //rst tx dig
}

