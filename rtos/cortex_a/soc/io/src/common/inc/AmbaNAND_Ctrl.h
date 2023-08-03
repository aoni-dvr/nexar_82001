/**
 *  @file AmbaNAND_Ctrl.h
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
 *  @details Definitions & Constants for NAND Controller APIs (Internal Use)
 *
 */

#ifndef AMBA_NAND_CTRL_H
#define AMBA_NAND_CTRL_H

#ifndef AMBA_NAND_DEF_H
#include "AmbaNAND_Def.h"
#endif

/*
 * Defined in AmbaNAND.c
 */
UINT32 AmbaNAND_Lock(void);
UINT32 AmbaNAND_Unlock(void);

UINT32 AmbaNAND_Read(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut);
UINT32 AmbaNAND_Program(UINT32 PageAddr, UINT32 NumPage, UINT8 *pMainBuf, UINT8 *pSpareBuf, UINT32 TimeOut);
UINT32 AmbaNAND_CopyBack(UINT32 DestPageAddr, UINT32 SrcPageAddr, UINT32 TimeOut);
UINT32 AmbaNAND_EraseBlock(UINT32 BlkAddr, UINT32 TimeOut);

AMBA_NAND_DEV_INFO_s *AmbaNAND_GetDevInfo(void);
AMBA_NAND_COMMON_INFO_s *AmbaNAND_GetCommonInfo(void);

UINT32 AmbaNAND_Reset(UINT32 TimeOut);
UINT32 AmbaNAND_ReadID(UINT8 NumReadCycle, UINT8 *pDeviceID, UINT32 TimeOut);
//INT32 AmbaNAND_ReadStatus(AMBA_NAND_STATUS_u *pStatus, UINT32 TimeOut);

UINT32 AmbaNAND_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pFtlInfo);

UINT32 AmbaNAND_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);
UINT32 AmbaNAND_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);

/*
 * Defined in AmbaNAND_Partition.c
 */
UINT32 AmbaNAND_InitPtbBbt(UINT32 TimeOut);
UINT32 AmbaNAND_CreatePTB(void);
UINT32 AmbaNAND_CreateExtPTB(UINT32 StartBlk);
UINT32 AmbaNAND_LoadNvmRomFileTable(void);
UINT32 AmbaNAND_GetActivePtbNo(UINT32 * pPTBActive);
UINT32 AmbaNAND_SetActivePtbNo(UINT32 PtbNo, UINT32 Active);


UINT32 AmbaNAND_GetRomFileSize(UINT32 RegionID, const char *pFileName, UINT32 *pFileSize);
UINT32 AmbaNAND_GetRomFileInfo(UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo);
UINT32 AmbaNAND_ReadRomFile(UINT32 RegionID, const char *pFileName, UINT32 StartPos,
                            UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut);

UINT32 AmbaNAND_ReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
UINT32 AmbaNAND_WriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
UINT32 AmbaNAND_ReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);
UINT32 AmbaNAND_WriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);

UINT32 AmbaNAND_ReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo);
UINT32 AmbaNAND_ReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaNAND_ReadPartitionPartial(UINT32 PartFlag, UINT32 PartID, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf);
UINT32 AmbaNAND_WritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaNAND_InvalidatePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);
UINT32 AmbaNAND_ErasePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);

UINT32 AmbaNAND_WritePhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaNAND_ReadPhyBlock(UINT32 BlkAddr, UINT8 * pDataBuf, UINT32 TimeOut);

UINT32 AmbaNAND_IsBldMagicCodeSet(void);
UINT32 AmbaNAND_SetBldMagicCode(UINT32 TimeOut);
UINT32 AmbaNAND_EraseBldMagicCode(UINT32 TimeOut);
UINT32 AmbaNAND_ReadBldMagicCode(void);

void AmbaNAND_SetWritePtbFlag(UINT32 Flag);

/*
 * Defined in AmbaNAND_BBM.c
 */
UINT32 AmbaNAND_CreateBBT(UINT32 TimeOut);
UINT32 AmbaNAND_GetBlkMark(UINT32 BlkAddr);
UINT32 AmbaNAND_SetBlkMark(UINT32 BlkAddr, UINT32 BlkMark, UINT32 TimeOut);
INT32 AmbaNAND_FindGoodBlkBackward(UINT32 BlkAddr);
INT32 AmbaNAND_FindGoodBlkForward(UINT32 BlkAddr);
UINT32 AmbaNAND_UpdateBBT(UINT32 Version, UINT32 BlkAddrPrimary, UINT32 BlkAddrMirror, UINT32 TimeOut);
UINT32 AmbaNAND_WriteBBT(UINT32 TimeOut);
UINT32 AmbaNAND_LoadBBT(UINT32 BlkAddr, UINT32 TimeOut);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 AmbaNAND_WriteRawSector(UINT32 PartFlag, UINT32 PartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 *pBadSectors, UINT32 TimeOut);
UINT32 AmbaNAND_SwitchBoot(UINT8 bootA);
#endif

#endif /* AMBA_NAND_CTRL_H */
