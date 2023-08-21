/**
 *  @file SvcControlTask_AdasDvr.c
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
 *  @details svc control task for liveview
 *
 */

#include "AmbaTypes.h"

/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaSensor.h"
#include "AmbaYuv.h"

#if defined(CONFIG_BUILD_COMMON_TUNE_CTUNER)
#include "AmbaCT_EmirTunerIF.h"
#endif

/* svc-framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcPref.h"
/* app-shared */
#include "SvcFlowControl.h"
#include "SvcVinSrc.h"
#include "SvcTiming.h"
#include "SvcOsd.h"
#include "SvcWrap.h"

/* app-icam */
#include "SvcUserPref.h"
#include "SvcMenuTask.h"
#include "SvcControlTask.h"
#include "SvcWarningIconTask.h"
#include "SvcGuiTask.h"
#include "SvcPlaybackTask.h"
#include "SvcRecTask.h"
#ifdef CONFIG_ICAM_REBEL_USAGE
#include "SvcRebelGUI.h"
#endif
#include "SvcSysInfoTask.h"

#include "SvcResCfg.h"
#include "SvcResCfgTask.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvMainTask.h"
#include "SvcCvCtrlTask.h"
#include "SvcODDrawTask.h"
#include "SvcGui.h"
#include "SvcSegDrawTask.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcExtOsdDrawTask.h"
#if defined(CONFIG_ICAM_CV_STEREO)
#include "SvcStereoTask.h"
#include "SvcStixelTask.h"
#include "SvcStereoACTask.h"
#endif
#if defined(CONFIG_ICAM_CV_FEX)
#include "SvcFexTask.h"
#endif
#endif

#if defined(CONFIG_ICAM_CANBUS_USED)
#include "SvcCan.h"
#endif

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
#include "RefFlow_FC.h"
#endif

#include "SvcLogoDraw.h"
#if defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaCalib_AVMIF.h"
#include "AmbaSR_Lane.h"
#include "AmbaAP_LKA.h"
#include "SvcCvCamCtrl.h"
#include "SvcAdasNotify.h"
#include "SvcCan.h"
#include "RefFlow_AutoCal.h"
#include "SvcAdasAutoCal.h"
#include "SvcLdwsTask.h"
#include "SvcLdwsTaskV2.h"
#include "SvcFcwsFcmdTask.h"
#include "SvcFcwsFcmdTaskV2.h"
#include "SvcCalibAdas.h"

#endif
#ifdef CONFIG_ICAM_FW_UPDATER_USED
#include "SvcFwUpdateTask.h"
#endif

#ifdef CONFIG_ICAM_ENABLE_REGRESSION_FLOW
#include "SvcRegressionTask.h"
#endif
#include "SvcVoutFrmCtrlTask.h"


#define INIT_TASK_AUTO_EXEC_SD      (0U)

#define SVC_LOG_CONTROL_TASK        "CTRL_TASK_ADAS_DVR"

#define CONTROL_TASK_CMD_ADAS_NUM       (12U)
#define CONTROL_TASK_CMD_ADAS_CMDMODE   ("cmdmode")
#define CONTROL_TASK_CMD_ADAS_LV        ("liveview")
#define CONTROL_TASK_CMD_ADAS_LV_SWITCH ("liveview_switch")
#define CONTROL_TASK_CMD_CV_RESTART     ("cv_restart")
#define CONTROL_TASK_CMD_ADAS_PBK       ("playback")
#define CONTROL_TASK_CMD_ADAS_GUI_ON    ("adas_dvr_gui_on")
#define CONTROL_TASK_CMD_ADAS_GUI_OFF   ("adas_dvr_gui_off")
#define CONTROL_TASK_CMD_ADAS_FUNC_ON   ("adas_dvr_func_on")
#define CONTROL_TASK_CMD_ADAS_FUNC_OFF  ("adas_dvr_func_off")
#define CONTROL_TASK_CMD_ADAS_SUSPEND   ("suspend")
#define CONTROL_TASK_CMD_ADAS_RESUME    ("resume")
#define CONTROL_TASK_CMD_ADAS_SUSPEND_RSUME ("suspend_resume")

#define MODE_STANDBY       (0U)
#define MODE_LIVEVIEW      (1U)
#define MODE_PLAYBACK      (2U)

static UINT32 ControlTask_AdasDvrCmdMode(void);
static UINT32 ControlTask_AdasDvrLiveview(void);
static UINT32 ControlTask_AdasDvrLiveviewSwitch(void);
static UINT32 ControlTask_AdasDvrCvRestart(void);
static UINT32 ControlTask_AdasDvrPlayback(void);
static UINT32 ControlTask_AdasDvrGuiOn(void);
static UINT32 ControlTask_AdasDvrGuiOff(void);
static UINT32 ControlTask_AdasDvrFuncOn(void);
static UINT32 ControlTask_AdasDvrFuncOff(void);
static UINT32 ControlTask_AdasDvrSuspend(void);
static UINT32 ControlTask_AdasDvrResume(void);
#if defined(CONFIG_ATF_SUSPEND_SRAM)
static UINT32 ControlTask_AdasDvrSuspendResume(void);
#endif

