/**
 *  @file host_sdfw_gen.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Ambarella firmware generator.
 *
 */
#include <stdio.h>
#include <string.h>
#include <fts.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>
#include <stdlib.h>

#include "AmbaTypes.h"
#include "AmbaNVM_Partition.h"
#include "AmbaSpiNOR_Def.h"
#define AMBA_FLASH_FW_HOST_TOOL
#include "AmbaFlashFwProg.h"

#define EMMC_TOTAL_SECS (61071360)

const unsigned int crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

static char *SysPartName[] = {
    "bst",
    "bld",
    "pba",
    "atf",
    "ipl",
    "bld2",
};

static char *UserPartName[] = {
    "Vender",
    "sys",
    "sec",
    "rom",
    "lnx",
    "rfs",
    "dtb",
    "idx",
    "cal",
    "usr",
    "dra",
    "drb",
    "r52sys",
    "xen",
    "xtb",
    "x0k",
    "x0d",
    "dtb"
};

static char StrSysPart[]  = "_SysPartConfig";
static char StrUserPart[] = "_UserPartConfig";

static char StrDevice[] = "AmbaNORSPI_DevInfo";

static char StrObjDump[]  = "amba_bld.map";
static char StrBinFile[]  = "amba_bld.bin";

static char StrObjDump64[]  = "amba_bld64.sym";
static char StrBinFile64[]  = "amba_bld64.bin";
/* ---------------------------------------------------------------------------*/
typedef struct _AMBA_FW_FILE_s_ {
    char    FileName[32];
    UINT32  ImgLen;
} AMBA_FW_FILE_s;

AMBA_FW_FILE_s SysFwFile[AMBA_NUM_SYS_PARTITION];
AMBA_FW_FILE_s UserFwFile[AMBA_NUM_USER_PARTITION];

AMBA_PARTITION_CONFIG_s  SysPartitionInfo[AMBA_NUM_SYS_PARTITION];
AMBA_PARTITION_CONFIG_s  UserPartitionInfo[AMBA_NUM_USER_PARTITION];

AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo = {0};

UINT32 IsEMMC = 0;

AMBA_SYS_PARTITION_TABLE_s  AmbaRTSL_SysPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
AMBA_USER_PARTITION_TABLE_s AmbaRTSL_UserPartTable GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
AMBA_USER_PARTITION_TABLE_s AmbaRTSL_UserPartTableExt GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

AMBA_SYS_PARTITION_TABLE_s  *pAmbaRTSL_SysPartTable  = &AmbaRTSL_SysPartTable; /* pointer to System Partition Table */
AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_UserPartTable = &AmbaRTSL_UserPartTable; /* pointer to User Partition Table   */
AMBA_USER_PARTITION_TABLE_s *pAmbaRTSL_UserPartTableExt = &AmbaRTSL_UserPartTableExt; /* pointer to User Partition Table   */

static UINT32 Rtsl_UserPtbNextBlock = 0U;

static inline UINT32 GetRoundUpValU32   (UINT32 Val, UINT32 NumDigits)
{
    return (0x0U != NumDigits) ? (((Val) + (NumDigits) - 0x1U) / (NumDigits)) : (Val);
}

void WriteToFile(const char* pName, void *Buf, UINT32 Size)
{
    FILE *fout = fopen(pName, "wb");

    if (fout == NULL) {
        fprintf(stderr, "\nunable to open '%s' for output!\n", pName);
    }
    fwrite(Buf, 1, Size, fout);
    fclose(fout);
}

UINT32 AmbaUtility_Crc32Sw(const UINT8 *pBuffer, UINT32 Size)
{
    const UINT8 *p;
    UINT32 Crc;
    UINT32 idx;
    UINT32 count = Size;

    p = pBuffer;
    Crc = ~0U;

    while (count > 0U) {
        idx = (Crc ^ (UINT32) * p) & 0x0FFU;
        Crc = crc32_tab[idx] ^ (Crc >> 8U);
        p++;
        count--;
    }

    return Crc ^ ~0U;
}

