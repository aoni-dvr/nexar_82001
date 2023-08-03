/**
 *  @file SvcRtspServer.h
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
 *  @details rtsp server
 *
 */
#ifndef SVC_RTSP_SERVER_H
#define SVC_RTSP_SERVER_H

#include "AmbaTypes.h"
#include <AmbaKAL.h>
#include "NetStack.h"
#if defined(CONFIG_NETX_ENET)
#include "NetXStack.h"
#include "nx_bsd.h"
#elif defined(CONFIG_LWIP_ENET)
#define LWIP_PATCH_MISRA   // for misra depress
#include "lwip/sockets.h"
#endif
#include "SvcRtspClntSession.h"
#include "SvcMediaSession.h"

#define SVC_RTSP_MAX_SPS_LENGTH 96U
#define SVC_RTSP_MAX_PPS_LENGTH 64U

#if defined(CONFIG_NETX_ENET)
#ifndef htons
#define htons
#endif
#ifndef htonl
#define htonl
#endif
#ifndef ntohs
#define ntohs
#endif
#ifndef ntohl
#define ntohl
#endif
#endif

typedef UINT32 (*SVC_RTSP_CALLBACK_f)(void* Hdlr, UINT32 Event, const void* Info);
typedef struct {
    UINT32  Priority;
    UINT32  StackSize;
    UINT32  CoreSelection;
} SVC_RTSP_TASK_INFO_s;

typedef struct {
    SVC_RTSP_CALLBACK_f cbEvent;        /* rtsp server use this func to send status/event to system */
    SVC_RTSP_CALLBACK_f cbPlayback;     /* rtsp server use this func to send playback command to system */
    SVC_RTSP_CALLBACK_f cbMediaInfo;    /* rtsp server use this func to retrieve media information */
    SVC_RTSP_CALLBACK_f cbStreamList;   /* rtsp server use this func to retrieve system stream list */
    UINT8* MemoryPoolAddr;              /* buffer start address for rtsp server */
    UINT32 MemoryPoolSize;              /* size of buffer */
    UINT32 NumMaxClient;                /* max supported client connection */
    UINT32 NumMaxMediaSession;          /* max supported media session */
    UINT32 NumMaxStreamer;              /* max concurrent tracks(subsessions) available */
    UINT32 NumMaxActiveLiveGroup;       /* max concurrent active live stream group available */
    SVC_RTSP_TASK_INFO_s TaskInfo;      /* RTSP server task info */
    UINT32 GuardSize;                   /* guard size on top of stack */
} SVC_RTSPSERVER_INIT_CFG_s;

typedef struct {
    UINT16 VPSLen;                          /* The VPS size of H265 */
    UINT16 SPSLen;                          /* The SPS size of H265 */
    UINT16 PPSLen;                          /* The PPS size of H265 */
    UINT8 VPS[SVC_RTSP_MAX_SPS_LENGTH];     /* The VPS of H265 */
    UINT8 SPS[SVC_RTSP_MAX_SPS_LENGTH];     /* The SPS of H265 */
    UINT8 PPS[SVC_RTSP_MAX_PPS_LENGTH];     /* The PPS of H265 */
} SVC_RTSP_H265_ENTRY_s;

typedef struct {
    UINT16 SPSLen;                          /* The SPS size of H264 */
    UINT16 PPSLen;                          /* The PPS size of H264 */
    UINT8 SPS[SVC_RTSP_MAX_SPS_LENGTH];     /* The SPS of H264 */
    UINT8 PPS[SVC_RTSP_MAX_PPS_LENGTH];     /* The PPS of H264 */
} SVC_RTSP_H264_ENTRY_s;

