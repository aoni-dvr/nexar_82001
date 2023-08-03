/**
 *  @file AmbaFS.h
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
 *  @details Abstraction layer of Ambarella File System.
 *
 */

#ifndef AMBA_FS_H
#define AMBA_FS_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#include "AmbaRTC.h"

#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <ftw.h>
#include <time.h>
#include <sys/types.h>
#include <utime.h>
#include <glob.h>

#define AMBA_FS_FILE     FILE
#define AMBA_FS_DIR      DIR
#define AMBA_FS_FPOS     fpos_t

#define VOLUME_LABEL_BUF_SIZE (22U)

typedef struct dirent    AMBA_FS_DIRENT;

#define VOLUME_MAGIC       (0x100efeedU)
#define PATH_BUF_SIZE      (520U)
#define LONG_NAME_BUF_SIZE (512U)
typedef struct {
    /*-----------------------------------------------------------------------*
      User fields: this is for user area, fsexec API sets file/dir pointer to this area/
     *-----------------------------------------------------------------------*/
    /* File descriptor */
    AMBA_FS_FILE*    p_file;

    /* Directory descriptor */
    AMBA_FS_DIR*     p_dir;

    /*-----------------------------------------------------------------------*
      Internal fields: do not use these as they will change without notice
     *-----------------------------------------------------------------------*/
    glob_t Glob;

    /* Volume of the parent directory   */
    void  *p_vol;

    /* Parent directory start cluster */
    UINT32   parent_start_cluster;

    /* Next entry position within parent */
    UINT32   parent_pos; /* started from 1 */

    /* Next entry position within parent */
    UINT32   Current_pos; /* started from 0 */

    /* Attributes searched for */
    UINT8   attr;

    /* Regular expression for file name */
    char   reg_exp[PATH_BUF_SIZE];

    /*-----------------------------------------------------------------------*
      Public fields:
     *-----------------------------------------------------------------------*/

    /* Modification time */
    UINT16  Time;

    /* Modification date */
    UINT16  Date;

    /* Access date */
    UINT16  ADate;

    /* Attr Change time */
    UINT16  CTime;

    /* Attr Change  date */
    UINT16  CDate;

    /* Create milliseconds (actually contains a count of tenths of a second) */
    UINT16  CTimeMs;

    /* File size in bytes */
    UINT64  FileSize;

    /* File attributes */
    UINT32  Attribute;

    /* Short file name (8.3) */
    char     FileName[16U];

    /* Long file name */
    char     LongName[LONG_NAME_BUF_SIZE];
} AMBA_FS_DTA_t;

#else
#include "prfile2/pf_apicmn.h"

#define DIR_ENTRY_SIZE          (32)

#define AMBA_FS_DTA_t   PF_DTA
#define AMBA_FS_FILE    PF_FILE
#define AMBA_FS_DIR     PF_DIR
#define AMBA_FS_DIRENT  PF_DIRENT
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

typedef struct {
    UINT16  SysYear;               /* year                */
    UINT16  SysMonth;              /* month (1-12)        */
    UINT16  SysDay;                /* day (1-31)          */
} AMBA_FS_SYS_DATE_s;

typedef struct {
    UINT16  SysHour;               /* hour (0-23)         */
    UINT16  SysMin;                /* minites (0-59)      */
    UINT16  SysSec;                /* second (0-59)       */
    UINT16  SysMs;                 /* milli second(0-199) */

    UINT16  SysUtcValid;           /* UTC Offset Valid for exFAT */
    INT16   SysUtc;                /* UTC Offset [minites] for exFAT */
} AMBA_FS_SYS_TIME_s;

typedef struct {
    AMBA_FS_SYS_DATE_s     CreateDate;   /* create date */
    AMBA_FS_SYS_TIME_s     Createtime;   /* create time */
    AMBA_FS_SYS_DATE_s     ModifiedDate; /* modified date */
    AMBA_FS_SYS_TIME_s     ModifiedTime; /* modified time */
    AMBA_FS_SYS_DATE_s     AccessDate;   /* last access date */
    AMBA_FS_SYS_TIME_s     AccessTime;   /* last access time */
} AMBA_FS_TIMESTMP_s;

typedef struct {
    UINT32  Attr;                           /* file attributes */
    UINT64  Size;                           /* total size, in bytes */
    AMBA_RTC_DATE_TIME_s   TimeModify;     /* time of last modification */
    AMBA_RTC_DATE_TIME_s   TimeCreate;     /* time of last modification */
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
    /* POSIX do not suport Create time */
    AMBA_RTC_DATE_TIME_s   TimeAccess;     /* time of last access */
    AMBA_RTC_DATE_TIME_s   TimeChange;     /* time of last attr change */
#endif
} AMBA_FS_FILE_INFO_s;


