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

#include "AmbaTypes.h"
#include "AmbaWrap.h"

#include "AmbaCSL_SD.h"

#include "AmbaMisraFix.h"

#ifdef CONFIG_QNX
AMBA_RCT_REG_s *pAmbaRCT_Reg;

AMBA_SD_REG_s *pAmbaSD_Reg[AMBA_NUM_SD_CHANNEL];
#endif

/**
 *  AmbaCSL_SdSetHostBusWidth - Set data transfer bus width
 *  @param[in] pSdRe Pointer to SD host control register
 *  @param[in] BusWidth Bus width of SD/eMMC card
 */
void AmbaCSL_SdSetHostBusWidth(AMBA_SD_REG_s *pSdReg, UINT32 BusWidth)
{
    switch (BusWidth) {
    case 8:     /* 8-bit */
        pSdReg->MiscCtrl0.DataBitMode = 1U;
        pSdReg->MiscCtrl0.DataWidth   = 0U;
        break;

    case 4:     /* 4-bit */
        pSdReg->MiscCtrl0.DataBitMode = 0U;
        pSdReg->MiscCtrl0.DataWidth   = 1U;
        break;

    default:    /* 1-bit */
        pSdReg->MiscCtrl0.DataBitMode = 0U;
        pSdReg->MiscCtrl0.DataWidth   = 0U;
        break;
    }
}

/**
 *  AmbaCSL_SdSetBlkCtrl - Set block size and count to BlkCtrl register.
 *  @param[in] pSdReg pointer to the SD Registers
 *  @param[in] BlkSize block size value
 *  @param[in] BlkCnt block count value
 */
void AmbaCSL_SdSetBlkCtrl(AMBA_SD_REG_s *pSdReg, UINT32 BlkSize, UINT16 BlkCnt)
{
    AMBA_SD_BLOCK_CTRL_REG_s BlkCtrl;

    UINT16 Value = 0;
    BlkSize &= 0xfffU;

    (void)AmbaWrap_memcpy(&Value, &BlkSize, sizeof(Value));

    BlkCtrl.BlkSize     = Value;                       /* [11:0]: Transfer Block Size  */
    BlkCtrl.SdmaBufSize = AMBA_SD_SDMA_BUF_SIZE_512KB; /* [14:12]: SDMA Buffer Size */
    BlkCtrl.BlkCount    = BlkCnt;

    /* [15]: Transfer Block Size 12th bit (for 4KB Data Block Transfers) */
    if (((BlkSize & 0x1000U) >> 12U) != 0U) {
        BlkCtrl.BlkSize12thBit = 1U;
    } else {
        BlkCtrl.BlkSize12thBit = 0;
    }

    pSdReg->BlkCtrl = BlkCtrl;
}

/**
 *  AmbaCSL_SdSetBlkSize - Set transfer block size
 *  @param[in] pSdReg pointer to the SD Registers
 *  @param[in] Value block size value
 */
void AmbaCSL_SdSetBlkSize(AMBA_SD_REG_s *pSdReg, UINT32 Value)
{
    UINT16 BlkSize = 0;
    Value &= 0xfffU;

    (void)AmbaWrap_memcpy(&BlkSize, &Value, sizeof(BlkSize));
    pSdReg->BlkCtrl.BlkSize = BlkSize;                 /* [11:0]: Transfer Block Size  */

    /* [15]: Transfer Block Size 12th bit (for 4KB Data Block Transfers) */
    if (((Value & 0x1000U) >> 12U) != 0U) {
        pSdReg->BlkCtrl.BlkSize12thBit = 1U;
    } else {
        pSdReg->BlkCtrl.BlkSize12thBit = 0;
    }
}

/**
 *  AmbaCSL_SdClockEnable - SD clock control
 *  @param[in] pSdReg pointer to the SD Registers
 *  @param[in] Enable 1 - enable; 0 - disalbe
 */
