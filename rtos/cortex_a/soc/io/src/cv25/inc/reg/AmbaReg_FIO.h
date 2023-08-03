/**
 *  @file AmbaReg_FIO.h
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
 *  @details Definitions & Constants for CV1 FIO (Flash I/O) Registers
 *
 */

#ifndef AMBA_REG_FIO_H
#define AMBA_REG_FIO_H

/*
 * CV1 Flash I/O: Control Register
 */
typedef struct {
    UINT32  Reserved0:          1;      /* [0]: Reserved */
    UINT32  RandomReadMode:     1;      /* [1]: 1 - Flash I/O Subsystem is in random read mode */
    UINT32  Reserved1:          1;      /* [2]: Reserved */
    UINT32  StopOnError:        1;      /* [3]: 1 - DMA will end early and an ERROR response will be returned */
    UINT32  Reserved2:          1;      /* [4]: Reserved */
    UINT32  Bch8Bits:           1;      /* [5]: 0 = 6-bit, 1 = 8-bit */
    UINT32  BchEnable:          1;      /* [6]: 1 - BCH encode/decode enable */
    UINT32  SkipBlankPageEcc:   1;      /* [7]: 1 - Enable Skip BCH decoding failure when the page is blank */
    UINT32  Reserved3:          24;     /* [31:8]: Reserved */
} AMBA_FIO_CTRL_REG_s;

/*
 * CV1 Flash I/O: Raw Interrupt Status Register
 */
typedef struct {
    UINT32  FioIntRaw:                 1;      /* [0]:(RW1C) 1 - Flash operation done Interrupt */
    UINT32  SnandLoopTimeoutRaw:       1;      /* [1]: Spinand timeout while executing auto states update */
    UINT32  AxiBusErrorRaw:            1;      /* [2]: Interrupt Status of DMA bus */
    UINT32  Reserved0:                 1;      /* [3]: Reserved  */
    UINT32  ErrorDetectedIntRaw:       1;      /* [4]:(RW1C) 1 - Interrupt status indicating there are BCH error bits greater or equal to ecc number corrected. */
    UINT32  ErrorNotCorrectableIntRaw: 1;      /* [5]:(RW1C) 1 - Interrupt status indicating there is uncorrectable BCH error detected. */
    UINT32  Reserved1:                 26;     /* [31:6]: Reserved  */
} AMBA_FIO_RIS_REG_s;

/*
 * CV1 Flash I/O: Control2, Flash Interface Select Register
 */
typedef struct {
    UINT32  NandFlashMode:      1;      /* [0]:(RO) NAND flash mode tied to 1 */
    UINT32  SelectSpiNand:      1;      /* [1]: 1: spi-NAND, 0: (parallel) NAND */
    UINT32  Reserved:           30;     /* [31:2]: Reserved */
} AMBA_FIO_CTRL2_REG_s;

/*
 * CV1 Flash I/O: Interrupt enable Register
 */
typedef struct {
    UINT32  IntFioEnable:             1;      /* [0]: Interrupt enable of fio_int */
    UINT32  SnandLoopTimeoutEnable:   1;      /* [1]: Interuput enable of Spinand autoStatus timeout  */
    UINT32  AxiBusErrorEnable:        1;      /* [2]: Interrupt enable of DMA bus Status */
    UINT32  Reserved0:                1;      /* [3]: Reserved */
    UINT32  IntErrorDetectedEnable:   1;      /* [4]: Interrupt enable of ecc_rpt_int. */
    UINT32  IntErrorNotCorrectEnable: 1;      /* [5]: Interrupt enable of ecc_rpt_uncorr_int. */
    UINT32  Reserved1:                26;     /* [31:6]: Reserved */
} AMBA_FIO_INT_ENABLE_REG_s;

/*
 * CV1 Flash I/O: Interrupt enable Register
 */
typedef struct {
    UINT32  FioDone:            1;      /* [0]: Interrupt of fio_int */
    UINT32  SnandLoopTimeout:   1;      /* [1]: Spinand timeout while executing auto states update */
    UINT32  AxiBusError:        1;      /* [2]: Interrupt Status of DMA bus */
    UINT32  Reserved0:          1;      /* [3]: Reserved */
    UINT32  ErrorDetected:      1;      /* [4]: Interrupt of ecc_rpt_int. (equivalent to (ecc_rpt_uncorr_raw_int & ecc_rpt_uncorr_int_en)) */
    UINT32  ErrorNotCorrectable: 1;     /* [5]: Interrupt of ecc_rpt_uncorr_int. equivalent to (ecc_rpt_raw_int & ecc_rpt_int_en) */
    UINT32  Reserved1:          26;     /* [31:6]: Reserved */
} AMBA_FIO_INT_STATUS_REG_s;

/*
 * CV1 Flash I/O: DMA Control Register
 */
typedef struct {
    UINT32  ByteCount:          22;     /* [21:0]: DMA Transfer Byte Count */
    UINT32  BusDataSize:        2;      /* [23:22]: Bus Data Transfer size */
    UINT32  BusBlockSize:       4;      /* [27:24]: Bus Transaction Block size */
    UINT32  MemTarget:          2;      /* [29:28]: 0 - NAND, 3 - SD */
    UINT32  ReadMem:            1;      /* [30]: 1 - DMA reads are from main memory */
    UINT32  DmaEnable:          1;      /* [31]: 1 - start DMA operation */
} AMBA_FIO_DMA_CTRL_REG_s;

/*
 *  BusXfrSize[23:22]
 */
#define AMBA_FIO_DMA_BUS_DATA_1BYTE (0U)        /* Bus Transfer Size = 1 Byte */
#define AMBA_FIO_DMA_BUS_DATA_2BYTE (1U)        /* Bus Transfer Size = Halfword (2 Byte) */
#define AMBA_FIO_DMA_BUS_DATA_4BYTE (2U)        /* Bus Transfer Size = 1 Word (4 Byte) */
#define AMBA_FIO_DMA_BUS_DATA_8BYTE (3U)        /* Bus Transfer Size = Double word (8 Byte) */

