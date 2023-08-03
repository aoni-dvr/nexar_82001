/**
*  @file Eth.c
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
*   @details Example to exchange data between remote PC and CV2x
*
*/

#include <string.h>          /* for memset(), memcmp() */

#include "ArmEth.h"
#include "Eth.h"

#define ARM_LOG_ETHERNET        "ETH-F" /* Ethernet data flow */
#define MAX_ETH_RX_DEPTH        (4U)    /* related to MAX_BUF_DEPTH in CtCvAlgoWrapper */
#define MAX_ETH_MSG_QUEUE_NUM   (8U)

typedef struct {
  UINT32              SeqNum;
  UINT64              TimeStamp;
  UINT32              RxNum;
  flexidag_memblk_t   Buf[ARM_ETH_MAX_IO];
} ETH_RX_DATA_s;

typedef struct {
  ETH_RX_DATA_s       RxData[MAX_ETH_RX_DEPTH];
  UINT32              Wp;               /* Write index*/
  UINT32              AvblNum;          /* Available buffer number */
  UINT32              MaxAvblNum;       /* The maximum available buffer number */
  ArmMutex_t          Mutex;            /* Protect AvblNum */
} ETH_RX_CTRL_s;

typedef struct {
  UINT32              RxDataIndex;
  ARM_ETH_SIZE_INFO_s SizeInfo;
} ETH_MQ_ITEM_s;

typedef struct _ETH_CONFIG {
  /* Ethernet communication cnannel */
  UINT32              Ch;
  /* Memory usage source */
  UINT32              MemPoolId;
  /* Message Q for read/feed threads*/
  ArmMsgQueue_t       MsgQueue;
  ETH_MQ_ITEM_s       MsgQueueBuf[MAX_ETH_MSG_QUEUE_NUM];
  /* Received data control */
  ETH_RX_CTRL_s       RxCtrl;
  /* ReedThead: read data from remote PC. */
  pthread_t           ReadThread;
  /* FeedThreed: trigger to run flexidag iterations */
  pthread_t           FeedThread;
} ETH_CONFIG_s;

static char MsgQName[]  = "EthMsgQ";
static char MutexName[] = "EthCtrlMut";

static ETH_CONFIG_s g_Config;


