/*
 * Copyright (c) 2021 Ambarella International LP
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

#ifndef AMBA_REG_SPI_H
#define AMBA_REG_SPI_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * SPI: Control Registers
 */
typedef struct {
    UINT32  DataFrameSize:      4;      /* [3:0]: Data Frame Length: 3 - 15 */
    UINT32  FrameFormat:        2;      /* [5:4]: Serial Protocol: 0 = motorola SPI */
    UINT32  ClkPhase:           1;      /* [6]: Clock Phase: clock toggles 0 = in middle of, 1 = at start of first data bit */
    UINT32  ClkPolarity:        1;      /* [7]: Clock Polarity: Inactive state of serial clock is 0 = low, 1 = high */
    UINT32  TransferMode:       2;      /* [9:8]: 0 = TxRx, 1 = Tx only, 2 = Rx only */
    UINT32  SlaveOutputEnable:  1;      /* [10]: Slave TXD: 0 = enable, 1 = disable */
    UINT32  ShiftRegLoop:       1;      /* [11]: 0 = Normal mode, 1 = Test mode */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32  Reserved:           5;      /* [16:12]: Reserved */
#elif defined(CONFIG_SOC_CV5)
    UINT32  CtrlFrmSize:        4;      /* [15:12]: control word for the Microwire frame format */
    UINT32  TransferDoneInt:    1;      /* [16]: Transfer done interrupt enable */
#else
    UINT32  Reserved:           4;      /* [15:12]: Reserved */
    UINT32  TransferDoneInt:    1;      /* [16]: 1 = Enable transfer done interrupt */
#endif
    UINT32  ResidueFlushMode:   1;      /* [17]: Enable residue flush mode */
    UINT32  TxLsbFirst:         1;      /* [18]: Tx LSB/MSB: 0 = MSB first, 1 = LSB first */
    UINT32  RxLsbFirst:         1;      /* [19]: Rx LSB/MSB: 0 = MSB first, 1 = LSB first */
    UINT32  PackMode:           1;      /* [20]: PackMode */
    UINT32  FrameCounterEnable: 1;      /* [21]: Enable frame counter */
    UINT32  ExtraRxdMargin:     4;      /* [25:22]: Enable round chip delay compensation mode */
    UINT32  ByteWiseMode:       1;      /* [26]: If DFC <= 8, enable this register to double FIFO's length. If DFC > 8, need to set 0 */
    UINT32  SpiHold:            1;      /* [27]: Hold if TX FIFO frame does not reach the target frame count */
    UINT32  IrqMode:            1;      /* [28]: 0 = Read clear (Legacy scheme), 1 = Write clear */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
    UINT32  Reserved0:          3;      /* [31:29]: Reserved */
#else
    UINT32  SuperMode:          1;      /* [29]: SSI continuously transmits data if this register is set. It is only used in SPI mode (frf=2'b00)*/
    UINT32  Reserved0:          2;      /* [31:30]: Reserved */
#endif
} AMBA_SPI_CTRL0_REG_s;

typedef struct {
    UINT32  RxNumDataFrames:    16;     /* [15:0]: Rx Number of Data Frames */
    UINT32  Reserved:           16;     /* [31:16] */
} AMBA_SPI_CTRL1_REG_s;

/*
 * SPI: Enable Register
 */
typedef struct {
    UINT32  Flag:               1;      /* [0]: 1 - Enable; 0 - Disable */
    UINT32  Reserved:           31;     /* [31:1]: Reserved */
} AMBA_SPI_ENABLE_REG_s;

/*
 * SPI: Slave Select Enable Flag/Select Signal Polarity Register
 */
