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

#include "Amba_wdt.h"

#include "AmbaCSL_WDT.h"

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
AMBA_WDT_REG_s * pAmbaWDT_Reg[6];
#else
AMBA_WDT_REG_s * pAmbaWDT_Reg;
#endif
AMBA_RCT_REG_s * pAmbaRCT_Reg;

int Amba_Wdt_Init(void)
{
    uintptr_t virt_base;

    virt_base = mmap_device_io(AMBA_WDT_SIZE, AMBA_WDT_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    pAmbaWDT_Reg[0] = (AMBA_WDT_REG_s *)virt_base;

    virt_base = mmap_device_io(AMBA_WDT_SIZE, AMBA_WDT1_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaWDT_Reg[1] = (AMBA_WDT_REG_s *)virt_base;
    virt_base = mmap_device_io(AMBA_WDT_SIZE, AMBA_WDT2_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaWDT_Reg[2] = (AMBA_WDT_REG_s *)virt_base;
    virt_base = mmap_device_io(AMBA_WDT_SIZE, AMBA_WDT3_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaWDT_Reg[3] = (AMBA_WDT_REG_s *)virt_base;
    virt_base = mmap_device_io(AMBA_WDT_SIZE, AMBA_WDT4_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaWDT_Reg[4] = (AMBA_WDT_REG_s *)virt_base;
    virt_base = mmap_device_io(AMBA_WDT_SIZE, AMBA_WDT5_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaWDT_Reg[5] = (AMBA_WDT_REG_s *)virt_base;
#else
    pAmbaWDT_Reg = (AMBA_WDT_REG_s *)virt_base;
#endif
    virt_base = mmap_device_io(AMBA_RCT_SIZE, AMBA_RCT_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaRCT_Reg = (AMBA_RCT_REG_s *)virt_base;

    AmbaRTSL_Wdt_Init();

    return 0;
}

void Amba_Wdt_Fini(void)
{
    munmap_device_io(AMBA_WDT_BASE, AMBA_WDT_SIZE);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    munmap_device_io(AMBA_WDT1_BASE, AMBA_WDT_SIZE);
    munmap_device_io(AMBA_WDT2_BASE, AMBA_WDT_SIZE);
    munmap_device_io(AMBA_WDT3_BASE, AMBA_WDT_SIZE);
    munmap_device_io(AMBA_WDT4_BASE, AMBA_WDT_SIZE);
    munmap_device_io(AMBA_WDT5_BASE, AMBA_WDT_SIZE);
#endif
    munmap_device_io(AMBA_RCT_BASE, AMBA_RCT_SIZE);
}

