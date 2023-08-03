// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 * @file AmbaRTSL_HDMI.c
 *
 * @copyright Copyright (c) 2021 Ambarella International LP
 *
 * @details HDMI RTSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaRTSL_HDMI.h"
#include "AmbaCSL_HDMI.h"

#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_GIC.h"
#endif


/**
 *  HDMI_UpdateCheckSumOfDataIslandPacket - Generate checksum of the data island packet
 *  @param[in,out] pPacket pointer to a DI packet
 */
static void HDMI_UpdateCheckSumOfDataIslandPacket(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket)
{
    UINT32 i, Sum;

    /* Sum of InfoFrame header */
    Sum = (pPacket->Header & 0xffU) +
          ((pPacket->Header >> 8U) & 0xffU) +
          ((pPacket->Header >> 16U) & 0xffU);

    /* Plus the sum of InfoFrame payload */
    for (i = 0U; i < ((pPacket->Header >> 16U) & 0xffU); i ++) {
        Sum += pPacket->Payload[i];
    }

    pPacket->Header &= 0xffffffU;
    pPacket->Header |= (UINT32)(0x100U - (Sum & 0xffU)) << 24U;
}

/**
 *  AmbaRTSL_HdmiInit - HDMI RTSL initializations
 */
void AmbaRTSL_HdmiInit(void)
{
    /* DO NOT READ ANY REGISTER before SwReset released and HdmiTxClock enabled. */

    /* Software reset on AHB clock, pixel clock, audio clock and HDMI PHY clock available */
    AmbaCSL_HdmiCoreReset();
    AmbaCSL_HdmiCoreResetRelease();

    AmbaCSL_HdmiSetHdmiTxClkCtrl(1U);    /* Enable DVI/HDMI signal encoder clock */
    AmbaCSL_HdmiSetHdcpClkCtrl(0U);
    AmbaCSL_HdmiSetCecClkCtrl(0U);

    AmbaCSL_HdmiPowerDownHdmiPhy();     /* Powered-Down and Reset */

    /* Enable interrupts for Hot-Plug Detection */
    AmbaCSL_HdmiEnableHotPlugAttachInt();
    AmbaCSL_HdmiEnableHotPlugDetachInt();
    AmbaCSL_HdmiEnableRxSensedInt();
    AmbaCSL_HdmiEnableRxNotSensedInt();
}

/* TODO: Linux HDMI Interrupt needs to be refined */
#if defined(CONFIG_LINUX)
static AMBA_HDMI_HPD_ISR_f AmbaHdmHpdISR;
static AMBA_HDMI_CEC_ISR_f AmbaHdmCecISR;

/**
 *  HDMI_IntHandler - ISR for HDMI transmitter
 *  @param[in] IntID Interupt ID
 *  @param[in] IsrArg Optional argument of the interrupt handler
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void HDMI_IntHandler(UINT32 IntID, UINT32 IsrArg)
{
    UINT32 HotPlugDetected;
    UINT32 RxSensed;

    /* CEC */
    if (AmbaCSL_HdmiIsCecTxOk() != 0U) {
        /* successful transmitting CEC frame */
        if (AmbaHdmCecISR != NULL) {
            AmbaHdmCecISR(HDMI_CEC_TX_OK);
        }
    } else if (AmbaCSL_HdmiIsCecTxFail() != 0U) {
        /* failed transmitting CEC frame (after retry fail) */
        if (AmbaHdmCecISR != NULL) {
            AmbaHdmCecISR(HDMI_CEC_TX_NG);
        }
    } else if (AmbaCSL_HdmiIsCecRx() != 0U) {
        /* successful receiving CEC frame (frame is available) */
        if (AmbaHdmCecISR != NULL) {
            AmbaHdmCecISR(HDMI_CEC_RX_OK);
        }
    } else {
        /* Hot-Plug Detection */
        if (AmbaHdmHpdISR != NULL) {
            HotPlugDetected = AmbaCSL_HdmiIsHotPlugDetected();
            RxSensed = AmbaCSL_HdmiIsRxSensed();
            AmbaHdmHpdISR(0U, HotPlugDetected, RxSensed);
        }
    }

    AmbaCSL_HdmiClearAllInt();
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&IsrArg);
}
#pragma GCC pop_options

