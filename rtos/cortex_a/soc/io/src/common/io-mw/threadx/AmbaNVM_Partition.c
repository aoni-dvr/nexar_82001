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
#include "AmbaDef.h"
#include "AmbaKAL.h"

#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
//#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"
#include "AmbaNVM_Ctrl.h"

static AMBA_NVM_CTRL_s AmbaNVM_Ctrl[AMBA_NUM_NVM] = {
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
        .WritePhyBlock           = AmbaNAND_WritePhyBlock,
        .ReadPhyBlock            = AmbaNAND_ReadPhyBlock,
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        .WriteRawSector          = AmbaNAND_WriteRawSector,
        .IsBldMagicCodeSet       = AmbaNAND_IsBldMagicCodeSet,
        .SetBldMagicCode         = AmbaNAND_SetBldMagicCode,
        .EraseBldMagicCode       = AmbaNAND_EraseBldMagicCode,
        .ReadBldMagicCode        = AmbaNAND_ReadBldMagicCode,
#endif
    },

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
        .WritePhyBlock           = AmbaEMMC_WritePhyBlock,
        .ReadPhyBlock            = AmbaEMMC_ReadPhyBlock,
    },

    [AMBA_NVM_SPI_NOR] = {
        .GetRomFileInfo          = AmbaSpiNOR_GetRomFileInfo,
        .GetRomFileSize          = AmbaSpiNOR_GetRomFileSize,        /* pointer to get ROM file size function */
        .ReadRomFile             = AmbaSpiNOR_ReadRomFile,           /* pointer to read ROM file function */

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
        .WritePhyBlock           = AmbaSpiNOR_WritePhyBlock,
        .ReadPhyBlock            = AmbaSpiNOR_ReadPhyBlock,
    },

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
        .WritePhyBlock           = AmbaNAND_WritePhyBlock,
        .ReadPhyBlock            = AmbaNAND_ReadPhyBlock,
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        .WriteRawSector          = AmbaNAND_WriteRawSector,
        .IsBldMagicCodeSet       = AmbaNAND_IsBldMagicCodeSet,
        .SetBldMagicCode         = AmbaNAND_SetBldMagicCode,
        .EraseBldMagicCode       = AmbaNAND_EraseBldMagicCode,
        .ReadBldMagicCode        = AmbaNAND_ReadBldMagicCode,
#endif
    },
};

static UINT32 AmbaSyncUserPartitionTable(UINT32 SouNvmID)
{
#if defined(CONFIG_MUTI_BOOT_DEVICE)
    AMBA_NVM_READ_USER_PARTITION_TABLE_f pReadUserPartitionTableApi = NULL;
    AMBA_NVM_WRITE_USER_PARTITION_TABLE_f pWriteUserPartitionTableApi = NULL;
    AMBA_USER_PARTITION_TABLE_s UserPartTable;
    UINT32 DestNvmID = AMBA_NVM_eMMC;
    extern AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_NorSpiUserPartTable;
    UINT32 UserPtbNo = pAmbaRTSL_NorSpiUserPartTable->PTBNumber;
    UINT32 RetVal;

    if (SouNvmID < AMBA_NUM_NVM) {
        if (SouNvmID == AMBA_NVM_eMMC) {
            DestNvmID = AMBA_NVM_SPI_NOR;
            RetVal = OK;
        } else if (SouNvmID == AMBA_NVM_SPI_NOR) {
            DestNvmID = AMBA_NVM_eMMC;
            RetVal = OK;
        } else {
            RetVal = OK;
        }
    } else {
        RetVal = NVM_ERR_ARG;
    }

    if (RetVal == OK) {
        if (SouNvmID < AMBA_NUM_NVM) {
            pReadUserPartitionTableApi = AmbaNVM_Ctrl[SouNvmID].ReadUserPartitionTable;
        }
        if (DestNvmID < AMBA_NUM_NVM) {
            pWriteUserPartitionTableApi = AmbaNVM_Ctrl[DestNvmID].WriteUserPartitionTable;
        }

        if ((SouNvmID >= AMBA_NUM_NVM) || (pReadUserPartitionTableApi == NULL)) {
            RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
        } else {

            RetVal = pReadUserPartitionTableApi(&UserPartTable, UserPtbNo, 5000U);
        }

        if (RetVal == OK) {
            if ((DestNvmID >= AMBA_NUM_NVM) || (pWriteUserPartitionTableApi == NULL)) {
                RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
            } else {
                RetVal = pWriteUserPartitionTableApi(&UserPartTable, UserPtbNo, 5000U);
            }
        }
    }

    return RetVal;
#else
    (void) SouNvmID;
    return OK;
#endif
}

