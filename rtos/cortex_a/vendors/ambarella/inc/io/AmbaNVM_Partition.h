/**
 *  @file AmbaNVM_Partition.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for NVM (Non-Volatile-Memory) Partitions
 *
 */

#ifndef AMBA_NVM_PARTITION_H
#define AMBA_NVM_PARTITION_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * NVM api return value
 */
#define NVM_ERR_0000                    (NVM_ERR_BASE)          /* Invalid argument */
#define NVM_ERR_0001                    (NVM_ERR_BASE | 0x1U)   /*   */
#define NVM_ERR_0002                    (NVM_ERR_BASE | 0x2U)   /* NAND Device driver Read error */

#define NVM_ERR_0003                    (NVM_ERR_BASE | 0x3U)
#define NVM_ERR_0004                    (NVM_ERR_BASE | 0x4U)
#define NVM_ERR_0005                    (NVM_ERR_BASE | 0x5U)   /* NAND Device driver write error */
#define NVM_ERR_0006                    (NVM_ERR_BASE | 0x6U)
#define NVM_ERR_0007                    (NVM_ERR_BASE | 0x7U)
#define NVM_ERR_0008                    (NVM_ERR_BASE | 0x8U)
#define NVM_ERR_0009                    (NVM_ERR_BASE | 0x9U)
#define NVM_ERR_000A                    (NVM_ERR_BASE | 0xAU)

#define NVM_ERR_NONE                    OK
#define NVM_ERR_ARG                     NVM_ERR_0000
#define NVM_ERR_NAND_READ               NVM_ERR_0001
#define NVM_ERR_eMMC_READ               NVM_ERR_0002
#define NVM_ERR_SPI_NOR_READ            NVM_ERR_0003
#define NVM_ERR_SPI_NAND_READ           NVM_ERR_0004
#define NVM_ERR_NAND_WRITE              NVM_ERR_0005
#define NVM_ERR_eMMC_WRITE              NVM_ERR_0006
#define NVM_ERR_SPI_NOR_WRITE           NVM_ERR_0007
#define NVM_ERR_SPI_NAND_WRITE          NVM_ERR_0008
#define NVM_ERR_FILE_NOT_FOUND          NVM_ERR_0009
#define NVM_ERR_API                     NVM_ERR_000A

/*
 * definitions of System partition ID
 */
#define AMBA_SYS_PARTITION_BOOTSTRAP    (0U)           /* Bootstrap */
#define AMBA_SYS_PARTITION_BOOTLOADER   (1U)           /* Bootloader */
#define AMBA_SYS_PARTITION_FW_UPDATER   (2U)           /* Firmware Updater */
#define AMBA_SYS_PARTITION_ARM_TRUST_FW (3U)           /* ARM trust Firmware */
#define AMBA_SYS_PARTITION_RESERVED0    (4U)           /* Reserved */

#define AMBA_SYS_PARTITION_QNX_IPL      (4U)           /* QNX IPL */
#define AMBA_SYS_PARTITION_BLD2         (5U)           /* BLD2 */

#define AMBA_NUM_SYS_PARTITION          (6U)           /* Total Number of System Partitions */

/*
 * definitions of User partition ID
 */
