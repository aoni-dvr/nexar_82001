/*
 * $QNXLicenseC:
 * Copyright 2020, QNX Software Systems.
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
#include "diag.h"
#include "AmbaPWM.h"

static void IoDiag_PwmCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf(pArgVector[0]);
    printf("      <Channel> <Freq> <Period> <Duty>\n");
    printf("      Example: pwm 0 10000 100 50\n");
}

static void IoDiag_PwmShowInfo(UINT32 PinPairID, UINT32 Period, UINT32 Duty)
{
    UINT32 ActualFreq;

    if (Period > 0U) {
        if (AmbaPWM_GetInfo(PinPairID, &ActualFreq) == OK) {
            printf("[Diag][PWM][%d]: ", PinPairID);
            printf("Frequency = ");

            printf("%d\n", ActualFreq / Period);

            printf("[Diag][PWM][%d]: ", PinPairID);
            printf("Duty Cycle = ");
            printf("%d%%\n", (Duty * 100U) / Period);
        }
    }
}

int DoPwmDiag(int argc, char *argv[])
{
    char **ptr = NULL;
    UINT32 PinPairID, Freq, Period, Duty;
    UINT32 RetVal;

    if (argc < 6) {
        IoDiag_PwmCmdUsage(argv);
    } else {
        PinPairID = strtoul(argv[2], ptr, 0);
        Freq = strtoul(argv[3], ptr, 0);
        Period = strtoul(argv[4], ptr, 0);
        Duty = strtoul(argv[5], ptr, 0);

        if (PinPairID >= AMBA_NUM_PWM_CHANNEL) {
            printf("[Diag][PWM] ERROR:Invalid Channel\n");
            RetVal = ERR_ARG;
        } else {
            RetVal = ERR_NONE;
        }

        if (RetVal == (UINT32)ERR_NONE) {
            if (Freq == 0U) {
                printf("[Diag][PWM] ERROR:Invalid Frequency\n");
                RetVal = ERR_ARG;
            } else {
                RetVal = ERR_NONE;
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            if (Period == 0U) {
                printf("[Diag][PWM] ERROR:Invalid Period\n");
                RetVal = ERR_ARG;
            } else {
                RetVal = ERR_NONE;
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            if (Duty > Period) {
                printf("[Diag][PWM] ERROR:Duty is greater than Period\n");
                RetVal = ERR_ARG;
            } else {
                RetVal = ERR_NONE;
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Config(PinPairID, Freq * Period);

            if (RetVal != (UINT32)ERR_NONE) {
                printf("[Diag][PWM] ERROR:AmbaPWM_Config failed\n");
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Start(PinPairID, Period, Duty);

            if (RetVal != (UINT32)ERR_NONE) {
                printf("[Diag][PWM] ERROR:AmbaPWM_Start failed\n");
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            IoDiag_PwmShowInfo(PinPairID, Period, Duty);
        }
    }


    return 0;
}

