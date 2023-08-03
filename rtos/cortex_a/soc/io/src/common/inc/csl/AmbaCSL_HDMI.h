/**
 * @file AmbaCSL_HDMI.h
 *
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

#ifndef AMBA_CSL_HDMI_H
#define AMBA_CSL_HDMI_H

#ifndef AMBA_HDMI_DEF_H
#include "AmbaHDMI_Def.h"
#endif

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#if defined(CONFIG_SOC_CV2)
#ifndef AMBA_REG_SCRATCHPAD_H
#include "AmbaReg_Scratchpad.h"
#endif
#else
#ifndef AMBA_REG_SCRATCHPAD_NS_H
#include "AmbaReg_ScratchpadNS.h"
#endif
#endif /* !(CONFIG_SOC_CV2) */
#include "AmbaReg_HDMI.h"

/*
 * HDMI Audio Clock Regeneration: Auto-generate CTS value in the Packet
 */
#define HDMI_CTS_AUTO_GEN                   ((UINT32)1U << 20U)

#define HDMI_CEC_RX_START_BIT               0U      /* Rx frame start bit */
#define HDMI_CEC_RX_DATA0_BIT               1U      /* Rx data bit for logical 0 */
#define HDMI_CEC_RX_DATA1_BIT               2U      /* Rx data bit for logical 1 */
#define HDMI_CEC_TX_START_BIT               3U      /* Tx frame start bit */
#define HDMI_CEC_TX_DATA0_BIT               4U      /* Tx data bit for logical 0 */
#define HDMI_CEC_TX_DATA1_BIT               5U      /* Tx data bit for logical 1 */

/*
 * Inline Function Definitions
 */
static inline void AmbaCSL_HdmiEnableCoreIdleInt(void)
{
    pAmbaHDMI_Reg->IntEnable.CoreIdle = 1U;
}
static inline void AmbaCSL_HdmiEnableHotPlugAttachInt(void)
{
    pAmbaHDMI_Reg->IntEnable.HotPlugAttach = 1U;
}
static inline void AmbaCSL_HdmiEnableHotPlugDetachInt(void)
{
    pAmbaHDMI_Reg->IntEnable.HotPlugDetach = 1U;
}
static inline void AmbaCSL_HdmiEnableRxSensedInt(void)
{
    pAmbaHDMI_Reg->IntEnable.RxSensed = 1U;
}
static inline void AmbaCSL_HdmiEnableRxNotSensedInt(void)
{
    pAmbaHDMI_Reg->IntEnable.RxNotSensed = 1U;
}
static inline void AmbaCSL_HdmiEnableCecTxDoneInt(void)
{
    pAmbaHDMI_Reg->IntEnable.CecTxDone = 1U;
}
static inline void AmbaCSL_HdmiEnableCecTxFailInt(void)
{
    pAmbaHDMI_Reg->IntEnable.CecTxFail = 1U;
}
static inline void AmbaCSL_HdmiEnableCecRxInt(void)
{
    pAmbaHDMI_Reg->IntEnable.CecRx = 1U;
}

static inline void AmbaCSL_HdmiCoreReset(void)
{
    pAmbaHDMI_Reg->CoreReset.Reset = 0U;
}
static inline void AmbaCSL_HdmiCoreResetRelease(void)
{
    pAmbaHDMI_Reg->CoreReset.Reset = 1U;
}
static inline void AmbaCSL_HdmiI2sFifoReset(void)
{
    pAmbaHDMI_Reg->I2sCtrl.Reset = 1U;
}
static inline void AmbaCSL_HdmiI2sFifoResetRelease(void)
{
    pAmbaHDMI_Reg->I2sCtrl.Reset = 0U;
}

static inline void AmbaCSL_HdmiClearAllInt(void)
{
    pAmbaHDMI_Reg->IntStatus = 0U;
}
static inline void AmbaCSL_HdmiClearCecTxInt(void)
{
    pAmbaHDMI_Reg->IntStatus &= ~(0x30U);
}
static inline void AmbaCSL_HdmiClearCecRxInt(void)
{
    pAmbaHDMI_Reg->IntStatus &= ~(0x08U);
}

static inline UINT32 AmbaCSL_HdmiIsHotPlugDetected(void)
{
    return pAmbaHDMI_Reg->Status.HotPlugDetected;
}
static inline UINT32 AmbaCSL_HdmiIsRxSensed(void)
{
    return pAmbaHDMI_Reg->Status.RxSensed;
}

