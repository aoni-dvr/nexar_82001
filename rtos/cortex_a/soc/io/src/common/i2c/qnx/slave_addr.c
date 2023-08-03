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

int Amba_I2c_SetSlaveAddr(void *hdl, unsigned int addr, i2c_addrfmt_t fmt)
{
    amba_i2c_dev_t *dev = hdl;

    switch (fmt) {
    case I2C_ADDRFMT_7BIT:
        break;
    case I2C_ADDRFMT_10BIT:
        /* AMBA I2C does not support 10bit slave address */
        return -1;
    default:
        return -1;
    }

    dev->slave_addr = addr;

    return 0;
}

