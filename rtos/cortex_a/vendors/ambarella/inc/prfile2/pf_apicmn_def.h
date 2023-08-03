/*
     Copyright (C) 2009 By eSOL Co.,Ltd. Tokyo, Japan

     This software is protected by the law and the agreement concerning
     a Japanese country copyright method, an international agreement,
     and other intellectual property right and may be used and copied
     only in accordance with the terms of such license and with the inclusion
     of the above copyright notice.

     This software or any other copies thereof may not be provided
     or otherwise made available to any other person.  No title to
     and ownership of the software is hereby transferred.

     The information in this software is subject to change without
     notice and should not be construed as a commitment by eSOL Co.,Ltd.
 */
 /****************************************************************************
 [pf_apicmn_def.h] - Definitions and Declarations commonly used in PrFILE2 API.

 NOTES:
  - Except headers which are included by 'pf_apicmn.h' itself, any of
    source (*.c) and header (*.h) files of PrFILE2 shall include this
    header 'pf_apicmn.h' (or 'pf_common.h' which includes the header
    internally) prior to any other header.
 ****************************************************************************/
#ifndef PF_APICMN_DEF_H
#define PF_APICMN_DEF_H

#include "prfile2/pf_config.h"
#include "prfile2/pf_types.h"


/****************************************************************************
  Internal Definitions and Declarations

  NOTES:
   - These definitions and declarations are only for internal use in PrFILE2;
     User don't have to use them in user applications.
 ****************************************************************************/
/*---------------------------------------------------------------------------
  File Lock Mode
 ----------------------------------------------------------------------------*/
#define PF_LOCK_SH                  (0x01U)   /* sharing lock       */
#define PF_LOCK_EX                  (0x02U)   /* exclusion lock     */
#define PF_LOCK_UN                  (0x04U)   /* unlock             */
#define PF_LOCK_NB                  (0x08U)   /* non-blocking mode  */


/*---------------------------------------------------------------------------
  Directory Lock Mode
 ----------------------------------------------------------------------------*/
#define PF_DLOCK_EX                 (0x02U)   /* exclusion lock     */
#define PF_DLOCK_UN                 (0x04U)   /* unlock             */
#define PF_DLOCK_NB                 (0x08U)   /* non-blocking mode  */

#define PF_DLOCK_MASK               (PF_DLOCK_EX | PF_DLOCK_UN | PF_DLOCK_NB)


/*---------------------------------------------------------------------------
  Cluster link information
 ----------------------------------------------------------------------------*/
#define PF_CLST_LINK_ENABLE         (1U)
#define PF_CLST_LINK_DISABLE        (0U)


/*---------------------------------------------------------------------------
  Maximum length of primary part of filename
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_BASE_SIZE        (8U)


/*---------------------------------------------------------------------------
  Maximum length of extention of filename
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_EXT_SIZE         (3U)


/*---------------------------------------------------------------------------
  Maximum length of Short/Long File Name in characters
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_CHARS            (11U)   /* = 8 + 3      */
#define SHORT_DOT_NAME_CHARS        (12U)   /* = 8 + 1 + 3  */
#define LONG_NAME_CHARS             (255U)


/*---------------------------------------------------------------------------
  Maximum size of Short/Long File Name in bytes
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_SIZE             (11U)   /* = SHORT_NAME_CHARS       */
#define SHORT_DOT_NAME_SIZE         (12U)   /* = SHORT_DOT_NAME_CHARS   */
#define LONG_NAME_SIZE              (510U)  /* = LONG_NAME_CHARS * 2    */

#define VOLUME_LABEL_SIZE_FAT       (11U)   /* 11[byte]                 */

#if PF_EXFAT_SUPPORT
#define VOLUME_LABEL_SIZE_EXFAT     (11U)   /* 11[Characters] (UNICODE) */
#endif


/*---------------------------------------------------------------------------
  Buffer size for Short/Long File Name in bytes
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_BUF_SIZE         (SHORT_DOT_NAME_SIZE+1U)    /* Including NUL    */
#define LONG_NAME_BUF_SIZE          (LONG_NAME_SIZE+2U)         /* Including NUL    */

#define NAME_BUF_SIZE               LONG_NAME_BUF_SIZE

#if PF_EXFAT_SUPPORT
#define VOLUME_LABEL_CHARS          (VOLUME_LABEL_SIZE_EXFAT)   /* Without NUL */
#define VOLUME_LABEL_BUF_SIZE       (VOLUME_LABEL_CHARS*2U)     /* Without NUL */
#else
#define VOLUME_LABEL_CHARS          (VOLUME_LABEL_SIZE_FAT)     /* Without NUL */
#define VOLUME_LABEL_BUF_SIZE       (VOLUME_LABEL_CHARS)        /* Without NUL */
#endif

/*---------------------------------------------------------------------------
  Maximum path length of Short/Long File Name in characters
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_PATH_CHARS       (80U-1U)    /* Without NUL              */
#define LONG_NAME_PATH_CHARS        (260U-1U)   /* Without NUL              */


/*---------------------------------------------------------------------------
  Maximum path size of Short/Long File Name in bytes
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_PATH_SIZE        SHORT_NAME_PATH_CHARS       /* Without NUL */
#define LONG_NAME_PATH_SIZE         (LONG_NAME_PATH_CHARS*2U)   /* Without NUL */


/*---------------------------------------------------------------------------
  Buffer size for path of Short/Long File Name in bytes
 ----------------------------------------------------------------------------*/
#define SHORT_NAME_PATH_BUF_SIZE    (SHORT_NAME_PATH_SIZE+1U)   /* Including NUL    */
#define LONG_NAME_PATH_BUF_SIZE     (LONG_NAME_PATH_SIZE+2U)    /* Including NUL    */

#define PATH_BUF_SIZE               LONG_NAME_PATH_BUF_SIZE


/*---------------------------------------------------------------------------
  Volume information
 ----------------------------------------------------------------------------*/
#ifndef AMBA_FS_H
typedef struct PF_VOLUME            PF_VOLUME;
#else
/* #define AMBA_FS_KAL */
/* For MisraC checking */
#define PF_VOLUME            void
#endif

/* AMBA_FS_KAL */
#if PF_BACKWARD_COMPATIBILITY_STRUCTURE
/*---------------------------------------------------------------------------
  PF_FUNCTBL - Driver Interface Function Table

  NOTES:
   - This is equivalent of 'FUNCTBL' for PrFILE1.
 ----------------------------------------------------------------------------*/
typedef struct {
    /* Initialize drive for attach */
    PF_ERROR    (*init)(PF_VOLUME*);

    /* Finalize drive for detach */
    PF_ERROR    (*finalize)(PF_VOLUME*);

    /* Mount driver to access */
    PF_ERROR    (*mount)(PF_VOLUME*);

    /* Un-mount driver */
    PF_ERROR    (*unmount)(PF_VOLUME*);

    /* Format the volume */
    PF_ERROR    (*format)(PF_VOLUME*, const PF_U_CHAR*);

    /* Read logical sectors */
    PF_ERROR    (*lread)(PF_VOLUME* p_vol, PF_U_CHAR* buf, PF_U_LONG sector, PF_U_LONG num, PF_U_LONG* success_num);

    /* Write logical sectors*/
    PF_ERROR    (*lwrite)(PF_VOLUME* p_vol, const PF_U_CHAR* buf, PF_U_LONG sector, PF_U_LONG num, PF_U_LONG* success_num);

} PF_FUNCTBL;

typedef PF_FUNCTBL                  FUNCTBL;        /* OBSOLETE         */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/****************************************************************************
  Public Type Definitions

  NOTES:
   - These types are used with API functions in user applications.
 ****************************************************************************/
/*---------------------------------------------------------------------------
  File information for pf_create(), pf_fopen(), pf_fclose(), ...

  NOTES:
   - This type is intentionally declared as 'Incomplete Type';
     Users should use this type only in the form of 'PF_FILE *'.
 ----------------------------------------------------------------------------*/
#ifndef AMBA_FS_H
typedef struct PF_FILE              PF_FILE;
#else
/* #define AMBA_FS_KAL */
/* For MisraC checking */
#define PF_FILE     void
#endif

#if PF_STDLIB_ALIAS_SUPPORT
  #define FILE                      PF_FILE
#endif /* PF_STDLIB_ALIAS_SUPPORT */


/*---------------------------------------------------------------------------
  Directory information for pf_opendir(), pf_closedir(), pf_fchdir(), ...

  NOTES:
   - This type is intentionally declared as 'Incomplete Type';
     Users should use this type only in the form of 'PF_DIR *'.
 ----------------------------------------------------------------------------*/
#ifndef AMBA_FS_H
typedef struct PF_DIR               PF_DIR;
#else
/* #define AMBA_FS_KAL */
/* For MisraC checking */
#define PF_DIR     void
#endif

/*---------------------------------------------------------------------------
  Tail Tracker Buffer for pf_settailbuf().
 ----------------------------------------------------------------------------*/
typedef PF_U_LONG  PF_TAIL_BUF;


#if PF_USE_CLSTLINK
/*---------------------------------------------------------------------------
  Cluster Link Buffer for pf_setclstlink().

  NOTES:
   - This type is setting for volume structure.
 ----------------------------------------------------------------------------*/
typedef PF_U_LONG  PF_CLSTSET_BUF[PF_MAX_FILE_NUM];


/*---------------------------------------------------------------------------
  Cluster Link Buffer for pf_fsetclstlink().

  NOTES:
   - This type is setting for FAT file discripter(within PF_CLUSTER_LINK).
 ----------------------------------------------------------------------------*/
typedef PF_U_LONG  PF_CLUSTER_BUF;


/*---------------------------------------------------------------------------
  Cluster Link Information for pf_setclstlink().
 ----------------------------------------------------------------------------*/
typedef struct {
    /* Cluster Link Buffer */
    PF_CLSTSET_BUF*   buffer;

    /* Cluster Link Max */
    PF_U_LONG         max_count;

    /* Cluster Link Interval */
    PF_U_SHORT        interval;

} PF_CLUSTER;


/*---------------------------------------------------------------------------
  Cluster Link Information for pf_fsetclstlink().
 ----------------------------------------------------------------------------*/
typedef struct {
    /* Cluster Link Buffer */
    PF_CLUSTER_BUF*   buffer;

    /* Cluster Link Max */
    PF_U_LONG         max_count;

    /* Cluster Link Interval */
    PF_U_SHORT        interval;

} PF_CLUSTER_FILE;
#endif /* PF_USE_CLSTLINK */


/*---------------------------------------------------------------------------
  FAT search
 ----------------------------------------------------------------------------*/
typedef struct
{
    /* The index of 'ci_clst' in the cluster chain (0 origin) */
    PF_CLST     ci_idx;

    /* The cluster number */
    PF_CLST     ci_clst;
} PF_CLST_IDX_TAG;
typedef PF_CLST_IDX_TAG  PF_CLST_IDX;