/**
 *  AmbaRTSL_HdmiHookHpdHandler - Hook HPD event handler
 *  @param[in] IntHandler Interrupt handler
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiHookHpdHandler(AMBA_HDMI_HPD_ISR_f IntHandler)
{
    UINT32 RetVal = HDMI_ERR_NONE;

    AmbaHdmHpdISR = IntHandler;

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiHookCecHandler - Hook CEC event handler
 *  @param[in] IntHandler Interrupt handler
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiHookCecHandler(AMBA_HDMI_CEC_ISR_f IntHandler)
{
    UINT32 RetVal = HDMI_ERR_NONE;

    AmbaHdmCecISR = IntHandler;

    return RetVal;
}
#endif

/**
 *  AmbaRTSL_HdmiSetPowerCtrl - Power control
 *  @param[in] EnableFlag Enable flag. 0 = Disable HDMI Tx; 1 = Enable HDMI Tx
 */
void AmbaRTSL_HdmiSetPowerCtrl(UINT32 EnableFlag)
{
    if (EnableFlag == 0U) {
        AmbaCSL_HdmiPowerDownHdmiPhy();         /* Power down and reset most of analog circuit */
    } else {
        AmbaCSL_HdmiPowerUpHdmiPhy();           /* Power up most of analog circuit */
    }
}

/**
 *  AmbaRTSL_HdmiSetPhyCtrl - PHY control
 *  @param[in] ParamID PHY parameter type
 *  @param[in] ParamVal PHY parameter value
 */
void AmbaRTSL_HdmiSetPhyCtrl(UINT32 ParamID, UINT32 ParamVal)
{
    if (ParamID == 0U) {
        AmbaCSL_HdmiSetPhyBiasCurrent((UINT8)ParamVal);
    } else if (ParamID == 1U) {
        AmbaCSL_HdmiSetPhySinkCurrent((UINT8)ParamVal);
    } else if (ParamID == 2U) {
        AmbaCSL_HdmiSetPhy1stTapPreEmphasis((UINT8)ParamVal);
    } else if (ParamID == 3U) {
        AmbaCSL_HdmiSetPhy2ndTapPreEmphasis((UINT8)ParamVal);
    } else if (ParamID == 4U) {
        AmbaCSL_HdmiSetPhy3rdTapPreEmphasis((UINT8)ParamVal);
    } else {
        /* Do nothing!! */
    }

    AmbaCSL_HdmiSetPhyTerminationResistance(0U);
}

/**
 *  AmbaRTSL_HdmiCheckConnection - Check connection status
 *  @param[out] pHotPlug pointer to Hot-Plug Detection status
 *  @param[out] pRxSense pointer to Receiver Sensitivity Detection status
 */
void AmbaRTSL_HdmiCheckConnection(UINT32 *pHotPlug, UINT32 *pRxSense)
{
    if (AmbaCSL_HdmiIsHotPlugDetected() != 0U) {
        /* HPD pin high means E-EDID information is available from the source */
        *pHotPlug = 1U;
    } else {
        /* HPD pin low means E-EDID is not available for reading */
        *pHotPlug = 0U;
    }

    if (AmbaCSL_HdmiIsRxSensed() != 0U) {
        /* Receiver is connected to the transmitter and powered up */
        *pRxSense = 1U;
    } else {
        /* Cable is unplugged or the receiver site is powered off */
        *pRxSense = 0U;
    }
}

/**
 *  AmbaRTSL_HdmiStart - Start DVI/HDMI signal encoder
 *  @param[in] IsHDMI HDMI mode or DVI mode
 */
void AmbaRTSL_HdmiStart(UINT32 IsHDMI)
{
    if (IsHDMI != 0U) {
        AmbaCSL_HdmiSetHdcpCtrl(HDMI_HDCP_EESS);    /* Set HDCP EESS mode for HDMI */
        AmbaCSL_HdmiSetEncoderCtrl(HDMI_CORE_HDMI); /* Let TMDS transmitter enter HDMI mode */
    } else {
        AmbaCSL_HdmiSetHdcpCtrl(HDMI_HDCP_OESS);    /* Set HDCP OESS mode by default */
        AmbaCSL_HdmiSetEncoderCtrl(HDMI_CORE_DVI);  /* Let TMDS transmitter enter DVI mode */
    }
    AmbaCSL_HdmiSetTmdsCtrl(1U);
}

