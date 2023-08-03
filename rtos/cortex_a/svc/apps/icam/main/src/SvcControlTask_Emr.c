/**
 *  @file SvcControlTask_Emr.c
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
 *  @details svc control task for emirror
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
#include "SvcWarningIconTask.h"
#include "SvcGuiTask.h"
#include "SvcColorBalance.h"
#include "SvcRecTask.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvMainTask.h"
#include "SvcCvCtrlTask.h"
#include "SvcODDrawTask.h"
#include "SvcGui.h"
#include "SvcSegDrawTask.h"
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
#include "AmbaOD_2DBbx.h"
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaWS_BSD.h"
#include "SvcRmgTask.h"
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
#include "SvcEmrAdaptiveTask.h"
#endif

#if defined(CONFIG_ICAM_CANBUS_USED)
#include "AmbaSurround.h"
#include "SvcCan.h"
#endif

#ifdef CONFIG_ICAM_FW_UPDATER_USED
#include "SvcFwUpdateTask.h"
#endif

#define INIT_TASK_AUTO_EXEC_SD      (0U)

#define SVC_LOG_CONTROL_TASK        "CTRL_TASK_EMR"

#define CONTROL_TASK_CMD_EMR_NUM        (7U)
#define CONTROL_TASK_CMD_EMR_CMDMODE    ("cmdmode")
#define CONTROL_TASK_CMD_EMR_LV         ("liveview")
#define CONTROL_TASK_CMD_EMR_LV_SWITCH  ("liveview_switch")
#define CONTROL_TASK_CMD_EMR_PBK        ("playback")
#define CONTROL_TASK_CMD_EMR_GUI        ("emr_gui")
#define CONTROL_TASK_CMD_EMR_FUNC_ON    ("emr_func_on")
#define CONTROL_TASK_CMD_EMR_FUNC_OFF   ("emr_func_off")

static UINT32 ControlTask_EmrCmdMode(void);
static UINT32 ControlTask_EmrLiveview(void);
static UINT32 ControlTask_EmrLiveviewSwitch(void);
static UINT32 ControlTask_EmrPlayback(void);
static UINT32 ControlTask_EmrGui(void);
static UINT32 ControlTask_EmrFuncOn(void);
static UINT32 ControlTask_EmrFuncOff(void);

static void   ControlTaskEmrCmdUsage(void);

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
* The entry function of project (EMR). User can implement their flow control here.
* @return ErrorCode
*/
UINT32 SvcControlTask_ProjectEntry(void)
{
    static SVC_FLOW_CONTROL_CMD_LIST_s SvcControlTaskEmrCmdList[CONTROL_TASK_CMD_EMR_NUM] = {
        { CONTROL_TASK_CMD_EMR_CMDMODE,   ControlTask_EmrCmdMode        },
        { CONTROL_TASK_CMD_EMR_LV,        ControlTask_EmrLiveview       },
        { CONTROL_TASK_CMD_EMR_LV_SWITCH, ControlTask_EmrLiveviewSwitch },
        { CONTROL_TASK_CMD_EMR_PBK,       ControlTask_EmrPlayback       },
        { CONTROL_TASK_CMD_EMR_GUI,       ControlTask_EmrGui            },
        { CONTROL_TASK_CMD_EMR_FUNC_ON,   ControlTask_EmrFuncOn         },
        { CONTROL_TASK_CMD_EMR_FUNC_OFF,  ControlTask_EmrFuncOff        },
    };

    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() start", 0U, 0U);

    RetVal = SvcFlowControl_CmdHook(SvcControlTaskEmrCmdList, CONTROL_TASK_CMD_EMR_NUM, ControlTaskEmrCmdUsage);

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
static UINT32 ControlTask_EmrCmdMode(void)
{
    UINT32 RetVal = SVC_OK;
    RetVal |= SvcFlowControl_Exec("icam_cmdmode");

    return RetVal;
}

/**
* Control task for entering Liveview with ADAS.
* @return ErrorCode
*/
static UINT32 ControlTask_EmrLiveview(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec("cv_config");
    RetVal |= SvcFlowControl_Exec("icam_rec_start");
    RetVal |= SvcFlowControl_Exec("icam_liv");
    RetVal |= SvcFlowControl_Exec("dspboot");
    RetVal |= SvcFlowControl_Exec("bist_on");
    RetVal |= SvcFlowControl_Exec("lnxboot");
    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("emr_gui");
    RetVal |= SvcFlowControl_Exec("emr_func_on");
    RetVal |= SvcFlowControl_Exec("vout_on");
    RetVal |= SvcFlowControl_Exec("pref_save");

    return RetVal;
}

/**
* Control task for Switch Liveview with ADAS.
* @return ErrorCode
*/
static UINT32 ControlTask_EmrLiveviewSwitch(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec("emr_func_off");
    RetVal |= SvcFlowControl_Exec("cv_stop");
    RetVal |= SvcFlowControl_Exec("icam_liv_stop");
    RetVal |= SvcFlowControl_Exec("update_format_id");
    RetVal |= SvcFlowControl_Exec("data_reload");
    RetVal |= SvcFlowControl_Exec("cv_config");
    RetVal |= SvcFlowControl_Exec("icam_liv");
    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("emr_func_on");
    RetVal |= SvcFlowControl_Exec("vout_on");

    return RetVal;
}

/**
* Control task for entering Liveview with Playback
* @return ErrorCode
*/
static UINT32 ControlTask_EmrPlayback(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec("icam_pbk_start");
    RetVal |= SvcFlowControl_Exec("dspboot");
    RetVal |= SvcFlowControl_Exec("vout_on");
    RetVal |= SvcFlowControl_Exec("icam_pbk_gui_on");
    RetVal |= SvcFlowControl_Exec("pref_save");

    return RetVal;
}

/**
* Control task for Emr function. The function is used to enable all emr Gui
* @return ErrorCode
*/
static UINT32 ControlTask_EmrGui(void)
{
    UINT32 RetVal = SVC_OK;

#ifdef CONFIG_ICAM_GUI_MENU_USED
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
    RetVal = SvcBSDTask_IsLiveView();
    if(SVC_OK == RetVal) {
        /* Enable the menu */
        RetVal  = SvcMenuTask_Config(SVC_MENU_TASK_TYPE_LV_EMR);
        RetVal |= SvcMenuTask_Load();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcMenuTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#else
    /* Enable the menu */
    RetVal  = SvcMenuTask_Config(SVC_MENU_TASK_TYPE_LV_EMR);
    RetVal |= SvcMenuTask_Load();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcMenuTask_Config() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
#endif

#ifdef CONFIG_ICAM_FW_UPDATER_USED
    SvcFwUpdateTask_GuiType(SVC_MENU_TASK_TYPE_LV_EMR);
#endif

#ifdef CONFIG_BUILD_CV
    /* Initialize OD (object detection) task */
    RetVal = SvcODDrawTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Configure OD (object detection) task */
    RetVal = SvcODDrawTask_Config(1);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODDrawTask_Config() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#if defined(CONFIG_ICAM_CANBUS_USED)
    SvcCan_Init();
    {
        SVC_USER_PREF_s  *pSvcUserPref;

        if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
            extern AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData;
            DOUBLE CanSpeed;

            if(pSvcUserPref->CanSpeed != 0U) {
                CanSpeed = (DOUBLE)(pSvcUserPref->CanSpeed)/(0.0073725);
                CanbusRawData.RawSpeed = (UINT32)CanSpeed + 1U;
                CanbusRawData.FlagValidRawData = 1U;
            }
        }
    }
#endif

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
    {
        UINT8 Enable;
        (void)SvcBsdTask_GetStatus(&Enable);
        if (Enable == 1U) {

            /* Enable BSD module */
            if (SVC_OK == RetVal) {
                /* Initialize BSD task */
                RetVal = SvcBsdTask_Init();
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcBsdTask_Init() failed with %d", RetVal, 0U);
                    SvcFlowControl_Assert();
                }
            }
        }
    }

    RetVal = SvcRmgTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRmgTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
#endif

    RetVal = SvcGuiTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcGuiTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    return RetVal;
}