/*---------------------------------------------------------------------------
  Last Access Cluster Number and Index
  * Holds information about the position of the last access cluster number
  * and index of the cluster in chain.
  *
  * Each PF_FILE (=UFD: user file descriptor) holds its own pointer to a
  * structure of this type describing the last access to the file made
  * through it. The goal is to avoid going through the file's whole FAT
  * chain each time an access is made to it's contents.
  *
  * To be used in the FAT modules' implementation only.
 ----------------------------------------------------------------------------*/
typedef PF_CLST_IDX_TAG  PF_FAT_HINT;


/*---------------------------------------------------------------------------
  Information for both pf_fsfirst() and pf_fsnext().
 ----------------------------------------------------------------------------*/
typedef struct {
    /*-----------------------------------------------------------------------*
      User fields: this is for user area, fsexec API sets file/dir pointer to this area/
     *-----------------------------------------------------------------------*/
    /* File descriptor */
    PF_FILE*    p_file;

    /* Directory descriptor */
    PF_DIR*     p_dir;

    /*-----------------------------------------------------------------------*
      Internal fields: do not use these as they will change without notice
     *-----------------------------------------------------------------------*/
    /* Volume of the parent directory   */
    PF_VOLUME*  p_vol;

    /* Parent directory start cluster */
    PF_U_LONG   parent_start_cluster;

    /* Next entry position within parent */
    PF_U_LONG   parent_pos; /* started from 1 */

    /* Last access cluster information */
    PF_FAT_HINT hint;

    /* Status of PF_DTA */
    PF_U_LONG   status;

    /* Located path length */
#if PF_CHECK_PATH_LENGTH
    PF_U_LONG   path_len;
#endif /* PF_CHECK_PATH_LENGTH */

    /* Number of LFNs; If it is zero, string in 'long_name' field is not valid */
    PF_U_CHAR   num_entry_LFNs;

    /* Ordinal field of LFN entry, without LAST_LONG_ENTRY flag */
    PF_U_CHAR   ordinal;

    /* FAT  : Checksum field of LFN entry
     * exFAT: Hash field of File name     */
    PF_U_SHORT   check_sum;

#if PF_EXFAT_SUPPORT
    /* Parent directory GeneralSecondaryFlags */
    PF_U_CHAR parent_general_flg;

    /* Parent directory size in bytes */
    PF_FSIZE_T parent_dir_size;

    /* Parent Path */
    PF_WCHAR parent_path[LONG_NAME_PATH_CHARS + 1U];

    /* Number of Secondary Entry */
    PF_U_CHAR   num_secondary;

    /* GeneralSecondaryFlags */
    PF_U_CHAR   general_flg;
#endif

    /* Attributes searched for */
    PF_U_CHAR   attr;

    /* Regular expression for file name */
    PF_CHAR     reg_exp[PATH_BUF_SIZE];


    /*-----------------------------------------------------------------------*
      Public fields:
     *-----------------------------------------------------------------------*/
#if PF_EXFAT_SUPPORT
    /* Modification milliseconds (actually contains a count of tenths of a second) */
    PF_U_SHORT  TimeMs;
#endif

    /* Modification time */
    PF_U_SHORT  Time;

    /* Modification date */
    PF_U_SHORT  Date;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC of Modification time */
    PF_U_CHAR   UtcOffset;
#endif

#if PF_EXFAT_SUPPORT
    /* Access time */
    PF_U_SHORT  ATime;
#endif

    /* Access date */
    PF_U_SHORT  ADate;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC of Access time */
    PF_U_CHAR   AUtcOffset;
#endif

    /* Create milliseconds (actually contains a count of tenths of a second) */
    PF_U_SHORT  CTimeMs;

    /* Create time */
    PF_U_SHORT  CTime;

    /* Create date */
    PF_U_SHORT  CDate;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC of Create time */
    PF_U_CHAR   CUtcOffset;
#endif

    /* File size in bytes */
    PF_FSIZE_T  FileSize;

    /* File attributes */
    PF_U_SHORT  Attribute;

    /* Short file name (8.3) */
    PF_CHAR     FileName[SHORT_NAME_BUF_SIZE];

    /* Long file name */
    PF_CHAR     LongName[LONG_NAME_BUF_SIZE];

} PF_DTA;

#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_DTA                      DTA;            /* OBSOLETE */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/*---------------------------------------------------------------------------
  Information for pf_fsexec().
 ----------------------------------------------------------------------------*/
typedef struct {
    /*-----------------------------------------------------------------------*
      User fields: this is for user area, fsexec API sets file/dir pointer to this area/
     *-----------------------------------------------------------------------*/
    /* File descriptor */
    PF_FILE*    p_file;

    /* Directory descriptor */
    PF_DIR*     p_dir;

    /*-----------------------------------------------------------------------*
      Internal fields: do not use these as they will change without notice
     *-----------------------------------------------------------------------*/
    /* Volume of the parent directory   */
    PF_VOLUME*  p_vol;

    /* Parent directory start cluster */
    PF_U_LONG   parent_start_cluster;

    /* Next entry position within parent */
    PF_U_LONG   parent_pos; /* started from 1 */

    /* Last access cluster information */
    PF_FAT_HINT hint;

    /* Status of PF_DTA */
    PF_U_LONG   status;

    /* Located path length */
#if PF_CHECK_PATH_LENGTH
    PF_U_LONG   path_len;
#endif /* PF_CHECK_PATH_LENGTH */

    /* Number of LFNs; If it is zero, string in 'long_name' field is not valid */
    PF_U_CHAR   num_entry_LFNs;

    /* Ordinal field of LFN entry, without LAST_LONG_ENTRY flag */
    PF_U_CHAR   ordinal;

    /* FAT  : Checksum field of LFN entry
     * exFAT: Hash field of File name     */
    PF_U_SHORT   check_sum;

#if PF_EXFAT_SUPPORT
    /* Parent directory GeneralSecondaryFlags */
    PF_U_CHAR parent_general_flg;

    /* Parent directory size in bytes */
    PF_FSIZE_T parent_dir_size;

    /* Parent Path */
    PF_WCHAR parent_path[LONG_NAME_PATH_CHARS + 1U];

    /* Number of Secondary Entry */
    PF_U_CHAR   num_secondary;
#endif /* PF_EXFAT_SUPPORT */

} PF_DTA_EXEC;

/*---------------------------------------------------------------------------
  File information table for pf_fstat().
 ----------------------------------------------------------------------------*/
typedef struct {
    /* file size in bytes */
    PF_FSIZE_T  fstfz;

#if PF_EXFAT_SUPPORT
    /* Last access time */
    PF_U_SHORT  fstact;
#endif

    /* Last access date */
    PF_U_SHORT  fstad;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC to the Last access date and time */
    PF_U_CHAR   fstautc;
#endif

    /* Last modify time */
    PF_U_SHORT  fstut;

#if PF_EXFAT_SUPPORT
    /* Component of the file Last modify time */
    PF_U_SHORT  fstuc;
#endif

    /* Last modify date */
    PF_U_SHORT  fstud;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC to the Last modify date and time */
    PF_U_CHAR   fstuutc;
#endif

    /* Create time      */
    PF_U_SHORT  fstct;

    /* Create date      */
    PF_U_SHORT  fstcd;

    /* Component of the file creation time */
    PF_U_SHORT  fstcc;

#if PF_EXFAT_SUPPORT
    /* Offset from UTC to the Create date and time */
    PF_U_CHAR   fstcutc;
#endif

    /* file attributes */
    PF_U_CHAR   fstat;
} PF_STAT;

#if PF_STDLIB_ALIAS_SUPPORT
  #define _stat                     PF_STAT
#endif /* PF_STDLIB_ALIAS_SUPPORT */


/*---------------------------------------------------------------------------
  Drive information table for pf_getdev().
 ----------------------------------------------------------------------------*/
typedef enum
{
    PF_FMT_FAT12  = 0,
    PF_FMT_FAT16  = 1,
    PF_FMT_FAT32  = 2,
    PF_FMT_EXFAT  = 3,
    PF_FMT_ERR    = -1
} PF_FAT_TYPE;

typedef struct {
    /* total number of logical clusters */
    PF_U_LONG   cls;

    /* number of empty clusters */
    PF_U_LONG   ecl;

    /* bytes per logical sector */
    PF_U_LONG   bps;

    /* logical sectors per cluster */
    PF_U_LONG   spc;

    /* clusters per cluster group */
    PF_U_LONG   cpg;

    /* number of empty cluster groups */
    PF_U_LONG   ecg;

    /* format type */
    PF_FAT_TYPE fmt;

    /* Disk attribute */
    PF_U_LONG   attr;

} PF_DEV_INF;

#define PF_DEV_ATTR_READONLY    (0x00000001UL)
#define PF_DEV_ATTR_REMOVABLE   (0x00000002UL)

#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_DEV_INF                  DEV_INF;            /* OBSOLETE */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/*---------------------------------------------------------------------------
  Volume label table for pf_getvol().
 ----------------------------------------------------------------------------*/
typedef struct
{
    /* Attributes of the volume label */
    PF_U_CHAR   v_attr;

    /* Date */
    PF_U_LONG   v_date;

    /* Time */
    PF_U_LONG   v_time;

    /* Volume label string (null-terminated) */
    PF_CHAR     v_name[VOLUME_LABEL_BUF_SIZE+1U];
} PF_VOLTAB;


#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_VOLTAB                   VOLTAB;             /* OBSOLETE */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/*---------------------------------------------------------------------------
    Cache Setting Table for pf_attach()

 ----------------------------------------------------------------------------*/
/* Cache buffer block size */
#define SECTOR_BUF_SIZE                 (512U)
#define LOG2_SECTOR_BUF_SIZE            (9U)


/*---------------------------------------------------------------------------
  PF_CACHE_PAGE
 ----------------------------------------------------------------------------*/
typedef struct PF_CACHE_PAGE
{
    /* Cache page status */
    PF_U_SHORT      stat;

    PF_U_SHORT      option;         /* User area (dose not use cache module) */
    PF_U_CHAR*      buffer;         /* the head address in buffer            */
    PF_U_CHAR*      p_buf;          /* the current pointer in buffer         */
    PF_U_CHAR*      p_mod_sbuf;     /* the start modified pointer in buffer  */
    PF_U_CHAR*      p_mod_ebuf;     /* the end modified pointer in buffer    */
    PF_U_LONG       size;           /* number of sector per buffer           */
    PF_U_LONG       sector;         /* the head sector number                */
    void*           signature;
    struct PF_CACHE_PAGE*  p_next;
    struct PF_CACHE_PAGE*  p_prev;
} PF_CACHE_PAGE;


/*---------------------------------------------------------------------------
  PF_SECTOR_BUF
 ----------------------------------------------------------------------------*/
typedef PF_U_CHAR  PF_SECTOR_BUF[SECTOR_BUF_SIZE];
typedef struct
{
    PF_CACHE_PAGE  *pages;            /* Cache page                 */
    PF_SECTOR_BUF  *buffers;          /* Cache buffer               */
    PF_U_SHORT     num_fat_pages;
    PF_U_SHORT     num_data_pages;
    PF_U_LONG      num_fat_buf_size;
    PF_U_LONG      num_data_buf_size;
} PF_CACHE_SETTING;


