/**
 *  @file AmbaSensor_MAX9295_96712_IMX224.h
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
 *  @details Control APIs of MAX9295_96712 plus SONY IMX224 CMOS sensor with MIPI interface
 *
 */

#ifndef MAX9295_96712_IMX224_H
#define MAX9295_96712_IMX224_H

#define MX01_IMX224_NUM_VIN0_SENSOR      4U
#define MX01_IMX224_NUM_VIN1_SENSOR      0U

#define MX01_IMX224_NUM_MAX_SENSOR_COUNT 4U

#define MX01_IMX224_I2C_SLAVE_ADDRESS       0x34U
#define MX01_IMX224_I2C_BC_SLAVE_ADDRESS    0x44U   //broadcast i2c addr

#define MX01_IMX224_SENSOR_I2C_MAX_SIZE  64U
/* SensorID definition */
#define MX01_IMX224_SENSOR_ID_CHAN_A   (0x00000010U) /* sensor on MAX9295 connecting with Channel-A of MAX96712 */
#define MX01_IMX224_SENSOR_ID_CHAN_B   (0x00000100U) /* sensor on MAX9295 connecting with Channel-B of MAX96712 */
#define MX01_IMX224_SENSOR_ID_CHAN_C   (0x00001000U) /* sensor on MAX9295 connecting with Channel-C of MAX96712 */
#define MX01_IMX224_SENSOR_ID_CHAN_D   (0x00010000U) /* sensor on MAX9295 connecting with Channel-D of MAX96712 */

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define MX01_IMX224_NUM_INIT_REG           37U
#define MX01_IMX224_NUM_READOUT_MODE_REG   64U

#define MX01_IMX224_STANDBY                0x3000
#define MX01_IMX224_REGHOLD                0x3001
#define MX01_IMX224_XMSTA                  0x3002
#define MX01_IMX224_SWRESET                0x3003
#define MX01_IMX224_ADBIT                  0x3005
#define MX01_IMX224_MODE                   0x3006
#define MX01_IMX224_WINMODE                0x3007
#define MX01_IMX224_FRSEL                  0x3009
#define MX01_IMX224_BLKLEVEL_LSB           0x300a
#define MX01_IMX224_BLKLEVEL_MSB           0x300b
#define MX01_IMX224_WDMODE                 0x300c
#define MX01_IMX224_GAIN_LSB               0x3014
#define MX01_IMX224_GAIN_MSB               0x3015
#define MX01_IMX224_VMAX_LSB               0x3018
#define MX01_IMX224_VMAX_MSB               0x3019
#define MX01_IMX224_VMAX_HSB               0x301a
#define MX01_IMX224_HMAX_LSB               0x301b
#define MX01_IMX224_HMAX_MSB               0x301c
#define MX01_IMX224_SHS1_LSB               0x3020
#define MX01_IMX224_SHS1_MSB               0x3021
#define MX01_IMX224_SHS1_HSB               0x3022
#define MX01_IMX224_SHS2_LSB               0x3023
#define MX01_IMX224_SHS2_MSB               0x3024
#define MX01_IMX224_SHS2_HSB               0x3025
#define MX01_IMX224_SHS3_LSB               0x3026
#define MX01_IMX224_SHS3_MSB               0x3027
#define MX01_IMX224_SHS3_HSB               0x3028
#define MX01_IMX224_RHS1_LSB               0x302c
#define MX01_IMX224_RHS1_MSB               0x302d
#define MX01_IMX224_RHS1_HSB               0x302e
#define MX01_IMX224_RHS2_LSB               0x302f
#define MX01_IMX224_RHS2_MSB               0x3030
#define MX01_IMX224_RHS2_HSB               0x3031
#define MX01_IMX224_DOL_PAT1               0x3043
#define MX01_IMX224_ODBIT                  0x3044
#define MX01_IMX224_INCKSEL1               0x305c
#define MX01_IMX224_INCKSEL2               0x305d
#define MX01_IMX224_INCKSEL3               0x305e
#define MX01_IMX224_INCKSEL4               0x305f
#define MX01_IMX224_DOL_PAT2               0x310a
#define MX01_IMX224_NULL0_SIZE             0x3354
#define MX01_IMX224_PIC_SIZE_LSB           0x3357
#define MX01_IMX224_PIC_SIZE_MSB           0x3358

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX01_IMX224_1280_960_30P           0U   /* 12bit A/D, 12bit OUT, non binning  */
#define MX01_IMX224_1280_960_60P           1U   /* 12bit A/D, 12bit OUT, non binning  */
#define MX01_IMX224_1280_960_120P          2U   /* 10bit A/D, 10bit OUT, non binning  */

