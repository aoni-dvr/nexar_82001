/**
 *  @file SvcGuiTask.c
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
 *  @details svc application for icam GUI
 *
 */

#include "AmbaTypes.h"
#include "AmbaShell.h"

/* ssp */
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"

/* app-shared */
#include "SvcResCfg.h"
#include "SvcGui.h"
#include "SvcGuiDiagBox.h"

/* app-icam */
#include "SvcGuiTask.h"

#define SVC_LOG_GUI_TASK        "GUI_TASK"

/**
 * Init SvcGui module
 * return 0-OK, 1-NG
 */
UINT32 SvcGuiTask_Init(void)
{

    SvcLog_OK(SVC_LOG_GUI_TASK, "SvcGuiTask_Init() start", 0U, 0U);
    if (SVC_OK != SvcGui_Init()) {
        SvcLog_NG(SVC_LOG_GUI_TASK, "SvcGui_Init() failed", 0U, 0U);
    }

    (void) SvcDiagBox_Initial("diagbox.bmp");

    SvcLog_OK(SVC_LOG_GUI_TASK, "SvcGuiTask_Init() done", 0U, 0U);

    return SVC_OK;
}

/**
 * Start SvcGui module
 * return 0-OK, 1-NG
 */
UINT32 SvcGuiTask_Start(void)
{
    UINT32 RetVal;
    UINT32 i, Bit = 0x1U;
    UINT32 VoutID;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDisp;

    SvcLog_OK(SVC_LOG_GUI_TASK, "SvcGuiTask_Start() start", 0U, 0U);

    for (i = 0; i < pCfg->DispNum; i++) {
        SVC_GUI_SETTING_s Config = {0U};
        if ((pCfg->DispBits & (UINT32)(Bit << i)) == 0U) {
            continue;
        }

        pDisp = &(pCfg->DispStrm[i]);
        VoutID = pDisp->VoutID;

        if ((pDisp->GuiCfg.Attribute & SVC_GUI_ATTR_TIMER) > 0U) {
            Config.Type = (UINT8) SVC_GUI_TYPE_TIMER;
        } else {
            if(pDisp->FrameRate.Interlace == 1U){ // CVBS
                Config.Type = (UINT8) SVC_GUI_TYPE_POLARITY;
            } else {
                Config.Type = (UINT8) SVC_GUI_TYPE_VOUT;
            }
        }
        Config.VoutFrameRate.TimeScale = pDisp->FrameRate.TimeScale;
        Config.VoutFrameRate.NumUnitsInTick = pDisp->FrameRate.NumUnitsInTick;
        if ((pDisp->GuiCfg.Attribute & SVC_GUI_ATTR_CUSTOM_RATE) > 0U) {
            Config.MaxFrameRate.TimeScale = pDisp->GuiCfg.UpdateFrameRate.TimeScale;
            Config.MaxFrameRate.NumUnitsInTick = pDisp->GuiCfg.UpdateFrameRate.NumUnitsInTick;
        } else {
            Config.MaxFrameRate.TimeScale = 60000U;
            Config.MaxFrameRate.NumUnitsInTick = 1000U;
        }

        if(Config.Type == (UINT8) SVC_GUI_TYPE_POLARITY){ // force CVBS MaxFrameRate
            Config.MaxFrameRate.TimeScale = 30000U;
            Config.MaxFrameRate.NumUnitsInTick = 1000U;
        }


        RetVal = SvcGui_Start(VoutID, &Config);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_GUI_TASK, "SvcGui_Start(%d) failed %d", i, RetVal);
        }

        SvcGui_DebugEnable(0U);
    }

    SvcLog_OK(SVC_LOG_GUI_TASK, "SvcGuiTask_Start() done", 0U, 0U);

    return SVC_OK;
}

/**
 * Stop SvcGui module
 * return 0-OK, 1-NG
 */
UINT32 SvcGuiTask_Stop(void)
{
    UINT32 RetVal;
    UINT32 i, Bit = 0x1U;
    UINT32 VoutID;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDisp;

    SvcLog_OK(SVC_LOG_GUI_TASK, "SvcGuiTask_Stop() start", 0U, 0U);

    for (i = 0; i < pCfg->DispNum; i++) {
        if ((pCfg->DispBits & (UINT32)(Bit << i)) == 0U) {
            continue;
        }

        pDisp = &(pCfg->DispStrm[i]);
        VoutID = pDisp->VoutID;
        RetVal = SvcGui_Stop(VoutID);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_GUI_TASK, "SvcGui_Stop(%d) failed %d", i, RetVal);
        }
    }

    SvcLog_OK(SVC_LOG_GUI_TASK, "SvcGuiTask_Stop() done", 0U, 0U);

    return SVC_OK;
}
