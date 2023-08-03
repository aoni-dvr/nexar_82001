/**
*  @file SsdDisplay.c
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
*   @details The SSD-like fusion example
*
*/


#include "SsdDisplay.h"
#include "ArmOsdSender.h"
#ifdef CONFIG_ENABLE_AMBALINK
#include "AmbaIPC_FlexidagIO.h"
#endif

#define ARM_LOG_SSD_DISP        "SsdDisp"

#define MAX_OSD_BOX_SIZE        (100)

#ifdef CONFIG_ENABLE_AMBALINK
#define ARM_IDSP_IPC_CH               (0U)
#define TASK_SSD_DISP_IPC_PRIORITY    (109U)
#define TASK_SSD_DISP_IPC_STACK_SZ    (0xC000U)
#endif

typedef struct {
  UINT32 OsdFlushFlg;
  UINT32 OsdClearFlg;
} SSD_DISPLAY_CTRL_s;

static SSD_DISPLAY_CTRL_s g_SsdDispCtrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SsdDisplayCallback
 *
 *  @Description:: Callback for ssd fusion and OSD display
 *
 *  @Input      ::
 *    Event:       The callback event
 *    pEventData:  Pointer to data (memio_sink_send_out_t)
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK or NG
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SsdDisplayCallback(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
  UINT32 Rval = ARM_OK;
  const CVALGO_SSD_OUTPUT_s *pCvAlgoSsdOutput;
  static OSD_BOX_s g_OsdBox[MAX_OSD_BOX_SIZE];

  if (Event == CALLBACK_EVENT_OUTPUT) {

    /* 1. Sanity check for parameters */
    if (pEventData == NULL) {
      ArmLog_ERR(ARM_LOG_SSD_DISP, "## SsdDisplayCallback fail (pEventData is null)", 0U, 0U);
      Rval = ARM_NG;
    } else {
      if (pEventData->pOutput == NULL) {
        ArmLog_ERR(ARM_LOG_SSD_DISP, "## SsdDisplayCallback fail (pOutput is null)", 0U, 0U);
        Rval = ARM_NG;
      }
    }

    /* 2. Save file for CALLBACK_EVENT_OUTPUT */
    if ((Rval == ARM_OK) && (pEventData != NULL)) {
      OSD_DRAW_BOX_CFG_s OsdCfg = {0};

      //pCvAlgoSsdOutput = (CVALGO_SSD_OUTPUT_s *)pEventData->pOutput->buf[0].pBuffer;
      (void) ArmStdC_memcpy(&pCvAlgoSsdOutput, &(pEventData->pOutput->buf[0].pBuffer), sizeof(void *));

      for (UINT32 i = 0; i < pCvAlgoSsdOutput->BoxNum; i ++) {
#if 0   // DEBUG LOG
        ArmLog_DBG(ARM_LOG_SSD_DISP, "[%u] Detect Class %u ", i, pCvAlgoSsdOutput->Box[i].Class);
        ArmLog_DBG(ARM_LOG_SSD_DISP, "    (X,Y) = (%u,%u)", pCvAlgoSsdOutput->Box[i].X, pCvAlgoSsdOutput->Box[i].Y);
        ArmLog_DBG(ARM_LOG_SSD_DISP, "    (W,H) = (%u,%u)", pCvAlgoSsdOutput->Box[i].W, pCvAlgoSsdOutput->Box[i].H);
#endif
        g_OsdBox[i].Color = pCvAlgoSsdOutput->Box[i].Class;
        g_OsdBox[i].X     = pCvAlgoSsdOutput->Box[i].X;
        g_OsdBox[i].Y     = pCvAlgoSsdOutput->Box[i].Y;
        g_OsdBox[i].W     = pCvAlgoSsdOutput->Box[i].W;
        g_OsdBox[i].H     = pCvAlgoSsdOutput->Box[i].H;
      }

      OsdCfg.BoxNum = pCvAlgoSsdOutput->BoxNum;
      OsdCfg.OsdClearFlg = g_SsdDispCtrl.OsdClearFlg;
      OsdCfg.OsdFlushFlg = g_SsdDispCtrl.OsdFlushFlg;
      OsdCfg.pOsdBox = g_OsdBox;
      if (OsdCfg.BoxNum > 0U) {
        Rval = ArmOsdSender_DrawBox(OsdCfg);
      }
    }

  }
  return Rval;
}

#ifdef CONFIG_ENABLE_AMBALINK
/*---------------------------------------------------------------------------*\
 *  @RoutineName:: SsdDisplayIPCEntry
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
static ArmTaskRet_t SsdDisplayIPCEntry(ArmTaskEntryArg_t EntryArg)
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
        SsdDisplayCallback(CALLBACK_EVENT_OUTPUT, &SvcCvAlgoOut);
    } else {
      ArmLog_ERR(ARM_LOG_SSD_DISP, "## AmbaIPC_FlexidagIO_GetResult() fail Rval %u GetLen %u", Rval, GetLen);
      break;
    }
  }
}
#endif


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SsdDisplay_Register
 *
 *  @Description:: Register SsdDisplay callback
 *
 *  @Input      ::
 *    pSsdDispCfg: The SsdDisplay config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 SsdDisplay_Register(const SSD_DISPLAY_CFG_s *pSsdDispCfg)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for parameters */
  if (pSsdDispCfg == NULL) {
    ArmLog_ERR(ARM_LOG_SSD_DISP, "## SsdDisplay_Register fail (pSsdDispCfg is null)", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Keep setting to global variable */
  if ((Rval == ARM_OK) && (pSsdDispCfg != NULL)) {
      g_SsdDispCtrl.OsdClearFlg = pSsdDispCfg->OsdClearFlg;
      g_SsdDispCtrl.OsdFlushFlg = pSsdDispCfg->OsdFlushFlg;
  }

#ifdef CONFIG_ENABLE_AMBALINK
  /* 3. Create task to receive result from linux */
  if (Rval == ARM_OK) {
    static char TskName[32] = "SsdDispIPCTask";
    static char StackBuf[TASK_SSD_DISP_IPC_STACK_SZ];
    static ArmTask_t Tsk;
    ARM_TASK_CREATE_ARG_s  TskArg = {0};


    TskArg.TaskPriority  = TASK_SSD_DISP_IPC_PRIORITY;
    TskArg.EntryFunction = SsdDisplayIPCEntry;
    TskArg.EntryArg      = 0;
    TskArg.pStackBase    = StackBuf;
    TskArg.StackByteSize = TASK_SSD_DISP_IPC_STACK_SZ;
    TskArg.CoreSel       = ARM_TASK_CORE0;

    Rval = ArmTask_Create(&Tsk, TskName, &TskArg);
  }

#else
  /* 3. Register callback */
  if ((Rval == ARM_OK) && (pSsdDispCfg != NULL)) {
    Rval = CtCvAlgoWrapper_RegCb(pSsdDispCfg->Slot, 0, SsdDisplayCallback);
    if (Rval != ARM_OK) {
      ArmLog_ERR(ARM_LOG_SSD_DISP, "## CtCvAlgoWrapper_RegCb fail", 0U, 0U);
      Rval = ARM_NG;
    }
  }
#endif

  return Rval;
}

