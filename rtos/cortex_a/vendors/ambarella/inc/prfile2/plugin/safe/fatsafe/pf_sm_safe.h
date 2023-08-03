/*
    Copyright(C) 2010 eSOL Co., Ltd. All rights reserved.

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
[pf_sm_safe.h] - Definitions of "PrFILE2 FAT Safe"
****************************************************************************/
#ifndef __PF_SM_SAFE_H__
#define __PF_SM_SAFE_H__

#include "prfile2/pfm_module.h"
#include "prfile2/plugin/safe/fatsafe/pf_sm_safe_config.h"

/****************************************************************************
  Function prototypes for user application
****************************************************************************/
/* Memory allocation function */
typedef int  (* pfsafe_callback_malloc) (char drive, PFM_U_LONG size, PFM_U_LONG **pp_bufAddr);

/* Memory release function */
typedef int  (* pfsafe_callback_free) (char drive, PFM_U_LONG *p_bufAddr);


/****************************************************************************
  Macros
****************************************************************************/
/* special value */
#define PFSAFE_FIRST_VALID_CLUSTER        (0x00000002ul)
#define PFSAFE_SECTOR_VOID                (0xFFFFFFFFul)
#define PFSAFE_CLUSTER_VOID               (0xFFFFFFFFul)
#define PFSAFE_INDEX_VOID                 (0xFFFFFFFFul)

/* Spec version */
#define PFSAFE_SPEC_VERSION               (100)

/* Magic code */
#define PFSAFE_MAGIC_CODE                 "PRF2SAFE"
#define PFSAFE_MAGIC_CODE_SIZE            (8)

/* inline function */
#define PFSAFE_INLINE                     ESOL_INLINE

/****************************************************************************
  Internal Configuration
****************************************************************************/
/* update bitmap status */
#define PFSAFE_UBMP_STAT_SECT_PER_BIT      (4)
#define PFSAFE_UBMP_STAT_LOG2_SECT_PER_BIT (2)

/* maximum number of directory entry backup sector */
#define PFSAFE_DENT_BACKUP_MAX_NUM_SECT    (9)
#define PFSAFE_DENT_FOOTER_NUM_SECT        (PFSAFE_DENT_BACKUP_MAX_NUM_SECT)

/* generic buffer size */
#define PFSAFE_GENERIC_BUF_NUM_SECT        (4)

/* recovery data */
#define PFSAFE_METADATA_SEARCH_START_SECT  (1)
#define PFSAFE_RCVD_AREA_SEARCH_START_CLST (2)

/* recovery file name */
#define PFSAFE_RECOVERY_FILE_NAME_MAX_LEN  (12)

/* debug function */
#define PFSAFE_DEBUG                       (0)

/****************************************************************************
  Difinitions for control I/F
****************************************************************************/
/* req_code */
#define PFSAFE_CONTROL_CHECK_META_DATA          (1)
#define PFSAFE_CONTROL_CHECK_RECOVERY_DATA      (2)
#define PFSAFE_CONTROL_CHECK_RECOVERY_FILE      (3)
#define PFSAFE_CONTROL_CREATE_META_DATA         (4)
#define PFSAFE_CONTROL_CREATE_RECOVERY_DATA     (5)
#define PFSAFE_CONTROL_CREATE_RECOVERY_FILE     (6)
#define PFSAFE_CONTROL_INIT_RECOVERY_DATA       (7)
#define PFSAFE_CONTROL_GET_MODE                 (8)
#define PFSAFE_CONTROL_SET_MODE                 (9)
#define PFSAFE_CONTROL_GET_CONFIG               (10)
#define PFSAFE_CONTROL_SET_CONFIG               (11)
#define PFSAFE_CONTROL_RECOVER_FILE_SYSTEM      (12)
#define PFSAFE_CONTROL_GET_ATTR_READONLY        (13)
#if PFSAFE_DEBUG
#define PFSAFE_CONTROL_READ_UBMP                (100)
#define PFSAFE_CONTROL_RESTORE_FAT              (101)
#define PFSAFE_CONTROL_RESTORE_DENT             (102)
#endif /* PFSAFE_DEBUG */

