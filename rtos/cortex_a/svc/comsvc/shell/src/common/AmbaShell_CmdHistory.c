/**
 *  @file AmbaShell_CmdHistory.c
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
 *  @details Command History Funcitons
 *
 */

#include <AmbaShell.h>
#include <AmbaUtility.h>
#include "AmbaShell_CmdHistory.h"

/*
 * SHELL: AMBA SHELL command history structure
 */
typedef struct {
    char   *pCmdBuf;
    UINT32  CmdBufSize;
    UINT32  CurrentIndex;      /* the current selected command of Shell Command Buffer */
    UINT32  OldestIndex;       /* the oldest command of Shell Command Buffer */
    UINT32  LatestIndex;       /* the latest command of Shell Command Buffer */
} AMBA_SHELL_CMD_HISTORY_CTRL_s;

static AMBA_SHELL_CMD_HISTORY_CTRL_s AmbaShellCmdHistoryCtrl = {
    NULL,
    0,
    0,    /* the current selected command of Shell Command Buffer */
    0,    /* the oldest command of Shell Command Buffer */
    0,    /* the latest command of Shell Command Buffer */
};

/**
 *  AmbaShell_InitCmdHistory - initial command history
 *  @param[in] pCmdHistoryPool command history buffer
 *  @param[in] CmdHistoryPoolSize command history buffer size
 */
void AmbaShell_InitCmdHistory(char *pCmdHistoryPool, UINT32 CmdHistoryPoolSize)
{
    AmbaShellCmdHistoryCtrl.pCmdBuf    = pCmdHistoryPool;
    AmbaShellCmdHistoryCtrl.CmdBufSize = CmdHistoryPoolSize;
    AmbaUtility_MemorySetChar(AmbaShellCmdHistoryCtrl.pCmdBuf, (char)0x0, AmbaShellCmdHistoryCtrl.CmdBufSize);
}

static void memcpy_char(char *dst, const char *src, UINT32 size)
{
    UINT32 i;
    for (i = 0; i < size; i++) {
        dst[i] = src[i];
    }
}

/**
 *  AmbaShell_SetCmdHistory - set command history
 *  @param[in] pCmdLine new command line
 */
void AmbaShell_SetCmdHistory(const char *pCmdLine)
{
    AMBA_SHELL_CMD_HISTORY_CTRL_s *pCmdHistoryCtrl = &AmbaShellCmdHistoryCtrl;
    UINT32 CmdLineSize = (UINT32)AmbaUtility_StringLength(pCmdLine);
    UINT32 HeadIndex, TmpIndex;

    if ((CmdLineSize == 0U) ||
        (CmdLineSize >= (pCmdHistoryCtrl->CmdBufSize - 1U)) ||
        (pCmdHistoryCtrl->pCmdBuf == NULL) ||
        (pCmdHistoryCtrl->CmdBufSize == 0U)) {
        // do nothing
    } else {

        /* save command to history */
        HeadIndex = pCmdHistoryCtrl->LatestIndex;
        if ((HeadIndex + CmdLineSize + 1U) >= pCmdHistoryCtrl->CmdBufSize) {
            /* Back to the begining of the circular buffer */
            AmbaUtility_MemorySetChar(&pCmdHistoryCtrl->pCmdBuf[HeadIndex], (char)0x0, pCmdHistoryCtrl->CmdBufSize - HeadIndex);
            memcpy_char(&pCmdHistoryCtrl->pCmdBuf[0], pCmdLine, CmdLineSize);
            pCmdHistoryCtrl->LatestIndex = CmdLineSize;
        } else {
            /* No wrap */
            memcpy_char(&pCmdHistoryCtrl->pCmdBuf[HeadIndex], pCmdLine, CmdLineSize);
            pCmdHistoryCtrl->LatestIndex = HeadIndex + CmdLineSize;
        }

        /* Append string end '\0' and clear dameged command */
        TmpIndex = pCmdHistoryCtrl->LatestIndex;
        while (TmpIndex < pCmdHistoryCtrl->CmdBufSize) {
            if (pCmdHistoryCtrl->pCmdBuf[TmpIndex] != '\0') {
                pCmdHistoryCtrl->pCmdBuf[TmpIndex] = '\0';
            }
            TmpIndex ++;
        }
        pCmdHistoryCtrl->LatestIndex++;

        /* Update the oldest index to the begining of the first undameged command */
        if (TmpIndex >= pCmdHistoryCtrl->CmdBufSize) {
            /* This case should not be happened!! */
            TmpIndex = 0;
        }

        if (pCmdHistoryCtrl->pCmdBuf[TmpIndex] != '\0') {
            pCmdHistoryCtrl->OldestIndex = TmpIndex;
        }

        pCmdHistoryCtrl->CurrentIndex = pCmdHistoryCtrl->LatestIndex;

    }

    return;
}