/*
 *  BurstBlockSize[27:24]
 */
#define AMBA_FIO_DMA_BUS_BLOCK_8BYTE   (0U)
#define AMBA_FIO_DMA_BUS_BLOCK_16BYTE  (1U)
#define AMBA_FIO_DMA_BUS_BLOCK_32BYTE  (2U)
#define AMBA_FIO_DMA_BUS_BLOCK_64BYTE  (3U)
#define AMBA_FIO_DMA_BUS_BLOCK_128BYTE (4U)
#define AMBA_FIO_DMA_BUS_BLOCK_256BYTE (5U)
#define AMBA_FIO_DMA_BUS_BLOCK_512BYTE (6U)
#define AMBA_FIO_DMA_BUS_BLOCK_1KBYTE  (7U)
#define AMBA_FIO_DMA_BUS_BLOCK_2KBYTE  (8U)
#define AMBA_FIO_DMA_BUS_BLOCK_4KBYTE  (9U)
#define AMBA_FIO_DMA_BUS_BLOCK_8KBYTE  (10U)
#define AMBA_FIO_DMA_BUS_BLOCK_16KBYTE (11U)
#define AMBA_FIO_DMA_BUS_BLOCK_32KBYTE (12U)

/*
 *  MemTarget[29:28]
 */
#define AMBA_FIO_DMA_MEM_TARGET_NAND  (0U)
#define AMBA_FIO_DMA_MEM_TARGET_SD    (3U)


/*
 * CV1 Flash I/O: DMA Status Register
 */
typedef struct {
    UINT32  ByteCount:          22;     /* [21:0]: DMA Transfer Byte Count */
    UINT32  Reserved0:          2;      /* [23:22]: Reserved */
    UINT32  DmaDone:            1;      /* [24]: 1 - DMA operation done */
    UINT32  AddrError:          1;      /* [25]: 1 - Address error (not a double word aligned address) */
    UINT32  ReadError:          1;      /* [26]: 1 - NAND read error */
    UINT32  Reserved1:          5;      /* [31:27]: Reserved */
} AMBA_FIO_DMA_STATUS_REG_s;

/*
 * CV1 Flash I/O: Dual-Space-Mode Control Register
 */
typedef struct {
    UINT32  SpareStrideSize:    4;      /* [3:0]: 3 to 9, Transfer 2^SpareTransferSize spare data and then transfer main data */
    UINT32  MainStrideSize:     4;      /* [7:4]: >= 9, Transfer 2^MainTransferSize main data and then transfer spare data */
    UINT32  Reserved:           23;     /* [30:8]: Reserved */
    UINT32  DsmEnable:          1;      /* [31]: 1 = Enable main and spare data interleaved transfer */
} AMBA_FIO_DSM_CTRL_REG_s;


#define AMBA_FIO_DSM_STRIDE_SIZE_1B   (0U)
#define AMBA_FIO_DSM_STRIDE_SIZE_2B   (1U)
#define AMBA_FIO_DSM_STRIDE_SIZE_4B   (2U)
#define AMBA_FIO_DSM_STRIDE_SIZE_8B   (3U)
#define AMBA_FIO_DSM_STRIDE_SIZE_16B  (4U)
#define AMBA_FIO_DSM_STRIDE_SIZE_32B  (5U)
#define AMBA_FIO_DSM_STRIDE_SIZE_64B  (6U)
#define AMBA_FIO_DSM_STRIDE_SIZE_128B (7U)
#define AMBA_FIO_DSM_STRIDE_SIZE_256B (8U)
#define AMBA_FIO_DSM_STRIDE_SIZE_512B (9U)
#define AMBA_FIO_DSM_STRIDE_SIZE_1KB  (10U)
#define AMBA_FIO_DSM_STRIDE_SIZE_2KB  (11U)
#define AMBA_FIO_DSM_STRIDE_SIZE_4KB  (12U)
#define AMBA_FIO_DSM_STRIDE_SIZE_8KB  (13U)
#define AMBA_FIO_DSM_STRIDE_SIZE_16KB (14U)
#define AMBA_FIO_DSM_STRIDE_SIZE_32KB (15U)


/*
 * CV1 Flash I/O: Multi-bits (6-bit, 8-bit BCH CODEC) ECC Report config Register
 */
typedef struct {
    UINT32  Reserved0:            16;     /* [15:0]: Reserved */
    UINT32  ErrorReportBitNumber: 4;      /* [19:16]: The min number of correctable bits in single BCH block not to trigger ecc report int */
    UINT32  Reserved1:            12;     /* [31:20]: Reserved */
} AMBA_FIO_ECC_RPT_CFG_REG_s;

/*
 * CV1 Flash I/O: Multi-bits (6-bit, 8-bit BCH CODEC) ECC Status Register
 */
typedef struct {
    UINT32  EccCorrectedBlockNo:    17;     /* [16:0]:  Block address that contains corrected error data, Valid only when ErrorDetected = 1 */
    UINT32  Reserved0:              3;      /* [19:17]: Reserved */
    UINT32  MaxErrorCorrectNumber:  4;      /* [23:20]: Indicates the maximum correctable errors in a single BCH block */
    UINT32  Reserved1:              5;      /* [28:24]: Reserved */
    UINT32  ErrorNotCorrectableOver: 1;     /* [29]: 1 - Error is not correctable, more than one NAND block. */
    UINT32  ErrorNotCorrectable:    1;      /* [30]: 1 - Error is not correctable, self-cleared when a new DMA begins. */
    UINT32  ErrorDetected:          1;      /* [31]: 1 - ECC error during the last DMA operation detected by BCH decoder */
} AMBA_FIO_ECC_STATUS_REG_s;

/*
 * CV1 Flash I/O: Multi-bits (6-bit, 8-bit BCH CODEC) ECC Status2 Register
 */
typedef struct {
    UINT32  EccErrorBlockNo:      17;     /* [16:0]: Block address that contains Uncorrectable error data,, Valid only when ErrorDetected = 1 */
    UINT32  Reserved:             15;     /* [31:17]: Reserved */
} AMBA_FIO_ECC_STATUS2_REG_s;

