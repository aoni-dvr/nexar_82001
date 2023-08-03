/**
 *  @file AmbaCSL_SpiNOR.h
 *
 *  @Copyright      :: Copyright (C) 2019 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions of Chip Support Library for NAND flash
 */

#ifndef AMBA_CSL_NOR_H
#define AMBA_CSL_NOR_H

#include "AmbaReg_SpiNOR.h"

#define NOR_TXFIFO_SIZE     (256U)
#define NOR_RXFIFO_SIZE     (256U)

/*
 * NOR Flash Memory commands
 */
#define AMBA_NOR_CMD_READ           (0x3U)
#define AMBA_NOR_CMD_BANKREG_WRITE  (0x17U)

/*
 * Macro Definitions
 */
#if 1
static inline UINT32 AmbaCSL_NorReadData(void)
{
    return pAmbaNOR_Reg->RxData[0].Bits.Data;
}
static inline void  AmbaCSL_NorWriteData(UINT8 n)
{
    pAmbaNOR_Reg->TxData[0].Bits.Data = (n);
}

static inline void AmbaCSL_NorTransEnable(void)
{
    pAmbaNOR_Reg->Start.Bits.Strttrx = 1U;
}
static inline void AmbaCSL_NorTransDisable(void)
{
    pAmbaNOR_Reg->Start.Bits.Strttrx = 0;
}

static inline void AmbaCSL_NorSetTxFifoThreshold(UINT8 n)
{
    pAmbaNOR_Reg->TxFifoThreshold.Bits.Value = (n);
}
static inline void AmbaCSL_NorSetRxFifoThreshold(UINT8 n)
{
    pAmbaNOR_Reg->RxFifoThreshold.Bits.Value = (n);
}
static inline UINT32 AmbaCSL_NorGetTxFifoDataSize(void)
{
    return pAmbaNOR_Reg->TxLevel.Bits.DataSize;   /* not real-time */
}
static inline UINT32 AmbaCSL_NorGetRxFifoDataSize(void)
{
    return pAmbaNOR_Reg->RxLevel.Bits.DataSize;   /* not real-time */
}

static inline void AmbaCSL_NorEnableTxDMA(void)
{
    pAmbaNOR_Reg->EnableDMA.Bits.TxDmaEnable = 1U;   /* not real-time */
}
static inline void AmbaCSL_NorEnableRxDMA(void)
{
    pAmbaNOR_Reg->EnableDMA.Bits.RxDmaEnable = 1U;   /* not real-time */
}
static inline void AmbaCSL_NorDisableTxDMA(void)
{
    pAmbaNOR_Reg->EnableDMA.Bits.TxDmaEnable  = 0;   /* not real-time */
}
static inline void AmbaCSL_NorDisableRxDMA(void)
{
    pAmbaNOR_Reg->EnableDMA.Bits.RxDmaEnable = 0;   /* not real-time */
}

static inline UINT32 AmbaCSL_NorGetRxFifoNotEmpty(void)
{
    return pAmbaNOR_Reg->FIFOStatus.Bits.RxFifoNotEmpty;
}
static inline UINT32 AmbaCSL_NorGetRxFifoFull(void)
{
    return pAmbaNOR_Reg->FIFOStatus.Bits.RxFifoFull;
}
static inline UINT32 AmbaCSL_NorGetTxFifoEmpty(void)
{
    return pAmbaNOR_Reg->FIFOStatus.Bits.TxFifoEmpty;
}
static inline UINT32 AmbaCSL_NorGetTxFifoNotFull(void)
{
    return pAmbaNOR_Reg->FIFOStatus.Bits.TxFifoNotFull;
}
static inline void AmbaCSL_NorResetTxFifo(void)
{
    pAmbaNOR_Reg->TxFIFOReset.Bits.Reset  = 1U;
}
static inline void AmbaCSL_NorResetRxFifio(void)
{
    pAmbaNOR_Reg->RxFIFOReset.Bits.Reset = 1U;
}

