/*
 *  @FileName       :: AmbaReg_SpiNOR.h
 *
 *  @Copyright      :: Copyright (C) 2019 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for SPI-NOR Flash APIs
 */

#ifndef AMBA_REG_SPINOR_H
#define AMBA_REG_SPINOR_H

/****************************************************/
/* Controller registers definitions                 */
/****************************************************/
/*
 * SPI-NOR Command fmt Register
 */
typedef struct {
    UINT32  DataLength:             22;     /* [21:0] Data Access length */
    UINT32  DummyLength:            5;      /* [26:22] Dummy Cycle length */
    UINT32  AddrLength:             3;      /* [29:27] Address length */
    UINT32  CmdLength:              2;      /* [31:30] Command length */
} AMBA_NOR_CMDFMT_REG_s;

/* NOR_CMDFMT_REG */
#define NOR_CMD_LEN_0BYTE       0x0
#define NOR_CMD_LEN_1BYTE       0x1
#define NOR_CMD_LEN_2BYTE       0x2
#define NOR_CMD_LEN_3BYTE       0x3

#define NOR_DATA_LEN_0BYTE      0x0
#define NOR_DATA_LEN_1BYTE      0x1
#define NOR_DATA_LEN_2BYTE      0x2
#define NOR_DATA_LEN_3BYTE      0x3
#define NOR_DATA_LEN_4BYTE      0x4
#define NOR_DATA_LEN_5BYTE      0x5
#define NOR_DATA_LEN_6BYTE      0x6
#define NOR_DATA_LEN_7BYTE      0x7

/*
 * SPI-NOR DTR Ctrl Register
 */
typedef struct {
    UINT32  DataReadn:          1;      /* [0] Data Part Read Mode */
    UINT32  DataWriteen:        1;      /* [1] Data Part Write Mode */
    UINT32  Reserved:           7;      /* [8:2] */
    UINT32  RxLane:             1;      /* [9] RxLANE count */
    UINT32  NumDataLane:        2;      /* [11:10] DataLANE count */
    UINT32  NumAddrLane:        2;      /* [13:12] AddrLANE count */
    UINT32  NumCmdLane:         2;      /* [15:14] CmdLANE count */
    UINT32  Reserved1:          8;      /* [23:16] */
    UINT32  LSBFirst:           1;      /* [24] LSB & MSB First */
    UINT32  Reserved2:          3;      /* [27:25] */
    UINT32  DataDTR:            1;      /* [28] Data Double Transfer Rate */
    UINT32  DummyDTR:           1;      /* [29] Dummy Double Transfer Rate */
    UINT32  AddressDTR:         1;      /* [30] Address DTR MODE */
    UINT32  CmdDTR:             1;      /* [31] Cmd DTR MODE */
} AMBA_NOR_DTR_CTRL_REG_s;

/* NOR_DTR_CTRL_REG */
#define NOR_DTR_MBR_FIRST       0x0
#define NOR_DTR_LSB_FIRST       0x1

#define NOR_DTR_DATA_LANE1      0x0
#define NOR_DTR_DATA_LANE2      0x1
#define NOR_DTR_DATA_LANE4      0x2
#define NOR_DTR_DATA_LANE8      0x3

#define NOR_DTR_ADDR_LANE1      0x0
#define NOR_DTR_ADDR_LANE2      0x1
#define NOR_DTR_ADDR_LANE4      0x2
#define NOR_DTR_ADDR_LANE8      0x3

#define NOR_DTR_CMD_LANE1       0x0
#define NOR_DTR_CMD_LANE2       0x1
#define NOR_DTR_CMD_LANE4       0x2
#define NOR_DTR_CMD_LANE8       0x3

/*
 * SPI-NOR Flow Control
 */
typedef struct {
    UINT32  RxSampleDelay:      5;      /* [4:0] Adjust RX sampling Data Phase */
    UINT32  Reserved:           5;      /* [9:5] */
    UINT32  ClkDivider:         8;      /* [17:10] Divide reference clock */
    UINT32  ChipSelect:         8;      /* [25:18] CEN for multiple device*/
    UINT32  HoldSwitchphase:    1;      /* [26] Clock will remain in standby mode*/
    UINT32  SpiClkPolarity:     1;      /* [27] Clock will remain in standby mode*/
    UINT32  Hold:               3;      /* [30:28] For Flow control purpose */
    UINT32  FlowControl:        1;      /* [31] Flow control enable*/
} AMBA_NOR_FLOWCTRL_REG_s;

/*
 * SPI-NOR Command Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Cmd0:               8;      /* [7:0] Command 0 for SPI Device*/
        UINT32  Cmd1:               8;      /* [15:8] Command 1 for SPI Device */
        UINT32  Cmd2:               8;      /* [23:16] Command 2 for SPI Device*/
        UINT32  Reserved:           8;      /* [31:24] */
    } Bits;
} AMBA_NOR_COMMAND_REG_u;

