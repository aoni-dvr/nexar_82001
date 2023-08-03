/**
*  @file SvcNvm.c
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
*  @details NVM related functions
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaSYS.h"

#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcNvm.h"

static inline void NVM_NG( const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_NG( "NVM", pFormat, Arg1, Arg2); }
static inline void NVM_OK( const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_OK( "NVM", pFormat, Arg1, Arg2); }

#define SVC_NVM_MAX_ROM_FILE_NUM   (512U)

static SVC_NVM_READ_ROM_FILE_POST_f SvcNvmReadRomFilePost = NULL;

static UINT32 GetRomFileInfo(UINT32 NvmID, UINT32 RegionID, const char *pFileName, UINT32 *pFileIdx, AMBA_NVM_ROM_FILE_INFO_s *pInfo)
{
    UINT32 RetVal = SVC_OK;

    if (pFileIdx == NULL) {
        RetVal = SVC_NG;
    } else if (pInfo == NULL) {
        RetVal = SVC_NG;
    } else if (pFileName == NULL) {
        RetVal = SVC_NG;
    } else {
        UINT32 FileIdx;
        AMBA_NVM_ROM_FILE_INFO_s RomFileInfo;

        *pFileIdx = 0xFFFFFFFFU;

        for (FileIdx = 0U; FileIdx < SVC_NVM_MAX_ROM_FILE_NUM; FileIdx ++) {
            AmbaSvcWrap_MisraMemset(&RomFileInfo, 0, sizeof(RomFileInfo));

            RetVal = AmbaNVM_GetRomFileInfo(NvmID, RegionID, FileIdx, &RomFileInfo);
            if (RetVal == NVM_ERR_NONE) {
                // TBD : if (0 == SvcWrap_strcmp(pFileName, RomFileInfo.FileName)) {
                if (0 == AmbaUtility_StringCompare(pFileName, RomFileInfo.FileName, SvcWrap_strlen(pFileName) - 1U)) {
                    *pFileIdx = FileIdx;
                    AmbaSvcWrap_MisraMemcpy(pInfo, &RomFileInfo, sizeof(AMBA_NVM_ROM_FILE_INFO_s));
                }
            }

            if ((RetVal != 0U) || (*pFileIdx == FileIdx)) {
                break;
            }
        }
    }

    return RetVal;
}

static UINT32 PartToRegionID(UINT32* RegionID, UINT32 UserPartID)
{
    UINT32 Rval = SVC_OK;

    if (AMBA_USER_PARTITION_DSP_uCODE == UserPartID) {
        *RegionID = AMBA_NVM_ROM_REGION_DSP_uCODE;
    } else if (AMBA_USER_PARTITION_SYS_DATA == UserPartID) {
        *RegionID = AMBA_NVM_ROM_REGION_SYS_DATA;
    } else {
        NVM_NG("PartToRegionID: invalid UserPartID %u", UserPartID, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

UINT32 SvcNvm_ReadRomFile(UINT32 UserPartID, const char *pFileName, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 NvmID, RegionID, Rval;
    
    Rval  = PartToRegionID(&RegionID, UserPartID);
    Rval |= AmbaSvcWrap_GetNVMID(&NvmID, 1U, UserPartID);
    if (SVC_OK == Rval) {
        Rval = AmbaNVM_ReadRomFile(NvmID, RegionID, pFileName, StartPos, Size, pDataBuf, TimeOut);
    }

    return Rval;
}

/**
* rom file read from nvm device
* @param [in] NvmID id of nvm device
* @param [in] RegionID id of rom region
* @param [in] pFileName offset of file read
* @param [in] StartPos byte position of the file
* @param [in] Size size of file read
* @param [in] pDataBuf buffer of file read
* @param [in] TimeOut timeout value
* @return 0-OK, 1-NG
*/

