/*
 * Copyright (c) 2021 Ambarella International LP
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

#ifndef AMBA_CSL_VOUT_H
#define AMBA_CSL_VOUT_H

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#ifndef AMBA_REG_VOUT_H
#include "AmbaReg_VOUT.h"
#endif

#ifndef AMBA_VOUT_DEF_H
#include "AmbaVOUT_Def.h"
#endif

/*
 * Macro Definitions
 */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static inline void AmbaCSL_Vout0EnableDigitalOutput(void)
{
    pAmbaVoutDisplay0_Reg->DispCtrl.DigitalOutput = 1U;
}
static inline void AmbaCSL_Vout0SetDigitalOutputMode(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.OutputMode = (d);
}
static inline void AmbaCSL_Vout0SetMipiLineSync(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiLineSync = (d);
}
static inline void AmbaCSL_Vout0SetMipiLineCount(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiLineCount = (d);
}
static inline void AmbaCSL_Vout0SetMipiFrameCount(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiFrameCount = (d);
}
static inline void AmbaCSL_Vout0SetMipiLineDataBlank(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiLineDataFrameBlank = (d);
}
static inline void AmbaCSL_Vout0SetMipiLineSyncBlank(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiLineSyncFrameBlank = (d);
}
static inline void AmbaCSL_Vout0SetMipiEccOrder(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiEccOrder = (d);
}

static inline void AmbaCSL_Mipi0SetHbpPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl0.PayloadBytes0 = (UINT16)(d);
}
static inline void AmbaCSL_Mipi0HbpUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl0.UseBlank = (d);
}
static inline void AmbaCSL_Mipi0SetHsaPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl0.PayloadBytes1 = (UINT16)(d);
}
static inline void AmbaCSL_Mipi0HsaUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl0.SyncUseBlank = (d);
}
static inline void AmbaCSL_Mipi0SetHfpPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl1.PayloadBytes0 = (UINT16)(d);
}
static inline void AmbaCSL_Mipi0HfpUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl1.UseBlank = (d);
}
static inline void AmbaCSL_Mipi0SetBllpPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl1.PayloadBytes1 = (UINT16)(d);
}
static inline void AmbaCSL_Mipi0BllpUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl1.SyncUseBlank = (d);
}

static inline void AmbaCSL_VoutDisplay0Reset(void)
{
    pAmbaVoutDisplay0_Reg->DispCtrl.ResetSection = (0x1U);
}

static inline void AmbaCSL_Mipi0SetCommandHeader(UINT32 d)
{
    pAmbaVoutMipiDsiCmd0_Reg->Header = (d);
}
static inline void AmbaCSL_Mipi0SetCommandParam0(UINT32 d)
{
    pAmbaVoutMipiDsiCmd0_Reg->Param[0] = (d);
}
static inline void AmbaCSL_Mipi0SetCommandParam1(UINT32 d)
{
    pAmbaVoutMipiDsiCmd0_Reg->Param[1] = (d);
}
static inline void AmbaCSL_Mipi0SetCommandParam2(UINT32 d)
{
    pAmbaVoutMipiDsiCmd0_Reg->Param[2] = (d);
}
static inline void AmbaCSL_Mipi0SetCommandParam3(UINT32 d)
{
    pAmbaVoutMipiDsiCmd0_Reg->Param[3] = (d);
}
static inline void AmbaCSL_Mipi0SetCommandCtrl(UINT32 d)
{
    pAmbaVoutMipiDsiCmd0_Reg->Ctrl = (d);
}

static inline void AmbaCSL_Vout1EnableDigitalOutput(void)
{
    pAmbaVoutDisplay1_Reg->DispCtrl.DigitalOutput = 1U;
}
static inline void AmbaCSL_Vout1SetDigitalOutputMode(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->DigitalOutputMode.OutputMode = (d);
}
static inline void AmbaCSL_Vout1SetMipiLineSync(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->DigitalOutputMode.MipiLineSync = (d);
}
static inline void AmbaCSL_Vout1SetMipiLineCount(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->DigitalOutputMode.MipiLineCount = (d);
}
static inline void AmbaCSL_Vout1SetMipiFrameCount(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->DigitalOutputMode.MipiFrameCount = (d);
}
static inline void AmbaCSL_Vout1SetMipiLineDataBlank(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->DigitalOutputMode.MipiLineDataFrameBlank = (d);
}
static inline void AmbaCSL_Vout1SetMipiLineSyncBlank(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->DigitalOutputMode.MipiLineSyncFrameBlank = (d);
}
static inline void AmbaCSL_Vout1SetMipiEccOrder(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->DigitalOutputMode.MipiEccOrder = (d);
}

