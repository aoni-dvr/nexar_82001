// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 * @file AmbaCSL_HDMI.c
 *
 * @copyright Copyright (c) 2021 Ambarella International LP
 *
 * @details HDMI CSL Device Driver
 *
 */

#include "AmbaTypes.h"
#ifndef CONFIG_LINUX
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#else
#include "osal.h"
#define AmbaWrap_memcpy OSAL_memcpy
#define AmbaMisra_TypeCast32(a, b)  OSAL_memcpy(a, b, 4)
#define AmbaMisra_TypeCast(a, b)    OSAL_memcpy(a, b, sizeof(void *))
#endif
#include "AmbaCSL_HDMI.h"

#if !defined(CONFIG_LINUX)
AMBA_HDMI_REG_s *pAmbaHDMI_Reg;
#endif
/**
 *  PackUINT32 - Combine four bytes into a word
 *  @param[in] Byte0 The 1st byte
 *  @param[in] Byte1 The 2nd byte
 *  @param[in] Byte2 The 3rd byte
 *  @param[in] Byte3 The 4th byte
 *  @return The combined word
 */
static UINT32 PackUINT32(UINT8 Byte0, UINT8 Byte1, UINT8 Byte2, UINT8 Byte3)
{
    return (((UINT32)(Byte3) << 24U) | ((UINT32)(Byte2) << 16U) | ((UINT32)(Byte1) << 8U)  | ((UINT32)(Byte0)));
}

/**
 *  HDMI_GetPacketTxCtrlAddr - Get DI packets transmitter control register address
 *  @return NULL or a pointer to the register address
 */
static UINT32 * HDMI_GetPacketTxCtrlAddr(void)
{
    const volatile AMBA_HDMI_DI_PACKET_TX_CTRL_REG_s *pPacketTxCtrlReg;
    UINT32 *pRegAddr = NULL;

    pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketTxCtrl;

    AmbaMisra_TypeCast(&pRegAddr, &pPacketTxCtrlReg);

    return pRegAddr;
}

/**
 *  HDMI_GetInfoFramePacketCtrlRegsAddr - Get InfoFrame packet control register address
 *  @param[in] PktType InfoFrame packet type
 *  @param[out] pBitOffset Bit offset of DI packets transmitter control register
 *  @return NULL or a pointer to the register address
 */
static UINT32 * HDMI_GetInfoFramePacketCtrlRegsAddr(UINT32 PktType, UINT32 *pBitOffset)
{
    const volatile AMBA_HDMI_DATA_ISLAND_PACKET_REG_s *pPacketTxCtrlReg = NULL;
    UINT32 *pRegAddr = NULL;

    if (PktType == HDMI_INFOFRAME_VENDOR_SPECIFIC) {
        pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketInfoFrameVendor;
        *pBitOffset = 16U;
    } else if (PktType == HDMI_INFOFRAME_AVI) {
        pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketInfoFrameAVI;
        *pBitOffset = 6U;
    } else if (PktType == HDMI_INFOFRAME_SPD) {
        pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketInfoFrameSPD;
        *pBitOffset = 8U;
    } else if (PktType == HDMI_INFOFRAME_AUDIO) {
        pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketInfoFrameAudio;
        *pBitOffset = 10U;
    } else if (PktType == HDMI_INFOFRAME_MPEG_SOURCE) {
        pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketInfoFrameMPEG;
        *pBitOffset = 12U;
    } else {
        *pBitOffset = 0xffU;
    }

    AmbaMisra_TypeCast(&pRegAddr, &pPacketTxCtrlReg);

    return pRegAddr;
}

/**
 *  HDMI_GetPacketCtrlRegsAddr - Get DI packet control register address
 *  @param[in] PktType DI packet type
 *  @param[out] pBitOffset Bit offset of DI packets transmitter control register
 *  @return NULL or a pointer to the register address
 */
