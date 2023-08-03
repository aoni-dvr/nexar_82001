/**
 *  @file SvcControlTask_Dms.c
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
 *  @details svc control task for DMS
 *
 */
#include ".svc_autogen"
#include "AmbaTypes.h"

/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaSensor.h"
#include "AmbaYuv.h"

/* svc-framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcPref.h"
/* app-shared */
#include "SvcFlowControl.h"
#include "SvcVinSrc.h"
/* app-icam */
#include "SvcUserPref.h"
#include "SvcMenuTask.h"
#include "SvcControlTask.h"
#include "SvcGuiTask.h"
#include "SvcPlaybackTask.h"
#include "SvcRecTask.h"
#include "SvcOsd.h"
#include "SvcLogoDraw.h"


#if defined(CONFIG_BUILD_CV)
#include "SvcCvMainTask.h"
#include "SvcCvCtrlTask.h"
#include "SvcODDrawTask.h"
#include "SvcGui.h"
#include "SvcSegDrawTask.h"
#include "SvcExtOsdDrawTask.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#endif
#if defined(CONFIG_BUILD_AMBA_ADAS)
#include "SvcCan.h"
#include "SvcBsdTask.h"
#endif

#ifdef CONFIG_ICAM_FW_UPDATER_USED
#include "SvcFwUpdateTask.h"
#endif

#define INIT_TASK_AUTO_EXEC_SD      (0U)

#define SVC_LOG_CONTROL_TASK        "CTRL_TASK_DMS"

#define CONTROL_TASK_CMD_DMS_NUM        (7U)
#define CONTROL_TASK_CMD_DMS_CMDMODE    ("cmdmode")
#define CONTROL_TASK_CMD_DMS_LV         ("liveview")
#define CONTROL_TASK_CMD_DMS_LV_SWITCH  ("liveview_switch")
#define CONTROL_TASK_CMD_DMS_PBK        ("playback")
#define CONTROL_TASK_CMD_DMS_GUI        ("dms_gui")
#define CONTROL_TASK_CMD_DMS_FUNC_ON    ("dms_func_on")
#define CONTROL_TASK_CMD_DMS_FUNC_OFF   ("dms_func_off")

static UINT32 ControlTask_DmsCmdMode(void);
static UINT32 ControlTask_DmsLiveview(void);
static UINT32 ControlTask_DmsLiveviewSwitch(void);
static UINT32 ControlTask_DmsPlayback(void);
static UINT32 ControlTask_DmsGui(void);
static UINT32 ControlTask_DmsFuncOn(void);
static UINT32 ControlTask_DmsFuncOff(void);

