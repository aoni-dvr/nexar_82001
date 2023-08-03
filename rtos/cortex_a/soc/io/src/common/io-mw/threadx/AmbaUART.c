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
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22)
#include "AmbaSYS.h"
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#include "AmbaRTSL_PLL.h"
#endif

#include "AmbaMisraFix.h"

#include "AmbaDrvEntry.h"
#include "AmbaCache.h"
#include "AmbaDMA.h"
#include "AmbaUART.h"
#include "AmbaRTSL_UART.h"
#include "AmbaRTSL_DMA.h"

/* Bit[31:24], Bit[23:16], Bit[15:8] and Bit[7:0] are reserved for eight UART channels */
#define AMBA_UART_FLAG_TX_HOLD_EMPTY    (UINT32)0x1U
#define AMBA_UART_FLAG_TX_TIMEOUT       (UINT32)0x100U
#define AMBA_UART_FLAG_RX_DATA_READY    (UINT32)0x10000U
#define AMBA_UART_FLAG_RX_TIMEOUT       (UINT32)0x1000000U

static AMBA_KAL_EVENT_FLAG_t AmbaUartEventFlag;
static AMBA_KAL_MUTEX_t AmbaUartTxMutex[AMBA_NUM_UART_CHANNEL];
static AMBA_KAL_MUTEX_t AmbaUartRxMutex[AMBA_NUM_UART_CHANNEL];
static AMBA_KAL_SEMAPHORE_t AmbaUartRxSem[AMBA_NUM_UART_CHANNEL];   /* Rx Counting Semaphore */
static AMBA_KAL_TIMER_t AmbaUartTxTimer[AMBA_NUM_UART_CHANNEL];
static AMBA_KAL_TIMER_t AmbaUartRxTimer[AMBA_NUM_UART_CHANNEL];
static char AmbaUartSemaphoreName[20] = "AmbaUartSemaphore";

static UINT32 AmbaUartRxDeffered[AMBA_NUM_UART_CHANNEL];

static UINT32 AmbaUartRxISRErr[AMBA_NUM_UART_CHANNEL];
static UINT32 AmbaUartTxISRErr[AMBA_NUM_UART_CHANNEL];
static UINT32 AmbaUartTxTimeOutFuncErr[AMBA_NUM_UART_CHANNEL];
static UINT32 AmbaUartRxTimeOutFuncErr[AMBA_NUM_UART_CHANNEL];

static void UART_TxISR(UINT32 UartCh);
static void UART_RxISR(UINT32 UartCh);
static void UART_TxTimeOutFunc(UINT32 UartCh);
static void UART_RxTimeOutFunc(UINT32 UartCh);
static UINT32 UART_DrvInitMutex(void);
static UINT32 UART_DrvInitSemaphore(void);
static UINT32 UART_DrvInitTimer(void);
static UINT32 UART_TxWriteFlow(UINT32 UartCh, UINT32 TxSize, const UINT8 *pTxBuf, UINT32 *pTotalCount);
static UINT32 UART_TxFifoEmpty(UINT32 UartCh, UINT32 TimeOut, UINT32 PreRetVal);
static UINT32 UART_RxReadBuffer(UINT32 UartCh, UINT32 RxSize, UINT8 *pRxBuf, UINT32 *pRemainCnt, UINT32 *pTotalCnt, UINT32 TimeOut);

/**
 *  UART_TxGetDmaChanNo - Acquire a dma channel for the specified UART channel
 *  @param[in] UartCh UART Channel Number
 *  @param[out] pTxDmaChanNo DMA channel allocated for UART TX
 *  @return error code
 */
static UINT32 UART_TxGetDmaChanNo(UINT32 UartCh, UINT32 *pTxDmaChanNo)
{
    static const UINT32 TxDmaType[AMBA_NUM_UART_CHANNEL] = {
        [AMBA_UART_APB_CHANNEL0] = 0xffffffffU,
        [AMBA_UART_AHB_CHANNEL0] = AMBA_DMA_CHANNEL_UART0_TX,
        [AMBA_UART_AHB_CHANNEL1] = AMBA_DMA_CHANNEL_UART1_TX,
        [AMBA_UART_AHB_CHANNEL2] = AMBA_DMA_CHANNEL_UART2_TX,
        [AMBA_UART_AHB_CHANNEL3] = AMBA_DMA_CHANNEL_UART3_TX,
    };
    UINT32 RetVal = UART_ERR_NONE;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (pTxDmaChanNo == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        if (TxDmaType[UartCh] >= AMBA_NUM_DMA_CHANNEL_TYPE) {
            RetVal = UART_ERR_UNAVAIL;
        } else {
            if (AmbaDMA_ChannelAllocate(TxDmaType[UartCh], pTxDmaChanNo) != DMA_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;
            } else {
                if (*pTxDmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
                    RetVal = UART_ERR_UNAVAIL;
                }
            }
        }
    }

    return RetVal;
}

