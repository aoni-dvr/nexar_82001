/**
 *  @file AmbaSensor_IMX390.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Control APIs of MAXIM MAX9295_9296 plus SONY IMX390 CMOS sensor with MIPI interface
 *
 */

#ifndef MAX9295_9296_IMX390_H
#define MAX9295_9296_IMX390_H

#define IMX390_NUM_VIN_CHANNEL      2U

#define IMX390_NUM_VIN0_SENSOR      1U
#define IMX390_NUM_VIN1_SENSOR      0U

#define IMX390_NUM_MAX_SENSOR_COUNT 2U

#ifdef CONFIG_BSP_MAGNA_EDB
#define IMX390_I2C_SLAVE_ADDRESS    0x34U
#else
#define IMX390_I2C_SLAVE_ADDRESS    0x42U
#endif
#define IMX390_SENSOR_I2C_MAX_SIZE  64U

#define IMX390_MAX_AGAIN      30.0f /* in dB (TBD) */
#define IMX390_MAX_DGAIN      42.0f
#define IMX390_MAX_TOTAL_GAIN (IMX390_MAX_AGAIN + IMX390_MAX_DGAIN)

/* SensorID definition */
#define IMX390_SENSOR_ID_CHAN_A   (0x00000010U) /* sensor on MAX9295 connecting with Channel-A of MAX9296 */
#define IMX390_SENSOR_ID_CHAN_B   (0x00000100U) /* sensor on MAX9295 connecting with Channel-B of MAX9296 */

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX390_NUM_REG_CASE5             2213U
#define IMX390_NUM_REG_CASE7             1966U
#define IMX390_NUM_REG_CASE8             1967U
#define IMX390_NUM_REG_CASE7_OTPM2      45U
#define IMX390_NUM_REG_CASE7_OTPM4      46U
#define IMX390_NUM_REG_CASE8_OTPM2      58U
#define IMX390_NUM_REG_CASE8_OTPM4      59U


#define IMX390_STANDBY             0x0000U
#define IMX390_SHS1                0x000CU
#define IMX390_SHS2                0x0010U
#define IMX390_FMAX                0x0090U

#define IMX390_AE_MODE             0x34A0U /* [0] SHS2_SET_MODE, [1] ADGAIN_SP1H_SEP_MODE, [2] ADGAIN_SP1L_SEP_MODE, [3] ADGAIN_SP2_SEP_MODE */

/* Gain Independent setting mode */
#define IMX390_AGAIN_SP1H          0x0018U
#define IMX390_AGAIN_SP1L          0x001AU
#define IMX390_PGA_SP1H            0x0024U
#define IMX390_PGA_SP1L            0x0026U
#define IMX390_PGA_SP2             0x0028U

/* Total Gain setting mode */
#define IMX390_GAIN_SP1H           0x002CU
#define IMX390_GAIN_SP1L           0x002EU
#define IMX390_AGAIN_SP1H_LIMIT    0x34A8U
#define IMX390_AGAIN_SP1L_LIMIT    0x34AAU

#define IMX390_WBGAIN_R            0x0030U
#define IMX390_WBGAIN_GR           0x0032U
#define IMX390_WBGAIN_GB           0x0034U
#define IMX390_WBGAIN_B            0x0036U

#define IMX390_SM_REVERSE_APL      0x03C0U
#define IMX390_REVERSE             0x0074U

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define IMX390_1920_1080_60P_HDR    0U  /* UC24 to PWL12 60fps, 4-lane */
#define IMX390_1920_1080_30P        1U  /* Normal mode 30fps (SP1L), 4-lane */
#define IMX390_1920_1080_30P_HDR    2U  /* UC24 to PWL12 30fps, 4-lane (longer VB) */
#define IMX390_1936_1108_60P_HDR    3U  /* UC24 to PWL12 60fps, 4-lane (for IMX390 calibration only) */
#define IMX390_1936_1108_30P        4U  /* Normal mode 30fps (SP1L), 4-lane (for IMX390 calibration only) */
#define IMX390_1920_1080_30P_HDR1   5U  /* UC24 to PWL12 30fps, 2-lane */