void AmbaCSL_SdClockEnable(AMBA_SD_REG_s *pSdReg, UINT32 Enable)
{
    AMBA_SD_MISC_CTRL1_REG_s MiscCtrl1;
    UINT32 i;

    MiscCtrl1 = pSdReg->MiscCtrl1;

    if (Enable != 0U) {
        MiscCtrl1.InternalClkEnable = 1U;
        MiscCtrl1.SdClockEnable     = 1U;
    } else {
        MiscCtrl1.SdClockEnable     = 0;
    }

    pSdReg->MiscCtrl1 = MiscCtrl1;

    if (Enable != 0U) {
        /* Wait until clock becomes stable */
        for (i = 0; i < 0xFFFFU; i++) {
            if (pSdReg->MiscCtrl1.InternalClkStable != 0U) {
                break;
            }
        }
    }
}

/**
 *  AmbaCSL_SdEnableIrq - Enable all host controller interrupt status
 *  @param[in] pSdReg  - Pointer to SD host control register
 */
void AmbaCSL_SdEnableIrq(AMBA_SD_REG_s *pSdReg)
{
    pSdReg->IrqEnable.Data = 0;
    pSdReg->IrqSignalEnable.Data = 0;

    /* IRQ_ENABLE */
    pSdReg->IrqEnable.Bits.CardRemoval       = 1U;
    pSdReg->IrqEnable.Bits.CardInsertion     = 1U;
    pSdReg->IrqEnable.Bits.DmqIRQ            = 1U;
    pSdReg->IrqEnable.Bits.BlkGapEvent       = 1U;
    pSdReg->IrqEnable.Bits.TransferDone      = 1U;
    pSdReg->IrqEnable.Bits.CmdDone           = 1U;
    pSdReg->IrqEnable.Bits.CmdTimeoutError   = 1U;
    pSdReg->IrqEnable.Bits.CmdCrcError       = 1U;
    pSdReg->IrqEnable.Bits.CmdEndBitError    = 1U;
    pSdReg->IrqEnable.Bits.CmdIndexError     = 1U;
    pSdReg->IrqEnable.Bits.DataTimeoutError  = 1U;
    pSdReg->IrqEnable.Bits.DataCrcError      = 1U;
    pSdReg->IrqEnable.Bits.DataEndBitError   = 1U;
    pSdReg->IrqEnable.Bits.CurrentLimitError = 1U;
    pSdReg->IrqEnable.Bits.AutoCmd12Error    = 1U;
    pSdReg->IrqEnable.Bits.AdmaError         = 1U;

    /* IRQ_SIGNAL_ENABLE */
    pSdReg->IrqSignalEnable.Bits.CmdDone           = 1U;
    pSdReg->IrqSignalEnable.Bits.TransferDone      = 1U;
    pSdReg->IrqSignalEnable.Bits.BlkGapEvent       = 1U;
    pSdReg->IrqSignalEnable.Bits.DmqIRQ            = 1U;
    pSdReg->IrqSignalEnable.Bits.CardInsertion     = 1U;
    pSdReg->IrqSignalEnable.Bits.CardRemoval       = 1U;
    pSdReg->IrqSignalEnable.Bits.CmdTimeoutError   = 1U;
    pSdReg->IrqSignalEnable.Bits.CmdCrcError       = 1U;
    pSdReg->IrqSignalEnable.Bits.CmdEndBitError    = 1U;
    pSdReg->IrqSignalEnable.Bits.CmdIndexError     = 1U;
    pSdReg->IrqSignalEnable.Bits.DataTimeoutError  = 1U;
    pSdReg->IrqSignalEnable.Bits.DataCrcError      = 1U;
    pSdReg->IrqSignalEnable.Bits.DataEndBitError   = 1U;
    pSdReg->IrqSignalEnable.Bits.CurrentLimitError = 1U;
    pSdReg->IrqSignalEnable.Bits.AutoCmd12Error    = 1U;
    pSdReg->IrqSignalEnable.Bits.AdmaError         = 1U;
}

/**
 *  AmbaCSL_SdioEnableCardIrq - Enable all host controller interrupt status
 *  @param[in] pSdReg Pointer to SD host control register
 */
void AmbaCSL_SdioEnableCardIrq(AMBA_SD_REG_s *pSdReg, UINT32 Enable)
{
    if (Enable != 0U) {
        pSdReg->IrqEnable.Bits.CardIRQ       = 1U;
        pSdReg->IrqSignalEnable.Bits.CardIRQ = 1U;
    } else {
        pSdReg->IrqEnable.Bits.CardIRQ       = 0;
        pSdReg->IrqSignalEnable.Bits.CardIRQ = 0;
    }
}

