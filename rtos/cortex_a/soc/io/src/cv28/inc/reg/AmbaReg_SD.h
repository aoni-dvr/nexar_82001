/**
 *  @file AmbaReg_SD.h
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
 *  @details Definitions & Constants for CV22 SD Controllers Registers
 *
 */

#ifndef AMBA_REG_SD_H
#define AMBA_REG_SD_H

/*
 * CV22 SD Controller: Block Size/Block Count
 */
typedef struct {
    UINT32  BlkSize:                12;     /* [11:0]: Transfer Block Size  */
    UINT32  SdmaBufSize:            3;      /* [14:12]: SDMA Buffer Size (Used for SDMA only) */
    UINT32  BlkSize12thBit:         1;      /* [15]: Transfer Block Size 12th bit (for 4KB Data Block Transfers) */
    UINT32  BlkCount:               16;     /* [31:16]: Blocks Count for Current Transfer (Not used for CMD23) */
} AMBA_SD_BLOCK_CTRL_REG_s;

/*
 * CV22 SD Controller: Transfer Mode/Command
 */
typedef struct {
    UINT32  DmaEnable:              1;      /* [0]: 1 - DMA Enable*/
    UINT32  BlkCountEnable:         1;      /* [1]: 1 - Block Count Enable */
    UINT32  AutoCmd12Enable:        1;      /* [2]: 1 - Auto CMD12 Enable */
    UINT32  Reserved0:              1;      /* [3]: Reserved */
    UINT32  DataTransferDir:        1;      /* [4]: Data Transfer Direction Select (1 - Read; 0 - Write) */
    UINT32  MultiBlkEnable:         1;      /* [5]: 1 - Multiple Block Select */
    UINT32  CmdCompleteSignal:      1;      /* [6]: 1 - Command completion signal enable for CE-ATA Devices */
    UINT32  Reserved1:              9;      /* [15:7]: Reserved */

    UINT32  ResponseType:           2;      /* [17:16]: Response Type Select */
    UINT32  Reserved2:              1;      /* [18]: Reserved */
    UINT32  CmdCrcCheckEnable:      1;      /* [19]: 1 - Command CRC Check Enable */
    UINT32  CmdIndexCheckEnable:    1;      /* [20]: 1 - Command Index Check Enable */
    UINT32  DataPresent:            1;      /* [21]: 1 - Data Present; 0 - No Data Present */
    UINT32  CmdType:                2;      /* [23:22]: Command Type */
    UINT32  CmdIndex:               6;      /* [29:24]: Command Index (CMD0-63, ACMD0-63) */
    UINT32  Reserved3:              2;      /* [31:30]: Reserved */
} AMBA_SD_TRANSFER_CTRL_REG_s;

#define    AMBA_SD_NO_RESPONSE                         0U        /* No Response */
#define    AMBA_SD_RESPONSE_LENGTH_136_BIT             1U        /* Response length 136 bits */
#define    AMBA_SD_RESPONSE_LENGTH_48_BIT              2U        /* Response length 48 bits */
#define    AMBA_SD_RESPONSE_LENGTH_48_BIT_CHECK_BUSY   3U        /* Response length 48 bits, check Busy after response */

typedef enum {
    AMBA_SD_CMD_TYPE_NORMAL = 0,                /* Normal */
    AMBA_SD_CMD_TYPE_SUSPEND,                   /* Suspend */
    AMBA_SD_CMD_TYPE_RESUME,                    /* Resume */
    AMBA_SD_CMD_TYPE_ABORT                      /* Abort */
} AMBA_SD_CMD_TYPE_e;

