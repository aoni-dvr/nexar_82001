/**
 *  @file AmbaEMMC_Partition.c
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
 *  @details EMMC Partition Management APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"

#include "AmbaNAND.h"
#include "AmbaNVM_Ctrl.h"

#include "AmbaSD.h"
#include "AmbaRTSL_SD.h"
#include "AmbaSD_Ctrl.h"

#define BOOT_BUS_WIDTH  177
#define BOOT_CONFIG     179U
#define EMMC_HW_RESET   162
#define EXT_CSD_SIZE    512

#define EMMC_ACCP_USER          0U
#define EMMC_ACCP_BP_1          1U
#define EMMC_ACCP_BP_2          2U

#define EMMC_BOOTP_USER         0x38U
#define EMMC_BOOTP_BP_1         0x8U
#define EMMC_BOOTP_BP_2         0x10U

#if defined(CONFIG_EMMC_BOOTPART_USER)
#define FLASH_PROG_EMMC_BOOT_PART   EMMC_BOOTP_USER
#define EMMC_CMD6_VALUE_ACCP        EMMC_ACCP_USER
#elif defined(CONFIG_EMMC_BOOTPART_BP1)
#define FLASH_PROG_EMMC_BOOT_PART   EMMC_BOOTP_BP_1
#define EMMC_CMD6_VALUE_ACCP        EMMC_ACCP_BP_1
#else
#define FLASH_PROG_EMMC_BOOT_PART   EMMC_BOOTP_BP_2
#define EMMC_CMD6_VALUE_ACCP        EMMC_ACCP_BP_2
#endif

/* #define AMBA_ROM_CRC_CHECK */

/* Due to the FDMA 8-Byte Boundary issue */
static AMBA_NVM_ROM_DSP_uCODE_CTRL_s AmbaEMMC_RomDspUcodeCtrl GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static AMBA_NVM_ROM_SYS_DATA_CTRL_s  AmbaEMMC_RomSysDataCtrl GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT32 WritePtbFlag = 0;
static UINT32 EmmcUserPtbNextBlock = 0U;

/*
 *  @RoutineName:: AmbaEMMC_GetActivePtbNo
 *
 *  @Description:: Get the No. of the current active parttiton table
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *        The No. of the current active parttiton table
 */