static inline UINT32 AmbaCSL_HdmiIsCecTxOk(void)
{
    return (pAmbaHDMI_Reg->IntStatus & (UINT32)0x20) >> 5;
}
static inline UINT32 AmbaCSL_HdmiIsCecTxFail(void)
{
    return (pAmbaHDMI_Reg->IntStatus & (UINT32)0x10) >> 4;
}
static inline UINT32 AmbaCSL_HdmiIsCecRx(void)
{
    return (pAmbaHDMI_Reg->IntStatus & (UINT32)0x8) >> 3;
}

static inline void AmbaCSL_HdmiSetVideoDataSource(UINT8 d)
{
    pAmbaHDMI_Reg->DebugMode.VideoDataSource = (d);
}
static inline void AmbaCSL_HdmiSetAudioDataSource(UINT8 d)
{
    pAmbaHDMI_Reg->TestMode.AudioDataSource = (d);
}

static inline void AmbaCSL_HdmiSetEncoderCtrl(UINT32 d)
{
    pAmbaHDMI_Reg->CoreCtrl = (d);
}
static inline void AmbaCSL_HdmiSetTmdsCtrl(UINT8 d)
{
    pAmbaHDMI_Reg->AsyncFifoCtrl.Enable = (d);
}
static inline void AmbaCSL_HdmiSetHdcpCtrl(UINT8 d)
{
    pAmbaHDMI_Reg->HdcpCtrl.EncryptionStatusSignal = (d);
}

static inline void AmbaCSL_HdmiSetHsyncActiveHigh(void)
{
    pAmbaHDMI_Reg->VideoCtrl.HsyncPolarity = 0U;
}
static inline void AmbaCSL_HdmiSetHsyncActiveLow(void)
{
    pAmbaHDMI_Reg->VideoCtrl.HsyncPolarity = 1U;
}
static inline void AmbaCSL_HdmiSetVsyncActiveHigh(void)
{
    pAmbaHDMI_Reg->VideoCtrl.VsyncPolarity = 0U;
}
static inline void AmbaCSL_HdmiSetVsyncActiveLow(void)
{
    pAmbaHDMI_Reg->VideoCtrl.VsyncPolarity = 1U;
}

static inline void AmbaCSL_HdmiSetVideoDeepColor8Bit(void)
{
    pAmbaHDMI_Reg->VideoCtrl.DeepColorMode = 0U;
}
static inline void AmbaCSL_HdmiSetVideoDeepColor10Bit(void)
{
    pAmbaHDMI_Reg->VideoCtrl.DeepColorMode = 1U;
}
static inline void AmbaCSL_HdmiSetVideoDeepColor12Bit(void)
{
    pAmbaHDMI_Reg->VideoCtrl.DeepColorMode = 2U;
}
static inline void AmbaCSL_HdmiForceSendGeneralCtrlPkt(UINT8 d)
{
    pAmbaHDMI_Reg->VideoCtrl.ForceSendGeneralCtrlPkt = (d);
}

static inline void AmbaCSL_HdmiSetVideoScanFormat(UINT8 d)
{
    pAmbaHDMI_Reg->VideoCtrl.ScanFormat = (d);
}
static inline void AmbaCSL_HdmiSetVideoActivePixels(UINT16 d)
{
    pAmbaHDMI_Reg->VideoActivePixels.Size = (d);
}
static inline void AmbaCSL_HdmiSetVideoActiveLines(UINT16 d)
{
    pAmbaHDMI_Reg->VideoActiveLines.Size = (d);
}
static inline void AmbaCSL_HdmiSetVideoHSyncFrontPorch(UINT16 d)
{
    pAmbaHDMI_Reg->VideoHSyncFrontPorch.Size = (d);
}
static inline void AmbaCSL_HdmiSetVideoHSyncPulseWidth(UINT16 d)
{
    pAmbaHDMI_Reg->VideoHSyncPulseWidth.Size = (d);
}
static inline void AmbaCSL_HdmiSetVideoHSyncBackPorch(UINT16 d)
{
    pAmbaHDMI_Reg->VideoHSyncBackPorch.Size = (d);
}
static inline void AmbaCSL_HdmiSetVideoVSyncFrontPorch(UINT16 d)
{
    pAmbaHDMI_Reg->VideoVSyncFrontPorch.Size = (d);
}
static inline void AmbaCSL_HdmiSetVideoVSyncPulseWidth(UINT16 d)
{
    pAmbaHDMI_Reg->VideoVSyncPulseWidth.Size = (d);
}
static inline void AmbaCSL_HdmiSetVideoVSyncBackPorch(UINT16 d)
{
    pAmbaHDMI_Reg->VideoVSyncBackPorch.Size = (d);
}

