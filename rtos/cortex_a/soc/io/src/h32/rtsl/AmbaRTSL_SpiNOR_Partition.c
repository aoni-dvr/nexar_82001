/*
 * Copyright (c) 2020 Ambarella International LP
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

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#include "AmbaIOUtility.h"
#include "AmbaWrap.h"

#include "AmbaSpiNOR_Def.h"
#include "AmbaRTSL_SpiNOR.h"
#include "AmbaRTSL_SpiNOR_Ctrl.h"

#if defined(CONFIG_MUTI_BOOT_DEVICE)
#include "AmbaRTSL_SD.h"
#endif

static UINT32 WritePtbFlag = 0;
static UINT32 Rtsl_UserPtbNextBlock = 0U;

AMBA_SYS_PARTITION_TABLE_s  *pAmbaRTSL_NorSpiSysPartTable  = NULL; /* pointer to System Partition Table */
AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_NorSpiUserPartTable = NULL; /* pointer to User Partition Table   */

AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_NorSysPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
AMBA_USER_PARTITION_TABLE_s AmbaRTSL_NorUserPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT8 NorPartBuf[AMBA_SPINOR_DMA_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")));


/*
 *  @RoutineName:: AmbaRTSL_NorGetActivePtbNo
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
UINT32 AmbaRTSL_NorSpiGetActivePtbNo(UINT32 * pPTBActive)
{
    UINT32 i;
    UINT32 RetVal;

    *pPTBActive = 0U;

    for(i = 0; i < 2U; i++) {
        RetVal = AmbaRTSL_NorSpiReadUserPTB(NULL, i);

        if(pAmbaRTSL_NorSpiUserPartTable->PTBActive != 0U) {
            *pPTBActive = i;
            break;
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSetActivePtbNo
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
UINT32 AmbaRTSL_NorSpiSetActivePtbNo(UINT32 PtbNo, UINT8 Active)
{
    UINT32 RetVal;
    RetVal = AmbaRTSL_NorSpiReadUserPTB(NULL, PtbNo);
    if (RetVal == SPINOR_ERR_NONE) {
        pAmbaRTSL_NorSpiUserPartTable->PTBActive = Active;
        RetVal = AmbaRTSL_NorSpiWriteUserPTB(NULL, PtbNo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiInitPtbBbt
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
UINT32 AmbaRTSL_NorSpiInitPtbBbt(void)
{
    UINT32 RetVal;
    UINT32 Crc32;
    const UINT8 *pTmp = NULL;
#if defined(CONFIG_MUTI_BOOT_DEVICE)
    UINT32 EndBlock = 0U;
#endif
    UINT32 ActivePTB = 0U;
    UINT32 CreatePtb = 0U;
    UINT32 CrcSize;

    //    if (pAmbaRTSL_NorSpiSysPartTable == NULL) {
    //        UINT32 SysTableAddr = (UINT32) AMBA_RAM_SYS_PTB_VIRT_ADDR;
    //        AmbaMisra_TypeCast32(&pAmbaRTSL_NorSpiSysPartTable, &SysTableAddr);
    //    }
    //
    //    if (pAmbaRTSL_NorSpiUserPartTable == NULL) {
    //        UINT32 UserTableAddr = (UINT32) AMBA_RAM_USER_PTB_VIRT_ADDR;
    //        AmbaMisra_TypeCast32(&pAmbaRTSL_NorSpiUserPartTable, &UserTableAddr);
    //    }

    pAmbaRTSL_NorSpiSysPartTable = &AmbaRTSL_NorSysPartTable;
    pAmbaRTSL_NorSpiUserPartTable = &AmbaRTSL_NorUserPartTable;

    /* Read NORSPI System Partition Table (Block 0, Page 2) to DRAM */
    RetVal = AmbaRTSL_NorSpiReadSysPTB(NULL);

    /* Check System Partition Table */
    AmbaMisra_TypeCast(&pTmp, &pAmbaRTSL_NorSpiSysPartTable);
    CrcSize = (UINT32)((sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U) & 0xffffffffU);
    Crc32 = IO_UtilityCrc32(pTmp, CrcSize);
    if ((RetVal == 0U) && (pAmbaRTSL_NorSpiSysPartTable->CRC32 == Crc32)) {

        /* if System Partion is OK */
        RetVal = AmbaRTSL_NorSpiGetActivePtbNo(&ActivePTB);

        if (RetVal == OK) {
            RetVal = AmbaRTSL_NorSpiReadUserPTB(NULL, ActivePTB);
        }

        if (RetVal == OK) {
            /* Check if User Partition Table is already on DRAM */
            CrcSize = (UINT32)(AMBA_USER_PTB_CRC32_SIZE & 0xffffffffU);
            AmbaMisra_TypeCast(&pTmp, &pAmbaRTSL_NorSpiUserPartTable);
            Crc32 = IO_UtilityCrc32(pTmp, CrcSize);
            if ((pAmbaRTSL_NorSpiUserPartTable->CRC32 == Crc32) &&
                (pAmbaRTSL_NorSpiUserPartTable->CRC32 != 0xffffffffU)) {
                CreatePtb = 0U;
            } else {
                CreatePtb = 1U;
            }
        }
    } else {
        if (RetVal == OK) {
            CreatePtb = 1U;
        }
    }

    if (CreatePtb != 0U) {
        if (0U != AmbaWrap_memset(pAmbaRTSL_NorSpiSysPartTable,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };
        if (0U != AmbaWrap_memset(pAmbaRTSL_NorSpiUserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
        RetVal = AmbaRTSL_SpiNorCreatePTB();
#if defined(CONFIG_MUTI_BOOT_DEVICE)
        if (RetVal == 0U) {
            RetVal = AmbaRTSL_EmmcCreateSlaveUserPTB(&AmbaRTSL_NorSpiCtrl.pUserPartConfig[0], pAmbaRTSL_NorSpiUserPartTable, &EndBlock);
        }
#endif

        if ((WritePtbFlag != 0U) && (RetVal == 0U)) {
            RetVal = AmbaRTSL_NorSpiWriteSysPTB(NULL);
            RetVal |= AmbaRTSL_NorSpiWriteUserPTB(NULL, 0U);
        }
#if defined(CONFIG_MUTI_BOOT_DEVICE)
        if (RetVal == 0U) {
            RetVal = AmbaRTSL_EmmcCreateExtUserPTB(&AmbaRTSL_NorSpiCtrl.pUserPartConfig[0], pAmbaRTSL_NorSpiUserPartTable, EndBlock);
        }

        if ((WritePtbFlag != 0U) && (RetVal == 0U)) {
            RetVal = AmbaRTSL_NorSpiWriteUserPTB(NULL, 1U);

            if (RetVal == 0U) {
                RetVal = AmbaRTSL_NorSpiReadUserPTB(NULL, 0U);
            }
        }
#else
        if (RetVal == SPINOR_ERR_NONE) {
            RetVal = AmbaRTSL_SpiNorCreateExtUserPTB(Rtsl_UserPtbNextBlock);
        }

        if ((WritePtbFlag == 1U) && (RetVal == SPINOR_ERR_NONE)) {
            RetVal = AmbaRTSL_NorSpiWriteUserPTB(NULL, 1U);
        }

        if (RetVal == SPINOR_ERR_NONE) {
            RetVal = AmbaRTSL_NorSpiReadUserPTB(NULL, 0U);
        }
#endif
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_SpiNorCreatePTB
 *
 *  @Description:: Create NAND System and User Partition Tables
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_SpiNorCreatePTB(void)
{
    const AMBA_PARTITION_CONFIG_s *pSysPartConfig  = AmbaRTSL_NorSpiCtrl.pSysPartConfig;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = AmbaRTSL_NorSpiCtrl.pUserPartConfig;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 BlkByteSize, BlkAddr = 0, TotalNumBlk;
    UINT32 i, RetVal = OK;

    if ((pSysPartConfig == NULL) || (pUserPartConfig == NULL)) {
        RetVal = SPINOR_ERR_ARG;
    } else {
        /* Block size in Byte */
        pAmbaRTSL_NorSpiSysPartTable->BlkByteSize = AmbaRTSL_NorSpiCtrl.PartitionAllocateSize;

        BlkByteSize = AmbaRTSL_NorSpiCtrl.PartitionAllocateSize;
        TotalNumBlk = AmbaRTSL_NorSpiCtrl.TotalByteSize / BlkByteSize;

        /* Block-0: Reserved for Bootstrap & System Partition Table */
        /* next Block: Reserved for User Partition Table and Vendor Data, AMBA_USER_PARTITION_PTB must be 0 !!! */

        /* Configure sys partition: BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_NorSpiSysPartTable->Entry[AMBA_SYS_PARTITION_BOOTSTRAP]);
        IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pSysPartConfig->PartitionName);     /* Partition Name */
        pPartEntry->Attribute      = pSysPartConfig->Attribute;    /* Attribute of the Partition */
        pPartEntry->ByteCount      = pSysPartConfig->ByteCount;    /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                            /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = 0;                            /* start Block Address = 0 */

        /* Block address of User Partition Table */
        BlkAddr = pPartEntry->BlkCount + GetRoundUpValU32((UINT32)(sizeof(AMBA_SYS_PARTITION_TABLE_s) & 0xffffffffU), BlkByteSize);
        pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB = BlkAddr;
        pAmbaRTSL_NorSpiUserPartTable->PTBNumber = 0U;
        pAmbaRTSL_NorSpiUserPartTable->PTBActive = 1U;
        pAmbaRTSL_NorSpiUserPartTable->BlkAddrCurentPTB = BlkAddr;

        /* Configure user partition USER_PTB */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_PTB]);
        IO_UtilityStringCopy((char *)pPartEntry->PartitionName, 32U, "USER_PTB");     /* Partition Name */
        pPartEntry->Attribute      = pUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = pUserPartConfig->ByteCount;   /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                            /* actual size in Bytes */
        pPartEntry->BlkCount       = 1U;                           /* number of Blocks for the Partition: fixed to be 1  */
        pPartEntry->StartBlkAddr   = BlkAddr;                      /* start Block Address = pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB */
        BlkAddr += pPartEntry->BlkCount;

        BlkAddr += GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), BlkByteSize);

        /* Configure System Partition Table */
        /* Configure remaining sys partitions, except the partition of BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_NorSpiSysPartTable->Entry[0]);
        for (i = 1U; i < AMBA_NUM_SYS_PARTITION; i++) {
            IO_UtilityStringCopy((char *)pPartEntry[i].PartitionName, sizeof(pPartEntry[i].PartitionName), pSysPartConfig[i].PartitionName);     /* Partition Name */
            pPartEntry[i].Attribute      = pSysPartConfig[i].Attribute;    /* Attribute of the Partition */
            pPartEntry[i].ByteCount      = pSysPartConfig[i].ByteCount;    /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0;                               /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            if (BlkAddr >= TotalNumBlk) {
                break;
            }
        }

        /* Configure User Partition Table */
        /* Configure remaining user partitions, except the partition of USER_PTB */
        pAmbaRTSL_NorSpiUserPartTable->PtbMagic = AMBA_USER_PTB_MAGIC;
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[0]);
        for (i = 1U; i < AMBA_NUM_USER_PARTITION; i++) {
            /* Partition Name */
            IO_UtilityStringCopy((char *)pPartEntry[i].PartitionName, sizeof(pPartEntry[i].PartitionName), pUserPartConfig[i].PartitionName);

            pPartEntry[i].Attribute      = pUserPartConfig[i].Attribute;   /* Attribute of the Partition */
            pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;   /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0;                               /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

#if defined(CONFIG_MUTI_BOOT_DEVICE)
            if (((pPartEntry[i].Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) != AMBA_NVM_SPI_NOR) {
                continue;
            }
#endif

            if (pPartEntry[i].ByteCount == 0U) {
                pPartEntry[i].StartBlkAddr = 0;
                pPartEntry[i].BlkCount = 0;
            } else {
                pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
                BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            }
            if (BlkAddr >= TotalNumBlk) {
                break;
            }
        }
    }

    Rtsl_UserPtbNextBlock = BlkAddr;

    return RetVal;
}