/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: GetAvailableRxDataIdx
 *
 *  @Description:: Get available Rx Data index
 *
 *  @Input      ::
 *     pRxCtrl:    The Rx control
 *
 *  @Output     ::
 *     pIndex:     The available Rx Data index
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 GetAvailableRxDataIdx(ETH_RX_CTRL_s *pRxCtrl, UINT32 *pIndex)
{
  UINT32 Rval = ARM_OK;
  UINT32 WaitTime = 10; // 10ms

  /* 1. Sanity check */
  if (pRxCtrl == NULL) {
    ArmLog_ERR(ARM_LOG_ETHERNET, "## pFrameCtrl is null", 0U, 0U);
    Rval = ARM_NG;
  }
  if (pIndex == NULL) {
    ArmLog_ERR(ARM_LOG_ETHERNET, "## pIndex is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Get available index */
  if ((pRxCtrl != NULL) && (pIndex != NULL)) {
    do {
      (void) ArmMutex_Take(&pRxCtrl->Mutex);

      if (pRxCtrl->AvblNum > 0U) {
        pRxCtrl->AvblNum--;

        *pIndex = pRxCtrl->Wp;
        pRxCtrl->Wp ++;
        pRxCtrl->Wp %= pRxCtrl->MaxAvblNum;

        (void) ArmMutex_Give(&pRxCtrl->Mutex);
        break;
      } else {
        (void) ArmMutex_Give(&pRxCtrl->Mutex);
        ArmLog_WARN(ARM_LOG_ETHERNET, "Warning!! Cannot get index, wait %ums", WaitTime, 0U);
        (void) ArmTask_Sleep(WaitTime);
      }
    } while (Rval == ARM_OK);
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: FreeRxDataIdx
 *
 *  @Description:: Free Rx Data index
 *
 *  @Input      ::
 *     pRxCtrl:    The Rx control
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 FreeRxDataIdx(ETH_RX_CTRL_s *pRxCtrl)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check */
  if (pRxCtrl == NULL) {
    ArmLog_ERR(ARM_LOG_ETHERNET, "## pRxCtrl is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Free index */
  if (Rval == ARM_OK) {
    (void) ArmMutex_Take(&pRxCtrl->Mutex);

    if (pRxCtrl->AvblNum >= pRxCtrl->MaxAvblNum) {
      ArmLog_DBG(ARM_LOG_ETHERNET, "## can't free index due to incorrect AvblNum %u", pRxCtrl->AvblNum, 0U);
      Rval = ARM_NG;
    } else {
      pRxCtrl->AvblNum++;
    }

    (void) ArmMutex_Give(&pRxCtrl->Mutex);
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ReadThread
 *
 *  @Description:: The thread to read data from remote PC
 *
 *  @Input      ::
 *      arg:       The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* ReadThread(void *arg)
{
  UINT32 Rval = ARM_OK;
  ETH_CONFIG_s *pConfig = &g_Config;

  do {
    ARM_ETH_SIZE_INFO_s SizeInfo = {0};
    ARM_ETH_DATA_INFO_s DataInfo = {0};
    ETH_RX_DATA_s *pRxData;
    UINT32 RxIndex;

    /* 1. Get size info from remote PC */
    Rval = ArmEth_GetSize(pConfig->Ch, &SizeInfo);

    /* 2. Get available Rx data for allocating mem */
    if (ARM_OK == Rval) {
      Rval = GetAvailableRxDataIdx(&pConfig->RxCtrl, &RxIndex);
    }

    /* 3. Allocate mem for receving data */
    if (ARM_OK == Rval) {
      pRxData = &(pConfig->RxCtrl.RxData[RxIndex]);
      /* Fill number of received data */
      pRxData->RxNum = SizeInfo.Num;
      for (UINT32 i = 0; i < SizeInfo.Num; i++) {
        /* Fill buf address from mem pool to RxData */
        Rval = ArmMemPool_AllocateBlock(pConfig->MemPoolId, SizeInfo.Size[i], &pRxData->Buf[i], 0U);
        if (ARM_OK != Rval) {
          ArmLog_ERR(ARM_LOG_ETHERNET, "## ArmMemPool_AllocateBlock fail", 0U, 0U);
          break;
        }
        /* Assign RX buffer address */
        DataInfo.pBuf[i] = pRxData->Buf[i].pBuffer;
      }
    }

    /* 4. Receive data from remote PC */
    if (ARM_OK == Rval) {
      Rval = ArmEth_Recv(pConfig->Ch, &SizeInfo, &DataInfo);
    }

    /* 5. Send message to feed thread */
    if (ARM_OK == Rval) {
      ETH_MQ_ITEM_s MqItem = {0};
      /* Fill SeqNum/TimeStamp from remote PC to RxData*/
      pRxData->SeqNum    = DataInfo.SeqNum;
      pRxData->TimeStamp = DataInfo.TimeStamp;

      MqItem.RxDataIndex = RxIndex;
      MqItem.SizeInfo    = SizeInfo;
      Rval = ArmMsgQueue_Send(&pConfig->MsgQueue, (void *)&MqItem);
    }

  } while (ARM_OK == Rval);

  ArmLog_DBG(ARM_LOG_ETHERNET, "Read thread Done!!", 0U, 0U);
  return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FeedThread
 *
 *  @Description:: The thread to trigger running flexidag iterations
 *
 *  @Input      ::
 *      arg:       The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* FeedThread(void *arg)
{
  UINT32 Rval = ARM_OK;
  ETH_CONFIG_s *pConfig = &g_Config;

  do {
    ETH_MQ_ITEM_s MqItem = {0};
    ETH_RX_DATA_s *pRxData;
    ARM_ETH_SIZE_INFO_s *pSizeInfo;

    Rval = ArmMsgQueue_Recv(&pConfig->MsgQueue, &MqItem);

    if (Rval == ARM_OK) {
      memio_source_recv_multi_raw_t MultiRaw = {0};
      void *pUserData;

      pSizeInfo = &MqItem.SizeInfo;
      pRxData   = &(pConfig->RxCtrl.RxData[MqItem.RxDataIndex]);

      /* Do cache clean before feeding data to flexidag */
      for (UINT32 i = 0U; i < pSizeInfo->Num; i++) {
        flexidag_memblk_t *pRawDataBuf = &(pRxData->Buf[i]);
        if(pRawDataBuf->buffer_cacheable != 0U){
            (void) ArmMemPool_CacheClean(pRawDataBuf);
          }
      }

      /* Prepare Raw */
      MultiRaw.num_io = pSizeInfo->Num;
      for (UINT32 i = 0U; i < pSizeInfo->Num; i ++) {
        MultiRaw.io[i].addr      = pRxData->Buf[i].buffer_daddr;   //physical address
        MultiRaw.io[i].size      = pSizeInfo->Size[i];
        MultiRaw.io[i].pitch     = 0;
        MultiRaw.io[i].batch_cnt = 0;
      }

      /* Prepare User Data */
      pUserData = (void *)pRxData;
      /* Test on Slot 0 */
      Rval = CtCvAlgoWrapper_FeedRaw(0U, &MultiRaw, pUserData);
    }

  } while (ARM_OK == Rval);

  ArmLog_DBG(ARM_LOG_ETHERNET, "Feed thread Done!!", 0U, 0U);
  return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FreeRxDataBufCallback
 *
 *  @Description:: Callback for free Rx Data Buf
 *
 *  @Input      ::
 *    Event:       The callback event
 *    pEventData:  Pointer to SVC_CV_ALGO_OUTPUT_s
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      OK or NG
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FreeRxDataBufCallback(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
  UINT32 Rval = ARM_OK;
  ETH_CONFIG_s *pConfig = &g_Config;
  ETH_RX_DATA_s *pRxData;

  if (Event == CALLBACK_EVENT_FREE_INPUT) {
    /* Sanity check */
    if (NULL == pEventData) {
      ArmLog_ERR(ARM_LOG_ETHERNET, "## FreeRxDataBufCallback fail (pEventData is null)", 0U, 0U);
      Rval = ARM_NG;
    }

    if (ARM_OK == Rval) {
      pRxData = (ETH_RX_DATA_s *)pEventData->pUserData;
      if (NULL == pRxData) {
        ArmLog_ERR(ARM_LOG_ETHERNET, "## FreeRxDataBufCallback fail (pUserData is null)", 0U, 0U);
        Rval = ARM_NG;
      }
    }

    /* free Rx buffer */
    if (ARM_OK == Rval) {
      for (UINT32 i = 0U; i < pRxData->RxNum; i++) {
        Rval = ArmMemPool_Free(pConfig->MemPoolId, &pRxData->Buf[i]);
      }
    }
    /* cannot free RxData index right now; free it during 'Event Output' */
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: EthTxCallback
 *
 *  @Description:: Callback for sending flexidag output to remote PC
 *
 *  @Input      ::
 *    Event:       The callback event
 *    pEventData:  Pointer to SVC_CV_ALGO_OUTPUT_s
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      OK or NG
\*-----------------------------------------------------------------------------------------------*/
static UINT32 EthTxCallback(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
  UINT32 Rval = ARM_OK;
  ETH_CONFIG_s *pConfig = &g_Config;
  ETH_RX_DATA_s *pRxData;
  AMBA_CV_FLEXIDAG_IO_s *pOutput;

  if (Event == CALLBACK_EVENT_OUTPUT) {
    /* Sanity check */
    if (NULL == pEventData) {
      ArmLog_ERR(ARM_LOG_ETHERNET, "## EthTxCallback fail (pEventData is null)", 0U, 0U);
      Rval = ARM_NG;
    }

    if (ARM_OK == Rval) {
      pRxData = (ETH_RX_DATA_s *)pEventData->pUserData;
      if (NULL == pRxData) {
        ArmLog_ERR(ARM_LOG_ETHERNET, "## EthTxCallback fail (pUserData is null)", 0U, 0U);
        Rval = ARM_NG;
      }
    }

    if (ARM_OK == Rval) {
      pOutput = pEventData->pOutput;
      if (NULL == pOutput) {
        ArmLog_ERR(ARM_LOG_ETHERNET, "## EthTxCallback fail (pOutput is null)", 0U, 0U);
        Rval = ARM_NG;
      }
    }

    if (ARM_OK == Rval) {
      ARM_ETH_SIZE_INFO_s SizeInfo = {0};
      ARM_ETH_DATA_INFO_s DataInfo = {0};

      /* Fill size info & DataInfo */
      SizeInfo.Num = pOutput->num_of_buf;
      // DataInfo.SeqNum will be assigned value internally in ArmEth_Send()
      DataInfo.TimeStamp = pRxData->TimeStamp;
      for (UINT32 i = 0U; i < SizeInfo.Num; i++) {
        SizeInfo.Size[i] = pOutput->buf[i].buffer_size;
        DataInfo.pBuf[i] = pOutput->buf[i].pBuffer;
      }

      /* free RxData index */
      Rval = FreeRxDataIdx(&pConfig->RxCtrl);
      if (ARM_OK == Rval) {
        Rval = ArmEth_Send(pConfig->Ch, &SizeInfo, &DataInfo);
      }
    }
  }

  return Rval;
}

/*---------------------------------------------------------------------------*\
 * API
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: Eth_Start
*
*  @Description:: Start to init Ethernet and flexidag application
*
*  @Input      ::
*    MemPoolId:      The memory pool id
*
*  @Output     :: None
*
*  @Return     :: None
*    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Eth_Start(UINT32 MemPoolId)
{
  UINT32 Rval = ARM_OK;
  ETH_CONFIG_s *pConfig = &g_Config;

  /* 0. Set memory pool */
  pConfig->MemPoolId = MemPoolId;

  /* 1. Init Ethernet and get channel id */
  Rval = ArmEth_Init(&(pConfig->Ch));

  /* 2. Throughput test */
  {
    flexidag_memblk_t TestBuf = {0};
    UINT32 Size = 10 * 1024 * 1024;

    if (ARM_OK == Rval) {
      Rval = ArmMemPool_Allocate(pConfig->MemPoolId, Size, &TestBuf);
    }

    if (ARM_OK == Rval) {
      Rval = ArmEth_TxRxTest(pConfig->Ch, TestBuf.pBuffer, Size);
      (void) ArmMemPool_Free(pConfig->MemPoolId, &TestBuf);
    }
  }

  /* 3. Create message queue for read/feed threads */
  if (ARM_OK == Rval) {
    Rval = ArmMsgQueue_Create(&pConfig->MsgQueue, MsgQName, sizeof(ETH_MQ_ITEM_s), MAX_ETH_MSG_QUEUE_NUM, (void *)&pConfig->MsgQueueBuf[0]);
  }

  /* 4. Init Rx data control */
  if (ARM_OK == Rval) {
    pConfig->RxCtrl.Wp         = 0U;
    pConfig->RxCtrl.AvblNum    = MAX_ETH_RX_DEPTH;
    pConfig->RxCtrl.MaxAvblNum = MAX_ETH_RX_DEPTH;

    Rval = ArmMutex_Create(&(pConfig->RxCtrl.Mutex), MutexName);
  }

  /* 5. Prepare flexidag callbacks */
  if (ARM_OK == Rval) {
    Rval = CtCvAlgoWrapper_RegCb(0U, 0U, FreeRxDataBufCallback);
  }

  if (ARM_OK == Rval) {
    Rval = CtCvAlgoWrapper_RegCb(0U, 0U, EthTxCallback);
  }

  if (ARM_OK == Rval) {
    /* 6. Create threads to read file/feed data */
    pthread_create(&pConfig->ReadThread, NULL, ReadThread, 0);
    pthread_create(&pConfig->FeedThread, NULL, FeedThread, 0);

    pthread_join(pConfig->ReadThread, NULL);
    ArmLog_DBG(ARM_LOG_ETHERNET, "Read thread terminated!", 0U, 0U);
    pthread_join(pConfig->FeedThread, NULL);
    ArmLog_DBG(ARM_LOG_ETHERNET, "Feed thread terminated!", 0U, 0U);
  }

  /* TBD: Deinit flow */
  if (ARM_OK != Rval) {
    ArmLog_ERR(ARM_LOG_ETHERNET, "## ArmEventFlag_Create fail", 0U, 0U);
  }

  return Rval;
}

