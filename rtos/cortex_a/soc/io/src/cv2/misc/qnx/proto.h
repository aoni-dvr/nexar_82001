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

#ifndef __PROTO_H_INCLUDED
#define __PROTO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/iofunc.h>
#include <sys/resmgr.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <errno.h>
#include <sys/procmgr.h>
#include <drvr/hwinfo.h>
#include <string.h>

#include <AmbaCortexA53.h>
#include <AmbaReg_RCT.h>
#include <AmbaReg_Scratchpad.h>
#include <AmbaReg_USB.h>
#include <hw/ambarella_misc.h>

#define AmbaMisra_TouchUnused(x)
#define AmbaMisra_TypeCast32(a, b) memcpy((void *)a, (void *)b, 4)

#define AMBA_RCT_BASE_ADDR                     0xED080000
#define AMBA_SCRATCHPAD_BASE_ADDR              AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR
#define AMBA_USB_BASE_ADDR                     AMBA_CORTEX_A53_USB_BASE_ADDR
#define AMBA_RCT_MMAP_SIZE                     0x00001000
#define AMBA_SCRATCHPAD_MMAP_SIZE              0x00000100
#define AMBA_USB_MMAP_SIZE                     0x00001000

int misc_dev_init(void);
int misc_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);
int misc_io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);
int misc_dev_dinit(void);

#define slogerr(...)  slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, __VA_ARGS__)
#define sloginfo(...)  slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, __VA_ARGS__)

#endif

