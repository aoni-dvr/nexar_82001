/**
 *  @file SvcButtonCtrl.c
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
 *  @details svc button control
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaShell.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcGpioButton.h"
#include "SvcAdcButton.h"
#include "SvcCmd.h"

#include "SvcButtonCtrl.h"

#define SVC_BUTTON_CTRL_LIST_SIZE_MAX   (32U)
#define SVC_BUTTON_CTRL_ID_MARK         (0xFFFF0000U)
#define SVC_BUTTON_CTRL_ID_MASK         (0x0000FFFFU)

static UINT32 ButtonCtrl_CheckCtrlID(SVC_BUTTON_CTRL_ID CtrlID);

static void ButtonCtrl_CmdInstall(void);
static void ButtonCtrl_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void ButtonCtrl_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);

/* Hooked list */
static SVC_BUTTON_CTRL_LIST_LINKER_s pSvcButtonCtrlListLinker[SVC_BUTTON_CTRL_LIST_SIZE_MAX];
static SVC_BUTTON_CTRL_ID SvcButtonCtrlRequestList[SVC_BUTTON_CTRL_LIST_SIZE_MAX] = {0U};

static UINT32 ButtonCtrl_EventHdlr(const char *pButtonName)
{
    INT32 i;
    UINT32 j, CtrlID, Idx;
    INT32 ListSize = (INT32) SVC_BUTTON_CTRL_LIST_SIZE_MAX;
    UINT32 RetVal = SVC_OK;
    UINT32 ButtonHandled = 0U;

    for (i = ListSize - 1; i >= 0; i--) { /* latest request one is the highest priority */
        CtrlID = SvcButtonCtrlRequestList[i];
        Idx = (CtrlID & SVC_BUTTON_CTRL_ID_MASK);
        if (SVC_OK == ButtonCtrl_CheckCtrlID(CtrlID)) {
            /* Get hooked button list */
            const SVC_BUTTON_CTRL_LIST_s *pList = pSvcButtonCtrlListLinker[Idx].pList;
            UINT32 ListNum = pSvcButtonCtrlListLinker[Idx].ListNum;

            if ((pSvcButtonCtrlListLinker[Idx].ButtonType == SVC_BUTTON_TYPE_GPIO) || (pSvcButtonCtrlListLinker[Idx].ButtonType == SVC_BUTTON_TYPE_ADC)) {
                if (pList != NULL) {
                    for (j = 0U; j < ListNum; j++) {
                        if (SvcWrap_strcmp(pList[j].ButtonName, pButtonName) == 0) {
                            /* If match, trigger the callback */
                            RetVal = pList[j].pFunc();
                            ButtonHandled = 1U;
                            if (SVC_OK != RetVal) {
                                SvcLog_NG("BTN_CTRL", "Button Handler failed with %d", RetVal, 0U);
                                RetVal = SVC_OK;
                            }
                            break;
                        }
                    }
                }
            }
            /* If the button event is handled by certain task, then break */
            if (ButtonHandled == 1U) {
                break;
            }
        }
    }
    return RetVal;
}

/**
* initialization of button control
* @return 0-OK, 1-NG
*/
UINT32 SvcButtonCtrl_Init(void)
{
    ButtonCtrl_CmdInstall();

    return SVC_OK;
}

/**
* gpio initialization of button control
* @param [in] pInfo info block of gpio
* @return 0-OK, 1-NG
*/
UINT32 SvcButtonCtrl_TypeGpioInit(const SVC_BUTTON_GPIO_TASK_INFO_s *pInfo)
{
    UINT32 RetVal;
    GPIO_BUTTON_CONFIG_s BtnCfg;

    BtnCfg.Priority       = pInfo->Priority;
    BtnCfg.CpuBits        = pInfo->CpuBits;
    BtnCfg.NumButton      = pInfo->NumButton;
    BtnCfg.SamplePeriod   = pInfo->SamplePeriod;
    BtnCfg.DeBounceTarget = pInfo->DeBounceTarget;
    BtnCfg.RepeatStart    = pInfo->RepeatStart;
    BtnCfg.RepeatInterval = pInfo->RepeatInterval;
    BtnCfg.Debug          = 0;
    BtnCfg.pPinID         = pInfo->pPinID;
    BtnCfg.CmdLength      = pInfo->CmdLength;
    BtnCfg.pCmd           = pInfo->pCmd;
    BtnCfg.Action         = ButtonCtrl_EventHdlr;

    RetVal = SvcGpioButton_Init(&BtnCfg);
    if (SVC_OK != RetVal) {
        SvcLog_NG("BTN_CTRL", "SvcGpioButton_Init fail", 0U, 0U);
    }

    return RetVal;
}

#if defined(CONFIG_ICAM_ADC_BUTTON_USED)
/**
* adc initialization of button control
* @param [in] pInfo info block of adc
* @return 0-OK, 1-NG
*/
UINT32 SvcButtonCtrl_TypeAdcInit(const SVC_BUTTON_ADC_TASK_INFO_s *pInfo)
{
    UINT32 RetVal = SVC_OK;
    SVC_ADC_BUTTON_CONFIG_s Config = {
        .AdcChannel  = pInfo->AdcChannel,
        .SampleRate  = pInfo->SampleRate,
        .Period      = pInfo->Period,
        .ButtonNum   = pInfo->ButtonNum,
        .pButtonVal  = pInfo->pButtonVal,
        .CmdLength   = pInfo->CmdLength,
        .pCmd        = pInfo->pCmd,
        .Action      = ButtonCtrl_EventHdlr,
    };

    RetVal = SvcAdcButton_Init(&Config);
    if (SVC_OK != RetVal) {
        SvcLog_NG("BTN_CTRL", "SvcAdcButton_Init fail", 0U, 0U);
    }

    return RetVal;
}
#endif

