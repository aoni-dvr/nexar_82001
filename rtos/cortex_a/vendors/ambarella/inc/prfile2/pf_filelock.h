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
 [pf_filelock.h] -
 ****************************************************************************/
#ifndef __PF_FILELOCK_H__
#define __PF_FILELOCK_H__

#include "pf_common.h"

/*---------------------------------------------------------------------------
  Prototypes
 ----------------------------------------------------------------------------*/
void PF_InitLockFile(void);
PF_ERROR PF_LockFile(PF_FILE* p_file, const PF_FILE* owner);
PF_ERROR PF_UnLockFile(const PF_FILE* p_file);

/* You must not modify these macro name: */
/*          INIT_FILELOCK                */
/*          FILE_LOCK(p_lock, p_vol)     */
/*          FILE_UNLOCK(p_lock)          */

#define INIT_FILELOCK               PF_InitLockFile
#define FILE_LOCK(p_file, owner)    PF_LockFile(p_file, owner)
#define FILE_UNLOCK(p_file)         PF_UnLockFile(p_file)

/*---------------------------------------------------------------------------
  Macro to control the file lock structure
 ----------------------------------------------------------------------------*/
/* You must not modify this define */
#define PF_SYS_SET_FLOCKC           (pf_sys_set.flock_count)

#define PF_MAX_FILELOCK             (0xFFFFFFFFuL)

#endif /* __PF_FILELOCK_H__ */
