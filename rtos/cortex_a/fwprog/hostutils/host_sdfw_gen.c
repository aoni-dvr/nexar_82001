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

//typedef unsigned long long    UINT64;
//typedef unsigned int    UINT32;
//typedef unsigned short  UINT16;
//typedef unsigned char   UINT8;

#include "AmbaTypes.h"
#include "AmbaNVM_Partition.h"
#include "autoconf.h"
#include "custom.h"
#define FILE_NAME_LENGTH    (256)

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

static char PloadRegionInfoPath[256];
static AMBA_PLOAD_PARTITION_s g_PloadInfo;

static char StrSysPart[]  = "_SysPartConfig";
static char StrUserPart[] = "_UserPartConfig";
static char StrObjDump[]  = "amba_bld.map";
static char StrBinFile[]  = "amba_bld.bin";

static char StrObjDump64[]  = "amba_bld64.sym";
static char StrBinFile64[]  = "amba_bld64.bin";
/*-----------------------------------------------------------------------------------------------*\
 * This is the header for a flash image partition.
 *-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_FIRMWARE_HEADER_s_ {
    char    ModelName[32];   /* model name */

    UINT32  PtbMagic;

    UINT32  CRC32;          /* CRC32 of entire Binary File: AmbaCamera.bin */

    struct {
        UINT32   Size;
        UINT32   Crc32;
    } AmbaFwInfo[AMBA_NUM_USER_PARTITION];

    AMBA_PARTITION_CONFIG_s  SysPartitionInfo[AMBA_NUM_SYS_PARTITION];
    AMBA_PARTITION_CONFIG_s  UserPartitionInfo[AMBA_NUM_USER_PARTITION];
    AMBA_PLOAD_PARTITION_s   PloadInfo;
} AMBA_FIRMWARE_HEADER_s;

/* ---------------------------------------------------------------------------*/

typedef struct _AMBA_FW_FILE_s_ {
    char    FileName[32];
    UINT32  ImgLen;
} AMBA_FW_FILE_s;

AMBA_FW_FILE_s SysFwFile[AMBA_NUM_SYS_PARTITION];
AMBA_FW_FILE_s UserFwFile[AMBA_NUM_USER_PARTITION];

AMBA_PARTITION_CONFIG_s  SysPartitionInfo[AMBA_NUM_SYS_PARTITION];
AMBA_PARTITION_CONFIG_s  UserPartitionInfo[AMBA_NUM_USER_PARTITION];

