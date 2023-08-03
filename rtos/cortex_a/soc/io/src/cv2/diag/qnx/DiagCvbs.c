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

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include <stdio.h>
#include <string.h>
#include "diag.h"
#include "AmbaCVBS.h"

static void IoDiag_CvbsCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf(pArgVector[0]);
    printf(" on [ntsc|pal]\n");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_CvbsDisplayOn
 *
 *  @Description:: Setup and output CVBS signal
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      printf:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void IoDiag_CvbsDisplayOn(char * const *pArgVector)
{
    if (strcmp(pArgVector[3], "ntsc") == 0) {
        printf("cvbs ntsc on\n");
        (void)AmbaCVBS_SetMode(AMBA_CVBS_NTSC_COLORBAR);
    }
    if (strcmp(pArgVector[3], "pal") == 0) {
        printf("cvbs pal on\n");
        (void)AmbaCVBS_SetMode(AMBA_CVBS_PAL_COLORBAR);
    }
}

int DoCvbsDiag(int argc, char *argv[])
{
    if (argc < 2) {
        IoDiag_CvbsCmdUsage(argv);
    } else {
        if (strcmp(argv[2], "on") == 0) {
            if (argc >= 3) {
                IoDiag_CvbsDisplayOn(argv);
            } else {
                printf("[Diag][CVBS] Hint: on [ntsc|pal]\n");
            }
        }
    }

    return 0;
}
