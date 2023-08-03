/**
 *  @file SvcVoutCtrlTask.c
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
 *  @details svc vout ctrl task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaHDMI_Def.h"
#include "AmbaFPD.h"
#include "AmbaSensor.h"
#include "AmbaShell.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaPWM.h"
#include "AmbaGPIO.h"
#include "AmbaVOUT.h"
#ifdef CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#if defined(CONFIG_FPD_MAXIM_ZS095_MAX96789)
#include "AmbaFPD_MAX96789_96752_ZS095BH.h"
#endif
#endif
#ifdef CONFIG_FPD_LT9611UXC
#include "AmbaFPD_LT9611UXC.h"
#endif

/* ssp */
#include "AmbaDSP.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Event.h"

/* svc-framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcTask.h"
#include "SvcPref.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "SvcWrap.h"

/* svc-shared */
#include "SvcIK.h"
#include "SvcLiveview.h"
#include "SvcDisplay.h"
#include "SvcEnc.h"
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcSysStat.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcGuiCheckBox.h"
#include "SvcGuiTextView.h"
#include "SvcButtonCtrl.h"

/* svc-app */
#include "SvcTaskList.h"
#include "SvcInfoPack.h"
#include "SvcVoutCtrlTask.h"
#include "SvcUserPref.h"
#include "SvcAppStat.h"
#include "SvcDisplayTask.h"

#define SVC_LOG_VOUT_CTRL_TASK        "VOUT_CTRL_TASK"
#define SVC_VOUT_CTRL_SRC_TIMEOUT     (10000)

#define VOUT_CTRL_TASK_GUI_CHECKBOX_W (260U)
#define VOUT_CTRL_TASK_GUI_CHECKBOX_H (60U)
#define VOUT_CTRL_TASK_GUI_TEXTBOX1_H (25U)
#define VOUT_CTRL_TASK_GUI_TEXTBOX2_H (25U)
#define VOUT_CTRL_TASK_GUI_W          (VOUT_CTRL_TASK_GUI_CHECKBOX_W)
#define VOUT_CTRL_TASK_GUI_H          (VOUT_CTRL_TASK_GUI_CHECKBOX_H + VOUT_CTRL_TASK_GUI_TEXTBOX1_H + VOUT_CTRL_TASK_GUI_TEXTBOX2_H)

#define VOUT_CTRL_TASK_GUI_SHOW       (0x01U)
#define VOUT_CTRL_TASK_GUI_UPDATE     (0x02U)

static AMBA_KAL_EVENT_FLAG_t SvcVoutCtrlTaskEventFlag;
static UINT32 VoutCtrlTaskGuiShow = 0U;
static UINT8 VoutCtrlTaskGuiOnVout = 0U;
static UINT8 VoutCtrlTaskTarget = 0U;
static SVC_GUI_CANVAS_s VoutCtrlTaskCanvas;
static SVC_GUI_CHECKBOX_s VoutCtrlTaskCheckBox = {
    .CheckBarColor    = 0x80e04000U,
    .SelectedBoxColor = 0x8000a0c0U,
    .BackgroundColor  = 0x80202040U,
    .FontColor        = 0xffffffffU,
    .FontSize         = 2U,
    .RowGap           = 1U,
    .RowNum           = 3U,
    .ListNum          = 0U,
    .pList            = { NULL },
    .CurIdx           = 0U,
    .CheckBits        = 0U,
};

/* Parameter for vout src configuration */
static SVC_VOUT_SRC_INIT_s VoutCtrlTaskSrcInit = {0};

/* Parameter for button control */
static SVC_BUTTON_CTRL_ID VoutCtrlTaskButtonCtrl_ID = 0U;

/* Callback of app status */
static void VoutCtrlTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo);
/* Callback of VoutCtrl Gui */
static void VoutCtrlTask_DrawEntry(UINT32 VoutIdx, UINT32 Level);
static void VoutCtrlTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
static void VoutCtrlTask_SrcReady(void);

/* Button Ctrl callback */
static UINT32 VoutCtrlTask_Minus(void);
static UINT32 VoutCtrlTask_Plus(void);
static UINT32 VoutCtrlTask_Move(void);

#if SVC_ENABLE_BACKLIGHT_CONTROL

#define SVC_NUM_BL_ON_DUTY (10)

typedef struct {
    UINT32 GpioFunc;
    UINT32 PwmID;
    UINT32 BaseFreq;
    UINT32 Period;
} AMBA_DISP_BL_CONFIG_s;

