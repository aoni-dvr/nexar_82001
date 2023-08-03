/**
 *  @file AmbaSensor_IMX334.h
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
 *  @details Control APIs of SONY IMX334 CMOS sensor with MIPI interface
 *
 */

#ifndef AMBA_SENSOR_IMX334_H
#define AMBA_SENSOR_IMX334_H

#define IMX334_SENSOR_I2C_SLAVE_ADDR    0x34
#define IMX334_SENSOR_I2C_MAX_SIZE      64U

#define IMX334_MAX_AGAIN_CTRL           100U   /* 100 * 0.3 = 30 dB */
#define IMX334_MAX_DGAIN_CTRL           140U   /* 140 * 0.3 = 42 dB */
#define IMX334_MAX_TOTAL_GAIN_CTRL      (IMX334_MAX_AGAIN_CTRL + IMX334_MAX_DGAIN_CTRL)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX334_NUM_INIT_REG                 69U
#define IMX334_NUM_READOUT_MODE_REG         59U
#define IMX334_NUM_READOUT_DIRECTION_REG    26U

#define IMX334_STANDBY                  0x3000U
#define IMX334_REGHOLD                  0x3001U
#define IMX334_MASTERSTOP               0x3002U
#define IMX334_RESTART                  0x3004U

#define IMX334_BCWAIT_TIME              0x300CU
#define IMX334_CPWAIT_TIME              0x300DU
#define IMX334_WINMODE                  0x3018U
#define IMX334_HTRIMMING_START_LSB      0x302CU
#define IMX334_HTRIMMING_START_MSB      0x302DU
#define IMX334_HNUM_LSB                 0x302EU
#define IMX334_HNUM_MSB                 0x302FU
#define IMX334_VMAX_LSB                 0x3030U
#define IMX334_VMAX_MSB                 0x3031U
#define IMX334_VMAX_HSB                 0x3032U
#define IMX334_HMAX_LSB                 0x3034U
#define IMX334_HMAX_MSB                 0x3035U
#define IMX334_WDMODE                   0x3048U
#define IMX334_WDSEL                    0x3049U
#define IMX334_WD_SET1                  0x304AU
#define IMX334_WD_SET2                  0x304BU
#define IMX334_OPB_SIZE_V               0x304CU
#define IMX334_HREVERSE                 0x304EU
#define IMX334_VREVERSE                 0x304FU
#define IMX334_ADBIT                    0x3050U
#define IMX334_SHR0_LSB                 0x3058U
#define IMX334_SHR0_MSB                 0x3059U
#define IMX334_SHR0_HSB                 0x305AU
#define IMX334_SHR1_LSB                 0x305CU
#define IMX334_SHR1_MSB                 0x305DU
#define IMX334_SHR1_HSB                 0x305EU
#define IMX334_SHR2_LSB                 0x3060U
#define IMX334_SHR2_MSB                 0x3061U
#define IMX334_SHR2_HSB                 0x3062U
#define IMX334_RHS1_LSB                 0x3068U
#define IMX334_RHS1_MSB                 0x3069U
#define IMX334_RHS1_HSB                 0x306AU
#define IMX334_RHS2_LSB                 0x306CU
#define IMX334_RHS2_MSB                 0x306DU
#define IMX334_RHS2_HSB                 0x306EU
#define IMX334_AREA3_ST_ADR_1_LSB       0x3074U
#define IMX334_AREA3_ST_ADR_1_MSB       0x3075U
#define IMX334_AREA3_WIDTH_1_LSB        0x3076U
#define IMX334_AREA3_WIDTH_1_MSB        0x3077U
#define IMX334_AREA3_ST_ADR_2_LSB       0x308EU
#define IMX334_AREA3_ST_ADR_2_MSB       0x308FU
#define IMX334_AREA3_WIDTH_2_LSB        0x3090U
#define IMX334_AREA3_WIDTH_2_MSB        0x3091U

#define IMX334_UNREAD_PARAM5_LSB        0x30B6U
#define IMX334_UNREAD_PARAM5_MSB        0x30B7U

#define IMX334_BLACK_OFSET_ADR_LSB      0x30C6U
#define IMX334_BLACK_OFSET_ADR_MSB      0x30C7U

#define IMX334_UNRD_LINE_MAX_LSB        0x30CEU
#define IMX334_UNRD_LINE_MAX_MSB        0x30CFU
#define IMX334_UNREAD_ED_ADR_LSB        0x30D8U
#define IMX334_UNREAD_ED_ADR_MSB        0x30D9U

#define IMX334_GAIN_LSB                 0x30E8U
#define IMX334_GAIN_MSB                 0x30E9U
#define IMX334_GAIN1_LSB                0x30EAU
#define IMX334_GAIN1_MSB                0x30EBU
#define IMX334_GAIN2_LSB                0x30ECU
#define IMX334_GAIN2_MSB                0x30EDU

#define IMX334_UNREAD_PARAM6_LSB        0x3116U
#define IMX334_UNREAD_PARAM6_MSB        0x3117U
#define IMX334_INCKSEL1_LSB             0x314CU
#define IMX334_INCKSEL1_MSB             0x314DU
#define IMX334_INCKSEL2                 0x315AU
#define IMX334_INCKSEL3                 0x3168U
#define IMX334_INCKSEL4                 0x316AU
#define IMX334_HVADD                    0x3199U
#define IMX334_MDBIT                    0x319DU
#define IMX334_SYSMODE                  0x319EU
#define IMX334_VCEN                     0x319FU
#define IMX334_XVSMSKCNT_INT            0x31D7U
#define IMX334_VALID_EXPAND             0x31DDU

