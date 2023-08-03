/**
 *  @file AmbaSensor.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions for Ambarella sensor driver APIs
 *
 */

#ifndef AMBA_SENSOR_H
#define AMBA_SENSOR_H

#ifndef AMBA_VIN_H
#include "AmbaVIN.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif
/*-----------------------------------------------------------------------------------------------*\
 * Common time table for sensor driver
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * Combine charges from M out of N pixels for one pixel,
 * where M is num and N is den in horizontal or vertical direction.
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Device Info.
\*-----------------------------------------------------------------------------------------------*/

/* ERROR CODE */
#define SENSOR_ERR_NONE         (0U)
#define SENSOR_ERR_INVALID_API  (SENSOR_ERR_BASE + 1U)
#define SENSOR_ERR_ARG          (SENSOR_ERR_BASE + 2U)
#define SENSOR_ERR_COMMUNICATE  (SENSOR_ERR_BASE + 3U)
#define SENSOR_ERR_SERDES       (SENSOR_ERR_BASE + 4U)
#define SENSOR_ERR_UNEXPECTED   (SENSOR_ERR_BASE + 5U)
#define SENSOR_ERR_MUTEX        (SENSOR_ERR_BASE + 6U)

/* print module */
#define SENSOR_MODULE_ID        ((UINT16)(SENSOR_ERR_BASE >> 16U))

#define AMBA_SENSOR_NUM_MAX_HDR_CHAN    4U
#define AMBA_SENSOR_CURRENT_MODE        0xffffffffU

typedef struct {
    UINT16 Module;
#define SENSOR_CFG_VERBOSE_IO   (0x0001)
#define SENSOR_CFG_VERBOSE_GAIN (0x0002)
#define SENSOR_CFG_VERBOSE_SHR  (0x0004)
} AMBA_SENSOR_DEBUG_LOG_s;

typedef struct {
    UINT32 VinID;
    UINT32 SensorID;
    UINT32 Reserved[2];
} AMBA_SENSOR_CHANNEL_s;

typedef struct {
    UINT32 ModeID;
    UINT32 ModeID_1;
    UINT32 ModeID_2;
    UINT32 ModeID_3;
    UINT8  EnableEmbDataCap;  /* Enable VIN to capture embedded data. */
    UINT8  KeepState;         /* Keep sensor state for special app restart flow */
    UINT8  Reserved0[2];
    UINT32 Reserved1[4];
} AMBA_SENSOR_CONFIG_s;

#define AMBA_SENSOR_SUBSAMPLE_NORMAL           0U
#define AMBA_SENSOR_SUBSAMPLE_BINNING          1U
#define AMBA_SENSOR_SUBSAMPLE_SCALING          2U
#define AMBA_SENSOR_SUBSAMPLE_BINNING_SCALING  3U

typedef struct {
    UINT32  SubsampleType;
    UINT8   FactorNum;              /* subsamping factor (numerator) */
    UINT8   FactorDen;              /* subsamping factor (denominator) */
} AMBA_SENSOR_SUBSAMPLING_s;

typedef struct {
    UINT16  StartX;
    UINT16  StartY;
    UINT16  Width;
    UINT16  Height;
} AMBA_SENSOR_AREA_INFO_s;

#define AMBA_SENSOR_COMMUNICATION_AT_VBLANK     0U
#define AMBA_SENSOR_COMMUNICATION_AT_NON_VBLANK 1U

#define AMBA_SENSOR_ESHUTTER_TYPE_ROLLING       0U
#define AMBA_SENSOR_ESHUTTER_TYPE_GLOBAL_RESET  1U

typedef struct {
    UINT32  CommunicationTime;  /* time to access sensor registers */
    UINT8   FirstReflectedFrame;                            /* number of frames from communication end to the first frame with new settings */
    UINT8   FirstBadFrame;                                  /* number of frames from communication end to the first bad frame */
    UINT8   NumBadFrames;                                   /* number of continouse bad frames from the first bad frame */
} AMBA_SENSOR_CTRL_INFO_s;

