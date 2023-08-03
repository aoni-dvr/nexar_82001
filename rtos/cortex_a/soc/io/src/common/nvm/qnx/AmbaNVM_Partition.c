/**
 *  @file AmbaNVM_Partition.c
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
 *  @details NVM Partition control related APIs
 *
 */

#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaNVM_Ctrl.h"

static AMBA_NVM_CTRL_s _AmbaNVM_Ctrl[AMBA_NUM_NVM] = {
#if defined(CONFIG_ENABLE_NAND_BOOT)
    [AMBA_NVM_NAND] = {
        .GetRomFileInfo          = AmbaNAND_GetRomFileInfo,
        .GetRomFileSize          = AmbaNAND_GetRomFileSize,             /* pointer to get ROM file size function */
        .ReadRomFile             = AmbaNAND_ReadRomFile,                /* pointer to read ROM file function */

        .GetFtlInfo              = AmbaNAND_GetFtlInfo,                 /* pointer to get NVM FTL information function */
        .ReadSector              = AmbaNAND_ReadSector,
        .WriteSector             = AmbaNAND_WriteSector,

        .ReadSysPartitionTable   = AmbaNAND_ReadSysPTB,
        .WriteSysPartitionTabe   = AmbaNAND_WriteSysPTB,
        .ReadUserPartitionTable  = AmbaNAND_ReadUserPTB,
        .WriteUserPartitionTable = AmbaNAND_WriteUserPTB,

        .ReadPartitionInfo       = AmbaNAND_ReadPartitionInfo,
        .ReadPartition           = AmbaNAND_ReadPartition,
        .WritePartition          = AmbaNAND_WritePartition,
        .InvalidatePartition     = AmbaNAND_InvalidatePartition,
        .ErasePartition          = AmbaNAND_ErasePartition,
    },
#endif
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    [AMBA_NVM_eMMC] = {
        .GetRomFileInfo          = AmbaEMMC_GetRomFileInfo,
        .GetRomFileSize          = AmbaEMMC_GetRomFileSize,         /* pointer to get ROM file size function */
        .ReadRomFile             = AmbaEMMC_ReadRomFile,            /* pointer to read ROM file function */

        .GetFtlInfo              = AmbaEMMC_GetFtlInfo,             /* pointer to get NVM FTL information function */
        .ReadSector              = AmbaEMMC_ReadSector,
        .WriteSector             = AmbaEMMC_WriteSector,

        .ReadSysPartitionTable   = AmbaEMMC_ReadSysPTB,
        .WriteSysPartitionTabe   = AmbaEMMC_WriteSysPTB,
        .ReadUserPartitionTable  = AmbaEMMC_ReadUserPTB,
        .WriteUserPartitionTable = AmbaEMMC_WriteUserPTB,

        .ReadPartitionInfo       = AmbaEMMC_ReadPartitionInfo,
        .ReadPartition           = AmbaEMMC_ReadPartition,
        .WritePartition          = AmbaEMMC_WritePartition,
        .InvalidatePartition     = AmbaEMMC_InvalidatePartition,
        .ErasePartition          = AmbaEMMC_ErasePartition,
    },
#endif
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
    [AMBA_NVM_SPI_NOR] = {
        .GetRomFileInfo          = AmbaSpiNOR_GetRomFileInfo,
        .GetRomFileSize          = AmbaSpiNOR_GetRomFileSize,            /* pointer to get ROM file size function */
        .ReadRomFile             = AmbaSpiNOR_ReadRomFile,            /* pointer to read ROM file function */

        .GetFtlInfo              = AmbaSpiNOR_GetFtlInfo,            /* pointer to get NVM FTL information function */
        .ReadSector              = AmbaSpiNOR_ReadSector,
        .WriteSector             = AmbaSpiNOR_WriteSector,

        .ReadSysPartitionTable   = AmbaSpiNOR_ReadSysPartitionTable,
        .WriteSysPartitionTabe   = AmbaSpiNOR_WriteSysPartitionTable,
        .ReadUserPartitionTable  = AmbaSpiNOR_ReadUserPartitionTable,
        .WriteUserPartitionTable = AmbaSpiNOR_WriteUserPartitionTable,

        .ReadPartitionInfo       = AmbaSpiNOR_ReadPartitionInfo,
        .ReadPartition           = AmbaSpiNOR_ReadPartition,
        .WritePartition          = AmbaSpiNOR_WritePartition,
        .InvalidatePartition     = AmbaSpiNOR_InvalidatePartition,
        .ErasePartition          = AmbaSpiNOR_ErasePartition,
    },
#endif
#if defined(CONFIG_ENABLE_SPINAND_BOOT)
    [AMBA_NVM_SPI_NAND] = {
        .GetRomFileInfo          = AmbaNAND_GetRomFileInfo,
        .GetRomFileSize          = AmbaNAND_GetRomFileSize,             /* pointer to get ROM file size function */
        .ReadRomFile             = AmbaNAND_ReadRomFile,                /* pointer to read ROM file function */

        .GetFtlInfo              = AmbaNAND_GetFtlInfo,                 /* pointer to get NVM FTL information function */
        .ReadSector              = AmbaNAND_ReadSector,
        .WriteSector             = AmbaNAND_WriteSector,

        .ReadSysPartitionTable   = AmbaNAND_ReadSysPTB,
        .WriteSysPartitionTabe   = AmbaNAND_WriteSysPTB,
        .ReadUserPartitionTable  = AmbaNAND_ReadUserPTB,
        .WriteUserPartitionTable = AmbaNAND_WriteUserPTB,

        .ReadPartitionInfo       = AmbaNAND_ReadPartitionInfo,
        .ReadPartition           = AmbaNAND_ReadPartition,
        .WritePartition          = AmbaNAND_WritePartition,
        .InvalidatePartition     = AmbaNAND_InvalidatePartition,
        .ErasePartition          = AmbaNAND_ErasePartition,
    },
#endif
};

