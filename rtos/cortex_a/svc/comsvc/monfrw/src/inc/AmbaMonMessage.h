/**
 *  @file AmbaMonMessage.h
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
 *  @details Constants and Definitions for Amba Monitor Message
 *
 */

#ifndef AMBA_MONITOR_MESSAGE_H
#define AMBA_MONITOR_MESSAGE_H

typedef enum /*_AMBA_MON_MSG_ID_e_*/ {
    AMBA_MON_MSG_ID_DATA = 0,
    AMBA_MON_MSG_ID_ECHO = 253,
    AMBA_MON_MSG_ID_LINK = 254,
    AMBA_MON_MSG_ID_BREAK
} AMBA_MON_MSG_ID_e;

typedef union /*_AMBA_MON_MESSAGE_HEADER_u_*/ {
    UINT64 Data[3];
    /* id msg */
    struct {
        UINT32 Id:             8;
        UINT32 Reserved:      24;
    } Com;
    /* link msg */
    struct {
        UINT32 Id:             8;
        UINT32 Reserved_L:    24;
        UINT32 Reserved_H:    32;
        UINT64 Pointer;
    } Link;
    /* event msg */
    struct {
        UINT32 Id:             8;
        UINT32 Reserved_L:    24;
        UINT32 Reserved_H:    32;
        UINT64 Reserved;
        UINT64 Flag;
    } Event;
    /* chunk msg */
    struct {
        UINT32 Id:             8;
        UINT32 Reserved_L:    14;
        UINT32 Size:           8;
        UINT32 Repeat:         1;
        UINT32 Timeout:        1;
        UINT32 Timetick:      32;
    } Chunk;
} AMBA_MON_MESSAGE_HEADER_u;

typedef struct /*_AMBA_MON_MESSAGE_HEADER_s_*/ {
    AMBA_MON_MESSAGE_HEADER_u    Ctx;
} AMBA_MON_MESSAGE_HEADER_s;

typedef struct /*_AMBA_MON_MESSAGE_WATCHDOG_s_*/ {
    UINT32 Period;
      void *pUser;
} AMBA_MON_MESSAGE_WATCHDOG_s;

typedef struct /*_AMBA_MON_MESSAGE_EVENT_s_*/ {
    UINT32  Id;
    UINT64  Flag;
} AMBA_MON_MESSAGE_EVENT_s;

typedef union /*_AMBA_MON_MESSAGE_CB_u_*/ {
    void                                *pVoid;
    struct AMBA_MON_MESSAGE_CB_ID_s_    *pId;
} AMBA_MON_MESSAGE_CB_u;

typedef struct /*_AMBA_MON_MESSAGE_CB_s_*/ {
    AMBA_MON_MESSAGE_CB_u    Ctx;
} AMBA_MON_MESSAGE_CB_s;

typedef void (*AMBA_MON_MESSAGE_CB_f)(void *pCbId, void *pMsg);

typedef struct /*_AMBA_MON_MESSAGE_CB_LINK_s_*/ {
    struct AMBA_MON_MESSAGE_CB_ID_s_    *Up;
    struct AMBA_MON_MESSAGE_CB_ID_s_    *Down;
} AMBA_MON_MESSAGE_CB_LINK_s;

typedef struct AMBA_MON_MESSAGE_CB_ID_s_ {
    UINT32                             Magic;
    AMBA_KAL_MUTEX_t                   *pMutex;
    struct AMBA_MON_MESSAGE_PORT_s_    *pSrcMsgPort;
    struct AMBA_MON_MESSAGE_PORT_s_    *pDesMsgPort;
    const char                         *pPutEventName;
    AMBA_MON_MESSAGE_CB_f              pFunc;
    AMBA_MON_MESSAGE_EVENT_s           PutEvent;
    AMBA_MON_MESSAGE_CB_LINK_s         Link;
} AMBA_MON_MESSAGE_CB_ID_s;

typedef struct /*_AMBA_MON_MESSAGE_PORT_LINK_s_*/ {
    struct AMBA_MON_MESSAGE_PORT_s_    *Up;
    struct AMBA_MON_MESSAGE_PORT_s_    *Down;
} AMBA_MON_MESSAGE_PORT_LINK_s;

