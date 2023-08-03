/**
 *  @file AmbaShell_CopyFile.c
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
 *  @details Shell command to copy file(s) from source to destination
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include <AmbaShell.h>
#include <AmbaUtility.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"
#include "AmbaFS.h"

/* TBD */
#define AMBA_FS_ATTR_DIR    S_IFDIR
#define AMBA_FS_ATTR_VOLUME 0U
#define PF_CHAR             char

static char *AmbaShell_PFChar2Char(const PF_CHAR *pfc_ptr)
{
    char *ptr = NULL;
    AmbaMisra_TypeCast32(&ptr, &pfc_ptr);
    return ptr;
}
/* end TBD */

static UINT32 SHELL_CopyFile(const char *pSrcFile, const char *pDstFile, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    AMBA_FS_FILE *pFileIn = NULL;
    AMBA_FS_FILE *pFileOut = NULL;
    UINT32 DataSize, BytesWritten;
    static char _AmbaShellCpBuffer[4096] __attribute__((section(".bss.noinit")));

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
        RetVal = 0xFFFFFFFFU;
    } else {
        /* Loop for: read from pSrc, and write to pDst */
        do {
            RetVal = AmbaFS_FileRead(_AmbaShellCpBuffer, 1, sizeof(_AmbaShellCpBuffer), pFileIn, &DataSize);
            if ((DataSize > 0U) && (RetVal == 0U)) {
                RetVal = AmbaFS_FileWrite(_AmbaShellCpBuffer, 1, DataSize, pFileOut, &BytesWritten);
                if ((BytesWritten != DataSize) || (RetVal != 0U))  {
                    RetVal = 0xFFFFFFFFU;
                    break;
                }
            }
        } while (DataSize > 0U);

        if ((AmbaFS_FileEof(pFileIn) == 0U) || (RetVal != 0U)) {
            PrintFunc("Failed to copy ");
            PrintFunc(pSrcFile);
            PrintFunc(" to ");
            PrintFunc(pDstFile);
            PrintFunc("\n");
        }
    }

    if (pFileIn != NULL) {
        RetVal = AmbaFS_FileClose(pFileIn);
        if (0U != RetVal) {
            PrintFunc("Failed to close source file.\n");
        }
    }
    if (pFileOut != NULL) {
        RetVal = AmbaFS_FileClose(pFileOut);
        if (0U != RetVal) {
            PrintFunc("Failed to close destination file.\n");
        }
    }

    return RetVal;
}