/**
 *  AmbaShell_RestoreCmdHistory - restore command history
 *  @param[in] Backward direction
 *  @return current command
 */
char *AmbaShell_RestoreCmdHistory(UINT32 Backward)
{
    AMBA_SHELL_CMD_HISTORY_CTRL_s *pCmdHistoryCtrl = &AmbaShellCmdHistoryCtrl;
    UINT32 NextIndex;
    char   *Ret = NULL;
    UINT32 IsBreak = 0;

    if ((pCmdHistoryCtrl->pCmdBuf == NULL) || (pCmdHistoryCtrl->CmdBufSize == 0U)) {
        // do nothing
    } else {
        if (Backward != 0U) {
            if (pCmdHistoryCtrl->CurrentIndex == pCmdHistoryCtrl->OldestIndex) {
                // do nothing
            } else {
                /* Intentionally go backward the current index for finding the previous command */
                pCmdHistoryCtrl->CurrentIndex--;
                if (pCmdHistoryCtrl->CurrentIndex == 0xFFFFFFFFUL) {
                    pCmdHistoryCtrl->CurrentIndex = pCmdHistoryCtrl->CmdBufSize - 1U;
                }

                for (;;) {
                    if (IsBreak != 0U) {
                        break;
                    }
                    if (pCmdHistoryCtrl->CurrentIndex == pCmdHistoryCtrl->OldestIndex) {
                        IsBreak = 1;
                        continue;  /* Avoid to exceed the oldest index */
                    }

                    NextIndex = pCmdHistoryCtrl->CurrentIndex - 1U;
                    if (NextIndex == 0xFFFFFFFFUL) {
                        NextIndex = pCmdHistoryCtrl->CmdBufSize - 1U;
                    }

                    /* Check if the current index has reach the previous command */
                    if ((pCmdHistoryCtrl->pCmdBuf[NextIndex] == '\0') && (pCmdHistoryCtrl->pCmdBuf[pCmdHistoryCtrl->CurrentIndex] != '\0')) {
                        IsBreak = 1;
                        continue;
                    } else {
                        pCmdHistoryCtrl->CurrentIndex = NextIndex;
                    }
                }
            }

        } else {
            if (pCmdHistoryCtrl->CurrentIndex == pCmdHistoryCtrl->LatestIndex) {
                // do nothing
            } else {
                for (;;) {
                    if (IsBreak != 0U) {
                        break;
                    }
                    if (pCmdHistoryCtrl->CurrentIndex == pCmdHistoryCtrl->LatestIndex) {
                        IsBreak = 1;
                        continue;  /* Avoid to exceed the latest index */
                    }

                    NextIndex = pCmdHistoryCtrl->CurrentIndex + 1U;
                    if (NextIndex >= pCmdHistoryCtrl->CmdBufSize) {
                        NextIndex = 0;
                    }

                    /* Check if the current index has reach the next command */
                    if ((pCmdHistoryCtrl->pCmdBuf[pCmdHistoryCtrl->CurrentIndex] == '\0') && (pCmdHistoryCtrl->pCmdBuf[NextIndex] != '\0')) {
                        pCmdHistoryCtrl->CurrentIndex = NextIndex;
                        IsBreak = 1;
                        continue;
                    } else {
                        pCmdHistoryCtrl->CurrentIndex = NextIndex;
                    }
                }
            }
        }

        Ret = &pCmdHistoryCtrl->pCmdBuf[pCmdHistoryCtrl->CurrentIndex];
    }
    return Ret;
}

