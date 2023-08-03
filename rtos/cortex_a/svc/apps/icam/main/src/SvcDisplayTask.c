/**
 *  @file SvcDisplayTask.c
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
 *  @details svc display task
 *
 */

#include "AmbaTypes.h"
#include "AmbaShell.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

#include "AmbaDSP.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaHDMI_Def.h"
#include "AmbaUtility.h"
#include "AmbaSensor.h"
#include "AmbaFPD.h"
#include "AmbaPWM.h"
#include "AmbaGPIO.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcIK.h"
#include "SvcCmd.h"
#include "SvcResCfg.h"
#include "SvcDisplay.h"
#include "SvcLiveview.h"
#include "SvcEnc.h"
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcInfoPack.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcGuiTextView.h"
#include "SvcSysStat.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "SvcUserPref.h"
#include "SvcPref.h"

#include "SvcAppStat.h"
#include "SvcDisplayTask.h"


#define SVC_LOG_DISPLAY_TASK "DISPLAY_TASK"

#define DISPLAY_TASK_GUI_TEXTBOX_W (480U)
#define DISPLAY_TASK_GUI_TEXTBOX_H (100U)
#define DISPLAY_TASK_GUI_W          (DISPLAY_TASK_GUI_TEXTBOX_W)
#define DISPLAY_TASK_GUI_H          (DISPLAY_TASK_GUI_TEXTBOX_H)

static UINT32 DisplayTaskGuiVout = VOUT_IDX_A;
static SVC_GUI_CANVAS_s DisplayTaskCanvas = {0};
static UINT32           DisplayTaskGuiUpdate = 0U;

/* Menu status callback */
static void DisplayTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo);
static void DisplayTask_DrawEntry(UINT32 VoutIdx, UINT32 Level);
static void DisplayTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);

static void DisplayTask_CmdInstall(void);
static void DisplayTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void DisplayTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);

/**
 * Init display. Prepare the resource needed to control display
 * return 0-OK, 1-NG
 */
UINT32 SvcDisplayTask_Init(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_SYS_STAT_ID StatusID = 0U;

    /* display init */
    SvcDisplay_Init();

    RetVal = SvcSysStat_Register(SVC_APP_STAT_MENU, DisplayTask_MenuStatusCallback, &StatusID);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DISPLAY_TASK, "Sys Stat Register failed", 0U, 0U);
    }

    DisplayTask_CmdInstall();

    return RetVal;
}

/**
 * Start display
 * return 0-OK, 1-NG
 */
UINT32 SvcDisplayTask_Start(void)
{
    UINT32 RetVal = SVC_OK, DispNum;
    SVC_DISP_CTRL_s DispCtrl[AMBA_DSP_MAX_VOUT_NUM] = {0};
    SVC_DISP_INFO_s DispInfo;
    UINT32 i;

    SvcLog_DBG(SVC_LOG_DISPLAY_TASK, "Display start", 0U, 0U);

    /* display configuration */
    SvcDisplay_InfoGet(&DispInfo);
    SvcInfoPack_DispConfig(&DispInfo);
    SvcDisplay_Config();

    DispNum = *(DispInfo.pNumDisp);

    for (i = 0; i < DispNum; i++) {
        DispCtrl[i].VoutID = DispInfo.pDispCfg[i].VoutID;
        DispCtrl[i].EnableVideo = 1U;
        DispCtrl[i].EnableOsd = 1U;
    }

    /* Start Display */
    SvcDisplay_Ctrl(DispNum, DispCtrl);

    SvcLog_DBG(SVC_LOG_DISPLAY_TASK, "Display start done", 0U, 0U);

    return RetVal;
}

/**
 * Stop display
 * return 0-OK, 1-NG
 */
UINT32 SvcDisplayTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DispNum;
    UINT32 i;

    SVC_DISP_INFO_s DispInfo;
    SVC_DISP_CTRL_s DispCtrl[AMBA_DSP_MAX_VOUT_NUM] = {0};

    SvcLog_DBG(SVC_LOG_DISPLAY_TASK, "Display stop", 0U, 0U);

    /* display configuration */
    SvcDisplay_InfoGet(&DispInfo);

    DispNum = *(DispInfo.pNumDisp);

    for (i = 0U; i < DispNum; i++) {
        DispCtrl[i].VoutID = DispInfo.pDispCfg[i].VoutID;
        DispCtrl[i].EnableVideo = 0U;
        DispCtrl[i].EnableOsd = 0U;
    }

    SvcDisplay_Ctrl(DispNum, DispCtrl);

    SvcLog_DBG(SVC_LOG_DISPLAY_TASK, "Display stop done", 0U, 0U);

    return RetVal;
}

