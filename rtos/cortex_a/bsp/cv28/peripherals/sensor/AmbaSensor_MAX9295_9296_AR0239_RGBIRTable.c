/**
 *  @file AmbaSensor_MAX9295_9296_AR0239_RGBIRTable.c
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
 *  @details Control APIs of MAXIM 9295/9296 serdes plus OnSemi AR0239 CMOS sensor with MIPI interface
 *
 */


#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_AR0239_RGBIR.h"

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Information
\*-----------------------------------------------------------------------------------------------*/
const AMBA_SENSOR_DEVICE_INFO_s MX00_AR0239_DeviceInfo = {
    .UnitCellWidth          = 3.0f,
    .UnitCellHeight         = 3.0f,
    .NumTotalPixelCols      = 1944,
    .NumTotalPixelRows      = 1188,
    .NumEffectivePixelCols  = 1936,
    .NumEffectivePixelRows  = 1188,
    .MinAnalogGainFactor    = 1.0f,
    .MaxAnalogGainFactor    = 85.333f,
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
    .StrobeCtrlInfo = {0},
    .HdrIsSupport = 1,
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings of each readout modes
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings for PLL
\*-----------------------------------------------------------------------------------------------*/
MX00_AR0239_REG_s MX00_AR0239_PllRegTable[MX00_AR0239_NUM_PLL_REG] = {
    {0x302A, 0x0006}, /* vt_pix_clk_div    */
    {0x302C, 0x0001}, /* vt_sys_clk_div    */
    {0x302E, 0x0009}, /* pre_pll_clk_div   */
    {0x3030, 0x0090}, /* pll_multiplier    */
    {0x3036, 0x000c}, /* op_pix_clk_div    */
    {0x3038, 0x0001}, /* op_sys_clk_div    */
    {0x31AC, 0x0c0c}, /* data_format_bits  */
    {0x31AE, 0x0204}, /* serial_format     */
    {0x3354, 0x002C}, /* mipi_cntrl        */
    {0x30B0, 0x022A}, /* disable_hispi_crc */
    {0x31B0, 0x00B1}, /* FRAME_PREAMBLE    */
    {0x31B2, 0x0071}, /* LINE_PREAMBLE     */
    {0x31B4, 0x1B87}, /* MIPI_TIMING_0     */
    {0x31B6, 0x2218}, /* MIPI_TIMING_1     */
    {0x31B8, 0x7049}, /* MIPI_TIMING_2     */
    {0x31BA, 0x0209}, /* MIPI_TIMING_3     */
    {0x31BC, 0x8008}, /* MIPI_TIMING_4     */
};

MX00_AR0239_REG_s MX00_AR0239_2LANE_PllRegTable[MX00_AR0239_NUM_PLL_REG] = {
    {0x302A,      6}, /* vt_pix_clk_div    */
    {0x302C,      2}, /* vt_sys_clk_div    */
    {0x302E,      9}, /* pre_pll_clk_div   */
    {0x3030,    128}, /* pll_multiplier    */
    {0x3036,     12}, /* op_pix_clk_div    */
    {0x3038,      1}, /* op_sys_clk_div    */
    {0x31AC, 0x0C0C}, /* data_format_bits  */
    {0x31AE, 0x0202}, /* serial_format     */
    {0x3354, 0x002C}, /* mipi_cntrl        */
    {0x30B0, 0x022A}, /* disable_hispi_crc */
    {0x31B0, 0x0090}, /* FRAME_PREAMBLE    */
    {0x31B2, 0x0065}, /* LINE_PREAMBLE     */
    {0x31B4, 0x2A86}, /* MIPI_TIMING_0     */
    {0x31B6, 0x21D6}, /* MIPI_TIMING_1     */
    {0x31B8, 0x6049}, /* MIPI_TIMING_2     */
    {0x31BA, 0x0208}, /* MIPI_TIMING_3     */
    {0x31BC, 0x8007}, /* MIPI_TIMING_4     */
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings for Recommended ES
\*-----------------------------------------------------------------------------------------------*/
MX00_AR0239_REG_s MX00_AR0239_RecomESRegTable[MX00_AR0239_NUM_RECOM_ES_REG] = {
    {0x3EFC, 0xE0FF},
    {0x30AE, 0x0003},
    {0x3092, 0x086F},
    {0x30BA, 0x762C},
    {0x317E, 0x8F70},
    {0x3180, 0x8040},
    {0x3ED2, 0x0099},
    {0x3ED6, 0x14A1},
    {0x3ED8, 0xF9EE},
    {0x3EDA, 0x6E40},
    {0x3EDC, 0x08C5},
    {0x3EDE, 0x11A1},
    {0x3EE0, 0x118A},
    {0x3EE2, 0x31C6},
    {0x3EE4, 0x0D77},
    {0x3EFE, 0x96F2},
    {0x3F04, 0x0C18},
    {0x3ED0, 0xC400},
    {0x3ED4, 0x03CF},
    {0x3ECE, 0x40F3},
    {0x3F02, 0x2E1C},
    {0x3EF4, 0x5805},
    {0x3EF2, 0x9464},
    {0x3EF2, 0x9464},
    {0x3EF2, 0x9464},
    {0x3EF4, 0x5805},
    {0x3EF4, 0x5805},
    {0x3EF4, 0x5805},
    {0x3EF6, 0x5253},
    {0x3EF6, 0x5253},
    {0x3EF6, 0x5253},
    {0x3EF8, 0x54F4},
    {0x3EF8, 0x54F4},
    {0x3EF8, 0x54F4},
    {0x3F4C, 0x52BC},
    {0x3F4C, 0x52BC},
    {0x3F4E, 0x3E3C},
    {0x3F4E, 0x3E3C},
    {0x3F50, 0x080B},
    //{0x3F50, 0x380B},
    {0x31E0, 0x000F},
    {0x3324, 0x001C},
    {0x30B4, 0x01C1},
};

/*-----------------------------------------------------------------------------------------------*\
 * Sensor register settings for Linear
\*-----------------------------------------------------------------------------------------------*/
MX00_AR0239_REG_s MX00_AR0239_LinearSeqRegTable[MX00_AR0239_NUM_LINEAR_SEQ_REG] = {
    {0x3088, 0x8000},
    {0x3086, 0x4558},
    {0x3086, 0x72A6},
    {0x3086, 0x4A31},
    {0x3086, 0x43C5},
    {0x3086, 0x2A08},
    {0x3086, 0x42CE},
    {0x3086, 0xCB8E},
    {0x3086,  0x345},
    {0x3086, 0x782A},
    {0x3086, 0x1C7B},
    {0x3086, 0xCA3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xEA2A},
    {0x3086,  0xC3D},
    {0x3086, 0x102A},
    {0x3086,  0xD2A},
    {0x3086, 0x1D35},
    {0x3086, 0x2A0D},
    {0x3086, 0x3D10},
    {0x3086, 0x4558},
    {0x3086, 0x2A0C},
    {0x3086, 0x8E03},
    {0x3086, 0x2A1C},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DEA},
    {0x3086, 0x2A0C},
    {0x3086, 0x62CC},
    {0x3086, 0xCF2A},
    {0x3086, 0x288E},
    {0x3086,   0x36},
    {0x3086, 0x2A08},
    {0x3086, 0x3D64},
    {0x3086, 0x7ACD},
    {0x3086, 0x3D04},
    {0x3086, 0x442A},
    {0x3086, 0x982C},
    {0x3086, 0x4BA4},
    {0x3086, 0xC257},
    {0x3086,   0x43},
    {0x3086,  0xC2D},
    {0x3086, 0x6343},
    {0x3086, 0x5FC4},
    {0x3086, 0xC52A},
    {0x3086, 0xFCC8},
    {0x3086, 0xC553},
    {0x3086,  0x553},
    {0x3086,  0x78E},
    {0x3086,  0x34D},
    {0x3086, 0x2BF8},
    {0x3086, 0x2924},
    {0x3086, 0x29AE},
    {0x3086, 0x3F07},
    {0x3086, 0x45DC},
    {0x3086, 0x5C19},
    {0x3086, 0x494C},
    {0x3086,  0x95C},
    {0x3086, 0x1B2A},
    {0x3086, 0xFE8E},
    {0x3086,  0xF3F},
    {0x3086,  0x92B},
    {0x3086, 0xB82B},
    {0x3086, 0x984E},
    {0x3086, 0x45CC},
    {0x3086, 0x1645},
    {0x3086, 0x8C3F},
    {0x3086,   0x8E},
    {0x3086,  0xD29},
    {0x3086, 0xBA60},
    {0x3086, 0x3F00},
    {0x3086, 0x5C0B},
    {0x3086, 0x2AFA},
    {0x3086, 0x3F32},
    {0x3086, 0x5C03},
    {0x3086, 0x3F23},
    {0x3086, 0x3E10},
    {0x3086, 0x5F2A},
    {0x3086, 0xF22B},
    {0x3086, 0x902B},
    {0x3086, 0x80C7},
    {0x3086, 0x3F08},
    {0x3086, 0x6029},
    {0x3086, 0xAAD0},
    {0x3086, 0x2983},
    {0x3086, 0x5F4D},
    {0x3086, 0x1945},
    {0x3086, 0xAC2A},
    {0x3086, 0xFAC6},
    {0x3086, 0x3E05},
    {0x3086, 0x2AFB},
    {0x3086, 0x5D88},
    {0x3086, 0x103F},
    {0x3086, 0x2F45},
    {0x3086, 0x8829},
    {0x3086, 0x9221},
    {0x3086, 0x2B04},
    {0x3086, 0x853F},
    {0x3086,  0x42A},
    {0x3086, 0xFA8E},
    {0x3086,  0x98D},
    {0x3086, 0x4D4E},
    {0x3086, 0x482B},
    {0x3086, 0x804C},
    {0x3086,  0xC60},
    {0x3086, 0x3F30},
    {0x3086, 0xC7C3},
    {0x3086, 0x3F0B},
    {0x3086, 0x2AF2},
    {0x3086, 0x3F08},
    {0x3086, 0x3F08},
    {0x3086, 0x2982},
    {0x3086, 0xC6D0},
    {0x3086, 0xD245},
    {0x3086, 0x58D3},
    {0x3086, 0x5F4D},
    {0x3086, 0x192A},
    {0x3086, 0xFAC9},
    {0x3086, 0x2A98},
    {0x3086, 0x8E00},
    {0x3086, 0x1244},
    {0x3086, 0x4A04},
    {0x3086, 0x4316},
    {0x3086,  0x543},
    {0x3086, 0x1658},
    {0x3086, 0x4316},
    {0x3086, 0x5A43},
    {0x3086, 0x1606},
    {0x3086, 0x4316},
    {0x3086,  0x743},
    {0x3086, 0x168E},
    {0x3086,  0x345},
    {0x3086, 0x783F},
    {0x3086,  0x52A},
    {0x3086, 0x9D5D},
    {0x3086, 0x8811},
    {0x3086, 0x2B04},
    {0x3086, 0x530D},
    {0x3086, 0x8629},
    {0x3086, 0x448D},
    {0x3086, 0x4D2B},
    {0x3086, 0x643F},
    {0x3086, 0x2045},
    {0x3086, 0x5825},
    {0x3086, 0x3F05},
    {0x3086, 0x8E00},
    {0x3086, 0x2A98},
    {0x3086, 0x1244},
    {0x3086, 0x4BC1},
    {0x3086, 0xA6B9},
    {0x3086, 0x2C2C},
    {0x3086, 0x2C2C},
    {0x3ED8, 0xF9EE},
};


MX00_AR0239_REG_s MX00_AR0239_CFPNImpTable[MX00_AR0239_NUM_CFPN_Impr_REG] = {
    {0x3EFE, 0x96F2},
    {0x3F04, 0x0F18},
    {0x3F02, 0x2F1C},
    {0x3ED0, 0xC400},
    {0x3ED4, 0x03CF},
    {0x3EF4, 0x1425},
    {0x3ECE, 0x40F3},
    {0x3EE6, 0xF723},
    {0x3EF0, 0x1828},
    {0x3EE8, 0xE4FF},
    {0x3EFA, 0x638A},
    {0x3EF2, 0x9464},
    {0x3EF2, 0x9464},
    {0x3EF2, 0x9464},
    {0x3EF4, 0x1825},
    {0x3EF4, 0x5825},
    {0x3EF4, 0x5825},
    {0x3EF6, 0x5253},
    {0x3EF6, 0x5253},
    {0x3EF6, 0x5253},
    {0x3EF8, 0x54F4},
    {0x3EF8, 0x54F4},
    {0x3EF8, 0x54F4},

};

MX00_AR0239_REG_s MX00_AR0239_HDRSeqTable[MX00_AR0239_NUM_HDR_Seq_REG] = {
    {0x3088, 0x8130},
    {0x3086, 0x4558},
    {0x3086, 0x729B},
    {0x3086, 0x4A31},
    {0x3086, 0x43C5},
    {0x3086, 0x2A08},
    {0x3086, 0x42CE},
    {0x3086, 0xCB8E},
    {0x3086,  0x345},
    {0x3086, 0x782A},
    {0x3086, 0x1C7B},
    {0x3086, 0xCA3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xFF3D},
    {0x3086, 0xEA2A},
    {0x3086,  0xC3D},
    {0x3086, 0x102A},
    {0x3086,  0xD2A},
    {0x3086, 0x1D35},
    {0x3086, 0x2A0D},
    {0x3086, 0x3D10},
    {0x3086, 0x4558},
    {0x3086, 0x2A0C},
    {0x3086, 0x8E03},
    {0x3086, 0x2A1C},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DFF},
    {0x3086, 0x3DEA},
    {0x3086, 0x2A0C},
    {0x3086, 0x62CC},
    {0x3086, 0xCF2A},
    {0x3086, 0x288E},
    {0x3086,   0x36},
    {0x3086, 0x2A08},
    {0x3086, 0x3D64},
    {0x3086, 0x7ACD},
    {0x3086, 0x3D04},
    {0x3086, 0x442A},
    {0x3086, 0x982C},
    {0x3086, 0x4B8F},
    {0x3086, 0xC257},
    {0x3086,   0x43},
    {0x3086,  0xC2D},
    {0x3086, 0x6343},
    {0x3086, 0x5FC4},
    {0x3086, 0xC52A},
    {0x3086, 0xFCC8},
    {0x3086, 0xC553},
    {0x3086,  0x553},
    {0x3086,  0x78E},
    {0x3086,  0x34D},
    {0x3086, 0x2BF8},
    {0x3086, 0x2924},
    {0x3086, 0x29AE},
    {0x3086, 0x45DC},
    {0x3086, 0x3F04},
    {0x3086, 0x5C19},
    {0x3086, 0x495C},
    {0x3086, 0x1B2A},
    {0x3086, 0xFE8E},
    {0x3086,  0xF3F},
    {0x3086,  0x32B},
    {0x3086, 0xB82B},
    {0x3086, 0x984E},
    {0x3086, 0x29BA},
    {0x3086, 0x5397},
    {0x3086, 0x604C},
    {0x3086,  0x945},
    {0x3086, 0xCC45},
    {0x3086, 0x8C5C},
    {0x3086,  0xB3F},
    {0x3086,  0x88E},
    {0x3086,  0xD2A},
    {0x3086, 0xFA3F},
    {0x3086, 0x105C},
    {0x3086,  0x33F},
    {0x3086,  0x23E},
    {0x3086, 0x10C3},
    {0x3086, 0x5F2A},
    {0x3086, 0xF2C7},
    {0x3086, 0x2B90},
    {0x3086, 0x2B80},
    {0x3086, 0x5307},
    {0x3086, 0x603F},
    {0x3086,  0x629},
    {0x3086, 0xAAD0},
    {0x3086, 0x2983},
    {0x3086, 0x5F4D},
    {0x3086, 0x1945},
    {0x3086, 0xAC2A},
    {0x3086, 0xFAC6},
    {0x3086, 0x3E05},
    {0x3086, 0x2AFB},
    {0x3086, 0x5D16},
    {0x3086, 0x8811},
    {0x3086, 0x3E87},
    {0x3086, 0x2992},
    {0x3086, 0x2145},
    {0x3086, 0x882B},
    {0x3086,  0x485},
    {0x3086, 0x3F04},
    {0x3086, 0x2AFA},
    {0x3086, 0x8E09},
    {0x3086, 0x3F04},
    {0x3086, 0x4D4E},
    {0x3086, 0x1648},
    {0x3086, 0x2B80},
    {0x3086, 0x4C0C},
    {0x3086, 0x603F},
    {0x3086,  0x7C3},
    {0x3086, 0x3F1A},
    {0x3086, 0x2AF2},
    {0x3086, 0xC73F},
    {0x3086,  0xD29},
    {0x3086, 0x82D0},
    {0x3086, 0x4558},
    {0x3086, 0x5F4D},
    {0x3086, 0x19C6},
    {0x3086, 0xD32A},
    {0x3086, 0xFAD2},
    {0x3086, 0xC92A},
    {0x3086, 0x988E},
    {0x3086,   0x12},
    {0x3086, 0x4404},
    {0x3086, 0x434A},
    {0x3086,  0x543},
    {0x3086, 0x1658},
    {0x3086, 0x4316},
    {0x3086, 0x5A43},
    {0x3086, 0x1606},
    {0x3086, 0x4316},
    {0x3086,  0x743},
    {0x3086, 0x1645},
    {0x3086, 0x783F},
    {0x3086,  0x48E},
    {0x3086,  0x32A},
    {0x3086, 0x9D5D},
    {0x3086, 0x1688},
    {0x3086, 0x112B},
    {0x3086,  0x453},
    {0x3086,  0xD86},
    {0x3086, 0x2944},
    {0x3086, 0x3F05},
    {0x3086, 0x2545},
    {0x3086, 0x584D},
    {0x3086, 0x2B64},
    {0x3086, 0x2A98},
    {0x3086, 0x8E00},
    {0x3086, 0x1244},
    {0x3086, 0x4BC1},
    {0x3086, 0x9BB9},
    {0x3086, 0x2C2C},
    {0x3086, 0x2C2C},
    {0x3ED8, 0xF9EE},
};

