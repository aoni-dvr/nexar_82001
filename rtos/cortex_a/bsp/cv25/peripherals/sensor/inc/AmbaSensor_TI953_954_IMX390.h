/**
 *  @file AmbaSensor_TI953_954_IMX390.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Control APIs of TI953_954 plus SONY IMX390 CMOS sensor with MIPI interface
 *
 */

#ifndef TI953_954_IMX390_H
#define TI953_954_IMX390_H

#define TI01_IMX390_NUM_MAX_SENSOR_COUNT       2U

#define TI01_IMX390_SENSOR_SLAVE_ADDR       0x42U
#define TI01_IMX390_SENSOR_I2C_MAX_SIZE       64U

#define TI01_IMX390_MAX_AGAIN               30.0f /* in dB (TBD) */
#define TI01_IMX390_MAX_DGAIN               42.0f
#define TI01_IMX390_MAX_TOTAL_GAIN          (TI01_IMX390_MAX_AGAIN + TI01_IMX390_MAX_DGAIN)

/* SensorID definition */
#define TI01_IMX390_SENSOR_ID_CHAN_A     (0x00000010U)
#define TI01_IMX390_SENSOR_ID_CHAN_B     (0x00000100U)

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define TI01_IMX390_NUM_REG_CASE5             2213U
#define TI01_IMX390_NUM_REG_CASE7             1964U
#define TI01_IMX390_NUM_REG_CASE8             1965U
#define TI01_IMX390_NUM_REG_CASE7_OTPM2         45U
#define TI01_IMX390_NUM_REG_CASE7_OTPM4         46U
#define TI01_IMX390_NUM_REG_CASE8_OTPM2         58U
#define TI01_IMX390_NUM_REG_CASE8_OTPM4         59U

#define TI01_IMX390_STANDBY             0x0000U
#define TI01_IMX390_SHS1                0x000CU
#define TI01_IMX390_SHS2                0x0010U
#define TI01_IMX390_FMAX                0x0090U

#define TI01_IMX390_AE_MODE             0x34A0U /* [0] SHS2_SET_MODE, [1] ADGAIN_SP1H_SEP_MODE, [2] ADGAIN_SP1L_SEP_MODE, [3] ADGAIN_SP2_SEP_MODE */

/* Gain Independent setting mode */
#define TI01_IMX390_AGAIN_SP1H          0x0018U
#define TI01_IMX390_AGAIN_SP1L          0x001AU
#define TI01_IMX390_PGA_SP1H            0x0024U
#define TI01_IMX390_PGA_SP1L            0x0026U
#define TI01_IMX390_PGA_SP2             0x0028U

/* Total Gain setting mode */
#define TI01_IMX390_GAIN_SP1H           0x002CU
#define TI01_IMX390_GAIN_SP1L           0x002EU
#define TI01_IMX390_AGAIN_SP1H_LIMIT    0x34A8U
#define TI01_IMX390_AGAIN_SP1L_LIMIT    0x34AAU

#define TI01_IMX390_WBGAIN_R            0x0030U
#define TI01_IMX390_WBGAIN_GR           0x0032U
#define TI01_IMX390_WBGAIN_GB           0x0034U
#define TI01_IMX390_WBGAIN_B            0x0036U

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define TI01_IMX390_1920_1080_60P_HDR    0U  /* UC24 to PWL12 60fps, 4-lane */
#define TI01_IMX390_1920_1080_30P        1U  /* Normal mode 30fps (SP1L), 4-lane */
#define TI01_IMX390_1920_1080_30P_HDR    2U  /* UC24 to PWL12 30fps, 4-lane (longer VB) */
#define TI01_IMX390_1936_1108_60P_HDR    3U  /* UC24 to PWL12 60fps, 4-lane (for IMX390 calibration only) */
#define TI01_IMX390_1936_1108_30P        4U  /* Normal mode 30fps (SP1L), 4-lane (for IMX390 calibration only) */
#define TI01_IMX390_1920_1080_30P_HDR1   5U  /* UC24 to PWL12 30fps, 2-lane */

