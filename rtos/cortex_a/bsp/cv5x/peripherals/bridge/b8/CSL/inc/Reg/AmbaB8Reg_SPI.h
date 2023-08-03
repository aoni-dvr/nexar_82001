/**
 *  @file AmbaReg_SPI.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for B6 SPI Control Registers
 *
 */

#ifndef AMBA_B8_REG_SPI_H
#define AMBA_B8_REG_SPI_H

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Control Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DataFrameSize:      4;      /* [3:0]: Data Frame Length: 3 - 15 */
    UINT32  FrameFormat:        2;      /* [5:4]: Serial Protocol: 0 = motorola SPI */
    UINT32  ClkPhase:           1;      /* [6]: Relationship of serial clk with slave select signal */
    UINT32  ClkIdleState:       1;      /* [7]: Inactive state of serial clk: 0 = Low, 1= High */
    UINT32  TransferMode:       2;      /* [9:8]: 0 = TxRx, 1 = Tx only, 2 = Rx only */
    UINT32  SlaveOutputEnable:  1;      /* [10]: Slave TXD: 0 = enable, 1 = disable */
    UINT32  ShiftRegLoop:       1;      /* [11]: 0 = Normal mode, 1 = Test mode */
    UINT32  Reserved0:          5;      /* [16:12]: Reserved */
    UINT32  ResidueMode:        1;      /* [17]: 1 = Enable Residue flush mode */
    UINT32  TxLfbFirst:         1;      /* [18]: 0 = MSB first, 1 = LSB first */
    UINT32  RxLfbFirst:         1;      /* [19]: 0 = MSB first, 1 = LSB first */
    UINT32  PackMode:           1;      /* [20]: Used for CPU or DMA to burst read data in byte or half-word access in order to save the memory space */
    UINT32  FrameCntEn:         1;      /* [21]: 1 = Enable Frame Counter */
    UINT32  ExtraRxdMargin:     4;      /* [25:22]: Enables round chip delay compensation mode */
    UINT32  ByteAccessMode:     1;      /* [26]: If DFS <= 8 bits, enable this register to double FIFO’s length */
    UINT32  SpiHold:            1;      /* [27]: When this bit is set, SSI/SPI mode state mechine will hold if TX FIFO frame doesn't reach the target frame count */
    UINT32  IrqMode:            1;      /* [28]: IRQ clear mechanism, 0: Read clear, 1: Write clear */
    UINT32  SuperMode:          1;      /* [29]: SSI will transmit data continuously if this register is set */
    UINT32  Reserved3:          2;      /* [31:30]: Reserved */
} B8_SPI_CTRL0_REG_s;

