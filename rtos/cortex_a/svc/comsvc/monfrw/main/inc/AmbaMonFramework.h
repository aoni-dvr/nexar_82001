/**
 *  @file AmbaMonFramework.h
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
 *  @details Constants and Definitions for Amba Monitor Framework
 *
 */

#ifndef AMBA_MONITOR_FRW_H
#define AMBA_MONITOR_FRW_H

#ifndef AMBA_MON_FRW_LIB_COMSVC

#if 0
/*-----------------------------------------------------------------------------------------------*\
 * AmbaMonRing
\*-----------------------------------------------------------------------------------------------*/
#define MON_RING_RESET           2U
#define MON_RING_OVERFLOW        3U

typedef enum /*_AMBA_MON_RING_RSEEK_FLAG_e_*/ {
    R_SEEK_PASS_BEGIN = 0,
    R_SEEK_PASS_LAST
} AMBA_MON_RING_RSEEK_FLAG_e;

typedef struct /*_AMBA_MON_RING_INDEX_s_*/ {
    UINT32 Rd;
    UINT32 Wr;
} AMBA_MON_RING_INDEX_s;

typedef union /*_AMBA_MON_RING_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Reset:        1;
        UINT32 Overflow:     1;
        UINT32 Reserved:    30;
    } Bits;
} AMBA_MON_RING_OP_u;

typedef struct /*_AMBA_MON_RING_CHUNK_INFO_s_*/ {
    UINT32 Num;
    UINT32 Size;
    UINT32 AuxInNum;
      void *pMemBase;
} AMBA_MON_RING_CHUNK_INFO_s;

typedef struct /*_AMBA_MON_RING_BUF_s_*/ {
    AMBA_MON_RING_CHUNK_INFO_s    ChunkInfo;
} AMBA_MON_RING_BUF_s;

typedef struct /*_AMBA_MON_RING_PORT_LINK_s_*/ {
    struct AMBA_MON_RING_PORT_s_    *Up;
    struct AMBA_MON_RING_PORT_s_    *Down;
} AMBA_MON_RING_PORT_LINK_s;

typedef struct AMBA_MON_RING_PORT_s_ {
    UINT32                        Magic;
    AMBA_KAL_MUTEX_t              Mutex;
    AMBA_MON_RING_OP_u            Op;
    AMBA_MON_RING_INDEX_s         Index;
    struct AMBA_MON_RING_ID_s_    *pRingId;
    AMBA_MON_RING_PORT_LINK_s     Link;
} AMBA_MON_RING_PORT_s;

typedef struct /*_AMBA_MON_RING_LINK_s_*/ {
    struct AMBA_MON_RING_ID_s_    *Up;
    struct AMBA_MON_RING_ID_s_    *Down;
} AMBA_MON_RING_ID_LINK_s;

typedef struct AMBA_MON_RING_ID_s_ {
    UINT32                     Magic;
    AMBA_KAL_MUTEX_t           Mutex;
    const char                 *pName;
    AMBA_MON_RING_OP_u         Op;
    AMBA_MON_RING_INDEX_s      Index;
    AMBA_MON_RING_BUF_s        Buf;
    AMBA_MON_RING_PORT_s       *pPortList;
    AMBA_MON_RING_ID_LINK_s    Link;
} AMBA_MON_RING_ID_s;

/*-----------------------------------------------------------------------------------------------*\
 * SafetyEvent
\*-----------------------------------------------------------------------------------------------*/
#define MON_EVENT_FLAG_OR_CLR       0
#define MON_EVENT_FLAG_AND_CLR      1

typedef union /*_AMBA_MON_EVENT_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 AndOrClr:     1;
        UINT32 Reserved:    31;
    } Bits;
} AMBA_MON_EVENT_OP_u;

typedef struct /*_AMBA_MON_EVENT_OP_s_*/ {
    AMBA_MON_EVENT_OP_u    Ctx;
} AMBA_MON_EVENT_OP_s;

typedef struct /*_AMBA_MON_EVENT_SEM_s_*/ {
    AMBA_KAL_SEMAPHORE_t    Req;
    AMBA_KAL_SEMAPHORE_t    Ack;
} AMBA_MON_EVENT_SEM_s;

typedef struct /*_AMBA_MON_EVENT_PORT_FLAG_s_*/ {
    UINT64  Req;
    UINT64  Acc;
    UINT64  Ret;
} AMBA_MON_EVENT_PORT_FLAG_s;