UINT32 AmbaEMMC_GetActivePtbNo(UINT32 * pPTBActive)
{
    UINT32 i;
    UINT32 RetVal;

    *pPTBActive = 0U;

    for(i = 0; i < 2U; i++) {
        RetVal = AmbaEMMC_ReadUserPTB(NULL, i, 1000);

        if(pAmbaRTSL_EmmcUserPartTable ->PTBActive != 0U) {
            *pPTBActive = i;
            break;
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_SetActivePtbNo
 *
 *  @Description:: Set specific parttiton table to active or nonactive
 *
 *  @Input      ::
 *      PtbNo: The No. of the  partition table (start form 0)
 *      Active: 0 -Nonactive 1 - Active
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
 */
UINT32 AmbaEMMC_SetActivePtbNo(UINT32 PtbNo, UINT32 Active)
{
    UINT32 RetVal;

    RetVal = AmbaEMMC_ReadUserPTB(NULL, PtbNo, 1000);

    if (RetVal == NVM_ERR_NONE) {
        pAmbaRTSL_EmmcUserPartTable ->PTBActive = Active;
        RetVal = AmbaEMMC_WriteUserPTB(NULL, PtbNo, 1000);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_InitPtbBbt
 *
 *  @Description:: Init BBT and System/User Partition Tables
 *
 *  @Input      ::
 *      TimeOut: Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_InitPtbBbt(UINT32 TimeOut)
{
    UINT32 RetVal;
    UINT32 Crc32;
    const UINT8 *pBuf = NULL;
    UINT32 CreatPTB = 0U;
    UINT32 ActivePTB = 0U;

    pAmbaRTSL_EmmcSysPartTable  = &AmbaRTSL_EmmcSysPartTable;
    pAmbaRTSL_EmmcUserPartTable  = &AmbaRTSL_EmmcUserPartTable;

    /* Read NAND System Partition Table (Block 0, Page 2) to DRAM */
    RetVal = AmbaEMMC_ReadSysPTB(NULL, TimeOut);

    /* Check System Partition Table */
    AmbaMisra_TypeCast(&pBuf, &pAmbaRTSL_EmmcSysPartTable );
    Crc32 = IO_UtilityCrc32(pBuf, (UINT32) sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);

    if ((RetVal == OK) &&
        (pAmbaRTSL_EmmcSysPartTable ->CRC32 == Crc32) &&
        (pAmbaRTSL_EmmcSysPartTable ->BlkByteSize != 0U)) {
        RetVal = AmbaEMMC_GetActivePtbNo(&ActivePTB);
        /* if System Partion is OK */
        if (RetVal == OK) {
            RetVal = AmbaEMMC_ReadUserPTB(NULL, ActivePTB, TimeOut);
        }

        if (RetVal == OK) {
            /* Check System Partition Table */
            AmbaMisra_TypeCast(&pBuf, &pAmbaRTSL_EmmcUserPartTable );
            Crc32 = IO_UtilityCrc32(pBuf, (UINT32) AMBA_USER_PTB_CRC32_SIZE);

            if ((pAmbaRTSL_EmmcUserPartTable ->CRC32 == Crc32) &&
                (pAmbaRTSL_EmmcUserPartTable ->PtbMagic == AMBA_USER_PTB_MAGIC)) {
                RetVal = 0; /* USER PTB is OK */
            } else {
                CreatPTB = 1U; /* USER PTB is NG */
            }
        }
    } else {
        CreatPTB = 1U;
    }

    if ((RetVal != OK) || (CreatPTB != 0U)) {
        if (0U != AmbaWrap_memset(pAmbaRTSL_EmmcSysPartTable,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };
        if (0U != AmbaWrap_memset(pAmbaRTSL_EmmcUserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
        RetVal = AmbaEMMC_CreatePTB();
        if ((WritePtbFlag != 0U) && (RetVal == NVM_ERR_NONE)) {
            RetVal = AmbaEMMC_WriteSysPTB(NULL, TimeOut);
            if (RetVal == NVM_ERR_NONE) {
                RetVal |= AmbaEMMC_WriteUserPTB(NULL, 0, TimeOut);
            }

            if (RetVal == OK) {
                RetVal = AmbaEMMC_CreateExtUserPTB(&AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0].pUserPartConfig[0], pAmbaRTSL_EmmcUserPartTable, EmmcUserPtbNextBlock);
            }

            if ((WritePtbFlag == 1U) && (RetVal == OK)) {
                RetVal = AmbaEMMC_WriteUserPTB(NULL, 1U, TimeOut);
            }

            if (RetVal == OK) {
                RetVal = AmbaEMMC_ReadUserPTB(NULL, 0U, TimeOut);
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_CreatePTB
 *
 *  @Description:: Create NAND System and User Partition Tables
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_CreatePTB(void)
{
    const AMBA_PARTITION_CONFIG_s *pSysPartConfig  = AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0].pSysPartConfig;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0].pUserPartConfig;
    const AMBA_SD_CARD_INFO_s     *pCardInfo       = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0].CardInfo;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 i;
    UINT32 RetVal = NVM_ERR_NONE;
    UINT32 BlkByteSize;
    UINT32 BlkAddr, Stg0TotalSector, Misalign;
    UINT32 TotalNumBlk = pCardInfo->MemCardInfo.TotalSectors;   /* Total number of blocks */
    UINT32 EmmcBackupBlkCount = 0U;

    if ((pSysPartConfig == NULL) || (pUserPartConfig == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {

        /* Block size in Byte */
        BlkByteSize = 512U;
        pAmbaRTSL_EmmcSysPartTable ->BlkByteSize = 512U;

        /* Block-0: Reserved for Bootstrap & System Partition Table */
        /* next Block: Reserved for User Partition Table and Vendor Data, AMBA_USER_PARTITION_VENDOR_DATA must be 0 !!! */

        /* Configure sys partition: BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcSysPartTable ->Entry[AMBA_SYS_PARTITION_BOOTSTRAP]);
        IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pSysPartConfig->PartitionName);      /* Partition Name */
        pPartEntry->Attribute = pSysPartConfig->Attribute;    /* Attribute of the Partition */
        pPartEntry->ByteCount      = pSysPartConfig->ByteCount;         /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0U;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = 0U;                                 /* start Block Address = 0 */

        /* Block address of User Partition Table */
        BlkAddr = pPartEntry->BlkCount + GetRoundUpValU32((UINT32) sizeof(AMBA_SYS_PARTITION_TABLE_s), 512U);
        pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB = BlkAddr;

        /* Configure user partition USER_PTB */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_PTB]);
        IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pUserPartConfig->PartitionName);     /* Partition Name */
        pPartEntry->Attribute = pUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = pUserPartConfig->ByteCount;        /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0U;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = 1U;                                 /* number of Blocks for the Partition: fixed to be 1  */
        pPartEntry->StartBlkAddr   = BlkAddr;                           /* start Block Address = pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB */

        BlkAddr += pPartEntry->BlkCount;
        pAmbaRTSL_EmmcUserPartTable ->PTBNumber = 0;

        /* Configure user partition 2nd USER_PTB */
        BlkAddr += pPartEntry->BlkCount;

        /* Configure System Partition Table */
        /* Configure remaining sys partitions, except the partition of BOOTSTRAP */
        pSysPartConfig++;
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcSysPartTable ->Entry[1]);
        for (i = 1U; i < AMBA_NUM_SYS_PARTITION ; i++) {
            IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pSysPartConfig->PartitionName);      /* Partition Name */
            pPartEntry->Attribute = pSysPartConfig->Attribute;    /* Attribute of the Partition */
            pPartEntry->ByteCount      = pSysPartConfig->ByteCount;         /* number of Bytes for the Partition */
            pPartEntry->ActualByteSize = 0U;                                 /* actual size in Bytes */
            pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            pPartEntry->StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry->BlkCount;                /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */

            if (BlkAddr >= TotalNumBlk) {
                break;
            } else {
                pSysPartConfig++;
                pPartEntry++;
            }
        }

        /* Configure User Partition Table */
        /* Configure remaining user partitions, except the partition of VENDOR_DATA */
        pAmbaRTSL_EmmcUserPartTable ->PtbMagic = AMBA_USER_PTB_MAGIC;
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[0]);
        for (i = 1U; i < AMBA_NUM_USER_PARTITION; i++) {
            /* Partition Name */
            if (i == AMBA_USER_PARTITION_FAT_DRIVE_A) {
                continue;
            }

            IO_UtilityStringCopy((char *)pPartEntry[i].PartitionName, sizeof(pPartEntry[i].PartitionName), pUserPartConfig[i].PartitionName);
            pPartEntry[i].Attribute = pUserPartConfig[i].Attribute;   /* Attribute of the Partition */
            pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;        /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0U;                                 /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            if (pPartEntry[i].ByteCount == 0U) {
                pPartEntry[i].StartBlkAddr = 0U;
                pPartEntry[i].BlkCount = 0U;
                continue;
            }

            if ((pUserPartConfig[i].Attribute & AMBA_PARTITION_ATTR_BACKUP) != 0U) {
                EmmcBackupBlkCount += pPartEntry->BlkCount;
            }

            pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
        }

        /* Configure remind space for storage0 */
        /* Put it to the end of the device.    */
        EmmcUserPtbNextBlock = BlkAddr;
        BlkAddr += EmmcBackupBlkCount;

        Misalign = BlkAddr & (8192U - 1U);

        if (Misalign != 0U) {
            BlkAddr += (8192U - Misalign);
        }

        Stg0TotalSector = TotalNumBlk - BlkAddr;
        pUserPartConfig = &AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0].pUserPartConfig[AMBA_USER_PARTITION_FAT_DRIVE_A];
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);

        IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pUserPartConfig->PartitionName);
        pPartEntry->Attribute = pUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = (Stg0TotalSector * BlkByteSize);   /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = Stg0TotalSector;                   /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = BlkAddr;                           /* start Block Address */

    }

    return RetVal;
}

UINT32 AmbaEMMC_CreateExtUserPTB(const AMBA_PARTITION_CONFIG_s *pUserPartConfig, AMBA_USER_PARTITION_TABLE_s * pUserPartTable, UINT32 StartBlk)
{
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 i;
    const AMBA_SD_CARD_INFO_s *pCardInfo           = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0].CardInfo;
    UINT32 TotalNumBlk = pCardInfo->MemCardInfo.TotalSectors;
    UINT32 BlkAddr, Stg0TotalSector, BlkByteSize;
    const AMBA_PARTITION_CONFIG_s *pTmpUserPartConfig;
    UINT32 RetVal = OK;

    BlkAddr = StartBlk;
    pUserPartTable->PTBNumber = 1U;
    pUserPartTable->PTBActive = 0U;
    pUserPartTable->BlkAddrCurentPTB = StartBlk;
    pUserPartTable->BlkAddrNextPTB = 0U;

    pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pUserPartTable->Entry[0]);
    BlkByteSize = 512U;
    for (i = 1U; i < AMBA_NUM_USER_PARTITION; i++) {
        /* Partition Name */
        if (i == AMBA_USER_PARTITION_FAT_DRIVE_A) {
            continue;
        }

        if (((pUserPartConfig[i].Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) != AMBA_NVM_eMMC) {
            continue;
        }

        if ((pUserPartConfig[i].Attribute & AMBA_PARTITION_ATTR_BACKUP) == 0U) {
            continue;
        }

        IO_UtilityStringCopy((char *)pPartEntry[i].PartitionName, sizeof(pPartEntry[i].PartitionName), pUserPartConfig[i].PartitionName);
        pPartEntry[i].Attribute = pUserPartConfig[i].Attribute;   /* Attribute of the Partition */
        pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;        /* number of Bytes for the Partition */
        pPartEntry[i].ActualByteSize = 0U;                                 /* actual size in Bytes */
        pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

        if (pPartEntry[i].ByteCount == 0U) {
            pPartEntry[i].StartBlkAddr = 0U;
            pPartEntry[i].BlkCount = 0U;
            continue;
        }

        pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
        BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */

        if (BlkAddr > TotalNumBlk) {
            RetVal = AMBA_SD_ERR_OTHER;
        }
        if (BlkAddr >= TotalNumBlk) {
            break;
        }
    }

    if (RetVal == OK) {
        /* Configure remind space for storage0 */
        /* Put it to the end of the device.    */
        Stg0TotalSector = TotalNumBlk - BlkAddr;
        pTmpUserPartConfig = & pUserPartConfig[AMBA_USER_PARTITION_FAT_DRIVE_A];
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pUserPartTable->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);

        IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pTmpUserPartConfig->PartitionName);
        pPartEntry->Attribute = pTmpUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = (Stg0TotalSector * BlkByteSize);   /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = Stg0TotalSector;                   /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = BlkAddr;                           /* start Block Address */
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_LoadNvmRomFileTable
 *
 *  @Description:: Load all NAND ROM File Tables
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_LoadNvmRomFileTable(void)
{
    UINT32 SectorCount, SectorSize;
    const AMBA_PARTITION_ENTRY_s *pPartEntry;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 RetVal;

    SectorSize = 512U;

    /* Load DSP uCODE ROM table */
    pPartEntry = AmbaRTSL_EmmcGetPartEntry(1U, AMBA_USER_PARTITION_DSP_uCODE);
    SectorCount = ((UINT32) sizeof(AmbaEMMC_RomDspUcodeCtrl) + SectorSize - 1U) / SectorSize;

    SecConfig.NumSector   = SectorCount;
    SecConfig.pDataBuf    = (UINT8 *) &AmbaEMMC_RomDspUcodeCtrl;
    SecConfig.StartSector = pPartEntry->StartBlkAddr;

    RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);

    if (RetVal == NVM_ERR_NONE) {
        /* Load SYS DATA ROM table */
        pPartEntry = AmbaRTSL_EmmcGetPartEntry(1U, AMBA_USER_PARTITION_SYS_DATA);
        SectorCount = ((UINT32) sizeof(AmbaEMMC_RomSysDataCtrl) + SectorSize - 1U) / SectorSize;

        SecConfig.NumSector   = SectorCount;
        SecConfig.pDataBuf    = (UINT8 *) &AmbaEMMC_RomSysDataCtrl;
        SecConfig.StartSector = pPartEntry->StartBlkAddr;

        RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
    }

    return RetVal;
}