/*
 * CV22 SD Controller: SD Present State Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  CmdLineInUse:           1;      /* [0]: 1 - Command Inhibit (CMD) */
        UINT32  DataLineInUse:          1;      /* [1]: Command Inhibit (DAT) */
        UINT32  DataLineActive:         1;      /* [2]: DAT line is in use */
        UINT32  Reserved0:              5;      /* [7:3]: Reserved */
        UINT32  WriteTransferActive:    1;      /* [8]: This status indicates a write transfer is active */
        UINT32  ReadTransferActive:     1;      /* [9]: This status is used for detecting completion of a read transfer */
        UINT32  BufWriteEnable:         1;      /* [10]: This status is used for non-DMA write transfers */
        UINT32  BufReadEnable:          1;      /* [11]: This status is used for non-DMA read transfers */
        UINT32  Reserved2:              4;      /* [15:12]: Reserved */
        UINT32  CardInserted:           1;      /* [16]: This bit indicates whether a card has been inserted */
        UINT32  CardStateStable:        1;      /* [17]: Card State Stable. This bit is used for testing. */
        UINT32  CardDetectPin:          1;      /* [18]: Card Detect Pin Level */
        UINT32  WriteProtectSwitchPin:  1;      /* [19] Write Protect Switch Pin Level */
        UINT32  DataLineSignalLow4:     4;      /* [23:20]: DAT[3:0] Line Signal Level */
        UINT32  CmdLineSignal:          1;      /* [24]: CMD Line Signal Level */
        UINT32  DataLineSignalHigh4:    4;      /* [28:25]: DAT[7:4] Line Signal Level */
        UINT32  Reserved3:              3;      /* [31:29]: Reserved */
    } Bits;
} AMBA_SD_PRESENT_STATE_REG_u;

/*
 * CV22 SD Controller: Host Control/Power Control/Block Gap/Wakeup Control Register
 */
typedef struct {
    UINT32  LedCtrl:                1;      /* [0]: LED Control */
    UINT32  DataWidth:              1;      /* [1]: Data Transfer Width: 1 - 4-bit; 0 - 1-bit */
    UINT32  HighSpeedMode:          1;      /* [2]: 1 - Hight Speed Mode (up to 50MHz); 0 - up to 25MHz */
    UINT32  DataBitMode:            1;      /* [3]: 1 - 8-bit mode; 0 - 1-bit mode */
    UINT32  DmaSelect:              1;      /* [4]: 1 - 32-bit address ADMA */
    UINT32  Reserved0:              3;      /* [7:5]: Reserved */

    UINT32  BusPower:               1;      /* [8]:SD Bus Power: 1 - Power on; 0 - Power off */
    UINT32  BusVoltage:             3;      /* [11:9]: SD Bus Voltage Select */
    UINT32  Reserved1:              4;      /* [15:12]: Reserved */

    UINT32  StopAtBlkGap:           1;      /* [16]: 1 - Stop At Block Gap Request */
    UINT32  ContinueReq:            1;      /* [17]: 1 - Restart */
    UINT32  ReadWaitCtrl:           1;      /* [18]: 1 - Enable Read Wait Control */
    UINT32  IrqAtBlkGap:            1;      /* [19]: 1 - Interrupt At Block Gap */
    UINT32  Reserved2:              4;      /* [23:20]: Reserved */

    UINT32  WakeupOnCardIrq:        1;      /* [24]: 1 - Enable Wakeup On Card Interrupt */
    UINT32  WakeupOnCardInsertion:  1;      /* [25]: 1 - Enable Wakeup On Card Insertion */
    UINT32  WakeupOnCardRemoval:    1;      /* [26]: 1 - Enable Wakeup On Card Removal */
    UINT32  Reserved3:              5;      /* [31:27]: Reserved */
} AMBA_SD_MISC_CTRL0_REG_s;

typedef enum {
    AMBA_SD_BUS_VOLTAGE_1D8V = 5,               /* 1.8V (Typ.) */
    AMBA_SD_BUS_VOLTAGE_3D0V,                   /* 3.0V (Typ.) */
    AMBA_SD_BUS_VOLTAGE_3D3V                    /* 3.3V (Flat-top) */
} AMBA_SD_BUS_VOLTAGE_e;

