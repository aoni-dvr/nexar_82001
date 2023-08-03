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

#ifndef AMBA_CSL_SPI_H
#define AMBA_CSL_SPI_H

#ifndef AMBA_SPI_DEF_H
#include "AmbaSPI_Def.h"
#endif

#include "AmbaReg_SPI.h"

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#define AMBA_SPI_MASTER_MAX_FIFO_ENTRY      32
#define AMBA_SPI_SLAVE_MAX_FIFO_ENTRY       32

/*
 * Macro Definitions
 */
static inline void AmbaCSL_SpiSetTransferModeTxRx(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.TransferMode = 0U;
}
static inline void AmbaCSL_SpiSetTransferModeTx(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.TransferMode = 1U;
}
static inline void AmbaCSL_SpiSetTransferModeRx(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.TransferMode = 2U;
}
static inline void AmbaCSL_SpiSetClockPhase(AMBA_SPI_REG_s *pSpiReg, UINT8 n)
{
    pSpiReg->Ctrl0.ClkPhase = (n);
}
static inline void AmbaCSL_SpiSetClockPolarity(AMBA_SPI_REG_s *pSpiReg, UINT8 n)
{
    pSpiReg->Ctrl0.ClkPolarity = (n);
}
static inline void AmbaCSL_SpiSetDataFrameSize(AMBA_SPI_REG_s *pSpiReg, UINT8 n)
{
    pSpiReg->Ctrl0.DataFrameSize = (UINT8)((n) - 1U);
}
static inline void AmbaCSL_SpiSetRxNumDataFrames(AMBA_SPI_REG_s *pSpiReg, UINT32 n)
{
    pSpiReg->Ctrl1.RxNumDataFrames = (UINT16)((n) - 1U);
}
static inline void AmbaCSL_SpiSetTxNumDataFrames(AMBA_SPI_REG_s *pSpiReg, UINT32 n)
{
    pSpiReg->TxFrameCount = ((n) - 1U) & 0xffffU;
}
static inline void AmbaCSL_SpiSetTxLsbTransmitFirst(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.TxLsbFirst = (UINT8)1U;
}
static inline void AmbaCSL_SpiSetRxLsbTransmitFirst(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.RxLsbFirst = (UINT8)1U;
}
static inline void AmbaCSL_SpiSetTxMsbTransmitFirst(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.TxLsbFirst = (UINT8)0U;
}
static inline void AmbaCSL_SpiSetRxMsbTransmitFirst(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.RxLsbFirst = (UINT8)0U;
}
static inline void AmbaCSL_SpiPackModeEnable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.PackMode = (UINT8)1U;
}
static inline void AmbaCSL_SpiSetExtraRxdMargin(AMBA_SPI_REG_s *pSpiReg, UINT8 n)
{
    pSpiReg->Ctrl0.ExtraRxdMargin = (UINT8)((n) & 0x0fU);
}
static inline UINT8 AmbaCSL_SpiGetDataFrameSize(const AMBA_SPI_REG_s *pSpiReg)
{
    return ((UINT8)pSpiReg->Ctrl0.DataFrameSize + 1U);
}

static inline void AmbaCSL_SpiDmaTxEnable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->DmaCtrl |= 2U;
}
static inline void AmbaCSL_SpiDmaRxEnable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->DmaCtrl |= 1U;
}
static inline void AmbaCSL_SpiDmaDisable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->DmaCtrl = 0U;
}

static inline void AmbaCSL_SpiResidueFlushModeEnable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.ResidueFlushMode = 1U;
}
static inline void AmbaCSL_SpiResidueFlushModeDisable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.ResidueFlushMode = 0U;
}
static inline void AmbaCSL_SpiSpiHoldEnable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.SpiHold = 1U;
}
static inline void AmbaCSL_SpiSpiHoldDisable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Ctrl0.SpiHold = 0U;
}

static inline void AmbaCSL_SpiEnable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Enable.Flag = 1U;
}
static inline void AmbaCSL_SpiDisable(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->Enable.Flag = 0U;
}

static inline void AmbaCSL_SpiDisableAllIRQ(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->IrqEnable = 0U;
}
static inline void AmbaCSL_SpiEnableTxFifoEmptyIRQ(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->IrqEnable |= 1U;
}
static inline void AmbaCSL_SpiDisableTxFifoEmptyIRQ(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->IrqEnable = pSpiReg->IrqEnable & (~0x1U);
}
static inline void AmbaCSL_SpiEnableRxFifoFullIRQ(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->IrqEnable |= 0x10U;
}
static inline void AmbaCSL_SpiDisableRxFifoFullIRQ(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->IrqEnable = pSpiReg->IrqEnable & (~0x10U);
}
static inline void AmbaCSL_SpiEnableTransDoneIRQ(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->IrqEnable |= 0x200U;
}
static inline void AmbaCSL_SpiDisableTransDoneIRQ(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->IrqEnable = pSpiReg->IrqEnable & (~0x200U);
}

