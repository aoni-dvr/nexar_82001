/**
 *  @file AmbaIOUTDiag_CmdWDT.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DWDTECT, INDWDTECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details WDT diagnostic command
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaWrap.h"

#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"

#include "AmbaIOUTDiag.h"
#include "AmbaSYS.h"
#include "AmbaWDT.h"

#define COUNTDOWN_NUM 1000U

/* Task control */
static AMBA_KAL_TASK_t DiagWdtMonitorTask;
static AMBA_KAL_EVENT_FLAG_t DiagWdtEventFlags;

static void IoDiag_WdtCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" start                   : Start WDT, system will reset after 1 sec\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" feed                    : Restart WDT\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" stop                    : Stop WDT\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" info [clr]              : Show/Clear WDT reset info\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" irq [on|off]            : Interrupt mode or system reset mode\n");
}

/**
 *  IoDiag_WdtISR - WDT timeout handler
 *  @param[in] EntryArg Argument
 */
static void IoDiag_WdtISR(UINT32 UserArg)
{
    if (UserArg == 0x47414944UL) {
        (void)AmbaKAL_EventFlagSet(&DiagWdtEventFlags, 0x10UL);
    }
}

/**
 *  IoDiag_WdtMonitorTaskEntry - WDT monitor task entry
 *  @param[in] EntryArg Argument
 */
