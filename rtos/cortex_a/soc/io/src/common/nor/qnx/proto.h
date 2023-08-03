/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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

#ifndef __PROTO_CLOCK_H_INCLUDED
#define __PROTO_CLOCK_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <errno.h>
#include <sys/procmgr.h>
#include <drvr/hwinfo.h>
#include <string.h>

#include "AmbaSYS.h"
#include "AmbaDebugPort.h"
#include "AmbaReg_RCT.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_ORC.h"
#include "AmbaCSL_DebugPort.h"

#include <hw/spinor.h>

#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaCortexA53.h"

#include "AmbaCSL_FIO.h"
#include "AmbaRTSL_FIO.h"

#define AMBA_CORC_BASE_ADDR             0xEd030000
#define AMBA_RCT_BASE_ADDR              0xEd080000

int spinor_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);

#endif