static UINT32 FileNameCmp(const char* pFile1, const char *pFile2)
{
    SIZE_t FileLen1, FileLen2;
    UINT32 Find = 0U;

    FileLen1 = IO_UtilityStringLength(pFile1);
    FileLen2 = IO_UtilityStringLength(pFile2);

    if (0 == IO_UtilityStringCompare(pFile1, pFile2, IO_UtilityStringLength(pFile2))) {
        if (FileLen1 == FileLen2) {
            Find = 1U;
        }
    }
    return Find;
}

static AMBA_NVM_ROM_FILE_ENTRY_s* AmbaEMMC_GetRomFileEntry(UINT32 RegionID, const char *pFileName)
{
    AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile = &(AmbaEMMC_RomDspUcodeCtrl.Header.FileEntry[0]);
    UINT32 FileCount = 0U, IndexMax = 0U, Index = 0U, Find = 0U;

    if ((RegionID < AMBA_NUM_NVM_ROM_REGION) && (pFileName != NULL)) {

        if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
            pRomFile  = &(AmbaEMMC_RomDspUcodeCtrl.Header.FileEntry[0]);
            FileCount = AmbaEMMC_RomDspUcodeCtrl.Header.FileCount;
            IndexMax  = AMBA_NVM_MAX_NUM_DSP_uCODE_ROM_FILE;
        } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
            pRomFile  = &(AmbaEMMC_RomSysDataCtrl.Header.FileEntry[0]);
            FileCount = AmbaEMMC_RomSysDataCtrl.Header.FileCount;
            IndexMax  = AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE;
        } else {
            /* for misraC check */
        }

        for (UINT32 i = 0U; i < FileCount; i += 1U) {
            if (Index < IndexMax) {
                if (FileNameCmp(pFileName, (const char *)pRomFile[Index].FileName) == 1U) {
                    Find = 1U;
                    pRomFile = &pRomFile[Index];
                    break;
                }
                Index += 1U;
            }
        }
    }

    if (Find == 0U) {
        pRomFile = NULL;
    }
    return pRomFile;
}

/*
 *  @RoutineName:: AmbaNAND_GetRomFileSize
 *
 *  @Description:: Get the size of the specific file in ROM partition
 *
 *  @Input      ::
 *      RegionID:  ROM Region ID
 *      pFileName: pointer to the Filename
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : File size (>0)/NG(-1)
 */
