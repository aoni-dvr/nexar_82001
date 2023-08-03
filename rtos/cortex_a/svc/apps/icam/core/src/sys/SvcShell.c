/**
*  @file SvcShell.c
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
*  @details svc shell functions
*
*/

#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaShell_Commands.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaGPIO.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcCmd.h"
#include "SvcWrap.h"
#include "SvcShell.h"
#include "SvcTask.h"

#define SVC_LOG_SHELL               "SHELL"

#define SHELL_EXEC_CMD_MAX_LENTH (128U)
#define SHELL_EXEC_CMD_QUEUE_LENTH (32U)
#define SHELL_EXEC_TASK_STACK_SIZE (512U*1024U)
typedef struct {
    UINT32 idx;
} SVC_EXEC_CMD_MSG_s;

static AMBA_KAL_MSG_QUEUE_t ExecCmdQueueID;
static char ExecCmdBuf[SHELL_EXEC_CMD_QUEUE_LENTH][SHELL_EXEC_CMD_MAX_LENTH] GNU_SECTION_NOZEROINIT;

static void SHELL_GpioCmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: gpio [show|<pin_id>] [option]\n");
    PrintFunc("       gpio show [all|<pin>] - show the GPIO pin status\n");
    PrintFunc("       gpio <pin> in - set input mode\n");
    PrintFunc("       gpio <pin> out [0|1] - output mode with logic low/high\n");
    PrintFunc("       gpio <pin> alt <func> - alternate pin function\n");
}

static void ShowGpioInfo(UINT32 N, AMBA_SHELL_PRINT_f PrintFunc)
{
    char TmpStr[8];
    AMBA_GPIO_INFO_s PinInfo = {0};

    (void)AmbaGPIO_GetInfo(N, &PinInfo);
    (void)AmbaUtility_UInt32ToStr(TmpStr, (UINT32)sizeof(TmpStr), N, 10);
    PrintFunc("[");
    PrintFunc(TmpStr);
    PrintFunc("]: \t");
    if (PinInfo.PinFunc == AMBA_GPIO_FUNC_GPI) {
        PrintFunc("IN   : ");
    } else if (PinInfo.PinFunc == AMBA_GPIO_FUNC_GPO) {
        PrintFunc("OUT  : ");
    } else {
        PrintFunc("ALT ");
        (void)AmbaUtility_UInt32ToStr(TmpStr, (UINT32)sizeof(TmpStr), PinInfo.PinFunc - 1U, 10);
        PrintFunc(TmpStr);
        PrintFunc(": ");
    }
    if (PinInfo.PinState == AMBA_GPIO_LEVEL_HIGH) {
        PrintFunc("HIGH\n");
    } else {
        PrintFunc("LOW\n");
    }
}

/**
* script execution of amba_shell
* @param [in] pFilename filename of script
* @return 0-OK, 1-NG
*/
UINT32 SvcShell_ExecScript(const char *pFilename)
{
    UINT32 Rval, Idx = 0U;
    AMBA_FS_FILE *pFile = NULL;
    char TempCmdBuf[AMBA_SHELL_MAX_CMD_LINE_SIZE];
    char OneChar;
    UINT32 NumSuccess;
    UINT32 EndOfFile = 0U, ExitLoop = 0U;

    Rval = AmbaFS_FileOpen(pFilename, "r", &pFile);
    if ((Rval == AMBA_FS_ERR_NONE) && (NULL != pFile)) {
        SvcLog_OK(SVC_LOG_SHELL, pFilename, 0U, 0U);

        if (0U != AmbaWrap_memset(TempCmdBuf, 0, AMBA_SHELL_MAX_CMD_LINE_SIZE)) {
            SvcLog_NG(SVC_LOG_SHELL, "memset CmdBuf failed", 0U, 0U);
        }

        Rval = SVC_OK;
        while (SVC_OK == Rval) {
            UINT32 ShellExec = 0U;

            if (Idx >= AMBA_SHELL_MAX_CMD_LINE_SIZE) {
                Rval = SVC_NG;
                SvcLog_NG(SVC_LOG_SHELL, "Command length over %d", AMBA_SHELL_MAX_CMD_LINE_SIZE, 0U);
                ExitLoop = 1U;
            } else {

                /* Check EOF */
                EndOfFile = AmbaFS_FileEof(pFile);
                if ((EndOfFile != 0U) && (EndOfFile != AMBA_FS_ERR_API)) {
                    TempCmdBuf[Idx] = '\0';
                    Idx = Idx + 1U;
                    ShellExec = 1U;
                    ExitLoop = 1U;
                } else {
                    Rval = AmbaFS_FileRead(&OneChar, 1U, 1U, pFile, &NumSuccess);
                    if (Rval == AMBA_FS_ERR_NONE) {
                        if (NumSuccess > 0U) {
                            /* CERT STR34-C, replace switch case to if */
                            if ((OneChar == '\0') || (OneChar == '\r') || (OneChar == '\n')) {
                                TempCmdBuf[Idx] = '\0';
                                Idx = Idx + 1U;
                                ShellExec = 1U;
                            } else {
                                TempCmdBuf[Idx] = OneChar;
                                Idx = Idx + 1U;
                            }
                        } else {
                            /* EOF */
                        }
                    } else {
                        Rval = SVC_NG;
                        SvcLog_NG(SVC_LOG_SHELL, "AmbaFS_FileRead failed", 0U, 0U);
                    }
                }

                if (ShellExec == 1U) {
                    SvcLog_OK(SVC_LOG_SHELL, TempCmdBuf, 0U, 0U);
                    (void)AmbaShell_ExecCommand(TempCmdBuf);
                    if (0U != AmbaWrap_memset(TempCmdBuf, 0, AMBA_SHELL_MAX_CMD_LINE_SIZE)) {
                        SvcLog_NG(SVC_LOG_SHELL, "memset CmdBuf failed", 0U, 0U);
                    }
                    Idx = 0U;
                }
            }

            if (1U == ExitLoop) {
                break;
            }
        }
        (void)AmbaFS_FileClose(pFile);
    } else {
        Rval = SVC_NG;
    }

    return Rval;
}

