/**
 *  @file AmbaRTSL_NAND.h
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
 *  @details Definitions & Constants for NAND Controller RTSL APIs
 *
 */

#ifndef AMBA_RTSL_NAND_H
#define AMBA_RTSL_NAND_H

#ifndef AMBA_NAND_DEF_H
#include "AmbaNAND_Def.h"
#endif

/*
 * Defined in AmbaRTSL_NAND.c
 */
UINT32 AmbaRTSL_NandInit(void);

UINT32 AmbaRTSL_NandReadID(UINT32 NumReadCycle, UINT8 *pDeviceID);
UINT32 AmbaRTSL_NandConfig(AMBA_NAND_CONFIG_s *pNandConfig);
void AmbaRTSL_NandSetTiming(const AMBA_NAND_DEV_INFO_s *pNandDevInfo);

/*
 * Defined in AmbaRTSL_NAND_Partition.c
 */
UINT32 AmbaRTSL_NandInitPtbBbt(void);
UINT32 AmbaRTSL_NandReadSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable);
UINT32 AmbaRTSL_NandWriteSysPTB(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable);
UINT32 AmbaRTSL_NandReadUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo);
UINT32 AmbaRTSL_NandWriteUserPTB(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo);
UINT32 AmbaRTSL_NandGetActivePtbNo(UINT32 * pPTBActive);
UINT32 AmbaRTSL_NandSetActivePtbNo(UINT32 PtbNo, UINT8 Active);

AMBA_PARTITION_ENTRY_s *AmbaRTSL_NandGetPartEntry(UINT32 PartFlag, UINT32 PartID);
UINT32 AmbaRTSL_NandReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo);
UINT32 AmbaRTSL_NandReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf);
UINT32 AmbaRTSL_NandReadPartitionPart(UINT32 PartFlag, UINT32 PartID, UINT32 StartPos, UINT32 Size, UINT8 * pDataBuf);
UINT32 AmbaRTSL_NandWritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf);
UINT32 AmbaRTSL_NandErasePartition(UINT32 PartFlag, UINT32 PartID);

UINT32 AmbaRTSL_NandIsBldMagicCodeSet(void);
UINT32 AmbaRTSL_NandSetBldMagicCode(void);
UINT32 AmbaRTSL_NandEraseBldMagicCode(void);
UINT32 AmbaRTSL_NandReadBldMagicCode(void);
void AmbaRTSL_NandSetWritePtbFlag(UINT32 Flag);

#endif /* AMBA_RTSL_NAND_H */
