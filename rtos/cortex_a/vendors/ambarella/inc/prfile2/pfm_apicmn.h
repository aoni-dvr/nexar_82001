/*
     Copyright (C) 2008 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pfm_apicmn.h] - Definitions and Declarations commonly used in PrFILE2 ExAPI.
 ****************************************************************************/
#ifndef __PFM_APICMN_H__
#define __PFM_APICMN_H__

#include "prfile2/pf_config.h"
#include "prfile2/pfm_types.h"
#include "prfile2/pf_apicmn_def.h"

#if PF_EXAPI_SUPPORT

#if PF_FATINFO_API_SUPPORT
/****************************************************************************
  FAT Information API
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Definitions
 ----------------------------------------------------------------------------*/
/* Handle */
typedef PFM_U_LONG              PFM_HANDLE;

/* The FAT descriptor */
typedef PFM_HANDLE              PFM_FATD;

#define PFM_FATD_VOID           (0)

/* BPB information */
typedef struct
{
    PFM_U_LONG  total_sect;         /* Total sectors                            */
    PFM_U_LONG  total_clst;         /* Total clusters                           */
    PFM_U_LONG  sect_per_FAT;       /* Count of sectors occupied by one FAT     */
    PFM_U_SHORT byte_per_sect;      /* Bytes per sector                         */
    PFM_U_CHAR  sect_per_clst;      /* Sectors per cluster                      */
    PFM_S_CHAR  fat_type;           /* FAT type                                 */
    PFM_U_LONG  root_dir_clst;      /* Cluster number of root directory         */
    PFM_SECT    num_reserved_sect;  /* Number of reserved sectors               */
    PFM_U_SHORT num_fats;           /* The count of FAT on the volume           */
    PFM_SECT    first_data_sect;    /* First sector number of the data region   */
} PFM_FAT_BPB;

/* FAT type */
#define PFM_FAT_12              (0)
#define PFM_FAT_16              (1)
#define PFM_FAT_32              (2)

/* The cluster value */
#define PFM_CLUSTER_FREE        (0)
#define PFM_CLUSTER_BAD         (0xFFFFFFF7ul)
#define PFM_CLUSTER_EOC         (0xFFFFFFFFul)

/* The special cluster value */
#define PFM_CLUSTER_VOID        (0xFFFFFFFFul)
#define PFM_CLUSTER_NONEXISTENT (0xFFFFFFFEul)  /* There is not free cluster in the volume */
#define PFM_CLUSTER_UNKOWN      (0xFFFFFFFDul)  /* The FCS Module can not return free cluster(s).
                                                   because the module does not manage the area. */

/* inerrnum, module_errnum */
typedef struct
{
    PFM_ERROR   err_inerrnum;       /* PrFILE2 API internal error code (PF_IER_XXX) */
    PFM_ERROR   err_driver_errnum;  /* Driver error code */
    PFM_ERROR   err_fcs_errnum;     /* Free Cluster Search Module error code */
    PFM_ERROR   err_safe_errnum;    /* Safe Module error code */
} PFM_FAT_ERRNUM;

/* Drive configurasion code
   for 'req_code' of pfm_fat_getDriveConf()/pfm_fat_setDriveConf(). */
#define PFM_DRV_AUTOMOUNT       (1)
#define PFM_DRV_DISKATTR        (2)
#define PFM_DRV_BUFFERING       (3)
#define PFM_DRV_VOLLABEL        (4)
#define PFM_DRV_FSINFO          (5)
#define PFM_DRV_ACTIVEFAT       (6)
#define PFM_DRV_ACCESSDATE      (7)

/* Automount behavior setting (PFM_FAT_DRVCONF.dc_amount) */
typedef struct
{
    PFM_U_LONG  am_code;    /* same as PFM_FAT_DRVCONF.dc_code */
    PFM_U_LONG  am_mode;    /* Do automount or not */
} PFM_FAT_DRVCONF_AUTOMOUNT;