static inline void AmbaCSL_Mipi1SetHbpPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay1_Reg->MipiBlankCtrl0.PayloadBytes0 = (UINT16)(d);
}
static inline void AmbaCSL_Mipi1HbpUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->MipiBlankCtrl0.UseBlank = (d);
}
static inline void AmbaCSL_Mipi1SetHsaPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay1_Reg->MipiBlankCtrl0.PayloadBytes1 = (UINT16)(d);
}
static inline void AmbaCSL_Mipi1HsaUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->MipiBlankCtrl0.SyncUseBlank = (d);
}
static inline void AmbaCSL_Mipi1SetHfpPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay1_Reg->MipiBlankCtrl1.PayloadBytes0 = (UINT16)(d);
}
static inline void AmbaCSL_Mipi1HfpUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->MipiBlankCtrl1.UseBlank = (d);
}
static inline void AmbaCSL_Mipi1SetBllpPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay1_Reg->MipiBlankCtrl1.PayloadBytes1 = (UINT16)(d);
}
static inline void AmbaCSL_Mipi1BllpUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay1_Reg->MipiBlankCtrl1.SyncUseBlank = (d);
}

static inline void AmbaCSL_VoutDisplay1Reset(void)
{
    pAmbaVoutDisplay1_Reg->DispCtrl.ResetSection = (0x1U);
}

static inline void AmbaCSL_Mipi1SetCommandHeader(UINT32 d)
{
    pAmbaVoutMipiDsiCmd1_Reg->Header = (d);
}
static inline void AmbaCSL_Mipi1SetCommandParam0(UINT32 d)
{
    pAmbaVoutMipiDsiCmd1_Reg->Param[0] = (d);
}
static inline void AmbaCSL_Mipi1SetCommandParam1(UINT32 d)
{
    pAmbaVoutMipiDsiCmd1_Reg->Param[1] = (d);
}
static inline void AmbaCSL_Mipi1SetCommandParam2(UINT32 d)
{
    pAmbaVoutMipiDsiCmd1_Reg->Param[2] = (d);
}
static inline void AmbaCSL_Mipi1SetCommandParam3(UINT32 d)
{
    pAmbaVoutMipiDsiCmd1_Reg->Param[3] = (d);
}
static inline void AmbaCSL_Mipi1SetCommandCtrl(UINT32 d)
{
    pAmbaVoutMipiDsiCmd1_Reg->Ctrl = (d);
}

