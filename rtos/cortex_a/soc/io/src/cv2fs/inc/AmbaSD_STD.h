/**
 *  @file AmbaSD_STD.h
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
 *  @details Definitions & Constants for SD Specifications v3.0
 *
 */

#ifndef AMBA_SD_STD_H
#define AMBA_SD_STD_H

/*
 * SD information CID (Card identification number, Width = 128 bits) Register, Mandatory
 */
typedef struct {
    UINT32   NotUsed:            1;  /* [0]: Not used, always 1 */
    UINT32   Crc7Checksum:       7;  /* [7:1]: CRC (CRC7 checksum) */
    UINT8   ManufacturingMonth;     /* [11:8]: MDT-Month(Manufacturing date) */
    UINT8   ManufacturingYear;      /* [19:12]: MDT-Year (Manufacturing year, 0 = 2000) */
    UINT32   Reserved0:          4;  /* [23:20]: Reserved */
    UINT32  ProductSerialNo;        /* [55:24]: PSN (Product serial number) */
    UINT8   ProductRevision;        /* [63:56]: PRV (Product revision) */
    UINT8   ProductName[5];         /* [103:64]: PNM (Product name) */
    UINT16  OemID;                  /* [119:104]: OID (OEM/Application ID) */
    UINT8   ManufacturerID;         /* [127:120]: MID (Manufacturer ID) */
} AMBA_SD_INFO_CID_REG_s;

/*
 * SD information RCA (Relative card address, Width = 16 bits) Register, Mandatory
 */

/*
 * SD information DSR (Driver Stage, Width = 16 bits) Register, Optional
 */

/*
 * SD information CSD (Card Specific Data, Width = 128 bits) Register, Mandatory
 */
typedef struct {
    UINT32   NotUsed:            1;  /* [0]: Not used, always 1 */
    UINT32   Crc:                7;  /* [7:1]: CRC */
    UINT32   Reserved5:          2;  /* [9:8]: Reserved */
    UINT32   FileFormat:         2;  /* [11:10]: file format */
    UINT32   TempWriteProtect:   1;  /* [12]: Temporary write protection */
    UINT32   PermWriteProtect:   1;  /* [13]: Permanent write protection */
    UINT32   Copy:               1;  /* [14]: Copy flag */
    UINT32   FileFormatGroup:    1;  /* [15]: File format group */
    UINT32   Reserved4:          5;  /* [20:16]: Reserved */
    UINT32   WriteBlockPartial:  1;  /* [21]: partial blocks for write allowed */
    UINT32   WirteBlockLength:   4;  /* [25:22]: Max. write data block length */
    UINT32   WriteSpeedFactor:   3;  /* [28:26]: Write speed factor */
    UINT32   Rserved3:           2;  /* [30:29]: Reserved */
    UINT32   WirteGroupEnable:   1;  /* [31]: Write protect group enable */
    UINT32   WriteGroupSize:     7;  /* [38:32]: Write protect group size */
    UINT32   SectorSize:         7;  /* [45:39]: Erase sector size */
    UINT32   EraseBlockEnable:   1;  /* [46]: Erase single block enable */

    union {
        struct {
            UINT32   DeviceSizeMult:     3;  /* [49:47]: device size multiplier  */
            UINT32   MaxWriteCurrentMax: 3;  /* [52:50]: max. write current @VDD max */
            UINT32   MaxWriteCurrentMin: 3;  /* [55:53]: max. write current @VDD min  */
            UINT32   MaxReadCurrentMax:  3;  /* [58:56]: max. read current @VDD max */
            UINT32   MaxReadCurrentMin:  3;  /* [61:59]: max. read current @VDD min */
            UINT32  DeviceSize:         12; /* [73:62]: Device size */
            UINT32   Reserved1:          2;  /* [75:74]: Reserved */
        } Version1;
        struct {
            UINT32   Reserved2:          1;  /* [47]: Reserved */
            UINT32  DeviceSize:         22; /* [69:48]: Device size */
            UINT32   Reserved1:          6;  /* [75:70]: Reserved */
        } Version2;
    } CsdVersion;

    UINT32   DsrImplemented:     1;  /* [76]: DSR implemented */
    UINT32   ReadBlockMisalign:  1;  /* [77]: Read block misalignment */
    UINT32   WriteBlockMisalign: 1;  /* [78]: Write block misalignment */
    UINT32   ReadBlockPartial:   1;  /* [79]: Partial blocks for read allowed */
    UINT32   ReadBlockLength:    4;  /* [83:80]: Max. read data block length */
    UINT32  CardCommandClass:   12; /* [95:84]: Card command classes */
    UINT8   TranSpeed;              /* [103:96]: Max. data transfer rate */
    UINT8   Nsac;                   /* [111:104]: Data read access-time in CLK cycles (NSAC*100) */
    UINT8   Taac;                   /* [119:112]: Data read access-time */
    UINT32   Reserved0:          6;  /* [125:120]: Reserved */
    UINT32   CsdStructure:       2;  /* [127:126]: CSD structure */
} AMBA_SD_INFO_CSD_REG_s;

/*
 * SD information SCR (SD Configuration, Width = 64 bits) Register, Mandatory
 */
typedef struct {
    UINT32  Reserved1;              /* [31:0]: Reserved for manufacturer usage */
    UINT32   CmdSupport:         4;  /* [35:32]: Command Support bits */
    UINT32   Reserved0:          6;  /* [41:36]: Reserved */
    UINT32   SdSpec4:            1;  /* [42]: Spec. Version 4.00 or higher */
    UINT32   ExSecurity:         4;  /* [46:43]: Extended Security Support */
    UINT32   SdSpec3:            1;  /* [47]: Spec. Version 3.00 or higher */
    UINT32   SdBusWidths:        4;  /* [51:48]: DAT Bus widths supported */
    UINT32   SdSecurity:         3;  /* [54:52]: CPRM Security Support */
    UINT32   DataStatAfterErase: 1;  /* [55]: Data_status_after erases */
    UINT32   SdSpec:             4;  /* [59:56]: SD Memory Card - Spec. Version */
    UINT32   ScrStructure:       4;  /* [63:60]: SCR structure */
} AMBA_SD_INFO_SCR_REG_s;

/*
 * SD information OCR (Operation conditions, Width = 32 bits) Register, Mandatory
 */
typedef struct {
    UINT32   Reserved0:          4;  /* [0:3]: Reserved */
    UINT32   Reserved1:          1;  /* [4]: Reserved */
    UINT32   Reserved2:          1;  /* [5]: Reserved */
    UINT32   Reserved3:          1;  /* [6]: Reserved */
    UINT32   Reserved4:          1;  /* [7]: Reserved for Low Voltage Range */
    UINT32   Reserved5:          1;  /* [8]: Reserved */
    UINT32   Reserved6:          1;  /* [9]: Reserved */
    UINT32   Reserved7:          1;  /* [10]: Reserved */
    UINT32   Reserved8:          1;  /* [11]: Reserved */
    UINT32   Reserved9:          1;  /* [12]: Reserved */
    UINT32   Reserved10:         1;  /* [13]: Reserved */
    UINT32   Reserved11:         1;  /* [14]: Reserved */
    UINT32   Vdd27:              1;  /* [15]: VDD 2.7-2.8 */
    UINT32   Vdd28:              1;  /* [16]: VDD 2.8-2.9 */
    UINT32   Vdd29:              1;  /* [17]: VDD 2.9-3.0 */
    UINT32   Vdd30:              1;  /* [18]: VDD 3.0-3.1 */
    UINT32   Vdd31:              1;  /* [19]: VDD 3.1-3.2 */
    UINT32   Vdd32:              1;  /* [20]: VDD 3.2-3.3 */
    UINT32   Vdd33:              1;  /* [21]: VDD 3.3-3.4 */
    UINT32   Vdd34:              1;  /* [22]: VDD 3.4-3.5 */
    UINT32   Vdd35:              1;  /* [23]: VDD 3.5-3.6 */
    UINT32   S18a:               1;  /* [24]: Switching to 1.8V Accepted */
    UINT32   Reserved12:         4;  /* [25:28]: reserved */
    UINT32   Uhs2CardStatus:     1;  /* [29]: UHS-II Card Status */
    UINT32   Ccs:                1;  /* [30]: Card Capacity Status */
    UINT32   Busy:               1;  /* [31]: Card power up status bit */
} AMBA_SD_INFO_OCR_REG_s;