static void * IoDiag_WdtMonitorTaskEntry(void * EntryArg)
{
    AMBA_WDT_INFO_s WdtInfo;
    UINT32 Loop = 1U;
    UINT32 ActualFlags = 0U;
    UINT32 i = 0U, Interval = 100U;
    UINT32 ChId = 0U;

    (void)AmbaKAL_EventFlagClear(&DiagWdtEventFlags, 0xffffffffUL);

    if (EntryArg != NULL) {
        AmbaMisra_TypeCast(&ChId, &EntryArg);
    } else {
        ChId = 0U;
    }

    AmbaPrint_PrintUInt5("[Diag][WDT] Start countdown %u ms", COUNTDOWN_NUM, 0U, 0U, 0U, 0U);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    (void)AmbaWDT_HookHandler((UINT32)ChId, IoDiag_WdtISR, 0x47414944UL);
    (void)AmbaWDT_StartByChId((UINT32)ChId, COUNTDOWN_NUM, 10U);
#else
    (void)AmbaWDT_Start(COUNTDOWN_NUM, 10U);
#endif

    if (AmbaWDT_GetInfo(&WdtInfo) == WDT_ERR_NONE) {
        if (WdtInfo.ExpireAction == AMBA_WDT_ACT_SYS_RESET) {
            AmbaPrint_PrintUInt5("[Diag][WDT] Watchdog timer reset is scheduled", 0U, 0U, 0U, 0U, 0U);
        } else if (WdtInfo.ExpireAction == AMBA_WDT_ACT_IRQ) {
            AmbaPrint_PrintUInt5("[Diag][WDT] Watchdog timer irq is scheduled", 0U, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_PrintUInt5("[Diag][WDT] Watchdog timer is not correctly enabled", 0U, 0U, 0U, 0U, 0U);
        }
    }

    while(1U == Loop) {
        if (AmbaKAL_EventFlagGet(&DiagWdtEventFlags, 0x11UL, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, Interval) == KAL_ERR_NONE) {
            if ((ActualFlags & 0x1UL) != 0UL) {
                if (AmbaWDT_Feed() == WDT_ERR_NONE) {
                    i = 0U;
                }
            }
            if ((ActualFlags & 0x10UL) != 0UL) {
                AmbaPrint_PrintUInt5("[Diag][WDT] Interrupt occured.", 0U, 0U, 0U, 0U, 0U);
                AmbaSYS_Reboot();
            }
        } else {
            /* Timeout */
            if (i < COUNTDOWN_NUM) {
                i += Interval;
            }
        }

        if (i < COUNTDOWN_NUM) {
            AmbaPrint_PrintUInt5("[Diag][WDT] Elapsed = %u ms. Remaining = %u ms", i, COUNTDOWN_NUM - i, 0U, 0U, 0U);
        }
    }
    return NULL;
}

/**
 *  IoDiag_WdtTaskStart - Start watchdog timer
 *  @param[in] Args Argument
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static UINT32 IoDiag_WdtTaskStart(UINT32 Args, AMBA_SHELL_PRINT_f PrintFunc)
{
    static UINT8 DiagWdtMonitorTaskStack[1024] GNU_SECTION_NOZEROINIT;
    static char DiagWdtTaskName[16] = "DiagWdtMonitor";
    static char DiagWdtEventFlagsName[18] = "DiagWdtEventFlags";
    UINT32 RetVal;
    void * EntryArg;

    (void)AmbaKAL_EventFlagCreate(&DiagWdtEventFlags, DiagWdtEventFlagsName);

    AmbaMisra_TypeCast(&EntryArg,&Args);
    AmbaMisra_TouchUnused(EntryArg);
    RetVal = AmbaKAL_TaskCreate(&DiagWdtMonitorTask,
                                DiagWdtTaskName,
                                AMBA_KAL_TASK_LOWEST_PRIORITY,
                                IoDiag_WdtMonitorTaskEntry,
                                EntryArg,
                                DiagWdtMonitorTaskStack,
                                sizeof(DiagWdtMonitorTaskStack),
                                AMBA_KAL_AUTO_START);

    if (RetVal == OK) {
        PrintFunc("[Diag][WDT] Watchdog timer starts counting.\n");
    }

    return RetVal;
}

/**
 *  IoDiag_WdtTaskFeed - Restart watchdog timer
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static UINT32 IoDiag_WdtTaskFeed(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;

    PrintFunc("[Diag][WDT] Watchdog timer restarts counting.\n");
    RetVal = AmbaKAL_EventFlagSet(&DiagWdtEventFlags, 0x1UL);

    return RetVal;
}

/**
 *  IoDiag_WdtTaskStop - Stop watchdog timer
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return error code
 */
static UINT32 IoDiag_WdtTaskStop(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;

    (void)AmbaWDT_Stop();

    (void)AmbaKAL_TaskTerminate(&DiagWdtMonitorTask);
    RetVal = AmbaKAL_TaskDelete(&DiagWdtMonitorTask);

    if (RetVal == OK) {
        PrintFunc("[Diag][WDT] Watchdog timer stops counting.\n");
        (void)AmbaWrap_memset(&DiagWdtMonitorTask, 0, sizeof(AMBA_KAL_TASK_t));
        (void)AmbaKAL_EventFlagDelete(&DiagWdtEventFlags);
    }

    return RetVal;
}

/**
 *  WDT diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 */
void AmbaIOUTDiag_CmdWDT(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_WDT_INFO_s WdtInfo;
    UINT32 Args;

    if (ArgCount < 2U) {
        IoDiag_WdtCmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "start", 5U) == 0) {
            if (ArgCount >= 2U) {
                if (ArgCount == 2U) {
                    Args = 0U;
                } else {
                    (void)AmbaUtility_StringToUInt32(pArgVector[2], &Args);
                }
                (void)IoDiag_WdtTaskStart(Args, PrintFunc);
            } else {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
                PrintFunc("Usage: ");
                PrintFunc(pArgVector[0]);
                PrintFunc(" start [ChId]    : Start WDT monitor task for Chid channel\n");
#else
                IoDiag_WdtCmdUsage(pArgVector, PrintFunc);
#endif
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "stop", 4U) == 0) {
            (void)IoDiag_WdtTaskStop(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "feed", 4U) == 0) {
            (void)IoDiag_WdtTaskFeed(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "info", 4U) == 0) {
            if (ArgCount >= 3U) {
                if (AmbaUtility_StringCompare(pArgVector[2], "clr", 3U) == 0) {
                    (void)AmbaWDT_ClearStatus();
                }
            } else {
                if (AmbaWDT_GetInfo(&WdtInfo) == WDT_ERR_NONE) {
                    if (WdtInfo.TimerExpired != 0U) {
                        PrintFunc("[Diag][WDT] Watchdog timer reset flag is detected.\n");
                    } else {
                        PrintFunc("[Diag][WDT] Watchdog timer reset flag is not detected.\n");
                    }
                }
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "irq", 3U) == 0) {
            if (ArgCount >= 3U) {
                if (AmbaUtility_StringCompare(pArgVector[2], "on", 2U) == 0) {
                    (void)AmbaWDT_HookTimeOutHandler(IoDiag_WdtISR, 0x47414944UL);
                }
                if (AmbaUtility_StringCompare(pArgVector[2], "off", 3U) == 0) {
                    (void)AmbaWDT_HookTimeOutHandler(NULL, 0U);
                }
            } else {
                PrintFunc("Usage: ");
                PrintFunc(pArgVector[0]);
                PrintFunc(" irq [on|off]            : Interrupt mode or system reset mode\n");
            }
        } else {
            IoDiag_WdtCmdUsage(pArgVector, PrintFunc);
        }
    }
}
