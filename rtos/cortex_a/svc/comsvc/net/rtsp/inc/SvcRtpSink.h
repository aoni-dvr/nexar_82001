/**
 *  @file SvcRtpSink.h
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
 *  @details rtp sink
 *
 */
#ifndef SVC_RTP_SINK_H
#define SVC_RTP_SINK_H

#include "NetStack.h"
#include "AmbaKAL.h"
#include <AmbaTypes.h>
#if defined(CONFIG_NETX_ENET)
#include "NetXStack.h"
#include "nx_bsd.h"
#elif defined(CONFIG_LWIP_ENET)
#define LWIP_PATCH_MISRA   // for misra depress
#include "lwip/sockets.h"
#endif

#include "SvcRtcp.h"

#define MAX_RTP_SESSION 2U

typedef struct {
    void * base;    /* Starting address */
    size_t Len;     /* Number of bytes to transfer */
} RTP_IO_VEC_s;

typedef struct {
    INT32 RtpSock;
    INT32 RtcpSock;
    struct sockaddr_in RemoteRtp;
    struct sockaddr_in RemoteRtcp;
    void* ClientSession;               /* the rtsp client session this rtp sink serve */
} RTP_TRANSPORT_s;

typedef struct {
    UINT8 *Buf;
    UINT32 Len;
    UINT32 TimeStamp;
    UINT8 Maker;
} RTP_PAYLOAD_INFO_s;

typedef struct {
    SVC_RTSP_SERVER_s* Srv;
    RTP_TRANSPORT_s* Transports[MAX_RTP_SESSION];
    DOUBLE CreateTime;
    UINT32 EnDyBr;
    UINT32 NeedToSendSrSdes;
    RTCP_SR_COMPOUND_s SrCompound;
    void (*ReportCallback) (UINT32 Event, void* Info, const void* Ctx);
    void* ReportCtx;
    UINT32 TimeStampBase;
    UINT32 LastTimeStamp;
    UINT32 StartTs;          //the timestamp return in the RTSP-PLAY command
    UINT32 Ssrc;
    UINT32 OctetCount;
    UINT32 PkgCount;
    UINT32 ClockRate;
    UINT32 PayloadType;
    UINT16 SeqNo;
} RTP_SINK_s;

#ifndef BIG_ENDIAN
#define BIG_ENDIAN
#endif

typedef struct {
#ifdef BIG_ENDIAN               /* byte 0 */
    UINT32 csrc_len:4;   /* expect 0 */
    UINT32 extension:1;  /* expect 1, see RTP_OP below */
    UINT32 padding:1;    /* expect 0 */
    UINT32 version:2;    /* expect 2 */
#else
    UINT32 version:2;
    UINT32 padding:1;
    UINT32 extension:1;
    UINT32 csrc_len:4;
#endif

#ifdef BIG_ENDIAN               /* byte 1 */
    UINT32 Payload:7;    /* RTP_PAYLOAD_RTSP */
    UINT32 marker:1;     /* expect 1 */
#else
    UINT32 marker:1;
    UINT32 Payload:7;
#endif
    UINT16 SeqNo;       /* bytes 2, 3 */
    UINT32 TimeStamp;   /* bytes 4-7 */
    UINT32 Ssrc;    /* bytes 8-11 stream number is used here. */
}rtpPackHeader;

UINT32 SvcRtpSink_Create(SVC_RTSP_SERVER_s* pSrv, RTP_SINK_s *thiz,
                UINT32 PayloadType, UINT32 ClockRate, UINT32 EnableBRC, void const * pCallback, void* pCtx);
UINT32 SvcRtpSink_Release(RTP_SINK_s* thiz);
UINT32 SvcRtpSink_IsCreated(RTP_SINK_s const * thiz);
UINT32 SvcRtpSink_GetCurTimestamp(RTP_SINK_s *thiz, UINT32 RefTimestamp);
UINT32 SvcRtpSink_RemoveTransport(RTP_SINK_s* thiz, const RTP_TRANSPORT_s *Transport);
UINT32 SvcRtpSink_AddTransport(RTP_SINK_s* thiz, RTP_TRANSPORT_s* Transport);
UINT32 SvcRtpSink_SendPacket(RTP_SINK_s *thiz, RTP_PAYLOAD_INFO_s const * pInfo);
UINT32 SvcRtpSink_SendPacketVector(RTP_SINK_s *thiz, RTP_PAYLOAD_INFO_s const * pInfo,
        RTP_IO_VEC_s const * pVec, UINT32 VecNum);

INT32 SvcRtcp_SetupSrSdes(const RTP_SINK_s* thiz, RTCP_SR_COMPOUND_s* pkt);
UINT32 SvcRtcp_SendSrSdes(RTP_SINK_s* thiz);
UINT32 SvcRtcp_SendGoodBye(RTP_SINK_s* thiz);
UINT32 SvcRtcp_SendPacket(RTP_SINK_s* thiz, UINT32 type, const RTP_TRANSPORT_s* Transport);
void SvcRtcp_RtcpHandler(INT32 Sd, const void* Ctx);

#endif /*SVC_RTP_SINK_H*/