/**
* register of button control
* @param [in] Type type of button
* @param [in] pList list of button
* @param [in] ButtonNum number of button
* @param [out] pCtrlID id of button control
* @return 0-OK, 1-NG
*/
UINT32 SvcButtonCtrl_Register(UINT32 Type, SVC_BUTTON_CTRL_LIST_s *pList, UINT32 ButtonNum, SVC_BUTTON_CTRL_ID *pCtrlID)
{
    UINT32 RetVal;
    UINT32 i;

    for (i = 0U; i < SVC_BUTTON_CTRL_LIST_SIZE_MAX; i++) {
        if (pSvcButtonCtrlListLinker[i].pList == NULL) {
            pSvcButtonCtrlListLinker[i].ButtonType = Type;
            pSvcButtonCtrlListLinker[i].pList = pList;
            pSvcButtonCtrlListLinker[i].ListNum = ButtonNum;
            *pCtrlID = (i | SVC_BUTTON_CTRL_ID_MARK);
            break;
        }
    }

    if (i == SVC_BUTTON_CTRL_LIST_SIZE_MAX) {
        RetVal = SVC_NG;
    } else {
        RetVal = SVC_OK;
    }

    return RetVal;
}

/**
* request of button control
* @param [in] pCtrlID id of button control
* @return 0-OK, 1-NG
*/
UINT32 SvcButtonCtrl_Request(const SVC_BUTTON_CTRL_ID *pCtrlID)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;
    SVC_BUTTON_CTRL_ID CtrlID = *pCtrlID;

    if (SVC_OK == ButtonCtrl_CheckCtrlID(CtrlID)) {
        for (i = 0U; i < SVC_BUTTON_CTRL_LIST_SIZE_MAX; i++) {
            if (SvcButtonCtrlRequestList[i] == 0U) {
                SvcButtonCtrlRequestList[i] = CtrlID;
                RetVal = SVC_OK;
                break;
            }
        }
    }

    return RetVal;
}

/**
* release of button control
* @param [in] pCtrlID id of button control
* @return 0-OK, 1-NG
*/
UINT32 SvcButtonCtrl_Release(const SVC_BUTTON_CTRL_ID *pCtrlID)
{
    UINT32 RetVal = SVC_NG;
    UINT32 i;
    SVC_BUTTON_CTRL_ID CtrlID = *pCtrlID;

    if (SVC_OK == ButtonCtrl_CheckCtrlID(CtrlID)) {
        for (i = 0U; i < SVC_BUTTON_CTRL_LIST_SIZE_MAX; i++) {
            if (SvcButtonCtrlRequestList[i] == CtrlID) {
                SvcButtonCtrlRequestList[i] = 0U;
                RetVal = SVC_OK;
                break;
            }
        }

        if (SVC_OK == RetVal) {
            /* don't need to handle if the release CtrlID is the last one */
            if (i < (SVC_BUTTON_CTRL_LIST_SIZE_MAX - 1U)) {
                UINT32 MemCpySize = (UINT32)sizeof(SVC_BUTTON_CTRL_ID) * ((SVC_BUTTON_CTRL_LIST_SIZE_MAX  - 1U) - i);
                RetVal = AmbaWrap_memcpy(&SvcButtonCtrlRequestList[i], &SvcButtonCtrlRequestList[i+1U], (SIZE_t)MemCpySize);
                if (SVC_OK != RetVal) {
                    SvcLog_NG("BTN_CTRL", "SvcButtonCtrl_Release err. AmbaWrap_memcpy fail with %d", RetVal, 0U);
                }
            }
        }
    }

    return RetVal;
}

static UINT32 ButtonCtrl_CheckCtrlID(SVC_BUTTON_CTRL_ID CtrlID)
{
    UINT32 RetVal = SVC_OK;

    if ((CtrlID & SVC_BUTTON_CTRL_ID_MARK) == SVC_BUTTON_CTRL_ID_MARK) {
        RetVal = SVC_OK;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static void ButtonCtrl_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s  SvcButonCmd;

    UINT32  RetVal;

    SvcButonCmd.pName    = "svc_button";
    SvcButonCmd.MainFunc = ButtonCtrl_CmdEntry;
    SvcButonCmd.pNext    = NULL;

    RetVal = SvcCmd_CommandRegister(&SvcButonCmd);
    if (SHELL_ERR_SUCCESS != RetVal) {
        SvcLog_NG("BTN_CTRL", "## fail to install svc button command", 0U, 0U);
    }
}

static void ButtonCtrl_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    if (1U < ArgCount) {
        RetVal = ButtonCtrl_EventHdlr(pArgVector[1]);
    } else {
        RetVal = SVC_NG;
    }

    if (SVC_OK != RetVal) {
        ButtonCtrl_CmdUsage(PrintFunc);
    }
}

static void ButtonCtrl_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, j;

    PrintFunc("svc_button commands:\n");
    for (i = 0; i < SVC_BUTTON_CTRL_LIST_SIZE_MAX; i++) { /* latest request one is the highest priority */
        /* Get hooked button list */
        const SVC_BUTTON_CTRL_LIST_s *pList = pSvcButtonCtrlListLinker[i].pList;
        UINT32 ListNum = pSvcButtonCtrlListLinker[i].ListNum;

        if (pList != NULL) {
            for (j = 0U; j < ListNum; j++) {
                SvcLog_DBG("BTN_CTRL", pList[j].ButtonName, 0U, 0U);
            }
        }
    }
}
