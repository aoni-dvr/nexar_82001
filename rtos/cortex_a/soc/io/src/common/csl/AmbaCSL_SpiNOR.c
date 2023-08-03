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
#include "AmbaCSL_SpiNOR.h"

#ifdef CONFIG_QNX
AMBA_NOR_REG_s *pAmbaNOR_Reg;
#endif

/*
 *  @RoutineName:: AmbaCSL_NorSpiSetChipSelect
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorSpiSetChipSelect(UINT8 n)
{
    UINT32 Tmp32 = (UINT32)1U << (n);
    UINT8  Tmp8  = 0, Val;

    if (OK != AmbaWrap_memcpy(&Tmp8, &Tmp32, sizeof(Tmp8))) { /* Misrac */ }

    Tmp8 = ~Tmp8;

    Val = (UINT8)(pAmbaNOR_Reg->FlowControl.ChipSelect & 0xffU);
    Val &= Tmp8;
    pAmbaNOR_Reg->FlowControl.ChipSelect = Val; /* pAmbaNOR_Reg->FlowControl.ChipSelect &= ~Tmp8; */
}

/*
 *  @RoutineName:: AmbaCSL_NorWaitInt
 *
 *  @Description:: Waiting for NAND command done interrupt
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorWaitInt(void)
{
    while ((pAmbaNOR_Reg->IrqRawStatus.Data & 0x20U) == 0x0U) {
        /* wait loop */
    }

    AmbaCSL_NorClearDataLenthreachIrqStatus();
}

