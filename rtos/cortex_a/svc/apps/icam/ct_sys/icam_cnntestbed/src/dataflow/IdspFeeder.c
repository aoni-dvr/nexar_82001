/**
*  @file IdspFeeder.c
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
*   @details The FileFeeder example (The input data comes from idsp)
*
*/

#include "IdspFeeder.h"

/* Amba header */
#include "AmbaMisraFix.h"
#include "AmbaCache.h"
#include "AmbaMMU.h"
#ifdef CONFIG_ENABLE_AMBALINK
#include "AmbaIPC_FlexidagIO.h"
#endif

#define ARM_LOG_IDSP_FEEDER_CMD     "IdspFeeder"
#define MAX_IDSP_FEEDER_SLOT_NUM    (4U)
#define MAX_IDSP_FEEDER_MSGQ_NUM    (32U)
#define IDSP_FEEDER_TSK_STACK_SIZE  (0x2000U)

#ifdef CONFIG_ENABLE_AMBALINK
#define ARM_IDSP_IPC_CH             (0U)
#endif

typedef struct {
  UINT32  FrameRateDivisor;
  UINT32  FrameCnt;
  UINT32  Valid;
} FEED_INFO_s;

typedef struct {
  UINT32             IsCreated;
  ArmTask_t          Tsk;
  char               TskStack[IDSP_FEEDER_TSK_STACK_SIZE];
  ArmMsgQueue_t      MsgQ;
  FEED_MQ_ITEM_s     MsgQBuf[MAX_IDSP_FEEDER_MSGQ_NUM];
  FEED_INFO_s        FeedInfo[MAX_IDSP_FEEDER_SLOT_NUM];
  char               TskName[32];
  char               MsgQName[32];
} IDSP_FEEDER_CTRL_s;

