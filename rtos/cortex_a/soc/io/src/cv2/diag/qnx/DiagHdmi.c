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
#include <stdlib.h>
#include "diag.h"

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaHDMI.h"

static void IoDiag_HdmiCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf(pArgVector[0]);
    printf(" [mode|info] <...>\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" mode <VIC> [<PixelFormat>]      : hdmi output with specific VIC\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" info                            : show current hdmi output information\n");
}

static void IoDiag_HdmiSetMode(UINT32 Vic, UINT32 PixelFormat, UINT32 ColorDepth)
{
    UINT32 HdmiPort = AMBA_HDMI_TX_PORT0;
    AMBA_HDMI_VIDEO_CONFIG_s VideoConfig = {0};
    AMBA_HDMI_MODE_INFO_s HdmiInfo = {0};

    printf("[Diag][HDMI] HdmiPort = %d, VideoIdCode = %d, PixelFormat = %d, ColorDepth = %d\n", HdmiPort, Vic, PixelFormat, ColorDepth);

    VideoConfig.VideoIdCode = (UINT8)Vic;
    VideoConfig.PixelFormat = (UINT8)PixelFormat;
    VideoConfig.ColorDepth = (UINT8)ColorDepth;
    VideoConfig.QuantRange = AMBA_HDMI_QRANGE_DEFAULT;

    (void)AmbaHDMI_TxEnable(HdmiPort);
    (void)AmbaHDMI_TxSetMode(HdmiPort, &VideoConfig, NULL);

    (void)AmbaHDMI_TxGetCurrModeInfo(HdmiPort, &HdmiInfo);
    printf("[Diag][HDMI] Pixel Clock %u\n", HdmiInfo.VideoConfig.PixelClkFreq);

    /* 0x1 - Debug mode using internal generated sync signals and video data.*/
    (void)AmbaHDMI_TxEnableTestPattern(HdmiPort);

    (void)AmbaHDMI_TxStart(HdmiPort);
    printf("[Diag][HDMI] HDMI starts\n");
}

static void IoDiag_HdmiSinkInfo(void)
{
    AMBA_HDMI_SINK_INFO_s SinkInfo = {0};
    UINT32 HpdState = 0, EdidDetect = 0;

    (void)AmbaHDMI_TxGetSinkInfo(AMBA_HDMI_TX_PORT0, &HpdState, &EdidDetect, &SinkInfo);
    printf("[Diag][HDMI] HpdState = %X, EdidDetect = %X", HpdState, EdidDetect);
    if (EdidDetect != AMBA_HDMI_EDID_UNAVAILABLE) {
        printf("[Diag][HDMI] DeepColorCapability = %X, ColorFormatCapability = %X", SinkInfo.DeepColorCapability, SinkInfo.ColorFormatCapability);
    }
}

int DoHdmiDiag(int argc, char *argv[])
{
    UINT32 Vic, PixelFormat = AMBA_HDMI_PIXEL_FORMAT_RGB_444, ColorDepth = AMBA_HDMI_COLOR_DEPTH_24BPP;
    char **ptr = NULL;

    if (argc >= 3) {
        if (strncmp(argv[2], "mode", 4U) == 0) {
            if (argc >= 4) {
                Vic = strtoul(argv[3], ptr, 0);
                printf("[Diag][HDMI] VideoIdCode = %d\n", Vic);

                if (argc >= 5) {
                    PixelFormat = strtoul(argv[4], ptr, 0);
                    printf("[Diag][HDMI] PixelFormat = %d\n", PixelFormat);
                }
                IoDiag_HdmiSetMode(Vic, PixelFormat, ColorDepth);
            } else {
                printf("[Diag][HDMI] Hint: mode <VIC> [<PixelFormat>]\n");
            }
        } else if (strncmp(argv[2], "info", 4U) == 0) {
            IoDiag_HdmiSinkInfo();
        } else {
            IoDiag_HdmiCmdUsage(&argv[1]);
        }
    } else {
        IoDiag_HdmiCmdUsage(&argv[1]);
    }
    return 0;
}

