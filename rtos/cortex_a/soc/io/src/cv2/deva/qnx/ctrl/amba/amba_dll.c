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

#include <unistd.h>
#include <string.h>

#include "AmbaKAL.h"
#include "amba.h"
#include "Amba_i2s.h"
#include "AmbaRTSL_I2S.h"
#include "audio_dma.h"
#include "AmbaMisraFix.h"
#include "AmbaCache.h"

uint32_t audio_osal_get_sys_tick(uint32_t *msec)
{
    uint32_t rval = 0;
    struct timespec tp;

    rval = clock_gettime(CLOCK_MONOTONIC, &tp);
    if (rval == 0) {
        *msec = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
    } else {
        *msec = 0;
    }

    return rval;
}

static int32_t amba_capabilities(HW_CONTEXT_T* amba, ado_pcm_t *pcm, snd_pcm_channel_info_t* info)
{
    //printf("###################### %s\n", __func__);

    ado_debug( DB_LVL_DRIVER, "CAPABILITIES" );

    if( amba->pcm != pcm ) {
        ado_error_fmt( "invalid pcm" );
        return EINVAL;
    }

    info->formats       = (SND_PCM_FMT_S32_LE);
    info->rates         = ado_pcm_rate2flag(48000);
    info->min_rate      = 48000;
    info->max_rate      = 48000;
    info->min_voices    = 2;
    info->max_voices    = 2;
    info->min_fragment_size = 1024;
    info->max_fragment_size = 8192;

    return EOK;
}

static int32_t amba_playback_acquire (HW_CONTEXT_T * amba, PCM_SUBCHN_CONTEXT_T ** pc,
                                      ado_pcm_config_t * config, ado_pcm_subchn_t * subchn, uint32_t * why_failed)
{
    ado_debug( DB_LVL_DRIVER, "ACQUIRE" );

    ado_mutex_lock (&amba->hw_lock);

    /* Allocate DMA buffer */
#if _NTO_VERSION >= 710
    if( ado_pcm_buf_alloc(amba->card, config, config->dmabuf.size,
                          ADO_BUF_DMA_SAFE | ADO_BUF_CACHE ) == NULL ) {
        ado_mutex_unlock (&amba->hw_lock);
        ado_error_fmt( "failed allocating shared memory" );
        return ENOMEM;
    }
#else
    if( ado_pcm_buf_alloc( config, config->dmabuf.size,
                           ADO_BUF_DMA_SAFE | ADO_BUF_CACHE ) == NULL ) {
        ado_mutex_unlock (&amba->hw_lock);
        ado_error_fmt( "failed allocating shared memory" );
        return ENOMEM;
    }
#endif
    amba->playback.pDmaBuf = config->dmabuf.addr;
    amba->playback.DmaBufSize = config->dmabuf.size;

    amba->playback.frag_size = config->mode.block.frag_size;
    amba->playback.frags_total = config->mode.block.frags_total;
    audio_tx_dma_desc_setup(amba);

    ado_debug( DB_LVL_DRIVER, "dmabuf.size = %X ", config->dmabuf.size );

    amba->playback.subchn = subchn;
    amba->playback.pcm_config = config;
    *pc = &amba->playback;

    ado_mutex_unlock (&amba->hw_lock);

    //printf("###############  Done  ########## %s\n", __func__);

    return EOK;
}

/* */
/*  Playback release*/
/* */
static int32_t amba_playback_release (HW_CONTEXT_T * amba, PCM_SUBCHN_CONTEXT_T * pc,
                                      ado_pcm_config_t * config)
{
    //printf("%s\n", __func__);

    ado_debug( DB_LVL_DRIVER, "RELEASE" );

    /* Free DMA transfer buffer*/
#if _NTO_VERSION >= 710
    ado_pcm_buf_free (amba->card, config);
#else
    ado_pcm_buf_free (config);
#endif
    config->dmabuf.addr = NULL;

    ado_mutex_unlock (&amba->hw_lock);

    return (EOK);
}

