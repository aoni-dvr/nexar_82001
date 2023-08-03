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

char *bsargs = NULL;

static char *supported_opts[] = {
    "clk",
    NULL
};

static void board_option(xzynq_qspi_t *qspi)
{
    char    *value, *freeptr, *options;
    int     opt;

    qspi->speed = XZYNQ_GQSPI_MAX_DRATE;

    if (bsargs == NULL) return;

    freeptr = strdup(bsargs);

    options = freeptr;

    while (options && *options != '\0') {
        opt = getsubopt(&options, supported_opts, &value);
        switch (opt) {
        case 0:
            qspi->speed = strtoul(value, 0, 0);
            break;
        }
    }

    free(freeptr);
}

/*
 * This is the open callout for the QSPI serial NOR flash driver.
 */
int32_t f3s_qspi_open(f3s_socket_t *socket, uint32_t flags)
{
    xzynq_qspi_t    *qspi;

    /* check if not initialized */
    if (socket->socket_handle == NULL) {
        qspi = xzynq_qspi_open();
        if (qspi == NULL) return (ENODEV);
#if 0
        // check command line options
        board_option(qspi);

        int ret = xzynq_qspi_setcfg(qspi, XZYNQ_GQSPI_IO_MODE, qspi->speed);

        if (ret != EOK) return ret;
#endif
        socket->name = (unsigned char*)"XZYNQ QSPI serial flash";
        socket->socket_handle = (void *)qspi;
        socket->window_size = socket->array_size = TOTAL_SIZE_BYTES;
    }

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/f3s_qspi_open.c $ $Rev: 859471 $");
#endif
