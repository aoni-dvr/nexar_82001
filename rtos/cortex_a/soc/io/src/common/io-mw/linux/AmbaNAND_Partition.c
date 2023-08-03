/**
 *  @file AmbaNAND_Partition.c
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
#include "AmbaKAL.h"

#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"
#include "AmbaUtility.h"
#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNVM_Ctrl.h"
#include "AmbaNAND_BBM_Def.h"
#include "AmbaNAND_OP.h"
#include "AmbaCache.h"

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"
#include "AmbaRTSL_FIO.h"

#include <mtd/mtd-user.h>
//static UINT32 UserPtbNextBlock = 0U;

static char DevPathUcode[40], DevPathSysData[40];
static char BlockPathUcode[40], BlockPathSysData[40];

/* Work Buffer for 1 Block. Due to the FDMA 8-Byte Boundary issue */
static UINT8 WorkBufMain[64 * 4 * 1024]
GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static AMBA_NVM_ROM_DSP_uCODE_CTRL_s _AmbaNAND_RomDspUcodeCtrl GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static AMBA_NVM_ROM_SYS_DATA_CTRL_s  _AmbaNAND_RomSysDataCtrl  GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT8 _AmbaNAND_PageBuf[64 * 4 * 1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static AMBA_KAL_MUTEX_t  PageBufMutex;

static UINT32 WritePtbFlag = 0U;

void AmbaNAND_SetWritePtbFlag(UINT32 Flag)
{
    WritePtbFlag = Flag;
}

void GNU_WEAK_SYMBOL AmbaFlashFwProg_SetPloadInfo(void);

void GNU_WEAK_SYMBOL AmbaFlashFwProg_SetPloadInfo(void)
{
    AmbaMisra_TouchUnused(&WritePtbFlag);
    return;
}

/**
 *  AmbaNAND_GetActivePtbNo - Get the No. of the current active partiton table
 *  @return The No. of the current active parttiton table
 */
UINT32 AmbaNAND_GetActivePtbNo(UINT32 * pPTBActive)
{
    UINT32 i;
    UINT32 RetVal;

    for (i = 0; i < 2U; i++) {
        RetVal = AmbaNAND_ReadUserPTB(NULL, i, 1000);

        if (pAmbaNandTblUserPart->PTBActive != 0U) {
            *pPTBActive = i;
            break;
        }
    }

    return RetVal;
}

/**
 *  AmbaNAND_SetActivePtbNo - Set specific parttiton table to active or nonactive
 *  @param[in] PtbNo The No. of the  partition table (start form 0)
 *  @param[in] Active 0 -Nonactive 1 - Active
 *  @return error code
 */
UINT32 AmbaNAND_SetActivePtbNo(UINT32 PtbNo, UINT32 Active)
{
    UINT32 RetVal;

    RetVal = AmbaNAND_ReadUserPTB(NULL, PtbNo, 1000);

    if (RetVal == NAND_ERR_NONE) {
        pAmbaNandTblUserPart->PTBActive = Active;
        RetVal = AmbaNAND_WriteUserPTB(NULL, PtbNo, 1000);
    }
    return RetVal;
}

/**
 *  AmbaNAND_InitPtbBbt - Init BBT and System/User Partition Tables
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_InitPtbBbt(UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
    UINT32 Crc32 = 0U;
    UINT32 ActiveNo = 0U;
    extern UINT32 *pAmbaNandTblBadBlk;
    char DevPath[15] = "/dev/mtd", BlockPath[15] = "/dev/mtdblock";
    UINT32 MtdNo = AMBA_USER_PARTITION_DSP_uCODE;
    UINT32 Rval = 0;

    pAmbaNandTblSysPart = &AmbaRTSL_SysPartTable;
    pAmbaNandTblUserPart = &AmbaRTSL_UserPartTable;
    pAmbaNandTblBadBlk = AmbaRTSL_NandBadBlkTable;

    /* Read NAND System Partition Table (Block 0, Page 2) to DRAM */
    RetVal = AmbaNAND_ReadSysPTB(NULL, TimeOut);
    if (RetVal == NAND_ERR_NONE) {
        /* Check System Partition Table */
        const UINT8 *pBuf = NULL;
        AmbaMisra_TypeCast32(&pBuf, &pAmbaNandTblSysPart);
        Crc32 = AmbaUtility_Crc32(pBuf, sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);
    }

    if ((RetVal == NAND_ERR_NONE) &&
        (pAmbaNandTblSysPart->CRC32 == Crc32) &&
        (pAmbaNandTblSysPart->BlkByteSize != 0U) &&
        (pAmbaNandTblSysPart->Version == USERPTB_VERSION)) {
        /* if System Partion is OK */
        RetVal = AmbaNAND_GetActivePtbNo(&ActiveNo);

        if (RetVal == NAND_ERR_NONE) {
            RetVal = AmbaNAND_ReadUserPTB(NULL, ActiveNo, TimeOut);
        }
#if 0
        if (RetVal != NAND_ERR_NONE) {
            /* Something wrong with USER PTB, re-create. */
            RetVal = AmbaNAND_CreatePtbBbt(TimeOut);
        }

        if (RetVal == NAND_ERR_NONE) {
            RetVal = AmbaNAND_LoadBBT(AmbaRTSL_NandBlkAddrPrimaryBBT, TimeOut);
        }

        if (RetVal != NAND_ERR_NONE) {
            RetVal = AmbaNAND_LoadBBT(AmbaRTSL_NandBlkAddrMirrorBBT, TimeOut);
        }

        if (RetVal != NAND_ERR_NONE) {
            RetVal = AmbaNAND_CreateBBT(TimeOut);
            if ((WritePtbFlag == 1U) && (RetVal == NAND_ERR_NONE)) {
                RetVal = AmbaNAND_WriteUserPTB(NULL, ActiveNo, TimeOut);
            }
        }
#endif
    } else {
        //RetVal = AmbaNAND_CreatePtbBbt(TimeOut);
    }
    /* Find DSP_uCODE and SYS_DATA  partition numbers */
    for (UINT32 Cnt = 0; Cnt < AMBA_USER_PARTITION_DSP_uCODE; Cnt ++) {
        AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(1U, Cnt);
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
        sprintf(BlockPathUcode, "%s%d", BlockPath, MtdNo);
    }
    MtdNo = AMBA_USER_PARTITION_SYS_DATA;
    for (UINT32 Cnt = 0; Cnt < AMBA_USER_PARTITION_SYS_DATA; Cnt ++) {
        AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(1U, Cnt);
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
        sprintf(BlockPathSysData, "%s%d", BlockPath, MtdNo);
    }

    return RetVal;
}

