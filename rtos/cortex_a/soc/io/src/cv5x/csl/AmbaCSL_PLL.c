/**
 *  @file AmbaCSL_PLL.c
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
 *  @details Chip Support Library (CSL) for PLL
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#include "AmbaCSL_PLL.h"

static UINT32 AmbaCpuCyclePerUs = 0U;
static UINT32 AmbaCpuCyclePerMs = 0U;

/**
 *  PLL_GetCtrlRegAddr - Get PLL control register address
 *  @param[in] PllIdx PLL ID
 *  @return NULL or a pointer to a register address
 */
static UINT32 * PLL_GetCtrlRegAddr(UINT32 PllIdx)
{
    volatile const AMBA_PLL_CTRL_REG_s *pPllCtrlReg = NULL;
    UINT32 *pRegAddr = NULL;

    if (PllIdx == AMBA_PLL_AUDIO) {
        pPllCtrlReg = &pAmbaRCT_Reg->AudioPllCtrl;
    } else if (PllIdx == AMBA_PLL_CORE) {
        pPllCtrlReg = &pAmbaRCT_Reg->CorePllCtrl;
    } else if (PllIdx == AMBA_PLL_CORTEX) {
        pPllCtrlReg = &pAmbaRCT_Reg->CortexPllCtrl;
    } else if (PllIdx == AMBA_PLL_DSU) {
        pPllCtrlReg = &pAmbaRCT_Reg->DsuPllCtrl;
    } else if (PllIdx == AMBA_PLL_ENET) {
        pPllCtrlReg = &pAmbaRCT_Reg->EnetPllCtrl;
    } else if (PllIdx == AMBA_PLL_HDMI) {
        pPllCtrlReg = &pAmbaRCT_Reg->HdmiPllCtrl;
    } else if (PllIdx == AMBA_PLL_HDMILC) {
        pPllCtrlReg = &pAmbaRCT_Reg->HdmiLcPllCtrl;
    } else if (PllIdx == AMBA_PLL_IDSP) {
        pPllCtrlReg = &pAmbaRCT_Reg->IdspPllCtrl;
    } else if (PllIdx == AMBA_PLL_IDSPV) {
        pPllCtrlReg = &pAmbaRCT_Reg->IdspvPllCtrl;
    } else if (PllIdx == AMBA_PLL_SD) {
        pPllCtrlReg = &pAmbaRCT_Reg->SdPllCtrl;
    } else if (PllIdx == AMBA_PLL_SENSOR0) {
        pPllCtrlReg = &pAmbaRCT_Reg->SensorPllCtrl;
    } else if (PllIdx == AMBA_PLL_SENSOR2) {
        pPllCtrlReg = &pAmbaRCT_Reg->Sensor2PllCtrl;
    } else if (PllIdx == AMBA_PLL_SLVSEC) {
        pPllCtrlReg = &pAmbaRCT_Reg->SlvsEcPllCtrl;
    } else if (PllIdx == AMBA_PLL_VIDEOA) {
        pPllCtrlReg = &pAmbaRCT_Reg->VideoAPllCtrl;
    } else if (PllIdx == AMBA_PLL_VISION) {
        pPllCtrlReg = &pAmbaRCT_Reg->VisionPllCtrl;
    } else if (PllIdx == AMBA_PLL_NAND) {
        pPllCtrlReg = &pAmbaRCT_Reg->NandPllCtrl;
    } else {
        pPllCtrlReg = NULL;
    }

    AmbaMisra_TypeCast(&pRegAddr, &pPllCtrlReg);

    return pRegAddr;
}

/**
 *  PLL_GetCtrl2RegAddr - Get PLL control 2 register address
 *  @param[in] PllIdx PLL ID
 *  @return NULL or a pointer to a register address
 */