/*
 * CV22 SD Controller: Clock Control / Timeout Control / Software Reset Register
 */
typedef struct {
    UINT32  InternalClkEnable:      1;      /* [0]: 1 - Internal Clock Enable */
    UINT32  InternalClkStable:      1;      /* [1]: 1 - Internal Clock Stable (Ready) */
    UINT32  SdClockEnable:          1;      /* [2]: 1 - SD Clock Enabled */
    UINT32  Reserved0:              5;      /* [7:3]: Reserved */

    UINT32  SdClkDivider:           8;      /* [15:8]: SD[N]_CLK Frequency Select */

    UINT32  DataTimeoutCounter:     4;      /* [19:16]: This value determines the interval by which DAT line timeouts are detected*/
    UINT32  Reserved1:              4;      /* [23:20]: Reserved */

    UINT32  SoftResetAll:           1;      /* [24]: 1- This reset affects the entire Host Controller except for the card detection circuit */
    UINT32  SoftResetCmdLine:       1;      /* [25]: 1- Software Reset for CMD line */
    UINT32  SoftResetDataLine:      1;      /* [26]: 1- Software Reset for DAT line */
    UINT32  Reserved3:              5;      /* [31:27]: Reserved */
} AMBA_SD_MISC_CTRL1_REG_s;

typedef enum {
    AMBA_SD_CLK_DIVIDER_1   = 0x00,             /* base clock */
    AMBA_SD_CLK_DIVIDER_2   = 0x01,             /* base clock / 2 */
    AMBA_SD_CLK_DIVIDER_4   = 0x02,             /* base clock / 4 */
    AMBA_SD_CLK_DIVIDER_8   = 0x04,             /* base clock / 8 */
    AMBA_SD_CLK_DIVIDER_16  = 0x08,             /* base clock / 16 */
    AMBA_SD_CLK_DIVIDER_32  = 0x10,             /* base clock / 32 */
    AMBA_SD_CLK_DIVIDER_64  = 0x20,             /* base clock / 64 */
    AMBA_SD_CLK_DIVIDER_128 = 0x40,             /* base clock / 128 */
    AMBA_SD_CLK_DIVIDER_256 = 0x80              /* base clock / 256 */
} AMBA_SD_CLK_DIVIDER_e;

/*
 * CV22 SD Controller: Normal Interrupt Status / Error Interrupt Status Register
 */
typedef struct {
    UINT32  CmdDone:                1;      /* [0]: 1 - Command Complete */
    UINT32  TransferDone:           1;      /* [1]: 1 - a read / write transaction is completed */
    UINT32  BlkGapEvent:            1;      /* [2]: 1 - Block Gap Event */
    UINT32  DmaIRQ:                 1;      /* [3]: 1 - DMA Interrupt is Generated */
    UINT32  BufWriteReady:          1;      /* [4]: 1 - Ready to Write Buffer */
    UINT32  BufReadReady:           1;      /* [5]: 1 - Ready to read Buffer */
    UINT32  CardInsertion:          1;      /* [6]: 1 - Card Inserted */
    UINT32  CardRemoval:            1;      /* [7]: 1 - Card Removed */
    UINT32  CardIRQ:                1;      /* [8]: 1 - Generate Card Interrupt */
    UINT32  Reserved0:              6;      /* [14:9]:  Reserved */
    UINT32  ErrorIRQ:               1;      /* [15]: 1 - Error Interrupt */
    UINT32  CmdTimeoutError:        1;      /* [16]: 1 - Command Timeout Error */
    UINT32  CmdCrcError:            1;      /* [17]: 1 - Command CRC Error */
    UINT32  CmdEndBitError:         1;      /* [18]: 1 - Command End Bit Error */
    UINT32  CmdIndexError:          1;      /* [19]: 1 - Command Index Error */
    UINT32  DataTimeoutError:       1;      /* [20]: 1 - Data Timeout Error */
    UINT32  DataCrcError:           1;      /* [21]: 1 - Data CRC Error */
    UINT32  DataEndBitError:        1;      /* [22]: 1 - Data End Bit Error */
    UINT32  CurrentLimitError:      1;      /* [23]: 1 - Current Limit Error (Power Fail) */
    UINT32  AutoCmd12Error:         1;      /* [24]: 1 - Auto CMD12 Error */
    UINT32  AdmaError:              1;      /* [25]: 1 - ADMA Error */
    UINT32  Reserved1:              2;      /* [27:26]: Reserved */
    UINT32  VendorErrorStatus:      4;      /* [31:28]: Vendor Specific Error Status */
} AMBA_SD_IRQ_STATUS_REG_s;