typedef struct /*_AMBA_MON_EVENT_PORT_LINK_s_*/ {
    struct AMBA_MON_EVENT_PORT_s_    *Up;
    struct AMBA_MON_EVENT_PORT_s_    *Down;
} AMBA_MON_EVENT_PORT_LINK_s;

typedef struct AMBA_MON_EVENT_PORT_s_ {
    UINT32                         Magic;
    AMBA_KAL_MUTEX_t               Mutex;
    AMBA_MON_EVENT_OP_u            Op;
    AMBA_MON_EVENT_SEM_s           Sem;
    AMBA_MON_EVENT_PORT_FLAG_s     Flags;
    struct AMBA_MON_EVENT_ID_s_    *pEventId;
    AMBA_MON_EVENT_PORT_LINK_s     Link;
} AMBA_MON_EVENT_PORT_s;

typedef struct /*_AMBA_MON_EVENT_FLAG_s_*/ {
    UINT64      BitAlloc;
    UINT32      BitCnt[64];
    const char  *pNameTable[64];
} AMBA_MON_EVENT_FLAG_s;

typedef struct /*_AMBA_MON_EVENT_ID_LINK_s_*/ {
    struct AMBA_MON_EVENT_ID_s_    *Up;
    struct AMBA_MON_EVENT_ID_s_    *Down;
} AMBA_MON_EVENT_ID_LINK_s;

typedef struct AMBA_MON_EVENT_ID_s_ {
    UINT32                      Magic;
    AMBA_KAL_MUTEX_t            Mutex;
    const char                  *pName;
    AMBA_MON_EVENT_FLAG_s       Flags;
    AMBA_MON_EVENT_PORT_s       *pPortList;
    AMBA_MON_EVENT_ID_LINK_s    Link;
} AMBA_MON_EVENT_ID_s;

/*-----------------------------------------------------------------------------------------------*\
 * SafetyWatchdog
\*-----------------------------------------------------------------------------------------------*/
typedef struct /*_AMBA_MON_WATCHDOG_PORT_LINK_s_*/ {
    struct AMBA_MON_WATCHDOG_PORT_s_    *Up;
    struct AMBA_MON_WATCHDOG_PORT_s_    *Down;
} AMBA_MON_WATCHDOG_PORT_LINK_s;

typedef struct AMBA_MON_WATCHDOG_PORT_s_ {
    UINT32                            Magic;
    AMBA_KAL_MUTEX_t                  Mutex;
    UINT32                            Period;
    UINT32                            Time2Wait;
    void                              *pUser;
    struct AMBA_MON_WATCHDOG_ID_s_    *pWdogId;
    AMBA_MON_WATCHDOG_PORT_LINK_s     WaitLink;
    AMBA_MON_WATCHDOG_PORT_LINK_s     Link;
} AMBA_MON_WATCHDOG_PORT_s;

typedef struct /*_AMBA_MON_WATCHDOG_ID_LINK_s_*/ {
    struct AMBA_MON_WATCHDOG_ID_s_    *Up;
    struct AMBA_MON_WATCHDOG_ID_s_    *Down;
} AMBA_MON_WATCHDOG_ID_LINK_s;

typedef struct AMBA_MON_WATCHDOG_ID_s_ {
    UINT32                         Magic;
    AMBA_KAL_MUTEX_t               Mutex;
    const char                     *pName;
    AMBA_MON_WATCHDOG_PORT_s       *pPortList;
    AMBA_MON_WATCHDOG_ID_LINK_s    Link;
} AMBA_MON_WATCHDOG_ID_s;

/*-----------------------------------------------------------------------------------------------*\
 * SafetyMessage
\*-----------------------------------------------------------------------------------------------*/
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
        UINT32 Reserved:      14;
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
 * AmbaSafetyListen
\*-----------------------------------------------------------------------------------------------*/
typedef AMBA_MON_MESSAGE_CHUNK_INFO_s AMBA_MON_LISTEN_CHUNK_INFO_s;

typedef struct /*_AMBA_MON_LISTEN_PORT_LINK_s_*/ {
    struct AMBA_MON_LISTEN_PORT_s_    *Up;
    struct AMBA_MON_LISTEN_PORT_s_    *Down;
} AMBA_MON_LISTEN_PORT_LINK_s;

typedef struct AMBA_MON_LISTEN_PORT_s_ {
    UINT32                           Magic;
    AMBA_KAL_MUTEX_t                 Mutex;
    AMBA_MON_MESSAGE_PORT_s          MsgPort;
    struct AMBA_MON_LISTEN_ID_s_    *pListenId;
    AMBA_MON_LISTEN_PORT_LINK_s      Link;
} AMBA_MON_LISTEN_PORT_s;

