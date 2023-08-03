/**
 *  @file AmbaSensor_B8N_IMX290Table.c
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
 *  @details Control APIs of SONY B8N_IMX290 CMOS sensor with LVDS interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_B8N_IMX290.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s B8N_IMX290DeviceInfo = {
    .UnitCellWidth          = (FLOAT)2.9,
    .UnitCellHeight         = (FLOAT)2.9,
    .NumTotalPixelCols      = 1945,
    .NumTotalPixelRows      = 1109,
    .NumEffectivePixelCols  = 1945,
    .NumEffectivePixelRows  = 1097,
    .MinAnalogGainFactor    = (FLOAT)1.0,
    .MaxAnalogGainFactor    = (FLOAT)3981.071706,
    .MinDigitalGainFactor   = (FLOAT)1.0,
    .MaxDigitalGainFactor   = (FLOAT)1.0,

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
        .FirstReflectedFrame    = 2,
        .FirstBadFrame          = 0,
        .NumBadFrames           = 0
    },
    .StrobeCtrlInfo = {0},
    .HdrIsSupport = 1,
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/

B8N_IMX290_SEQ_REG_s B8N_IMX290InitRegTable[B8N_IMX290_NUM_INIT_REG] = {
    /* Chip ID: 02h */
    {0x300F, 0x00},
    {0x3010, 0x21},
    {0x3012, 0x64},
    {0x3016, 0x09},
    {0x3070, 0x02},
    {0x3071, 0x11},
    {0x309B, 0x10},
    {0x309C, 0x22},
    {0x30A2, 0x02},
    {0x30A6, 0x20},
    {0x30A8, 0x20},
    {0x30AA, 0x20},
    {0x30AC, 0x20},
    {0x30B0, 0x43},
    /* Chip ID: 03h */
    /*{0x310B, 0x01},*/ /* According to sony fae's suggestion, set to 1 to fix lowlight issue for 1080p */
    {0x3119, 0x9E},
    {0x311C, 0x1E},
    {0x311E, 0x08},
    {0x3128, 0x05},
    {0x313D, 0x83},
    {0x3150, 0x03},
    {0x317E, 0x00},
    /* Chip ID: 04h */
    {0x32B8, 0x50},
    {0x32B9, 0x10},
    {0x32BA, 0x00},
    {0x32BB, 0x04},
    {0x32C8, 0x50},
    {0x32C9, 0x10},
    {0x32CA, 0x00},
    {0x32CB, 0x04},
    /* Chip ID: 05h */
    {0x332C, 0xD3},
    {0x332D, 0x10},
    {0x332E, 0x0D},
    {0x3358, 0x06},
    {0x3359, 0xE1},
    {0x335A, 0x11},
    {0x3360, 0x1E},
    {0x3361, 0x61},
    {0x3362, 0x10},
    {0x33B0, 0x50},
    {0x33B2, 0x1A},
    {0x33B3, 0x04},
    /* Chip ID: 06h */
    {0x3444, 0x20},
    {0x3445, 0x25},
};

