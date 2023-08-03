/**
 *  @file AmbaShell_ListDir.c
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
 *  @details Shell command to list directory contents
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
//#include <AmbaMisraFix.h>
#include <AmbaShell.h>
#include <AmbaFS.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

//#define PROFILE_LS  0x2

static void SHELL_ListDirProfile(const char *pName, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    AMBA_FS_DTA_t FileSearch;
    UINT32 StartTime, EndTime, Count = 0;
    UINT32 ElapsedTime;

    if (AmbaKAL_GetSysTickCount(&StartTime) != 0U) {
        // ignore error
    }
    PrintFunc("start time = ");
    AmbaShell_PrintUInt32(StartTime, PrintFunc);
    PrintFunc(" ms\n");

    RetVal = AmbaFS_FileSearchFirst(pName, ATTR_ALL, &FileSearch);

    while (RetVal == 0U) {
        if ((FileSearch.Attribute & (UINT16)ATTR_VOLUME) != 0U) {
            // do nothing
        } else {
            Count++;
        }
        RetVal = AmbaFS_FileSearchNext(&FileSearch);
    }

    if (Count == 0U) {
        Count = 1;
    }
    if (AmbaKAL_GetSysTickCount(&EndTime) != 0U) {
        // ignore error
    }
    if (StartTime < EndTime) {
        ElapsedTime = EndTime - StartTime;
    } else {
        ElapsedTime = (((0xffffffffU - StartTime) + EndTime) + 1U);
    }

    PrintFunc("end time = ");
    AmbaShell_PrintUInt32(EndTime, PrintFunc);
    PrintFunc(" ms\n");

    AmbaShell_PrintUInt32(Count, PrintFunc);
    PrintFunc(" files, total takes ");
    AmbaShell_PrintUInt32(ElapsedTime / 1000U, PrintFunc);
    PrintFunc(" sec, each takes ");
    AmbaShell_PrintUInt32(ElapsedTime / Count, PrintFunc);
    PrintFunc(" ms\n");

    return;
}

static const char *SHELL_GetMonthString(UINT32 Index)
{
    const char *rptr;
    switch(Index) {
    case 1:
        rptr = "Jan";
        break;
    case 2:
        rptr = "Feb";
        break;
    case 3:
        rptr = "Mar";
        break;
    case 4:
        rptr = "Apr";
        break;
    case 5:
        rptr = "May";
        break;
    case 6:
        rptr = "Jun";
        break;
    case 7:
        rptr = "Jul";
        break;
    case 8:
        rptr = "Aug";
        break;
    case 9:
        rptr = "Sep";
        break;
    case 10:
        rptr = "Oct";
        break;
    case 11:
        rptr = "Nov";
        break;
    case 12:
        rptr = "Dec";
        break;
    default:
        rptr = "Nul";
        break;
    }

    return rptr;
}

static void SHELL_MakeFileInfoString(char *StringBuf, UINT32 BufferSize, const AMBA_FS_DTA_t *FileSearch,
                                     UINT32 Sec, UINT32 Min, UINT32 Hour, UINT32 Day, UINT32 Month, UINT32 Year)
{
    SIZE_t StrLen, i;
    char IntString[UTIL_MAX_INT_STR_LEN];

    StringBuf[0] = ((FileSearch->Attribute & (UINT16)ATTR_DIR) != 0U)    ? 'd' : 'f';
    StringBuf[1] = ((FileSearch->Attribute & (UINT16)ATTR_RDONLY) != 0U) ? 'r' : '-';
    StringBuf[2] = ((FileSearch->Attribute & (UINT16)ATTR_ARCH) != 0U)   ? 'a' : '-';
    StringBuf[3] = ((FileSearch->Attribute & (UINT16)ATTR_SYSTEM) != 0U) ? 's' : '-';
    StringBuf[4] = ((FileSearch->Attribute & (UINT16)ATTR_HIDDEN) != 0U) ? 'h' : '-';
    StringBuf[5] = ' ';
    StringBuf[6] = '\0';
    AmbaUtility_StringAppend(StringBuf, BufferSize, SHELL_GetMonthString(Month));
    AmbaUtility_StringAppend(StringBuf, BufferSize, " ");
    // day
    if (Day < 10U) {
        AmbaUtility_StringAppend(StringBuf, BufferSize, "0");
    }
    if (AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Day, 10) == 0U) {
        // ignore return value
    }
    AmbaUtility_StringAppend(StringBuf, BufferSize, IntString);
    AmbaUtility_StringAppend(StringBuf, BufferSize, " ");

    // year
    if (AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Year, 10) == 0U) {
        // ignore return value
    }
    AmbaUtility_StringAppend(StringBuf, BufferSize, IntString);
    AmbaUtility_StringAppend(StringBuf, BufferSize, " ");

    // hour
    if (Hour < 10U) {
        AmbaUtility_StringAppend(StringBuf, BufferSize, "0");
    }
    if (AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Hour, 10) == 0U) {
        // ignore return value
    }
    AmbaUtility_StringAppend(StringBuf, BufferSize, IntString);
    AmbaUtility_StringAppend(StringBuf, BufferSize, ":");

    // minute
    if (Min < 10U) {
        AmbaUtility_StringAppend(StringBuf, BufferSize, "0");
    }
    if (AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Min, 10) == 0U) {
        // ignore return value
    }
    AmbaUtility_StringAppend(StringBuf, BufferSize, IntString);
    AmbaUtility_StringAppend(StringBuf, BufferSize, ":");

    // second
    if (Sec < 10U) {
        AmbaUtility_StringAppend(StringBuf, BufferSize, "0");
    }
    if (AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Sec, 10) == 0U) {
        // ignore return value
    }
    AmbaUtility_StringAppend(StringBuf, BufferSize, IntString);
    AmbaUtility_StringAppend(StringBuf, BufferSize, " ");

    // file size
    if (AmbaUtility_UInt64ToStr(IntString, UTIL_MAX_INT_STR_LEN, (UINT64)FileSearch->FileSize, 10) == 0U) {
        // ignore return value
    }
    StrLen = AmbaUtility_StringLength(IntString);
    if (StrLen < 12U) {
        for (i = 0; i < (12U - StrLen); i++) {
            AmbaUtility_StringAppend(StringBuf, BufferSize, " ");
        }
    }
    AmbaUtility_StringAppend(StringBuf, BufferSize, IntString);
    AmbaUtility_StringAppend(StringBuf, BufferSize, " ");
}

static void SHELL_ListDirInfo(const char *pName, AMBA_SHELL_PRINT_f PrintFunc)
{

    UINT32 RetVal;
    UINT32 Sec, Min, Hour, Day, Month, Year;
    AMBA_FS_DTA_t FileSearch;
    char StringBuf[560];

    RetVal = AmbaFS_FileSearchFirst(pName, ATTR_ALL, &FileSearch);

    while (RetVal == 0U) {
        if ((FileSearch.Attribute & (UINT16)ATTR_VOLUME) != 0U) {
            // do nothing
        } else {
            Sec   = ((UINT32)FileSearch.Time & 0x1fU) * 2U;
            Min   = ((UINT32)FileSearch.Time >> 5U) & 0x3fU;
            Hour  = ((UINT32)FileSearch.Time >> 11U) & 0x1fU;
            Day   = ((UINT32)FileSearch.Date & 0x1fU);
            Month = ((UINT32)FileSearch.Date >> 5U) & 0xfU;
            Year  = (((UINT32)FileSearch.Date >> 9U) & 0x7fU) + 1980U;

            AmbaUtility_MemorySetChar(StringBuf, '\0', (UINT32)sizeof(StringBuf));
            SHELL_MakeFileInfoString(StringBuf, 560, &FileSearch, Sec, Min, Hour, Day, Month, Year);

            if ((FileSearch.Attribute & ATTR_DIR) != 0U) {
                AmbaUtility_StringAppend(StringBuf, 560, "[");
            }

            if (FileSearch.LongName[0] != (PF_CHAR)'\0') {
                const char *ptr = AmbaShell_PFChar2Char(&FileSearch.LongName[0]);
                AmbaUtility_StringAppend(StringBuf, 560, ptr);
            } else {
                const char *ptr = AmbaShell_PFChar2Char(&FileSearch.FileName[0]);
                AmbaUtility_StringAppend(StringBuf, 560, ptr);
            }

            if ((FileSearch.Attribute & (UINT16)ATTR_DIR) != 0U) {
                AmbaUtility_StringAppend(StringBuf, 560, "]");
            }

            AmbaUtility_StringAppend(StringBuf, 560, "\n");

            PrintFunc(StringBuf);

        }

        RetVal = AmbaFS_FileSearchNext(&FileSearch);
    }

    return;
}

static void SHELL_ListDirL3(char *ParentPath, UINT32 BufferSize, const char *pWildcard, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("\n");
    PrintFunc(ParentPath);
    PrintFunc("\n");
    AmbaUtility_StringAppend(ParentPath, BufferSize, pWildcard);
    SHELL_ListDirInfo(ParentPath, PrintFunc);

    // no more directory list

    return;
}

static void SHELL_ListDirL2(char *ParentPath, UINT32 BufferSize, const char *pWildcard, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_FS_DTA_t FileSearch;
    const char* pFileName;
    UINT32 RetVal;
    SIZE_t PathSize;

    PathSize = AmbaUtility_StringLength(ParentPath);
    PrintFunc("\n");
    PrintFunc(ParentPath);
    PrintFunc("\n");
    AmbaUtility_StringAppend(ParentPath, BufferSize, pWildcard);
    SHELL_ListDirInfo(ParentPath, PrintFunc);

    if ((BufferSize > 2U) && (PathSize < ((SIZE_t)BufferSize - 2U))) {

        ParentPath[PathSize] = '*';
        ParentPath[PathSize + 1U] = '\0';

        RetVal = AmbaFS_FileSearchFirst(ParentPath, ATTR_ALL, &FileSearch);

        while (RetVal == 0U) {
            if ((FileSearch.Attribute & (UINT16)ATTR_DIR) == 0U) {
                // do nothing
            } else {
                if (FileSearch.LongName[0] != (INT8)'\0') {
                    const char *ptr = AmbaShell_PFChar2Char(&FileSearch.LongName[0]);
                    pFileName = ptr;
                } else {
                    const char *ptr = AmbaShell_PFChar2Char(&FileSearch.FileName[0]);
                    pFileName = ptr;
                }

                if ((pFileName[0] == '.') &&
                    (pFileName[1] == '\0')) {
                    // do nothing
                } else if ((pFileName[0] == '.') &&
                           (pFileName[1] == '.') &&
                           (pFileName[2] == '\0')) {
                    // do nothing
                } else {
                    /* restore ParentPath to original */
                    ParentPath[PathSize] = '\0';

                    AmbaUtility_StringAppend(ParentPath, BufferSize, pFileName);
                    AmbaUtility_StringAppend(ParentPath, BufferSize, "\\");
                    SHELL_ListDirL3(ParentPath, BufferSize, pWildcard, PrintFunc);
                }
            }

            RetVal = AmbaFS_FileSearchNext(&FileSearch);

        }

        /* restore ParentPath to original */
        ParentPath[PathSize] = '\0';
    }
    return;
}

