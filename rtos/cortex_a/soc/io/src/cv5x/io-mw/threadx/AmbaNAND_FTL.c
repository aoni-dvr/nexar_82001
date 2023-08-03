/**
 *  @file AmbaNAND_FTL.c
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
 *  @details NAND Flash Translation Layer (NFTL) APIs
 *
 */

#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"

#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"
#include "AmbaNAND_FTL.h"

#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_NAND_Ctrl.h"

static UINT32 AmbaNFTL_InitMode[AMBA_NUM_USER_PARTITION] = {0};

/*
 *  @RoutineName:: AmbaNAND_FtlInit
 *
 *  @Description:: NAND FTL initializations
 *
 *  @Input      ::
 *      UserPartID: User Partition ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_FtlInit(UINT32 UserPartID)
{
    const AMBA_USER_PARTITION_TABLE_s *pPartTable = pAmbaNandTblUserPart;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = &pPartTable->Entry[UserPartID];
    UINT32 RetVal = NAND_ERR_NONE;
    UINT32 Attr = CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_FTL);

    if ((UserPartID >= AMBA_NUM_USER_PARTITION) ||
        (Attr == 0x0U)                          ||
        (pPartEntry->ByteCount == 0U)) {
        RetVal = NAND_ERR_ARG;
    } else {
        RetVal = AmbaNAND_InitNftlPart(UserPartID, pPartEntry->StartBlkAddr, pPartEntry->BlkCount);
        if (RetVal == NAND_ERR_NONE) {
            if (AmbaNFTL_IsInit(UserPartID) != OK) {
                RetVal = AmbaNFTL_InitLock(UserPartID);
            }

            if (RetVal == NAND_ERR_NONE) {
                if (UserPartID == AMBA_USER_PARTITION_CALIBRATION_DATA) {
                    RetVal = AmbaNFTL_Init(UserPartID, 2U);
                } else {
                    RetVal = AmbaNFTL_Init(UserPartID, AmbaNFTL_InitMode[UserPartID]);
                }
            }
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_FtlSetInitMode
 *
 *  @Description::
 *
 *  @Input      ::
 *      UserPartID: User Partition ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_FtlSetInitMode(UINT32 UserPartID, UINT32 Mode)
{
    const AMBA_USER_PARTITION_TABLE_s *pPartTable = pAmbaNandTblUserPart;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = &pPartTable->Entry[UserPartID];
    UINT32 RetVal = NAND_ERR_NONE;
    UINT32 Attr = CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_FTL);

    AmbaMisra_TouchUnused(&AmbaNFTL_InitMode[0]);
    if ((UserPartID >= AMBA_NUM_USER_PARTITION) ||
        (Attr == 0U)  ||
        (pPartEntry->ByteCount == 0U)) {
        RetVal = NAND_ERR_ARG;  /* wrong parameter */
    } else {
        AmbaNFTL_InitMode[UserPartID] = Mode;
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNVM_GetFtlInfo
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
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pFtlInfo)
{
    UINT32 RetVal = NAND_ERR_NONE;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = & (pAmbaNandTblUserPart->Entry[UserPartID]);
    const AMBA_NAND_COMMON_INFO_s *pNandDevInfo = AmbaRTSL_NandCommonInfo;     /* Pointer to external NAND device information */
    UINT32 Attr = CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_FTL);

    if ((UserPartID >= AMBA_NUM_USER_PARTITION) || (Attr == 0U)) {
        RetVal = NAND_ERR_ARG;  /* wrong parameter */
    } else {
        pFtlInfo->Initialized = AmbaNFTL_IsInit(UserPartID); /* 1 - this partion has been initialized */

        if (CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_WRITE_PROTECT) != 0U) {
            pFtlInfo->WriteProtect = 1U; /* 1 - this partition is write protected */
        } else {
            pFtlInfo->WriteProtect = 0;
        }

        /* Total size of the FTL partition in Sectors */
        (void)AmbaNFTL_GetTotalSecs(UserPartID, &pFtlInfo->TotalSizeInSectors);
        pFtlInfo->SectorSizeInBytes = 512U;                                  /* Sector size in Bytes */

        pFtlInfo->TotalSizeInBytes = (UINT64)(pFtlInfo->TotalSizeInSectors) * pFtlInfo->SectorSizeInBytes;
        /* Total size of the FTL partition in Bytes */

        pFtlInfo->PageSizeInBytes   = pNandDevInfo->MainByteSize;             /* Page size in Bytes */
        pFtlInfo->BlockSizeInPages  = pNandDevInfo->BlockPageSize;           /* Block size in Pages */
        pFtlInfo->EraseSizeInBlocks = 1U;                                    /* Erase size in Blocks */
    }
    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_ReadSector
 *
 *  @Description:: Read NAND sector data
 *
 *  @Input      ::
 *      UserPartID:     User Partition ID
 *      pSecConfig:     Sector config struture
 *      TimeOut:        Time out value
 *
 *  @Output     ::
 *      pDataBuf:    pointer to the data
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaNandTblUserPart->Entry[UserPartID]);
    UINT32 Attr = CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_FTL);

    AmbaMisra_TouchUnused(&TimeOut);

    if ((UserPartID >= AMBA_NUM_USER_PARTITION) || /* wrong parameter */
        (Attr == 0x0U)) {   /* wrong parameter: not a FTL partition */
        RetVal = NAND_ERR_ARG;
    } else {
        RetVal = AmbaNFTL_Read(UserPartID, (UINT8 *)pSecConfig->pDataBuf, pSecConfig->StartSector, pSecConfig->NumSector);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_WriteSector
 *
 *  @Description:: Write NAND Sector data
 *
 *  @Input      ::
 *      UserPartID:     User Partition ID
 *      pSecConfig:     Sector config struture
 *      TimeOut:        Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
    const AMBA_PARTITION_ENTRY_s *pPartEntry = (AMBA_PARTITION_ENTRY_s *) & (pAmbaNandTblUserPart->Entry[UserPartID]);
    UINT32 Attr = CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_FTL);

    AmbaMisra_TouchUnused(&TimeOut);

    if ((UserPartID >= AMBA_NUM_USER_PARTITION) || /* wrong parameter */
        (Attr == 0x0U)) {   /* wrong parameter: not a FTL partition */
        RetVal = NAND_ERR_ARG;
    } else {
        RetVal = AmbaNFTL_Write((UINT32) UserPartID, (UINT8 *)pSecConfig->pDataBuf, pSecConfig->StartSector, pSecConfig->NumSector);
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaNAND_FTL_Format
 *
 *  @Description:: NAND format a partition
 *
 *  @Input      ::
 *      UserPartID: User Partition ID
 *      TimeOut:    Time out value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
 */
UINT32 AmbaNAND_FTL_Format(UINT32 UserPartID, UINT32 TimeOut)
{
    UINT32 RetVal = NAND_ERR_NONE;
#if 0
    AMBA_USER_PARTITION_TABLE_s *pPartTable = pAmbaNandTblUserPart;
    AMBA_PARTITION_ENTRY_s *pPartEntry = &pPartTable->Entry[UserPartID];

    AmbaMisra_TouchUnused(&TimeOut);

    if (UserPartID >= AMBA_NUM_USER_PARTITION) {
        RetVal = NAND_ERR_ARG;  /* wrong parameter */
    } else {
        if (CheckBits(pPartEntry->Attribute, AMBA_PARTITION_ATTR_FTL) == 0x0U) {
            RetVal = NAND_ERR_ARG;  /* wrong parameter: not a FTL partition */
        } else {
            RetVal = AmbaNFTL_EraseLogicalBlocks(UserPartID, pPartEntry->StartBlkAddr, pPartEntry->BlkCount);
        }
    }
#else
    (void)UserPartID;
    (void)TimeOut;
#endif
    return RetVal;
}