static inline void AmbaCSL_NorClearTxAlmostEmptyIrqStatus(void)
{
    pAmbaNOR_Reg->IrqClear.Bits.TxAlmostEmpty= 1U;
}
static inline void AmbaCSL_NorClearTxOverflowIrqStatus(void)
{
    pAmbaNOR_Reg->IrqClear.Bits.TxOverflow = 1U;
}
static inline void AmbaCSL_NorClearRxUnderflowIrqStatus(void)
{
    pAmbaNOR_Reg->IrqClear.Bits.RxUnderflow = 1U;
}
static inline void AmbaCSL_NorClearRxOverflowIrqStatus(void)
{
    pAmbaNOR_Reg->IrqClear.Bits.RxOverflow = 1U;
}
static inline void AmbaCSL_NorClearRxAlmostfullIrqStatus(void)
{
    pAmbaNOR_Reg->IrqClear.Bits.RxAlmostfull = 1U;
}
static inline void AmbaCSL_NorClearDataLenthreachIrqStatus(void)
{
    pAmbaNOR_Reg->IrqClear.Bits.DataLenthreach = 1U;
}
static inline void AmbaCSL_NorClearTxunderflowIrqStatus(void)
{
    pAmbaNOR_Reg->IrqClear.Bits.Txunderflow = 1U;
}
static inline void AmbaCSL_NorClearAllIrqStatus(void)
{
    pAmbaNOR_Reg->IrqClear.Data = NOR_INTR_ALL;
}

static inline UINT32 AmbaCSL_NorIsTxAlmostEmpty(void)
{
    return pAmbaNOR_Reg->IrqRawStatus.Bits.TxAlmostEmpty;
}
static inline UINT32 AmbaCSL_NorIsTxOverflow(void)
{
    return pAmbaNOR_Reg->IrqRawStatus.Bits.TxOverflow;
}
static inline UINT32 AmbaCSL_NorIsRxFifoUnderflow(void)
{
    return pAmbaNOR_Reg->IrqRawStatus.Bits.RxUnderflow;
}
static inline UINT32 AmbaCSL_NorIsRxFifoOverflow(void)
{
    return pAmbaNOR_Reg->IrqRawStatus.Bits.RxOverflow;
}
static inline UINT32 AmbaCSL_NorIsRxAlmostfull(void)
{
    return pAmbaNOR_Reg->IrqRawStatus.Bits.RxAlmostfull;
}
static inline UINT32 AmbaCSL_NorIsDataLenthreach(void)
{
    return pAmbaNOR_Reg->IrqRawStatus.Bits.DataLenthreach;
}
static inline UINT32 AmbaCSL_NorIsTxunderflow(void)
{
    return pAmbaNOR_Reg->IrqRawStatus.Bits.Txunderflow;
}

static inline void AmbaCSL_NorEnableAllIRQ(void)
{
    pAmbaNOR_Reg->IrqEnable.Data = NOR_INTR_ALL;
}
static inline void AmbaCSL_NorDisableAllIRQ(void)
{
    pAmbaNOR_Reg->IrqEnable.Data = 0;
}
static inline void AmbaCSL_NorEnableDataLenthreachIRQ(void)
{
    pAmbaNOR_Reg->IrqEnable.Bits.DataLenthreach = 1U;
}
static inline void AmbaCSL_NorDisableDataLenthreachIRQ(void)
{
    pAmbaNOR_Reg->IrqEnable.Bits.DataLenthreach = 0;
}

static inline UINT32 AmbaCSL_NorSpiGetClockDiv(void)
{
    return pAmbaNOR_Reg->FlowControl.ClkDivider;
}
static inline void AmbaCSL_NorSpiSetClockDiv(UINT8 n)
{
    pAmbaNOR_Reg->FlowControl.ClkDivider = (n);
}
//static inline void AmbaCSL_NorSpiSetChipSelect(UINT8 n)                       {pAmbaNOR_Reg->FlowControl.ChipSelect &= ~((UINT32)1U << (n));}
void AmbaCSL_NorSpiSetChipSelect(UINT8 n);
static inline void AmbaCSL_NorEnableFlowControl(void)
{
    pAmbaNOR_Reg->FlowControl.FlowControl= 1U;
}
static inline void AmbaCSL_NorDisableFlowControl(void)
{
    pAmbaNOR_Reg->FlowControl.FlowControl = 0;
}
static inline void AmbaCSL_NorSpiSetHoldPing(UINT8 n)
{
    pAmbaNOR_Reg->FlowControl.Hold = (n);
}
static inline void AmbaCSL_NorSpiSetSpiclkpolarity(void)
{
    pAmbaNOR_Reg->FlowControl.SpiClkPolarity = 1U;
}
static inline void AmbaCSL_NorSpiSetFlowControl(AMBA_NOR_FLOWCTRL_REG_s n)
{
    pAmbaNOR_Reg->FlowControl = (n);
}
static inline void AmbaCSL_NorSpiSetRxSampleDelay(UINT8 n)
{
    pAmbaNOR_Reg->FlowControl.RxSampleDelay = (n);
}