UINT32 AmbaEMMC_GetRomFileSize(UINT32 RegionID, const char *pFileName, UINT32 *pFileSize)
{
    UINT32 RetVal = NVM_ERR_FILE_NOT_FOUND;
    const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile;

    (*pFileSize) = 0;

    pRomFile = AmbaEMMC_GetRomFileEntry(RegionID, pFileName);
    if (pRomFile != NULL) {
        (*pFileSize) = pRomFile->ByteSize;
        RetVal = NVM_ERR_NONE;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_GetRomFileInfo
 *
 *  @Description:: Get the file info of the specific file in ROM partition form index
 *
 *  @Input      ::
 *      RegionID: ROM Region ID
 *      Index:    Index to the ROM File
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : (>0)/NG(-1)
 */
UINT32 AmbaEMMC_GetRomFileInfo(UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo)
{
    const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile = NULL;
    UINT32 RetVal = NVM_ERR_NONE;

    if ((RegionID >= AMBA_NUM_NVM_ROM_REGION) || (Index > AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        if ((RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) && (Index < AMBA_NVM_MAX_NUM_DSP_uCODE_ROM_FILE)) {
            pRomFile = &(AmbaEMMC_RomDspUcodeCtrl.Header.FileEntry[Index]);
        } else if ((RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) && (Index < AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE)) {
            pRomFile = &(AmbaEMMC_RomSysDataCtrl.Header.FileEntry[Index]);
        } else {
            /* for misraC checking, do nothing */
        }

        if ((pRomFileInfo != NULL) && (pRomFile != NULL)) {
            if (IO_UtilityStringLength((const char *)pRomFile->FileName) == 0U) {
                RetVal = NVM_ERR_FILE_NOT_FOUND;  /* no such file */
            } else {
                if ((pRomFile->ByteSize  == 0U) || (pRomFile->ByteSize  == 0xffffffffU) ||
                    (pRomFile->FileCRC32 == 0U) || (pRomFile->FileCRC32 == 0xffffffffU)) {
                    RetVal = NVM_ERR_FILE_NOT_FOUND;  /* no such file */
                } else {
                    IO_UtilityStringCopy(pRomFileInfo->FileName, IO_UtilityStringLength((const char *)pRomFile->FileName), (const char *)pRomFile->FileName);
                    pRomFileInfo->ByteSize  = pRomFile->ByteSize;
                    pRomFileInfo->FileCRC32 = pRomFile->FileCRC32;
                }
            }
        }

    }

    return RetVal;
}

static UINT32 CheckReadPos(const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile, UINT32 ReadPos, UINT32 Size)
{
    UINT32 RetVal;

    if (pRomFile == NULL) {
        RetVal = NVM_ERR_FILE_NOT_FOUND; /* no such file or wrong file position */
    } else if ((ReadPos > pRomFile->ByteSize) || (Size > (pRomFile->ByteSize - ReadPos))) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        RetVal = NVM_ERR_NONE;
    }
    return RetVal;
}

static const AMBA_PARTITION_ENTRY_s * AmbaEMMC_GetEntry(UINT32 RegionID)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_DSP_uCODE);

    if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
        pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_DSP_uCODE);
    } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
        pPartEntry = AmbaRTSL_EmmcGetPartEntry(1, AMBA_USER_PARTITION_SYS_DATA);
    } else {
        pPartEntry = NULL;
    }
    return pPartEntry;
}