static UINT32 * PLL_GetCtrl2RegAddr(UINT32 PllIdx)
{
    volatile const AMBA_PLL_CTRL2_REG_s *pPllCtrlReg = NULL;
    UINT32 *pRegAddr = NULL;

    if (PllIdx == AMBA_PLL_AUDIO) {
        pPllCtrlReg = &pAmbaRCT_Reg->AudioPllCtrl2;
    } else if (PllIdx == AMBA_PLL_CORE) {
        pPllCtrlReg = &pAmbaRCT_Reg->CorePllCtrl2;
    } else if (PllIdx == AMBA_PLL_CORTEX) {
        pPllCtrlReg = &pAmbaRCT_Reg->CortexPllCtrl2;
    } else if (PllIdx == AMBA_PLL_DSU) {
        pPllCtrlReg = &pAmbaRCT_Reg->DsuPllCtrl2;
    } else if (PllIdx == AMBA_PLL_ENET) {
        pPllCtrlReg = &pAmbaRCT_Reg->EnetPllCtrl2;
    } else if (PllIdx == AMBA_PLL_HDMI) {
        pPllCtrlReg = &pAmbaRCT_Reg->HdmiPllCtrl2;
    } else if (PllIdx == AMBA_PLL_HDMILC) {
        pPllCtrlReg = &pAmbaRCT_Reg->HdmiLcPllCtrl2;
    } else if (PllIdx == AMBA_PLL_IDSP) {
        pPllCtrlReg = &pAmbaRCT_Reg->IdspPllCtrl2;
    } else if (PllIdx == AMBA_PLL_IDSPV) {
        pPllCtrlReg = &pAmbaRCT_Reg->IdspvPllCtrl2;
    } else if (PllIdx == AMBA_PLL_SD) {
        pPllCtrlReg = &pAmbaRCT_Reg->SdPllCtrl2;
    } else if (PllIdx == AMBA_PLL_SENSOR0) {
        pPllCtrlReg = &pAmbaRCT_Reg->SensorPllCtrl2;
    } else if (PllIdx == AMBA_PLL_SENSOR2) {
        pPllCtrlReg = &pAmbaRCT_Reg->Sensor2PllCtrl2;
    } else if (PllIdx == AMBA_PLL_SLVSEC) {
        pPllCtrlReg = &pAmbaRCT_Reg->SlvsEcPllCtrl2;
    } else if (PllIdx == AMBA_PLL_VIDEOA) {
        pPllCtrlReg = &pAmbaRCT_Reg->VideoAPllCtrl2;
    } else if (PllIdx == AMBA_PLL_VISION) {
        pPllCtrlReg = &pAmbaRCT_Reg->VisionPllCtrl2;
    } else if (PllIdx == AMBA_PLL_NAND) {
        pPllCtrlReg = &pAmbaRCT_Reg->NandPllCtrl2;
    } else {
        pPllCtrlReg = NULL;
    }

    AmbaMisra_TypeCast(&pRegAddr, &pPllCtrlReg);

    return pRegAddr;
}

/**
 *  PLL_GetCtrl3RegAddr - Get PLL control 3 register address
 *  @param[in] PllIdx PLL ID
 *  @return NULL or a pointer to a register address
 */