static void* ExecCmdTaskEntry(void* EntryArg)
{
    UINT32 Rval = SVC_OK;
    SVC_EXEC_CMD_MSG_s Req;
    extern UINT32 AmbaShell_ExecCommand(char *pCmdLine);

    while (SVC_OK == Rval) {
        Rval = AmbaKAL_MsgQueueReceive(&ExecCmdQueueID, &Req, AMBA_KAL_WAIT_FOREVER);
        if (KAL_ERR_NONE == Rval) {
            if (SVC_OK != AmbaShell_ExecCommand(ExecCmdBuf[Req.idx])) {
                /* Do nothing */
            }
        } else {
            Rval = SVC_NG;
        }
    }

    AmbaMisra_TouchUnused(EntryArg);

    return NULL;
}

/**
* Initialize exec command task
* @return none
*/
void SvcShell_ExecTaskInit(void)
{
    UINT32 Rval;
    static char CmdQueueName[] = "ExecCmdQueue";
    static SVC_EXEC_CMD_MSG_s ExecCmdMsgQueue[SHELL_EXEC_CMD_QUEUE_LENTH] = {0};
    static SVC_TASK_CTRL_s ExecCmdTask;
    static UINT8 ExecCmdTaskStack[SHELL_EXEC_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    SvcLog_DBG(SVC_LOG_SHELL, "SvcShell_ExecInit start",0,0);

    Rval = AmbaKAL_MsgQueueCreate(&ExecCmdQueueID,
                                  CmdQueueName,
                                  (UINT32) sizeof(SVC_EXEC_CMD_MSG_s),
                                  &ExecCmdMsgQueue,
                                  SHELL_EXEC_CMD_QUEUE_LENTH * (UINT32)sizeof(SVC_EXEC_CMD_MSG_s));
    if (KAL_ERR_NONE != Rval) {
        SvcLog_NG(SVC_LOG_SHELL, "Create ExecCmdMsgQueue failed(%u)", Rval, 0);
    } else {
        ExecCmdTask.Priority   = 125;
        ExecCmdTask.EntryFunc  = ExecCmdTaskEntry;
        ExecCmdTask.EntryArg   = 0;
        ExecCmdTask.pStackBase = &ExecCmdTaskStack;
        ExecCmdTask.StackSize  = SHELL_EXEC_TASK_STACK_SIZE;
        ExecCmdTask.CpuBits    = 0x01U;
        Rval = SvcTask_Create("ExecCmdTask", &ExecCmdTask);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_SHELL, "Create ExecCmdTask failed(%u)", Rval, 0);
        }
    }
}

