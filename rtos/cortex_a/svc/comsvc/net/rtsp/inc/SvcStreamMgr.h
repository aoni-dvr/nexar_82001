/**
 *  @file SvcStreamMgr.h
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
 *  @details stream manager
 *
 */

#ifndef SVC_STREAM_MGR_H
#define SVC_STREAM_MGR_H

#include "AmbaTypes.h"

#define STREAM_MARK_DROP_FRAME 0x0fffffffU

#define STREAM_MGR_STACK_SIZE                   (12U*1024U)
#define STREAM_MGR_DEFAULT_CORE                 1U
#define STREAM_MGR_GUARD_SIZE                   1024U
#define STREAM_MGR_QUEUE_NUMBER                 512U
#define STREAM_MGR_MAX_STREAMERS                12U
#define STREAM_MGR_MAX_ACTIVE_LIVE_STREAM_GROUP 4U
#define STREAM_MGR_MAX_STREAM_NAME_LENGTH       64U
#define STREAM_MGR_MAX_STREAMER_PER_GROUP       SVC_RTSP_MAX_TRACK_PER_MEDIA

typedef struct {
    UINT8 Vps[SVC_RTSP_MAX_SPS_LENGTH];    /* H265 VPS */
    UINT32 VpsLen;                         /* H265 VPS size */
    UINT8 Sps[SVC_RTSP_MAX_SPS_LENGTH];    /* H265 or H264 SPS */
    UINT32 SpsLen;                         /* H265 or H264 SPS size */
    UINT8 Pps[SVC_RTSP_MAX_PPS_LENGTH];    /* H265 or H264 PPS */
    UINT32 PpsLen;                         /* H265 or H264 PPS size */
    UINT16 M;                               /* The number of the picture between reference pictures(IDR, I, P) */
    UINT16 N;                               /* The number of the picture between I pictures */
    UINT32 GOPSize;                         /* The number of the picture between IDR pictures. */
    UINT16 IRCycle;                         /* Intra refresh cycle */
    UINT16 RecoveryFrameCnt;                /* The value of recovery_frame_cnt in SEI-recovery point */
    UINT32 ProfileLevelId;
    UINT32 Updated;
} SVC_STREAM_VIDEO_PARAM_s;

typedef struct {
    UINT32 SampleRate;         /* The sample rate(Hz) of the audio track. */
    UINT8 Default;             /* The flag defines the track as default audio track, if the media hasn't one audio track. */
    UINT8 Channels;            /* The number of audio channel. */
    UINT8 BitsPerSample;       /* The per sample size of the audio track. Ex: 8 bits, 16 bits, ....n bits */
} SVC_STREAM_AUDIO_PARAM_s;

typedef struct {
    UINT32 Nothing;
} SVC_STREAM_TEXT_PARAM_s;

typedef void (*STREAM_CB)(SVC_RTSP_FRAME_DESC_s const * Desc, void const* Ctx);

typedef struct {
    AMBA_KAL_MUTEX_t Mtx;       /* Mutex to protect this structure */
    STREAM_CB StreamFunc;     /* The callback streamerMgr should use upon frame ready */
    void* StreamCtx;          /* Context used by streamerFunc */
    UINT32 Enable;               /* Indicate whether need to call streamerFunc or not */
    UINT32 TimerReg;             /* Indicate whether playback timer registered or not */
    UINT32 IsLive;               /* Indicate whether this is live view streamer */
    UINT32 TrackID;              /* track ID in the media */
    UINT32 MediaID;             /* media ID of this stream */
    UINT32 FifoHndlr;           /* AMP Fifo Handler*/
    UINT32 Codec;
    ULONG  hCodec;              /* AMP Codec */
    UINT8 *pBufferBase;         /* The start address of the FIFO of the track */
    UINT8 *pBufferLimit;        /* The end address of the FIFO of the track*/
    UINT32 TimeScale;           /* Frame timing information */
    UINT32 TickPerFrame;        /* Ticks of each frame */
    AMBA_KAL_TIMER_t Timer;     /* Timer for playback stream */
    UINT32 TimerTick;           /* Tick of the timer */
    DOUBLE DeviationPerFrame;   /* The accuracy of timer is ms, there will be us part deviation */
    DOUBLE Deviation;           /* The current deviation of the streamer */
    UINT32 TrackType;
    union {
        SVC_STREAM_VIDEO_PARAM_s Video;     /* Video specific parameter */
        SVC_STREAM_AUDIO_PARAM_s Audio;     /* Audio specific parameter */
        SVC_STREAM_TEXT_PARAM_s Text;       /* Text specific parameter */
    } Param;
}AMBA_STREAM_s;

typedef struct {
    AMBA_KAL_MUTEX_t Mtx;                                   /* Mutex to protect this structure */
    char Name[STREAM_MGR_MAX_STREAM_NAME_LENGTH];           /* url-suffix of this streamer group */
    AMBA_STREAM_s* Stms[STREAM_MGR_MAX_STREAMER_PER_GROUP];  /* streamers of this group */
} SVC_STREAM_GROUP_s;