/*
 * H2 NAND: Control Register
 */
typedef struct {
    UINT32  Reserved:                   4;      /* [3:0]: Reserved */
    UINT32  ChipSize:                   3;      /* [6:4]: Flash chip size */
    UINT32  Reserved1:                  2;      /* [8:7]: Reserved */
    UINT32  WriteProtectEnable:         1;      /* [9]: 1 - Write protect (can't write any data onto NAND) */
    UINT32  WriteWithAutoStatusUpdate:  1;      /* [10]: 1 - Write with automatic status update */
    UINT32  Reserved2:                  12;     /* [22:11]: Reserved */
    UINT32  ReadId4Cycle:               1;      /* [23]: 1 - Read ID in 4 Cycles, 0 - 2 Cycles */
    UINT32  PageAddr3Cycle:             1;      /* [24]: 1 - Page Address in 3 Cycles */
    UINT32  Reserved3:                  2;      /* [26:25]: Reserved */
    UINT32  SpareAddr:                  1;      /* [27]: 1 - Spare Area Address; 0 - Main Area address */
    UINT32  Addr33_32:                  2;      /* [29:28]: Address[33:32] */
    UINT32  Reserved4:                  2;      /* [31:30]: Reserved */
} AMBA_NAND_CTRL_REG_s;

/*
 * H2 NAND: Command Register
 */
typedef struct {
    UINT32  CmdCode:    4;      /* [3:0]: Command code */
    UINT32  Addr:       28;     /* [31:4]: Address */
} AMBA_NAND_CMD_REG_s;

/*
 * H2 NAND: Command Register: Command[3:0]
 */
#define AMBA_NAND_CMD_NOP0         (0x0U)
#define AMBA_NAND_CMD_DMA          (0x1U)
#define AMBA_NAND_CMD_RESET        (0x2U)           /* Command FFh */
#define AMBA_NAND_CMD_NOP1         (0x3U)
#define AMBA_NAND_CMD_NOP2         (0x4U)
#define AMBA_NAND_CMD_NOP3         (0x5U)
#define AMBA_NAND_CMD_NOP4         (0x6U)
#define AMBA_NAND_CMD_NOP5         (0x7U)             /* Command 00h,35h(Copyback Read) and 85h,10h(Copyback Program) */
#define AMBA_NAND_CMD_NOP6         (0x8U)
#define AMBA_NAND_CMD_NOP7         (0x9U)             /* Command 60h,D0h(D1h for Multi-plane, Erase) */
#define AMBA_NAND_CMD_READ_ID      (0xaU)          /* Command 90h */
#define AMBA_NAND_CMD_NOP8         (0xbU)
#define AMBA_NAND_CMD_READ_STATUS  (0xcU)      /* Command 70h */
#define AMBA_NAND_CMD_NOP9         (0xdU)
#define AMBA_NAND_CMD_READ         (0xeU)            /* Command 00h,30h(32h for Multi-plane) */
#define AMBA_NAND_CMD_PAGE_PROGRAM (0xfU)      /* Command 80h,10h(11h for Multi-plane) */

/*
 * H2 NAND: Timing0 Register
 */
typedef struct {
    UINT32  tDS:        8;      /* [7:0]: tDS (data setup to write pulse rising edge time) */
    UINT32  tCS:        8;      /* [15:8]: tCS (chip enable setup to write pulse rising edge time) */
    UINT32  tALS:       8;      /* [23:16]: tALS (address setup to write pulse rising edge time) */
    UINT32  tCLS:       8;      /* [31:24]: tCLS (command setup to write pulse rising edge time) */
} AMBA_NAND_TIMING0_REG_s;

/*
 * H2 NAND: Timing1 Register
 */
typedef struct {
    UINT32  tDH:        8;      /* [7:0]: tDH (data hold from write pulse rising edge time) */
    UINT32  tCH:        8;      /* [15:8]: tCH (chip enable hold from write pulse rising edge time) */
    UINT32  tALH:       8;      /* [23:16]: tALH (address hold from write pulse rising edge time) */
    UINT32  tCLH:       8;      /* [31:24]: tCLH (command hold from write pulse rising edge time) */
} AMBA_NAND_TIMING1_REG_s;

/*
 * H2 NAND: Timing2 Register
 */
typedef struct {
    UINT32  tRR:        8;      /* [7:0]: tRR (ready setup to read pulse falling edge time) */
    UINT32  tWB:        8;      /* [15:8]: tWB (write pulse rising edge to busy time) */
    UINT32  tWH:        8;      /* [23:16]: tWH (write high hold time) */
    UINT32  tWP:        8;      /* [31:24]: tWP (write pulse width) */
} AMBA_NAND_TIMING2_REG_s;

/*
 * H2 NAND: Timing3 Register
 */
typedef struct {
    UINT32  tCEH:       8;      /* [7:0]: tCEH (chip enable hold from read pulse rising edge time) */
    UINT32  tRB:        8;      /* [15:8]: tRB (read pulse rising edge to busy time) */
    UINT32  tREH:       8;      /* [23:16]: tREH (read high hold time) */
    UINT32  tRP:        8;      /* [31:24]: tRP (read pulse width) */
} AMBA_NAND_TIMING3_REG_s;

/*
 * Definitions & Constants for H2 NAND: Timing4 Register
 */
typedef struct {
    UINT32  tIR:        8;      /* [7:0]: tIR (Flash chip output high Z to read pulse falling edge time) */
    UINT32  tWHR:       8;      /* [15:8]: tWHR (write pulse rising edge to read pulse falling edge time) */
    UINT32  tCLR:       8;      /* [23:16]: tCLR (command setup to read pulse falling edge time) */
    UINT32  tRDELAY:    8;      /* [31:24]: tRDELAY (read pulse falling edge to data valid time) */
} AMBA_NAND_TIMING4_REG_s;

/*
 * H2 NAND: Timing5 Register
 */