static IDSP_FEEDER_CTRL_s       g_IdspFeederCtrl = {0};

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: IdspFeederEntry
 *
 *  @Description:: The task to feed picinfo to cvalgo
 *
 *  @Input      ::
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*---------------------------------------------------------------------------*/
static ArmTaskRet_t IdspFeederEntry(ArmTaskEntryArg_t EntryArg)
{
  UINT32             Rval = ARM_OK;
  FEED_MQ_ITEM_s     MsgQItem = {0};
  FEED_INFO_s        *pFeedInfo;
  memio_source_recv_picinfo_t FeedPicInfo = {0};

  AmbaMisra_TouchUnused(EntryArg);

  while (Rval == ARM_OK) {
    Rval = ArmMsgQueue_Recv(&g_IdspFeederCtrl.MsgQ, &MsgQItem);

    if (Rval == ARM_OK) {
      UINT32 IsFill = 0U;

      /* 1. Feed idsp info to each register slot */
      for (UINT32 SlotIdx = 0U; SlotIdx < MAX_IDSP_FEEDER_SLOT_NUM; SlotIdx++) {
        pFeedInfo = &g_IdspFeederCtrl.FeedInfo[SlotIdx];
        if (pFeedInfo->Valid == 1U) {

          /* 2. Only send in corresponding framerate */
          if (pFeedInfo->FrameCnt == 0U) {

            /* 3. Fill pic_info data */
            if (IsFill == 0U) { // only fill once for each frame
              FeedPicInfo.pic_info.frame_num               = (UINT32)MsgQItem.CapSequence;
              FeedPicInfo.pic_info.capture_time            = (UINT32)MsgQItem.CapPts;
              FeedPicInfo.pic_info.pyramid.image_width_m1  = MsgQItem.Window[0U].Width - 1U;
              FeedPicInfo.pic_info.pyramid.image_height_m1 = MsgQItem.Window[0U].Height - 1U;
              FeedPicInfo.pic_info.pyramid.image_pitch_m1  = (UINT32)MsgQItem.YuvImgBuf[0U].Pitch - 1U;

              for (UINT32 i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
                ULONG TempPhyAddr;
                FeedPicInfo.pic_info.pyramid.half_octave[i].ctrl.disable   = 0U;
                FeedPicInfo.pic_info.pyramid.half_octave[i].ctrl.roi_pitch = MsgQItem.YuvImgBuf[i].Pitch;
                FeedPicInfo.pic_info.pyramid.half_octave[i].roi_width_m1   = MsgQItem.Window[i].Width - 1U;
                FeedPicInfo.pic_info.pyramid.half_octave[i].roi_height_m1  = MsgQItem.Window[i].Height - 1U;
                FeedPicInfo.pic_info.pyramid.half_octave[i].roi_start_col  = 0;
                FeedPicInfo.pic_info.pyramid.half_octave[i].roi_start_row  = 0;
                (void) AmbaMMU_VirtToPhys(MsgQItem.YuvImgBuf[i].BaseAddrY, &TempPhyAddr);
                FeedPicInfo.pic_info.rpLumaLeft[i]                         = (UINT32)TempPhyAddr;
                FeedPicInfo.pic_info.rpLumaRight[i]                        = (UINT32)TempPhyAddr;
                (void) AmbaMMU_VirtToPhys(MsgQItem.YuvImgBuf[i].BaseAddrUV, &TempPhyAddr);
                FeedPicInfo.pic_info.rpChromaLeft[i]                       = (UINT32)TempPhyAddr;
                FeedPicInfo.pic_info.rpChromaRight[i]                      = (UINT32)TempPhyAddr;
              }

              IsFill = 1U;
            }  //  end of (IsFill == 0U)

            /* 4. Feed to cvalgo */
#ifdef CONFIG_ENABLE_AMBALINK
            Rval = AmbaIPC_FlexidagIO_SetInput(ARM_IDSP_IPC_CH, &FeedPicInfo, sizeof(memio_source_recv_picinfo_t));
            if (Rval != ARM_OK) {
              ArmLog_ERR(ARM_LOG_IDSP_FEEDER_CMD, "## AmbaIPC_FlexidagIO_SetInput() fail", 0U, 0U);
            }
#else
            Rval = CtCvAlgoWrapper_FeedPicinfo(SlotIdx, &FeedPicInfo, NULL);
            if (Rval != ARM_OK) {
              ArmLog_ERR(ARM_LOG_IDSP_FEEDER_CMD, "## CtCvAlgoWrapper_FeedPicinfo() fail", 0U, 0U);
            }
#endif
          }

          /* 5. Update FrameCnt */
          pFeedInfo->FrameCnt ++;
          pFeedInfo->FrameCnt %= pFeedInfo->FrameRateDivisor;

        }  // end of "if(pFeedInfo->Valid == 1)"
      }  // end of "for (UINT32 SlotIdx = 0U; SlotIdx < MAX_IDSP_FEEDER_SLOT_NUM; SlotIdx++)"
    }  // end of "if (Rval == ARM_OK)"
  } // end of while(Rval == ARM_OK)

  ArmLog_ERR(ARM_LOG_IDSP_FEEDER_CMD, "## Terminate IdspFeederEntry due to errors", 0U, 0U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CreateResource
 *
 *  @Description:: Create resource - task and message queue
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 CreateResource(void)
{
  UINT32 Rval = ARM_OK;

  if (g_IdspFeederCtrl.IsCreated == 0U) {
    const FEED_MQ_ITEM_s *pMsgQBuf;
    void *pVoidBuf;

    /* Create Message Queue */
    ArmStdC_strcpy(g_IdspFeederCtrl.MsgQName, 32, "IdspFeederMsgQ");
    pMsgQBuf = &g_IdspFeederCtrl.MsgQBuf[0];
    //pVoidBuf = (void *) pMsgQBuf
    (void) ArmStdC_memcpy(&pVoidBuf, &pMsgQBuf, sizeof(void *));
    Rval = ArmMsgQueue_Create(&g_IdspFeederCtrl.MsgQ, g_IdspFeederCtrl.MsgQName, sizeof(FEED_MQ_ITEM_s),
      MAX_IDSP_FEEDER_MSGQ_NUM, pVoidBuf);

    /* Create Task */
    if (Rval == ARM_OK) {
      ARM_TASK_CREATE_ARG_s  TskArg = {0};

      ArmStdC_strcpy(g_IdspFeederCtrl.TskName, 32, "IdspFeederTsk");

      TskArg.TaskPriority  = 107U;
      TskArg.EntryFunction = IdspFeederEntry;
      TskArg.EntryArg      = NULL;
      TskArg.pStackBase    = &(g_IdspFeederCtrl.TskStack[0]);
      TskArg.StackByteSize = IDSP_FEEDER_TSK_STACK_SIZE;
      TskArg.CoreSel       = ARM_TASK_CORE0;

      Rval = ArmTask_Create(&(g_IdspFeederCtrl.Tsk), g_IdspFeederCtrl.TskName, &TskArg);
    }

    if (Rval == ARM_OK) {
      g_IdspFeederCtrl.IsCreated = 1U;
    } else {
      ArmLog_ERR(ARM_LOG_IDSP_FEEDER_CMD, "## CreateResource fail", 0U, 0U);
    }
  }

  return Rval;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: DeleteResource
 *
 *  @Description:: Delete resource - task and message queue
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 DeleteResource(void)
{
  UINT32 Rval = ARM_OK;

  if (g_IdspFeederCtrl.IsCreated == 1U) {
    /* Delete Task */
    Rval = ArmTask_Delete(&(g_IdspFeederCtrl.Tsk));

    /* Delete Message Queue */
    if (Rval == ARM_OK) {
      Rval = ArmMsgQueue_Delete(&g_IdspFeederCtrl.MsgQ, g_IdspFeederCtrl.MsgQName);
    }

    if (Rval == ARM_OK) {
      g_IdspFeederCtrl.IsCreated = 0U;
    } else {
      ArmLog_ERR(ARM_LOG_IDSP_FEEDER_CMD, "## DeleteResource fail", 0U, 0U);
    }
  }

  return Rval;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IdspFeeder_Start
 *
 *  @Description:: Load pyramid data from idsp and feed to DataSrc cvtask
 *
 *  @Input      ::
 *   pFeederCfg:   The IdspFeeder Config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32       ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 IdspFeeder_Start(const IDSP_FEEDER_CFG_s *pFeederCfg)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for parameters */
  if (pFeederCfg == NULL) {
    ArmLog_ERR(ARM_LOG_IDSP_FEEDER_CMD, "## IdspFeeder_Start fail (pFeederCfg is NULL)", 0U, 0U);
    Rval = ARM_NG;
  } else {
    if (pFeederCfg->Slot >= MAX_IDSP_FEEDER_SLOT_NUM) {
      ArmLog_ERR(ARM_LOG_IDSP_FEEDER_CMD, "## IdspFeeder_Start fail (invalid Slot %u)", pFeederCfg->Slot, 0U);
      Rval = ARM_NG;
    }
    if (pFeederCfg->FrameRateDivisor == 0U) {
      ArmLog_ERR(ARM_LOG_IDSP_FEEDER_CMD, "## IdspFeeder_Start fail (FrameRateDivisor can't be zero)", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  /* 2. Create task and message queue */
  if (Rval == ARM_OK) {
    Rval = CreateResource();
  }

  /* 3. Fill FeedInfo */
  if ((Rval == ARM_OK) && (pFeederCfg != NULL)) {
    g_IdspFeederCtrl.FeedInfo[pFeederCfg->Slot].FrameRateDivisor = pFeederCfg->FrameRateDivisor;
    g_IdspFeederCtrl.FeedInfo[pFeederCfg->Slot].FrameCnt         = 0U;
    g_IdspFeederCtrl.FeedInfo[pFeederCfg->Slot].Valid            = 1U;

    ArmLog_DBG(ARM_LOG_IDSP_FEEDER_CMD, "Slot %d FrameRateDivisor %d", pFeederCfg->Slot, pFeederCfg->FrameRateDivisor);
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IdspFeeder_Stop
 *
 *  @Description:: Stop and delete idsp feeder task
 *
 *  @Input      ::
 *    Slot:        The slot id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32       ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 IdspFeeder_Stop(UINT32 Slot)
{
  UINT32 Rval = ARM_OK;
  UINT32 ValidNum = 0U;

  /* 1. Sanity check for parameters */
  if (Slot >= MAX_IDSP_FEEDER_SLOT_NUM) {
    ArmLog_ERR(ARM_LOG_IDSP_FEEDER_CMD, "## IdspFeeder_Stop fail (invalid Slot %u)", Slot, 0U);
    Rval = ARM_NG;
  }

  if ((Rval == ARM_OK) && (Slot < MAX_IDSP_FEEDER_SLOT_NUM)) {
    /* 2-1. Clear FeedInfo */
    g_IdspFeederCtrl.FeedInfo[Slot].Valid = 0U;

    /* 2-2. Find remaining feeding number */
    ValidNum = 0;
    for (UINT32 i = 0U; i < MAX_IDSP_FEEDER_SLOT_NUM; i++) {
      if (g_IdspFeederCtrl.FeedInfo[i].Valid == 1U) {
        ValidNum ++;
      }
    }

    /* 2-3. Delete resource if no one is using it. */
    if (ValidNum == 0U) {
      Rval = DeleteResource();
    }
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IdspFeeder_Feed
 *
 *  @Description:: Feed MqItem to message queue
 *
 *  @Input      ::
 *    MqItem:      The message queue items
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32       ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 IdspFeeder_Feed(FEED_MQ_ITEM_s *pMqItem)
{
  UINT32 Rval = ARM_OK;
  ARM_MSG_QUEUE_QUERY_INFO_s  QueueInfo = {0};

  if (g_IdspFeederCtrl.IsCreated == 1U) {
    Rval = ArmMsgQueue_Query(&g_IdspFeederCtrl.MsgQ, &QueueInfo);
    if (Rval == ARM_OK) {
      if (QueueInfo.NumAvailable > 0U) {
        Rval = ArmMsgQueue_Send(&g_IdspFeederCtrl.MsgQ, pMqItem);
      } else {
        ArmLog_DBG(ARM_LOG_IDSP_FEEDER_CMD, "MsgQueue of IDSP_FEEDER is full!! Slow down!", 0U, 0U);
      }
    }
  }

  return Rval;
}

