/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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

#include <hw/inout.h>
#include "ipl.h"
#include "sdmmc.h"
#include "private/fat-fs.h"
#include "sdhc_ambarella.h"
#include "ipl_ambarella.h"

#if !defined(CONFIG_XEN_SUPPORT_QNX)

/*
* The buffer used by fat-fs.c as the common buffer, which should be cache-disabled,
* Also ADMA requires that the buffers should be in the external memory
*/
static unsigned char fat_buf2[FAT_COMMON_BUF_SIZE] __attribute__ ((section(".scratch")));
static unsigned char fat_buf1[FAT_FS_INFO_BUF_SIZE] __attribute__ ((section(".scratch")));

/* ADMA demands that both the buffer and DMA descriptor should be in DRAM, not internal SRAM */
static amba_adma32_t adma_des[MAX_BLKCNT * SDMMC_BLOCKSIZE / ADMA_CHUNK_SIZE] __attribute__ ((section(".scratch")));

void delay(unsigned dly)
{
    volatile int j;

    while (dly--) {
        for (j = 0; j < 32; j++) ;
    }
}

static inline int sdmmc_load_file(paddr_t address, const char *fn)
{
    amba_sdmmc_t sdmmc;
    amba_adma_ext_t dma_ext = {
        .adma_des = adma_des
    };
    int status = SDMMC_OK;

    /*
     * Initialize the SDMMC interface
     */
    sdmmc.sdmmc_pbase = AMBA_SDIO1_BASEADDR;    // SD1 base address

    sdmmc.adma_ext = &dma_ext;

    /* initialize the sdmmc interface and card */
    if (SDMMC_OK != sdmmc_init_ctrl(&sdmmc)) {
        return SDMMC_ERROR;
    }

    if (sdmmc_init_card(&sdmmc)) {
        return SDMMC_ERROR;
    }

    ser_putstr("Load QNX image from SDMMC...\n");

    fat_sdmmc_t fat = {
        .ext = &sdmmc,
        .buf1 = (void *)fat_buf1,
        .buf1_len = FAT_FS_INFO_BUF_SIZE,
        .buf2 = (void *)fat_buf2,
        .buf2_len = FAT_COMMON_BUF_SIZE,
        .verbose = 0
    };

    if (fat_init(&fat)) {
        ser_putstr("Failed to init fat-fs\n");
        status = SDMMC_ERROR;
        goto done;
    }

    status = fat_copy_named_file((unsigned char *)address, (char *)fn);
    if (status != SDMMC_OK) {
        ser_putstr("Failed to copy named file\n");
    }

done:
    sdmmc_fini(&sdmmc);

    return status;
}

#endif

int ipl_main()
{
#if !defined(CONFIG_XEN_SUPPORT_QNX)
    unsigned image = QNX_LOAD_ADDR;
#else
    unsigned image = 0x4f024000;  // FIXME: find another way around this!
#endif
    int      do_cksum = 0;

    /* Init serial interface */
#if !defined(CONFIG_XEN_QNX_IMG)
    init_serial();
#else
    init_serial_xen();
#endif

    ser_putstr("\nWelcome to QNX Neutrino Initial Program Loader for Ambarella CV2FS \n");

    ser_putstr("Scanning for image at @ 0x");
    ser_puthex(image);
    ser_putstr("\n\n");

    image = image_scan_2(image, image + 0x1000, do_cksum);

    if (image != 0xffffffff) {
        ser_putstr("Found image               @ 0x");
        ser_puthex(image);
        ser_putstr("\n");

        image_setup_2(image);

        ser_putstr("Jumping to startup        @ 0x");
        ser_puthex(startup_hdr.startup_vaddr);
        ser_putstr("\n\n");

        image_start_2(image);

        /* Never reach here */
        return 0;
    }

    ser_putstr("image_scan_2 failed...\n");

    return 0;
}