/**
 *  AmbaRTSL_HdmiStop - Stop DVI/HDMI signal encoder
 */
void AmbaRTSL_HdmiStop(void)
{
    AmbaCSL_HdmiSetTmdsCtrl(0U);
    AmbaCSL_HdmiSetEncoderCtrl(HDMI_CORE_NONE);
}

/**
 *  AmbaRTSL_HdmiPacketUpdate - Apply all the changes of ACP, ISRC, Gamut Metadata, and InfoFrame packets at the same time when the function "unfreezes" the packet update.
 */
void AmbaRTSL_HdmiPacketUpdate(void)
{
    AmbaCSL_HdmiUpdatePacketContent();
}

/**
 *  AmbaRTSL_HdmiDataIslandPackCtrl - Transmission control of Data Island InfoFrame packets
 *  @param[in] pPacket pointer to InfoFrame packet
 *  @param[in] Mode Packet transmission mode
 */
void AmbaRTSL_HdmiDataIslandPackCtrl(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, UINT32 Mode)
{
    if (pPacket != NULL) {
        HDMI_UpdateCheckSumOfDataIslandPacket(pPacket);

        if (Mode == HDMI_PACKET_SEND_ONCE) {
            AmbaCSL_HdmiPacketCtrl(pPacket, HDMI_PACKET_TX_NO_REPEAT);
        } else if (Mode == HDMI_PACKET_SEND_EVERY_FRAME) {
            AmbaCSL_HdmiPacketCtrl(pPacket, HDMI_PACKET_TX_REPEAT);
        } else {
            AmbaCSL_HdmiPacketCtrl(pPacket, HDMI_PACKET_TX_DISABLE);
        }
    }
}

