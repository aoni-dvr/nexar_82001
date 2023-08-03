/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <sys/slog.h>

#include "f3s_qspi.h"

/*
 * This is the erase callout for SPI serial NOR flash.
 */
int f3s_qspi_erase(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags,
                   uint32_t offset)
{
    int rc;

//    slogf(1000, 1, "(devf  t%d::%s:%d) offset=%x", pthread_self(), __func__, __LINE__, offset);

    if (access->service->page(&access->socket, 0, offset, NULL) == NULL) {
        return (ERANGE);
    }

    rc = sector_erase(access->socket.socket_handle, offset);
    if (rc < 0) return (EIO);

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/f3s_qspi_erase.c $ $Rev: 859471 $");
#endif
