/**
 *  @file AmbaRomGen.c
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
 *  @details Ambarella ROM generator.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "AmbaTypes.h"
#include "AmbaNVM_Partition.h"
#include "AmbaNVM_Ctrl.h"

#define NG (-1)
#define ROM_META_MAGIC              0x66fc328a

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

/*-----------------------------------------------------------------------------------------------*\
 * Used by the AmbaRomGen parser and runtime.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ROM_LIST_ {
    char *pFile;
    char *pAlias;
    unsigned int Size;
    unsigned int Offset;
    unsigned int Padding;
    struct _AMBA_ROM_LIST_ *pNext;
} AMBA_ROM_LIST_s;

/*-----------------------------------------------------------------------------------------------*\
 * Used by the AmbaRomGen parser.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_ROM_PARSED_ {
    char *pTop;
    struct _AMBA_ROM_LIST_ *pList;
} AMBA_ROM_PARSED_s;

AMBA_ROM_PARSED_s g_AmbaRomParsed = {
    NULL,
    NULL,
};

char buf[0x10000];
char gFileName[0x10000];
char gAlias[0x10000];

/* Alignment to 2K is general from nand and nor */
#define ROMFS_DATA_ALIGN    4096


/*-------------------------------------------------------------------------------------------------------------------*\
 * Program usage.
\*-------------------------------------------------------------------------------------------------------------------*/
static void Usage(int Argc, char **pArgv)
{
    fprintf(stderr, "Usage: %s <infile> <outfile> <partsize>\n", pArgv[0]);
}

/*-------------------------------------------------------------------------------------------------------------------*\
 * Free allocated memory.
\*-------------------------------------------------------------------------------------------------------------------*/
static void FreeMem(void)
{
    AMBA_ROM_LIST_s *pList;
    AMBA_ROM_LIST_s *pUsed;

    for (pList = g_AmbaRomParsed.pList; pList != NULL; pList = pList->pNext) {
        if (pList->pFile)
            free(pList->pFile);
        if (pList->pAlias)
            free(pList->pAlias);
    }

    pList = g_AmbaRomParsed.pList;
    while (pList != NULL) {
        pUsed = pList;
        pList = pList->pNext;
        if (pUsed)
            free(pUsed);
    }

    if (g_AmbaRomParsed.pTop)
        free(g_AmbaRomParsed.pTop);
}

static void GetName(char *p, char *pFlieName)
{
    int Cnt = 0;
    char Temp[0x1000];

    while ((*p) != '>') {
        p ++;
    }
    p ++;

    strcpy(Temp, p);

    while ((*p) != '<') {
        p ++;
        Cnt ++;
    }

    Temp[Cnt] = '\0';

    strcpy(pFlieName, Temp);
}

static int ParseFileName(FILE *pFin, char *pLabel, char *pLabel2, char *pFlieName, char *pFlieName2)
{
    char *p;

    while (fgets(buf, sizeof(buf), pFin)) {

        p = strstr(buf, pLabel);
        if (p == NULL)
            continue; /* find not found */

        GetName(p, pFlieName);

        if (pLabel2 == NULL)
            return OK;

        p = strstr(buf, pLabel2);
        if (p == NULL)
            return OK; /* Do not have alias */

        GetName(p, pFlieName2);

        return OK;
    }
    return NG;
}

