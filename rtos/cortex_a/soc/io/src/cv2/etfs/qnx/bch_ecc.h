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

#ifndef BCH_ECC_H_
#define BCH_ECC_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @file       imx-micron/bch_ecc.h
 * @addtogroup etfs_bch ECC
 * @{
 */

#define BCH_SUBBLOCK_SIZE 1024 //!< BCH sub block size in bytes
#define BCH_ECC_SIZE      42   //!< BCH ECC size in bytes

/* Forward declaration */
typedef struct _chipio_t chipio;

#define BV_GPMI_ECCCTRL_ECC_CMD__DECODE_4_BIT  0x0
#define BV_GPMI_ECCCTRL_ECC_CMD__ENCODE_4_BIT  0x1
#define BV_GPMI_ECCCTRL_ECC_CMD__DECODE_8_BIT  0x2
#define BV_GPMI_ECCCTRL_ECC_CMD__ENCODE_8_BIT  0x3

void *bchint_thread(void *arg);

int bch_intr_wait(chipio *chipio);

void bch_init(chipio *chipio);

void bch_set_layout(chipio *chipio);

uint32_t bch_get_ecc_status(chipio *chipio);

void bch_set_erase_threshold(chipio *chipio, uint8_t threshold);

/** @}*/

#endif /* BCH_ECC_H_ */
