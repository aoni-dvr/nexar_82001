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

#ifndef _HW_PWM_DRIVER_H_INCLUDED
#define _HW_PWM_DRIVER_H_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif
#ifndef __NEUTRINO_H_INCLUDED
#include <sys/neutrino.h>
#endif

#include <stdint.h>

#include "AmbaPWM_Def.h"

#include <devctl.h>

typedef struct _pwm_ch_t {
    UINT32 PwmCh;
    UINT32 ChData;
    UINT32 Freq;
    UINT32 Period;
    UINT32 Duty;
    UINT32 ActualBaseFreq;
    UINT32 PinGrpID;
    UINT32 BaseFreq;
    UINT32 PulseWidth;
    UINT32 PinID;
} pwm_ch_t;

#define _DCMD_PWM   _DCMD_MISC

#define DCMD_PWM_SET_CONFIG         __DIOTF(_DCMD_PWM, 0, pwm_ch_t)
#define DCMD_PWM_START              __DIOTF(_DCMD_PWM, 1, pwm_ch_t)
#define DCMD_PWM_STOP               __DIOTF(_DCMD_PWM, 2, pwm_ch_t)
#define DCMD_PWM_GET_INFO           __DIOTF(_DCMD_PWM, 3, pwm_ch_t)
#define DCMD_PWM_SET_STEPCONFIG     __DIOTF(_DCMD_PWM, 4, pwm_ch_t)
#define DCMD_PWM_SET_STEPDUTY       __DIOTF(_DCMD_PWM, 5, pwm_ch_t)
#define DCMD_PWM_SET_STEP_ACT       __DIOTF(_DCMD_PWM, 6, pwm_ch_t)
#define DCMD_PWM_GET_STEP_STATUS    __DIOTF(_DCMD_PWM, 7, pwm_ch_t)

#endif /* _HW_PWM_H_INCLUDED */
