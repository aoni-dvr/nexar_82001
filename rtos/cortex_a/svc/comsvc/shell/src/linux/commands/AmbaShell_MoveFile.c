/**
 *  @file AmbaShell_MoveFile.c
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
 *  @details Shell command to move or rename files and directories
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaShell.h>
#include <AmbaFS.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"
#include <ctype.h>

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

static UINT32 SHELL_MoveWildCard(const char *pSrcFile, const char *pDstFile, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    AMBA_FS_FILE *pFileIn = NULL;
    AMBA_FS_FILE *pFileOut = NULL;
    UINT32 DataSize, ByteWritten;
    static char AmbaShellCpBuffer[4096] __attribute__((section(".bss.noinit")));

    RetVal = AmbaFS_FileOpen(pSrcFile, "r", &pFileIn);
    if (RetVal != 0U) {
        RetVal = 0xFFFFFFFFU;
    } else {
        RetVal = AmbaFS_FileOpen(pDstFile, "w", &pFileOut);
        if (RetVal != 0U) {
            RetVal = 0xFFFFFFFFU;
        }
    }

    if ((pFileIn == NULL) || (pFileOut == NULL) || (RetVal != 0U)) {
        PrintFunc("Cannot open '");
        PrintFunc(pSrcFile);
        PrintFunc("' or '");
        PrintFunc(pDstFile);
        PrintFunc("'\n");
    } else {
        /* Loop for: read from pSrc, and write to pDst */
        do {
            RetVal = AmbaFS_FileRead(AmbaShellCpBuffer, 1, sizeof(AmbaShellCpBuffer), pFileIn, &DataSize);
            if ((DataSize > 0U) && (RetVal == 0U)) {
                RetVal = AmbaFS_FileWrite(AmbaShellCpBuffer, 1, DataSize, pFileOut, &ByteWritten);
                if ((ByteWritten != DataSize) || (RetVal != 0U)) {
                    // error
                    break;
                }
            }
        } while (DataSize > 0U);

        if ((AmbaFS_FileEof(pFileIn) != 1U) || (RetVal != 0U)) {
            PrintFunc("Failed to copy ");
            PrintFunc(pSrcFile);
            PrintFunc(" to ");
            PrintFunc(pDstFile);
            PrintFunc("'\n");
        }
    }

    if (pFileIn != NULL) {
        if (AmbaFS_FileClose(pFileIn) != 0U) {
            PrintFunc("Failed to close ");
            PrintFunc(pSrcFile);
            PrintFunc("!\n");
        }
    }
    if (pFileOut != NULL) {
        if (AmbaFS_FileClose(pFileOut) != 0U) {
            PrintFunc("Failed to close ");
            PrintFunc(pDstFile);
            PrintFunc("!\n");
        }
    }

    RetVal = AmbaFS_Remove(pSrcFile);
    if (RetVal != 0U) {
        PrintFunc("ERROR (");
        AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
        PrintFunc("): Cannot remove ");
        PrintFunc(pSrcFile);
        PrintFunc("\n");
    }

    return RetVal;
}


