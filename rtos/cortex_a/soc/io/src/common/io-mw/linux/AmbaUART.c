/**
 *  @file AmbaUART.c
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
 *  @details UART APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaDrvEntry.h"
#include "AmbaCache.h"
#include "AmbaDMA.h"
#include "AmbaUART.h"

//#include "AmbaRTSL_UART.h"
//#include "AmbaRTSL_PLL.h"
//#include "AmbaRTSL_DMA.h"
/* Bit[31:24], Bit[23:16], Bit[15:8] and Bit[7:0] are reserved for eight UART channels */
#define AMBA_UART_FLAG_TX_HOLD_EMPTY    (UINT32)0x1U
#define AMBA_UART_FLAG_TX_TIMEOUT       (UINT32)0x100U
#define AMBA_UART_FLAG_RX_DATA_READY    (UINT32)0x10000U
#define AMBA_UART_FLAG_RX_TIMEOUT       (UINT32)0x1000000U




/**
 *  UART_TxGetDmaChanNo - Acquire a dma channel for the specified UART channel
 *  @param[in] UartCh UART Channel Number
 *  @param[out] pTxDmaChanNo DMA channel allocated for UART TX
 *  @return error code
 */

/**
 *  AmbaUART_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaUART_DrvEntry(void)
{
    return 1;//RetVal;
}

/**
 *  AmbaUART_Config - configure the speed and data frame structure of the specified UART channel
 *  @param[in] UartCh UART Channel Number
 *  @param[in] BaudRate Preset baud rate
 *  @param[in] pUartConfig pointer to the configurations
 *  @return error code
 */
UINT32 AmbaUART_Config(UINT32 UartCh, UINT32 BaudRate, const AMBA_UART_CONFIG_s *pUartConfig)
{
    UINT32 RetVal = UART_ERR_NONE;
    (void) UartCh;
    (void) BaudRate;
    (void) pUartConfig;
    return RetVal;
}

/**
 *  AmbaUART_HookRxReadyNotifier - The function registers a handler on RX data ready event of the specified UART channel
 *  @param[in] UartCh UART Channel Number
 *  @param[in] NotifyFunc Callback on RX data ready
 *  @param[in] NotifyFuncArg Optional argument of the callback
 *  @return error code
 */
UINT32 AmbaUART_HookRxReadyNotifier(UINT32 UartCh, AMBA_UART_ISR_f NotifyFunc, UINT32 NotifyFuncArg)
{
    UINT32 RetVal = UART_ERR_NONE;
    (void) UartCh;
    (void) NotifyFunc;
    (void) NotifyFuncArg;

    return RetVal;
}

/**
 *  AmbaUART_HookDeferredRxBuf - The function registers a deferred buffer to store the data from UART receiver.
 *  @note These data could be retrieved later by the AmbaUART_Read function
 *  @param[in] UartCh UART Channel Number
 *  @param[in] BufSize Number of bytes to store data frames
 *  @param[in] pBuf The buffer used to store data frames
 *  @return error code
 */
UINT32 AmbaUART_HookDeferredRxBuf(UINT32 UartCh, UINT32 BufSize, UINT8 *pBuf)
{
    UINT32 RetVal = UART_ERR_NONE;
    (void) UartCh;
    (void) BufSize;
    (void) pBuf;

    return RetVal;
}

/**
 *  AmbaUART_ClearDeferredRxBuf - The function resets the status of the deferred buffer.
 *  @param[in] UartCh UART Channel Number
 *  @return error code
 */
UINT32 AmbaUART_ClearDeferredRxBuf(UINT32 UartCh)
{
    UINT32 RetVal = UART_ERR_NONE;
    (void) UartCh;

    return RetVal;
}

/**
 *  UART_HandleRxData - Handle UART receiver
 *  @param[in] UartCh UART Channel Number
 *  @param[in] RxSize Data size in Byte
 *  @param[out] pRxBuf Pointer to the Rx data buffer
 *  @param[out] pTotalCount The total number of the RX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */

/**
 *  AmbaUART_Read - UART receiver
 *  @param[in] UartCh UART Channel Number
 *  @param[in] RxSize Data size in Byte
 *  @param[out] pRxBuf Pointer to the Rx data buffer
 *  @param[out] pActualRxSize The desired number of the RX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */

