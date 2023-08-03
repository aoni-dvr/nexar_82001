/**
 *  @file AmbaRTSL_EMMC_Partition.c
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
 *  @details NAND Partition Management APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"

#include "AmbaIOUtility.h"

#include "AmbaRTSL_SD.h"

#include "AmbaSD_Ctrl.h"

#define BOOT_CONFIG     179U

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

static UINT32 WritePtbFlag = 0;

/* pointer to System Partition Table */
AMBA_SYS_PARTITION_TABLE_s  *pAmbaRTSL_EmmcSysPartTable  = NULL;

/* pointer to User Partition Table */
AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_EmmcUserPartTable  = NULL;

AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_EmmcSysPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
AMBA_USER_PARTITION_TABLE_s AmbaRTSL_EmmcUserPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT32 Rtsl_EmmcUserPtbNextBlock = 0U;

/*
 *  @RoutineName:: AmbaRTSL_EmmcGetActivePtbNo
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
INT32 AmbaRTSL_EmmcGetActivePtbNo(UINT32 * pPTBActive)
{
    UINT32 i;
    INT32 RetVal;

    *pPTBActive = 0U;

    for(i = 0; i < 2U; i++) {
        RetVal = AmbaRTSL_EmmcReadUserPTB(NULL, i);

        if(pAmbaRTSL_EmmcUserPartTable ->PTBActive != 0U) {
            *pPTBActive = i;
            break;
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcSetActivePtbNo
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
INT32 AmbaRTSL_EmmcSetActivePtbNo(UINT32 PtbNo, UINT8 Active)
{
    INT32 RetVal;
    RetVal = AmbaRTSL_EmmcReadUserPTB(NULL, PtbNo);
    if (RetVal == 0) {
        pAmbaRTSL_EmmcUserPartTable ->PTBActive = Active;
        RetVal = AmbaRTSL_EmmcWriteUserPTB(NULL, PtbNo);
    }
    return RetVal;
}


/*
 *  @RoutineName:: AmbaRTSL_EmmcCreatePTB
 *
 *  @Description:: Create eMMC System and User Partition Tables
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static INT32 AmbaRTSL_EmmcCreatePTB(void)
{
    const AMBA_PARTITION_CONFIG_s *pSysPartConfig  = AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0].pSysPartConfig;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0].pUserPartConfig;
    const AMBA_SD_CARD_INFO_s *pCardInfo           = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0].CardInfo;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 BlkByteSize;
    INT32 RetVal = 0;
    UINT32 BlkAddr, Stg0TotalSector, i;
    UINT32 TotalNumBlk = pCardInfo->MemCardInfo.TotalSectors;                /* Total number of blocks */
    UINT32 Rtsl_EmmcBackupBlkCount = 0U;

    if ((pSysPartConfig != NULL) && (pUserPartConfig != NULL)) {
        /* Block size in Byte */
        pAmbaRTSL_EmmcSysPartTable ->BlkByteSize = 512U;
        BlkByteSize = 512U;

        /* Block-0: Reserved for Bootstrap & System Partition Table */
        /* next Block: Reserved for User Partition Table and Vendor Data, AMBA_USER_PARTITION_VENDOR_DATA must be 0 !!! */


        /* Configure sys partition: BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcSysPartTable ->Entry[AMBA_SYS_PARTITION_BOOTSTRAP]);
        IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pSysPartConfig->PartitionName);
        pPartEntry->Attribute = pSysPartConfig->Attribute;    /* Attribute of the Partition */
        pPartEntry->ByteCount      = pSysPartConfig->ByteCount;         /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = 0;                                 /* start Block Address = 0 */


        /* Block address of User Partition Table */
        BlkAddr = pPartEntry->BlkCount + GetRoundUpValU32((UINT32)(sizeof(AMBA_SYS_PARTITION_TABLE_s) & 0xffffffffU), 512);
        pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB = BlkAddr;

        /* Configure user partition USER_PTB */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_PTB]);
        IO_UtilityStringCopy((char *)pPartEntry->PartitionName, 9U, "USER_PTB");     /* Partition Name */
        pPartEntry->Attribute = pUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = pUserPartConfig->ByteCount;        /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), 512);                                 /* number of Blocks for the Partition: fixed to be 1  */
        pPartEntry->StartBlkAddr   = BlkAddr;                           /* start Block Address = pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB */
        pAmbaRTSL_EmmcUserPartTable ->PTBNumber = 0;

        /* Configure user partition 2nd USER_PTB */
        BlkAddr += pPartEntry->BlkCount;

        BlkAddr += pPartEntry->BlkCount;  /* reserved for 2nd PTB. */

        /* Configure System Partition Table */
        /* Configure remaining sys partitions, except the partition of BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_EmmcSysPartTable ->Entry[0]);
        for (i = 1U; i < AMBA_NUM_SYS_PARTITION; i++) {
            IO_UtilityStringCopy((char *)pPartEntry[i].PartitionName, sizeof(pPartEntry[i].PartitionName), pSysPartConfig[i].PartitionName);
            pPartEntry[i].Attribute = pSysPartConfig[i].Attribute;    /* Attribute of the Partition */
            pPartEntry[i].ByteCount      = pSysPartConfig[i].ByteCount;         /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0U;                                 /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry[i].BlkCount;                /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            if (BlkAddr >= TotalNumBlk) {
                break;
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
                Rtsl_EmmcBackupBlkCount += pPartEntry[i].BlkCount;
            }

            pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            if (BlkAddr >= TotalNumBlk) {
                break;
            }
        }

        /* Configure remind space for storage0 */
        /* Put it to the end of the device.    */
        Rtsl_EmmcUserPtbNextBlock = BlkAddr;
        BlkAddr += Rtsl_EmmcBackupBlkCount;

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

