/**
 *  @file AmbaShell.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Ambarella Shell functions.
 *
 */

#include <AmbaKAL.h>
#include "AmbaShell.h"
#include "AmbaShell_KeyCode.h"
#include "AmbaShell_CmdHistory.h"
#include <AmbaUtility.h>
#include <AmbaWrap.h>
#include <AmbaMisraFix.h>

/* AMBA_SHELL_EXECUTE_FLAG definition */
#define AMBA_SHELL_EXECUTE_FLAG         0x01UL

/* AMBA_SHELL_DONE_FLAG definition */
#define AMBA_SHELL_DONE_FLAG            0x10UL

/* SHELL_WAIT_FOREVER definition */
#define SHELL_WAIT_FOREVER              (0xFFFFFFFFU)

/*
 * SHELL: AMBA_SHELL_CTRL_s, SHELL control structure
 */
typedef struct {
    AMBA_KAL_TASK_t         CliTask;
    AMBA_KAL_TASK_t         ExecTask;
    AMBA_KAL_EVENT_FLAG_t   EventFlag;

    AMBA_KAL_MUTEX_t        ExecMutex;
    AMBA_KAL_MUTEX_t        CmdListMutex;
    AMBA_KAL_MUTEX_t        PrintMutex;

    AMBA_SHELL_PUT_CHAR_f   ConsolePutCharCb;
    AMBA_SHELL_GET_CHAR_f   ConsoleGetCharCb;

    char CurrWorkDir[AMBA_SHELL_MAX_DIR_LENGTH]; /* Current working directory */

    UINT32                   LastExecCmdResult;
    UINT32                   StopExecCmdFlag;
} AMBA_SHELL_CTRL_s;

static char _AmbaShellCmdLineBuf[AMBA_SHELL_MAX_CMD_LINE_SIZE] __attribute__((section(".bss.noinit")));
static char _AmbaShellKeyBuf[AMBA_SHELL_MAX_CMD_LINE_SIZE] __attribute__((section(".bss.noinit")));
static AMBA_SHELL_CTRL_s _AmbaShellCtrl __attribute__((section(".bss.noinit")));
static AMBA_SHELL_COMMAND_s *ShellCmdList = NULL;

static void Shell_ConsoleWrite(const char *pString, UINT32 StringSize)
{
    UINT32 NumWrite = 0;
    UINT32 Index = 0;
    UINT32 NumWritten;
    static char NewLinePrintBuf[2048] __attribute__((section(".bss.noinit")));

    if (_AmbaShellCtrl.ConsolePutCharCb == NULL) {
        // do nothing
    } else {
        while (Index < StringSize) {
            if (pString[Index] == '\n') {
                NewLinePrintBuf[NumWrite] = '\r';
                NumWrite++;
            }
            NewLinePrintBuf[NumWrite] = pString[Index];
            Index++;
            NumWrite++;
        }
        NumWritten = _AmbaShellCtrl.ConsolePutCharCb(NumWrite, &NewLinePrintBuf[0], SHELL_WAIT_FOREVER);
        if (NumWritten != NumWrite) {
            // ignore this error
        }
    }
    return;
}

static UINT32 Shell_ConsoleRead(char *pString, UINT32 StringSize)
{
    UINT32 NumRead = 0;
    UINT32 RetVal;

    if (_AmbaShellCtrl.ConsoleGetCharCb == NULL) {
        RetVal = SHELL_ERR_FAIL;
    } else {
        while (NumRead < StringSize) {
            RetVal = _AmbaShellCtrl.ConsoleGetCharCb(1, &pString[NumRead], SHELL_WAIT_FOREVER);
            if (RetVal > 0U) {
                NumRead += RetVal;
            }
        }
        RetVal = SHELL_ERR_SUCCESS;
    }

    return RetVal;
}

