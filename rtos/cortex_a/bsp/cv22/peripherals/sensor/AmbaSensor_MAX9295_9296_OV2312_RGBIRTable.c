/**
 *  @file AmbaSensor_MX00_OV2312Table.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Control APIs of MX00_OV2312 CMOS sensor with DVP interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_OV2312_RGBIR.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s MX00_OV2312_DeviceInfo = {
    .UnitCellWidth          = (FLOAT)3.0,
    .UnitCellHeight         = (FLOAT)3.0,
    .NumTotalPixelCols      = 1616,
    .NumTotalPixelRows      = 1316,
    .NumEffectivePixelCols  = 1600,
    .NumEffectivePixelRows  = 1300,
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
MX00_OV2312_REG_s MX00_OV2312_RegTable[MX00_OV2312_NUM_MODE_REG] = {
    {0x010c, 0x02},
    {0x010b, 0x01},
    {0x0300, 0x01},
    {0x0302, 0x32},
    {0x0303, 0x00},
    {0x0304, 0x03},
    {0x0305, 0x02},
    {0x0306, 0x01},
    {0x030d, 0x5a},
    {0x030e, 0x04},
    {0x3001, 0x02},
    {0x3004, 0x00},
    {0x3005, 0x00},
    {0x3006, 0x0a},
    {0x3011, 0x0d},
    {0x3014, 0x04},
    {0x301c, 0xf0},
    {0x3020, 0x20},
    {0x302c, 0x00},
    {0x302d, 0x00},
    {0x302e, 0x00},
    {0x302f, 0x03},
    {0x3030, 0x10},
    {0x303f, 0x03},
    {0x3103, 0x00},
    {0x3106, 0x08},
    {0x31ff, 0x01},
    {0x3501, 0x05},
    {0x3502, 0x7c},
    {0x3506, 0x00},
    {0x3507, 0x00},
    {0x3620, 0x67},
    {0x3633, 0x78},
    {0x3662, 0x65},
    {0x3664, 0xb0},
    {0x3666, 0x70},
    {0x3670, 0x68},
    {0x3674, 0x10},
    {0x3675, 0x00},
    {0x367e, 0x90},
    {0x3680, 0x84},
    {0x36a2, 0x04},
    {0x36a3, 0x80},
    {0x36b0, 0x00},
    {0x3700, 0x35},
    {0x3704, 0x39},
    {0x370a, 0x50},
    {0x3712, 0x00},
    {0x3713, 0x02},
    {0x3778, 0x00},
    {0x379b, 0x01},
    {0x379c, 0x10},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x00},
    {0x3804, 0x06},
    {0x3805, 0x4f},
    {0x3806, 0x05},
    {0x3807, 0x23},
    {0x3808, 0x06},
    {0x3809, 0x40},
    {0x380a, 0x05},
    {0x380b, 0x14},
    {0x380c, 0x03},
    {0x380d, 0xa8},
    {0x380e, 0x05},
    {0x380f, 0x88},
    {0x3810, 0x00},
    {0x3811, 0x08},
    {0x3812, 0x00},
    {0x3813, 0x08},
    {0x3814, 0x11},
    {0x3815, 0x11},
    {0x3816, 0x00},
    {0x3817, 0x01},
    {0x3818, 0x00},
    {0x3819, 0x05},
    {0x3820, 0x00},
    {0x3821, 0x00},
    {0x382b, 0x5a},
    {0x382c, 0x0a},
    {0x382d, 0xf8},
    {0x3881, 0x44},
    {0x3882, 0x02},
    {0x3883, 0x8c},
    {0x3885, 0x07},
    {0x389d, 0x03},
    {0x38a6, 0x00},
    {0x38a7, 0x01},
    {0x38b3, 0x07},
    {0x38b1, 0x00},
    {0x38e5, 0x02},
    {0x38e7, 0x00},
    {0x38e8, 0x00},
    {0x3910, 0xff},
    {0x3911, 0xff},
    {0x3912, 0x08},
    {0x3913, 0x00},
    {0x3914, 0x00},
    {0x3915, 0x00},
    {0x391c, 0x00},
    {0x3920, 0xff},
    {0x3921, 0x80},
    {0x3922, 0x00},
    {0x3923, 0x00},
    {0x3924, 0x05},
    {0x3925, 0x00},
    {0x3926, 0x00},
    {0x3927, 0x00},
    {0x3928, 0x00},
    {0x392d, 0x03},
    {0x392e, 0xa8},
    {0x392f, 0x08},
    {0x4001, 0x00},
    {0x4003, 0x40},
    {0x4008, 0x04},
    {0x4009, 0x1b},
    {0x400c, 0x04},
    {0x400d, 0x1b},
    {0x4010, 0xf4},
    {0x4011, 0x00},
    {0x4016, 0x00},
    {0x4017, 0x04},
    {0x4042, 0x11},
    {0x4043, 0x70},
    {0x4045, 0x00},
    {0x4409, 0x5f},
    {0x4509, 0x00},
    {0x450b, 0x00},
    {0x4600, 0x00},
    {0x4601, 0xa0},
    {0x4708, 0x09},
    {0x470c, 0x81},
    {0x4710, 0x06},
    {0x4711, 0x00},
    {0x4800, 0x00},
    {0x481f, 0x30},
    {0x4837, 0x14},
    {0x4f00, 0x00},
    {0x4f07, 0x00},
    {0x4f08, 0x03},
    {0x4f09, 0x08},
    {0x4f0c, 0x05},
    {0x4f0d, 0xb4},
    {0x4f10, 0x00},
    {0x4f11, 0x00},
    {0x4f12, 0x07},
    {0x4f13, 0xe2},
    {0x5000, 0x9f},
    {0x5001, 0x20},
    {0x5026, 0x00},
    {0x5c00, 0x00},
    {0x5c01, 0x2c},
    {0x5c02, 0x00},
    {0x5c03, 0x7f},
    {0x5e00, 0x00},
    {0x5e01, 0x41},
    {0x38b1, 0x02},
    {0x3880, 0x00},
    {0x5000, 0x97},
    {0x380e, 0x0b},
    {0x380f, 0x10},
    {0x3006, 0x08},//IR LED strobe control
    {0x3004, 0x02},
    {0x3007, 0x02},
    {0x301c, 0x20},
    {0x3020, 0x20},
    {0x3025, 0x02},
    {0x382c, 0x0a},
    {0x382d, 0xf8},
    {0x3920, 0xff},
    {0x3921, 0x00},
    {0x3923, 0x00},
    {0x3924, 0x00},
    {0x3925, 0x00},
    {0x3926, 0x00},
    {0x3927, 0x00},
    {0x3928, 0x00},
    {0x392b, 0x00},
    {0x392c, 0x00},
    {0x392d, 0x03},
    {0x392e, 0xa8},
    {0x392f, 0x0b},
    {0x38b3, 0x07},
    {0x3885, 0x07},
    {0x382b, 0x3a},
    {0x3670, 0x68},
};
MX00_OV2312_REG_s MX00_OV2312_HDRegTable[MX00_OV2312_NUM_HD_MODE_REG] = {
    {0x3800, 0x00},
    {0x3801, 0xA0},
    {0x3802, 0x01},
    {0x3803, 0x22},
    {0x3804, 0x06},
    {0x3805, 0x4f},
    {0x3806, 0x05},
    {0x3807, 0x23},
    {0x3808, 0x05},
    {0x3809, 0x00},
    {0x380a, 0x02},
    {0x380b, 0xd0},
};
const MX00_OV2312_SENSOR_INFO_s MX00_OV2312SensorInfo[MX00_OV2312_NUM_MODE] = {
    /* MX00_OV2312_1280_800_A30P   */ {24000000, 800000000, 2, 10, 1872, 2832, 1280, 720, {0, 0, 1280, 720}, {0}},
    /* MX00_OV2312_1600_1300_A30P   */ {24000000, 800000000, 2, 10, 1872, 2832, 1600, 1300, {0, 0, 1600, 1300}, {0}},
    /* MX00_OV2312_1280_720_A60P   */ {24000000, 800000000, 2, 10, 1872, 1416, 1280, 720, {0, 0, 1280, 720}, {0}},
    /* MX00_OV2312_1600_1300_A60P   */ {24000000, 800000000, 2, 10, 1872, 1416, 1600, 1300, {0, 0, 1600, 1300}, {0}},
    /* MX00_OV2312_1600_1300_A60P_1 */ {24000000, 800000000, 2, 10, 1872, 1416, 1600, 1300, {0, 0, 1600, 1300}, {0}},
};

