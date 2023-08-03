/**
 *  @file SvcControlTask_CnnTestbed.c
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
 *  @details svc control task for cnn_testbed
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
#include "SvcControlTask.h"

#include "CtSysInit.h"

#if defined(CONFIG_BUILD_CV)
#include "SvcCvAlgo.h"
#include "SvcCvMainTask.h"
#include "SvcCvCtrlTask.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlow_CnnTestbed.h"
#endif

#define INIT_TASK_AUTO_EXEC_SD          (0U)

#define SVC_LOG_CONTROL_TASK            "CTRL_TASK_CNNTESTBED"

#define CONTROL_TASK_CMD_CT_NUM        (4U)
#define CONTROL_TASK_CMD_CT_CMDMODE    ("cmdmode")
#define CONTROL_TASK_CMD_CT_LV         ("liveview")
#define CONTROL_TASK_CMD_CT_SYS_INIT   ("ct_sys_init")
#define CONTROL_TASK_CMD_CT_CV_ON      ("ct_cv_on")


static UINT32 ControlTask_CtCmdMode(void);
static UINT32 ControlTask_CtSysInit(void);
static UINT32 ControlTask_CtLiveview(void);
static UINT32 ControlTask_CtCvOn(void);

static void   ControlTaskCtCmdUsage(void);

static void OpMode_Enter(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal = SvcFlowControl_Exec("cmdmode");

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
    static SVC_FLOW_CONTROL_CMD_LIST_s SvcControlTaskCtCmdList[CONTROL_TASK_CMD_CT_NUM] = {
        { CONTROL_TASK_CMD_CT_CMDMODE,   ControlTask_CtCmdMode  },
        { CONTROL_TASK_CMD_CT_LV,        ControlTask_CtLiveview },
        { CONTROL_TASK_CMD_CT_SYS_INIT,  ControlTask_CtSysInit  },
        { CONTROL_TASK_CMD_CT_CV_ON,     ControlTask_CtCvOn     },
    };

    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() start", 0U, 0U);

    RetVal = SvcFlowControl_CmdHook(SvcControlTaskCtCmdList, CONTROL_TASK_CMD_CT_NUM, ControlTaskCtCmdUsage);

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
* Control task for entering Liveview.
*/
static UINT32 ControlTask_CtCmdMode(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec("icam_cmdmode");
    RetVal |= SvcFlowControl_Exec("lnxboot");
    RetVal |= SvcFlowControl_Exec("ct_sys_init");

    return RetVal;
}

/**
* Control task for entering Liveview .
*/
static UINT32 ControlTask_CtLiveview(void)
{
    UINT32 RetVal = SVC_OK;
    static UINT8  InitCV = 0U;

    RetVal |= SvcFlowControl_Exec("icam_liv");
    RetVal |= SvcFlowControl_Exec("dspboot");

    if (InitCV == 0U) {
        RetVal |= SvcFlowControl_Exec("cv_config");
        InitCV = 1U;
    }

    RetVal |= SvcFlowControl_Exec("cv_start");
    RetVal |= SvcFlowControl_Exec("ct_cv_on");
    RetVal |= SvcFlowControl_Exec("vout_on");

    return RetVal;
}

/**
* Control task for entering system init
* @return ErrorCode
*/
static UINT32 ControlTask_CtSysInit(void)
{
    CtSysInit();

    return SVC_OK;
}

/**
* Control task for entering system init
* @return ErrorCode
*/
static UINT32 ControlTask_CtCvOn(void)
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
#endif
        return RetVal;
}

/**
* Show the ControlTask cmd usage
* @return ErrorCode
*/
static void ControlTaskCtCmdUsage(void)
{
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "CNN_TestBed Usage :", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           liveview: flow control for entering CNN_TestBed project liveview", 0U, 0U);
}