static void   ControlTaskAdasDvrCmdUsage(void);

#ifdef CONFIG_ICAM_THMVIEW_IN_DPX
static UINT32 ControlTask_RebootCheck(UINT32 EnterMode);
#endif

static UINT8 CurrentMode = MODE_STANDBY;

static void OpMode_Enter(void)
{
    extern void AmbaDSP_CmdShowAll(UINT8 On);
    SVC_USER_PREF_s *pSvcUserPref;
    UINT32 OperationMode, RetVal = SVC_OK;

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        OperationMode = pSvcUserPref->OperationMode;
    } else {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "## OpMode_Enter() failed with get preference error", 0U, 0U);
        OperationMode = 0U;
    }

#ifdef CONFIG_ICAM_THMVIEW_IN_DPX
    if (OperationMode == 1U) {
        UINT32 PbkFormatId = 0U;
        if (SVC_OK == SvcResCfgTask_GetPbkModeId(&PbkFormatId)) {
            if (PbkFormatId == pSvcUserPref->FormatId) {
                OperationMode = 2U;
            }
        }
    }
#endif

    /* Control task operation executed */
    if (OperationMode == 0U) {
        RetVal = SvcFlowControl_Exec("cmdmode");
    } else if (OperationMode == 1U) {
        RetVal = SvcFlowControl_Exec("liveview");
    } else {
        RetVal = SvcFlowControl_Exec("playback");
    }

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "## SvcFlowControl_Exec() failed with %d", RetVal, 0U);
    }
}

/**
* The entry function of project (EMR). User can implement their flow control here.
* @return ErrorCode
*/
UINT32 SvcControlTask_ProjectEntry(void)
{
    static SVC_FLOW_CONTROL_CMD_LIST_s SvcControlTaskAdasDvrCmdList[CONTROL_TASK_CMD_ADAS_NUM] = {
        { CONTROL_TASK_CMD_ADAS_CMDMODE,   ControlTask_AdasDvrCmdMode        },
        { CONTROL_TASK_CMD_ADAS_LV,        ControlTask_AdasDvrLiveview       },
        { CONTROL_TASK_CMD_ADAS_LV_SWITCH, ControlTask_AdasDvrLiveviewSwitch },
        { CONTROL_TASK_CMD_ADAS_PBK,       ControlTask_AdasDvrPlayback       },
        { CONTROL_TASK_CMD_ADAS_GUI_ON,    ControlTask_AdasDvrGuiOn          },
        { CONTROL_TASK_CMD_ADAS_GUI_OFF,   ControlTask_AdasDvrGuiOff         },
        { CONTROL_TASK_CMD_ADAS_FUNC_ON,   ControlTask_AdasDvrFuncOn         },
        { CONTROL_TASK_CMD_ADAS_FUNC_OFF,  ControlTask_AdasDvrFuncOff        },
        { CONTROL_TASK_CMD_ADAS_SUSPEND,   ControlTask_AdasDvrSuspend        },
        { CONTROL_TASK_CMD_ADAS_RESUME,    ControlTask_AdasDvrResume         },
        { CONTROL_TASK_CMD_CV_RESTART,     ControlTask_AdasDvrCvRestart      },
#if defined(CONFIG_ATF_SUSPEND_SRAM)
        { CONTROL_TASK_CMD_ADAS_SUSPEND_RSUME,  ControlTask_AdasDvrSuspendResume  },
#else
        { "", NULL },   // misra-c
#endif
    };

    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() start", 0U, 0U);

    RetVal = SvcFlowControl_CmdHook(SvcControlTaskAdasDvrCmdList, CONTROL_TASK_CMD_ADAS_NUM, ControlTaskAdasDvrCmdUsage);

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() done", 0U, 0U);
    }

#if INIT_TASK_AUTO_EXEC_SD
    if (SVC_OK == RetVal) {
        RetVal = SvcFlowControl_Exec("system_cfg");
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "## SvcFlowControl_Exec(\"system_cfg\") failed with %d", RetVal, 0U);
        }
    }
#endif

    if (SVC_OK == RetVal) {
        OpMode_Enter();
    }

    return RetVal;
}