static AMBA_DISP_BL_CONFIG_s BlCfg[SVC_MAX_NUM_PANEL] = {
#ifdef CONFIG_SOC_CV25
    [0] = {GPIO_PIN_99_PWM7,  AMBA_PWM_CHANNEL7, 10000,  100}, /* center */
    [1] = {GPIO_PIN_20_PWM1,  AMBA_PWM_CHANNEL1, 10000,  100}, /* left */
    [2] = {GPIO_PIN_19_PWM0,  AMBA_PWM_CHANNEL0, 10000,  100}, /* right, need rework on CV25DK */
#else
#ifdef CONFIG_SOC_CV22
    [0] = {GPIO_PIN_0_PWM3,   AMBA_PWM_CHANNEL3, 10000,  100}, /* center */
    [1] = {GPIO_PIN_41_PWM0,  AMBA_PWM_CHANNEL0, 10000,  100}, /* left */
    [2] = {GPIO_PIN_153_PWM9, AMBA_PWM_CHANNEL9, 10000,  100}, /* right */
#endif
#if defined(CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined(CONFIG_BSP_CV2XFSDK_OPTION_B)
    [0] = {GPIO_PIN_018_PWM0, AMBA_PWM_CHANNEL0, 10000,  100}, /* center */
    [1] = {GPIO_PIN_019_PWM1, AMBA_PWM_CHANNEL1, 10000,  100}, /* left */
    [2] = {GPIO_PIN_019_PWM1, AMBA_PWM_CHANNEL1, 10000,  100}, /* right */
#endif
#if defined(CONFIG_BSP_CV28DK_8LAYER_A_V100) || defined(CONFIG_BSP_CV28DK_OPTION_B_V100)
    [0] = {GPIO_PIN_92_PWM11, AMBA_PWM_CHANNEL11, 10000,  100}, /* center */
    [1] = {GPIO_PIN_92_PWM11, AMBA_PWM_CHANNEL11, 10000,  100}, /* left */
    [2] = {GPIO_PIN_92_PWM11, AMBA_PWM_CHANNEL11, 10000,  100}, /* right */
#endif
#endif
};

/**
 * Set backlight PWM duty cycle index of specified panel
 * @param [in] panel index
 * @param [in] backlight PWM duty cycle index increment, table is {10, 20, 30, 40, 50, 60, 70, 80, 90, 99}
 * return 0-OK, 1-NG
 */
UINT32 SvcVoutCtrlTask_SetBacklightIdx(UINT32 PanelID, INT32 BlIdxInc)
{
    SVC_USER_PREF_s     *pSvcUserPref;
    UINT32              *LcdBlOnDytyId;
    static const UINT32 BlOnDuty[SVC_NUM_BL_ON_DUTY] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 99};
    UINT32 Rval = SVC_NG;

    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
        SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcUserPref_Get() failed!!", 0U, 0U);
    } else {
        INT32 Temp;
        LcdBlOnDytyId = &pSvcUserPref->LcdBlOnDytyId[PanelID];
        Temp = (INT32)*LcdBlOnDytyId + BlIdxInc;
        if (Temp >= SVC_NUM_BL_ON_DUTY) {
            Temp = SVC_NUM_BL_ON_DUTY - 1;
        } else {
            if (Temp < 0) {
                Temp = 0;
            }
        }
        if (*LcdBlOnDytyId != (UINT32)Temp){
            UINT32  PrefBufSize;
            ULONG   PrefBufAddr;

            *LcdBlOnDytyId = (UINT32)Temp;
            SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
            if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "Pref save failed!!", 0U, 0U);
            } else {
                SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "Pref save successfully!!", 0U, 0U);
            }
        }
        Rval = SvcVoutCtrlTask_SetBacklight(PanelID, BlCfg[PanelID].Period, BlOnDuty[*LcdBlOnDytyId]);
    }
    return Rval;
}

/**
 * Set backlight PWM duty cycle of specified panel
 * @param [in] panel index
 * @param [in] period of duty cycle
 * @param [in] period of on duty cycle
 * return 0-OK, 1-NG
 */
