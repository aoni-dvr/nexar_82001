/**
 *  @file AmbaSensor_MAX9295_9296_AR0323.h
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
 *  @details Control APIs of MAXIM 9295/9296 serdes plus OnSemi AR0323 CMOS sensor with MIPI interface
 *
 */

#ifndef MAX9295_9296_AR0323_H
#define MAX9295_9296_AR0323_H

#define MX00_AR0323_I2C_SLAVE_ADDRESS        0x20U

#define MX00_AR0323_NUM_VIN_CHANNEL             4U
#define MX00_AR0323_NUM_MAX_SENSOR_COUNT        2U

#define MX00_AR0323_SENSOR_ID_CHAN_A   0x00000010U
#define MX00_AR0323_SENSOR_ID_CHAN_B   0x00000100U

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define AR0323_NUM_PLL_REG                 24U
#define AR0323_NUM_DESIGN_RECOM_REG        26U
#define AR0323_NUM_DESIGN_BOOSTER_REG      17U
#define AR0323_NUM_OVORIGIN_REG            66U
#define AR0323_NUM_PIXEL_RECOM_REG         36U
#define AR0323_NUM_PIXEL_RECOM_LIN_REG     13U
#define AR0323_NUM_SEQ_HIDY_REG          1280U
#define AR0323_NUM_SEQ_HIDY_LIN_REG      1281U
#define AR0323_NUM_DATA_PEDESTAL_RT_REG     3U
#define AR0323_NUM_TEMP_INIT_REG            9U
#define AR0323_NUM_TEMP_INIT_LIN_REG        6U
#define AR0323_NUM_SEQ_REG                  5U
#define AR0323_NUM_SEQ_LIN_REG              5U
#define AR0323_NUM_MEC_REG                 12U
#define AR0323_NUM_LUT24T16                19U
#define AR0323_NUM_SEAGAIN_REG             13U
#define AR0323_NUM_WORKING_MODE_REG        17U

#define MX00_AR0323_NUM_AGC_STEP           80U
#define MX00_AR0323_NUM_HDR_SE_AGC_STEP     1U


#define T1_DELAY_BUFFER                    72U
#define T2_SH_ADVANCE                       2U

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX00_AR0323_2880_1080_A60P          0U //Linear 12-bit 60fps
#define MX00_AR0323_2880_1080_A30P_HDR      1U //SE_T1+T2 24-bit 30fps compand to 16-bit
#define MX00_AR0323_NUM_MODE                2U

/*-----------------------------------------------------------------------------------------------*\
 *  Conversion Gain Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX00_AR0323_LOW_CONVERSION_GAIN     0U
#define MX00_AR0323_HIGH_CONVERSION_GAIN    1U

typedef struct {
    UINT16                      Addr;
    UINT16                      Data[MX00_AR0323_NUM_MODE];
} MX00_AR0323_REG_s;

typedef struct {
    UINT16                      Addr;
    UINT16                      Data;
} MX00_AR0323_SEQ_REG_s;

typedef struct {
    FLOAT                       Factor;
    UINT16                      CoarseData;
    UINT16                      FineData;
    UINT32                      ConvGain;
} MX00_AR0323_AGC_REG_s;

typedef struct {
    FLOAT                       Factor;
    UINT16                      CoarseData; //R0x3366
    UINT16                      FineData;   //R0x336A
    UINT32                      ConvGain;
} MX00_AR0323_SE_AGC_REG_s;

typedef struct {
    UINT32                      InputClk;           /* Sensor side input clock frequency */
    UINT32                      Linelengthpck;      /* Number of pixel clock cycles per line of frame */
    UINT32                      FrameLengthLines;   /* Number of lines per frame */
    AMBA_VIN_FRAME_RATE_s       FrameRate;          /* Framerate value of this sensor mode */
} MX00_AR0323_FRAME_TIMING_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    MX00_AR0323_FRAME_TIMING_s  FrameTime;
    UINT32                      CurrentShtCtrl[MX00_AR0323_NUM_MAX_SENSOR_COUNT][2]; /* Shutter setting, HDR id: 0 = L, 1 = S */
    UINT32                      CurrentAgc[MX00_AR0323_NUM_MAX_SENSOR_COUNT][2];     /* AGC table index, HDR id: 0 = L, 1 = S */
    UINT32                      CurrentDgc[MX00_AR0323_NUM_MAX_SENSOR_COUNT][2];     /* DGC setting, HDR id: 0 = L, 1 = S */
    AMBA_SENSOR_WB_CTRL_s       CurrentWbCtrl[MX00_AR0323_NUM_MAX_SENSOR_COUNT][2];  /* WBGainCtrl setting, HDR id: 0 = L, 1 = S */
} MX00_AR0323_CTRL_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s       FrameRate;
    FLOAT                       RowTime;            /* Calculated in MX00_AR0323_Init */
    FLOAT                       PixelRate;          /* Calculated in MX00_AR0323_Init */
} MX00_AR0323_MODE_INFO_s;

