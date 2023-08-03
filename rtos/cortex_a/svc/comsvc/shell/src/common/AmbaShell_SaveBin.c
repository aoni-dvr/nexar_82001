/**
 *  @file AmbaShell_SaveBin.c
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
 *  @details Dump memory data to storage
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

#define AMBA_SHELL_SAVABIN_WRITE_SIZE 524288U //(512 << 10)

static void SHELL_SaveBinCmdUsage(char *const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [Filename] [Addr1] to [Addr2] : Saving binary from the start address to the end address\n");
    PrintFunc("         [Filename]: The file for saving binary data\n");
    PrintFunc("         [Addr1]: The start address\n");
    PrintFunc("         [Addr1]: The end address\n");
    PrintFunc("\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [Filename] [Addr] l [Len] : Saving binary in an range of addresses\n");
    PrintFunc("         [Filename]: The file for saving binary data\n");
    PrintFunc("         [Addr]: The start address\n");
    PrintFunc("         [Len]: The binary length\n");
}

/**
 *  AmbaShell_CommandSaveBinary - execute command save bin
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandSaveBinary(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Rval = 0;
    UINT64 MemoryAddress, EndMemoryAddress;
    UINT32 Len = 0, ByteWritten;
    char FileName[256];
    AMBA_FS_FILE *pFile = NULL;

    if (ArgCount != 5U) {
        SHELL_SaveBinCmdUsage(pArgVector, PrintFunc);
    } else {
        AmbaUtility_StringCopy(FileName, sizeof(FileName), pArgVector[1]);
        (void)AmbaUtility_StringToUInt64(pArgVector[2], &MemoryAddress);

        if (AmbaUtility_StringCompare(pArgVector[3], "to", 2) == 0) {
            (void)AmbaUtility_StringToUInt64(pArgVector[4], &EndMemoryAddress);
            Len = (UINT32)(EndMemoryAddress - MemoryAddress) + 1U;
        } else if (pArgVector[3][0] == 'l') {
            (void)AmbaUtility_StringToUInt32(pArgVector[4], &Len);
        } else {
            PrintFunc( "unknown syntax!\n");
            Rval = 0xFFFFFFFFU;
        }

        if (Rval == 0U) {
            Rval = AmbaFS_FileOpen(FileName, "w", &pFile);
            if ((pFile == NULL) || (Rval != 0U)) {
                Rval = AmbaFS_GetError();
                PrintFunc(pArgVector[0]);
                PrintFunc(": Fail to open file ");
                PrintFunc(FileName);
                PrintFunc(" err: ");
                AmbaShell_PrintUInt32(Rval, PrintFunc);
                PrintFunc("!\n");
            } else {
                UINT32 i;
                if ((Len / AMBA_SHELL_SAVABIN_WRITE_SIZE) > 0U) {
                    for (i = 0; i < (Len / AMBA_SHELL_SAVABIN_WRITE_SIZE); i++) {
                        UINT8 *MemoryPtr = NULL;
                        AmbaMisra_TypeCast(&MemoryPtr, &MemoryAddress);
                        Rval = AmbaFS_FileWrite(MemoryPtr, 1, AMBA_SHELL_SAVABIN_WRITE_SIZE, pFile, &ByteWritten);
                        if ((ByteWritten != AMBA_SHELL_SAVABIN_WRITE_SIZE) || (Rval != 0U)) {
                            Rval = AmbaFS_GetError();
                            PrintFunc(pArgVector[0]);
                            PrintFunc(": Fail to write file ");
                            PrintFunc(FileName);
                            PrintFunc(" err: ");
                            AmbaShell_PrintUInt32(Rval, PrintFunc);
                            PrintFunc("!\n");
                            break;
                        } else {
                            MemoryAddress += AMBA_SHELL_SAVABIN_WRITE_SIZE;
                            (void)AmbaKAL_TaskSleep(10);
                        }
                    }
                    if ((Len % AMBA_SHELL_SAVABIN_WRITE_SIZE) != 0U) {
                        UINT8 *MemoryPtr = NULL;
                        AmbaMisra_TypeCast(&MemoryPtr, &MemoryAddress);
                        Rval = AmbaFS_FileWrite(MemoryPtr, 1, (Len % AMBA_SHELL_SAVABIN_WRITE_SIZE), pFile, &ByteWritten);
                        if ((ByteWritten != (Len % AMBA_SHELL_SAVABIN_WRITE_SIZE)) || (Rval != 0U)) {
                            Rval = AmbaFS_GetError();
                            PrintFunc(pArgVector[0]);
                            PrintFunc(": Fail to write file ");
                            PrintFunc(FileName);
                            PrintFunc(" err: ");
                            AmbaShell_PrintUInt32(Rval, PrintFunc);
                            PrintFunc("!\n");
                        } else {
                            MemoryAddress += AMBA_SHELL_SAVABIN_WRITE_SIZE;
                        }
                    }
                } else {
                    UINT8 *MemoryPtr = NULL;
                    AmbaMisra_TypeCast(&MemoryPtr, &MemoryAddress);
                    Rval = AmbaFS_FileWrite(MemoryPtr, 1, Len, pFile, &ByteWritten);
                    if ((ByteWritten != Len) || (Rval != 0U)) {
                        Rval = AmbaFS_GetError();
                        PrintFunc(pArgVector[0]);
                        PrintFunc(": Fail to write file ");
                        PrintFunc(FileName);
                        PrintFunc(" err: ");
                        AmbaShell_PrintUInt32(Rval, PrintFunc);
                        PrintFunc("!\n");
                    }
                }
                Rval = AmbaFS_FileClose(pFile);
                if (Rval != 0U) {
                    Rval = AmbaFS_GetError();
                    PrintFunc(pArgVector[0]);
                    PrintFunc(": Fail to close file ");
                    PrintFunc(FileName);
                    PrintFunc(" err: ");
                    AmbaShell_PrintUInt32(Rval, PrintFunc);
                    PrintFunc("!\n");
                }
            }
        }
    }

    return;
}