/*
  Struct definition for
    - PFSAFE_CONTROL_CHECK_META_DATA
    - PFSAFE_CONTROL_CHECK_RECOVERY_DATA
    - PFSAFE_CONTROL_CHECK_RECOVERY_FILE
    - PFSAFE_CONTROL_CREATE_META_DATA
    - PFSAFE_CONTROL_CREATE_RECOVERY_DATA
    - PFSAFE_CONTROL_CREATE_RECOVERY_FILE
    - PFSAFE_CONTROL_RECOVER_FILE_SYSTEM
*/
typedef enum
{
    PFSAFE_MDSTAT_NONE,                 /* Meta data is not found   */
    PFSAFE_MDSTAT_VALID,                /* Meta data is valid       */
    PFSAFE_MDSTAT_INVALID               /* Meta data is invalid     */
} PFSAFE_MDSTAT;

typedef enum
{
    PFSAFE_RDSTAT_NONE,                 /* Recovery data is not found                       */
    PFSAFE_RDSTAT_MAIN_VALID,           /* Main management data is valid                    */
    PFSAFE_RDSTAT_BACKUP_VALID,         /* Backup management data is valid                  */
    PFSAFE_RDSTAT_INVALID               /* Recovery data is invalid                         */
} PFSAFE_RDSTAT;

typedef enum
{
    PFSAFE_RFSTAT_NONE,                 /* Recovery file is not found */
    PFSAFE_RFSTAT_VALID,                /* Recovery file is valid     */
    PFSAFE_RFSTAT_INVALID               /* Recovery file is invalid   */
} PFSAFE_RFSTAT;

typedef struct
{
    /* Meta data */
    PFM_SECT            ri_metd_sect;   /* Meta data sector          */
    PFSAFE_MDSTAT       ri_metd_stat;   /* Meta data status          */

    /* Recovery data */
    PFM_CLST            ri_rcvd_clst;     /* Recovery data cluster           */
    PFM_U_LONG          ri_rcvd_num_sect; /* Number of Recovery data sectors */
    PFSAFE_RDSTAT       ri_rcvd_stat;     /* Recovery data status            */
    PFM_API_CODE        ri_rcvd_apicode;  /* API code                        */
    PFM_U_LONG          ri_rcvd_sign;     /* Signature                       */

    /* Recovery file */
    PFSAFE_RFSTAT       ri_rcvf_stat;     /* Recovery file status            */
} PFSAFE_CTL_RCV_INFO;

/* PFSAFE_CONTROL_GET_CONFIG */
typedef struct
{
    PFM_CHAR      cgc_rcvf_name[PFSAFE_RECOVERY_FILE_NAME_MAX_LEN + 1];
    PFM_BOOL      cgc_check_rcvf;  /* PFSAFE_CHECK_RECOVERY_FILE */
    PFM_SECT      cgc_metd_sect;   /* PFSAFE_META_DATA_SECTOR    */
} PFSAFE_CTL_GET_CONFIG;


/* PFSAFE_CONTROL_SET_CONFIG */
typedef struct
{
    PFM_CHAR      *csc_rcvf_name;
    PFM_BOOL      *csc_check_rcvf;  /* PFSAFE_CHECK_RECOVERY_FILE */
    PFM_SECT      *csc_metd_sect;   /* PFSAFE_META_DATA_SECTOR    */
} PFSAFE_CTL_SET_CONFIG;

/* PFSAFE_CONTROL_GET_ATTR_READONLY */
typedef struct
{
    PFM_BOOL    cgr_is_read_only;
} PFSAFE_CTL_GET_ATTR_READONLY;

