/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
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

struct ambacodec_context;
#define MIXER_CONTEXT_T struct ambacodec_context

#include <audio_driver.h>
#include <mixer/i2s_codec_dll.h>

#ifndef AMBA_AUDIO_CODEC_H
#include "AmbaAudio_CODEC.h"
#endif
#ifdef CONFIG_AUDIO_AK4951EN
#include "AmbaAudio_AK4951EN.h"
#endif
#ifdef CONFIG_AUDIO_NAU8822
#include "AmbaAudio_NAU8822.h"
#endif
static int32_t pcm_devices[1] = { 0 };

#if _NTO_VERSION < 710
static snd_mixer_voice_t    stereo_voices[2] = {
    {SND_MIXER_VOICE_LEFT,  0}, // left channel
    {SND_MIXER_VOICE_RIGHT, 0}  // right channel
};
#endif

#define AMBACODEC_MAX_DIGITAL_VOL      254

/* Output volume range of the AMBACODEC DAC */
static struct snd_mixer_element_volume1_range ambacodec_output_range[2] = {
    {0, AMBACODEC_MAX_DIGITAL_VOL, -12700, 0, 100},    // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
    {0, AMBACODEC_MAX_DIGITAL_VOL, -12700, 0, 100}     // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
};

static int32_t
ambacodec_master_vol_control (MIXER_CONTEXT_T * ambacodec, ado_mixer_delement_t * element, uint8_t set,
                              uint32_t * vol, void *instance_data)
{
    uint8_t data[12];
    int32_t altered = 0, i;


    /* get output volume from hardware */
    //ak4951_output_vol_get( ak4951, ak4951->params.tx_voices, data );

    if (set) {
        for ( i = 0; i < ambacodec->params.tx_voices; i++ ) {
            if ( data[i] != min( vol[i], 0xFF ) ) {
                data[i] = min( vol[i], 0xFF );
                ado_debug( DB_LVL_DRIVER, "AMBACODEC : changing channel %u output volume to %x", i, data[i] );
                altered = 1;
            }
        }

        /* set output volume to CODEC */
        if ( altered ) {
            //ak4951_output_vol_set( ak4951, ak4951->params.tx_voices, data );
        }
    } else { /* read volume */
        for ( i = 0; i < ambacodec->params.tx_voices; i++ ) {
            vol[i] = data[i];
        }
    }

    return altered;
}

static int32_t
ambacodec_master_mute_control (MIXER_CONTEXT_T * ambacodec, ado_mixer_delement_t * element, uint8_t set,
                               uint32_t * mute, void *instance_data)
{
    uint8_t data;
    int32_t altered = 0;

    //ak4951_output_mute_get(ak4951, &data);

    /* implement joint-mute, as the mute bit for L,R channels is common */
    if (set) {
        altered = data != *mute;
        if (altered) {
            //ak4951_output_mute_set(ambacodec, *mute);
        }
    } else { /* read mute */
        *mute = data;
    }

    return altered;
}

/*
 *  build_ambacodec_mixer
 */
static int32_t
build_ambacodec_mixer(MIXER_CONTEXT_T * ambacodec)
{
    int     error = 0;
    ado_mixer_delement_t *pre_elem, *vol_elem, *mute_elem, *elem = NULL;
#if _NTO_VERSION >= 710
    const snd_pcm_chmap_t *chmap = ado_pcm_get_default_chmap(2);
#endif
    ado_debug (DB_LVL_DRIVER, "AMBACODEC");

    /* ################# */
    /* the OUTPUT GROUPS */
    /* ################# */
    if (!error && (elem = ado_mixer_element_pcm1 (ambacodec->mixer, SND_MIXER_ELEMENT_PLAYBACK,
                          SND_MIXER_ETYPE_PLAYBACK1, 1, &pcm_devices[0])) == NULL)
        error++;

    pre_elem = elem;

    if (!error && (vol_elem = ado_mixer_element_volume1 (ambacodec->mixer, "Digital Volume",
                              2, ambacodec_output_range, ambacodec_master_vol_control, NULL, NULL)) == NULL)
        error++;

    /* route pcm to volume */
    if (!error && ado_mixer_element_route_add(ambacodec->mixer, pre_elem, vol_elem) != 0)
        error++;

    pre_elem = vol_elem;

    if (!error && (mute_elem = ado_mixer_element_sw2 (ambacodec->mixer, "Digital Mute",
                               ambacodec_master_mute_control, NULL, NULL)) == NULL)
        error++;

    /* route volume to mute */
    if (!error && ado_mixer_element_route_add(ambacodec->mixer, pre_elem, mute_elem) != 0)
        error++;

    pre_elem = mute_elem;

#if _NTO_VERSION >= 710
    if (!error && (elem = ado_mixer_element_io (ambacodec->mixer, "DAC Output",
                          SND_MIXER_ETYPE_OUTPUT, 0, chmap)) == NULL)
        error++;
#else
    if (!error && (elem = ado_mixer_element_io (ambacodec->mixer, "DAC Output",
                          SND_MIXER_ETYPE_OUTPUT, 0, 2, stereo_voices)) == NULL)
        error++;
#endif

    if (!error && ado_mixer_element_route_add (ambacodec->mixer, pre_elem, elem) != 0)
        error++;
#if _NTO_VERSION >= 710
    if (!error && ado_mixer_playback_group_create(ambacodec->mixer, SND_MIXER_MASTER_OUT,
            chmap, vol_elem, mute_elem) == NULL)
        error++;
#else
    if (!error && ado_mixer_playback_group_create(ambacodec->mixer, SND_MIXER_MASTER_OUT,
            SND_MIXER_CHN_MASK_STEREO, vol_elem, mute_elem) == NULL)
        error++;
#endif

    if (!error) {
        return EOK;
    }

    return ENOMEM;
}

