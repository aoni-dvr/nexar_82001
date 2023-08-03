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

#include <arm/inout.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/slogcodes.h>
#include <unistd.h>

#include <aarch64/imx8_common/imx_bch.h>

#include "chipio.h"
#include "apbh_dma.h"

/**
 * @file       imx-micron/apbh_dma.c
 * @addtogroup etfs_dma DMA
 * @{
 */

/**
 * Conditional wait for interrupt occurrence.
 *
 * @param chipio Low level driver handle.
 *
 * @return NULL always.
 */
int apbh_intr_wait(chipio *chipio)
{
    pthread_mutex_lock(&chipio->apbhmutex);
    while (chipio->apbhirq_expected == 0) {
        pthread_cond_wait(&chipio->apbhcond, &chipio->apbhmutex);
    }
    chipio->apbhirq_expected = 0;
    pthread_cond_signal(&chipio->apbhcond);
    pthread_mutex_unlock(&chipio->apbhmutex);
    return NULL;
}

/**
 * APBH interrupt thread.
 *
 * @param arg Low level driver handle.
 */
void *apbhint_thread(void *arg)
{
    chipio *chipio = arg;

    ThreadCtl (_NTO_TCTL_IO, NULL);
    SIGEV_INTR_INIT(&chipio->apbhevent);
    chipio->apbhiid = InterruptAttachEvent(chipio->apbhirq, &chipio->apbhevent, _NTO_INTR_FLAGS_TRK_MSK);
    if (chipio->apbhiid == -1) {
        slogf(1000, 1, "(devf  t%d::%s:%d) irq=%x", pthread_self(), __func__, __LINE__, chipio->apbhirq);
        return NULL;
    }

    while (1) {
        InterruptWait (NULL, NULL);
        out32(chipio->apbh_dma_reg_base + IMX_APBH_CTRL1_CLR_OFFSET, 0x1); /* Clear the apbh flag */
        pthread_mutex_lock(&chipio->apbhmutex);
        while (chipio->apbhirq_expected == 1) {
            pthread_cond_wait(&chipio->apbhcond, &chipio->apbhmutex);
        }
        chipio->apbhirq_expected = 1;
        pthread_cond_signal(&chipio->apbhcond);
        //
        pthread_mutex_unlock(&chipio->apbhmutex);
        InterruptUnmask(chipio->apbhirq, chipio->apbhiid);
    }
    return NULL;
}

/**
 * DMA global initialization.
 *
 * @param chipio Low level driver handle.
 */
void apbh_init(chipio *chipio)
{
    uint32_t apbh_virt_base = chipio->apbh_dma_reg_base;

    // APBH - disable reset, enable clock, bring APBH out of reset
    out32((apbh_virt_base + IMX_APBH_CTRL0_CLR_OFFSET), IMX_APBH_CTRL0_SFTRST_MASK);

    // Poll until the SFTRST is truly deasserted.
    while((*(volatile imx_apbh_ctrl0_t *) (apbh_virt_base + IMX_APBH_CTRL0_OFFSET)).B.SFTRST);

    out32((apbh_virt_base + IMX_APBH_CTRL0_CLR_OFFSET), IMX_APBH_CTRL0_CLKGATE_MASK);

    // Poll until the CLKGATE is truly deasserted.
    while((*(volatile imx_apbh_ctrl0_t *) (apbh_virt_base + IMX_APBH_CTRL0_OFFSET)).B.CLKGATE);
}

/**
 * DMA channel initialization.
 *
 * @param chipio Low level driver handle.
 */
void apbh_init_dma_channel(chipio *chipio)
{
    uint32_t apbh_virt_base = chipio->apbh_dma_reg_base;
    uint32_t apbh_channel_mask = 0x10001;
    uint8_t cnt = 0;

    /* Reset dma channel 0 */
    out32((apbh_virt_base + IMX_APBH_CHANNEL_CTRL_SET_OFFSET), IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL(apbh_channel_mask));

    /* Wait for the reset to complete */
    while((*(volatile imx_apbh_channel_ctrl_t *) (apbh_virt_base + IMX_APBH_CHANNEL_CTRL_OFFSET)).B.RESET_CHANNEL & apbh_channel_mask) {
        delay(1);
        cnt++;
        if (cnt > 10) {
            break;
        }
    };

    /* Clear corresponding IRQ and enable interrupt */
    out32((apbh_virt_base + IMX_APBH_CTRL1_OFFSET), apbh_channel_mask);
}

/** @}*/
