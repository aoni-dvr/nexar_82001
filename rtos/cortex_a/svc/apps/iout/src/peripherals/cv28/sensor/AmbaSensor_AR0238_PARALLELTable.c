/**
 *  [Engineering Version]
 *  @file AmbaSensor_AR0238_PARALLELTable.c
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
 *  @details Control APIs of Aptina AR0238_PARALLEL CMOS sensor with HiSPi interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_AR0238_PARALLEL.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s AR0238_PARALLELDeviceInfo = {
    .UnitCellWidth          = 3.0f,
    .UnitCellHeight         = 3.0f,
    .NumTotalPixelCols      = 1948U,
    .NumTotalPixelRows      = 1116U,
    .NumEffectivePixelCols  = 1928U,
    .NumEffectivePixelRows  = 1088U,
    .MinAnalogGainFactor    = 1.52f,
    .MaxAnalogGainFactor    = 43.2f,
    .MinDigitalGainFactor   = 1.039f,
    .MaxDigitalGainFactor   = 1.039f,

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

    .HdrIsSupport = 0,
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
AR0238_PARALLEL_REG_s AR0238_PARALLELModeRegTable[AR0238_PARALLEL_NUM_READOUT_MODE_REG] = {
    /* PLL Setting: PLL-3 */
    {0x302A, {     8}}, // vt_pix_clk_div
    {0x302C, {     1}}, // vt_sys_clk_div
    {0x302E, {     2}}, // pre_pll_clk_div
    {0x3030, {    44}}, // pll_multiplier
    {0x3036, {    12}}, // op_pix_clk_div
    {0x3038, {     1}}, // op_sys_clk_div

    {0x3004, {    12}}, // x_addr_start
    {0x3008, {  1931}}, // x_addr_end
    {0x3002, {     4}}, // y_addr_start
    {0x3006, {  1083}}, // y_addr_end
    {0x30A2, {     1}}, // x_odd_inc
    {0x30A6, {     1}}, // y_odd_inc
    {0x3040, {0x0000}}, // read mode
    {0x300A, {  1098}}, // frame_length_line
    {0x300C, {  1128}}, // line_length_pck
    {0x3012, {  1095}}, // coarse_integration_time
    {0x3212, {0xffff}}  // coarse_integration_time_T2
};

