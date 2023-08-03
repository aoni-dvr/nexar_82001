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

#include "proto.h"

/* open() ... */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <hw/ambarella_clk.h>

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
AMBA_I2CS_REG_s * pAmbaI2C_SlaveReg = (AMBA_I2CS_REG_s *)AMBA_CA53_I2C_SLAVE_BASE_ADDR;
AMBA_I2C_REG_s * pAmbaI2C_MasterReg = (AMBA_I2C_REG_s *)AMBA_CA53_I2C0_BASE_ADDR;
#else
AMBA_I2CS_REG_s * pAmbaI2C_SlaveReg = (AMBA_I2CS_REG_s *)AMBA_CORTEX_A53_SSI_SLAVE_BASE_ADDR;
AMBA_I2C_REG_s * pAmbaI2C_MasterReg = (AMBA_I2C_REG_s *)AMBA_CORTEX_A53_I2C0_BASE_ADDR;
#endif

void Amba_I2c_reset(amba_i2c_dev_t* dev)
{
    /* set duty cycle zero */
    AmbaRTSL_I2cInit();

    /* disable I2C */
    AmbaRTSL_I2cStop((dev->stop << 4U) | dev->ChannelId);
}

void Amba_I2c_Abort(void *hdl, int rcvid)
{
    (void) rcvid;
    amba_i2c_dev_t *dev = hdl;

    AmbaRTSL_I2cStop(dev->ChannelId);
}

void * Amba_I2c_init(int argc, char *argv[])
{
    amba_i2c_dev_t *dev;

    if (ThreadCtl(_NTO_TCTL_IO, 0) == -1) {
        perror("ThreadCtl");
        return NULL;
    }

    dev = malloc(sizeof(amba_i2c_dev_t));
    if (!dev)
        return NULL;

    if (AMBA_I2c_options(dev, argc, argv) == -1)
        goto fail;

    dev->regbase = mmap_device_io(dev->reglen, dev->physbase);
    if (dev->regbase == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        goto fail;
    } else {
        pAmbaI2C_MasterReg = (AMBA_I2C_REG_s *)dev->regbase;
    }

    /* Initialize interrupt handler */
    SIGEV_INTR_INIT(&dev->intrevent);
    dev->iid = InterruptAttach(dev->intr, Amba_I2c_intr, dev, 0, 0);
    if (dev->iid == -1) {
        perror("InterruptAttach");
        goto fail_int_attach;
    }

    /* Reset the i2c controller */
    Amba_I2c_reset(dev);

    return dev;

fail_int_attach:
    munmap_device_io(dev->regbase, dev->reglen);
fail:
    free(dev);
    return NULL;
}

