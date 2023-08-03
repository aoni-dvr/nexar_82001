/**
 *  @file AmbaSensor_TI913_954_OV10635.h
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
 *  @details Control APIs of TI SerDes + OV10635 CMOS sensors with Parallel interface
 *
 */

#ifndef AMBA_SENSOR_TI03_OV10635_H
#define AMBA_SENSOR_TI03_OV10635_H


#define TI03_OV10635_FSYNC_MODE            1U  /* 0:none, 1:external fsync, 2: internal fsync */
#define TI03_OV10635_I2C_ADDR              0x60U
#define TI03_OV10635_BROADCAST_I2C_ADDR    0x90U
#define TI03_OV10635_I2C_DATA_MAX_SIZE     64U
#define TI03_OV10635_NUM_MAX_SENSOR_COUNT  2U

/*-----------------------------------------------------------------------------------------------*\
 * SensorID definition
\*-----------------------------------------------------------------------------------------------*/
#define TI03_OV10635_SENSOR_ID_CHAN_0      (0x00000010U) /* sensor on TI913 connecting with Channel-0 of TI954 */
#define TI03_OV10635_SENSOR_ID_CHAN_1      (0x00000100U) /* sensor on TI913 connecting with Channel-1 of TI954 */

/*-----------------------------------------------------------------------------------------------*\
 * Sensor registers definition
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define TI03_OV10635_NUM_READOUT_MODE_REG  1917U
#define TI03_OV10635_NUM_MANAEC_MODE_REG   31U
#define TI03_OV10635_NUM_FSIN_MODE_REG     5U
#define TI03_OV10635_NUM_SHDR_MODE_REG     1914U
#define TI03_OV10635_NUM_PURERAW_MODE_REG  25U

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define TI03_OV10635_1280_800_30P_LONG     0U
#define TI03_OV10635_1280_800_30P_SHORT    1U
#define TI03_OV10635_1280_800_30P_SHDR     2U
#define TI03_OV10635_NUM_MODE              3U

#define TI03_OV10635_NUM_LINEARMODE        2U


#define OV10635_AEC_MANUAL_EN                   0x56d0  //[0]
#define OV10635_AEC_MANUAL_DONE                 0x3504  //[0]
#define OV10635_GAIN_LONG_H                     0x56d1  //[9:8]
#define OV10635_GAIN_LONG_L                     0x56d2  //[7:0]
#define OV10635_GAIN_SHORT_H                    0x56d3  //[9:8]
#define OV10635_GAIN_SHORT_L                    0x56d4  //[7:0]
#define OV10635_INT_EXP_LONG_H                  0x56d5  //[31:24]
#define OV10635_INT_EXP_LONG_L                  0x56d6  //[23:16]
#define OV10635_FRC_EXP_LONG_H                  0x56d7  //[15:8]
#define OV10635_FRC_EXP_LONG_L                  0x56d8  //[7:0]
#define OV10635_INT_EXP_SHORT_H                 0x56d9  //[31:24]
#define OV10635_INT_EXP_SHORT_L                 0x56da  //[23:16]
#define OV10635_FRC_EXP_SHORT_H                 0x56db  //[15:8]
#define OV10635_FRC_EXP_SHORT_L                 0x56dc  //[7:0]
#define OV10635_AWB_B_LONG_H                    0x5100  //[9:8]
#define OV10635_AWB_B_LONG_L                    0x5101  //[7:0]
#define OV10635_AWB_GB_LONG_H                   0x5102  //[9:8]
#define OV10635_AWB_GB_LONG_L                   0x5103  //[7:0]
#define OV10635_AWB_GR_LONG_H                   0x5104  //[9:8]
#define OV10635_AWB_GR_LONG_L                   0x5105  //[7:0]
#define OV10635_AWB_R_LONG_H                    0x5106  //[9:8]
#define OV10635_AWB_R_LONG_L                    0x5107  //[7:0]
#define OV10635_AWB_B_SHORT_H                   0x5110  //[9:8]
#define OV10635_AWB_B_SHORT_L                   0x5111  //[7:0]
#define OV10635_AWB_GB_SHORT_H                  0x5112  //[9:8]
#define OV10635_AWB_GB_SHORT_L                  0x5113  //[7:0]
#define OV10635_AWB_GR_SHORT_H                  0x5114  //[9:8]
#define OV10635_AWB_GR_SHORT_L                  0x5115  //[7:0]
#define OV10635_AWB_R_SHORT_H                   0x5116  //[9:8]
#define OV10635_AWB_R_SHORT_L                   0x5117  //[7:0]
#define OV10635_VTS_H                           0x380e
#define OV10635_VTS_L                           0x380f


typedef struct {
    UINT16  Addr;
    UINT8   Data[TI03_OV10635_NUM_MODE];
} TI03_OV10635_MODE_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[TI03_OV10635_NUM_LINEARMODE];
} TI03_OV10635_LINEARMODE_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} TI03_OV10635_SEQ_REG_s;

typedef struct {
    UINT32  InputClk;                                    /* Sensor side input clock frequency */
    UINT32  DataRate;                                    /* output bit/pixel clock frequency from image sensor */
    UINT8   NumDataLanes;                                /* active data channels */
    UINT8   NumDataBits;                                 /* pixel data bit depth */
    UINT32  LineLengthPck;                               /* Active pixels + H-blanking */
    UINT32  FrameLengthLines;                            /* Active lines + V-blanking */
    UINT32  OutputWidth;                                 /* valid pixels per line */
    UINT32  OutputHeight;                                /* valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s RecordingPixels;             /* maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s OpticalBlackPixels;          /* user clamp area */
} TI03_OV10635_SENSOR_INFO_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s               FrameRate;       /* framerate value of this sensor mode */
    FLOAT                               RowTime;
    FLOAT                               PixelRate;
} TI03_OV10635_MODE_INFO_s;