static UINT32 SHELL_CopyMultiSrcFiles(const char *pSrc, char *pPathDst, UINT32 DstAttribute, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal, uRet;
    AMBA_FS_DTA_t FileSearchSrc;
    char PathSrc[LONG_NAME_BUF_SIZE];
    UINT32 OffsetSrcFileName, OffsetDstFileName;
    const char *pFileName;
    UINT16 FolderAttrMask = (UINT16)AMBA_FS_ATTR_VOLUME | (UINT16)AMBA_FS_ATTR_DIR;


    OffsetSrcFileName = AmbaShell_FindRepeatSep(pSrc);
    OffsetDstFileName = AmbaUtility_StringLength(pPathDst);

    /* search for existing source file */
    for (RetVal = AmbaFS_FileSearchFirst(pSrc, AMBA_FS_ATTR_ALL, &FileSearchSrc);
         RetVal == 0U;
         RetVal = AmbaFS_FileSearchNext(&FileSearchSrc)) {

        /* source must be file(s) */
        if ((FileSearchSrc.Attribute & FolderAttrMask) != 0U) {
            continue;
        }

        if (FileSearchSrc.LongName[0] != (INT8)'\0') {
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

        if ((DstAttribute & ((UINT32)AMBA_FS_ATTR_VOLUME | (UINT32)AMBA_FS_ATTR_DIR)) != 0U) {
            pPathDst[OffsetDstFileName] = '\\';
            pPathDst[OffsetDstFileName + 1U] = '\0';
            AmbaUtility_StringAppend(pPathDst, LONG_NAME_BUF_SIZE, (const char *) pFileName);
        }

        PrintFunc("Copying ");
        PrintFunc(PathSrc);
        PrintFunc(" to ");
        PrintFunc(pPathDst);
        PrintFunc("\n");
        uRet = SHELL_CopyFile(PathSrc, pPathDst, PrintFunc);
        if (0U != uRet) {
            break;
        }
    }

    return RetVal;
}

static void SHELL_Copy(const char *pSrc, char *pDst, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal, uRet;
    AMBA_FS_FILE *pFile;
    AMBA_FS_DTA_t FileSearchDst;
    UINT32 OffsetSrcWildcard, OffsetDstWildcard;
    UINT32 OffsetDstFileName;
    char PathSrc[LONG_NAME_BUF_SIZE];
    char PathDst[LONG_NAME_BUF_SIZE];
    const char *pFileName;
    UINT16 FolderAttrMask = (UINT16)AMBA_FS_ATTR_VOLUME | (UINT16)AMBA_FS_ATTR_DIR;

    if (AmbaUtility_StringLength(pDst) > (UINT32)LONG_NAME_BUF_SIZE) {
        PrintFunc("String ");
        PrintFunc(pDst);
        PrintFunc(" is too long\n");
    } else {
        RetVal = AMBSH_CHKCWD(PrintFunc);
        if (RetVal == 0U) {
            /* check if file/directory exists */
            RetVal = AmbaFS_FileOpen(pDst, "r", &pFile);
            if ((pFile == NULL) || (RetVal != 0U)) {
                /* create an empty file when the file does not exist */
                RetVal = AmbaFS_FileOpen(pDst, "w", &pFile);
            }
            /* close file/directory */
            if ((pFile != NULL) && (RetVal == 0U)) {
                if (AmbaFS_FileClose(pFile) != 0U) {
                    PrintFunc("Close file failed.\n");
                }
            }

            AmbaUtility_StringCopy(PathSrc, LONG_NAME_BUF_SIZE, pSrc);
            /* Check if source is a volume */
            if (AmbaShell_IsVolumePath(pSrc) == 0U) {
                AmbaUtility_StringAppend(PathSrc, LONG_NAME_BUF_SIZE, "*.*");
            }

            OffsetSrcWildcard = AmbaShell_FindWild(&PathSrc[0]);
            OffsetDstWildcard = AmbaShell_FindWild(pDst);
            OffsetDstFileName = AmbaShell_FindRepeatSep(pDst);

            /* Check if destination is a volume */
            if (AmbaShell_IsVolumePath(pDst) == 0U) {
                if ((AmbaUtility_StringLength(pDst) == 3U) && (pDst[2] == '\\')) {
                    pDst[2] = '\0';
                }
                RetVal = SHELL_CopyMultiSrcFiles(&PathSrc[0], pDst, AMBA_FS_ATTR_VOLUME, PrintFunc);
                if (RetVal != 0U) {
                    // ignore error
                }
            } else {
                /* search for existing destination file/directory */
                for (RetVal = AmbaFS_FileSearchFirst(pDst, AMBA_FS_ATTR_ALL, &FileSearchDst);
                     RetVal == 0U;
                     RetVal = AmbaFS_FileSearchNext(&FileSearchDst)) {

                    /* if wildcard '*' is found, the destination must be directory */
                    if ((OffsetSrcWildcard > 0U) || (OffsetDstWildcard > 0U)) {
                        if ((FileSearchDst.Attribute & FolderAttrMask) == 0U) {
                            continue;
                        }
                    }

                    if (FileSearchDst.LongName[0] != (INT8)'\0') {
                        const char *ptr = AmbaShell_PFChar2Char(&FileSearchDst.LongName[0]);
                        pFileName = ptr;
                    } else {
                        const char *ptr = AmbaShell_PFChar2Char(&FileSearchDst.FileName[0]);
                        pFileName = ptr;
                    }

                    if (OffsetDstFileName != 0U) {
                        AmbaShell_StringCopyLength(PathDst, LONG_NAME_BUF_SIZE, pDst, OffsetDstFileName);
                    }
                    PathDst[OffsetDstFileName] = '\0';
                    AmbaUtility_StringAppend(PathDst, LONG_NAME_BUF_SIZE, pFileName);

                    uRet = SHELL_CopyMultiSrcFiles(&PathSrc[0], &PathDst[0], FileSearchDst.Attribute, PrintFunc);

                    if (0U != uRet) {
                        break;
                    }
                }
            }
        }
    }

    return;
}

static inline void SHELL_CpCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Copies one or more files to another location.\n\n");
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [source] [destination]\n");
}

/**
 *  AmbaShell_CommandCRC32Sum - execute command copy file
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandCopy(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 3U) {
        SHELL_CpCmdUsage(pArgVector, PrintFunc);
    } else {
        SHELL_Copy(pArgVector[1], pArgVector[2], PrintFunc);
    }
    return;
}
