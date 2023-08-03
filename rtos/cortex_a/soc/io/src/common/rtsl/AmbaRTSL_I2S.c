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

#include "AmbaMisraFix.h"

#include "AmbaRTSL_I2S.h"
#include "AmbaCSL_I2S.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#include "AmbaCSL_Scratchpad.h"
#else
#include "AmbaCortexA53.h"
#endif

AMBA_I2S_REG_s *pAmbaI2S_Reg[AMBA_NUM_I2S_CHANNEL];

/**
 *  I2S_ConfigFreq - Configure I2S frequency
 *  @param[in] pI2sReg pointer to I2S registers
 *  @param[in] pCtrl I2S control information
 */
static void I2S_ConfigFreq(AMBA_I2S_REG_s *pI2sReg, const AMBA_I2S_CTRL_s *pCtrl)
{
    /* Transmitter is at falling edge and receiver is at rising edge */
    pI2sReg->ClkCtrl.ClkDiv = (UINT8)(pCtrl->ClkDivider & 0x1FU);
    AmbaCSL_I2sSetClkRsp(pI2sReg, pCtrl->RxCtrl.Rsp);
    AmbaCSL_I2sSetClkTsp(pI2sReg, pCtrl->TxCtrl.Tsp);
    if ((pCtrl->ClkDirection == AMBA_I2S_SLAVE) && (pCtrl->Mode == AMBA_I2S_MODE_I2S)) {
        pI2sReg->ClkCtrl.Ss = 0U;
        AmbaCSL_I2sSetWsBclkOutput(pI2sReg, 0U);
    } else {
        pI2sReg->ClkCtrl.Ss = 1U;
        AmbaCSL_I2sSetWsBclkOutput(pI2sReg, 1U);
    }
}

