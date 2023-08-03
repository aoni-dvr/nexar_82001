/**
*  @file AmbaDCF_Camera.c
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*
*  @details DCF (Design rule for Camera File system v2.0) related APIs
*
*/

#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"

#include "AmbaDCF.h"
#include "AmbaDCF_Camera.h"

#define DEBUG_DCF                   0

#define GET                         0U
#define ADD                         1U

#define AMBA_DCF_MIN_DIR_NO                 100U
#define AMBA_DCF_MAX_DIR_NO                 999U
#define AMBA_DCF_MAX_DIR_AMOUNT             900U
#define AMBA_DCF_MIN_FILE_NO                1U
#define AMBA_DCF_MAX_FILE_NO                9999U
#define AMBA_DCF_MAX_FILE_AMOUNT            9999U

#define AMBA_DCF_DIR_NO_STRLEN              3U
#define AMBA_DCF_DIR_POSTFIX_STRLEN         5U
#define AMBA_DCF_DIR_NAME_STRLEN            8U
#define AMBA_DCF_FILE_NO_STRLEN             4U
#define AMBA_DCF_FILE_PREFIX_STRLEN         4U

#define AMBA_DCF_MAX_DIR_TABLE_SIZE         ((AMBA_DCF_MAX_DIR_NO / 8U) + 1U)
#define AMBA_DCF_MAX_FILE_TABLE_SIZE        ((AMBA_DCF_MAX_FILE_NO / 8U) + 1U)

#define CHECK                               0U
#define SET                                 1U
#define CLEAR                               2U

typedef enum {
    AMBA_DCF_TABLE_TYPE_JPEG = 0,
    AMBA_DCF_TABLE_TYPE_MOV,
    AMBA_DCF_TABLE_TYPE_AUDIO,
    AMBA_DCF_TABLE_TYPE_VALID_FILE,
    AMBA_DCF_TABLE_TYPE_DIR,
    AMBA_DCF_TABLE_TYPE_VALID_DIR
} AMBA_DCF_TABLE_TYPE_e;

typedef struct {
    UINT32 DirNo;
    UINT32 FileNo;
} AMBA_DCF_FILE_ID_s;

typedef struct {
    UINT32 DirNo;
    UINT8  JpegBitTable[AMBA_DCF_MAX_FILE_TABLE_SIZE];
    UINT8  MovBitTable[AMBA_DCF_MAX_FILE_TABLE_SIZE];
    UINT8  AudioBitTable[AMBA_DCF_MAX_FILE_TABLE_SIZE];
    UINT8  ValidFileTable[AMBA_DCF_MAX_FILE_TABLE_SIZE];
    UINT32 TotalJpegAmount;
    UINT32 TotalMovAmount;
    UINT32 TotalAudioAmount;
    UINT32 TotalFileAmount;
} AMBA_DCF_DIR_INFO_s;

typedef struct {
    UINT8  DirBitTable[AMBA_DCF_MAX_DIR_TABLE_SIZE];
    UINT8  ValidDirTable[AMBA_DCF_MAX_DIR_TABLE_SIZE];
    UINT32 DirFileAmount[AMBA_DCF_NUM_FILE_TYPE][AMBA_DCF_MAX_DIR_NO + 1U];
    UINT32 TotalDirAmount;
    char   DirPostfix[AMBA_DCF_MAX_DIR_NO + 1U][AMBA_DCF_DIR_POSTFIX_STRLEN + 1U];

    AMBA_DCF_DIR_INFO_s CurDir;
    AMBA_DCF_DIR_INFO_s PrevDir;
    AMBA_DCF_DIR_INFO_s NextDir;
    AMBA_DCF_DIR_INFO_s TempDir;

    UINT32 TotalJpegAmount;
    UINT32 TotalMovAmount;
    UINT32 TotalAudioAmount;
    UINT32 TotalFileAmount;

    AMBA_DCF_FILE_ID_s CurJpegFileID;
    AMBA_DCF_FILE_ID_s CurMovFileID;
    AMBA_DCF_FILE_ID_s CurAudioFileID;
    AMBA_DCF_FILE_ID_s CurFileID;
    AMBA_DCF_FILE_ID_s MaxFileID;
} AMBA_DCF_ROOT_INFO_s;

typedef enum {
    DIR_TYPE_CUR = 0,
    DIR_TYPE_PREV,
    DIR_TYPE_NEXT,
    DIR_TYPE_TEMP
} DCF_DIR_TYPE_e;

typedef struct {
    char RootDrive[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char RootPath[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char DirPostfix[AMBA_DCF_DIR_POSTFIX_STRLEN + 1U];
    char FilePrefix[AMBA_DCF_FILE_PREFIX_STRLEN + 1U];
} DCF_CONFIG_s;

typedef struct {
    AMBA_KAL_MUTEX_t MutexID;
} AMBA_DCF_INFO_s;

static UINT32 ScanRestDir(UINT8 DriveID, UINT32 LastValidDirNo);
static UINT32 ScanSingleDir(UINT8 DriveID, DCF_DIR_TYPE_e DirType, UINT32 DirNo, UINT32 Action);
static UINT32 CheckDirExist(const char *Path);
static UINT32 MakeDir(const char *Dir);
static UINT32 CreateDir(UINT8 DriveID, UINT32 DirNo);
static UINT32 CheckIllegalDir(const char *DirName);
static UINT32 CheckIllegalFile(const char *FileName);
static UINT32 GetFileType(const char *FileName, AMBA_DCF_FILE_TYPE_e *FileType);
static UINT32 GetFileExt(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 FileNo, char *Ext);
static UINT32 GetFullPath(UINT8 DriveID, AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 DirNo, UINT32 FileNo, char *FileName);
static void GenerateFullPath(UINT8 DriveID, AMBA_DCF_FILE_TYPE_e FileType, UINT32 DirNo, UINT32 FileNo, char *FileName);
static UINT32 FindValidDir(UINT8 DriveID, UINT32 DirNo);
static UINT32 FindNextValidDir(UINT8 DriveID, UINT32 DirNo);
static UINT32 FindValidFile(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 FileNo);
static void UpdateDirForDelete(UINT8 DriveID, UINT32 DirNo);
static UINT32 UpdateCurDir(UINT8 DriveID);
static UINT32 UpdatePrevDir(UINT8 DriveID);
static UINT32 UpdateNextDir(UINT8 DriveID);
static void UpdateCurFileID(UINT8 DriveID);
static void UpdateMaxFileID(UINT8 DriveID);
static UINT32 HandleBitTable(UINT8 *Table, UINT32 No, UINT32 Action);

static UINT32 SvcDcfTable_AddDir(UINT8 DriveID, UINT32 DirNo, const char *DirName);
static UINT32 SvcDcfTable_AddFile(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 FileNo);
static UINT32 SvcDcfTable_DeleteDir(UINT8 DriveID, UINT32 DirNo);
static UINT32 SvcDcfTable_DeleteFile(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 FileNo);
static UINT32 SvcDcfTable_HandleDirTable(UINT8 DriveID, AMBA_DCF_TABLE_TYPE_e TableType, UINT32 DirNo, UINT32 Action);
static UINT32 SvcDcfTable_HandleFileTable(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_TABLE_TYPE_e TableType, UINT32 FileNo, UINT32 Action);

static void NoToText(UINT32 No, char *Text, UINT32 Len);
static size_t DcfCamera_strlen(const char *str);
static void DcfCamera_strcat(char *dest, const char *src);
static void DcfCamera_strcpy(char *dest, const char *src);
static char *DcfCamera_strchr(const char *str, char c);
static void DcfCamera_strncat(char *pBuffer, const char *pSource, UINT32 BufferSize);
static void DcfCamera_strncpy(char *pDest, const char *pSource, size_t DestSize);
static UINT32 StrToUint32(const char *str);
static UINT32 SvcDCF_CameraGetDriveID(UINT8 *pDriveID, const char *pFileName);

static const char *pDefaultDirPostfix     = "MEDIA";
static const char *pDefaultFilePrefix     = "AMBA";
static const char *pSlash                 = "\\";
static char FileExt[3][5]                 = {".JPG",".MP4",".AAC"};
static char Drive[AMBA_DCF_DRIVER_MAX_NUM][2] = {"0","0","0"};

static AMBA_DCF_ROOT_INFO_s RootInfo[AMBA_DCF_DRIVER_MAX_NUM] __attribute__((section(".bss.noinit")));
static DCF_CONFIG_s    DCFConfig[AMBA_DCF_DRIVER_MAX_NUM] __attribute__((section(".bss.noinit")));
static AMBA_DCF_INFO_s      DCFInfo[AMBA_DCF_DRIVER_MAX_NUM] __attribute__((section(".bss.noinit")));
static AMBA_FS_DTA_t   Data __attribute__((section(".bss.noinit")));

static void SvcDcf_CameraNG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#define LOG_BUF_SIZE        512U
#define LOG_ARGC            2U
#define SvcWrap_sprintfStr       AmbaUtility_StringPrintStr

    char        LogBuf[LOG_BUF_SIZE];
    const char  *ArgS[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = SvcWrap_sprintfStr(LogBuf, LOG_BUF_SIZE, "\033""[0;31m[%s|NG]: %s", Argc, ArgS);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
        //AmbaPrint_Flush();
    }
}

static UINT32 SvcDcfTable_AddDir(UINT8 DriveID, UINT32 DirNo, const char *DirName)
{
    UINT32 retval = OK_UINT32;

    if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_DIR, DirNo, CHECK) == 1U) {
        if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_VALID_DIR, DirNo, CHECK) == 1U) {
            if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_VALID_DIR, DirNo, CLEAR) != OK) {
                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
            }
            (RootInfo[DriveID].TotalDirAmount)--;
        }
    } else if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_DIR, DirNo, CHECK) == 0U) {
        if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_DIR, DirNo, SET) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_VALID_DIR, DirNo, SET) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        (RootInfo[DriveID].TotalDirAmount)++;
    } else {
        retval = NG_UINT32;
    }

    if(retval != NG_UINT32) {
        if (DirNo >= AMBA_DCF_MIN_DIR_NO) {
            if (DirNo <= AMBA_DCF_MAX_DIR_NO) {
                DcfCamera_strcpy(RootInfo[DriveID].DirPostfix[DirNo], &DirName[AMBA_DCF_DIR_NO_STRLEN]);
            }
        }
    }

    return retval;
}

