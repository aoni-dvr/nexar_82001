/**
 *  @file AmbaShell_RemoveFile.c
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
 *  @details Shell command to remove file(s)
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaShell.h>
#include <AmbaFS.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

/* TBD */
#define PF_CHAR                 char
#define AMBA_FS_ATTR_FILE_ONLY  S_IFREG
static char *AmbaShell_PFChar2Char(const PF_CHAR *pfc_ptr)
{
    char *ptr = NULL;
    AmbaMisra_TypeCast32(&ptr, &pfc_ptr);
    return ptr;
}
/* end TBD */

static inline void SHELL_RmCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [file]\n");
}

/**
 *  AmbaShell_CommandRemove - execute command remove file
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandRemove(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = 0, Count = 0;
    SIZE_t i;
    AMBA_FS_DTA_t FileSearch;
    const char *pFileName;
    char FilePath[256]; /* absolute or relative */
    UINT32 SearchResult;
    SIZE_t FilePathSize = 0;
    UINT16 SkipMask = (UINT16)AMBA_FS_ATTR_VOLUME | (UINT16)AMBA_FS_ATTR_DIR;
    SIZE_t StringLength;

    if (ArgCount < 2U) {
        SHELL_RmCmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringLength(pArgVector[1]) >= (sizeof(FilePath) - 1U)) {
            PrintFunc("FilePath ");
            PrintFunc(pArgVector[1]);
            PrintFunc(" is too long.\n");
        } else {
            RetVal = AMBSH_CHKCWD(PrintFunc); /* Set working directory */
            if (RetVal == 0U) {
                AmbaUtility_StringCopy(FilePath, 256, pArgVector[1]);
                StringLength = AmbaUtility_StringLength(FilePath);
                if (StringLength > 0U) {
                    for (i = StringLength - 1U; i > 0U; i --) {
                        if ((FilePath[i] == '\\') || (FilePath[i] == '/')) {
                            FilePathSize = i + 1U;
                            break;
                        }
                    }
                }
                SearchResult = AmbaFS_FileSearchFirst(pArgVector[1], AMBA_FS_ATTR_ALL, &FileSearch);

                while ((SearchResult == 0U) && (RetVal == 0U)) {

                    if ((FileSearch.Attribute & SkipMask) != 0U) {
                        // do nothing for non-file object
                    } else {
                        /* Get match filename */
                        if (FileSearch.LongName[0] != 0) {
                            const char *ptr = AmbaShell_PFChar2Char(&FileSearch.LongName[0]);
                            pFileName = ptr;
                        } else {
                            const char *ptr = AmbaShell_PFChar2Char(&FileSearch.FileName[0]);
                            pFileName = ptr;
                        }

                        if ((FilePathSize + AmbaUtility_StringLength(pFileName)) >= (sizeof(FilePath) - 1U)) {
                            PrintFunc("FilePath ");
                            PrintFunc(FilePath);
                            PrintFunc("\\");
                            PrintFunc(pFileName);
                            PrintFunc(" is too long.\n");
                            RetVal = 0xFFFFFFFFU;
                        } else {
                            /* Attach relative/absolute path to the filename */
                            FilePath[FilePathSize] = '\0';
                            AmbaUtility_StringAppend(FilePath, 256, (const char *) pFileName);

                            RetVal = AmbaFS_Remove(FilePath);
                            if (RetVal != 0U) {
                                PrintFunc("ERROR (");
                                AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
                                PrintFunc("): Cannot remove ");
                                PrintFunc(FilePath);
                                PrintFunc("\n");
                                RetVal = 0xFFFFFFFFU;
                            } else {
                                Count++;
                            }
                        }
                    }
                    SearchResult = AmbaFS_FileSearchNext(&FileSearch);
                }

                AmbaShell_PrintUInt32(Count, PrintFunc);
                PrintFunc(" files removed.\n");

            }
        }
    }

    return;
}
