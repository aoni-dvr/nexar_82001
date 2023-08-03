/*
 * Copyright (c) 2020 Ambarella International LP
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
 */

#ifndef AMBA_CSL_PLL_H
#define AMBA_CSL_PLL_H

#ifndef AMBA_RTSL_PLL_DEF_H
#include "AmbaRTSL_PLL_Def.h"
#endif

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

/*
 * Pll Control Register Definitions
 */
#define AMBA_PLL_CTRL_BYPASS_MASK       (0x4U)
#define AMBA_PLL_CTRL_BYPASS_OFFSET     (2U)
#define AMBA_PLL_CTRL_RESET_MASK        (0x10U)
#define AMBA_PLL_CTRL_RESET_OFFSET      (4U)
#define AMBA_PLL_CTRL_POWERDOWN_MASK    (0x20U)
#define AMBA_PLL_CTRL_POWERDOWN_OFFSET  (5U)
#define AMBA_PLL_CTRL_FRAC_MASK         (0x8U)
#define AMBA_PLL_CTRL_SOUT_MASK         (0xF0000U)
#define AMBA_PLL_CTRL_SOUT_OFFSET       (16U)
#define AMBA_PLL_CTRL_SDIV_MASK         (0xF000U)
#define AMBA_PLL_CTRL_SDIV_OFFSET       (12U)
#define AMBA_PLL_CTRL_INT_MASK          (0x3F000000U)
#define AMBA_PLL_CTRL_INT_OFFSET        (24U)
#define AMBA_PLL_CTRL1_HSDIV_MASK       (0xFFFU)

/*
 * Inline Function Definitions
 */