static UINT32 SvcDcfTable_AddFile(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 FileNo)
{
    UINT32 retval = OK_UINT32;
    AMBA_DCF_TABLE_TYPE_e TableType = AMBA_DCF_TABLE_TYPE_JPEG;

    if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
        TableType = AMBA_DCF_TABLE_TYPE_JPEG;
    } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
        TableType = AMBA_DCF_TABLE_TYPE_MOV;
    } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
        TableType = AMBA_DCF_TABLE_TYPE_AUDIO;
    } else {
        retval = NG_UINT32;
    }

    #if DEBUG_DCF
    AmbaPrint_PrintUInt5("SvcDcfTable_AddFile FileNo = %d", FileNo, 0U, 0U, 0U, 0U);
    #endif

    if(retval == OK_UINT32) {
        if (SvcDcfTable_HandleFileTable(DirInfo, TableType, FileNo, CHECK) == 1U) {
            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CHECK) == 1U) {
                if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CLEAR) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
                if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
                    (DirInfo->TotalJpegAmount)--;
                } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
                    (DirInfo->TotalMovAmount)--;
                } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
                    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 1U) {
                        (DirInfo->TotalJpegAmount)--;
                    } else {
                        (DirInfo->TotalAudioAmount)--;
                    }
                } else {
                    //
                }
            }
        } else if (SvcDcfTable_HandleFileTable(DirInfo, TableType, FileNo, CHECK) == 0U) {
            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CHECK) == 1U) {
                if (SvcDcfTable_HandleFileTable(DirInfo, TableType, FileNo, SET) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
                if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CLEAR) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
                if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
                    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_MOV, FileNo, CHECK) == 1U) {
                        (DirInfo->TotalMovAmount)--;
                    }

                    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_AUDIO, FileNo, CHECK) == 1U) {
                        (DirInfo->TotalJpegAmount)++;
                        (DirInfo->TotalAudioAmount)--;
                        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, SET) != OK) {
                            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                        }
                    }
                } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
                    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 1U) {
                        (DirInfo->TotalJpegAmount)--;
                    } else {
                        (DirInfo->TotalAudioAmount)--;
                    }
                } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
                    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 1U) {
                        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, SET) != OK) {
                            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                        }
                    }

                    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_MOV, FileNo, CHECK) == 1U) {
                        (DirInfo->TotalMovAmount)--;
                    }
                } else {
                    //
                }
            } else if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CHECK) == 0U) {
                if (SvcDcfTable_HandleFileTable(DirInfo, TableType, FileNo, SET) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
                if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
                    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_MOV, FileNo, CHECK) == 0U) {
                        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_AUDIO, FileNo, CHECK) == 0U) {
                            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, SET) != OK) {
                                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                            }
                            (DirInfo->TotalJpegAmount)++;
                        }
                    }
                } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
                    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 0U) {
                        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_AUDIO, FileNo, CHECK) == 0U) {
                            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, SET) != OK) {
                                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                            }
                            (DirInfo->TotalMovAmount)++;
                        }
                    }
                } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
                    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 0U) {
                        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_MOV, FileNo, CHECK) == 0U) {
                            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, SET) != OK) {
                                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                            }
                            (DirInfo->TotalAudioAmount)++;
                        }
                    }
                } else {
                    //
                }
            } else {
                //
            }
        } else {
            retval = NG_UINT32;
        }
    }

    if(retval == OK_UINT32) {
        DirInfo->TotalFileAmount = DirInfo->TotalJpegAmount + DirInfo->TotalMovAmount + DirInfo->TotalAudioAmount;
    }

#if DEBUG_DCF
    //SvcDcfTable_AddFile retval
#endif

    return retval;
}

static UINT32 SvcDcfTable_DeleteDir(UINT8 DriveID, UINT32 DirNo)
{
    if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_DIR, DirNo, CLEAR) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_VALID_DIR, DirNo, CLEAR) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    (RootInfo[DriveID].TotalDirAmount)--;

    if (AmbaWrap_memset(RootInfo[DriveID].DirPostfix[DirNo], 0, 1) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    return OK_UINT32;
}

static UINT32 SvcDcfTable_DeleteFile(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 FileNo)
{
    UINT32 retval = OK_UINT32;

    if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CLEAR) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_AUDIO, FileNo, CLEAR) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if(DirInfo != NULL) {
            (DirInfo->TotalJpegAmount)--;
        }
    } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_MOV, FileNo, CLEAR) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if(DirInfo != NULL) {
            (DirInfo->TotalMovAmount)--;
        }
    } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 1U) {
            retval = NG_UINT32;
        }
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_AUDIO, FileNo, CLEAR) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if(DirInfo != NULL) {
            (DirInfo->TotalAudioAmount)--;
        }
    } else {
        //
    }

    if(retval == OK_UINT32) {
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CLEAR) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }

        if(DirInfo != NULL) {
            DirInfo->TotalFileAmount = DirInfo->TotalJpegAmount + DirInfo->TotalMovAmount + DirInfo->TotalAudioAmount;
        }
    }

    return retval;
}

static UINT32 SvcDcfTable_HandleDirTable(UINT8 DriveID, AMBA_DCF_TABLE_TYPE_e TableType, UINT32 DirNo, UINT32 Action)
{
    UINT8 *Table;
    UINT32 retval = OK_UINT32;

    if (TableType == AMBA_DCF_TABLE_TYPE_DIR) {
        Table = RootInfo[DriveID].DirBitTable;
    } else if (TableType == AMBA_DCF_TABLE_TYPE_VALID_DIR) {
        Table = RootInfo[DriveID].ValidDirTable;
    } else {
        Table = RootInfo[DriveID].DirBitTable;
        retval = NG_UINT32;
    }

    if(DirNo > AMBA_DCF_MAX_DIR_NO){
        retval = NG_UINT32;
    }

    if(retval != NG_UINT32) {
        retval = HandleBitTable(Table, DirNo, Action);
    }

    return retval;
}

static UINT32 SvcDcfTable_HandleFileTable(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_TABLE_TYPE_e TableType, UINT32 FileNo, UINT32 Action)
{
    UINT8 *Table = NULL;
    UINT32 retval = OK_UINT32;

    if (TableType == AMBA_DCF_TABLE_TYPE_JPEG) {
        if(DirInfo != NULL) {
            Table = DirInfo->JpegBitTable;
        }
    } else if (TableType == AMBA_DCF_TABLE_TYPE_MOV) {
        if(DirInfo != NULL) {
            Table = DirInfo->MovBitTable;
        }
    } else if (TableType == AMBA_DCF_TABLE_TYPE_AUDIO) {
        if(DirInfo != NULL) {
            Table = DirInfo->AudioBitTable;
        }
    } else if (TableType == AMBA_DCF_TABLE_TYPE_VALID_FILE) {
        if(DirInfo != NULL) {
            Table = DirInfo->ValidFileTable;
        }
    } else {
        retval = NG_UINT32;
    }

    if(FileNo > AMBA_DCF_MAX_FILE_NO){
        retval = NG_UINT32;
    }

    if(retval == OK_UINT32) {
        retval = HandleBitTable(Table, FileNo, Action);
    }

    return retval;
}

static UINT32 HandleBitTable(UINT8 *Table, UINT32 No, UINT32 Action)
{
    //0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
    UINT8 Mask[8U];
    UINT32 TableIndex, MaskIndex;
    UINT32 retval = 0U;

    Mask[0] = 0x80;
    Mask[1] = 0x40;
    Mask[2] = 0x20;
    Mask[3] = 0x10;
    Mask[4] = 0x08;
    Mask[5] = 0x04;
    Mask[6] = 0x02;
    Mask[7] = 0x01;

    TableIndex = (No - 1U) >> 3U;
    MaskIndex = (No - 1U) & 7U;

    if (Action == CHECK) {
        if(Table != NULL) {
            if ((Table[TableIndex] & Mask[MaskIndex]) != 0U) {
                retval = 1U;
            }
        }
    } else if (Action == SET) {
        if(Table != NULL) {
            Table[TableIndex] |= Mask[MaskIndex];
        }
    } else if (Action == CLEAR) {
        if(Table != NULL) {
            Table[TableIndex] &= ~Mask[MaskIndex];
        }
    } else {
        retval = 0U;
    }

    return retval;
}