/****************************************************************************
  Struct definitions
****************************************************************************/
/*------------------------
   PFSAFE_INFO.si_mode
 ------------------------*/
typedef enum
{
    PFSAFE_INFO_MODE_INVALID,
    PFSAFE_INFO_MODE_STANDBY,
    PFSAFE_INFO_MODE_READY,
    PFSAFE_INFO_MODE_BACKUP,
    PFSAFE_INFO_MODE_SAFE

} PFSAFE_INFO_MODE;

typedef struct
{
    PFM_U_LONG  *us_addr;
    PFM_U_LONG  us_size;
    PFM_U_LONG  us_num_bit;
} PFSAFE_UBMP_STAT;

typedef struct
{
    PFM_SECT    ds_sect;
    PFM_SECT    ds_num_sect;
} PFSAFE_DENT_STAT;

/*------------------------
   PFSAFE_INFO
 ------------------------*/
typedef struct
{
    PFM_U_LONG                  si_sign;            /* signature of PFSAFE_INFO        */
    PFSAFE_INFO_MODE            si_mode;            /* mode of this module             */

    /* Information of API transaction */
    struct
    {
        /* Transaction management data */
        PFM_U_LONG              sia_stat;           /* status of executing api         */
        PFM_API_CODE            sia_apicode;        /* code of executing api           */
        PFM_U_LONG              sia_sign;           /* unique signature of transaction */
        PFM_ERROR               sia_moderrnum;      /* module error code               */

        /* Status of Update Bitmap Sectors */
        PFSAFE_UBMP_STAT        sia_ubmp_stat;

        /* Status of DE backup */
        PFSAFE_DENT_STAT        sia_dent_stat[PFSAFE_DENT_BACKUP_MAX_NUM_SECT];
    } api;

    /* Buffers */
    struct
    {
        /* NOTE */
        /* thease buffers are contained in user buffer(p_siu_bufaddr) */

        /* Update bitmap */
        PFM_U_LONG              *p_sib_ubmpaddr;    /* address of update bitmap buffer        */
        PFM_SECT                sib_num_ubmpsect;   /* number of update bitmap buffer sectors */

        /* Generic buffer */
        PFM_U_LONG              *p_sib_bufaddr;     /* address of generic buffer              */
        PFM_SECT                sib_num_bufsect;    /* number of update bitmap buffer sectors */
    } buf;

    /* Configurations */
    struct
    {
        PFM_CHAR                sic_rcvf_name[PFSAFE_RECOVERY_FILE_NAME_MAX_LEN + 1];  /* PFSAFE_RECOVERY_FILE_NAME  */
        PFM_BOOL                sic_check_rcvf;     /* PFSAFE_CHECK_RECOVERY_FILE */
        PFM_SECT                sic_metd_sect;      /* PFSAFE_META_DATA_SECTOR    */
    } cfg;

    /* User items */
    struct
    {
        /* User buffer */
        PFM_U_LONG              *p_siu_bufaddr;     /* address of user buffer */
        PFM_U_LONG              siu_bufsize;        /* size of user buffer    */

        /* Memory allocate/release function */
        pfsafe_callback_malloc  siu_malloc;         /* function pointer of user malloc() */
        pfsafe_callback_free    siu_free;           /* function pointer of user free()   */
    } usr;

    /* Recovery information */
    struct
    {
        PFM_SECT                sir_rcvd_sect;      /* start sector of recovery data  */
        PFSAFE_RDSTAT           sir_rcvd_stat;      /* status of recovery data        */
        PFM_U_LONG              sir_rcvd_sign;      /* signature of recovery data     */
        PFM_API_CODE            sir_rcvd_api;       /* api code of recovery data      */
    } rcv;

    /* Media informaition */
    struct
    {
        PFM_FAT_BPB             sid_bpb;             /* media BPB information         */
        PFM_BOOL                sid_save_mirroring;  /* original FAT mirroring mode   */
        PFM_U_LONG              sid_save_active_fat; /* original active FAT number    */
    } drv;
} PFSAFE_INFO;