typedef struct {
    UINT32 nGOPSize;                    /* The number of the picture between IDR pictures. */
    UINT32 nTrickRecDen;                /* For AmpFormat_ConvertPTS(), the Denominator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    UINT32 nTrickRecNum;                /* For AmpFormat_ConvertPTS(), the Numerator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    UINT32 nCodecTimeScale;             /* TODO: It needs get from H264 bitstream, it is not configurable. */
    UINT16 nWidth;                      /* Picture width */
    UINT16 nHeight;                     /* Picture height */
    UINT16 nM;                          /* The number of the picture between reference pictures(IDR, I, P) */
    UINT16 nN;                          /* The number of the picture between I pictures */
    UINT16 nIRCycle;                    /* Intra refresh cycle */
    UINT16 nRecoveryFrameCnt;           /* The value of recovery_frame_cnt in SEI-recovery point */
    UINT8 Default;                      /* The flag defines the track as default video track, if the media hasn't one video track. */
    #define SVC_VIDEO_MODE_P                (0U)    /**< Progressive */
    #define SVC_VIDEO_MODE_I_A_FRM_PER_SMP  (1U)    /**< Frame per sample */
    #define SVC_VIDEO_MODE_I_A_FLD_PER_SMP  (2U)    /**< Field per sample */
    UINT8 nMode;                        /* The value defines the picture mode of the video. It has progressive and interlaced mode. Interlaced mode has Field Per Sample and Frame Per Sample, See AMP_VIDEO_MODE_s */
    UINT8 bClosedGOP;                   /* The structure of the Close GOP is I P B B P B B. The structure of the Open GOP is I B B P B B, If resume or auto split, the value always is Open GOP. */
    union {
        SVC_RTSP_H264_ENTRY_s H264;     /* H264 specific information */
        SVC_RTSP_H265_ENTRY_s h265;     /* H265 specific information */
    } xInfo;
} SVC_RTSP_VIDEO_TRACK_CFG_s;

typedef struct {
    UINT32 SampleRate;          /* The sample rate(Hz) of the audio track. */
    UINT8 Default;              /* The flag defines the track as default audio track, if the media hasn't one audio track. */
    UINT8 Channels;             /* The number of audio channel. */
    UINT8 BitsPerSample;        /* The per sample size of the audio track. Ex: 8 bits, 16 bits, ....n bits */
} SVC_RTSP_AUDIO_TRACK_CFG_s;

typedef struct {
    UINT8 Default;              /* The flag defines the track as default text track, if the media hasn't one text track. */
} SVC_RTSP_TEXT_TRACK_CFG_s;

typedef struct {
    UINT32 nMediaId;                        /* The media type of the track. The id is media id, See AMP_FORMAT_MID_e */
    UINT32 nTimeScale;                      /* Time scale, the same as the LCM of those of all tracks */
    UINT32 nTimePerFrame;                   /* The time of the frame that the unit of it is Time scale. */
    UINT32 nInitDelay;                      /* Initial delay time(ms) of the track. */
    void *hCodec;                           /* the codec which this track is working on. */
    UINT32 Codec;
    UINT8 *pBufferBase;                     /* The start address of the FIFO of the track. User pushs data to the FIFO, the FIFO will write data to the address of the buffer. */
    UINT8 *pBufferLimit;                    /* The end address of the FIFO of the track, The data can't write overlap the address, FIFO size = FIFO buffer limit - FIFO buffer base. */
    union {
        SVC_RTSP_VIDEO_TRACK_CFG_s Video;   /* See AMP_RTSP_VIDEO_TRACK_CFG_s, the information of the video track. */
        SVC_RTSP_AUDIO_TRACK_CFG_s Audio;   /* See AMP_RTSP_AUDIO_TRACK_CFG_s, the information of the audio track. */
        SVC_RTSP_TEXT_TRACK_CFG_s Text;     /* See AMP_RTSP_TEXT_TRACK_CFG_s, the information of the text track. */
    } Info;
    UINT32 nTrackType;                       /* Track type, See AMP_MEDIA_TRACK_TYPE_e */
} SVC_RTSP_MEDIA_TRACK_CFG_s;