typedef struct {
    UINT32  tAR:        8;      /* [7:0]: tAR (address setup to read pulse falling edge time) */
    UINT32  tRHZ:       8;      /* [15:8]: tRHZ (read pulse rising edge to Flash chip output high Z time) */
    UINT32  tWW:        8;      /* [23:16]: tWW (ready to write pulse falling edge time) */
    UINT32  Reserved:   8;      /* [31:24]: Reserved */
} AMBA_NAND_TIMING5_REG_s;

/*
 * H2 NAND: Status Register
 */
typedef struct {
    UINT32  Status:     8;      /* [7:0]: Status0 */
    UINT32  Reserved:   24;     /* [31:8]: Status3 */
} AMBA_NAND_STATUS_REG_s;

/*
 * H2 NAND: ID Register
 */
typedef struct {
    UINT32  IdByte4:    8;      /* [7:0]: ID4 = Page Size, Block Size, Redundant Area Size, Organization, Serial Access Minimum */
    UINT32  IdByte3:    8;      /* [15:8]: ID3 = Internal Chip Number, Cell Type, Number of Simultaneously Programmed Pages, Etc */
    UINT32  DeviceID:   8;      /* [23:16]: ID2 = Device Code (Flash chip device code) */
    UINT32  MakerID:    8;      /* [31:24]: ID1 = Maker Code (Flash chip manufacturer code) */
} AMBA_NAND_ID_REG_s;

/*
 * H2 NAND: Debug 0 Register
 */
typedef struct {
    UINT32  Status:             4;      /* [3:0]: Indicates the state of parallel/spi NAND controller */
    UINT32  LegacyCmd:          4;      /* [7:4]: Current legacy NAND command */
    UINT32  RemainedDataLength: 22;     /* [29:8]: Left data bytes not issued to NAND interface controller yet. */
    UINT32  WdataActive:        1;      /* [30]: Indicates the state of parallel/spi NAND controller */
    UINT32  WaveformRequest:    1;      /* [31]: Indicates if nand controller is requesting to generate NAND */
} AMBA_NAND_DEBUG0_REG_s;

/*
 * H2 NAND: Debug 1 Register
 */
typedef struct {
    UINT32  RemainedFifoDataLength: 22;  /* [21:0]:  1 - Left data bytes not received from/transmitted to FIFO controller yet. */
    UINT32  DmaDataFlow:            2;   /* [23:22]: 0: idle.  1: transferring main data. 2: trasnferring spare data. */
    UINT32  LastExecuteCmd:         8;   /* [31:24]: To know the flash controller is running at which command */
} AMBA_NAND_DEBUG1_REG_s;

/*
 * H2 NAND: Read Command Word Register
 */
typedef struct {
    UINT32  CmdWord2:   8;      /* [7:0]: Second command word of read operation */
    UINT32  Reserved0:  8;      /* [15:8]: Reserved */
    UINT32  CmdWord1:   8;      /* [23:16]: First command word of read operation */
    UINT32  Reserved1:  8;      /* [31:24]: Reserved */
} AMBA_NAND_READ_CMD_WORD_REG_s;

/*
 * Definitions & Constants for H2 NAND: Program Command Word Register
 */
typedef struct {
    UINT32  CmdWord2:   8;      /* [7:0]: Second command word of program operation */
    UINT32  Reserved0:  8;      /* [15:8]: : Reserved */
    UINT32  CmdWord1:   8;      /* [23:16]: First command word of program operation */
    UINT32  Reserved1:  8;      /* [31:24]: Reserved */
} AMBA_NAND_PROG_CMD_WORD_REG_s;

/*
 * H2 NAND: Extended Control Register
 */
typedef struct {
    UINT32  Spare2xEnable:  1;  /* [0]: 1 - 2x spare area enable */
    UINT32  Reserved0:      22; /* [22:1]: Reserved */
    UINT32  Id5Bytes:       1;  /* [23]: 1 - ID register read in five cycles */
    UINT32  ChipSizeExt:    1;  /* [24]: */
    UINT32  Page4kEnable:   1;  /* [25]: 1 - Page size is 4K-byte. 0 - Page size is 2K-byte. */
    UINT32  Reserved1:      6;  /* [31:26]: Reserved */
} AMBA_NAND_EXT_CTRL_REG_s;

/*
 * H2 NAND: Extended ID Register
 */
typedef struct {
    UINT32  IdByte5:    8;      /* [7:0]: ID5 = Plane Number, Plane Size */
    UINT32  Reserved:   24;     /* [31:8]: Reserved */
} AMBA_NAND_EXT_ID_REG_s;

/*
 * H2 NAND: Timing6 Register
 */
typedef struct {
    UINT32  tCRL:       8;      /* [7:0]: Chip Enable Falling to Read Plus Falling Edge Time */
    UINT32  tADL:       8;      /* [15:8]: Write Pulse Rising Edge of the last address cycle to the write plus rising edge of the first data cycle */
    UINT32  tRHW:       8;      /* [23:16]: Read Pulse Rising Edge to the Write Puls Falling Edge */
    UINT32  Reserved:   8;      /* [31:24]: Reserved */
} AMBA_NAND_TIMING6_REG_s;

/*
 * H2 NAND: Customer Command Register
 */