/*---------------------------------------------------------------------------
  Drive Table for pf_attach() (and pf_mount())

    NOTES: This is equivalent of 'DRV_TBL' for PrFILE1.
 ----------------------------------------------------------------------------*/
typedef struct {
    /* Pointer to a partition */
    void*           p_part;

    /* Cache Settings */
    PF_CACHE_SETTING* cache;

    /* Attached driver number */
    PF_DRV_CHAR     drive;

    /* Drive status */
    PF_U_CHAR       stat;

} PF_DRV_TBL;

#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_DRV_TBL                  DRV_TBL;        /* OBSOLETE             */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */

/* The flag of 'stat' of PF_DRV_TBL */
#define PF_ATTACHED             (0x01U)
#define PF_MOUNTED              (0x02U)
#define PF_DISK_INSERTED        (0x04U)


/*---------------------------------------------------------------------------
  File information table for pf_finfo().
 ----------------------------------------------------------------------------*/
typedef struct {

    /* file size in bytes */
    PF_FSIZE_T  file_size;

    /* file I/O pointer in bytes */
    PF_FPOS_T   io_pointer;

    /* file size in bytes */
    PF_FSIZE_T  empty_size;

    /* file size in bytes */
    PF_FSIZE_T  allocated_size;

    /* file lock mode */
    PF_U_LONG   lock_mode;

    /* file lock owner */
    PF_FILE*    lock_owner;

    /* lock count by this descriptor */
    PF_U_LONG   lock_count;

    /* total lock count by all descriptor */
    PF_U_LONG   lock_tcount;

    /* file open mode */
    PF_U_LONG   open_mode;

    /* file start cluster number */
    PF_CLST     start_cluster;

} PF_INFO;


#if PF_ICONV_SUPPORT
/*---------------------------------------------------------------------------
 iconv conversion functions table
 ----------------------------------------------------------------------------*/
#define PF_ICONV_MAX_CODENAME   (16U)

/*
 * This function conversion from a UTF-32 character to a multibytes character that the specified code.
 *
 * [Parameters]
 *   outchar         [OUT]  The output character set data. (e.g. CP932)
 *                          The variable type -- char or wchar -- depends on a conversion routine.
 *   outbytes        [OUT]  The number of bytes converted.
 *   inchar          [IN]   The input character set data. (e.g. UTF32)
 *   outbytesleft    [IN]   Output bytes available to store converted value.
 *   reversible_conv [OUT]  Boolean value for success or failure.
 *
 * [Return]
 *   OK                         Complete.
 *   PF_IER_INVALID_CHARS       The conversion from 'inchar' to the code is impossible.
 *   PF_IER_NOT_ENOUGH_BUFFER   The capacity of 'outchar' is short.
 */
typedef PF_ERROR (*pf_iconv_from_utf32)(void *outchar, PF_SIZE_T *outbytes, PF_UTF32 inchar, PF_SIZE_T outbytesleft, PF_BOOL *reversible_conv);

/*
 * This function conversion from a multibytes character that the speficied code to a UTF-32 character.
 *
 * [Parameters]
 *   outchar         [OUT]  Output character set data. (e.g. UTF32)
 *   convbytes       [OUT]  The number of bytes converted.
 *   inchar          [IN]   The input character set data. (e.g. CP932)
 *                          The variable type -- char or wchar -- depends on a conversion routine.
 *   inbytesleft     [IN]   Input bytes available for conversion.
 *   reversible_conv [OUT]  Boolean value for success or failure.
 *
 * [Return]
 *   OK                         Complete.
 *   PF_IER_INVALID_CHARS       The conversion from 'inchar' to UTF-32 is impossible.
 *   PF_IER_INCOMPLETE_CHARS    'inchar' is incomplete multibyte sequence.
 */
typedef PF_ERROR (*pf_iconv_to_utf32)(PF_UTF32 *outchar, PF_SIZE_T *convbytes, PF_CONST void *inchar, PF_SIZE_T inbytesleft, PF_BOOL *reversible_conv);

typedef struct
{
    PF_CONST PF_CHAR    *ics_codename; /* /^[0-9A-Za-z_-]+$/ is available */
    pf_iconv_from_utf32 ics_from_utf32;
    pf_iconv_to_utf32   ics_to_utf32;
} PF_ICONV_CODESET;


/*---------------------------------------------------------------------------
 iconv descriptor
 ----------------------------------------------------------------------------*/
typedef struct
{
    PF_CONST PF_ICONV_CODESET *ic_from;
    PF_CONST PF_ICONV_CODESET *ic_to;
} PF_ICONV;
#endif /* PF_ICONV_SUPPORT*/


/*---------------------------------------------------------------------------
 Character set of volumes
 ----------------------------------------------------------------------------*/
#if defined AMBA_FS_KAL
/* For MisraC checking. */
typedef struct
{
    /* function to transfer OEM code to Unicode */
    PF_S_LONG   (*oem2unicode)(const PF_CHAR* pSrc, PF_WCHAR* pDst);

    /* function to transfer Unicode to OEM code */
    PF_S_LONG   (*unicode2oem)(const PF_WCHAR* pSrc, PF_CHAR* pDst);

    /* function to get the width of OEM code */
    PF_S_LONG   (*oem_char_width)(const PF_CHAR* buf);

    /* function to check 1st multiple-byte character */
    PF_BOOL     (*is_oem_mb_char)(const PF_CHAR oem_src, PF_U_LONG num);

    /* function to get the width of Unicode */
    PF_S_LONG   (*unicode_char_width)(const PF_WCHAR* buf);

    /* function to check 1st multiple-byte character */
    PF_BOOL     (*is_unicode_mb_char)(const PF_WCHAR uc_src, PF_U_LONG num);

} PF_CHARCODE;

#else
typedef struct
{
    /* function to transfer OEM code to Unicode */
    PF_S_LONG   (*oem2unicode)(const PF_CHAR*, PF_WCHAR*);

    /* function to transfer Unicode to OEM code */
    PF_S_LONG   (*unicode2oem)(const PF_WCHAR*, PF_CHAR*);

    /* function to get the width of OEM code */
    PF_S_LONG   (*oem_char_width)(const PF_CHAR*);

    /* function to check 1st multiple-byte character */
    PF_BOOL     (*is_oem_mb_char)(const PF_CHAR, PF_U_LONG);

    /* function to get the width of Unicode */
    PF_S_LONG   (*unicode_char_width)(const PF_WCHAR*);

    /* function to check 1st multiple-byte character */
    PF_BOOL     (*is_unicode_mb_char)(const PF_WCHAR, PF_U_LONG);

} PF_CHARCODE;
#endif

/*---------------------------------------------------------------------------
  Directory information for pf_readdir()
 ----------------------------------------------------------------------------*/
typedef struct {

    /* long file name field */
    PF_CHAR         long_dname[LONG_NAME_BUF_SIZE];

    /* short file name field */
    PF_CHAR         short_dname[SHORT_NAME_BUF_SIZE];

    /* Attributes */
    PF_U_LONG       attr;

} PF_DIRENT;


/*---------------------------------------------------------------------------
  System date and time
 ----------------------------------------------------------------------------*/
typedef struct
{
    PF_U_SHORT  sys_year;               /* year                */
    PF_U_SHORT  sys_month;              /* month (1-12)        */
    PF_U_SHORT  sys_day;                /* day (1-31)          */

} PF_SYS_DATE;

typedef struct
{
    PF_U_SHORT  sys_hour;               /* hour (0-23)         */
    PF_U_SHORT  sys_min;                /* minites (0-59)      */
    PF_U_SHORT  sys_sec;                /* second (0-59)       */
/* AMBA_FS_KAL */
#if (PF_BACKWARD_COMPATIBILITY_SYSTIME == 0)   /* !PF_BACKWARD_COMPATIBILITY_SYSTIME */
    PF_U_SHORT  sys_ms;                 /* milli second(0-199) */
#if PF_EXFAT_SUPPORT
    /* UTC Offset for exFAT */
    PF_U_SHORT  sys_utc_valid;          /* UTC Offset Valid    */
    PF_S_SHORT  sys_utc;                /* UTC Offset [minites]*/
#endif /* PF_EXFAT_SUPPORT */
#endif /* !PF_BACKWARD_COMPATIBILITY_SYSTIME */

} PF_SYS_TIME;

#define PF_SETSTMP_NONE         (0xFFFFU)   /* No setting */

#define PF_SETSTMP_UTC_INVALID  (0x0000U)
#define PF_SETSTMP_UTC_VALID    (0x0080U)

#if PF_BACKWARD_COMPATIBILITY_STRUCTURE

typedef PF_SYS_DATE             SYS_DATE;   /* OBSOLETE */
typedef PF_SYS_TIME             SYS_TIME;   /* OBSOLETE */

#endif /* PF_BACKWARD_COMPATIBILITY_STRUCTURE */


/*---------------------------------------------------------------------------
  Tiemstamp information
 ----------------------------------------------------------------------------*/
typedef struct
{
    /* create date */
    PF_SYS_DATE     cre_date;

    /* create time */
    PF_SYS_TIME     cre_time;

    /* modified date */
    PF_SYS_DATE     mod_date;

    /* modified time */
    PF_SYS_TIME     mod_time;

    /* last access date */
    PF_SYS_DATE     acc_date;

#if PF_EXFAT_SUPPORT
    /* last access time */
    PF_SYS_TIME     acc_time;
#endif

} PF_TIMESTMP;


/*---------------------------------------------------------------------------
  Divide information for pf_fdivide()
 ----------------------------------------------------------------------------*/
typedef struct
{
    /* Pointer to file path for file to divide */
    PF_CHAR         *fbase;

    /* Cluster offset from the beginning of the file to the division location */
    PF_U_LONG       idivide;

    /* Pointer to file path for file of the first half created after division */
    PF_CHAR         *f1half;

    /* Pointer to file path for file of the latter half created after division */
    PF_CHAR         *f2half;

    /* Size of file of the first half */
    PF_FSIZE_T      f1size;
} PF_FDIVIDE;


/*---------------------------------------------------------------------------
  Seek mode for pf_fseek()
 ----------------------------------------------------------------------------*/
#define PF_SEEK_CUR             (1)             /* current              */
#define PF_SEEK_SET             (0)             /* head                 */
#define PF_SEEK_END             (2)             /* end                  */

#if PF_STDLIB_ALIAS_SUPPORT
  #define SEEK_CUR              PF_SEEK_CUR     /* current              */
  #define SEEK_SET              PF_SEEK_SET     /* head                 */
  #define SEEK_END              PF_SEEK_END     /* end                  */
#endif /* PF_STDLIB_ALIAS_SUPPORT */


/*---------------------------------------------------------------------------
  Buffering mode for pf_buffering()
 ----------------------------------------------------------------------------*/
#define PF_WRTSOON              (0x01U)         /* write soon                */
#define PF_NWRTSOON             (0x00U)         /* not write soon            */
#define PF_AUTOSYNC             (0x02U)         /* cache is not used effectively */
#define PF_NAUTOSYNC            (0x00U)         /* cache is used effectively */
#define PF_FAT_WRITE_THRU       (0x04U)
#define PF_FAT_WRITE_BACK       (0x00U)