MX00_AR0239_REG_s MX00_AR0239_RecomSettingESTable[MX00_AR0239_NUM_Recom_Set_REG] = {
    {0x3EFC, 0xE0FF},
    {0x30AE, 0x0003},
    {0x3092, 0x086F},
    {0x30BA, 0x762C},
    {0x317E, 0x8F70},
    {0x3180, 0x8040},
    {0x3ED2, 0x0099},
    {0x3ED6, 0x14A1},
    {0x3ED8, 0xF9EE},
    {0x3EDA, 0x6E40},
    {0x3EDC, 0x08C5},
    {0x3EDE, 0x11A1},
    {0x3EE0, 0x118A},
    {0x3EE2, 0x31C6},
    {0x3EE4, 0x0D77},
    {0x3EFE, 0x96F2},
    {0x3F04, 0x0C18},
    {0x3ED0, 0xC400},
    {0x3ED4, 0x03CF},
    {0x3ECE, 0x40F3},
    {0x3F02, 0x2E1C},
    {0x3EF4, 0x5805},
    {0x3EF2, 0x9464},
    {0x3EF2, 0x9464},
    {0x3EF2, 0x9464},
    {0x3EF4, 0x5805},
    {0x3EF4, 0x5805},
    {0x3EF4, 0x5805},
    {0x3EF6, 0x5253},
    {0x3EF6, 0x5253},
    {0x3EF6, 0x5253},
    {0x3EF8, 0x54F4},
    {0x3EF8, 0x54F4},
    {0x3EF8, 0x54F4},
    {0x3F4C, 0x52BC},
    {0x3F4C, 0x52BC},
    {0x3F4E, 0x3E3C},
    {0x3F4E, 0x3E3C},
    {0x3F50, 0x080B},
    {0x3F50, 0x380B},
    {0x31E0, 0x000F},
    {0x3324, 0x001C},
    {0x30B4, 0x01C1},

};

