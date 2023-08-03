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
#include "AmbaADC.h"

static void IoDiag_AdcCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf(pArgVector[0]);
    printf(" unittest                : Sanity testing\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" read <channel>          : Get adc value of the spceified channel\n");
    printf("       ");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_AdcUnitTest
 *
 *  @Description:: Test all functionality
 *
 *  @Input      :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IoDiag_AdcUnitTest(void)
{
    UINT32 i, Data = 0;
    UINT32 RetVal;

    for (i = 0; i < AMBA_NUM_ADC_CHANNEL; i++) {
        RetVal = AmbaADC_SingleRead(i, &Data);
        if (RetVal == 0)printf("Channel %d, Val = %d", i, Data);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_AdcGetValue
 *
 *  @Description:: Test AmbaADC_GetData
 *
 *  @Input      ::
 *      PrintFunc: function to print messages on shell task
 *      AdcChanNo: channel number
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IoDiag_AdcGetValue(UINT32 AdcChanNo)
{
    UINT32 Data = 0;
    UINT32 RetVal;

    if (AdcChanNo < AMBA_NUM_ADC_CHANNEL) {
        RetVal = AmbaADC_SingleRead(AdcChanNo, &Data);
        if (RetVal == 0)printf("Channel %d, Val = %d", AdcChanNo, Data);
    } else {
        printf("No such ADC channel!");
    }
}



int DoAdcDiag(int argc, char *argv[])
{
    char **ptr = NULL;
    UINT32 ChanId;

    if (argc < 3) {
        IoDiag_AdcCmdUsage(argv);
    } else {
        if (strcmp("unittest", argv[2]) == 0) {
            IoDiag_AdcUnitTest();
        }

        if (strcmp("read", argv[2]) == 0) {
            if (argc >= 4) {
                ChanId = strtoul(argv[3], ptr, 0);
                IoDiag_AdcGetValue(ChanId);
            } else {
                fprintf(stderr, "%s %d, Channel number is not specified!\n", __FUNCTION__, __LINE__);
            }
        }
    }

    return 0;
}

