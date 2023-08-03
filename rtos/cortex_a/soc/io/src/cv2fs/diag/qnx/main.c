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
#include "diag.h"

static void supported_commands_print(void)
{
    printf("diag-ambarella [i2c|gpio|timer|rng|rtc|otp|spi|mprint|dma|gdma|crc|pwm]\n");
}

static int parse_options(int argc, char *argv[])
{
    if (argv == NULL)
        return 0;

    if (argc >= 2) {

        if (strncmp(argv[1], "i2c", 3U) == 0) {
            DoI2CDiag(argc, argv);
        }

        if (strncmp(argv[1], "clock", 5U) == 0) {
            DoClockDiag(argc, argv);
        }

        if (strncmp(argv[1], "gpio", 4U) == 0) {
            DoGpioDiag(argc, argv);
        }

        if (strncmp(argv[1], "timer", 5U) == 0) {
            DoTimerDiag(argc, argv);
        }

        if (strncmp(argv[1], "rng", 3U) == 0) {
            DoRngDiag(argc, argv);
        }

        if (strncmp(argv[1], "rtc", 3U) == 0) {
            DoRtcDiag(argc, argv);
        }

        if (strncmp(argv[1], "otp", 3U) == 0) {
            DoOtpDiag(argc, argv);
        }

        if (strncmp(argv[1], "spi", 3U) == 0) {
            DoSPIDiag(argc, argv);
        }

        if (strncmp(argv[1], "mprint", 6U) == 0) {
            DoMPrintDiag(argc, argv);
        }

        if (strncmp(argv[1], "pwm", 3U) == 0) {
            DoPwmDiag(argc, argv);
        }

        if (strncmp(argv[1], "dma", 3U) == 0) {
            DoDmaDiag(argc, argv);
        }

        if (strncmp(argv[1], "gdma", 4U) == 0) {
            DoGdmaDiag(argc, argv);
        }

        if (strncmp(argv[1], "wdt", 3U) == 0) {
            DoWdtDiag(argc, argv);
        }

        if (strncmp(argv[1], "crc", 3U) == 0) {
            DoCrcDiag(argc, argv);
        }

        if (strncmp(argv[1], "nvm", 3U) == 0) {
            DoNvmDiag(argc, argv);
        }

        if (strncmp(argv[1], "can", 3U) == 0) {
            DoCanDiag(argc, argv);
        }
    } else {
        supported_commands_print();
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int i = 0;

    for (i = 0; i < argc; i++) {
        printf("diag argv[%d] : %s\n", i, argv[i]);
    }

    parse_options(argc, argv);

    return 0;
}