static void Shell_Print(const char *pString)
{
    UINT32 Len;

    // VA_ARG and vsprintf can't be used if MISRA_C check is enabled
    /*-----------------------------------------------------------------------*\
     * Take the Mutex
    \*-----------------------------------------------------------------------*/
    if (AmbaKAL_MutexTake(&_AmbaShellCtrl.PrintMutex, SHELL_WAIT_FOREVER) != KAL_ERR_NONE) {
        /* should never happen */
    } else {
        if (pString != NULL) {
            Len = AmbaUtility_StringLength(pString);
            Shell_ConsoleWrite(pString, Len);
        } else {
            Shell_ConsoleWrite("NULL", 4);
        }

        /*-----------------------------------------------------------------------*\
         * Release the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexGive(&_AmbaShellCtrl.PrintMutex) != KAL_ERR_NONE) {
            /* should never happen */
        }
    }

    return;
}

static void Shell_RemoveChar(UINT32 *pLocate)
{
    UINT32 i;
    char *pString = &_AmbaShellKeyBuf[0];
    UINT32 Len = AmbaUtility_StringLength(pString);
    UINT32 Loc = *pLocate;

    if (Len != 0U) {
        if (Loc == Len) {
            pString[Len - 1U] = '\0';
            *pLocate -= 1U;
        } else {
            for (i = (Loc - 1U); i <= Len; i++) {
                pString[i] = pString[i + 1U];
            }
            *pLocate -= 1U;
        }
    }
    return;
}

static UINT32 Shell_InsertChar(char Value, UINT32 *pLocate)
{
    UINT32 i, nRet = SHELL_ERR_SUCCESS;
    char *pString = &_AmbaShellKeyBuf[0];
    UINT32 Len = AmbaUtility_StringLength(pString);
    UINT32 Loc = *pLocate;

    if (Loc > Len) {
        nRet = SHELL_ERR_FAIL;
    } else {
        pString[Len + 1U] = '\0';

        for (i = Len; i > Loc; i--) {
            pString[i] = pString[i - 1U];
        }

        pString[Loc] = Value;
        *pLocate += 1U;
    }

    return nRet;
}

static void Shell_RestoreCmdLine(UINT32 Backward, UINT32 *pNumPrint)
{
    const char *pRplstr;
    char *pString = _AmbaShellKeyBuf;

    pRplstr = AmbaShell_RestoreCmdHistory(Backward);
    if (pRplstr == NULL) {
        // do nothing
    } else {
        while (*pNumPrint > 0U) {
            *pNumPrint = *pNumPrint - 1U;
            Shell_ConsoleWrite("\x1b" "[D" "\x1b" "[P", 6);
        }

        while ((*pRplstr != '\n') && (*pRplstr != '\r') && (*pRplstr != '\0')) {
            Shell_ConsoleWrite(pRplstr, 1);
            *pString = *pRplstr;
            *pNumPrint   = *pNumPrint + 1U;
            pString++;
            pRplstr++;
        }
    }
}

static UINT32 Shell_KeyCodeArrow(AMBA_SHELL_KEY_CODE_e KeyCode, UINT32 *pNumPrint, UINT32 *pCursorPos)
{
    UINT32 ret_handled;
    char EchoString[10];
    char IntString[UTIL_MAX_INT_STR_LEN] = {(char)0};

    EchoString[0] = '\0';

    if (KeyCode == AMBA_SHELL_KEY_UP_ARROW) {
        /* move cursor position to line end */
        if ((*pCursorPos) < (*pNumPrint)) {
            (void)AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, ((*pNumPrint) - (*pCursorPos)), 10);
            AmbaUtility_StringAppend(EchoString, 10, "\x1b""[");
            AmbaUtility_StringAppend(EchoString, 10, IntString);
            AmbaUtility_StringAppend(EchoString, 10, "C");

            Shell_ConsoleWrite(EchoString, AmbaUtility_StringLength(EchoString));
        }

        /* Replace to history command */
        Shell_RestoreCmdLine(1, pNumPrint);
        (*pCursorPos) = (*pNumPrint);
        ret_handled = 1;
    } else if (KeyCode == AMBA_SHELL_KEY_DOWN_ARROW) {
        /* move cursor position to line end */
        if ((*pCursorPos) < (*pNumPrint)) {
            (void)AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, ((*pNumPrint) - (*pCursorPos)), 10);
            AmbaUtility_StringAppend(EchoString, 10, "\x1b""[");
            AmbaUtility_StringAppend(EchoString, 10, IntString);
            AmbaUtility_StringAppend(EchoString, 10, "C");

            Shell_ConsoleWrite(EchoString, AmbaUtility_StringLength(EchoString));
        }

        /* Replace to history command */
        Shell_RestoreCmdLine(0, pNumPrint);
        (*pCursorPos) = (*pNumPrint);
        ret_handled = 1;
    } else if (KeyCode == AMBA_SHELL_KEY_RIGHT_ARROW) {
        if ((*pCursorPos) < (*pNumPrint)) {
            Shell_ConsoleWrite("\x1b""[C", 3);
            (*pCursorPos)++;
        }
        ret_handled = 1;
    } else if (KeyCode == AMBA_SHELL_KEY_LEFT_ARROW) {
        if ((*pCursorPos) > 0U) {
            Shell_ConsoleWrite("\x1b""[D", 3);
            (*pCursorPos)--;
        }
        ret_handled = 1;
    } else {
        ret_handled = 0;
    }
    return ret_handled;
}

