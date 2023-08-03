/**
 *  @file AmbaReg_NAND.h
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
 *  @details Definitions & Constants for H2 NAND: Controller Registers
 *
 */

#ifndef AMBA_REG_NAND_H
#define AMBA_REG_NAND_H

/*
 * H2 NAND: Control Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  DataBusWidth:               2;      /* [1:0]: data bus width of the flash controller */
        UINT32  NumBank:                    2;      /* [3:2]: the number of flash banks in the system */
        UINT32  ChipSize:                   3;      /* [6:4]: Flash chip size */
        UINT32  DataBus16Bit:               1;      /* [7]: 0 = 8-bit, 1 = 16-bit data bus */
        UINT32  IrqEnable:                  1;      /* [8]: 1 - Interrupt enabled (flash command done interrupt occurs) */
        UINT32  WriteProtectEnable:         1;      /* [9]: 1 - Write protect (can't write any data onto NAND) */
        UINT32  WriteWithAutoStatusUpdate:  1;      /* [10]: 1 - Write with automatic status update */
        UINT32  Reserved0:                  5;      /* [15:11]: Reserved */
        UINT32  EccGenSpareEnable:          1;      /* [16]: 1 - ECC generation for Program enabled (for spare area) */
        UINT32  EccGenMainEnable:           1;      /* [17]: 1 - ECC generation for Program enabled (for main area)*/
        UINT32  EccCheckSpareEnable:        1;      /* [18]: 1 - ECC checking on reads enabled (for spare area) */
        UINT32  EccCheckMainEnable:         1;      /* [19]: 1 - ECC checking on reads enabled (for main area) */
        UINT32  CopyBackCmdEnable:          1;      /* [20]: 1 - Copy Back Command enabled */
        UINT32  CopyConfirm:                1;      /* [21]: 1 - Require a copy confirm command */
        UINT32  ReadConfirm:                1;      /* [22]: 1 - Require a read confirm command */
        UINT32  ReadId4Cycle:               1;      /* [23]: 1 - Read ID in 4 Cycles, 0 - 2 Cycles */
        UINT32  PageAddr3Cycle:             1;      /* [24]: 1 - Page Address in 3 Cycles */
        UINT32  ColumnAddr2Cycle:           1;      /* [25]: 1 - Column Address in 2 Cycles for 2KB/page, 0 - for 512B/page */
        UINT32  SpareEnable:                1;      /* [26]: 1 - Spare Area Enable */
        UINT32  SpareAddr:                  1;      /* [27]: 1 - Spare Area Address; 0 - Main Area address */
        UINT32  Addr33_32:                  2;      /* [29:28]: Address[33:32] */
        UINT32  Reserved1:                  2;      /* [31:30]: Reserved */
    } Bits;
} AMBA_NAND_CTRL_REG_u;

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
typedef enum {
    AMBA_NAND_CMD_NOP0 = 0x0,
    AMBA_NAND_CMD_DMA,
    AMBA_NAND_CMD_RESET,            /* Command FFh */
    AMBA_NAND_CMD_NOP1,
    AMBA_NAND_CMD_NOP2,
    AMBA_NAND_CMD_NOP3,
    AMBA_NAND_CMD_NOP4,
    AMBA_NAND_CMD_COPYBACK,         /* Command 00h,35h(Copyback Read) and 85h,10h(Copyback Program) */
    AMBA_NAND_CMD_NOP5,
    AMBA_NAND_CMD_BLOCK_ERASE,      /* Command 60h,D0h(D1h for Multi-plane) */
    AMBA_NAND_CMD_READ_ID,          /* Command 90h */
    AMBA_NAND_CMD_NOP6,
    AMBA_NAND_CMD_READ_STATUS,      /* Command 70h */
    AMBA_NAND_CMD_NOP7,
    AMBA_NAND_CMD_READ,             /* Command 00h,30h(32h for Multi-plane) */
    AMBA_NAND_CMD_PAGE_PROGRAM      /* Command 80h,10h(11h for Multi-plane) */
} AMBA_NAND_CMD_e;