/*
 *  @RoutineName:: AmbaEMMC_ReadRomFile
 *
 *  @Description:: Read a specific file from ROM partiton
 *
 *  @Input      ::
 *      RegionID:  ROM Region ID
 *      StartPos:  Byte position of the file
 *      Size:      Byte count to be read
 *      pFileName: Filename
 *      pDataBuf:  Data buffer to store read data
 *      TimeOut:   Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_ReadRomFile(UINT32 RegionID, const char *pFileName, UINT32 StartPos,
                            UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut)
{
    const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile = NULL;
    const AMBA_PARTITION_ENTRY_s *pPartEntry;
    static UINT8 AmbaEMMC_SectorBuf[2048] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 DataSize;
    UINT32 ReadSize = Size;
    UINT32 PageAddr, SectorSize, Pages, FirstPageSize, Index = 0U, ReadPos = StartPos;
    UINT32 RetVal = NVM_ERR_NONE;

    (void) TimeOut;

    pPartEntry = AmbaEMMC_GetEntry(RegionID);

    if ((pFileName == NULL) || (pDataBuf == NULL) || (pPartEntry == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {

        SectorSize = 512U;

        pRomFile = AmbaEMMC_GetRomFileEntry(RegionID, pFileName);

        RetVal = CheckReadPos(pRomFile, ReadPos, Size);

        if ((pRomFile != NULL) && (RetVal == NVM_ERR_NONE)) {

            if (ReadSize > (pRomFile->ByteSize - ReadPos)) {
                ReadSize = pRomFile->ByteSize - ReadPos;
            }

            /* Convert StartPos from file position to ROM partition position */
            PageAddr = (ReadPos + pRomFile->FileOffset) / SectorSize;
            ReadPos %= SectorSize;

            if (ReadPos == 0U) {
                FirstPageSize = 0U;
            } else {
                ReadPos %= SectorSize;
                FirstPageSize = SectorSize - ReadPos;
            }

            PageAddr += pPartEntry->StartBlkAddr;

            while((ReadSize > 0U) && (RetVal == NVM_ERR_NONE) && (PageAddr < (pPartEntry->StartBlkAddr + pPartEntry->BlkCount))) {

                Pages = 1U;

                if (FirstPageSize != 0U) {
                    SecConfig.NumSector   = 1;
                    SecConfig.pDataBuf    = (UINT8 *) AmbaEMMC_SectorBuf;
                    SecConfig.StartSector = PageAddr;

                    RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
                    if (FirstPageSize >= ReadSize) {
                        if (0U != AmbaWrap_memcpy(pDataBuf, &AmbaEMMC_SectorBuf[ReadPos], ReadSize)) { /* Misrac */ };
                        break;
                    }
                    if (0U != AmbaWrap_memcpy(pDataBuf, &AmbaEMMC_SectorBuf[ReadPos], FirstPageSize)) { /* Misrac */ }
                    PageAddr += 1U;
                    ReadSize     -= FirstPageSize;
                    Index    += FirstPageSize;
                    //ReadPos -= FirstPageSize;
                    FirstPageSize = 0;
                }

                if (ReadSize >= SectorSize) {
                    Pages = ReadSize / SectorSize;
                    DataSize = Pages * SectorSize;
                } else {
                    DataSize = ReadSize;
                }

                /* Read one page size from file */
                if (DataSize >= SectorSize) {
                    SecConfig.NumSector   = Pages;
                    SecConfig.pDataBuf    = (UINT8 *) &pDataBuf[Index];
                    SecConfig.StartSector = PageAddr;

                    RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
                } else {
                    SecConfig.NumSector   = 1U;
                    SecConfig.pDataBuf    = (UINT8 *) AmbaEMMC_SectorBuf;
                    SecConfig.StartSector = PageAddr;

                    RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
                    if (0U != AmbaWrap_memcpy(&pDataBuf[Index], AmbaEMMC_SectorBuf, DataSize)) { /* Misrac */ };
                }

                ReadSize -= DataSize;
                Index += DataSize;

                ReadPos = 0;
                PageAddr += Pages;
            }

            if (PageAddr >= (pPartEntry->StartBlkAddr + pPartEntry->BlkCount)) {
                RetVal = NVM_ERR_ARG;    /* out of partition range */
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_GetFtlInfo
 *
 *  @Description:: Get NAND FTL informatio of the partition
 *
 *  @Input      ::
 *      NvmID:      Memory type of the ROM partiton
 *      UserPartID: User Partition ID
 *      pFtlInfo:   pointer to the data buffer oof FTL information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pFtlInfo)
{
    const AMBA_SD_CTRL_s            *pSdCtrl = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0];
    const AMBA_SD_CARD_INFO_s *pCardInfo     = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0].CardInfo;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[UserPartID]);
    UINT32 RetVal = NVM_ERR_NONE;

    if ((UserPartID >= AMBA_NUM_USER_PARTITION) || ((pPartEntry->Attribute & AMBA_PARTITION_ATTR_FTL) == 0U)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        pFtlInfo->Initialized  = pSdCtrl->CardInfo.MemCardInfo.CardIsInit; /* 1 - this partion has been initialized */
        pFtlInfo->WriteProtect = pCardInfo->MemCardInfo.WriteProtect;   /* 1 - this partition is write protected */

        /* Total size of the FTL partition in Sectors */
        pFtlInfo->TotalSizeInSectors = pPartEntry->BlkCount;
        pFtlInfo->SectorSizeInBytes  = 512;         /* Sector size in Bytes */

        pFtlInfo->TotalSizeInBytes = (UINT64)(pFtlInfo->TotalSizeInSectors) * pFtlInfo->SectorSizeInBytes;
        /* Total size of the FTL partition in Bytes */

        pFtlInfo->PageSizeInBytes   = 512;           /* Page size in Bytes */
        pFtlInfo->BlockSizeInPages  = 1;           /* Block size in Pages */
        pFtlInfo->EraseSizeInBlocks = 1;             /* Erase size in Blocks */
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_ReadSysPTB
 *
 *  @Description:: Read NAND System Partition Table (Block 0, Page 2)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the buffer of System partition table
 *      TimeOut:       Time out value
 *
 *  @Output     ::
 *      pSysPartTable: pointer to the data of System partition table
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_ReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    const AMBA_RTSL_SD_CTRL_s *pSdCtrl = (AMBA_RTSL_SD_CTRL_s *) & (AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0]);
    UINT8 *pWorkBufMain = AmbaRTSL_SdWorkBuf;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    UINT32 RetVal;

    (void) TimeOut;

    /* System partition table is located at block0 page2 (following BST pages) */
    SecConfig.NumSector   = pSdCtrl->SysPtblPageCount;
    SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
    SecConfig.StartSector = pSdCtrl->BstPageCount;

    RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
    if (RetVal == OK) {
        /* copy to the user area while needed */
        if (pSysPartTable != NULL) {
            if (0U != AmbaWrap_memcpy(pSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };
        } else {
            /* copy to the DRAM memory */
            if (0U != AmbaWrap_memcpy(pAmbaRTSL_EmmcSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_WriteSysPTB
 *
 *  @Description:: Write NAND System Partition Table (Block 0, Page 2)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the data of System partition table
 *      TimeOut:       Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_WriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    const AMBA_RTSL_SD_CTRL_s *pSdCtrl = (AMBA_RTSL_SD_CTRL_s *) & (AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0]);
    UINT32 BstPageCount     = pSdCtrl->BstPageCount;      /* Number of Pages for BST */
    UINT32 SysPtblPageCount = pSdCtrl->SysPtblPageCount;  /* Number of Pages for System Partition Table */
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    AMBA_SYS_PARTITION_TABLE_s *pWorkSysPartTable;
    UINT32 RetVal;
    const UINT8 *pBuf;

    (void) TimeOut;

    if (pSysPartTable != NULL) {
        /* update EMMC through User defined Partition Table */
        pWorkSysPartTable = pSysPartTable;
    } else {
        /* update EMMC through DRAM Partition Table */
        pWorkSysPartTable = pAmbaRTSL_EmmcSysPartTable ;
    }

    /* 1. Calculate CRC32 */
    AmbaMisra_TypeCast(&pBuf, &pWorkSysPartTable);
    pWorkSysPartTable->CRC32 = IO_UtilityCrc32(pBuf, (UINT32) sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);

    /* 2. Write new System Partition Table */
    SecConfig.NumSector   = SysPtblPageCount;
    SecConfig.pDataBuf    = (UINT8 *) pWorkSysPartTable;
    SecConfig.StartSector = BstPageCount;

    RetVal = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_ReadUserPTB
 *
 *  @Description:: Read NAND User Partition Table (Block 1, Page 0)
 *
 *  @Input      ::
 *      pUserPartTable: pointer to the buffer of User partition table
 *      TimeOut:        Time out value
 *
 *  @Output     ::
 *      pUserPartTable: pointer to the data of User partition table
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_ReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    const AMBA_RTSL_SD_CTRL_s *pSdCtrl = (AMBA_RTSL_SD_CTRL_s *) & (AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0]);
    UINT8 *pWorkBufMain = AmbaRTSL_SdWorkBuf;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    UINT32 RetVal;

    (void) TimeOut;

    /* Read User partition table from NAND */
    SecConfig.NumSector   = pSdCtrl->UserPtblPageCount;
    SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
    SecConfig.StartSector = pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB;

#if defined(CONFIG_MUTI_BOOT_DEVICE)
    if(UserPtbNo == 0U) {
        SecConfig.StartSector = 0U;
    } else {
        SecConfig.StartSector = pSdCtrl->UserPtblPageCount;
    }
#else
    if(UserPtbNo != 0U) {
        SecConfig.StartSector += SecConfig.NumSector;
    }
#endif

    RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);

    if (RetVal == OK) {
        /* copy to the user area while needed */
        if (pUserPartTable != NULL) {
            if (0U != AmbaWrap_memcpy(pUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
        } else {
            /* copy to the DRAM memory */
            if (0U != AmbaWrap_memcpy(pAmbaRTSL_EmmcUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_WriteUserPTB
 *
 *  @Description:: Write NAND User Partition Table (Block 1, Page 0)
 *
 *  @Input      ::
 *      pUserPartTable: pointer to the data of User partition table
 *      TimeOut:        Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_WriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    const AMBA_RTSL_SD_CTRL_s *pSdCtrl = (AMBA_RTSL_SD_CTRL_s *) & (AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0]);
    AMBA_USER_PARTITION_TABLE_s *pWorkUserPartTable;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    const UINT8 *pTmp = NULL;
    UINT32 RetVal;

    (void) TimeOut;

    if (pUserPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkUserPartTable = pUserPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkUserPartTable = pAmbaRTSL_EmmcUserPartTable ;
    }

    /* Calculate CRC32 */
    AmbaMisra_TypeCast32(&pTmp, &pWorkUserPartTable);
    pWorkUserPartTable->CRC32 = IO_UtilityCrc32(pTmp, (UINT32) AMBA_USER_PTB_CRC32_SIZE);

    /* Write new User partition table (block 1, page 0) */
    SecConfig.NumSector   = pSdCtrl->UserPtblPageCount;
    SecConfig.pDataBuf    = (UINT8 *) pWorkUserPartTable;

#if defined(CONFIG_MUTI_BOOT_DEVICE)
    if(UserPtbNo == 0U) {
        SecConfig.StartSector = 0U;
    } else {
        SecConfig.StartSector = pSdCtrl->UserPtblPageCount;
    }
#else
    SecConfig.StartSector = pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB + (UserPtbNo * pSdCtrl->UserPtblPageCount);
#endif

    RetVal = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_ReadPartitionInfo
 *
 *  @Description:: Read partition information
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *      pPartInfo: pointer to partition information
 *
 *  @Return     ::
 *          int : actual size of Vendor Specific Data (>0)/NG(-1)
 */
UINT32 AmbaEMMC_ReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    UINT32 RetVal = NVM_ERR_NONE;

    if (AmbaRTSL_EmmcReadPartitionInfo(PartFlag, PartID, pPartInfo) != 0) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_ReadPartition
 *
 *  @Description:: Read partition data from NAND flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *      TimeOut:  Time out value
 *
 *  @Output     ::
 *      pDataBuf: pointer to partition data
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_ReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 BlkCount, BlkByteSize = 512;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    UINT32 RetVal = NVM_ERR_NONE;

    (void) TimeOut;

    if ((pDataBuf == NULL) || (pPartEntry == NULL) ||
        (pPartEntry->ActualByteSize == 0U) || (pPartEntry->ImageCRC32 == 0xffffffffU)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
#if (FLASH_PROG_EMMC_BOOT_PART != EMMC_BOOTP_USER)
        if ((PartFlag == 0U) && (PartID == AMBA_SYS_PARTITION_BOOTSTRAP)) {
            AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};
            ArgCmd6.Access = ACCESS_WRITE_BYTE;
            ArgCmd6.Index  = BOOT_CONFIG;
            ArgCmd6.Value  = (FLASH_PROG_EMMC_BOOT_PART | EMMC_CMD6_VALUE_ACCP);

            RetVal = AmbaSD_SendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
            if (RetVal != OK) {
                RetVal = NVM_ERR_eMMC_READ;
            }
        }
        if (RetVal == NVM_ERR_NONE) {
#endif
            BlkCount = (pPartEntry->ActualByteSize + BlkByteSize - 1U) / BlkByteSize;
            if (BlkCount > pPartEntry->BlkCount) {
                BlkCount = pPartEntry->BlkCount;  /* should never happen */
            }

            SecConfig.NumSector   = BlkCount;
            SecConfig.pDataBuf    = (UINT8 *) pDataBuf;
            SecConfig.StartSector = pPartEntry->StartBlkAddr;

            RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
#if (FLASH_PROG_EMMC_BOOT_PART != EMMC_BOOTP_USER)
        }
        if ((RetVal == NVM_ERR_NONE) &&
            (PartFlag == 0U) && (PartID == AMBA_SYS_PARTITION_BOOTSTRAP)) {
            AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};
            ArgCmd6.Access = ACCESS_WRITE_BYTE;
            ArgCmd6.Index  = BOOT_CONFIG;
            ArgCmd6.Value  = (FLASH_PROG_EMMC_BOOT_PART | EMMC_ACCP_USER);

            RetVal = AmbaSD_SendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
            if (RetVal != OK) {
                RetVal = NVM_ERR_eMMC_READ;
            }
        }
#endif
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_WritePartition
 *
 *  @Description:: Write partition data from NAND flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *      DataSize: Data size in Byte to write to the partition
 *      pDataBuf: pointer to partition data
 *      TimeOut:  Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_WritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 BlkCount, BlkByteSize = 512U;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    UINT32 RetVal = NVM_ERR_NONE;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;

    if ((DataSize == 0U) || (pDataBuf == NULL) || (pPartEntry == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {

#if (FLASH_PROG_EMMC_BOOT_PART != EMMC_BOOTP_USER)
        if ((PartFlag == 0U) && (PartID == AMBA_SYS_PARTITION_BOOTSTRAP)) {
            AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};
            ArgCmd6.Access = ACCESS_WRITE_BYTE;
            ArgCmd6.Index  = BOOT_CONFIG;
            ArgCmd6.Value  = (FLASH_PROG_EMMC_BOOT_PART | EMMC_CMD6_VALUE_ACCP);

            RetVal = AmbaSD_SendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
            if (RetVal != OK) {
                RetVal = NVM_ERR_eMMC_WRITE;
            }
        }
        if (RetVal == OK) {
#endif
            BlkCount = (DataSize + BlkByteSize - 1U) / BlkByteSize;

            /* 1. Program blocks in target partition */
            SecConfig.NumSector   = BlkCount;
            SecConfig.pDataBuf    = (UINT8 *) pDataBuf;
            SecConfig.StartSector = pPartEntry->StartBlkAddr;

            RetVal = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);
#if (FLASH_PROG_EMMC_BOOT_PART != EMMC_BOOTP_USER)
        }
        if ((RetVal == OK) &&
            (PartFlag == 0U) && (PartID == AMBA_SYS_PARTITION_BOOTSTRAP)) {
            AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};
            ArgCmd6.Access = ACCESS_WRITE_BYTE;
            ArgCmd6.Index  = BOOT_CONFIG;
            ArgCmd6.Value  = (FLASH_PROG_EMMC_BOOT_PART | EMMC_ACCP_USER);

            RetVal = AmbaSD_SendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
            if (RetVal != OK) {
                RetVal = NVM_ERR_eMMC_WRITE;
            }
        }
#endif
        /* 2. Update partition table */
        if (RetVal == OK) {
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
            pPartEntry->ActualByteSize = DataSize;
            pPartEntry->ProgramStatus = 0;
            pPartEntry->ImageCRC32 = IO_UtilityCrc32(pDataBuf, DataSize);

            if (PartFlag != 0U) {
                /* User Partition */
                RetVal = AmbaEMMC_WriteUserPTB(NULL, pAmbaRTSL_EmmcUserPartTable ->PTBNumber, TimeOut);

                /* Sync partition info if the partition is nonbackup partition */
                if ((CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_BACKUP) == 0x0U) && (RetVal == OK)) {
                    /* Switch to the other user PTB */
                    if (pAmbaRTSL_EmmcUserPartTable ->PTBNumber == 0U) {
                        RetVal = AmbaEMMC_ReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 1U, TimeOut);
                    } else {
                        RetVal = AmbaEMMC_ReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 0U, TimeOut);
                    }

                    pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
                    pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
                    pPartEntry->ActualByteSize = DataSize;
                    pPartEntry->ProgramStatus = 0;
                    pPartEntry->ImageCRC32 = IO_UtilityCrc32(pDataBuf, DataSize);

                    if (RetVal == OK) {
                        /* Update the user PTB */
                        RetVal = AmbaEMMC_WriteUserPTB(NULL, pAmbaRTSL_EmmcUserPartTable ->PTBNumber, TimeOut);
                    }

                    if (RetVal == OK) {
                        /* Switch to the original user PTB */
                        if (pAmbaRTSL_EmmcUserPartTable ->PTBNumber == 0U) {
                            RetVal = AmbaEMMC_ReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 1U, TimeOut);
                        } else {
                            RetVal = AmbaEMMC_ReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 0U, TimeOut);
                        }
                    }
                }

            } else {
                /* System Partition */
                RetVal = AmbaEMMC_WriteSysPTB(NULL, TimeOut);
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_InvalidatePartition
 *
 *  @Description:: Invalidate the partition
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *      TimeOut:  Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_InvalidatePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);

    if (pPartEntry == NULL) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {

        if ((pPartEntry->Attribute & AMBA_PARTITION_ATTR_INVALID) != 0U) {
            RetVal = NVM_ERR_NONE;
        } else {

            pPartEntry->Attribute = SetBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);

            if (PartFlag != 0U) {
                /* User Partition */
                RetVal = AmbaEMMC_WriteUserPTB(NULL, pAmbaRTSL_EmmcUserPartTable ->PTBNumber, TimeOut);
            } else {
                /* System Partition */
                RetVal = AmbaEMMC_WriteSysPTB(NULL, TimeOut);
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_ErasePartition
 *
 *  @Description:: Erase a partition
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *      TimeOut:  Time out value
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_ErasePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
    UINT32 RetVal = NVM_ERR_NONE;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    UINT32 BlkAddr, BlkCount, EraseSecCount = AmbaRTSL_SdGetWorkBufferSize() / 512U;
    UINT8 *pWorkBufMain = AmbaRTSL_SdWorkBuf;

    if (pPartEntry == NULL) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        if (0U != AmbaWrap_memset(AmbaRTSL_SdWorkBuf, 0x0, AmbaRTSL_SdGetWorkBufferSize())) { /* Misrac */ };

        /* 1. Erase all blocks in target partition */
        BlkCount = pPartEntry->BlkCount;
        BlkAddr = pPartEntry->StartBlkAddr;

        while(BlkCount > 0U) {

            if (BlkCount < EraseSecCount) {
                EraseSecCount = BlkCount;
            }

            SecConfig.NumSector   = EraseSecCount;
            SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
            SecConfig.StartSector = BlkAddr;

            (void) AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);

            if (BlkCount >= EraseSecCount) {
                BlkCount -= EraseSecCount;
            } else {
                break;
            }
            BlkAddr += SecConfig.NumSector;
        }

        /* 2. Update partition table */
        pPartEntry->Attribute = SetBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);  /* 1 - Erased */
        pPartEntry->ActualByteSize = 0;
        pPartEntry->ImageCRC32 = 0xffffffffU;
        pPartEntry->ProgramStatus = 0xffffffffU;

        if (PartFlag != 0U) {
            /* User Partition */
            RetVal = AmbaEMMC_WriteUserPTB(NULL, pAmbaRTSL_EmmcUserPartTable ->PTBNumber, TimeOut);

            /* Sync partition info if the partition is nonbackup partition */
            if ((CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_BACKUP) == 0x0U) && (RetVal == OK)) {
                /* Switch to the other user PTB */
                if (pAmbaRTSL_EmmcUserPartTable ->PTBNumber == 0U) {
                    RetVal = AmbaEMMC_ReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 1U, TimeOut);
                } else {
                    RetVal = AmbaEMMC_ReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 0U, TimeOut);
                }

                pPartEntry->Attribute = SetBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);  /* 1 - Erased */
                pPartEntry->ActualByteSize = 0;
                pPartEntry->ImageCRC32 = 0xffffffffU;
                pPartEntry->ProgramStatus = 0xffffffffU;

                if (RetVal == OK) {
                    /* Update the user PTB */
                    RetVal = AmbaEMMC_WriteUserPTB(NULL, pAmbaRTSL_EmmcUserPartTable ->PTBNumber, TimeOut);
                }

                if (RetVal == OK) {
                    /* Switch to the original user PTB */
                    if (pAmbaRTSL_EmmcUserPartTable ->PTBNumber == 0U) {
                        RetVal = AmbaEMMC_ReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 1U, TimeOut);
                    } else {
                        RetVal = AmbaEMMC_ReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 0U, TimeOut);
                    }
                }
            }
        } else {
            /* System Partition */
            RetVal = AmbaEMMC_WriteSysPTB(NULL, TimeOut);
        }
    }

    return RetVal;
}