static UINT32 Shell_KeyCodeArithmeticOperator(AMBA_SHELL_KEY_CODE_e KeyCode, UINT32 *pNumPrint, UINT32 *pCursorPos)
{
    UINT32 ret_handled;
    char EchoString[10];

    EchoString[0] = '\0';

    if (KeyCode == AMBA_SHELL_KEY_DIVIDE) {
        EchoString[0] = '/';
        if (Shell_InsertChar(EchoString[0], pCursorPos) == SHELL_ERR_SUCCESS) {
            (*pNumPrint)++;
            Shell_ConsoleWrite(EchoString, 1);
        }
        ret_handled = 1;

    } else if (KeyCode == AMBA_SHELL_KEY_MULTIPLY) {
        EchoString[0] = '*';
        if (Shell_InsertChar(EchoString[0], pCursorPos) == SHELL_ERR_SUCCESS) {
            (*pNumPrint)++;
            Shell_ConsoleWrite(EchoString, 1);
        }
        ret_handled = 1;
    } else if (KeyCode == AMBA_SHELL_KEY_SUBTRACT) {
        EchoString[0] = '-';
        if (Shell_InsertChar(EchoString[0], pCursorPos) == SHELL_ERR_SUCCESS) {
            (*pNumPrint)++;
            Shell_ConsoleWrite(EchoString, 1);
        }
        ret_handled = 1;
    } else if (KeyCode == AMBA_SHELL_KEY_ADD) {
        EchoString[0] = '+';
        if (Shell_InsertChar(EchoString[0], pCursorPos) == SHELL_ERR_SUCCESS) {
            (*pNumPrint)++;
            Shell_ConsoleWrite(EchoString, 1);
        }
        ret_handled = 1;
    } else {
        ret_handled = 0;
    }
    return ret_handled;
}