AR0238_PARALLEL_SEQ_REG_s AR0238_PARALLELLinearSeqRegTbl[AR0238_PARALLEL_NUM_SEQUENCER_LINEAR_REG] = {
    {0x3088,    0x8000},
    {0x3086,    0x4558},
    {0x3086,    0x72A6},
    {0x3086,    0x4A31},
    {0x3086,    0x4342},
    {0x3086,    0x8E03},
    {0x3086,    0x2A14},
    {0x3086,    0x4578},
    {0x3086,    0x7B3D},
    {0x3086,    0xFF3D},
    {0x3086,    0xFF3D},
    {0x3086,    0xEA2A},
    {0x3086,    0x43D },
    {0x3086,    0x102A},
    {0x3086,    0x52A },
    {0x3086,    0x1535},
    {0x3086,    0x2A05},
    {0x3086,    0x3D10},
    {0x3086,    0x4558},
    {0x3086,    0x2A04},
    {0x3086,    0x2A14},
    {0x3086,    0x3DFF},
    {0x3086,    0x3DFF},
    {0x3086,    0x3DEA},
    {0x3086,    0x2A04},
    {0x3086,    0x622A},
    {0x3086,    0x288E},
    {0x3086,    0x36 },
    {0x3086,    0x2A08},
    {0x3086,    0x3D64},
    {0x3086,    0x7A3D},
    {0x3086,    0x444 },
    {0x3086,    0x2C4B},
    {0x3086,    0xA403},
    {0x3086,    0x430D},
    {0x3086,    0x2D46},
    {0x3086,    0x4316},
    {0x3086,    0x2A90},
    {0x3086,    0x3E06},
    {0x3086,    0x2A98},
    {0x3086,    0x5F16},
    {0x3086,    0x530D},
    {0x3086,    0x1660},
    {0x3086,    0x3E4C},
    {0x3086,    0x2904},
    {0x3086,    0x2984},
    {0x3086,    0x8E03},
    {0x3086,    0x2AFC},
    {0x3086,    0x5C1D},
    {0x3086,    0x5754},
    {0x3086,    0x495F},
    {0x3086,    0x5305},
    {0x3086,    0x5307},
    {0x3086,    0x4D2B},
    {0x3086,    0xF810},
    {0x3086,    0x164C},
    {0x3086,    0x955 },
    {0x3086,    0x562B},
    {0x3086,    0xB82B},
    {0x3086,    0x984E},
    {0x3086,    0x1129},
    {0x3086,    0x9460},
    {0x3086,    0x5C19},
    {0x3086,    0x5C1B},
    {0x3086,    0x4548},
    {0x3086,    0x4508},
    {0x3086,    0x4588},
    {0x3086,    0x29B6},
    {0x3086,    0x8E01},
    {0x3086,    0x2AF8},
    {0x3086,    0x3E02},
    {0x3086,    0x2AFA},
    {0x3086,    0x3F09},
    {0x3086,    0x5C1B},
    {0x3086,    0x29B2},
    {0x3086,    0x3F0C},
    {0x3086,    0x3E03},
    {0x3086,    0x3E15},
    {0x3086,    0x5C13},
    {0x3086,    0x3F11},
    {0x3086,    0x3E0F},
    {0x3086,    0x5F2B},
    {0x3086,    0x902B},
    {0x3086,    0x803E},
    {0x3086,    0x62A },
    {0x3086,    0xF23F},
    {0x3086,    0x103E},
    {0x3086,    0x160 },
    {0x3086,    0x29A2},
    {0x3086,    0x29A3},
    {0x3086,    0x5F4D},
    {0x3086,    0x1C2A},
    {0x3086,    0xFA29},
    {0x3086,    0x8345},
    {0x3086,    0xA83E},
    {0x3086,    0x72A },
    {0x3086,    0xFB3E},
    {0x3086,    0x2945},
    {0x3086,    0x8824},
    {0x3086,    0x3E08},
    {0x3086,    0x2AFA},
    {0x3086,    0x5D29},
    {0x3086,    0x9288},
    {0x3086,    0x102B},
    {0x3086,    0x48B },
    {0x3086,    0x1686},
    {0x3086,    0x8D48},
    {0x3086,    0x4D4E},
    {0x3086,    0x2B80},
    {0x3086,    0x4C0B},
    {0x3086,    0x3F36},
    {0x3086,    0x2AF2},
    {0x3086,    0x3F10},
    {0x3086,    0x3E01},
    {0x3086,    0x6029},
    {0x3086,    0x8229},
    {0x3086,    0x8329},
    {0x3086,    0x435C},
    {0x3086,    0x155F},
    {0x3086,    0x4D1C},
    {0x3086,    0x2AFA},
    {0x3086,    0x4558},
    {0x3086,    0x8E00},
    {0x3086,    0x2A98},
    {0x3086,    0x3F0A},
    {0x3086,    0x4A0A},
    {0x3086,    0x4316},
    {0x3086,    0xB43 },
    {0x3086,    0x168E},
    {0x3086,    0x32A },
    {0x3086,    0x9C45},
    {0x3086,    0x783F},
    {0x3086,    0x72A },
    {0x3086,    0x9D3E},
    {0x3086,    0x305D},
    {0x3086,    0x2944},
    {0x3086,    0x8810},
    {0x3086,    0x2B04},
    {0x3086,    0x530D},
    {0x3086,    0x4558},
    {0x3086,    0x3E08},
    {0x3086,    0x8E01},
    {0x3086,    0x2A98},
    {0x3086,    0x8E00},
    {0x3086,    0x76A7},
    {0x3086,    0x77A7},
    {0x3086,    0x4644},
    {0x3086,    0x1616},
    {0x3086,    0xA57A},
    {0x3086,    0x1244},
    {0x3086,    0x4B18},
    {0x3086,    0x4A04},
    {0x3086,    0x4316},
    {0x3086,    0x643 },
    {0x3086,    0x1605},
    {0x3086,    0x4316},
    {0x3086,    0x743 },
    {0x3086,    0x1658},
    {0x3086,    0x4316},
    {0x3086,    0x5A43},
    {0x3086,    0x1645},
    {0x3086,    0x588E},
    {0x3086,    0x32A },
    {0x3086,    0x9C45},
    {0x3086,    0x787B},
    {0x3086,    0x3F07},
    {0x3086,    0x2A9D},
    {0x3086,    0x530D},
    {0x3086,    0x8B16},
    {0x3086,    0x863E},
    {0x3086,    0x2345},
    {0x3086,    0x5825},
    {0x3086,    0x3E10},
    {0x3086,    0x8E01},
    {0x3086,    0x2A98},
    {0x3086,    0x8E00},
    {0x3086,    0x3E10},
    {0x3086,    0x8D60},
    {0x3086,    0x1244},
    {0x3086,    0x4BB9},
    {0x3086,    0x2C2C},
    {0x3086,    0x2C2C},
    {0x3086,    0x2C2C},
};