typedef struct AMBA_MON_MESSAGE_PORT_s_ {
    UINT32                           Magic;
    AMBA_KAL_MUTEX_t                 Mutex;
    AMBA_MON_RING_PORT_s             RingPort;
    AMBA_MON_EVENT_PORT_s            EventPort;
    AMBA_MON_WATCHDOG_PORT_s         WatchdogPort;
    AMBA_MON_MESSAGE_WATCHDOG_s      Watchdog;
    const char                       *pPutEventName;
    AMBA_MON_MESSAGE_EVENT_s         PutEvent;
    AMBA_MON_MESSAGE_CB_ID_s         *pCbList;
    struct AMBA_MON_MESSAGE_ID_s_    *pMsgId;
    AMBA_MON_MESSAGE_PORT_LINK_s     Link;
} AMBA_MON_MESSAGE_PORT_s;

typedef AMBA_MON_RING_CHUNK_INFO_s AMBA_MON_MESSAGE_CHUNK_INFO_s;

typedef struct /*_AMBA_MON_MESSAGE_ID_LINK_s_*/ {
    struct AMBA_MON_MESSAGE_ID_s_    *Up;
    struct AMBA_MON_MESSAGE_ID_s_    *Down;
} AMBA_MON_MESSAGE_ID_LINK_s;

typedef struct AMBA_MON_MESSAGE_ID_s_ {
    UINT32                        Magic;
    AMBA_KAL_MUTEX_t              Mutex;
    const char                    *pName;
    AMBA_MON_RING_ID_s            RingId;
    AMBA_MON_EVENT_ID_s           EventId;
    AMBA_MON_WATCHDOG_ID_s        WatchdogId;
    AMBA_MON_MESSAGE_PORT_s       *pPortList;
    AMBA_MON_MESSAGE_ID_LINK_s    Link;
} AMBA_MON_MESSAGE_ID_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaMonMessage.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaMonMessage_Init(void);

UINT32 AmbaMonMessage_Create(AMBA_MON_MESSAGE_ID_s *pMsgId, const char *pName, const AMBA_MON_MESSAGE_CHUNK_INFO_s *pChunkInfo);
UINT32 AmbaMonMessage_Reset(AMBA_MON_MESSAGE_ID_s *pMsgId);
UINT32 AmbaMonMessage_Delete(AMBA_MON_MESSAGE_ID_s *pMsgId);

UINT32 AmbaMonMessage_Open(AMBA_MON_MESSAGE_PORT_s *pPort, const char *pName, const char *pPutEventName);
UINT32 AmbaMonMessage_PutCbRegister(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_MESSAGE_CB_ID_s *pCbId, AMBA_MON_MESSAGE_CB_f pFunc);
UINT32 AmbaMonMessage_PutCbUnregister(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_MESSAGE_CB_ID_s *pCbId);
 void *AmbaMonMessage_Put(AMBA_MON_MESSAGE_PORT_s *pPort, void *pData, UINT32 Size);
 void *AmbaMonMessage_Put2(AMBA_MON_MESSAGE_PORT_s *pPort, void *pHeader, UINT32 HSize, const void *pData, UINT32 DSize);
 void *AmbaMonMessage_PutEx(AMBA_MON_MESSAGE_PORT_s *pPort, void *pData, UINT32 Size, const AMBA_MON_MESSAGE_EVENT_s *pPutEvent);
UINT32 AmbaMonMessage_Get(AMBA_MON_MESSAGE_PORT_s *pPort, void **pData);
UINT32 AmbaMonMessage_RSeek(AMBA_MON_MESSAGE_PORT_s *pPort, UINT32 Offset, UINT32 Flag);
UINT32 AmbaMonMessage_Find(AMBA_MON_MESSAGE_PORT_s *pPort, const char *pFlagName, UINT32 *pId, UINT64 *pFlag);
UINT64 AmbaMonMessage_Wait(AMBA_MON_MESSAGE_PORT_s *pPort, UINT64 Flags, UINT32 Option, UINT32 Timeout);
UINT32 AmbaMonMessage_Close(AMBA_MON_MESSAGE_PORT_s *pPort);

UINT32 AmbaMonMessage_TimeoutSet(AMBA_MON_MESSAGE_PORT_s *pPort, UINT32 Period, void *pUser);
UINT32 AmbaMonMessage_TimeoutGet(AMBA_MON_MESSAGE_PORT_s **pPort, UINT32 *pPeriod, void **pUser);
UINT32 AmbaMonMessage_TimeoutRefresh(AMBA_MON_MESSAGE_PORT_s *pPort, UINT32 Period);

#endif /* AMBA_MON_MESSAGE_H */