MX00_AR0239_MODE_REG_s MX00_AR0239_ModeRegTable[MX00_AR0239_NUM_READOUT_MODE_REG] = {
    {0x30A2,  {0x0001, 0x0001,   0x0001,   0x0001}}, /* X_ODD_INC*/
    {0x30A6,  {0x0001, 0x0001,   0x0001,   0x0001}}, /* Y_ODD_INC*/
    {0x3040,  {0x0000, 0x0000,   0x0000,   0x0000}}, /* READ_MODE*/
    {0x3004,  {    12,     12,        12,          12}}, /* x_add_start*/
    {0x3008,  {  1931,   1931,     1931,     1931}}, /* x_add_end*/
    {0x3002,  {    54,     54,       54,       54}}, /* y_add_start*/
    {0x3006,  {  1133,   1133,     1133,     1133}}, /* y_add_end*/
    {0x300C,  {  2000,   1116,       968,         968}}, /* LLPCK*/
    {0x300A,  {  1198,   2148,     1100,     2200}}, /* FLL*/
    {0x3012,  {  1099,   1924,     1098,     1098}}, /* coarse_integration_time*/
    {0x3042,  {0x0000, 0x0000,   0x0000,   0x0000}}, /* EXTRA_DELAY*/
};

MX00_AR0239_AGC_REG_s MX00_AR0239_AgcRegTable[MX00_AR0239_NUM_AGC_STEP] = {
    { 1.000f, 0,  0, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.032f, 0,  1, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.067f, 0,  2, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.103f, 0,  3, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.143f, 0,  4, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.185f, 0,  5, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.231f, 0,  6, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.280f, 0,  7, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.333f, 0,  8, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.391f, 0,  9, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.455f, 0, 10, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.524f, 0, 11, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.600f, 0, 12, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.684f, 0, 13, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.778f, 0, 14, MX00_AR0239_GAIN_CONVERT_LOW},
    { 1.882f, 0, 15, MX00_AR0239_GAIN_CONVERT_LOW},
    { 2.000f, 1,  0, MX00_AR0239_GAIN_CONVERT_LOW},
    { 2.133f, 1,  2, MX00_AR0239_GAIN_CONVERT_LOW},
    { 2.286f, 1,  4, MX00_AR0239_GAIN_CONVERT_LOW},
    { 2.462f, 1,  6, MX00_AR0239_GAIN_CONVERT_LOW},
    { 2.667f, 1,  8, MX00_AR0239_GAIN_CONVERT_LOW},
    { 2.909f, 1, 10, MX00_AR0239_GAIN_CONVERT_LOW},
    { 3.000f, 0,  0, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 3.097f, 0,  1, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 3.200f, 0,  2, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 3.310f, 0,  3, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 3.429f, 0,  4, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 3.556f, 0,  5, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 3.692f, 0,  6, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 3.840f, 0,  7, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 4.000f, 0,  8, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 4.174f, 0,  9, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 4.364f, 0, 10, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 4.571f, 0, 11, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 4.800f, 0, 12, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 5.053f, 0, 13, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 5.333f, 0, 14, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 5.647f, 0, 15, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 6.000f, 1,  0, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 6.400f, 1,  2, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 6.857f, 1,  4, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 7.385f, 1,  6, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 8.000f, 1,  8, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 8.727f, 1, 10, MX00_AR0239_GAIN_CONVERT_HIGH},
    { 9.600f, 1, 12, MX00_AR0239_GAIN_CONVERT_HIGH},
    {10.667f, 1, 14, MX00_AR0239_GAIN_CONVERT_HIGH},
    {12.000f, 2,  0, MX00_AR0239_GAIN_CONVERT_HIGH},
    {12.387f, 2,  1, MX00_AR0239_GAIN_CONVERT_HIGH},
    {12.800f, 2,  2, MX00_AR0239_GAIN_CONVERT_HIGH},
    {13.241f, 2,  3, MX00_AR0239_GAIN_CONVERT_HIGH},
    {13.714f, 2,  4, MX00_AR0239_GAIN_CONVERT_HIGH},
    {14.222f, 2,  5, MX00_AR0239_GAIN_CONVERT_HIGH},
    {14.769f, 2,  6, MX00_AR0239_GAIN_CONVERT_HIGH},
    {15.360f, 2,  7, MX00_AR0239_GAIN_CONVERT_HIGH},
    {16.000f, 2,  8, MX00_AR0239_GAIN_CONVERT_HIGH},
    {16.696f, 2,  9, MX00_AR0239_GAIN_CONVERT_HIGH},
    {17.455f, 2, 10, MX00_AR0239_GAIN_CONVERT_HIGH},
    {18.286f, 2, 11, MX00_AR0239_GAIN_CONVERT_HIGH},
    {19.200f, 2, 12, MX00_AR0239_GAIN_CONVERT_HIGH},
    {20.211f, 2, 13, MX00_AR0239_GAIN_CONVERT_HIGH},
    {21.333f, 2, 14, MX00_AR0239_GAIN_CONVERT_HIGH},
    {22.588f, 2, 15, MX00_AR0239_GAIN_CONVERT_HIGH},
    {24.000f, 3,  0, MX00_AR0239_GAIN_CONVERT_HIGH},
    {24.774f, 3,  1, MX00_AR0239_GAIN_CONVERT_HIGH},
    {25.600f, 3,  2, MX00_AR0239_GAIN_CONVERT_HIGH},
    {26.483f, 3,  3, MX00_AR0239_GAIN_CONVERT_HIGH},
    {27.429f, 3,  4, MX00_AR0239_GAIN_CONVERT_HIGH},
    {28.444f, 3,  5, MX00_AR0239_GAIN_CONVERT_HIGH},
    {29.538f, 3,  6, MX00_AR0239_GAIN_CONVERT_HIGH},
    {30.720f, 3,  7, MX00_AR0239_GAIN_CONVERT_HIGH},
    {32.000f, 3,  8, MX00_AR0239_GAIN_CONVERT_HIGH},
    {33.391f, 3,  9, MX00_AR0239_GAIN_CONVERT_HIGH},
    {34.909f, 3, 10, MX00_AR0239_GAIN_CONVERT_HIGH},
    {36.571f, 3, 11, MX00_AR0239_GAIN_CONVERT_HIGH},
    {38.400f, 3, 12, MX00_AR0239_GAIN_CONVERT_HIGH},
    {40.421f, 3, 13, MX00_AR0239_GAIN_CONVERT_HIGH},
    {42.667f, 3, 14, MX00_AR0239_GAIN_CONVERT_HIGH},
    {45.176f, 3, 15, MX00_AR0239_GAIN_CONVERT_HIGH},
    {48.000f, 4,  0, MX00_AR0239_GAIN_CONVERT_HIGH},
    {51.200f, 4,  2, MX00_AR0239_GAIN_CONVERT_HIGH},
    {54.857f, 4,  4, MX00_AR0239_GAIN_CONVERT_HIGH},
    {59.077f, 4,  6, MX00_AR0239_GAIN_CONVERT_HIGH},
    {64.000f, 4,  8, MX00_AR0239_GAIN_CONVERT_HIGH},
    {69.818f, 4, 10, MX00_AR0239_GAIN_CONVERT_HIGH},
    {76.800f, 4, 12, MX00_AR0239_GAIN_CONVERT_HIGH},
    {85.333f, 4, 14, MX00_AR0239_GAIN_CONVERT_HIGH},
};


