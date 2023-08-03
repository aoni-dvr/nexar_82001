/**
 *  @file AmbaShell_FsThruput.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Shell cmd "thruput" functions
 *
 */

#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"
#ifndef AMBA_CACHE_DEF_H
#include "AmbaCache_Def.h"
#endif
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaFS.h"

/* TBD */
#define PF_SEEK_SET SEEK_SET


/* FsThruput Definitions */
#define FIOPRF_THRUPUT_TIME (20000U)   /* time of test cycle */
#define THRUPUT_RESULT_SIZE (15)
#define MAX_ARG_NUM         (5U)
#define HUGE_BUFFER_SIZE    (0x100000U * 4U) /* 4MB */

static UINT8 FsTestBuf[HUGE_BUFFER_SIZE] __attribute__((aligned(AMBA_CACHE_LINE_SIZE)))  __attribute__((section(".bss.noinit")));

struct ThruputField_s {
    UINT32  Label;
    FLOAT   ReadValue;
    FLOAT   WriteValue;
};

static UINT32 TestThruputWrite(const char *pPath, const UINT32* BsMultiple, UINT32 BlockSize, struct ThruputField_s* pResult, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 StartTime, EndTime, TimeDiff;
    UINT32 ByteWritten, Rval, TestBlockSize = BlockSize;
    UINT32 FileLen = 0, FileLenMb;
    UINT32 BsIndex = 0;
    FLOAT Sec, Rate;
    AMBA_FS_FILE *pFileOut = NULL;

    UINT8 *pHughBuf = FsTestBuf;

    pResult->Label = BlockSize;

    (void)AmbaKAL_GetSysTickCount(&StartTime);

    Rval =  AmbaFS_FileOpen(pPath, "w", &pFileOut);
    if (pFileOut == NULL) {
        Rval = AmbaFS_GetError();
        PrintFunc("fopen failed (");
        AmbaShell_PrintUInt32(Rval, PrintFunc);
        PrintFunc(")\n");
    } else {
        for (TimeDiff = 0; TimeDiff <= FIOPRF_THRUPUT_TIME;) {
            if (BsMultiple[BsIndex] != 0U) {
                TestBlockSize = BsMultiple[BsIndex];
                BsIndex ++;
            } else {
                BsIndex = 0;
            }

            Rval = AmbaFS_FileWrite(pHughBuf, 1U, TestBlockSize, pFileOut, &ByteWritten);
            if (ByteWritten != TestBlockSize) {
                Rval = AmbaFS_GetError();
                PrintFunc("fwrite failed (");
                AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
                PrintFunc(")\n");
            } else if (ByteWritten == 0U) {
                INT64 Offset = 0;
                Rval = AmbaFS_FileSeek(pFileOut, Offset, AMBA_FS_SEEK_START);
                if (Rval != OK) {
                    Rval = AmbaFS_GetError();
                    PrintFunc("fseek failed (");
                    AmbaShell_PrintUInt32(Rval, PrintFunc);
                    PrintFunc(")\n");
                }
            } else {
                /* make misra happy */
            }

            if (Rval != OK) {
                break;
            }


            FileLen += ByteWritten;

            (void)AmbaKAL_GetSysTickCount(&EndTime);
            if (StartTime < EndTime) {
                TimeDiff = EndTime - StartTime;
            } else {
                TimeDiff = ((0xffffffffU - StartTime) + EndTime + 1U);
            }
        }

        Rval = AmbaFS_FileClose(pFileOut);
        if (Rval != OK) {
            Rval = AmbaFS_GetError();
            PrintFunc("fclose failed (");
            AmbaShell_PrintUInt32(Rval, PrintFunc);
            PrintFunc(")\n");
        }

        (void)AmbaKAL_GetSysTickCount(&EndTime);
        if (StartTime < EndTime) {
            TimeDiff = EndTime - StartTime;
        } else {
            TimeDiff = ((0xffffffffU - StartTime) + EndTime + 1U);
        }


        Sec = (FLOAT)TimeDiff / (FLOAT)1000.0;
        FileLenMb = FileLen / 131072U; // Mb
        Rate = (FLOAT)FileLenMb / Sec;

        PrintFunc("(BlockSize=");
        AmbaShell_PrintUInt32(TestBlockSize, PrintFunc);
        PrintFunc(") wrote ");
        AmbaShell_PrintUInt32(FileLen, PrintFunc);
        PrintFunc(" bytes\n");

        PrintFunc("(BlockSize=");
        AmbaShell_PrintUInt32(TestBlockSize, PrintFunc);
        PrintFunc(") write time elapsed : ");
        AmbaShell_PrintUInt32((UINT32)Sec, PrintFunc);
        PrintFunc(" Sec\n");

        PrintFunc("(BlockSize=");
        AmbaShell_PrintUInt32(TestBlockSize, PrintFunc);
        PrintFunc(") write throughput : ");
        AmbaShell_PrintUInt32((UINT32)Rate, PrintFunc);
        PrintFunc(" Mbit/Sec\n");

        pResult->WriteValue = Rate;
    }
    return Rval;
}

