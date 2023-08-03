/**
 *  @file AmbaShell_Cat.c
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
 *  @details Shell command to concatenate file(s) to standard output.
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaShell.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"
#include "AmbaFS.h"

/* TBD */
#define AMBA_FS_ATTR_DIR    S_IFDIR
#define AMBA_FS_ATTR_VOLUME 0U

static void SHELL_CatOneFile(const char *pFileName, AMBA_SHELL_PRINT_f PrintFunc)
{
    char Buf[256];
    AMBA_FS_FILE *pFile;
    UINT32 RetVal;
    UINT32 ByteRead;

    RetVal = AmbaFS_FileOpen(pFileName, "r", &pFile);
    if (RetVal != 0U) {
        PrintFunc("ERROR (");
        AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
        PrintFunc("): Cannot open '");
        PrintFunc(pFileName);
        PrintFunc("'\n");
    } else {
        do {
            ByteRead = 0;
            RetVal = AmbaFS_FileRead(Buf, 1, sizeof(Buf) - 1U, pFile, &ByteRead);
            if (ByteRead > 0U) {
                Buf[ByteRead] = '\0'; /* Put '\0' in the end before printing string */
                PrintFunc(Buf);
            }
            if (RetVal != 0U) {
                // ignore error
            }
        } while (ByteRead > 0U);

        RetVal = AmbaFS_FileClose(pFile);
        if (RetVal != 0U) {
            PrintFunc("ERROR (");
            AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
            PrintFunc("): Cannot close '");
            PrintFunc(pFileName);
            PrintFunc("'\n");
        }

        PrintFunc("\n");
    }
}

static void SHELL_CatCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [file]\n");
}

/**
 *  AmbaShell_CommandCat - do command cat
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandCat(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Count = 0;
    AMBA_FS_DTA_t FileSearch;
    const char *pFileName;
    char FilePath[256]; /* absolute or relative */
    SIZE_t FilePathSize = 0;
    UINT32 i;
    UINT16 DirMask = (UINT16)AMBA_FS_ATTR_VOLUME | (UINT16)AMBA_FS_ATTR_DIR;
    UINT32 uRet = 0;

    if (ArgCount < 2U) {
        SHELL_CatCmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringLength(pArgVector[1]) >= (sizeof(FilePath) - 1U)) {
            PrintFunc("FilePath ");
            PrintFunc(pArgVector[1]);
            PrintFunc(" is too long.\n");
        } else {
            uRet = AMBSH_CHKCWD(PrintFunc); /* Set working directory */
            if (uRet == 0U) {
                AmbaUtility_StringCopy(FilePath, 256, pArgVector[1]);
                for (i = (AmbaUtility_StringLength(FilePath) - 1U); i != 0U; i--) {
                    if ((FilePath[i] == '\\') || (FilePath[i] == '/')) {
                        FilePathSize = (SIZE_t)i + 1U;
                        break;
                    }
                }

                for (uRet = AmbaFS_FileSearchFirst(pArgVector[1], AMBA_FS_ATTR_ALL, &FileSearch);
                     uRet == 0U;
                     uRet = AmbaFS_FileSearchNext(&FileSearch)) {

                    if ((FileSearch.Attribute & DirMask) != 0U) {
                        continue;
                    }

                    /* Get match filename */
                    pFileName = (FileSearch.LongName[0] != 0) ?
                                (char *)FileSearch.LongName :
                                (char *)FileSearch.FileName;

                    if ((FilePathSize + AmbaUtility_StringLength(pFileName)) >= (sizeof(FilePath) - 1U)) {
                        PrintFunc("FilePath ");
                        PrintFunc(FilePath);
                        PrintFunc("\\");
                        PrintFunc(pFileName);
                        PrintFunc(" is too long.\n");
                        break;
                    }

                    /* Attach relative/absolute path to the filename */
                    FilePath[FilePathSize] = '\0';
                    AmbaUtility_StringAppend(FilePath, 256, pFileName);

                    SHELL_CatOneFile(FilePath, PrintFunc);

                    Count++;
                }

                AmbaShell_PrintUInt32(Count, PrintFunc);
                PrintFunc(" files printed.\n");
            }
        }
    }
    return;
}