/*
 * SD information SSR (SD Status, Width = 512 bits) Register, Mandatory
 */
typedef struct {
    UINT32   DatBusWidth:        2;      /* [511:502]: Data bus width by ACMD6 &  */
    UINT32   SecureMode:         1;      /* [509]: Card is in Secured Mode of operation */
    UINT32   RsvSecureFunc:      7;      /* [508:502]: Reserved for Security Functions */
    UINT32   Rsv0:               6;      /* [501:496]: Reserved */
    UINT16  SdCardType;                 /* [495:480]: Type of SD memory card */
    UINT32  SizeOFProtectedArea;        /* [479:448]: Size of protected area */
    UINT8   SpeedClass;                 /* [447:440]: Speed Class of the card */
    UINT8   PerformanceMove;            /* [439:432]: Performance of move indicated by 1 [MB/s] step */
    UINT32   AuSize:             4;      /* [431:428]: Size of AU */
    UINT32   Rsv1:               4;      /* [427:424]: Reserved */
    UINT16  EraseSize;                  /* [423:408]: Number of AUs to be erased at a time */
    UINT32   EraseTimeout:       6;      /* [407:402]: Timeout value for erasing areas */
    UINT32   EraseOffset:        2;      /* [401:400]: Fixed offset value added to erase time */
    UINT32   UhsSpeedGrade:      4;      /* [399:396]: Speed Grade for UHS mode */
    UINT32   UhsAuSize:          4;      /* [395:392]: Size of AU for UHS mode */
    UINT32   Rsv[49];                    /* [391:0]: Reserved */
} AMBA_SD_INFO_SSR_REG_s;

/*
 * SD information CSR (Card Status, Width = 32 bits) Register, Mandatory
 */
typedef struct {
    UINT32  Reserve5:            2;  /* [1:0]: Reserved for manufacturer test mode */
    UINT32  Reserve4:            1;  /* [2]: Reserved for application specific commands */
    UINT32  AkeSeqError:         1;  /* [3]: Error in the sequence of the authentication process */
    UINT32  Reserve3:            1;  /* [4]: Reserved for SD I/O Card */
    UINT32  AppCmd:              1;  /* [5]: An indication that the command has been interpreted as ACMD */
    UINT32  Reserve2:            2;  /* [7:6]: Reserved */
    UINT32  ReadyForData:        1;  /* [8]: Corresponds to buffer empty signaling on the bus */
    UINT32  CurrentState:        4;  /* [12:9]: The state of the card when receiving the command */
    UINT32  EraseReset:          1;  /* [13]: Erase sequence */
    UINT32  CardEccDisabled:     1;  /* [14]: The command has been executed without using the internal ECC */
    UINT32  WpEraseSkip:         1;  /* [15]: Set when only partial address space was erased */
    UINT32  CsdOverwrite:        1;  /* [16]: The read only section of the CSD does not match the card content or reverse the copy */
    UINT32  Reserved1:           1;  /* [17]: Reserved for DEFERRED_RESPONSE */
    UINT32  Reserved0:           1;  /* [18]: Reserved */
    UINT32  Error:               1;  /* [19]: A general or an unknown error occurred during the operation */
    UINT32  CcError:             1;  /* [20]: Internal card controller error */
    UINT32  CardEccError:        1;  /* [21]: Card internal ECC was applied but failed to correct the data */
    UINT32  IllegalCommand:      1;  /* [22]: Command not legal for the card state */
    UINT32  ComCrcError:         1;  /* [23]: The CRC check of the previous command failed */
    UINT32  LockUnlockFailed:    1;  /* [24]: Set when a sequence or password error has been detected in lock/unlock card command */
    UINT32  CardIsLocked:        1;  /* [25]: signals that the card is locked by the host */
    UINT32  WpViolation:         1;  /* [26]: Set when the host attempts to write to a protected block */
    UINT32  EraseParam:          1;  /* [27]: An invalid selection of write-blocks for erase occurred */
    UINT32  EraseSeqError:       1;  /* [28]: An error in the sequence of erase commands occurred */
    UINT32  BlockLenError:       1;  /* [29]: The transferred block length is not allowed for this card */
    UINT32  AddressError:        1;  /* [30]: A misaligned address which did not match the block length was used in the command */
    UINT32  OutOfRange:          1;  /* [31]: The command's argument was out of the allowed range for this card */
} AMBA_SD_INFO_CSR_REG_s;