typedef struct {
    UINT32   TotalClusters;          /* total number of logical clusters */
    UINT32   EmptyClusters;          /* number of empty clusters */
    UINT32   BytesPerSector;         /* bytes per logical sector  */
    UINT32   SectorsPerCluster;      /* logical sectors per cluster */
    UINT32   ClustersPerGroup;       /* clusters per cluster group */
    UINT32   EmptyClusterGroups;     /* number of empty cluster groups */
    UINT32   FormatType;             /* format type */
    UINT32   Attr;                   /* Disk attribute */
} AMBA_FS_DRIVE_INFO_t;

#define AMBA_FS_VOLUME_LABEL_BUF_SIZE (VOLUME_LABEL_BUF_SIZE + 1U)
typedef struct {
    UINT8    VolAttr;          /* Attributes of the volume label */
    UINT32   VolDate;          /* Date */
    UINT32   VolTime;          /* Time */
    char     VolName[AMBA_FS_VOLUME_LABEL_BUF_SIZE]; /* Volume label string (null-terminated) */
} AMBA_FS_VOL_INFO_t;


/*
 * Error code for definition
 */
#define FS_SAFE_ERR_BASE    (0x100U)

#define FS_ERR_0000         (FS_ERR_BASE)
#define FS_ERR_0004         (FS_ERR_BASE | 0x4U)       /* FS API error */

#define FS_ERR_0001         (FS_ERR_BASE | 1U)         /* PF_ERR_EPERM : Operation is not possible    */
#define FS_ERR_0002         (FS_ERR_BASE | 2U)         /* PF_ERR_ENOENT: No such file or directory    */
#define FS_ERR_0003         (FS_ERR_BASE | 3U)         /* PF_ERR_ESRCH : No such process              */