/**
* Control task for entering Cmdmode.
*/
static UINT32 ControlTask_AdasDvrCmdMode(void)
{
    UINT32 RetVal = SVC_OK;
    RetVal |= SvcFlowControl_Exec("icam_cmdmode");
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    RetVal |= SvcFlowControl_Exec("lnxboot");
#endif

    return RetVal;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static int cv_run = 0;
void SvcControlTask_SetCvRun(int run)
{
    cv_run = run;
}
#endif

/**
* Control task for entering Liveview with ADAS.
* @return ErrorCode
*/
#ifndef CONFIG_ICAM_THMVIEW_IN_DPX
static UINT32 ControlTask_AdasDvrLiveview(void)
{
    UINT32        RetVal = SVC_OK;

    if (CurrentMode != MODE_LIVEVIEW) {
        if (CurrentMode == MODE_PLAYBACK) {
            RetVal |= SvcFlowControl_Exec("icam_pbk_gui_off");
            RetVal |= SvcFlowControl_Exec("icam_pbk_stop");
        }

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        if (cv_run) {
#endif
        /* Config CV to allow preload CV binaries early */
        RetVal |= SvcFlowControl_Exec("cv_config");
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        }
#endif

        /* boot2rec should only be triggered from standby */
        if (CurrentMode == MODE_STANDBY) {
            RetVal |= SvcFlowControl_Exec("icam_rec_start");
        }

        RetVal |= SvcFlowControl_Exec("icam_liv");

        /* dsp/lnx should be booted only once */
        if (CurrentMode == MODE_STANDBY) {
            RetVal |= SvcFlowControl_Exec("dspboot");
            RetVal |= SvcFlowControl_Exec("bist_on");
            RetVal |= SvcFlowControl_Exec("lnxboot");
        }

        /* Might be switched from playback or standby */
        if ((CurrentMode == MODE_PLAYBACK) || (CurrentMode == MODE_STANDBY)) {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            if (cv_run) {
#endif
            RetVal |= SvcFlowControl_Exec("cv_start");
            RetVal |= SvcFlowControl_Exec("adas_dvr_gui_on");
            RetVal |= SvcFlowControl_Exec("adas_dvr_func_on");
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            }
#endif
            RetVal |= SvcFlowControl_Exec("cvbs_adc_tune");
            RetVal |= SvcFlowControl_Exec("vout_on");
        }
        RetVal |= SvcFlowControl_Exec("pref_save");
    } else {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "Already in Liveview mode", 0U, 0U);
    }

    CurrentMode = MODE_LIVEVIEW;

    return RetVal;
}
#else
static UINT32 ControlTask_AdasDvrLiveview(void)
{
    UINT32 RetVal = SVC_OK, Reboot;

    Reboot = ControlTask_RebootCheck(MODE_LIVEVIEW);

    if (CurrentMode == MODE_STANDBY) {
        RetVal |= SvcFlowControl_Exec("cv_config");
        RetVal |= SvcFlowControl_Exec("icam_rec_start");
        RetVal |= SvcFlowControl_Exec("icam_liv");
        RetVal |= SvcFlowControl_Exec("dspboot");
        RetVal |= SvcFlowControl_Exec("bist_on");
        RetVal |= SvcFlowControl_Exec("lnxboot");
        RetVal |= SvcFlowControl_Exec("cv_start");
        RetVal |= SvcFlowControl_Exec("adas_dvr_gui_on");
        RetVal |= SvcFlowControl_Exec("adas_dvr_func_on");
        RetVal |= SvcFlowControl_Exec("vout_on");
        RetVal |= SvcFlowControl_Exec("pref_save");
    } else if (CurrentMode == MODE_PLAYBACK) {
        if (Reboot == 0U) {
            RetVal |= SvcFlowControl_Exec("cv_stop");
            RetVal |= SvcFlowControl_Exec("icam_pbk_gui_off");
            RetVal |= SvcFlowControl_Exec("icam_liv_stop");
            RetVal |= SvcFlowControl_Exec("update_format_id");
            RetVal |= SvcFlowControl_Exec("data_reload");
            RetVal |= SvcFlowControl_Exec("cv_config");
            RetVal |= SvcFlowControl_Exec("icam_liv");
            RetVal |= SvcFlowControl_Exec("cv_start");
            RetVal |= SvcFlowControl_Exec("adas_dvr_gui_on");
            RetVal |= SvcFlowControl_Exec("adas_dvr_func_on");
            RetVal |= SvcFlowControl_Exec("vout_on");
            RetVal |= SvcFlowControl_Exec("pref_save");
        } else {
            RetVal |= SvcFlowControl_Exec("pref_save");
            RetVal |= SvcFlowControl_Exec("reboot");
        }
    } else {
        /* already in liveview mode */
    }

    CurrentMode = MODE_LIVEVIEW;

    return RetVal;
}
#endif