/*-------------------------------------------------------------------------------------------------------------------*\
 * Program entry point.
\*-------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    AMBA_ROM_LIST_s *pList;
    AMBA_NVM_ROM_SYS_DATA_HEADER_s Meta;
    struct stat buf;
    int Offset = 0;
    FILE *pIn = NULL, *pOut = NULL;
    char BinDat[0x20000];
    int FileCnt, Tmp;
    int i, RetVal;
    unsigned int HeaderOffset, RomSize = 0;
    FILE *pFin;
    AMBA_ROM_LIST_s *pListSearch;

    if (argc != 4) {
        Usage(argc, argv);
        return -1;
    }

#if 0
    printf("\n"
           "List File:%s\n"
           "Partition size:%s\n"
           "Output:%s\n", argv[1], argv[3], argv[2]);
#endif

    if (argv[3][1] == 'x' || argv[3][1] == 'X')
        RomSize = strtol(argv[3], NULL, 16);
    else
        RomSize = strtol(argv[3], NULL, 10);

    if (RomSize <= 0) {
        printf("error can't get %s size in info file!\n", argv[4]);
        return -4;
    }

    memset(&g_AmbaRomParsed, 0x0, sizeof(g_AmbaRomParsed));

    pFin = fopen(argv[1], "r");
    if (pFin == NULL) {
        printf("ERROR: can't open ROM list %s \r\n", argv[1]);
        return -2;
    }

    ParseFileName(pFin, "<top>", NULL, gFileName, NULL);

    g_AmbaRomParsed.pTop = malloc(strlen(gFileName) + 1);
    strcpy(g_AmbaRomParsed.pTop, gFileName);

    g_AmbaRomParsed.pList = malloc(sizeof(AMBA_ROM_LIST_s));
    pListSearch = g_AmbaRomParsed.pList;
    pListSearch->pNext = NULL;

    while (ParseFileName(pFin, "<file>", "<alias>", gFileName, gAlias) == OK) {
        if (pListSearch->pNext != NULL)
            pListSearch = pListSearch->pNext;

        pListSearch->pFile = malloc(strlen(gFileName) + 1);
        pListSearch->pAlias = malloc(strlen(gAlias) + 1);
        strcpy(pListSearch->pFile, gFileName);
        strcpy(pListSearch->pAlias, gAlias);

        pListSearch->pNext = malloc(sizeof(AMBA_ROM_LIST_s));
        memset(pListSearch->pNext, 0x0, sizeof(AMBA_ROM_LIST_s));
    }

    pListSearch->pNext = NULL;
    pListSearch = NULL;

    fclose(pFin);

    pOut = fopen(argv[2], "wb");
    if (pOut == NULL) {
        fprintf(stderr, "%s: error in opening file!\n", argv[2]);
        return -4;
    }

    RetVal = chdir(g_AmbaRomParsed.pTop);
    if (RetVal < 0) {
        perror("chdir");
        return -5;
    }

    Offset = sizeof(AMBA_NVM_ROM_SYS_DATA_HEADER_s);//ROM_META_SIZE;

    /* 1st pass: check for file existence and gather file size */
    for (pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext) {

        RetVal = stat(pList->pFile, &buf);
        if (RetVal < 0) {
            perror(pList->pFile);
            return -6;
        }

        if (!S_ISREG(buf.st_mode)) {
            fprintf(stderr, "%s: invalid file!\n", pList->pFile);
            return -7;

        }

        pList->Size = buf.st_size;
        /* Offset += sizeof(AMBA_NVM_ROM_FILE_ENTRY_s); */
    }

    /* padding ROMFS header data to 2K aligned */
    Tmp = Offset;
    Tmp %= ROMFS_DATA_ALIGN;

    if (Tmp > 0)
        Tmp = ROMFS_DATA_ALIGN - Tmp;
    else
        Tmp = 0;

    Offset += Tmp;

    HeaderOffset = Offset;

    /* 2nd pass: calculate the Offset */
    for (pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext) {
        pList->Offset = Offset;
        pList->Padding = (ROMFS_DATA_ALIGN - (pList->Size % ROMFS_DATA_ALIGN));
        Offset = Offset + pList->Size + pList->Padding;
    }

    /* 3rd pass: output ROMFS pMeta data */
    memset((void *)&Meta, 0x0, sizeof(Meta));
    FileCnt = 0;

    for (pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext) {
        FileCnt++;
    }

    /* pMeta = (AMBA_NVM_ROM_SYS_DATA_HEADER_s *) (BinDat); */
    Meta.FileCount = FileCnt;
    Meta.Version = ROM_META_MAGIC;


    /* 4rd pass: output ROMFS header data */
    for (i = 0, pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext, i++) {
        AMBA_NVM_ROM_FILE_ENTRY_s *pHeader = &Meta.FileEntry[i];
        memset(pHeader, 0x0, sizeof(AMBA_NVM_ROM_FILE_ENTRY_s));

        if (pList->pAlias)
            strncpy((char *)pHeader->FileName, (const char *)pList->pAlias, strlen(pList->pAlias));
        else
            strncpy((char *)pHeader->FileName, (const char *)pList->pFile, strlen(pList->pFile));

//        pHeader->FileName[sizeof(pHeader->FileName) - 1] = '\0';
        pHeader->FileOffset   = pList->Offset;
        pHeader->ByteSize     = pList->Size;
    }

    RetVal = fseek(pOut, HeaderOffset, 0x0);

    /* 5th pass: output ROMFS binary data */
    for (i = 0, pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext, i++) {
        AMBA_NVM_ROM_FILE_ENTRY_s *pHeader = &Meta.FileEntry[i];
        unsigned int crc = ~0U;
        int Len;
        char *p;

        pIn = fopen(pList->pFile, "rb");
        if (pIn == NULL) {
            fprintf(stderr, "%s: error in opening file!\n", pList->pFile);
            exit(2);
        }

        /* output file content */
        while ((Len = fread(BinDat, 1, sizeof(BinDat), pIn)) > 0) {
            fwrite(BinDat, 1, Len, pOut);
            if (Len < 0) {
                perror("fwrite");
                exit(4);
            }
            p = BinDat;
            while (Len--) {
                crc = crc32_tab[(crc ^ *p++) & 0xff] ^ (crc >> 8);
            }
        }
        crc ^= ~0U;
        pHeader->FileCRC32 = crc;

        fclose(pIn);

        /* output padding */
        BinDat[0] = 0x0;
        while (pList->Padding > 0) {
            if (fwrite(BinDat, 1, 1, pOut) == 0) {
                perror("fwrite");
                exit(5);
            }
            Offset += RetVal;
            pList->Padding--;
        }
    }

    RetVal = fseek(pOut, 0, 0);

    RetVal = fwrite(&Meta, 1, sizeof(Meta), pOut);
    if (RetVal < 0) {
        perror("fwrite");
        exit(1);
    }

    fclose(pOut);
    FreeMem();

    return 0;
}