static UINT32 * HDMI_GetPacketCtrlRegsAddr(UINT32 PktType, UINT32 *pBitOffset)
{
    const volatile AMBA_HDMI_DATA_ISLAND_PACKET_REG_s *pPacketTxCtrlReg = NULL;
    UINT32 *pRegAddr = NULL;

    if (PktType == HDMI_PACKET_GENERAL_CTRL) {
        pPacketTxCtrlReg = NULL;
        *pBitOffset = 0U;
    } else if (PktType == HDMI_PACKET_ACP) {
        pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketACP;
        *pBitOffset = 2U;
    } else if (PktType == HDMI_PACKET_ISRC1) {
        pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketISRC1;
        *pBitOffset = 4U;
    } else if (PktType == HDMI_PACKET_ISRC2) {
        pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketISRC2;
        *pBitOffset = 4U;
    } else if (PktType == HDMI_PACKET_GAMUT_METADATA) {
        pPacketTxCtrlReg = &pAmbaHDMI_Reg->PacketGamut;
        *pBitOffset = 14U;
    } else {
        *pBitOffset = 0xffU;
    }

    AmbaMisra_TypeCast(&pRegAddr, &pPacketTxCtrlReg);

    return pRegAddr;
}

/**
 *  AmbaCSL_HdmiSetAudioChanStat - Set IEC 60958 channel status
 *  @param[in] pChanStatus pointer to IEC 60958 channel status block
 */
void AmbaCSL_HdmiSetAudioChanStat(const UINT32 *pChanStatus)
{
    pAmbaHDMI_Reg->AudioChannelStatus[0U] = pChanStatus[0U];
    pAmbaHDMI_Reg->AudioChannelStatus[1U] = pChanStatus[1U];
    pAmbaHDMI_Reg->AudioChannelStatus[2U] = pChanStatus[2U];
    pAmbaHDMI_Reg->AudioChannelStatus[3U] = pChanStatus[3U];
    pAmbaHDMI_Reg->AudioChannelStatus[4U] = pChanStatus[4U];
    pAmbaHDMI_Reg->AudioChannelStatus[5U] = pChanStatus[5U];
}

/**
 *  AmbaCSL_HdmiSetAudioRegenCtrl - Enable Audio Clock Regeneration packet
 *  @param[in] EnableFlag Enable flag. 1 = Enable; 0 = Disable.
 *  @param[in] N The value of audio clock regeneration "N"
 *  @param[in] CTS Cycle Time Stamp
 */
void AmbaCSL_HdmiSetAudioRegenCtrl(UINT32 EnableFlag, UINT32 N, UINT32 CTS)
{
    if (EnableFlag != 0U) {
        pAmbaHDMI_Reg->AudioClkRegenN.Value = N;

        if (CTS >= HDMI_CTS_AUTO_GEN) {
            pAmbaHDMI_Reg->AudioClkRegenCtrl.ProgrammableCTS = 0U;
        } else {
            pAmbaHDMI_Reg->AudioClkRegenCtrl.ProgrammableCTS = 1U;  /* Make CTS value be programmable */
            pAmbaHDMI_Reg->AudioClkRegenCTS.Value = CTS;
        }
    }

    pAmbaHDMI_Reg->AudioClkRegenCtrl.PacketEnable = (UINT8)EnableFlag;
}

/**
 *  AmbaCSL_HdmiSetAVMUTE - Set the AVMUTE flag
 *  @param[in] EnableFlag Enable flag. 1 = Set AVMUTE flag; 0 = Clear AVMUTE flag.
 */
void AmbaCSL_HdmiSetAVMUTE(UINT32 EnableFlag)
{
    if (EnableFlag == (UINT32)0x1U) {
        pAmbaHDMI_Reg->GeneralCtrlPacket.SetAVMUTE = 1U;
        pAmbaHDMI_Reg->GeneralCtrlPacket.ClearAVMUTE = 0U;
    } else {
        pAmbaHDMI_Reg->GeneralCtrlPacket.SetAVMUTE = 0U;
        pAmbaHDMI_Reg->GeneralCtrlPacket.ClearAVMUTE = 1U;
    }

    pAmbaHDMI_Reg->PacketTxCtrl.SendGeneralCtrlPacket = 1U;
    pAmbaHDMI_Reg->PacketTxCtrl.RepeatGeneralCtrlPacket = 1U;
}

/**
 *  AmbaCSL_HdmiPacketCtrl - Transmission control of Data Island packet
 *  @param[in] pPacket A pointer to Packet header and Packet body (ACP, ISRC1/ISRC2, Gamut and InfoFrames)
 *  @param[in] EnableFlag Enable flag. 1 = Enable packet transmission; 0 = Disable packet transmission.
 */
