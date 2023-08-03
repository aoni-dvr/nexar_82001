/**
 *  @file SvcRtpSink.c
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
 *  @details rtp sink module
 *
 */

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#if defined(CONFIG_NETX_ENET)
#include "NetStack.h"
#include "NetXStack.h"
#include "nx_bsd.h"
#elif defined(CONFIG_LWIP_ENET)
#define LWIP_PATCH_MISRA   // for misra depress
#include "lwip/sockets.h"
#include "lwip/errno.h"
#endif

#include "SvcMediaSubsession.h"
#include "SvcRtspRandom.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"


/**
* get current timestamp for specific rtpsink.
* @param [in]  RTP_SINK_s object pointer.
* @param [in]  always be 0 now.
* @return current timestamp for thiz.
*/
UINT32 SvcRtpSink_GetCurTimestamp(RTP_SINK_s *thiz, UINT32 RefTimestamp)
{
    DOUBLE Now;
    UINT64 TmpLL;
    DOUBLE TmpDL;
    UINT32 TimeStamp, SysTickCnt;
    UINT32 RetTimestamp;

    if (thiz == NULL) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetTimestamp = 0U;
    } else {
        (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
        Now = (DOUBLE) SysTickCnt / (DOUBLE)1000;
        if((INT32)thiz->CreateTime == -1) { //not init yet
            if(RefTimestamp != 0U){
                TimeStamp = RefTimestamp;
                AmbaPrint_PrintUInt5("SvcRtpSink_GetCurTimestamp: return time stamp as ref_ts:%u", \
                                TimeStamp, 0U, 0U, 0U, 0U);
            } else {
                thiz->CreateTime = Now;
                TmpDL = Now * (DOUBLE)thiz->ClockRate;
                TmpLL = (UINT64)TmpDL;
                TimeStamp = (UINT32)(TmpLL);
                thiz->TimeStampBase = TimeStamp;

                {
                    UINT32 RetVal;
                    char dst_str[128U] = {'\0'};

                    if (AmbaWrap_memset(dst_str, 0, 128U)!= 0U) { }

                    // RetVal = AmbaAdvSnPrint(dst_str, 128U, "SvcRtpSink_GetCurTimestamp: init TimeStamp:%u, cur:%u, now:%lf, ClockRate:%u", thiz->LastTimeStamp, TimeStamp,
                    //                         Now, thiz->ClockRate);
                    {
                        char    *StrBuf = dst_str;
                        UINT32  BufSize = 128U;
                        UINT32  CurStrLen;

                        AmbaUtility_StringAppend(StrBuf, BufSize, "SvcRtpSink_GetCurTimestamp: init TimeStamp:");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(thiz->LastTimeStamp), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, ", cur:");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(TimeStamp), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, ", now:");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_DoubleToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (DOUBLE)(Now), 6U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, ", ClockRate:");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(thiz->ClockRate), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, "");

                        RetVal = AmbaUtility_StringLength(StrBuf);
                    }

                    AmbaPrint_PrintStr5("%s, %s", __func__, dst_str, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("Len %d", RetVal, 0U, 0U, 0U, 0U);
                }
                AmbaPrint_PrintUInt5("PkgCount %d, ClockRate %d, PayloadType %d", thiz->PkgCount, thiz->ClockRate, thiz->PayloadType, 0U, 0U);
            }
        } else {
            TmpDL = ((Now - thiz->CreateTime) * (DOUBLE)thiz->ClockRate) + (DOUBLE)thiz->TimeStampBase;
            TmpLL = (UINT64)TmpDL;
            TimeStamp = (UINT32)TmpLL;
        }
        RetTimestamp = TimeStamp;
    }
    return RetTimestamp;
}