#if 0
/**
 *  AmbaNAND_CreatePTB - Create NAND System and User Partition Tables
 *  @return error code
 */
UINT32 AmbaNAND_CreatePTB(void)
{
    UINT32 RetVal = NAND_ERR_NONE;

    return RetVal;
}

/**
 *  AmbaNAND_CreateExtPTB - Create NAND System and User Partition Tables
 *  @return error code
 */
UINT32 AmbaNAND_CreateExtPTB(UINT32 StartBlk)
{
    UINT32 RetVal = NAND_ERR_NONE;
    (void) StartBlk;
    return RetVal;
}
#endif

/**
 *  AmbaNAND_LoadNvmRomFileTable - Load all NAND ROM File Tables
 *  @return error code
 */
UINT32 AmbaNAND_LoadNvmRomFileTable(void)
{
    int fd;
    UINT32 TableSize;

    (void)AmbaKAL_MutexCreate(&PageBufMutex, NULL);
    /* Load DSP uCODE ROM table */
    TableSize = sizeof(_AmbaNAND_RomDspUcodeCtrl);

    fd = open(BlockPathUcode, O_RDONLY);

    if (TableSize != read(fd, (void *)&_AmbaNAND_RomDspUcodeCtrl, TableSize)) {
        perror("read");
    }
    close(fd);

    TableSize = sizeof(_AmbaNAND_RomSysDataCtrl);

    fd = open(BlockPathSysData, O_RDONLY);

    if (TableSize != read(fd, (void *)&_AmbaNAND_RomSysDataCtrl, TableSize)) {
        perror("read");
        return 1;
    }
    close(fd);

    return 0;
}

/**
 *  FileNameCmp -
 *  @return
 */
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

/**
 *  AmbaNAND_GetRomFileEntry -
 *  @param[in] RegionID
 *  @param[in] pFileName
 *  @return
 */
