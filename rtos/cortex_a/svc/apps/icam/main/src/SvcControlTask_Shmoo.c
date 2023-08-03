/**
 *  @file SvcControlTask_Shmoo.c
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


/* svc-framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcPref.h"

#include "AmbaDSP_Capability.h" /*For pref*/
/* app-icam */
#include "SvcUserPref.h"
#include "SvcControlTask.h"
#include "SvcFlowControl.h"

#include "AmbaMisraFix.h"

#if defined(CONFIG_BUILD_CV)    /* CV related function */
#include "SvcCvMainTask.h"
#include "SvcCvCtrlTask.h"
#endif
#include "SvcAdvancedShmooTask.h"

#include "SvcShmooCvTask.h"
#include "SvcShmooRecTask.h"




#define SVC_LOG_CONTROL_TASK        "CTRL_TASK_SHMOO"

#define CONTROL_TASK_CMD_SHMOO_NUM       (8U)
#define CONTROL_TASK_CMD_SHMOO_CMDMODE   ("cmdmode")
#define CONTROL_TASK_CMD_SHMOO_ENCODE    ("mode_0")
#define CONTROL_TASK_CMD_SHMOO_CV        ("mode_1")
#define CONTROL_TASK_CMD_SHMOO_MIXED     ("mode_mix")
#define CONTROL_TASK_CMD_SHMOO_SHMOOTASK ("shmootask")
#define CONTROL_TASK_CMD_CV_SHMOO_START  ("shmoo_cv_start")
#define CONTROL_TASK_CMD_REC_SHMOO_START  ("shmoo_rec_start")
#define CONTROL_TASK_CMD_ADV_SHMOO_START ("adv_shmoo_start")

static UINT32 ControlTask_ShmooCmdMode(void);
static UINT32 ControlTask_ShmooEncode(void);
static UINT32 ControlTask_ShmooCV(void);
static UINT32 ControlTask_ShmooMix(void);
static UINT32 ControlTask_ShmooTask(void);
static UINT32 ControlTask_CvShmooStart(void);
static UINT32 ControlTask_RecShmooStart(void);
static UINT32 ControlTask_AdvShmooStart(void);

static void  ControlTaskShmooCmdUsage(void);


static void OpMode_Enter(void)
{
    SVC_USER_PREF_s *pSvcUserPref;
    UINT32 formatId, ShmooStart, ShmooMode;

    SvcCmdAdvShmoo_Install();

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        formatId = pSvcUserPref->FormatId;
        ShmooStart = pSvcUserPref->ShmooInfo.ShmooTask;
        ShmooMode = pSvcUserPref->ShmooInfo.ShmooMode;
        AmbaMisra_TouchUnused(&formatId);

        if(ShmooStart == 1U){
            if(ShmooMode == 0U){
                if (SVC_OK != SvcFlowControl_Exec(CONTROL_TASK_CMD_SHMOO_CMDMODE)) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFlowControl_Exec \"icam_cmdmode\" failed", 0U, 0U);
                }
            } else if (ShmooMode == 1U){
                if (SVC_OK != SvcFlowControl_Exec(CONTROL_TASK_CMD_SHMOO_ENCODE)) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFlowControl_Exec \"shmoo encode mode\" failed", 0U, 0U);
                }
            } else if (ShmooMode == 2U){
                if (SVC_OK != SvcFlowControl_Exec(CONTROL_TASK_CMD_SHMOO_CV)) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFlowControl_Exec \"shmoo cv mode\" failed", 0U, 0U);
                }
            } else if (ShmooMode == 3U){
                if (SVC_OK != SvcFlowControl_Exec(CONTROL_TASK_CMD_SHMOO_MIXED)) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFlowControl_Exec \"shmoo mix mode\" failed", 0U, 0U);
                }
            } else {
                if (SVC_OK != SvcFlowControl_Exec(CONTROL_TASK_CMD_SHMOO_CMDMODE)) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFlowControl_Exec \"shmoo encode mode\" failed", 0U, 0U);
                }
            }

            /*After activate app, start advanced shmoo*/
            if (SVC_OK != SvcFlowControl_Exec(CONTROL_TASK_CMD_ADV_SHMOO_START)) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFlowControl_Exec \"shmoo encode mode\" failed", 0U, 0U);
            }

        } else {
            if (SVC_OK != SvcFlowControl_Exec(CONTROL_TASK_CMD_SHMOO_CMDMODE)) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFlowControl_Exec \"icam_cmdmode\" failed", 0U, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "## OpMode_Enter() failed with get preference error", 0U, 0U);
        if (SVC_OK != SvcFlowControl_Exec(CONTROL_TASK_CMD_SHMOO_CMDMODE)) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFlowControl_Exec \"icam_cmdmode\" failed", 0U, 0U);
        }
    }
}

