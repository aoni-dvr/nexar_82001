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
#include "Amba_dramc.h"

static uintptr_t dram_virt_base;
AMBA_DRAMC_REG_s *pAmbaDRAMC_Reg;

int Amba_Dram_Init(void)
{
    unsigned char i = 0;

    dram_virt_base = mmap_device_io(0x2000, 0xdffe0000);
    if (dram_virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaDRAMC_Reg = (AMBA_DRAMC_REG_s *)dram_virt_base;

    return 0;
}

void Amba_Dram_Fini(void)
{
    munmap_device_io(0xdffe0000, 0x2000);
}

