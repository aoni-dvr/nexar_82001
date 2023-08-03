/**
 *  @file AmbaRTSL_UART.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for UART RTSL APIs
 *
 */

#ifndef AMBA_RTSL_UART_H
#define AMBA_RTSL_UART_H

#ifndef AMBA_UART_DEF_H
#include "AmbaUART_Def.h"
#endif

/*
 * Defined in AmbaRTSL_UART.c
 */
UINT32 AmbaRTSL_UartInit(void);
UINT32 AmbaRTSL_UartInitIrq(AMBA_UART_ISR_f TxIsrCb, AMBA_UART_ISR_f RxIsrCb);

UINT32 AmbaRTSL_UartConfig(UINT32 UartCh, UINT32 SourceClk, UINT32 BaudRate, const AMBA_UART_CONFIG_s *pUartConfig);
UINT32 AmbaRTSL_UartSetFlowCtrl(UINT32 UartCh, UINT32 FlowCtrl);
UINT32 AmbaRTSL_UartHookRxReadyISR(UINT32 UartCh, AMBA_UART_ISR_f NotifyFunc, UINT32 NotifyFuncArg);
UINT32 AmbaRTSL_UartHookRxBuf(UINT32 UartCh, UINT32 BufSize, UINT8 *pBuf);
UINT32 AmbaRTSL_UartWrite(UINT32 UartCh, UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT32 *pActualTxSize);
UINT32 AmbaRTSL_UartRead(UINT32 UartCh, UINT32 RxDataSize, UINT8 *pRxDataBuf, UINT32 *pActualRxSize);
UINT32 AmbaRTSL_UartGetPreReadData(UINT32 UartCh, UINT8 *pRxData);
UINT32 AmbaRTSL_UartClearRxRingBuf(UINT32 UartCh);
UINT32 AmbaRTSL_UartGetStatus(UINT32 UartCh, UINT32 *pStatus);
UINT32 AmbaRTSL_UartGetTxEmpty(UINT32 UartCh, UINT32 *pTxEmpty);

void AmbaRTSL_UartRecoverTxHalt(UINT32 UartCh);
void AmbaRTSL_UartGetDataPortAddr(UINT32 UartCh, ULONG *pDataPortAddr);
void AmbaRTSL_UartSetTxDmaCtrl(UINT32 UartCh, UINT32 EnableFlag);
void AmbaRTSL_UartSetRxDmaCtrl(UINT32 UartCh, UINT32 EnableFlag);

#endif /* AMBA_RTSL_UART_H */
