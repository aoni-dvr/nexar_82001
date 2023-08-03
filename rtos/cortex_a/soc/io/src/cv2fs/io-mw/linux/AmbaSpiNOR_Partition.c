/**
 *  @file AmbaSpiNOR_Partition.c
 *
 *  @Copyright      :: Copyright (C) 2019 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND Partition Management APIs
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaWrap.h"

#include "AmbaKAL.h"
#include "AmbaIOUtility.h"
#include "AmbaMisraFix.h"

#include "AmbaSPINOR.h"
#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaNVM_Ctrl.h"
//#include "AmbaRTSL_SpiNOR.h"
//#include "AmbaRTSL_SpiNOR_Ctrl.h"

#define SEC_SIZE 512U



/*
 *  @RoutineName:: AmbaSpiNOR_InitPtbBbt
 *
 *  @Description:: Init BBT and System/User Partition Tables
 *
 *  @Input      ::
 *      TimeOut: Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_InitPtbBbt(UINT32 TimeOut)
{
    UINT32 RetVal = 1;
    (void) TimeOut;
    return RetVal;
}

#if 0
/*
 *  @RoutineName:: AmbaSpiNOR_CreatePTB
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
INT32 AmbaSpiNOR_CreatePTB(void)
{
    AMBA_PARTITION_CONFIG_s *pSysPartConfig = AmbaRTSL_NorSpiCtrl.pSysPartConfig;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = AmbaRTSL_NorSpiCtrl.pUserPartConfig;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 i, BlkByteSize;
    UINT32 BlkAddr, TotalNumBlk;                /* Total number of blocks */
    INT32 RetVal = 0;

    if ((pSysPartConfig == NULL) || (pUserPartConfig == NULL))
        return -1;

    /* Block size in Byte */
    BlkByteSize = pAmbaRTSL_NorSpiSysPartTable->BlkByteSize = AmbaRTSL_NorSpiCtrl.PartitionAllocateSize;
    TotalNumBlk = AmbaRTSL_NorSpiCtrl.TotalByteSize / BlkByteSize;

    /* Block-0: Reserved for Bootstrap & System Partition Table */
    /* next Block: Reserved for User Partition Table and Vendor Data, AMBA_USER_PARTITION_PTB must be 0 !!! */

    if (RetVal == OK) {
        /* Configure sys partition: BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_NorSpiSysPartTable->Entry[AMBA_SYS_PARTITION_BOOTSTRAP]);
        IO_UtilityStringCopy(pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pSysPartConfig->PartitionName);     /* Partition Name */
        pPartEntry->Attribute = pSysPartConfig->Attribute;    /* Attribute of the Partition */
        pPartEntry->ByteCount      = pSysPartConfig->ByteCount;         /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = 0;                                 /* start Block Address = 0 */


        /* Block address of User Partition Table */
        BlkAddr = pPartEntry->BlkCount + GetRoundUpValU32(sizeof(AMBA_SYS_PARTITION_TABLE_s), BlkByteSize);
        pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB = BlkAddr;
        BlkAddr += GetRoundUpValU32(sizeof(AMBA_USER_PARTITION_TABLE_s), BlkByteSize);

        /* Configure user partition VENDOR_DATA */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_PTB]);
        IO_UtilityStringCopy((char *)pPartEntry->PartitionName, sizeof(pPartEntry->PartitionName), pSysPartConfig->PartitionName);     /* Partition Name */
        pPartEntry->Attribute = pUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = pUserPartConfig->ByteCount;        /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = 1;                                 /* number of Blocks for the Partition: fixed to be 1  */
        pPartEntry->StartBlkAddr   = BlkAddr;                           /* start Block Address = pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB */
        BlkAddr += pPartEntry->BlkCount;

        /* Configure System Partition Table */
        /* Configure remaining sys partitions, except the partition of BOOTSTRAP */
        pSysPartConfig++;
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_NorSpiSysPartTable->Entry[1]);
        for (i = 1; RetVal == OK && i < AMBA_NUM_SYS_PARTITION && BlkAddr < TotalNumBlk; i++, pSysPartConfig++, pPartEntry++) {
            (void)AmbaWrap_memcpy(pPartEntry->PartitionName, pSysPartConfig->PartitionName, sizeof(pPartEntry->PartitionName));     /* Partition Name */
            pPartEntry->Attribute = pSysPartConfig->Attribute;    /* Attribute of the Partition */
            pPartEntry->ByteCount      = pSysPartConfig->ByteCount;         /* number of Bytes for the Partition */
            pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
            pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            pPartEntry->StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry->BlkCount;                /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
        }

        /* Configure User Partition Table */
        /* Configure remaining user partitions, except the partition of VENDOR_DATA */
        pUserPartConfig++;
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[1]);
        for (i = 1; RetVal == OK && i < AMBA_NUM_USER_PARTITION && BlkAddr < TotalNumBlk; i++, pUserPartConfig++, pPartEntry++) {
            /* Partition Name */
            if (i == AMBA_USER_PARTITION_FAT_DRIVE_A)
                continue;

            (void)AmbaWrap_memcpy(pPartEntry->PartitionName, pUserPartConfig->PartitionName, sizeof(pPartEntry->PartitionName));
            pPartEntry->Attribute      = pUserPartConfig->Attribute;   /* Attribute of the Partition */
            pPartEntry->ByteCount      = pUserPartConfig->ByteCount;        /* number of Bytes for the Partition */
            pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
            pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            if (pPartEntry->ByteCount == 0U) {
                pPartEntry->StartBlkAddr = 0;
                pPartEntry->BlkCount = 0;
                continue;
            }

            pPartEntry->StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry->BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
        }
    }

    return RetVal;
}
#endif