/*
 *  AmbaEMMC_WritePhyBlock - write a phyical sector
 *  @param[in] BlkAddr
 *  @param[in] pDataBuf
 *  @param[in] TimeOut
 *  @return error code
 */
UINT32 AmbaEMMC_WritePhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = NVM_ERR_NONE;
    const AMBA_SD_CARD_INFO_s *pCardInfo           = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0].CardInfo;
    UINT32 TotalNumBlk = pCardInfo->MemCardInfo.TotalSectors;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;

    if ((BlkAddr >= TotalNumBlk) || (pDataBuf == NULL)) {
        RetVal = NVM_ERR_ARG;
    } else {
        SecConfig.NumSector   = 1U;
        SecConfig.pDataBuf    = (UINT8 *) pDataBuf;
        SecConfig.StartSector = BlkAddr;

        RetVal = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);
    }

    (void) TimeOut;

    return RetVal;
}

/*
 *  AmbaEMMC_ReadPhyBlock - Read a phyical sector
 *  @param[in] BlkAddr
 *  @param[out] pDataBuf
 *  @param[in] TimeOut
 *  @return error code
 */
UINT32 AmbaEMMC_ReadPhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = NVM_ERR_NONE;
    const AMBA_SD_CARD_INFO_s *pCardInfo           = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0].CardInfo;
    UINT32 TotalNumBlk = pCardInfo->MemCardInfo.TotalSectors;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;

    if ((BlkAddr >= TotalNumBlk) || (pDataBuf == NULL)) {
        RetVal = NVM_ERR_ARG;
    } else {
        SecConfig.NumSector   = 1U;
        SecConfig.pDataBuf    = (UINT8 *) pDataBuf;
        SecConfig.StartSector = BlkAddr;

        RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
    }

    (void) TimeOut;

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_IsBldMagicCodeSet
 *
 *  @Description:: check if Bootloader Magic Code has been set for System Software load or not
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : Yes(1)/No(0)
 */