typedef struct {
    UINT32  Slave0:             1;      /* [0]: Activate Slave Select pin of device 0 */
    UINT32  Slave1:             1;      /* [1]: Activate Slave Select pin of device 1 */
    UINT32  Slave2:             1;      /* [2]: Activate Slave Select pin of device 2 */
    UINT32  Slave3:             1;      /* [3]: Activate Slave Select pin of device 3 */
    UINT32  Slave4:             1;      /* [4]: Activate Slave Select pin of device 4 */
    UINT32  Slave5:             1;      /* [5]: Activate Slave Select pin of device 5 */
    UINT32  Slave6:             1;      /* [6]: Activate Slave Select pin of device 6 */
    UINT32  Slave7:             1;      /* [7]: Activate Slave Select pin of device 7 */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_SPI_SLAVE_CTRL_REG_s;

/*
 * SPI: Baud Rate Select (SSI Clock Divider) Register
 */
typedef struct {
    UINT32  ClkDivider:         16;     /* [15:0]: Clock Divider */
    UINT32  Reserved:           16;     /* [31:16]: Reserved */
} AMBA_SPI_BAUD_RATE_REG_s;

/*
 * SPI: Transmit/Receive FIFO Threshold Register
 */
typedef struct {
    UINT32  Value:              6;      /* [5:0]: FIFO Threshold */
    UINT32  Reserved:           26;     /* [31:6]: Reserved */
} AMBA_SPI_FIFO_THRESHOLD_REG_s;

/*
 * SPI: Transmit/Receive FIFO Level (Data Size) Register
 */
typedef struct {
    UINT32  Value:              6;      /* [5:0]: number of valid data entries */
    UINT32  Reserved:           26;     /* [31:6]: Reserved */
} AMBA_SPI_FIFO_DATA_SIZE_REG_s;

/*
 * SPI: Status Register
 */
typedef struct {
    UINT32  Busy:               1;      /* [0]: SSI Busy flag, 0 = idle/disable, 1 = active */
    UINT32  TxFifoNotFull:      1;      /* [1]: 0 = Transmit FIFO is full */
    UINT32  TxFifoEmpty:        1;      /* [2]: 0 = Transmit FIFO is not empty */
    UINT32  RxFifoNotEmpty:     1;      /* [3]: 0 = Receive FIFO is empty */
    UINT32  RxFifoFull:         1;      /* [4]: 0 = Receive FIFO is not full */
    UINT32  TxError:            1;      /* [5]: Transmission Error, 0 = No Error */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
    UINT32  DataCollisionError: 1;      /* [6]: Data Collision Error, 0 = No Error */
    UINT32  Reserved:           25;     /* [31:7]: Reserved */
#else
    UINT32  Reserved:           26;     /* [31:6]: Reserved */
#endif
} SPI_STATUS_REG_s;

/*
 * SPI: Interrupt Status (Enable or Mask, Raw or After Masked) Register
 */
typedef struct {
    UINT32  TxFifoEmpty:            1;  /* [0]: 1 - Transmit FIFO Empty */
    UINT32  TxFifoOverflow:         1;  /* [1]: 1 - Transmit FIFO Overflow */
    UINT32  RxFifoUnderflow:        1;  /* [2]: 1 - Receive FIFO Underflow */
    UINT32  RxFifoOverflow:         1;  /* [3]: 1 - Receive FIFO Overflow */
    UINT32  RxFifoFull:             1;  /* [4]: 1 - Receive FIFO Full */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
    UINT32  MultiMasterContention:  1;  /* [5]: 1 - Multi-Master Contention */
    UINT32  Reserved0:              2;  /* [7:6] Reserved */
#else
    UINT32  Reserved0:              3;  /* [7:5] Reserved */
#endif
    UINT32  FrameCounter:           1;  /* [8] Frame counter reaches target value */
    UINT32  TransferDone:           1;  /* [9] SSI transfer done */
    UINT32  Reserved1:              22; /* [31:10]: Reserved */
} AMBA_SPI_IRQ_STATUS_REG_s;

/*
 * SPI: Interrupt Clear Registers
 */
typedef struct {
    struct {
        UINT32  Flag:               1;      /* [0]: Reflect the status of SSI_TXO_INTR */
        UINT32  Reserved:           31;     /* [31:1]: Reserved */
    } TxFifoOverflow;

    struct {
        UINT32  Flag:               1;      /* [0]: Reflect the status of SSI_RXO_INTR */
        UINT32  Reserved:           31;     /* [31:1]: Reserved */
    } RxFifoOverflow;

    struct {
        UINT32  Flag:               1;      /* [0]: Reflect the status of SSI_RXU_INTR */
        UINT32  Reserved:           31;     /* [31:1]: Reserved */
    } RxFifoUnderflow;

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
    struct {
        UINT32  Flag:               1;      /* [0]: Reflect the status of SSI_MST_INTR */
        UINT32  Reserved:           31;     /* [31:1]: Reserved */
    } MultiMasterContention;
#else
    UINT32  Reserved;
#endif

    struct {
        UINT32  Flag:               1;      /* [0]: Reflect either onf of those 4 IRQ Status */
        UINT32  Reserved:           31;     /* [31:1]: Reserved */
    } AnyOne;
} AMBA_SPI_IRQ_CLEAR_REG_s;

/*
 * SPI: Data Read/Write Data Buffer Register
 */
typedef struct {
    UINT32  RxEnable:           1;     /* [0]: 1 = Rx DMA request enable */
    UINT32  TxEnable:           1;     /* [1]: 1 = Tx DMA request enable */
    UINT32  Reserved:           30;    /* [31:2]: Reserved */
} AMBA_SPI_DMA_CTEL_REG_s;

/*
 * SPI: All Registers
 */
typedef struct {
    volatile AMBA_SPI_CTRL0_REG_s           Ctrl0;              /* 0x000(RW): SPI Control Register 0 */
    volatile AMBA_SPI_CTRL1_REG_s           Ctrl1;              /* 0x004(RW): SPI Control Register 1 */
    volatile AMBA_SPI_ENABLE_REG_s          Enable;             /* 0x008(RW): SSI Enable Register */
    volatile UINT32                         Reserved0;          /* 0x00C: Reserved */
    volatile UINT32                         ChipSelect;         /* 0x010(RW): Slave Select Enable Register */
    volatile UINT32                         BaudRate;           /* 0x014(RW): Baud Rate Select Register */
    volatile AMBA_SPI_FIFO_THRESHOLD_REG_s  TxFifoThreshold;    /* 0x018(RW): Transmit FIFO Threshold Level Register */
    volatile AMBA_SPI_FIFO_THRESHOLD_REG_s  RxFifoThreshold;    /* 0x01C(RW): Receive FIFO Threshold Level Register */
    volatile AMBA_SPI_FIFO_DATA_SIZE_REG_s  TxFifoDataSize;     /* 0x020(RW): Transmit FIFO Level Register */
    volatile AMBA_SPI_FIFO_DATA_SIZE_REG_s  RxFifoDataSize;     /* 0x024(RW): Receive FIFO Level Register */
    volatile UINT32                         Status;             /* 0x028(RO): Status Register */
    volatile UINT32                         IrqEnable;          /* 0x02C(RW): Interrupt Mask Register */
    volatile AMBA_SPI_IRQ_STATUS_REG_s      IrqStatus;          /* 0x030(RO): Interrupt Status Register */
    volatile AMBA_SPI_IRQ_STATUS_REG_s      IrqRawStatus;       /* 0x034(RO): Raw Interrupt Status */
    volatile AMBA_SPI_IRQ_CLEAR_REG_s       IrqClear;           /* 0x038-0x048(R): Interrupt Clear Registers */
    volatile UINT32                         DmaCtrl;            /* 0x04C(RW): DMA Control Register */
    volatile UINT32                         Reserved1[2];       /* 0x050-0x054: Reserved */
    volatile UINT32                         ID;                 /* 0x058(RO): Identification Register */
    volatile UINT32                         Version;            /* 0x05C(RO): Version ID Register */
    volatile UINT32                         DataBuf[128];       /* 0x060-0x25C(RW): Data Buffer Register */
    volatile UINT32                         ChipSelectPolarity; /* 0x260(RW): Slave Select Signal Polarity Register */
    volatile UINT32                         SclkDelay;          /* 0x264(RW): SSI[N]_CLK Delay Counter (LSB 17-bit) */
    volatile UINT32                         Reserved2[5];       /* 0x268-0x278: Reserved */
    volatile UINT32                         TargetFrameCount;   /* 0x27C(RW): Target Frame Count for Interrupt (LSB 17-bit) */
    volatile UINT32                         FrameCount;         /* 0x280(RW): Current Frame Count (LSB 17-bit) */
    volatile UINT32                         FrameCountIrqStatus;/* 0x284(RW): Frame Counter Reach Interrupt Status (read clear) */
    volatile UINT32                         TxFrameCount;       /* 0x288(RW): Total Tx Frame Count (LSB 16-bit) */
    volatile UINT32                         TxDoneIrqStatus;    /* 0x28C(RW): Transfer Done Interrupt Status (read clear) */
#if defined(CONFIG_SOC_CV5)
    volatile UINT32                         DeselectIrqClr;     /* 0x290(RW): Deselect Eage Interrupt Clear */
#endif
} AMBA_SPI_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_SPI_REG_s *pAmbaSPI_MasterReg[AMBA_NUM_SPI_MASTER];
extern AMBA_SPI_REG_s *pAmbaSPI_SlaveReg[AMBA_NUM_SPI_SLAVE];

#endif /* AMBA_REG_SPI_H */