static AMBA_NVM_ROM_FILE_ENTRY_s* AmbaNAND_GetRomFileEntry(UINT32 RegionID, const char *pFileName)
{
    AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile = &(_AmbaNAND_RomDspUcodeCtrl.Header.FileEntry[0]);
    UINT32 FileCount = 0U, IndexMax = 0U, Index = 0U, Find = 0U;

    if ((RegionID < AMBA_NUM_NVM_ROM_REGION) && (pFileName != NULL)) {

        if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
            pRomFile  = &(_AmbaNAND_RomDspUcodeCtrl.Header.FileEntry[0]);
            FileCount = _AmbaNAND_RomDspUcodeCtrl.Header.FileCount;
            IndexMax  = AMBA_NVM_MAX_NUM_DSP_uCODE_ROM_FILE;
        } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
            pRomFile  = &(_AmbaNAND_RomSysDataCtrl.Header.FileEntry[0]);
            FileCount = _AmbaNAND_RomSysDataCtrl.Header.FileCount;
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

/**
 *  AmbaNAND_GetRomFileSize - Get the size of the specific file in ROM partition
 *  @param[in] RegionID ROM Region ID
 *  @param[in] pFileName pointer to the Filename
 *  @param[out] pFileSize pointer to the file size
 *  @return error code
 */
UINT32 AmbaNAND_GetRomFileSize(UINT32 RegionID, const char *pFileName, UINT32 *pFileSize)
{
    UINT32 RetVal = NVM_ERR_FILE_NOT_FOUND;
    const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile;

    (*pFileSize) = 0;

    pRomFile = AmbaNAND_GetRomFileEntry(RegionID, pFileName);
    if (pRomFile != NULL) {
        (*pFileSize) = pRomFile->ByteSize;
        RetVal = NVM_ERR_NONE;
    }

    return RetVal;
}

/**
 *  AmbaNAND_GetRomFileInfo - Get the file info of the specific file in ROM partition form index
 *  @param[in] RegionID ROM Region ID
 *  @param[in] Index Index to the ROM File
 *  @param[out] pRomFileInfo pointer of the file info
 *  @return error code
 */
UINT32 AmbaNAND_GetRomFileInfo(UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo)
{
    UINT32 RetVal = NVM_ERR_NONE;
    const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile = NULL;

    if ((RegionID >= AMBA_NUM_NVM_ROM_REGION) || (Index > AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameters */
    } else {

        if ((RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE)  && (Index < AMBA_NVM_MAX_NUM_DSP_uCODE_ROM_FILE)) {
            pRomFile = &(_AmbaNAND_RomDspUcodeCtrl.Header.FileEntry[Index]);
        } else if ((RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) && (Index < AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE)) {
            pRomFile = &(_AmbaNAND_RomSysDataCtrl.Header.FileEntry[Index]);
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
                    AmbaUtility_StringCopy(pRomFileInfo->FileName, sizeof(pRomFileInfo->FileName), (const char *)pRomFile->FileName);
                    pRomFileInfo->ByteSize  = pRomFile->ByteSize;
                    pRomFileInfo->FileCRC32 = pRomFile->FileCRC32;
                }
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaNAND_GetEntry -
 *  @param[in] RegionID ROM Region ID
 *  @return
 */
static const AMBA_PARTITION_ENTRY_s * AmbaNAND_GetEntry(UINT32 RegionID)
{
    const AMBA_PARTITION_ENTRY_s *pPartEntry;

    if ((RegionID >= AMBA_NUM_NVM_ROM_REGION)) {
        pPartEntry = NULL;  /* wrong parameters */
    } else if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
        pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_DSP_uCODE);
    } else if (RegionID == AMBA_NVM_ROM_REGION_SYS_DATA) {
        pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_DATA);
    } else {
        pPartEntry = NULL;
    }
    return pPartEntry;
}

/**
 *  AmbaNAND_CheckRomFileParam -
 *  @param[in] pRomFile
 *  @param[in] ReadPos
 *  @param[in] Size
 *  @return error code
 */
static UINT32 AmbaNAND_CheckRomFileParam(const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile, UINT32 ReadPos, UINT32 Size)
{
    UINT32 RetVal;

    if (pRomFile == NULL) {
        RetVal = NVM_ERR_FILE_NOT_FOUND; /* no such file or wrong file position */
    } else if ((ReadPos > pRomFile->ByteSize) || (Size > (pRomFile->ByteSize - ReadPos))) {
        RetVal = NAND_ERR_ARG;  /* wrong parameters */
    } else {
        RetVal = NAND_ERR_NONE;
    }
    return RetVal;
}

static UINT32 AmbaNAND_ReadRomFileLoop(const AMBA_PARTITION_ENTRY_s *pPartEntry, UINT32 StartPos, UINT32 FirstPageSize, UINT32 PageAddr, UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut, int fd)
{
    //const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 ReadPos = StartPos;
    UINT32 RetVal = NAND_ERR_NONE;
    UINT32 Index = 0U;
    UINT32 DataSize;
    UINT32 Pages;
    UINT32 SizeTmp = Size;
    UINT32 FirstPageSizeTmp = FirstPageSize;

    UINT32 BlkPageSize;
    UINT32 PageByteSize;

    UINT32 Offset;
    mtd_info_t mtd_info;
    struct mtd_ecc_stats mtd_stat;

    (void)TimeOut;

    ioctl(fd, MEMGETINFO, &mtd_info);   // get the device info
    ioctl(fd, ECCGETSTATS, &mtd_stat);

    BlkPageSize  = mtd_info.erasesize / mtd_info.writesize;
    PageByteSize = mtd_info.writesize;

    while (SizeTmp > 0U) {
        UINT32 BlkAddr = GetRoundDownValU32(PageAddr, BlkPageSize);

        if (BlkAddr >= pPartEntry->BlkCount) {
            RetVal = NAND_ERR_ARG;    /* out of partition range */
        } else {
            Offset = BlkAddr * mtd_info.erasesize;

            if (mtd_stat.bbtblocks != 0) {
                if (ioctl(fd, MEMGETBADBLOCK, &Offset) != 0) {
                    Pages = BlkPageSize;
                    PageAddr += BlkPageSize;
                    continue;
                }
            }

            if (FirstPageSizeTmp != 0U) {
                (void)AmbaKAL_MutexTake(&PageBufMutex, AMBA_KAL_WAIT_FOREVER);

                lseek(fd, PageAddr * PageByteSize, SEEK_SET);

                //RetVal = AmbaNandOp_Read(PageAddr, 1U, _AmbaNAND_PageBuf, NULL, TimeOut);
                if (read(fd, _AmbaNAND_PageBuf, 1 * mtd_info.writesize) >= 0) {
                    RetVal = OK;
                }

                if ((RetVal != OK) || (FirstPageSizeTmp >= SizeTmp)) {
                    /* Read done or read error. */
                    if (RetVal == OK) {
                        (void)AmbaWrap_memcpy(&pDataBuf[0], &_AmbaNAND_PageBuf[ReadPos], SizeTmp); /* read finish */
                    } else {
                        RetVal = NAND_ERR_IO_FAIL;
                    }
                    break;
                }
                (void)AmbaWrap_memcpy(&pDataBuf[0], &_AmbaNAND_PageBuf[ReadPos], FirstPageSizeTmp);

                (void)AmbaKAL_MutexGive(&PageBufMutex);

                PageAddr += 1U;
                SizeTmp  -= FirstPageSizeTmp;
                Index    += FirstPageSizeTmp;
                ReadPos  -= FirstPageSizeTmp;
                FirstPageSizeTmp = 0U;
            }

            if (SizeTmp >= PageByteSize) {
                if ((PageAddr % BlkPageSize) == 0U) {
                    Pages = BlkPageSize;
                } else {
                    /* Read to block boubdary */
                    Pages = BlkPageSize - (PageAddr % BlkPageSize);
                }
                if (Pages > GetRoundDownValU32(SizeTmp, PageByteSize)) {
                    Pages = GetRoundDownValU32(SizeTmp, PageByteSize);
                }

                DataSize = Pages * PageByteSize;
            } else {
                DataSize = SizeTmp;
                Pages = 1U;
            }

            /* Read one page size from file */
            if (DataSize >= PageByteSize) {

                lseek(fd, PageAddr * PageByteSize, SEEK_SET);

                if (read(fd, &pDataBuf[Index], Pages * mtd_info.writesize) >= 0) {
                    RetVal = OK;
                }

                //RetVal = AmbaNandOp_Read(PageAddr, Pages, &pDataBuf[Index], NULL, TimeOut);
                PageAddr += Pages;
            } else {
                (void)AmbaKAL_MutexTake(&PageBufMutex, AMBA_KAL_WAIT_FOREVER);

                lseek(fd, PageAddr * PageByteSize, SEEK_SET);

                if (read(fd, _AmbaNAND_PageBuf, 1 * mtd_info.writesize) >= 0) {
                    RetVal = OK;
                }

                //RetVal = AmbaNandOp_Read(PageAddr, 1U, _AmbaNAND_PageBuf, NULL, TimeOut);
                (void)AmbaWrap_memcpy(&pDataBuf[Index], _AmbaNAND_PageBuf, DataSize);

                (void)AmbaKAL_MutexGive(&PageBufMutex);

                PageAddr += 1U;
            }

            if (RetVal != OK) {
                RetVal = NAND_ERR_IO_FAIL; /* break */
            } else {

                SizeTmp -= DataSize;
                Index   += DataSize;

                ReadPos = 0U;
            }
        }
    }
    return RetVal;
}


/**
 *  AmbaNAND_ReadRomFile - Read a specific file from ROM partiton
 *  @param[in] RegionID ROM Region ID
 *  @param[in] pFileName Filename
 *  @param[in] StartPos Byte position of the file
 *  @param[in] Size Byte count to be read
 *  @param[out] pDataBuf Data buffer to store read data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_ReadRomFile(UINT32 RegionID, const char *pFileName, UINT32 StartPos,
                            UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut)
{
    const AMBA_NVM_ROM_FILE_ENTRY_s *pRomFile = NULL;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;
    UINT32 i, PageAddr, BlkPageSize = 0U, PageByteSize = 0U;
    UINT32 FirstPageSize, ReadPos = StartPos;
    UINT32 RetVal = NAND_ERR_NONE;

    int fd;
    mtd_info_t mtd_info;
    struct mtd_ecc_stats mtd_stat;

    pPartEntry = AmbaNAND_GetEntry(RegionID);

    if ((pFileName != NULL) && (pDataBuf != NULL) && (pPartEntry != NULL)) {

        if (RegionID == AMBA_NVM_ROM_REGION_DSP_uCODE) {
            fd = open(DevPathUcode, O_RDONLY);
        } else {
            fd = open(DevPathSysData, O_RDONLY);
        }

        ioctl(fd, MEMGETINFO, &mtd_info);   // get the device info
        ioctl(fd, ECCGETSTATS, &mtd_stat);

        BlkPageSize  = mtd_info.erasesize / mtd_info.writesize;
        PageByteSize = mtd_info.writesize;

        pRomFile = AmbaNAND_GetRomFileEntry(RegionID, pFileName);

        RetVal = AmbaNAND_CheckRomFileParam(pRomFile, ReadPos, Size);
        if (RetVal == NAND_ERR_NONE) {
            /* Convert StartPos from file position to ROM partition position */
            PageAddr = GetRoundDownValU32((ReadPos + pRomFile->FileOffset), PageByteSize);
            if (PageByteSize != 0U) {
                ReadPos %= PageByteSize;
            }
            if (ReadPos == 0U) {
                FirstPageSize = 0U;
            } else {
                FirstPageSize = PageByteSize - ReadPos;
            }

            for (i = 0U; i < GetRoundDownValU32(PageAddr, BlkPageSize); i += 1U) {
                UINT32 Offset = PageAddr * PageByteSize;
                if (mtd_stat.badblocks == 0) {
                    break;
                }

                if (ioctl(fd, MEMGETBADBLOCK, &Offset) != 0) {
                    PageAddr += BlkPageSize;
                }
            }
            RetVal = AmbaNAND_ReadRomFileLoop(pPartEntry, ReadPos, FirstPageSize, PageAddr, Size, pDataBuf, TimeOut, fd);
        }
        close(fd);
    }
    return RetVal;
}