typedef struct {
    UINT32                      InputClk;           /* Sensor side input clock frequency */
    UINT32                      DataRate;           /* Output bit/pixel clock frequency from image sensor */
    UINT8                       NumDataLanes;       /* Active data channels */
    UINT8                       NumDataBits;        /* Pixel data bit depth */
    UINT32                      LineLengthPck;      /* number of pixel clock cycles per line of frame */
    UINT32                      FrameLengthLines;   /* number of lines per frame */
    UINT32                      OutputWidth;        /* Valid pixels per line */
    UINT32                      OutputHeight;       /* Valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s     RecordingPixels;    /* Maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s     OpticalBlackPixels; /* User clamp area */
} MX00_AR0323_SENSOR_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MAX9295_9296_AR0323Table.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MX00_AR0323DeviceInfo;
extern const MX00_AR0323_SENSOR_INFO_s MX00_AR0323SensorInfo[MX00_AR0323_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s MX00_AR0323InputInfo[MX00_AR0323_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX00_AR0323OutputInfo[MX00_AR0323_NUM_MODE];
extern MX00_AR0323_MODE_INFO_s MX00_AR0323ModeInfoList[MX00_AR0323_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX00_AR0323HdrInfo[MX00_AR0323_NUM_MODE];

// Gain Reg Table
extern MX00_AR0323_AGC_REG_s MX00_AR0323AgcRegTable[MX00_AR0323_NUM_AGC_STEP];
extern MX00_AR0323_SE_AGC_REG_s MX00_AR0323HDRSEAgcRegTable[MX00_AR0323_NUM_HDR_SE_AGC_STEP];

//PLL
extern MX00_AR0323_REG_s AR0323PLLRegTable[AR0323_NUM_PLL_REG];
//AR0323_Design_recommended_settings_OV_HDR_V1
extern MX00_AR0323_SEQ_REG_s AR0323DesignRecomRegTable[AR0323_NUM_DESIGN_RECOM_REG];
//AR0323_Design_Booster_Settings
extern MX00_AR0323_SEQ_REG_s AR0323DesignBoosterRegTable[AR0323_NUM_DESIGN_BOOSTER_REG];
//AR0323_OV_Origin settings_V1
extern MX00_AR0323_SEQ_REG_s AR0323OVOriginRegTable[AR0323_NUM_OVORIGIN_REG];
//AR0323_Pixel_Recommended_Settings_SE_v5
extern MX00_AR0323_SEQ_REG_s AR0323PixelRecomRegTable[AR0323_NUM_PIXEL_RECOM_REG];
//AR0323_Pixel_Recommended_Settings_Linear_v2
extern MX00_AR0323_SEQ_REG_s AR0323PixelRecomLinRegTable[AR0323_NUM_PIXEL_RECOM_LIN_REG];
//sequence_hidy_ar0323_REV1_OV_R5_RSON
extern MX00_AR0323_SEQ_REG_s AR0323SeqHidyRegTable[AR0323_NUM_SEQ_HIDY_REG];
//AR0323_Sequence_HiDy_REV1_OV_R4(Linear)
extern MX00_AR0323_SEQ_REG_s AR0323SeqHidyLinRegTable[AR0323_NUM_SEQ_HIDY_LIN_REG];
//Data_Pedestal_RT
extern MX00_AR0323_SEQ_REG_s AR0323DataPedestalRTRegTable[AR0323_NUM_DATA_PEDESTAL_RT_REG];
//Tempsensor_init
extern MX00_AR0323_SEQ_REG_s AR0323TempInitRegTable[AR0323_NUM_TEMP_INIT_REG];
//Tempsensor_init_linear
extern MX00_AR0323_SEQ_REG_s AR0323TempInitLinRegTable[AR0323_NUM_TEMP_INIT_LIN_REG];
//Reg Sequence
extern MX00_AR0323_SEQ_REG_s AR0323SeqRegTable[AR0323_NUM_SEQ_REG];
//Reg Sequence Linear
extern MX00_AR0323_SEQ_REG_s AR0323SeqLinRegTable[AR0323_NUM_SEQ_LIN_REG];
//MEC_Settings_OV_HDR
extern MX00_AR0323_SEQ_REG_s AR0323MECRegTable[AR0323_NUM_MEC_REG];
//LUT_24_to_16
extern MX00_AR0323_SEQ_REG_s AR0323Lut24to16Table[AR0323_NUM_LUT24T16];
//SE_AGain 1x
extern MX00_AR0323_SEQ_REG_s AR0323SEAGainRegTable[AR0323_NUM_SEAGAIN_REG];
//WORKING MODE
extern MX00_AR0323_REG_s AR0323WorkingModeRegTable[AR0323_NUM_WORKING_MODE_REG];


extern AMBA_SENSOR_OBJ_s AmbaSensor_MX00_AR0323Obj;

#endif /* MAX9295_9296_AR0323_H */

