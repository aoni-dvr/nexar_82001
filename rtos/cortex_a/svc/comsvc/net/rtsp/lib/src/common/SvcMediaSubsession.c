/**
 *  @file SvcMediaSubsession.c
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
 *  @details media subsession module
 *
 */

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#if defined(CONFIG_NETX_ENET)
#include "NetStack.h"
#include "NetXStack.h"
#include "nx_api.h"
#include "nx_bsd.h"
#else
#endif
#include "SvcRtsp.h"
#include "SvcRtspServer.h"
#include "SvcMediaSubsession.h"
#include "SvcStreamMgr.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaPlayer.h"

INT32 make_socket_nonblocking(INT32 sock) {
#if defined(CONFIG_NETX_ENET)
  return fcntl(sock, F_SETFL, 0x4000);
#else
  return fcntl(sock, F_SETFL, 1);
#endif
}

static INT32 SetupDatagramSocket(UINT32 addr, UINT16 Port, UINT32 nonblocking)
{
    struct sockaddr_in servaddr;
    INT32 sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    const struct sockaddr_in* pServaddr;
#if defined(CONFIG_NETX_ENET)
    struct sockaddr* pSockaddr;
#else
    const struct sockaddr* pSockaddr;
#endif

    (VOID)addr;
    (VOID)Port;
    if (sock < 0) {
        AmbaPrint_PrintUInt5("SetupDatagramSocket: unable to create data gram socket", \
                        0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("SetupDatagramSocket src 0x%x", (UINT32)addr, 0U, 0U, 0U, 0U);

        if (AmbaWrap_memset(&servaddr, 0, sizeof(servaddr))!= 0U) { }
        servaddr.sin_family      = AF_INET;
        #if defined(CONFIG_NETX_ENET)
        servaddr.sin_addr.s_addr = htonl(addr);
        servaddr.sin_port        = htons(Port);
        #else
        servaddr.sin_addr.s_addr = htonl(addr);
        servaddr.sin_port        = htons(Port);
        #endif

        AmbaPrint_PrintUInt5("SetupDatagramSocket src 2 0x%x", (UINT32)servaddr.sin_addr.s_addr, 0U, 0U, 0U, 0U);

        pServaddr = &servaddr;
        AmbaMisra_TypeCast(&pSockaddr, &pServaddr);

        if (bind(sock, pSockaddr, (INT)sizeof servaddr) != 0) {
        #if defined(CONFIG_NETX_ENET)
            (void)soc_close(sock);
        #else
            (void)close(sock);
        #endif
            sock = -1;
        } else {
            if (1U == nonblocking) {
                if(make_socket_nonblocking(sock) < 0) {
                    AmbaPrint_PrintUInt5("SetupDatagramSocket: failed to make non-blocking", \
                                    0U, 0U, 0U, 0U, 0U);
            #if defined(CONFIG_NETX_ENET)
                    (void)soc_close(sock);
            #else
                    (void)close(sock);
            #endif
                    sock = -1;
                }
            }
        }
    }
    return sock;
}

static INT32 FreeSdpLines(const void *pthiz)
{
    MEDIA_SUBSESSION_s* thiz;
    UINT32 RetVal = OK;

    AmbaMisra_TypeCast(&thiz, &pthiz);

    if (thiz == NULL) {
        RetVal = ERR_NA;
    } else {
        SvcRtspServer_MemPool_Free(SVC_RTSP_POOL_SDP_LINE_BUF, thiz->AuxSdpLine);
        SvcRtspServer_MemPool_Free(SVC_RTSP_POOL_SDP_LINE_BUF, thiz->SdpLines);
        thiz->AuxSdpLine = NULL;
        thiz->SdpLines = NULL;
        RetVal = OK;
    }
    return (INT32)RetVal;
}


static void ReportStreamStatus(UINT32 Event, void* Info, const void* Ctx)
{
    const AMBA_STREAM_s* pCtx;

    if (Ctx == NULL) {
        AmbaPrint_PrintUInt5("ReportStreamStatus: invalid input", \
                        0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaMisra_TypeCast(&pCtx, &Ctx);
        (void)SvcRtspStrmReport( pCtx, Event, Info);

    }

}

/**
* SvcMediaSubsession_GetTrackID
* @param [in]  pthiz
* @return pTrackId
*/
const char* SvcMediaSubsession_GetTrackID(const void *pthiz)
{
    MEDIA_SUBSESSION_s *thiz;
    const char* pTrackId;
    AmbaMisra_TypeCast(&thiz, &pthiz);

    if(0U == thiz->TrackNum) {
        pTrackId = NULL;
    } else {
        if(0U == AmbaUtility_StringLength(thiz->TrackId)) {
            {
                UINT32 RetVal;

                if (AmbaWrap_memset(thiz->TrackId, 0, 8)!= 0U) { }

                // RetVal = AmbaAdvSnPrint(thiz->TrackId, 8, "track%d", thiz->TrackNum);
                {
                    char    *StrBuf = thiz->TrackId;
                    UINT32  BufSize = 8U;
                    UINT32  CurStrLen;

                    AmbaUtility_StringAppend(StrBuf, BufSize, "track");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(thiz->TrackNum), 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, "");

                    RetVal = AmbaUtility_StringLength(StrBuf);
                }

                AmbaPrint_PrintStr5("%s, TrackId %s", __func__, thiz->TrackId, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("Len %d", RetVal, 0U, 0U, 0U, 0U);
            }
        }
        pTrackId = thiz->TrackId;
    }

    return pTrackId;
}

INT32 SvcMediaSubsession_SetupUDPTransport(const void *pSub, UINT32 sessionid,
                                    struct in_addr clnt_addr,
                                    UINT32 clntRTPPortNum,
                                    UINT32 clntRTCPPortNum,
                                    UINT32* servRTPPortNum,
                                    UINT32* servRTCPPortNum,
                                    void* clientSession)
{
    UINT32 i, ServerRtpPort, RtpStartPort = 6000U;
    INT32 RtpSock = -1;
    INT32 RtcpSock = -1;
    INT32 rval = 0;
    UINT32 enable_dyBR;
    RTP_TRANSPORT_s Transport;
    MEDIA_SUBSESSION_s* sub;
#if defined(CONFIG_NETX_ENET)
    struct sockaddr* pSockaddr;
#else
    const struct sockaddr* pSockaddr;
#endif
    const void* pVoidSockaddr;
    const void* pVoidCallback;
    void (*pCallBack) (UINT32 Event, void* Info, const void* Ctx);

    AmbaMisra_TypeCast(&sub, &pSub);

    (VOID)clntRTPPortNum;
    (VOID)clntRTCPPortNum;

    if ((sub == NULL) || (servRTPPortNum == NULL) || (servRTCPPortNum == NULL)) {
        AmbaPrint_PrintUInt5("SvcMediaSubsession_SetupUDPTransport: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        rval = -1;
    } else {

        if((1U == sub->IsLive) && ((sub->CodecID>=AMBA_FORMAT_MID_H264) && (sub->CodecID<AMBA_FORMAT_MID_HVC))) {
            enable_dyBR = 1U;
        } else {
            enable_dyBR = 0U;
        }

        // if rtp sink is not created, create one
        if (OK != SvcRtpSink_IsCreated(&sub->Rtp)) {
            SVC_RTSP_SERVER_s* pSrv;
            AmbaMisra_TypeCast(&pSrv, &sub->Srv);

            pCallBack = ReportStreamStatus;
            AmbaMisra_TypeCast(&pVoidCallback, &pCallBack);
            rval = (INT32)SvcRtpSink_Create(pSrv, &sub->Rtp, (96U + sub->TrackNum - 1U), sub->RefClock, enable_dyBR,
                        pVoidCallback, sub->Streamer);
            if (rval < 0) {
                AmbaPrint_PrintUInt5("SvcMediaSubsession_SetupUDPTransport: create Rtp sink fail", \
                                0U, 0U, 0U, 0U, 0U);
                rval = -1;
            }
        }
        if (rval >= 0) {
            // create datagram port for rtp/rtcp
            for (ServerRtpPort = RtpStartPort; ServerRtpPort <= (RtpStartPort + 1000U); ServerRtpPort += 2U) {
                RtpSock = SetupDatagramSocket((UINT32)INADDR_ANY, (UINT16)ServerRtpPort, 1U);
                if (0 <= RtpSock) {
                    RtcpSock = SetupDatagramSocket((UINT32)INADDR_ANY, (UINT16)(ServerRtpPort + 1U), 1U);
                    if (0 <= RtcpSock) {
                        break;
                    } else {
                        #if defined(CONFIG_NETX_ENET)
                            (void)soc_close(RtpSock);
                        #else
                            (void)close(RtpSock);
                        #endif
                        RtpSock = -1;
                    }
                }
            }

            if (RtpSock < 0) {
                AmbaPrint_PrintUInt5("SvcMediaSubsession_SetupUDPTransport: Cannot find valid Port for Rtp socket!", \
                                0U, 0U, 0U, 0U, 0U);
                (void)SvcRtpSink_Release(&sub->Rtp);
                rval = -1;
            }

            if (rval >= 0) {
                // setup transport
                if (AmbaWrap_memset(&Transport, 0, sizeof(RTP_TRANSPORT_s))!= 0U) { }
                Transport.RtpSock = RtpSock;
                Transport.RtcpSock = RtcpSock;
                Transport.RemoteRtp.sin_family = AF_INET;
                Transport.RemoteRtp.sin_addr   = clnt_addr;
                #if defined(CONFIG_NETX_ENET)
                    Transport.RemoteRtp.sin_port   = (UINT16)htons(clntRTPPortNum);
                #else
                    Transport.RemoteRtp.sin_port   = htons((UINT16)clntRTPPortNum);
                #endif

                Transport.RemoteRtcp.sin_family = AF_INET;
                Transport.RemoteRtcp.sin_addr   = clnt_addr;

                #if defined(CONFIG_NETX_ENET)
                    Transport.RemoteRtcp.sin_port   = (UINT16)htons(clntRTCPPortNum);
                #else
                    Transport.RemoteRtcp.sin_port   = htons((UINT16)clntRTCPPortNum);
                #endif

                Transport.ClientSession = clientSession;

                pVoidSockaddr = &Transport.RemoteRtcp;
                AmbaMisra_TypeCast(&pSockaddr, &pVoidSockaddr);
                (void)pSockaddr->sa_data;
                (void)connect(Transport.RtcpSock, pSockaddr, (INT)sizeof(Transport.RemoteRtcp));

                // add transport
                for(i = 0; i < sub->MaxTransport; i++) {
                    if(0U == sub->Map[i].Session) {
                        sub->Map[i].Session = sessionid;
                        sub->Map[i].Transport = Transport;
                        sub->Used++;
                        break;
                    }
                }
                if (i == sub->MaxTransport) {
                    AmbaPrint_PrintUInt5("[RTSP Server] Too many Transport, not allowed", \
                                    0U, 0U, 0U, 0U, 0U);
                #if defined(CONFIG_NETX_ENET)
                        (void)soc_close(RtpSock);
                        (void)soc_close(RtcpSock);
                #else
                        (void)close(RtpSock);
                        (void)close(RtcpSock);
                #endif
                        rval = -1;
                } else {

                    *servRTPPortNum = ServerRtpPort;
                    *servRTCPPortNum = (ServerRtpPort + 1U);
                }
            }
        }
    }

    return rval;
}

/**
* start stream of specific media subsession.
* @param [in]  MEDIA_SUBSESSION_s object pointer.
* @param [in]  sessionid of client session.
* @param [in]  PLAY / SEEK / RESUME.
* @param [out]  to store the rtp seqNo.
* @param [out]  to store the rtsp current timestamp.
* @return OK / number of remained connection.
*/
UINT32 SvcMediaSubsession_StartStream(const void *pSub, UINT32 sessionid, UINT32 op,
                                    UINT16 *rtpseqnum, UINT32 *RtpTimestamp)
{
    UINT32 i;
    RTP_TRANSPORT_s* Transport = NULL;
    MEDIA_SUBSESSION_s *sub;
    UINT32 RetVal = OK;
    const AMBA_STREAM_s* pStreamer;

    AmbaMisra_TypeCast(&sub, &pSub);

    if ((sub == NULL) || (rtpseqnum == NULL) || (RtpTimestamp == NULL)) {
        AmbaPrint_PrintUInt5("SvcMediaSubsession_StartStream: invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        // find transport
        for (i = 0; i < MAX_RTP_SESSION; i++) {
            if((sub->Map[i].Session != 0U) && (sub->Map[i].Session == sessionid)) {
                Transport = &sub->Map[i].Transport;
                break;
            }
        }
        if (Transport == NULL) {
            AmbaPrint_PrintUInt5("SvcMediaSubsession_StartStream: cannot find correspond Transport", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
        } else {


            if (op == SVC_RTSP_SESSION_SEEK) {
                AmbaMisra_TypeCast(&pStreamer, &sub->Streamer);
                SvcRtspStrmResetFifo(pStreamer);
            } else if (op == SVC_RTSP_SESSION_RESUME) {
                ; //nothing
            } else if (op == SVC_RTSP_SESSION_PLAY) {
                (void)SvcRtpSink_AddTransport(&sub->Rtp, Transport);
            } else {
                ; //nothing
            }



            // fill up rtp_seq, rtp_ts
            *rtpseqnum = sub->Rtp.SeqNo;
            *RtpTimestamp = SvcRtpSink_GetCurTimestamp(&sub->Rtp, 0U);

            // send first SR as fast as we can
            (void)SvcRtcp_SendSrSdes(&sub->Rtp);
            AmbaMisra_TypeCast(&pStreamer, &sub->Streamer);
            SvcRtspStrmEnable(pStreamer);
            sub->TotalReadFrames = 0U;
        }
    }
    return RetVal;

}

/**
* pause stream of specific media subsession.
* @param [in]  MEDIA_SUBSESSION_s object pointer.
* @param [in]  sessionid of client session.
* @return OK/ERR_NA.
*/
UINT32 SvcMediaSubsession_PauseStream(const void *pSub, UINT32 sessionid)
{
    UINT32 i;
    MEDIA_SUBSESSION_s *sub;
    UINT32 RetVal = OK;
    const AMBA_STREAM_s* pStreamer;

    AmbaMisra_TypeCast(&sub, &pSub);

    if (sub == NULL) {
        AmbaPrint_PrintUInt5("SvcMediaSubsession_PauseStream: invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        for (i = 0U; i < MAX_RTP_SESSION; i++) {
            if ((sub->Map[i].Session != 0U) && (sub->Map[i].Session == sessionid)) {
                if (1U == sub->IsLive) {
                    (void)SvcRtpSink_RemoveTransport(&sub->Rtp, &sub->Map[i].Transport);
                } else {
                    AmbaMisra_TypeCast(&pStreamer, &sub->Streamer);
                    SvcRtspStrmDisable(pStreamer);
                }
                break;
            }

        }
    }
    return RetVal;
}

/**
* teardown stream of specific media subsession.
* @param [in]  MEDIA_SUBSESSION_s object pointer.
* @param [in]  session_id of client session.
* @return active client number.
*/
UINT32 SvcMediaSubsession_TearDown(const void *pSub, UINT32 sessionid)
{
    UINT32 i;
    MEDIA_SUBSESSION_s *sub;
    UINT32 RetVal = OK;
    const AMBA_STREAM_s* pStreamer;

    AmbaMisra_TypeCast(&sub, &pSub);

    if (sub == NULL) {
        AmbaPrint_PrintUInt5("SvcMediaSubsession_TearDown: invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = OK;  /* no session pointed, assume it success. */
    } else {

        UINT32 Tmp;
        for (i = 0U; i < MAX_RTP_SESSION; i++) {
            if ((sub->Map[i].Session != 0U) && (sub->Map[i].Session == sessionid)) {
                (void)SvcRtpSink_RemoveTransport(&sub->Rtp, &sub->Map[i].Transport);
                {
                    #if defined(CONFIG_NETX_ENET)
                        (void)soc_close(sub->Map[i].Transport.RtcpSock);
                        (void)soc_close(sub->Map[i].Transport.RtpSock);
                    #else
                        (void)close(sub->Map[i].Transport.RtcpSock);
                        (void)close(sub->Map[i].Transport.RtpSock);
                    #endif
                }

                if (AmbaWrap_memset(&sub->Map[i], 0, sizeof(sub->Map[i]))!= 0U) { }

                if(sub->Used > 0U){
                    --sub->Used;
                }
                if (0U == sub->Used) {                //live session allow multiple client
                    AmbaMisra_TypeCast(&pStreamer, &sub->Streamer);
                    SvcRtspStrmDisable(pStreamer);//only disable streamer when there is no more client
                    sub->Used = 0U;
                }
                break;

            }
        }
        Tmp = SvcRtpSink_IsCreated(&sub->Rtp);
        if ((0U == sub->Used) && (OK == Tmp)) {
            (void)SvcRtpSink_Release(&sub->Rtp);
        }
        RetVal = sub->Used;
    }
    return RetVal;
}

/**
* create media subsession object.
* @param [in]  SVC_RTSP_SERVER_s object pointer.
* @param [in]  SVC_STREAM_LIVE_MEDIA_s object pointer if live = 1
*              SVC_RTSP_MEDIA_TRACK_CFG_s object pointer if live = 0
* @param [in]  indicate whether subsession is live.
* @return MEDIA_SUBSESSION_s object pointer new created, NULL when failed.
*/
MEDIA_SUBSESSION_s* SvcMediaSubsession_Create(const void *pSrv, const void* Info, UINT32 IsLive)
{
    MEDIA_SUBSESSION_s* sub = NULL;
    const SVC_RTSP_MEDIA_TRACK_CFG_s* trk = NULL;
    const SVC_STREAM_LIVE_MEDIA_s* liveMedia = NULL;
    UINT32 MediaId = 0U;
    SVC_RTSP_SERVER_s *Srv;

    AmbaMisra_TypeCast(&Srv, &pSrv);

    if ((Srv == NULL) || (NULL == Info)) {
        AmbaPrint_PrintUInt5("SvcMediaSubsession_Create: Invalid parameter.", \
                        0U, 0U, 0U, 0U, 0U);
        sub = NULL;
    } else {

        UINT32 Rval = OK;
        if (0U == IsLive) {
            AmbaMisra_TypeCast(&trk, &Info);
            MediaId = trk->nMediaId;
        } else if (1U == IsLive) {
            AmbaMisra_TypeCast(&liveMedia, &Info);
            MediaId = liveMedia->MediaId;
        } else {
            Rval = ERR_NA;
        }
        if(OK == Rval) {
            switch (MediaId){
            case 0U:
                break;
            case AMBA_FORMAT_MID_H264:
            case AMBA_FORMAT_MID_AVC:
                sub = SvcH264Subsession_Create(Srv, Info, IsLive);
                break;
            case AMBA_FORMAT_MID_HVC:
            case AMBA_FORMAT_MID_H265:
            #if defined(CONFIG_AMBA_RTSP_HEVC)
                sub = SvcH265Subsession_Create(Srv, Info, IsLive);
            #endif
                break;
            case AMBA_FORMAT_MID_AAC:
                sub = SvcAacSubsession_Create(Srv, Info, IsLive);
                break;
            case AMBA_FORMAT_MID_PCM:
                sub = SvcPcmSubsession_Create(Srv, Info, IsLive);
                break;
            default:
                AmbaPrint_PrintUInt5("unsupported media(%x)", \
                                MediaId, 0U, 0U, 0U, 0U);
                break;
            }

            if (sub != NULL) {
                sub->Srv = Srv;
                sub->TrackId[0] = '\0';
                sub->SdpLines = NULL;
                sub->AuxSdpLine = NULL;
                sub->FreeSdpLines = FreeSdpLines;
            }
        }
    }
    return sub;
}

/**
* release media subsession object.
* @param [in]  point to SVC_RTSP_SERVER_s object to be released.
*/
void SvcMediaSubsession_Release(const void *pSub)
{
    MEDIA_SUBSESSION_s *sub;
    AMBA_STREAM_s* pStreamer;

    AmbaMisra_TypeCast(&sub, &pSub);

    if (sub == NULL) {
        AmbaPrint_PrintUInt5("SvcMediaSubsession_Release: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
    } else {
        (void)sub->FreeSdpLines(sub);
        sub->CodecID = 0U;
        if (0U == sub->IsLive) {
            AmbaMisra_TypeCast(&pStreamer, &sub->Streamer);
            (void)SvcRtspStrmDelete(pStreamer);
        }
    }

}
