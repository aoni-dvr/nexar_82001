/* Copyright(C) 2005 eSOL Co., Ltd. All rights reserved.

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
[pdm_partition.h] - 
****************************************************************************/
#ifndef __PDM_PARTITION_H
#define __PDM_PARTITION_H

#include "dskmng/pdm_dskmng.h"


/****************************************************************************
  Prototypes
****************************************************************************/
/*---------------------------------------------------------------------------
    pdm_part_open_partition - Open partition.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_open_partition(PDM_DISK*         p_disk,
                                  PDM_U_SHORT       part_id,
                                  PDM_PARTITION**   pp_part);

/*---------------------------------------------------------------------------
    pdm_part_close_partition - Close partition.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_part_close_partition(PDM_PARTITION* p_part);

/*---------------------------------------------------------------------------
    pdm_part_set_driver_error_code - Set error code.
---------------------------------------------------------------------------*/
void pdm_part_set_driver_error_code(PDM_PARTITION* p_part, PDM_ERROR error_code);

/*---------------------------------------------------------------------------
    pdm_part_check_partition_open - Check partition open status on disk.
---------------------------------------------------------------------------*/
void pdm_part_check_partition_open(PDM_DISK* p_disk, PDM_BOOL* is_open);

/*---------------------------------------------------------------------------
    pdm_part_notify_media_event - Getting a disk events
---------------------------------------------------------------------------*/
void  pdm_part_notify_media_event(PDM_DISK* p_disk, PDM_U_LONG event);

#endif /* __PDM_PARTITION_H */
