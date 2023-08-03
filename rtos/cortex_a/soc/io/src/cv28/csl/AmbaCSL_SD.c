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

//extern AMBA_RCT_REG_s *const pAmbaRCT_Reg;
/*
 *  @RoutineName:: AmbaCSL_SdSetHostBusWidth
 *
 *  @Description:: Set data transfer bus width.
 *
 *  @Input      ::
 *      pSdReg: Pointer to SD host control register
 *      Width:  Bus width of SD/eMMC card
 *
 *  @Output     :: none
 *
 *  @Return     :: none
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

/*
 *  @RoutineName:: AmbaCSL_SdSetBlkSize
 *
 *  @Description:: Set transfer block size
 *
 *  @Input      ::
 *      pSdReg: pointer to the SD Registers
 *      Value:  block size value
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_SdSetBlkSize(AMBA_SD_REG_s *pSdReg, UINT32 Value)
{
    UINT16 BlkSize = 0;
    UINT32 ValueTmp = Value;
    ValueTmp &= 0xfffU;

    if (0U != AmbaWrap_memcpy(&BlkSize, &ValueTmp, sizeof(BlkSize))) { /* Misrac */ };
    pSdReg->BlkCtrl.BlkSize = BlkSize;                 /* [11:0]: Transfer Block Size  */

    /* [15]: Transfer Block Size 12th bit (for 4KB Data Block Transfers) */
    if (((ValueTmp & 0x1000U) >> 12U) != 0U) {
        pSdReg->BlkCtrl.BlkSize12thBit = 1U;
    } else {
        pSdReg->BlkCtrl.BlkSize12thBit = 0;
    }
}

/*
 *  @RoutineName:: AmbaCSL_SdClockEnable
 *
 *  @Description:: SD clock control
 *
 *  @Input      ::
 *      pSdReg: pointer to the SD Registers
 *      Enable: 1 - enable; 0 - disalbe
 *
 *  @Output     :: none
 *
 *  @Return     :: none
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

/*
 *  @RoutineName:: AmbaCSL_SdEnableIrq
 *
 *  @Description:: Enable all host controller interrupt status.
 *
 *  @Input      ::
 *                 pSdReg  - Pointer to SD host control register.
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
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

/*
 *  @RoutineName:: AmbaCSL_SdioEnableCardIrq
 *
 *  @Description:: Enable all host controller interrupt status.
 *
 *  @Input      ::
 *                 pSdReg  - Pointer to SD host control register.
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
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

/*
 *  @RoutineName:: AmbaCSL_SdResetCmdLine
 *
 *  @Description:: Reset SD CMD Line and wait for its completion
 *
 *  @Input      ::
 *      pSdReg: pointer to the SD Registers
 *
 *  @Output     :: none
 *
 *  @Return     :: none
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

/*
 *  @RoutineName:: AmbaCSL_SdResetDataLine
 *
 *  @Description:: Reset SD DAT Line and wait for its completion
 *
 *  @Input      ::
 *      pSdReg: pointer to the SD Registers
 *
 *  @Output     :: none
 *
 *  @Return     :: none
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

/*
 *  @RoutineName:: AmbaCSL_SdResetAll
 *
 *  @Description:: Reset host controller.
 *
 *  @Input      ::
 *                 pSdReg  - Pointer to SD host control register.
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
 */
void AmbaCSL_SdResetAll(UINT32 ChannelNo, AMBA_SD_REG_s *pSdReg)
{
    UINT8 HiSpdEnable, EnableDDR;
    UINT32 i;
    (void)ChannelNo;

    HiSpdEnable = AmbaCSL_SDGetHiSpdEnable(pSdReg);
    EnableDDR = AmbaCSL_SDGetEnableDDR(pSdReg);

    pSdReg->MiscCtrl1.SoftResetAll = 1U;

    /* Wait for reset to complete (busy wait!) */
    for (i = 0; i < 0xFFFFU; i++) {
        if (pSdReg->MiscCtrl1.SoftResetAll == 0U) {
            break;
        }
    }

    AmbaCSL_SDSetHiSpdEnable(pSdReg, HiSpdEnable);
    AmbaCSL_SDSetEnableDDR(pSdReg, EnableDDR);
}

