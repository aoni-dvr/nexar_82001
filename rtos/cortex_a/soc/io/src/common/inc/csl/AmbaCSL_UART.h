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

#ifndef AMBA_CSL_UART_H
#define AMBA_CSL_UART_H

#ifndef AMBA_UART_DEF_H
#include "AmbaUART_Def.h"
#endif

#include "AmbaReg_UART.h"

/*
 * Inline Function Definitions
 */
static inline UINT8 AmbaCSL_UartReadData(const AMBA_UART_REG_s *pUartReg)
{
    return (UINT8)(pUartReg->DataBuf & 0xffU);
}
static inline void AmbaCSL_UartWriteData(AMBA_UART_REG_s *pUartReg, UINT8 Val)
{
    pUartReg->DataBuf = (UINT32)Val;
}

static inline UINT32 AmbaCSL_UartGetIrqID(const AMBA_UART_REG_s *pUartReg)
{
    return (pUartReg->FifoCtrl & 0xfU);
}
static inline void AmbaCSL_UartSetFifoCtrl(AMBA_UART_REG_s *pUartReg, UINT32 Val)
{
    pUartReg->FifoCtrl = Val;
}
static inline void AmbaCSL_UartSetRxThreshold(AMBA_UART_REG_s *pUartReg, UINT32 Val)
{
    pUartReg->RxThreshold = Val;
}

static inline void AmbaCSL_UartResetRxFIFO(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->ResetCtrl = 2U;
}
static inline void AmbaCSL_UartResetTxFIFO(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->ResetCtrl = 4U;
}
static inline void AmbaCSL_UartResetAllFIFO(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->ResetCtrl = 6U;
}

static inline void AmbaCSL_UartDisableAllIRQ(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->IntCtrl = 0U;
}
static inline void AmbaCSL_UartEnableAllIRQ(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->IntCtrl = 0xbfU;
}
static inline void AmbaCSL_UartEnableAllExceptTxIRQ(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->IntCtrl = 0x3dU;
}
static inline void AmbaCSL_UartEnablePtimeIRQ(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->IntCtrl = 0x80U;
}

static inline void AmbaCSL_UartDisableTxBufEmptyIRQ(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->IntCtrl &= ~0x2U;
}
static inline void AmbaCSL_UartEnableRxDataReadyIRQ(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->IntCtrl |= 0x1U;
}
static inline void AmbaCSL_UartDisableRxDataReadyIRQ(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->IntCtrl &= ~0x1U;
}

static inline void AmbaCSL_UartSetDataBits(AMBA_UART_REG_s *pUartReg, UINT32 Val)
{
    pUartReg->LineCtrl = (pUartReg->LineCtrl & ~0x3U) | (Val & 0x3U);
}
static inline void AmbaCSL_UartSetStopBits(AMBA_UART_REG_s *pUartReg, UINT32 Val)
{
    pUartReg->LineCtrl = (pUartReg->LineCtrl & ~0x4U) | ((Val & 0x1U) << 2U);
}

static inline void AmbaCSL_UartDisableAutoFlowCtrl(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->ModemCtrl.AutoFlowCtrl = 0U;
}
static inline void AmbaCSL_UartEnableAutoFlowCtrl(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->ModemCtrl.AutoFlowCtrl = 1U;
}
static inline void AmbaCSL_UartDisableRTS(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->ModemCtrl.RTS = 0U;
}
static inline void AmbaCSL_UartEnableRTS(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->ModemCtrl.RTS = 1U;
}

static inline UINT32 AmbaCSL_UartGetBusyState(const AMBA_UART_REG_s *pUartReg)
{
    return pUartReg->Status & 0x1U;
}
static inline UINT32 AmbaCSL_UartGetTxNotFullState(const AMBA_UART_REG_s *pUartReg)
{
    return pUartReg->Status & 0x2U;
}
static inline UINT32 AmbaCSL_UartGetTxEmptyState(const AMBA_UART_REG_s *pUartReg)
{
    return pUartReg->LineStatus & 0x40U;
}
static inline UINT32 AmbaCSL_UartGetDataReadyState(const AMBA_UART_REG_s *pUartReg)
{
    return pUartReg->LineStatus & 0x1U;
}

static inline void AmbaCSL_UartDmaDisable(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->DmaCtrl = 0U;
}
static inline void AmbaCSL_UartDmaRxDisable(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->DmaCtrl &= ~0x1U;
}
static inline void AmbaCSL_UartDmaRxEnable(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->DmaCtrl |= 0x1U;
}
static inline void AmbaCSL_UartDmaTxDisable(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->DmaCtrl &= ~0x2U;
}
static inline void AmbaCSL_UartDmaTxEnable(AMBA_UART_REG_s *pUartReg)
{
    pUartReg->DmaCtrl |= 0x2U;
}

static inline UINT32 AmbaCSL_UartGetRxFifoDataSize(const AMBA_UART_REG_s *pUartReg)
{
    return pUartReg->RxFifoDataSize.Value;
}
/*
 * Defined in AmbaCSL_UART.c
 */
void AmbaCSL_UartReset(AMBA_UART_REG_s *pUartReg);

void AmbaCSL_UartSetDivider(AMBA_UART_REG_s *pUartReg, UINT32 Value);
UINT32 AmbaCSL_UartGetDivider(AMBA_UART_REG_s *pUartReg);

void AmbaCSL_UartSetConfig(AMBA_UART_REG_s *pUartReg, const AMBA_UART_CONFIG_s *pUartConfig);
void AmbaCSL_UartSetFlowCtrl(AMBA_UART_REG_s *pUartReg, UINT32 FlowCtrl);

#endif /* AMBA_CSL_UART_H */