static inline void AmbaCSL_NorSpiSetDTRNumCmdLane(UINT8 n)
{
    pAmbaNOR_Reg->DTR_Ctrl.NumCmdLane = (n);
}
static inline void AmbaCSL_NorSpiSetDTRNumAddrLane(UINT8 n)
{
    pAmbaNOR_Reg->DTR_Ctrl.NumAddrLane = (n);
}
static inline void AmbaCSL_NorSpiSetDTRNumDataLane(UINT8 n)
{
    pAmbaNOR_Reg->DTR_Ctrl.NumDataLane = (n);
}
static inline void AmbaCSL_NorSpiSetDTRLSBFirst(void)
{
    pAmbaNOR_Reg->DTR_Ctrl.LSBFirst = 1U;
}
static inline void AmbaCSL_NorSpiSetDTRMSBFirst(void)
{
    pAmbaNOR_Reg->DTR_Ctrl.LSBFirst = 0;
}
static inline void AmbaCSL_NorSpiSetDTRDataReadEn(void)
{
    pAmbaNOR_Reg->DTR_Ctrl.DataReadn = 1U;
}
static inline void AmbaCSL_NorSpiSetDTRDataReadDis(void)
{
    pAmbaNOR_Reg->DTR_Ctrl.DataReadn = 0;
}
static inline void AmbaCSL_NorSpiSetDTRDataWriteEn(void)
{
    pAmbaNOR_Reg->DTR_Ctrl.DataWriteen = 1U;
}
static inline void AmbaCSL_NorSpiSetDTRDataWriteDis(void)
{
    pAmbaNOR_Reg->DTR_Ctrl.DataWriteen = 0;
}
static inline void AmbaCSL_NorSpiSetDTRCmdEnable(void)
{
    pAmbaNOR_Reg->DTR_Ctrl.CmdDTR = 1U;
}
static inline void AmbaCSL_NorSpiSetDTRCmdDisable(void)
{
    pAmbaNOR_Reg->DTR_Ctrl.CmdDTR = 0;
}
static inline void AmbaCSL_NorSpiSetDTRCtrl(AMBA_NOR_DTR_CTRL_REG_s n)
{
    pAmbaNOR_Reg->DTR_Ctrl = (n);
}

#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)|| defined (CONFIG_SOC_CV2FS)|| defined (CONFIG_SOC_CV22FS)
static inline void AmbaCSL_NorSpiDqsEnable(void)
{
    pAmbaNOR_Reg->DqsOcta.DqsEnable = 1U;
}
static inline void AmbaCSL_NorSpiDqsDisable(void)
{
    pAmbaNOR_Reg->DqsOcta.DqsEnable = 0U;
}
#endif

#else
#define AmbaCSL_NorWriteData(n)            pAmbaNOR_Reg->TxData[0].Bits.Data = (n)
#define AmbaCSL_NorReadData()           pAmbaNOR_Reg->RxData[0].Bits.Data
#define AmbaCSL_NorTransEnable()         pAmbaNOR_Reg->Start.Bits.Strttrx = 1
#define AmbaCSL_NorTransDisable()         pAmbaNOR_Reg->Start.Bits.Strttrx = 0

#define AmbaCSL_NorSetTxFifoThreshold(n)               pAmbaNOR_Reg->TxFifoThreshold.Bits.Value = (n)
#define AmbaCSL_NorSetRxFifoThreshold(n)               pAmbaNOR_Reg->RxFifoThreshold.Bits.Value = (n)
#define AmbaCSL_NorGetTxFifoDataSize()                   pAmbaNOR_Reg->TxLevel.Bits.DataSize      /* not real-time */
#define AmbaCSL_NorGetRxFifoDataSize()                   pAmbaNOR_Reg->RxLevel.Bits.DataSize      /* not real-time */

#define AmbaCSL_NorEnableTxDMA()                   pAmbaNOR_Reg->EnableDMA.Bits.TxDmaEnable = 1      /* not real-time */
#define AmbaCSL_NorEnableRxDMA()                   pAmbaNOR_Reg->EnableDMA.Bits.RxDmaEnable = 1      /* not real-time */
#define AmbaCSL_NorDisableTxDMA()                   pAmbaNOR_Reg->EnableDMA.Bits.TxDmaEnable  = 0    /* not real-time */
#define AmbaCSL_NorDisableRxDMA()                   pAmbaNOR_Reg->EnableDMA.Bits.RxDmaEnable = 0     /* not real-time */

