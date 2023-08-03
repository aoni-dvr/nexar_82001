/**
*  @file SvcNvm.h
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*
*  @details svc nvm definition
*
*/

#ifndef SVC_NVM_H
#define SVC_NVM_H

#include "AmbaNVM_Partition.h"

typedef void (*SVC_NVM_READ_ROM_FILE_POST_f)(UINT32 NvmID, UINT32 RegionID, UINT32 FileIdx, UINT32 ReadSize, UINT32 CalcCRC32, const char *pFileName);
UINT32 SvcNvm_RegisterReadRomPost(SVC_NVM_READ_ROM_FILE_POST_f pFunc);

void SvcNvm_ShowPtb(void);
void SvcNvm_ShowRomList(UINT32 RegionID);

UINT32 SvcNvm_GetRomFileInfo(UINT32 UserPartID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s * pRomFileInfo);
UINT32 SvcNvm_GetRomFileSize(UINT32 UserPartID, const char *pFileName, UINT32 *pFileSize);
UINT32 SvcNvm_ReadRomFile(UINT32 UserPartID, const char *pFileName, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 SvcNvm_ReadRomFileCrc(UINT32 UserPartID, const char *pFileName, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut);

UINT32 SvcNvm_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pInfo);
UINT32 SvcNvm_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);
UINT32 SvcNvm_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);

UINT32 SvcNvm_ReadSysPartitionTable(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
UINT32 SvcNvm_WriteSysPartitionTable(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
UINT32 SvcNvm_ReadUserPartitionTable(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);
UINT32 SvcNvm_WriteUserPartitionTable(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);

UINT32 SvcNvm_ReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo);
UINT32 SvcNvm_ReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 SvcNvm_WritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 SvcNvm_ErasePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);

#endif  /* SVC_NVM_H */
