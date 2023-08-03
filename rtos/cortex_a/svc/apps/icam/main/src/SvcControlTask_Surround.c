/**
 *  @file SvcControlTask_Surround.c
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
#include "SvcWarningIconTask.h"
#include "SvcGuiTask.h"
#include "SvcPlaybackTask.h"
#include "SvcRecTask.h"
#include "SvcGui.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvMainTask.h"
#include "SvcCvCtrlTask.h"
#include "SvcODDrawTask.h"
#include "SvcSegDrawTask.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcExtOsdDrawTask.h"
#include "SvcSurDrawTask.h"
#include "SvcPsdTask.h"
#include "SvcOwsTask.h"
#include "RefFlow_RCTA.h"
#include "SvcRctaTask.h"
#endif
#include "AmbaCalib_AVMIF.h"
#include "AmbaCT_AvmTunerIF.h"
#include "SvcAnimCalib.h"
#include "SvcLogoDraw.h"

#ifdef CONFIG_ICAM_FW_UPDATER_USED
#include "SvcFwUpdateTask.h"
#endif

#define INIT_TASK_AUTO_EXEC_SD      (0U)

#define SVC_LOG_CONTROL_TASK        "CTRL_TASK_SUR"

#define CONTROL_TASK_CMD_SUR_NUM       (7U)
#define CONTROL_TASK_CMD_SUR_CMDMODE   ("cmdmode")
#define CONTROL_TASK_CMD_SUR_LV        ("liveview")
#define CONTROL_TASK_CMD_SUR_LV_SWITCH ("liveview_switch")
#define CONTROL_TASK_CMD_SUR_GUI_ON    ("sur_gui_on")
#define CONTROL_TASK_CMD_SUR_GUI_OFF   ("sur_gui_off")
#define CONTROL_TASK_CMD_SUR_FUNC_ON   ("sur_func_on")
#define CONTROL_TASK_CMD_SUR_FUNC_OFF  ("sur_func_off")

static UINT32 ControlTask_SurCmdMode(void);
static UINT32 ControlTask_SurroundLiveview(void);
static UINT32 ControlTask_SurroundLiveviewSwitch(void);
static UINT32 ControlTask_SurroundGuiOn(void);
static UINT32 ControlTask_SurroundGuiOff(void);
static UINT32 ControlTask_SurroundFuncOn(void);
static UINT32 ControlTask_SurroundFuncOff(void);

static void   ControlTaskSurroundCmdUsage(void);

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
        RetVal = SvcFlowControl_Exec("icam_cmdmode");
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
    static SVC_FLOW_CONTROL_CMD_LIST_s SvcControlTaskSurroundCmdList[CONTROL_TASK_CMD_SUR_NUM] = {
        { CONTROL_TASK_CMD_SUR_CMDMODE,   ControlTask_SurCmdMode        },
        { CONTROL_TASK_CMD_SUR_LV,        ControlTask_SurroundLiveview       },
        { CONTROL_TASK_CMD_SUR_LV_SWITCH, ControlTask_SurroundLiveviewSwitch },
        { CONTROL_TASK_CMD_SUR_GUI_ON,    ControlTask_SurroundGuiOn          },
        { CONTROL_TASK_CMD_SUR_GUI_OFF,   ControlTask_SurroundGuiOff         },
        { CONTROL_TASK_CMD_SUR_FUNC_ON,   ControlTask_SurroundFuncOn         },
        { CONTROL_TASK_CMD_SUR_FUNC_OFF,  ControlTask_SurroundFuncOff        },
    };

    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() start", 0U, 0U);

    RetVal = SvcFlowControl_CmdHook(SvcControlTaskSurroundCmdList, CONTROL_TASK_CMD_SUR_NUM, ControlTaskSurroundCmdUsage);

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
static UINT32 ControlTask_SurCmdMode(void)
{
    UINT32 RetVal = SVC_OK;
    RetVal |= SvcFlowControl_Exec("icam_cmdmode");

    return RetVal;
}

/**
* Control task for entering Liveview with ADAS.
* @return ErrorCode
*/
static UINT32 ControlTask_SurroundLiveview(void)
{
    UINT32        RetVal = SVC_OK;
    static UINT8  InitCV = 0U;

    RetVal |= SvcFlowControl_Exec("icam_liv");
    RetVal |= SvcFlowControl_Exec("dspboot");
    RetVal |= SvcFlowControl_Exec("bist_on");
    RetVal |= SvcFlowControl_Exec("lnxboot");

    if (InitCV == 0U) {
        RetVal |= SvcFlowControl_Exec("cv_config");
        InitCV = 1U;
    }

    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("sur_gui_on");
    RetVal |= SvcFlowControl_Exec("sur_func_on");

    RetVal |= SvcFlowControl_Exec("vout_on");
    RetVal |= SvcFlowControl_Exec("pref_save");

    return RetVal;
}

