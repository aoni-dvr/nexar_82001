/**
 *  @file AmbaSpiNOR_Ctrl.h
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
 *  @details Definitions & Constants for NOR Controller APIs (Internal Use)
 *
 */

#ifndef AMBA_NOR_SPI_CTRL_H
#define AMBA_NOR_SPI_CTRL_H

#ifndef AMBA_NOR_SPI_DEF_H
#include "AmbaSpiNOR_Def.h"
#endif

/*
 * The flag of Nor interrupt definitions.
 */
#define NOR_TRANS_DONE_FLAG     (0x1U)
#define NOR_SPIDMA_DONE_FLAG    (0x2U)

/*
 *  Timeout value for CMD and DataTrans
 */
#define NOR_CMD_TIMEOUT_VALUE    (1000U)
#define NOR_TRAN_TIMEOUT_VALUE   (10000U)

void AmbaSpiNOR_SetWritePtbFlag(UINT32 Flag);

INT32 AmbaSpiNOR_CreatePTB(void);
UINT32 AmbaSpiNOR_InitPtbBbt(UINT32 TimeOut);
UINT32 AmbaSPINOR_InitUserPartLock(void);

UINT32 AmbaSpiNOR_LoadNvmRomFileTable(void);
UINT32 AmbaSpiNOR_GetRomFileInfo(UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s * pRomFileInfo);
UINT32 AmbaSpiNOR_GetRomFileSize(UINT32 RegionID, const char *pFileName, UINT32 *pFileSize);
UINT32 AmbaSpiNOR_ReadRomFile(UINT32 RegionID, const char *pFileName, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaSpiNOR_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pFtlInfo);
UINT32 AmbaSpiNOR_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);
UINT32 AmbaSpiNOR_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);

UINT32 AmbaSpiNOR_ErasePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);

UINT32 AmbaSpiNOR_ReadSysPartitionTable(AMBA_SYS_PARTITION_TABLE_s * pSysPartTable, UINT32 TimeOut);
UINT32 AmbaSpiNOR_WriteSysPartitionTable(AMBA_SYS_PARTITION_TABLE_s * pSysPartTable, UINT32 TimeOut);

UINT32 AmbaSpiNOR_ReadUserPartitionTable(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);
UINT32 AmbaSpiNOR_WriteUserPartitionTable(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);

UINT32 AmbaSpiNOR_ReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo);
UINT32 AmbaSpiNOR_InvalidatePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);
UINT32 AmbaSpiNOR_WritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 * pDataBuf, UINT32 TimeOut);
UINT32 AmbaSpiNOR_ReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 * pDataBuf, UINT32 TimeOut);

UINT32 AmbaSpiNOR_WritePhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaSpiNOR_ReadPhyBlock(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut);

UINT32 AmbaSpiNOR_SetActivePtbNo(UINT32 PtbNo, UINT32 Active);
UINT32 AmbaSpiNOR_GetActivePtbNo(UINT32 * pPTBActive);
/*
 * Defined in AmbaNOR.c (MW for NOR flash)
 */
UINT32 AmbaSPINOR_WriteDisable(void);

UINT32 AmbaSPINOR_ClearStatus(void);

UINT32 AmbaSpiNOR_Erase(UINT32 Offset, UINT32 ByteCount, UINT32 TimeOut);
UINT32 AmbaSpiNOR_SectorErase(UINT32 Offset, UINT32 ByteCount, UINT32 TimeOut);
UINT32 AmbaSpiNOR_Readbyte(UINT32 Offset, UINT32 ByteCount, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 AmbaSpiNOR_Program(UINT32 Offset, UINT32 ByteCount, const UINT8 *pDataBuf, UINT32 TimeOut);
UINT8 AmbaSpiNOR_Isinit(void);
void AmbaSPINOR_RestoreRebootClkSetting(void);
void AmbaSpiNOR_HandleCoreFreqChange(void);
AMBA_NORSPI_DEV_INFO_s *AmbaSpiNOR_GetDevInfo(void);
#endif /* _AMBA_NOR_SPI_H_ */
