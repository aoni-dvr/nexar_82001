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
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#include "AmbaCSL_PLL.h"
#include "AmbaCSL_UART.h"

/**
 *  AmbaCSL_UartReset - Reset UART and FIFO
 *  @param[in] pUartReg pointer to the UART registers
 */
#ifdef CONFIG_QNX
void AmbaCSL_UartReset(AMBA_UART_REG_s *pUartReg)
{
    /* reset UART */
    pUartReg->ResetCtrl = 0x1U;   /* Reset UART */
    AmbaDelayCycles(0x100000U);
    pUartReg->ResetCtrl = 0x0U;
}
#else
void AmbaCSL_UartReset(AMBA_UART_REG_s *pUartReg)
{
    UINT32 Delay = AmbaCSL_PllGetNumCpuCycleMs();

    /* reset UART */
    pUartReg->ResetCtrl = 0x1U;   /* Reset UART */
    AmbaDelayCycles(Delay);
    pUartReg->ResetCtrl = 0x0U;
}
#endif

/**
 *  AmbaCSL_UartSetDivider - Set Divider Latch values
 *  @param[in] pUartReg pointer to the UART registers
 *  @param[in] Value Divider Latch values (MSB = High, LSB = Low)
 */
void AmbaCSL_UartSetDivider(AMBA_UART_REG_s *pUartReg, UINT32 Value)
{
    /* enable Divisor Latch Access */
    pUartReg->LineCtrl |= 0x80U;

    /* set Divisor Latch value (16-bit) */
    pUartReg->DataBuf = Value & 0xffU;
    pUartReg->IntCtrl = (Value >> 8U) & 0xffU;

    /* disable Divisor Latch Access */
    pUartReg->LineCtrl &= ~0x80U;
}

/**
 *  AmbaCSL_UartGetDivider - Set Divider Latch value
 *  @param[in] pUartReg pointer to the UART registers
 *  @return the divider value
 */
UINT32 AmbaCSL_UartGetDivider(AMBA_UART_REG_s *pUartReg)
{
    UINT32 WorkUINT32;

    /* enable Divisor Latch Access */
    pUartReg->LineCtrl |= 0x80U;

    /* get Divisor Latch value (16-bit) */
    WorkUINT32 = pUartReg->IntCtrl & 0xffU;
    WorkUINT32 = (WorkUINT32 << 8U) | (pUartReg->DataBuf & 0xffU);

    /* disable Divisor Latch Access */
    pUartReg->LineCtrl &= ~0x80U;

    return WorkUINT32;
}

/**
 *  AmbaCSL_UartSetConfig - UART Configurations
 *  @param[in] pUartReg pointer to the UART registers
 *  @param[in] pUartConfig pointer to the configurations
 */
void AmbaCSL_UartSetConfig(AMBA_UART_REG_s *pUartReg, const AMBA_UART_CONFIG_s *pUartConfig)
{
    AMBA_UART_LINE_CTRL_REG_s LineCtrlRegVal = { 0U };
    UINT32 RegVal;

    if (pUartConfig->NumDataBits == UART_DATA_5_BIT) {
        LineCtrlRegVal.DataBits = 0U;
    } else if (pUartConfig->NumDataBits == UART_DATA_6_BIT) {
        LineCtrlRegVal.DataBits = 1U;
    } else if (pUartConfig->NumDataBits == UART_DATA_7_BIT) {
        LineCtrlRegVal.DataBits = 2U;
    } else {
        LineCtrlRegVal.DataBits = 3U;
    }

    if (pUartConfig->NumStopBits == UART_STOP_BIT_ONE) {
        LineCtrlRegVal.StopBits = 0U;
    } else {
        LineCtrlRegVal.StopBits = 1U;
    }

    if (pUartConfig->ParityBitMode == UART_PARITY_EVEN) {
        LineCtrlRegVal.ParityEnable = 1U;
        LineCtrlRegVal.EvenParity = 1U;
    } else if (pUartConfig->ParityBitMode == UART_PARITY_ODD) {
        LineCtrlRegVal.ParityEnable = 1U;
        LineCtrlRegVal.EvenParity = 0U;
    } else {
        LineCtrlRegVal.ParityEnable = 0U;
        LineCtrlRegVal.EvenParity = 0U;
    }

    AmbaMisra_TypeCast32(&RegVal, &LineCtrlRegVal);

    pUartReg->LineCtrl = RegVal;
}

/**
 *  AmbaCSL_UartSetFlowCtrl - Set flow control
 *  @param[in] pUartReg pointer to the UART registers
 *  @param[in] FlowCtrl Flow control selection
 */
void AmbaCSL_UartSetFlowCtrl(AMBA_UART_REG_s *pUartReg, UINT32 FlowCtrl)
{
    if (FlowCtrl == UART_FLOW_CTRL_HW) {
        AmbaCSL_UartEnableAutoFlowCtrl(pUartReg);
        AmbaCSL_UartEnableRTS(pUartReg);
    } else {
        AmbaCSL_UartDisableAutoFlowCtrl(pUartReg);
        AmbaCSL_UartDisableRTS(pUartReg);
    }
}