static void SHELL_MoveMultiSrcFiles(const char *pSrc, char *pPathDst, UINT32 DstAttribute, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    AMBA_FS_DTA_t FileSearchSrc;
    char PathSrc[LONG_NAME_BUF_SIZE];
    UINT32 OffsetSrcFileName, OffsetDstFileName;
    const char *pFileName;
    char SrcDrive, DstDrive;
    const char *pCwd = AmbaShell_CurrentWorkDirGet();
    UINT16 AttribMask = (UINT16)AMBA_FS_ATTR_VOLUME | (UINT16)AMBA_FS_ATTR_DIR;

    OffsetSrcFileName = AmbaShell_FindRepeatSep(pSrc);
    OffsetDstFileName = AmbaUtility_StringLength(pPathDst);

    if(pSrc[1] == ':') {
        SrcDrive = toupper(pSrc[0]);
    } else {
        SrcDrive = toupper(pCwd[0]);
    }

    if(pPathDst[1] == ':') {
        DstDrive = toupper(pPathDst[0]);
    } else {
        DstDrive = toupper(pCwd[0]);
    }

    /* search for existing source file */
    RetVal = AmbaFS_FileSearchFirst(pSrc, AMBA_FS_ATTR_ALL, &FileSearchSrc);
    while (RetVal == 0U) {

        if ((FileSearchSrc.Attribute & AttribMask) != 0U) {
            /* source must be file(s) */
            // do nothing
        } else {
            if (FileSearchSrc.LongName[0] != (PF_CHAR)'\0') {
                const char *ptr = AmbaShell_PFChar2Char(&FileSearchSrc.LongName[0]);
                pFileName = ptr;
            } else {
                const char *ptr = AmbaShell_PFChar2Char(&FileSearchSrc.FileName[0]);
                pFileName = ptr;
            }

            if (OffsetSrcFileName != 0U) {
                AmbaShell_StringCopyLength(PathSrc, LONG_NAME_BUF_SIZE, pSrc, OffsetSrcFileName);
            }
            PathSrc[OffsetSrcFileName] = '\0';
            AmbaUtility_StringAppend(PathSrc, LONG_NAME_BUF_SIZE, pFileName);

            if ((DstAttribute & AttribMask) != 0U) {
                pPathDst[OffsetDstFileName] = '\\';
                pPathDst[OffsetDstFileName + 1U] = '\0';
                AmbaUtility_StringAppend(pPathDst, LONG_NAME_BUF_SIZE, pFileName);
            }

            PrintFunc("Moving ");
            PrintFunc(PathSrc);
            PrintFunc(" to ");
            PrintFunc(pPathDst);
            PrintFunc("\n");

            if(SrcDrive == DstDrive) {
                if (AmbaFS_Move(PathSrc, pPathDst) != 0U) {
                    PrintFunc("Failed to move ");
                    PrintFunc(PathSrc);
                    PrintFunc(" to ");
                    PrintFunc(pPathDst);
                    PrintFunc("\n");
                }
            } else {
                if (SHELL_MoveWildCard(PathSrc, pPathDst, PrintFunc) != 0U) {
                    PrintFunc("Failed to move ");
                    PrintFunc(PathSrc);
                    PrintFunc(" to ");
                    PrintFunc(pPathDst);
                    PrintFunc("\n");
                }
            }
        }
        RetVal = AmbaFS_FileSearchNext(&FileSearchSrc);
    }

    return;
}

static void SHELL_Move(const char *pSrc, char *pDst, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = OK;
    AMBA_FS_FILE_INFO_s FileInfo;
    char PathSrc[LONG_NAME_BUF_SIZE];
    SIZE_t MaxLength = LONG_NAME_BUF_SIZE;

    if (AmbaUtility_StringLength(pDst) > MaxLength) {
        PrintFunc("String ");
        PrintFunc(pDst);
        PrintFunc(" is too long\n");
    } else {

        RetVal = AMBSH_CHKCWD(PrintFunc);

        if (RetVal == 0U) {
            AmbaUtility_StringCopy(PathSrc, LONG_NAME_BUF_SIZE, pSrc);
            /* Check if source is a volume */
            if (AmbaShell_IsVolumePath(pSrc) == 0U) {
                AmbaUtility_StringAppend(PathSrc, LONG_NAME_BUF_SIZE, "*.*");
            }

            /* Check if destination is a volume */
            if (AmbaShell_IsVolumePath(pDst) == 0U) {
                if ((AmbaUtility_StringLength(pDst) == 3U) && (pDst[2] == '\\')) {
                    pDst[2] = '\0';
                }
                SHELL_MoveMultiSrcFiles(&PathSrc[0], pDst, AMBA_FS_ATTR_VOLUME, PrintFunc);
            } else {
                /* If destination is not exist, it is exptected the destination is a file path */
                if (AmbaFS_GetFileInfo(pDst, &FileInfo) != 0U) {
                    SHELL_MoveMultiSrcFiles(&PathSrc[0], pDst, AMBA_FS_ATTR_FILE_ONLY, PrintFunc);
                } else {
                    SHELL_MoveMultiSrcFiles(&PathSrc[0], pDst, FileInfo.Attr, PrintFunc);
                }
            }
        }
    }

    return;
}

static inline void SHELL_MvCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Move (rename) files.\n\n");
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [source] [destination]\n");
    PrintFunc(" Note: 1. It will not move the file once the file is already existed in the destination.\n");
    PrintFunc("       2. It does not support to use wildcard (*) to specify the destination name.\n");
    PrintFunc("       3. It only support to move file.\n");
}

/**
 *  AmbaShell_CommandMove - execute command mv
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandMove(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 3U) {
        SHELL_MvCmdUsage(pArgVector, PrintFunc);
    } else {
        SHELL_Move(pArgVector[1], pArgVector[2], PrintFunc);
    }
    return;
}