#define PF_EJECT_SAFE_WITH_WRITE_THRU   PF_WRTSOON
#define PF_EJECT_SAFE_WITH_WRITE_BACK   (PF_NWRTSOON | PF_AUTOSYNC | PF_FAT_WRITE_THRU)
#define PF_WRITE_BACK_ON_SIGNIF_API     (PF_NWRTSOON | PF_AUTOSYNC)
#define PF_WRITE_BACK_CACHE             (PF_NWRTSOON | PF_NAUTOSYNC)

#if PF_BACKWARD_COMPATIBILITY_PARAMETER
#define NWRTSOON                PF_NWRTSOON     /* not write soon            */
#define WRTSOON                 PF_WRTSOON      /* write soon                */
#endif /* PF_BACKWARD_COMPATIBILITY_PARAMETER */


/*---------------------------------------------------------------------------
  Cache Invalidation mode for pf_sync()
 ----------------------------------------------------------------------------*/
#define PF_INVALIDATE           (0x1)
#define PF_NINVALIDATE          (0x0)


/*---------------------------------------------------------------------------
  FSINFO Setup Flags
 ----------------------------------------------------------------------------*/
#define PF_FSI_TRUST_ENABLE     (0x0001U)       /* Trust FSINFO Data      */
#define PF_FSI_TRUST_DISABLE    (0x0002U)       /* No Trust FSINFO Data   */
#define PF_FSI_REFRESH_ENABLE   (0x0004U)       /* Refresh FSINFO Data    */
#define PF_FSI_REFRESH_DISABLE  (0x0008U)       /* No Refresh FSINFO Data */


/*---------------------------------------------------------------------------
  Unmount mode for pf_unmount()
 ----------------------------------------------------------------------------*/
#define PF_UNMOUNT_NORMAL       (0x00U)         /* normal unmount mode    */
#define PF_UNMOUNT_FORCE        (0x01U)         /* force unmount mode     */


/*---------------------------------------------------------------------------
  DTA status
 ----------------------------------------------------------------------------*/
#define PF_DTA_STAT_LOCAL       (0x00000001U)
#define PF_DTA_STAT_UNICODE     (0x00000002U)


/*---------------------------------------------------------------------------
  DTA path_len
 ----------------------------------------------------------------------------*/
#define PF_DTA_PATHLEN_CDIR_SHIFT   (16U)
#define PF_DTA_PATHLEN_CDIR_MASK    (0xffff0000UL)
#define PF_DTA_PATHLEN_FNAME_SHIFT  (0U)
#define PF_DTA_PATHLEN_FNAME_MASK   (0x0000ffffUL)


/*---------------------------------------------------------------------------
  File attributes
 ----------------------------------------------------------------------------*/
#define ATTR_RDONLY             (0x0001U)           /* read only    */
#define ATTR_HIDDEN             (0x0002U)           /* hidden       */
#define ATTR_SYSTEM             (0x0004U)           /* system       */
#define ATTR_VOLUME             (0x0008U)           /* volume label */
#define ATTR_DIR                (0x0010U)           /* subdirectory */
#define ATTR_ARCH               (0x0020U)           /* archives     */
#define ATTR_NONE               (0x0040U)           /* none         */
#define ATTR_ALL                (0x007FU)           /* for fsfirst function */
#define ATTR_NOT_DIR            (0x0100U)           /* except for directory and volume lavel */

#define ATTR_CAND               (0x0080U)           /* AND compare mode     */
#define ATTR_CMATCH             (0x1000U)           /* MATCH compare mode   */

#define ATTR_ADD                (0x2000U)           /* Attribute add mode   */
#define ATTR_SUB                (0x4000U)           /* Attribute del mode   */

#if PF_DIRCACHE_SUPPORT
#undef ATTR_ALL
#define ATTR_ALL                (0x0077U)           /* = (ATTR_ALL - ATTR_VOLUME) */
                                                    /* for fsfirst function       */
#endif /* PF_DIRCACHE_SUPPORT */

#if PF_BACKWARD_COMPATIBILITY_PARAMETER
#define EATRO                   ATTR_RDONLY         /* read only    */
#define EATHD                   ATTR_HIDDEN         /* hidden       */
#define EATSY                   ATTR_SYSTEM         /* system       */
#define EATVL                   ATTR_VOLUME         /* volume label */
#define EATDR                   ATTR_DIR            /* subdirectory */
#define EATAR                   ATTR_ARCH           /* archives     */
#define EATALL                  ATTR_ALL            /* for fsfirst function */
#endif /* PF_BACKWARD_COMPATIBILITY_PARAMETER */


/*---------------------------------------------------------------------------
  File open mode
 ----------------------------------------------------------------------------*/
#define PF_OPEN_MODE_WRITE          (0x01U)
#define PF_OPEN_MODE_READ           (0x02U)
#define PF_OPEN_MODE_APPEND         (0x04U)
#define PF_OPEN_MODE_PLUS           (0x08U)
#define PF_OPEN_MODE_NO_OVERWRITE   (0x10U)
#define PF_OPEN_MODE_WRITE_ONLY     (0x20U)
#define PF_OPEN_MODE_CONT_CLUSTER   (0x40U)


/*---------------------------------------------------------------------------
  definition/macro for fsexec
 ----------------------------------------------------------------------------*/
/* fsexec specified types */
#define PF_FSEXEC_REMOVE        (0x01U)
#define PF_FSEXEC_CHGMOD        (0x06U)
#define PF_FSEXEC_FOPEN         (0x10U)
#define PF_FSEXEC_OPENDIR       (0x20U)
#define PF_FSEXEC_DELETEDIR     (0x40U)
#define PF_FSEXEC_CHDIR         (0x80U)
#define PF_FSEXEC_DTA_EXEC      (0x80000000UL)

#if PF_BACKWARD_COMPATIBILITY_PARAMETER
#define FS_NORMAL               (0x00U)
#define FS_REMOVE               PF_FSEXEC_REMOVE
#define FS_CHMOD                (0x02U)
#define FS_CHDMOD               (0x04U)
#define FS_FLASH                (0x08U)
#endif /* PF_BACKWARD_COMPATIBILITY_PARAMETER */

/* fsexec specified types */
#define PF_FSEXEC_WRITE         PF_OPEN_MODE_WRITE
#define PF_FSEXEC_READ          PF_OPEN_MODE_READ
#define PF_FSEXEC_APPEND        PF_OPEN_MODE_APPEND
#define PF_FSEXEC_PLUS          PF_OPEN_MODE_PLUS
#define PF_FSEXEC_WRITE_ONLY    PF_OPEN_MODE_WRITE_ONLY
#define PF_FSEXEC_CONT_CLUSTER  PF_OPEN_MODE_CONT_CLUSTER

#if defined AMBA_FS_KAL
/* For MisraC checking. */
#else
/* create macro for PF_DTA_EXEC */
#define PF_DTACPY(a,b)          pf_memcpy((void*)b, (void*)a, sizeof(PF_DTA_EXEC));
#endif

/*---------------------------------------------------------------------------
  _init_prfile2/_setvolcfg/_getvolcfg option
 ----------------------------------------------------------------------------*/
typedef struct {
    PF_U_LONG   volume_config;      /* volume configurations  */
    PF_U_LONG   file_config;        /* file configurations    */
    PF_U_LONG   fat_buff_size;      /* FAT cache buffer size  */
    PF_U_LONG   data_buff_size;     /* data cache buffer size */
} PF_VOL_CFG;

#define PF_ALL_VOLUME                       ((PF_DRV_CHAR)0x7F)

/* for pf_vol_set.config */
#define PF_CHAR_CHECK_DISABLE               (0x00010000UL)  /* used for internal flag       */
#define PF_CHAR_CHECK_ENABLE                (0x00020000UL)  /* used for only a parameter    */
#if PF_EXFAT_SUPPORT
#define PF_UPDATE_VOLUME_DIRTY_DISABLE      (0x10000000UL)  /* Not update VolumeDirty bit   */
#define PF_UPDATE_VOLUME_DIRTY_ENABLE       (0x20000000UL)  /* update VolumeDirty bit       */
#define PF_CHECK_VOLUME_DIRTY_DISABLE       (0x40000000UL)  /* Not check VolumeDirty bit    */
#define PF_CHECK_VOLUME_DIRTY_ENABLE        (0x80000000UL)  /* Checke VolumeDirty bit       */
#endif /* PF_EXFAT_SUPPORT */

/* for PF_VOL_CFG.volume_config */
#define PF_SET_FAT_CACHE_SIZE               (0x00000001UL)
#define PF_SET_DATA_CACHE_SIZE              (0x00000002UL)

/* for p_vol->config */
#define PF_CONT_CLUSTER_ENABLE              (0x00040000UL)
#define PF_CONT_CLUSTER_DISABLE             (0x00080000UL)
#define PF_CONT_CLUSTER_DIS_NOT_BPS         (0x00100000UL | PF_CONT_CLUSTER_DISABLE)
#define PF_CONT_CLUSTER_DIS_NOT_DRIVER      (0x00200000UL | PF_CONT_CLUSTER_DISABLE)
#define PF_CONT_CLUSTER_DIS_NOT_BOUNDARY    (0x00400000UL | PF_CONT_CLUSTER_DISABLE)
#define PF_CONT_CLUSTER_DIS_NOT_DIVIDE      (0x00800000UL | PF_CONT_CLUSTER_DISABLE)
#define PF_CONT_CLUSTER_DIS_NOT_CLUSTER     (0x01000000UL)  /* used for internal flag */

/* for p_vol->file_config */
#define PF_UPDATE_ACCESS_DATE_DISABLE       (0x00000001UL)  /* used for internal flag       */
#define PF_UPDATE_ACCESS_DATE_ENABLE        (0x00000002UL)  /* used for only a parameter    */

#define PF_WRITE_TIMESTMP_AT_UPDATE         (0x00000004UL)  /* used for internal flag       */
#define PF_WRITE_TIMESTMP_AT_FCLOSE         (0x00000008UL)  /* use for only a parameter     */

#if PF_ACTIVE_UNICOM_ENABLE
#define PF_UNICOM_ENABLE                    (0x00000010UL)  /* used for internal flag       */
#define PF_UNICOM_DISABLE                   (0x00000020UL)  /* use for only a parameter     */
#define PF_NOSHORT_ENABLE                   PF_UNICOM_ENABLE    /* for backward compatibility */
#define PF_NOSHORT_DISABLE                  PF_UNICOM_DISABLE   /* for backward compatibility */
#endif /* PF_ACTIVE_UNICOM_ENABLE*/


/*---------------------------------------------------------------------------
  create option
 ----------------------------------------------------------------------------*/
#if PF_BACKWARD_COMPATIBILITY_CREATE
#define PF_BLOCK                PF_OPEN_MODE_CONT_CLUSTER

#if PF_STDLIB_ALIAS_SUPPORT
#define O_BLOCK                 PF_BLOCK
#else
#define FCP_O_BLOCK             PF_BLOCK
#endif /* PF_STDLIB_ALIAS_SUPPORT */
#endif /* PF_BACKWARD_COMPATIBILITY_CREATE */


