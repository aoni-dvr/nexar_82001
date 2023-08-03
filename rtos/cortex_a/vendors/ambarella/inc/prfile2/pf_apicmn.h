/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_apicmn.h] - Definitions and Declarations commonly used in PrFILE2 API.

 NOTES:
  - Except headers which are included by 'pf_apicmn.h' itself, any of
    source (*.c) and header (*.h) files of PrFILE2 shall include this
    header 'pf_apicmn.h' (or 'pf_common.h' which includes the header
    internally) prior to any other header.
 ****************************************************************************/
#ifndef PF_APICMN_H
#define PF_APICMN_H

#include "prfile2/pf_apicmn_def.h"

#if PF_UNICODE_INTERFACE_SUPPORT
#include "prfile2/pf_w_apicmn.h"
#endif /* PF_UNICODE_INTERFACE_SUPPORT */

#ifdef EB_PRFILE2_SUPPORT_PFS_IF
#include "prfile2/pf_e_apicmn.h"
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */

#if PF_EXFAT_SUPPORT
#include "prfile2/pf_exfat_apicmn.h"
#endif /* PF_EXFAT_SUPPORT */


/****************************************************************************
  PrFILE2 API Functions Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 pf2_init_prfile2 - Initialize PrFILE2.
 ----------------------------------------------------------------------------*/
INT32  pf2_init_prfile2(LONG config, void* param);
#if PF_USE_API_STUB
INT32  pfstub_init_prfile2(LONG config, void* param);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_create - Create a file.
 ----------------------------------------------------------------------------*/
PF_FILE*    pf2_create(PF_CONST char* path, INT32 mode);
#if PF_USE_API_STUB
PF_FILE*    pfstub_create(PF_CONST char* path, INT32 mode);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fopen - open a file stream.
 ----------------------------------------------------------------------------*/
PF_FILE*    pf2_fopen(PF_CONST char* path, PF_CONST char* mode);
#if PF_USE_API_STUB
PF_FILE*    pfstub_fopen(PF_CONST char* path, PF_CONST char* mode);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_close - Close a file.
 ----------------------------------------------------------------------------*/
INT32         pf2_fclose(PF_FILE* p_file);
#if PF_USE_API_STUB
INT32         pfstub_fclose(PF_FILE* p_file);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fread - Read data from a file.
 ----------------------------------------------------------------------------*/
PF_SIZE_T   pf2_fread(void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE* p_file);
#if PF_USE_API_STUB
PF_SIZE_T   pfstub_fread(void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE* p_file);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fwrite - Write data into a file.
 ----------------------------------------------------------------------------*/
PF_SIZE_T  pf2_fwrite(void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE* p_file);
#if PF_USE_API_STUB
PF_SIZE_T  pfstub_fwrite(void* p_buf, PF_SIZE_T size, PF_SIZE_T count, PF_FILE* p_file);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fseek -
 ----------------------------------------------------------------------------*/
INT32         pf2_fseek(PF_FILE* p_file, PF_OFF_T offset, INT32 origin);
#if PF_USE_API_STUB
INT32         pfstub_fseek(PF_FILE*, PF_OFF_T, INT32);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsfirst -
 ----------------------------------------------------------------------------*/
