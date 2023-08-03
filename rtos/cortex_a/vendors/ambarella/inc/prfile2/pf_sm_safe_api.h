/*
     Copyright (C) 2010 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_sm_safe_api.h] - Declarations of PrFILE2 FAT Safe API functions
 ****************************************************************************/
#ifndef __PF_SM_SAFE_API_H__
#define __PF_SM_SAFE_API_H__

#include "prfile2/pf_apicmn_def.h"
#include "prfile2/plugin/safe/fatsafe/pf_sm_safe.h"

/****************************************************************************
  Public struct definitions
****************************************************************************/
typedef struct
{
    PFSAFE_INFO             si_info;    /*  */
    pfsafe_callback_malloc  si_malloc;  /*  */
    pfsafe_callback_free    si_free;    /*  */
} PF_SAFE_INIT;

/* enum for PF_SAFE_STAT.ss_stat */
typedef enum
{
    PF_SAFE_STAT_NORMAL,
    PF_SAFE_STAT_ABNORMAL,
    PF_SAFE_STAT_ERROR,
    PF_SAFE_STAT_UNSUPPORTED
} PF_SAFE_STAT_FS;

typedef struct
{
    PF_SAFE_STAT_FS         ss_stat;    /* Status of filesystem */
    unsigned long           ss_apicode; /* API code number      */
} PF_SAFE_STAT;


/****************************************************************************
  PrFILE2 FAT Safe API Functions Prototypes
 ****************************************************************************/
int  pf_safe_initialize(char drive, PF_SAFE_INIT  *p_param);
int  pf_safe_finalize  (char drive);
int  pf_safe_check     (char drive, PF_SAFE_STAT *p_sstat);
int  pf_safe_recover   (char drive);
int  pf_safe_backup_on (char drive);
int  pf_safe_backup_off(char drive);
int  pf_safe_readonly  (char drive);

/****************************************************************************
  Internal definitions
****************************************************************************/

/* PrFILE2 API */
#if PF_STDLIB_ALIAS_SUPPORT

#define  PFSAFE_fstat(a,b)           _fstat(a,b)
#define  PFSAFE_format(a,b)          _format(a,b)
#define  PFSAFE_setinerrnum(a)       _setinerrnum(a)
#define  PFSAFE_inerrnum()           _inerrnum()
#define  PFSAFE_ctrl_module(a,b,c,d) _ctrl_module(a,b,c,d)
#define  PFSAFE_reg_module(a,b,c,d)  _reg_module(a,b,c,d)
#define  PFSAFE_mount(a)             _mount(a)
#define  PFSAFE_unmount(a,b)         _unmount(a,b)

#else /* ! PF_STDLIB_ALIAS_SUPPORT */

#define  PFSAFE_fstat(a,b)           pf_fstat(a,b)
#define  PFSAFE_format(a,b)          pf_format(a,b)
#define  PFSAFE_setinerrnum(a)       pf_setinerrnum(a)
#define  PFSAFE_inerrnum()           pf_inerrnum()
#define  PFSAFE_ctrl_module(a,b,c,d) pf_ctrl_module(a,b,c,d)
#define  PFSAFE_reg_module(a,b,c,d)  pf_reg_module(a,b,c,d)
#define  PFSAFE_mount(a)             pf_mount(a)
#define  PFSAFE_unmount(a,b)         pf_unmount(a,b)

#endif /* ! PF_STDLIB_ALIAS_SUPPORT */

/* meta data */
typedef struct
{
    PFSAFE_MDSTAT md_stat;
    PFM_SECT      md_sect;
} PFSAFE_API_META_DATA;

/* recovery data */
typedef struct
{
    PFSAFE_RDSTAT rd_stat;
    PFM_CLST      rd_clst;
    PFM_SECT      rd_num_sect;
    PFM_API_CODE  rd_apicode;
    PFM_U_LONG    rd_sign;
} PFSAFE_API_RCV_DATA;

/* revcovery file */
typedef struct
{
    PFSAFE_RFSTAT rf_stat;
} PFSAFE_API_RCV_FILE;

/* macros for pf_safe_backup_on  */
#define PFSAFE_REQ_RCV_DATA_CREATE          (0x0001)
#define PFSAFE_REQ_RCV_DATA_INITIALIZE      (0x0002)
#define PFSAFE_REQ_META_DATA_INITIALIZE     (0x0010)
#define PFSAFE_REQ_META_DATA_CREATE         (0x0020)
#define PFSAFE_REQ_RCV_FILE_OVERWRITE       (0x0100)
#define PFSAFE_REQ_RCV_FILE_CREATE          (0x0200)



#endif  /* __PF_SM_SAFE_API_H__ */