static inline void AmbaCSL_Vout0MipiDsiSetCtrl0(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl0 = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetCtrl1(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl1 = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetCtrl2(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl2 = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetCtrl5(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl5 = (d);
}

static inline void AmbaCSL_Vout0MipiDsiSetAuxEn(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0AuxCtrl.TxAuxEn = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetPreEn(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0AuxCtrl.TxPreEn = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetPre(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0AuxCtrl.TxPre = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetTxMode(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0AuxCtrl.TxMode = (d);
}
static inline void AmbaCSL_Vout0MipiDsiBtaEnable(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0AuxCtrl.BtaRqst = (d);
}
static inline void AmbaCSL_Vout0MipiDsiBtaClrData(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0AuxCtrl.LpRecvClr = (d);
}
static inline void AmbaCSL_Vout0MipiCsiBypassEnable(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0AuxCtrl.CsiBypass = (d);
}
static inline void AmbaCSL_Vout0MipiDsiObsvSel(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0AuxCtrl.ObsvSel = (d);
}
static inline UINT32 AmbaCSL_Vout0MipiDsiGetAckData(void)
{
    return pAmbaRCT_Reg->MipiDsi0Obsv[0];
}

static inline void AmbaCSL_Vout0MipiDsiSetInitTx(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl3.RctInitTxCtrl = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetClkTrail(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl3.RctClkTrailCtrl = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetClkZero(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl3.RctClkZeroCtrl = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetClkPrepare(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl3.RctClkPrepareCtrl = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetClkMode(UINT32 Val)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl3.RctClkContModeCtrl = (UINT8)Val;
}

static inline void AmbaCSL_Vout0MipiDsiSetZero(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl4.ZeroCtrl = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetPrepare(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl4.PrepareCtrl = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetTrail(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl4.TrailCtrl = (d);
}
static inline void AmbaCSL_Vout0MipiDsiSetLpx(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi0Ctrl4.LpxCtrl = (d);
}

static inline void AmbaCSL_Vout1MipiDsiSetCtrl0(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl0 = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetCtrl1(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl1 = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetCtrl2(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl2 = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetCtrl5(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl5 = (d);
}

static inline void AmbaCSL_Vout1MipiDsiSetAuxEn(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1AuxCtrl.TxAuxEn = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetPreEn(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1AuxCtrl.TxPreEn = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetPre(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1AuxCtrl.TxPre = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetTxMode(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1AuxCtrl.TxMode = (d);
}
static inline void AmbaCSL_Vout1MipiDsiBtaEnable(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1AuxCtrl.BtaRqst = (d);
}
static inline void AmbaCSL_Vout1MipiDsiBtaClrData(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1AuxCtrl.LpRecvClr = (d);
}
static inline void AmbaCSL_Vout1MipiCsiBypassEnable(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1AuxCtrl.CsiBypass = (d);
}
static inline void AmbaCSL_Vout1MipiDsiObsvSel(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1AuxCtrl.ObsvSel = (d);
}
static inline UINT32 AmbaCSL_Vout1MipiDsiGetAckData(void)
{
    return pAmbaRCT_Reg->MipiDsi1Obsv[0];
}

static inline void AmbaCSL_Vout1MipiDsiSetInitTx(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl3.RctInitTxCtrl = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetClkTrail(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl3.RctClkTrailCtrl = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetClkZero(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl3.RctClkZeroCtrl = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetClkPrepare(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl3.RctClkPrepareCtrl = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetClkMode(UINT32 Val)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl3.RctClkContModeCtrl = (UINT8)Val;
}

static inline void AmbaCSL_Vout1MipiDsiSetZero(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl4.ZeroCtrl = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetPrepare(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl4.PrepareCtrl = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetTrail(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl4.TrailCtrl = (d);
}
static inline void AmbaCSL_Vout1MipiDsiSetLpx(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsi1Ctrl4.LpxCtrl = (d);
}

static inline UINT32 AmbaCSL_Vout0GetStatus(void)
{
    return pAmbaVoutDisplay0_Reg->DispStatus;
}
static inline void AmbaCSL_Vout0ClearStatus(void)
{
    pAmbaVoutDisplay0_Reg->DispStatus = 0xffffffffU;
}

static inline void AmbaCSL_Vout0SetTimeout(UINT32 d)
{
    pAmbaVoutDisplay0_Reg->WatchdogTimeout= (d);
}
static inline void AmbaCSL_Vout0SetMipiMaxFrame(UINT32 d)
{
    pAmbaVoutDisplay0_Reg->MipiMaxFrameCount = (d);
}

static inline UINT32 AmbaCSL_Vout1GetStatus(void)
{
    return pAmbaVoutDisplay1_Reg->DispStatus;
}
static inline void AmbaCSL_Vout1ClearStatus(void)
{
    pAmbaVoutDisplay1_Reg->DispStatus = 0xffffffffU;
}

static inline void AmbaCSL_Vout1SetTimeout(UINT32 d)
{
    pAmbaVoutDisplay1_Reg->WatchdogTimeout= (d);
}
static inline void AmbaCSL_Vout1SetMipiMaxFrame(UINT32 d)
{
    pAmbaVoutDisplay1_Reg->MipiMaxFrameCount = (d);
}

static inline UINT32 AmbaCSL_Vout2GetStatus(void)
{
    return pAmbaVoutDisplay2_Reg->DispStatus;
}
static inline void AmbaCSL_Vout2ClearStatus(void)
{
    pAmbaVoutDisplay2_Reg->DispStatus = 0xffffffffU;
}
static inline void AmbaCSL_Vout2SetTimeout(UINT32 d)
{
    pAmbaVoutDisplay2_Reg->WatchdogTimeout= (d);
}

static inline void AmbaCSL_VoutDisplay2Reset(void)
{
    pAmbaVoutDisplay2_Reg->DispCtrl.ResetSection = (0x1U);
}

static inline void AmbaCSL_VoutDacPowerDown(void)
{
    pAmbaRCT_Reg->DacCtrl.PowerDown = 1;
}
static inline void AmbaCSL_VoutDacPowerOn(void)
{
    pAmbaRCT_Reg->DacCtrl.PowerDown = 0;
}

static inline void AmbaCSL_VoutDacBistEnable(void)
{
    pAmbaRCT_Reg->DacCtrl.BistEnable = 1;
}
static inline void AmbaCSL_VoutDacBistDisable(void)
{
    pAmbaRCT_Reg->DacCtrl.BistEnable = 0;
}
#else
/* cv2x */
static inline void AmbaCSL_VoutEnableDigitalOutput(void)
{
    pAmbaVoutDisplay0_Reg->DispCtrl.DigitalOutput = 1U;
}
static inline void AmbaCSL_VoutSetDigitalOutputMode(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.OutputMode = (d);
}
#if !defined(CONFIG_SOC_H32)
static inline void AmbaCSL_VoutSetMipiLineSync(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiLineSync = (d);
}
static inline void AmbaCSL_VoutSetMipiLineCount(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiLineCount = (d);
}
static inline void AmbaCSL_VoutSetMipiFrameCount(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiFrameCount = (d);
}
static inline void AmbaCSL_VoutSetMipiLineDataBlank(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiLineDataFrameBlank = (d);
}
static inline void AmbaCSL_VoutSetMipiLineSyncBlank(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiLineSyncFrameBlank = (d);
}
static inline void AmbaCSL_VoutSetMipiEccOrder(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->DigitalOutputMode.MipiEccOrder = (d);
}

static inline void AmbaCSL_MipiSetHbpPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl0.PayloadBytes0 = (UINT16)(d);
}
static inline void AmbaCSL_MipiHbpUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl0.UseBlank = (d);
}
static inline void AmbaCSL_MipiSetHsaPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl0.PayloadBytes1 = (UINT16)(d);
}
static inline void AmbaCSL_MipiHsaUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl0.SyncUseBlank = (d);
}
static inline void AmbaCSL_MipiSetHfpPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl1.PayloadBytes0 = (UINT16)(d);
}
static inline void AmbaCSL_MipiHfpUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl1.UseBlank = (d);
}
static inline void AmbaCSL_MipiSetBllpPayloadBytes(UINT16 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl1.PayloadBytes1 = (UINT16)(d);
}
static inline void AmbaCSL_MipiBllpUseBlankEnable(UINT8 d)
{
    pAmbaVoutDisplay0_Reg->MipiBlankCtrl1.SyncUseBlank = (d);
}

static inline void AmbaCSL_MipiSetCommandHeader(UINT32 d)
{
    pAmbaVoutMipiDsiCmd_Reg->Header = (d);
}
static inline void AmbaCSL_MipiSetCommandParam0(UINT32 d)
{
    pAmbaVoutMipiDsiCmd_Reg->Param[0] = (d);
}
static inline void AmbaCSL_MipiSetCommandParam1(UINT32 d)
{
    pAmbaVoutMipiDsiCmd_Reg->Param[1] = (d);
}
static inline void AmbaCSL_MipiSetCommandParam2(UINT32 d)
{
    pAmbaVoutMipiDsiCmd_Reg->Param[2] = (d);
}
static inline void AmbaCSL_MipiSetCommandParam3(UINT32 d)
{
    pAmbaVoutMipiDsiCmd_Reg->Param[3] = (d);
}
static inline void AmbaCSL_MipiSetCommandCtrl(UINT32 d)
{
    pAmbaVoutMipiDsiCmd_Reg->Ctrl = (d);
}

static inline void AmbaCSL_VoutMipiDsiSetCtrl0(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl0 = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetCtrl1(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl1 = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetCtrl2(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl2 = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetCtrl5(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl5 = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetAuxCtrl(UINT32 d)
{
    pAmbaRCT_Reg->MipiDsiAuxCtrl = (d);
}
#if defined(CONFIG_SOC_CV28)
static inline UINT32 AmbaCSL_VoutMipiDsiGetAckReg(void)
{
    return pAmbaRCT_Reg->MipiDsiObsv[0];
}
#endif
static inline void AmbaCSL_VoutMipiDsiSetInitTx(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl3.RctInitTxCtrl = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetClkTrail(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl3.RctClkTrailCtrl = (d);
}

static inline void AmbaCSL_VoutMipiDsiSetClkZero(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl3.RctClkZeroCtrl = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetClkPrepare(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl3.RctClkPrepareCtrl = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetClkMode(UINT32 Val)
{
    pAmbaRCT_Reg->MipiDsiCtrl3.RctClkContModeCtrl = (UINT8)Val;
}

static inline void AmbaCSL_VoutMipiDsiSetZero(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl4.ZeroCtrl = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetPrepare(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl4.PrepareCtrl = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetTrail(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl4.TrailCtrl = (d);
}
static inline void AmbaCSL_VoutMipiDsiSetLpx(UINT8 d)
{
    pAmbaRCT_Reg->MipiDsiCtrl4.LpxCtrl = (d);
}
#endif
static inline void AmbaCSL_VoutDacPowerDown(void)
{
    pAmbaRCT_Reg->DacCtrl.PowerDown = 1;
}
static inline void AmbaCSL_VoutDacPowerOn(void)
{
    pAmbaRCT_Reg->DacCtrl.PowerDown = 0;
}

static inline UINT32 AmbaCSL_Vout0GetStatus(void)
{
    return pAmbaVoutDisplay0_Reg->DispStatus;
}
static inline void AmbaCSL_Vout0ClearStatus(void)
{
    pAmbaVoutDisplay0_Reg->DispStatus = 0xffffffffU;
}
#if !defined(CONFIG_SOC_CV28)
static inline UINT32 AmbaCSL_Vout1GetStatus(void)
{
    return pAmbaVoutDisplay1_Reg->DispStatus;
}
static inline void AmbaCSL_Vout1ClearStatus(void)
{
    pAmbaVoutDisplay1_Reg->DispStatus = 0xffffffffU;
}
static inline void AmbaCSL_VoutDisplay1Reset(void)
{
    pAmbaVoutDisplay1_Reg->DispCtrl.ResetSection = (0x1U);
}
#endif
#endif


/*
 * Defined in AmbaCSL_VOUT.c
 */

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 AmbaCSL_VoutCsiPhyPowerUp(UINT32 VoutChannel, UINT32 ParamVal);
UINT32 AmbaCSL_VoutCsiPhyPowerDown(UINT32 VoutChannel);
UINT32 AmbaCSL_VoutDsiPhyPowerUp(UINT32 VoutChannel, UINT32 ParamVal);
UINT32 AmbaCSL_VoutDsiPhyPowerDown(UINT32 VoutChannel);
void AmbaCSL_VoutMipiDsiSetBtaSrc(UINT32 BtaEnSrc);
UINT32 AmbaCSL_VoutMipiDsiGetAckData(UINT32 VoutChannel, UINT32 *pAckData);
#else
/* cv2x */
#if !defined(CONFIG_SOC_H32)
UINT32 AmbaCSL_VoutCsiPhyPowerUp(UINT32 ParamVal);
UINT32 AmbaCSL_VoutCsiPhyPowerDown(void);
UINT32 AmbaCSL_VoutDsiPhyPowerUp(UINT32 ParamVal);
UINT32 AmbaCSL_VoutDsiPhyPowerDown(void);
#endif
#if defined(CONFIG_SOC_CV28)
void AmbaCSL_VoutMipiDsiSetBtaSrc(UINT32 BtaEnSrc);
UINT32 AmbaCSL_VoutMipiDsiGetAckData(UINT32 *pAckData);
#endif
#endif

#endif /* AMBA_CSL_VOUT_H */