typedef struct {
    UINT32  DataCycle:              5;  /* [4:0]: Number of RE/WE Puls in non-DMA mode. Valid only if the value of 'Type' is not Zero. */
    UINT32  WaitCycle:              2;  /* [6:5]: 0 = No Wait Cycles, 1 = Wait For R/B, 2 = Wait For tWHR */
    UINT32  ReadWriteType:          2;  /* [8:7]: 0 = No Data Phase, 1 = WE type , 2 = RE type */
    UINT32  CmdPhase2Count:         2;  /* [10:9]: 0 = No Cmd2Phase , 1 = CusCmd2Value0 Would be Present as WE type after Data Phase ,RE after Address Phase */
    UINT32  AdressCycle:            3;  /* [13:11]: 0 = No Adress Phase , 1~4 = Number of Address Cycles during Address Phase */
    UINT32  CmdPhase1Count:         2;  /* [15:14]: 0 = No Cmd Before Adress Phase , 1 = CusCmd1Valu0 would be Present in Command Phase1 ,2 CusCmd1Value1 would be present */
    UINT32  AddrSrc:                2;  /* [17:16]: */
    UINT32  Addr1NoIncrease:        1;  /* [18]: 0: increment address for multi-page access. 1: don't increment address for multi-page access.*/
    UINT32  Addr2NoIncrease:        1;  /* [19]: The definition of this bit is the same as cc_addr1_ni but is applied to the 2nd address phase of two-plane access. */
    UINT32  Reserved0:              2;  /* [21:20]: Reserved */
    UINT32  DataSrcSelect:          1;  /* [22]: 0: source of the data in data phase is from/to nd_cc_dat* registers. 1: source of the data in data phase is from/to DMA engine. */
    UINT32  Reserved1:              8;  /* [30:23]: Reserved */
    UINT32  ChipEnableTerminate:    1;  /* [31]: 0 = CE ping Keep Low, 1 =  CE ping Keep High After Customer Cmd Complete */
} AMBA_NAND_CUSTOM_CMD_REG_s;

/*
 * H2 NAND: Custom Command Register
 */
#define AMBA_NAND_CMD_WAIT_NONE (0x0U)
#define AMBA_NAND_CMD_WAIT_RB   (0x1U)
#define AMBA_NAND_CMD_WAIT_TWHR (0x2U)

#define AMBA_NAND_CMD_NO_DATA (0x0U)
#define AMBA_NAND_CMD_WE_TYPE (0x1U)
#define AMBA_NAND_CMD_RE_TYPE (0x2U)

#define AMBA_NAND_CMD_NO_PHASE2            (0x0U)
#define AMBA_NAND_CMD_PHASE2_PRESENT       (0x1U)
#define AMBA_NAND_CMD_PHASE2_FOLLOW_PHASE1 (0x2U)

#define AMBA_NAND_CMD_NO_PHASE1                (0x0U)
#define AMBA_NAND_CMD_PHASE1_PRESENT_CMD1      (0x1U)
#define AMBA_NAND_CMD_PHASE2_PRESENT_CMD1_CMD2 (0x2U)

#define AMBA_NAND_DATA_FROM_REGISTER   (0x0U)
#define AMBA_NAND_DATA_FROM_DMA_ENGINE (0x1U)

/*
 * H2 NAND: Timing6 Register
 */
typedef struct {
    UINT32  Cmd1Val0:       8;      /* [7:0]: The First Command Byte in Command Phase-1 of Customer Command */
    UINT32  Cmd1Val1:       8;      /* [15:8]: The Second Command Byte in Command Phase-1 of Customer Command */
    UINT32  Cmd2Val0:       8;      /* [23:16]: The First Command Byte in Command Phase-2 of Customer Command */
    UINT32  Cmd2Val1:       8;      /* [31:24]: The Second Command Byte in Command Phase-2 of Customer Command */
} AMBA_NAND_CUSTOM_CMD_WORD_REG_s;

/*
 * H2 NAND: Extended ID Register
 */
typedef struct {
    UINT32  CopyAddrHigh:  8;      /* [7:0]: The 5th byte of nand CP address. */
    UINT32  Reserved:      24;     /* [31:8]: Reserved */
} AMBA_NAND_COPY_ADDR_HIGH_REG_s;

typedef struct {
    UINT32  MaxCmdLoop:                 18;     /* [17:0]: 0 = infinite loop count , 0x1 ~ 0x3ffff */
    UINT32  Reserved:                   10;     /* [27:18]: Reserved */
    UINT32  PlaneSelect:                2;      /* [29:28]: Plane select */
    UINT32  FlowCtrl:                   1;      /* [30]: Stop Clock to halt data transmission */
    UINT32  SpiClockMode:               1;      /* [31]: 0 = SCK remains at 0, 1 = SCK remains at 1 */
} AMBA_SPINAND_CTRL_REG_s;


#define AMBA_SPINAND_DATA_1LANE 0x0U
#define AMBA_SPINAND_DATA_2LANE 0x1U
#define AMBA_SPINAND_DATA_4LANE 0x2U

#define AMBA_SPINAND_DATAPHASE_ONLY             0x0U
#define AMBA_SPINAND_DUMYDATAPHAS_DATAPHASE     0x1U
#define AMBA_SPINAND_DUMMY_ADDRESS_DATA_PHASE   0x2U
#define AMBA_SPINAND_ALL_PHASE                  0x3U

#define AMBA_SPINAND_NO_DATAPHASE_TYPE 0x0U
#define AMBA_SPINAND_WE_TYPE           0x1U
#define AMBA_SPINAND_RE_TYPE           0x2U


#define AMBA_SPINAND_ADRRESS_CUSTOMCMD 0x0U
#define AMBA_SPINAND_ADRRESS_ROW       0x1U
#define AMBA_SPINAND_ADRESS_ROW_COLUMN 0x2U
#define AMBA_SPINAND_ADRESS_TWO_PLANE  0x3U

typedef struct {
    UINT32  DataCycle:                  5;      /* [4:0]: Number of plus in non-DMA mode */
    UINT32  LaneNum:                    2;      /* [6:5]: 0 = data lane, 1 = data lane, 4 = data lane */
    UINT32  CustCmdType:                2;      /* [8:7]: 0 = no data phase, 1 = WE type, 2 = RE type */
    UINT32  AddrSrc:                    2;      /* [10:9]: 0 = data phase is from/to nand_cc_dat, 1 = is from/to DMA engine */
    UINT32  AddrCycle:                  3;      /* [13:11]: 0x1 ~ 0x5 : number of address cycle */
    UINT32  LaneType:                   2;      /* [15:14]: 0 = data phase only, 1 = dummy-data phase and data phase,2 = all*/
    UINT32  DataPhaseDummy:             3;      /* [18:16]: Dummy byte length in dummy-Data phase */
    UINT32  AddrPhaseDummy:             3;      /* [21:19]: Dummy byte length in dummy-addr phase */
    UINT32  DataPhaseSrc:               1;      /* [22]: 0 = data phase is from/to nand_cc_dat, 1 = is from/to DMA engine */
    UINT32  Reserved:                   7;      /* [29:23]: Reserved */
    UINT32  AutoReadStatus:             1;      /* [30]:  1 = Execute read status cmd recursively */
    UINT32  AutoWriteEnable:            1;      /* [31]:  1 = Execute write enable cmd first */
} AMBA_SPINAND_CUSTOM_CMD1_REG_s;