AR0238_PARALLEL_SEQ_REG_s AR0238_PARALLELDefSetRegTable[AR0238_PARALLEL_NUM_DEFAULT_SETTING_REG] = {
    {0x3064,    0x1802},
    {0x3EEE,    0xA0AA},
    {0x30BA,    0x762C},
    {0x3F4A,    0x0F70},
    {0x309E,    0x016C},
    {0x3092,    0x006F},
    {0x3EE4,    0x9937},
    {0x3EE6,    0x3863},
    {0x3EEC,    0x3B0C},
    {0x3EEA,    0x2839},
    {0x3ECC,    0x4E2D},
    {0x3ED2,    0xFEA6},
    {0x3ED6,    0x2CB3},
    {0x3EEA,    0x2819},
};

AR0238_PARALLEL_SEQ_REG_s AR0238_PARALLELSetupRegTable[AR0238_PARALLEL_NUM_SETUP_REG] = {
    {0x31AE,    0x0301},
    {0x31C6,    0x0006},
    {0x306E,    0x9018},//change from 0x2418->0x9018: increase power driving(default setting, should not lead to row noise problem)
    {0x301A,    0x10D8},
};

AR0238_PARALLEL_AGC_REG_s AR0238_PARALLELAgcRegTable[AR0238_PARALLEL_NUM_AGC_STEP] = {
    /*   factor R0x3060                               */
    {  1.52f, 0x000B, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {   1.6f, 0x000C, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {  1.68f, 0x000D, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {  1.78f, 0x000E, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {  1.88f, 0x000F, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {   2.0f, 0x0010, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {  2.13f, 0x0012, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {  2.29f, 0x0014, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {  2.46f, 0x0016, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {  2.67f, 0x0018, AR0238_PARALLEL_LOW_CONVERSION_GAIN},
    {   2.7f, 0x0000, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  2.79f, 0x0001, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  2.88f, 0x0002, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  2.98f, 0x0003, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  3.09f, 0x0004, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {   3.2f, 0x0005, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  3.32f, 0x0006, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  3.46f, 0x0007, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {   3.6f, 0x0008, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  3.76f, 0x0009, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  3.93f, 0x000A, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  4.11f, 0x000B, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  4.32f, 0x000C, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  4.55f, 0x000D, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {   4.8f, 0x000E, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  5.08f, 0x000F, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {   5.4f, 0x0010, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  5.76f, 0x0012, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  6.17f, 0x0014, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  6.65f, 0x0016, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {   7.2f, 0x0018, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  7.86f, 0x001A, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  8.64f, 0x001C, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {   9.6f, 0x001E, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  10.8f, 0x0020, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 11.52f, 0x0022, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 12.34f, 0x0024, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 13.29f, 0x0026, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  14.4f, 0x0028, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 15.71f, 0x002A, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 17.28f, 0x002C, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  19.2f, 0x002E, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  21.6f, 0x0030, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 23.05f, 0x0032, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 24.69f, 0x0034, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 26.59f, 0x0036, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 28.79f, 0x0038, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 31.43f, 0x003A, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    //{  32.4, 0x003B, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    { 34.56f, 0x003C, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  38.4f, 0x003E, AR0238_PARALLEL_HIGH_CONVERSION_GAIN},
    {  43.2f, 0x0040, AR0238_PARALLEL_HIGH_CONVERSION_GAIN}
};

const AMBA_SENSOR_INPUT_INFO_s AR0238_PARALLELInputInfo[AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE] = {
    /* AR0238_PARALLEL_1920_1080_30P */ {{ 12, 4, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, 1},
};

const AMBA_SENSOR_OUTPUT_INFO_s AR0238_PARALLELOutputInfo[AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE] = {
    /* AR0238_PARALLEL_1920_1080_30P */ { 74238401, 12, 12, AMBA_SENSOR_BAYER_PATTERN_GR, 1920, 1080, { 0, 0, 1920, 1080}, {0, 0, 0, 0}},
};

AR0238_PARALLEL_MODE_INFO_s AR0238_PARALLELModeInfoList[AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE] = {
    /* AMBA_SENSOR_AR0238_PARALLEL_1920_1080_30P */ {{26995782, 1128 * 2, 1098, { .Interlace = 0U, .TimeScale = 30000U, .NumUnitsInTick = 1001U}}, 0.0f},

};

