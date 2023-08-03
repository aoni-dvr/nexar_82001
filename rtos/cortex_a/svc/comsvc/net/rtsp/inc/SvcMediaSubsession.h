/**
 *  @file SvcMediaSubsession.h
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
 *  @details media subsession
 *
 */
#ifndef SVC_MEDIA_SUBSESSION_H
#define SVC_MEDIA_SUBSESSION_H

#include "AmbaTypes.h"
#include "SvcRtspServer.h"
#include "SvcRtpSink.h"

#define MTU_SIZE                        1500U
#define IP_HEADER_SIZE                  20U
#define UDP_HEADER_SIZE                 8U
#define TCP_HEADER_SIZE                 20U
#define RTP_HEADER_SIZE                 12U

#define RTP_OVER_UDP_MAX (((MTU_SIZE - IP_HEADER_SIZE) - UDP_HEADER_SIZE) - RTP_HEADER_SIZE)
#define INTERLEAVED_HEADER 4U
#define RTP_OVER_TCP_MAX ((((MTU_SIZE - IP_HEADER_SIZE) - TCP_HEADER_SIZE) - RTP_HEADER_SIZE) - INTERLEAVED_HEADER)

#define MAX_SUBSESSION_TRANSPORT MAX_RTP_SESSION

#define MAX_SEI_RECOVERY_PKT_LENGTH     12U //large enough

typedef struct {
    UINT32 GotFirstIdr;
    char SeiRecoveryPkt[MAX_SEI_RECOVERY_PKT_LENGTH];
    UINT32 SeiRecoveryLen;
} H264_PAYLOAD_CONTEX_s;

typedef struct {
    UINT32 SamplingFrequency;
    UINT32 ChannelConfiguration;
} AAC_PAYLOAD_CONTEX_s;

typedef struct {
    UINT32 SamplingFrequency;
    UINT32 ChannelNum;
    UINT32 BitsPerSample;
    UINT32 CodingLE; // 1: LittleEndian or 0: BigEndian
} PCM_PAYLOAD_CONTEXT_s;

typedef struct {
    UINT32 Session;
    RTP_TRANSPORT_s Transport;
} TRANSPORT_MAP_s;

typedef struct {
    void *Srv;
    void *Parent;
    void *Streamer;      /* pointer of the streamer instance we create */
    TRANSPORT_MAP_s Map[MAX_SUBSESSION_TRANSPORT];
    RTP_SINK_s Rtp;
    UINT16 Used;                        /* the number of clients using this subsession */
    UINT8 IsLive;                       /* true: live stream, false: playback stream */
    UINT8 MaxTransport;                 /* the maximum allowed transport per subsession */

    //for Packtize
    UINT32 LastPts;
    UINT32 CurTimestamp;
    UINT32 TotalReadFrames;
    UINT32 NextSrPts;
    UINT32 ChangeTick;                  /* indicate whether there is pts tick change from encoder/demuxer */

    //payload type specific data
    UINT32 CodecID;                     /* identifier of the codec */
    union {
        H264_PAYLOAD_CONTEX_s H264;
        AAC_PAYLOAD_CONTEX_s Aac;
        PCM_PAYLOAD_CONTEXT_s Pcm;
    }PayloadCtx;

    //for RTSP cmd
    const char* (*GetSdpLines)(void const *pthiz);
    INT32 (*FreeSdpLines)(const void *pthiz);
    UINT32 TrackNum;
    UINT32 RefClock;
    char* SdpLines;
    char* AuxSdpLine;
    char TrackId[8];                /*  enough for track0 ... track10 */

    //for streamerFunc ctx
    DOUBLE VectorClk;               /* save the refClock/TimeScale, convert pts to correct clock domain */
    UINT32 TicksPerFrame;           /* calculated ticks per frame according to refClock/TimeScale/TimesPerFrame */
    UINT8 *pBufferBase;             /* The start address of the FIFO buffer of the track. */
    UINT8 *pBufferLimit;            /* The end address of the FIFO buffer of the track. */
} MEDIA_SUBSESSION_s;

INT32 make_socket_nonblocking(INT32 sock) ;
const char* SvcMediaSubsession_GetTrackID(const void *pthiz);
INT32 SvcMediaSubsession_SetupUDPTransport(const void *pSub, UINT32 sessionid,
                                    struct in_addr clnt_addr,
                                    UINT32 clntRTPPortNum,
                                    UINT32 clntRTCPPortNum,
                                    UINT32* servRTPPortNum,
                                    UINT32* servRTCPPortNum,
                                    void* clientSession);
UINT32 SvcMediaSubsession_StartStream(const void *pSub, UINT32 sessionid, UINT32 op,
                                    UINT16 *rtpseqnum,
                                    UINT32 *RtpTimestamp);
UINT32 SvcMediaSubsession_PauseStream(const void *pSub, UINT32 sessionid);
UINT32 SvcMediaSubsession_TearDown(const void *pSub, UINT32 sessionid);

MEDIA_SUBSESSION_s* SvcMediaSubsession_Create(const void *pSrv, const void *Info, UINT32 IsLive);
void SvcMediaSubsession_Release(const void *pSub);

MEDIA_SUBSESSION_s* SvcH265Subsession_Create(void *pSrv, const void* Info, UINT32 IsLive);
MEDIA_SUBSESSION_s* SvcH264Subsession_Create(void const * pSrv, void const * pInfo, UINT32 IsLive);
MEDIA_SUBSESSION_s* SvcAacSubsession_Create(void const * pSrv, void const * pInfo, UINT32 IsLive);
MEDIA_SUBSESSION_s* SvcPcmSubsession_Create(void const * pSrv, void const * pInfo, UINT32 IsLive);

#endif /*SVC_MEDIA_SUBSESSION_H*/