typedef struct {
    UINT32  DataCycle:                  5;      /* [4:0]: Number of plus in non-DMA mode */
    UINT32  LaneNum:                    2;      /* [6:5]: 0 = data lane, 1 = data lane, 4 = data lane */
    UINT32  CustCmdType:                2;      /* [8:7]: 0 = no data phase, 1 = WE type, 2 = RE type */
    UINT32  AddrSrc:                    2;      /* [10:9]: 0 = data phase is from/to nand_cc_dat, 1 = is from/to DMA engine */
    UINT32  AddrCycle:                  3;      /* [13:11]: 0x1 ~ 0x5 : number of address cycle */
    UINT32  LaneType:                   2;      /* [15:14]: 0 = data phase only, 1 = dummy-data phase and data phase,2 = all*/
    UINT32  DataPhaseDummy:             3;      /* [18:16]: Dummy byte length in dummy-Data phase */
    UINT32  AddrPhaseDummy:             3;      /* [21:19]: Dummy byte length in dummy-addr phase */
    UINT32  DataPhaseSrc:               1;      /* [22]: 0 = data phase is from/to nand_cc_dat, 1 = is from/to DMA engine */
    UINT32  Reserved2:                  3;      /* [25:23]: Reserved */
    UINT32  CusCmd2Enable:              1;      /* [26]: Indicates CustomCmd2 is valid */
    UINT32  Reserved1:                  3;      /* [29:27]: Reserved */
    UINT32  AutoReadStatus:             1;      /* [30]:  1 = Execute read status cmd recursively */
    UINT32  Reserved:                   1;      /* [31]:  1 = Execute write enable cmd first */
} AMBA_SPINAND_CUSTOM_CMD2_REG_s;

typedef struct {
    UINT32  DonePattern:                  8;     /* [7:0]: Done Pattern */
    UINT32  Reserved1:                    8;     /* [15:8]: Reserved */
    UINT32  DonePatterEn:                 8;     /* [23:16]: Done pattern Enable  */
    UINT32  Reserved:                     8;     /* [31:24]: Reserved */
} AMBA_SPINAND_DONEPATTERN_REG_s;

typedef struct {
    UINT32  ErrorPattern:                  8;     /* [7:0]: Error Pattern 0x2C */
    UINT32  Reserved1:                     8;     /* [15:8]: Reserved */
    UINT32  ErrorPatterEn:                 8;     /* [23:16]: Error pattern Enable  */
    UINT32  Reserved:                      8;     /* [31:24]: Reserved */
} AMBA_SPINAND_ERRORPATTERN_REG_s;

typedef struct {
    UINT32  CurCmdLoopCnt:                 18;    /* [17:0]: Indicates the current counter of auto stschk */
    UINT32  Reserved1:                     2;     /* [19:18]: Reserved */
    UINT32  CurStatus:                     3;     /* [22:20]: Current state of Waveform generator */
    UINT32  Reserved:                      9;     /* [31:23]: Reserved */
} AMBA_SPINAND_STATUS_REG_s;

/*
 * H2 SPINAND: Timing0 Register
 */
typedef struct {
    UINT32  tCLQV:      8;      /* [7:0]: tCLQV (Clock low to read data valid time) */
    UINT32  tCS:        8;      /* [15:8]: tCS (Command deselect time ) */
    UINT32  tCLL:       8;      /* [23:16]: tCLL (clock low time) */
    UINT32  tCLH:       8;      /* [31:24]: tCLH (clock High time) */
} AMBA_SPINAND_TIMING0_REG_s;

/*
 * H2 SPINAND: Timing1 Register
 */
typedef struct {
    UINT32  tSHC:       8;      /* [7:0]: tSHC (from rising edge of CS to rising edge of SCK) */
    UINT32  tCHS:       8;      /* [15:8]: tCHS (from rising edge of SCK to rising edge of CS#) */
    UINT32  tSLCH:      8;      /* [23:16]: tSLCH (from rising edge of CS to falling edge of SCK) */
    UINT32  tCHSL:      8;      /* [31:24]: tCHSL (from rising edge of SCK to falling edge of CS# ) */
} AMBA_SPINAND_TIMING1_REG_s;

/*
 * H2 SPINAND: Timing2 Register
 */
typedef struct {
    UINT32  tWPH:        8;      /* [7:0]: tWPH (from Rising Edge of CS to falling edge of WP) */
    UINT32  tWPS:        8;      /* [15:8]: tWPS (from riging edge of WP to falling edge of CS) */
    UINT32  tHHQX:       8;      /* [23:16]: tHHQX (from rising edge of Hold to read Data valid) */
    UINT32  Reserved:    8;      /* [31:24]: */
} AMBA_SPINAND_TIMING2_REG_s;


typedef struct {
    UINT32  ByteCount:          22;     /* [21:0]: DMA Transfer Byte Count */
    UINT32  Reserved:           2;      /* [23:22]: Reserved */
    UINT32  BusBlockSize:       3;      /* [26:24]: Bus Transaction Block size */
    UINT32  Reserved1:          2;      /* [28:27]: Reserved */
    UINT32  WriteMem:           1;      /* [29]: 1 - Write to Memory; 0 - Write to I/O  */
    UINT32  DescMode:           1;      /* [30]: 1 - Descriptor Mode */
    UINT32  Enable:             1;      /* [31]: 1 - DMA Channel Enable */
} AMBA_FDMA_CTRL_REG_s;

