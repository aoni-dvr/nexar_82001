/**
 *  @file AmbaSensor_MAX96707_9286_AR0144.h
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
 *  @details Control APIs of MAXIM SerDes + AR0144 CMOS sensors with Parallel interface
 *
 */

#ifndef MAX96707_9286_AR0144_H
#define MAX96707_9286_AR0144_H

/* Sensor Slave Address */
#define MX03_AR0144_I2C_ADDR                   0x30U

/* Maximum Sensor Count */
#define MX03_AR0144_NUM_MAX_SENSOR_COUNT       4U

/*-----------------------------------------------------------------------------------------------*\
 * SensorID Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX03_AR0144_SENSOR_ID_CHAN_0   (0x00000010U)
#define MX03_AR0144_SENSOR_ID_CHAN_1   (0x00000100U)
#define MX03_AR0144_SENSOR_ID_CHAN_2   (0x00001000U)
#define MX03_AR0144_SENSOR_ID_CHAN_3   (0x00010000U)

/*-----------------------------------------------------------------------------------------------*\
 * Control Register Counts
\*-----------------------------------------------------------------------------------------------*/
#define MX03_AR0144_NUM_RECOM_REG                  25U
#define MX03_AR0144_NUM_PLL_REG                     9U
#define MX03_AR0144_NUM_READOUT_MODE_REG           12U
#define MX03_AR0144_NUM_AGC_STEP                   37U

/*-----------------------------------------------------------------------------------------------*\
 * Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX03_AR0144_1280_720_30P                    0U
#define MX03_AR0144_1280_720_60P                    1U
#define MX03_AR0144_1280_800_60P                    2U
#define MX03_AR0144_1280_720_A30P                   3U
#define MX03_AR0144_NUM_MODE                        4U

typedef struct {
    UINT16  Addr;
    UINT16  Data[MX03_AR0144_NUM_MODE];
} MX03_AR0144_MODE_REG_s;

typedef struct {
    UINT16  Addr;
    UINT16  Data;
} MX03_AR0144_INIT_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  LineLengthPck;                  /* number of pixel clock cycles per line of frame */
    UINT32  FrameLengthLines;               /* number of lines per frame */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* Framerate value of this sensor mode */
} MX03_AR0144_FRAME_TIMING_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* Framerate value of this sensor mode */
    FLOAT PixelRate;
    FLOAT RowTime;
} MX03_AR0144_MODE_INFO_s;

typedef struct {
    UINT32                      CurrentAgcCtrl[MX03_AR0144_NUM_MAX_SENSOR_COUNT];
    UINT32                      CurrentDgcCtrl[MX03_AR0144_NUM_MAX_SENSOR_COUNT];
    UINT32                      CurrentShutterCtrl[MX03_AR0144_NUM_MAX_SENSOR_COUNT];
    AMBA_SENSOR_WB_CTRL_s       CurrentWbCtrl[MX03_AR0144_NUM_MAX_SENSOR_COUNT];
} MX03_AR0144_CURRENT_AE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s        Status;
    MX03_AR0144_FRAME_TIMING_s       FrameTime;
    MX03_AR0144_CURRENT_AE_INFO_s    CurrentAEInfo;
    UINT32                           EnabledSensorID;
} MX03_AR0144_CTRL_s;

typedef struct {
    FLOAT   Factor;
    UINT16  Data;
} MX03_AR0144_AGC_REG_s;

typedef struct {
    UINT32                       InputClk;           /* Sensor side input clock frequency */
    UINT32                       DataRate;           /* Output bit/pixel clock frequency from image sensor */
    UINT8                        NumDataLanes;       /* Active data channels */
    UINT8                        NumDataBits;        /* Pixel data bit depth */
    UINT32                       LineLengthPck;      /* number of pixel clock cycles per line of frame */
    UINT32                       FrameLengthLines;   /* number of lines per frame */
    UINT32                       OutputWidth;        /* Valid pixels per line */
    UINT32                       OutputHeight;       /* Valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s      RecordingPixels;    /* Maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s      OpticalBlackPixels; /* User clamp area */
} MX03_AR0144_SENSOR_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaSensor_MAX96707_9286_AR0144Table.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MX03_AR0144DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s MX03_AR0144InputInfo[MX03_AR0144_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX03_AR0144OutputInfo[MX03_AR0144_NUM_MODE];
extern const MX03_AR0144_SENSOR_INFO_s MX03_AR0144SensorInfo[MX03_AR0144_NUM_MODE];
extern MX03_AR0144_MODE_INFO_s MX03_AR0144ModeInfoList[MX03_AR0144_NUM_MODE];
extern MX03_AR0144_INIT_REG_s MX03_AR0144RecomRegTable[MX03_AR0144_NUM_RECOM_REG];
extern MX03_AR0144_INIT_REG_s MX03_AR0144PLLRegTable[MX03_AR0144_NUM_PLL_REG];
extern MX03_AR0144_MODE_REG_s MX03_AR0144ModeRegTable[MX03_AR0144_NUM_READOUT_MODE_REG];

extern MX03_AR0144_AGC_REG_s MX03_AR0144AgcRegTable[MX03_AR0144_NUM_AGC_STEP];

extern AMBA_SENSOR_OBJ_s AmbaSensor_MX03_AR0144Obj;

#endif /* MAX96707_9286_AR0144_H */
