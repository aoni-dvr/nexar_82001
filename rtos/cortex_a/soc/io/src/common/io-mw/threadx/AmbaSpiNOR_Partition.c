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
#include "AmbaRTSL_SpiNOR.h"
#include "AmbaRTSL_SpiNOR_Ctrl.h"

#define SEC_SIZE 512U

static AMBA_NVM_ROM_DSP_uCODE_CTRL_s AmbaNorSpi_RomDspUcodeCtrl GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static AMBA_NVM_ROM_SYS_DATA_CTRL_s  AmbaNorSpi_RomSysDataCtrl  GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT32 WritePtbFlag = 0;
static AMBA_KAL_MUTEX_t PtbMutex = {0};
static AMBA_KAL_MUTEX_t UserPartationMutex = {0};

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
    UINT32 RetVal;
    UINT32 Crc32;
    const UINT8 *pBuffer = NULL;

    if (PtbMutex.tx_mutex_id == 0U) {
        (void)AmbaKAL_MutexCreate(&PtbMutex, NULL);
    }

    pAmbaRTSL_NorSpiSysPartTable = &AmbaRTSL_NorSysPartTable;
    pAmbaRTSL_NorSpiUserPartTable = &AmbaRTSL_NorUserPartTable;

    /* Read NAND System Partition Table (Block 0, Page 2) to DRAM */
    RetVal = AmbaSpiNOR_ReadSysPartitionTable(NULL, TimeOut);

    /* Check System Partition Table */
    AmbaMisra_TypeCast32(&pBuffer, &pAmbaRTSL_NorSpiSysPartTable);
    Crc32 = IO_UtilityCrc32(pBuffer, (UINT32) sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);
    if ((RetVal == OK) &&
        (pAmbaRTSL_NorSpiSysPartTable->CRC32 == Crc32) &&
        (pAmbaRTSL_NorSpiSysPartTable->BlkByteSize != 0U)) {
        /* if System Partion is OK */
        UINT32 ActiveNo = 0U;
        RetVal = AmbaSpiNOR_GetActivePtbNo(&ActiveNo);

        if (AmbaSpiNOR_ReadUserPartitionTable(NULL, ActiveNo, TimeOut) != OK) {
            RetVal = SPINOR_ERR_IO_FAIL;
        }
    } else {
        if (OK != AmbaWrap_memset(pAmbaRTSL_NorSpiSysPartTable,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };
        if (OK != AmbaWrap_memset(pAmbaRTSL_NorSpiUserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
        //RetVal = AmbaSpiNOR_CreatePTB();
        if ((WritePtbFlag != 0U) && (RetVal == OK)) {
            RetVal = AmbaSpiNOR_WriteSysPartitionTable(NULL, TimeOut);
            RetVal |= AmbaSpiNOR_WriteUserPartitionTable(NULL, 0, TimeOut);
        }
    }

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
            if (OK != AmbaWrap_memcpy(pPartEntry->PartitionName, pSysPartConfig->PartitionName, sizeof(pPartEntry->PartitionName))) { /* Misrac */ };    /* Partition Name */
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

            if (OK != AmbaWrap_memcpy(pPartEntry->PartitionName, pUserPartConfig->PartitionName, sizeof(pPartEntry->PartitionName))) { /* Misrac */ };
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

static UINT32 AmbaSpiNOR_ROMReadByDevice(UINT32 RegionID, UINT32 StartPos, UINT32 Size,
        UINT8 *pDataBuf, UINT32 Timeout)
{
    UINT32 RetVal;
    UINT32 BlkByteSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize, Offset;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;

    if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, AMBA_USER_PARTITION_DSP_uCODE);
    } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, AMBA_USER_PARTITION_SYS_DATA);
    } else {
        /* for misraC checking, do nothing */
    }

    if ((pDataBuf == NULL) || (pPartEntry == NULL) ||
        (pPartEntry->ActualByteSize == 0U) || (pPartEntry->ImageCRC32 == 0xffffffffU)) {
        RetVal = SPINOR_ERR_ARG;  /* Wrong Parameters */
    } else {
        Offset = (pPartEntry->StartBlkAddr * BlkByteSize) + StartPos;
        RetVal = AmbaSpiNOR_Readbyte(Offset, Size, pDataBuf, Timeout);
        if (RetVal != OK) {
            RetVal = 0;
        } else {
            RetVal = Size;
        }
    }
    return RetVal;
}

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
    if (OK != AmbaWrap_memcpy(pDataBuf, pUnzipBuf + StartPos, Size)) { /* Misrac */ };
    RetVal = Size;

Done:
    if (pTempBuf != NULL) {
        AmbaKAL_MemFree(&_AmbaNorSpi_RomMemCtrl[RegionID].RomLoadBuf);
    }

    return RetVal;
}
#endif

static UINT32 AmbaSpiNOR_ROMRead(UINT32 RegionID, UINT32 StartPos, UINT32 Size,
                                 UINT8 *pDataBuf, UINT32 Timeout)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;
    UINT32 RetVal = SPINOR_ERR_NONE;

    if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, AMBA_USER_PARTITION_DSP_uCODE);
    } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1, AMBA_USER_PARTITION_SYS_DATA);
    } else {
        RetVal = SPINOR_ERR_ARG;
    }

    if (RetVal == SPINOR_ERR_NONE) {
#if 0
        if (CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_COMPRESSED)) {
            //RetVal = AmbaSpiNOR_ROMReadByMemory(RegionID, StartPos, Size, pDataBuf, Timeout);
        } else {
            RetVal = AmbaSpiNOR_ROMReadByDevice(RegionID, StartPos, Size, pDataBuf, Timeout);
        }