/**
 *  UART_RxGetDmaChanNo - Acquire a dma channel for the specified UART channel
 *  @param[in] UartCh UART Channel Number
 *  @param[out] pRxDmaChanNo DMA channel allocated for UART RX
 *  @return error code
 */
static UINT32 UART_RxGetDmaChanNo(UINT32 UartCh, UINT32 *pRxDmaChanNo)
{
    static const UINT32 RxDmaType[AMBA_NUM_UART_CHANNEL] = {
        [AMBA_UART_APB_CHANNEL0] = 0xffffffffU,
        [AMBA_UART_AHB_CHANNEL0] = AMBA_DMA_CHANNEL_UART0_RX,
        [AMBA_UART_AHB_CHANNEL1] = AMBA_DMA_CHANNEL_UART1_RX,
        [AMBA_UART_AHB_CHANNEL2] = AMBA_DMA_CHANNEL_UART2_RX,
        [AMBA_UART_AHB_CHANNEL3] = AMBA_DMA_CHANNEL_UART3_RX,
    };
    UINT32 RetVal = UART_ERR_NONE;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (pRxDmaChanNo == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        if (RxDmaType[UartCh] >= AMBA_NUM_DMA_CHANNEL_TYPE) {
            RetVal = UART_ERR_UNAVAIL;
        } else {
            if (AmbaDMA_ChannelAllocate(RxDmaType[UartCh], pRxDmaChanNo) != DMA_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;
            } else {
                if (*pRxDmaChanNo >= AMBA_NUM_DMA_CHANNEL) {
                    RetVal = UART_ERR_UNAVAIL;
                }
            }
        }
    }

    return RetVal;
}

