/*
 * $QNXLicenseC:
 * Copyright 2014, 2016 QNX Software Systems.
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

#ifndef _AMBA_H
#define _AMBA_H

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <atomic.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <mqueue.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/cache.h>
#include <sys/procmgr.h>
#include <sys/ioctl.h>
#include <fcntl.h>

struct  amba_context;
struct  amba_audio_channel;

#define  HW_CONTEXT_T           struct amba_context
#define  MIXER_CONTEXT_T        struct amba_context
#define  PCM_SUBCHN_CONTEXT_T   struct amba_audio_channel

#include <audio_driver.h>
#include "AmbaDMA.h"
#if _NTO_VERSION >= 700
#include <mixer/i2s_codec_dll.h>
#endif

#define MAX_DMA_FRAGS_NUM   16U
#define DEVA_RX_PRIORITY    120
#define DEVA_TX_PRIORITY    120

typedef struct {
    volatile uint32_t           flag;
    sem_t                       wait_queue;
} osal_even_t;

typedef struct amba_audio_channel {
    ado_pcm_hw_t          pcm_funcs;
    ado_pcm_subchn_t      *subchn;
    ado_pcm_config_t      *pcm_config;
    uint32_t              active;
    uint32_t              cur_dma_frag;
    uint32_t              frag_size;
    uint32_t              frags_total;
    uint32_t              DmaChanNo;
    int8_t                *pDmaBuf;
    size_t                DmaBufSize;
    AMBA_DMA_DESC_s       *pDesc;
    uint32_t              *pStatus;
    uint32_t              i2s_index;
    void                  *pulse;
    osal_even_t           flag;
} amba_audio_channel_t;

typedef struct amba_context {
    ado_mutex_t              hw_lock;
    ado_pcm_cap_t            pcm_caps;
    ado_pcm_t                *pcm;
    ado_mixer_t              *codec_mixer;
    ado_mixer_t              *mixer; /* points to simple mixer */
    ado_card_t               *card;
#if _NTO_VERSION >= 700
    ado_mixer_dll_codec_callbacks_t codec_mixer_callbacks;
#endif
    amba_audio_channel_t     playback;  /* Settings for one playback channel.*/
    amba_audio_channel_t     capture;   /* Settings for one capture channel.*/
} amba_context_t;

#endif /* _AMBA_H */
