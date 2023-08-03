/**
 *  @file SvcRtcp.c
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
 *  @details rtcp module
 *
 */

#include "AmbaTypes.h"
#include <AmbaKAL.h>
#include "AmbaPrint.h"
#if defined(CONFIG_NETX_ENET)
#include "NetStack.h"
#include "NetXStack.h"
#include "nx_api.h"
#include "nx_bsd.h"
#else
#endif
#include "SvcMediaSubsession.h"
#include "SvcRtspRandom.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"

#define STANDARD_TS_BASE (0x83aa7e80U)   //1970/01/01, 00:00:00

static void RtcpSr(RTP_SINK_s *thiz, RTCP_SR_COMPOUND_s *pkt)
{
    DOUBLE now;
    UINT32 t_sec, t_nsec;
    UINT32 sr_size = sizeof(RTCP_HEADER_s) + sizeof(RTCP_HEADER_SR_s);
    UINT32 SysTickCnt = 0U;
    DOUBLE Tmp;

    pkt->SrHdr.version = 2U;

    pkt->SrHdr.padding = 0U;
    pkt->SrHdr.count = 0U;
    pkt->SrHdr.pt = SR;
    pkt->SrHdr.Length = SvcRtspServer_ihtons((UINT16)((sr_size>>2U) - 1U));

    pkt->SrPkt.Ssrc = SvcRtspServer_ihtonl(thiz->Ssrc);

    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
    now = (DOUBLE) SysTickCnt/ (DOUBLE)1000U;
    t_sec = (UINT32) now + STANDARD_TS_BASE;
    Tmp = (now - (DOUBLE)((UINT32) now))*(DOUBLE)1000000000LU;
    t_nsec = (UINT32)Tmp;
    pkt->SrPkt.NtpTimestampH = SvcRtspServer_ihtonl(t_sec);
    pkt->SrPkt.NtpTimestampL = SvcRtspServer_ihtonl((UINT32)((((UINT64) t_nsec) << 32U) / 1000000000u));
    if(-1 == (INT32)thiz->CreateTime){
        thiz->CreateTime = now;
        Tmp = (now*(DOUBLE)thiz->ClockRate);
        thiz->LastTimeStamp =  (UINT32)Tmp;
        thiz->TimeStampBase = thiz->LastTimeStamp;

        Tmp = (now*(DOUBLE)thiz->ClockRate);
        AmbaPrint_PrintUInt5("RtcpSr: init TimeStamp base: %u, cur: %u", \
                        thiz->LastTimeStamp, (UINT32)Tmp, 0U, 0U, 0U);
        pkt->SrPkt.RtpTimestamp = SvcRtspServer_ihtonl(thiz->LastTimeStamp);
    } else {
        DOUBLE curr_RTCPTS = (DOUBLE)thiz->TimeStampBase;

        curr_RTCPTS += (now - thiz->CreateTime)*(DOUBLE)thiz->ClockRate;
        if(curr_RTCPTS > (DOUBLE)0x00000000ffffffffLU){
            curr_RTCPTS -= (DOUBLE)0x00000000ffffffffLU;
            thiz->CreateTime = now;
            thiz->TimeStampBase = (UINT32)curr_RTCPTS;
        }
        pkt->SrPkt.RtpTimestamp = SvcRtspServer_ihtonl((UINT32)curr_RTCPTS);

        {
            UINT32 RetVal;
            char dst_str[128] = {'\0'};

            if (AmbaWrap_memset(dst_str, 0, 128)!= 0U) { }

            // RetVal = AmbaAdvSnPrint(dst_str, 128U, "curr_RTCPTS: %lf, LastTimeStamp: %u t=(%f), TimeStampBase=%u", curr_RTCPTS, thiz->LastTimeStamp,
            //                        (DOUBLE)thiz->LastTimeStamp/(DOUBLE)thiz->ClockRate, thiz->TimeStampBase);
            {
                char    *StrBuf = dst_str;
                UINT32  BufSize = 128U;
                UINT32  CurStrLen;

                AmbaUtility_StringAppend(StrBuf, BufSize, "curr_RTCPTS: ");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_DoubleToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (DOUBLE)(curr_RTCPTS), 6U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, ", LastTimeStamp: ");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(thiz->LastTimeStamp), 10U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, " t=(");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_FloatToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (FLOAT)((DOUBLE)thiz->LastTimeStamp/(DOUBLE)thiz->ClockRate), 6U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, "), TimeStampBase=");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(thiz->TimeStampBase), 10U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, "");

                RetVal = AmbaUtility_StringLength(StrBuf);

            }

            AmbaPrint_PrintStr5("%s, %s", __func__, dst_str, NULL, NULL, NULL);
            AmbaPrint_PrintUInt5("Len %d", RetVal, 0U, 0U, 0U, 0U);
        }
    }

    pkt->SrPkt.PktCount = SvcRtspServer_ihtonl(thiz->PkgCount);
    pkt->SrPkt.OctetCount = SvcRtspServer_ihtonl(thiz->OctetCount);
}