/* Mode operation macros */
#define PFSAFE_GET_MODE(p_info)         ((p_info)->si_mode)
#define PFSAFE_SET_MODE(p_info,mode)    ((p_info)->si_mode = (mode))

#define PFSAFE_SET_MODE_INVALID(p_info) PFSAFE_SET_MODE(p_info,PFSAFE_INFO_MODE_INVALID)
#define PFSAFE_SET_MODE_STANDBY(p_info) PFSAFE_SET_MODE(p_info,PFSAFE_INFO_MODE_STANDBY)
#define PFSAFE_SET_MODE_READY(p_info)   PFSAFE_SET_MODE(p_info,PFSAFE_INFO_MODE_READY)
#define PFSAFE_SET_MODE_BACKUP(p_info)  PFSAFE_SET_MODE(p_info,PFSAFE_INFO_MODE_BACKUP)
#define PFSAFE_SET_MODE_SAFE(p_info)    PFSAFE_SET_MODE(p_info,PFSAFE_INFO_MODE_SAFE)

/* Mode check macros */
#define PFSAFE_IS_INITIALIZED(p_info)   (((PFSAFE_GET_MODE(p_info) == PFSAFE_INFO_MODE_STANDBY)  \
                                         || (PFSAFE_GET_MODE(p_info) == PFSAFE_INFO_MODE_READY)  \
                                         || (PFSAFE_GET_MODE(p_info) == PFSAFE_INFO_MODE_BACKUP) \
                                         || (PFSAFE_GET_MODE(p_info) == PFSAFE_INFO_MODE_SAFE))  \
                                         ? PFM_TRUE : PFM_FALSE)

#define PFSAFE_IS_INVALID(p_info)       ((PFSAFE_GET_MODE(p_info) == PFSAFE_INFO_MODE_INVALID) ? PFM_TRUE : PFM_FALSE)
#define PFSAFE_IS_STANDBY_MODE(p_info)  ((PFSAFE_GET_MODE(p_info) == PFSAFE_INFO_MODE_STANDBY) ? PFM_TRUE : PFM_FALSE)
#define PFSAFE_IS_READY_MODE(p_info)    ((PFSAFE_GET_MODE(p_info) == PFSAFE_INFO_MODE_READY  ) ? PFM_TRUE : PFM_FALSE)
#define PFSAFE_IS_BACKUP_MODE(p_info)   ((PFSAFE_GET_MODE(p_info) == PFSAFE_INFO_MODE_BACKUP ) ? PFM_TRUE : PFM_FALSE)
#define PFSAFE_IS_SAFE_MODE(p_info)     ((PFSAFE_GET_MODE(p_info) == PFSAFE_INFO_MODE_SAFE   ) ? PFM_TRUE : PFM_FALSE)

/*------------------------------
   PFSAFE_INFO.api.sia_stat
 ------------------------------*/

/* sia_stat */
#define PFSAFE_API_STAT_API_EXECUTING       (0x01ul)
#define PFSAFE_API_STAT_DENT_UPDATING       (0x02ul)
#define PFSAFE_API_STAT_FAT_UPDATING        (0x04ul)
#define PFSAFE_API_STAT_FAT_FINALIZING      (0x08ul)

/* status operation macros */
#define PFSAFE_GET_API_STAT(p_info)         ((p_info)->api.sia_stat)
#define PFSAFE_SET_API_STAT(p_info,stat)    (((p_info)->api.sia_stat)|=(stat))
#define PFSAFE_CLEAR_API_STAT(p_info,stat)  (((p_info)->api.sia_stat)&=~(stat))