typedef struct {
    /* Modes segment */
    struct {
        UINT8   Reserved0[16];              /* [15:0]: Reserved */
        UINT8   SecureRemovalType;          /* [16]: Secure Removal Type */
        UINT8   ProductStateAwarenessEnablement;  /* 17: Product state awareness enablement */
        UINT32  MaxPreLoadingDataSize;      /* 18-21: Max pre loading data size */
        UINT32  PreLoadingDataSize;         /* 22-25: Pre loading data size */
        UINT8   FfuStatus;                  /* 26: FFU status */
        UINT8   Reserved1[2];               /* 27-28: Reserved */
        UINT8   ModeOperationCodes;         /* 29: Mode operation codes*/
        UINT8   ModeConfig;                 /* 30: Mode config */
        UINT8   Reserved2;                  /* 31: Reserved */
        UINT8   FlushCache;                 /* 32: Flushing of the cache */
        UINT8   CacheCtrl;                  /* 33: Control to turn the Cache ON/OFF */
        UINT8   PowerOffNotification;       /* 34: Power Off Notification */
        UINT8   PackedFailureIndex;         /* 35: Packed command failure index */
        UINT8   PackedCommandStatus;        /* 36: Packed command status */
        UINT8   ContextConfig[15];          /* 37-51: Context configuration */
        UINT8   ExtPartitionsAttribute;     /* 52-53: Extended Partitions Attribute */
        UINT8   ExceptionEventStatus;       /* 54-55: Exception events status */
        UINT8   ExceptionEventCrtl;         /* 56-57: Exception events control */
        UINT8   DyncapNeeded;               /* 58: Number of addressed group to be Released*/
        UINT8   Class6CmdCtrl;              /* 59: Class 6 commands control */
        UINT8   LnitilaTimeoutEmu;          /* 60: 1st initialization after disabling sector size emulation */
        UINT8   DataSectorSize;             /* 61: Sector size */
        UINT8   UseNativeSector;            /* 62: Sector size emulation */
        UINT8   NativeSectorSize;           /* 63: Native sector size */
        UINT8   VenderSpecificField[64];    /* 64-127: Vendor Specific Fields */
        UINT8   Reserved3[2];               /* 128-129: Reserved */
        UINT8   ProgramCidCsdDdrSupport;    /* 130: Program CID/CSD in DDR mode support */
        UINT8   PeriodicWakeup;             /* 131: Periodic Wake-up */
        UINT8   TemperatureCaseSupport;     /* 132: Package Case Temperature is controlled */
        UINT8   ProductionStateAwareness;   /* 133: Production state awareness */
        UINT8   SecBadBlkMagementMode;      /* 134: Bad Block Management mode */
        UINT8   Reserved4;                  /* 135: Reserved */
        UINT32  EnhancedStartAddr;          /* 136-139: Enhanced User Data Start Address */
        UINT8   EnhancedSizeMult[3];        /* 140-142: Enhanced User Data Area Size */
        UINT8   GeneralPurposeSizeMult[12]; /* 143-154: General Purpose Partition Size */

        UINT8   PartitionSettingCompleted;  /* 155: Paritioning Setting */
        UINT8   PartitionsAttribute;        /* 156: Partitions attribute */
        UINT8   MaxEnhancedAreaSize[3];     /* 157-159: Max Enhanced Area Size */
        UINT8   PartitioningSupport;        /* 160: Partitioning Support */
        UINT8   HpiManagment;               /* 161: HPI management */
        UINT8   HwRstFunction;              /* 162: H/W reset function */
#define MMC_RST_N_TEMP_DISABLE 0x0
#define MMC_RST_N_PERM_ENABLE 0x1
#define MMC_RST_N_PERM_DISABLE 0x2
        UINT8   bkops_en;             /* 163: Enable background operations handshake */
        UINT8   bkops_start;          /* 164: Manually start background operations */
        UINT8   SanitizeStart;        /* 165: Start Sanitize operation */
        UINT8   WriteRelParam;        /* 166: Write reliability parameter register */
        UINT8   WriteRelSetting;      /* 167: Write reliability setting register */
        UINT8   RpmbSizeMult;         /* 168: RPMB Size */
        UINT8   FwConfig;             /* 169: FW configuration */
        UINT8   Reserved5;            /* 170: Reserved */
        UINT8   UserWp;               /* 171: User area write protection register */
        UINT8   Reserved6;            /* 172: Reserved */
        UINT8   BootWp;               /* 173: Boot area write protection register */
        UINT8   BootWpStatus;         /* 174: Boot  write protection register */
        UINT8   EraseGroupDefinition; /* 175: High-density erase group definition */

        UINT8   Reserved7;            /* 176: Reserved */
        UINT8   BootBusConditions;    /* 177: Boot bus conditions (Boot bus width1 for MMC 4.41) */
        UINT8   BootConfigProtection; /* 178: Boot config protection */
        UINT8   BootConfig;           /* 179: Partition configuration */

        UINT8   Reserved8;            /* 180: Reserved */
        UINT8   ErasedMemContent;     /* 181: Erased memory content */
        UINT8   Reserved9;            /* 182: Reserved */
        UINT8   BusWidth;             /* 183: Bus width mode */
#define MMC_DATA_BUS_1_bit 0
#define MMC_DATA_BUS_4_bit 1
#define MMC_DATA_BUS_8_bit 2
#define MMC_DATA_BUS_DDR_4_bit 5
#define MMC_DATA_BUS_DDR_8_bit 6
        UINT8   Reserved10;         /* 184: Reserved */
        UINT8   HighspeedTiming;    /* 185: High-speed interface timing */
        UINT8   Reserved11;         /* 186: Reserved */
        UINT8   PowerClass;         /* 187: Power class */
        UINT8   Reserved12;         /* 188: Reserved */
        UINT8   CmdSetRevision;     /* 189: Command set revision */
        UINT8   Reserved13;         /* 190: Reserved */
        UINT8   Cmdset;             /* 191: Command set */
    } Modes;

    /* 196: Device type (Card type for MMC 4.41) */
#define HIGH_SPEED_MMC_26MHZ        0x1U
#define HIGH_SPEED_MMC_52MHZ        0x2U
#define HIGH_SPEED_MMC_200MHZ_1_8V  0x10U
#define HIGH_SPEED_MMC_200MHZ_1_2V  0x20U
#define HIGH_SPEED_MMC_200MHZ       0x30U
#define HIGH_SPEED_MMC_400MHZ_1_2V  0x40U
#define HIGH_SPEED_MMC_400MHZ_1_8V  0x80U

    /* Properties segment */
    struct {
        UINT8   ExtcsdRevision;    /* 192: Extended CSD revision */
#define EXT_CSD_REV_1_7 0x7 /* for MMC v5.0 */
#define EXT_CSD_REV_1_6 0x6 /* for MMC v4.5, v4.51 */
#define EXT_CSD_REV_1_5 0x5 /* for MMC v4.41 */
#define EXT_CSD_REV_1_3 0x3 /* for MMC v4.3 */
#define EXT_CSD_REV_1_2 0x2 /* for MMC v4.2 */
#define EXT_CSD_REV_1_1 0x1 /* for MMC v4.1 */
#define EXT_CSD_REV_1_0 0x0 /* for MMC v4.0 */
        UINT8   Reserved0;     /* 193: Reserved */
        UINT8   CsdStructure;  /* 194: CSD structure version */
        UINT8   Reserved1;     /* 195: Reserved */
        UINT8   DeviceType;    /* 196: Device type (Card type for MMC 4.41) */
        UINT8   DriverStrength;            /* 197: I/O driver strength */
#define EMMC_DRV_STG_TYPE_0_SUPPORT   0x1U   /* The Default value, 50 ohm */
#define EMMC_DRV_STG_TYPE_1_SUPPORT   0x2U   /* 33 ohm Driving capability x 1.5 */
#define EMMC_DRV_STG_TYPE_2_SUPPORT   0x4U   /* 66 ohm Driving capability x 0.75 */
#define EMMC_DRV_STG_TYPE_3_SUPPORT   0x8U   /* 100 ohm Driving capability x 0.5 */
#define EMMC_DRV_STG_TYPE_4_SUPPORT   0x10U  /* 44 ohm Driving capability x 1.2 */
        UINT8   OutOfInterruptTime;        /* 198: out-of-interrupt busy time */
        UINT8   PartitionSwitchTime;       /* 199: partition switching timing */
        UINT8   PowerClass52_195;          /* 200: Power class for 52Mhz at 1.95V 1 R */
        UINT8   PowerClass26_195;          /* 201: Power class for 26Mhz at 1.95V 1 R */
        UINT8   PowerClass52_360;          /* 202: Power class for 52Mhz at 3.6V 1 R */
        UINT8   PowerClass26_360;          /* 203: Power class for 26Mhz at 3.6V 1 R */
        UINT8   Reserved2;                 /* 204: Reserved */
        UINT8   MinPerfRead_4_26;          /* 205: Minimum Read Performance for 4bit at 26MHz */
        UINT8   MinPerfWrite_4_26;         /* 206: Minimum Write Performance for 4bit at 26MHz */
        UINT8   MinPerfRead_8_26_4_52;     /* 207: Minimum Read Performance for 8bit at 26MHz, for 4bit at 52MHz */
        UINT8   MinPerfWrite_8_26_4_52;    /* 208: Minimum Write Performance for 8bit at 26MHz, for 4bit at 52MHz */
        UINT8   MinPerfRead_8_52;          /* 209: Minimum Read Performance for 8bit at 52MHz */
        UINT8   MinPerfWrite_8_52;         /* 210: Minimum Write Performance for 8bit at 52MHz */
        UINT8   Reserved3;                 /* 211: Reserved */
        UINT32  SecCount;                  /* 212-215: Sector Count */
        UINT8   Reserved4;                 /* 216: Reserved */
        UINT8   SleepAwakeTimeout;         /* 217: Sleep/awake timeout */
        UINT8   ProductionStateAwarenessTimeout; /* 218: Production state awareness timeout */
        UINT8   SleepCurrentVccq;          /* 219: Sleep current (VCCQ)  */
        UINT8   SleepCurrentVcc;           /* 220: Sleep current (VCC)  */
        UINT8   HcWpGrpSize;               /* 221: High-capacity write protect group size  */
        UINT8   RelWpSecCnt;               /* 222: Reliable write sector count */
        UINT8   EraseTimeoutMult;          /* 223: High-capacity erase timeout */
        UINT8   HighCapacityErasegrpSize;  /* 224:High-capacity erase unit size */
        UINT8   AccessSize;                /* 225: Access size */
        UINT8   BootSizeMult;              /* 226: Boot partition size */
        UINT8   Reserved5;                 /* 227: Reserved */
        UINT8   BootInfo;                  /* 228: Boot information */
#define MMC_BOOT_ALT 0x1    /* Support alternative boot method (from MMC 4.4) */
#define MMC_BOOT_DDR 0x5    /* Support dual data rate during boot */
#define MMC_BOOT_HS 0x7     /* Support high speed timing during boot */
        UINT8   SecureTrimMult;            /* 229: Secure TRIM Multiplier */
        UINT8   SecureEraseMult;           /* 230: Secure Erase Multiplier */
        UINT8   SecureFeatureSupport;      /* 231: Secure Feature support */
        UINT8   TrimMult;                  /* 232: TRIM Multiplier */
        UINT8   Reserved6;                 /* 233: Reserved */

        UINT8   Minpref_ddr_r_8_52;        /* 234: Minimum Read Performance for 8bit at 52MHz in DDR mode */
        UINT8   Minpref_ddr_w_8_52;        /* 235: Minimum Write Performance for 8bit at 52MHz in DDR mode */
        UINT8   PowerClass200_130;         /* 236: Power class for 200MHz, at VCCQ = 1.3V, VCC = 3.6V */
        UINT8   PowerClass200_195;         /* 237: Power class for 200MHz, at VCCQ = 1.95V, VCC = 3.6V */
        UINT8   PowerClassDdr_52_195;      /* 238: Power class for 52MHz, DDR at 1.95V */
        UINT8   PowerClassDdr_52_360;      /* 239: Power class for 52MHz, DDR at 3.6V */
        UINT8   Reserved7;                 /* 240: Reserved */
        UINT8   InitialTimeoutAp;          /* 241: 1st initialization time after partitioning */
        UINT8   CorrectlyPrgSectorsNum[4]; /* 242-245: Number of correctly programmed sectors */
        UINT8   BkOpsStatus;               /* 246: Background operations status */
        UINT8   PowerOffLongTime;          /* 247: Power of notification(long) time */
        UINT8   GenericCmd6Time;           /* 248: Generic CMD6 timeout */
        UINT8   CacheSize[4];              /* 249-252: Cache size */
        UINT8   PowerClassDdr_200_360;     /* 253: Power class for 200MHz,  DDR at VCC=3.6V */
        UINT8   FirmwareVersion[8];        /* 254-261: Firmware version */
        UINT16  DeviceVersion;             /* 262-263: Device version */
        UINT8   Optimaltrim_unit_size;     /* 264: Optimal trim unit size */
        UINT8   Optimalwrite_size;         /* 265: Optimal write size */
        UINT8   Optimalread_size;          /* 266: Optimal read size */
        UINT8   PreEolInfo;                /* 267: Pre EOL information */
        UINT8   DeviceLifeTimeEstTypeA;    /* 268: Device life time estimation type A */
        UINT8   DeviceLifeTimeEstTypeB;    /* 269: Device life time estimation type B */
        UINT8   VendorProprietaryHealthReport[32];   /* 270-301 : Vendor proprietary health report */
        UINT8   NumFwSecCorrectlyProgrammed[4];      /* 302-305 : Number of FW sectors correctly programmed */
        UINT8   Reserved8[181];         /* 306-486: Reserved */
        UINT8   FfuArg[4];             /* 487-490: FFU Argument */
        UINT8   OperationCodesTimeout; /* 491: Operation codes timeout */
        UINT8   FfuFeatures;           /* 492: FFU features */
        UINT8   SupportedModes;        /* 493: Supported modes */
        UINT8   ExtSupport;            /* 494: Extended partitions attribute support */
        UINT8   LargeUnitSizeM1;       /* 495: Large Unit size */
        UINT8   ContextCapabilities;   /* 496: Context management capabilities */
        UINT8   TagResSize;            /* 497: Tag Resources Size */
        UINT8   TagUnitSize;           /* 498: Tag Unit Size */
        UINT8   DataTagSupport;        /* 499: Data Tag Support */
        UINT8   MaxtagWrites;          /* 500: Max packed write commands */
        UINT8   MaxtagReads;           /* 501: Max packed read commands */
        UINT8   BackGroundOpsSupport;  /* 502: Background operations support */
        UINT8   HpiFeatures;           /* 503: HPI features */
        UINT8   SupportCmdSet;         /* 504: Supported Command Sets */
        UINT8   ExtsecurityCmdError;   /* 505: Extended Security Commands Error */
        UINT8   Reserved9[6];          /* 506-511: Reserved */
    } Properties;

} GNU_MIN_PADDING AMBA_SD_INFO_EXT_CSD_REG_s;