/**
 *  AmbaNAND_ReadSysPTB - Read NAND System Partition Table (Block 0, Page 2)
 *  @param[out] pSysPartTable pointer to the data of System partition table
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_ReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    UINT8 *pWorkBufMain = WorkBufMain;
    UINT32 RetVal = NAND_ERR_IO_FAIL;

    /* System partition table is located at block0 page2 (following BST pages) */
    if (OK == AmbaNandOp_Read(AmbaRTSL_NandCtrl.BstPageCount, AmbaRTSL_NandCtrl.SysPtblPageCount, pWorkBufMain, NULL, TimeOut)) {
        RetVal = NAND_ERR_NONE;

        /* copy to the user area while needed */
        if (pSysPartTable != NULL) {
            (void)AmbaWrap_memcpy(pSysPartTable, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s));
        } else {
            /* copy to the DRAM memory */
            (void)AmbaWrap_memcpy(pAmbaNandTblSysPart, pWorkBufMain, sizeof(AMBA_SYS_PARTITION_TABLE_s));
        }
    }

    return RetVal;
}

/**
 *  AmbaNAND_WriteSysPTB - Write NAND System Partition Table (Block 0, Page 2)
 *  @param[in] pSysPartTable pointer to the data of System partition table
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_WriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    UINT32 BstPageCount     = AmbaRTSL_NandCtrl.BstPageCount;      /* Number of Pages for BST */
    UINT32 SysPtblPageCount = AmbaRTSL_NandCtrl.SysPtblPageCount;  /* Number of Pages for System Partition Table */
    UINT8  *pWorkBufMain    = WorkBufMain;
    AMBA_SYS_PARTITION_TABLE_s *pWorkSysPartTable;
    UINT32 RetVal;

    if (pSysPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkSysPartTable = pSysPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkSysPartTable = pAmbaNandTblSysPart;
    }

    /* 1. Read BST data (block 0, page 0) */
    RetVal = AmbaNandOp_Read(0, BstPageCount, pWorkBufMain, NULL, TimeOut);

    if (RetVal == NAND_ERR_NONE) {
        /* 2. Erase the block 0 */
        RetVal = AmbaNandOp_EraseBlock(0, TimeOut);
    }

    if (RetVal == NAND_ERR_NONE) {
        /* 3. Write back BST data (block 0, page 0) */
        RetVal = AmbaNandOp_Program(0, BstPageCount, pWorkBufMain, NULL, TimeOut);
    }

    if (RetVal == NAND_ERR_NONE) {
        /* 4. Calculate CRC32 */
        const UINT8 *pBuf = NULL;
        AmbaMisra_TypeCast32(&pBuf, &pWorkSysPartTable);
        pWorkSysPartTable->CRC32 = AmbaUtility_Crc32(pBuf, sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U);

        /* 5. Write new System Partition Table (block 0, page 2) */
        RetVal = AmbaNandOp_Program(BstPageCount, SysPtblPageCount, (UINT8 *) pWorkSysPartTable, NULL, TimeOut);
    }

    return RetVal;
}

/**
 *  AmbaNAND_ReadUserPTB - Read NAND User Partition Table (Block 1, Page 0)
 *  @param[out] pUserPartTable: pointer to the data of User partition table
 *  @param[in] UserPtbNo
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_ReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;     /* Pointer to external NAND device information */
    UINT8 *pWorkBufMain = WorkBufMain;
    UINT32 RetVal = NAND_ERR_NONE;
    UINT32 UserPtblBlkAddr, BlkAddr;
    INT32 fd = -1;
    char Mtd[16] = "/dev/mtd0";
    struct mtd_ecc_stats mtd_stat;

    UserPtblBlkAddr = pAmbaNandTblSysPart->BlkAddrUserPTB;  /* Block address of User Partition Table */
    if (UserPtbNo != 0U) {
        fd = open(Mtd, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = NAND_ERR_IO_FAIL;
        } else {
            if(ioctl(fd, ECCGETSTATS, &mtd_stat) != 0) {
               RetVal = NAND_ERR_IO_FAIL;
            }
            else {
                for (BlkAddr = UserPtblBlkAddr + 1U; BlkAddr < AmbaRTSL_NandCtrl.TotalNumBlk; BlkAddr += 1U) {
                    UINT32 Offset = BlkAddr * pNandDevInfo->BlockPageSize * (pNandDevInfo->MainByteSize);

                    if (mtd_stat.badblocks == 0) {
                        UserPtblBlkAddr = BlkAddr;
                        break;
                    }
                    if (ioctl(fd, MEMGETBADBLOCK, &Offset) == 0) {
                        UserPtblBlkAddr = BlkAddr;
                        break;
                    }
                }
                if (BlkAddr >= AmbaRTSL_NandCtrl.TotalNumBlk) {
                    RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
                }
            }
            close(fd);
        }
    }

    /* Read User partition table from NAND */
    if (RetVal == NAND_ERR_NONE) {
        RetVal = AmbaNandOp_Read(UserPtblBlkAddr * pNandDevInfo->BlockPageSize,
                                 AmbaRTSL_NandCtrl.UserPtblPageCount, pWorkBufMain, NULL, TimeOut);
    }
    if (RetVal == NAND_ERR_NONE) {
        /* copy to the user area while needed */
        if (pUserPartTable != NULL) {
            (void)AmbaWrap_memcpy(pUserPartTable, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s));
        } else {
            /* copy to the DRAM memory */
            (void)AmbaWrap_memcpy(pAmbaNandTblUserPart, pWorkBufMain, sizeof(AMBA_USER_PARTITION_TABLE_s));
        }
    }

    return RetVal;
}

