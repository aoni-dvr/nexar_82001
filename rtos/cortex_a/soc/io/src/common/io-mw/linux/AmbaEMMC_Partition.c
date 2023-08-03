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
 *  @details NAND Partition Management APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"
#include "AmbaUtility.h"
#include "AmbaNAND.h"
#include "AmbaNVM_Ctrl.h"

#include "AmbaSD.h"
#include "AmbaRTSL_SD.h"
#include "AmbaSD_Ctrl.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <syslog.h>

#include <sys/types.h>
#include <unistd.h>

static char DevPathUcode[15], DevPathSysData[15];

static AMBA_NVM_ROM_DSP_uCODE_CTRL_s AmbaEMMC_RomDspUcodeCtrl GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static AMBA_NVM_ROM_SYS_DATA_CTRL_s  AmbaEMMC_RomSysDataCtrl GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT32 WritePtbFlag = 0;

UINT8 AmbaRTSL_SdWorkBuf_Lnx[AMBA_NUM_SD_CHANNEL * AMBA_SD_WORK_BUFFER_SIZE];

/* #define AMBA_ROM_CRC_CHECK */

/* Due to the FDMA 8-Byte Boundary issue */

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
    char DevPath[15] = "/dev/mmcblk0p";
    UINT32 MtdNo = AMBA_USER_PARTITION_DSP_uCODE;
    UINT32 Rval = 0;

    pAmbaRTSL_EmmcSysPartTable  = &AmbaRTSL_EmmcSysPartTable;
    pAmbaRTSL_EmmcUserPartTable  = &AmbaRTSL_EmmcUserPartTable;

    /* Read NAND System Partition Table (Block 0, Page 2) to DRAM */
    RetVal = AmbaEMMC_ReadSysPTB(NULL, TimeOut);
    /* Check System Partition Table */
    AmbaMisra_TypeCast(&pBuf, &pAmbaRTSL_EmmcSysPartTable);
    Crc32 = AmbaUtility_Crc32(pBuf, sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);

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
            AmbaMisra_TypeCast(&pBuf, &pAmbaRTSL_EmmcUserPartTable);
            Crc32 = AmbaUtility_Crc32(pBuf, AMBA_USER_PTB_CRC32_SIZE);

            if ((pAmbaRTSL_EmmcUserPartTable->CRC32 == Crc32) &&
                (pAmbaRTSL_EmmcUserPartTable->PtbMagic == AMBA_USER_PTB_MAGIC)) {
                RetVal = 0; /* USER PTB is OK */
            } else {
                CreatPTB = 1U; /* USER PTB is NG */
            }
        }
    } else {
        CreatPTB = 1U;
    }

    if ((RetVal != OK) || (CreatPTB != 0U)) {
        if (0U != AmbaWrap_memset(pAmbaRTSL_EmmcSysPartTable,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s))) {
            /* Misrac */
        };
        if (0U != AmbaWrap_memset(pAmbaRTSL_EmmcUserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s))) {
            /* Misrac */
        };
        RetVal = AmbaEMMC_CreatePTB();
#if 0
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
#endif
    }
    /* Find DSP_uCODE and SYS_DATA  partition numbers */
    for (UINT32 Cnt = 0; Cnt < AMBA_USER_PARTITION_DSP_uCODE; Cnt ++) {
        AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(1U, Cnt);
        if (pPartEntry == NULL) {
            Rval = 1;
            break;
        }
        /* If one of the partition size is 0, the DSP_uCODE partition number-1 */
        if (pPartEntry->BlkCount == 0) {
            MtdNo -= 1U;
        }
    }
    if (Rval == 0) {
        sprintf(DevPathUcode, "%s%d", DevPath, MtdNo);
    }
    MtdNo = AMBA_USER_PARTITION_SYS_DATA;
    for (UINT32 Cnt = 0; Cnt < AMBA_USER_PARTITION_SYS_DATA; Cnt ++) {
        AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(1U, Cnt);
        if (pPartEntry == NULL) {
            Rval = 1;
            break;
        }
        /* If one of the partition size is 0, SYS_DATA partition number-1 */
        if (pPartEntry->BlkCount == 0) {
            MtdNo -= 1U;
        }
    }
    if (Rval == 0) {
        sprintf(DevPathSysData, "%s%d", DevPath, MtdNo);
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
    UINT32 RetVal = NVM_ERR_NONE;
    return RetVal;
}