/**
* The entry function of project (EMR). User can implement their flow control here.
* @return ErrorCode
*/
UINT32 SvcControlTask_ProjectEntry(void)
{
    static SVC_FLOW_CONTROL_CMD_LIST_s SvcControlTaskShmooCmdList[CONTROL_TASK_CMD_SHMOO_NUM] = {
        { CONTROL_TASK_CMD_SHMOO_CMDMODE,    ControlTask_ShmooCmdMode          },
        { CONTROL_TASK_CMD_SHMOO_ENCODE,     ControlTask_ShmooEncode           },
        { CONTROL_TASK_CMD_SHMOO_CV,         ControlTask_ShmooCV               },
        { CONTROL_TASK_CMD_SHMOO_MIXED,      ControlTask_ShmooMix              },
        { CONTROL_TASK_CMD_SHMOO_SHMOOTASK,  ControlTask_ShmooTask             },
        { CONTROL_TASK_CMD_CV_SHMOO_START,   ControlTask_CvShmooStart          },
        { CONTROL_TASK_CMD_REC_SHMOO_START,  ControlTask_RecShmooStart         },
        { CONTROL_TASK_CMD_ADV_SHMOO_START,  ControlTask_AdvShmooStart         },

    };

    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() start", 0U, 0U);

    RetVal = SvcFlowControl_CmdHook(SvcControlTaskShmooCmdList, CONTROL_TASK_CMD_SHMOO_NUM, ControlTaskShmooCmdUsage);

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() done", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
        OpMode_Enter();
    }

    return RetVal;
}

static UINT32 ControlTask_ShmooTask(void)
{

    UINT32 RetVal = 0U;

    return RetVal;

}

/**
* Control task for entering Cmdmode.
*/
static UINT32 ControlTask_ShmooCmdMode(void)
{
    UINT32 RetVal = SVC_OK;
    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_CMDMODE);

    return RetVal;
}

/**
* Control task for entering Shmoo Encode mode.
* @return ErrorCode
*/
static UINT32 ControlTask_ShmooEncode(void)
{
    UINT32      RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec(CONTROL_TASK_CMD_REC_SHMOO_START);
    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_LIVEVIEW);
    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_DSP_BOOT);
    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_VOUT_ON);

    return RetVal;
}

/**
* Control task for Shmoo CV mode.
* @return ErrorCode
*/
static UINT32 ControlTask_ShmooCV(void)
{
    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_CV_CONFIG);
    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_CV_START);
    RetVal |= SvcFlowControl_Exec(CONTROL_TASK_CMD_CV_SHMOO_START);

    return RetVal;
}

static UINT32 ControlTask_ShmooMix(void)
{

    UINT32 RetVal = SVC_OK;

    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_CV_CONFIG);
    RetVal |= SvcFlowControl_Exec(CONTROL_TASK_CMD_REC_SHMOO_START);
    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_LIVEVIEW);
    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_DSP_BOOT);
    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_CV_START);
    RetVal |= SvcFlowControl_Exec(CONTROL_TASK_CMD_CV_SHMOO_START);
    RetVal |= SvcFlowControl_Exec(SVC_CONTROL_TASK_CMD_VOUT_ON);

    return RetVal;

}

static UINT32 ControlTask_RecShmooStart(void)
{

#if defined(CONFIG_ICAM_RECORD_USED)
    UINT32 RetVal;

    RetVal = SvcShmooRecTask_ShmooTask();

    if (RetVal ==  SVC_OK){
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "Svc Shmoo Rec Start Successfully", 0U, 0U);
    } else {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "Svc Shmoo Rec Start failed", 0U, 0U);
    }

    return RetVal;
#else
    return SVC_OK;
#endif
}

static UINT32 ControlTask_CvShmooStart(void)
{

    UINT32 RetVal = SVC_OK;
#if defined(CONFIG_ICAM_PROJECT_SHMOO) && defined(CONFIG_BUILD_CV)
    RetVal = SvcCvMainTask_WaitSrcReady();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvMainTask_WaitSrcReady() failed with %d", RetVal , 0U);
        SvcFlowControl_Assert();
    }
    RetVal = SvcCvCtrlTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvCtrlTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
    RetVal = SvcShmooCvTask_ShmooTask();
#endif
    return RetVal;
}

static UINT32 ControlTask_AdvShmooStart(void)
{

    UINT32 RetVal = SVC_OK;
#if defined(CONFIG_ICAM_PROJECT_SHMOO)
    RetVal = SvcAdvancedShmooTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAdvancedShmooTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
    return RetVal;
}

/**
* Show the ControlTask cmd usage
* @return ErrorCode
*/
static void ControlTaskShmooCmdUsage(void)
{
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, " Cmd Usage (Shmoo):", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           cmdmode:    flow control for entering cmdmode", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           mode_0: flow control for entering encode mode", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           mode_1: flow control for cv mode", 0U, 0U);
}
