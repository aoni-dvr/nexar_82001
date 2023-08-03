/**
*  @file SvcAdcButton.c
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
*  @details svc ADC button handler
*
*/

#include "AmbaKAL.h"
#include "AmbaADC.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"

/* svc-framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcTask.h"

/* svc-shared */
#include "SvcAdcButton.h"

#define SVC_LOG_ADC_BUTTON  "ADC_BTN"

#define SVC_ADC_BUTTON_STATUS_IDLE      (0U)
#define SVC_ADC_BUTTON_STATUS_PRESSED   (1U)
#define SVC_ADC_BUTTON_STATUS_PRESSING  (2U)

#define SVC_ADC_BUTTON_DBG_LV_1         (1U)
#define SVC_ADC_BUTTON_DBG_LV_2         (2U)

typedef struct {
    UINT32 ButtonIdx;
} SVC_ADC_BUTTON_MSG_s;

typedef struct {
    char Cmd[16U];
} SVC_ADC_BUTTON_s;

static AMBA_KAL_MSG_QUEUE_t    SvcAdcButtonMsg;
static SVC_ADC_BUTTON_CONFIG_s SvcAdcButtonConfig = {0};
static SVC_ADC_BUTTON_s        SvcAdcButton[16U];
static UINT32                  SvcAdcButtonDbgLv = 0U;

static void* AdcButton_SampleTaskEntry(void* EntryArg);
static void* AdcButton_ExecTaskEntry(void* EntryArg);

static void AdcButton_Dbg(UINT32 Level, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if ((Level & SvcAdcButtonDbgLv) > 0U) {
        SvcLog_DBG(SVC_LOG_ADC_BUTTON, pFormat, Arg1, Arg2);
    }
}

static void* AdcButton_SampleTaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ButtonStatus = SVC_ADC_BUTTON_STATUS_IDLE;
    UINT32 AdcData = 0U;
    DOUBLE AdcDataInDouble, ButtonAdcInDouble;
    UINT32 ButtonIdx = 0U;
    UINT32 PrevButtonIdx = 0xDeadbeefU;
    ULONG  ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while ( ArgVal != 0xCafeU ) {
        SVC_ADC_BUTTON_MSG_s Msg = {0};

        RetVal = AmbaADC_SingleRead(SvcAdcButtonConfig.AdcChannel, &AdcData);

        if (SVC_OK == RetVal) {
            AdcButton_Dbg(SVC_ADC_BUTTON_DBG_LV_2, "Ch : %d, Data : %d", SvcAdcButtonConfig.AdcChannel, AdcData);
        } else {
            SvcLog_NG(SVC_LOG_ADC_BUTTON, "AmbaADC_SingleRead() failed with 0x%x", RetVal, 0U);
        }


        AdcDataInDouble = (DOUBLE) AdcData;
        for (ButtonIdx = 0U; ButtonIdx < SvcAdcButtonConfig.ButtonNum; ButtonIdx++) {
            ButtonAdcInDouble = (DOUBLE) SvcAdcButtonConfig.pButtonVal[ButtonIdx];
            if ((AdcDataInDouble < (ButtonAdcInDouble * 1.5)) && (AdcDataInDouble > (ButtonAdcInDouble * 0.5))) {
                /* Reset button status if another button is pressed */
                if (PrevButtonIdx != ButtonIdx) {
                    ButtonStatus = SVC_ADC_BUTTON_STATUS_IDLE;
                }

                if (ButtonStatus == SVC_ADC_BUTTON_STATUS_IDLE) {
                    ButtonStatus = SVC_ADC_BUTTON_STATUS_PRESSED;
                } else if (ButtonStatus == SVC_ADC_BUTTON_STATUS_PRESSED) {
                    ButtonStatus = SVC_ADC_BUTTON_STATUS_PRESSING;
                } else if (ButtonStatus == SVC_ADC_BUTTON_STATUS_PRESSING) {
                    /* Do nothing now. No support continuous pressing now */
                } else {
                    SvcLog_NG(SVC_LOG_ADC_BUTTON, "Error status, should not enter (%d)", ButtonStatus, 0U);
                }

                if (ButtonStatus == SVC_ADC_BUTTON_STATUS_PRESSED) {
                    Msg.ButtonIdx = ButtonIdx;
                    RetVal = AmbaKAL_MsgQueueSend(&SvcAdcButtonMsg, &Msg, AMBA_KAL_NO_WAIT);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_ADC_BUTTON, "AmbaKAL_MsgQueueSend() failed in AdcButton_SampleTaskEntry() with 0x%x", RetVal, 0U);
                    }
                }

                PrevButtonIdx = ButtonIdx;

                break;
            }
        }

        /* Not find any button match */
        if (ButtonIdx == SvcAdcButtonConfig.ButtonNum) {
            ButtonStatus = SVC_ADC_BUTTON_STATUS_IDLE;
            PrevButtonIdx = 0xDeadbeefU;
        }

        (void) AmbaKAL_TaskSleep(SvcAdcButtonConfig.Period);

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