#define FS_ERR_0005         (FS_ERR_BASE | 5U)         /* PF_ERR_EIO    : I/O Error(Driver Error)      */
#define FS_ERR_0007         (FS_ERR_BASE | 7U)         /* PF_ERR_E2BIG  : Argument list too long       */
#define FS_ERR_0008         (FS_ERR_BASE | 8U)         /* PF_ERR_ENOEXEC: Not Executable by internal   */
#define FS_ERR_0009         (FS_ERR_BASE | 9U)         /* PF_ERR_EBADF  : Bad file descriptor          */
/* 9 ~ B: Reserve */
#define FS_ERR_000C         (FS_ERR_BASE | 12U)        /* PF_ERR_ENOMEM : Not enough system memory     */
#define FS_ERR_000D         (FS_ERR_BASE | 13U)        /* PF_ERR_EACCES : Permission denied            */
#define FS_ERR_0010         (FS_ERR_BASE | 16U)        /* PF_ERR_EBUSY  : Can not use system resouces  */
#define FS_ERR_0011         (FS_ERR_BASE | 17U)        /* PF_ERR_EEXIST : File already exists          */
#define FS_ERR_0013         (FS_ERR_BASE | 19U)        /* PF_ERR_ENODEV : No such device.              */
#define FS_ERR_0015         (FS_ERR_BASE | 21U)        /* PF_ERR_EISDIR : find directory when file req */
#define FS_ERR_0016         (FS_ERR_BASE | 22U)        /* PF_ERR_EINVAL : Invalid argument             */
#define FS_ERR_0017         (FS_ERR_BASE | 23U)        /* PF_ERR_ENFILE : Too many open files(system)  */
#define FS_ERR_0018         (FS_ERR_BASE | 24U)        /* PF_ERR_EMFILE : Too many open files(user)    */
#define FS_ERR_001B         (FS_ERR_BASE | 27U)        /* PF_ERR_EFBIG  : Over file size limit(4GB-1)  */
#define FS_ERR_001C         (FS_ERR_BASE | 28U)        /* PF_ERR_ENOSPC : Device out of space          */
/* 1D~2D: Reserve */
#define FS_ERR_002E         (FS_ERR_BASE | 46U)        /* PF_ERR_ENOLCK : Can not lock the file        */
/* 2E~55: Reserve */
#define FS_ERR_0055         (FS_ERR_BASE | 85U)        /* PF_ERR_EILSEQ    : Illegal byte sequence     */
#define FS_ERR_0058         (FS_ERR_BASE | 88U)        /* PF_ERR_ENOSYS    : Not implement function    */
#define FS_ERR_005A         (FS_ERR_BASE | 90U)        /* PF_ERR_ENOTEMPTY : Directory is not empty    */
/* 5B~63: Reserve */
#define FS_ERR_0064         (FS_ERR_BASE | 100U)       /* PF_ERR_EMOD_NOTSPRT: Module is not supported      */
#define FS_ERR_0065         (FS_ERR_BASE | 101U)       /* PF_ERR_EMOD_NOTREG : Module is not registered     */
#define FS_ERR_0066         (FS_ERR_BASE | 102U)       /* PF_ERR_EMOD_FCS    : FreeClusterSearch Module Err */
#define FS_ERR_0067         (FS_ERR_BASE | 103U)       /* PF_ERR_EMOD_SAFE   : Safe Module Err              */
/* 68~7A: Reserve */
#define FS_ERR_007B         (FS_ERR_BASE | 123U)       /* PF_ERR_ENOMEDIUM: No medium found              */
/* 7C ~ 0100  : Reserve */
/* 0101 ~ 0118: For FS safe module */
#define FS_ERR_0101         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_IMPOSSIBLE))           /* An inexecutable call occurred */
#define FS_ERR_0102         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_UNEXPECTED))           /* Unexpected state occurred */
#define FS_ERR_0103         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_INFO_NOT_INIT))        /* An uninitialized PFSAFE_INFO-type variable is set */
#define FS_ERR_0104         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_INFO_OVERLAP))         /* A PFSAFE_INFO-type variable that is the same as another drive is set */
#define FS_ERR_0105         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_INFO_FULL))            /* FAT Safe is registered more than the number of drives that PrFILE2 may manage */
#define FS_ERR_0106         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_INFO_NOT_REGISTERED))  /* Attempted to release unregistered PFSAFE_INFO-type information */
#define FS_ERR_0107         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_INVALID_PARAM))        /* An invalid parameter is set */
#define FS_ERR_0108         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_INVALID_FILENAME))     /* An invalid recovery file name is specified */
#define FS_ERR_0109         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_MALLOC))               /* Failed to allocate memory */
#define FS_ERR_010A         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FREE))                 /* Failed to release memory */
#define FS_ERR_010B         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_UNSUPPORTED_REQ))      /* An unsupported request occurred */
#define FS_ERR_010C         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_UNSUPPORTED_FAT_TYPE)) /* Unsupported FAT type*/
#define FS_ERR_010D         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_RECOVERY_REQUIRED))    /* File system recovery is required */
#define FS_ERR_010E         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_VOLUME_FULL))          /* Unable to allocate backup area on media */
#define FS_ERR_010F         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_NOT_AVAILABLE))        /* Attempted to execute an unavailable API */
#define FS_ERR_0110         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FILE_OPENED))          /* Recovery file is opened */
#define FS_ERR_0111         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FAT_GETBPB))           /* Failed to get drive information */
#define FS_ERR_0112         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FAT_READSECT))         /* Failed to read from media */
#define FS_ERR_0113         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FAT_WRITESECT))        /* Failed to write to media */
#define FS_ERR_0114         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FAT_GETERRNUM))        /* Failed to get error number */
#define FS_ERR_0115         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FAT_SETDRVCONF))       /* Failed to set drive information */
#define FS_ERR_0116         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FAT_GETDRVCONF))       /* Failed to get drive information */
#define FS_ERR_0117         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FAT_PFAPI))            /* Failed to execute an internal API */
#define FS_ERR_0118         (FS_ERR_BASE | FS_SAFE_ERR_BASE | (-PFSAFE_ERR_FAT_GETCLST))          /* Failed to get cluster information */
/* 119~0FFE: Reserve */
#define FS_ERR_0FFF         (FS_ERR_BASE | 0xFFFU)    /* PF_ERR_SYSTEM :system error(general error)  */
#define FS_ERR_1000         (FS_ERR_BASE | 0x1000U)   /* Driver Error                 */


#define FS_ERR_0302         (FS_ERR_BASE | 302U)       /* EBADFSYS: Corrupted filesystem detected */
#define FS_ERR_0049         (FS_ERR_BASE | 49U)        /* EDQUOT: Disk quota exceeded */
#define FS_ERR_0027         (FS_ERR_BASE | 27U)        /* EFBIG: File is too large */
#define FS_ERR_0028         (FS_ERR_BASE | 28U)        /* ENOSPC: No space left on device */

