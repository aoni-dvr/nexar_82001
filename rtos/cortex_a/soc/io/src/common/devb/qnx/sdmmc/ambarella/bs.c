/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems.
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

/* Module Description:  board specific interface */

#include <internal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/inout.h>
#include <sys/mman.h>

#include <sdhci.h>
#include <bs.h>


static int xzynq_bs_init(sdio_hc_t *hc)
{
    int    status = EOK;

    if(hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) {
        hc->caps |= HC_CAP_HS200;
    }

    status = sdhci_init(hc);
    if( status != EOK ) {
        return status;
    }

    if(!(hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED)) {
        /* Overwrite some of the capabilities that are set by sdhci_init() */
        hc->caps &= ~HC_CAP_CD_INTR;
        /* Ambarella */
        //hc->caps &= ~HC_CAP_SV_1_8V;
    }
    return status;
}

int bs_event(sdio_hc_t *hc, sdio_event_t *ev)
{
    int    status;

    switch(ev->code) {
    case HC_EV_CD:
        status = sdio_hc_event(hc, HC_EV_CD);
        break;

    default:
        status = ENOTSUP;
        break;
    }

    return status;
}

sdio_product_t  sdio_fs_products[] = {
    { SDIO_DEVICE_ID_WILDCARD, 0, 0, "Ambarella", xzynq_bs_init },
};

sdio_vendor_t   sdio_vendors[] = {
    { SDIO_VENDOR_ID_WILDCARD, "Ambarella", sdio_fs_products },
    { 0, NULL, NULL }
};

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/devb/sdmmc/aarch64/xzynq.le/bs.c $ $Rev: 895454 $")
#endif