static void NoToText(UINT32 No, char *Text, UINT32 Len)
{
    if (AmbaUtility_UInt32ToStr(Text, (Len+1U), No, 10U) == OK){
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
}

static size_t DcfCamera_strlen(const char *str)
{
    size_t len;
    len = AmbaUtility_StringLength(str);
    return len;
}

static void DcfCamera_strcat(char *dest, const char *src)
{
    UINT32 len1,len2;

    len1 = (UINT32)AmbaUtility_StringLength(src);
    len2 = (UINT32)AmbaUtility_StringLength(dest);

    AmbaUtility_StringAppend(dest, len1+len2+1U, src);
}

static void DcfCamera_strcpy(char *dest, const char *src)
{
    UINT32 len;

    len = (UINT32)AmbaUtility_StringLength(src);
    if(src[len] == '\0') {
       len++;
    }

    AmbaUtility_StringCopy(dest, len, src);
}

static char *DcfCamera_strchr(const char *str, char c)
{
    const char *input_str = NULL;
    char *ret_str = NULL;

    AmbaMisra_TypeCast(&input_str, &str);

    for (; *input_str != '\0'; ++input_str) {
        if (*input_str == c) {
            AmbaMisra_TypeCast(&ret_str, &input_str);
            break;
        }
    }
    return ret_str;
}

static void DcfCamera_strncat(char *pBuffer, const char *pSource, UINT32 BufferSize) {
    AmbaUtility_StringAppend(pBuffer, BufferSize, pSource);
}

static void DcfCamera_strncpy(char *pDest, const char *pSource, size_t DestSize) {
    AmbaUtility_StringCopy(pDest, DestSize, pSource);
}

static UINT32 StrToUint32(const char *str)
{
    UINT32 Value = 0U;

    if (AmbaUtility_StringToUInt32(str, &Value) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    return Value;
}

static UINT32 ScanRestDir(UINT8 DriveID, UINT32 LastValidDirNo)
{
    DCF_DIR_TYPE_e DirType;
    UINT32 DirNo;

    for (DirNo = AMBA_DCF_MIN_DIR_NO; DirNo < LastValidDirNo; DirNo++) {
        if (FindValidDir(DriveID, DirNo) == 1U) {
            /* decide the dir type by dir no */
            if (RootInfo[DriveID].NextDir.DirNo == 0U) {
                DirType = DIR_TYPE_NEXT;
            } else if (FindNextValidDir(DriveID, DirNo) == LastValidDirNo) {
                DirType = DIR_TYPE_PREV;
            } else {
                DirType = DIR_TYPE_TEMP;
            }

            if(ScanSingleDir(DriveID, DirType, DirNo, ADD) != OK) {
                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
            }
        }
    }

    /* let current file id be the last valid file */
    UpdateCurFileID(DriveID);

    return OK_UINT32;
}

static UINT32 ScanSingleDir(UINT8 DriveID, DCF_DIR_TYPE_e DirType, UINT32 DirNo, UINT32 Action)
{
    AMBA_DCF_DIR_INFO_s *DirInfo = NULL;
    char SearchPattern[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    const char FilePattern[] = "\\*.*";
    char FileName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char DirNoStr[AMBA_DCF_DIR_NO_STRLEN + 1U];
    char FileNoStr[AMBA_DCF_FILE_NO_STRLEN + 1U];
    UINT32 FileNo;
    UINT32 RetStatus;
    AMBA_DCF_FILE_TYPE_e FileType;
    INT32 rval = OK_INT32;

    if (AmbaWrap_memset(SearchPattern, 0, sizeof(SearchPattern)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaWrap_memset(FileName, 0, sizeof(FileName)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaWrap_memset(DirNoStr, 0, sizeof(DirNoStr)) != OK){
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    if (DirType == DIR_TYPE_CUR) {
        DirInfo = &RootInfo[DriveID].CurDir;
    } else if (DirType == DIR_TYPE_PREV) {
        DirInfo = &RootInfo[DriveID].PrevDir;
    } else if (DirType == DIR_TYPE_NEXT) {
        DirInfo = &RootInfo[DriveID].NextDir;
    } else if (DirType == DIR_TYPE_TEMP) {
        DirInfo = &RootInfo[DriveID].TempDir;
    } else {
        DirInfo = &RootInfo[DriveID].CurDir;
        rval = NG_INT32;
    }

    /* init setting */
    if (AmbaWrap_memset(DirInfo, 0, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    DirInfo->DirNo = DirNo;

    /* create search pattern to scan file */
    DcfCamera_strcpy(SearchPattern, DCFConfig[DriveID].RootPath);
    DcfCamera_strncat(SearchPattern, pSlash, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);
    NoToText(DirNo, DirNoStr, AMBA_DCF_DIR_NO_STRLEN);
    DcfCamera_strncat(SearchPattern, DirNoStr, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);
    if (RootInfo[DriveID].DirPostfix[DirNo][0] != '\0') {
        DcfCamera_strcat(SearchPattern, RootInfo[DriveID].DirPostfix[DirNo]);
    } else {
        DcfCamera_strcat(SearchPattern, DCFConfig[DriveID].DirPostfix);
    }
    DcfCamera_strncat(SearchPattern, FilePattern, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);

    #if DEBUG_DCF
    AmbaPrint_PrintStr5("ScanSingleDir: Search Pattern = %s", SearchPattern, NULL, NULL, NULL, NULL);
    AmbaPrint_PrintUInt5("ScanSingleDir: DirType = %d, DirNo = %d", (UINT32)DirType, DirNo, 0U, 0U, 0U);
    #endif

    /* main scan flow */
    if (AmbaWrap_memset(&Data, 0, sizeof(AMBA_FS_DTA_t)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    RetStatus = AmbaFS_FileSearchFirst(SearchPattern, (UINT8)(AMBA_FS_ATTR_ARCH | AMBA_FS_ATTR_RDONLY | AMBA_FS_ATTR_NONE | AMBA_FS_ATTR_HIDDEN), &Data);

    if(rval != NG_INT32) {
        if (RetStatus == AMBA_FS_ERR_API) {
            if ((Action == ADD) && (DirType == DIR_TYPE_CUR)) {
                RootInfo[DriveID].MaxFileID.DirNo = DirNo;
                RootInfo[DriveID].MaxFileID.FileNo = 0;
            }
            rval = NG_INT32;
        }
    }

    if(rval != NG_INT32) {
        while (RetStatus != AMBA_FS_ERR_API) {
            /* get filename */
            if ((char)Data.LongName[0] != '\0') {
                if (DcfCamera_strlen((const char *)Data.LongName) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                    DcfCamera_strncpy(FileName, (const char *)Data.LongName, AMBA_DCF_MAX_FILE_FULL_PATH_LEN - 1U);
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "illegal Data.LongName = %s", (const char *)Data.LongName, NULL, NULL, NULL, NULL);
                    rval = NG_INT32;
                }
            } else {
                if (DcfCamera_strlen((const char *)Data.FileName) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                    DcfCamera_strncpy(FileName, (const char *)Data.FileName, AMBA_DCF_MAX_FILE_FULL_PATH_LEN - 1U);
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "illegal Data.FileName = %s", (const char *)Data.FileName, NULL, NULL, NULL, NULL);
                    rval = NG_INT32;
                }
            }

            #if DEBUG_DCF
            AmbaPrint_PrintStr5("FileName = %s", FileName, NULL, NULL, NULL, NULL);
            #endif

            if(rval != NG_INT32) {
                /* filter out illegal file name */
                if (CheckIllegalFile(FileName) != OK_UINT32) {
                    #if DEBUG_DCF
                    AmbaPrint_PrintStr5("Illegal FileName = %s", FileName, NULL, NULL, NULL, NULL);
                    #endif
                    RetStatus = AmbaFS_FileSearchNext(&Data);
                    continue;
                }

                /* filter out illegal file no */
                DcfCamera_strncpy(FileNoStr, &FileName[AMBA_DCF_FILE_PREFIX_STRLEN], sizeof(FileNoStr));
                FileNo = StrToUint32(FileNoStr);
                #if DEBUG_DCF
                AmbaPrint_PrintUInt5("FileNo = %d", FileNo, 0U, 0U, 0U, 0U);
                #endif
                if ((FileNo < AMBA_DCF_MIN_FILE_NO) || (FileNo > AMBA_DCF_MAX_FILE_NO)) {
                    #if DEBUG_DCF
                    AmbaPrint_PrintUInt5("illegal FileNo = %d", FileNo, 0U, 0U, 0U, 0U);
                    #endif
                    RetStatus = AmbaFS_FileSearchNext(&Data);
                    continue;
                }

                /* save max file id */
                if ((Action == ADD) && (DirType == DIR_TYPE_CUR)) {
                    if (FileNo > RootInfo[DriveID].MaxFileID.FileNo) {
                        RootInfo[DriveID].MaxFileID.DirNo = DirNo;
                        RootInfo[DriveID].MaxFileID.FileNo = FileNo;
                    }
                }

                /* skip hidden file */
#if AMBA_FS_ATTR_HIDDEN != 0U
                if((Data.Attribute & AMBA_FS_ATTR_HIDDEN) != 0U) {
                    RetStatus = AmbaFS_FileSearchNext(&Data);
                    continue;
                }
#endif

                /* set bit table and file amount by type */
                if (GetFileType(FileName, &FileType) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
                if (SvcDcfTable_AddFile(DirInfo, FileType, FileNo) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }

                /* check total file amount */
                if (DirInfo->TotalFileAmount >= AMBA_DCF_MAX_FILE_AMOUNT) {
                    break;
                }

                RetStatus = AmbaFS_FileSearchNext(&Data);
            }
        }
    }

    if(rval != NG_INT32) {
        /* save total file amount */
        if (Action == ADD) {
            RootInfo[DriveID].TotalJpegAmount  += DirInfo->TotalJpegAmount;
            RootInfo[DriveID].TotalMovAmount   += DirInfo->TotalMovAmount;
            RootInfo[DriveID].TotalAudioAmount += DirInfo->TotalAudioAmount;
            RootInfo[DriveID].TotalFileAmount  += DirInfo->TotalFileAmount;
            RootInfo[DriveID].DirFileAmount[AMBA_DCF_FILE_TYPE_IMAGE][DirNo] = DirInfo->TotalJpegAmount;
            RootInfo[DriveID].DirFileAmount[AMBA_DCF_FILE_TYPE_VIDEO][DirNo] = DirInfo->TotalMovAmount;
            RootInfo[DriveID].DirFileAmount[AMBA_DCF_FILE_TYPE_AUDIO][DirNo] = DirInfo->TotalAudioAmount;
            RootInfo[DriveID].DirFileAmount[AMBA_DCF_FILE_TYPE_ALL][DirNo]   = DirInfo->TotalFileAmount;
        }
    }

    #if DEBUG_DCF
    AmbaPrint_PrintUInt5("TotalJpegAmount%d TotalMovAmount%d TotalAudioAmount%d TotalFileAmount%d", DirInfo->TotalJpegAmount, DirInfo->TotalMovAmount, DirInfo->TotalAudioAmount, DirInfo->TotalFileAmount, 0U);
    #endif

    return OK_UINT32;
}

static UINT32 CheckDirExist(const char *Path)
{
    AMBA_FS_FILE_INFO_s Status;
    UINT32 rval;

    if (AmbaFS_GetFileInfo(Path, &Status) == OK_UINT32) {
        if ((Status.Attr & (UINT32)AMBA_FS_ATTR_DIR) != 0U) {
            rval = OK_UINT32;
        } else {
            rval = NG_UINT32;
        }
    } else {
        rval = NG_UINT32;
    }

    return rval;
}

static UINT32 MakeDir(const char *Dir)
{
    UINT32 rval;

    rval = AmbaFS_MakeDir(Dir);
    if(rval != OK_UINT32) {
        AmbaPrint_ModulePrintUInt5(AMBA_DCF_PRINT_MODULE_ID, "MakeDir fail %d", rval, 0U, 0U, 0U, 0U);
    }

    return rval;
}

static UINT32 CreateDir(UINT8 DriveID, UINT32 DirNo)
{
    char DirPath[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char DirNoStr[AMBA_DCF_DIR_NO_STRLEN + 1U];
    UINT32 rval = OK_INT32;

    if (AmbaWrap_memset(DirNoStr, 0, sizeof(DirNoStr)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    DcfCamera_strcpy(DirPath, DCFConfig[DriveID].RootPath);
    if (DcfCamera_strlen(DirPath) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
        if (DcfCamera_strlen(pSlash) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
            if (DcfCamera_strlen(DCFConfig[DriveID].DirPostfix) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                DcfCamera_strcat(DirPath, pSlash);
                NoToText(DirNo, DirNoStr, AMBA_DCF_DIR_NO_STRLEN);
                DcfCamera_strncat(DirPath, DirNoStr, (UINT32)(sizeof(DirPath)-DcfCamera_strlen(DirPath))-1U);
                DcfCamera_strncat(DirPath, DCFConfig[DriveID].DirPostfix, (UINT32)(sizeof(DirPath)-DcfCamera_strlen(DirPath))-1U);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Illegal DirNoStr = %s, DCFConfig[DriveID].DirPostfix = %s", DirNoStr, DCFConfig[DriveID].DirPostfix, NULL, NULL, NULL);
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        if(AmbaFS_MakeDir(DirPath) == OK_UINT32) {
            rval = OK_UINT32;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "AmbaFS_MakeDir fail %s", DirPath, NULL, NULL, NULL, NULL);
            rval = NG_UINT32;
        }
    }

    return rval;
}

static UINT32 CheckIllegalDir(const char *DirName)
{
    UINT32 i;
    UINT32 rval = OK_UINT32;

    /* check dir length */
    if (DcfCamera_strlen(DirName) != AMBA_DCF_DIR_NAME_STRLEN) {
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        /* check dir char */
        for (i = 0U; i < AMBA_DCF_DIR_NO_STRLEN; i++) {
            if ((DirName[i] >= '0') && (DirName[i] <= '9')) {
                /* ok char */
            } else {
                rval = NG_UINT32;
            }
        }
    }

    if(rval != NG_UINT32) {
        //DirName
        for (i = AMBA_DCF_DIR_NO_STRLEN; i < AMBA_DCF_DIR_NAME_STRLEN; i++) {
            if (((DirName[i] >= '0') && (DirName[i] <= '9')) || ((DirName[i] >= 'A') && (DirName[i] <= 'Z')) ||
                ((DirName[i] >= 'a') && (DirName[i] <= 'z')) || (DirName[i] == '_')) {
                /* ok char */
            } else {
                rval = NG_UINT32;
            }
        }
    }

    return rval;
}

static UINT32 CheckIllegalFile(const char *FileName)
{
    AMBA_DCF_FILE_TYPE_e FileType;
    UINT32 i;
    UINT32 rval = OK_UINT32;

    /* check file length */
    if (DcfCamera_strlen(FileName) != 12U) {
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        /* check file ext */
        if (GetFileType(FileName, &FileType) == NG_UINT32) {
            rval = NG_UINT32;
        }
    }

    if(rval != NG_UINT32) {
        /* check file char */
        for (i = 0U; i < AMBA_DCF_FILE_PREFIX_STRLEN; i++) {
            if (((FileName[i] >= '0') && (FileName[i] <= '9')) || ((FileName[i] >= 'A') && (FileName[i] <= 'Z')) ||
                ((FileName[i] >= 'a') && (FileName[i] <= 'z')) || (FileName[i] == '_')) {
                if ((FileName[AMBA_DCF_FILE_PREFIX_STRLEN + i] >= '0') && (FileName[AMBA_DCF_FILE_PREFIX_STRLEN + i] <= '9')) {
                    /* ok char */
                } else {
                    rval = NG_UINT32;
                }
            } else {
                rval = NG_UINT32;
            }
        }
    }

    return rval;
}

static UINT32 GetFileType(const char *FileName, AMBA_DCF_FILE_TYPE_e *FileType)
{
    const char *Ext;
    UINT32 rval = OK_UINT32;

    Ext = DcfCamera_strchr(FileName, '.');
    if (Ext == NULL) {
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        if (AmbaUtility_StringCompare(Ext, FileExt[AMBA_DCF_FILE_TYPE_IMAGE], 3) == OK_INT32) {
            *FileType = AMBA_DCF_FILE_TYPE_IMAGE;
        } else if (AmbaUtility_StringCompare(Ext, FileExt[AMBA_DCF_FILE_TYPE_VIDEO], 3) == OK_INT32) {
            *FileType = AMBA_DCF_FILE_TYPE_VIDEO;
        } else if (AmbaUtility_StringCompare(Ext, FileExt[AMBA_DCF_FILE_TYPE_AUDIO], 3) == OK_INT32) {
            *FileType = AMBA_DCF_FILE_TYPE_AUDIO;
        } else {
            rval = NG_UINT32;
        }
    }

    return rval;
}

static UINT32 GetFileExt(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 FileNo, char *Ext)
{
    UINT32 rval = OK_UINT32;

    if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CHECK) == 1U) {
        if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
            DcfCamera_strcpy(Ext, FileExt[AMBA_DCF_FILE_TYPE_IMAGE]);
        } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
            DcfCamera_strcpy(Ext, FileExt[AMBA_DCF_FILE_TYPE_VIDEO]);
        } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
            DcfCamera_strcpy(Ext, FileExt[AMBA_DCF_FILE_TYPE_AUDIO]);
        } else if (FileType == AMBA_DCF_FILE_TYPE_ALL) {
            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 1U) {
                DcfCamera_strcpy(Ext, FileExt[AMBA_DCF_FILE_TYPE_IMAGE]);
            } else if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_MOV, FileNo, CHECK) == 1U) {
                DcfCamera_strcpy(Ext, FileExt[AMBA_DCF_FILE_TYPE_VIDEO]);
            } else if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_AUDIO, FileNo, CHECK) == 1U) {
                if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 0U) {
                    DcfCamera_strcpy(Ext, FileExt[AMBA_DCF_FILE_TYPE_AUDIO]);
                }
            } else {
                //
            }
        } else {
            rval = NG_UINT32;
        }
    } else {
        rval = NG_UINT32;
    }

    return rval;
}

static UINT32 GetFullPath(UINT8 DriveID, AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 DirNo, UINT32 FileNo, char *FileName)
{
    char SearchPattern[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char DirPath[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char DirNoStr[AMBA_DCF_DIR_NAME_STRLEN];
    char FileNoStr[AMBA_DCF_DIR_NAME_STRLEN];
    char FilePrefixPattern[4];//????
    char Ext[5];
    UINT32 RetStatus;
    UINT32 rval = OK_UINT32;

    if (AmbaWrap_memset(SearchPattern, 0, sizeof(SearchPattern)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaWrap_memset(DirPath, 0, sizeof(DirPath)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaWrap_memset(DirNoStr, 0, sizeof(DirNoStr)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaWrap_memset(FileNoStr, 0, sizeof(FileNoStr)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaWrap_memset(Ext, 0, sizeof(Ext)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaUtility_MemorySetChar(FilePrefixPattern, '?', 4U);

    /* create search pattern */
    if (DcfCamera_strlen(SearchPattern) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
        if(DcfCamera_strlen(DCFConfig[DriveID].RootPath) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
            if (DcfCamera_strlen(pSlash) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                if (DcfCamera_strlen(RootInfo[DriveID].DirPostfix[DirNo]) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                    if (DcfCamera_strlen(FilePrefixPattern) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                        DcfCamera_strncpy(SearchPattern, DCFConfig[DriveID].RootPath, sizeof(SearchPattern));
                        DcfCamera_strncat(SearchPattern, pSlash, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);
                        NoToText(DirNo, DirNoStr, (UINT32)AMBA_DCF_DIR_NO_STRLEN);
                        DcfCamera_strncat(SearchPattern, DirNoStr, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);
                        DcfCamera_strncat(SearchPattern, RootInfo[DriveID].DirPostfix[DirNo], (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);
                        DcfCamera_strncat(SearchPattern, pSlash, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);
                        DcfCamera_strncpy(DirPath, SearchPattern, (UINT32)sizeof(SearchPattern));
                        DcfCamera_strncat(SearchPattern, FilePrefixPattern, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);
                        if (AmbaUtility_StringPrintUInt32(FileNoStr, (UINT32)sizeof(FileNoStr), "%04u", 1, &FileNo) != 4U) {
                            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                        }
                        DcfCamera_strncat(SearchPattern, FileNoStr, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);
                        if (GetFileExt(DirInfo, FileType, FileNo, Ext) != OK) {
                            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                        }
                        DcfCamera_strncat(SearchPattern, Ext, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);
                    }
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "DirNoStr %s, RootInfo[DriveID].DirPostfix[DirNo] = %s, FileNoStr = %s, Ext = %s", DirNoStr, RootInfo[DriveID].DirPostfix[DirNo], FileNoStr, Ext, NULL);
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        if (AmbaWrap_memset(&Data, 0, sizeof(AMBA_FS_DTA_t)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        RetStatus = AmbaFS_FileSearchFirst(SearchPattern, (UINT8)(AMBA_FS_ATTR_ARCH | AMBA_FS_ATTR_RDONLY | AMBA_FS_ATTR_NONE), &Data);
        if (RetStatus == AMBA_FS_ERR_API) {
            AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "SearchPattern %s fail", SearchPattern, NULL, NULL, NULL, NULL);
            rval = NG_UINT32;
        }
    }

    if(rval != NG_UINT32) {
        /* get filename */
        DcfCamera_strcpy(FileName, DirPath);
        if ((char)Data.LongName[0] != '\0') {
            DcfCamera_strcat(FileName, (const char *)Data.LongName);
        } else {
            DcfCamera_strcat(FileName, (const char *)Data.FileName);
        }
    }

    #if DEBUG_DCF
    AmbaPrint_PrintUInt5("GetFullPath DirNo %d FileNo %d", DirNo, FileNo, 0U, 0U, 0U);
    AmbaPrint_PrintStr5("FileName %s", FileName, NULL, NULL, NULL, NULL);
    #endif

    return rval;
}

static void GenerateFullPath(UINT8 DriveID, AMBA_DCF_FILE_TYPE_e FileType, UINT32 DirNo, UINT32 FileNo, char *FileName)
{
    char DirNoStr[AMBA_DCF_DIR_NAME_STRLEN];
    char FileNoStr[AMBA_DCF_DIR_NAME_STRLEN];

    #if DEBUG_DCF
    AmbaPrint_PrintUInt5("GenerateFullPath DirNo %d FileNo %d", DirNo, FileNo, 0U, 0U, 0U);
    #endif

    if (AmbaWrap_memset(DirNoStr, 0, sizeof(DirNoStr)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaWrap_memset(FileNoStr, 0, sizeof(FileNoStr)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    DcfCamera_strcpy(FileName, DCFConfig[DriveID].RootPath);
    DcfCamera_strcat(FileName, pSlash);
    NoToText(DirNo, DirNoStr, AMBA_DCF_DIR_NO_STRLEN);
    DcfCamera_strcat(FileName, DirNoStr);

    if (RootInfo[DriveID].DirPostfix[DirNo][0] != '\0') {
        DcfCamera_strcat(FileName, RootInfo[DriveID].DirPostfix[DirNo]);
    } else {
        DcfCamera_strcat(FileName, DCFConfig[DriveID].DirPostfix);
    }

    DcfCamera_strcat(FileName, pSlash);
    DcfCamera_strcat(FileName, DCFConfig[DriveID].FilePrefix);
    if (AmbaUtility_StringPrintUInt32(FileNoStr, (UINT32)sizeof(FileNoStr), "%04u", 1, &FileNo) != 4U) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    DcfCamera_strcat(FileName, FileNoStr);
    DcfCamera_strcat(FileName, FileExt[FileType]);

    #if DEBUG_DCF
    AmbaPrint_PrintStr5("GenerateFullPath FileName %s", FileName, NULL, NULL, NULL, NULL);
    #endif
}

static UINT32 FindValidDir(UINT8 DriveID, UINT32 DirNo)
{
    UINT32 Found = 0U;

    if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_DIR, DirNo, CHECK) == 1U) {
        if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_VALID_DIR, DirNo, CHECK) == 1U) {
            Found = 1U;
        }
    }

    return Found;
}

static UINT32 FindNextValidDir(UINT8 DriveID, UINT32 DirNo)
{
    UINT32 DNo;
    UINT32 Found = 0;
    UINT32 NextValidDirNo = 0;

    for (DNo = DirNo + 1U; DNo <= AMBA_DCF_MAX_DIR_NO; DNo++) {
        Found = FindValidDir(DriveID, DNo);
        if (Found == 1U) {
            NextValidDirNo = DNo;
            break;
        }
    }

    return NextValidDirNo;
}

static UINT32 FindValidFile(AMBA_DCF_DIR_INFO_s *DirInfo, AMBA_DCF_FILE_TYPE_e FileType, UINT32 FileNo)
{
    UINT32 Found = 0U;

    if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 1U) {
            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CHECK) == 1U) {
                Found = 1U;
            }
        }
    } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_MOV, FileNo, CHECK) == 1U){
            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CHECK) == 1U) {
                Found = 1U;
            }
        }
    } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_AUDIO, FileNo, CHECK) == 1U) {
            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) == 0U) {
                if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CHECK) == 1U) {
                    Found = 1U;
                }
            }
        }
    } else if (FileType == AMBA_DCF_FILE_TYPE_ALL) {
        if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_VALID_FILE, FileNo, CHECK) == 1U) {
            Found = 1U;
        }
    } else {
        Found = 0U;
    }

    #if DEBUG_DCF
    if(Found == 1U){
        AmbaPrint_PrintUInt5("FindValidFile FileNo %d TRUE", FileNo, 0U, 0U, 0U, 0U);
    } else {
        //FileNo
    }
    #endif

    return Found;
}