static void Shell_KeyCodeHandler(AMBA_SHELL_KEY_CODE_e KeyCode, UINT32 *pNumPrint, UINT32 *pCursorPos)
{
    char EchoString[10];
    char IntString[UTIL_MAX_INT_STR_LEN] = {(char)0};

    EchoString[0] = '\0';

    if (KeyCode == AMBA_SHELL_KEY_BACKSPACE) {      /* Backspace */
        if ((*pNumPrint > 0U) && (*pCursorPos > 0U)) {
            Shell_ConsoleWrite("\x1b""[D""\x1b""[P", 6);
            Shell_RemoveChar(pCursorPos);
            (*pNumPrint)--;
        }

    } else if (KeyCode == AMBA_SHELL_KEY_CTRL_C) {  /* Ctrl + C */
        Shell_ConsoleWrite("^C\n", 4);
        (*pNumPrint) = 0;
        _AmbaShellKeyBuf[0] = '\0';
        _AmbaShellCtrl.StopExecCmdFlag = 1;

    } else if (KeyCode == AMBA_SHELL_KEY_INSERT) {
        Shell_ConsoleWrite("\x1b""[4h", 4);

    } else if (KeyCode == AMBA_SHELL_KEY_DELETE) {
        if ((*pCursorPos) < (*pNumPrint)) {
            Shell_ConsoleWrite("\x1b""[P", 3);
            (*pNumPrint)--;
        }

    } else if (KeyCode == AMBA_SHELL_KEY_HOME) {
        if ((*pCursorPos) > 0U) {
            (void)AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, *pCursorPos, 10);
            AmbaUtility_StringAppend(EchoString, 10, "\x1b""[");
            AmbaUtility_StringAppend(EchoString, 10, IntString);
            AmbaUtility_StringAppend(EchoString, 10, "D");
            Shell_ConsoleWrite(EchoString, AmbaUtility_StringLength(EchoString));
            (*pCursorPos) = 0;
        }

    } else if (KeyCode == AMBA_SHELL_KEY_END) {
        if ((*pCursorPos) < (*pNumPrint)) {
            (void)AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, ((*pNumPrint) - (*pCursorPos)), 10);
            AmbaUtility_StringAppend(EchoString, 10, "\x1b""[");
            AmbaUtility_StringAppend(EchoString, 10, IntString);
            AmbaUtility_StringAppend(EchoString, 10, "C");
            Shell_ConsoleWrite(EchoString, AmbaUtility_StringLength(EchoString));
            (*pCursorPos) = (*pNumPrint);
        }

    } else if (Shell_KeyCodeArrow(KeyCode, pNumPrint, pCursorPos) == 1U) {

        // arrow keys are handled

    } else if (Shell_KeyCodeArithmeticOperator(KeyCode, pNumPrint, pCursorPos) == 1U) {

        // arithmetic operator keys are handled

    } else {
        /* do nothing */
    }
}

static void Shell_GetOneCmdLine(void)
{
    AMBA_SHELL_KEY_CODE_e KeyCode;
    char InputValue = (char)0;
    UINT32 CursorPos = 0;          /* Cursor position */
    UINT32 NumPrint = 0;           /* Number of printed characters */

    AmbaShell_ResetKeyCode();

    /* Get the first character */
    while (Shell_ConsoleRead(&InputValue, 1) != SHELL_ERR_SUCCESS) {
        continue;
    }

    /* Handle string until newline */
    while ((InputValue != '\n') && (InputValue != '\r')) {
        if (AmbaShell_GetKeyCode(InputValue, &KeyCode) == SHELL_ERR_SUCCESS) {
            if (KeyCode < AMBA_SHELL_NUM_KEY) {
                Shell_KeyCodeHandler(KeyCode, &NumPrint, &CursorPos);
            } else {
                if (Shell_InsertChar(InputValue, &CursorPos) == SHELL_ERR_SUCCESS) {
                    NumPrint++;
                    Shell_ConsoleWrite(&InputValue, 1);
                }
            }
        }

        /* Get the following character */
        (void)Shell_ConsoleRead(&InputValue, 1);

    } /* end of while */

    _AmbaShellKeyBuf[NumPrint] = '\0';

    /* save command to history buffer */
    AmbaShell_SetCmdHistory(_AmbaShellKeyBuf);

}

