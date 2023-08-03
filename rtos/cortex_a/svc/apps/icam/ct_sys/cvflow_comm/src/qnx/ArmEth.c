/**
*  @file ArmEth.c
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
*   @details The Arm Ethernet access utility
*
*/

#include <stdio.h>
#include <stdlib.h>         /* for exit() */
#include <string.h>         /* for memset(), strlen() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>     /* Internet address family, struct sockaddr_in */
#include <netinet/tcp.h>    /* TCP protocol */
#include <signal.h>         /* for signal() */
#include <sys/time.h>       /* for gettimeofday() */

/* Arm header */
#include "ArmLog.h"
#include "ArmErrCode.h"
#include "ArmEth.h"
#include "ArmTask.h"
#include "ArmMutex.h"
#include "ArmMsgQueue.h"

/*---------------------------------------------------------------------------*\
 * Socket Port Configuration
\*---------------------------------------------------------------------------*/
typedef struct _XPCB {
  INT32       Flags;
  INT32       Fd;
  UINT32      SeqNum;
  ArmMutex_t  Lock;
} XPCB_s;

typedef struct _ETH_CH {
  XPCB_s *pTxCh;
  XPCB_s *pRxCh;
} ETH_CH_s;

typedef struct _ETH_MQ_ITEM {
 UINT32 Ch;
} ETH_MQ_ITEM_s;

#define ARM_LOG_ETH         "ArmUtil_ETH"
#define MAX_CHANNEL         64
#define MAX_CONNECTIONS     (MAX_CHANNEL << 1)
#define MAX_MSQ_NUM         2
#define SOCKET_PORT         8888
#define TXRX_TEST_SIZE      (10 * 1024 * 1024)
#define MAX_HEADER_SIZE     (10)
#define LOCK()              ArmMutex_Take(&g_Mutex)
#define UNLOCK()            ArmMutex_Give(&g_Mutex)
#define GETTIMESTAMP(s, e)  ((e.tv_usec - s.tv_usec) + (e.tv_sec - s.tv_sec)*1000000)

static INT32            g_ServerFD;
static ArmMutex_t       g_Mutex;
static ArmMsgQueue_t    g_MsgQ;
static XPCB_s           g_Connection[MAX_CONNECTIONS];
static ETH_CH_s         g_Channel[MAX_CHANNEL];

static const char HEADER_TX_TEST[]    = "+TEST";
static const char HEADER_SIZE_INFO[]  = "+SIZE";
static const char HEADER_TIME_STAMP[] = "+TIME";
static const char HEADER_DATA[]       = "+DATA";


/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/

static void SigHandler(INT32 SigNo)
{
  switch (SigNo) {
    case SIGTERM:
    case SIGINT:
      ArmLog_WARN(ARM_LOG_ETH, "Closing socket ...", 0U, 0U);
      close(g_ServerFD);
      ArmLog_WARN(ARM_LOG_ETH, "Exit", 0U, 0U);
      exit(0);
      break;
    default:
      break;
  }
}

static XPCB_s* GetFreeXPCB(void)
{
  UINT32 i;
  XPCB_s *Conn = NULL;
  char MutexName[] = "XPCB_Mut";

  LOCK();
  for (i = 0; i < MAX_CONNECTIONS; i++) {
    if (g_Connection[i].Flags == 0) {
      Conn = &g_Connection[i];
      Conn->Flags = 1;
      Conn->SeqNum = 0;
      ArmMutex_Create(&Conn->Lock, MutexName);
      break;
    }
  }
  UNLOCK();

  return Conn;
}