/**
 *  AmbaCSL_SdResetCmdLine - Reset SD CMD Line and wait for its completion
 *  @param[in] pSdReg pointer to the SD Registers
 */
void AmbaCSL_SdResetCmdLine(AMBA_SD_REG_s *pSdReg)
{
    UINT32 i;
    pSdReg->MiscCtrl1.SoftResetCmdLine = 1U;

    /* wait for its completion */
    for (i = 0; i < 0xFFFFU; i++) {
        if (pSdReg->MiscCtrl1.SoftResetCmdLine == 0U) {
            break;
        }
    }
}

/**
 *  AmbaCSL_SdResetDataLine - Reset SD DAT Line and wait for its completion
 *  @param[in] pSdReg pointer to the SD Registers
 */
void AmbaCSL_SdResetDataLine(AMBA_SD_REG_s *pSdReg)
{
    UINT32 i;

    pSdReg->MiscCtrl1.SoftResetDataLine = 1U;

    /* wait for its completion */
    for (i = 0; i < 0xFFFFU; i++) {
        if (pSdReg->MiscCtrl1.SoftResetDataLine == 0U) {
            break;
        }
    }
}

/**
 *  AmbaCSL_SdResetAll - Reset host controller
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSdReg pointer to the SD Registers
 */
void AmbaCSL_SdResetAll(UINT32 ChannelNo, AMBA_SD_REG_s *pSdReg)
{
    UINT32 DelayCycle;
    UINT8 HiSpdEnable, EnableDDR;
    UINT32 i;

    DelayCycle = AmbaCSL_SDGetRdLatencyCtrl(ChannelNo);
    HiSpdEnable = AmbaCSL_SDGetHiSpdEnable(pSdReg);
    EnableDDR = AmbaCSL_SDGetEnableDDR(pSdReg);

    pSdReg->MiscCtrl1.SoftResetAll = 1U;

    /* Wait for reset to complete (busy wait!) */
    for (i = 0; i < 0xFFFFU; i++) {
        if (pSdReg->MiscCtrl1.SoftResetAll == 0U) {
            if (ChannelNo == AMBA_SD_CHANNEL2) {
                for (volatile UINT32 j = 0; j < 15000U; j++) {
                    /* Wait for BootStatus.Bits.ReadyToBoot */
                }
                if (pSdReg->BootStatus.Bits.ReadyToBoot == 0U) {
                    continue;
                }
            }
            break;
        }
    }

    if (DelayCycle != 0U) {
        AmbaCSL_SDSetRdLatencyCtrl(ChannelNo, 1U);
    } else {
        AmbaCSL_SDSetRdLatencyCtrl(ChannelNo, 0);
    }
    AmbaCSL_SDSetHiSpdEnable(pSdReg, HiSpdEnable);
    AmbaCSL_SDSetEnableDDR(pSdReg, EnableDDR);
}

/**
 *  AmbaCSL_SdWaitCmdLineReady - wait for SD CMD Line ready for use
 *  @param[in] pSdReg pointer to the SD Registers
 *  @return 1 - Ready; 0 - not Ready
 */
UINT32 AmbaCSL_SdWaitCmdLineReady(AMBA_SD_REG_s *pSdReg)
{
    INT32 i;
    UINT32 Rval;

    for (i = 0; i < 1000; i++) {
        if (pSdReg->PresentState.Bits.CmdLineInUse != 0U) {
            break;
        }
    }

    if (pSdReg->PresentState.Bits.CmdLineInUse != 0U) {
        AmbaCSL_SdResetCmdLine(pSdReg);     /* reset CMD Line */
        for (i = 0; i < 1000; i++) {
            if (pSdReg->PresentState.Bits.CmdLineInUse != 0U) {
                break;
            }
        }
    }

    if (pSdReg->PresentState.Bits.CmdLineInUse == 0U) {
        Rval = 1U;
    } else {
        Rval = 0;
    }

    return Rval;
}

/**
 *  AmbaCSL_SdWaitDataLineReady - wait for SD DAT Line ready for use
 *  @param[in] pSdReg pointer to the SD Registers
 *  @return 1 - Ready; 0 - not Ready
 */
