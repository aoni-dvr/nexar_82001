/**
 *  @file AmbaSensor_IMX686Table.c
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
 *  @details Control APIs of SONY IMX686 CMOS sensor with MIPI interface
 *
 */

#include "AmbaSensor.h"
#include "AmbaSensor_IMX686.h"

//#define IMX686_1899M

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s IMX686DeviceInfo = {
    .UnitCellWidth          = 0.8f,
    .UnitCellHeight         = 0.8f,
    .NumTotalPixelCols      = 9344,
    .NumTotalPixelRows      = 7024,
    .NumEffectivePixelCols  = 9248,
    .NumEffectivePixelRows  = 6944,
    .MinAnalogGainFactor    = 1.122807018f,
    .MaxAnalogGainFactor    = 64.0f, /* 36.12359948 dB */
    .MinDigitalGainFactor   = 1.0f,
    .MaxDigitalGainFactor   = 15.85f,      /* 24.080 dB */

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
    .HdrIsSupport = 1,
};

IMX686_REG_s IMX686GlobalRegTable[IMX686_NUM_GOLBAL_REG] = {
    /*External clock setting*/
    {0x0136,    0x18},
    {0x0137,    0x00},
    /*Register version*/
    {0x33F0,    0x02},
    {0x33F1,    0x08},
    /*Singaling mode setting*/
#ifdef IMX686_CPHY
    {0x0111,    0x03},
#else
    {0x0111,    0x02},
#endif

#if 0
    /* AE-Hist1 data type Setting    */
    {0x3068,    0x00},
    {0x3069,    0x30},

    /* AE-Hist1 Ave data type Setting    */
    {0x306A,    0x00},
    {0x306B,    0x30},
    /* Flicker data type Setting */
    {0x3070,    0x00},
    {0x3071,    0x30},
    /* AE-Hist2 data type Setting    */
    {0x306C,    0x00},
    {0x306D,    0x30},
    /* AE-Hist2 Ave data type Setting */
    {0x306E,    0x00},
    {0x306F,    0x30},
    /* PDAF TYPE1 data type Setting */
    {0x3062,    0x00},
    {0x3063,    0x30},
#endif
    /*Global Setting*/
    {0x4008, 0x10},
    {0x4009, 0x10},
    {0x400A, 0x10},
    {0x400B, 0x10},
    {0x400C, 0x10},
    {0x400F, 0x01},
    {0x4011, 0x01},
    {0x4013, 0x01},
    {0x4015, 0x01},
    {0x4017, 0x40},
    {0x4FA3, 0x1F},
    {0x4FA5, 0xA6},
    {0x4FA7, 0x63},
    {0x4FF7, 0x1F},
    {0x4FF9, 0xEA},
    {0x4FFB, 0x63},
    {0x59DD, 0x02},
    {0x5B72, 0x05},
    {0x5B74, 0x06},
    {0x5B86, 0x05},
    {0x5BCC, 0x0B},
    {0x5C59, 0x52},
    {0x5C5C, 0x48},
    {0x5C5E, 0x48},
    {0x5EDA, 0x02},
    {0x5EDB, 0x02},
    {0x5EDF, 0x02},
    {0x5EE7, 0x02},
    {0x5EE8, 0x02},
    {0x5EEC, 0x02},
    {0x5F3E, 0x19},
    {0x5F40, 0x0F},
    {0x5F42, 0x05},
    {0x5F50, 0x32},
    {0x5F52, 0x32},
    {0x5F54, 0x2D},
    {0x6069, 0x15},
    {0x606B, 0x15},
    {0x606F, 0x14},
    {0x607D, 0x14},
    {0x607F, 0x14},
    {0x6083, 0x14},
    {0x60E9, 0x1B},
    {0x6104, 0x00},
    {0x6105, 0x1C},
    {0x6108, 0x00},
    {0x6109, 0x1C},
    {0x6110, 0x00},
    {0x6111, 0x1C},
    {0x61C9, 0x5F},
    {0x61E4, 0x00},
    {0x61E5, 0x60},
    {0x61E8, 0x00},
    {0x61E9, 0x60},
    {0x61F0, 0x00},
    {0x61F1, 0x60},
    {0x9003, 0x08},
    {0x9004, 0x18},
    {0x9200, 0x54},
    {0x9201, 0x4A},
    {0x9202, 0x54},
    {0x9203, 0x4D},
    {0x9204, 0x58},
    {0x9205, 0x59},
    {0x9206, 0x57},
    {0x9207, 0x1A},
    {0x9208, 0x50},
    {0x9209, 0xF8},
    {0x920A, 0x50},
    {0x920B, 0xF9},
    {0x9210, 0xEA},
    {0x9211, 0x7A},
    {0x9212, 0xEA},
    {0x9213, 0x7D},
    {0x9214, 0xEA},
    {0x9215, 0x80},
    {0x9216, 0xEA},
    {0x9217, 0x83},
    {0x9218, 0xEA},
    {0x9219, 0x86},
    {0x921A, 0xEA},
    {0x921B, 0xB8},
    {0x921C, 0xEA},
    {0x921D, 0xB9},
    {0x921E, 0xEA},
    {0x921F, 0xBE},
    {0x9220, 0xEA},
    {0x9221, 0xBF},
    {0x9222, 0xEA},
    {0x9223, 0xC4},
    {0x9224, 0xEA},
    {0x9225, 0xC5},
    {0x9226, 0xEA},
    {0x9227, 0xCA},
    {0x9228, 0xEA},
    {0x9229, 0xCB},
    {0x922A, 0xEA},
    {0x922B, 0xD0},
    {0x922C, 0xEA},
    {0x922D, 0xD1},
    {0x922E, 0x91},
    {0x922F, 0x2A},
    {0x9230, 0xE2},
    {0x9231, 0xC0},
    {0x9232, 0xE2},
    {0x9233, 0xC1},
    {0x9234, 0xE2},
    {0x9235, 0xC2},
    {0x9236, 0xE2},
    {0x9237, 0xC3},
    {0x9238, 0xE2},
    {0x9239, 0xD4},
    {0x923A, 0xE2},
    {0x923B, 0xD5},
    {0x923C, 0x43},
    {0x923D, 0x28},
    {0x923E, 0x43},
    {0x923F, 0x29},
    {0xBC76, 0x10},
    {0xBC7A, 0x34},
    {0xBC7B, 0xA0},
    {0xBC7C, 0x12},
    {0xBC7D, 0xB0},
    {0xBC7E, 0x1B},
    {0xBC80, 0x1B},
    {0xBC81, 0x90},
    {0xBC82, 0x16},
    {0xBC83, 0x60},
    {0xBC84, 0x3F},
    {0xBC85, 0xF0},
    {0xBC86, 0x15},
    {0xBC87, 0xE0},
    {0xBC88, 0x19},
    {0xBC89, 0xD0},
#if 0 //#ifdef IMX686_CPHY
    /* global timing */
#ifdef IMX686_1899M
    {0x0808,    0x02},
    {0x084e,    0x00},
    {0x084f,    17},
    {0x0850,    0x00},
    {0x0851,    15},
    {0x0852,    0x00},
    {0x0853,    29},
    {0x0854,    0x00},
    {0x0855,    0x32}, /* default:20, manual change*/
    {0x0858,    0x00},
    {0x0859,    25},
#else /* 3798M */
    {0x0808,    0x02},
    {0x084e,    0x00},
    {0x084f,    33},
    {0x0850,    0x00},
    {0x0851,    27},
    {0x0852,    0x00},
    {0x0853,    55},
    {0x0854,    0x00},
    {0x0855,    0x32}, /* default:20, manual change*/
    {0x0858,    0x00},
    {0x0859,    25},

#endif
#endif

};