#else
        (void) pPartEntry;
        RetVal = AmbaSpiNOR_ROMReadByDevice(RegionID, StartPos, Size, pDataBuf, Timeout);
#endif
    }

    return RetVal;
}

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
    UINT32 RetSize;
    UINT32 RetVal = SPINOR_ERR_NONE;
    UINT32 expSize = (UINT32) sizeof(AmbaNorSpi_RomDspUcodeCtrl);

    /* Load DSP uCODE ROM table */
    RetSize = AmbaSpiNOR_ROMRead(AMBA_NVM_ROM_REGION_DSP_uCODE, 0, expSize,
                                 (UINT8 *) &AmbaNorSpi_RomDspUcodeCtrl, 1000U);

    if (RetSize != expSize) {
        RetVal = SPINOR_ERR_IO_FAIL;
    } else {
        /* Load SYS DATA ROM table */
        RetSize = AmbaSpiNOR_ROMRead(AMBA_NVM_ROM_REGION_SYS_DATA, 0, (UINT32) sizeof(AmbaNorSpi_RomSysDataCtrl),
                                     (UINT8 *) &AmbaNorSpi_RomSysDataCtrl, 1000U);
        if (RetSize != sizeof(AmbaNorSpi_RomSysDataCtrl)) {
            RetVal = SPINOR_ERR_IO_FAIL;
        }
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

static AMBA_NVM_ROM_FILE_ENTRY_s* AmbaSpiNOR_GetRomFileEntry(UINT32 RegionID, const char *pFileName)
{
    AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile = &(AmbaNorSpi_RomDspUcodeCtrl.Header.FileEntry[0]);
    UINT32 FileCount = 0U, IndexMax = 0U, Index = 0U, Find = 0U;

    if ((RegionID < AMBA_NUM_NVM_ROM_REGION) && (pFileName != NULL)) {

        if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
            pRomFile  = &(AmbaNorSpi_RomDspUcodeCtrl.Header.FileEntry[0]);
            FileCount = AmbaNorSpi_RomDspUcodeCtrl.Header.FileCount;
            IndexMax  = AMBA_NVM_MAX_NUM_DSP_uCODE_ROM_FILE;
        } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
            pRomFile  = &(AmbaNorSpi_RomSysDataCtrl.Header.FileEntry[0]);
            FileCount = AmbaNorSpi_RomSysDataCtrl.Header.FileCount;
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
 *          INT32 : File size (>0)/NG(-1)
 */
UINT32 AmbaSpiNOR_GetRomFileSize(UINT32 RegionID, const char *pFileName, UINT32 *pFileSize)
{
    UINT32 RetVal = NVM_ERR_FILE_NOT_FOUND;
    const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile;

    (*pFileSize) = 0;

    pRomFile = AmbaSpiNOR_GetRomFileEntry(RegionID, pFileName);
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
 *          INT32 : (>0)/NG(-1)
 */
UINT32 AmbaSpiNOR_GetRomFileInfo(UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo)
{
    UINT32 RetVal = NVM_ERR_NONE;
    const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile = &(AmbaNorSpi_RomDspUcodeCtrl.Header.FileEntry[0]);

    if ((RegionID >= AMBA_NUM_NVM_ROM_REGION) || (Index > AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {

        if ((RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) && (Index < AMBA_NVM_MAX_NUM_DSP_uCODE_ROM_FILE)) {
            pRomFile = &(AmbaNorSpi_RomDspUcodeCtrl.Header.FileEntry[Index]);
        } else if ((RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) && (Index < AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE)) {
            pRomFile = &(AmbaNorSpi_RomSysDataCtrl.Header.FileEntry[Index]);
        } else {
            RetVal = NVM_ERR_ARG;  /* wrong parameters */
        }

        if (RetVal == SPINOR_ERR_NONE) {
            if (IO_UtilityStringLength((const char *)pRomFile->FileName) == 0U) {
                RetVal = NVM_ERR_FILE_NOT_FOUND;  /* no such file */
            } else {
                if ((pRomFile->ByteSize  == 0U) || (pRomFile->ByteSize  == 0xffffffffU) ||
                    (pRomFile->FileCRC32 == 0U) || (pRomFile->FileCRC32 == 0xffffffffU)) {
                    RetVal = NVM_ERR_FILE_NOT_FOUND;  /* no such file */
                } else {
                    IO_UtilityStringCopy((char *)pRomFileInfo->FileName, IO_UtilityStringLength((const char *)pRomFile->FileName), (const char *)pRomFile->FileName);
                    pRomFileInfo->ByteSize  = pRomFile->ByteSize;
                    pRomFileInfo->FileCRC32 = pRomFile->FileCRC32;
                }
            }
        }
    }
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
    const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile = &(AmbaNorSpi_RomDspUcodeCtrl.Header.FileEntry[0]);
    UINT32 FileCount = 0;
    UINT32 RetVal = SPINOR_ERR_NONE;
    UINT32 ReadSize;
    UINT32 i;

    if ((pFileName == NULL) || (pDataBuf == NULL)) {
        RetVal = SPINOR_ERR_ARG;  /* wrong parameters */
    } else if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
        pRomFile   = &(AmbaNorSpi_RomDspUcodeCtrl.Header.FileEntry[0]);
        FileCount  = AmbaNorSpi_RomDspUcodeCtrl.Header.FileCount;
    } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
        pRomFile   = &(AmbaNorSpi_RomSysDataCtrl.Header.FileEntry[0]);
        FileCount  = AmbaNorSpi_RomSysDataCtrl.Header.FileCount;
    } else {
        RetVal = SPINOR_ERR_ARG;
    }

    if (RetVal == SPINOR_ERR_NONE) {
        SIZE_t FileLen1, FileLen2;

        for (i = 0; i < FileCount; i++) {
            FileLen1 = IO_UtilityStringLength(pFileName);
            FileLen2 = IO_UtilityStringLength((const char *) pRomFile->FileName);

            if (0 == IO_UtilityStringCompare(pFileName, (const char *) pRomFile->FileName, IO_UtilityStringLength((const char *) pRomFile->FileName))) {
                if (FileLen1 == FileLen2) {
                    break;
                }
            }
            pRomFile++;
        }

        if ((i >= FileCount) || (StartPos > pRomFile->ByteSize)) {
            RetVal = NVM_ERR_FILE_NOT_FOUND;  /* no such file or wrong file position */
        } else {
            if (Size > (pRomFile->ByteSize - StartPos)) {
                ReadSize = pRomFile->ByteSize - StartPos;
            } else {
                ReadSize = Size;
            }

            if (ReadSize != AmbaSpiNOR_ROMRead(RegionID, StartPos + pRomFile->FileOffset, ReadSize, pDataBuf, TimeOut)) {
                RetVal = SPINOR_ERR_IO_FAIL;
            }
        }
    }
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
    UINT8 *pWorkBufMain = AmbaRTSL_NorSpiCtrl.pBuf;
    UINT32 RetVal;
    UINT32 PageSize = AmbaRTSL_NorSpiDevInfo->PageSize;

    (void)AmbaKAL_MutexTake(&PtbMutex, KAL_WAIT_FOREVER);

    /* System partition table is located at block0 page2 (following BST pages) */
    RetVal = AmbaSpiNOR_Readbyte(AmbaRTSL_NorSpiCtrl.BstPageCount * PageSize,
                                 AmbaRTSL_NorSpiCtrl.SysPtblPageCount * PageSize, pWorkBufMain, TimeOut);
    if (RetVal == OK) {
        /* copy to the user area while needed */
        if (pSysPartTable != NULL) {
            if (OK != AmbaWrap_memcpy(pSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };
        } else {
            /* copy to the DRAM memory */
            if (OK != AmbaWrap_memcpy(pAmbaRTSL_NorSpiSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s))) { /* Misrac */ };
        }
    }

    (void)AmbaKAL_MutexGive(&PtbMutex);
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
    UINT32 BstPageCount     = AmbaRTSL_NorSpiCtrl.BstPageCount;      /* Number of Pages for BST */
    UINT32 SysPtblPageCount = AmbaRTSL_NorSpiCtrl.SysPtblPageCount;  /* Number of Pages for System Partition Table */
    UINT8  *pWorkBufMain = AmbaRTSL_NorSpiCtrl.pBuf;
    AMBA_SYS_PARTITION_TABLE_s *pWorkSysPartTable;
    UINT32 PageSize = AmbaRTSL_NorSpiDevInfo->PageSize;
    UINT32 BlockSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 RetVal;

    (void)AmbaKAL_MutexTake(&PtbMutex, KAL_WAIT_FOREVER);

    if (pSysPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkSysPartTable = pSysPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkSysPartTable = pAmbaRTSL_NorSpiSysPartTable;
    }

    /* 1. Read BST data (block 0, page 0) */
    RetVal = AmbaSpiNOR_Readbyte(0, BstPageCount * PageSize, pWorkBufMain, TimeOut);

    if (RetVal == SPINOR_ERR_NONE) {
        /* 2. Erase the block 0 */
        RetVal = AmbaSpiNOR_Erase(0, BlockSize, TimeOut);
    }

    if (RetVal == SPINOR_ERR_NONE) {
        /* 3. Write back BST data (block 0, page 0) */
        RetVal = AmbaSpiNOR_Program(0, BstPageCount * PageSize, pWorkBufMain, TimeOut);
    }

    if (RetVal == SPINOR_ERR_NONE) {
        /* 4. Calculate CRC32 */
        const UINT8 *pBuf = NULL;
        AmbaMisra_TypeCast32(&pBuf, &pWorkSysPartTable);
        pWorkSysPartTable->CRC32 = IO_UtilityCrc32(pBuf, (UINT32) sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);

        /* 5. Write new System Partition Table (block 0, page 2) */
        RetVal = AmbaSpiNOR_Program(BstPageCount * PageSize, SysPtblPageCount * PageSize,
                                    (UINT8 *) pWorkSysPartTable, TimeOut);
    }

    (void)AmbaKAL_MutexGive(&PtbMutex);

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
    UINT8 *pWorkBufMain = AmbaRTSL_NorSpiCtrl.pBuf;
    UINT32 PageSize = AmbaRTSL_NorSpiDevInfo->PageSize;
    UINT32 RetVal;
    UINT32 UserPtblBlkAddr;
#if defined(CONFIG_MUTI_BOOT_DEVICE)
    extern AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_EmmcUserPartTable ;
#endif

    AmbaMisra_TouchUnused(&UserPtbNo);

    (void)AmbaKAL_MutexTake(&PtbMutex, KAL_WAIT_FOREVER);

    UserPtblBlkAddr = pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB + UserPtbNo;

    /* Read User partition table from NORSPI */
    RetVal = AmbaSpiNOR_Readbyte(UserPtblBlkAddr * AmbaRTSL_NorSpiDevInfo->EraseBlockSize,
                                 AmbaRTSL_NorSpiCtrl.UserPtblPageCount * PageSize, pWorkBufMain, TimeOut);

    if (RetVal == OK) {
        /* copy to the user area while needed */
        if (pUserPartTable != NULL) {
            if (OK != AmbaWrap_memcpy(pUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
        } else {
            /* copy to the DRAM memory */
            if (OK != AmbaWrap_memcpy(pAmbaRTSL_NorSpiUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
#if defined(CONFIG_MUTI_BOOT_DEVICE)
            if (OK != AmbaWrap_memcpy(pAmbaRTSL_EmmcUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s))) { /* Misrac */ };
#endif
        }
    }

    (void)AmbaKAL_MutexGive(&PtbMutex);
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
    AMBA_USER_PARTITION_TABLE_s *pWorkUserPartTable;
    UINT32 UserPtblBlkAddr;
    UINT32 UserPtblPageCount;
    UINT32 PageSize = AmbaRTSL_NorSpiDevInfo->PageSize;
    UINT32 BlkByteSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 RetVal;

    UserPtblBlkAddr   = pAmbaRTSL_NorSpiSysPartTable->BlkAddrUserPTB + UserPtbNo;  /* Block address of User Partition Table */
    UserPtblPageCount = AmbaRTSL_NorSpiCtrl.UserPtblPageCount;            /* Number of Pages for User Partition Table */

    if (pUserPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkUserPartTable = pUserPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkUserPartTable = pAmbaRTSL_NorSpiUserPartTable;
    }

    (void)AmbaKAL_MutexTake(&PtbMutex, KAL_WAIT_FOREVER);

    /* 2. Erase the block-1 */
    RetVal = AmbaSpiNOR_Erase(UserPtblBlkAddr * BlkByteSize, BlkByteSize, TimeOut);
    if (RetVal == OK) {
        /* 4. Calculate CRC32 */
        const UINT8 *pBuf = NULL;
        AmbaMisra_TypeCast32(&pBuf, &pWorkUserPartTable);
        pWorkUserPartTable->CRC32 = IO_UtilityCrc32(pBuf, (UINT32) AMBA_USER_PTB_CRC32_SIZE);

        /* 5. Write new User partition table (block 1, page 0) */
        RetVal = AmbaSpiNOR_Program(UserPtblBlkAddr * BlkByteSize, UserPtblPageCount * PageSize, (UINT8 *) pWorkUserPartTable, TimeOut);
    }

    (void)AmbaKAL_MutexGive(&PtbMutex);

    return RetVal;
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
    return AmbaRTSL_NorSpiReadPartitionInfo(PartFlag, PartID, pPartInfo);
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
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo;     /* Pointer to external NAND device information */
    UINT32 BlkByteSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);
    UINT32 RetVal;

    if ((pDataBuf == NULL) || (pPartEntry == NULL) || (pNorDevInfo == NULL) ||
        (pPartEntry->ActualByteSize == 0U) || (pPartEntry->ImageCRC32 == 0xffffffffU)) {
        RetVal = SPINOR_ERR_ARG;  /* Wrong Parameters */
    } else {
        RetVal = AmbaSpiNOR_Readbyte(pPartEntry->StartBlkAddr * BlkByteSize,
                                     pPartEntry->ActualByteSize, pDataBuf, TimeOut);
    }
    return RetVal;
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
 */
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

    if (UserPartationMutex.tx_mutex_id == 0U) {
        if (OK != AmbaKAL_MutexCreate(&UserPartationMutex, NULL)) {
            RetVal = SPINOR_ERR_OS_API_FAIL;
        }
    }
    return RetVal;
}

static UINT32 AmbaSpiNOR_UserPartEraseHandler(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig,
        UINT32 Sector, UINT32 Sectors, UINT32 TimeOut)
{
    static UINT8 gEraseBuf[64 * 1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[UserPartID]);
    UINT32 BlockSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 SecSize   = AmbaRTSL_NorSpiDevInfo->EraseSectorSize;
    UINT8 *pOffset;
    UINT32 RetVal;
    UINT32 OffsetBySec, ErasePerSec, Offset;

    ErasePerSec = SecSize / SEC_SIZE;
    OffsetBySec = Sector & (ErasePerSec - 1U);

    Offset = (pPartEntry->StartBlkAddr * BlockSize) + ((Sector - OffsetBySec) * SEC_SIZE);

    /* Read the Data on Erased Area */
    RetVal = AmbaSpiNOR_Readbyte(Offset, ErasePerSec * SEC_SIZE, gEraseBuf, TimeOut);
    if (RetVal == SPINOR_ERR_NONE) {
        if (Sector == pSecConfig->StartSector) {
            pOffset = (UINT8 *) &gEraseBuf[OffsetBySec * SEC_SIZE];
            if (OK != AmbaWrap_memcpy(pOffset, pSecConfig->pDataBuf, (SIZE_t) Sectors * SEC_SIZE)) { /* Misrac */ };
        } else {
            pOffset = &pSecConfig->pDataBuf[(Sector - pSecConfig->StartSector) * SEC_SIZE];
            if (OK != AmbaWrap_memcpy(&gEraseBuf[0], pOffset, (SIZE_t) Sectors * SEC_SIZE)) { /* Misrac */ };
        }

        /* Erase the target Sectors */
        RetVal = AmbaSpiNOR_SectorErase(Offset, Sectors * SEC_SIZE, TimeOut);
        if (RetVal == SPINOR_ERR_NONE) {
            /* Write Back the Data */
            RetVal = AmbaSpiNOR_Program(Offset, SecSize, gEraseBuf, TimeOut);
        }
    }
    return RetVal;
}

static UINT32 AmbaNOR_MediaPartWrite(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[UserPartID]);
    UINT32 OffsetBySec, ErasePerSec, FirstEraseCount = 0;
    UINT32 EraseCount = 0, LastEraseCount = 0, Sector, Sectors;
    UINT32 BlkSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 SecSize = AmbaRTSL_NorSpiDevInfo->EraseSectorSize;
    UINT32 Rval = SPINOR_ERR_NONE;

    ErasePerSec = SecSize / SEC_SIZE;
    Sector = pSecConfig->StartSector;
    Sectors = pSecConfig->NumSector;

    OffsetBySec = Sector & (ErasePerSec - 1U);
    if (OffsetBySec == 0U) {
        FirstEraseCount = 0;
    } else {
        FirstEraseCount = ErasePerSec - OffsetBySec;
    }

    if (Sectors >= FirstEraseCount) {
        EraseCount     = (Sectors - FirstEraseCount) / ErasePerSec;
        LastEraseCount = (Sectors - FirstEraseCount) & (ErasePerSec - 1U);
    } else {
        FirstEraseCount = Sectors;
        LastEraseCount  = 0;
        EraseCount      = 0;
    }
    if (FirstEraseCount != 0U) {
        Rval = AmbaSpiNOR_UserPartEraseHandler(UserPartID, pSecConfig, Sector, FirstEraseCount, TimeOut);
    }

    if ((Rval == OK) && (EraseCount != 0U)) {
        /* Erase one Sector */
        const UINT8 *pOffset;
        UINT32 Offset = (pPartEntry->StartBlkAddr * BlkSize) + ((Sector + FirstEraseCount) * SEC_SIZE);
        Rval = AmbaSpiNOR_SectorErase(Offset, SecSize * EraseCount, TimeOut);

        if (Rval == OK) {
            pOffset = &pSecConfig->pDataBuf[FirstEraseCount * SEC_SIZE];
            Rval = AmbaSpiNOR_Program(Offset, EraseCount * SecSize, pOffset, TimeOut);
        }
    }

    if ((Rval == OK) && (LastEraseCount != 0U)) {
        Rval = AmbaSpiNOR_UserPartEraseHandler(UserPartID, pSecConfig,
                                               (Sector + FirstEraseCount + (EraseCount * ErasePerSec)), LastEraseCount, TimeOut);
    }
    return Rval;
}

/*
 *  @RoutineName:: AmbaNOR_MediaPartRead
 *
 *  @Description:: Read sectors to the Media partition
 *
 *  @Input      ::
 *          INT32 ID      : Partition object ID
 *          UINT8 *pBuf : Pointer to the data to be written
 *          long Sec    : Write start from this sector
 *          INT32 Secs    : Sectors to Write
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
static UINT32 AmbaNOR_MediaPartRead(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[UserPartID]);
    UINT32 BlockSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 Offset;
    UINT32 RetVal;

    Offset = (pPartEntry->StartBlkAddr * BlockSize) + (pSecConfig->StartSector * SEC_SIZE);

    if (((pSecConfig->StartSector + pSecConfig->NumSector) * SEC_SIZE) >  (pPartEntry->BlkCount * BlockSize)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        RetVal = AmbaSpiNOR_Readbyte(Offset, pSecConfig->NumSector * SEC_SIZE, pSecConfig->pDataBuf, TimeOut);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNOR_MediaPartGetTotalSector
 *
 *  @Description:: Get total sectors to the Media partition
 *
 *  @Input      ::
 *          INT32 ID      : Partition object ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : The number of sectors
 */
/*UINT32 AmbaNOR_MediaPartGetTotalSector(INT32 ID)*/
/*{*/
/*    AMBA_NOR_DEV_s *pDev;*/
/*    UINT32 BlockSize = 0;*/

/*    NOR_CHECK_ID(ID);*/
/*    pDev = AmbaRTSL_NorGetDev();*/
/*    BlockSize = pDev->DevLogicInfo.EraseBlockSize;*/

/*    return (AmbaNOR_MediaPartInfo.NumBlks[ID] * BlockSize >> 9);*/
/*}*/

/*
 *  @RoutineName:: AmbaSpiNOR_GetFtlInfo
 *
 *  @Description:: Get SpiNOR FTL informatio of the partition
 *
 *  @Input      ::
 *      UserPartID: User Partition ID
 *      pFtlInfo:   pointer to the data buffer oof FTL information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaSpiNOR_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pFtlInfo)
{
    UINT32 RetVal = NVM_ERR_NONE;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[UserPartID]);

    if (UserPartID >= AMBA_NUM_USER_PARTITION) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        if (CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_FTL) == 0x0U) {
            RetVal = NVM_ERR_ARG;  /* wrong parameter: not a FTL partition */
        } else {
            pFtlInfo->Initialized  = AmbaSpiNOR_Isinit(); /* 1 - this partion has been initialized */

            if (CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_WRITE_PROTECT) != 0U) {
                pFtlInfo->WriteProtect = 1U; /* 1 - this partition is write protected */
            } else {
                pFtlInfo->WriteProtect = 0;
            }

            /* Total size of the FTL partition in Sectors */
            pFtlInfo->TotalSizeInBytes   = (UINT64) (AmbaRTSL_NorSpiDevInfo->EraseBlockSize) * pPartEntry->BlkCount;
            pFtlInfo->SectorSizeInBytes  = SEC_SIZE;         /* Sector size in Bytes */
            pFtlInfo->TotalSizeInSectors = (UINT32) (pFtlInfo->TotalSizeInBytes / SEC_SIZE);

            /* Total size of the FTL partition in Bytes */
            pFtlInfo->PageSizeInBytes   = AmbaRTSL_NorSpiDevInfo->EraseSectorSize;           /* Page size in Bytes */
            pFtlInfo->BlockSizeInPages  = 1U;           /* Block size in Pages */
            pFtlInfo->EraseSizeInBlocks = 1U;             /* Erase size in Blocks */
        }
    }
    return RetVal;
}

