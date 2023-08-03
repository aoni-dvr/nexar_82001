/**
 *  @file AmbaShell_FsRandVerify.c
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
 *  @details Ambarella file system abstraction layer.
 *
 */

#include <AmbaMisraFix.h>

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaShell_Utility.h"
#include "AmbaFS.h"

UINT8 Wrbuf[0x900000];
UINT8 RdBuf[0x900000];


extern int sprintf(char *, const char *fmt, ...);

static int DataVerify(UINT8* origin, UINT8* data, UINT32 len, AMBA_SHELL_PRINT_f PrintFunc)
{
    int ErrCnt = 0;
    UINT32 i;

    for (i = 0; i < len; i++) {
        if (origin[i] != data[i]) {
            ErrCnt++;
            break;
        }
    }
    if (ErrCnt > 0) {
        PrintFunc("len=");
        AmbaShell_PrintUInt32(len, PrintFunc);
        PrintFunc(" ErrCnt=");
        AmbaShell_PrintUInt32(ErrCnt, PrintFunc);
        PrintFunc(" i = ");
        AmbaShell_PrintUInt32(i, PrintFunc);
        PrintFunc("\n");
    }

    return ErrCnt;
}

#define FIOPRF_TEST_USE_MPL 1
#define NUM_RANDVRFY        16
#define RANDVRFY_ALL        -1
#define RANDVRFY_FIXED      0

/**
 * Generate a random pattern; write to media; read it back to verify.
 */
void TestRandvrfy(char Drive, AMBA_SHELL_PRINT_f PrintFunc)
{
    INT32 i, n = 0;
    UINT32 RetVal, j;
    INT32 ErrCnt = 0;
    UINT32 FileLen, TestSize;
    AMBA_FS_FILE *pFile = NULL;
    char path[128] = "c:\\test.bin";

    AmbaMisra_TouchUnused(&Drive);
    /***************************************/
    /* Write random pattern and verify */
    /***************************************/

    for (j = 0; j < 0x900000U; j++) {
#if 1
        Wrbuf[j] = (j % 0x1000);
#else
        Wrbuf[j] = (0x1000 - (j % 0x1000));
#endif
    }
    TestSize = 0x80000;
    n = NUM_RANDVRFY;

    path[0] = Drive;

    for (i = 0; i < n; i++) {
        UINT8 *pWrbuf = Wrbuf;
        UINT8 *pRdBuf = RdBuf;

        pWrbuf = &pWrbuf[i];
        pRdBuf = &pRdBuf[i];

        AmbaFS_FileOpen(path, "wb", &pFile);
        if (pFile == NULL) {
            RetVal = AmbaFS_GetError();
            PrintFunc("fopen ");
            PrintFunc(path);
            PrintFunc(" failed\n");
            AmbaShell_PrintUInt32(RetVal, PrintFunc);
            goto done_pat;
        }

        AmbaFS_FileWrite(pWrbuf, 1, TestSize, pFile, &FileLen);
        if (FileLen != TestSize) {
            PrintFunc("[FAILED]wrote ");
            AmbaShell_PrintUInt32(FileLen, PrintFunc);
            PrintFunc(" bytes to ");
            PrintFunc(path);
            goto done_pat;
        }

        RetVal = AmbaFS_FileClose(pFile);
        if (RetVal != OK) {
            RetVal = AmbaFS_GetError();
            PrintFunc("fclose failed ");
            AmbaShell_PrintUInt32(RetVal, PrintFunc);
            goto done_pat;
        }

        AmbaFS_FileOpen(path, "r", &pFile);
        if (pFile == NULL) {
            RetVal = AmbaFS_GetError();
            PrintFunc("fopen");
            PrintFunc(path);
            PrintFunc(" failed ");
            AmbaShell_PrintUInt32(RetVal, PrintFunc);
            goto done_pat;
        }

        AmbaFS_FileRead(pRdBuf, 1, TestSize, pFile, &FileLen);
        if (FileLen != TestSize) {
            PrintFunc("[FAILED]read ");
            AmbaShell_PrintUInt32(FileLen, PrintFunc);
            PrintFunc(path);
            goto done_pat;
        }

        RetVal = AmbaFS_FileClose(pFile);
        if (RetVal != OK) {
            RetVal = AmbaFS_GetError();
            PrintFunc("fclose failed ");
            AmbaShell_PrintUInt32(RetVal, PrintFunc);
            PrintFunc("\n");
            goto done_pat;
        }

        RetVal = DataVerify(pWrbuf, pRdBuf, TestSize, PrintFunc);
        if (RetVal > 0) {
            PrintFunc("write random pattern and verify fail byte counts ");
            AmbaShell_PrintUInt32(RetVal, PrintFunc);
            PrintFunc("\n");
            ErrCnt++;
        } else
            PrintFunc( "write random pattern and verify ok\n");

        /* Remove the file */
        AmbaFS_Remove(path);
    }

done_pat:
    if (ErrCnt > 0) {
        AmbaShell_PrintUInt32(ErrCnt, PrintFunc);
        PrintFunc( "%d files with random pattern tested are failed");
    } else {
        PrintFunc( "test_fioprf_randvrfy is successful\n");
    }

    return;
}

/**********************************/
/* Export TestRandvrfy(). */
/**********************************/

static void SHELL_RandvrfyCmdUsage(char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [Slot] \n");
#if 0
    PrintFunc("  Size is an option means to the test file Size.\n");
    PrintFunc("  all is an option means to test with all free space\n");
    PrintFunc("  times means how many files will be tested\n");
    PrintFunc("  Delay is an option means ms Delay after each file\n");
    PrintFunc("  Keep is an option means to Keep files after test\n");
#endif
}

void AmbaShell_CommandRandvrfy(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    char Drive;

    if ((ArgCount < 2U) || (ArgCount > 6U)) {
        SHELL_RandvrfyCmdUsage(pArgVector, PrintFunc);
    } else {

        Drive = pArgVector[1][0];

        TestRandvrfy(Drive, PrintFunc);

        PrintFunc("test_fioprf_randvrfy completed!\n");
    }

}