/*
 * CV22 SD Controller: Normal Interrupt Status Enable / Error Interrupt Status Enable Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  CmdDone:                1;      /* [0]: 1 - Command Complete Status Enabled */
        UINT32  TransferDone:           1;      /* [1]: 1 - Transfer Complete Status Enabled */
        UINT32  BlkGapEvent:            1;      /* [2]: 1 - Block Gap Event Status Enabled */
        UINT32  DmqIRQ:                 1;      /* [3]: 1 - DMA Interrupt Status Enabled */
        UINT32  BufWriteReady:          1;      /* [4]: 1 - Buffer Write Ready Status Enabled */
        UINT32  BufReadReady:           1;      /* [5]: 1 - Buffer Read Ready Status Enabled */
        UINT32  CardInsertion:          1;      /* [6]: 1 - Card Insertion Status Enabled */
        UINT32  CardRemoval:            1;      /* [7]: 1 - Card Removal Status Enabled */
        UINT32  CardIRQ:                1;      /* [8]: 1 - Card Interrupt Status Enabled */
        UINT32  Reserved0:              6;      /* [14:9]:  Reserved */
        UINT32  FixedTo0:               1;      /* [15](R): Fixed to 0 */
        UINT32  CmdTimeoutError:        1;      /* [16]: 1 - Command Timeout Error Status Enabled */
        UINT32  CmdCrcError:            1;      /* [17]: 1 - Command CRC Error Status Enabled */
        UINT32  CmdEndBitError:         1;      /* [18]: 1 - Command End Bit Error Status Enabled */
        UINT32  CmdIndexError:          1;      /* [19]: 1 - Command Index Error Status Enabled */
        UINT32  DataTimeoutError:       1;      /* [20]: 1 - Data Timeout Error Status Enabled */
        UINT32  DataCrcError:           1;      /* [21]: 1 - Data CRC Error Status Enabled */
        UINT32  DataEndBitError:        1;      /* [22]: 1 - Data End Bit Error Status Enabled */
        UINT32  CurrentLimitError:      1;      /* [23]: 1 - Current Limit Error Status Enabled */
        UINT32  AutoCmd12Error:         1;      /* [24]: 1 - Auto CMD12 Error Status Enabled */
        UINT32  AdmaError:              1;      /* [25]: 1 - ADMA Error Status Enabled */
        UINT32  Reserved1:              2;      /* [27:26]: Reserved */
        UINT32  VendorErrorStatus:      4;      /* [31:28]: Vendor Specific Error Status Enabled */
    } Bits;
} AMBA_SD_IRQ_ENABLE_REG_u;

