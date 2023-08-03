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

#ifndef AMBA_REG_I2S_H
#define AMBA_REG_I2S_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#include "AmbaI2S_Priv.h"

/*
 * Definitions & Constants for I2S Mode Register
 */
typedef struct {
    UINT32  Mode:               3;      /* [2:0]  Select audio data format */
    UINT32  Reserved:           29;     /* [31:3] Reserved */
} AMBA_I2S_MODE_REG_s;

/*
 * Definitions & Constants for I2S Receiver Control Register
 */
typedef struct {
    UINT32  RxWsInv:            1;      /* [0]    0 - Receiver first data on WS = 0, 1 - Receive first data on WS = 1 */
    UINT32  RxWsMst:            1;      /* [1]    0 - Receiver is slave mode, 1 - Receiver is master mode */
    UINT32  Order:              1;      /* [2]    Receiver bit order: 0 - MSB is first, 1 - LSB is first */
    UINT32  Loopback:           1;      /* [3]    Receiver loop back test */
    UINT32  Reserved:           28;     /* [31:4] Reserved */
} AMBA_I2S_RX_CTRL_REG_s;

/*
 * Definitions & Constants for I2S Transmitter Control Register
 */
typedef struct {
    UINT32  Mono:               2;      /* [1:0]  Transmitter mono */
    UINT32  Mute:               1;      /* [2]    Transmitter mute: 0 - Normal, 1 - Mute */
    UINT32  Unison:             1;      /* [3]    Transmitter unison */
    UINT32  TxWsInv:            1;      /* [4]    0 - Left channel under WS = 0, 1 - Left channel under WS = 1 */
    UINT32  TxWsMst:            1;      /* [5]    0 - Transmitter is slave mode, 1 - Transmitter is master mode */
    UINT32  Order:              1;      /* [6]    Transmitter bit order: 0 - MSB is first, 1 - LSB is first */
    UINT32  Loopback:           1;      /* [7]    Transmitter loop back test */
    UINT32  Reserved:           24;     /* [31:8] Reserved */
} AMBA_I2S_TX_CTRL_REG_s;

/*
 * Definitions & Constants for I2S Word Length Register
 */
typedef struct {
    UINT32  Wlen:               5;      /* [4:0]  Word precision - 1, For example, 16 bit = 0xF and 24 bit = 0x17 */
    UINT32  Reserved:           27;     /* [31:5] Reserved */
} AMBA_I2S_WLEN_REG_s;

/*
 * Definitions & Constants for I2S Word Position Register
 */
typedef struct {
    UINT32  IgnoredBits:        5;      /* [4:0]  Ignored bits between two ws edges */
    UINT32  Reserved:           27;     /* [31:5] Reserved */
} AMBA_I2S_WPOS_REG_s;

/*
 * Definitions & Constants for I2S Slot Register
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
typedef struct {
    UINT32  SlotCount:          5;      /* [4:0]  Data slot counts after each ws strobe (only in DSP mode) */
    UINT32  Reserved:           27;     /* [31:5] Reserved */
} AMBA_I2S_SLOT_REG_s;
#else
typedef struct {
    UINT32  SlotCount:          8;      /* [7:0]  Data slot counts after each ws strobe (only in DSP mode) */
    UINT32  Reserved:           24;     /* [31:8] Reserved */
} AMBA_I2S_SLOT_REG_s;
#endif

/*
 * Definitions & Constants for I2S Transmitter FIFO Threshold Register
 */
typedef struct {
    UINT32  Ft:                 7;      /* [6:0]  Transmitter FIFO threshold register */
    UINT32  Reserved:           25;     /* [31:7] Reserved */
} AMBA_I2S_TX_FIFO_LTH_REG_s;

/*
 * Definitions & Constants for I2S Receiver FIFO Threshold Register
 */
typedef struct {
    UINT32  Ft:                 7;      /* [6:0]  Receiver FIFO threshold register */
    UINT32  Reserved:           25;     /* [31:7] Reserved */
} AMBA_I2S_RX_FIFO_GTH_REG_s;

/*
 * Definitions & Constants for I2S Clock Control Register
 */
