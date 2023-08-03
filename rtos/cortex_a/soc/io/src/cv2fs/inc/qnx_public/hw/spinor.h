/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, 2018, QNX Software Systems.
 * Copyright 2013, Adeneo Embedded.
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

#ifndef _HW_SPINOR_DRIVER_H_INCLUDED
#define _HW_SPINOR_DRIVER_H_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif
#ifndef __NEUTRINO_H_INCLUDED
#include <sys/neutrino.h>
#endif

#include <stdint.h>

#include "AmbaSpiNOR_Def.h"

typedef struct _amba_spinor_config_t {
    unsigned int    Addr;
    unsigned int    Offset;
    unsigned int    Cmd;
    unsigned int    DataLen;
    unsigned int    TimeOut;
    void    *pDstAddr;              /* pointer to the destination */
} amba_spinor_config_t;

#include <devctl.h>

#define _DCMD_SPINOR   _DCMD_MISC

#define DCMD_SPINOR_LOCK    __DIOT(_DCMD_SPINOR, 0, amba_spinor_config_t)
#define DCMD_SPINOR_UNLOCK  __DIOT(_DCMD_SPINOR, 1, amba_spinor_config_t)
#define DCMD_SPINOR_WAIT    __DIOT(_DCMD_SPINOR, 2, amba_spinor_config_t)

#endif /* _HW_DMA_H_INCLUDED */
