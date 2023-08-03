#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaDrvEntry.h"
#include "AmbaDMA.h"
#include "AmbaKAL.h"
#include "AmbaUART.h"
#include "AmbaRTSL_UART.h"
#include "AmbaCSL_UART.h"
#include "AmbaReg_UART.h"
#include "AmbaCortexA53.h"

typedef struct {
    UINT32  ReadPtr;                /* pointer to the next rx ring buffer data for read */
    UINT32  WritePtr;               /* pointer to the next rx ring buffer data for write */

    UINT32  RingBufSize;            /* Size of the rx ring buffer */
    UINT8   *pRingBuf;              /* pointer to the rx ring buffer */
} AMBA_RTSL_UART_RX_BUF_CTRL_s;

typedef struct {
    AMBA_RTSL_UART_RX_BUF_CTRL_s    RxBufCtrl;
    UINT32  TxDataSize;             /* Number of data in tx data buffer */
    UINT8   *pTxDataBuf;            /* pointer to tx data buffer */

    UINT32  FlowCtrlSupport;        /* UART flow control support */

    AMBA_UART_ISR_f UsrRxReadyISR;  /* Callback function for rx data ready */
    UINT32 UsrRxReadyIsrArg;        /* argument attached to the rx data ready callback */

    UINT16  IntID;                  /* Interrupt ID */
    void    (*pISR)(UINT32 IntID, UINT32 UartCh);
    UINT32  DebugInfo;
} AMBA_RTSL_UART_CTRL_s;