static inline void AmbaCSL_SpiSlaveDisableAll(AMBA_SPI_REG_s *pSpiReg)
{
    pSpiReg->ChipSelect = 0U;
}
static inline void AmbaCSL_SpiSlaveDisable(AMBA_SPI_REG_s *pSpiReg, UINT32 n)
{
    pSpiReg->ChipSelect &= ~(n);
}
static inline void AmbaCSL_SpiSlaveEnable(AMBA_SPI_REG_s *pSpiReg, UINT32 n)
{
    pSpiReg->ChipSelect |= (n);
}

static inline void AmbaCSL_SpiSetCsActiveHigh(AMBA_SPI_REG_s *pSpiReg, UINT32 n)
{
    pSpiReg->ChipSelectPolarity |= (n);
}
static inline void AmbaCSL_SpiSetCsActiveLow(AMBA_SPI_REG_s *pSpiReg, UINT32 n)
{
    pSpiReg->ChipSelectPolarity &= ~(n);
}

static inline void AmbaCSL_SpiSetClkDivider(AMBA_SPI_REG_s *pSpiReg, UINT32 n)
{
    pSpiReg->BaudRate = (n & 0xffffU);
}

static inline void AmbaCSL_SpiSetTxFifoThreshold(AMBA_SPI_REG_s *pSpiReg, UINT8 n)
{
    pSpiReg->TxFifoThreshold.Value = (n);
}
static inline void AmbaCSL_SpiSetRxFifoThreshold(AMBA_SPI_REG_s *pSpiReg, UINT8 n)
{
    pSpiReg->RxFifoThreshold.Value = (n);
}

static inline UINT8 AmbaCSL_SpiGetRxFifoThreshold(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->RxFifoThreshold.Value;
}
static inline UINT8 AmbaCSL_SpiGetTxFifoDataSize(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->TxFifoDataSize.Value;    /* not real-time */
}
static inline UINT8 AmbaCSL_SpiGetRxFifoDataSize(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->RxFifoDataSize.Value;    /* not real-time */
}

static inline UINT32 AmbaCSL_SpiGetStatBusy(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->Status & 0x1U;
}
static inline UINT32 AmbaCSL_SpiGetStatTxFifoNotFull(const AMBA_SPI_REG_s *pSpiReg)
{
    return (pSpiReg->Status >> 1U) & 0x1U;
}
static inline UINT32 AmbaCSL_SpiGetStatTxFifoEmp(const AMBA_SPI_REG_s *pSpiReg)
{
    return (pSpiReg->Status >> 2U) & 0x1U;
}
static inline UINT32 AmbaCSL_SpiGetStatRxFifoNotEmp(const AMBA_SPI_REG_s *pSpiReg)
{
    return (pSpiReg->Status >> 3U) & 0x1U;
}
static inline UINT32 AmbaCSL_SpiGetStatRxFifoFull(const AMBA_SPI_REG_s *pSpiReg)
{
    return (pSpiReg->Status >> 4U) & 0x1U;
}
static inline UINT32 AmbaCSL_SpiGetStat(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->Status;
}

static inline UINT32 AmbaCSL_SpiClrTxFifoOvflowIrqSta(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqClear.TxFifoOverflow.Flag;
}
static inline UINT32 AmbaCSL_SpiClrRxFifoOvflowIrqSta(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqClear.RxFifoOverflow.Flag;
}
static inline UINT32 AmbaCSL_SpiClrRxFifoUnflowIrqSta(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqClear.RxFifoUnderflow.Flag;
}
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
static inline UINT32 AmbaCSL_SpiClrMultiMasContIrqSta(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqClear.MultiMasterContention.Flag;
}
static inline UINT32 AmbaCSL_SpiIsMultiMaster(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqRawStatus.MultiMasterContention;
}
#endif
static inline UINT32 AmbaCSL_SpiClrAnyOneIrqSta(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqClear.AnyOne.Flag;
}

static inline UINT32 AmbaCSL_SpiIsTxFifoOverflow(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqRawStatus.TxFifoOverflow;
}
static inline UINT32 AmbaCSL_SpiIsRxFifoUnderflow(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqRawStatus.RxFifoUnderflow;
}
static inline UINT32 AmbaCSL_SpiIsRxFifoOverflow(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqRawStatus.RxFifoOverflow;
}
static inline UINT32 AmbaCSL_SpiIsRxFifoFull(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->IrqRawStatus.RxFifoFull;
}

static inline void AmbaCSL_SpiWriteData(AMBA_SPI_REG_s *pSpiReg, UINT32 n)
{
    pSpiReg->DataBuf[0] = (n);
}
static inline UINT32 AmbaCSL_SpiReadData(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->DataBuf[0];
}

static inline void AmbaCSL_SpiSetSclkDelay(AMBA_SPI_REG_s *pSpiReg, UINT32 n)
{
    pSpiReg->SclkDelay = n;
}
static inline UINT32 AmbaCSL_SpiGetSclkDelay(const AMBA_SPI_REG_s *pSpiReg)
{
    return pSpiReg->SclkDelay;
}

/*
 * Defined in AmbaCSL_SPI.c
 */
void AmbaCSL_SpiClearIrqStatus(const AMBA_SPI_REG_s *pSpiReg);

#endif /* AMBA_CSL_SPI_H */