static int GetPartNum(char *FileName, UINT32 PartFlag)
{
    if (PartFlag) {
        if (!strncmp(FileName,      "sys.bin", 7))          return AMBA_USER_PARTITION_SYS_SOFTWARE;
        else if (!strncmp(FileName, "dsp.bin", 10))      return AMBA_USER_PARTITION_DSP_uCODE;
        else if (!strncmp(FileName, "rom.bin", 10))      return AMBA_USER_PARTITION_SYS_DATA;
        else if (!strncmp(FileName, "r52sys.bin", 10))      return AMBA_USER_PARTITION_R52SYS;
        else if (!strncmp(FileName, "lnx.bin", 7))          return AMBA_USER_PARTITION_LINUX_KERNEL;
        else if (!strncmp(FileName, "rfs.bin", 7))          return AMBA_USER_PARTITION_LINUX_ROOT_FS;
#ifndef CONFIG_LINUX
        else if (!strncmp(FileName, "dtb.bin", 7))          return AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG;
#endif
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
#if 0
void DumpHeader(AMBA_FIRMWARE_HEADER_s *pUserFwHeader)
{
    int i;

    if (pUserFwHeader->ModelName != NULL && (*pUserFwHeader->ModelName) != '\0')
        printf("\r\nModelName %s\r\n", pUserFwHeader->ModelName);

    printf("CRC32 0x%x\r\n",       pUserFwHeader->CRC32);

    for (i = 0; i < AMBA_NUM_FIRMWARE_IMAGE; i++) {
        printf("Part %d Crc32 = 0x%x\r\n",   i, pUserFwHeader->AmbaFwInfo[i].Crc32);
        printf("Part %d Size =  %d\r\n\r\n", i, pUserFwHeader->AmbaFwInfo[i].Size);
    }

    for (i = 0; i < HAS_IMG_PARTS; i++) {
        printf("PartitionSize %d = 0x%x\r\n", i, pUserFwHeader->PartitionSize[i]);
    }
}
#endif

static int GenerateSdFirmware(AMBA_FW_FILE_s *pFwFile, char *OutfileName,
                              int StartPartNum, int TotalPartNum)
{
    AMBA_FIRMWARE_HEADER_s FwHeader;
    unsigned int crc = ~0U;
    int i;
    int empty = 0;

    if (strncmp(OutfileName + strlen(OutfileName) - strlen(".empty"), ".empty", strlen(".empty")) == 0) {
        empty = 1;
        printf("\r\n empty file: %s\r\n", OutfileName);
    }

    /* Generate multiple Firmware File. */
    FILE *fout = fopen(OutfileName, "wb");
    if (fout == NULL) {
        fprintf(stderr, "\nunable to open '%s' for output!\n", OutfileName);
        return 0;
    }

    memset((void *)&FwHeader, 0x0, sizeof(AMBA_FIRMWARE_HEADER_s));
    fseek(fout, sizeof(FwHeader), SEEK_SET);

    crc = ~0U;
    for (i = StartPartNum; i < (StartPartNum + TotalPartNum); i++) {
        static char buf[0x40000];
        char *p;
        size_t len, flen = 0;
        FILE *fin;

        if (empty || pFwFile[i].ImgLen == 0)
            continue;

        if ((fin = fopen(pFwFile[i].FileName, "rb")) == NULL) {
            fprintf(stderr, "\nopen %d '%s' for read fail!\n", i, pFwFile[i].FileName);
            continue;
        }

        while ((len = fread(buf, 1, sizeof(buf), fin)) > 0) {
            fwrite(buf, 1, len, fout);
            flen += len;

            p = buf;
            while (len--) {
                crc = crc32_tab[(crc ^ *p++) & 0xff] ^ (crc >> 8);
            }
        }
        fclose(fin);

        if(i == AMBA_USER_PARTITION_LINUX_HIBERNATION_IMG) {
            FwHeader.AmbaFwInfo[i].Size = 0;
        } else
            FwHeader.AmbaFwInfo[i].Size = flen;
        FwHeader.AmbaFwInfo[i].Crc32 = crc;
    }
    crc ^= ~0U;

    fseek(fout, 0, SEEK_SET);

#ifdef PROJECT_MODEL
    strcpy(FwHeader.ModelName, PROJECT_MODEL);
#endif
    FwHeader.CRC32 = crc;
    FwHeader.PtbMagic = AMBA_USER_PTB_MAGIC;
    memcpy(&FwHeader.PloadInfo,        &g_PloadInfo,      sizeof(g_PloadInfo));
    memcpy(FwHeader.SysPartitionInfo,  SysPartitionInfo,  sizeof(SysPartitionInfo));
    memcpy(FwHeader.UserPartitionInfo, UserPartitionInfo, sizeof(UserPartitionInfo));
    fwrite(&FwHeader, sizeof(FwHeader), 1, fout);
    /* DumpHeader(&FwHeader); */

    fclose(fout);
    return 0;
}

static int GetOutputFileName(AMBA_FW_FILE_s *pFwFile, char *OutfileName,
                             int StartPartNum, int EndPartNum, UINT32 PartFlag)
{
    int i, count = 0;
    UINT32 IsFirstPart = 1;

    for (i = StartPartNum; i <= EndPartNum; i++) {

        if ((pFwFile[i].ImgLen == 0))
            continue;

        if ((i != StartPartNum) && (IsFirstPart == 0U)) {
            strcat(OutfileName, "_");
        }

        IsFirstPart = 0;

        if (PartFlag)
            strcat(OutfileName, UserPartName[i]);
        else
            strcat(OutfileName, SysPartName[i]);
        count += 1;
    }
    strcat(OutfileName, ".devfw");
#if 0
    if (count > 0)
        fprintf(stderr, "  GEN     %s\r\n", OutfileName);
#endif
    return count;
}

char buf[0x40000];

static int GetPartSize(int argc, char **argv, UINT32 PartFlag)
{
    FILE *finfo = NULL;
    FILE *fin = NULL;
    char *p, *pPartStr;
    UINT32 Offset = 0;
    int i, ReadSize;
    UINT32 Temp;
    UINT32 Attr, ByteCount;
    char BldPath[FILE_NAME_LENGTH];

    if (chdir(argv[3]) < 0)
        return -1;

    pPartStr = (PartFlag == 0) ? StrSysPart : StrUserPart;

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

        p = strstr(buf, pPartStr); /* find "AmbaXXX_SysPartConfig" or "AmbaXXX_UserPartConfig" in map file */
        if (p == NULL) {
            continue; /* find not found */
        }
        fgets(buf, sizeof(buf), finfo);  /* set to next line to find offset */
        p = strtok(buf,  "");

        if (strtoul(p, NULL, 16) == 0) {
            continue;  /* offset is 0, find next symbol */
        }

        Offset = strtoul(p, NULL, 16) - FIRM_MEM_ADDR;

        /* fprintf(stderr, "read partition info from offset 0x%x\n", Offset); */
        break;
    }

    if (Offset == 0) {
        fprintf(stderr, "can not find partition size info\n");
        fclose(finfo);
        fclose(fin);
        return -1;
    }

    fseek(fin, Offset, SEEK_SET);
    if (PartFlag == 0) {
#if 0
        ReadSize = sizeof(SysPartitionInfo);
        if (ReadSize != fread((void *)&SysPartitionInfo, 1, sizeof(SysPartitionInfo), fin)) {
            fprintf(stderr, "fread SysPartitionInfo fail!\n");
        }
#else
        for(i = 0; i < AMBA_NUM_SYS_PARTITION; i++) {
            ReadSize = fread((void *)&SysPartitionInfo[i].PartitionName, 1, sizeof(SysPartitionInfo[i].PartitionName), fin);
            if (ReadSize != sizeof(SysPartitionInfo[i].PartitionName))
                fprintf(stderr, "fread SysPartitionInfo fail!\n");
            ReadSize = fread(&Temp, 1, sizeof(Temp), fin);
            ReadSize = fread(&Attr, 1, sizeof(Attr), fin);
            if (ReadSize != sizeof(Attr))
                fprintf(stderr, "fread SysPartitionInfo fail!\n");
            SysPartitionInfo[i].Attribute = Attr;
            ReadSize = fread(&ByteCount, 1, sizeof(ByteCount), fin);
            if (ReadSize != sizeof(ByteCount))
                fprintf(stderr, "fread SysPartitionInfo fail!\n");
            SysPartitionInfo[i].ByteCount = ByteCount;
        }
#endif
    } else {
#if 0
        ReadSize = sizeof(UserPartitionInfo);
        if (ReadSize != fread((void *)&UserPartitionInfo, 1, sizeof(UserPartitionInfo), fin)) {
            fprintf(stderr, "fread UserPartitionInfo fail!\n");
        }
#else
        for(i = 0; i < AMBA_NUM_USER_PARTITION; i++) {
            ReadSize = fread((void *)&UserPartitionInfo[i].PartitionName, 1, sizeof(UserPartitionInfo[i].PartitionName), fin);
            if (ReadSize != sizeof(UserPartitionInfo[i].PartitionName))
                fprintf(stderr, "fread UserPartitionInfo fail!\n");
            ReadSize = fread(&Temp, 1, sizeof(Temp), fin);
            ReadSize = fread(&Attr, 1, sizeof(Attr), fin);
            if (ReadSize != sizeof(ByteCount))
                fprintf(stderr, "fread UserPartitionInfo fail!\n");
            UserPartitionInfo[i].Attribute = Attr;
            ReadSize = fread(&ByteCount, 1, sizeof(ByteCount), fin);
            if (ReadSize != sizeof(ByteCount))
                fprintf(stderr, "fread UserPartitionInfo fail!\n");
            UserPartitionInfo[i].ByteCount = ByteCount;
        }

#endif
    }

    fclose(finfo);
    fclose(fin);

#if 0
    {
        UINT32 PartNum = (PartFlag == 0) ? AMBA_NUM_SYS_PARTITION : AMBA_NUM_USER_PARTITION;
        for (i = 0; i < PartNum; i++) {
            fprintf(stderr, "0x%x\n", pFwInfo[i].ByteCount);
        }

        fprintf(stderr, "\n");
    }
#endif
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

static void GetPloadRegionInfo(char *buf)
{
    char RegionName[32];
    UINT32 Value, Value2;

    if (buf == NULL)
        return;

    sscanf(buf, "%s 0x%x", RegionName, &Value);
    if (!strcmp(RegionName, "REGION0_RO_BASE")) {
        g_PloadInfo.RegionRoStart[0] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION0_RW_BASE")) {
        g_PloadInfo.RegionRwStart[0] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION1_RO_BASE")) {
        g_PloadInfo.RegionRoStart[1] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION1_RW_BASE")) {
        g_PloadInfo.RegionRwStart[1] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION2_RO_BASE")) {
        g_PloadInfo.RegionRoStart[2] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION2_RW_BASE")) {
        g_PloadInfo.RegionRwStart[2] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION3_RO_BASE")) {
        g_PloadInfo.RegionRoStart[3] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION3_RW_BASE")) {
        g_PloadInfo.RegionRwStart[3] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION4_RO_BASE")) {
        g_PloadInfo.RegionRoStart[4] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION4_RW_BASE")) {
        g_PloadInfo.RegionRwStart[4] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION5_RO_BASE")) {
        g_PloadInfo.RegionRoStart[5] = Value;
        return;
    } else if (!strcmp(RegionName, "REGION5_RW_BASE")) {
        g_PloadInfo.RegionRwStart[5] = Value;
        return;
    } else if (!strcmp(RegionName, "DSP_BUF_BASE")) {
        g_PloadInfo.DspBufStart = Value;
        return;
    } else if (!strcmp(RegionName, "LINKER_STUB_BASE")) {
        g_PloadInfo.LinkerStubStart = Value;
        return;
    }  else if (!strcmp(RegionName, "LINKER_STUB_LENGTH")) {
        g_PloadInfo.LinkerStubSize = Value;
        return;
    }

    sscanf(buf, "%s (0x%x - 0x%x)", RegionName, &Value, &Value2);
    if (!strcmp(RegionName, "REGION0_RO_LENGTH")) {
        g_PloadInfo.RegionRoSize[0] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION0_RW_LENGTH")) {
        g_PloadInfo.RegionRwSize[0] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION1_RO_LENGTH")) {
        g_PloadInfo.RegionRoSize[1] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION1_RW_LENGTH")) {
        g_PloadInfo.RegionRwSize[1] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION2_RO_LENGTH")) {
        g_PloadInfo.RegionRoSize[2] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION2_RW_LENGTH")) {
        g_PloadInfo.RegionRwSize[2] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION3_RO_LENGTH")) {
        g_PloadInfo.RegionRoSize[3] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION3_RW_LENGTH")) {
        g_PloadInfo.RegionRwSize[3] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION4_RO_LENGTH")) {
        g_PloadInfo.RegionRoSize[4] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION4_RW_LENGTH")) {
        g_PloadInfo.RegionRwSize[4] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION5_RO_LENGTH")) {
        g_PloadInfo.RegionRoSize[5] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "REGION5_RW_LENGTH")) {
        g_PloadInfo.RegionRwSize[5] = (Value - Value2);
        return;
    } else if (!strcmp(RegionName, "DSP_BUF_LENGTH")) {
        g_PloadInfo.DspBufSize = (Value - Value2);
        return;
    }

    return;
}

int main(int argc, char **argv)
{
    int i, Rval;
    char OutfileName[FILE_NAME_LENGTH], OutfileNameFullPath[FILE_NAME_LENGTH];

    memset(SysFwFile, 0x0, sizeof(SysFwFile));
    memset(UserFwFile, 0x0, sizeof(UserFwFile));

    if (argc < 2) {
        fprintf(stderr, "useage: host_sdfw_gen [input path]  [output path] [objdump info path]\r\n");
        return -1;
    }

    GetPartSize(argc, argv, 0);
    GetPartSize(argc, argv, 1);

    if (chdir(argv[1]) < 0)
        return -1;

    GetFwFileName(argc, argv, SysFwFile,  0);
    GetFwFileName(argc, argv, UserFwFile, 1);

    /* Get PloadInfo from list */
    if (argv[4] != NULL) {
        FILE *fout = NULL;
        static char buf[64];

        sprintf(PloadRegionInfoPath, "%s%s", argv[4], "AmbaFwLoader_RegionInfo.list");

        /* fprintf(stderr, " --------- %s----------\n", PloadRegionInfoPath); */
        fout = fopen(PloadRegionInfoPath, "r");
        if (fout != NULL) {
            while(fgets(buf, 64, fout) != NULL) {
                /* fprintf(stderr, "%s", buf); */
                GetPloadRegionInfo(buf);
            }
            fclose(fout);
        } else {
            fprintf(stderr, " Can not open: %s \r\n", PloadRegionInfoPath);
            return -1;
        }
    }

    /* Single IMAGE file */
    for (i = AMBA_SYS_PARTITION_BOOTSTRAP; i < AMBA_NUM_SYS_PARTITION; i++) {

        memset(OutfileName, 0x0, sizeof(OutfileName));
        memset(OutfileNameFullPath, 0x0, sizeof(OutfileNameFullPath));

        if (0 == GetOutputFileName(SysFwFile, OutfileName, i, i, 0))
            continue;

        sprintf(OutfileNameFullPath, "%s%s", argv[2], OutfileName);
        fprintf(stderr, "  Gen     %s\r\n", OutfileNameFullPath);
        Rval = GenerateSdFirmware(SysFwFile, OutfileNameFullPath, i, 1);
        if (Rval < 0)
            return -1;
    }

    for (i = AMBA_USER_PARTITION_SYS_SOFTWARE; i < AMBA_NUM_USER_PARTITION; i++) {
        memset(OutfileName, 0x0, sizeof(OutfileName));
        memset(OutfileNameFullPath, 0x0, sizeof(OutfileNameFullPath));

        if (0 == GetOutputFileName(UserFwFile, OutfileName, i, i, 1))
            continue;

        sprintf(OutfileNameFullPath, "%s%s", argv[2], OutfileName);
        fprintf(stderr, "  Gen     %s\r\n", OutfileNameFullPath);
        Rval = GenerateSdFirmware(UserFwFile, OutfileNameFullPath, i, 1);
        if (Rval < 0)
            return -1;
    }

    /* ----------------------- User partition firmware Multiple ------------- */
    memset(OutfileName, 0x0, sizeof(OutfileName));
    memset(OutfileNameFullPath, 0x0, sizeof(OutfileNameFullPath));
    if (0 == GetOutputFileName(UserFwFile, OutfileName, AMBA_USER_PARTITION_SYS_SOFTWARE, AMBA_NUM_USER_PARTITION -1, 1))
        return -1;

    sprintf(OutfileNameFullPath, "%s%s", argv[2], OutfileName);
    fprintf(stderr, "  Gen     %s\r\n", OutfileNameFullPath);
    Rval = GenerateSdFirmware(UserFwFile, OutfileNameFullPath, AMBA_USER_PARTITION_SYS_SOFTWARE, AMBA_NUM_USER_PARTITION - 1);
    if (Rval < 0)
        return -1;

    /* ----------------------- Sys partition firmware Multiple -------------- */
    memset(OutfileName, 0x0, sizeof(OutfileName));
    memset(OutfileNameFullPath, 0x0, sizeof(OutfileNameFullPath));
    if (0 == GetOutputFileName(SysFwFile, OutfileName, AMBA_SYS_PARTITION_BOOTSTRAP, AMBA_SYS_PARTITION_FW_UPDATER, 0))
        return -1;

    sprintf(OutfileNameFullPath, "%s%s", argv[2], OutfileName);
    fprintf(stderr, "  Gen     %s\r\n", OutfileNameFullPath);
    Rval = GenerateSdFirmware(SysFwFile, OutfileNameFullPath, AMBA_SYS_PARTITION_BOOTSTRAP, AMBA_NUM_SYS_PARTITION);
    if (Rval < 0)
        return -1;
    sprintf(OutfileNameFullPath, "%s%s.empty", argv[2], OutfileName);
    fprintf(stderr, "  Gen     %s\r\n", OutfileNameFullPath);
    Rval = GenerateSdFirmware(SysFwFile, OutfileNameFullPath, AMBA_SYS_PARTITION_BOOTSTRAP, AMBA_NUM_SYS_PARTITION);
    if (Rval < 0)
        return -1;

    return 0;
}