typedef struct {
    AMBA_SENSOR_AREA_INFO_s     PhotodiodeArray;    /* effective photodiode array position */
    AMBA_SENSOR_SUBSAMPLING_s   HSubsample;         /* info about frame data readout from active pixel array */
    AMBA_SENSOR_SUBSAMPLING_s   VSubsample;         /* info about frame data readout from active pixel array */
    UINT8                       SummingFactor;      /* number of pixel data to be sumed up */
} AMBA_SENSOR_INPUT_INFO_s;

/* For RGB sensor */
#define AMBA_SENSOR_BAYER_PATTERN_RG    AMBA_VIN_BAYER_PATTERN_RG
#define AMBA_SENSOR_BAYER_PATTERN_BG    AMBA_VIN_BAYER_PATTERN_BG
#define AMBA_SENSOR_BAYER_PATTERN_GR    AMBA_VIN_BAYER_PATTERN_GR
#define AMBA_SENSOR_BAYER_PATTERN_GB    AMBA_VIN_BAYER_PATTERN_GB

/* For RGB-IR sensor */
#define AMBA_SENSOR_BAYER_PATTERN_RGGI  AMBA_VIN_BAYER_PATTERN_RGGI
#define AMBA_SENSOR_BAYER_PATTERN_IGGR  AMBA_VIN_BAYER_PATTERN_IGGR
#define AMBA_SENSOR_BAYER_PATTERN_GRIG  AMBA_VIN_BAYER_PATTERN_GRIG
#define AMBA_SENSOR_BAYER_PATTERN_GIRG  AMBA_VIN_BAYER_PATTERN_GIRG
#define AMBA_SENSOR_BAYER_PATTERN_BGGI  AMBA_VIN_BAYER_PATTERN_BGGI
#define AMBA_SENSOR_BAYER_PATTERN_IGGB  AMBA_VIN_BAYER_PATTERN_IGGB
#define AMBA_SENSOR_BAYER_PATTERN_GBIG  AMBA_VIN_BAYER_PATTERN_GBIG
#define AMBA_SENSOR_BAYER_PATTERN_GIBG  AMBA_VIN_BAYER_PATTERN_GIBG

/* For RCCC sensor */
#define AMBA_SENSOR_BAYER_PATTERN_RCCC  AMBA_VIN_BAYER_PATTERN_RCCC
#define AMBA_SENSOR_BAYER_PATTERN_CCCR  AMBA_VIN_BAYER_PATTERN_CCCR
#define AMBA_SENSOR_BAYER_PATTERN_CRCC  AMBA_VIN_BAYER_PATTERN_CRCC
#define AMBA_SENSOR_BAYER_PATTERN_CCRC  AMBA_VIN_BAYER_PATTERN_CCRC

typedef struct {
    UINT64                      DataRate;           /* output bit/pixel clock frequency from image sensor */
    UINT8                       NumDataLanes;       /* active data channels */
    UINT8                       NumDataBits;        /* pixel data bit depth */
    UINT8                       BayerPattern;       /* Color coding of the first 2x2 pixels, please check AMBA_SENSOR_BAYER_PATTERN_xxx */
    UINT16                      OutputWidth;        /* valid pixels per line */
    UINT16                      OutputHeight;       /* valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s     RecordingPixels;    /* maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s     OpticalBlackPixels; /* user clamp area */
} AMBA_SENSOR_OUTPUT_INFO_s;


typedef struct {
    AMBA_SENSOR_AREA_INFO_s   EffectiveArea;         /* effective area for each channel */
    AMBA_SENSOR_AREA_INFO_s   OpticalBlackPixels;    /* user clamp area for each channel */
    AMBA_SENSOR_CTRL_INFO_s   ShutterSpeedCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s   OutputFormatCtrlInfo;
    UINT32 MaxExposureLine;                          /* maximum exposure line (in row time) */
    UINT32 MinExposureLine;                          /* minimum exposure line. (in row time) */
    UINT32 NumExposureStepPerFrame;                  /* number of exposure time unit for the expsosure channel per frame */
} AMBA_SENSOR_HDR_CHANNEL_INFO_s;