/**
* Control task for Emr function. The function is used to enable all emr function
* @return ErrorCode
*/
static UINT32 ControlTask_EmrFuncOn(void)
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

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
    /* Enable BSD module */
    {
        UINT8 Enable;
        (void)SvcBsdTask_GetStatus(&Enable);
        if (Enable == 1U) {
            RetVal = SvcODDrawTask_Enable(FALSE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODDrawTask_Enable failed", 0U, 0U);
            }

            RetVal = SvcBsdTask_Start();
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcBsdTask_Start() failed with %d", RetVal, 0U);
                SvcFlowControl_Assert();
            }
        }
    }

    RetVal = SvcRmgTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRmgTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
    RetVal = SvcEmrAdaptiveTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcEmrAdaptiveTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    /* Start Color Balance Task */
    RetVal = SvcColorBalance_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcColorBalance_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_ICAM_RECORD_USED)
    /* start MV record streams if they are enabled */
    SvcRecTask_StartMV();
#endif

    return RetVal;
}

/**
* Control task for Emr function. The function is used to disable all emr function
* @return ErrorCode
*/
static UINT32 ControlTask_EmrFuncOff(void)
{
    UINT32 RetVal = SVC_OK;

#ifdef CONFIG_BUILD_CV
    /* Disable Object Detection module */
    RetVal = SvcODDrawTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcODDrawTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
    /* Disable BSD module */
    {
        UINT8 Enable;
        (void)SvcBsdTask_GetStatus(&Enable);
        if (Enable == 1U) {
            RetVal = SvcBsdTask_Stop();
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcBsdTask_Stop() failed with %d", RetVal, 0U);
                SvcFlowControl_Assert();
            }
        }
    }
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
    RetVal = SvcEmrAdaptiveTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcEmrAdaptiveTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    RetVal = SvcColorBalance_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcColorBalance_Stop() failed with %d", RetVal, 0U);
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
static void ControlTaskEmrCmdUsage(void)
{
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "Cmd Usage (EMR):", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           liveview: flow control for entering EMR project liveview", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           playback: flow control for entering EMR project playback", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           emr: flow control for project EMR", 0U, 0U);
}
