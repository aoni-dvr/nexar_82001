/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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

#include <math.h>
#include <proto.h>
#include <AmbaWrap.h>
#include <AmbaKAL.h>
#include <hw/ambarella_clk.h>


int rng_dev_init(void)
{
    void* VirtualAddr;

    if ((VirtualAddr = mmap_device_memory( NULL, AMBA_RCT_MMAP_SIZE,
                                           PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                           AMBA_RCT_BASE_ADDR)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to rct register region",
              __FUNCTION__);
        return -1;
    } else {
        pAmbaRCT_Reg =  (AMBA_RCT_REG_s *) VirtualAddr;
        AmbaCSL_RngRctBaseAddressSet((UINT64)pAmbaRCT_Reg);
    }

    if ((VirtualAddr = mmap_device_memory( NULL, AMBA_SCRATCHPADS_MMAP_SIZE,
                                           PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                           AMBA_SCRATCHPADS_BASE_ADDR)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to rng register region",
              __FUNCTION__);
        return -1;
    } else {
        pAmbaScratchpadS_Reg = (AMBA_SCRATCHPAD_REG_s *) VirtualAddr;
        AmbaCSL_RngSpBaseAddressSet((UINT64)pAmbaScratchpadS_Reg);
    }

    return 0;
}

int rng_dev_dinit(void)
{
    if (munmap_device_memory((void*)pAmbaRCT_Reg, AMBA_RCT_MMAP_SIZE) < 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot free memory",
              __FUNCTION__);
        return -1;
    }

    if (munmap_device_memory((void*)pAmbaScratchpadS_Reg, AMBA_SCRATCHPADS_MMAP_SIZE) < 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot free memory",
              __FUNCTION__);
        return -1;
    }

    return 0;
}