void AmbaCSL_HdmiPacketCtrl(const AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, UINT32 EnableFlag)
{
    volatile UINT32 *pPacketRegAddr;
    UINT32 PktType, Temp;
    UINT32 RegBitOffset;
    volatile UINT32 *pPacketTxCtrlReg = HDMI_GetPacketTxCtrlAddr();

    PktType = pPacket->Header & 0x7fU;
    if ((pPacket->Header & 0x80U) != 0x0U) {
        pPacketRegAddr = HDMI_GetInfoFramePacketCtrlRegsAddr(PktType, &RegBitOffset);
    } else {
        pPacketRegAddr = HDMI_GetPacketCtrlRegsAddr(PktType, &RegBitOffset);
    }

    if (pPacketRegAddr != NULL) {
        pPacketRegAddr[0] = PackUINT32((UINT8)((pPacket->Header) & 0xffU),          (UINT8)((pPacket->Header >> 8U) & 0xffU),   (UINT8)((pPacket->Header >> 16U) & 0xffU),  0x00U);
        pPacketRegAddr[1] = PackUINT32((UINT8)((pPacket->Header >> 24U) & 0xffU),   pPacket->Payload[0],                        pPacket->Payload[1],                        pPacket->Payload[2]);
        pPacketRegAddr[2] = PackUINT32(pPacket->Payload[3],                         pPacket->Payload[4],                        pPacket->Payload[5],                        0x00U);
        pPacketRegAddr[3] = PackUINT32(pPacket->Payload[6],                         pPacket->Payload[7],                        pPacket->Payload[8],                        pPacket->Payload[9]);
        pPacketRegAddr[4] = PackUINT32(pPacket->Payload[10],                        pPacket->Payload[11],                       pPacket->Payload[12],                       0x00U);
        pPacketRegAddr[5] = PackUINT32(pPacket->Payload[13],                        pPacket->Payload[14],                       pPacket->Payload[15],                       pPacket->Payload[16]);
        pPacketRegAddr[6] = PackUINT32(pPacket->Payload[17],                        pPacket->Payload[18],                       pPacket->Payload[19],                       0x00U);
        pPacketRegAddr[7] = PackUINT32(pPacket->Payload[20],                        pPacket->Payload[21],                       pPacket->Payload[22],                       pPacket->Payload[23]);
        pPacketRegAddr[8] = PackUINT32(pPacket->Payload[24],                        pPacket->Payload[25],                       pPacket->Payload[26],                       0x00U);
    }

    if (RegBitOffset < 32U) {
        /* Enable/Disable Packet Transmission */
        Temp = *pPacketTxCtrlReg;
        Temp &= ~((UINT32)0x3U << RegBitOffset);
        Temp |= (EnableFlag << RegBitOffset);
        *pPacketTxCtrlReg = Temp;
    }
}

/**
 *  AmbaCSL_HdmiCecSetBitTiming - Set Bit Timing of CEC line
 *  @param[in] BitType Bit symbol timing to be configured
 *  @param[in] LowLevelPeriod Low level period of the bit symbol
 *  @param[in] WholePeriod Whole period of the bit symbol
 */