/**
 *  AmbaRTSL_HdmiReset - Reset HDMI configuration
 *  @param[in] ResetFlag Reset flag (0 or 1)
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiReset(UINT8 ResetFlag)
{
    UINT32 RetVal = HDMI_ERR_NONE;

    /* Software reset on AHB clock, pixel clock, audio clock and HDMI PHY clock available */
    if (ResetFlag == 0U) {
        AmbaCSL_HdmiCoreResetRelease();
    } else if (ResetFlag == 1U) {
        AmbaCSL_HdmiCoreReset();
    } else {
        RetVal = HDMI_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiConfigScrambler - Set clock ratio and config scrambler
 *  @param[in] EnableFlag Enable flag
 */
void AmbaRTSL_HdmiConfigScrambler(UINT8 EnableFlag)
{
    if (EnableFlag != 0U) {
        AmbaCSL_HdmiEnableScrambler();
        AmbaCSL_HdmiSetClkRatio40();
    } else {
        AmbaCSL_HdmiDisableScrambler();
        AmbaCSL_HdmiSetClkRatio10();
    }
}


/**
 *  AmbaRTSL_HdmiSendAVMUTE - Enables/disables the HDMI Audio/Video Mute (AVMUTE) feature
 *  @param[in] EnableFlag Set the AVMUTE flag(1)/Clear the AVMUTE flag(0)
 */
void AmbaRTSL_HdmiSendAVMUTE(UINT32 EnableFlag)
{
    /* These flags will be transmitted between the active edge of VSYNC and 384 pixels following the edge */
    AmbaCSL_HdmiSetAVMUTE(EnableFlag);
}

/**
 *  AmbaRTSL_HdmiCecSetClkFreq - Set HDMI CEC base clock frequency
 */
void AmbaRTSL_HdmiCecSetClkFreq(void)
{
#if !(defined(CONFIG_LINUX) || defined(CONFIG_QNX))
    UINT32 CecInputClk;
    UINT32 ClkDivider;

    /* HDMI CEC Input Clock = AHB bus clock */
    CecInputClk = AmbaRTSL_PllGetAhbClk();

    /* Set prescaler ratio to produce 0.005 ms to be CEC base clock cycle period */
    /* CEC base clock freq = AHB clock freq / ClockDivider */
    ClkDivider = (CecInputClk / 200000U);
    AmbaCSL_HdmiCecSetClockDivider(ClkDivider);
#endif
}

/**
 *  AmbaRTSL_HdmiCecEnable - Enable HDMI CEC module
 */
void AmbaRTSL_HdmiCecEnable(void)
{
    /* Enable CEC clock */
    AmbaCSL_HdmiSetCecClkCtrl(1U);

    /* Reset CEC */
    AmbaCSL_HdmiCecReset();
    AmbaCSL_HdmiCecResetRelease();

    /* Set CEC base clock frequency */
    AmbaRTSL_HdmiCecSetClkFreq();

    /* Set CEC bit timing (0.005 ms per CEC base clock cycle) */
    AmbaCSL_HdmiCecSetBitTiming(HDMI_CEC_RX_START_BIT, 740U, 900U);   /* 3.7ms/0.005ms = 740, 4.5ms/0.005ms = 900 */
    AmbaCSL_HdmiCecSetBitTiming(HDMI_CEC_RX_DATA0_BIT, 300U, 480U);   /* 1.5ms/0.005ms = 300, 2.4ms/0.005ms = 480 */
    AmbaCSL_HdmiCecSetBitTiming(HDMI_CEC_RX_DATA1_BIT, 120U, 480U);   /* 0.6ms/0.005ms = 120, 2.4ms/0.005ms = 480 */
    AmbaCSL_HdmiCecSetBitTiming(HDMI_CEC_TX_START_BIT, 740U, 900U);
    AmbaCSL_HdmiCecSetBitTiming(HDMI_CEC_TX_DATA0_BIT, 300U, 480U);
    AmbaCSL_HdmiCecSetBitTiming(HDMI_CEC_TX_DATA1_BIT, 120U, 480U);

    /* Set CEC bit tolerance timing */
    AmbaCSL_HdmiCecSetBitToleTiming(40U, 70U, 40U, 70U);           /* 0.2ms/0.005ms = 40, 0.35ms/0.005ms = 70 */

    /* Enable interrupts for CEC */
    AmbaCSL_HdmiEnableCecTxDoneInt();
    AmbaCSL_HdmiEnableCecTxFailInt();
    AmbaCSL_HdmiEnableCecRxInt();
}

/**
 *  AmbaRTSL_HdmiCecDisable - Disable HDMI CEC module
 */
void AmbaRTSL_HdmiCecDisable(void)
{
    /* Disable CEC clock */
    AmbaCSL_HdmiSetCecClkCtrl(0U);
}

/**
 *  AmbaRTSL_HdmiCecSetLogicAddr - Set device logical address
 *  @param[in] LogicalAddress Logical address of the device
 */
void AmbaRTSL_HdmiCecSetLogicalAddr(UINT32 LogicalAddress)
{
    AmbaCSL_HdmiCecSetLogicalAddress((UINT8)LogicalAddress);
}

/**
 *  AmbaRTSL_HdmiCecTransmit - Transmit one CEC message
 *  @param[in] pMessage CEC message
 *  @param[in] MsgSize Size of the CEC message (in Bytes)
 *  @return error code
 */
UINT32 AmbaRTSL_HdmiCecTransmit(const UINT8 *pMessage, UINT32 MsgSize)
{
    UINT32 CecRxStatus;
    UINT32 RetVal = HDMI_ERR_NONE;

    AmbaCSL_HdmiClearCecTxInt();

    /* Ensure Singal Free Time (CEC line inactive duration) before frame transmission */
    /*  >= 7 data bit periods: Present Initiator wants to immediately send additional frame */
    /*  >= 5 data bit periods: New Initiator wants to send a frame */
    /*  >= 3 data bit periods: Previouse attempt to send frame unsuccessful */
    CecRxStatus = AmbaCSL_HdmiCecGetRxStatus();
    if ((CecRxStatus == HDMI_CEC_STATUS_RX_IDLE_5_7)  ||
        (CecRxStatus == HDMI_CEC_STATUS_RX_IDLE_7_10) ||
        (CecRxStatus == HDMI_CEC_STATUS_RX_IDLE_10)) {
        AmbaCSL_HdmiCecTx(pMessage, MsgSize);
        RetVal = HDMI_ERR_NONE;
    } else {
        RetVal = HDMI_ERR_CEC;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_HdmiCecReceive - Receive one CEC message
 *  @param[in] pMessage CEC message
 *  @param[out] pMsgSize Size of the CEC message (in Bytes)
 */
void AmbaRTSL_HdmiCecReceive(UINT8 *pMessage, UINT32 *pMsgSize)
{
    /* Receive Frame */
    AmbaCSL_HdmiCecRx(pMessage, pMsgSize);
}