/* Automount behavior (PFM_FAT_DRVCONF_AUTOMOUNT.am_mode) */
#define PFM_DRV_AUTOMOUNT_ENABLE    (1)
#define PFM_DRV_AUTOMOUNT_DISABLE   (2)

/* Disk attribute (PFM_FAT_DRVCONF.dc_disk_attr) */
typedef struct
{
    PFM_U_LONG  da_code;        /* same as PFM_FAT_DRVCONF.dc_code */
    PFM_U_LONG  da_readonly;    /* Readonly or writable */
    PFM_U_LONG  da_removable;   /* Removable or fixed.
                                   - This field is available for pfm_getDriveConf(). */
} PFM_FAT_DRVCONF_DISKATTR;

/* Readonly or writable (PFM_FAT_DRVCONF_DISKATTR.da_readonly) */
#define PFM_DRV_DISKATTR_READONLY   (1)
#define PFM_DRV_DISKATTR_WRITABLE   (2)

/* Removable or fixed (PFM_FAT_DRVCONF_DISKATTR.da_removable) */
#define PFM_DRV_DISKATTR_REMOVABLE  (3)
#define PFM_DRV_DISKATTR_FIXED      (4)

/* Cache write behavior setting (PFM_FAT_DRVCONF.dc_buffering) */
typedef struct
{
    PFM_U_LONG  buf_code;       /* same as PFM_FAT_DRVCONF.dc_code */
    PFM_U_LONG  buf_mode;       /* same as 'mode' parameter of pf_buffering() */
} PFM_FAT_DRVCONF_BUFFERING;

/* Volume label write behavior setting (PFM_FAT_DRVCONF.dc_vol_label) */
typedef struct
{
    PFM_U_LONG  vl_code;        /* same as PFM_FAT_DRVCONF.dc_code */
    PFM_U_LONG  vl_bpb_update;  /* BS_VolLab field of BPB is updated or not-updated */
} PFM_FAT_DRVCONF_VOLLABEL;

/* BS_VolLab field of BPB is updated or not-updated (PFM_FAT_DRVCONF_VOLLABEL.vl_bpb_update) */
#define PFM_DRV_VOLLABEL_BPB_UPDATE_ENABLE  (1)
#define PFM_DRV_VOLLABEL_BPB_UPDATE_DISABLE (2)

/* FSInfo update behavior setting (PFM_FAT_DRVCONF.dc_fsinfo) */
typedef struct
{
    PFM_U_LONG  fsi_code;       /* same as PFM_FAT_DRVCONF.dc_code */
    PFM_U_LONG  fsi_mode;       /* same as 'mode' parameter of pf_setupfsi() */
} PFM_FAT_DRVCONF_FSINFO;

/* FAT update behavior setting (PFM_FAT_DRVCONF.dc_active_fat) */
typedef struct
{
    PFM_U_LONG  af_code;        /* same as PFM_FAT_DRVCONF.dc_code */
    PFM_U_LONG  af_mirroring;   /* FAT mirroring or not-mirroring */
    PFM_U_LONG  af_active_fat;  /* Active FAT number (0, 1, 2, ..., 15)
                                   - This field is available
                                     when 'af_mirroring' is PFM_DRV_ACTIVEFAT_MIRRORING_DISABLE */
} PFM_FAT_DRVCONF_ACTIVEFAT;

/* FAT mirroring or not-mirroring (PFM_FAT_DRVCONF_ACTIVEFAT.af_mirroring) */
#define PFM_DRV_ACTIVEFAT_MIRRORING_ENABLE  (1)
#define PFM_DRV_ACTIVEFAT_MIRRORING_DISABLE (2)

/* Access date update setting (PFM_FAT_DRVCONF.dc_access_date) */
typedef struct
{
    PFM_U_LONG  ad_code;        /* same as PFM_FAT_DRVCONF.dc_code */
    PFM_U_LONG  ad_update;      /* Access date is updated or not-updated */
} PFM_FAT_DRVCONF_ACCESSDATE;