/**
* Control task for Switch Liveview with ADAS.
* @return ErrorCode
*/
extern void SvcBuffer_CV_Init(void);
static UINT32 ControlTask_AdasDvrLiveviewSwitch(void)
{
    UINT32 RetVal = SVC_OK;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (cv_run) {
        RetVal |= SvcFlowControl_Exec("adas_dvr_func_off");
        RetVal |= SvcFlowControl_Exec("cv_stop");
        RetVal |= SvcFlowControl_Exec("icam_liv_stop");
        RetVal |= SvcFlowControl_Exec("update_format_id");
        RetVal |= SvcFlowControl_Exec("data_reload");
        SvcBuffer_CV_Init();
        RetVal |= SvcFlowControl_Exec("cv_config");
        RetVal |= SvcFlowControl_Exec("icam_liv");
        RetVal |= SvcFlowControl_Exec("cv_start");
        RetVal |= SvcFlowControl_Exec("adas_dvr_func_on");
        RetVal |= SvcFlowControl_Exec("vout_on");
    } else {
        RetVal |= SvcFlowControl_Exec("icam_liv_stop");
        RetVal |= SvcFlowControl_Exec("update_format_id");
        RetVal |= SvcFlowControl_Exec("data_reload");
        RetVal |= SvcFlowControl_Exec("icam_liv");
        RetVal |= SvcFlowControl_Exec("vout_on");
    }
#else
    RetVal |= SvcFlowControl_Exec("adas_dvr_func_off");
    RetVal |= SvcFlowControl_Exec("cv_stop");
    RetVal |= SvcFlowControl_Exec("icam_liv_stop");
    RetVal |= SvcFlowControl_Exec("update_format_id");
    RetVal |= SvcFlowControl_Exec("data_reload");
    SvcBuffer_CV_Init();
    RetVal |= SvcFlowControl_Exec("cv_config");
    RetVal |= SvcFlowControl_Exec("icam_liv");
    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("adas_dvr_func_on");
    RetVal |= SvcFlowControl_Exec("vout_on");
#endif

    return RetVal;
}


static UINT32 ControlTask_AdasDvrCvRestart(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec("adas_dvr_func_off");
    RetVal |= SvcFlowControl_Exec("cv_stop");
    RetVal |= SvcFlowControl_Exec("cv_reload");
    SvcBuffer_CV_Init();
    RetVal |= SvcFlowControl_Exec("cv_config");
    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("adas_dvr_func_on");

    return RetVal;
}

/**
* Control task for entering Liveview with Playback
* @return ErrorCode
*/
#ifndef CONFIG_ICAM_THMVIEW_IN_DPX
static UINT32 ControlTask_AdasDvrPlayback(void)
{
    UINT32 RetVal = SVC_OK;

    if (CurrentMode != MODE_PLAYBACK) {
        if (CurrentMode == MODE_LIVEVIEW) {
            RetVal |= SvcFlowControl_Exec("adas_dvr_func_off");
            RetVal |= SvcFlowControl_Exec("adas_dvr_gui_off");
            RetVal |= SvcFlowControl_Exec("cv_stop");
            RetVal |= SvcFlowControl_Exec("icam_liv_stop");
        }

        /* Might be switched from liveview or standby */
        if ((CurrentMode == MODE_LIVEVIEW) || (CurrentMode == MODE_STANDBY)) {
            RetVal |= SvcFlowControl_Exec("icam_pbk_start");
        }

        /* dsp/lnx should be booted only once */
        if (CurrentMode == MODE_STANDBY) {
            RetVal |= SvcFlowControl_Exec("dspboot");
        }

        /* Might be switched from liveview or liveview */
        if ((CurrentMode == MODE_LIVEVIEW) || (CurrentMode == MODE_STANDBY)) {
            RetVal |= SvcFlowControl_Exec("vout_on");
            RetVal |= SvcFlowControl_Exec("icam_pbk_gui_on");
        }
        RetVal |= SvcFlowControl_Exec("pref_save");
    } else {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "Already in Playback mode", 0U, 0U);
    }

    CurrentMode = MODE_PLAYBACK;

    return RetVal;
}
#else
static UINT32 ControlTask_AdasDvrPlayback(void)
{
    UINT32 RetVal = SVC_OK, Reboot;

    Reboot = ControlTask_RebootCheck(MODE_PLAYBACK);

    if (CurrentMode == MODE_STANDBY) {
        /* Config CV to allow preload CV binaries early */
        RetVal |= SvcFlowControl_Exec("update_format_id");
        RetVal |= SvcFlowControl_Exec("cv_config");
        RetVal |= SvcFlowControl_Exec("icam_liv");
        RetVal |= SvcFlowControl_Exec("dspboot");
        RetVal |= SvcFlowControl_Exec("bist_on");
        RetVal |= SvcFlowControl_Exec("lnxboot");
        RetVal |= SvcFlowControl_Exec("cv_start");
        RetVal |= SvcFlowControl_Exec("icam_pbk_gui_on");
        RetVal |= SvcFlowControl_Exec("vout_on");
        RetVal |= SvcFlowControl_Exec("pref_save");
    } else if (CurrentMode == MODE_LIVEVIEW) {
        if (Reboot == 0U) {
            RetVal |= SvcFlowControl_Exec("adas_dvr_func_off");
            RetVal |= SvcFlowControl_Exec("adas_dvr_gui_off");
            RetVal |= SvcFlowControl_Exec("cv_stop");
            RetVal |= SvcFlowControl_Exec("icam_liv_stop");
            RetVal |= SvcFlowControl_Exec("update_format_id");
            RetVal |= SvcFlowControl_Exec("data_reload");
            RetVal |= SvcFlowControl_Exec("cv_config");
            RetVal |= SvcFlowControl_Exec("icam_liv");
            RetVal |= SvcFlowControl_Exec("cv_start");
            RetVal |= SvcFlowControl_Exec("icam_pbk_gui_on");
            RetVal |= SvcFlowControl_Exec("vout_on");
            RetVal |= SvcFlowControl_Exec("pref_save");
        } else {
            RetVal |= SvcFlowControl_Exec("pref_save");
            RetVal |= SvcFlowControl_Exec("reboot");
        }
    } else {
        /* already in playback mode */
    }

    CurrentMode = MODE_PLAYBACK;

    return RetVal;
}
#endif

