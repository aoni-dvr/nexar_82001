/**
 *  @file AmbaSensor_MAX9295_9296_AR0233.h
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
 *  @details Control APIs of MAXIM 9295/9296 serdes plus OnSemi AR0233 CMOS sensor with MIPI interface
 *
 */

#ifndef MAX9295_9296_AR0233_H
#define MAX9295_9296_AR0233_H

#define MX00_AR0233_I2C_SLAVE_ADDRESS        0x20U

#define MX00_AR0233_NUM_MAX_SENSOR_COUNT        2U

#define MX00_AR0233_SENSOR_ID_CHAN_A   0x00000010U
#define MX00_AR0233_SENSOR_ID_CHAN_B   0x00000100U

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
//SE+T2, Mode6, 1920x1080@30P/60P, PLL5
#define AR0233_R3M6_NUM_PLL_REG                      24U//25U
#define AR0233_R3M6_NUM_DESIGN_RECOM_REG             39U//41U
#define AR0233_R3M6_NUM_SE_T2_PHASE_REG              16U //20200703
#define AR0233_R3M6_NUM_SE_MODE_REG                  7U//6U
#define AR0233_R3M6_NUM_SE_HDR_REG                   13U//14U
#define AR0233_R3M6_NUM_PIXEL_CHAR_RECOM_REG         24U//27U
#define AR0233_R3M6_NUM_SEQ_HIDY_REG                 1024U
#define AR0233_R3M6_NUM_WORKING_MODE_REG             16U
#define AR0233_R3M6_NUM_SEQ_REG                      6U
#define AR0233_R3M6_NUM_LUT24T16                     19U
#define AR0233_R3M6_NUM_SE_AGAIN_1X                  6U
#define AR0233_R3M6_NUM_TEMP_INIT_REG                6U
////////////////////////////////////////////////////////////
//Linear mode, 2048x1280@30fps, PLL1
#define AR0233_NUM_DESIGN_RECOM_REG            29U
#define AR0233_NUM_RECOM_LFMHDR_REG            13U
#define AR0233_NUM_SEQ_HIDY_REG                1024U
#define AR0233_NUM_PLL_REG                     24U //25U
#define AR0233_NUM_WORKING_MODE_REG            16U
///////////////////////////////////////////////////////////

#define MX00_AR0233_NUM_AGC_STEP               51U
//#define MX00_AR0233_NUM_HDR_AGC_STEP           97U
#define MX00_AR0233_NUM_HDR_SE_AGC_STEP         1U//4U

#define T1_DELAY_BUFFER_P60                   140U/*when R0x3C08[8]=1,T1_DELAY_BUFFER=140*/
#define T1_DELAY_BUFFER_P50                   140U/*when R0x3C08[8]=1,T1_DELAY_BUFFER=140*/
#define T1_DELAY_BUFFER                       140U/*when R0x3C08[8]=1,T1_DELAY_BUFFER=140*/

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX00_AR0233_1920_1080_A30P_MODE6         0U
#define MX00_AR0233_1920_1080_A60P_MODE6         1U
#define MX00_AR0233_2048_1280_A30P               2U
#define MX00_AR0233_1920_1080_A27P6_M6           3U
#define MX00_AR0233_1920_1080_A27P6_M6_1         4U
#define MX00_AR0233_1920_1280_A50P_MODE6         5U
#define MX00_AR0233_NUM_MODE                     6U//3U

