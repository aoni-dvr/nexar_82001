/**
 *  @file AmbaSensor_IMX586.h
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
 *  @details Control APIs of SONY IMX586 CMOS sensor with MIPI interface
 *
 */

#ifndef AMBA_SENSOR_IMX586_H
#define AMBA_SENSOR_IMX586_H

#define IMX586_CPHY      /* comment it when using DPHY */

#define IMX586_I2C_SLAVE_ADDRESS    0x34 /* 0x20, Master, SLASEL = High Level. 0x34, Slave, SLASEL = Low Level or NC */
#define TCA9548A_I2C_SLAVE_ADDRESS  0xE0

#define IMX586_NUM_GOLBAL_REG       2466U
#define IMX586_NUM_IMG_QUALITY_REG  12U
#ifdef IMX586_CPHY
#define IMX586_NUM_READOUT_MODE_REG 142U
#else
#define IMX586_NUM_READOUT_MODE_REG 129U
#endif


/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define IMX586_8000_6000_19P    0U  /* with CPHY, the frame rate is 30 fps */
#define IMX586_8000_6000_15P    1U  /* for CPHY only, test half data rate */
#define IMX586_7680_4320_30P    2U  /* for CPHY only */
#define IMX586_7680_4320_A30P   3U  /* for CPHY only */
#define IMX586_3840_2160_120P   4U  /* for CPHY only */
#define IMX586_NUM_MODE         5U


/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/

/* SW-Standby on/off */
#define IMX586_MODE_SEL_REG                     0x0100
/* Image orientation for Vertical direction */
#define IMX586_IMG_ORIENTATION_V_REG            0x0101
/* Shutter Control */
#define IMX586_COARSE_INTEG_TIME_MSB_REG        0x0202
#define IMX586_COARSE_INTEG_TIME_LSB_REG        0x0203
#define IMX586_MID_COARSE_INTEG_TIME_MSB_REG    0x3FE0
#define IMX586_MID_COARSE_INTEG_TIME_LSB_REG    0x3FE1
#define IMX586_ST_COARSE_INTEG_TIME_MSB_REG     0x0224
#define IMX586_ST_COARSE_INTEG_TIME_LSB_REG     0x0225
/* Gain Control */
#define IMX586_ANA_GAIN_GLOBAL_MSB_REG          0x0204
#define IMX586_ANA_GAIN_GLOBAL_LSB_REG          0x0205
#define IMX586_DIG_GAIN_GLOBAL_MSB_REG          0x020E
#define IMX586_DIG_GAIN_GLOBAL_LSB_REG          0x020F
#define IMX586_MID_ANA_GAIN_GLOBAL_MSB_REG      0x3FE2
#define IMX586_MID_ANA_GAIN_GLOBAL_LSB_REG      0x3FE3
#define IMX586_MID_DIG_GAIN_GLOBAL_MSB_REG      0x3FE4
#define IMX586_MID_DIG_GAIN_GLOBAL_LSB_REG      0x3FE5
#define IMX586_ST_ANA_GAIN_GLOBAL_MSB_REG       0x0216
#define IMX586_ST_ANA_GAIN_GLOBAL_LSB_REG       0x0217
#define IMX586_ST_DIG_GAIN_GLOBAL_MSB_REG       0x0218
#define IMX586_ST_DIG_GAIN_GLOBAL_LSB_REG       0x0219

/*WB Gain Control*/
#define IMX586_ABS_GAIN_GR_MSB_REG              0x0B8E
#define IMX586_ABS_GAIN_GR_LSB_REG              0x0B8F
#define IMX586_ABS_GAIN_R_MSB_REG               0x0B90
#define IMX586_ABS_GAIN_R_LSB_REG               0x0B91
#define IMX586_ABS_GAIN_B_MSB_REG               0x0B92
#define IMX586_ABS_GAIN_B_LSB_REG               0x0B93
#define IMX586_ABS_GAIN_GB_MSB_REG              0x0B94
#define IMX586_ABS_GAIN_GB_LSB_REG              0x0B95

#define IMX586_FRM_LENGTH_LINES_MSB_REG         0x0340
#define IMX586_FRM_LENGTH_LINES_LSB_REG         0x0341
#define IMX586_LINE_LENGTH_PCK_MSB_REG          0x0342
#define IMX586_LINE_LENGTH_PCK_LSB_REG          0x0343

#define IMX586_CIT_LSHIFT_REG                   0x3100

typedef struct  {
    UINT16  Addr;
    UINT8   Data;
} IMX586_REG_s;

typedef struct  {
    UINT16  Addr;
    UINT8   Data[IMX586_NUM_MODE];
} IMX586_MODE_REG_s;

typedef struct  {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  LineLengthPck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* frame rate value of this sensor mode */
} IMX586_FRAME_TIMING_s;

typedef struct  {
    IMX586_FRAME_TIMING_s   FrameTiming;
    UINT32  MinExpLineNum;
    UINT32  ExpStepVal;     /* 1 or 2. Check Step value(x) in IMX586 software reference manual doc */
    FLOAT   MaxAGainFactor;
    FLOAT   RowTime;        /* Calculated in IMX586_Init */
} IMX586_MODE_INFO_s;

typedef struct  {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgc[3];
    UINT32                      CurrentDgc[3];
    UINT32                      CurrentShrCtrl[3];
} IMX586_CTRL_s;


/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX586.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX586DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX586InputInfo[IMX586_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX586OutputInfo[IMX586_NUM_MODE];
extern IMX586_MODE_INFO_s IMX586ModeInfoList[IMX586_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX586HdrInfo[IMX586_NUM_MODE];

extern IMX586_MODE_REG_s IMX586ModeRegTable[IMX586_NUM_READOUT_MODE_REG];
extern IMX586_REG_s IMX586GlobalRegTable[IMX586_NUM_GOLBAL_REG];
extern IMX586_REG_s IMX586ImageQualityRegTable[IMX586_NUM_IMG_QUALITY_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX586Obj;

#endif /* _AMBA_SENSOR_IMX586_H_ */
