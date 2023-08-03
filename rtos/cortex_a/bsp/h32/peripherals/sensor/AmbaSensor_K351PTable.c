/**
 *  @file AmbaSensor_K351PTable.c
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
 *  @details Control APIs of SOI K351P CMOS sensor with MIPI interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_K351P.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s K351PDeviceInfo = {
    .UnitCellWidth          = (FLOAT)2.0,
    .UnitCellHeight         = (FLOAT)2.0,
    .NumTotalPixelCols      = 2008,
    .NumTotalPixelRows      = 2008,
    .NumEffectivePixelCols  = 2000,
    .NumEffectivePixelRows  = 2000,
    .MinAnalogGainFactor    = 1.0f,
    .MaxAnalogGainFactor    = (FLOAT)31.0,  /*  dB */
    .MinDigitalGainFactor   = 1.0f,
    .MaxDigitalGainFactor   = (FLOAT)1.0,       /*  dB */

    .FrameRateCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 3,
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
K351P_REG_s K351PRegTable[K351P_NUM_READOUT_MODE_REG] = {
    {0x12,  0x40},
	{0xAD,  0x01},
    {0xAD,  0x00},
    {0x0E,  0x11},
    {0x0F,  0x0C},
    {0x10,  0x3F},
    {0x0C,  0x80},
    {0x67,  0xA2},
    {0x0D,  0x21},
    {0x64,  0x01},
    {0x65,  0x9B},
    {0xBE,  0x18},
    {0xBF,  0x60},
    {0xBC,  0xC0},
    {0x20,  0x2C},
    {0x21,  0x01},
    {0x22,  0x34},
    {0x23,  0x08},
    {0x24,  0xF4},
    {0x25,  0xD0},
    {0x26,  0x71},
    {0x27,  0x0C},
    {0x28,  0x0D},
    {0x29,  0x00},
    {0x2B,  0x10},
    {0x2C,  0x00},
    {0x2D,  0x06},
    {0x2E,  0xFB},
    {0x2F,  0x14},
    {0x30,  0xF8},
    {0x87,  0xC5},
    {0x9D,  0xB9},
    {0xAC,  0x00},
    {0x1D,  0x00},
    {0x1E,  0x10},
    {0x3A,  0xD5},
    {0x3B,  0x9B},
    {0x3C,  0x6D},
    {0x3D,  0x59},
    {0x3E,  0x12},
    {0x3F,  0x14},
    {0x42,  0x11},
    {0x43,  0x00},
    {0x70,  0xA0},
    {0x71,  0x24},
    {0x76,  0x08},
    {0x31,  0x04},
    {0x32,  0x04},
    {0x33,  0xCC},
    {0x78,  0x41},
    {0xB0,  0x16},
    {0xB1,  0xA0},
    {0xB2,  0x24},
    {0xB3,  0x14},
    {0xB5,  0x46},
    {0xB6,  0x4F},
    {0xB8,  0x06},
    {0xB9,  0x08},
    {0xBA,  0x8B},
    {0xBB,  0x8B},
    {0xC3,  0x90},
    {0xF9,  0x00},
    {0x56,  0xB2},
    {0x57,  0xC0},
    {0x58,  0x42},
    {0x59,  0x94},
    {0x5A,  0x40},
    {0x5B,  0x10},
    {0x5C,  0x10},
    {0x5D,  0x49},
    {0x60,  0xC0},
    {0x61,  0x42},
    {0x62,  0x94},
    {0x68,  0x00},
    {0x69,  0x10},
    {0xA5,  0x08},
    {0xAA,  0x00},
    {0xC4,  0x42},
    {0xEB,  0x15},
    {0xEC,  0x03},
    {0xE1,  0xFA},
    {0x80,  0x81},
    {0x81,  0x44},
    {0xFB,  0x20},
    {0xFC,  0x32},
    {0xFA,  0x01},
    {0x16,  0xFF},
    {0x17,  0x08},
    {0x49,  0x10},
    {0x85,  0x00},
    {0xB4,  0x01},
    {0xD3,  0x2D},
    {0xD2,  0x80},
    {0xD0,  0x00},
    {0x39,  0x88},
    {0xFF,  0x01},
    {0x74,  0x04},
    {0xFF,  0x00},
    {0x89,  0x00},
    {0x12,  0x00},
};




const AMBA_SENSOR_OUTPUT_INFO_s K351POutputInfo[K351P_NUM_MODE] = {
    /* K351P_2000_2000_30P_10BIT     */{ 756000000, 2, 10, AMBA_SENSOR_BAYER_PATTERN_BG, 2000, 2000, { 0, 0, 2000, 2000}, {0}},
};

const AMBA_SENSOR_INPUT_INFO_s K351PInputInfo[K351P_NUM_MODE] = {
    /* K351P_2000_2000_30P_10BIT     */{{   0,   0, 2000, 2000}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
};

K351P_MODE_INFO_s K351PModeInfoList[K351P_NUM_MODE] = {
    /* K351P_2000_2000_30P_10BIT       */{{ 24000000, 2400, 2100, {.Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1000}}, 0.0f},
};

const AMBA_SENSOR_HDR_INFO_s K351PHdrInfo[K351P_NUM_MODE] = {
    [K351P_2000_2000_30P_10BIT] = {0},


};

