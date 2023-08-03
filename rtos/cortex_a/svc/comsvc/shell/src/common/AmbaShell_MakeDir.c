/**
 *  @file AmbaShell_MakeDir.c
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
 *  @details Shell command to create a directory
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaShell.h>
#include <AmbaFS.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

static inline void SHELL_MkdirCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Make directory.\n\n");
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [dir]\n");
}

/**
 *  AmbaShell_CommandMakeDir - execute command mkdir
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandMakeDir(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = 0U;
    char   AbsolutePath[256] = {'\0'};
    SIZE_t TargetLength;

    if (ArgCount < 2U) {
        SHELL_MkdirCmdUsage(pArgVector, PrintFunc);
    } else {
        if (pArgVector[1][1] == ':') {
            AmbaUtility_StringCopy(AbsolutePath, 256, pArgVector[1]);
        } else {
            RetVal = AMBSH_CHKCWD(PrintFunc);
            if (RetVal == 0U) {
                TargetLength = AmbaUtility_StringLength(AmbaShell_CurrentWorkDirGet());
                TargetLength += AmbaUtility_StringLength(pArgVector[1]);

                if (TargetLength < sizeof(AbsolutePath)) {
                    AmbaUtility_StringCopy(AbsolutePath, 256, AmbaShell_CurrentWorkDirGet());
                    AmbaUtility_StringAppend(AbsolutePath, 256, pArgVector[1]);
                } else {
                    PrintFunc("Unable create directory ");
                    PrintFunc(pArgVector[1]);
                    PrintFunc(" under ");
                    PrintFunc(AmbaShell_CurrentWorkDirGet());
                    PrintFunc("\n");
                    RetVal = 0xFFFFFFFFU;
                }
            }
        }

        if (RetVal == 0U) {
            RetVal = AmbaFS_MakeDir(AbsolutePath);
            if (RetVal != 0U) {
                PrintFunc(pArgVector[0]);
                PrintFunc(": '");
                PrintFunc(AbsolutePath);
                PrintFunc("': cannot create directory!\n");
            }
        }
    }

    return;
}
