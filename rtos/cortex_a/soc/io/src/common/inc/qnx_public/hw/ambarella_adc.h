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

#ifndef _HW_ADC_DRIVER_H_INCLUDED
#define _HW_ADC_DRIVER_H_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif
#ifndef __NEUTRINO_H_INCLUDED
#include <sys/neutrino.h>
#endif

#include <stdint.h>

#include "AmbaADC_Def.h"

#include <devctl.h>

typedef struct _adc_ch_t {
    UINT32 AdcCh;
    UINT32 ChData;
    UINT32 SampleRate;
    UINT32 ActiveChanList;
    UINT32 BufSize;
    UINT32 *pBuffer;
    UINT32 *pActualSize;
} adc_ch_t;

#define _DCMD_ADC   _DCMD_MISC

#define DCMD_ADC_SET_SAMPLERATE         __DIOT(_DCMD_ADC, 0, adc_ch_t)
#define DCMD_ADC_GET_SINGLEREAD         __DIOTF(_DCMD_ADC, 1, adc_ch_t)
#define DCMD_ADC_GET_INFO               __DIOTF(_DCMD_ADC, 2, adc_ch_t)
#define DCMD_ADC_SEAMLESS_READ          __DIOTF(_DCMD_ADC, 3, adc_ch_t)
#define DCMD_ADC_STOP                   __DIOTF(_DCMD_ADC, 4, adc_ch_t)

#endif /* _HW_ADC_H_INCLUDED */
