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

#ifndef AUDIO_IRQ_H_
#define AUDIO_IRQ_H_

#include "amba.h"

#define DEVA_DMA_STOP_FLAG   0x00000001

uint32_t audio_rx_dma_init(uint32_t Index, amba_context_t *amba);
uint32_t audio_tx_dma_init(uint32_t Index, amba_context_t *amba);
uint32_t audio_dma_alloc_addr(void);
uint32_t audio_dma_alloc_buf(ULONG *pBase, uint32_t ReqSize, uint32_t Align);
uint32_t audio_rx_dma_desc_setup(amba_context_t *amba);
uint32_t audio_rx_dma_start(amba_context_t *amba);
uint32_t audio_rx_dma_stop(amba_context_t *amba);
uint32_t audio_tx_dma_desc_setup(amba_context_t *amba);
uint32_t audio_tx_dma_start(amba_context_t *amba);
uint32_t audio_tx_dma_stop(amba_context_t *amba);
uint32_t audio_rx_attach_dma(void **x, void (*handler) (HW_CONTEXT_T * hw_context), HW_CONTEXT_T * hw_context);
uint32_t audio_tx_attach_dma(void **x, void (*handler) (HW_CONTEXT_T * hw_context), HW_CONTEXT_T * hw_context);
uint32_t audio_osal_eventflag_init(osal_even_t *event, const char *pName);
uint32_t audio_osal_eventflag_set(osal_even_t *event, uint32_t flag);
uint32_t audio_osal_eventflag_get(osal_even_t *event, uint32_t reqflag, uint32_t all, uint32_t clear, uint32_t *actflag, uint32_t timeout);
uint32_t audio_osal_eventflag_clear(osal_even_t *event, uint32_t flag);

#endif
