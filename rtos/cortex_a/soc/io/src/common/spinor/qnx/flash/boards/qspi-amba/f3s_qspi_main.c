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

#include "AmbaTypes.h"
#include "AmbaSYS.h"

extern char *bsargs;

/*
 * This is the main function for the QSPI f3s flash file system.
 */
int main(int argc, char **argv)
{
    int    error;

    static f3s_service_t service[] = {
        {
            sizeof(f3s_service_t),
            f3s_qspi_open,
            f3s_qspi_page,
            f3s_qspi_status,
            f3s_qspi_close
        },
        {
            0, 0, 0, 0, 0    /* mandatory last entry */
        }
    };

#if MTD_VER == 2
    static f3s_flash_v2_t flash[] = {
        {
            sizeof(f3s_flash_v2_t),
            f3s_qspi_ident,        /* Ident */
            f3s_qspi_reset,        /* Reset */

            NULL,                  /* v1 Read                  */
            NULL,                  /* v1 Write                 */
            NULL,                  /* v1 Erase                 */
            NULL,                  /* v1 Suspend               */
            NULL,                  /* v1 Resume                */
            NULL,                  /* v1 Sync                  */

            f3s_qspi_read,         /* v2 Read                  */
            f3s_qspi_write,        /* v2 Write                 */
            f3s_qspi_erase,        /* v2 Erase                 */
            NULL,                  /* v2 Suspend               */
            NULL,                  /* v2 Resume                */
            f3s_qspi_sync,         /* v2 Sync                  */
            NULL,                  /* v2 Islock                */
            NULL,                  /* v2 Lock                  */
            NULL,                  /* v2 Unlock                */
            NULL,                  /* v2 Unlockall             */
            NULL
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    /* mandatory last entry */
        }
    };
#else
#error "MTD version must be 2"
#endif

    if (strstr(argv[argc - 1], "clk") != NULL) bsargs = argv[--argc];

    UINT32 BootMode;

    (void)AmbaSYS_GetBootMode(&BootMode);
    if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NOR) {
        /* init f3s */
        f3s_init(argc, argv, (f3s_flash_t *)flash);

        /* start f3s */
        error = f3s_start(service, (f3s_flash_t *)flash);
    } else {
        error = -1;
    }

    return error;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/f3s_qspi_main.c $ $Rev: 859471 $");
#endif