static void *Shell_CliTaskEntry(void *EntryArg)
{
    UINT32 ActualFlag;
    UINT32 nRet;
    UINT32 FlagLoop = 1;

    static const char *pAmbaShellPromptMsg =
        "*****************************************************************\n"
        "*                                                               *\n"
        "*                        AmbaShell ;)                           *\n"
        "*                                                               *\n"
        "*****************************************************************\n"
        "\r\n";

    /* pass misra check */
    AmbaMisra_TouchUnused(EntryArg);


    /* Print welcome message */
    Shell_Print(pAmbaShellPromptMsg);
    Shell_Print("Type 'help' for help\n\n");

    /* Print current directory */
    Shell_Print(_AmbaShellCtrl.CurrWorkDir);
    Shell_Print("> ");

    /* Clear key buffer */
    AmbaUtility_MemorySetChar(_AmbaShellKeyBuf, '\0', sizeof(_AmbaShellKeyBuf));

    /* Endless loop for receiving command */
    while (FlagLoop == 1U) {
        Shell_GetOneCmdLine();

        /* Should wait last command done */
        nRet = AmbaKAL_EventFlagGet(&_AmbaShellCtrl.EventFlag, AMBA_SHELL_DONE_FLAG, 1, 1, &ActualFlag, SHELL_WAIT_FOREVER);
        if (nRet != KAL_ERR_NONE) {
            Shell_Print("[Shell][CLI] failed to get event flag, exit.\r\n");
            FlagLoop = 0;
        }

        Shell_ConsoleWrite("\n", 2);
        AmbaUtility_StringCopy(_AmbaShellCmdLineBuf, AMBA_SHELL_MAX_CMD_LINE_SIZE, _AmbaShellKeyBuf);
        AmbaUtility_MemorySetChar(_AmbaShellKeyBuf, '\0', sizeof(_AmbaShellKeyBuf));

        nRet = AmbaKAL_EventFlagSet(&_AmbaShellCtrl.EventFlag, AMBA_SHELL_EXECUTE_FLAG);
        if (nRet != KAL_ERR_NONE) {
            Shell_Print("[Shell][CLI] failed to set event flag, exit.\r\n");
            FlagLoop = 0;
        }
    }
    return NULL;
}

static AMBA_SHELL_PROC_f Shell_FindCmdProc(const char *pName)
{
    const AMBA_SHELL_COMMAND_s *pCmd;
    AMBA_SHELL_PROC_f ret = NULL;

    for (pCmd = ShellCmdList; pCmd != NULL; pCmd = pCmd->pNext) {
        UINT32 len1 = AmbaUtility_StringLength(pName);
        UINT32 len2 = AmbaUtility_StringLength(pCmd->pName);
        if ((AmbaUtility_StringCompare(pName, pCmd->pName, AmbaUtility_StringLength(pName)) == 0) && (len1 == len2)) {
            ret = pCmd->MainFunc;
            break;
        }
    }

    return ret;
}

static UINT32 Shell_ExecSingleCmd(const char *pCmdBuf)
{
    AMBA_SHELL_PROC_f CmdProc;
    char *pArgVector[AMBA_SHELL_MAX_ARG_VECTOR];
    char Args[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH];
    UINT32 LastRetStatus = SHELL_ERR_CMD_FAIL;
    UINT32 ArgCount, uRet, i;

    if (pCmdBuf == NULL) {
        LastRetStatus = SHELL_ERR_CMD_PARSE;
    } else {
        if (AmbaWrap_memset(pArgVector, 0x0, sizeof(pArgVector)) != 0U) {
            // no action
        } else {
            /* Apart cmd string by space */
            uRet = AmbaUtility_StringToken(pCmdBuf, ' ', Args, &ArgCount);
            if ((uRet != 0U) || (pCmdBuf[0] == '#')) {
                LastRetStatus = SHELL_ERR_SUCCESS;
            } else {
                /* Normal command */
                CmdProc = Shell_FindCmdProc(Args[0]);

                if (CmdProc != NULL) {
                    /* Initialize argument vectors */
                    for (i = 0; i < ArgCount; i++) {
                        pArgVector[i] = &Args[i][0];
                    }

                    CmdProc(ArgCount, pArgVector, Shell_Print);
                    _AmbaShellCtrl.LastExecCmdResult = LastRetStatus;
                } else {
                    Shell_Print(Args[0]);
                    Shell_Print(": command not found\n");
                }
            }
        }
    }
    return LastRetStatus;
}

