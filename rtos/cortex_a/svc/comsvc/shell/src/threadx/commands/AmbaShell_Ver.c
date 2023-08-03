/**
 *  @file AmbaShell_Ver.c
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
 *  @details Display version information of modules
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaVer.h>
#include <AmbaShell.h>
#include <AmbaFS.h>
#include <AmbaUtility.h>
#include "AmbaShell_Commands.h"
#include "AmbaShell_Utility.h"

static AMBA_SHELL_LIBINFO   *LibInfoArray = NULL;
static UINT32                LibInfoNumber = 0;
static AMBA_SHELL_LINKINFO  *LinkInfo = NULL;

static void SHELL_VerBinUsage(char * const * pArgv, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgv[0]);
    PrintFunc("               Show all libraries Version\n");

    PrintFunc("       ");
    PrintFunc(pArgv[0]);
    PrintFunc(" [LibName]     Show the library version\n");

    PrintFunc("       ");
    PrintFunc(pArgv[0]);
    PrintFunc(" -e            Show elf info\n");

    PrintFunc("       ");
    PrintFunc(pArgv[0]);
    PrintFunc(" -h            Show help\n");
}

static void Show(AMBA_SHELL_PRINT_f PrintFunc, const AMBA_VerInfo_s *pVerInfo)
{
    PrintFunc("Module:          ");
    PrintFunc(pVerInfo->Key);
    PrintFunc("\n");

    PrintFunc("Built Machine:   ");
    PrintFunc(pVerInfo->MachStr);
    PrintFunc("\n");

    if (pVerInfo->DateStr != NULL) {
        if (AmbaUtility_StringLength(pVerInfo->DateStr) != 0U) {
            PrintFunc("Built Date:      ");
            PrintFunc(pVerInfo->DateStr);
            PrintFunc("\n");
        }
    }

    PrintFunc("Commit count:    ");
    AmbaShell_PrintUInt32(pVerInfo->CiCount, PrintFunc);
    PrintFunc("\n");

    PrintFunc("Commit ID:       ");
    PrintFunc(pVerInfo->CiIdStr);
    PrintFunc("\n");
}

static void ShowAllVerInfo(AMBA_SHELL_PRINT_f PrintFunc)
{
    static AMBA_VerInfo_s Info;
    UINT32 i;

    if ((LibInfoArray != NULL) && (LibInfoNumber > 0U)) {
        for (i = 0; i < LibInfoNumber; i++) {
            if (LibInfoArray[i].pLibName != NULL) {
                LibInfoArray[i].FuncSet(&Info);
                Info.Key = LibInfoArray[i].pLibName;
                Show(PrintFunc, &Info);
            }
        }
    }

#if defined(CONFIG_THREADX64)
    PrintFunc("OS: ThreadX 64-bit\r\n");
#elif defined(CONFIG_THREADX)
    PrintFunc("OS: ThreadX 32-bit\r\n");
#else
    PrintFunc("OS: Unknown\r\n");
#endif
}

static void ShowLinkVerInfo(AMBA_SHELL_PRINT_f PrintFunc)
{
    if (LinkInfo != NULL) {
        PrintFunc("Built ");
        if (LinkInfo->pLinkTarget == NULL) {
            PrintFunc("Unknown");
        } else {
            PrintFunc(LinkInfo->pLinkTarget);
        }
        PrintFunc(" by ");
        if (LinkInfo->pLinkMachine != NULL) {
            PrintFunc(LinkInfo->pLinkMachine);
        } else {
            PrintFunc("Unknown");
        }
        PrintFunc(" at ");
        if (LinkInfo->pLinkDate != NULL) {
            PrintFunc(LinkInfo->pLinkDate);
        } else {
            PrintFunc("Unknown");
        }
        PrintFunc("\n");
    }
}

static void ShowAllLibs(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;
    if ((LibInfoArray != NULL) && (LibInfoNumber > 0U)) {
        for (i = 0; i < LibInfoNumber; i++) {
            if (LibInfoArray[i].pLibName != NULL) {
                PrintFunc(LibInfoArray[i].pLibName);
                PrintFunc("\n");
            }
        }
    }
}

static void ShowVerInfo(AMBA_SHELL_PRINT_f PrintFunc, const char *pModuleName)
{
    AMBA_VerInfo_s VerInfo;
    UINT32 i;

    if ((LibInfoArray != NULL) && (LibInfoNumber > 0U)) {
        for (i = 0; i < LibInfoNumber; i++) {
            if (LibInfoArray[i].pLibName != NULL) {
                if (AmbaUtility_StringCompare(LibInfoArray[i].pLibName, pModuleName, AmbaUtility_StringLength(pModuleName)) == 0) {
                    LibInfoArray[i].FuncSet(&VerInfo);
                    VerInfo.Key = LibInfoArray[i].pLibName;
                    Show(PrintFunc, &VerInfo);
                    break;
                }
            }
        }
    }
}

void AmbaShell_CommandVersionInit(AMBA_SHELL_LIBINFO *pLibInfo, UINT32 InfoNumber, AMBA_SHELL_LINKINFO *pLinkInfo)
{
    LibInfoArray = pLibInfo;
    LibInfoNumber = InfoNumber;
    LinkInfo = pLinkInfo;
}

/**
 *  AmbaShell_CommandVersion - show command version
 *  @param[in] Argc argument count
 *  @param[in] pArgv argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandVersion(UINT32 Argc, char * const * pArgv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (Argc == 1U) {
        ShowAllVerInfo(PrintFunc);
        ShowLinkVerInfo(PrintFunc);

    } else if (AmbaUtility_StringCompare(pArgv[1], "-h", 2) == 0) {
        SHELL_VerBinUsage(pArgv, PrintFunc);
        PrintFunc("Avaiable library name:\n");
        ShowAllLibs(PrintFunc);

    } else if ((AmbaUtility_StringCompare(pArgv[1], "-e", 2) == 0) && (Argc == 2U)) {
        ShowLinkVerInfo(PrintFunc);

    } else if (Argc == 2U) {
        ShowVerInfo(PrintFunc, pArgv[1]);
    } else {
        // pass vcast check
    }
    return;
}
