/**
 *  @file AmbaAudio_AacEnc.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Ambarella AAC encoder header.
 *
 */
#ifndef AMBA_AUDIO_AACENC_H
#define AMBA_AUDIO_AACENC_H

#ifndef AMBA_VER_H
#include "AmbaVer.h"
#endif

typedef struct {
    /* Input parameters */
    UINT32 sample_freq;
    UINT32 Src_numCh;
    UINT32 bitRate;
    UINT32 quantizerQuality;
    UINT32 tns;
    UINT32 crc;
    UINT32 pns;
    UINT32 ffType;
    UINT32 enc_mode;
    UINT8 perceptual_mode;
    UINT8 original_copy;
    UINT8 copyright_identification_bit;
    UINT8 copyright_identification_start;
    UINT32 channelMode;
    UINT32 sendSbrHeader;
    UINT32 *codec_lib_mem_adr;
    UINT32 codec_lib_mem_size;
    UINT8 *ancillary_buf;
    UINT32 ancillary_size;
    UINT8 *enc_wptr;
    UINT32 *enc_rptr;
    /* Output parameters */
    UINT32 ErrorStatus;
    UINT32 coreSampleRate;
    UINT32 nBitsInRawDataBlock;
} au_aacenc_config_t;

INT32 aacenc_get_mem_size(UINT32 max_ch_num, UINT32 max_sample_freq, UINT32 max_bitrate, UINT32 mode);
void aacenc_setup(au_aacenc_config_t *pAacEncConfig);
void aacenc_open(au_aacenc_config_t *pAacEncConfig);
void aacenc_encode(au_aacenc_config_t *pAacEncConfig);
void AmbaLibAacEnc_GetVerInfo(AMBA_VerInfo_s *pVerInfo);

#endif /* AMBA_AUDIO_AACENC_H */