UINT32 SvcNvm_ReadRomFileCrc(UINT32 UserPartID, const char *pFileName, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut)
{
    #define CRC_ERR_RETRY       (5U)

    UINT32 i, HwCRC32 = 0U;
    UINT32 FileIdx = 0xFFFFFFFFU;
    UINT32 NvmID, RegionID, Rval;
    
    Rval  = PartToRegionID(&RegionID, UserPartID);
    Rval |= AmbaSvcWrap_GetNVMID(&NvmID, 1U, UserPartID);

    if (NVM_ERR_NONE == Rval) {
        Rval = AmbaNVM_ReadRomFile(NvmID, RegionID, pFileName, StartPos, Size, pDataBuf, TimeOut);
    }

    if (NVM_ERR_NONE == Rval) {
        AMBA_NVM_ROM_FILE_INFO_s RomFileInfo;

        AmbaSvcWrap_MisraMemset(&RomFileInfo, 0, sizeof(RomFileInfo));

        Rval = GetRomFileInfo(NvmID, RegionID, pFileName, &FileIdx, &RomFileInfo);
        if (Rval == 0U) {
            // If the system read full ROM file, compare file CRC and calculation CRC
            if ((StartPos == 0U) && (RomFileInfo.ByteSize == Size)) {
                // Calculate CRC by Hardware
                HwCRC32 = AmbaUtility_Crc32(pDataBuf, Size);
                if (HwCRC32 != RomFileInfo.FileCRC32) {
                    Rval = ( NVM_ERR_0000 | 0xFFFFU );

                    /* retry */
                    SVC_WRAP_PRINT "#%s# crc_mismatch, rom/hw/sw crc(0x%08X/0x%08X/0x%08X)"
                        SVC_PRN_ARG_S SVC_LOG_SYS
                        SVC_PRN_ARG_CSTR   pFileName                           SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 RomFileInfo.FileCRC32               SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 HwCRC32                             SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 AmbaUtility_Crc32Sw(pDataBuf, Size) SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E

                    for (i = 0U; i < CRC_ERR_RETRY; i++) {
                        (void)AmbaNVM_ReadRomFile(NvmID, RegionID, pFileName, StartPos, Size, pDataBuf, TimeOut);
                        HwCRC32 = AmbaUtility_Crc32(pDataBuf, Size);
                        SVC_WRAP_PRINT "  ##retry_%u rom/hw/sw crc(0x%08X/0x%08X/0x%08X)"
                            SVC_PRN_ARG_S SVC_LOG_SYS
                            SVC_PRN_ARG_UINT32 i                                   SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 RomFileInfo.FileCRC32               SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 HwCRC32                             SVC_PRN_ARG_POST
                            SVC_PRN_ARG_UINT32 AmbaUtility_Crc32Sw(pDataBuf, Size) SVC_PRN_ARG_POST
                            SVC_PRN_ARG_E

                        if (HwCRC32 == RomFileInfo.FileCRC32) {
                            Rval = NVM_ERR_NONE;
                            break;
                        }
                    }
                }
            }

            if ((SvcNvmReadRomFilePost != NULL) && (HwCRC32 == 0U)) {
                (SvcNvmReadRomFilePost)(NvmID, RegionID, FileIdx, Size, HwCRC32, pFileName);
            }
        }
    }

    return Rval;
}

/**
* callback function register of rom file read
* @param [in] pFunc callback function
* @return 0-OK, 1-NG
*/
UINT32 SvcNvm_RegisterReadRomPost(SVC_NVM_READ_ROM_FILE_POST_f pFunc)
{
    SvcNvmReadRomFilePost = pFunc;

    return SVC_OK;
}

