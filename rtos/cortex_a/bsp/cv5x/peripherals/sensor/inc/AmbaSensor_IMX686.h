/**
 *  @file AmbaSensor_IMX686.h
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

#ifndef AMBA_SENSOR_IMX686_H
#define AMBA_SENSOR_IMX686_H

#define IMX686_CPHY      /* comment it when using DPHY */

#define IMX686_I2C_SLAVE_ADDRESS    0x34 /* 0x20, Master, SLASEL = High Level. 0x34, Slave, SLASEL = Low Level or NC */
#define TCA9548A_I2C_SLAVE_ADDRESS  0xE0

#ifdef IMX686_CPHY
#define IMX686_NUM_GOLBAL_REG       139U //162 151-12= 139
#else
#define IMX686_NUM_GOLBAL_REG       158U
#endif
#define IMX686_NUM_IMG_QUALITY_REG  364U
#ifdef IMX686_CPHY
#define IMX686_NUM_READOUT_MODE_REG  114U //110 121-7 = 114U
#else
#define IMX686_NUM_READOUT_MODE_REG 125U
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/

typedef enum {
    /* SW-Standby on/off */
    IMX686_MODE_SEL_REG                     = 0x0100,
    /* Image orientation for Vertical direction */
    IMX686_IMG_ORIENTATION_V_REG            = 0x0101,
    /* Shutter Control */
    IMX686_COARSE_INTEG_TIME_MSB_REG        = 0x0202,
    IMX686_COARSE_INTEG_TIME_LSB_REG        = 0x0203,
    IMX686_MID_COARSE_INTEG_TIME_MSB_REG    = 0x3116,
    IMX686_MID_COARSE_INTEG_TIME_LSB_REG    = 0x3117,
    IMX686_ST_COARSE_INTEG_TIME_MSB_REG     = 0x0224,
    IMX686_ST_COARSE_INTEG_TIME_LSB_REG     = 0x0225,
    /* Gain Control */
    IMX686_ANA_GAIN_GLOBAL_MSB_REG          = 0x0204,
    IMX686_ANA_GAIN_GLOBAL_LSB_REG          = 0x0205,
    IMX686_DIG_GAIN_GLOBAL_MSB_REG          = 0x020E,
    IMX686_DIG_GAIN_GLOBAL_LSB_REG          = 0x020F,
    IMX686_MID_ANA_GAIN_GLOBAL_MSB_REG      = 0x3118,
    IMX686_MID_ANA_GAIN_GLOBAL_LSB_REG      = 0x3119,
    IMX686_MID_DIG_GAIN_GLOBAL_MSB_REG      = 0x311A,
    IMX686_MID_DIG_GAIN_GLOBAL_LSB_REG      = 0x311B,
    IMX686_ST_ANA_GAIN_GLOBAL_MSB_REG       = 0x0216,
    IMX686_ST_ANA_GAIN_GLOBAL_LSB_REG       = 0x0217,
    IMX686_ST_DIG_GAIN_GLOBAL_MSB_REG       = 0x0218,
    IMX686_ST_DIG_GAIN_GLOBAL_LSB_REG       = 0x0219,

    /*WB Gain Control*/
    IMX686_ABS_GAIN_GR_MSB_REG          = 0x0B8E,
    IMX686_ABS_GAIN_GR_LSB_REG          = 0x0B8F,
    IMX686_ABS_GAIN_R_MSB_REG           = 0x0B90,
    IMX686_ABS_GAIN_R_LSB_REG           = 0x0B91,
    IMX686_ABS_GAIN_B_MSB_REG           = 0x0B92,
    IMX686_ABS_GAIN_B_LSB_REG           = 0x0B93,
    IMX686_ABS_GAIN_GB_MSB_REG          = 0x0B94,
    IMX686_ABS_GAIN_GB_LSB_REG          = 0x0B95,

    IMX686_FRM_LENGTH_LINES_MSB_REG     = 0x0340,
    IMX686_FRM_LENGTH_LINES_LSB_REG     = 0x0341,
    IMX686_LINE_LENGTH_PCK_MSB_REG      = 0x0342,
    IMX686_LINE_LENGTH_PCK_LSB_REG      = 0x0343,

    IMX686_CIT_LSHIFT_REG               = 0x3100,
} IMX686_CTRL_REG_e;

#if 0
typedef enum _IMX686_READOUT_MODE_e_ {
    IMX686_8000_6000_10BIT = 0,
    IMX686_4000_3000_10BIT,
    IMX686_4000_3000_10BIT_PDAF,
    IMX686_2000_1128_10BIT,
    IMX686_1920_1080_10BIT_PDAF,
    IMX686_1000_748_10BIT_PDAF,
    IMX686_4000_3000_10BIT_HDR,

    IMX686_NUM_READOUT_MODE,
} IMX686_READOUT_MODE_e;
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
typedef enum {
    AMBA_SENSOR_IMX686_9248_6944_30P,
    AMBA_SENSOR_IMX686_9248_6944_15P,
    AMBA_SENSOR_IMX686_4624_2680_60P,
    AMBA_SENSOR_IMX686_3840_2160_120P,
    AMBA_SENSOR_IMX686_3840_2160_60P,
    AMBA_SENSOR_IMX686_3840_2160_30P,

    AMBA_SENSOR_IMX686_NUM_MODE,
} AMBA_SENSOR_IMX686_MODE_ID_e;

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} IMX686_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[AMBA_SENSOR_IMX686_NUM_MODE];
} IMX686_MODE_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  PixelRate;                      /* Sensor side pixel rate */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* frame rate value of this sensor mode */
} IMX686_FRAME_TIMING_s;

typedef struct {
    IMX686_FRAME_TIMING_s   FrameTiming;
    UINT32  HdrType;
} IMX686_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgc[3];
    UINT32                      CurrentDgc[3];
    UINT32                      CurrentShrCtrl[3];
} IMX686_CTRL_s;



/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX686.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX686DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX686InputInfoNormalReadout[AMBA_SENSOR_IMX686_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX686OutputInfo[AMBA_SENSOR_IMX686_NUM_MODE];
extern const IMX686_MODE_INFO_s IMX686ModeInfoList[AMBA_SENSOR_IMX686_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX686HdrInfo[AMBA_SENSOR_IMX686_NUM_MODE];

extern IMX686_MODE_REG_s IMX686ModeRegTable[IMX686_NUM_READOUT_MODE_REG];
extern IMX686_REG_s IMX686GlobalRegTable[IMX686_NUM_GOLBAL_REG];
extern IMX686_REG_s IMX686ImageQualityRegTable[IMX686_NUM_IMG_QUALITY_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX686Obj;

#endif /* _AMBA_SENSOR_IMX686_H_ */
