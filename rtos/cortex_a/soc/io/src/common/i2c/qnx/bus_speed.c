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
#include <math.h>
#include <hw/ambarella_clk.h>

static void I2C_Config(void *hdl, unsigned int I2cSpeed)
{
    AMBA_I2C_REG_s *pI2cReg = pAmbaI2C_MasterReg;
    amba_i2c_dev_t *dev = hdl;

    static const unsigned int I2cSpeedFactor[4U] = {
        [I2C_SPEED_STANDARD]   = 400000U,   /* for I2C Standard speed: 100Kbps */
        [I2C_SPEED_FAST]       = 1320000U,  /* for I2C Fast speed: 330Kbps */
        [I2C_SPEED_FAST_PLUS]  = 4000000U,  /* for I2C Fast-mode Plus speed: 1Mbps */
        [I2C_SPEED_HIGH]       = 13600000U  /* for I2C High-speed mode: 3.4Mbps */
    };
    unsigned int PrescalerVal;
    int fd;
    int err;
    clk_freq_t clk_freq;
    unsigned bus_freq = I2cSpeedFactor[I2cSpeed];

    /* setup source clock */
    fd = open("/dev/clock", O_RDWR);
    if (fd == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Can't attach to /dev/clock manager. Use default clocks", __FUNCTION__);
    } else {
        clk_freq.freq = 0;
        clk_freq.id = AMBA_CLK_APB;
    }

    /* Get APB clock */
    err = devctl(fd, DCMD_CLOCK_GET_FREQ, &clk_freq, sizeof(clk_freq_t), NULL);
    if (err) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Clock GET devctl failed: %s", __FUNCTION__, strerror(errno));
    }

    if (clk_freq.freq == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: /dev/clock: Invalid frequency (-1)", __FUNCTION__);
    } else {
        /* Set real frequency */
        bus_freq = clk_freq.freq;
    }
    close(fd);

    /*
     * I2C period = (APB bus clock) / (4 * (prescaler[15:0] + 1) + 2)
     * 4 * (prescaler[15:0] + 1) = ((APB bus clock) / I2C period) - 2
     * prescaler[15:0] = ((((APB bus clock) / I2C period) - 2) / 4) - 1
     * prescaler[15:0] = (((APB bus clock) / ((I2C Period) * 4))) - 0.5) - 1
     */
    PrescalerVal = (bus_freq / I2cSpeedFactor[I2cSpeed]) - 1U;
    pI2cReg->PrescalerLowByte.Value = (PrescalerVal & 0xffU);
    pI2cReg->PrescalerHighByte.Value = ((PrescalerVal >> 8U) & 0xffU);

    dev->speed = I2cSpeed;
}

int Amba_I2c_SetSpeed(void *hdl, unsigned int speed, unsigned int *ospeed)
{
    unsigned int SpeedTmp = I2C_SPEED_STANDARD;
    unsigned int ActualSpeed;

    if (speed <= 100000U) {
        SpeedTmp = I2C_SPEED_STANDARD;
        ActualSpeed = 100000;
    } else if (speed <= 400000U) {
        SpeedTmp = I2C_SPEED_FAST;
        ActualSpeed = 330000;
    } else if (speed <= 1000000U) {
        SpeedTmp = I2C_SPEED_FAST_PLUS;
        ActualSpeed = 1000000;
    } else {
        SpeedTmp = I2C_SPEED_HIGH;
        ActualSpeed = 3400000;
    }

    I2C_Config(hdl, SpeedTmp);

    if (ospeed != NULL) {
        *ospeed = ActualSpeed;
    }

    return 0;
}

