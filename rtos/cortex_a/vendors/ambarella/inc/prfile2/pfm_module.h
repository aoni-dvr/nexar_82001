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
 [pfm_module.h] - Definitions and Declarations commonly used in PrFILE2 Module.
 ****************************************************************************/
#ifndef __PFM_MODULE_H__
#define __PFM_MODULE_H__

#include "prfile2/pf_apicmn_def.h"
#include "prfile2/pfm_api.h"

/*---------------------------------------------------------------------------
  Definitions
 ----------------------------------------------------------------------------*/
/* The error code */
#define PFM_ERR_OK_RESERVE      (1)     /* reserved for pf_ctrl_module */
#define PFM_ERR_OK              (0)     /* Success */
#define PFM_ERR_NG              (-1)    /* Error */

#if PF_FREE_CLUSTER_SEARCH_SUPPORT
/****************************************************************************
  Free Cluster Search
 ****************************************************************************/
/* The mount attribute */
#define PFM_FCS_MOUNTED         (0x00000001)    /* mounted */
#define PFM_FCS_NOTSUPPORTED    (0x00000002)    /* not mounted because the media isn't supported */


/* The free cluster search descriptor */
struct PFM_FCSP_tag
{
    /* The FAT descriptor */
    PFM_FATD    fatd;

    /* The parameter which is set to pf_regfclst() */
    void        *p_param;
};
typedef struct PFM_FCSP_tag  PFM_FCSP;


/* Cluster group */
struct PFM_FCS_CGROUP_tag
{
    PFM_U_LONG  clst_per_group;
    PFM_U_LONG  start_clst;
};
typedef struct PFM_FCS_CGROUP_tag   PFM_FCS_CGROUP;


/* The optional request */
struct PFM_FCS_REQOPT_tag
{
    /* Search type
       - PFM_FCS_REQOPT_STYPE_xxx
     */
    PFM_U_LONG      search_type;

    /* Search start cluster number and limit number.
       - The FCS module must search the free cluster between 'area_start'
         and 'area_end' ('area_end' is included in the area).
     */
    PFM_U_LONG      area_start;
    PFM_U_LONG      area_end;

    /* Cluster group information
       - Minimum allocated unit
       - The module must search the free cluster of the specified unit.
       - This menber is available only when 'search_type' is PFM_FCS_REQOPT_STYPE_GROUP_xxx.
       - When this member is invalid, 'clst_group.clst_per_group' is 0.
     */
    PFM_FCS_CGROUP  clst_group;
};
typedef struct PFM_FCS_REQOPT_tag PFM_FCS_REQOPT;

#define PFM_FCS_REQOPT_STYPE_FIRST          (1)
#define PFM_FCS_REQOPT_STYPE_BEST           (2)
#define PFM_FCS_REQOPT_STYPE_GROUP_FIRST    (3)
#define PFM_FCS_REQOPT_STYPE_GROUP_BEST     (4)


/* The inferface of Free Cluster Search Module */
struct PFM_FCS_IF_tag
{
    /* Initialize the internal structures */
    PFM_ERROR   (*init) (PFM_FCSP *p_fcs);

    /* Release the internal structures */
    PFM_ERROR   (*finalize) (PFM_FCSP *p_fcs);

    /* Make the free clusters information */
    PFM_ERROR   (*mount) (PFM_FCSP   *p_fcs,
                          PFM_BOOL   is_formatted,
                          PFM_U_LONG *p_attr);

    /* Release the free clusters information */
    PFM_ERROR   (*unmount) (PFM_FCSP *p_fcs);

    /* Media eject happened */
    void        (*media_eject) (PFM_FCSP *p_fcs);

    /* Initialize the free clusters information */
    PFM_ERROR   (*format) (PFM_FCSP *p_fcs);

    /* Return the free clusters number */
    PFM_ERROR   (*notify_free_clst) (PFM_FCSP       *p_fcs,
                                     PFM_U_LONG     req_cnt,
                                     PFM_FCS_REQOPT *p_req_opt,
                                     PFM_U_LONG     *p_area_start,
                                     PFM_U_LONG     *p_area_num);

    /* Set the free clusters information to 'allocated' */
    PFM_ERROR   (*alloc_clst) (PFM_FCSP   *p_fcs,
                               PFM_U_LONG area_start,
                               PFM_U_LONG area_num);