static UINT32 TestThruputRead(const char *pPath, const UINT32* BsMultiple, UINT32 BlockSize, struct ThruputField_s* pResult, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 StartTime, EndTime, TimeDiff;
    UINT32 ByteRead, Rval, TestBlockSize = BlockSize;
    UINT32 FileLen = 0, BsIndex = 0, FileLenMb;
    FLOAT Sec, Rate;
    AMBA_FS_FILE *pFileIn = NULL;

    UINT8 *pHughBuf = FsTestBuf;

    (void)AmbaKAL_GetSysTickCount(&StartTime);

    Rval = AmbaFS_FileOpen(pPath, "r", &pFileIn);
    if (pFileIn == NULL) {
        Rval = AmbaFS_GetError();
        PrintFunc("fopen failed (");
        AmbaShell_PrintUInt32(Rval, PrintFunc);
        PrintFunc(")\n");
    } else {

        for (TimeDiff = 0; TimeDiff <= FIOPRF_THRUPUT_TIME;) {
            if (BsMultiple[BsIndex] != 0U) {
                TestBlockSize = BsMultiple[BsIndex];
                BsIndex ++;
            } else {
                BsIndex = 0;
            }

            Rval = AmbaFS_FileRead(pHughBuf, 1U, TestBlockSize, pFileIn, &ByteRead);
            if (ByteRead == 0U) {
                INT64 Offset = 0;
                Rval = AmbaFS_FileSeek(pFileIn, Offset, PF_SEEK_SET);
                if (Rval != OK) {
                    Rval = AmbaFS_GetError();
                    PrintFunc("fseek failed (");
                    AmbaShell_PrintUInt32(Rval, PrintFunc);
                    PrintFunc("%d)\n");
                }
            } else if (ByteRead <= TestBlockSize) {
                /* it is normal to read smaller data */
            } else {
                Rval = AmbaFS_GetError();
                //PrintFunc("fread failed (%d), expect %d but get %d\n", Rval, BlockSize, ByteRead);
                PrintFunc("fread failed (");
                AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
                PrintFunc(")\n");
            }

            FileLen += ByteRead;

            (void)AmbaKAL_GetSysTickCount(&EndTime);
            if (StartTime < EndTime) {
                TimeDiff = EndTime - StartTime;
            } else {
                TimeDiff = ((0xffffffffU - StartTime) + EndTime + 1U);
            }
        }

        Rval = AmbaFS_FileClose(pFileIn);
        if (Rval != OK) {
            Rval = AmbaFS_GetError();
            PrintFunc("fclose failed (");
            AmbaShell_PrintUInt32(Rval, PrintFunc);
            PrintFunc("%d)\n");
        }

        (void)AmbaKAL_GetSysTickCount(&EndTime);
        if (StartTime < EndTime) {
            TimeDiff = EndTime - StartTime;
        } else {
            TimeDiff = ((0xffffffffU - StartTime) + EndTime + 1U);
        }

        Sec = (FLOAT)TimeDiff / (FLOAT)1000.0;
        FileLenMb = FileLen / 131072U; // Mb
        Rate = (FLOAT)FileLenMb / Sec;

        PrintFunc("(BlockSize=");
        AmbaShell_PrintUInt32(TestBlockSize, PrintFunc);
        PrintFunc(") read ");
        AmbaShell_PrintUInt32(FileLen, PrintFunc);
        PrintFunc(" bytes\n");

        PrintFunc("(BlockSize=");
        AmbaShell_PrintUInt32(TestBlockSize, PrintFunc);
        PrintFunc(") read time elapsed : ");
        AmbaShell_PrintUInt32((UINT32)Sec, PrintFunc);
        PrintFunc(" Sec\n");

        PrintFunc("(BlockSize=");
        AmbaShell_PrintUInt32(TestBlockSize, PrintFunc);
        PrintFunc(") read throughput : ");
        AmbaShell_PrintUInt32((UINT32)Rate, PrintFunc);
        PrintFunc(" Mbit/Sec\n");

        pResult->ReadValue = Rate;

        /*******************/
        /* Remove the file */
        /*******************/

        Rval = AmbaFS_Remove(pPath);
        if (Rval != OK) {
            Rval = AmbaFS_GetError();
            PrintFunc("fremove failed (");
            AmbaShell_PrintUInt32(Rval, PrintFunc);
            PrintFunc("%d)\n");
        }
    }
    return Rval;
}

