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

#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNVM_Ctrl.h"
#include "AmbaNAND_BBM_Def.h"
#include "AmbaNAND_OP.h"
#include "AmbaCache.h"

//#include "AmbaRTSL_NAND.h"
//#include "AmbaRTSL_NAND_Ctrl.h"
//#include "AmbaRTSL_FIO.h"


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
    (void) pPTBActive;

    for (i = 0; i < 2U; i++) {
        RetVal = AmbaNAND_ReadUserPTB(NULL, i, 1000);
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
    UINT32 RetVal = NAND_ERR_NONE;
    (void) PtbNo;
    (void) Active;

    return RetVal;
}

/**
 *  AmbaNAND_CreatePtbBbt -
 *  @param[in] TimeOut Time out value
 *  @return error code
 */

/**
 *  AmbaNAND_InitPtbBbt - Init BBT and System/User Partition Tables
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_InitPtbBbt(UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
    (void) TimeOut;

    return RetVal;
}


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


/**
 *  AmbaNAND_LoadNvmRomFileTable - Load all NAND ROM File Tables
 *  @return error code
 */
UINT32 AmbaNAND_LoadNvmRomFileTable(void)
{
    UINT32 RetVal = NAND_ERR_NONE;
    return RetVal;
}

/**
 *  FileNameCmp -
 *  @return
 */

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
    (void) RegionID;
    (void) pFileName;
    (void) pFileSize;

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
    (void) RegionID;
    (void) Index;
    (void) pRomFileInfo;

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
    UINT32 RetVal = NAND_ERR_NONE;
    (void) RegionID;
    (void) pFileName;
    (void) StartPos;
    (void) Size;
    (void) pDataBuf;
    (void) TimeOut;

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
    UINT32 RetVal = NAND_ERR_IO_FAIL;
    (void) pSysPartTable;
    (void) TimeOut;

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
    UINT32 RetVal = NAND_ERR_IO_FAIL;
    (void) pSysPartTable;
    (void) TimeOut;
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
    UINT32 RetVal = NAND_ERR_NONE;
    (void) pUserPartTable;
    (void) UserPtbNo;
    (void) TimeOut;
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
    UINT32 RetVal = NAND_ERR_NONE;
    (void) pUserPartTable;
    (void) UserPtbNo;
    (void) TimeOut;
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
    (void) PartFlag;
    (void) PartID;
    (void) pPartInfo;
    return 0;//AmbaRTSL_NandReadPartitionInfo(PartFlag, PartID, pPartInfo);
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
    UINT32 RetVal = OK;
    (void) PartFlag;
    (void) PartID;
    (void) StartPos;
    (void) Size;
    (void) pDataBuf;

    return RetVal;
}

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
    UINT32 RetVal = NAND_ERR_NONE;
    (void) PartFlag;
    (void) PartID;
    (void) pDataBuf;
    (void) TimeOut;
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
    UINT32 RetVal = NAND_ERR_NONE;
    (void) PartFlag;
    (void) PartID;
    (void) DataSize;
    (void) pDataBuf;
    (void) TimeOut;

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
    UINT32 RetVal = NAND_ERR_NONE;
    (void) PartFlag;
    (void) PartID;
    (void) TimeOut;

    return RetVal;
}

/*
 *  AmbaNAND_WritePhyBlock - Write a phyical block
 *  @param[in] BlkAddr write block address
 *  @param[in] pDataBuf pointer to write data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_WritePhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
    (void) BlkAddr;
    (void) pDataBuf;
    (void) TimeOut;
    return RetVal;
}

/*
 *  AmbaNAND_ReadPhyBlock - Read a phyical block
 *  @param[in] BlkAddr write block address
 *  @param[out] pDataBuf pointer to read data
 *  @param[in] TimeOut Time out value
 *  @return error code
 */
UINT32 AmbaNAND_ReadPhyBlock(UINT32 BlkAddr, UINT8 * pDataBuf, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
    (void) BlkAddr;
    (void) pDataBuf;
    (void) TimeOut;

    return RetVal;
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
    AmbaMisra_TouchUnused(&TimeOut);
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
    return 0;//AmbaRTSL_NandReadBldMagicCode();
}

