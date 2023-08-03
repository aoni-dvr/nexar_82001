/**
 *  @file AmbaCSL_DMIC.h
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
 *  @details Definitions & Constants for DMIC Control APIs
 *
 */

#ifndef AMBA_CSL_DMIC_H
#define AMBA_CSL_DMIC_H

#ifndef AMBA_DMIC_DEF_H
#include "AmbaDMIC_Def.h"
#endif

#include "AmbaReg_DMIC.h"
#include "AmbaReg_ScratchpadNS.h"

/*
 * Macro Definitions
 */
static inline void AmbaCSL_DmicAudioCodecDpReset(AMBA_DMIC_REG_s *pDmicReg,UINT32 d)
{
    pDmicReg->DmicAdcDpReset.AdcDpReset = (UINT8)(d & 0x01U);
}
static inline void AmbaCSL_DmicSetDecFactor0(AMBA_DMIC_REG_s *pDmicReg,UINT32 d)
{
    pDmicReg->DmicDecFactor.DecFactor0 = (UINT8)(d & 0x3FU);
}
static inline void AmbaCSL_DmicSetDecFactor1(AMBA_DMIC_REG_s *pDmicReg,UINT32 d)
{
    pDmicReg->DmicDecFactor.DecFactor1 = (UINT8)(d & 0x07U);
}
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
static inline void AmbaCSL_DmicSetDecWnf(AMBA_DMIC_REG_s *pDmicReg,UINT32 d)
{
    pDmicReg->DmicDecFactor.DecWnf = (UINT8)(d & 0x01U);
}
#endif
static inline void AmbaCSL_DmicSetDecFs(AMBA_DMIC_REG_s *pDmicReg,UINT32 d)
{
    pDmicReg->DmicDecFactor.DecFs = (UINT16)(d & 0xFFFFU);
}
static inline void AmbaCSL_DmicClkDiv(AMBA_DMIC_REG_s *pDmicReg, UINT32 d)
{
    pDmicReg->DmicClockDivisor.FdivPdm = (UINT8)(d);
}
static inline void AmbaCSL_DmicDataPhaseLeft(AMBA_DMIC_REG_s *pDmicReg,UINT32 d)
{
    pDmicReg->DmicDataPhase.PhaseLeft = (UINT8)(d);
}
static inline void AmbaCSL_DmicDataPhaseRight(AMBA_DMIC_REG_s *pDmicReg,UINT32 d)
{
    pDmicReg->DmicDataPhase.PhaseRight = (UINT8)(d);
}
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
static inline void AmbaCSL_DmicWindNoiseFilterGammaContrl(AMBA_DMIC_REG_s *pDmicReg,UINT32 d)
{
    pDmicReg->DmicWindNoiseFilterGammaControl = (UINT32)(d);
}
#endif
static inline void AmbaCSL_DmicDroopCompFilterCof(AMBA_DMIC_REG_s *pDmicReg, UINT32 RegId,UINT32 d)
{
    pDmicReg->DroopCompFilterCoefficient[RegId] = (UINT32)(d);
}
static inline void AmbaCSL_DmicHalfBandFilterCof(AMBA_DMIC_REG_s *pDmicReg, UINT32 RegId, UINT32 d)
{
    pDmicReg->HalfBandFilterrCoefficient[RegId] = (UINT32)(d);
}
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
static inline void AmbaCSL_DmicWndNoiseHPFilterCof(AMBA_DMIC_REG_s *pDmicReg, UINT32 RegId, UINT32 d)
{
    pDmicReg->WindNoiseFilterHpfCoeffcient[RegId] = (UINT32)(d);
}
static inline void AmbaCSL_DmicWndNoiseLPFilterCof(AMBA_DMIC_REG_s *pDmicReg, UINT32 RegId, UINT32 d)
{
    pDmicReg->WindNoiseFilterLpfCoeffcient[RegId] = (UINT32)(d);
}
static inline void AmbaCSL_DmicWndNoiseDetectionFilterCof(AMBA_DMIC_REG_s *pDmicReg, UINT32 RegId, UINT32 d)
{
    pDmicReg->WindNoiseFilterWndCoeffcient[RegId] = (UINT32)(d);
}
static inline void AmbaCSL_DmicWndNoiseVocHPFilterCof(AMBA_DMIC_REG_s *pDmicReg, UINT32 RegId, UINT32 d)
{
    pDmicReg->WindNoiseFilterVocHpfCoeffcient[RegId] = (UINT32)(d);
}
static inline void AmbaCSL_DmicWndNoiseDownSampleFilterCof(AMBA_DMIC_REG_s *pDmicReg, UINT32 RegId, UINT32 d)
{
    pDmicReg->WindNoiseFilterVocLpfCoeffcient[RegId] = (UINT32)(d);
}
#endif
static inline void AmbaCSL_DmicCustomIirFilterCof(AMBA_DMIC_REG_s *pDmicReg, UINT32 RegId, UINT32 d)
{
    pDmicReg->CustomIIRCoeffcient[RegId] = (UINT32)(d);
}
static inline void AmbaCSL_DmicI2sSelect(UINT32 d)
{
    pAmbaScratchpadNS_Reg->AhbCtrl.DmicI2sSelect = (UINT8)(d & 0x01U);
}
static inline void AmbaCSL_DmicEn(AMBA_DMIC_REG_s *pDmicReg, UINT32 d)
{
    pDmicReg->DmicEnable.DmicEn = (UINT8)(d & 0x01U);
}
static inline void AmbaCSL_DmicEnSt(AMBA_DMIC_REG_s *pDmicReg, UINT32 d)
{
    pDmicReg->DmicEnable.DmicEnSt = (UINT8)(d & 0x01U);
}
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
static inline void AmbaCSL_DmicWndEn(AMBA_DMIC_REG_s *pDmicReg, UINT32 d)
{
    pDmicReg->DmicEnable.WndEn = (UINT8)(d & 0x01U);
}
static inline void AmbaCSL_DmicWnfEn(AMBA_DMIC_REG_s *pDmicReg, UINT32 d)
{
    pDmicReg->DmicEnable.WnfEn = (UINT8)(d & 0x01U);
}
#endif

/*
 * Defined in AmbaCSL_DMIC.c
 */
void AmbaCSL_DmicReset(AMBA_DMIC_REG_s *pDmicReg);
void AmbaCSL_DmicEnable(AMBA_DMIC_REG_s *pDmicReg);
void AmbaCSL_DmicInitClk(AMBA_DMIC_REG_s *pDmicReg);
void AmbaCSL_DmicInitDroopCompFltr(AMBA_DMIC_REG_s *pDmicReg);
void AmbaCSL_DmicInitHalfBandFltr(AMBA_DMIC_REG_s *pDmicReg);
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
void AmbaCSL_DmicInitWindNoiseFltr(AMBA_DMIC_REG_s *pDmicReg);
#endif

#endif /* AMBA_CSL_DMIC_H */
