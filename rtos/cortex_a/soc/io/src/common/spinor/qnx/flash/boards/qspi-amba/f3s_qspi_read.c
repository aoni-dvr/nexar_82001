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

#include "f3s_qspi.h"

/*
 * This is the read callout for SPI serial NOR flash.
 */
int32_t f3s_qspi_read(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags,
                      uint32_t text_offset, int32_t buffer_size, uint8_t *buffer)
{
    int rc;
    /* check if offset does not fit in array */
    if (text_offset >= access->socket.window_size) {
        errno = ERANGE;
        return -1;
    }

    /* ensure that offset + size is not out of bounds */
    buffer_size = min(buffer_size, access->socket.window_size - text_offset);
    rc = read_from(access->socket.socket_handle, text_offset,
                   buffer_size, buffer);
    if (-1 == rc) {
        fprintf(stderr,"\tOffset: 0x%X Size: %d\n",text_offset,buffer_size);
        errno = EIO;
        return -1;
    }
    return rc; /* return number of bytes read */
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/f3s_qspi_read.c $ $Rev: 859471 $");
#endif