typedef struct {
    UINT32  RxNumDataFrames:    16;     /* [15:0]: Rx Number of Data Frames */
    UINT32  Reserved0:          16;     /* [31:16] */
} B8_SPI_CTRL1_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Enable Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Flag:               1;      /* [0]: 1 - Enable; 0 - Disable */
    UINT32  Reserved0:          31;     /* [31:1]: Reserved */
} B8_SPI_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Slave Select Enable Flag/Select Signal Polarity Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  EnableSlave0:       1;      /* [0]: Activate Slave Select pin of device 0 */
    UINT32  EnableSlave1:       1;      /* [1]: Activate Slave Select pin of device 1 */
    UINT32  EnableSlave2:       1;      /* [2]: Activate Slave Select pin of device 2 */
    UINT32  EnableSlave3:       1;      /* [3]: Activate Slave Select pin of device 3 */
    UINT32  EnableSlave4:       1;      /* [4]: Activate Slave Select pin of device 4 */
    UINT32  EnableSlave5:       1;      /* [5]: Activate Slave Select pin of device 5 */
    UINT32  EnableSlave6:       1;      /* [6]: Activate Slave Select pin of device 6 */
    UINT32  EnableSlave7:       1;      /* [7]: Activate Slave Select pin of device 7 */
    UINT32  Reserved0:          24;     /* [31:8]: Reserved */
} B8_SPI_SLAVE_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Baud Rate Select (SSI Clock Divider) Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ClkDivider:         16;     /* [15:0]: Clock Divider */
    UINT32  Reserved0:          16;     /* [31:16]: Reserved */
} B8_SPI_BAUD_RATE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Transmit/Receive FIFO Threshold Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Value:              7;      /* [6:0]: FIFO Threshold */
    UINT32  Reserved0:          25;     /* [31:7]: Reserved */
} B8_SPI_FIFO_THRESHOLD_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Transmit/Receive FIFO Level (Data Size) Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Value:              8;      /* [7:0]: Number of valid data entries */
    UINT32  Reserved0:          24;     /* [31:8]: Reserved */
} B8_SPI_FIFO_DATA_SIZE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Status Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Busy:               1;      /* [0]: SSI Busy flag, 0 = idle/disable, 1 = active */
    UINT32  TxFifoNotFull:      1;      /* [1]: 0 = Transmit FIFO is full */
    UINT32  TxFifoEmpty:        1;      /* [2]: 0 = Transmit FIFO is not empty */
    UINT32  RxFifoNotEmpty:     1;      /* [3]: 0 = Receive FIFO is empty */
    UINT32  RxFifoFull:         1;      /* [4]: 0 = Receive FIFO is not full */
    UINT32  TxError:            1;      /* [5]: Transmission Error, 0 = No Error */
    UINT32  Reserved0:          26;     /* [31:6]: Reserved */
} B8_SPI_STATUS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Interrupt Status (Enable or Mask, Raw or After Masked) Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TxFifoEmpty:            1;  /* [0]: 1 - Transmit FIFO Empty */
    UINT32  TxFifoOverllow:         1;  /* [1]: 1 - Transmit FIFO Overflow */
    UINT32  RxFifoUnderflow:        1;  /* [2]: 1 - Receive FIFO Underflow */
    UINT32  RxFifoOverflow:         1;  /* [3]: 1 - Receive FIFO Overflow */
    UINT32  RxFifoFull:             1;  /* [4]: 1 - Receive FIFO Full */
    UINT32  Reserved0:              3;  /* [7:5]: Reserved */
    UINT32  FrmCntIrqMsk:           1;  /* [8]: Frame counter reaches target value interrupt mask, 0: Masked, 1: Not-masked */
    UINT32  TransferDoneMsk:        1;  /* [9]: SSI transfer done interrupt mask, 0: Masked, 1: Not-masked */
    UINT32  Reserved1:              22; /* [31:10]: Reserved */
} B8_SPI_IRQ_STATUS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Interrupt Clear Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    struct {
        UINT32  Flag:               1;      /* [0]: Reflect the status of SSI_TXO_INTR */
        UINT32  Reserved0:          31;     /* [31:1]: Reserved */
    } TxFifoOver;

    struct {
        UINT32  Flag:               1;      /* [0]: Reflect the status of SSI_RXO_INTR */
        UINT32  Reserved0:          31;     /* [31:1]: Reserved */
    } RxFifoOver;

    struct {
        UINT32  Flag:               1;      /* [0]: Reflect the status of SSI_RXU_INTR */
        UINT32  Reserved0:          31;     /* [31:1]: Reserved */
    } RxFifoUnderflow;

    UINT32 Reserved;                        /* 0x044: Reserved */

    struct {
        UINT32  Flag:               1;      /* [0]: Reflect either onf of those 4 IRQ Status */
        UINT32  Reserved0:          31;     /* [31:1]: Reserved */
    } AnyOne;
} B8_SPI_IRQ_CLEAR_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Data Read/Write Data Buffer Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Data:               16;     /* [15:0]: Data Read/Write Data Buffer */
    UINT32  Reserved0:          16;     /* [31:16]: Reserved */
} B8_SPI_DATA_BUF_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Sclk Delay Counter Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SclkOutDelay:       17;     /* [16:0]: Delay Cycles after Slave Select Asserted */
    UINT32  Reserved0:          15;     /* [31:17]: Reserved */
} B8_SPI_SCLK_DELAY_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Frame Count Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Count:              17;     /* [16:0]: Count */
    UINT32  Reserved0:          15;     /* [31:17]: Reserved */
} B8_SPI_FRAME_COUNT_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : TX Frame Count Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Count:              16;     /* [15:0]: Count */
    UINT32  Reserved0:          16;     /* [31:16]: Reserved */
} B8_SPI_TX_FRAME_COUNT_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Frame Counter Reaches Interrupt Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  FrameCntIrq:        1;      /* [0]: Frame Counter Reaches Interrupt */
    UINT32  Reserved0:          31;     /* [31:1]: Reserved */
} B8_SPI_FRAME_COUNT_IRQ_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : Frame Counter Reaches Interrupt Clear Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Flag:               1;      /* [0]: When irq_mode = 0, read this register to clear transfer_doneir */
    UINT32  Reserved0:          31;     /* [31:1]: Reserved */
} B8_SPI_FRAME_COUNT_IRQ_CLEAR_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 SPI : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_SPI_CTRL0_REG_s                  Ctrl0;              /* 0x000(RW): SPI Control Register 0 */
    volatile B8_SPI_CTRL1_REG_s                  Ctrl1;              /* 0x004(RW): SPI Control Register 1 */
    volatile B8_SPI_ENABLE_REG_s                 Enable;             /* 0x008(RW): SSI Enable Register */
    volatile UINT32                              Reserved0;          /* 0x00C: Reserved */
    volatile B8_SPI_SLAVE_CTRL_REG_s             ChipSelect;         /* 0x010(RW): Slave Select Enable Register */
    volatile B8_SPI_BAUD_RATE_REG_s              BaudRate;           /* 0x014(RW): Baud Rate Select Register */
    volatile B8_SPI_FIFO_THRESHOLD_REG_s         TxFifoThreshold;    /* 0x018(RW): Transmit FIFO Threshold Level Register */
    volatile B8_SPI_FIFO_THRESHOLD_REG_s         RxFifoThreshold;    /* 0x01C(RW): Receive FIFO Threshold Level Register */
    volatile B8_SPI_FIFO_DATA_SIZE_REG_s         TxFifoDataSize;     /* 0x020(RW): Transmit FIFO Level Register */
    volatile B8_SPI_FIFO_DATA_SIZE_REG_s         RxFifoDataSize;     /* 0x024(RW): Receive FIFO Level Register */
    volatile B8_SPI_STATUS_REG_s                 Status;             /* 0x028(R): Status Register */
    volatile B8_SPI_IRQ_STATUS_REG_s             IrqMask;            /* 0x02C(RW): Interrupt Mask Register */
    volatile B8_SPI_IRQ_STATUS_REG_s             IrqStatus;          /* 0x030(R): Interrupt Status Register */
    volatile B8_SPI_IRQ_STATUS_REG_s             IrqRawStatus;       /* 0x034(R): Interrupt Raw Status Register */
    volatile B8_SPI_IRQ_CLEAR_REG_s              IrqClear;           /* 0x038-0x048(R): Interrupt Clear Registers */
    volatile UINT32                              DmaCtrl;            /* 0x04C: DMA FIFO Control Register */
    volatile UINT32                              Reserved1[2];       /* 0x050-0x054: Reserved */
    volatile UINT32                              ID;                 /* 0x058(R): Identification Register */
    volatile UINT32                              Version;            /* 0x05C(R): Version ID Register */
    volatile B8_SPI_DATA_BUF_REG_s               DataBuf[128];       /* 0x060-0x25C(RW): Data Buffer Registers */
    volatile B8_SPI_SLAVE_CTRL_REG_s             ChipSelectPol;      /* 0x260(RW): Slave Select Signal Polarity Register */
    volatile B8_SPI_SCLK_DELAY_REG_s             SclkDelay;          /* 0x264(RW): SPI_CLK Delay Counter Register */
    volatile UINT32                              Reserved2[5];       /* 0x268-0x278: Reserved */
    volatile B8_SPI_FRAME_COUNT_REG_s            TargetFrameCnt;     /* 0x27C(RW): Targrt Frame Count for Interrupt Register */
    volatile B8_SPI_FRAME_COUNT_REG_s            CurrentFrameCnt;    /* 0x280(R): Current Frame Count */
    volatile B8_SPI_FRAME_COUNT_IRQ_REG_s        IrqFrameCnt;        /* 0x284(R): Interrupt Frame Counter Reaches Register */
    volatile B8_SPI_TX_FRAME_COUNT_REG_s         TotalTxFrameCnt;    /* 0x288(RW): Total TX Frame Count Register */
    volatile B8_SPI_FRAME_COUNT_IRQ_CLEAR_REG_s  TransferIrqClear;   /* 0x28C(R): Transfer done Interrupt Clear Register */
} B8_SPI_REG_s;

#endif /* AMBA_B8_REG_SPI_H */