const MX00_AR0239_SENSOR_INFO_s MX00_AR0239_SensorInfo[MX00_AR0239_NUM_MODE] = {
    /* MX00_AR0239_1920_1080_30P */ {27000000, 216000000, 4, 12, 2000, 1198, 1316, 3123, { 0,  0, 1920, 1080}, {0}},
    /* MX00_AR0239_1920_1080_30P_HDR */ {26941198, 862118336, 4, 12,  1116*2, 2148*2, 1920, 4256, { 0,  0, 1920, 4256}, {0}},
    /* MX00_AR0239_1920_1080_2LANE_30P */ {27000000, 192000000, 2, 12, 968, 1100, 1920, 1080, { 0,  0, 1920, 1080}, {0}},
    /* MX00_AR0239_1920_1080_2LANE_15P */ {27000000, 192000000, 2, 12, 968, 2200, 1920, 1080, { 0,  0, 1920, 1080}, {0}},
};


const AMBA_SENSOR_OUTPUT_INFO_s MX00_AR0239_OutputInfo[MX00_AR0239_NUM_MODE] = {
    /* MX00_AR0239_1920_1080_A30P */ {600000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_GRIG, 1920, 1080, { 0,   0, 1920, 1080}, {0}},
    /* MX00_AR0239_1920_1080_30P_HDR */  {900000000,  4, 12, AMBA_SENSOR_BAYER_PATTERN_GRIG, 1920, 4256, { 0,   0, 1920, 4256}, {0}},
    /* MX00_AR0239_1920_1080_2LANE_A30P */{400000000,  2, 12, AMBA_SENSOR_BAYER_PATTERN_GRIG, 1920, 1080, { 0,   0, 1920, 1080}, {0}},
    /* MX00_AR0239_1920_1080_2LANE_A15P */{400000000,  2, 12, AMBA_SENSOR_BAYER_PATTERN_GRIG, 1920, 1080, { 0,   0, 1920, 1080}, {0}},


};

