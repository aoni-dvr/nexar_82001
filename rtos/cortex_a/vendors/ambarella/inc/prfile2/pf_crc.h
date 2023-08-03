/*
     Copyright (C) 2015 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_crc.h] - Cyclic Redundancy Check
 ****************************************************************************/
#ifndef __PF_CRC_H__
#define __PF_CRC_H__

#include "prfile2/pf_types.h"

/*---------------------------------------------------------------------------
 PFCRC_crc32 - Get CRC32 value of the specified buffer
---------------------------------------------------------------------------*/
PF_U_LONG  PFCRC_crc32(PF_CONST void *buffer, PF_U_LONG bytes);

#endif /* __PF_CRC_H__ */