/* Access date is updated or not-updated (PFM_FAT_DRVCONF_ACCESSDATE.ad_update) */
#define PFM_DRV_ACCESSDATE_UPDATE_ENABLE  (1)
#define PFM_DRV_ACCESSDATE_UPDATE_DISABLE (2)

/* Drive configurations */
typedef union
{
    PFM_U_LONG                  dc_code;        /* Drive configuration code
                                                   - same as 'req_code' of pfm_fat_getDriveConf().  */
    PFM_FAT_DRVCONF_AUTOMOUNT   dc_amount;      /* Automount behavior setting                       */
    PFM_FAT_DRVCONF_DISKATTR    dc_disk_attr;   /* Disk attrubute                                   */
    PFM_FAT_DRVCONF_BUFFERING   dc_buffering;   /* Cache write behavior setting                     */
    PFM_FAT_DRVCONF_VOLLABEL    dc_vol_label;   /* Volume label write behavior setting              */
    PFM_FAT_DRVCONF_FSINFO      dc_fsinfo;      /* FSInfo update behavior setting                   */
    PFM_FAT_DRVCONF_ACTIVEFAT   dc_active_fat;  /* FAT update behavior setting                      */
    PFM_FAT_DRVCONF_ACCESSDATE  dc_access_date; /* Access date update setting                       */
} PFM_FAT_DRVCONF;

/* Sector area */
typedef enum
{
    PFM_SECT_AREA_BPB,      /* BPB, Backup BPB                                          */
    PFM_SECT_AREA_FSINFO,   /* FSInfo, Backup FSInfo (only FAT32)                       */
    PFM_SECT_AREA_RESERVED, /* #1 to #(FAT1-1) except Backup BPB, FSInfo, Backup FSInfo */
    PFM_SECT_AREA_FAT1,     /* FAT1                                                     */
    PFM_SECT_AREA_FAT2,     /* FAT2                                                     */
    PFM_SECT_AREA_ROOT,     /* Root directory (only FAT12, FAT16)                       */
    PFM_SECT_AREA_DATA,     /* Data (are assigned to clusters)                          */
    PFM_SECT_AREA_UNUSED    /* End of Data to end of partition                          */
} PFM_SECT_AREA;

/* PrFILE2 API Code */
typedef PF_API_CODE PFM_API_CODE;
//#define PFM_API_CODE PF_API_CODE