/*
 * CV22 SD Controller: Normal Interrupt Signal Enable / Error Interrupt Signal Enable Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  CmdDone:                1;      /* [0]: 1 - Command Complete Signal Enabled */
        UINT32  TransferDone:           1;      /* [1]: 1 - Transfer Complete Signal Enabled */
        UINT32  BlkGapEvent:            1;      /* [2]: 1 - Block Gap Event Signal Enabled */
        UINT32  DmqIRQ:                 1;      /* [3]: 1 - DMA Interrupt Signal Enabled */
        UINT32  BufWriteReady:          1;      /* [4]: 1 - Buffer Write Ready Signa Enabled */
        UINT32  BufReadReady:           1;      /* [5]: 1 - Buffer Read Ready Signal Enabled */
        UINT32  CardInsertion:          1;      /* [6]: 1 - Card Insertion Signal Enabled */
        UINT32  CardRemoval:            1;      /* [7]: 1 - Card Removal Signal Enabled */
        UINT32  CardIRQ:                1;      /* [8]: 1 - Card Interrupt Signal Enabled */
        UINT32  Reserved0:              6;      /* [14:9]:  Reserved */
        UINT32  FixedTo0:               1;      /* [15](R): Fixed to 0 */
        UINT32  CmdTimeoutError:        1;      /* [16]: 1 - Command Timeout Error Signal Enabled */
        UINT32  CmdCrcError:            1;      /* [17]: 1 - Command CRC Error Signal Enabled */
        UINT32  CmdEndBitError:         1;      /* [18]: 1 - Command End Bit Error Signal Enabled */
        UINT32  CmdIndexError:          1;      /* [19]: 1 - Command Index Error Signal Enabled */
        UINT32  DataTimeoutError:       1;      /* [20]: 1 - Data Timeout Error Signal Enabled */
        UINT32  DataCrcError:           1;      /* [21]: 1 - Data CRC Error Signal Enabled */
        UINT32  DataEndBitError:        1;      /* [22]: 1 - Data End Bit Error Signal Enabled */
        UINT32  CurrentLimitError:      1;      /* [23]: 1 - Current Limit Error Signal Enabled */
        UINT32  AutoCmd12Error:         1;      /* [24]: 1 - Auto CMD12 Error Signal Enabled */
        UINT32  AdmaError:              1;      /* [25]: 1 - ADMA Error Status Enabled */
        UINT32  Reserved1:              2;      /* [27:26]: Reserved */
        UINT32  VendorErrorStatus:      4;      /* [31:28]: Vendor Specific Error Signal Enabled */
    } Bits;
} AMBA_SD_IRQ_SIGNAL_ENABLE_REG_u;

/*
 * CV22 SD Controller: Auto CMD12 Error Status Register
 */
typedef struct {
    UINT32  ACmd12NotExe:           1;      /* [0] Auto CMD12 not Executed */
    UINT32  ACmd12TimeoutErr:       1;      /* [1] */
    UINT32  ACmd12CrcErr:           1;      /* [2] */
    UINT32  ACmd12EndBitErr:        1;      /* [3] */
    UINT32  ACmd12IndexErr:         1;      /* [4] */
    UINT32  Reserved:               2;      /* [6:5] */
    UINT32  CmdNotIssuedErr:        1;      /* [7] Not Issued By Auto CMD12 Error */
    UINT32  Reserved2:              10;     /* [17:8] */
    UINT32  DdrMode:                1;      /* [18] 0 - SDR mode, 1 - DDR mode */
    UINT32  Enable18VoltSignaling:  1;      /* [19] 1.8 Volt signaling enable*/
    UINT32  Reserved3:              9;      /* [28:20] */
    UINT32  Cmd23Disable:           1;      /* [29] */
    UINT32  AsyncIntrEnable:        1;      /* [30] */
    UINT32  Reserved4:              1;      /* [31] */
} AMBA_SD_AUTO_CMD12_STATUS_REG_s;