/* Image Quality adjustment setting */
IMX686_REG_s IMX686ImageQualityRegTable[IMX686_NUM_IMG_QUALITY_REG] = {
    {0x3547,    0x00},
    {0x3549,    0x00},
    {0x354B,    0x00},
    {0x354D,    0x00},
    {0x85B1,    0x01},
    {0x9865,    0xA0},
    {0x9866,    0x14},
    {0x9867,    0x0A},
    {0x98D7,    0xB4},
    {0x98D8,    0x8C},
    {0x98DA,    0xA0},
    {0x98DB,    0x78},
    {0x98DC,    0x50},
    {0x99B8,    0x17},
    {0x99BA,    0x17},
    {0x99C4,    0x16},
    {0x9A12,    0x15},
    {0x9A13,    0x15},
    {0x9A14,    0x15},
    {0x9A15,    0x0B},
    {0x9A16,    0x0B},
    {0x9A49,    0x0B},
    {0x9A4A,    0x0B},
    {0xA503,    0x04},
    {0xA539,    0x03},
    {0xA53A,    0x03},
    {0xA53B,    0x03},
    {0xA575,    0x03},
    {0xA576,    0x03},
    {0xA577,    0x03},
    {0xA57A,    0x80},
    {0xA660,    0x01},
    {0xA661,    0x69},
    {0xA66C,    0x01},
    {0xA66D,    0x27},
    {0xA673,    0x40},
    {0xA675,    0x40},
    {0xA677,    0x43},
    {0xA67D,    0x06},
    {0xA6DE,    0x01},
    {0xA6DF,    0x69},
    {0xA6EA,    0x01},
    {0xA6EB,    0x27},
    {0xA6F1,    0x40},
    {0xA6F3,    0x40},
    {0xA6F5,    0x43},
    {0xA6FB,    0x06},
    {0xA76D,    0x40},
    {0xA76F,    0x40},
    {0xA771,    0x43},
    {0xAA37,    0x76},
    {0xAA39,    0xAC},
    {0xAA3B,    0xC8},
    {0xAA3D,    0x76},
    {0xAA3F,    0xAC},
    {0xAA41,    0xC8},
    {0xAA43,    0x76},
    {0xAA45,    0xAC},
    {0xAA47,    0xC8},
    {0xAD1C,    0x01},
    {0xAD1D,    0x3D},
    {0xAD23,    0x4F},
    {0xAD4C,    0x01},
    {0xAD4D,    0x3D},
    {0xAD53,    0x4F},
    {0xAD7C,    0x01},
    {0xAD7D,    0x3D},
    {0xAD83,    0x4F},
    {0xADAC,    0x01},
    {0xADAD,    0x3D},
    {0xADB3,    0x4F},
    {0xAE00,    0x01},
    {0xAE01,    0xA9},
    {0xAE02,    0x01},
    {0xAE03,    0xA9},
    {0xAE05,    0x86},
    {0xAE0D,    0x10},
    {0xAE0F,    0x10},
    {0xAE11,    0x10},
    {0xAE24,    0x03},
    {0xAE25,    0x03},
    {0xAE26,    0x02},
    {0xAE27,    0x49},
    {0xAE28,    0x01},
    {0xAE29,    0x3B},
    {0xAE31,    0x10},
    {0xAE33,    0x10},
    {0xAE35,    0x10},
    {0xAE48,    0x02},
    {0xAE4A,    0x01},
    {0xAE4B,    0x80},
    {0xAE4D,    0x80},
    {0xAE55,    0x10},
    {0xAE57,    0x10},
    {0xAE59,    0x10},
    {0xAE6C,    0x01},
    {0xAE6D,    0xC1},
    {0xAE6F,    0xA5},
    {0xAE79,    0x10},
    {0xAE7B,    0x10},
    {0xAE7D,    0x13},
    {0xAE90,    0x04},
    {0xAE91,    0xB0},
    {0xAE92,    0x01},
    {0xAE93,    0x70},
    {0xAE94,    0x01},
    {0xAE95,    0x3B},
    {0xAE9D,    0x10},
    {0xAE9F,    0x10},
    {0xAEA1,    0x10},
    {0xAEB4,    0x02},
    {0xAEB5,    0xCB},
    {0xAEB6,    0x01},
    {0xAEB7,    0x58},
    {0xAEB9,    0xB4},
    {0xAEC1,    0x10},
    {0xAEC3,    0x10},
    {0xAEC5,    0x10},
    {0xAF01,    0x13},
    {0xAF02,    0x00},
    {0xAF08,    0x78},
    {0xAF09,    0x6E},
    {0xAF0A,    0x64},
    {0xAF0B,    0x5A},
    {0xAF0C,    0x50},
    {0xAF0D,    0x46},
    {0xAF0E,    0x3C},
    {0xAF0F,    0x32},
    {0xAF10,    0x28},
    {0xAF11,    0x00},
    {0xAF17,    0x50},
    {0xAF18,    0x3C},
    {0xAF19,    0x28},
    {0xAF1A,    0x14},
    {0xAF1B,    0x00},
    {0xAF26,    0xA0},
    {0xAF27,    0x96},
    {0xAF28,    0x8C},
    {0xAF29,    0x82},
    {0xAF2A,    0x78},
    {0xAF2B,    0x6E},
    {0xAF2C,    0x64},
    {0xAF2D,    0x5A},
    {0xAF2E,    0x50},
    {0xAF2F,    0x00},
    {0xAF31,    0x96},
    {0xAF32,    0x8C},
    {0xAF33,    0x82},
    {0xAF34,    0x78},
    {0xAF35,    0x6E},
    {0xAF36,    0x64},
    {0xAF38,    0x3C},
    {0xAF39,    0x00},
    {0xAF3A,    0xA0},
    {0xAF3B,    0x96},
    {0xAF3C,    0x8C},
    {0xAF3D,    0x82},
    {0xAF3E,    0x78},
    {0xAF3F,    0x6E},
    {0xAF40,    0x64},
    {0xAF41,    0x50},
    {0xAF94,    0x03},
    {0xAF95,    0x02},
    {0xAF96,    0x02},
    {0xAF99,    0x01},
    {0xAF9B,    0x02},
    {0xAFA5,    0x01},
    {0xAFA7,    0x03},
    {0xAFB4,    0x02},
    {0xAFB5,    0x02},
    {0xAFB6,    0x03},
    {0xAFB7,    0x03},
    {0xAFB8,    0x03},
    {0xAFB9,    0x04},
    {0xAFBA,    0x04},
    {0xAFBC,    0x03},
    {0xAFBD,    0x03},
    {0xAFBE,    0x02},
    {0xAFBF,    0x02},
    {0xAFC0,    0x02},
    {0xAFC3,    0x01},
    {0xAFC5,    0x03},
    {0xAFC6,    0x04},
    {0xAFC7,    0x04},
    {0xAFC8,    0x03},
    {0xAFC9,    0x03},
    {0xAFCA,    0x02},
    {0xAFCC,    0x01},
    {0xAFCE,    0x02},
    {0xB02A,    0x00},
    {0xB02E,    0x02},
    {0xB030,    0x02},
    {0xB501,    0x02},
    {0xB503,    0x02},
    {0xB505,    0x02},
    {0xB507,    0x02},
    {0xB515,    0x00},
    {0xB517,    0x00},
    {0xB519,    0x02},
    {0xB51F,    0x00},
    {0xB521,    0x01},
    {0xB527,    0x02},
    {0xB53D,    0x01},
    {0xB53F,    0x02},
    {0xB541,    0x02},
    {0xB543,    0x02},
    {0xB545,    0x02},
    {0xB547,    0x02},
    {0xB54B,    0x03},
    {0xB54D,    0x03},
    {0xB551,    0x02},
    {0xB553,    0x02},
    {0xB555,    0x02},
    {0xB557,    0x02},
    {0xB559,    0x02},
    {0xB55B,    0x02},
    {0xB55D,    0x01},
    {0xB563,    0x02},
    {0xB565,    0x03},
    {0xB567,    0x03},
    {0xB569,    0x02},
    {0xB56B,    0x02},
    {0xB58D,    0xE7},
    {0xB58F,    0xCC},
    {0xB591,    0xAD},
    {0xB593,    0x88},
    {0xB595,    0x66},
    {0xB597,    0x88},
    {0xB599,    0xAD},
    {0xB59B,    0xCC},
    {0xB59D,    0xE7},
    {0xB5A1,    0x2A},
    {0xB5A3,    0x1A},
    {0xB5A5,    0x27},
    {0xB5A7,    0x1A},
    {0xB5A9,    0x2A},
    {0xB5AB,    0x3C},
    {0xB5AD,    0x59},
    {0xB5AF,    0x77},
    {0xB5B1,    0x9A},
    {0xB5B3,    0xE9},
    {0xB5C9,    0x5B},
    {0xB5CB,    0x73},
    {0xB5CD,    0x9D},
    {0xB5CF,    0xBA},
    {0xB5D1,    0xD9},
    {0xB5D3,    0xED},
    {0xB5D5,    0xF9},
    {0xB5D7,    0xFE},
    {0xB5D8,    0x01},
    {0xB5D9,    0x00},
    {0xB5DA,    0x01},
    {0xB5DB,    0x00},
    {0xB5DD,    0xF6},
    {0xB5DF,    0xE9},
    {0xB5E1,    0xD1},
    {0xB5E3,    0xBB},
    {0xB5E5,    0x9A},
    {0xB5E7,    0x77},
    {0xB5E9,    0x59},
    {0xB5EB,    0x77},
    {0xB5ED,    0x9A},
    {0xB5EF,    0xE9},
    {0xB600,    0x01},
    {0xB601,    0x00},
    {0xB603,    0xFE},
    {0xB605,    0xF8},
    {0xB607,    0xED},
    {0xB609,    0xD4},
    {0xB60B,    0xB7},
    {0xB60D,    0x93},
    {0xB60F,    0xB7},
    {0xB611,    0xD4},
    {0xB612,    0x00},
    {0xB613,    0xFE},
    {0xB628,    0x00},
    {0xB629,    0xAA},
    {0xB62A,    0x00},
    {0xB62B,    0x78},
    {0xB62D,    0x55},
    {0xB62F,    0x3E},
    {0xB631,    0x2B},
    {0xB633,    0x20},
    {0xB635,    0x18},
    {0xB637,    0x12},
    {0xB639,    0x0E},
    {0xB63B,    0x06},
    {0xB63C,    0x02},
    {0xB63D,    0xAA},
    {0xB63E,    0x02},
    {0xB63F,    0x00},
    {0xB640,    0x01},
    {0xB641,    0x99},
    {0xB642,    0x01},
    {0xB643,    0x24},
    {0xB645,    0xCC},
    {0xB647,    0x66},
    {0xB649,    0x38},
    {0xB64B,    0x21},
    {0xB64D,    0x14},
    {0xB64F,    0x0E},
    {0xB664,    0x00},
    {0xB665,    0xCC},
    {0xB666,    0x00},
    {0xB667,    0x92},
    {0xB669,    0x66},
    {0xB66B,    0x4B},
    {0xB66D,    0x34},
    {0xB66F,    0x28},
    {0xB671,    0x1E},
    {0xB673,    0x18},
    {0xB675,    0x11},
    {0xB677,    0x08},
    {0xB678,    0x04},
    {0xB679,    0x00},
    {0xB67A,    0x04},
    {0xB67B,    0x00},
    {0xB67C,    0x02},
    {0xB67D,    0xAA},
    {0xB67E,    0x02},
    {0xB67F,    0x00},
    {0xB680,    0x01},
    {0xB681,    0x99},
    {0xB682,    0x01},
    {0xB683,    0x24},
    {0xB685,    0xCC},
    {0xB687,    0x66},
    {0xB689,    0x38},
    {0xB68B,    0x0E},
    {0xB68C,    0x02},
    {0xB68D,    0xAA},
    {0xB68E,    0x02},
    {0xB68F,    0x00},
    {0xB690,    0x01},
    {0xB691,    0x99},
    {0xB692,    0x01},
    {0xB693,    0x24},
    {0xB695,    0xE3},
    {0xB697,    0x9D},
    {0xB699,    0x71},
    {0xB69B,    0x37},
    {0xB69D,    0x1F},
    {0xE869,    0x00},
    {0xE877,    0x00},
    {0xEE01,    0x30},
    {0xEE03,    0x30},
    {0xEE07,    0x08},
    {0xEE09,    0x08},
    {0xEE0B,    0x08},
    {0xEE0D,    0x30},
    {0xEE0F,    0x30},
    {0xEE12,    0x00},
    {0xEE13,    0x10},
    {0xEE14,    0x00},
    {0xEE15,    0x10},
    {0xEE16,    0x00},
    {0xEE17,    0x10},
    {0xEE31,    0x30},
    {0xEE33,    0x30},
    {0xEE3D,    0x30},
    {0xEE3F,    0x30},
    {0xF645,    0x40},
    {0xF646,    0x01},
    {0xF647,    0x00},

};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/
IMX686_MODE_REG_s IMX686ModeRegTable[IMX686_NUM_READOUT_MODE_REG] = {
    /*
        readout   7680x  8000x  4624x  3840x  3840x  3840x
                  4320   6000   2608   2160   2160   2160
                  Normal Normal Normal Normal Normal Normal
                  RAW10  RAW10  RAW10  RAW10  RAW10  RAW10
     */
    /*MIPI output setting*/
    {0x0112,  {0x0A,  0x0A, 0x0A,  0x0A,  0x0A,  0x0A,}},  /* CSI_DT_FMT_H */
    {0x0113,  {0x0A,  0x0A, 0x0A,  0x0A,  0x0A,  0x0A,}},  /* CSI_DT_FMT_L */
#ifdef IMX686_CPHY
    {0x0114,  {0x02,  0x02, 0x02,  0x02,  0x02,  0x02,}},  /* CSI_LANE_MODE */
#else
    {0x0114,  {0x03,  0x03, }},  /* CSI_LANE_MODE */
#endif
    /*ROI setting*/
    {0x0344,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},  /* X_ADD_STA[12:8] */
    {0x0345,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},  /* X_ADD_STA[7:0] */
    {0x0346,  {0x05,  0x00, 0x03,  0x05,  0x05,  0x05,}},  /* Y_ADD_STA[12:8] */
    {0x0347,  {0x20,  0x00, 0x60,  0x20,  0x20,  0x20,}},  /* Y_ADD_STA[7:0] */
    {0x0348,  {0x24,  0x24, 0x24,  0x24,  0x24,  0x24,}},  /* X_ADD_END[12:8] */
    {0x0349,  {0x1F,  0x1F, 0x1F,  0x1F,  0x1F,  0x1F,}},  /* X_ADD_END[7:0] */
    {0x034A,  {0x15,  0x1B, 0x17,  0x15,  0x15,  0x15,}},  /* Y_ADD_END[12:8] */
    {0x034B,  {0xFF,  0x1F, 0xBF,  0xFF,  0xFF,  0xFF,}},  /* Y_ADD_END[7:0] */
    /*Mode setting*/
    {0x0220,  {0x62,  0x62, 0x62,  0x62,  0x62,  0x62,}},  /* HDR_MODE */
    {0x0221,  {0x11,  0x11, 0x11,  0x11,  0x11,  0x11,}},  /* HDR_MODE */
    {0x0222,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}},  /* HDR_RESO_REDU_V */
    {0x0900,  {0x00,  0x00, 0x01,  0x01,  0x01,  0x01,}},  /* BINNING_MODE */
    {0x0901,  {0x11,  0x11, 0x22,  0x22,  0x22,  0x22,}},  /* [7:4] BINNING_TYPE_H, [3:0] BINNING_TYPE_V */
    {0x0902,  {0x0A,  0x0A, 0x08,  0x08,  0x08,  0x08,}},  /* BINNING_WEIGHTING */

    {0x30D8,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x3200,  {0x01,  0x01, 0x41,  0x41,  0x41,  0x41,}},
    {0x3201,  {0x01,  0x01, 0x41,  0x41,  0x41,  0x41,}},
    {0x350C,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x350D,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},

    /*Digital Crop & Scaling*/
    {0x0408,  {0x03,  0x00, 0x00,  0x01,  0x01,  0x01,}},  /* DIG_CROP_X_OFFSET[12:8] */
    {0x0409,  {0x10,  0x00, 0x00,  0x88,  0x88,  0x88,}},  /* DIG_CROP_X_OFFSET[7:0] */
    {0x040A,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},  /* DIG_CROP_Y_OFFSET[12:8] */
    {0x040B,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},  /* DIG_CROP_Y_OFFSET[7:0] */
    {0x040C,  {0x1e,  0x24, 0x12,  0x0F,  0x0F,  0x0F,}},  /* DIG_CROP_IMAGE_WIDTH[12:8] */
    {0x040D,  {0x00,  0x20, 0x10,  0x00,  0x00,  0x00,}},  /* DIG_CROP_IMAGE_WIDTH[7:0] */
    {0x040E,  {0x10,  0x1B, 0x0A,  0x08,  0x08,  0x08,}},  /* DIG_CROP_IMAGE_HEIGHT[12:8] */
    {0x040F,  {0xe0,  0x20, 0x30,  0x70,  0x70,  0x70,}},  /* DIG_CROP_IMAGE_HEIGHT[7:0] */
    /*Output size setting*/
    {0x034C,  {0x1e,  0x24, 0x12,  0x0F,  0x0F,  0x0F,}},  /* X_OUT_SIZE[12:8] */
    {0x034D,  {0x00,  0x20, 0x10,  0x00,  0x00,  0x00,}},  /* X_OUT_SIZE[7:0] */
    {0x034E,  {0x10,  0x1B, 0x0A,  0x08,  0x08,  0x08,}},  /* Y_OUT_SIZE[12:8] */
    {0x034F,  {0xe0,  0x20, 0x30,  0x70,  0x70,  0x70,}},  /* Y_OUT_SIZE[7:0] */
    /*Clock setting*/
    {0x0301,  {0x08,  0x08, 0x08,  0x08,  0x08,  0x08,}},  /* IVT_PXCK_DIV */
    {0x0303,  {0x02,  0x04, 0x02,  0x02,  0x04,  0x04,}},  /* IVT_SYCK_DIV */
    {0x0305,  {0x04,  0x03, 0x04,  0x04,  0x04,  0x04,}},  /* IVT_PREPLLCK_DIV */
    {0x0306,  {0x01,  0x00, 0x01,  0x01,  0x01,  0x01,}},  /* IVT_PLL_MPY[10:8] */
    {0x0307,  {0x2f,  0xF2, 0x57,  0x4A,  0x4A,  0x4A,}},  /* IVT_PLL_MPY[7:0] */
    {0x030B,  {0x02,  0x01, 0x02,  0x02,  0x04,  0x08,}},  /* IOP_SYCK_DIV */
    {0x030D,  {0x03,  0x03, 0x03,  0x03,  0x03,  0x03,}},  /* IOP_PREPLLCK_DIV */
