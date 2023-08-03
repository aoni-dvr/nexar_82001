/**
 *  @file AmbaFPD_Y85183.c
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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
 *  @details Control APIs of DZXtech Display FPD panel Y85183
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaGPIO.h"
#include "AmbaFPD.h"
#include "AmbaVOUT.h"
#include "AmbaFPD_Y85183.h"

#define AMBA_LCD_BACKLIGHT   GPIO_PIN_41
#define AMBA_LCD_RESET       GPIO_PIN_42
#define Y85183_MIPI_DELAY    2

static AMBA_FPD_Y85183_CONFIG_s *pDispConfig;

/*-----------------------------------------------------------------------------------------------*\
 * Y85183 video mode info
\*-----------------------------------------------------------------------------------------------*/
static AMBA_FPD_Y85183_CONFIG_s Y85183_Config[AMBA_FPD_Y85183_NUM_MODE] = {
    [AMBA_FPD_Y85183_720_1280_60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_565_4LANE,
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
            .LenHBP     = 28,
            .LenHSA     = 10,
            .LenHFP     = 90,
            .LenBLLP    = 0,
        }
    },
    [AMBA_FPD_Y85183_720_1280_A60HZ] = {
        .OutputMode     = VOUT_MIPI_DSI_MODE_565_4LANE,
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
            .LenHBP     = 28,
            .LenHSA     = 10,
            .LenHFP     = 90,
            .LenBLLP    = 0,
        }
    }
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Y85183_Init
 *
 *  @Description:: Write Y85183 register
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
static void Y85183_Init(void)
{
    UINT8 Parameter[3];

    /* Page 3 programming */
    Parameter[0] = 0x98;
    Parameter[1] = 0x81;
    Parameter[2] = 0x03;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xff, 3, Parameter);

    Parameter[0] = 0x00;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SOFT_RESET, 1, Parameter);
    Parameter[0] = 0x00;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x2, 1, Parameter);
    Parameter[0] = 0x53;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x3, 1, Parameter);
    Parameter[0] = 0x13;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x4, 1, Parameter);
    Parameter[0] = 0x13;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x5, 1, Parameter);
    Parameter[0] = 0x6;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_GET_RED_CHANNEL, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_GET_GREEN_CHANNEL, 1, Parameter);
    Parameter[0] = 0x4;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_GET_BLUE_CHANNEL, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x9, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_GET_POWER_MODE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_GET_ADDRESS_MODE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_GET_PIXEL_FORMAT, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_GET_DISPLAY_MODE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_GET_SIGNAL_MODE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_GET_DIAGNOSTIC_RESULT, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_ENTER_SLEEP_MODE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_EXIT_SLEEP_MODE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_ENTER_PARTIAL_MODE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_ENTER_NORMAL_MODE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x14, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x15, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x16, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x17, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x18, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x19, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x1a, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x1b, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x1c, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x1d, 1, Parameter);
    Parameter[0] = 0xC0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x1e, 1, Parameter);
    Parameter[0] = 0x80;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x1f, 1, Parameter);

    Parameter[0] = 0x4;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x20, 1, Parameter);
    Parameter[0] = 0xb;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_ENTER_INVERT_MODE, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x22, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x23, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x24, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x25, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_GAMMA_CURVE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x27, 1, Parameter);
    Parameter[0] = 0x55;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_DISPLAY_OFF, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x3;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_DISPLAY_ON, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_COLUMN_ADDRESS, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_PAGE_ADDRESS, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_WRITE_MEMORY_START, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_WRITE_LUT, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_READ_MEMORY_START, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x2f, 1, Parameter);

    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_PARTIAL_ROWS, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_PARTIAL_COLUMNS, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x32, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_SCROLL_AREA, 1, Parameter);
    Parameter[0] = 0x4;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_TEAR_OFF, 1, Parameter);
    Parameter[0] = 0x5;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_TEAR_ON, 1, Parameter);
    Parameter[0] = 0x5;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_ADDRESS_MODE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_SCROLL_START, 1, Parameter);
    Parameter[0] = 0x3c;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_EXIT_IDLE_MODE, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_ENTER_IDLE_MODE, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x40;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_PIXEL_FORMAT, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x40;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x3b, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_WRITE_MEMORY_CONTINUE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x3d, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_READ_MEMORY_CONTINUE, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x3f, 1, Parameter);

    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x40, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x41, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x42, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x43, 1, Parameter);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_TEAR_SCANLINE, 1, Parameter);

    Parameter[0] = 0x1;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x50, 1, Parameter);
    Parameter[0] = 0x23;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x51, 1, Parameter);
    Parameter[0] = 0x45;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x52, 1, Parameter);

    Parameter[0] = 0x67;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x53, 1, Parameter);
    Parameter[0] = 0x89;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x54, 1, Parameter);
    Parameter[0] = 0xAB;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x55, 1, Parameter);
    Parameter[0] = 0x01;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x56, 1, Parameter);
    Parameter[0] = 0x23;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x57, 1, Parameter);
    Parameter[0] = 0x45;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x58, 1, Parameter);
    Parameter[0] = 0x67;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x59, 1, Parameter);
    Parameter[0] = 0x89;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x5a, 1, Parameter);
    Parameter[0] = 0xAB;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x5b, 1, Parameter);
    Parameter[0] = 0xCD;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x5c, 1, Parameter);
    Parameter[0] = 0xEF;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x5d, 1, Parameter);
    Parameter[0] = 0x1;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x5e, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x14;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x5f, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);

    Parameter[0] = 0x15;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x60, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xc;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x61, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xd;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x62, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xe;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x63, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xf;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x64, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x10;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x65, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x11;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x66, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x8;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x67, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x68, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xa;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x69, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x6a, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x6b, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x6c, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x6d, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x6e, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x6f, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);

    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x70, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x71, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x6;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x72, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x73, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x74, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x14;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x75, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x15;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x76, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x11;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x77, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x10;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x78, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0f;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x79, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0e;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x7a, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0d;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x7b, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0c;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x7c, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x06;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x7d, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x7e, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0a;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x7f, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);

    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x80, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x81, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x82, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x83, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x84, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x85, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x86, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x87, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x08;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x88, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x89, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x02;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x8a, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);

    /* Page 4 Programming */
    Parameter[0] = 0x98;
    Parameter[1] = 0x81;
    Parameter[2] = 0x04;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xff, 3, Parameter);

    Parameter[0] = 0x15;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x6c, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x3b;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x6e, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x53;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x6f, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xA4;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x3a, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x15;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x8d, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xBA;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x87, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x76;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x26, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xD1;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xb2, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xB;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x88, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);

    /* Page 1 Programming */
    Parameter[0] = 0x98;
    Parameter[1] = 0x81;
    Parameter[2] = 0x01;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xff, 3, Parameter);

    Parameter[0] = 0xA;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x22, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x0;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x31, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x8A;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x53, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x88;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x55, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xA6;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x50, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0xA6;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x51, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x14;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x60, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);

    Parameter[0] = 0x8;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xA0, 1, Parameter);
    Parameter[0] = 0x27;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_READ_DDB_START, 1, Parameter);
    Parameter[0] = 0x36;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xA2, 1, Parameter);
    Parameter[0] = 0x15;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xA3, 1, Parameter);
    Parameter[0] = 0x17;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xA4, 1, Parameter);
    Parameter[0] = 0x2B;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xA5, 1, Parameter);
    Parameter[0] = 0x1E;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xA6, 1, Parameter);
    Parameter[0] = 0x1F;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xA7, 1, Parameter);
    Parameter[0] = 0x96;
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_READ_DDB_CONTINUE, 1, Parameter);
    Parameter[0] = 0x1C;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xA9, 1, Parameter);
    Parameter[0] = 0x28;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xAA, 1, Parameter);
    Parameter[0] = 0x7C;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xAB, 1, Parameter);
    Parameter[0] = 0x1B;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xAC, 1, Parameter);
    Parameter[0] = 0x1A;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xAD, 1, Parameter);
    Parameter[0] = 0x4D;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xAE, 1, Parameter);
    Parameter[0] = 0x23;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xAF, 1, Parameter);

    Parameter[0] = 0x29;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xB0, 1, Parameter);
    Parameter[0] = 0x4B;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xB1, 1, Parameter);
    Parameter[0] = 0x5A;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xB2, 1, Parameter);
    Parameter[0] = 0x2C;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xB3, 1, Parameter);

    Parameter[0] = 0x8;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC0, 1, Parameter);
    Parameter[0] = 0x26;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC1, 1, Parameter);
    Parameter[0] = 0x36;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC2, 1, Parameter);
    Parameter[0] = 0x15;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC3, 1, Parameter);
    Parameter[0] = 0x17;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC4, 1, Parameter);
    Parameter[0] = 0x2B;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC5, 1, Parameter);
    Parameter[0] = 0x1F;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC6, 1, Parameter);
    Parameter[0] = 0x1F;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC7, 1, Parameter);
    Parameter[0] = 0x96;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC8, 1, Parameter);
    Parameter[0] = 0x1C;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xC9, 1, Parameter);
    Parameter[0] = 0x29;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xCA, 1, Parameter);
    Parameter[0] = 0x7c;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xCB, 1, Parameter);
    Parameter[0] = 0x1A;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xCC, 1, Parameter);
    Parameter[0] = 0x19;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xCD, 1, Parameter);
    Parameter[0] = 0x4D;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xCE, 1, Parameter);
    Parameter[0] = 0x22;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xCF, 1, Parameter);
    Parameter[0] = 0x29;

    (void)AmbaVout_MipiDsiSendDcsWrite(0xD0, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x4B;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xD1, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x59;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xD2, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);
    Parameter[0] = 0x2C;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xD3, 1, Parameter);
    (void)AmbaKAL_TaskSleep(Y85183_MIPI_DELAY);

    /* Page 0 Programming */
    Parameter[0] = 0x98;
    Parameter[1] = 0x81;
    Parameter[2] = 0x00;
    (void)AmbaVout_MipiDsiSendDcsWrite(0xff, 3, Parameter);

    Parameter[0] = 0x00;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x35, 1, Parameter);
    Parameter[0] = 0x04;
    (void)AmbaVout_MipiDsiSendDcsWrite(0x3A, 1, Parameter);

    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_EXIT_SLEEP_MODE, 0, NULL);
    (void)AmbaKAL_TaskSleep(120);

    /* Display on */
    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_DISPLAY_ON, 0, NULL);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85183Enable
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
static UINT32 FPD_Y85183Enable(void)
{
    UINT32 RetVal = ERR_NONE;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85183Disable
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
static UINT32 FPD_Y85183Disable(void)
{
    UINT32 RetVal = ERR_NONE;

    (void)AmbaVout_MipiDsiSendDcsWrite(MIPI_DCS_SET_DISPLAY_OFF, 0, NULL);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85183GetInfo
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
static UINT32 FPD_Y85183GetInfo(AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    if (pDispConfig == NULL) {
        RetVal = ERR_ARG;
    } else {
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85183GetModeInfo
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
static UINT32 FPD_Y85183GetModeInfo(UINT8 mode, AMBA_FPD_INFO_s *pInfo)
{
    UINT32 RetVal = ERR_NONE;

    if ( (mode >= AMBA_FPD_Y85183_NUM_MODE) || (pInfo == NULL)) {
        RetVal = ERR_ARG;
    } else {
        pDispConfig = &Y85183_Config[mode];
        pInfo->Width = pDispConfig->DisplayTiming.ActivePixels;
        pInfo->Height = pDispConfig->DisplayTiming.ActiveLines;
        pInfo->AspectRatio.X = 16U;
        pInfo->AspectRatio.Y = 9U;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85183Config
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
static UINT32 FPD_Y85183Config(UINT8 Mode)
{
    UINT32 RetVal = ERR_NONE;
    UINT32 ColorOrder = 0U;

    if (Mode >= AMBA_FPD_Y85183_NUM_MODE) {
        RetVal = ERR_ARG;
    } else {
        /* Reset LCD */
        (void)AmbaGPIO_SetFuncGPO(AMBA_LCD_RESET, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(10);
        (void)AmbaGPIO_SetFuncGPO(AMBA_LCD_RESET, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(10);
        (void)AmbaGPIO_SetFuncGPO(AMBA_LCD_RESET, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(1000);

        pDispConfig = &Y85183_Config[Mode];

        RetVal = AmbaVout_MipiDsiSetBlankPkt(&pDispConfig->BlankPacket);

        if (RetVal == OK) {
            RetVal = AmbaVout_MipiDsiEnable(pDispConfig->OutputMode, ColorOrder, &pDispConfig->DisplayTiming);

            /* initial sequence for panel Y85183 */
            Y85183_Init();
        }

        /* Turn on backlight */
        (void)AmbaGPIO_SetFuncGPO(AMBA_LCD_BACKLIGHT, AMBA_GPIO_LEVEL_HIGH);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FPD_Y85183SetBacklight
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
static UINT32 FPD_Y85183SetBacklight(UINT32 EnableFlag)
{
    extern void AmbaUserGPIO_FpdCtrl(UINT32 FpdFlag);
    AmbaUserGPIO_FpdCtrl(EnableFlag);

    return ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 * FPD driver object
\*-----------------------------------------------------------------------------------------------*/
AMBA_FPD_OBJECT_s AmbaFPD_Y85183Obj = {
    .FpdEnable          = FPD_Y85183Enable,
    .FpdDisable         = FPD_Y85183Disable,
    .FpdGetInfo         = FPD_Y85183GetInfo,
    .FpdGetModeInfo     = FPD_Y85183GetModeInfo,
    .FpdConfig          = FPD_Y85183Config,
    .FpdSetBacklight    = FPD_Y85183SetBacklight,
    .FpdGetLinkStatus   = NULL,

    .pName = "Y85183"
};
