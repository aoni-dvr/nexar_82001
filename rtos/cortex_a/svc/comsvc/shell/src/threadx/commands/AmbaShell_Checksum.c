/**
 *  @file AmbaShell_Checksum.c
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
 *  @details Calculate checksum shell command.
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaShell.h>
#include <AmbaUtility.h>
#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

static void SHELL_CheckSumCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <filename>\n");
}

/**
 *  AmbaShell_CommandCRC32Sum - execute command crc32sum
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandCRC32Sum(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_FS_FILE *pFile = NULL;
    UINT8  Buf[256];
    UINT32 ActSize;
    UINT32 Crc32 = 0xFFFFFFFFU;
    UINT32 uRet = 0;

    if (ArgCount < 2U) {
        SHELL_CheckSumCmdUsage(pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[0], "crc32sum", 8) != 0) {
        SHELL_CheckSumCmdUsage(pArgVector, PrintFunc);
    } else {
        uRet = AmbaFS_FileOpen(pArgVector[1], "r", &pFile);
        if (uRet == 0U) {
            while (AmbaFS_FileEof(pFile) == 0U) {
                uRet = AmbaFS_FileRead(Buf, (UINT32)sizeof(UINT8), (UINT32)sizeof(Buf), pFile, &ActSize);
                if ((uRet == 0U) && (ActSize > 0U)) {
                    Crc32 = AmbaUtility_Crc32Add(Buf, ActSize, Crc32);
                }
            }
            Crc32 = AmbaUtility_Crc32Finalize(Crc32);
            AmbaShell_PrintHexUInt32(Crc32, PrintFunc);
            PrintFunc("  ");
            PrintFunc(pArgVector[1]);
            PrintFunc("\n");

            if (AmbaFS_FileClose(pFile) != 0U) {
                // ignore error
            }
        }
    }
    return;
}
