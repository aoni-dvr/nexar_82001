/**
 *  @file AmbaCSL_PLL.h
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
 *  @details Definitions & Constants for PLL CSL APIs
 *
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
 * Inline Function Definitions
 */
static inline UINT32 AmbaCSL_PllGetDebounceDivider(void)
{
    return pAmbaRCT_Reg->PlugInDetectClkDivider.ClkDivider;
}

static inline UINT32 AmbaCSL_PllGetPwmClkSrcSel(void)
{
    return pAmbaRCT_Reg->PwmClkSrcSelect;
}
static inline UINT32 AmbaCSL_PllGetPwmDivider(void)
{
    return pAmbaRCT_Reg->PwmClkDivider.ClkDivider;
}

static inline UINT32 AmbaCSL_PllGetSpiMasterClkDivider(void)
{
    return pAmbaRCT_Reg->SpiMasterClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PllGetSpiSlaveClkDivider(void)
{
    return pAmbaRCT_Reg->SpiSlaveClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PllGetSpiNorClkDivider(void)
{
    return pAmbaRCT_Reg->SpiNorClkDivider.ClkDivider;
}

static inline UINT32 AmbaCSL_PllGetUartApbDivider(void)
{
    return pAmbaRCT_Reg->UartApbClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PllGetUart0Divider(void)
{
    return pAmbaRCT_Reg->Uart0ClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PllGetUart1Divider(void)
{
    return pAmbaRCT_Reg->Uart1ClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PllGetUart2Divider(void)
{
    return pAmbaRCT_Reg->Uart2ClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PllGetUart3Divider(void)
{
    return pAmbaRCT_Reg->Uart3ClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PllGetUartApbClkSrcSel(void)
{
    return pAmbaRCT_Reg->UartApbClkSrcSelect.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetUart0ClkSrcSel(void)
{
    return pAmbaRCT_Reg->Uart0ClkSrcSelect.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetUart1ClkSrcSel(void)
{
    return pAmbaRCT_Reg->Uart1ClkSrcSelect.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetUart2ClkSrcSel(void)
{
    return pAmbaRCT_Reg->Uart2ClkSrcSelect.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetUart3ClkSrcSel(void)
{
    return pAmbaRCT_Reg->Uart3ClkSrcSelect.ClkSrc;
}

static inline UINT32 AmbaCSL_PllGetAudioRefClkCtrl0(void)
{
    return pAmbaRCT_Reg->AudioRefClkCtrl0.ClkRefSel;
}
static inline UINT32 AmbaCSL_PllGetAudioRefClkCtrl1(void)
{
    return pAmbaRCT_Reg->AudioRefClkCtrl1.ExtClkSrc;
}
static inline UINT32 AmbaCSL_PllGetAudioExtClkCtrl(void)
{
    return pAmbaRCT_Reg->AudioExtClkCtrl.AudioClkSelect;
}

static inline UINT32 AmbaCSL_PllGetCanClkSrcSel(void)
{
    return pAmbaRCT_Reg->CanClkSrcSelect.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetCanDivider(void)
{
    return pAmbaRCT_Reg->CanClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PllGetCortex0ClkSrcSelect(void)
{
    return pAmbaRCT_Reg->Cortex0ClkSrcSelect.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetCortex1ClkSrcSelect(void)
{
    return pAmbaRCT_Reg->Cortex1ClkSrcSelect.ClkSrc;
}

static inline UINT32 AmbaCSL_PllGetEnetPostscaler(void)
{
    return ((UINT32)pAmbaRCT_Reg->Enet0ClkDivider.ClkDivider + 1U);
}
static inline UINT32 AmbaCSL_PllGetEnet1Postscaler(void)
{
    return ((UINT32)pAmbaRCT_Reg->Enet1ClkDivider.ClkDivider + 1U);
}

static inline UINT32 AmbaCSL_PllGetSd0Postscaler(void)
{
    return pAmbaRCT_Reg->Sd0ClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PllGetSd1Postscaler(void)
{
    return pAmbaRCT_Reg->Sd1ClkDivider.ClkDivider;
}
static inline UINT32 AmbaCSL_PLLGetSd0ScalerClkSrc(void)
{
    return pAmbaRCT_Reg->SdClkSrcSelect.Sd0ClkSrc;
}
static inline UINT32 AmbaCSL_PLLGetSd1ScalerClkSrc(void)
{
    return pAmbaRCT_Reg->SdClkSrcSelect.Sdio0ClkSrc;
}

static inline UINT32 AmbaCSL_PllGetSensorPostscaler(void)
{
    return ((UINT32)pAmbaRCT_Reg->SensorPllPostscaler.ClkDivider + 1U);
}
static inline UINT32 AmbaCSL_PllGetSensor2Postscaler(void)
{
    return ((UINT32)pAmbaRCT_Reg->Sensor2PllPostscaler.ClkDivider + 1U);
}

static inline UINT32 AmbaCSL_PllGetSpiMasterRefClkSelect(void)
{
    return pAmbaRCT_Reg->SpiMasterClkSrcSelect;
}
static inline UINT32 AmbaCSL_PllGetSpiSlaveRefClkSelect(void)
{
    return pAmbaRCT_Reg->SpiSlaveRefClkSelect.ClkSrc;
}
static inline UINT32 AmbaCSL_PllGetSpiNorRefClkSelect(void)
{
    return pAmbaRCT_Reg->SpiNorRefClkSelect.ClkSrc;
}

static inline UINT32 AmbaCSL_PllGetVin0Divider(void)
{
    return ((UINT32)pAmbaRCT_Reg->Vin0ClkDivider.ClkDivider + 1U);
}
static inline UINT32 AmbaCSL_PllGetVin1Divider(void)
{
    return ((UINT32)pAmbaRCT_Reg->Vin1ClkDivider.ClkDivider + 1U);
}
static inline UINT32 AmbaCSL_PllGetVin1ClkSelect(void)
{
    return pAmbaRCT_Reg->Vin1ClkSelect;
}
static inline UINT32 AmbaCSL_PllGetVoutTvRefClkCtrl0(void)
{
    return pAmbaRCT_Reg->Vout1RefClkCtrl0.ClkRefSel;
}
static inline UINT32 AmbaCSL_PllGetVoutTvRefClkCtrl1(void)
{
    return pAmbaRCT_Reg->Vout1RefClkCtrl1.ExtClkSrc;
}
static inline UINT32 AmbaCSL_PllGetVoutComboPhy0ClkSel(void)
{
    return pAmbaRCT_Reg->VoutClkSelect.ComboPhy0ClkSelect;
}
static inline UINT32 AmbaCSL_PllGetVoutComboPhy1ClkSel(void)
{
    return pAmbaRCT_Reg->VoutClkSelect.ComboPhy1ClkSelect;
}
static inline UINT32 AmbaCSL_PllGetVout1ClkSrc(void)
{
    return pAmbaRCT_Reg->VoutClkSelect.Vout1ClkSelect;
}

/**/
static inline void AmbaCSL_PllSetDebounceDivider(UINT32 d)
{
    pAmbaRCT_Reg->PlugInDetectClkDivider.ClkDivider = d;
}

static inline void AmbaCSL_PllSetPwmRefClkSel(UINT32 d)
{
    pAmbaRCT_Reg->PwmClkSrcSelect = d;
}
static inline void AmbaCSL_PllSetPwmDivider(UINT32 d)
{
    pAmbaRCT_Reg->PwmClkDivider.ClkDivider = d;
}

static inline void AmbaCSL_PllSetSpiMasterClkDivider(UINT32 d)
{
    pAmbaRCT_Reg->SpiMasterClkDivider.ClkDivider = d;
}
static inline void AmbaCSL_PllSetSpiSlaveClkDivider(UINT32 d)
{
    pAmbaRCT_Reg->SpiSlaveClkDivider.ClkDivider = d;
}
static inline void AmbaCSL_PllSetSpiNorClkDivider(UINT32 d)
{
    pAmbaRCT_Reg->SpiNorClkDivider.ClkDivider = d;
}
static inline void AmbaCSL_PllDisableSpiMasterClk(UINT32 d)
{
    pAmbaRCT_Reg->SpiMasterClkDivider.Reset = (UINT8)d;
}
static inline void AmbaCSL_PllDisableSpiSlaveClk(UINT32 d)
{
    pAmbaRCT_Reg->SpiSlaveClkDivider.Reset = (UINT8)d;
}
static inline void AmbaCSL_PllDisableSpiNorClk(UINT32 d)
{
    pAmbaRCT_Reg->SpiNorClkDivider.Reset = (UINT8)d;
}

static inline void AmbaCSL_PllSetUartApbDivider(UINT32 d)
{
    pAmbaRCT_Reg->UartApbClkDivider.ClkDivider = d;
}
static inline void AmbaCSL_PllSetUart0Divider(UINT32 d)
{
    pAmbaRCT_Reg->Uart0ClkDivider.ClkDivider = d;
}
static inline void AmbaCSL_PllSetUart1Divider(UINT32 d)
{
    pAmbaRCT_Reg->Uart1ClkDivider.ClkDivider = d;
}
static inline void AmbaCSL_PllSetUart2Divider(UINT32 d)
{
    pAmbaRCT_Reg->Uart2ClkDivider.ClkDivider = d;
}
static inline void AmbaCSL_PllSetUart3Divider(UINT32 d)
{
    pAmbaRCT_Reg->Uart3ClkDivider.ClkDivider = d;
}
static inline void AmbaCSL_PllSetUartApbClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->UartApbClkSrcSelect.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetUart0ClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->Uart0ClkSrcSelect.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetUart1ClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->Uart1ClkSrcSelect.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetUart2ClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->Uart2ClkSrcSelect.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetUart3ClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->Uart3ClkSrcSelect.ClkSrc = (UINT8)d;
}

static inline void AmbaCSL_PllSetAudioRefClkCtrl0(UINT32 d)
{
    pAmbaRCT_Reg->AudioRefClkCtrl0.ClkRefSel = (UINT8)d;
}
static inline void AmbaCSL_PllSetAudioRefClkCtrl1(UINT32 d)
{
    pAmbaRCT_Reg->AudioRefClkCtrl1.ExtClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetAudioExtClkCtrl(UINT32 d)
{
    pAmbaRCT_Reg->AudioExtClkCtrl.AudioClkSelect = (UINT8)d;
}

static inline void AmbaCSL_PllSetCanClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->CanClkSrcSelect.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetCanDivider(UINT32 d)
{
    pAmbaRCT_Reg->CanClkDivider.ClkDivider = (UINT8)d;
}

static inline void AmbaCSL_PllSetSd0Postscaler(UINT32 d)
{
    pAmbaRCT_Reg->Sd0ClkDivider.ClkDivider = (UINT16)d;
}
static inline void AmbaCSL_PllSetSd1Postscaler(UINT32 d)
{
    pAmbaRCT_Reg->Sd1ClkDivider.ClkDivider = (UINT16)d;
}
static inline void AmbaCSL_PLLSetSd0ScalerClkSrc(UINT32 d)
{
    pAmbaRCT_Reg->SdClkSrcSelect.Sd0ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PLLSetSd1ScalerClkSrc(UINT32 d)
{
    pAmbaRCT_Reg->SdClkSrcSelect.Sdio0ClkSrc = (UINT8)d;
}

static inline void AmbaCSL_PllSetSpiMasterRefClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->SpiMasterClkSrcSelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetSpiSlaveRefClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->SpiSlaveRefClkSelect.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllSetSpiNorRefClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->SpiNorRefClkSelect.ClkSrc = (UINT8)d;
}

static inline void AmbaCSL_PllSetVoutDisplay0Bypass(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.Vout0BypassSelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutDisplay1Bypass(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.Vout1BypassSelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutDisplay0Select(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.Vout0ClkSelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutDisplay1Select(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.Vout1ClkSelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutComboPhy0ClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.ComboPhy0ClkSelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetVoutComboPhy1ClkSelect(UINT32 d)
{
    pAmbaRCT_Reg->VoutClkSelect.ComboPhy1ClkSelect = (UINT8)d;
}
static inline void AmbaCSL_PllSetVin1ClkSrcSel(UINT32 d)
{
    pAmbaRCT_Reg->Vin1ClkSelect = (UINT8)d;
}

static inline void AmbaCSL_PllSetMuxedLvdsSpclkSelect(UINT32 d)
{
    pAmbaRCT_Reg->MuxedLvdsSpclk.ClkSrc = (UINT8)d;
}
static inline void AmbaCSL_PllDisableIdspScalar(void)
{
    pAmbaRCT_Reg->ClkDisableCtrl.IdspPostScaler = 1U;
}
static inline void AmbaCSL_PllSetClkCtrl(UINT32 d)
{
    pAmbaRCT_Reg->ClkEnableCtrl = (d | 0xC00U);
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