const AMBA_SENSOR_INPUT_INFO_s MX00_AR0239_InputInfo[MX00_AR0239_NUM_MODE] = {
    /* MX00_AR0239_1920_1080_30AP */ {{ 0,  0, 1920,  1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX00_AR0239_1920_1080_30P_HDR */ {{ 12,  54, 1920,  1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX00_AR0239_1920_1080_2LANE_A30P */ {{ 0,  0, 1920,  1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},
    /* MX00_AR0239_1920_1080_2LANE_A15P */ {{ 0,  0, 1920,  1080}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, { AMBA_SENSOR_SUBSAMPLE_NORMAL, 1, 1}, 1},


};

MX00_AR0239_MODE_INFO_s MX00_AR0239_ModeInfoList[MX00_AR0239_NUM_MODE] = {
    /* MX00_AR0239_1920_1080_A30P */ {MX00_AR0239_1920_1080_A30P, {{ .Interlace = 0, .TimeScale =  30, .NumUnitsInTick = 1}}, 0.0f, 0.0f},
    /* MX00_AR0239_1920_1080_30P_HDR */ {MX00_AR0239_1920_1080_30P_HDR, {{ .Interlace = 0, .TimeScale =  30000, .NumUnitsInTick = 1001}}, 0.0f, 0.0f},
    /* MX00_AR0239_1920_1080_2LANE_A30P */ {MX00_AR0239_1920_1080_2LANE_A30P, {{ .Interlace = 0, .TimeScale =  30, .NumUnitsInTick = 1}}, 0.0f, 0.0f},
    /* MX00_AR0239_1920_1080_2LANE_A15P */ {MX00_AR0239_1920_1080_2LANE_A15P, {{ .Interlace = 0, .TimeScale =  15, .NumUnitsInTick = 1}}, 0.0f, 0.0f},

};

const AMBA_SENSOR_HDR_INFO_s MX00_AR0239_HdrInfo[MX00_AR0239_NUM_MODE] = {
    [MX00_AR0239_1920_1080_A30P] = {0},
    [MX00_AR0239_1920_1080_2LANE_A30P] = {0},
    [MX00_AR0239_1920_1080_2LANE_A15P] = {0},
    [MX00_AR0239_1920_1080_30P_HDR] =  {
        .HdrType = AMBA_SENSOR_HDR_TYPE_MULTI_SLICE,
        .ActiveChannels = 2,
        .ChannelInfo = {
            [0] = {
                .EffectiveArea = {0,  0, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 1804, //28ms
                .MinExposureLine = 1,
            },
            [1] = {
                .EffectiveArea = {1920, 1048, 1920, 1080},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 2, 0, 0},
                .OutputFormatCtrlInfo = {AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK, 1, 0, 0},
                .MaxExposureLine = 322, //5ms, T1 + T2 <= FLL -22
                .MinExposureLine = 1,
            },
            [2] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0U,
                .MinExposureLine = 0U,
            },
            [3] = {
                .EffectiveArea = {0},
                .OpticalBlackPixels = {0},
                .ShutterSpeedCtrlInfo = {0},
                .OutputFormatCtrlInfo = {0},
                .MaxExposureLine = 0U,
                .MinExposureLine = 0U,
            },
        },
    },
};