static int32_t amba_capture_acquire (HW_CONTEXT_T * amba, PCM_SUBCHN_CONTEXT_T ** pc,
                                     ado_pcm_config_t * config, ado_pcm_subchn_t * subchn, uint32_t * why_failed)
{
    ado_debug( DB_LVL_DRIVER, "ACQUIRE" );

    ado_mutex_lock (&amba->hw_lock);

    /* Allocate DMA buffer */
#if _NTO_VERSION >= 710
    if( ado_pcm_buf_alloc(amba->card, config, config->dmabuf.size,
                          ADO_BUF_DMA_SAFE | ADO_BUF_CACHE ) == NULL ) {
        ado_mutex_unlock (&amba->hw_lock);
        ado_error_fmt( "failed allocating shared memory" );
        return ENOMEM;
    }
#else
    if( ado_pcm_buf_alloc( config, config->dmabuf.size,
                           ADO_BUF_DMA_SAFE | ADO_BUF_CACHE ) == NULL ) {
        ado_mutex_unlock (&amba->hw_lock);
        ado_error_fmt( "failed allocating shared memory" );
        return ENOMEM;
    }
#endif
    amba->capture.pDmaBuf = config->dmabuf.addr;
    amba->capture.DmaBufSize = config->dmabuf.size;

    amba->capture.frag_size = config->mode.block.frag_size;
    amba->capture.frags_total = config->mode.block.frags_total;
    audio_rx_dma_desc_setup(amba);

    ado_debug( DB_LVL_DRIVER, "dmabuf.size = %X ", config->dmabuf.size );

    amba->capture.subchn = subchn;
    amba->capture.pcm_config = config;
    *pc = &amba->capture;

    ado_mutex_unlock (&amba->hw_lock);

    //printf("###############  Done  ########## %s\n", __func__);

    return EOK;
}


/* */
/*  Recording release */
/* */
static int32_t amba_capture_release (HW_CONTEXT_T * amba, PCM_SUBCHN_CONTEXT_T * pc,
                                     ado_pcm_config_t * config)
{
    //printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX %s\n", __func__);

    ado_debug (DB_LVL_DRIVER, "RELEASE");

    ado_mutex_lock (&amba->hw_lock);

    /* Free DMA transfer buffer*/
#if _NTO_VERSION >= 710
    ado_pcm_buf_free (amba->card, config);
#else
    ado_pcm_buf_free (config);
#endif
    config->dmabuf.addr = NULL;

    ado_mutex_unlock (&amba->hw_lock);

    return (EOK);
}

static int32_t amba_playback_prepare (HW_CONTEXT_T * amba, PCM_SUBCHN_CONTEXT_T * pc, ado_pcm_config_t * config)
{
    int status = EOK;

    //printf("%s\n", __func__);

    ado_debug (DB_LVL_DRIVER, "PREPARE");

    pc->cur_dma_frag = 0;

    ado_mutex_lock (&amba->hw_lock);

    ado_mutex_unlock (&amba->hw_lock);

    return status;
}

static int32_t amba_capture_prepare (HW_CONTEXT_T * amba, PCM_SUBCHN_CONTEXT_T * pc, ado_pcm_config_t * config)
{
    int status = EOK;

    //printf("%s\n", __func__);

    ado_debug (DB_LVL_DRIVER, "PREPARE");

    pc->cur_dma_frag = 0;

    ado_mutex_lock (&amba->hw_lock);

    ado_mutex_unlock (&amba->hw_lock);

    return status;
}

static int32_t amba_playback_trigger (HW_CONTEXT_T * amba, PCM_SUBCHN_CONTEXT_T * pc, uint32_t cmd)
{
    //printf("%s: %d\n", __func__, cmd);

    ado_debug (DB_LVL_DRIVER, "TRIGGER - cmd : %d", cmd);

    if( pc->subchn != amba->playback.subchn ) {
        ado_debug( DB_LVL_DRIVER, "unknown subchn" );
        return EINVAL;
    }

    if( cmd == ADO_PCM_TRIGGER_GO ) {
        ado_debug( DB_LVL_DRIVER, "ADO_PCM_TRIGGER_GO" );

        audio_tx_dma_start(amba);
        (void)AmbaRTSL_I2sTxTrigger(amba->playback.i2s_index, 1);
        amba->playback.active = 1;
    } else {
        if (amba->playback.active == 1) {
            ado_debug (DB_LVL_DRIVER, "ADO_PCM_TRIGGER_STOP");
            audio_tx_dma_stop(amba);
            //printf("Stop %s: done!! %d\n", __func__, amba->playback.active);
        }
    }
    return EOK;
}