void AmbaCSL_HdmiCecSetBitTiming(UINT32 BitType, UINT32 LowLevelPeriod, UINT32 WholePeriod)
{
    switch (BitType) {
    case HDMI_CEC_RX_START_BIT:
        pAmbaHDMI_Reg->CecRxStartBitPtrn.Period0 = (UINT16)LowLevelPeriod;
        pAmbaHDMI_Reg->CecRxStartBitPtrn.PeriodAll = (UINT16)WholePeriod;
        break;
    case HDMI_CEC_RX_DATA0_BIT:
        pAmbaHDMI_Reg->CecRxZeroBitPtrn.Period0 = (UINT16)LowLevelPeriod;
        pAmbaHDMI_Reg->CecRxZeroBitPtrn.PeriodAll = (UINT16)WholePeriod;
        break;
    case HDMI_CEC_RX_DATA1_BIT:
        pAmbaHDMI_Reg->CecRxOneBitPtrn.Period0 = (UINT16)LowLevelPeriod;
        pAmbaHDMI_Reg->CecRxOneBitPtrn.PeriodAll = (UINT16)WholePeriod;
        break;
    case HDMI_CEC_TX_START_BIT:
        pAmbaHDMI_Reg->CecTxStartBitPtrn.Period0 = (UINT16)LowLevelPeriod;
        pAmbaHDMI_Reg->CecTxStartBitPtrn.PeriodAll = (UINT16)WholePeriod;
        break;
    case HDMI_CEC_TX_DATA0_BIT:
        pAmbaHDMI_Reg->CecTxZeroBitPtrn.Period0 = (UINT16)LowLevelPeriod;
        pAmbaHDMI_Reg->CecTxZeroBitPtrn.PeriodAll = (UINT16)WholePeriod;
        break;
    case HDMI_CEC_TX_DATA1_BIT:
        pAmbaHDMI_Reg->CecTxOneBitPtrn.Period0 = (UINT16)LowLevelPeriod;
        pAmbaHDMI_Reg->CecTxOneBitPtrn.PeriodAll = (UINT16)WholePeriod;
        break;
    default:
        /* else */
        break;
    }
}

/**
 *  AmbaCSL_HdmiCecSetBitToleTiming - Set Bit Tolerance Timing of CEC line
 *  @param[in] LowerBound200us Tolerance timing of lower boundary for 0.2ms
 *  @param[in] LowerBound350us Tolerance timing of lower boundary for 0.35ms
 *  @param[in] UpperBound200us Tolerance timing of upper boundary for 0.2ms
 *  @param[in] UpperBound350us Tolerance timing of upper boundary for 0.35ms
 */
void AmbaCSL_HdmiCecSetBitToleTiming(UINT32 LowerBound200us, UINT32 LowerBound350us,
                                     UINT32 UpperBound200us, UINT32 UpperBound350us)
{
    pAmbaHDMI_Reg->CecToleLowerBoundary.Tolerance200us = (UINT16)LowerBound200us;
    pAmbaHDMI_Reg->CecToleLowerBoundary.Tolerance350us = (UINT16)LowerBound350us;
    pAmbaHDMI_Reg->CecToleUpperBoundary.Tolerance200us = (UINT16)UpperBound200us;
    pAmbaHDMI_Reg->CecToleUpperBoundary.Tolerance350us = (UINT16)UpperBound350us;
}

/**
 *  AmbaCSL_HdmiCecTx - CEC low level Tx
 *  @param[in] pMessage A pointer to a CEC message
 *  @param[in] MsgSize Size of the CEC message (in Bytes)
 */
void AmbaCSL_HdmiCecTx(const UINT8 *pMessage, UINT32 MsgSize)
{
    AMBA_CEC_DATA_REG_s CecTxData = {0U};
    UINT32 Temp;

    CecTxData.Block0 = pMessage[0];    /* Header Block */
    CecTxData.Block1 = pMessage[1];    /* Opcode Block */
    CecTxData.Block2 = pMessage[2];    /* Operand Block 1 */
    CecTxData.Block3 = pMessage[3];    /* Operand Block 2 */
    if (AmbaWrap_memcpy(&Temp, &CecTxData, sizeof(AMBA_CEC_DATA_REG_s)) == ERR_NONE) {
        pAmbaHDMI_Reg->CecTxData0 = Temp;
    }
    CecTxData.Block0 = pMessage[4];    /* Operand Block 3 */
    CecTxData.Block1 = pMessage[5];    /* Operand Block 4 */
    CecTxData.Block2 = pMessage[6];    /* Operand Block 5 */
    CecTxData.Block3 = pMessage[7];    /* Operand Block 6 */
    if (AmbaWrap_memcpy(&Temp, &CecTxData, sizeof(AMBA_CEC_DATA_REG_s)) == ERR_NONE) {
        pAmbaHDMI_Reg->CecTxData4 = Temp;
    }
    CecTxData.Block0 = pMessage[8];    /* Operand Block 7 */
    CecTxData.Block1 = pMessage[9];    /* Operand Block 8 */
    CecTxData.Block2 = pMessage[10];    /* Operand Block 9 */
    CecTxData.Block3 = pMessage[11];    /* Operand Block 10 */
    if (AmbaWrap_memcpy(&Temp, &CecTxData, sizeof(AMBA_CEC_DATA_REG_s)) == ERR_NONE) {
        pAmbaHDMI_Reg->CecTxData8 = Temp;
    }
    CecTxData.Block0 = pMessage[12];    /* Operand Block 11 */
    CecTxData.Block1 = pMessage[13];    /* Operand Block 12 */
    CecTxData.Block2 = pMessage[14];    /* Operand Block 13 */
    CecTxData.Block3 = pMessage[15];    /* Operand Block 14 */
    if (AmbaWrap_memcpy(&Temp, &CecTxData, sizeof(AMBA_CEC_DATA_REG_s)) == ERR_NONE) {
        pAmbaHDMI_Reg->CecTxData12 = Temp;
    }
    pAmbaHDMI_Reg->CecCtrl.TxBlockNumMinus1 = (UINT8)(MsgSize - 1U);    /* Total number of header block and valid data blocks (opcode+operands) */
    pAmbaHDMI_Reg->CecCtrl.EnableTx = 1U;                               /* Start frame transmission */
}

