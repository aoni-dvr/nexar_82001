/**
 *  @file AmbaSensor_AR0239_RGBIR.h
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
 *  @details Control APIs of SONY AR0239 CMOS sensor with MIPI interface
 *
 */

#ifndef AR0239_H
#define AR0239_H

#define AR0239_IN_SLAVE_MODE
#define AR0239_SENSOR_I2C_MAX_SIZE    64U

#define AR0239_NUM_VIN_CHANNEL        AMBA_NUM_VIN_CHANNEL

#define AR0239_NUM_MAX_SENSOR_COUNT   2U

#define AR0239_I2C_SLAVE_ADDRESS      0x20U

/* SensorID definition */
#define AR0239_SENSOR_ID_CHAN_A       (0x00000010U)     /* sensor on MAX9295 connecting with Channel-A of MAX9296 */
#define AR0239_SENSOR_ID_CHAN_B       (0x00000100U)     /* sensor on MAX9295 connecting with Channel-B of MAX9296 */

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define AR0239_NUM_PLL_REG                     17U
#define AR0239_NUM_RECOM_ES_REG                42U
#define AR0239_NUM_LINEAR_SEQ_REG             154U
#define AR0239_NUM_HDR_SEQ_REG                154U
#define AR0239_NUM_HDR_CFPN_REG                23U
#define AR0239_NUM_READOUT_MODE_REG            11U
#define AR0239_NUM_CFPN_Impr_REG          23U
#define AR0239_NUM_HDR_Seq_REG           154U
#define AR0239_NUM_Recom_Set_REG          43U

#define AR0239_NUM_AGC_STEP                    86U

#define AR0239_OP_MODE_LINEAR                   0U
#define AR0239_OP_MODE_HDR                      1U
#define AR0239_NUM_OP_MODE                      2U

#define AR0239_GAIN_CONVERT_LOW                 0U
#define AR0239_GAIN_CONVERT_HIGH                1U


/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define AR0239_1920_1080_A30P          0U
#define AR0239_1920_1080_A30P_HDR      1U

#define AR0239_NUM_MODE                2U

typedef struct {
    UINT16 Addr;
    UINT16 Data;
} AR0239_REG_s;

typedef struct {
    UINT16  Addr;
    UINT16  Data[AR0239_NUM_MODE];
} AR0239_MODE_REG_s;

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
} AR0239_SENSOR_INFO_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} AR0239_FRAME_TIMING_s;

typedef struct {
    UINT32                            OperationMode;
    AR0239_FRAME_TIMING_s        FrameTime;
    FLOAT                             RowTime;    /* Calculated in AR0239_Init */
    FLOAT                             PixelRate;  /* Calculated in AR0239_Init */
} AR0239_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s         Status;
    AR0239_FRAME_TIMING_s        FrameTime;
    UINT32                            CurrentShrCtrl[AR0239_NUM_MAX_SENSOR_COUNT][2];
    UINT32                            CurrentAgcCtrl[AR0239_NUM_MAX_SENSOR_COUNT][3];
    UINT32                            CurrentDgcCtrl[AR0239_NUM_MAX_SENSOR_COUNT][3];
} AR0239_CTRL_s;

typedef struct {
    FLOAT   Factor;
    UINT16   HData;
    UINT16   LData;
    UINT32  GainConv;
} AR0239_AGC_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_AR0239Table.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s AR0239_DeviceInfo;
extern const AR0239_SENSOR_INFO_s AR0239_SensorInfo[AR0239_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s AR0239_InputInfo[AR0239_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s AR0239_OutputInfo[AR0239_NUM_MODE];
extern AR0239_MODE_INFO_s AR0239_ModeInfoList[AR0239_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s AR0239_HdrInfo[AR0239_NUM_MODE];

extern AR0239_REG_s AR0239_PllRegTable[AR0239_NUM_PLL_REG];
extern AR0239_REG_s AR0239_RecomESRegTable[AR0239_NUM_RECOM_ES_REG];
extern AR0239_REG_s AR0239_LinearSeqRegTable[AR0239_NUM_LINEAR_SEQ_REG];
extern AR0239_REG_s AR0239_CFPNImpTable[AR0239_NUM_CFPN_Impr_REG];
extern AR0239_REG_s AR0239_HDRSeqTable[AR0239_NUM_HDR_Seq_REG];
extern AR0239_REG_s AR0239_RecomSettingESTable[AR0239_NUM_Recom_Set_REG];

extern AR0239_MODE_REG_s AR0239_ModeRegTable[AR0239_NUM_READOUT_MODE_REG];
extern AR0239_AGC_REG_s AR0239_AgcRegTable[AR0239_NUM_AGC_STEP];

extern AMBA_SENSOR_OBJ_s AmbaSensor_AR0239Obj;

#endif /* AR0239_H */
