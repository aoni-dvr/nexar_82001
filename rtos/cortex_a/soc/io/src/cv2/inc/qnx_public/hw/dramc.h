/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, 2018, QNX Software Systems.
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

#ifndef _HW_DRAM_DRIVER_H_INCLUDED
#define _HW_DRAM_DRIVER_H_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif
#ifndef __NEUTRINO_H_INCLUDED
#include <sys/neutrino.h>
#endif

#include <stdint.h>

#include "AmbaDRAMC_Def.h"

#include <devctl.h>

#define _DCMD_DRAM   _DCMD_MISC

#define DCMD_DRAM_RESET                  __DION(_DCMD_DRAM, 0)
#define DCMD_DRAM_GET_STATIS             __DIOTF(_DCMD_DRAM, 1, AMBA_DRAMC_STATIS_s)

#endif /* _HW_DRAM_H_INCLUDED */
