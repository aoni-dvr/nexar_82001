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

uint32_t Amba_wait_bus_not_busy(amba_i2c_dev_t *dev)
{
    (void) dev;

    /* Wait no more than 1s */
    unsigned tries = 1000;

    while (0x0 != pAmbaI2C_MasterReg->Status.Status) {
        delay(1);
        if (tries-- <= 0)
            return -1;
    }

    return 0;
}

static unsigned int I2C_MasterCheckACK(amba_i2c_dev_t *dev)
{
    unsigned int CtrlReg = pAmbaI2C_MasterReg->Ctrl;
    unsigned int RetVal = 0U;

    /* The following code segment is necessary for the device cannot ACK on time. */
    /* However it needs to skip the check for those devices never ACK. (e.g., some HDMI devices) */
#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if (dev->ChannelId != AMBA_I2C_HDMI_CHANNEL) {
#endif
        if ((CtrlReg & AMBA_I2C_CR_ACK_MASK) != 0x0U) {
            dev->stop = 1;
            AmbaRTSL_I2cStop(dev->ChannelId | (dev->stop << 4));
            RetVal = 1U;
        }
#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    }
#endif

    return RetVal;
}

static void I2C_MasterWriteIntHandler(amba_i2c_dev_t *dev)
{
    if (I2C_MasterCheckACK(dev) == 0x0U) {
        if (dev->size_to_send != 0U) {
            pAmbaI2C_MasterReg->Data.Value = *(dev->Txbuf) & 0xff;
            dev->size_to_send--;
            dev->Txbuf++;
            pAmbaI2C_MasterReg->Ctrl = 0x0U;
        } else {
            if (dev->stop) {
                AmbaRTSL_I2cStop(dev->ChannelId | (dev->stop << 4U));
                dev->status = I2C_STATUS_DONE;
            } else {
                dev->stop = 1U;
                pAmbaI2C_MasterReg->Data.Value = (dev->slave_addr) | 0x1;
                AmbaCSL_I2cStart(pAmbaI2C_MasterReg);
            }
        }
    } else {
        dev->status = I2C_STATUS_NACK;
    }
}

static void I2C_MasterReadIntHandler(amba_i2c_dev_t *dev)
{
    if (dev->RxStatus == 1) {
        if (I2C_MasterCheckACK(dev) == 0U) {
            if (dev->size_to_receive == 1U) {
                /* One byte remains */
                pAmbaI2C_MasterReg->Ctrl = AMBA_I2C_NACK;
            } else {
                /* More than one byte remains */
                pAmbaI2C_MasterReg->Ctrl = AMBA_I2C_ACK;
            }
            dev->RxStatus = 0;
        } else {
            dev->status = I2C_STATUS_NACK;
        }
    } else {
        *(dev->Rxbuf)++ = (uint8_t)pAmbaI2C_MasterReg->Data.Value;
        dev->size_to_receive--;
        if (dev->size_to_receive == 0U) {
            AmbaRTSL_I2cStop(dev->ChannelId | 0x10);
            dev->status = I2C_STATUS_DONE;
        } else if (dev->size_to_receive == 1U) {
            /* One byte remains */
            pAmbaI2C_MasterReg->Ctrl = AMBA_I2C_NACK;
        } else {
            /* More than one byte remains */
            pAmbaI2C_MasterReg->Ctrl = AMBA_I2C_ACK;
        }
    }
}

const struct sigevent *Amba_I2c_intr(void *area, int id)
{
    amba_i2c_dev_t *dev = area;
    uint32_t status_reg;

    (void) id;
    status_reg = in32(dev->regbase + AMBA_I2C_STATUS_OFFSET);

    if ((status_reg & AMBA_I2C_SR_OPMODE_MASK) == 0U) {
        /* Master Write Mode */
        I2C_MasterWriteIntHandler(dev);
    } else {
        /* Master Read Mode */
        I2C_MasterReadIntHandler(dev);
    }

    if ((dev->status != I2C_STATUS_DONE) && (dev->status != I2C_STATUS_NACK)) {
        /* still have data to send/recieve */
        return NULL;
    }

    /* transfer done */
    if (dev->stop) {
        AmbaRTSL_I2cStop(dev->ChannelId);
    }

    return &dev->intrevent;
}

void Amba_I2c_WaitComplete(amba_i2c_dev_t *dev)
{
    uint64_t ntime = 500000000ULL;

    dev->status = I2C_STATUS_ERROR;
    TimerTimeout(CLOCK_MONOTONIC, _NTO_TIMEOUT_INTR, NULL, &ntime, NULL);
    InterruptWait_r(0, NULL);
    InterruptUnmask(dev->intr, dev->iid);
}