UINT32 AmbaCSL_SdWaitDataLineReady(AMBA_SD_REG_s *pSdReg)
{
    UINT32 Rval;

    for (INT32 i = 0; i < 1000; i++) {
        if (pSdReg->PresentState.Bits.DataLineInUse != 0U) {
            break;
        }
    }

    if (pSdReg->PresentState.Bits.DataLineInUse != 0U) {
        AmbaCSL_SdResetDataLine(pSdReg);    /* reset DAT Line */
        for (INT32 i = 0; i < 1000; i++) {
            if (pSdReg->PresentState.Bits.DataLineInUse != 0U) {
                break;
            }
        }
    }

    if (pSdReg->PresentState.Bits.DataLineInUse == 0U) {
        Rval = 1U;
    } else {
        Rval = 0;
    }

    return Rval;
}

/**
 *  AmbaCSL_SdDisableIrqStatus - Disable SD host controller interrupt status
 *  @param[in] pSdReg pointer to the SD Registers
 */
void AmbaCSL_SdDisableIrqStatus(AMBA_SD_REG_s * pSdReg)
{
    pSdReg->IrqEnable.Data       = 0;
    pSdReg->IrqSignalEnable.Data = 0;
}

/**
 *  AmbaCSL_SdSetBlkSizeBlkCount - Set transfer block size and count
 *  @param[in] pSdReg pointer to the SD Registers
 *  @param[in] BlkSize block size
 *  @param[in] BlkCnt block count
 */
void AmbaCSL_SdSetBlkSizeBlkCount(AMBA_SD_REG_s *pSdReg, UINT32 BlkSize, UINT16 BlkCnt)
{
    if (BlkCnt > 1U) {
        AmbaCSL_SdBlkCountEnable(pSdReg);
        AmbaCSL_SdMultiBlkEnable(pSdReg);
        AmbaCSL_SdAutoCmd12Enable(pSdReg);
    } else {
        AmbaCSL_SdBlkCountDisable(pSdReg);
        AmbaCSL_SdMultiBlkDisable(pSdReg);
        AmbaCSL_SdAutoCmd12Disable(pSdReg);
    }

    AmbaCSL_SdSetBlkSize(pSdReg, BlkSize);
    AmbaCSL_SdSetBlkCount(pSdReg, BlkCnt);
}

/**
 *  AmbaCSL_SDSetRdLatencyCtrl -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] DelayCycle
 */
void AmbaCSL_SDSetRdLatencyCtrl(UINT32 ChannelNo, UINT32 DelayCycle)
{
#if 0
    DelayCycle &= 0xfU;
    pAmbaSD_Reg[ChannelNo]->RdLatencyCtrl.Data &= 0xffff0000U;
    pAmbaSD_Reg[ChannelNo]->RdLatencyCtrl.Data += (DelayCycle << 12U) + (DelayCycle << 8U) + (DelayCycle << 4U) + DelayCycle;
#else
    (void)ChannelNo;
    (void)DelayCycle;
#endif
}

/**
 *  AmbaCSL_SDGetRdLatencyCtrl -
 *  @param[in] SdChanNo SD Channel Number
 *  @return
 */
UINT32 AmbaCSL_SDGetRdLatencyCtrl(UINT32 ChannelNo)
{
#if 0
    return pAmbaSD_Reg[ChannelNo]->RdLatencyCtrl.Data;
#else
    (void)ChannelNo;
    return 0;
#endif
}

/**
 *  AmbaCSL_SdGetIrqStatus -
 *  @param[in] pSdReg pointer to the SD Registers
 *  @return
 */
UINT32 AmbaCSL_SdGetIrqStatus(const AMBA_SD_REG_s *pSdReg)
{
    UINT32 Temp;
    AMBA_SD_IRQ_STATUS_REG_s IrqStatus = pSdReg->IrqStatus;
    AmbaMisra_TypeCast32(&Temp, &IrqStatus);
    return Temp;
}

/**
 *  AmbaCSL_SdClearIrqStatus -
 *  @param[in] pSdReg pointer to the SD Registers
 *  @param[in] d
 */
