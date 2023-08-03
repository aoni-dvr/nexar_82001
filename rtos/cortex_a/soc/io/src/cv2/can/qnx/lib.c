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

int can_dev_init(UINT32 CanCh)
{
    void* VirtualAddr;
    UINT64 Addr;

    switch (CanCh) {
    case AMBA_CAN_CHANNEL0:
        Addr = AMBA_CAN0_BASE;
        break;
    case AMBA_CAN_CHANNEL1:
        Addr = AMBA_CAN1_BASE;
        break;
    default:
        break;
    }


    if ((VirtualAddr = mmap_device_memory( NULL, AMBA_CAN_SIZE,
                                           PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                           Addr)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to can register region",
              __FUNCTION__);
        return -1;
    } else {
        pAmbaCAN_Reg[CanCh] =  (AMBA_CAN_REG_s *) VirtualAddr;

    }

    return 0;
}

int can_dev_dinit(UINT32 CanCh)
{
    // deinit controller

    if (munmap_device_memory((void*)pAmbaCAN_Reg[CanCh], AMBA_CAN_SIZE) < 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot free memory",
              __FUNCTION__);
        return -1;
    }

    return 0;
}
