/**
*  @file SvcRebelGUI.h
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
*  @details Svc SvcRebel GUI header file
*
*/

#ifndef SVC_Rebel_GUI_H
#define SVC_Rebel_GUI_H

#define SVC_Rebel_GUI_DRAW_LANE      (1U)
#define SVC_Rebel_GUI_DRAW_MSG       (5U)

#ifndef CONFIG_SVC_LVGL_USED 
#define SVC_Rebel_GUI_CHAR_W         (30U)
#define GUI_CHAR_OFFSET              (0U)
#define SVC_Rebel_GUI_CHAR_H         (40U)
#else
#define SVC_Rebel_GUI_CHAR_W         (40U)
#define GUI_CHAR_OFFSET              (20U)
#define SVC_Rebel_GUI_CHAR_H         (40U)
#endif

typedef struct {
    UINT32 StartW;
    UINT32 StartH;
} SVC_Rebel_GUI_DRAW_s;

UINT32 SvcRebelGUI_UpdatGUIeInfo(void);
void SvcRebelGUI_DrawInit(void);
void SvcRebelGUI_TuneLineEnable(UINT8 Enable);

#endif  /* SVC_Rebel_GUI_H */