typedef struct {
    UINT32  ClkDiv:             5;      /* [4:0]   Clock divider: BCLK = CLK_AU/[2*(clk_div+1)] */
    UINT32  Rsp:                1;      /* [5]     Receiver BLCK polarity */
    UINT32  Tsp:                1;      /* [6]     Transmitter BCLK polarity */
    UINT32  Ss:                 1;      /* [7]     BCLK source selection */
    UINT32  Soe:                1;      /* [8]     BCLK signal output enable: 0 - Disable output, 1 - Enable output */
    UINT32  Woe:                1;      /* [9]     WS signal output enable: 0 - Disable output, 1 - Enable output */
    UINT32  ClkDivScale:        2;      /* [11:10] 0:  scaled_clk_div = clk_div, 1:  scaled_clk_div = clk_div*2, 2:  scaled_clk_div = clk_div*4, 3:  scaled_clk_div = clk_div*8 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
    UINT32  Reserved:           20;     /* [31:12] Reserved */
#else
    UINT32  DataMode:           1;      /* [12]    Set to enable data mode. When data mode is enabled, I2S_CLK will not toggle if TX FIFO is empty while transmitting data or RX FIFO is full while receiving data */
    UINT32  Reserved:           19;     /* [31:13] Reserved */
#endif
} AMBA_I2S_CLK_CTRL_REG_s;

#define I2S_CLK_CTRL_REG_CLKDIV_SHIFT       (0UL)
#define I2S_CLK_CTRL_REG_RSP_SHIFT          (5UL)
#define I2S_CLK_CTRL_REG_TSP_SHIFT          (6UL)
#define I2S_CLK_CTRL_REG_SS_SHIFT           (7UL)
#define I2S_CLK_CTRL_REG_SOE_SHIFT          (8UL)
#define I2S_CLK_CTRL_REG_WOE_SHIFT          (9UL)
#define I2S_CLK_CTRL_REG_CLKDIV_SCALE_SHIFT (10UL)

#define I2S_CLK_CTRL_REG_CLKDIV_MASK        (0x01FUL)
#define I2S_CLK_CTRL_REG_RSP_MASK           (0x001UL << I2S_CLK_CTRL_REG_RSP_SHIFT)
#define I2S_CLK_CTRL_REG_TSP_MASK           (0x001UL << I2S_CLK_CTRL_REG_TSP_SHIFT)
#define I2S_CLK_CTRL_REG_SS_MASK            (0x001UL << I2S_CLK_CTRL_REG_SS_SHIFT)
#define I2S_CLK_CTRL_REG_SOE_MASK           (0x001UL << I2S_CLK_CTRL_REG_SOE_SHIFT)
#define I2S_CLK_CTRL_REG_WOE_MASK           (0x001UL << I2S_CLK_CTRL_REG_WOE_SHIFT)
#define I2S_CLK_CTRL_REG_CLKDIV_SCALE_MASK  (0x003UL << I2S_CLK_CTRL_REG_CLKDIV_SCALE_SHIFT)

/*
 * Definitions & Constants for I2S Initial Control Register
 */
typedef struct {
    UINT32  Frst:               1;      /* [0]    FIFO reset, write 1 to reset both Transmitter & Receiver FIFOs */
    UINT32  Re:                 1;      /* [1]    Receiver enable */
    UINT32  Te:                 1;      /* [2]    Transmitter enable */
    UINT32  Rxfrst:             1;      /* [3]    Receiver FIFO Reset. Write 1 to reset Receiver FIFO */
    UINT32  Txfrst:             1;      /* [4]    Transmitter FIFO Reset. Write 1 to reset Transmitter FIFO */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
    UINT32  enSrc:              1;      /* [5]    Tx_en/Rx_en source select(internal or external): 0 - internal enable, 1 - external enable */
    UINT32  Reserved:           26;     /* [31:6] Reserved */
#else
    UINT32  Reserved:           27;     /* [31:5] Reserved */
#endif
} AMBA_I2S_INIT_REG_s;

/*
 * Definitions & Constants for I2S Transmitter Status Register
 */
typedef struct {
    UINT32  Fe:                 1;      /* [0]    FIFO empty flag */
    UINT32  Ff:                 1;      /* [1]    FIFO full flag */
    UINT32  Reserved1:          1;      /* [2]    Reserved */
    UINT32  Ftv:                1;      /* [3]    FIFO threshold is valid */
    UINT32  Ti:                 1;      /* [4]    Transmitter idle flag */
    UINT32  Reserved2:          27;     /* [31:5] Reserved */
} AMBA_I2S_TX_FIFO_FLAG_REG_s;

/*
 * Definitions & Constants for I2S Transmitter Right Channel Data Register
 */
typedef struct {
    UINT32  Data:               24;     /* [23:0]  Data */
    UINT32  Reserved:           8;      /* [31:24] Reserved */
} AMBA_I2S_TX_RIGHT_DATA_REG_s;

