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

#ifndef _HW_GDMA_DRIVER_H_INCLUDED
#define _HW_GDMA_DRIVER_H_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif
#ifndef __NEUTRINO_H_INCLUDED
#include <sys/neutrino.h>
#endif

#include <stdint.h>

#include "AmbaGDMA_Def.h"

typedef struct _amba_timer_t {
    void    *pSrcImg;
    void    *pDstImg;
    unsigned int NumPixels;
    unsigned int PixelFormat;
} amba_gdma_linear_t;

typedef struct {
    void    *pSrcImg;                       /* Virtual address */
    void    *pDstImg;                       /* Virtual address */
    unsigned int SrcRowStride;              /* Rowstride of the source image (bytes) */
    unsigned int DstRowStride;              /* Rowstride of the destination image (bytes) */
    unsigned int BltWidth;                  /* Width of the block transfer (in number of pixels) */
    unsigned int BltHeight;                 /* Height of the block transfer (in number of rows) */
    unsigned int PixelFormat;
    unsigned int TransparentColor;
    unsigned int AlphaVal;
    unsigned int BlendMode;
} amba_gdma_block_t;

typedef struct {
    unsigned int TimeOut;
} amba_gdma_wait_t;

#include <devctl.h>

#define _DCMD_GDMA   _DCMD_MISC

#define DCMD_GDMA_LINEAR        __DIOT(_DCMD_GDMA, 0, amba_gdma_linear_t)
#define DCMD_GDMA_BLOCK         __DIOT(_DCMD_GDMA, 1, amba_gdma_block_t)
#define DCMD_GDMA_COLOTKEY      __DIOT(_DCMD_GDMA, 2, amba_gdma_block_t)
#define DCMD_GDMA_ALPHABLEND    __DIOT(_DCMD_GDMA, 3, amba_gdma_block_t)
#define DCMD_GDMA_WAIT_COMPLETE     __DIOT(_DCMD_GDMA, 4, amba_gdma_wait_t)

#endif /* _HW_GDMA_DRIVER_H_INCLUDED */