static int32_t amba_capture_trigger (HW_CONTEXT_T * amba, PCM_SUBCHN_CONTEXT_T * pc, uint32_t cmd)
{
    //printf("%s: cmd:%d \n", __func__, cmd);

    ado_debug (DB_LVL_DRIVER, "TRIGGER - cmd: %d", cmd);

    if( pc->subchn != amba->capture.subchn ) {
        ado_debug (DB_LVL_DRIVER, "unknown subchn");
        return EINVAL;
    }

    if (cmd == ADO_PCM_TRIGGER_GO) {
        ado_debug (DB_LVL_DRIVER, "ADO_PCM_TRIGGER_GO");

        audio_rx_dma_start(amba);
        (void)AmbaRTSL_I2sRxTrigger(amba->capture.i2s_index, 1);
        amba->capture.active = 1;
    } else {
        if (amba->capture.active == 1) {
            ado_debug (DB_LVL_DRIVER, "ADO_PCM_TRIGGER_STOP");
            audio_rx_dma_stop(amba);
            //printf("Stop %s: done!! %d\n", __func__, amba->capture.active);
        }
    }
    return EOK;
}

static uint32_t amba_position
(
    HW_CONTEXT_T * amba,
    PCM_SUBCHN_CONTEXT_T * pc,
    ado_pcm_config_t * config,
    uint32_t * hw_buffer_level
)
{
    uint32_t bytes_left = 0;
    uint32_t pos;

    printf("%s\n", __func__);

    if( hw_buffer_level ) {
        *hw_buffer_level = 0;
    }

    ado_mutex_lock (&amba->hw_lock);


    /* cur_dma_frag is the 0 based index of the current fragment being dma transferred;
     * at the time the current fragment's dma transfer is complete, we will have sent
     * cur_dma_frag + 1 fragments; the current position is hence the difference between
     * (cur_dma_frag + 1) * frag_size - bytes_left
     */
    pos = config->mode.block.frag_size * (pc->cur_dma_frag + 1) - bytes_left;


    ado_mutex_unlock (&amba->hw_lock);

    ado_debug (DB_LVL_DRIVER, "position=%d/%d, cur_dma_frag=%d", pos, config->dmabuf.size, pc->cur_dma_frag);

    return pos;
}

static void amba_release_resources(amba_context_t * amba)
{
    printf("%s\n", __func__);
}

static bool amba_mixer_is_active ( HW_CONTEXT_T *amba, int channel )
{
    if ( channel == ADO_PCM_CHANNEL_CAPTURE ) {
        return (amba->capture.active ? true : false);
    } else if ( channel == ADO_PCM_CHANNEL_PLAYBACK ) {
        return (amba->playback.active ? true : false);
    }
    return (false);
}

int32_t
build_codec_mixer( ado_card_t * card, HW_CONTEXT_T * amba )
{
    int32_t status;

    memset ( &amba->codec_mixer_callbacks, 0, sizeof (amba->codec_mixer_callbacks) );

    ado_mixer_dll_codec_params_t params = {0};

    params.hw_context = amba;

    params.tx_sample_rate = ado_pcm_rate2flag(48000);
    params.rx_sample_rate = ado_pcm_rate2flag(48000);
    params.tx_sample_size = 4;
    params.rx_sample_size = 4;
    params.tx_voices = 2;
    params.rx_voices = 2;
    params.clk_master = false;

    /* set i2c_dev, i2c_addr, mclk to invalid values, these should be passed as config file codec params */
    params.i2c_dev = -1;
    params.i2c_addr = -1;
    params.mclk = 0;

    /* mixer -> ctrl callbacks */
    params.is_active = amba_mixer_is_active;

    /* If NULL is passed in as the DLL name, this will cause the ado_mixer_dll() call
     * to look in the audio configuration file for the dll name (mixer_dll=xxxx)
     */
    if ((status = ado_mixer_dll (card, NULL, I2S_CODEC_MINOR_VERSION, &params, &amba->codec_mixer_callbacks, &amba->codec_mixer))) {
        /* ado_mixer_dll returns -1 on failure, return instead ENOENT */
        status = errno;
        ado_error_fmt("ado_mixer_dll failed (%d)", status);
    } else if ( amba->codec_mixer_callbacks.codec_set_default_group == NULL ) {
        ado_error_fmt("Mixer DLL missing codec_set_default_group callback");
        status = ENOTSUP;
        ado_mixer_destroy ( amba->codec_mixer );
    }
    return status;
}

