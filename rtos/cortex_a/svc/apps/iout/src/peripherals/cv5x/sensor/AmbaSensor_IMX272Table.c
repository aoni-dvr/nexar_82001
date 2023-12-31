/**
 *  @file AmbaSensor_IMX272Table.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details Control APIs of SONY IMX272 CMOS sensor with SLVS-EC interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_IMX272.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s IMX272DeviceInfo = {
    .UnitCellWidth          = (FLOAT)3.30,
    .UnitCellHeight         = (FLOAT)3.30,
    .NumTotalPixelCols      = 5388,
    .NumTotalPixelRows      = 4040,
    .NumEffectivePixelCols  = 5324,
    .NumEffectivePixelRows  = 3996,
    .MinAnalogGainFactor    = (FLOAT)1.0,
    .MaxAnalogGainFactor    = (FLOAT)16.05882353, /* 24.1142745 dB */
    .MinDigitalGainFactor   = (FLOAT)1.0,
    .MaxDigitalGainFactor   = (FLOAT)1.0,         /* 0 dB */

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
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
IMX272_MODE_REG_s IMX272ModeRegTable[IMX272_NUM_READOUT_MODE_REG] = {
    /*CID_A,     0,    1,    4,    5,    9,   10,   11,   12 */
    {0x0200, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0201, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0202, {0x00, 0x01, 0x04, 0x05, 0x09, 0x0A, 0x0B, 0x0C}},
    {0x0204, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // Set 4:3 all-pixel mode to rolling shutter by default
    {0x020A, {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {0x020E, {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}},
    {0x0210, {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01}},
    {0x0217, {0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x3F, 0x3F, 0x3F}},
    {0x0219, {0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x021C, {0x42, 0x42, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12}},
    {0x0223, {0x06, 0x06, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {0x0226, {0x80, 0x84, 0x80, 0x84, 0x80, 0x80, 0x80, 0x80}},
    {0x0227, {0x40, 0x40, 0x40, 0x40, 0x5B, 0x40, 0x40, 0x40}},
    {0x0229, {0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00}},
    {0x0264, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0270, {0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0}},
    {0x0277, {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02}},
    {0x027E, {0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C}},
    {0x027F, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x029D, {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02}},
    {0x02A9, {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}},
    {0x02AD, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}},
    {0x02B2, {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22}},
    {0x02B5, {0x22, 0x12, 0x22, 0x12, 0x12, 0x12, 0x12, 0x12}},
    {0x02BF, {0x57, 0x57, 0x57, 0x57, 0x51, 0x51, 0x51, 0x51}},
    {0x02C0, {0x55, 0x55, 0x55, 0x55, 0x44, 0x44, 0x44, 0x44}},
    {0x02C3, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x02C4, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x02D2, {0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E, 0x6E}},
    {0x02DA, {0x07, 0x06, 0x07, 0x06, 0x04, 0x04, 0x04, 0x04}},
    {0x02DB, {0x07, 0x06, 0x07, 0x06, 0x04, 0x04, 0x04, 0x04}},
    {0x02DD, {0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44}},
    {0x02E3, {0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB}},
    {0x031C, {0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28}},
    {0x031D, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x031E, {0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C}},
    {0x0320, {0x08, 0x08, 0x08, 0x08, 0x0D, 0x0D, 0x0D, 0x0D}},
    {0x03F0, {0x07, 0x06, 0x07, 0x06, 0x05, 0x05, 0x05, 0x05}},
    {0x03F1, {0x07, 0x06, 0x07, 0x06, 0x05, 0x05, 0x05, 0x05}},
    {0x03F4, {0x07, 0x06, 0x07, 0x06, 0x05, 0x05, 0x05, 0x05}},
    {0x03F5, {0x07, 0x06, 0x07, 0x06, 0x05, 0x05, 0x05, 0x05}},
    {0x03F9, {0x77, 0x77, 0x77, 0x77, 0xBB, 0xBB, 0xBB, 0xBB}},
    {0x042D, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {0x042E, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}},
    {0x0439, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {0x043A, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}},
    {0x0441, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {0x0442, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}},
    {0x0473, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x04B5, {0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD}},
    {0x04B9, {0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74}},
    {0x04FB, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0503, {0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23}},
    {0x0507, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0508, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x050B, {0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23}},
    {0x0523, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0524, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0537, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {0x0538, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}},
    {0x0575, {0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35}},
    {0x0576, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0577, {0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37}},
    {0x0578, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0579, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {0x057A, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}},
    {0x057B, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x057C, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0584, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0585, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0588, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0589, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x058C, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x058D, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0590, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0591, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x05A8, {0x10, 0x10, 0x10, 0x10, 0x13, 0x13, 0x13, 0x13}},
    {0x05A9, {0x0E, 0x0E, 0x0E, 0x0E, 0x00, 0x00, 0x00, 0x00}},
    {0x05AA, {0x0A, 0x0A, 0x0A, 0x0A, 0x00, 0x00, 0x00, 0x00}},
    {0x05B1, {0x4D, 0x4D, 0x4D, 0x4D, 0xC0, 0xC0, 0xC0, 0xC0}},
    {0x05B2, {0x03, 0x03, 0x03, 0x03, 0x0D, 0x0D, 0x0D, 0x0D}},
    {0x05B3, {0xEC, 0xEC, 0xEC, 0xEC, 0x00, 0x00, 0x00, 0x00}},
    {0x05B4, {0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00}},
    {0x05B5, {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00}},
    {0x05B6, {0x07, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00}},
    {0x05C1, {0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E}},
    {0x05CA, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {0x05CB, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}},
    {0x05DA, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x05DB, {0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E}},
    {0x08FE, {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}},
    {0x0915, {0x32, 0x8F, 0x32, 0x8F, 0x32, 0x32, 0x32, 0x32}},
    {0x0917, {0x00, 0x28, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00}},
    {0x0919, {0x52, 0x27, 0x52, 0x27, 0x52, 0x52, 0x52, 0x52}},
    {0x091A, {0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}},
    {0x091B, {0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00}},
    {0x0931, {0x52, 0x52, 0x52, 0x52, 0x8F, 0x8F, 0x8F, 0x8F}},
    {0x0933, {0x00, 0x00, 0x00, 0x00, 0x19, 0x19, 0x19, 0x19}},
    {0x0935, {0x5F, 0x5F, 0x5F, 0x5F, 0x27, 0x27, 0x27, 0x27}},
    {0x0936, {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01}},
    {0x0937, {0x00, 0x00, 0x00, 0x00, 0x6F, 0x6F, 0x6F, 0x6F}},
    {0x0939, {0xA0, 0xA0, 0xA0, 0xA0, 0x63, 0x63, 0x63, 0x63}},
    {0x093B, {0x00, 0x00, 0x00, 0x00, 0x0F, 0x28, 0x0F, 0x28}},
    {0x0949, {0x00, 0x00, 0x00, 0x00, 0x61, 0x61, 0x61, 0x61}},
    {0x094D, {0xFE, 0xFE, 0xFE, 0xB0, 0xFE, 0xFE, 0xFE, 0xFE}},
    {0x094E, {0x07, 0x07, 0x07, 0x04, 0x07, 0x07, 0x07, 0x07}},
    {0x094F, {0x00, 0x02, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02}},
    {0x0A02, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0A20, {0x4E, 0x44, 0x41, 0x44, 0x3C, 0x43, 0x56, 0x43}},
    {0x0A35, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0A3A, {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40}},
    {0x0C17, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0CE1, {0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC}},
    {0x0CE2, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {0x0CE3, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}},
    {0x0D17, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
};

/*-----------------------------------------------------------------------------------------------*\
 * Attribute register and PHY control code
\*-----------------------------------------------------------------------------------------------*/
IMX272_REG_s IMX272AttrRegTable[IMX272_NUM_ATTR_REG] = {
    {0x0CE0, 0x10}, /* [4:0] INIT_LENGTH[4:0], [7:5] - */
    {0x0CE1, 0xFF}, /* [7:0] SYNCCODE_LEN_MAIN[7:0] */
    {0x0CE2, 0x7F}, /* [7:0] SYNCCODE_LEN_MAIN[15:8] */
    {0x0CE3, 0x00}, /* [7:0] SYNCCODE_LEN_MAIN[23:16] */
    {0x0CE4, 0xFF}, /* [7:0] DESKEWCODE_LEN_MAIN[7:0] */
    {0x0CE5, 0xFF}, /* [7:0] DESKEWCODE_INTVL_MAIN[7:0] */
    {0x0CE6, 0xFF}, /* [7:0] STBYCODE_LEN_MAIN[7:0] */
    {0x0CE7, 0xAA}, /* [7:0] SYNCCODE[7:0] */
    {0x0CE8, 0x00}, /* [0] SYNCCODE[8], [7:1] - */
    {0x0CE9, 0x60}, /* [7:0] DESKEWCODE[7:0] */
    {0x0CEA, 0x00}, /* [0] DESKEWCODE[8], [7:1] - */
    {0x0CEB, 0x00}, /* [7:0] IDLECODE1[7:0] */
    {0x0CEC, 0x00}, /* [0] IDLECODE1[8], [7:1] - */
    {0x0CED, 0x00}, /* [7:0] IDLECODE2[7:0] */
    {0x0CEE, 0x00}, /* [0] IDLECODE2[8], [7:1] - */
    {0x0CEF, 0x00}, /* [7:0] IDLECODE3[7:0] */
    {0x0CF0, 0x00}, /* [0] IDLECODE3[8], [7:1] - */
    {0x0CF1, 0x00}, /* [7:0] IDLECODE4[7:0] */
    {0x0CF2, 0x00}, /* [0] IDLECODE4[8], [7:1] - */
    {0x0CF6, 0x03}, /* [7:0] STBYCODE[7:0] */
    {0x0CF7, 0x00}, /* [0] STBYCODE[8], [7:1] - */
};

const AMBA_SENSOR_OUTPUT_INFO_s IMX272OutputInfo[IMX272_NUM_MODE] = {
    /* IMX272_5280_3956_60P   */ {2304000000U, 6 + 6, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 5376, 3974 + 2, { 72, 20, 5280, 3956}, {0}},
    /* IMX272_5280_3956_30P   */ {2304000000U, 6 + 6, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 5376, 3974 + 2, { 72, 20, 5280, 3956}, {0}},
    /* IMX272_5280_3044_60P   */ {2304000000U, 6 + 6, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 5376, 3062 + 2, { 72, 20, 5280, 3044}, {0}},
    /* IMX272_5280_3044_30P   */ {2304000000U, 6 + 6, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 5376, 3062 + 2, { 72, 20, 5280, 3044}, {0}},
    /* IMX272_4128_2264_60P   */ {2304000000U, 4 + 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 4176, 2290 + 2, { 48, 28, 4128, 2264}, {0}},
    /* IMX272_2640_1522_60P   */ {2304000000U,     4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 2688, 1540 + 2, { 36, 20, 2640, 1522}, {0}},
    /* IMX272_2640_1522_120P  */ {2304000000U,     6, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 2688, 1540 + 2, { 36, 20, 2640, 1522}, {0}},
    /* IMX272_1760_1018_60P   */ {2304000000U,     2, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1792, 1036 + 2, { 24, 20, 1760, 1018}, {0}},
};

/* NOTE: The information in this table needs to be revisied according to pixel address of each mode from Sony */
const AMBA_SENSOR_INPUT_INFO_s IMX272InputInfo[IMX272_NUM_MODE] = {
    /* IMX272_5280_3956_60P   */ {{   0,   0, 5280, 3956}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* IMX272_5280_3956_30P   */ {{   0,   0, 5280, 3956}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* IMX272_5280_3044_60P   */ {{   0, 456, 5280, 3044}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* IMX272_5280_3044_30P   */ {{   0, 456, 5280, 3044}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* IMX272_4128_2264_60P   */ {{ 592, 898, 4096, 2160}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* IMX272_2640_1522_60P   */ {{   0, 456, 5280, 3044}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* IMX272_2640_1522_120P  */ {{   0, 456, 5280, 3044}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* IMX272_1760_1018_60P   */ {{   0, 452, 5280, 3054}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
};

IMX272_MODE_INFO_s IMX272ModeInfoList[IMX272_NUM_MODE] = {
    /* IMX272_5280_3956_60P   */ {1, { /*71928072*/72000000,  300, 1.0, 4000, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX272_5280_3956_30P   */ {1, { /*71928072*/72000000,  600, 1.0, 4000, 1, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX272_5280_3044_60P   */ {1, { /*71928072*/72000000,  300, 1.0, 4000, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX272_5280_3044_30P   */ {1, { /*71928072*/72000000,  600, 1.0, 4000, 1, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX272_4128_2264_60P   */ {1, { /*71928072*/72000000,  300, 1.0, 4000, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX272_2640_1522_60P   */ {0, { /*71928072*/72000000,  600, 1.0, 2000, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX272_2640_1522_120P  */ {0, { /*71928072*/72000000,  300, 1.0, 2000, 1, { .Interlace = 0, .TimeScale = 120000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX272_1760_1018_60P   */ {0, { /*72000000*/72000000, 1092, 1.0, 1100, 1, { .Interlace = 0, .TimeScale =  60000,  .NumUnitsInTick = 1001}}, 0.0f},
};

const AMBA_SENSOR_HDR_INFO_s IMX272HdrInfo[IMX272_NUM_MODE] = {0};
