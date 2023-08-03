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

#include <stdint.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <stdio.h>

#include "Amba_dma.h"

AMBA_DMA_REG_s * pAmbaDMA_Reg[2];

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
AMBA_SCRATCHPAD_NS_REG_s * pAmbaScratchpadNS_Reg = AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR;
#else
AMBA_RCT_REG_s * pAmbaRCT_Reg;
AMBA_SCRATCHPAD_REG_s * pAmbaScratchpadS_Reg;
#endif

int Amba_Dma_Init(void)
{
    uintptr_t virt_base;

    virt_base = mmap_device_io(AMBA_DMA_SIZE, AMBA_DMA0_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaDMA_Reg[0] = (AMBA_DMA_REG_s *)virt_base;

    virt_base = mmap_device_io(AMBA_DMA_SIZE, AMBA_DMA1_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaDMA_Reg[1] = (AMBA_DMA_REG_s *)virt_base;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    virt_base = mmap_device_io(AMBA_SCRATCHPADS_SIZE, AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR);
#else
    virt_base = mmap_device_io(AMBA_RCT_SIZE, AMBA_RCT_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaRCT_Reg = (AMBA_RCT_REG_s *)virt_base;

    virt_base = mmap_device_io(AMBA_SCRATCHPADS_SIZE, AMBA_SCRATCHPADS_BASE);
#endif
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    pAmbaScratchpadNS_Reg = (AMBA_SCRATCHPAD_NS_REG_s *)virt_base;
#else
    pAmbaScratchpadS_Reg = (AMBA_SCRATCHPAD_REG_s *)virt_base;
#endif
    AmbaRTSL_DmaInit();

    return 0;
}

void Amba_Dma_Fini(void)
{
    munmap_device_io(pAmbaDMA_Reg[0], AMBA_DMA_SIZE);
    munmap_device_io(pAmbaDMA_Reg[1], AMBA_DMA_SIZE);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    munmap_device_io(pAmbaScratchpadNS_Reg, AMBA_DMA_SIZE);
#else
    munmap_device_io(pAmbaRCT_Reg, AMBA_DMA_SIZE);
    munmap_device_io(pAmbaScratchpadS_Reg, AMBA_DMA_SIZE);
#endif
}