/**
 *  AmbaNAND_WriteUserPTB - Write NAND User Partition Table (Block 1, Page 0)
 *  @param[in] pUserPartTable pointer to the data of User partition table
 *  @param[in] UserPtbNo
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_WriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;     /* Pointer to external NAND device information */
    AMBA_USER_PARTITION_TABLE_s *pWorkUserPartTable;
    UINT32 UserPtblBlkAddr, BlkAddr;
    UINT32 UserPtblPageAddr;
    UINT32 UserPtblPageCount;
    UINT32 RetVal = NAND_ERR_NONE;
    INT32 fd = -1;
    char Mtd[16] = "/dev/mtd0";
    struct mtd_ecc_stats mtd_stat;

    UserPtblBlkAddr = pAmbaNandTblSysPart->BlkAddrUserPTB;  /* Block address of User Partition Table */
    if (UserPtbNo != 0U) {
        fd = open(Mtd, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "open %s failed!\n", Mtd);
            RetVal = NAND_ERR_IO_FAIL;
        } else {
            if(ioctl(fd, ECCGETSTATS, &mtd_stat) != 0) {
               RetVal = NAND_ERR_IO_FAIL;
            } else {
                for (BlkAddr = UserPtblBlkAddr + 1U; BlkAddr < AmbaRTSL_NandCtrl.TotalNumBlk; BlkAddr += 1U) {
                    UINT32 Offset = BlkAddr * pNandDevInfo->BlockPageSize * pNandDevInfo->MainByteSize;
                    if (mtd_stat.badblocks == 0) {
                        UserPtblBlkAddr = BlkAddr;
                        break;
                    }
                    if (ioctl(fd, MEMGETBADBLOCK, &Offset) == 0) {
                        UserPtblBlkAddr = BlkAddr;
                        break;
                    }
                }
                if (BlkAddr >= AmbaRTSL_NandCtrl.TotalNumBlk) {
                    RetVal = NAND_ERR_BAD_BLOCK_OVER;    /* no more good block ? */
                }
            }
            close(fd);
        }
    }

    UserPtblPageAddr = UserPtblBlkAddr * pNandDevInfo->BlockPageSize;
    UserPtblPageCount = AmbaRTSL_NandCtrl.UserPtblPageCount;       /* Number of Pages for User Partition Table */

    if (pUserPartTable != NULL) {
        /* update NAND through User defined Partition Table */
        pWorkUserPartTable = pUserPartTable;
    } else {
        /* update NAND through DRAM Partition Table */
        pWorkUserPartTable = pAmbaNandTblUserPart;
    }

    /* 2. Erase the block-1 */
    if (RetVal == NAND_ERR_NONE) {
        RetVal = AmbaNandOp_EraseBlock(UserPtblBlkAddr, TimeOut);
    }

    if (RetVal == NAND_ERR_NONE) {
        /* 4. Calculate CRC32 */
        const UINT8 *pTmp = NULL;
        AmbaMisra_TypeCast32(&pTmp, &pWorkUserPartTable);
        pWorkUserPartTable->CRC32 = AmbaUtility_Crc32(pTmp, AMBA_USER_PTB_CRC32_SIZE);

        /* 5. Write new User partition table (block 1, page 0) */
        RetVal = AmbaNandOp_Program(UserPtblPageAddr, UserPtblPageCount, (UINT8 *) pWorkUserPartTable, NULL, TimeOut);
    }

    return RetVal;
}

/**
 *  AmbaNAND_ReadPartitionInfo - Read partition information
 *  @param[in] PartFlag 0 - System Partition; 1 - User Partition
 *  @param[in] PartID NAND flash partition ID
 *  @param[out] pPartInfo pointer to partition information
 *  @return error code
 */
UINT32 AmbaNAND_ReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    UINT32 Rval = NAND_ERR_NONE;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);

    if ((pPartInfo == NULL) || (pPartEntry == NULL)) {
        Rval = NAND_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pPartInfo, pPartEntry, sizeof(AMBA_PARTITION_ENTRY_s));
    }

    return Rval;
}

#if 0
static UINT32 AmbaNAND_ReadPartialLoop(const AMBA_PARTITION_ENTRY_s *pPartEntry, UINT32 StartPos, UINT32 FirstPageSize, UINT32 PageAddr, UINT32 Size, UINT8 *pDataBuf)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlkPageSize  = pNandDevInfo->BlockPageSize;
    UINT32 PageByteSize = pNandDevInfo->MainByteSize;

    UINT8  *pWorkBufMain = WorkBufMain;
    UINT32 StartPosTmp = StartPos;
    UINT32 DataBufIdx = 0;
    UINT32 Pages = 1U;
    UINT32 DataSize;
    UINT32 RetVal = OK;
    UINT32 FirstPageSizeTmp = FirstPageSize;

    for (PageAddr += (pPartEntry->StartBlkAddr * BlkPageSize); (RetVal == OK) && (Size > 0U);) {
        UINT32  BlkAddr = PageAddr / BlkPageSize;
        if (BlkAddr >= (pPartEntry->StartBlkAddr + pPartEntry->BlkCount)) {
            RetVal = NAND_ERR_ARG;    /* out of partition range */
        } else {
            Pages = 1U;

            if (AmbaNAND_GetBlkMark(BlkAddr) != AMBA_NAND_BLK_GOOD) {
                Pages = BlkPageSize;
                continue;
            }

            if (0x0U != FirstPageSizeTmp) {
                const UINT8 *pTmpPtr = &pDataBuf[DataBufIdx];
                UINT32 Addr = 0;
                AmbaMisra_TypeCast32(&Addr, &pTmpPtr);

                //AmbaKAL_MutexTake(&RomReadMutex, 0xFFFFFFFFUL);
                if (OK != AmbaNandOp_Read(PageAddr, 1U, pWorkBufMain, NULL, 5000U)) {
                    RetVal = NAND_ERR_IO_FAIL;
                }
                if (FirstPageSizeTmp >= Size) {
                    (void)AmbaWrap_memcpy(&pDataBuf[DataBufIdx], &pWorkBufMain[StartPosTmp], Size);
                    (void)AmbaCache_DataClean(Addr, Size);
                    break;
                }
                (void)AmbaWrap_memcpy(&pDataBuf[DataBufIdx], &pWorkBufMain[StartPosTmp], FirstPageSizeTmp);
                (void)AmbaCache_DataClean(Addr, FirstPageSizeTmp);

                //AmbaKAL_MutexGive(&RomReadMutex);

                PageAddr     += 1U;
                Size         -= FirstPageSizeTmp;
                DataBufIdx   += FirstPageSizeTmp;
                StartPosTmp  -= FirstPageSizeTmp;
                FirstPageSizeTmp = 0;
            }

            if (Size >= PageByteSize) {
                if ((PageAddr % BlkPageSize) == 0U) {
                    Pages = BlkPageSize;
                } else {
                    /* Read to block boubdary */
                    Pages = BlkPageSize - (PageAddr % BlkPageSize);
                }
                if (Pages > (Size / PageByteSize)) {
                    Pages = Size / PageByteSize;
                }

                DataSize = Pages * PageByteSize;
            } else {
                DataSize = Size;
            }

            /* Read one page size from file */
            if (DataSize >= PageByteSize) {
                if (OK != AmbaNandOp_Read(PageAddr, Pages, &pDataBuf[DataBufIdx], NULL, 5000U)) {
                    RetVal = NAND_ERR_IO_FAIL;
                }
            } else {
                const UINT8 *pTmpPtr = &pDataBuf[DataBufIdx];
                UINT32 Addr = 0;
                AmbaMisra_TypeCast32(&Addr, &pTmpPtr);

                //AmbaKAL_MutexTake(&RomReadMutex, 0xFFFFFFFFUL);

                if (OK != AmbaNandOp_Read(PageAddr, 1U, pWorkBufMain, NULL, 5000U)) {
                    RetVal = NAND_ERR_IO_FAIL;
                }
                (void)AmbaWrap_memcpy(&pDataBuf[DataBufIdx], pWorkBufMain, DataSize);
                (void)AmbaCache_DataClean(Addr, DataSize);

                //AmbaKAL_MutexGive(&RomReadMutex);
            }

            if (RetVal == OK) {
                Size       -= DataSize;
                DataBufIdx += DataSize;
                StartPosTmp = 0;
            }
        }
        PageAddr += Pages;
    }
    return RetVal;
}