#define IMX334_FGAINEN                  0x3200U

#define IMX334_TCYCLE                   0x3300U
#define IMX334_BLKLEVEL_LSB             0x3302U
#define IMX334_BLKLEVEL_MSB             0x3303U
#define IMX334_Y_OUT_SIZE_LSB           0x3308U
#define IMX334_Y_OUT_SIZE_MSB           0x3309U

#define IMX334_ADBIT1_LSB               0x341CU
#define IMX334_ADBIT1_MSB               0x341DU

#define IMX334_LANEMODE                 0x3A01U
#define IMX334_TCLKPOST_LSB             0x3A18U
#define IMX334_TCLKPOST_MSB             0x3A19U
#define IMX334_TCLKPRE_LSB              0x3A1AU
#define IMX334_TCLKPRE_MSB              0x3A1BU
#define IMX334_TCLKTRAIL_LSB            0x3A1CU
#define IMX334_TCLKTRAIL_MSB            0x3A1DU
#define IMX334_TCLKZERO_LSB             0x3A1EU
#define IMX334_TCLKZERO_MSB             0x3A1FU
#define IMX334_THSPRE_LSB               0x3A20U
#define IMX334_THSPRE_MSB               0x3A21U
#define IMX334_THSZERO_LSB              0x3A22U
#define IMX334_THSZERO_MSB              0x3A23U
#define IMX334_THSTRAIL_LSB             0x3A24U
#define IMX334_THSTRAIL_MSB             0x3A25U
#define IMX334_THSEXIT_LSB              0x3A26U
#define IMX334_THSEXIT_MSB              0x3A27U
#define IMX334_TLPX_LSB                 0x3A28U
#define IMX334_TLPX_MSB                 0x3A29U

#define IMX334_3840_2160_30P_0     0U  /* 4-lane,  891Mbps, ADC: 12-bit, data width: 12-bit */
#define IMX334_3840_2160_30P_1     1U  /* 8-lane,  891Mbps, ADC: 12-bit, data width: 12-bit */
#define IMX334_3840_2160_60P_0     2U  /* 4-lane, 1782Mbps, ADC: 12-bit, data width: 12-bit */
#define IMX334_3840_2160_60P_1     3U  /* 8-lane,  891Mbps, ADC: 12-bit, data width: 12-bit */
#define IMX334_3840_2160_60P_2     4U  /* 4-lane, 1782Mbps, ADC: 10-bit, data width: 10-bit */
#define IMX334_3840_2160_30P_HDR_0 5U  /* 8-lane, 1188Mbps, ADC: 12-bit, data width: 12-bit, DOL 2 frame, VC mode (within FSC) */
#define IMX334_3840_2160_30P_HDR_1 6U  /* 4-lane, 1782Mbps, ADC: 12-bit, data width: 12-bit, DOL 2 frame, VC mode (beyond FSC) */
#define IMX334_3840_2160_30P_HDR_2 7U  /* 8-lane,  891Mbps, ADC: 12-bit, data width: 12-bit, DOL 2 frame, VC mode (beyond FSC) */
#define IMX334_3840_2160_30P_HDR_3 8U  /* 4-lane, 1782Mbps, ADC: 10-bit, data width: 10-bit, DOL 2 frame, VC mode (within FSC) */
#define IMX334_3840_2160_30P_HDR_4 9U  /* 4-lane, 1782Mbps, ADC: 10-bit, data width: 10-bit, DOL 2 frame, LI mode (within FSC) */
#define IMX334_NUM_MODE            10U

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} IMX334_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[IMX334_NUM_MODE];
} IMX334_MODE_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  RefClkMul;                      /* RefClk = InputClk * RefClkMul */
    UINT32  NumTickPerXhs;                  /* XHS period (in RefClk cycles) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                     /* vertical operating period (in number of XVS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX334_FRAME_TIMING_s;

typedef struct {
    IMX334_FRAME_TIMING_s   FrameTiming;
    DOUBLE                  LinePeriod;     /* 1H period. Calculated in IMX334_Init */
} IMX334_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentGainCtrl[3];
    UINT32                      CurrentShrCtrl[3];
} IMX334_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX334.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX334DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX334InputInfo[IMX334_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX334OutputInfo[IMX334_NUM_MODE];
extern IMX334_MODE_INFO_s IMX334ModeInfoList[IMX334_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX334HdrInfo[IMX334_NUM_MODE];

extern IMX334_REG_s IMX334InitRegTable[IMX334_NUM_INIT_REG];
extern IMX334_MODE_REG_s IMX334ModeRegTable[IMX334_NUM_READOUT_MODE_REG];
extern IMX334_REG_s IMX334NormalVRegTable[2][IMX334_NUM_READOUT_DIRECTION_REG];
extern IMX334_REG_s IMX334InvertedVRegTable[2][IMX334_NUM_READOUT_DIRECTION_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX334Obj;

#endif /* AMBA_SENSOR_IMX334_H */
