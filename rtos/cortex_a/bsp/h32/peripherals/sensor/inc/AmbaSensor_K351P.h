/**
 *  @file AmbaSensor_K351P.h
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
 *  @details Control APIs of SOI K351P CMOS sensor with MIPI interface
 *
 */

#ifndef AMBA_SENSOR_K351P_H
#define AMBA_SENSOR_K351P_H

#define K351P_SENSOR_I2C_SLAVE_ADDR    0x80 /* 0x20, Master, SLASEL = High Level. 0x34, Slave, SLASEL = Low Level or NC */
#define K351P_SENSOR_I2C_MAX_SIZE      64U

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/

#define K351P_NUM_READOUT_MODE_REG        108U

#define K351P_MODE_SEL_REG				   0x12
#define K351P_ANA_GAIN_GLOBAL_MSB_REG	   0x00
#define K351P_COARSE_INTEG_TIME_LSB_REG	   0x01

typedef enum _K351P_READOUT_MODE_e_ {
    K351P_2000_2000_30P_10BIT=0U,   
    K351P_1920_1080_5P_10BIT, 
    K351P_NUM_MODE,
} K351P_READOUT_MODE_e;

typedef struct {
    UINT8  Addr;
    UINT8  Data[K351P_NUM_MODE];
} K351P_REG_s;



typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  LineLengthPck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} K351P_FRAME_TIMING_s;

typedef struct {
    
    K351P_READOUT_MODE_e   ReadoutMode;
    K351P_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in K351P_Init */
} K351P_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgcCtrl[2];
    UINT32                      CurrentDgcCtrl[2];
    UINT32                      CurrentShrCtrl[2];
} K351P_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_K351P.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s K351PDeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s K351PInputInfo[K351P_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s K351POutputInfo[K351P_NUM_MODE];
extern K351P_MODE_INFO_s K351PModeInfoList[K351P_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s K351PHdrInfo[K351P_NUM_MODE];


extern K351P_REG_s K351PRegTable[K351P_NUM_READOUT_MODE_REG];



extern AMBA_SENSOR_OBJ_s AmbaSensor_K351PObj;

#endif /* AMBA_SENSOR_K351P_H */