/*
 *  @RoutineName:: AmbaNVM_GetRomFileSize
 *
 *  @Description:: Get the size of the specific file in ROM partition
 *
 *  @Input      ::
 *      NvmID:     Memory type of the ROM partiton
 *      RegionID:  ROM Region ID
 *      pFileName: pointer to the Filename
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_GetRomFileSize(UINT32 NvmID, UINT32 RegionID, const char *pFileName, UINT32 *pFileSize)
{
    UINT32 RetVal;
    AMBA_NVM_GET_ROM_FILE_SIZE_f pGetRomFileSizeApi = _AmbaNVM_Ctrl[NvmID].GetRomFileSize;

    (*pFileSize) = 0;

    if ((NvmID >= AMBA_NUM_NVM) || (pGetRomFileSizeApi == NULL)) {
        RetVal = NVM_ERR_ARG;    /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pGetRomFileSizeApi(RegionID, pFileName, pFileSize);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_GetRomFileInfo
 *
 *  @Description:: Get the file info of the specific file in ROM partition
 *
 *  @Input      ::
 *      NvmID:     Memory type of the ROM partiton
 *      RegionID:  ROM Region ID
 *      Index: Index of the ROM Filename
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_GetRomFileInfo(UINT32 NvmID, UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo)
{
    UINT32 RetVal;
    AMBA_NVM_GET_ROM_FILE_INFO_f pGetRomFileInfoApi = _AmbaNVM_Ctrl[NvmID].GetRomFileInfo;

    if ((NvmID >= AMBA_NUM_NVM) || (pGetRomFileInfoApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pGetRomFileInfoApi(RegionID, Index, pRomFileInfo);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_ReadRomFile
 *
 *  @Description:: Read a specific file from ROM partiton
 *
 *  @Input      ::
 *      NvmID:     Memory type of the ROM partiton
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
UINT32 AmbaNVM_ReadRomFile(UINT32 NvmID, UINT32 RegionID, const char *pFileName, UINT32 StartPos,
                           UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = NVM_ERR_NONE;
    AMBA_NVM_READ_ROM_FILE_f pReadRomFileApi = _AmbaNVM_Ctrl[NvmID].ReadRomFile;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadRomFileApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pReadRomFileApi(RegionID, pFileName, StartPos, Size, pDataBuf, TimeOut);
        if (RetVal != NAND_ERR_NONE) {
            RetVal = (NvmID + NVM_ERR_NAND_READ);
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_GetFtlInfo
 *
 *  @Description:: Get FTL informatio of the partition
 *
 *  @Input      ::
 *      NvmID:      Memory type of the ROM partiton
 *      UserPartID: User Partition ID
 *      pFtlInfo:   pointer to the data buffer oof FTL information
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_GetFtlInfo(UINT32 NvmID, UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pInfo)
{
    UINT32 RetVal;
    AMBA_NVM_GET_FTL_INFO_f pGetFtlInfoApi = _AmbaNVM_Ctrl[NvmID].GetFtlInfo;

    if ((NvmID >= AMBA_NUM_NVM) || (pGetFtlInfoApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pGetFtlInfoApi(UserPartID, pInfo);
    }

    if (RetVal != NVM_ERR_NONE) {
        RetVal = (NvmID + NVM_ERR_NAND_READ);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_ReadSector
 *
 *  @Description:: Read sector data from partition
 *
 *  @Input      ::
 *      NvmID:       Memory type of the ROM partiton
 *      UserPartID:  User Partition ID
 *      pSecConfig:  Sector config struture
 *      TimeOut:     Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_ReadSector(UINT32 NvmID, UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_READ_SECTOR_f pReadSectorApi = _AmbaNVM_Ctrl[NvmID].ReadSector;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadSectorApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pReadSectorApi(UserPartID, pSecConfig, TimeOut);
    }

    if (RetVal != OK) {
        RetVal = (NvmID + NVM_ERR_NAND_READ);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_WriteSector
 *
 *  @Description:: Read sector data from partition
 *
 *  @Input      ::
 *      NvmID:       Memory type of the ROM partiton
 *      UserPartID:  User Partition ID
 *      pSecConfig:  Sector config struture
 *      TimeOut:     Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_WriteSector(UINT32 NvmID, UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_SECTOR_f pWrireSectorApi = _AmbaNVM_Ctrl[NvmID].WriteSector;

    if ((NvmID >= AMBA_NUM_NVM) || (pWrireSectorApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWrireSectorApi(UserPartID, pSecConfig, TimeOut);
    }

    if (RetVal != OK) {
        RetVal = (NvmID + NVM_ERR_NAND_WRITE);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_ReadSysPartitionTable
 *
 *  @Description:: Read System Partition Table
 *
 *  @Input      ::
 *      NvmID:         Memory type of the ROM partiton
 *      pSysPartTable: pointer to the buffer of System partition table
 *      TimeOut:       Time out value
 *
 *  @Output     ::
 *      pSysPartTable: pointer to the data of System partition table
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_ReadSysPartitionTable(UINT32 NvmID, AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_READ_SYS_PARTITION_TABLE_f pReadSysPartitionTableApi = _AmbaNVM_Ctrl[NvmID].ReadSysPartitionTable;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadSysPartitionTableApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pReadSysPartitionTableApi(pSysPartTable, TimeOut);
    }

    if ((RetVal != NVM_ERR_NONE) && (RetVal != NVM_ERR_ARG)) {
        RetVal = (NvmID + NVM_ERR_NAND_READ);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_WriteSysPartitionTable
 *
 *  @Description:: Write User Partition Table
 *
 *  @Input      ::
 *      NvmID:          Memory type of the ROM partiton
 *      pUserPartTable: pointer to the data of User partition table
 *      TimeOut:        Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_WriteSysPartitionTable(UINT32 NvmID, AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_SYS_PARTITION_TABLE_f pWriteSysPartitionTabeApi = _AmbaNVM_Ctrl[NvmID].WriteSysPartitionTabe;

    if ((NvmID >= AMBA_NUM_NVM) || (pWriteSysPartitionTabeApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWriteSysPartitionTabeApi(pSysPartTable, TimeOut);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_ReadUserPartitionTable
 *
 *  @Description:: Read User Partition Table
 *
 *  @Input      ::
 *      NvmID:         Memory type of the ROM partiton
 *      pUserPartTable: pointer to the buffer of User partition table
 *      TimeOut:        Time out value
 *
 *  @Output     ::
 *      pUserPartTable: pointer to the data of User partition table
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_ReadUserPartitionTable(UINT32 NvmID, AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_READ_USER_PARTITION_TABLE_f pReadUserPartitionTableApi = _AmbaNVM_Ctrl[NvmID].ReadUserPartitionTable;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadUserPartitionTableApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {

        RetVal = pReadUserPartitionTableApi(pUserPartTable, UserPtbNo, TimeOut);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_WriteUserPartitionTable
 *
 *  @Description:: Write User Partition Table
 *
 *  @Input      ::
 *      NvmID:          Memory type of the ROM partiton
 *      pUserPartTable: pointer to the data of User partition table
 *      TimeOut:        Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_WriteUserPartitionTable(UINT32 NvmID, AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_USER_PARTITION_TABLE_f pWriteUserPartitionTableApi = _AmbaNVM_Ctrl[NvmID].WriteUserPartitionTable;

    if ((NvmID >= AMBA_NUM_NVM) || (pWriteUserPartitionTableApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWriteUserPartitionTableApi(pUserPartTable, UserPtbNo, TimeOut);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_ReadPartitionInfo
 *
 *  @Description:: Read partition information
 *
 *  @Input      ::
 *      NvmID:    Memory type of the ROM partiton
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   partition ID
 *
 *  @Output     ::
 *      pPartInfo: pointer to partition information
 *
 *  @Return     ::
 *          INT32 : actual size of Vendor Specific Data (>0)/NG(-1)
 */
