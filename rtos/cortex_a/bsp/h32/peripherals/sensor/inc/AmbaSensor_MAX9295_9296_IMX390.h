/**
 *  @file AmbaSensor_MAX9295_9296_IMX390.h
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
 *  @details Control APIs of MAXIM MAX9295_9296 plus SONY IMX390 CMOS sensor with MIPI interface
 *
 */

#ifndef MAX9295_9296_IMX390_H
#define MAX9295_9296_IMX390_H

#define MX00_IMX390_NUM_VIN0_SENSOR      1U
#define MX00_IMX390_NUM_VIN1_SENSOR      0U

#define MX00_IMX390_NUM_MAX_SENSOR_COUNT 2U

#define MX00_IMX390_I2C_SLAVE_ADDRESS    0x42U
#define MX00_IMX390_SENSOR_I2C_MAX_SIZE  64U

#define MX00_IMX390_MAX_AGAIN      30.0f /* in dB (TBD) */
#define MX00_IMX390_MAX_DGAIN      42.0f
#define MX00_IMX390_MAX_TOTAL_GAIN (MX00_IMX390_MAX_AGAIN + MX00_IMX390_MAX_DGAIN)

/* SensorID definition */
#define MX00_IMX390_SENSOR_ID_CHAN_A   (0x00000010U) /* sensor on MAX9295 connecting with Channel-A of MAX9296 */
#define MX00_IMX390_SENSOR_ID_CHAN_B   (0x00000100U) /* sensor on MAX9295 connecting with Channel-B of MAX9296 */

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define MX00_IMX390_NUM_REG_CASE5             2213U
#define MX00_IMX390_NUM_REG_CASE7             1964U
#define MX00_IMX390_NUM_REG_CASE8             1973U//1965U  
#define MX00_IMX390_NUM_REG_CASE7_OTPM2      45U
#define MX00_IMX390_NUM_REG_CASE7_OTPM4      46U
#define MX00_IMX390_NUM_REG_CASE8_OTPM2      58U
#define MX00_IMX390_NUM_REG_CASE8_OTPM4      59U


#define MX00_IMX390_STANDBY             0x0000U
#define MX00_IMX390_SHS1                0x000CU
#define MX00_IMX390_SHS2                0x0010U
#define MX00_IMX390_FMAX                0x0090U

#define MX00_IMX390_AE_MODE             0x34A0U /* [0] SHS2_SET_MODE, [1] ADGAIN_SP1H_SEP_MODE, [2] ADGAIN_SP1L_SEP_MODE, [3] ADGAIN_SP2_SEP_MODE */

/* Gain Independent setting mode */
#define MX00_IMX390_AGAIN_SP1H          0x0018U
#define MX00_IMX390_AGAIN_SP1L          0x001AU
#define MX00_IMX390_PGA_SP1H            0x0024U
#define MX00_IMX390_PGA_SP1L            0x0026U
#define MX00_IMX390_PGA_SP2             0x0028U

/* Total Gain setting mode */
#define MX00_IMX390_GAIN_SP1H           0x002CU
#define MX00_IMX390_GAIN_SP1L           0x002EU
#define MX00_IMX390_AGAIN_SP1H_LIMIT    0x34A8U
#define MX00_IMX390_AGAIN_SP1L_LIMIT    0x34AAU

#define MX00_IMX390_WBGAIN_R            0x0030U
#define MX00_IMX390_WBGAIN_GR           0x0032U
#define MX00_IMX390_WBGAIN_GB           0x0034U
#define MX00_IMX390_WBGAIN_B            0x0036U

#define MX00_IMX390_SM_REVERSE_APL      0x03C0U
#define MX00_IMX390_REVERSE             0x0074U

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX00_IMX390_1920_1080_60P_HDR    0U  /* UC24 to PWL12 60fps, 4-lane */
#define MX00_IMX390_1920_1080_30P        1U  /* Normal mode 30fps (SP1L), 4-lane */
#define MX00_IMX390_1920_1080_30P_HDR    2U  /* UC24 to PWL12 30fps, 4-lane (longer VB) */
#define MX00_IMX390_1936_1108_60P_HDR    3U  /* UC24 to PWL12 60fps, 4-lane (for IMX390 calibration only) */
#define MX00_IMX390_1936_1108_30P        4U  /* Normal mode 30fps (SP1L), 4-lane (for IMX390 calibration only) */
#define MX00_IMX390_1920_1080_30P_HDR1   5U  /* UC24 to PWL12 30fps, 2-lane */