#if 0
extern UINT32 decompress(UINT32 input_start, UINT32 input_end, UINT32 output_start);
static AMBA_KAL_BYTE_POOL_t GNU_WEAK_SYMBOL AmbaBytePool_NonCached;

static AMBA_NVM_ROM_MEMORY_CTRL_s _AmbaNorSpi_RomMemCtrl[AMBA_NUM_NVM_ROM_REGION] = {
    [AMBA_NVM_ROM_REGION_DSP_uCODE] = {
        .IsInit = 0,
        .RomUnzipBuf = {0},
        .RomLoadBuf = {0},
    },
    [AMBA_NVM_ROM_REGION_SYS_DATA] = {
        .IsInit = 0,
        .RomUnzipBuf = {0},
        .RomLoadBuf = {0},
    },
};

/*
 *  @RoutineName:: AmbaSpiNOR_ROMRead
 *
 *  @Description:: Load Data of ROM File from Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaSpiNOR_ROMReadByMemory(UINT32 RegionID, UINT32 StartPos, UINT32 Size,
        UINT8 *pDataBuf, UINT32 Timeout)
{
    UINT32 RetVal = SPINOR_ERR_NONE;
    UINT32 UnzipSize = 0, ImgSize = 0, PartID;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;

    if ((RegionID >= AMBA_NUM_NVM_ROM_REGION) || (Size == 0U) || (pDataBuf == NULL)) {
        RetVal = SPINOR_ERR_ARG;
    } else if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, AMBA_USER_PARTITION_DSP_uCODE);
        PartID = AMBA_USER_PARTITION_DSP_uCODE;
        UnzipSize = pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_DSP_uCODE].Attribute.Bits.Reserved;
    } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, AMBA_USER_PARTITION_SYS_DATA);
        PartID = AMBA_USER_PARTITION_SYS_DATA;
        UnzipSize = pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_SYS_DATA].Attribute.Bits.Reserved;
    } else {
        RetVal = SPINOR_ERR_ARG;
    }

    if (pPartEntry != NULL) {
        ImgSize = pPartEntry->ActualByteSize;
        if ((UnzipSize == 0U) || (ImgSize == 0U)) {
            RetVal = SPINOR_ERR_ARG;
        }
    }

    if (_AmbaNorSpi_RomMemCtrl[RegionID].IsInit == 0) {
        UINT8 *pTempBuf = NULL, *pUnzipBuf = NULL;
        /* Load compressed image to TempBuf */
        if (_AmbaNorSpi_RomMemCtrl[RegionID].RomLoadBuf.pMemAlignedBase == NULL) {
            RetVal = AmbaKAL_MemAllocate(&AmbaBytePool_NonCached, &_AmbaNorSpi_RomMemCtrl[RegionID].RomLoadBuf, ImgSize, 32);
            if (RetVal < 0) {
                /* DBGMSG("%s get temp_buf %d fail!!", __func__, ImgSize); */
                return 0;
            }
        }
        pTempBuf = _AmbaNorSpi_RomMemCtrl[RegionID].RomLoadBuf.pMemAlignedBase;
        RetVal = AmbaSpiNOR_ReadPartition(1, PartID, pTempBuf, 1000);
        if (RetVal < 0)
            return NG;

        if (_AmbaNorSpi_RomMemCtrl[RegionID].RomUnzipBuf.pMemAlignedBase == NULL) {
            RetVal = AmbaKAL_MemAllocate(&AmbaBytePool_NonCached, &_AmbaNorSpi_RomMemCtrl[RegionID].RomUnzipBuf, UnzipSize, 32);
            if (RetVal < 0) {
                /* DBGMSG("%s get NVM region: %d Buf Size:%d fail!!", __func__, RegionID, UnzipSize); */
                goto Done;
            } else
                /* DBGMSG("%s get NVM region: %d Buf Size:%d buf address:0x%x fail!!", __func__, RegionID, UnzipSize, &_AmbaNorSpi_RomMemCtrl[RegionID].RomUnzipBuf.pMemAlignedBase); */
            }
        RetVal = decompress((uintptr_t)pTempBuf, (uintptr_t)(pTempBuf + ImgSize),
                            (uintptr_t) _AmbaNorSpi_RomMemCtrl[RegionID].RomUnzipBuf.pMemAlignedBase);
        if (RetVal == 0)
            //DBGMSG("%s Decompress ROM(%d) fail!!", __func__, RegionID);

            _AmbaNorSpi_RomMemCtrl[RegionID].IsInit = 1;
    }
    pUnzipBuf = (UINT8 *)_AmbaNorSpi_RomMemCtrl[RegionID].RomUnzipBuf.pMemAlignedBase;
    (void)AmbaWrap_memcpy(pDataBuf, pUnzipBuf + StartPos, Size);
    RetVal = Size;