static inline void AmbaCSL_HdmiSetI2sCtrl(UINT8 d)
{
    pAmbaHDMI_Reg->I2sCtrl.Enable = (d);
}
static inline void AmbaCSL_HdmiSetI2sMode(UINT8 d)
{
    pAmbaHDMI_Reg->I2sInterface.Format = (d);
}
static inline void AmbaCSL_HdmiSetI2sWordLength(UINT8 d)
{
    pAmbaHDMI_Reg->I2sWordLength.LengthMinus1 = (d);
}
static inline void AmbaCSL_HdmiSetI2sWordPosition(UINT8 d)
{
    pAmbaHDMI_Reg->I2sWordPosition.IgnoredBits = (d);
}
static inline void AmbaCSL_HdmiSetI2sSignalFormat(UINT32 d)
{
    pAmbaHDMI_Reg->I2sSignalFmt = (d);
}
static inline void AmbaCSL_HdmiSetI2sClockEdge(UINT8 d)
{
    pAmbaHDMI_Reg->I2sClockCtrl.SampleClkEdge = (d);
}

static inline void AmbaCSL_HdmiSetAudioChan0Ctrl(UINT8 d)
{
    pAmbaHDMI_Reg->AudioSampleCtrl.SamplePresent0 = (d);
}
static inline void AmbaCSL_HdmiSetAudioChan1Ctrl(UINT8 d)
{
    pAmbaHDMI_Reg->AudioSampleCtrl.SamplePresent1 = (d);
}

static inline void AmbaCSL_HdmiSetSaiRxCtrl(UINT8 d)
{
    pAmbaHDMI_Reg->DIPacketMiscCtrl.I2sRxEnable = (d);
}
static inline void AmbaCSL_HdmiSetAudioClkToSampleFreqRatio(UINT8 d)
{
    pAmbaHDMI_Reg->AudioClkAdjust.Mode = (UINT8)(d);
}
static inline void AmbaCSL_HdmiSetAudioSampleLayout(UINT8 d)
{
    pAmbaHDMI_Reg->AudioSampleLayout.Layout = (UINT8)(d);
}

static inline void AmbaCSL_HdmiUpdatePacketContent(void)
{
    pAmbaHDMI_Reg->PacketTxCtrl.UpdatePackets = 1U;
}

static inline void AmbaCSL_HdmiEnableScrambler(void)
{
    pAmbaHDMI_Reg->ScramblerCtrl.Enable = 1U;
}
static inline void AmbaCSL_HdmiDisableScrambler(void)
{
    pAmbaHDMI_Reg->ScramblerCtrl.Enable = 0U;
}

static inline void AmbaCSL_HdmiSetHdmiTxClkCtrl(UINT8 d)
{
    pAmbaHDMI_Reg->ClkCtrl.HdmiTxClock = (d);
}
static inline void AmbaCSL_HdmiSetHdcpClkCtrl(UINT8 d)
{
    pAmbaHDMI_Reg->ClkCtrl.HdcpClock = (d);
}
static inline void AmbaCSL_HdmiSetCecClkCtrl(UINT8 d)
{
    pAmbaHDMI_Reg->ClkCtrl.CecClock = (d);
}

static inline void AmbaCSL_HdmiCecReset(void)
{
    pAmbaHDMI_Reg->CecCtrl.Reset = 0U;
}
static inline void AmbaCSL_HdmiCecResetRelease(void)
{
    pAmbaHDMI_Reg->CecCtrl.Reset = 1U;
}
static inline UINT32 AmbaCSL_HdmiCecGetResetStatus(void)
{
    return pAmbaHDMI_Reg->CecStatus.ResetOK;
}

static inline UINT32 AmbaCSL_HdmiCecGetTxStatus(void)
{
    return pAmbaHDMI_Reg->CecStatus.TxStatus;
}
static inline UINT32 AmbaCSL_HdmiCecGetRxStatus(void)
{
    return pAmbaHDMI_Reg->CecStatus.RxStatus;
}

static inline void AmbaCSL_HdmiCecSetClockDivider(UINT32 d)
{
    pAmbaHDMI_Reg->CecClkDivider.Divider = (UINT16)(d);
}
static inline UINT32 AmbaCSL_HdmiCecGetClockDivider(void)
{
    return pAmbaHDMI_Reg->CecClkDivider.Divider;
}
static inline void AmbaCSL_HdmiCecSetLogicalAddress(UINT8 d)
{
    pAmbaHDMI_Reg->CecCtrl2.LogicalAddress = (d);
}

#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
static inline void AmbaCSL_HdmiSetClkRatio10(void)
{
    pAmbaRCT_Reg->HdmiClkSelect.ClkRatioSelect = 0U;
}
static inline void AmbaCSL_HdmiSetClkRatio40(void)
{
    pAmbaRCT_Reg->HdmiClkSelect.ClkRatioSelect = 1U;
}