static void
ambacodec_mixer_set_default_group( ado_pcm_t *pcm, ado_mixer_t *mixer, int channel, int index )
{
    switch (channel) {
    case ADO_PCM_CHANNEL_PLAYBACK:
        ado_pcm_chn_mixer (pcm, ADO_PCM_CHANNEL_PLAYBACK, mixer,
                           ado_mixer_find_element (mixer, SND_MIXER_ETYPE_PLAYBACK1,
                                   SND_MIXER_ELEMENT_PLAYBACK, index), ado_mixer_find_group (mixer,
                                           SND_MIXER_MASTER_OUT, index));
        break;
    case ADO_PCM_CHANNEL_CAPTURE: /* no input group */
    default:
        break;
    }
}

void ambacodec_mixer_register_dump( MIXER_CONTEXT_T *ambacodec )
{
    //ak4951_register_dump( ak4951 );
}

static ado_mixer_reset_t ambacodec_mixer_reset;
static int32_t ambacodec_mixer_reset (MIXER_CONTEXT_T *ambacodec)
{
    int32_t status = 0;
    AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl;

    ado_debug (DB_LVL_MIXER, "MIXER RESET");

    pCodecCtrl = &ambacodec->CodecCtrl;
#ifdef CONFIG_AUDIO_AK4951EN
    printf("AK4951EN codec\n");
    pCodecCtrl->CtrlMode = AUCODEC_I2C_CONTROL;
    pCodecCtrl->I2cCtrl.I2cChanNo = AMBA_I2C_CHANNEL3;
    pCodecCtrl->I2cCtrl.SlaveAddr = AK4951_I2C_ADDR;
    pCodecCtrl->I2cCtrl.I2cSpeed = AMBA_I2C_SPEED_FAST;
    if (AmbaAudio_CodecHook(AMBA_AUDIO_CODEC_0, &AmbaAudio_AK4951Obj, pCodecCtrl) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Hook AK4951 failed\n");
    }
    if (AmbaAudio_CodecInit(AMBA_AUDIO_CODEC_0) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Init AK4951 failed\n");
    }
    if (AmbaAudio_CodecModeConfig(AMBA_AUDIO_CODEC_0, AUCODEC_I2S)  != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Mode config AK4951 failed\n");
    }
    if (AmbaAudio_CodecFreqConfig(AMBA_AUDIO_CODEC_0, 48000) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Freq config AK4951 failed\n");
    }
    if (AmbaAudio_CodecSetInput(AMBA_AUDIO_CODEC_0, AUCODEC_DMIC_IN) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Set AK4951 input path failed\n");
    }
    if (AmbaAudio_CodecSetOutput(AMBA_AUDIO_CODEC_0, AUCODEC_HEADPHONE_OUT) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Set AK4951 output path failed\n");
    }
    if (AmbaAudio_CodecSetMute(AMBA_AUDIO_CODEC_0, 0) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Unmute AK4951 failed\n");
    }
#endif
#ifdef CONFIG_AUDIO_NAU8822
    printf("NAU8822 codec\n");
    pCodecCtrl->CtrlMode = AUCODEC_I2C_CONTROL;
    pCodecCtrl->I2cCtrl.I2cChanNo = AMBA_I2C_CHANNEL2;
    pCodecCtrl->I2cCtrl.SlaveAddr = NAU8822AYG_I2C_ADDR;
    pCodecCtrl->I2cCtrl.I2cSpeed = AMBA_I2C_SPEED_STANDARD;
    if (AmbaAudio_CodecHook(AMBA_AUDIO_CODEC_1, &AmbaAudio_NAU8822Obj, pCodecCtrl) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Hook NAU8822 failed\n");
    }
    if (AmbaAudio_CodecInit(AMBA_AUDIO_CODEC_1) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Init NAU8822 failed\n");
    }
    if (AmbaAudio_CodecModeConfig(AMBA_AUDIO_CODEC_1, AUCODEC_I2S) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Mode config NAU8822 failed\n");
    }
    if (AmbaAudio_CodecFreqConfig(AMBA_AUDIO_CODEC_1, 48000) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Freq config NAU8822 failed\n");
    }
    if (AmbaAudio_CodecSetInput(AMBA_AUDIO_CODEC_1, AUCODEC_AMIC_IN) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Set NAU8822 input path failed\n");
    }
    if (AmbaAudio_CodecSetOutput(AMBA_AUDIO_CODEC_1, AUCODEC_SPEAKER_OUT) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Set NAU8822 output path failed\n");
    }
    if (AmbaAudio_CodecSetMute(AMBA_AUDIO_CODEC_1, 0) != AUCODEC_ERR_NONE) {
        printf("AUCODEC: Unmute NAU8822 failed\n");
    }
#endif

    return status;
}

