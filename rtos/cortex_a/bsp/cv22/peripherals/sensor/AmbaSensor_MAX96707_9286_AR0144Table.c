/**
 *  @file AmbaSensor_MAX96707_9286_AR0144Table.c
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
 *  @details Control APIs of MAXIM SerDes + AR0144 CMOS sensors with Parallel interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_MAX96707_9286_AR0144.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s MX03_AR0144DeviceInfo = {
    .UnitCellWidth          = 3.0f,
    .UnitCellHeight         = 3.0f,
    .NumTotalPixelCols      = 1288,
    .NumTotalPixelRows      = 808,
    .NumEffectivePixelCols  = 1280,
    .NumEffectivePixelRows  = 800,
    .MinAnalogGainFactor    = 1.0f,
    .MaxAnalogGainFactor    = 16.0f,
    .MinDigitalGainFactor   = 1.0f,
    .MaxDigitalGainFactor   = 15.9921875f,

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
    .WbGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .StrobeCtrlInfo = {0},
    .HdrIsSupport           = 0,
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
/* Based on AR0144_Parallel_spreadsheet_Ambarella.xls:
 */
MX03_AR0144_INIT_REG_s MX03_AR0144RecomRegTable[MX03_AR0144_NUM_RECOM_REG] = {
    {0x3ED6, 0x3CB5},
    {0x3ED8, 0x8765},
    {0x3EDA, 0x8888},
    {0x3EDC, 0x97FF},
    {0x3EF8, 0x6522},
    {0x3EFA, 0x2222},
    {0x3EFC, 0x6666},
    {0x3F00, 0xAA05},
    {0x3EE2, 0x180E},
    {0x3EE4, 0x0808},
    {0X3EEA, 0x2A09},
    {0x3060, 0x000D},
    {0x3092, 0x00CF},
    {0X3268, 0x0030},
    {0X3786, 0x0006},
    {0x3F4A, 0x0F70},
    {0x306E, 0x4810},
    {0x3064, 0x1802},
    {0x3EF6, 0x804D},
    {0x3180, 0xC08F},
    {0x30BA, 0x7623},
    {0x3176, 0x0480},
    {0x3178, 0x0480},
    {0x317A, 0x0480},
    {0x317C, 0x0480},

};

MX03_AR0144_INIT_REG_s MX03_AR0144PLLRegTable[MX03_AR0144_NUM_PLL_REG] = {
    {0x302A,      6},
    {0x302C,      1},
    {0x302E,      4},
    {0x3030,     74},    //for 24MHz
    {0x3036,     12},
    {0x3038,      1},
    {0x30B0, 0x0000},
    {0x31AC, 0x0C0C},    //data format bit
    {0x31AE, 0x0200},    //serial format
};


MX03_AR0144_MODE_REG_s MX03_AR0144ModeRegTable[MX03_AR0144_NUM_READOUT_MODE_REG] = {
    {0x3004, {      4,      4,      4,      4} },
    {0x3008, {   1283,   1283,   1283,   1283} },
    {0x3002, {     40,     40,      0,     40} },
    {0x3006, {    759,    759,    799,    759} },
    {0x3040, { 0x0C00, 0x0C00, 0x0C00, 0x0C00} },
    {0x30A2, { 0x0001, 0x0001, 0x0001, 0x0001} },
    {0x30A6, { 0x0001, 0x0001, 0x0001, 0x0001} },
    {0x30A8, { 0x0003, 0x0003, 0x0003, 0x0003} },
    {0x30AE, { 0x0003, 0x0003, 0x0003, 0x0003} },
    {0x300C, {   1488,   1488,   1488,   1488} },
    {0x300A, {   1652,    823,    823,   1652} },
    {0x3012, {   1652,    823,    823,   1652} },
};