const AMBA_SENSOR_OUTPUT_INFO_s MX00_OV2312_OutputInfo[MX00_OV2312_NUM_MODE] = {
    /* MX00_OV2312_1280_800_A30P       */ {800000000, 2, 10, AMBA_VIN_BAYER_PATTERN_BGGI, 1280, 720, {0, 0, 1280, 720}, {0}},
    /* MX00_OV2312_1600_1300_A30P       */ {800000000, 2, 10, AMBA_VIN_BAYER_PATTERN_BGGI, 1600, 1300, {0, 0, 1600, 1300}, {0}},
    /* MX00_OV2312_1280_720_A60P       */ {800000000, 2, 10, AMBA_VIN_BAYER_PATTERN_BGGI, 1280, 720, {0, 0, 1280, 720}, {0}},
    /* MX00_OV2312_1600_1300_A60P       */ {800000000, 2, 10, AMBA_VIN_BAYER_PATTERN_BGGI, 1600, 1300, {0, 0, 1600, 1300}, {0}},
    /* MX00_OV2312_1600_1300_A60P_1     */ {800000000, 2, 10, AMBA_VIN_BAYER_PATTERN_BGGI, 1600, 1300, {0, 0, 1600, 1300}, {0}},
};

const AMBA_SENSOR_INPUT_INFO_s MX00_OV2312_InputInfo[MX00_OV2312_NUM_MODE] = {
    /* MX00_OV2312_1280_800_A30P       */ {{0,   0, 1280,720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* MX00_OV2312_1600_1300_A30P       */ {{0,   0, 1600,1300}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* MX00_OV2312_1280_720_A60P       */ {{0,   0, 1280,720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* MX00_OV2312_1600_1300_A60P       */ {{0,   0, 1600,1300}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* MX00_OV2312_1600_1300_A60P_1     */ {{0,   0, 1600,1300}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
};

MX00_OV2312_MODE_INFO_s MX00_OV2312_ModeInfoList[MX00_OV2312_NUM_MODE] = {
    /* MX00_OV2312_1280_800_A30P       */ {{24000000, 1872, 2832, { .Interlace = 0, .TimeScale = 30, .NumUnitsInTick = 1}}, 0.0f,0.0f},
    /* MX00_OV2312_1600_1300_A30P       */ {{24000000, 1872, 2832, { .Interlace = 0, .TimeScale = 30, .NumUnitsInTick = 1}}, 0.0f,0.0f},
    /* MX00_OV2312_1280_720_A60P       */ {{24000000, 1872, 1416, { .Interlace = 0, .TimeScale = 60, .NumUnitsInTick = 1}}, 0.0f,0.0f},
    /* MX00_OV2312_1600_1300_A60P       */ {{24000000, 1872, 1416, { .Interlace = 0, .TimeScale = 60, .NumUnitsInTick = 1}}, 0.0f,0.0f},
    /* MX00_OV2312_1600_1300_A60P_1     */ {{24000000, 1872, 1416, { .Interlace = 0, .TimeScale = 60, .NumUnitsInTick = 1}}, 0.0f,0.0f},
};

const AMBA_SENSOR_HDR_INFO_s MX00_OV2312_HdrInfo[MX00_OV2312_NUM_MODE] = {
    [MX00_OV2312_1280_800_A30P] = {0},
    [MX00_OV2312_1600_1300_A30P] = {0},
    [MX00_OV2312_1280_720_A60P] = {0},
    [MX00_OV2312_1600_1300_A60P] = {0},
    [MX00_OV2312_1600_1300_A60P_1] = {0},
};

