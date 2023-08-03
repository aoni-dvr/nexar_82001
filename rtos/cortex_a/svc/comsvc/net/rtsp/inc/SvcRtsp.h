/**
 *  @file SvcRtsp.h
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
 *  @details rtsp service
 *
 */

#ifndef SVC_RTSP_H
#define SVC_RTSP_H

#if defined(CONFIG_LINUX)
#ifndef VOID
#define VOID void
#endif
#ifndef UINT32
#define UINT32 uint32_t
#endif
#endif

#include "AmbaTypes.h"
#include <AmbaPrint.h>
#include <AmbaKAL.h>
#include <AmbaRTC.h>
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaDSP_VideoEnc.h"

#define SVC_RTSP_MAX_FN_SIZE (64U)

#define RTSP_SERVICE_NOTIFY_VID    (0x1U)
#define RTSP_SERVICE_NOTIFY_AUD    (0x2U)

#define RTSP_SERVICE_HANDLER_VID    (0x1U)
#define RTSP_SERVICE_HANDLER_AUD    (0x2U)

#define NET_RTSP_STREAM_VIDEO_ENABLE    (1U)        /* enable video stream */
#define NET_RTSP_STREAM_AUDIO_ENABLE    (2U)  /* enable audio stream */
#define NET_RTSP_STREAM_TEXT_ENABLE     (4U)  /* enable text stream */
#define NET_RTSP_STREAM_ALL             (NET_RTSP_STREAM_VIDEO_ENABLE | NET_RTSP_STREAM_AUDIO_ENABLE | NET_RTSP_STREAM_TEXT_ENABLE)

#define NET_RTSP_STREAM_NOTIFY_STARTENC         1U  /* Start encode. May from STOP_ENC or SWITCHENCSESSION */
#define NET_RTSP_STREAM_NOTIFY_STOPENC          2U  /* Stop encode then stay in idle. Such as menu operation or switch to thumbnail mode */
#define NET_RTSP_STREAM_NOTIFY_SWITCHENCSESSION 3U  /* Stop encode then start another encode session (VF <-> REC) */
#define NET_RTSP_STREAM_NOTIFY_STARTNETPLAY     4U  /* Start playback for streaming. set param1 as stream_id */
#define NET_RTSP_STREAM_NOTIFY_STOPNETPLAY      5U  /* Stop playback for streaming. set param1 as STARTENC to indecate APP is recording */
#define NET_RTSP_STREAM_NOTIFY_RELEASE          6U  /* RTOS RTSP mudule released */

typedef struct {
    UINT32 MessageID;       /* Message Id.*/
    UINT32 MessageData[2];  /* Message data.*/
    char Filename[SVC_RTSP_MAX_FN_SIZE];
} SVC_RTSP_NET_PB_MESSAGE_s;

typedef void (*SVC_RTSP_PB_f)(void);

typedef struct {
    UINT32 CurBitRate;      /* indicate the current bitrate */
    UINT32 MaxStableBR;     /* max stable bitrate */
    UINT32 LastBitRate;     /* indicate the bitrate when last RR come */
    UINT32 MaxBitRate;      /* the max bitrate we can reach in this sensor_config */
    UINT32 NetBandwidth;    /* network bandwidth */
    UINT32 ZeroLost;        /* indicate how many consecutive fr_lost=0 we got */
    UINT32 LastFrameLost;   /* fr_lost of last RR */
    UINT8 InMiddleOfIncrement;
    UINT8 InMiddleOfDecrement;
    UINT8 Inited;           /* indicate that whether some value is inited or not */
} NET_RTSP_STREAM_BITRATE_STAT_s;

typedef struct {
    INT32 (*Open)(void* Hdlr, UINT32 pipeId);           /* open invoked while switch from idle state to running states */
    INT32 (*Close)(void* Hdlr);                         /* close invoked while switch from running state to idle states */
    INT32 (*LiveviewStart)(void* Hdlr, UINT32 flag);    /* liveviewStart invoked while live view start */
    INT32 (*LiveviewStop)(void* Hdlr, UINT32 flag);     /* liveviewStart invoked while live view stop, flag: 0 - show bgcolor, 1 - show last frame */
    INT32 (*Start)(void* Hdlr, UINT32 flag);            /* start invoked while system start to encode */
    INT32 (*Pause)(void* Hdlr, UINT32 flag);            /* pause invoked while system pause */
    INT32 (*Resume)(void* Hdlr, UINT32 flag);           /* resume invoked while system restart to encode */
    INT32 (*Stop)(void* Hdlr, UINT32 flag);             /* stop invoked while system stop */
} SVC_ENC_AV_ENC_CODEC_s;

typedef struct {
    SVC_ENC_AV_ENC_CODEC_s *Function;   /* pointer to codec operation functions */
} SVC_ENC_AV_ENC_HDLR_s;

#define SVC_VIDEO_ENC_STREAM_ID_PRIMARY     0U
#define SVC_VIDEO_ENC_STREAM_ID_SECONDARY   1U
#define SVC_VIDEO_ENC_STREAM_ID_TERTIARY    2U
#define SVC_VIDEO_ENC_STREAM_ID_QUATERNARY  3U
#define SVC_VIDEO_ENC_STREAM_ID_NUM         4U