UINT32 SvcVoutCtrlTask_SetBacklight(UINT32 PanelID, UINT32 Period, UINT32 OnPeriod)
{
    static UINT8 PwmConfigured[SVC_MAX_NUM_PANEL] = {0, 0, 0};
    UINT32 Rval = SVC_OK;

    if (PwmConfigured[PanelID] == 0U){
        Rval = AmbaGPIO_SetFuncAlt(BlCfg[PanelID].GpioFunc);
        if (Rval == OK) {
            SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "AmbaGPIO_SetFuncAlt func %u", BlCfg[PanelID].GpioFunc, 0);
        } else {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "AmbaGPIO_SetFuncAlt func %u failed!!", BlCfg[PanelID].GpioFunc, 0);
        }
        Rval = AmbaPWM_Config(BlCfg[PanelID].PwmID, BlCfg[PanelID].BaseFreq);
        if (Rval == OK) {
            SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "AmbaPWM_Config pwm %u freq %u", BlCfg[PanelID].PwmID, BlCfg[PanelID].BaseFreq);
        } else {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "AmbaPWM_Config pwm %u failed!!", BlCfg[PanelID].PwmID, 0U);
        }
        PwmConfigured[PanelID] = 1;
    }
    if (Rval == OK) {
        Rval = AmbaPWM_Start(BlCfg[PanelID].PwmID, Period, OnPeriod);
        if (Rval == OK) {
            SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "AmbaPWM_Start period %u on %u", Period, OnPeriod);
        } else {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "AmbaPWM_Start period %u on %u failed!!", Period, OnPeriod);
        }
    }
    return Rval;
}

/**
 * Set backlight of all panels
 * return 0-OK, 1-NG
 */
void SvcVoutCtrlTask_SetAllBacklight(void)
{
    UINT32 DispNum, i;
    SVC_DISP_INFO_s DispInfo;
    SvcDisplay_InfoGet(&DispInfo);
    DispNum = *(DispInfo.pNumDisp);
    for (i = 0; i < DispNum; i++) {
        UINT32 VoutID = DispInfo.pDispCfg[i].VoutID;
        if (VoutID == 0U) {
            (void)SvcVoutCtrlTask_SetBacklightIdx(0, SVC_NUM_BL_ON_DUTY);
        } else {
            (void)SvcVoutCtrlTask_SetBacklightIdx(1, SVC_NUM_BL_ON_DUTY);
            (void)SvcVoutCtrlTask_SetBacklightIdx(2, SVC_NUM_BL_ON_DUTY);
        }
    }
}
#endif

/**
 * Initialize tht Vout control module with customized configuration
 * return 0-OK, 1-NG
 */
UINT32 SvcVoutCtrlTask_Init(void)
{
    UINT32 RetVal;
    SVC_APP_STAT_VOUT_s VoutStatus = { .Status = SVC_APP_STAT_VOUT_CONFIGING };
    static char VoutCtrlTaskEventFlagName[16] = "VoutCtrlTaskFlag";

    /* Callback for ButtonControl */
    static SVC_BUTTON_CTRL_LIST_s VoutCtrlTaskButtonCtrl[3] = {
        { "button_1", VoutCtrlTask_Minus },
        { "button_2", VoutCtrlTask_Plus  },
        { "button_3", VoutCtrlTask_Move  },
    };

#if defined(CONFIG_SOC_CV2FS)
    static const char VoutCtrlVoutName[2U][6U] = { "REAR", "SIDE" };
#else
    static const char VoutCtrlVoutName[3U][6U] = { "REAR", "LEFT", "RIGHT" };
#endif

    SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Init() Start", 0U, 0U);

    RetVal = AmbaKAL_EventFlagCreate(&SvcVoutCtrlTaskEventFlag, VoutCtrlTaskEventFlagName);
    /* Create event flag for vin status */
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "Event flag create failed with %d", RetVal, 0U);
    }

    /* Clear event flag */
    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_EventFlagClear(&SvcVoutCtrlTaskEventFlag, 0xFFFFFFFFU);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "Event flag clear failed with %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        SVC_SYS_STAT_ID StatusID;
        RetVal |= SvcSysStat_Register(SVC_APP_STAT_MENU, VoutCtrlTask_MenuStatusCallback, &StatusID);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "Init err. Register menu status failed", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcButtonCtrl_Register(SVC_BUTTON_TYPE_GPIO, VoutCtrlTaskButtonCtrl, 3U, &VoutCtrlTaskButtonCtrl_ID);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "Init err. Button register fail", 0U, 0U);
        }
    }

#if defined(CONFIG_SOC_CV2FS)
    if (SVC_OK == RetVal) {
        UINT32 i;
        for (i = 0U; i < 2U; i++) {
            VoutCtrlTaskCheckBox.pList[i] = VoutCtrlVoutName[i];
        }
        VoutCtrlTaskCheckBox.ListNum = 2U;
        VoutCtrlTaskCheckBox.CurIdx  = VoutCtrlTaskTarget;
    }