/*
 * Error code for AmbaFS internal use.
 */
#define AMBA_FS_ERR_NONE    (OK)
#define AMBA_FS_ERR_SYSTEM  (FS_ERR_0FFF)
#define AMBA_FS_ERR_ARG     (FS_ERR_0000)
#define AMBA_FS_ERR_API     (FS_ERR_0004)

#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)

/*
 * Seek mode for _fseek()
 */
#define AMBA_FS_SEEK_START              SEEK_SET     /* current */
#define AMBA_FS_SEEK_CUR                SEEK_CUR     /* head    */
#define AMBA_FS_SEEK_END                SEEK_END     /* end     */

/*
 * Seek mode for _fseek()
 */
#define AMBA_FS_SEEK_START              SEEK_SET     /* current */
#define AMBA_FS_SEEK_CUR                SEEK_CUR     /* head    */
#define AMBA_FS_SEEK_END                SEEK_END     /* end     */

/* fsexec specified types */
#define AMBA_FS_FSEXEC_REMOVE  (0x0)   /* Deletes the file or directory */
#define AMBA_FS_FSEXEC_CHGMOD  (0x06)  /* Changes the file or directory attributes */
#define AMBA_FS_FSEXEC_FOPEN   (0x10)  /* Opens the file */
#define AMBA_FS_FSEXEC_OPENDIR (0x20)  /* Opens the directory */

/* fsexec specified types */
#define AMBA_FS_FSEXEC_WRITE            (0x1U)
#define AMBA_FS_FSEXEC_READ             (0x2U)
#define AMBA_FS_FSEXEC_APPEND           (0x4U)
#define AMBA_FS_FSEXEC_PLUS             (0x8U)

/*
 * Cache Invalidation mode for _sync()
 */
#define AMBA_FS_NINVALIDATE             (0U)
#define AMBA_FS_INVALIDATE              (1U)

/* Other file attribute */
#define AMBA_FS_ATTR_ALL                (0U)        /* for _FileSearchFirst function */

#define AMBA_FS_ATTR_ARCH               (0U)
#define AMBA_FS_ATTR_RDONLY             ((UINT32)S_IRUSR | (UINT32)S_IRGRP | (UINT32)S_IROTH)
#define AMBA_FS_ATTR_NONE               (0U)
#define AMBA_FS_ATTR_HIDDEN             (0U)
#define AMBA_FS_ATTR_VOLUME             (0U)
#define AMBA_FS_ATTR_DIR                (S_IFDIR)

#else /* CONFIG_QNX */

/*
 * Buffering mode for _buffering()
 */
#define AMBA_FS_EJECT_SAFE_WITH_WRITE_THRU      PF_EJECT_SAFE_WITH_WRITE_THRU
#define AMBA_FS_EJECT_SAFE_WITH_WRITE_BACK      PF_EJECT_SAFE_WITH_WRITE_BACK
#define AMBA_FS_WRITE_BACK_ON_SIGNIF_API        PF_WRITE_BACK_ON_SIGNIF_API
#define AMBA_FS_WRITE_BACK_CACHE                PF_WRITE_BACK_CACHE

/*
 * Cache Invalidation mode for _sync()
 */
#define AMBA_FS_NINVALIDATE             PF_NINVALIDATE
#define AMBA_FS_INVALIDATE              PF_INVALIDATE

/*
 * Standard file attribute
 */
#define AMBA_FS_ATTR_RDONLY             ATTR_RDONLY
#define AMBA_FS_ATTR_HIDDEN             ATTR_HIDDEN
#define AMBA_FS_ATTR_SYSTEM             ATTR_SYSTEM
#define AMBA_FS_ATTR_VOLUME             ATTR_VOLUME
#define AMBA_FS_ATTR_DIR                ATTR_DIR
#define AMBA_FS_ATTR_ARCH               ATTR_ARCH

/* Other file attribute */
#define AMBA_FS_ATTR_NONE               ATTR_NONE
#define AMBA_FS_ATTR_ALL                ATTR_ALL        /* for _FileSearchFirst function */
#define AMBA_FS_ATTR_FILE_ONLY          ATTR_NOT_DIR    /* except for directory and volume lavel */