static UINT32 ControlTask_AdasDvrSuspend(void)
{
    UINT32 RetVal = SVC_OK;

#ifdef CONFIG_ICAM_DSP_SUSPEND
    if (CurrentMode == MODE_LIVEVIEW) {
        RetVal |= SvcFlowControl_Exec("adas_dvr_func_off");
        RetVal |= SvcFlowControl_Exec("cv_stop");
        RetVal |= SvcFlowControl_Exec("icam_liv_stop");
        RetVal |= SvcFlowControl_Exec("dspsuspend");
    } else if (CurrentMode == MODE_PLAYBACK) {
    #ifdef CONFIG_ICAM_THMVIEW_IN_DPX
        RetVal |= SvcFlowControl_Exec("icam_pbk_gui_off");
        RetVal |= SvcFlowControl_Exec("cv_stop");
        RetVal |= SvcFlowControl_Exec("icam_liv_stop");
        RetVal |= SvcFlowControl_Exec("dspsuspend");
    #endif
    } else {
        /* nothing */
    }
#endif

    return RetVal;
}

static UINT32 ControlTask_AdasDvrResume(void)
{
    UINT32 RetVal = SVC_OK;

#ifdef CONFIG_ICAM_DSP_SUSPEND
    if (CurrentMode == MODE_LIVEVIEW) {
        RetVal |= SvcFlowControl_Exec("dspresume");
        RetVal |= SvcFlowControl_Exec("cv_reload");
        RetVal |= SvcFlowControl_Exec("cv_config");
        RetVal |= SvcFlowControl_Exec("icam_liv");
        RetVal |= SvcFlowControl_Exec("dspboot");
        RetVal |= SvcFlowControl_Exec("cv_start");
        RetVal |= SvcFlowControl_Exec("adas_dvr_func_on");
        RetVal |= SvcFlowControl_Exec("vout_on");
    } else if (CurrentMode == MODE_PLAYBACK) {
    #ifdef CONFIG_ICAM_THMVIEW_IN_DPX
        RetVal |= SvcFlowControl_Exec("dspresume");
        RetVal |= SvcFlowControl_Exec("cv_config");
        RetVal |= SvcFlowControl_Exec("icam_liv");
        RetVal |= SvcFlowControl_Exec("dspboot");
        RetVal |= SvcFlowControl_Exec("cv_start");
        RetVal |= SvcFlowControl_Exec("icam_pbk_gui_on");
        RetVal |= SvcFlowControl_Exec("vout_on");
    #endif
    } else {
        /* nothing */
    }
#endif

    return RetVal;
}

#if defined(CONFIG_ATF_SUSPEND_SRAM)
static UINT32 ControlTask_AdasDvrSuspendResume(void)
{
    UINT32 RetVal = SVC_OK;

    SvcCvMainTask_Ctrl("schdr_reset", NULL);
//    RetVal |= SvcFlowControl_Exec("icam_rec_stop");
    RetVal |= SvcFlowControl_Exec("adas_dvr_func_off");
    RetVal |= SvcFlowControl_Exec("cv_stop");
    RetVal |= SvcFlowControl_Exec("icam_liv_stop");
    RetVal |= SvcFlowControl_Exec("dspsuspend");
    RetVal |= SvcFlowControl_Exec("suspend_lnx");
    RetVal |= SvcFlowControl_Exec("dspresume");
    RetVal |= SvcFlowControl_Exec("cv_reload");
    RetVal |= SvcFlowControl_Exec("cv_config");
    RetVal |= SvcFlowControl_Exec("icam_liv");
    RetVal |= SvcFlowControl_Exec("dspboot");
//    RetVal |= SvcFlowControl_Exec("icam_rec_start");
    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("adas_dvr_func_on");

    return RetVal;
}
#endif