static inline void AmbaCSL_HdmiPowerDownHdmiPhy(void)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Reset = 0U;
}
static inline void AmbaCSL_HdmiPowerUpHdmiPhy(void)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Reset = 1U;
}

static inline void AmbaCSL_HdmiSetPhyTerminationResistance(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.TerminationResistance = (d);
}

static inline void AmbaCSL_HdmiSetPhySinkCurrent(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.SinkCurrent = (d);
}
static inline void AmbaCSL_HdmiSetPhyBiasCurrent(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.BiasCurrent = (d);
}

static inline void AmbaCSL_HdmiSetPhy1stTapPreEmphasis(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.FirstTapPreEmphasis = (d);
}
static inline void AmbaCSL_HdmiSetPhy2ndTapPreEmphasis(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.SecondTapPreEmphasis = (d);
}
static inline void AmbaCSL_HdmiSetPhy3rdTapPreEmphasis(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.ThirdTapPreEmphasis = (d);
}
#else /* !(CONFIG_SOC_CV25) */
static inline void AmbaCSL_HdmiSetClkRatio10(void)
{
    pAmbaRCT_Reg->HdmiClkSelect.Bits.ClkRatioSelect = 0U;
}
static inline void AmbaCSL_HdmiSetClkRatio40(void)
{
    pAmbaRCT_Reg->HdmiClkSelect.Bits.ClkRatioSelect = 1U;
}

static inline void AmbaCSL_HdmiPowerDownHdmiPhy(void)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Bits.Reset = 0U;
}
static inline void AmbaCSL_HdmiPowerUpHdmiPhy(void)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Bits.Reset = 1U;
}

static inline void AmbaCSL_HdmiSetPhyTerminationResistance(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Bits.TerminationResistance = (d);
}

static inline void AmbaCSL_HdmiSetPhySinkCurrent(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Bits.SinkCurrent = (d);
}
static inline void AmbaCSL_HdmiSetPhyBiasCurrent(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Bits.BiasCurrent = (d);
}

static inline void AmbaCSL_HdmiSetPhy1stTapPreEmphasis(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Bits.FirstTapPreEmphasis = (d);
}
static inline void AmbaCSL_HdmiSetPhy2ndTapPreEmphasis(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Bits.SecondTapPreEmphasis = (d);
}
static inline void AmbaCSL_HdmiSetPhy3rdTapPreEmphasis(UINT8 d)
{
    pAmbaRCT_Reg->HdmiPhyCtrl.Bits.ThirdTapPreEmphasis = (d);
}

static inline UINT32 AmbaCSL_HdmiGetPhyCtrl(void)
{
    return pAmbaRCT_Reg->HdmiPhyCtrl.Data;
}
#if defined(CONFIG_SOC_CV2)
static inline void AmbaCSL_HdmiSetAudioSrc(UINT32 d)
{
    pAmbaScratchpadS_Reg->HdmiAudCtrl.HdmiAudClkSrc = (UINT8)d;
}
#else
static inline void AmbaCSL_HdmiSetAudioSrc(UINT32 d)
{
    pAmbaScratchpadNS_Reg->HdmiAudCtrl.HdmiAudClkSrc = (UINT8)d;
}
#endif /* !(CONFIG_SOC_CV2) */
#endif /* !(CONFIG_SOC_CV25)*/
/*
 * Defined in AmbaCSL_HDMI.c
 */
void AmbaCSL_HdmiSetAudioChanStat(const UINT32 *pChanStatus);
void AmbaCSL_HdmiSetAudioRegenCtrl(UINT32 EnableFlag, UINT32 N, UINT32 CTS);
void AmbaCSL_HdmiPacketCtrl(const AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, UINT32 EnableFlag);
void AmbaCSL_HdmiSetAVMUTE(UINT32 EnableFlag);

/*
 * HDMI CEC module
 */
void AmbaCSL_HdmiCecSetBitTiming(UINT32 BitType, UINT32 LowLevelPeriod, UINT32 WholePeriod);
void AmbaCSL_HdmiCecSetBitToleTiming(UINT32 LowerBound200us, UINT32 LowerBound350us, UINT32 UpperBound200us, UINT32 UpperBound350us);

void AmbaCSL_HdmiCecTx(const UINT8 *pMessage, UINT32 MsgSize);
void AmbaCSL_HdmiCecRx(UINT8 *pMessage, UINT32 *pMsgSize);

#endif /* AMBA_CSL_HDMI_H */
