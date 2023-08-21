/**
 *  @file SvcControlTask.c
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
 *  @details svc control task
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaGPIO.h"
#include "AmbaUtility.h"
#include "AmbaSYS.h"
#include "AmbaSD.h"
#include "AmbaFPD.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_VideoDec.h"
#if defined(CONFIG_ICAM_SWPLL_CONTROL)
#include "AmbaFTCM.h"
#endif
#include "AmbaSvcWrap.h"
#include "AmbaVfs.h"
#include "AmbaCodecCom.h"
#include "AmbaDspInt.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#if defined(CONFIG_ENABLE_AMBALINK)
#if defined(CONFIG_AMBALINK_BOOT_OS)
#include "AmbaLink.h"
#include "SvcRpmsgApp.h"
#else
#include "AmbaIPC.h"
#endif
#endif

#if defined(CONFIG_ICAM_IMGITUNER_USED)
#include "AmbaTUNE_USBCtrl.h"
#endif

#if defined(CONFIG_ICAM_USB_USED)
#include "AmbaSvcUsb.h"
#endif

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcPref.h"
#include "SvcMem.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#if defined(CONFIG_ICAM_IMGCAL_USED)
#include "SvcCalibMgr.h"
#include "SvcCalibTask.h"
#endif
#include "SvcBuffer.h"
#include "SvcCmdDSP.h"
#include "SvcFlowControl.h"
#include "SvcResCfg.h"
#include "SvcButtonCtrl.h"
#include "SvcShell.h"
#if defined(CONFIG_ICAM_SWPLL_CONTROL)
#include "SvcSwPll.h"
#endif
#include "SvcSysStat.h"
#include "SvcVoutSrc.h"
#include "SvcDataLoader.h"
#include "SvcOsd.h"
#include "SvcUserPref.h"
#include "SvcResCfgTask.h"
#include "SvcIKCfg.h"
#include "SvcVinCtrlTask.h"
#include "SvcVinBufMonitorTask.h"
#include "SvcVoutCtrlTask.h"
#include "SvcVoutFrmCtrlTask.h"
#include "SvcLiveviewTask.h"
#include "SvcDisplayTask.h"
#include "SvcDspTask.h"
#include "SvcRecTask.h"
#include "SvcImgTask.h"
#include "SvcSafeTask.h"
#include "SvcBufMap.h"
#include "SvcEmrTask.h"
#include "SvcOsdTask.h"
#include "SvcGuiTask.h"
#include "SvcMenuTask.h"
#include "SvcPlaybackTask.h"
#include "SvcRawEncTask.h"
#include "SvcFwUpdateTask.h"
#include "SvcRawCapTask.h"
#include "SvcCmdApp.h"
#include "SvcUcode.h"
#include "SvcDSP.h"
#include "SvcButtonCtrlTask.h"
#include "SvcWarningIconTask.h"
#include "SvcSysStatTask.h"
#include "SvcDirtDetectTask.h"
#include "SvcDefogTask.h"
#include "SvcColorBalance.h"
#include "SvcStill.h"
#include "SvcStillCap.h"
#include "SvcStillTask.h"
#include "SvcControlTask.h"
#include "SvcStgMonitorTask.h"
#include "SvcClkSwTask.h"
#if defined(CONFIG_ICAM_TIMING_LOG)
#include "SvcTiming.h"
#endif
#if defined(CONFIG_SVC_VIN_ERR_CTRL_USED)
#include "SvcVinErrTask.h"
#endif
#if defined(CONFIG_SVC_VOUT_ERR_CTRL_USED)
#include "SvcVoutErrTask.h"
#endif
#include "SvcAppStat.h"
#include "SvcCanSpeedTask.h"
#if defined(CONFIG_BUILD_CV)    /* CV related function */
#include "SvcCvMainTask.h"
#include "SvcCvCtrlTask.h"
#endif
#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
#include "SvcEmrAdaptiveTask.h"
#endif
#ifdef CONFIG_ICAM_AUDIO_USED
#include "SvcAudioTask.h"
#endif
#ifdef CONFIG_ICAM_ENABLE_REGRESSION_FLOW
#include "SvcRegressionTask.h"
#endif
#ifdef CONFIG_ICAM_PROJECT_SURROUND
#include "SvcAnimTask.h"
#endif
#include "SvcNvmTask.h"
#include "SvcTaskList.h"
#include "SvcPlat.h"
#if defined(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED)
#include "SvcLvFeedExtRawTask.h"
#endif
#if defined(CONFIG_ICAM_LV_FEED_EXT_YUV_SUPPORTED)
#include "SvcLvFeedExtYuvTask.h"
#endif
#if defined(CONFIG_ICAM_BIST_UCODE)
#include "AmbaShell.h"
#include "SvcUcBIST.h"
#endif
#if defined(CONFIG_ICAM_SENSOR_ASIL_ENABLED)
#include "SvcSensorCrcCheckTask.h"
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include <app_base.h>
#endif


#define SVC_LOG_CONTROL_TASK        "CTRL_TASK"

/* Internal function */
#include "SvcControlTask_Playback.c"
#include "SvcControlTask_Liveview.c"

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static void ControlTask_LnxBoot_Timeout_Handler(int eid)
{
    static int cnt = 0;
    if (eid == TIMER_UNREGISTER) {
        cnt = 0;
        Pmic_SetBootFailTimes(0);
        return;
    }
    cnt += 1;
    if (cnt >= 2) {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "#### lnx load timeout. reboot", 0U, 0U);AmbaPrint_Flush();
        Pmic_SoftReset();
    }
}
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static int liview_switch_finish = 0;
int SvcControlTask_CheckLiveviewSwitchFinish(void)
{
    return liview_switch_finish;
}

void SvcControlTask_SetLiveviewSwitchFinish(int finish)
{
    liview_switch_finish = finish;
}
#endif