UINT32 AmbaSpiNOR_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal;
    UINT32 BlockSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[UserPartID]);

    if ((UserPartID >= AMBA_NUM_USER_PARTITION) || (pSecConfig->NumSector == 0U)) {
        RetVal = SPINOR_ERR_ARG;  /* wrong parameter */
    } else if ((pSecConfig->StartSector * SEC_SIZE) > (pPartEntry->BlkCount * BlockSize)) {
        RetVal = SPINOR_ERR_ARG;  /* wrong parameter */
    } else {
        if (AmbaKAL_MutexTake(&UserPartationMutex, KAL_WAIT_FOREVER) != OK) { /* Take the Mutex */
            RetVal = SPINOR_ERR_OS_API_FAIL;
        } else {
            RetVal = AmbaNOR_MediaPartRead(UserPartID, pSecConfig, TimeOut);
            (void)AmbaKAL_MutexGive(&UserPartationMutex);   /* Release the Mutex */
        }
    }

    return RetVal;
}

UINT32 AmbaSpiNOR_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_NorSpiUserPartTable->Entry[UserPartID]);
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo;     /* Pointer to external NAND device information */
    UINT32 RetVal;

    if ((UserPartID >= AMBA_NUM_USER_PARTITION) || (pSecConfig->NumSector == 0U)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else if ((pSecConfig->StartSector * SEC_SIZE) > (pPartEntry->BlkCount * pNorDevInfo->EraseBlockSize)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&UserPartationMutex, KAL_WAIT_FOREVER) != OK) {
            RetVal = SPINOR_ERR_OS_API_FAIL;
        } else {
            RetVal = AmbaNOR_MediaPartWrite(UserPartID, pSecConfig, TimeOut);

            /* Release the Mutex */
            (void)AmbaKAL_MutexGive(&UserPartationMutex);
        }
    }

    return RetVal;
}