/**
 * Resume display from stop stage
 * return 0-OK, 1-NG
 */
UINT32 SvcDisplayTask_Resume(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DispNum;
    UINT32 i;

    SVC_DISP_INFO_s DispInfo;
    SVC_DISP_CTRL_s DispCtrl[AMBA_DSP_MAX_VOUT_NUM] = {0};

    UINT32 StrmNum;
    SVC_LIV_INFO_s LivInfo;

    SvcLog_DBG(SVC_LOG_DISPLAY_TASK, "Display resume", 0U, 0U);

    /* display configuration */
    SvcDisplay_InfoGet(&DispInfo);
    SvcInfoPack_DispConfig(&DispInfo);

    DispNum = *(DispInfo.pNumDisp);

    for (i = 0U; i < DispNum; i++) {
        DispCtrl[i].VoutID = DispInfo.pDispCfg[i].VoutID;
        DispCtrl[i].EnableVideo = 1U;
        DispCtrl[i].EnableOsd = 1U;
    }

    SvcDisplay_Update();
    SvcDisplay_Ctrl(DispNum, DispCtrl);

    /* liveview configuration */
    SvcLiveview_InfoGet(&LivInfo);
    SvcInfoPack_LivStrmCfg(LivInfo.pNumStrm, LivInfo.pStrmCfg, LivInfo.pStrmChan);

    StrmNum = *(LivInfo.pNumStrm);

    for (i = 0U; i < StrmNum; i++) {
        SvcLiveview_UpdateStream(i);
    }

    SvcLog_DBG(SVC_LOG_DISPLAY_TASK, "Display resume done", 0U, 0U);

    return RetVal;
}

static void DisplayTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_MENU_s *pStatus = NULL;

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    AmbaMisra_TouchUnused(pInfo);   /* Misra-c fixed */
    AmbaMisra_TouchUnused(pStatus); /* Misra-c fixed */

    AmbaMisra_TypeCast(&pStatus, &pInfo);

    if (StatIdx == SVC_APP_STAT_MENU) {
        if ((pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) && (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_LAYOUT)) {
            if (pCfg->DispAltNum > 1U) {
                UINT32 DispAltIdx = pCfg->DispAltIdx;

                DispAltIdx = (DispAltIdx + 1U) % pCfg->DispAltNum;

                RetVal |= SvcDisplayTask_Stop();
                RetVal |= SvcResCfg_ConfigDispAlt(DispAltIdx);
                RetVal |= SvcDisplayTask_Resume();
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DISPLAY_TASK, "Layout switch failed", 0U, 0U);
                }
            } else {
                UINT32 OsdWidth, OsdHeight;

                SvcLog_NG(SVC_LOG_DISPLAY_TASK, "Layout switch not support in this format id", 0U, 0U);

                DisplayTaskGuiVout = (UINT32) pStatus->Vout;
                (void) SvcOsd_GetOsdBufSize(DisplayTaskGuiVout, &OsdWidth, &OsdHeight);
                DisplayTaskCanvas.StartX = (OsdWidth - (DISPLAY_TASK_GUI_W)) >> (UINT32) 1U;
                DisplayTaskCanvas.StartY = (OsdHeight - (DISPLAY_TASK_GUI_H)) >> (UINT32) 1U;
                DisplayTaskCanvas.Width  = DISPLAY_TASK_GUI_W;
                DisplayTaskCanvas.Height = DISPLAY_TASK_GUI_H;

                SvcGui_Register(pStatus->Vout, 17U, "DispTsk", DisplayTask_DrawEntry, DisplayTask_DrawUpdate);
                DisplayTaskGuiUpdate = 1U;
                (void) AmbaKAL_TaskSleep(1500U);
                SvcGui_Unregister(pStatus->Vout, 17U);
            }
        }
    }
}

static void DisplayTask_DrawEntry(UINT32 VoutIdx, UINT32 Level)
{
    SVC_GUI_TEXTVIEW_s TextView = {
        .BackgroundColor = 0x80202040U,
        .FontColor = 0xffffffffU,
        .FontSize = 1U,
        .LineGap = 1U,
        .pText = "Layout switch not support in this format id",
    };

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    SvcGuiTextView_Draw(DisplayTaskGuiVout, &DisplayTaskCanvas, &TextView);
}