#if defined(CONFIG_MUTI_BOOT_DEVICE)
UINT32 AmbaRTSL_EmmcCreateSlaveUserPTB(const AMBA_PARTITION_CONFIG_s *pUserPartConfig, AMBA_USER_PARTITION_TABLE_s * pUserPartTable, UINT32 *pEndBlock)
{
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 i;
    const AMBA_SD_CARD_INFO_s *pCardInfo           = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0].CardInfo;
    UINT32 TotalNumBlk = pCardInfo->MemCardInfo.TotalSectors;
    UINT32 BlkAddr, Stg0TotalSector, BlkByteSize;
    const AMBA_PARTITION_CONFIG_s *pTmpUserPartConfig;
    UINT32 RetVal = OK;
    UINT32 Rtsl_EmmcBackupBlkCount = 0U;

    if ((pUserPartTable == NULL) || (pUserPartConfig == NULL)) {
        RetVal = AMBA_SD_ERR_INVALID_ARG;
    } else {
        BlkAddr = (UINT32) (2U * GetRoundUpValU32(sizeof(AMBA_USER_PARTITION_TABLE_s), 512U));

        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pUserPartTable->Entry[0]);
        BlkByteSize = 512U;
        for (i = 1U; (i < AMBA_NUM_USER_PARTITION) && (BlkAddr < TotalNumBlk); i++) {
            /* Partition Name */
            if (i == AMBA_USER_PARTITION_FAT_DRIVE_A) {
                continue;
            }

            if (((pPartEntry[i].Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) != AMBA_NVM_eMMC) {
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
                Rtsl_EmmcBackupBlkCount += pPartEntry[i].BlkCount;
            }

            pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */

            if (BlkAddr > TotalNumBlk) {
                RetVal = AMBA_SD_ERR_OTHER;
            }
        }

        if (RetVal == OK) {
            AmbaMisra_TypeCast32(pEndBlock, &BlkAddr);

            /* Configure remind space for storage0 */
            /* Put it to the end of the device.    */
            Stg0TotalSector = TotalNumBlk - BlkAddr - Rtsl_EmmcBackupBlkCount;
            pTmpUserPartConfig = & pUserPartConfig[AMBA_USER_PARTITION_FAT_DRIVE_A];
            pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pUserPartTable->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);

            IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pTmpUserPartConfig->PartitionName);
            pPartEntry->Attribute = pTmpUserPartConfig->Attribute;   /* Attribute of the Partition */
            pPartEntry->ByteCount      = (Stg0TotalSector * BlkByteSize);   /* number of Bytes for the Partition */
            pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
            pPartEntry->BlkCount       = Stg0TotalSector;                   /* number of Blocks for the Partition  */
            pPartEntry->StartBlkAddr   = BlkAddr + Rtsl_EmmcBackupBlkCount;                           /* start Block Address */
        }
    }
    return RetVal;
}

#endif