#ifdef CONFIG_AMBALINK_BOOT_OS
void AmbaLink_EchoCh_Callback(char *Msg);
#ifdef CONFIG_ICAM_ENABLE_REGRESSION_FLOW
void AmbaLink_EchoCh_Callback(char *Msg)
{
#define AMBALINK_BOOTDONE "S99bootdone"
#define REGRESSION_EXEC_MAGIC "rexe: "

    AmbaMisra_TouchUnused(Msg);

    if (0 == SvcWrap_strcmp(Msg, AMBALINK_BOOTDONE)) {
        SVC_APP_STAT_LINUX_s LinuxStatus = {0};

        /* Got Linux boot done message */
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "#### linux boot done", 0U, 0U);
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime(SVC_TIME_LINUX_BOOT_DONE, "LINUX BOOT DONE");
        SvcTime_PrintBootTimeLinux();
        #endif

        (void)SvcSysStat_Get(SVC_APP_STAT_LINUX, &LinuxStatus);
        LinuxStatus.Status |= SVC_APP_STAT_LINUX_READY;
        (void)SvcSysStat_Issue(SVC_APP_STAT_LINUX, &LinuxStatus);

        SvcCvRegression_ExecInit();
    } else if (0 == AmbaUtility_StringCompare(Msg, REGRESSION_EXEC_MAGIC, AmbaUtility_StringLength(REGRESSION_EXEC_MAGIC))) {
        /* execute command requested by Linux */
        SvcCvRegression_ExecAddCmd(Msg + SvcWrap_strlen(REGRESSION_EXEC_MAGIC));
    } else {
        /* show message from Linux */
        AmbaPrint_PrintStr5("[Linux]%s", Msg, NULL, NULL, NULL, NULL);
    }
}
#else
void AmbaLink_EchoCh_Callback(char *Msg)
{
    #define AMBALINK_BOOTDONE "S99bootdone"
    #define EXEC_CMD_MAGIC "rexec: "
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    #define WIFI_BOOTDONE "wifibootdone"
    #define BT_BOOTDONE "btbootdone"
    #define ITE_ON "lteon"
    #define ITE_BOOTDONE "ltebootdone"
    #define USB_WIFI_BOOTDONE "usbwifibootdone"
#endif
    SVC_APP_STAT_LINUX_s LinuxStatus = {0};
    const char *pCmd;

    AmbaMisra_TouchUnused(Msg);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (0 == SvcWrap_strcmp(Msg, AMBALINK_BOOTDONE)) {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "#### linux boot done", 0U, 0U);

        app_timer_unregister(TIMER_30S, ControlTask_LnxBoot_Timeout_Handler);
        LinuxStatus.Status = SVC_APP_STAT_LINUX_READY;
        (void)SvcSysStat_Issue(SVC_APP_STAT_LINUX, &LinuxStatus);
    } else if (0 == AmbaUtility_StringCompare(Msg, EXEC_CMD_MAGIC, AmbaUtility_StringLength(EXEC_CMD_MAGIC))) {
        /* execute command requested by Linux */
        pCmd = &Msg[SvcWrap_strlen(EXEC_CMD_MAGIC)];
        SvcShell_ExecAddCmd(pCmd);
    } else if (0 == SvcWrap_strcmp(Msg, WIFI_BOOTDONE)) {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "#### wifi boot done", 0U, 0U);

        LinuxStatus.Status = SVC_APP_STAT_WIFI_READY;
        (void)SvcSysStat_Issue(SVC_APP_STAT_LINUX, &LinuxStatus);
    } else if (0 == SvcWrap_strcmp(Msg, BT_BOOTDONE)) {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "#### bt boot done", 0U, 0U);

        LinuxStatus.Status = SVC_APP_STAT_BT_READY;
        (void)SvcSysStat_Issue(SVC_APP_STAT_LINUX, &LinuxStatus);
    } else if (0 == SvcWrap_strcmp(Msg, ITE_ON)) {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "#### lte on", 0U, 0U);

        LinuxStatus.Status = SVC_APP_STAT_LTE_ON;
        (void)SvcSysStat_Issue(SVC_APP_STAT_LINUX, &LinuxStatus);
    } else if (0 == SvcWrap_strcmp(Msg, ITE_BOOTDONE)) {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "#### lte boot done", 0U, 0U);

        LinuxStatus.Status = SVC_APP_STAT_LTE_READY;
        (void)SvcSysStat_Issue(SVC_APP_STAT_LINUX, &LinuxStatus);
    } else if (0 == SvcWrap_strcmp(Msg, USB_WIFI_BOOTDONE)) {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "#### usb wifi boot done", 0U, 0U);

        LinuxStatus.Status = SVC_APP_STAT_USB_WIFI_READY;
        (void)SvcSysStat_Issue(SVC_APP_STAT_LINUX, &LinuxStatus);
    }
#else
    (void)SvcSysStat_Get(SVC_APP_STAT_LINUX, &LinuxStatus);
    if (0 == SvcWrap_strcmp(Msg, AMBALINK_BOOTDONE)) {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "#### linux boot done", 0U, 0U);
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime(SVC_TIME_LINUX_BOOT_DONE, "LINUX BOOT DONE");
        SvcTime_PrintBootTimeLinux();
        #endif

        LinuxStatus.Status |= SVC_APP_STAT_LINUX_READY;
        (void)SvcSysStat_Issue(SVC_APP_STAT_LINUX, &LinuxStatus);

        SvcShell_ExecTaskInit();
    } else if (0 == AmbaUtility_StringCompare(Msg, EXEC_CMD_MAGIC, AmbaUtility_StringLength(EXEC_CMD_MAGIC))) {
        /* execute command requested by Linux */
        pCmd = &Msg[SvcWrap_strlen(EXEC_CMD_MAGIC)];
        SvcShell_ExecAddCmd(pCmd);
    } else {
        /* show message from Linux */
        AmbaPrint_PrintStr5("[Linux]%s", Msg, NULL, NULL, NULL, NULL);
    }
#endif
}
#endif
#endif

#if defined(CONFIG_ICAM_STLCAP_USED)
static UINT32 LiveviewStopForStill(void)
{
    UINT32 Rval;

#if defined(CONFIG_BUILD_COMSVC_IMGFRW)
    Rval = SvcImgTask_Lock();
    if (SVC_OK == Rval) {
        Rval = SvcImgTask_Stop();
    }
#else
    Rval = SVC_OK;
#endif
#if defined(CONFIG_ICAM_SWPLL_CONTROL)
    if (SVC_OK == Rval) {
        Rval = SvcSwPll_Delete();
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSwPll_Delete() failed with %d", Rval, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif
#if defined(CONFIG_SVC_VIN_ERR_CTRL_USED)
    /* Disable vin error task */
    if (SVC_OK == Rval) {
        Rval = SvcVinErrTask_Stop();
    }
#endif
#if defined(CONFIG_BUILD_COMSVC_MONFRW)
    if (SVC_OK == Rval) {
        Rval = SvcSafeTask_Stop();
    }
#endif
    if (Rval == OK) {
        Rval = SvcLiveviewTask_Stop();
    }
    if (Rval == OK) {
        Rval = SvcVinCtrlTask_Destroy();
    }
    if (Rval == OK) {
        Rval = SvcVinBufMonTask_Delete();
    }
    return Rval;
}

static UINT32 LiveviewStartFromStill(void)
{
    UINT32 Rval;

    Rval = SvcVinCtrlTask_Config();
    if (Rval == OK) {
        Rval = SvcVinCtrlTask_WaitSrcReady();
    }
    if (Rval == OK) {
        Rval = SvcLiveviewTask_Config();
    }

#if defined(CONFIG_ICAM_IMGCAL_USED)
    if (Rval == OK) {
        Rval = SvcCalibTask_Start();
    }
#endif

#if defined(CONFIG_BUILD_COMSVC_IMGFRW)
    if (Rval == OK) {
        Rval = SvcImgTask_Config();
    }
#endif
#if defined(CONFIG_BUILD_COMSVC_MONFRW)
    /* Enable safe task */
    if (SVC_OK == Rval) {
        Rval = SvcSafeTask_Config();
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSafeTask_Config() failed with %d", Rval, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_SVC_VIN_ERR_CTRL_USED)
    /* Enable vin error task */
    if (SVC_OK == Rval) {
        if (SVC_OK != SvcVinErrTask_Start()) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinErrTask_Start() failed", 0U, 0U);
        }
    }
#endif

    if (Rval == OK) {
        if (SVC_OK != SvcVinBufMonTask_Create()) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinBufMonTask_Create() failed", 0U, 0U);
        }
    }

    if (Rval == OK) {
        Rval = SvcLiveviewTask_Start();
    }
#if defined(CONFIG_ICAM_SWPLL_CONTROL)
    if (SVC_OK == Rval) {
        SVC_SW_PLL_CFG_s Cfg;
        Cfg.Priority = 60;
        Cfg.CpuBits = 0;
        Rval = SvcSwPll_Create(&Cfg);
    }
#endif
    return Rval;
}
#endif