/*
 * H2 NAND: Timing0 Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  tDS:        8;      /* [7:0]: tDS (data setup to write pulse rising edge time) */
        UINT32  tCS:        8;      /* [15:8]: tCS (chip enable setup to write pulse rising edge time) */
        UINT32  tALS:       8;      /* [23:16]: tALS (address setup to write pulse rising edge time) */
        UINT32  tCLS:       8;      /* [31:24]: tCLS (command setup to write pulse rising edge time) */
    } Bits;
} AMBA_NAND_TIMING0_REG_u;

/*
 * H2 NAND: Timing1 Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  tDH:        8;      /* [7:0]: tDH (data hold from write pulse rising edge time) */
        UINT32  tCH:        8;      /* [15:8]: tCH (chip enable hold from write pulse rising edge time) */
        UINT32  tALH:       8;      /* [23:16]: tALH (address hold from write pulse rising edge time) */
        UINT32  tCLH:       8;      /* [31:24]: tCLH (command hold from write pulse rising edge time) */
    } Bits;
} AMBA_NAND_TIMING1_REG_u;

/*
 * H2 NAND: Timing2 Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  tRR:        8;      /* [7:0]: tRR (ready setup to read pulse falling edge time) */
        UINT32  tWB:        8;      /* [15:8]: tWB (write pulse rising edge to busy time) */
        UINT32  tWH:        8;      /* [23:16]: tWH (write high hold time) */
        UINT32  tWP:        8;      /* [31:24]: tWP (write pulse width) */
    } Bits;
} AMBA_NAND_TIMING2_REG_u;

/*
 * H2 NAND: Timing3 Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  tCEH:       8;      /* [7:0]: tCEH (chip enable hold from read pulse rising edge time) */
        UINT32  tRB:        8;      /* [15:8]: tRB (read pulse rising edge to busy time) */
        UINT32  tREH:       8;      /* [23:16]: tREH (read high hold time) */
        UINT32  tRP:        8;      /* [31:24]: tRP (read pulse width) */
    } Bits;
} AMBA_NAND_TIMING3_REG_u;

/*
 * Definitions & Constants for H2 NAND: Timing4 Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  tIR:        8;      /* [7:0]: tIR (Flash chip output high Z to read pulse falling edge time) */
        UINT32  tWHR:       8;      /* [15:8]: tWHR (write pulse rising edge to read pulse falling edge time) */
        UINT32  tCLR:       8;      /* [23:16]: tCLR (command setup to read pulse falling edge time) */
        UINT32  tRDELAY:    8;      /* [31:24]: tRDELAY (read pulse falling edge to data valid time) */
    } Bits;
} AMBA_NAND_TIMING4_REG_u;

/*
 * H2 NAND: Timing5 Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  tAR:        8;      /* [7:0]: tAR (address setup to read pulse falling edge time) */
        UINT32  tRHZ:       8;      /* [15:8]: tRHZ (read pulse rising edge to Flash chip output high Z time) */
        UINT32  tWW:        8;      /* [23:16]: tWW (ready to write pulse falling edge time) */
        UINT32  Reserved:   8;      /* [31:24]: Reserved */
    } Bits;
} AMBA_NAND_TIMING5_REG_u;

/*
 * H2 NAND: Status Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Status0:    8;      /* [7:0]: Status0 */
        UINT32  Status1:    8;      /* [15:8]: Status1 */
        UINT32  Status2:    8;      /* [23:16]: Status2 */
        UINT32  Status3:    8;      /* [31:24]: Status3 */
    } Bits;
} AMBA_NAND_STATUS_REG_u;

/*
 * H2 NAND: ID Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  IdByte4:    8;      /* [7:0]: ID4 = Page Size, Block Size, Redundant Area Size, Organization, Serial Access Minimum */
        UINT32  IdByte3:    8;      /* [15:8]: ID3 = Internal Chip Number, Cell Type, Number of Simultaneously Programmed Pages, Etc */
        UINT32  DeviceID:   8;      /* [23:16]: ID2 = Device Code (Flash chip device code) */
        UINT32  MakerID:    8;      /* [31:24]: ID1 = Maker Code (Flash chip manufacturer code) */
    } Bits;
} AMBA_NAND_ID_REG_u;