UINT32 AmbaRTSL_EmmcCreateExtUserPTB(const AMBA_PARTITION_CONFIG_s *pUserPartConfig, AMBA_USER_PARTITION_TABLE_s * pUserPartTable, UINT32 StartBlk)
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

        if (BlkAddr >= TotalNumBlk) {
            RetVal = AMBA_SD_ERR_OTHER;
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

static INT32 AmbaRTSL_EmmcHandleCreatePtb(void)
{
    INT32 RetVal = 0;
    AMBA_PARTITION_ENTRY_s  Stg0PartEntry;

    if (AmbaWrap_memset(pAmbaRTSL_EmmcSysPartTable,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s)) != OK) { /* Do nothing */ };
    if (AmbaWrap_memset(pAmbaRTSL_EmmcUserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s)) != OK) { /* Do nothing */ };
    RetVal = AmbaRTSL_EmmcCreatePTB();

    if ((WritePtbFlag != 0U) && (RetVal == 0)) {
        RetVal = AmbaRTSL_EmmcWriteSysPTB(NULL);
        if (RetVal == 0) {
            RetVal = AmbaRTSL_EmmcWriteUserPTB(NULL, 0U);
        }
    }

    if (AmbaWrap_memcpy(&Stg0PartEntry, &pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A], sizeof(Stg0PartEntry)) != OK) { /* Do nothing */ };

    if (RetVal == 0) {
        if (AmbaRTSL_EmmcCreateExtUserPTB(&AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0].pUserPartConfig[0], pAmbaRTSL_EmmcUserPartTable, Rtsl_EmmcUserPtbNextBlock) != 0U) {
            RetVal = -1;
        }
    }

    if ((WritePtbFlag == 1U) && (RetVal == 0)) {
        /* Write EXT PTB */
        RetVal = AmbaRTSL_EmmcWriteUserPTB(NULL, 1U);
    }

    if ((Stg0PartEntry.StartBlkAddr != pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A].StartBlkAddr) ||
        (Stg0PartEntry.BlkCount     != pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A].BlkCount)) {
        /* Update remind space for storage0 to USER PTB0 */
        if (AmbaWrap_memcpy(&Stg0PartEntry, &pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A], sizeof(Stg0PartEntry)) != OK) { /* Do nothing */ };
        if (RetVal == 0) {
            RetVal = AmbaRTSL_EmmcReadUserPTB(NULL, 0U);  /* Switch back to PTB0 */
        }

        if (AmbaWrap_memcpy(&pAmbaRTSL_EmmcUserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A], &Stg0PartEntry, sizeof(Stg0PartEntry)) != OK) {
            /* Do nothing */
        }

        if ((WritePtbFlag == 1U) && (RetVal == 0)) {
            RetVal = AmbaRTSL_EmmcWriteUserPTB(NULL, 0U);
        }
    } else {
        if (RetVal == 0) {
            RetVal = AmbaRTSL_EmmcReadUserPTB(NULL, 0U);    /* Switch back to PTB0 */
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcInitPtbBbt
 *
 *  @Description:: Init BBT and System/User Partition Tables
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcInitPtbBbt(void)
{
    INT32 RetVal = 0;
    UINT32 Crc32;
    const UINT8 *pTmp = NULL;
    UINT32 ActivePTB = 0U;

    // (void) AmbaWrap_memcpy(&AmbaRTSL_EmmcSysPartTable,  pAmbaRTSL_EmmcSysPartTable ,  sizeof(AMBA_SYS_PARTITION_TABLE_s));
    // (void) AmbaWrap_memcpy(&AmbaRTSL_EmmcUserPartTable, pAmbaRTSL_EmmcUserPartTable , sizeof(AMBA_USER_PARTITION_TABLE_s));

    pAmbaRTSL_EmmcSysPartTable   = &AmbaRTSL_EmmcSysPartTable;
    pAmbaRTSL_EmmcUserPartTable  = &AmbaRTSL_EmmcUserPartTable;

    RetVal = AmbaRTSL_EmmcReadSysPTB(NULL);

    /* Check System Partition Table */
    AmbaMisra_TypeCast(&pTmp, &pAmbaRTSL_EmmcSysPartTable );
    Crc32 = IO_UtilityCrc32(pTmp, (UINT32)((sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U) & 0xffffffffU));

    if ((RetVal == 0) &&
        (pAmbaRTSL_EmmcSysPartTable ->CRC32 == Crc32) && (pAmbaRTSL_EmmcSysPartTable ->BlkByteSize != 0U)) {

        RetVal = AmbaRTSL_EmmcGetActivePtbNo(&ActivePTB);
        /* if System Partion is OK */
        if (RetVal == 0) {
            if (AmbaRTSL_EmmcReadUserPTB(NULL, ActivePTB) != 0) {
                RetVal = -1;
            }
        }

        if (RetVal == 0) {
            AmbaMisra_TypeCast(&pTmp, &pAmbaRTSL_EmmcUserPartTable );
            Crc32 = IO_UtilityCrc32(pTmp, (UINT32)(AMBA_USER_PTB_CRC32_SIZE & 0xffffffffU));
            if ((pAmbaRTSL_EmmcUserPartTable ->CRC32 == Crc32) &&
                (pAmbaRTSL_EmmcUserPartTable ->PtbMagic == AMBA_USER_PTB_MAGIC)) {
                RetVal = 1; /* SYS, USER PTB & BBT is OK */
            }
        }
    }

    if (RetVal == 0) {
        RetVal = AmbaRTSL_EmmcHandleCreatePtb();
    }

    if (RetVal > 0) {
        RetVal = 0;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcReadSysPTB
 *
 *  @Description:: Read NAND System Partition Table (Block 0, Page 2)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the buffer of System partition table
 *
 *  @Output     ::
 *      pSysPartTable: pointer to the data of System partition table
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable)
{
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    UINT8 *pWorkBufMain = AmbaRTSL_SdWorkBuf;
    INT32 RetVal;
    const AMBA_RTSL_SD_CTRL_s *pSdCtrl = (AMBA_RTSL_SD_CTRL_s *) & (AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0]);

    /* Read System partition table from NAND which is located at block-0 page-2 */
    SecConfig.NumSector   = pSdCtrl->SysPtblPageCount;
    SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
    SecConfig.StartSector = pSdCtrl->BstPageCount;

    RetVal = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);
    if (RetVal == 0) {
        /* copy to the user area while needed */
        if (pSysPartTable != NULL) {
            if (AmbaWrap_memcpy(pSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s)) != OK) { /* Do nothing */ };
        } else {
            /* copy to the DRAM memory */
            if (AmbaWrap_memcpy(pAmbaRTSL_EmmcSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s)) != OK) { /* Do nothing */ };
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcWriteSysPTB
 *
 *  @Description:: Write NAND System Partition Table (Block 0, Page 2)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the data of System partition table
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcWriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable)
{
    const AMBA_RTSL_SD_CTRL_s *pSdCtrl = (AMBA_RTSL_SD_CTRL_s *) & (AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0]);

    UINT32 BstPageCount     = pSdCtrl->BstPageCount;      /* Number of Pages for BST */
    UINT32 SysPtblPageCount = pSdCtrl->SysPtblPageCount;  /* Number of Pages for System Partition Table */
    UINT8  *pWorkBufMain    = AmbaRTSL_SdWorkBuf;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    AMBA_SYS_PARTITION_TABLE_s *pWorkSysPartTable;
    INT32 RetVal;
    const UINT8 * pTmp = NULL;

    if (pSysPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkSysPartTable = pSysPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkSysPartTable = pAmbaRTSL_EmmcSysPartTable ;
    }

    /* 1. Read BST data (block 0, page 0) */
    SecConfig.NumSector   = BstPageCount;
    SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
    SecConfig.StartSector = 0;

    RetVal = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);

    if (RetVal == 0) {
        /* 2. Erase the block 0 */
        /* 3. Write back BST data (block 0, page 0) */
        SecConfig.NumSector   = BstPageCount;
        SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
        SecConfig.StartSector = 0;

        RetVal = AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);
    }

    if (RetVal == 0) {
        /* 4. Calculate CRC32 */
        AmbaMisra_TypeCast(&pTmp, &pWorkSysPartTable);
        pWorkSysPartTable->CRC32 = IO_UtilityCrc32(pTmp, (UINT32)((sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U) & 0xffffffffU));

        /* 5. Write new System Partition Table (block 0, page 2) */
        SecConfig.NumSector   = SysPtblPageCount;
        SecConfig.pDataBuf    = (UINT8 *) pWorkSysPartTable;
        SecConfig.StartSector = BstPageCount;

        RetVal = AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcReadUserPTB
 *
 *  @Description:: Read NAND User Partition Table (Block 1, Page 0)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the buffer of User partition table
 *
 *  @Output     ::
 *      pSysPartTable: pointer to the data of User partition table
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo)
{
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    UINT8 *pWorkBufMain = AmbaRTSL_SdWorkBuf;
    INT32 RetVal;
#if !defined(CONFIG_MUTI_BOOT_DEVICE)
    const AMBA_RTSL_SD_CTRL_s *pSdCtrl = (AMBA_RTSL_SD_CTRL_s *) & (AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0]);