#ifdef IMX686_CPHY
    {0x030E,  {0x01,  0x00, 0x01,  0x01,  0x01,  0x01,}},  /* IOP_PLL_MPY[10:8] */
    {0x030F,  {0x96,  0xd3, 0x90,  0xB9,  0xB3,  0xB0,}},  /* IOP_PLL_MPY[7:0] */
#else
    {0x030E,  {0x01,  0x01,   }},  /* IOP_PLL_MPY[10:8] */
    {0x030F,  {0xA0,  0xA0,   }},  /* IOP_PLL_MPY[7:0] */
#endif
    {0x0310,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}},  /* PLL_MULT_DRIV */
    /*Other setting*/

    {0x30D9,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}},
    {0x32D5,  {0x01,  0x01, 0x00,  0x00,  0x00,  0x00,}},
    {0x32D6,  {0x01,  0x01, 0x00,  0x00,  0x00,  0x00,}},
    {0x403D,  {0x10,  0x10, 0x0B,  0x0B,  0x0B,  0x0B,}},
    {0x403E,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x403F,  {0x78,  0x78, 0x78,  0x78,  0x78,  0x78,}},
    {0x40BC,  {0x00,  0x00, 0x01,  0x00,  0x00,  0x00,}},
    {0x40BD,  {0xA0,  0xA0, 0xF4,  0x78,  0x78,  0x78,}},
    {0x40BE,  {0x00,  0x00, 0x01,  0x00,  0x00,  0x00,}},
    {0x40BF,  {0xA0,  0xA0, 0xF4,  0x78,  0x78,  0x78,}},
    {0x40B8,  {0x01,  0x01, 0x04,  0x00,  0x00,  0x00,}},
    {0x40B9,  {0xF4,  0xF4, 0x10,  0xC8,  0xC8,  0xC8,}},
    {0x40A4,  {0x02,  0x02, 0x02,  0x01,  0x01,  0x01,}},
    {0x40A5,  {0xA8,  0xA8, 0x80,  0x54,  0x54,  0x54,}},
    {0x40A0,  {0x02,  0x02, 0x02,  0x01,  0x01,  0x01,}},
    {0x40A1,  {0xA8,  0xA8, 0x08,  0x54,  0x54,  0x54,}},
    {0x40A6,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x40A7,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x40AA,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x40AB,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x4000,  {0xE0,  0xE0, 0xE0,  0xE0,  0xE0,  0xE0,}},
    {0x4001,  {0xE2,  0xE2, 0xE2,  0xE2,  0xE2,  0xE2,}},
    {0x4002,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}},
    {0x4003,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x4004,  {0xFF,  0xFF, 0xFF,  0xFF,  0xFF,  0xFF,}},
    {0x4005,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x4006,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x4007,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x401E,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x401F,  {0xCC,  0xCC, 0xCC,  0xCC,  0xCC,  0xCC,}},
    {0x59EE,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x59D1,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}},
    {0xAF06,  {0x03,  0x03, 0x07,  0x07,  0x07,  0x07,}},
    {0xAF07,  {0xFB,  0xFB, 0xF1,  0xF1,  0xF1,  0xF1,}},

    /*Intergration setting*/
    {0x0202,  {0x10,  0x1b, 0x0A,  0x08,  0x08,  0x08,}}, /*COARSE_INTEG_TIME[15:8]*/