static UINT32 ControlTask_BistOn(void)
{
    UINT32  Rval = SVC_OK;

#if defined(CONFIG_ICAM_BIST_UCODE)
    SvcUcBIST_CtrlReset();

    Rval = SvcUcBIST_Ctrl(UC_BIST_STAGE_C2Y);
    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "enable ucode c2y BIST", 0U, 0U);
    } else {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "## fail to enable ucode c2y BIST", 0U, 0U);
    }

    Rval = SvcUcBIST_Ctrl(UC_BIST_STAGE_Y2Y);
    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "enable ucode y2y BIST", 0U, 0U);
    } else {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "## fail to enable ucode y2y BIST", 0U, 0U);
    }
#endif

    return Rval;
}

/**
* Control task for Autoexec. The function is used to execute the script in SD card
* @return ErrorCode
*/
static UINT32 ControlTask_SystemCfg(void)
{
#if defined(CONFIG_THREADX)
    UINT32 RetVal = SVC_OK;
    const char ScriptName[32] = {"c:\\system_cfg.ash"};
    AMBA_FS_FILE_INFO_s  Fstat;
    AMBA_SD_CARD_STATUS_s CardStatus;

    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "ControlTask_SystemCfg ...", 0U, 0U);

    do {
        RetVal = AmbaSD_GetCardStatus(0, &CardStatus);
        if (SVC_OK != RetVal) {
            break;
        }
        (void)AmbaKAL_TaskSleep(1);
    } while (CardStatus.CardIsInit == (UINT8)0);

    if (SVC_OK == RetVal) {
        RetVal = AmbaFS_GetFileInfo(ScriptName, &Fstat);
    } else {
        RetVal = AMBA_FS_ERR_SYSTEM;
    }

    if (AMBA_FS_ERR_NONE == RetVal) {
        RetVal = SvcShell_ExecScript(ScriptName);
    }

    if (SVC_OK == RetVal) {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "ControlTask_SystemCfg Done", 0U, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "ControlTask_SystemCfg No script is executed.", 0U, 0U);
    }
#endif
    return SVC_OK;
}

/**
* Control task for Standby. The function is used to enter standby mode
* @return ErrorCode
*/
static UINT32 ControlTask_CmdMode(void)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&RetVal);

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "This is Command mode", 0U, 0U);

    /* Update the OperationMode into the user preference */
    if (SVC_OK == RetVal) {
        SVC_USER_PREF_s *pSvcUserPref;
        UINT32  PrefBufSize;
        ULONG   PrefBufAddr;

        RetVal = SvcUserPref_Get(&pSvcUserPref);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcUserPref_Get() failed with %d", RetVal, 0U);
        } else {
            if (pSvcUserPref->OperationMode != 0U) {
                pSvcUserPref->OperationMode = 0U;   /* Command mode */

#ifdef CONFIG_ICAM_THMVIEW_IN_DPX
                {
                    UINT32 PbkFormatId = 0U;
                    if (SVC_OK == SvcResCfgTask_GetPbkModeId(&PbkFormatId)) {
                        if (PbkFormatId == pSvcUserPref->FormatId) {
                            pSvcUserPref->FormatId = 0U;
                        }
                    }
                }
#endif
                SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

                RetVal = SvcPref_Save(PrefBufAddr, PrefBufSize);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcUserPref_Save() failed with %d", RetVal, 0U);
                } else {
                    SvcLog_OK(SVC_LOG_CONTROL_TASK, "Save operation mode \"Command\"(%d) to user preference", 0U, 0U);
                }
                /* Reboot to command mode if boot status is not command mode */
                if (SVC_OK != AmbaSYS_Reboot()) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "AmbaSYS_Reboot() failed!!", 0U, 0U);
                }
            }
        }
    }

#if defined(CONFIG_ICAM_FW_UPDATER_USED)
    (void) SvcFwUpdateTask_Start();
#endif

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "ControlTask_CmdMode() done", 0U, 0U);
    }

    return RetVal;
}

/**
* Control task for Update format id. The function will read from preference and apply
* @return ErrorCode
*/
static UINT32 ControlTask_FormatIdUpdate(void)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&RetVal);

    /* Update the OperationMode into the user preference */
    if (SVC_OK == RetVal) {
        SVC_USER_PREF_s *pSvcUserPref;

        RetVal = SvcUserPref_Get(&pSvcUserPref);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcUserPref_Get() failed with %d", RetVal, 0U);
        } else {
            /* To get latest FormatId */
            RetVal = SvcUserPref_Get(&pSvcUserPref);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcUserPref_Get() failed with %d", RetVal, 0U);
                SvcFlowControl_Assert();
            }

            /* Configure ResCfg */
            RetVal = SvcResCfgTask_Config(pSvcUserPref->FormatId);
            if (RetVal == SVC_OK) {
                SvcClkSwTask_Update();
            } else {
                SvcLog_NG(SVC_LOG_CONTROL_TASK,"SvcResCfgTask_Config() failed with %d", RetVal, 0U);
                SvcFlowControl_Assert();
            }
        }
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "Update FormatId done", 0U, 0U);
    }

    return RetVal;
}

static UINT32 ControlTask_DataReload(void)
{
    UINT32        RetVal = SVC_OK;

#ifdef CONFIG_BUILD_CV
    /* Initialize CV task */
    RetVal = SvcDataLoader_Load(2U);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDataLoader_Load() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

    return RetVal;
}

/**
* Start CV Boot
* @return ErrorCode
*/
static UINT32 ControlTask_CvInit(void)
{
    UINT32        RetVal = SVC_OK;

#ifdef CONFIG_BUILD_CV
    RetVal = SvcCvCtrlTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvCtrlTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
    return RetVal;
}

