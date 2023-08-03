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
 [pdm_memory.h] - Memory
 ****************************************************************************/
#ifndef __PDM__MEMORY_H__
#define __PDM__MEMORY_H__

#include "kernel.cfg"
#include "prfile2/pdm_config.h"

#if defined __KERNEL_PRKERNELV4__
#include "bios_ep/kernel/prkernelv4/prfile2/pfk_api.h"
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#include "bios_ep/kernel/tk/prfile2/pft_api.h"
#elif defined __WIN32__
#include "bios_ep/kernel/win32/prfile2/pfk_api.h"
#elif defined AMBA_FS_KAL
#else
#error The specified kernel is not supported.
#endif

/*------------------------------------------------------------------------
  Lock/Unlock memory space
 *-----------------------------------------------------------------------*/
#if PDM_LOCK_SPACE_ENABLE
 #define pdm_mem_lock_space(vp,len)     ((pfk_lock_space((void*)(vp),(PF_S_LONG)(len))==0) ? (PDM_OK) : (PDM_ERR_ACCESS_PERMISSION))
 #define pdm_mem_unlock_space(vp,len)   ((pfk_unlock_space((void*)(vp),(PF_S_LONG)(len))==0) ? (PDM_OK) : (PDM_ERR_ACCESS_PERMISSION))
#else /* PDM_LOCK_SPACE_ENABLE */
 #define pdm_mem_lock_space(vp,len)     (PDM_OK)
 #define pdm_mem_unlock_space(vp,len)   (PDM_OK)
#endif /* PDM_LOCK_SPACE_ENABLE */

#endif /* __PDM__MEMORY_H__ */