MX03_AR0144_AGC_REG_s MX03_AR0144AgcRegTable[MX03_AR0144_NUM_AGC_STEP] = {
    /*
    =========================================================
        The following factor is analog gain.
        Minimum total gain is 1.68x, so data(R0x3060) starts at 0x0B.
        When AGain < 4x, DGain = 1.14x.
    =========================================================
    */
    {  1.52f, 0x0B},
    {  1.60f, 0x0C},
    {  1.68f, 0x0D},
    {  1.78f, 0x0E},
    {  1.88f, 0x0F},
    {  2.00f, 0x10},
    {  2.13f, 0x12},
    {  2.29f, 0x14},
    {  2.46f, 0x16},
    {  2.67f, 0x18},
    {  2.91f, 0x1A},
    {  3.20f, 0x1C},
    /*
    =========================================================
        The following factor is total gain(DGain=1x).
        When AGain >= 4x, DGain need not to be fixed.
    =========================================================
    */
    {  4.00f, 0x20},
    {  4.13f, 0x21},
    {  4.27f, 0x22},
    {  4.41f, 0x23},
    {  4.57f, 0x24},
    {  4.74f, 0x25},
    {  4.92f, 0x26},
    {  5.12f, 0x27},
    {  5.33f, 0x28},
    {  5.57f, 0x29},
    {  5.82f, 0x2A},
    {  6.10f, 0x2B},
    {  6.40f, 0x2C},
    {  6.74f, 0x2D},
    {  7.11f, 0x2E},
    {  7.53f, 0x2F},
    {  8.00f, 0x30},
    {  8.53f, 0x32},
    {  9.14f, 0x34},
    {  9.85f, 0x36},
    { 10.67f, 0x38},
    { 11.64f, 0x3A},
    { 12.80f, 0x3C},
    { 14.22f, 0x3E},
    { 16.00f, 0x40},
};

const MX03_AR0144_SENSOR_INFO_s MX03_AR0144SensorInfo[MX03_AR0144_NUM_MODE] = {
    /* MX03_AR0144_1280_720_30P */ {24000000, 73671608, 12, 12, 1488, 1652, 1280, 720, { 0, 0, 1280, 720}, {0, 0, 0, 0}},
    /* MX03_AR0144_1280_720_60P */ {24000000, 73404036, 12, 12, 1488,  823, 1280, 720, { 0, 0, 1280, 720}, {0, 0, 0, 0}},
    /* MX03_AR0144_1280_800_60P */ {24000000, 73404036, 12, 12, 1488,  823, 1280, 800, { 0, 0, 1280, 800}, {0, 0, 0, 0}},
    /* MX03_AR0144_1280_720_A30P*/ {24000000, 73671608, 12, 12, 1488, 1652, 1280, 720, { 0, 0, 1280, 720}, {0, 0, 0, 0}},
};
const AMBA_SENSOR_OUTPUT_INFO_s MX03_AR0144OutputInfo[MX03_AR0144_NUM_MODE] = {
    /* MX03_AR0144_1280_720_30P */ {884059296, 1, 12, AMBA_SENSOR_BAYER_PATTERN_GR, 1280, 720, { 0, 0, 1280, 720}, {0, 0, 0, 0}},
    /* MX03_AR0144_1280_720_60P */ {880848432, 1, 12, AMBA_SENSOR_BAYER_PATTERN_GR, 1280, 720, { 0, 0, 1280, 720}, {0, 0, 0, 0}},
    /* MX03_AR0144_1280_800_60P */ {880848432, 1, 12, AMBA_SENSOR_BAYER_PATTERN_GR, 1280, 800, { 0, 0, 1280, 800}, {0, 0, 0, 0}},
    /* MX03_AR0144_1280_720_A30P*/ {884059296, 1, 12, AMBA_SENSOR_BAYER_PATTERN_GR, 1280, 720, { 0, 0, 1280, 720}, {0, 0, 0, 0}},
};

const AMBA_SENSOR_INPUT_INFO_s MX03_AR0144InputInfo[MX03_AR0144_NUM_MODE] = {
    /* MX03_AR0144_1280_720_30P */ {{ 4, 40, 1280, 720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* MX03_AR0144_1280_720_60P */ {{ 4, 40, 1280, 720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* MX03_AR0144_1280_800_60P */ {{ 4,  0, 1280, 800}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* MX03_AR0144_1280_720_A30P*/ {{ 4, 40, 1280, 720}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
};

MX03_AR0144_MODE_INFO_s MX03_AR0144ModeInfoList[MX03_AR0144_NUM_MODE] = {
    /* MX03_AR0144_1280_720_30P */ { {.Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* MX03_AR0144_1280_720_60P */ { {.Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* MX03_AR0144_1280_800_60P */ { {.Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* MX03_AR0144_1280_720_A30P*/ { {.Interlace = 0, .TimeScale = 30, .NumUnitsInTick = 1}, 0.0f, 0.0f},
};