#define AMBA_SENSOR_HDR_NONE                 0U
#define AMBA_SENSOR_HDR_TYPE_MULTI_SLICE     1U
#define AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND  2U
#define AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL 3U

typedef struct {
    UINT32  HdrType;
    UINT8   NotSupportIndividualGain;                /* 0: sensor supports individual gain in hdr mode, 1: sensor doesn't support individual gain in hdr mode */
    UINT8   ActiveChannels;                          /* HDR exposure channel number */
    AMBA_SENSOR_HDR_CHANNEL_INFO_s ChannelInfo[AMBA_SENSOR_NUM_MAX_HDR_CHAN];
} AMBA_SENSOR_HDR_INFO_s;

typedef struct {
    UINT32  IsSupport;      /* 0: not support embbed data output, 1: support embbed data output */
    UINT32  NumDataBits;    /* bits per pixel */
    UINT32  Width;          /* number of pixels per line */
    UINT32  Height;         /* number of lines per frame */
} AMBA_SENSOR_EMB_DATA_INFO_s;

typedef struct {
    AMBA_SENSOR_CONFIG_s        Config;
    UINT32                      LineLengthPck;              /* number of pixel clock cycles per line of frame */
    UINT32                      FrameLengthLines;           /* number of lines per frame */
    UINT32                      NumExposureStepPerFrame;    /* number of exposure time unit per frame */
    FLOAT                       InternalExposureOffset;     /* internal exposure offset (unit: second) */
    AMBA_VIN_FRAME_RATE_s       FrameRate;                  /* frame rate of this sensor mode */
    AMBA_VIN_FRAME_RATE_s       MinFrameRate;               /* minimum framerate value */
    UINT32                      InputClk;                   /* Sensor side input clock frequency */
    FLOAT                       RowTime;                    /* time interval of LineLengthPck (in seconds) */
    AMBA_SENSOR_INPUT_INFO_s    InputInfo;                  /* info about photodetector */
    AMBA_SENSOR_OUTPUT_INFO_s   OutputInfo;                 /* info about data output interface */
    AMBA_SENSOR_HDR_INFO_s      HdrInfo;                    /* HDR information */
    AMBA_SENSOR_EMB_DATA_INFO_s EmbDataInfo;                /* Embedded data information */
} AMBA_SENSOR_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_MODE_INFO_s     ModeInfo;
    UINT32                      ElecShutterMode;
} AMBA_SENSOR_STATUS_INFO_s;

typedef struct {
    FLOAT   UnitCellWidth;      /* pixel width (um) */
    FLOAT   UnitCellHeight;     /* pixel height (um) */

    UINT16  NumTotalPixelCols;
    UINT16  NumTotalPixelRows;
    UINT16  NumEffectivePixelCols;
    UINT16  NumEffectivePixelRows;

    AMBA_SENSOR_CTRL_INFO_s     FrameRateCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s     ShutterSpeedCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s     AnalogGainCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s     DigitalGainCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s     WbGainCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s     StrobeCtrlInfo;
    AMBA_SENSOR_CTRL_INFO_s     MasterSyncCtrlInfo;

    FLOAT   MinAnalogGainFactor;
    FLOAT   MaxAnalogGainFactor;
    FLOAT   MinDigitalGainFactor;
    FLOAT   MaxDigitalGainFactor;

    UINT8   HdrIsSupport;       /* 0: sensor doesn't support hdr, 1: sensor supports hdr */
} AMBA_SENSOR_DEVICE_INFO_s;

typedef struct {
    FLOAT R;
    FLOAT Gr;
    FLOAT Gb;
    FLOAT B;
} AMBA_SENSOR_WB_GAIN_FACTOR_s;