/**
* Control task for Switch Liveview with ADAS.
* @return ErrorCode
*/
static UINT32 ControlTask_SurroundLiveviewSwitch(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec("sur_func_off");
    RetVal |= SvcFlowControl_Exec("cv_stop");
    RetVal |= SvcFlowControl_Exec("icam_liv_stop");
    RetVal |= SvcFlowControl_Exec("update_format_id");
    RetVal |= SvcFlowControl_Exec("data_reload");
    RetVal |= SvcFlowControl_Exec("cv_config");
    RetVal |= SvcFlowControl_Exec("icam_liv");
    RetVal |= SvcFlowControl_Exec("bist_on");
    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("sur_func_on");
    RetVal |= SvcFlowControl_Exec("vout_on");

    return RetVal;
}

/**
* Control task for ADAS function. The function is used to enable all ADAS GUI
* @return ErrorCode
*/
static UINT32 ControlTask_SurroundGuiOn(void)
{
    UINT32 RetVal = SVC_OK;
    static UINT8 IsInit = 0U;

    RetVal = SvcGuiTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcGuiTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

#ifdef CONFIG_ICAM_GUI_MENU_USED
    /* Enable the menu when ADAS is enable */
    RetVal  = SvcMenuTask_Config(SVC_MENU_TASK_TYPE_SURROUND);
    RetVal |= SvcMenuTask_Load();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcMenuTask_Config() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#ifdef CONFIG_ICAM_FW_UPDATER_USED
    SvcFwUpdateTask_GuiType(SVC_MENU_TASK_TYPE_SURROUND);
#endif

    /* Draw Amba Logo */
    SvcLogoDraw_Init(VOUT_IDX_B);
    SvcLogoDraw_Update();

    if (IsInit == 0U) {
#ifdef CONFIG_BUILD_CV
        /* Initialize OD (object detection) task */
        /*RetVal = SvcODDrawTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }*/

        /* Configure OD (object detection) task */
        /*RetVal = SvcODDrawTask_Config(0);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODDrawTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }*/

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

        /* Initialize avm adas osd draw task */
        RetVal = SvcSurDrawTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAvmDrawTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }

#endif
        IsInit = 1U;
    }

    return RetVal;
}

/**
* Control task for ADAS function. The function is used to disable all ADAS GUI
* @return ErrorCode
*/
static UINT32 ControlTask_SurroundGuiOff(void)
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
static UINT32 ControlTask_SurroundFuncOn(void)
{
UINT32 RetVal = SVC_OK;
#ifdef CONFIG_BUILD_CV
    {
        extern void SvcSurTask_Init(void);
        SvcSurTask_Init();
    }

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
    /*RetVal = SvcODDrawTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODDrawTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }*/

    /* Enable Ext Draw */
    RetVal = SvcExtOsdDrawTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcExtOsdDrawTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Enable sur adas Draw */
    RetVal = SvcSurDrawTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAvmDrawTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }


    /* Inital PSD */
    RetVal = SvcPsdTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcPsdTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Inital OWS */
    RetVal = SvcOwsTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcOwsTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Inital RCTA */
    RetVal = SvcRctaTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRctaTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Enable Psd */
    RetVal = SvcPsdTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcPsdTask_Start() failed with %d", RetVal, 0U);
    }

    /* Enable OWS */
    RetVal = SvcOwsTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcOwsTask_Start() failed with %d", RetVal, 0U);
    }

    /* Enable RCTA */
    RetVal = SvcRctaTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRctaTask_Start() failed with %d", RetVal, 0U);
    }

#endif


#if defined(CONFIG_ICAM_RECORD_USED)
    /* start MV record streams if they are enabled */
    SvcRecTask_StartMV();
#endif

    return RetVal;
}

/**
* Control task for ADAS function. The function is used to disable all ADAS function
* @return ErrorCode
*/
static UINT32 ControlTask_SurroundFuncOff(void)
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

    return RetVal;

}

/**
* Show the ControlTask cmd usage
* @return ErrorCode
*/
static void ControlTaskSurroundCmdUsage(void)
{
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "Cmd Usage (SUR):", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           liveview: flow control for entering SUR project liveview", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           sur: flow control for project ADAS_DVR", 0U, 0U);
}