typedef struct {
    UINT32  Count:              22;     /* [21:0]: Transfer byte count */
    UINT32  DmaDone:            1;      /* [22]: Operation done */
    UINT32  AddrError:          1;      /* [23]: Address error */
    UINT32  Reserved:           1;      /* [24]: Reserved */
    UINT32  BusError:           1;      /* [25]: Bus error */
    UINT32  MemError:           1;      /* [26]: Memory error */
    UINT32  DescDmaDone:        1;      /* [27]: Descriptor DMA operation done */
    UINT32  DescChainDone:      1;      /* [28]: Descriptor chain done */
    UINT32  DescAddrError:      1;      /* [29]: Descriptor address error */
    UINT32  DescDmaError:       1;      /* [30]: Descriptor DMA operation error */
    UINT32  DescMemError:       1;      /* [31]: Descriptor memory error */
} AMBA_FDMA_MAIN_STATUS_REG_s;

/*
 *  CV1 FDMA:
 */
typedef struct {
    UINT32  ByteCountLeft:      22;     /* [21:0]: Total data byte count not requested to DRAM yet. */
    UINT32  DataFromSpare:      1;      /* [22]: Current data from/to DRAM is for spare-area */
    UINT32  CmdAddrFromSpare:   1;      /* [23]: Current address phase from/to DRAM is for spare-area */
    UINT32  MemDataStatus:      2;      /* [25:24]: current DRAM data access state; 0: idle, 1: waiting the 1st data from/to DRAM, 2: waiting the last data from/to DRAM */
    UINT32  MemCmdStatus:       3;      /* [28:26]: current DRAM address access state */
    UINT32  Reserved:           3;      /* [31:29]: Reserved */
} AMBA_FDMA_BUS_DBG0_REG_s;

/*
 *  CV1 FDMA:
 */
typedef struct {
    UINT32  DsmByteCountLeft:     16;     /* [15:0]: DSM data byte count not requested to DRAM yet */
    UINT32  BufferByteCount:      4;      /* [19:16]: Data byte count in data-alignment buffer */
    UINT32  BufferAddrShift:      3;      /* [22:20]: address shift amount in data-alignment buffer */
    UINT32  BufferAddrShiftValid: 1;      /* [23]: valid signal of dbuf_mem_addr_shift. */
    UINT32  FifoStatus:           2;      /* [25:24]: current data state accessing FIFO. */
    UINT32  FifoFromSpare:        1;      /* [26]: Current data from/to DRAM is for spare-area */
    UINT32  Reserved:             1;      /* [27]: Reserved */
    UINT32  DescropterStatus:     3;      /* [30:28]: current state of descriptor manager */
    UINT32  Reserved1:            1;      /* [31]: Reserved */
} AMBA_FDMA_BUS_DBG1_REG_s;

/*
 *  CV1 FDMA:
 */
typedef struct {
    UINT32  XactLeftByteCnt:       11;     /* [10:0]: transaction data byte count not transferred from/to DRAM yet. */
    UINT32  MemDataLeftByteCnt:    21;     /* [31:11]: total data byte count not transferred from/to DRAM yet. (AXI R/W channel) */
} AMBA_FDMA_BUS_DBG2_REG_s;

typedef struct {
    UINT32  SpareStrideSize:    4;      /* [3:0]: 3 to 9, Transfer 2^SpareTransferSize spare data and then transfer main data */
    UINT32  MainStrideSize:     4;      /* [7:4]: >= 9, Transfer 2^MainTransferSize main data and then transfer spare data */
    UINT32  Reserved:           24;     /* [31:8]: Reserved */
} AMBA_FDMA_DSM_CTRL_REG_s;

#define AMBA_FDMA_DSM_STRIDE_SIZE_1B   (0U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_2B   (1U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_4B   (2U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_8B   (3U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_16B  (4U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_32B  (5U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_64B  (6U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_128B (7U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_256B (8U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_512B (9U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_1KB  (10U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_2KB  (11U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_4KB  (12U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_8KB  (13U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_16KB (14U)
#define AMBA_FDMA_DSM_STRIDE_SIZE_32KB (15U)

/*
 * CV1 Flash I/O: All Registers
 */
