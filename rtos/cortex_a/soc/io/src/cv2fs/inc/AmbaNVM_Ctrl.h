/**
 *  @file AmbaNVM_Ctrl.h
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
 *  @details Definitions & Constants for ROM APIs (Internal Use)
 *
 */

#ifndef AMBA_NVM_CTRL_H
#define AMBA_NVM_CTRL_H

//typedef INT32 (*AMBA_NVM_LOAD_ROM_FILE_TABLE_f)(void);
typedef UINT32 (*AMBA_NVM_GET_ROM_FILE_SIZE_f)(UINT32 RegionID, const char *pFileName, UINT32 *pFileSize);
typedef UINT32 (*AMBA_NVM_GET_ROM_FILE_INFO_f)(UINT32 RegionID, UINT32 Index, AMBA_NVM_ROM_FILE_INFO_s *pRomFileInfo);
typedef UINT32 (*AMBA_NVM_READ_ROM_FILE_f)(UINT32 RegionID, const char *pFileName, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut);

typedef UINT32 (*AMBA_NVM_GET_FTL_INFO_f)(UINT32 UserPartID, AMBA_NVM_FTL_INFO_s *pFtlInfo);
typedef UINT32 (*AMBA_NVM_READ_SECTOR_f)(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_WRITE_SECTOR_f)(UINT32 UserPartID, const AMBA_NVM_SECTOR_CONFIG_s *pSecConfig, UINT32 TimeOut);

typedef UINT32 (*AMBA_NVM_READ_SYS_PARTITION_TABLE_f)(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_WRITE_SYS_PARTITION_TABLE_f)(AMBA_SYS_PARTITION_TABLE_s *pSysPartTable, UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_READ_USER_PARTITION_TABLE_f)(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_WRITE_USER_PARTITION_TABLE_f)(AMBA_USER_PARTITION_TABLE_s *pUserPartTable, UINT32 UserPtbNo, UINT32 TimeOut);

typedef UINT32 (*AMBA_NVM_READ_PARTITION_INFO_f)(UINT32 PartFlag, UINT32 PartID, AMBA_PARTITION_ENTRY_s *pPartInfo);
typedef UINT32 (*AMBA_NVM_READ_PARTITION_f)(UINT32 PartFlag, UINT32 PartID, UINT8 *pDataBuf, UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_WRITE_PARTITION_f)(UINT32 PartFlag, UINT32 PartID, UINT32 DataSize, UINT8 *pDataBuf, UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_INVALIDATE_PARTITION_f)(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_ERASE_PARTITION_f)(UINT32 PartFlag, UINT32 PartID, UINT32 TimeOut);

typedef UINT32 (*AMBA_NVM_WRITE_PHY_BLOCK_f)(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_READ_PHY_BLOCK_f)(UINT32 BlkAddr, UINT8 *pDataBuf, UINT32 TimeOut);

typedef UINT32 (*AMBA_NVM_IS_BLD_MAGIC_CODE_SET_f)(void);
typedef UINT32 (*AMBA_NVM_SET_BLD_MAGIC_CODE_f)(UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_ERASE_BLD_MAGIC_CODE_f)(UINT32 TimeOut);
typedef UINT32 (*AMBA_NVM_READ_BLD_MAGIC_CODE_f)(void);

typedef struct {
    AMBA_NVM_GET_ROM_FILE_INFO_f            GetRomFileInfo;
    AMBA_NVM_GET_ROM_FILE_SIZE_f            GetRomFileSize;     /* pointer to get ROM file size function */
    AMBA_NVM_READ_ROM_FILE_f                ReadRomFile;        /* pointer to read ROM file function */

    AMBA_NVM_GET_FTL_INFO_f                 GetFtlInfo;         /* pointer to get NVM FTL information function */
    AMBA_NVM_READ_SECTOR_f                  ReadSector;
    AMBA_NVM_WRITE_SECTOR_f                 WriteSector;

    AMBA_NVM_READ_SYS_PARTITION_TABLE_f     ReadSysPartitionTable;
    AMBA_NVM_WRITE_SYS_PARTITION_TABLE_f    WriteSysPartitionTabe;
    AMBA_NVM_READ_USER_PARTITION_TABLE_f    ReadUserPartitionTable;
    AMBA_NVM_WRITE_USER_PARTITION_TABLE_f   WriteUserPartitionTable;

    AMBA_NVM_READ_PARTITION_INFO_f          ReadPartitionInfo;
    AMBA_NVM_READ_PARTITION_f               ReadPartition;
    AMBA_NVM_WRITE_PARTITION_f              WritePartition;
    AMBA_NVM_INVALIDATE_PARTITION_f         InvalidatePartition;
    AMBA_NVM_ERASE_PARTITION_f              ErasePartition;

    AMBA_NVM_WRITE_PHY_BLOCK_f              WritePhyBlock;
    AMBA_NVM_READ_PHY_BLOCK_f               ReadPhyBlock;
} AMBA_NVM_CTRL_s;

typedef struct {
    UINT8   FileName[64];                       /* ROM file name */
    UINT32  ByteSize;                           /* ROM file size in Bytes */
    UINT32  FileOffset;                         /* file offset */
    UINT32  FileCRC32;                          /* CRC32 of the File */
} AMBA_NVM_ROM_FILE_ENTRY_s;

#define AMBA_NVM_MAX_NUM_DSP_uCODE_ROM_FILE (160U)  /* maximum number of ROM files for DSP uCode */

typedef struct {
    UINT32  Version;                            /* Version of the ROM File Table */
    UINT32  FileCount;                          /* number of ROM files in the Partition  */
    AMBA_NVM_ROM_FILE_ENTRY_s  FileEntry[AMBA_NVM_MAX_NUM_DSP_uCODE_ROM_FILE];
} AMBA_NVM_ROM_DSP_uCODE_HEADER_s;

#define DSP_CTRL_PAD_SIZE (512U - (sizeof(AMBA_NVM_ROM_DSP_uCODE_HEADER_s) % 512U))

typedef struct {
    AMBA_NVM_ROM_DSP_uCODE_HEADER_s Header;
    UINT8 Reserved[DSP_CTRL_PAD_SIZE];
} AMBA_NVM_ROM_DSP_uCODE_CTRL_s;

#define AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE  (512U)  /* maximum number of ROM files for System Data */

typedef struct {
    UINT32  Version;                            /* Version of the ROM File Table */
    UINT32  FileCount;                          /* number of ROM files in the Partition  */
    AMBA_NVM_ROM_FILE_ENTRY_s  FileEntry[AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE];
} AMBA_NVM_ROM_SYS_DATA_HEADER_s;

#define SYSDATA_CTRL_PAD_SIZE (512U - (sizeof(AMBA_NVM_ROM_SYS_DATA_HEADER_s) % 512U))

typedef struct {
    AMBA_NVM_ROM_SYS_DATA_HEADER_s Header;
    UINT8 Reserved[SYSDATA_CTRL_PAD_SIZE];
} AMBA_NVM_ROM_SYS_DATA_CTRL_s;

typedef struct {
    UINT8 IsInit;
    UINT8 *pRomUnzipBuf;
    UINT8 *pRomLoadBuf;
} AMBA_NVM_ROM_MEMORY_CTRL_s;

#endif /* AMBA_NVM_CTRL_H */