static UINT32 * PLL_GetCtrl3RegAddr(UINT32 PllIdx)
{
    volatile const AMBA_PLL_CTRL3_REG_s *pPllCtrlReg = NULL;
    UINT32 *pRegAddr = NULL;

    if (PllIdx == AMBA_PLL_AUDIO) {
        pPllCtrlReg = &pAmbaRCT_Reg->AudioPllCtrl3;
    } else if (PllIdx == AMBA_PLL_CORE) {
        pPllCtrlReg = &pAmbaRCT_Reg->CorePllCtrl3;
    } else if (PllIdx == AMBA_PLL_CORTEX) {
        pPllCtrlReg = &pAmbaRCT_Reg->CortexPllCtrl3;
    } else if (PllIdx == AMBA_PLL_DSU) {
        pPllCtrlReg = &pAmbaRCT_Reg->DsuPllCtrl3;
    } else if (PllIdx == AMBA_PLL_ENET) {
        pPllCtrlReg = &pAmbaRCT_Reg->EnetPllCtrl3;
    } else if (PllIdx == AMBA_PLL_HDMI) {
        pPllCtrlReg = &pAmbaRCT_Reg->HdmiPllCtrl3;
    } else if (PllIdx == AMBA_PLL_HDMILC) {
        pPllCtrlReg = &pAmbaRCT_Reg->HdmiLcPllCtrl3;
    } else if (PllIdx == AMBA_PLL_IDSP) {
        pPllCtrlReg = &pAmbaRCT_Reg->IdspPllCtrl3;
    } else if (PllIdx == AMBA_PLL_IDSPV) {
        pPllCtrlReg = &pAmbaRCT_Reg->IdspvPllCtrl3;
    } else if (PllIdx == AMBA_PLL_SD) {
        pPllCtrlReg = &pAmbaRCT_Reg->SdPllCtrl3;
    } else if (PllIdx == AMBA_PLL_SENSOR0) {
        pPllCtrlReg = &pAmbaRCT_Reg->SensorPllCtrl3;
    } else if (PllIdx == AMBA_PLL_SENSOR2) {
        pPllCtrlReg = &pAmbaRCT_Reg->Sensor2PllCtrl3;
    } else if (PllIdx == AMBA_PLL_SLVSEC) {
        pPllCtrlReg = &pAmbaRCT_Reg->SlvsEcPllCtrl3;
    } else if (PllIdx == AMBA_PLL_VIDEOA) {
        pPllCtrlReg = &pAmbaRCT_Reg->VideoAPllCtrl3;
    } else if (PllIdx == AMBA_PLL_VISION) {
        pPllCtrlReg = &pAmbaRCT_Reg->VisionPllCtrl3;
    } else if (PllIdx == AMBA_PLL_NAND) {
        pPllCtrlReg = &pAmbaRCT_Reg->NandPllCtrl3;
    } else {
        pPllCtrlReg = NULL;
    }

    AmbaMisra_TypeCast(&pRegAddr, &pPllCtrlReg);

    return pRegAddr;
}

/**
 *  PLL_GetFracRegAddr - Get PLL fraction register address
 *  @param[in] PllIdx PLL ID
 *  @return NULL or a pointer to a register address
 */
static UINT32 * PLL_GetFracRegAddr(UINT32 PllIdx)
{
    volatile const UINT32 *pPllFracReg = NULL;
    UINT32 *pRegAddr = NULL;

    if (PllIdx == AMBA_PLL_AUDIO) {
        pPllFracReg = &pAmbaRCT_Reg->AudioPllFraction;
    } else if (PllIdx == AMBA_PLL_CORE) {
        pPllFracReg = &pAmbaRCT_Reg->CorePllFraction;
    } else if (PllIdx == AMBA_PLL_CORTEX) {
        pPllFracReg = &pAmbaRCT_Reg->CortexPllFraction;
    } else if (PllIdx == AMBA_PLL_DSU) {
        pPllFracReg = &pAmbaRCT_Reg->DsuPllFraction;
    } else if (PllIdx == AMBA_PLL_ENET) {
        pPllFracReg = &pAmbaRCT_Reg->EnetPllFraction;
    } else if (PllIdx == AMBA_PLL_HDMI) {
        pPllFracReg = &pAmbaRCT_Reg->HdmiPllFraction;
    } else if (PllIdx == AMBA_PLL_HDMILC) {
        pPllFracReg = &pAmbaRCT_Reg->HdmiLcPllFraction;
    } else if (PllIdx == AMBA_PLL_IDSP) {
        pPllFracReg = &pAmbaRCT_Reg->IdspPllFraction;
    } else if (PllIdx == AMBA_PLL_IDSPV) {
        pPllFracReg = &pAmbaRCT_Reg->IdspvPllFraction;
    } else if (PllIdx == AMBA_PLL_SD) {
        pPllFracReg = &pAmbaRCT_Reg->SdPllFraction;
    } else if (PllIdx == AMBA_PLL_SENSOR0) {
        pPllFracReg = &pAmbaRCT_Reg->SensorPllFraction;
    } else if (PllIdx == AMBA_PLL_SENSOR2) {
        pPllFracReg = &pAmbaRCT_Reg->Sensor2PllFraction;
    } else if (PllIdx == AMBA_PLL_SLVSEC) {
        pPllFracReg = &pAmbaRCT_Reg->SlvsEcPllFraction;
    } else if (PllIdx == AMBA_PLL_VIDEOA) {
        pPllFracReg = &pAmbaRCT_Reg->VideoAPllFraction;
    } else if (PllIdx == AMBA_PLL_VISION) {
        pPllFracReg = &pAmbaRCT_Reg->VisionPllFraction;
    } else if (PllIdx == AMBA_PLL_NAND) {
        pPllFracReg = &pAmbaRCT_Reg->NandPllFraction;
    } else {
        pPllFracReg = NULL;
    }

    AmbaMisra_TypeCast(&pRegAddr, &pPllFracReg);

    return pRegAddr;
}

