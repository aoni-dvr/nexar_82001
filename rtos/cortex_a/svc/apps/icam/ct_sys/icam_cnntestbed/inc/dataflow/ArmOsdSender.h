/**
*  @file ArmOsdSender.h
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

#ifndef ARM_OSD_SENDER_H
#define ARM_OSD_SENDER_H

#define MAX_DRAW_STR_LEN    (8)

typedef struct {
  UINT32 Color;
  UINT32 X;
  UINT32 Y;
  UINT32 W;
  UINT32 H;
  char   Str[MAX_DRAW_STR_LEN];
} OSD_BOX_s;

typedef struct {
  UINT32      OsdFlushFlg;
  UINT32      OsdClearFlg;
  UINT32      BoxNum;
  OSD_BOX_s   *pOsdBox;
} OSD_DRAW_BOX_CFG_s;

typedef struct {
  UINT32 OsdFlushFlg;
  UINT32 OsdClearFlg;

  UINT32 RoiWidth;
  UINT32 RoiHeight;
  UINT32 RoiStartX;
  UINT32 RoiStartY;
  UINT32 NetworkWidth;
  UINT32 NetworkHeight;
  UINT8  *pArgMaxOut;
} OSD_DRAW_SEG_CFG_s;

/******************************************************************************
 *  Defined in ArmOsdSender.c
 ******************************************************************************/
UINT32 ArmOsdSender_DrawBox(OSD_DRAW_BOX_CFG_s OsdBoxCfg);
UINT32 ArmOsdSender_DrawSeg(OSD_DRAW_SEG_CFG_s OsdSegCfg);

#endif /* ARM_OSD_SENDER_H */