typedef struct {
    SVC_RTSP_MEDIA_TRACK_CFG_s Track[SVC_RTSP_MAX_TRACK_PER_MEDIA];     /* See AMP_RTSP_MEDIA_TRACK_CFG_s, the tracks in the movie. */
    UINT32 nTrack;                                                       /* The number of Tracks in the movie. */
    UINT32 nDuration;                                                   /* The duration of this movie(only valid for rtsp playback). */
} SVC_RTSP_MOVIE_INFO_CFG_s;

#define SVC_RTSP_NOTIFY_STARTENC                1U           /* Start encode. May from STOP_ENC or SWITCHENCSESSION */
#define SVC_RTSP_NOTIFY_STOPENC                 2U           /* Stop encode then stay in idle. Such as menu operation or switch to thumbnail mode */
#define SVC_RTSP_NOTIFY_SWITCHENCSESSION        3U           /* Stop encode then start another encode session (VF <-> REC) */
#define SVC_RTSP_NOTIFY_STARTNETPLAY            4U           /* Start playback for streaming. set param1 as stream_id */
#define SVC_RTSP_NOTIFY_STOPNETPLAY             5U           /* Stop playback for streaming. set param1 as STARTENC to indecate APP is recording */
#define SVC_RTSP_NOTIFY_RELEASE                 6U           /* RTOS RTSP mudule released */

typedef struct {
    UINT32 FrameLost;                   /* The fraction of packet lost since last RR (FrameLost/255)*/
    UINT32 Jitter;                      /* The interarrival jitter */
    DOUBLE PGDelay;                     /* SR+RR on the air time */
} SVC_RTSP_RTCP_REPORT_s;


#define SVC_RTSP_FIFO_EVENT_START               1U            /* The start event of the RTSP fifo. The RTSP fifo created */
#define SVC_RTSP_FIFO_EVENT_END                 2U            /* The end event of the RTSP fifo. The RTSP fifo deleted */
#define SVC_RTSP_FIFO_EVENT_SWITCHENCSESSION    3U            /* The end event of the RTSP fifo. The RTSP fifo deleted for enc session switching. */
#define SVC_RTSP_RTCP_RR_STAT                   4U            /* The event of new RTCP Receiver Report */
#define SVC_RTSP_RTCP_RESET                     5U            /* The session is gone, reset the statistic and setting which caused by RR_STAT */
#define SVC_RTSP_EVENT_GENERAL_ERROR            0xF0U         /* The error event of the RTSP. */


#define SVC_RTSP_MEDIA_CMD_GET_STREAM_LIST  (0x0001U)
#define SVC_RTSP_MEDIA_CMD_GET_INFO         (0x0002U)

#define SVC_RTSP_STREAMLIST_MAX_ENTRY 16U
typedef struct {
    const char* Name;                   /* The stream name used for comparing with RTSP url */
    UINT32 Active;                      /* Indicate whether this stream is playable or not */
} SVC_RTSP_STREAM_ITEM_s;

typedef struct {
    UINT32 Amount;                                                                 /* Amount of valid list entry. */
    SVC_RTSP_STREAM_ITEM_s StreamItemList[SVC_RTSP_STREAMLIST_MAX_ENTRY];       /* Stream item list. */
} SVC_RTSP_MEDIA_STREAM_ITEM_LIST_s;


#define SVC_RTSP_PLAYBACK_OPEN            1U         /* open file for playback. */
#define SVC_RTSP_PLAYBACK_PLAY            2U         /* start playback. */
#define SVC_RTSP_PLAYBACK_STOP            3U         /* stop playback */
#define SVC_RTSP_PLAYBACK_RESET           4U         /* reset playback */
#define SVC_RTSP_PLAYBACK_PAUSE           5U         /* pause playback */
#define SVC_RTSP_PLAYBACK_RESUME          6U         /* resume playback */
#define SVC_RTSP_PLAYBACK_SET_PARAMETER   7U         /* send RTSP extend field */