/**
 *  AmbaCSL_PllGetPreScaleRegVal - Get PLL prescaler register value
 *  @param[in] PllIdx PLL ID
 *  @return Divider value or 0xffffffffU (if no divider)
 */
UINT32 AmbaCSL_PllGetPreScaleRegVal(UINT32 PllIdx)
{
    UINT32 DivVal = 0xffffffffU;

    if (PllIdx == AMBA_PLL_AUDIO) {
        DivVal = pAmbaRCT_Reg->AudioPllPrescaler.Divider + (UINT32)1U;
    } else if (PllIdx == AMBA_PLL_SENSOR0) {
        DivVal = pAmbaRCT_Reg->SensorPllPrescaler.Divider + (UINT32)1U;
    } else if (PllIdx == AMBA_PLL_VIDEOA) {
        DivVal = pAmbaRCT_Reg->VideoAPllPrescaler.Divider + (UINT32)1U;
    } else {
        DivVal = 0xffffffffU;   /* as a hint for no divider */
    }

    return DivVal;
}

/**
 *  AmbaCSL_PllSetPreScaleRegVal - Set PLL prescaler register value
 *  @param[in] PllIdx PLL ID
 *  @param[in] DivVal Divider value
 */
void AmbaCSL_PllSetPreScaleRegVal(UINT32 PllIdx, UINT32 DivVal)
{
    UINT32 Cycle2Ms = AmbaCSL_PllGetNumCpuCycleMs() << 1U;

    if ((DivVal == 0U) || (DivVal == 0xffffffffU)) {
        /* Shall not happen!! */
    } else if (PllIdx == AMBA_PLL_AUDIO) {
        pAmbaRCT_Reg->ScalerReset.AudioPreScaler = 1;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->ScalerReset.AudioPreScaler = 0;

        pAmbaRCT_Reg->AudioPllPrescaler.WriteEnable = 0U;
        pAmbaRCT_Reg->AudioPllPrescaler.Divider = (UINT8)(DivVal - 1U);
        pAmbaRCT_Reg->AudioPllPrescaler.WriteEnable = 1U;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->AudioPllPrescaler.WriteEnable = 0U;
    } else if (PllIdx == AMBA_PLL_SENSOR0) {
        pAmbaRCT_Reg->ScalerReset.SensorPreScaler = 1;
        pAmbaRCT_Reg->ScalerReset.SensorPreScaler = 0;

        pAmbaRCT_Reg->SensorPllPrescaler.WriteEnable = 0U;
        pAmbaRCT_Reg->SensorPllPrescaler.Divider = (UINT8)(DivVal - 1U);
        pAmbaRCT_Reg->SensorPllPrescaler.WriteEnable = 1U;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->SensorPllPrescaler.WriteEnable = 0U;
    } else if (PllIdx == AMBA_PLL_VIDEOA) {
        pAmbaRCT_Reg->ScalerReset.VideoAPreScaler = 1;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->ScalerReset.VideoAPreScaler = 0;

        pAmbaRCT_Reg->VideoAPllPrescaler.WriteEnable = 0U;
        pAmbaRCT_Reg->VideoAPllPrescaler.Divider = (UINT8)(DivVal - 1U);
        pAmbaRCT_Reg->VideoAPllPrescaler.WriteEnable = 1U;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->VideoAPllPrescaler.WriteEnable = 0U;
    } else {
        /* No divider. Do nothing. */
    }
}

/**
 *  AmbaCSL_PllGetPostScaleRegVal - Get PLL postscaler register value
 *  @param[in] PllIdx PLL ID
 *  @return Divider value or 0xffffffffU (if no divider)
 */
