/**
 *  @file AmbaSensor_MAX9295_96712_IMX224Table.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Control APIs of MAX9295_96712 plus SONY IMX224 CMOS sensor with MIPI interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_96712_IMX224.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s MX01_IMX224_DeviceInfo = {
    .UnitCellWidth          = 3.75f,
    .UnitCellHeight         = 3.75f,
    .NumTotalPixelCols      = 1312,
    .NumTotalPixelRows      = 993,
    .NumEffectivePixelCols  = 1280,
    .NumEffectivePixelRows  = 960,
    .MinAnalogGainFactor    = 1.0f,
    .MaxAnalogGainFactor    = 3981.071706f * 2.0f, /* 30.0 dB analog gain + 42.0 dB digital gain + 6.02 dB in HCG mode */
    .MinDigitalGainFactor   = 1.0f,
    .MaxDigitalGainFactor   = 1.0f,

    .FrameRateCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .ShutterSpeedCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .AnalogGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .DigitalGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .WbGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 0,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },

    .HdrIsSupport = 1U,
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings for initialization
\*-----------------------------------------------------------------------------------------------*/
MX01_IMX224_REG_s MX01_IMX224_InitRegTable[MX01_IMX224_NUM_INIT_REG] = {
    /* chip ID = 02h, do not change */
    {0x300f, 0x00},
    {0x3012, 0x2c},
    {0x3013, 0x01},
    {0x3016, 0x09},
    {0x301d, 0xc2},
    {0x3054, 0x66}, /* CSI-2 output */
    {0x3070, 0x02},
    {0x3071, 0x01},
    {0x309e, 0x22},
    {0x30a5, 0xfb},
    {0x30a6, 0x02},
    {0x30b3, 0xff},
    {0x30b4, 0x01},
    {0x30b5, 0x42},
    {0x30b8, 0x10},
    {0x30c2, 0x01},

    /* chip ID = 03h, do not change */
    {0x310f, 0x0f},
    {0x3110, 0x0e},
    {0x3111, 0xe7},
    {0x3112, 0x9c},
    {0x3113, 0x83},
    {0x3114, 0x10},
    {0x3115, 0x42},
    {0x3128, 0x1e},
    {0x31ed, 0x38},

    /* chip ID = 04h, do not change */
    {0x320c, 0xcf},
    {0x324c, 0x40},
    {0x324d, 0x03},
    {0x3261, 0xe0},
    {0x3262, 0x02},
    {0x326e, 0x2f},
    {0x326f, 0x30},
    {0x3270, 0x03},
    {0x3298, 0x00},
    {0x329a, 0x12},
    {0x329b, 0xf1},
    {0x329c, 0x0c},
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
MX01_IMX224_MODE_REG_s MX01_IMX224_ModeRegTable[MX01_IMX224_NUM_READOUT_MODE_REG] = {
    /* Addr    Q30,  Q60, Q120,  Q25,  Q50, Q100,C30_H,C60_H,C25_H,C50_H,Q30_H,Q25_H,Q30H1,C60_H1, Q60 */
    {0x3005, {0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01}}, /*   [0]: ADBIT */
    {0x3006, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: MODE */
    {0x3007, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00}}, /*   [0]: VREVERSE, [1]: HREVERSE, [6:4]: WINMODE */
    {0x3009, {0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01}}, /* [1:0]: FRSEL, [4]: FDG_SEL */
    {0x300a, {0xf0, 0xf0, 0x3c, 0xf0, 0xf0, 0x3c, 0xf0, 0x3c, 0xf0, 0x3c, 0xf0, 0xf0, 0x3c, 0x3c, 0xf0}}, /* [7:0]: BLKLEVEL_LSB */
    {0x300b, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*   [0]: BLKLEVEL_MSB */
    {0x300c, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x21, 0x11, 0x00}}, /*#  [0]: WDMODE, [5:4]: WDSEL */
    {0x3018, {0x4c, 0x4c, 0x4c, 0x28, 0x28, 0x28, 0x4c, 0x4c, 0x28, 0x28, 0x4c, 0x28, 0x4c, 0x4c, 0x4c}}, /* [7:0]: VMAX_LSB (valid when sensor in master mode) */
    {0x3019, {0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x04, 0x04, 0x05, 0x05, 0x04, 0x05, 0x04, 0x04, 0x04}}, /* [7:0]: VMAX_MSB (valid when sensor in master mode) */
    {0x301b, {0x94, 0xca, 0x65, 0x94, 0xca, 0x65, 0xca, 0x65, 0xca, 0x65, 0xca, 0xca, 0x65, 0x65, 0xca}}, /* [7:0]: HMAX_LSB (valid when sensor in master mode) */
    {0x301c, {0x11, 0x08, 0x04, 0x11, 0x08, 0x04, 0x08, 0x04, 0x08, 0x04, 0x08, 0x08, 0x04, 0x04, 0x08}}, /* [5:0]: HMAX_MSB (valid when sensor in master mode) */
    {0x3020, {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x03, 0x03, 0x04, 0x03, 0x03}}, /*#[7:0]: SHS1_LSB */
    {0x3021, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[7:0]: SHS1_MSB */
    {0x3022, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[3:0]: SHS1_HSB */
    {0x3023, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x2f, 0x2f, 0xc4, 0xc4, 0x8c, 0xff, 0x00}}, /*#[7:0]: SHS2_LSB */
    {0x3024, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[7:0]: SHS2_MSB */
    {0x3025, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[3:0]: SHS2_HSB */
    {0x3026, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00}}, /*#[7:0]: SHS3_LSB */
    {0x3027, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00}}, /*#[7:0]: SHS3_MSB */
    {0x3028, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[3:0]: SHS3_HSB */
    {0x302c, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7, 0xf7, 0x29, 0x29, 0xc1, 0xc1, 0x85, 0xf7, 0x00}}, /*#[7:0]: RHS1_LSB */
    {0x302d, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[7:0]: RHS1_MSB */
    {0x302e, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[3:0]: RHS1_HSB */
    {0x302f, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x00, 0x00}}, /*#[7:0]: RHS2_LSB */
    {0x3030, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[7:0]: RHS2_MSB */
    {0x3031, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[3:0]: RHS2_HSB */
    {0x3036, {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x06, 0x06, 0x06, 0x06, 0x10, 0x10, 0x10, 0x06, 0x10}}, /*#[4:0]: WINWV_OB */
    {0x3038, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x78, 0x78, 0x78, 0x00, 0x00, 0x00, 0x78, 0x00}}, /*#[7:0]: WINPV_LSB */
    {0x3039, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[1:0]: WINPV_MSB */
    {0x303a, {0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd8, 0xd8, 0xd8, 0xd8, 0xd1, 0xd1, 0xd1, 0xd8, 0xd1}}, /*#[7:0]: WINWV_LSB */
    {0x303b, {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x02, 0x03}}, /*#[1:0]: WINWV_MSB */
    {0x3043, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x01}}, /*#[1]: DOLSYDINFOEN, [2]: HINFOEN */
    {0x3044, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}}, /* [1:0]: ODBIT, [7:4]: OPORTSEL */
    {0x3049, {0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a}}, /*#[1:0]: XVSOUTSEL, [3:2]: XHSOUTSEL (valid when sensor in master mode) */
    {0x3054, {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66}}, /* [7:0]: CSI-2 setting */
    {0x305c, {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}}, /* [7:0]: INCKSEL1 */
    {0x305d, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: INCKSEL2 */
    {0x305e, {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}}, /* [7:0]: INCKSEL3 */
    {0x305f, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: INCKSEL4 */
    {0x3108, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /*#[1:0]: XVSMSKCNT, [5:4]: XHSMSKCNT (must be 0 when sensor in slave mode) */
    {0x3109, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x00}}, /*#[1:0]: XVSMSKCNT_INT */
    {0x3344, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [5:4]: REPETITION */
    {0x3346, {0x00, 0x01, 0x03, 0x00, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x01, 0x03, 0x03, 0x01}}, /* [1:0]: PHYSICAL_LANE_NUM */
    {0x3353, {0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x06, 0x06, 0x06, 0x06, 0x0e, 0x0e, 0x0e, 0x06, 0x0e}}, /* [5:0]: OB_SIZE_V */
    {0x3354, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}, /*#[7:0]: NULL0_SIZE_V */
    {0x3357, {0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0xd1, 0x64, 0x64, 0x50, 0x50, 0x74, 0x74, 0x08, 0x64, 0xd1}}, /* [7:0]: PIC_SIZE_V_LSB */
    {0x3358, {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x08, 0x08, 0x0a, 0x0a, 0x08, 0x08, 0x11, 0x08, 0x03}}, /* [4:0]: PIC_SIZE_V_MSB */
    {0x336b, {0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57}}, /* [7:0]: TXSEXIT */
    {0x336c, {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f}}, /* [7:0]: TCLKPRE */
    {0x337d, {0x0c, 0x0c, 0x0a, 0x0c, 0x0c, 0x0a, 0x0c, 0x0a, 0x0c, 0x0a, 0x0c, 0x0c, 0x0a, 0x0a, 0x0c}}, /* [7:0]: CSI_DT_FMT_LSB */
    {0x337e, {0x0c, 0x0c, 0x0a, 0x0c, 0x0c, 0x0a, 0x0c, 0x0a, 0x0c, 0x0a, 0x0c, 0x0c, 0x0a, 0x0a, 0x0c}}, /* [7:0]: CSI_DT_FMT_MSB */
    {0x337f, {0x00, 0x01, 0x03, 0x00, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x01, 0x03, 0x03, 0x01}}, /* [1:0]: CSI_LANE_MODE */
    {0x3380, {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}}, /* [7:0]: INCK_FREQ1_LSB */
    {0x3381, {0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25}}, /* [7:0]: INCK_FREQ1_MSB */
    {0x3382, {0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f}}, /* [7:0]: TCLKPOST */
    {0x3383, {0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27}}, /* [7:0]: THSPREPARE */
    {0x3384, {0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f}}, /* [7:0]: THSZERO */
    {0x3385, {0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f}}, /* [7:0]: THSTRAIL */
    {0x3386, {0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f}}, /* [7:0]: TCLKTRAIL */
    {0x3387, {0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f}}, /* [7:0]: TCLKPREPARE */
    {0x3388, {0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f}}, /* [7:0]: TCLKZERO */
    {0x3389, {0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37}}, /* [7:0]: TLPX */
    {0x338d, {0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4}}, /* [7:0]: INCK_FREQ2_LSB */
    {0x338e, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}}, /* [7:0]: INCK_FREQ2_MSB */
};

const MX01_IMX224_SENSOR_INFO_s MX01_IMX224_SensorInfo[MX01_IMX224_NUM_MODE] = {
#ifdef MX01_IMX224_IN_SLAVE_MODE
    /* MX01_IMX224_1280_960_30P      */ {37125000, 594000000, 1, 12, 1500, 1100, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_60P      */ {37125000, 594000000, 2, 12, 1500, 1100, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_120P     */ {37125000, 594000000, 4, 10, 1800, 1100, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_25P      */ {37125000, 594000000, 1, 12, 1500, 1320, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_50P      */ {37125000, 594000000, 2, 12, 1500, 1320, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_100P     */ {37125000, 594000000, 4, 10, 1800, 1320, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_720_30P_HDR  */ {37125000, 594000000, 2, 12, 1500, 2200, 1316, 1716, { 20,  0, 1280, 1716}, {0}},
    /* MX01_IMX224_1280_720_60P_HDR  */ {37125000, 594000000, 4, 10, 1800, 2200, 1316, 1716, { 20,  0, 1280, 1716}, {0}},
    /* MX01_IMX224_1280_720_25P_HDR  */ {37125000, 594000000, 2, 12, 1500, 2640, 1316, 1766, { 20,  0, 1280, 1766}, {0}},
    /* MX01_IMX224_1280_720_50P_HDR  */ {37125000, 594000000, 4, 10, 1800, 2640, 1316, 1766, { 20,  0, 1280, 1766}, {0}},
    /* MX01_IMX224_1280_960_30P_HDR  */ {37125000, 594000000, 2, 12, 1500, 2200, 1316, 2178, { 20,  0, 1280, 2178}, {0}},
    /* MX01_IMX224_1280_960_25P_HDR  */ {37125000, 594000000, 2, 12, 1500, 2640, 1316, 2178, { 20,  0, 1280, 2178}, {0}},
    /* MX01_IMX224_1280_720_30P_HDR_1*/ {37125000, 594000000, 4, 10, 1800, 4400, 1316, 3123, { 20,  0, 1280, 3123}, {0}},
    /* MX01_IMX224_1280_720_60P_HDR_1*/ {37125000, 594000000, 4, 10, 1800, 2200, 1316, 1716, { 20,  0, 1280, 1716}, {0}},
    /* MX01_IMX224_1280_960_60P_1    */ {37125000, 594000000, 2, 12, 1500, 1100, 1312,  991, { 16, 22, 1280,  960}, {0}},
#else
    /* MX01_IMX224_1280_960_30P      */ {37125000, 594000000, 1, 12, 1500, 1100, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_60P      */ {37125000, 594000000, 2, 12, 1500, 1100, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_120P     */ {37125000, 594000000, 4, 10, 1800, 1100, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_25P      */ {37125000, 594000000, 1, 12, 1500, 1320, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_50P      */ {37125000, 594000000, 2, 12, 1500, 1320, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_100P     */ {37125000, 594000000, 4, 10, 1800, 1320, 1312,  991, { 16, 22, 1280,  960}, {0}},
    /* MX01_IMX224_1280_720_30P_HDR  */ {37125000, 594000000, 2, 12, 1500, 2200, 1316, 1716, { 20,  0, 1280, 1716}, {0}},
    /* MX01_IMX224_1280_720_60P_HDR  */ {37125000, 594000000, 4, 10, 1800, 2200, 1316, 1716, { 20,  0, 1280, 1716}, {0}},
    /* MX01_IMX224_1280_720_25P_HDR  */ {37125000, 594000000, 2, 12, 1500, 2640, 1316, 1766, { 20,  0, 1280, 1766}, {0}},
    /* MX01_IMX224_1280_720_50P_HDR  */ {37125000, 594000000, 4, 10, 1800, 2640, 1316, 1766, { 20,  0, 1280, 1766}, {0}},
    /* MX01_IMX224_1280_960_30P_HDR  */ {37125000, 594000000, 2, 12, 1500, 2200, 1316, 2178, { 20,  0, 1280, 2178}, {0}},
    /* MX01_IMX224_1280_960_25P_HDR  */ {37125000, 594000000, 2, 12, 1500, 2640, 1316, 2178, { 20,  0, 1280, 2178}, {0}},
    /* MX01_IMX224_1280_720_30P_HDR_1*/ {37125000, 594000000, 4, 10, 1800, 4400, 1316, 3123, { 20,  0, 1280, 3123}, {0}},
    /* MX01_IMX224_1280_720_60P_HDR_1*/ {37125000, 594000000, 4, 10, 1800, 2200, 1316, 1716, { 20,  0, 1280, 1716}, {0}},
    /* MX01_IMX224_1280_960_60P_1    */ {37125000, 594000000, 2, 12, 1500, 1100, 1312,  991, { 16, 22, 1280,  960}, {0}},
#endif
};

const AMBA_SENSOR_OUTPUT_INFO_s MX01_IMX224_OutputInfo[MX01_IMX224_NUM_MODE] = {
    /* MX01_IMX224_1280_960_30P      */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1312,  977, { 16,  8, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_60P      */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1312,  977, { 16,  8, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_120P     */ {600000000,  4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1312,  977, { 16,  8, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_25P      */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1312,  977, { 16,  8, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_50P      */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1312,  977, { 16,  8, 1280,  960}, {0}},
    /* MX01_IMX224_1280_960_100P     */ {600000000,  4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1312,  977, { 16,  8, 1280,  960}, {0}},
    /* MX01_IMX224_1280_720_30P_HDR  */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_GB, 1316, 1710, { 20,  0, 1280, 1710}, {0}},
    /* MX01_IMX224_1280_720_60P_HDR  */ {600000000,  4, 10, AMBA_SENSOR_BAYER_PATTERN_GB, 1316, 1710, { 20,  0, 1280, 1710}, {0}},
    /* MX01_IMX224_1280_720_25P_HDR  */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_GB, 1316, 1760, { 20,  0, 1280, 1760}, {0}}, //TBD OB 6->3
    /* MX01_IMX224_1280_720_50P_HDR  */ {600000000,  4, 10, AMBA_SENSOR_BAYER_PATTERN_GB, 1316, 1760, { 20,  0, 1280, 1760}, {0}},
    /* MX01_IMX224_1280_960_30P_HDR  */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_GB, 1316, 2156, { 20,  0, 1280, 2156}, {0}}, //TBD OB 14->3
    /* MX01_IMX224_1280_960_25P_HDR  */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_GB, 1316, 2156, { 20,  0, 1280, 2156}, {0}},
    /* MX01_IMX224_1280_720_30P_HDR_1*/ {600000000,  4, 10, AMBA_SENSOR_BAYER_PATTERN_GB, 1316, 3110, { 20,  0, 1280, 3110}, {0}},
    /* MX01_IMX224_1280_720_60P_HDR_1*/ {600000000,  4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1316, 1716, { 20,  0, 1280, 1716}, {0}},
    /* MX01_IMX224_1280_960_60P_1    */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1312,  991, { 16, 22, 1280,  960}, {0}},
};

const AMBA_SENSOR_INPUT_INFO_s MX01_IMX224_InputInfo[MX01_IMX224_NUM_MODE] = {
    /* MX01_IMX224_1280_960_30P      */ {{ 16,   24, 1280,  960}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_960_60P      */ {{ 16,   24, 1280,  960}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_960_120P     */ {{ 16,   24, 1280,  960}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_960_25P      */ {{ 16,   24, 1280,  960}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_960_50P      */ {{ 16,   24, 1280,  960}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_960_100P     */ {{ 16,   24, 1280,  960}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_720_30P_HDR  */ {{ 16,  144, 1280,  720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_720_60P_HDR  */ {{ 16,  144, 1280,  720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_720_25P_HDR  */ {{ 16,  144, 1280,  720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_720_50P_HDR  */ {{ 16,  144, 1280,  720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_960_30P_HDR  */ {{ 16,   24, 1280,  960}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_960_25P_HDR  */ {{ 16,   24, 1280,  960}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_720_30P_HDR_1*/ {{ 16,  144, 1280,  720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_720_60P_HDR_1*/ {{ 16,  144, 1280,  720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX01_IMX224_1280_960_60P_1    */ {{ 16,   24, 1280,  960}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
};

MX01_IMX224_MODE_INFO_s MX01_IMX224_ModeInfoList[MX01_IMX224_NUM_MODE] = {
    /* MX01_IMX224_1280_960_30P      */ {{37125000, 1125, 1, 1100, 1, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_960_60P      */ {{37125000,  563, 1, 1100, 1, { .Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_960_120P     */ {{37125000,  282, 1, 1100, 1, { .Interlace = 0, .TimeScale = 120000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_960_25P      */ {{37125000, 1125, 1, 1320, 1, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_960_50P      */ {{37125000,  563, 1, 1320, 1, { .Interlace = 0, .TimeScale =     50, .NumUnitsInTick =    1}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_960_100P     */ {{37125000,  282, 1, 1320, 1, { .Interlace = 0, .TimeScale =    100, .NumUnitsInTick =    1}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_720_30P_HDR  */ {{37125000,  563, 1, 2200, 2, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_720_60P_HDR  */ {{37125000,  282, 1, 2200, 2, { .Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_720_25P_HDR  */ {{37125000,  563, 1, 2640, 2, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_720_50P_HDR  */ {{37125000,  282, 1, 2640, 2, { .Interlace = 0, .TimeScale =     50, .NumUnitsInTick =    1}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_960_30P_HDR  */ {{37125000,  563, 1, 2200, 2, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_960_25P_HDR  */ {{37125000,  563, 1, 2640, 2, { .Interlace = 0, .TimeScale =     25, .NumUnitsInTick =    1}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_720_30P_HDR_1*/ {{37125000,  282, 1, 4400, 4, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_720_60P_HDR_1*/ {{37125000,  282, 1, 2200, 2, { .Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
    /* MX01_IMX224_1280_960_60P_1    */ {{37125000,  563, 1, 1100, 1, { .Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
};


const AMBA_SENSOR_HDR_INFO_s MX01_IMX224_HdrInfo[MX01_IMX224_NUM_MODE] = {
    [MX01_IMX224_1280_960_30P] = {0},
    [MX01_IMX224_1280_960_60P] = {0},
    [MX01_IMX224_1280_960_120P] = {0},
    [MX01_IMX224_1280_960_25P] = {0},
    [MX01_IMX224_1280_960_50P] = {0},
    [MX01_IMX224_1280_960_100P] = {0},
    [MX01_IMX224_1280_720_30P_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 11, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 1949,
                .MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1280, 134, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 243,
                .MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0U,
                .MinExposureLine = 0U,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [MX01_IMX224_1280_720_60P_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 11, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 1949,
                .MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1280, 134, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 243,
                .MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0U,
                .MinExposureLine = 0U,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [MX01_IMX224_1280_720_25P_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 11, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 2339,
                .MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1280, 159, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 293,
                .MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0U,
                .MinExposureLine = 0U,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [MX01_IMX224_1280_720_50P_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 11, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                . MaxExposureLine = 2339,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1280, 159, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                . MaxExposureLine = 293,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0U,
                .MinExposureLine = 0U,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [MX01_IMX224_1280_960_30P_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 13, 1280, 960}, //TBD: 14 -> 3
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 2003,
                .MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1280, 109, 1280, 960},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 189,
                .MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0U,
                .MinExposureLine = 0U,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [MX01_IMX224_1280_960_25P_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 13, 1280, 960}, //TBD: 14 -> 3
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 2443,
                .MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1280, 109, 1280, 960},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 189,
                .MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0U,
                .MinExposureLine = 0U,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [MX01_IMX224_1280_720_30P_HDR_1] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 3,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 133, 1280, 720}, //TBD: 14 -> 3
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 4249,
                .MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1280, 177, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 128,
                .MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {2560, 181, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 8,
                .MinExposureLine = 1,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [MX01_IMX224_1280_720_60P_HDR_1] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 20, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 1949,
                .MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1280, 137, 1280, 720},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 243,
                .MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0U,
                .MinExposureLine = 0U,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [MX01_IMX224_1280_960_60P_1] = {0},
};