void AmbaNorSpi_Setup_Boot_Header(AMBA_NOR_FW_HEADER_s * header)
{
    UINT32 AMBA_NOR_CMD_READ = 0x3U;

    memset(header, 0x0, sizeof(AMBA_NOR_FW_HEADER_s));

    /* SPINOR_LENGTH_REG */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    header->Img.Bits.ImageLen  = 8192U - 1U;
#else
    header->Img.Bits.ImageLen  = AMBA_NORSPI_BOOTSTRAP_CODE_SPACE_SIZE - 1U;
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    header->Flow.Bits.Clock_Div = 10;
#else
    header->Img.Bits.Clock_Div = 10;
#endif
    header->Img.Bits.DummyLen  = 0;
    header->Img.Bits.AddrLen   = 3;
    header->Img.Bits.CmdLen    = 1;

    /* SPINOR_CTRL_REG */
    header->DTR.Bits.DataReadn   = 1;
    header->DTR.Bits.DataWriteen = 0;
    header->DTR.Bits.RxLane      = 1;
    header->DTR.Bits.NumDataLane = 1;
    header->DTR.Bits.NumAddrLane = 0;
    header->DTR.Bits.NumCmdLane  = 0;
    header->DTR.Bits.DataDTR     = 0;
    header->DTR.Bits.DummyDTR    = 0;
    header->DTR.Bits.AddressDTR  = 0;
    header->DTR.Bits.CmdDTR      = 0;

    /* SPINOR_CFG_REG */
    header->Flow.Bits.RxSampleDelay = 0;
    header->Flow.Bits.ChipSelect = (~(1 << 0)) & 0xff;
    header->Flow.Bits.HoldSwitchphase = 0;
    header->Flow.Bits.Hold = 0;
    header->Flow.Bits.FlowControl = 1;

    /* SPINOR_CMD_REG */
    header->Cmd.Data = AMBA_NOR_CMD_READ;
    /* SPINOR_ADDRHI_REG */
    header->AddrHi.Data = 0x0;

    /* SPINOR_ADDRLO_REG */
    header->AddrLow.Data = 0x0 + sizeof(AMBA_NOR_FW_HEADER_s);
}

UINT32 AmbaRTSL_SpiNorCreatePTB(void)
{
    const AMBA_PARTITION_CONFIG_s *pSysPartConfig  = SysPartitionInfo;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = UserPartitionInfo;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 BlkByteSize, BlkAddr = 0, TotalNumBlk;
    UINT32 i, RetVal = OK;

    if ((pSysPartConfig == NULL) || (pUserPartConfig == NULL)) {
        RetVal = SPINOR_ERR_ARG;
    } else {

        memset(pAmbaRTSL_SysPartTable, 0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s));
        memset(pAmbaRTSL_UserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s));

        /* Block size in Byte */
        pAmbaRTSL_SysPartTable->BlkByteSize = AmbaNORSPI_DevInfo.EraseBlockSize;

        BlkByteSize = pAmbaRTSL_SysPartTable->BlkByteSize;
        if (IsEMMC == 0) {
            TotalNumBlk = AmbaNORSPI_DevInfo.TotalByteSize / BlkByteSize;
        } else {
            TotalNumBlk = AmbaNORSPI_DevInfo.TotalByteSize;
        }

        /* Block-0: Reserved for Bootstrap & System Partition Table */
        /* next Block: Reserved for User Partition Table and Vendor Data, AMBA_USER_PARTITION_PTB must be 0 !!! */

        /* Configure sys partition: BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_SysPartTable->Entry[AMBA_SYS_PARTITION_BOOTSTRAP]);
        strncpy((char *)pPartEntry->PartitionName, pSysPartConfig->PartitionName, strlen(pSysPartConfig->PartitionName) + 1);     /* Partition Name */
        pPartEntry->Attribute      = pSysPartConfig->Attribute;    /* Attribute of the Partition */
        pPartEntry->ByteCount      = pSysPartConfig->ByteCount;    /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                            /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = 0;                            /* start Block Address = 0 */

        /* Block address of User Partition Table */
        BlkAddr = pPartEntry->BlkCount + GetRoundUpValU32((UINT32)(sizeof(AMBA_SYS_PARTITION_TABLE_s) & 0xffffffffU), BlkByteSize);
        pAmbaRTSL_SysPartTable->BlkAddrUserPTB = BlkAddr;
        pAmbaRTSL_UserPartTable->PTBNumber = 0U;
        pAmbaRTSL_UserPartTable->PTBActive = 1U;
        pAmbaRTSL_UserPartTable->BlkAddrCurentPTB = BlkAddr;

        /* Configure user partition USER_PTB */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_PTB]);
        strncpy((char *)pPartEntry->PartitionName, "USER_PTB", 9U);     /* Partition Name */
        pPartEntry->Attribute      = pUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = pUserPartConfig->ByteCount;   /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                            /* actual size in Bytes */
        pPartEntry->BlkCount       = 1U;                           /* number of Blocks for the Partition: fixed to be 1  */
        pPartEntry->StartBlkAddr   = BlkAddr;                      /* start Block Address = pAmbaRTSL_SysPartTable->BlkAddrUserPTB */
        BlkAddr += pPartEntry->BlkCount;

        BlkAddr += GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), BlkByteSize);

        /* Configure System Partition Table */
        /* Configure remaining sys partitions, except the partition of BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_SysPartTable->Entry[0]);
        for (i = 1U; i < AMBA_NUM_SYS_PARTITION; i++) {
            strncpy((char *)pPartEntry[i].PartitionName, pSysPartConfig[i].PartitionName, strlen(pSysPartConfig[i].PartitionName) + 1);     /* Partition Name */
            pPartEntry[i].Attribute      = pSysPartConfig[i].Attribute;    /* Attribute of the Partition */
            pPartEntry[i].ByteCount      = pSysPartConfig[i].ByteCount;    /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0;                               /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            if (BlkAddr >= TotalNumBlk) {
                break;
            }
        }

        /* Configure User Partition Table */
        /* Configure remaining user partitions, except the partition of USER_PTB */
        pAmbaRTSL_UserPartTable->PtbMagic = AMBA_USER_PTB_MAGIC;
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_UserPartTable->Entry[0]);
        for (i = 1U; i < AMBA_NUM_USER_PARTITION; i++) {
            /* Partition Name */
            strncpy((char *)pPartEntry[i].PartitionName, pUserPartConfig[i].PartitionName, strlen(pUserPartConfig[i].PartitionName) + 1);

            pPartEntry[i].Attribute      = pUserPartConfig[i].Attribute;   /* Attribute of the Partition */
            pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;   /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0;                               /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            if (pPartEntry[i].ByteCount == 0U) {
                pPartEntry[i].StartBlkAddr = 0;
                pPartEntry[i].BlkCount = 0;
            } else {
                pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
                BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            }

            if (BlkAddr >= TotalNumBlk) {
                fprintf(stderr, "\n PTB gen error blk:%d bigger than %d \n", BlkAddr, TotalNumBlk);
                RetVal = SPINOR_ERR_NOT_ENOUGH_SPACE;
                break;
            }
        }
    }

    Rtsl_UserPtbNextBlock = BlkAddr;

    return RetVal;
}