/**
 *  AmbaShell_RestoreCmdHistory - clean command history
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_ClearAllCmdHistory(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Clear entire history command buffer... ");

    AmbaUtility_MemorySetChar(AmbaShellCmdHistoryCtrl.pCmdBuf, (char)0x0, AmbaShellCmdHistoryCtrl.CmdBufSize);
    AmbaShellCmdHistoryCtrl.CurrentIndex = 0;
    AmbaShellCmdHistoryCtrl.OldestIndex  = 0;
    AmbaShellCmdHistoryCtrl.LatestIndex  = 0;

    PrintFunc("done\n");
}

/**
 *  AmbaShell_RestoreCmdHistory - execute command from command history
 *  @param[in] CmdId command id
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_ExecCmdHistory(UINT32 CmdId, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_SHELL_CMD_HISTORY_CTRL_s *pCmdHistoryCtrl = &AmbaShellCmdHistoryCtrl;
    UINT32 i, CurrentIndexBackup;
    char *pCmdStr = NULL;

    /* Backup the currrent history command index */
    CurrentIndexBackup = pCmdHistoryCtrl->CurrentIndex;

    /* backward to the latest command line */
    do {
        pCmdStr = AmbaShell_RestoreCmdHistory(0);
    } while (pCmdStr != NULL);

    /* Execute the specified history command */
    for (i = 0; i <= (CmdId + 1U); i++) {
        pCmdStr = AmbaShell_RestoreCmdHistory(1);
        if (pCmdStr != NULL) {
            if (i == (CmdId + 1U)) {
                PrintFunc(pCmdStr);
                PrintFunc("\n");
                (void)AmbaShell_ExecCommand(pCmdStr);
            }
        } else {
            break;
        }
    }

    /* Restore the currrent history command index */
    pCmdHistoryCtrl->CurrentIndex = CurrentIndexBackup;
}

/**
 *  AmbaShell_RestoreCmdHistory - show command history
 *  @param[in] CmdCount total command count
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_ShowCmdHistory(UINT32 CmdCount, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_SHELL_CMD_HISTORY_CTRL_s *pCmdHistoryCtrl = &AmbaShellCmdHistoryCtrl;
    UINT32 i, CurrentIndexBackup;
    const char *pCmdStr = NULL;
    char   uint32_string[UTIL_MAX_INT_STR_LEN];
    UINT32 uint32_args[2];

    /* Backup the currrent history command index */
    CurrentIndexBackup = pCmdHistoryCtrl->CurrentIndex;

    /* backward to the latest command line */
    do {
        pCmdStr = AmbaShell_RestoreCmdHistory(0);
    } while (pCmdStr != NULL);

    /* Show history commands */
    for (i = 0; i < CmdCount; i++) {
        pCmdStr = AmbaShell_RestoreCmdHistory(1);
        if (pCmdStr == NULL) {
            break;
        } else {
            PrintFunc(" [");
            uint32_args[0] = i;
            (void)AmbaUtility_StringPrintUInt32(uint32_string, UTIL_MAX_INT_STR_LEN, "%d", 1, uint32_args);
            PrintFunc(uint32_string);
            PrintFunc(" ]\t");
            PrintFunc(pCmdStr);
            PrintFunc("\n");
        }
    }

    /* Restore the currrent history command index */
    pCmdHistoryCtrl->CurrentIndex = CurrentIndexBackup;
}
