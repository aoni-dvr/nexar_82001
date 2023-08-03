/**
 *  @file AmbaAudio_CODEC.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Audio AD/DA CODEC driver
 *
 */

#ifndef AMBA_AUDIO_CODEC_H
#define AMBA_AUDIO_CODEC_H

#ifndef AMBA_KAL_H
#include "AmbaKAL.h"
#endif
#ifndef AMBA_I2C_H
#include "AmbaI2C.h"
#endif
#ifndef AMBA_SPI_H
#include "AmbaSPI.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#if defined(CONFIG_QNX)
#include <audio_driver.h>
#if _NTO_VERSION >= 700
#include <mixer/i2s_codec_dll.h>
#endif
#endif

/* ERROR CODE */
#define AUCODEC_ERR_NONE        ERR_NONE
#define AUCODEC_ERR_MUTEX       (AUCODEC_ERR_BASE + 1U)
#define AUCODEC_ERR_ARG         (AUCODEC_ERR_BASE + 2U)

/* print module */
#define AUCODEC_MODULE_ID       ((UINT16)(AUCODEC_ERR_BASE >> 16U))

#define AMBA_AUDIO_CODEC_0      0U
#define AMBA_AUDIO_CODEC_1      1U
#define AMBA_NUM_AUDIO_CODEC    2U


#define AUCODEC_LEFT_JUSTIFIED  0x0U    /* Left-Justified data format */
#define AUCODEC_RIGHT_JUSTIFIED 0x1U    /* Right-Justified data format */
#define AUCODEC_MSB_EXTENDED    0x2U    /* MSB extended data format */
#define AUCODEC_I2S             0x4U    /* Philips I2S data format */
#define AUCODEC_DSP             0x6U    /* DSP / TDM data format */

#define AUCODEC_HEADPHONE_OUT   0x4U
#define AUCODEC_LINE_OUT        0x2U
#define AUCODEC_SPEAKER_OUT     0x1U
#define AUCODEC_SILENT_OUT      0x0U

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define AUCODEC_DMIC2_IN        0x10U
#endif
#define AUCODEC_AMIC2_IN        0x8U
#define AUCODEC_DMIC_IN         0x4U
#define AUCODEC_LINE_IN         0x2U
#define AUCODEC_AMIC_IN         0x1U
#define AUCODEC_SILENT_IN       0x0U

#define AUCODEC_I2C_CONTROL     0x0U
#define AUCODEC_SPI_CONTROL     0x1U

typedef struct {
    UINT32 SpiChanNo;
    UINT32 SlaveID;
    UINT32 BaudRate;
} AUCODEC_SPI_CTRL_s;

typedef struct {
    UINT32 I2cChanNo;
    UINT32 SlaveAddr;
    UINT32 I2cSpeed;
} AUCODEC_I2C_CTRL_s;

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    UINT32 CtrlMode;
    AUCODEC_I2C_CTRL_s I2cCtrl;
    AUCODEC_SPI_CTRL_s SpiCtrl;
} AMBA_AUDIO_CODEC_CTRL_s;

typedef struct {
    UINT32 (*Init)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl);
    UINT32 (*ModeConfig)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Mode);
    UINT32 (*FreqConfig)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Freq);
    UINT32 (*SetOutput)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 OutputMode);
    UINT32 (*SetInput)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 InputMode);
    UINT32 (*SetMute)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 MuteEnable);
    UINT32 (*Write)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, UINT32 Data);
    UINT32 (*Read)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Addr, const UINT32 *pRxData);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    UINT32 (*SetVolume)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Level);
    UINT32 (*EnableSpeaker)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl, UINT32 Enable);
    UINT32 (*GetMicType)(AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl);
#endif
} AMBA_AUDIO_CODEC_OBJ_s;

typedef struct {
    AMBA_AUDIO_CODEC_CTRL_s AmbaAudioCodecCtrl;
    AMBA_AUDIO_CODEC_OBJ_s AmbaAudioCodecObj;
} AMBA_AUDIO_CODEC_s;

