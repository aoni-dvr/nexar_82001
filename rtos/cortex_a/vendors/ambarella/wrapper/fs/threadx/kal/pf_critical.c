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
[pf_critical.c] - Critical-section control routines.

 NOTES:
  - This file is sample implementation of 'pf_critical.c'.
    USER MUST MODIFY THIS FILE FOR USER'S ENVIRONMENT.

****************************************************************************/
#include "prfile2/pf_config.h"

#if PF_CRITICAL_SECTION_ENABLE

#include "AmbaTypes.h"
#include "AmbaFS_KAL.h"

#include "pf_critical_config.h"
#define AMBA_FS_H
#include "pf_critical.h"

#define AMBA_MAX_PF_SEM_CNT (10U)

/*---------------------------------------------------------------------------
 PF_initCritical - Initialization for critical-section control routines.

 [Synopsis]
    void  PF_initCritical(PF_CRITICAL* pIdCrit);
        pIdCrit      PrFILE2 critical-section table structure.

 [Description]
    Create semaphores for each volume labels and volume label set.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void PF_initCritical(PF_CRITICAL* pIdCrit)
{
    static PFK_CSEM    pfk_csem[AMBA_MAX_PF_SEM_CNT];
    static UINT32      SemCount = 0;

    if (SemCount < AMBA_MAX_PF_SEM_CNT) {
        if (pIdCrit->state == (UINT32)PF_CRITICAL_NOINIT) {
            /* create semaphores */
            pfk_csem[SemCount].InitCount  = 1;
            pIdCrit->Resource = (PF_S_LONG)pfk_create_semaphore(&pfk_csem[SemCount]);
            SemCount ++;

            /* initialize lock count */
            pIdCrit->LockCount = 0;
            /* initialize owner context ID */
            pIdCrit->owner = 0;

            /* Clear all status and mark initialized flag */
            pIdCrit->state = PF_CRITICAL_INIT;
        }
    }
}

/*---------------------------------------------------------------------------
 PF_deleteCritical - Finalization for critical-section control routines.

 [Synopsis]
    void  PF_deleteCritical(PF_CRITICAL* pIdCrit);
        pIdCrit      PrFILE2 critical-section table structure.

 [Description]
    Delete semaphores for each volume labels and volume label set.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void PF_deleteCritical(PF_CRITICAL* pIdCrit)
{
    if ((pIdCrit->state & (UINT32)PF_CRITICAL_INIT) != 0U) {
        /* delete semaphores */
        (void) pfk_delete_semaphore(pIdCrit->Resource);

        /* Clear all status */
        pIdCrit->state = PF_CRITICAL_NOINIT;
    }
}

/*---------------------------------------------------------------------------
 PF_enterCritical - Enter critical-section.

 [Synopsis]
    void  PF_enterCritical(PF_CRITICAL* pIdCrit);
        pIdCrit      PrFILE2 critical-section table structure.

 [Description]
    get semaphores for a volume label or volume label set.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void PF_enterCritical(PF_CRITICAL* pIdCrit)
{
    PF_S_LONG   taskid;

    if (pIdCrit->LockCount == 0) {
        (void) pfk_get_semaphore((PF_S_LONG)pIdCrit->Resource);
        (void) pfk_get_task_id(&pIdCrit->owner);
    } else {
        (void) pfk_get_task_id(&taskid);
        if (pIdCrit->owner != taskid) {
            (void) pfk_get_semaphore((PF_S_LONG)pIdCrit->Resource);
            pIdCrit->owner = taskid;
        }
    }
    pIdCrit->LockCount++;
}

/*---------------------------------------------------------------------------
 PF_exitCritical - Enter critical-section.

 [Synopsis]
    void  PF_exitCritical(PF_CRITICAL* pIdCrit);
        pIdCrit      PrFILE2 critical-section table structure.

 [Description]
    release semaphores for a volume label or volume label set.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void PF_exitCritical(PF_CRITICAL* pIdCrit)
{
    pIdCrit->LockCount--;
    if (pIdCrit->LockCount == 0) {
        pIdCrit->owner = 0;
        (void) pfk_release_semaphore((PF_S_LONG)pIdCrit->Resource);
    }
}

#endif  /* PF_CRITICAL_SECTION_ENABLE  */