UINT32 AmbaRTSL_SpiNorCreateExtUserPTB(UINT32 StartBlk)
{
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = UserPartitionInfo;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 BlkByteSize, BlkAddr, TotalNumBlk;
    UINT32 i, RetVal = OK;

    memcpy(pAmbaRTSL_UserPartTableExt, pAmbaRTSL_UserPartTable, sizeof(AMBA_USER_PARTITION_TABLE_s));

    if (pUserPartConfig == NULL) {
        RetVal = SPINOR_ERR_ARG;
    } else {
        /* Block size in Byte */
        pAmbaRTSL_SysPartTable->BlkByteSize = AmbaNORSPI_DevInfo.EraseBlockSize;

        BlkByteSize = pAmbaRTSL_SysPartTable->BlkByteSize;
        if (IsEMMC == 0) {
            TotalNumBlk = AmbaNORSPI_DevInfo.TotalByteSize / BlkByteSize;
        } else {
            TotalNumBlk = AmbaNORSPI_DevInfo.TotalByteSize;
        }

        pAmbaRTSL_UserPartTableExt->PTBNumber = 1U;
        pAmbaRTSL_UserPartTableExt->PTBActive = 0U;
        pAmbaRTSL_UserPartTableExt->BlkAddrCurentPTB += 1U;
        BlkAddr = StartBlk;

        /* Configure User Partition Table */
        /* Configure remaining user partitions, except the partition of VENDOR_DATA */
        pAmbaRTSL_UserPartTableExt->PtbMagic = AMBA_USER_PTB_MAGIC;
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_UserPartTableExt->Entry[0]);
        for (i = 1U; i < AMBA_NUM_USER_PARTITION; i++) {

            if ((pUserPartConfig[i].Attribute & AMBA_PARTITION_ATTR_BACKUP) == 0U) {
                continue;
            }
            pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;   /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0;                               /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            if (pPartEntry[i].ByteCount == 0U) {
                pPartEntry[i].StartBlkAddr = 0;
                pPartEntry[i].BlkCount = 0;
            } else {
                pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
                BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            }
            if (BlkAddr >= TotalNumBlk) {
                break;
            }
        }
        pAmbaRTSL_UserPartTableExt->CRC32 = AmbaUtility_Crc32Sw((const UINT8 *)pAmbaRTSL_UserPartTableExt, AMBA_USER_PTB_CRC32_SIZE);
    }

    return RetVal;
}

