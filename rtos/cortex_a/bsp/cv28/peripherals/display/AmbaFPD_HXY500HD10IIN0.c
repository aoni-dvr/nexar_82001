/**
 *  @file AmbaFPD_HXY500HD10IIN0.c
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
 *  @details Control APIs of Display FPD panel HXY500HD10IIN0
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaFPD.h"
#include "AmbaVOUT.h"
#include "AmbaFPD_HXY500HD10IIN0.h"

#define HXY500HD10IIN0_MIPI_DELAY    2

static AMBA_FPD_HXY500HD10IIN0_CONFIG_s *pDispConfig;

/*-----------------------------------------------------------------------------------------------*\
 * HXY500HD10IIN0 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_HXY500HD10IIN0_CONFIG_s HXY500HD10IIN0_Config[AMBA_FPD_HXY500HD10IIN0_NUM_MODE] = {
    [AMBA_FPD_HXY500HD10IIN0_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 62145854,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 49,
            .HsyncPulseWidth    = 10,
            .HsyncBackPorch     = 21,
            .VsyncFrontPorch    = 9,
            .VsyncPulseWidth    = 3,
            .VsyncBackPorch     = 4,
            .ActivePixels       = 720,
            .ActiveLines        = 1280,
        },
        .BlankPacket = {
            .LenHBP     = 44,
            .LenHSA     = 18,
            .LenHFP     = 146,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_HXY500HD10IIN0_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_888_4LANE,
        .DisplayTiming    = {
            .PixelClkFreq       = 62208000,
            .DisplayMethod      = 0,
            .HsyncFrontPorch    = 49,
            .HsyncPulseWidth    = 10,
            .HsyncBackPorch     = 21,
            .VsyncFrontPorch    = 9,
            .VsyncPulseWidth    = 3,
            .VsyncBackPorch     = 4,
            .ActivePixels       = 720,
            .ActiveLines        = 1280,
        },
        .BlankPacket = {
            .LenHBP     = 44,
            .LenHSA     = 18,
            .LenHFP     = 146,
            .LenBLLP    = 0,
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: HXY500HD10IIN0_Init
 *
 *  @Description:: Write HXY500HD10IIN0 register
 *
 *  @Input      ::
 *      Offset:     Register offset
 *      Data:       Register data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void HXY500HD10IIN0_ExcuteCommandSet(const AMBA_FPD_HXY500HD10IIN0_COMMAND_s *pCommendSet, UINT32 Count)
{
    UINT8 Parameter[16] = {0};
    UINT32 i, j, NumData;

    for ( i = 0U; i < Count; i++) {

        NumData = pCommendSet->NumData;
        if (NumData > 16U) {
            /* Treat as delay */
            (void)AmbaKAL_TaskSleep(NumData);
        } else if (NumData == 1U) {
            (void)AmbaVout_MipiDsiSendDcsWrite(pCommendSet->Address, NumData, &pCommendSet->Data[0]);
        } else {
            for (j = 0U; j < NumData; j++) {
                Parameter[j] = pCommendSet->Data[j];
            }
            (void)AmbaVout_MipiDsiSendDcsWrite(pCommendSet->Address, NumData, Parameter);
        }
        pCommendSet++;
        (void)AmbaKAL_TaskSleep(HXY500HD10IIN0_MIPI_DELAY);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: HXY500HD10IIN0_Init
 *
 *  @Description:: Write HXY500HD10IIN0 register
 *
 *  @Input      ::
 *      Offset:     Register offset
 *      Data:       Register data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void HXY500HD10IIN0_Init(void)
{
    UINT32 Count;
    AMBA_FPD_HXY500HD10IIN0_COMMAND_s CommandSet3[] = {
        {0xff, 3, {0x98, 0x81, 0x03}},
        {0x01, 1, {0x00, 0x00, 0x00}},
        {0x02, 1, {0x00, 0x00, 0x00}},
        {0x03, 1, {0x53, 0x00, 0x00}},
        {0x04, 1, {0x53, 0x00, 0x00}},
        {0x05, 1, {0x13, 0x00, 0x00}},
        {0x06, 1, {0x04, 0x00, 0x00}},
        {0x07, 1, {0x02, 0x00, 0x00}},
        {0x08, 1, {0x02, 0x00, 0x00}},
        {0x09, 1, {0x00, 0x00, 0x00}},
        {0x0a, 1, {0x00, 0x00, 0x00}},
        {0x0b, 1, {0x00, 0x00, 0x00}},
        {0x0c, 1, {0x00, 0x00, 0x00}},
        {0x0d, 1, {0x00, 0x00, 0x00}},
        {0x0e, 1, {0x00, 0x00, 0x00}},
        {0x0f, 1, {0x00, 0x00, 0x00}},
        {0x10, 1, {0x00, 0x00, 0x00}},
        {0x11, 1, {0x00, 0x00, 0x00}},
        {0x12, 1, {0x00, 0x00, 0x00}},
        {0x13, 1, {0x00, 0x00, 0x00}},
        {0x14, 1, {0x00, 0x00, 0x00}},
        {0x15, 1, {0x08, 0x00, 0x00}},
        {0x16, 1, {0x10, 0x00, 0x00}},
        {0x17, 1, {0x00, 0x00, 0x00}},
        {0x18, 1, {0x08, 0x00, 0x00}},
        {0x19, 1, {0x00, 0x00, 0x00}},
        {0x1a, 1, {0x00, 0x00, 0x00}},
        {0x1b, 1, {0x00, 0x00, 0x00}},
        {0x1c, 1, {0x00, 0x00, 0x00}},
        {0x1d, 1, {0x00, 0x00, 0x00}},
        {0x1e, 1, {0xc0, 0x00, 0x00}},
        {0x1f, 1, {0x80, 0x00, 0x00}},
        {0x20, 1, {0x02, 0x00, 0x00}},
        {0x21, 1, {0x09, 0x00, 0x00}},
        {0x22, 1, {0x00, 0x00, 0x00}},
        {0x23, 1, {0x00, 0x00, 0x00}},
        {0x24, 1, {0x00, 0x00, 0x00}},
        {0x25, 1, {0x00, 0x00, 0x00}},
        {0x26, 1, {0x00, 0x00, 0x00}},
        {0x27, 1, {0x00, 0x00, 0x00}},
        {0x28, 1, {0x55, 0x00, 0x00}},
        {0x29, 1, {0x03, 0x00, 0x00}},
        {0x2a, 1, {0x00, 0x00, 0x00}},
        {0x2b, 1, {0x00, 0x00, 0x00}},
        {0x2c, 1, {0x00, 0x00, 0x00}},
        {0x2d, 1, {0x00, 0x00, 0x00}},
        {0x2e, 1, {0x00, 0x00, 0x00}},
        {0x2f, 1, {0x00, 0x00, 0x00}},
        {0x30, 1, {0x00, 0x00, 0x00}},
        {0x31, 1, {0x00, 0x00, 0x00}},
        {0x32, 1, {0x00, 0x00, 0x00}},
        {0x33, 1, {0x00, 0x00, 0x00}},
        {0x34, 1, {0x04, 0x00, 0x00}},
        {0x35, 1, {0x05, 0x00, 0x00}},
        {0x36, 1, {0x02, 0x00, 0x00}},
        {0x37, 1, {0x00, 0x00, 0x00}},
        {0x38, 1, {0x3c, 0x00, 0x00}},
        {0x39, 1, {0x35, 0x00, 0x00}},
        {0x3a, 1, {0x00, 0x00, 0x00}},
        {0x3b, 1, {0x40, 0x00, 0x00}},
        {0x3c, 1, {0x00, 0x00, 0x00}},
        {0x3d, 1, {0x00, 0x00, 0x00}},
        {0x3e, 1, {0x00, 0x00, 0x00}},
        {0x3f, 1, {0x00, 0x00, 0x00}},
        {0x40, 1, {0x00, 0x00, 0x00}},
        {0x41, 1, {0x88, 0x00, 0x00}},
        {0x42, 1, {0x00, 0x00, 0x00}},
        {0x43, 1, {0x00, 0x00, 0x00}},
        {0x44, 1, {0x1f, 0x00, 0x00}},
        {0x50, 1, {0x01, 0x00, 0x00}},
        {0x51, 1, {0x23, 0x00, 0x00}},
        {0x52, 1, {0x45, 0x00, 0x00}},
        {0x53, 1, {0x67, 0x00, 0x00}},
        {0x54, 1, {0x89, 0x00, 0x00}},
        {0x55, 1, {0xab, 0x00, 0x00}},
        {0x56, 1, {0x01, 0x00, 0x00}},
        {0x57, 1, {0x23, 0x00, 0x00}},
        {0x58, 1, {0x45, 0x00, 0x00}},
        {0x59, 1, {0x67, 0x00, 0x00}},
        {0x5a, 1, {0x89, 0x00, 0x00}},
        {0x5b, 1, {0xab, 0x00, 0x00}},
        {0x5c, 1, {0xcd, 0x00, 0x00}},
        {0x5d, 1, {0xef, 0x00, 0x00}},
        {0x5e, 1, {0x03, 0x00, 0x00}},
        {0x5f, 1, {0x14, 0x00, 0x00}},
        {0x60, 1, {0x15, 0x00, 0x00}},
        {0x61, 1, {0x0c, 0x00, 0x00}},
        {0x62, 1, {0x0D, 0x00, 0x00}},
        {0x63, 1, {0x0e, 0x00, 0x00}},
        {0x64, 1, {0x0F, 0x00, 0x00}},
        {0x65, 1, {0x10, 0x00, 0x00}},
        {0x66, 1, {0x11, 0x00, 0x00}},
        {0x67, 1, {0x08, 0x00, 0x00}},
        {0x68, 1, {0x02, 0x00, 0x00}},
        {0x69, 1, {0x0A, 0x00, 0x00}},
        {0x6a, 1, {0x02, 0x00, 0x00}},
        {0x6b, 1, {0x02, 0x00, 0x00}},
        {0x6c, 1, {0x02, 0x00, 0x00}},
        {0x6d, 1, {0x02, 0x00, 0x00}},
        {0x6e, 1, {0x02, 0x00, 0x00}},
        {0x6f, 1, {0x02, 0x00, 0x00}},
        {0x70, 1, {0x02, 0x00, 0x00}},
        {0x71, 1, {0x02, 0x00, 0x00}},
        {0x72, 1, {0x06, 0x00, 0x00}},
        {0x73, 1, {0x02, 0x00, 0x00}},
        {0x74, 1, {0x02, 0x00, 0x00}},
        {0x75, 1, {0x14, 0x00, 0x00}},
        {0x76, 1, {0x15, 0x00, 0x00}},
        {0x77, 1, {0x0F, 0x00, 0x00}},
        {0x78, 1, {0x0E, 0x00, 0x00}},
        {0x79, 1, {0x0D, 0x00, 0x00}},
        {0x7a, 1, {0x0C, 0x00, 0x00}},
        {0x7b, 1, {0x11, 0x00, 0x00}},
        {0x7c, 1, {0x10, 0x00, 0x00}},
        {0x7d, 1, {0x06, 0x00, 0x00}},
        {0x7e, 1, {0x02, 0x00, 0x00}},
        {0x7f, 1, {0x0A, 0x00, 0x00}},
        {0x80, 1, {0x02, 0x00, 0x00}},
        {0x81, 1, {0x02, 0x00, 0x00}},
        {0x82, 1, {0x02, 0x00, 0x00}},
        {0x83, 1, {0x02, 0x00, 0x00}},
        {0x84, 1, {0x02, 0x00, 0x00}},
        {0x85, 1, {0x02, 0x00, 0x00}},
        {0x86, 1, {0x02, 0x00, 0x00}},
        {0x87, 1, {0x02, 0x00, 0x00}},
        {0x88, 1, {0x08, 0x00, 0x00}},
        {0x89, 1, {0x02, 0x00, 0x00}},
        {0x8a, 1, {0x02, 0x00, 0x00}},
    };

    AMBA_FPD_HXY500HD10IIN0_COMMAND_s CommandSet4[] = {
        {0xff, 3, {0x98, 0x81, 0x04}},
        {0x70, 1, {0x00, 0x00, 0x00}},
        {0x71, 1, {0x00, 0x00, 0x00}},
        {0x66, 1, {0xFE, 0x00, 0x00}},
        {0x82, 1, {0x13, 0x00, 0x00}},
        {0x84, 1, {0x13, 0x00, 0x00}},
        {0x85, 1, {0x15, 0x00, 0x00}},
        {0x3A, 1, {0x24, 0x00, 0x00}},
        {0x32, 1, {0xAC, 0x00, 0x00}},
        {0x8C, 1, {0x80, 0x00, 0x00}},
        {0x3C, 1, {0xF5, 0x00, 0x00}},
        {0x88, 1, {0x33, 0x00, 0x00}},
    };

    AMBA_FPD_HXY500HD10IIN0_COMMAND_s CommandSet1[] = {
        {0xff, 3, {0x98, 0x81, 0x01}},
        {0x22, 1, {0x0A, 0x00, 0x00}},
        {0x31, 1, {0x00, 0x00, 0x00}},
        {0x53, 1, {0x7D, 0x00, 0x00}},
        {0x55, 1, {0x77, 0x00, 0x00}},
        {0x50, 1, {0x5B, 0x00, 0x00}},
        {0x51, 1, {0x5B, 0x00, 0x00}},
        {0x60, 1, {0x1B, 0x00, 0x00}},
        {0x61, 1, {0x00, 0x00, 0x00}},
        {0x62, 1, {0x0D, 0x00, 0x00}},
        {0x63, 1, {0x00, 0x00, 0x00}},
        {0xa0, 1, {0x00, 0x00, 0x00}},
        {0xa1, 1, {0x1B, 0x00, 0x00}},
        {0xa2, 1, {0x27, 0x00, 0x00}},
        {0xa3, 1, {0x0B, 0x00, 0x00}},
        {0xa4, 1, {0x14, 0x00, 0x00}},
        {0xa5, 1, {0x26, 0x00, 0x00}},
        {0xa6, 1, {0x1D, 0x00, 0x00}},
        {0xa7, 1, {0x1E, 0x00, 0x00}},
        {0xa8, 1, {0x7D, 0x00, 0x00}},
        {0xa9, 1, {0x7D, 0x00, 0x00}},
        {0xaa, 1, {0x27, 0x00, 0x00}},
        {0xab, 1, {0x6E, 0x00, 0x00}},
        {0xac, 1, {0x1F, 0x00, 0x00}},
        {0xad, 1, {0x20, 0x00, 0x00}},
        {0xae, 1, {0x55, 0x00, 0x00}},
        {0xaf, 1, {0x29, 0x00, 0x00}},
        {0xb0, 1, {0x2C, 0x00, 0x00}},
        {0xb1, 1, {0x49, 0x00, 0x00}},
        {0xb2, 1, {0x68, 0x00, 0x00}},
        {0xb3, 1, {0x39, 0x00, 0x00}},
        {0xc0, 1, {0x00, 0x00, 0x00}},
        {0xc1, 1, {0x0F, 0x00, 0x00}},
        {0xc2, 1, {0x1E, 0x00, 0x00}},
        {0xc3, 1, {0x18, 0x00, 0x00}},
        {0xc4, 1, {0x16, 0x00, 0x00}},
        {0xc5, 1, {0x2B, 0x00, 0x00}},
        {0xc6, 3, {0x1D, 0x81, 0x00}},
        {0xc7, 1, {0x1E, 0x00, 0x00}},
        {0xc8, 1, {0x7D, 0x00, 0x00}},
        {0xc9, 1, {0x19, 0x00, 0x00}},
        {0xca, 1, {0x28, 0x00, 0x00}},
        {0xcb, 1, {0x95, 0x00, 0x00}},
        {0xcc, 1, {0x1C, 0x00, 0x00}},
        {0xcd, 3, {0x1E, 0x81, 0x00}},
        {0xce, 1, {0x56, 0x00, 0x00}},
        {0xcf, 1, {0x2C, 0x00, 0x00}},
        {0xd0, 1, {0x2F, 0x00, 0x00}},
        {0xd1, 1, {0x4B, 0x00, 0x00}},
        {0xd2, 1, {0x6B, 0x00, 0x00}},
        {0xd3, 1, {0x39, 0x00, 0x00}},
    };

    AMBA_FPD_HXY500HD10IIN0_COMMAND_s CommandSet0[] = {
        {0xff, 3, {0x98, 0x81, 0x00}},
        {0x35, 1, {0x00, 0x00, 0x00}},
        //{0x36, 1, {0x02, 0x00, 0x00}},      //Flip
        {0x11, 1, {0x00, 0x00, 0x00}},
        {0x00, 120, {0x00, 0x00, 0x00}},    //Delay
        {0x29, 1, {0x00, 0x00, 0x00}},
        {0x00, 50, {0x00, 0x00, 0x00}},     //Delay
    };

    /* Page 3 programming */
    Count = sizeof(CommandSet3) / sizeof(AMBA_FPD_HXY500HD10IIN0_COMMAND_s);
    HXY500HD10IIN0_ExcuteCommandSet(CommandSet3, Count);

    /* Page 4 Programming */
    Count = sizeof(CommandSet4) / sizeof(AMBA_FPD_HXY500HD10IIN0_COMMAND_s);
    HXY500HD10IIN0_ExcuteCommandSet(CommandSet4, Count);

    /* Page 1 Programming */
    Count = sizeof(CommandSet1) / sizeof(AMBA_FPD_HXY500HD10IIN0_COMMAND_s);
    HXY500HD10IIN0_ExcuteCommandSet(CommandSet1, Count);

    /* Page 0 Programming */
    Count = sizeof(CommandSet0) / sizeof(AMBA_FPD_HXY500HD10IIN0_COMMAND_s);
    HXY500HD10IIN0_ExcuteCommandSet(CommandSet0, Count);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_HXY500HD10IIN0Enable
 *
 *  @Description:: Enable FPD panel device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_HXY500HD10IIN0Enable(void)
{
    UINT32 RetVal = ERR_NONE;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_HXY500HD10IIN0Disable
 *
 *  @Description:: Disable FPD panel device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_HXY500HD10IIN0Disable(void)
{
    UINT32 RetVal = ERR_NONE;

    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_DISPLAY_OFF, 0, NULL);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_HXY500HD10IIN0GetInfo
 *
 *  @Description:: Get vout configuration for current FPD display mode
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pInfo:      pointer to FPD display mode info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_HXY500HD10IIN0GetInfo(AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 Htotal, Vtotal;
    UINT32 Framerate;

    if (pDispConfig == NULL) {
        RetVal = ERR_ARG;
    } else {
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;

        Htotal = (UINT32)pDispConfig->DisplayTiming.ActivePixels + (UINT32)pDispConfig->DisplayTiming.HsyncBackPorch + (UINT32)pDispConfig->DisplayTiming.HsyncFrontPorch + (UINT32)pDispConfig->DisplayTiming.HsyncPulseWidth;
        Vtotal = (UINT32)pDispConfig->DisplayTiming.ActiveLines + (UINT32)pDispConfig->DisplayTiming.VsyncBackPorch + (UINT32)pDispConfig->DisplayTiming.VsyncFrontPorch + (UINT32)pDispConfig->DisplayTiming.VsyncPulseWidth;
        Framerate = (UINT32) pDispConfig->DisplayTiming.PixelClkFreq / (Htotal * Vtotal);
        if (Framerate == 60U) {
            pInfo->FrameRate.TimeScale = 60U;
            pInfo->FrameRate.NumUnitsInTick = 1U;
            pInfo->FrameRate.Interlace = 0U;
        } else {
            /* 59.94Hz */
            pInfo->FrameRate.TimeScale = 60000U;
            pInfo->FrameRate.NumUnitsInTick = 1001U;
            pInfo->FrameRate.Interlace = 0U;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_HXY500HD10IIN0GetModeInfo
 *
 *  @Description:: Get vout configuration for specific mode
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pInfo:      pointer to FPD display mode info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_HXY500HD10IIN0GetModeInfo(UINT8 mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 Htotal, Vtotal;
    UINT32 Framerate;

    if ( (mode >= AMBA_FPD_HXY500HD10IIN0_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &HXY500HD10IIN0_Config[mode];
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;

        Htotal = (UINT32)pDispConfig->DisplayTiming.ActivePixels + (UINT32)pDispConfig->DisplayTiming.HsyncBackPorch + (UINT32)pDispConfig->DisplayTiming.HsyncFrontPorch + (UINT32)pDispConfig->DisplayTiming.HsyncPulseWidth;
        Vtotal = (UINT32)pDispConfig->DisplayTiming.ActiveLines + (UINT32)pDispConfig->DisplayTiming.VsyncBackPorch + (UINT32)pDispConfig->DisplayTiming.VsyncFrontPorch + (UINT32)pDispConfig->DisplayTiming.VsyncPulseWidth;
        Framerate = (UINT32) pDispConfig->DisplayTiming.PixelClkFreq / (Htotal * Vtotal);
        if (Framerate == 60U) {
            pInfo->FrameRate.TimeScale = 60U;
            pInfo->FrameRate.NumUnitsInTick = 1U;
            pInfo->FrameRate.Interlace = 0U;
        } else {
            /* 59.94Hz */
            pInfo->FrameRate.TimeScale = 60000U;
            pInfo->FrameRate.NumUnitsInTick = 1001U;
            pInfo->FrameRate.Interlace = 0U;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_HXY500HD10IIN0Config
 *
 *  @Description:: Configure FPD display mode
 *
 *  @Input      ::
 *      pFpdConfig: configuration of FPD display mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_HXY500HD10IIN0Config(UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 ColorOrder = 0U;

    if (Mode >= AMBA_FPD_HXY500HD10IIN0_NUM_MODE) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &HXY500HD10IIN0_Config[Mode];

        RetVal = AmbaVout_MipiDsiSetBlankPkt(&pDispConfig->BlankPacket);

        if (RetVal == OK) {
            RetVal = AmbaVout_MipiDsiEnable(pDispConfig->OutputMode, ColorOrder, &pDispConfig->DisplayTiming);

            /* initial sequence for panel HXY500HD10IIN0 */
            HXY500HD10IIN0_Init();
        }

        if (RetVal == OK) {
            /* YCC(Limited) to RGB(Full) */
            AMBA_VOUT_CSC_MATRIX_s CscBt601YccLIimit2GbrFull = {
                .Coef = {
                    [0] = {  1.16438f, -0.39200f, -0.81300f },
                    [1] = {  1.16438f,  2.01700f,  0.00000f },
                    [2] = {  1.16438f,  0.00000f,  1.59600f },
                },
                .Offset = { [0] =  135.61f, [1] = -276.81f, [2] = -222.92f },
                .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
                .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
            };
            RetVal = AmbaVout_MipiDsiSetCscMatrix(&CscBt601YccLIimit2GbrFull, 0);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_HXY500HD10IIN0SetBacklight
 *
 *  @Description:: Turn FPD Backlight On/Off
 *
 *  @Input      ::
 *      EnableFlag: 1 = Backlight On, 0 = Backlight Off
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FPD_HXY500HD10IIN0SetBacklight(UINT32 EnableFlag)
{
    extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
    AmbaUserGPIO_FpdCtrl(EnableFlag);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_HXY500HD10IIN0Obj = {
    .FpdEnable          = FPD_HXY500HD10IIN0Enable,
    .FpdDisable         = FPD_HXY500HD10IIN0Disable,
    .FpdGetInfo         = FPD_HXY500HD10IIN0GetInfo,
    .FpdGetModeInfo     = FPD_HXY500HD10IIN0GetModeInfo,
    .FpdConfig          = FPD_HXY500HD10IIN0Config,
    .FpdSetBacklight    = FPD_HXY500HD10IIN0SetBacklight,
    .FpdGetLinkStatus   = NULL,

    .pName = "HXY500HD10IIN0"
};
