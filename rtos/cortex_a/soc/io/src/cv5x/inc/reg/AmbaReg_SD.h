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

#define AMBA_SD_SDMA_BUF_SIZE_4KB           0U
#define AMBA_SD_SDMA_BUF_SIZE_8KB           1U
#define AMBA_SD_SDMA_BUF_SIZE_16KB          2U
#define AMBA_SD_SDMA_BUF_SIZE_32KB          3U
#define AMBA_SD_SDMA_BUF_SIZE_64KB          4U
#define AMBA_SD_SDMA_BUF_SIZE_128KB         5U
#define AMBA_SD_SDMA_BUF_SIZE_256KB         6U
#define AMBA_SD_SDMA_BUF_SIZE_512KB         7U

/*
 * CV22 SD Controller: Transfer Mode/Command
 */
typedef struct {
    UINT32  DmaEnable:              1;      /* [0]: 1 - DMA Enable*/
    UINT32  BlkCountEnable:         1;      /* [1]: 1 - Block Count Enable */
    UINT32  AutoCmd12Enable:        2;      /* [3:2]: 1 - Auto CMD12, CMD23 Enable */
    UINT32  DataTransferDir:        1;      /* [4]: Data Transfer Direction Select (1 - Read; 0 - Write) */
    UINT32  MultiBlkEnable:         1;      /* [5]: 1 - Multiple Block Select */
    UINT32  RespType:               1;      /* [6]: Response type R1/R5. Select either R1 or R5 response type when Response Error check is selected */
    UINT32  RespErrChkEnable:       1;      /* [7]: Response Error check enabled */
    UINT32  RespIntDisable:         1;      /* [8]: Response Interrupt Disable */
    UINT32  Reserved1:              7;      /* [15:9]: Reserved */

    UINT32  ResponseType:           2;      /* [17:16]: Response Type Select */
    UINT32  SubCmdFlag:             1;      /* [18]: 1 - Sub-Command, 0 - main command */
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
        UINT32  ReTuneRequest:          1;      /* [3]: Re-Tune request */
        UINT32  DataLineSignalHigh4:    4;      /* [7:4]: DAT[7:4] Line Signal Level */
        UINT32  WriteTransferActive:    1;      /* [8]: This status indicates a write transfer is active */
        UINT32  ReadTransferActive:     1;      /* [9]: This status is used for detecting completion of a read transfer */
        UINT32  BufWriteEnable:         1;      /* [10]: This status is used for non-DMA write transfers */
        UINT32  BufReadEnable:          1;      /* [11]: This status is used for non-DMA read transfers */
        UINT32  Reserved2:              4;      /* [15:12]: Reserved */
        UINT32  CardInserted:           1;      /* [16]: This bit indicates whether a card has been inserted */
        UINT32  CardStateStable:        1;      /* [17]: Card State Stable. 1 - no card or card inserted */
        UINT32  CardDetectPin:          1;      /* [18]: Card Detect Pin Level */
        UINT32  WriteProtectSwitchPin:  1;      /* [19] Write Protect Switch Pin Level */
        UINT32  DataLineSignalLow4:     4;      /* [23:20]: DAT[3:0] Line Signal Level */
        UINT32  CmdLineSignal:          1;      /* [24]: CMD Line Signal Level */
        UINT32  HostRegVoltStable:      1;      /* [25]: 1 - Host regulator voltage is stable */
        UINT32  Reserved26:             1;      /* [26]: Reserved */
        UINT32  CmdIssueErr:               1;      /* [27]: 1 - Sub command status, 0 - Main cmd status */
        UINT32  SubCmdStatus:           1;      /* [28]: 1 - Sub command status, 0 - Main cmd status */
        UINT32  Reserved3:              3;      /* [31:29]: Reserved */
    } Bits;
} AMBA_SD_PRESENT_STATE_REG_u;

#define AMBA_SD_VOLTAGE_SD_33V (0x7U)
#define AMBA_SD_VOLTAGE_SD_30V (0x6U)
#define AMBA_SD_VOLTAGE_SD_18V (0x5U)

