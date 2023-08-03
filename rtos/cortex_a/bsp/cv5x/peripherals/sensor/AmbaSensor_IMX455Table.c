/**
 *  @file AmbaSensor_IMX455Table.c
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
 *  @details Control APIs of SONY IMX455 CMOS sensor with SLVS-EC interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_IMX455.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s IMX455DeviceInfo = {
    .UnitCellWidth          = (FLOAT)3.76,
    .UnitCellHeight         = (FLOAT)3.76,
    .NumTotalPixelCols      = 9602,
    .NumTotalPixelRows      = 6498,
    .NumEffectivePixelCols  = 9576,
    .NumEffectivePixelRows  = 6388,
    .MinAnalogGainFactor    = (FLOAT)1.0,
    .MaxAnalogGainFactor    = (FLOAT)15.81081081, /* 23.979 dB */
    .MinDigitalGainFactor   = (FLOAT)1.0,
    .MaxDigitalGainFactor   = (FLOAT)63.09573445, /* 36 dB */

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
 * Initial Setting Registers
\*-----------------------------------------------------------------------------------------------*/
IMX455_INIT_REG_s IMX455InitRegTable[IMX455_NUM_INIT_REG] = {
    /*       [ Mode No. ] */
    /* Addr,  0A-b, 0A-c  */
    {0x0000, {0x04, 0x04}},
    {0x0001, {0x40, 0x80}}, /* leave 0A-b as default */
    {0x0002, {0x10, 0x10}},
    {0x0003, {0x10, 0x10}},
    {0x0005, {0x09, 0x09}},
    {0x0006, {0x23, 0x23}},
    {0x0007, {0x04, 0x04}},
    {0x0008, {0xE0, 0xE0}},
    {0x0009, {0x10, 0x10}},
    {0x000A, {0x09, 0x09}},
    {0x000C, {0x08, 0x08}},
    {0x0016, {0x02, 0x02}},
    {0x0019, {0x09, 0x09}}, /* leave 0A-b, 0A-c as default */
    {0x001B, {0x05, 0x05}}, /* leave 0A-b, 0A-c as default */
    {0x0025, {0x0A, 0x0A}},
    {0x0028, {0x04, 0x04}}, /* leave 0A-b, 0A-c as default */
    {0x0046, {0x03, 0x03}},
    {0x004F, {0x08, 0x08}},
    {0x0052, {0xDA, 0xF8}},
    {0x0053, {0x04, 0x04}},
    {0x0058, {0x52, 0x13}},
    {0x0059, {0x04, 0x04}},
    {0x005A, {0x9A, 0xA2}},
    {0x005B, {0x04, 0x04}},
    {0x0060, {0x77, 0x5D}},
    {0x0061, {0x05, 0x04}},
    {0x0067, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x00A5, {0x01, 0x01}},
    {0x00A6, {0x3B, 0x3B}},
    {0x00A9, {0x01, 0x02}}, /* leave 0A-b as default */
    {0x00C6, {0x08, 0x08}},
    {0x00CC, {0x6A, 0x7E}},
    {0x00CE, {0x6A, 0x7E}},
    {0x00D1, {0x72, 0x86}},
    {0x00D3, {0x72, 0x86}},
    {0x00D4, {0x80, 0x00}}, /* leave 0A-b as default */
    {0x00D5, {0x01, 0x00}}, /* leave 0A-b as default */
    {0x00D7, {0x88, 0x88}},
    {0x00DA, {0x31, 0x31}},
    {0x010E, {0x02, 0x02}}, /* leave 0A-b, 0A-c as default */
    {0x0112, {0x03, 0x02}}, /* leave 0A-b as default */
    {0x0187, {0x05, 0x05}},
    {0x0188, {0x20, 0x20}},
    {0x0189, {0x1E, 0x1E}},
    {0x018C, {0x20, 0x20}},
    {0x018D, {0x1E, 0x1E}},
    {0x019E, {0x01, 0x01}},
    {0x01A0, {0x06, 0x06}},
    {0x0201, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0207, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0366, {0x0E, 0x0E}},
    {0x03A0, {0x0F, 0x0F}},
    {0x03A2, {0x07, 0x07}},
    {0x03A3, {0x11, 0x11}},
    {0x03A4, {0x11, 0x11}},
    {0x03A5, {0x11, 0x11}},
    {0x03A6, {0x11, 0x11}},
    {0x048F, {0xAD, 0xC1}},
    {0x0498, {0xAD, 0xC1}},
    {0x04BF, {0x01, 0x01}},
    {0x04C3, {0x01, 0x01}},
    {0x04CB, {0x02, 0x02}},
    {0x0509, {0x90, 0x98}},
    {0x050F, {0x4E, 0x62}},
    {0x0510, {0x00, 0x1B}}, /* leave 0A-b as default */
    {0x0512, {0x70, 0xD2}}, /* leave 0A-b as default */
    {0x0513, {0x70, 0xFF}}, /* leave 0A-b as default */
    {0x0514, {0x70, 0xFF}}, /* leave 0A-b as default */
    {0x0515, {0x03, 0x00}}, /* leave 0A-b as default */
    {0x0517, {0x4E, 0x62}},
    {0x0518, {0x00, 0x1B}}, /* leave 0A-b as default */
    {0x051A, {0x70, 0xD2}}, /* leave 0A-b as default */
    {0x051B, {0x70, 0xFF}}, /* leave 0A-b as default */
    {0x051C, {0x70, 0xFF}}, /* leave 0A-b as default */
    {0x051F, {0x5D, 0x71}},
    {0x0553, {0xAC, 0xC0}},
    {0x0559, {0x2D, 0x37}}, /* leave 0A-c as default */
    {0x055A, {0x2B, 0x35}}, /* leave 0A-c as default */
    {0x055C, {0x2D, 0x37}}, /* leave 0A-c as default */
    {0x055D, {0x2B, 0x35}}, /* leave 0A-c as default */
    {0x055F, {0x34, 0x3E}}, /* leave 0A-c as default */
    {0x0560, {0x32, 0x3C}}, /* leave 0A-c as default */
    {0x0562, {0x34, 0x3E}}, /* leave 0A-c as default */
    {0x0563, {0x32, 0x3C}}, /* leave 0A-c as default */
    {0x056B, {0x3B, 0x45}}, /* leave 0A-c as default */
    {0x056C, {0x39, 0x43}}, /* leave 0A-c as default */
    {0x056E, {0x34, 0x3E}}, /* leave 0A-c as default */
    {0x056F, {0x32, 0x3C}}, /* leave 0A-c as default */
    {0x0573, {0x00, 0x00}},
    {0x0574, {0x02, 0x02}},
    {0x0575, {0x02, 0x02}},
    {0x0576, {0x02, 0x02}},
    {0x0577, {0x02, 0x02}},
    {0x0581, {0x02, 0x04}}, /* leave 0A-b as default */
    {0x0582, {0x10, 0x10}},
    {0x0583, {0x10, 0x10}},
    {0x0584, {0x10, 0x10}},
    {0x0585, {0x10, 0x10}},
    {0x0586, {0x10, 0x10}},
    {0x0587, {0x10, 0x10}},
    {0x0588, {0x10, 0x10}},
    {0x0589, {0x10, 0x10}},
    {0x059A, {0x00, 0x04}}, /* leave 0A-c as default */
    {0x05A1, {0x4E, 0x62}},
    {0x05A2, {0x00, 0x1B}}, /* leave 0A-b as default */
    {0x05A4, {0x70, 0xD2}}, /* leave 0A-b as default */
    {0x05A5, {0x70, 0xFF}}, /* leave 0A-b as default */
    {0x05A6, {0x70, 0xFF}}, /* leave 0A-b as default */
    {0x05A8, {0x4E, 0x62}},
    {0x05A9, {0x00, 0x1B}}, /* leave 0A-b as default */
    {0x05AB, {0x70, 0xD2}}, /* leave 0A-b as default */
    {0x05AC, {0x70, 0xFF}}, /* leave 0A-b as default */
    {0x05AD, {0x70, 0xFF}}, /* leave 0A-b as default */
    {0x05AF, {0x5D, 0x71}},
    {0x0603, {0x4E, 0x62}},
    {0x0605, {0x4E, 0x62}},
    {0x062A, {0xB6, 0xD4}},
    {0x0630, {0xB4, 0xD2}},
    {0x0635, {0x2D, 0x37}}, /* leave 0A-c as default */
    {0x0636, {0x29, 0x33}}, /* leave 0A-c as default */
    {0x0637, {0x34, 0x3E}}, /* leave 0A-c as default */
    {0x0638, {0x29, 0x33}}, /* leave 0A-c as default */
    {0x063A, {0x2D, 0x37}}, /* leave 0A-c as default */
    {0x063B, {0x29, 0x33}}, /* leave 0A-c as default */
    {0x063C, {0x34, 0x3E}}, /* leave 0A-c as default */
    {0x063D, {0x29, 0x33}}, /* leave 0A-c as default */
    {0x063F, {0x2D, 0x37}}, /* leave 0A-c as default */
    {0x0640, {0x29, 0x33}}, /* leave 0A-c as default */
    {0x0641, {0x34, 0x3E}}, /* leave 0A-c as default */
    {0x0642, {0x29, 0x33}}, /* leave 0A-c as default */
    {0x0646, {0xB1, 0xC5}},
    {0x064A, {0xB1, 0xC5}},
    {0x066D, {0x77, 0x00}}, /* leave 0A-b as default */
    {0x066E, {0x77, 0x00}}, /* leave 0A-b as default */
    {0x0670, {0x77, 0x00}}, /* leave 0A-b as default */
    {0x0671, {0x77, 0x00}}, /* leave 0A-b as default */
    {0x0673, {0x77, 0x00}},
    {0x0674, {0x77, 0x00}},
    {0x0676, {0x77, 0x00}},
    {0x0677, {0x77, 0x00}},
    {0x0679, {0x05, 0x07}}, /* leave 0A-b as default */
    {0x067E, {0x06, 0x06}},
    {0x068A, {0x88, 0x88}},
    {0x06A2, {0x03, 0x03}},
    {0x06AB, {0x31, 0x31}},
    {0x06B0, {0x31, 0x31}},
    {0x06B5, {0x31, 0x31}},
    {0x06BA, {0x01, 0x01}},
    {0x07D0, {0x06, 0x06}},
    {0x07D1, {0x0B, 0x0B}},
    {0x07D3, {0x06, 0x06}},
    {0x07D4, {0x0B, 0x0B}},
    {0x07D6, {0x06, 0x06}},
    {0x0A43, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A44, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A45, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A48, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A5A, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A5C, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A5E, {0x12, 0x12}}, /* leave 0A-b, 0A-c as default */
    {0x0A5F, {0x12, 0x12}}, /* leave 0A-b, 0A-c as default */
    {0x0A80, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A81, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A82, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A83, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A84, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A85, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A86, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A87, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A88, {0x00, 0x00}}, /* leave 0A-b, 0A-c as default */
    {0x0A96, {0x01, 0x01}},
};