typedef struct {
    UINT32                              CurrentAgcCtrl[TI03_OV10635_NUM_MAX_SENSOR_COUNT][2];        /* HDR id: 0 = L, 1 = S */
    UINT32                              CurrentDgcCtrl[TI03_OV10635_NUM_MAX_SENSOR_COUNT][2];        /* HDR id: 0 = L, 1 = S */
    UINT32                              CurrentShutterCtrl[TI03_OV10635_NUM_MAX_SENSOR_COUNT][2];    /* HDR id: 0 = L, 1 = S */
    AMBA_SENSOR_WB_CTRL_s               CurrentWbCtrl[TI03_OV10635_NUM_MAX_SENSOR_COUNT][2];         /* HDR id: 0 = L, 1 = S */
} TI03_OV10635_CURRENT_AE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s           Status;
    TI03_OV10635_CURRENT_AE_INFO_s CurrentAEInfo;
    UINT32                              EnabledSensorID;
} TI03_OV10635_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_TI03_OV10635.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s TI03_OV10635DeviceInfo;
extern const TI03_OV10635_SENSOR_INFO_s TI03_OV10635SensorInfo[TI03_OV10635_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s TI03_OV10635InputInfo[TI03_OV10635_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s TI03_OV10635OutputInfo[TI03_OV10635_NUM_MODE];
extern TI03_OV10635_MODE_INFO_s TI03_OV10635ModeInfoList[TI03_OV10635_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s TI03_OV10635HdrInfo[TI03_OV10635_NUM_MODE];

extern const TI03_OV10635_LINEARMODE_REG_s TI03_OV10635ModeRegTable[TI03_OV10635_NUM_READOUT_MODE_REG];
extern const TI03_OV10635_SEQ_REG_s TI03_OV10635SHDRRegTable[TI03_OV10635_NUM_SHDR_MODE_REG];
extern const TI03_OV10635_MODE_REG_s TI03_OV10635PureRawRegTable[TI03_OV10635_NUM_PURERAW_MODE_REG];
extern const TI03_OV10635_MODE_REG_s TI03_OV10635MANAECRegTable[TI03_OV10635_NUM_MANAEC_MODE_REG];
extern const TI03_OV10635_MODE_REG_s TI03_OV10635FSINRegTable[TI03_OV10635_NUM_FSIN_MODE_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_TI03_OV10635Obj;

#endif /* AMBA_SENSOR_TI03_OV10635_H */