UINT32 AmbaNVM_ReadPartitionInfo(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    UINT32 RetVal;
    AMBA_NVM_READ_PARTITION_INFO_f pReadPartitionInfoApi = _AmbaNVM_Ctrl[NvmID].ReadPartitionInfo;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadPartitionInfoApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pReadPartitionInfoApi(PartFlag, PartID, pPartInfo);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_ReadPartition
 *
 *  @Description:: Read partition data from NVM device
 *
 *  @Input      ::
 *      NvmID:    Memory type of the ROM partiton
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   partition ID
 *      TimeOut:  Time out value
 *
 *  @Output     ::
 *      pDataBuf: pointer to partition data
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_ReadPartition(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_READ_PARTITION_f pReadPartitionApi = _AmbaNVM_Ctrl[NvmID].ReadPartition;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadPartitionApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pReadPartitionApi(PartFlag, PartID, pDataBuf, TimeOut);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_WritePartition
 *
 *  @Description:: Write partition data from NVM device
 *
 *  @Input      ::
 *      NvmID:     Memory type of the ROM partiton
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   partition ID
 *      DataSize: Data size in Byte to write to the partition
 *      pDataBuf: pointer to partition data
 *      TimeOut:  Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_WritePartition(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_PARTITION_f pWritePartitionApi = _AmbaNVM_Ctrl[NvmID].WritePartition;

    if ((NvmID >= AMBA_NUM_NVM) || (pWritePartitionApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWritePartitionApi(PartFlag, PartID, DataSize, pDataBuf, TimeOut);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_ErasePartition
 *
 *  @Description:: Erase a partition
 *
 *  @Input      ::
 *      NvmID:    Memory type of the ROM partiton
 *      PartFlag: 0 - System Partition; 1 - User Partition
 *      PartID:   partition ID
 *      TimeOut:  Time out value
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_ErasePartition(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_ERASE_PARTITION_f pErasePartitionApi = _AmbaNVM_Ctrl[NvmID].ErasePartition;

    if ((NvmID >= AMBA_NUM_NVM) || (pErasePartitionApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pErasePartitionApi(PartFlag, PartID, TimeOut);
    }
    return RetVal;
}

