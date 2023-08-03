/**
 *  @file SvcControlTask.h
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
 *  @details svc vin control task header file
 *
 */

#ifndef SVC_CONTROL_TASK_H
#define SVC_CONTROL_TASK_H

#include "AmbaTypes.h"

#define SVC_CONTROL_TASK_CMD_INIT               ("init")
#define SVC_CONTROL_TASK_CMD_SYSTEM_CFG         ("system_cfg")
#define SVC_CONTROL_TASK_CMD_CMDMODE            ("icam_cmdmode")
#define SVC_CONTROL_TASK_CMD_FORMAT_UPDATE      ("update_format_id")
#define SVC_CONTROL_TASK_CMD_LIVEVIEW           ("icam_liv")
#define SVC_CONTROL_TASK_CMD_LIVEVIEW_STOP      ("icam_liv_stop")
#define SVC_CONTROL_TASK_CMD_PBK_START          ("icam_pbk_start")
#define SVC_CONTROL_TASK_CMD_PBK_STOP           ("icam_pbk_stop")
#define SVC_CONTROL_TASK_CMD_PBK_GUI_ON         ("icam_pbk_gui_on")
#define SVC_CONTROL_TASK_CMD_PBK_GUI_OFF        ("icam_pbk_gui_off")
#define SVC_CONTROL_TASK_CMD_CV_INIT            ("cv_init")
#define SVC_CONTROL_TASK_CMD_CV_CONFIG          ("cv_config")
#define SVC_CONTROL_TASK_CMD_CV_START           ("cv_start")
#define SVC_CONTROL_TASK_CMD_CV_STOP            ("cv_stop")
#define SVC_CONTROL_TASK_CMD_DTL_RELOAD         ("data_reload")
#define SVC_CONTROL_TASK_CMD_DSP_BOOT           ("dspboot")
#define SVC_CONTROL_TASK_CMD_LNX_BOOT           ("lnxboot")
#define SVC_CONTROL_TASK_CMD_VOUT_ON            ("vout_on")
#define SVC_CONTROL_TASK_CMD_PREF_SAVE          ("pref_save")
#define SVC_CONTROL_TASK_CMD_BIST_ON            ("bist_on")
#define SVC_CONTROL_TASK_CMD_REC_START          ("icam_rec_start")
#define SVC_CONTROL_TASK_CMD_REBOOT             ("reboot")
#define SVC_CONTROL_TASK_CMD_DSP_SUSPEND        ("dspsuspend")
#define SVC_CONTROL_TASK_CMD_DSP_RESUME         ("dspresume")
#define SVC_CONTROL_TASK_CMD_SUSPEND_LNX        ("suspend_lnx")
#define SVC_CONTROL_TASK_CMD_DTL_CV_RELOAD      ("cv_reload")
#define SVC_CONTROL_TASK_CMD_CVBS_ADC_TUNE      ("cvbs_adc_tune")
#define SVC_CONTROL_TASK_CMD_NUM                (27)

UINT32 SvcControlTask_CmdInstall(void);
UINT32 SvcControlTask_ProjectEntry(void)   GNU_WEAK_SYMBOL;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
int SvcControlTask_CheckLiveviewSwitchFinish(void);
void SvcControlTask_SetLiveviewSwitchFinish(int finish);
void SvcControlTask_SetCvRun(int run);
#endif

#endif /* SVC_CONTROL_TASK_H */