void AmbaCSL_SdClearIrqStatus(AMBA_SD_REG_s *pSdReg, UINT32 d)
{
    AMBA_SD_IRQ_STATUS_REG_s IrqStatus;
    AmbaMisra_TypeCast32(&IrqStatus, &d);
    pSdReg->IrqStatus = IrqStatus;
}

/**
 *  AmbaCSL_SdSetTransferCtrl -
 *  @param[in] pSdReg pointer to the SD Registers
 *  @param[in] d
 */
void AmbaCSL_SdSetTransferCtrl(AMBA_SD_REG_s *pSdReg, UINT32 d)
{
    AMBA_SD_TRANSFER_CTRL_REG_s        TransferCtrl;
    AmbaMisra_TypeCast32(&TransferCtrl, &d);
    pSdReg->TransferCtrl = TransferCtrl;
}

/**
 *  AmbaCSL_RctSetSd0CtrlPhy0 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlPhy0(UINT32 Value)
{
    //AMBA_SD_PHY_CTRL0_REG_s SdPhyCtrl0;

    //AmbaMisra_TypeCast32(&SdPhyCtrl0, &Value);
    //pAmbaRCT_Reg->Sd0PhyCtrl0 = SdPhyCtrl0;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlPhy0 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlPhy0(UINT32 Value)
{
#if 0
    AMBA_SD_PHY_CTRL0_REG_s SdPhyCtrl0;

    AmbaMisra_TypeCast32(&SdPhyCtrl0, &Value);
    pAmbaRCT_Reg->Sd1PhyCtrl0 = SdPhyCtrl0;
#endif
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlDinClkPolarity -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlDinClkPolarity(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl0.SdDinClkPolarity = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlClkOutBypass -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlClkOutBypass(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl0.SdClkOutBypass = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlReset -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlReset(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl0.Reset = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlClk270Alone -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlClk270Alone(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl0.Clk270Alone = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlDelayChainSelect -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlDelayChainSelect(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl0.DelayChainSelect = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlSdDutySelect -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlSdDutySelect(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl0.SdDutySelect = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlRxClkPolarity -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlRxClkPolarity(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl0.RxClkPolarity = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlDataCmdBypass -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlDataCmdBypass(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl0.SdDataCmdBypass = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlDllBypass -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlDllBypass(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl0.SdDllBypass = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlEnableDll -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlEnableDll(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.EnableDll = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlCoarseDelayStep -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlCoarseDelayStep(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.CoarseDelayStep = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlLockRange -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlLockRange(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.LockRange = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlSelectInterClock -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlSelectInterClock(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.InternalClk = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlPDbb -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlPDbb(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.PDbb = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlBypassFilter -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlBypassFilter(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.BypassFilter = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlSelectFSM -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlSelectFSM(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.SelectFSM = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlForceLockVshift -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlForceLockVshift(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.ForceLockVshift = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlForceLockCycle -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlForceLockCycle(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.ForceLockCycle = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlPowerDownShift -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlPowerDownShift(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.PowerDownShift = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlEnableAutoCoarse -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlEnableAutoCoarse(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl2.EnableAutoCoarse = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlSelect -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlSelect(UINT32 Value)
{
#if 0
    AMBA_SD_PHY_CTRL1_REG_s SdPhyCtrl1;
    AmbaMisra_TypeCast32(&SdPhyCtrl1, &Value);
    pAmbaRCT_Reg->Sd0PhyCtrl1 = SdPhyCtrl1;
#endif
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlSelect -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlSelect(UINT32 Value)
{
#if 0
    AMBA_SD_PHY_CTRL1_REG_s SdPhyCtrl1;
    AmbaMisra_TypeCast32(&SdPhyCtrl1, &Value);
    pAmbaRCT_Reg->Sd1PhyCtrl1 = SdPhyCtrl1;
#endif
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlSelect0 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlSelect0(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect0 = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlSelect1 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlSelect1(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect1 = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlSelect2 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlSelect2(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect2 = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd0CtrlSelect3 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd0CtrlSelect3(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect3 = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctGetSd0CtrlDinClkPolarity -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlDinClkPolarity(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl0.SdDinClkPolarity;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlClkoutBypass -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlClkoutBypass(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl0.SdClkOutBypass;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlReset -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlReset(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl0.Reset;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlClk270Alone -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlClk270Alone(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl0.Clk270Alone;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlDelayChainSelect -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlDelayChainSelect(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl0.DelayChainSelect;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlSdDutySelect -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlSdDutySelect(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl0.SdDutySelect;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlRxClkPolarity -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlRxClkPolarity(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl0.RxClkPolarity;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlDataCmdBypass -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlDataCmdBypass(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl0.SdDataCmdBypass;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlDllBypass -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlDllBypass(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl0.SdDllBypass;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlSelect -
 *  @return
 */