/*
 * H2 NAND: Length (Active Command and Remained Byte Count) Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  CmdCode:            4;      /* [3:0]: Command code */
        UINT32  Reserved0:          12;     /* [15:4]: Reserved */
        UINT32  RemainedDataLength: 15;     /* [30:16]: Remained data byte size for Read/Program command */
        UINT32  Reserved1:          1;      /* [31]: Reserved */
    } Bits;
} AMBA_NAND_ACT_CMD_REG_u;

/*
 * H2 NAND: Interrupt Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  CmdDoneIrq: 1;      /* [0]: 1 - Command Done Interrupt (write 0 to clear) */
        UINT32  Reserved:   31;     /* [31:1]: Reserved */
    } Bits;
} AMBA_NAND_IRQ_STATUS_REG_u;

/*
 * H2 NAND: Read Command Word Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  CmdWord2:   8;      /* [7:0]: Second command word of read operation */
        UINT32  Reserved0:  8;      /* [15:8]: Reserved */
        UINT32  CmdWord1:   8;      /* [23:16]: First command word of read operation */
        UINT32  Reserved1:  8;      /* [31:24]: Reserved */
    } Bits;
} AMBA_NAND_READ_CMD_WORD_REG_u;

/*
 * Definitions & Constants for H2 NAND: Program Command Word Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  CmdWord2:   8;      /* [7:0]: Second command word of program operation */
        UINT32  Reserved0:  8;      /* [15:8]: : Reserved */
        UINT32  CmdWord1:   8;      /* [23:16]: First command word of program operation */
        UINT32  Reserved1:  8;      /* [31:24]: Reserved */
    } Bits;
} AMBA_NAND_PROG_CMD_WORD_REG_u;

/*
 * H2 NAND: Extended Control Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Spare2xEnable:  1;  /* [0]: 1 - 2x spare area enable */
        UINT32  Reserved0:      22; /* [22:1]: Reserved */
        UINT32  Id5Bytes:       1;  /* [23]: 1 - ID register read in five cycles */
        UINT32  Reserved1:      8;  /* [31:24]: Reserved */
    } Bits;
} AMBA_NAND_EXT_CTRL_REG_u;

/*
 * H2 NAND: Extended ID Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  IdByte5:    8;      /* [7:0]: ID5 = Plane Number, Plane Size */
        UINT32  Reserved:   24;     /* [31:8]: Reserved */
    } Bits;
} AMBA_NAND_EXT_ID_REG_u;

/*
 * H2 NAND: Timing6 Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  tCRL:       8;      /* [7:0]: Chip Enable Falling to Read Plus Falling Edge Time */
        UINT32  tADL:       8;      /* [15:8]: Write Pulse Rising Edge of the last address cycle to the write plus rising edge of the first data cycle */
        UINT32  tRHW:       8;      /* [23:16]: Read Pulse Rising Edge to the Write Puls Falling Edge */
        UINT32  Reserved:   8;      /* [31:24]: Reserved */
    } Bits;
} AMBA_NAND_TIMING6_REG_u;

/*
 * H2 NAND: Customer Command Register
 */
typedef struct {
    UINT32  DataCycle:              5;  /* [4:0]: Number of RE/WE Puls. Valid only if the value of type is not Zero */
    UINT32  WaitCycle:              2;  /* [6:5]: 0 = No Wait Cycles, 1 = Wait For R/B, 2 = Wait For tWHR */
    UINT32  Type:                   2;  /* [8:7]: 0 = No Data Phase, 1 = WE type , 2 = RE type */
    UINT32  CmdPhase2Count:         2;  /* [10:9]: 0 = No Cmd2Phase , 1 = CusCmd2Value0 Would be Present as WE type after Data Phase ,RE after Address Phase */
    UINT32  AdressCycle:            3;  /* [13:11]: 0 = No Adress Phase , 1~4 = Number of Address Cycles during Address Phase */
    UINT32  CmdPhase1Count:         2;  /* [15:14]: 0 = No Cmd Before Adress Phase , 1 = CusCmd1Valu0 would be Present in Command Phase1 ,2 CusCmd1Value1 would be present */
    UINT32  Reserved:               15; /* [30:16]: Reserved */
    UINT32  ChipEnableTerminate:    1;  /* [31]: 0 = CE ping Keep Low, 1 =  CE ping Keep High After Customer Cmd Complete */
} AMBA_NAND_CUSTOM_CMD_REG_s;