#define AMBA_SD_VOLTAGE_EMMC_33V (0x7U)
#define AMBA_SD_VOLTAGE_EMMC_18V (0x6U)
#define AMBA_SD_VOLTAGE_EMMC_12V (0x5U)

/*
 * CV22 SD Controller: Host Control/Power Control/Block Gap/Wakeup Control Register
 */
typedef struct {
    UINT32  LedCtrl:                1;      /* [0]: LED Control */
    UINT32  DataWidth:              1;      /* [1]: Data Transfer Width: 1 - 4-bit; 0 - 1-bit */
    UINT32  HighSpeedMode:          1;      /* [2]: 1 - Hight Speed Mode (up to 50MHz); 0 - up to 25MHz */
    UINT32  DmaSelect:              2;      /* [4:3]: 0: SDMA, 1:Reserved, 2:ADMA2, 3:ADMA2 or ADMA3 */
    UINT32  DataBitMode:            1;      /* [5]: EXT_DAT_XFER. 1 - 8-bit mode; 0 - select by data transfer width */
    UINT32  CardDetectTestLvl:      1;      /* [6]: Card detect test level, 1 - card inserted, 0 - no card */
    UINT32  CardDetectSingalSel:    1;      /* [7]: Card detect Singal selection. 0 - for normal use */

    UINT32  BusPower:               1;      /* [8]:SD Bus Power: 1 - Power on; 0 - Power off */
    UINT32  BusVoltage:             3;      /* [11:9]: SD Bus Voltage Select */
    UINT32  Reserved1:              1;      /* [12]: Reserved */
    UINT32  BusVoltageVdd2:         3;      /* [15:13]: Reserved */

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
    UINT32  PllEnable:              1;      /* [3]: */
    UINT32  Reserved0:              1;      /* [4]: Reserved */

    UINT32  ClkGenSelect:           1;      /* [5]: Clock generator Select */
    UINT32  UpperFreqSelect:        2;      /* [7:6]: Specify the upper 2 bits of 10bit SDCLK/RCLK Frequency Select */

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

    UINT32  IntAIRQ:                1;      /* [9]: 1 - INT_A (Embedded) Status */
    UINT32  IntBIRQ:                1;      /* [10]: 1 - INT_B (Embedded) Status */
    UINT32  IntCIRQ:                1;      /* [11]: 1 - INT_C (Embedded) Status */
    UINT32  ReTuneEventIRQ:         1;      /* [12]: 1 - Re-Tuning Event (UHS-I only) Status */
    UINT32  FxTuneEventIRQ:         1;      /* [13]: 1 - FX Event Status */
    UINT32  CqeEventIRQ:            1;      /* [14]: 1 - CQE Event Status */

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
    UINT32  TuningError:            1;      /* [26]: R/W1C - Tuning Error Error. Set when an unrecoverable error is detected in a tuning circuit except during the tuning procedure */
    UINT32  RespError:              1;      /* [27]: 1 - Response Error. Host Controller Checks R1 or R5 response. If an error is detected in a response, this bit is set to 1 */
    UINT32  BootAckError:           1;      /* [28]: 1 - Boot Acknowledg Error. Set when there is a timeout for boot acknowledgement or when detecting boot ack status having a value other than 010 */
    UINT32  VendorErrorStatus:      3;      /* [31:29]: Vendor Specific Error Status */
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
        UINT32  IntAStatus:             1;      /* [9]: 1 - INT_A (Embedded) Status Enabled */
        UINT32  IntBStatus:             1;      /* [10]:  1 - INT_B (Embedded) Status Enabled */
        UINT32  IntCStatus:             1;      /* [11]: 1 - INT_C (Embedded) Status Enabled */
        UINT32  ReTuneEventStatus:      1;      /* [12]: 1 - Re-Tuning Event (UHS-I only) Status Enabled */
        UINT32  FxTuneEventStatus:      1;      /* [13]: 1 - FX Event Status Enabled */
        UINT32  CqeEventStatus:         1;      /* [14]: 1 - CQE Event Status Enabled */
        UINT32  Reserved0:              1;      /* [15](R): Fixed to 0 */

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
        UINT32  TuningErrEnable:        1;      /* [26]:  */
        UINT32  RespErrEnable:          1;      /* [27]:  */
        UINT32  BootAckErrEnable:       1;      /* [28]:  */
        UINT32  VendorErrorStatus:      3;      /* [31:29]: Vendor Specific Error Status Enabled */
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
        UINT32  IntAIRQ:                1;      /* [9]: 1 - INT_A (Embedded) Status */
        UINT32  IntBIRQ:                1;      /* [10]: 1 - INT_B (Embedded) Status */
        UINT32  IntCIRQ:                1;      /* [11]: 1 - INT_C (Embedded) Status */
        UINT32  ReTuneEventIRQ:         1;      /* [12]: 1 - Re-Tuning Event (UHS-I only) Status */
        UINT32  FxTuneEventIRQ:         1;      /* [13]: 1 - FX Event Status */
        UINT32  CqeEventIRQ:            1;      /* [14]: 1 - CQE Event Status */
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
        UINT32  TuningErrorSigEnable:   1;      /* [26]: 1 - ADMA Error Status Enabled */
        UINT32  RespErrorSigEnable:     1;      /* [27]: 1 - ADMA Error Status Enabled */
        UINT32  BootAckErrorSigEnable:  1;      /* [28]: 1 - ADMA Error Status Enabled */
        UINT32  VendorErrorStatus:      3;      /* [31:29]: Vendor Specific Error Signal Enabled */
    } Bits;
} AMBA_SD_IRQ_SIGNAL_ENABLE_REG_u;

