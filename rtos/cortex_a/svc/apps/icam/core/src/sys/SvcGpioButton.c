/**
*  @file SvcGpioButton.c
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
*  @details svc boot device
*
*/

#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcGpioButton.h"
#include "SvcTask.h"

typedef struct {
    UINT8    NumButton;
    UINT32   (*Action)(const char *pCmd);
    UINT8    CmdLength;
    UINT16   SamplePeriod;
    UINT8    DeBounceTarget;
    UINT8    RepeatStart;
    UINT8    RepeatInterval;
    UINT8    Debug;
} G_BTN_CFG_s;

static G_BTN_CFG_s GBtnCfg;

typedef struct {
    UINT32   PreState;
    UINT8    Repeating;
    UINT8    DeBounceCnt;
    UINT8    PinID;
    char     Cmd[MAX_CMD_LENGTH];
} G_BTN_INFO_s;

static G_BTN_INFO_s GBtnInfo[MAX_NUM_GPIO_BUTTON] GNU_SECTION_NOZEROINIT;

static void* GpioBtn_TaskEntry(void* EntryArg)
{
    UINT8 LoopU = 1U;
    UINT8 i;
    AMBA_GPIO_INFO_s Info;
    UINT32 Ret;

    AmbaMisra_TouchUnused(EntryArg);

    while (LoopU == 1U) {
        for (i = 0; i < GBtnCfg.NumButton; i ++) {
            Ret = AmbaGPIO_GetInfo(GBtnInfo[i].PinID, &Info);
            if (Ret != OK) {
                SvcLog_NG(SVC_LOG_G_BTN, "AmbaGPIO_GetInfo fail return %u", Ret, 0);
            } else {
                if (Info.PinState != GBtnInfo[i].PreState) {
                    if(Info.PinState == RELEASE_STAT) {
                        GBtnInfo[i].DeBounceCnt = 0;
                        GBtnInfo[i].PreState = Info.PinState;
                    } else {
                        GBtnInfo[i].DeBounceCnt ++;
                    }
                    if (GBtnCfg.Debug != 0U) {
                        SvcLog_OK(SVC_LOG_G_BTN, "Button %u, state %u", i, Info.PinState);
                        SvcLog_OK(SVC_LOG_G_BTN, "DeBounceCnt %u, pre-state %u", GBtnInfo[i].DeBounceCnt, GBtnInfo[i].PreState);
                    }
                    if (GBtnInfo[i].DeBounceCnt == GBtnCfg.DeBounceTarget) {
                        if (Info.PinState == PRESS_STAT) {
                            SvcLog_OK(SVC_LOG_G_BTN, "Button %u pressed", i, 0);
                            Ret = GBtnCfg.Action(GBtnInfo[i].Cmd);
                            if (Ret != OK) {
                                SvcLog_NG(SVC_LOG_G_BTN, "ActionCB fail return %u", Ret, 0);
                            }
                        } else {
                            SvcLog_OK(SVC_LOG_G_BTN, "Button %u released", i, 0);
                        }
                        GBtnInfo[i].PreState = Info.PinState;
                        GBtnInfo[i].DeBounceCnt = 0;
                    }
                    GBtnInfo[i].Repeating = 0;
                } else {
                    if ((Info.PinState == PRESS_STAT) && (GBtnCfg.RepeatStart != 0U)) {
                        UINT8 Target;
                        GBtnInfo[i].DeBounceCnt ++;
                        if (GBtnCfg.Debug != 0U) {
                            SvcLog_OK(SVC_LOG_G_BTN, "Button %u, state = pre-state %u", i, Info.PinState);
                            SvcLog_OK(SVC_LOG_G_BTN, "DeBounceCnt %u, repeat %u", GBtnInfo[i].DeBounceCnt, GBtnInfo[i].Repeating);
                        }
                        if (GBtnInfo[i].Repeating == 0U) {
                            Target = GBtnCfg.RepeatStart;
                        } else {
                            Target = GBtnCfg.RepeatInterval;
                        }
                        if (GBtnInfo[i].DeBounceCnt == Target) {
                            SvcLog_OK(SVC_LOG_G_BTN, "Button %u repeat", i, 0);
                            Ret = GBtnCfg.Action(GBtnInfo[i].Cmd);
                            if (Ret != OK) {
                                SvcLog_NG(SVC_LOG_G_BTN, "ActionCB fail return %u", Ret, 0);
                            }
                            GBtnInfo[i].DeBounceCnt = 0;
                            GBtnInfo[i].Repeating = 1;
                        }
                    } else {
                        GBtnInfo[i].DeBounceCnt = 0;
                    }
                }
            }
        }
        (void)AmbaKAL_TaskSleep(GBtnCfg.SamplePeriod);

        AmbaMisra_TouchUnused(&LoopU);
    }

    return NULL;
}