#define MX01_IMX224_1280_960_25P           3U   /* 12bit A/D, 12bit OUT, non binning  */
#define MX01_IMX224_1280_960_50P           4U   /* 12bit A/D, 12bit OUT, non binning  */
#define MX01_IMX224_1280_960_100P          5U   /* 10bit A/D, 10bit OUT, non binning  */

#define MX01_IMX224_1280_720_30P_HDR       6U   /* 12bit A/D, 12bit OUT, non binning, window crop 720p, from Quad VGA  60fps */
#define MX01_IMX224_1280_720_60P_HDR       7U   /* 10bit A/D, 10bit OUT, non binning, window crop 720p, from Quad VGA 120fps */

#define MX01_IMX224_1280_720_25P_HDR       8U   /* 12bit A/D, 12bit OUT, non binning, window crop 720p, from Quad VGA  50fps */
#define MX01_IMX224_1280_720_50P_HDR       9U   /* 10bit A/D, 10bit OUT, non binning, window crop 720p, from Quad VGA 100fps */

#define MX01_IMX224_1280_960_30P_HDR       10U  /* 12bit A/D, 12bit OUT, non binning, 2-exp  */
#define MX01_IMX224_1280_960_25P_HDR       11U  /* 12bit A/D, 12bit OUT, non binning, 2-exp  */
#define MX01_IMX224_1280_720_30P_HDR_1     12U  /* 10bit A/D, 10bit OUT, non binning, 3-exp  */
#define MX01_IMX224_1280_720_60P_HDR_1     13U  /* 1ch only, 10bit A/D, 10bit OUT, non binning, window crop 720p, from Quad VGA 120fps */
#define MX01_IMX224_1280_960_60P_1         14U  /* 1ch only, 12bit A/D, 12bit OUT, non binning  */


#define MX01_IMX224_NUM_MODE               15U

typedef struct {
    UINT16 Addr;
    UINT8  Data;
} MX01_IMX224_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[MX01_IMX224_NUM_MODE];
} MX01_IMX224_MODE_REG_s;

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
} MX01_IMX224_SENSOR_INFO_s;

typedef struct {
    UINT32  VinRefClk;                                   /* reference clock of vin master sync */
    UINT32  NumTickPerXhs;                               /* XHS period (in input clock cycles) */
    UINT32  NumXhsPerH;                                  /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                                  /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                                  /* vertical operating period (in number of XVS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;                   /* Frame rate value of this sensor mode */
} MX01_IMX224_FRAME_TIMING_s;

typedef struct {
    MX01_IMX224_FRAME_TIMING_s       FrameTime;
    FLOAT                             RowTime;    /* Calculated in MX01_IMX224_Init */
    FLOAT                             PixelRate;  /* Calculated in MX01_IMX224_Init */
} MX01_IMX224_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s         Status;
    MX01_IMX224_FRAME_TIMING_s       FrameTime;
    UINT32                            CurrentShrCtrl[MX01_IMX224_NUM_MAX_SENSOR_COUNT][3];
    UINT32                            CurrentGainCtrl[MX01_IMX224_NUM_MAX_SENSOR_COUNT];
    UINT32                            CurrentCGCtrl[MX01_IMX224_NUM_MAX_SENSOR_COUNT];
} MX01_IMX224_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MX01_IMX224.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MX01_IMX224_DeviceInfo;
extern const MX01_IMX224_SENSOR_INFO_s MX01_IMX224_SensorInfo[MX01_IMX224_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s MX01_IMX224_InputInfo[MX01_IMX224_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX01_IMX224_OutputInfo[MX01_IMX224_NUM_MODE];
extern MX01_IMX224_MODE_INFO_s MX01_IMX224_ModeInfoList[MX01_IMX224_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX01_IMX224_HdrInfo[MX01_IMX224_NUM_MODE];

extern MX01_IMX224_REG_s MX01_IMX224_InitRegTable[MX01_IMX224_NUM_INIT_REG];
extern MX01_IMX224_MODE_REG_s MX01_IMX224_ModeRegTable[MX01_IMX224_NUM_READOUT_MODE_REG];


extern AMBA_SENSOR_OBJ_s AmbaSensor_MX01_IMX224Obj;

#endif /* MAX9295_9296_IMX224_H */