#define TI01_IMX390_1920_1080_60P_HDR1   6U  /* UC20 to PWL12 60fps, 4-lane */
#define TI01_IMX390_1920_1080_30P_HDR2   7U  /* UC20 to PWL12 30fps, 4-lane (longer VB) */
#define TI01_IMX390_1920_1080_30P_HDR3   8U  /* UC20 to PWL12 30fps, 2-lane */
#define TI01_IMX390_1920_1080_A60P_HDR1  9U  /* UC20 to PWL12 accurate 60fps, 4-lane */
#define TI01_IMX390_1920_1080_A30P_HDR2 10U  /* UC20 to PWL12 accurate 30fps, 4-lane */

#define TI01_IMX390_1920_1080_A60P_HDR  11U  /* UC24 to PWL12 60fps, 4-lane */
#define TI01_IMX390_1920_1080_A30P_HDR  12U  /* UC24 to PWL12 30fps, 4-lane (longer VB) */

#define TI01_IMX390_NUM_MODE            13U


#define TI01_IMX390_CHIP_VERSION_105DEG     0U
#define TI01_IMX390_CHIP_VERSION_60DEG      1U
#define TI01_IMX390_CHIP_VERSION_CASE5      2U
#define TI01_IMX390_CHIP_VERSION_CASE7      3U
#define TI01_IMX390_CHIP_VERSION_CASE8      4U


typedef struct {
    UINT16  Addr;
    UINT8   Data[TI01_IMX390_NUM_MODE];
} TI01_IMX390_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} TI01_IMX390_SEQ_REG_s;

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
} TI01_IMX390_SENSOR_INFO_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s  FrameRate;                  /* Frame rate value of this sensor mode */
    FLOAT RowTime;
    FLOAT PixelRate;
} TI01_IMX390_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s  Status;
    UINT32                     CurrentAgcCtrl[TI01_IMX390_NUM_MAX_SENSOR_COUNT][3];      /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    UINT32                     CurrentDgcCtrl[TI01_IMX390_NUM_MAX_SENSOR_COUNT][3];      /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    AMBA_SENSOR_WB_CTRL_s      CurrentWbCtrl[TI01_IMX390_NUM_MAX_SENSOR_COUNT][3];       /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    UINT32                     CurrentShutterCtrl[TI01_IMX390_NUM_MAX_SENSOR_COUNT][3];  /* [0]: Long frame, [1]: Short frame, [2]: Very Short frame */
    UINT16                     EnabledSensorID;                                          /* RxPortID in TI serdes driver */
} TI01_IMX390_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_TI953_954_IMX390.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s TI01_IMX390DeviceInfo;
extern const TI01_IMX390_SENSOR_INFO_s TI01_IMX390SensorInfo[TI01_IMX390_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s TI01_IMX390InputInfo[TI01_IMX390_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s TI01_IMX390OutputInfo[TI01_IMX390_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s TI01_IMX390HdrInfo[TI01_IMX390_NUM_MODE];
extern const TI01_IMX390_REG_s TI01_IMX390_RegTableCase5[TI01_IMX390_NUM_REG_CASE5];
extern const TI01_IMX390_REG_s TI01_IMX390_RegTableCase7[TI01_IMX390_NUM_REG_CASE7];
extern const TI01_IMX390_REG_s TI01_IMX390_RegTableCase8[TI01_IMX390_NUM_REG_CASE8];
extern const TI01_IMX390_SEQ_REG_s TI01_IMX390_RegTableCase7_OTPM2[TI01_IMX390_NUM_REG_CASE7_OTPM2];
extern const TI01_IMX390_SEQ_REG_s TI01_IMX390_RegTableCase7_OTPM4[TI01_IMX390_NUM_REG_CASE7_OTPM4];
extern const TI01_IMX390_SEQ_REG_s TI01_IMX390_RegTableCase8_OTPM2[TI01_IMX390_NUM_REG_CASE8_OTPM2];
extern const TI01_IMX390_SEQ_REG_s TI01_IMX390_RegTableCase8_OTPM4[TI01_IMX390_NUM_REG_CASE8_OTPM4];
extern TI01_IMX390_MODE_INFO_s TI01_IMX390ModeInfoList[TI01_IMX390_NUM_MODE];
extern AMBA_SENSOR_OBJ_s AmbaSensor_TI01_IMX390Obj;

#endif /* TI953_954_IMX390_H */