/* UHS Mode (SD/UHS-II mode only) */
#define UHS_SDR12  (0x0U)
#define UHS_SDR25  (0x1U)
#define UHS_SDR50  (0x2U)
#define UHS_SDR104 (0x3U)
#define UHS_DDR50  (0x4U)

#define UHS_UHS_II (0x7U)

/* eMMC Speed Mode (eMMC mode only) */
#define EMMC_MODE_LEGACY         (0x0U)
#define EMMC_MODE_HIGH_SPEED_SDR (0x1U)

#define EMMC_MODE_HS200          (0x3U)
#define EMMC_MODE_HIGH_SPEED_DDR (0x4U)

#define EMMC_MODE_HS400          (0x7U)

/*
 * CV22 SD Controller: Auto CMD12 Error Status Register
 */
typedef struct {
    UINT32  ACmd12NotExe:           1;      /* [0] Auto CMD12 not Executed */
    UINT32  ACmd12TimeoutErr:       1;      /* [1] */
    UINT32  ACmd12CrcErr:           1;      /* [2] */
    UINT32  ACmd12EndBitErr:        1;      /* [3] */
    UINT32  ACmd12IndexErr:         1;      /* [4] */
    UINT32  ACmd12RespErr:          1;      /* [5] */
    UINT32  Reserved:               1;      /* [6] */
    UINT32  CmdNotIssuedErr:        1;      /* [7] Not Issued By Auto CMD12 Error */
    UINT32  Reserved2:              8;      /* [15:8] */
    UINT32  UHSModeSelect:          3;      /* [18:16] */
    UINT32  Enable18VoltSignaling:  1;      /* [19] 1.8 Volt signaling enable*/
    UINT32  DriverStrengthSelect:   2;      /* [21:20] 0x0 - Driver TYPEB, 0x1 - Driver TYPEA , 0x2 - Driver TYPEC, 0x3 - Driver TYPED */
    UINT32  ExecuteTuning:          1;      /* [22] 1 - Execute Tuning */
    UINT32  SampleClockSelect:      1;      /* [23] 1 - Tuned clock is used to sample data, 0 - Fixed clock is used to sample data */
    UINT32  Reserved8:              1;      /* [24] */
    UINT32  Reserved9:              1;      /* [25] */
    UINT32  Adma2LenMode:           1;      /* [26] 1 - selects ADMA2 Length mode to 26-bit, 0 - 16-bit */
    UINT32  Cmd23Enable:            1;      /* [27] 1 - Auto CMD23 is enabled */
    UINT32  HostVer4Enable:         1;      /* [28] 1- Version 4 mode */
    UINT32  Address64Bit:           1;      /* [29] 1- 64-bit Addressing, 0 - 32 bits addressing */
    UINT32  AsyncIntrEnable:        1;      /* [30] Asynchronous Interrupt Enable */
    UINT32  PresetValEnable:        1;      /* [31] Preset Value Enable. Enables automatic selection of SDCLK frequency and Driver strength Preset Value registers */
} AMBA_SD_AUTO_CMD12_STATUS_REG_s;