    /* Set the free clusters information to 'free' */
    PFM_ERROR   (*release_clst) (PFM_FCSP   *p_fcs,
                                 PFM_U_LONG area_start,
                                 PFM_U_LONG area_num);

    /* The extension function */
    PFM_S_LONG  (*control) (PFM_FCSP   *p_fcs,
                            PFM_U_LONG req_code,
                            PFM_U_LONG *p_param,
                            PFM_BOOL   is_blocking);
};
typedef struct PFM_FCS_IF_tag PFM_FCS_IF;

#endif /* PF_FREE_CLUSTER_SEARCH_SUPPORT */

#if PF_TRANSACTION_SAFE_SUPPORT
/****************************************************************************
  Transaction Safe
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Definitions
 ----------------------------------------------------------------------------*/
/* The Safe descriptor */
typedef struct
{
    /* The FAT descriptor */
    PFM_FATD    fatd;

    /* The parameter which is set to pf_regfclst() */
    void        *p_param;
} PFM_SAFEP;

/* The parameter of 'init' of Safe Module */
typedef struct
{
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_INIT;

/* The parameter of 'finalize' of Safe Module */
typedef struct
{
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_FINALIZE;

/* The parameter of 'mount' of Safe Module */
typedef struct
{
    PFM_API_CODE    api_code;       /* [I] API number */
    PFM_U_LONG      attr;           /* [I] The mount attribute */
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_MOUNT;

/* The mount attribute */
#define PFM_SAFE_MOUNTED        (0x00000001)    /* mounted */
#define PFM_SAFE_NOTSUPPORTED   (0x00000002)    /* not mounted because the media isn't supported */

/* The parameter of 'unmount' of Safe Module */
typedef struct
{
    PFM_API_CODE    api_code;       /* [I] API number */
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_UNMOUNT;

/* The parameter of 'api_start' of Safe Module */
typedef struct
{
    PFM_API_CODE    api_code;       /* [I] API number */
    PF_CONST PF_API *p_api;         /* [I] API information */
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_API_START;

/* The parameter of 'api_end' of Safe Module */
typedef struct
{
    PFM_API_CODE    api_code;       /* [I] API number */
    PFM_ERROR       inerrnum;       /* [I] Internal errnum of the API running */
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_API_END;

/* The parameter of 'fat_update_start' of Safe Module */
typedef struct
{
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_FAT_START;

/* The parameter of 'fat_update_end' of Safe Module */
typedef struct
{
    PFM_ERROR       inerrnum;       /* [I] Internal errnum of the API running */
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_FAT_END;

/* The parameter of 'fat_cache_read' of Safe Module */
typedef struct
{
    PFM_SECT        sect;           /* [I] PrFILE2 was going to read this sector */
    PFM_SECT        num_sect;       /* [I] PrFILE2 was going to read this sector */
    PFM_SECT        read_sect;      /* [O] Actually PrFILE2 will read this sector */
    PFM_SECT        num_read_sect;  /* [O] Actually PrFILE2 will read this sector */
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_FAT_CACHE_READ;

/* The parameter of 'fat_cache_write' of Safe Module */
typedef struct
{
    PFM_SECT        sect;           /* [I] PrFILE2 was going to write this sector */
    PFM_SECT        num_sect;       /* [I] PrFILE2 was going to write this sector */
    PFM_SECT        write_sect;     /* [O] Actually PrFILE2 will write this sector */
    PFM_SECT        num_write_sect; /* [O] Actually PrFILE2 will write this sector */
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_FAT_CACHE_WRITE;

/* The parameter of 'fat_cache_modify' of Safe Module */
typedef struct
{
    PFM_SECT        cache_start_sect;   /* [I] The PrFILE2 cache manages this sector */
    PFM_SECT        cache_num_sect;     /* [I] The PrFILE2 cache manages this sector */
    PFM_SECT        sect;               /* [I] Now, this sector is dirty */
    PFM_SECT        num_sect;           /* [I] Now, this sector is dirty */
    PFM_SECT        flush_sect;         /* [O] Actually this sector is dirty (in the cache) */
    PFM_SECT        num_flush_sect;     /* [O] Actually this sector is dirty (in the cache) */
    PF_ERROR        alt_err;            /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_FAT_CACHE_MODIFY;

/* The parameter of 'dent_update_start' of Safe Module */
typedef struct
{
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_DENT_START;

/* The parameter of 'dent_update_end' of Safe Module */
typedef struct
{
    PFM_ERROR       inerrnum;       /* [I] Internal errnum of the API running */
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_DENT_END;

/* The parameter of 'dent_update' of Safe Module */
typedef struct
{
    PFM_SECT        sect;           /* [I] The sectors are going to be updated */
    PFM_SECT        num_sect;       /* [I] The sectors are going to be updated */
    PF_ERROR        alt_err;        /* [O] Alternative PrFILE2 internal errnum */
} PFM_SIP_DENT_UPDATE;

/* The interface of Safe Module */
typedef struct
{
    /* Initialize the internal structures. */
    PFM_ERROR   (*init) (PFM_CONST PFM_SAFEP *p_safe,
                         PFM_SIP_INIT        *p_sip);

    /* Release the internal structures. */
    PFM_ERROR   (*finalize) (PFM_CONST PFM_SAFEP *p_safe,
                             PFM_SIP_FINALIZE    *p_sip);

    /* PrFILE2 notify a media did be mounted. */
    PFM_ERROR   (*mount) (PFM_CONST PFM_SAFEP *p_safe,
                          PFM_SIP_MOUNT       *p_sip);

    /* PrFILE2 notify a media will be unmounted. */
    PFM_ERROR   (*unmount) (PFM_CONST PFM_SAFEP *p_safe,
                            PFM_SIP_UNMOUNT     *p_sip);

    /* PrFILE2 notify an API did be started. */
    PFM_ERROR   (*api_start) (PFM_CONST PFM_SAFEP *p_safe,
                              PFM_SIP_API_START   *p_sip);

    /* PrFILE2 notify an API will be ended. */
    PFM_ERROR   (*api_end) (PFM_CONST PFM_SAFEP *p_safe,
                            PFM_SIP_API_END     *p_sip);

    /* PrFILE2 notify FAT entries will be updated. */
    PFM_ERROR   (*fat_update_start) (PFM_CONST PFM_SAFEP *p_safe,
                                     PFM_SIP_FAT_START   *p_sip);

    /* PrFILE2 notify FAT entries did be updated. */
    PFM_ERROR   (*fat_update_end) (PFM_CONST PFM_SAFEP *p_safe,
                                   PFM_SIP_FAT_END     *p_sip);

    /* PrFILE2 notify a FAT cache will be read. */
    PFM_ERROR   (*fat_cache_read) (PFM_CONST PFM_SAFEP    *p_safe,
                                   PFM_SIP_FAT_CACHE_READ *p_sip);

    /* PrFILE2 notify a FAT cache will be write. */
    PFM_ERROR   (*fat_cache_write) (PFM_CONST PFM_SAFEP     *p_safe,
                                    PFM_SIP_FAT_CACHE_WRITE *p_sip);

    /* PrFILE2 notify a FAT cache will be write. */
    PFM_ERROR   (*fat_cache_modify) (PFM_CONST PFM_SAFEP      *p_safe,
                                     PFM_SIP_FAT_CACHE_MODIFY *p_sip);

    /* PrFILE2 notify directory entries will be updated. */
    PFM_ERROR   (*dent_update_start) (PFM_CONST PFM_SAFEP *p_safe,
                                      PFM_SIP_DENT_START  *p_sip);

    /* PrFILE2 notify directory entries did be updated. */
    PFM_ERROR   (*dent_update_end) (PFM_CONST PFM_SAFEP *p_safe,
                                    PFM_SIP_DENT_END    *p_sip);

    /* PrFILE2 notify a directory entry will be updated. */
    PFM_ERROR   (*dent_update) (PFM_CONST PFM_SAFEP *p_safe,
                                PFM_SIP_DENT_UPDATE *p_sip);

    /* The extension function */
    PFM_S_LONG  (*control) (PFM_CONST PFM_SAFEP *p_safe,
                            PFM_U_LONG          req_code,
                            PFM_U_LONG          *p_param,
                            PFM_BOOL            is_blocking);
} PFM_SAFE_IF;

#endif /* PF_TRANSACTION_SAFE_SUPPORT */

#endif /* __PFM_MODULE_H__ */
