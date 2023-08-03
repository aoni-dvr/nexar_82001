/**
 *  @file AmbaIOUTDiag_CmdVOUT.c
 *
 *  @copyright Copyright (c) 2022 Ambarella, Inc.
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
 *  @details VOUT diagnostic command
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaWrap.h"

#include "AmbaIOUTDiag.h"
#include "AmbaVOUT.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define IODIAG_VOUT_CSI_1080P60         0U
#define IODIAG_VOUT_CSI_4KP30           1U
#define IODIAG_VOUT_CSI_MAX_MODE_NUM    2U

#define IODIAG_VOUT_DSI_1080P60         0U
#define IODIAG_VOUT_DSI_4KP30           1U
#define IODIAG_VOUT_DSI_MAX_MODE_NUM    2U

static AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTimingCsi[IODIAG_VOUT_CSI_MAX_MODE_NUM] = {
    [IODIAG_VOUT_CSI_1080P60] = {
        .PixelClkFreq       = 147032820,
        .DisplayMethod      = 0,
        .HsyncFrontPorch    = 16,
        .HsyncPulseWidth    = 1948,
        .HsyncBackPorch     = 236,
        .VsyncFrontPorch    = 16,
        .VsyncPulseWidth    = 1095,
        .VsyncBackPorch     = 4,
        .ActivePixels       = 1920,
        .ActiveLines        = 1080,
    },
    [IODIAG_VOUT_CSI_4KP30] = {
        .PixelClkFreq       = 296703297,
        .DisplayMethod      = 0,
        .HsyncFrontPorch    = 200,
        .HsyncPulseWidth    = 3900,
        .HsyncBackPorch     = 300,
        .VsyncFrontPorch    = 50,
        .VsyncPulseWidth    = 2180,
        .VsyncBackPorch     = 20,
        .ActivePixels       = 3840,
        .ActiveLines        = 2160,
    },
};

static AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTimingDsi[IODIAG_VOUT_DSI_MAX_MODE_NUM] = {
    [IODIAG_VOUT_DSI_1080P60] = {
        .PixelClkFreq       = 148351648,
        .DisplayMethod      = 0,
        .HsyncFrontPorch    = 161,
        .HsyncPulseWidth    = 55,
        .HsyncBackPorch     = 64,
        .VsyncFrontPorch    = 30,
        .VsyncPulseWidth    = 8,
        .VsyncBackPorch     = 7,
        .ActivePixels       = 1920,
        .ActiveLines        = 1080,
    },
    [IODIAG_VOUT_DSI_4KP30] = {
        .PixelClkFreq       = 296703297,
        .DisplayMethod      = 0,
        .HsyncFrontPorch    = 318,
        .HsyncPulseWidth    = 117,
        .HsyncBackPorch     = 125,
        .VsyncFrontPorch    = 64,
        .VsyncPulseWidth    = 13,
        .VsyncBackPorch     = 13,
        .ActivePixels       = 3840,
        .ActiveLines        = 2160,
    },
};

static AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s BlankPacket[IODIAG_VOUT_DSI_MAX_MODE_NUM] = {
    [IODIAG_VOUT_DSI_1080P60] = {
        .LenHBP     = 182,
        .LenHSA     = 152,
        .LenHFP     = 474,
        .LenBLLP    = 0,
    },
    [IODIAG_VOUT_DSI_4KP30] = {
        .LenHBP     = 366,
        .LenHSA     = 338,
        .LenHFP     = 944,
        .LenBLLP    = 0,
    },
};

//CSC_TYPE_IDENTITY
static AMBA_VOUT_CSC_MATRIX_s CscIdentity = {
    .Coef = {
        [0] = { 1.00000f, 0.00000f, 0.00000f },
        [1] = { 0.00000f, 1.00000f, 0.00000f },
        [2] = { 0.00000f, 0.00000f, 1.00000f },
    },
    .Offset = { [0] = 0.00000f, [1] = 0.00000f, [2] = 0.00000f },
    .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
    .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
};

//709 YCC(Full) to RGB(Full)
static AMBA_VOUT_CSC_MATRIX_s CscBt709YccFull2GbrFull = {
    .Coef = {
        [0] = {  1.00000f, -0.18711f, -0.46908f },
        [1] = {  1.00000f,  1.85788f,  0.00000f },
        [2] = {  1.00000f,  0.00000f,  1.57503f },
    },
    .Offset = { [0] = 83.99f,   [1] = -237.81f, [2] = -201.60f },
    .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
    .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
};


static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void IoDiag_VoutCsiOutput(UINT32 VoutID, UINT32 ModeSelect, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 OutputMode = VOUT_MIPI_CSI_MODE_RAW8_4LANE;
    UINT32 ColorOrder = 0U;

    if (ModeSelect <= IODIAG_VOUT_CSI_MAX_MODE_NUM) {
        (void)AmbaVout_ChannelSelect(VoutID);

        /* MIPI CLK Continuous mode */
        AmbaVout_MipiSetDphyContClk(&OutputMode);

        (void)AmbaVout_MipiCsiEnable(OutputMode, ColorOrder, &DisplayTimingCsi[ModeSelect]);

        (void)AmbaVout_MipiCsiSetCscMatrix(&CscIdentity, 0);

        (void)AmbaDiag_VoutMipiTestPattern();

        (void)AmbaDiag_VoutConfigThrDbgPort(VoutID);

        IoDiag_PrintFormattedInt("[Diag][VOUT] VoutChan = %d", VoutID, PrintFunc);
        IoDiag_PrintFormattedInt(", Mode = %d \n", ModeSelect, PrintFunc);
        PrintFunc("[Diag][VOUT] MIPI CSI Test pattern output! \n");
    } else {
        PrintFunc("[Diag][VOUT] Invalid Mode! \n");
    }
}

