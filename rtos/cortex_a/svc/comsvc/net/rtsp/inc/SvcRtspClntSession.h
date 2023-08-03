/**
 *  @file SvcRtspClntSession.h
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
 *  @details rtsp client session
 *
 */

#ifndef SVC_RTSP_CLNT_SESSION_H
#define SVC_RTSP_CLNT_SESSION_H

#if defined(CONFIG_NETX_ENET)
#include "NetXStack.h"
#include "nx_api.h"
#include "nx_bsd.h"
#elif defined(CONFIG_LWIP_ENET)
#define AMBA_ERROR_CODE_H
#define LWIP_PATCH_MISRA   // for misra depress
#include "lwip/sockets.h"
#include "lwip/errno.h"
#endif
#include "SvcMediaSession.h"

#define SVC_RTSP_REQ_BUFFER_SIZE ((UINT32)2U << 10U)

#define SVC_RTSP_CLNT_INIT      0U
#define SVC_RTSP_CLNT_READY     1U
#define SVC_RTSP_CLNT_PLAYING   2U
#define SVC_RTSP_CLNT_PAUSE     3U

typedef struct {
    void *Srv;          /*< rtsp server */
    UINT32 Status;                          /*< the state machine of client sesssion */
    UINT8 Active;                           /*< indicate whether session is still active */
    UINT8 Padding1;                         /*< reserved 1 bytes */
    UINT16 Padding2;                        /*< reserved 2 bytes */
    INT32 Sd;                                 /*< client socket descriptor */
    UINT32 SessionId;                       /*< unique session identifier */
    UINT32 LastActivity;                    /*< the system time tick of last activity */
    struct sockaddr_in ClientAddr;
    UINT32 RequestBytes;
    UINT32 RequestBufLeft;
    UINT8* LastCRLF;
    UINT8 RequestBuf[SVC_RTSP_REQ_BUFFER_SIZE];
    UINT8* ResponseBuf;
    MEDIA_SESSION_s* MediaSession;           /*< pointer to instance in rtspServer */
    UINT32 StartNpt;                        /*< saved play start time */
    UINT32 StartSysTick;                    /*< saved play start system tick */
    UINT32 PauseNpt;                        /*< the npt time of pause */
}SVC_RTSP_CLNTSESSION_s;

#define RTSP_PARAM_STRISVC_FRWK_NG_MAX 256U
#define RTP_UDP 0U
#define RTP_TCP 1U
#define RAW_UDP 2U
UINT32 RtspClnt_ResetLstActTime(SVC_RTSP_CLNTSESSION_s* Clnt);
UINT32 RtspClnt_GetLstActTime(const SVC_RTSP_CLNTSESSION_s* Clnt);
UINT32 RtspClnt_Release(SVC_RTSP_CLNTSESSION_s* Clnt);
UINT32 RtspClnt_Create(const void *pSrv, INT32 Sd, struct sockaddr_in ClientAddr);
void RtspClnt_IncomingHandler(SVC_RTSP_CLNTSESSION_s* Clnt);

#endif /*SVC_RTSP_CLNT_SESSION_H*/