#define PFSAFE_START_API(p_info)            PFSAFE_SET_API_STAT(p_info,PFSAFE_API_STAT_API_EXECUTING)
#define PFSAFE_START_DENT_UPDATE(p_info)    PFSAFE_SET_API_STAT(p_info,PFSAFE_API_STAT_DENT_UPDATING)
#define PFSAFE_START_FAT_UPDATE(p_info)     PFSAFE_SET_API_STAT(p_info,PFSAFE_API_STAT_FAT_UPDATING)
#define PFSAFE_START_FAT_FINALIZE(p_info)   PFSAFE_SET_API_STAT(p_info,PFSAFE_API_STAT_FAT_FINALIZING)
#define PFSAFE_END_API(p_info)              PFSAFE_CLEAR_API_STAT(p_info,PFSAFE_API_STAT_API_EXECUTING)
#define PFSAFE_END_DENT_UPDATE(p_info)      PFSAFE_CLEAR_API_STAT(p_info,PFSAFE_API_STAT_DENT_UPDATING)
#define PFSAFE_END_FAT_UPDATE(p_info)       PFSAFE_CLEAR_API_STAT(p_info,PFSAFE_API_STAT_FAT_UPDATING)
#define PFSAFE_END_FAT_FINALIZE(p_info)     PFSAFE_CLEAR_API_STAT(p_info,PFSAFE_API_STAT_FAT_FINALIZING)

#define PFSAFE_IS_API_EXECUTING(p_info)     (((PFSAFE_GET_API_STAT(p_info) & PFSAFE_API_STAT_API_EXECUTING) == PFSAFE_API_STAT_API_EXECUTING) ? PFM_TRUE : PFM_FALSE)
#define PFSAFE_IS_DENT_UPDATING(p_info)     (((PFSAFE_GET_API_STAT(p_info) & PFSAFE_API_STAT_DENT_UPDATING) == PFSAFE_API_STAT_DENT_UPDATING) ? PFM_TRUE : PFM_FALSE)
#define PFSAFE_IS_FAT_UPDATING(p_info)      (((PFSAFE_GET_API_STAT(p_info) & PFSAFE_API_STAT_FAT_UPDATING) == PFSAFE_API_STAT_FAT_UPDATING) ? PFM_TRUE : PFM_FALSE)
#define PFSAFE_IS_FAT_FINALIZING(p_info)    (((PFSAFE_GET_API_STAT(p_info) & PFSAFE_API_STAT_FAT_FINALIZING) == PFSAFE_API_STAT_FAT_FINALIZING) ? PFM_TRUE : PFM_FALSE)

/*------------------------------
   PFSAFE_INFO.api
 ------------------------------*/
#define PFSAFE_API_APICODE(p_info)                      ((p_info)->api.sia_apicode)
#define PFSAFE_API_SIGNATURE(p_info)                    ((p_info)->api.sia_sign)

#define PFSAFE_API_SET_APICODE(p_info, code)            ((p_info)->api.sia_apicode = (code))
#define PFSAFE_API_SET_SIGNATURE(p_info, sign)          ((p_info)->api.sia_sign = (sign))

#define PFSAFE_API_MODULE_ERROR(p_info)                 ((p_info)->api.sia_moderrnum)
#define PFSAFE_API_SET_MODULE_ERROR(p_info,err)         ((p_info)->api.sia_moderrnum = (err))

#define PFSAFE_API_UBMP_STAT(p_info)                    ((p_info)->api.sia_ubmp_stat)
#define PFSAFE_API_UBMP_STAT_BUF(p_info)                ((void*)(p_info)->api.sia_ubmp_stat.us_addr)
#define PFSAFE_API_UBMP_STAT_SIZE(p_info)               ((p_info)->api.sia_ubmp_stat.us_size)
#define PFSAFE_API_UBMP_STAT_NUM_BIT(p_info)            ((p_info)->api.sia_ubmp_stat.us_num_bit)

#define PFSAFE_API_CURRENT_UBMP_OFFSET(p_info)          ((p_info)->api.sia_ubmp_offset)
#define PFSAFE_API_CURRENT_DENT_OFFSET(p_info)          ((p_info)->api.sia_dent_offset)

