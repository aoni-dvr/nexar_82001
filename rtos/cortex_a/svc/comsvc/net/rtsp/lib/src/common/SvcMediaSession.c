/**
 *  @file SvcMediaSession.c
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
 *  @details media session module
 *
 */

#include "AmbaTypes.h"
#include <AmbaKAL.h>
#include "AmbaPrint.h"
#include "SvcRtspServer.h"
#include "SvcMediaSubsession.h"
#include "SvcStreamMgr.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"

#if defined(CONFIG_NETX_ENET)
#else
#endif

static char const* server_str = "Ambarella Streaming";
static char const* server_ver = "2016.03.24";

static UINT32 AddSubsession(MEDIA_SESSION_s* thiz, MEDIA_SUBSESSION_s* sub)
{
    UINT32 i;
    UINT32 RetVal = OK;

    for(i = 0U; i < MAX_MEDIA_SUBSESSIONS; i++ ) {
        if(thiz->Subsessions[i] == NULL) {
            break;
        }
    }
    if(i >= MAX_MEDIA_SUBSESSIONS) {
        AmbaPrint_PrintUInt5("AddSubsession: too many media Subsessions", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        thiz->Subsessions[i] = sub;
        sub->Parent = thiz;
        ++(thiz->SubsessionCount);
        sub->TrackNum = thiz->SubsessionCount;
    }

    return RetVal;
}

static UINT32 InitLiveSession(const SVC_RTSP_SERVER_s *pSrv, MEDIA_SESSION_s *pSession, const char *pStreamName)
{
    UINT32 i, MediaCount;
    const UINT32 Live = 1U;
    MEDIA_SUBSESSION_s* pSub = NULL;
    SVC_STREAM_LIVE_MEDIA_s MediaList[MAX_MEDIA_SUBSESSIONS];
    const SVC_STREAM_LIVE_MEDIA_s* pMediaList;
    const void* pVoidMediaList;
    UINT32 RetVal = OK;


    MediaCount = SvcRtspStrmGetMediaID(pStreamName, MediaList);

    for (i = 0U; i < MediaCount; i++) {
        pMediaList = &MediaList[i];
        AmbaMisra_TypeCast(&pVoidMediaList, &pMediaList);
        pSub = SvcMediaSubsession_Create(pSrv, pVoidMediaList, Live);

        if (pSub == NULL) {
            AmbaPrint_PrintUInt5("InitLiveSession: subsession create fail", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
            break;
        }
        (void)AddSubsession(pSession, pSub);
    }
    if(OK == RetVal) {
       if(0U == MediaCount) {
            RetVal = ERR_NA;
       }
    }

    return RetVal;
}

static UINT32 InitPlaybackSession(const SVC_RTSP_SERVER_s *pSrv, MEDIA_SESSION_s* pSession, const char* pStreamName)
{
    UINT32 i;
    INT32 StreamID;
    const UINT32 playback = 0U;
    MEDIA_SUBSESSION_s* pSub;
    SVC_RTSP_MOVIE_INFO_CFG_s mInfo;
    char Filepath[MAX_MEDIA_STREAM_NAME_LENGTH];
    const char* pFilepath;
    const void* pVoidFilepath;
    UINT32 RetVal = OK;

    if (AmbaWrap_memset(&mInfo, 0, sizeof(SVC_RTSP_MOVIE_INFO_CFG_s))!= 0U) { }
    if (SvcRtspServer_GetPlaybackSessionCount(pSrv) > 0U) {
        AmbaPrint_PrintUInt5("[RTSP Server] Multiple playback Session is not allowed", 0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    }

    if(OK == RetVal) {
        // convert url streamName to filepath
        {
            UINT32 Ret;

            if (AmbaWrap_memset(Filepath, 0, MAX_MEDIA_STREAM_NAME_LENGTH)!= 0U) { }
            // Ret = AmbaAdvSnPrint(Filepath, MAX_MEDIA_STREAM_NAME_LENGTH, "/%s", pStreamName);
            AmbaUtility_StringAppend(Filepath, MAX_MEDIA_STREAM_NAME_LENGTH, "/");
            AmbaUtility_StringAppend(Filepath, MAX_MEDIA_STREAM_NAME_LENGTH, pStreamName);
            Ret = AmbaUtility_StringLength(Filepath);

            AmbaPrint_PrintStr5("%s, filepath %s", __func__, Filepath, NULL, NULL, NULL);
            AmbaPrint_PrintUInt5("Len %d", Ret, 0U, 0U, 0U, 0U);
        }

        pFilepath = &Filepath[0];
        AmbaMisra_TypeCast(&pVoidFilepath, &pFilepath);
        StreamID = (INT32)SvcRtspServer_PlaybackCmd(pSrv, SVC_RTSP_PLAYBACK_OPEN, pVoidFilepath);
        if (StreamID < 0) {
            AmbaPrint_PrintStr5("InitPlaybackSession: RTSP_PLAYBACK_OPEN %s fail", Filepath, NULL, NULL, NULL, NULL);
            RetVal = ERR_NA;
        }

        if(OK == RetVal) {
            // get media info of playback stream
            (void)SvcRtspStrmMgrGetMediaInfo(NULL, (UINT32)StreamID, &mInfo);

            if (0U == mInfo.nTrack) {
                AmbaPrint_PrintUInt5("InitPlaybackSession: no track in mediaInfo", 0U, 0U, 0U, 0U, 0U);
                RetVal = ERR_NA;
            }

            if(OK == RetVal) {

                pSession->Duration = mInfo.nDuration;

                AmbaPrint_PrintUInt5("InitPlaybackSession: Got Duration = %u", pSession->Duration, 0U, 0U, 0U, 0U);
                pSub = NULL;
                for (i = 0U; i < mInfo.nTrack; i++) {
                    pSub = SvcMediaSubsession_Create(pSrv, &mInfo.Track[i], playback);
                    if (pSub == NULL) {
                        AmbaPrint_PrintUInt5("InitPlaybackSession: subsession create fail", 0U, 0U, 0U, 0U, 0U);
                        RetVal = ERR_NA;
                        break;
                    }
                    (void)AddSubsession(pSession, pSub);
                }
            }

        }
    }

    return RetVal;
}

/**
* create MediaSession object.
* @param [in]  SVC_RTSP_SERVER_s object pointer.
* @param [in]  stream name of new created mediaSession.
* @return MEDIA_SESSION_s object created. NULL when failed.
*/
MEDIA_SESSION_s* SvcMediaSession_Create(const void *pSrv, const char* pStreamName)
{
    MEDIA_SESSION_s* Session;
    const void* pSession;
    UINT32 rval;
    UINT32 SysTickCnt = 0U;
    DOUBLE Now;
    const SVC_RTSP_SERVER_s *Srv ;
    AmbaMisra_TypeCast(&Srv, &pSrv);

    if ((Srv == NULL) || (pStreamName == NULL) || (pStreamName[0] == '\0')) {
        AmbaPrint_PrintUInt5("SvcMediaSession_Create: Invalid parameter.", \
                        0U, 0U, 0U, 0U, 0U);
        Session = NULL;
    } else {
        pSession = SvcRtspServer_GetSessionSlot(Srv);
        AmbaMisra_TypeCast(&Session, &pSession);

        if(Session != NULL){
            if (AmbaUtility_StringCompare(pStreamName, "live", 4) == 0) { // streams' name begin with live are regarded as live session
                rval = InitLiveSession(Srv, Session, pStreamName);
            } else {
                rval = InitPlaybackSession(Srv, Session, pStreamName);
            }

            if (OK == rval) {
                DOUBLE Tmp;
                (void)AmbaKAL_GetSysTickCount(&SysTickCnt);
                Now = (DOUBLE) SysTickCnt/ (DOUBLE)1000;
                Session->CreateSec = (UINT32) Now + 2208988800u;
                Tmp = (Now - (DOUBLE)((UINT32) Now))*(DOUBLE)1000000000;
                Session->CreateNsec =  (UINT32)Tmp;
                AmbaPrint_PrintUInt5("sec %d, nsec %d", Session->CreateSec, Session->CreateNsec, 0U, 0U, 0U);
                AmbaUtility_StringCopy(Session->StreamName, MAX_MEDIA_STREAM_NAME_LENGTH, pStreamName);
                AmbaUtility_StringCopy(Session->DescSdpString, MAX_MEDIA_DESC_LENGTH, server_str);
                AmbaUtility_StringCopy(Session->InfoSdpString, MAX_MEDIA_DESC_LENGTH, server_ver);
            }
        }
    }
    return Session;
}

/**
* release MediaSession object.
* @param [in]  MEDIA_SESSION_s object pointer.
*/
void SvcMediaSession_Release(const void *pthiz)
{
    UINT32 i;
    MEDIA_SESSION_s* thiz;
    AmbaMisra_TypeCast(&thiz, &pthiz);

    for (i = 0U; i < MAX_MEDIA_SUBSESSIONS; i++ ) {
        if (thiz->Subsessions[i] != NULL) {
            SvcMediaSubsession_Release(thiz->Subsessions[i]);
        }
    }

    if (AmbaWrap_memset(thiz, 0, sizeof(MEDIA_SESSION_s))!= 0U) { }
}

/**
* free all sdp descriptions of mediasession and all submediaSessions.
* @param [in]  MEDIA_SESSION_s object pointer.
* @return OK/ERR_NA
*/
UINT32 SvcMediaSession_FreeSDPDescription(const void *pthiz)
{
    UINT32 i;
    const MEDIA_SUBSESSION_s* pSubsession;
    const void* pVoidSubsession;
    MEDIA_SESSION_s* thiz;
    UINT32 RetVal = OK;
    AmbaMisra_TypeCast(&thiz, &pthiz);

    if (thiz == NULL) {
        AmbaPrint_PrintUInt5("SvcMediaSession_FreeSDPDescription: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        SvcRtspServer_MemPool_Free(SVC_RTSP_POOL_SDP_LINE_BUF, thiz->SdpLine);
        thiz->SdpLine = NULL;
        for(i = 0U; i < MAX_MEDIA_SUBSESSIONS; i++) {
            pVoidSubsession = thiz->Subsessions[i];
            AmbaMisra_TypeCast(&pSubsession, &pVoidSubsession);
            if(pSubsession != NULL) {
                (void)pSubsession->FreeSdpLines(pSubsession);
            }
        }
    }

    return RetVal;
}

/**
* generate all sdp descriptions of mediasession and all submediaSessions.
* Caller need to call AmpRTSPSession_FreeSDPDescription after use it.
* @param [in]  SVC_RTSP_SERVER_s object pointer.
* @param [in]  MEDIA_SESSION_s object pointer.
* @param [in]  IP address of this server.
* @return SDP Description string pointer, NULL when failed.
*/
char* SvcMediaSession_GenSDPDescription(const void *pSrv, const void *pthiz, UINT32 IpAddress)
{
    UINT32 i = 0U;
    char ip[16];
    UINT32 SdpLen = 0U;
    const MEDIA_SUBSESSION_s* pSubsession = NULL;
    char RangeLine[32];
    char* pMediaSdp;
    const char* pSourceFilterLine = "";
    char const* pSdpPrefixFmt = "v=0\r\n"
            "o=- %u%06u %d IN IP4 %s\r\n"
            "s=%s\r\n"
            "i=%s\r\n"
            "t=0 0\r\n"
            "a=tool:%s %s\r\n"
            "a=type:broadcast\r\n"
            "a=control:*\r\n"
            "%s"
            "%s"
            "a=x-qt-text-nam:%s\r\n"
            "a=x-qt-text-inf:%s\r\n";
    const SVC_RTSP_SERVER_s* Srv;
    MEDIA_SESSION_s* thiz;
    char* pSdpLine = NULL;
    const void* pVoidChar;

    AmbaMisra_TypeCast(&Srv, &pSrv);
    AmbaMisra_TypeCast(&thiz, &pthiz);

    if ((Srv == NULL) || (thiz == NULL)) {
        AmbaPrint_PrintStr5("SvcMediaSession_GenSDPDescription: Invalid input", NULL, NULL, NULL, NULL, NULL);
        pSdpLine = NULL;
    } else {

        if (0U != IpAddress) {
            {
                UINT32 Ret;

                if (AmbaWrap_memset(ip, 0, 16)!= 0U) { }
                // Ret = AmbaAdvSnPrint(ip, 16U, "%u.%u.%u.%u", (UINT8)((IpAddress & 0xFF000000U) >> 24U),
                //                         (UINT8)((IpAddress & 0x00FF0000U) >> 16U), (UINT8)((IpAddress & 0x0000FF00U) >> 8U), (UINT8)(IpAddress & 0x000000FFU));

                {
                    char    *StrBuf = ip;
                    UINT32  BufSize = 16UL;
                    UINT32  CurStrLen, Temp;

                    AmbaUtility_StringAppend(StrBuf, BufSize, "");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        Temp = ((IpAddress & 0xFF000000UL) >> 24U);
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)Temp, 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, ".");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        Temp = ((IpAddress & 0x00FF0000UL) >> 16U);
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)Temp, 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, ".");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        Temp = ((IpAddress & 0x0000FF00UL) >> 8U);
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)Temp, 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, ".");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        Temp = ((IpAddress & 0x000000FFUL));
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)Temp, 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, "");

                    Ret = AmbaUtility_StringLength(StrBuf);

                }
                AmbaPrint_PrintStr5("%s, ip %s", __func__, ip, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("Len %d", Ret, 0U, 0U, 0U, 0U);
            }
        } else {
            {
                UINT32 Ret;

                if (AmbaWrap_memset(ip, 0, 16)!= 0U) { }
                // Ret = AmbaAdvSnPrint(ip, 16U, "192.168.42.1");
                AmbaUtility_StringAppend(ip, 16U, "192.168.42.1");
                Ret = AmbaUtility_StringLength(ip);

                AmbaPrint_PrintStr5("%s, ip %s", __func__, ip, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("Len %d", Ret, 0U, 0U, 0U, 0U);
            }
        }

        if(0U == thiz->Duration){
            {
                UINT32 Ret;

                if (AmbaWrap_memset(RangeLine, 0, 32) != 0U) {
                }
                AmbaUtility_StringAppend(RangeLine, 32UL, "a=range:npt=0-\r\n");
                Ret = AmbaUtility_StringLength(RangeLine);
                AmbaPrint_PrintStr5("%s, RangeLine %s", __func__, RangeLine, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("Len %d", Ret, 0U, 0U, 0U, 0U);
            }
        } else {
            {
                UINT32 Ret, CurStrLen;

                if (AmbaWrap_memset(RangeLine, 0, 32) != 0U) {
                }
                AmbaUtility_StringAppend(RangeLine, 32UL, "a=range:npt=0-");
                CurStrLen = AmbaUtility_StringLength(RangeLine);
                Ret = CurStrLen;
                if (AmbaUtility_DoubleToStr(&(RangeLine[CurStrLen]),
                                            (32UL - CurStrLen),
                                            (DOUBLE)thiz->Duration/(DOUBLE)1000,
                                            3U) == OK) {
                    Ret = AmbaUtility_StringLength(RangeLine);
                }

                AmbaPrint_PrintStr5("%s, RangeLine %s", __func__, RangeLine, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("Len %d", Ret, 0U, 0U, 0U, 0U);
            }
        }

        for(i = 0U; i < MAX_MEDIA_SUBSESSIONS; i++) {
            const void* pVoidSubsessions;
            pVoidSubsessions = thiz->Subsessions[i];
            AmbaMisra_TypeCast(&pSubsession, &pVoidSubsessions);
            if(pSubsession != NULL) {
                SdpLen += AmbaUtility_StringLength(pSubsession->GetSdpLines(pSubsession));
            }
        }

        SdpLen += (UINT32)AmbaUtility_StringLength(pSdpPrefixFmt);
        SdpLen += ((20U + 6U) + 20U);
        SdpLen += AmbaUtility_StringLength(ip);
        SdpLen += (UINT32)AmbaUtility_StringLength(thiz->DescSdpString);
        SdpLen += (UINT32)AmbaUtility_StringLength(thiz->InfoSdpString);
        SdpLen += (UINT32)AmbaUtility_StringLength(server_str);
        SdpLen += (UINT32)AmbaUtility_StringLength(server_ver);
        SdpLen += (UINT32)AmbaUtility_StringLength(pSourceFilterLine);
        SdpLen += (UINT32)AmbaUtility_StringLength(RangeLine);
        SdpLen += (UINT32)AmbaUtility_StringLength(thiz->DescSdpString);
        SdpLen += (UINT32)AmbaUtility_StringLength(thiz->InfoSdpString);

        if (thiz->SdpLine != NULL) {
            SvcRtspServer_MemPool_Free(SVC_RTSP_POOL_SDP_LINE_BUF, thiz->SdpLine);
        }

        //thiz->SdpLine = RTSPServer_MemPool_Malloc(SVC_RTSP_POOL_SDP_LINE_BUF, SdpLen + 1);
        pVoidChar = SvcRtspServer_MemPool_Malloc(SVC_RTSP_POOL_SDP_LINE_BUF, (SdpLen + 1U));
        AmbaMisra_TypeCast(&thiz->SdpLine, &pVoidChar);


        if (thiz->SdpLine == NULL) {
            AmbaPrint_PrintUInt5("SvcMediaSession_GenSDPDescription: malloc SdpLen(%u) fail", \
                            SdpLen, 0U, 0U, 0U, 0U);
            pSdpLine = NULL;
        } else {
            // Generate the SDP prefix (session-level lines):
            {
                UINT32 Ret;

                if (AmbaWrap_memset(thiz->SdpLine, 0, SdpLen)!= 0U) { }

                // Ret = AmbaAdvSnPrint(thiz->SdpLine, SdpLen, pSdpPrefixFmt, thiz->CreateSec, thiz->CreateNsec, 1U, ip, thiz->DescSdpString,
                //                      thiz->InfoSdpString, server_str, server_ver, pSourceFilterLine, RangeLine, thiz->DescSdpString,
                //                      thiz->InfoSdpString);
                {
                    char    *StrBuf = thiz->SdpLine;
                    UINT32  BufSize = SdpLen;
                    UINT32  CurStrLen;

                    AmbaUtility_StringAppend(StrBuf, BufSize, "v=0\r\no=- ");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(thiz->CreateSec), 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, "");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        char    NumBuf[16];
                        UINT32  NumLen;

                        if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                        (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(thiz->CreateNsec), 10U);
                        NumLen = AmbaUtility_StringLength(NumBuf);

                        while (NumLen < 6UL) {
                            AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                            NumLen++;
                        }

                        AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, " ");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(1U), 10U);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, " IN IP4 ");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (ip));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\ns=");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (thiz->DescSdpString));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\ni=");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (thiz->InfoSdpString));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\nt=0 0\r\na=tool:");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (server_str));
                    AmbaUtility_StringAppend(StrBuf, BufSize, " ");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (server_ver));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\na=type:broadcast\r\na=control:*\r\n");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (pSourceFilterLine));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (RangeLine));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "a=x-qt-text-nam:");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (thiz->DescSdpString));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\na=x-qt-text-inf:");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (thiz->InfoSdpString));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");

                    Ret = AmbaUtility_StringLength(StrBuf);
                }

                AmbaPrint_PrintStr5("%s, sdp %s", __func__, thiz->SdpLine, NULL, NULL, NULL);
                AmbaPrint_PrintUInt5("Len %d", Ret, 0U, 0U, 0U, 0U);
            }

            // Then, add the (media-level) lines for each pSubsession:
            pMediaSdp = thiz->SdpLine;
            for(i = 0U; i < MAX_MEDIA_SUBSESSIONS; i++) {
                const void* pVoidSubsessions;
                pVoidSubsessions = thiz->Subsessions[i];
                AmbaMisra_TypeCast(&pSubsession, &pVoidSubsessions);
                if(pSubsession != NULL) {
                    AmbaUtility_StringAppend(pMediaSdp, SdpLen, pSubsession->GetSdpLines(pSubsession));
                }
            }
            pSdpLine = thiz->SdpLine;
        }
     }
    return pSdpLine;
}