extern void Set_RetVal_EventFlagCreate(UINT32 RetVal);
extern void Set_RetVal_EventFlagGet(UINT32 RetVal);
extern void Set_RetVal_EventFlagClear(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_RetVal_SemaphoreCreate(UINT32 RetVal);
extern void Set_RetVal_SemaphoreDelete(UINT32 RetVal);
extern void Set_RetVal_SemaphoreTake(UINT32 RetVal);
extern void Set_RetVal_TimerCreate(UINT32 RetVal);
extern void Set_RetVal_TimerChange(UINT32 RetVal);
extern void Set_RetVal_TimerStop(UINT32 RetVal);
extern void Set_RetVal_DmaChanNo(UINT32 RetVal);
extern void Set_RetVal_DmaGetTrfByteCount(UINT32 RetVal);
extern void Set_RetVal_DmaStop(UINT32 RetVal);
extern void Set_RetVal_Wait(UINT32 RetVal);
extern void Set_RetVal_Transfer(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);
extern void AmbaMisra_TypeCast(void * pNewType, const void * pOldType);
extern void AmbaDelayCycles(UINT32 Delays);

void TEST_AmbaUART_Config(void)
{
    UINT32 UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    UINT32 BaudRate = 1U;
    AmbaUART_Config(UartCh, BaudRate, NULL);

    AMBA_UART_CONFIG_s UartConfig = {0};
    AMBA_UART_CONFIG_s *pUartConfig = &UartConfig;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    UartCh = AMBA_UART_AHB_CHANNEL3;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    UartCh = AMBA_UART_AHB_CHANNEL2;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    UartCh = AMBA_UART_AHB_CHANNEL1;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    UartCh = AMBA_UART_AHB_CHANNEL0;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    UartCh = AMBA_UART_APB_CHANNEL0;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    UartCh = AMBA_NUM_UART_CHANNEL;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    /* AmbaRTSL_UART.c */
    UINT32 SourceClk = 24000000U;
    AmbaRTSL_UartConfig(UartCh, SourceClk, BaudRate, pUartConfig);

    UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    AmbaRTSL_UartConfig(UartCh, SourceClk, BaudRate, NULL);

    /* AmbaCSL_UART.c */
    pUartConfig->NumDataBits = UART_DATA_5_BIT;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    pUartConfig->NumDataBits = UART_DATA_6_BIT;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    pUartConfig->NumDataBits = UART_DATA_7_BIT;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    pUartConfig->NumDataBits = UART_DATA_8_BIT;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    pUartConfig->NumStopBits = UART_STOP_BIT_ONE;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    pUartConfig->NumStopBits = UART_STOP_BIT_TWO;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    pUartConfig->ParityBitMode = UART_PARITY_EVEN;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    pUartConfig->ParityBitMode = UART_PARITY_ODD;
    AmbaUART_Config(UartCh, BaudRate, pUartConfig);

    printf("AmbaUART_Config\n");
}

void TEST_AmbaUART_HookRxReadyNotifier(void)
{
    UINT32 UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    AMBA_UART_ISR_f NotifyFunc;
    UINT32 NotifyFuncArg = 0U;
    AmbaUART_HookRxReadyNotifier(UartCh, NotifyFunc, NotifyFuncArg);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaUART_HookRxReadyNotifier(UartCh, NotifyFunc, NotifyFuncArg);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaUART_HookRxReadyNotifier(UartCh, NotifyFunc, NotifyFuncArg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    UartCh = AMBA_NUM_UART_CHANNEL;
    AmbaUART_HookRxReadyNotifier(UartCh, NotifyFunc, NotifyFuncArg);

    /* AmbaRTSL_UART.c */
    AmbaRTSL_UartHookRxReadyISR(UartCh, NotifyFunc, NotifyFuncArg);

    printf("AmbaUART_HookRxReadyNotifier\n");
}

void TEST_AmbaUART_HookDeferredRxBuf(void)
{
    UINT32 UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    UINT32 BufSize = 1U;
    UINT8 Buf = 0U;
    UINT8 *pBuf = &Buf;
    AmbaUART_HookDeferredRxBuf(UartCh, BufSize, NULL);

    BufSize = 0U;
    AmbaUART_HookDeferredRxBuf(UartCh, BufSize, pBuf);

    BufSize = 1U;
    Set_RetVal_SemaphoreCreate(KAL_ERR_0000);
    AmbaUART_HookDeferredRxBuf(UartCh, BufSize, pBuf);
    Set_RetVal_SemaphoreCreate(KAL_ERR_NONE);

    AmbaUART_HookDeferredRxBuf(UartCh, BufSize, pBuf);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaUART_HookDeferredRxBuf(UartCh, BufSize, pBuf);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_SemaphoreDelete(KAL_ERR_0000);
    AmbaUART_HookDeferredRxBuf(UartCh, BufSize, pBuf);
    Set_RetVal_SemaphoreDelete(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaUART_HookDeferredRxBuf(UartCh, BufSize, pBuf);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    UartCh = AMBA_NUM_UART_CHANNEL;
    AmbaUART_HookDeferredRxBuf(UartCh, BufSize, pBuf);

    /* AmbaRTSL_UART.c */
    AmbaRTSL_UartHookRxBuf(UartCh, BufSize, pBuf);

    printf("AmbaUART_HookDeferredRxBuf\n");
}


void TEST_AmbaUART_ClearDeferredRxBuf(void)
{
    UINT32 UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    AmbaUART_ClearDeferredRxBuf(UartCh);

    Set_RetVal_SemaphoreCreate(KAL_ERR_0000);
    AmbaUART_ClearDeferredRxBuf(UartCh);
    Set_RetVal_SemaphoreCreate(KAL_ERR_NONE);

    AmbaUART_ClearDeferredRxBuf(UartCh);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaUART_ClearDeferredRxBuf(UartCh);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_SemaphoreDelete(KAL_ERR_0000);
    AmbaUART_ClearDeferredRxBuf(UartCh);
    Set_RetVal_SemaphoreDelete(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaUART_ClearDeferredRxBuf(UartCh);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    UartCh = AMBA_NUM_UART_CHANNEL;
    AmbaUART_ClearDeferredRxBuf(UartCh);

    /* AmbaRTSL_UART.c */
    AmbaRTSL_UartClearRxRingBuf(UartCh);

    printf("AmbaUART_ClearDeferredRxBuf\n");
}


void TEST_AmbaUART_Read(void)
{
    UINT32 UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    UINT32 FlowCtrlMode = UART_FLOW_CTRL_NONE;
    UINT32 RxSize = 1U;
    UINT8 RxBuf = 0U;
    UINT8 *pRxBuf = &RxBuf;
    UINT32 ActualRxSize = 0U;
    UINT32 *pActualRxSize = &ActualRxSize;
    UINT32 TimeOut = KAL_WAIT_NEVER;
    UINT32 BufSize = 0U;
    UINT8 Buf = 0U;
    UINT8 *pBuf = &Buf;
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, NULL, pActualRxSize, TimeOut);

    RxSize = 0U;
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);

    RxSize = 1U;
    Set_RetVal_SemaphoreTake(KAL_ERR_TIMEOUT);
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_SemaphoreTake(KAL_ERR_0000);
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_SemaphoreTake(KAL_ERR_NONE);

    Set_RetVal_TimerStop(KAL_ERR_TIMEOUT);
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_TimerStop(KAL_ERR_NONE);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    TimeOut = KAL_WAIT_FOREVER;
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);

    Set_RetVal_TimerChange(KAL_ERR_0000);
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_TimerChange(KAL_ERR_NONE);

    AmbaUART_HookDeferredRxBuf(UartCh, BufSize, pBuf);
    
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);

    FlowCtrlMode = 2U;
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, NULL, TimeOut);

    UartCh = AMBA_NUM_UART_CHANNEL;
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);

    /* AmbaRTSL_UART.c */
    AmbaRTSL_UartSetFlowCtrl(UartCh, FlowCtrlMode);

    AmbaRTSL_UartRead(UartCh, RxSize, pRxBuf, pActualRxSize);

    AmbaRTSL_UartGetPreReadData(UartCh, pRxBuf);

    UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    AmbaRTSL_UartRead(UartCh, RxSize, NULL, NULL);

    RxSize = 0U;
    AmbaRTSL_UartRead(UartCh, RxSize, pRxBuf, pActualRxSize);

    AMBA_RTSL_UART_CTRL_s AmbaUartCtrl[AMBA_NUM_UART_CHANNEL];
    AmbaRTSL_UartGetPreReadData(UartCh, pRxBuf);

    AmbaRTSL_UartGetPreReadData(UartCh, NULL);

    /* AmbaCSL_UART.c */
    RxSize = 1U;
    FlowCtrlMode = UART_FLOW_CTRL_HW;
    AmbaUART_Read(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);

    printf("AmbaUART_Read\n");
}


