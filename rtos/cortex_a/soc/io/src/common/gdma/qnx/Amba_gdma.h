/*
 * $QNXLicenseC:
 * Copyright 2010, 2018, QNX Software Systems.
 * Copyright 2013, Adeneo Embedded.
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

#ifndef AMBA_TIMER_H
#define AMBA_TIMER_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/syspage.h>
#include <sys/mman.h>
#include <sys/procmgr.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <arm/ambarella.h>
#include <hw/gdma.h>

#include "AmbaCortexA53.h"
#include "AmbaRTSL_GDMA.h"
#include "AmbaReg_GDMA.h"
#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define AMBA_GDMA_BASE  AMBA_CA53_GDMA_BASE_ADDR
#else
#define AMBA_GDMA_BASE  AMBA_CORTEX_A53_GDMA_BASE_ADDR
#endif
#define AMBA_GDMA_SIZE  0x1000

int gdma_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);

int Amba_Gdma_Init(void);
void Amba_Gdma_Fini(void);

#endif //AMBA_TIMER_H