/**
* Setup UDP Transport.
* @param [in]  MEDIA_SESSION_s object pointer.
* @param [in]  TrackId name.
* @param [in]  SessionId of client session.
* @param [in]  IPV4 address of client.
* @param [in]  RTP port of client.
* @param [in]  RTCP port of client.
* @param [in]  SVC_RTSP_CLNTSESSION_s object pointer.
* @param [out]  RTP port of RTSP server.
* @param [out]  RTCP port of RTSP Server.
* @return OK when success, -1/-2/-3 for other errors.
*/
INT32 SvcMediaSession_SetupUDPTransport(const void *pSession,
                                const char* TrackId,
                                UINT32 SessionId,
                                struct in_addr clnt_addr,
                                UINT32 clntRTPPortNum,
                                UINT32 clntRTCPPortNum,
                                UINT32* servRTPPortNum,
                                UINT32* servRTCPPortNum,
                                void* clientSession)
{
    const MEDIA_SUBSESSION_s* pSub = NULL;
    UINT32 i;
    const MEDIA_SESSION_s* Session;
    INT32 Rval = 0;
    AmbaMisra_TypeCast(&Session, &pSession);

    if ((Session == NULL) || (TrackId == NULL) || (servRTPPortNum == NULL) || (servRTCPPortNum == NULL)) {
        AmbaPrint_PrintUInt5("SvcMediaSession_SetupUDPTransport: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        Rval = -1;
    } else {

       if (TrackId[0] != '\0') {
            for (i = 0; i < MAX_MEDIA_SUBSESSIONS ; i++) {
                const void* pVoidSubsessions;
                INT32 Temp;
                pVoidSubsessions = Session->Subsessions[i];
                AmbaMisra_TypeCast(&pSub, &pVoidSubsessions);
                Temp = AmbaUtility_StringCompare(pSub->TrackId, TrackId, AmbaUtility_StringLength(pSub->TrackId));
                if ((pSub != NULL) && (0 == Temp)) {
                    break;
                }
            }
            if ((i == MAX_MEDIA_SUBSESSIONS) && (pSub == NULL)) {
                Rval = -2;
            }
        } else {
            Rval = -3;
        }
        if(0 == Rval) {
            Rval = SvcMediaSubsession_SetupUDPTransport(pSub,
                                    SessionId,
                                    clnt_addr,
                                    clntRTPPortNum,
                                    clntRTCPPortNum,
                                    servRTPPortNum,
                                    servRTCPPortNum,
                                    clientSession);
        }
    }
    return Rval;
}

/**
* start stream of specific media session.
* @param [in]  MEDIA_SESSION_s object pointer.
* @param [in]  SessionId of client session.
* @param [in]  operation code.
* @param [in]  rtsp url.
* @param [in]  buffer length of pRtpInfo.
* @param [out]  buffer to store rtp info.
* @return OK/ERR_NA.
*/
UINT32 SvcMediaSession_StartStream(const void *pSession,
                                UINT32 SessionId,
                                UINT32 op,
                                const char* pRtspUrl,
                                char* pRtpInfo,
                                UINT32 RtpInfoMaxLen)
{
    UINT32 i;
    const MEDIA_SUBSESSION_s* sub;
    UINT16 rtp_seq;
    UINT32 RtpTimestamp, rtp_info_cnt = 0U;
    char rtp_info_tmp[512] = { '\0' };
    const MEDIA_SESSION_s *Session;
    MEDIA_SUBSESSION_s *SubSession;
    UINT32 RetVal = OK;
    AmbaMisra_TypeCast(&Session, &pSession);

    if ((Session == NULL) || (pRtspUrl == NULL) || (pRtpInfo == NULL)) {
        AmbaPrint_PrintUInt5("SvcMediaSession_StartStream: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {

        sub = NULL;
        for (i = 0; i < MAX_MEDIA_SUBSESSIONS ; i++) {
            const void* pVoidSubsessions;
            UINT32 Temp;
            pVoidSubsessions = Session->Subsessions[i];
            AmbaMisra_TypeCast(&sub, &pVoidSubsessions);
            Temp = SvcMediaSubsession_StartStream(sub, SessionId, op, &rtp_seq, &RtpTimestamp);
            if ((sub != NULL) &&
                (OK == Temp)) {
                {
                    UINT32 Ret;
                    const void* pVoidSubsessionsTmp;

                    if (AmbaWrap_memset(rtp_info_tmp, 0, RtpInfoMaxLen)!= 0U) { }

                    pVoidSubsessionsTmp = Session->Subsessions[i];
                    AmbaMisra_TypeCast(&SubSession, &pVoidSubsessionsTmp);
                    AmbaMisra_TouchUnused(SubSession);

                    // Ret = AmbaAdvSnPrint(rtp_info_tmp, 512, "url=%s/%s", pRtspUrl, SubSession->TrackId);

                    {
                        char    *StrBuf = rtp_info_tmp;
                        UINT32  BufSize = 512;

                        AmbaUtility_StringAppend(StrBuf, BufSize, "url=");
                        AmbaUtility_StringAppend(StrBuf, BufSize, pRtspUrl);
                        AmbaUtility_StringAppend(StrBuf, BufSize, "/");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (SubSession->TrackId));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "");

                        Ret = AmbaUtility_StringLength(StrBuf);
                    }

                    AmbaUtility_StringAppend(pRtpInfo, RtpInfoMaxLen, rtp_info_tmp);

                    AmbaPrint_PrintStr5("%s, pRtpInfo %s, rtp_info_tmp %s", __func__, pRtpInfo, rtp_info_tmp, NULL, NULL);
                    AmbaPrint_PrintUInt5("Len %d", Ret, 0U, 0U, 0U, 0U);
                }

                {
                    UINT32 Ret;

                    if (AmbaWrap_memset(rtp_info_tmp, 0, RtpInfoMaxLen)!= 0U) { }
                    // Ret = AmbaAdvSnPrint(rtp_info_tmp, 512, ";seq=%u", rtp_seq);
                    {
                        char    *StrBuf = rtp_info_tmp;
                        UINT32  BufSize = RtpInfoMaxLen;
                        UINT32  CurStrLen;

                        AmbaUtility_StringAppend(StrBuf, BufSize, ";seq=");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(rtp_seq), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, "");

                        Ret = AmbaUtility_StringLength(StrBuf);
                    }
                    AmbaUtility_StringAppend(pRtpInfo, RtpInfoMaxLen, rtp_info_tmp);

                    AmbaPrint_PrintStr5("%s, pRtpInfo 2 %s, rtp_info_tmp %s", __func__, pRtpInfo, rtp_info_tmp, NULL, NULL);
                    AmbaPrint_PrintUInt5("Len %d", Ret, 0U, 0U, 0U, 0U);
                }

                {
                    UINT32 Ret;

                    if (AmbaWrap_memset(rtp_info_tmp, 0, RtpInfoMaxLen)!= 0U) { }

                    // Ret = AmbaAdvSnPrint(rtp_info_tmp, 512, ";rtptime=%u,", (UINT16)RtpTimestamp);
                    {
                        char    *StrBuf = rtp_info_tmp;
                        UINT32  BufSize = RtpInfoMaxLen;
                        UINT32  CurStrLen;

                        AmbaUtility_StringAppend(StrBuf, BufSize, ";rtptime=");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)((UINT16)RtpTimestamp), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, ",");

                        Ret = AmbaUtility_StringLength(StrBuf);
                    }

                    AmbaUtility_StringAppend(pRtpInfo, RtpInfoMaxLen, rtp_info_tmp);

                    AmbaPrint_PrintStr5("%s, pRtpInfo 2 %s, rtp_info_tmp %s", __func__, pRtpInfo, rtp_info_tmp, NULL, NULL);
                    AmbaPrint_PrintUInt5("Len %d", Ret, 0U, 0U, 0U, 0U);
                }
               rtp_info_cnt++;
            } else {
                break;
            }
        }

        if (0U == rtp_info_cnt) {
            pRtpInfo[0] = '\0';
        } else {
            pRtpInfo[((UINT32)AmbaUtility_StringLength(pRtpInfo) - 1U)] = '\0';  // remove last ','
        }
    }

    return RetVal;
}