static INT32 AmbaRTSL_EmmcCreatePTB(void)
{
    const AMBA_PARTITION_CONFIG_s *pSysPartConfig  = SysPartitionInfo;
    const AMBA_PARTITION_CONFIG_s *pUserPartConfig = UserPartitionInfo;
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 BlkByteSize;
    INT32 RetVal = 0;
    UINT32 BlkAddr, Stg0TotalSector, i;
    UINT32 TotalNumBlk = EMMC_TOTAL_SECS;                /* Total number of blocks */
    UINT32 Rtsl_EmmcBackupBlkCount = 0U;

    if ((pSysPartConfig != NULL) && (pUserPartConfig != NULL)) {
        /* Block size in Byte */
        pAmbaRTSL_SysPartTable ->BlkByteSize = 512U;
        BlkByteSize = 512U;

        /* Block-0: Reserved for Bootstrap & System Partition Table */
        /* next Block: Reserved for User Partition Table and Vendor Data, AMBA_USER_PARTITION_VENDOR_DATA must be 0 !!! */


        /* Configure sys partition: BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_SysPartTable ->Entry[AMBA_SYS_PARTITION_BOOTSTRAP]);
        strncpy((char *)pPartEntry->PartitionName, pSysPartConfig->PartitionName, strlen(pSysPartConfig->PartitionName) + 1);     /* Partition Name */
        pPartEntry->Attribute = pSysPartConfig->Attribute;    /* Attribute of the Partition */
        pPartEntry->ByteCount      = pSysPartConfig->ByteCount;         /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundUpValU32(pPartEntry->ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = 0;                                 /* start Block Address = 0 */


        /* Block address of User Partition Table */
        BlkAddr = pPartEntry->BlkCount + GetRoundUpValU32((UINT32)(sizeof(AMBA_SYS_PARTITION_TABLE_s) & 0xffffffffU), 512);
        pAmbaRTSL_SysPartTable ->BlkAddrUserPTB = BlkAddr;

        /* Configure user partition USER_PTB */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_UserPartTable ->Entry[AMBA_USER_PARTITION_PTB]);
        strncpy((char *)pPartEntry->PartitionName, "USER_PTB", 9U);     /* Partition Name */

        pPartEntry->Attribute = pUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = pUserPartConfig->ByteCount;        /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = GetRoundUpValU32((UINT32)(sizeof(AMBA_USER_PARTITION_TABLE_s) & 0xffffffffU), 512);                                 /* number of Blocks for the Partition: fixed to be 1  */
        pPartEntry->StartBlkAddr   = BlkAddr;                           /* start Block Address = pAmbaRTSL_SysPartTable ->BlkAddrUserPTB */
        pAmbaRTSL_UserPartTable ->PTBNumber = 0;
        pAmbaRTSL_UserPartTable ->PTBActive = 1U;

        /* Configure user partition 2nd USER_PTB */
        BlkAddr += pPartEntry->BlkCount;

        BlkAddr += pPartEntry->BlkCount;  /* reserved for 2nd PTB. */

        /* Configure System Partition Table */
        /* Configure remaining sys partitions, except the partition of BOOTSTRAP */
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_SysPartTable ->Entry[0]);

        for (i = 1U; i < AMBA_NUM_SYS_PARTITION; i++) {

            strncpy((char *)pPartEntry[i].PartitionName, pSysPartConfig[i].PartitionName, strlen(pSysPartConfig[i].PartitionName) + 1);     /* Partition Name */

            pPartEntry[i].Attribute      = pSysPartConfig[i].Attribute;    /* Attribute of the Partition */
            pPartEntry[i].ByteCount      = pSysPartConfig[i].ByteCount;         /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0U;                                 /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry[i].BlkCount;                /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            if (BlkAddr >= TotalNumBlk) {
                break;
            }
        }

        /* Configure User Partition Table */
        /* Configure remaining user partitions, except the partition of VENDOR_DATA */
        pAmbaRTSL_UserPartTable ->PtbMagic = AMBA_USER_PTB_MAGIC;
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_UserPartTable ->Entry[0]);
        for (i = 1U; i < AMBA_NUM_USER_PARTITION; i++) {
            /* Partition Name */
            if (i == AMBA_USER_PARTITION_FAT_DRIVE_A) {
                continue;
            }
            //IO_UtilityStringCopy((char *)pPartEntry[i].PartitionName, sizeof(pPartEntry[i].PartitionName), pUserPartConfig[i].PartitionName);
            strncpy((char *)pPartEntry[i].PartitionName, pUserPartConfig[i].PartitionName, strlen(pUserPartConfig[i].PartitionName) + 1);


            pPartEntry[i].ProgramStatus = 0U;
            pPartEntry[i].Attribute = pUserPartConfig[i].Attribute;   /* Attribute of the Partition */
            pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;        /* number of Bytes for the Partition */
            pPartEntry[i].ActualByteSize = 0U;                                 /* actual size in Bytes */
            pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

            if (pPartEntry[i].ByteCount == 0U) {
                pPartEntry[i].StartBlkAddr = 0U;
                pPartEntry[i].BlkCount = 0U;
                continue;
            }

            if ((pUserPartConfig[i].Attribute & AMBA_PARTITION_ATTR_BACKUP) != 0U) {
                Rtsl_EmmcBackupBlkCount += pPartEntry[i].BlkCount;
            }

            pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
            BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */
            if (BlkAddr >= TotalNumBlk) {
                break;
            }
        }

        /* Configure remind space for storage0 */
        /* Put it to the end of the device.    */
        Rtsl_UserPtbNextBlock = BlkAddr;
        BlkAddr += Rtsl_EmmcBackupBlkCount;

        Stg0TotalSector = TotalNumBlk - BlkAddr;
        pUserPartConfig = &UserPartitionInfo[AMBA_USER_PARTITION_FAT_DRIVE_A];
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pAmbaRTSL_UserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);

        strncpy((char *)pPartEntry->PartitionName, pUserPartConfig->PartitionName, strlen(pUserPartConfig->PartitionName) + 1);
        pPartEntry->Attribute = pUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = (Stg0TotalSector * BlkByteSize);   /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = Stg0TotalSector;                   /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = BlkAddr;                           /* start Block Address */
    }

    pAmbaRTSL_UserPartTable->Entry[AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG].Attribute |= AMBA_PARTITION_ATTR_ERASED;

    return RetVal;
}

