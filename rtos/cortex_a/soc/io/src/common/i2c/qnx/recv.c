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

i2c_status_t Amba_I2c_recv(void *hdl, void *buf, unsigned int len, unsigned int stop)
{
    amba_i2c_dev_t *dev = hdl;
    AMBA_I2C_TRANSACTION_s RxTransaction;

    if (len <= 0) {
        return I2C_STATUS_DONE;
    }
    dev->size_to_receive = len;
    dev->Rxbuf = buf;
    dev->stop = stop;

    RxTransaction.DataSize = dev->size_to_receive;
    RxTransaction.SlaveAddr = dev->slave_addr;
    RxTransaction.pDataBuf = dev->Rxbuf;

    dev->RxStatus = 1;

    if (stop) {
        AmbaRTSL_I2cRead(dev->ChannelId, (dev->stop << 0x4) | dev->speed, &RxTransaction);
    } else {
        AmbaRTSL_I2cWrite(dev->ChannelId, dev->speed, &RxTransaction);
    }

    /* Wait for the total transfer */
    Amba_I2c_WaitComplete(dev);

    if (dev->status != I2C_STATUS_DONE) {
        Amba_I2c_Abort(hdl, 0);
    }

    Amba_I2c_reset(dev);

    return dev->status;
}