/**
 * Test file system throughput.
 */
static void FsTestThruput(char Drive, const UINT32* BsMultiple, AMBA_SHELL_PRINT_f PrintFunc)
{
    static struct ThruputField_s ThruputResult[THRUPUT_RESULT_SIZE];
    char path[256];
    UINT32 OneShot, BlockSize, BsNum = 0, Count = 0;

    (void)AmbaWrap_memset(ThruputResult, 0, sizeof(ThruputResult));

    /* get BlockSize number */
    for (UINT32 i = 0; i < MAX_ARG_NUM; i++) {
        if (BsMultiple[i] != 0U) {
            BsNum ++;
        }
    }

    if (BsNum == 0U) {
        BlockSize = 0x1000U;
        OneShot = 0;
    } else {
        BlockSize = BsMultiple[0];
        OneShot = 1U;
    }

    for (; BlockSize <= HUGE_BUFFER_SIZE; BlockSize <<= 0x1U) {

        /* sprintf(path, "%c:\\throughput_%d.bin", Drive, BlockSize); */
        AmbaUtility_MemorySetChar(path, '\0', sizeof(path));
        (void)AmbaWrap_memcpy(path, &Drive, sizeof(Drive));
        AmbaUtility_StringAppend(path, sizeof(path), ":\\throughput_");
        AmbaUtility_StringAppendUInt32(path, sizeof(path), BlockSize, 10U);
        AmbaUtility_StringAppend(path, sizeof(path), ".bin");

        /********************/
        /* Large file write */
        /********************/
        if (BsNum < 2U) {
            PrintFunc("\n\r--- large file write with BlockSize=");
            AmbaShell_PrintUInt32(BlockSize, PrintFunc);
            PrintFunc(" ---\n");
        } else {
            PrintFunc("--- large file write with multi BlockSize ---\n");
            for (UINT32 i = 0; BsMultiple[i] > 0U; i++) {
                PrintFunc("--- BlockSize ");
                AmbaShell_PrintUInt32(i, PrintFunc);
                PrintFunc(" = ");
                AmbaShell_PrintUInt32(BsMultiple[i], PrintFunc);
                PrintFunc("---\n");
            }
        }

        (void)TestThruputWrite(path, BsMultiple, BlockSize, &ThruputResult[Count], PrintFunc);

        /*******************/
        /* Large file read */
        /*******************/
        if (BsNum < 2U) {
            PrintFunc("\n\r--- large file write with BlockSize=");
            AmbaShell_PrintUInt32(BlockSize, PrintFunc);
            PrintFunc(" ---\n");
        } else {
            PrintFunc("--- large file read with multi BlockSize ---\n");
            for (UINT32 i = 0; BsMultiple[i] > 0U; i++) {
                PrintFunc("--- BlockSize ");
                AmbaShell_PrintUInt32(i, PrintFunc);
                PrintFunc(" = ");
                AmbaShell_PrintUInt32(BsMultiple[i], PrintFunc);
                PrintFunc("---\n");
            }
        }

        (void)TestThruputRead(path, BsMultiple, BlockSize, &ThruputResult[Count], PrintFunc);

        if (OneShot != 0U) {
            break;
        }
        Count++;
    }
}

