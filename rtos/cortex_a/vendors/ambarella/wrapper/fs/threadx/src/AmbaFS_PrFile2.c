/**
 *  @file AmbaFS_PrFile2.c
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

#include "AmbaWrap.h"

#include "AmbaFS.h"
#include "AmbaFS_Format.h"
#include "AmbaMisraFix.h"
#include <prfile2/pdm_api.h>

#define AMBA_FS_EXT_DRIVE_MAX_NUM         ((UINT32)3UL)

static AMBA_FS_EXT_DRIVE_f ext_drive_func_array[AMBA_FS_EXT_DRIVE_MAX_NUM] = {NULL, NULL, NULL};

UINT32 AmbaFS_RegisterExtDrive(AMBA_FS_EXT_DRIVE_f ExtDrvFunc)
{
    UINT32 i;
    UINT32 uret = 1;
    for (i = 0; i < AMBA_FS_EXT_DRIVE_MAX_NUM; i++) {
        if (ext_drive_func_array[i] == NULL) {
            ext_drive_func_array[i] = ExtDrvFunc;
            uret = 0;
            break;
        }
    }
    return uret;
}

/*-----------------------------------------------------------------------------------------------*\
 * UTC offset set/get API added by Esol in pf_system.c
\*-----------------------------------------------------------------------------------------------*/
extern INT32 AMBA_get_UtcOffsetMinutes(void);
extern INT32 AMBA_set_UtcOffsetMinutes(INT32 UtcOffsetMinutes);

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
    static PF_DRV_TBL  _AmbaFS_DriveTable[PF_MAX_VOLUME];
    static PF_DRV_TBL* _pAmbaFS_DriveTable[PF_MAX_VOLUME];

    UINT32 Rval = AMBA_FS_ERR_NONE;
    UINT32 NumDrive = 0U;
    INT32 RvalPrf2;

    /* Initialize disk manager */
    RvalPrf2 = pdm_init_diskmanager(0U, NULL);

    if (RvalPrf2 == PDM_OK) {
        /* Initialize PrFILE2 */
        RvalPrf2 = pf2_init_prfile2(0, NULL);

        if (RvalPrf2 == PF_RET_ERR) {
            Rval = AMBA_FS_ERR_API;
        } else {
            /* Register physical devices to the disk manager. Get disk descriptors and their partition descriptors */
            NumDrive += AmbaFS_NandInit(&_AmbaFS_DriveTable[NumDrive]);

            NumDrive += AmbaFS_SdInit(&_AmbaFS_DriveTable[NumDrive]);

            for (UINT32 i = 0; i < AMBA_FS_EXT_DRIVE_MAX_NUM; i++) {
                AMBA_FS_EXT_DRIVE_f ext_drv_func = ext_drive_func_array[i];
                if (ext_drv_func != NULL) {
                    NumDrive += ext_drv_func(&_AmbaFS_DriveTable[NumDrive]);
                }
            }

            for (UINT32 i = 0U; i < NumDrive; i += 1U) {
                _pAmbaFS_DriveTable[i] = &_AmbaFS_DriveTable[i];
            }

            _pAmbaFS_DriveTable[NumDrive] = NULL;

            /* Attach all Drive */
            RvalPrf2 = pf2_attach(_pAmbaFS_DriveTable);
            if (RvalPrf2 == PF_RET_ERR) {
                Rval = AMBA_FS_ERR_API;
            }
        }
    } else {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;
    *pFile = pf2_fopen(pFileName, pMode);
    if ((*pFile) == NULL) {
        Rval = AMBA_FS_ERR_API;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_fclose(pFile);
    if (RvalPrf2 == PF_RET_ERR) {
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
    (*pNumSuccess) = pf2_fread(pBuf, Size, Count, pFile);

    return AMBA_FS_ERR_NONE;
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
    (*pNumSuccess) = pf2_fwrite(pBuf, Size, Count, pFile);

    return AMBA_FS_ERR_NONE;
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
UINT32 AmbaFS_FileGetPos(AMBA_FS_FILE *pFile, UINT64 *pFilePos)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32  RvalPrf2 = pf2_fgetpos(pFile, pFilePos);
    if (RvalPrf2 == PF_RET_ERR) {
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
UINT32 AmbaFS_FileSetPos(AMBA_FS_FILE *pFile, const UINT64 *pFilePos)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32  RvalPrf2 = pf2_fsetpos(pFile, pFilePos);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;
    UINT64 FilePos;

    if (pf2_fgetpos(pFile, &FilePos) == 0) {
        Rval = 0U;
        (*pFilePos) = FilePos;
    } else {
        Rval = AMBA_FS_ERR_API;
    }
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

    INT32 RvalPrf2 = pf2_fseek(pFile, Offset, Origin);
    if (RvalPrf2 == PF_RET_ERR) {
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

    INT32 RvalPrf2 = pf2_feof(pFile);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    } else {
        AmbaMisra_TypeCast32(&Rval, &RvalPrf2);
    }

    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    if (pFile != NULL) {
        INT32 RvalPrf2 = pf2_ferror(pFile);
        if (RvalPrf2 == PF_ERR_SYSTEM) {
            Rval = AMBA_FS_ERR_SYSTEM;
        } else {
            AmbaMisra_TypeCast32(&Rval, &RvalPrf2);
            Rval |= FS_ERR_BASE;
        }
    } else {
        Rval = AMBA_FS_ERR_ARG;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_errnum();
    if (RvalPrf2 == PF_ERR_SYSTEM) {
        Rval = AMBA_FS_ERR_SYSTEM;
    } else {
        AmbaMisra_TypeCast32(&Rval, &RvalPrf2);
        Rval |= FS_ERR_BASE;
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

    INT32 RvalPrf2 = pf2_remove(pFileName);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_rename(pOldName, pNewName);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_move(pSrcName, pDstName);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_combine(pHeadFileName, pTailFileName);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_divide(pSrcFileName, pNewFileName, Offset);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_truncate(pFileName, Offset);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    (*pNumSuccess) = pf2_fappend(pFile, Size);
    if ((*pNumSuccess) != Size) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_fadjust(pFile);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_cinsert(pFileName, Offset, Number);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_cdelete(pFileName, Offset, Number);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_chmod(pFileName, (INT32)Attr);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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
    INT32 RvalPrf2;

    ULONG *pPfCount = NULL;

    AmbaMisra_TypeCast32(&pPfCount, &pCount);

    if (pf2_chmoddir(pDirName, pFileName, Mode, Attr, pPfCount) == 0) {
        INT32 AttrChmod;
        AmbaMisra_TypeCast32(&AttrChmod, &Attr);
        RvalPrf2 = pf2_chdmod(pDirName, AttrChmod);

        if (RvalPrf2 == PF_RET_ERR) {
            Rval = AMBA_FS_ERR_API;
        }
    } else {
        Rval = AMBA_FS_ERR_API;
    }

    AmbaMisra_TypeCast32(&(*pCount), &(*pPfCount));

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

    INT32 RvalPrf2 = pf2_chdmod(pDirName, (INT32)Attr);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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
    PF_STAT FileStat;

    if (pf2_fstat(pName, &FileStat) == 0) {
        pInfo->Size = FileStat.fstfz;
        pInfo->Attr = FileStat.fstat;

        pInfo->TimeModify.Day    = (UINT32)FileStat.fstud & 0x1fU ;
        pInfo->TimeModify.Month  = ((UINT32)FileStat.fstud >> 5U) & 0xfU;;
        pInfo->TimeModify.Year   = ((UINT32)FileStat.fstud >> 9U) & 0x7fU;

        pInfo->TimeModify.Second = ((UINT32)FileStat.fstut & 0x1fU) * 2U;
        pInfo->TimeModify.Minute = ((UINT32)FileStat.fstut >> 5U) & 0x3fU;
        pInfo->TimeModify.Hour   = ((UINT32)FileStat.fstut >> 11U) & 0x1fU;

        pInfo->TimeCreate.Day    = (UINT32)FileStat.fstcd & 0x1fU;
        pInfo->TimeCreate.Month  = ((UINT32)FileStat.fstcd >> 5U) & 0xfU;
        pInfo->TimeCreate.Year   = ((UINT32)FileStat.fstcd >> 9U) & 0x7fU;

        pInfo->TimeCreate.Second = ((UINT32)FileStat.fstct & 0x1fU) * 2U;
        pInfo->TimeCreate.Minute = ((UINT32)FileStat.fstct >> 5U) & 0x3fU;
        pInfo->TimeCreate.Hour   = ((UINT32)FileStat.fstct >> 11U) & 0x1fU;
    } else {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_fsfirst(pName, Attr, pFileSearch);
    if (RvalPrf2 == PF_RET_ERR) {
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_fsnext(pFileSearch);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_FileSearchFinish
 *
 *  @Description:: Finish Search for the next matching file
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
    (void)pFileSearch;
    return AMBA_FS_ERR_NONE;
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

    INT32 RvalPrf2 = pf2_fsexec(pFileSearch, Operation, Attr);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_chdir(pDirName);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_mkdir(pDirName);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_rmdir(pDirName);
    if (RvalPrf2 == PF_RET_ERR) {
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

    INT32 RvalPrf2 = pf2_deletedir(pDirName);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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
    INT32 RvalPrf2;

    RvalPrf2 = pf2_cleandir(pDirName, "*", PF_CLEANDIR_TREE | ATTR_ALL, NULL);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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
    (*pDir) = pf2_opendir(pDirName);
    if ((*pDir) == NULL) {
        Rval = AMBA_FS_ERR_API;
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

    INT32 RvalPrf2 = pf2_closedir(pDir);
    if (RvalPrf2 == PF_RET_ERR) {
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

    INT32 RvalPrf2 = pf2_readdir(pDir, pDirEntry);
    if (RvalPrf2 == PF_RET_ERR) {
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

    INT32 RvalPrf2 = pf2_telldir(pDir, pOffset);
    if (RvalPrf2 == PF_RET_ERR) {
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
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32 RvalPrf2;

    RvalPrf2 = pf2_seekdir(pDir, Offset);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_rewinddir(pDir);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
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
    PF_DEV_INF DevInfo;
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_devinf((char)Drive, &DevInfo);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    } else {
        pDriveInfo->Attr               = DevInfo.attr;
        pDriveInfo->BytesPerSector     = DevInfo.bps;
        pDriveInfo->ClustersPerGroup   = DevInfo.cpg;
        pDriveInfo->EmptyClusterGroups = DevInfo.ecg;
        pDriveInfo->EmptyClusters      = DevInfo.ecl;
        AmbaMisra_TypeCast32(&pDriveInfo->FormatType, &DevInfo.fmt);
        pDriveInfo->SectorsPerCluster  = DevInfo.spc;
        pDriveInfo->TotalClusters      = DevInfo.cls;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_setvol((char)Drive, pVolName);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
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
    PF_VOLTAB VolTab;

    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_getvol((char)Drive, &VolTab);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    } else {
        pVolInfo->VolAttr = VolTab.v_attr;
        pVolInfo->VolDate = VolTab.v_date;
        pVolInfo->VolTime = VolTab.v_time;
        (void)AmbaWrap_memcpy(pVolInfo->VolName, VolTab.v_name, sizeof(pVolInfo->VolName));
    }
    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_rmvvol(Drive);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
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
    /* this function always returns success so that we can ignore its return value */
    (void)AMBA_set_UtcOffsetMinutes(UtcOffsetMinutes);
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
    return AMBA_get_UtcOffsetMinutes();
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
    PF_TIMESTMP Timestamp = {0};
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32 RvalPrf2;

    Timestamp.cre_date.sys_day   = pTimestamp->CreateDate.SysDay;
    Timestamp.cre_date.sys_month = pTimestamp->CreateDate.SysMonth;
    Timestamp.cre_date.sys_year  = pTimestamp->CreateDate.SysYear;

    Timestamp.cre_time.sys_hour      = pTimestamp->Createtime.SysHour;
    Timestamp.cre_time.sys_min       = pTimestamp->Createtime.SysMin;
    Timestamp.cre_time.sys_ms        = pTimestamp->Createtime.SysMs;
    Timestamp.cre_time.sys_sec       = pTimestamp->Createtime.SysSec;
    Timestamp.cre_time.sys_utc       = pTimestamp->Createtime.SysUtc;
    Timestamp.cre_time.sys_utc_valid = pTimestamp->Createtime.SysUtcValid;

    Timestamp.acc_date.sys_day       = pTimestamp->AccessDate.SysDay;
    Timestamp.acc_date.sys_month     = pTimestamp->AccessDate.SysMonth;
    Timestamp.acc_date.sys_year      = pTimestamp->AccessDate.SysYear;

    Timestamp.acc_time.sys_hour      = pTimestamp->AccessTime.SysHour;
    Timestamp.acc_time.sys_min       = pTimestamp->AccessTime.SysMin;
    Timestamp.acc_time.sys_ms        = pTimestamp->AccessTime.SysMs;
    Timestamp.acc_time.sys_sec       = pTimestamp->AccessTime.SysSec;
    Timestamp.acc_time.sys_utc       = pTimestamp->AccessTime.SysUtc;
    Timestamp.acc_time.sys_utc_valid = pTimestamp->AccessTime.SysUtcValid;

    Timestamp.mod_date.sys_day       = pTimestamp->ModifiedDate.SysDay;
    Timestamp.mod_date.sys_month     = pTimestamp->ModifiedDate.SysMonth;
    Timestamp.mod_date.sys_year      = pTimestamp->ModifiedDate.SysYear;

    Timestamp.mod_time.sys_hour      = pTimestamp->ModifiedTime.SysHour;
    Timestamp.mod_time.sys_min       = pTimestamp->ModifiedTime.SysMin;
    Timestamp.mod_time.sys_ms        = pTimestamp->ModifiedTime.SysMs;
    Timestamp.mod_time.sys_sec       = pTimestamp->ModifiedTime.SysSec;
    Timestamp.mod_time.sys_utc       = pTimestamp->ModifiedTime.SysUtc;
    Timestamp.mod_time.sys_utc_valid = pTimestamp->ModifiedTime.SysUtcValid;

    RvalPrf2 = pf2_setstamp(pFileName, &Timestamp);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
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
    PF_TIMESTMP Timestamp;
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_getstamp(pFileName, &Timestamp);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    } else {
        pTimestamp->CreateDate.SysDay   = Timestamp.cre_date.sys_day;
        pTimestamp->CreateDate.SysMonth = Timestamp.cre_date.sys_month;
        pTimestamp->CreateDate.SysYear  = Timestamp.cre_date.sys_year;

        pTimestamp->Createtime.SysHour     = Timestamp.cre_time.sys_hour;
        pTimestamp->Createtime.SysMin      = Timestamp.cre_time.sys_min;
        pTimestamp->Createtime.SysMs       = Timestamp.cre_time.sys_ms;
        pTimestamp->Createtime.SysSec      = Timestamp.cre_time.sys_sec;
        pTimestamp->Createtime.SysUtc      = Timestamp.cre_time.sys_utc;
        pTimestamp->Createtime.SysUtcValid = Timestamp.cre_time.sys_utc_valid;

        pTimestamp->ModifiedDate.SysDay   = Timestamp.mod_date.sys_day;
        pTimestamp->ModifiedDate.SysMonth = Timestamp.mod_date.sys_month;
        pTimestamp->ModifiedDate.SysYear  = Timestamp.mod_date.sys_year;

        pTimestamp->ModifiedTime.SysHour = Timestamp.mod_time.sys_hour;
        pTimestamp->ModifiedTime.SysMin  = Timestamp.mod_time.sys_min;
        pTimestamp->ModifiedTime.SysMs   = Timestamp.mod_time.sys_ms;
        pTimestamp->ModifiedTime.SysSec  = Timestamp.mod_time.sys_sec;
        pTimestamp->ModifiedTime.SysUtc  = Timestamp.mod_time.sys_utc;
        pTimestamp->ModifiedTime.SysUtcValid = Timestamp.mod_time.sys_utc_valid;

        pTimestamp->AccessDate.SysDay   = Timestamp.acc_date.sys_day;
        pTimestamp->AccessDate.SysMonth = Timestamp.acc_date.sys_month;
        pTimestamp->AccessDate.SysYear  = Timestamp.acc_date.sys_year;

        pTimestamp->AccessTime.SysHour = Timestamp.acc_time.sys_hour;
        pTimestamp->AccessTime.SysMin  = Timestamp.acc_time.sys_min;
        pTimestamp->AccessTime.SysMs   = Timestamp.acc_time.sys_ms;
        pTimestamp->AccessTime.SysSec  = Timestamp.acc_time.sys_sec;
        pTimestamp->AccessTime.SysUtc  = Timestamp.acc_time.sys_utc;
        pTimestamp->AccessTime.SysUtcValid = Timestamp.acc_time.sys_utc_valid;
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

    INT32 RvalPrf2 = pf2_format(Drive, "");
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_sync(Drive, Mode);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
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

    INT32 RvalPrf2 = pf2_fsync(pFile);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
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
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf2_buffering(Drive, Mode);
    if (RvalPrf2 == PF_RET_ERR) {
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

    INT32 RvalPrf2 = pf2_mount(Drive);
    if (RvalPrf2 == PF_RET_ERR) {
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

    INT32 RvalPrf2 = pf2_unmount(Drive, PF_UNMOUNT_FORCE);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
}

#if 0
#define AMBA_FS_NUM_DRIVE    (5U)
#define AMBA_FS_SAFE_MAX_BUF (8192U)

static INT32 AmbaFS_FatSafeMalloc(char Drive, PFM_U_LONG Size, PFM_U_LONG **ppBufAddr)
{
    static PFM_U_LONG g_FatSafeBuffer[AMBA_FS_NUM_DRIVE][AMBA_FS_SAFE_MAX_BUF]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    INT32 Rval = 0;
    UINT32 Index;

    AmbaMisra_TouchUnused(&Drive);
    AmbaMisra_TouchUnused(&Size);

    switch (Drive) {
    case 'a':
    case 'A':
        Index = 0;
        break;
    case 'b':
    case 'B':
        Index = 1;
        break;

    case 'c':
    case 'C':
        Index = 2;
        break;
    case 'd':
    case 'D':
        Index = 3;
        break;
    case 'e':
    case 'E':
        Index = 4;
        break;
    default:
        Index = AMBA_FS_NUM_DRIVE;
        break;
    }

    if (Index < AMBA_FS_NUM_DRIVE) {
        (*ppBufAddr) = &g_FatSafeBuffer[Index][0];
    } else {
        Rval = -1;
    }
    return Rval;
}

static INT32 AmbaFS_FatSafeFree(char Drive, PFM_U_LONG *pBufAddr)
{
    AmbaMisra_TouchUnused(&Drive);
    AmbaMisra_TouchUnused(pBufAddr);

    return 0;
}

UINT32 AmbaFS_SafeInit(char Drive)
{
    INT32 RvalPrf2;
    UINT32 Rval = AMBA_FS_ERR_NONE;
    static PF_SAFE_INIT SafeInitParam;

    SafeInitParam.si_malloc = AmbaFS_FatSafeMalloc;
    SafeInitParam.si_free   = AmbaFS_FatSafeFree;

    RvalPrf2 = pf_safe_initialize((char)Drive, &SafeInitParam);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
}

UINT32 AmbaFS_SafeRecover(char Drive)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf_safe_recover((char)Drive);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
}

UINT32 AmbaFS_SafeBackupOn(char Drive)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf_safe_backup_on((char)Drive);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
}

UINT32 AmbaFS_SafeBackupOff(char Drive)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf_safe_backup_off((char)Drive);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
}

UINT32 AmbaFS_SafeFinalize(char Drive)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;

    INT32 RvalPrf2 = pf_safe_finalize((char)Drive);
    if (RvalPrf2 == PF_RET_ERR) {
        Rval = AMBA_FS_ERR_API;
    }
    return Rval;
}

UINT32 AmbaFS_SafeCheck(char Drive, AMBA_FS_SAFE_STAT_s *pSafeStat)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    PF_SAFE_STAT SafeStat;

    if (pSafeStat == NULL) {
        Rval = AMBA_FS_ERR_ARG;
    } else {
        INT32 RvalPrf2 = pf_safe_check((char)Drive, &SafeStat);
        if (RvalPrf2 == PF_RET_ERR) {
            Rval = AMBA_FS_ERR_API;
        } else {
            pSafeStat->ApiCode = SafeStat.ss_apicode;
            AmbaMisra_TypeCast32(&pSafeStat->Stat, &SafeStat.ss_stat);
        }
    }
    return Rval;
}

UINT32 AmbaFS_SafeGetError(char Drive)
{
    UINT32 Rval = AMBA_FS_ERR_NONE;
    INT32 Err = pf_errnum();
    if (Err == PF_ERR_EMOD_SAFE) {
        Err = pf_module_errnum((char)Drive, PF_MODULE_ID_SAFE);
        if (Err != 0) {
            Rval = GetAbsValU32(Err);
            Rval |= (FS_ERR_BASE | FS_SAFE_ERR_BASE);
        }
    } else if (Err == PF_ERR_SYSTEM) {
        Rval = AMBA_FS_ERR_SYSTEM;
    } else {
        AmbaMisra_TypeCast32(&Rval, &Err);
        Rval |= FS_ERR_BASE;
    }
    return Rval;
}
#endif

