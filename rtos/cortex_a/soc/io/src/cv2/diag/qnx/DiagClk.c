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
#include "diag.h"
#include "AmbaSYS.h"

static void IoDiag_PllCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf(pArgVector[0]);
    printf(" [info|show] <...>\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" info                            : show current system clock settings\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" show <clk_namd>                 : show specified clock setting\n");
    printf("clk_namd: core, idsp, cortex, dram, dsp_sys, vision, fex, audio0, vin0, vin1, vout0, vout1, sd\n");
}

#define AMBA_NUM_PLL 12
static void IoDiag_PllShowAll(void)
{
    const char *ClockName[AMBA_NUM_PLL] = {
        "AMBA_PLL_AUDIO",
        "AMBA_PLL_CORE",
        "AMBA_PLL_CORTEX",
        "AMBA_PLL_DDR",
        "AMBA_PLL_FEX",
        "AMBA_PLL_HDMI",
        "AMBA_PLL_IDSP",
        "AMBA_PLL_SD",
        "AMBA_PLL_SENSOR0",
        "AMBA_PLL_SENSOR2",
        "AMBA_PLL_VIDEO2",
        "AMBA_PLL_VISION"
    };

    UINT32 ClockSysId[AMBA_NUM_PLL] = {
        AMBA_SYS_CLK_AUD_0,
        AMBA_SYS_CLK_CORE,
        AMBA_SYS_CLK_CORTEX,
        AMBA_SYS_CLK_DRAM,
        AMBA_SYS_CLK_FEX,
        AMBA_SYS_CLK_VID_OUT1,
        AMBA_SYS_CLK_IDSP,
        AMBA_SYS_CLK_SD,
        AMBA_SYS_CLK_REF_OUT0,
        AMBA_SYS_CLK_REF_OUT1,
        AMBA_SYS_CLK_VID_OUT0,
        AMBA_SYS_CLK_VISION
    };

    UINT32 CLockId = 0U;
    UINT32 Freq = 0U;

    printf(":::::::::::::::::::::::::::::::::::::::::::::::::::\n");

    for (CLockId = 0; CLockId < (AMBA_NUM_PLL); CLockId++) {
        AmbaSYS_GetClkFreq(ClockSysId[CLockId], &Freq);
        printf("Clock:%s actual frequency %uHz\n", ClockName[CLockId], Freq);
    }

    printf(":::::::::::::::::::::::::::::::::::::::::::::::::::\n");
}

static void IoDiag_PllShowDetails(UINT32 PllIdx)
{
    const char *ClockName[AMBA_SYS_CLK_SD + 1U] = {
        "AMBA_PLL_CORE",
        "AMBA_PLL_CORTEX",
        "AMBA_PLL_DDR",
        "AMBA_PLL_IDSP",
        "AMBA_PLL_VISION"
        "AMBA_PLL_FEX",
        "AMBA_PLL_DSP_SYS",
        "AMBA_PLL_SD",
    };

    UINT32 Freq;

    AmbaSYS_GetClkFreq(PllIdx, &Freq);
    if (PllIdx <= AMBA_SYS_CLK_SD) {
        printf("Clock:%s actual frequency %uHz\n", ClockName[PllIdx], Freq);
    } else {
        switch (PllIdx) {
        case AMBA_SYS_CLK_AUD_0:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_AUD_0", Freq);
        case AMBA_SYS_CLK_SD:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_SD", Freq);
        case AMBA_SYS_CLK_VID_IN0:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_VIN0", Freq);
        case AMBA_SYS_CLK_VID_IN1:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_VIN1", Freq);
        case AMBA_SYS_CLK_VID_OUT0:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_VOUT0", Freq);
        case AMBA_SYS_CLK_VID_OUT1:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_VOUT1", Freq);
        default:
            printf("[Diag][CLK] invalid clock name\n");
        }
    }
}