typedef struct /*_AMBA_MON_LISTEN_ID_LINK_s_*/ {
    struct AMBA_MON_LISTEN_ID_s_    *Up;
    struct AMBA_MON_LISTEN_ID_s_    *Down;
} AMBA_MON_LISTEN_ID_LINK_s;

typedef struct AMBA_MON_LISTEN_ID_s_ {
    UINT32                           Magic;
    AMBA_KAL_MUTEX_t                 Mutex;
    const char                       *pName;
    AMBA_MON_MESSAGE_ID_s            MsgId;
    AMBA_MON_LISTEN_PORT_s           *pPortList;
    AMBA_MON_LISTEN_ID_LINK_s        Link;
} AMBA_MON_LISTEN_ID_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaMonRing.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaMonRing_Init(void);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaMonEvent.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaMonEvent_Init(void);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaMonWatchdog.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaMonWatchdog_Init(void);

 INT32 AmbaMonWatchdog_TimeoutGet(AMBA_MON_WATCHDOG_PORT_s **pTimeoutPort, UINT32 *pTimeoutPeriod, void **pUser);
 INT32 AmbaMonWatchdog_TimeoutRestart(void);

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

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaMonListen.c
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

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaMonVar.c
\*-----------------------------------------------------------------------------------------------*/
typedef enum /*_AMBA_MON_VAR_LEADING_e_*/ {
    SVAR_LEADING_NONE = 0,
    SVAR_LEADING_ZERO,
    SVAR_LEADING_SPACE
} AMBA_MON_VAR_LEADING_e;

  void svar_utoa(UINT32 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag);
  void svar_itoa( INT32 Value, char *pString, UINT32 Base, UINT32 Count, UINT32 Flag);
UINT32 svar_strcat(char *pString1, const char *pString2);

  void svar_print(const char *pFmt, const UINT32 *Argc, const char * const *Argv);

  void AmbaMonPrint_Enable(UINT32 Flag);

  void AmbaMonPrintEx(UINT32 Id, const char *pStr, UINT32 var_ul, UINT32 var_base);
  void AmbaMonPrint(UINT32 Id, const char *pStr, UINT32 var_ul);
  void AmbaMonPrintStr(UINT32 Id, const char *pStr);
  void AmbaMonPrintEx2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var0_base, UINT32 var1_ul, UINT32 var1_base);
  void AmbaMonPrint2(UINT32 Id, const char *pStr, UINT32 var0_ul, UINT32 var1_ul);
#else
#include "../../src/inc/AmbaMonRing.h"
#include "../../src/inc/AmbaMonEvent.h"
#include "../../src/inc/AmbaMonWatchdog.h"
#include "../../src/inc/AmbaMonMessage.h"
#include "../../src/inc/AmbaMonListen.h"
#include "../../src/inc/AmbaMonVar.h"
#endif

#else

#include "AmbaMonRing.h"
#include "AmbaMonEvent.h"
#include "AmbaMonWatchdog.h"
#include "AmbaMonMessage.h"
#include "AmbaMonListen.h"
#include "AmbaMonVar.h"

#endif

typedef enum /*_AMBA_MON_VAR_MSG_ID_e_*/ {
    S_PRINT_ID_DBG = 0,
    S_PRINT_ID_MSG,
    S_PRINT_ID_ERR,
    S_PRINT_ID_STATE,
    S_PRINT_ID_WDOG,
    S_PRINT_ID_REPEAT
} AMBA_MON_VAR_MSG_ID_e;

#define S_PRINT_FLAG_DBG    (((UINT32) 1U) << (UINT32) S_PRINT_ID_DBG)
#define S_PRINT_FLAG_MSG    (((UINT32) 1U) << (UINT32) S_PRINT_ID_MSG)
#define S_PRINT_FLAG_ERR    (((UINT32) 1U) << (UINT32) S_PRINT_ID_ERR)
#define S_PRINT_FLAG_STATE  (((UINT32) 1U) << (UINT32) S_PRINT_ID_STATE)
#define S_PRINT_FLAG_WDOG   (((UINT32) 1U) << (UINT32) S_PRINT_ID_WDOG)
#define S_PRINT_FLAG_REPEAT (((UINT32) 1U) << (UINT32) S_PRINT_ID_REPEAT)

#endif  /* AMBA_MONITOR_FRW_H */