/*
 * SPI-NOR ADDRESSS Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Addr4:              8;      /* [7:0] Address 4 Field for SPI Device*/
        UINT32  Addr5:              8;      /* [15:8] Address 5 Field for SPI Device */
        UINT32  Addr6:              8;      /* [23:16] Address 6 Field for SPI Device */
        UINT32  Reserved:           8;      /* [31:24] */
    } Bits;
} AMBA_NOR_ADDR1_REG_u;

/*
 * SPI-NOR ADDRESSS1 Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Addr0:              8;      /* [7:0] Address 0 Field for SPI Device */
        UINT32  Addr1:              8;      /* [15:8] Address 1 Field for SPI Device */
        UINT32  Addr2:              8;      /* [23:16] Address 2 Field for SPI Device */
        UINT32  Addr3:              8;      /* [31:24] Address 3 Field for SPI Device */
    } Bits;
} AMBA_NOR_ADDR0_REG_u;

/*
 * SPI-NOR DMA Enable Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  RxDmaEnable:        1;      /* [0]  Transmit DMA Enable */
        UINT32  TxDmaEnable:        1;      /* [1]  Transmit DMA Enable */
        UINT32  Reserved:           30;     /* [31:2] */
    } Bits;
} AMBA_NOR_DMA_EN_REG_u;

/*
 * SPI-NOR TX Threshold Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Value:              9;      /* [8:0] The threshold of Transmit FIFO */
        UINT32  Reserved:           23;     /* [31:9] */
    } Bits;
} AMBA_NOR_TXFIFO_THLV_REG_u;

/*
 * SPI-NOR RX Threshold Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Value:              9;      /* [8:0] The threshold of Receive FIFO */
        UINT32  Reserved:           23;     /* [31:9] */
    } Bits;
} AMBA_NOR_RXFIFO_THLV_REG_u;

/*
 * SPI-NOR TX Fifo Level Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  DataSize:           9;      /* [8:0] The Level of Transmit FIFO */
        UINT32  Reserved:           23;     /* [31:9] */
    } Bits;
} AMBA_NOR_TXFIFO_LEV_REG_u;

/*
 * SPI-NOR RX Fifo Level Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  DataSize:           9;      /* [8:0] The Level of Receive FIFO */
        UINT32  Reserved:           23;     /* [31:9] */
    } Bits;
} AMBA_NOR_RXFIFO_LEV_REG_u;

/*
 * SPI-NOR FIFO Status Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Reserved:           1;      /* [0] */
        UINT32  TxFifoNotFull:      1;      /* [1] */
        UINT32  TxFifoEmpty:        1;      /* [2] TX FIFO empty*/
        UINT32  RxFifoNotEmpty:     1;      /* [3] */
        UINT32  RxFifoFull:         1;      /* [4] RX FIFO Full */
        UINT32  Reserved1:          27;     /* [31:5] */
    } Bits;
} AMBA_NOR_FIFO_STA_REG_u;

/*
 * SPI-NOR Interrupt Mask Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  TxAlmostEmptyMsk:   1;      /* [0] TX FIFO almost empty */
        UINT32  TxOverflowMsk:      1;      /* [1]  Transmit Overflow interrupt Mask*/
        UINT32  RxUnderflowMsk:     1;      /* [2] Receive Underflow interrupt Mask*/
        UINT32  RxOverflowMsk:      1;      /* [3] Receive Overflow interrupt Mask */
        UINT32  RxAlmostfullMsk:    1;      /* [4] Receive FIFO Almost Full */
        UINT32  DataLenthreacMsk:   1;      /* [5] Transmit complete interrupt Mask */
        UINT32  TxunderflowMsk:     1;      /* [6] Transmit Underflow interrupt Mask */
        UINT32  Reserved:           25;     /* [31:7] */
    } Bits;
} AMBA_NOR_IRQ_MASK_REG_u;

#define NOR_TXEMPTYINTR_MASK        0x00000001
#define NOR_TXOVERFLOWINTR_MASK     0x00000002
#define NOR_RXUNDERFLOWINTR_MASK    0x00000004
#define NOR_RXOVERFLOWINTR_MASK     0x00000008
#define NOR_RXFULLINTR_MASK         0x00000010
#define NOR_DATALENREACHINTR_MASK   0x00000020
#define NOR_TXUNDERFLOWINTR_MASK    0x00000040
#define NOR_INTR_ALL                0x0000007f

/*
 * SPI-NOR Interrupt Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  TxAlmostEmpty:      1;      /* [0] TX FIFO almost empty */
        UINT32  TxOverflow:         1;      /* [1]  Transmit Overflow interrupt */
        UINT32  RxUnderflow:        1;      /* [2] Receive Underflow interrupt */
        UINT32  RxOverflow:         1;      /* [3] Receive Overflow interrupt */
        UINT32  RxAlmostfull:       1;      /* [4] Receive FIFO Almost Full */
        UINT32  DataLenthreach:     1;      /* [5] Transmit complete interrupt */
        UINT32  Txunderflow:        1;      /* [6] Transmit Underflow interrupt */
        UINT32  Reserved:           25;     /* [31:7] */
    } Bits;
} AMBA_NOR_IRQ_REG_u;