static ado_mixer_destroy_t ambacodec_mixer_destroy;
static int32_t ambacodec_mixer_destroy (MIXER_CONTEXT_T *ambacodec)
{
    ado_debug ( DB_LVL_MIXER, "MIXER DESTROY" );

    //ambacodec_deinit( ambacodec );

    return EOK;
}

static void ambacodec_mixer_set_rate (ado_mixer_t *mixer, uint32_t sample_rate, int channel)
{
    ambacodec_context_t *ambacodec = ado_mixer_get_context(mixer);

    ado_debug ( DB_LVL_DRIVER, "Setting AMBACODEC sample rate to %d", sample_rate );

    //ambacodec_rate_setting ( ambacodec, sample_rate );
}

/* Option names are prefixed with codec name to prevent name collisions
 * with other options handled at higher levels in io-audio;
 * TODO: add option for tdm when needed
 */
static const char* opts[] = {
#define I2C_DEV              0
    "ambacodec_i2c_dev",
#define I2C_ADDR             1
    "ambacodec_i2c_addr",
#define DEBUG                2
    "ambacodec_debug",
#define OUT1                 3
    "ambacodec_out1",
#define OUT2                 4
    "ambacodec_out2",
#define OUT3                 5
    "ambacodec_out3",
#define OUT4                 6
    "ambacodec_out4",
#define OUT5                 7
    "ambacodec_out5",
#define OUT6                 8
    "ambacodec_out6",
#define IN1                  9
    "ambacodec_in1",
#define IN2                  10
    "ambacodec_in2",
    NULL
};

#define N_OPTS ((sizeof(opts)/sizeof(opts[0])) - 1U)