UINT32 AmbaEMMC_IsBldMagicCodeSet(void)
{
#if 0
    return AmbaRTSL_EmmcIsBldMagicCodeSet();
#endif
    return 0;
}

/*
 *  @RoutineName:: AmbaRTSL_NandSetBldMagicCode
 *
 *  @Description:: Write Bootloader Magic Code onto NAND device
 *
 *  @Input      ::
 *      TimeOut: Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_SetBldMagicCode(UINT32 TimeOut)
{
    UINT32 RetVal = OK;
#if 0
    UINT32 *pBldMagicCode = (UINT32 *) AMBA_ROM_WARM_BOOT_MAGIC_CODE_ADDR;
    int RetVal = OK;

    if (pAmbaRTSL_EmmcUserPartTable ->BldMagicCode != *pBldMagicCode) {
        /* Write Bootloader Magic Code */
        pAmbaRTSL_EmmcUserPartTable ->BldMagicCode = *pBldMagicCode; /* Bootloader Magic Code */
        RetVal = AmbaEMMC_WriteUserPTB(NULL, 0, TimeOut);
    }
#endif
    (void) TimeOut;

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NandEraseBldMagicCode
 *
 *  @Description:: Erase Bootloader Magic Code from NAND device
 *
 *  @Input      ::
 *      TimeOut: Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaEMMC_EraseBldMagicCode(UINT32 TimeOut)
{
    UINT32 RetVal = OK;
#if 0
    UINT32 *pBldMagicCode = (UINT32 *) AMBA_ROM_WARM_BOOT_MAGIC_CODE_ADDR;
    int RetVal = OK;

    if (pAmbaRTSL_EmmcUserPartTable ->BldMagicCode == *pBldMagicCode) {
        /* Erase Bootloader Magic Code */
        pAmbaRTSL_EmmcUserPartTable ->BldMagicCode = 0xffffffffU; /* Bootloader Magic Code */
        RetVal = AmbaEMMC_WriteUserPTB(NULL, 0, TimeOut);
    }
#endif
    (void) TimeOut;

    return RetVal;
}