UINT32 AmbaRTSL_EmmcCreateExtUserPTB(const AMBA_PARTITION_CONFIG_s *pUserPartConfig, AMBA_USER_PARTITION_TABLE_s * pUserPartTable, UINT32 StartBlk)
{
    AMBA_PARTITION_ENTRY_s  *pPartEntry;
    UINT32 i;
    UINT32 TotalNumBlk = EMMC_TOTAL_SECS;
    UINT32 BlkAddr, Stg0TotalSector, BlkByteSize;
    const AMBA_PARTITION_CONFIG_s *pTmpUserPartConfig;
    UINT32 RetVal = OK;

    BlkAddr = StartBlk;
    pUserPartTable->PTBNumber = 1U;
    pUserPartTable->PTBActive = 0U;
    pUserPartTable->BlkAddrCurentPTB = StartBlk;
    pUserPartTable->BlkAddrNextPTB = 0U;

    pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pUserPartTable->Entry[0]);
    BlkByteSize = 512U;
    for (i = 1U; i < AMBA_NUM_USER_PARTITION; i++) {
        /* Partition Name */
        if (i == AMBA_USER_PARTITION_FAT_DRIVE_A) {
            continue;
        }

        if (((pUserPartConfig[i].Attribute & AMBA_PARTITION_ATTR_STORE_DEVIC) >> 6) != AMBA_NVM_eMMC) {
            continue;
        }

        if ((pUserPartConfig[i].Attribute & AMBA_PARTITION_ATTR_BACKUP) == 0U) {
            continue;
        }

        strncpy((char *)pPartEntry[i].PartitionName, pUserPartConfig[i].PartitionName, strlen(pUserPartConfig[i].PartitionName) + 1);     /* Partition Name */
        pPartEntry[i].Attribute = pUserPartConfig[i].Attribute;   /* Attribute of the Partition */
        pPartEntry[i].ByteCount      = pUserPartConfig[i].ByteCount;        /* number of Bytes for the Partition */
        pPartEntry[i].ActualByteSize = 0U;                                 /* actual size in Bytes */
        pPartEntry[i].BlkCount       = GetRoundUpValU32(pPartEntry[i].ByteCount, BlkByteSize);  /* number of Blocks for the Partition  */

        if (pPartEntry[i].ByteCount == 0U) {
            pPartEntry[i].StartBlkAddr = 0U;
            pPartEntry[i].BlkCount = 0U;
            continue;
        }

        pPartEntry[i].StartBlkAddr = BlkAddr;   /* start Block Address */
        BlkAddr += pPartEntry[i].BlkCount;      /* BlkAddr = (pPartEntry->StartBlkAddr + 1) */

        if (BlkAddr >= TotalNumBlk) {
            RetVal = -1;
            break;
        }
    }

    if (RetVal == OK) {
        /* Configure remind space for storage0 */
        /* Put it to the end of the device.    */
        Stg0TotalSector = TotalNumBlk - BlkAddr;
        pTmpUserPartConfig = & pUserPartConfig[AMBA_USER_PARTITION_FAT_DRIVE_A];
        pPartEntry = (AMBA_PARTITION_ENTRY_s  *) & (pUserPartTable->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A]);

        strncpy((char *)pPartEntry->PartitionName, pTmpUserPartConfig->PartitionName, strlen(pTmpUserPartConfig->PartitionName) + 1);     /* Partition Name */
        pPartEntry->Attribute      = pTmpUserPartConfig->Attribute;   /* Attribute of the Partition */
        pPartEntry->ByteCount      = (Stg0TotalSector * BlkByteSize);   /* number of Bytes for the Partition */
        pPartEntry->ActualByteSize = 0;                                 /* actual size in Bytes */
        pPartEntry->BlkCount       = Stg0TotalSector;                   /* number of Blocks for the Partition  */
        pPartEntry->StartBlkAddr   = BlkAddr;                           /* start Block Address */
    }

    pAmbaRTSL_UserPartTableExt->CRC32 = AmbaUtility_Crc32Sw((const UINT8 *)pAmbaRTSL_UserPartTableExt, AMBA_USER_PTB_CRC32_SIZE);

    return RetVal;
}

