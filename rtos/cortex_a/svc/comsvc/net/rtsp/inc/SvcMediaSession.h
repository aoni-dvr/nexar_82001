/**
 *  @file SvcMediaSession.h
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
 *  @details media session
 *
 */
#ifndef SVC_MEDIA_SESSION_H
#define SVC_MEDIA_SESSION_H

#include "AmbaKAL.h"
#include "NetStack.h"
#include <AmbaTypes.h>

#if defined(CONFIG_NETX_ENET)
#include "NetXStack.h"
#include "nx_bsd.h"
#elif defined(CONFIG_LWIP_ENET)
#define LWIP_PATCH_MISRA   // for misra depress
#include "lwip/sockets.h"
#elif defined(CONFIG_LINUX) && !defined(CONFIG_RTSP_LINUX)
#include <netinet/in.h>
#endif
#include "SvcRtsp.h"

#define MAX_MEDIA_SUBSESSIONS SVC_RTSP_MAX_TRACK_PER_MEDIA
#define MAX_MEDIA_STREAM_NAME_LENGTH 128
#define MAX_MEDIA_DESC_LENGTH 64

typedef struct {
    void *Subsessions[MAX_MEDIA_SUBSESSIONS];
    char StreamName[MAX_MEDIA_STREAM_NAME_LENGTH];
    char InfoSdpString[MAX_MEDIA_DESC_LENGTH];
    char DescSdpString[MAX_MEDIA_DESC_LENGTH];
    char* SdpLine;
    UINT32 SubsessionCount;
    UINT32 Duration;
    UINT32 CreateSec;
    UINT32 CreateNsec;
} MEDIA_SESSION_s;

MEDIA_SESSION_s* SvcMediaSession_Create(const void *pSrv, const char* pStreamName);
void SvcMediaSession_Release(const void *pthiz);
UINT32 SvcMediaSession_FreeSDPDescription(const void *pthiz);
char* SvcMediaSession_GenSDPDescription(const void *pSrv, const void *pthiz, UINT32 IpAddress);

INT32 SvcMediaSession_SetupUDPTransport(const void *pSession,
                                const char* TrackId,
                                UINT32 SessionId,
                                struct in_addr clnt_addr,
                                UINT32 clntRTPPortNum,
                                UINT32 clntRTCPPortNum,
                                UINT32* servRTPPortNum,
                                UINT32* servRTCPPortNum,
                                void* clientSession);

UINT32 SvcMediaSession_StartStream(const void *pSession,
                                UINT32 SessionId,
                                UINT32 op,
                                const char* pRtspUrl,
                                char* pRtpInfo,
                                UINT32 RtpInfoMaxLen);
UINT32 SvcMediaSession_PauseStream(const void *pSession, UINT32 SessionId);
UINT32 SvcMediaSession_TearDown(const void *pSession, UINT32 SessionId);

#endif /*SVC_MEDIA_SESSION_H*/