/*
 *  @RoutineName:: AmbaCSL_SdWaitCmdLineReady
 *
 *  @Description:: wait for SD CMD Line ready for use
 *
 *  @Input      ::
 *      pSdReg: pointer to the SD Registers
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32     : 1 - Ready; 0 - not Ready
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
/*
 *  @RoutineName:: AmbaCSL_SdWaitDataLineReady
 *
 *  @Description:: wait for SD DAT Line ready for use
 *
 *  @Input      ::
 *      pSdReg: pointer to the SD Registers
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32     : 1 - Ready; 0 - not Ready
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

/*
 *  @RoutineName:: AmbaCSL_SdDisableIrqStatus
 *
 *  @Description:: Disable SD host controller interrupt status.
 *
 *  @Input      ::
 *      pSdReg: pointer to the SD Registers
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_SdDisableIrqStatus(AMBA_SD_REG_s * pSdReg)
{
    pSdReg->IrqEnable.Data       = 0;
    pSdReg->IrqSignalEnable.Data = 0;
}

/*
 *  @RoutineName:: AmbaCSL_SdSetBlkSizeBlkCount
 *
 *  @Description:: Set transfer block size and count.
 *
 *  @Input      ::
 *      pSdReg:   Pointer to SD host control register
 *      BlkSize:  block size
 *      BlkCount: block count
 *
 *  @Output     :: none
 *
 *  @Return     :: none
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
void AmbaCSL_SDSetRdLatencyCtrl(UINT32 ChannelNo, UINT32 DelayCycle)
{
    UINT32 DelayCycleTmp = DelayCycle;

    DelayCycleTmp &= 0xfU;
    pAmbaSD_Reg[ChannelNo]->RdLatencyCtrl.Data &= 0xffff0000U;
    pAmbaSD_Reg[ChannelNo]->RdLatencyCtrl.Data += (DelayCycleTmp << 12U) + (DelayCycleTmp << 8U) + (DelayCycleTmp << 4U) + DelayCycleTmp;
    /* the cycle count between command and response (ncrg + 1)cycles. Always set NCR=2 */
    pAmbaSD_Reg[ChannelNo]->RdLatencyCtrl.Bits.NcrReg = 2U;
}
UINT32 AmbaCSL_SDGetRdLatencyCtrl(UINT32 ChannelNo)
{
    return pAmbaSD_Reg[ChannelNo]->RdLatencyCtrl.Data;
}

UINT32 AmbaCSL_SdGetIrqStatus(const AMBA_SD_REG_s *pSdReg)
{
    UINT32 Temp;
    AMBA_SD_IRQ_STATUS_REG_s IrqStatus = pSdReg->IrqStatus;
    AmbaMisra_TypeCast32(&Temp, &IrqStatus);
    return Temp;
}

void AmbaCSL_SdClearIrqStatus(AMBA_SD_REG_s *pSdReg, UINT32 d)
{
    AMBA_SD_IRQ_STATUS_REG_s IrqStatus;
    AmbaMisra_TypeCast32(&IrqStatus, &d);
    pSdReg->IrqStatus = IrqStatus;
}

void AmbaCSL_SdSetTransferCtrl(AMBA_SD_REG_s *pSdReg, UINT32 d)
{
    AMBA_SD_TRANSFER_CTRL_REG_s        TransferCtrl;
    AmbaMisra_TypeCast32(&TransferCtrl, &d);
    pSdReg->TransferCtrl = TransferCtrl;
}

void AmbaCSL_RctSetSd0CtrlPhy0(UINT32 Value)
{
    AMBA_SD_PHY_CTRL0_REG_s SdPhyCtrl0;

    AmbaMisra_TypeCast32(&SdPhyCtrl0, &Value);
    pAmbaRCT_Reg->Sd0PhyCtrl0 = SdPhyCtrl0;
}

void AmbaCSL_RctSetSd2CtrlPhy0(UINT32 Value)
{
    AMBA_SD_PHY_CTRL0_REG_s SdPhyCtrl0;

    AmbaMisra_TypeCast32(&SdPhyCtrl0, &Value);
    pAmbaRCT_Reg->Sd1PhyCtrl0 = SdPhyCtrl0;
}

void AmbaCSL_RctSetSd0CtrlDinClkPolarity(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl0.SdDinClkPolarity = Value;
}

void AmbaCSL_RctSetSd0CtrlClkOutBypass(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl0.SdClkOutBypass = Value;
}

