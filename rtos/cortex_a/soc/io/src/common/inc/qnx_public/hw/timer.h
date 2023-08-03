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

#include "AmbaTMR_Def.h"

typedef struct _amba_timer_t {
    unsigned int TimerId;
    unsigned int TimerFreq;
    unsigned int NumPeriodicTick;
    unsigned int NumRemainTick;
    AMBA_TMR_ISR_f Handler;
    unsigned int HandlerArgv;
} amba_timer_t;

typedef struct {
    unsigned int TimerId;
    unsigned int SysFreq;
    unsigned int TimerFreq;
    unsigned int PeriodicInterval;
    unsigned int ExpireCount;
    unsigned int State;
} timer_info_t;

#include <devctl.h>

#define _DCMD_TIMER   _DCMD_MISC

#define DCMD_TIMER_CONFIG   __DIOT(_DCMD_TIMER, 0, amba_timer_t)
#define DCMD_TIMER_START    __DIOT(_DCMD_TIMER, 1, amba_timer_t)
#define DCMD_TIMER_STOP     __DIOT(_DCMD_TIMER, 2, amba_timer_t)
#define DCMD_TIMER_SHOW     __DIOTF(_DCMD_TIMER, 3, amba_timer_t)
#define DCMD_TIMER_GETINFO  __DIOTF(_DCMD_TIMER, 4, timer_info_t)
#define DCMD_TIMER_GETAVAIL __DIOTF(_DCMD_TIMER, 5, amba_timer_t)
#define DCMD_TIMER_LOCK     __DIOTF(_DCMD_TIMER, 6, amba_timer_t)
#define DCMD_TIMER_RELEASE  __DIOT(_DCMD_TIMER, 7, amba_timer_t)

#endif /* _HW_GPIO_H_INCLUDED */
