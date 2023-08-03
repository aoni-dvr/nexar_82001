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


#ifndef _VARIANT_H
#define _VARIANT_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <hw/inout.h>

#include "amba_gqspi.h"

/* MT25Q Family specs */
#define ADDR_LEN             4        // 32 bit address
#define CMD_LEN              1        // one byte commands

#define TOTAL_SIZE_BYTES    0x4000000 // Default chip size in bytes: 64M

/* According to the Device ID Data Table in Micron Flash chip
 * datasheet, the first byte is manufacturer ID. The second
 * and third bytes are device ID.
 *
 * ident[0]: 0x20 - Micron
 * ident[1]: 0xBB - 1.8V
 *           0xBA - 3V
 * ident[2]: 0x19 - 256Mb
 *           0x20 - 512Mb
 *           0x21 - 1Gb
 * ident[3] - ident[19]: Unique ID
 */

#define IDENT_LEN            20       // Device ident data length
#define SFDP_LEN             0x88     // Serial Flash Discovery Parameters length

/* Offset of each parameter in device ID data  */
#define IDENT_MID            0
#define IDENT_MEMTYPE        1
#define IDENT_MEMCAP         2
#define IDENT_UID            3

#define MAX_BURST           16384

#endif /* VARIANT_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/aarch64/le.zcu102/variant.h $ $Rev: 859471 $");
#endif