int amba_mixer_init(ado_card_t* card, HW_CONTEXT_T* amba)
{
    int status = OK;
    int ret;

    status = build_codec_mixer(card, amba);
    //printf("build_codec_mixer, status=%d\n", status);
    if (amba->codec_mixer) {
        amba->mixer = amba->codec_mixer;
    } else {
        ret = ado_mixer_create( card, "amba_mixer", &amba->mixer, amba );
        //printf("ado_mixer_create, ret=%d\n", ret);
        if (ret != EOK) {
            /* ado_mixer_create returns -1 and sets errno on failure */
            status = errno;
            printf ( "Failed building a simple mixer (%d)\n", status );
            return status;
        }
    }
    return 0;
}

void amba_play_pulse_hdlr (HW_CONTEXT_T * amba)
{
    UINT32 Loop = 0;
    INT32 LookAhead;
    volatile uint32_t *pRpt;
    volatile int32_t cur_frag; /* volatile so compiler won't optimize */
    uint32_t systick = 0;

    do {
        pRpt = &amba->playback.pStatus[amba->playback.cur_dma_frag];
        if ((*pRpt & (UINT32)0x08000000U) != 0U) {
            break;
        }
        Loop++;
        if (Loop > 5U) {
            (void)AmbaKAL_TaskSleep(1);
        }
        if (Loop > 10U) {
            break;
        }
    } while ((*pRpt & (UINT32)0x08000000U) == 0U);

    for(LookAhead = 0; LookAhead < amba->playback.pcm_config->mode.block.frags_total; LookAhead++) {
        pRpt = &amba->playback.pStatus[amba->playback.cur_dma_frag];
        if ((*pRpt & (UINT32)0x08000000U) == 0U) {
            break;
        }
        if ((*pRpt & (UINT32)0x08000000U) != 0U) {
            (void)audio_osal_get_sys_tick(&systick);
            //printf("[%08d] %s: %d, 0x%x\n", systick, __func__, amba->capture.cur_dma_frag, *pRpt);
            *pRpt &= (~0x08000000U);

            /* Signal to io-audio (DMA transfer was completed) */
            if (amba->playback.active == 1) {
                dma_interrupt(amba->playback.subchn);
            }

            if ((*pRpt & (UINT32)0x10000000U) != 0U) {
                *pRpt &= (~0x10000000U);
                amba->playback.active = 0;
                (void)AmbaRTSL_I2sTxTrigger(amba->playback.i2s_index, 0);
                //printf("Dma Stop %d\n", amba->playback.active);
            }

            //AmbaMisra_TypeCast(&DataAddr, &amba->playback.pDmaBuf);
            //DataAddr = DataAddr + amba->playback.frag_size * amba->playback.cur_dma_frag;
            //(void)AmbaCache_DataInvalidate(DataAddr, amba->playback.frag_size);

            /* update cur_dma_frag on the stack so we can set it atomically without a lock */
            cur_frag = amba->playback.cur_dma_frag + 1;

            if (cur_frag == amba->playback.pcm_config->mode.block.frags_total) {
                cur_frag = 0;
            }
            amba->playback.cur_dma_frag = cur_frag;

            pRpt = &amba->playback.pStatus[amba->playback.cur_dma_frag];
        }
    }
}

