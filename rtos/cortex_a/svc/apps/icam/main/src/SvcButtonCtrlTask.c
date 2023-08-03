/**
 *  @file SvcButtonCtrlTask.c
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
 *  @details svc button control task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaADC_Def.h"

#include ".svc_autogen"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcGpioButton.h"

/* svc-shared */
#include "SvcFlowControl.h"
#include "SvcButtonCtrl.h"

/* svc-icam */
#include "SvcTaskList.h"
#include "SvcControlTask.h"
#include "SvcButtonCtrlTask.h"

#if defined(SVC_BOARD_DK) && defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII)
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "SvcResCfg.h"
#include "AmbaFPD.h"
#include "AmbaFPD_MAXIM_ZS095BH3001A8H3_BII.h"
#if defined(CONFIG_FPD_MAXIM_ZS095_MAX96789)
#include "AmbaFPD_MAX96789_96752_ZS095BH.h"
#endif

#define ICAM_NUM_BUTTON             4
#define ICAM_BUTTON_CMD_LENGTH      (10U)
#else
#include "AmbaMisraFix.h"
#endif

/* Add other button if necessary */
#if defined(SVC_BOARD_DK) && defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII)
static void ButtonCtrlTask_TypeGpioInit(void);
#endif

#if defined(CONFIG_ICAM_ADC_BUTTON_USED)
static void ButtonCtrlTask_TypeAdcInit(void);
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcButtonCtrlTask_Init
 *
 *  @Description:: Initialize button control task with customized configuration
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcButtonCtrlTask_Init(void)
{
    UINT32 RetVal;
    RetVal = SvcButtonCtrl_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG("BTN_TSK", "SvcButtonCtrl_Init() fail with %d", RetVal, 0U);
    }

#if defined(SVC_BOARD_DK) && defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII)
    /* Gpio button init */
    ButtonCtrlTask_TypeGpioInit();
#endif

#if defined(CONFIG_ICAM_ADC_BUTTON_USED)
    /* Adc button init */
    ButtonCtrlTask_TypeAdcInit();
#endif

    /* Add other type button here if necessary */
    return SVC_OK;
}

#if defined(SVC_BOARD_DK) && defined(CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII)
static void ButtonCtrlTask_TypeGpioInit(void)
{
    UINT32 i, ToEnableGpioButton = 0;

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
#if defined(CONFIG_FPD_MAXIM_ZS095_MAX96789)
    for (i = 0; i < pCfg->DispNum; i++) {
        if (pCfg->DispStrm[i].pDriver == &AmbaFPD_MAX789_752_ZS095Obj) {
            ToEnableGpioButton = 1;
            break;
        }
    }
#else
    for (i = 0; i < pCfg->DispNum; i++) {
        if (pCfg->DispStrm[i].pDriver == &AmbaFPD_MAXIM_ZS095Obj) {
            ToEnableGpioButton = 1;
            break;
        }
    }
#endif
    SvcLog_OK("BTN_TSK", "ToEnableGpioButton %d", ToEnableGpioButton, 0U);

    if (ToEnableGpioButton == 1U) {
        UINT8 PinID[ICAM_NUM_BUTTON] = {
#if defined(CONFIG_BSP_CV25DK_V100) || defined(CONFIG_BSP_CV25DK_8LAYER_V110)
            3, 100, 9, 80
#endif

#if defined(CONFIG_BSP_CV22DK)
            10, 1, 2, 3 /* reworked HW */
#endif

#if defined(CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined(CONFIG_BSP_CV2XFSDK_OPTION_B)
            7, 58, 44, 48 /* reworked HW */
#endif

#if defined(CONFIG_BSP_CV28DK_8LAYER_A_V100) || defined(CONFIG_BSP_CV28DK_OPTION_B_V100)
            74, 89, 90, 91 /* reworked HW */
#endif
        };
        static char Cmd[ICAM_NUM_BUTTON][ICAM_BUTTON_CMD_LENGTH];
        UINT32 RetVal;
        /* Information for Gpio Button */
        SVC_BUTTON_GPIO_TASK_INFO_s ButtonGpioTaskInfo = {0};

        AmbaUtility_StringCopy(Cmd[0], sizeof("button_1"), "button_1");
        AmbaUtility_StringCopy(Cmd[1], sizeof("button_2"), "button_2");
        AmbaUtility_StringCopy(Cmd[2], sizeof("button_3"), "button_3");
        AmbaUtility_StringCopy(Cmd[3], sizeof("button_4"), "button_4");

        ButtonGpioTaskInfo.Priority = SVC_GPIO_BUTTON_TASK_PRI;
        ButtonGpioTaskInfo.CpuBits = SVC_GPIO_BUTTON_TASK_CPU_BITS;
        ButtonGpioTaskInfo.NumButton = ICAM_NUM_BUTTON;
        ButtonGpioTaskInfo.SamplePeriod = 5;
        ButtonGpioTaskInfo.DeBounceTarget = 4;
        ButtonGpioTaskInfo.RepeatStart = 100;
        ButtonGpioTaskInfo.RepeatInterval = 3;
        ButtonGpioTaskInfo.Debug = 0;
        ButtonGpioTaskInfo.pPinID = PinID;
        ButtonGpioTaskInfo.CmdLength = ICAM_BUTTON_CMD_LENGTH;
        ButtonGpioTaskInfo.pCmd = &Cmd[0][0];

        RetVal = SvcButtonCtrl_TypeGpioInit(&ButtonGpioTaskInfo);
        if (SVC_OK != RetVal) {
            SvcLog_NG("BTN_TSK", "SvcGpioButton_Init fail", 0U, 0U);
        }
    }
}
#endif

#if defined(CONFIG_ICAM_ADC_BUTTON_USED)
static void ButtonCtrlTask_TypeAdcInit(void)
{
    UINT32 RetVal = SVC_OK;

    static UINT32 AdcSampleData[3U] = {12U, 442U, 886U};
    static char   AdcButtonName[3U][16U] = {"button_adc_1", "button_adc_2", "button_adc_3"};

    SVC_BUTTON_ADC_TASK_INFO_s ButtonAdcTaskInfo = {
        .AdcChannel   = AMBA_ADC_CHANNEL1,
        .SampleRate   = 10000U,   /* 10KHz */
        .Period       = 50U,      /* 50 ms */
        .ButtonNum    = 3U,
        .pButtonVal   = AdcSampleData,
        .CmdLength    = 16U,
        .pCmd         = &AdcButtonName[0U][0U],
    };

    RetVal = SvcButtonCtrl_TypeAdcInit(&ButtonAdcTaskInfo);
    if (SVC_OK != RetVal) {
        SvcLog_NG("BTN_TSK", "SvcAdcButton_Init fail", 0U, 0U);
    }

    AmbaMisra_TouchUnused(&AdcSampleData);
    AmbaMisra_TouchUnused(&AdcButtonName);
}
#endif