#define SVC_RTSP_FRAME_TYPE_MJPEG_FRAME         0U
#define SVC_RTSP_FRAME_TYPE_IDR_FRAME           1U
#define SVC_RTSP_FRAME_TYPE_I_FRAME             2U
#define SVC_RTSP_FRAME_TYPE_P_FRAME             3U
#define SVC_RTSP_FRAME_TYPE_B_FRAME             4U
#define SVC_RTSP_FRAME_TYPE_JPEG_FRAME          5U
#define SVC_RTSP_TYPE_THUMBNAIL_FRAME           6U
#define SVC_RTSP_FRAME_TYPE_SCREENNAIL_FRAME    7U
#define SVC_RTSP_FRAME_TYPE_AUDIO_FRAME         8U
#define SVC_RTSP_FRAME_TYPE_UNDEFINED           9U
#define SVC_RTSP_FRAME_TYPE_EOS                 255U
#define SVC_RTSP_FRAME_TYPE_LAST                SVC_RTSP_FRAME_TYPE_EOS


#define SVC_RTSP_FRAME_MARK_EOS                 0x00FFFFFFU

typedef struct {
    UINT32  SeqNum;            /* sequential number of bits buffer */
    UINT64  Pts;         /* time stamp in ticks */
    UINT32  Type;             /* data type of the entry. see AMBA_EXAMFRAMESHARE_FRAME_TYPE_e*/
    UINT8   Completed;        /* if the buffer content a complete entry */
    UINT16  Align;          /* data size alignment (in bytes, align = 2^n, n is a integer )*/
    ULONG   StartAddr;   /* start address of data */
    UINT32  Size;              /* real data size */
} SVC_RTSP_FRAME_DESC_s;

#define SVC_RTSP_POOL_SDP_LINE_BUF      0U
#define SVC_RTSP_POOL_VPS_SPS_PPS_BUF   1U
#define SVC_RTSP_POOL_RESPONSE_BUF      2U
#define SVC_RTSP_POOL_MAX               3U

typedef struct {
    UINT8       *pPtr;
    UINT8       *pIndex;
    UINT32      Number;
    UINT32      Size;
} SVC_RTSP_SERVER_MEM_POOL_s;

#define SVC_RTSP_SESSION_PLAY 0U
#define SVC_RTSP_SESSION_SEEK 1U
#define SVC_RTSP_SESSION_RESUME 2U

#if defined(CONFIG_NETX_ENET)
#define EAGAIN 11               /* No more processes */
#define EWOULDBLOCK EAGAIN      /* Operation would block */
#define ENOBUFS 105             /* No buffer space available */
/*#define ENOTCONN 128*/        /* Socket is not connected */
#endif

ULONG ALIGN_32(ULONG x);
#define SVC_RTSP_SERVER_DEFAULT_CORE 1U
#define SVC_RTSP_SERVER_STACK_SIZE (10U*1024U)
#define SVC_RTSP_SERVER_GUARD_SIZE 1024U
#define SVC_RTSP_SERVER_DEFAULT_PORT 554U
#define SVC_RTSP_SERVER_BYTE_POOL_SIZE ((UINT32)6U << 10U)

#define SVC_RTSP_MAX_CLIENT_SESSION 1U
#define SVC_RTSP_MAX_MEDIA_SESSION (SVC_RTSP_MAX_CLIENT_SESSION*2U)
#define SVC_RTSP_MAX_IO_WATCHER 8U
#define SVC_RTSP_TIMEOUT_MS 60000U
#define SVC_RTSP_TIMEOUT_S 60

typedef void (*watcher_cb)(INT32 Sd, const void* Ctx);
typedef struct {
    INT32 Sd;
    watcher_cb func;
    void* Ctx;
} IO_WATCHER_s;

#define RTSP_SERVER_FLAG_SOCKET_DONE (1U)
#define RTSP_SERVER_FLAG_SOCKET_FAIL (2U)
#define RTSP_SERVER_FLAG_START       (4U)

