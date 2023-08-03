/**
 *  @file SvcVoutCtrlTask.h
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
 *  @details svc vout ctrl task header file
 *
 */

#ifndef SVC_VOUT_CTRL_TASK_H
#define SVC_VOUT_CTRL_TASK_H

#include "AmbaTypes.h"

#define SVC_VOUT_CTRL_SRC_INIT_DONE (1)

#include ".svc_autogen"

#if defined(SVC_BOARD_DK) && defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII)
#if !defined(CONFIG_SOC_CV2)
#define SVC_ENABLE_BACKLIGHT_CONTROL 1
#else
#define SVC_ENABLE_BACKLIGHT_CONTROL 0  /* CV2 not supported yet */
#endif
#else
#define SVC_ENABLE_BACKLIGHT_CONTROL 0
#endif

UINT32 SvcVoutCtrlTask_Init(void);
UINT32 SvcVoutCtrlTask_Config(void);
UINT32 SvcVoutCtrlTask_Destroy(void);
UINT32 SvcVoutCtrlTask_WaitSrcReady(void);
#if SVC_ENABLE_BACKLIGHT_CONTROL
UINT32 SvcVoutCtrlTask_SetBacklight(UINT32 PanelID, UINT32 Period, UINT32 OnPeriod);
UINT32 SvcVoutCtrlTask_SetBacklightIdx(UINT32 PanelID, INT32 BlIdxInc);
void   SvcVoutCtrlTask_SetAllBacklight(void);
#endif

#endif /* SVC_VOUT_CTRL_TASK_H */