/**
* pause stream of specific media session.
* @param [in]  MEDIA_SESSION_s object pointer.
* @param [in]  SessionId of client session.
* @return OK/ERR_NA.
*/
UINT32 SvcMediaSession_PauseStream(const void *pSession, UINT32 SessionId)
{
    UINT32 i;
    const MEDIA_SUBSESSION_s* sub;
    const MEDIA_SESSION_s* Session;
    UINT32 RetVal = OK;
    AmbaMisra_TypeCast(&Session, &pSession);

    if ((Session == NULL) || (0U == SessionId)) {
        AmbaPrint_PrintUInt5("SvcMediaSession_PauseStream: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {

        sub = NULL;
        for (i = 0U; i < MAX_MEDIA_SUBSESSIONS; i++) {
            const void* pVoidSubsessions;
            pVoidSubsessions = Session->Subsessions[i];
            AmbaMisra_TypeCast(&sub, &pVoidSubsessions);
            if (sub != NULL) {
                (void)SvcMediaSubsession_PauseStream(sub, SessionId);
            }
        }
    }
    return RetVal;
}

/**
* teardown stream of specific media session.
* @param [in]  MEDIA_SESSION_s object pointer.
* @param [in]  SessionId of client session.
* @return OK / number of remained connection.
*/
UINT32 SvcMediaSession_TearDown(const void *pSession, UINT32 SessionId)
{
    UINT32 i;
    const MEDIA_SUBSESSION_s* sub;
    UINT32 RemainedConnection = 0U;
    const MEDIA_SESSION_s* Session;
    AmbaMisra_TypeCast(&Session, &pSession);

    if (Session == NULL) {
        AmbaPrint_PrintUInt5("SvcMediaSession_TearDown: Invalid input", \
                        0U, 0U, 0U, 0U, 0U);
    } else {

        sub = NULL;
        for (i = 0U; i< MAX_MEDIA_SUBSESSIONS ; i++) {
            const void* pVoidSubsessions;
            pVoidSubsessions = Session->Subsessions[i];
            AmbaMisra_TypeCast(&sub, &pVoidSubsessions);
            if (sub != NULL){
                RemainedConnection += SvcMediaSubsession_TearDown(sub, SessionId);
            }
        }
    }
    return RemainedConnection;
}
