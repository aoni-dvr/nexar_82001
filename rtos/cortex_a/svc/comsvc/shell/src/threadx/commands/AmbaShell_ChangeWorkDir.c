/**
 *  @file AmbaShell_ChangeWorkDir.c
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
 *  @details Shell command to change working directory
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaShell.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

static void SHELL_CdCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [path]\n");
}

static void up_dir(char *LastWorkDir)
{
    SIZE_t position;

    position = AmbaUtility_StringLength(LastWorkDir);
    while (LastWorkDir[position] != '\\') {
        LastWorkDir[position] = '\0';
        if (position == 0U) {
            break;
        }
        position--;
    }
    LastWorkDir[position] = '\0';
    if (position != 0U) {
        position--;
    }
    while (LastWorkDir[position] != '\\') {
        LastWorkDir[position] = '\0';
        if (position == 0U) {
            break;
        }
        position--;
    }
    LastWorkDir[position] = '\0';
    return;
}

static void set_dir(char *LastWorkDir)
{
    SIZE_t length;

    /* make sure the last character is '\\' */
    length = AmbaUtility_StringLength(LastWorkDir);
    if (length > 0U) {
        if (LastWorkDir[length - 1U] != '\\') {
            LastWorkDir[length] = '\\';
            LastWorkDir[length + 1U] = '\0';
        }
        AmbaShell_CurrentWorkDirSet(LastWorkDir);
    }
}

/**
 *  AmbaShell_CommandChangeDir - execute command 'cd'
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandChangeDir(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = 0U;
    const char *pCurrWorkDir = AmbaShell_CurrentWorkDirGet();
    char AbsolutePath[AMBA_SHELL_MAX_DIR_LENGTH] = {'\0'};
    char LastWorkDir[AMBA_SHELL_MAX_DIR_LENGTH] = {'\0'};
    SIZE_t MaxPathLength = sizeof(AbsolutePath);

    if (ArgCount < 2U) {
        SHELL_CdCmdUsage(pArgVector, PrintFunc);
    } else {
        if ((pArgVector[1][0] == '.') && (pArgVector[1][1] == '\0')) {
            /* cd . is a nop */
        } else if ((pArgVector[1][0] == '.') && (pArgVector[1][1] == '.') && (pArgVector[1][2] == '\0') &&
                   (pCurrWorkDir[3] == '\0')) {
            /* cd .. is a nop */
        } else {
            /* Make absoulte path */
            if (pArgVector[1][1] == ':') {
                AmbaUtility_StringCopy(AbsolutePath, AMBA_SHELL_MAX_DIR_LENGTH, pArgVector[1]);
            } else {
                RetVal = AMBSH_CHKCWD(PrintFunc);
                if (RetVal == 0U) {
                    SIZE_t LenXyz = AmbaUtility_StringLength(AbsolutePath);
                    LenXyz += AmbaUtility_StringLength(pArgVector[1]);
                    AmbaUtility_StringCopy(AbsolutePath, AMBA_SHELL_MAX_DIR_LENGTH, pCurrWorkDir);
                    if (LenXyz < MaxPathLength) {
                        AmbaUtility_StringAppend(AbsolutePath, AMBA_SHELL_MAX_DIR_LENGTH, pArgVector[1]);
                    } else {
                        PrintFunc("Path ");
                        PrintFunc(AbsolutePath);
                        PrintFunc("\\");
                        PrintFunc(pArgVector[1]);
                        PrintFunc(" is too long");
                        RetVal = 0xFFFFFFFFU;
                    }
                }
            }

            if (RetVal == 0U) {
                RetVal = AmbaFS_ChangeDir(AbsolutePath);
                if ((pArgVector[1][0] == ':') || (RetVal != 0U)) {
                    PrintFunc("Cannot change working directory to '");
                    PrintFunc(AbsolutePath);
                    PrintFunc("' (ErrorCode: ");
                    AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
                    PrintFunc(")\n");
                    RetVal = 0xFFFFFFFFU;
                }

                if (RetVal == 0U) {
                    /* Update current working directory */
                    AmbaUtility_StringCopy(LastWorkDir, AMBA_SHELL_MAX_DIR_LENGTH, AbsolutePath);

                    /* We are here because of success in AmbaFS_ChangeDir() */
                    if ((pArgVector[1][0] == '.') && (pArgVector[1][1] == '.') && (pArgVector[1][2] == '\0')) {
                        up_dir(LastWorkDir);
                    }

                    set_dir(LastWorkDir);
                }
            }
        }
    }
}