/*
 * Definitions & Constants for I2S Receiver Status Register
 */
typedef struct {
    UINT32  Fe:                 1;      /* [0]    FIFO empty flag */
    UINT32  Ff:                 1;      /* [1]    FIFO full flag */
    UINT32  Fo:                 1;      /* [2]    FIFO overflow flag */
    UINT32  Ftv:                1;      /* [3]    FIFO threshold is valid */
    UINT32  Ri:                 1;      /* [4]    Receiver idle flag */
    UINT32  Reserved:           27;     /* [31:5] Reserved */
} AMBA_I2S_RX_FIFO_FLAG_REG_s;

/*
 * Definitions & Constants for I2S Receiver Data Register
 */
typedef struct {
    UINT32  Data:               24;     /* [23:0]  Data */
    UINT32  Reserved:           8;      /* [31:24] Reserved */
} AMBA_I2S_RX_DATA_REG_s;

/*
 * Definitions & Constants for I2S Transmitter FIFO Counter Register
 */
typedef struct {
    UINT32  C:                  7;      /* [6:0]  Transmitter FIFO data count */
    UINT32  Reserved:           25;     /* [31:7] Reserved */
} AMBA_I2S_TX_FIFO_CNTR_REG_s;

/*
 * Definitions & Constants for I2S Receiver FIFO Counter Register
 */
typedef struct {
    UINT32  C:                  8;      /* [7:0]  Receiver FIFO data count */
    UINT32  Reserved:           24;     /* [31:8] Reserved */
} AMBA_I2S_RX_FIFO_CNTR_REG_s;

/*
 * Definitions & Constants for I2S Transmitter Interrupt Enable Register
 */
typedef struct {
    UINT32  Fe:                 1;      /* [0]    FIFO empty interrupt enable */
    UINT32  Ff:                 1;      /* [1]    FIFO full interrupt enable */
    UINT32  Reserved1:          1;      /* [2]    Reserved */
    UINT32  Ftv:                1;      /* [3]    FIFO valid threshold interrupt enable */
    UINT32  Ti:                 1;      /* [4]    Transmitter idle interrupt enable */
    UINT32  Reserved2:          27;     /* [31:5] Reserved */
} AMBA_I2S_TX_INTERRUPT_ENABLE_REG_s;

/*
 * Definitions & Constants for I2S Receiver Interrupt Enable Register
 */
typedef struct {
    UINT32  Fe:                 1;      /* [0]    FIFO empty interrupt enable */
    UINT32  Ff:                 1;      /* [1]    FIFO full interrupt enable */
    UINT32  Fo:                 1;      /* [2]    FIFO overflow interrupt enable */
    UINT32  Ftv:                1;      /* [3]    FIFO valid threshold interrupt enable */
    UINT32  Ri:                 1;      /* [4]    Receiver idle interrupt enable */
    UINT32  Reserved:           27;     /* [31:5] Reserved */
} AMBA_I2S_RX_INTERRUPT_ENABLE_REG_s;

/*
 * Definitions & Constants for I2S Receiver Rx Echo Register
 */
