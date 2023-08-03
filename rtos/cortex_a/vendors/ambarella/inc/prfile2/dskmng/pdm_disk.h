/*
    Copyright(C) 2005 eSOL Co., Ltd. All rights reserved.

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
[pdm_disk.h] - 
****************************************************************************/
#ifndef __PDM_DISK_H__
#define __PDM_DISK_H__

#include "dskmng/pdm_dskmng.h"

/****************************************************************************
  Prototypes
****************************************************************************/
#if PDM_PARAM_CHECK_ENABLE
/*---------------------------------------------------------------------------
    pdm_disk_check_disk_handle - Check disk handle.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_check_disk_handle(PDM_DISK* p_disk);
#endif /* PDM_PARAM_CHECK_ENABLE */

/*---------------------------------------------------------------------------
    pdm_disk_open_disk - Open disk.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_open_disk(PDM_INIT_DISK *p_init_disk_tbl, PDM_DISK** pp_disk);

/*---------------------------------------------------------------------------
    pdm_disk_close_disk - Close disk.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_close_disk(PDM_DISK *p_disk);

/*---------------------------------------------------------------------------
    pdm_disk_register_erase - Register a disk erase function.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_register_erase(PDM_DISK* p_disk, PDM_ERASE_FUNC p_func);

/*---------------------------------------------------------------------------
    pdm_disk_get_part_permission - Get access permision and lock disk.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_get_part_permission(PDM_DISK *p_disk);

/*---------------------------------------------------------------------------
    pdm_disk_release_part_permission - Release access permission and unlock disk.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_release_part_permission(PDM_DISK *p_disk, PDM_U_LONG mode);

/*---------------------------------------------------------------------------
    pdm_disk_set_disk - Set disk information.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_set_disk(PDM_DISK* p_disk, PDM_PARTITION* p_part);

/*---------------------------------------------------------------------------
    pdm_disk_get_lba_size - Get bytes per LBA sector in media.
---------------------------------------------------------------------------*/
PDM_ERROR pdm_disk_get_lba_size(PDM_DISK* p_disk, PDM_U_SHORT* p_lba_size);


#endif /* __PDM_DISK_H__ */
