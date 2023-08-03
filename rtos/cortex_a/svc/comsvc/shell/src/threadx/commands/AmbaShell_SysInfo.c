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

#include "AmbaSYS.h"

static void SHELL_SysInfoCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
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
}

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

static void SHELL_ShowVoutInfo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 VoutID;

    if (ArgCount >= 3U) {
        if (AmbaUtility_StringCompare(pArgVector[2], "dump", 4U) == 0) {
            (void)AmbaSYS_DispInfo(SYS_LOG_VOUT, SYS_LOG_FUNC_RAW, PrintFunc);
        } else {
            (void)AmbaUtility_StringToUInt32(pArgVector[2], &VoutID);
            (void)AmbaSYS_DispInfo(SYS_LOG_VOUT, SYS_LOG_FUNC_CUSTOM + VoutID, PrintFunc);
        }
    } else {
        PrintFunc("Usage: ");
        PrintFunc(pArgVector[0]);
        PrintFunc(" vout [<id>] - vout display info\n");
        PrintFunc("               vout dump - dump vout registers\n");
    }
}

static void SHELL_ShowVinInfo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 VinID, ModuleID;

    if (ArgCount >= 3U) {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &VinID);
        if (VinID == 0U) {
            ModuleID = SYS_LOG_VIN;
        } else if (VinID == 1U) {
            ModuleID = SYS_LOG_PIP;
        } else if (VinID == 2U) {
            ModuleID = SYS_LOG_PIP2;
        } else if (VinID == 3U) {
            ModuleID = SYS_LOG_PIP3;
        } else if (VinID == 4U) {
            ModuleID = SYS_LOG_PIP4;
        } else if (VinID == 5U) {
            ModuleID = SYS_LOG_PIP5;
        } else if (VinID == 6U) {
            ModuleID = SYS_LOG_PIP6;
        } else if (VinID == 7U) {
            ModuleID = SYS_LOG_PIP7;
        } else if (VinID == 8U) {
            ModuleID = SYS_LOG_PIP8;
        } else if (VinID == 9U) {
            ModuleID = SYS_LOG_PIP9;
        } else if (VinID == 10U) {
            ModuleID = SYS_LOG_PIP10;
        } else if (VinID == 11U) {
            ModuleID = SYS_LOG_PIP11;
        } else if (VinID == 12U) {
            ModuleID = SYS_LOG_PIP12;
        } else if (VinID == 13U) {
            ModuleID = SYS_LOG_PIP13;
        } else {
            ModuleID = SYS_LOG_VIN;
        }
        if (AmbaUtility_StringCompare(pArgVector[3], "reset", 5U) == 0) {
            (void)AmbaSYS_DispInfo(ModuleID, SYS_LOG_FUNC_RESET, PrintFunc);
        } else {
            (void)AmbaSYS_DispInfo(ModuleID, SYS_LOG_FUNC_NORMAL, PrintFunc);
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
    } else if (AmbaUtility_StringCompare(pArgVector[1], "dram", 4U) == 0) {
        SHELL_ShowDramLoadInfo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "irq", 3U) == 0) {
        SHELL_ShowIrqInfo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "vout", 4U) == 0) {
        SHELL_ShowVoutInfo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "vin", 3U) == 0) {
        SHELL_ShowVinInfo(ArgCount, pArgVector, PrintFunc);
    } else {
        SHELL_SysInfoCmdUsage(pArgVector, PrintFunc);
    }
}