/**
* Config CV
* @return ErrorCode
*/
static UINT32 ControlTask_CvConfig(void)
{
    UINT32 RetVal = SVC_OK;

#ifdef CONFIG_BUILD_CV
    /* Configure CV channels */
    RetVal = SvcCvCtrlTask_Config();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvCtrlTask_Config() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
    return RetVal;
}

/**
* Start CV
* @return ErrorCode
*/
static UINT32 ControlTask_CvStart(void)
{
    UINT32 RetVal = SVC_OK;

#ifdef CONFIG_BUILD_CV
    /* Boot up CV task before DSP start */
    RetVal = SvcCvMainTask_Start();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvMainTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
    return RetVal;
}

/**
* Stop CV
* @return ErrorCode
*/
static UINT32 ControlTask_CvStop(void)
{
UINT32 RetVal = SVC_OK;

#ifdef CONFIG_BUILD_CV
    /* Stop CV channels */
    RetVal = SvcCvCtrlTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvCtrlTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Stop CV task before DSP start */
    RetVal = SvcCvMainTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvMainTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
    return RetVal;
}

/**
* Start DSP Boot
* @return ErrorCode
*/
static UINT32 ControlTask_DspBoot(void)
{
    UINT32            RetVal;
    SVC_USER_PREF_s   *pSvcUserPref;

    /* Init SVC DSP INT Task */
    {
        static UINT32 TaskCreated = 0U;

        if (TaskCreated == 0U) {
            RetVal = AmbaDspInt_TaskCreate(3U, 0x01U);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "AmbaDspIntMgr_Create() failed with %d", RetVal, 0U);
                SvcFlowControl_Assert();
            } else {
                TaskCreated = 1U;
            }
        }
    }

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcUserPref_Get() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    if (pSvcUserPref->OperationMode == 1U) {    /* Liveview mode */
        RetVal = SvcDspTask_Start(AMBA_DSP_SYS_STATE_LIVEVIEW);
    } else if (pSvcUserPref->OperationMode == 2U) { /* Playback mode */
        RetVal = SvcDspTask_Start(AMBA_DSP_SYS_STATE_PLAYBACK);
    } else {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "Dsp boot only valid when operation mode 1 or 2, current : %d", pSvcUserPref->OperationMode, 0U);
    }

    /* Bootup the DSP */
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDspTask_Start() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    return RetVal;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static int linux_boot_pre_check(void)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    AMBA_PARTITION_ENTRY_s *pPartEntry = NULL;

    if (AmbaSD_IsCardPresent(SD_CHANNEL)) {
        int timeout = 50;
        while (timeout >= 0) {
            if (app_helper.sd_init_done) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "sd_init_done", 0U, 0U);
                break;
            }
            msleep(100);
            timeout--;
        }
    }
    SvcLog_NG(SVC_LOG_CONTROL_TASK, "usb_uvc_mode=%d", dqa_test_script.usb_uvc_mode, 0U);
    if (dqa_test_script.usb_uvc_mode) {
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        unsigned char value = 0;
        if (app_helper.check_usb_left_connected()) {
            //switch usb
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
            value |= 0x04;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);
        } else if (app_helper.check_usb_right_connected()) {
            //switch usb
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
            value &= ~0x04;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);
        }
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
        app_helper.gpio_set(GPIO_PIN_33, 0);
#endif
        dqa_test_script_run_uvc(dqa_test_script.usb_uvc_mode - 1);
        return -1;
    }
    if (app_util_check_lte_usb_mass_storage()) {
        return -1;
    }
    SvcUserPref_Get(&pSvcUserPref);
#if 0
    gnss_task_start();
    if (dqa_test_script.gps_debug) {
        gnss_debug_task_start();
    }
#endif
#if defined(CONFIG_GNSS_PPS_TIME_SYNC_ENABLE)
    gnss_pps_task_start();
#endif
    if (pSvcUserPref->LinuxBoot == 0) {
        return -1;
    }
    app_timer_register(TIMER_30S, ControlTask_LnxBoot_Timeout_Handler);
    pPartEntry = AmbaRTSL_NandGetPartEntry(1, AMBA_USER_PARTITION_SYS_SOFTWARE);
    if ((pPartEntry != NULL) && (pPartEntry->ActualByteSize != 0U) && (pPartEntry->Attribute & FW_PARTITION_ACTIVE_FLAG)) {
        AmbaLink_SetBootFromA(1);
        AmbaLink_SetKernelPartId(AMBA_USER_PARTITION_LINUX_KERNEL);
    } else {
        AmbaLink_SetBootFromA(0);
        AmbaLink_SetKernelPartId(AMBA_USER_PARTITION_LINUX_KERNEL_B);
    }
    if (app_helper.linux_console_enable >= 0) {
        AmbaLink_SetLinuxConsoleEnable(app_helper.linux_console_enable);
    } else {
        AmbaLink_SetLinuxConsoleEnable(pSvcUserPref->LinuxConsole == OPTION_ON);
    }
    if (app_helper.factory_reset_flag/* || app_helper.hard_reset_flag*/) {
        AmbaLink_SetEraseData(1);
    } else {
        AmbaLink_SetEraseData(0);
    }

    return 0;
}
#endif

static UINT32 ControlTask_LnxBoot(void)
{
#if defined(CONFIG_AMBALINK_BOOT_OS)
    UINT32 Rval;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (linux_boot_pre_check() < 0) {
        return 0;
    }
#endif
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_LINUX_BOOT_START, "LINUX BOOT START");
    #endif
    (void) AmbaPrint_ModuleSetAllowList(AMBALINK_MODULE_ID, 1U);

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_LINUX_IPC_INIT_DONE, "AMBA IPC INIT DONE");
    #endif
    AmbaPrint_PrintStr5("AmbaLink_OS start", NULL, NULL, NULL, NULL, NULL);
    AmbaLink_OS();

    AmbaPrint_PrintStr5("AmbaLink_OS done", NULL, NULL, NULL, NULL, NULL);
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_LINUX_OS_DONE, "AMBALINK OS DONE");
    #endif
#if defined(CONFIG_OPENAMP)
    AmbaPrint_PrintStr5("AmbaLink_RpmsgInit start", NULL, NULL, NULL, NULL, NULL);
    Rval = AmbaLink_RpmsgInit(RPMSG_DEV_OAMP);
    if (Rval == OK) {
        AmbaPrint_PrintStr5("AmbaLink_RpmsgInit done", NULL, NULL, NULL, NULL, NULL);
    } else {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "AmbaLink_RpmsgInit return 0x%x", Rval, 0U);
    }
#elif !defined(CONFIG_AMBALINK_RPMSG_G2)
    AmbaPrint_PrintStr5("AmbaLink_RpmsgInit() start", NULL, NULL, NULL, NULL, NULL);
    Rval = AmbaLink_RpmsgInit(RPMSG_DEV_AMBA);
    if (Rval == OK) {
        AmbaPrint_PrintStr5("AmbaLink_RpmsgInit done", NULL, NULL, NULL, NULL, NULL);
    } else {
        SvcLog_DBG(SVC_LOG_CONTROL_TASK, "AmbaLink_RpmsgInit return 0x%x", Rval, 0U);
    }
