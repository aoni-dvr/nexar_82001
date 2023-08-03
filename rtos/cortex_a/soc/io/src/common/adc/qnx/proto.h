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

#ifndef __PROTO_H_INCLUDED
#define __PROTO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <sys/hwinfo.h>
#include <drvr/hwinfo.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <arm/ambarella.h>
#include <fcntl.h>
#include "Amba_adc.h"

typedef struct _adc_functions {
    int (*init)(void);
    void (*fini)(void);

    int (*adc_get_singleread)(uint32_t AdcCh, uint32_t *pData);
    int (*adc_set_samplerate)(uint32_t *pSampleRate, uint32_t *pActiveChanList);
    int (*adc_get_info)(uint32_t *pSampleRate, uint32_t *pActiveChanList);
    int (*adc_get_seamlessread)(uint32_t AdcCh, uint32_t BufSize, uint32_t *pBuf);
    int (*adc_set_stop)(uint32_t *pSampleRate, uint32_t *pActiveChanList);
} adc_functions_t;

typedef int (*get_adcfuncs_t)(adc_functions_t *funcs, int tabsize);

/* Macro used by H/W driver when populating adc_functions table */
#define ADC_ADD_FUNC(table, entry, func, limit) \
    if ((int) ((size_t)&(((adc_functions_t *)0)->entry)) + \
        (int) sizeof (void (*)()) <= (limit)) \
        (table)->entry = (func);

/** @name function format
 */
int get_adcfuncs(adc_functions_t *functable, int tabsize);

#endif