/*---------------------------------------------------------------------------
  encoding mode
 ----------------------------------------------------------------------------*/
#define PF_ENCODE_MODE_UTF16            (0x01U)
#define PF_ENCODE_MODE_UTF16_EXT        (0x02U)
#define PF_ENCODE_MODE_UTF16_ONLY       (0x02U)
#define PF_ENCODE_MODE_ALL              (PF_ENCODE_MODE_UTF16 | PF_ENCODE_MODE_UTF16_EXT)


/*---------------------------------------------------------------------------
  Clean mode for pf_cleandir(), pf_chmoddir()
 ----------------------------------------------------------------------------*/
#define PF_SEARCH_BELOW                 (0x00010000UL)  /* Directory below only mode */
#define PF_SEARCH_TREE                  (0x00020000UL)  /* Tree all mode             */
#define PF_SEARCH_TEST                  (0x00100000UL)  /* Test mode                 */

#define PF_CLEANDIR_BELOW               PF_SEARCH_BELOW
#define PF_CLEANDIR_TREE                PF_SEARCH_TREE
#define PF_CLEANDIR_TEST                PF_SEARCH_TEST

#if PF_SEARCH_BELOW != PF_CLEANDIR_BELOW
#error "PF_SEARCH_BELOW != PF_CLEANDIR_BELOW"
#endif
#if PF_SEARCH_TREE != PF_CLEANDIR_TREE
#error "PF_SEARCH_TREE != PF_CLEANDIR_TREE"
#endif
#if PF_SEARCH_TEST != PF_CLEANDIR_TEST
#error "PF_SEARCH_TEST != PF_CLEANDIR_TEST"
#endif


/*---------------------------------------------------------------------------
  Command code definition macro for pf_fpctl, pf_dpctl, pf_drvctl
 ----------------------------------------------------------------------------*/
/* 'direction' field */
#define PF_CTL_DIR_NONE             (0x00UL)
#define PF_CTL_DIR_IN               (0x01UL)
#define PF_CTL_DIR_OUT              (0x02UL)
#define PF_CTL_DIR_INOUT            (PF_CTL_DIR_IN | PF_CTL_DIR_OUT)
#define PF_CTL_DIR_MASK             (0x3UL)
#define PF_CTL_DIR_SHIFT            (30)

/* 'command group' field */
#define PF_CTL_GRP_DRVCTL           (0x0001UL)
#define PF_CTL_GRP_FPCTL            (0x0002UL)
#define PF_CTL_GRP_DPCTL            (0x0003UL)
#define PF_CTL_GRP_MASK             (0x1fffUL)
#define PF_CTL_GRP_SHIFT            (16)

/* 'command' field */
#define PF_CTL_CMD_MASK             (0xffffUL)
#define PF_CTL_CMD_SHIFT            (0)

/* Definition macro */
typedef PF_U_LONG                   PF_CTL_CMDCODE;

#if defined AMBA_FS_KAL
#ifndef AMBA_FS_H
#define PF_CTL_GEN_CMDCODE(dir, grp, cmd)       \
        (PF_CTL_CMDCODE)(                                       \
            (((dir) & PF_CTL_DIR_MASK) << PF_CTL_DIR_SHIFT)     \
            | (((grp) & PF_CTL_GRP_MASK) << PF_CTL_GRP_SHIFT)   \
            | (((cmd) & PF_CTL_CMD_MASK) << PF_CTL_CMD_SHIFT)   \
        )


/*---------------------------------------------------------------------------
  Command code for pf_drvctl
 ----------------------------------------------------------------------------*/
/* Get the format type (same as PF_DEV_INF.fmt) */
#define PF_DRVCTL_GET_FORMATTYPE    PF_CTL_GEN_CMDCODE(PF_CTL_DIR_OUT, PF_CTL_GRP_DRVCTL, 0x0001U)
#endif
#endif

/*---------------------------------------------------------------------------
  The extension module
 ----------------------------------------------------------------------------*/
#if PF_MODULE_SUPPORT

/* The Module ID */
typedef PF_U_LONG   PF_MODULE_ID;

#define PF_MODULE_ID_NONE       (0x00000000UL)
#define PF_MODULE_ID_FCS        (0x00000002UL)  /* Free Cluster Search Module */
#define PF_MODULE_ID_SAFE       (0x00000003UL)  /* Safe Module */

#endif /* PF_MODULE_SUPPORT */


/*---------------------------------------------------------------------------
  Plug-in for LFS - statvfs_np()
 ----------------------------------------------------------------------------*/
typedef struct
{
   /* same as statvfs */

       /* cluster size (byte) */
       PF_U_LONG  f_bsize;

       /* cluster size (byte) */
       PF_U_LONG  f_frsize;

       /* total number of clusters */
       PF_U_LONG  f_blocks;

       /* total number of free clusters */
       PF_U_LONG  f_bfree;

       /* total number of free clusters */
       PF_U_LONG  f_bavail;

       /* unsupported (always 0)*/
       PF_U_LONG  f_files;
       PF_U_LONG  f_ffree;
       PF_U_LONG  f_favail;
       PF_U_LONG  f_fsid;

       /* mount flag */
       PF_U_LONG  f_flag;

       /* max length of file name */
       PF_U_LONG  f_namemax;

   /* original */

       /* sector size (byte) */
       PF_U_LONG  f_ssize;

       /* cluster group size(byte) */
       PF_U_LONG  f_gsize;

       /* total number of free cluster groups */
       PF_U_LONG  f_gfree;
} PF_STATVFS;

/* flags for 'f_flag' */
#define PF_ST_READONLY     (1)    /* Readonly file system */
#define PF_ST_REMOVABLE    (2)    /* Removable file system */
#define PF_ST_NOSUID       (128)  /* Does not support setuid()/ setgid() semantics */

/*---------------------------------------------------------------------------
  API
 ----------------------------------------------------------------------------*/
#define PF_APICODE_MIN          (1)
#define PF_APICODE_W_MIN        (128)
#define PF_APICODE_E_MIN        (256)
#define PF_APICODE_M_MIN        (512)
#define PF_APICODE_M_FAT_MIN    (512)

