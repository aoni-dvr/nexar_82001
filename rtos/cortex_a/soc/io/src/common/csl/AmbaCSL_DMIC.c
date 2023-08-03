/**
 *  @file AmbaCSL_DMIC.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details DMIC CSL Device Driver
 *
 */

#include "AmbaCSL_DMIC.h"

/**
 *  AmbaRTSL_DmicReset - Reset dmic datapath
 *  @param[in] pDmicReg pointer to dmic registers
 */
void AmbaCSL_DmicReset(AMBA_DMIC_REG_s *pDmicReg)
{
    AmbaCSL_DmicAudioCodecDpReset(pDmicReg, 0x0);
    AmbaCSL_DmicAudioCodecDpReset(pDmicReg, 0x1);
    AmbaCSL_DmicAudioCodecDpReset(pDmicReg, 0x0);
}

/**
 *  AmbaCSL_DmicEnable - Enable dmic
 *  @param[in] pDmicReg pointer to dmic registers
 */
void AmbaCSL_DmicEnable(AMBA_DMIC_REG_s *pDmicReg)
{
    AmbaCSL_DmicEn(pDmicReg, 0x1);
    AmbaCSL_DmicEnSt(pDmicReg, 0x0);
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
    AmbaCSL_DmicWndEn(pDmicReg, 0x0);
    AmbaCSL_DmicWnfEn(pDmicReg, 0x0);
#endif
}

/**
 *  AmbaCSL_DmicInitClk - Initialize clock divider for each dmic filter
 *  @param[in] pDmicReg pointer to dmic registers
 */
void AmbaCSL_DmicInitClk(AMBA_DMIC_REG_s *pDmicReg)
{
    AmbaCSL_DmicSetDecFactor0(pDmicReg, 0x1F);
    AmbaCSL_DmicSetDecFactor1(pDmicReg, 0x1);
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
    AmbaCSL_DmicSetDecWnf(pDmicReg, 0x0);
#endif
    AmbaCSL_DmicSetDecFs(pDmicReg, 0x3FF);
    AmbaCSL_DmicClkDiv(pDmicReg, 0x10);
    AmbaCSL_DmicDataPhaseLeft(pDmicReg, 0xC);
    AmbaCSL_DmicDataPhaseRight(pDmicReg, 0x4);
}

/**
 *  AmbaCSL_DmicInitDroopCompFltr - Initialize CIC droop compensation filter
 *  @param[in] pDmicReg pointer to dmic registers
 */
void AmbaCSL_DmicInitDroopCompFltr(AMBA_DMIC_REG_s *pDmicReg)
{
    static const UINT32 DroopCompCoeff[8] = {
        0xFF85C000U, 0xFF0C9000U, 0x01CAB000U, 0x04A9E000U,
        0xF9A26000U, 0xEF8AE000U, 0x0F0B6000U, 0x4381A000U
    };
    UINT32 i;

    for (i = 0; i < 8U; i++) {
        AmbaCSL_DmicDroopCompFilterCof(pDmicReg, i, DroopCompCoeff[i]);
    }
}

/**
 *  AmbaCSL_DmicInitHalfBandFltr - Initialize Half-band filter
 *  @param[in] pDmicReg pointer to dmic registers
 */
void AmbaCSL_DmicInitHalfBandFltr(AMBA_DMIC_REG_s *pDmicReg)
{
    static const UINT32 HalfBandCoeff[32] = {
        0xFFC87000U, 0xFFFD2000U, 0x00601000U, 0x00056000U,
        0xFF23F000U, 0xFFF61000U, 0x01CCF000U, 0x000FC000U,
        0xFC93E000U, 0xFFEA3000U, 0x06462000U, 0x001B2000U,
        0xF39CC000U, 0xFFE14000U, 0x28558000U, 0x40200000U,
        0x28558000U, 0xFFE14000U, 0xF39CC000U, 0x001B2000U,
        0x06462000U, 0xFFEA3000U, 0xFC93E000U, 0x000FC000U,
        0x01CCF000U, 0xFFF61000U, 0xFF23F000U, 0x00056000U,
        0x00601000U, 0xFFFD2000U, 0xFFC87000U, 0x00000000U
    };
    UINT32 i;

    for (i = 0; i < 32U; i++) {
        AmbaCSL_DmicHalfBandFilterCof(pDmicReg, i, HalfBandCoeff[i]);
    }
}

#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
/**
 *  AmbaCSL_DmicInitWindNoiseFltr - Initialize wind noise filter
 *  @param[in] pDmicReg pointer to dmic registers
 */
