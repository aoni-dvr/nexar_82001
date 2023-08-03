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

#ifndef AMBA_REG_UART_H
#define AMBA_REG_UART_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * UART: Receive/Transmit Data Buffer Register
 */
typedef struct {
    UINT32  DataByte:            8;     /* [7:0]:  Data byte received/Tx_Holding */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} UART_DATA_BUF_REG_s;

/*
 * UART: Divisor Latch Low Register
 */
typedef struct {
    UINT32  Value:              8;      /* [7:0]: Lower/Higher 8bits of divider */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_UART_DIVIDER_REG_s;

/*
 * UART: Interruput Enable Register
 */
typedef struct {
    UINT32  RxDataReady:        1;      /* [0]: 1 = Received Data Available Interrupt Enabled */
    UINT32  TxBufEmpty:         1;      /* [1]: 1 = Transmit Holding Register Empty Interrupt Enabled */
    UINT32  RxLineStatus:       1;      /* [2]: 1 = Receiver Line Status Interruput Enabled */
    UINT32  ModemStatus:        1;      /* [3]: 1 = Modem Status Interrupt Enabled */
    UINT32  BusyDection:        1;      /* [4]: 1 = Busy Detect Indication Enabled */
    UINT32  TimeOut:            1;      /* [5]: 1 = Time Out Interruput Enabled */
    UINT32  RxFifoTimeout:      1;      /* [6]: 1 = Reciever Fifo Empty and Timeout Interruput Enable */
    UINT32  ProgTxBufEmptyMode: 1;      /* [7]: 1 = Programmable THRE Interruput Mode Enabled */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_UART_IRQ_ENABLE_REG_s;

/*
 * UART: Interrupt ID Register
 */
#define AMBA_UART_IRQ_MODEM_STATUS_CHANGED  0x00U
#define AMBA_UART_IRQ_NO_PENDING            0x01U
#define AMBA_UART_IRQ_THRESHOLD_EMPTY       0x02U
#define AMBA_UART_IRQ_RX_DATA_READY         0x04U
#define AMBA_UART_IRQ_RX_LINE_STATUS        0x06U
#define AMBA_UART_IRQ_BUSY                  0x07U
#define AMBA_UART_IRQ_TIME_OUT              0x0CU

typedef struct {
    UINT32  IrqID:              4;      /* [3:0]: Interrupt ID */
    UINT32  Reserved0:          2;      /* [5:4]: Reserved */
    UINT32  FifoEnable:         2;      /* [7:6]: 0 = Tx/Rx FIFOs Disabled, 3 = Tx/Rx FIFOs Enabled */
    UINT32  Reserved1:          24;     /* [31:8]: Reserved */
} AMBA_UART_IRQ_ID_REG_s;

/*
 * UART: FIFO Control Register
 */
typedef struct {
    UINT32  FifoEnable:         1;      /* [0]: 1 = FIFO Enable */
    UINT32  RxFifoReset:        1;      /* [1]: 1 = Rx FIFO Reset */
    UINT32  TxFifoReset:        1;      /* [2]: 1 = Tx FIFO Reset */
    UINT32  DmaMode:            1;      /* [3]: 0 = Mode0, 1 = Mode1 */
    UINT32  TxFifoTrigger:      2;      /* [5:4]: 0 = Empty, 1 = 4char, 2 = 8char. 3 = defined in TxThreshold. */
    UINT32  RxFifoTrigger:      2;      /* [7:6]: 0 = 1char, 1 = 4char, 2 = 8char. 3 = defined in RxThreshold*/
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_UART_FIFO_CTRL_REG_s;

/*
 * UART: Line Control Register
 */
typedef struct {
    UINT32  DataBits:           2;      /* [1:0]: 0 = 5bits, 1 = 6bits, 2 = 7bits, 3 = 8bits */
    UINT32  StopBits:           1;      /* [2]: Stop Bits Per Character. 0 = 1stop, 1 = 1.5 or 2stop */
    UINT32  ParityEnable:       1;      /* [3]: 1 = Parity Enabled */
    UINT32  EvenParity:         1;      /* [4]: 0 = Odd Parity, 1 = Even Parity */
    UINT32  Reserved0:          1;      /* [5]: Reserved */
    UINT32  BreakCtrl:          1;      /* [6]: Break Control */
    UINT32  DLAB:               1;      /* [7]: Divisor Latch Access Bit */
    UINT32  Reserved1:          24;     /* [31:8]: Reserved */
} AMBA_UART_LINE_CTRL_REG_s;

/*
 * UART: Modem Control Register
 */
typedef struct {
    UINT32  DTR:                1;      /* [0]: 1 = Data Terminal Ready(DTR_N) Asserted */
    UINT32  RTS:                1;      /* [1]: Request to Send */
    UINT32  OUT1:               1;      /* [2]: 1 = OUT1_N Asserted */
    UINT32  OUT2:               1;      /* [3]: 1 = OUT2_N Asserted */
    UINT32  LoopBack:           1;      /* [4]: LoopBack Bit */
    UINT32  AutoFlowCtrl:       1;      /* [5]: 1 = Auto Flow Control Mode Enabled */
    UINT32  Reserved:           26;     /* [31:6]: Reserved */
} AMBA_UART_MODEM_CTRL_REG_s;

/*
 * UART: Line Status Register
 */
typedef struct {
    UINT32  DataReady:          1;      /* [0]: 1 = Data Ready */
    UINT32  OverrunError:       1;      /* [1]: 1 = Overrun Error */
    UINT32  ParityError:        1;      /* [2]: 1 = Parity Error */
    UINT32  FramingError:       1;      /* [3]: 1 = Framing Error */
    UINT32  BreakIrq:           1;      /* [4]: Break Interrupt */
    UINT32  TxFifoEmpty:        1;      /* [5]: Transmit Holding Register Empty(THRE) */
    UINT32  TxEmpty:            1;      /* [6]: Transmitter Empty */
    UINT32  RxFifoError:        1;      /* [7]: 1 = Error in Rx FIFO */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_UART_LINE_STATUS_REG_s;

/*
 * UART: Modem Status Register
 */
typedef struct {
    UINT32  DCTS:               1;      /* [0]: Delta Clear to Send (DCTS) */
    UINT32  DDSR:               1;      /* [1]: Delta Data Set Ready (DDSR) */
    UINT32  TERI:               1;      /* [2]: Trailing Edge of Ring Indicator */
    UINT32  DDCD:               1;      /* [3]: Delta Data Carrier Detect */
    UINT32  CTS:                1;      /* [4]: Clear to Send */
    UINT32  DSR:                1;      /* [5]: Data Set Ready */
    UINT32  RingIndicator:      1;      /* [6]: Ring Indicator */
    UINT32  DCD:                1;      /* [7]: Data Carrier Detect */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_UART_MODEM_STATUS_REG_s;

/*
 * UART: Scratchpad Register
 */
typedef struct {
    UINT32  DataByte:           8;      /* [7:0]: Reset = 0; Use for temporary storage */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_UART_SCRATCHPAD_REG_s;

/*
 * UART: DMA Enable Register
 */
typedef struct {
    UINT32  RxEnable:           1;     /* [0]: RX DMA request enable */
    UINT32  TxEnable:           1;     /* [1]: TX DMA request enable */
    UINT32  Reserved:           30;    /* [31:2]: Reserved */
} AMBA_UART_DMA_CTRL_REG_s;

/*
 * UART: Status Register
 */
typedef struct {
    UINT32  Busy:               1;      /* [0]: 1 = UART is Busy */
    UINT32  TxFifoNotFull:      1;      /* [1]: 1 = Transmit FIFO is not full */
    UINT32  TxFifoEmpty:        1;      /* [2]: 1 = Transmit FIFO is empty */
    UINT32  RxFifoNotEmpty:     1;      /* [3]: 1 = Receive FIFO is not empty */
    UINT32  RxFifoFull:         1;      /* [4]: 1 = Receive FIFO is full */
    UINT32  Reserved:           27;     /* [31:5]: Reserved */
} AMBA_UART_STATUS_REG_s;

/*
 * UART: Number of data entries in Transmit/Receive Register
 */
typedef struct {
    UINT32  Value:              13;     /* [12:0]: the number of data entries in the FIFO */
    UINT32  Reserved:           19;     /* [31:13]: Reserved */
} AMBA_UART_FIFO_DATA_SIZE_REG_s;

/*
 * UART: Software Reset Register
 */
typedef struct {
    UINT32  UartReset:          1;      /* [0]: UART Reset */
    UINT32  RxFifoReset:        1;      /* [1]: Rx FIFO Reset */
    UINT32  TxFifoReset:        1;      /* [2]: Tx FIFO Reset */
    UINT32  Reserved:           29;     /* [31:3]: Reserved */
} AMBA_UART_RESET_CTRL_REG_s;

/*
 * UART: Halt Tx Register
 */
typedef struct {
    UINT32  Enable:             1;      /* [0]: 1 = Halt Tx Enabled */
    UINT32  Reserved:           31;     /* [31:1]: Reserved */
} AMBA_UART_HALT_TX_REG_s;

/*
 * UART: Trigger Threshold for Transmit/Receive Ctrl Register
 */
typedef struct {
    UINT32  Threshold:          6;      /* [5:0]: the number of trigger threshold for the UART fifo ctrl */
    UINT32  Reserved:           26;     /* [31:6]: Reserved */
} AMBA_UART_TRIGGER_THRESHOLD_SIZE_REG_s;

/*
 * UART: Component Parameter Register
 */
typedef struct {
    UINT32  ApbDataWidth:       2;      /* [1:0]: 0 = 8bits, 1 = 16bits, 2 = 32 bits, 3 = Reserved */
    UINT32  Reserved0:          2;      /* [3:2]: Reserved */
    UINT32  AfceMode:           1;      /* [4]: 1 = True */
    UINT32  ThreMode:           1;      /* [5]: 1 = True */
    UINT32  SirMode:            1;      /* [6]: 1 = True */
    UINT32  SirLpMode:          1;      /* [7]: 1 = True */
    UINT32  AdditionalFeat:     1;      /* [8]: 1 = True */
    UINT32  FifoAccess:         1;      /* [9]: 1 = True */
    UINT32  FifoStat:           1;      /* [10]: 1 = True */
    UINT32  Shadow:             1;      /* [11]: 1 = True */
    UINT32  AddEncodedParams:   1;      /* [12]: 1 = True */
    UINT32  DmaExtra:           1;      /* [13]: 1 = True */
    UINT32  Reserved1:          2;      /* [15:14]: Reserved */
    UINT32  FifoMode:           8;      /* [23:16]: 0x00:0, 0x01:16, 0x02:32,,, 0x80:2048, 0x81~0xFF:Reserved */
    UINT32  Reserved2:          8;      /* [31:24]: Reserved */
} AMBA_UART_INFO_REG_s;

/*
 * UART: All Registers
 */
/* DLAB = 0 for normal case */
typedef struct {
    UART_DATA_BUF_REG_s    DataBuf;        /* 0x000(R,W):  */
    AMBA_UART_IRQ_ENABLE_REG_s  IrqEnable;      /* 0x004(RW): Interruput Enable Register */
} AMBA_UART_DLAB0_REG_s;

/* DLAB = 1 for Divider Acccess */
typedef struct {
    AMBA_UART_DIVIDER_REG_s     DividerLow;     /* 0x000(RW): Divisor Latch Low Register */
    AMBA_UART_DIVIDER_REG_s     DividerHigh;    /* 0x004(RW): Divisor Latch High Register */
} AMBA_UART_DLAB1_REG_s;

typedef struct {
    volatile UINT32                                     DataBuf;            /* 0x000(R,W): Receive(R) or Tx Holding(W) Data Buffer Register */
    volatile UINT32                                     IntCtrl;            /* 0x004(RW): Interruput Enable Register */
    volatile UINT32                                     FifoCtrl;           /* 0x008(R,W): Interrupt ID(R) or FIFO Control(W) Register */
    volatile UINT32                                     LineCtrl;           /* 0x00C(RW): Line Control Register */
    volatile AMBA_UART_MODEM_CTRL_REG_s                 ModemCtrl;          /* 0x010(RW): Modem Control Register */
    volatile UINT32                                     LineStatus;         /* 0x014(R): Line Status Register */
    volatile UINT32                                     ModemStatus;        /* 0x018(R): Modem Status Register */
    volatile AMBA_UART_SCRATCHPAD_REG_s                 Scratchpad;         /* 0x01C(RW): Scratchpad Register */
    volatile UINT32                                     Reserved0[2];       /* 0x020-0x024: Reserved */
    volatile UINT32                                     DmaCtrl;            /* 0x028: DMA Control Register */
    volatile UINT32                                     Reserved1[5];       /* 0x02C-0x03C: Reserved */
    volatile UART_DATA_BUF_REG_s                        DataBurst[8];       /* 0x040-0x05C(R,W): Receive Buffer(R) or Transmit Holding(W) Register */
    volatile UINT32                                     Reserved2[7];       /* 0x060-0x078: Reserved */
    volatile UINT32                                     Status;             /* 0x07C(R): UART Status Register */
    volatile AMBA_UART_FIFO_DATA_SIZE_REG_s             TxFifoDataSize;     /* 0x080(R): Transmit FIFO Level Register */
    volatile AMBA_UART_FIFO_DATA_SIZE_REG_s             RxFifoDataSize;     /* 0x084(R): Receive FIFO Level Register */
    volatile UINT32                                     ResetCtrl;          /* 0x088(W): Software Reset Register */
    volatile UINT32                                     Reserved3[6];       /* 0x08C-0x0A0: Reserved*/
    volatile AMBA_UART_HALT_TX_REG_s                    HaltTx;             /* 0x0A4(RW): Halt Tx Register */
    volatile UINT32                                     Reserved4;          /* 0x0A8: Reserved */
    volatile UINT32                                     RxThreshold;        /* 0x0AC: Rx Trigger Threshold */
    volatile AMBA_UART_TRIGGER_THRESHOLD_SIZE_REG_s     TxThreshold;        /* 0x0B0: Tx Trigger Threshold */
    volatile UINT32                                     State;              /* 0x0B4(R): TX/RX State Register */
    volatile UINT32                                     Busy;               /* 0x0B8(R): TX/RX Busy Register */
    volatile UINT32                                     Reserved5[14];      /* 0x0BC-0x0F0: Reserved */
    volatile AMBA_UART_INFO_REG_s                       Info;               /* 0x0F4(R): Component Parameter Register */
    volatile UINT32                                     Version;            /* 0x0F8(R): Component Version Register */
    volatile UINT32                                     PeripheralID;       /* 0x0FC(R): Component Type Register */
} AMBA_UART_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_QNX
extern AMBA_UART_REG_s * pAmbaUART_Reg[AMBA_NUM_UART_CHANNEL];
#else
extern AMBA_UART_REG_s * pAmbaUART_Reg[AMBA_NUM_UART_CHANNEL];
#endif
#endif /* AMBA_REG_UART_H */