typedef struct {
    volatile AMBA_FIO_CTRL_REG_s             Ctrl;           /* 0x000(RW): Flash I/O control (global control info) */
    volatile AMBA_FIO_RIS_REG_s              IntStatusRaw;   /* 0x004(RW1C): Flash I/O Raw INT status */
    volatile AMBA_FIO_CTRL2_REG_s            Ctrl2;          /* 0x008(RW): Flash I/O Interface select */
    volatile AMBA_FIO_INT_ENABLE_REG_s       IntEnable;      /* 0x00C(RW): Flash I/O Interrupt enable */
    volatile AMBA_FIO_INT_STATUS_REG_s       Status;         /* 0x010(RW): Flash I/O Interrupt status */
    volatile UINT32                          Reserved0[35];  /* 0x014-0x09C: Reserved  */
    volatile AMBA_FIO_ECC_RPT_CFG_REG_s      EccReportConfig;/* 0x0A0(RW): Flash 6/8-bit ECC Report config Register */
    volatile AMBA_FIO_ECC_STATUS_REG_s       EccStatus;      /* 0x0A4(RO): Flash 6/8-bit ECC Error Detected Status */
    volatile AMBA_FIO_ECC_STATUS2_REG_s      EccStatus2;     /* 0x0A8(RO): Flash 6/8-bit ECC Error Detected Status2 */

    /* NAND control */
    volatile UINT32                          Reserved1[29];  /* 0xAC~0x11C: Reserved */
    volatile AMBA_NAND_CTRL_REG_s            NandCtrl;       /* 0x120(RW): Flash controller control information */
    volatile AMBA_NAND_CMD_REG_s             Cmd;            /* 0x124(RW): Flash controller command and associated address */
    volatile AMBA_NAND_TIMING0_REG_s         Timing0;        /* 0x128(RW): NAND flash chip timing0 parameters */
    volatile AMBA_NAND_TIMING1_REG_s         Timing1;        /* 0x12C(RW): NAND flash chip timing1 parameters */
    volatile AMBA_NAND_TIMING2_REG_s         Timing2;        /* 0x130(RW): NAND flash chip timing2 parameters */
    volatile AMBA_NAND_TIMING3_REG_s         Timing3;        /* 0x134(RW): NAND flash chip timing3 parameters */
    volatile AMBA_NAND_TIMING4_REG_s         Timing4;        /* 0x138(RW): NAND flash chip timing4 parameters */
    volatile AMBA_NAND_TIMING5_REG_s         Timing5;        /* 0x13C(RW): NAND flash chip timing5 parameters */
    volatile AMBA_NAND_STATUS_REG_s          NandStatus;     /* 0x140(RO): NAND flash chip status */
    volatile AMBA_NAND_ID_REG_s              ID;             /* 0x144(RO): NAND flash device ID information */
    volatile UINT32                          CopyDestAddr;   /* 0x148(RW): Start address where the Copy Back command data is to be copied */
    volatile AMBA_NAND_DEBUG0_REG_s          Debug0;         /* 0x14C(RO): NAND flash Debug register 0 */
    volatile AMBA_NAND_DEBUG1_REG_s          Debug1;         /* 0x150(RO): NAND flash command done interrupt */
    volatile UINT32                          Reserved2[2];   /* 0x154~0x158 : Reserved */
    volatile AMBA_NAND_EXT_CTRL_REG_s        ExtCtrl;        /* 0x15C(RW): NAND flash controller extended control information */
    volatile AMBA_NAND_EXT_ID_REG_s          ExtID;          /* 0x160(RW): NAND flash device extended ID information */
    volatile AMBA_NAND_TIMING6_REG_s         Timing6;        /* 0x164(RW): NAND flash chip timing6 parameters */
    volatile UINT32                          Reserved3[2];   /* 0x168~0x15c : Reserved */
    volatile AMBA_NAND_CUSTOM_CMD_REG_s      CustomCmd;      /* 0x170(RW): NAND flash chip customer command */
    volatile AMBA_NAND_CUSTOM_CMD_WORD_REG_s CusCmdWord;     /* 0x174(RW): NAND flash chip customer command word */
    volatile UINT32                          Reserved4;      /* 0x178 : Reserved */
    volatile AMBA_NAND_COPY_ADDR_HIGH_REG_s  CopyAddrHigh;   /* 0x17C(RW): 5th Byte of NAND flash copy address. */
    volatile UINT32                          CusCmdData[8];  /* 0x180~0x19C(RW): NAND flash chip customer command Data */
    volatile AMBA_SPINAND_CTRL_REG_s         SCtrl;          /* 0x1a0(RW): SPINAND Flash controller control information */
    volatile AMBA_SPINAND_CUSTOM_CMD1_REG_s  SpiCusCmd1;     /* 0x1a4(RW): SPINAND flash chip customer command */
    volatile AMBA_SPINAND_CUSTOM_CMD2_REG_s  SpiCusCmd2;     /* 0x1a8(RW): SPINAND flash chip customer command */
    volatile AMBA_SPINAND_DONEPATTERN_REG_s  EnableDonePatn; /* 0x1ac(RW): SPINAND flash chip command done pattern */
    volatile AMBA_SPINAND_ERRORPATTERN_REG_s ErrorPatn;      /* 0x1b0(RW): SPINAND flash chip command error pattern */
    volatile UINT32                          Reserved5;      /* 0x1b4 : Reserved */
    volatile AMBA_SPINAND_STATUS_REG_s       SpiNandStatus;  /* 0x1b8(RO): SPINAND flash chip command error pattern */
    volatile UINT32                          Reserved6;      /* 0x1bc : Reserved */
    volatile AMBA_SPINAND_TIMING0_REG_s      STiming0;       /* 0x1c0(RW): SPINAND flash chip timing0 parameters */
    volatile AMBA_SPINAND_TIMING1_REG_s      STiming1;       /* 0x1c4(RW): SPINAND flash chip timing1 parameters */
    volatile AMBA_SPINAND_TIMING2_REG_s      STiming2;       /* 0x1c8(RW): SPINAND flash chip timing2 parameters */

    volatile UINT32                          Reserved7[14];  /* 0x1CC~0x200 */
    volatile UINT32                          SpareMemAddr;   /* 0x204(RW): Channel 0 Spare Area */
    volatile UINT32                          Reserved8[62];  /* 0x208-0x2FC */
    volatile AMBA_FDMA_CTRL_REG_s            FdmaCtrl;       /* 0x300: control */
    volatile UINT32                          DestAddr;       /* 0x304: Main Memory address ofthe DMA request */
    volatile UINT32                          Reserved9;      /* 0x308: Reserved */
    volatile AMBA_FDMA_MAIN_STATUS_REG_s     FdmaMainStatus; /* 0x30c: Status */
    volatile UINT32                          Reserved10[8];  /* 0x310-0x32c */
    volatile AMBA_FDMA_BUS_DBG0_REG_s        BusStatus0;     /* 0x330(R) */
    volatile AMBA_FDMA_BUS_DBG1_REG_s        BusStatus1;     /* 0x334(R) */
    volatile AMBA_FDMA_BUS_DBG2_REG_s        BusStatus2;     /* 0x338(R) */
    volatile UINT32                          Reserved11;     /* 0x33C */
    volatile UINT32                          DescWord[10];   /* 0x340-0x364(RW): The current content of word#0 the descriptor buffer */
    volatile UINT32                          Reserved12[6];  /* 0x368-0x37c */
    volatile UINT32                          FdmaDescAddr;   /* 0x380: Byte address of the next descriptor that will be used to configure DMA Channel if it is in descriptor mode. Must be eight byte aligned. */
    volatile UINT32                          Reserved13[7];  /* 0x384-0x39c */
    volatile AMBA_FDMA_DSM_CTRL_REG_s        Chan0DsmCtrl;   /* 0x3A0(RW): Channel 0 Dual Space Mode Control */
} AMBA_FIO_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_FIO_REG_s *pAmbaFIO_Reg;

#endif /* AMBA_REG_FIO_H */