/*
 * CV22 SD Controller: Capabilities Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  TimeoutClkFreq:         6;      /* [5:0]: Timeout Clock Frequency */
        UINT32  Reserved0:              1;      /* [6]: Reserved */
        UINT32  TimeoutClkUnit:         1;      /* [7]: Timeout Clock Unit: 1 - MHz; 0 - KHz */
        UINT32  BaseClkFreq:            6;      /* [13:8]: Base Clock Frequency for SD Clock */
        UINT32  Reserved1:              2;      /* [15:14]: Reserved */
        UINT32  MaxBlkLength:           2;      /* [17:16]: Max Block Length */
        UINT32  Reserved2:              1;      /* [18]: Reserved */
        UINT32  AdmaSupport:            1;      /* [19]: ADM Support */
        UINT32  Reserved3:              1;      /* [20]: Reserved */
        UINT32  HighSpeedSupport:       1;      /* [21]: 1 - High Speed Support */
        UINT32  DmaSupport:             1;      /* [22]: 1 - DMA Support */
        UINT32  SuspendResumeSupport:   1;      /* [23]: 1 Suspend / Resume Support */
        UINT32  Voltage3D3vSupport:     1;      /* [24]: 1 - Voltage Support 3.3V */
        UINT32  Voltage3D0vSupport:     1;      /* [25]: 1 - Voltage Support 3.0V */
        UINT32  Voltage1D8vSupport:     1;      /* [26]: 1 - Voltage Support 1.8V */
        UINT32  IrqSupport:             1;      /* [27]: 1 - Interrupt Support */
        UINT32  Reserved4:              4;      /* [31:28]: Reserved */
    } Bits;
} AMBA_SD_CAPABILITY_REG_u;

/*
 * CV22 SD Controller: Maximum Current Capabilities Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Voltage3D3V:            8;      /* [7:0]: Maximum Current for 3.3 V */
        UINT32  Voltage3D0V:            8;      /* [15:8]: Maximum Current for 3.0 V */
        UINT32  Voltage1D8V:            8;      /* [23:16]: Maximum Current for 1.8 V */
        UINT32  Reserved:               8;      /* [31:24]: Reserved */
    } Bits;
} AMBA_SD_MAX_CURRENT_REG_u;

/*
 * CV22 SD Controller: Advanced DMA (ADMA) Error Status Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  AdmaErrorState:         2;      /* [1:0]: ADMA Error State */
        UINT32  AdmaLengthError:        1;      /* [2]: 1 - ADMA Length Mismatch Error */
        UINT32  InvalidAdmaDescriptor:  1;      /* [3]: 1 - Invalid ADMA Descriptor */
        UINT32  AdmaBlockError:         1;      /* [4]: 1 - ADMA Block Error */
        UINT32  AdmaByteError:          1;      /* [5]: 1 - ADMA Byte Error */
        UINT32  Reserved:               26;     /* [31:6]: Reserved */
    } Bits;
} AMBA_SD_ADMA_STATUS_REG_u;

/*
 * CV22 SD Controller: SDXC Control Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  Reserved0:              15;     /* [14:0]: Reserved */
        UINT32  DdrMode:                1;      /* [15]: 1 - DDR mode; 0 - SDR mode */
        UINT32  Reserved1:              15;     /* [30:16]: Reserved */
        UINT32  Cmd23Disable:           1;      /* [31]: 1 - Disable SDXC CMD23; 0 - Enable SDXC CMD23 */
    } Bits;
} AMBA_SD_SDXC_CTRL_REG_u;

/*
 * CV22 SD Controller: Boot Control Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  BootCodeByteSize:       12;     /* [11:0]: Boot Code Length in Bytes */
        UINT32  Reserved0:              4;      /* [15:12]: Reserved */
        UINT32  ResetFioBootEnable:     1;      /* [16]: FIO Boot Enable on SD Software Reset */
        UINT32  Reserved1:              15;     /* [31:17]: Reserved */
    } Bits;
} AMBA_SD_BOOT_CTRL_REG_u;

