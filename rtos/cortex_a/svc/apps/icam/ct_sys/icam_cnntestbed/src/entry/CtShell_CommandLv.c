/**
*  @file CtShell_CommandLv.c
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
*   @details The cnn_testbed lv related shell commands
*
*/

#include "CtShell_CommandCv.h"

/* ArmUtil header */
#include "ArmLog.h"
#include "ArmStdC.h"
#include "ArmErrCode.h"
#include "ArmFIO.h"
#include "ArmTask.h"

#include "SvcFlowControl.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "IdspFeeder.h"

#ifdef CONFIG_ENABLE_AMBALINK
#include "AmbaIPC_FlexidagIO.h"
#endif

#define ARM_LOG_LV_CMD        "LvCmd"

#define CT_PYMD_YUV_BUF_NUM        (4U)

//#define CT_ENABLE_DUMP_Y_CMD

#ifdef CT_ENABLE_DUMP_Y_CMD
UINT32 DumpBuf[3904*2160];
UINT32 DumpFlag  = 0U;
UINT32 DumpScale = 0U;
UINT32 DumpSize  = 0U;
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LvPyramidRdyHandler
 *
 *  @Description:: The dsp event handler for pyramid data ready info
 *
 *  @Input      ::
 *    EventInfo:   The pointer to pyramid data ready info
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *    UINT32:      CT_OK(0)/CT_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LvPyramidRdyHandler(const void *EventInfo)
{
  UINT32                          Rval = ARM_OK;
  UINT32                          i = 0U;
  static UINT8                    idx = 0U;
  static AMBA_DSP_YUV_IMG_BUF_s   YuvImgBuf[CT_PYMD_YUV_BUF_NUM][AMBA_DSP_MAX_HIER_NUM];
  static AMBA_DSP_WINDOW_s        Window[CT_PYMD_YUV_BUF_NUM][AMBA_DSP_MAX_HIER_NUM];
  const AMBA_DSP_PYMD_DATA_RDY_s  *pYuvInfo = NULL;
  static UINT8 IsPrint = 0U;

  AmbaMisra_TypeCast(&pYuvInfo, &EventInfo);

  if (pYuvInfo != NULL) {
    FEED_MQ_ITEM_s MqItem;

    MqItem.CapPts      = pYuvInfo->CapPts;
    MqItem.CapSequence = pYuvInfo->CapSequence;
    MqItem.YuvImgBuf   = &YuvImgBuf[idx][0U];
    MqItem.Window      = &Window[idx][0U];

    for (i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
      YuvImgBuf[idx][i].Pitch      = pYuvInfo->YuvBuf[i].Pitch;
      YuvImgBuf[idx][i].BaseAddrY  = pYuvInfo->YuvBuf[i].BaseAddrY;
      YuvImgBuf[idx][i].BaseAddrUV = pYuvInfo->YuvBuf[i].BaseAddrUV;
      Window[idx][i]               = pYuvInfo->YuvBuf[i].Window;
    }

    if (IsPrint == 0U) {
      ArmLog_DBG(ARM_LOG_LV_CMD, "==============================================", 0U, 0U);
      for (i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
        ArmLog_DBG(ARM_LOG_LV_CMD, "PyramidYuvInfo[%u]: Pitch = %u", i, pYuvInfo->YuvBuf[i].Pitch);
        ArmLog_DBG(ARM_LOG_LV_CMD, "                    Width = %u Height = %u", pYuvInfo->YuvBuf[i].Window.Width, pYuvInfo->YuvBuf[i].Window.Height);
      }
      ArmLog_DBG(ARM_LOG_LV_CMD, "==============================================", 0U, 0U);

      IsPrint = 1U;
    }

#ifdef CT_ENABLE_DUMP_Y_CMD
    if (DumpFlag != 0U) {
      DumpSize = pYuvInfo->YuvBuf[DumpScale].Pitch*pYuvInfo->YuvBuf[DumpScale].Window.Height;
      ArmStdC_memcpy(DumpBuf, (void *)pYuvInfo->YuvBuf[DumpScale].BaseAddrY, DumpSize);
      ArmLog_DBG(ARM_LOG_LV_CMD, "DUMP Y - Pitch = %d, Height = %d", pYuvInfo->YuvBuf[DumpScale].Pitch, pYuvInfo->YuvBuf[DumpScale].Window.Height);

      DumpFlag = 0U;
    }
#endif

    Rval = IdspFeeder_Feed(&MqItem);
    if (Rval != ARM_OK) {
      Rval = ARM_NG;
    } else {
      if ((idx + 1U) == CT_PYMD_YUV_BUF_NUM) {
        idx = 0U;
      } else {
        idx += 1U;
      }
    }
  } else {
    ArmLog_ERR(ARM_LOG_LV_CMD, "## YuvInfo is NULL", 0U, 0U);
    Rval = ARM_NG;
  }

  return Rval;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LvCmdUsage
 *
 *  @Description:: The usage of Lv test command
 *
 *  @Input      ::
 *    PrintFunc:   The pointer of print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void LvCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
  PrintFunc("Lv test commands:\n");
  PrintFunc("    start\n");
#ifdef CT_ENABLE_DUMP_Y_CMD
  PrintFunc("    dump [Scale]\n");
  PrintFunc("       [Scale] - The pyramid scale to dump\n");
#endif

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: CtShell_CommandLv
 *
 *  @Description:: The cnn_testbed lv test command
 *
 *  @Input      ::
 *    ArgCount:    The arg number
 *    pArgVector:  The pointer of arg
 *    PrintFunc:   The pointer of print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
void CtShell_CommandLv(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
  if (ArgCount < 2U) {
    LvCmdUsage(PrintFunc);
  } else {

    /* start */
    if (((ArmStdC_strcmp(pArgVector[1], "start") == 0)) && (ArgCount == 2U)) {
      UINT32 Rval = 0U;

      Rval |= SvcFlowControl_Exec("icam_liv");

      Rval |= SvcFlowControl_Exec("dspboot");

      Rval |= SvcFlowControl_Exec("vout_on");

      if (Rval == 0U) {
        Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_PYRAMID_RDY, LvPyramidRdyHandler);
        if (Rval != 0U) {
          ArmLog_ERR(ARM_LOG_LV_CMD, "## fail to register dsp event handler LvPyramidRdyHandler, rval(%u)", Rval, 0U);
        } else {
          ArmLog_OK(ARM_LOG_LV_CMD, "boot liveview and register dsp event successfully", 0U, 0U);
        }
      } else {
        ArmLog_ERR(ARM_LOG_LV_CMD, "## liveview start fail", 0U, 0U);
      }

    // We move the FlexidagIO IPC init to here to keep this path co-exist with CvFlow Driver
#ifdef CONFIG_ENABLE_AMBALINK
    /* Init FlexidagIO IPC channel 0 */
    Rval = AmbaIPC_FlexidagIO_Init(0U /*Channel*/, NULL);
    if (FLEXIDAGIO_OK != Rval) {
      ArmLog_ERR(ARM_LOG_LV_CMD, "## fail to AmbaIPC_FlexidagIO_Init (ch = 0)", 0U, 0U);
    } else {
      ArmLog_DBG(ARM_LOG_LV_CMD, "#### AmbaIPC_FlexidagIO init done (ch = 0)", 0U, 0U);
    }
#endif

#ifdef CT_ENABLE_DUMP_Y_CMD
    } else if ((ArmStdC_strcmp(pArgVector[1], "dump") == 0) && (ArgCount == 3U)) {
      UINT32 NumByteAccess;

      (void) ArmStdC_strtoul(pArgVector[2],&DumpScale);
      DumpFlag = 1U;

      (void) ArmTask_Sleep(100);
      (void) ArmFIO_Save(DumpBuf, DumpSize, "c:\\DUMP.y", &NumByteAccess);
      ArmLog_DBG(ARM_LOG_LV_CMD, "save to c:\\DUMP.y", 0U, 0U);
#endif
    } else {
      LvCmdUsage(PrintFunc);
    }
  }
  return;
}