INT32         pf2_fsfirst(PF_CONST char* path, UINT8 attrs, PF_DTA* p_dta);
#if PF_USE_API_STUB
INT32         pfstub_fsfirst(PF_CONST char*, unsigned char, PF_DTA*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsnext -
 ----------------------------------------------------------------------------*/
INT32         pf2_fsnext(PF_DTA* p_dta);
#if PF_USE_API_STUB
INT32         pfstub_fsnext(PF_DTA*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fsexec -- Change entry information or remove entry.
 ----------------------------------------------------------------------------*/
INT32         pf2_fsexec(PF_DTA* p_dta, INT32 type, UINT8 attrs);
#if PF_USE_API_STUB
INT32         pfstub_fsexec(PF_DTA*, INT32, unsigned char);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_remove -
 ----------------------------------------------------------------------------*/
INT32         pf2_remove(PF_CONST char* path);
#if PF_USE_API_STUB
int         pfstub_remove(PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_rename -
 ----------------------------------------------------------------------------*/
INT32         pf2_rename(PF_CONST char* sOldName, PF_CONST char* sNewName);
#if PF_USE_API_STUB
int         pfstub_rename(PF_CONST char*, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_move -
 ----------------------------------------------------------------------------*/
INT32         pf2_move(PF_CONST char* sSrcName, PF_CONST char* sDstName);
#if PF_USE_API_STUB
int         pfstub_move(PF_CONST char*, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_mkdir -
 ----------------------------------------------------------------------------*/
INT32         pf2_mkdir(PF_CONST char* sPath);
#if PF_USE_API_STUB
int         pfstub_mkdir(PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_createdir -
 ----------------------------------------------------------------------------*/
INT32         pf2_createdir(PF_CONST char* sPath, INT32 option, PF_DTA* p_dta);
#if PF_USE_API_STUB
int         pfstub_createdir(PF_CONST char*, int, PF_DTA*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_rmdir -
 ----------------------------------------------------------------------------*/
INT32         pf2_rmdir(PF_CONST char* sPath);
#if PF_USE_API_STUB
int         pfstub_rmdir(PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_chdir -
 ----------------------------------------------------------------------------*/
INT32         pf2_chdir(PF_CONST char* sPath);
#if PF_USE_API_STUB
int         pfstub_chdir(PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fstat -
 ----------------------------------------------------------------------------*/
INT32         pf2_fstat(PF_CONST char* sPath, PF_STAT* pStat);
#if PF_USE_API_STUB
int         pfstub_fstat(PF_CONST char*, PF_STAT*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fpstat -
 ----------------------------------------------------------------------------*/
INT32         pf2_fpstat(PF_FILE* p_file, PF_STAT* pStat);
#if PF_USE_API_STUB
int         pfstub_fpstat(PF_FILE*, PF_STAT*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_chmod -
 ----------------------------------------------------------------------------*/
INT32         pf2_chmod(PF_CONST char* path, INT32 attr);
#if PF_USE_API_STUB
int         pfstub_chmod(PF_CONST char*, int);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_chdmod -
 ----------------------------------------------------------------------------*/
INT32         pf2_chdmod(PF_CONST char* sPath, INT32 bAttrs);
#if PF_USE_API_STUB
int         pfstub_chdmod(PF_CONST char*, int);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_attach - Attach drives.
 ----------------------------------------------------------------------------*/
INT32         pf2_attach(PF_DRV_TBL** drv_tbl);
#if PF_USE_API_STUB
int         pfstub_attach(PF_DRV_TBL** drv_tbl);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_detach - Detach drive.
 ----------------------------------------------------------------------------*/
INT32  pf2_detach(char drive);
#if PF_USE_API_STUB
int  pfstub_detach(char drive);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_mount - Attach drives. (OBSOLETE) & No Backword Compatibility, Device Mount.
 ----------------------------------------------------------------------------*/
#if PF_BACKWARD_COMPATIBILITY_MOUNT
INT32         pf2_mount(PF_DRV_TBL** drv_tbl);
#else
INT32         pf2_mount(char drive);
#endif /* PF_BACKWARD_COMPATIBILITY_MOUNT */
#if PF_USE_API_STUB
#if PF_BACKWARD_COMPATIBILITY_MOUNT
INT32         pfstub_mount(PF_DRV_TBL** drv_tbl);
#else
INT32         pfstub_mount(char drive);
#endif /* PF_BACKWARD_COMPATIBILITY_MOUNT */
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_unmount - Unmount drives.
 ----------------------------------------------------------------------------*/
INT32  pf2_unmount(char drive, PF_U_LONG mode);
#if PF_USE_API_STUB
int  pfstub_unmount(char drive, PF_U_LONG mode);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_format - Format a volume specified by 'drive'.
 ----------------------------------------------------------------------------*/
INT32         pf2_format(char drive, PF_CONST char* param);
#if PF_USE_API_STUB
int         pfstub_format(char drive, PF_CONST char* param);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_buffering -
 ----------------------------------------------------------------------------*/
INT32         pf2_buffering(char drv_char, INT32 mode);
#if PF_USE_API_STUB
int         pfstub_buffering(char, int);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_ferror - returns error number of the last occurred error associated
              with the file specified by 'p_file'.
 ----------------------------------------------------------------------------*/
INT32         pf2_ferror(PF_FILE* p_file);
#if PF_USE_API_STUB
int         pfstub_ferror(PF_FILE*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_feof -
 ----------------------------------------------------------------------------*/
INT32         pf2_feof(PF_FILE* p_file);
#if PF_USE_API_STUB
int         pfstub_feof(PF_FILE*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_errnum - returns the error number of the current context

 [Return Value]
    The error number
 ----------------------------------------------------------------------------*/
INT32         pf2_errnum(void);
#if PF_USE_API_STUB
int         pfstub_errnum(void);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_inerrnum - returns the internal error number of current context

 [Return Value]
    The internal error number
 ----------------------------------------------------------------------------*/
INT32         pf2_inerrnum(void);
#if PF_USE_API_STUB
int         pfstub_inerrnum(void);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_setinerrnum - Set the internal error number of the current context
 ----------------------------------------------------------------------------*/
INT32         pf2_setinerrnum(INT32 inerrnum);
#if PF_USE_API_STUB
int         pfstub_setinerrnum(int inerrnum);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_devinf -
 ----------------------------------------------------------------------------*/
INT32         pf2_devinf(char drv_char, PF_DEV_INF* dev_inf);
#if PF_USE_API_STUB
int         pfstub_devinf(char, PF_DEV_INF*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_drvctl -
 ----------------------------------------------------------------------------*/
INT32         pf2_drvctl(char drive, PF_CTL_CMDCODE cmdcode, void *buf, PF_SIZE_T bufbytes);
#if PF_USE_API_STUB
int         pfstub_drvctl(char drive, PF_CTL_CMDCODE cmdcode, void *buf, PF_SIZE_T bufbytes);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_setvol -
 ----------------------------------------------------------------------------*/
INT32         pf2_setvol(char drv_char, PF_CONST char* vname);
#if PF_USE_API_STUB
int         pfstub_setvol(char, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_getvol -
 ----------------------------------------------------------------------------*/
INT32         pf2_getvol(char drv_char, PF_VOLTAB* voltab);
#if PF_USE_API_STUB
int         pfstub_getvol(char, PF_VOLTAB*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_rmvvol -
 ----------------------------------------------------------------------------*/
INT32         pf2_rmvvol(char drv_char);
#if PF_USE_API_STUB
int         pfstub_rmvvol(char);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_setupfsi -
 ----------------------------------------------------------------------------*/
INT32 pf2_setupfsi(char drv_char, INT16 flags);
#if PF_USE_API_STUB
int         pfstub_setupfsi(char, short);
#endif /* PF_USE_API_STUB */

#if PF_USE_CLSTLINK
/*---------------------------------------------------------------------------
 pf2_setclstlink -
 ----------------------------------------------------------------------------*/
INT32 pf2_setclstlink(char drv_char, LONG mode, PF_CLUSTER* inf);
#if PF_USE_API_STUB
int         pfstub_setclstlink(char, LONG, PF_CLUSTER*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsetclstlink -
 ----------------------------------------------------------------------------*/
INT32 pf2_fsetclstlink(PF_FILE* p_file, LONG mode, PF_CLUSTER_FILE* inf);
#if PF_USE_API_STUB
int         pfstub_fsetclstlink(PF_FILE*, LONG, PF_CLUSTER_FILE*);
#endif /* PF_USE_API_STUB */
#endif /* PF_USE_CLSTLINK*/

/*---------------------------------------------------------------------------
 pf2_sync -
 ----------------------------------------------------------------------------*/
INT32 pf2_sync(char drv_char, LONG mode);
#if PF_USE_API_STUB
int         pfstub_sync(char, LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsync -
 ----------------------------------------------------------------------------*/
INT32 pf2_fsync(PF_FILE* p_file);
#if PF_USE_API_STUB
int         pfstub_fsync(PF_FILE*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fappend -
 ----------------------------------------------------------------------------*/
PF_FSIZE_T   pf2_fappend(PF_FILE* p_file, PF_FSIZE_T byte);
#if PF_USE_API_STUB
PF_FSIZE_T   pfstub_fappend(PF_FILE*, PF_FSIZE_T);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fadjust -- Adjust the unused cluster in opened file.
 ----------------------------------------------------------------------------*/
INT32         pf2_fadjust(PF_FILE* p_file);
#if PF_USE_API_STUB
int         pfstub_fadjust(PF_FILE*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fgetdta -- Get DTA from file pointer.
 ----------------------------------------------------------------------------*/
INT32         pf2_fgetdta(PF_FILE* p_file, PF_DTA* p_dta);
#if PF_USE_API_STUB
int         pfstub_fgetdta(PF_FILE*, PF_DTA*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_finfo -- Get file information.
 ----------------------------------------------------------------------------*/
INT32         pf2_finfo(PF_FILE* p_file, PF_INFO* p_info);
#if PF_USE_API_STUB
int         pfstub_finfo(PF_FILE*, PF_INFO*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_derrnum - returns error number of the last occurred driver error.
 ----------------------------------------------------------------------------*/
INT32         pf2_derrnum(char drv_char);
#if PF_USE_API_STUB
int         pfstub_derrnum(char);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_combine - Combine specified files.
 ----------------------------------------------------------------------------*/
INT32         pf2_combine(PF_CONST char* base_path, PF_CONST char* add_path);
#if PF_USE_API_STUB
int         pfstub_combine(PF_CONST char*, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fconcat - Combine specified files.
 ----------------------------------------------------------------------------*/
INT32         pf2_fconcat(PF_CONST char* base_path, PF_CONST char* add_path);
#if PF_USE_API_STUB
int         pfstub_fconcat(PF_CONST char*, PF_CONST char*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_divide -- Divide specified file.
 ----------------------------------------------------------------------------*/
INT32         pf2_divide(PF_CONST char* org_path, PF_CONST char* new_path, PF_FPOS_T offset);
#if PF_USE_API_STUB
int         pfstub_divide(PF_CONST char*, PF_CONST char*, PF_FPOS_T);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_xdivide -- Divide specified file.
 ----------------------------------------------------------------------------*/
INT32         pf2_xdivide(PF_CONST char* org_path, PF_CONST char* new_path, PF_SIZE_T offset);
#if PF_USE_API_STUB
int         pfstub_xdivide(PF_CONST char*, PF_CONST char*, PF_SIZE_T);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_fdivide -- Divide specified file.
 ----------------------------------------------------------------------------*/
INT32         pf2_fdivide(PF_FDIVIDE *pfdivide);
#if PF_USE_API_STUB
int         pfstub_fdivide(PF_FDIVIDE *pfdivide);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_cinsert -- Insert clusters in file.
 ----------------------------------------------------------------------------*/
INT32         pf2_cinsert(PF_CONST char* p_path, PF_U_LONG cluster_offset, PF_U_LONG num_cluster);
#if PF_USE_API_STUB
int         pfstub_cinsert(PF_CONST char*, PF_U_LONG, PF_U_LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_insert -- Insert cluster in file.
 ----------------------------------------------------------------------------*/
INT32         pf2_insert(PF_CONST char* p_path, PF_U_LONG num_cluster);
#if PF_USE_API_STUB
int         pfstub_insert(PF_CONST char*, PF_U_LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_cdelete -- Delete clusters in file.
 ----------------------------------------------------------------------------*/
INT32         pf2_cdelete(PF_CONST char* p_path, PF_U_LONG cluster_offset, PF_U_LONG num_cluster);
#if PF_USE_API_STUB
int         pfstub_cdelete(PF_CONST char*, PF_U_LONG, PF_U_LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_cut -- Delete cluster in file.
 ----------------------------------------------------------------------------*/
INT32         pf2_cut(PF_CONST char* p_path, PF_U_LONG cluster_offset);
#if PF_USE_API_STUB
int         pfstub_cut(PF_CONST char*, PF_U_LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_settailbuf --
 ----------------------------------------------------------------------------*/
INT32         pf2_settailbuf(char drive, PF_U_LONG size, PF_TAIL_BUF* tailbuf);
#if PF_USE_API_STUB
int         pfstub_settailbuf(char, PF_U_LONG, PF_TAIL_BUF*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_setvolcfg --
 ----------------------------------------------------------------------------*/
INT32         pf2_setvolcfg(char drive, PF_VOL_CFG* config);
#if PF_USE_API_STUB
int         pfstub_setvolcfg(char, PF_VOL_CFG*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
  pf2_getvolcfg --
 ----------------------------------------------------------------------------*/
INT32         pf2_getvolcfg(char drive, PF_VOL_CFG* config);
#if PF_USE_API_STUB
int         pfstub_getvolcfg(char, PF_VOL_CFG*);
#endif

/*---------------------------------------------------------------------------
  pf2_setcode --
 ----------------------------------------------------------------------------*/
INT32         pf2_setcode(PF_CHARCODE* p_codeset);
#if PF_USE_API_STUB
int         pfstub_setcode(PF_CHARCODE*);
#endif

/*---------------------------------------------------------------------------
 pf2_opendir -
 ----------------------------------------------------------------------------*/
PF_DIR*     pf2_opendir(PF_CONST char* path);
#if PF_USE_API_STUB
PF_DIR*     pfstub_opendir(PF_CONST char*);
#endif

/*---------------------------------------------------------------------------
 pf2_closedir -
 ----------------------------------------------------------------------------*/
INT32         pf2_closedir(PF_DIR* p_dir);
#if PF_USE_API_STUB
int         pfstub_closedir(PF_DIR* p_dir);
#endif

/*---------------------------------------------------------------------------
 pf2_readdir -
 ----------------------------------------------------------------------------*/
INT32         pf2_readdir(PF_DIR* p_dir, PF_DIRENT* p_dirent);
#if PF_USE_API_STUB
int         pfstub_readdir(PF_DIR* p_dir, PF_DIRENT* p_dirent);
#endif

/*---------------------------------------------------------------------------
 pf2_telldir -
 ----------------------------------------------------------------------------*/
INT32         pf2_telldir(PF_DIR* p_dir, PF_S_LONG* p_offset);
#if PF_USE_API_STUB
int         pfstub_telldir(PF_DIR* p_dir, PF_S_LONG* p_offset);
#endif

/*---------------------------------------------------------------------------
 pf2_seekdir -
 ----------------------------------------------------------------------------*/
INT32         pf2_seekdir(PF_DIR* p_dir, PF_S_LONG offset);
#if PF_USE_API_STUB
int         pfstub_seekdir(PF_DIR* p_dir, PF_S_LONG offset);
#endif

/*---------------------------------------------------------------------------
 pf2_rewinddir -
 ----------------------------------------------------------------------------*/
INT32         pf2_rewinddir(PF_DIR* p_dir);
#if PF_USE_API_STUB
int         pfstub_rewinddir(PF_DIR* p_dir);
#endif

/*---------------------------------------------------------------------------
 pf2_fchdir -
 ----------------------------------------------------------------------------*/
INT32         pf2_fchdir(PF_DIR* p_dir);
#if PF_USE_API_STUB
int         pfstub_fchdir(PF_DIR* p_dir);
#endif

/*---------------------------------------------------------------------------
 pf2_regctx -
 ----------------------------------------------------------------------------*/
INT32         pf2_regctx(void);
#if PF_USE_API_STUB
int         pfstub_regctx(void);
#endif

/*---------------------------------------------------------------------------
 pf2_unregctx -
 ----------------------------------------------------------------------------*/
INT32         pf2_unregctx(void);
#if PF_USE_API_STUB
int         pfstub_unregctx(void);
#endif

/*---------------------------------------------------------------------------
 pf2_flock -
 ----------------------------------------------------------------------------*/
INT32         pf2_flock(PF_FILE* p_file, PF_U_LONG lock_mode);
#if PF_USE_API_STUB
int         pfstub_flock(PF_FILE*, PF_U_LONG);
#endif

/*---------------------------------------------------------------------------
 pf2_setencode -
 ----------------------------------------------------------------------------*/
INT32         pf2_setencode(PF_U_LONG encode_mode);
#if PF_USE_API_STUB
int         pfstub_setencode(PF_U_LONG encode_mode);
#endif

/*---------------------------------------------------------------------------
 pf2_cleandir -
 ----------------------------------------------------------------------------*/
INT32         pf2_cleandir(PF_CONST char* p_path, PF_CONST char* p_pattern,
                         ULONG mode, ULONG* p_count);
#if PF_USE_API_STUB
int         pfstub_cleandir(PF_CONST char* p_path, PF_CONST char* p_pattern,
                            unsigned long mode, unsigned long* p_count);
#endif

/*---------------------------------------------------------------------------
 pf2_deletedir -
 ----------------------------------------------------------------------------*/
INT32         pf2_deletedir(PF_CONST char* p_path);
#if PF_USE_API_STUB
int         pfstub_deletedir(PF_CONST char* p_path);
#endif

/*---------------------------------------------------------------------------
 pf2_chmoddir -
 ----------------------------------------------------------------------------*/
INT32         pf2_chmoddir(PF_CONST char *dname, PF_CONST char *fname,
                         ULONG mode, ULONG attr,
                         ULONG *p_count);
#if PF_USE_API_STUB
int         pfstub_chmoddir(PF_CONST char *dname, PF_CONST char *fname,
                            unsigned long mode, unsigned long attr,
                            unsigned long *p_count);
#endif

/*---------------------------------------------------------------------------
 pf2_getempent -
 ----------------------------------------------------------------------------*/
INT32         pf2_getempent(LONG *p_entries);
#if PF_USE_API_STUB
int         pfstub_getempent(long *p_entries);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_dgetempent -
 ----------------------------------------------------------------------------*/
INT32  pf2_dgetempent(char drive, LONG *p_entries);
#if PF_USE_API_STUB
int         pfstub_dgetempent(char drive, long *p_entries);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_truncate -
 ----------------------------------------------------------------------------*/
INT32         pf2_truncate(const char* p_path, PF_FPOS_T offset);
#if PF_USE_API_STUB
int         pfstub_truncate(const char* p_path, PF_FPOS_T offset);
#endif

/*---------------------------------------------------------------------------
 pf2_ftruncate -
 ----------------------------------------------------------------------------*/
INT32         pf2_ftruncate(PF_FILE* p_file, PF_FPOS_T offset);
#if PF_USE_API_STUB
int         pfstub_ftruncate(PF_FILE* p_file, PF_FPOS_T offset);
#endif

/*---------------------------------------------------------------------------
 pf2_getstamp -
 ----------------------------------------------------------------------------*/
INT32         pf2_getstamp(const char* p_path, PF_TIMESTMP* p_timestamp);
#if PF_USE_API_STUB
int         pfstub_getstamp(const char* p_path, PF_TIMESTMP* p_timestamp);
#endif

/*---------------------------------------------------------------------------
 pf2_setstamp -
 ----------------------------------------------------------------------------*/
INT32         pf2_setstamp(const char* p_path, PF_TIMESTMP* p_timestamp);
#if PF_USE_API_STUB
int         pfstub_setstamp(const char* p_path, PF_TIMESTMP* p_timestamp);
#endif

/*---------------------------------------------------------------------------
 pf2_getstmp -
 ----------------------------------------------------------------------------*/
INT32         pf2_getstmp(const char*     p_path,
                        PF_SYS_DATE*   p_mkdate,
                        PF_SYS_TIME*   p_mktime,
                        PF_SYS_DATE*   p_update,
                        PF_SYS_TIME*   p_uptime);
#if PF_USE_API_STUB
int         pfstub_getstmp(const char*  p_path,
                           PF_SYS_DATE* p_mkdate,
                           PF_SYS_TIME* p_mktime,
                           PF_SYS_DATE* p_update,
                           PF_SYS_TIME* p_uptime);
#endif

/*---------------------------------------------------------------------------
 pf2_setstmp -
 ----------------------------------------------------------------------------*/
INT32         pf2_setstmp(const char*     p_path,
                        PF_SYS_DATE*   p_mkdate,
                        PF_SYS_TIME*   p_mktime,
                        PF_SYS_DATE*   p_update,
                        PF_SYS_TIME*   p_uptime);
#if PF_USE_API_STUB
int         pfstub_setstmp(const char*  p_path,
                           PF_SYS_DATE* p_mkdate,
                           PF_SYS_TIME* p_mktime,
                           PF_SYS_DATE* p_update,
                           PF_SYS_TIME* p_uptime);
#endif

/*---------------------------------------------------------------------------
 pf2_fgetpos - Get current file I/O pointer
 ----------------------------------------------------------------------------*/
INT32         pf2_fgetpos(PF_FILE* p_file, PF_FPOS_T* pos);
#if PF_USE_API_STUB
int         pfstub_fgetpos(PF_FILE*, PF_FPOS_T*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_fsetpos - Set current file I/O pointer
 ----------------------------------------------------------------------------*/
INT32         pf2_fsetpos(PF_FILE* p_file, const PF_FPOS_T* pos);
#if PF_USE_API_STUB
int         pfstub_fsetpos(PF_FILE*, const PF_FPOS_T*);
#endif /* PF_USE_API_STUB */

#if PF_MODULE_SUPPORT
/*---------------------------------------------------------------------------
 pf2_reg_module - Register/release a module I/F
 ----------------------------------------------------------------------------*/
INT32         pf2_reg_module(char drive, PF_MODULE_ID modid, void* p_module, void* p_param);
#if PF_USE_API_STUB
int         pfstub_reg_module(char, PF_MODULE_ID, void*, void*);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_ctrl_module - ioctl for the module
 ----------------------------------------------------------------------------*/
INT32         pf2_ctrl_module(char drive, PF_MODULE_ID modid, PF_U_LONG req_code, void* p_param);
#if PF_USE_API_STUB
int         pfstub_ctrl_module(char, PF_MODULE_ID, PF_U_LONG, void*);
#endif /* PF_USE_API_STUB */

INT32         pf2_ctrl_module_nb(char drive, PF_MODULE_ID modid, PF_U_LONG req_code, void* p_param);

/*---------------------------------------------------------------------------
 pf2_module_errnum - The last error of the module
 ----------------------------------------------------------------------------*/
INT32         pf2_module_errnum(char drive, PF_MODULE_ID modid);
#if PF_USE_API_STUB
int         pfstub_module_errnum(char, PF_MODULE_ID);
#endif /* PF_USE_API_STUB */

#endif /* PF_MODULE_SUPPORT */

/*---------------------------------------------------------------------------
  pf2_dlock -- Lock the directory
 ----------------------------------------------------------------------------*/
INT32         pf2_dlock(PF_DIR* p_dir, PF_U_LONG lock_mode);
#if PF_USE_API_STUB
int         pfstub_dlock(PF_DIR*, PF_U_LONG);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_setfcspos - Set the last free cluster number of the drive
 ----------------------------------------------------------------------------*/
INT32         pf2_setfcspos(char drive, PF_CLST clst);
#if PF_USE_API_STUB
int         pfstub_setfcspos(char, PF_CLST);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_iconv
 ----------------------------------------------------------------------------*/
#if PF_ICONV_SUPPORT
INT32 pf2_iconv_install(PF_CONST PF_ICONV_CODESET *codeset, char codename[PF_ICONV_MAX_CODENAME+1U]);
INT32 pf2_iconv_uninstall(PF_CONST char *codename);
INT32 pf2_iconv_open(PF_ICONV *cp, PF_CONST char *to_codename, PF_CONST char *from_codename);
INT32 pf2_iconv_close(PF_ICONV *cp);
PF_SIZE_T pf2_iconv(PF_ICONV *cp, void **inbuf, PF_SIZE_T *inbytesleft, void **outbuf, PF_SIZE_T *outbytesleft);
#if PF_USE_API_STUB
int pfstub_iconv_install(PF_CONST PF_ICONV_CODESET *codeset, char codename[PF_ICONV_MAX_CODENAME+1]);
int pfstub_iconv_uninstall(PF_CONST char *codename);
int pfstub_iconv_open(PF_ICONV *cp, PF_CONST char *to_codename, PF_CONST char *from_codename);
int pfstub_iconv_close(PF_ICONV *cp);
#endif /* PF_USE_API_STUB */
#endif /* PF_ICONV_SUPPORT*/

/*---------------------------------------------------------------------------
 pf2_dp_fstat
 ----------------------------------------------------------------------------*/
INT32 pf2_dp_fstat(PF_DIR *p_dir, PF_CONST char *basename, PF_STAT *statbuf);
#if PF_USE_API_STUB
int pfstub_dp_fstat(PF_DIR *p_dir, PF_CONST char* basename, PF_STAT* statbuf);
#endif /* PF_USE_API_STUB */

/*---------------------------------------------------------------------------
 pf2_dp_telldir
 ----------------------------------------------------------------------------*/
INT32 pf2_dp_telldir(PF_DIR *p_dir, PF_S_LONG *search_offset);
#if PF_USE_API_STUB
int pfstub_dp_telldir(PF_DIR *p_dir, PF_S_LONG *search_offset);
#endif /* PF_USE_API_STUB */

#endif  /* PF_APICMN_H */