UINT32 AmbaCSL_PllGetPostScaleRegVal(UINT32 PllIdx)
{
    UINT32 DivVal = 0xffffffffU;

    if (PllIdx == AMBA_PLL_AUDIO) {
        DivVal = pAmbaRCT_Reg->AudioPllPostscaler.Divider + (UINT32)1U;
    } else if (PllIdx == AMBA_PLL_ENET) {
        DivVal = pAmbaRCT_Reg->EnetPllPostscaler.Divider + (UINT32)1U;
    } else if (PllIdx == AMBA_PLL_SENSOR0) {
        DivVal = pAmbaRCT_Reg->SensorPllPostscaler.Divider + (UINT32)1U;
    } else if (PllIdx == AMBA_PLL_SENSOR2) {
        DivVal = pAmbaRCT_Reg->Sensor2Postscaler.Divider + (UINT32)1U;
    } else if (PllIdx == AMBA_PLL_VIDEOA) {
        DivVal = pAmbaRCT_Reg->VideoAPllPostscaler.Divider;
    } else if (PllIdx == AMBA_PLL_HDMI) {
        DivVal = pAmbaRCT_Reg->VoutBClkDivider.Divider + (UINT32)1U;
    } else {
        DivVal = 0xffffffffU;   /* as a hint for no divider */
    }

    return DivVal;
}

/**
 *  AmbaCSL_PllSetPostScaleRegVal - Set PLL prescaler register value
 *  @param[in] PllIdx PLL ID
 *  @param[in] DivVal Divider value
 */
void AmbaCSL_PllSetPostScaleRegVal(UINT32 PllIdx, UINT32 DivVal)
{
    UINT32 Cycle2Ms = AmbaCSL_PllGetNumCpuCycleMs() << 1U;

    if ((DivVal == 0U) || (DivVal == 0xffffffffU)) {
        /* Shall not happen!! */
    } else if (PllIdx == AMBA_PLL_AUDIO) {
        pAmbaRCT_Reg->AudioPllPostscaler.WriteEnable = 0U;
        pAmbaRCT_Reg->AudioPllPostscaler.Divider = (UINT8)(DivVal - 1U);
        pAmbaRCT_Reg->AudioPllPostscaler.WriteEnable = 1U;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->AudioPllPostscaler.WriteEnable = 0U;
    } else if (PllIdx == AMBA_PLL_ENET) {
        pAmbaRCT_Reg->EnetPllPostscaler.WriteEnable = 0U;
        pAmbaRCT_Reg->EnetPllPostscaler.Divider = (UINT8)(DivVal - 1U);
        pAmbaRCT_Reg->EnetPllPostscaler.WriteEnable = 1U;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->EnetPllPostscaler.WriteEnable = 0U;
    } else if (PllIdx == AMBA_PLL_SENSOR0) {
        pAmbaRCT_Reg->SensorPllPostscaler.WriteEnable = 0U;
        pAmbaRCT_Reg->SensorPllPostscaler.Divider = (UINT8)(DivVal - 1U);
        pAmbaRCT_Reg->SensorPllPostscaler.WriteEnable = 1U;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->SensorPllPostscaler.WriteEnable = 0U;
    }  else if (PllIdx == AMBA_PLL_SENSOR2) {
        pAmbaRCT_Reg->Sensor2Postscaler.WriteEnable = 0U;
        pAmbaRCT_Reg->Sensor2Postscaler.Divider = (UINT8)(DivVal - 1U);
        pAmbaRCT_Reg->Sensor2Postscaler.WriteEnable = 1U;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->Sensor2Postscaler.WriteEnable = 0U;
    } else if (PllIdx == AMBA_PLL_VIDEOA) {
        pAmbaRCT_Reg->VideoAPllPostscaler.Divider = (UINT16)DivVal;
    }  else if (PllIdx == AMBA_PLL_HDMI) {
        pAmbaRCT_Reg->VoutBClkDivider.WriteEnable = 0U;
        pAmbaRCT_Reg->VoutBClkDivider.Divider = (UINT8)(DivVal - 1U);
        pAmbaRCT_Reg->VoutBClkDivider.WriteEnable = 1U;
        AmbaDelayCycles(Cycle2Ms);
        pAmbaRCT_Reg->VoutBClkDivider.WriteEnable = 0U;
    } else {
        /* No divider. Do nothing. */
    }
}

/**
 *  AmbaCSL_PllGetCtrlRegsVal - Get PLL Control register values
 *  @param[in] PllIdx PLL ID
 *  @param[out] pPllCtrlRegsVal A pointer to store PLL control register values
 */
