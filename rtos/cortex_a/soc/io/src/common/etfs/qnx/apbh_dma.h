/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
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

#ifndef APBH_DMA_H_
#define APBH_DMA_H_

#include <stdbool.h>
#include <sys/neutrino.h>
#include <pthread.h>

#include <arm/imx/imx_apbh.h>
#include <arm/imx/imx_gpmi.h>

/**
 * @file       imx-micron/apbh_dma.h
 * @addtogroup etfs_dma DMA
 * @{
 */

int apbh_intr_wait(chipio *chipio);

void *apbhint_thread(void *arg);

void apbh_init(chipio *chipio);

void apbh_init_dma_channel(chipio *chipio);

/** @}*/

#endif /* APBH_DMA_H_ */