#else
    Rval = SVC_OK;
#endif
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_LINUX_RPMSG_INIT_DONE, "AMBALINK RPMSG INIT DONE");
    #endif

    //ambalink defapp
    {
        extern void AmbaLink_StartBasicRpmsgApp(void);
        AmbaPrint_PrintStr5("AmbaLink_StartBasicRpmsgApp start", NULL, NULL, NULL, NULL, NULL);
        AmbaLink_StartBasicRpmsgApp();
        AmbaPrint_PrintStr5("AmbaLink_StartBasicRpmsgApp done", NULL, NULL, NULL, NULL, NULL);
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime(SVC_TIME_LINUX_RPMSG_INIT_APP_DONE, "AMBALINK RPMSG DEFAPP INIT DONE");
        #endif
    }

    /* wait linux boot done */
    {
        SVC_APP_STAT_LINUX_s  LnxStatus;

        AmbaSvcWrap_MisraMemset(&LnxStatus, 0, sizeof(LnxStatus));
        if (SvcSysStat_Get(SVC_APP_STAT_LINUX, &LnxStatus) == SVC_OK) {
            while ((LnxStatus.Status & SVC_APP_STAT_LINUX_READY) == 0U) {
                if (AmbaKAL_TaskSleep(10U) != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "Fail to get sleep", 0U, 0U);
                }

                if (SvcSysStat_Get(SVC_APP_STAT_LINUX, &LnxStatus) != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "Fail to get linux status", 0U, 0U);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "Fail to get linux status", 0U, 0U);
        }
    }

    return Rval;
#else
    return SVC_OK;
#endif
}

/**
* enable vout
* @return ErrorCode
*/
static UINT32 ControlTask_VoutOn(void)
{
#if SVC_ENABLE_BACKLIGHT_CONTROL
    SvcVoutCtrlTask_SetAllBacklight();
#else
    SvcVoutSrc_Ctrl(1U, 0U);
#endif
    return SVC_OK;
}

static UINT32 ControlTask_NetInit(void)
{
    UINT32 RetVal = SVC_OK;

#if defined(CONFIG_BUILD_COMMON_SERVICE_NET)
    /* enet reset */
    {
        extern UINT32 AmbaUserGPIO_EnetResetCtrl(void) GNU_WEAK_SYMBOL;
        typedef UINT32 (*SVC_ENET_RESET)(void);

        SVC_ENET_RESET  pfnEnetReset = AmbaUserGPIO_EnetResetCtrl;

        if (pfnEnetReset != NULL) {
            RetVal = pfnEnetReset();
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "AmbaUserGPIO_EnetResetCtrl failed with 0x%x", RetVal, 0U);
            }
        }
    }
#endif

    return RetVal;
}


/**
* Safe Preference
* @return ErrorCode
*/
static UINT32 ControlTask_PrefSave(void)
{
    UINT32 RetVal;
    UINT32 PrefBufSize;
    ULONG  PrefBufAddr;

    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

    RetVal = SvcPref_Save(PrefBufAddr, PrefBufSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcUserPref_Save() failed with %d", RetVal, 0U);
    } else {
        SVC_USER_PREF_s *pSvcUserPref;

        RetVal = SvcUserPref_Get(&pSvcUserPref);
        if (SVC_OK == RetVal) {
            SvcLog_OK(SVC_LOG_CONTROL_TASK, "Save user preference. Current Operation Mode = %d", pSvcUserPref->OperationMode, 0U);
        }
    }

    return RetVal;
}

static UINT32 ControlTask_RecStart(void)
{
#if defined(CONFIG_ICAM_RECORD_USED)
    UINT32 RetVal;

    RetVal = SvcRecTask_AutoStart();

    if (RetVal ==  SVC_OK){
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "Auto Record Start Successfully", 0U, 0U);
    } else {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "Auto Record Start failed", 0U, 0U);
    }

    return RetVal;
#else
    return SVC_OK;
#endif
}

static UINT32 ControlTask_Reboot(void)
{
    if (SVC_OK != AmbaSYS_Reboot()) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "AmbaSYS_Reboot() failed!!", 0U, 0U);
    }
    return SVC_OK;
}

static UINT32 ControlTask_DspSuspend(void)
{
    UINT32 RetVal = SVC_OK;
#ifdef CONFIG_ICAM_DSP_SUSPEND
#if defined(CONFIG_ATF_SUSPEND_SRAM)
    SvcDisplayTask_Stop();
    AmbaKAL_TaskSleep(100);
#endif
    if (SVC_OK != SvcDSP_Suspend()) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDSP_Suspend() failed", 0U, 0U);
        RetVal = SVC_NG;
    }
#endif
    return RetVal;
}

static UINT32 ControlTask_DspResume(void)
{
    UINT32 RetVal = SVC_OK;
#ifdef CONFIG_ICAM_DSP_SUSPEND
    if (SVC_OK != SvcDSP_Resume()) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDSP_Resume() failed", 0U, 0U);
        RetVal = SVC_NG;
    }
#endif
    return RetVal;
}

#ifdef CONFIG_ICAM_REBEL_USAGE
static void* ExtWdt_Entry(void* EntryArg)
{
    const volatile UINT32 condition = 1U;
    const ULONG *pArg;
    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    while (condition == 1U) {
        UINT32 RetVal;
        RetVal  = AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_LOW);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_HIGH);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_LOW);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_HIGH);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_LOW);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |=  AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_HIGH);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_LOW);
        RetVal |= AmbaKAL_TaskSleep(1000);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "ExtWdt_Entry SvcExtWdtPull NG", 0U, 0U);
        } else {
//            SvcLog_DBG(SVC_LOG_CONTROL_TASK, "SvcExtWdtPull: sent GPIO 007 signal in ExtWdt_Entry", 0U, 0U);
        }
    }

    return NULL;
}

#define SVC_ExtWdt_TASK_STACK_SIZE (0x10000)
static void SvcExtWdtPull(void)
{
    UINT32 Rval;
    static SVC_TASK_CTRL_s          SvcExtWdtCtrl                                 GNU_SECTION_NOZEROINIT;
    static UINT8                    SvcFcTaskStack[SVC_ExtWdt_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    /* Create FC task */
    SvcExtWdtCtrl.Priority   = 100;
    SvcExtWdtCtrl.EntryFunc  = ExtWdt_Entry;
    SvcExtWdtCtrl.pStackBase = SvcFcTaskStack;
    SvcExtWdtCtrl.StackSize  = SVC_ExtWdt_TASK_STACK_SIZE;
    SvcExtWdtCtrl.CpuBits    = 0x2U;
    SvcExtWdtCtrl.EntryArg   = 0x0U;

    {
        UINT32 RetVal;
        RetVal  = AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_LOW);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_HIGH);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_LOW);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_HIGH);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_LOW);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |=  AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_HIGH);
        RetVal |= AmbaKAL_TaskSleep(1);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_007, GPIO_LV_LOW);
        RetVal |= AmbaKAL_TaskSleep(1);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcExtWdtPull NG", 0U, 0U);
        } else {
//            SvcLog_DBG(SVC_LOG_CONTROL_TASK, "SvcExtWdtPull: sent GPIO 007 signal", 0U, 0U);
        }
    }

    /* Create data loader task */
    Rval = SvcTask_Create("SvcExtWdtTask", &SvcExtWdtCtrl);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcExtWdtPull: SvcTask_Create failed, Rval 0x%X", Rval, 0U);
    } else {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcExtWdtPull: SvcTask_Create success", 0U, 0U);
    }
}
#endif