Done:
    if (pTempBuf != NULL) {
        AmbaKAL_MemFree(&_AmbaNorSpi_RomMemCtrl[RegionID].RomLoadBuf);
    }

    return RetVal;
}
#endif

/*
 *  @RoutineName:: AmbaSpiNOR_LoadNvmRomFileTable
 *
 *  @Description:: Load all NAND ROM File Tables
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_LoadNvmRomFileTable(void)
{
    UINT32 RetVal = SPINOR_ERR_NONE;
    return RetVal;
}

UINT32 AmbaSpiNOR_GetRomFileSize(UINT32 RegionID, const char *pFileName, UINT32 *pFileSize)
{
    UINT32 RetVal = NVM_ERR_FILE_NOT_FOUND;
    (void) RegionID;
    (void) pFileName;
    (void) pFileSize;
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
 *          INT32 : (>0)/NG(-1)
 */
UINT32 AmbaSpiNOR_GetRomFileInfo(UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo)
{
    UINT32 RetVal = NVM_ERR_NONE;
    (void) RegionID;
    (void) Index;
    (void) pRomFileInfo;
    return RetVal;
}

/*
 *  @RoutineName:: AmbaSpiNOR_ReadRomFile
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_ReadRomFile(UINT32 RegionID, const char *pFileName, UINT32 StartPos,
                              UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = SPINOR_ERR_NONE;
    (void) RegionID;
    (void) pFileName;
    (void) StartPos;
    (void) Size;
    (void) pDataBuf;
    (void) TimeOut;
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNorSpi_ReadSysPartitionTable
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_ReadSysPartitionTable(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    UINT32 RetVal = 1;
    (void) pSysPartTable;
    (void) TimeOut;
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNorSpi_WriteSysPartitionTable
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_WriteSysPartitionTable(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    UINT32 RetVal = 1;
    (void) pSysPartTable;
    (void) TimeOut;

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNorSpi_ReadUserPartitionTable
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_ReadUserPartitionTable(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    UINT32 RetVal = 1;
    (void) pUserPartTable;
    (void) UserPtbNo;
    (void) TimeOut;
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNorSpi_WriteUserPartitionTable
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_WriteUserPartitionTable(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    (void) pUserPartTable;
    (void) UserPtbNo;
    (void) TimeOut;
    return 1;
}

/*
 *  @RoutineName:: AmbaSpiNOR_ReadPartitionInfo
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
 *          INT32 : actual size of Vendor Specific Data (>0)/NG(-1)
 */
UINT32 AmbaSpiNOR_ReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    (void) PartFlag;
    (void) PartID;
    (void) pPartInfo;
    return 1;//AmbaRTSL_NorSpiReadPartitionInfo(PartFlag, PartID, pPartInfo);
}