void AmbaCSL_RctSetSd0CtrlReset(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl0.Reset = Value;
}

void AmbaCSL_RctSetSd0CtrlClk270Alone(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl0.Clk270Alone = Value;
}

void AmbaCSL_RctSetSd0CtrlDelayChainSelect(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl0.DelayChainSelect = Value;
}

void AmbaCSL_RctSetSd0CtrlSdDutySelect(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl0.SdDutySelect = Value;
}

void AmbaCSL_RctSetSd0CtrlRxClkPolarity(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl0.RxClkPolarity = Value;
}

void AmbaCSL_RctSetSd0CtrlDataCmdBypass(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl0.SdDataCmdBypass = Value;
}

void AmbaCSL_RctSetSd0CtrlDllBypass(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl0.SdDllBypass = Value;
}

void AmbaCSL_RctSetSd0CtrlEnableDll(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.EnableDll = Value;
}

void AmbaCSL_RctSetSd0CtrlCoarseDelayStep(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.CoarseDelayStep = Value;
}

void AmbaCSL_RctSetSd0CtrlLockRange(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.LockRange = Value;
}

void AmbaCSL_RctSetSd0CtrlSelectInterClock(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.InternalClk = Value;
}

void AmbaCSL_RctSetSd0CtrlPDbb(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.PDbb = Value;
}

void AmbaCSL_RctSetSd0CtrlBypassFilter(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.BypassFilter = Value;
}

void AmbaCSL_RctSetSd0CtrlSelectFSM(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.SelectFSM = Value;
}

void AmbaCSL_RctSetSd0CtrlForceLockVshift(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.ForceLockVshift = Value;
}

void AmbaCSL_RctSetSd0CtrlForceLockCycle(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.ForceLockCycle = Value;
}

void AmbaCSL_RctSetSd0CtrlPowerDownShift(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.PowerDownShift = Value;
}

void AmbaCSL_RctSetSd0CtrlEnableAutoCoarse(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl2.EnableAutoCoarse = Value;
}

void AmbaCSL_RctSetSd0CtrlSelect(UINT32 Value)
{
    AMBA_SD_PHY_CTRL1_REG_s SdPhyCtrl1;
    AmbaMisra_TypeCast32(&SdPhyCtrl1, &Value);
    pAmbaRCT_Reg->Sd0PhyCtrl1 = SdPhyCtrl1;
}

void AmbaCSL_RctSetSd2CtrlSelect(UINT32 Value)
{
    AMBA_SD_PHY_CTRL1_REG_s SdPhyCtrl1;
    AmbaMisra_TypeCast32(&SdPhyCtrl1, &Value);
    pAmbaRCT_Reg->Sd1PhyCtrl1 = SdPhyCtrl1;
}

void AmbaCSL_RctSetSd0CtrlSelect0(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect0 = Value;
}

void AmbaCSL_RctSetSd0CtrlSelect2(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect2 = Value;
}

void AmbaCSL_RctSetSd0CtrlSelect3(UINT8 Value)
{
    pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect3 = Value;
}

UINT8 AmbaCSL_RctGetSd0CtrlDinClkPolarity(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl0.SdDinClkPolarity;
}

UINT8 AmbaCSL_RctGetSd0CtrlClkoutBypass(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl0.SdClkOutBypass;
}

UINT8 AmbaCSL_RctGetSd0CtrlReset(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl0.Reset;
}

UINT8 AmbaCSL_RctGetSd0CtrlClk270Alone(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl0.Clk270Alone;
}

UINT8 AmbaCSL_RctGetSd0CtrlDelayChainSelect(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl0.DelayChainSelect;
}

UINT8 AmbaCSL_RctGetSd0CtrlSdDutySelect(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl0.SdDutySelect;
}

UINT8 AmbaCSL_RctGetSd0CtrlRxClkPolarity(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl0.RxClkPolarity;
}

UINT8 AmbaCSL_RctGetSd0CtrlDataCmdBypass(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl0.SdDataCmdBypass;
}

UINT8 AmbaCSL_RctGetSd0CtrlDllBypass(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl0.SdDllBypass;
}