void AmbaCSL_PllGetCtrlRegsVal(UINT32 PllIdx, AMBA_PLL_CTRL_REGS_VAL_s *pPllCtrlRegsVal)
{
    volatile const UINT32 *pPllCtrlReg = PLL_GetCtrlRegAddr(PllIdx);
    volatile const UINT32 *pPllCtrl2Reg = PLL_GetCtrl2RegAddr(PllIdx);
    volatile const UINT32 *pPllCtrl3Reg = PLL_GetCtrl3RegAddr(PllIdx);
    volatile const UINT32 *pPllFracReg = PLL_GetFracRegAddr(PllIdx);

    pPllCtrlRegsVal->Ctrl  = *pPllCtrlReg;  /* Control Register */
    pPllCtrlRegsVal->Ctrl2 = *pPllCtrl2Reg; /* Control Register-2 */
    pPllCtrlRegsVal->Ctrl3 = *pPllCtrl3Reg; /* Control Register-3 */

    /* Fraction */
    if (pPllFracReg != NULL) {
        pPllCtrlRegsVal->Fraction = *pPllFracReg;
    } else {
        pPllCtrlRegsVal->Fraction = 0U;     /* no fraction */
    }
}

/**
 *  AmbaCSL_PllSetCtrlRegsVal - Set PLL Control register values
 *  @param[in] PllIdx PLL ID
 *  @param[in] pNewRegsVal A pointer to load PLL control register values
 */
void AmbaCSL_PllSetCtrlRegsVal(UINT32 PllIdx, AMBA_PLL_CTRL_REGS_VAL_s *pNewRegsVal)
{
    volatile UINT32 *pPllCtrlReg = PLL_GetCtrlRegAddr(PllIdx);
    volatile UINT32 *pPllCtrl2Reg = PLL_GetCtrl2RegAddr(PllIdx);
    volatile UINT32 *pPllCtrl3Reg = PLL_GetCtrl3RegAddr(PllIdx);
    volatile UINT32 *pPllFracReg = PLL_GetFracRegAddr(PllIdx);
    UINT32 Cycle1Us = AmbaCSL_PllGetNumCpuCycleUs();

    pNewRegsVal->Ctrl  &= ~(UINT32)0x51U;
    pNewRegsVal->Ctrl3 &= ~(UINT32)0x18U;

    *pPllCtrl2Reg = pNewRegsVal->Ctrl2;
    *pPllCtrl3Reg = pNewRegsVal->Ctrl3 | 0x8U;

    /* write new value to fractional register (if required) */
    if (pPllFracReg != NULL) {
        *pPllFracReg = pNewRegsVal->Fraction;
        if (pNewRegsVal->Fraction != 0U) {
            pNewRegsVal->Ctrl |= 0x8U;
        } else {
            pNewRegsVal->Ctrl &= ~0x8U;
        }
    }

    /* no matter fractional mode changes or not, always do vco_halt and pll_reset */
    *pPllCtrlReg = pNewRegsVal->Ctrl | 0x50U;
    AmbaDelayCycles(Cycle1Us);
    *pPllCtrlReg = pNewRegsVal->Ctrl | 0x10U;
    AmbaDelayCycles(Cycle1Us);
    *pPllCtrlReg = pNewRegsVal->Ctrl;
    AmbaDelayCycles(Cycle1Us << 4U);
    *pPllCtrlReg = pNewRegsVal->Ctrl | 0x1U;
    AmbaDelayCycles(Cycle1Us << 4U);
    *pPllCtrlReg = pNewRegsVal->Ctrl;
    *pPllCtrl3Reg = pNewRegsVal->Ctrl3;
}

/**
 *  AmbaCSL_PllSetPowerDown - Power down the PLL (save power)
 *  @param[in] PllIdx PLL ID
 */
void AmbaCSL_PllSetPowerDown(UINT32 PllIdx)
{
    volatile UINT32 *pPllCtrlReg = PLL_GetCtrlRegAddr(PllIdx);

    if (pPllCtrlReg != NULL) {
        *pPllCtrlReg |= (UINT32)0x20U;
    }
}

/**
 *  AmbaCSL_PllSetBypass - Force PLL bypass
 *  @param[in] PllIdx PLL ID
 */