typedef struct {
    UINT32  SpportDirectCmd:         1;  /* [0] SDC:  Support Direct Commad during data transfer */
    UINT32  SupportMultiBlk:         1;  /* [1] SMB:  Support Mult-Block */
    UINT32  SupportReadWait:         1;  /* [2] SRW:  Support Read Wait */
    UINT32  SupportBusSuspend:       1;  /* [3] SBS:  Support Suspend/Resume , only apply to SD mode*/
    UINT32  Support4bitMultiBlkInt:  1;  /* [4] S4MI: Supports interrupt between blocks of data */
    UINT32  Enable4bitMultiBlkInt:   1;  /* [5] E4MI: Enable interrupt between blocks of data */
    UINT32  LowSpeedCard:            1;  /* [6] LSC:  1: Low-Speed SDIO card. 0: Full-Speed SDIO card */
    UINT32  LowSpeedSupport4Bit:     1;  /* [7] 4BLS: 4-bit support for Low-Speed cards. */
} GNU_MIN_PADDING AMBA_SD_IO_INFO_CAP_REG_s;

/************************************************/
/* CCCR (Card Common Control Registers).    */
/************************************************/
#define     AMBA_SD_CCCR_SDIO_REV_REG          0U
#define     AMBA_SD_SDIO_SPEC_REVISION         1U
#define    AMBA_SD_IO_ENABLE_REG               2U
#define    AMBA_SD_IO_READY_REG                3U
#define    AMBA_SD_INT_ENABLE_REG              4U
#define    AMBA_SD_INT_PENDING_REG             5U
#define    AMBA_SD_IO_ABORT_REG                6U
#define    AMBA_SD_BUS_INTERFACE_CONTROL_REG   7U
#define    AMBA_SD_CARD_CAPABILITY_REG         8U
#define    AMBA_SD_COMMON_CIS_POINTER_0_REG    9U
#define    AMBA_SD_COMMON_CIS_POINTER_1_REG    0xAU
#define    AMBA_SD_COMMON_CIS_POINTER_2_REG    0xBU
#define    AMBA_SD_BUS_SUSPEND_REG             0xCU
#define    AMBA_SD_FUNCTION_SELECT_REG         0xDU
#define    AMBA_SD_EXEC_FLAGS_REG              0xEU
#define    AMBA_SD_READY_FLAGS_REG             0xFU
#define    AMBA_SD_FN0_BLOCK_SIZE_0_REG        0x10U
#define    AMBA_SD_FN0_BLOCK_SIZE_1_REG        0x11U
#define    AMBA_SD_POWER_CONTROL_REG           0x12U
#define    AMBA_SD_HIGH_SPEED_REG              0x13U

#define SDIO_BUS_WIDTH_1_BIT (0U)
#define SDIO_BUS_WIDTH_4_BIT (2U)
#define CD_DISABLE           (0x80U)