static void SetupSrBye(const RTP_SINK_s* thiz, RTCP_SR_COMPOUND_s* pkt)
{
    const char* Name = SVC_BYE_REASON;
    UINT32 name_len = AmbaUtility_StringLength(Name);
    UINT32 bye_size = sizeof(RTCP_HEADER_s) + sizeof(RTCP_HEADER_SDES_s);
    UINT32 padding = 4U - (bye_size%4U);
    bye_size += padding;
    if (AmbaWrap_memset(pkt, 0, sizeof(RTCP_SR_COMPOUND_s))!= 0U) { }

    pkt->PayloadHdr.version = 2U;
    pkt->PayloadHdr.padding = 0U;
    pkt->PayloadHdr.count = 1U;
    pkt->PayloadHdr.pt = BYE;
    pkt->PayloadHdr.Length = SvcRtspServer_ihtons((UINT16)((bye_size>>2U) - 1U));

    pkt->Payload.Bye.Ssrc = SvcRtspServer_ihtonl(thiz->Ssrc);
    pkt->Payload.Bye.Length = (UINT8)name_len;
    if (AmbaWrap_memcpy(pkt->Payload.Bye.Reason, Name, name_len)!= 0U) { }
}

/**
* send rtcp packet.
* @param [in]  RTP_SINK_s object pointer.
* @param [in]  RTCP Packet type.
* @param [in]  transport including RTCP socket.
* @return OK/ERR_NA
*/
UINT32 SvcRtcp_SendPacket(RTP_SINK_s* thiz, UINT32 type, const RTP_TRANSPORT_s* Transport)
{
    char                *pPkt;
    RTCP_SR_COMPOUND_s* pkt = NULL;
    RTCP_SR_COMPOUND_s goodBye;
    UINT32 Len = 0U;
    UINT32 RetVal = OK;
    INT32 ErrNum;
    INT32 ret = 0;
    const struct sockaddr_in* pRemoteRtcp;
#if defined(CONFIG_NETX_ENET)
    struct sockaddr* pDestAddr;
#else
    const struct sockaddr* pDestAddr;
#endif

    if ((thiz == NULL) || (Transport == NULL)) {
        AmbaPrint_PrintUInt5("SvcRtcp_SendPacket: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        switch(type) {
        case SDES:
            pkt = &thiz->SrCompound;
            RtcpSr(thiz, pkt);
            Len = sizeof(thiz->SrCompound);
            break;

        case BYE:
            SetupSrBye(thiz, &goodBye);
            pkt = &goodBye;
            RtcpSr(thiz, pkt);
            Len = sizeof(thiz->SrCompound);
            break;
        default:
            AmbaPrint_PrintUInt5("SetupSrBye: rtcp type %u is not implement", \
                            type, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
            break;
        }

        if(OK == RetVal) {
             {
                pRemoteRtcp = &Transport->RemoteRtcp;
                AmbaMisra_TypeCast(&pDestAddr, &pRemoteRtcp);

                AmbaMisra_TypeCast(&pPkt, &pkt);
#if defined(CONFIG_NETX_ENET)
                ret = sendto(Transport->RtcpSock, pPkt, (INT32)Len, 0,
                        pDestAddr, (INT32)sizeof(Transport->RemoteRtcp));
#else
                ret = (INT32)sendto(Transport->RtcpSock, pPkt, Len, 0,
                        pDestAddr, (INT32)sizeof(Transport->RemoteRtcp));
#endif
#if defined(CONFIG_NETX_ENET)
                if(ret != (INT32)Len) {
                    ErrNum = _nx_get_errno();
#else
                if(ret != (INT32)Len) {
                    ErrNum = errno;
#endif

#if defined(CONFIG_NETX_ENET)
                    if ((ErrNum == EAGAIN) || (ErrNum == ENOBUFS)) {
                        (VOID)AmbaKAL_TaskSleep(10U);

                        ret = sendto(Transport->RtcpSock, pPkt, (INT32)Len, 0,
                                    pDestAddr, (INT32)sizeof(Transport->RemoteRtcp));
                        if(ret != (INT32)Len) {

#else
                    if ((ErrNum == EAGAIN) || (ErrNum == EWOULDBLOCK) || (ErrNum == ENOBUFS)) {
                        (VOID)AmbaKAL_TaskSleep(10U);

                        ret = (INT32)sendto(Transport->RtcpSock, pPkt, Len, 0,
                                    pDestAddr, (INT32)sizeof(Transport->RemoteRtcp));
                        if(ret != (INT32)Len) {
#endif
                            AmbaPrint_PrintUInt5("SvcRtcp_SendPacket: error send rtcp packet", \
                                            0U, 0U, 0U, 0U, 0U);
                        }
                    } else {
                        AmbaPrint_PrintUInt5("error send rtcp packet", \
                                        0U, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
        RetVal = OK;
    }
    return RetVal;
}

/**
* send rtcp bye packet.
* @param [in]  RTP_SINK_s object pointer.
* @return OK/ERR_NA
*/
UINT32 SvcRtcp_SendGoodBye(RTP_SINK_s* thiz)
{
    UINT32 i = 0U;
    UINT32 RetVal = OK;

    if(thiz == NULL) {
        RetVal = ERR_NA;
    } else {

        for(i = 0U; i < MAX_RTP_SESSION; i++) {
            if(thiz->Transports[i] != NULL) {
                (VOID)SvcRtcp_SendPacket(thiz, BYE, thiz->Transports[i]);
            }
        }
    }
    return RetVal;
}

/**
* send rtcp SR & SDES packet.
* @param [in]  RTP_SINK_s object pointer.
* @return OK/ERR_NA
*/
UINT32 SvcRtcp_SendSrSdes(RTP_SINK_s* thiz)
{
    UINT32 i = 0U;
    UINT32 RetVal = OK;

    if(thiz == NULL) {
        RetVal = ERR_NA;
    } else {

        for(i = 0; i < MAX_RTP_SESSION; i++) {
            if(thiz->Transports[i] != NULL) {
                (VOID)SvcRtcp_SendPacket(thiz, SDES, thiz->Transports[i]);
            }
        }
    }
    return RetVal;
}

/**
* send rtcp SR & SDES packet.
* @param [in]  RTP_SINK_s object pointer.
* @return OK/ERR_NA
*/
INT32 SvcRtcp_SetupSrSdes(const RTP_SINK_s* thiz, RTCP_SR_COMPOUND_s* pkt)
{
    const char* Name = SVC_DV_NAME;
    UINT32 name_len = AmbaUtility_StringLength(Name);
    UINT32 sdes_size = sizeof(RTCP_HEADER_s) + sizeof(RTCP_HEADER_SDES_s);// + name_len;
    UINT32 padding = 4U - (sdes_size%4U);
    sdes_size += padding;
    if (AmbaWrap_memset(pkt, 0, sizeof(RTCP_SR_COMPOUND_s))!= 0U) { }

    pkt->PayloadHdr.version = 2U;
    pkt->PayloadHdr.padding = 0U;
    pkt->PayloadHdr.count = 1U;
    pkt->PayloadHdr.pt = SDES;
    pkt->PayloadHdr.Length = SvcRtspServer_ihtons((UINT16)((sdes_size>>2U) - 1U));

    pkt->Payload.Sdes.Ssrc = SvcRtspServer_ihtonl(thiz->Ssrc);
    pkt->Payload.Sdes.AttrName = (UINT8)CNAME;
    pkt->Payload.Sdes.Len = (UINT8)name_len;

    if (AmbaWrap_memcpy(pkt->Payload.Sdes.Name, Name, name_len)!= 0U) { }
    return 0;
}

static void ParseReceiverReport(const RTP_SINK_s* thiz, const UINT8* packet, INT32 count)
{
    DOUBLE now, Diff;
    DOUBLE propagationdelay = (DOUBLE)0;
    DOUBLE lastsrts, DelayLastSr;
    UINT32 t_sec;
    SVC_RTSP_RTCP_REPORT_s rr_stat;
    UINT32 SysTickCnt = 0U;
    ULONG AddrCast = 0;
    INT32  Count = count;

    (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
    now = (DOUBLE) SysTickCnt / (DOUBLE)1000;
    t_sec = (UINT32)now + STANDARD_TS_BASE;

    AmbaMisra_TypeCast(&AddrCast, &packet);
    AddrCast += 4U;
    AmbaMisra_TypeCast(&packet, &AddrCast);

  //  packet += 4;
    while(Count > 0) {
        const RTCP_REPORT_BLOCK_s *report;
        AmbaMisra_TypeCast(&report, &packet);
        Count--;
        {//priv->en_rtcp){
            /* LSR represent the middle 32bits of 64bit NTP timestamp format(a fixed-point number with integer part in the first 32bit
            *  and the fractional part in the last 32 bits).
            *  Hence, LSR&0xffff0000 >> 16 = integer part. LSR& 0x0000ffff/65536 = fraction part.
            */
            UINT32 HTemp1;
            DOUBLE HTemp2;
            UINT32 LTemp1;
            DOUBLE LTemp2;
            HTemp1 = ((SvcRtspServer_intohl(report->LastSr) & 0xffff0000U)>>16U);
            HTemp2 = (DOUBLE)HTemp1;
            LTemp1 = (SvcRtspServer_intohl(report->LastSr) & 0x0000ffffU);
            LTemp2 =  (DOUBLE)LTemp1;
            LTemp2 =  LTemp2/(DOUBLE)65536;
            //lastsrts = (DOUBLE)((intohl(report->LastSr) & 0xffff0000U)>>16U) + ((DOUBLE)(intohl(report->LastSr) & 0x0000ffffU)/(DOUBLE)65536);
            lastsrts = HTemp2 + LTemp2;

            LTemp1 = (t_sec & (UINT32)0x0000ffff);
            LTemp2 = (DOUBLE)LTemp1;
            now = LTemp2 +  (now - (DOUBLE)((UINT32) now));
            //now = (DOUBLE)(t_sec & (UINT32)0x0000ffff) + (now - (DOUBLE)((UINT32) now));
            DelayLastSr = (DOUBLE)SvcRtspServer_intohl(report->DelayLastSr)/(DOUBLE)65536;
            if((report->LastSr != 0U) && (report->DelayLastSr != 0U)){
                if(now < lastsrts) {
                    Diff = ((DOUBLE)65536 + now - lastsrts);
                } else {
                    Diff = now - lastsrts;
                }
                propagationdelay = (Diff - DelayLastSr);
            }
        }
/*  Debug
        do {
            int ssrc = SvcRtspServer_intohl(((rtcp_header_rr*)packet)->ssrc);
            AmbaPrint("[RTCP] ssrc %u, fraction %d, lost %d, "
                "sequence %u, jitter %u, "
                "last Sender Report %lf, delay %u\n",
                report->ssrc, report->fract_lost,
                (report->packet_lost[0]<<3)|(report->packet_lost[1]<<2)|(report->packet_lost[2]),
                SvcRtspServer_intohl(report->h_seq_no), SvcRtspServer_intohl(report->jitter),
                last_sr, SvcRtspServer_intohl(report->delay_last_sr));

        } while(0);
*/
        AmbaMisra_TypeCast(&AddrCast, &packet);
        AddrCast += sizeof(RTCP_REPORT_BLOCK_s);
        AmbaMisra_TypeCast(&packet, &AddrCast);

        //packet += sizeof(RTCP_REPORT_BLOCK_s);

        // Only send RR for liveview video now
        rr_stat.FrameLost = (UINT32)report->FractLost;
        rr_stat.Jitter = SvcRtspServer_intohl(report->Jitter);
        rr_stat.PGDelay = propagationdelay;
        if ((1U == thiz->EnDyBr) && (thiz->ReportCallback != NULL)) {
            thiz->ReportCallback(SVC_RTSP_RTCP_RR_STAT, &rr_stat, thiz->ReportCtx);
        }
    }
}

static void ResetClientSessionTimeout(const RTP_SINK_s* thiz, INT32 Sd)
{
    UINT32 i;
    for(i = 0U; i < MAX_RTP_SESSION; i++) {
        if(thiz->Transports[i] != NULL){
            if(thiz->Transports[i]->RtcpSock == Sd) {
                SVC_RTSP_CLNTSESSION_s* pClientSession;
                AmbaMisra_TypeCast(&pClientSession, &thiz->Transports[i]->ClientSession);
                (void)RtspClnt_ResetLstActTime(pClientSession);
                break;
            }
        }
    }
}

/**
* SvcRtcp_RtcpHandler
* @param [in]  RTCP Socket.
* @param [in]  RTP_SINK_s object pointer.
*/
void SvcRtcp_RtcpHandler(INT32 Sd, const void* Ctx)
{
    static char data[MTU_SIZE * 2U];
    const RTP_SINK_s* thiz;
    INT32 Len, Param1, Param2;
    UINT32 rtcp_size;
    ULONG AddrCast = 0;

    Param1 = (INT32)MTU_SIZE * 2;
    Param2 = 0x40;

    if ((Sd == 0) || (Ctx == NULL)) {
        AmbaPrint_PrintUInt5("SvcRtcp_RtcpHandler: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaMisra_TypeCast(&thiz, &Ctx);
        Len = recv(Sd, data, Param1, Param2); // MSG_NOWAIT 0x40
        if(Len > 0) {
            const UINT8* ptr;
            const char* pData;

            pData = &data[0];
            AmbaMisra_TypeCast(&ptr, &pData);

            while((UINT32)Len > sizeof(RTCP_HEADER_s)) {
                const RTCP_HEADER_s *rtcp;
                AmbaMisra_TypeCast(&rtcp, &ptr);
                rtcp_size = (((UINT32)SvcRtspServer_intohs(rtcp->Length) + 1U ) << 2U);
                if(rtcp_size > (UINT32)Len) {
                    AmbaPrint_PrintUInt5("[RTCP] malformed packet", \
                                    0U, 0U, 0U, 0U, 0U);
                    break;
                } else {
                    switch(rtcp->pt) {
                    case BYE:
                        break;
                    case SR:
                        break;
                    case RR:
                        ParseReceiverReport(thiz, &ptr[4], (INT)rtcp->count);
                        ResetClientSessionTimeout(thiz, Sd); //RR can be keep-alive signal
                        break;
                    case SDES:
                    default:
                        /* do nothing */
                        break;
                    }
                    Len -= (INT32)rtcp_size;
                    AmbaMisra_TypeCast(&AddrCast, &ptr);
                    AddrCast += rtcp_size;
                    AmbaMisra_TypeCast(&ptr, &AddrCast);
                }
            }
        }
    }
}