/*
 * SPI-NOR TXFIFO Reset Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Reset:              1;      /* [0] TX FIFO Software Reset */
        UINT32  Reserved:           31;     /* [31:1] */
    } Bits;
} AMBA_NOR_TXFIFO_RESET_REG_u;

/*
 * SPI-NOR RXFIFO Reset Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Reset:              1;      /* [0] RX FIFO Software Reset */
        UINT32  Reserved:           31;     /* [31:1] */
    } Bits;
} AMBA_NOR_RXFIFO_RESET_REG_u;

/*
 * SPI-NOR Start Receiving/Transmitting Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Strttrx:            1;      /* [0] Start Transmitting or Receiving */
        UINT32  Reserved:           31;     /* [31:1] */
    } Bits;
} AMBA_NOR_START_TR_REG_u;

/*
 * SPI-NOR TX DATA Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Data:               8;      /* [7:0] Data to be written to the NOR device */
        UINT32  Reserved:           24;     /* [31:8] */
    } Bits;
} AMBA_NOR_TXDATA_REG_u;

/*
 * SPI-NOR RX DATA Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Data:               8;      /* [7:0] Data to be read from the NOR device */
        UINT32  Reserved:           24;     /* [31:8] */
    } Bits;
} AMBA_NOR_RXDATA_REG_u;

/*
 * H22 SPI-NOR All Registers
 */
typedef struct {
    volatile AMBA_NOR_CMDFMT_REG_s              CmdFmt;             /* 0x000: NOR flash the format of command  */
    volatile AMBA_NOR_DTR_CTRL_REG_s            DTR_Ctrl;           /* 0x004: NOR flash DTR mode control register  */
    volatile AMBA_NOR_FLOWCTRL_REG_s            FlowControl;        /* 0x008: NOR flash flowcontrol */
    volatile AMBA_NOR_COMMAND_REG_u             Command;            /* 0x00C: NOR flash command for SPI device*/
    volatile AMBA_NOR_ADDR1_REG_u               Addr1;              /* 0x010: NOR flash Address 6~4 Field for SPI Command */
    volatile AMBA_NOR_ADDR0_REG_u               Addr0;              /* 0x014: NOR flash Address 3~0 Field for SPI Command */
    volatile AMBA_NOR_DMA_EN_REG_u              EnableDMA;          /* 0x018: Transmit/Receive DMA Enable  */
    volatile AMBA_NOR_TXFIFO_THLV_REG_u         TxFifoThreshold;    /* 0x01C: Transmit FIFO Threshold Level */
    volatile AMBA_NOR_RXFIFO_THLV_REG_u         RxFifoThreshold;    /* 0x020: Receive FIFO Threshold Level */
    volatile AMBA_NOR_TXFIFO_LEV_REG_u          TxLevel;            /* 0x024: Transmit FIFO Level */
    volatile AMBA_NOR_RXFIFO_LEV_REG_u          RxLevel;            /* 0x028: Receive FIFO Level */
    volatile AMBA_NOR_FIFO_STA_REG_u            FIFOStatus;         /* 0x02C: The status of Transmit/Receive FIFO */
    volatile AMBA_NOR_IRQ_REG_u                 IrqEnable;          /* 0x030: NOR flash Transmit/Receive interrupt Mask */
    volatile AMBA_NOR_IRQ_REG_u                 IrqStatus;          /* 0x034: NOR flash Transmit/Receive done interrupt  */
    volatile AMBA_NOR_IRQ_REG_u                 IrqRawStatus;       /* 0x038: NOR flash Transmit/Receive Raw interrupt */
    volatile AMBA_NOR_IRQ_REG_u                 IrqClear;           /* 0x03C : NOR flash Transmit/Receive interrupt Clear status */
    volatile AMBA_NOR_TXFIFO_RESET_REG_u        TxFIFOReset;        /* 0x040: TX FIFIO Software Reset */
    volatile AMBA_NOR_RXFIFO_RESET_REG_u        RxFIFOReset;        /* 0x044: RX FIFIO Software Reset*/
    volatile UINT32                             Reserved1[2];       /* 0x048 - 0x04C: Reserved */
    volatile AMBA_NOR_START_TR_REG_u            Start;              /* 0x050: Start Transmitting or Receiveing */
    volatile UINT32                             Reserved[43];       /* 0x054 - 0x0FC: Reserved */
    volatile AMBA_NOR_TXDATA_REG_u              TxData[64];         /* 0x100 - 0x1FF */
    volatile AMBA_NOR_RXDATA_REG_u              RxData[64];         /* 0x200 - 0x2FF */
} AMBA_NOR_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_NOR_REG_s *pAmbaNOR_Reg;

#endif /* AMBA_REG_SPINOR_H */