#endif

    /* Read User partition table from NAND */
    SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;

#if defined(CONFIG_MUTI_BOOT_DEVICE)
    SecConfig.NumSector   = GetRoundUpValU32(sizeof(AMBA_USER_PARTITION_TABLE_s), 512U);
    if(UserPtbNo == 0U) {
        SecConfig.StartSector = 0U;
    } else {
        SecConfig.StartSector = SecConfig.NumSector;
    }
#else
    SecConfig.NumSector   = pSdCtrl->UserPtblPageCount;
    SecConfig.StartSector = pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB + (UserPtbNo * pSdCtrl->UserPtblPageCount);
#endif


    RetVal = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);
    if (RetVal == 0) {
        /* copy to the user area while needed */
        if (pUserPartTable != NULL) {
            if (AmbaWrap_memcpy(pUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s)) != OK) { /* Do nothing */ };
        } else {
            /* copy to the DRAM memory */
            if (AmbaWrap_memcpy(pAmbaRTSL_EmmcUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s)) != OK) { /* Do nothing */ };
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcWriteUserPTB
 *
 *  @Description:: Write NAND User Partition Table (Block 1, Page 0)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the data of User partition table
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcWriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo)
{
#if !defined(CONFIG_MUTI_BOOT_DEVICE)
    const AMBA_RTSL_SD_CTRL_s *pSdCtrl = (AMBA_RTSL_SD_CTRL_s *) & (AmbaRTSL_SdCtrl[AMBA_SD_CHANNEL0]);
#endif
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    AMBA_USER_PARTITION_TABLE_s *pWorkUserPartTable = pUserPartTable;
    INT32 RetVal;
    const UINT8 * pTmp = NULL;

    if (pUserPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkUserPartTable = pUserPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkUserPartTable = pAmbaRTSL_EmmcUserPartTable ;
    }

    AmbaMisra_TypeCast(&pTmp, &pWorkUserPartTable);
    pWorkUserPartTable->CRC32 = IO_UtilityCrc32(pTmp, (UINT32)(AMBA_USER_PTB_CRC32_SIZE & 0xffffffffU));

    /* Write new User partition table (block 1, page 0) */
    SecConfig.pDataBuf    = (UINT8 *) pWorkUserPartTable;
#if defined(CONFIG_MUTI_BOOT_DEVICE)
    SecConfig.NumSector   = GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), 512U);
    if(UserPtbNo == 0U) {
        SecConfig.StartSector = 0U;
    } else {
        SecConfig.StartSector = SecConfig.NumSector;
    }