#define MX00_IMX390_1920_1080_60P_HDR1   6U  /* UC20 to PWL12 60fps, 4-lane */
#define MX00_IMX390_1920_1080_30P_HDR2   7U  /* UC20 to PWL12 30fps, 4-lane (longer VB) */
#define MX00_IMX390_1920_1080_30P_HDR3   8U  /* UC20 to PWL12 30fps, 2-lane */
#define MX00_IMX390_1920_1080_A60P_HDR1  9U  /* UC20 to PWL12 accurate 60fps, 4-lane */
#define MX00_IMX390_1920_1080_A30P_HDR2 10U  /* UC20 to PWL12 accurate 30fps, 4-lane */

#define MX00_IMX390_1920_1080_A60P_HDR  11U  /* UC24 to PWL12 60fps, 4-lane */
#define MX00_IMX390_1920_1080_A30P_HDR  12U  /* UC24 to PWL12 30fps, 4-lane (longer VB) */

#define MX00_IMX390_NUM_MODE            13U


#define MX00_IMX390_CHIP_VERSION_105DEG     0U
#define MX00_IMX390_CHIP_VERSION_60DEG      1U
#define MX00_IMX390_CHIP_VERSION_CASE5      2U
#define MX00_IMX390_CHIP_VERSION_CASE7      3U
#define MX00_IMX390_CHIP_VERSION_CASE8      4U


typedef struct {
    UINT16 Addr;
    UINT8  Data[MX00_IMX390_NUM_MODE];
} MX00_IMX390_REG_s;

typedef struct {
    UINT16 Addr;
    UINT8  Data;
} MX00_IMX390_SEQ_REG_s;

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
} MX00_IMX390_SENSOR_INFO_s;


typedef struct {
    UINT32  InputClk;                                  /* Sensor side input clock frequency */
    UINT32  HMAX;                                      /* HMAX */
    UINT32  VMAX;                                      /* VMAX */
    AMBA_VIN_FRAME_RATE_s  FrameRate;                  /* Frame rate value of this sensor mode */
} MX00_IMX390_FRAME_TIMING_s;

typedef struct {
    MX00_IMX390_FRAME_TIMING_s       FrameTime;
    FLOAT                             RowTime;    /* Calculated in MX00_IMX390_Init */
    FLOAT                             PixelRate;  /* Calculated in MX00_IMX390_Init */
} MX00_IMX390_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s  Status;
    UINT32                     CurrentAgcCtrl[MX00_IMX390_NUM_MAX_SENSOR_COUNT][3];      /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    UINT32                     CurrentDgcCtrl[MX00_IMX390_NUM_MAX_SENSOR_COUNT][3];      /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    AMBA_SENSOR_WB_CTRL_s      CurrentWbCtrl[MX00_IMX390_NUM_MAX_SENSOR_COUNT][3];       /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    UINT32                     CurrentShutterCtrl[MX00_IMX390_NUM_MAX_SENSOR_COUNT][3];  /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
} MX00_IMX390_CTRL_s;

typedef struct {
    UINT16  Reg0x3060;
    UINT16  Reg0x3067; //Only check [7:4]
    UINT16  Reg0x3064;
    UINT8   ChipVer;
} MX00_IMX390_CHIP_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MX00_IMX390.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MX00_IMX390_DeviceInfo;
extern const MX00_IMX390_SENSOR_INFO_s MX00_IMX390_SensorInfo[MX00_IMX390_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s MX00_IMX390_InputInfo[MX00_IMX390_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX00_IMX390_OutputInfo[MX00_IMX390_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX00_IMX390_HdrInfo[MX00_IMX390_NUM_MODE];
extern const MX00_IMX390_REG_s MX00_IMX390_RegTableCase5[MX00_IMX390_NUM_REG_CASE5];
extern const MX00_IMX390_REG_s MX00_IMX390_RegTableCase7[MX00_IMX390_NUM_REG_CASE7];
extern const MX00_IMX390_REG_s MX00_IMX390_RegTableCase8[MX00_IMX390_NUM_REG_CASE8];
extern const MX00_IMX390_SEQ_REG_s MX00_IMX390_RegTableCase7_OTPM2[MX00_IMX390_NUM_REG_CASE7_OTPM2];
extern const MX00_IMX390_SEQ_REG_s MX00_IMX390_RegTableCase7_OTPM4[MX00_IMX390_NUM_REG_CASE7_OTPM4];
extern const MX00_IMX390_SEQ_REG_s MX00_IMX390_RegTableCase8_OTPM2[MX00_IMX390_NUM_REG_CASE8_OTPM2];
extern const MX00_IMX390_SEQ_REG_s MX00_IMX390_RegTableCase8_OTPM4[MX00_IMX390_NUM_REG_CASE8_OTPM4];
extern MX00_IMX390_MODE_INFO_s MX00_IMX390ModeInfoList[MX00_IMX390_NUM_MODE];

extern AMBA_SENSOR_OBJ_s AmbaSensor_MX00_IMX390Obj;

#endif /* MAX9295_9296_IMX390_H */