static int
parse_config_opts ( MIXER_CONTEXT_T *ambacodec )
{
    const ado_dict_t *dict = NULL;
    const char* opt_str[N_OPTS] = {NULL};
    const char* sub_opt_str = NULL;
    int opt;
    long value;
    ado_card_t *card = ado_mixer_get_card ( ambacodec->mixer );

    dict = ado_get_card_dict (card);

    ado_config_load_key_values(dict, opts, opt_str, N_OPTS, 0, -1);

    if ( opt_str[I2C_DEV] != NULL ) {
        if ( ( value = strtol ( opt_str[I2C_DEV], NULL, 0 ) ) >= 0 ) {
            ambacodec->params.i2c_dev = value;
        } else {
            ado_error_fmt( "Incorrect value for option %s: %s", opts[I2C_DEV], opt_str[I2C_DEV] );
            return EINVAL;
        }
    }

    if ( opt_str[I2C_ADDR] != NULL ) {
        if ( ( value = strtol ( opt_str[I2C_ADDR], NULL, 0 ) ) != 0 ) {
            ambacodec->params.i2c_addr = value;
        } else {
            ado_error_fmt( "Incorrect value for option %s: %s", opts[I2C_ADDR], opt_str[I2C_ADDR] );
            return EINVAL;
        }
    }

    if (opt_str[DEBUG] != NULL) {
        if ( !strncasecmp ( opt_str[DEBUG], "enable", strlen("enable") ) ) {
            ambacodec->debug = true;
        }
    }

    for ( opt = OUT1; opt <= OUT6; opt++ ) {
        if ( opt_str[opt] ) {
            if ( !strncasecmp ( opt_str[opt], "enable", strlen("enable") ) ) {
                ambacodec->dac_mask |= (1 << (opt-OUT1));
                sub_opt_str = strpbrk ( opt_str[opt], ":" );
                if ( sub_opt_str ) {
                    sub_opt_str++;
                    if ( !strncasecmp ( sub_opt_str, "differential", strlen("differential") ) ) {
                        ambacodec->differential_out_mask |= (1 << (opt-OUT1));
                    } else if ( strncasecmp ( sub_opt_str, "single-ended", strlen("single-ended") ) ) {
                        ado_error_fmt ( "Incorrect sub-option for %s: %s", opts[opt], sub_opt_str );
                        return EINVAL;
                    }
                }
            } else if ( strncasecmp ( opt_str[opt], "disable", strlen("disable") ) ) {
                ado_error_fmt ( "Incorrect option for %s: %s", opts[opt], opt_str[opt] );
                return EINVAL;
            }
        }
    }

    for ( opt = IN1; opt <= IN2; opt++ ) {
        if ( opt_str[opt] ) {
            if ( !strncasecmp ( opt_str[opt], "enable", strlen("enable") ) ) {
                ambacodec->adc_mask |= (1 << (opt-IN1));
                sub_opt_str = strpbrk ( opt_str[opt], ":" );
                if ( sub_opt_str ) {
                    sub_opt_str++;
                    if ( !strncasecmp ( sub_opt_str, "differential", strlen("differential") ) ) {
                        ambacodec->differential_in_mask |= (1 << (opt-IN1));
                    } else if ( strncasecmp ( sub_opt_str, "single-ended", strlen("single-ended") ) ) {
                        ado_error_fmt ( "Incorrect sub-option for %s: %s", opts[opt], sub_opt_str );
                        return EINVAL;
                    }
                }
            } else if ( strncasecmp ( opt_str[opt], "disable", strlen("disable") ) ) {
                ado_error_fmt ( "Incorrect option for %s: %s", opts[opt], opt_str[opt] );
                return EINVAL;
            }
        }
    }

    /* do a basic validation of the options:
     * i2c_dev and i2c_addr must be >= 0 (should have been initialized to -1 in the deva ctrl driver) */
    if ( ambacodec->params.i2c_dev < 0 || ambacodec->params.i2c_addr < 0 ) {
        ado_error_fmt ( "Missing or incorrect I2C params: i2c_dev %d, i2c_addr %d",
                        ambacodec->params.i2c_dev, ambacodec->params.i2c_addr );
        return EINVAL;
    }

    return EOK;
}

ado_mixer_dll_init_t mixer_dll_init;
int mixer_dll_init (MIXER_CONTEXT_T ** context, ado_mixer_t * mixer, void *params, void *raw_callbacks, int version)
{
    ambacodec_context_t *ambacodec;
    ado_mixer_dll_codec_callbacks_t *callbacks = raw_callbacks;

    ado_debug(DB_LVL_MIXER, "Initializing AMBACODEC Codec");
    //printf("Initializing AMBACODEC Codec\n");

    if ( (ambacodec = (ambacodec_context_t *) ado_calloc(1, sizeof (ambacodec_context_t))) == NULL ) {
        ado_error_fmt( "AMBACODEC: Failed to allocate device structure - %s", strerror(errno) );
        return (-1);
    }
    *context = ambacodec;
    ado_mixer_set_name ( mixer, "AMBACODEC" );

    ambacodec->mixer = mixer;
    memcpy ( &ambacodec->params, params, sizeof (ambacodec->params) );

    if ( ambacodec->params.is_active == NULL ) {
        ado_error_fmt ( "AMBACODEC: is_active routine missing" );
        ado_free( ambacodec );
        return (-1);
    }

    if ( parse_config_opts(ambacodec) != EOK ) {
        ado_error_fmt("Failed parsing AMBACODEC options");
        ado_free(ambacodec);
        return -1;
    }

    if (callbacks) {
        callbacks->codec_set_default_group = ambacodec_mixer_set_default_group;
        callbacks->codec_set_rate = ambacodec_mixer_set_rate;
    }

    if ( build_ambacodec_mixer(ambacodec) != EOK ) {
        ado_error_fmt("Failed building AMBACODEC mixer");
        ado_free(ambacodec);
        return -1;
    }

    if ( ambacodec_mixer_reset(ambacodec) != EOK ) {
        ado_error_fmt("Failed to reset AMBACODEC");
        ado_free(ambacodec);
        return -1;
    }

    ado_mixer_set_reset_func ( ambacodec->mixer, ambacodec_mixer_reset );
    ado_mixer_set_destroy_func ( ambacodec->mixer, ambacodec_mixer_destroy );

    return 0;
}

ado_dll_version_t version;
#if _NTO_VERSION >= 710
void
version (int *major, int *minor, const char **date)
{
    *major = ADO_MAJOR_VERSION;
    *minor = I2S_CODEC_MINOR_VERSION;
    *date = __DATE__;
}
#else
void
version (int *major, int *minor, char *date)
{
    *major = ADO_MAJOR_VERSION;
    *minor = I2S_CODEC_MINOR_VERSION;
    date = __DATE__;
}
#endif