void TEST_AmbaUART_Write(void)
{
    UINT32 UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    UINT32 FlowCtrlMode = 0U;
    UINT32 TxSize = 1U;
    UINT8 TxBuf = 0U;
    UINT8 *pTxBuf = &TxBuf;
    UINT32 ActualTxSize = 0U;
    UINT32 *pActualTxSize = &ActualTxSize;
    UINT32 TimeOut = KAL_WAIT_NEVER;
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, NULL, pActualTxSize, TimeOut);

    TxSize = 0U;
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);

    TxSize = 1U;
    Set_RetVal_TimerStop(KAL_ERR_TIMEOUT);
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_TimerStop(KAL_ERR_NONE);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    TimeOut = KAL_WAIT_FOREVER;
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_TimerChange(KAL_ERR_0000);
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_TimerChange(KAL_ERR_NONE);

    FlowCtrlMode = 2U;
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, NULL, TimeOut);

    UartCh = AMBA_NUM_UART_CHANNEL;
    AmbaUART_Write(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);

    /* AmbaRTSL_UART.c */
    AmbaRTSL_UartWrite(UartCh, TxSize, pTxBuf, pActualTxSize);

    AmbaRTSL_UartGetTxEmpty(UartCh, NULL);

    AmbaRTSL_UartRecoverTxHalt(UartCh);

    UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    AmbaRTSL_UartWrite(UartCh, TxSize, NULL, NULL);

    TxSize = 0U;
    AmbaRTSL_UartWrite(UartCh, TxSize, pTxBuf, pActualTxSize);

    AmbaRTSL_UartGetTxEmpty(UartCh, NULL);

    UINT32 TxEmpty = 0U;
    UINT32 *pTxEmpty = &TxEmpty;
    AmbaRTSL_UartGetTxEmpty(UartCh, pTxEmpty);

    AmbaRTSL_UartRecoverTxHalt(UartCh);

    printf("AmbaUART_Write\n");
}


void TEST_AmbaUART_DmaRead(void)
{
    UINT32 UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    UINT32 FlowCtrlMode = 0U;
    UINT32 RxSize = 1U;
    UINT8 RxBuf = 0U;
    UINT8 *pRxBuf = &RxBuf;
    UINT32 ActualRxSize = 0U;
    UINT32 *pActualRxSize = &ActualRxSize;
    UINT32 TimeOut = 0U;
    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, NULL, pActualRxSize, TimeOut);

    RxSize = 0U;
    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);

    RxSize = 1U;
    Set_RetVal_DmaChanNo(KAL_ERR_0000);
    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_DmaChanNo(KAL_ERR_NONE);

    AmbaUART_DmaRead(UartCh, 2U, RxSize, pRxBuf, pActualRxSize, TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, pRxBuf, NULL, TimeOut);

    Set_RetVal_Wait(DMA_ERR_BASE);
    Set_RetVal_DmaGetTrfByteCount(DMA_ERR_BASE);
    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_DmaGetTrfByteCount(DMA_ERR_NONE);

    Set_RetVal_DmaStop(DMA_ERR_BASE);
    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_DmaStop(DMA_ERR_NONE);
    Set_RetVal_Wait(DMA_ERR_NONE);

    Set_RetVal_Transfer(1U);
    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);
    Set_RetVal_Transfer(0U);

    Set_RetVal_DmaChanNo(1U);

    UartCh = AMBA_NUM_UART_CHANNEL;
    AmbaUART_DmaRead(UartCh, FlowCtrlMode, RxSize, pRxBuf, pActualRxSize, TimeOut);

    printf("AmbaUART_DmaRead\n");
}