/**
 *  AmbaNAND_ReadPartitionPartial - Read partition data from NAND flash
 *  @param[in] PartFlag 0 - System Partition; 1 - User Partition
 *  @param[in] PartID NAND flash partition ID
 *  @param[in] StartPos
 *  @param[in] Size
 *  @param[out] pDataBuf pointer to partition information
 *  @return error code
 */
UINT32 AmbaNAND_ReadPartitionPartial(UINT32 PartFlag, UINT32 PartID, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 PageAddr, BlkPageSize;
    UINT32 PageByteSize, FirstPageSize;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    UINT32 StartPosTmp = StartPos;
    UINT32 RetVal = OK;
    UINT32 BlkMark = AMBA_NAND_BLK_GOOD;

    if (Size == 0U) {
        RetVal = OK;
    } else if ((pDataBuf == NULL) || (pPartEntry == NULL)) {
        RetVal = NAND_ERR_ARG;  /* Wrong Parameter */
    } else {
        BlkPageSize  = pNandDevInfo->BlockPageSize;
        PageByteSize = pNandDevInfo->MainByteSize;

        /* Convert StartPos from file position to ROM partition position */
        PageAddr = StartPosTmp / PageByteSize;
        StartPosTmp %= PageByteSize;

        if (StartPosTmp == 0U) {
            FirstPageSize = 0;
        } else {
            StartPosTmp %= PageByteSize;
            FirstPageSize = PageByteSize - StartPosTmp;
        }

        for (UINT32 i = 0; i < (PageAddr / BlkPageSize); i++) {
            BlkMark = AmbaNAND_GetBlkMark(i + pPartEntry->StartBlkAddr);
            if (BlkMark != AMBA_NAND_BLK_GOOD) {
                PageAddr += BlkPageSize;
            }
        }

        RetVal = AmbaNAND_ReadPartialLoop(pPartEntry, StartPosTmp, FirstPageSize, PageAddr, Size, pDataBuf);
    }

    return RetVal;
}
#endif

/**
 *  AmbaNAND_ReadPartition - Read partition data from NAND flash
 *  @param[in] PartFlag 0 - System Partition; 1 - User Partition
 *  @param[in] PartID NAND flash partition ID
 *  @param[out] pDataBuf pointer to partition data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_ReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlkAddr, BlkCount, PageAddr, BlkPageSize, BlkByteSize, LoadSize = 0;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    UINT32 RetVal = NAND_ERR_NONE, Index = 0U;

    if ((pDataBuf == NULL) || (pPartEntry == NULL) ||
        (pPartEntry->ActualByteSize == 0U) || (pPartEntry->ImageCRC32 == 0xffffffffU)) {
        RetVal = NAND_ERR_ARG;  /* Wrong Parameters */
    } else {
        BlkPageSize = pNandDevInfo->BlockPageSize;
        BlkByteSize = pNandDevInfo->MainByteSize * BlkPageSize;

        BlkCount = (pPartEntry->ActualByteSize + BlkByteSize - 1U) / BlkByteSize;
        if (BlkCount > pPartEntry->BlkCount) {
            BlkCount = pPartEntry->BlkCount;  /* should never happen */
        }

        BlkAddr = pPartEntry->StartBlkAddr;
        while (BlkCount > 0U) {
            if (AmbaNAND_GetBlkMark(BlkAddr) == AMBA_NAND_BLK_GOOD) {
                PageAddr = BlkAddr * BlkPageSize;
                if (OK != AmbaNandOp_Read(PageAddr, BlkPageSize, &pDataBuf[Index], NULL, TimeOut)) {
                    RetVal = NAND_ERR_IO_FAIL;  /* Wrong Parameters */
                }
                LoadSize += BlkByteSize;
                Index    += BlkByteSize;
                BlkCount -= 1U;

                if ((LoadSize >= pPartEntry->ActualByteSize) || (RetVal == NAND_ERR_IO_FAIL)) {
                    break;
                }
            }
            BlkAddr += 1U;
        }
    }
    return RetVal;
}

static UINT32 AmbaNAND_SyncPTB(AMBA_PARTITION_ENTRY_s *pPartEntry, UINT32 DataSize, const UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;

    /* Sync partition info if the partition is nonbackup partition */
    if (CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_BACKUP) == 0x0U) {
        /* Switch to the other user PTB */
        if (pAmbaNandTblUserPart->PTBNumber == 0U) {
            RetVal = AmbaNAND_ReadUserPTB(pAmbaNandTblUserPart, 1U, TimeOut);
        } else {
            RetVal = AmbaNAND_ReadUserPTB(pAmbaNandTblUserPart, 0U, TimeOut);
        }

        pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
        pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
        pPartEntry->ActualByteSize = DataSize;
        if (DataSize != 0U) {
            pPartEntry->ProgramStatus  = 0U;
            pPartEntry->ImageCRC32     = AmbaUtility_Crc32(pDataBuf, DataSize);
        } else {
            pPartEntry->ProgramStatus  = 0xffffffffU;
            pPartEntry->ImageCRC32     = 0xffffffffU;
        }

        if (RetVal == OK) {
            /* Update the user PTB */
            RetVal = AmbaNAND_WriteUserPTB(NULL, pAmbaNandTblUserPart->PTBNumber, TimeOut);
        }

        if (RetVal == OK) {
            /* Switch to the original user PTB */
            if (pAmbaNandTblUserPart->PTBNumber == 0U) {
                RetVal = AmbaNAND_ReadUserPTB(pAmbaNandTblUserPart, 1U, TimeOut);
            } else {
                RetVal = AmbaNAND_ReadUserPTB(pAmbaNandTblUserPart, 0U, TimeOut);
            }
        }
    }
    return RetVal;
}