#else
    if (SVC_OK == RetVal) {
        UINT32 i;
        for (i = 0U; i < 3U; i++) {
            VoutCtrlTaskCheckBox.pList[i] = VoutCtrlVoutName[i];
        }
        VoutCtrlTaskCheckBox.ListNum = 3U;
        VoutCtrlTaskCheckBox.CurIdx  = VoutCtrlTaskTarget;
    }
#endif

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Init() VoutSrcInit()", 0U, 0U);
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_VOUT, &VoutStatus);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Init() SvcSysStat_Issue() err", 0U, 0U);
        }

        SvcInfoPack_VoutSrcInit(&VoutCtrlTaskSrcInit);
        VoutCtrlTaskSrcInit.pInitDoneCB = VoutCtrlTask_SrcReady;
        SvcVoutSrc_Init(&VoutCtrlTaskSrcInit, 1U /* Force to start FPD */, SVC_VOUT_SRC_TASK_PRI, SVC_VOUT_SRC_TASK_CPU_BITS);
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Init() Done", 0U, 0U);
    }

    return RetVal;
}

/**
 * Configure tht Vout control module with customized configuration
 * return 0-OK, 1-NG
 */
UINT32 SvcVoutCtrlTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_VOUT_s VoutStatus = { 0U };

    SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Config() start ", 0U, 0U);

    RetVal = SvcSysStat_Get(SVC_APP_STAT_VOUT, &VoutStatus);
    if (SVC_OK == RetVal) {
        if (VoutStatus.Status == SVC_APP_STAT_VOUT_NOT_READY) {
            SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Config() VoutSrcInit()", 0U, 0U);
            if (SVC_OK != AmbaWrap_memset(&VoutStatus, 0, sizeof(SVC_APP_STAT_VOUT_s))) {
                SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Config() AmbaWrap_memset() err", 0U, 0U);
            }
            VoutStatus.Status = SVC_APP_STAT_VOUT_CONFIGING;
            RetVal = SvcSysStat_Issue(SVC_APP_STAT_VOUT, &VoutStatus);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Init() SvcSysStat_Issue() err", 0U, 0U);
            }

            SvcInfoPack_VoutSrcInit(&VoutCtrlTaskSrcInit);
            VoutCtrlTaskSrcInit.pInitDoneCB = VoutCtrlTask_SrcReady;
            SvcVoutSrc_Init(&VoutCtrlTaskSrcInit, 1U /* Force to start FPD */, SVC_VOUT_SRC_TASK_PRI, SVC_VOUT_SRC_TASK_CPU_BITS);

        } else {
            /* Do nothing */
            SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Config() Skip", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Config() SvcSysStat_Get() err", 0U, 0U);
    }

    return RetVal;
}

/**
 * Destroy Vout confiugration
 * return 0-OK, 1-NG
 */
UINT32 SvcVoutCtrlTask_Destroy(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_APP_STAT_VOUT_s VoutStatus = { 0U };

    RetVal = SvcSysStat_Get(SVC_APP_STAT_VOUT, &VoutStatus);
    if (SVC_OK == RetVal) {
        if (VoutStatus.Status == SVC_APP_STAT_VOUT_READY) {
            SvcVoutSrc_DeInit(&VoutCtrlTaskSrcInit, 1U /* Force DSP to do VOUT reset and FPD HW reset */);
        } else {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Destroy() error status (%d), should be %d", VoutStatus.Status, SVC_APP_STAT_VOUT_READY);
        }
        RetVal = AmbaKAL_EventFlagClear(&SvcVoutCtrlTaskEventFlag, 0xFFFFFFFFU);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Destroy() Event flag clear failed with %d", RetVal, 0U);
        }

        if (SVC_OK != AmbaWrap_memset(&VoutStatus, 0, sizeof(SVC_APP_STAT_VOUT_s))) {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Destory() AmbaWrap_memset() err", 0U, 0U);
        }

        VoutStatus.Status = SVC_APP_STAT_VOUT_NOT_READY;
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_VOUT, &VoutStatus);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcVoutCtrlTask_Destroy() SvcSysStat_Issue() err", 0U, 0U);
        }
    }