/**
* Control task for ADAS function. The function is used to enable all ADAS GUI
* @return ErrorCode
*/
static UINT32 ControlTask_AdasDvrGuiOn(void)
{
    UINT32 RetVal = SVC_OK;
    static UINT8 IsInit = 0U;

#ifdef CONFIG_ICAM_GUI_MENU_USED
    /* Enable the menu when ADAS is enable */
    RetVal  = SvcMenuTask_Config(SVC_MENU_TASK_TYPE_LV_ADAS_DVR);
    RetVal |= SvcMenuTask_Load();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcMenuTask_Config() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#ifdef CONFIG_ICAM_FW_UPDATER_USED
    SvcFwUpdateTask_GuiType(SVC_MENU_TASK_TYPE_LV_ADAS_DVR);
#endif

    if (IsInit == 0U) {

#ifdef CONFIG_ICAM_32BITS_OSD_USED
        /* Draw Amba Logo */
#ifdef CONFIG_ICAM_REBEL_USAGE
        SvcLogoDraw_Init(VOUT_IDX_A);
#else
        SvcLogoDraw_Init(VOUT_IDX_B);
#endif
        SvcLogoDraw_Update();
#endif

#ifdef CONFIG_BUILD_CV
        /* Initialize OD (object detection) task */
        RetVal = SvcODDrawTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }

        /* Configure OD (object detection) task */
        RetVal = SvcODDrawTask_Config(0);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODDrawTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }

        /* Initialize Segmentation draw task */
        RetVal = SvcSegDrawTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSegDrawTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }

        /* Initialize ext osd draw task */
        RetVal = SvcExtOsdDrawTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcExtOsdDrawTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }

#if defined(CONFIG_ICAM_CANBUS_USED)
    SvcCan_Init();
#ifdef CONFIG_ICAM_REBEL_USAGE
    SvcRebelGUI_DrawInit();

    RetVal = SvcRebelGUI_UpdatGUIeInfo();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRebelGUI_UpdatGUIeInfo() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    } else {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcRebelGUI_UpdatGUIeInfo() OK", 0U, 0U);
    }
#endif
#endif

        /* Initialize LDWS task */
#if defined(CONFIG_BUILD_AMBA_ADAS)
        if (SVC_CALIB_ADAS_VERSION == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
            RetVal = SvcAdasAutoCalTask_Init();
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAdasAutoCalTask_Init() failed with %d", RetVal, 0U);
                SvcFlowControl_Assert();
            } else {
                SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcAdasAutoCalTask_Init() OK", 0U, 0U);
            }
        } else {
            //SVC_CALIB_ADAS_VERSION_V1 not do AutoCal
        }

        RetVal = SvcLdwsTask_Init();
        RetVal |= SvcLdwsTaskV2_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcLdwsTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        } else {
            SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcLdwsTask_Init() OK", 0U, 0U);
        }

        SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT) = %d",
                                        SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT), 0U);

        /* TV OSD ADAS msg draw initialization */
        SvcAdasNotify_DrawInit();

        RetVal = SvcFcTaskV2_Init();
        RetVal |= SvcFcTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFcTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        } else {
            SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFcTask_Init() OK", 0U, 0U);
        }

        RetVal = SvcAdasNotify_DrawSpeed();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAdasNotify_DrawSpeed() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        } else {
            SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcAdasNotify_DrawSpeed() OK", 0U, 0U);
        }

        RetVal = SvcAdasNotify_DrawFcInfo();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAdasNotify_DrawFcInfo() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        } else {
            SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcAdasNotify_DrawFcInfo() OK", 0U, 0U);
        }

        RetVal = SvcAdasNotify_LdwsDistR();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAdasNotify_LdwsDistR() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        } else {
            SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcAdasNotify_LdwsDistR() OK", 0U, 0U);
        }

        RetVal = SvcAdasNotify_LdwsDistL();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAdasNotify_LdwsDistL() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        } else {
            SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcAdasNotify_LdwsDistL() OK", 0U, 0U);
        }

        /* Warning icon is used by Fc function */
        RetVal = SvcWarnIconTask_Config(SVC_WARN_ICON_TYPE_ADAS);
        if (SVC_OK != RetVal) {
            SvcLog_DBG(SVC_LOG_CONTROL_TASK, "SvcWarnIconTask_Config() failed %d", 0U, 0U);
            RetVal = SVC_OK;
        }

#endif

#if defined(CONFIG_ICAM_CV_STEREO)
        /* Initialize stereo task */
        RetVal = SvcStereoTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStereoTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }

        RetVal = SvcStereoTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStereoTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
#endif