/* FsThruput Definitions */
#define DEFAULT_SIZE 20U
#define THRUPUT_TEST_ONE_SIZE (131072U)  //512 * 256
#define MEGA_DATA_SIZE (10485764U) // 1024 * 1024

static void FsTestThruputOne(char Drive, UINT32 fsize, AMBA_SHELL_PRINT_f PrintFunc)
{
    char Path[128];
    UINT32 StartTime, EndTime, TimeDiff;
    UINT32 RetVal, CluNum, Cont, n, ByteWritten;
    AMBA_FS_FILE *fp = NULL;
    UINT64 Size, NumCluster = 0U, FileLen = 0U, FileLenMb;
    UINT8 *pHughBuf = FsTestBuf;
    UINT32 Sec, Rate;

    AmbaUtility_MemorySetChar(Path, '\0', sizeof(Path));
    (void)AmbaWrap_memcpy(Path, &Drive, sizeof(Drive));
    AmbaUtility_StringAppend(Path, sizeof(Path), ":\\throughput_one.bin");

    (void)AmbaKAL_GetSysTickCount(&StartTime);

    RetVal =  AmbaFS_FileOpen(Path, "w", &fp);

    if ((fp == NULL) || (RetVal != OK)) {
        PrintFunc("fopen failed (");
        AmbaShell_PrintUInt32(RetVal, PrintFunc);
        PrintFunc(")\n");
    }

    if (RetVal == OK) {
        Size = (UINT64)fsize * MEGA_DATA_SIZE;
        RetVal = AmbaFS_ClusterAppend(fp, Size, &NumCluster);

        if (Size != NumCluster) {
            PrintFunc("Failed to create ");
            AmbaShell_PrintUInt32(fsize, PrintFunc);
            PrintFunc(" MB region, please format the card!!");
        }
    }

    if (RetVal == OK) {
        FileLen = 0U;
        CluNum = 0U;

        n = 8U * fsize;
        Cont = THRUPUT_TEST_ONE_SIZE;
        PrintFunc("Start to write data to");
        AmbaShell_PrintUInt32(n, PrintFunc);
        PrintFunc(" clusters to each file...\n");

        while (n > 0U) {
            if ((CluNum % 10U) == 0U) {
                PrintFunc("\tWriting data to cluster");
                AmbaShell_PrintUInt32(CluNum, PrintFunc);
                PrintFunc("\n");
            }

            RetVal = AmbaFS_FileWrite(pHughBuf, 1U, Cont, fp, &ByteWritten);

            if (ByteWritten != Cont) {
                PrintFunc("AmbaFS_write failed (");
                AmbaShell_PrintUInt32(ByteWritten, PrintFunc);
                PrintFunc(" != ");
                AmbaShell_PrintUInt32(Cont, PrintFunc);
                PrintFunc("\n");
                ByteWritten = 0U;
            }

            FileLen += ByteWritten;
            n--;
            CluNum++;
        }
    }


    if (RetVal == OK) {
        if (OK != AmbaFS_FileClose(fp)) {
            PrintFunc("can't close file!\n");
        }

        (void)AmbaKAL_GetSysTickCount(&EndTime);

        if (StartTime < EndTime) {
            TimeDiff = EndTime - StartTime;
        } else {
            TimeDiff = ((0xffffffffU - StartTime) + EndTime + 1U);
        }

        Sec = TimeDiff / 1000U;
        FileLenMb = FileLen / 131072U; // Mb
        Rate = ((UINT32) FileLenMb) / Sec;

        PrintFunc("write time elapsed : ");
        AmbaShell_PrintUInt32(Sec, PrintFunc);
        PrintFunc(" sec\n");

        PrintFunc("write throughput : ");
        AmbaShell_PrintUInt32(Rate, PrintFunc);
        PrintFunc(" Mbit/sec\n");

        if (OK != AmbaFS_Remove(Path)) {
            PrintFunc("can't remove file.");
        }
    }
}