UINT32 AmbaCSL_RctGetSd0CtrlSelect(void)
{
    UINT32 Value;
    AMBA_SD_PHY_CTRL1_REG_s SdPhyCtrl1;

    SdPhyCtrl1 = pAmbaRCT_Reg->Sd0PhyCtrl1;
    AmbaMisra_TypeCast32(&Value, &SdPhyCtrl1);

    return Value;
}

UINT8 AmbaCSL_RctGetSd0CtrlSelect0(void)
{
    return (UINT8) pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect0;
}

UINT8 AmbaCSL_RctGetSd0CtrlSelect2(void)
{
    return (UINT8) pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect2;
}

UINT8 AmbaCSL_RctGetSd0CtrlSelect3(void)
{
    return (UINT8) pAmbaRCT_Reg->Sd0PhyCtrl1.SdSelect3;
}

UINT32 AmbaCSL_RctGetSd0CtrlSharedBusCtrl(void)
{
    UINT32 Value;
    AMBA_SD_PHY_CTRL2_REG_s SdPhyCtrl2;

    SdPhyCtrl2 = pAmbaRCT_Reg->Sd0PhyCtrl2;
    AmbaMisra_TypeCast32(&Value, &SdPhyCtrl2);

    return Value;
}

UINT8 AmbaCSL_RctGetSd0CtrlEnableDll(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.EnableDll;
}

UINT8 AmbaCSL_RctGetSd0CtrlCoarseDelayStep(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.CoarseDelayStep;
}

UINT8 AmbaCSL_RctGetSd0CtrlLockRange(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.LockRange;
}

UINT8 AmbaCSL_RctGetSd0CtrlSelectInterClock(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.InternalClk;
}

UINT8 AmbaCSL_RctGetSd0CtrlPDbb(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.PDbb;
}

UINT8 AmbaCSL_RctGetSd0CtrlBypassFilter(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.BypassFilter;
}

UINT8 AmbaCSL_RctGetSd0CtrlSelectFSM(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.SelectFSM;
}

UINT8 AmbaCSL_RctGetSd0CtrlForceLockVshift(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.ForceLockVshift;
}

UINT8 AmbaCSL_RctGetSd0CtrlForceLockCycle(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.ForceLockCycle;
}

UINT8 AmbaCSL_RctGetSd0CtrlPowerDownShift(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.PowerDownShift;
}

UINT8 AmbaCSL_RctGetSd0CtrlEnableAutoCoarse(void)
{
    return pAmbaRCT_Reg->Sd0PhyCtrl2.EnableAutoCoarse;
}

void AmbaCSL_RctSetSd2CtrlDinClkPolarity(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl0.SdDinClkPolarity = Value;
}

void AmbaCSL_RctSetSd2CtrlClkOutBypass(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl0.SdClkOutBypass  = Value;
}

void AmbaCSL_RctSetSd2CtrlReset(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl0.Reset = Value;
}

void AmbaCSL_RctSetSd2CtrlClk270Alone(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl0.Clk270Alone = Value;
}

void AmbaCSL_RctSetSd2CtrlDelayChainSelect(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl0.DelayChainSelect = Value;
}

void AmbaCSL_RctSetSd2CtrlSdDutySelect(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl0.SdDutySelect = Value;
}

void AmbaCSL_RctSetSd2CtrlRxClkPolarity(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl0.RxClkPolarity = Value;
}

void AmbaCSL_RctSetSd2CtrlDataCmdBypass(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl0.SdDataCmdBypass = Value;
}

void AmbaCSL_RctSetSd2CtrlDllBypass(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl0.SdDllBypass = Value;
}

void AmbaCSL_RctSetSd2CtrlSelect0(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect0 = Value;
}

void AmbaCSL_RctSetSd2CtrlSelect2(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect2 = Value;
}

void AmbaCSL_RctSetSd2CtrlSelect3(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect3 = Value;
}

void AmbaCSL_RctSetSd2CtrlEnableDll(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.EnableDll = Value;
}

void AmbaCSL_RctSetSd2CtrlCoarseDelayStep(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.CoarseDelayStep = Value;
}

void AmbaCSL_RctSetSd2CtrlLockRange(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.LockRange = Value;
}

void AmbaCSL_RctSetSd2CtrlSelectInterClock(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.InternalClk = Value;
}

void AmbaCSL_RctSetSd2CtrlPDbb(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.PDbb = Value;
}

void AmbaCSL_RctSetSd2CtrlBypassFilter(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.BypassFilter = Value;
}