#define PFM_APICODE_NONE            PF_APICODE_NONE
#define PFM_APICODE_FOPEN           PF_APICODE_FOPEN            /* 0x0002 (  2), pf_fopen           */
#define PFM_APICODE_FCLOSE          PF_APICODE_FCLOSE           /* 0x0004 (  3), pf_fclose          */
#define PFM_APICODE_FWRITE          PF_APICODE_FWRITE           /* 0x0005 (  5), pf_fwrite          */
#define PFM_APICODE_REMOVE          PF_APICODE_REMOVE           /* 0x0009 (  9), pf_remove          */
#define PFM_APICODE_RENAME          PF_APICODE_RENAME           /* 0x000a ( 10), pf_rename          */
#define PFM_APICODE_MKDIR           PF_APICODE_MKDIR            /* 0x000b ( 11), pf_mkdir           */
#define PFM_APICODE_RMDIR           PF_APICODE_RMDIR            /* 0x000c ( 12), pf_rmdir           */
#define PFM_APICODE_CHMOD           PF_APICODE_CHMOD            /* 0x000f ( 15), pf_chmod           */
#define PFM_APICODE_CHDMOD          PF_APICODE_CHDMOD           /* 0x0010 ( 16), pf_chdmod          */
#define PFM_APICODE_MOUNT           PF_APICODE_MOUNT            /* 0x0011 ( 17), pf_mount           */
#define PFM_APICODE_FORMAT          PF_APICODE_FORMAT           /* 0x0012 ( 18), pf_format          */
#define PFM_APICODE_BUFFERING       PF_APICODE_BUFFERING        /* 0x0013 ( 19), pf_buffering       */
#define PFM_APICODE_GETDEV          PF_APICODE_GETDEV           /* 0x0017 ( 23), pf_getdev          */
#define PFM_APICODE_SETVOL          PF_APICODE_SETVOL           /* 0x0018 ( 24), pf_setvol          */
#define PFM_APICODE_RMVVOL          PF_APICODE_RMVVOL           /* 0x001a ( 26), pf_rmvvol          */
#define PFM_APICODE_ATTACH          PF_APICODE_ATTACH           /* 0x001b ( 27), pf_attach          */
#define PFM_APICODE_SETUPFSI        PF_APICODE_SETUPFSI         /* 0x001d ( 29), pf_setupfsi        */
#define PFM_APICODE_FSYNC           PF_APICODE_FSYNC            /* 0x0021 ( 33), pf_fsync           */
#define PFM_APICODE_FAPPEND         PF_APICODE_FAPPEND          /* 0x0022 ( 34), pf_fappend         */
#define PFM_APICODE_UNMOUNT         PF_APICODE_UNMOUNT          /* 0x0026 ( 38), pf_unmount         */
#define PFM_APICODE_COMBINE         PF_APICODE_COMBINE          /* 0x0027 ( 39), pf_combine         */
#define PFM_APICODE_DIVIDE          PF_APICODE_DIVIDE           /* 0x002a ( 42), pf_divide          */
#define PFM_APICODE_FADJUST         PF_APICODE_FADJUST          /* 0x002b ( 43), pf_fadjust         */
#define PFM_APICODE_FINFO           PF_APICODE_FINFO            /* 0x002c ( 44), pf_finfo           */
#define PFM_APICODE_MOVE            PF_APICODE_MOVE             /* 0x002d ( 45), pf_move            */
#define PFM_APICODE_CINSERT         PF_APICODE_CINSERT          /* 0x002e ( 46), pf_cinsert         */
#define PFM_APICODE_CDELETE         PF_APICODE_CDELETE          /* 0x0030 ( 48), pf_cdelete         */
#define PFM_APICODE_SETVOLCFG       PF_APICODE_SETVOLCFG        /* 0x0034 ( 52), pf_setvolcfg       */
#define PFM_APICODE_CREATEDIR       PF_APICODE_CREATEDIR        /* 0x0041 ( 65), pf_createdir       */
#define PFM_APICODE_CLEANDIR        PF_APICODE_CLEANDIR         /* 0x0042 ( 66), pf_cleandir        */
#define PFM_APICODE_DELETEDIR       PF_APICODE_DELETEDIR        /* 0x0043 ( 67), pf_deletedir       */
#define PFM_APICODE_TRUNCATE        PF_APICODE_TRUNCATE         /* 0x0044 ( 68), pf_truncate        */
#define PFM_APICODE_FTRUNCATE       PF_APICODE_FTRUNCATE        /* 0x0045 ( 69), pf_ftruncate       */
#define PFM_APICODE_SETSTAMP        PF_APICODE_SETSTAMP         /* 0x0047 ( 71), pf_setstamp        */
#define PFM_APICODE_CHMODDIR        PF_APICODE_CHMODDIR         /* 0x004c ( 76), pf_chmoddir        */
#define PFM_APICODE_REGMOD          PF_APICODE_REGMOD           /* 0x004d ( 77), pf_regmod          */
#define PFM_APICODE_FDIVIDE         PF_APICODE_FDIVIDE          /* 0x0057 ( 87), pf_fdivide         */
#define PFM_APICODE_SETFCSPOS       PF_APICODE_SETFCSPOS        /* 0x005a ( 90), pf_setfcspos       */
#define PFM_APICODE_FSEXEC_REMOVE   PF_APICODE_FSEXEC_REMOVE    /* 0x005b ( 91), pf_fsexec(REMOVE)  */
#define PFM_APICODE_FSEXEC_CHGMOD   PF_APICODE_FSEXEC_CHGMOD    /* 0x005c ( 92), pf_fsexec(CHGMOD)  */
#define PFM_APICODE_FSEXEC_FOPEN    PF_APICODE_FSEXEC_FOPEN     /* 0x005d ( 93), pf_fsexec(FOPEN)   */
#define PFM_APICODE_AUTOMOUNT       PF_APICODE_AUTOMOUNT        /* 0x0060 ( 96), auto-mount         */