static INT32 AmbaRTSL_EmmcHandleCreatePtb(void)
{
    INT32 RetVal = 0;
    AMBA_PARTITION_ENTRY_s  Stg0PartEntry;

    memset(pAmbaRTSL_SysPartTable,  0xff, sizeof(AMBA_SYS_PARTITION_TABLE_s));
    memset(pAmbaRTSL_UserPartTable, 0xff, sizeof(AMBA_USER_PARTITION_TABLE_s));
    RetVal = AmbaRTSL_EmmcCreatePTB();

    memcpy(&Stg0PartEntry, &pAmbaRTSL_UserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A], sizeof(Stg0PartEntry));

    memcpy(pAmbaRTSL_UserPartTableExt, pAmbaRTSL_UserPartTable, sizeof(AMBA_USER_PARTITION_TABLE_s));

    if (RetVal == 0) {
        if (AmbaRTSL_EmmcCreateExtUserPTB(UserPartitionInfo, pAmbaRTSL_UserPartTableExt, Rtsl_UserPtbNextBlock) != 0U) {
            RetVal = -1;
        }
    }

    if ((Stg0PartEntry.StartBlkAddr != pAmbaRTSL_UserPartTableExt ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A].StartBlkAddr) ||
        (Stg0PartEntry.BlkCount     != pAmbaRTSL_UserPartTableExt ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A].BlkCount)) {

        /* Update remind space for storage0 to USER PTB0 */
        memcpy(&Stg0PartEntry, &pAmbaRTSL_UserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A], sizeof(Stg0PartEntry));


        memcpy(&pAmbaRTSL_UserPartTable ->Entry[AMBA_USER_PARTITION_FAT_DRIVE_A], &Stg0PartEntry, sizeof(Stg0PartEntry));

    }

    return RetVal;
}

static int GetPartNum(char *FileName, UINT32 PartFlag)
{
    if (PartFlag) {
        if (!strncmp(FileName,      "sys.bin", 7))          return AMBA_USER_PARTITION_SYS_SOFTWARE;
        else if (!strncmp(FileName, "dsp.bin", 10))      return AMBA_USER_PARTITION_DSP_uCODE;
        else if (!strncmp(FileName, "rom.bin", 10))      return AMBA_USER_PARTITION_SYS_DATA;
        else if (!strncmp(FileName, "r52sys.bin", 10))      return AMBA_USER_PARTITION_R52SYS;
        else if (!strncmp(FileName, "lnx.bin", 7))          return AMBA_USER_PARTITION_LINUX_KERNEL;
        else if (!strncmp(FileName, "rfs.bin", 7))          return AMBA_USER_PARTITION_LINUX_ROOT_FS;
        else if (!strncmp(FileName, "dtb.bin", 7))          return AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG;
        else if (!strncmp(FileName, "xen.bin", 7))          return AMBA_USER_PARTITION_XEN;
        else if (!strncmp(FileName, "xtb.bin", 7))          return AMBA_USER_PARTITION_XTB;
        else if (!strncmp(FileName, "x0k.bin", 7))          return AMBA_USER_PARTITION_X0K;
        else if (!strncmp(FileName, "x0d.bin", 7))          return AMBA_USER_PARTITION_X0D;
    } else {
        if (!strncmp(FileName,      "bst.bin", 7))          return AMBA_SYS_PARTITION_BOOTSTRAP;
        else if (!strncmp(FileName, "bld.bin", 7))          return AMBA_SYS_PARTITION_BOOTLOADER;
        else if (!strncmp(FileName, "pba.bin", 7))          return AMBA_SYS_PARTITION_FW_UPDATER;
        else if (!strncmp(FileName, "atf.bin", 7))          return AMBA_SYS_PARTITION_ARM_TRUST_FW;
        else if (!strncmp(FileName, "ipl.bin", 7))          return AMBA_SYS_PARTITION_QNX_IPL;
        else if (!strncmp(FileName, "bld2.bin", 8))          return AMBA_SYS_PARTITION_BLD2;
    }
    return -1;
}