#define PFSAFE_API_SET_CURRENT_UBMP_OFFSET(p_info, sec) ((p_info)->api.sia_ubmp_offset = (sec))
#define PFSAFE_API_SET_CURRENT_DENT_OFFSET(p_info, sec) ((p_info)->api.sia_dent_offset = (sec))

/*------------------------------
   PFSAFE_INFO.buf
 ------------------------------*/
#define PFSAFE_BUF_GET_UBMP_BUF(p_info)             ((void*)(p_info)->buf.p_sib_ubmpaddr)
#define PFSAFE_BUF_RELEASE_UBMP_BUF(p_info)         do{;}while(0)

#define PFSAFE_BUF_UBMP_BUF_NUM_SECT(p_info)        ((p_info)->buf.sib_num_ubmpsect)

#define PFSAFE_BUF_GET_GENERIC_BUF(p_info)          ((void*)(p_info)->buf.p_sib_bufaddr)
#define PFSAFE_BUF_RELEASE_GENERIC_BUF(p_info)      do{;}while(0)

#define PFSAFE_BUF_GENERIC_BUF_NUM_SECT(p_info)     ((p_info)->buf.sib_num_bufsect)

/*------------------------------
   PFSAFE_INFO.cfg
 ------------------------------*/

#define PFSAFE_CFG_RECOVERY_FILE_NAME(p_info)          ((p_info)->cfg.sic_rcvf_name)
#define PFSAFE_CFG_CHECK_RECOVERY_FILE(p_info)         ((p_info)->cfg.sic_check_rcvf)
#define PFSAFE_CFG_META_DATA_SECTOR(p_info)            ((p_info)->cfg.sic_metd_sect)

#define PFSAFE_CFG_SET_CHECK_RECOVERY_FILE(p_info,val) ((p_info)->cfg.sic_check_rcvf = (val))
#define PFSAFE_CFG_SET_META_DATA_SECTOR(p_info,val)    ((p_info)->cfg.sic_metd_sect  = (val))

/*------------------------------
   PFSAFE_INFO.drv.si_bpb
 ------------------------------*/
#define PFSAFE_INFO_BPB(p_info)                     ((p_info)->drv.sid_bpb)

#define PFSAFE_BPB_BYTE_PER_SECT(p_info)            (PFSAFE_INFO_BPB(p_info).byte_per_sect)
#define PFSAFE_BPB_SECT_PER_CLST(p_info)            (PFSAFE_INFO_BPB(p_info).sect_per_clst)

#define PFSAFE_BPB_TOTAL_SECT(p_info)               (PFSAFE_INFO_BPB(p_info).total_sect)
#define PFSAFE_BPB_TOTAL_CLST(p_info)               (PFSAFE_INFO_BPB(p_info).total_clst)

#define PFSAFE_BPB_NUM_FAT_SECT(p_info)             (PFSAFE_INFO_BPB(p_info).sect_per_FAT)
#define PFSAFE_BPB_NUM_FATS(p_info)                 (PFSAFE_INFO_BPB(p_info).num_fats)
#define PFSAFE_BPB_FAT_TYPE(p_info)                 (PFSAFE_INFO_BPB(p_info).fat_type)

#define PFSAFE_BPB_FAT1_START_SECT(p_info)          (PFSAFE_INFO_BPB(p_info).num_reserved_sect)
#define PFSAFE_BPB_FAT2_START_SECT(p_info)          (PFSAFE_BPB_FAT1_START_SECT(p_info) + PFSAFE_BPB_NUM_FAT_SECT(p_info))
#define PFSAFE_BPB_DATA_START_SECT(p_info)          (PFSAFE_INFO_BPB(p_info).first_data_sect)
#define PFSAFE_BPB_ROOT_DIR_CLST(p_info)            (PFSAFE_INFO_BPB(p_info).root_dir_clst)
#define PFSAFE_BPB_UPPER_BOUND_CLST(p_info)         (PFSAFE_BPB_TOTAL_CLST(p_info) + 2)