/**
 *  AmbaCSL_HdmiCecRx - CEC low level Rx
 *  @param[in] pMessage A pointer to a CEC message
 *  @param[out] pMsgSize Size of the CEC message (in Bytes)
 */
void AmbaCSL_HdmiCecRx(UINT8 *pMessage, UINT32 *pMsgSize)
{
    AMBA_CEC_DATA_REG_s CecRxData;
    UINT32 Temp;

    Temp = pAmbaHDMI_Reg->CecRxData0;
    if (AmbaWrap_memcpy(&CecRxData, &Temp, sizeof(AMBA_CEC_DATA_REG_s)) == ERR_NONE) {

        pMessage[0] = (UINT8)CecRxData.Block0;      /* Header Block */
        pMessage[1] = (UINT8)CecRxData.Block1;      /* Opcode Block */
        pMessage[2] = (UINT8)CecRxData.Block2;      /* Operand Block 1 */
        pMessage[3] = (UINT8)CecRxData.Block3;      /* Operand Block 2 */
    }

    Temp = pAmbaHDMI_Reg->CecRxData4;
    if (AmbaWrap_memcpy(&CecRxData, &Temp, sizeof(AMBA_CEC_DATA_REG_s)) == ERR_NONE) {
        pMessage[4] = (UINT8)CecRxData.Block0;      /* Operand Block 3 */
        pMessage[5] = (UINT8)CecRxData.Block1;      /* Operand Block 4 */
        pMessage[6] = (UINT8)CecRxData.Block2;      /* Operand Block 5 */
        pMessage[7] = (UINT8)CecRxData.Block3;      /* Operand Block 6 */
    }

    Temp = pAmbaHDMI_Reg->CecRxData8;
    if (AmbaWrap_memcpy(&CecRxData, &Temp, sizeof(AMBA_CEC_DATA_REG_s)) == ERR_NONE) {
        pMessage[8] = (UINT8)CecRxData.Block0;      /* Operand Block 7 */
        pMessage[9] = (UINT8)CecRxData.Block1;      /* Operand Block 8 */
        pMessage[10] = (UINT8)CecRxData.Block2;     /* Operand Block 9 */
        pMessage[11] = (UINT8)CecRxData.Block3;     /* Operand Block 10 */
    }

    Temp = pAmbaHDMI_Reg->CecRxData12;
    if (AmbaWrap_memcpy(&CecRxData, &Temp, sizeof(AMBA_CEC_DATA_REG_s)) == ERR_NONE) {
        pMessage[12] = (UINT8)CecRxData.Block0;     /* Operand Block 11 */
        pMessage[13] = (UINT8)CecRxData.Block1;     /* Operand Block 12 */
        pMessage[14] = (UINT8)CecRxData.Block2;     /* Operand Block 13 */
        pMessage[15] = (UINT8)CecRxData.Block3;     /* Operand Block 14 */
    }

    *pMsgSize = (UINT32)((UINT32)pAmbaHDMI_Reg->CecCtrl.RxBlockNumMinus1 + (UINT32)1U);
}