#define VIDEO_ENC_STREAM_TYPE_H264  0U
#define VIDEO_ENC_STREAM_TYPE_H265  1U
#define VIDEO_ENC_STREAM_TYPE_MJPEG 2U
#define VIDEO_ENC_STREAM_TYPE_NUM   3U

typedef struct {
    UINT32 Id;
    UINT32 VideoCodingFormat;
    SVC_ENC_AV_ENC_HDLR_s *HdlVideoEnc;
    SVC_ENC_AV_ENC_HDLR_s *HdlAudioEnc;
    SVC_ENC_AV_ENC_HDLR_s *HdlTextEnc;
    void *HdlEncMonitorBitrate;
    char StreamName[32];
    UINT32 Active;
} SVC_VIDEOENC_STREAM_INFO_s;

typedef struct {
    UINT32   StreamCount;
    UINT32  ActiveStreamID;
    SVC_VIDEOENC_STREAM_INFO_s  StreamList[SVC_VIDEO_ENC_STREAM_ID_NUM];
} SVC_VIDEOENC_STREAM_LIST_s;

#define SVC_FORMAT_VPS_GENERAL_CFG  12U /* The length of the general config in VPS */

#define MAX_FILENAME_LENGTH    (64U)     /* The maximum length of file names */

#define SVC_MEDIA_TRACK_TYPE_VIDEO  0x01U   /* The video track type */
#define SVC_MEDIA_TRACK_TYPE_AUDIO  0x02U   /* The audio track type */
#define SVC_MEDIA_TRACK_TYPE_TEXT   0x03U   /* The text track type */
#define SVC_MEDIA_TRACK_TYPE_MAX    0x04U   /* Maximum value (It is used to check the range of a media track type.) */

#define SVC_RTSP_MEDIA_TRACK_TYPE_VIDEO     0x01U   /* The track's type is Video */
#define SVC_RTSP_MEDIA_TRACK_TYPE_AUDIO     0x02U   /* The track's type is Audio */
#define SVC_RTSP_MEDIA_TRACK_TYPE_TEXT      0x03U   /* The track's type is Text */
#define SVC_RTSP_MEDIA_TRACK_TYPE_MAX       0x04U   /* Max value, for check use */

typedef struct {
    UINT32 EncoderStreamId;
    UINT32 TrackId;
} STREAM_TRACK_ID_s;

#define SVC_RTSP_MAX_TRACK_PER_MEDIA  2U     /* The max track number of a media. */

typedef struct {
    void *pPool;
    void *pPoolRaw;
} SVC_RTSP_STACK_s;

typedef struct {
    AMBA_KAL_MSG_QUEUE_t Hdlr;
    SVC_RTSP_NET_PB_MESSAGE_s *pPool;
    void *pPoolRaw;
} SVC_RTSP_MSG_QUEUE_s;

typedef struct {
    UINT8 Inited;
    SVC_RTSP_STACK_s Stack;
    AMBA_KAL_TASK_t Task;
    SVC_RTSP_MSG_QUEUE_s MsgQueue;
    SVC_RTSP_PB_f PlaybackEntry;
} SVC_RTSP_PB_MGR_s;

typedef struct {
    #define SVC_RTSP_AUD_AAC        (0U)
    #define SVC_RTSP_AUD_PCM        (1U)

    UINT32                        Format;     /**< 0: AAC, 1: PCM */
    AMBA_AUDIO_ENC_CREATE_INFO_s  *pEncInfo;
    UINT8                         *pBsBufBase;
    UINT32                        BsBufSize;
} SVC_RTSP_AUD_CONFIG_s;

typedef struct {
    UINT32                            RecStreamId;
    UINT32                            IsUsed;
    AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s  *pVideoCfg;
    SVC_RTSP_AUD_CONFIG_s             AudInfo;

    UINT32                            NumTrack;
    STREAM_TRACK_ID_s                 TrackId[SVC_RTSP_MAX_TRACK_PER_MEDIA];
} SVC_RTSP_STREAM_s;

typedef struct {
    UINT32                            *pNumStrm;
    AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s  *pStrmCfg;
} SVC_RTSP_ENC_INFO_TEMP_s;

UINT32 SvcRtsp_GetMemSize(UINT32 *pSize);
VOID   SvcRtsp_SetServiceBuf(ULONG RtspBufBase, UINT32 RtspBufSz);
UINT32 SvcRtsp_Init(VOID);
UINT32 SvcRtsp_DeInit(VOID);
VOID   SvcRtsp_SetInfo(UINT32 RecStreamId,
                     AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pVideoCfg,
                     const SVC_RTSP_AUD_CONFIG_s *pAudConfig);
UINT32 SvcRtsp_Notify(UINT32 RecStreamId, UINT32 Type, const void *pDesc);
VOID   SvcRtsp_Delete(UINT32 RecStreamId);

#if defined(CONFIG_NETX_ENET)
UINT32 SvcRtsp_NetxGetIP(ULONG* Ip);
#endif

#endif /*SVC_RTSP_H*/