typedef struct {
    FLOAT Gain[AMBA_SENSOR_NUM_MAX_HDR_CHAN];
    AMBA_SENSOR_WB_GAIN_FACTOR_s WbGain[AMBA_SENSOR_NUM_MAX_HDR_CHAN];
    UINT32 Reserved[1];
} AMBA_SENSOR_GAIN_FACTOR_s;

typedef struct {
    UINT32 R;
    UINT32 Gr;
    UINT32 Gb;
    UINT32 B;
} AMBA_SENSOR_WB_CTRL_s;

typedef struct {
    UINT32 AnalogGain[AMBA_SENSOR_NUM_MAX_HDR_CHAN];
    UINT32 DigitalGain[AMBA_SENSOR_NUM_MAX_HDR_CHAN];
    AMBA_SENSOR_WB_CTRL_s WbGain[AMBA_SENSOR_NUM_MAX_HDR_CHAN];
    UINT32 Reserved[1];
} AMBA_SENSOR_GAIN_CTRL_s;

typedef struct {
    UINT32 LinkLock;    /* 0: not support to check, 1: Far-end chip access OK, 2: Far-end chip access NG */
    UINT32 VideoLock;   /* 0: not support to check, 1: Near-end chip receives video data OK, 2: receive video data NG */
    UINT32 Reserved[2];
} AMBA_SENSOR_SERDES_STATUS_s;

typedef struct {
    char SensorName[32];  /* Name of the image sensor */
    char SerdesName[32];  /* Name of the serdes */

    UINT32 (*Init)(const AMBA_SENSOR_CHANNEL_s *pChan);
    UINT32 (*Enable)(const AMBA_SENSOR_CHANNEL_s *pChan);
    UINT32 (*Disable)(const AMBA_SENSOR_CHANNEL_s *pChan);
    UINT32 (*Config)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode);

    UINT32 (*GetStatus)(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus);
    UINT32 (*GetModeInfo)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo);
    UINT32 (*GetDeviceInfo)(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo);
    UINT32 (*GetHdrInfo)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo);
    UINT32 (*GetCurrentGainFactor)(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor);
    UINT32 (*GetCurrentShutterSpeed)(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime);

    UINT32 (*ConvertGainFactor)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl);
    UINT32 (*ConvertShutterSpeed)(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl);

    UINT32 (*SetAnalogGainCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl);
    UINT32 (*SetDigitalGainCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl);
    UINT32 (*SetWbGainCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl);
    UINT32 (*SetShutterCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl);
    UINT32 (*SetSlowShutterCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl);
    UINT32 (*SetMasterSyncCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl);

    UINT32 (*RegisterRead)(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16* pData);
    UINT32 (*RegisterWrite)(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data);

    UINT32 (*ConfigPost)(const AMBA_SENSOR_CHANNEL_s *pChan);

    UINT32 (*GetSerdesStatus)(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_SERDES_STATUS_s *pSerdesStatus);

    UINT32 (*ConvertStrobeWidth)(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredWidth, FLOAT *pActualWidth, UINT32 *pStrobeCtrl);
    UINT32 (*SetStrobeCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pStrobeCtrl);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    UINT32 (*SetRotation)(const AMBA_SENSOR_CHANNEL_s *pChan, UINT8 Rotation);
#endif

} AMBA_SENSOR_OBJ_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaSensor.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_SENSOR_OBJ_s *pAmbaSensorObj[AMBA_NUM_VIN_CHANNEL];
extern void AmbaSensor_Hook(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_OBJ_s *pSensorObj);

static inline UINT32 AmbaSensor_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->Init == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->Init(pChan));
}
static inline UINT32 AmbaSensor_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->Enable == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->Enable(pChan));
}
static inline UINT32 AmbaSensor_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->Disable == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->Disable(pChan));
}
static inline UINT32 AmbaSensor_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->Config == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->Config(pChan,pMode));
}