void TEST_AmbaUART_DmaWrite(void)
{
    UINT32 UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    UINT32 FlowCtrlMode = 0U;
    UINT32 TxSize = 1U;
    UINT8 TxBuf = 0U;
    UINT8 *pTxBuf = &TxBuf;
    UINT32 ActualTxSize = 0U;
    UINT32 *pActualTxSize = &ActualTxSize;
    UINT32 TimeOut = 0U;
    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, NULL, pActualTxSize, TimeOut);

    TxSize = 0U;
    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);

    TxSize = 1U;
    Set_RetVal_DmaChanNo(KAL_ERR_0000);
    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_DmaChanNo(KAL_ERR_NONE);

    AmbaUART_DmaWrite(UartCh, 2U, TxSize, pTxBuf, pActualTxSize, TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, pTxBuf, NULL, TimeOut);

    Set_RetVal_Wait(DMA_ERR_BASE);
    Set_RetVal_DmaGetTrfByteCount(DMA_ERR_BASE);
    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_DmaGetTrfByteCount(DMA_ERR_NONE);

    Set_RetVal_DmaStop(DMA_ERR_BASE);
    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_DmaStop(DMA_ERR_NONE);
    Set_RetVal_Wait(DMA_ERR_NONE);

    Set_RetVal_Transfer(KAL_ERR_0000);
    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);
    Set_RetVal_Transfer(KAL_ERR_NONE);

    UartCh = AMBA_NUM_UART_CHANNEL;
    AmbaUART_DmaWrite(UartCh, FlowCtrlMode, TxSize, pTxBuf, pActualTxSize, TimeOut);

    printf("AmbaUART_DmaWrite\n");
}


void TEST_AmbaUART_GetInfo(void)
{
    UINT32 UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    AmbaUART_GetInfo(UartCh, NULL);

    UINT32 Status = 0U;
    UINT32 *pStatus = &Status;
    AmbaUART_GetInfo(UartCh, pStatus);

    UartCh = AMBA_NUM_UART_CHANNEL;
    AmbaUART_GetInfo(UartCh, pStatus);

    /* AmbaRTSL_UART.c */
    AmbaRTSL_UartGetStatus(UartCh, pStatus);

    UartCh = AMBA_NUM_UART_CHANNEL - 1U;
    AmbaRTSL_UartGetStatus(UartCh, NULL);

    AmbaRTSL_UartGetStatus(UartCh, pStatus);

    printf("AmbaUART_GetInfo\n");
}

void TEST_AmbaUART_DrvEntry(void)
{
    AmbaUART_DrvEntry();
    Set_RetVal_TimerCreate(KAL_ERR_0000);
    AmbaUART_DrvEntry();
    Set_RetVal_TimerCreate(KAL_ERR_NONE);

    Set_RetVal_EventFlagCreate(KAL_ERR_0000);
    AmbaUART_DrvEntry();
    Set_RetVal_EventFlagCreate(KAL_ERR_NONE);

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaUART_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    Set_RetVal_SemaphoreCreate(KAL_ERR_0000);
    AmbaUART_DrvEntry();
    Set_RetVal_SemaphoreCreate(KAL_ERR_NONE);

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaUART_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("AmbaUART_DrvEntry\n");
}

void TEST_AmbaCSL_UartGetDivider(void)
{
    AMBA_UART_REG_s UartReg = {0};
    AMBA_UART_REG_s *pUartReg = &UartReg;
    AmbaCSL_UartGetDivider(pUartReg);

    printf("AmbaCSL_UartGetDivider\n");
}

int main(void)
{
    for (UINT8 i = 0U; i < AMBA_NUM_UART_CHANNEL; i++) {
        pAmbaUART_Reg[i] = malloc(sizeof(AMBA_UART_REG_s));
    }

    /* avoid AmbaUART_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaUART_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaUART_Config();
    TEST_AmbaUART_HookRxReadyNotifier();
    TEST_AmbaUART_HookDeferredRxBuf();
    TEST_AmbaUART_ClearDeferredRxBuf();
    TEST_AmbaUART_Read();
    TEST_AmbaUART_Write();
    TEST_AmbaUART_DmaRead();
    TEST_AmbaUART_DmaWrite();
    TEST_AmbaUART_GetInfo();

    /* AmbaCSL_UART.c */
    TEST_AmbaCSL_UartGetDivider();

    return 0;
}

