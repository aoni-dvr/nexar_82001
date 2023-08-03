/**
 *  @file AmbaSensor_IMX415.h
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
 *  @details Control APIs of SONY IMX415 CMOS sensor with MIPI interface
 *
 */

#ifndef _AMBA_SENSOR_IMX415_H_
#define _AMBA_SENSOR_IMX415_H_

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX415_SENSOR_I2C_SLAVE_ADDR    0x34 /* 0x20, Master, SLASEL = High Level. 0x34, Slave, SLASEL = Low Level or NC */
#define IMX415_SENSOR_I2C_MAX_SIZE      64U


#define IMX415_NUM_READOUT_MODE_REG      169//92
#define IMX415_4K_NUM_READOUT_MODE_REG      98
#define IMX415_1080P_NUM_READOUT_MODE_REG      104

typedef enum _IMX415_CTRL_REG_e_ {
    IMX415_STANDBY_REG  = 0x3000,
    IMX415_XMSTA_REG    = 0x3002,
    /* Shutter Control */
    IMX415_COARSE_INTEG_TIME_HSB_REG    = 0x3052,
    IMX415_COARSE_INTEG_TIME_MSB_REG    = 0x3051,
    IMX415_COARSE_INTEG_TIME_LSB_REG    = 0x3050,
    /* Gain Control */
    IMX415_ANA_GAIN_GLOBAL_MSB_REG      = 0x3091, /* ANA_GAIN_GLOBAL, range: 0 to 240 */
    IMX415_ANA_GAIN_GLOBAL_LSB_REG      = 0x3090,


} IMX415_CTRL_REG_e;

typedef enum _IMX415_READOUT_MODE_e_ {
    IMX415_1920_1080_30P = 0,
	IMX415_3840_2160_30P,
	IMX415_3840_2160_60P,
	IMX415_1920_1080_30P_BINNING,
	IMX415_3840_2160_25P,
	IMX415_4K_25P,
    IMX415_NUM_READOUT_MODE,
} IMX415_READOUT_MODE_e;

typedef struct _IMX415_MODE_REG_s_ {
    UINT16  Addr;
    UINT8   Data[IMX415_NUM_READOUT_MODE];
} IMX415_MODE_REG_s;

typedef struct _IMX415_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  NumTickPerXhs;                  /* XHS period (in 74.25MHz clock cycles) */
    UINT32  NumXhsPerH;                     /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* Framerate value of this sensor mode */
} IMX415_FRAME_TIMING_s;

typedef struct _IMX415_MODE_INFO_s_ {
    IMX415_READOUT_MODE_e   ReadoutMode;
    IMX415_FRAME_TIMING_s   FrameTiming;
	FLOAT                   RowTime;
} IMX415_MODE_INFO_s;

typedef struct _IMX415_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    AMBA_SENSOR_MODE_INFO_s     ModeInfo;
    UINT32                      CurrentPgc[2];
    UINT32                      CurrentShutterCtrl[2];
} IMX415_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_SENSOR_IMX415_MODE_ID_e_ {
    AMBA_SENSOR_IMX415_1920_1080_30P = 0,
	AMBA_SENSOR_IMX415_3840_2160_30P,
	AMBA_SENSOR_IMX415_3840_2160_60P,
	AMBA_SENSOR_IMX415_1920_1080_30P_BINNING,
	AMBA_SENSOR_IMX415_3840_2160_25P,
    AMBA_SENSOR_IMX415_NUM_MODE,
} AMBA_SENSOR_IMX415_MODE_ID_e;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX415.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX415DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX415InputInfoNormalReadout[IMX415_NUM_READOUT_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX415OutputInfo[IMX415_NUM_READOUT_MODE];
extern IMX415_MODE_INFO_s IMX415ModeInfoList[AMBA_SENSOR_IMX415_NUM_MODE];
extern IMX415_MODE_REG_s IMX415ModeRegTable[IMX415_NUM_READOUT_MODE_REG];
extern IMX415_MODE_REG_s IMX415_4KP25_ModeRegTable[IMX415_4K_NUM_READOUT_MODE_REG];
extern IMX415_MODE_REG_s IMX415_1080P30_ModeRegTable[IMX415_1080P_NUM_READOUT_MODE_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX415Obj;
#endif /* _AMBA_SENSOR_IMX415_H_ */