/*-----------------------------------------------------------------------------------------------*\
 *  Conversion Gain Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX00_AR0233_LOW_CONVERSION_GAIN         0U
#define MX00_AR0233_HIGH_CONVERSION_GAIN        1U

typedef struct {
    UINT16                      Addr;
    UINT16                      Data[MX00_AR0233_NUM_MODE];
} MX00_AR0233_REG_s;

typedef struct {
    UINT16                      Addr;
    UINT16                      Data;
} MX00_AR0233_SEQ_REG_s;

typedef struct {
    FLOAT                       Factor;
    UINT16                      CoarseData;
    UINT16                      FineData;
    UINT32                      ConvGain;
} MX00_AR0233_AGC_REG_s;

typedef struct {
    FLOAT                       Factor;
    UINT16                      CoarseData; //R0x3366
    UINT16                      FineData;   //R0x336A
    UINT16                      Data562E;
    UINT16                      Data3E14;
    UINT16                      Data562A;
    UINT32                      ConvGain;
} MX00_AR0233_SE_AGC_REG_s;
typedef struct {
    UINT32                      InputClk;           /* Sensor side input clock frequency */
    UINT32                      Linelengthpck;      /* Number of pixel clock cycles per line of frame */
    UINT32                      FrameLengthLines;   /* Number of lines per frame */
    AMBA_VIN_FRAME_RATE_s       FrameRate;          /* Framerate value of this sensor mode */
} MX00_AR0233_FRAME_TIMING_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    MX00_AR0233_FRAME_TIMING_s  FrameTime;
    UINT32                      CurrentShtCtrl[MX00_AR0233_NUM_MAX_SENSOR_COUNT][2]; /* Shutter setting, HDR id: 0 = L, 1 = S */
    UINT32                      CurrentAgc[MX00_AR0233_NUM_MAX_SENSOR_COUNT][2];     /* AGC table index, HDR id: 0 = L, 1 = S */
    UINT32                      CurrentDgc[MX00_AR0233_NUM_MAX_SENSOR_COUNT][2];     /* DGC setting, HDR id: 0 = L, 1 = S */
    AMBA_SENSOR_WB_CTRL_s       CurrentWbCtrl[MX00_AR0233_NUM_MAX_SENSOR_COUNT][2];  /* WBGainCtrl setting, HDR id: 0 = L, 1 = S */
} MX00_AR0233_CTRL_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s       FrameRate;
    FLOAT                       RowTime;            /* Calculated in MX00_AR0233_Init */
    FLOAT                       PixelRate;          /* Calculated in MX00_AR0233_Init */
} MX00_AR0233_MODE_INFO_s;

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
} MX00_AR0233_SENSOR_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MAX9295_9296_AR0233Table.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MX00_AR0233DeviceInfo;
extern const MX00_AR0233_SENSOR_INFO_s MX00_AR0233SensorInfo[MX00_AR0233_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s MX00_AR0233InputInfo[MX00_AR0233_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX00_AR0233OutputInfo[MX00_AR0233_NUM_MODE];
extern MX00_AR0233_MODE_INFO_s MX00_AR0233ModeInfoList[MX00_AR0233_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX00_AR0233HdrInfo[MX00_AR0233_NUM_MODE];

//PLL5
extern MX00_AR0233_REG_s AR0233R3M6PLLRegTable[AR0233_R3M6_NUM_PLL_REG];
//SE_Design_Recommended_REV2_V1
extern MX00_AR0233_SEQ_REG_s AR0233R3M6DesignRecomRegTable[AR0233_R3M6_NUM_DESIGN_RECOM_REG];
//SE_plus_T2_Phase_settings
extern MX00_AR0233_SEQ_REG_s AR0233R3M6SET2PhaseRegTable[AR0233_R3M6_NUM_SE_T2_PHASE_REG];
//SE_mode_REV2_V1
extern MX00_AR0233_SEQ_REG_s AR0233R3M6SEModeRegTable[AR0233_R3M6_NUM_SE_MODE_REG];
//SE_HDR_REV2_V1
extern MX00_AR0233_SEQ_REG_s AR0233R3M6SEHdrRegTable[AR0233_R3M6_NUM_SE_HDR_REG];
//SE_Pixel_char_Recommended_REV2_V3
extern MX00_AR0233_SEQ_REG_s AR0233R3M6PixelCharReRegTable[AR0233_R3M6_NUM_PIXEL_CHAR_RECOM_REG];
//sequence_hidy_ar0233_REV2_SE_V11
extern MX00_AR0233_SEQ_REG_s AR0233R3M6SeqHidyRegTable[AR0233_R3M6_NUM_SEQ_HIDY_REG];
//WORKING MODE
extern MX00_AR0233_REG_s AR0233R3M6WorkingModeRegTable[AR0233_R3M6_NUM_WORKING_MODE_REG];
//Reg Sequence
extern MX00_AR0233_SEQ_REG_s AR0233R3M6SeqRegTable[AR0233_R3M6_NUM_SEQ_REG];
//LUT_24_to_16
extern MX00_AR0233_SEQ_REG_s AR0233R3M6Lut24to16Table[AR0233_R3M6_NUM_LUT24T16];
//SE_AGain 1x
extern MX00_AR0233_SEQ_REG_s AR0233R3M6SEAGin1XTable[AR0233_R3M6_NUM_SE_AGAIN_1X];
//Tempsensor_init
extern MX00_AR0233_SEQ_REG_s AR0233R3M6TempInitRegTable[AR0233_R3M6_NUM_TEMP_INIT_REG];


//Linear mode, 12bit
//Design_recommended_settings_REV2_V10
extern MX00_AR0233_SEQ_REG_s AR0233R3LinDesignRecomRegTable[AR0233_NUM_DESIGN_RECOM_REG];
//pixel_recommended_settings_REV2_LFMHDR_V2
extern MX00_AR0233_SEQ_REG_s AR0233LinRecomLFMHDRTable[AR0233_NUM_RECOM_LFMHDR_REG];
//sequence_hidy_ar0233_REV2_V18
extern MX00_AR0233_SEQ_REG_s AR0233LinSeqHidyRegTable[AR0233_NUM_SEQ_HIDY_REG];
//PLL1
extern MX00_AR0233_REG_s AR0233LinPLLRegTable[AR0233_NUM_PLL_REG];
//WORKING MODE
extern MX00_AR0233_REG_s AR0233LinWorkingModeRegTable[AR0233_NUM_WORKING_MODE_REG];


extern MX00_AR0233_AGC_REG_s MX00_AR0233AgcRegTable[MX00_AR0233_NUM_AGC_STEP];
//extern MX00_AR0233_AGC_REG_s MX00_AR0233HDRAgcRegTable[MX00_AR0233_NUM_HDR_AGC_STEP];
extern MX00_AR0233_SE_AGC_REG_s MX00_AR0233HDRSEAgcRegTable[MX00_AR0233_NUM_HDR_SE_AGC_STEP];

extern AMBA_SENSOR_OBJ_s AmbaSensor_MX00_AR0233Obj;

#endif /* MAX9295_9296_AR0233_H */