typedef enum
{
    PF_APICODE_NONE = 0,
    /* pf_xxx() */
    PF_APICODE_CREATE = PF_APICODE_MIN,              /* 0x0001 (  1), pf_create                 */
    PF_APICODE_FOPEN,                                /* 0x0002 (  2), pf_fopen                  */
    PF_APICODE_FCLOSE,                               /* 0x0004 (  3), pf_fclose                 */
    PF_APICODE_FREAD,                                /* 0x0004 (  4), pf_fread                  */
    PF_APICODE_FWRITE,                               /* 0x0005 (  5), pf_fwrite                 */
    PF_APICODE_FSEEK,                                /* 0x0006 (  6), pf_fseek                  */
    PF_APICODE_FSFIRST,                              /* 0x0007 (  7), pf_fsfirst                */
    PF_APICODE_FSNEXT,                               /* 0x0008 (  8), pf_fsnext                 */
    PF_APICODE_REMOVE,                               /* 0x0009 (  9), pf_remove                 */
    PF_APICODE_RENAME,                               /* 0x000a ( 10), pf_rename                 */
    PF_APICODE_MKDIR,                                /* 0x000b ( 11), pf_mkdir                  */
    PF_APICODE_RMDIR,                                /* 0x000c ( 12), pf_rmdir                  */
    PF_APICODE_CHDIR,                                /* 0x000d ( 13), pf_chdir                  */
    PF_APICODE_FSTAT,                                /* 0x000e ( 14), pf_fstat                  */
    PF_APICODE_CHMOD,                                /* 0x000f ( 15), pf_chmod                  */
    PF_APICODE_CHDMOD,                               /* 0x0010 ( 16), pf_chdmod                 */
    PF_APICODE_MOUNT,                                /* 0x0011 ( 17), pf_mount                  */
    PF_APICODE_FORMAT,                               /* 0x0012 ( 18), pf_format                 */
    PF_APICODE_BUFFERING,                            /* 0x0013 ( 19), pf_buffering              */
    PF_APICODE_FERROR,                               /* 0x0014 ( 20), pf_ferror                 */
    PF_APICODE_FEOF,                                 /* 0x0015 ( 21), pf_feof                   */
    PF_APICODE_ERRNUM,                               /* 0x0016 ( 22), pf_errnum                 */
    PF_APICODE_GETDEV,                               /* 0x0017 ( 23), pf_getdev                 */
    PF_APICODE_SETVOL,                               /* 0x0018 ( 24), pf_setvol                 */
    PF_APICODE_GETVOL,                               /* 0x0019 ( 25), pf_getvol                 */
    PF_APICODE_RMVVOL,                               /* 0x001a ( 26), pf_rmvvol                 */
    PF_APICODE_ATTACH,                               /* 0x001b ( 27), pf_attach                 */
    PF_APICODE_INIT_PRFILE2,                         /* 0x001c ( 28), pf_init_prfile2           */
    PF_APICODE_SETUPFSI,                             /* 0x001d ( 29), pf_setupfsi               */
    PF_APICODE_SETCLSTLINK,                          /* 0x001e ( 30), pf_setclstlink            */
    PF_APICODE_FSETCLSTLINK,                         /* 0x001f ( 31), pf_fsetclstlink           */
    PF_APICODE_SYNC,                                 /* 0x0020 ( 32), pf_sync                   */
    PF_APICODE_FSYNC,                                /* 0x0021 ( 33), pf_fsync                  */
    PF_APICODE_FAPPEND,                              /* 0x0022 ( 34), pf_fappend                */
    PF_APICODE_SETTAILBUF,                           /* 0x0023 ( 35), pf_settailbuf             */
    PF_APICODE_DERRNUM,                              /* 0x0024 ( 36), pf_derrnum                */
    PF_APICODE_FSEXEC,                               /* 0x0025 ( 37), pf_fsexec                 */
    PF_APICODE_UNMOUNT,                              /* 0x0026 ( 38), pf_unmount                */
    PF_APICODE_COMBINE,                              /* 0x0027 ( 39), pf_combine                */
    PF_APICODE_FCONCAT,                              /* 0x0028 ( 40), pf_fconcat                */
    PF_APICODE_DETACH,                               /* 0x0029 ( 41), pf_detach                 */
    PF_APICODE_DIVIDE,                               /* 0x002a ( 42), pf_divide                 */
    PF_APICODE_FADJUST,                              /* 0x002b ( 43), pf_fadjust                */
    PF_APICODE_FINFO,                                /* 0x002c ( 44), pf_finfo                  */
    PF_APICODE_MOVE,                                 /* 0x002d ( 45), pf_move                   */
    PF_APICODE_CINSERT,                              /* 0x002e ( 46), pf_cinsert                */
    PF_APICODE_INSERT,                               /* 0x002f ( 47), pf_insert                 */
    PF_APICODE_CDELETE,                              /* 0x0030 ( 48), pf_cdelete                */
    PF_APICODE_CUT,                                  /* 0x0031 ( 49), pf_cut                    */
    PF_APICODE_XDIVIDE,                              /* 0x0032 ( 50), pf_xdivide                */
    PF_APICODE_GETVOLCFG,                            /* 0x0033 ( 51), pf_getvolcfg              */
    PF_APICODE_SETVOLCFG,                            /* 0x0034 ( 52), pf_setvolcfg              */
    PF_APICODE_CLOSEDIR,                             /* 0x0035 ( 53), pf_closedir               */
    PF_APICODE_FCHDIR,                               /* 0x0036 ( 54), pf_fchdir                 */
    PF_APICODE_FLOCK,                                /* 0x0037 ( 55), pf_flock                  */
    PF_APICODE_OPENDIR,                              /* 0x0038 ( 56), pf_opendir                */
    PF_APICODE_READDIR,                              /* 0x0039 ( 57), pf_readdir                */
    PF_APICODE_REGCTX,                               /* 0x003a ( 58), pf_regctx                 */
    PF_APICODE_REWINDDIR,                            /* 0x003b ( 59), pf_rewinddir              */
    PF_APICODE_SEEKDIR,                              /* 0x003c ( 60), pf_seekdir                */
    PF_APICODE_SETCODE,                              /* 0x003d ( 61), pf_setcode                */
    PF_APICODE_TELLDIR,                              /* 0x003e ( 62), pf_telldir                */
    PF_APICODE_UNREGCTX,                             /* 0x003f ( 63), pf_unregctx               */
    PF_APICODE_SETENCODE,                            /* 0x0040 ( 64), pf_setencode              */
    PF_APICODE_CREATEDIR,                            /* 0x0041 ( 65), pf_createdir              */
    PF_APICODE_CLEANDIR,                             /* 0x0042 ( 66), pf_cleandir               */
    PF_APICODE_DELETEDIR,                            /* 0x0043 ( 67), pf_deletedir              */
    PF_APICODE_TRUNCATE,                             /* 0x0044 ( 68), pf_truncate               */
    PF_APICODE_FTRUNCATE,                            /* 0x0045 ( 69), pf_ftruncate              */
    PF_APICODE_GETSTAMP,                             /* 0x0046 ( 70), pf_getstamp               */
    PF_APICODE_SETSTAMP,                             /* 0x0047 ( 71), pf_setstamp               */
    PF_APICODE_GETSTMP,                              /* 0x0048 ( 72), pf_getstmp                */
    PF_APICODE_SETSTMP,                              /* 0x0049 ( 73), pf_setstmp                */
    PF_APICODE_FGETPOS,                              /* 0x004a ( 74), pf_fgetpos                */
    PF_APICODE_FSETPOS,                              /* 0x004b ( 75), pf_fsetpos                */
    PF_APICODE_CHMODDIR,                             /* 0x004c ( 76), pf_chmoddir               */
    PF_APICODE_REGMOD,                               /* 0x004d ( 77), pf_regmod                 */
    PF_APICODE_CTRLMOD,                              /* 0x004e ( 78), pf_ctrlmod                */
    PF_APICODE_MODERRNUM,                            /* 0x004f ( 79), pf_moderrnum              */
    PF_APICODE_CTRLMODNB,                            /* 0x0050 ( 80), pf_ctrlmodnb              */
    PF_APICODE_GETEMPENT,                            /* 0x0051 ( 81), pf_getempent              */
    PF_APICODE_FGETDTA,                              /* 0x0052 ( 82), pf_fgetdta                */
    PF_APICODE_INERRNUM,                             /* 0x0053 ( 83), pf_inerrnum               */
    PF_APICODE_DLOCK,                                /* 0x0054 ( 84), pf_dlock                  */
    PF_APICODE_FPSTAT,                               /* 0x0055 ( 85), pf_fpstat                 */
    PF_APICODE_SETUPCASEBUF,                         /* 0x0056 ( 86), pf_setupcasebuf           */
    PF_APICODE_FDIVIDE,                              /* 0x0057 ( 87), pf_fdivide                */
    PF_APICODE_DGETEMPENT,                           /* 0x0058 ( 88), pf_dgetempent             */
    PF_APICODE_SETINERRNUM,                          /* 0x0059 ( 89), pf_setinerrnum            */
    PF_APICODE_SETFCSPOS,                            /* 0x005a ( 90), pf_setfcspos              */
    PF_APICODE_FSEXEC_REMOVE,                        /* 0x005b ( 91), pf_fsexec(REMOVE)         */
    PF_APICODE_FSEXEC_CHGMOD,                        /* 0x005c ( 92), pf_fsexec(CHGMOD)         */
    PF_APICODE_FSEXEC_FOPEN,                         /* 0x005d ( 93), pf_fsexec(FOPEN)          */
    PF_APICODE_FSEXEC_OPENDIR,                       /* 0x005e ( 94), pf_fsexec(OPENDIR)        */
    PF_APICODE_FSEXEC_DELETEDIR,                     /* 0x005f ( 95), pf_fsexec(DELETEDIR)      */
    PF_APICODE_AUTOMOUNT,                            /* 0x0060 ( 96), auto-mount                */
    PF_APICODE_ICONV_INSTALL,                        /* 0x0061 ( 97), pf_iconv_install          */
    PF_APICODE_ICONV_UNINSTALL,                      /* 0x0062 ( 98), pf_iconv_uninstall        */
    PF_APICODE_ICONV_OPEN,                           /* 0x0063 ( 99), pf_iconv_open             */
    PF_APICODE_ICONV_CLOSE,                          /* 0x0064 (100), pf_iconv_close            */
    PF_APICODE_ICONV,                                /* 0x0065 (101), pf_iconv                  */
    PF_APICODE_DRVCTL,                               /* 0x0066 (102), pf_drvctl                 */
    PF_APICODE_DP_FSTAT,                             /* 0x0067 (103), pf_dp_fstat               */
    PF_APICODE_DP_TELLDIR,                           /* 0x0068 (104), pf_dp_telldir             */
    PF_APICODE_MAX,
    /* pf_w_xxx() */
    PF_APICODE_W_CREATE = PF_APICODE_W_MIN,          /* 0x0080 (128), pf_w_create               */
    PF_APICODE_W_FOPEN,                              /* 0x0081 (129), pf_w_fopen                */
    PF_APICODE_W_REMOVE,                             /* 0x0082 (130), pf_w_remove               */
    PF_APICODE_W_RENAME,                             /* 0x0083 (131), pf_w_rename               */
    PF_APICODE_W_MKDIR,                              /* 0x0084 (132), pf_w_mkdir                */
    PF_APICODE_W_RMDIR,                              /* 0x0085 (133), pf_w_rmdir                */
    PF_APICODE_W_CHDIR,                              /* 0x0086 (134), pf_w_chdir                */
    PF_APICODE_W_FSTAT,                              /* 0x0087 (135), pf_w_fstat                */
    PF_APICODE_W_CHMOD,                              /* 0x0088 (136), pf_w_chmod                */
    PF_APICODE_W_CHDMOD,                             /* 0x0089 (137), pf_w_chdmod               */
    PF_APICODE_W_FCONCAT,                            /* 0x008a (138), pf_w_fconcat              */
    PF_APICODE_W_FSFIRST,                            /* 0x008b (139), pf_w_fsfirst              */
    PF_APICODE_W_FSNEXT,                             /* 0x008c (140), pf_w_fsnext               */
    PF_APICODE_W_FSEXEC,                             /* 0x008d (141), pf_w_fsexec               */
    PF_APICODE_W_COMBINE,                            /* 0x008e (142), pf_w_combine              */
    PF_APICODE_W_DIVIDE,                             /* 0x008f (143), pf_w_divide               */
    PF_APICODE_W_XDIVIDE,                            /* 0x0090 (144), pf_w_xdivide              */
    PF_APICODE_W_MOVE,                               /* 0x0091 (145), pf_w_move                 */
    PF_APICODE_W_CINSERT,                            /* 0x0092 (146), pf_w_cinsert              */
    PF_APICODE_W_INSERT,                             /* 0x0093 (147), pf_w_insert               */
    PF_APICODE_W_CDELETE,                            /* 0x0094 (148), pf_w_cdelete              */
    PF_APICODE_W_CUT,                                /* 0x0095 (149), pf_w_cut                  */
    PF_APICODE_W_OPENDIR,                            /* 0x0096 (150), pf_w_opendir              */
    PF_APICODE_W_CREATEDIR,                          /* 0x0097 (151), pf_w_createdir            */
    PF_APICODE_W_CLEANDIR,                           /* 0x0098 (152), pf_w_cleandir             */
    PF_APICODE_W_DELETEDIR,                          /* 0x0099 (153), pf_w_deletedir            */
    PF_APICODE_W_TRUNCATE,                           /* 0x009a (154), pf_w_truncate             */
    PF_APICODE_W_GETSTAMP,                           /* 0x009b (155), pf_w_getstamp             */
    PF_APICODE_W_SETSTAMP,                           /* 0x009c (156), pf_w_setstamp             */
    PF_APICODE_W_CHMODDIR,                           /* 0x009d (157), pf_w_chmoddir             */
    PF_APICODE_W_FGETDTA,                            /* 0x009e (158), pf_w_fgetdta              */
    PF_APICODE_W_FDIVIDE,                            /* 0x009f (159), pf_w_fdivide              */
    PF_APICODE_W_GETVOL,                             /* 0x00a0 (160), pf_w_getvol               */
    PF_APICODE_W_SETVOL,                             /* 0x00a1 (161), pf_w_setvol               */
    PF_APICODE_W_FSEXEC_REMOVE,                      /* 0x00a2 (162), pf_w_fsexec(REMOVE)       */
    PF_APICODE_W_FSEXEC_CHGMOD,                      /* 0x00a3 (163), pf_w_fsexec(CHGMOD)       */
    PF_APICODE_W_FSEXEC_FOPEN,                       /* 0x00a4 (164), pf_w_fsexec(FOPEN)        */
    PF_APICODE_W_FSEXEC_OPENDIR,                     /* 0x00a5 (165), pf_w_fsexec(OPENDIR)      */
    PF_APICODE_W_FSEXEC_DELETEDIR,                   /* 0x00a6 (166), pf_w_fsexec(DELETEDIR)    */
    PF_APICODE_W_READDIR,                            /* 0x00a7 (167), pf_w_readdir              */
    PF_APICODE_W_DLOCK_ISLOCKED,                     /* 0x00a8 (168), pf_w_dlock_islocked       */
    PF_APICODE_W_DP_FSTAT,                           /* 0x00a9 (169), pf_w_dp_fstat             */
    PF_APICODE_W_MAX,
    /* pf_e_xxx() */
    PF_APICODE_E_CREATE = PF_APICODE_E_MIN,          /* 0x0100 (256), pf_e_create               */
    PF_APICODE_E_FCLOSE,                             /* 0x0101 (257), pf_e_fclose               */
    PF_APICODE_E_FREAD,                              /* 0x0102 (258), pf_e_fread                */
    PF_APICODE_E_FWRITE,                             /* 0x0103 (259), pf_e_fwrite               */
    PF_APICODE_E_FSEEK,                              /* 0x0104 (260), pf_e_fseek                */
    PF_APICODE_E_FSFIRST,                            /* 0x0105 (261), pf_e_fsfirst              */
    PF_APICODE_E_FSEXEC,                             /* 0x0106 (262), pf_e_fsexec               */
    PF_APICODE_E_REMOVE,                             /* 0x0107 (263), pf_e_remove               */
    PF_APICODE_E_RENAME,                             /* 0x0108 (264), pf_e_rename               */
    PF_APICODE_E_MKDIR,                              /* 0x0109 (265), pf_e_mkdir                */
    PF_APICODE_E_RMDIR,                              /* 0x010a (266), pf_e_rmdir                */
    PF_APICODE_E_FSTAT,                              /* 0x010b (267), pf_e_fstat                */
    PF_APICODE_E_FEOF,                               /* 0x010c (268), pf_e_feof                 */
    PF_APICODE_E_FSYNC,                              /* 0x010d (269), pf_e_fsync                */
    PF_APICODE_E_OPENDIR,                            /* 0x010e (270), pf_e_opendir              */
    PF_APICODE_E_CLOSEDIR,                           /* 0x010f (271), pf_e_closedir             */
    PF_APICODE_E_READDIR,                            /* 0x0110 (272), pf_e_readdir              */
    PF_APICODE_E_SEEKDIR,                            /* 0x0111 (273), pf_e_seekdir              */
    PF_APICODE_E_FTRUNCATE,                          /* 0x0112 (274), pf_e_ftruncate            */
    PF_APICODE_E_GETSTAMP,                           /* 0x0113 (275), pf_e_getstamp             */
    PF_APICODE_E_SETSTAMP,                           /* 0x0114 (276), pf_e_setstamp             */
    PF_APICODE_E_FOPEN,                              /* 0x0115 (277), pf_e_fopen                */
    PF_APICODE_E_CINSERT,                            /* 0x0118 (278), pf_e_cinsert              */
    PF_APICODE_E_CDELETE,                            /* 0x0119 (279), pf_e_cdelete              */
    PF_APICODE_E_COMBINE,                            /* 0x011a (280), pf_e_combine              */
    PF_APICODE_E_DIVIDE,                             /* 0x011b (281), pf_e_divide               */
    PF_APICODE_E_CLEANDIR,                           /* 0x011c (282), pf_e_cleandir             */
    PF_APICODE_E_DELETEDIR,                          /* 0x011d (283), pf_e_deletedir            */
    PF_APICODE_E_CHMODDIR,                           /* 0x011e (284), pf_e_chmoddir             */
    PF_APICODE_E_GETDEV,                             /* 0x011f (285), pf_e_getdev               */
    PF_APICODE_E_SETVOL,                             /* 0x0120 (286), pf_e_setvol               */
    PF_APICODE_E_GETVOL,                             /* 0x0121 (287), pf_e_getvol               */
    PF_APICODE_E_RMVVOL,                             /* 0x0122 (288), pf_e_rmvvol               */
    PF_APICODE_E_BUFFERING,                          /* 0x0123 (289), pf_e_buffering            */
    PF_APICODE_E_SETUPFSI,                           /* 0x0124 (290), pf_e_setupfsi             */
    PF_APICODE_E_SETTAILBUF,                         /* 0x0125 (291), pf_e_settailbuf           */
    PF_APICODE_E_SETVOLCFG,                          /* 0x0126 (292), pf_e_setvolcfg            */
    PF_APICODE_E_GETVOLCFG,                          /* 0x0127 (293), pf_e_getvolcfg            */
    PF_APICODE_E_SETENCODE,                          /* 0x0128 (294), pf_e_setencode            */
    PF_APICODE_E_MOUNT,                              /* 0x0129 (295), pf_e_mount                */
    PF_APICODE_E_UNMOUNT,                            /* 0x012a (296), pf_e_unmount              */
    PF_APICODE_E_FORMAT,                             /* 0x012b (297), pf_e_format               */
    PF_APICODE_E_DLOCK,                              /* 0x012c (298), pf_e_dlock                */
    PF_APICODE_E_FSEXEC_REMOVE,                      /* 0x012d (299), pf_e_fsexec(REMOVE)       */
    PF_APICODE_E_FSEXEC_CHGMOD,                      /* 0x012e (300), pf_e_fsexec(CHGMOD)       */
    PF_APICODE_E_FSEXEC_FOPEN,                       /* 0x012f (301), pf_e_fsexec(FOPEN)        */
    PF_APICODE_E_FSEXEC_OPENDIR,                     /* 0x0130 (302), pf_e_fsexec(OPENDIR)      */
    PF_APICODE_E_FSEXEC_DELETEDIR,                   /* 0x0131 (303), pf_e_fsexec(DELETEDIR)    */
    PF_APICODE_E_MAX,
    /* pfm_xxx() */
    PF_APICODE_M_FAT_GETBPB = PF_APICODE_M_FAT_MIN,  /* 0x0200 (512), pfm_fat_getBPB            */
    PF_APICODE_M_FAT_GETCLST,                        /* 0x0201 (513), pfm_fat_getClst           */
    PF_APICODE_M_FAT_SRCHCLST,                       /* 0x0202 (514), pfm_fat_srchClst          */
    PF_APICODE_M_FAT_ERRNUM,                         /* 0x0203 (515), pfm_fat_errnum            */
    PF_APICODE_M_FAT_GETDRVNAME,                     /* 0x0204 (516), pfm_fat_getDrvname        */
    PF_APICODE_M_FAT_GETERRNUM,                      /* 0x0205 (517), pfm_fat_getErrnum         */
    PF_APICODE_M_FAT_SETERRNUM,                      /* 0x0206 (518), pfm_fat_setErrnum         */
    PF_APICODE_M_FAT_GETDRVCONF,                     /* 0x0207 (519), pfm_fat_getDrvConf        */
    PF_APICODE_M_FAT_SETDRVCONF,                     /* 0x0208 (520), pfm_fat_setDrvConf        */
    PF_APICODE_M_FAT_READSECT,                       /* 0x0209 (521), pfm_fat_readSect          */
    PF_APICODE_M_FAT_WRITESECT,                      /* 0x020a (522), pfm_fat_writeSect         */
    PF_APICODE_M_FAT_PFAPI,                          /* 0x020b (523), pfm_fat_pfapi             */
    PF_APICODE_M_FAT_PFAPI_FOPEN,                    /* 0x020c (524), pfm_fat_pfapi(FOPEN)      */
    PF_APICODE_M_FAT_PFAPI_FCLOSE,                   /* 0x020d (525), pfm_fat_pfapi(FCLOSE)     */
    PF_APICODE_M_FAT_PFAPI_FAPPEND,                  /* 0x020e (526), pfm_fat_pfapi(FAPPEND)    */
    PF_APICODE_M_FAT_PFAPI_FINFO,                    /* 0x020f (527), pfm_fat_pfapi(FINF)       */
    PF_APICODE_M_FAT_PFAPI_FTRUNCATE,                /* 0x0210 (528), pfm_fat_pfapi(FTRUNCATE)  */
    PF_APICODE_M_FAT_PFAPI_SETFCSPOS,                /* 0x0211 (529), pfm_fat_pfapi(SETFCSPOS)  */
    PF_APICODE_M_FAT_PFAPI_CHMOD,                    /* 0x0212 (530), pfm_fat_pfapi(CHMOD)      */
    PF_APICODE_M_FAT_MAX
} PF_API_CODE;