static ArmTaskRet_t ServerEntry(ArmTaskEntryArg_t EntryArg)
{
  INT32 Rval;
  UINT32 *pPort = (UINT32 *)EntryArg;
  struct sockaddr_in Addr = {0}, Cli = {0};
  socklen_t Len = sizeof(Cli);
  ETH_MQ_ITEM_s MQItem = {0};
  UINT32 Num = 0U;

  ArmLog_DBG(ARM_LOG_ETH, "ServerEntry() port %u", *pPort, 0U);
  signal(SIGTERM, SigHandler);
  signal(SIGINT,  SigHandler);

  g_ServerFD = socket(AF_INET, SOCK_STREAM, 0);

  Addr.sin_family      = AF_INET;
  Addr.sin_addr.s_addr = INADDR_ANY;
  Addr.sin_port        = htons(*pPort);

  Rval = bind(g_ServerFD, (const struct sockaddr*)&Addr, sizeof(Addr));
  if (Rval < 0) {
    ArmLog_ERR(ARM_LOG_ETH, "Can't bind to port %u", *pPort, 0U);
    exit(-1);
  }

  Rval = listen(g_ServerFD, MAX_CONNECTIONS);
  if (Rval < 0) {
    ArmLog_ERR(ARM_LOG_ETH, "Can't listen to port %u", *pPort, 0U);
    exit(-1);
  }

  ArmLog_DBG(ARM_LOG_ETH, "Listening on port %u ...\n", *pPort, 0U);
  while (1) {
    /* Establish connection for TX */
    XPCB_s *Conn = GetFreeXPCB();
    Conn->Fd = accept(g_ServerFD, (struct sockaddr *)&Cli, &Len);
    g_Channel[Num].pTxCh = Conn;
    ArmTask_Sleep(50);

    /* Establish connection for RX */
    Conn = GetFreeXPCB();
    Conn->Fd = accept(g_ServerFD, (struct sockaddr *)&Cli, &Len);
    g_Channel[Num].pRxCh = Conn;
    ArmTask_Sleep(50);

    /* Notify TX/RX Channels are ready */
    MQItem.Ch = Num;
    ArmMsgQueue_Send(&g_MsgQ, (void *)&MQItem);
    Num++;
  }

  return ARM_TASK_NULL_RET;
}

static UINT32 XportInit(UINT32 *pPort)
{
  UINT32 Rval = ARM_OK;
  static ArmTask_t ServerThread;
  static ETH_MQ_ITEM_s MsgQBuf[MAX_MSQ_NUM];
  char ServerName[] = "TCP Server";
  char MutexName[]  = "ETH Lock";
  char MsgQName[]   = "ETH Queue";
  ARM_TASK_CREATE_ARG_s TskArg = {0};

  ArmLog_DBG(ARM_LOG_ETH, "XportInit()", 0U, 0U);

  Rval = ArmMutex_Create(&g_Mutex, MutexName);

  if (Rval == ARM_OK) {
    Rval = ArmMsgQueue_Create(&g_MsgQ, MsgQName, sizeof(ETH_MQ_ITEM_s), MAX_MSQ_NUM, (void *)MsgQBuf);
  }

  if (Rval == ARM_OK) {
    TskArg.EntryFunction = ServerEntry;
    TskArg.EntryArg      = (ArmTaskEntryArg_t)pPort;
    Rval = ArmTask_Create(&ServerThread, ServerName, &TskArg);
  }

  return Rval;
}

static inline UINT32 XportSend(XPCB_s *pXport, const char *pBuff, UINT32 Size)
{
  ArmMutex_Take(&pXport->Lock);
  {
    UINT32 Pos, Bytes, Flag = 1;
    for (Pos = 0;  Pos < Size; Pos += Bytes) {
      Bytes = send(pXport->Fd, &pBuff[Pos], Size - Pos, 0);
      if (Bytes <= 0) {
        Size = 0;
        break;
      }
    }
    setsockopt(pXport->Fd, IPPROTO_TCP, TCP_NODELAY, (char*)&Flag, 4);
  }
  ArmMutex_Give(&pXport->Lock);

  return Size;
}

static inline UINT32 XportRecv(XPCB_s *pXport, char *pBuff, UINT32 Size)
{
  ArmMutex_Take(&pXport->Lock);
  {
    INT32 Pos, Bytes;
    for (Pos = 0; Pos < Size; Pos += Bytes) {
      Bytes = recv(pXport->Fd, &pBuff[Pos], Size - Pos, 0);
      if (Bytes <= 0) {
        Size = 0;
        break;
      }
    }
  }
  ArmMutex_Give(&pXport->Lock);

  return Size;
}