/**
* Show the ControlTask cmd usage
* @return ErrorCode
*/
static void ControlTaskCmdUsage(void)
{
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "Cmd Usage:", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           cmdmode:  change to command mode", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           icam_liv: enable icam basic function - liveview", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           icam_pbk: enable icam basic function - playback", 0U, 0U);
    SvcLog_DBG(SVC_LOG_CONTROL_TASK, "           cvboot:   start cv, only valid before icam_liv", 0U, 0U);
}

/**
* Initialize all the main function.
* Including "Liveview" task, "Img" task, "Display" task, "Playback" task,
* "Record" task, "Osd" task, "VinCfg" task, "VoutCfg" task
* @return ErrorCode
*/
static UINT32 ControlTask_Init(void)
{
#ifdef CONFIG_ICAM_AUDIO_USED
    SVC_USER_PREF_s *pSvcUserPref;
#endif

    UINT32 RetVal;

    /* Init svc buffer layout */
    SvcBufMap_Init();

#ifdef CONFIG_ICAM_REBEL_USAGE
    /* Rebel Watch Dog Timer (WDT) */
    SvcExtWdtPull();
#endif

    /* install app command */
    SvcCmdApp_Install();

    /* Init svc user defined system status */
    RetVal = SvcSysStatTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSysStatTask_Init() fail to init svc system status", 0U, 0U);
        SvcFlowControl_Assert();
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    RetVal = SvcStgMonitorTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStgMonitorTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#ifdef CONFIG_ICAM_AUDIO_USED
    /* Initial audio codec and audio memory packing */
    RetVal = SvcUserPref_Get(&pSvcUserPref);
    SvcAudioTask_Init(pSvcUserPref->EnableDMIC);
    SvcAudioTask_Config();
#endif
    {
        SVC_USER_PREF_s *tmpSvcUserPref = NULL;
        SvcUserPref_Get(&tmpSvcUserPref);
        if (tmpSvcUserPref->OperationMode != 0U) {
            extern int app_init(void);
            app_init();
        }
    }
#endif

    /* Init svc user defined resolution cfg */
    RetVal = SvcResCfgTask_Init();
    if (RetVal == SVC_OK) {
        SvcClkSwTask_Update();
    } else {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcResCfgTask_Init() fail to init svc resolution configured", 0U, 0U);
        SvcFlowControl_Assert();
    }

    /* Init svc nvm crc check */
    RetVal = SvcNvmTask_Create();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcNvmTask_Init() fail to init svc nvm crc checking mechanism", 0U, 0U);
    }

    /* Initialize the Loader task */
    RetVal = SvcDataLoader_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSysDataLoader_Init() fail to init svc data loader", 0U, 0U);
        SvcFlowControl_Assert();
    }

#if defined(CONFIG_ICAM_IMGCAL_USED)
    /* Init Calibration Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcCalibTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCalibTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    /* Init Image/3A Cfg Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcImgTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcImgTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Init Color Balance Task */
    SvcColorBalance_Init();
#endif

#if defined(CONFIG_BUILD_CV)
    RetVal = SvcCvMainTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCvMainTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
    RetVal = ControlTask_CvInit();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "ControlTask_CvInit() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

    /* Init DSP Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcDspTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDspTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* trigger boot loading flow */
    RetVal = SvcDataLoader_Load(1U);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDataLoader_Load() fail to init svc data loader", 0U, 0U);
        SvcFlowControl_Assert();
    }

    /* Must init IK context before BufMap configure because need to know how much memory IK need */
    RetVal = SvcIKCfg_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcIKCfg_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Init Vin Cfg Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcVinCtrlTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinCtrlTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Init Vout Cfg Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcVoutCtrlTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutCtrlTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Init Liveview Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcLiveviewTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcLiveviewTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#if defined(CONFIG_ICAM_PLAYBACK_USED)
    /* Init Playback task */
    if (SVC_OK == RetVal) {
        RetVal = SvcPlaybackTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcPlaybackTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

    /* Init Display Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcDisplayTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDisplayTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Init SVC DSP INT */
    if (SVC_OK == RetVal) {
        RetVal = AmbaDspInt_SysInit();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "AmbaDspIntMgr_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

#if defined(CONFIG_BUILD_MONFRW_GRAPH)
    /* Init Safe cfg Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcSafeTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSafeTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
#if defined(CONFIG_SVC_VOUT_ERR_CTRL_USED)
        /* Init vout error task */
        if (SVC_OK == RetVal) {
            if (SVC_OK != SvcVoutErrTask_Init()) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutErrTask_Init() failed", 0U, 0U);
            }
        }
#endif
    }
#endif

#if defined(CONFIG_SVC_VIN_ERR_CTRL_USED)
    /* Init vin error task */
    if (SVC_OK == RetVal) {
        if (SVC_OK != SvcVinErrTask_Init()) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinErrTask_Init() failed", 0U, 0U);
        }
    }
#endif

#if defined(CONFIG_ICAM_RECORD_USED)
    /* Init Record Cfg Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcRecTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRecTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
#ifdef CONFIG_ICAM_AUDIO_USED
    /* Initial audio codec and audio memory packing */
    RetVal = SvcUserPref_Get(&pSvcUserPref);
    SvcAudioTask_Init(pSvcUserPref->EnableDMIC);
#endif
#endif

    /* Init Osd */
    if (SVC_OK == RetVal) {
        RetVal = SvcOsdTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcOsdTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Init Gui Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcGuiTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcGuiTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

#if defined(CONFIG_ICAM_VIEWCTRL_USED)
    /* Init Mirror Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcEmrTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcEmrTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#ifdef CONFIG_ICAM_GUI_MENU_USED
    /* Init Menu Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcMenuTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcMenuTask_InfoMenu() failed with %d", RetVal, 0U);
            /* No need to assert even menu failed */
        }
    }
#endif

#if defined(CONFIG_BUILD_CV)
#if defined(CONFIG_THREADX)
    /* Init Warning Icon Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcWarnIconTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcWarnIconTask_Init() failed with %d", RetVal, 0U);
            /* No need to assert even menu failed */
        }
    }
#endif
#endif