static void FsTestRandWriteThruput(char Drive, UINT32 fsize, AMBA_SHELL_PRINT_f PrintFunc)
{
    char Path[128], Path1[128];
    AMBA_FS_FILE *fp, *fp1;
    UINT32 RetVal, CluNum, n, Cont, ByteWritten;
    UINT64 Size, AppendedSize = 0U, FileLen = 0U, FileLenMb;
    UINT32 StartTime, EndTime, TimeDiff;
    UINT8 *pHughBuf = FsTestBuf;
    UINT32 Sec, Rate;

    AmbaUtility_MemorySetChar(Path, '\0', sizeof(Path));
    (void)AmbaWrap_memcpy(Path, &Drive, sizeof(Drive));
    AmbaUtility_StringAppend(Path, sizeof(Path), ":\\throughput_one.bin");
    AmbaUtility_MemorySetChar(Path1, '\0', sizeof(Path1));
    (void)AmbaWrap_memcpy(Path1, &Drive, sizeof(Drive));
    AmbaUtility_StringAppend(Path1, sizeof(Path1), ":\\throughput_2ch_ch1.bin");

    (void)AmbaKAL_GetSysTickCount(&StartTime);

    RetVal =  AmbaFS_FileOpen(Path, "w", &fp);

    if (fp == NULL) {
        PrintFunc("fopen file1 failed (");
        AmbaShell_PrintUInt32(RetVal, PrintFunc);
        PrintFunc(")\n");
    }

    RetVal =  AmbaFS_FileOpen(Path1, "w", &fp1);
    if (fp == NULL) {
        PrintFunc("fopen file1 failed (");
        AmbaShell_PrintUInt32(RetVal, PrintFunc);
        PrintFunc(")\n");
    }

    if (RetVal == OK) {
        Size = (UINT64)fsize * MEGA_DATA_SIZE;

        RetVal = AmbaFS_ClusterAppend(fp, Size, &AppendedSize);

        if (AppendedSize != Size) {
            PrintFunc("Failed to create ");
            AmbaShell_PrintUInt32(fsize, PrintFunc);
            PrintFunc(" MB region, please format the card!!");
        }
    }

    if (RetVal == OK) {

        FileLen = 0U;
        CluNum = 0U;

        n = 8U * fsize;
        Cont = THRUPUT_TEST_ONE_SIZE;


        PrintFunc("Start to write data to");
        AmbaShell_PrintUInt32(n, PrintFunc);
        PrintFunc(" clusters to each file...\n");


        while (n > 0U) {
            if ((CluNum % 10U) == 0U) {
                PrintFunc("\tWriting data to cluster");
                AmbaShell_PrintUInt32(CluNum, PrintFunc);
                PrintFunc("\n");
            }

            RetVal = AmbaFS_FileWrite(pHughBuf, 1U, Cont, fp, &ByteWritten);

            if (RetVal == OK) {

                if (ByteWritten != Cont) {
                    PrintFunc("AmbaFS_write failed (");
                    AmbaShell_PrintUInt32(ByteWritten, PrintFunc);
                    PrintFunc(" != ");
                    AmbaShell_PrintUInt32(Cont, PrintFunc);
                    PrintFunc("\n");
                    ByteWritten = 0U;
                }

                FileLen += ByteWritten;

                RetVal = AmbaFS_FileWrite(pHughBuf, 1U, Cont, fp1, &ByteWritten);

                if (ByteWritten != Cont) {
                    PrintFunc("AmbaFS_write failed (");
                    AmbaShell_PrintUInt32(ByteWritten, PrintFunc);
                    PrintFunc(" != ");
                    AmbaShell_PrintUInt32(Cont, PrintFunc);
                    PrintFunc("\n");
                    ByteWritten = 0U;
                }

                n--;
                CluNum++;
            }
        }
    }



    if (RetVal == OK) {
        if (OK != AmbaFS_FileClose(fp)) {
            PrintFunc("can't close file1!\n");
        }

        if (OK != AmbaFS_FileClose(fp1)) {
            PrintFunc("can't close file2!\n");
        }

        (void)AmbaKAL_GetSysTickCount(&EndTime);

        if (StartTime < EndTime) {
            TimeDiff = EndTime - StartTime;
        } else {
            TimeDiff = ((0xffffffffU - StartTime) + EndTime + 1U);
        }

        Sec = TimeDiff / 1000U;
        FileLenMb = FileLen / 131072U; // Mb
        Rate = ((UINT32) FileLenMb) / Sec;

        PrintFunc("write time elapsed : ");
        AmbaShell_PrintUInt32(Sec, PrintFunc);
        PrintFunc(" sec\n");

        PrintFunc("write throughput : ");
        AmbaShell_PrintUInt32(Rate, PrintFunc);
        PrintFunc(" Mbit/sec\n");

        if (OK != AmbaFS_Remove(Path)) {
            PrintFunc("can't remove file.");
        }

        if (OK != AmbaFS_Remove(Path1)) {
            PrintFunc("can't remove file.");
        }

    }
}