static INT32 GeneratePTB(AMBA_FW_FILE_s *pFwFile, UINT32 StartPartNum, UINT32 PartFlag)
{
    INT32 Rval = 0;
    AMBA_IMG_HEADER ImgHeader;
    AMBA_PARTITION_ENTRY_s *pPartEntry;

    if (pFwFile[StartPartNum].ImgLen != 0) {
        FILE *fin = fopen(pFwFile[StartPartNum].FileName, "rb");
        fread(&ImgHeader, 1, sizeof(AMBA_IMG_HEADER), fin);
        fclose(fin);

        if (PartFlag == 0) {
            pPartEntry = &pAmbaRTSL_SysPartTable->Entry[StartPartNum];

            pPartEntry->ActualByteSize = ImgHeader.ImgLen;
            pPartEntry->RamLoadAddr    = ImgHeader.MemAddr;
            pPartEntry->ImageCRC32     = ImgHeader.Crc32;
            pPartEntry->Attribute      = SysPartitionInfo[StartPartNum].Attribute;

            pAmbaRTSL_SysPartTable->CRC32 = AmbaUtility_Crc32Sw((const UINT8 *)pAmbaRTSL_SysPartTable, (sizeof(AMBA_SYS_PARTITION_TABLE_s) - 4U));
        } else {
            pPartEntry = &pAmbaRTSL_UserPartTable->Entry[StartPartNum];

            pPartEntry->ActualByteSize = ImgHeader.ImgLen;
            pPartEntry->RamLoadAddr    = ImgHeader.MemAddr;
            pPartEntry->ImageCRC32     = ImgHeader.Crc32;
            pPartEntry->Attribute      = UserPartitionInfo[StartPartNum].Attribute;

            pAmbaRTSL_UserPartTable->CRC32 = AmbaUtility_Crc32Sw((const UINT8 *)pAmbaRTSL_UserPartTable, AMBA_USER_PTB_CRC32_SIZE);
        }
        if (pPartEntry->ActualByteSize > (pPartEntry->BlkCount * pAmbaRTSL_SysPartTable->BlkByteSize)) {
            fprintf(stderr, "\n part:'%s' bigger than partition size\n", pPartEntry->PartitionName);
            //Rval = -1;
        }
    }
    /* fprintf(stderr, "\nopen '%s' for read \n", pFwFile[StartPartNum].FileName); */
    return Rval;
}

char buf[0x40000];

static int GetDataFromMap(int argc, char **argv, void *pData, UINT32 Size, char *pPattern)
{
    FILE *finfo = NULL;
    FILE *fin = NULL;
    char *pPartStr;
    UINT32 Offset = 0;
    char BldPath[256];

    if (chdir(argv[3]) < 0)
        return -1;

    finfo = fopen(StrObjDump, "r"); /* finfo = fopen("amba_bld.sym", "r"); */
    if (finfo == NULL) {
        finfo = fopen(StrObjDump64, "r"); /* finfo = fopen("amba_bld64.sym", "r"); */
        if (finfo == NULL) {
            fprintf(stderr, "fopen %s / %s fail!\n", StrObjDump, StrObjDump64);
            return -1;
        }
    }

    sprintf(BldPath, "%s%s", argv[3], StrBinFile);

    fin = fopen(BldPath, "r"); /* fin = fopen("amba_bld.bin", "r"); */
    if (fin == NULL) {
        fin = fopen(StrBinFile64, "r"); /* fin = fopen("amba_bld64.bin", "r"); */
        if (fin == NULL) {
            fprintf(stderr, "fopen %s / %s fail!\n", StrBinFile, StrBinFile64);
            fclose(finfo);
            return -1;
        }
    }

    while (fgets(buf, sizeof(buf), finfo)) {

        char *p = strstr(buf, pPattern); /* find "AmbaXXX_SysPartConfig" or "AmbaXXX_UserPartConfig" in map file */
        if (p == NULL) {
            continue; /* find not found */
        }
        fgets(buf, sizeof(buf), finfo);  /* set to next line to find offset */
        p = strtok(buf,  "");

        if (strtoul(p, NULL, 16) == 0) {
            continue;  /* offset is 0, find next symbol */
        }

        Offset = strtoul(p, NULL, 16) - FIRM_MEM_ADDR;

        break;
    }

    if ((Offset > FIRM_MEM_ADDR) || (Offset == 0)) {
        //fprintf(stderr, "%s can not find %s info\n", __func__, pPattern);
        fclose(finfo);
        fclose(fin);
        return -1;
    }

    fseek(fin, Offset, SEEK_SET);

    if (Size != fread((void *)pData, 1, Size, fin)) {
        fprintf(stderr, "%s fread fail!\n", __func__);
    }

    fclose(finfo);
    fclose(fin);

    return 0;
}