static UINT32 Shell_ExecCmds(char *pCmdBuf)
{
    UINT32 i, CmdStartIdx, StringLength;
    UINT32 LastRetStatus = SHELL_ERR_CMD_EXEC;

    if (pCmdBuf == NULL) {
        LastRetStatus = SHELL_ERR_INVALID_PARAMETER;
    } else {
        StringLength = AmbaUtility_StringLength(pCmdBuf);
        CmdStartIdx = 0;
        for (i = 0; i <= StringLength; i++) {
            if ((pCmdBuf[i] == ';') || (i == StringLength)) {
                pCmdBuf[i] = '\0';
                LastRetStatus = Shell_ExecSingleCmd(&pCmdBuf[CmdStartIdx]);
                CmdStartIdx = i + 1U;
            }
        }
    }
    return LastRetStatus;
}


static UINT32 Shell_ParseAndDoExecCmd(char *pCmdBuf)
{
    UINT32 RetStatus;

    if (pCmdBuf == NULL) {
        RetStatus = SHELL_ERR_INVALID_PARAMETER;
    } else if (pCmdBuf[0] == '\0') {
        RetStatus = SHELL_ERR_INVALID_PARAMETER;
    } else {
        // Explicitly reset StopExecCmdFlag
        _AmbaShellCtrl.StopExecCmdFlag = 0;

        /* Get the compound command information first */
        /* Only Normal command is supported */
        RetStatus = Shell_ExecCmds(pCmdBuf);
    }
    return RetStatus;
}

static void *Shell_ExecTaskEntry(void  *EntryArg)
{
    UINT32 ActualFlag;
    UINT32 nRet;
    UINT32 FlagLoop = 1;

    /* pass misra check */
    AmbaMisra_TouchUnused(EntryArg);

    /* Signal the execution done flag firstly */
    nRet = AmbaKAL_EventFlagSet(&(_AmbaShellCtrl.EventFlag), AMBA_SHELL_DONE_FLAG);
    if (nRet != KAL_ERR_NONE) {
        /* need to handle this error? */
    }

    while (FlagLoop == 1U) {
        /* Wait event flag from cli shell task */
        nRet = AmbaKAL_EventFlagGet(&_AmbaShellCtrl.EventFlag, AMBA_SHELL_EXECUTE_FLAG, 1, 1, &ActualFlag, SHELL_WAIT_FOREVER);
        if (nRet != KAL_ERR_NONE) {
            Shell_Print("[Shell][EXEC] failed to get event flag, exit.\r\n");
            FlagLoop = 0;
        }

        /* Parsing and execute script/cmds */
        nRet = Shell_ParseAndDoExecCmd(_AmbaShellCmdLineBuf);

        if (nRet != SHELL_ERR_SUCCESS) {
            /* need to handle this error? */
        }

        /* Print current directory */
        Shell_Print(_AmbaShellCtrl.CurrWorkDir);
        Shell_Print("> ");
        Shell_Print(_AmbaShellKeyBuf);

        /* Send execution done flag */
        nRet = AmbaKAL_EventFlagSet(&_AmbaShellCtrl.EventFlag, AMBA_SHELL_DONE_FLAG);
        if (nRet != KAL_ERR_NONE) {
            Shell_Print("[Shell][EXEC] failed to set event flag, exit.\r\n");
            FlagLoop = 0;
        }
    }
    return NULL;
}

static UINT32 Shell_CreateCliTask(UINT32 SmpCoreSet, UINT32 TaskPriority, void *pTaskStack, UINT32 TaskStackSize)
{
    static char shell_cli_task_name[] = "AmbaShell_CliTask";

    UINT32 RetVal = AmbaKAL_TaskCreate(&_AmbaShellCtrl.CliTask,
                                       shell_cli_task_name,
                                       TaskPriority,
                                       Shell_CliTaskEntry,
                                       NULL,
                                       pTaskStack,
                                       TaskStackSize,
                                       0);

    if (RetVal == KAL_ERR_NONE) {
        RetVal = AmbaKAL_TaskSetSmpAffinity(&_AmbaShellCtrl.CliTask, SmpCoreSet);
        if (RetVal != KAL_ERR_NONE) {
            /* need to handle this error? */
        }
    }

    return AmbaKAL_TaskResume(&_AmbaShellCtrl.CliTask);
}

