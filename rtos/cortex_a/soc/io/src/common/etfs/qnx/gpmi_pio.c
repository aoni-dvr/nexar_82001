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

#include <stdint.h>
#include <arm/inout.h>
#include <unistd.h>

#include <arm/imx/imx_gpmi.h>

#include "chipio.h"

/**
 * @file       imx-micron/gpmi_pio.c
 * @addtogroup etfs_gpmi PIO
 * @{
 */

/**
 * GPMI software reset.
 *
 * @param chipio Low level driver handle.
 */
void gpmi_soft_reset(chipio *chipio)
{
    uintptr_t gpmi_virt_base = chipio->gpmi_pio_reg_base;

    // Reset the GPMI_CTRL0 block.
    // Prepare for soft-reset by making sure that SFTRST is not currently
    // asserted.  Also clear CLKGATE so we can wait for its assertion below.
    out32((gpmi_virt_base + IMX_GPMI_CTRL0_CLR_OFFSET), IMX_GPMI_CTRL0_SFTRST_MASK);

    // Wait at least a microsecond for SFTRST to deassert.
    delay(2);
    while((*(volatile imx_gpmi_ctrl0_t *) (gpmi_virt_base + IMX_GPMI_CTRL0_OFFSET)).B.SFTRST);

    // Also clear CLKGATE so we can wait for its assertion below.
    out32((gpmi_virt_base + IMX_GPMI_CTRL0_CLR_OFFSET), IMX_GPMI_CTRL0_CLKGATE_MASK);

    // Now soft-reset the hardware.
    out32((gpmi_virt_base + IMX_GPMI_CTRL0_SET_OFFSET), IMX_GPMI_CTRL0_SFTRST_MASK);

    // Poll until clock is in the gated state before subsequently
    // clearing soft reset and clock gate.
    while(!(*(volatile imx_gpmi_ctrl0_t *) (gpmi_virt_base + IMX_GPMI_CTRL0_OFFSET)).B.CLKGATE);

    // Bring GPMI_CTRL0 out of reset
    out32((gpmi_virt_base + IMX_GPMI_CTRL0_CLR_OFFSET), IMX_GPMI_CTRL0_SFTRST_MASK);

    // Wait at least a microsecond for SFTRST to deassert.
    delay(2);
    while((*(volatile imx_gpmi_ctrl0_t *) (gpmi_virt_base + IMX_GPMI_CTRL0_OFFSET)).B.SFTRST);

    out32((gpmi_virt_base + IMX_GPMI_CTRL0_CLR_OFFSET), IMX_GPMI_CTRL0_CLKGATE_MASK);

    // Poll until clock is in the NON-gated state before returning.
    while((*(volatile imx_gpmi_ctrl0_t *) (gpmi_virt_base + IMX_GPMI_CTRL0_OFFSET)).B.CLKGATE);
}

/**
 * Sets busy time out for GPMI transfers.
 *
 * @param chipio       Low level driver handle.
 * @param busy_timeout Time out value.
 */
void gpmi_set_busy_timeout(chipio *chipio, uint16_t busy_timeout)
{
    uint32_t gpmi_virt_base = chipio->gpmi_pio_reg_base;

    out32(gpmi_virt_base + IMX_GPMI_TIMING1_OFFSET, IMX_GPMI_TIMING1_DEVICE_BUSY_TIMEOUT(busy_timeout));
}

/** @}*/