#ifdef CONFIG_DISP_MIPIDSI_SUPPORT
    { // reset dsp vout before reseting mipi phy
        UINT8 VoutID;
        for (VoutID = 0; VoutID < AMBA_DSP_MAX_VOUT_NUM; VoutID ++) {
            RetVal = AmbaDSP_VoutReset(VoutID);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "## VoutReset(%d) failed 0x%x", VoutID, RetVal);
            }
        }
    }
    RetVal = AmbaVout_MipiDsiSetPhyCtrl(VOUT_PHY_MIPI_DPHY_POWER_DOWN, 0U);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "AmbaVout_MipiDsiSetPhyCtrl() failed with %d", RetVal, 0U);
    }
#endif

    return RetVal;
}

/**
 * Wait vout src is ready or not
 * return 0-OK, 1-NG
 */
UINT32 SvcVoutCtrlTask_WaitSrcReady(void)
{
    UINT32 RetVal;
    UINT32 ActualFlags = {0};

    /* Get event flag, maybe timeout */
    RetVal = AmbaKAL_EventFlagGet(&SvcVoutCtrlTaskEventFlag, SVC_VOUT_CTRL_SRC_INIT_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, SVC_VOUT_CTRL_SRC_TIMEOUT);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "Event flag get failed with %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_VOUT_CTRL_TASK, "Vout src is ready", 0U, 0U);
    }

    return RetVal;
}

static void VoutCtrlTask_SrcReady(void)
{
    SVC_APP_STAT_VOUT_s VoutStatus = { .Status = SVC_APP_STAT_VOUT_READY };
    UINT32 RetVal = SVC_OK;

    RetVal = AmbaKAL_EventFlagSet(&SvcVoutCtrlTaskEventFlag, SVC_VOUT_CTRL_SRC_INIT_DONE);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "Event flag set failed with %d", RetVal, 0U);
    }

    RetVal = SvcSysStat_Issue(SVC_APP_STAT_VOUT, &VoutStatus);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "Sys Status issue failed with %d", RetVal, 0U);
    }
}

static void VoutCtrlTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal = 0U;

    SVC_APP_STAT_MENU_s *pStatus = NULL;

    AmbaMisra_TouchUnused(pInfo);   /* Misra-c fixed */
    AmbaMisra_TouchUnused(pStatus); /* Misra-c fixed */
    AmbaMisra_TouchUnused(&RetVal); /* Misra-c fixed */

    AmbaMisra_TypeCast(&pStatus, &pInfo);

    if (StatIdx == SVC_APP_STAT_MENU) {
        if ((pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) && (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_DIM)) {
            if ((VoutCtrlTaskGuiShow & VOUT_CTRL_TASK_GUI_SHOW) == 0U) {
                UINT32 OsdWidth, OsdHeight;

                VoutCtrlTaskGuiShow |= VOUT_CTRL_TASK_GUI_SHOW;
                VoutCtrlTaskGuiShow |= VOUT_CTRL_TASK_GUI_UPDATE;
                VoutCtrlTaskGuiOnVout = pStatus->Vout;
                VoutCtrlTaskCheckBox.CurIdx = 0U;
                VoutCtrlTaskCheckBox.CheckBits = 1U;

                RetVal = SvcOsd_GetOsdBufSize(VoutCtrlTaskGuiOnVout, &OsdWidth, &OsdHeight);
                if (SVC_OK == RetVal) {
                    VoutCtrlTaskCanvas.StartX = (OsdWidth - (VOUT_CTRL_TASK_GUI_W)) >> (UINT32) 1U;
                    VoutCtrlTaskCanvas.StartY = (OsdHeight - (VOUT_CTRL_TASK_GUI_H)) >> (UINT32) 1U;
                    VoutCtrlTaskCanvas.Width  = VOUT_CTRL_TASK_GUI_W;
                    VoutCtrlTaskCanvas.Height = VOUT_CTRL_TASK_GUI_H;
                }
                SvcGui_Register(VoutCtrlTaskGuiOnVout, 17U, "VoutCtrl", VoutCtrlTask_DrawEntry, VoutCtrlTask_DrawUpdate);
                RetVal = SvcButtonCtrl_Request(&VoutCtrlTaskButtonCtrl_ID);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcButtonCtrl_Request(%d) failed", VoutCtrlTaskButtonCtrl_ID, 0U);
                }
            } else {
                VoutCtrlTaskGuiShow &= ~(VOUT_CTRL_TASK_GUI_SHOW);
                RetVal = SvcButtonCtrl_Release(&VoutCtrlTaskButtonCtrl_ID);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "SvcButtonCtrl_Release(%d) failed", VoutCtrlTaskButtonCtrl_ID, 0U);
                }
                SvcGui_Unregister(VoutCtrlTaskGuiOnVout, 17U);
            }
        }
    }
}