#define     AMBA_USER_PARTITION_PTB                   (0U)       /* User PTB, only 1 block */
#define     AMBA_USER_PARTITION_SYS_SOFTWARE          (1U)       /* System Software */
#define     AMBA_USER_PARTITION_DSP_uCODE             (2U)       /* DSP uCode (ROM Region) */
#define     AMBA_USER_PARTITION_SYS_DATA              (3U)       /* System Data (ROM Region) */
#define     AMBA_USER_PARTITION_LINUX_KERNEL          (4U)       /* Linux Kernel */
#define     AMBA_USER_PARTITION_LINUX_ROOT_FS         (5U)       /* Linux Root File System */
#define     AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG (6U)       /* Linux Hibernation Image */

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define     AMBA_USER_PARTITION_SYS_SOFTWARE_B         (7U)       /* System Software */
#define     AMBA_USER_PARTITION_DSP_uCODE_B            (8U)       /* DSP uCode (ROM Region) */
#define     AMBA_USER_PARTITION_SYS_DATA_B             (9U)       /* System Data (ROM Region) */
#define     AMBA_USER_PARTITION_LINUX_KERNEL_B         (10U)       /* Linux Kernel */
#define     AMBA_USER_PARTITION_LINUX_ROOT_FS_B        (11U)       /* Linux Root File System */
#define     AMBA_USER_PARTITION_VIDEO_REC_INDEX       (12U)       /* Video Recording Index */
#define     AMBA_USER_PARTITION_CALIBRATION_DATA      (13U)       /* Calibration Data */
#define     AMBA_USER_PARTITION_USER_SETTING          (14U)       /* User Settings */
#define     AMBA_USER_PARTITION_FAT_DRIVE_A           (15U)      /* Internal Storage FAT Drive 'A' */
#define     AMBA_USER_PARTITION_FAT_DRIVE_B           (16U)      /* Internal Storage FAT Drive 'B' */
#define     AMBA_USER_PARTITION_R52SYS                (17U)      /* R52 RTOS */
#define     AMBA_USER_PARTITION_XEN                   (18U)      /* XEN */
#define     AMBA_USER_PARTITION_XTB                   (19U)      /* XEN DTB */
#define     AMBA_USER_PARTITION_X0K                   (20U)      /* XEN Dom0 Kernel */
#define     AMBA_USER_PARTITION_X0D                   (21U)      /* XEN Dom0 Initrd */
#else
#define     AMBA_USER_PARTITION_VIDEO_REC_INDEX       (7U)       /* Video Recording Index */
#define     AMBA_USER_PARTITION_CALIBRATION_DATA      (8U)       /* Calibration Data */
#define     AMBA_USER_PARTITION_USER_SETTING          (9U)       /* User Settings */
#define     AMBA_USER_PARTITION_FAT_DRIVE_A           (10U)      /* Internal Storage FAT Drive 'A' */
#define     AMBA_USER_PARTITION_FAT_DRIVE_B           (11U)      /* Internal Storage FAT Drive 'B' */
#define     AMBA_USER_PARTITION_R52SYS                (12U)      /* R52 RTOS */
#define     AMBA_USER_PARTITION_XEN                   (13U)      /* XEN */
#define     AMBA_USER_PARTITION_XTB                   (14U)      /* XEN DTB */
#define     AMBA_USER_PARTITION_X0K                   (15U)      /* XEN Dom0 Kernel */
#define     AMBA_USER_PARTITION_X0D                   (16U)      /* XEN Dom0 Initrd */
#define     AMBA_USER_PARTITION_RESERVED0             (17U)      /* Reserved User Partition-0 */
#define     AMBA_USER_PARTITION_RESERVED1             (18U)      /* Reserved User Partition-1 */
#define     AMBA_USER_PARTITION_RESERVED2             (19U)      /* Reserved User Partition-2 */
#define     AMBA_USER_PARTITION_RESERVED3             (20U)      /* Reserved User Partition-3 */
#define     AMBA_USER_PARTITION_RESERVED4             (21U)      /* Reserved User Partition-4 */
#endif
#define     AMBA_NUM_USER_PARTITION                   (22U)      /* Total Number of User Partitions */

/*
 * definitions of partal load information
 */
#define PLOAD_REGION_NUM  6
typedef struct {
    UINT32  RegionRoStart[PLOAD_REGION_NUM];
    UINT32  RegionRwStart[PLOAD_REGION_NUM];
    UINT32  RegionRoSize [PLOAD_REGION_NUM];
    UINT32  RegionRwSize [PLOAD_REGION_NUM];
    UINT32  LinkerStubStart;
    UINT32  LinkerStubSize;
    UINT32  DspBufStart;
    UINT32  DspBufSize;
    UINT32  ExceptionHdlStart;
    UINT32  ExceptionHdlSize;
} AMBA_PLOAD_PARTITION_s;

/*
 * definitions of partition attributes
 */
#define AMBA_PARTITION_ATTR_READ_ONLY     (0x1U)        /* [0]: 1 - Read Only */
#define AMBA_PARTITION_ATTR_WRITE_PROTECT (0x2U)        /* [1]: 1 - Write Protect */
#define AMBA_PARTITION_ATTR_COMPRESSED    (0x4U)        /* [2]: 1 - Compressed */
#define AMBA_PARTITION_ATTR_EXECUTABLE    (0x8U)        /* [3]: 1 - Executable */
#define AMBA_PARTITION_ATTR_FTL           (0x10U)       /* [4]: 1 - Flash Translation Layer, 0 - Raw */
#define AMBA_PARTITION_ATTR_BACKUP        (0x20U)       /* [5]: 1 - Partition need backup */
#define AMBA_PARTITION_ATTR_STORE_DEVIC   (0xC0U)       /* [7:6]: 0- Nand 1- eMMc 2- Nor 3- SPI Nand */
#define AMBA_PARTITION_ATTR_ERASED        (0x40000000U) /* [30]: 1 - Erased */
#define AMBA_PARTITION_ATTR_INVALID       (0x80000000U) /* [31]: 1 - Invalid/Not-Used */

/*
 * definitions of Network device information
 */