#define SDIO_SUPPORT_HIGH_SPEED (0x1U)
#define SDIO_ENABLE_HIGH_SPEED  (0x2U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
 * SD All Information Registers
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct {
    AMBA_SD_INFO_CID_REG_s      CardID;                 /* CID (Card identification number, Width = 128 bits) */
    UINT16                      RelativeCardAddr;       /* RCA (Relative card address, Width = 16 bits) */
    UINT16                      DriverStage;            /* DSR (Driver Stage, Width = 16 bits) */
    AMBA_SD_INFO_CSD_REG_s      CardSpecificData;       /* CSD (Card Specific Data, Width = 128 bits) */
    AMBA_SD_INFO_SCR_REG_s      SdConfig;               /* SCR (SD Configuration, Width = 64 bits) */
    AMBA_SD_INFO_OCR_REG_s      OpCondition;            /* OCR (Operation conditions, Width = 32 bits) */
    AMBA_SD_INFO_SSR_REG_s      SdStatus;               /* SSR (SD Status, Width = 512 bits) */
    AMBA_SD_INFO_CSR_REG_s      CardStatus;             /* CSR (Card Status, Width = 32 bits) */
    AMBA_SD_INFO_EXT_CSD_REG_s  ExtCsd;             /* EXTCSD Extend Card Specific Data, Width = 512 bits) */
} AMBA_SD_INFO_REG_s;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
 * SD Commands Supported. All commands have a fixed code length of 48 bits.
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define    AMBA_SD_CMD0_GO_IDEL_STATE               0U    /* (bc): Resets all cards to idle state */
#define    AMBA_SD_CMD2_ALL_SEND_CID                2U    /* (bcr): Asks any card to send the CID numbers on the CMD line */
#define    AMBA_SD_CMD3_SEND_RELATIVE_ADDR          3U    /* (bcr): Asks the card to publish a new relative address (RCA) */
#define    AMBA_SD_CMD4_SET_DSR                     4U    /* (bc): Programs the DSR of all cards */
#define    AMBA_SD_CMD6_SWITCH_FUNC                 6U    /* (adtc): Checks switchable function (mode 0) and switch card function (mode 1) */
#define    AMBA_SD_CMD7_SELECT_DESELECT_CARD        7U    /* (ac): Toggles a card between the stand-by and transfer states or programming and disconnect states */
#define    AMBA_SD_CMD8_SEND_IF_COND                8U    /* (bcr): Sends SD Memory Card interface condition */
#define    AMBA_SD_CMD9_SEND_CSD                    9U    /* (ac): Addressed card sends its card-specific data (CSD) on the CMD line */
#define    AMBA_SD_CMD10_SEND_CID                   10U   /* (ac): Addressed card sends its card identification (CID) on the CMD line */
#define    AMBA_SD_CMD11_VOLTAGE_SWITCH             11U   /* (ac): Switch to 1.8V bus signal level */
#define    AMBA_SD_CMD12_STOP_TRANSMISSION          12U   /* (ac): Forces the card to stop transmission */
#define    AMBA_SD_CMD13_SEND_STATUS                13U   /* (ac): Addressed card sends its status register */
#define    AMBA_SD_CMD16_SET_BLOCKLEN               16U   /* (ac): Set block length */
#define    AMBA_SD_CMD17_READ_SINGLE_BLOCK          17U   /* (adtc): Read one block */
#define    AMBA_SD_CMD18_READ_MULTIPLE_BLOCK        18U   /* (adtc): Read multiple blocks */
#define    AMBA_SD_CMD19_SEND_TUNING_BLOCK          19U   /* (adtc): 64 bytes tuning pattern is sent for SDR50 & SDR104 */
#define    AMBA_SD_CMD20_SPEED_CLASS_CONTROL        20U   /* (ac): Speed Class control command */
#define    AMBA_SD_CMD24_WRITE_SINGLE_BLOCK         24U   /* (adtc): Write one block */
#define    AMBA_SD_CMD25_WRITE_MULTIPLE_BLOCK       25U   /* (adtc): Write multiple blocks */
#define    AMBA_SD_CMD32_ERASE_WR_BLOCK_START       32U   /* (ac): Sets the address of the first write block to be erased */
#define    AMBA_SD_CMD33_ERASE_WR_BLOCK_END         33U   /* (ac): Sets the address of the last write block of the continuous range to be erased */
#define    AMBA_SD_CMD38_ERASE                      38U   /* (ac): Erases all previously selected write blocks */

#define    AMBA_SD_CMD55_APP_CMD                    55U   /* (ac): Indicates to the card that the next CMD is an application specific CMD */
#define    AMBA_SD_CMD56_GEN_CMD                    56U   /* (ac): Used either to transfer or get  a data block for general purpose/application specific commands */
#define    AMBA_SD_ACMD6_SET_BUS_WIDTH              6U    /* (ac): Defines the data bus width ('00'=1bit or '10'=4 bits bus) to be used for data transfer */
#define    AMBA_SD_ACMD13_SD_STATUS                 13U   /* (adtc): Sends the SD Status */
#define    AMBA_SD_ACMD22_SEND_NUM_WR_BLOCKS        22U   /* (adtc): Sends the number of the written (w/o errors) blocks */
#define    AMBA_SD_ACMD23_SET_WR_BLK_ERASE_COUNT    23U   /* (adtc): Sets the number of write blocks to be pre-erased before writing */
#define    AMBA_SD_ACMD41_SD_SEND_OP_COND           41U   /* (bcr): Sends host capacity support info (HCS) and asks the card to send OCR */
#define    AMBA_SD_ACMD42_SET_CLR_CARD_DETECT       42U   /* (ac): Connect[1]/Disconnect[0] the 50KOhm pull-up resistor on CD/DAT3 (pin 1) of the card */
#define    AMBA_SD_ACMD51_SEND_SCR                  51U   /* (adtc): Reads the SD Configuration Register (SCR) */

#define    AMBA_SD_CMD1_SEND_OP_COND                1U    /* (MMC): (bcr): Asks the card, in idlestate, to send its Operat-ing Conditions Register contents in the response on the CMD line */
#define    AMBA_SD_CMD8_SEND_EXT_CSD                8U    /* (MMC): (adtc): The card sends its EXT_CSD register as a block of data. */
#define    AMBA_SD_CMD21_SEND_TUNING_BLOCK          21U   /* (MMC): (adtc): 64 or 128 bytes tuning pattern is sent for HS200. */

#define    AMBA_SD_CMD5_IO_SEND_OP_COND             5U    /* (SDIO): The function of CMD5 for SDIO cards is similar to the operation of ACMD41 for SD memory cards */
#define    AMBA_SD_CMD52_CCCR                       52U   /* (SDIO): I/O Reset, I/O about, or I/O Block Length, Bus_Width */
#define    AMBA_SD_CMD53_IO_BLK_OPERATION           53U   /* (SDIO): I/O block operations */


/*
 * SD RCA Argument for CMD7, CMD9, CMD10, CMD13, CMD15, CMD55
 */
typedef struct {
    UINT32  StuffBits:          16; /* [15:0]: Stuff bits */
    UINT32  RelativeCardAddr:   16; /* [31:16]: Relative Card Address */
} AMBA_SD_RCA_ARGUMENT_s;

/*
 * SD CMD4 Argument
 */
typedef union {
    UINT32  Data;                       /* 32 bits = 1 word */

    struct {
        UINT32  StuffBits:          16; /* [15:0]: Stuff bits */
        UINT32  DriverStage:        16; /* [31:16]: Programs the DSR of all cards */
    } Bits;
} AMBA_SD_CMD4_ARGUMENT_u;

/*
 * SD CMD6 Argument
 */
typedef struct {
    UINT32  AccessMode:         4;  /* [3:0]: Function group 1 for access mode */
    UINT32  CommandSystem:      4;  /* [7:4]: Function group 2 for command system */
    UINT32  DrivingStrength:    4;  /* [11:8]: Function group 3 for driving strength */
    UINT32  CurrentLimit:       4;  /* [15:12]: Function group 4 for current limit */
    UINT32  Reserved2:          4;  /* [19:16]: Reserved for function group 5 (0x0 or 0xf) */
    UINT32  Reserved1:          4;  /* [23:20]: Reserved for function group 6 (0x0 or 0xf) */
    UINT32  Reserved0:          7;  /* [30:24]: Reserved (All zero) */
    UINT32  Mode:               1;  /* [31]: 0 - Check function; 1 - Switch function */
} AMBA_SD_CMD6_ARGUMENT_s;