static inline UINT32 AmbaSensor_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->GetStatus == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->GetStatus((pChan),pStatus));
}
static inline UINT32 AmbaSensor_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->GetModeInfo == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->GetModeInfo(pChan,pMode,pModeInfo));
}
static inline UINT32 AmbaSensor_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->GetDeviceInfo == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->GetDeviceInfo(pChan,pDeviceInfo));
}
static inline UINT32 AmbaSensor_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->GetHdrInfo == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->GetHdrInfo(pChan,pShutterCtrl,pHdrInfo));
}
static inline UINT32 AmbaSensor_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->GetCurrentGainFactor == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->GetCurrentGainFactor(pChan,pGainFactor));
}
static inline UINT32 AmbaSensor_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->GetCurrentShutterSpeed == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->GetCurrentShutterSpeed(pChan,pExposureTime));
}

static inline UINT32 AmbaSensor_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->ConvertGainFactor == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->ConvertGainFactor(pChan,pDesiredFactor,pActualFactor,pGainCtrl));
}
static inline UINT32 AmbaSensor_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExpTime, FLOAT *pActualExpTime, UINT32 *pShutterCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->ConvertShutterSpeed == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->ConvertShutterSpeed(pChan,pDesiredExpTime,pActualExpTime,pShutterCtrl));
}

static inline UINT32 AmbaSensor_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->SetAnalogGainCtrl == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->SetAnalogGainCtrl(pChan,pAnalogGainCtrl));
}
static inline UINT32 AmbaSensor_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->SetDigitalGainCtrl == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->SetDigitalGainCtrl(pChan,pDigitalGainCtrl));
}
static inline UINT32 AmbaSensor_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->SetWbGainCtrl == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->SetWbGainCtrl(pChan,pWbGainCtrl));
}
static inline UINT32 AmbaSensor_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->SetShutterCtrl == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->SetShutterCtrl(pChan,pShutterCtrl));
}
static inline UINT32 AmbaSensor_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->SetSlowShutterCtrl == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->SetSlowShutterCtrl(pChan,SlowShutterCtrl));
}
static inline UINT32 AmbaSensor_SetMasterSyncCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->SetMasterSyncCtrl == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->SetMasterSyncCtrl(pChan, SlowShutterCtrl));
}

static inline UINT32 AmbaSensor_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->RegisterRead == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->RegisterRead(pChan,Addr,pData));
}
static inline UINT32 AmbaSensor_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->RegisterWrite == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->RegisterWrite(pChan,Addr,Data));
}

static inline UINT32 AmbaSensor_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->ConfigPost == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->ConfigPost(pChan));
}

static inline UINT32 AmbaSensor_GetSerdesStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_SERDES_STATUS_s *pSerdesStatus)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->GetSerdesStatus == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->GetSerdesStatus(pChan, pSerdesStatus));
}

static inline UINT32 AmbaSensor_ConvertStrobeWidth(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredWidth, FLOAT *pActualWidth, UINT32 *pStrobeCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->ConvertStrobeWidth == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->ConvertStrobeWidth(pChan, pDesiredWidth, pActualWidth, pStrobeCtrl));
}
static inline UINT32 AmbaSensor_SetStrobeCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pStrobeCtrl)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->SetStrobeCtrl == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->SetStrobeCtrl(pChan, pStrobeCtrl));
}
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static inline UINT32 AmbaSensor_SetRotation(const AMBA_SENSOR_CHANNEL_s *pChan, UINT8 rotation)
{
    return ((pAmbaSensorObj[(pChan)->VinID]->SetRotation == NULL) ? SENSOR_ERR_INVALID_API : pAmbaSensorObj[(pChan)->VinID]->SetRotation(pChan,rotation));
}
#endif

#endif /* AMBA_SENSOR_H */
