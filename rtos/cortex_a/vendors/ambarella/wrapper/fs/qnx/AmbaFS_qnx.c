/**
 *  @file AmbaFS_qnx.c
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
 *  @details Ambarella file system abstraction layer.
 *
 */

#include "AmbaFS.h"
#include "AmbaWrap.h"
#include <process.h>
#include <AmbaUtility.h>
#include <libgen.h>
#include <string.h>

#define AMBAFS_DRIVE_A        "a:"
#define AMBAFS_DRIVE_A_UPCASE "A:"
#define AMBAFS_DRIVE_A_POSIX  "/tmp/FL0"

#define AMBAFS_DRIVE_B        "b:"
#define AMBAFS_DRIVE_B_UPCASE "B:"
#define AMBAFS_DRIVE_B_POSIX  "/tmp/FL1"

#define AMBAFS_DRIVE_C        "c:"
#define AMBAFS_DRIVE_C_UPCASE "C:"
#define AMBAFS_DRIVE_C_POSIX  "/tmp/SD0"

#define AMBAFS_DRIVE_D        "d:"
#define AMBAFS_DRIVE_D_UPCASE "D:"
#define AMBAFS_DRIVE_D_POSIX  "/tmp/SD1"

#define AMBA_PATH_STYLE       "\\"
#define AMBA_PATH_STYLE_POSIX "/"

#define AMBA_PATH_APPEND_SIZE (16U)

static UINT32 ISAbsolutePath(const char *path)
{
    UINT32 Rval = 0;

    if (path[1] == ':') {
        Rval = 1U;   /* Unicode absolute path */
    }
    return Rval;
}

static const char * AmbaFS_strstr(const char * string, const char *substring)
{
    /* string : String to search. */
    /* substring : Substring to try to find in string. */

    const char *a, *b;

    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = substring;
    if (*b == 0) {
        return string;
    }

    for (; *string != 0; string += 1) {
        if (*string != *b) {
            continue;
        }
        a = string;
        while (1) {
            if (*b == 0) {
                return string;
            }
            if (*a++ != *b++) {
                break;
            }
        }
        b = substring;
    }
    return NULL;
}

static void replaceWord(const char* s, const char* oldW, const char* newW, char *result)
{
    int i, cnt = 0;
    int newWlen = AmbaUtility_StringLength(newW);
    int oldWlen = AmbaUtility_StringLength(oldW);

    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++) {
        if (AmbaFS_strstr(&s[i], oldW) == &s[i]) {
            cnt++;

            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }

    // Making new string of enough length
    //result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1);

    i = 0;
    while (*s) {
        // compare the substring with the result
        if (AmbaFS_strstr(s, oldW) == s) {
            AmbaUtility_StringCopy(&result[i], 512, newW);
            i += newWlen;
            s += oldWlen;
        } else
            result[i++] = *s++;
    }

    result[i] = '\0';
}