/*
 * CV22 SD Controller: Boot Status Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  ReadyToBoot:            1;      /* [0]: Ready to Boot */
        UINT32  Reserved0:              15;     /* [15:1]: Reserved */
        UINT32  BootEnd:                1;      /* [16]: Boot End */
        UINT32  Reserved1:              7;      /* [23:17]: Reserved */
        UINT32  BootEndAlt:             1;      /* [24]: Boot End Alt */
        UINT32  Reserved2:              7;      /* [31:25]: Reserved */
    } Bits;
} AMBA_SD_BOOT_STATUS_REG_u;

/*
 * CV22 SD Controller: Voltage Switch Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  DataLineStatus:         4;      /* [3:0]: DAT Status */
        UINT32  Reserved0:              12;     /* [15:4]: Reserved */
        UINT32  CmdLineStatus:          1;      /* [16]: CMD Status */
        UINT32  Reserved1:              15;     /* [31:17]: Reserved */
    } Bits;
} AMBA_SD_VOLTAGE_SWITCH_REG_u;

/*
 * CV22 SD Read Latency Control Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  NcrReg:                    4;      /* [3:0] Determine the cycle counts between command and response */
        UINT32  TrnCrcStatus:              4;      /* [7:4] Determine the cycle counts between write data and response*/
        UINT32  DsmCmdConflictChkTap:      3;      /* [10:8] Selects which tap should be used to check for command conflict in default-speed mode */
        UINT32  DsmCmdConflictChkDisable:  1;      /* [11] 1 = disable command conflict check in default-speed mode*/
        UINT32  HsmCmdConflictChkTap:      3;      /* [14:12] Selects which tap should be used to check for command conflict in high-speed mode */
        UINT32  HsmCmdConflictChkDisable:  1;      /* [15] 1 = disable command conflict check in high-speed mode*/
        UINT32  Reserved:                  16;     /* [31:16] */
    } Bits;
} AMBA_SD_READ_LATENCY_CTRL_REG_u;

/*
 * CV22 SD Controller: Slot Interrupt Status and Controller Version Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  IrqSignalForEachSlot:   8;      /* [7:0]: Interrupt Signal for Each Slot */
        UINT32  Reserved:               8;      /* [15:8]: Reserved */
        UINT32  SpecVer:                8;      /* [23:16]: Specification Version */
        UINT32  VendorVer:              8;      /* [31:24]: Vendor Version */
    } Bits;
} AMBA_SD_SLOT_IRQ_VER_REG_u;

/*
 * CV22 SD Controller: ADMA Descriptor
 */
typedef struct {
    UINT32  AttributeValid:         1;      /* [0]: 1 - Indicates this line of descriptor is effective */
    UINT32  AttributeEnd:           1;      /* [1]: 1 - Indicates end of the descriptor */
    UINT32  AttributeIRQ:           1;      /* [2]: 1 - Generates DMA Interrupt when the operation of the descriptor line is completed */
    UINT32  AttributeWord:          1;      /* [3]: 1 - Indicates size in words (32-bit) */
    UINT32  AttributeAct:           2;      /* [5:4]: Actions */

    UINT32  Reserved:               10;     /* [15:6]: Reserved */
    UINT32  Length:                 16;     /* [31:16]: Length */
    UINT32  SysMemAddr:             32;     /* [63:32]: System memory address */
} AMBA_SD_ADMA_DESCRIPTOR_s;

#define     AMBA_SD_ADMA_ATTR_NOP        0U      /* Do not execute current line and go to next line */
#define     AMBA_SD_ADMA_ATTR_RESERVED   1U      /* Do not execute current line and go to next line */
#define     AMBA_SD_ADMA_ATTR_TRANSFER   2U      /* Transfer data of one descriptor line */
#define     AMBA_SD_ADMA_ATTR_LINK       3U      /* Link to another descriptor */

/*
 * CV22 SD Controller: All Registers
 */
