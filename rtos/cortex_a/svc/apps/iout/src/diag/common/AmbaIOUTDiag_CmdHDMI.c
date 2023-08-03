/**
 *  @file AmbaIOUTDiag_CmdHDMI.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details HDMI diagnostic command
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaPrint.h"

#include "AmbaIOUTDiag.h"
#include "AmbaVOUT.h"
#include "AmbaHDMI.h"
#include "AmbaCSL_HDMI.h"

static void IoDiag_HdmiCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [mode|info] <...>\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" mode <VIC> [<PixelFormat>]      : hdmi output with specific VIC\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" info                            : show current hdmi output information\n");
}

static void IoDiag_HdmiSetMode(UINT32 Vic, UINT32 PixelFormat, UINT32 ColorDepth)
{
    UINT32 HdmiPort = AMBA_HDMI_TX_PORT0;
    AMBA_HDMI_VIDEO_CONFIG_s VideoConfig = {0};
    AMBA_HDMI_MODE_INFO_s HdmiInfo = {0};

    AmbaPrint_PrintUInt5("[Diag][HDMI] HdmiPort = %d, VideoIdCode = %d, PixelFormat = %d, ColorDepth = %d", HdmiPort, Vic, PixelFormat, ColorDepth, 0U);

    VideoConfig.VideoIdCode = (UINT8)Vic;
    VideoConfig.PixelFormat = (UINT8)PixelFormat;
    VideoConfig.ColorDepth = (UINT8)ColorDepth;
    VideoConfig.QuantRange = AMBA_HDMI_QRANGE_DEFAULT;

    (void)AmbaHDMI_TxEnable(HdmiPort);
    (void)AmbaHDMI_TxSetMode(HdmiPort, &VideoConfig, NULL);

    (void)AmbaHDMI_TxGetCurrModeInfo(HdmiPort, &HdmiInfo);
    AmbaPrint_PrintUInt5("[Diag][HDMI] Pixel Clock %u", HdmiInfo.VideoConfig.PixelClkFreq, 0U, 0U, 0U, 0U);

    /* Debug mode using internal generated sync signals and video data.*/
    AmbaHDMI_TxEnableTestPattern(HdmiPort);

    (void)AmbaHDMI_TxStart(HdmiPort);
    AmbaPrint_PrintUInt5("[Diag][HDMI] HDMI starts", 0U, 0U, 0U, 0U, 0U);
}

static void IoDiag_HdmiSinkInfo(void)
{
    AMBA_HDMI_SINK_INFO_s SinkInfo = {0};
    UINT32 HpdState = 0U, EdidDetect = 0U;

    (void)AmbaHDMI_TxGetSinkInfo(AMBA_HDMI_TX_PORT0, &HpdState, &EdidDetect, &SinkInfo);
    AmbaPrint_PrintUInt5("[Diag][HDMI] HpdState = %X, EdidDetect = %X", HpdState, EdidDetect, 0U, 0U, 0U);
    if (EdidDetect != AMBA_HDMI_EDID_UNAVAILABLE) {
        AmbaPrint_PrintUInt5("[Diag][HDMI] DeepColorCapability = %X, ColorFormatCapability = %X", SinkInfo.DeepColorCapability, SinkInfo.ColorFormatCapability, 0U, 0U, 0U);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIOUTDiag_CmdHDMI
 *
 *  @Description:: HDMI diagnostic command
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIOUTDiag_CmdHDMI(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Vic, PixelFormat = AMBA_HDMI_PIXEL_FORMAT_RGB_444, ColorDepth = AMBA_HDMI_COLOR_DEPTH_24BPP;

    if (ArgCount < 2U) {
        IoDiag_HdmiCmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "mode", 4U) == 0) {
            if (ArgCount >= 3U) {
                (void)AmbaUtility_StringToUInt32(pArgVector[2], &Vic);
                if (ArgCount >= 4U) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[3], &PixelFormat);
                }
                IoDiag_HdmiSetMode(Vic, PixelFormat, ColorDepth);
            } else {
                PrintFunc("[Diag][HDMI] Hint: mode <VIC> [<PixelFormat>]\n");
            }
        }
        if (AmbaUtility_StringCompare(pArgVector[1], "info", 4) == 0) {
            IoDiag_HdmiSinkInfo();
        }
    }
}
