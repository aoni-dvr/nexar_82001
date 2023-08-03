/**
 *  @file AmbaShell_SaveImage.c
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
 *  @details Dump image data to storage from memory
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

static void SHELL_SaveImageCmdUsage(char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [Filename] [Addr] [Width] [Height] [Pitch]: Saving image with dimension Width x Height\n");
    PrintFunc("         [Filename]: The file for saving image data\n");
    PrintFunc("         [Addr]: The start address\n");
    PrintFunc("         [Width]: Image width (in bytes)\n");
    PrintFunc("         [Height]: Image height\n");
    PrintFunc("         [Pitch]: Image pitch (in bytes)\n");
}

/**
 *  AmbaShell_CommandSaveImage - execute command save image
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandSaveImage(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, Rval;
    UINT32 MemoryAddress, Len, BytesWritten;
    UINT32 Width, Height, Pitch;
    char FileName[256];
    AMBA_FS_FILE *pFile = NULL;
    UINT8 *MemoryPtr = NULL;

    if (ArgCount != 6U) {
        SHELL_SaveImageCmdUsage(pArgVector, PrintFunc);
    } else {
        AmbaUtility_StringCopy(FileName, 256, pArgVector[1]);

        (void)AmbaUtility_StringToUInt32(pArgVector[2], &MemoryAddress);
        (void)AmbaUtility_StringToUInt32(pArgVector[3], &Width);
        (void)AmbaUtility_StringToUInt32(pArgVector[4], &Height);
        (void)AmbaUtility_StringToUInt32(pArgVector[5], &Pitch);

        if (Width > Pitch) {
            PrintFunc("Width (");
            AmbaShell_PrintUInt32(Width, PrintFunc);
            PrintFunc(") shouldn't be greater than Pitch (");
            AmbaShell_PrintUInt32(Pitch, PrintFunc);
            PrintFunc(")!\n");
        } else {
            Rval = AmbaFS_FileOpen(FileName, "w", &pFile);

            if (Rval != 0U) {
                Rval = AmbaFS_GetError();
                PrintFunc("Fail to open file '");
                PrintFunc(FileName);
                PrintFunc("' err: ");
                AmbaShell_PrintUInt32(Rval, PrintFunc);
                PrintFunc( "!\n");

            } else {
                if (Width == Pitch) {
                    Len = Width * Height;

                    AmbaMisra_TypeCast32(&MemoryPtr, &MemoryAddress);

                    Rval = AmbaFS_FileWrite(MemoryPtr, 1, Len, pFile, &BytesWritten);
                    if ((BytesWritten != Len) || (Rval != 0U)) {
                        Rval = AmbaFS_GetError();
                        PrintFunc("Fail to write file '");
                        PrintFunc(FileName);
                        PrintFunc("' err: ");
                        AmbaShell_PrintUInt32(Rval, PrintFunc);
                        PrintFunc( "!\n");
                    }
                } else {
                    UINT32 Addr;
                    Len = Width;
                    for (i = 0; i < Height; i++) {
                        Addr = MemoryAddress + (i * Pitch);
                        AmbaMisra_TypeCast32(&MemoryPtr, &Addr);
                        Rval = AmbaFS_FileWrite(MemoryPtr, 1, Len, pFile, &BytesWritten);

                        if ((BytesWritten != Len) || (Rval != 0U)) {
                            Rval = AmbaFS_GetError();
                            PrintFunc("Fail to write file '");
                            PrintFunc(FileName);
                            PrintFunc("' err: ");
                            AmbaShell_PrintUInt32(Rval, PrintFunc);
                            PrintFunc( "!\n");
                        }
                    }
                }

                Rval = AmbaFS_FileClose(pFile);
                if (Rval != 0U) {
                    Rval = AmbaFS_GetError();
                    PrintFunc("Fail to close file '");
                    PrintFunc(FileName);
                    PrintFunc("' err: ");
                    AmbaShell_PrintUInt32(Rval, PrintFunc);
                    PrintFunc( "!\n");
                }
            }
        }
    }
    return;
}