/**
* Add command to command queue
* @param [in] pCmd command
* @return none
*/
void SvcShell_ExecAddCmd(const char *pCmd)
{
    UINT32 Rval;
    SVC_EXEC_CMD_MSG_s Msg;
    char *pCmdBuf;
    static UINT32 ExecCmdIdx = 0;

    AmbaPrint_PrintStr5("[Enqueue] %s", pCmd, NULL, NULL, NULL, NULL);

    pCmdBuf = &ExecCmdBuf[ExecCmdIdx][0];

    SvcWrap_strcpy(pCmdBuf, SHELL_EXEC_CMD_MAX_LENTH, pCmd);
    Msg.idx = ExecCmdIdx;

    ExecCmdIdx++;
    if (ExecCmdIdx >= SHELL_EXEC_CMD_QUEUE_LENTH) {
        ExecCmdIdx = 0;
    }

    Rval = AmbaKAL_MsgQueueSend(&ExecCmdQueueID, &Msg, AMBA_KAL_NO_WAIT);
    if (KAL_ERR_NONE != Rval) {
        SvcLog_NG(SVC_LOG_SHELL, "SvcShell_ExecAddCmd: AmbaKAL_MsgQueueSend() failed(%u)", Rval, 0);
    }
}

/**
* command to dump dsp log
* @param [in] ArgCount arguments count
* @param [in] pArgVector arguments array
* @param [in] PrintFunc printf function
* @return none
*/
void SvcCmd_DisplayMsg(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#ifdef CONFIG_ICAM_UCODE_PARTIAL_LOAD
    static UINT8 uCodeStrTblLoadDone = 0U;
    if ((ArgCount >= 2U) && (uCodeStrTblLoadDone == 0U)) {
        if (0 == SvcWrap_strcmp(pArgVector[1U], "dsp")) {
            extern void SvcUcode_LoadStrTbl(void);
            SvcUcode_LoadStrTbl();
            AmbaPrint_Flush();
            uCodeStrTblLoadDone = 1;
        }
    }
#endif

    AmbaShell_CommandDisplayMsg(ArgCount, pArgVector, PrintFunc);
}

/**
* command to dump gpio
* @param [in] ArgCount arguments count
* @param [in] pArgVector arguments array
* @param [in] PrintFunc printf function
* @return none
*/
void SvcCmd_Gpio(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i = 0U;

    if (ArgCount > 1U) {
        if (AmbaUtility_StringCompare(pArgVector[1], "show", 4U) == 0) {
            if (ArgCount > 2U) {
                if (AmbaUtility_StringCompare(pArgVector[2], "all", 3U) == 0) {
                    for (i = 0U; i < AMBA_NUM_GPIO_PIN; i++) {
                        ShowGpioInfo(i, PrintFunc);
                    }
                } else {
                    (void)AmbaUtility_StringToUInt32(pArgVector[2], &i);
                    if (i < AMBA_NUM_GPIO_PIN) {
                        ShowGpioInfo(i, PrintFunc);
                    } else {
                        SHELL_GpioCmdUsage(PrintFunc);
                    }
                }
            } else {
                SHELL_GpioCmdUsage(PrintFunc);
            }
        } else if (ArgCount > 2U) {
            UINT32 GpioId = 0U;

            (void)AmbaUtility_StringToUInt32(pArgVector[1], &GpioId);
            if (AmbaUtility_StringCompare(pArgVector[2], "in", 2U) == 0) {
                (void)AmbaGPIO_SetFuncGPI(GpioId);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "out", 3U) == 0) {
                if (ArgCount > 3U) {
                    UINT32 Val = 0U;

                    (void)AmbaUtility_StringToUInt32(pArgVector[3], &Val);
                    if (Val > 0U) {
                        (void)AmbaGPIO_SetFuncGPO(GpioId, AMBA_GPIO_LEVEL_HIGH);
                    } else {
                        (void)AmbaGPIO_SetFuncGPO(GpioId, AMBA_GPIO_LEVEL_LOW);
                    }
                } else {
                    SHELL_GpioCmdUsage(PrintFunc);
                }
            } else if (AmbaUtility_StringCompare(pArgVector[2], "alt", 3U) == 0) {
                if (ArgCount > 3U) {
                    UINT32 Val = 0U;

                    (void)AmbaUtility_StringToUInt32(pArgVector[3], &Val);
                    (void)AmbaGPIO_SetFuncAlt(GPIO_SET_PIN_FUNC(Val, GpioId));
                } else {
                    SHELL_GpioCmdUsage(PrintFunc);
                }
            } else {
                PrintFunc("Not supported now!\n");
            }
        } else {
            SHELL_GpioCmdUsage(PrintFunc);
        }
    } else {
        SHELL_GpioCmdUsage(PrintFunc);
    }
}
