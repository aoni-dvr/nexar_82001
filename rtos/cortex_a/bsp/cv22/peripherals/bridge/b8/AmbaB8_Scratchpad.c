/**
 *  @file AmbaB8_Scratchpad.c
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
 *  @details Ambarella B8 Scratchpad APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Scratchpad.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8CSL_Scratchpad.h"
#include "AmbaB8CSL_PLL.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_ScratchpadConfig
 *
 *  @Description:: Configure B8 Scratchpad
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *     Channel:          B8 Vin channel
 *     pScratchpadConfig: Pointer to configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
void AmbaB8_ScratchpadConfig(UINT32 ChipID, const B8_SCRATCHPAD_CONFIG_s* pScratchpadConfig)
{
    AmbaB8_Misra_TouchUnused(&ChipID);
    AmbaB8_Misra_TouchUnused(&pScratchpadConfig);
}

UINT32 AmbaB8_EccConfig(UINT32 ChipID, const B8_ECC_CONFIG_s* pConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_SCRATCHPAD_ECC_CFG_REG_s EccConfig;
    UINT32 i;
    UINT32 Val;

    (void) AmbaB8_Wrap_memset(&EccConfig, 0, sizeof(B8_SCRATCHPAD_ECC_CFG_REG_s));

    for (i = 0U; i < B8_NUM_ECC_ENC_CHANNEL; i++) {
        Val = (pConfig->EncEnable[i] & (UINT32)1U) << i;
        EccConfig.EccEncEn |= (UINT8) Val;
    }
    for (i = 0U; i < B8_NUM_ECC_DEC_CHANNEL; i++) {
        Val = (pConfig->DecEnable[i] & (UINT32)1U) << i;
        EccConfig.EccDecEn |= (UINT8) Val;
    }

    if ((pConfig->NumCheckSymbol >> 3U) == 1U) {
        EccConfig.EccEnc0Sel = 3U;
        EccConfig.EccDec0Select = 3U;
    } else if ((pConfig->NumCheckSymbol >> 2U) == 1U) {
        EccConfig.EccEnc0Sel = 2U;
        EccConfig.EccDec0Select = 2U;
    } else {
        EccConfig.EccEnc0Sel = 0U;
        EccConfig.EccDec0Select = 0U;
    }

    RetVal |= AmbaB8_RegWrite(ChipID, & pAmbaB8_ScratchpadReg->EccConfig, 0U, B8_DATA_WIDTH_32BIT, 1U, &EccConfig);

    return RetVal;
}

UINT32 AmbaB8_EccEncReset(UINT32 ChipID, UINT32 Channel)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 ResetValue = 0xfffff;
    B8_AHB_SW_RST_0_REG_s AhbSwRst0;

    (void) AmbaB8_Wrap_memcpy(&AhbSwRst0, &ResetValue, sizeof(B8_AHB_SW_RST_0_REG_s));

    if (Channel <= B8_ECC_ENC_CHANNEL5) {
        AhbSwRst0.RstLEccEnc0_5 &= (UINT8)(~((UINT32)0x1U << Channel));
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);
        AhbSwRst0.RstLEccEnc0_5 |= (UINT8)((UINT32)0x1U << Channel);
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);

    } else if ((Channel >= B8_ECC_ENC_CHANNEL6) && (Channel <= B8_ECC_ENC_CHANNEL7)) {
        AhbSwRst0.RstLEccEnc6_7 &= (UINT8)(~((UINT32)0x1U << (Channel - B8_ECC_ENC_CHANNEL6)));
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);
        AhbSwRst0.RstLEccEnc6_7 |= (UINT8)((UINT32)0x1U << (Channel - B8_ECC_ENC_CHANNEL6));
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

UINT32 AmbaB8_EccDecReset(UINT32 ChipID, UINT32 Channel)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 ResetValue = 0xfffff;
    B8_AHB_SW_RST_0_REG_s AhbSwRst0;

    (void) AmbaB8_Wrap_memcpy(&AhbSwRst0, &ResetValue, sizeof(B8_AHB_SW_RST_0_REG_s));

    if (Channel <= B8_ECC_DEC_CHANNEL3) {
        AhbSwRst0.RstLEccDec0_3 &= (UINT8)(~((UINT32)0x1 << Channel));
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);
        AhbSwRst0.RstLEccDec0_3 |= (UINT8)((UINT32)0x1 << Channel);
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);

    } else if ((Channel >= B8_ECC_DEC_CHANNEL4) && (Channel <= B8_ECC_DEC_CHANNEL5)) {
        AhbSwRst0.RstLEccDec4_5 &= (UINT8)(~((UINT32)0x1 << (Channel - B8_ECC_DEC_CHANNEL4)));
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);
        AhbSwRst0.RstLEccDec4_5 |= (UINT8)((UINT32)0x1 << (Channel - B8_ECC_DEC_CHANNEL4));
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->AhbSwReset0, 0U, B8_DATA_WIDTH_32BIT, 1U, &AhbSwRst0);

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}

UINT32 AmbaB8_VoutInputSrcEnable(UINT32 ChipID, UINT32 Channel)
{
    UINT32 RetVal;
    B8_SCRATCHPAD_VOUT_SRC_REG_s VoutSrcReg;

    RetVal = AmbaB8_RegRead(ChipID, & (pAmbaB8_ScratchpadReg->VoutSrc), 0, B8_DATA_WIDTH_32BIT, 1, &VoutSrcReg);

    if (RetVal == B8_ERR_NONE) {
        if (Channel == B8_VOUT_CHANNEL1) {
            VoutSrcReg.Vout1SrcEn = 1U;
        } else {
            VoutSrcReg.Vout0SrcEn = 1U;
        }
        RetVal = AmbaB8_RegWrite(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK, & (pAmbaB8_ScratchpadReg->VoutSrc), 0, B8_DATA_WIDTH_32BIT, 1, &VoutSrcReg);
    }

    return RetVal;
}

