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

#ifndef _HW_GPIO_DRIVER_H_INCLUDED
#define _HW_GPIO_DRIVER_H_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif
#ifndef __NEUTRINO_H_INCLUDED
#include <sys/neutrino.h>
#endif

#include <stdint.h>

#include "AmbaGPIO_Def.h"

typedef struct _gpio_pin_t {
    unsigned int PinId;
    unsigned int PinValue;
} gpio_pin_t;

typedef struct _gpio_pin_info_t {
    unsigned int PinId;
    AMBA_GPIO_INFO_s PinInfo;
} gpio_pin_info_t;

#include <devctl.h>

#define _DCMD_GPIO   _DCMD_MISC

#define DCMD_GPIO_SET_GPI                __DIOT(_DCMD_GPIO, 0, gpio_pin_t)
#define DCMD_GPIO_SET_GPO                __DIOT(_DCMD_GPIO, 1, gpio_pin_t)
#define DCMD_GPIO_SET_FUNC               __DIOT(_DCMD_GPIO, 2, gpio_pin_t)
#define DCMD_GPIO_SET_PULL               __DIOT(_DCMD_GPIO, 3, gpio_pin_t)
#define DCMD_GPIO_GET_INFO               __DIOTF(_DCMD_GPIO, 4, gpio_pin_info_t)

typedef struct _gpio_functions {
    int (*init)(void);
    void (*fini)(void);

    int (*gpio_set_direction)(unsigned int pin, unsigned int dir);
    int (*gpio_get_direction)(unsigned int pin);
    int (*gpio_set_output_enable)(unsigned int pin, unsigned int value);
    int (*gpio_get_output_enable)(unsigned int pin);
    int (*gpio_get_input)(unsigned int pin);
    int (*gpio_get_irq_type)(unsigned int pin);
    int (*gpio_set_irq_type)(unsigned int pin, unsigned int irq_type);
    int (*gpio_irq_clear)(unsigned int pin);
    int (*gpio_get_irq_enable)(unsigned int pin);
    int (*gpio_irq_enable)(unsigned int pin);
    int (*gpio_irq_disable)(unsigned int pin);

    int (*gpio_get_pin_pull_enable)(unsigned int pin);
    int (*gpio_get_pin_pull_level)(unsigned int pin);
    unsigned int (*gpio_get_pin_func)(unsigned int pin_number);
    int (*gpio_set_pin_func)(unsigned int pin_number);
    int (*gpio_set_pin_pull_level)(unsigned int pin_number, unsigned int up_down);
} gpio_functions_t;

/* Macro used by H/W driver when populating gpio_functions table */
#define GPIO_ADD_FUNC(table, entry, func, limit) \
    if ((int)((size_t)&(((gpio_functions_t *)0)->entry)) + \
        (int)sizeof (void (*)()) <= (limit)) \
        (table)->entry = (func);

typedef int (*get_gpiofuncs_t)(gpio_functions_t *funcs, int tabsize);

int get_gpiofuncs(gpio_functions_t *funcs, int tabsize);

#endif /* _HW_GPIO_H_INCLUDED */