static UINT32 Shell_CreateExecTask(UINT32 SmpCoreSet, UINT32 TaskPriority, void *pTaskStack, UINT32 TaskStackSize)
{
    static char shell_exec_task_name[] = "AmbaShell_ExecTask";

    UINT32 RetVal = AmbaKAL_TaskCreate(&_AmbaShellCtrl.ExecTask,
                                       shell_exec_task_name,
                                       TaskPriority,
                                       Shell_ExecTaskEntry,
                                       NULL,
                                       pTaskStack,
                                       TaskStackSize,
                                       0);

    if (RetVal == KAL_ERR_NONE) {
        RetVal = AmbaKAL_TaskSetSmpAffinity(&_AmbaShellCtrl.ExecTask, SmpCoreSet);
        if (RetVal != KAL_ERR_NONE) {
            /* need to handle this error? */
        }
    }

    return AmbaKAL_TaskResume(&_AmbaShellCtrl.ExecTask);
}

static void Shell_InitVariables(void)
{
    (void)AmbaWrap_memset(&_AmbaShellCtrl.CliTask, 0, sizeof(AMBA_KAL_TASK_t));
    (void)AmbaWrap_memset(&_AmbaShellCtrl.ExecTask, 0, sizeof(AMBA_KAL_TASK_t));
    (void)AmbaWrap_memset(&_AmbaShellCtrl.EventFlag, 0, sizeof(AMBA_KAL_EVENT_FLAG_t));
    (void)AmbaWrap_memset(&_AmbaShellCtrl.ExecMutex, 0, sizeof(AMBA_KAL_MUTEX_t));
    (void)AmbaWrap_memset(&_AmbaShellCtrl.CmdListMutex, 0, sizeof(AMBA_KAL_MUTEX_t));
    (void)AmbaWrap_memset(&_AmbaShellCtrl.PrintMutex, 0, sizeof(AMBA_KAL_MUTEX_t));
    _AmbaShellCtrl.ConsolePutCharCb = NULL;
    _AmbaShellCtrl.ConsoleGetCharCb = NULL;
    _AmbaShellCtrl.LastExecCmdResult = 0;
    _AmbaShellCtrl.StopExecCmdFlag = 0;
}

/**
 *  AmbaShell_Init - Configure Shell
 *  @param[in] pConfig shell configuration
 *  @return error code
 */
UINT32 AmbaShell_Init(const AMBA_SHELL_CONFIG_s *pConfig)
{
    UINT32  nRet = SHELL_ERR_SUCCESS;
    static char event_flag_name[] = "AmbaShellEventFlag";
    static char mutex_exec_name[] = "AmbaShellMutexExec";
    static char mutex_cmd_name[] = "AmbaShellMutexCmd";
    static char mutex_print_name[] = "AmbaShellMutexPrint";
    static char AmbaShellCmdHistoryBuf[AMBA_SHELL_MAX_CMD_HISTORY_SIZE] __attribute__((section(".bss.noinit")));

    Shell_InitVariables();

    if (AmbaKAL_EventFlagCreate(&_AmbaShellCtrl.EventFlag, event_flag_name) != KAL_ERR_NONE) {
        nRet = SHELL_ERR_RESOURCE_CREATE;
    } else {
        if (AmbaKAL_MutexCreate(&_AmbaShellCtrl.ExecMutex, mutex_exec_name) != KAL_ERR_NONE) {
            nRet = SHELL_ERR_RESOURCE_CREATE;
        } else {
            if (AmbaKAL_MutexCreate(&_AmbaShellCtrl.CmdListMutex, mutex_cmd_name) != KAL_ERR_NONE) {
                nRet = SHELL_ERR_RESOURCE_CREATE;
            } else {
                if (AmbaKAL_MutexCreate(&_AmbaShellCtrl.PrintMutex, mutex_print_name) != KAL_ERR_NONE) {
                    nRet = SHELL_ERR_RESOURCE_CREATE;
                }
            }
        }
    }

    if (nRet == KAL_ERR_NONE) {
        _AmbaShellCtrl.ConsoleGetCharCb = pConfig->GetCharFunc;
        _AmbaShellCtrl.ConsolePutCharCb = pConfig->PutCharFunc;
        _AmbaShellCtrl.LastExecCmdResult = SHELL_ERR_SUCCESS;

        /* Initialize command history control */
        AmbaShell_InitCmdHistory(AmbaShellCmdHistoryBuf, AMBA_SHELL_MAX_CMD_HISTORY_SIZE);

        /* Default working directory */
        AmbaUtility_StringCopy(_AmbaShellCtrl.CurrWorkDir, AMBA_SHELL_MAX_DIR_LENGTH, "a:\\");

        if (Shell_CreateCliTask(pConfig->CliTaskSmpCoreSet, pConfig->CliTaskPriority, pConfig->pCliTaskStack, pConfig->CliTaskStackSize) != KAL_ERR_NONE) {
            nRet = SHELL_ERR_RESOURCE_CREATE;
        } else {
            if (Shell_CreateExecTask(pConfig->ExecTaskSmpCoreSet, pConfig->ExecTaskPriority, pConfig->pExecTaskStack, pConfig->ExecTaskStackSize) != KAL_ERR_NONE) {
                nRet = SHELL_ERR_RESOURCE_CREATE;
            }
        }

    }

    return nRet;
}

