/**
 *  @file AmbaSensor_B8N_IMX290.h
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
 *  @details Control APIs of SONY B8N_IMX290 CMOS sensor with LVDS interface
 *
 */

#ifndef AMBA_SENSOR_B8N_IMX290_H
#define AMBA_SENSOR_B8N_IMX290_H

#include "AmbaB8.h"

#define B8N_IMX290_SENSOR_I2C_SLAVE_ADDR       0x34U

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define B8N_IMX290_NUM_INIT_REG                43U
#define B8N_IMX290_NUM_MODE_REG                69U

#define B8N_IMX290_STANDBY         0x3000U
#define B8N_IMX290_MASTERSTOP      0x3002U
#define B8N_IMX290_RESET           0x3003U
#define B8N_IMX290_AGAIN           0x3014U
#define B8N_IMX290_VMAX_LSB        0x3018U
#define B8N_IMX290_VMAX_MSB        0x3019U
#define B8N_IMX290_VMAX_HSB        0x301AU
#define B8N_IMX290_SHS1_LSB        0x3020U
#define B8N_IMX290_SHS1_MSB        0x3021U
#define B8N_IMX290_SHS1_HSB        0x3022U
#define B8N_IMX290_SHS2_LSB        0x3024U
#define B8N_IMX290_SHS2_MSB        0x3025U
#define B8N_IMX290_SHS2_HSB        0x3026U
#define B8N_IMX290_SHS3_LSB        0x3028U
#define B8N_IMX290_SHS3_MSB        0x3029U
#define B8N_IMX290_SHS3_HSB        0x302AU
#define B8N_IMX290_RHS1_LSB        0x3030U
#define B8N_IMX290_RHS1_MSB        0x3031U
#define B8N_IMX290_RHS1_HSB        0x3032U
#define B8N_IMX290_RHS2_LSB        0x3034U
#define B8N_IMX290_RHS2_MSB        0x3035U
#define B8N_IMX290_RHS2_HSB        0x3036U
#define B8N_IMX290_DOL_FORMAT      0x3045U
#define B8N_IMX290_DOL_SYNCSIGNAL  0x3106U
#define B8N_IMX290_DOL_HBFIXEN     0x3107U
#define B8N_IMX290_NULL0_SIZEV     0x3415U



#define B8N_IMX290_1920_1080_P60        0U
#define B8N_IMX290_1920_1080_P30        1U
#define B8N_IMX290_1920_1080_P60_HDR    2U  /* DOL 2-frame 60fps */
#define B8N_IMX290_1920_1080_P30_HDR_0  3U  /* DOL 2-frame 30fps */
#define B8N_IMX290_1920_1080_P30_HDR_1  4U  /* DOL 3-frame 30fps */
#define B8N_IMX290_1820_400_P60_HDR     5U  /* DOL 2-frame 60fps */
#define B8N_IMX290_1920_500_P60_HDR     6U  /* DOL 3-frame 60fps */
#define B8N_IMX290_1096_736_P60_HDR     7U  /* DOL 2-frame 60fps */
#define B8N_IMX290_1920_1080_P50        8U
#define B8N_IMX290_1920_1080_P25        9U
#define B8N_IMX290_1920_1080_P50_HDR    10U /* DOL 2-frame 50fps */
#define B8N_IMX290_1920_1080_P25_HDR_0  11U /* DOL 2-frame 25fps */
#define B8N_IMX290_1920_1080_P25_HDR_1  12U /* DOL 3-frame 25fps */
#define B8N_IMX290_1820_400_P50_HDR     13U /* DOL 2-frame 50fps */
#define B8N_IMX290_1920_1080_P20        14U
#define B8N_IMX290_NUM_MODE             15U

typedef struct {
    UINT16  Addr;
    UINT8  Data;
} B8N_IMX290_SEQ_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8  Data[B8N_IMX290_NUM_MODE];
} B8N_IMX290_REG_s;

typedef struct {
    UINT16  CfaOutputWidth;     /* Frame width after Pre-scaler */
    UINT32  SerdesDataRate;     /* SERDES Data Rate */
    UINT32  CompressionRatio;   /* Compression Ratio */
} B8N_IMX290_B8_CTRL_s;

typedef struct {
    UINT32                           InputClk;           /* Sensor side input clock frequency */
    UINT32                           DataRate;           /* Output bit/pixel clock frequency from image sensor */
    UINT8                            NumDataLanes;       /* Active data channels */
    UINT8                            NumDataBits;        /* Pixel data bit depth */
    UINT32                           LineLengthPck;      /* XHS period (in input clock cycles) */
    UINT32                           FrameLengthLines;   /* Horizontal operating period (in number of XHS pulses) */
    UINT32                           OutputWidth;        /* Valid pixels per line */
    UINT32                           OutputHeight;       /* Valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s          RecordingPixels;    /* Maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s          OpticalBlackPixels; /* User clamp area */
} B8N_IMX290_SENSOR_INFO_s;

#if 0
typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  LineLengthPck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} B8N_IMX290_FRAME_TIMING_s;
#endif
typedef struct {
    B8N_IMX290_B8_CTRL_s    B8Ctrl;
    AMBA_VIN_FRAME_RATE_s   FrameRate;          /* Framerate value of this sensor mode */
    FLOAT                   RowTime;            /* Calculated in B8N_IMX290_Init */
    FLOAT                   PixelRate;          /* Calculated in B8N_IMX290_Init */
} B8N_IMX290_MODE_INFO_s;

#if 0
typedef struct {
    B8N_IMX290_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in B8N_IMX290_Init */
} B8N_IMX290_MODE_INFO_s;
#endif

typedef struct {
    UINT32                  AgcCtrl;
    UINT32                  ShutterCtrl[3];     /* HDR ID: 0 - 2 */
} B8N_IMX290_AE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    B8N_IMX290_AE_INFO_s        CurrentAEInfo[6];
} B8N_IMX290_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_B8N_IMX290.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s B8N_IMX290DeviceInfo;
extern B8N_IMX290_SEQ_REG_s B8N_IMX290InitRegTable[B8N_IMX290_NUM_INIT_REG];
extern B8N_IMX290_REG_s B8N_IMX290RegTable[B8N_IMX290_NUM_MODE_REG];
extern const B8N_IMX290_SENSOR_INFO_s B8N_IMX290SensorInfo[B8N_IMX290_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s B8N_IMX290OutputInfo[B8N_IMX290_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s B8N_IMX290InputInfo[B8N_IMX290_NUM_MODE];
extern B8N_IMX290_MODE_INFO_s B8N_IMX290ModeInfoList[B8N_IMX290_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s B8N_IMX290HdrInfo[B8N_IMX290_NUM_MODE];

extern AMBA_SENSOR_OBJ_s AmbaSensor_B8N_IMX290Obj;

#endif /* AMBA_SENSOR_B8N_IMX290_H */