#else
    SecConfig.NumSector   = pSdCtrl->UserPtblPageCount;
    SecConfig.StartSector = pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB + (UserPtbNo * pSdCtrl->UserPtblPageCount);
#endif

    RetVal = AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcGetPartEntry
 *
 *  @Description:: get partition entry
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          AMBA_PARTITION_ENTRY_s * : pointer to the partition entry/NG(NULL)
 */
AMBA_PARTITION_ENTRY_s *AmbaRTSL_EmmcGetPartEntry(UINT32 PartFlag, UINT32 PartID)
{
    AMBA_PARTITION_ENTRY_s *pPartEntry;

    if (PartFlag != 0U) {
        /* User Partition */
        if (PartID >= AMBA_NUM_USER_PARTITION) {
            pPartEntry = NULL;    /* Wrong Parameter */
        } else {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcUserPartTable ->Entry[PartID]);
        }
    } else {
        /* System Partition */
        if (PartID >= AMBA_NUM_SYS_PARTITION) {
            pPartEntry = NULL;    /* Wrong Parameter */
        } else {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcSysPartTable ->Entry[PartID]);
        }
    }

    return pPartEntry;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcReadPartitionInfo
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
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    INT32 RetVal = 0;

    if ((pPartInfo == NULL) || (pPartEntry == NULL)) {
        RetVal = -1;
    } else {
        if (AmbaWrap_memcpy(pPartInfo, pPartEntry, sizeof(AMBA_PARTITION_ENTRY_s)) != OK) { /* Do nothing */ };
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcReadPartition
 *
 *  @Description:: Read partition data from NAND flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *      pDataBuf: pointer to partition data
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf)
{
    UINT32 BlkByteSize = 512U;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    INT32 RetVal = 0;

    if ((pDataBuf != NULL) && (pPartEntry != NULL)) {
#if (FLASH_PROG_EMMC_BOOT_PART != EMMC_BOOTP_USER)
        if ((PartFlag == 0U) && (PartID == AMBA_SYS_PARTITION_BOOTSTRAP)) {
            AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};
            ArgCmd6.Access = ACCESS_WRITE_BYTE;
            ArgCmd6.Index  = BOOT_CONFIG;
            ArgCmd6.Value  = (FLASH_PROG_EMMC_BOOT_PART | EMMC_CMD6_VALUE_ACCP);

            (void) AmbaRTSL_SdSendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
            RetVal = AmbaRTSL_SdWaitCmdDone(AMBA_SD_CHANNEL0, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        }
#endif
        if (RetVal == 0) {
            UINT32 BlkCount = GetRoundUpValU32(pPartEntry->ActualByteSize, BlkByteSize);
            UINT32 ReadCount = 2048U;
            UINT32 SecAddr = pPartEntry->StartBlkAddr;

            if (BlkCount > pPartEntry->BlkCount) {
                BlkCount = pPartEntry->BlkCount;  /* should never happen */
            }

            for (UINT32 i = 0; i <= BlkCount; i += ReadCount) {
                if (BlkCount < ReadCount) {
                    SecConfig.NumSector   = BlkCount;
                } else {
                    SecConfig.NumSector  = ReadCount;
                }
                SecConfig.pDataBuf    = &pDataBuf[i * BlkByteSize];
                SecConfig.StartSector = SecAddr;

                RetVal = AmbaRTSL_SdReadSector(AMBA_SD_CHANNEL0, &SecConfig);

                if (RetVal < 0) {
                    break;
                } else {
                    SecAddr += ReadCount;
                }
            }
        }
#if (FLASH_PROG_EMMC_BOOT_PART != EMMC_BOOTP_USER)
        if ((RetVal == 0) &&
            (PartFlag == 0U) && (PartID == AMBA_SYS_PARTITION_BOOTSTRAP)) {
            AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};
            ArgCmd6.Access = ACCESS_WRITE_BYTE;
            ArgCmd6.Index  = BOOT_CONFIG;
            ArgCmd6.Value  = (FLASH_PROG_EMMC_BOOT_PART | EMMC_ACCP_USER);

            (void) AmbaRTSL_SdSendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
            RetVal = AmbaRTSL_SdWaitCmdDone(AMBA_SD_CHANNEL0, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
        }
#endif
    } else {
        RetVal = -1;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcWritePartition
 *
 *  @Description:: Write partition data from NAND flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *      DataSize: Data size in Byte to write to the partition
 *      pDataBuf: pointer to partition data
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcWritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf)
{
    UINT32 BlkCount, BlkByteSize;
    INT32 RetVal = 0;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;

    if ((DataSize != 0U) && (pDataBuf != NULL) && (pPartEntry != NULL)) {

        BlkByteSize = 512U;
        BlkCount = (DataSize + BlkByteSize - 1U) / BlkByteSize;

        if (BlkCount <= pPartEntry->BlkCount) {

#if (FLASH_PROG_EMMC_BOOT_PART != EMMC_BOOTP_USER)
            if ((PartFlag == 0U) && (PartID == AMBA_SYS_PARTITION_BOOTSTRAP)) {
                AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};
                ArgCmd6.Access = ACCESS_WRITE_BYTE;
                ArgCmd6.Index  = BOOT_CONFIG;
                ArgCmd6.Value  = (FLASH_PROG_EMMC_BOOT_PART | EMMC_CMD6_VALUE_ACCP);

                (void) AmbaRTSL_SdSendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
                RetVal = AmbaRTSL_SdWaitCmdDone(AMBA_SD_CHANNEL0, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
            }
#endif
            if (RetVal == 0) {
                /* 1. Program blocks in target partition */
                SecConfig.NumSector   = BlkCount;
                SecConfig.pDataBuf    = (UINT8 *) pDataBuf;
                SecConfig.StartSector = pPartEntry->StartBlkAddr;

                RetVal = AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);
            }
#if (FLASH_PROG_EMMC_BOOT_PART != EMMC_BOOTP_USER)
            if ((RetVal == 0) &&
                (PartFlag == 0U) && (PartID == AMBA_SYS_PARTITION_BOOTSTRAP)) {
                AMBA_SD_EMMC_CMD6_ARGUMENT_s ArgCmd6 = {0};
                ArgCmd6.Access = ACCESS_WRITE_BYTE;
                ArgCmd6.Index  = BOOT_CONFIG;
                ArgCmd6.Value  = (FLASH_PROG_EMMC_BOOT_PART | EMMC_ACCP_USER);

                (void) AmbaRTSL_SdSendEmmcCMD6(AMBA_SD_CHANNEL0, &ArgCmd6);
                RetVal = AmbaRTSL_SdWaitCmdDone(AMBA_SD_CHANNEL0, AMBA_SD_IRQ_CMD_DONE | AMBA_SD_IRQ_TRANSFER_DONE);
                if (RetVal != 0) {
                    RetVal = -1;
                }
            }
#endif
            /* 3. Update partition table */
            if (RetVal == 0) {
                pPartEntry->Attribute = pPartEntry->Attribute & (~(AMBA_PARTITION_ATTR_ERASED | AMBA_PARTITION_ATTR_INVALID));
                pPartEntry->ActualByteSize = DataSize;
                pPartEntry->ProgramStatus = 0;
                pPartEntry->ImageCRC32 = IO_UtilityCrc32(pDataBuf, DataSize);

                if (PartFlag != 0U) {
                    /* User Partition */
                    RetVal = AmbaRTSL_EmmcWriteUserPTB(NULL, pAmbaRTSL_EmmcUserPartTable ->PTBNumber);

                    /* Sync partition info if the partition is nonbackup partition */
                    if ((CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_BACKUP) == 0x0U) && (RetVal == 0)) {
                        /* Switch to the other user PTB */
                        if (pAmbaRTSL_EmmcUserPartTable ->PTBNumber == 0U) {
                            RetVal = AmbaRTSL_EmmcReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 1U);
                        } else {
                            RetVal = AmbaRTSL_EmmcReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 0U);
                        }

                        pPartEntry->Attribute = pPartEntry->Attribute & (~(AMBA_PARTITION_ATTR_ERASED | AMBA_PARTITION_ATTR_INVALID));
                        pPartEntry->ActualByteSize = DataSize;
                        pPartEntry->ProgramStatus = 0;
                        pPartEntry->ImageCRC32 = IO_UtilityCrc32(pDataBuf, DataSize);

                        if (RetVal == 0) {
                            /* Update the user PTB */
                            RetVal = AmbaRTSL_EmmcWriteUserPTB(pAmbaRTSL_EmmcUserPartTable, pAmbaRTSL_EmmcUserPartTable ->PTBNumber);
                        }

                        if (RetVal == 0) {
                            /* Switch to the original user PTB */
                            if (pAmbaRTSL_EmmcUserPartTable ->PTBNumber == 0U) {
                                RetVal = AmbaRTSL_EmmcReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 1U);
                            } else {
                                RetVal = AmbaRTSL_EmmcReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 0U);
                            }
                        }
                    }
                } else {
                    /* System Partition */
                    RetVal = AmbaRTSL_EmmcWriteSysPTB(NULL);
                }
            }
        }
    } else {
        RetVal = -1;
    }

    return RetVal;
}