void AmbaCSL_PllSetBypass(UINT32 PllIdx)
{
    volatile UINT32 *pPllCtrlReg = PLL_GetCtrlRegAddr(PllIdx);

    if (pPllCtrlReg != NULL) {
        *pPllCtrlReg |= (UINT32)0x200024U;
    }
}

/**
 *  AmbaCSL_PllSetFractionMode - Enable/Disable PLL fraction mode
 *  @param[in] PllIdx PLL ID
 *  @param[in] EnableFlag Use fractional mode or not
 */
void AmbaCSL_PllSetFractionMode(UINT32 PllIdx, UINT32 EnableFlag)
{
    volatile UINT32 *pPllCtrlReg = PLL_GetCtrlRegAddr(PllIdx);

    if (pPllCtrlReg != NULL) {
        if (EnableFlag != 0U) {
            *pPllCtrlReg |= (UINT32)0x8U;
        } else {
            *pPllCtrlReg &= ~(UINT32)0x8U;
        }

        /* A PLL reset is required on mode change */
        *pPllCtrlReg |= (UINT32)0x10U;
        *pPllCtrlReg &= ~(UINT32)0x10U;
    }
}

/**
 *  AmbaCSL_PllSetCtrl3Reg - Set PLL control 3 register value
 *  @param[in] PllIdx PLL ID
 *  @param[in] NewRegVal The new register value
 */
void AmbaCSL_PllSetCtrl3Reg(UINT32 PllIdx, UINT32 NewRegVal)
{
    volatile UINT32 *pPllCtrl3Reg = PLL_GetCtrl3RegAddr(PllIdx);
    *pPllCtrl3Reg = NewRegVal;
}

/**
 *  AmbaCSL_PllSetCtrlRegVal - Update PLL Control register value
 *  @param[in] PllIdx PLL ID
 *  @param[in] NewRegVal The new register value
 */
void AmbaCSL_PllSetCtrlRegVal(UINT32 PllIdx, UINT32 NewRegVal)
{
    volatile UINT32 *pPllCtrlReg = PLL_GetCtrlRegAddr(PllIdx);
    UINT32 Cycle1Us = AmbaCSL_PllGetNumCpuCycleUs();

    if (pPllCtrlReg != NULL) {
        *pPllCtrlReg = NewRegVal | 0x1U;
        AmbaDelayCycles(Cycle1Us << 4U);
        *pPllCtrlReg = NewRegVal;
    }
}

/**
 *  AmbaCSL_PllSetFracRegVal - Set PLL Fraction register value
 *  @param[in] PllIdx PLL ID
 *  @param[in] NewRegVal The new register value
 */
void AmbaCSL_PllSetFracRegVal(UINT32 PllIdx, UINT32 NewRegVal)
{
    volatile UINT32 *pPllFracReg = PLL_GetFracRegAddr(PllIdx);

    if (pPllFracReg != NULL) {
        *pPllFracReg = NewRegVal;
    }
}

/**
 *  AmbaCSL_PllSetNumCpuCycleUs - Set cpu tick value per micro-second
 *  @param[in] Frequency CPU frequency value
 */
void AmbaCSL_PllSetNumCpuCycleUs(UINT32 Frequency)
{
    AmbaCpuCyclePerUs = Frequency / 1000000U;
}

/**
 *  AmbaCSL_PllSetNumCpuCycleMs - Set system tick value per milli-second
 *  @param[in] Frequency CPU frequency value
 */
void AmbaCSL_PllSetNumCpuCycleMs(UINT32 Frequency)
{
    AmbaCpuCyclePerMs = Frequency / 1000U;
}

/**
 *  AmbaCSL_PllGetNumCpuCycleUs - Get cpu cycle value per micro-second
 *  @return CPU cycle value per micro-second
 */
UINT32 AmbaCSL_PllGetNumCpuCycleUs(void)
{
    return AmbaCpuCyclePerUs;
}

/**
 *  AmbaCSL_PllGetNumCpuCycleMs - Get cpu cycle value per milli-second
 *  @return CPU cycle value per milli-second
 */
UINT32 AmbaCSL_PllGetNumCpuCycleMs(void)
{
    return AmbaCpuCyclePerMs;
}
