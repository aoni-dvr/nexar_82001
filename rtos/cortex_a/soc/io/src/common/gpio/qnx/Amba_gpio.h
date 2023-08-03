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

#ifndef AMBA_GPIO_H
#define AMBA_GPIO_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <sys/mman.h>
#include <sys/procmgr.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <hw/inout.h>
#include <hw/gpio.h>

#include <arm/ambarella.h>
#include "AmbaCortexA53.h"
#include "AmbaTypes.h"
#include "AmbaRTSL_GPIO.h"
#include "AmbaRTSL_GPIO_Ctrl.h"
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#include "AmbaCSL_Scratchpad.h"
#else
#include "AmbaReg_MISC.h"
#endif
#include "AmbaReg_GPIO.h"
#include "AmbaReg_IoMux.h"
#include "AmbaReg_RCT.h"

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_DUMP(s)          printf(s)
#else
#define DEBUG_DUMP(...)
#endif

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define AMBA_GPIO0_BASE             AMBA_CA53_GPIO0_BASE_ADDR
#define AMBA_GPIO1_BASE             AMBA_CA53_GPIO1_BASE_ADDR
#define AMBA_GPIO2_BASE             AMBA_CA53_GPIO2_BASE_ADDR
#define AMBA_GPIO3_BASE             AMBA_CA53_GPIO3_BASE_ADDR
#else
#define AMBA_GPIO0_BASE             AMBA_CORTEX_A53_GPIO0_BASE_ADDR
#define AMBA_GPIO1_BASE             AMBA_CORTEX_A53_GPIO1_BASE_ADDR
#define AMBA_GPIO2_BASE             AMBA_CORTEX_A53_GPIO2_BASE_ADDR
#define AMBA_GPIO3_BASE             AMBA_CORTEX_A53_GPIO3_BASE_ADDR
#define AMBA_GPIO4_BASE             AMBA_CORTEX_A53_GPIO4_BASE_ADDR
#define AMBA_GPIO5_BASE             AMBA_CORTEX_A53_GPIO5_BASE_ADDR
#endif

#define AMBA_GPIO_REGISTER_SIZE     0x1000

int gpio_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);

int Amba_Gpio_Set_Direction(unsigned int pin, unsigned int dir);
int Amba_Gpio_Get_Direction(unsigned int pin);
int Amba_Gpio_Set_Output_Enable(unsigned int pin, unsigned int value);
int Amba_Gpio_Get_Output_Enable(unsigned int pin);
int Amba_Gpio_Get_Input(unsigned int pin);
int Amba_Gpio_Get_Irq_Type(unsigned int pin);
int Amba_Gpio_Set_Irq_Type(unsigned int pin_number, unsigned int irq_type);
int Amba_Gpio_Irq_Clear(unsigned int pin);
int Amba_Gpio_Get_Irq_Enable(unsigned int pin);
int Amba_Gpio_Irq_Enable(unsigned int pin);
int Amba_Gpio_Irq_Disable(unsigned int pin_number);

int Amba_Get_Pin_Pull_Enable(unsigned int pin_number);
int Amba_Get_Pin_Pull_UpDown(unsigned int pin_number);
unsigned int Amba_Get_Pin_Func(unsigned int pin_number);
int Amba_Set_Pin_Func(unsigned int pin_number);
int Amba_Set_Pin_Pull_UpDown(unsigned int pin_number, unsigned int up_down);

int Amba_Gpio_Init(void);
void Amba_Gpio_Fini(void);

int Amba_Gpio_Set_Default(const AMBA_GPIO_DEFAULT_s * pDefaultParam);

#endif