typedef struct {
    UINT8   MacAddr[6];                 /* MAC address */
    UINT8   Reserved[2];
    UINT32  IP;                         /* Bootloader's LAN IP address  */
    UINT32  Mask;                       /* Bootloader's LAN mask */
    UINT32  Gateway;                    /* Bootloader's LAN gateway */
} AMBA_NET_DEV_INFO_s;

/*
 * Partition entry structure
 */
typedef struct {
    UINT8   PartitionName[32];          /* Partition Name */
    UINT32  Attribute;                  /* Attribute of the Partition */
    UINT32  ByteCount;                  /* number of Bytes allocated for the Partition */
    UINT32  BlkCount;                   /* number of Blocks for the Partition  */
    UINT32  StartBlkAddr;               /* start Block Address */
    UINT32  RamLoadAddr;                /* Load address of this partiontion in RAM */

    UINT32  ActualByteSize;             /* actual size in Bytes */
    UINT32  ProgramStatus;              /* the Status of after programming the Partition */

    UINT32  ImageCRC32;                 /* CRC32 of the Image inside the Partition */
} AMBA_PARTITION_ENTRY_s;

/*
 * System Partition table
 */
typedef struct {
    UINT32  Version;                    /* Version of the Partition Table */
    UINT32  PtbMagic;                   /* Magic code of User Partition Table */
    UINT32  BlkByteSize;                /* Block size in Byte */
    UINT32  BlkAddrUserPTB;             /* Block address of User Partition Table */
    AMBA_PARTITION_ENTRY_s  Entry[AMBA_NUM_SYS_PARTITION];

    UINT8   ModelName[64];              /* Product model name */
    UINT8   SerialNo[64];               /* Product Serial Number */
    AMBA_NET_DEV_INFO_s NetDevInfo[6];  /* Network device information */

    UINT32  NAND_SignaturePrimaryBBT;   /* NAND Signature of Primary BBT(Bad Block Table) */
    UINT32  NAND_SignatureMirrorBBT;    /* NAND Signature of Mirror BBT(Bad Block Table) */

    UINT32  CRC32;                      /* CRC32 of System Partition Table */
} AMBA_SYS_PARTITION_TABLE_s;

/*
 * User Partition table
 */
#define AMBA_USER_PTB_TOTAL_SIZE       (2048U)
#define AMBA_USER_PTB_CRC32_SIZE       ((sizeof(UINT32) * 0x2U) + \
                                        (sizeof(AMBA_PARTITION_ENTRY_s) * (UINT32)AMBA_NUM_USER_PARTITION) + \
                                        (sizeof(INT32) * 0x2U))
#define AMBA_USER_PTB_ACTURAL_SIZE     (AMBA_USER_PTB_CRC32_SIZE + (sizeof(UINT32) * 2U) + \
                                        sizeof(AMBA_PLOAD_PARTITION_s) + (sizeof(UINT32) * 6U))
#define AMBA_USER_PTB_PAD_SIZE         (AMBA_USER_PTB_TOTAL_SIZE - AMBA_USER_PTB_ACTURAL_SIZE)

#define AMBA_USER_PTB_EXTRA_CRC32_ADDR (AMBA_USER_PTB_CRC32_SIZE + (sizeof(UINT32) * 0x2U))
#define AMBA_USER_PTB_EXTRA_CRC32_SIZE (sizeof(AMBA_PLOAD_PARTITION_s))

#define AMBA_USER_PTB_MAGIC            (0x1732dfe7U)

#define USERPTB_VERSION (0x2006U)

typedef struct {
    UINT32  Version;                    /* the version of the Partition Table */
    UINT32  BldMagicCode;               /* Bootloader Magic Code: set - Load SysSW; clear - FwUpdater */
    AMBA_PARTITION_ENTRY_s  Entry[AMBA_NUM_USER_PARTITION];

    UINT32  NAND_BlkAddrPrimaryBBT;     /* NAND Block address of Primary BBT(Bad Block Table) */
    UINT32  NAND_BlkAddrMirrorBBT;      /* NAND Block address of Mirror BBT(Bad Block Table) */

    UINT32  CRC32;                      /* CRC32 of User Partition Table */
    UINT32  PtbMagic;                   /* Magic code of User Partition Table */

    AMBA_PLOAD_PARTITION_s PloadInfo;   /* Pload region info */
    UINT32  ExtraCRC32;                 /* Extra CRC32 for the field after PtbMagic */

    UINT32 BlkAddrCurentPTB;            /* NAND Block address of current User PTB */
    UINT32 BlkAddrNextPTB;              /* NAND Block address Pointer to next PTB */
    UINT32 PTBActive;                   /* Indicate this PTB active or not */
    UINT32 PTBNumber;                   /* The No. of this User PTB */
    UINT32 ErrorCode;                   /* Store Error code during FW updater */

    UINT8   Reserved[AMBA_USER_PTB_PAD_SIZE];
} AMBA_USER_PARTITION_TABLE_s;

