/**
 *  @file AmbaUART.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for UART Middleware APIs
 *
 */

#ifndef AMBA_UART_H
#define AMBA_UART_H

#ifndef AMBA_UART_DEF_H
#include "AmbaUART_Def.h"
#endif

/*
 * Defined in AmbaUART.c
 */
UINT32 AmbaUART_Config(UINT32 UartCh, UINT32 BaudRate, const AMBA_UART_CONFIG_s *pUartConfig);
UINT32 AmbaUART_HookRxReadyNotifier(UINT32 UartCh, AMBA_UART_ISR_f NotifyFunc, UINT32 NotifyFuncArg);
UINT32 AmbaUART_HookDeferredRxBuf(UINT32 UartCh, UINT32 BufSize, UINT8 *pBuf);
UINT32 AmbaUART_ClearDeferredRxBuf(UINT32 UartCh);
UINT32 AmbaUART_Read(UINT32 UartCh, UINT32 FlowCtrlMode, UINT32 RxSize, UINT8 *pRxBuf, UINT32 *pActualRxSize, UINT32 TimeOut);
UINT32 AmbaUART_Write(UINT32 UartCh, UINT32 FlowCtrlMode, UINT32 TxSize, const UINT8 *pTxBuf, UINT32 *pActualTxSize, UINT32 TimeOut);
UINT32 AmbaUART_DmaRead(UINT32 UartCh, UINT32 FlowCtrlMode, UINT32 RxSize, UINT8 *pRxBuf, UINT32 *pActualRxSize, UINT32 TimeOut);
UINT32 AmbaUART_DmaWrite(UINT32 UartCh, UINT32 FlowCtrlMode, UINT32 TxSize, const UINT8 *pTxBuf, UINT32 *pActualTxSize, UINT32 TimeOut);
UINT32 AmbaUART_GetInfo(UINT32 UartCh, UINT32 *pStatus);

#endif /* AMBA_UART_H */