#define PFSAFE_BYTES_PER_CLUSTER(p_info)            ((PFSAFE_INFO_BPB(p_info).sect_per_clst) * PFSAFE_BPB_BYTE_PER_SECT(p_info))

#define PFSAFE_BYTES_TO_SECTORS(p_info,bytes)       (((bytes) / PFSAFE_INFO_BPB(p_info).byte_per_sect)  \
                                                     + ((((bytes) % PFSAFE_INFO_BPB(p_info).byte_per_sect) == 0) ? 0 : 1))
#define PFSAFE_SECTORS_TO_BYTES(p_info,sects)       ((sects) * PFSAFE_INFO_BPB(p_info).byte_per_sect)
#define PFSAFE_CLUSTERS_TO_BYTES(p_info,clsts)      ((clsts) * PFSAFE_BYTES_PER_CLUSTER(p_info))

#define PFSAFE_SECTORS_TO_CLUSTERS(p_info,sects)    (((sects) / PFSAFE_INFO_BPB(p_info).sect_per_clst)  \
                                                     + ((((sects) % PFSAFE_INFO_BPB(p_info).sect_per_clst) == 0) ? 0 : 1))
#define PFSAFE_SECTOR_TO_CLUSTER(p_info,sect)       ((((sect) - PFSAFE_INFO_BPB(p_info).first_data_sect) / PFSAFE_INFO_BPB(p_info).sect_per_clst) + 2)

#define PFSAFE_CLUSTERS_TO_SECTORS(p_info,clsts)    ((clsts) * PFSAFE_INFO_BPB(p_info).sect_per_clst)
#define PFSAFE_CLUSTER_TO_SECTOR(p_info,clst)       (PFSAFE_INFO_BPB(p_info).first_data_sect + PFSAFE_CLUSTERS_TO_SECTORS(p_info,clst-2))

/*------------------------------
   PFSAFE_INFO.si_rcvd_sect
 ------------------------------*/
#define PFSAFE_GET_RECOVERY_DATA_STATUS(p_info)             ((p_info)->rcv.sir_rcvd_stat)
#define PFSAFE_GET_RECOVERY_DATA_START_SECT(p_info)         ((p_info)->rcv.sir_rcvd_sect)
#define PFSAFE_GET_RECOVERY_DATA_SIGNATURE(p_info)          ((p_info)->rcv.sir_rcvd_sign)
#define PFSAFE_GET_RECOVERY_DATA_API_CODE(p_info)           ((p_info)->rcv.sir_rcvd_api)

#define PFSAFE_SET_RECOVERY_DATA_STATUS(p_info, stat)       ((p_info)->rcv.sir_rcvd_stat = (stat))
#define PFSAFE_SET_RECOVERY_DATA_START_SECT(p_info, sect)   ((p_info)->rcv.sir_rcvd_sect = (sect))
#define PFSAFE_SET_RECOVERY_DATA_SIGNATURE(p_info, sign)    ((p_info)->rcv.sir_rcvd_sign = (sign))
#define PFSAFE_SET_RECOVERY_DATA_API_CODE(p_info, apicode)  ((p_info)->rcv.sir_rcvd_api  = (apicode))

#define PFSAFE_RCV_RECOVERY_DATA_BACKUP_SECT(p_info)        (PFSAFE_GET_RECOVERY_DATA_START_SECT(p_info) + 1uL)
#define PFSAFE_RCV_DENT_BACKUP_START_SECT(p_info)           (PFSAFE_GET_RECOVERY_DATA_START_SECT(p_info) + 2uL)
#define PFSAFE_RCV_UPDATE_BITMAP_START_SECT(p_info)         (PFSAFE_GET_RECOVERY_DATA_START_SECT(p_info) + 20uL)