/*
 * H2 NAND: Timing6 Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Cmd1Val0:       8;      /* [7:0]: The First Command Byte in Command Phase-1 of Customer Command */
        UINT32  Cmd1Val1:       8;      /* [15:8]: The Second Command Byte in Command Phase-1 of Customer Command */
        UINT32  Cmd2Val0:       8;      /* [23:16]: The First Command Byte in Command Phase-2 of Customer Command */
        UINT32  Cmd2Val1:       8;      /* [31:24]: The Second Command Byte in Command Phase-2 of Customer Command */
    } Bits;
} AMBA_NAND_CUSTOM_CMD_WORD_REG_u;

/*
 * H2 NAND: Controller All Registers
 */
typedef struct {
    volatile UINT32                         Reserved0[8];   /* 0x100~0x11C: Reserved */
    volatile AMBA_NAND_CTRL_REG_u           Ctrl;           /* 0x120(RW): Flash controller control information */
    volatile AMBA_NAND_CMD_REG_s            Cmd;            /* 0x124(RW): Flash controller command and associated address */
    volatile AMBA_NAND_TIMING0_REG_u        Timing0;        /* 0x128(RW): NAND flash chip timing0 parameters */
    volatile AMBA_NAND_TIMING1_REG_u        Timing1;        /* 0x12C(RW): NAND flash chip timing1 parameters */
    volatile AMBA_NAND_TIMING2_REG_u        Timing2;        /* 0x130(RW): NAND flash chip timing2 parameters */
    volatile AMBA_NAND_TIMING3_REG_u        Timing3;        /* 0x134(RW): NAND flash chip timing3 parameters */
    volatile AMBA_NAND_TIMING4_REG_u        Timing4;        /* 0x138(RW): NAND flash chip timing4 parameters */
    volatile AMBA_NAND_TIMING5_REG_u        Timing5;        /* 0x13C(RW): NAND flash chip timing5 parameters */
    volatile AMBA_NAND_STATUS_REG_u         Status;         /* 0x140(RO): NAND flash chip status */
    volatile AMBA_NAND_ID_REG_u             ID;             /* 0x144(RO): NAND flash device ID information */
    volatile UINT32                         CopyDestAddr;   /* 0x148(RW): Start address where the Copy Back command data is to be copied */
    volatile AMBA_NAND_ACT_CMD_REG_u        ActCmd;         /* 0x14C(RO): NAND flash command and associated length */
    volatile AMBA_NAND_IRQ_STATUS_REG_u     IrqStatus;      /* 0x150(RO): NAND flash command done interrupt */
    volatile AMBA_NAND_READ_CMD_WORD_REG_u  ReadCmd;        /* 0x154(RW): NAND flash command word for read command */
    volatile AMBA_NAND_PROG_CMD_WORD_REG_u  ProgCmd;        /* 0x158(RW): NAND flash command word for program command */
    volatile AMBA_NAND_EXT_CTRL_REG_u       ExtCtrl;        /* 0x15C(RW): NAND flash controller extended control information */
    volatile AMBA_NAND_EXT_ID_REG_u         ExtID;          /* 0x160(RW): NAND flash device extended ID information */
    volatile AMBA_NAND_TIMING6_REG_u        Timing6;        /* 0x164(RW): NAND flash chip timing6 parameters */
    volatile UINT32                         Reserved1[2];   /* 0x168~0x16C : Reserved */
    volatile AMBA_NAND_CUSTOM_CMD_REG_s     CusCmd;         /* 0x170(RW): NAND flash chip customer command */
    volatile AMBA_NAND_CUSTOM_CMD_WORD_REG_u CusCmdWord;    /* 0x174(RW): NAND flash chip customer command word */
    volatile UINT32                         Reserved2[2];   /* 0x178~0x17C : Reserved */
    volatile UINT32                         CusCmdData[8];  /* 0x180~0x19C(RW): NAND flash chip customer command Data */
} AMBA_NAND_REG_s;

#endif /* AMBA_REG_NAND_H */
