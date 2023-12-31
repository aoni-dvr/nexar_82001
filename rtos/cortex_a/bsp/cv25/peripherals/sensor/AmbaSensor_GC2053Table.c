/**
 *  @file AmbaSensor_GC2053Table.c
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
 *  @details Control APIs of OmniVision GC2053 CMOS sensor with MIPI interface
 *
 */

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

//#include "AmbaDataType.h"
//#include "AmbaDSP.h"
#include "AmbaSensor.h"
#include "AmbaSensor_GC2053.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s GC2053DeviceInfo = {
    .UnitCellWidth          = 2.8,
    .UnitCellHeight         = 2.8,
    .NumTotalPixelCols      = 1920,
    .NumTotalPixelRows      = 1080,
    .NumEffectivePixelCols  = 1920,
    .NumEffectivePixelRows  = 1080,
    .MinAnalogGainFactor    = 1.0,
    .MaxAnalogGainFactor    = 110.5,
    .MinDigitalGainFactor   = 1.0,
    .MaxDigitalGainFactor   = 16.0,

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
};


/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
GC2053_REG_s GC2053RegTable[GC2053_NUM_READOUT_MODE_REG] = {
    /*  1080P30 */
	{0xfe,{0x80, 0x80}},
    {0xfe,{0x80, 0x80}},
    {0xfe,{0x80, 0x80}},
    {0xfe,{0x00, 0x00}},
    {0xf2,{0x00, 0x00}},
    {0xf3,{0x00, 0x00}},
    {0xf4,{0x36, 0x36}},
    {0xf5,{0xc0, 0xc0}},
    {0xf6,{0x44, 0x44}},
    {0xf7,{0x01, 0x01}},
    {0xf8,{0x63, 0x63}},
    {0xf9,{0x40, 0x40}},
    {0xfc,{0x8e, 0x8e}},
	{0xfe,{0x00, 0x00}},
    {0x87,{0x18, 0x18}},
    {0xee,{0x30, 0x30}},
    {0xd0,{0xb7, 0xb7}},
    {0x03,{0x04, 0x04}},
    {0x04,{0x60, 0x60}},
    {0x05,{0x04, 0x05}},
    {0x06,{0x4c, 0x28}},
    {0x07,{0x00, 0x00}},
    {0x08,{0x11, 0x11}},
    {0x09,{0x00, 0x00}},
    {0x0a,{0x02, 0x02}},
    {0x0b,{0x00, 0x00}},
    {0x0c,{0x02, 0x02}},
    {0x0d,{0x04, 0x04}},
    {0x0e,{0x40, 0x40}},
    {0x12,{0xe2, 0xe2}},
    {0x13,{0x16, 0x16}},
    {0x19,{0x0a, 0x0a}},
    {0x21,{0x1c, 0x1c}},
    {0x28,{0x0a, 0x0a}},
    {0x29,{0x24, 0x24}},
    {0x2b,{0x04, 0x04}},
    {0x32,{0xf8, 0xf8}},
    {0x37,{0x03, 0x03}},
    {0x39,{0x15, 0x15}},
    {0x43,{0x07, 0x07}},
    {0x44,{0x40, 0x40}},
    {0x46,{0x0b, 0x0b}},
    {0x4b,{0x20, 0x20}},
    {0x4e,{0x08, 0x08}},
    {0x55,{0x20, 0x20}},
    {0x66,{0x05, 0x05}},
    {0x67,{0x05, 0x05}},
    {0x77,{0x01, 0x01}},
    {0x78,{0x00, 0x00}},
    {0x7c,{0x93, 0x93}},
    {0x8c,{0x12, 0x12}},
    {0x8d,{0x92, 0x92}},
    {0x90,{0x00, 0x00}},
	{0x41,{0x04, 0x04}},
	{0x42,{0x65, 0x65}},
    {0x9d,{0x10, 0x10}},
    {0xce,{0x7c, 0x7c}},
    {0xd2,{0x41, 0x41}},
    {0xd3,{0xdc, 0xdc}},
    {0xe6,{0x50, 0x50}},
	{0xb6,{0xc0, 0xc0}},
    {0xb0,{0x70, 0x70}},
    {0xb1,{0x01, 0x01}},
    {0xb2,{0x00, 0x00}},
    {0xb3,{0x00, 0x00}},
    {0xb4,{0x00, 0x00}},
    {0xb8,{0x01, 0x01}},
    {0xb9,{0x00, 0x00}},
    {0x26,{0x30, 0x30}},
    {0xfe,{0x01, 0x01}},
    {0x40,{0x23, 0x23}},
    {0x55,{0x07, 0x07}},
    {0x60,{0x40, 0x40}},
    {0xfe,{0x04, 0x04}},
    {0x14,{0x78, 0x78}},
    {0x15,{0x78, 0x78}},
    {0x16,{0x78, 0x78}},
    {0x17,{0x78, 0x78}},
    {0xfe,{0x01, 0x01}},
    {0x92,{0x00, 0x00}},
    {0x94,{0x03, 0x03}},
    {0x95,{0x04, 0x04}},
    {0x96,{0x38, 0x38}},
    {0x97,{0x07, 0x07}},
    {0x98,{0x80, 0x80}},
	{0xfe,{0x01, 0x01}},
    {0x01,{0x05, 0x05}},
    {0x02,{0x89, 0x89}},
    {0x04,{0x01, 0x01}},
    {0x07,{0xa6, 0xa6}},
    {0x08,{0xa9, 0xa9}},
    {0x09,{0xa8, 0xa8}},
    {0x0a,{0xa7, 0xa7}},
    {0x0b,{0xff, 0xff}},
    {0x0c,{0xff, 0xff}},
    {0x0f,{0x00, 0x00}},
    {0x50,{0x1c, 0x1c}},
    {0x89,{0x03, 0x03}},
    {0xfe,{0x04, 0x04}},
    {0x28,{0x86, 0x86}},
    {0x29,{0x86, 0x86}},
    {0x2a,{0x86, 0x86}},
    {0x2b,{0x68, 0x68}},
    {0x2c,{0x68, 0x68}},
    {0x2d,{0x68, 0x68}},
    {0x2e,{0x68, 0x68}},
    {0x2f,{0x68, 0x68}},
    {0x30,{0x4f, 0x4f}},
    {0x31,{0x68, 0x68}},
    {0x32,{0x67, 0x67}},
    {0x33,{0x66, 0x66}},
    {0x34,{0x66, 0x66}},
    {0x35,{0x66, 0x66}},
    {0x36,{0x66, 0x66}},
    {0x37,{0x66, 0x66}},
    {0x38,{0x62, 0x62}},
    {0x39,{0x62, 0x62}},
    {0x3a,{0x62, 0x62}},
    {0x3b,{0x62, 0x62}},
    {0x3c,{0x62, 0x62}},
    {0x3d,{0x62, 0x62}},
    {0x3e,{0x62, 0x62}},
    {0x3f,{0x62, 0x62}},
	{0xfe,{0x01, 0x01}},
    {0x9a,{0x06, 0x06}},
    {0xfe,{0x00, 0x00}},
    {0x7b,{0x2a, 0x2a}},
    {0x23,{0x2d, 0x2d}},
    {0xfe,{0x03, 0x03}},
    {0x01,{0x27, 0x27}},
    {0x02,{0x5f, 0x5f}},
    {0x03,{0xce, 0xce}},
    {0x12,{0x80, 0x80}},
    {0x13,{0x07, 0x07}},
    {0x15,{0x12, 0x12}},
};