static void IoDiag_PllSetClk(UINT32 PllIdx, UINT32 TargetFreq)
{
    const char *ClockName[AMBA_SYS_CLK_SD + 1U] = {
        "AMBA_PLL_CORE",
        "AMBA_PLL_CORTEX",
        "AMBA_PLL_DDR",
        "AMBA_PLL_IDSP",
        "AMBA_PLL_VISION"
        "AMBA_PLL_FEX",
        "AMBA_PLL_DSP_SYS",
        "AMBA_PLL_SD",
    };

    UINT32 Freq = 0U;

    AmbaSYS_SetClkFreq(PllIdx, TargetFreq, &Freq);

    if (PllIdx <= AMBA_SYS_CLK_SD) {
        printf("Clock:%s actual frequency %uHz\n", ClockName[PllIdx], Freq);
    } else {
        switch (PllIdx) {
        case AMBA_SYS_CLK_AUD_0:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_AUD_0", Freq);
        case AMBA_SYS_CLK_SD:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_SD", Freq);
        case AMBA_SYS_CLK_VID_IN0:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_VIN0", Freq);
        case AMBA_SYS_CLK_VID_IN1:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_VIN1", Freq);
        case AMBA_SYS_CLK_VID_OUT0:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_VOUT0", Freq);
        case AMBA_SYS_CLK_VID_OUT1:
            printf("Clock:%s actual frequency %uHz\n", "AMBA_SYS_CLK_VOUT1", Freq);
        default:
            printf("[Diag][CLK] invalid clock name\n");
        }
    }
}

static UINT32 GetClkId(char *argv)
{
    UINT32 RetVal = AMBA_CLK_NUM;

    if (strncmp(argv, "core", 4U) == 0) {
        RetVal = AMBA_SYS_CLK_CORE;
    } else if (strncmp(argv, "idsp", 4U) == 0) {
        RetVal = AMBA_SYS_CLK_IDSP;
    } else if (strncmp(argv, "cortex", 6U) == 0) {
        RetVal = AMBA_SYS_CLK_CORTEX;
    } else if (strncmp(argv, "dram", 4U) == 0) {
        RetVal = AMBA_SYS_CLK_DRAM;
    } else if (strncmp(argv, "dsp_sys", 7U) == 0) {
        RetVal = AMBA_SYS_CLK_DSP_SYS;
    } else if (strncmp(argv, "vision", 6U) == 0) {
        RetVal = AMBA_SYS_CLK_VISION;
    } else if (strncmp(argv, "fex", 3U) == 0) {
        RetVal = AMBA_SYS_CLK_FEX;
    } else if (strncmp(argv, "audio0", 6U) == 0) {
        RetVal = AMBA_SYS_CLK_AUD_0;
    } else if (strncmp(argv, "vin0", 4U) == 0) {
        RetVal = AMBA_SYS_CLK_VID_IN0;
    } else if (strncmp(argv, "vin1", 4U) == 0) {
        RetVal = AMBA_SYS_CLK_VID_IN1;
    } else if (strncmp(argv, "vout0", 5U) == 0) {
        RetVal = AMBA_SYS_CLK_VID_OUT0;
    } else if (strncmp(argv, "vout1", 5U) == 0) {
        RetVal = AMBA_SYS_CLK_VID_OUT1;
    } else if (strncmp(argv, "sd", 2U) == 0) {
        RetVal = AMBA_SYS_CLK_SD;
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

int DoClockDiag(int argc, char *argv[])
{
    UINT32 PllIdx = AMBA_NUM_PLL;
    UINT32 TargetFreq = 0U;
    char **ptr = NULL;

    if (argc < 3U) {
        IoDiag_PllCmdUsage(argv);
    } else {
        if (strncmp(argv[2], "info", 4U) == 0) {
            IoDiag_PllShowAll();
        }
        if (strncmp(argv[2], "show", 4U) == 0) {
            if (argc >= 4U) {
                PllIdx = GetClkId(argv[3]);
                if (PllIdx != SYS_ERR_ARG) {
                    IoDiag_PllShowDetails(PllIdx);
                } else {
                    printf("[Diag][CLK] invalid clock name\n");
                }
            } else {
                printf("[Diag][CLK] Hint: show <pll_name>\n");
            }
        } else if (strncmp(argv[2], "set", 4U) == 0) {
            if (argc >= 4U) {
                PllIdx = GetClkId(argv[3]);
                TargetFreq = strtoul(argv[4], ptr, 0);
                if (PllIdx != SYS_ERR_ARG) {
                    IoDiag_PllSetClk(PllIdx, TargetFreq);
                } else {
                    printf("[Diag][CLK] invalid clock name\n");
                }
            } else {
                printf("[Diag][CLK] Hint: set <pll_name> <target clock value>\n");
            }
        }
    }
    return 0;
}