/*
 *  @RoutineName:: AmbaSpiNOR_ReadPartition
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_ReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut)
{
    (void) PartFlag;
    (void) PartID;
    (void) pDataBuf;
    (void) TimeOut;
    return 1;
}

#if 0
/*
 *  @RoutineName:: AmbaNOR_ProgBlock
 *
 *  @Description:: Program the partition to NOR device.
 *
 *  @Input      ::
 *          UINT8 *pRaw     : Pointer to raw image.
 *          UINT32 RawSize  : Raw image size.
 *          UINT32 StartBlk : Start block to be programmed.
 *          UINT32 NumBlk   : Number of blocks to be programmed
 *          INT32 (*OutputProgress)(INT32, void *)  : Function to report program process
 *          void *pArg      : Function argument.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 *-*/
INT32 AmbaNOR_ProgBlock(UINT8 *pRaw, UINT32 RawSize, UINT32 StartBlk, UINT32 NumBlks)
{
    INT32 RetVal = 0, FirmOK = 0;
    UINT32 Block, Page, Offset, PreOffset, BlkSize, PagesPerBlock, PageSize;
    AMBA_NOR_DEV_s *pDev;

    pDev = AmbaRTSL_NorGetDev();
    K_ASSERT(pDev->DevLogicInfo.EraseBlockSize != 0x0 ||
             pDev->DevLogicInfo.PageSize != 0x0);

    BlkSize = pDev->DevLogicInfo.EraseBlockSize;
    PageSize = pDev->DevLogicInfo.PageSize;
    PagesPerBlock = BlkSize / PageSize;

    /* Program image into the Norflash */
    Offset = 0;
    for (Block = StartBlk; Block < (StartBlk + NumBlks); Block++) {

        RetVal = AmbaNOR_Erase(Block * BlkSize, BlkSize);
        if (RetVal < 0) {
            //DBGMSG ("erase failed. <block %d>", Block);
            return -1;
        }

        /* erase the unused block after program ok */
        if (FirmOK == 1)
            continue;

        PreOffset = Offset;

        /* Program each page */
        for (Page = 0; Page < PagesPerBlock; Page++) {
            /* Program a page */
            RetVal = AmbaNOR_Program(Block * BlkSize + Page * PageSize, PageSize, pRaw + Offset);
            if (RetVal < 0) {
                //DBGMSG ("program failed. <block %d, page %d>", Block, Page);
                break;
            }

            /* Read it back for verification */
            RetVal = AmbaNOR_Readbyte(Block * BlkSize + Page * PageSize, PageSize, g_NorOP.pPageBuf);
            if (RetVal < 0) {
                //DBGMSG ("read failed. <block %d, page %d>", Block, Page);
                break;
            }

            /* Compare memory content after read back */
            RetVal = memcmp(pRaw + Offset, g_NorOP.pPageBuf, PageSize);
            if (RetVal != 0) {
                //DBGMSG ("check failed. <block %d, page %d>", Block, Page);
                RetVal = -1;
                break;
            }
            Offset += PageSize;

            if (Offset >= RawSize) {
                FirmOK = 1;
                break;
            }
        }

        if (RetVal < 0) {
            Offset = PreOffset;
            return -1;
        }
    }
    if (RetVal < 0 || FirmOK == 0)
        RetVal = -1;

    return RetVal;
}
#endif

UINT32 AmbaSPINOR_InitUserPartLock(void)
{
    UINT32 RetVal = OK;
    return RetVal;
}

UINT32 AmbaSpiNOR_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pFtlInfo)
{
    UINT32 RetVal = NVM_ERR_NONE;
    (void) UserPartID;
    (void) pFtlInfo;
    return RetVal;
}

UINT32 AmbaSpiNOR_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal = 1;
    (void) UserPartID;
    (void) pSecConfig;
    (void) TimeOut;

    return RetVal;
}

UINT32 AmbaSpiNOR_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    (void) UserPartID;
    (void) pSecConfig;
    (void) TimeOut;
    return 1;
}

/*
 *  @RoutineName:: AmbaSpiNOR_WritePartition
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_WritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut)
{
    (void) PartFlag;
    (void) PartID;
    (void) DataSize;
    (void) pDataBuf;
    (void) TimeOut;
    return 1;
}

/*
 *  @RoutineName:: AmbaNorSpi_InvalidatePartition
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_InvalidatePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
#if 0
    UINT32 RetVal = SPINOR_ERR_NONE;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);

    if (CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID) == 0U) {

        SetBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID); /* 1 - Invalid/Not-Use */
        if (PartFlag) {
            /* User Partition */
            RetVal = AmbaSpiNOR_WriteUserPartitionTable(NULL, 0, TimeOut);
        } else {
            /* System Partition */
            RetVal = AmbaSpiNOR_WriteSysPartitionTable(NULL, TimeOut);
        }
    }
    return RetVal;