B8N_IMX290_REG_s B8N_IMX290RegTable[B8N_IMX290_NUM_MODE_REG] = {
    {0x305C, {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x10, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}}, // INCKSEL 1
    {0x305D, {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03}}, // INCKSEL 2
    {0x305E, {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}}, // INCKSEL 3
    {0x305F, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}}, // INCKSEL 4
    {0x315E, {0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A}}, // INCKSEL 5
    {0x3164, {0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A}}, // INCKSEL 6
    {0x3005, {0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01}}, // ADBIT[0] 0:10bit, 1:12bit
    {0x3007, {0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00}}, // VREVERSE[0]; HREVERSE[1]; WINMODE[6:4];
    {0x3009, {0x01, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x01, 0x02}}, // FRSEL[1:0] 2:30/25fps, 1:60/50fps, 0:120/100fps; FDGSEL[4] 0:LCG, 1:HCG
    {0x300A, {0xF0, 0xF0, 0x3C, 0x3C, 0x3C, 0xF0, 0x3c, 0x3c, 0xF0, 0xF0, 0x3C, 0x3C, 0x3C, 0xF0, 0xF0}}, // BLKLEVEL_LSB 12bit:0xF0 10bit:0x3C
    {0x300B, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // BLKLEVEL_MSB
    {0x300C, {0x00, 0x00, 0x11, 0x11, 0x21, 0x11, 0x21, 0x11, 0x00, 0x00, 0x11, 0x11, 0x21, 0x11, 0x00}}, // WDMODE [0] 0:Normal mode, 1:DOL mode, [5:4] 0:Normal mode, 1:DOL 2frame, 2:DOL 3frame
    {0x3018, {0x65, 0x65, 0xC4, 0xCA, 0x65, 0x32, 0x96, 0x65, 0x65, 0x65, 0xC4, 0xCA, 0x65, 0x32, 0x97}}, // VMAX_LSB
    {0x3019, {0x04, 0x04, 0x04, 0x08, 0x04, 0x02, 0x01, 0x04, 0x04, 0x04, 0x04, 0x08, 0x04, 0x02, 0x06}}, // VMAX_MSB
    {0x301A, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // VMAX_HSB
    {0x301C, {0x98, 0x30, 0xF6, 0x4C, 0x4C, 0x98, 0xf4, 0x4c, 0x50, 0xA0, 0xC0, 0x28, 0x28, 0x50, 0x30}}, // HMAX_LSB
    {0x301D, {0x08, 0x11, 0x03, 0x04, 0x04, 0x08, 0x05, 0x04, 0x0A, 0x14, 0x04, 0x05, 0x05, 0x0A, 0x11}}, // HMAX_HSB
    {0x3046, {0x01, 0x01, 0xE0, 0xE0, 0xE0, 0x01, 0xF0, 0x00, 0x01, 0x01, 0xE0, 0xE0, 0xE0, 0xE0, 0x01}}, // ODBIT[1:0] MIPI fixed to 1(DOL mode check with FAE); OPORTSEL[7:4] MIPI fixed to 0(DOL mode check with FAE)
    {0x3129, {0x00, 0x00, 0x1D, 0x1D, 0x1D, 0x00, 0x1d, 0x1d, 0x00, 0x00, 0x1D, 0x1D, 0x1D, 0x00, 0x00}}, // ADBIT2 10bit:0x1D, 12bit:0x00
    {0x317C, {0x00, 0x00, 0x12, 0x12, 0x12, 0x00, 0x12, 0x12, 0x00, 0x00, 0x12, 0x12, 0x12, 0x00, 0x00}}, // ADBIT2 10bit:0x12, 12bit:0x00
    {0x31EC, {0x0E, 0x0E, 0x37, 0x37, 0x37, 0x0E, 0x37, 0x37, 0x0E, 0x0E, 0x37, 0x37, 0x37, 0x0E, 0x0E}}, // ADBIT3 10bit:0x37, 12bit:0x0E
    {0x3405, {0x10, 0x20, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x20, 0x00, 0x00, 0x00, 0x10, 0x20}}, // REPETITION
    {0x3407, {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03}}, // PHY_LANE_NUM[1:0] 3:4 Lane; 1:2 Lane
    {0x3414, {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A}}, // OPB_SIZE_V
    {0x31A0, {0xFC, 0xFC, 0x50, 0xB4, 0xB4, 0x50, 0xFC, 0x50, 0xFC, 0xFC, 0x50, 0xB4, 0xB4, 0x50, 0xFC}}, // HBLANK_LSB
    {0x31A1, {0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x00, 0x02, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x00}}, // HBLANK_MSB
    {0x3415, {0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01}}, // NULL0_Size
    {0x3418, {0x49, 0x49, 0x64, 0x64, 0x2E, 0x64, 0x55, 0xb2, 0x49, 0x49, 0x64, 0x64, 0x2E, 0x64, 0x49}}, // YOUTSIZE_LSB (when mipi, need to *2 )
    {0x3419, {0x04, 0x04, 0x11, 0x11, 0x1A, 0x04, 0x11, 0x08, 0x04, 0x04, 0x11, 0x11, 0x1A, 0x04, 0x04}}, // YOUTSIZE_MSB (when mipi, need to *2 )
    {0x3441, {0x0C, 0x0C, 0x0A, 0x0A, 0x0A, 0x0C, 0x0a, 0x0a, 0x0C, 0x0C, 0x0A, 0x0A, 0x0A, 0x0C, 0x0C}}, // CSI_DT_FMT_LSB 10-bit:0x0A 12-bit:0x0c
    {0x3442, {0x0C, 0x0C, 0x0A, 0x0A, 0x0A, 0x0C, 0x0a, 0x0a, 0x0C, 0x0C, 0x0A, 0x0A, 0x0A, 0x0C, 0x0C}}, // CSI_DT_FMT_MSB 10-bit:0x0A 12-bit:0x0c
    {0x3443, {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03}}, // CSI_LANE_MODE[1:0] 3:4 Lane; 1:2 Lane
    {0x3446, {0x57, 0x47, 0x77, 0x77, 0x77, 0x57, 0x67, 0x57, 0x57, 0x47, 0x77, 0x77, 0x77, 0x57, 0x47}}, // TCLKPOST_LSB
    {0x3447, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // TCLKPOST_MSB[0]
    {0x3448, {0x37, 0x1F, 0x67, 0x67, 0x67, 0x37, 0x57, 0x67, 0x37, 0x1F, 0x67, 0x67, 0x67, 0x37, 0x1F}}, // THSZERO_LSB
    {0x3449, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // THSZERO_MSB[0]
    {0x344A, {0x1F, 0x17, 0x47, 0x47, 0x47, 0x1F, 0x2f, 0x47, 0x1F, 0x17, 0x47, 0x47, 0x47, 0x1F, 0x17}}, // THSPREPARE_LSB
    {0x344B, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // THSPREPARE_MSB[0]
    {0x344C, {0x1F, 0x0F, 0x37, 0x37, 0x37, 0x1F, 0x27, 0x37, 0x1F, 0x0F, 0x37, 0x37, 0x37, 0x1F, 0x0F}}, // TCLKTRAIL_LSB
    {0x344D, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // TCLKTRAIL_MSB[0]
    {0x344E, {0x1F, 0x17, 0x3F, 0x3F, 0x3F, 0x1F, 0x2f, 0x3F, 0x1F, 0x17, 0x3F, 0x3F, 0x3F, 0x1F, 0x17}}, // THSTRAIL_LSB
    {0x344F, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // THSTRAIL_MSB[0]
    {0x3450, {0x77, 0x47, 0xFF, 0xFF, 0xFF, 0x77, 0xBF, 0xff, 0x77, 0x47, 0xFF, 0xFF, 0xFF, 0x77, 0x47}}, // TCLKZERO_LSB
    {0x3451, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // TCLKZERO_MSB[0]
    {0x3452, {0x1F, 0x0F, 0x3F, 0x3F, 0x3F, 0x1F, 0x2F, 0x3F, 0x1F, 0x0F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F}}, // TCLKPREPARE_LSB
    {0x3453, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // TCLKPREPARE_MSB[0]
    {0x3454, {0x17, 0x0F, 0x37, 0x37, 0x37, 0x17, 0x27, 0x37, 0x17, 0x0F, 0x37, 0x37, 0x37, 0x17, 0x0F}}, // TLPX_LSB
    {0x3455, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // TLPX_MSB[0]
    {0x3472, {0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x20, 0xa0, 0x4c, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x20, 0x9C}}, // XOUTSIZE_LSB DOL TBD
    {0x3473, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x04, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}}, // XOUTSIZE_MSB DOL TBD
    {0x3030, {0x00, 0x00, 0xC9, 0xA3, 0x85, 0x45, 0x46, 0x87, 0x00, 0x00, 0xC9, 0xA3, 0x85, 0x45, 0x00}}, // RHS1_LSB
    {0x3031, {0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}}, // RHS1_MSB
    {0x3032, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // RHS1_HSB
    {0x3034, {0x00, 0x00, 0x00, 0x00, 0x92, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x00, 0x00}}, // RHS2_LSB
    {0x3035, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // RHS2_MSB
    {0x3036, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // RHS2_HSB
    {0x3045, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}}, // DOL_FORMAT
    {0x3106, {0x00, 0x00, 0x10, 0x10, 0x30, 0x10, 0x30, 0x10, 0x00, 0x00, 0x10, 0x10, 0x30, 0x10, 0x00}}, // DOL_SYNCSIGNAL
    {0x303C, {0x00, 0x00, 0x00, 0x00, 0x00, 0x5C, 0x22, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5C, 0x00}}, // WINPV_LSB[7:0]
    {0x303D, {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00}}, // WINPV_MSB[2:0]
    {0x303E, {0x49, 0x49, 0x49, 0x49, 0x49, 0x90, 0xf4, 0xe0, 0x49, 0x49, 0x49, 0x49, 0x49, 0x90, 0x49}}, // WINWV_LSB[7:0]
    {0x303F, {0x04, 0x04, 0x04, 0x04, 0x04, 0x01, 0x01, 0x02, 0x04, 0x04, 0x04, 0x04, 0x04, 0x01, 0x04}}, // WINWV_MSB[2:0]
    {0x3040, {0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x04, 0xa4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x00}}, // WINPH_LSB[7:0]
    {0x3041, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, // WINPH_MSB
    {0x3042, {0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x1C, 0x90, 0x48, 0x9C, 0x9C, 0x9C, 0x9C, 0x9C, 0x1C, 0x9C}}, // WINWH_LSB[7:0]
    {0x3043, {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x04, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}}, // WINWH_MSB
    {0x304B, {0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x0A, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00}}, // SVSOUTSEL
    {0x347B, {0x24, 0x24, 0x24, 0x24, 0x24, 0x23, 0x23, 0x23, 0x24, 0x24, 0x24, 0x24, 0x24, 0x23, 0x24}},
    {0x3480, {0x92, 0x92, 0x92, 0x92, 0x92, 0x49, 0x49, 0x49, 0x92, 0x92, 0x92, 0x92, 0x92, 0x49, 0x92}},
};

const B8N_IMX290_SENSOR_INFO_s B8N_IMX290SensorInfo[B8N_IMX290_NUM_MODE] = {
    /* B8N_IMX290_1920_1080_P60       */ {37087912, 445054944, 4, 12, 2200, 1125, 1948, 1107, {12, 18, 1920, 1080}, {0}},
    /* B8N_IMX290_1920_1080_P30       */ {37087912, 222527472, 4, 12, 2200, 1125, 1948, 1107, {12, 18, 1920, 1080}, {0}},
    /* B8N_IMX290_1920_1080_P60_HDR   */ {37075325, 889807800, 4, 10, 2434, 2440, 1948, 2418, {12,  0, 1920, 2418}, {0}},
    /* B8N_IMX290_1920_1080_P30_HDR_0 */ {37087912, 890109888, 4, 10, 2640, 4500, 1948, 2892, {12,  0, 1920, 2892}, {0}},
    /* B8N_IMX290_1920_1080_P30_HDR_1 */ {37087912, 890109888, 4, 10, 2640, 4500, 1948, 3471, {12,  0, 1920, 3471}, {0}},
    /* B8N_IMX290_1820_400_P60_HDR    */ {37054945, 444659340, 4, 12, 2200, 1124, 1823,  892, { 0,  0, 1820,  892}, {0}},
    /* B8N_IMX290_1920_500_P60_HDR    */ {37087912, 593406592, 4, 10, 2438, 1624, 1936, 1596, {12,  0, 1920, 1596}, {0}},
    /* B8N_IMX290_1096_736_P60_HDR    */ {37087912, 890109890, 4, 10, 2640, 2250, 1096, 1630, { 0,  0, 1096, 1630}, {0}},
    /* B8N_IMX290_1920_1080_P50       */ {37125000, 445500000, 4, 12, 2640, 1125, 1948, 1107, {12, 18, 1920, 1080}, {0}},
    /* B8N_IMX290_1920_1080_P25       */ {37125000, 222750000, 4, 12, 2640, 1125, 1948, 1107, {12, 18, 1920, 1080}, {0}},
    /* B8N_IMX290_1920_1080_P50_HDR   */ {37088000, 890112000, 4, 10, 2918, 2440, 1948, 2418, {12,  0, 1920, 2418}, {0}},
    /* B8N_IMX290_1920_1080_P25_HDR_0 */ {37125000, 891000000, 4, 10, 3168, 4500, 1948, 2892, {12,  0, 1920, 2892}, {0}},
    /* B8N_IMX290_1920_1080_P25_HDR_1 */ {37125000, 891000000, 4, 10, 3168, 4500, 1948, 3471, {12,  0, 1920, 3471}, {0}},
    /* B8N_IMX290_1820_400_P50_HDR    */ {37092000, 445500000, 4, 12, 2640, 1124, 1823,  892, { 0,  0, 1820,  892}, {0}},
    /* B8N_IMX290_1920_1080_P20       */ {37114000, 222684000, 4, 12, 2200, 1687, 1948, 1107, {12, 18, 1920, 1080}, {0}},
};

const AMBA_SENSOR_OUTPUT_INFO_s B8N_IMX290OutputInfo[B8N_IMX290_NUM_MODE] = {
    /* B8N_IMX290_1920_1080_P60       */ { 500000000, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 1080, {0, 0, 1920, 1080}, {0}},
    /* B8N_IMX290_1920_1080_P30       */ { 250000000, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 1080, {0, 0, 1920, 1080}, {0}},
    /* B8N_IMX290_1920_1080_P60_HDR   */ { 900000000, 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 2418, {0, 0, 1920, 2418}, {0}},
    /* B8N_IMX290_1920_1080_P30_HDR_0 */ { 900000000, 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 2892, {0, 0, 1920, 2892}, {0}},
    /* B8N_IMX290_1920_1080_P30_HDR_1 */ { 900000000, 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 3471, {0, 0, 1920, 3471}, {0}},
    /* B8N_IMX290_1820_400_P60_HDR    */ { 500000000, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1820,  892, {0, 0, 1820,  892}, {0}},
    /* B8N_IMX290_1920_500_P60_HDR    */ { 800000000, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 1596, {0, 0, 1920, 1596}, {0}},
    /* B8N_IMX290_1096_736_P60_HDR    */ { 800000000, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1096, 1630, {0, 0, 1096, 1630}, {0}},
    /* B8N_IMX290_1920_1080_P50       */ { 500000000, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 1080, {0, 0, 1920, 1080}, {0}},
    /* B8N_IMX290_1920_1080_P25       */ { 250000000, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 1080, {0, 0, 1920, 1080}, {0}},
    /* B8N_IMX290_1920_1080_P50_HDR   */ { 900000000, 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 2418, {0, 0, 1920, 2418}, {0}},
    /* B8N_IMX290_1920_1080_P25_HDR_0 */ { 900000000, 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 2892, {0, 0, 1920, 2892}, {0}},
    /* B8N_IMX290_1920_1080_P25_HDR_1 */ { 900000000, 4, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 3471, {0, 0, 1920, 3471}, {0}},
    /* B8N_IMX290_1820_400_P50_HDR    */ { 500000000, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1820,  892, {0, 0, 1820,  892}, {0}},
    /* B8N_IMX290_1920_1080_P20       */ { 400000000, 4, 12, AMBA_SENSOR_BAYER_PATTERN_RG, 1920, 1080, {0, 0, 1920, 1080}, {0}},
};

const AMBA_SENSOR_INPUT_INFO_s B8N_IMX290InputInfo[B8N_IMX290_NUM_MODE] = {
    /* B8N_IMX290_1920_1080_P60       */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P30       */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P60_HDR   */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P30_HDR_0 */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P30_HDR_1 */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1820_400_P60_HDR    */ {{58, 348, 1820,  400}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_500_P60_HDR    */ {{16, 290, 1920,  500}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1096_736_P60_HDR    */ {{420, 180, 1096, 736}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P50       */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P25       */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P50_HDR   */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P25_HDR_0 */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P25_HDR_1 */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1820_400_P50_HDR    */ {{58, 348, 1820,  400}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
    /* B8N_IMX290_1920_1080_P20       */ {{12,   8, 1920, 1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL,  1,  1}, 1},
};

B8N_IMX290_MODE_INFO_s B8N_IMX290ModeInfoList[B8N_IMX290_NUM_MODE] = {
    /* B8N_IMX290_1920_1080_P60       */ {{1920,  B8_SERDES_RATE_2P3G, B8_COMPRESS_NONE}, { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P30       */ {{1920, B8_SERDES_RATE_2P3G, B8_COMPRESS_NONE}, { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P60_HDR   */ {{1920,  B8_SERDES_RATE_2P3G, B8_COMPRESS_5P75}, { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P30_HDR_0 */ {{1920,  B8_SERDES_RATE_2P3G, B8_COMPRESS_5P75}, { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P30_HDR_1 */ {{1920,  B8_SERDES_RATE_2P3G, B8_COMPRESS_5P75}, { .Interlace = 0, .TimeScale = 30000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* B8N_IMX290_1820_400_P60_HDR    */ {{1820,  B8_SERDES_RATE_2P3G, B8_COMPRESS_NONE}, { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_500_P60_HDR    */ {{1920,  B8_SERDES_RATE_2P3G, B8_COMPRESS_8P75}, { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* B8N_IMX290_1096_736_P60_HDR    */ {{1096,  B8_SERDES_RATE_2P3G, B8_COMPRESS_NONE}, { .Interlace = 0, .TimeScale = 60000, .NumUnitsInTick = 1001}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P50       */ {{1920,  B8_SERDES_RATE_2P3G, B8_COMPRESS_NONE}, { .Interlace = 0, .TimeScale =    50, .NumUnitsInTick =    1}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P25       */ {{1920, B8_SERDES_RATE_2P3G, B8_COMPRESS_NONE}, { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P50_HDR   */ {{1920,  B8_SERDES_RATE_2P3G, B8_COMPRESS_5P75}, { .Interlace = 0, .TimeScale =    50, .NumUnitsInTick =    1}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P25_HDR_0 */ {{1920,  B8_SERDES_RATE_2P3G, B8_COMPRESS_5P75}, { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P25_HDR_1 */ {{1920,  B8_SERDES_RATE_2P3G, B8_COMPRESS_5P75}, { .Interlace = 0, .TimeScale =    25, .NumUnitsInTick =    1}, 0.0f, 0.0f},
    /* B8N_IMX290_1820_400_P50_HDR    */ {{1820,  B8_SERDES_RATE_2P3G, B8_COMPRESS_NONE}, { .Interlace = 0, .TimeScale =    50, .NumUnitsInTick =    1}, 0.0f, 0.0f},
    /* B8N_IMX290_1920_1080_P20       */ {{1920, B8_SERDES_RATE_2P3G, B8_COMPRESS_NONE}, { .Interlace = 0, .TimeScale =    20, .NumUnitsInTick =    1}, 0.0f, 0.0f},
};

const AMBA_SENSOR_HDR_INFO_s B8N_IMX290HdrInfo[B8N_IMX290_NUM_MODE] = {
    [B8N_IMX290_1920_1080_P60] = {0},
    [B8N_IMX290_1920_1080_P30] = {0},
    [B8N_IMX290_1920_1080_P60_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 20, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 2236,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1920, 120, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 198,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1920_1080_P30_HDR_0] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 20, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 3822,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1920, 357, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 672,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1920_1080_P30_HDR_1] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 3,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 20, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 4350,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1920, 64, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 129,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {3840, 68, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 9,
                . MinExposureLine = 1,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1820_400_P60_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 12, 1820, 400},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 1052,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1820, 46, 1820, 400},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 66,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1920_500_P60_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 3,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 6, 1920, 500},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 1540,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1920, 29, 1920, 500},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 66,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {3840, 32, 1920, 500},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 6,
                . MinExposureLine = 1,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1096_736_P60_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 12, 1096, 736},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 2112,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1096, 79, 1096, 736},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 132,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1920_1080_P50] = {0},
    [B8N_IMX290_1920_1080_P25] = {0},
    [B8N_IMX290_1920_1080_P50_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 20, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 2236,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1920, 120, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 198,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1920_1080_P25_HDR_0] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 20, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 3822,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1920, 357, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 672,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1920_1080_P25_HDR_1] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 3,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 20, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 4350,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1920, 64, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 129,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {3840, 68, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 9,
                . MinExposureLine = 1,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1820_400_P50_HDR] = {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0, 12, 1820, 400},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 1052,
                . MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1820, 46, 1820, 400},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                . MaxExposureLine = 66,
                . MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                . MaxExposureLine = 0,
                . MinExposureLine = 0,
            },
        },
    },
    [B8N_IMX290_1920_1080_P20] = {0}
};