#define AMBA_SD_HOST_CTRL2_REG_s AMBA_SD_AUTO_CMD12_STATUS_REG_s

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
        UINT32  Emmc8BitSupport:        1;      /* [18]: 8-bit Support for Embedded Device */
        UINT32  AdmaSupport:            1;      /* [19]: ADM Support */
        UINT32  Reserved3:              1;      /* [20]: Reserved */
        UINT32  HighSpeedSupport:       1;      /* [21]: 1 - High Speed Support */
        UINT32  DmaSupport:             1;      /* [22]: 1 - DMA Support */
        UINT32  SuspendResumeSupport:   1;      /* [23]: 1 Suspend / Resume Support */
        UINT32  Voltage3D3vSupport:     1;      /* [24]: 1 - Voltage Support 3.3V */
        UINT32  Voltage3D0vSupport:     1;      /* [25]: 1 - Voltage Support 3.0V */
        UINT32  Voltage1D8vSupport:     1;      /* [26]: 1 - Voltage Support 1.8V */
        UINT32  SystemAddr64SupportV4:  1;      /* [27]: 64-bit System Address Support for V4 */
        UINT32  SystemAddr64SupportV3:  1;      /* [28]: 64-bit System Address Support for V3 */
        UINT32  ASyncIntSupport:        1;      /* [29]: Asynchronous Interrupt Support (SD Mode only) */
        UINT32  SlotType:               2;      /* [31:30]: Slot Type. These bits indicate usage of a slot by a specific Host System. */
    } Bits;
} AMBA_SD_CAPABILITY_REG_u;

#define RE_TUNING_MODE1 (0x0U) /* Timer */
#define RE_TUNING_MODE2 (0x1U) /* Timer and Re-Tuning Request (Notsupported) */
#define RE_TUNING_MODE3 (0x2U) /* Auto Re-Tuning (for transfer) */

typedef struct {
    UINT32  Sdr50Support:           1;      /* [0]: SDR50 Support (UHS-I only) */
    UINT32  Sdr104Support:          1;      /* [1]: SDR104 Support (UHS-I only) */
    UINT32  Ddr50Support:           1;      /* [2]: DDR50 Support (UHS-I only) */
    UINT32  Uhs2Support:            1;      /* [3]: UHS-II Support (UHS-II only) */
    UINT32  DriverTypeASupport:     1;      /* [4]: Driver Type A Support (UHS-I only) */
    UINT32  DriverTypeCSupport:     1;      /* [5]: Driver Type C Support (UHS-I only) */
    UINT32  DriverTypeDSupport:     1;      /* [6]: Driver Type D Support (UHS-I only) */
    UINT32  Reserved0:              1;      /* [7]: Reserved */
    UINT32  RetuneCount:            4;      /* [11:8]: Timer Count for Re-Tuning (UHS-I only) */
    UINT32  Reserved1:              1;      /* [12]: Reserved */
    UINT32  UseTuningSdr50:         1;      /* [13]: 1 - SDR50 requires tuning) */
    UINT32  ReTuningModes:          2;      /* [15:14]: Re-Tuning Modes (UHS-I only). Select the re-tuning method and limit the maximum data length. */
    UINT32  ClockMultiplier:        8;      /* [23:16]: Clock Multiplier */
    UINT32  Reserved2:              3;      /* [26:24]: Reserved */
    UINT32  Adma3Support:           1;      /* [27]: ADMA3 Support */
    UINT32  Vdd218vSupport:         1;      /* [28]: 1.8V VDD2 Support */
    UINT32  Reserved3:              3;      /* [31:29]: Reserved */
} AMBA_SD_CAPABILITY_REG2_s;

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