/**
 *  AmbaUART_Read - UART receiver
 *  @param[in] UartCh UART Channel Number
 *  @param[in] FlowCtrlMode Flow control mode
 *  @param[in] RxSize Data size in Byte
 *  @param[out] pRxBuf Pointer to the Rx data buffer
 *  @param[out] pActualRxSize The desired number of the RX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaUART_Read(UINT32 UartCh, UINT32 FlowCtrlMode, UINT32 RxSize, UINT8 *pRxBuf, UINT32 *pActualRxSize, UINT32 TimeOut)
{
    UINT32 RetVal = UART_ERR_NONE;
    (void) UartCh;
    (void) FlowCtrlMode;
    (void) RxSize;
    (void) pRxBuf;
    (void) pActualRxSize;
    (void) TimeOut;

    return RetVal;
}

/**
 *   UART_HandleTxData - Handle UART transmitter
 *   @param[in] UartCh UART Channel Number
 *   @param[in] TxSize The desired number of the TX data
 *   @param[in] pTxBuf pointer to the Tx data buffer
 *   @param[out] pTotalCount The total number of the TX data
 *   @param[in] TimeOut Time out value
 *   @return error code
 */

/**
 *  UART_TxData - UART transmitter
 *  @param[in] UartCh UART Channel Number
 *  @param[in] TxSize The desired number of the TX data
 *  @param[in] pTxBuf pointer to the Tx data buffer
 *  @param[out] pActualRxSize The desired number of the TX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */

/**
 *  AmbaUART_Write - UART transmitter
 *  @param[in] UartCh UART Channel Number
 *  @param[in] FlowCtrlMode Flow control mode
 *  @param[in] TxSize The desired number of the TX data
 *  @param[in] pTxBuf pointer to the Tx data buffer
 *  @param[out] pActualRxSize The desired number of the TX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaUART_Write(UINT32 UartCh, UINT32 FlowCtrlMode, UINT32 TxSize, const UINT8 *pTxBuf, UINT32 *pActualTxSize, UINT32 TimeOut)
{
    UINT32 RetVal = UART_ERR_NONE;
    (void) UartCh;
    (void) FlowCtrlMode;
    (void) TxSize;
    (void) pTxBuf;
    (void) pActualTxSize;
    (void) TimeOut;

    return RetVal;
}

/**
 *  AmbaUART_DmaRead - UART receiver with DMA
 *  @param[in] UartCh UART Channel Number
 *  @param[in] FlowCtrlMode Flow control mode
 *  @param[in] RxSize Data size in Byte
 *  @param[out] pRxBuf Pointer to the Rx data buffer
 *  @param[out] pActualRxSize The desired number of the RX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaUART_DmaRead(UINT32 UartCh, UINT32 FlowCtrlMode, UINT32 RxSize, UINT8 *pRxBuf, UINT32 *pActualRxSize, UINT32 TimeOut)
{
    UINT32 RetVal = UART_ERR_NONE;
    (void) UartCh;
    (void) FlowCtrlMode;
    (void) RxSize;
    (void) pRxBuf;
    (void) pActualRxSize;
    (void) TimeOut;
    return RetVal;
}

/**
 *  AmbaUART_DmaWrite - UART transmitter with DMA
 *  @param[in] UartCh UART Channel Number
 *  @param[in] FlowCtrlMode Flow control mode
 *  @param[in] TxSize The desired number of the TX data
 *  @param[in] pTxBuf pointer to the Tx data buffer
 *  @param[out] pActualRxSize The desired number of the TX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaUART_DmaWrite(UINT32 UartCh, UINT32 FlowCtrlMode, UINT32 TxSize, const UINT8 *pTxBuf, UINT32 *pActualTxSize, UINT32 TimeOut)
{
    (void) UartCh;
    (void) FlowCtrlMode;
    (void) TxSize;
    (void) pTxBuf;
    (void) pActualTxSize;
    (void) TimeOut;
    return 1;
}

/**
 *  AmbaUART_GetInfo - The function returns all the information about the spec master channel
 *  @param[in] UartCh UART Channel Number
 *  @param[out] pStatus Pointer to the current status
 *  @return error code
 */
UINT32 AmbaUART_GetInfo(UINT32 UartCh, UINT32 *pStatus)
{
    UINT32 RetVal = UART_ERR_NONE;
    (void) UartCh;
    (void) pStatus;

    return RetVal;
}


