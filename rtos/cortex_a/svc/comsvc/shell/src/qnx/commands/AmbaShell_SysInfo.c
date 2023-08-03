/**
 *  @file AmbaShell_SysInfo.c
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
 *  @details System Information shell command.
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"

#include "AmbaShell.h"
#include "AmbaShell_Commands.h"

//#include "AmbaSYS.h"
#include "AmbaVIN.h"
#include "AmbaVOUT.h"

static void SHELL_SysInfoCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#if 0
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [dram|irq|vout|vin] ([option])\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" dram [reset] - memory load info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" irq [hook|reset|all|<id>] - IRQ info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" vout [dump|<id>] - vout display info\n");
#else
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [vin] ([option])\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" vin [<id>] - vin info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" vout [<id>] - vout display info\n");
#endif
}

#if 0
static void SHELL_ShowDramLoadInfo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount >= 3U) {
        if (AmbaUtility_StringCompare(pArgVector[2], "reset", 5U) == 0) {
            (void)AmbaSYS_DispInfo(SYS_LOG_DRAM, SYS_LOG_FUNC_RESET, PrintFunc);
        } else {
            PrintFunc("Usage: ");
            PrintFunc(pArgVector[0]);
            PrintFunc(" dram [reset] - memory load info\n");
        }
    } else {
        (void)AmbaSYS_DispInfo(SYS_LOG_DRAM, SYS_LOG_FUNC_NORMAL, PrintFunc);
    }
}

static void SHELL_ShowIrqInfo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 IntID;

    if (ArgCount >= 3U) {
        if (AmbaUtility_StringCompare(pArgVector[2], "reset", 5U) == 0) {
            (void)AmbaSYS_DispInfo(SYS_LOG_INT, SYS_LOG_FUNC_RESET, PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[2], "all", 3U) == 0) {
            (void)AmbaSYS_DispInfo(SYS_LOG_INT, SYS_LOG_FUNC_NORMAL, PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[2], "hook", 4U) == 0) {
            (void)AmbaSYS_DispInfo(SYS_LOG_INT, SYS_LOG_FUNC_INIT, PrintFunc);
        } else {
            (void)AmbaUtility_StringToUInt32(pArgVector[2], &IntID);
            (void)AmbaSYS_DispInfo(SYS_LOG_INT, SYS_LOG_FUNC_CUSTOM + IntID, PrintFunc);
        }
    } else {
        (void)AmbaSYS_DispInfo(SYS_LOG_INT, SYS_LOG_FUNC_NORMAL, PrintFunc);
    }
}
#endif

static void SHELL_ShowVoutInfo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 VoutID;

    if (ArgCount >= 3U) {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &VoutID);
        (void)AmbaDiag_VoutShowInfo(VoutID, PrintFunc);
    } else {
        PrintFunc("Usage: ");
        PrintFunc(pArgVector[0]);
        PrintFunc(" vout [<id>] - vout display info\n");
    }
}

static void SHELL_ShowVinInfo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 VinID;

    if (ArgCount >= 3U) {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &VinID);
        if (AmbaUtility_StringCompare(pArgVector[3], "reset", 5U) == 0) {
            (void)AmbaDiag_VinResetStatus(VinID, PrintFunc);
        } else {
            (void)AmbaDiag_VinShowInfo(VinID, PrintFunc);
        }
    } else {
        PrintFunc("Usage: ");
        PrintFunc(pArgVector[0]);
        PrintFunc(" vin [<id>] - vin info\n");
        PrintFunc("               vin [<id>] reset - reset vin status\n");
    }
}

/**
 *  AmbaShell_CommandSysInfo - execute command sysinfo
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandSysInfo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount == 1U) {
        SHELL_SysInfoCmdUsage(pArgVector, PrintFunc);
#if 0
    } else if (AmbaUtility_StringCompare(pArgVector[1], "dram", 4U) == 0) {
        SHELL_ShowDramLoadInfo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "irq", 3U) == 0) {
        SHELL_ShowIrqInfo(ArgCount, pArgVector, PrintFunc);
#endif
    } else if (AmbaUtility_StringCompare(pArgVector[1], "vout", 4U) == 0) {
        SHELL_ShowVoutInfo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "vin", 3U) == 0) {
        SHELL_ShowVinInfo(ArgCount, pArgVector, PrintFunc);
    } else {
        SHELL_SysInfoCmdUsage(pArgVector, PrintFunc);
    }
}
