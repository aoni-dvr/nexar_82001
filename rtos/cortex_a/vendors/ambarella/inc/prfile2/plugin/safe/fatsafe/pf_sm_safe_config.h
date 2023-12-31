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
[pf_sm_safe_config.h] - Configurations of "PrFILE2 FAT Safe"
****************************************************************************/
#ifndef __PF_SM_SAFE_CONFIG_H__
#define __PF_SM_SAFE_CONFIG_H__

#include "parts.cfg"


/****************************************************************************
  CONFIGURATIONS
****************************************************************************/
#if USE_EBINDER
  /*
   * In eBinder environment, The configurations are defined in
   * 'eb_pf_fcsm_bitmap_config.h' which is automatically generated by
   * eBinder Configurator.
   */
#include "prfile2/plugin/safe/fatsafe/eb_pf_sm_safe_config.h"

#else /* ! USE_EBINDER */

  /*
   * If this value is 1, the programs check feasibility.
   * Default : 0
   */
#define PFSAFE_PARAM_CHECK_ENABLE        (1)

  /*
   * Set the name of the recovery file.
   * Default : "PRF2SAFE.RCV"
   */
#define PFSAFE_RECOVERY_FILE_NAME        "PRF2SAFE.RCV"

  /*
   * If this value is 1, the programs check whether the clusters of the recovery
   * file are continue.
   * Default : 0
   */
#define PFSAFE_CHECK_RECOVERY_FILE       (1)

  /*
   * Set the sector of creating meta-data. If set 0xFFFFFFFF to this,
   * the programs searches an empty sector to create meta-data.
   * Default : 0xFFFFFFFFuL
   */
#define PFSAFE_META_DATA_SECTOR          (0xFFFFFFFFuL)

#endif /* USE_EBINDER */


#endif  /* __PF_SM_SAFE_CONFIG_H__ */