typedef struct {
    volatile UINT32                             SdmaSysMemAddr;     /* 0x000(RW): System Address (High/Low) for SDMA transfers */
    volatile AMBA_SD_BLOCK_CTRL_REG_s           BlkCtrl;            /* 0x004(RW): Block Size/Block Count (SD/MMC boot) */
    volatile UINT32                             CmdArgument;        /* 0x008(RW): Command Argument */
    volatile AMBA_SD_TRANSFER_CTRL_REG_s        TransferCtrl;       /* 0x00C(RW): Transfer Mode/Command (SD/MMC boot) */
    volatile UINT32                             Response[4];        /* 0x010 - 0x01C(R): Response */
    volatile UINT32                             DataBuf;            /* 0x020(RW): Buffer data port */
    volatile AMBA_SD_PRESENT_STATE_REG_u        PresentState;       /* 0x024(R): Present state */
    volatile AMBA_SD_MISC_CTRL0_REG_s           MiscCtrl0;          /* 0x028(RW): Host Control (SD/MMC boot)/Power Control/Block Gap/Wakeup */
    volatile AMBA_SD_MISC_CTRL1_REG_s           MiscCtrl1;          /* 0x02C(RW): Clock Control (SD/MMC boot)/Timeout Control/ Software Reset */
    volatile AMBA_SD_IRQ_STATUS_REG_s           IrqStatus;          /* 0x030(RW): Normal/Error Interrupt Status */
    volatile AMBA_SD_IRQ_ENABLE_REG_u           IrqEnable;          /* 0x034(RW): Normal/Error Interrupt Status enable */
    volatile AMBA_SD_IRQ_SIGNAL_ENABLE_REG_u    IrqSignalEnable;    /* 0x038(RW): Normal interrupt signal enable */
    volatile AMBA_SD_AUTO_CMD12_STATUS_REG_s    AutoCmd12Status;    /* 0x03C(R): Auto CMD12 error status register */
    volatile AMBA_SD_CAPABILITY_REG_u           Capability;         /* 0x040(R): Capabilities register */
    volatile UINT32                             Reserved0;          /* 0x044: Reserved */
    volatile AMBA_SD_MAX_CURRENT_REG_u          MaxCurrent;         /* 0x048(R): Maximum current capabilities */
    volatile UINT32                             Reserved1[2];       /* 0x04C-0x050: Reserved */
    volatile AMBA_SD_ADMA_STATUS_REG_u          AdmaStatus;         /* 0x054(R): Advanced DMA (ADMA) error status */
    volatile UINT32                             AdmaAddr;           /* 0x058(RW): Advanced DMA (ADMA) system address */
    volatile UINT32                             Reserved2;          /* 0x05C: Reserved */
    volatile AMBA_SD_SDXC_CTRL_REG_u            SdxcCtrl;           /* 0x060(RW): SDXC Control (SD/MMC boot) */
    volatile UINT32                             Reserved3[3];       /* 0x064 - 0x06C: Reserved */
    volatile AMBA_SD_BOOT_CTRL_REG_u            BootCtrl;           /* 0x070(RW): Boot Control (SD/MMC boot) */
    volatile AMBA_SD_BOOT_STATUS_REG_u          BootStatus;         /* 0x074(RW): Boot Status (SD/MMC boot) */
    volatile UINT32                             BlkCountCMD23;      /* 0x078(R): Block Count for CMD23 */
    volatile AMBA_SD_VOLTAGE_SWITCH_REG_u       VoltageSwitch;      /* 0x07C(RW): Voltage switch */
    volatile UINT32                             Reserved4[30];      /* 0x080 - 0x0F4: Reserved */
    volatile AMBA_SD_READ_LATENCY_CTRL_REG_u    RdLatencyCtrl;      /* 0x0F8 Read latency control */
    volatile AMBA_SD_SLOT_IRQ_VER_REG_u         SlotIrqVerID;       /* 0x0FC(R): Slot Interrupt / Version ID */
} AMBA_SD_REG_s;

#endif /* _AMBA_REG_SD_H_ */