void AmbaCSL_RctSetSd2CtrlSelectFSM(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.SelectFSM = Value;
}

void AmbaCSL_RctSetSd2CtrlForceLockVshift(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.ForceLockVshift = Value;
}

void AmbaCSL_RctSetSd2CtrlForceLockCycle(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.ForceLockCycle = Value;
}

void AmbaCSL_RctSetSd2CtrlPowerDownShift(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.PowerDownShift = Value;
}

void AmbaCSL_RctSetSd2CtrlEnableAutoCoarse(UINT8 Value)
{
    pAmbaRCT_Reg->Sd1PhyCtrl2.EnableAutoCoarse = Value;
}

UINT8 AmbaCSL_RctGetSd2CtrlDinClkPolarity(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl0.SdDinClkPolarity;
}

UINT8 AmbaCSL_RctGetSd2CtrlClkoutBypass(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl0.SdClkOutBypass;
}

UINT8 AmbaCSL_RctGetSd2CtrlReset(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl0.Reset;
}

UINT8 AmbaCSL_RctGetSd2CtrlClk270Alone(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl0.Clk270Alone;
}

UINT8 AmbaCSL_RctGetSd2CtrlDelayChainSelect(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl0.DelayChainSelect;
}

UINT8 AmbaCSL_RctGetSd2CtrlSdDutySelect(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl0.SdDutySelect;
}

UINT8 AmbaCSL_RctGetSd2CtrlRxClkPolarity(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl0.RxClkPolarity;
}

UINT8 AmbaCSL_RctGetSd2CtrlDataCmdBypass(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl0.SdDataCmdBypass;
}

UINT8 AmbaCSL_RctGetSd2CtrlDllBypass(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl0.SdDllBypass;
}

UINT32 AmbaCSL_RctGetSd2CtrlSharedBusCtrl(void)
{
    UINT32 Value;
    AMBA_SD_PHY_CTRL2_REG_s SdPhyCtrl2;

    SdPhyCtrl2 = pAmbaRCT_Reg->Sd1PhyCtrl2;
    AmbaMisra_TypeCast32(&Value, &SdPhyCtrl2);

    return Value;
}

UINT8 AmbaCSL_RctGetSd2CtrlEnableDll(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.EnableDll;
}

UINT8 AmbaCSL_RctGetSd2CtrlCoarseDelayStep(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.CoarseDelayStep;
}

UINT8 AmbaCSL_RctGetSd2CtrlLockRange(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.LockRange;
}

UINT8 AmbaCSL_RctGetSd2CtrlSelectInterClock(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.InternalClk;
}

UINT8 AmbaCSL_RctGetSd2CtrlPDbb(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.PDbb;
}

UINT8 AmbaCSL_RctGetSd2CtrlBypassFilter(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.BypassFilter;
}

UINT8 AmbaCSL_RctGetSd2CtrlSelectFSM(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.SelectFSM;
}

UINT8 AmbaCSL_RctGetSd2CtrlForceLockVshift(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.ForceLockVshift;
}

UINT8 AmbaCSL_RctGetSd2CtrlForceLockCycle(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.ForceLockCycle;
}

UINT8 AmbaCSL_RctGetSd2CtrlPowerDownShift(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.PowerDownShift;
}

UINT8 AmbaCSL_RctGetSd2CtrlEnableAutoCoarse(void)
{
    return pAmbaRCT_Reg->Sd1PhyCtrl2.EnableAutoCoarse;
}

UINT32 AmbaCSL_RctGetSd2CtrlSelect(void)
{
    UINT32 Value;
    AMBA_SD_PHY_CTRL1_REG_s SdPhyCtrl1;

    SdPhyCtrl1 = pAmbaRCT_Reg->Sd1PhyCtrl1;
    AmbaMisra_TypeCast32(&Value, &SdPhyCtrl1);

    return Value;
}

UINT8 AmbaCSL_RctGetSd2CtrlSelect0(void)
{
    return (UINT8) pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect0;
}

UINT8 AmbaCSL_RctGetSd2CtrlSelect2(void)
{
    return (UINT8) pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect2;
}

UINT8 AmbaCSL_RctGetSd2CtrlSelect3(void)
{
    return (UINT8) pAmbaRCT_Reg->Sd1PhyCtrl1.SdSelect3;
}