void amba_cap_pulse_hdlr (HW_CONTEXT_T * amba)
{
    UINT32 Loop = 0;
    INT32 LookAhead;
    volatile uint32_t *pRpt;
    volatile int32_t cur_frag; /* volatile so compiler won't optimize */
    uint32_t systick = 0;

    do {
        pRpt = &amba->capture.pStatus[amba->capture.cur_dma_frag];
        if ((*pRpt & (UINT32)0x08000000U) != 0U) {
            break;
        }
        Loop++;
        if (Loop > 5U) {
            (void)AmbaKAL_TaskSleep(1);
        }
        if (Loop > 10U) {
            break;
        }
    } while ((*pRpt & (UINT32)0x08000000U) == 0U);

    for(LookAhead = 0; LookAhead < amba->capture.pcm_config->mode.block.frags_total; LookAhead++) {
        pRpt = &amba->capture.pStatus[amba->capture.cur_dma_frag];
        if ((*pRpt & (UINT32)0x08000000U) == 0U) {
            break;
        }
        if ((*pRpt & (UINT32)0x08000000U) != 0U) {
            (void)audio_osal_get_sys_tick(&systick);
            //printf("[%08d] %s: %d, 0x%x\n", systick, __func__, amba->capture.cur_dma_frag, *pRpt);
            *pRpt &= (~0x08000000U);

            /* Signal to io-audio (DMA transfer was completed) */
            if (amba->capture.active == 1) {
                dma_interrupt(amba->capture.subchn);
            }

            if ((*pRpt & (UINT32)0x10000000U) != 0U) {
                *pRpt &= (~0x10000000U);
                amba->capture.active = 0;
                (void)AmbaRTSL_I2sRxTrigger(amba->capture.i2s_index, 0);
                //printf("Dma Stop %d\n", amba->capture.active);
            }

            //AmbaMisra_TypeCast(&DataAddr, &amba->capture.pDmaBuf);
            //DataAddr = DataAddr + amba->capture.frag_size * amba->capture.cur_dma_frag;
            //(void)AmbaCache_DataInvalidate(DataAddr, amba->capture.frag_size);

            /* update cur_dma_frag on the stack so we can set it atomically without a lock */
            cur_frag = amba->capture.cur_dma_frag + 1;

            if (cur_frag == amba->capture.pcm_config->mode.block.frags_total) {
                cur_frag = 0;
            }
            amba->capture.cur_dma_frag = cur_frag;

            pRpt = &amba->capture.pStatus[amba->capture.cur_dma_frag];
        }
    }
}

ado_dll_version_t ctrl_version;
#if _NTO_VERSION >= 710
void ctrl_version (int *major, int *minor, const char **date)
{
    *major = ADO_MAJOR_VERSION;
    *minor = 1;
    *date = __DATE__;
}
#else
void ctrl_version (int *major, int *minor, char *date)
{
    *major = ADO_MAJOR_VERSION;
    *minor = 1;
    date = __DATE__;
    (void)(date); /* Unused */
}
#endif

static void ctrl_init_cleanup(amba_context_t * amba)
{
    ado_debug (DB_LVL_DRIVER, "amba");

    ado_mutex_destroy (&amba->hw_lock);

    amba_release_resources (amba);

    ado_free (amba);

    amba = NULL;
}

/* */
/*  Initialize */
/* */