/**
 *  AmbaNAND_WritePartition - Write partition data from NAND flash
 *  @param[in] PartFlag 0 - System Partition; 1 - User Partition
 *  @param[in] PartID NAND flash partition ID
 *  @param[in] DataSize Data size in Byte to write to the partition
 *  @param[in] pDataBuf pointer to partition data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_WritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut)
{
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlkAddr, BlkCount, PageAddr, BlkPageSize, BlkByteSize, WriteSize = 0;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    UINT32 RetVal = NAND_ERR_NONE, BbtUpdateFlag = 0;
    UINT32 BufIdx = 0U;

    BlkPageSize = pNandDevInfo->BlockPageSize;
    BlkByteSize = pNandDevInfo->MainByteSize * BlkPageSize;

    if ((DataSize == 0U) || (pDataBuf == NULL) || (pPartEntry == NULL)) {
        RetVal = NAND_ERR_ARG;  /* Wrong Parameters */
    } else if (((DataSize + BlkByteSize - 1U) / BlkByteSize) > pPartEntry->BlkCount) {
        RetVal = NAND_ERR_ARG;  /* should never happen */
    } else {
        /* 1. Program blocks in target partition */
        BlkCount = pPartEntry->BlkCount;
        BlkAddr  = pPartEntry->StartBlkAddr;
        PageAddr = BlkAddr * BlkPageSize;
        for (; BlkCount > 0U; BlkCount -= 1U) {
            if (AmbaNAND_GetBlkMark(BlkAddr) == AMBA_NAND_BLK_GOOD) {
                /* it is a good block */
                RetVal = AmbaNandOp_EraseBlock(BlkAddr, 5000);
                if (RetVal == NAND_ERR_NONE) {
                    RetVal = AmbaNandOp_Program(PageAddr, BlkPageSize, &pDataBuf[BufIdx], NULL, TimeOut);
                }

                if (RetVal == NAND_ERR_IO_FAIL) {
                    /* program failed. mark as runtime bad block. */
                    (void) AmbaNAND_SetBlkMark(BlkAddr, AMBA_NAND_BLK_RUNTIME_BAD, TimeOut);
                    BbtUpdateFlag = 1U;
                } else {
                    if (RetVal == NAND_ERR_NONE) {
                        BufIdx  += BlkByteSize;
                        WriteSize += BlkByteSize;
                    }
                    if (WriteSize >= DataSize) {
                        break;
                    }
                }
            }
            BlkAddr += 1U;
            PageAddr += BlkPageSize;
        }
        /* 2. Update NAND BBTs if necessary */
        if ((BbtUpdateFlag == 1U)) {
            AmbaRTSL_NandCtrl.BbtVersion += 1U;
            RetVal = AmbaNAND_UpdateBBT(AmbaRTSL_NandCtrl.BbtVersion, AmbaRTSL_NandBlkAddrPrimaryBBT,
                                        AmbaRTSL_NandBlkAddrMirrorBBT, TimeOut);
        }

        if (WriteSize < DataSize) {
            RetVal = NAND_ERR_NOT_ENOUGH_SPACE;
        }

        /* 3. Update partition table */
        if (RetVal == NAND_ERR_NONE) {
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID);
            pPartEntry->Attribute = ClearBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);
            pPartEntry->ActualByteSize = DataSize;
            pPartEntry->ProgramStatus  = 0U;
            pPartEntry->ImageCRC32     = AmbaUtility_Crc32(pDataBuf, DataSize);

            if (0x0U != PartFlag) {
                /* User Partition */
                RetVal = AmbaNAND_WriteUserPTB(NULL, pAmbaNandTblUserPart->PTBNumber, TimeOut);
                if (RetVal == OK) {
                    /* Sync partition info if the partition is nonbackup partition */
                    RetVal = AmbaNAND_SyncPTB(pPartEntry, DataSize, pDataBuf, TimeOut);
                }
            } else {
                /* System Partition */
                RetVal = AmbaNAND_WriteSysPTB(NULL, TimeOut);
            }
        }
    }
    return RetVal;
}