/* (PF_API.api_common) */
typedef struct
{
    PF_API_CODE cmn_code;           /* API code             */
    PF_ERROR    cmn_inerrnum;       /* [O] internal errnum  */
} PF_API_COMMON;

/* pf_fopen() (PF_API.api_fopen) */
typedef struct
{
    PF_API_CODE fop_code;          /* API code             */
    PF_ERROR    fop_inerrnum;      /* [O] internal errnum  */
    PF_FILE     *fop_fp;           /* [O] return value     */
    PF_CHAR     *fop_path;         /* [I] parameter1       */
    PF_CHAR     *fop_mode;         /* [I] parameter2       */
} PF_API_FOPEN;

/* pf_fclose() (PF_API.api_fclose) */
typedef struct
{
    PF_API_CODE fcl_code;           /* API code             */
    PF_ERROR    fcl_inerrnum;       /* [O] internal errnum  */
    PF_FILE     *fcl_fp;            /* [I] parameter1       */
} PF_API_FCLOSE;

/* pf_remove() (PF_API.api_remove) */
typedef struct
{
    PF_API_CODE rm_code;            /* API code             */
    PF_ERROR    rm_inerrnum;        /* [O] internal errnum  */
    PF_CHAR     *rm_path;           /* [I] parameter1       */
} PF_API_REMOVE;

/* pf_chmod() (PF_API.api_chmod) */
typedef struct
{
    PF_API_CODE chm_code;
    PF_ERROR    chm_inerrnum;
    PF_CHAR     *chm_path;
    PF_U_LONG   chm_attr;
} PF_API_CHMOD;

/* pf_fappend() (PF_API.api_fappend) */
typedef struct
{
    PF_API_CODE fap_code;           /* API code             */
    PF_ERROR    fap_inerrnum;       /* [O] internal errnum  */
    PF_FSIZE_T  fap_append_byte;    /* [O] retnrn value     */
    PF_FILE     *fap_fp;            /* [I] parameter1       */
    PF_FSIZE_T  fap_req_byte;       /* [I] parameter2       */
} PF_API_FAPPEND;

/* pf_finfo() (PF_API.api_finfo) */
typedef struct
{
    PF_API_CODE fif_code;           /* API code             */
    PF_ERROR    fif_inerrnum;       /* [O] internal errnum  */
    PF_FILE     *fif_fp;            /* [I] parameter1       */
    PF_INFO     *fif_info;          /* [I] parameter2       */
} PF_API_FINFO;

/* pf_ftruncate() (PF_API.api_ftruncate) */
typedef struct
{
    PF_API_CODE ftr_code;           /* API code             */
    PF_ERROR    ftr_inerrnum;       /* [O] internal errnum  */
    PF_FILE     *ftr_fp;            /* [I] parameter1       */
    PF_FPOS_T   ftr_offset;         /* [I] parameter2       */
} PF_API_FTRUNCATE;

/* pf_setfcspos() (PF_API.api_setfcspos) */
typedef struct
{
    PF_API_CODE sfp_code;           /* API code             */
    PF_ERROR    sfp_inerrnum;       /* [O] internal errnum  */
    PF_CLST     sfp_clst;           /* [I] parameter1       */
} PF_API_SETFCSPOS;

/* pf_setvolcfg() (PF_API.api_setvolcfg) */
typedef struct
{
    PF_API_CODE svc_code;           /* API code             */
    PF_ERROR    svc_inerrnum;       /* [O] internal errnum  */
    PF_VOL_CFG  *svc_config;        /* [I] parameter1       */
} PF_API_SETVOLCFG;

/* pf_drvctl() (PF_API.api_drvctl) */
typedef struct
{
    PF_API_CODE dcl_code;           /* API code             */
    PF_ERROR    dcl_inerrnum;       /* [O] internal errnum  */
    PF_DRV_CHAR dcl_drive;          /* [I] parameter1       */
    PF_U_LONG   dcl_cmdcode;        /* [I] parameter2       */
    void        *dcl_buf;           /* [I] parameter3       */
    PF_SIZE_T   dcl_bufbytes;       /* [I] parameter4       */
} PF_API_DRVCTL;