/*
 * The switch function status is the returned data block that contains function
 * and current consumption information. The block length is predefined to
 * 512 bits and the use of SET_BLK_LEN command is not necessary.
 */
typedef struct {
    UINT32  MaxCurrent:        16;  /* [511:496]: Maximum current consumption (0:Error, 1:1mA, 2:2mA... ) */
    UINT32  SupportGroup6:     16;  /* [495:480]: Function group 6, information. If a bit i is set, function i is supported */
    UINT32  SupportGroup5:     16;  /* [479:464]: Function group 5, information. If a bit i is set, function i is supported */
    UINT32  SupportGroup4:     16;  /* [463:448]: Function group 4, information. If a bit i is set, function i is supported */
    UINT32  SupportGroup3:     16;  /* [447:432]: Function group 3, information. If a bit i is set, function i is supported */
    UINT32  SupportGroup2:     16;  /* [431:416]: Function group 2, information. If a bit i is set, function i is supported */
    UINT32  SupportGroup1:     16;  /* [415:400]: Function group 1, information. If a bit i is set, function i is supported */
    UINT32  StatusGroup6 :     4;   /* [399:396]: mode0 - Function can be switched. mode1 - Switch result of the command, 0xF: argument error */
    UINT32  StatusGroup5 :     4;   /* [395:392]: mode0 - Function can be switched. mode1 - Switch result of the command, 0xF: argument error */
    UINT32  StatusGroup4 :     4;   /* [391:388]: mode0 - Function can be switched. mode1 - Switch result of the command, 0xF: argument error */
    UINT32  StatusGroup3 :     4;   /* [387:384]: mode0 - Function can be switched. mode1 - Switch result of the command, 0xF: argument error */
    UINT32  StatusGroup2 :     4;   /* [383:380]: mode0 - Function can be switched. mode1 - Switch result of the command, 0xF: argument error */
    UINT32  StatusGroup1 :     4;   /* [379:376]: mode0 - Function can be switched. mode1 - Switch result of the command, 0xF: argument error */
    UINT32  StructVersion:     8;   /* [375:368]: Data Structure Version, 0 : bits 511:376 are defined ,1: bits 511:272 are defined */
    UINT32  BusyGroup6   :     16;  /* [367:352]: Busy Status of functions in group 6. bit [i] is set, function [i] is busy. */
    UINT32  BusyGroup5   :     16;  /* [351:336]: Busy Status of functions in group 5. bit [i] is set, function [i] is busy. */
    UINT32  BusyGroup4   :     16;  /* [335:320]: Busy Status of functions in group 4. bit [i] is set, function [i] is busy. */
    UINT32  BusyGroup3   :     16;  /* [319:304]: Busy Status of functions in group 3. bit [i] is set, function [i] is busy. */
    UINT32  BusyGroup2   :     16;  /* [302:288]: Busy Status of functions in group 2. bit [i] is set, function [i] is busy. */
    UINT32  BusyGroup1   :     16;  /* [287:272]: Busy Status of functions in group 1. bit [i] is set, function [i] is busy. */
    UINT8   Reserve[34];            /* [271:0]: Reserved (All '0') */
} AMBA_SD_CMD6_STATUS_s;

#define AMBA_SD_MODE_CHECK_FUNC         0x0U
#define AMBA_SD_MODE_SWITCH_FUNC        0x1U
#define AMBA_SD_CURRENT_FUNC            0xFU
#define AMBA_SD_DEFAULT_FUNC            0x0U
#define AMBA_SD_CMD_SYS_ECOMMERCE       0x1U
#define AMBA_SD_ACCESS_HIGH_SPEED       0x1U

/** Switch command (spec 3.x) command) status */
#define AMBA_SD_UHS_DDR50_SUPPORT       0x10U
#define AMBA_SD_UHS_SDR104_SUPPORT      0x8U
#define AMBA_SD_UHS_SDR50_SUPPORT       0x4U
#define AMBA_SD_UHS_SDR25_SUPPORT       0x2U
#define AMBA_SD_UHS_SDR12_SUPPORT       0x1U

#define AMBA_SD_DRIVE_TYPE_D_SUPPORT    0x8
#define AMBA_SD_DRIVE_TYPE_C_SUPPORT    0x4
#define AMBA_SD_DRIVE_TYPE_A_SUPPORT    0x2
#define AMBA_SD_DRIVE_TYPE_B_SUPPORT    0x1

#define AMBA_SD_CURRENT_800MA_SUPPORT   0x8U
#define AMBA_SD_CURRENT_600MA_SUPPORT   0x4U
#define AMBA_SD_CURRENT_400MA_SUPPORT   0x2U
#define AMBA_SD_CURRENT_200MA_SUPPORT   0x1U

#define AMBA_SD_UHS_DDR50_MODE          0x4U
#define AMBA_SD_UHS_SDR104_MODE         0x3U
#define AMBA_SD_UHS_SDR50_MODE          0x2U
#define AMBA_SD_UHS_SDR25_MODE          0x1U
#define AMBA_SD_UHS_SDR12_MODE          0x0U

#define AMBA_SD_DRIVE_STG_TYPE_D        0x3
#define AMBA_SD_DRIVE_STG_TYPE_C        0x2
#define AMBA_SD_DRIVE_STG_TYPE_A        0x1
#define AMBA_SD_DRIVE_STG_TYPE_B        0x0

#define AMBA_SD_CURRENT_800MA           0x3
#define AMBA_SD_CURRENT_600MA           0x2
#define AMBA_SD_CURRENT_400MA           0x1
#define AMBA_SD_CURRENT_200MA           0x0

#define AMBA_SD_SDR12_MAX_FREQUENCY     25000000U
#define AMBA_SD_SDR25_MAX_FREQUENCY     50000000U
#define AMBA_SD_SDR50_MAX_FREQUENCY     100000000U
#define AMBA_SD_SDR104_MAX_FREQUENCY    208000000U
#define AMBA_SD_DDR50_MAX_FREQUENCY     50000000U

/*
 * EMMC CMD6 Argument
 */
typedef struct {
    UINT32  CmdSet:             3;  /* [2:0]:   */
    UINT32  Reserved1:          5;  /* [7:3]:   Reserved (All zero) */
    UINT32  Value:              8;  /* [15:8]:  */
    UINT32  Index:              8;  /* [23:16]: */
    UINT32  Access:             2;  /* [25:24]  */
    UINT32  Reserved0:          6;  /* [31:26]: Reserved (All zero) */
} AMBA_SD_EMMC_CMD6_ARGUMENT_s;

#define ACCESS_COMMAND_SET  0x0
#define ACCESS_SET_BITS     0x1U
#define ACCESS_CLEAR_BITS   0x2U
#define ACCESS_WRITE_BYTE   0x3U
/* HS200 Argument */
#define EMMC_DRV_STG_TYPE_0_MODE   0x0fU
#define EMMC_DRV_STG_TYPE_1_MODE   0x10U
#define EMMC_DRV_STG_TYPE_2_MODE   0x20U
#define EMMC_DRV_STG_TYPE_3_MODE   0x30U
#define EMMC_DRV_STG_TYPE_4_MODE   0x40U

#define EMMC_HS_SDR52_MODE   0x1U
#define EMMC_HS200_MODE      0x2U
#define EMMC_HS400_MODE      0x3U

#define AMBA_EMMC_HS200_MAX_FREQUENCY   200000000U
#define AMBA_EMMC_HS400_MAX_FREQUENCY   200000000U
#define AMBA_EMMC_SDR52_MAX_FREQUENCY   50000000U

/*
 * SD CMD8 Argument
 */