/*********************************/
/* Export FsTestThruput(). */
/*********************************/

static void usage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [Slot] \n");

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [Slot] [BlockSize]\n");

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [Slot] one [FileSize]\n");

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [Slot] multi 2 [BlockSize1] [BlockSize2]\n");

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [Slot] rand_write\n");

    PrintFunc("Where\n");
    PrintFunc("  BlockSize is an option that means block size\n");
    PrintFunc("  Areas is an option that means number of Areas ");
    PrintFunc("in all free space\n");
}

/**
 *  AmbaShell_CommandFsThruput - execute command FsThruput
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandFsThruput(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    INT32  CmpResult = 0;
    UINT32 RetVal = 0U, BsNum;
    UINT32 i;

    if (ArgCount < 2U) {
        usage(pArgVector, PrintFunc);
    } else {

        UINT32 BsMultiple[MAX_ARG_NUM];
        char Drive = pArgVector[1][0];

        (void)AmbaWrap_memset(BsMultiple, 0, sizeof(BsMultiple));

        switch (ArgCount) {

        case 3U:
            RetVal = AmbaWrap_memcmp(pArgVector[2], "rand_write", 10U, &CmpResult);

            if ((CmpResult == 0) && (RetVal == 0U)) {
                FsTestRandWriteThruput(Drive, DEFAULT_SIZE, PrintFunc);
            } else {
                (void)AmbaUtility_StringToUInt32(pArgVector[2], &BsMultiple[0]);
                FsTestThruput(Drive, BsMultiple, PrintFunc);
            }
            break;

        case 4U:
            RetVal = AmbaWrap_memcmp(pArgVector[2], "one", 3U, &CmpResult);
            if ((RetVal == 0U) && (CmpResult == 0)) {
                UINT32 Fsize;
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &Fsize);
                if (Fsize == 0U) {
                    Fsize = DEFAULT_SIZE;
                }
                FsTestThruputOne(Drive, Fsize, PrintFunc);
            }
            break;
        case 6U:
            RetVal = AmbaWrap_memcmp(pArgVector[2], "multi", 5U, &CmpResult);

            if ((RetVal == 0U) && (CmpResult == 0)) {
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &BsNum);

                if (BsNum > MAX_ARG_NUM) {
                    PrintFunc("BsNum bigger then ");
                    AmbaShell_PrintUInt32(MAX_ARG_NUM, PrintFunc);
                }

                for (i = 0; i < BsNum; i++) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[i + 4U], &BsMultiple[i]);
                }

                FsTestThruput(Drive, BsMultiple, PrintFunc);
            }

            break;
        default:
            usage(pArgVector, PrintFunc);
            break;
        }

        PrintFunc("test_fioprf_thruput completed!\n");
    }
}