/**
 *  I2S_ConfigMode - Configure DAI mode
 *  @param[in] pI2sReg pointer to I2S registers
 *  @param[in] pCtrl I2S control information
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
static void I2S_ConfigMode(AMBA_I2S_REG_s *pI2sReg, const AMBA_I2S_CTRL_s *pCtrl)
{
    AmbaCSL_I2sConfigMode(pI2sReg, pCtrl->Mode);

    if ((pCtrl->Mode != AMBA_I2S_MODE_I2S) && (pCtrl->Mode != AMBA_I2S_MODE_MSB_EXTENDED)) {
        AmbaCSL_I2sSetRxWsInv(pI2sReg, 1U);
        AmbaCSL_I2sSetTxWsInv(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetRxWsInv(pI2sReg, 0U);
        AmbaCSL_I2sSetTxWsInv(pI2sReg, 0U);
    }

    if ((pCtrl->ClkDirection == AMBA_I2S_SLAVE) && (pCtrl->Mode == AMBA_I2S_MODE_I2S)) {
        AmbaCSL_I2sSetRxWsMst(pI2sReg, 0U);
        AmbaCSL_I2sSetTxWsMst(pI2sReg, 0U);
    } else {
        AmbaCSL_I2sSetRxWsMst(pI2sReg, 1U);
        AmbaCSL_I2sSetTxWsMst(pI2sReg, 1U);
    }

    if ((pCtrl->WordPrecision == 16U) && (pCtrl->TxCtrl.Unison == 1U)) {
        AmbaCSL_I2sSet24BitMuxCtrl(pI2sReg, 0U);
        AmbaCSL_I2sSetTxUnison(pI2sReg, 1U);
        AmbaCSL_I2sSetWordPosition(pI2sReg, 0U);
    } else {
        AmbaCSL_I2sSet24BitMuxCtrl(pI2sReg, 1U);
        AmbaCSL_I2sSetTxUnison(pI2sReg, 0U);
        AmbaCSL_I2sSetWordPosition(pI2sReg, (0x20U - pCtrl->WordPrecision));
    }
    AmbaCSL_I2sSetWordLength(pI2sReg, pCtrl->WordPrecision);
    AmbaCSL_I2sSetTxMono(pI2sReg, pCtrl->TxCtrl.Mono);

    if (pCtrl->TxCtrl.Mute == 1U) {
        AmbaCSL_I2sSetTxMute(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetTxMute(pI2sReg, 0U);
    }
    if (pCtrl->TxCtrl.Order == 1U) {
        AmbaCSL_I2sSetTxOrder(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetTxOrder(pI2sReg, 0U);
    }
    if (pCtrl->TxCtrl.Loopback == 1U) {
        AmbaCSL_I2sSetTxLoopback(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetTxLoopback(pI2sReg, 0U);
    }
    if (pCtrl->RxCtrl.Order == 1U) {
        AmbaCSL_I2sSetRxOrder(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetRxOrder(pI2sReg, 0U);
    }
    if (pCtrl->RxCtrl.Loopback == 1U) {
        AmbaCSL_I2sSetRxLoopback(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetRxLoopback(pI2sReg, 0U);
    }
}
#else   //CV2FS/CV22FS/CV28/H32/CV5/CV52
static void I2S_ConfigMode(AMBA_I2S_REG_s *pI2sReg, const AMBA_I2S_CTRL_s *pCtrl)
{
    AmbaCSL_I2sConfigMode(pI2sReg, pCtrl->Mode);

    if ((pCtrl->Mode != AMBA_I2S_MODE_I2S) && (pCtrl->Mode != AMBA_I2S_MODE_DSP)) {
        AmbaCSL_I2sSetRxWsInv(pI2sReg, 1U);
        AmbaCSL_I2sSetTxWsInv(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetRxWsInv(pI2sReg, 0U);
        AmbaCSL_I2sSetTxWsInv(pI2sReg, 0U);
    }

    if ((pCtrl->WordPrecision == 16U) && (pCtrl->TxCtrl.Unison == 1U)) {
        AmbaCSL_I2sSet24BitMuxCtrl(pI2sReg, 0U);
        AmbaCSL_I2sSetTxUnison(pI2sReg, 1U);
        AmbaCSL_I2sSetWordPosition(pI2sReg, 0U);
    } else {
        AmbaCSL_I2sSet24BitMuxCtrl(pI2sReg, 1U);
        AmbaCSL_I2sSetTxUnison(pI2sReg, 0U);
        AmbaCSL_I2sSetWordPosition(pI2sReg, (0x20U - pCtrl->WordPrecision));
    }
    AmbaCSL_I2sSetWordLength(pI2sReg, pCtrl->WordPrecision);
    AmbaCSL_I2sSetTxMono(pI2sReg, pCtrl->TxCtrl.Mono);

    if (pCtrl->TxCtrl.Mute == 1U) {
        AmbaCSL_I2sSetTxMute(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetTxMute(pI2sReg, 0U);
    }
    if (pCtrl->TxCtrl.Order == 1U) {
        AmbaCSL_I2sSetTxOrder(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetTxOrder(pI2sReg, 0U);
    }
    if (pCtrl->TxCtrl.Loopback == 1U) {
        AmbaCSL_I2sSetTxLoopback(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetTxLoopback(pI2sReg, 0U);
    }
    if (pCtrl->RxCtrl.Order == 1U) {
        AmbaCSL_I2sSetRxOrder(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetRxOrder(pI2sReg, 0U);
    }
    if (pCtrl->RxCtrl.Loopback == 1U) {
        AmbaCSL_I2sSetRxLoopback(pI2sReg, 1U);
    } else {
        AmbaCSL_I2sSetRxLoopback(pI2sReg, 0U);
    }
}
#endif

/**
 *  AmbaRTSL_I2sInit - I2s initializations
 *  @return error code
 */
UINT32 AmbaRTSL_I2sInit(void)
{
    ULONG base_addr;

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_I2S0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2S_Reg[0], &base_addr);
    base_addr = AMBA_CORTEX_A76_I2S1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2S_Reg[1], &base_addr);
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    base_addr = AMBA_CA53_I2S0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2S_Reg[0], &base_addr);
#else
    base_addr = AMBA_CORTEX_A53_I2S0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2S_Reg[0], &base_addr);
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV28)
    base_addr = AMBA_CORTEX_A53_I2S1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaI2S_Reg[1], &base_addr);
#endif
#endif

    return I2S_ERR_NONE;
}

/**
 *  AmbaRTSL_I2sConfig - Configure I2S
 *  @param[in] I2sPortIdx I2S port number
 *  @param[in] pCtrl I2S control information
 *  @return error code
 */