static void   ControlTaskDmsCmdUsage(void);

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
* The entry function of project (DMS). User can implement their flow control here.
* @return ErrorCode
*/
UINT32 SvcControlTask_ProjectEntry(void)
{
    static SVC_FLOW_CONTROL_CMD_LIST_s SvcControlTaskDmsCmdList[CONTROL_TASK_CMD_DMS_NUM] = {
        { CONTROL_TASK_CMD_DMS_CMDMODE,   ControlTask_DmsCmdMode        },
        { CONTROL_TASK_CMD_DMS_LV,        ControlTask_DmsLiveview       },
        { CONTROL_TASK_CMD_DMS_LV_SWITCH, ControlTask_DmsLiveviewSwitch },
        { CONTROL_TASK_CMD_DMS_PBK,       ControlTask_DmsPlayback       },
        { CONTROL_TASK_CMD_DMS_GUI,       ControlTask_DmsGui            },
        { CONTROL_TASK_CMD_DMS_FUNC_ON,   ControlTask_DmsFuncOn         },
        { CONTROL_TASK_CMD_DMS_FUNC_OFF,  ControlTask_DmsFuncOff        },
    };

    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() start", 0U, 0U);

    RetVal = SvcFlowControl_CmdHook(SvcControlTaskDmsCmdList, CONTROL_TASK_CMD_DMS_NUM, ControlTaskDmsCmdUsage);

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
static UINT32 ControlTask_DmsCmdMode(void)
{
    UINT32 RetVal = SVC_OK;
    RetVal |= SvcFlowControl_Exec("icam_cmdmode");

    return RetVal;
}

/**
* Control task for entering Liveview with ADAS.
*/
static UINT32 ControlTask_DmsLiveview(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec("cv_config");
    RetVal |= SvcFlowControl_Exec("icam_liv");
    RetVal |= SvcFlowControl_Exec("dspboot");
    RetVal |= SvcFlowControl_Exec("bist_on");
    RetVal |= SvcFlowControl_Exec("lnxboot");
    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("dms_gui");
    RetVal |= SvcFlowControl_Exec("dms_func_on");
    RetVal |= SvcFlowControl_Exec("vout_on");
    RetVal |= SvcFlowControl_Exec("pref_save");

    return RetVal;
}

/**
* Control task for Switch Liveview with ADAS.
* @return ErrorCode
*/
static UINT32 ControlTask_DmsLiveviewSwitch(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec("dms_func_off");
    RetVal |= SvcFlowControl_Exec("cv_stop");
    RetVal |= SvcFlowControl_Exec("icam_liv_stop");
    RetVal |= SvcFlowControl_Exec("update_format_id");
    RetVal |= SvcFlowControl_Exec("data_reload");
    RetVal |= SvcFlowControl_Exec("cv_config");
    RetVal |= SvcFlowControl_Exec("icam_liv");
    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("dms_func_on");
    RetVal |= SvcFlowControl_Exec("vout_on");

    return RetVal;
}

/**
* Control task for entering Liveview with Playback
* @return ErrorCode
*/
static UINT32 ControlTask_DmsPlayback(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec("icam_pbk");
    RetVal |= SvcFlowControl_Exec("dspboot");
    RetVal |= SvcFlowControl_Exec("icam_pbk_gui");
    RetVal |= SvcFlowControl_Exec("vout_on");
    RetVal |= SvcFlowControl_Exec("pref_save");

    return RetVal;
}

/**
* Control task for Dms function. The function is used to enable all DMS GUI
* @return ErrorCode
*/
static UINT32 ControlTask_DmsGui(void)
{
    UINT32 RetVal = SVC_OK;

#ifdef CONFIG_ICAM_GUI_MENU_USED
    /* Enable the menu */
    RetVal  = SvcMenuTask_Config(SVC_MENU_TASK_TYPE_LV_DMS);
    RetVal |= SvcMenuTask_Load();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcMenuTask_Config() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#ifdef CONFIG_ICAM_FW_UPDATER_USED
    SvcFwUpdateTask_GuiType(SVC_MENU_TASK_TYPE_LV_DMS);
#endif

#if defined(CONFIG_BUILD_CV)

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

    /* Initialize ext osd draw task */
    RetVal = SvcExtOsdDrawTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcExtOsdDrawTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

    /* Draw Amba Logo */
    SvcLogoDraw_Init(VOUT_IDX_B);
    SvcLogoDraw_Update();

#if defined(CONFIG_ICAM_PLAYBACK_USED)
    RetVal = SvcPlaybackTask_DuplexStart();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcPlaybackTask_DuplexStart() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

    RetVal = SvcGuiTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcGuiTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    return RetVal;
}

/**
* Control task for Dms function. The function is used to enable all DMS function
* @return ErrorCode
*/
static UINT32 ControlTask_DmsFuncOn(void)
{
    UINT32 RetVal = SVC_OK;

#ifdef CONFIG_BUILD_CV
    /* Wait CV Src Ready to start other related CV application */
    RetVal = SvcCvMainTask_WaitSrcReady();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvMainTask_WaitSrcReady() failed with %d", RetVal , 0U);
        SvcFlowControl_Assert();
    }

    /* Start CV channels */
    RetVal = SvcCvCtrlTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvCtrlTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Enable Object Detection module */
    RetVal = SvcODDrawTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODDrawTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Enable Ext Draw */
    RetVal = SvcExtOsdDrawTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcExtOsdDrawTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Set Ext Draw Mode */
    RetVal = SvcExtOsd_SetMode(OSD_DMS_DEMO_MODE);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcExtOsd_SetMode() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_ICAM_RECORD_USED)
    /* start MV record streams if they are enabled */
    SvcRecTask_StartMV();
#endif

#if defined(CONFIG_ICAM_DMS_FUNC_AUTO_EXEC)
    {   /* Record Function */
        RetVal |= SvcRecTask_SetStreamBits(0x1U);    /* Only First stream will be recorded */
        RetVal |= SvcRecTask_Start();

        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRecTask_Start() failed with %d", RetVal, 0U);
        }
    }
#if defined(CONFIG_ICAM_PLAYBACK_USED)
    {   /* Playback Function */
        UINT32 FileIdx = 0U;
        SvcPlaybackTask_LoopPbkStart(1UL, &FileIdx, 0U);
    }
#endif
#endif

    return RetVal;
}

/**
* Control task for Dms function. The function is used to disable all DMS function
* @return ErrorCode
*/
static UINT32 ControlTask_DmsFuncOff(void)
{
#if defined(CONFIG_ICAM_RECORD_USED)
    /* stop MV record streams if they are enabled */
    SvcRecTask_StopMV();
#endif

    return SVC_OK;
}

/**
* Show the ControlTask cmd usage
* @return ErrorCode
*/
static void ControlTaskDmsCmdUsage(void)
{
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "Cmd Usage (DMS):", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           liveview: flow control for entering DMS project liveview", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           playback: flow control for entering DMS project playback", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           dms: flow control for project DMS", 0U, 0U);
}
