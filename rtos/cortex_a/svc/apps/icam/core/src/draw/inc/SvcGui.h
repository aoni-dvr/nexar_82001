/**
*  @file SvcGui.h
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
*  @details svc application gui draw
*
*/

#ifndef SVC_GUI_H
#define SVC_GUI_H

#include "AmbaDSP_EventInfo.h"
#include "AmbaShell.h"

#define SVC_GUI_LEVEL_MAX_NUM (64U)
#define SVC_GUI_EVENT_OSD_FLUSH (1U)

typedef struct /* _SVC_GUI_CANVAS_s_ */ {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 Width;
    UINT32 Height;
} SVC_GUI_CANVAS_s;

typedef enum /* _SVC_GUI_TYPE_e_ */ {
    SVC_GUI_TYPE_TIMER = 0L,
    SVC_GUI_TYPE_VOUT,
    SVC_GUI_TYPE_POLARITY,
    SVC_GUI_TYPE_TOTAL
} SVC_GUI_TYPE_e;

typedef struct /* _SVC_GUI_SETTING_s_ */ {
    UINT8                  Type;
    AMBA_DSP_FRAME_RATE_s  VoutFrameRate;   /* Take effect only when Type == SVC_GUI_TYPE_VOUT */
    AMBA_DSP_FRAME_RATE_s  MaxFrameRate;    /* GUI updating frame rate */
} SVC_GUI_SETTING_s;

typedef void (* SVC_GUI_DRAW_FUNC)(UINT32 VoutIdx, UINT32 Level);
typedef void (* SVC_GUI_UPDATE_FUNC)(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
typedef void (* SVC_GUI_EVENT_FUNC)(UINT32 VoutIdx, UINT32 Level, UINT32 EventID, void *pEvent);

UINT32 SvcGui_Init(void);
UINT32 SvcGui_Start(UINT32 VoutIdx, const SVC_GUI_SETTING_s *pConfig);
UINT32 SvcGui_Stop(UINT32 VoutIdx);
void   SvcGui_Register(UINT32 VoutIdx, UINT32 Level, const char *pName, SVC_GUI_DRAW_FUNC pDrawFunc, SVC_GUI_UPDATE_FUNC pUpdateFunc);
void   SvcGui_Unregister(UINT32 VoutIdx, UINT32 Level);
void   SvcGui_EventRegister(UINT32 VoutIdx, UINT32 Level, SVC_GUI_EVENT_FUNC pEventFunc);

void   SvcGui_DebugEnable(UINT32 DebugEnable);
#ifndef CONFIG_ICAM_32BITS_OSD_USED
void   SvcGuiPalette_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#endif

#endif  /* SVC_GUI_H */