#if (defined(CONFIG_ICAM_CV_STIXEL) ||  defined(CONFIG_ICAM_CV_LINUX_STEREO_APP))

        /* Initialize stixel task */
        RetVal = SvcStixelTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStixelTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }

        RetVal = SvcStixelTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStixelTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
#endif

#if (defined(CONFIG_ICAM_CV_STEREO_AUTO_CALIB) ||  defined(CONFIG_ICAM_CV_LINUX_STEREO_APP))

        /* Initialize stixel task */
        RetVal = SvcStereoACTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStereoACTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }

        RetVal = SvcStereoACTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStereoACTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
#endif

#if defined(CONFIG_ICAM_CV_FEX)
        /* Initialize FEX task */
        RetVal = SvcFexTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFexTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }

        RetVal = SvcFexTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFexTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
#endif
#endif

#if defined(CONFIG_ICAM_PLAYBACK_USED)
        RetVal = SvcPlaybackTask_DuplexStart();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcPlaybackTask_DuplexStart() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
#endif

        IsInit = 1U;
    }

    RetVal = SvcGuiTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcGuiTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    return RetVal;
}

/**
* Control task for ADAS function. The function is used to disable all ADAS GUI
* @return ErrorCode
*/
static UINT32 ControlTask_AdasDvrGuiOff(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal = SvcGuiTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcGuiTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    return RetVal;
}

/**
* Control task for ADAS function. The function is used to enable all ADAS function
* @return ErrorCode
*/
static UINT32 ControlTask_AdasDvrFuncOn(void)
{
UINT32 RetVal = SVC_OK;
#ifdef CONFIG_BUILD_CV
    /* Wait CV Src Ready to start other related CV application */
    RetVal = SvcCvMainTask_WaitSrcReady();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvMainTask_WaitSrcReady() failed with %d", RetVal , 0U);
        SvcFlowControl_Assert();
    }

#if defined(CONFIG_ICAM_TIMING_LOG)
    /* time stamp for FlexiDag boot start */
    SvcTime(SVC_TIME_CV_FLEXIDAG_BOOT_START, "CV FlexiDag boot start");
#endif

    /* Start CV channels */
    RetVal = SvcCvCtrlTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvCtrlTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

#if defined CONFIG_ICAM_VOUT_FRAME_CTRL_AUTO_START
    {
        SVC_VOUT_FRM_CTRL_CFG_s Cfg = {0};

       if (SVC_OK != SvcVoutFrmCtrlTask_Init()) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutFrmCtrlTask_Init failed", 0U, 0U);
        }

        Cfg.YuvStrmIdx = CONFIG_ICAM_VOUT_FRAME_CTRL_STREAM_IDX;
        Cfg.DlyCount = CONFIG_ICAM_VOUT_FRAME_CTRL_DLY_COUNT;
        Cfg.SyncMode = CONFIG_ICAM_VOUT_FRAME_CTRL_SYNC_MODE;
        RetVal = SvcVoutFrmCtrlTask_Config(&Cfg);
        RetVal |= SvcVoutFrmCtrlTask_Start();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutFrmCtrlTask_Start() failed", 0U , 0U);
        }
    }
#endif

    /* Enable Object Detection module */
    RetVal = SvcODDrawTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODDrawTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

#if defined(CONFIG_BUILD_AMBA_ADAS)
{
    UINT32  FcRval, SvcODDrawRval;

    /* Need to return calibration version or don't launch FC */
    if (SVC_OK == SvcAdasNotify_CalibCheck()) {
        SvcODDrawRval = SvcODDrawTask_Enable(0U);
        if (SvcODDrawRval != SVC_OK) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "## fail to do SvcODDrawTask_Enable() (%u)", SvcODDrawRval, 0U);
        }
        if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
            FcRval = SvcFcTaskV2_Start();
        } else { //SVC_CALIB_ADAS_VERSION
            FcRval = SvcFcTask_Start();
        }
        if (FcRval != SVC_OK) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "## fail to do SvcFcTask_Start() (%u)", FcRval, 0U);
        }
    }


    /* Init Warning Zone AutoTune task entry */
    SvcAdasNotify_WarningZoneAutoTuneInit();

}
#endif

    /* Enable Seg Draw */
    RetVal = SvcSegDrawTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSegDrawTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Enable Ext Draw */
    RetVal = SvcExtOsdDrawTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcExtOsdDrawTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