/**
 *  AmbaShell_CommandRegister - register specified command
 *  @param[in] pNewCmd command structure
 *  @return error code
 */
UINT32 AmbaShell_CommandRegister(AMBA_SHELL_COMMAND_s *pNewCmd)
{
    AMBA_SHELL_COMMAND_s *pCmd;
    UINT32 nRet = SHELL_ERR_SUCCESS;

    if ((pNewCmd == NULL) || (pNewCmd->pName == NULL) || (pNewCmd->MainFunc == NULL)) {
        nRet = SHELL_ERR_INVALID_PARAMETER;
    } else {
        if (AmbaKAL_MutexTake(&_AmbaShellCtrl.CmdListMutex, SHELL_WAIT_FOREVER) != KAL_ERR_NONE) {
            nRet = SHELL_ERR_MUTEX_LOCK;
        } else {
            if (ShellCmdList == NULL) {
                // first command
                ShellCmdList = pNewCmd;
            } else {
                pCmd = ShellCmdList;
                while (pCmd->pNext != NULL) {
                    pCmd = pCmd->pNext;
                }
                pNewCmd->pNext = NULL;
                pCmd->pNext = pNewCmd;
            }

            if (AmbaKAL_MutexGive(&_AmbaShellCtrl.CmdListMutex) != KAL_ERR_NONE) {
                nRet = SHELL_ERR_MUTEX_UNLOCK;
            }
        }
    }
    return nRet;
}

/**
 *  AmbaShell_CurrentWorkDirGet - get current working folder name
 *  @return current working folder name
 */
const char *AmbaShell_CurrentWorkDirGet(void)
{
    return _AmbaShellCtrl.CurrWorkDir;
}

/**
 *  AmbaShell_CurrentWorkDirGet - set current working folder name
 *  @param[in] WorkingDir pointer to working directory
 */
void AmbaShell_CurrentWorkDirSet(const char *WorkingDir)
{
    AmbaUtility_StringCopy(_AmbaShellCtrl.CurrWorkDir, AMBA_SHELL_MAX_DIR_LENGTH, WorkingDir);
}

/**
 *  AmbaShell_CommandListGet - get command list
 *  @return command list
 */
const AMBA_SHELL_COMMAND_s *AmbaShell_CommandListGet(void)
{
    return ShellCmdList;
}

/**
 *  AmbaShell_CommandListGet - executed specified command
 *  @param[in] pCmdLine command to be executed
 *  @return error code
 */
UINT32 AmbaShell_ExecCommand(char *pCmdLine)
{
    return Shell_ParseAndDoExecCmd(pCmdLine);
}