#define AmbaCSL_NorGetRxFifoNotEmpty()                   pAmbaNOR_Reg->FIFOStatus.Bits.RxFifoNotEmpty
#define AmbaCSL_NorGetRxFifoFull()                   pAmbaNOR_Reg->FIFOStatus.Bits.RxFifoFull
#define AmbaCSL_NorGetTxFifoEmpty()                   pAmbaNOR_Reg->FIFOStatus.Bits.TxFifoEmpty
#define AmbaCSL_NorGetTxFifoNotFull()                   pAmbaNOR_Reg->FIFOStatus.Bits.TxFifoNotFull
#define AmbaCSL_NorResetTxFifo()                   pAmbaNOR_Reg->TxFIFOReset.Bits.Reset  = 1
#define AmbaCSL_NorResetRxFifio()                   pAmbaNOR_Reg->RxFIFOReset.Bits.Reset = 1

#define AmbaCSL_NorClearTxAlmostEmptyIrqStatus()            pAmbaNOR_Reg->IrqClear.Bits.TxAlmostEmpty= 1
#define AmbaCSL_NorClearTxOverflowIrqStatus()            pAmbaNOR_Reg->IrqClear.Bits.TxOverflow = 1
#define AmbaCSL_NorClearRxUnderflowIrqStatus()       pAmbaNOR_Reg->IrqClear.Bits.RxUnderflow = 1
#define AmbaCSL_NorClearRxOverflowIrqStatus()         pAmbaNOR_Reg->IrqClear.Bits.RxOverflow = 1
#define AmbaCSL_NorClearRxAlmostfullIrqStatus()       pAmbaNOR_Reg->IrqClear.Bits.RxAlmostfull = 1
#define AmbaCSL_NorClearDataLenthreachIrqStatus()         pAmbaNOR_Reg->IrqClear.Bits.DataLenthreach = 1
#define AmbaCSL_NorClearTxunderflowIrqStatus()       pAmbaNOR_Reg->IrqClear.Bits.Txunderflow = 1
#define AmbaCSL_NorClearAllIrqStatus()                pAmbaNOR_Reg->IrqClear.Data = NOR_INTR_ALL

#define AmbaCSL_NorIsTxAlmostEmpty()             pAmbaNOR_Reg->IrqRawStatus.Bits.TxAlmostEmpty
#define AmbaCSL_NorIsTxOverflow()                    pAmbaNOR_Reg->IrqRawStatus.Bits.TxOverflow
#define AmbaCSL_NorIsRxFifoUnderflow()                   pAmbaNOR_Reg->IrqRawStatus.Bits.RxUnderflow
#define AmbaCSL_NorIsRxFifoOverflow()                    pAmbaNOR_Reg->IrqRawStatus.Bits.RxOverflow
#define AmbaCSL_NorIsRxAlmostfull()                        pAmbaNOR_Reg->IrqRawStatus.Bits.RxAlmostfull
#define AmbaCSL_NorIsDataLenthreach()         pAmbaNOR_Reg->IrqRawStatus.Bits.DataLenthreach
#define AmbaCSL_NorIsTxunderflow()       pAmbaNOR_Reg->IrqRawStatus.Bits.Txunderflow

#define AmbaCSL_NorEnableAllIRQ()                       pAmbaNOR_Reg->IrqEnable.Data = NOR_INTR_ALL
#define AmbaCSL_NorDisableAllIRQ()                       pAmbaNOR_Reg->IrqEnable.Data = 0
#define AmbaCSL_NorEnableDataLenthreachIRQ()         pAmbaNOR_Reg->IrqEnable.Bits.DataLenthreach = 1
#define AmbaCSL_NorDisableDataLenthreachIRQ()         pAmbaNOR_Reg->IrqEnable.Bits.DataLenthreach = 0

#define AmbaCSL_NorSpiGetClockDiv()                       pAmbaNOR_Reg->FlowControl.Bits.ClkDivider
#define AmbaCSL_NorSpiSetClockDiv(n)                       pAmbaNOR_Reg->FlowControl.Bits.ClkDivider = (n)
#define AmbaCSL_NorSpiSetChipSelect(n)                       pAmbaNOR_Reg->FlowControl.Bits.ChipSelect &= ~(0x01 << (n))
#define AmbaCSL_NorEnableFlowControl()                      pAmbaNOR_Reg->FlowControl.Bits.FlowControl= 1
#define AmbaCSL_NorDisableFlowControl()                     pAmbaNOR_Reg->FlowControl.Bits.FlowControl = 0
#define AmbaCSL_NorSpiSetHoldPing(n)                       pAmbaNOR_Reg->FlowControl.Bits.Hold = (n)
#define AmbaCSL_NorSpiSetSpiclkpolarity()                       pAmbaNOR_Reg->FlowControl.Bits.SpiClkPolarity = 1
#define AmbaCSL_NorSpiSetFlowControl(n)                       pAmbaNOR_Reg->FlowControl.Data = (n)
#define AmbaCSL_NorSpiSetRxSampleDelay(n)                       pAmbaNOR_Reg->FlowControl.Bits.RxSampleDelay = (n)