/**
 *  UART_DmaRead - UART receiver with dma
 *  @param[in] UartCh UART Channel Number
 *  @param[in] DmaChan Allocated DMA channel
 *  @param[in] RxSize Data size in Byte
 *  @param[out] pRxBuf Pointer to the Rx data buffer
 *  @param[out] pActualRxSize The desired number of the RX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
static UINT32 UART_DmaRead(UINT32 UartCh, UINT32 DmaChan, UINT32 RxDataSize, UINT8 *pRxDataBuf, UINT32 *pActualRxSize, UINT32 TimeOut)
{
    UINT32 RetVal = UART_ERR_NONE;
    AMBA_DMA_DESC_s RxDmaDesc;
    UINT32 DmaByteCount = 0U;
    ULONG DataBufAddr, DataPortAddr;
    void *pSrcAddr = NULL;
    UINT32 ActualCount = 0;
    UINT32 ResidueDataCount = 0U;

    /* Source is from UART buf registers */
    AmbaRTSL_UartGetDataPortAddr(UartCh, &DataPortAddr);
    AmbaMisra_TypeCast(&pSrcAddr, &DataPortAddr);

    /* Destination is to outside rx buffer */

    RxDmaDesc.pSrcAddr = pSrcAddr;
    RxDmaDesc.pDstAddr = pRxDataBuf;
    RxDmaDesc.pNextDesc = NULL;
    RxDmaDesc.pStatus = NULL;
    RxDmaDesc.DataSize = RxDataSize;

    RxDmaDesc.Ctrl.StopOnError = 1U;
    RxDmaDesc.Ctrl.IrqOnError = 0U;
    RxDmaDesc.Ctrl.IrqOnDone = 0U;
    RxDmaDesc.Ctrl.BusBlockSize = DMA_BUS_BLOCK_8BYTE;
    RxDmaDesc.Ctrl.BusDataSize = DMA_BUS_DATA_1BYTE;
    RxDmaDesc.Ctrl.NoBusAddrInc = 1U;
    RxDmaDesc.Ctrl.ReadMem = 0U;
    RxDmaDesc.Ctrl.WriteMem = 1U;
    RxDmaDesc.Ctrl.EndOfChain = 1U;

    /* Sync cache data to DRAM and invalidate cache */
    AmbaMisra_TypeCast(&DataBufAddr, &pRxDataBuf);
    (void)AmbaCache_DataFlush(DataBufAddr, RxDataSize);

    /* Do DMA transfer */
    if (AmbaDMA_Transfer(DmaChan, &RxDmaDesc) != OK) {
        RetVal = UART_ERR_UNAVAIL;
    } else {
        AmbaRTSL_UartSetRxDmaCtrl(UartCh, 1U);

        if (AmbaDMA_Wait(DmaChan, TimeOut) != DMA_ERR_NONE) {
            if (AmbaRTSL_DmaStop(DmaChan) != DMA_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;
            }
        } else {
            RetVal = UART_ERR_TIMEOUT;
        }

        AmbaRTSL_UartSetRxDmaCtrl(UartCh, 0U);
    }

    if ((pActualRxSize != NULL) && (RetVal == UART_ERR_NONE)) {
        if (AmbaRTSL_DmaGetTrfByteCount(DmaChan, &DmaByteCount) == DMA_ERR_NONE) {
            (void)AmbaCache_DataInvalidate(DataBufAddr, RxDataSize);
            /* Due to DMA burst size has 8 bytes limitation, if DMA timeout, there could some bytes still left in UART FIFO.
             * So, let's uses CPU to move the remaining bytes from UART FIFO to pRxDataBuf.
             */
            if (DmaByteCount < RxDataSize) {
                /* fetch residue data */
                ResidueDataCount = AmbaRTSL_UartGetRxFifoDataSize(UartCh);

                if (ResidueDataCount > (RxDataSize - DmaByteCount)) {
                    ResidueDataCount = RxDataSize - DmaByteCount;
                }

                RetVal = AmbaRTSL_UartRead(UartCh, ResidueDataCount, &pRxDataBuf[DmaByteCount], &ActualCount);
            }

            *pActualRxSize = (DmaByteCount + ActualCount);

        } else {
            RetVal = UART_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  UART_DmaWrite - UART transmitter with dma
 *  @param[in] UartCh UART Channel Number
 *  @param[in] DmaChan Allocated DMA channel
 *  @param[in] TxSize The desired number of the TX data
 *  @param[in] pTxBuf pointer to the Tx data buffer
 *  @param[out] pActualRxSize The desired number of the TX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
static UINT32 UART_DmaWrite(UINT32 UartCh, UINT32 DmaChan, UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT32 *pActualTxSize, UINT32 TimeOut)
{
    UINT32 RetVal = UART_ERR_NONE;
    AMBA_DMA_DESC_s TxDmaDesc;
    UINT32 DmaByteCount = 0U;
    ULONG DataBufAddr, DataPortAddr;
    void *pSrcAddr = NULL, *pDstAddr = NULL;

    /* Source is from outside tx buffer */
    AmbaMisra_TypeCast(&pSrcAddr, &pTxDataBuf);

    /* Destination is to UART buf registers */
    AmbaRTSL_UartGetDataPortAddr(UartCh, &DataPortAddr);
    AmbaMisra_TypeCast(&pDstAddr, &DataPortAddr);

    TxDmaDesc.pSrcAddr = pSrcAddr;
    TxDmaDesc.pDstAddr = pDstAddr;
    TxDmaDesc.pNextDesc = NULL;
    TxDmaDesc.pStatus = NULL;
    TxDmaDesc.DataSize = TxDataSize;

    TxDmaDesc.Ctrl.StopOnError = 1U;
    TxDmaDesc.Ctrl.IrqOnError = 0U;
    TxDmaDesc.Ctrl.IrqOnDone = 0U;
    TxDmaDesc.Ctrl.BusBlockSize = DMA_BUS_BLOCK_8BYTE;
    TxDmaDesc.Ctrl.BusDataSize = DMA_BUS_DATA_1BYTE;
    TxDmaDesc.Ctrl.NoBusAddrInc = 1U;
    TxDmaDesc.Ctrl.ReadMem = 1U;
    TxDmaDesc.Ctrl.WriteMem = 0U;
    TxDmaDesc.Ctrl.EndOfChain = 1U;

    /* Sync cache data to DRAM and invalidate cache */
    AmbaMisra_TypeCast(&DataBufAddr, &pTxDataBuf);
    (void)AmbaCache_DataFlush(DataBufAddr, TxDataSize);

    /* Do DMA transfer */
    if (AmbaDMA_Transfer(DmaChan, &TxDmaDesc) != OK) {
        RetVal = UART_ERR_UNAVAIL;
    } else {
        AmbaRTSL_UartSetTxDmaCtrl(UartCh, 1U);

        /* Wait until the transmission of Tx data is done */
        if (AmbaDMA_Wait(DmaChan, TimeOut) != DMA_ERR_NONE) {
            if (AmbaRTSL_DmaStop(DmaChan) != DMA_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;
            }
        }

        AmbaRTSL_UartSetTxDmaCtrl(UartCh, 0U);
    }

    if ((pActualTxSize != NULL) && (RetVal == UART_ERR_NONE)) {
        if (AmbaRTSL_DmaGetTrfByteCount(DmaChan, &DmaByteCount) == DMA_ERR_NONE) {
            *pActualTxSize = DmaByteCount;
        } else {
            RetVal = UART_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaUART_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaUART_DrvEntry(void)
{
    static char AmbaUartEventFlagName[20] = "AmbaUartEventFlag";
    UINT32 RetVal = UART_ERR_NONE;

    if (AmbaKAL_EventFlagCreate(&AmbaUartEventFlag, AmbaUartEventFlagName) != KAL_ERR_NONE) {
        RetVal = UART_ERR_UNEXPECTED;  /* should never happen */
    }

    if (RetVal == UART_ERR_NONE) {
        RetVal = UART_DrvInitMutex();
    }

    if (RetVal == UART_ERR_NONE) {
        RetVal = UART_DrvInitSemaphore();
    }

    if (RetVal == UART_ERR_NONE) {
        RetVal = UART_DrvInitTimer();
    }

    if (RetVal == UART_ERR_NONE) {
        (void)AmbaRTSL_UartInit();
        (void)AmbaRTSL_UartInitIrq(UART_TxISR, UART_RxISR);
    }

    return RetVal;
}

/**
 *  UART_DrvInitMutex - Initializes Mutex
 *  @return error code
 *  @note this function is an internal using only API
 */
static UINT32 UART_DrvInitMutex(void)
{
    static char AmbaUartTxMutexName[16] = "AmbaUartTxMutex";
    static char AmbaUartRxMutexName[16] = "AmbaUartRxMutex";
    UINT32 i, RetVal = UART_ERR_NONE;

    for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
        if (AmbaKAL_MutexCreate(&AmbaUartTxMutex[i], AmbaUartTxMutexName) != KAL_ERR_NONE) {
            RetVal = UART_ERR_UNEXPECTED;  /* should never happen ! */
            break;
        }
    }

    if (RetVal == UART_ERR_NONE) {
        for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
            if (AmbaKAL_MutexCreate(&AmbaUartRxMutex[i], AmbaUartRxMutexName) != KAL_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;  /* should never happen ! */
                break;
            }
        }
    }

    return RetVal;
}

/**
 *  UART_DrvInitSemaphore - Initializes Semaphore
 *  @return error code
 *  @note this function is an internal using only API
 */
static UINT32 UART_DrvInitSemaphore(void)
{
    UINT32 i, RetVal = UART_ERR_NONE;

    for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
        if (AmbaKAL_SemaphoreCreate(&AmbaUartRxSem[i], AmbaUartSemaphoreName, 0U) != KAL_ERR_NONE) {
            RetVal = UART_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  UART_DrvInitTimer - Initializes Timer
 *  @return error code
 *  @note this function is an internal using only API
 */
static UINT32 UART_DrvInitTimer(void)
{
    static char AmbaUartTxTimerName[16] = "AmbaUartTxTimer";
    static char AmbaUartRxTimerName[16] = "AmbaUartRxTimer";
    UINT32 i, RetVal = UART_ERR_NONE;

    for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
        if (AmbaKAL_TimerCreate(&AmbaUartTxTimer[i], AmbaUartTxTimerName, UART_TxTimeOutFunc, i, 10U * AMBA_KAL_SYS_TICK_MS, 0U, 0U) != KAL_ERR_NONE) {
            RetVal = UART_ERR_UNEXPECTED;  /* should never happen ! */
            break;
        }
    }

    if (RetVal == UART_ERR_NONE) {
        for (i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
            if (AmbaKAL_TimerCreate(&AmbaUartRxTimer[i], AmbaUartRxTimerName, UART_RxTimeOutFunc, i, 10U * AMBA_KAL_SYS_TICK_MS, 0U, 0U) != KAL_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;  /* should never happen ! */
                break;
            }
        }
    }

    return RetVal;
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
    UINT32 SourceClk;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (pUartConfig == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaUartTxMutex[UartCh], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = UART_ERR_MUTEX;
        } else {
            if (AmbaKAL_MutexTake(&AmbaUartRxMutex[UartCh], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
                RetVal = UART_ERR_MUTEX;

                if (AmbaKAL_MutexGive(&AmbaUartTxMutex[UartCh]) != KAL_ERR_NONE) {
                    /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                    RetVal = UART_ERR_UNEXPECTED;
                }
            }
        }

        if (RetVal == UART_ERR_NONE) {
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22)
            (void)AmbaSYS_GetIoClkFreq((UINT32)AMBA_CLK_UARTAPB, &SourceClk);
            RetVal = AmbaRTSL_UartConfig(UartCh, SourceClk, BaudRate, pUartConfig);
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV25)
            SourceClk = 24000000U;
            RetVal = AmbaRTSL_UartConfig(UartCh, SourceClk, BaudRate, pUartConfig);
#else   //CV2FS/CV22FS
            if (UartCh == AMBA_UART_APB_CHANNEL0) {
                SourceClk = AmbaRTSL_PllGetUartApbClk();
            } else if (UartCh == AMBA_UART_AHB_CHANNEL0) {
                SourceClk = AmbaRTSL_PllGetUart0Clk();
            } else if (UartCh == AMBA_UART_AHB_CHANNEL1) {
                SourceClk = AmbaRTSL_PllGetUart1Clk();
            } else if (UartCh == AMBA_UART_AHB_CHANNEL2) {
                SourceClk = AmbaRTSL_PllGetUart2Clk();
            } else {
                SourceClk = AmbaRTSL_PllGetUart3Clk();
            }
            RetVal = AmbaRTSL_UartConfig(UartCh, SourceClk, BaudRate, pUartConfig);
#endif

            if (AmbaKAL_MutexGive(&AmbaUartRxMutex[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
            if (AmbaKAL_MutexGive(&AmbaUartTxMutex[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }
    }

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

    if (UartCh >= AMBA_NUM_UART_CHANNEL) {
        RetVal = UART_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaUartRxMutex[UartCh], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = UART_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_UartHookRxReadyISR(UartCh, NotifyFunc, NotifyFuncArg);

            if (AmbaKAL_MutexGive(&AmbaUartRxMutex[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }
    }

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

    if (UartCh >= AMBA_NUM_UART_CHANNEL) {
        RetVal = UART_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaUartRxMutex[UartCh], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = UART_ERR_MUTEX;
        } else {
            /* Reset Counting Semaphore */
            if (AmbaKAL_SemaphoreDelete(&AmbaUartRxSem[UartCh]) != KAL_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;
            } else {
                AmbaUartRxDeffered[UartCh] = 0U;
                if ((pBuf == NULL) || (BufSize == 0U)) {
                    RetVal = AmbaRTSL_UartHookRxBuf(UartCh, 0U, NULL);
                } else {
                    if (AmbaKAL_SemaphoreCreate(&AmbaUartRxSem[UartCh], AmbaUartSemaphoreName, 0U) != KAL_ERR_NONE) {
                        RetVal = UART_ERR_UNEXPECTED;
                    } else {
                        RetVal = AmbaRTSL_UartHookRxBuf(UartCh, BufSize, pBuf);
                        AmbaUartRxDeffered[UartCh] = 1U;
                    }
                }
            }

            if (AmbaKAL_MutexGive(&AmbaUartRxMutex[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }
    }

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

    if (UartCh >= AMBA_NUM_UART_CHANNEL) {
        RetVal = UART_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaUartRxMutex[UartCh], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = UART_ERR_MUTEX;
        } else {
            /* Reset Counting Semaphore */
            if (AmbaKAL_SemaphoreDelete(&AmbaUartRxSem[UartCh]) != KAL_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;
            } else {
                if (AmbaKAL_SemaphoreCreate(&AmbaUartRxSem[UartCh], AmbaUartSemaphoreName, 0U) != KAL_ERR_NONE) {
                    RetVal = UART_ERR_UNEXPECTED;
                } else {
                    RetVal = AmbaRTSL_UartClearRxRingBuf(UartCh);
                }
            }

            if (AmbaKAL_MutexGive(&AmbaUartRxMutex[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaUART_Read - UART receiver
 *  @param[in] UartCh UART Channel Number
 *  @param[in] RxSize Data size in Byte
 *  @param[out] pRxBuf Pointer to the Rx data buffer
 *  @param[out] pActualRxSize The desired number of the RX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
static UINT32 UART_RxData(UINT32 UartCh, UINT32 RxSize, UINT8 *pRxBuf, UINT32 *pActualRxSize, UINT32 TimeOut)
{
    UINT32 RetVal = UART_ERR_NONE;
    UINT32 RemainCount = RxSize, TotalCount = 0U;

    if (AmbaKAL_EventFlagClear(&AmbaUartEventFlag, (AMBA_UART_FLAG_RX_DATA_READY | AMBA_UART_FLAG_RX_TIMEOUT) << UartCh) != KAL_ERR_NONE) {
        RetVal = UART_ERR_UNEXPECTED;
    } else {
        AmbaUartRxISRErr[UartCh]            = UART_ERR_NONE;    /* clear error status reported by interrupt handler */
        AmbaUartRxTimeOutFuncErr[UartCh]    = UART_ERR_NONE;    /* clear error status reported by timeout function */

        if (TimeOut == KAL_WAIT_NEVER) {
            UART_RxTimeOutFunc(UartCh);
        } else {
            if (AmbaKAL_TimerChange(&AmbaUartRxTimer[UartCh], TimeOut, 0U, KAL_START_AUTO) != KAL_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;
            }
        }

        if (RetVal == UART_ERR_NONE) {
            RetVal = UART_RxReadBuffer(UartCh, RxSize, pRxBuf, &RemainCount, &TotalCount, TimeOut);

            if (AmbaKAL_TimerStop(&AmbaUartRxTimer[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }

        if (AmbaUartRxISRErr[UartCh] != UART_ERR_NONE) {            /* check if any error reported in interrupt handler */
            RetVal = AmbaUartRxISRErr[UartCh];
        }

        if (AmbaUartRxTimeOutFuncErr[UartCh] != UART_ERR_NONE) {    /* check if any error reported in timeout function */
            RetVal = AmbaUartRxTimeOutFuncErr[UartCh];
        }
    }

    if (pActualRxSize != NULL) {
        *pActualRxSize = TotalCount;
    }

    return RetVal;
}

/**
 *  AmbaUART_Read - UART receiver
 *  @param[in] UartCh UART Channel Number
 *  @param[in] RxSize Data size in Byte
 *  @param[in] pRemainCnt Remain Count
 *  @param[out] pRxBuf Pointer to the Rx data buffer
 *  @param[out] pRemainCnt The Remain number of the RX data
 *  @param[out] pTotalCnt The total number of the RX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
static UINT32 UART_RxReadBuffer(UINT32 UartCh, UINT32 RxSize, UINT8 *pRxBuf, UINT32 *pRemainCnt, UINT32 *pTotalCnt, UINT32 TimeOut)
{
    UINT32 KalRetVal, RetVal = UART_ERR_NONE;
    UINT32 RemainCount = RxSize, ActualCount = 0U, TotalCount = 0U;

    while (RetVal == UART_ERR_NONE) {
        if (AmbaUartRxDeffered[UartCh] != 0U) {
            /* Fetch data from deffered rx buffer */
            KalRetVal = AmbaKAL_SemaphoreTake(&AmbaUartRxSem[UartCh], TimeOut);
            if (KalRetVal == KAL_ERR_TIMEOUT) {
                RetVal = UART_ERR_TIMEOUT;
            } else if (KalRetVal != KAL_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;
            } else {
                RetVal = AmbaRTSL_UartGetPreReadData(UartCh, &pRxBuf[TotalCount]);
                if (RetVal == UART_ERR_NONE) {
                    RemainCount --;
                    TotalCount ++;
                } else {
                    RemainCount = 0U;
                }
            }
        } else {
            /* Get data immediately from hardware register */
            if (AmbaKAL_EventFlagGet(&AmbaUartEventFlag, AMBA_UART_FLAG_RX_DATA_READY << UartCh, 0x1U, 0x1U, NULL, TimeOut) == KAL_ERR_NONE) {
                RetVal = AmbaRTSL_UartRead(UartCh, RemainCount, &pRxBuf[TotalCount], &ActualCount);
                if (RetVal == UART_ERR_NONE) {
                    RemainCount -= ActualCount;
                    TotalCount += ActualCount;
                } else {
                    RemainCount = 0U;
                }
            }
        }

        if (RemainCount == 0U) {
            break;
        }

        if (AmbaKAL_EventFlagGet(&AmbaUartEventFlag, AMBA_UART_FLAG_RX_TIMEOUT << UartCh, 0x1U, 0x1U, NULL, KAL_WAIT_NEVER) == KAL_ERR_NONE) {
            RetVal = UART_ERR_TIMEOUT;
        } else {
            /* this function always returns success so that we can ignore its return value */
            (void)AmbaKAL_TaskYield();
        }
    }

    *pRemainCnt = RemainCount;
    *pTotalCnt = TotalCount;

    return RetVal;
}

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
    UINT32 ActualCount = 0U;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (RxSize == 0U) || (pRxBuf == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaUartRxMutex[UartCh], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = UART_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_UartSetFlowCtrl(UartCh, FlowCtrlMode);
            if (RetVal == UART_ERR_NONE) {
                RetVal = UART_RxData(UartCh, RxSize, pRxBuf, &ActualCount, TimeOut);
            }

            if (AmbaKAL_MutexGive(&AmbaUartRxMutex[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }

        if (pActualRxSize != NULL) {
            *pActualRxSize = ActualCount;
        }
    }

    return RetVal;
}

/**
 *  UART_TxData - UART transmitter
 *  @param[in] UartCh UART Channel Number
 *  @param[in] TxSize The desired number of the TX data
 *  @param[in] pTxBuf pointer to the Tx data buffer
 *  @param[out] pActualRxSize The desired number of the TX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
static UINT32 UART_TxData(UINT32 UartCh, UINT32 TxSize, const UINT8 *pTxBuf, UINT32 *pActualTxSize, UINT32 TimeOut)
{
    UINT32 RetVal = UART_ERR_NONE;
    UINT32 TotalCount = 0U;

    if (AmbaKAL_EventFlagClear(&AmbaUartEventFlag, (UINT32)(AMBA_UART_FLAG_TX_HOLD_EMPTY | AMBA_UART_FLAG_TX_TIMEOUT) << UartCh) != KAL_ERR_NONE) {
        RetVal = UART_ERR_UNEXPECTED;
    } else {
        AmbaUartTxISRErr[UartCh]            = UART_ERR_NONE;    /* clear error status reported by interrupt handler */
        AmbaUartTxTimeOutFuncErr[UartCh]    = UART_ERR_NONE;    /* clear error status reported by timeout function */

        if (TimeOut == KAL_WAIT_NEVER) {
            UART_TxTimeOutFunc(UartCh);
        } else {
            if (AmbaKAL_TimerChange(&AmbaUartTxTimer[UartCh], TimeOut, 0U, KAL_START_AUTO) != KAL_ERR_NONE) {
                RetVal = UART_ERR_UNEXPECTED;
            }
        }

        if (RetVal == UART_ERR_NONE) {
            RetVal = UART_TxWriteFlow(UartCh, TxSize, pTxBuf, &TotalCount);

            if (TotalCount != 0U) {
                RetVal =  UART_TxFifoEmpty(UartCh, TimeOut, RetVal);
            }

            if (AmbaKAL_TimerStop(&AmbaUartTxTimer[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }

        if (AmbaUartTxISRErr[UartCh] != UART_ERR_NONE) {            /* check if any error reported in interrupt handler */
            RetVal = AmbaUartTxISRErr[UartCh];
        }

        if (AmbaUartTxTimeOutFuncErr[UartCh] != UART_ERR_NONE) {    /* check if any error reported in timeout function */
            RetVal = AmbaUartTxTimeOutFuncErr[UartCh];
        }
    }

    if (pActualTxSize != NULL) {
        *pActualTxSize = TotalCount;
    }

    return RetVal;
}

/**
 *  UART_TxWriteFlow - UART write data flow
 *  @param[in] UartCh UART Channel Number
 *  @param[in] TxSize The desired number of the TX data
 *  @param[in] pTxBuf pointer to the Tx data buffer
 *  @param[out] pTotalCount The total number of the TX data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
static UINT32 UART_TxWriteFlow(UINT32 UartCh, UINT32 TxSize, const UINT8 *pTxBuf, UINT32 *pTotalCount)
{
    UINT32 RetVal = UART_ERR_NONE;
    UINT32 RemainCount = TxSize, ActualCount = 0U, TotalCount = *pTotalCount;
    UINT32 TimeStampStart = 0U, TimeStampStop = 0U, TickingStart = 0U;

    while (RetVal == UART_ERR_NONE) {
        RetVal = AmbaRTSL_UartWrite(UartCh, RemainCount, &pTxBuf[TotalCount], &ActualCount);
        if (RetVal == UART_ERR_NONE) {
            RemainCount -= ActualCount;
            TotalCount += ActualCount;

            /* Some chips are much weaker so that we implement a recovery solution here */
            if (ActualCount == 0U) {
                if (TickingStart == 0U) {
                    (void)AmbaKAL_GetSysTickCount(&TimeStampStart);
                    TimeStampStop = TimeStampStart;
                    TickingStart = 1U;
                } else {
                    /* Some chips are much weaker so that we implement a recovery solution here */
                    (void)AmbaKAL_GetSysTickCount(&TimeStampStop);
                    if ((TimeStampStop - TimeStampStart) > 2000U) {   /* Assume all the tx fifo data could be transmitted within 10ms */
                        RetVal = UART_ERR_RECOVERY; /* Controller is abnormal. A soft reset is required. */
                    }
                }
            } else {
                TickingStart = 0U;
            }
        }

        if ((RetVal != UART_ERR_NONE) || (RemainCount == 0U)) {
            break;
        }

        if (AmbaKAL_EventFlagGet(&AmbaUartEventFlag, AMBA_UART_FLAG_TX_TIMEOUT << UartCh, 0x1U, 0x1U, NULL, KAL_WAIT_NEVER) == KAL_ERR_NONE) {
            RetVal = UART_ERR_TIMEOUT;
        } else {
            /* this function always returns success so that we can ignore its return value */
            (void)AmbaKAL_TaskYield();
        }
    }

    *pTotalCount = TotalCount;

    return RetVal;
}

/**
 *  UART_TxFifoEmpty - UART check TX FIFO empty
 *  @param[in] UartCh UART Channel Number
 *  @param[in] TimeOut Time out value
 *  @param[in] PreRetVal Past return value
 *  @return error code
 */
static UINT32 UART_TxFifoEmpty(UINT32 UartCh, UINT32 TimeOut, UINT32 PreRetVal)
{
    UINT32 TxEmpty, RetVal = PreRetVal;
    UINT32 TimeStampStart = 0U, TimeStampStop;

    if (RetVal != UART_ERR_TIMEOUT) {
        /* try to suspend task */
        if (AmbaKAL_EventFlagGet(&AmbaUartEventFlag, AMBA_UART_FLAG_TX_HOLD_EMPTY << UartCh, 0x1U, 0x1U, NULL, TimeOut) == KAL_ERR_NONE) {
            RetVal = UART_ERR_TIMEOUT;
        }
    }

    /* Some chips are much weaker so that we implement a recovery solution here */
    (void)AmbaKAL_GetSysTickCount(&TimeStampStart);
    TimeStampStop = TimeStampStart;
    do {
        (void)AmbaRTSL_UartGetTxEmpty(UartCh, &TxEmpty);
        (void)AmbaKAL_TaskYield();  /* relinquish time-slice */

        (void)AmbaKAL_GetSysTickCount(&TimeStampStop);
        if ((TimeStampStop - TimeStampStart) > 2000U) {   /* Assume all the tx fifo data could be transmitted within 10ms */
            RetVal = UART_ERR_RECOVERY; /* Controller is abnormal. A soft reset is required. */
            break;
        }
    } while (TxEmpty == 0U);

    return RetVal;
}

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
    UINT32 ActualCount = 0U;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (TxSize == 0U) || (pTxBuf == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaUartTxMutex[UartCh], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = UART_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_UartSetFlowCtrl(UartCh, FlowCtrlMode);
            if (RetVal == UART_ERR_NONE) {
                RetVal = UART_TxData(UartCh, TxSize, pTxBuf, &ActualCount, TimeOut);
            }

            /* Recovery once */
            if (RetVal == UART_ERR_RECOVERY) {
                AmbaRTSL_UartRecoverTxHalt(UartCh);
                RetVal = UART_ERR_NONE;
            }

            if (AmbaKAL_MutexGive(&AmbaUartTxMutex[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }

        if (pActualTxSize != NULL) {
            *pActualTxSize = ActualCount;
        }
    }

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
    UINT32 DmaChannel;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (RxSize == 0U) || (pRxBuf == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaUartRxMutex[UartCh], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = UART_ERR_MUTEX;
        } else {
            RetVal = UART_RxGetDmaChanNo(UartCh, &DmaChannel);
            if (RetVal == UART_ERR_NONE) {
                RetVal = AmbaRTSL_UartSetFlowCtrl(UartCh, FlowCtrlMode);
                if (RetVal == UART_ERR_NONE) {
                    RetVal = UART_DmaRead(UartCh, DmaChannel, RxSize, pRxBuf, pActualRxSize, TimeOut);
                }
            }

            if (AmbaKAL_MutexGive(&AmbaUartRxMutex[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }
    }

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
    UINT32 RetVal = UART_ERR_NONE;
    UINT32 ActualCount = 0U;
    UINT32 DmaChannel;

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (TxSize == 0U) || (pTxBuf == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaUartTxMutex[UartCh], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = UART_ERR_MUTEX;
        } else {
            RetVal = UART_TxGetDmaChanNo(UartCh, &DmaChannel);
            if (RetVal == UART_ERR_NONE) {
                RetVal = AmbaRTSL_UartSetFlowCtrl(UartCh, FlowCtrlMode);
                if (RetVal == UART_ERR_NONE) {
                    RetVal = UART_DmaWrite(UartCh, DmaChannel, TxSize, pTxBuf, &ActualCount, TimeOut);
                }
            }

            if (AmbaKAL_MutexGive(&AmbaUartTxMutex[UartCh]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = UART_ERR_UNEXPECTED;
            }
        }

        if (pActualTxSize != NULL) {
            *pActualTxSize = ActualCount;
        }
    }

    return RetVal;
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

    if ((UartCh >= AMBA_NUM_UART_CHANNEL) || (pStatus == NULL)) {
        RetVal = UART_ERR_ARG;
    } else {
        RetVal = AmbaRTSL_UartGetStatus(UartCh, pStatus);
    }

    return RetVal;
}

/**
 *  UART_RxISR - UART ISR for Reading data
 *  @param[in] UartCh UART Channel Number
 */
static void UART_RxISR(UINT32 UartCh)
{
    if (AmbaUartRxDeffered[UartCh] != 0U) {
        /* increase the counting semaphore */
        (void)AmbaKAL_SemaphoreGive(&AmbaUartRxSem[UartCh]);
    } else {
        if (AmbaKAL_EventFlagSet(&AmbaUartEventFlag, AMBA_UART_FLAG_RX_DATA_READY << UartCh) != KAL_ERR_NONE) {
            AmbaUartRxISRErr[UartCh] = UART_ERR_UNEXPECTED;
        }
    }
}

/**
 *  UART_TxISR - UART ISR for Writing data
 *  @param[in] UartCh UART Channel Number
 */
static void UART_TxISR(UINT32 UartCh)
{
    if (AmbaKAL_EventFlagSet(&AmbaUartEventFlag, AMBA_UART_FLAG_TX_HOLD_EMPTY << UartCh) != KAL_ERR_NONE) {
        AmbaUartTxISRErr[UartCh] = UART_ERR_UNEXPECTED;
    }
}

/**
 *  UART_RxTimeOutFunc - Time Out Acting Function called when RX Time Out
 *  @param[in] UartCh UART Channel Number
 */
static void UART_RxTimeOutFunc(UINT32 UartCh)
{
    if (AmbaKAL_EventFlagSet(&AmbaUartEventFlag, AMBA_UART_FLAG_RX_TIMEOUT << UartCh) != KAL_ERR_NONE) {
        AmbaUartRxTimeOutFuncErr[UartCh] = UART_ERR_UNEXPECTED;
    }
}

/**
 *  UART_TxTimeOutFunc - Time Out Acting Function called when TX Time Out
 *  @param[in] UartCh UART Channel Number
 */
static void UART_TxTimeOutFunc(UINT32 UartCh)
{
    if (AmbaKAL_EventFlagSet(&AmbaUartEventFlag, AMBA_UART_FLAG_TX_TIMEOUT << UartCh) != KAL_ERR_NONE) {
        AmbaUartTxTimeOutFuncErr[UartCh] = UART_ERR_UNEXPECTED;
    }
}

