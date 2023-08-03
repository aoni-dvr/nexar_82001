/**
 *  @file AmbaSensor_MAX9295_9296_OV9284_CCCCTable.c
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
 *  @details Control APIs of MAX9295_9296 plus OV9284_CCCC CMOS sensor with MIPI interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_OV9284_CCCC.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s MX00_OV9284_DeviceInfo = {
    .UnitCellWidth          = (FLOAT)3.0,
    .UnitCellHeight         = (FLOAT)3.0,
    .NumTotalPixelCols      = 1296,
    .NumTotalPixelRows      = 816,
    .NumEffectivePixelCols  = 1280,
    .NumEffectivePixelRows  = 800,
    .MinAnalogGainFactor    = 1.0f,
    .MaxAnalogGainFactor    = 15.9375f,
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
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .WbGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .StrobeCtrlInfo = {0},
    .HdrIsSupport = 1,
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/

MX00_OV9284_REG_s MX00_OV9284_RegTable[MX00_OV9284_NUM_MODE_REG] = {
    {0x0302, 0x32}, //Initial Seq
    {0x030d, 0x50},
    {0x030e, 0x02},
    {0x3001, 0x00},
    {0x3004, 0x00},
    {0x3005, 0x00},
    {0x3006, 0x08},
    {0x3011, 0x0a},
    {0x3013, 0x18},
    {0x301c, 0xf0},
    {0x3022, 0x01},
    {0x3030, 0x10},
    {0x3039, 0x32},
    {0x303a, 0x00},
    {0x3500, 0x00},
    {0x3501, 0x2a},
    {0x3502, 0x90},
    {0x3503, 0x08},
    {0x3505, 0x8c},
    {0x3507, 0x03},
    {0x3508, 0x00},
    {0x3509, 0x10},
    {0x3610, 0x80},
    {0x3611, 0xa0},
    {0x3620, 0x6e},
    {0x3632, 0x56},
    {0x3633, 0x78},
    {0x3662, 0x05},
    {0x3666, 0x00},
    {0x366f, 0x5a},
    {0x3680, 0x84},
    {0x3712, 0x80},
    {0x372d, 0x22},
    {0x3731, 0x80},
    {0x3732, 0x30},
    {0x3778, 0x00},
    {0x377d, 0x22},
    {0x3788, 0x02},
    {0x3789, 0xa4},
    {0x378a, 0x00},
    {0x378b, 0x4a},
    {0x3799, 0x20},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x00},
    {0x3804, 0x05},
    {0x3805, 0x0f},
    {0x3806, 0x03},
    {0x3807, 0x2f},
    {0x3808, 0x05},
    {0x3809, 0x00},
    {0x380a, 0x02},
    {0x380b, 0xd0},
    {0x380c, 0x02},
    {0x380d, 0xd8},
    {0x380e, 0x03},
    {0x380f, 0x8e},
    {0x3810, 0x00},
    {0x3811, 0x08},
    {0x3812, 0x00},
    {0x3813, 0x08},
    {0x3814, 0x11},
    {0x3815, 0x11},
    {0x3820, 0x40},
    {0x3821, 0x00},
    {0x382c, 0x05},
    {0x382d, 0xb0},
    {0x389d, 0x00},
    {0x3881, 0x42},
    {0x3882, 0x01},
    {0x3883, 0x00},
    {0x3885, 0x02},
    {0x38a8, 0x02},
    {0x38a9, 0x80},
    {0x38b1, 0x00},
    {0x38b3, 0x02},
    {0x38c4, 0x00},
    {0x38c5, 0xc0},
    {0x38c6, 0x04},
    {0x38c7, 0x80},
    {0x3920, 0xff},
    {0x4003, 0x40},
    {0x4008, 0x04},
    {0x4009, 0x0b},
    {0x400c, 0x00},
    {0x400d, 0x07},
    {0x4010, 0x40},
    {0x4043, 0x40},
    {0x4307, 0x30},
    {0x4317, 0x00},
    {0x4501, 0x00},
    {0x4507, 0x00},
    {0x4509, 0x00},
    {0x450a, 0x08},
    {0x4601, 0x04},
    {0x470f, 0x00},
    {0x4f07, 0x00},
    {0x4800, 0x00},
    {0x5000, 0x9f},
    {0x5001, 0x00},
    {0x5e00, 0x00},
    {0x5d00, 0x07},
    {0x5d01, 0x00},
    {0x4f00, 0x04},
    {0x4f10, 0x00},
    {0x4f11, 0x98},
    {0x4f12, 0x0f},
    {0x4f13, 0xc4},
    {0x0100, 0x01},
    {0x3501, 0x38},
    {0x3502, 0x20},
    {0x5000, 0x87},
    {0x380e, 0x0e},
    {0x380f, 0x38},
    {0x3006, 0x08},// 08 ; enable
    {0x3210, 0x00},// 10 ; disable GPIO trigger mode
    {0x3007, 0x02},
    {0x301c, 0x20},// 22 ;enable the clock for group write, [1]rst_grp
    {0x3020, 0x20},// 20 ; strobe logic always on
    {0x3025, 0x02},
    {0x382c, 0x07},
    {0x382d, 0x10},
    {0x3920, 0xff},
    {0x3923, 0x00},// ; delay
    {0x3924, 0x00},// ;
    {0x3925, 0x00},// ; width
    {0x3926, 0x00},
    {0x3927, 0x00},
    {0x3928, 0x80},
    {0x392b, 0x00},
    {0x392c, 0x00},
    {0x392d, 0x02},
    {0x392e, 0xd8},
    {0x392f, 0xcb},
    {0x38b3, 0x07},
    {0x3885, 0x07},
    {0x382b, 0x5a},
    {0x3670, 0x68},
};

const MX00_OV9284_SENSOR_INFO_s MX00_OV9284SensorInfo[MX00_OV9284_NUM_MODE] = {
    /* MX00_OV9284_1280_720_A30P   */ {24000000, 794181818, 2, 10, 1456, 3640, 1280, 720, {0, 0, 1280, 720}, {0}},
};

const AMBA_SENSOR_OUTPUT_INFO_s MX00_OV9284_OutputInfo[MX00_OV9284_NUM_MODE] = {
    /* MX00_OV9284_1280_720_A30P       */ {800000000, 2, 10, AMBA_SENSOR_BAYER_PATTERN_BG, 1280, 720, {0, 0, 1280, 720}, {0}},
};

const AMBA_SENSOR_INPUT_INFO_s MX00_OV9284_InputInfo[MX00_OV9284_NUM_MODE] = {
    /* MX00_OV9284_1280_720_A30P       */ {{0,   0, 1280,720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
};

MX00_OV9284_MODE_INFO_s MX00_OV9284_ModeInfoList[MX00_OV9284_NUM_MODE] = {
    /* MX00_OV9284_1280_720_A30P       */ {{24000000, 1456, 3640, { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001}}, 0.0f,0.0f},
};

const AMBA_SENSOR_HDR_INFO_s MX00_OV9284_HdrInfo[MX00_OV9284_NUM_MODE] = {
    [MX00_OV9284_1280_720_A30P] = {0},
};

