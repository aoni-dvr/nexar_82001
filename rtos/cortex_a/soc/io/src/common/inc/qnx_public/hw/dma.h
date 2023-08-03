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

#ifndef _HW_DMA_DRIVER_H_INCLUDED
#define _HW_DMA_DRIVER_H_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif
#ifndef __NEUTRINO_H_INCLUDED
#include <sys/neutrino.h>
#endif

#include <stdint.h>

#include "AmbaDMA_Def.h"

typedef struct _amba_dma_config_t {
    unsigned int    DmaChanNo;
    AMBA_DMA_DESC_s DmaDesc;
    unsigned int    DmaChanType;
    unsigned int    TimeOut;
    unsigned int    IntCount;
} amba_dma_config_t;

typedef struct _amba_dma_des_config_t {
    unsigned int    DmaChanNo;
    unsigned int    DmaDescAddr;
} amba_dma_des_config_t;

#include <devctl.h>

#define _DCMD_DMA   _DCMD_MISC

#define DCMD_DMA_TRANSFER           __DIOT(_DCMD_DMA, 0, amba_dma_config_t)
#define DCMD_DMA_WAIT               __DIOT(_DCMD_DMA, 1, amba_dma_config_t)
#define DCMD_DMA_CHANNEL_ALLOCATE   __DIOTF(_DCMD_DMA, 2, amba_dma_config_t)
#define DCMD_DMA_RELEASE            __DIOT(_DCMD_DMA, 3, amba_dma_config_t)
#define DCMD_DMA_TRANSFER_DES       __DIOT(_DCMD_DMA, 4, amba_dma_des_config_t)
#define DCMD_DMA_GET_INT_COUNT      __DIOTF(_DCMD_DMA, 5, amba_dma_config_t)
#define DCMD_DMA_RESET_INIT_COUNT   __DIOT(_DCMD_DMA, 6, amba_dma_config_t)

#endif /* _HW_DMA_H_INCLUDED */
