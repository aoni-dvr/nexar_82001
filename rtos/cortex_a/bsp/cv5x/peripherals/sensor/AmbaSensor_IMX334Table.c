/**
 *  @file AmbaSensor_IMX334Table.c
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
 *  @details Control APIs of SONY IMX334 CMOS sensor with MIPI interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_IMX334.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s IMX334DeviceInfo = {
    .UnitCellWidth          = (FLOAT)2.0,
    .UnitCellHeight         = (FLOAT)2.0,
    .NumTotalPixelCols      = 3952,
    .NumTotalPixelRows      = 2320,
    .NumEffectivePixelCols  = 3864,
    .NumEffectivePixelRows  = 2180,
    .MinAnalogGainFactor    = (FLOAT)1.0,
    .MaxAnalogGainFactor    = (FLOAT)31.622777, /* 30 dB */
    .MinDigitalGainFactor   = (FLOAT)1.0,
    .MaxDigitalGainFactor   = (FLOAT)125.89254, /* 42 dB */

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
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },

    .HdrIsSupport = 1,
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings
\*-----------------------------------------------------------------------------------------------*/
IMX334_REG_s IMX334InitRegTable[IMX334_NUM_INIT_REG] = {
    {0x300c, 0x5b}, /* [7:0]: BCWAIT_TIME (INCK=37.125MHz) */
    {0x300d, 0x40}, /* [7:0]: CPWAIT_TIME (INCK=37.125MHz) */
    {0x3288, 0x21},
    {0x328a, 0x02},
    {0x3414, 0x05},
    {0x3416, 0x18},
    {0x35ac, 0x0e},
    {0x3648, 0x01},
    {0x364a, 0x04},
    {0x364c, 0x04},
    {0x3678, 0x01},
    {0x367c, 0x31},
    {0x367e, 0x31},
    {0x3708, 0x02},
    {0x3714, 0x01},
    {0x3715, 0x02},
    {0x3716, 0x02},
    {0x3717, 0x02},
    {0x371c, 0x3d},
    {0x371d, 0x3f},
    {0x372c, 0x00},
    {0x372d, 0x00},
    {0x372e, 0x46},
    {0x372f, 0x00},
    {0x3730, 0x89},
    {0x3731, 0x00},
    {0x3732, 0x08},
    {0x3733, 0x01},
    {0x3734, 0xfe},
    {0x3735, 0x05},
    {0x375d, 0x00},
    {0x375e, 0x00},
    {0x375f, 0x61},
    {0x3760, 0x06},
    {0x3768, 0x1b},
    {0x3769, 0x1b},
    {0x376a, 0x1a},
    {0x376b, 0x19},
    {0x376c, 0x18},
    {0x376d, 0x14},
    {0x376e, 0x0f},
    {0x3776, 0x00},
    {0x3777, 0x00},
    {0x3778, 0x46},
    {0x3779, 0x00},
    {0x377a, 0x08},
    {0x377b, 0x01},
    {0x377c, 0x45},
    {0x377d, 0x01},
    {0x377e, 0x23},
    {0x377f, 0x02},
    {0x3780, 0xd9},
    {0x3781, 0x03},
    {0x3782, 0xf5},
    {0x3783, 0x06},
    {0x3784, 0xa5},
    {0x3788, 0x0f},
    {0x378a, 0xd9},
    {0x378b, 0x03},
    {0x378c, 0xe8},
    {0x378d, 0x05},
    {0x378e, 0x87},
    {0x378f, 0x06},
    {0x3790, 0xf5},
    {0x3792, 0x43},
    {0x3794, 0x7a},
    {0x3796, 0xa1},
    {0x37b0, 0x36}, /* XMASTER pin High: Set to "37h", XMASTER pin Low: Set to "36h" */
    {0x3e04, 0x0e},
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
IMX334_MODE_REG_s IMX334ModeRegTable[IMX334_NUM_READOUT_MODE_REG] = {
    /* Addr,     0,    1,    2,    3,    4,    5,    6,    7,    8,    9*/
    {0x3018, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [3:0]: WINMODE */
    {0x3030, {0xca, 0xca, 0xca, 0xca, 0xc4, 0xc4, 0xca, 0xca, 0xc4, 0xc4}}, /* [7:0]: VMAX_LSB */
    {0x3031, {0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09}}, /* [7:0]: VMAX_MSB */
    {0x3032, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [3:0]: VMAX_HSB */
    {0x3034, {0x4c, 0x4c, 0x26, 0x26, 0xef, 0xef, 0x26, 0x26, 0xef, 0xef}}, /* [7:0]: HMAX_LSB */
    {0x3035, {0x04, 0x04, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01}}, /* [7:0]: HMAX_MSB */
    {0x3050, {0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00}}, /* [7:0]: ABIT */
    {0x314c, {0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0xc0, 0xc0, 0xc0, 0xc0}}, /* [7:0]: INCKSEL1_LSB */
    {0x314d, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [0]:   INCKSEL1_MSB */
    {0x315a, {0x06, 0x06, 0x02, 0x06, 0x02, 0x02, 0x02, 0x06, 0x02, 0x02}}, /* [3:2]: PLL_IF_GC, [1:0] INCKSEL2 */
    {0x3168, {0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68}}, /* [7:0]: INCKSEL3 */
    {0x316a, {0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e}}, /* [1:0]: INCKSEL4 */
    {0x3199, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [5]: VADD, [4]: HADD */
    {0x319d, {0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00}}, /* [0]: MDBIT */
    {0x319e, {0x02, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00}}, /* [0]: SYS_MODE */
    {0x31dd, {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03}}, /* [2:0]: VALID_EXPAND */
    {0x3300, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [1:0]: TCYCLE */
    {0x3308, {0x84, 0x84, 0x84, 0x84, 0x84, 0x73, 0x73, 0x73, 0x73, 0x87}}, /* [7:0]: Y_OUT_SIZE_LSB */
    {0x3309, {0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09}}, /* [4:0]: Y_OUT_SIZE_MSB */
    {0x341c, {0x47, 0x47, 0x47, 0x47, 0xff, 0x47, 0x47, 0x47, 0xff, 0xff}}, /* [7:0]: ADBIT1_LSB */
    {0x341d, {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01}}, /* [0]: ADBIT1_MSB */
    {0x3a01, {0x03, 0x07, 0x03, 0x07, 0x03, 0x07, 0x03, 0x07, 0x03, 0x03}}, /* [2:0]: LANE_MODE */
    {0x3a18, {0x7f, 0x7f, 0xb7, 0x7f, 0xb7, 0x8f, 0xb7, 0x7f, 0xb7, 0xb7}}, /* [7:0]: TCLKPOST_LSB */
    {0x3a19, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: TCLKPOST_MSB */
    {0x3a1a, {0x37, 0x37, 0x67, 0x37, 0x67, 0x4f, 0x67, 0x37, 0x67, 0x67}}, /* [7:0]: TCLKPREPARE_LSB */
    {0x3a1b, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: TCLKPREPARE_MSB */
    {0x3a1c, {0x37, 0x37, 0x6f, 0x37, 0x6f, 0x47, 0x6f, 0x37, 0x6f, 0x6f}}, /* [7:0]: TCLKTRAIL_LSB */
    {0x3a1d, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: TCLKTRAIL_MSB */
    {0x3a1e, {0xf7, 0xf7, 0xdf, 0xf7, 0xdf, 0x37, 0xdf, 0xf7, 0xdf, 0xdf}}, /* [7:0]: TCLKZERO_LSB */
    {0x3a1f, {0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01}}, /* [7:0]: TCLKZERO_MSB */
    {0x3a20, {0x3f, 0x3f, 0x6f, 0x3f, 0x6f, 0x4f, 0x6f, 0x3f, 0x6f, 0x6f}}, /* [7:0]: THSPREPARE_LSB */
    {0x3a21, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: THSPREPARE_MSB */
    {0x3a22, {0x6f, 0x6f, 0xcf, 0x6f, 0xcf, 0x87, 0xcf, 0x6f, 0xcf, 0xcf}}, /* [7:0]: THSZERO_LSB */
    {0x3a23, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: THSZERO_MSB */
    {0x3a24, {0x3f, 0x3f, 0x6f, 0x3f, 0x6f, 0x4f, 0x6f, 0x3f, 0x6f, 0x6f}}, /* [7:0]: THSTRAIL_LSB */
    {0x3a25, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: THSTRAIL_MSB */
    {0x3a26, {0x5f, 0x5f, 0xb7, 0x5f, 0xb7, 0x7f, 0xb7, 0x5f, 0xb7, 0xb7}}, /* [7:0]: THSEXIT_LSB */
    {0x3a27, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: THSEXIT_MSB */
    {0x3a28, {0x2f, 0x2f, 0x5f, 0x2f, 0x5f, 0x3f, 0x5f, 0x2f, 0x5f, 0x5f}}, /* [7:0]: TLPX_LSB */
    {0x3a29, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: TLPX_MSB */
    /* DOL related */
    {0x319f, {0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x02}}, /* [0]: VCEN */
    {0x3048, {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01}}, /* [0]: WDMODE */
    {0x3049, {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01}}, /* [1:0]: WDSEL */
    {0x304a, {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01}}, /* [2:0]: WD_SET1 */
    {0x304b, {0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02}}, /* [3:0]: WD_SET2 */
    {0x304c, {0x14, 0x14, 0x14, 0x14, 0x14, 0x13, 0x13, 0x13, 0x13, 0x00}}, /* [7:0]: OPB_SIZE_V */
    {0x3058, {0x05, 0x05, 0x05, 0x05, 0x05, 0xea, 0xea, 0xea, 0xea, 0xea}}, /* [7:0]: SHR0_LSB */
    {0x3059, {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01}}, /* [7:0]: SHR0_MSB */
    {0x305a, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: SHR0_HSB */
    {0x305c, {0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09}}, /* [7:0]: SHR0_LSB */
    {0x305d, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: SHR0_MSB */
    {0x305e, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: SHR0_HSB */
    {0x3068, {0x11, 0x11, 0x11, 0x11, 0x11, 0xe1, 0xe1, 0xe1, 0xe1, 0xe1}}, /* [7:0]: RHS1_LSB */
    {0x3069, {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01}}, /* [7:0]: RHS1_MSB */
    {0x306a, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: RHS1_HSB */
    {0x306b, {0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44}}, /* [7:0]: RHS2_LSB */
    {0x306c, {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03}}, /* [7:0]: RHS2_MSB */
    {0x306d, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: RHS2_HSB */
    {0x31d7, {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01}}, /* [5:4]: XHSMSKCNT, [3:2] XVSMSKCNT, [1:0] XVSMSKCNT_INT */
};

IMX334_REG_s IMX334NormalVRegTable[2][IMX334_NUM_READOUT_DIRECTION_REG] = {
    [0] = { /* all-pixel, normal readout direction */
        {0x3078, 0x02},
        {0x3079, 0x00},
        {0x307a, 0x00},
        {0x307b, 0x00},
        {0x3080, 0x02},
        {0x3081, 0x00},
        {0x3082, 0x00},
        {0x3083, 0x00},
        {0x3088, 0x02},
        {0x3094, 0x00},
        {0x3095, 0x00},
        {0x3096, 0x00},
        {0x309b, 0x02},
        {0x309c, 0x00},
        {0x309d, 0x00},
        {0x309e, 0x00},
        {0x30a4, 0x00},
        {0x30a5, 0x00},
        {0x3074, 0xb0}, /* [7:0]: AREA3_ST_ADR_1_LSB */
        {0x3075, 0x00}, /* [4:0]: AREA3_ST_ADR_1_MSB */
        {0x308e, 0xb1}, /* [7:0]: AREA3_ST_ADR_2_LSB */
        {0x308f, 0x00}, /* [4:0]: AREA3_ST_ADR_2_MSB */
        {0x30b6, 0x00}, /* [7:0]: UNREAD_PARAM5_LSB */
        {0x30b7, 0x00}, /* [0]: UNREAD_PARAM5_MSB */
        {0x3116, 0x08}, /* [7:0]: UNREAD_PARAM6_LSB */
        {0x3117, 0x00}, /* [0]: UNREAD_PARAM6_MSB */
    },
    [1] = { /* 1080p, normal readout direction */
        {0x3078, 0x04},
        {0x3079, 0xfe},
        {0x307a, 0x04},
        {0x307b, 0x02},
        {0x3080, 0x04},
        {0x3081, 0xfe},
        {0x3082, 0x04},
        {0x3083, 0x02},
        {0x3088, 0x04},
        {0x3094, 0xfe},
        {0x3095, 0x04},
        {0x3096, 0x02},
        {0x309b, 0x04},
        {0x309c, 0xfe},
        {0x309d, 0x04},
        {0x309e, 0x02},
        {0x30a4, 0x33},
        {0x30a5, 0x33},
        {0x3074, 0xc0}, /* [7:0]: AREA3_ST_ADR_1_LSB */
        {0x3075, 0x11}, /* [4:0]: AREA3_ST_ADR_1_MSB */
        {0x308e, 0xc1}, /* [7:0]: AREA3_ST_ADR_2_LSB */
        {0x308f, 0x11}, /* [4:0]: AREA3_ST_ADR_2_MSB */
        {0x30b6, 0xfa}, /* [7:0]: UNREAD_PARAM5_LSB */
        {0x30b7, 0x01}, /* [0]: UNREAD_PARAM5_MSB */
        {0x3116, 0x02}, /* [7:0]: UNREAD_PARAM6_LSB */
        {0x3117, 0x00}, /* [0]: UNREAD_PARAM6_MSB */
    },
};

IMX334_REG_s IMX334InvertedVRegTable[2][IMX334_NUM_READOUT_DIRECTION_REG] = {
    [0] = { /* all-pixel, inverted readout direction */
        {0x3078, 0x02},
        {0x3079, 0x00},
        {0x307a, 0x00},
        {0x307b, 0x00},
        {0x3080, 0xfe},
        {0x3081, 0x00},
        {0x3082, 0x00},
        {0x3083, 0x00},
        {0x3088, 0x02},
        {0x3094, 0x00},
        {0x3095, 0x00},
        {0x3096, 0x00},
        {0x309b, 0xfe},
        {0x309c, 0x00},
        {0x309d, 0x00},
        {0x309e, 0x00},
        {0x30a4, 0x00},
        {0x30a5, 0x00},
        {0x3074, 0xb0}, /* [7:0]: AREA3_ST_ADR_1_LSB */
        {0x3075, 0x00}, /* [4:0]: AREA3_ST_ADR_1_MSB */
        {0x308e, 0xb1}, /* [7:0]: AREA3_ST_ADR_2_LSB */
        {0x308f, 0x00}, /* [4:0]: AREA3_ST_ADR_2_MSB */
        {0x30b6, 0x00}, /* [7:0]: UNREAD_PARAM5_LSB */
        {0x30b7, 0x00}, /* [0]: UNREAD_PARAM5_MSB */
        {0x3116, 0x08}, /* [7:0]: UNREAD_PARAM6_LSB */
        {0x3117, 0x00}, /* [0]: UNREAD_PARAM6_MSB */
    },
    [1] = { /* 1080p, inverted readout direction */
        {0x3078, 0x04},
        {0x3079, 0xfe},
        {0x307a, 0x04},
        {0x307b, 0x02},
        {0x3080, 0xfc},
        {0x3081, 0x02},
        {0x3082, 0xfc},
        {0x3083, 0xfe},
        {0x3088, 0x04},
        {0x3094, 0xfe},
        {0x3095, 0x04},
        {0x3096, 0x02},
        {0x309b, 0xfc},
        {0x309c, 0x02},
        {0x309d, 0xfc},
        {0x309e, 0xfe},
        {0x30a4, 0x33},
        {0x30a5, 0x33},
        {0x3074, 0xc0}, /* [7:0]: AREA3_ST_ADR_1_LSB */
        {0x3075, 0x11}, /* [4:0]: AREA3_ST_ADR_1_MSB */
        {0x308e, 0xc1}, /* [7:0]: AREA3_ST_ADR_2_LSB */
        {0x308f, 0x11}, /* [4:0]: AREA3_ST_ADR_2_MSB */
        {0x30b6, 0xfa}, /* [7:0]: UNREAD_PARAM5_LSB */
        {0x30b7, 0x01}, /* [0]: UNREAD_PARAM5_MSB */
        {0x3116, 0x02}, /* [7:0]: UNREAD_PARAM6_LSB */
        {0x3117, 0x00}, /* [0]: UNREAD_PARAM6_MSB */
    },
};

const AMBA_SENSOR_OUTPUT_INFO_s IMX334OutputInfo[IMX334_NUM_MODE] = {
    /* IMX334_3840_2160_30P_0     */ { 890109888, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2200, { 12, 32, 3840, 2160}, {0}},
    /* IMX334_3840_2160_30P_1     */ { 890109888, 8, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2200, { 12, 32, 3840, 2160}, {0}},
    /* IMX334_3840_2160_60P_0     */ {1780219776, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2200, { 12, 32, 3840, 2160}, {0}},
    /* IMX334_3840_2160_60P_1     */ { 890109888, 8, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2200, { 12, 32, 3840, 2160}, {0}},
    /* IMX334_3840_2160_60P_2     */ {1780219776, 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2200, { 12, 32, 3840, 2160}, {0}},
    /* IMX334_3840_2160_30P_HDR_0 */ {1186813184, 8, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2199, { 12, 31, 3840, 2160}, {0}},
    /* IMX334_3840_2160_30P_HDR_1 */ {1780219776, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2199, { 12, 31, 3840, 2160}, {0}},
    /* IMX334_3840_2160_30P_HDR_2 */ { 890109888, 8, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2199, { 12, 31, 3840, 2160}, {0}},
    /* IMX334_3840_2160_30P_HDR_3 */ {1780219776, 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2199, { 12, 31, 3840, 2160}, {0}},
    /* IMX334_3840_2160_30P_HDR_4 */ {1780219776, 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 4878, { 20,  0, 3840, 4878}, {0}},
};

const AMBA_SENSOR_INPUT_INFO_s IMX334InputInfo[IMX334_NUM_MODE] = {
    /* IMX334_3840_2160_30P_0     */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX334_3840_2160_30P_1     */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX334_3840_2160_60P_0     */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX334_3840_2160_60P_1     */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX334_3840_2160_60P_2     */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX334_3840_2160_30P_HDR_0 */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX334_3840_2160_30P_HDR_1 */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX334_3840_2160_30P_HDR_2 */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX334_3840_2160_30P_HDR_3 */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX334_3840_2160_30P_HDR_4 */ {{ 12, 32, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
};

IMX334_MODE_INFO_s IMX334ModeInfoList[IMX334_NUM_MODE] = {
    /* IMX334_3840_2160_30P_0     */ {{ 37087912, 2, 1100, 2250, 1, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX334_3840_2160_30P_1     */ {{ 37087912, 2, 1100, 2250, 1, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX334_3840_2160_60P_0     */ {{ 37087912, 2,  550, 2250, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX334_3840_2160_60P_1     */ {{ 37087912, 2,  550, 2250, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX334_3840_2160_60P_2     */ {{ 37087912, 2,  495, 2500, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX334_3840_2160_30P_HDR_0 */ {{ 37087912, 2,  495, 2500, 2, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX334_3840_2160_30P_HDR_1 */ {{ 37087912, 2,  550, 2250, 2, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX334_3840_2160_30P_HDR_2 */ {{ 37087912, 2,  550, 2250, 2, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX334_3840_2160_30P_HDR_3 */ {{ 37087912, 2,  495, 2500, 2, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX334_3840_2160_30P_HDR_4 */ {{ 37087912, 2,  495, 2500, 2, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
};

const AMBA_SENSOR_HDR_INFO_s IMX334HdrInfo[IMX334_NUM_MODE] = {
    [IMX334_3840_2160_30P_0] = {0},
    [IMX334_3840_2160_30P_1] = {0},
    [IMX334_3840_2160_60P_0] = {0},
    [IMX334_3840_2160_60P_1] = {0},
    [IMX334_3840_2160_60P_2] = {0},
    [IMX334_3840_2160_30P_HDR_0] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 0, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 4510,
                .MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {3840, 240, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 472,
                .MinExposureLine = 2,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
        },
        .NotSupportIndividualGain = 1,
    },
    [IMX334_3840_2160_30P_HDR_1] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 0, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 4010,
                .MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {3840, 240, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 472,
                .MinExposureLine = 2,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
        },
        .NotSupportIndividualGain = 1,
    },
    [IMX334_3840_2160_30P_HDR_2] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 0, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 4010,
                .MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {3840, 240, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 472,
                .MinExposureLine = 2,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
        },
        .NotSupportIndividualGain = 1,
    },
    [IMX334_3840_2160_30P_HDR_3] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 0, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 4510,
                .MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {3840, 240, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 472,
                .MinExposureLine = 2,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
        },
        .NotSupportIndividualGain = 1,
    },
    [IMX334_3840_2160_30P_HDR_4] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 31, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 4510,
                .MinExposureLine = 2,
            },
            [1] = {
                .EffectiveArea = {3840, 271, 3840, 2160},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .MaxExposureLine = 472,
                .MinExposureLine = 2,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0,
                .MinExposureLine = 0,
            },
        },
        .NotSupportIndividualGain = 1,
    },
};