typedef struct {
    AMBA_KAL_TASK_t Task;
    SVC_RTSP_TASK_INFO_s TaskInfo;
    AMBA_KAL_EVENT_FLAG_t EventFlag;
    UINT32 RunningFlag;
    UINT8* Stack;
    UINT8* GuardAddr;
    UINT32 GuardSize;
    UINT32 Port;
    fd_set MasterList;
    fd_set ReadReady;
    int    MaxSd;
    IO_WATCHER_s  WatchTable[SVC_RTSP_MAX_IO_WATCHER];
    UINT8 MaxClient;
    UINT16 MaxMediaSession;
    UINT16 MaxSubsession;
    UINT8* ResponseBuf;
    SVC_RTSP_CLNTSESSION_s* ClntList;
    void *SessionList;
    void *SubSessionList;
    SVC_RTSP_CALLBACK_f cbEvent;
    SVC_RTSP_CALLBACK_f cbPlayback;
}SVC_RTSP_SERVER_s;

/** just for misra-c */
static inline void MisraCUnused(void)
{
    static struct sockaddr SockAddr;
    static struct timeval Timeval;

    (void)SockAddr.sa_family;
    (void)Timeval.tv_sec;
}


SVC_RTSP_CLNTSESSION_s* SvcRtspServer_GetClientSlot(SVC_RTSP_SERVER_s const * pSrv);

void* SvcRtspServer_GetSessionSlot(SVC_RTSP_SERVER_s const * pSrv);

void* SvcRtspServer_GetSubsessionSlot(SVC_RTSP_SERVER_s const * pSrv);

void* SvcRtspServer_LookupMediaSession(SVC_RTSP_SERVER_s const *pSrv, const char* Name);

UINT32 SvcRtspServer_GetPlaybackSessionCount(SVC_RTSP_SERVER_s const * pSrv);

UINT32 SvcRtspServer_GetRTSP_URL(SVC_RTSP_SERVER_s const * pSrv, void const *pSession,
                              char* pBuf, UINT32 Len, UINT32 IpAddress);
INT32 SvcRtspServer_ReportStatus(SVC_RTSP_SERVER_s const * pSrv, UINT32 Event, void const * pInfo);

UINT32 SvcRtspServer_PlaybackCmd(SVC_RTSP_SERVER_s const * pSrv, UINT32 Op, void const * pInfo);
UINT32 SvcRtspServer_RegWatcher(SVC_RTSP_SERVER_s* pSrv, INT32 Sd, watcher_cb EventFunc, void * pCtx);
UINT32 SvcRtspServer_UnregWatcher(SVC_RTSP_SERVER_s* pSrv, INT32 Sd);

UINT32 SvcRtspServer_MemPool_Init(void);
void *SvcRtspServer_MemPool_Malloc(UINT32 Num, UINT32 Size);
void SvcRtspServer_MemPool_Free(UINT32 Num, void const *pPtr);
UINT32 SvcRtspServer_GetBufferSize(void);
void* AmbaRTSPServer_Malloc(INT32 size);
UINT32 SvcRtspServer_ClearSd(SVC_RTSP_SERVER_s* pSrv, INT32 Sd);

//host to network order
UINT32 SvcRtspServer_ihtonl(UINT32 n);
UINT16 SvcRtspServer_ihtons(UINT16 n);
UINT16 SvcRtspServer_intohs(UINT16 n);
UINT32 SvcRtspServer_intohl(UINT32 n) ;


UINT32 SvcRtspServer_GetInitDefaultCfg(SVC_RTSPSERVER_INIT_CFG_s *pDefaultCfg);
UINT32 SvcRtspServer_GetRequiredMemorySize(SVC_RTSPSERVER_INIT_CFG_s const *pCfg, UINT32 *pMemSize);
UINT32 SvcRtspServer_Init(SVC_RTSPSERVER_INIT_CFG_s const *Cfg);
UINT32 SvcRtspServer_Release(void);
UINT32 SvcRtspServer_Start(void);
UINT32 SvcRtspServer_Stop(void);
UINT32 SvcRtspServer_Notify(UINT32 Notify, UINT32 Param1, UINT32 Param2);

 #endif /* SVC_RTSP_SERVER_H */