static void UpdateDirForDelete(UINT8 DriveID, UINT32 DirNo)
{
    if (DirNo == RootInfo[DriveID].CurDir.DirNo) {
        if (UpdateCurDir(DriveID) == OK_UINT32) {
            if (UpdatePrevDir(DriveID) == OK_UINT32) {
                if (UpdateNextDir(DriveID) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            }
        }
    } else if (DirNo == RootInfo[DriveID].PrevDir.DirNo) {
        if (UpdatePrevDir(DriveID) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
    } else if (DirNo == RootInfo[DriveID].NextDir.DirNo) {
        if (UpdateNextDir(DriveID) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
    } else {
        //
    }
}

static UINT32 UpdateCurDir(UINT8 DriveID)
{
    UINT32 DirNo;
    UINT32 Found;
    UINT32 rval = OK_INT32;

    for (DirNo = AMBA_DCF_MAX_DIR_NO; DirNo >= AMBA_DCF_MIN_DIR_NO; DirNo--) {
        Found = FindValidDir(DriveID, DirNo);
        if (Found == 1U) {
            break;
        }
    }

    if (Found == 0U) {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not update cur dir", NULL, NULL, NULL, NULL, NULL);
        if (AmbaWrap_memset(&RootInfo[DriveID].CurDir, 0, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if (AmbaWrap_memset(&RootInfo[DriveID].PrevDir, 0, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if (AmbaWrap_memset(&RootInfo[DriveID].NextDir, 0, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        if (DirNo != RootInfo[DriveID].CurDir.DirNo) {
            if (DirNo == RootInfo[DriveID].PrevDir.DirNo) {
                if (AmbaWrap_memcpy(&RootInfo[DriveID].CurDir, &RootInfo[DriveID].PrevDir, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            } else if (DirNo == RootInfo[DriveID].NextDir.DirNo) {
                if (AmbaWrap_memcpy(&RootInfo[DriveID].CurDir, &RootInfo[DriveID].NextDir, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            } else {
                /* scan single dir to save information in cur dir */
                if (ScanSingleDir(DriveID, DIR_TYPE_CUR, DirNo, GET) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            }
        }
    }

    return rval;
}

static UINT32 UpdatePrevDir(UINT8 DriveID)
{
    UINT32 DirNo;
    UINT32 Found = 0;
    UINT32 rval = OK_UINT32;

    for (DirNo = RootInfo[DriveID].CurDir.DirNo - 1U; DirNo >= AMBA_DCF_MIN_DIR_NO; DirNo--) {
        Found = FindValidDir(DriveID, DirNo);
        if (Found == 1U) {
            break;
        }
    }

    if (Found == 0U) {
        for (DirNo = AMBA_DCF_MAX_DIR_NO; DirNo >= (RootInfo[DriveID].CurDir.DirNo + 1U); DirNo--) {
            Found = FindValidDir(DriveID, DirNo);
            if (Found == 1U) {
                break;
            }
        }
    }

    if (Found == 0U) {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not update previous dir", NULL, NULL, NULL, NULL, NULL);
        if (AmbaWrap_memset(&RootInfo[DriveID].PrevDir, 0, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if (AmbaWrap_memset(&RootInfo[DriveID].NextDir, 0, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        if (DirNo != RootInfo[DriveID].PrevDir.DirNo) {
            if (DirNo == RootInfo[DriveID].CurDir.DirNo) {
                if (AmbaWrap_memcpy(&RootInfo[DriveID].PrevDir, &RootInfo[DriveID].CurDir, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            } else if (DirNo == RootInfo[DriveID].NextDir.DirNo) {
                if (AmbaWrap_memcpy(&RootInfo[DriveID].PrevDir, &RootInfo[DriveID].NextDir, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            } else {
                /* scan single dir to save information in prev dir */
                if (ScanSingleDir(DriveID, DIR_TYPE_PREV, DirNo, GET) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            }
        }
    }

    return rval;
}

static UINT32 UpdateNextDir(UINT8 DriveID)
{
    UINT32 DirNo;
    UINT32 Found = 0;
    UINT32 rval = OK_UINT32;

    for (DirNo = RootInfo[DriveID].CurDir.DirNo + 1U; DirNo <= AMBA_DCF_MAX_DIR_NO; DirNo++) {
        Found = FindValidDir(DriveID, DirNo);
        if (Found == 1U) {
            break;
        }
    }

    if (Found == 0U) {
        for (DirNo = AMBA_DCF_MIN_DIR_NO; DirNo <= (RootInfo[DriveID].CurDir.DirNo - 1U); DirNo++) {
            Found = FindValidDir(DriveID, DirNo);
            if (Found == 1U) {
                break;
            }
        }
    }

    if (Found == 0U) {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not update next dir", NULL, NULL, NULL, NULL, NULL);
        if (AmbaWrap_memset(&RootInfo[DriveID].NextDir, 0, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if (AmbaWrap_memset(&RootInfo[DriveID].PrevDir, 0, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        if (DirNo != RootInfo[DriveID].NextDir.DirNo) {
            if (DirNo == RootInfo[DriveID].CurDir.DirNo) {
                if (AmbaWrap_memcpy(&RootInfo[DriveID].NextDir, &RootInfo[DriveID].CurDir, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            } else if (DirNo == RootInfo[DriveID].PrevDir.DirNo) {
                if (AmbaWrap_memcpy(&RootInfo[DriveID].NextDir, &RootInfo[DriveID].PrevDir, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            } else {
                /* scan single dir to save information in next dir */
                if (ScanSingleDir(DriveID, DIR_TYPE_NEXT, DirNo, GET) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            }
        }
    }

    return rval;
}

static void UpdateCurFileID(UINT8 DriveID)
{
    UINT32 DirNo, FileNo;
    UINT32 Found;
    AMBA_DCF_DIR_INFO_s *DirInfo;
    UINT8 FileTypeU;
    AMBA_DCF_FILE_TYPE_e FileType = AMBA_DCF_NUM_FILE_TYPE;

    for (FileTypeU = (UINT8)AMBA_DCF_FILE_TYPE_IMAGE; FileTypeU <= (UINT8)AMBA_DCF_FILE_TYPE_ALL; FileTypeU++) {
        for (DirNo = AMBA_DCF_MAX_DIR_NO; DirNo >= AMBA_DCF_MIN_DIR_NO; DirNo--) {
            Found = FindValidDir(DriveID, DirNo);
            if (Found == 1U) {
                if(RootInfo[DriveID].DirFileAmount[FileTypeU][DirNo] != 0U) {
                    break;
                }
            }
        }

        if (FileTypeU == (UINT8)AMBA_DCF_FILE_TYPE_IMAGE) {
            FileType = AMBA_DCF_FILE_TYPE_IMAGE;
        } else if (FileTypeU == (UINT8)AMBA_DCF_FILE_TYPE_VIDEO) {
            FileType = AMBA_DCF_FILE_TYPE_VIDEO;
        } else if (FileTypeU == (UINT8)AMBA_DCF_FILE_TYPE_AUDIO) {
            FileType = AMBA_DCF_FILE_TYPE_AUDIO;
        } else {
            FileType = AMBA_DCF_FILE_TYPE_ALL;
        }

        if (DirNo < AMBA_DCF_MIN_DIR_NO) {
            if (FileTypeU == (UINT8)AMBA_DCF_FILE_TYPE_IMAGE) {
                RootInfo[DriveID].CurJpegFileID.DirNo = 0;
                RootInfo[DriveID].CurJpegFileID.FileNo = 0;
            } else if (FileTypeU == (UINT8)AMBA_DCF_FILE_TYPE_VIDEO) {
                RootInfo[DriveID].CurMovFileID.DirNo = 0;
                RootInfo[DriveID].CurMovFileID.FileNo = 0;
            } else if (FileTypeU == (UINT8)AMBA_DCF_FILE_TYPE_AUDIO) {
                RootInfo[DriveID].CurAudioFileID.DirNo = 0;
                RootInfo[DriveID].CurAudioFileID.FileNo = 0;
            } else {
                RootInfo[DriveID].CurFileID.DirNo = 0;
                RootInfo[DriveID].CurFileID.FileNo = 0;
            }
            continue;
        }

        if (DirNo == RootInfo[DriveID].CurDir.DirNo) {
            DirInfo = &RootInfo[DriveID].CurDir;
        } else if (DirNo == RootInfo[DriveID].PrevDir.DirNo) {
            DirInfo = &RootInfo[DriveID].PrevDir;
        } else if (DirNo == RootInfo[DriveID].NextDir.DirNo) {
            DirInfo = &RootInfo[DriveID].NextDir;
        } else {
            /* scan single dir to save information in temp dir */
            if (ScanSingleDir(DriveID, DIR_TYPE_TEMP, DirNo, GET) != OK) {
                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
            }
            DirInfo = &RootInfo[DriveID].TempDir;
        }

        for (FileNo = AMBA_DCF_MAX_FILE_NO; FileNo >= AMBA_DCF_MIN_FILE_NO; FileNo--) {
            Found = FindValidFile(DirInfo, FileType, FileNo);
            if (Found == 1U) {
                if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
                    RootInfo[DriveID].CurJpegFileID.DirNo = DirNo;
                    RootInfo[DriveID].CurJpegFileID.FileNo = FileNo;
                } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
                    RootInfo[DriveID].CurMovFileID.DirNo = DirNo;
                    RootInfo[DriveID].CurMovFileID.FileNo = FileNo;
                } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
                    RootInfo[DriveID].CurAudioFileID.DirNo = DirNo;
                    RootInfo[DriveID].CurAudioFileID.FileNo = FileNo;
                } else {
                    RootInfo[DriveID].CurFileID.DirNo = DirNo;
                    RootInfo[DriveID].CurFileID.FileNo = FileNo;
                }
                break;
            }
        }
    }
}

static void UpdateMaxFileID(UINT8 DriveID)
{
    UINT32 DirNo, FileNo;
    AMBA_DCF_DIR_INFO_s *DirInfo;
    INT32 rval = NG_INT32;

    for (DirNo = AMBA_DCF_MAX_DIR_NO; DirNo >= AMBA_DCF_MIN_DIR_NO; DirNo--) {
        if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_DIR, DirNo, CHECK) != 1U) {
            break;
        }
    }

    if (DirNo < AMBA_DCF_MIN_DIR_NO) {
        RootInfo[DriveID].MaxFileID.DirNo = AMBA_DCF_MIN_DIR_NO;
        RootInfo[DriveID].MaxFileID.FileNo = 0;
        rval = OK_INT32;
    }

    if(rval != OK_INT32) {
        if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_VALID_DIR, DirNo, CHECK) == 0U) {
            RootInfo[DriveID].MaxFileID.DirNo = DirNo + 1U;
            RootInfo[DriveID].MaxFileID.FileNo = 0;
            rval = OK_INT32;
        }
    }

    if(rval != OK_INT32) {
        if (DirNo == RootInfo[DriveID].CurDir.DirNo) {
            DirInfo = &RootInfo[DriveID].CurDir;
        } else if (DirNo == RootInfo[DriveID].PrevDir.DirNo) {
            DirInfo = &RootInfo[DriveID].PrevDir;
        } else if (DirNo == RootInfo[DriveID].NextDir.DirNo) {
            DirInfo = &RootInfo[DriveID].NextDir;
        } else {
            /* scan single dir to save information in temp dir */
            if (ScanSingleDir(DriveID, DIR_TYPE_TEMP, DirNo, GET) != OK) {
                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
            }
            DirInfo = &RootInfo[DriveID].TempDir;
        }

        for (FileNo = AMBA_DCF_MAX_FILE_NO; FileNo >= AMBA_DCF_MIN_FILE_NO; FileNo--) {
            if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_JPEG, FileNo, CHECK) != 0U) {
                rval = 1;
            } else if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_MOV, FileNo, CHECK) != 0U) {
                rval = 1;
            } else if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_AUDIO, FileNo, CHECK) != 0U) {
                rval = 1;
            } else {
                //
            }

            if(rval == 1){
                break;
            }
        }

        RootInfo[DriveID].MaxFileID.DirNo = DirNo;
        RootInfo[DriveID].MaxFileID.FileNo = FileNo;
    }
}

static UINT32 SvcDCF_CameraGetDriveID(UINT8 *pDriveID, const char *pFileName)
{
    UINT8 DriveIDNo;
    char FileName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];

    DcfCamera_strcpy(FileName, pFileName);

    for (DriveIDNo = (UINT8)AMBA_DCF_DRIVE_0; DriveIDNo < (UINT8)AMBA_DCF_DRIVER_MAX_NUM; DriveIDNo ++) {
        if(AmbaUtility_StringCompare(&FileName[0], &Drive[DriveIDNo][0], 1U) == OK_INT32) {
            *pDriveID = DriveIDNo;
        }
    }

    return OK_UINT32;
}

UINT32 SvcDCF_CameraConfigDrive(UINT8 DriveID, const char * pRootDrive)
{
    Drive[DriveID][0] = pRootDrive[0];

    return OK_UINT32;
}

UINT32 SvcDCF_CameraInit(UINT8 DriveID, AMBA_DCF_MOV_FMT_e MovFmt)
{
    UINT32 rval = OK_UINT32;

    /* create mutex */
    if (AmbaKAL_MutexCreate(&DCFInfo[DriveID].MutexID, NULL) != OK_UINT32) {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not create mutex", NULL, NULL, NULL, NULL, NULL);
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        /* init setting */
        if (AmbaWrap_memset(&RootInfo[DriveID], 0, sizeof(AMBA_DCF_ROOT_INFO_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if (AmbaWrap_memset(&DCFConfig[DriveID], 0, sizeof(DCF_CONFIG_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }

        if (DcfCamera_strlen(DCFConfig[DriveID].DirPostfix) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
            if(DcfCamera_strlen(DCFConfig[DriveID].FilePrefix) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                if(DcfCamera_strlen(pDefaultDirPostfix) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                    if(DcfCamera_strlen(pDefaultFilePrefix) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                        DcfCamera_strncpy(DCFConfig[DriveID].DirPostfix, pDefaultDirPostfix, sizeof(DCFConfig[DriveID].DirPostfix));
                        DcfCamera_strncpy(DCFConfig[DriveID].FilePrefix, pDefaultFilePrefix, sizeof(DCFConfig[DriveID].FilePrefix));
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "illegal pDefaultDirPostfix = %s, pDefaultFilePrefix = %s", pDefaultDirPostfix, pDefaultFilePrefix, NULL, NULL, NULL);
            rval = NG_UINT32;
        }
    }


    if(rval != NG_UINT32) {
        if (MovFmt == AMBA_DCF_MOV_FMT_MP4) {
            DcfCamera_strcpy(FileExt[AMBA_DCF_FILE_TYPE_VIDEO], ".MP4");
        } else {
            DcfCamera_strcpy(FileExt[AMBA_DCF_FILE_TYPE_VIDEO], ".MOV");
        }
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "DCF init success", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "DCF init fail", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

UINT32 SvcDCF_CameraScanRootStep1(UINT8 DriveID, UINT32 *pLastValidDirNo)
{
    char SearchPattern[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    const char DirPattern[] = "\\*";
    char DirName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    UINT32 DirNo, MaxDirNo = 0U;
    UINT32 rval = OK_UINT32;
    char DirNumberStr[AMBA_DCF_DIR_NO_STRLEN+1U];
    static const char *pDefaultImageRootDir = ":\\DCIM";
    const char *pRootDrive;
    UINT32 SkipFlow = 0U;

    if (AmbaWrap_memset(DirName, 0, sizeof(DirName)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    pRootDrive = &Drive[DriveID][0];
    *pLastValidDirNo = 0U;

    /* init setting */
    if (AmbaWrap_memset(&RootInfo[DriveID], 0, sizeof(AMBA_DCF_ROOT_INFO_s)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (DcfCamera_strlen(DCFConfig[DriveID].RootDrive) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
        if(DcfCamera_strlen(DCFConfig[DriveID].RootPath) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
            if(DcfCamera_strlen(pRootDrive) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                DcfCamera_strncpy(DCFConfig[DriveID].RootDrive, pRootDrive, sizeof(DCFConfig[DriveID].RootDrive));
                DcfCamera_strncpy(DCFConfig[DriveID].RootPath, pRootDrive, sizeof(DCFConfig[DriveID].RootPath));
                DcfCamera_strncat(DCFConfig[DriveID].RootPath, pDefaultImageRootDir, (UINT32)sizeof(DCFConfig[DriveID].RootPath));
            }
        }
    } else {
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        /* if root dir doesn't exist, make root dir */
        if (CheckDirExist(DCFConfig[DriveID].RootPath) != OK_UINT32) {
            if (MakeDir(DCFConfig[DriveID].RootPath) != OK_UINT32) {
                rval = NG_UINT32;
            }
        }
    }

    if(rval != NG_UINT32) {
        /* create search pattern to scan dir */
        DcfCamera_strncpy(SearchPattern, DCFConfig[DriveID].RootPath, sizeof(SearchPattern));
        DcfCamera_strncat(SearchPattern, DirPattern, (UINT32)(sizeof(SearchPattern)-DcfCamera_strlen(SearchPattern))-1U);

        /* scan dir */
        if (AmbaWrap_memset(&Data, 0, sizeof(AMBA_FS_DTA_t)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        rval = AmbaFS_FileSearchFirst(SearchPattern, (UINT8)(AMBA_FS_ATTR_ARCH | AMBA_FS_ATTR_RDONLY | AMBA_FS_ATTR_NONE | (UINT32)AMBA_FS_ATTR_DIR | AMBA_FS_ATTR_HIDDEN), &Data);
        if (rval == AMBA_FS_ERR_API) {
            RootInfo[DriveID].MaxFileID.DirNo  = AMBA_DCF_MIN_DIR_NO;
            RootInfo[DriveID].MaxFileID.FileNo = 0;
            SkipFlow = 1U;
            rval = OK_UINT32;
        }
    }

    if(SkipFlow == 0U) {
        if(rval != NG_UINT32) {
            while (rval == OK_UINT32) {
                /* get dir name */
                if ((char)Data.LongName[0] != '\0') {
                    DcfCamera_strcpy(DirName, (const char*)Data.LongName);
                } else {
                    DcfCamera_strcpy(DirName, (const char*)Data.FileName);
                }

                /* filter out illegal dir name */
                if (CheckIllegalDir(DirName) != OK_UINT32) {
            #if DEBUG_DCF
                    AmbaPrint_PrintStr5("Illegal folder name: %s", DirName, NULL, NULL, NULL, NULL);
            #endif
                    rval = AmbaFS_FileSearchNext(&Data);
                    continue;
                }

                /* filter out illegal dir no */
                DcfCamera_strncpy(&DirNumberStr[0], &DirName[0], sizeof(DirNumberStr));
                DirNo = StrToUint32(&DirNumberStr[0]);

                if ((DirNo < AMBA_DCF_MIN_DIR_NO) || (DirNo > AMBA_DCF_MAX_DIR_NO)) {
            #if DEBUG_DCF
                    AmbaPrint_PrintUInt5("Illegal DirNo = %d", DirNo, 0U, 0U, 0U, 0U);
            #endif
                    rval = AmbaFS_FileSearchNext(&Data);
                    continue;
                }

                /* save max dir no */
                if (DirNo > MaxDirNo) {
                    MaxDirNo = DirNo;
                }

#if AMBA_FS_ATTR_HIDDEN != 0U
                /* skip hidden dir */
                if((Data.Attribute & (UINT32)AMBA_FS_ATTR_HIDDEN) != 0U) {
                    rval = AmbaFS_FileSearchNext(&Data);
                    continue;
                }
#endif

                /* set dir bit table and save dir amount */
                if(SvcDcfTable_AddDir(DriveID, DirNo, DirName) != OK_UINT32){
                    rval = AmbaFS_FileSearchNext(&Data);
                    continue;
                }

                /* check total dir amount */
                if (RootInfo[DriveID].TotalDirAmount >= AMBA_DCF_MAX_DIR_AMOUNT) {
                    break;
                }

                rval = AmbaFS_FileSearchNext(&Data);
            }
        }
    }

    if(SkipFlow == 0U) {
        if(rval != NG_UINT32){
            /* scan file in last valid dir */
            for (DirNo = AMBA_DCF_MAX_DIR_NO; DirNo >= AMBA_DCF_MIN_DIR_NO; DirNo--) {
                if(FindValidDir(DriveID, DirNo) == 1U){
                    if (ScanSingleDir(DriveID, DIR_TYPE_CUR, DirNo, ADD) != OK) {
                        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                    }
                    *pLastValidDirNo = DirNo;
                    rval = OK_UINT32;
                    break;
                }
            }

            if(DirNo <= AMBA_DCF_MIN_DIR_NO){
                rval = OK_UINT32;
            }

            /* deal with max file id */
            if (RootInfo[DriveID].MaxFileID.DirNo == 0U) {
                if(RootInfo[DriveID].MaxFileID.FileNo == 0U) {
                    RootInfo[DriveID].MaxFileID.DirNo = AMBA_DCF_MIN_DIR_NO;
                    RootInfo[DriveID].MaxFileID.FileNo = 0;
                }
            }

            if (MaxDirNo > RootInfo[DriveID].MaxFileID.DirNo) {
                RootInfo[DriveID].MaxFileID.DirNo = MaxDirNo + 1U;
                RootInfo[DriveID].MaxFileID.FileNo = 0U;
            }
        }
    }

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    return rval;
}

UINT32 SvcDCF_CameraScanRootStep2(UINT8 DriveID, UINT32 LastValidDirNo)
{
    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    /* scan the rest valid dir */
    if(LastValidDirNo != 0U){
        if (ScanRestDir(DriveID, LastValidDirNo) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
    } else {
        UpdateCurFileID(DriveID);
    }

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    return OK_UINT32;
}

UINT32 SvcDCF_CameraDestroy(UINT8 DriveID)
{
    UINT32 rval = OK_UINT32;

    if(DriveID == 0U) {
    }

    /* delete mutex */
    if (AmbaKAL_MutexDelete(&DCFInfo[DriveID].MutexID) != OK_UINT32) {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not delete mutex", NULL, NULL, NULL, NULL, NULL);
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        /* reset setting */
        if (AmbaWrap_memset(&RootInfo[DriveID], 0, sizeof(AMBA_DCF_ROOT_INFO_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if (AmbaWrap_memset(&DCFConfig[DriveID], 0, sizeof(DCF_CONFIG_s)) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }

        if (DcfCamera_strlen(DCFConfig[DriveID].DirPostfix) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
            if(DcfCamera_strlen(DCFConfig[DriveID].FilePrefix) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                if(DcfCamera_strlen(pDefaultDirPostfix) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                    if(DcfCamera_strlen(pDefaultFilePrefix) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                        DcfCamera_strncpy(DCFConfig[DriveID].DirPostfix, pDefaultDirPostfix, sizeof(DCFConfig[DriveID].DirPostfix));
                        DcfCamera_strncpy(DCFConfig[DriveID].FilePrefix, pDefaultFilePrefix, sizeof(DCFConfig[DriveID].FilePrefix));
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "illegal pDefaultDirPostfix = %s, pDefaultFilePrefix = %s", pDefaultDirPostfix, pDefaultFilePrefix, NULL, NULL, NULL);
            rval = NG_UINT32;
        }
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "DCF destroy success", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "DCF destroy fail", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

UINT32 SvcDCF_CameraCreateFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 vinid, UINT32 StreamId)
{
    UINT32 DirNo = 0U, FileNo = 0U;
    UINT32 rval = OK_UINT32;

    if(DriveID == 0U) {
    }

    if(FolderID == 0U) {
    }

    if(vinid == 0U) {
    }

    if(StreamId == 0U) {
    }

    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    if ((FileType != AMBA_DCF_FILE_TYPE_IMAGE) && (FileType != AMBA_DCF_FILE_TYPE_VIDEO) &&
        (FileType != AMBA_DCF_FILE_TYPE_AUDIO)) {
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        DirNo  = RootInfo[DriveID].MaxFileID.DirNo;
        FileNo = RootInfo[DriveID].MaxFileID.FileNo;

        if ((DirNo >= AMBA_DCF_MAX_DIR_NO) && (FileNo >= AMBA_DCF_MAX_FILE_NO)) {
            AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Already reach the upper limit of available file", NULL, NULL, NULL, NULL, NULL);
            rval = NG_UINT32;
        }
    }

    if(rval != NG_UINT32) {
        if ((FileNo == 0U) || (FileNo == AMBA_DCF_MAX_FILE_NO)) {
            if (FileNo == AMBA_DCF_MAX_FILE_NO) {
                DirNo++;
            }

            /* if the dir doesn't exist, create dir */
            if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_DIR, DirNo, CHECK) == 0U) {
                if (CreateDir(DriveID, DirNo) != OK_UINT32) {
                    AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not create new dir", NULL, NULL, NULL, NULL, NULL);
                    rval = NG_UINT32;
                }
            }
        }
    }

    if(rval != NG_UINT32) {
        /* update max file id */
        if (FileNo == AMBA_DCF_MAX_FILE_NO) {
            FileNo = AMBA_DCF_MIN_FILE_NO;
            RootInfo[DriveID].MaxFileID.DirNo = DirNo;
            RootInfo[DriveID].MaxFileID.FileNo = FileNo;
        } else {
            RootInfo[DriveID].MaxFileID.FileNo ++;
            FileNo = RootInfo[DriveID].MaxFileID.FileNo;
        }

        GenerateFullPath(DriveID, FileType, DirNo, FileNo, pFileName);
    }

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    return rval;
}

UINT32 SvcDCF_CameraUpdateFile(const char *pFileName)
{
    UINT32 DirNo = 0U, FileNo = 0U;
    AMBA_DCF_FILE_TYPE_e FileType = AMBA_DCF_FILE_TYPE_IMAGE;
    char DirName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char FileName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char Path[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    AMBA_DCF_DIR_INFO_s *pDirInfo;
    UINT32 rval = OK_UINT32;
    char DirNoStr[AMBA_DCF_DIR_NO_STRLEN + 1U];
    char FileNoStr[AMBA_DCF_FILE_NO_STRLEN + 1U];
    UINT8 DriveID = 0U;

    if (AmbaWrap_memset(Path, 0, sizeof(Path)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaWrap_memset(DirName, 0, sizeof(DirName)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaWrap_memset(FileName, 0, sizeof(FileName)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (SvcDCF_CameraGetDriveID(&DriveID, pFileName) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    if(DcfCamera_strlen(pFileName) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
        DcfCamera_strncpy(Path, pFileName, sizeof(Path));
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Illegal pFileName = %s", pFileName, NULL, NULL, NULL, NULL);
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        DcfCamera_strncpy(DirName, &Path[8U], 9U);
        DirName[AMBA_DCF_DIR_NAME_STRLEN] = '\0';
        if (CheckIllegalDir(DirName) != OK_UINT32) {
            AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Illegal DirName = %s", DirName, NULL, NULL, NULL, NULL);
            rval = NG_UINT32;
        }
    }

    if(rval != NG_UINT32) {
        DcfCamera_strncpy(DirNoStr, &Path[8U], sizeof(DirNoStr));
        if (AmbaUtility_StringToUInt32(DirNoStr, &DirNo) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if ((DirNo < AMBA_DCF_MIN_DIR_NO) || (DirNo > AMBA_DCF_MAX_DIR_NO)) {
            AmbaPrint_ModulePrintUInt5(AMBA_DCF_PRINT_MODULE_ID, "Illegal DirNo = %d", DirNo, 0U, 0U, 0U, 0U);
            rval = NG_UINT32;
        }
    }

    if(rval != NG_UINT32) {
        DcfCamera_strncpy(FileName, &Path[17U], sizeof(FileName));
        FileName[12U] = '\0';
        if (CheckIllegalFile(FileName) != OK_UINT32) {
            AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Illegal FileName = %s", FileName, NULL, NULL, NULL, NULL);
            rval = NG_UINT32;
        }
    }

    if(rval != NG_UINT32) {
        DcfCamera_strncpy(FileNoStr, &Path[21U], sizeof(FileNoStr));
        if (AmbaUtility_StringToUInt32(FileNoStr, &FileNo) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
        if ((FileNo < AMBA_DCF_MIN_FILE_NO) || (FileNo > AMBA_DCF_MAX_FILE_NO)) {
            AmbaPrint_ModulePrintUInt5(AMBA_DCF_PRINT_MODULE_ID, "Illegal FileNo = %d", FileNo, 0U, 0U, 0U, 0U);
            rval = NG_UINT32;
        }
    }

    if(rval != NG_UINT32) {
        if (GetFileType(pFileName, &FileType) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }

        /* if the dir is new created, add dir */
        if (SvcDcfTable_HandleDirTable(DriveID, AMBA_DCF_TABLE_TYPE_DIR, DirNo, CHECK) == 0U) {
            if (SvcDcfTable_AddDir(DriveID, DirNo, DirName) == 0U) {
                /* Cannot ScanSingleDir directly because unclosed files will be taken as valid. *\
                 * If no folder in storage, take the first folder as CurDir. If some folders    *
                \* existed, take the new folder as NextDir.                                     */
                pDirInfo = (RootInfo[DriveID].CurDir.DirNo == 0U) ? &RootInfo[DriveID].CurDir : &RootInfo[DriveID].NextDir;
                if (AmbaWrap_memset(pDirInfo, 0, sizeof(AMBA_DCF_DIR_INFO_s)) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
                pDirInfo->DirNo = DirNo;
                if (SvcDcfTable_AddFile(pDirInfo, FileType, FileNo) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            }
        } else {
            /* add file */
            if (DirNo == RootInfo[DriveID].CurDir.DirNo) {
                if (SvcDcfTable_AddFile(&RootInfo[DriveID].CurDir, FileType, FileNo) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            } else if (DirNo == RootInfo[DriveID].PrevDir.DirNo) {
                if (SvcDcfTable_AddFile(&RootInfo[DriveID].PrevDir, FileType, FileNo) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            } else if (DirNo == RootInfo[DriveID].NextDir.DirNo) {
                if (SvcDcfTable_AddFile(&RootInfo[DriveID].NextDir, FileType, FileNo) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            } else {
                //
            }
        }

        //if (DirNo >= AMBA_DCF_MIN_DIR_NO) {
            //if (DirNo <= AMBA_DCF_MAX_DIR_NO) {
                /* update total file amount */
                if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
                    RootInfo[DriveID].TotalJpegAmount++;
                } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
                    RootInfo[DriveID].TotalMovAmount++;
                } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
                    RootInfo[DriveID].TotalAudioAmount++;
                } else {
                    //
                }

                RootInfo[DriveID].TotalFileAmount++;
                RootInfo[DriveID].DirFileAmount[FileType][DirNo]++;
                RootInfo[DriveID].DirFileAmount[AMBA_DCF_FILE_TYPE_ALL][DirNo]++;
            //}
        //}
    }

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    return rval;
}

UINT32 SvcDCF_CameraDeleteFile(const char *pFileName)
{
    UINT32 DirNo = 0U, FileNo = 0U;
    AMBA_DCF_DIR_INFO_s *DirInfo = NULL;
    char DirName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char DirPath[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char Path[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    char AssociatedFileName[AMBA_DCF_MAX_FILE_FULL_PATH_LEN];
    AMBA_FS_FILE_INFO_s Status;
    UINT8 DriveID = 0U;
    char DirNoStr[AMBA_DCF_DIR_NO_STRLEN + 1U];
    char FileNoStr[AMBA_DCF_FILE_NO_STRLEN + 1U];
    INT32 rval = OK_INT32;
    AMBA_DCF_FILE_TYPE_e FileType;

    if (AmbaWrap_memset(Path, 0, sizeof(Path)) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if (SvcDCF_CameraGetDriveID(&DriveID, pFileName) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }
    if(AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    if (GetFileType(pFileName, &FileType) == OK_UINT32) {
        rval = OK_INT32;
    } else {
        rval = NG_INT32;
    }

    if(rval != NG_INT32) {
        /* filter out read only file */
        if (AmbaFS_GetFileInfo(pFileName, &Status) == OK_UINT32) {
            if((Status.Attr & (UINT32)AMBA_FS_ATTR_RDONLY) != 0U) {
                rval = NG_INT32;
            }
        } else {
            rval = NG_INT32;
        }
    }

    if(rval != NG_INT32) {
        if (DcfCamera_strlen(Path) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
            if(DcfCamera_strlen(pFileName) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                DcfCamera_strncpy(Path, pFileName, sizeof(Path));
            }
        } else {
            rval = NG_INT32;
        }
    }

    DcfCamera_strncpy(DirName, &Path[8U], 9U);
    DcfCamera_strncpy(DirNoStr, &Path[8U], sizeof(DirNoStr));
    if (AmbaUtility_StringToUInt32(DirNoStr, &DirNo) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    DcfCamera_strncpy(FileNoStr, &Path[21U], sizeof(FileNoStr));
    if (AmbaUtility_StringToUInt32(FileNoStr, &FileNo) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    #if DEBUG_DCF
    AmbaPrint_PrintStr5("DirName %s &Path[8U] %s",DirName ,&Path[8U], NULL, NULL, NULL);
    AmbaPrint_PrintStr5("DirNoStr %s &Path[8U] %s",DirNoStr ,&Path[8U], NULL, NULL, NULL);
    AmbaPrint_PrintUInt5("DirNo %d ", DirNo, 0U, 0U, 0U, 0U);

    AmbaPrint_PrintStr5("FileNoStr %s &Path[21U] %s",FileNoStr ,&Path[21U], NULL, NULL, NULL);
    AmbaPrint_PrintUInt5("FileNo %d ", FileNo, 0U, 0U, 0U, 0U);
    #endif

    if(rval != NG_INT32) {
        /* delete file */
        if (DirNo == RootInfo[DriveID].CurDir.DirNo) {
            DirInfo = &RootInfo[DriveID].CurDir;
        } else if (DirNo == RootInfo[DriveID].PrevDir.DirNo) {
            DirInfo = &RootInfo[DriveID].PrevDir;
        } else if (DirNo == RootInfo[DriveID].NextDir.DirNo) {
            DirInfo = &RootInfo[DriveID].NextDir;
        } else {
            /* scan single dir to save information in temp dir */
            if (ScanSingleDir(DriveID, DIR_TYPE_TEMP, DirNo, GET) != OK) {
                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
            }
            DirInfo = &RootInfo[DriveID].TempDir;
        }

        if (FindValidFile(DirInfo, FileType, FileNo) == 0U) {
            AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "This is not a valid file. Path = %s", pFileName, NULL, NULL, NULL, NULL);
            rval = NG_INT32;
        }
    }

    if(rval != NG_INT32) {
        if (AmbaFS_Remove(pFileName) != OK_UINT32) {
            AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not delete file. Path = %s", pFileName, NULL, NULL, NULL, NULL);
            rval = NG_INT32;
        } else {
            /* delete the associated file */
            if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
                if (SvcDcfTable_HandleFileTable(DirInfo, AMBA_DCF_TABLE_TYPE_AUDIO, FileNo, CHECK) == 1U) {
                    if (GetFullPath(DriveID, DirInfo, AMBA_DCF_FILE_TYPE_AUDIO, DirNo, FileNo, AssociatedFileName) != OK) {
                        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                    }
                    if (AmbaFS_Remove(AssociatedFileName) != OK_UINT32) {
                        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not delete file. Path = %s", AssociatedFileName, NULL, NULL, NULL, NULL);
                        rval = NG_INT32;
                    }
                }
            }

            if(rval != NG_INT32) {
                if (SvcDcfTable_DeleteFile(DirInfo, FileType, FileNo) != OK) {
                    SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
                }
            }
        }
    }

    if(rval != NG_INT32) {
        /* update total file amount */
        if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
            RootInfo[DriveID].TotalJpegAmount--;
        } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
            RootInfo[DriveID].TotalMovAmount--;
        } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
            RootInfo[DriveID].TotalAudioAmount--;
        } else {
            //
        }

        RootInfo[DriveID].TotalFileAmount--;
        RootInfo[DriveID].DirFileAmount[FileType][DirNo]--;
        RootInfo[DriveID].DirFileAmount[AMBA_DCF_FILE_TYPE_ALL][DirNo]--;

        /* if the dir is empty, delete dir */
        if (RootInfo[DriveID].DirFileAmount[AMBA_DCF_FILE_TYPE_ALL][DirNo] == 0U) {
            if (AmbaFS_ChangeDir(DCFConfig[DriveID].RootPath) != OK) {
                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
            }
            DcfCamera_strcpy(DirPath, DCFConfig[DriveID].RootPath);
            if (DcfCamera_strlen(DirPath) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                if (DcfCamera_strlen(pSlash) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                    if (DcfCamera_strlen(DirName) < AMBA_DCF_MAX_FILE_FULL_PATH_LEN) {
                        DcfCamera_strncat(DirPath, pSlash, (UINT32)(sizeof(DirPath)-DcfCamera_strlen(DirPath))-1U);
                        DcfCamera_strncat(DirPath, DirName, (UINT32)(sizeof(DirPath)-DcfCamera_strlen(DirPath))-1U);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Illegal DirName = %s", DirName, NULL, NULL, NULL, NULL);
                rval = NG_INT32;
            }

            if(rval != NG_INT32) {
                if (AmbaFS_RemoveDir(DirPath) == OK_UINT32) {
                    if (SvcDcfTable_DeleteDir(DriveID, DirNo) == 0U) {
                        UpdateDirForDelete(DriveID ,DirNo);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Can not delete dir. Path = %s", DirPath, NULL, NULL, NULL, NULL);
                }
            }
        }
    }

    if(rval != NG_INT32) {
        if (DirNo == RootInfo[DriveID].CurJpegFileID.DirNo) {
            if(FileNo == RootInfo[DriveID].CurJpegFileID.FileNo) {
                UpdateCurFileID(DriveID);
            }
        } else if (DirNo == RootInfo[DriveID].CurMovFileID.DirNo) {
            if(FileNo == RootInfo[DriveID].CurMovFileID.FileNo) {
                UpdateCurFileID(DriveID);
            }
        } else if (DirNo == RootInfo[DriveID].CurAudioFileID.DirNo) {
            if(RootInfo[DriveID].CurAudioFileID.FileNo != 0U) {
                UpdateCurFileID(DriveID);
            }
        } else if (DirNo == RootInfo[DriveID].CurFileID.DirNo) {
            if(FileNo == RootInfo[DriveID].CurFileID.FileNo) {
                UpdateCurFileID(DriveID);
            }
        } else {
            //
        }

        if (DirNo == RootInfo[DriveID].MaxFileID.DirNo) {
            if (FileNo == RootInfo[DriveID].MaxFileID.FileNo) {
                UpdateMaxFileID(DriveID);
            }
        }
    }

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    return OK_UINT32;
}

UINT32 SvcDCF_CameraGetFileName(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, char *pFileName, UINT32 Index)
{
    UINT32 DirNo = 0, FileNo = 0, AccumulateCount = 0, ValidCount = 0;
    UINT32 Found = 0;
    AMBA_DCF_DIR_INFO_s *DirInfo = NULL;
    UINT32 rval = OK_UINT32;

    if(DriveID == 0U) {
    }

    if(FolderID == 0U) {
    }

    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
        if ((Index < 1U) || (Index > RootInfo[DriveID].TotalJpegAmount)) {
            rval = NG_UINT32;
        }
    } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
        if ((Index < 1U) || (Index > RootInfo[DriveID].TotalMovAmount)) {
            rval = NG_UINT32;
        }
    } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
        if ((Index < 1U) || (Index > RootInfo[DriveID].TotalAudioAmount)) {
            rval = NG_UINT32;
        }
    } else if (FileType == AMBA_DCF_FILE_TYPE_ALL) {
        if ((Index < 1U) || (Index > RootInfo[DriveID].TotalFileAmount)) {
            rval = NG_UINT32;
        }
    } else {
        rval = NG_UINT32;
    }

    if(rval != NG_UINT32) {
        /* find the dir which the index belongs to */
        for (DirNo = AMBA_DCF_MIN_DIR_NO; DirNo <= AMBA_DCF_MAX_DIR_NO; DirNo++) {
            AccumulateCount += RootInfo[DriveID].DirFileAmount[FileType][DirNo];
            if (AccumulateCount >= Index) {
                break;
            }
        }

        if (DirNo > AMBA_DCF_MAX_DIR_NO) {
            rval = NG_UINT32;
        }
    }

    #if 0//DEBUG_DCF
    //AccumulateCount
    //Index
    #endif

    if(rval != NG_UINT32) {
        if (DirNo == RootInfo[DriveID].CurDir.DirNo) {
            DirInfo = &RootInfo[DriveID].CurDir;
        } else if (DirNo == RootInfo[DriveID].PrevDir.DirNo) {
            DirInfo = &RootInfo[DriveID].PrevDir;
        } else if (DirNo == RootInfo[DriveID].NextDir.DirNo) {
            DirInfo = &RootInfo[DriveID].NextDir;
        } else {
            /* scan single dir to save information in temp dir */
            if (ScanSingleDir(DriveID, DIR_TYPE_TEMP, DirNo, GET) != OK) {\
                SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
            }
            DirInfo = &RootInfo[DriveID].TempDir;
        }

        for (FileNo = AMBA_DCF_MIN_FILE_NO; FileNo <= AMBA_DCF_MAX_FILE_NO; FileNo++) {
            #if 0//DEBUG_DCF
            //FileNo
            #endif
            Found = FindValidFile(DirInfo, FileType, FileNo);
            if (Found == 1U) {
                ValidCount++;
                #if 0//DEBUG_DCF
                //FileNo
                #endif
                if (ValidCount == (Index - (AccumulateCount - RootInfo[DriveID].DirFileAmount[FileType][DirNo]))) {
                    break;
                }
            }
        }

        #if 0//DEBUG_DCF
        //AccumulateCount, RootInfo[DriveID].DirFileAmount[FileType][DirNo], ValidCount, Index
        #endif

        #if 1
        if (((AccumulateCount - RootInfo[DriveID].DirFileAmount[FileType][DirNo]) + ValidCount) != Index) {
            #if DEBUG_DCF
            AmbaPrint_PrintUInt5("Can not find the file name DirNo %d FileNo %d", DirNo, FileNo, 0U, 0U, 0U);
            #endif
            rval = NG_UINT32;
        }
        #endif
    }

    if(rval != NG_UINT32) {
        if (GetFullPath(DriveID, DirInfo, FileType, DirNo, FileNo, pFileName) != OK) {
            SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
        }
    }

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    #if DEBUG_DCF
    AmbaPrint_PrintUInt5("SvcDCF_CameraGetFileName index %d", Index, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintStr5("FileName = %s", pFileName, NULL, NULL, NULL, NULL);
    #endif

    return rval;
}

UINT32 SvcDCF_CameraGetFileAmount(UINT8 DriveID, UINT8 FolderID, AMBA_DCF_FILE_TYPE_e FileType, UINT32 *pFileAmount)
{
    UINT32 rval = OK_INT32;

    if(DriveID == 0U) {
    }

    if(FolderID == 0U) {
    }

    if (AmbaKAL_MutexTake(&DCFInfo[DriveID].MutexID, AMBA_KAL_WAIT_FOREVER) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    if (FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
        *pFileAmount = RootInfo[DriveID].TotalJpegAmount;
    } else if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
        *pFileAmount = RootInfo[DriveID].TotalMovAmount;
    } else if (FileType == AMBA_DCF_FILE_TYPE_AUDIO) {
        *pFileAmount = RootInfo[DriveID].TotalAudioAmount;
    } else if (FileType == AMBA_DCF_FILE_TYPE_ALL) {
        *pFileAmount = RootInfo[DriveID].TotalFileAmount;
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_DCF_PRINT_MODULE_ID, "Wrong File Type", NULL, NULL, NULL, NULL, NULL);
        rval = NG_UINT32;
    }

    if (AmbaKAL_MutexGive(&DCFInfo[DriveID].MutexID) != OK) {
        SvcDcf_CameraNG(__func__, "Line %d", __LINE__, 0U);
    }

    return rval;
}