void AmbaCSL_DmicInitWindNoiseFltr(AMBA_DMIC_REG_s *pDmicReg)
{
    static const UINT32 WindNoiseReduceHpCoeff[68] = {
        0x3BF2E000U, 0x85187000U, 0x3B062000U, 0x80006000U, 0x40000000U, 0x828AC000U, 0x3D8C7000U, 0x8002B000U, 0x40000000U, 0x80BF7000U, 0x3F53F000U, 0x8004C000U, 0x40000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
        0x38E59000U, 0x890E5000U, 0x3751B000U, 0x80013000U, 0x40000000U, 0x84A21000U, 0x3BA7E000U, 0x80089000U, 0x40000000U, 0x8172F000U, 0x3ECBC000U, 0x800F7000U, 0x40000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
        0x328CE000U, 0x900EB000U, 0x31196000U, 0x80047000U, 0x40000000U, 0x89000000U, 0x37E4F000U, 0x80214000U, 0x40000000U, 0x84363000U, 0x3C847000U, 0x80437000U, 0x40001000U, 0x81A16000U, 0x3F09D000U, 0x8057D000U, 0x40000000U,
        0x2F01E000U, 0x94AC2000U, 0x2D3BD000U, 0x80079000U, 0x40000000U, 0x8BE0D000U, 0x359EE000U, 0x8038E000U, 0x40000000U, 0x85BF3000U, 0x3B7D4000U, 0x80737000U, 0x40000000U, 0x8264E000U, 0x3EBF3000U, 0x80964000U, 0x40000000U
    };
    static const UINT32 WindNoiseReduceLpCoeff[68] = {
        0x00051000U, 0x83247000U, 0x3CE74000U, 0x83F84000U, 0x40000000U, 0x8216E000U, 0x3DF8F000U, 0x808F3000U, 0x40000000U, 0x80BF1000U, 0x3F544000U, 0x804FA000U, 0x40000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
        0x00052000U, 0x873C9000U, 0x3900E000U, 0x943AB000U, 0x40000000U, 0x84F56000U, 0x3B5EE000U, 0x83045000U, 0x40000000U, 0x81F61000U, 0x3E728000U, 0x81AF6000U, 0x40000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
        0x00051000U, 0x89175000U, 0x37486000U, 0x9B6A1000U, 0x40000000U, 0x86C71000U, 0x39BEF000U, 0x8401C000U, 0x40000000U, 0x840DF000U, 0x3CA0E000U, 0x81FD2000U, 0x40000000U, 0x81CEB000U, 0x3EF76000U, 0x8187C000U, 0x40000000U,
        0x00055000U, 0x8C457000U, 0x34684000U, 0xAEBBB000U, 0x40000000U, 0x89539000U, 0x37A13000U, 0x875C9000U, 0x40000000U, 0x85CDA000U, 0x3B740000U, 0x83ADC000U, 0x40000000U, 0x82D5E000U, 0x3E98E000U, 0x82D5B000U, 0x40000000U
    };
    static const UINT32 WindNoiseDetectCoeff[17] = {
        0x000FD000U, 0x827D6000U, 0x3D894000U, 0x81EE7000U,
        0x40000000U, 0x82609000U, 0x3DAD1000U, 0x8043A000U,
        0x40000000U, 0x810CC000U, 0x3F05C000U, 0x80226000U,
        0x40000000U, 0x801DE000U, 0x3FF1A000U, 0x801AF000U,
        0x40000000U
    };

    static const UINT32 SpeechSynthHpCoeff[17] = {
        0x3CA45000U, 0x83F9C000U, 0x3C18C000U, 0x80000000U,
        0x3FFE1000U, 0x82308000U, 0x3DDF6000U, 0x80000000U,
        0x411EB000U, 0x808D6000U, 0x3F806000U, 0x81200000U,
        0x3EE63000U, 0x80396000U, 0x3FCEC000U, 0x8007B000U,
        0x4001F000U
    };
    static const UINT32 SpeechSynthLpCoeff[11] = {
        0x00000000U, 0x00600000U, 0x00D00000U, 0x007C0000U,
        0xFE680000U, 0xFB980000U, 0xFBEC0000U, 0x036C0000U,
        0x11B00000U, 0x20340000U, 0x26600000U
    };
    UINT32 i;

    /* Wind noise reduction */
    for (i = 0; i < 68U; i++) {
        AmbaCSL_DmicWndNoiseHPFilterCof(pDmicReg, i, WindNoiseReduceHpCoeff[i]);
        AmbaCSL_DmicWndNoiseLPFilterCof(pDmicReg, i, WindNoiseReduceLpCoeff[i]);
    }

    for (i = 0; i < 17U; i++) {
        AmbaCSL_DmicWndNoiseDetectionFilterCof(pDmicReg, i, WindNoiseDetectCoeff[i]);
    }

    /* Speech synthesis */
    for (i = 0; i < 17U; i++) {
        AmbaCSL_DmicWndNoiseVocHPFilterCof(pDmicReg, i, SpeechSynthHpCoeff[i]);
    }
    for (i = 0; i < 11U; i++) {
        AmbaCSL_DmicWndNoiseDownSampleFilterCof(pDmicReg, i, SpeechSynthLpCoeff[i]);
    }
}
#endif
