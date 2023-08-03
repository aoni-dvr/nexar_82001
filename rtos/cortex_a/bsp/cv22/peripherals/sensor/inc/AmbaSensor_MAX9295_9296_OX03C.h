/**
 *  @file AmbaSensor_MAX9295_9296_OX03C.h
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
 *  @details Control APIs of MAX9295_9296 plus OV OX03C CMOS sensor with MIPI interface
 *
 */

#ifndef MX00_OX03C_H
#define MX00_OX03C_H

#ifdef CONFIG_MX00_OX03C_IN_SLAVE_MODE
#define MX00_OX03C_IN_SLAVE_MODE
#endif

#define MX00_OX03C_SENSOR_I2C_MAX_SIZE    64U

#define MX00_OX03C_NUM_VIN_CHANNEL      2U

#define MX00_OX03C_NUM_VIN0_SENSOR      1U
#define MX00_OX03C_NUM_VIN1_SENSOR      0U

#define MX00_OX03C_NUM_MAX_SENSOR_COUNT 2U

#define MX00_OX03C_I2C_SLAVE_ADDRESS      0x6CU

/* SensorID definition */
#define MX00_OX03C_SENSOR_ID_CHAN_A   (0x00000010U) /* sensor on MAX9295 connecting with Channel-A of MAX9296 */
#define MX00_OX03C_SENSOR_ID_CHAN_B   (0x00000100U) /* sensor on MAX9295 connecting with Channel-B of MAX9296 */

//#define MX00_OX03C_HCG_LCG_Conversion_Ratio    7.32f
//#define MX00_OX03C_HCG_LCG_Ratio_Limit    1.1428f//1.59
#define MX00_OX03C_WBGain_Max           15.996f
#define MX00_OX03C_WBGain_Min           1.0f
//#define MX00_OX03C_LCG_GAIN_MIN          1.828125f
//#define MX00_OX03C_LCG_DGAIN_MIN         1.125f
//#define MX00_OX03C_LCG_AGAIN_MIN         1.625f
#define MX00_OX03C_SPD_DGAIN_MIN        1.0f
#define MX00_OX03C_SPD_GAIN_MIN         4.25f
#define MX00_OX03C_LPD_GAIN_MIN         1.0f
#define MX00_OX03C_CG_GAIN_RATIO        7.0f
#define MX00_OX03C_PD_GAIN_RATIO        108.0f
/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define MX00_OX03C_NUM_READOUT_HDR4_REG 1587U
#define MX00_OX03C_NUM_READOUT_HDR4_NoVC_REG 1410U
#define MX00_OX03C_NUM_READOUT_960M_HDR4_NoVC_REG 1522U

#define MX00_OX03C_WBGAIN_HCG_B     0x5280U
#define MX00_OX03C_WBGAIN_LCG_B     0x5480U
#define MX00_OX03C_WBGAIN_SPD_B     0x5680U
#define MX00_OX03C_WBGAIN_VS_B      0x5880U
#define MX00_OX03C_DCG_SHR_REG      0x3501U
#define MX00_OX03C_SPD_SHR_REG      0x3541U
#define MX00_OX03C_VS_SHR_REG       0x35C1U
#define MX00_OX03C_HCG_AGAIN        0x3508U
#define MX00_OX03C_HCG_DGAIN        0x350aU
#define MX00_OX03C_SPD_AGAIN        0x3548U
#define MX00_OX03C_SPD_DGAIN        0x354aU
#define MX00_OX03C_LCG_AGAIN        0x3588U
#define MX00_OX03C_LCG_DGAIN        0x358aU
#define MX00_OX03C_VS_AGAIN         0x35C8U
#define MX00_OX03C_VS_DGAIN         0x35CAU
#define MX00_OX03C_STANDBY          0x0100U
#define MX00_OX03C_SW_RST           0x0107U
/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX00_OX03C_1920_1280_SHDR4          0U
#define MX00_OX03C_1920_1280_SHDR4_P30_NoVC 1U
#define MX00_OX03C_1920_1280_SHDR4_P60_NoVC 2U
#define MX00_OX03C_1920_1080_SHDR4_P30_NoVC 3U
#define MX00_OX03C_1920_712_SHDR4_P60_NoVC  4U
#define MX00_OX03C_NUM_MODE                 5U



typedef struct {
    UINT16  Addr;
    UINT8   Data;
} MX00_OX03C_MODE_REG_s;


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
} MX00_OX03C_SENSOR_INFO_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} MX00_OX03C_FRAME_TIMING_s;

typedef struct {
    UINT32                            OperationMode;
    MX00_OX03C_FRAME_TIMING_s              FrameTime;
    FLOAT                             RowTime;    /* Calculated in MX00_OX03C_Init */
    FLOAT                             PixelRate;  /* Calculated in MX00_OX03C_Init */
} MX00_OX03C_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s                Status;
    MX00_OX03C_FRAME_TIMING_s                     FrameTime;
    AMBA_SENSOR_WB_CTRL_s                    CurrentWbCtrl[MX00_OX03C_NUM_MAX_SENSOR_COUNT][4];
    UINT32                                   CurrentShrCtrl[MX00_OX03C_NUM_MAX_SENSOR_COUNT][4];
    UINT32                                   CurrentAGCtrl[MX00_OX03C_NUM_MAX_SENSOR_COUNT][4];
    UINT32                                   CurrentDGCtrl[MX00_OX03C_NUM_MAX_SENSOR_COUNT][4];
    UINT32                                   CurrentGainType[MX00_OX03C_NUM_MAX_SENSOR_COUNT][4];
} MX00_OX03C_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MX00_OX03CTable.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MX00_OX03C_DeviceInfo;
extern const MX00_OX03C_SENSOR_INFO_s MX00_OX03C_SensorInfo[MX00_OX03C_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s MX00_OX03C_InputInfo[MX00_OX03C_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX00_OX03C_OutputInfo[MX00_OX03C_NUM_MODE];
extern MX00_OX03C_MODE_INFO_s MX00_OX03C_ModeInfoList[MX00_OX03C_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX00_OX03C_HdrInfo[MX00_OX03C_NUM_MODE];

extern MX00_OX03C_MODE_REG_s MX00_OX03C_HDR4Table[MX00_OX03C_NUM_READOUT_HDR4_REG];
extern MX00_OX03C_MODE_REG_s MX00_OX03C_HDR4NoVCTable[MX00_OX03C_NUM_READOUT_HDR4_NoVC_REG];
extern MX00_OX03C_MODE_REG_s MX00_OX03C_960M_HDR4NoVCTable[MX00_OX03C_NUM_READOUT_960M_HDR4_NoVC_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_MX00_OX03CObj;

#endif /* MX00_OX03C_H */
