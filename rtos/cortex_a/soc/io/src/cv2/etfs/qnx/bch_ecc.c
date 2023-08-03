/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/slog.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <fs/etfs.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <aarch64/imx8_common/imx_bch.h>
//#include <arm/imx/imx_bch.h>
#include <arm/imx/imx_gpmi.h>

#include "bch_ecc.h"
#include "chipio.h"
#include "devio.h"

/**
 * @file       imx-micron/bch_ecc.c
 * @addtogroup etfs_bch ECC
 * @{
 */

/**
 * Conditional wait for interrupt occurrence.
 *
 * @param chipio Low level driver handle.
 *
 * @return NULL always.
 */
int bch_intr_wait(chipio *chipio)
{
    pthread_mutex_lock(&chipio->bchmutex);
    while (chipio->bchirq_expected == 0) {
        pthread_cond_wait(&chipio->bchcond, &chipio->bchmutex);
    }
    chipio->bchirq_expected = 0;
    pthread_cond_signal(&chipio->bchcond);
    pthread_mutex_unlock(&chipio->bchmutex);
    return NULL;
}

/**
 * BCH interrupt thread.
 *
 * @param arg Low level driver handle.
 */
void *bchint_thread(void *arg)
{
    chipio *chipio = arg;

    ThreadCtl (_NTO_TCTL_IO, NULL);
    SIGEV_INTR_INIT(&chipio->bchevent);
    chipio->bchiid = InterruptAttachEvent(chipio->bchirq, &chipio->bchevent, _NTO_INTR_FLAGS_TRK_MSK);
    if (chipio->bchiid == -1) {
        slogf(1000, 1, "(devf  t%d::%s:%d) irq=%x", pthread_self(), __func__, __LINE__, chipio->bchirq);
        return NULL;
    }

    while (1) {
        InterruptWait (NULL, NULL);
        out32(chipio->bch_ecc_reg_base + IMX_BCH_CTRL_CLR_OFFSET, 1); //clear the bch flag
        pthread_mutex_lock(&chipio->bchmutex);
        while (chipio->bchirq_expected == 1) { //not needed
            pthread_cond_wait(&chipio->bchcond, &chipio->bchmutex);
        }
        chipio->bchirq_expected = 1;
        pthread_cond_signal(&chipio->bchcond);
        //
        pthread_mutex_unlock(&chipio->bchmutex);
        InterruptUnmask(chipio->bchirq, chipio->bchiid);
    }
    return NULL;
}

/**
 * BCH peripheral initialization routine.
 *
 * @param chipio Low level driver handle.
 */
void bch_init (chipio *chipio)
{

    uintptr_t bch_virt_base  = chipio->bch_ecc_reg_base;

    // A soft reset can take multiple clocks to complete, so do NOT gate the
    // clock when setting soft reset. The reset process will gate the clock
    // automatically. Poll until this has happened before subsequently
    // preparing soft-reset and clock gate
    out32(bch_virt_base + IMX_BCH_CTRL_CLR_OFFSET, IMX_BCH_CTRL_SFTRST_MASK);
    out32(bch_virt_base + IMX_BCH_CTRL_CLR_OFFSET, IMX_BCH_CTRL_CLKGATE_MASK);

    //asserting soft-reset
    out32(bch_virt_base + IMX_BCH_CTRL_SET_OFFSET, IMX_BCH_CTRL_SFTRST_MASK);
    //waiting for confirmation of soft-reset
    while (!(*(volatile imx_bch_ctrl_t *) (bch_virt_base + IMX_BCH_CTRL_OFFSET)).B.CLKGATE);

    //done
    out32(bch_virt_base + IMX_BCH_CTRL_CLR_OFFSET, IMX_BCH_CTRL_SFTRST_MASK);
    out32(bch_virt_base + IMX_BCH_CTRL_CLR_OFFSET, IMX_BCH_CTRL_CLKGATE_MASK);

    //Enable completion IRQ
    out32(bch_virt_base + IMX_BCH_CTRL_SET_OFFSET, IMX_BCH_CTRL_COMPLETE_IRQ_EN_MASK);
}

/**
 * Sets device parameters for BCH engine. (memory layout, sub-block size, ecc size, etc...).
 *
 * @param chipio Low level driver handle.
 */
void bch_set_layout (chipio *chipio)
{
    uintptr_t bch_virt_base = chipio->bch_ecc_reg_base;
    static const uint32_t subsequent_block_size = BCH_SUBBLOCK_SIZE + ETFS_META_SIZE_PER_SUBBLOCK; //see data parts in spare_t struct

    // Fill the layout 0 for ECC subsequent page layout
    out32( (bch_virt_base + IMX_BCH_FLASH0LAYOUT0_OFFSET),
           IMX_BCH_FLASH0LAYOUT0_DATA0_SIZE(subsequent_block_size/4) |      // Subsequent data blocks (in DWORDS) on the flash page.
           IMX_BCH_FLASH0LAYOUT0_GF13_0_GF14_1(1) |                         // 1 = 14 GF
           IMX_BCH_FLASH0LAYOUT0_ECC0(0xC) |                                // 0xC ECC24 — ECC 24 to be performed
           IMX_BCH_FLASH0LAYOUT0_META_SIZE(0) |                             // No BCH meta data usage
           IMX_BCH_FLASH0LAYOUT0_NBLOCKS(3) );                              // 4096/1024 (- 1 for subsequent block 0)
    // Fill the layout 1 for ECC subsequent page layout
    out32( (bch_virt_base + IMX_BCH_FLASH0LAYOUT1_OFFSET),
           IMX_BCH_FLASH0LAYOUT1_DATAN_SIZE(subsequent_block_size/4) |      //Subsequent data blocks (in DWORDS) on the flash page.
           IMX_BCH_FLASH0LAYOUT1_GF13_0_GF14_1(1) |                         // 1 = 14 GF
           IMX_BCH_FLASH0LAYOUT1_ECCN(0xC)  |                               // 0x4 ECC8 — ECC 8 to be performed
           IMX_BCH_FLASH0LAYOUT1_PAGE_SIZE(DATASIZE + SPARESIZE) );

    // Set all chip enables to use layout 0 by clearing the layout select register.
    out32((bch_virt_base + IMX_BCH_LAYOUTSELECT_OFFSET), 0);
}

/**
 * Sets the BCH erase threshold value.
 *
 * @param chipio    Low level driver handle.
 * @param threshold Threshold value.
 */
void bch_set_erase_threshold (chipio *chipio, uint8_t threshold)
{
    uintptr_t bch_virt_base = chipio->bch_ecc_reg_base;

    out32((bch_virt_base + IMX_BCH_MODE_OFFSET), IMX_BCH_MODE_ERASE_THRESHOLD(threshold));
}

/**
 * @param chipio Low level driver handle.
 *
 * @return BCH status register value.
 */
uint32_t bch_get_ecc_status (chipio *chipio)
{
    return in32(chipio->bch_ecc_reg_base + IMX_BCH_STATUS0_OFFSET);
}

/** @}*/