/**
* to check whether RtpSink is created.
* @param [in]  RTP_SINK_s object pointer.
* @return OK = created, NG = not yet.
*/
UINT32 SvcRtpSink_IsCreated(RTP_SINK_s const *thiz)
{
    UINT32 RetVal;

    if (thiz == NULL){
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else if ((0U == thiz->ClockRate) && (0U == thiz->PayloadType)) {
        RetVal = ERR_NA;
    } else {
        RetVal = OK;
    }

    return RetVal;
}

/**
* remove transport from specific RTP_SINK_s object.
* send bye rtcp packet, and unregister RTCP handler before remove transport.
* @param [in]  RTP_SINK_s object pointer.
* @param [in]  transport to be removed.
* @return OK/ERR_NA.
*/
UINT32 SvcRtpSink_RemoveTransport(RTP_SINK_s *thiz, const RTP_TRANSPORT_s *Transport)
{
    UINT32 i;
    UINT32 RetVal = ERR_NA;

    if((NULL == thiz) || (NULL == Transport)) {
        AmbaPrint_PrintStr5("invalid input", NULL, NULL, NULL, NULL, NULL);
    } else {
        for(i = 0U; i < MAX_RTP_SESSION; i++) {
            if(thiz->Transports[i] == Transport) {
                if(OK != SvcRtcp_SendPacket(thiz, BYE, Transport)) {
                    AmbaPrint_PrintUInt5("[SvcRtpSink]<RemoveTransport>SvcRtcp_SendPacket failed",
                        0U, 0U, 0U, 0U, 0U);
                }
                if (1U == thiz->EnDyBr) {
                    if(OK != SvcRtspServer_UnregWatcher(thiz->Srv, Transport->RtcpSock)) {
                        AmbaPrint_PrintUInt5("[SvcRtpSink]<RemoveTransport>AmbaRTSPServer_UnregWatcher failed",
                            0U, 0U, 0U, 0U, 0U);
                    }
                }
                thiz->Transports[i] = NULL;
                RetVal = OK;
                break;
            }
        }
    }
    return RetVal;
}

/**
* add transport to specific RTP_SINK_s object.
* and register RTCP handler for rtcp socket.
* @param [in]  RTP_SINK_s object pointer.
* @param [in]  transport to be added.
* @return OK/ERR_NA.
*/
UINT32 SvcRtpSink_AddTransport(RTP_SINK_s *thiz, RTP_TRANSPORT_s* Transport)
{
    UINT32 i;
    UINT32 RetVal = ERR_NA;

    if((NULL == thiz) || (NULL == Transport)) {
        AmbaPrint_PrintStr5("invalid input", NULL, NULL, NULL, NULL, NULL);
    } else {

        for(i = 0U; i < MAX_RTP_SESSION; i++) {
            if(thiz->Transports[i] == NULL) {
                thiz->Transports[i] = Transport;
                if (1U == thiz->EnDyBr) {
                    if(OK != SvcRtspServer_RegWatcher(thiz->Srv, Transport->RtcpSock, SvcRtcp_RtcpHandler, thiz)) {
                        AmbaPrint_PrintUInt5("[SvcRtpSink]<AddTransport>AmbaRTSPServer_RegWatcher failed",
                            0U, 0U, 0U, 0U, 0U);
                    }
                }
                RetVal = OK;
                break;
            }
        }
    }
    return RetVal;
}

/**
* create rtpSink object.
* @param [in]  SVC_RTSP_SERVER_s object pointer..
* @param [in]  transport to be added.
* @param [in]  payload type.96 for video, 97 for audio.
* @param [in]  clock rate of timestamp.
* @param [in]  enable BRC according to network situation.
* @param [in]
* @param [out] RTP_SINK_s object pointer to be created.
* @return OK/ERR_NA.
*/
UINT32 SvcRtpSink_Create(SVC_RTSP_SERVER_s* pSrv, RTP_SINK_s *thiz,
                UINT32 PayloadType, UINT32 ClockRate, UINT32 EnableBRC, void const * pCallback, void* pCtx)
{
    UINT32 RetVal;

    if ((NULL == pSrv) || (NULL == thiz)) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        thiz->Srv = pSrv;
        thiz->SeqNo = (UINT16) our_random();
        thiz->Ssrc = our_random32();
        thiz->ClockRate = ClockRate;
        thiz->CreateTime = (DOUBLE)-1;
        thiz->PayloadType = PayloadType;
        thiz->EnDyBr = EnableBRC;
        AmbaMisra_TypeCast(&(thiz->ReportCallback), &pCallback);
        thiz->ReportCtx = pCtx;
        AmbaPrint_PrintUInt5("SvcRtpSink_Create ClockRate %d, thiz->ClockRate %d", ClockRate, thiz->ClockRate, 0U, 0U, 0U);

        if(OK != (UINT32)SvcRtcp_SetupSrSdes(thiz, &thiz->SrCompound)) {
            AmbaPrint_PrintStr5("[SvcRtpSink]<Create>: SvcRtcp_SetupSrSdes Failed", NULL, NULL, NULL, NULL, NULL);
        }

        RetVal = OK;
    }

    return RetVal;
}

