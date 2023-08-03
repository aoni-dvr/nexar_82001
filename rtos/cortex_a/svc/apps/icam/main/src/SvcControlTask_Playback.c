/**
 *  @file SvcControlTask_Playback.c
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
 *  @details svc control task for playback
 *
 */

#ifndef CTRL_TASK_PBK_H     /* Dir 4.10 */
#define CTRL_TASK_PBK_H

#ifdef CONFIG_ICAM_PLAYBACK_USED
#include "AmbaGDMA.h"
#include "SvcPbkPictDisp.h"
#include "SvcPbkThmDisp.h"
#endif

/**
* Control task for Playback. The function is used to enter playback mode
* @return ErrorCode
*/
static UINT32 ControlTask_PlaybackStart(void)
{
    UINT32               RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&RetVal);
#ifdef CONFIG_ICAM_PLAYBACK_USED

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "This is Playback mode", 0U, 0U);

    if (SVC_OK == RetVal) {
        SVC_USER_PREF_s *pSvcUserPref;

        RetVal = SvcUserPref_Get(&pSvcUserPref);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcUserPref_Get() failed with %d", RetVal, 0U);
        } else {
            pSvcUserPref->OperationMode = 2U;   /* Playback mode */
        }
    }

    /* Re-map the shared memory */
    SvcBufMap_Config(SMAP_ID_PLAYBACK);

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    /* Enable image/3A task or FixedIsoConfig for debug usage */
    if (SVC_OK == RetVal) {
        RetVal = SvcImgTask_ConfigNull();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcImgTask_ConfigNull() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

    /* Config Vout (including lcd device enable/config) based on current resolution */
    if (SVC_OK == RetVal) {
        RetVal = SvcVoutCtrlTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutCtrlTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Wait Vout Src is ready */
    if (SVC_OK == RetVal) {
        RetVal = SvcVoutCtrlTask_WaitSrcReady();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutCtrlTask_WaitSrcReady() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcDisplayTask_Start();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDisplayTask_Start() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

#ifdef CONFIG_ICAM_AUDIO_USED
    /* audio config */
    if (SVC_OK == RetVal) {
        SvcAudioTask_Config();
    }
#endif

    /* Playback config */
    if (SVC_OK == RetVal) {
        RetVal = SvcPlaybackTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcPlaybackTask_Config() failed with %d", RetVal, 0U);
        }
    }
#endif

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "ControlTask_PlaybackStart() done", 0U, 0U);
    }

    return RetVal;
}

static UINT32 ControlTask_PlaybackStop(void)
{
    UINT32 RetVal = SVC_OK, Err;

    AmbaMisra_TouchUnused(&RetVal);
#ifdef CONFIG_ICAM_PLAYBACK_USED
    RetVal = SvcVoutCtrlTask_Destroy();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutCtrlTask_Destroy() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "ControlTask_PlaybackStop() done", 0U, 0U);
    }

    Err = AmbaKAL_TaskSleep(1000U);
    if (SVC_OK != Err) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "AmbaKAL_TaskSleep() failed with %d", Err, 0U);
    }

    return RetVal;
}

static UINT32 ControlTask_PlaybackGuiOn(void)
{
    UINT32               RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&RetVal);

#ifdef CONFIG_ICAM_PLAYBACK_USED
    if (SVC_OK == RetVal) {
        RetVal = SvcGuiTask_Start();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcGuiTask_Start() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

#ifdef CONFIG_ICAM_GUI_MENU_USED
    if (SVC_OK == RetVal) {
        RetVal  = SvcMenuTask_Config(SVC_MENU_TASK_TYPE_PLAYBACK);
        RetVal |= SvcMenuTask_Load();
        if (SVC_OK != RetVal) {
            SvcLog_DBG(SVC_LOG_CONTROL_TASK, "SvcMenuTask_Config() failed %d", 0U, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

    if (SVC_OK == RetVal) {
        RetVal = SvcPlaybackTask_ThumbnailStart();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcPlaybackTask_ThumbnailStart() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "ControlTask_PlaybackGuiOn() done", 0U, 0U);
    }

    return RetVal;
}

static UINT32 ControlTask_PlaybackGuiOff(void)
{
    UINT32               RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&RetVal);
#ifdef CONFIG_ICAM_PLAYBACK_USED

    if (SVC_OK == RetVal) {
        RetVal = SvcPlaybackTask_ThumbnailStop();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcPlaybackTask_ThumbnailStop() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcGuiTask_Stop();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcGuiTask_Stop() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "ControlTask_PlaybackGuiOff() done", 0U, 0U);
    }

    return RetVal;
}

#endif /* CTRL_TASK_PBK_H */