typedef struct {
    UINT32  CheckPattern:       8;  /* [7:0]: Check pattern */
    UINT32  SupplyVoltage:      4;  /* [11:8]: Supply voltage (VHS) - 1: 2.7~3.6V */
    UINT32  Reserved:           20; /* [31:12]: Reserved */
} AMBA_SD_CMD8_ARGUMENT_s;

#define SD_CHECK_PATTERN    (0xaa)    /* Recommanded value in SD 2.0 */
#define SD_HIGH_VOLTAGE     (0x01)    /* 2.7~3.6V */
#define SD_LOW_VOLTAGE      (0x02)    /* Reserved for Low Voltage Range */

/*
 * SD CMD20 Argument
 */
typedef struct {
    UINT32  Reserved:           28; /* [27:0]: Reserved */
    UINT32  SpeedClassCtrl:     4;  /* [31:28]: Speed class control */
} AMBA_SD_CMD20_ARGUMENT_s;

/*
 * SD CMD56 Argument
 */
typedef union {
    UINT32  Data;                       /* 32 bits = 1 word */

    struct {
        UINT32  ReadWriteFlag:      1;  /* [0]: 1 - Reading data from the card; 0 - Writing data to the card */
        UINT32  StuffBits:          31; /* [31:1]: Stuff bits */
    } Bits;
} AMBA_SD_CMD56_ARGUMENT_u;

/*
 * SD ACMD6 Argument
 */
typedef struct {
    UINT32  BusWidth:           2;  /* [1:0]: 0x0 - 1 bit bus width; 0x2 - 4 bits bus width */
    UINT32  StuffBits:          30; /* [31:2]: Stuff bits */
} AMBA_SD_ACMD6_ARGUMENT_s;

#define AMBA_SD_ACMD6_BUS_WIDTH_1_BIT   0
#define AMBA_SD_ACMD6_BUS_WIDTH_4_BIT   2

/*
 * SD CMD19 Argument
 */
#define AMBA_SD_TUNING_BLOCK_BYTE_SIZE  64U  /* Tuning block patter size in Byte */

/*
 * SD CMD1 Argument
 */
typedef struct {
    UINT32  Reserved0:          7;  /* [6:0]:   Reserved */
    UINT32  V195_170:           1;  /* [7]:     Voltage Range 1.70-1.95 */
    UINT32  V20_26:             7;  /* [14:8]:  Voltage Range 2.00-2.60 */
    UINT32  V27_36:             9;  /* [23:15]: Voltage Range 2.00-2.60 */
    UINT32  Reserved1:          5;  /* [28:24]: Reserved */
    UINT32  AccessMode:         2;  /* [30:29]: 00b: (byte mode), 10b (sector mode) */
    UINT32  Reserved2:          1;  /* [31]:    Reserved ((card power up status bit (busy)) */
} AMBA_SD_CMD1_ARGUMENT_s;

/*
 * SD ACMD23 Argument
 */
typedef struct {
    UINT32  StuffBits:          9;  /* [31:23]: Stuff bits */
    UINT32  NumOfBlock:         23; /* [22:0]: Number of write block to be pre-erased before writing */
} AMBA_SD_ACMD23_ARGUMENT_s;

/*
 * SD ACMD41 Argument
 */
typedef struct {
    UINT32  Vdd:                24; /* [31:8]: Voltage window (OCR[23:0]) */
    UINT32  S18r:               1;  /* [32]: Request to switch to 1.8V signal */
    UINT32  Reserved0:          3;  /* [35:33]: Reserved */
    UINT32  Xpc:                1;  /* [36]: 0 - 0.36W max power; 1 - 0.54W max power */
    UINT32  Reserved1:          1;  /* [37]: Reserved for eSD */
    UINT32  Hcs:                1;  /* [38]: OCR[30] */
    UINT32  Reserved2:          1;  /* [39]: Reserved */
} AMBA_SD_ACMD41_ARGUMENT_s;

/*
 * SD ACMD42 Argument
 */
typedef struct {
    UINT32  SetCd:              1;  /* [0]: 1 - Conntect; 0 -Disconnect the 50kOhm pull-up resistor on CD/DAT3 (pin 1) */
    UINT32  StuffBits:          31; /* [31:1]: Stuff bits */
} AMBA_SD_ACMD42_ARGUMENT_s;

/*
 * SDIO CMD5 Argument
 */
typedef struct {
    UINT32  IoOCR:              24; /* [31:8]: Operation Condition Register (OCR) */
    UINT32  S18R:               1;  /* [32]: Switch to 1.8V Request */
    UINT32  Reserved:           7;  /* [39:33]: Reserved */
} AMBA_SDIO_CMD5_ARGUMENT_s;

/*
 * SDIO CMD52 Argument
 */
typedef struct {
    UINT32  WriteData:          8;  /* [15:8]: Write Data or Stuff Bits */
    UINT32  Reserved0:          1;  /* [16]: Stuff bits */
    UINT32  RegAddr:            17; /* [33:17]: Register Address */
    UINT32  Reserved1:          1;  /* [34]: Stuff bits */
    UINT32  ReadAfterWriteFlag: 1;  /* [35]: Read after Write flag */
    UINT32  FunctionNo:         3;  /* [38:36]: Function Number */
    UINT32  ReadWriteFlag:      1;  /* [39]: Read(0)/Write(1) flag */
} AMBA_SDIO_CMD52_ARGUMENT_s;

/*
 * SDIO CMD53 Argument
 */
typedef struct {
    UINT32  Count:              9;  /* [16:8]: Byte or Block count */
    UINT32  RegAddr:            17; /* [33:17]: Register Address */
    UINT32  OpCode:             1;  /* [34]: 0 - Muilti byte R/W to fixed address; 1 - Multi byte R/W to incrementing address */
    UINT32  BlockMode:          1;  /* [35]: 0 - Byte basis; 1 - Block basis */
    UINT32  FunctionNo:         3;  /* [38:36]: Function Number */
    UINT32  ReadWriteFlag:      1;  /* [39]: Read(0)/Write(1) flag */
} AMBA_SDIO_CMD53_ARGUMENT_s;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
 * SD All Command Argument
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef union {
    AMBA_SD_RCA_ARGUMENT_s      Rca;    /* RCA: Relative Card Address */
    AMBA_SD_CMD4_ARGUMENT_u     Cmd4;   /* CMD4: Programs the DSR of all cards */
    AMBA_SD_CMD6_ARGUMENT_s     Cmd6;   /* CMD6: Switch function */
    AMBA_SD_CMD8_ARGUMENT_s     Cmd8;   /* CMD8: Sends SD Memory Card interface condition */
    AMBA_SD_CMD20_ARGUMENT_s    Cmd20;  /* CMD20: Speed class control */
    AMBA_SD_CMD56_ARGUMENT_u    Cmd56;  /* CMD56: Used either to transfer or get a data block for general purpose/application specific commands */

    AMBA_SD_ACMD6_ARGUMENT_s    Acmd6;  /* ACMD6: Defines the data bus width to be used for data transfer */
    AMBA_SD_ACMD23_ARGUMENT_s   Acmd23; /* ACMD23: Sets the number of write blocks to be pre-erased before writing */
    AMBA_SD_ACMD41_ARGUMENT_s   Acmd41; /* ACMD41: Sends host capacity support info (HCS) and asks the card to send OCR */
    AMBA_SD_ACMD42_ARGUMENT_s   Acmd42; /* ACMD42: Connect[1]/Disconnect[0] the 50KOhm pull-up resistor on CD/DAT3 (pin 1) of the card */

    AMBA_SDIO_CMD5_ARGUMENT_s   Cmd5;   /* CMD5: SDIO command */
    AMBA_SDIO_CMD52_ARGUMENT_s  Cmd52;  /* CMD52: I/O Reset, I/O about, or I/O Block Length, Bus_Width */
} AMBA_SD_CMD_ARGUMENT_u;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
 * SD Response: R1/R1b (normal response command), Code length = 48 bits
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct {
    UINT32  CardStatus;     /* [39:8]: Card status */
} AMBA_SD_RESPONSE_R1_s;