static UINT32 AmbaSpiNOR_SyncPTB(AMBA_PARTITION_ENTRY_s *pPartEntry, UINT32 DataSize, const UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = SPINOR_ERR_NONE;

    /* Sync partition info if the partition is nonbackup partition */
    if (CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_BACKUP) == 0x0U) {
        /* Switch to the other user PTB */
        if (pAmbaRTSL_NorSpiUserPartTable->PTBNumber == 0U) {
            RetVal = AmbaSpiNOR_ReadUserPartitionTable(pAmbaRTSL_NorSpiUserPartTable, 1U, TimeOut);
        } else {
            RetVal = AmbaSpiNOR_ReadUserPartitionTable(pAmbaRTSL_NorSpiUserPartTable, 0U, TimeOut);
        }

        pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
        pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
        pPartEntry->ActualByteSize = DataSize;
        if (DataSize != 0U) {
            pPartEntry->ProgramStatus  = 0U;
            pPartEntry->ImageCRC32     = IO_UtilityCrc32(pDataBuf, DataSize);
        } else {
            pPartEntry->ProgramStatus  = 0xffffffffU;
            pPartEntry->ImageCRC32     = 0xffffffffU;
        }

        if (RetVal == OK) {
            /* Update the user PTB */
            RetVal = AmbaSpiNOR_WriteUserPartitionTable(NULL, pAmbaRTSL_NorSpiUserPartTable->PTBNumber, TimeOut);
        }

        if (RetVal == OK) {
            /* Switch to the original user PTB */
            if (pAmbaRTSL_NorSpiUserPartTable->PTBNumber == 0U) {
                RetVal = AmbaSpiNOR_ReadUserPartitionTable(pAmbaRTSL_NorSpiUserPartTable, 1U, TimeOut);
            } else {
                RetVal = AmbaSpiNOR_ReadUserPartitionTable(pAmbaRTSL_NorSpiUserPartTable, 0U, TimeOut);
            }
        }
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaSpiNOR_WritePartition
 *
 *  @Description:: Write partition data from NOR flash
 *
 *  @Input      ::
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   NOR flash partition ID
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
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo;     /* Pointer to external NAND device information */
    UINT32 BlkAddr, BlkCount, Offset, BlkByteSize, WriteSize = 0;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);
    UINT32 RetVal = SPINOR_ERR_NONE;
    INT32 CmpResult = 0;
    UINT8 *pWorkBufMain = AmbaRTSL_NorSpiCtrl.pBuf;
    UINT32 BufIdx = 0U;

    BlkByteSize = pNorDevInfo->EraseBlockSize;
    BlkCount = (DataSize + BlkByteSize - 1U) / BlkByteSize;

    if ((pPartEntry == NULL) || (DataSize == 0U) || (pDataBuf == NULL) || (BlkCount > pPartEntry->BlkCount)) {
        RetVal = SPINOR_ERR_ARG;  /* Wrong Parameter */
    } else {

        /* 1. Program blocks in target partition */
        BlkAddr = pPartEntry->StartBlkAddr;
        Offset = BlkAddr * BlkByteSize;
        for (; BlkCount > 0U; BlkCount -= 1U) {
            RetVal = AmbaSpiNOR_Erase(BlkAddr * BlkByteSize, BlkByteSize, TimeOut);

            if (RetVal == SPINOR_ERR_NONE) {
                RetVal = AmbaSpiNOR_Program(Offset, BlkByteSize, &pDataBuf[BufIdx], TimeOut);
            }

            if (RetVal == SPINOR_ERR_NONE) {
                /* Read it back for verification */
                RetVal = AmbaSpiNOR_Readbyte(Offset, BlkByteSize, pWorkBufMain, TimeOut);
            }

            if (RetVal == SPINOR_ERR_NONE) {
                /* Compare memory content after read back */
                if (BlkCount == 1U) {
                    if (OK != AmbaWrap_memcmp(&pDataBuf[BufIdx], pWorkBufMain, (SIZE_t) DataSize - WriteSize, &CmpResult)) { /* Misrac */ };
                } else {
                    if (OK != AmbaWrap_memcmp(&pDataBuf[BufIdx], pWorkBufMain, BlkByteSize, &CmpResult)) { /* Misrac */ };
                }
                if (CmpResult != 0) {
                    RetVal = SPINOR_ERR_IO_FAIL;
                }
            }

            if (RetVal == SPINOR_ERR_NONE) {
                BufIdx  += BlkByteSize;
                WriteSize += BlkByteSize;
            }
            if ((RetVal != SPINOR_ERR_NONE) || (WriteSize >= DataSize)) {
                break;
            }
            BlkAddr++;
            Offset += BlkByteSize;
        }
    }

    /* 3. Update partition table */
    if (RetVal == OK) {
        pPartEntry->Attribute      = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
        pPartEntry->Attribute      = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
        pPartEntry->ActualByteSize = DataSize;
        pPartEntry->ProgramStatus  = 0;
        pPartEntry->ImageCRC32     = IO_UtilityCrc32(pDataBuf, DataSize);

        if (PartFlag != 0U) {
            /* User Partition */
            RetVal = AmbaSpiNOR_WriteUserPartitionTable(NULL, pAmbaRTSL_NorSpiUserPartTable->PTBNumber, TimeOut);
            if (RetVal == OK) {
                /* Sync partition info if the partition is nonbackup partition */
                RetVal = AmbaSpiNOR_SyncPTB(pPartEntry, DataSize, pDataBuf, TimeOut);
            }
        } else {
            /* System Partition */
            RetVal = AmbaSpiNOR_WriteSysPartitionTable(NULL, TimeOut);
        }
    }

    if (RetVal == OK) {
        // For Misrac check
        AmbaMisra_TouchUnused(pDataBuf);
    }

    return RetVal;
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
            RetVal = AmbaSpiNOR_WriteUserPartitionTable(NULL, pAmbaRTSL_NorSpiUserPartTable->PTBNumber, TimeOut);
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
    UINT32 RetVal;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NorSpiGetPartEntry(PartFlag, PartID);
    UINT32 BlkByteSize;
    const AMBA_NORSPI_DEV_INFO_s *pNorDevInfo = AmbaRTSL_NorSpiDevInfo;     /* Pointer to external NAND device information */

    /* 1. Erase all blocks in target partition */
    BlkByteSize = pNorDevInfo->EraseBlockSize;

    if (pPartEntry != NULL) {
        RetVal = AmbaSpiNOR_Erase(pPartEntry->StartBlkAddr * BlkByteSize, BlkByteSize * pPartEntry->BlkCount, TimeOut);
        if (RetVal == OK) {
            /* 3. Update partition table */
            pPartEntry->Attribute      = SetBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);  /* 1 - Erased */
            pPartEntry->ActualByteSize = 0;
            pPartEntry->ImageCRC32     = 0xffffffffU;
            pPartEntry->ProgramStatus = 0xffffffffU;
            if (PartFlag != 0U) {
                /* User Partition */
                RetVal = AmbaSpiNOR_WriteUserPartitionTable(NULL, pAmbaRTSL_NorSpiUserPartTable->PTBNumber, TimeOut);
                if (RetVal == OK) {
                    /* Sync partition info if the partition is nonbackup partition */
                    RetVal = AmbaSpiNOR_SyncPTB(pPartEntry, 0U, NULL, TimeOut);
                }
            } else {
                /* System Partition */
                RetVal = AmbaSpiNOR_WriteSysPartitionTable(NULL, TimeOut);
            }
        }
    } else {
        RetVal = SPINOR_ERR_ARG;
    }
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
    UINT32 RetVal = SPINOR_ERR_NONE;
    UINT32 BlkByteSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 BlkByteAddr = BlkAddr * BlkByteSize;
    UINT32 TotalNumBlk = (AmbaRTSL_NorSpiCtrl.TotalByteSize / BlkByteSize);
    UINT8 *pWorkBufMain = AmbaRTSL_NorSpiCtrl.pBuf;
    INT32 CmpResult = 0;

    if ((BlkAddr >= TotalNumBlk) || (pDataBuf == NULL)) {
        RetVal = SPINOR_ERR_ARG;
    } else {
        RetVal = AmbaSpiNOR_Erase(BlkByteAddr, BlkByteSize, TimeOut);

        if (RetVal == SPINOR_ERR_NONE) {
            RetVal = AmbaSpiNOR_Program(BlkByteAddr, BlkByteSize, pDataBuf, TimeOut);
        }

        if (RetVal == SPINOR_ERR_NONE) {
            /* Read it back for verification */
            RetVal = AmbaSpiNOR_Readbyte(BlkByteAddr, BlkByteSize, pWorkBufMain, TimeOut);
        }

        if (RetVal == SPINOR_ERR_NONE) {
            /* Compare memory content after read back */
            if (OK != AmbaWrap_memcmp(pDataBuf, pWorkBufMain, BlkByteSize, &CmpResult)) { /* Misrac */ };
        }

        if(CmpResult == 0) {
            RetVal = SPINOR_ERR_NONE;
        } else {
            RetVal = SPINOR_ERR_IO_FAIL;
        }
    }

    AmbaMisra_TouchUnused(pDataBuf);

    return RetVal;
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
    UINT32 BlkByteSize = AmbaRTSL_NorSpiDevInfo->EraseBlockSize;
    UINT32 BlkByteAddr = BlkAddr * BlkByteSize;
    UINT32 TotalNumBlk = (AmbaRTSL_NorSpiCtrl.TotalByteSize / BlkByteSize);

    if ((BlkAddr >= TotalNumBlk) || (pDataBuf == NULL)) {
        RetVal = SPINOR_ERR_ARG;
    } else {
        RetVal = AmbaSpiNOR_Readbyte(BlkByteAddr, BlkByteSize, pDataBuf, TimeOut);
    }

    return RetVal;
}