/*
 *  @RoutineName:: AmbaEMMC_ReadBldMagicCode
 *
 *  @Description:: Read Bootloader Magic Code
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Bootloader Magic Code
 */
UINT32 AmbaEMMC_ReadBldMagicCode(void)
{
#if 0
    return AmbaRTSL_EmmcReadBldMagicCode();
#endif
    return 0;
}

void AmbaEMMC_SetWritePtbFlag(UINT32 Flag)
{
    WritePtbFlag = Flag;
}

UINT32 AmbaEMMC_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal = NVM_ERR_NONE;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[UserPartID]);
    AMBA_NVM_SECTOR_CONFIG_s SecConfig = {0};

    (void) TimeOut;

    if (UserPartID >= AMBA_NUM_USER_PARTITION) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        if (0U != AmbaWrap_memcpy(&SecConfig, pSecConfig, sizeof(AMBA_NVM_SECTOR_CONFIG_s))) { /* Misrac */ };

        SecConfig.StartSector += pPartEntry->StartBlkAddr;

        if (SecConfig.StartSector > (pPartEntry->StartBlkAddr + pPartEntry->BlkCount)) {
            RetVal = NVM_ERR_ARG;  /* wrong parameters */
        } else {
            RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
        }
    }

    return RetVal;
}

UINT32 AmbaEMMC_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal = NVM_ERR_NONE;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[UserPartID]);
    AMBA_NVM_SECTOR_CONFIG_s SecConfig = {0};

    (void) TimeOut;

    if (UserPartID >= AMBA_NUM_USER_PARTITION) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        if (0U != AmbaWrap_memcpy(&SecConfig, pSecConfig, sizeof(AMBA_NVM_SECTOR_CONFIG_s))) { /* Misrac */ };

        SecConfig.StartSector += pPartEntry->StartBlkAddr;

        if (SecConfig.StartSector > (pPartEntry->StartBlkAddr + pPartEntry->BlkCount)) {
            RetVal = NVM_ERR_ARG;  /* wrong parameters */
        } else {
            RetVal = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);
        }
    }

    return RetVal;
}

