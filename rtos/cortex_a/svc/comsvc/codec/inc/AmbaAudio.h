/**
*  @file AmbaAudio.h
*
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*
*  @details task create/destroy functions
*
*/

#ifndef AMBA_AUDIO_H
#define AMBA_AUDIO_H

#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_ADEC.h"

#define AMBA_AUDIO_TYPE_PCM      (0U)
#define AMBA_AUDIO_TYPE_AAC      (1U)

#define AMBA_AUDIO_SOURCE_MP4      (0U)
#define AMBA_AUDIO_SOURCE_AUDIO    (1U)
#define AMBA_AUDIO_SOURCE_ROM_AUD  (2U)

#define AIN2AENC_OK         0x0000U
#define AIN2AENC_ERR_0000   0x0001U
#define AIN2AENC_ERR_0001   0x0002U
#define AIN2AENC_ERR_0003   0x0003U
#define AIN2AENC_ERR_0004   0x0004U

#define ADEC2AOUT_OK        0x0000U
#define ADEC2AOUT_ERR_0000  0x0001U
#define ADEC2AOUT_ERR_0001  0x0002U
#define ADEC2AOUT_ERR_0003  0x0003U
#define ADEC2AOUT_ERR_0004  0x0004U

/* Stereo: Setting AIN_CH_NUM = 2, AENC_CH_NUM = 2*/
/* Mono  : Setting AIN_CH_NUM = 2, AENC_CH_NUM = 1*/

/* AIN Info */
#define AIN_CH_NUM      2U
#define AIN_FRAME_SIZE  1024U
#define AIN_DESC_NUM    16U
#define AIN_IO_NUM      (CONFIG_AENC_NUM)

/* AENC Info */
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define AENC_CH_NUM      1U
#else
#define AENC_CH_NUM      2U
#endif
#define AENC_FRAME_SIZE  1024U
#define AENC_SAMPLE_FREQ 48000U
#define AENC_IO_NUM      1U

/* AOUT Info */
#define AOUT_CH_NUM      2U
#define AOUT_FRAME_SIZE  1024U
#define AOUT_DESC_NUM    16U
#define AOUT_IO_NUM      1U

/* ADEC Info */
#define ADEC_CH_NUM      2U
#define ADEC_FRAME_SIZE  1024U
#define ADEC_SAMPLE_FREQ 48000U
#define ADEC_IO_NUM      1U

#define WAIT_FLAG_TIMEOUT    10000U
#define AIN_DMA_STOP_FLG     0x00000001U
#define AIN_DMA_START_FLG    0x00000002U
#define AENC_STOP_FLG        0x00000001U
#define ADEC_DATA_READY_FLG  0x00000001U
#define ADEC_STOP_FLG        0x00000002U
#define ADEC_START_FLG       0x00000004U
#define AOUT_START_FLG       0x00000001U
#define AOUT_STOP_FLG        0x00000002U
#define AOUT_PAUSE_FLG       0x00000004U
#define AOUT_NO_WORK_IO_FLG  0x00000008U
#define AOUT_RUNNING_FLG     0x00000010U
#define AOUT_PROC_DONE       0x00000020U

#define SAMPLE_RESOLUTION       32U  /* I2S input is 32-bit */
#define AAC_SAMPLE_RESOLUTION   16U  /* AAC codec(Plug-in) use 16 bit */
#define PCM_SAMPLE_RESOLUTION   32U  /* PCM codec(Plug-in) use 32 bit */

typedef UINT32 (*AIN_STOP_CB)(void *pEventData);
typedef UINT32 (*AENC_DATA_HDLR_CB)(const void *pEventData);
typedef UINT32 (*ADEC_DATA_HDLR_CB)(void *pEventData);

typedef enum {
    AAC_BS_RAW  = 0U,
    AAC_BS_ADIF = 1U,
    AAC_BS_ADTS = 2U,
    AAC_BS_LOAS = 3U
} AMBA_AUDIO_AAC_BS_TYPE_e;

typedef struct {
    UINT8 svnrev[8];
    UINT8 svnhttp[128];
} audio_lib_info_t;

typedef struct {
    /* In: fill by user */
    UINT32                 Priority;
    AMBA_KAL_TASK_ENTRY_f  EntryFunc;
    UINT32                 EntryArg;
    void                   *pStackBase;
    UINT32                 StackSize;
    UINT32                 CpuBits;

    /* Out: valid after created */
    AMBA_KAL_TASK_t        Task;
} AMBA_AUDIO_TASK_CTRL_s;

typedef struct {
    AMBA_AIN_IO_CREATE_INFO_s AinInfo;
    AMBA_AIN_BUF_INFO_s       CachedInfo;
    AMBA_AIN_BUF_INFO_s       NonCachedInfo;
    char                      *pInputTskName;
    AMBA_AUDIO_TASK_CTRL_s    AinputTskCtrl;
} AMBA_AUDIO_INPUT_s;