#if defined(CONFIG_ICAM_FW_UPDATER_USED)
    /* Init FwUpdate Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcFwUpdateTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcFwUpdateTask_Init() failed with %d", RetVal, 0U);
        }
    }
#endif

#if defined(CONFIG_ICAM_IMGRAWCAP_USED)
    /* Init Raw capture module */
    if (SVC_OK == RetVal) {
        RetVal = SvcRawCapTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRawCapTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_RAWENC_USED)
    /* Init Raw encode module */
    if (SVC_OK == RetVal) {
        RetVal = SvcRawEncTask_Init();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRawEncTask_Init() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_USB_USED)
    {
        UINT8   *pUsbCaMem, *pUsbNcMem;
        ULONG   UsbCaBase = 0U, UsbNcBase = 0U;
        UINT32  UsbCaSize = 0U, UsbNcSize = 0U;

        if (SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_USB_CA, &UsbCaBase, &UsbCaSize) != SVC_OK) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "Fail to request FMEM_ID_USB_CA memory", 0U, 0U);
        }

        if (SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_USB_NC, &UsbNcBase, &UsbNcSize) != SVC_OK) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "Fail to request FMEM_ID_USB_CA memory", 0U, 0U);
        }

        if ((UsbCaSize != 0U) && (UsbNcSize != 0U)) {
            AmbaMisra_TypeCast(&pUsbCaMem, &UsbCaBase);
            AmbaMisra_TypeCast(&pUsbNcMem, &UsbNcBase);
            AmbaSvcUsb_MemInit(pUsbCaMem, UsbCaSize, pUsbNcMem, UsbNcSize);
        }
    }
#endif

#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    RetVal = SvcStgMonitorTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcStgMonitorTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_BUILD_COMAMBA_CODEC_COMMON)
    /* init vfs and file fifo task */
    {
        UINT32             Err;
        ULONG              BufBase;
        AMBA_VFS_INIT_s    Cfg = {0};

        AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(Cfg));

        /* init VFS */
        Err = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_VFS, &BufBase, &(Cfg.BufSize));
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcBuffer_Request %u failed", FMEM_ID_VFS, 0U);
        } else {
            Cfg.BufAddr      = BufBase;
            Cfg.TaskPriority = SVC_VFS_TASK_PRI;
            Cfg.TaskCpuBits  = SVC_VFS_TASK_CPU_BITS;
            if (g_pPlatCbEntry != NULL) {
                Cfg.pCbCacheChk = g_pPlatCbEntry->pCbCacheChk;
                Cfg.pCbCacheCln = g_pPlatCbEntry->pCbCacheCln;
                Cfg.pCbCacheInv = g_pPlatCbEntry->pCbCacheInv;
                Cfg.pCbVirToPhy = g_pPlatCbEntry->pCbVirToPhy;
            }

            RetVal = AmbaVFS_Init(&Cfg);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "## fail to init AmbaVfs", 0U, 0U);
            }
        }

        /* init BitsFifo */
        {
            AMBA_BFIFO_INIT_s  Init;

            /* bits fifo */
            Err = AmbaWrap_memset(&Init, 0, sizeof(AMBA_BFIFO_INIT_s));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "AmbaWrap_memset failed(%u)", Err, 0U);
            }

            Init.TaskPriority = SVC_BITS_FIFO_TASK_PRI;
            Init.TaskCpuBits  = SVC_BITS_FIFO_TASK_CPU_BITS;

            Err = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_BITS_FIFO, &BufBase, &(Init.BufSize));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcBuffer_Request failed(%u)", FMEM_ID_BITS_FIFO, 0U);
                RetVal = SVC_NG;
            } else {
                Init.BufAddr = BufBase;
            }

            if (RetVal == SVC_OK) {
                Err = AmbaBitsFifo_Init(&Init);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "AmbaBitsFifo_Init failed(%u)", Err, 0U);
                    RetVal = SVC_NG;
                }
            }
        }

        /* init AmbaSvcWrap */
        if (RetVal == SVC_OK) {
            AMBA_SVC_WRAP_INIT_s Init;

            Err = AmbaWrap_memset(&Init, 0, sizeof(AMBA_SVC_WRAP_INIT_s));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "AmbaWrap_memset failed(%u)", Err, 0U);
            }

            Init.pAmbaSvcCacheClean      = g_pPlatCbEntry->pAmbaCacheClean;
            Init.pAmbaSvcCacheInvalidate = g_pPlatCbEntry->pAmbaCacheInvalidate;
            Init.pAmbaSvcCacheFlush      = g_pPlatCbEntry->pAmbaCacheFlush;
            Init.pAmbaSvcCacheChk        = g_pPlatCbEntry->pCbCacheChk;

            AmbaSvcWrap_Init(&Init);
        }
    }
#endif

    /* Enable Dirt Detection module */
    {
        SVC_DIRTDETECT_TASK_FP  pFunc = SvcDirtDetectTask_Init;

        if (pFunc != NULL) {
            RetVal = pFunc();
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDirtDetect_Init() failed with %d", RetVal, 0U);
                SvcFlowControl_Assert();
            }
        }
    }

#if defined(CONFIG_ICAM_USE_AMBA_DEFOG)
    RetVal = SvcDefogTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDefogTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
    SvcEmrAdaptiveTask_Init();
#endif

    /* Initialize button control task, by default the button will be requested by ControlTask */
    RetVal = SvcButtonCtrlTask_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcButtonCtrlTask_Init() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

#if defined(CONFIG_ICAM_FW_UPDATER_USED)
    (void) SvcFwUpdateTask_Start();
#endif

    SvcCanSpeed_TaskInit();

    RetVal = ControlTask_NetInit();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "ControlTask_NetInit() failed with %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        SvcResCfgTask_Dump();
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "Task init done", 0U, 0U);
    }

    return RetVal;

}