/*-----------------------------------------------------------------------------------------------*\
 * Attribute Register and PHY Control Code
\*-----------------------------------------------------------------------------------------------*/
IMX455_REG_s IMX455AttrRegTable[IMX455_NUM_ATTR_REG] = {
    {0x0115, 0x0A}, /* [4:0] INIT_LENGTH[4:0], [7:5] - */
    {0x0116, 0xFF}, /* [7:0] SYNC_LENGTH[7:0] */
    {0x0117, 0x7F}, /* [7:0] SYNC_LENGTH[15:8] */
    {0x0118, 0x00}, /* [7:0] SYNC_LENGTH[23:16] */
    {0x0119, 0xFF}, /* [7:0] DESKEW_LENGTH[7:0] */
    {0x011A, 0xFF}, /* [7:0] DESKEW_INTERVAL[7:0] */
    {0x011B, 0xFF}, /* [7:0] STANDBY_LENGTH[7:0] */
    {0x011C, 0xAA}, /* [7:0] SYNC_SYMBOL[7:0] */
    {0x011D, 0x00}, /* [0]   SYNC_SYMBOL[8], [7:1] - */
    {0x011E, 0x60}, /* [7:0] DESKEW_SYMBOL[7:0] */
    {0x011F, 0x00}, /* [0]   DESKEW_SYMBOL[8], [7:1] - */
    {0x0120, 0x00}, /* [7:0] IDLE_CODE1[7:0] */
    {0x0121, 0x00}, /* [0]   IDLE_CODE1[8], [7:1] - */
    {0x0122, 0x00}, /* [7:0] IDLE_CODE2[7:0] */
    {0x0123, 0x00}, /* [0]   IDLE_CODE2[8], [7:1] - */
    {0x0124, 0x00}, /* [7:0] IDLE_CODE3[7:0] */
    {0x0125, 0x00}, /* [0]   IDLE_CODE3[8], [7:1] - */
    {0x0126, 0x00}, /* [7:0] IDLE_CODE4[7:0] */
    {0x0127, 0x00}, /* [0]   IDLE_CODE4[8], [7:1] - */
    {0x012A, 0x03}, /* [7:0] STANDBY_SYMBOL[7:0] */
    {0x012B, 0x00}, /* [0]   STANDBY_SYMBOL[8], [7:1] - */
};