static UINT32 AmbaFS_DriveToPath(const char *pFileName, char *pAmbaFilePath)
{
    UINT32 Rval = OK;

    UINT32 PathLen = AmbaUtility_StringLength(pFileName) + AMBA_PATH_APPEND_SIZE;
    char   TempBuf[PathLen];

    AmbaWrap_memset(TempBuf, 0, PathLen);
    AmbaWrap_memset(pAmbaFilePath, 0, PathLen);

    if (ISAbsolutePath(pFileName) == 1U) {
        char Drive = pFileName[0];

        switch (Drive) {
        case 'c':
            replaceWord(pFileName, AMBAFS_DRIVE_C, AMBAFS_DRIVE_C_POSIX, TempBuf);
            break;
        case 'C':
            replaceWord(pFileName, AMBAFS_DRIVE_C_UPCASE, AMBAFS_DRIVE_C_POSIX, TempBuf);
            break;
        case 'a':
            replaceWord(pFileName, AMBAFS_DRIVE_A, AMBAFS_DRIVE_A_POSIX, TempBuf);
            break;
        case 'A':
            replaceWord(pFileName, AMBAFS_DRIVE_A_UPCASE, AMBAFS_DRIVE_A_POSIX, TempBuf);
            break;
        case 'b':
            replaceWord(pFileName, AMBAFS_DRIVE_B, AMBAFS_DRIVE_B_POSIX, TempBuf);
            break;
        case 'B':
            replaceWord(pFileName, AMBAFS_DRIVE_B_UPCASE, AMBAFS_DRIVE_B_POSIX, TempBuf);
            break;
        case 'd':
            replaceWord(pFileName, AMBAFS_DRIVE_D, AMBAFS_DRIVE_D_POSIX, TempBuf);
            break;
        case 'D':
            replaceWord(pFileName, AMBAFS_DRIVE_D_UPCASE, AMBAFS_DRIVE_B_POSIX, TempBuf);
            break;
        default:
            Rval = AMBA_FS_ERR_ARG;
            break;
        }
        /* Change '\' to '/' */
        replaceWord(TempBuf, AMBA_PATH_STYLE, AMBA_PATH_STYLE_POSIX, pAmbaFilePath);
    } else {
        AmbaUtility_StringCopy(pAmbaFilePath, PathLen, pFileName);
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_Init
 *
 *  @Description:: Init AmbaFS file system.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_Init(void)
{
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileOpen
 *
 *  @Description:: Opens an existing file and returns the file descriptor
 *
 *  @Input      ::
 *      pFileName:  Pointer to the path and file name
 *      pMode:      Pointer to access Mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileOpen(const char *pFileName, const char *pMode, AMBA_FS_FILE **pFile)
{
    UINT32 Rval = 0U;

    UINT32 PathLen = AmbaUtility_StringLength(pFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pFileName, AmbaFilePath);

    if (Rval == OK) {
        *pFile = fopen(AmbaFilePath, pMode);
        if ((*pFile) == NULL) {
            Rval = AMBA_FS_ERR_API;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileClose
 *
 *  @Description:: Close an opened file
 *
 *  @Input      ::
 *      pFile:  Pointer to file descriptor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileClose(AMBA_FS_FILE *pFile)
{
    UINT32 Rval = 0U;

    INT32 RvalInt = fclose(pFile);
    if (RvalInt != 0) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileRead
 *
 *  @Description:: Read file data
 *
 *  @Input      ::
 *      Size:   Size for data to be read in bytes
 *      pFile:  Pointer to file descriptor
 *
 *  @Output     ::
 *      pBuf:   Pointer to data storage area for data to be read
 *
 *  @Return     ::
 *      UINT64  :   File read count
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileRead(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    (*pNumSuccess) = fread(pBuf, Size, Count, pFile);

    return 0U;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileWrite
 *
 *  @Description:: Write file data
 *
 *  @Input      ::
 *      pBuf:   Pointer to data storage area for data to be written
 *      Size:   Size for data to be written in bytes
 *      pFile:  Pointer to file descriptor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT64  :   File write bytes.
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileWrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    (*pNumSuccess) = fwrite(pBuf, Size, Count, pFile);

    return 0U;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileTell
 *
 *  @Description:: Get current value of position indicator of the file.
 *
 *  @Input      ::
 *      pFile:  Pointer to file descriptor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT64   :   current file position/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileTell(AMBA_FS_FILE * pFile, UINT64* pFilePos)
{
    UINT32 Rval = 0U;

    (*pFilePos) = ftello64(pFile);

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_fseek
 *
 *  @Description:: Reposition file position indicator.
 *
 *  @Input      ::
 *      pFile:  Pointer to file descriptor
 *      Offset: Byte count from reference point
 *      Origin: Reference point
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileSeek(AMBA_FS_FILE *pFile, INT64 Offset, INT32 Origin)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalInt = fseeko64(pFile, Offset, Origin);
    if (RvalInt != 0) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_remove
 *
 *  @Description:: Delete a file
 *
 *  @Input      ::
 *      pFileName:  Pointer to path and file name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_Remove(const char *pFileName)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLen = AmbaUtility_StringLength(pFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pFileName, AmbaFilePath);

    if (Rval == OK) {
        INT32 RvalInt = remove(AmbaFilePath);
        if (RvalInt != 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_rename
 *
 *  @Description:: Renames a file or a directory
 *
 *  @Input      ::
 *   pOldName: Pointer to path and file name
 *   pNewName: Pointer to new file name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_Rename(const char *pOldName, const char *pNewName)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLenOld = AmbaUtility_StringLength(pOldName) + AMBA_PATH_APPEND_SIZE;
    char AmbaPathOld[PathLenOld];

    UINT32 PathLenNew = AmbaUtility_StringLength(pNewName) + AMBA_PATH_APPEND_SIZE;
    char AmbaPathNew[PathLenNew];

    Rval = AmbaFS_DriveToPath(pOldName, AmbaPathOld);

    if (Rval == OK) {
        Rval = AmbaFS_DriveToPath(pNewName, AmbaPathNew);
    }

    if (Rval == OK) {
        INT32 RvalInt = rename(AmbaPathOld, AmbaPathNew);
        if (RvalInt != 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_Move
 *
 *  @Description:: Moves a file or directory
 *
 *  @Input      ::
 *      pSrcName:   Pointer to source file/directory and path name
 *      pDstName:   Pointer to destination file/directory and path name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_Move(const char *pSrcName, const char *pDstName)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLenSrc = AmbaUtility_StringLength(pSrcName) + AMBA_PATH_APPEND_SIZE;
    char AmbaPathSrc[PathLenSrc];

    UINT32 PathLenDst = AmbaUtility_StringLength(pDstName) + AMBA_PATH_APPEND_SIZE;
    char AmbaPathDst[PathLenDst];

    Rval = AmbaFS_DriveToPath(pSrcName, AmbaPathSrc);
    if (Rval == OK) {
        Rval = AmbaFS_DriveToPath(pDstName, AmbaPathDst);
    }

    if (Rval == OK) {
        INT32 RvalInt = rename(AmbaPathSrc, AmbaPathDst);
        if (RvalInt != 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileGetPos
 *
 *  @Description:: Get current file I/O pointer
 *
 *  @Input      ::
 *      pFile:  Pointer to file descriptor
 *
 *  @Output     ::
 *      pPos:   Position of file I/O pointer
 *
 *  @Return     ::
 *      INT64   : current file position
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileGetPos(AMBA_FS_FILE *pFile, AMBA_FS_FPOS *pFilePos)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32  RvalInt = fgetpos(pFile, pFilePos);
    if (RvalInt != 0) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileSetPos
 *
 *  @Description:: Move file I/O pointer.
 *
 *  @Input      ::
 *      pFile:  Pointer to file descriptor
 *      pPos:   Position of file I/O pointer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileSetPos(AMBA_FS_FILE *pFile, AMBA_FS_FPOS *pFilePos)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32  RvalInt = fsetpos(pFile, pFilePos);
    if (RvalInt != 0) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_feof
 *
 *  @Description:: Check for end of file
 *
 *  @Input      ::
 *      pFile:  Pointer to file descriptor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileEof(AMBA_FS_FILE *pFile)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalInt = feof(pFile);
    if (RvalInt < 0) {
        Rval = AMBA_FS_ERR_API;
    } else {
        AmbaMisra_TypeCast32(&Rval, &RvalInt);
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileTruncate
 *
 *  @Description:: Divides a file in two at a byte boundary.
 *                 The data after Offset of the source file will be moved to the new file.
 *                 And the source file will be trancated to Offset.
 *
 *  @Input      ::
 *      pFileName: Pointer to the path name of the file to be divided
 *      Offset:    Byte offset from the beginning of the file to the division location
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileTruncate(const char *pFileName, UINT64 Offset)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32 FileCtrl;

    UINT32 PathLen = AmbaUtility_StringLength(pFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pFileName, AmbaFilePath);

    if (Rval == OK) {
        FileCtrl = open(AmbaFilePath, O_RDWR | O_CLOEXEC | O_LARGEFILE);
        if (FileCtrl < 0) {
            Rval = AMBA_FS_ERR_API;
        } else {
            INT32 RvalInt = ftruncate64(FileCtrl, Offset);
            if (RvalInt != 0) {
                Rval = AMBA_FS_ERR_API;
            }
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_ChangeFileMode
 *
 *  @Description:: Change file attributes
 *
 *  @Input      ::
 *      pFileName:  Pointer to path and file name
 *      Attr:       File attribute to change
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_ChangeFileMode(const char *pFileName, UINT32 Attr)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLen = AmbaUtility_StringLength(pFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pFileName, AmbaFilePath);

    if (Rval == OK) {
        INT32 RvalInt = chmod(AmbaFilePath, (INT32)Attr);
        if (RvalInt != 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_ChangeDirAttributes
 *
 *  @Description:: Change directory attributes.
 *
 *  @Input      ::
 *      pDirName:   Pointer to path and directory name
 *      Attr:       File attribute, Change attribute mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_ChangeDirAttributes(const char *pDirName, UINT32 Attr)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLen = AmbaUtility_StringLength(pDirName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pDirName, AmbaFilePath);

    if (Rval == OK) {
        INT32 RvalInt = chmod(AmbaFilePath, (INT32)Attr);
        if (RvalInt != 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_GetFileInfo
 *
 *  @Description:: Get file or directory information
 *
 *  @Input      ::
 *      pName:  Pointer to path and file/directory name
 *
 *  @Output     ::
 *      pInfo:  Pointer to file information area
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_GetFileInfo(const char *pName, AMBA_FS_FILE_INFO_s *pInfo)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    struct stat FileStat;
    struct tm *pTimeInfo;

    UINT32 PathLen = AmbaUtility_StringLength(pName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pName, AmbaFilePath);

    if (Rval == OK) {
        if (lstat(AmbaFilePath, &FileStat) == 0) {
            Rval = OK;
        } else {
            Rval = AMBA_FS_ERR_API;
        }
    }

    if (Rval == OK) {
        pInfo->Size = FileStat.st_size;
        pInfo->Attr = FileStat.st_mode;

        pTimeInfo = localtime(&FileStat.st_atime);

        pInfo->TimeAccess.Day    = pTimeInfo->tm_mday;
        pInfo->TimeAccess.Month  = pTimeInfo->tm_mon;
        pInfo->TimeAccess.Year   = pTimeInfo->tm_year;

        pInfo->TimeAccess.Second = pTimeInfo->tm_sec;
        pInfo->TimeAccess.Minute = pTimeInfo->tm_min;
        pInfo->TimeAccess.Hour   = pTimeInfo->tm_hour;

        pTimeInfo = localtime(&FileStat.st_mtime);

        pInfo->TimeModify.Day    = pTimeInfo->tm_mday;
        pInfo->TimeModify.Month  = pTimeInfo->tm_mon;
        pInfo->TimeModify.Year   = pTimeInfo->tm_year;

        pInfo->TimeModify.Second = pTimeInfo->tm_sec;
        pInfo->TimeModify.Minute = pTimeInfo->tm_min;
        pInfo->TimeModify.Hour   = pTimeInfo->tm_hour;

        pTimeInfo = localtime(&FileStat.st_ctime);

        pInfo->TimeChange.Day    = pTimeInfo->tm_mday;
        pInfo->TimeChange.Month  = pTimeInfo->tm_mon;
        pInfo->TimeChange.Year   = pTimeInfo->tm_year;

        pInfo->TimeChange.Second = pTimeInfo->tm_sec;
        pInfo->TimeChange.Minute = pTimeInfo->tm_min;
        pInfo->TimeChange.Hour   = pTimeInfo->tm_hour;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_ChangeDir
 *
 *  @Description:: Change directory
 *
 *  @Input      ::
 *      pDirName:   Pointer to path and directory name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_ChangeDir(const char *pDirName)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLen = AmbaUtility_StringLength(pDirName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pDirName, AmbaFilePath);

    if (Rval == OK) {
        INT32 RvalInt = chdir(AmbaFilePath);
        if (RvalInt != 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_MakeDir
 *
 *  @Description:: Creates a directory
 *
 *  @Input      ::
 *      pDirName:   Pointer to path and directory name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_MakeDir(const char *pDirName)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLen = AmbaUtility_StringLength(pDirName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pDirName, AmbaFilePath);

    if (Rval == OK) {
        INT32 RvalInt = mkdir(AmbaFilePath, S_IRWXU | S_IRWXG | S_IRWXO);
        if (RvalInt != 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_RemoveDir
 *
 *  @Description:: Delete directory
 *
 *  @Input      ::
 *      pDirName:   Pointer to path and directory name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_RemoveDir(const char *pDirName)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLen = AmbaUtility_StringLength(pDirName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pDirName, AmbaFilePath);

    if (Rval == OK) {
        INT32 RvalInt = rmdir(AmbaFilePath);
        if (RvalInt != 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_OpenDir
 *
 *  @Description:: opens a directory stream for the directory specified by 'pDirName'.
 *
 *  @Input      ::
 *      pDirName:   Pointer to path and directory name
 *
 *  @Output     ::
 *      pDelCount:  A pointer to the PF_DIR structure assocated to the stream
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_OpenDir(const char *pDirName, AMBA_FS_DIR **pDir)
{
    UINT32 Rval = 0;

    UINT32 PathLen = AmbaUtility_StringLength(pDirName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pDirName, AmbaFilePath);

    if (Rval == OK) {
        (*pDir) = opendir(AmbaFilePath);
        if ((*pDir) == NULL) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_CloseDir
 *
 *  @Description:: closes the directory specified by 'pDir'.
 *
 *  @Input      ::
 *      pDir:   Pointer to directory
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_CloseDir(AMBA_FS_DIR* pDir)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalInt = closedir(pDir);
    if (RvalInt != 0) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_ReadDir
 *
 *  @Description:: reads a directory stream for the directory specified by 'pDir'.
 *
 *  @Input      ::
 *      pDir:        Pointer to directory
 *      pDirEntry:   Pointer to directory Entry
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_ReadDir(AMBA_FS_DIR* pDir, AMBA_FS_DIRENT* pDirEntry)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    pDirEntry = readdir(pDir);
    if (pDirEntry == NULL) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_TellDir
 *
 *  @Description:: gets the directory position.
 *
 *  @Input      ::
 *      pDir:    Pointer to directory
 *      pOffset: Pointer to directory offset
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_TellDir(AMBA_FS_DIR* pDir, INT32* pOffset)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    (*pOffset) = telldir(pDir);
    if ((*pOffset) == -1) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_SeekDir
 *
 *  @Description:: changes the directory I/O position.
 *
 *  @Input      ::
 *      pDir:    Pointer to directory
 *      Offset:  directory offset
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_SeekDir(AMBA_FS_DIR* pDir, INT32 Offset)
{
    seekdir(pDir, Offset);

    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_RewindDir
 *
 *  @Description:: initializes the directory I/O position.
 *
 *  @Input      ::
 *      pDir:    Pointer to directory
 *
 *  @Output     ::
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_RewindDir(AMBA_FS_DIR* pDir)
{
    rewinddir(pDir);

    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_Sync
 *
 *  @Description:: Writes data in the cache for the specified Drive to device
 *
 *  @Input      ::
 *      Drive:  Drive name to be specified (example: 'A')
 *      Mode:   Mode to specify if invalidate cache or not.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(others)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_Sync(char Drive, INT32 Mode)
{
    (void) Drive;
    (void) Mode;

    sync();

    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileSync
 *
 *  @Description:: Writes data in the cache for the specified Drive to device
 *
 *  @Input      ::
 *      Drive:  Drive name to be specified (example: 'A')
 *      Mode:   Mode to specify if invalidate cache or not.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(Others)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileSync(AMBA_FS_FILE* pFile)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalInt = fflush(pFile);
    if (RvalInt != 0) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_Mount
 *
 *  @Description:: Mount drive
 *
 *  @Input      ::
 *      Drive:  Drive name to be specified (example: 'A')
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_Mount(char Drive)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalInt;

    switch (Drive) {
    case 'c':
    case 'C':
        RvalInt = system("mount -t dos -o exe=all /dev/hd0t* /tmp/SD0");
        break;
    case 'd':
    case 'D':
        RvalInt = system("mount -t dos -o exe=all /dev/hd0t* /tmp/SD1");
        break;
    default:
        RvalInt = -1;
        break;
    }

    if (RvalInt != 0) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_UnMount
 *
 *  @Description:: Release a mounted drive
 *
 *  @Input      ::
 *      Drive:  Drive name to be specified (example: 'A')
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_UnMount(char Drive)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32 RvalInt;

    switch (Drive) {
    case 'c':
    case 'C':
        RvalInt = umount("/tmp/SD0", _MOUNT_FORCE);
        break;
    case 'd':
    case 'D':
        RvalInt = umount("/tmp/SD1", _MOUNT_FORCE);
        break;
    default:
        RvalInt = -1;
        break;
    }

    if (RvalInt != 0) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_GetError
 *
 *  @Description:: Get file access error.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_GetError(void)
{
    return (FS_ERR_BASE | (UINT32)errno);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_GetFileError
 *
 *  @Description:: Check error indicator
 *
 *  @Input      ::
 *      pFile:  Pointer to file descriptor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_GetFileError(AMBA_FS_FILE *pFile)
{
    (void) pFile;
    return (FS_ERR_BASE | (UINT32)errno);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_GetDriveInfo
 *
 *  @Description:: Get Drive information
 *
 *  @Input      ::
 *      Drive:      Drive name to be specified (example: 'A')
 *
 *  @Output     ::
 *      pDriveInfo: Address to store the device capacity informaiton
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_GetDriveInfo(char Drive, AMBA_FS_DRIVE_INFO_t *pDriveInfo)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32 RvalInt;
    struct statvfs64 Info;

    switch (Drive) {
    case 'a':
    case 'A':
        RvalInt = statvfs64(AMBAFS_DRIVE_A_POSIX, &Info);
        break;
    case 'b':
    case 'B':
        RvalInt = statvfs64(AMBAFS_DRIVE_B_POSIX, &Info);
        break;
    case 'c':
    case 'C':
        RvalInt = statvfs64(AMBAFS_DRIVE_C_POSIX, &Info);
        break;
    case 'd':
    case 'D':
        RvalInt = statvfs64(AMBAFS_DRIVE_D_POSIX, &Info);
        break;
    default:
        RvalInt = -1;
        break;
    }

    if (RvalInt != 0) {
        Rval = AMBA_FS_ERR_API;
    } else {
        pDriveInfo->TotalClusters      = Info.f_blocks;
        pDriveInfo->EmptyClusters      = Info.f_bfree;
        pDriveInfo->BytesPerSector     = 1U;
        pDriveInfo->SectorsPerCluster  = Info.f_bsize;
        pDriveInfo->ClustersPerGroup   = Info.f_blocks;
        pDriveInfo->EmptyClusterGroups = 1U;
        pDriveInfo->FormatType         = Info.f_fsid;
        pDriveInfo->Attr               = Info.f_flag;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_GetTimeStamp
 *
 *  @Description:: Get timestamp information.
 *
 *  @Input   ::
 *   pFileName:   Pointer to file name
 *   pTimestamp:  Pointer to timestamp information
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       int : OK(0)/NG(-1)
 *-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_GetTimeStamp(const char *pFileName, AMBA_FS_TIMESTMP_s *pTimestamp)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    struct stat FileStat;
    struct tm *pTimeInfo;

    UINT32 PathLen = AmbaUtility_StringLength(pFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pFileName, AmbaFilePath);

    if (Rval == OK) {
        if (lstat(AmbaFilePath, &FileStat) == 0) {
            Rval = OK;
        } else {
            Rval = AMBA_FS_ERR_API;
        }
    }

    if (Rval == OK) {
        pTimeInfo = localtime(&FileStat.st_atime);

        pTimestamp->AccessDate.SysDay   = pTimeInfo->tm_mday;
        pTimestamp->AccessDate.SysMonth = pTimeInfo->tm_mon;
        pTimestamp->AccessDate.SysYear  = pTimeInfo->tm_year;

        pTimestamp->AccessTime.SysHour = pTimeInfo->tm_year;
        pTimestamp->AccessTime.SysMin  = pTimeInfo->tm_min;
        pTimestamp->AccessTime.SysSec  = pTimeInfo->tm_sec;
        pTimestamp->AccessTime.SysUtc  = 0;
        pTimestamp->AccessTime.SysMs   = 0;
        pTimestamp->AccessTime.SysUtcValid = 0;

        pTimeInfo = localtime(&FileStat.st_mtime);


        pTimestamp->ModifiedDate.SysDay   = pTimeInfo->tm_mday;
        pTimestamp->ModifiedDate.SysMonth = pTimeInfo->tm_mon;
        pTimestamp->ModifiedDate.SysYear  = pTimeInfo->tm_year;

        pTimestamp->ModifiedTime.SysHour = pTimeInfo->tm_year;
        pTimestamp->ModifiedTime.SysMin  = pTimeInfo->tm_min;
        pTimestamp->ModifiedTime.SysSec  = pTimeInfo->tm_sec;
        pTimestamp->ModifiedTime.SysMs   = 0;
        pTimestamp->ModifiedTime.SysUtc  = 0;
        pTimestamp->ModifiedTime.SysUtcValid = 0;

        pTimeInfo = localtime(&FileStat.st_ctime);

        pTimestamp->CreateDate.SysDay   = pTimeInfo->tm_mday;
        pTimestamp->CreateDate.SysMonth = pTimeInfo->tm_mon;
        pTimestamp->CreateDate.SysYear  = pTimeInfo->tm_year;

        pTimestamp->Createtime.SysHour     = pTimeInfo->tm_hour;
        pTimestamp->Createtime.SysMin      = pTimeInfo->tm_min;
        pTimestamp->Createtime.SysSec      = pTimeInfo->tm_sec;
        pTimestamp->Createtime.SysMs       = 0;
        pTimestamp->Createtime.SysUtc      = 0;
        pTimestamp->Createtime.SysUtcValid = 0;

    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_SetTimeStamp
 *
 *  @Description:: Set timestamp information.
 *
 *  @Input   ::
 *   pFileName:   Pointer to path and file name
 *   pTimestamp:  Pointer to timestamp information
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       int : OK(0)/NG(-1)
 *-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_SetTimeStamp(const char *pFileName, const AMBA_FS_TIMESTMP_s *pTimestamp)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32 RvalInt;

    struct utimbuf Times;
    struct tm ActTimeInfo = {0};
    struct tm ModTimeInfo = {0};

    ActTimeInfo.tm_sec = pTimestamp->AccessTime.SysSec;
    ActTimeInfo.tm_min = pTimestamp->AccessTime.SysMin;
    ActTimeInfo.tm_hour = pTimestamp->AccessTime.SysHour;

    ActTimeInfo.tm_mday = pTimestamp->AccessDate.SysDay;
    ActTimeInfo.tm_mon  = pTimestamp->AccessDate.SysMonth;
    ActTimeInfo.tm_year = pTimestamp->AccessDate.SysYear;

    ModTimeInfo.tm_sec = pTimestamp->ModifiedTime.SysSec;
    ModTimeInfo.tm_min = pTimestamp->ModifiedTime.SysMin;

    ModTimeInfo.tm_hour = pTimestamp->ModifiedTime.SysHour;
    ModTimeInfo.tm_mday = pTimestamp->ModifiedDate.SysDay;

    ModTimeInfo.tm_mon  = pTimestamp->ModifiedDate.SysMonth;
    ModTimeInfo.tm_year = pTimestamp->ModifiedDate.SysYear;

    Times.actime  = mktime(&ActTimeInfo);
    Times.modtime = mktime(&ModTimeInfo);

    UINT32 PathLen = AmbaUtility_StringLength(pFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pFileName, AmbaFilePath);

    if (Rval == OK) {
        RvalInt = utime(AmbaFilePath, &Times);
        if (RvalInt < 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileCombine
 *
 *  @Description:: Combines two files into one file
 *
 *  @Input      ::
 *      pHeadFileName:  Pointer to the path name of the first half file after combination
 *      pTailFileName:  Pointer to the path name of the second half file after combination
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileCombine(const char *pHeadFileName, const char *pTailFileName)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLenSrc = AmbaUtility_StringLength(pHeadFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaPathHead[PathLenSrc];

    UINT32 PathLenDst = AmbaUtility_StringLength(pTailFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaPathTail[PathLenDst];

    Rval = AmbaFS_DriveToPath(pHeadFileName, AmbaPathHead);
    if (Rval == OK) {
        Rval = AmbaFS_DriveToPath(pTailFileName, AmbaPathTail);
    }

    if (Rval == OK) {
        INT32 RvalInt = execlp("cat", AmbaPathHead, AmbaPathTail, ">", AmbaPathHead, NULL);
        if (RvalInt < 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileDivide
 *
 *  @Description:: Divides a file in two at a byte boundary.
 *                 The data after Offset of the source file will be moved to the new file.
 *                 And the source file will be trancated to Offset.
 *
 *  @Input      ::
 *      pSrcFileName:   Pointer to the path name of the file to be divided
 *      pNewFileName:   Pointer to the path name of the file created after division
 *      Offset:         Byte offset from the beginning of the file to the division location
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileDivide(const char *pSrcFileName, const char *pNewFileName, UINT64 Offset)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLenSrc = AmbaUtility_StringLength(pSrcFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaPathSrc[PathLenSrc];

    UINT32 PathLenDst = AmbaUtility_StringLength(pNewFileName) + AMBA_PATH_APPEND_SIZE;
    char AmbaPathDst[PathLenDst];

    Rval = AmbaFS_DriveToPath(pSrcFileName, AmbaPathSrc);
    if (Rval == OK) {
        Rval = AmbaFS_DriveToPath(pNewFileName, AmbaPathDst);
    }

    if (Rval == OK) {
        INT32 RvalInt = execlp("split", "-b", Offset, AmbaPathSrc, AmbaPathDst, NULL);
        if (RvalInt < 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_ChangeDirMode
 *
 *  @Description:: Change the attribute of files/directories under the directory.
 *                 NOTE: The attribute of directory specifed with pDirName will also be changed.
 *
 *  @Input      ::
 *      pDirName:   Pointer to path and directory name
 *      pFileName:  Pointer to file name/pattern
 *      Mode:       Search mode, Test mode, Attribute of retrieved files, Attribute comparing mode
 *      Attr:       File attribute, Change attribute mode
 *      pCount:     Pointer to number of change the attribute
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_ChangeDirMode(const char *pDirName, const char *pFileName, UINT32 Mode, UINT32 Attr, UINT32 *pCount)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32 RvalInt;
    char AttrBuffer[32];

    UINT32 PathLength = AmbaUtility_StringLength(pDirName) + AmbaUtility_StringLength(pFileName);
    char PathBuffer[PathLength];

    (void) Mode;
    *pCount = 0;

    /* convert Attr to string. */
    AmbaUtility_UInt32ToStr(AttrBuffer, sizeof(AttrBuffer), Attr, 10);

    /* Cat "pDirName", "pFileName" to complete path */
    AmbaUtility_StringCopy(PathBuffer, sizeof(PathBuffer), pDirName);
    AmbaUtility_StringAppend(PathBuffer, sizeof(PathBuffer), pFileName);

    UINT32 PathLen = AmbaUtility_StringLength(pDirName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(PathBuffer, AmbaFilePath);

    if (Rval == OK) {
        RvalInt = execlp("chmod", "-R", AttrBuffer, AmbaFilePath, NULL);
        if (RvalInt < 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_Format
 *
 *  @Description:: Format devcie
 *
 *  @Input      ::
 *      Drive:  Drive name to be specified (example: 'A')
 *      pParam: Strings of format parameters which seperate by ","
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_Format(char Drive)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32 RvalInt;

    switch (Drive) {
    case 'c':
    case 'C':
        RvalInt = system("mkdosfs -c 32768 /dev/hd0t*");
        break;
    case 'd':
    case 'D':
        RvalInt = system("mkdosfs -c 32768 /dev/hd1t*");
        break;
    default:
        RvalInt = -1;
        break;
    }

    if (RvalInt < 0) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_DeleteDir
 *
 *  @Description:: Delete the directory and all its contents
 *
 *  @Input      ::
 *      pDirName:   Pointer to path and directory name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_DeleteDir(const char *pDirName)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLen = AmbaUtility_StringLength(pDirName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pDirName, AmbaFilePath);

    if (Rval == OK) {
        INT32 RvalInt = execlp("rm", "rm", "-Rf", AmbaFilePath, NULL);
        if (RvalInt < 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_CleanDir
 *
 *  @Description:: Delete all the files and subdirectories under the specified directory
 *                 NOTE: The directory specified with pDirName will not be deleted.
 *
 *  @Input      ::
 *      pDirName:   Pointer to path and directory name
 *
 *  @Output     ::
 *      pDelCount:  Pointer to number of deleted files
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_CleanDir(const char *pDirName)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    UINT32 PathLen = AmbaUtility_StringLength(pDirName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pDirName, AmbaFilePath);

    if (Rval == OK) {
        INT32 RvalInt = execlp("rm", "rm", "-Rf", AmbaFilePath, NULL);
        if (RvalInt < 0) {
            Rval = AMBA_FS_ERR_API;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_SetVolumeLabel
 *
 *  @Description:: Set volume label
 *
 *  @Input      ::
 *      Drive:      Drive name to be specified (example: 'A')
 *      pVolName:   Pointer to volume label name
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_SetVolumeLabel(char Drive, const char *pVolName)
{
    (void) Drive;
    (void) pVolName;

    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_GetVolumeLabel
 *
 *  @Description:: Get volume label
 *
 *  @Input      ::
 *      Drive:      Drive name to be specified (example: 'A')
 *
 *  @Output     ::
 *      pVolInfo:   Address to store the volume label informaiton
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_GetVolumeLabel(char Drive, AMBA_FS_VOL_INFO_t *pVolInfo)
{
    (void) Drive;
    (void) pVolInfo;

    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_RemoveVolumeLabel
 *
 *  @Description:: Remove volume label
 *
 *  @Input      ::
 *      Drive:      Drive name to be specified (example: 'A')
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_RemoveVolumeLabel(char Drive)
{
    (void) Drive;
    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_SetUtcOffsetMinutes
 *
 *  @Description:: Set Offset of UTC in minutes(default 540 -> UTC +9 Japan).
 *
 *  @Input   ::
 *   UtcOffsetMinutes:   UTC offset in minutes
 *
 *  @Output  :: none
 *
 *  @Return  :: OK(0)
 *
 *-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_SetUtcOffsetMinutes(INT32 UtcOffsetMinutes)
{
    (void) UtcOffsetMinutes;
    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_GetUtcOffsetMinutes
 *
 *  @Description:: Get Offset of UTC in minutes.
 *
 *  @Input   :: none
 *
 *  @Output  :: none
 *
 *  @Return  :: UTC offset in minutes
 *
 *-----------------------------------------------------------------------------------------------*/
INT32 AmbaFS_GetUtcOffsetMinutes(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_SetBufferingMode
 *
 *  @Description:: Configure data buffer and directory entry buffer write mode.
 *
 *  @Input      ::
 *      Drive:  Drive name to be specified (example: 'A')
 *      Mode:   (EjectSafe) Write-back/Write-through mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_SetBufferingMode(char Drive, INT32 Mode)
{
    (void) Drive;
    (void) Mode;
    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_ClusterAppend
 *
 *  @Description:: Adds consecutive clusters of the specified size to the end of a file.
 *
 *  @Input      ::
 *      pFile:  Pointer to file descriptor
 *      Size:   Size of the area to be added (in bytes)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      Same value as Size  : Completed successfully (size of the added area)
 *      Different value than Size : Failed to add the area
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_ClusterAppend(AMBA_FS_FILE * pFile, UINT64 Size, UINT64 *pNumSuccess)
{
    (void) pFile;
    (void) Size;
    (void) pNumSuccess;

    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_ClusterTrimTailing
 *
 *  @Description:: Deletes unused clusters at the end of a file.
 *
 *  @Input      ::
 *      pFile:  Pointer to file descriptor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_ClusterTrimTailing(AMBA_FS_FILE * pFile)
{
    (void) pFile;

    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_ClusterInsert
 *
 *  @Description:: Insert number of clusters in a file.
 *
 *  @Input      ::
 *      pFileName:  Pointer to File name.
 *      Offset:     The cluster insert point in the file.
 *      Number:     Insert cluster number.
 *
 *  @Output     ::    none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_ClusterInsert(const char *pFileName, UINT32 Offset, UINT32 Number)
{
    (void) pFileName;
    (void) Offset;
    (void) Number;

    return AMBA_FS_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_ClusterDelete
 *
 *  @Description:: Delete the specified number of clusters from the cluster boundary of a file.
 *
 *  @Input      ::
 *      pFileName:  Pointer to File name.
 *      Offset:     The cluster insert point in the file.
 *      Number:     Insert cluster number.
 *
 *  @Output     ::    none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_ClusterDelete(const char *pFileName, UINT32 Offset, UINT32 Number)
{
    (void) pFileName;
    (void) Offset;
    (void) Number;

    return AMBA_FS_ERR_NONE;
}

static UINT32 GetSearchPattern(char *pSrc, char **pPattern)
{
    char *ptr;
    UINT32 Rval = AMBA_FS_ERR_NONE;

    ptr = strtok(pSrc, "/");

    while (ptr != NULL) {
        ptr = strtok(NULL, "/");
        if (ptr != NULL) {
            *pPattern = ptr;
        }
    }
    if (*pPattern == NULL) {
        Rval = AMBA_FS_ERR_ARG;
    }
    return Rval;
}

static void SetErrno(int err)
{
    errno = err;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileSearchFirst
 *
 *  @Description:: Search for first matching file.
 *
 *  @Input      ::
 *      pDirName:  Pointer to path and file name
 *      Attr:      File attribute
 *      pFileSearch: Pointer to AMBA_FS_DTA_t struture
 *
 *  @Output     ::
 *      pFileSearch:    Pointer to file search informaiton
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileSearchFirst(const char *pName, UINT8 Attr, AMBA_FS_DTA_t *pFileSearch)
{
    INT32 RvalInt = -1;
    UINT32 Rval;

    pFileSearch->attr = Attr;
    pFileSearch->Current_pos   = 0;
    pFileSearch->Glob.gl_pathc = 0;
    pFileSearch->Glob.gl_pathv = NULL;
    pFileSearch->Glob.gl_offs  = 0;

    UINT32 PathLen = AmbaUtility_StringLength(pName) + AMBA_PATH_APPEND_SIZE;
    char AmbaFilePath[PathLen];

    Rval = AmbaFS_DriveToPath(pName, AmbaFilePath);

    if (Rval == OK) {
        RvalInt = glob(AmbaFilePath, GLOB_NOSORT, NULL, &pFileSearch->Glob);
    }

    if (RvalInt == 0) {
        AMBA_FS_FILE_INFO_s Info;
        char *pFileName = basename(pFileSearch->Glob.gl_pathv[pFileSearch->Current_pos]);

        if (AmbaUtility_StringLength(pFileName) > LONG_NAME_BUF_SIZE) {
            Rval = AMBA_FS_ERR_API;
        } else {
            char *pPattern = NULL;

            pFileSearch->parent_start_cluster = AmbaUtility_Crc32((const UINT8 *)AmbaFilePath, AmbaUtility_StringLength(AmbaFilePath));

            if (GetSearchPattern(AmbaFilePath, &pPattern) == AMBA_FS_ERR_NONE) {
                AmbaUtility_StringCopy(pFileSearch->reg_exp, PATH_BUF_SIZE, pPattern);
            }

            AmbaUtility_StringCopy(pFileSearch->LongName, LONG_NAME_BUF_SIZE, pFileName);


            Rval = AmbaFS_GetFileInfo(pFileSearch->Glob.gl_pathv[pFileSearch->Current_pos], &Info);

            pFileSearch->p_vol = (void *)pName[0];

            pFileSearch->Attribute = Info.Attr;
            pFileSearch->FileSize  = Info.Size;

            pFileSearch->Date = Info.TimeModify.Month & 0x1fU;
            pFileSearch->Date |= (Info.TimeModify.Day & 0xfU) << 5U;
            pFileSearch->Date |= (Info.TimeModify.Year & 0x7fU) << 9U;

            pFileSearch->Time = (Info.TimeModify.Second & 0x1fU) << 1U;
            pFileSearch->Time |= (Info.TimeModify.Minute & 0x3fU) << 5U;
            pFileSearch->Time |= (Info.TimeModify.Hour & 0x1fU) << 11U;

            pFileSearch->ADate = Info.TimeAccess.Month & 0x1fU;
            pFileSearch->ADate |= (Info.TimeAccess.Day & 0xfU) << 5U;
            pFileSearch->ADate |= (Info.TimeAccess.Year & 0x7fU) << 9U;

            pFileSearch->CTime = (Info.TimeChange.Second & 0x1fU) << 1U;
            pFileSearch->CTime |= (Info.TimeChange.Minute & 0x3fU) << 5U;
            pFileSearch->CTime |= (Info.TimeChange.Hour & 0x1fU) << 11U;

            pFileSearch->CDate = Info.TimeChange.Month & 0x1fU;
            pFileSearch->CDate |= (Info.TimeChange.Day & 0xfU) << 5U;
            pFileSearch->CDate |= (Info.TimeChange.Year & 0x7fU) << 9U;
        }
        pFileSearch->Current_pos += 1U;
        pFileSearch->parent_pos = pFileSearch->Current_pos + 1U;
    }  else if (RvalInt == GLOB_NOMATCH) {
        errno = 0; // For MisraC
        SetErrno(ENOENT);
        if (errno != 0) {
            // For MisraC
        }
        Rval  = AMBA_FS_ERR_API;
    } else {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileSearchNext
 *
 *  @Description:: Search for the next matching file
 *
 *  @Input      ::
 *      pFileSearch:    Pointer to file search informaiton
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileSearchNext(AMBA_FS_DTA_t *pFileSearch)
{
    UINT32 Rval = AMBA_FS_ERR_API;

    if (pFileSearch->Current_pos < pFileSearch->Glob.gl_pathc) {
        char *pFileName = basename(pFileSearch->Glob.gl_pathv[pFileSearch->Current_pos]);

        if ((AmbaUtility_StringLength(pFileName) > LONG_NAME_BUF_SIZE) ||
            (AmbaUtility_StringLength(pFileName) == 0U)) {
            Rval = AMBA_FS_ERR_API;
        } else {
            AMBA_FS_FILE_INFO_s Info;
            AmbaUtility_StringCopy(pFileSearch->LongName, LONG_NAME_BUF_SIZE, pFileName);


            Rval = AmbaFS_GetFileInfo(pFileSearch->Glob.gl_pathv[pFileSearch->Current_pos], &Info);

            pFileSearch->Attribute = Info.Attr;
            pFileSearch->FileSize = Info.Size;

            pFileSearch->Date = Info.TimeModify.Month & 0x1fU;
            pFileSearch->Date |= (Info.TimeModify.Day & 0xfU) << 5U;
            pFileSearch->Date |= (Info.TimeModify.Year & 0x7fU) << 9U;

            pFileSearch->Time = (Info.TimeModify.Second & 0x1fU) << 1U;
            pFileSearch->Time |= (Info.TimeModify.Minute & 0x3fU) << 5U;
            pFileSearch->Time |= (Info.TimeModify.Hour & 0x1fU) << 11U;

            pFileSearch->ADate = Info.TimeAccess.Month & 0x1fU;
            pFileSearch->ADate |= (Info.TimeAccess.Day & 0xfU) << 5U;
            pFileSearch->ADate |= (Info.TimeAccess.Year & 0x7fU) << 9U;

            pFileSearch->CTime = (Info.TimeChange.Second & 0x1fU) << 1U;
            pFileSearch->CTime |= (Info.TimeChange.Minute & 0x3fU) << 5U;
            pFileSearch->CTime |= (Info.TimeChange.Hour & 0x1fU) << 11U;

            pFileSearch->CDate = Info.TimeChange.Month & 0x1fU;
            pFileSearch->CDate |= (Info.TimeChange.Day & 0xfU) << 5U;
            pFileSearch->CDate |= (Info.TimeChange.Year & 0x7fU) << 9U;
        }
        pFileSearch->Current_pos += 1U;
        pFileSearch->parent_pos = pFileSearch->Current_pos + 1U;
    } else {
        errno = 0; // For MisraC
        SetErrno(ENOENT);
        if (errno != 0) {
            // For MisraC
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileSearchFinish
 *
 *  @Description:: Search for the next matching file
 *
 *  @Input      ::
 *      pFileSearch:    Pointer to file search informaiton
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileSearchFinish(AMBA_FS_DTA_t *pFileSearch)
{
    globfree(&pFileSearch->Glob);

    return AMBA_FS_ERR_NONE;
}

static UINT32 GetOpenMode(UINT8 Attr, char *pMode, UINT32 BufSize)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    switch (Attr) {
    case AMBA_FS_FSEXEC_WRITE:
        AmbaUtility_StringCopy(pMode, BufSize, "w");
        break;
    case AMBA_FS_FSEXEC_READ:
        AmbaUtility_StringCopy(pMode, BufSize, "r");
        break;
    case AMBA_FS_FSEXEC_APPEND:
        AmbaUtility_StringCopy(pMode, BufSize, "w+");
        break;
    case AMBA_FS_FSEXEC_PLUS:
        AmbaUtility_StringCopy(pMode, BufSize, "r+");
        break;
    default:
        Rval = AMBA_FS_ERR_ARG;
        break;
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileSearchExec
 *
 *  @Description:: Use the DTA information to open a file, delete a file or directory,
 *                 or change an attribute.
 *
 *  @Input      ::
 *      pFileSearch:    Pointer to file search informaiton
 *      Operation:      Operation to be executed
 *      Attr:           Attribute to specify to file or directory, Access mode
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_FileSearchExec(AMBA_FS_DTA_t *pFileSearch, INT32 Operation, UINT8 Attr)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    char Mode[8];

    if ((pFileSearch->Glob.gl_pathc == 0U) ||
        (pFileSearch->Current_pos >= pFileSearch->Glob.gl_pathc)) {
        Rval = AMBA_FS_ERR_ARG;
    } else {
        switch (Operation) {
        case AMBA_FS_FSEXEC_REMOVE:
            Rval = AmbaFS_Remove(pFileSearch->Glob.gl_pathv[pFileSearch->Current_pos]);
            break;
        case AMBA_FS_FSEXEC_CHGMOD:
            Rval = AmbaFS_ChangeFileMode(pFileSearch->Glob.gl_pathv[pFileSearch->Current_pos], Attr);
            break;
        case AMBA_FS_FSEXEC_FOPEN:
            Rval = GetOpenMode(Attr, Mode, sizeof(Mode));
            if (Rval == AMBA_FS_ERR_NONE) {
                Rval = AmbaFS_FileOpen(pFileSearch->Glob.gl_pathv[pFileSearch->Current_pos], Mode, &pFileSearch->p_file);
            }
            break;
        case AMBA_FS_FSEXEC_OPENDIR:
            Rval = AmbaFS_OpenDir(pFileSearch->Glob.gl_pathv[pFileSearch->Current_pos], &pFileSearch->p_dir);
            break;

        default:
            Rval = AMBA_FS_ERR_ARG;
            break;
        }
    }

    return Rval;
}

