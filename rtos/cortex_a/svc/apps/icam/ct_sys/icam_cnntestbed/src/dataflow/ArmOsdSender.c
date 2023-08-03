/**
*  @file ArmOsdSender.c
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
*   @details The arm osd command sender to CamCtrl_ProcessRequest()
*
*/

/* Amba header */
#include "AmbaTypes.h"

/* Arm header */
#include "ArmLog.h"
#include "ArmErrCode.h"
#include "ArmStdC.h"
#include "ArmOsdSender.h"

#include "SvcOsd.h"

#define ARM_LOG_OSD          "ArmUtil_OSD"

#if !defined (CONFIG_SOC_CV28)
#define ARM_OSD_CHANNEL      (1U)     // VOUT_IDX_B (LCD)
#else
#define ARM_OSD_CHANNEL      (0U)     // VOUT_IDX_A (LT9611UXC for CV28)
#endif
#define ARM_OSD_THICKNESS    (3U)

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OsdClear
 *
 *  @Description:: Clear OSD
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void OsdClear(void)
{
  UINT32 Rval;
  Rval = SvcOsd_Clear(ARM_OSD_CHANNEL);
  if (Rval != 0U) {
    ArmLog_ERR(ARM_LOG_OSD, "## SvcOsd_Clear() fail", 0U, 0U);
  }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OsdFlush
 *
 *  @Description:: Flush OSD
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void OsdFlush(void)
{
  UINT32 Rval;
  Rval = SvcOsd_Flush(ARM_OSD_CHANNEL);
  if (Rval != 0U) {
    ArmLog_ERR(ARM_LOG_OSD, "## SvcOsd_Flush() fail", 0U, 0U);
  }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmOsdSender_DrawBox
 *
 *  @Description:: Draw bounding box on OSD
 *
 *  @Input      ::
 *    OsdBoxCfg:   The bounding box config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK ro ARM_NG
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmOsdSender_DrawBox(OSD_DRAW_BOX_CFG_s OsdBoxCfg)
{
  UINT32 Rval = ARM_OK, i = 0U;
  const OSD_BOX_s *pCurBox;

  /* 1. Santiy Check */
  if (OsdBoxCfg.pOsdBox == NULL) {
    ArmLog_ERR(ARM_LOG_OSD, "## ArmOsdSender_DrawBox(): pOsdBox is null", 0U, 0U);
    Rval = ARM_NG;
  }

  if (Rval == ARM_OK) {
    /* 2. Clear OSD */
    if (OsdBoxCfg.OsdClearFlg == 1U) {
      OsdClear();
    }

    /* 3. Draw Rectangle */
    pCurBox = OsdBoxCfg.pOsdBox;
    for (i = 0; i < OsdBoxCfg.BoxNum; i++) {
      Rval = SvcOsd_DrawRect(ARM_OSD_CHANNEL, pCurBox->X, pCurBox->Y, pCurBox->X + pCurBox->W, pCurBox->Y + pCurBox->H,
        pCurBox->Color, ARM_OSD_THICKNESS);

      pCurBox ++;
    }

    /* 4. Flush OSD */
    if (OsdBoxCfg.OsdFlushFlg == 1U) {
      OsdFlush();
    }
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmOsdSender_DrawSeg
 *
 *  @Description:: Draw segmentation result on OSD
 *
 *  @Input      ::
 *    OsdSegCfg:   The segmentation config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK ro ARM_NG
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmOsdSender_DrawSeg(OSD_DRAW_SEG_CFG_s OsdSegCfg)
{
  UINT32 Rval;

  /* 1. Clear OSD */
  if (OsdSegCfg.OsdClearFlg == 1U) {
    OsdClear();
  }

  /* 2. Draw segmentation */
  Rval = SvcOsd_DrawSeg(
      ARM_OSD_CHANNEL,
      OsdSegCfg.pArgMaxOut,
      OsdSegCfg.RoiWidth,
      OsdSegCfg.RoiHeight,
      OsdSegCfg.RoiStartX,
      OsdSegCfg.RoiStartY,
      OsdSegCfg.NetworkWidth,
      OsdSegCfg.NetworkHeight
  );

  /* 3. Flush OSD */
  if (OsdSegCfg.OsdFlushFlg == 1U) {
    OsdFlush();
  }

  return Rval;
}