/**
* show partition layout
* @param [in] none
* @return none
*/
void SvcNvm_ShowPtb(void)
{
    UINT32 Rval;
    AMBA_SYS_PARTITION_TABLE_s  SysTbl;
    AMBA_USER_PARTITION_TABLE_s UsrTbl;
    const AMBA_PARTITION_ENTRY_s* pPartEntry;
    UINT32 i;
    UINT32 LastBlock;
    UINT32 BackupEna;
    UINT32 NumMax = 0xFFFFFFFFU;
    
    AmbaPrint_PrintUInt5("                                               Block addr", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("______________________________________________ 0x0", 0U, 0U, 0U, 0U, 0U);
    Rval = AmbaNVM_ReadSysPartitionTable(PtbNvmID, &SysTbl, 5000U);
    if (SVC_OK == Rval) {
        LastBlock = NumMax;
        AmbaPrint_PrintStr5( "|                                          |", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintStr5( "|>> SYS PTB                                |", NULL, NULL, NULL, NULL, NULL);
        for (i = 0; i < AMBA_NUM_SYS_PARTITION; i++) {
            pPartEntry = &(SysTbl.Entry[i]);
            if ((pPartEntry->ByteCount != 0U) && (pPartEntry->Attribute != 0xFFFFFFFFU)) {
                if (LastBlock < pPartEntry->StartBlkAddr) {
                    AmbaPrint_PrintStr5( "|                                          |", NULL, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("|__________________________________________|__ 0x%8X", pPartEntry->StartBlkAddr, 0U, 0U, 0U, 0U);    
                }

                AmbaPrint_PrintStr5( "|                                          |", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintStr5( "|  %s",                                        (const char*)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("|  attr 0x%2X, block size 0x%8X        |", pPartEntry->Attribute, pPartEntry->BlkCount, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("|__________________________________________|__ 0x%8X", pPartEntry->StartBlkAddr + pPartEntry->BlkCount, 0U, 0U, 0U, 0U);
                LastBlock = pPartEntry->StartBlkAddr + pPartEntry->BlkCount;
            }
        }
    }

    Rval = AmbaNVM_ReadUserPartitionTable(PtbNvmID, &UsrTbl, 0U, 5000U);
    if (SVC_OK == Rval) {
        LastBlock = NumMax;
        AmbaPrint_PrintStr5( "|                                          |", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintStr5( "|>> USER PTB 0                             |", NULL, NULL, NULL, NULL, NULL);
        for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
            pPartEntry = &(UsrTbl.Entry[i]);
            if ((pPartEntry->ByteCount != 0U) && (pPartEntry->Attribute != 0xFFFFFFFFU)) {
                if (LastBlock < pPartEntry->StartBlkAddr) {
                    AmbaPrint_PrintStr5( "|                                          |", NULL, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("|__________________________________________|__ 0x%8X", pPartEntry->StartBlkAddr, 0U, 0U, 0U, 0U);    
                }
                AmbaPrint_PrintStr5( "|                                          |", NULL, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintStr5( "|  %s",                                        (const char*)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("|  attr 0x%2X, block size 0x%8X        |",     pPartEntry->Attribute, pPartEntry->BlkCount, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("|__________________________________________|__ 0x%8X", pPartEntry->StartBlkAddr + pPartEntry->BlkCount, 0U, 0U, 0U, 0U);
                LastBlock = pPartEntry->StartBlkAddr + pPartEntry->BlkCount;
            }
        }
    }

    Rval = AmbaNVM_ReadUserPartitionTable(PtbNvmID, &UsrTbl, 1U, 5000U);
    if (SVC_OK == Rval) {
        LastBlock = NumMax;
        AmbaPrint_PrintStr5( "|                                          |", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintStr5( "|>> USER PTB 1                             |", NULL, NULL, NULL, NULL, NULL);
        for (i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
            pPartEntry = &(UsrTbl.Entry[i]);
            if ((pPartEntry->ByteCount != 0U) && (pPartEntry->Attribute != 0xFFFFFFFFU)) {
                BackupEna = pPartEntry->Attribute & AMBA_PARTITION_ATTR_BACKUP;
                if (0U != BackupEna) {
                    if (LastBlock < pPartEntry->StartBlkAddr) {
                        AmbaPrint_PrintStr5( "|                                          |", NULL, NULL, NULL, NULL, NULL);
                        AmbaPrint_PrintUInt5("|__________________________________________|__ 0x%8X", pPartEntry->StartBlkAddr, 0U, 0U, 0U, 0U);    
                    }
                    AmbaPrint_PrintStr5( "|                                          |", NULL, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintStr5( "|  %s",                                        (const char*)pPartEntry->PartitionName, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("|  attr 0x%2X, block size 0x%8X        |", pPartEntry->Attribute, pPartEntry->BlkCount, 0U, 0U, 0U);
                    AmbaPrint_PrintStr5( "|  # backup part                           |", NULL, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("|__________________________________________|__ 0x%8X", pPartEntry->StartBlkAddr + pPartEntry->BlkCount, 0U, 0U, 0U, 0U);
                    LastBlock = pPartEntry->StartBlkAddr + pPartEntry->BlkCount;
                }
            }
        }
    }
    AmbaPrint_PrintStr5("", NULL, NULL, NULL, NULL, NULL);
}

/**
* show rom list
* @param [in] RegionID id of nvm region
* @return none
*/
void SvcNvm_ShowRomList(UINT32 RegionID)
{
    UINT32 Idx = 0U;
    UINT32 Rval, NvmID;
    char StrSize[10U];
    AMBA_NVM_ROM_FILE_INFO_s RomFileInfo;
    
    Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_USER_PARTITION_SYS_DATA);

    AmbaPrint_PrintStr5("", NULL, NULL, NULL, NULL, NULL);
    while ((SVC_OK == Rval) && (NvmID != AMBA_NVM_ID_NULL)) {
        Rval = AmbaNVM_GetRomFileInfo(NvmID, RegionID, Idx, &RomFileInfo);

        if (SVC_OK == Rval) {
            (void)SvcWrap_sprintfU32(StrSize, 10U, "%u", 1U, &RomFileInfo.ByteSize);
            AmbaPrint_PrintStr5("-- FileSize: %10s, FileName: %s", StrSize, &RomFileInfo.FileName[0], NULL, NULL, NULL);
        }

        Idx++;
    }
}

/* partition table (PTB) is the header that remember all partition info, it's in the same dev as bootstrap */
UINT32 SvcNvm_ReadSysPartitionTable(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    return AmbaNVM_ReadSysPartitionTable(PtbNvmID, pSysPartTable, TimeOut);
}

UINT32 SvcNvm_WriteSysPartitionTable(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut)
{
    return AmbaNVM_WriteSysPartitionTable(PtbNvmID, pSysPartTable, TimeOut);
}

UINT32 SvcNvm_ReadUserPartitionTable(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    return AmbaNVM_ReadUserPartitionTable(PtbNvmID, pUserPartTable, UserPtbNo, TimeOut);
}

UINT32 SvcNvm_WriteUserPartitionTable(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut)
{
    return AmbaNVM_WriteUserPartitionTable(PtbNvmID, pUserPartTable, UserPtbNo, TimeOut);
}

UINT32 SvcNvm_ReadPartitionInfo(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo)
{
    return AmbaNVM_ReadPartitionInfo(PtbNvmID, PartFlag, PartID, pPartInfo);
}

/* rom file list (header) is in the same partition as rom file data (content) */
UINT32 SvcNvm_GetRomFileInfo(UINT32 UserPartID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s * pRomFileInfo)
{
    UINT32 NvmID, RegionID, Rval;
    
    Rval  = PartToRegionID(&RegionID, UserPartID);
    Rval |= AmbaSvcWrap_GetNVMID(&NvmID, 1U, UserPartID);
    if (SVC_OK == Rval) {
        Rval = AmbaNVM_GetRomFileInfo(NvmID, RegionID, Index, pRomFileInfo);
    }

    return Rval;
}

UINT32 SvcNvm_GetRomFileSize(UINT32 UserPartID, const char *pFileName, UINT32 *pFileSize)
{
    UINT32 NvmID, RegionID, Rval;
    
    Rval  = PartToRegionID(&RegionID, UserPartID);
    Rval |= AmbaSvcWrap_GetNVMID(&NvmID, 1U, UserPartID);
    if (SVC_OK == Rval) {
        Rval = AmbaNVM_GetRomFileSize(NvmID, RegionID, pFileName, pFileSize);
    }

    return Rval;
}

UINT32 SvcNvm_GetFtlInfo(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pInfo)
{
    UINT32 NvmID;
    UINT32 Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, UserPartID);
    if (SVC_OK == Rval) {
        Rval = AmbaNVM_GetFtlInfo(NvmID, UserPartID, pInfo);
    }

    return Rval;
}

UINT32 SvcNvm_ReadSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 NvmID;
    UINT32 Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, UserPartID);
    if (SVC_OK == Rval) {
        Rval = AmbaNVM_ReadSector(NvmID, UserPartID, pSecConfig, TimeOut);
    }

    return Rval;
}

UINT32 SvcNvm_WriteSector(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut)
{
    UINT32 NvmID;
    UINT32 Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, UserPartID);
    if (SVC_OK == Rval) {
        Rval = AmbaNVM_WriteSector(NvmID, UserPartID, pSecConfig, TimeOut);
    }

    return Rval;
}

UINT32 SvcNvm_ReadPartition(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 NvmID;
    UINT32 Rval = AmbaSvcWrap_GetNVMID(&NvmID, PartFlag, PartID);
    if (SVC_OK == Rval) {
        Rval = AmbaNVM_ReadPartition(NvmID, PartFlag, PartID, pDataBuf, TimeOut);
    }

    return Rval;
}

UINT32 SvcNvm_WritePartition(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut)
{
    UINT32 NvmID;
    UINT32 Rval = AmbaSvcWrap_GetNVMID(&NvmID, PartFlag, PartID);
    if (SVC_OK == Rval) {
        Rval = AmbaNVM_WritePartition(NvmID, PartFlag, PartID, DataSize, pDataBuf, TimeOut);
    }

    return Rval;
}

UINT32 SvcNvm_ErasePartition(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut)
{
    UINT32 NvmID;
    UINT32 Rval = AmbaSvcWrap_GetNVMID(&NvmID, PartFlag, PartID);
    if (SVC_OK == Rval) {
        Rval = AmbaNVM_ErasePartition(NvmID, PartFlag, PartID, TimeOut);
    }

    return Rval;
}