/*---------------------------------------------------------------------------
 PROTOTYPES
 ----------------------------------------------------------------------------*/
PFM_ERROR  pfm2_fat_getBPB(PFM_FATD, PFM_FAT_BPB*);
PFM_ERROR  pfm2_fat_getClst(PFM_FATD, PFM_U_LONG, PFM_U_LONG, PFM_U_LONG*, PFM_U_LONG*);
PFM_ERROR  pfm2_fat_searchFreeClst(PFM_FATD, PFM_U_LONG, PFM_U_LONG, PFM_U_LONG*, PFM_U_LONG*);
PF_CHAR    pfm2_fat_getDriveName(PFM_FATD);
PFM_ERROR  pfm2_fat_getErrnum(PFM_FATD, PFM_FAT_ERRNUM*);
PFM_ERROR  pfm2_fat_setErrnum(PFM_FATD, PFM_CONST PFM_FAT_ERRNUM*);
PFM_ERROR  pfm2_fat_getDriveConf(PFM_FATD, PFM_U_LONG, PFM_FAT_DRVCONF*);
PFM_ERROR  pfm2_fat_setDriveConf(PFM_FATD, PFM_U_LONG, PFM_CONST PFM_FAT_DRVCONF*);
PFM_ERROR  pfm2_fat_readSect(PFM_FATD, PFM_U_CHAR*, PFM_SECT, PFM_SECT, PFM_SECT_AREA, PFM_SECT*);
PFM_ERROR  pfm2_fat_writeSect(PFM_FATD, PFM_CONST PFM_U_CHAR*, PFM_SECT, PFM_SECT, PFM_SECT_AREA, PFM_SECT*);
PFM_ERROR  pfm2_fat_pfapi(PFM_FATD, PFM_API_CODE, PF_API*);

#if PF_USE_API_STUB
PFM_ERROR  pfmstub_fat_getBPB(PFM_FATD, PFM_FAT_BPB*);
PFM_ERROR  pfmstub_fat_getClst(PFM_FATD, PFM_U_LONG, PFM_U_LONG, PFM_U_LONG*, PFM_U_LONG*);
PFM_ERROR  pfmstub_fat_searchFreeClst(PFM_FATD, PFM_U_LONG, PFM_U_LONG, PFM_U_LONG*, PFM_U_LONG*);
PF_CHAR    pfmstub_fat_getDriveName(PFM_FATD);
PFM_ERROR  pfmstub_fat_getErrnum(PFM_FATD, PFM_FAT_ERRNUM*);
PFM_ERROR  pfmstub_fat_setErrnum(PFM_FATD, PFM_CONST PFM_FAT_ERRNUM*);
PFM_ERROR  pfmstub_fat_getDriveConf(PFM_FATD, PFM_U_LONG, PFM_FAT_DRVCONF*);
PFM_ERROR  pfmstub_fat_setDriveConf(PFM_FATD, PFM_U_LONG, PFM_CONST PFM_FAT_DRVCONF*);
PFM_ERROR  pfmstub_fat_readSect(PFM_FATD, PFM_U_CHAR*, PFM_SECT, PFM_SECT, PFM_SECT_AREA, PFM_SECT*);
PFM_ERROR  pfmstub_fat_writeSect(PFM_FATD, PFM_CONST PFM_U_CHAR*, PFM_SECT, PFM_SECT, PFM_SECT_AREA, PFM_SECT*);
PFM_ERROR  pfmstub_fat_pfapi(PFM_FATD, PFM_API_CODE, PF_API*);
#endif /* PF_USE_API_STUB */

#endif /* PF_FATINFO_API_SUPPORT */

#endif /* PF_EXAPI_SUPPORT */

#endif /* __PFM_APICMN_H__ */