static void SHELL_ListDirL1(char *ParentPath, UINT32 BufferSize, const char *pWildcard, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_FS_DTA_t FileSearch;
    const char* pFileName;
    UINT32 RetVal;
    SIZE_t PathSize;

    PathSize = AmbaUtility_StringLength(ParentPath);
    if (ParentPath[PathSize] != '\\') {
        AmbaUtility_StringAppend(ParentPath, BufferSize, "\\");
        PathSize = AmbaUtility_StringLength(ParentPath);
    }
    PrintFunc("\n");
    PrintFunc(ParentPath);
    PrintFunc("\n");
    AmbaUtility_StringAppend(ParentPath, BufferSize, pWildcard);
    SHELL_ListDirInfo(ParentPath, PrintFunc);

    if ((BufferSize > 2U) && (PathSize < ((SIZE_t)BufferSize - 2U))) {
        ParentPath[PathSize] = '*';
        ParentPath[PathSize + 1U] = '\0';

        RetVal = AmbaFS_FileSearchFirst(ParentPath, ATTR_ALL, &FileSearch);

        while (RetVal == 0U) {
            if ((FileSearch.Attribute & (UINT16)ATTR_DIR) == 0U) {
                // do nothing
            } else {
                if (FileSearch.LongName[0] != (INT8)'\0') {
                    const char *ptr = AmbaShell_PFChar2Char(&FileSearch.LongName[0]);
                    pFileName = ptr;
                } else {
                    const char *ptr = AmbaShell_PFChar2Char(&FileSearch.FileName[0]);
                    pFileName = ptr;
                }

                if ((pFileName[0] == '.') &&
                    (pFileName[1] == '\0')) {
                    // do nothing
                } else if ((pFileName[0] == '.') &&
                           (pFileName[1] == '.') &&
                           (pFileName[2] == '\0')) {
                    // do nothing
                } else {

                    /* restore ParentPath to original */
                    ParentPath[PathSize] = '\0';

                    AmbaUtility_StringAppend(ParentPath, BufferSize, pFileName);
                    AmbaUtility_StringAppend(ParentPath, BufferSize, "\\");
                    SHELL_ListDirL2(ParentPath, BufferSize, pWildcard, PrintFunc);
                }
            }

            RetVal = AmbaFS_FileSearchNext(&FileSearch);

        }

        /* restore ParentPath to original */
        ParentPath[PathSize] = '\0';
    }
    return;
}