#ifdef IMX686_CPHY    /* TBD, mode 1 ~ mode 6*/
    {0x0203,  {0xfe,  0x37, 0xF8,  0x64,  0x64,  0x64,}}, /*COARSE_INTEG_TIME[7:0]*/
#else
    {0x0203,  {0xF9,  0xF9,  }}, /*COARSE_INTEG_TIME[7:0]*/
#endif
    {0x0224,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}}, /*ST_COARSE_INTEG_TIME[15:8]*/
    {0x0225,  {0xF4,  0xF4, 0xF4,  0xF4,  0xF4,  0xF4,}}, /*ST_COARSE_INTEG_TIME[7:0]*/
    {0x3116,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}},
    {0x3117,  {0xF4,  0xF4, 0xF4,  0xF4,  0xF4,  0xF4,}},
    /*Gain setting*/
    {0x0204,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}}, /*ANA_GAIN_GLOBAL[9:8]*/
    {0x0205,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}}, /*ANA_GAIN_GLOBAL[7:0]*/
    {0x0216,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}}, /*ST_ANA_GAIN_GLOBAL[9:8]*/
    {0x0217,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}}, /*ST_ANA_GAIN_GLOBAL[7:0]*/
    {0x0218,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}}, /*ST_DIG_GAIN_GLOBAL[15:8]*/
    {0x0219,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}}, /*ST_DIG_GAIN_GLOBAL[7:0]*/
    {0x020E,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}}, /*DIG_GAIN_GLOBAL[15:8]*/
    {0x020F,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}}, /*DIG_GAIN_GLOBAL[7:0]*/
    {0x3118,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}}, /*DIG_GAIN_R[15:8]*/
    {0x3119,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}}, /*DIG_GAIN_R[7:0]*/
    {0x311A,  {0x01,  0x01, 0x01,  0x01,  0x01,  0x01,}}, /*DIG_GAIN_B[15:8]*/
    {0x311B,  {0x00,  0x00, 0x00,  0x00,  0x00,  0x00,}}, /*DIG_GAIN_B[7:0]*/

    /* PDAF Setting */
    {0x4018,  {    0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x4019,  {    0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x401A,  {    0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x401B,  {    0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},

    /* HG MODE Setting */
    {0x30D0,  {    0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x30D1,  {    0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x30D2,  {    0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
#if 0
    /* AE-Hist1 Setting */
    {0x3094,  { 0x00, 0x00, }},

    /*  AE-Hist1 Ave Setting */
    {0x3095,  {0x00, 0x00, }},
    /* Flicker Setting */
    {0x3098,  {0x00, 0x00, }},
    /* AE-Hist2 Setting */
    {0x3096,  {0x00, 0x00, }},
    /* AE-Hist2 Ave Setting    */
    {0x3097,  {0x00, 0x00, }},
    /* PDAF TYPE Setting */
    {0x3400,  {0x01, 0x01, }},
    /* PDAF TYPE1 Setting */
    {0x3091,  {0x01, 0x01, }},
#endif
    /* manual adjust MIPI timing, required by samsung cphy */
    {0x0808,  { 0x02, 0x02, 0x02,  0x02,  0x02,  0x02,}},
    {0x084e,  { 0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x084f,  {   15,   17,   17,    15,     8,     3,}},
    {0x0850,  { 0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x0851,  {   13,   15,   15,    13,     7,     3,}},
    {0x0852,  { 0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x0853,  {   25,   29,   29,    25,    13,     7,}},
    {0x0854,  { 0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x0855,  { 0x32, 0x32, 0x32,  0x32,  0x32,  0x32,}}, /* default:20, manual change*/
    {0x0858,  { 0x00, 0x00, 0x00,  0x00,  0x00,  0x00,}},
    {0x0859,  {   25,   25,   25,    25,    25,    25,}},


};
/* 16bits to 7 symbols , data rate (bits/sec) = ((symbol/sec) / 7 * 16 )    */
const AMBA_SENSOR_OUTPUT_INFO_s IMX686OutputInfo[AMBA_SENSOR_IMX686_NUM_MODE] = {
    /* AMBA_SENSOR_IMX686_9248_6944_30P */      { 3712000000U, 3, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 7680, 4320, { 0, 0, 7680, 4320}, {0}},
    /* AMBA_SENSOR_IMX686_9248_6944_15P */      { 4340000000U, 3, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 8000, 6000, { 0, 0, 8000, 6000}, {0}},
    /* AMBA_SENSOR_IMX686_4624_2680_60P */      { 3657142857U, 3, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 4624, 2608, { 0, 0, 4624, 2608}, {0}},
    /* AMBA_SENSOR_IMX686_3840_2160_120P*/      { 4032000000U, 3, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 3840, 2160, { 0, 0, 3840, 2160}, {0}},
    /* AMBA_SENSOR_IMX686_3840_2160_60P */      { 1988571429U, 3, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 3840, 2160, { 0, 0, 3840, 2160}, {0}},
    /* AMBA_SENSOR_IMX686_3840_2160_30P */      {  987428571U, 3, 10, AMBA_SENSOR_BAYER_PATTERN_RG, 3840, 2160, { 0, 0, 3840, 2160}, {0}},
};

const AMBA_SENSOR_INPUT_INFO_s IMX686InputInfoNormalReadout[AMBA_SENSOR_IMX686_NUM_MODE] = {
    /* AMBA_SENSOR_IMX686_9248_6944_30P */     {{   0,   0, 7680, 4320}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* AMBA_SENSOR_IMX686_9248_6944_15P */     {{   0,   0, 8000, 6000}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL,  1, 1}, {  AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* AMBA_SENSOR_IMX686_4624_2680_60P */     {{   0,   0, 4624, 2608}, {  AMBA_SENSOR_SUBSAMPLE_BINNING, 1, 2}, {  AMBA_SENSOR_SUBSAMPLE_BINNING,1, 2}, 1},
    /* AMBA_SENSOR_IMX686_3840_2160_120P*/     {{   0,   0, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_BINNING, 1, 2}, {  AMBA_SENSOR_SUBSAMPLE_BINNING,1, 2}, 1},
    /* AMBA_SENSOR_IMX686_3840_2160_60P */     {{   0,   0, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_BINNING, 1, 2}, {  AMBA_SENSOR_SUBSAMPLE_BINNING,1, 2}, 1},
    /* AMBA_SENSOR_IMX686_3840_2160_30P */     {{   0,   0, 3840, 2160}, {  AMBA_SENSOR_SUBSAMPLE_BINNING, 1, 2}, {  AMBA_SENSOR_SUBSAMPLE_BINNING,1, 2}, 1},
};

/* Input clock, 45Mhz if using bridge board on-board OSC, follow sensor mode setting if adopt clk-si */
const IMX686_MODE_INFO_s IMX686ModeInfoList[AMBA_SENSOR_IMX686_NUM_MODE] = {
    /* AMBA_SENSOR_IMX686_9248_6944_30P     */    {{ 24000000, 1818000000, 13728, 4414, {.Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1000}}, AMBA_SENSOR_HDR_NONE},
    /* AMBA_SENSOR_IMX686_9248_6944_15P     */    {{ 24000000, 1728000000, 10336, 7031, {.Interlace = 0, .TimeScale =  15000, .NumUnitsInTick = 1000}}, AMBA_SENSOR_HDR_NONE},
    /* AMBA_SENSOR_IMX686_4624_2680_60P     */    {{ 24000000, 2058000000, 11936, 2874, {.Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1000}}, AMBA_SENSOR_HDR_NONE},
    /* AMBA_SENSOR_IMX686_3840_2160_120P    */    {{ 24000000, 1980000000,  7456, 2212, {.Interlace = 0, .TimeScale = 120000, .NumUnitsInTick = 1000}}, AMBA_SENSOR_HDR_NONE},
    /* AMBA_SENSOR_IMX686_3840_2160_60P     */    {{ 24000000,  990000000,  7456, 2212, {.Interlace = 0, .TimeScale =  60000, .NumUnitsInTick = 1000}}, AMBA_SENSOR_HDR_NONE},
    /* AMBA_SENSOR_IMX686_3840_2160_30P     */    {{ 24000000,  990000000, 14912, 2212, {.Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1000}}, AMBA_SENSOR_HDR_NONE},
};

const AMBA_SENSOR_HDR_INFO_s IMX686HdrInfo[AMBA_SENSOR_IMX686_NUM_MODE] = {
};
