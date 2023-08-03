/**
 *  @file AmbaMonListen.h
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Constants and Definitions for Amba Monitor Listen
 *
 */

#ifndef AMBA_MONITOR_LISTEN_H
#define AMBA_MONITOR_LISTEN_H

typedef AMBA_MON_MESSAGE_CHUNK_INFO_s AMBA_MON_LISTEN_CHUNK_INFO_s;

typedef struct /*_AMBA_MON_LISTEN_PORT_LINK_s_*/ {
    struct AMBA_MON_LISTEN_PORT_s_    *Up;
    struct AMBA_MON_LISTEN_PORT_s_    *Down;
} AMBA_MON_LISTEN_PORT_LINK_s;

typedef struct AMBA_MON_LISTEN_PORT_s_ {
    UINT32                          Magic;
    AMBA_KAL_MUTEX_t                Mutex;
    AMBA_MON_MESSAGE_PORT_s         MsgPort;
    struct AMBA_MON_LISTEN_ID_s_    *pListenId;
    AMBA_MON_LISTEN_PORT_LINK_s     Link;
} AMBA_MON_LISTEN_PORT_s;

typedef struct /*_AMBA_MON_LISTEN_ID_LINK_s_*/ {
    struct AMBA_MON_LISTEN_ID_s_    *Up;
    struct AMBA_MON_LISTEN_ID_s_    *Down;
} AMBA_MON_LISTEN_ID_LINK_s;

typedef struct AMBA_MON_LISTEN_ID_s_ {
    UINT32                       Magic;
    AMBA_KAL_MUTEX_t             Mutex;
    const char                   *pName;
    AMBA_MON_MESSAGE_ID_s        MsgId;
    AMBA_MON_LISTEN_PORT_s       *pPortList;
    AMBA_MON_LISTEN_ID_LINK_s    Link;
} AMBA_MON_LISTEN_ID_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SafetyListen.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaMonListen_Init(void);

UINT32 AmbaMonListen_Create(AMBA_MON_LISTEN_ID_s *pListenId, const char *pName, const AMBA_MON_LISTEN_CHUNK_INFO_s *pChunkInfo);
UINT32 AmbaMonListen_Reset(AMBA_MON_LISTEN_ID_s *pListenId);
UINT32 AmbaMonListen_Delete(AMBA_MON_LISTEN_ID_s *pListenId);

UINT32 AmbaMonListen_Open(AMBA_MON_LISTEN_PORT_s *pPort, const char *pName);
UINT32 AmbaMonListen_Attach(AMBA_MON_LISTEN_PORT_s *pPort, AMBA_MON_MESSAGE_PORT_s *pSrcMsgPort, const char *pPutEventName, AMBA_MON_MESSAGE_CB_ID_s *pCbId, AMBA_MON_MESSAGE_CB_f pFunc);
UINT32 AmbaMonListen_Remove(AMBA_MON_LISTEN_PORT_s *pPort, AMBA_MON_MESSAGE_CB_ID_s *pCbId);
UINT32 AmbaMonListen_Close(AMBA_MON_LISTEN_PORT_s *pPort);

  void AmbaMonListen_CbEntry(void *pCbId, void *pMsg);

#endif /* AMBA_MON_LISTEN_H */