/****************************************************************************
  Prototypes for user
****************************************************************************/
/* Transaction Safe Module function table */
extern PFM_CONST PFM_SAFE_IF    pfsafe_func_tbl;

/* initialize PFSAFE_INFO */
int  pfsafe_init_safeinfo(PFSAFE_INFO           *p_si_info,
                          pfsafe_callback_malloc si_malloc,
                          pfsafe_callback_free   si_free);


/****************************************************************************
  module error code
****************************************************************************/
/* status code */
#define PFSAFE_ERR_OK                    (0)

#define PFSAFE_ERR_IMPOSSIBLE            (-1)
#define PFSAFE_ERR_UNEXPECTED            (-2)
#define PFSAFE_ERR_INFO_NOT_INIT         (-3)
#define PFSAFE_ERR_INFO_OVERLAP          (-4)
#define PFSAFE_ERR_INFO_FULL             (-5)
#define PFSAFE_ERR_INFO_NOT_REGISTERED   (-6)

#define PFSAFE_ERR_INVALID_PARAM         (-7)
#define PFSAFE_ERR_INVALID_FILENAME      (-8)
#define PFSAFE_ERR_MALLOC                (-9)
#define PFSAFE_ERR_FREE                  (-10)

#define PFSAFE_ERR_UNSUPPORTED_REQ       (-11)
#define PFSAFE_ERR_UNSUPPORTED_FAT_TYPE  (-12)

#define PFSAFE_ERR_RECOVERY_REQUIRED     (-13)
#define PFSAFE_ERR_VOLUME_FULL           (-14)
#define PFSAFE_ERR_NOT_AVAILABLE         (-15)
#define PFSAFE_ERR_FILE_OPENED           (-16)

#define PFSAFE_ERR_FAT_GETBPB            (-20)
#define PFSAFE_ERR_FAT_READSECT          (-21)
#define PFSAFE_ERR_FAT_WRITESECT         (-22)
#define PFSAFE_ERR_FAT_GETERRNUM         (-23)
#define PFSAFE_ERR_FAT_SETDRVCONF        (-24)
#define PFSAFE_ERR_FAT_GETDRVCONF        (-25)
#define PFSAFE_ERR_FAT_PFAPI             (-26)
#define PFSAFE_ERR_FAT_GETCLST           (-27)
/* function code setter */
#define PFSAFE_SET_ERR_BY(err,err_by)    ((0<=(err))?((err)+=(err_by)):((err)-=(err_by)))

/* function code */
#define PFSAFE_ERR_BY_INIT               (0)
#define PFSAFE_ERR_BY_FINALIZE           (0)
#define PFSAFE_ERR_BY_MOUNT              (0)
#define PFSAFE_ERR_BY_UNMOUNT            (0)
#define PFSAFE_ERR_BY_CONTROL            (0)
#define PFSAFE_ERR_BY_API_START          (0)
#define PFSAFE_ERR_BY_API_END            (0)
#define PFSAFE_ERR_BY_FAT_UPDATE_START   (0)
#define PFSAFE_ERR_BY_FAT_CACHE_READ     (0)
#define PFSAFE_ERR_BY_FAT_CACHE_WRITE    (0)
#define PFSAFE_ERR_BY_FAT_CACHE_MODIFY   (0)
#define PFSAFE_ERR_BY_FAT_UPDATE_END     (0)
#define PFSAFE_ERR_BY_DENT_UPDATE_START  (0)
#define PFSAFE_ERR_BY_DENT_UPDATE        (0)
#define PFSAFE_ERR_BY_DENT_UPDATE_END    (0)

#define PFSAFE_IS_DRIVER_ERROR(err)      (((err)==PFSAFE_ERR_FAT_READSECT)    \
                                          ||((err)==PFSAFE_ERR_FAT_WRITESECT) \
                                          ||((err)==PFSAFE_ERR_FAT_PFAPI))

#endif  /* __PF_FCSM_BITMAP_H__ */
