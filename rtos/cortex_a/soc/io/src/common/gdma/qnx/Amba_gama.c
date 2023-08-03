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

#include "Amba_gdma.h"

#include "AmbaRTSL_GDMA.h"
#include "AmbaCSL_GDMA.h"

AMBA_GDMA_REG_s *pAmbaGDMA_Reg;

int Amba_Gdma_Init(void)
{
    uintptr_t virt_base;

    virt_base = mmap_device_io(AMBA_GDMA_SIZE, AMBA_GDMA_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaGDMA_Reg = (AMBA_GDMA_REG_s *)virt_base;

    AmbaRTSL_GdmaInit();

    return 0;
}

void Amba_Gdma_Fini(void)
{
    munmap_device_io((_Uintptrt)pAmbaGDMA_Reg, AMBA_GDMA_SIZE);
}

