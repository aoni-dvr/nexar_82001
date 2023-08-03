/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <stdio.h>
#include <string.h>

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"

#include "diag.h"
#include "AmbaSYS.h"
#include "AmbaWDT.h"

/* Task control */
static AMBA_KAL_TASK_t DiagWdtMonitorTask;
static AMBA_KAL_EVENT_FLAG_t DiagWdtEventFlags;

static void IoDiag_WdtCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf(pArgVector[1]);
    printf(" start                   : Start WDT, system will reset after 1 sec\n");
    printf("       ");
    printf(pArgVector[1]);
    printf(" feed                    : Restart WDT\n");
    printf("       ");
    printf(pArgVector[1]);
    printf(" stop                    : Stop WDT\n");
    printf("       ");
    printf(pArgVector[1]);
    printf(" info [clr]              : Show/Clear WDT reset info\n");
    printf("       ");
    printf(pArgVector[1]);
    printf(" irq [on|off]            : Interrupt mode or system reset mode\n");
}

/**
 *  IoDiag_WdtISR - WDT timeout handler
 *  @param[in] EntryArg Argument
 */
static void IoDiag_WdtISR(UINT32 UserArg)
{
    if (UserArg == 0x47414944UL) {
#if defined(CONFIG_SOC_CV2)
        (void)AmbaKAL_EventFlagSet(&DiagWdtEventFlags, 0x10UL);
#else
        AmbaWDT_SetPattern();
        AmbaSYS_Reboot();
#endif
    }
}

/**
 *  IoDiag_WdtTaskFeed - Restart watchdog timer
 *  @return error code
 */
static UINT32 IoDiag_WdtTaskFeed(void)
{
    UINT32 RetVal;

    printf("[Diag][WDT] Watchdog timer restarts counting.\n");
    RetVal = AmbaKAL_EventFlagSet(&DiagWdtEventFlags, 0x1UL);

    return RetVal;
}

/**
 *  IoDiag_WdtTaskStop - Stop watchdog timer
 *  @return error code
 */
static UINT32 IoDiag_WdtTaskStop(void)
{
    UINT32 RetVal;

    (void)AmbaWDT_Stop();

    (void)AmbaKAL_TaskTerminate(&DiagWdtMonitorTask);
    RetVal = AmbaKAL_TaskDelete(&DiagWdtMonitorTask);

    if (RetVal == OK) {
        printf("[Diag][WDT] Watchdog timer stops counting.\n");
        (void)memset(&DiagWdtMonitorTask, 0, sizeof(AMBA_KAL_TASK_t));
        (void)AmbaKAL_EventFlagDelete(&DiagWdtEventFlags);
    }

    return RetVal;
}

int DoWdtDiag(int argc, char *argv[])
{
    AMBA_WDT_INFO_s WdtInfo;
    UINT32 WakeInfo = 0;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    char **ptr = NULL;
#endif

    if (argc < 3) {
        IoDiag_WdtCmdUsage(argv);
    } else {
        if (strcmp(argv[2], "start") == 0) {
#if defined(CONFIG_SOC_CV2)
            printf("[Diag][WDT] Start countdown 1000 ms");
            (void)AmbaWDT_Start((UINT32)1000, 10U);
#else
            printf("[Diag][WDT] Start countdown 1000 ms\n");
            (void)AmbaWDT_HookHandler(0U, IoDiag_WdtISR, 0x47414944UL);
            (void)AmbaWDT_StartByChId(0U, (UINT32)1000, 10U);
            sleep(1000000);
#endif
        } else if (strcmp(argv[2], "stop") == 0) {
            (void)IoDiag_WdtTaskStop();
        } else if (strcmp(argv[2], "feed") == 0) {
            (void)IoDiag_WdtTaskFeed();
        } else if (strcmp(argv[2], "info") == 0) {
            if (argc >= 4) {
                if (strcmp(argv[3], "clr") == 0) {
                    (void)AmbaWDT_ClearStatus();
                }
            } else {
                if (AmbaWDT_GetInfo(&WdtInfo) == WDT_ERR_NONE) {
                    if (WdtInfo.TimerExpired != 0U) {
                        printf("[Diag][WDT] Watchdog timer reset flag is detected.\n");
                    } else {
                        printf("[Diag][WDT] Watchdog timer reset flag is not detected.\n");
                    }
                }

                if (AmbaSYS_GetWakeUpInfo(&WakeInfo) == SYS_ERR_NONE) {
                    if (WakeInfo != 0x0) {
                        printf("[Diag][WDT] WakeInfo value %d\n", WakeInfo);
                    }
                }
            }
        } else if (strcmp(argv[2], "irq") == 0) {
            if (argc >= 4) {
                if (strcmp(argv[3], "on") == 0) {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
                    printf("[Diag][WDT] Start countdown 1000 ms\n");
                    if (argc >= 5) {
                        (void)AmbaWDT_HookHandler(strtoul(argv[4], ptr, 0), IoDiag_WdtISR, 0x47414944UL);
                        (void)AmbaWDT_StartByChId(strtoul(argv[4], ptr, 0), (UINT32)1000, 10U);
                    } else {
                        (void)AmbaWDT_HookHandler(0U, IoDiag_WdtISR, 0x47414944UL);
                        (void)AmbaWDT_StartByChId(0U, (UINT32)1000, 10U);
                    }
                    sleep(1000000);
#else
                    (void)AmbaWDT_HookTimeOutHandler(IoDiag_WdtISR, 0x47414944UL);
#endif
                }
                if (strcmp(argv[3], "off") == 0) {
                    (void)AmbaWDT_HookTimeOutHandler(NULL, 0U);
                }
            } else {
                printf("Usage: ");
                printf(argv[1]);
                printf(" irq [on|off]            : Interrupt mode or system reset mode\n");
            }
        } else {
            IoDiag_WdtCmdUsage(argv);
        }
    }
    return 0;
}