/**
* release rtpSink object.
* @param [in]  RTP_SINK_s object pointer.
* @return OK/ERR_NA.
*/
UINT32 SvcRtpSink_Release(RTP_SINK_s *thiz)
{
    UINT32 RetVal;

    if (NULL == thiz) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        thiz->ClockRate = 0;
        thiz->PayloadType = 0;
        if ((1U == thiz->EnDyBr) && (NULL != thiz->ReportCallback)) {
            thiz->ReportCallback(SVC_RTSP_RTCP_RESET, NULL, thiz->ReportCtx);
        }
        RetVal = OK;
    }
    return RetVal;
}

/**
* send udp/rtp packet.
* @param [in]  RTP_SINK_s object pointer.
* @param [in]  payload info.
* @return OK/ERR_NA.
*/
UINT32 SvcRtpSink_SendPacket(RTP_SINK_s *thiz, RTP_PAYLOAD_INFO_s const * pInfo)
{
    UINT32 i;
    UINT32 UintRet;
    INT32 IntRet;
    RTP_TRANSPORT_s const * Transport;
    rtpPackHeader Header;
    char PkgBuf[MTU_SIZE];
    UINT32 PkgSize;
    UINT32 RetVal;
#if defined(CONFIG_NETX_ENET)
    struct sockaddr *pRemoteRtp;
#else
    const struct sockaddr *pRemoteRtp;
#endif
    struct sockaddr_in const *pSockAddr;
    INT32 ErrNum;

    if((NULL == thiz) || (NULL == pInfo)) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        /* set last_timestamp first for the sr_sdes */
        thiz->LastTimeStamp = pInfo->TimeStamp;

        if(0U != thiz->NeedToSendSrSdes){
            thiz->NeedToSendSrSdes = 0U;
            UintRet = SvcRtcp_SendSrSdes(thiz);
            if(OK != UintRet){
                AmbaPrint_PrintStr5("Fail to do send_sr_sdes", NULL, NULL, NULL, NULL, NULL);
            }
        }

        Header.version = 2U;
        Header.padding = 0U;
        Header.extension = 0U;
        Header.csrc_len = 0U;
        i = pInfo->Maker;
        i &= (UINT32)0x01U;
        Header.marker = (UINT8)i;
        i = thiz->PayloadType;
        i &= (UINT32)0x7FU;
        Header.Payload = (UINT8)i;
        Header.SeqNo = SvcRtspServer_ihtons(thiz->SeqNo);
        thiz->SeqNo++;
        Header.TimeStamp = SvcRtspServer_ihtonl(pInfo->TimeStamp);
        Header.Ssrc = SvcRtspServer_ihtonl(thiz->Ssrc);

        if (AmbaWrap_memcpy(PkgBuf, &Header, sizeof(rtpPackHeader))!= 0U) { }
        if (AmbaWrap_memcpy(&PkgBuf[sizeof(rtpPackHeader)], pInfo->Buf, pInfo->Len)!= 0U) { }
        PkgSize = sizeof(rtpPackHeader) + pInfo->Len;

        for(i = 0U; i < MAX_RTP_SESSION; i++) {
            if(thiz->Transports[i] != NULL) {
                Transport = thiz->Transports[i];

                pSockAddr = &Transport->RemoteRtp;
                AmbaMisra_TypeCast(&pRemoteRtp, &pSockAddr);
                (VOID)pRemoteRtp->sa_family;
            #if defined(CONFIG_NETX_ENET)
                IntRet = sendto(Transport->RtpSock, PkgBuf, (INT32)PkgSize, 0,
                    pRemoteRtp, (INT32)sizeof(Transport->RemoteRtp));
            #else
                IntRet = (INT32)sendto(Transport->RtpSock, PkgBuf, PkgSize, 0,
                    pRemoteRtp, (INT32)sizeof(Transport->RemoteRtp));
            #endif
                if ((UINT32)IntRet != PkgSize) {
                #if defined(CONFIG_NETX_ENET)
                    ErrNum = _nx_get_errno();
                #else
                    ErrNum = errno;
                #endif
                    #if defined(CONFIG_NETX_ENET)
                    if ((ErrNum == EAGAIN) || (ErrNum == ENOBUFS)){
                        (void)AmbaKAL_TaskSleep(10U);

                        IntRet = sendto(Transport->RtpSock, PkgBuf, (INT32)PkgSize, 0,
                            pRemoteRtp, (INT32)sizeof(Transport->RemoteRtp));
                    #else
                    if ((ErrNum == EAGAIN) || (ErrNum == EWOULDBLOCK) || (ErrNum == ENOBUFS)){
                        (void)AmbaKAL_TaskSleep(10U);

                        IntRet = (INT32)sendto(Transport->RtpSock, PkgBuf, PkgSize, 0,
                            pRemoteRtp, (INT32)sizeof(Transport->RemoteRtp));
                    #endif
                        if ((UINT32)IntRet != PkgSize) {
                            AmbaPrint_PrintInt5("[Sd=%d]error re-send packet ...", Transport->RtpSock, 0, 0, 0, 0);
                        }
                    } else {
                        AmbaPrint_PrintInt5("[Sd=%d]error send packet: %d", Transport->RtpSock, ErrNum, 0, 0, 0);
                    }
                } else if ((UINT32)IntRet < PkgSize) {
                    AmbaPrint_PrintUInt5("truncted, only send %u bytes", (UINT32)IntRet, 0U, 0U, 0U, 0U);
                } else {
                    // Do nothing here.
                }
            }
        }
        thiz->PkgCount++;
        thiz->OctetCount += pInfo->Len;

        RetVal = OK;
    }

    return RetVal;
}