UINT32 AmbaEMMC_CreateExtUserPTB(const AMBA_PARTITION_CONFIG_s *pUserPartConfig, AMBA_USER_PARTITION_TABLE_s * pUserPartTable, UINT32 StartBlk)
{
    UINT32 RetVal = NVM_ERR_NONE;
    (void) pUserPartConfig;
    (void) pUserPartTable;
    (void) StartBlk;
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
    int fd;
    UINT32 TableSize;

    /* Load DSP uCODE ROM table */
    TableSize = sizeof(AmbaEMMC_RomDspUcodeCtrl);

    fd = open(DevPathUcode, O_RDONLY);

    if (TableSize != read(fd, (void *)&AmbaEMMC_RomDspUcodeCtrl, TableSize)) {
        perror("read");
    }
    close(fd);

    TableSize = sizeof(AmbaEMMC_RomSysDataCtrl);

    fd = open(DevPathSysData, O_RDONLY);

    if (TableSize != read(fd, (void *)&AmbaEMMC_RomSysDataCtrl, TableSize)) {
        perror("read");
        return 1;
    }
    close(fd);

    return 0;
}

static UINT32 FileNameCmp(const char* pFile1, const char *pFile2)
{
    UINT32 FileLen1, FileLen2;
    UINT32 Find = 0U;

    FileLen1 = AmbaUtility_StringLength(pFile1);
    FileLen2 = AmbaUtility_StringLength(pFile2);

    if (0 == AmbaUtility_StringCompare(pFile1, pFile2, AmbaUtility_StringLength(pFile2))) {
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
            if (AmbaUtility_StringLength((const char *)pRomFile->FileName) == 0U) {
                RetVal = NVM_ERR_FILE_NOT_FOUND;  /* no such file */
            } else {
                if ((pRomFile->ByteSize  == 0U) || (pRomFile->ByteSize  == 0xffffffffU) ||
                    (pRomFile->FileCRC32 == 0U) || (pRomFile->FileCRC32 == 0xffffffffU)) {
                    RetVal = NVM_ERR_FILE_NOT_FOUND;  /* no such file */
                } else {
                    AmbaUtility_StringCopy(pRomFileInfo->FileName, AmbaUtility_StringLength((const char *)pRomFile->FileName), (const char *)pRomFile->FileName);
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
    UINT32 RetVal = NVM_ERR_NONE;
    UINT32 ReadSize = Size;
    int fd;

    (void) TimeOut;

    if ((pFileName == NULL) || (pDataBuf == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {

        pRomFile = AmbaEMMC_GetRomFileEntry(RegionID, pFileName);

        RetVal = CheckReadPos(pRomFile, StartPos, ReadSize);

        if ((pRomFile != NULL) && (RetVal == NVM_ERR_NONE)) {

            if (ReadSize > (pRomFile->ByteSize - StartPos)) {
                ReadSize = pRomFile->ByteSize - StartPos;
            }

            if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
                fd = open(DevPathUcode, O_RDONLY);
            } else {
                fd = open(DevPathSysData, O_RDONLY);
            }

            lseek(fd, StartPos + pRomFile->FileOffset, 0);

            if (ReadSize != read(fd, (void *)pDataBuf, ReadSize)) {
                perror("read");
                RetVal = NVM_ERR_eMMC_READ;
            }
            close(fd);
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
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaRTSL_EmmcUserPartTable->Entry[UserPartID]);
    UINT32 RetVal = NVM_ERR_NONE;

    if ((UserPartID >= AMBA_NUM_USER_PARTITION) || ((pPartEntry->Attribute & AMBA_PARTITION_ATTR_FTL) == 0U)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        pFtlInfo->Initialized  = 1U; /* 1 - this partion has been initialized */
        pFtlInfo->WriteProtect = 0U;   /* 1 - this partition is write protected */

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
    UINT8 *pWorkBufMain = (UINT8 *)AmbaRTSL_SdWorkBuf_Lnx;
    UINT32 RetVal = OK;
    UINT32 TableSize = sizeof(AMBA_SYS_PARTITION_TABLE_s);
    INT32 Offset = AMBA_EMMC_BOOTSTRAP_CODE_SPACE_SIZE;

    INT32 fd;

    (void) TimeOut;

    fd = open("/dev/mmcblk0", O_RDWR);

    if (TableSize != pread(fd, (void *)pWorkBufMain, TableSize, Offset)) {
        perror("read");
        RetVal = NVM_ERR_eMMC_READ;
    }
    close(fd);

    if (RetVal == OK) {
        /* copy to the user area while needed */
        if (pSysPartTable != NULL) {
            if (0U != AmbaWrap_memcpy(pSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s))) {
                /* Misrac */
            }
        } else {
            /* copy to the DRAM memory */
            if (0U != AmbaWrap_memcpy(pAmbaRTSL_EmmcSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s))) {
                /* Misrac */
            }
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
    UINT32 BstPageCount     = AMBA_EMMC_BOOTSTRAP_CODE_SPACE_SIZE / 512U;      /* Number of Pages for BST */
    UINT32 SysPtblPageCount = GetRoundUpValU32(sizeof(AMBA_SYS_PARTITION_TABLE_s), 512U);  /* Number of Pages for System Partition Table */
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
    pWorkSysPartTable->CRC32 = AmbaUtility_Crc32(pBuf, (UINT32) sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);

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
    UINT8 *pWorkBufMain = (UINT8 *)AmbaRTSL_SdWorkBuf_Lnx;
    UINT32 RetVal = OK;

    UINT32 TableSize = sizeof(AMBA_USER_PARTITION_TABLE_s);
    UINT32 Offset = (pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB * UserPtbNo) * 512U;

    INT32  fd = open("/dev/mmcblk0", O_RDWR);

    if (UserPtbNo != 0U) {
        Offset = pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB * 512U + TableSize;
    } else {
        Offset = pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB * 512U;
    }

    (void)TimeOut;

    if (TableSize != pread(fd, (void *)pWorkBufMain, TableSize, Offset)) {
        perror("read");
        RetVal = NVM_ERR_eMMC_READ;
    }

    close(fd);

    if (RetVal == OK) {
        /* copy to the user area while needed */
        if (pUserPartTable != NULL) {
            if (0U != AmbaWrap_memcpy(pUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s))) {
                /* Misrac */
            }
        } else {
            /* copy to the DRAM memory */
            if (0U != AmbaWrap_memcpy(pAmbaRTSL_EmmcUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s))) {
                /* Misrac */
            }
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
    AMBA_USER_PARTITION_TABLE_s *pWorkUserPartTable;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;
    const UINT8 *pTmp = NULL;
    UINT32 RetVal;
    UINT32 UserPtblPageCount = GetRoundUpValU32(sizeof(AMBA_USER_PARTITION_TABLE_s), 512U);

    (void) TimeOut;

    if (pUserPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkUserPartTable = pUserPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkUserPartTable = pAmbaRTSL_EmmcUserPartTable ;
    }

    /* Calculate CRC32 */
    AmbaMisra_TypeCast(&pTmp, &pWorkUserPartTable);
    pWorkUserPartTable->CRC32 = AmbaUtility_Crc32(pTmp, AMBA_USER_PTB_CRC32_SIZE);

    /* Write new User partition table */
    SecConfig.NumSector   = UserPtblPageCount;
    SecConfig.pDataBuf    = (UINT8 *) pWorkUserPartTable;

#if defined(CONFIG_MUTI_BOOT_DEVICE)
    if(UserPtbNo == 0U) {
        SecConfig.StartSector = 0U;
    } else {
        SecConfig.StartSector = UserPtblPageCount;
    }
#else
    SecConfig.StartSector = pAmbaRTSL_EmmcSysPartTable ->BlkAddrUserPTB + (UserPtbNo * UserPtblPageCount);
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
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    INT32 RetVal = 0;

    if ((pPartInfo == NULL) || (pPartEntry == NULL)) {
        RetVal = -1;
    } else {
        if (AmbaWrap_memcpy(pPartInfo, pPartEntry, sizeof(AMBA_PARTITION_ENTRY_s)) != OK) {
            /* Do nothing */
        };
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
        BlkCount = (pPartEntry->ActualByteSize + BlkByteSize - 1U) / BlkByteSize;
        if (BlkCount > pPartEntry->BlkCount) {
            BlkCount = pPartEntry->BlkCount;  /* should never happen */
        }

        SecConfig.NumSector   = BlkCount;
        SecConfig.pDataBuf    = (UINT8 *) pDataBuf;
        SecConfig.StartSector = pPartEntry->StartBlkAddr;

        RetVal = AmbaSD_ReadSector(AMBA_SD_CHANNEL0, &SecConfig);
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
    UINT32 BlkCount, BlkByteSize;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_EmmcGetPartEntry(PartFlag, PartID);
    UINT32 RetVal = NVM_ERR_NONE;
    AMBA_NVM_SECTOR_CONFIG_s   SecConfig;

    if ((DataSize == 0U) || (pDataBuf == NULL) || (pPartEntry == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {

        BlkByteSize = 512U;

        BlkCount = (DataSize + BlkByteSize - 1U) / BlkByteSize;

        /* 1. Program blocks in target partition */
        SecConfig.NumSector   = BlkCount;
        SecConfig.pDataBuf    = (UINT8 *) pDataBuf;
        SecConfig.StartSector = pPartEntry->StartBlkAddr;

        RetVal = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);

        /* 2. Update partition table */
        if (RetVal == OK) {
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
            pPartEntry->ActualByteSize = DataSize;
            pPartEntry->ProgramStatus = 0;
            pPartEntry->ImageCRC32 = AmbaUtility_Crc32(pDataBuf, DataSize);

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
                    pPartEntry->ImageCRC32 = AmbaUtility_Crc32(pDataBuf, DataSize);

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
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0].CardInfo;
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
    const AMBA_SD_CARD_INFO_s *pCardInfo = &AmbaSD_Ctrl[AMBA_SD_CHANNEL0].CardInfo;
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
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    (void) TimeOut;

    if (UserPartID >= AMBA_NUM_USER_PARTITION) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        if (0U != AmbaWrap_memcpy(&SecConfig, pSecConfig, sizeof(AMBA_NVM_SECTOR_CONFIG_s))) {
            /* Misrac */
        };

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
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    (void) TimeOut;

    if (UserPartID >= AMBA_NUM_USER_PARTITION) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {
        if (0U != AmbaWrap_memcpy(&SecConfig, pSecConfig, sizeof(AMBA_NVM_SECTOR_CONFIG_s))) {
            /* Misrac */
        };

        SecConfig.StartSector += pPartEntry->StartBlkAddr;

        if (SecConfig.StartSector > (pPartEntry->StartBlkAddr + pPartEntry->BlkCount)) {
            RetVal = NVM_ERR_ARG;  /* wrong parameters */
        } else {
            RetVal = AmbaSD_WriteSector(AMBA_SD_CHANNEL0, &SecConfig);
        }
    }

    return RetVal;
}

