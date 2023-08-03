/**
 *  @file AmbaSensor_MAX96705_9286_MX00_OV2312.h
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
 *  @details Control APIs of OV 9284 CMOS sensor with DVP interface
 *
 */

#ifndef AMBA_SENSOR_MX00_OV2312_H
#define AMBA_SENSOR_MX00_OV2312_H

#define MX00_OV2312_SENSOR_I2C_SLAVE_ADDR            0xC0U    // 0x20 if pull to DOVDD

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define MX00_OV2312_NUM_MODE_REG                        188U
#define MX00_OV2312_NUM_HD_MODE_REG                        12U
//#define MX00_OV2312_NUM_PLL_REG                         18U//18U
#define MX00_OV2312_NUM_MAX_SENSOR_COUNT      2U
#define MX00_OV2312_I2C_WR_BUF_SIZE           64U
#define MX00_OV2312_SC_MODE_SELECT                            0x0100
#define MX00_OV2312_SC_SOFTWARE_RESET                        0x0103
#define MX00_OV2312_GAIN_H                                    0x3508
#define MX00_OV2312_GAIN_L                                    0x3509
#define MX00_OV2312_EXPO_H                                    0x3501
#define MX00_OV2312_EXPO_L                                    0x3502
#define MX00_OV2312_IRLED_STROBE_WIDTH                      0x3927
#define MX00_OV2312_IRLED_STROBE_START                      0x3929


// below is MAXIAM setting

/* SensorID definition */
#define MX00_OV2312_SENSOR_ID_CHAN_0   (0x00000010U) /* sensor on MAX96705 connecting with Channel-0 of MAX9296 */
#define MX00_OV2312_SENSOR_ID_CHAN_1   (0x00000100U) /* sensor on MAX96705 connecting with Channel-1 of MAX9296 */
#define MX00_OV2312_SENSOR_ID_CHAN_2   (0x00001000U) /* sensor on MAX96705 connecting with Channel-2 of MAX9296 */
#define MX00_OV2312_SENSOR_ID_CHAN_3   (0x00010000U) /* sensor on MAX96705 connecting with Channel-3 of MAX9296 */

#define MX00_OV2312_1280_800_A30P                        0U
#define MX00_OV2312_1600_1300_A30P                        1U
#define MX00_OV2312_1280_720_A60P                        2U
#define MX00_OV2312_1600_1300_A60P                        3U
#define MX00_OV2312_NUM_MODE                            4U
typedef struct {
    UINT16  Addr;
    UINT8   Data;
} MX00_OV2312_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  LineLengthPck;                  /* number of pixel clock cycles per line of frame */
    UINT32  FrameLengthLines;               /* number of lines per frame */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} MX00_OV2312_FRAME_TIMING_s;

typedef struct {
    MX00_OV2312_FRAME_TIMING_s       FrameTiming;
    FLOAT                       RowTime;        /* Calculated in IMX290_MIPI_Init */
    FLOAT                        PixelRate;          /* Calculated in MX00_IMX424_Init */
} MX00_OV2312_MODE_INFO_s;

typedef struct {
    UINT32                      CurrentAgcCtrl[2];        /* HDR id: 0 = L, 1 = S */
    UINT32                      CurrentDgcCtrl[2];        /* HDR id: 0 = L, 1 = S */
    UINT32                      CurrentShutterCtrl[2];    /* HDR id: 0 = L, 1 = S */
    FLOAT                        CurrentCG;
    AMBA_SENSOR_WB_CTRL_s       CurrentWbCtrl[2];         /* HDR id: 0 = L, 1 = S */
} MX00_OV2312_CURRENT_AE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    MX00_OV2312_FRAME_TIMING_s         FrameTime;
    MX00_OV2312_CURRENT_AE_INFO_s      CurrentAEInfo;
    UINT32                      EnabledSensorID;
} MX00_OV2312_CTRL_s;

typedef struct {
    UINT32                       InputClk;           /* Sensor side input clock frequency */
    UINT32                       DataRate;           /* Output bit/pixel clock frequency from image sensor */
    UINT8                        NumDataLanes;       /* Active data channels */
    UINT8                        NumDataBits;        /* Pixel data bit depth */
    UINT32                       LineLengthPck;      /* Active pixels + H-blanking */
    UINT32                       FrameLengthLines;   /* Number of output lines incluidng blanking */
    UINT32                       OutputWidth;        /* Valid pixels per line */
    UINT32                       OutputHeight;       /* Valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s      RecordingPixels;    /* Maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s      OpticalBlackPixels; /* User clamp area */
} MX00_OV2312_SENSOR_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_OV10640_MIPI.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MX00_OV2312_DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s MX00_OV2312_InputInfo[MX00_OV2312_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX00_OV2312_OutputInfo[MX00_OV2312_NUM_MODE];
extern MX00_OV2312_MODE_INFO_s MX00_OV2312_ModeInfoList[MX00_OV2312_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX00_OV2312_HdrInfo[MX00_OV2312_NUM_MODE];
extern MX00_OV2312_REG_s MX00_OV2312_RegTable[MX00_OV2312_NUM_MODE_REG];
extern MX00_OV2312_REG_s MX00_OV2312_HDRegTable[MX00_OV2312_NUM_HD_MODE_REG];
extern const MX00_OV2312_SENSOR_INFO_s MX00_OV2312SensorInfo[MX00_OV2312_NUM_MODE];
extern AMBA_SENSOR_OBJ_s AmbaSensor_MX00_OV2312Obj;

#endif /* AMBA_SENSOR_MX00_OV2312_H */