const AMBA_SENSOR_OUTPUT_INFO_s IMX455OutputInfo[IMX455_NUM_MODE] = {
    /* IMX455_7696_4320_19P   */ {2304000000U,  8, 14, AMBA_SENSOR_BAYER_PATTERN_RG, 7712, 4332, { 16, 8, 7696, 4320}, {0}},
    /* IMX455_7696_4320_30P   */ {2304000000U,  8, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 7712, 4332, { 16, 8, 7696, 4320}, {0}},
};

/* NOTE: The information in this table needs to be revisied according to pixel address of each mode from Sony */
const AMBA_SENSOR_INPUT_INFO_s IMX455InputInfo[IMX455_NUM_MODE] = {
    /* IMX455_7696_4320_19P   */ {{  16, 8, 7696, 4320}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
    /* IMX455_7696_4320_30P   */ {{  16, 8, 7696, 4320}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1,  1}, 1},
};

IMX455_MODE_INFO_s IMX455ModeInfoList[IMX455_NUM_MODE] = {
    /* IMX455_7696_4320_19P   */ {0, { 72000000, 840, 1.0, 4350, 1, { .Interlace = 0, .TimeScale =  19000,  .NumUnitsInTick = 1001}}, 0.0f},
    /* IMX455_7696_4320_30P   */ {0, { 72000000, 550, 1.0, 4368, 1, { .Interlace = 0, .TimeScale =  30000,  .NumUnitsInTick = 1001}}, 0.0f},
};

const AMBA_SENSOR_HDR_INFO_s IMX455HdrInfo[IMX455_NUM_MODE] = {0};