#define AmbaCSL_NorSpiSetDTRNumCmdLane(n)                       pAmbaNOR_Reg->DTR_Ctrl.NumCmdLane = (n)
#define AmbaCSL_NorSpiSetDTRNumAddrLane(n)                       pAmbaNOR_Reg->DTR_Ctrl.NumAddrLane = (n)
#define AmbaCSL_NorSpiSetDTRNumDataLane(n)                       pAmbaNOR_Reg->DTR_Ctrl.NumDataLane = (n)
#define AmbaCSL_NorSpiSetDTRLSBFirst()                     pAmbaNOR_Reg->DTR_Ctrl.LSBFirst = 1
#define AmbaCSL_NorSpiSetDTRMSBFirst()                     pAmbaNOR_Reg->DTR_Ctrl.LSBFirst = 0
#define AmbaCSL_NorSpiSetDTRDataReadEn()                     pAmbaNOR_Reg->DTR_Ctrl.DataReadn = 1
#define AmbaCSL_NorSpiSetDTRDataReadDis()                     pAmbaNOR_Reg->DTR_Ctrl.DataReadn = 0
#define AmbaCSL_NorSpiSetDTRDataWriteEn()                     pAmbaNOR_Reg->DTR_Ctrl.DataWriteen = 1
#define AmbaCSL_NorSpiSetDTRDataWriteDis()                     pAmbaNOR_Reg->DTR_Ctrl.DataWriteen = 0
#define AmbaCSL_NorSpiSetDTRCmdEnable()                     pAmbaNOR_Reg->DTR_Ctrl.CmdDTR = 1
#define AmbaCSL_NorSpiSetDTRCmdDisable()                     pAmbaNOR_Reg->DTR_Ctrl.CmdDTR = 0
#define AmbaCSL_NorSpiSetDTRCtrl(n)                     pAmbaNOR_Reg->DTR_Ctrl.Data = (n)
#define AmbaCSL_NorSpiSetCMDFmt(n)                     pAmbaNOR_Reg->CmdFmt.Data = (n)
#endif
/*
 * Defined in AmbaCSL_NOR.c (CSL for Nor flash)
 */
void AmbaCSL_NorWaitInt(void);
void AmbaCSL_NorResetEnable(UINT8 CmdIndex);
void AmbaCSL_NorReset(UINT8 CmdIndex);
void AmbaCSL_NorReadID(UINT8 CmdIndex);
void AmbaCSL_NorReadSFDP(UINT8 CmdIndex);
void AmbaCSL_NorReadIDOct(UINT8 CmdIndex);
void AmbaCSL_NorReadStatusCmd(UINT8 CmdIndex);
void AmbaCSL_NorReadStatusCmdOct(UINT8 CmdIndex);
void AmbaCSL_NorWriteEnableCmd(UINT8 CmdIndex);
void AmbaCSL_NorWriteDisableCmd(UINT8 CmdIndex);
void AmbaCSL_NorWriteStatusCmd(UINT8 CmdIndex, UINT8 Status);
void AmbaCSL_NorClearStatusCmd(UINT8 CmdIndex);
void AmbaCSL_NorSetExtAddr(UINT8 CmdIndex, UINT8 BankData);
void AmbaCSL_NorEraseBlock(UINT8 CmdIndex, UINT32 BlkOffset, UINT8 AddrSize);
void AmbaCSL_NorEraseChip(UINT8 CmdIndex);
void AmbaCSL_NorReadSetup(UINT8 CmdIndex, UINT8 DummyCycle, UINT32 Offset,
                          UINT32 Datalen, UINT8 AddrSize);
void AmbaCSL_NorProgramSetup(UINT8 CmdIndex, UINT8 DummyCycle, UINT32 Offset,
                             UINT32 Datalen, UINT8 AddrSize);
void AmbaCSL_NorSendQUADCmd(UINT8 CmdIndex, UINT8 ByteCount);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
void AmbaCSL_NorSmErrorDisable(void);
void AmbaCSL_NorSmErrorEnable(void);
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
void AmbaCSL_NorSetDqsDelay(UINT32 Lo, UINT32 Hi);
#endif
#endif /* AMBA_CSL_NOR_H */