typedef struct {
    AMBA_ADEC_BUF_INFO_s AdecCachedInfo;
} AMBA_AUDIO_DECODE_s;

typedef struct {
    UINT32  *pHead;
    UINT32   Size;
} AMBA_AUDIO_ENCODE_BS_INFO_s;

typedef struct {
    UINT32 *pAencHdlr;
    UINT32 *pAinHdlr;
    UINT32 NumOfAenc;
    UINT32 AencTskIdx;
    AMBA_AENC_CB_HDLR_s *pAencCbHdlr;
    AMBA_AIN_CB_HDLR_s *pAinCbHdlr;
    UINT8  I2SChannel;
    UINT32 FadingTime;
} AMBA_AUDIO_ENC_INFO_s;
typedef struct {
    AMBA_AUDIO_ENC_CREATE_INFO_s AencCreateInfo;
    AMBA_AENC_BUF_INFO_s         AencBufInfo;
    char                         *pAencTskName;
    UINT32                       AencTskIdx;
    AMBA_AUDIO_TASK_CTRL_s       AencTskCtrl[AMBA_AUDIO_MAX_BUF_NUM];
    AMBA_AUDIO_ENCODE_BS_INFO_s  pAencBsInfo;
    AMBA_AUDIO_ENC_INFO_s        AencInfo;
} AMBA_AUDIO_ENC_s;

/* AAC CODEC data structure */
typedef struct {
    UINT32  AencIndex;
    UINT32  BitstreamType;
    UINT32  Bitrate;
    UINT32  SampleFreq;
    UINT8   PerceptualMode;
    AMBA_AENC_PLUGIN_ENC_CS_s AencPlugInCs;
    AMBA_ADEC_PLUGIN_DEC_CS_s AdecPlugInCs;
} AMBA_AUDIO_AAC_CONFIG_s;

/* PCM CODEC data structure */
typedef struct {
    UINT32  AencIndex;
    UINT32  BitsPerSample;          /* 8, 16, 24, or 32-bits per sample */
    #define SVC_AUDIO_ENDIAN_L      (0U)
    #define SVC_AUDIO_ENDIAN_B      (1U)
    UINT32  DataFormat;             /* Intel (LSB,MSB), Motorola (MSB, LSB) */
    UINT32  FrameSize;
    AMBA_AENC_PLUGIN_ENC_CS_s AencPlugInCs;
    AMBA_ADEC_PLUGIN_DEC_CS_s AdecPlugInCs;
} AMBA_AUDIO_PCM_CONFIG_s;

typedef struct {
    UINT32                    Type;
    AMBA_AUDIO_AAC_CONFIG_s   AAC;
    AMBA_AUDIO_PCM_CONFIG_s   PCM;
    UINT32                    NvmID;
    ULONG                     BitsBufAddr;
    UINT32                    BitsBufSize;
    UINT32                    FeedTaskPriority;
    UINT32                    FeedTaskCpuBits;
} AMBA_AUDIO_SETUP_INFO_s;



typedef struct {
    UINT32 *pAdecHdlr;
    UINT32 *pAoutHdlr;
    AMBA_ADEC_CB_HDLR_s *pAdecCbHdlr;
    AMBA_AOUT_CB_HDLR_s *pAoutCbHdlr;
    UINT8  I2SChannel;
} AMBA_AUDIO_DEC_INFO_s;

typedef enum {
    AACENC_RAW = 0,                 /*!< Raw payload */
    AACENC_ADIF,                    /*!< ADIF header before first frame */
    AACENC_ADTS,                    /*!< ADTS MPEG-2 header */
    AACENC_ADTS_MP4,                /*!< ADTS MPEG-4 header */
    AACENC_LOAS                     /*!< LOAS, implicit SBR signalling */
} AAC_FILE_FORMAT_TYPE;


typedef struct {
    AMBA_AOUT_IO_CREATE_INFO_s AoutInfo;
    AMBA_AOUT_BUF_INFO_s       CachedInfo;
    AMBA_AOUT_BUF_INFO_s       NonCachedInfo;
    char                       *pOutputTskName;
    AMBA_AUDIO_TASK_CTRL_s     AoutputTskCtrl;
    UINT32                     OutputCnt;
} AMBA_AUDIO_OUTPUT_s;

typedef struct {
    AMBA_AUDIO_DEC_CREATE_INFO_s AdecInfo;
    AMBA_ADEC_BUF_INFO_s         AdecBufInfo;
    char                         *pAdecTskName;
    AMBA_AUDIO_TASK_CTRL_s       AdecTskCtrl;
} AMBA_AUDIO_DEC_s;


/* Op: 0-reset, 1-on, 2-off */
typedef void (*AMBA_AIN_CTRL_t)(UINT32 Chan, UINT32 Op);

