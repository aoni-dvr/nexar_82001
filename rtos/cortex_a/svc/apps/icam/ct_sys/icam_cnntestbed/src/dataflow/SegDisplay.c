/**
*  @file SegDisplay.c
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
*   @details The SEG-like fusion example
*
*/


#include "SegDisplay.h"
#include "ArmOsdSender.h"
#ifdef CONFIG_ENABLE_AMBALINK
#include "AmbaIPC_FlexidagIO.h"
#endif

#define ARM_LOG_SEG_DISP        "SegDisp"

#ifdef CONFIG_ENABLE_AMBALINK
#define TASK_SEG_DISP_IPC_PRIORITY    (110U)
#define TASK_SEG_DISP_IPC_STACK_SZ    (0xC000U)
#define ARM_IDSP_IPC_CH               (0U)
#endif

typedef struct {
  SEG_WIN_SETTING_s Win;
  UINT32 OsdFlushFlg;
  UINT32 OsdClearFlg;
} SEG_DISPLAY_CTRL_s;

static SEG_DISPLAY_CTRL_s g_SegDispCtrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SegDisplayCallback
 *
 *  @Description:: Callback for seg fusion and OSD display
 *
 *  @Input      ::
 *    Event:       The callback event
 *    pEventData:  Pointer to data
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK or NG
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SegDisplayCallback(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
  UINT32 Rval = ARM_OK;

  if (Event == CALLBACK_EVENT_OUTPUT) {

    /* 1. Sanity check for parameters */
    if (pEventData == NULL) {
      ArmLog_ERR(ARM_LOG_SEG_DISP, "## SegDisplayCallback fail (pEventData is null)", 0U, 0U);
      Rval = ARM_NG;
    } else {
      if (pEventData->pOutput == NULL) {
        ArmLog_ERR(ARM_LOG_SEG_DISP, "## SegDisplayCallback fail (pOutput is null)", 0U, 0U);
        Rval = ARM_NG;
      }
    }

    /* 2. Do Osd drawing */
    if ((Rval == ARM_OK) && (pEventData != NULL)) {
      OSD_DRAW_SEG_CFG_s OsdSegCfg = {0};

      OsdSegCfg.OsdClearFlg   = g_SegDispCtrl.OsdClearFlg;
      OsdSegCfg.OsdFlushFlg   = g_SegDispCtrl.OsdFlushFlg;

      OsdSegCfg.RoiWidth      = g_SegDispCtrl.Win.RoiWidth;
      OsdSegCfg.RoiHeight     = g_SegDispCtrl.Win.RoiHeight;
      OsdSegCfg.RoiStartX     = g_SegDispCtrl.Win.RoiStartX;
      OsdSegCfg.RoiStartY     = g_SegDispCtrl.Win.RoiStartY;
      OsdSegCfg.NetworkWidth  = g_SegDispCtrl.Win.NetworkWidth;
      OsdSegCfg.NetworkHeight = g_SegDispCtrl.Win.NetworkHeight;

      //OsdSegCfg.pArgMaxOut = (UINT8 *) pEventData->pOutput->buf[0].pBuffer
      (void) ArmStdC_memcpy(&OsdSegCfg.pArgMaxOut, &pEventData->pOutput->buf[0].pBuffer, sizeof(UINT8 *));

      Rval = ArmOsdSender_DrawSeg(OsdSegCfg);
    }
  }

  return Rval;
}

