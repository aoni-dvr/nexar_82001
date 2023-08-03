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

int AMBA_I2c_options(amba_i2c_dev_t *dev, int argc, char *argv[])
{
    int c;
    int prev_optind;
    int done = 0;

    /* defaults */
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    dev->intr = AMBA_INT_SPI_ID046_I2C_MASTER0;
#else
    dev->intr = AMBA_INT_SPI_ID47_I2C_MASTER0;
#endif
    dev->iid = -1;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    dev->physbase = AMBA_CA53_I2C0_BASE_ADDR;
#else
    dev->physbase = AMBA_I2C0_BASE_ADDR;
#endif
    dev->reglen = AMBA_I2C_REG_SIZE;
    dev->verbose = 0;
    dev->ChannelId = 0U;

    while (!done) {
        prev_optind = optind;
        c = getopt(argc, argv, "v:C:i");
        switch (c) {
        case 'i':
            dev->intr = strtol(optarg, &optarg, 0);
            break;
        case 'C':
            dev->ChannelId = strtol(optarg, &optarg, 0);
            break;

        case 'v':
            dev->verbose++;
            break;

        case '?':
            if (optopt == '-') {
                ++optind;
                break;
            }
            return -1;

        case -1:
            if (prev_optind < optind) /* -- */
                return -1;

            if (argv[optind] == NULL) {
                done = 1;
                break;
            }
            if (*argv[optind] != '-') {
                ++optind;
                break;
            }
            return -1;

        case ':':
        default:
            return -1;
        }
    }

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    switch (dev->ChannelId) {
    case AMBA_I2C_CHANNEL0:
        dev->intr = AMBA_INT_SPI_ID046_I2C_MASTER0;
        dev->physbase = AMBA_CA53_I2C0_BASE_ADDR;
        break;
    case AMBA_I2C_CHANNEL1:
        dev->intr = AMBA_INT_SPI_ID047_I2C_MASTER1;
        dev->physbase = AMBA_CA53_I2C1_BASE_ADDR;
        break;
    case AMBA_I2C_CHANNEL2:
        dev->intr = AMBA_INT_SPI_ID048_I2C_MASTER2;
        dev->physbase = AMBA_CA53_I2C2_BASE_ADDR;
        break;
    case AMBA_I2C_CHANNEL3:
        dev->intr = AMBA_INT_SPI_ID049_I2C_MASTER3;
        dev->physbase = AMBA_CA53_I2C3_BASE_ADDR;
        break;
    case AMBA_I2C_CHANNEL4:
        dev->intr = AMBA_INT_SPI_ID050_I2C_MASTER4;
        dev->physbase = AMBA_CA53_I2C4_BASE_ADDR;
        break;
    case AMBA_I2C_CHANNEL5:
        dev->intr = AMBA_INT_SPI_ID051_I2C_MASTER5;
        dev->physbase = AMBA_CA53_I2C5_BASE_ADDR;
        break;
    default:
        return -1;
        break;
    }
#else
    switch (dev->ChannelId) {
    case AMBA_I2C_CHANNEL0:
        dev->intr = AMBA_INT_SPI_ID47_I2C_MASTER0;
        dev->physbase = AMBA_I2C0_BASE_ADDR;
        break;
    case AMBA_I2C_CHANNEL1:
        dev->intr = AMBA_INT_SPI_ID48_I2C_MASTER1;
        dev->physbase = AMBA_I2C1_BASE_ADDR;
        break;
    case AMBA_I2C_CHANNEL2:
        dev->intr = AMBA_INT_SPI_ID49_I2C_MASTER2;
        dev->physbase = AMBA_I2C2_BASE_ADDR;
        break;
    case AMBA_I2C_CHANNEL3:
        dev->intr = AMBA_INT_SPI_ID50_I2C_MASTER3;
        dev->physbase = AMBA_I2C3_BASE_ADDR;
        break;
    default:
        return -1;
        break;
    }
#endif

    return 0;
}