UINT32 AmbaCSL_RctGetSd0CtrlSelect(void)
{
#if 0
    UINT32 Value;
    AMBA_SD_PHY_CTRL1_REG_s SdPhyCtrl1;

    SdPhyCtrl1 = pAmbaRCT_Reg->Sd0PhyCtrl1;
    AmbaMisra_TypeCast32(&Value, &SdPhyCtrl1);

    return Value;
#else
    return 0U;
#endif
}

/**
 *  AmbaCSL_RctGetSd0CtrlSelect0 -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlSelect0(void)
{
    //return (UINT8) pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect0;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlSelect1 -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlSelect1(void)
{
    //return (UINT8) pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect1;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlSelect2 -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlSelect2(void)
{
    //return (UINT8) pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect2;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlSelect3 -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlSelect3(void)
{
    //return (UINT8) pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect3;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlSharedBusCtrl -
 *  @return
 */
UINT32 AmbaCSL_RctGetSd0CtrlSharedBusCtrl(void)
{
#if 0
    UINT32 Value;
    AMBA_SD_PHY_CTRL2_REG_s SdPhyCtrl2;

    SdPhyCtrl2 = pAmbaRCT_Reg->Sd0PhyCtrl2;
    AmbaMisra_TypeCast32(&Value, &SdPhyCtrl2);

    return Value;
#else
    return 0U;
#endif
}

/**
 *  AmbaCSL_RctGetSd0CtrlEnableDll -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlEnableDll(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.EnableDll;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlCoarseDelayStep -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlCoarseDelayStep(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.CoarseDelayStep;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlLockRange -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlLockRange(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.LockRange;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlSelectInterClock -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlSelectInterClock(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.InternalClk;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlPDbb -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlPDbb(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.PDbb;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlBypassFilter -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlBypassFilter(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.BypassFilter;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlSelectFSM -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlSelectFSM(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.SelectFSM;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlForceLockVshift -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlForceLockVshift(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.ForceLockVshift;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlForceLockCycle -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlForceLockCycle(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.ForceLockCycle;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlPowerDownShift -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlPowerDownShift(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.PowerDownShift;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd0CtrlEnableAutoCoarse -
 *  @return
 */
UINT8 AmbaCSL_RctGetSd0CtrlEnableAutoCoarse(void)
{
    //return pAmbaRCT_Reg->Sd0PhyCtrl2.EnableAutoCoarse;
    return 0U;
}

/**
 *  AmbaCSL_RctSetSd2CtrlDinClkPolarity -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlDinClkPolarity(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl0.SdDinClkPolarity = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlClkOutBypass -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlClkOutBypass(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl0.SdClkOutBypass  = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlReset -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlReset(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl0.Reset = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlClk270Alone -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlClk270Alone(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl0.Clk270Alone = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlDelayChainSelect -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlDelayChainSelect(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl0.DelayChainSelect = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlSdDutySelect -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlSdDutySelect(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl0.SdDutySelect = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlRxClkPolarity -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlRxClkPolarity(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl0.RxClkPolarity = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlDataCmdBypass -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlDataCmdBypass(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl0.SdDataCmdBypass = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlDllBypass -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlDllBypass(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl0.SdDllBypass = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlSelect0 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlSelect0(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect0 = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlSelect1 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlSelect1(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect1 = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlSelect2 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlSelect2(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect2 = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlSelect3 -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlSelect3(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect3 = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlEnableDll -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlEnableDll(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.EnableDll = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlCoarseDelayStep -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlCoarseDelayStep(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.CoarseDelayStep = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlLockRange -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlLockRange(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.LockRange = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlSelectInterClock -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlSelectInterClock(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.InternalClk = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlPDbb -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlPDbb(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.PDbb = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlBypassFilter -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlBypassFilter(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.BypassFilter = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlSelectFSM -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlSelectFSM(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.SelectFSM = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlForceLockVshift -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlForceLockVshift(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.ForceLockVshift = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlForceLockCycle -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlForceLockCycle(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.ForceLockCycle = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlPowerDownShift -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlPowerDownShift(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.PowerDownShift = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctSetSd2CtrlEnableAutoCoarse -
 *  @param[in] Value
 */