static int GetFwFileName(int argc, char **argv, AMBA_FW_FILE_s *pFwFile, UINT32 PartFlag)
{
    char *dot[] = {".", 0};
    char **paths = dot;
    FTSENT *node;
    FTS *tree = fts_open(paths, FTS_NOCHDIR, 0);
    if (!tree) {
        perror("fts_open");
        return -1;
    }

    while ((node = fts_read(tree))) {
        if (node->fts_level > 0 && node->fts_name[0] == '.') {
            fts_set(tree, node, FTS_SKIP);
        } else if (node->fts_info & FTS_F) {
            /* Find file */
            int PartNum = GetPartNum(node->fts_name, PartFlag);
            if (PartNum < 0)
                continue;
            strcpy(pFwFile[PartNum].FileName, node->fts_name);
            pFwFile[PartNum].ImgLen = node->fts_statp->st_size;
            /* fprintf(stderr, "Found part: %d %s\r\n", PartNum, pFwFile[PartNum].FileName); */
        }
    }

    if (fts_close(tree)) {
        perror("fts_close");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    INT32 Rval;
    AMBA_NOR_FW_HEADER_s Header;
    UINT32 RetVal;

    memset(SysFwFile, 0x0, sizeof(SysFwFile));
    memset(UserFwFile, 0x0, sizeof(UserFwFile));

    if (argc < 2) {
        fprintf(stderr, "useage: host_sdfw_gen [input path]  [output path] [objdump info path]\r\n");
        return -1;
    }

    if (sizeof(AMBA_USER_PARTITION_TABLE_s) > AMBA_USER_PTB_TOTAL_SIZE) {
        fprintf(stderr, "User PTB size incorrect: %d\r\n", sizeof(AMBA_USER_PARTITION_TABLE_s));
        return -1;
    }

    GetDataFromMap(argc, argv, &SysPartitionInfo, sizeof(SysPartitionInfo), StrSysPart);
    GetDataFromMap(argc, argv, &UserPartitionInfo, sizeof(UserPartitionInfo), StrUserPart);
    Rval = GetDataFromMap(argc, argv, &AmbaNORSPI_DevInfo, sizeof(AmbaNORSPI_DevInfo), StrDevice);
    if (Rval == -1) {
        IsEMMC = 1;
    }

    if (chdir(argv[1]) < 0) {
        return -1;
    }
    GetFwFileName(argc, argv, SysFwFile,  0);
    GetFwFileName(argc, argv, UserFwFile, 1);

    if (IsEMMC == 1U) {
        RetVal = AmbaRTSL_EmmcHandleCreatePtb();
    } else {
        RetVal = AmbaRTSL_SpiNorCreatePTB();
        if (RetVal == OK) {
            RetVal = AmbaRTSL_SpiNorCreateExtUserPTB(Rtsl_UserPtbNextBlock);
        }
    }

    if (RetVal != OK) {
        return -1;
    }

    /* Sys Table */
    for (UINT32 Cnt = AMBA_SYS_PARTITION_BOOTSTRAP; Cnt < AMBA_NUM_SYS_PARTITION; Cnt++) {
        Rval = GeneratePTB(SysFwFile, Cnt, 0);
        if (Rval < 0) {
            return -1;
        }
    }

    /* User Table */
    for (UINT32 Cnt = AMBA_USER_PARTITION_SYS_SOFTWARE; Cnt < AMBA_NUM_USER_PARTITION; Cnt++) {
        Rval = GeneratePTB(UserFwFile, Cnt, 1);
        if (Rval < 0) {
            return -1;
        }
    }

    if (chdir(argv[3]) < 0) {
        return -1;
    }
    if (IsEMMC != 1U) {
        AmbaNorSpi_Setup_Boot_Header(&Header);
        WriteToFile("SPINOR_BootHeader.bin", &Header, sizeof(AMBA_NOR_FW_HEADER_s));
    }

    fprintf(stderr, "Partition table output in :%s\r\n", argv[3]);
    WriteToFile("SysPtb.bin", pAmbaRTSL_SysPartTable, sizeof(AMBA_SYS_PARTITION_TABLE_s));

    WriteToFile("UserPtb.bin", pAmbaRTSL_UserPartTable, sizeof(AMBA_USER_PARTITION_TABLE_s));

    WriteToFile("UserPtbExt.bin", pAmbaRTSL_UserPartTableExt, sizeof(AMBA_USER_PARTITION_TABLE_s));

    return 0;
}
