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
 [pdm_critical.h] - 
 ****************************************************************************/
#ifndef     __PDM_CRITICAL_H__
#define     __PDM_CRITICAL_H__

#include "prfile2/pdm_config.h"

#if PDM_CRITICAL_SECTION_ENABLE

#include "prfile2/pdm_types.h"

/*---------------------------------------------------------------------------
  Structure to control the critical section
 ----------------------------------------------------------------------------*/
/* You should modify the typedef of this structure (PF_CRITICAL) */
typedef struct PDM_CRITICAL
{
    PDM_BOOL    is_init;           /* Status                        */
    PDM_S_LONG  lockcount;         /* Lock count                    */
    PDM_S_LONG  resource;          /* Resource for critical section */
    PDM_S_LONG  owner;             /* Owner context ID              */
} PDM_CRITICAL;

/*---------------------------------------------------------------------------
  Prototypes
 ----------------------------------------------------------------------------*/
void      pdm_init_critical(PDM_CRITICAL *critical);
void      pdm_finalize_critical(PDM_CRITICAL *critical);
PDM_ERROR pdm_enter_critical(PDM_CRITICAL *critical);
PDM_ERROR pdm_exit_critical(PDM_CRITICAL *critical);

/* You must not modify these macro name:    */
/*      PDM_INIT_CRITICAL(id)               */
/*      PDM_FINALIZE_CRITICAL(id)           */
/*      PDM_ENTER_DISK_CRITICAL(p_ldisk)     */
/*      PDM_EXIT_DISK_CRITICAL(p_ldisk)      */
/*      PDM_ENTER_DISK_SET_CRITICAL()       */
/*      PDM_EXIT_DISK_SET_CRITICAL()        */

#define PDM_INIT_DISK_CRITICAL(p_ldisk)     pdm_init_critical    (&((p_ldisk)->critical))
#define PDM_FINALIZE_DISK_CRITICAL(p_ldisk) pdm_finalize_critical(&((p_ldisk)->critical))
#define PDM_ENTER_DISK_CRITICAL(p_ldisk)    pdm_enter_critical   (&((p_ldisk)->critical));{{
#define PDM_EXIT_DISK_CRITICAL(p_ldisk)     }}pdm_exit_critical  (&((p_ldisk)->critical))

#define PDM_INIT_DISK_SET_CRITICAL()        pdm_init_critical    (&(pdm_disk_set.critical))
#define PDM_FINALIZE_DISK_SET_CRITICAL()    pdm_finalize_critical(&(pdm_disk_set.critical))
#define PDM_ENTER_DISK_SET_CRITICAL()       pdm_enter_critical   (&(pdm_disk_set.critical));{
#define PDM_EXIT_DISK_SET_CRITICAL()        }pdm_exit_critical   (&(pdm_disk_set.critical))

#else /* PDM_CRITICAL_SECTION_ENABLE */

/* You must not modify these macro name:    */
/*      PDM_INIT_CRITICAL(id)               */
/*      PDM_FINALIZE_CRITICAL(id)           */
/*      PDM_ENTER_DISK_CRITICAL(p_ldisk)     */
/*      PDM_EXIT_DISK_CRITICAL(p_ldisk)      */
/*      PDM_ENTER_DISK_SET_CRITICAL()       */
/*      PDM_EXIT_DISK_SET_CRITICAL()        */

#define PDM_INIT_DISK_CRITICAL(p_ldisk)
#define PDM_FINALIZE_DISK_CRITICAL(p_ldisk)
#define PDM_ENTER_DISK_CRITICAL(p_ldisk)
#define PDM_EXIT_DISK_CRITICAL(p_ldisk)

#define PDM_INIT_DISK_SET_CRITICAL()
#define PDM_FINALIZE_DISK_SET_CRITICAL()
#define PDM_ENTER_DISK_SET_CRITICAL()
#define PDM_EXIT_DISK_SET_CRITICAL()

#endif  /* PDM_CRITICAL_SECTION_ENABLE  */

#endif  /* __PDM_CRITICAL_H__ */