UINT32 AmbaRTSL_SpiNorCreateExtUserPTB(UINT32 StartBlk)
{
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = AmbaRTSL_NorSpiCtrl.pUserPartConfig;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 BlkByteSize, BlkAddr, TotalNumBlk;
    UINT32 i, RetVal = OK;

    if (pUserPartConfig == NULL) {
        RetVal = SPINOR_ERR_ARG;
    } else {
        /* Block size in Byte */
        pAmbaRTSL_NorSpiSysPartTable->BlkByteSize = AmbaRTSL_NorSpiCtrl.PartitionAllocateSize;

        BlkByteSize = AmbaRTSL_NorSpiCtrl.PartitionAllocateSize;
        TotalNumBlk = AmbaRTSL_NorSpiCtrl.TotalByteSize / BlkByteSize;

        pAmbaRTSL_NorSpiUserPartTable->PTBNumber = 1U;
        pAmbaRTSL_NorSpiUserPartTable->PTBActive = 0U;
        pAmbaRTSL_NorSpiUserPartTable->BlkAddrCurentPTB += 1U;
        BlkAddr = StartBlk;

        /* Configure User Partition Table */
        /* Configure remaining user partitions, except the partition of VENDOR_DATA */
        pAmbaRTSL_NorSpiUserPartTable->PtbMagic = AMBA_USER_PTB_MAGIC;
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[0]);
        for (i = 1U; i < AMBA_NUM_USER_PARTITION; i++) {

            if ((pUserPartConfig[i].Attribute & AMBA_PARTITION_ATTR_BACKUP) == 0U) {
                continue;
            }
            pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;   /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0;                               /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            if (pPartEntry[i].ByteCount == 0U) {
                pPartEntry[i].StartBlkAddr = 0;
                pPartEntry[i].BlkCount = 0;
            } else {
                pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
                BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            }
            if (BlkAddr >= TotalNumBlk) {
                break;
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiReadSysPTB
 *
 *  @Description:: Read NORSPI System Partition Table (Block 0, Page 2)
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
UINT32 AmbaRTSL_NorSpiReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable)
{
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo;     /* Pointer to external NAND device information */
    UINT8 *pWorkBufMain = NorPartBuf;
    UINT32 RetVal;

    /* Read System partition table from NAND which is located at block-0 page-2 */
    RetVal = AmbaRTSL_NOR_Readbyte(AmbaRTSL_NorSpiCtrl.BstPageCount     * pNorDevInfo->PageSize,
                                   AmbaRTSL_NorSpiCtrl.SysPtblPageCount * pNorDevInfo->PageSize,
                                   pWorkBufMain);
    if (RetVal == OK) {
        /* copy to the DRAM memory */
        if (0U != AmbaWrap_memcpy(pAmbaRTSL_NorSpiSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };

        /* copy to the user area while needed */
        if ((pSysPartTable != NULL) && ((UINT8 *) pSysPartTable != pWorkBufMain)) {
            if (0U != AmbaWrap_memcpy(pSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiWriteSysPTB
 *
 *  @Description:: Write System Partition Table (Block 0, Page 2)
 *
 *  @Input      ::
 *      pSysPartTable: pointer to the data of System partition table
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NorSpiWriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable)
{
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo; /* Pointer to external NAND device information */
    UINT32 BstPageCount     = AmbaRTSL_NorSpiCtrl.BstPageCount;        /* Number of Pages for BST */
    UINT32 SysPtblPageCount = AmbaRTSL_NorSpiCtrl.SysPtblPageCount;    /* Number of Pages for System Partition Table */
    UINT32 PageSize = AmbaRTSL_NorSpiDevInfo->PageSize;

    UINT8 *pWorkBufMain = NorPartBuf;
    AMBA_SYS_PARTITION_TABLE_s *pWorkSysPartTable;
    UINT32 RetVal;

    if (pSysPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkSysPartTable = pSysPartTable;
    } else {
        /* update NORSPI through DRAM Partition Table */
        pWorkSysPartTable = pAmbaRTSL_NorSpiSysPartTable;
    }

    /* 1. Read BST data (block 0, page 0) */
    RetVal = AmbaRTSL_NOR_Readbyte(0, BstPageCount * PageSize, pWorkBufMain);
    if (RetVal == OK) {
        /* 2. Erase the block 0 */
        RetVal = AmbaRTSL_NorErase(0, pNorDevInfo->EraseBlockSize);
    }

    if (RetVal == OK) {
        /* 3. Write back BST data (block 0, page 0) */
        RetVal = AmbaRTSL_NorProgram(0, BstPageCount * PageSize, pWorkBufMain);
    }

    if (RetVal == OK) {
        /* 4. Calculate CRC32 */
        const UINT8 *pTmp = NULL;
        UINT32 CrcSize = (UINT32)((sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U) & 0xffffffffU);

        AmbaMisra_TypeCast(&pTmp, &pWorkSysPartTable);
        pWorkSysPartTable->CRC32 = IO_UtilityCrc32(pTmp, CrcSize);

        /* 5. Write new System Partition Table (block 0, page 2) */
        RetVal = AmbaRTSL_NorProgram(BstPageCount     * PageSize,
                                     SysPtblPageCount * PageSize,
                                     (UINT8 *) pWorkSysPartTable);
        if ((RetVal == OK) && (pWorkSysPartTable != pAmbaRTSL_NorSpiSysPartTable)) {
            /* copy to the DRAM memory */
            if (0U != AmbaWrap_memcpy(pAmbaRTSL_NorSpiSysPartTable, pWorkSysPartTable, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiReadUserPTB
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
UINT32 AmbaRTSL_NorSpiReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo)
{
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo; /* Pointer to external NAND device information */
    UINT32 PageSize = AmbaRTSL_NorSpiDevInfo->PageSize;
    UINT8 *pWorkBufMain = NorPartBuf;
    UINT32 RetVal;
    UINT32 UserPtblBlkAddr;

    UserPtblBlkAddr = pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB + UserPtbNo;

    /* Read User partition table from NORSPI Flash */
    RetVal = AmbaRTSL_NOR_Readbyte(UserPtblBlkAddr * pNorDevInfo->EraseBlockSize,
                                   AmbaRTSL_NorSpiCtrl.UserPtblPageCount * PageSize,
                                   pWorkBufMain);
    if (RetVal == OK) {
        /* copy to the DRAM memory */
        if (0U != AmbaWrap_memcpy(pAmbaRTSL_NorSpiUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };

        /* copy to the user area while needed */
        if ((pUserPartTable != NULL) && ((UINT8 *) pUserPartTable != pWorkBufMain)) {
            if (0U != AmbaWrap_memcpy(pUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiWriteUserPTB
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
UINT32 AmbaRTSL_NorSpiWriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo)
{
    UINT8  *pWorkBufMain = NorPartBuf;
    AMBA_USER_PARTITION_TABLE_s *pWorkUserPartTable = pUserPartTable;
    UINT32 RetVal;

    UINT32 PageSize    = AmbaRTSL_NorSpiDevInfo->PageSize;
    UINT32 BlkByteSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;

    UINT32 UserPtblBlkAddr     = pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB + UserPtbNo;  /* Block address of User Partition Table */
    UINT32 UserPtblPageCount   = AmbaRTSL_NorSpiCtrl.UserPtblPageCount;         /* Number of Pages for User Partition Table */
    UINT32 VendorDataPageCount = AmbaRTSL_NorSpiCtrl.VendorDataPageCount;       /* Number of Pages for Vendor Specific Data */

    UINT32 UserPtblAddr   = UserPtblBlkAddr * BlkByteSize;
    UINT32 VendorDataAddr = UserPtblAddr + (UserPtblPageCount * PageSize);

    if (pUserPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkUserPartTable = pUserPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkUserPartTable = pAmbaRTSL_NorSpiUserPartTable;
    }

    /* 1. Read metadata */
    RetVal = AmbaRTSL_NOR_Readbyte(UserPtblBlkAddr * BlkByteSize, VendorDataPageCount * PageSize, pWorkBufMain);
    if (RetVal == OK) {
        /* 2. Erase the block-1 */
        RetVal = AmbaRTSL_NorErase(UserPtblBlkAddr * BlkByteSize, BlkByteSize);
    }

    if (RetVal == OK) {
        /* 3. Write back Vendor Data (block 1, page 1) */
        RetVal = AmbaRTSL_NorProgram(VendorDataAddr, VendorDataPageCount * PageSize, pWorkBufMain);
    }

    if (RetVal == OK) {
        /* 4. Calculate CRC32 */
        const UINT8 *pTmp = NULL;
        UINT32 CrcSize = (UINT32)(AMBA_USER_PTB_CRC32_SIZE & 0xffffffffU);

        AmbaMisra_TypeCast(&pTmp, &pWorkUserPartTable);
        pWorkUserPartTable->CRC32 = IO_UtilityCrc32(pTmp, CrcSize);

        /* 5. Write new User partition table (block 1, page 0) */
        RetVal = AmbaRTSL_NorProgram(UserPtblAddr, UserPtblPageCount * PageSize, (UINT8 *) pWorkUserPartTable);
        if ((RetVal == 0U) && (pWorkUserPartTable != pAmbaRTSL_NorSpiUserPartTable)) {
            /* copy to the DRAM memory */
            if (0U != AmbaWrap_memcpy(pAmbaRTSL_NorSpiUserPartTable, pWorkUserPartTable, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiGetPartEntry
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
AMBA_PARTITION_ENTRY_s *AmbaRTSL_NorSpiGetPartEntry(UINT32 PartFlag, UINT32 PartID)
{
    AMBA_PARTITION_ENTRY_s *pPartEntry;

    if (PartFlag != 0U) {
        /* User Partition */
        if (PartID >= AMBA_NUM_USER_PARTITION) {
            pPartEntry = NULL;    /* Wrong Parameter */
        } else {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[PartID]);
        }
    } else {
        /* System Partition */
        if (PartID >= AMBA_NUM_SYS_PARTITION) {
            pPartEntry = NULL;    /* Wrong Parameter */
        } else {
            pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiSysPartTable->Entry[PartID]);
        }
    }

    return pPartEntry;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiReadPartitionInfo
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
 *          UINT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NorSpiReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    UINT32 RetVal = OK;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);

    if ((pPartInfo == NULL) || (pPartEntry == NULL)) {
        RetVal = SPINOR_ERR_ARG;
    } else {
        if (0U != AmbaWrap_memcpy(pPartInfo, pPartEntry, sizeof(AMBA_PARTITION_ENTRY_s))) { /* Misrac */ };
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiReadPartition
 *
 *  @Description:: Read partition data from NOR flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *
 *  @Output     ::
 *      pDataBuf: pointer to partition data
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
 */
UINT32 AmbaRTSL_NorSpiReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf)
{
    UINT32 BlkByteSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 RetVal;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);

    if ((pDataBuf == NULL) || (pPartEntry == NULL)) {
        RetVal = SPINOR_ERR_ARG;  /* Wrong Parameter */
    } else {
        RetVal = AmbaRTSL_NOR_Readbyte(pPartEntry->StartBlkAddr * BlkByteSize,
                                       pPartEntry->ActualByteSize,
                                       pDataBuf);
    }

    return RetVal;
}

static UINT32 AmbaRTSL_NorSpiWritePartitionLoop(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf)
{
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo;
    UINT32 RetVal = 0U, FirmOK = 0U, CmpResult = 0U;
    UINT32 Block, Page, PreOffset, Offset = 0U;
    UINT32 PageAddr;
    UINT8 *pMainBuf;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);
    UINT32 PageSize     = pNorDevInfo->PageSize;
    UINT32 BlkByteSize  = pNorDevInfo->EraseBlockSize;
    UINT32 PagePerBlock = BlkByteSize / PageSize;;

    for (Block = pPartEntry->StartBlkAddr; Block < (pPartEntry->StartBlkAddr + pPartEntry->BlkCount); Block++) {
        RetVal = AmbaRTSL_NorErase(Block * BlkByteSize, BlkByteSize);

        if (RetVal == SPINOR_ERR_NONE) {
            /* erase the unused block after program ok */
            if (FirmOK != 1U) {
                PreOffset = Offset;
                /* Program each page */
                for (Page = 0U; Page < PagePerBlock; Page++) {
                    pMainBuf = &pDataBuf[Offset];
                    PageAddr = (Block * PagePerBlock) + Page;

                    /* Program a page */
                    RetVal = AmbaRTSL_NorProgram(PageAddr * PageSize, PageSize, pMainBuf);
                    if (RetVal == SPINOR_ERR_NONE) {
                        /* Read it back for verification */
                        RetVal = AmbaRTSL_NOR_Readbyte(PageAddr * PageSize, PageSize, NorPartBuf);
                    }

                    if (RetVal == SPINOR_ERR_NONE) {
                        if (0U != AmbaWrap_memcmp(pMainBuf, NorPartBuf, PageSize, &CmpResult)) {
                            /* Compare memory content after read back */
                        }
                        if (CmpResult != 0U) {
                            RetVal = SPINOR_ERR_IO_FAIL;
                        }
                        if (FirmOK == 0U) {
                            Offset += PageSize;
                            if (Offset >= DataSize) {
                                FirmOK = 1U;
                            }
                        }
                    }
                    if (RetVal != SPINOR_ERR_NONE) {
                        RetVal = SPINOR_ERR_IO_FAIL;
                    }

                    if ((RetVal != 0U) ||
                        ((FirmOK == 1U) && (PartID == AMBA_SYS_PARTITION_BOOTSTRAP))) {
                        break;
                    }
                }
                if (RetVal != SPINOR_ERR_NONE) {
                    Offset = PreOffset;
                }
            }
        }
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiWritePartition
 *
 *  @Description:: Write partition data to flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   partition ID
 *      DataSize: Data size in Byte to write to the partition
 *      pDataBuf: pointer to partition data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : SpiNOR error code
 */
UINT32 AmbaRTSL_NorSpiWritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf)
{
    UINT32 RetVal = 0U;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);

    if ((pDataBuf == NULL) || (pPartEntry == NULL)) {
        RetVal = SPINOR_ERR_ARG;
    } else {
        RetVal = AmbaRTSL_NorSpiWritePartitionLoop(PartFlag, PartID, DataSize, pDataBuf);

        /* 3. Update partition table */
        if (RetVal == SPINOR_ERR_NONE) {
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
            pPartEntry->ActualByteSize = DataSize;
            pPartEntry->ProgramStatus  = 0U;
            pPartEntry->ImageCRC32     = IO_UtilityCrc32(pDataBuf, DataSize);
            if (0x0U != PartFlag) {
                /* User Partition */
                RetVal = AmbaRTSL_NorSpiWriteUserPTB(pAmbaRTSL_NorSpiUserPartTable, pAmbaRTSL_NorSpiUserPartTable->PTBNumber);
            } else {
                /* System Partition */
                RetVal = AmbaRTSL_NorSpiWriteSysPTB(NULL);
            }
        }
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiErasePartition
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
UINT32 AmbaRTSL_NorSpiErasePartition(UINT32 PartFlag, UINT32 PartID)
{
    UINT32 RetVal;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);

    if (pPartEntry == NULL) {
        RetVal = SPINOR_ERR_ARG;  /* Wrong Parameter */
    } else {
        /* 1. Erase all blocks in target partition */
        UINT32 BlkByteSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
        UINT32 BlkCount    = pPartEntry->BlkCount;
        UINT32 BlkAddr     = pPartEntry->StartBlkAddr * BlkByteSize;

        RetVal = AmbaRTSL_NorErase(BlkAddr, BlkCount * BlkByteSize);
        if (RetVal == OK) {
            /* 2. Update partition table */
            pPartEntry->Attribute      = SetBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED); /* Erased */
            pPartEntry->ImageCRC32     = 0xffffffffU;
            pPartEntry->ProgramStatus  = 0xffffffffU;
            pPartEntry->ActualByteSize = 0;

            if (PartFlag != 0U) {
                RetVal = AmbaRTSL_NorSpiWriteUserPTB(NULL, pAmbaRTSL_NorSpiUserPartTable->PTBNumber); /* User Partition */
            } else {
                RetVal = AmbaRTSL_NorSpiWriteSysPTB(NULL);  /* System Partition */
            }
        }
    }
    return RetVal;
}

void AmbaRTSL_NorSpiSetWritePtbFlag(UINT32 Flag)
{
    WritePtbFlag = Flag;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiIsBldMagicCodeSet
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
UINT32 AmbaRTSL_NorSpiIsBldMagicCodeSet(void)
{
    UINT32 RetVal;

    if (pAmbaRTSL_NorSpiUserPartTable->BldMagicCode == AMBA_USER_PTB_MAGIC) {
        RetVal = 1U;
    } else {
        RetVal = 0;
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiSetBldMagicCode
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
UINT32 AmbaRTSL_NorSpiSetBldMagicCode(void)
{
    UINT32 RetVal = 0;
    UINT32 PTBActive;

    (void)AmbaRTSL_NorSpiGetActivePtbNo(&PTBActive);

    if (pAmbaRTSL_NorSpiUserPartTable->BldMagicCode != AMBA_USER_PTB_MAGIC) {
        /* Write Bootloader Magic Code */
        pAmbaRTSL_NorSpiUserPartTable->BldMagicCode = AMBA_USER_PTB_MAGIC; /* Bootloader Magic Code */
        RetVal = AmbaRTSL_NorSpiWriteUserPTB(NULL, PTBActive);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiEraseBldMagicCode
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
UINT32 AmbaRTSL_NorSpiEraseBldMagicCode(void)
{
    UINT32 RetVal = 0;
    UINT32 PTBActive;

    (void)AmbaRTSL_NorSpiGetActivePtbNo(&PTBActive);

    if (pAmbaRTSL_NorSpiUserPartTable->BldMagicCode == AMBA_USER_PTB_MAGIC) {
        /* Erase Bootloader Magic Code */
        pAmbaRTSL_NorSpiUserPartTable->BldMagicCode = 0xffffffffU; /* Bootloader Magic Code */
        RetVal = AmbaRTSL_NorSpiWriteUserPTB(NULL, PTBActive);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiReadBldMagicCode
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
UINT32 AmbaRTSL_NorSpiReadBldMagicCode(void)
{
    return pAmbaRTSL_NorSpiUserPartTable->BldMagicCode;
}

#if 0
/*
 *  @RoutineName:: AmbaRTSL_NorSpiReadVendorData
 *
 *  @Description:: Read NAND Vendor Data (Block 1, Page 1)
 *
 *  @Input      ::
 *      pDataBuf: pointer to the buffer of NAND Vendor Data
 *
 *  @Output     ::
 *      pDataBuf: pointer to the data of NAND Vendor Data
 *
 *  @Return     ::
 *          INT32 : actual size of Vendor Data (>0)/NG(-1)
 */
INT32 AmbaRTSL_NorSpiReadVendorData(void *pDataBuf)
{
    AMBA_PARTITION_ENTRY_s *pPartEntry = &(pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_PTB]);
    UINT32 UserPtblBlkAddr   = pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB;  /* Block address of User Partition Table */
    UINT32 UserPtblAddr  = UserPtblBlkAddr * AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 UserPtblPageCount = AmbaRTSL_NorSpiCtrl.UserPtblPageCount;            /* Number of Pages for User Partition Table */
    UINT32 PageSize = AmbaRTSL_NorSpiDevInfo->PageSize;
    UINT32 VendorDataOffset;
    UINT8  *pWorkBufMain = NorPartBuf;
    INT32 RetVal;

    if (pDataBuf == NULL)
        return NG;  /* Wrong Parameter */

    VendorDataOffset = UserPtblAddr + UserPtblPageCount * PageSize;
    if ((RetVal = pPartEntry->ActualByteSize) > 0) {
        /* Meta data is located at block 1 page 1, following USER_PTB */
        RetVal = AmbaRTSL_NOR_Readbyte(VendorDataOffset,
                                       AmbaRTSL_NorSpiCtrl.VendorDataPageCount * PageSize,
                                       pWorkBufMain);
    }

    if (RetVal == 0) {
        RetVal = pPartEntry->ActualByteSize;
        if (0U != AmbaWrap_memcpy(pDataBuf, pWorkBufMain, RetVal)) { /* Misrac */ };
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_NorSpiWriteVendorData
 *
 *  @Description:: Write NAND Vendor Data (Block 1, Page 1)
 *
 *  @Input      ::
 *      pDataBuf: pointer to the data of metadata
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : actual size of Vendor Data (>0)/NG(-1)
 */
INT32 AmbaRTSL_NorSpiWriteVendorData(UINT32 DataSize, void *pDataBuf)
{
    AMBA_PARTITION_ENTRY_s *pPartEntry = &(pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_PTB]);
    AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo;         /* Pointer to external NORSPI device information */
    UINT32 BlkByteSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 PageSize = AmbaRTSL_NorSpiDevInfo->PageSize;
    UINT32 UserPtblBlkAddr, VendorDataOffset;
    UINT32 UserPtblPageCount;
    INT32 RetVal;

    if (DataSize == 0 || pDataBuf == NULL || pNorDevInfo == NULL)
        return NG;  /* wrong parameter */

    UserPtblBlkAddr   = pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB;    /* Block address of User Partition Table */
    UserPtblPageCount = AmbaRTSL_NorSpiCtrl.UserPtblPageCount;           /* Number of Pages for User Partition Table */

    /* 1. Erase the block-1 */
    if ((RetVal = AmbaRTSL_NorErase(UserPtblBlkAddr * BlkByteSize, BlkByteSize)) != OK)
        return RetVal;

    VendorDataOffset = UserPtblBlkAddr * BlkByteSize + UserPtblPageCount * PageSize;
    /* 2. Write new meta data (block 1, page 1) */
    RetVal = AmbaRTSL_NorProgram(VendorDataOffset, DataSize, pDataBuf);
    if (RetVal == 0)
        pPartEntry->ActualByteSize = DataSize;

    /* 3. Write back User Partition Table (block 1, page 0) */
    RetVal = AmbaRTSL_NorProgram(UserPtblBlkAddr * BlkByteSize, UserPtblPageCount * PageSize,
                                 (UINT8 *) pAmbaRTSL_NorSpiUserPartTable);
    if (RetVal == 0)
        RetVal = pPartEntry->ActualByteSize;

    return RetVal;
}

#endif