#define IMX390_1920_1080_60P_HDR1   6U  /* UC20 to PWL12 60fps, 4-lane */
#define IMX390_1920_1080_30P_HDR2   7U  /* UC20 to PWL12 30fps, 4-lane (longer VB) */
#define IMX390_1920_1080_30P_HDR3   8U  /* UC20 to PWL12 30fps, 2-lane */
#define IMX390_1920_1080_A60P_HDR1  9U  /* UC20 to PWL12 accurate 60fps, 4-lane */
#define IMX390_1920_1080_A30P_HDR2 10U  /* UC20 to PWL12 accurate 30fps, 4-lane */

#define IMX390_1920_1080_A60P_HDR  11U  /* UC24 to PWL12 60fps, 4-lane */
#define IMX390_1920_1080_A30P_HDR  12U  /* UC24 to PWL12 30fps, 4-lane (longer VB) */

#define IMX390_NUM_MODE            13U


#define IMX390_CHIP_VERSION_105DEG     0U
#define IMX390_CHIP_VERSION_60DEG      1U
#define IMX390_CHIP_VERSION_CASE5      2U
#define IMX390_CHIP_VERSION_CASE7      3U
#define IMX390_CHIP_VERSION_CASE8      4U


typedef struct {
    UINT16 Addr;
    UINT8  Data[IMX390_NUM_MODE];
} IMX390_REG_s;

typedef struct {
    UINT16 Addr;
    UINT8  Data;
} IMX390_SEQ_REG_s;

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
} IMX390_SENSOR_INFO_s;


typedef struct {
    UINT32  InputClk;                                  /* Sensor side input clock frequency */
    UINT32  HMAX;                                      /* HMAX */
    UINT32  VMAX;                                      /* VMAX */
    AMBA_VIN_FRAME_RATE_s  FrameRate;                  /* Frame rate value of this sensor mode */
} IMX390_FRAME_TIMING_s;

typedef struct {
    IMX390_FRAME_TIMING_s       FrameTime;
    FLOAT                             RowTime;    /* Calculated in IMX390_Init */
    FLOAT                             PixelRate;  /* Calculated in IMX390_Init */
} IMX390_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s  Status;
    UINT32                     CurrentAgcCtrl[IMX390_NUM_MAX_SENSOR_COUNT][3];      /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    UINT32                     CurrentDgcCtrl[IMX390_NUM_MAX_SENSOR_COUNT][3];      /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    AMBA_SENSOR_WB_CTRL_s      CurrentWbCtrl[IMX390_NUM_MAX_SENSOR_COUNT][3];       /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    UINT32                     CurrentShutterCtrl[IMX390_NUM_MAX_SENSOR_COUNT][3];  /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
} IMX390_CTRL_s;


/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX390.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX390_DeviceInfo;
extern const IMX390_SENSOR_INFO_s IMX390_SensorInfo[IMX390_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s IMX390_InputInfo[IMX390_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX390_OutputInfo[IMX390_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX390_HdrInfo[IMX390_NUM_MODE];
extern const IMX390_REG_s IMX390_RegTableCase5[IMX390_NUM_REG_CASE5];
extern const IMX390_REG_s IMX390_RegTableCase7[IMX390_NUM_REG_CASE7];
extern const IMX390_REG_s IMX390_RegTableCase8[IMX390_NUM_REG_CASE8];
extern const IMX390_SEQ_REG_s IMX390_RegTableCase7_OTPM2[IMX390_NUM_REG_CASE7_OTPM2];
extern const IMX390_SEQ_REG_s IMX390_RegTableCase7_OTPM4[IMX390_NUM_REG_CASE7_OTPM4];
extern const IMX390_SEQ_REG_s IMX390_RegTableCase8_OTPM2[IMX390_NUM_REG_CASE8_OTPM2];
extern const IMX390_SEQ_REG_s IMX390_RegTableCase8_OTPM4[IMX390_NUM_REG_CASE8_OTPM4];
extern IMX390_MODE_INFO_s IMX390ModeInfoList[IMX390_NUM_MODE];
extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX390Obj;

#endif /* MAX9295_9296_IMX390_H */