/**
 *  AmbaNAND_InvalidatePartition - Invalidate the partition
 *  @param[in] PartFlag 0 - System Partition; 1 - User Partition
 *  @param[in] PartID NAND flash partition ID
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_InvalidatePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
#if 0
    INT32 RetVal = OK;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);

    if (pPartEntry == NULL) {
        RetVal = NG;  /* Wrong Parameter */
    } else if (0x0U != CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID)) {
        RetVal = OK;
    } else {
        pPartEntry->Attribute = SetBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_INVALID); /* 1 - Invalid/Not-Use */
        if (0x0 != PartFlag) {
            /* User Partition */
            RetVal = AmbaNAND_WriteUserPTB(NULL, TimeOut);
        } else {
            /* System Partition */
            RetVal = AmbaNAND_WriteSysPTB(NULL, TimeOut);
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

/**
 *  AmbaNAND_ErasePartition - Erase a partition
 *  @param[in] PartFlag 0 - System Partition; 1 - User Partition
 *  @param[in] PartID NAND flash partition ID
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_ErasePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE, BbtUpdateFlag = 0;
    AMBA_PARTITION_ENTRY_s *pPartEntry = AmbaRTSL_NandGetPartEntry(PartFlag, PartID);
    UINT32 BlkAddr, BlkCount, BlkMark;

    if (pPartEntry == NULL) {
        RetVal = NAND_ERR_ARG;  /* Wrong Parameter */
    } else {
        /* 1. Erase all blocks in target partition */
        BlkCount = pPartEntry->BlkCount;
        for (BlkAddr = pPartEntry->StartBlkAddr; BlkCount > 0U;) {
            BlkMark = AmbaNAND_GetBlkMark(BlkAddr);
            if (BlkMark != AMBA_NAND_BLK_FACTORY_BAD) {

                RetVal = AmbaNandOp_EraseBlock(BlkAddr, TimeOut);

                if ((RetVal == OK) && (BlkMark == AMBA_NAND_BLK_RUNTIME_BAD)) {
                    /* erase successfully. mark as good block. */
                    RetVal = AmbaNAND_SetBlkMark(BlkAddr, AMBA_NAND_BLK_GOOD, TimeOut);
                    BbtUpdateFlag = 1U;
                } else if ((RetVal != OK) && (BlkMark == AMBA_NAND_BLK_GOOD)) {
                    /* erase failed. mark as runtime bad block. */
                    RetVal = AmbaNAND_SetBlkMark(BlkAddr, AMBA_NAND_BLK_RUNTIME_BAD, TimeOut);
                    BbtUpdateFlag = 1U;
                } else {
                    /* for misraC checking, do nothing */
                }
            }
            BlkCount--;
            BlkAddr++;
        }

        /* 2. Update NAND BBTs if necessary */
        if (0x0U != BbtUpdateFlag) {
            AmbaRTSL_NandCtrl.BbtVersion++;
            RetVal = AmbaNAND_UpdateBBT(AmbaRTSL_NandCtrl.BbtVersion, AmbaRTSL_NandBlkAddrPrimaryBBT, AmbaRTSL_NandBlkAddrMirrorBBT, TimeOut);
        }

        /* 3. Update partition table */
        pPartEntry->Attribute = SetBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_ERASED);  /* 1 - Erased */
        pPartEntry->ActualByteSize = 0;
        pPartEntry->ProgramStatus  = 0xffffffffU;
        pPartEntry->ImageCRC32     = 0xffffffffU;
        if ((0x0U != PartFlag) && (RetVal == OK)) {
            /* User Partition */
            RetVal = AmbaNAND_WriteUserPTB(NULL, pAmbaNandTblUserPart->PTBNumber, TimeOut);
            if (RetVal == OK) {
                /* Sync partition info if the partition is nonbackup partition */
                RetVal = AmbaNAND_SyncPTB(pPartEntry, 0U, NULL, TimeOut);
            }
        } else {
            /* System Partition */
            RetVal = AmbaNAND_WriteSysPTB(NULL, TimeOut);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaNAND_WritePhyBlock
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
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaNAND_WritePhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut)
{
#if 0
    UINT32 RetVal = NAND_ERR_NONE;
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlkMark, BlkPageSize, PageAddr;

    if ((BlkAddr >= AmbaRTSL_NandCtrl.TotalNumBlk) || (pDataBuf == NULL)) {
        RetVal = NAND_ERR_ARG;
    } else {
        BlkMark = AmbaNAND_GetBlkMark(BlkAddr);

        if (BlkMark != AMBA_NAND_BLK_GOOD) {
            RetVal = NAND_ERR_BAD_BLOCK_OVER;
        } else {
            RetVal = AmbaNandOp_EraseBlock(BlkAddr, TimeOut);

            if(RetVal == NAND_ERR_NONE) {
                BlkPageSize = pNandDevInfo->BlockPageSize;
                PageAddr = BlkAddr * BlkPageSize;

                RetVal = AmbaNandOp_Program(PageAddr, BlkPageSize, pDataBuf, NULL, TimeOut);
            }
        }
    }

    return RetVal;
#else
    (void)BlkAddr;
    (void)pDataBuf;
    (void)TimeOut;

    return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaNAND_ReadPhyBlock
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
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaNAND_ReadPhyBlock(UINT32 BlkAddr, UINT8 * pDataBuf, UINT32 TimeOut)
{
#if 0
    UINT32 RetVal = NAND_ERR_NONE;
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;
    UINT32 BlkMark, BlkPageSize, PageAddr;

    if ((BlkAddr >= AmbaRTSL_NandCtrl.TotalNumBlk) || (pDataBuf == NULL)) {
        RetVal = NAND_ERR_ARG;
    } else {
        BlkMark = AmbaNAND_GetBlkMark(BlkAddr);

        if (BlkMark != AMBA_NAND_BLK_GOOD) {
            RetVal = NAND_ERR_BAD_BLOCK_OVER;
        }

        if(RetVal == NAND_ERR_NONE) {
            BlkPageSize = pNandDevInfo->BlockPageSize;
            PageAddr = BlkAddr * BlkPageSize;

            RetVal = AmbaNandOp_Read(PageAddr, BlkPageSize, pDataBuf, NULL, TimeOut);
        }
    }

    return RetVal;
#else
    (void)BlkAddr;
    (void)pDataBuf;
    (void)TimeOut;

    return OK;
#endif
}

/**
 *  AmbaNAND_IsBldMagicCodeSet - check if Bootloader Magic Code has been set for System Software load or not
 *  @return error code
 */
UINT32 AmbaNAND_IsBldMagicCodeSet(void)
{
#if 0
    return AmbaRTSL_NandIsBldMagicCodeSet();
#else
    return OK;
#endif
}

/**
 *  AmbaRTSL_NandSetBldMagicCode - Write Bootloader Magic Code onto NAND device
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_SetBldMagicCode(UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
#if 0
    const UINT32 *pBldMagicCode = (UINT32 *) AMBA_ROM_MAGIC_CODE_VIRT_ADDR;

    if (pAmbaNandTblUserPart->BldMagicCode != *pBldMagicCode) {
        /* Write Bootloader Magic Code */
        pAmbaNandTblUserPart->BldMagicCode = *pBldMagicCode; /* Bootloader Magic Code */
        RetVal = AmbaNAND_WriteUserPTB(NULL, TimeOut);
    }
#else
    (void)TimeOut;
#endif
    return RetVal;
}

/**
 *  AmbaRTSL_NandEraseBldMagicCode - Erase Bootloader Magic Code from NAND device
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_EraseBldMagicCode(UINT32 TimeOut)
{
#if 0
    const UINT32 *pBldMagicCode = (UINT32 *) AMBA_ROM_MAGIC_CODE_VIRT_ADDR;
    INT32 RetVal = OK;

    if (pAmbaNandTblUserPart->BldMagicCode == *pBldMagicCode) {
        /* Erase Bootloader Magic Code */
        pAmbaNandTblUserPart->BldMagicCode = 0xffffffffU; /* Bootloader Magic Code */
        RetVal = AmbaNAND_WriteUserPTB(NULL, TimeOut);
    }

    return RetVal;
#else
    AmbaMisra_TouchUnused(&TimeOut);

    return OK;
#endif
}

/**
 *  AmbaNAND_ReadBldMagicCode - Read Bootloader Magic Code
 *  @return Bootloader Magic Code
 */
UINT32 AmbaNAND_ReadBldMagicCode(void)
{
    return pAmbaNandTblUserPart->BldMagicCode;
}