//static inline void XportUpdateSeqNum(XPCB_s *pXport)
//static inline UINT32 XportGetSeqNum(XPCB_s *pXport)

/*---------------------------------------------------------------------------*\
 * API
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEth_TxRxTest
 *
 *  @Description:: Tx/Rx throughput test
 *
 *  @Input      ::
 *    Ch:          The channel id
 *    pBuff:       The test buffer
 *    Size:        The test buffer size
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEth_TxRxTest(UINT32 Ch, char *pBuff, UINT32 Size)
{
  UINT32 Rval = ARM_OK;
  UINT32 TestSize = TXRX_TEST_SIZE/2; // 5MB
  XPCB_s *pTxCh, *pRxCh;
  struct timeval  tv_start[2], tv_end[2];

  /* 1. Sanity check for input parameters */
  if ((Ch >= MAX_CHANNEL) || (NULL == pBuff)) {
    ArmLog_ERR(ARM_LOG_ETH, "## ArmEth_TxRxTest() sanity check (Ch:%u >= %u?)", Ch, MAX_CHANNEL);
    Rval = ARM_NG;
  }

  if (ARM_OK == Rval) {
    if (Size < TXRX_TEST_SIZE) {
      ArmLog_ERR(ARM_LOG_ETH, "## Test buffer requirement: %u(%u)", TXRX_TEST_SIZE, Size);
      Rval = ARM_NG;
    }
  }

  if (ARM_OK == Rval) {
    pTxCh = g_Channel[Ch].pTxCh;
    pRxCh = g_Channel[Ch].pRxCh;
    if ((0 == pTxCh->Flags) || (0 == pRxCh->Flags)) {
      ArmLog_ERR(ARM_LOG_ETH, "## TX/RX flags: %d, %d", pTxCh->Flags, pRxCh->Flags);
      Rval = ARM_NG;
    }
  }

  /* 2. TX Throughput test */
  if (ARM_OK == Rval) {
    UINT32 TxSize;

    /* 2.1 Send Header */
    TxSize = XportSend(pTxCh, HEADER_TX_TEST, strlen(HEADER_TX_TEST));
    if (TxSize != strlen(HEADER_TX_TEST)) {
      ArmLog_ERR(ARM_LOG_ETH, "## TxSize != HEADER_TX_TEST Size (%u, %u)", TxSize, strlen(HEADER_TX_TEST));
      Rval = ARM_NG;
    }
  }

  if (ARM_OK == Rval) {
    UINT32 TxSize;

    /* 2.2 Send Data */
    memset((void *)&pBuff[0], 0xAB, TestSize); // Send 0xAB

    gettimeofday(&tv_start[0], NULL);
    TxSize = XportSend(pTxCh, &pBuff[0], TestSize);
    gettimeofday(&tv_end[0], NULL);
    if (TxSize != TestSize) {
      ArmLog_ERR(ARM_LOG_ETH, "## TxSize != TestSize (%u, %u)", TxSize, TestSize);
      Rval = ARM_NG;
    }
  }

  /* 3. RX Throughput test */
  if (ARM_OK == Rval) {
    UINT32 RxSize;
    char HeaderBuf[MAX_HEADER_SIZE] = {0};

    /* 3.1 Receive Header, '+TEST' */
    RxSize = XportRecv(pRxCh, HeaderBuf, strlen(HEADER_TX_TEST));
    if (0 == RxSize) {
      ArmLog_ERR(ARM_LOG_ETH, "## XportRecv() fail", 0U, 0U);
      Rval = ARM_NG;
    }

    if (ARM_OK == Rval){
      INT32 Ret;
      Ret = memcmp((void *)HeaderBuf, (void *)HEADER_TX_TEST, strlen(HEADER_TX_TEST));
      if (0 != Ret) {
        ArmLog_ERR(ARM_LOG_ETH, "## Header check fail", 0U, 0U);
        ArmLog_STR(ARM_LOG_ETH, "## Header %s != %s", HeaderBuf, HEADER_TX_TEST);
        Rval = ARM_NG;
      }
    }
  }

  if (ARM_OK == Rval) {
    UINT32 RxSize;
    memset((void *)&pBuff[0], 0x00, TestSize); // Reset buffer
    memset((void *)&pBuff[TestSize], 0xCD, TestSize); // RX comparing

    /* 3.2 Receive Data */
    gettimeofday(&tv_start[1], NULL);
    RxSize = XportRecv(pRxCh, &pBuff[0], TestSize);
    gettimeofday(&tv_end[1], NULL);
    if (0 == RxSize) {
      ArmLog_ERR(ARM_LOG_ETH, "## XportRecv() fail", 0U, 0U);
      Rval = ARM_NG;
    }

    if (ARM_OK == Rval){
      INT32 Ret;
      Ret = memcmp((void *)&pBuff[0], (void *)&pBuff[TestSize], TestSize);
      if (0 != Ret) {
        ArmLog_ERR(ARM_LOG_ETH, "## XportRecv() data is incorrect!", 0U, 0U);
        Rval = ARM_NG;
      }
    }
  }

  /* 4. Show TX/RX throughput */
  if (ARM_OK == Rval) {
    UINT32 Mbps;
    Mbps = (TestSize * 8) / (GETTIMESTAMP(tv_start[0],tv_end[0]));
    ArmLog_DBG(ARM_LOG_ETH, "", 0U, 0U);
    if (Mbps > 500) {
      ArmLog_DBG(ARM_LOG_ETH, "TX: Connection okay.", 0U, 0U);
    } else {
      ArmLog_WARN(ARM_LOG_ETH, "Poor speed!! TX: %uMbps", Mbps, 0U);
    }
    Mbps = (TestSize * 8) / (GETTIMESTAMP(tv_start[1],tv_end[1]));
    if (Mbps > 500) {
      ArmLog_DBG(ARM_LOG_ETH, "RX: Connection okay.", 0U, 0U);
    } else {
      ArmLog_WARN(ARM_LOG_ETH, "Poor speed!! RX: %uMbps", Mbps, 0U);
    }
    ArmLog_DBG(ARM_LOG_ETH, "", 0U, 0U);
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEth_GetSize
 *
 *  @Description:: Get size info from remote PC
 *
 *  @Input      ::
 *    Ch:          The channel id
 *
 *  @Output     ::
 *    pSizeInfo:   The data size information
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEth_GetSize(UINT32 Ch, ARM_ETH_SIZE_INFO_s *pSizeInfo)
{
  UINT32 Rval = ARM_OK;
  XPCB_s *pRxCh;

  /* 1. Sanity check for input parameters */
  if ((Ch >= MAX_CHANNEL) || (NULL == pSizeInfo)) {
    ArmLog_ERR(ARM_LOG_ETH, "## ArmEth_GetSize() sanity check (Ch:%u >= %u?)", Ch, MAX_CHANNEL);
    Rval = ARM_NG;
  }

  if (ARM_OK == Rval) {
    pRxCh = g_Channel[Ch].pRxCh;
    if (0 == pRxCh->Flags) {
      ArmLog_ERR(ARM_LOG_ETH, "## RX flags: %d", pRxCh->Flags, 0U);
      Rval = ARM_NG;
    }
  }

  /* 2. Receive Header, '+SIZE' */
  if (ARM_OK == Rval) {
    UINT32 RxSize;
    char HeaderBuf[MAX_HEADER_SIZE] = {0};

    RxSize = XportRecv(pRxCh, HeaderBuf, strlen(HEADER_SIZE_INFO));
    if (0 == RxSize) {
      ArmLog_ERR(ARM_LOG_ETH, "## XportRecv() fail", 0U, 0U);
      Rval = ARM_NG;
    }

    if (ARM_OK == Rval){
      INT32 Ret;
      Ret = memcmp((void *)HeaderBuf, (void *)HEADER_SIZE_INFO, strlen(HEADER_SIZE_INFO));
      if (0 != Ret) {
        ArmLog_ERR(ARM_LOG_ETH, "## Header check fail", 0U, 0U);
        ArmLog_STR(ARM_LOG_ETH, "## Header %s != %s", HeaderBuf, HEADER_SIZE_INFO);
        Rval = ARM_NG;
      }
    }
  }

  /* 3. Receive Data, size information */
  if (ARM_OK == Rval) {
    UINT32 RxSize;
    char *pBuf = (char *)pSizeInfo;

    RxSize = XportRecv(pRxCh, pBuf, sizeof(ARM_ETH_SIZE_INFO_s));

    if (0 == RxSize) {
      ArmLog_ERR(ARM_LOG_ETH, "## XportRecv() fail", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEth_Recv
 *
 *  @Description:: Recive data from remote PC
 *
 *  @Input      ::
 *    Ch:          The channel id
 *    pSizeInfo:   The data size information
 *    pDataInfo:   The data information
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEth_Recv(UINT32 Ch, const ARM_ETH_SIZE_INFO_s *pSizeInfo, ARM_ETH_DATA_INFO_s *pDataInfo)
{
  UINT32 Rval = ARM_OK;
  UINT32 i;
  XPCB_s *pRxCh;

  /* 1. Sanity check for input parameters */
  if ((Ch >= MAX_CHANNEL) || (NULL == pDataInfo)) {
    ArmLog_ERR(ARM_LOG_ETH, "## ArmEth_Recv() sanity check (Ch:%u >= %u?)", Ch, MAX_CHANNEL);
    Rval = ARM_NG;
  }

  if (NULL == pSizeInfo) {
    ArmLog_ERR(ARM_LOG_ETH, "## pSizeInfo is Null!", 0U, 0U);
    Rval = ARM_NG;
  }

  if (ARM_OK == Rval) {
    if ((pSizeInfo->Num > ARM_ETH_MAX_IO) || (pSizeInfo->Num == 0)) {
      ArmLog_ERR(ARM_LOG_ETH, "## SizeInfo Num: %u", pSizeInfo->Num, 0U);
      Rval = ARM_NG;
    }
  }

  if (ARM_OK == Rval) {
    pRxCh = g_Channel[Ch].pRxCh;
    if (0 == pRxCh->Flags) {
      ArmLog_ERR(ARM_LOG_ETH, "## RX flags: %d", pRxCh->Flags, 0U);
      Rval = ARM_NG;
    }
  }

  /* 2. Receive time stamp */
  if (ARM_OK == Rval) {
    UINT32 RxSize;
    char HeaderBuf[MAX_HEADER_SIZE] = {0};

    /* Header, '+TIME' */
    RxSize = XportRecv(pRxCh, HeaderBuf, strlen(HEADER_TIME_STAMP));
    if (0 == RxSize) {
      ArmLog_ERR(ARM_LOG_ETH, "## XportRecv() fail", 0U, 0U);
      Rval = ARM_NG;
    }

    if (ARM_OK == Rval){
      INT32 Ret;
      Ret = memcmp((void *)HeaderBuf, (void *)HEADER_TIME_STAMP, strlen(HEADER_TIME_STAMP));
      if (0 != Ret) {
        ArmLog_ERR(ARM_LOG_ETH, "## Header check fail", 0U, 0U);
        ArmLog_STR(ARM_LOG_ETH, "## Header %s != %s", HeaderBuf, HEADER_TIME_STAMP);
        Rval = ARM_NG;
      }
    }
  }

  if (ARM_OK == Rval) {
    UINT32 RxSize;
    char *pBuf = (char *)(&(pDataInfo->SeqNum));

    /* SeqNum */
    RxSize = XportRecv(pRxCh, pBuf, sizeof(pDataInfo->SeqNum));
    /* TimeStamp */
    pBuf = (char *)(&(pDataInfo->TimeStamp));
    RxSize += XportRecv(pRxCh, pBuf, sizeof(pDataInfo->TimeStamp));

    if ((sizeof(pDataInfo->SeqNum) + sizeof(pDataInfo->TimeStamp)) != RxSize) {
      ArmLog_ERR(ARM_LOG_ETH, "## XportRecv() fail", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  /* 3. Receive data */
  if (ARM_OK == Rval) {
    for (i = 0; i < pSizeInfo->Num; i++) {

      /* Header, '+DATA' */
      if (ARM_OK == Rval) {
        UINT32 RxSize;
        char HeaderBuf[MAX_HEADER_SIZE] = {0};

        RxSize = XportRecv(pRxCh, HeaderBuf, strlen(HEADER_DATA));
        if (0 == RxSize) {
          ArmLog_ERR(ARM_LOG_ETH, "## XportRecv() fail", 0U, 0U);
          Rval = ARM_NG;
        }

        if (ARM_OK == Rval){
          INT32 Ret;
          Ret = memcmp((void *)HeaderBuf, (void *)HEADER_DATA, strlen(HEADER_DATA));
          if (0 != Ret) {
            ArmLog_ERR(ARM_LOG_ETH, "## Header check fail", 0U, 0U);
            ArmLog_STR(ARM_LOG_ETH, "## Header %s != %s", HeaderBuf, HEADER_DATA);
            Rval = ARM_NG;
          }
        }
      }

      /* Data */
      if (ARM_OK == Rval) {
        UINT32 RxSize;
        char *pBuf = pDataInfo->pBuf[i];

        RxSize = XportRecv(pRxCh, pBuf, pSizeInfo->Size[i]);

        if (0 == RxSize) {
          ArmLog_ERR(ARM_LOG_ETH, "## XportRecv() fail", 0U, 0U);
          Rval = ARM_NG;
        }
      }

      /* Check RX status */
      if (ARM_NG == Rval) {
        break;
      }

    }
  }
  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEth_Send
 *
 *  @Description:: Send size info and data to remote PC
 *
 *  @Input      ::
 *    Ch:          The channel id
 *    pSizeInfo:   The data size information
 *    pDataInfo:   The data information
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEth_Send(UINT32 Ch, const ARM_ETH_SIZE_INFO_s *pSizeInfo, const ARM_ETH_DATA_INFO_s *pDataInfo)
{
  UINT32 Rval = ARM_OK;
  UINT32 i;
  XPCB_s *pTxCh;

  /* 1. Sanity check for input parameters */
  if ((Ch >= MAX_CHANNEL) || (NULL == pDataInfo)) {
    ArmLog_ERR(ARM_LOG_ETH, "## ArmEth_Send() sanity check (Ch:%u >= %u?)", Ch, MAX_CHANNEL);
    Rval = ARM_NG;
  }

  if (NULL == pSizeInfo) {
    ArmLog_ERR(ARM_LOG_ETH, "## pSizeInfo is Null!", 0U, 0U);
    Rval = ARM_NG;
  }

  if (ARM_OK == Rval) {
    if ((pSizeInfo->Num > ARM_ETH_MAX_IO) || (pSizeInfo->Num == 0)) {
      ArmLog_ERR(ARM_LOG_ETH, "## SizeInfo Num: %u", pSizeInfo->Num, 0U);
      Rval = ARM_NG;
    }
  }

  if (ARM_OK == Rval) {
    pTxCh = g_Channel[Ch].pTxCh;
    if (0 == pTxCh->Flags) {
      ArmLog_ERR(ARM_LOG_ETH, "## TX flags: %d", pTxCh->Flags, 0U);
      Rval = ARM_NG;
    }
  }

  /* 2. Send Size Info */
  if (ARM_OK == Rval) {
    UINT32 TxSize;

    /* Header, '+SIZE' */
    TxSize = XportSend(pTxCh, HEADER_SIZE_INFO, strlen(HEADER_SIZE_INFO));
    if (TxSize != strlen(HEADER_SIZE_INFO)) {
      ArmLog_ERR(ARM_LOG_ETH, "## TxSize != HEADER_SIZE_INFO Size (%u, %u)", TxSize, strlen(HEADER_SIZE_INFO));
      Rval = ARM_NG;
    }
  }

  if (ARM_OK == Rval) {
    UINT32 TxSize;
    char *pBuf = (char *)pSizeInfo;

    /* Size Info */
    TxSize = XportSend(pTxCh, pBuf, sizeof(ARM_ETH_SIZE_INFO_s));
    if (TxSize != sizeof(ARM_ETH_SIZE_INFO_s)) {
      ArmLog_ERR(ARM_LOG_ETH, "## TxSize != SizeInfo Size (%u, %u)", TxSize, sizeof(ARM_ETH_SIZE_INFO_s));
      Rval = ARM_NG;
    }
  }

  /* 3. Send Time Stamp */
  if (ARM_OK == Rval) {
    UINT32 TxSize;

    /* Header, '+TIME' */
    TxSize = XportSend(pTxCh, HEADER_TIME_STAMP, strlen(HEADER_TIME_STAMP));
    if (TxSize != strlen(HEADER_TIME_STAMP)) {
      ArmLog_ERR(ARM_LOG_ETH, "## TxSize != HEADER_TIME_STAMP Size (%u, %u)", TxSize, strlen(HEADER_TIME_STAMP));
      Rval = ARM_NG;
    }
  }

  if (ARM_OK == Rval) {
    UINT32 TxSize;
    char *pBuf = (char *)(&(pTxCh->SeqNum));

    /* SeqNum */
    TxSize = XportSend(pTxCh, pBuf, sizeof(pTxCh->SeqNum));
    pTxCh->SeqNum++;
    /* TimeStamp */
    pBuf = (char *)(&(pDataInfo->TimeStamp));
    TxSize += XportSend(pTxCh, pBuf, sizeof(pDataInfo->TimeStamp));

    if (TxSize != (sizeof(pTxCh->SeqNum) + sizeof(pDataInfo->TimeStamp))) {
      ArmLog_ERR(ARM_LOG_ETH, "## TxSize != TimeStamp Size (%u, %u)", TxSize, sizeof(pTxCh->SeqNum) + sizeof(pDataInfo->TimeStamp));
      Rval = ARM_NG;
    }
  }

  /* Send Data */
  if (ARM_OK == Rval) {
    for (i = 0; i < pSizeInfo->Num; i++) {

      if (ARM_OK == Rval) {
        UINT32 TxSize;

        /* Header, '+DATA' */
        TxSize = XportSend(pTxCh, HEADER_DATA, strlen(HEADER_DATA));
        if (TxSize != strlen(HEADER_DATA)) {
          ArmLog_ERR(ARM_LOG_ETH, "## TxSize != HEADER_DATA Size (%u, %u)", TxSize, strlen(HEADER_DATA));
          Rval = ARM_NG;
        }
      }

      if (ARM_OK == Rval) {
        UINT32 TxSize;
        char *pBuf = pDataInfo->pBuf[i];

        /* Data */
        TxSize = XportSend(pTxCh, pBuf, pSizeInfo->Size[i]);
        if (TxSize != pSizeInfo->Size[i]) {
          ArmLog_ERR(ARM_LOG_ETH, "## TxSize != Data Size (%u, %u)", TxSize, pSizeInfo->Size[i]);
          Rval = ARM_NG;
        }
      }

      /* Check TX status */
      if (ARM_NG == Rval) {
        break;
      }

    }
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmEth_Init
 *
 *  @Description:: Create TCP server and wait for client to connect
 *
 *  @Input      :: None
 *
 *  @Output     ::
 *    pCh:         The communication channel id
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmEth_Init(UINT32 *pCh)
{
  UINT32 Rval = ARM_OK;
  ETH_MQ_ITEM_s MQItem = {0};
  static UINT32 ChNum  = 0U;
  static UINT32 Inited = 0U;
  static UINT32 Port   = SOCKET_PORT;

  if (Inited == 0U) {
    Inited = 1U;
    /* Create one socket to listen on a particular port */
    Rval = XportInit(&Port);
  }

  if (Rval == ARM_OK) {
    if (ChNum >= MAX_CHANNEL) {
      ArmLog_ERR(ARM_LOG_ETH, "## ArmEth_Init fail: channel# over %u", MAX_CHANNEL, 0U);
      Rval = ARM_NG;
    } else {
      /* Wait channel connetcted */
      Rval = ArmMsgQueue_Recv(&g_MsgQ, (void *)&MQItem);
    }
  }

  if (Rval == ARM_OK) {
    ArmLog_DBG(ARM_LOG_ETH, "ArmEth_Init() channel %u connected!!", MQItem.Ch, 0U);
    *pCh = MQItem.Ch;
    ChNum++;
  }

  return Rval;
}

