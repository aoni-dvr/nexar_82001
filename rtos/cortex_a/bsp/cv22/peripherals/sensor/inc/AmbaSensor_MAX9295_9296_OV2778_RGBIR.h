/**
 *  @file AmbaSensor_MX00_OV2778.h
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
 *  @details Control APIs of SONY MX00_OV2778 CMOS sensor with MIPI interface
 *
 */

#ifndef MX00_OV2778_H
#define MX00_OV2778_H

#define MX00_OV2778_SENSOR_I2C_MAX_SIZE          64U
#define MX00_OV2778_NUM_VIN0_SENSOR              1U
#define MX00_OV2778_NUM_VIN1_SENSOR              0U
#define MX00_OV2778_NUM_MAX_SENSOR_COUNT         1U
#define MX00_OV2778_SENSOR_I2C_SLAVE_ADDR        0x20
#define MX00_OV2778_I2C_WR_BUF_SIZE               64U


#define MX00_OV2778_HCG_SWITCH                  14.85f //switch to HCG (MIN_HCG*GAIN_RATIO)X~64X
#define MX00_OV2778_LCG_SWITCH                  13.5f  //switch to LCG (MIN_HCG*GAIN_RATIO)X~64X
#define MX00_OV2778_GAIN_RATIO                  10.8f  //brightness ratio between HCG and LCG
#define MX00_OV2778_WB_GAIN_MIN                 1.0f
#define MX00_OV2778_WB_GAIN_MAX                 15.996f
#define MX00_OV2778_MIN_DGAIN_HCG                 1.25f
#define MX00_OV2778_MIN_AGAIN_DGAIN_LCG         3.0f
#define MX00_OV2778_MIN_DGAIN_LCG                 1.09375f
#define MX00_OV2778_HCG_LCG_RATIO_CONSTRAINS     1.60f
#define MX00_OV2778_NUM_READOUT_MODE_REG        1809U
#define MX00_OV2778_FHD_NUM_READOUT_MODE_REG    1806U
#define MX00_OV2778_DCG_NUM_READOUT_MODE_REG    1844U
#define MX00_OV2778_DCGVS_NUM_READOUT_MODE_REG  1875U


/* SensorID definition */
#define MX00_OV2778_SENSOR_ID_CHAN_A          (0x00000010U)    /* sensor on MAX9295 connecting with Channel-A of MAX9296 */
#define MX00_OV2778_SENSOR_ID_CHAN_B          (0x00000100U)    /* sensor on MAX9295 connecting with Channel-B of MAX9296 */

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define MX00_OV2778_SFW_CTRL1_STANDBY           0x3012U
#define MX00_OV2778_WB_CTRL_START_ADDR          0xb360U
#define MX00_OV2778_SFW_CTRL2_SW_RST            0x3013U
#define MX00_OV2778_GROUP_CTRL                  0x3464U
#define MX00_OV2778_OPERATION_CTRL              0x3467U
#define MX00_OV2778_CEXP_DCG_H                  0x30b6U
#define MX00_OV2778_CG_AGAIN                    0x30bbU
#define MX00_OV2778_CG_AGAIN_GROUP_HOLD         0xb0bbU
#define MX00_OV2778_DIG_GAIN_HCG_H              0x315aU
#define MX00_OV2778_DIG_GAIN_HCG_H_GROUP_HOLD   0xb15aU
/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX00_OV2778_1280_720_A30P                0U          /* 12bit A/D, 12bit OUT, non binning  */
#define MX00_OV2778_1920_1080_A30P                1U          /* 12bit A/D, 12bit OUT, non binning  */
#define MX00_OV2778_1920_1080_DCG_A30P          2U          /* 16bit DCG combine, non binning ,16bit is OK for bridge board but NG with 9295/9296 */
#define MX00_OV2778_1920_1080_DCG12b_A30P       3U
#define MX00_OV2778_1920_1080_DCGVS_A30P        4U
#define MX00_OV2778_NUM_MODE                     5U

typedef struct {
    UINT16 Addr;
    UINT8  Data;
} MX00_OV2778_REG_s;


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
} MX00_OV2778_SENSOR_INFO_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} MX00_OV2778_FRAME_TIMING_s;

typedef struct {
    UINT32                            OperationMode;
    MX00_OV2778_FRAME_TIMING_s              FrameTime;
    FLOAT                             RowTime;    /* Calculated in MX00_OV2778_Init */
    FLOAT                             PixelRate;  /* Calculated in MX00_OV2778_Init */
} MX00_OV2778_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s                Status;
    MX00_OV2778_FRAME_TIMING_s                     FrameTime;
    AMBA_SENSOR_WB_CTRL_s                    CurrentWbCtrl[MX00_OV2778_NUM_MAX_SENSOR_COUNT][3];
    UINT32                                   CurrentShrCtrl[MX00_OV2778_NUM_MAX_SENSOR_COUNT][3];
    UINT32                                   CurrentAGCtrl[MX00_OV2778_NUM_MAX_SENSOR_COUNT][3];
    UINT32                                   CurrentDGCtrl[MX00_OV2778_NUM_MAX_SENSOR_COUNT][3];
    UINT32                                   CurrentGainType[MX00_OV2778_NUM_MAX_SENSOR_COUNT][3];
} MX00_OV2778_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MX00_OV2778Table.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MX00_OV2778_DeviceInfo;
extern const MX00_OV2778_SENSOR_INFO_s MX00_OV2778_SensorInfo[MX00_OV2778_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s MX00_OV2778_InputInfo[MX00_OV2778_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX00_OV2778_OutputInfo[MX00_OV2778_NUM_MODE];
extern MX00_OV2778_MODE_INFO_s MX00_OV2778_ModeInfoList[MX00_OV2778_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX00_OV2778_HdrInfo[MX00_OV2778_NUM_MODE];

extern MX00_OV2778_REG_s MX00_OV2778RegTable[MX00_OV2778_NUM_READOUT_MODE_REG];
extern MX00_OV2778_REG_s MX00_OV2778RegTableFHD[MX00_OV2778_FHD_NUM_READOUT_MODE_REG];
extern MX00_OV2778_REG_s MX00_OV2778RegTableDCG[MX00_OV2778_DCG_NUM_READOUT_MODE_REG];
extern MX00_OV2778_REG_s MX00_OV2778DCG_VSRegTable[MX00_OV2778_DCGVS_NUM_READOUT_MODE_REG];


extern AMBA_SENSOR_OBJ_s AmbaSensor_MX00_OV2778Obj;

#endif /* MX00_OV2778_H */