#if defined(CONFIG_QNX)
typedef struct ambacodec_context {
    ado_mixer_t                  *mixer;
    ado_mixer_dll_codec_params_t params;
    bool                         debug;
    bool                         tdm;
    uint8_t                      differential_in_mask;
    uint8_t                      differential_out_mask;
    uint8_t                      adc_mask;
    uint8_t                      dac_mask;
    AMBA_AUDIO_CODEC_CTRL_s      CodecCtrl;
} ambacodec_context_t;
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaAudio_CODEC.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_AUDIO_CODEC_s AmbaAudioCodec[AMBA_NUM_AUDIO_CODEC];

static inline UINT32 AmbaAudio_CodecInit(UINT32 Index)
{
    return AmbaAudioCodec[Index].AmbaAudioCodecObj.Init(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl);
}
static inline UINT32 AmbaAudio_CodecModeConfig(UINT32 Index, UINT32 Mode)
{
    return AmbaAudioCodec[Index].AmbaAudioCodecObj.ModeConfig(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl, Mode);
}
static inline UINT32 AmbaAudio_CodecFreqConfig(UINT32 Index, UINT32 Freq)
{
    return AmbaAudioCodec[Index].AmbaAudioCodecObj.FreqConfig(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl, Freq);
}
static inline UINT32 AmbaAudio_CodecSetOutput(UINT32 Index, UINT32 OutputMode)
{
    return AmbaAudioCodec[Index].AmbaAudioCodecObj.SetOutput(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl, OutputMode);
}
static inline UINT32 AmbaAudio_CodecSetInput(UINT32 Index, UINT32 InputMode)
{
    return AmbaAudioCodec[Index].AmbaAudioCodecObj.SetInput(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl, InputMode);
}
static inline UINT32 AmbaAudio_CodecSetMute(UINT32 Index, UINT32 MuteEnable)
{
    return AmbaAudioCodec[Index].AmbaAudioCodecObj.SetMute(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl, MuteEnable);
}
static inline UINT32 AmbaAudio_CodecWrite(UINT32 Index, UINT32 Addr, UINT32 Data)
{
    return AmbaAudioCodec[Index].AmbaAudioCodecObj.Write(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl, Addr, Data);
}
static inline UINT32 AmbaAudio_CodecRead(UINT32 Index, UINT32 Addr, const UINT32 *pRxData)
{
    return AmbaAudioCodec[Index].AmbaAudioCodecObj.Read(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl, Addr, pRxData);
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static inline UINT32 AmbaAudio_CodecSetVolume(UINT32 Index, UINT32 Level)
{
    if (AmbaAudioCodec[Index].AmbaAudioCodecObj.SetVolume != NULL) {
        return AmbaAudioCodec[Index].AmbaAudioCodecObj.SetVolume(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl, Level);
    }
    return AUCODEC_ERR_ARG;
}

static inline UINT32 AmbaAudio_CodecEnableSpeaker(UINT32 Index, UINT32 Enable)
{
    if (AmbaAudioCodec[Index].AmbaAudioCodecObj.EnableSpeaker != NULL) {
        return AmbaAudioCodec[Index].AmbaAudioCodecObj.EnableSpeaker(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl, Enable);
    }
    return AUCODEC_ERR_ARG;
}

static inline UINT32 AmbaAudio_CodecGetMicType(UINT32 Index)
{
    if (AmbaAudioCodec[Index].AmbaAudioCodecObj.GetMicType != NULL) {
        return AmbaAudioCodec[Index].AmbaAudioCodecObj.GetMicType(&AmbaAudioCodec[Index].AmbaAudioCodecCtrl);
    }
    return 0;
}
#endif
UINT32 AmbaAudio_CodecHook(UINT32 Index, const AMBA_AUDIO_CODEC_OBJ_s *pCodecObj, const AMBA_AUDIO_CODEC_CTRL_s *pCodecCtrl);

#endif /* AMBA_AUDIO_CODEC_H */