#define R1_OUT_OF_RANGE         0x80000000U  /* er, c */
#define R1_ADDRESS_ERROR        0x40000000U  /* erx, c */
#define R1_BLOCK_LEN_ERROR      0x20000000U  /* er, c */
#define R1_ERASE_SEQ_ERROR      0x10000000U  /* er, c */
#define R1_ERASE_PARAM          0x08000000U  /* ex, c */
#define R1_WP_VIOLATION         0x04000000U  /* erx, c */
#define R1_CARD_IS_LOCKED       0x02000000U  /* sx, a */
#define R1_LOCK_UNLOCK_FAILED   0x01000000U  /* erx, c */
#define R1_COM_CRC_ERROR        0x00800000U  /* er, b */
#define R1_ILLEGAL_COMMAND      0x00400000U  /* er, b */
#define R1_CARD_ECC_FAILED      0x00200000U  /* ex, c */
#define R1_CC_ERROR             0x00100000U  /* erx, c */
#define R1_ERROR                0x00080000U  /* erx, c */
#define R1_UNDERRUN             0x00040000U  /* ex, c */
#define R1_OVERRUN              0x00020000U  /* ex, c */
#define R1_CID_CSD_OVERWRITE    0x00010000U  /* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP        0x00008000U  /* sx, c */
#define R1_CARD_ECC_DISABLED    0x00004000U  /* sx, a */
#define R1_ERASE_RESET          0x00002000U  /* sr, c */
#define R1_READY_FOR_DATA       0x00000100U  /* sx, a */
#define R1_APP_CMD              0x00000080U  /* sr, c */

/* AMBA SD/MMC error code */
#define SD_ERR_R1_OUT_OF_RANGE          (-200)
#define SD_ERR_R1_ADDRESS_ERROR         (-201)
#define SD_ERR_R1_BLOCK_LEN_ERROR       (-202)
#define SD_ERR_R1_ERASE_SEQ_ERROR       (-203)
#define SD_ERR_R1_ERASE_PARAM           (-204)
#define SD_ERR_R1_WP_VIOLATION          (-205)
#define SD_ERR_R1_CARD_IS_LOCKED        (-206)
#define SD_ERR_R1_LOCK_UNLOCK_FAILED    (-207)
#define SD_ERR_R1_COM_CRC_ERROR         (-208)
#define SD_ERR_R1_ILLEGAL_COMMAND       (-209)
#define SD_ERR_R1_CARD_ECC_FAILED       (-210)
#define SD_ERR_R1_CC_ERROR              (-211)
#define SD_ERR_R1_ERROR                 (-212)
#define SD_ERR_R1_UNDERRUN              (-213)
#define SD_ERR_R1_OVERRUN               (-214)
#define SD_ERR_R1_CID_CSD_OVERWRITE     (-215)
#define SD_ERR_R1_WP_ERASE_SKIP         (-216)
#define SD_ERR_R1_CARD_ECC_DISABLED     (-217)
#define SD_ERR_R1_ERASE_RESET           (-218)

/*
 * SD Response: R2 (CID, CSD register), Code length = 136 bits
 */
typedef struct {
    UINT32  CidCsdReg[4];           /* [127:0]: CID or CSD resigter including internal CRC7 */
} AMBA_SD_RESPONSE_R2_s;

/*
 * SD Response: R3 (OCR register), Code length = 48 bits
 */
typedef struct {
    UINT32  OcrReg:         32;     /* [39:8]: OCR register */
} AMBA_SD_RESPONSE_R3_s;

/*
 * SD Response: R6 (Published RCA response), Code length = 48 bits
 */
typedef struct {
    UINT32  CardStatus:     16;     /* [23:8]: [15:0] Card status bits: 23,22,19,12:0 */
    UINT32  PublishedRCA:   16;     /* [39:24]: New published RCA[31:16] of the card */
} AMBA_SD_RESPONSE_R6_s;

/*
 * SD Response: R7 (Card interface condition), Code length = 48 bits
 */
typedef struct {
    UINT32  EchoBack:       8;      /* [15:8]: Ech-back of check pattern */
    UINT32  VoltageRange:   4;      /* [19:16]: The voltage range that the card supports */
    UINT32  Reserved:       20;     /* [39:20]: Reserved */
} AMBA_SD_RESPONSE_R7_s;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
 * SDIO Response to CMD5: R4 (IO_SEND_OP_COND Response), Code length = 48 bits
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct {
    UINT32  IoOCR:          24;     /* [31:8]: I/O Operation Conditions Register */
    UINT32  S18A:           1;      /* [32]: 1 = Switching to 1.8V accepted (Support in SD mode only) */
    UINT32  StuffBits:      2;      /* [34:33]: Sfuff Bits */
    UINT32  MemoryPresent:  1;      /* [35]: 1 = the card also contains SD memory */
    UINT32  NumIoFunction:  3;      /* [38:36]: Number of I/O functions */
    UINT32  Ready:          1;      /* [39]: 1 = Card is ready to operate after initialization */
} AMBA_SDIO_RESPONSE_R4_s;

/*
 * SDIO Response to CMD52: R5 (IO_RW_DIRECT Response), Code length = 48 bits
 */
typedef struct {
    UINT32  Data:           8;      /* [15:8]: Read or Write Data */
    UINT32  Flags:          8;      /* [23:16]: The status of SDIO card */
    UINT32  StuffBits:      16;     /* [39:24]: Sfuff Bits */
} AMBA_SDIO_RESPONSE_R5_s;

/* IO_RW_DIRECT response(R5) flags */
#define R5_COM_CRC_ERROR    0x80U
#define R5_ILLEGAL_COMMAND  0x40U

/* bit 5:4 IO_CURRENT_STATE */
#define R5_IO_CUR_STA_DIS   0x0
#define R5_IO_CUR_STA_CMD   0x1
#define R5_IO_CUR_STA_TRN   0x2
#define R5_IO_CUR_STA_RFU   0x3

#define R5_ERROR            0x08U
#define R5_FUNC_NUM_ERROR   0x02U
#define R5_OUT_OF_RANGE     0x01U

/*
 * SDIO Modified R6 Response to CMD3: Code length = 48 bits
 */
typedef struct {
    UINT32  Undefined:      13;     /* [20:8]: [12:0] = Undefined. Should read as 0 for SDIO only cards. */
    UINT32  Error:          1;      /* [21]: [13] = A general or an unknown error occurred during the operation */
    UINT32  IllegalCommand: 1;      /* [22]: [14] = Command not legal for the card state */

    /* [23:8]: [15:0] SDIO card status bits */
    UINT32  ComCrcError:    1;      /* [23]: [15] = The CRC check of the previous command failed */

    UINT32  PublishedRCA:   16;     /* [39:24]: New published RCA[31:16] of the card */
} AMBA_SDIO_RESPONSE_R6_s;

typedef union {
    UINT32   Data;                      /* [39:8]: 32 bits = 1 word */

    AMBA_SD_RESPONSE_R1_s   SdR1;
    AMBA_SD_RESPONSE_R3_s   SdR3;
    AMBA_SD_RESPONSE_R6_s   SdR6;
    AMBA_SD_RESPONSE_R7_s   SdR7;

    AMBA_SDIO_RESPONSE_R4_s SdioR4;
    AMBA_SDIO_RESPONSE_R5_s SdioR5;
    AMBA_SDIO_RESPONSE_R6_s SdioR6;
} AMBA_SD_RESPONSE_48BITS_u;

#define AMBA_MAX_NUM_SDIO_FUNCTION      (1+7)   /* Maximum Number of SDIO Functions */

#endif /* AMBA_SD_STD_H */