UINT32 AmbaAudioInp_QueryBufSize(AMBA_AIN_IO_CREATE_INFO_s *pAinInfo);
UINT32 AmbaAudioInp_Create(const AMBA_AUDIO_INPUT_s *pAinInfo, UINT32 **pAinHdlr);
UINT32 AmbaAudioInp_IONodeRegister(const UINT32 *pAinHdlr, AMBA_AIN_CB_HDLR_s *pAinCbHdlr);
UINT32 AmbaAudioInp_Start(const AMBA_AUDIO_ENC_INFO_s *pAEncInfo, AMBA_AIN_CTRL_t pCbAinCtrl);
UINT32 AmbaAudioInp_Stop(const AMBA_AUDIO_ENC_INFO_s *pAEncInfo, AMBA_AIN_CTRL_t pCbAinCtrl);
UINT32 AmbaAudioInp_Delete(const UINT32 *pAinHdlr);

UINT32 AmbaAudioEnc_QueryBsSize(void);
UINT32 AmbaAudioEnc_QueryBufSize(AMBA_AUDIO_ENC_CREATE_INFO_s *pAencInfo);
UINT32 AmbaAudioEnc_Init(const AMBA_AUDIO_ENC_s *pEncInfo, UINT32 **pAencHdlr);
UINT32 AmbaAudioEnc_IONodeRegister(const UINT32 *pAencHdlr, AMBA_AENC_CB_HDLR_s *pAencCbHdlr);
UINT32 AmbaAudioEnc_Create(const UINT32 *pAencHdlr, UINT32  *pAencBsBuf, UINT32 BsBufSize, AENC_DATA_HDLR_CB AencDataHdlrCB);
UINT32 AmbaAudioEnc_Setup(const UINT32 *pAencHdlr, const AMBA_AUDIO_SETUP_INFO_s *pSetupInfo);
UINT32 AmbaAudioEnc_Start(const AMBA_AUDIO_ENC_s *pEncInfo);
UINT32 AmbaAudioEnc_Stop(const AMBA_AUDIO_ENC_INFO_s *pAEncInfo);
UINT32 AmbaAudioEnc_Delete(const UINT32 *pAencHdlr, AENC_DATA_HDLR_CB AencDataHdlrCB);

/* Op: 0-reset, 1-on, 2-off */
typedef void (*AMBA_AOUT_CTRL_t)(UINT32 Chan, UINT32 Op);

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 AmbaAudioEnc_SetMute(UINT8 Mute);
#endif

UINT32 AmbaAudioOup_QueryBufSize(AMBA_AOUT_IO_CREATE_INFO_s *pAoutInfo);
UINT32 AmbaAudioOup_Create(const AMBA_AUDIO_OUTPUT_s *pAoutInfo, UINT32 **pAoutHdlr);
UINT32 AmbaAudioOup_IONodeRegister(const UINT32 *pAoutHdlr, AMBA_AOUT_CB_HDLR_s *pAoutCbHdlr);
UINT32 AmbaAudioOup_Start(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, AMBA_AOUT_CTRL_t pCbAoutCtrl);
UINT32 AmbaAudioOup_Stop(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, UINT32 EosStop, AMBA_AOUT_CTRL_t pCbAoutCtrl);
UINT32 AmbaAudioOup_Delete(const UINT32 *pAoutHdlr, const AMBA_AOUT_CB_HDLR_s *pAoutCbHdlr);
UINT32 AmbaAudioOup_Pause(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, AMBA_AOUT_CTRL_t pCbAoutCtrl);
UINT32 AmbaAudioOup_Resume(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, AMBA_AOUT_CTRL_t pCbAoutCtrl);

UINT32 AmbaAudioDec_QueryBufSize(AMBA_AUDIO_DEC_CREATE_INFO_s *pAdecInfo);
UINT32 AmbaAudioDec_Init(const AMBA_AUDIO_DEC_s *pDecInfo, UINT32 **pAdecHdlr);
UINT32 AmbaAudioDec_IONodeRegister(const UINT32 *pAdecHdlr, AMBA_ADEC_CB_HDLR_s *pAdecCbHdlr);
UINT32 AmbaAudioDec_Create(const UINT32 *pAdecHdlr, UINT8  *pAdecBsBuf, UINT32 BsBufSize, ADEC_DATA_HDLR_CB AdecDataHdlrCB);
UINT32 AmbaAudioDec_Setup(const UINT32 *pAdecHdlr, const AMBA_AUDIO_SETUP_INFO_s *pSetupInfo);
UINT32 AmbaAudioDec_Start(const AMBA_AUDIO_DEC_INFO_s *pAdecInfo);
UINT32 AmbaAudioDec_Stop(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, UINT32 Eos);
UINT32 AmbaAudioDec_Delete(const UINT32 *pAdecHdlr, const AMBA_ADEC_CB_HDLR_s *pAdecCbHdlr, ADEC_DATA_HDLR_CB AdecDataHdlrCB);

#endif  /* AMBA_AUDIO_H */