static void* AdcButton_ExecTaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    ULONG  ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while ( ArgVal != 0xCafeU ) {
        SVC_ADC_BUTTON_MSG_s Msg = {0};

        /* Wait for sample task issue */
        RetVal = AmbaKAL_MsgQueueReceive(&SvcAdcButtonMsg, &Msg, AMBA_KAL_WAIT_FOREVER);

        if (SVC_OK == RetVal) {

            AdcButton_Dbg(SVC_ADC_BUTTON_DBG_LV_1, "Exec callback of ButtonIdx %d", Msg.ButtonIdx, 0U);
            AdcButton_Dbg(SVC_ADC_BUTTON_DBG_LV_1, SvcAdcButton[Msg.ButtonIdx].Cmd, 0U, 0U);

            (void) SvcAdcButtonConfig.Action(SvcAdcButton[Msg.ButtonIdx].Cmd);
        } else {
            SvcLog_NG(SVC_LOG_ADC_BUTTON, "AmbaKAL_EventFlagGet() failed with 0x%x", RetVal, 0U);
        }

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}

/**
* initialization of adc button module
* @param [in] pConfig configuration of adc button module
* @return 0-OK, 1-NG
*/
UINT32 SvcAdcButton_Init(const SVC_ADC_BUTTON_CONFIG_s *pConfig)
{
#define SVC_ADC_BUTTON_TASK_STACK_SIZE  (0x1000U)
    UINT32 RetVal;
    UINT32 i;

    static SVC_ADC_BUTTON_MSG_s SvcAdcButtonMsgQue[16U];

    static UINT8 SvcAdcButtonSampleTaskStack[SVC_ADC_BUTTON_TASK_STACK_SIZE];
    static SVC_TASK_CTRL_s SvcAdcButtonSampleTaskCtrl = {
        .Priority   = 200U,
        .EntryFunc  = AdcButton_SampleTaskEntry,
        .EntryArg   = 0U,
        .pStackBase = SvcAdcButtonSampleTaskStack,
        .StackSize  = SVC_ADC_BUTTON_TASK_STACK_SIZE,
        .CpuBits    = 0x01U,
    };

    static UINT8 SvcAdcButtonExecTaskStack[SVC_ADC_BUTTON_TASK_STACK_SIZE];
    static SVC_TASK_CTRL_s SvcAdcButtonExecTaskCtrl = {
        .Priority   = 201U,
        .EntryFunc  = AdcButton_ExecTaskEntry,
        .EntryArg   = 0U,
        .pStackBase = SvcAdcButtonExecTaskStack,
        .StackSize  = SVC_ADC_BUTTON_TASK_STACK_SIZE,
        .CpuBits    = 0x01U,
    };

    static char SvcAdcButtonMsgName[32] = "SvcAdcButtonMsg";

    AmbaMisra_TouchUnused(SvcAdcButtonSampleTaskStack);
    AmbaMisra_TouchUnused(SvcAdcButtonExecTaskStack);

    RetVal = AmbaWrap_memcpy(&SvcAdcButtonConfig, pConfig, sizeof(SVC_ADC_BUTTON_CONFIG_s));
    for (i = 0U; i < SvcAdcButtonConfig.ButtonNum; i++) {
        AmbaUtility_StringCopy(SvcAdcButton[i].Cmd, SvcAdcButtonConfig.CmdLength, &pConfig->pCmd[i * SvcAdcButtonConfig.CmdLength]);
    }
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_ADC_BUTTON, "AmbaWrap_memcpy() failed with 0x%x", RetVal, 0U);
    }

    /* Config AmbaADC */
    RetVal = AmbaADC_Config(SvcAdcButtonConfig.SampleRate);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_ADC_BUTTON, "AmbaADC_Config() failed with 0x%x", RetVal, 0U);
    }

    /* Create event flag for timer to inform the task to fetch ADC data */
    RetVal = AmbaKAL_MsgQueueCreate(&SvcAdcButtonMsg, SvcAdcButtonMsgName, (UINT32)sizeof(SVC_ADC_BUTTON_MSG_s), SvcAdcButtonMsgQue, (UINT32)sizeof(SvcAdcButtonMsgQue));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_ADC_BUTTON, "AmbaKAL_EventFlagCreate() failed with 0x%x", RetVal, 0U);
    }

    /* Create task to fetch ADC data in fixed period (by timer) */
    RetVal = SvcTask_Create("SvcAdcButtonSampleTask", &SvcAdcButtonSampleTaskCtrl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_ADC_BUTTON, "SvcTask_Create() SampleTask failed with 0x%x", RetVal, 0U);
    }

    RetVal = SvcTask_Create("SvcAdcButtonExecTask", &SvcAdcButtonExecTaskCtrl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_ADC_BUTTON, "SvcTask_Create() ExecTask failed with 0x%x", RetVal, 0U);
    }

    return SVC_OK;
}

/**
* configure debug level of adc button module
* @param [in] Level debug level
* @return none
*/
void SvcAdcButton_SetDebugLv(UINT32 Level)
{
    SvcAdcButtonDbgLv = Level;
    SvcLog_DBG(SVC_LOG_ADC_BUTTON, "Set debug level = %d", SvcAdcButtonDbgLv, 0U);
}