#if defined(CONFIG_ICAM_CV_STEREO)
    /* Start stereo task */
    RetVal = SvcStereoTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStereoTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if (defined(CONFIG_ICAM_CV_STIXEL) ||  defined(CONFIG_ICAM_CV_LINUX_STEREO_APP))

    /* Start stixel task */
    RetVal = SvcStixelTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStixelTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if (defined(CONFIG_ICAM_CV_STEREO_AUTO_CALIB) ||  defined(CONFIG_ICAM_CV_LINUX_STEREO_APP))
    /* Start stereo auto calibration task */
    RetVal = SvcStereoACTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStereoACTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_ICAM_CV_FEX)
    /* Start FEX task */
    RetVal = SvcFexTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFexTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#ifdef CONFIG_ICAM_ENABLE_REGRESSION_FLOW
    /* Enable Regression Flow */
    RetVal = SvcCvRegression_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvRegression_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#endif

#ifdef CONFIG_ICAM_SYS_INFO_TASK
    RetVal = SvcSysInfoTask_Start(SVC_SYS_INFO_ALL);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSysInfoTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

#ifdef CONFIG_ICAM_ENABLE_SYS_INFO_OSD
    RetVal = SvcSysInfoTask_DispEnable(SVC_SYS_INFO_ALL);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSysInfoTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
#endif

    return RetVal;
}

/**
* Control task for ADAS function. The function is used to disable all ADAS function
* @return ErrorCode
*/
static UINT32 ControlTask_AdasDvrFuncOff(void)
{
    UINT32 RetVal = SVC_OK;
#ifdef CONFIG_BUILD_CV
    /* Disable Object Detection module */
    RetVal = SvcODDrawTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODDrawTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Disable Seg Draw */
    RetVal = SvcSegDrawTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSegDrawTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Disable Ext Draw */
    RetVal = SvcExtOsdDrawTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcExtOsdDrawTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_ICAM_RECORD_USED)
    /* stop MV record streams if they are enabled */
    SvcRecTask_StopMV();
#endif

#ifdef CONFIG_ICAM_SYS_INFO_TASK
    RetVal = SvcSysInfoTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSysInfoTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

    return RetVal;

}

#ifdef CONFIG_ICAM_THMVIEW_IN_DPX
static UINT32 ControlTask_RebootCheck(UINT32 EnterMode)
{
    UINT32                PbkFormatId = 0U, ResCfgNum = 0U, Err, Reboot = 1U, RetVal = SVC_OK;
    static UINT32         CurrentLiveviewId = 0U;
    SVC_USER_PREF_s       *pSvcUserPref = NULL;
    const SVC_RES_CFG_s   *ResCfgArr = NULL;

    AmbaMisra_TouchUnused(&RetVal);

    if (RetVal == SVC_OK) {
        Err = SvcResCfgTask_GetPbkModeId(&PbkFormatId);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcResCfgTask_GetPbkModeId failed %u", Err, 0U);
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        Err = SvcUserPref_Get(&pSvcUserPref);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "get preference failed %u", Err, 0U);
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        SvcResCfgTask_GetCfgArr(&ResCfgArr, &ResCfgNum);
    }

    if ((RetVal == SVC_OK) && (pSvcUserPref != NULL) && (ResCfgArr != NULL)) {
        if (CurrentMode == MODE_PLAYBACK) { /* pbk to liveview */
            pSvcUserPref->FormatId = CurrentLiveviewId;

            if (0 != SvcWrap_strcmp(ResCfgArr[CurrentLiveviewId].GroupName, "")) {
                if (0 == SvcWrap_strcmp(ResCfgArr[CurrentLiveviewId].GroupName, ResCfgArr[PbkFormatId].GroupName)) {
                    Reboot = 0U;
                }
            }
        } else if (CurrentMode == MODE_LIVEVIEW) { /* liveview to pbk */
            CurrentLiveviewId      = pSvcUserPref->FormatId;

            if (0 != SvcWrap_strcmp(ResCfgArr[PbkFormatId].GroupName, "")) {
                if (0 == SvcWrap_strcmp(ResCfgArr[PbkFormatId].GroupName, ResCfgArr[CurrentLiveviewId].GroupName)) {
                    Reboot = 0U;
                }
            }

            pSvcUserPref->FormatId = PbkFormatId;
        } else { /* from cmd mode */
            Reboot = 0U;

            if (EnterMode == MODE_PLAYBACK) {
                pSvcUserPref->FormatId = PbkFormatId;
            } else if (EnterMode == MODE_LIVEVIEW) {
                if (pSvcUserPref->FormatId == PbkFormatId) {
                    pSvcUserPref->FormatId = CurrentLiveviewId;
                }
            } else {
                /* nothing */
            }
        }
    }

    if (RetVal != SVC_OK) {
        Reboot = 0U;
    }

    return Reboot;
}
#endif

/**
* Show the ControlTask cmd usage
* @return ErrorCode
*/
static void ControlTaskAdasDvrCmdUsage(void)
{
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "Cmd Usage (ADAS_DVR):", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           liveview: flow control for entering ADAS_DVR project liveview", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           playback: flow control for entering ADAS_DVR project playback", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           adas_dvr: flow control for project ADAS_DVR", 0U, 0U);
}
