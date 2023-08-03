/**
 *  @file AmbaShell_ProcessStatus.c
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
 *  @details Process Status shell command.
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"

#include "AmbaShell.h"
#include "AmbaShell_Commands.h"

#include "AmbaSYS.h"

static void SHELL_PsCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [tsk|mtx|sem|flg|msg|all|irq|profile] ([option])\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" tsk - Task info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" mtx - Mutex info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" sem - Semaphore info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" flg - Eventflag info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" msg - Message queue info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" all - All the above info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" profile [reset] - profiler result\n");
}

static void SHELL_ShowTaskInfo(AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)AmbaSYS_DispInfo(SYS_LOG_KAL_TASK, SYS_LOG_FUNC_NORMAL, PrintFunc);
}

static void SHELL_ShowMutexInfo(AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)AmbaSYS_DispInfo(SYS_LOG_KAL_MUTEX, SYS_LOG_FUNC_NORMAL, PrintFunc);
}

static void SHELL_ShowSemaphoreInfo(AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)AmbaSYS_DispInfo(SYS_LOG_KAL_SEMAPHORE, SYS_LOG_FUNC_NORMAL, PrintFunc);
}

static void SHELL_ShowEventFlagInfo(AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)AmbaSYS_DispInfo(SYS_LOG_KAL_EVENTFLAGS, SYS_LOG_FUNC_NORMAL, PrintFunc);
}

static void SHELL_ShowMessageQueueInfo(AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)AmbaSYS_DispInfo(SYS_LOG_KAL_MSG_QUEUE, SYS_LOG_FUNC_NORMAL, PrintFunc);
}

static void SHELL_ShowCpuLoadInfo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount >= 3U) {
        if (AmbaUtility_StringCompare(pArgVector[2], "reset", 5U) == 0) {
            (void)AmbaSYS_DispInfo(SYS_LOG_KAL_CPU_LOAD, SYS_LOG_FUNC_RESET, PrintFunc);
        }
    } else {
        (void)AmbaSYS_DispInfo(SYS_LOG_KAL_CPU_LOAD, SYS_LOG_FUNC_NORMAL, PrintFunc);
    }
}

/**
 *  AmbaShell_CommandProcessStatus - execute command print ps
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandProcessStatus(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount == 1U) {
        SHELL_PsCmdUsage(pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "tsk", 3U) == 0) {
        SHELL_ShowTaskInfo(PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "mtx", 3U) == 0) {
        SHELL_ShowMutexInfo(PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "sem", 3U) == 0) {
        SHELL_ShowSemaphoreInfo(PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "flg", 3U) == 0) {
        SHELL_ShowEventFlagInfo(PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "msg", 3U) == 0) {
        SHELL_ShowMessageQueueInfo(PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "all", 3U) == 0) {
        SHELL_ShowTaskInfo(PrintFunc);
        SHELL_ShowMutexInfo(PrintFunc);
        SHELL_ShowSemaphoreInfo(PrintFunc);
        SHELL_ShowEventFlagInfo(PrintFunc);
        SHELL_ShowMessageQueueInfo(PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "irq", 3U) == 0) {
        PrintFunc("Obsoleted. Please use 'sysinfo irq' instead.\n");
    } else if (AmbaUtility_StringCompare(pArgVector[1], "profile", 7U) == 0) {
        SHELL_ShowCpuLoadInfo(ArgCount, pArgVector, PrintFunc);
    } else {
        SHELL_PsCmdUsage(pArgVector, PrintFunc);
    }
}