typedef struct {
    UINT32  Reserved:               32;     /* [31:0]: Reserved */
} AMBA_SD_MAX_CURRENT_REG2_s;


typedef struct {
    UINT32  ForceACmd12NotExe:           1;      /* [0] Auto CMD12 not Executed */
    UINT32  ForceACmd12TimeoutErr:       1;      /* [1] */
    UINT32  ForceACmd12CrcErr:           1;      /* [2] */
    UINT32  ForceACmd12EndBitErr:        1;      /* [3] */
    UINT32  ForceACmd12IndexErr:         1;      /* [4] */
    UINT32  ForceACmd12RespErr:          1;      /* [5] */
    UINT32  Reserved:                    1;      /* [6] */
    UINT32  ForceCmdNotIssuedErr:        1;      /* [7] Not Issued By Auto CMD12 Error */
    UINT32  Reserved2:                   8;      /* [15:8] */
    UINT32  ForceCmdTimeOutErr:          1;      /* [16] */
    UINT32  ForceCmdCrcErr:              1;      /* [17] */
    UINT32  ForceCmdEndBitErr:           1;      /* [18] */
    UINT32  ForceCmdIndexErr:            1;      /* [19] */
    UINT32  ForceCmdDataTimeoutErr:      1;      /* [20] */
    UINT32  ForceCmdDataCrcErr:          1;      /* [21] */
    UINT32  ForceCmdDataEndBitErr:       1;      /* [22] */
    UINT32  ForceCmdCurrentLimitErr:     1;      /* [23] */
    UINT32  ForceAutoCmdErr:             1;      /* [24] */
    UINT32  ForceAdmaErr:                1;      /* [25] */
    UINT32  ForceTuningErr:              1;      /* [26] */
    UINT32  ForceRespErr:                1;      /* [27] */
    UINT32  ForceBootAckErr:             1;      /* [28] */
    UINT32  ForceVendorErr1:             1;      /* [29] */
    UINT32  ForceVendorErr2:             1;      /* [30] */
    UINT32  ForceVendorErr3:             1;      /* [31] */
} AMBA_SD_FORCE_STATUS_REG_s;

/*
 * CV22 SD Controller: Advanced DMA (ADMA) Error Status Register
 */