typedef union
{
    PF_API_CODE         api_code;
    PF_API_COMMON       api_common;
    PF_API_FOPEN        api_fopen;      /* pf_fopen()       */
    PF_API_FCLOSE       api_fclose;     /* pf_fclose()      */
    PF_API_REMOVE       api_remove;     /* pf_remove()      */
    PF_API_CHMOD        api_chmod;      /* pf_chmod()       */
    PF_API_FAPPEND      api_fappend;    /* pf_fappend()     */
    PF_API_FINFO        api_finfo;      /* pf_finfo()       */
    PF_API_FTRUNCATE    api_ftruncate;  /* pf_ftruncate()   */
    PF_API_SETVOLCFG    api_setvolcfg;  /* pf_setvolcfg()   */
    PF_API_SETFCSPOS    api_setfcspos;  /* pf_setfcspos()   */
    PF_API_DRVCTL       api_drvctl;     /* pf_drvctl()      */
} PF_API;

/*---------------------------------------------------------------------------
  Error numbers for API
 ----------------------------------------------------------------------------*/
#define PF_ERR_EPERM            (1)         /* Operation is not possible    */
#define PF_ERR_ENOENT           (2)         /* No such file or directory    */
#define PF_ERR_ESRCH            (3)         /* No such process              */
#define PF_ERR_EIO              (5)         /* I/O Error(Driver Error)      */
#define PF_ERR_E2BIG            (7)         /* Argument list too long       */
#define PF_ERR_ENOEXEC          (8)         /* Not Executable by internal   */
#define PF_ERR_EBADF            (9)         /* Bad file descriptor          */
#define PF_ERR_ENOMEM           (12)        /* Not enough system memory     */
#define PF_ERR_EACCES           (13)        /* Permission denied            */
#define PF_ERR_EBUSY            (16)        /* Can not use system resouces  */
#define PF_ERR_EEXIST           (17)        /* File already exists          */
#define PF_ERR_ENODEV           (19)        /* No such device.              */
#define PF_ERR_EISDIR           (21)        /* find directory when file req */
#define PF_ERR_EINVAL           (22)        /* Invalid argument             */
#define PF_ERR_ENFILE           (23)        /* Too many open files(system)  */
#define PF_ERR_EMFILE           (24)        /* Too many open files(user)    */
#define PF_ERR_EFBIG            (27)        /* Over file size limit(4GB-1)  */
#define PF_ERR_ENOSPC           (28)        /* Device out of space          */
#define PF_ERR_ENOLCK           (46)        /* Can not lock the file        */
#define PF_ERR_EILSEQ           (85)        /* Illegal byte sequence        */
#define PF_ERR_ENOSYS           (88)        /* Not implement function       */
#define PF_ERR_ENOTEMPTY        (90)        /* Directory is not empty       */
#define PF_ERR_EMOD_NOTSPRT     (100)       /* Module is not supported      */
#define PF_ERR_EMOD_NOTREG      (101)       /* Module is not registered     */
#define PF_ERR_EMOD_FCS         (102)       /* FreeClusterSearch Module Err */
#define PF_ERR_EMOD_SAFE        (103)       /* Safe Module Err              */
#define PF_ERR_ENOMEDIUM        (123)       /* No medium found              */
#define PF_ERR_DFNC             (0x1000)    /* Driver Error                 */
#define PF_ERR_SYSTEM           (-1)        /* system error(general error)  */

#if PF_BACKWARD_COMPATIBILITY_ERRORCODE

#define FCP_ENOENT           PF_ERR_ENOENT  /* No such file or directory    */
#define FCP_EBADF            PF_ERR_EBADF   /* Bad file descriptor          */
#define FCP_EACCES           PF_ERR_EACCES  /* Permission denied            */
#define FCP_EEXIST           PF_ERR_EEXIST  /* File already exists          */
#define FCP_EINVAL           PF_ERR_EINVAL  /* Invalid argument             */
#define FCP_EMFILE           PF_ERR_EMFILE  /* Too many open files          */
#define FCP_ENOSPC           PF_ERR_ENOSPC  /* Device out of space          */
#define DFNC_ERR             PF_ERR_DFNC

#endif /* PF_BACKWARD_COMPATIBILITY_ERRORCODE */


/*---------------------------------------------------------------------------
  Internal Error numbers for API
 ----------------------------------------------------------------------------*/
#define PF_IER_NO_ERR                      (0)     /* No error; Done successfully     */
#define PF_IER_INVALID_FILENAME            (1)     /* Invalid file name               */
#define PF_IER_INVALID_PATHNAME            (2)     /* Invalid path name               */
#define PF_IER_FILE_NOT_FOUND              (3)     /* File not found                  */
#define PF_IER_TOO_MANY_VOLUMES_ATTACHED   (4)     /* Too many volumes attached       */
#define PF_IER_DIRECTORY_FULL              (5)     /* Directory full                  */
#define PF_IER_VOLUME_FULL                 (6)     /* Volume full                     */
#define PF_IER_INVALID_DISK_FORMAT         (7)     /* Invalid disk format             */
#define PF_IER_FILE_ALREADY_EXISTS         (8)     /* File already exists             */
#define PF_IER_VOLUME_NOT_MOUNTED          (9)     /* Volume not mounted              */
#define PF_IER_INVALID_PARAMETER           (10)    /* Invalid parameter               */
#define PF_IER_WRITE_PROTECTED             (11)    /* Write protected                 */
#define PF_IER_UNSUPPORTED_FORMAT          (12)    /* Unsupported disk format         */
#define PF_IER_BROKEN_CLUSTER_CHAIN        (13)    /* Broken cluster chain            */
#define PF_IER_INVALID_CLUSTER_NUM         (14)    /* Invalid cluster number          */
#define PF_IER_INVALID_BPB                 (15)    /* Invalid BPB in Boot Sector      */
#define PF_IER_ACCESS_OUT_OF_VOLUME        (16)    /* Access out of volume            */
#define PF_IER_DRIVER_ERROR                (17)    /* Driver error                    */
#define PF_IER_INVALID_VOLUME_LABEL        (18)    /* Invalid volume label            */
#define PF_IER_FILE_OPENED                 (19)    /* File is opened                  */
#define PF_IER_NOT_A_DIRECTORY             (20)    /* Not a directory                 */
#define PF_IER_TOO_MANY_FILES_OPENED_S     (21)    /* Too many files opened (SFD)     */
#define PF_IER_TOO_MANY_FILES_OPENED_U     (22)    /* Too many files opened (UFD)     */
#define PF_IER_NOT_A_FILE                  (23)    /* Not a (regular) file            */
#define PF_IER_READ_ONLY                   (24)    /* Read only                       */
#define PF_IER_LOCK_ERROR                  (25)    /* Lock error                      */
#define PF_IER_INTERNAL_ERROR              (26)    /* Internal error                  */
#define PF_IER_END_OF_FILE                 (27)    /* End of file                     */
#define PF_IER_ACCESS_NOT_ALLOWED          (28)    /* Access not allowed              */
#define PF_IER_DIRECTORY_NOT_EMPTY         (29)    /* Directory not empty             */
#define PF_IER_NOT_ENOUGH_CACHE_PAGES      (30)    /* Not enough cache pages          */
#define PF_IER_DIFFERENT_DRIVE             (31)    /* Drive is different              */
#define PF_IER_DIFFERENT_ENTRY             (32)    /* Entry is different              */
#define PF_IER_INVALID_ENTRY               (33)    /* Invalid directry entry          */
#define PF_IER_INVALID_SECTOR              (34)    /* Invalid sector number           */
#define PF_IER_BROKEN_VOLUME               (35)    /* Broken volume structure         */
#define PF_IER_NOT_EFFECTIVE               (36)    /* Processing is not effective     */
#define PF_IER_FILE_SIZE_OVER              (37)    /* Over file size limit(4GB-1)     */
#define PF_IER_INVALID_FILE_DISCRIPTOR     (38)    /* Invalid file discriptor         */
#define PF_IER_INVALID_LOCK_FILE           (39)    /* File is already locked          */
#define PF_IER_MODULE_NOT_SUPPORTED        (40)    /* The module isnot supported      */
#define PF_IER_MODULE_NOT_REGISTERED       (41)    /* The module isnot registered     */
#define PF_IER_MODULE_FCS_ERROR            (42)    /* FCS module error                */
#define PF_IER_VOLUME_DIRTY                (43)    /* Volume dirty                    */
#define PF_IER_MEDIA_FAILURE               (44)    /* Media failure                   */
#define PF_IER_INVALID_CHECKSUM            (45)    /* Invalid checksum                */
#define PF_IER_NO_MEDIUM                   (46)    /* No medium found                 */
#define PF_IER_CONTEXT_NOT_REGISTERED      (47)    /* The context isn't registered    */
#define PF_IER_MALLOC                      (48)    /* Not enough memory               */
#define PF_IER_DIR_LOCKED                  (49)    /* The directory is locked         */
#define PF_IER_DIR_ALREADY_LOCKED          (50)    /* The directory is already locked */
#define PF_IER_DISK_NOT_INSERTED           (51)    /* The disk is not inserted        */
#define PF_IER_MODULE_SAFE_ERROR           (52)    /* Safe module error               */
#define PF_IER_PARENT_NOT_FOUND            (53)    /* Parent directory is not found   */
#define PF_IER_RESOURCE_FULL               (54)    /* Resource full                   */
#define PF_IER_RESOURCE_ALREADY_EXISTS     (55)    /* Resource already exists         */
#define PF_IER_RESOURCE_OPENED             (56)    /* Resource is opened              */
#define PF_IER_INVALID_CHARS               (57)    /* Invalid char or string          */
#define PF_IER_INCOMPLETE_CHARS            (58)    /* Incomplete string               */
#define PF_IER_NOT_ENOUGH_BUFFER           (59)    /* Not enough buffer size          */


/*---------------------------------------------------------------------------
  Return numbers for API
 ----------------------------------------------------------------------------*/
#define PF_RET_NO_ERR           (0)         /* No error                     */
#define PF_RET_ERR              (-1)        /* API error                    */
#define PF_RET_NULL             NULL        /* NULL                         */
                                            /* [Use API] _create, fopen     */
#define PF_RET_NOT_END_OF_FILE  (0)         /* Not end of file              */
                                            /* [Use API] feof               */
#define PF_RET_END_OF_FILE      (1)         /* End of file                  */
                                            /* [Use API] feof               */
#define PF_RET_DERRNUM_ERR      (1)         /* _derrnum API error           */
                                            /* [Use API] _derrnum           */
#define PF_RET_MODERRNUM_ERR    (1)         /* _module_errnum API error     */
                                            /* [Use API] _ctrl_module,      */
                                            /*           _module_errnum     */
#define PF_RET_UNMOUNT_ERR      (1)         /* _unmount API error           */
                                            /* [Use API] _unmount           */
                                            /* Note: This is for force mode */

#endif /* PF_APICMN_DEF_H */