#if 0
/*
 *  @RoutineName:: AmbaRTSL_EmmcInvalidatePartition
 *
 *  @Description:: Invalidate the partition
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcInvalidatePartition(INT32 PartFlag, UINT32 PartID)
{
    INT32 RetVal;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);

    if (pPartEntry == NULL)
        return NG;  /* Wrong Parameter */

    if ((pPartEntry->Attribute & AMBA_PARTITION_ATTR_INVALID) != 0U)
        return OK;

    pPartEntry->Attribute = (pPartEntry->Attribute | AMBA_PARTITION_ATTR_INVALID); /* 1 - Invalid/Not-Use */
    if (PartFlag) {
        /* User Partition */
        RetVal = AmbaRTSL_EmmcWriteUserPTB(NULL);
    } else {
        /* System Partition */
        RetVal = AmbaRTSL_EmmcWriteSysPTB(NULL);
    }

    return RetVal;
}
#endif

/*
 *  @RoutineName:: AmbaRTSL_EmmcErasePartition
 *
 *  @Description:: Erase a partition
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
INT32 AmbaRTSL_EmmcErasePartition(UINT32 PartFlag, UINT32 PartID)
{
    INT32 RetVal = 0;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    UINT8 *pWorkBufMain = AmbaRTSL_SdWorkBuf;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    UINT32 BlkAddr, BlkCount, EraseSecCount, EndBlkAddr;

    EraseSecCount = (AmbaRTSL_SdGetWorkBufferSize() / 512U);

    if (pPartEntry == NULL) {
        RetVal = -1;  /* Wrong Parameter */
    } else if ((pPartEntry->Attribute & AMBA_PARTITION_ATTR_ERASED) == 0U) {

        if (AmbaWrap_memset(AmbaRTSL_SdWorkBuf, 0x0, AmbaRTSL_SdGetWorkBufferSize()) != OK) { /* Do nothing */ };

        /* 1. Erase all blocks in target partition */
        BlkCount = pPartEntry->BlkCount;
        EndBlkAddr = pPartEntry->StartBlkAddr + pPartEntry->BlkCount;

        BlkAddr = pPartEntry->StartBlkAddr;
        while (BlkAddr < EndBlkAddr) {

            if (BlkCount < EraseSecCount) {
                EraseSecCount = BlkCount;
            }

            SecConfig.NumSector   = EraseSecCount;
            SecConfig.pDataBuf    = (UINT8 *) pWorkBufMain;
            SecConfig.StartSector = BlkAddr;

            (void) AmbaRTSL_SdWriteSector(AMBA_SD_CHANNEL0, &SecConfig);

            if (BlkCount >= EraseSecCount) {
                BlkCount -= EraseSecCount;
            } else {
                break;
            }
            BlkAddr += SecConfig.NumSector;
        }

        /* 3. Update partition table */
        pPartEntry->Attribute = (pPartEntry->Attribute | AMBA_PARTITION_ATTR_ERASED);  /* 1 - Erased */
        pPartEntry->ActualByteSize = 0;
        pPartEntry->ImageCRC32 = 0xffffffffU;
        pPartEntry->ProgramStatus = 0xffffffffU;
        if (PartFlag != 0U) {
            /* User Partition */
            RetVal = AmbaRTSL_EmmcWriteUserPTB(NULL, pAmbaRTSL_EmmcUserPartTable ->PTBNumber);

            /* Sync partition info if the partition is nonbackup partition */
            if ((CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_BACKUP) == 0x0U) && (RetVal == 0)) {
                /* Switch to the other user PTB */
                if (pAmbaRTSL_EmmcUserPartTable ->PTBNumber == 0U) {
                    RetVal = AmbaRTSL_EmmcReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 1U);
                } else {
                    RetVal = AmbaRTSL_EmmcReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 0U);
                }

                pPartEntry->Attribute = (pPartEntry->Attribute | AMBA_PARTITION_ATTR_ERASED);  /* 1 - Erased */
                pPartEntry->ActualByteSize = 0;
                pPartEntry->ImageCRC32 = 0xffffffffU;
                pPartEntry->ProgramStatus = 0xffffffffU;

                if (RetVal == 0) {
                    /* Update the user PTB */
                    RetVal = AmbaRTSL_EmmcWriteUserPTB(pAmbaRTSL_EmmcUserPartTable, pAmbaRTSL_EmmcUserPartTable ->PTBNumber);
                }

                if (RetVal == 0) {
                    /* Switch to the original user PTB */
                    if (pAmbaRTSL_EmmcUserPartTable ->PTBNumber == 0U) {
                        RetVal = AmbaRTSL_EmmcReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 1U);
                    } else {
                        RetVal = AmbaRTSL_EmmcReadUserPTB(pAmbaRTSL_EmmcUserPartTable, 0U);
                    }
                }
            }
        } else {
            /* System Partition */
            RetVal = AmbaRTSL_EmmcWriteSysPTB(NULL);
        }
    } else {
        RetVal = 0;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcIsBldMagicCodeSet
 *
 *  @Description:: check if Bootloader Magic Code has been set for System Software load or not
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : Yes(1)/No(0)
 */
UINT32 AmbaRTSL_EmmcIsBldMagicCodeSet(void)
{
    UINT32 RetVal;

    if (pAmbaRTSL_EmmcUserPartTable->BldMagicCode == AMBA_USER_PTB_MAGIC) {
        RetVal = 1U;
    } else {
        RetVal = 0;
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcSetBldMagicCode
 *
 *  @Description:: Write Bootloader Magic Code onto NAND device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_EmmcSetBldMagicCode(void)
{
    UINT32 RetVal = 0;
    UINT32 PTBActive;

    (void)AmbaRTSL_EmmcGetActivePtbNo(&PTBActive);

    if (pAmbaRTSL_EmmcUserPartTable->BldMagicCode != AMBA_USER_PTB_MAGIC) {
        /* Write Bootloader Magic Code */
        pAmbaRTSL_EmmcUserPartTable->BldMagicCode = AMBA_USER_PTB_MAGIC; /* Bootloader Magic Code */
        if (0 != AmbaRTSL_EmmcWriteUserPTB(NULL, pAmbaRTSL_EmmcUserPartTable->PTBNumber)) {
            RetVal = AMBA_SD_ERR_UNEXPECT;
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcEraseBldMagicCode
 *
 *  @Description:: Erase Bootloader Magic Code from NAND device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_EmmcEraseBldMagicCode(void)
{
    UINT32 RetVal = 0;
    UINT32 PTBActive;

    (void)AmbaRTSL_EmmcGetActivePtbNo(&PTBActive);

    if (pAmbaRTSL_EmmcUserPartTable->BldMagicCode == AMBA_USER_PTB_MAGIC) {
        /* Erase Bootloader Magic Code */
        pAmbaRTSL_EmmcUserPartTable->BldMagicCode = 0xffffffffU; /* Bootloader Magic Code */
        if (0 != AmbaRTSL_EmmcWriteUserPTB(NULL, pAmbaRTSL_EmmcUserPartTable->PTBNumber)) {
            RetVal = AMBA_SD_ERR_UNEXPECT;
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcReadBldMagicCode
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
UINT32 AmbaRTSL_EmmcReadBldMagicCode(void)
{
    return pAmbaRTSL_EmmcUserPartTable->BldMagicCode;
}

/*
 *  @RoutineName:: AmbaRTSL_EmmcGetBootChannel
 *
 *  @Description:: Get Emmc boot channel
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: Channel number
 */
UINT8 AmbaRTSL_EmmcGetBootChannel(void)
{
    return 0;
}


void AmbaRTSL_EmmcSetWritePtbFlag(UINT32 Flag)
{
    WritePtbFlag = Flag;
}