/**
* debug messages control of gpio button module
* @param [in] Debug debug control
* @return none
*/
void SvcGpioButton_Debug(UINT8 Debug)
{
    GBtnCfg.Debug = Debug;
}

/**
* initialization of gpio button module
* @param [in] pCfg configuration of gpio button module
* @return 0-OK, 1-NG
*/
UINT32 SvcGpioButton_Init(const GPIO_BUTTON_CONFIG_s *pCfg)
{
    UINT32                  Rval = SVC_OK;
    static UINT8            GpioBtnStack[SVC_G_BTN_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static SVC_TASK_CTRL_s  TaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8            Init = 0;
    UINT8 i;

    /* task create */
    TaskCtrl.Priority    = pCfg->Priority;
    TaskCtrl.EntryFunc   = GpioBtn_TaskEntry;
    TaskCtrl.EntryArg    = 0U;
    TaskCtrl.pStackBase  = GpioBtnStack;
    TaskCtrl.StackSize   = SVC_G_BTN_STACK_SIZE;
    TaskCtrl.CpuBits     = pCfg->CpuBits;

    if (MAX_NUM_GPIO_BUTTON < pCfg->NumButton) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_G_BTN, "NumButton(%u) > MAX_NUM_GPIO_BUTTON(%u)!", pCfg->NumButton, MAX_NUM_GPIO_BUTTON);
    }

    if (Rval == SVC_OK) {
        if (MAX_CMD_LENGTH < pCfg->CmdLength) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_G_BTN, "CmdLength(%u) > MAX_CMD_LENGTH(%u)!", pCfg->CmdLength, MAX_CMD_LENGTH);
        }
    }

    if (Rval == SVC_OK) {
        if (NULL == pCfg->Action) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_G_BTN, "Action = NULL!", 0, 0);
        }
    }

    if (Rval == SVC_OK) {
        if (NULL == pCfg->pPinID) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_G_BTN, "pPinID = NULL!", 0, 0);
        }
    }

    if (Rval == SVC_OK) {
        if (NULL == pCfg->pCmd) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_G_BTN, "pCmd = NULL!", 0, 0);
        }
    }

    if (Rval == SVC_OK) {
        if (0U == pCfg->SamplePeriod) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_G_BTN, "SamplePeriod %u", pCfg->SamplePeriod, 0);
        }
    }

    if (Rval == SVC_OK) {
        if ((0U == pCfg->RepeatInterval) && (0U != pCfg->RepeatStart)) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_G_BTN, "RepeatInterval %u, RepeatStart %u", pCfg->RepeatInterval, pCfg->RepeatStart);
        }
    }

    if (Rval == SVC_OK) {
        if ((0U == pCfg->CmdLength) || (0U == pCfg->DeBounceTarget)) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_G_BTN, "CmdLength %u, DeBounceTarget %u", pCfg->CmdLength, pCfg->DeBounceTarget);
        }
    }

    if (Rval == SVC_OK) {
        GBtnCfg.NumButton = pCfg->NumButton;
        GBtnCfg.Action = pCfg->Action;
        GBtnCfg.CmdLength = pCfg->CmdLength;
        GBtnCfg.SamplePeriod = pCfg->SamplePeriod;
        GBtnCfg.DeBounceTarget = pCfg->DeBounceTarget;
        GBtnCfg.RepeatStart = pCfg->RepeatStart;
        GBtnCfg.RepeatInterval = pCfg->RepeatInterval;
        GBtnCfg.Debug = pCfg->Debug;
        for (i = 0; i < GBtnCfg.NumButton; i ++) {
            GBtnInfo[i].PinID = pCfg->pPinID[i];
            SvcLog_OK(SVC_LOG_G_BTN, "Button %u PinID %u", i, GBtnInfo[i].PinID);
            Rval = AmbaGPIO_SetFuncGPI(GBtnInfo[i].PinID);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_G_BTN, "AmbaGPIO_SetFuncGPI %u failed! return %u", GBtnInfo[i].PinID, Rval);
            } else {
                AmbaUtility_StringCopy(GBtnInfo[i].Cmd, GBtnCfg.CmdLength, &pCfg->pCmd[i * GBtnCfg.CmdLength]);
                AmbaPrint_PrintStr5("Cmd %s", GBtnInfo[i].Cmd, NULL, NULL, NULL, NULL);
                GBtnInfo[i].PreState = RELEASE_STAT;
                GBtnInfo[i].DeBounceCnt = 0;
                GBtnInfo[i].Repeating = 0;
            }
        }
        if ((Rval == SVC_OK) && (Init == 0U)) {
            Rval = SvcTask_Create("GpioButton_Task", &TaskCtrl);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_G_BTN, "GpioButton_Task created failed! return %u", Rval, 0U);
                Rval = SVC_NG;
            } else {
                Init = 1;
            }
        }
    }

    return Rval;
}
