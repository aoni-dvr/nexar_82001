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
 [pf_critical_config.h] -
 ****************************************************************************/
#ifndef     PF_CRITICAL_CONFIG_H
#define     PF_CRITICAL_CONFIG_H

#ifndef PF_CONFIG_H
#include "prfile2/pf_config.h"
#endif

#if PF_CRITICAL_SECTION_ENABLE

#ifndef KERNEL_CFG
#include "kernel.cfg"
#endif

#if defined __KERNEL_PRKERNELV4__
#include "bios_ep/kernel/prkernelv4/prfile2/pfk_api.h"
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#include "bios_ep/kernel/tk/prfile2/pft_api.h"
#elif defined __WIN32__
#include "bios_ep/kernel/win32/prfile2/pfk_api.h"
#elif defined __LINUX__
#include "bios_ep/kernel/linux/prfile2/pfk_api.h"
#elif defined AMBA_FS_KAL
#else
#error The specified kernel is not supported.
#endif


/*---------------------------------------------------------------------------
  Macro to control the critical section
 ----------------------------------------------------------------------------*/
/* definition for pfk_create_semaphore */
#if defined __KERNEL_PRKERNELV4__
#define CRIT_SEM_ATR        PFK_TPRI
#define CRIT_SEM_INITCNT    1
#define CRIT_SEM_MAXCNT     1
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#define CRIT_SEM_EXINF      0
#define CRIT_SEM_ATR        PFK_TPRI
#define CRIT_SEM_INITCNT    1
#define CRIT_SEM_MAXCNT     1
#elif defined __WIN32__
#define CRIT_SEM_INITCNT    1
#define CRIT_SEM_MAXCNT     1
#elif defined __LINUX__
#define CRIT_SEM_INITCNT    1
#define CRIT_SEM_MAXCNT     1
#endif

#define PF_CRITICAL_NOINIT  0x0
#define PF_CRITICAL_INIT    0x1

#endif  /* PF_CRITICAL_SECTION_ENABLE  */

#endif