#if defined(CONFIG_ATF_SUSPEND_SRAM)
static UINT32 ControlTask_Suspend(void)
{
#if defined(CONFIG_LINUX)
    INT32 RetVal;
    char cmd[64] = {0};
    UINT32 CoreClk, IdspClk, IdspvClk, VisionClk, n;

    AmbaSYS_GetClkFreq(AMBA_SYS_CLK_CORE, &CoreClk);
    AmbaSYS_GetClkFreq(AMBA_SYS_CLK_IDSP, &IdspClk);
    AmbaSYS_GetClkFreq(AMBA_SYS_CLK_IDSPV, &IdspvClk);
    AmbaSYS_GetClkFreq(AMBA_SYS_CLK_VISION, &VisionClk);

    // lower clock.
    RetVal = system("echo gclk_so 0 > /proc/ambarella/clock");
    RetVal = system("echo pll_out_slvsec 0 > /proc/ambarella/clock");
    RetVal = system("echo pll_out_hdmi 0 > /proc/ambarella/clock");
    RetVal = system("echo pll_out_vo2 0 > /proc/ambarella/clock");
    RetVal = system("echo gclk_core 72000000 > /proc/ambarella/clock");
    RetVal = system("echo gclk_idsp 24000000 > /proc/ambarella/clock");
    RetVal = system("echo gclk_idspv 24000000 > /proc/ambarella/clock");
    RetVal = system("echo gclk_vision 24000000 > /proc/ambarella/clock");

    SvcClock_FeatureCtrl(0U, SVC_CLK_FEA_BIT_DSP | SVC_CLK_FEA_BIT_CNN);

    RetVal = system("echo freeze > /sys/power/state");

    SvcClock_FeatureCtrl(1U, SVC_CLK_FEA_BIT_DSP | SVC_CLK_FEA_BIT_CNN);

    // raise clock back.
    n = snprintf(cmd, sizeof(cmd), "echo gclk_core %d > /proc/ambarella/clock", CoreClk);
    cmd[n] = '\0';
    RetVal = system(cmd);
    n = snprintf(cmd, sizeof(cmd), "echo gclk_idsp %d > /proc/ambarella/clock", IdspClk);
    cmd[n] = '\0';
    RetVal = system(cmd);
#if defined(CONFIG_SOC_CV5)
    n = snprintf(cmd, sizeof(cmd), "echo gclk_idspv %d > /proc/ambarella/clock", IdspvClk);
    cmd[n] = '\0';
    RetVal = system(cmd);
#endif
    n = snprintf(cmd, sizeof(cmd), "echo gclk_vision %d > /proc/ambarella/clock", VisionClk);
    cmd[n] = '\0';
    RetVal = system(cmd);
#endif

    return RetVal;
}
#endif
static UINT32 ControlTask_CvReload(void)
{
    UINT32        RetVal = SVC_OK;

#ifdef CONFIG_BUILD_CV
    /* Initialize CV task */
    RetVal = SvcDataLoader_Load(4U);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDataLoader_Load() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

    return RetVal;
}

static UINT32 ControlTask_CvbsAdcTune(void)
{
#if defined(CONFIG_ICAM_CVBS_ADC_FINE_TUNE)
    extern UINT32 SvcNtscAdjust_TuneAdcValue(void);
    return SvcNtscAdjust_TuneAdcValue();
#else
    return 0U;

#endif

}

/**
* Install the SVC flow control command/operation
* @return ErrorCode
*/
UINT32 SvcControlTask_CmdInstall(void)
{
    static SVC_FLOW_CONTROL_CMD_LIST_s SvcControlTaskCmdList[SVC_CONTROL_TASK_CMD_NUM] = {
        { SVC_CONTROL_TASK_CMD_INIT ,            ControlTask_Init           },
        { SVC_CONTROL_TASK_CMD_SYSTEM_CFG ,      ControlTask_SystemCfg      },
        { SVC_CONTROL_TASK_CMD_CMDMODE ,         ControlTask_CmdMode        },
        { SVC_CONTROL_TASK_CMD_FORMAT_UPDATE,    ControlTask_FormatIdUpdate },
        { SVC_CONTROL_TASK_CMD_LIVEVIEW ,        ControlTask_LiveviewStart  },
        { SVC_CONTROL_TASK_CMD_LIVEVIEW_STOP,    ControlTask_LiveviewStop   },
        { SVC_CONTROL_TASK_CMD_PBK_START,        ControlTask_PlaybackStart  },
        { SVC_CONTROL_TASK_CMD_PBK_STOP,         ControlTask_PlaybackStop   },
        { SVC_CONTROL_TASK_CMD_PBK_GUI_ON ,      ControlTask_PlaybackGuiOn  },
        { SVC_CONTROL_TASK_CMD_PBK_GUI_OFF,      ControlTask_PlaybackGuiOff },
        { SVC_CONTROL_TASK_CMD_CV_INIT ,         ControlTask_CvInit         },
        { SVC_CONTROL_TASK_CMD_CV_START ,        ControlTask_CvStart        },
        { SVC_CONTROL_TASK_CMD_CV_CONFIG ,       ControlTask_CvConfig       },
        { SVC_CONTROL_TASK_CMD_CV_STOP ,         ControlTask_CvStop         },
        { SVC_CONTROL_TASK_CMD_DSP_BOOT ,        ControlTask_DspBoot        },
        { SVC_CONTROL_TASK_CMD_DTL_RELOAD ,      ControlTask_DataReload     },
        { SVC_CONTROL_TASK_CMD_LNX_BOOT ,        ControlTask_LnxBoot        },
        { SVC_CONTROL_TASK_CMD_VOUT_ON ,         ControlTask_VoutOn         },
        { SVC_CONTROL_TASK_CMD_PREF_SAVE ,       ControlTask_PrefSave       },
        { SVC_CONTROL_TASK_CMD_BIST_ON ,         ControlTask_BistOn         },
        { SVC_CONTROL_TASK_CMD_REC_START,        ControlTask_RecStart       },
        { SVC_CONTROL_TASK_CMD_REBOOT,           ControlTask_Reboot         },
        { SVC_CONTROL_TASK_CMD_DSP_SUSPEND,      ControlTask_DspSuspend     },
        { SVC_CONTROL_TASK_CMD_DSP_RESUME,       ControlTask_DspResume      },
#if defined(CONFIG_ATF_SUSPEND_SRAM)
        { SVC_CONTROL_TASK_CMD_SUSPEND_LNX,      ControlTask_Suspend        },
#else
        { "", NULL },    // misra-c
#endif
        { SVC_CONTROL_TASK_CMD_DTL_CV_RELOAD ,   ControlTask_CvReload       },
        { SVC_CONTROL_TASK_CMD_CVBS_ADC_TUNE,    ControlTask_CvbsAdcTune    },
    };

    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() start", 0U, 0U);

    RetVal = SvcFlowControl_CmdHook(SvcControlTaskCmdList, SVC_CONTROL_TASK_CMD_NUM, ControlTaskCmdUsage);

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "SvcFlowControl_CmdHook() done", 0U, 0U);
    }

#if defined(CONFIG_ICAM_STLCAP_USED)
    {
        SVC_STILL_CALLBACK_s Callback;

        Callback.pLivStop  = LiveviewStopForStill;
        Callback.pLivStart = LiveviewStartFromStill;
        SvcStill_RegisterCallback(&Callback);
    }
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
#if defined(CONFIG_ICAM_IMGITUNER_USED)
    {
        AmbaItn_USBCtrl_RawEnc_Config_s RawEncCfg;
        RawEncCfg.GetIkCtxId = SvcStillTask_GetIkCtxId;
        RawEncCfg.GetCapBuffer = SvcStillTask_GetCapBuffer;
        RawEncCfg.StillRawEncodeProc = SvcStillTask_RawEncodeAmage;
        RetVal = USBDataCtrl_Init_RawEncode(&RawEncCfg);
    }
#endif
#endif
#endif

    return RetVal;
}