UINT32 AmbaRTSL_I2sConfig(UINT32 I2sPortIdx, const AMBA_I2S_CTRL_s *pCtrl)
{
    UINT32 RetVal = I2S_ERR_NONE;
    AMBA_I2S_REG_s *pI2sReg;

    if ((I2sPortIdx >= AMBA_NUM_I2S_CHANNEL) || (pCtrl == NULL)) {
        RetVal = I2S_ERR_ARG;
    } else {
        pI2sReg = pAmbaI2S_Reg[I2sPortIdx];

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        /* Enable CLK_AU Pad */
        AmbaCSL_DisableClkAuPad(0U);
#endif
        /* Disable Tx/Rx */
        AmbaCSL_I2sSetTxEnable(pI2sReg, 0U);
        AmbaCSL_I2sSetRxEnable(pI2sReg, 0U);

        /* Set Tx/Rx FIFO threshold */
        AmbaCSL_I2sSetTxFifoTh(pI2sReg, 0x10U);
        AmbaCSL_I2sSetRxFifoTh(pI2sReg, 0x40U);

        /* Reset FIFO */
        AmbaCSL_I2sTxFifoReset(pI2sReg);
        AmbaCSL_I2sRxFifoReset(pI2sReg);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
        AmbaCSL_I2sFifoSetSource(pI2sReg, 0U);
#endif
        AmbaCSL_I2sFifoReset(pI2sReg);

        I2S_ConfigFreq(pI2sReg, pCtrl);
        I2S_ConfigMode(pI2sReg, pCtrl);

        /* If DSP mode, set slot values, else 0 */
        if (pCtrl->Mode == AMBA_I2S_MODE_DSP) {
            AmbaCSL_I2sConfigSlot(pI2sReg, pCtrl->DspModeSlots);
        } else {
            AmbaCSL_I2sConfigSlot(pI2sReg, 0U);
        }

        if (pCtrl->TxCtrl.Shift == 16U) {
            AmbaCSL_I2sSetTxShiftCtrl(pI2sReg, 1U);
            AmbaCSL_I2sSetTxShift16bCtrl(pI2sReg, 1U);
        } else if (pCtrl->TxCtrl.Shift == 8U) {
            AmbaCSL_I2sSetTxShiftCtrl(pI2sReg, 1U);
            AmbaCSL_I2sSetTxShift16bCtrl(pI2sReg, 0U);
        } else {
            AmbaCSL_I2sSetTxShiftCtrl(pI2sReg, 0U);
            AmbaCSL_I2sSetTxShift16bCtrl(pI2sReg, 0U);
        }

        if (pCtrl->RxCtrl.Shift == 16U) {
            AmbaCSL_I2sSetRxShiftCtrl(pI2sReg, 1U);
            AmbaCSL_I2sSetRxShift16bCtrl(pI2sReg, 1U);
        } else if (pCtrl->RxCtrl.Shift == 8U) {
            AmbaCSL_I2sSetRxShiftCtrl(pI2sReg, 1U);
            AmbaCSL_I2sSetRxShift16bCtrl(pI2sReg, 0U);
        } else {
            AmbaCSL_I2sSetRxShiftCtrl(pI2sReg, 0U);
            AmbaCSL_I2sSetRxShift16bCtrl(pI2sReg, 0U);
        }

        if (pCtrl->Echo == 1U) {
            AmbaCSL_I2sSetRxEchoCtrl(pI2sReg, 1U);
        } else {
            AmbaCSL_I2sSetRxEchoCtrl(pI2sReg, 0U);
        }

        /* Configure audio channels */
        AmbaCSL_I2sSetNumChannels(pAmbaI2S_Reg[I2sPortIdx], AMBA_I2S_2_CHANNELS);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        AmbaCSL_I2sSetWsEnable(pI2sReg, 1U);
#endif
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2sTxGetDmaAddress - Get I2S Tx address for DMA
 *  @param[in] I2sPortIdx I2S port number
 *  @return error code
 */
UINT32 AmbaRTSL_I2sTxGetDmaAddress(UINT32 I2sPortIdx, void **pTxDmaAddress)
{
    UINT32 RetVal = I2S_ERR_NONE;
    const volatile UINT32 *pRegAddr;

    if (I2sPortIdx >= AMBA_NUM_I2S_CHANNEL) {
        RetVal = I2S_ERR_ARG;
    } else {
        pRegAddr = AmbaCSL_I2sGetTxDmaAddress(pAmbaI2S_Reg[I2sPortIdx]);
        AmbaMisra_TypeCast(&(*pTxDmaAddress), &pRegAddr);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2sTxResetFifo - Reset I2S Tx fifo
 *  @param[in] I2sPortIdx I2S port number
 *  @return error code
 */
UINT32 AmbaRTSL_I2sTxResetFifo(UINT32 I2sPortIdx)
{
    UINT32 RetVal = I2S_ERR_NONE;

    if (I2sPortIdx >= AMBA_NUM_I2S_CHANNEL) {
        RetVal = I2S_ERR_ARG;
    } else {
        AmbaCSL_I2sTxFifoReset(pAmbaI2S_Reg[I2sPortIdx]);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2sTxWriteFifo - Write data to I2S Tx FIFO
 *  @param[in] I2sPortIdx I2S port number
 *  @param[in] Data Data to write
 *  @return error code
 */
UINT32 AmbaRTSL_I2sTxWriteFifo(UINT32 I2sPortIdx, UINT32 Data)
{
    UINT32 RetVal = I2S_ERR_NONE;

    AmbaCSL_I2sWriteTxFifo(pAmbaI2S_Reg[I2sPortIdx], Data);

    return RetVal;
}

/**
 *  AmbaRTSL_I2sTxGetFifoStatus - Get I2S Tx FIFO status
 *  @param[in] I2sPortIdx I2S port number
 *  @param[out] pStatus pointer to the status
 *  @return error code
 */
UINT32 AmbaRTSL_I2sTxGetFifoStatus(UINT32 I2sPortIdx, UINT32 *pStatus)
{
    UINT32 RetVal = I2S_ERR_NONE;

    *pStatus = AmbaCSL_I2sGetTxFifoStatus(pAmbaI2S_Reg[I2sPortIdx]);

    return RetVal;
}

/**
 *  AmbaRTSL_I2sTxTrigger - Enable/Disable I2S Tx
 *  @param[in] I2sPortIdx I2S port number
 *  @param[in] Cmd Enable or Disable
 *  @return error code
 */
UINT32 AmbaRTSL_I2sTxTrigger(UINT32 I2sPortIdx, UINT32 Cmd)
{
    UINT32 RetVal = I2S_ERR_NONE;

    if (I2sPortIdx >= AMBA_NUM_I2S_CHANNEL) {
        RetVal = I2S_ERR_ARG;
    } else {
        if (Cmd == 1U) {
            AmbaCSL_I2sSetTxEnable(pAmbaI2S_Reg[I2sPortIdx], 1U);
        } else {
            AmbaCSL_I2sSetTxEnable(pAmbaI2S_Reg[I2sPortIdx], 0U);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2sRxGetDmaAddress - Get I2S Rx address for DMA
 *  @param[in] I2sPortIdx I2S port number
 *  @return error code
 */
UINT32 AmbaRTSL_I2sRxGetDmaAddress(UINT32 I2sPortIdx, void **pRxDmaAddress)
{
    UINT32 RetVal = I2S_ERR_NONE;
    const volatile UINT32 *pRegAddr;

    if (I2sPortIdx >= AMBA_NUM_I2S_CHANNEL) {
        RetVal = I2S_ERR_ARG;
    } else {
        pRegAddr = AmbaCSL_I2sGetRxDmaAddress(pAmbaI2S_Reg[I2sPortIdx]);
        AmbaMisra_TypeCast(&(*pRxDmaAddress), &pRegAddr);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2sRxResetFifo - Reset I2S Rx fifo
 *  @param[in] I2sPortIdx I2S port number
 *  @return error code
 */
UINT32 AmbaRTSL_I2sRxResetFifo(UINT32 I2sPortIdx)
{
    UINT32 RetVal = I2S_ERR_NONE;

    if (I2sPortIdx >= AMBA_NUM_I2S_CHANNEL) {
        RetVal = I2S_ERR_ARG;
    } else {
        AmbaCSL_I2sRxFifoReset(pAmbaI2S_Reg[I2sPortIdx]);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_I2sRxTrigger - Enable/Disable I2S Rx
 *  @param[in] I2sPortIdx I2S port number
 *  @param[in] Cmd Enable or Disable
 *  @return error code
 */
UINT32 AmbaRTSL_I2sRxTrigger(UINT32 I2sPortIdx, UINT32 Cmd)
{
    UINT32 RetVal = I2S_ERR_NONE;

    if (I2sPortIdx >= AMBA_NUM_I2S_CHANNEL) {
        RetVal = I2S_ERR_ARG;
    } else {
        if (Cmd == 1U) {
            AmbaCSL_I2sSetRxEnable(pAmbaI2S_Reg[I2sPortIdx], 1U);
        } else {
            AmbaCSL_I2sSetRxEnable(pAmbaI2S_Reg[I2sPortIdx], 0U);
        }
    }

    return RetVal;
}