static inline UINT32 AmbaCSL_PllGetDebounceDivider(void)
{
    return pAmbaRCT_Reg->DebounceClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetGpioDebounceDivider(void)
{
    return pAmbaRCT_Reg->GpioDebounceClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetIrDivider(void)
{
    return pAmbaRCT_Reg->IrClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetMotorDivider(void)
{
    return pAmbaRCT_Reg->MotorClkDivider.Divider;
}

static inline UINT32 AmbaCSL_PllGetPwmClkSrcSel(void)
{
    return pAmbaRCT_Reg->PwmClkSrcSelect.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetPwmDivider(void)
{
    return pAmbaRCT_Reg->PwmClkDivider.Divider;
}

static inline UINT32 AmbaCSL_PllGetSpiMasterClkDivider(void)
{
    return pAmbaRCT_Reg->SpiMasterClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetSpiSlaveClkDivider(void)
{
    return pAmbaRCT_Reg->SpiSlaveClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetSpiNorClkDivider(void)
{
    return pAmbaRCT_Reg->SpiNorClkDivider.Divider;
}

static inline UINT32 AmbaCSL_PllGetUartApbDivider(void)
{
    return pAmbaRCT_Reg->UartApbClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetUart0Divider(void)
{
    return pAmbaRCT_Reg->Uart0ClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetUart1Divider(void)
{
    return pAmbaRCT_Reg->Uart1ClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetUart2Divider(void)
{
    return pAmbaRCT_Reg->Uart2ClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetUart3Divider(void)
{
    return pAmbaRCT_Reg->Uart3ClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetUartApbClkSrcSel(void)
{
    return pAmbaRCT_Reg->UartApbClkSrcSelect.Bits.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetUart0ClkSrcSel(void)
{
    return pAmbaRCT_Reg->Uart0ClkSrcSelect.Bits.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetUart1ClkSrcSel(void)
{
    return pAmbaRCT_Reg->Uart1ClkSrcSelect.Bits.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetUart2ClkSrcSel(void)
{
    return pAmbaRCT_Reg->Uart2ClkSrcSelect.Bits.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetUart3ClkSrcSel(void)
{
    return pAmbaRCT_Reg->Uart3ClkSrcSelect.Bits.ClkSrc;
}

static inline UINT32 AmbaCSL_PllGetAdcDivider(void)
{
    return pAmbaRCT_Reg->AdcClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetAdcRefClkSelect(void)
{
    return pAmbaRCT_Reg->AdcClkSelect.Bits.ClkSrc;
}

static inline UINT32 AmbaCSL_PllGetAudioRefClkCtrl0(void)
{
    return pAmbaRCT_Reg->AudioRefClkCtrl0.Bits.ClkRefSel;
}
static inline UINT32 AmbaCSL_PllGetAudioRefClkCtrl1(void)
{
    return pAmbaRCT_Reg->AudioRefClkCtrl1.Bits.ExtClkSrc;
}
static inline UINT32 AmbaCSL_PllGetAudioExtClkCtrl(void)
{
    return pAmbaRCT_Reg->AudioExtClkCtrl.Bits.AudioClkSrc;
}
static inline UINT32 AmbaCSL_PllGetAudio1ExtClkCtrl(void)
{
    return pAmbaRCT_Reg->Audio1ExtClkCtrl.Bits.AudioClkSrc;
}
static inline UINT32 AmbaCSL_PllGetAudio1Postscaler(void)
{
    return (pAmbaRCT_Reg->Audio1PllPostscaler.Divider + (UINT32)1U);
}

static inline UINT32 AmbaCSL_PllGetCanClkSrcSel(void)
{
    return pAmbaRCT_Reg->CanClkSrcSelect.Bits.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetCanDivider(void)
{
    return pAmbaRCT_Reg->CanClkDivider;
}
static inline UINT32 AmbaCSL_PllGetCortexClkSrcSelect(void)
{
    return pAmbaRCT_Reg->CortexClkSrcSelect.Bits.ClkSrc;
}

static inline UINT32 AmbaCSL_PllGetEnetPostscaler(void)
{
    return ((UINT32)pAmbaRCT_Reg->EnetPllPostscaler.Divider + 1U);
}

static inline UINT32 AmbaCSL_PllGetHdmiRefClkSelect(void)
{
    return pAmbaRCT_Reg->HdmiClkSelect.Bits.RefClkSel;
}
static inline UINT32 AmbaCSL_PllGetHdmiPllCascade(void)
{
    return pAmbaRCT_Reg->HdmiClkSelect.Bits.PllCascade;
}
static inline UINT32 AmbaCSL_PllGetHdmiUsePhyClkVo(void)
{
    return pAmbaRCT_Reg->HdmiClkSelect.Bits.UsePhyClkVo;
}
static inline UINT32 AmbaCSL_PllGetHdmiPhyPllSrcSelect(void)
{
    return pAmbaRCT_Reg->HdmiClkSelect.Bits.PhyPllSrcSel;
}
static inline UINT32 AmbaCSL_PllGetHdmiPhyPrescaler(void)
{
    return (pAmbaRCT_Reg->HdmiPhyPrescaler.Divider + (UINT32)1U);
}

static inline UINT32 AmbaCSL_PllGetSd0Postscaler(void)
{
    return pAmbaRCT_Reg->Sd0ClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PllGetSd1Postscaler(void)
{
    return pAmbaRCT_Reg->Sd1ClkDivider.Divider;
}
static inline UINT32 AmbaCSL_PLLGetSd0ScalerClkSrc(void)
{
    return pAmbaRCT_Reg->SdClkSrcSelect.Bits.Sd0ClockSrc;
}
static inline UINT32 AmbaCSL_PLLGetSd1ScalerClkSrc(void)
{
    return pAmbaRCT_Reg->SdClkSrcSelect.Bits.Sd1ClockSrc;
}

static inline UINT32 AmbaCSL_PllGetSensorPostscaler(void)
{
    return ((UINT32)pAmbaRCT_Reg->SensorPllPostscaler.Divider + 1U);
}

static inline UINT32 AmbaCSL_PllGetSpiMasterRefClkSelect(void)
{
    return pAmbaRCT_Reg->SpiMasterRefClkSelect.Bits.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetSpiSlaveRefClkSelect(void)
{
    return pAmbaRCT_Reg->SpiSlaveRefClkSelect.Bits.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetSpiNorRefClkSelect(void)
{
    return pAmbaRCT_Reg->SpiNorRefClkSelect.Bits.ClkSrc;
}

static inline UINT32 AmbaCSL_PllGetVin0Divider(void)
{
    return ((UINT32)pAmbaRCT_Reg->Vin0ClkDivider.Divider + 1U);
}
static inline UINT32 AmbaCSL_PllGetVin1Divider(void)
{
    return ((UINT32)pAmbaRCT_Reg->Vin1ClkDivider.Divider + 1U);
}
static inline UINT32 AmbaCSL_PllGetVoutTvRefClkCtrl0(void)
{
    return pAmbaRCT_Reg->VoutTvRefClkCtrl0.Bits.ClkRefSel;
}
static inline UINT32 AmbaCSL_PllGetVoutTvRefClkCtrl1(void)
{
    return pAmbaRCT_Reg->VoutTvRefClkCtrl1.Bits.ExtClkSrc;
}
static inline UINT32 AmbaCSL_PllGetVoutComboPhyClkSelect(void)
{
    return pAmbaRCT_Reg->VoutClkSelect.ComboPhySelect;
}
static inline UINT32 AmbaCSL_PllGetVoutBClkSrc(void)
{
    return pAmbaRCT_Reg->VoutClkSelect.VoutBSelect;
}
static inline UINT32 AmbaCSL_PllGetVoutBClkDivider(void)
{
    return (pAmbaRCT_Reg->VoutTvClkDivider.Divider + (UINT32)1U);
}

/**/
static inline void AmbaCSL_PllSetDebounceDivider(UINT32 d)
{
    pAmbaRCT_Reg->DebounceClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetGpioDebounceDivider(UINT32 d)
{
    pAmbaRCT_Reg->GpioDebounceClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetIrDivider(UINT32 d)
{
    pAmbaRCT_Reg->IrClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetMotorDivider(UINT32 d)
{
    pAmbaRCT_Reg->MotorClkDivider.Divider = d;
}

static inline void AmbaCSL_PllSetPwmRefClkSel(UINT32 d)
{
    pAmbaRCT_Reg->PwmClkSrcSelect.ClkSrc = ((UINT8)d);
}
static inline void AmbaCSL_PllSetPwmDivider(UINT32 d)
{
    pAmbaRCT_Reg->PwmClkDivider.Divider = d;
}

static inline void AmbaCSL_PllSetSpiMasterClkDivider(UINT32 d)
{
    pAmbaRCT_Reg->SpiMasterClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetSpiSlaveClkDivider(UINT32 d)
{
    pAmbaRCT_Reg->SpiSlaveClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetSpiNorClkDivider(UINT32 d)
{
    pAmbaRCT_Reg->SpiNorClkDivider.Divider = d;
}
static inline void AmbaCSL_PllDisableSpiMasterClk(UINT32 d)
{
    pAmbaRCT_Reg->SpiMasterClkDivider.SoftReset = (UINT8)d;
}
static inline void AmbaCSL_PllDisableSpiSlaveClk(UINT32 d)
{
    pAmbaRCT_Reg->SpiSlaveClkDivider.SoftReset = (UINT8)d;
}
static inline void AmbaCSL_PllDisableSpiNorClk(UINT32 d)
{
    pAmbaRCT_Reg->SpiNorClkDivider.SoftReset = (UINT8)d;
}

static inline void AmbaCSL_PllSetUartApbDivider(UINT32 d)
{
    pAmbaRCT_Reg->UartApbClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetUart0Divider(UINT32 d)
{
    pAmbaRCT_Reg->Uart0ClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetUart1Divider(UINT32 d)
{
    pAmbaRCT_Reg->Uart1ClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetUart2Divider(UINT32 d)
{
    pAmbaRCT_Reg->Uart2ClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetUart3Divider(UINT32 d)
{
    pAmbaRCT_Reg->Uart3ClkDivider.Divider = d;
}
static inline void AmbaCSL_PllSetUartApbClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->UartApbClkSrcSelect.Bits.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetUart0ClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->Uart0ClkSrcSelect.Bits.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetUart1ClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->Uart1ClkSrcSelect.Bits.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetUart2ClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->Uart2ClkSrcSelect.Bits.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetUart3ClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->Uart3ClkSrcSelect.Bits.ClkSrc = (UINT8)d;
}

static inline void AmbaCSL_PllSetAdcDivider(UINT32 d)
{
    pAmbaRCT_Reg->AdcClkDivider.Divider = (UINT16)d;
}
static inline void AmbaCSL_PllSetAdcRefClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->AdcClkSelect.Bits.ClkSrc = (UINT8)d;
}

static inline void AmbaCSL_PllSetAudioRefClkCtrl0(UINT32 d)
{
    pAmbaRCT_Reg->AudioRefClkCtrl0.Bits.ClkRefSel = (UINT8)d;
}
static inline void AmbaCSL_PllSetAudioRefClkCtrl1(UINT32 d)
{
    pAmbaRCT_Reg->AudioRefClkCtrl1.Bits.ExtClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetAudioExtClkCtrl(UINT32 d)
{
    pAmbaRCT_Reg->AudioExtClkCtrl.Bits.AudioClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetAudio1ExtClkCtrl(UINT32 d)
{
    pAmbaRCT_Reg->Audio1ExtClkCtrl.Bits.AudioClkSrc = (UINT8)d;
}

static inline void AmbaCSL_PllSetCanClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->CanClkSrcSelect.Bits.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetCanDivider(UINT32 d)
{
    pAmbaRCT_Reg->CanClkDivider = d;
}
static inline void AmbaCSL_PllSetCortexClkSrcSelect(UINT32 d)
{
    pAmbaRCT_Reg->CortexClkSrcSelect.Bits.ClkSrc = (UINT8)d;
}

static inline void AmbaCSL_PllSetHdmiRefClkSel(UINT32 d)
{
    pAmbaRCT_Reg->HdmiClkSelect.Bits.RefClkSel = (UINT8)d;
}
static inline void AmbaCSL_PllSetHdmiPllCascade(UINT32 d)
{
    pAmbaRCT_Reg->HdmiClkSelect.Bits.PllCascade = (UINT8)d;
}
static inline void AmbaCSL_PllSetHdmiUsePhyClkVo(UINT32 d)
{
    pAmbaRCT_Reg->HdmiClkSelect.Bits.UsePhyClkVo = (UINT8)d;
}
static inline void AmbaCSL_PllSetHdmiPhyPllSrcSelect(UINT32 d)
{
    pAmbaRCT_Reg->HdmiClkSelect.Bits.PhyPllSrcSel = (UINT8)d;
}
static inline void AmbaCSL_PllSetHdmiPhyPowerDown(UINT32 d)
{
    pAmbaRCT_Reg->HdmiClkSelect.Bits.PowerDown = (UINT8)d;
}

static inline void AmbaCSL_PllSetSd0Postscaler(UINT32 d)
{
    pAmbaRCT_Reg->Sd0ClkDivider.Divider = (UINT16)d;
}
static inline void AmbaCSL_PllSetSd1Postscaler(UINT32 d)
{
    pAmbaRCT_Reg->Sd1ClkDivider.Divider = (UINT16)d;
}
static inline void AmbaCSL_PLLSetSd0ScalerClkSrc(UINT32 d)
{
    pAmbaRCT_Reg->SdClkSrcSelect.Bits.Sd0ClockSrc = (UINT8)d;
}
static inline void AmbaCSL_PLLSetSd1ScalerClkSrc(UINT32 d)
{
    pAmbaRCT_Reg->SdClkSrcSelect.Bits.Sd1ClockSrc = (UINT8)d;
}

static inline void AmbaCSL_PllSetSpiMasterRefClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->SpiMasterRefClkSelect.Bits.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetSpiSlaveRefClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->SpiSlaveRefClkSelect.Bits.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetSpiNorRefClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->SpiNorRefClkSelect.Bits.ClkSrc = (UINT8)d;
}

static inline void AmbaCSL_PllSetVoutTvRefClkCtrl0(UINT32 d)
{
    pAmbaRCT_Reg->VoutTvRefClkCtrl0.Bits.ClkRefSel = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutTvRefClkCtrl1(UINT32 d)
{
    pAmbaRCT_Reg->VoutTvRefClkCtrl1.Bits.ExtClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutBypassSelect(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.VoutBypassSelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutComboPhyClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.ComboPhySelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutDisplay0Select(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.VoutASelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutDisplay1Select(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.VoutBSelect = (UINT8)d;
}

static inline void AmbaCSL_PllSetMuxedLvdsSpclkSelect(UINT32 d)
{
    pAmbaRCT_Reg->MuxedLvdsSpclk.Data = d;
}
static inline void AmbaCSL_PllDisableIdspScalar(void)
{
    pAmbaRCT_Reg->ClkDisableCtrl.Bits.IdspPostScaler = 1U;
}
static inline void AmbaCSL_PllSetClkCtrl(UINT32 d)
{
    pAmbaRCT_Reg->ClkEnableCtrl = (d | 0x400U);
}
static inline UINT32 AmbaCSL_PllGetClkCtrl(void)
{
    return pAmbaRCT_Reg->ClkEnableCtrl;
}

typedef struct {
    UINT32 Ctrl;       /* Control Register */
    UINT32 Ctrl2;      /* Control Register-2 */
    UINT32 Ctrl3;      /* Control Register-3 */
    UINT32 Fraction;   /* Fraction */
} AMBA_PLL_CTRL_REGS_VAL_s;

/*
 * Defined in AmbaCSL_PLL.c
 */
UINT32 AmbaCSL_PllGetPreScaleRegVal(UINT32 PllIdx);
void AmbaCSL_PllSetPreScaleRegVal(UINT32 PllIdx, UINT32 DivVal);
UINT32 AmbaCSL_PllGetPostScaleRegVal(UINT32 PllIdx);
void AmbaCSL_PllSetPostScaleRegVal(UINT32 PllIdx, UINT32 DivVal);
void AmbaCSL_PllGetCtrlRegsVal(UINT32 PllIdx, AMBA_PLL_CTRL_REGS_VAL_s *pPllCtrlRegsVal);
void AmbaCSL_PllSetCtrlRegsVal(UINT32 PllIdx, AMBA_PLL_CTRL_REGS_VAL_s *pNewRegsVal);

void AmbaCSL_PllSetPowerDown(UINT32 PllIdx);
void AmbaCSL_PllSetBypass(UINT32 PllIdx);
void AmbaCSL_PllSetFractionMode(UINT32 PllIdx, UINT32 EnableFlag);
void AmbaCSL_PllSetCtrl3Reg(UINT32 PllIdx, UINT32 NewRegVal);
void AmbaCSL_PllSetCtrlRegVal(UINT32 PllIdx, UINT32 NewRegVal);
void AmbaCSL_PllSetFracRegVal(UINT32 PllIdx, UINT32 NewRegVal);

void AmbaCSL_PllSetNumCpuCycleUs(UINT32 Frequency);
void AmbaCSL_PllSetNumCpuCycleMs(UINT32 Frequency);
UINT32 AmbaCSL_PllGetNumCpuCycleUs(void);
UINT32 AmbaCSL_PllGetNumCpuCycleMs(void);

#endif /* AMBA_CSL_PLL_H */