typedef struct {
    UINT32  Echo:               1;      /* [0]    0 - No echo, 1 - Echo Rx data to Tx as well as forwarding to main memory */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_I2S_RX_ECHO_REG_s;

/*
 * Definitions & Constants for I2S 24-Bit Multiplexed Mode Register
 */
typedef struct {
    UINT32  Multi24En:          1;      /* [0]    24-bit multiplexed mode: 0 - Disable, 1 - Enable */
    UINT32  Reserved:           31;     /* [31:1] Reserved */
} AMBA_I2S_24BITMUX_MODE_REG_s;

/*
 * Definitions & Constants for I2S Shift Register
 */
typedef struct {
    UINT32  TxShiftEn:          1;      /* [0]    0 - Disable Tx shift, 1 - Enable Tx shift */
    UINT32  RxShiftEn:          1;      /* [1]    0 - Disable Rx shift, 1 - Enable Rx shift */
    UINT32  TxShift16bEn:       1;      /* [2]    0 - Disable Tx shift 16 bit, 1 - Enable Tx shift 16 bit*/
    UINT32  RxShift16bEn:       1;      /* [3]    0 - Disable Rx shift 16 bit, 1 - Enable Rx shift 16 bit*/
    UINT32  Reserved:           28;     /* [31:4] Reserved */
} AMBA_I2S_SHIFT_REG_s;

/*
 * Definitions & Constants for I2S Channel Select Register
 */
typedef struct {
    UINT32  ChannelSelect:      2;      /* [1:0]  Select channel */
    UINT32  Reserved:           30;     /* [31:2] Reserved */
} AMBA_I2S_CHANNEL_SELECT_REG_s;

/*
 * Definitions & Constants for I2S Ws Control Register
 */
typedef struct {
    UINT32  WsEn:               1;      /* [0]  Ws enable */
    UINT32  ChannelSelect:      1;      /* [1]  Ws reset: 0 - reset to 0, 1 - Enable output*/
    UINT32  Reserved:           30;     /* [31:2] Reserved */
} AMBA_I2S_WS_CTRL_REG_s;

typedef struct {
    volatile UINT32                             Mode;                /* 0x00: I2S Mode Register */
    volatile AMBA_I2S_RX_CTRL_REG_s             RxCtrl;              /* 0x04: I2S Receiver Control Register */
    volatile AMBA_I2S_TX_CTRL_REG_s             TxCtrl;              /* 0x08: I2S Transmitter Control Register */
    volatile UINT32                             Wlen;                /* 0x0c: I2S Word Length Register */
    volatile UINT32                             Wpos;                /* 0x10: I2S Word Position Register */
    volatile UINT32                             Slot;                /* 0x14: I2S Slot Register */
    volatile AMBA_I2S_TX_FIFO_LTH_REG_s         TxFifoLth;           /* 0x18: I2S Transmitter FIFO Threshold Register */
    volatile AMBA_I2S_RX_FIFO_GTH_REG_s         RxFifoGth;           /* 0x1c: I2S Receiver FIFO Threshold Register */
    volatile AMBA_I2S_CLK_CTRL_REG_s            ClkCtrl;             /* 0x20: I2S Clock Control Register */
    volatile AMBA_I2S_INIT_REG_s                Init;                /* 0x24: I2S Initial Control Register */
    volatile AMBA_I2S_TX_FIFO_FLAG_REG_s        TxFifoFlag;          /* 0x28: I2S Transmitter Status Register */
    volatile UINT32                             TxLeftData;          /* 0x2c: I2S Transmitter Left Channel Data Register */
    volatile AMBA_I2S_TX_RIGHT_DATA_REG_s       TxRightData;         /* 0x30: I2S Transmitter Right Channel Data Register */
    volatile AMBA_I2S_RX_FIFO_FLAG_REG_s        RxFifoFlag;          /* 0x34: I2S Receiver Status Register */
    volatile AMBA_I2S_RX_DATA_REG_s             RxData;              /* 0x38: I2S Receiver Data Register */
    volatile AMBA_I2S_TX_FIFO_CNTR_REG_s        TxFifoCntr;          /* 0x3c: I2S Transmitter FIFO Counter Register */
    volatile AMBA_I2S_RX_FIFO_CNTR_REG_s        RxFifoCntr;          /* 0x40: I2S Receiver FIFO Counter Register */
    volatile AMBA_I2S_TX_INTERRUPT_ENABLE_REG_s TxInterruptEnable;   /* 0x44: I2S Transmitter Interrupt Enable Register */
    volatile AMBA_I2S_RX_INTERRUPT_ENABLE_REG_s RxInterruptEnable;   /* 0x48: I2S Receiver Interrupt Enable Register */
    volatile AMBA_I2S_RX_ECHO_REG_s             RxEcho;              /* 0x4c: I2S Receiver Rx Echo Register */
    volatile AMBA_I2S_24BITMUX_MODE_REG_s       BitmuxMode24;        /* 0x50: I2S 24-Bit Multiplexed Mode Register */
    volatile AMBA_I2S_SHIFT_REG_s               Shift;               /* 0x54: I2S Shift Register */
    volatile AMBA_I2S_CHANNEL_SELECT_REG_s      ChannelSelect;       /* 0x58: I2S Channel Select Register */
    volatile AMBA_I2S_WS_CTRL_REG_s             WsCtrl;              /* 0x5c: I2S Ws Control Register */
    volatile UINT32                             Reserved1[8];        /* 0x60 - 0x7c */
    volatile UINT32                             RxDataDma;           /* 0x80: I2S Receive DMA Data Register */
    volatile UINT32                             Reserved2[15];       /* 0x84 - 0xbc */
    volatile UINT32                             TxLeftDataDma;       /* 0xC0: I2S Transmit DMA Left Data Register */
} AMBA_I2S_REG_s;

extern AMBA_I2S_REG_s *pAmbaI2S_Reg[AMBA_NUM_I2S_CHANNEL];

#endif /* AMBA_REG_I2S_H */