#else
    AmbaMisra_TouchUnused(&PartFlag);
    AmbaMisra_TouchUnused(&PartID);
    AmbaMisra_TouchUnused(&TimeOut);

    return OK;
#endif
}

/*
 *  @RoutineName:: AmbaSpiNOR_ErasePartition
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_ErasePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
    UINT32 RetVal = 1;
    (void) PartFlag;
    (void) PartID;
    (void) TimeOut;
    return RetVal;
}

/*
 *  @RoutineName:: AmbaSpiNOR_WritePhyBlock
 *
 *  @Description::  To write the block of data identified by the block address parameter
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *      TimeOut:  Time out value
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_WritePhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut)
{
    (void) BlkAddr;
    (void) pDataBuf;
    (void) TimeOut;
    return 1;
}

/*
 *  @RoutineName:: AmbaSpiNOR_ReadPhyBlock
 *
 *  @Description::  To read the block of data identified by the block address parameter
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NAND flash partition ID
 *      TimeOut:  Time out value
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_ReadPhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = SPINOR_ERR_NONE;
    (void) BlkAddr;
    (void) pDataBuf;
    (void) TimeOut;
    return RetVal;
}

void AmbaSpiNOR_SetWritePtbFlag(UINT32 Flag)
{
    (void) Flag;
}

#if 0
/*
 *  @RoutineName:: AmbaNorSpi_IsBldMagicCodeSet
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
UINT32 AmbaSpiNOR_IsBldMagicCodeSet(void)
{
    return AmbaRTSL_NorSpiIsBldMagicCodeSet();
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_SetBldMagicCode(UINT32 TimeOut)
{
    UINT32 *pBldMagicCode = (UINT32 *) AMBA_ROM_MAGIC_CODE_VIRT_ADDR;
    INT32 RetVal = OK;

    if (pAmbaRTSL_NorSpiUserPartTable->BldMagicCode != *pBldMagicCode) {
        /* Write Bootloader Magic Code */
        pAmbaRTSL_NorSpiUserPartTable->BldMagicCode = *pBldMagicCode; /* Bootloader Magic Code */
        RetVal = AmbaSpiNOR_WriteUserPartitionTable(NULL, 0, TimeOut);
    }

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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_EraseBldMagicCode(UINT32 TimeOut)
{
    UINT32 *pBldMagicCode = (UINT32 *) AMBA_ROM_MAGIC_CODE_VIRT_ADDR;
    INT32 RetVal = OK;

    if (pAmbaRTSL_NorSpiUserPartTable->BldMagicCode == *pBldMagicCode) {
        /* Erase Bootloader Magic Code */
        pAmbaRTSL_NorSpiUserPartTable->BldMagicCode = 0xffffffffU; /* Bootloader Magic Code */
        RetVal = AmbaSpiNOR_WriteUserPartitionTable(NULL, 0, TimeOut);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaSpiNOR_ReadBldMagicCode
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
UINT32 AmbaSpiNOR_ReadBldMagicCode(void)
{
    return AmbaRTSL_NorSpiReadBldMagicCode();
}

/*
 *  @RoutineName:: AmbaSpiNOR_AllocateRomBuf
 *
 *  @Description:: Allocate the memory to load and unzip image by ROM and DSP
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_AllocateRomBuf(UINT32 RegionID, UINT8 *pRomLoadBuf, UINT8 *pRomUnzipBuf)
{
    AmbaMisra_TouchUnused(&RegionID);
    AmbaMisra_TouchUnused(&pRomLoadBuf);
    AmbaMisra_TouchUnused(&pRomUnzipBuf);
    return OK;
}

/*
 *  @RoutineName:: AmbaSpiNOR_FreeRomBuf
 *
 *  @Description:: Release the memory allocated by ROM and DSP.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_FreeRomBuf(UINT32 RegionID)
{
    (void)RegionID;
    return OK;
}

/*
 *  @RoutineName:: AmbaSpiNOR_GetRomImgSize
 *
 *  @Description:: Get the actual ImgSize and UnzipSize by ROM and DSP.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *         INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_GetRomImgSize(UINT32 RegionID,
                                UINT32 *ActualImgSize, UINT32 *UnzipImgSize)
{
    UINT32 Rval = 1;
    (void) RegionID;
    (void) ActualImgSize;
    (void) UnzipImgSize;
    return Rval;
}
#endif