static void IoDiag_VoutDsiOutput(UINT32 VoutID, UINT32 ModeSelect, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 OutputMode = VOUT_MIPI_DSI_MODE_888_4LANE;
    UINT32 ColorOrder = 0U;

    if (ModeSelect <= IODIAG_VOUT_DSI_MAX_MODE_NUM) {
        (void)AmbaVout_ChannelSelect(VoutID);

        (void)AmbaVout_MipiDsiSetBlankPkt(&BlankPacket[ModeSelect]);

        (void)AmbaVout_MipiDsiEnable(OutputMode, ColorOrder, &DisplayTimingDsi[ModeSelect]);

        (void)AmbaVout_MipiDsiSetCscMatrix(&CscBt709YccFull2GbrFull, 0);

        (void)AmbaDiag_VoutMipiTestPattern();

        (void)AmbaDiag_VoutConfigThrDbgPort(VoutID);

        IoDiag_PrintFormattedInt("[Diag][VOUT] VoutChan = %d", VoutID, PrintFunc);
        IoDiag_PrintFormattedInt(", Mode = %d \n", ModeSelect, PrintFunc);
        PrintFunc("[Diag][VOUT] MIPI DSI Test pattern output! \n");
    } else {
        PrintFunc("[Diag][VOUT] Invalid Mode! \n");
    }
}
#endif

static void IoDiag_VoutCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: \n");
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    PrintFunc(pArgVector[0]);
    PrintFunc(" <channel> csi <mode>                - MIPI CSI output, <mode> 0=1080p60, 1=4Kp30\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <channel> dsi <mode>                - MIPI DSI output, <mode> 0=1080p60, 1=4Kp30\n");
#endif
    PrintFunc(pArgVector[0]);
    PrintFunc(" <channel> info                      - show current vout information\n");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIOUTDiag_CmdVOUT
 *
 *  @Description:: VOUT diagnostic command
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
void AmbaIOUTDiag_CmdVOUT(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 VoutID;
    UINT32 ModeSelect = 0U;

    if (ArgCount < 3U) {
        IoDiag_VoutCmdUsage(pArgVector, PrintFunc);
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[1], &VoutID);

        if (AmbaUtility_StringCompare(pArgVector[2], "info", 4U) == 0) {
            (void)AmbaDiag_VoutShowInfo(VoutID, PrintFunc);

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        } else if (AmbaUtility_StringCompare(pArgVector[2], "csi", 3U) == 0) {
            if (ArgCount >= 4U) {
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &ModeSelect);
            }
            IoDiag_VoutCsiOutput(VoutID, ModeSelect, PrintFunc);

        } else if (AmbaUtility_StringCompare(pArgVector[2], "dsi", 3U) == 0) {
            if (ArgCount >= 4U) {
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &ModeSelect);
            }
            IoDiag_VoutDsiOutput(VoutID, ModeSelect, PrintFunc);
#endif
        } else {
            IoDiag_VoutCmdUsage(pArgVector, PrintFunc);
        }
    }
}