#ifdef CONFIG_ENABLE_AMBALINK
/*---------------------------------------------------------------------------*\
 *  @RoutineName:: SegDisplayIPCEntry
 *
 *  @Description:: The task to get flexidag result from FlexidagIO
 *
 *  @Input      ::
 *     Slot:       Flexidag id
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*---------------------------------------------------------------------------*/
static ArmTaskRet_t SegDisplayIPCEntry(ArmTaskEntryArg_t EntryArg)
{
  UINT32 Rval, GetLen;
  memio_sink_send_out_t SinkOut = {0};
  SVC_CV_ALGO_OUTPUT_s  SvcCvAlgoOut = {0};
  AMBA_CV_FLEXIDAG_IO_s Out = {0};

  (void) EntryArg;

  while (1) {
    Rval = AmbaIPC_FlexidagIO_GetResult(ARM_IDSP_IPC_CH, (void *)&SinkOut, &GetLen);
    if ((Rval == 0U) && (GetLen == sizeof(memio_sink_send_out_t))) {
        Out.num_of_buf = SinkOut.num_of_io;
        for (UINT32 i = 0; i < Out.num_of_buf; i++) {
          ULONG VirtAddr;
          (void) AmbaMMU_PhysToVirt((ULONG)SinkOut.io[i].addr, &VirtAddr);
          Out.buf[i].pBuffer          = (char *) VirtAddr;
          Out.buf[i].buffer_daddr     = SinkOut.io[i].addr;
          Out.buf[i].buffer_size      = SinkOut.io[i].size;
          Out.buf[i].buffer_cacheable = 1;
        }

        SvcCvAlgoOut.pOutput    = &Out;
        SvcCvAlgoOut.pUserData  = NULL;
        SvcCvAlgoOut.pExtOutput = NULL;
        SegDisplayCallback(CALLBACK_EVENT_OUTPUT, &SvcCvAlgoOut);
    } else {
      ArmLog_ERR(ARM_LOG_SEG_DISP, "## AmbaIPC_FlexidagIO_GetResult() fail Rval %u GetLen %u", Rval, GetLen);
      break;
    }
  }
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SegDisplay_Register
 *
 *  @Description:: Register SegDisplay callback
 *
 *  @Input      ::
 *    pSegDispCfg: The SegDisplay config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 SegDisplay_Register(const SEG_DISPLAY_CFG_s *pSegDispCfg)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for parameters */
  if (pSegDispCfg == NULL) {
    ArmLog_ERR(ARM_LOG_SEG_DISP, "## SegDisplay_Register fail (pSegDispCfg is null)", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Keep setting to global variable */
  if ((Rval == ARM_OK) && (pSegDispCfg != NULL)) {
      g_SegDispCtrl.Win = pSegDispCfg->Win;
      g_SegDispCtrl.OsdClearFlg = pSegDispCfg->OsdClearFlg;
      g_SegDispCtrl.OsdFlushFlg = pSegDispCfg->OsdFlushFlg;
  }

#ifdef CONFIG_ENABLE_AMBALINK
    /* 3. Create task to receive result from linux */
    if (Rval == ARM_OK) {
      static char TskName[32] = "SegDispIPCTask";
      static char StackBuf[TASK_SEG_DISP_IPC_STACK_SZ];
      static ArmTask_t Tsk;
      ARM_TASK_CREATE_ARG_s  TskArg = {0};


      TskArg.TaskPriority  = TASK_SEG_DISP_IPC_PRIORITY;
      TskArg.EntryFunction = SegDisplayIPCEntry;
      TskArg.EntryArg      = 0;
      TskArg.pStackBase    = StackBuf;
      TskArg.StackByteSize = TASK_SEG_DISP_IPC_STACK_SZ;
      TskArg.CoreSel       = ARM_TASK_CORE0;

      Rval = ArmTask_Create(&Tsk, TskName, &TskArg);
    }

#else
  /* 3. Register callback */
  if ((Rval == ARM_OK) && (pSegDispCfg != NULL)) {
    Rval = CtCvAlgoWrapper_RegCb(pSegDispCfg->Slot, 0, SegDisplayCallback);
    if (Rval != ARM_OK) {
      ArmLog_ERR(ARM_LOG_SEG_DISP, "## CtCvAlgoWrapper_RegCb fail", 0U, 0U);
      Rval = ARM_NG;
    }
  }
#endif

  return Rval;
}