typedef union {
    UINT32  Data;

    struct {
        UINT32  AdmaErrorState:         2;      /* [1:0]: ADMA Error State */
        UINT32  AdmaLengthError:        1;      /* [2]: 1 - ADMA Length Mismatch Error */
        UINT32  Reserved:              29;     /* [31:3]: Reserved */
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

typedef struct {
    UINT32  FreqSelectValInit:    10;     /* [9:0]: Reserved */
    UINT32  ClkGenSelectValInit:  1;      /* [10]: 1 - Programmable Clock Generator; 0 - Host Ver2.0 Clock Generator */
    UINT32  Reserved0:            3;      /* [13:11]: Reserved */
    UINT32  DriverSelValInit:     2;      /* [15:14]: Driver Strength Select Value */
    UINT32  FreqSelectVal:        10;     /* [25:16]: Reserved */
    UINT32  ClkGenSelectVal:      1;      /* [26]: 1 - Programmable Clock Generator; 0 - Host Ver2.0 Clock Generator */
    UINT32  Reserved1:            3;      /* [29:27]: Reserved */
    UINT32  DriverSelVal:         2;      /* [31:30]: Driver Strength Select Value */
} AMBA_SD_PRESET_INIT_DS_REG_s;


typedef struct {
    UINT32  FreqSelectValHs:      10;     /* [9:0]  : Reserved */
    UINT32  ClkGenSelectValHs:    1;      /* [10]   : 1 - Programmable Clock Generator; 0 - Host Ver2.0 Clock Generator */
    UINT32  Reserved0:            3;      /* [13:11]: Reserved */
    UINT32  DriverSelValHs:       2;      /* [15:14]: Driver Strength Select Value */
    UINT32  FreqSelectValSdr12:   10;     /* [25:16]: Reserved */
    UINT32  ClkGenSelectValSdr12: 1;      /* [26]   : 1 - Programmable Clock Generator; 0 - Host Ver2.0 Clock Generator */
    UINT32  Reserved1:            3;      /* [29:27]: Reserved */
    UINT32  DriverSelValSdr12:    2;      /* [31:30]: Driver Strength Select Value */
} AMBA_SD_PRESET_HS_SDR12_REG_s;

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
    volatile AMBA_SD_AUTO_CMD12_STATUS_REG_s    AutoCmd12Status;    /* 0x03C(RW): Auto CMD12 error status & Host control2 register */
    volatile AMBA_SD_CAPABILITY_REG_u           Capability;         /* 0x040(R): Capabilities register */
    volatile AMBA_SD_CAPABILITY_REG2_s          Capability2;        /* 0x044(R): Capabilities2 register */
    volatile AMBA_SD_MAX_CURRENT_REG_u          MaxCurrent;         /* 0x048(R): Maximum current capabilities */
    volatile AMBA_SD_MAX_CURRENT_REG2_s         MaxCurrent2;        /* 0x04C(R): Maximum current capabilities2 */
    volatile AMBA_SD_FORCE_STATUS_REG_s         ForceStatus;        /* 0x050: */
    volatile AMBA_SD_ADMA_STATUS_REG_u          AdmaStatus;         /* 0x054(R): Advanced DMA (ADMA) error status */
    volatile UINT32                             AdmaAddr;           /* 0x058(RW): Advanced DMA (ADMA) system address */
    volatile UINT32                             AdmaAddrHigh;       /* 0x05C(RW): Advanced DMA (ADMA) system address High */

    volatile AMBA_SD_PRESET_INIT_DS_REG_s       PresetInitDs;       /* 0x060(R): Preset Value for Initialization and Default Speed mode */
    volatile AMBA_SD_PRESET_HS_SDR12_REG_s      PresetHsSdr12;      /* 0x064(R): Preset Value for High Speed mode and SDR12 mode */
    volatile UINT32                             PresetSdr25Sdr50;   /* 0x068(R): Preset Value for SDR25 mode and SDR50 mode */
    volatile UINT32                             PresetSdr104ddr50;  /* 0x06C(R): Preset Value for SDR104 mode and DDR50 mode */

    volatile AMBA_SD_BOOT_CTRL_REG_u            BootCtrl;           /* 0x070(RW): Boot Control (SD/MMC boot) */
    volatile AMBA_SD_BOOT_STATUS_REG_u          BootStatus;         /* 0x074(RW): Boot Status (SD/MMC boot) */

    volatile UINT32                             AdmaID;             /* 0x078:  */
    volatile UINT32                             AdmaIDHigh;         /* 0x07C:  */
    volatile UINT32                             Reserved4[26];      /* 0x080 - 0x0E4: Reserved */
    volatile UINT32                             VendorAreaAddr;     /* 0x0E8(R): Pointer to vendor Specific Area */
    volatile UINT32                             Reserved5[4];       /* 0x0EC - 0x0F8: Reserved */
    volatile AMBA_SD_SLOT_IRQ_VER_REG_u         SlotIntHostVerReg;  /* 0x0FC:  */
} AMBA_SD_REG_s;

#endif /* _AMBA_REG_SD_H_ */