/*
 *  @RoutineName:: AmbaCSL_NorSetCmdFmt
 *
 *  @Description:: Set the format of command to NOR chip
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void AmbaCSL_NorSetCmdFmt(UINT8 CmdSize, UINT8 AddrSize, UINT8 DummySize,
                                 UINT32 DataByteCount)
{
#if 0
    AMBA_NOR_CMDFMT_REG_s Cmdfmt = { .Data = 0 };

    Cmdfmt.Bits.CmdLength = CmdSize & 0x3U;
    Cmdfmt.Bits.AddrLength = AddrSize & 0x7U;
    Cmdfmt.Bits.DummyLength = DummySize & 0x1fU;
    Cmdfmt.Bits.DataLength = DataByteCount;

    pAmbaNOR_Reg->CmdFmt.Data = Cmdfmt.Data;
#else
    pAmbaNOR_Reg->CmdFmt.CmdLength   = CmdSize;
    pAmbaNOR_Reg->CmdFmt.AddrLength  = AddrSize;
    pAmbaNOR_Reg->CmdFmt.DummyLength = DummySize;
    pAmbaNOR_Reg->CmdFmt.DataLength  = DataByteCount;
#endif
    pAmbaNOR_Reg->Command.Data = 0x0;
}

/*
 *  @RoutineName:: AmbaCSL_NorSetAddrReg
 *
 *  @Description:: Set the format of command to NOR chip
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
static void AmbaCSL_NorSetAddrReg(UINT32 Addr, UINT32 AddrHi)
{
    pAmbaNOR_Reg->Addr0.Data = Addr;
    pAmbaNOR_Reg->Addr1.Data = AddrHi;
}

/*
 *  @RoutineName:: AmbaCSL_NorResetEnable
 *
 *  @Description:: Reset the NOR chip
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorResetEnable(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, 0x0);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
}

/*
 *  @RoutineName:: AmbaCSL_NorReset
 *
 *  @Description:: Reset the NOR chip
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorReset(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, 0x0);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
}

/*
 *  @RoutineName:: AmbaCSL_NorReadID
 *
 *  @Description:: Issue Read ID command to Nor flash chip
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorReadID(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, 0x2U);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
    AmbaCSL_NorSetAddrReg(0x0,  0x0);
}

void AmbaCSL_NorReadSFDP(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x3U, 0x8U, 0x88U);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
    AmbaCSL_NorSetAddrReg(0x0,  0x0);
}

void AmbaCSL_NorReadIDOct(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x8, 20U);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
    AmbaCSL_NorSetAddrReg(0x0,  0x0);
}

/*
 *  @RoutineName:: AmbaCSL_NorReadStatusCmd
 *
 *  @Description:: Issue the read status command to read the Nor status
 *
 *  @Input      ::
 *      UINT8 SatusNum  : The number of Nor status register
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorReadStatusCmd(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, 0x1U);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
}

void AmbaCSL_NorReadStatusCmdOct(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x8, 0x1U);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
}

/*
 *  @RoutineName:: AmbaCSL_NorSetExtAddr
 *
 *  @Description:: Issue Bank addrss enable command to write the Nor status
 *
 *  @Input      ::  none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorSetExtAddr(UINT8 CmdIndex, UINT8 BankData)
{
    if (CmdIndex == AMBA_NOR_CMD_BANKREG_WRITE) {
        AmbaCSL_NorSetCmdFmt(0x1U, 0x1U, 0x0, 0x0);
        pAmbaNOR_Reg->Command.Bits.Cmd0 = AMBA_NOR_CMD_BANKREG_WRITE;
        AmbaCSL_NorSetAddrReg(BankData,  0x0);
    } else {
        AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, 0x0);
        pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
    }
}

/*
 *  @RoutineName:: AmbaCSL_NorWriteEnableCmd
 *
 *  @Description:: Issue the write enable command to write the Nor status
 *
 *  @Input      ::  none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorWriteEnableCmd(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, 0x0);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
}

/*
 *  @RoutineName:: AmbaCSL_NorWriteDisableCmd
 *
 *  @Description:: Issue the write Disable command to write the Nor status
 *
 *  @Input      ::  none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorWriteDisableCmd(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, 0x0);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
}

void AmbaCSL_NorSendQUADCmd(UINT8 CmdIndex, UINT8 ByteCount)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, ByteCount);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
}

/*
 *  @RoutineName:: AmbaCSL_NorWriteStatusCmd
 *
 *  @Description:: Issue the write status command to write the Nor status
 *
 *  @Input      ::
 *      UINT8 SatusNum  : The number of Nor status register
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorWriteStatusCmd(UINT8 CmdIndex, UINT8 Status)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x1U, 0x0, 0x0);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
    AmbaCSL_NorSetAddrReg(Status,  0x0);
}

/*
 *  @RoutineName:: AmbaCSL_NorClearStatusCmd
 *
 *  @Description:: Issue the clear status command to clear the Nor status
 *
 *  @Input      ::  none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorClearStatusCmd(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, 0x0);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
}

/*
 *  @RoutineName:: AmbaCSL_NorErase
 *
 *  @Description:: To erase the necessary block
 *
 *  @Input      ::
 *      UINT32 DefaultNandCtrl  : Default nand control register
 *      UINT32 AddrHi           : [33:32] address
 *      UINT32 Addr             : [31:0] address
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorEraseBlock(UINT8 CmdIndex, UINT32 BlkOffset, UINT8 AddrSize)
{
    AmbaCSL_NorSetCmdFmt(0x1U, AddrSize, 0x0, 0x0);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
    AmbaCSL_NorSetAddrReg(BlkOffset, 0x0);
}

/*
 *  @RoutineName:: AmbaCSL_NorErase
 *
 *  @Description:: To erase the entire of nor flash
 *
 *  @Input      ::
 *      UINT32 DefaultNandCtrl  : Default nand control register
 *      UINT32 AddrHi           : [33:32] address
 *      UINT32 Addr             : [31:0] address
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorEraseChip(UINT8 CmdIndex)
{
    AmbaCSL_NorSetCmdFmt(0x1U, 0x0, 0x0, 0x0);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
}

/*
 *  @RoutineName:: AmbaCSL_NorReadByte
 *
 *  @Description:: Read NAND flash memory by Byte
 *
 *  @Input      ::
 *      UINT32 DefaultNandCtrl  : Default nand control register
 *      UINT32 AddrHi           : [33:32] address
 *      UINT32 Addr             : [31:0] address
 *      UINT8 Area              : Programmed area
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorReadSetup(UINT8 CmdIndex, UINT8 DummyCycle, UINT32 Offset,
                          UINT32 Datalen, UINT8 AddrSize)
{
    AmbaCSL_NorSetCmdFmt(0x1U, AddrSize, DummyCycle, Datalen);
    pAmbaNOR_Reg->Command.Bits.Cmd0 = CmdIndex;
    AmbaCSL_NorSetAddrReg(Offset, 0x0);
}

/*
 *  @RoutineName:: AmbaCSL_NorProgramPage
 *
 *  @Description:: Program Nor flash memory by Page size
 *
 *  @Input      ::
 *      UINT32 DefaultNandCtrl  : Default nand control register
 *      UINT32 AddrHi           : [33:32] address
 *      UINT32 Addr             : [31:0] address
 *      UINT32 Area             : Programmed area
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorProgramSetup(UINT8 CmdIndex, UINT8 DummyCycle, UINT32 Offset,
                             UINT32 Datalen, UINT8 AddrSize)
{
    AmbaCSL_NorSetCmdFmt(0x1U, AddrSize, DummyCycle, Datalen);
    pAmbaNOR_Reg->Command.Bits.Cmd0 =  CmdIndex;
    AmbaCSL_NorSetAddrReg(Offset, 0x0);
}

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
/*
 *  @RoutineName:: AmbaCSL_NorSmErrorDisable
 *
 *  @Description:: The SM error detection logic is disabled
 *
 *  @Input      ::  none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorSmErrorDisable(void)
{
    pAmbaNOR_Reg->FaultInject.SmErrorDisable = 1U;
}

/*
 *  @RoutineName:: AmbaCSL_NorSmErrorEnable
 *
 *  @Description:: The SM error detection logic is enabled
 *
 *  @Input      ::  none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorSmErrorEnable(void)
{
    pAmbaNOR_Reg->FaultInject.SmErrorDisable = 0U;
}
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
/*
 *  @RoutineName:: AmbaCSL_NorSetDqsDelay
 *
 *  @Description:: To set the dqs signal delay
 *
 *  @Input      ::  none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_NorSetDqsDelay(UINT32 Lo, UINT32 Hi)
{
    pAmbaNOR_Reg->DqsOcta.DqsDelayBinLo = (UINT8)(Lo & 0x7fU);
    pAmbaNOR_Reg->DqsOcta.DqsDelayBinHi = (UINT8)(Hi & 0x7fU);
}
#endif
