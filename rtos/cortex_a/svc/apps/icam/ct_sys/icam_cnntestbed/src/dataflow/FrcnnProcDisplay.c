/**
*  @file FrcnnProcDisplay.c
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
*   @details The FRCNN-like fusion example
*
*/

#include "CtCvAlgoWrapper.h"
#include "FrcnnProcDisplay.h"
#include "ArmOsdSender.h"

#define ARM_LOG_FRCNN_PROC_DISP       "FrcnnProcDisplay"
#define MAX_OSD_BOX_SIZE              (100U)

typedef struct {
  UINT32 OsdFlushFlg;
  UINT32 OsdClearFlg;
} FRCNN_PROC_DISPLAY_CTRL_s;

static FRCNN_PROC_DISPLAY_CTRL_s g_FrcnnProcDispCtrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FrcnnProcDisplayCallback
 *
 *  @Description:: Callback for frcnn fusion and OSD display
 *
 *  @Input      ::
 *    Event:       The callback event
 *    pEventData:  Pointer to data (memio_sink_send_out_t)
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FrcnnProcDisplayCallback(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
  UINT32 Rval = ARM_OK;
  const CVALGO_FRCNN_OUTPUT_s *pCvAlgoFrcnnOutput;
  static OSD_BOX_s g_OsdBox[MAX_OSD_BOX_SIZE];

  if (Event == CALLBACK_EVENT_OUTPUT) {
    /* 1. Sanity check for parameters */
    if (pEventData == NULL) {
      ArmLog_ERR(ARM_LOG_FRCNN_PROC_DISP, "## FrcnnProcDisplayCallback fail (pEventData is null)", 0U, 0U);
      Rval = ARM_NG;
    } else {
      if (pEventData->pOutput == NULL) {
        ArmLog_ERR(ARM_LOG_FRCNN_PROC_DISP, "## FrcnnProcDisplayCallback fail (pOutput is null)", 0U, 0U);
        Rval = ARM_NG;
      }
    }

    /* 2. Save file for CALLBACK_EVENT_OUTPUT */
    if ((Rval == ARM_OK) && (pEventData != NULL)) {
      OSD_DRAW_BOX_CFG_s OsdCfg = {0};

      (void) ArmStdC_memcpy(&pCvAlgoFrcnnOutput, &(pEventData->pOutput->buf[0].pBuffer), sizeof(void *));

      for (UINT32 i = 0U; i < MAX_OSD_BOX_SIZE; i ++) {
        g_OsdBox[i].Color = pCvAlgoFrcnnOutput->Box[i].Class;
        g_OsdBox[i].X     = pCvAlgoFrcnnOutput->Box[i].Xmin;
        g_OsdBox[i].Y     = pCvAlgoFrcnnOutput->Box[i].Ymin;
        g_OsdBox[i].W     = pCvAlgoFrcnnOutput->Box[i].Xmax - pCvAlgoFrcnnOutput->Box[i].Xmin + 1U;
        g_OsdBox[i].H     = pCvAlgoFrcnnOutput->Box[i].Ymax - pCvAlgoFrcnnOutput->Box[i].Ymin + 1U;
      }

      OsdCfg.BoxNum = pCvAlgoFrcnnOutput->BoxNum;
      OsdCfg.OsdClearFlg = g_FrcnnProcDispCtrl.OsdClearFlg;
      OsdCfg.OsdFlushFlg = g_FrcnnProcDispCtrl.OsdFlushFlg;
      OsdCfg.pOsdBox = g_OsdBox;

      if (OsdCfg.BoxNum > 0U) {
        Rval = ArmOsdSender_DrawBox(OsdCfg);
      }
    }
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FrcnnProcDisplay_Register
 *
 *  @Description:: Register FrcnnProcDisplay callback
 *
 *  @Input      ::
 *    pFrcnnProcDispCfg: Pointer to FrcnnProcDisplay config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 FrcnnProcDisplay_Register(const FRCNN_PROC_DISPLAY_CFG_s *pFrcnnProcDispCfg)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for parameters */
  if (pFrcnnProcDispCfg == NULL) {
    ArmLog_ERR(ARM_LOG_FRCNN_PROC_DISP, "## FrcnnProcDisplay_Register fail (pFrcnnProcDispCfg is null)", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Keep setting to global variable */
  if ((Rval == ARM_OK) && (pFrcnnProcDispCfg != NULL)) {
      g_FrcnnProcDispCtrl.OsdClearFlg = pFrcnnProcDispCfg->OsdClearFlg;
      g_FrcnnProcDispCtrl.OsdFlushFlg = pFrcnnProcDispCfg->OsdFlushFlg;
  }

  /* 3. Register callback */
  if ((Rval == ARM_OK) && (pFrcnnProcDispCfg != NULL)) {
    Rval = CtCvAlgoWrapper_RegCb(pFrcnnProcDispCfg->Slot, 0, FrcnnProcDisplayCallback);
    if (Rval != ARM_OK) {
      ArmLog_ERR(ARM_LOG_FRCNN_PROC_DISP, "## CtCvAlgoWrapper_RegCb fail", 0U, 0U);
      Rval = ARM_NG;
    }
  }

  return Rval;
}