/**
* send udp/rtp packet in iov(filled by subsession).
* @param [in]  RTP_SINK_s object pointer.
* @param [in]  payload info such as ts, len, marker
* @param [in]  iovec contains the NALs vector
* @param [in]  indicate how many nals are in iovec
* @return OK/ERR_NA.
*/
UINT32 SvcRtpSink_SendPacketVector(RTP_SINK_s *thiz, RTP_PAYLOAD_INFO_s const * pInfo,
        RTP_IO_VEC_s const * pVec, UINT32 VecNum)
{
    UINT32 i;
    UINT32 RetVal;
    RTP_TRANSPORT_s const * pTransport;
    rtpPackHeader Header;
    char PkgBuf[MTU_SIZE];
    UINT32 PkgSize;
    UINT32 Offset;
    INT32 IntRet;
    UINT32 UintRet;
    INT32 ErrNum;
    const struct sockaddr_in  *pSockAddrIn;
#if defined(CONFIG_NETX_ENET)
    struct sockaddr *pRemoteRtp;
#else
    const struct sockaddr *pRemoteRtp;
#endif

    if((NULL == thiz) || (NULL == pInfo) || (NULL == pVec)) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        /* set last_timestamp first for the sr_sdes */
        thiz->LastTimeStamp = pInfo->TimeStamp;

        if(0U != thiz->NeedToSendSrSdes){
            thiz->NeedToSendSrSdes = 0U;
            UintRet = SvcRtcp_SendSrSdes(thiz);
            if(OK != UintRet){
                AmbaPrint_PrintStr5("Fail to do send_sr_sdes", NULL, NULL, NULL, NULL, NULL);
            }
        }

        Header.version = 2U;
        Header.padding = 0U;
        Header.extension = 0U;
        Header.csrc_len = 0U;
        i = pInfo->Maker;
        i &= (UINT32)0x01U;
        Header.marker = (UINT8)i;
        i = thiz->PayloadType;
        i &= (UINT32)0x7fU;
        Header.Payload = (UINT8)i;
        Header.SeqNo = SvcRtspServer_ihtons(thiz->SeqNo);
        thiz->SeqNo++;
        Header.TimeStamp = SvcRtspServer_ihtonl(pInfo->TimeStamp);
        Header.Ssrc = SvcRtspServer_ihtonl(thiz->Ssrc);

        if (AmbaWrap_memcpy(PkgBuf, &Header, sizeof(rtpPackHeader))!= 0U) { }
        Offset = sizeof(rtpPackHeader);
        for (i = 0U; i < VecNum; i++) {
            if (AmbaWrap_memcpy(&PkgBuf[Offset], pVec[i].base, pVec[i].Len)!= 0U) { }
            Offset += pVec[i].Len;
        }
        PkgSize = Offset;

        for(i = 0U; i < MAX_RTP_SESSION; i++) {
            if(thiz->Transports[i] != NULL) {
                pTransport = thiz->Transports[i];
                pSockAddrIn = &pTransport->RemoteRtp;

                AmbaMisra_TypeCast(&pRemoteRtp, &pSockAddrIn);
                (VOID)pRemoteRtp->sa_family;
            #if defined(CONFIG_NETX_ENET)
                IntRet = sendto(pTransport->RtpSock, PkgBuf, (INT32)PkgSize, 0,
                        pRemoteRtp, (INT32)sizeof(pTransport->RemoteRtp));
            #else
                IntRet = (INT32)sendto(pTransport->RtpSock, PkgBuf, PkgSize, 0,
                        pRemoteRtp, (INT32)sizeof(pTransport->RemoteRtp));
            #endif
                if((UINT32)IntRet != PkgSize) {
                #if defined(CONFIG_NETX_ENET)
                    ErrNum = _nx_get_errno();
                #else
                    ErrNum = errno;
                #endif
                    #if defined(CONFIG_NETX_ENET)
                    if ((EAGAIN == ErrNum) || (ENOBUFS == ErrNum)) {
                        (void)AmbaKAL_TaskSleep(10);

                        IntRet = sendto(pTransport->RtpSock, PkgBuf, (INT32)PkgSize, 0,
                            pRemoteRtp, (INT32)sizeof(pTransport->RemoteRtp));
                    #else
                    if ((EAGAIN == ErrNum) || (EWOULDBLOCK == ErrNum) || (ENOBUFS == ErrNum)) {
                        (void)AmbaKAL_TaskSleep(10);

                        IntRet = (INT32)sendto(pTransport->RtpSock, PkgBuf, PkgSize, 0,
                            pRemoteRtp, (INT32)sizeof(pTransport->RemoteRtp));
                    #endif
                        if ((UINT32)IntRet != PkgSize) {
                            AmbaPrint_PrintInt5("[Sd=%d]error re-send packet ...", pTransport->RtpSock, 0, 0, 0, 0);
                        }
                    }
                } else if ( (UINT32)IntRet < PkgSize) {
                    AmbaPrint_PrintInt5("truncted, only send %d bytes", 0, 0, 0, 0, 0);
                } else {
                    // Do nothing.
                }
            }
        }
        thiz->PkgCount++;
        thiz->OctetCount += pInfo->Len;

        RetVal = OK;
    }
    return RetVal;
}
