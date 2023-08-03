/**
 *  @file AmbaSensor_MAX9295_9296_IMX490.h
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
 *  @details Control APIs of MAXIM MAX9295_9296 plus SONY IMX490 CMOS sensor with MIPI interface
 *
 */

#ifndef MAX9295_9296_IMX490_H
#define MAX9295_9296_IMX490_H

#define MX00_IMX490_NUM_MAX_SENSOR_COUNT 2U


//#define MX00_IMX490_I2C_SLAVE_ADDRESS    0x34U//for sensor board
#define MX00_IMX490_I2C_SLAVE_ADDRESS    0x3AU//for TFF module
#define MX00_IMX490_SENSOR_I2C_MAX_SIZE  64U

#define MX00_IMX490_MAX_AGAIN      28.5f /* in dB (TBD) */
#define MX00_IMX490_MAX_DGAIN      42.0f
#define MX00_IMX490_MAX_TOTAL_GAIN (MX00_IMX490_MAX_AGAIN + MX00_IMX490_MAX_DGAIN)

/* SensorID definition */
#define MX00_IMX490_SENSOR_ID_CHAN_A   (0x00000010U) /* sensor on MAX9295 connecting with Channel-A of MAX9296 */
#define MX00_IMX490_SENSOR_ID_CHAN_B   (0x00000100U) /* sensor on MAX9295 connecting with Channel-B of MAX9296 */

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/

#define MX00_IMX490_STANDBY             0x3C00U
#define MX00_IMX490_SHS1                0x36C0U
#define MX00_IMX490_SHS2                0x36C4U

#define MX00_IMX490_AGAIN_SP1H          0x36C8U
#define MX00_IMX490_AGAIN_SP1L          0x36CAU
#define MX00_IMX490_AGAIN_SP2H          0x36CCU
#define MX00_IMX490_AGAIN_SP2L          0x36CEU

#define MX00_IMX490_DGAIN_SP1H          0x36D0U
#define MX00_IMX490_DGAIN_SP1L          0x36D2U

#define MX00_IMX490_WBGAIN_R            0x3730U
#define MX00_IMX490_WBGAIN_GR           0x3732U
#define MX00_IMX490_WBGAIN_GB           0x3734U
#define MX00_IMX490_WBGAIN_B            0x3736U

#define MX00_IMX490_FMAX                0x36D8U

#define MX00_IMX490_NUM_MODE_NML_REG          923U
#define MX00_IMX490_NUM_MODE_REG_HDR30P         1932U
#define MX00_IMX490_NUM_MODE_REG_HDR60P      1330U
#define MX00_IMX490_NUM_REG                  1534U
#define MX00_IMX490_NUM_CALIB                 1561U

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX_I490_2880_1860_A30P                0U
#define MX00_IMX490_2880_1860_A30P_HDR        1U
#define MX00_IMX490_2880_1192_A60P_HDR        2U

#define MX_I490_2880_1860_A30P_HDR_53         1U
#define MX_I490_2880_1192_A60P_HDR_53         2U
#define MX_I490_2880_1860_A30P_HDR_36         3U
#define MX_I490_2880_1192_A60P_HDR_36         4U

#define MX00_IMX490_NUM_MODE                5U


typedef struct {
    UINT16  Addr;
    UINT8  Data;
} MX00_IMX490_SEQ_REG_s;

typedef struct {
    UINT16 Addr;
    UINT8  Data[MX00_IMX490_NUM_MODE];
} MX00_IMX490_REG_s;


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
} MX00_IMX490_SENSOR_INFO_s;


typedef struct {
    UINT32  InputClk;                                  /* Sensor side input clock frequency */
    UINT32  HMAX;                                      /* HMAX */
    UINT32  VMAX;                                      /* VMAX */
    AMBA_VIN_FRAME_RATE_s  FrameRate;                  /* Frame rate value of this sensor mode */
} MX00_IMX490_FRAME_TIMING_s;

typedef struct {
    MX00_IMX490_FRAME_TIMING_s       FrameTime;
    FLOAT                             RowTime;    /* Calculated in MX00_IMX490_Init */
    FLOAT                             PixelRate;  /* Calculated in MX00_IMX490_Init */
} MX00_IMX490_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s  Status;
    UINT32                     CurrentAgcCtrl[MX00_IMX490_NUM_MAX_SENSOR_COUNT][4];      /* [0]: SP1H, [1]: SP1L, [2]: SP2H, [3]: SP2L */
    UINT32                     CurrentDgcCtrl[MX00_IMX490_NUM_MAX_SENSOR_COUNT][4];      /* [0]: SP1H, [1]: SP1L, [2]: SP2H, [3]: SP2L */
    AMBA_SENSOR_WB_CTRL_s      CurrentWbCtrl[MX00_IMX490_NUM_MAX_SENSOR_COUNT][4];       /* [0]: SP1H, [1]: SP1L, [2]: SP2H, [3]: SP2L */
    UINT32                     CurrentShutterCtrl[MX00_IMX490_NUM_MAX_SENSOR_COUNT][4];  /* [0]: SP1H, [1]: SP1L, [2]: SP2H, [3]: SP2L */
} MX00_IMX490_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MX00_IMX490.c
\*-----------------------------------------------------------------------------------------------*/
extern const MX00_IMX490_SEQ_REG_s MX00_IMX490_RegTable_NML[MX00_IMX490_NUM_MODE_NML_REG];
extern const MX00_IMX490_SEQ_REG_s MX00_IMX490_RegTable_HDR30P[MX00_IMX490_NUM_MODE_REG_HDR30P];
extern const MX00_IMX490_SEQ_REG_s MX00_IMX490_RegTable_HDR60P[MX00_IMX490_NUM_MODE_REG_HDR60P];

extern const MX00_IMX490_REG_s MX00_IMX490_RegTable[MX00_IMX490_NUM_REG];
extern const MX00_IMX490_REG_s MX00_IMX490_CalibTable_53[MX00_IMX490_NUM_CALIB];
extern const MX00_IMX490_REG_s MX00_IMX490_CalibTable_36[MX00_IMX490_NUM_CALIB];
extern const AMBA_SENSOR_DEVICE_INFO_s MX00_IMX490_DeviceInfo;
extern const MX00_IMX490_SENSOR_INFO_s MX00_IMX490_SensorInfo[MX00_IMX490_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s MX00_IMX490_InputInfo[MX00_IMX490_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX00_IMX490_OutputInfo[MX00_IMX490_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX00_IMX490_HdrInfo[MX00_IMX490_NUM_MODE];
extern MX00_IMX490_MODE_INFO_s MX00_IMX490ModeInfoList[MX00_IMX490_NUM_MODE];
extern AMBA_SENSOR_OBJ_s AmbaSensor_MX00_IMX490Obj;

#endif /* MAX9295_9296_IMX490_H */
