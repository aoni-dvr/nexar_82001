/**
 *  @file AmbaShell_LoadBin.c
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
 *  @details Load memory data from storage
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include <AmbaShell.h>
#include <AmbaFS.h>
#include <AmbaUtility.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

static inline void SHELL_LoadBinUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [filename] [address]\n");
}

/**
 *  AmbaShell_CommandLoadBinary - execute command LoadBin
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandLoadBinary(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 LoadAddr, LoadSize, TargetSize;
    AMBA_FS_FILE *pFile = NULL;
    AMBA_FS_FILE_INFO_s Info;
    UINT32 uRet = 0;

    if (ArgCount != 3U) {
        SHELL_LoadBinUsage(pArgVector, PrintFunc);
    } else {
        if (pArgVector[1][1] != ':') {
            PrintFunc("no drive assigned\n");
        } else {
            if (0U != AmbaFS_GetFileInfo(pArgVector[1], &Info)) {
                PrintFunc("'");
                PrintFunc(pArgVector[1]);
                PrintFunc("' file not found err: ");
                AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
                PrintFunc("!\n");
            }

            TargetSize = (UINT32)Info.Size;

            (void)AmbaUtility_StringToUInt32(pArgVector[2], &LoadAddr);

            uRet = AmbaFS_FileOpen(pArgVector[1], "r", &pFile);
            if (uRet != 0U) {
                PrintFunc("Fail to open ");
                PrintFunc(pArgVector[1]);
                PrintFunc(", err: ");
                AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
                PrintFunc("!\n");
            } else {
                UINT8 *LoadPtr = NULL;
                AmbaMisra_TypeCast32(&LoadPtr, &LoadAddr);
                uRet = AmbaFS_FileRead(LoadPtr, 1, TargetSize, pFile, &LoadSize);
                if ((LoadSize != TargetSize) || (uRet != 0U)) {
                    PrintFunc("Fail to read ");
                    PrintFunc(pArgVector[1]);
                    PrintFunc(", err: ");
                    AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
                    PrintFunc("!\n");
                } else {
                    if (AmbaFS_FileClose(pFile) != 0U) {
                        PrintFunc("Fail to read ");
                        PrintFunc(pArgVector[1]);
                        PrintFunc(", err: ");
                        AmbaShell_PrintUInt32(AmbaFS_GetError(), PrintFunc);
                        PrintFunc("!\n");
                    }
                }
            }
        }
    }

    return;
}
