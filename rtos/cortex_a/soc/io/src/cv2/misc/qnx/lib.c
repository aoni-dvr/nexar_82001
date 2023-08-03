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

#include <proto.h>
#include <AmbaWrap.h>
#include <AmbaKAL.h>
#include <hw/ambarella_misc.h>
#include <AmbaCSL_USB.h>

AMBA_RCT_REG_s        *pAmbaRCT_Reg;
AMBA_SCRATCHPAD_REG_s *pAmbaScratchpadS_Reg;
AMBA_USB_REG_s        *pAmbaUSB_Reg;

int misc_dev_init(void)
{
    void* virtual_addr;
    CSL_USB_HW_SETTING_s csl_hws = {0U};

    if ((virtual_addr = mmap_device_memory( NULL, AMBA_RCT_MMAP_SIZE,
                                            PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                            AMBA_RCT_BASE_ADDR)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to RCT register region",
              __FUNCTION__);
        return -1;
    } else {
        pAmbaRCT_Reg =  (AMBA_RCT_REG_s *) virtual_addr;
    }

    if ((virtual_addr = mmap_device_memory( NULL, AMBA_SCRATCHPAD_MMAP_SIZE,
                                            PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                            AMBA_SCRATCHPAD_BASE_ADDR)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to ScratchPad register region",
              __FUNCTION__);
        return -1;
    } else {
        pAmbaScratchpadS_Reg =  (AMBA_SCRATCHPAD_REG_s *) virtual_addr;
    }

    if ((virtual_addr = mmap_device_memory( NULL, AMBA_USB_MMAP_SIZE,
                                            PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                            AMBA_USB_BASE_ADDR)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to USB register region",
              __FUNCTION__);
        return -1;
    } else {
        pAmbaUSB_Reg =  (AMBA_USB_REG_s *) virtual_addr;
    }

    csl_hws.Rct.BaseAddress = (UINT64)pAmbaRCT_Reg;
    AmbaCSL_UsbSetHwInfo(&csl_hws);

    return 0;
}

int misc_dev_dinit(void)
{
    if (munmap_device_memory((void*)AMBA_RCT_BASE_ADDR, AMBA_RCT_MMAP_SIZE) < 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot free RCT memory region",
              __FUNCTION__);
        return -1;
    }

    if (munmap_device_memory((void*)AMBA_SCRATCHPAD_BASE_ADDR, AMBA_SCRATCHPAD_MMAP_SIZE) < 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot free ScratchPad memory region",
              __FUNCTION__);
        return -1;
    }

    if (munmap_device_memory((void*)AMBA_USB_BASE_ADDR, AMBA_USB_MMAP_SIZE) < 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot free USB memory region",
              __FUNCTION__);
        return -1;
    }

    return 0;
}