/* File operation mode */
#define AMBA_FS_ATTR_CAND               ATTR_CAND       /* AND Comparing mode */
#define AMBA_FS_ATTR_CMATCH             ATTR_CMATCH     /* MATCH compare mode */
#define AMBA_FS_ATTR_ADD                ATTR_ADD        /* Attribute Change mode */
#define AMBA_FS_ATTR_SUB                ATTR_SUB

/*
 * Clean mode for _CleanDir(), _ChangeDirMode()
 */
#define AMBA_FS_MODE_SEARCH_BELOW       PF_SEARCH_BELOW /* Search mode */
#define AMBA_FS_MODE_SEARCH_TREE        PF_SEARCH_TREE  /* Tree all mode */
#define AMBA_FS_MODE_TEST               PF_SEARCH_TEST  /* Test mode */

/*
 * Seek mode for _fseek()
 */
#define AMBA_FS_SEEK_START              PF_SEEK_SET     /* current */
#define AMBA_FS_SEEK_CUR                PF_SEEK_CUR     /* head    */
#define AMBA_FS_SEEK_END                PF_SEEK_END     /* end     */

/* fsexec specified types */
#define AMBA_FS_FSEXEC_REMOVE           PF_FSEXEC_REMOVE
#define AMBA_FS_FSEXEC_CHGMOD           PF_FSEXEC_CHGMOD
#define AMBA_FS_FSEXEC_FOPEN            PF_FSEXEC_FOPEN
#define AMBA_FS_FSEXEC_OPENDIR          PF_FSEXEC_OPENDIR
#define AMBA_FS_FSEXEC_DELETEDIR        PF_FSEXEC_DELETEDIR
#define AMBA_FS_FSEXEC_DTA_EXEC         PF_FSEXEC_DTA_EXEC

/* fsexec specified types */
#define AMBA_FS_FSEXEC_WRITE            PF_FSEXEC_WRITE
#define AMBA_FS_FSEXEC_READ             PF_FSEXEC_READ
#define AMBA_FS_FSEXEC_APPEND           PF_FSEXEC_APPEND
#define AMBA_FS_FSEXEC_PLUS             PF_FSEXEC_PLUS
#define AMBA_FS_FSEXEC_WRITE_ONLY       PF_FSEXEC_WRITE_ONLY
#define AMBA_FS_FSEXEC_CONT_CLUSTER     PF_FSEXEC_CONT_CLUSTER

/*
 * For PF_SAFE_STAT.ss_stat
 */
#if 0
#define AMBA_FS_SAFE_STAT_NORMAL        PF_SAFE_STAT_NORMAL
#define AMBA_FS_SAFE_STAT_ABNORMAL      PF_SAFE_STAT_ABNORMAL
#define AMBA_FS_SAFE_STAT_ERROR         PF_SAFE_STAT_ERROR
#define AMBA_FS_SAFE_STAT_UNSUPPORTED   PF_SAFE_STAT_UNSUPPORTED

typedef struct {
    UINT32   Stat;    /* Status of filesystem */
    UINT32   ApiCode; /* API code number      */
} AMBA_FS_SAFE_STAT_s;
#endif

#endif

/*
 * Defined in AmbaFS.c
 */
UINT32 AmbaFS_Init(void);

/*
 * input/output functions
 */
/* File access */
UINT32 AmbaFS_FileOpen(const char *pFileName, const char *pMode, AMBA_FS_FILE **pFile);
UINT32 AmbaFS_FileClose(AMBA_FS_FILE *pFile);

/* Direct input/output */
UINT32 AmbaFS_FileRead(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess);
UINT32 AmbaFS_FileWrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess);

/* File positioning */
UINT32 AmbaFS_FileTell(AMBA_FS_FILE * pFile, UINT64* pFilePos);
UINT32 AmbaFS_FileSeek(AMBA_FS_FILE *pFile, INT64 Offset, INT32 Origin);
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
UINT32 AmbaFS_FileSetPos(AMBA_FS_FILE *pFile, AMBA_FS_FPOS *pFilePos);
UINT32 AmbaFS_FileGetPos(AMBA_FS_FILE *pFile, AMBA_FS_FPOS *pFilePos);
#else
UINT32 AmbaFS_FileSetPos(AMBA_FS_FILE *pFile, const UINT64 *pFilePos);
UINT32 AmbaFS_FileGetPos(AMBA_FS_FILE *pFile, UINT64 *pFilePos);
#endif

/* Error handling */
UINT32 AmbaFS_FileEof(AMBA_FS_FILE *pFile);
UINT32 AmbaFS_GetError(void);