void AmbaSpiNOR_SetWritePtbFlag(UINT32 Flag)
{
    WritePtbFlag = Flag;
}

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
UINT32 AmbaSpiNOR_GetActivePtbNo(UINT32 * pPTBActive)
{
    UINT32 i;
    UINT32 RetVal;

    for(i = 0; i < 2U; i++) {
        RetVal = AmbaSpiNOR_ReadUserPartitionTable(NULL, i, 5000U);

        if(pAmbaRTSL_NorSpiUserPartTable->PTBActive != 0U) {
            *pPTBActive = i;
            break;
        }
    }

    return RetVal;
}

/**
 *  AmbaSPINOR_SetActivePtbNo - Set specific parttiton table to active or nonactive
 *  @param[in] PtbNo The No. of the  partition table (start form 0)
 *  @param[in] Active 0 -Nonactive 1 - Active
 *  @return error code
 */
UINT32 AmbaSpiNOR_SetActivePtbNo(UINT32 PtbNo, UINT32 Active)
{
    UINT32 RetVal;

    RetVal = AmbaSpiNOR_ReadUserPartitionTable(NULL, PtbNo, 1000);

    if (RetVal == SPINOR_ERR_NONE) {
        pAmbaRTSL_NorSpiUserPartTable->PTBActive = Active;
        RetVal = AmbaSpiNOR_WriteUserPartitionTable(NULL, PtbNo, 1000);
    }
    return RetVal;
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
    UINT32 Rval;
    AMBA_PARTITION_ENTRY_s *pPartEntry;

    if (RegionID >= AMBA_NUM_NVM_ROM_REGION) {
        Rval = SPINOR_ERR_ARG;
    } else if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1U, AMBA_USER_PARTITION_DSP_uCODE);
        //*UnzipImgSize = pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_DSP_uCODE].Attribute.Bits.Reserved;
    } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
        pPartEntry = AmbaRTSL_NorSpiGetPartEntry(1U, AMBA_USER_PARTITION_SYS_DATA);
        //*UnzipImgSize = pAmbaRTSL_NorSpiUserPartTable->Entry[AMBA_USER_PARTITION_SYS_DATA].Attribute.Bits.Reserved;
    } else {
        Rval = SPINOR_ERR_ARG;
    }

    if (Rval == OK) {
        if (0U == CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_COMPRESSED)) {
            *UnzipImgSize = 0;
        }

        *ActualImgSize = pPartEntry->ActualByteSize;
    }
    return Rval;
}
#endif