/*
 * Partition configuration structure
 */
typedef struct {
    char    PartitionName[32];          /* pointer to Partition Name */
    UINT32  Attribute;                  /* Attribute of the Partition */
    UINT32  ByteCount;                  /* number of Bytes for the Partition */
} AMBA_PARTITION_CONFIG_s;

/*
 * definitions of NVM (Non-Volatile-Memory) ID
 */
#define AMBA_NVM_NAND       (0x0U)
#define AMBA_NVM_eMMC       (0x1U)
#define AMBA_NVM_SPI_NOR    (0x2U)
#define AMBA_NVM_SPI_NAND   (0x3U)
#define AMBA_NUM_NVM        (0x4U)        /* Total number of Non-Volatile-Memory types */

/*
 * definitions of ROM ID
 */
#define AMBA_NVM_ROM_REGION_DSP_uCODE (0x0U)      /* DSP uCode */
#define AMBA_NVM_ROM_REGION_SYS_DATA  (0x1U)      /* System Data */
#define AMBA_NUM_NVM_ROM_REGION       (0x2U)      /* Total Number of NVM-ROM Regions */

typedef struct {
    char    FileName[64];               /* ROM file name */
    UINT32  ByteSize;                   /* ROM file size in Bytes */
    UINT32  FileCRC32;                  /* CRC32 of the File */
} AMBA_NVM_ROM_FILE_INFO_s;

typedef struct {
    UINT32  NumSector;                  /* Number of access sectors */
    UINT32  StartSector;                /* Start sector */
    UINT8    *pDataBuf;                  /* Pointer to data buffer */
} AMBA_NVM_SECTOR_CONFIG_s;

typedef struct {
    UINT32  Initialized;                /* 1 - this partion has been initialized */
    UINT32  WriteProtect;               /* 1 - this partition is write protected */
    UINT32  TotalSizeInSectors;         /* Total size of the FTL partition in Sectors */
    UINT64  TotalSizeInBytes;           /* Total size of the FTL partition in Bytes */
    UINT32  PageSizeInBytes;            /* Page size in Bytes */
    UINT32  BlockSizeInPages;           /* Block size in Pages */
    UINT32  EraseSizeInBlocks;          /* Erase size in Blocks */
    UINT32  SectorSizeInBytes;          /* Sector size in Bytes */
} AMBA_NVM_FTL_INFO_s;

/*
 * Defined in AmbaNVM_Partition.c
 */
UINT32 AmbaNVM_GetRomFileInfo(UINT32 NvmID, UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s * pRomFileInfo);
UINT32 AmbaNVM_GetRomFileSize(UINT32 NvmID, UINT32 RegionID, const char *pFileName, UINT32 *pFileSize);
UINT32 AmbaNVM_ReadRomFile(UINT32 NvmID, UINT32 RegionID, const char *pFileName, UINT32 StartPos,
                           UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut);

UINT32 AmbaNVM_GetFtlInfo(UINT32 NvmID, UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pInfo);
UINT32 AmbaNVM_ReadSector(UINT32 NvmID, UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);
UINT32 AmbaNVM_WriteSector(UINT32 NvmID, UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);

UINT32 AmbaNVM_ReadSysPartitionTable(UINT32 NvmID, AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
UINT32 AmbaNVM_WriteSysPartitionTable(UINT32 NvmID, AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
UINT32 AmbaNVM_ReadUserPartitionTable(UINT32 NvmID, AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);
UINT32 AmbaNVM_WriteUserPartitionTable(UINT32 NvmID, AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);

UINT32 AmbaNVM_ReadPartitionInfo(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo);
UINT32 AmbaNVM_ReadPartition(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaNVM_WritePartition(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaNVM_ErasePartition(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);

UINT32 AmbaNVM_WritePhyBlock(UINT32 NvmID, UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaNVM_ReadPhyBlock(UINT32 NvmID, UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define BLD_MAGIC_CODE (0x5A56A50A)

#define FW_PARTITION_ACTIVE_FLAG        (0x10000000)

UINT32 AmbaNVM_WriteRawSector(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 *pBadSectors, UINT32 TimeOut);
UINT32 AmbaNVM_IsBldMagicCodeSet(UINT32 NvmID);
UINT32 AmbaNVM_SetBldMagicCode(UINT32 NvmID, UINT32 TimeOut);
UINT32 AmbaNVM_EraseBldMagicCode(UINT32 NvmID, UINT32 TimeOut);
UINT32 AmbaNVM_ReadBldMagicCode(UINT32 NvmID);
#endif

#endif  /* AMBA_NVM_PARTITION_H */