/* Operations on files */
UINT32 AmbaFS_Remove(const char *pFileName);
UINT32 AmbaFS_Rename(const char *pOldName, const char *pNewName);

UINT32 AmbaFS_Move(const char *pSrcName, const char *pDstName);
UINT32 AmbaFS_FileCombine(const char *pHeadFileName, const char *pTailFileName);
UINT32 AmbaFS_FileDivide(const char *pSrcFileName, const char *pNewFileName, UINT64 Offset);
UINT32 AmbaFS_FileTruncate(const char *pFileName, UINT64 Offset);
UINT32 AmbaFS_ClusterAppend(AMBA_FS_FILE * pFile, UINT64 Size, UINT64 *pNumSuccess);
UINT32 AmbaFS_ClusterTrimTailing(AMBA_FS_FILE * pFile);
UINT32 AmbaFS_ClusterInsert(const char *pFileName, UINT32 Offset, UINT32 Number);
UINT32 AmbaFS_ClusterDelete(const char *pFileName, UINT32 Offset, UINT32 Number);

UINT32 AmbaFS_ChangeDirAttributes(const char *pDirName, UINT32 Attr);
UINT32 AmbaFS_ChangeFileMode(const char *pFileName, UINT32 Attr);
UINT32 AmbaFS_ChangeDirMode(const char *pDirName, const char *pFileName, UINT32 Mode, UINT32 Attr, UINT32 *pCount);

UINT32 AmbaFS_GetFileInfo(const char *pName, AMBA_FS_FILE_INFO_s *pInfo);

UINT32 AmbaFS_FileSearchFirst(const char *pName, UINT8 Attr, AMBA_FS_DTA_t *pFileSearch);
UINT32 AmbaFS_FileSearchNext(AMBA_FS_DTA_t *pFileSearch);
UINT32 AmbaFS_FileSearchExec(AMBA_FS_DTA_t *pFileSearch, INT32 Operation, UINT8 Attr);
UINT32 AmbaFS_FileSearchFinish(AMBA_FS_DTA_t *pFileSearch);

UINT32 AmbaFS_ChangeDir(const char *pDirName);
UINT32 AmbaFS_MakeDir(const char *pDirName);
UINT32 AmbaFS_RemoveDir(const char *pDirName);    /* Delete the empty directory */
UINT32 AmbaFS_DeleteDir(const char * pDirName);   /* Delete the directory and all its contents */
UINT32 AmbaFS_CleanDir(const char *pDirName);

UINT32 AmbaFS_OpenDir(const char *pDirName, AMBA_FS_DIR **pDir);

UINT32 AmbaFS_CloseDir(AMBA_FS_DIR* pDir);
UINT32 AmbaFS_ReadDir(AMBA_FS_DIR* pDir, AMBA_FS_DIRENT* pDirEntry);
UINT32 AmbaFS_TellDir(AMBA_FS_DIR* pDir, INT32* pOffset);
UINT32 AmbaFS_SeekDir(AMBA_FS_DIR* pDir, INT32 Offset);
UINT32 AmbaFS_RewindDir(AMBA_FS_DIR* pDir);

INT32 AmbaFS_GetUtcOffsetMinutes(void);
UINT32 AmbaFS_SetUtcOffsetMinutes(INT32 UtcOffsetMinutes);

UINT32 AmbaFS_SetTimeStamp(const char * pFileName, const AMBA_FS_TIMESTMP_s * pTimestamp);
UINT32 AmbaFS_GetTimeStamp(const char * pFileName, AMBA_FS_TIMESTMP_s * pTimestamp);

UINT32 AmbaFS_GetDriveInfo(char Drive, AMBA_FS_DRIVE_INFO_t *pDriveInfo);
UINT32 AmbaFS_SetVolumeLabel(char Drive, const char *pVolName);
UINT32 AmbaFS_GetVolumeLabel(char Drive, AMBA_FS_VOL_INFO_t *pVolInfo);
UINT32 AmbaFS_RemoveVolumeLabel(char Drive);
UINT32 AmbaFS_Format(char Drive);
UINT32 AmbaFS_Sync(char Drive, INT32 Mode);
UINT32 AmbaFS_FileSync(AMBA_FS_FILE * pFile);
UINT32 AmbaFS_SetBufferingMode(char Drive, INT32 Mode);
UINT32 AmbaFS_Mount(char Drive);
UINT32 AmbaFS_UnMount(char Drive);

UINT32 AmbaFS_GetFileError(AMBA_FS_FILE * pFile);

#endif  /* AMBA_FS_H */