#define STREAMER_MSG_DATA_READY             0U      /* frame ready */
#define STREAMER_MSG_PERIODIC_DATA_READY    1U      /* periodic frame ready, for playback use */
#define STREAMER_MSG_EOS                    2U      /* eos frame */
#define STREAMER_MSG_ENABLE                 3U      /* enable streamer */
#define STREAMER_MSG_DISABLE                4U      /* disable streamer */
#define STREAMER_MSG_RESET_FIFO             5U      /* reset fifo of streamer */
#define STREAMER_MSG_LIVE_STARTENC          6U      /* encoder start encode, streamer could create vfifo*/
#define STREAMER_MSG_LIVE_STOPENC           7U      /* encoder stop encode, streamer could delete vfifo*/
#define STREAMER_MSG_LIVE_SWITCHSESSION     8U      /* encoder stop encode, streamer could delete vfifo*/

typedef struct {
    UINT32 Event;               /* Equal to AMP_FIFO_EVENT_e */
    UINT32 Info;                /* Info based on event type */
    SVC_RTSP_FRAME_DESC_s Desc; /* FrameDescription */
} SVC_STREAM_MSG_S;

typedef struct {
    AMBA_STREAM_s* StreamList;        /* Streamer instances pool */
    SVC_RTSP_CALLBACK_f cbEvent;            /* callback for sending event to Applib */
    SVC_RTSP_CALLBACK_f cbMediaInfo;        /* callback for retrieving media information */
    SVC_RTSP_CALLBACK_f cbStreamList;       /* callback for retrieving system stream list */
    UINT32 MaxLiveGroup;
    UINT32 MaxStreamer;
    UINT32 MaxStreamerPerLiveGroup;
    SVC_STREAM_GROUP_s* LiveGroup;          /* live group instances pool */
    UINT32 LiveState;                       /* indicate the state of liveGroup */
    AMBA_KAL_TASK_t Task;
    UINT8* Stack;
    UINT8* MsgPool;
    AMBA_KAL_MSG_QUEUE_t MsgQueue;
    UINT8* GuardAddr;
    UINT32 GuardSize;
} SVC_STREAM_MGR_s;

typedef struct {
    SVC_RTSP_TASK_INFO_s TaskInfo;
    UINT8* MemoryPoolAddr;              /* buffer start address for rtsp server */
    UINT32 MemoryPoolSize;              /* size of buffer */
    UINT32 MsgQueueNumber;              /* number of entries per message queue */
    UINT32 NumMaxStreamer;              /* max streamer resources */
    UINT32 NumMaxActiveLiveGroup;       /* max live group resources */
    SVC_RTSP_CALLBACK_f cbEvent;
    SVC_RTSP_CALLBACK_f cbMediaInfo;
    SVC_RTSP_CALLBACK_f cbStreamList;
    UINT32 GuardSize;                   /* guard size on top of stack */
} SVC_STREAM_MGR_INIT_CFG_s;

typedef struct {
    UINT32 MediaId;
    AMBA_STREAM_s* Stm;
} SVC_STREAM_LIVE_MEDIA_s;

UINT32 SvcRtspStrmNotify(UINT32 hndlr, const SVC_RTSP_FRAME_DESC_s *Desc);
UINT32 SvcRtspStrmGetVpsSpsPps(AMBA_STREAM_s* Strm,
                                            char *Vps, UINT32 *VpsLen,
                                            char *Sps, UINT32 *SpsLen,
                                            char *Pps, UINT32 *PpsLen);
UINT32 SvcRtspStrmGetSpsPps(AMBA_STREAM_s* Strm,
                                        char *Sps, UINT32 *SpsLen,
                                        char *Pps, UINT32 *PpsLen,
                                        UINT32 *profileLevelID);
void SvcRtspStrmResetFifo(const AMBA_STREAM_s* Strm);
void SvcRtspStrmEnable(const AMBA_STREAM_s* Strm);
void SvcRtspStrmDisable(const AMBA_STREAM_s* Strm);
AMBA_STREAM_s* SvcRtspStrmCreate(void);
UINT32 SvcRtspStrmDelete(AMBA_STREAM_s* Strm);
UINT32 SvcRtspStrmReport(const AMBA_STREAM_s* Strm, UINT32 Event, void* Info);
UINT32 SvcRtspStrmChangeTickTimer(AMBA_STREAM_s* Strm, UINT32 PrevTick, UINT32 Tick);
AMBA_STREAM_s* SvcRtspStrmBind(SVC_RTSP_MEDIA_TRACK_CFG_s* Trk, AMBA_STREAM_s* Strm,
                                    STREAM_CB StreamFunc, void *Ctx);
UINT32 SvcRtspStrmGetMediaID(const char* StreamName, SVC_STREAM_LIVE_MEDIA_s* MediaList);
UINT32 SvcStrmMgrNotify(UINT32 Notify);
UINT32 SvcRtspStrmMgrGetMediaInfo(void* Hdlr, UINT32 Event , const void* Info);
UINT32 SvcRtspStrmMgrGetStreamList(void* Hdlr, UINT32 Event, const void* Info);
UINT32 SvcRtspStrmMgrGetRequiredMemorySize(const SVC_STREAM_MGR_INIT_CFG_s *Cfg, UINT32* MemSize);
UINT32 SvcRtspStrmMgrGetInitDefaultCfg(SVC_STREAM_MGR_INIT_CFG_s *DefaultCfg);
UINT32 SvcRtspStrmMgrInit(const SVC_STREAM_MGR_INIT_CFG_s *Cfg);
UINT32 SvcStrmMgrRelease(void);

#endif /*SVC_STREAM_MGR_H*/
