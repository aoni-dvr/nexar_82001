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
 [pf_system.h] - System-dependent functions declarations

  NOTES:
   - All of functions declared in this file are system-dependent functions
     which PrFILE2's user must implement.

 ****************************************************************************/
#ifndef __PF_SYSTEM_H__
#define __PF_SYSTEM_H__

#include "prfile2/pf_apicmn.h"
#include "prfile2/pf_sysif.h"
#include "pf_stub.h"

/****************************************************************************
 Macro definition
 ****************************************************************************/
#define PF_TASKID_VOID      (0)
#define PF_CONTEXTID_VOID   PF_TASKID_VOID

/****************************************************************************
 Typedef definitions
 ****************************************************************************/
typedef struct
{
    /* Total file locked count (not include multiple lock) */
    PF_U_LONG       flock_count;

#if PF_USE_API_STUB
    /* Informations for API stub */
    PF_STUB         stub;
#endif /* PF_USE_API_STUB */

} PF_SYSTEM_SET;

extern PF_SYSTEM_SET    pf_sys_set;

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFSYS_initializeSYS - Initialize the system
 ----------------------------------------------------------------------------*/
void PFSYS_initializeSYS(void);

/*---------------------------------------------------------------------------
 PFSYS_GetCurrentContextID - Get current context ID
 ----------------------------------------------------------------------------*/
PF_ERROR PFSYS_GetCurrentContextID(PF_S_LONG *p_context_id);

#endif  /* __PF_SYSTEM_H__ */