/**
 *  AmbaNVM_GetRomFileSize - Get the size of the specific file in ROM partition
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] RegionID ROM Region ID
 *  @param[in] pFileName pointer to the Filename
 *  @param[out] pFileSize pointer to the file size
 *  @return error code
 */
UINT32 AmbaNVM_GetRomFileSize(UINT32 NvmID, UINT32 RegionID, const char *pFileName, UINT32 *pFileSize)
{
    UINT32 RetVal;
    AMBA_NVM_GET_ROM_FILE_SIZE_f pGetRomFileSizeApi = AmbaNVM_Ctrl[NvmID].GetRomFileSize;

    (*pFileSize) = 0;

    if ((NvmID >= AMBA_NUM_NVM) || (pGetRomFileSizeApi == NULL)) {
        RetVal = NVM_ERR_ARG;    /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pGetRomFileSizeApi(RegionID, pFileName, pFileSize);
    }

    return RetVal;
}

/**
 *  AmbaNAND_GetRomFileInfo - Get the file info of the specific file in ROM partition
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] RegionID ROM Region ID
 *  @param[in] Index Index of the ROM Filename
 *  @param[out] pRomFileInfo pointer to the file info
 *  @return error code
 */
UINT32 AmbaNVM_GetRomFileInfo(UINT32 NvmID, UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo)
{
    UINT32 RetVal;
    AMBA_NVM_GET_ROM_FILE_INFO_f pGetRomFileInfoApi = AmbaNVM_Ctrl[NvmID].GetRomFileInfo;

    if ((NvmID >= AMBA_NUM_NVM) || (pGetRomFileInfoApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pGetRomFileInfoApi(RegionID, Index, pRomFileInfo);
    }

    return RetVal;
}

/**
 *  AmbaNVM_ReadRomFile - Read a specific file from ROM partiton
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] RegionID ROM Region ID
 *  @param[in] pFileName Filename
 *  @param[in] StartPos Byte position of the file
 *  @param[in] Size Byte count to be read
 *  @param[out] pDataBuf Data buffer to store read data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_ReadRomFile(UINT32 NvmID, UINT32 RegionID, const char *pFileName, UINT32 StartPos,
                           UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = NVM_ERR_NONE;
    AMBA_NVM_READ_ROM_FILE_f pReadRomFileApi = AmbaNVM_Ctrl[NvmID].ReadRomFile;

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

/**
 *  AmbaNVM_GetFtlInfo - Get FTL informatino of the partition
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] UserPartID User Partition ID
 *  @param[out] pFtlInfo pointer to the data buffer of FTL information
 *  @return error code
 */
UINT32 AmbaNVM_GetFtlInfo(UINT32 NvmID, UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pInfo)
{
    UINT32 RetVal;
    AMBA_NVM_GET_FTL_INFO_f pGetFtlInfoApi = AmbaNVM_Ctrl[NvmID].GetFtlInfo;

    if ((NvmID >= AMBA_NUM_NVM) || (pGetFtlInfoApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pGetFtlInfoApi(UserPartID, pInfo);
    }

    return RetVal;
}

/**
 *  AmbaNVM_ReadSector - Read sector data from partition
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] UserPartID User Partition ID
 *  @param[in] pSecConfig Sector config struture
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_ReadSector(UINT32 NvmID, UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_READ_SECTOR_f pReadSectorApi = AmbaNVM_Ctrl[NvmID].ReadSector;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadSectorApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pReadSectorApi(UserPartID, pSecConfig, TimeOut);
    }

    return RetVal;
}

/**
 *  AmbaNVM_WriteSector - Read sector data from partition
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] UserPartID User Partition ID
 *  @param[in] pSecConfig Sector config struture
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_WriteSector(UINT32 NvmID, UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_SECTOR_f pWrireSectorApi = AmbaNVM_Ctrl[NvmID].WriteSector;

    if ((NvmID >= AMBA_NUM_NVM) || (pWrireSectorApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWrireSectorApi(UserPartID, pSecConfig, TimeOut);
    }

    return RetVal;
}

/**
 *  AmbaNVM_ReadSysPartitionTable - Read System Partition Table
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[out] pSysPartTable pointer to the data of System partition table
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_ReadSysPartitionTable(UINT32 NvmID, AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_READ_SYS_PARTITION_TABLE_f pReadSysPartitionTableApi = AmbaNVM_Ctrl[NvmID].ReadSysPartitionTable;

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

/**
 *  AmbaNVM_WriteSysPartitionTable - Write User Partition Table
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[out] pUserPartTable pointer to the data of User partition table
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_WriteSysPartitionTable(UINT32 NvmID, AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_SYS_PARTITION_TABLE_f pWriteSysPartitionTabeApi = AmbaNVM_Ctrl[NvmID].WriteSysPartitionTabe;

    if ((NvmID >= AMBA_NUM_NVM) || (pWriteSysPartitionTabeApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWriteSysPartitionTabeApi(pSysPartTable, TimeOut);
    }
    return RetVal;
}

/**
 *  AmbaNVM_ReadUserPartitionTable - Read User Partition Table
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[out] pUserPartTable pointer to the data of User partition table
 *  @param[in] UserPtbNo
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_ReadUserPartitionTable(UINT32 NvmID, AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_READ_USER_PARTITION_TABLE_f pReadUserPartitionTableApi = AmbaNVM_Ctrl[NvmID].ReadUserPartitionTable;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadUserPartitionTableApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {

        RetVal = pReadUserPartitionTableApi(pUserPartTable, UserPtbNo, TimeOut);
    }
    return RetVal;
}

/**
 *  AmbaNVM_WriteUserPartitionTable - Write User Partition Table
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[out] pUserPartTable pointer to the data of User partition table
 *  @param[in] UserPtbNo
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_WriteUserPartitionTable(UINT32 NvmID, AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_USER_PARTITION_TABLE_f pWriteUserPartitionTableApi = AmbaNVM_Ctrl[NvmID].WriteUserPartitionTable;

    if ((NvmID >= AMBA_NUM_NVM) || (pWriteUserPartitionTableApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWriteUserPartitionTableApi(pUserPartTable, UserPtbNo, TimeOut);
    }

    if (RetVal == OK) {
        RetVal = AmbaSyncUserPartitionTable(NvmID);
    }

    return RetVal;
}

/**
 *  AmbaNVM_ReadPartitionInfo - Read partition information
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] PartFlag: 0 - System Partition; 1 - User Partition
 *  @param[in] PartID partition ID
 *  @param[out] pPartInfo pointer to partition information
 *  @return error code
 */
UINT32 AmbaNVM_ReadPartitionInfo(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    UINT32 RetVal;
    AMBA_NVM_READ_PARTITION_INFO_f pReadPartitionInfoApi = AmbaNVM_Ctrl[NvmID].ReadPartitionInfo;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadPartitionInfoApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pReadPartitionInfoApi(PartFlag, PartID, pPartInfo);
    }
    return RetVal;
}

/**
 *  AmbaNVM_ReadPartition - Read partition data from NVM device
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] PartFlag: 0 - System Partition; 1 - User Partition
 *  @param[in] PartID partition ID
 *  @param[out] pDataBuf pointer to partition data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_ReadPartition(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_READ_PARTITION_f pReadPartitionApi = AmbaNVM_Ctrl[NvmID].ReadPartition;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadPartitionApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pReadPartitionApi(PartFlag, PartID, pDataBuf, TimeOut);
    }
    return RetVal;
}

/**
 *  AmbaNVM_WritePartition - Write partition data from NVM device
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] PartFlag: 0 - System Partition; 1 - User Partition
 *  @param[in] PartID partition ID
 *  @param[in] DataSize Data size in Byte to write to the partition
 *  @param[in] pDataBuf pointer to partition data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_WritePartition(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_PARTITION_f pWritePartitionApi = AmbaNVM_Ctrl[NvmID].WritePartition;

    if ((NvmID >= AMBA_NUM_NVM) || (pWritePartitionApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWritePartitionApi(PartFlag, PartID, DataSize, pDataBuf, TimeOut);
    }

    if (RetVal == OK) {
        RetVal = AmbaSyncUserPartitionTable(NvmID);
    }

    return RetVal;
}

/**
 *  AmbaNVM_ErasePartition - Erase a partition
 *  @param[in] NvmID Memory type of the ROM partiton
 *  @param[in] PartFlag: 0 - System Partition; 1 - User Partition
 *  @param[in] PartID partition ID
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNVM_ErasePartition(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_ERASE_PARTITION_f pErasePartitionApi = AmbaNVM_Ctrl[NvmID].ErasePartition;

    if ((NvmID >= AMBA_NUM_NVM) || (pErasePartitionApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pErasePartitionApi(PartFlag, PartID, TimeOut);
    }

    if (RetVal == OK) {
        RetVal = AmbaSyncUserPartitionTable(NvmID);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_WritePhyBlock
 *
 *  @Description:: Read data to specific block
 *
 *  @Input      ::
 *      NvmID:    Memory type of the ROM partiton
 *      BlkAddr: Block address
 *      BlkCount:  Block count
 *      Forward:  1: The next block is searched  Forward 0: The next block is searched  Backward
 *      TimeOut:  Time out value
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_WritePhyBlock(UINT32 NvmID, UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_PHY_BLOCK_f pWritePhyBlockApi = AmbaNVM_Ctrl[NvmID].WritePhyBlock;

    if ((NvmID >= AMBA_NUM_NVM) || (pWritePhyBlockApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWritePhyBlockApi(BlkAddr, pDataBuf, TimeOut);
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_ReadPhyBlock
 *
 *  @Description:: Read data to specific block
 *
 *  @Input      ::
 *      NvmID:    Memory type of the ROM partiton
 *      BlkAddr: Block address
 *      BlkCount:  Block count
 *      Forward:  1: The next block is searched  Forward 0: The next block is searched  Backward
 *      TimeOut:  Time out value
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNVM_ReadPhyBlock(UINT32 NvmID, UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_READ_PHY_BLOCK_f pReadPhyBlockApi = AmbaNVM_Ctrl[NvmID].ReadPhyBlock;

    if ((NvmID >= AMBA_NUM_NVM) || (pReadPhyBlockApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pReadPhyBlockApi(BlkAddr, pDataBuf, TimeOut);
    }
    return RetVal;

}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 AmbaNVM_WriteRawSector(UINT32 NvmID, UINT32 PartFlag, UINT32 PartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 *pBadSectors, UINT32 TimeOut)
{
    UINT32 RetVal;
    AMBA_NVM_WRITE_RAW_SECTOR_f pWrireRawSectorApi = AmbaNVM_Ctrl[NvmID].WriteRawSector;

    if ((NvmID >= AMBA_NUM_NVM) || (pWrireRawSectorApi == NULL)) {
        RetVal = NVM_ERR_ARG;  /* wrong parameter, or API not registered yet */
    } else {
        RetVal = pWrireRawSectorApi(PartFlag, PartID, pSecConfig, pBadSectors, TimeOut);
    }

    if (RetVal != OK) {
        RetVal = (NvmID + NVM_ERR_NAND_WRITE);
    }

    return RetVal;
}

