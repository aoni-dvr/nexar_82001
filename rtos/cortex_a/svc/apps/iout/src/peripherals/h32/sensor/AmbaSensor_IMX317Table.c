/**
 *  @file AmbaSensor_IMX317Table.c
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
 *  @details Control APIs of SONY IMX317 CMOS sensor with LVDS interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_IMX317.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s IMX317DeviceInfo = {
    .UnitCellWidth          = (FLOAT)1.62,
    .UnitCellHeight         = (FLOAT)1.62,
    .NumTotalPixelCols      = 3864,
    .NumTotalPixelRows      = 2218,
    .NumEffectivePixelCols  = 3864,
    .NumEffectivePixelRows  = 2202,
    .MinAnalogGainFactor    = (FLOAT)1.0,
    .MaxAnalogGainFactor    = (FLOAT)22.50549451, /* 27.0457712 dB */
    .MinDigitalGainFactor   = (FLOAT)1.0,
    .MaxDigitalGainFactor   = (FLOAT)7.943282347, /* 18 dB */

    .FrameRateCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 2,
        .NumBadFrames           = 1
    },
    .ShutterSpeedCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .AnalogGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .DigitalGainCtrlInfo = {
        .CommunicationTime      = AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK,
        .FirstReflectedFrame    = 1,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },

    .StrobeCtrlInfo = {0},
    .HdrIsSupport = 0U,
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings
\*-----------------------------------------------------------------------------------------------*/
IMX317_REG_s IMX317PlstmgRegTable[IMX317_NUM_READOUT_PSTMG_REG] = {
    {0x004c, 0x00}, /* [7:0]: PLSTMG01 */
    {0x004d, 0x03}, /* [1:0]: PLSTMG02 */
    {0x031c, 0x1a}, /* [7:0]: PLSTMG03_LSB */
    {0x031d, 0x00}, /* [7:0]: PLSTMG03_MSB */
    {0x0502, 0x02}, /* [7:0]: PLSTMG04 */
    {0x0529, 0x0e}, /* [7:0]: PLSTMG05 */
    {0x052a, 0x0e}, /* [7:0]: PLSTMG06 */
    {0x052b, 0x0e}, /* [7:0]: PLSTMG07 */
    {0x0538, 0x0e}, /* [7:0]: PLSTMG08 */
    {0x0539, 0x0e}, /* [7:0]: PLSTMG09 */
    {0x0553, 0x00}, /* [4:0]: PLSTMG10 */
    {0x057d, 0x05}, /* [4:0]: PLSTMG11 */
    {0x057f, 0x05}, /* [4:0]: PLSTMG12 */
    {0x0581, 0x04}, /* [4:0]: PLSTMG13 */
    {0x0583, 0x76}, /* [7:0]: PLSTMG14 */
    {0x0587, 0x01}, /* [7:0]: PLSTMG15 */
    {0x05bb, 0x0e}, /* [7:0]: PLSTMG16 */
    {0x05bc, 0x0e}, /* [7:0]: PLSTMG17 */
    {0x05bd, 0x0e}, /* [7:0]: PLSTMG18 */
    {0x05be, 0x0e}, /* [7:0]: PLSTMG19 */
    {0x05bf, 0x0e}, /* [7:0]: PLSTMG20 */
    {0x066e, 0x00}, /* [4:0]: PLSTMG21 */
    {0x066f, 0x00}, /* [4:0]: PLSTMG22 */
    {0x0670, 0x00}, /* [4:0]: PLSTMG23 */
    {0x0671, 0x00}, /* [4:0]: PLSTMG24 */
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
IMX317_MODE_REG_s IMX317ModeRegTable[IMX317_NUM_READOUT_MODE_REG] = {
    /*       [              Type 1/2.5 8.51M         ]   */
    /* Addr,     C,    0,    1,    2,    3,    4,    6   */
    {0x0003, {0x22, 0x22, 0x00, 0x33, 0x22, 0x22, 0x33}}, /* [3:0]: STBLVDS, [7:0]: LANESEL */
    {0x0004, {0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x04}}, /* [7:0]: MDSEL1 */
    {0x0005, {0x07, 0x07, 0x01, 0x27, 0x21, 0x61, 0x31}}, /* [7:0]: MDSEL2 */
    {0x0006, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: MDSEL3 */
    {0x0007, {0x02, 0x02, 0x02, 0x11, 0x11, 0x19, 0x02}}, /* [7:0]: MDSEL4 */
    {0x000e, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: SVR_LSB */
    {0x000f, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [7:0]: SVR_MSB */
    {0x001a, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, /* [0]: MDVREV */
    {0x006b, {0x07, 0x07, 0x05, 0x07, 0x05, 0x05, 0x05}}, /* MDPLS17 */
    {0x00e2, {0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x04}}, /* [7:0]: VCOUTMODE */
    {0x00ee, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}}, /* [7:0]: PSMOVEN */
    {0x0342, {0xff, 0xff, 0x0a, 0xff, 0x0a, 0x0a, 0x0a}}, /* [7:0]: MDPLS01_LSB */
    {0x0343, {0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00}}, /* [7:0]: MDPLS01_MSB */
    {0x0344, {0xff, 0xff, 0x16, 0xff, 0x1a, 0x1b, 0x1a}}, /* [7:0]: MDPLS02_LSB */
    {0x0345, {0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00}}, /* [7:0]: MDPLS02_MSB */
    {0x03a6, {0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01}}, /* MDPLS16 */
    {0x0528, {0x0f, 0x0f, 0x0e, 0x0f, 0x0e, 0x0e, 0x0e}}, /* [7:0]: MDPLS03 */
    {0x0554, {0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00}}, /* [4:0]: MDPLS04 */
    {0x0555, {0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01}}, /* [4:0]: MDPLS05 */
    {0x0556, {0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01}}, /* [4:0]: MDPLS06 */
    {0x0557, {0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01}}, /* [4:0]: MDPLS07 */
    {0x0558, {0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01}}, /* [4:0]: MDPLS08 */
    {0x0559, {0x1f, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00}}, /* [4:0]: MDPLS09 */
    {0x055a, {0x1f, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00}}, /* [4:0]: MDPLS10 */
    {0x05ba, {0x0f, 0x0f, 0x0e, 0x0f, 0x0e, 0x0e, 0x0e}}, /* [7:0]: MDPLS11 */
    {0x066a, {0x00, 0x00, 0x1b, 0x00, 0x1b, 0x1b, 0x1b}}, /* [4:0]: MDPLS12 */
    {0x066b, {0x00, 0x00, 0x1a, 0x00, 0x1a, 0x19, 0x19}}, /* [4:0]: MDPLS13 */
    {0x066c, {0x00, 0x00, 0x19, 0x00, 0x19, 0x17, 0x17}}, /* [4:0]: MDPLS14 */
    {0x066d, {0x00, 0x00, 0x17, 0x00, 0x17, 0x17, 0x17}}, /* [4:0]: MDPLS15 */
};

const AMBA_SENSOR_OUTPUT_INFO_s IMX317OutputInfo[AMBA_SENSOR_IMX317_NUM_MODE] = {
    /*AMBA_SENSOR_IMX317_3840_2162_30P  */ {576000000,  6, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2218, { 12, 40, 3840, 2162}, {0}},
    /*AMBA_SENSOR_IMX317_3840_2160_30P  */ {576000000,  6, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2218, { 12, 40, 3840, 2160}, {0}},
    /*AMBA_SENSOR_IMX317_3840_2160_60P  */ {576000000, 10, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 3864, 2218, { 12, 40, 3840, 2160}, {0}},
    /*AMBA_SENSOR_IMX317_1920_1080_60P  */ {576000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1932, 1102, {  6, 18, 1920, 1080}, {0}},
    /*AMBA_SENSOR_IMX317_1920_1080_120P */ {576000000,  6, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1932, 1102, {  6, 18, 1920, 1080}, {0}},
    /*AMBA_SENSOR_IMX317_1920_1080_30P  */ {576000000,  6, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1932, 1102, {  6, 18, 1920, 1080}, {0}},
    /*AMBA_SENSOR_IMX317_1280_540_240P  */ {576000000,  4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1288,  550, {  4,  8, 1280,  540}, {0}},
};

const AMBA_SENSOR_INPUT_INFO_s IMX317InputInfo[AMBA_SENSOR_IMX317_NUM_MODE] = {
    /*AMBA_SENSOR_IMX317_3840_2162_30P  */ {{  12,  42, 3840, 2162}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /*AMBA_SENSOR_IMX317_3840_2160_30P  */ {{  12,  42, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /*AMBA_SENSOR_IMX317_3840_2160_60P  */ {{  12,  42, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /*AMBA_SENSOR_IMX317_1920_1080_60P  */ {{  12,  42, 3840, 2160}, { AMBA_SENSOR_SUBSAMPLE_BINNING, 1, 2}, { AMBA_SENSOR_SUBSAMPLE_BINNING, 1,  2}, 1},
    /*AMBA_SENSOR_IMX317_1920_1080_120P */ {{  12,  42, 3840, 2160}, { AMBA_SENSOR_SUBSAMPLE_BINNING, 1, 2}, { AMBA_SENSOR_SUBSAMPLE_BINNING, 1,  2}, 1},
    /*AMBA_SENSOR_IMX317_1920_1080_30P  */ {{  12,  42, 3840, 2160}, { AMBA_SENSOR_SUBSAMPLE_BINNING, 1, 2}, { AMBA_SENSOR_SUBSAMPLE_BINNING, 1,  2}, 1},
    /*AMBA_SENSOR_IMX317_1280_540_240P  */ {{  12,  44, 3840, 2160}, { AMBA_SENSOR_SUBSAMPLE_BINNING, 1, 3}, { AMBA_SENSOR_SUBSAMPLE_BINNING, 1,  4}, 1},
};

IMX317_MODE_INFO_s IMX317ModeInfoList[AMBA_SENSOR_IMX317_NUM_MODE] = {
    /*AMBA_SENSOR_IMX317_3840_2162_30P  */ {{ 72000000, 528, 2, 4550, 1, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /*AMBA_SENSOR_IMX317_3840_2160_30P  */ {{ 72000000, 528, 2, 4550, 1, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /*AMBA_SENSOR_IMX317_3840_2160_60P  */ {{ 72000000, 264, 2, 4550, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
    /*AMBA_SENSOR_IMX317_1920_1080_60P  */ {{ 72000000, 520, 2, 2310, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
    /*AMBA_SENSOR_IMX317_1920_1080_120P */ {{ 72000000, 264, 2, 2275, 1, { .Interlace = 0, .TimeScale = 120000,  .NumUnitsInTick = 1001}}, 0.0f},
    /*AMBA_SENSOR_IMX317_1920_1080_30P  */ {{ 72000000, 264, 2, 9100, 4, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /*AMBA_SENSOR_IMX317_1280_540_240P  */ {{ 72000000, 260, 2, 1155, 1, { .Interlace = 0, .TimeScale = 240000,  .NumUnitsInTick = 1001}}, 0.0f},
};

const AMBA_SENSOR_HDR_INFO_s IMX317HdrInfo[AMBA_SENSOR_IMX317_NUM_MODE] = {0};