int ctrl_init (HW_CONTEXT_T ** hw_context, ado_card_t * card, char *args)
{
    amba_context_t *amba;
    int status;
    int ret = 0;
    ado_pcm_cap_t* pcm_caps;
    ado_pcm_hw_t* tx_pcm_funcs = NULL;
    ado_pcm_hw_t* rx_pcm_funcs = NULL;

    static char DmaRxFlag[] = "DmaRxFlag";
    static char DmaTxFlag[] = "DmaTxFlag";

    ado_debug (DB_LVL_DRIVER, "amba : CTRL_DLL_INIT");

    if ((amba = (amba_context_t *) ado_calloc (1, sizeof (amba_context_t))) == NULL) {
        ado_error_fmt ("Unable to allocate memory (%s)", strerror (errno));
        return ENOMEM;
    }

    memset(amba, 0, sizeof(amba_context_t));
    *hw_context = amba;

    /* TBD, will be chosen by cmd */
    amba->capture.i2s_index = 0;
    amba->playback.i2s_index = 0;

    ado_card_set_shortname (card, "amba");
    ado_card_set_longname (card, "amba", AMBA_I2S0_BASE);

    /* Map I2S base register */
    if ((status = Amba_I2s_Init()) != 0) {
        ado_error_fmt ("amba: I2S init failed");
    }

    audio_osal_eventflag_init(&amba->capture.flag, DmaRxFlag);
    audio_osal_eventflag_init(&amba->playback.flag, DmaTxFlag);


    audio_rx_dma_init(0, amba);
    //printf("RX dma ch: %d\n", amba->capture.DmaChanNo);

    audio_tx_dma_init(0, amba);
    //printf("TX dma ch: %d\n", amba->playback.DmaChanNo);

    amba->capture.active = 0;
    amba->playback.active = 0;

    audio_rx_attach_dma(&amba->capture.pulse, amba_cap_pulse_hdlr, amba);
    audio_tx_attach_dma(&amba->playback.pulse, amba_play_pulse_hdlr, amba);

    rx_pcm_funcs = &amba->capture.pcm_funcs;
    tx_pcm_funcs = &amba->playback.pcm_funcs;

    ado_mutex_init (&amba->hw_lock);

    /* Set capabilities of playback and recording */
    pcm_caps = &amba->pcm_caps;

#if _NTO_VERSION >= 710
    pcm_caps->chn_flags = SND_PCM_CHNINFO_BLOCK |
                          SND_PCM_CHNINFO_INTERLEAVE | SND_PCM_CHNINFO_BLOCK_TRANSFER |
                          SND_PCM_CHNINFO_MMAP | SND_PCM_CHNINFO_MMAP_VALID;
#else
    pcm_caps->chn_flags = SND_PCM_CHNINFO_BLOCK | SND_PCM_CHNINFO_STREAM |
                          SND_PCM_CHNINFO_INTERLEAVE | SND_PCM_CHNINFO_BLOCK_TRANSFER |
                          SND_PCM_CHNINFO_MMAP | SND_PCM_CHNINFO_MMAP_VALID;
#endif
    /* TBD, choose correct flags for current system */

    pcm_caps->formats = (SND_PCM_FMT_S32_LE);
    pcm_caps->rates = ado_pcm_rate2flag(48000);
    pcm_caps->min_rate = 48000;
    pcm_caps->max_rate = 48000;
    pcm_caps->min_voices = 2;
    pcm_caps->max_voices = 2;
    pcm_caps->min_fragsize = 1024;
    pcm_caps->max_fragsize = 8192;
    pcm_caps->max_frags = MAX_DMA_FRAGS_NUM;

    if (tx_pcm_funcs) {
        /* Set functions for playback */
        tx_pcm_funcs->aquire = amba_playback_acquire;
        tx_pcm_funcs->release = amba_playback_release;
        tx_pcm_funcs->prepare = amba_playback_prepare;
        tx_pcm_funcs->trigger = amba_playback_trigger;
        tx_pcm_funcs->position = amba_position;
        tx_pcm_funcs->capabilities = amba_capabilities;
    }

    if (rx_pcm_funcs) {
        /* Set functions for recording */
        rx_pcm_funcs->aquire = amba_capture_acquire;
        rx_pcm_funcs->release = amba_capture_release;
        rx_pcm_funcs->prepare = amba_capture_prepare;
        rx_pcm_funcs->trigger = amba_capture_trigger;
        rx_pcm_funcs->position = amba_position;
        rx_pcm_funcs->capabilities = amba_capabilities;
    }

    /* Create a PCM audio device */
    amba_mixer_init(card, amba);

    errno = 0;
    ret = ado_pcm_create (card, "AMBA I2S", 0, "amba",
                          1, pcm_caps, tx_pcm_funcs,
                          1, pcm_caps, rx_pcm_funcs, amba->mixer, &amba->pcm);
    //printf("ado_pcm_create, ret=%d\n", ret);

    amba->card = card;

    ado_debug (DB_LVL_DRIVER, "amba initialization complete.....");
    printf("amba deva initialization complete.....\n");

    return ret;
}

ado_ctrl_dll_destroy_t ctrl_destroy;
int ctrl_destroy (HW_CONTEXT_T * amba)
{
    ado_debug (DB_LVL_DRIVER, "amba : CTRL_DLL_DESTROY");

    ctrl_init_cleanup(amba);

    amba = NULL;

    return EOK;
}