static void VoutCtrlTask_DrawEntry(UINT32 VoutIdx, UINT32 Level)
{
    SVC_GUI_CANVAS_s CheckBoxCanvas = { .StartX = VoutCtrlTaskCanvas.StartX, .StartY = VoutCtrlTaskCanvas.StartY, .Width = VOUT_CTRL_TASK_GUI_W, .Height = VOUT_CTRL_TASK_GUI_CHECKBOX_H };
    SVC_GUI_CANVAS_s TextViewCanvas = { .StartX = VoutCtrlTaskCanvas.StartX, .StartY = VoutCtrlTaskCanvas.StartY + VOUT_CTRL_TASK_GUI_CHECKBOX_H, .Width = VOUT_CTRL_TASK_GUI_W, .Height = VOUT_CTRL_TASK_GUI_TEXTBOX1_H };
    SVC_GUI_TEXTVIEW_s TextView = {
        .BackgroundColor = 0x80202040U,
        .FontColor = 0xffffffffU,
        .FontSize = 1U,
        .LineGap = 1U,
        .pText = "1: Up 2: Down 3: Move 4: Leave",
    };
    SVC_GUI_CANVAS_s TextViewCanvas1 = { .StartX = VoutCtrlTaskCanvas.StartX, .StartY = VoutCtrlTaskCanvas.StartY + VOUT_CTRL_TASK_GUI_CHECKBOX_H + VOUT_CTRL_TASK_GUI_TEXTBOX1_H, .Width = VOUT_CTRL_TASK_GUI_W, .Height = VOUT_CTRL_TASK_GUI_TEXTBOX2_H };
    SVC_GUI_TEXTVIEW_s TextView1 = {
        .BackgroundColor = 0x80202040U,
        .FontColor = 0xffffffffU,
        .FontSize = 1U,
        .LineGap = 1U,
        .pText = "-: Up +: Down !: Move @: Leave",
    };

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((VoutCtrlTaskGuiShow & VOUT_CTRL_TASK_GUI_SHOW) > 0U) {
        SvcGuiCheckBox_Draw(VoutCtrlTaskGuiOnVout, &CheckBoxCanvas,  &VoutCtrlTaskCheckBox);
        SvcGuiTextView_Draw(VoutCtrlTaskGuiOnVout, &TextViewCanvas,  &TextView);
        SvcGuiTextView_Draw(VoutCtrlTaskGuiOnVout, &TextViewCanvas1, &TextView1);
    }
}

static void VoutCtrlTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((VoutCtrlTaskGuiShow & VOUT_CTRL_TASK_GUI_UPDATE) > 0U) {
        VoutCtrlTaskGuiShow &= ~(VOUT_CTRL_TASK_GUI_UPDATE);
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static UINT32 VoutCtrlTask_Minus(void)
{
#if SVC_ENABLE_BACKLIGHT_CONTROL
    UINT32 RetVal;
    RetVal = SvcVoutCtrlTask_SetBacklightIdx(VoutCtrlTaskTarget, -1);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "VoutCtrlTask_Minus err. Cannot set backlight idx %d", VoutCtrlTaskTarget, 0U);
    }
#endif
    return SVC_OK;
}

static UINT32 VoutCtrlTask_Plus(void)
{
#if SVC_ENABLE_BACKLIGHT_CONTROL
    UINT32 RetVal;
    RetVal = SvcVoutCtrlTask_SetBacklightIdx(VoutCtrlTaskTarget, 1);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_VOUT_CTRL_TASK, "VoutCtrlTask_Minus err. Cannot set backlight idx %d", VoutCtrlTaskTarget, 0U);
    }
#endif
    return SVC_OK;
}

static UINT32 VoutCtrlTask_Move(void)
{
    VoutCtrlTaskCheckBox.CurIdx = (VoutCtrlTaskCheckBox.CurIdx + 1U) % VoutCtrlTaskCheckBox.ListNum;
    VoutCtrlTaskCheckBox.CheckBits = ((UINT32) 1U << VoutCtrlTaskCheckBox.CurIdx);
    VoutCtrlTaskTarget = (UINT8) VoutCtrlTaskCheckBox.CurIdx;
    VoutCtrlTaskGuiShow |= VOUT_CTRL_TASK_GUI_UPDATE;

    return SVC_OK;
}
