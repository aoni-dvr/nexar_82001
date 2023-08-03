/**
 *  @file AmbaSensor_IMX577.h
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
 *  @details Control APIs of SONY IMX577 CMOS sensor with LVDS interface
 *
 */

#ifndef AMBA_SENSOR_IMX577_H
#define AMBA_SENSOR_IMX577_H

#define IMX577_SENSOR_I2C_SLAVE_ADDR    0x20 /* 0x20, Master, SLASEL = High Level. 0x34, Slave, SLASEL = Low Level or NC */
#define IMX577_SENSOR_I2C_MAX_SIZE      64U

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX577_NUM_EXTCLK_REG           2U
#define IMX577_NUM_GOLBAL_REG           109U
#define IMX577_NUM_IMG_QUALITY_REG      64U
#define IMX577_NUM_READOUT_MODE_REG     130U


#define IMX577_MODE_SEL_REG                 0x0100U
#define IMX577_IMG_ORIENTATION_V_REG        0x0101U
#define IMX577_COARSE_INTEG_TIME_MSB_REG    0x0202U
#define IMX577_COARSE_INTEG_TIME_LSB_REG    0x0203U
#define IMX577_DOL_FRAME_OFFSET_2ND_MSB     0x00E6U
#define IMX577_DOL_FRAME_OFFSET_2ND_LSB     0x00E7U
#define IMX577_DOL_CIT_1ST_MSB              0x00EAU
#define IMX577_DOL_CIT_1ST_LSB              0x00EBU
#define IMX577_DOL_CIT_2ND_MSB              0x00ECU
#define IMX577_DOL_CIT_2ND_LSB              0x00EDU
#define IMX577_ANA_GAIN_GLOBAL_MSB_REG      0x0204U  /* ANA_GAIN_GLOBAL, range: 0 to 978 */
#define IMX577_ANA_GAIN_GLOBAL_LSB_REG      0x0205U
#define IMX577_USE_GLOBAL_GAIN_REG          0x3ff9U  /* Digital gain control mode select, 0: by color, 1: all color */
#define IMX577_DIG_GAIN_GR_MSB_REG          0x020eU  /* Range: 1 to 15 */
#define IMX577_DIG_GAIN_GR_LSB_REG          0x020fU  /* Range: 0 to 255 */
#define IMX577_DOL_ANA_GAIN_1ST_MSB_REG     0x00F0U  /* DOL_ANA_GAIN_1ST, range: 0 to 987 */
#define IMX577_DOL_ANA_GAIN_1ST_LSB_REG     0x00F1U
#define IMX577_DOL_ANA_GAIN_2ND_MSB_REG     0x00F2U  /* DOL_ANA_GAIN_2ND, range: 0 to 987 */
#define IMX577_DOL_ANA_GAIN_2ND_LSB_REG     0x00F3U
#define IMX577_DOL_DIG_GAIN_1ST_MSB_REG     0x00F6U  /* DOL_DIG_GAIN_1ST, range: 1 to 15 */
#define IMX577_DOL_DIG_GAIN_1ST_LSB_REG     0x00F7U  /* Range: 0 to 255 */
#define IMX577_DOL_DIG_GAIN_2ND_MSB_REG     0x00F8U  /* DOL_DIG_GAIN_2ND, range: 1 to 15 */
#define IMX577_DOL_DIG_GAIN_2ND_LSB_REG     0x00F9U  /* Range: 0 to 255 */
#define IMX577_FRM_LENGTH_LINES_MSB_REG     0x0340U
#define IMX577_FRM_LENGTH_LINES_LSB_REG     0x0341U
#define IMX577_LINE_LENGTH_PCK_MSB_REG      0x0342U
#define IMX577_LINE_LENGTH_PCK_LSB_REG      0x0343U


#define IMX577_4056_3040_60P           0U   /* RAW10, Normal, 2.1Gbps */
#define IMX577_4056_3040_30P           1U   /* RAW10, Normal, 2.1Gbps */
#define IMX577_3840_2160_60P           2U   /* RAW10, Normal, 1.6Gbps */
#define IMX577_3840_2160_30P           3U   /* RAW10, Normal, 1.6Gbps */
#define IMX577_3840_2160_30P_HDR       4U   /* DOL 2-frame 30fps */
#define IMX577_2028_1520_A30P          5U   /* RAW10, Normal, 1.5Gbps */
#define IMX577_1920_1080_A30P          6U   /* RAW10, Normal, 1.5Gbps */
#define IMX577_2028_1520_30P           5U   /* RAW10, Normal, 1.5Gbps */
#define IMX577_1920_1080_30P           6U   /* RAW10, Normal, 1.5Gbps */
#define IMX577_3840_2160_A30P          7U   /* RAW10, Normal, 1.6Gbps */
#define IMX577_3840_2160_A60P          8U   /* RAW10, Normal, 1.6Gbps */
#define IMX577_NUM_MODE                9U

typedef struct {
    UINT16  Addr;
    UINT8  Data;
} IMX577_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[IMX577_NUM_MODE];
} IMX577_MODE_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  LineLengthPck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX577_FRAME_TIMING_s;

typedef struct {
    IMX577_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in IMX577_Init */
} IMX577_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgcCtrl[2];
    UINT32                      CurrentDgcCtrl[2];
    UINT32                      CurrentShrCtrl[2];
} IMX577_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX577.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX577DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX577InputInfo[IMX577_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX577OutputInfo[IMX577_NUM_MODE];
extern IMX577_MODE_INFO_s IMX577ModeInfoList[IMX577_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX577HdrInfo[IMX577_NUM_MODE];

extern IMX577_MODE_REG_s IMX577ModeRegTable[IMX577_NUM_READOUT_MODE_REG];
extern IMX577_REG_s IMX577GlobalRegTable[IMX577_NUM_GOLBAL_REG];
extern IMX577_MODE_REG_s IMX577ExtClkRegTable[IMX577_NUM_EXTCLK_REG];
extern IMX577_REG_s IMX577ImageQualityRegTable[IMX577_NUM_IMG_QUALITY_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX577Obj;

#endif /* AMBA_SENSOR_IMX577_H */
