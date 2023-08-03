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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "diag.h"
#include "AmbaTMR.h"

static unsigned int UnitTimer0 = 0, UnitTimer1 = 0;
static unsigned int UnitTimerID = 0xfffff;

void IoDiag_TimerISR0(UINT32 TimerID, UINT32 UserArg)
{
    (void) UserArg;
    UnitTimer0 += 1U;
    printf("%s %d, TimerID(%d) = %d\n", __FUNCTION__, __LINE__, TimerID, UnitTimer0);
}

void IoDiag_TimerISR1(UINT32 TimerID, UINT32 UserArg)
{
    (void) UserArg;
    UnitTimer1 += 1U;

    if (UnitTimer1 > UnitTimer0) {
        printf("%s %d, Timer error", __FUNCTION__, __LINE__);
    } else {
        printf("%s %d, TimerID(%d) = %d\n", __FUNCTION__, __LINE__, TimerID, UnitTimer1);
    }
}

static void IoDiag_TimerCmdUsage(void)
{
    printf("Usage:\n");
    printf("timer <TimerID> <Freq> <StartTick> <Period>\n");
    printf("EX: diag timer 0 1000000 200000 1000000\n");
    printf(" [find_avail]\n");
    printf(" [stop] <TimerID>\n");
    printf(" [release] <TimerID>\n");
    printf(" [unittest]\n");
    printf(" [unittest stop]\n");
    printf(" [unittest show]\n");
}

int DoTimerDiag(int argc, char *argv[])
{
    char **ptr = NULL;
    unsigned int TimerID;
    unsigned int StartTick;
    unsigned int NumPeriodicTick;
    unsigned int Freq;
    AMBA_TMR_INFO_s TimerInfo = {0};
    UINT32 RetVal;

    if (argc >= 3) {
        if (strcmp("unittest", argv[2]) == 0) {
            if (argc > 3) {
                if (strcmp("stop", argv[3]) == 0) {
                    printf("Stop timer %d\n", UnitTimerID);
                    AmbaTMR_Stop(UnitTimerID);
                } else if (strcmp("show", argv[3]) == 0) {
                    printf("%s %d, UnitTimer(%d)\n", __FUNCTION__, __LINE__, UnitTimerID);
                    AmbaTMR_GetInfo(UnitTimerID, &TimerInfo);
                    printf("%s %d, UnitTimer(%d) SysFreq(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.SysFreq);
                    printf("%s %d, UnitTimer(%d) TimerFreq(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.TimerFreq);
                    printf("%s %d, UnitTimer(%d) PeriodicInterval(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.PeriodicInterval);
                    printf("%s %d, UnitTimer(%d) ExpireCount(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.ExpireCount);
                    printf("%s %d, UnitTimer(%d) State(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.State);
                } else {
                    IoDiag_TimerCmdUsage();
                }
            } else {
                UnitTimer0 = 0;
                AmbaTMR_FindAvail(&UnitTimerID, 0);
                AmbaTMR_Acquire(UnitTimerID, 0);
                AmbaTMR_HookTimeOutHandler(UnitTimerID, &IoDiag_TimerISR0, UnitTimerID);
                AmbaTMR_Config(UnitTimerID, 1000000, 1000000);
                AmbaTMR_Start(UnitTimerID, 1000000);
                printf("%s %d, UnitTimer(%d) start\n", __FUNCTION__, __LINE__, UnitTimerID);
                AmbaTMR_GetInfo(UnitTimerID, &TimerInfo);
                printf("%s %d, UnitTimer(%d) SysFreq(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.SysFreq);
                printf("%s %d, UnitTimer(%d) TimerFreq(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.TimerFreq);
                printf("%s %d, UnitTimer(%d) PeriodicInterval(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.PeriodicInterval);
                printf("%s %d, UnitTimer(%d) ExpireCount(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.ExpireCount);
                printf("%s %d, UnitTimer(%d) State(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.State);
                AmbaTMR_FindAvail(&UnitTimerID, 0);
                AmbaTMR_Acquire(UnitTimerID, 0);
                AmbaTMR_HookTimeOutHandler(UnitTimerID, &IoDiag_TimerISR1, UnitTimerID);
                AmbaTMR_Config(UnitTimerID, 2000000, 2000000);
                AmbaTMR_Start(UnitTimerID, 2000000);
                printf("%s %d, UnitTimer(%d) start\n", __FUNCTION__, __LINE__, UnitTimerID);
                AmbaTMR_GetInfo(UnitTimerID, &TimerInfo);
                printf("%s %d, UnitTimer(%d) SysFreq(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.SysFreq);
                printf("%s %d, UnitTimer(%d) TimerFreq(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.TimerFreq);
                printf("%s %d, UnitTimer(%d) PeriodicInterval(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.PeriodicInterval);
                printf("%s %d, UnitTimer(%d) ExpireCount(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.ExpireCount);
                printf("%s %d, UnitTimer(%d) State(%d)\n", __FUNCTION__, __LINE__, UnitTimerID, TimerInfo.State);
                sleep(1000000);
            }
        } else if (strcmp("find_avail", argv[2]) == 0) {
            RetVal = AmbaTMR_FindAvail(&TimerID, 0);
            if (RetVal == ERR_NONE) {
                printf("Available TimerID: %u\n", TimerID);
            } else {
                printf("Couldn't find available timer\n");
            }
        } else if (strcmp("stop", argv[2]) == 0) {
            TimerID = strtoul(argv[3], ptr, 0);

            RetVal = AmbaTMR_Stop(TimerID);
            if (RetVal == ERR_NONE) {
                printf("Timer%u stopped.\n", TimerID);
            } else {
                printf("Couldn't stop Timer%u! (RetVal=0x%08x)\n", TimerID, RetVal);
            }
        } else if (strcmp("release", argv[2]) == 0) {
            TimerID = strtoul(argv[3], ptr, 0);

            RetVal = AmbaTMR_Release(TimerID);
            if (RetVal == ERR_NONE) {
                printf("Timer%u released.\n", TimerID);
            } else {
                printf("Couldn't release Timer%u! (RetVal=0x%08x)\n", TimerID, RetVal);
            }
        } else if (argc >= 6) {
            TimerID = strtoul(argv[2], ptr, 0);
            Freq = strtoul(argv[3], ptr, 0);
            StartTick = strtoul(argv[4], ptr, 0);
            NumPeriodicTick = strtoul(argv[5], ptr, 0);
            AmbaTMR_Acquire(TimerID, 0);
            AmbaTMR_HookTimeOutHandler(TimerID, &IoDiag_TimerISR0, TimerID);
            AmbaTMR_Config(TimerID, Freq, NumPeriodicTick);
            AmbaTMR_Start(TimerID, StartTick);
            sleep(1000000);
        } else {
            IoDiag_TimerCmdUsage();
        }
    } else {
        IoDiag_TimerCmdUsage();
    }
    return 0;
}