void AmbaCSL_RctSetSd2CtrlEnableAutoCoarse(UINT8 Value)
{
    //pAmbaRCT_Reg->Sd1PhyCtrl2.EnableAutoCoarse = Value;
    (void)Value;
}

/**
 *  AmbaCSL_RctGetSd2CtrlDinClkPolarity -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlDinClkPolarity(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl0.SdDinClkPolarity;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlClkoutBypass -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlClkoutBypass(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl0.SdClkOutBypass;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlReset -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlReset(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl0.Reset;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlClk270Alone -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlClk270Alone(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl0.Clk270Alone;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlDelayChainSelect -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlDelayChainSelect(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl0.DelayChainSelect;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlSdDutySelect -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlSdDutySelect(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl0.SdDutySelect;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlRxClkPolarity -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlRxClkPolarity(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl0.RxClkPolarity;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlDataCmdBypass -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlDataCmdBypass(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl0.SdDataCmdBypass;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlDllBypass -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlDllBypass(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl0.SdDllBypass;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlSharedBusCtrl -
 *  @erturn
 */
UINT32 AmbaCSL_RctGetSd2CtrlSharedBusCtrl(void)
{
#if 0
    UINT32 Value;
    AMBA_SD_PHY_CTRL2_REG_s SdPhyCtrl2;

    SdPhyCtrl2 = pAmbaRCT_Reg->Sd1PhyCtrl2;
    AmbaMisra_TypeCast32(&Value, &SdPhyCtrl2);

    return Value;
#else
    return 0U;
#endif
}

/**
 *  AmbaCSL_RctGetSd2CtrlEnableDll -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlEnableDll(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.EnableDll;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlCoarseDelayStep -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlCoarseDelayStep(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.CoarseDelayStep;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlLockRange -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlLockRange(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.LockRange;'
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlSelectInterClock -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlSelectInterClock(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.InternalClk;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlPDbb -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlPDbb(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.PDbb;
    return 0U;
}

UINT8 AmbaCSL_RctGetSd2CtrlBypassFilter(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.BypassFilter;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlSelectFSM -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlSelectFSM(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.SelectFSM;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlForceLockVshift -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlForceLockVshift(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.ForceLockVshift;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlForceLockCycle -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlForceLockCycle(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.ForceLockCycle;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlPowerDownShift -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlPowerDownShift(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.PowerDownShift;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlEnableAutoCoarse -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlEnableAutoCoarse(void)
{
    //return pAmbaRCT_Reg->Sd1PhyCtrl2.EnableAutoCoarse;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlSelect -
 *  @erturn
 */
UINT32 AmbaCSL_RctGetSd2CtrlSelect(void)
{
#if 0
    UINT32 Value;
    AMBA_SD_PHY_CTRL1_REG_s SdPhyCtrl1;

    SdPhyCtrl1 = pAmbaRCT_Reg->Sd1PhyCtrl1;
    AmbaMisra_TypeCast32(&Value, &SdPhyCtrl1);

    return Value;
#else
    return 0U;
#endif
}

/**
 *  AmbaCSL_RctGetSd2CtrlSelect0 -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlSelect0(void)
{
    //return (UINT8) pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect0;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlSelect1 -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlSelect1(void)
{
    //return (UINT8) pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect1;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlSelect2 -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlSelect2(void)
{
    //return (UINT8) pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect2;
    return 0U;
}

/**
 *  AmbaCSL_RctGetSd2CtrlSelect3 -
 *  @erturn
 */
UINT8 AmbaCSL_RctGetSd2CtrlSelect3(void)
{
    //return (UINT8) pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect3;
    return 0U;
}