/**
 *  AmbaShell_CommandListDir - execute command ls
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandListDir(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    static const char *pWildcard = "*.*";
    const char *pPathName = NULL;
    char WorkDir[LONG_NAME_BUF_SIZE]   = {'\0'};
    char InputPath[LONG_NAME_BUF_SIZE] = {'\0'};
    AMBA_FS_FILE_INFO_s PathInfo;
    UINT32 RetVal;
    SIZE_t StringLength;

    RetVal = AMBSH_CHKCWD(PrintFunc);
    if (RetVal == 0U) {
        if (ArgCount > 2U) {
            pPathName = pArgVector[2];
        } else if (ArgCount == 2U) {
            pPathName = pArgVector[1];
        } else {
            pPathName = ".";
        }

        /* If get a volume or directory, try to add wildcard at the end of the path */
        AmbaUtility_StringCopy(InputPath, LONG_NAME_BUF_SIZE, pPathName);
        StringLength = AmbaUtility_StringLength(InputPath);
        RetVal = AmbaFS_GetFileInfo(InputPath, &PathInfo);
        if ((RetVal == 0U) && (StringLength > 0U)) {
            if ((AmbaShell_IsVolumePath(InputPath) == 0U) || (PathInfo.Attr == AMBA_FS_ATTR_DIR)) {
                if (InputPath[StringLength - 1U] == '\\') {
                    AmbaUtility_StringAppend(InputPath, LONG_NAME_BUF_SIZE, pWildcard);
                } else {
                    AmbaUtility_StringAppend(InputPath, LONG_NAME_BUF_SIZE, "\\*.*");
                }
            }

            if ((ArgCount >= 2U) && (pArgVector[1][0] == '-')) {
                char char_source = pArgVector[1][1];
                if (char_source == 'R') {
                    AmbaUtility_StringCopy(WorkDir, LONG_NAME_BUF_SIZE, pPathName);
                    SHELL_ListDirL1(WorkDir, LONG_NAME_BUF_SIZE, pWildcard, PrintFunc);
                } else if (char_source == 'p') {
                    SHELL_ListDirProfile(InputPath, PrintFunc);
                } else if (char_source == 'l') {
                    SHELL_ListDirInfo(InputPath, PrintFunc);
                } else {
                    PrintFunc("Unknown parameter: ");
                    PrintFunc(pArgVector[1]);
                    PrintFunc("\n");
                }
            } else {
                SHELL_ListDirInfo(InputPath, PrintFunc);
            }
        }
    }
    return;
}