static void DisplayTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (DisplayTaskGuiUpdate == 1U) {
        DisplayTaskGuiUpdate = 0U;
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static void DisplayTask_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s  SvcDisplayTaskCmd;

    UINT32  RetVal;

    SvcDisplayTaskCmd.pName    = "svc_display_task";
    SvcDisplayTaskCmd.MainFunc = DisplayTask_CmdEntry;
    SvcDisplayTaskCmd.pNext    = NULL;

    RetVal = SvcCmd_CommandRegister(&SvcDisplayTaskCmd);
    if (SHELL_ERR_SUCCESS != RetVal) {
        SvcLog_NG(SVC_LOG_DISPLAY_TASK, "## fail to install svc display command", 0U, 0U);
    }
}

static void DisplayTask_AppTaskReset(void)
{
    SVC_APP_STAT_DISP_s DisplayStatus = {0};

    DisplayStatus.Status = SVC_APP_STAT_DISP_CHG;
    if (SVC_OK != SvcSysStat_Issue(SVC_APP_STAT_DISP, &DisplayStatus)) {
        SvcLog_NG(SVC_LOG_DISPLAY_TASK, "## fail to issue display change statue", 0U, 0U);
    }
}

static void DisplayTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_NG;
    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("stop", pArgVector[1U])) {
            RetVal = SvcDisplayTask_Stop();
        } else if (0 == SvcWrap_strcmp("start", pArgVector[1U])) {
            RetVal = SvcDisplayTask_Resume();
        } else if (0 == SvcWrap_strcmp("disp_alt", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 DispAltIdx = 0U;
                if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DispAltIdx)) {
                    SvcLog_NG(SVC_LOG_DISPLAY_TASK, "SvcWrap_strtoul() failed!!", 0U, 0U);
                }
                RetVal = SvcDisplayTask_Stop();
                RetVal = SvcResCfg_ConfigDispAlt(DispAltIdx);
                if (RetVal == SVC_OK){
                    DisplayTask_AppTaskReset();
                } else {
                    // Nothing
                }
                RetVal = SvcDisplayTask_Resume();
            } else {
                RetVal = SVC_NG;
            }

        } else if (0 == SvcWrap_strcmp("sync_from_vin", pArgVector[1U])) {
            if (3U < ArgCount) {
                SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
                UINT32 DispIdx = 255U;
                UINT32 DelayTime = 0xFFFFFFFFU;
                UINT32 SyncVinID = 255U;

                if (pResCfg != NULL) {
                    if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DispIdx)) {
                        SvcLog_NG(SVC_LOG_DISPLAY_TASK, "SvcWrap_strtoul() failed!!", 0U, 0U);
                    }
                    if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &DelayTime)) {
                        SvcLog_NG(SVC_LOG_DISPLAY_TASK, "SvcWrap_strtoul() failed!!", 0U, 0U);
                    }
                    if (4U < ArgCount) {
                        if (SVC_OK != SvcWrap_strtoul(pArgVector[4U], &SyncVinID)) {
                            SvcLog_NG(SVC_LOG_DISPLAY_TASK, "SvcWrap_strtoul() failed!!", 0U, 0U);
                        }
                    }

                    if (DispIdx >= pResCfg->DispNum) {
                        RetVal = SVC_NG;
                    } else if (DelayTime > 0xFFFFU) {
                        RetVal = SVC_NG;
                    } else {
                        SvcLog_DBG(SVC_LOG_DISPLAY_TASK, "Configure DispID(%d), VoutID(%d) Sync delay time", DispIdx, pResCfg->DispStrm[DispIdx].VoutID);
                        SvcLog_DBG(SVC_LOG_DISPLAY_TASK, "  %d -> %d (1 unit = 100 us = 0.1 ms)", pResCfg->DispStrm[DispIdx].VinVoutSyncCfg.DelayTime, DelayTime);
                        pResCfg->DispStrm[DispIdx].VinVoutSyncCfg.DelayTime = DelayTime;

                        RetVal = SVC_OK;
                    }
                }
            }
        } else {
            RetVal = SVC_NG;
        }
    }

    if (SVC_NG == RetVal) {
        DisplayTask_CmdUsage(PrintFunc);
    }
}

static void DisplayTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_display_task commands:\n");
    PrintFunc(" =======================\n");
    PrintFunc("                 stop      : Stop display \n");
    PrintFunc("                 start     : Start display \n");
    PrintFunc("                 disp_alt  : Change display layout if user has designed \n");
    PrintFunc("                  [AltIdx] - Target display layout idx \n");
    PrintFunc("            sync_from_vin  : Change display sync from vin setting \n");
    PrintFunc("                 [dispIdx] - Target display idx \n");
    PrintFunc("                   [delay] - Configure delay time after vin data ready. 1 unit = 100 us \n");
    PrintFunc("             [sync_vin_id] - Configure sync with specify vin. Only for CV2xFS/CV5x now\n");
}
