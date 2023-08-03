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

#ifndef _HW_TIMER_DRIVER_H_INCLUDED
#define _HW_TIMER_DRIVER_H_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif
#ifndef __NEUTRINO_H_INCLUDED
#include <sys/neutrino.h>
#endif

#include <stdint.h>

#include "AmbaWDT_Def.h"

typedef struct _amba_wdt_t {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    unsigned int Channel;
#endif
    unsigned int CountDown;
    unsigned int ResetIrqPulseWidth;
    AMBA_WDT_ISR_f Handler;
    unsigned int HandlerArgv;
} amba_wdt_t;

typedef struct _amba_wdt_info_t {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    unsigned int Channel;
#endif
    uint32_t TimerExpired;
    uint32_t ExpireAction;
    uint32_t ResetStatus;
} amba_wdt_info_t;

#include <devctl.h>

#define _DCMD_WDT   _DCMD_MISC

#define DCMD_WDT_FEED     __DIOT(_DCMD_WDT, 0, amba_wdt_t)
#define DCMD_WDT_START    __DIOT(_DCMD_WDT, 1, amba_wdt_t)
#define DCMD_WDT_STOP     __DIOT(_DCMD_WDT, 2, amba_wdt_t)
#define DCMD_WDT_HOOKHAND __DIOT(_DCMD_WDT, 3, amba_wdt_t)
#define DCMD_WDT_CLEAN    __DIOT(_DCMD_WDT, 4, amba_wdt_t)
#define DCMD_WDT_GET_INFO   __DIOTF(_DCMD_WDT, 5, amba_wdt_info_t)
#define DCMD_WDT_RET_STATUS __DIOTF(_DCMD_WDT, 6, amba_wdt_info_t)
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define DCMD_WDT_SETPATTERN __DIOT(_DCMD_WDT, 7, amba_wdt_t)
#endif

#endif /* _HW_GPIO_H_INCLUDED */
