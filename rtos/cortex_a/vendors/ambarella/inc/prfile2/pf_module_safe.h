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
 [pf_module_safe.h] - Safe Module
 ****************************************************************************/
#ifndef __PF_MODULE_SAFE_H__
#define __PF_MODULE_SAFE_H__

#include "prfile2/pfm_module.h"
#include "pf_common.h"
#include "pf_volume.h"

#if PF_TRANSACTION_SAFE_SUPPORT
/****************************************************************************
  FUNCTIONS PROTOTYPE
 ****************************************************************************/
PF_ERROR  PFMOD_Safe_Initialize(PF_VOLUME*, PFM_SAFE_IF*, void*);
PF_ERROR  PFMOD_Safe_Finalize(PF_VOLUME*);
PF_ERROR  PFMOD_Safe_Mount(PF_VOLUME*, PF_API_CODE);
PF_ERROR  PFMOD_Safe_Unmount(PF_VOLUME*, PF_API_CODE);
PF_ERROR  PFMOD_Safe_ApiStart(PF_VOLUME*, PF_API_CODE, PFM_CONST PF_API*);
PF_ERROR  PFMOD_Safe_ApiEnd(PF_VOLUME*, PF_API_CODE, PF_ERROR);
PF_ERROR  PFMOD_Safe_FatUpdateStart(PF_VOLUME*);
PF_ERROR  PFMOD_Safe_FatUpdateEnd(PF_VOLUME*, PF_ERROR);
PF_ERROR  PFMOD_Safe_FatCacheRead(PF_VOLUME*, PF_SECT, PF_SECT, PF_SECT*, PF_SECT*);
PF_ERROR  PFMOD_Safe_FatCacheWrite(PF_VOLUME*, PF_SECT, PF_SECT, PF_SECT*, PF_SECT*);
PF_ERROR  PFMOD_Safe_FatCacheModify(PF_VOLUME*, PF_SECT, PF_SECT, PF_SECT, PF_SECT, PF_SECT*, PF_SECT*);
PF_ERROR  PFMOD_Safe_DentUpdateStart(PF_VOLUME*);
PF_ERROR  PFMOD_Safe_DentUpdateEnd(PF_VOLUME*, PF_ERROR);
PF_ERROR  PFMOD_Safe_DentUpdate(PF_VOLUME*, PF_SECT, PF_SECT);
PF_ERROR  PFMOD_Safe_Control(PF_VOLUME*, PF_U_LONG, void*, PF_BOOL);

/****************************************************************************
  MACROS
 ****************************************************************************/
/* fatd */
#define SAFEM_FATD(p_vol)                   ((p_vol)->safed.fatd)

/* status */
#define SAFE_STAT_UNREGISTERED              (0x00000000ul)
#define SAFE_STAT_REGISTERED                (0x00000001ul)
#define SAFE_STAT_MOUNTED                   (0x00000002ul)
#define SAFE_STAT_APISTART_MULTICALL        (0x00000004ul)

#define SAFEM_SET_UNREGISTERED(p_vol)       (((p_vol)->safe_state) = SAFE_STAT_UNREGISTERED)
#define SAFEM_SET_REGISTERED(p_vol)         (((p_vol)->safe_state) = SAFE_STAT_REGISTERED)
#define SAFEM_SET_MOUNTED(p_vol)            (((p_vol)->safe_state) = (SAFE_STAT_REGISTERED | SAFE_STAT_MOUNTED))

#define SAFEM_IS_UNREGISTERED(p_vol)        (((p_vol)->safe_state) == SAFE_STAT_UNREGISTERED)
#define SAFEM_IS_REGISTERED(p_vol)          ((((p_vol)->safe_state) & SAFE_STAT_REGISTERED) == SAFE_STAT_REGISTERED)
#define SAFEM_IS_MOUNTED(p_vol)             ((((p_vol)->safe_state) & SAFE_STAT_MOUNTED) == SAFE_STAT_MOUNTED)

#define SAFEM_SET_APISTART_MULTICALL(p_vol)     (((p_vol)->safe_state) |= SAFE_STAT_APISTART_MULTICALL)
#define SAFEM_CLEAR_APISTART_MULTICALL(p_vol)   (((p_vol)->safe_state) &= ~SAFE_STAT_APISTART_MULTICALL)
#define SAFEM_IS_APISTART_MULTICALL(p_vol)      ((((p_vol)->safe_state) & SAFE_STAT_APISTART_MULTICALL) == SAFE_STAT_APISTART_MULTICALL)

#define SAFEM_APICODE(p_vol)                ((p_vol)->safe_api_code)
#define SAFEM_SET_APICODE(p_vol,code)       (((p_vol)->safe_api_code) = (code))

#define SAFEM_DENT_CNT(p_vol)               ((p_vol)->safe_dent_cnt)
#define SAFEM_SET_DENT_CNT(p_vol,n)         (((p_vol)->safe_dent_cnt) = (n))

/* last error */
#define SAFEM_LAST_ERROR(p_vol)             (CURRENT_CONTEXT(p_vol)->last_safe_error[DRV_CHAR_TO_VOL_IDX((p_vol)->drv_char)])
#define SAFEM_SET_VOLUME_ERROR(p_vol,merr)  (CURRENT_CONTEXT(p_vol)->last_safe_error[DRV_CHAR_TO_VOL_IDX((p_vol)->drv_char)] = merr)

/* for API */
#define PFSAFE_API_START(pv,api,pa)         PFMOD_Safe_ApiStart((pv),(api),(pa))
#define PFSAFE_API_END(pv,api,e)            PFMOD_Safe_ApiEnd((pv),(api),(e))
#define PFSAFE_FAT_START(pv)                PFMOD_Safe_FatUpdateStart((pv))
#define PFSAFE_FAT_END(pv,e)                PFMOD_Safe_FatUpdateEnd((pv),(e))
#define PFSAFE_DENT_START(pv)               PFMOD_Safe_DentUpdateStart((pv))
#define PFSAFE_DENT_END(pv,e)               PFMOD_Safe_DentUpdateEnd((pv),(e))
#define PFSAFE_DENT_UPDATE(pv,s,n)          PFMOD_Safe_DentUpdate((pv),(s),(n))

#else /* PF_TRANSACTION_SAFE_SUPPORT */
/****************************************************************************
  MACROS DUMMY
 ****************************************************************************/
/* status */
#define SAFEM_IS_UNREGISTERED(p_vol)        (0)
#define SAFEM_IS_REGISTERED(p_vol)          (0)
#define SAFEM_IS_MOUNTED(p_vol)             (0)

#define SAFEM_APICODE(p_vol)                (PF_APICODE_NONE)
#define SAFEM_SET_APICODE(p_vol,code)

#define SAFEM_DENT_CNT(p_vol)               (0)
#define SAFEM_SET_DENT_CNT(p_vol,n)

/* last error */
#define SAFEM_LAST_ERROR(p_vol)             (0)
#define SAFEM_SET_VOLUME_ERROR(p_vol,merr)

/* for API */
#define PFSAFE_API_START(pv,api,pa)         (OK)
#define PFSAFE_API_END(pv,api,e)            (OK)
#define PFSAFE_FAT_START(pv)                (OK)
#define PFSAFE_FAT_END(pv,e)                (OK)
#define PFSAFE_DENT_START(pv)               (OK)
#define PFSAFE_DENT_END(pv,e)               (OK)
#define PFSAFE_DENT_UPDATE(pv,s,n)          (OK)

#endif /* PF_TRANSACTION_SAFE_SUPPORT */

#endif /* __PF_MODULE_SAFE_H__ */

