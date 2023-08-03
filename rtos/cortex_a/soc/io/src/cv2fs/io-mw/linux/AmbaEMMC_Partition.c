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

#include "AmbaNAND.h"
#include "AmbaNVM_Ctrl.h"

#include "AmbaSD.h"
//#include "AmbaRTSL_SD.h"
#include "AmbaSD_Ctrl.h"

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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) pPTBActive;

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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) PtbNo;
    (void) Active;

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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) TimeOut;

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
    UINT32 RetVal = NVM_ERR_NONE;
    return RetVal;
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
 *          int : (>0)/NG(-1)
 */
UINT32 AmbaEMMC_GetRomFileInfo(UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo)
{
    UINT32 RetVal = NVM_ERR_NONE;

    (void) RegionID;
    (void) Index;
    (void) pRomFileInfo;

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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) RegionID;
    (void) StartPos;
    (void) Size;
    (void) pFileName;
    (void) pDataBuf;
    (void) TimeOut;
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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) UserPartID;
    (void) pFtlInfo;
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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) pSysPartTable;
    (void) TimeOut;
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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) pSysPartTable;
    (void) TimeOut;
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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) pUserPartTable;
    (void) UserPtbNo;
    (void) TimeOut;
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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) pUserPartTable;
    (void) UserPtbNo;
    (void) TimeOut;
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

    (void) PartFlag;
    (void) PartID;
    (void) pPartInfo;

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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) PartFlag;
    (void) PartID;
    (void) pDataBuf;
    (void) TimeOut;
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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) PartFlag;
    (void) PartID;
    (void) DataSize;
    (void) pDataBuf;
    (void) TimeOut;
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
    UINT32 RetVal = NVM_ERR_NONE;
    (void) PartFlag;
    (void) PartID;
    (void) TimeOut;
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
    (void) PartFlag;
    (void) PartID;
    (void) TimeOut;
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
    (void) BlkAddr;
    (void) pDataBuf;
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
    (void) BlkAddr;
    (void) pDataBuf;
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
    (void) Flag;
}

UINT32 AmbaEMMC_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal = NVM_ERR_NONE;
    (void) UserPartID;
    (void) pSecConfig;
    (void) TimeOut;

    return RetVal;
}

UINT32 AmbaEMMC_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal = NVM_ERR_NONE;

    (void) UserPartID;
    (void) TimeOut;
    (void) pSecConfig;

    return RetVal;
}