UINT32 AmbaNVM_IsBldMagicCodeSet(UINT32 NvmID)
{
    AMBA_NVM_IS_BLD_MAGIC_CODE_SET_f pIsBldMagicCodeSetApi = AmbaNVM_Ctrl[NvmID].IsBldMagicCodeSet;

    if (NvmID >= AMBA_NUM_NVM || pIsBldMagicCodeSetApi == NULL)
        return -1;  /* wrong parameter, or API not registered yet */

    return pIsBldMagicCodeSetApi();
}

UINT32 AmbaNVM_SetBldMagicCode(UINT32 NvmID, UINT32 TimeOut)
{
    AMBA_NVM_SET_BLD_MAGIC_CODE_f pSetBldMagicCodeApi = AmbaNVM_Ctrl[NvmID].SetBldMagicCode;

    if (NvmID >= AMBA_NUM_NVM || pSetBldMagicCodeApi == NULL)
        return -1;  /* wrong parameter, or API not registered yet */

    return pSetBldMagicCodeApi(TimeOut);
}

UINT32 AmbaNVM_EraseBldMagicCode(UINT32 NvmID, UINT32 TimeOut)
{
    AMBA_NVM_ERASE_BLD_MAGIC_CODE_f pEraseBldMagicCodeApi = AmbaNVM_Ctrl[NvmID].EraseBldMagicCode;

    if (NvmID >= AMBA_NUM_NVM || pEraseBldMagicCodeApi == NULL)
        return -1;  /* wrong parameter, or API not registered yet */

    return pEraseBldMagicCodeApi(TimeOut);
}

UINT32 AmbaNVM_ReadBldMagicCode(UINT32 NvmID)
{
    AMBA_NVM_READ_BLD_MAGIC_CODE_f pReadBldMagicCodeApi = AmbaNVM_Ctrl[NvmID].ReadBldMagicCode;

    if (NvmID >= AMBA_NUM_NVM || pReadBldMagicCodeApi == NULL)
        return -1;  /* wrong parameter, or API not registered yet */

    return pReadBldMagicCodeApi();
}
#endif