UINT8 Gc2053RegValTable[GC2053_NUM_AGC_STEP][4] = {
    {0x00, 0x00,0x01,0x00},
    {0x00, 0x10,0x01,0x0c},
    {0x00, 0x20,0x01,0x1b},
    {0x00, 0x30,0x01,0x2c},
    {0x00, 0x40,0x01,0x3f},
    {0x00, 0x50,0x02,0x16},
    {0x00, 0x60,0x02,0x35},
    {0x00, 0x70,0x03,0x16},
    {0x00, 0x80,0x04,0x02},
    {0x00, 0x90,0x04,0x31},
    {0x00, 0xa0,0x05,0x32},
    {0x00, 0xb0,0x06,0x35},
    {0x00, 0xc0,0x08,0x04},
    {0x00, 0x5a,0x09,0x19},
    {0x00, 0x83,0x0b,0x0f},
    {0x00, 0x93,0x0d,0x12},
    {0x00, 0x84,0x10,0x00},
    {0x00, 0x94,0x12,0x3a},
    {0x01, 0x2c,0x1a,0x02},
    {0x01, 0x3c,0x1b,0x20},
    {0x00, 0x8c,0x20,0x0f},
    {0x00, 0x9c,0x26,0x07},
    {0x02, 0x64,0x36,0x21},
    {0x02, 0x74,0x37,0x3a},
    {0x00, 0xc6,0x3d,0x02},
    {0x00, 0xdc,0x3f,0x3f},
    {0x02, 0x85,0x3f,0x3f},
    {0x02, 0x95,0x3f,0x3f},
    {0x00, 0xce,0x3f,0x3f},
};
						
UINT32 Gc2053GainLevelTable[GC2053_NUM_AGC_STEP+1] = {
   64 ,
   74 ,
   89 ,
   102,
   127,
   147,
   177,
   203,
   260,
   300,
   361,
   415,
   504,
   581,
   722,
   832,
   1027,
   1182,
   1408,
   1621,
   1990,
   2291,
   2850,
   3282,
   4048,
   5180,
   5500,
   6744,
   7073,
   0xffffffff,
};


const AMBA_SENSOR_INPUT_INFO_s GC2053InputInfo[] = {
/* GC2053_1920_1080_30P     */ {{0, 0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
/* GC2053_1920_1080_25P     */ {{0, 0, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
};

const AMBA_SENSOR_OUTPUT_INFO_s GC2053OutputInfo[] = {
/* GC2053_1920_1080_30P      */ { 594000000, 2, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 1080, { 0, 0, 1920, 1080}, {0}},
/* GC2053_1920_1080_25P      */ { 594000000, 2, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 1080, { 0, 0, 1920, 1080}, {0}},
};

GC2053_MODE_INFO_s GC2053ModeInfoList[AMBA_SENSOR_GC2053_NUM_MODE] = {
/* AMBA_SENSOR_GC2053_1920_1080_30P     */ { GC2053_1920_1080_30P,     {24000000, 2200, 1125, { .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1000}}},
/* AMBA_SENSOR_GC2053_1920_1080_25P     */ { GC2053_1920_1080_25P,     {24000000, 2640, 1125, { .Interlace = 0, .TimeScale =  25000, .NumUnitsInTick = 1000}}},
};
