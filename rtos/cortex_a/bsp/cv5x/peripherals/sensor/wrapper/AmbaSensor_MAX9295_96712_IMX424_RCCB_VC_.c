/**
 *  @file AmbaSensor_MAX9295_96712_IMX424_VC_.c
 *
 *  Copyright (c) 2022 Ambarella International LP
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
 *  @details Control APIs of MAX9295_96712 plus SONY IMX424 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaMisraFix.h"

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_96712_IMX424_RCCB.h"
#include "AmbaSensor_MAX9295_96712_Wrapper.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSbrg_Max9295_96712.h"

#include "bsp.h"

//#define DEBUG_MSG //enable if when need to print all sensor setting

#ifdef CONFIG_SVC_APPS_ICAM
#define MX01_IMX424_VC_IN_SLAVE_MODE
#endif
#define MX01_IMX424_NUM_CTRL_BUF    4

//static UINT32 MX01_IMX424_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);

static UINT32 MX01_IMX424I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_MAXIM_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_MAXIM_I2C_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_MAXIM_I2C_CHANNEL_VIN2,
    [AMBA_VIN_CHANNEL3]  = AMBA_MAXIM_I2C_CHANNEL_VIN3,
    [AMBA_VIN_CHANNEL4]  = AMBA_MAXIM_I2C_CHANNEL_VIN4,
    [AMBA_VIN_CHANNEL5]  = AMBA_MAXIM_I2C_CHANNEL_VIN5,
    [AMBA_VIN_CHANNEL6]  = AMBA_MAXIM_I2C_CHANNEL_VIN6,
    [AMBA_VIN_CHANNEL7]  = AMBA_MAXIM_I2C_CHANNEL_VIN7,
    [AMBA_VIN_CHANNEL8]  = AMBA_MAXIM_I2C_CHANNEL_VIN8,
    [AMBA_VIN_CHANNEL9]  = AMBA_MAXIM_I2C_CHANNEL_VIN9,
    [AMBA_VIN_CHANNEL10] = AMBA_MAXIM_I2C_CHANNEL_VIN10,
    [AMBA_VIN_CHANNEL11] = AMBA_MAXIM_I2C_CHANNEL_VIN11,
    [AMBA_VIN_CHANNEL12] = AMBA_MAXIM_I2C_CHANNEL_VIN12,
    [AMBA_VIN_CHANNEL13] = AMBA_MAXIM_I2C_CHANNEL_VIN13,
};

#ifdef MX01_IMX424_VC_IN_SLAVE_MODE
static UINT32 MX01_IMX424MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN2,
    [AMBA_VIN_CHANNEL3]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN3,
    [AMBA_VIN_CHANNEL4]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN4,
    [AMBA_VIN_CHANNEL5]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN5,
    [AMBA_VIN_CHANNEL6]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN6,
    [AMBA_VIN_CHANNEL7]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN7,
    [AMBA_VIN_CHANNEL8]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN8,
    [AMBA_VIN_CHANNEL9]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN9,
    [AMBA_VIN_CHANNEL10] = AMBA_SENSOR_MSYNC_CHANNEL_VIN10,
    [AMBA_VIN_CHANNEL11] = AMBA_SENSOR_MSYNC_CHANNEL_VIN11,
    [AMBA_VIN_CHANNEL12] = AMBA_SENSOR_MSYNC_CHANNEL_VIN12,
    [AMBA_VIN_CHANNEL13] = AMBA_SENSOR_MSYNC_CHANNEL_VIN13,
};

static UINT32 MX01_IMX424_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID);
#endif
/*-----------------------------------------------------------------------------------------------*\
 * MX01_IMX424 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX01_IMX424_CTRL_s MX01_IMX424CtrlBuf[MX01_IMX424_NUM_CTRL_BUF] = {0};

static MX01_IMX424_CTRL_s *pMX01_IMX424Ctrl[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0  ... AMBA_VIN_CHANNEL3]  = &MX01_IMX424CtrlBuf[0],
    [AMBA_VIN_CHANNEL4  ... AMBA_VIN_CHANNEL7]  = &MX01_IMX424CtrlBuf[1],
    [AMBA_VIN_CHANNEL8  ... AMBA_VIN_CHANNEL10] = &MX01_IMX424CtrlBuf[2],
    [AMBA_VIN_CHANNEL11 ... AMBA_VIN_CHANNEL13] = &MX01_IMX424CtrlBuf[3],
};

static MAX9295_96712_SERDES_CONFIG2_s MX01_IMX424SerdesConfig[MX01_IMX424_NUM_CTRL_BUF] = {
    [0 ... (MX01_IMX424_NUM_CTRL_BUF - 1)] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   =  {
            [0] = MX01_IMX424_I2C_SLAVE_ADDRESS,
            [1] = MX01_IMX424_I2C_SLAVE_ADDRESS,
            [2] = MX01_IMX424_I2C_SLAVE_ADDRESS,
            [3] = MX01_IMX424_I2C_SLAVE_ADDRESS,
        },
        .SensorBCID      = MX01_IMX424_I2C_BC_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = 0x60U,
            [1] = 0x62U,
            [2] = 0x64U,
            [3] = 0x66U,
        },
        .DataType        = {
            [0] = 0x2cU,  /* Default DataType: RAW12 */
            [1] = 0x2cU,  /* Default DataType: RAW12 */
            [2] = 0x2cU,  /* Default DataType: RAW12 */
            [3] = 0x2cU,  /* Default DataType: RAW12 */
        },
        .DataType2       = {
            [0] = 0xffU,  /* Default secondary DataType: not used */
            [1] = 0xffU,  /* Default secondary DataType: not used */
            [2] = 0xffU,  /* Default secondary DataType: not used */
            [3] = 0xffU,  /* Default secondary DataType: not used */
        },
        .CSIRxLaneNum    = {4, 4, 4, 4},
        .CSITxLaneNum    = {4, 4, 4, 4},

        .CSITxSpeed      = {10, 10, 10, 10},

        .VideoAggregationMode = MAX9295_MAX96712_VIDEO_AGGR_FCFS,
    },
};

static UINT32 MX01_IMX424_GetModeID(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 ModeID = pMode->ModeID;

    if ((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_A) != 0U) {
        ModeID = pMode->ModeID;
    } else if ((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_B) != 0U) {
        ModeID = pMode->ModeID_1;
    } else if ((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_C) != 0U) {
        ModeID = pMode->ModeID_2;
    } else if ((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_D) != 0U) {
        ModeID = pMode->ModeID_3;
    } else {
        /* avoid misraC error */
    }

    return ModeID;
}

#if 0
static UINT32 MX01_IMX424_UpdateSerdesCSITxConfig(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 i, ModeID;
    UINT32 DataRate;
    UINT64 WorkUINT64, TotalBandwidth = 0U;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg = pMX01_IMX424Ctrl[pChan->VinID]->pSerdesCfg;
    DOUBLE CeilVal;
    UINT8 CSITxLaneNum = 1U;
    AMBA_SENSOR_CHANNEL_s Chan_t;
    UINT32 RetVal = SENSOR_ERR_NONE;

    for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
        if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            Chan_t.VinID = pChan->VinID;
            Chan_t.SensorID = (UINT32)1U << (4U * (i + 1U));

            ModeID = MX01_IMX424_GetModeID(&Chan_t, pMode);

            pOutputInfo = &MX01_IMX424_OutputInfo[ModeID];

            WorkUINT64 = (UINT64) pOutputInfo->DataRate;
            WorkUINT64 *= (UINT64) pOutputInfo->NumDataLanes;

            TotalBandwidth += WorkUINT64;

            if (CSITxLaneNum < pOutputInfo->NumDataLanes) {
                CSITxLaneNum = pOutputInfo->NumDataLanes;
            }
        }
    }

    DataRate = (UINT32) (TotalBandwidth / (UINT64) CSITxLaneNum);

    if (DataRate > 2500000000U) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "DataRate %d bps/lane over Maxim 2.5Gbps/lane limitation, set 2.5Gbps/lane instead", DataRate, 0U, 0U, 0U, 0U);
        DataRate = 2500000000U;
    }

    if (AmbaWrap_ceil((DOUBLE)DataRate * 1e-8, &CeilVal) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    pSerdesCfg->CSITxSpeed[0] = (UINT8)CeilVal;
    pSerdesCfg->CSITxLaneNum[0] = CSITxLaneNum;

    return RetVal;
}
#endif

static UINT16 MX01_IMX424_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Chan:     Vin ID and sensor ID
 *      Mode:     Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = MX01_IMX424_GetModeID(pChan, pMode);
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &MX01_IMX424_InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &MX01_IMX424_OutputInfo[ModeID];
    const MX01_IMX424_SENSOR_INFO_s     *pSensorInfo    = &MX01_IMX424_SensorInfo[ModeID];
    UINT32 DataRate;
    DOUBLE PixelRate, FloorVal;
    UINT8 NumDataLanes;
    UINT32 RetVal = SENSOR_ERR_NONE;
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg = pMX01_IMX424Ctrl[pChan->VinID]->pSerdesCfg;

    if (AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    DataRate = (UINT32) pSerdesCfg->CSITxSpeed[0] * 100000000U;
    NumDataLanes = pSerdesCfg->CSITxLaneNum[0];

    pModeInfo->FrameLengthLines = pSensorInfo->FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame = pSensorInfo->VMAX * 500U;
    pModeInfo->InternalExposureOffset = 0.0f;
    pModeInfo->RowTime = MX01_IMX424_ModeInfoList[ModeID].RowTime;

    PixelRate = (DOUBLE) DataRate * (DOUBLE) NumDataLanes / (DOUBLE) pOutputInfo->NumDataBits;
    if (AmbaWrap_floor(((DOUBLE) PixelRate * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    pModeInfo->LineLengthPck = (UINT32)FloorVal;
    pModeInfo->InputClk = pSensorInfo->InputClk;

    if (AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->FrameRate, &MX01_IMX424_ModeInfoList[ModeID].FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &MX01_IMX424_ModeInfoList[ModeID].FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX01_IMX424_HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) { // HDR information
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    /* update for MAX96712 CSI-2 output */
    pModeInfo->OutputInfo.DataRate = DataRate;
    pModeInfo->OutputInfo.NumDataLanes = NumDataLanes;

    /* Updated minimum frame rate limitation */
    /*
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
    */

    return RetVal;
}

#if 0
static UINT32 MX01_IMX424_GetDataType(UINT8 NumDataBits, UINT8 *pDataType)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (NumDataBits == 10U) {
        *pDataType = 0x2bU;
    } else if (NumDataBits == 12U) {
        *pDataType = 0x2cU;
    } else if (NumDataBits == 14U) {
        *pDataType = 0x2dU;
    } else if (NumDataBits == 16U) {
        *pDataType = 0x2eU;
    } else {
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_ConfigSerDes
 *
 *  @Description:: Configure Serdes sync/video forwarding, I2C translation, and MIPI I/F
 *
 *  @Input      ::
 *      Chan:        Vin ID and sensor ID
 *      SensorMode:  Sensor mode
 *      pModeInfo:   Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX01_IMX424_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pModeList)
{
    const MX01_IMX424_SENSOR_INFO_s *pSensorInfo = NULL;
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg = pMX01_IMX424Ctrl[pChan->VinID]->pSerdesCfg;
    UINT32 i;

    pSerdesCfg->EnabledLinkID = MX01_IMX424_GetEnabledLinkID(pChan->SensorID);

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        pSensorInfo = &MX01_IMX424_SensorInfo[pModeList[i]];

        if (MX01_IMX424_GetDataType(pSensorInfo->NumDataBits, &pSerdesCfg->DataType[i]) != SENSOR_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX424] Can't get DataType for NumDataBits[%d]=%d", i, pSensorInfo->NumDataBits, 0U, 0U, 0U);
        }
        pSerdesCfg->NumDataBits[i] = pSensorInfo->NumDataBits;
        pSerdesCfg->CSIRxLaneNum[i] = pSensorInfo->NumDataLanes;
    }

    (void) Max9295_96712_Config2(pChan->VinID, pSerdesCfg);
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output frames of the new readout mode
 *
 *  @Input      ::
 *      pModeInfo:  Details of the specified readout mode
 *      pFrameTime: Sensor frame time configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID = pModeInfo->Config.ModeID;
    const AMBA_SENSOR_OUTPUT_INFO_s  *pOutputInfo = &MX01_IMX424_OutputInfo[ModeID];
    AMBA_VIN_MIPI_CONFIG_s VinCfg;
    UINT32 HdrExpNum;
    UINT32 RetVal = SENSOR_ERR_NONE;

    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    AMBA_VIN_MIPI_CONFIG_s MX01_IMX424_VinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace = AMBA_VIN_COLOR_SPACE_RGB,
            .BayerPattern = AMBA_SENSOR_BAYER_PATTERN_RG,
            .NumDataBits  = 0,
            .NumSkipFrame       = 1U,
            .RxHvSyncCtrl           = {
                .NumActivePixels    = 0,
                .NumActiveLines     = 0,
                .NumTotalPixels     = 0,
                .NumTotalLines      = 0,
            },
            .SplitCtrl = {0},
            .DelayedVsync = 0,
        },
        .NumActiveLanes = 4U,
        .DataType = 0x20U,
        .DataTypeMask = 0x1fU,
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0}
    };

    (void)AmbaWrap_memcpy(&VinCfg, &MX01_IMX424_VinConfig, sizeof(AMBA_VIN_MIPI_CONFIG_s));

    VinCfg.NumActiveLanes       = pOutputInfo->NumDataLanes;
    VinCfg.Config.NumDataBits   = pOutputInfo->NumDataBits;
    VinCfg.Config.BayerPattern  = pOutputInfo->BayerPattern;

    if ((MX01_IMX424_HdrInfo[ModeID].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE)) {
        HdrExpNum = pMX01_IMX424Ctrl[VinID]->Status[0].ModeInfo.HdrInfo.ActiveChannels;
        VinCfg.Config.RxHvSyncCtrl.NumActivePixels   = pOutputInfo->RecordingPixels.Width * HdrExpNum;
        VinCfg.Config.RxHvSyncCtrl.NumActiveLines    = pOutputInfo->RecordingPixels.Height / HdrExpNum;
        VinCfg.Config.RxHvSyncCtrl.NumTotalPixels    = pOutputInfo->OutputWidth * HdrExpNum;
        VinCfg.Config.RxHvSyncCtrl.NumTotalLines     = pOutputInfo->OutputHeight / HdrExpNum;

        VinCfg.Config.SplitCtrl.NumSplits = (UINT8)(HdrExpNum);      /* note: should be <= 4 */
        VinCfg.Config.SplitCtrl.SplitWidth = pOutputInfo->OutputWidth;
    } else {
        VinCfg.Config.RxHvSyncCtrl.NumActivePixels   = pOutputInfo->RecordingPixels.Width;
        VinCfg.Config.RxHvSyncCtrl.NumActiveLines    = pOutputInfo->RecordingPixels.Height;
        VinCfg.Config.RxHvSyncCtrl.NumTotalPixels    = pOutputInfo->OutputWidth;
        VinCfg.Config.RxHvSyncCtrl.NumTotalLines     = pOutputInfo->OutputHeight;


    }

    if (AmbaWrap_memcpy(&(VinCfg.Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    RetVal |= AmbaVIN_MipiConfig(VinID, &VinCfg);
    return RetVal;
}

static UINT32 MX01_IMX424_SetSensorClock(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_MODE_INFO_s *pModeInfo[MX01_IMX424_NUM_MAX_SENSOR_COUNT])
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 InputClk;
    UINT8 i = 0U;

    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Max9295_96712_SetSensorClk", NULL, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "; # Link A: MAX9295A MFP2 to output 27M for IMX424", NULL, NULL, NULL, NULL, NULL);

    /* set MAX9295 MFP2 to output sensor clock */
    for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            InputClk = pModeInfo[i]->InputClk;
            if (InputClk == 23976024) {
                RetVal |= Max9295_96712_SetSensorClk(pChan->VinID, i, MAX9295_96712_RCLK_OUT_FREQ_23976024);
            } else {
                RetVal |= Max9295_96712_SetSensorClk(pChan->VinID, i, MAX9295_96712_RCLK_OUT_FREQ_24M);
            }
        }
    }

    if (RetVal != MAX9295_96712_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_ResetSensor
 *
 *  @Description:: Reset IMX424 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MX01_IMX424_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX424_ResetSensor] ", NULL, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "; # Link A: MAX9295A MFP4 H->L->H for IMX424", NULL, NULL, NULL, NULL, NULL);

    if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_A_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_A_ID, MAX9295_96712_GPIO_PIN_4, 1);
    }
    if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_B_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_B_ID, MAX9295_96712_GPIO_PIN_4, 1);
    }
    if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_C) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_C_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_C_ID, MAX9295_96712_GPIO_PIN_4, 1);
    }
    if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_D) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_D_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_D_ID, MAX9295_96712_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start */

    if (RetVal != MAX9295_96712_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: RegWrite
 *
 *  @Description:: Read/Write sensor registers through I2C bus
 *
 *  @Input      ::
 *      pChan:      pointer to Vin ID and sensor ID
 *      Addr:       Register Address
 *      pTxData:    Pointer to Write data buffer
 *      pRxData:    Pointer to Echo data buffer
 *      Size:       Number of Read/Write data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX01_IMX424_I2C_WR_BUF_SIZE + 2U];
    UINT32 TxSize, i;
#ifdef DEBUG_MSG
    UINT32 k;
#endif
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg;

    if ((pChan == NULL) || (Size > MX01_IMX424_I2C_WR_BUF_SIZE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(MX01_IMX424I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        pSerdesCfg = pMX01_IMX424Ctrl[pChan->VinID]->pSerdesCfg;
        if ((pChan->SensorID == (MX01_IMX424_SENSOR_ID_CHAN_A)) ||
            (pChan->SensorID == (MX01_IMX424_SENSOR_ID_CHAN_B)) ||
            (pChan->SensorID == (MX01_IMX424_SENSOR_ID_CHAN_C)) ||
            (pChan->SensorID == (MX01_IMX424_SENSOR_ID_CHAN_D))) {

            for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = pSerdesCfg->SensorAliasID[i];

                    RetVal |= AmbaI2C_MasterWrite(MX01_IMX424I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD,
                                                  &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[SensorID 0x%02x] Slave ID 0x%2x", ((UINT32)1U << i), I2cConfig.SlaveAddr, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                    for (k = 0U; k < Size; k++) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
                        AmbaPrint_Flush();
                    }
#endif
                }
            }


        } else {
            I2cConfig.SlaveAddr = MX01_IMX424_I2C_BC_SLAVE_ADDRESS;

            RetVal = AmbaI2C_MasterWrite(MX01_IMX424I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD,
                                         &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Broadcast] SensorID 0x%05x Slave ID 0x%2x", pChan->SensorID, I2cConfig.SlaveAddr, 0U, 0U, 0U);
            AmbaPrint_Flush();
            for (k = 0U; k < Size; k++) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
#endif
        }

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX424] I2C_Ch %d Slave 0x%02x Addr 0x%04x Size %d I2C does not work!!!!!", MX01_IMX424I2cChannel[pChan->VinID], I2cConfig.SlaveAddr, Addr, Size, 0U);
        }
    }
#if 0
    for (i = 0U; i < Size; i++) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX424][IO] Addr = 0x%04x, Data = 0x%02x", Addr+i, pTxData[i], 0U, 0U, 0U);
        TxDataBuf[0] = (UINT8)((Addr+i) & 0xff);
        TxDataBuf[1] = (UINT8)(((Addr+i)>>8) & 0xff);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "sendln 't drv i2c write 0 0x%02X 0x%02X 0x%02X 0x%02X", I2cConfig.SlaveAddr, TxDataBuf[1], TxDataBuf[0], pTxData[i], 0U);
    }
#endif
    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_RegisterWrite
 *
 *  @Description:: Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      pChan:  pointer to Vin ID and sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)Data;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegWrite(pChan, Addr, &WData, 1U);
    }

    return RetVal;
}

static UINT32 MX01_IMX424_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2] = {0};
    UINT32 TxSize;
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg;

    I2cTxConfig.SlaveAddr = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        pSerdesCfg = pMX01_IMX424Ctrl[pChan->VinID]->pSerdesCfg;
        if(pChan->SensorID == MX01_IMX424_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = pSerdesCfg->SensorAliasID[0];
        } else if(pChan->SensorID == MX01_IMX424_SENSOR_ID_CHAN_B) {
            I2cTxConfig.SlaveAddr = pSerdesCfg->SensorAliasID[1];
        } else if(pChan->SensorID == MX01_IMX424_SENSOR_ID_CHAN_C) {
            I2cTxConfig.SlaveAddr = pSerdesCfg->SensorAliasID[2];
        } else if(pChan->SensorID == MX01_IMX424_SENSOR_ID_CHAN_D) {
            I2cTxConfig.SlaveAddr = pSerdesCfg->SensorAliasID[3];
        } else {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX01_IMX424_RegRead] NG. incorrect sensor ID 0x%x", pChan->SensorID, 0U, 0U, 0U, 0U);
            RetVal = SENSOR_ERR_ARG;
        }
        if (RetVal == SENSOR_ERR_NONE) {
            I2cTxConfig.DataSize  = 2U;
            I2cTxConfig.pDataBuf  = TxData;
            TxData[0] = (UINT8)(Addr >> 8U);
            TxData[1] = (UINT8)(Addr & 0xffU);

            I2cRxConfig.SlaveAddr = (I2cTxConfig.SlaveAddr | (UINT32)1U);
            I2cRxConfig.DataSize  = 1U;
            I2cRxConfig.pDataBuf  = pRxData;

            RetVal = AmbaI2C_MasterReadAfterWrite(MX01_IMX424I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                                  &I2cRxConfig, &TxSize, 1000U);
        } else {
            //SENSOR_ERR
        }
    }
    if (RetVal != I2C_ERR_NONE) {
        RetVal = SENSOR_ERR_COMMUNICATE;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX424] i2c Slave Addr 0x%02x Addr 0x%04x I2C does not work!!!!!", I2cTxConfig.SlaveAddr, Addr, 0U, 0U, 0U);
    } else {
        //SENSOR_ERR_NONE
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX9295_96712_IMX424_RegisterRead
 *
 *  @Description:: Read sensor registers API
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX01_IMX424_RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_SetAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      AGC:    Analog gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AGC)
{
    UINT8 WData[2];

    WData[0] = (UINT8)(AGC & 0xffU);
    WData[1] = (UINT8)((AGC >> 8U) & 0x01U);

    return RegWrite(pChan, MX01_IMX424_AGAIN, WData, 2U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_SetWbGainReg
 *
 *  @Description:: Configure sensor WB gain setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pWbGainCtrl:     WB gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[8];

    WData[0] = (UINT8)(pWbGainCtrl[0].Gb & 0xffU);
    WData[1] = (UINT8)((pWbGainCtrl[0].Gb >> 8U) & 0xfU);
    WData[2] = (UINT8)(pWbGainCtrl[0].B & 0xffU);
    WData[3] = (UINT8)((pWbGainCtrl[0].B >> 8U) & 0xfU);
    WData[4] = (UINT8)(pWbGainCtrl[0].R & 0xffU);
    WData[5] = (UINT8)((pWbGainCtrl[0].R >> 8U) & 0xfU);
    WData[6] = (UINT8)(pWbGainCtrl[0].Gr & 0xffU);
    WData[7] = (UINT8)((pWbGainCtrl[0].Gr >> 8U) & 0xfU);

    RetVal |= RegWrite(pChan, MX01_IMX424_WBGAIN_CF0, WData, 8);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void MX01_IMX424_CheckFineShutter(const UINT16 *pFineShutterD, UINT16 *pFineShutterA)
{
    UINT16 FineShutterD, FineShutterA;
    FineShutterD = *pFineShutterD;
    FineShutterA = FineShutterD;

    if(FineShutterD < 1U) {
        FineShutterA = 1U;
    } else if((FineShutterD > 26U) && (FineShutterD < 131U)) {
        FineShutterA = 26U;
    } else if((FineShutterD > 205U) && (FineShutterD < 481U)) {
        FineShutterA = 205U;
    } else { /* MisraC */ }
    *pFineShutterA = FineShutterA;
}

static UINT32 MX01_IMX424_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    MX01_IMX424_INTEG_REG_s Integ[3] = {0};
    UINT8 WData[6U];
    //UINT16 FineShutterA;

    /* Coarse shutter time (Long exposure) */
    Integ[0].Coarse = pShutterCtrl[0] / 500U;
    Integ[0].Fine = (UINT16)(pShutterCtrl[0] % 500U);
    WData[0] = (UINT8)(Integ[0].Coarse & 0xffU);
    WData[1] = (UINT8)((Integ[0].Coarse >> 8U) & 0xffU);
    WData[2] = (UINT8)((Integ[0].Coarse >> 16U) & 0x01U);
    RetVal |= RegWrite(pChan, MX01_IMX424_INTGL_COARSE, WData, 3U);

    /* Coarse shutter time (Medium exposure) */
    Integ[1].Coarse = pShutterCtrl[1] / 500U;
    Integ[1].Fine = (UINT16)(pShutterCtrl[1] % 500U);
    WData[0] = (UINT8)(Integ[1].Coarse & 0xffU);
    WData[1] = (UINT8)((Integ[1].Coarse >> 8U) & 0xffU);
    WData[2] = (UINT8)((Integ[1].Coarse >> 16U) & 0x01U);
    RetVal |= RegWrite(pChan, MX01_IMX424_INTGM_COARSE, WData, 3U);

    /* Coarse shutter time (Short exposure) */
    Integ[2].Coarse = pShutterCtrl[2] / 500U;
    Integ[2].Fine = (UINT16)(pShutterCtrl[2] % 500U);
    WData[0] = (UINT8)(Integ[2].Coarse & 0xffU);
    WData[1] = (UINT8)((Integ[2].Coarse >> 8U) & 0xffU);
    WData[2] = (UINT8)((Integ[2].Coarse >> 16U) & 0x01U);
    RetVal |= RegWrite(pChan, MX01_IMX424_INTGS_COARSE, WData, 3U);

    /* Fine shutter time (Long, Medium and Short exposures) */
#if 1
    WData[0] = (UINT8)(Integ[0].Fine & 0xffU);
    WData[1] = (UINT8)((Integ[0].Fine >> 8U) & 0x0fU);
    WData[2] = (UINT8)(Integ[1].Fine & 0xffU);
    WData[3] = (UINT8)((Integ[1].Fine >> 8U) & 0x0fU);
    WData[4] = (UINT8)(Integ[2].Fine & 0xffU);
    WData[5] = (UINT8)((Integ[2].Fine >> 8U) & 0x0fU);
    RetVal |= RegWrite(pChan, MX01_IMX424_INTGL_FINE, WData, 6U);
#else
    MX01_IMX424_CheckFineShutter(&Integ[0].Fine, &FineShutterA);
    WData[0] = (UINT8)(FineShutterA & 0xffU);
    WData[1] = (UINT8)((FineShutterA >> 8U) & 0x0fU);
    MX01_IMX424_CheckFineShutter(&Integ[1].Fine, &FineShutterA);
    WData[2] = (UINT8)(FineShutterA & 0xffU);
    WData[3] = (UINT8)((FineShutterA >> 8U) & 0x0fU);
    MX01_IMX424_CheckFineShutter(&Integ[1].Fine, &FineShutterA);
    WData[4] = (UINT8)(FineShutterA & 0xffU);
    WData[5] = (UINT8)((FineShutterA >> 8U) & 0x0fU);
    RetVal |= RegWrite(pChan, MX01_IMX424_INTGL_FINE, WData, 6U);
#endif
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_SetSlowShutterReg
 *
 *  @Description:: Apply slow shutter setting
 *
 *  @Input      ::
 *     IntegrationPeriodInFrame: Number of frames in integation period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    (void) IntegrationPeriodInFrame;
    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_StandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      ::
 *     pChan:      pointer to Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x01;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= RegWrite(pChan, MX01_IMX424_STANDBY, &TxData, 1);
//    AmbaPrint("[MAX9295_96712_IMX424] MX01_IMX424_SetStandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_StandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      ::
 *     pChan:      pointer to Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x00;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= RegWrite(pChan, MX01_IMX424_STANDBY, &TxData, 1);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      ReadoutMode: Sensor readout mode
 *      SensorMode : Sensor mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 i, j, k, DataSize;
    const MX01_IMX424_MODE_REG_s *pModeRegTable = &MX01_IMX424RegTable[0];
    UINT16 FirstAddr, NextAddr;
    UINT8 FirstData, NextData, TxData[MX01_IMX424_I2C_WR_BUF_SIZE];
    UINT32 U32RVal = 0;
    AMBA_SENSOR_CHANNEL_s Chan_t;

    /*
    for (i = 0U; i < MX01_IMX424_NUM_REGTABLE_REG; i += DataSize) {
        DataSize = 0;
        FirstAddr = pModeRegTable[i].Addr;
        FirstData = pModeRegTable[i].Data[ModeID];

        TxData[DataSize] = FirstData;
        DataSize++;

        for (j = i + 1U; j < MX01_IMX424_NUM_REGTABLE_REG; j++) {
            NextAddr = pModeRegTable[j].Addr;
            NextData = pModeRegTable[j].Data[ModeID];

            if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)(MX01_IMX424_I2C_WR_BUF_SIZE))) {
                break;
            } else {
                TxData[DataSize] = NextData;
                DataSize++;
            }
        }

        RetVal |= RegWrite(pChan, FirstAddr, TxData, DataSize);
    }
    */
    Chan_t.VinID = pChan->VinID;
    for (k = 0U; k < MX01_IMX424_NUM_MAX_SENSOR_COUNT; k++) {
        if ((pChan->SensorID & ((UINT32)1U << (4U * (k + 1U)))) != 0U) {
            Chan_t.SensorID = (UINT32)1U << (4U * (k + 1U));
            i = 0U;
            while(i < MX01_IMX424_NUM_REGTABLE_REG) {
                DataSize = 0;
                FirstAddr = pModeRegTable[i].Addr;
                FirstData = pModeRegTable[i].Data[pModeID[k]];

                TxData[DataSize] = FirstData;
                DataSize++;

                for (j = i + 1U; j < MX01_IMX424_NUM_REGTABLE_REG; j++) {
                    NextAddr = pModeRegTable[j].Addr;
                    NextData = pModeRegTable[j].Data[pModeID[k]];

                    if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)(MX01_IMX424_I2C_WR_BUF_SIZE))) {
                        break;
                    } else {
                        if(DataSize < MX01_IMX424_I2C_WR_BUF_SIZE) {
                            TxData[DataSize] = NextData;
                            DataSize++;
                        }
                    }
                }

                RetVal |= RegWrite(&Chan_t, FirstAddr, TxData, DataSize);

                i += DataSize;
            }
        }
    }

    /* Reset current AE information */
    U32RVal |= AmbaWrap_memset(pMX01_IMX424Ctrl[pChan->VinID]->CurrentAEInfo, 0x0, sizeof(MX01_IMX424_AE_INFO_s) * MX01_IMX424_NUM_MAX_SENSOR_COUNT);
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX01_IMX424_PreCalculateModeInfo(void)
{
    const AMBA_VIN_FRAME_RATE_s *pFrameRate;
    UINT8 i;

    for (i = 0U; i < MX01_IMX424_NUM_MODE; i++) {
        pFrameRate = &MX01_IMX424_ModeInfoList[i].FrameRate;

        MX01_IMX424_ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)pFrameRate->NumUnitsInTick /
                                             ((DOUBLE)MX01_IMX424_SensorInfo[i].FrameLengthLines *
                                              (DOUBLE)pFrameRate->TimeScale));

        MX01_IMX424_ModeInfoList[i].PixelRate = (FLOAT)((DOUBLE)MX01_IMX424_OutputInfo[i].DataRate *
                                               (DOUBLE)MX01_IMX424_OutputInfo[i].NumDataLanes /
                                               (DOUBLE)MX01_IMX424_OutputInfo[i].NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE, i;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX424] Init", NULL, NULL, NULL, NULL, NULL);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        MX01_IMX424_PreCalculateModeInfo();

        /* Initialize the pointers to serdes configuration */
        for (i = 0U; i < MX01_IMX424_NUM_CTRL_BUF; i++) {
            MX01_IMX424CtrlBuf[i].pSerdesCfg = &MX01_IMX424SerdesConfig[i];
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX424] Enable", NULL, NULL, NULL, NULL, NULL);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX01_IMX424_ResetSensor(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX424] Disable", NULL, NULL, NULL, NULL, NULL);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX01_IMX424_StandbyOn(pChan);
    }


    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_GetStatus
 *
 *  @Description:: Get current sensor status
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pStatus:    pointer to current sensor status
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &pMX01_IMX424Ctrl[pChan->VinID]->Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_GetModeInfo
 *
 *  @Description:: Get Mode Info of indicated mode
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Mode:   Sensor Readout Mode Number
 *
 *  @Output     ::
 *      pModeInfo: pointer to requested Mode Info
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s SensorConfig;
    UINT32 i;

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        SensorConfig = *pMode;
        SensorConfig.ModeID = MX01_IMX424_GetModeID(pChan, pMode);
        if (SensorConfig.ModeID == AMBA_SENSOR_CURRENT_MODE) {
            for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    SensorConfig.ModeID = pMX01_IMX424Ctrl[pChan->VinID]->Status[i].ModeInfo.Config.ModeID;
                }
            }
        }

        if (SensorConfig.ModeID >= MX01_IMX424_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            RetVal = MX01_IMX424_PrepareModeInfo(pChan, &SensorConfig, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_GetDeviceInfo
 *
 *  @Description:: Get Sensor Device Info
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to device info
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &MX01_IMX424DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_GetHdrInfo
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      *ShutterCtrl:    Electronic shutter control
 *  @Output     ::
 *      pExposureTime:   senosr hdr information
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                if (AmbaWrap_memcpy(pHdrInfo, &pMX01_IMX424Ctrl[pChan->VinID]->Status[i].ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
                    RetVal = SENSOR_ERR_UNEXPECTED;
                }
            }
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_GetCurrentGainFactor
 *
 *  @Description:: Get Sensor Current Gain Factor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to current gain factor
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i, k;
    DOUBLE GainFactor64 = 0.0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &pMX01_IMX424Ctrl[pChan->VinID]->Status[0].ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API;
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    (void)AmbaWrap_pow(10.0, 0.005 * (DOUBLE)pMX01_IMX424Ctrl[pChan->VinID]->CurrentAEInfo[i].AgcCtrl, &GainFactor64);
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pGainFactor[k] = (FLOAT)GainFactor64;
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to current exposure time
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 FrameLengthLines, VMAX;
    UINT32 RetVal = SENSOR_ERR_NONE;
    const UINT32 *pShutterCtrl;
    FLOAT CoarseShutter, FineShutter;
    MX01_IMX424_INTEG_REG_s Integ = {0};
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &pMX01_IMX424Ctrl[pChan->VinID]->Status[0].ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API;
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            FrameLengthLines = pModeInfo->FrameLengthLines;
            VMAX = pModeInfo->NumExposureStepPerFrame / 500U;
            CoarseShutter = pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)VMAX;
            FineShutter = CoarseShutter / 500.0f;
            for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pShutterCtrl = &pMX01_IMX424Ctrl[pChan->VinID]->CurrentAEInfo[i].ShutterCtrl[k];
                        Integ.Coarse = (*pShutterCtrl) / 500U;
                        Integ.Fine = (UINT16)((*pShutterCtrl) % 500U);
                        pExposureTime[k] = ((FLOAT)Integ.Coarse * CoarseShutter) + ((FLOAT)Integ.Fine * FineShutter);
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_ConvertGainFactor
 *
 *  @Description:: Convert gain factor to analog and digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      DesiredFactor:      Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      Achievable gain factor (12.20 fixed point)
 *      pGainCtrl:          Analog/Digital/WB gain control for achievable gain factor
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType;
    DOUBLE DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
    DOUBLE LogDesiredFactor = 1.0;
    FLOAT ActualFactor = 0.0f;
    DOUBLE ActualFactor64 = 0.0;
    DOUBLE AnalogGainCtrlInDb;
    UINT32 AnalogGainCtrl;
    AMBA_SENSOR_WB_GAIN_FACTOR_s WbGain;
    DOUBLE WbGainInDb;
    UINT32 U32RVal = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        HdrType = pMX01_IMX424Ctrl[pChan->VinID]->Status[0].ModeInfo.HdrInfo.HdrType;
        if (DesiredFactor > 31.622776) {
            AnalogGainCtrl = 300U;
            ActualFactor = (FLOAT) 31.622776;
        } else if (DesiredFactor >= 1.0) {
            U32RVal |= AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            U32RVal |= AmbaWrap_floor((200.0 * LogDesiredFactor), &AnalogGainCtrlInDb);
            U32RVal |= AmbaWrap_pow(10.0, 0.005 * AnalogGainCtrlInDb, &ActualFactor64);

            AnalogGainCtrl = (UINT32)AnalogGainCtrlInDb;
            ActualFactor = (FLOAT)ActualFactor64;
        } else {
            AnalogGainCtrl = 0U;
            ActualFactor = 1.0f;
        }

        pActualFactor->Gain[0] = ActualFactor;
        pGainCtrl->AnalogGain[0] = AnalogGainCtrl;

        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            pActualFactor->Gain[1] = ActualFactor;
            pActualFactor->Gain[2] = ActualFactor;

            pGainCtrl->AnalogGain[1] = AnalogGainCtrl;
            pGainCtrl->AnalogGain[2] = AnalogGainCtrl;
        }

        /* WB gain */
        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb >= 15.996094f) ? 15.996094f : pDesiredFactor->WbGain[0].Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B >= 15.996094f)  ? 15.996094f : pDesiredFactor->WbGain[0].B;
        WbGain.R  = (pDesiredFactor->WbGain[0].R >= 15.996094f)  ? 15.996094f : pDesiredFactor->WbGain[0].R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr >= 15.996094f) ? 15.996094f : pDesiredFactor->WbGain[0].Gr;

        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb <= 1.0f) ? 1.0f : WbGain.Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B <= 1.0f)  ? 1.0f : WbGain.B;
        WbGain.R  = (pDesiredFactor->WbGain[0].R <= 1.0f)  ? 1.0f : WbGain.R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr <= 1.0f) ? 1.0f : WbGain.Gr;

        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gb * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].Gb = (UINT32)WbGainInDb;
        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.B * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].B = (UINT32)WbGainInDb;
        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.R * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].R = (UINT32)WbGainInDb;
        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gr * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].Gr = (UINT32)WbGainInDb;

        pActualFactor->WbGain[0].Gb = (FLOAT)pGainCtrl->WbGain[0].Gb / 256.0f;
        pActualFactor->WbGain[0].B  = (FLOAT)pGainCtrl->WbGain[0].B  / 256.0f;
        pActualFactor->WbGain[0].R  = (FLOAT)pGainCtrl->WbGain[0].R  / 256.0f;
        pActualFactor->WbGain[0].Gr = (FLOAT)pGainCtrl->WbGain[0].Gr / 256.0f;

        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            U32RVal |= AmbaWrap_memcpy(&pGainCtrl->WbGain[1], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));
            U32RVal |= AmbaWrap_memcpy(&pGainCtrl->WbGain[2], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));

            U32RVal |= AmbaWrap_memcpy(&pActualFactor->WbGain[1], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
            U32RVal |= AmbaWrap_memcpy(&pActualFactor->WbGain[2], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
        }
        if(U32RVal != 0U) {
            /* MisraC */
        }
    }

    return RetVal;
}

#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ConvertShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      HdrChannel:     0: LEF(HDR mode) or linear mdoe
 *                      1: SEF1(HDR mode),
 *                      2: SEF2(HDR mode),
 *
 *      ExposureTime:   Exposure time / shutter speed
 *
 *  @Output     ::
 *      pShutterCtrl:   Electronic shutter control
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 MaxExposureline, MinExposureline;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ShutterCtrl;
    DOUBLE ShutterTimeUnit, ShutterCtrlInDb;
    UINT32 U32RVal = 0;

    ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

    U32RVal |= AmbaWrap_floor(((DOUBLE)ExposureTime / ShutterTimeUnit), &ShutterCtrlInDb);
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    /* VMAX is 17 bits */
    if (ShutterCtrl > ((0x1ffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame)) {
        ShutterCtrl = ((0x1ffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame);
    }

    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MaxExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
        /* For none-HDR mode */
    } else {
        /* TBD */
        MaxExposureline = (((ShutterCtrl / pModeInfo->NumExposureStepPerFrame) + 1U) * pModeInfo->NumExposureStepPerFrame);
        MinExposureline = 1U;
    }

    if (ShutterCtrl > MaxExposureline) {
        ShutterCtrl = MaxExposureline;
    } else if (ShutterCtrl < MinExposureline) {
        ShutterCtrl = MinExposureline;
    } else {
        /* MisraC */  /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl   = ShutterCtrl;
    *pActualExptime = (FLOAT)((DOUBLE)ShutterCtrl * ShutterTimeUnit);
    if(U32RVal != 0U) {
        /* MisraC */
    }
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ConvertHdrShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      pModeInfo:       pointer to mode info of target mode
 *      pExposureTime:   Desired Exposure time / shutter speed
 *
 *  @Output     ::
 *      pActualExpTime:  Achievable exposure time
 *      pShutterCtrl:    Shutter control for achievable exposure time
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void ConvertHdrShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, const FLOAT *pExposureTime, FLOAT *pActualExpTime, UINT32 *pShutterCtrl)
{
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines, VMAX;
    UINT32 MaxExposurelineL, MinExposurelineL;
    UINT32 MaxExposurelineM, MinExposurelineM;
    UINT32 MaxExposurelineS, MinExposurelineS;
    FLOAT ExpTime = pExposureTime[0];
    FLOAT ExpTimeUpperLimit, ExpTimeLowerLimit, WorkFLOAT;
    FLOAT CoarseShutter, FineShutter;
    UINT32 Rlm, Rms;
    UINT32 TlTmp, Tl, Tm, Ts, MaxTl, MinTl, MaxTm, MinTm, MaxTs, MinTs; /* exposure time in unit of fine shutter */
    MX01_IMX424_INTEG_REG_s IntegL = {0}, IntegM = {0}, IntegS = {0};
    UINT16 FineShutterD, FineShutterA;

    Rlm = MX01_IMX424_EXP_RATE_L_M;
    Rms = MX01_IMX424_EXP_RATE_M_S;

    VMAX = pModeInfo->NumExposureStepPerFrame / 500U;
    CoarseShutter = pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)VMAX;
    FineShutter = CoarseShutter / 500.0f;

    /* desired shutter time, in unit of fine shutter */
    WorkFLOAT = ExpTime / FineShutter;
    TlTmp = (UINT32)WorkFLOAT;

    /* bounds checking */
#if 0
    ExpTimeUpperLimit = (((FLOAT)Rlm * 7.0f / 8.0f) + 1.0f) / 1000.0f;
    WorkFLOAT = ExpTimeUpperLimit / FineShutter;
    MaxTl = (UINT32)WorkFLOAT;
    MinTl = (Rlm * Rms * 500U);
#else
    MaxExposurelineL = pModeInfo->HdrInfo.ChannelInfo[0].MaxExposureLine;
    MinExposurelineL = pModeInfo->HdrInfo.ChannelInfo[0].MinExposureLine;
    MaxExposurelineM = pModeInfo->HdrInfo.ChannelInfo[1].MaxExposureLine;
    MinExposurelineM = pModeInfo->HdrInfo.ChannelInfo[1].MinExposureLine;
    MaxExposurelineS = pModeInfo->HdrInfo.ChannelInfo[2].MaxExposureLine;
    MinExposurelineS = pModeInfo->HdrInfo.ChannelInfo[2].MinExposureLine;

    ExpTimeUpperLimit = (FLOAT)MaxExposurelineL * CoarseShutter;
    WorkFLOAT = ExpTimeUpperLimit / FineShutter;
    MaxTl = (UINT32)WorkFLOAT;
    ExpTimeLowerLimit = (FLOAT)MinExposurelineL * CoarseShutter;
    WorkFLOAT = ExpTimeLowerLimit / FineShutter;
    MinTl = (UINT32)WorkFLOAT;

    ExpTimeUpperLimit = (FLOAT)MaxExposurelineM * CoarseShutter;
    WorkFLOAT = ExpTimeUpperLimit / FineShutter;
    MaxTm = (UINT32)WorkFLOAT;
    ExpTimeLowerLimit = (FLOAT)MinExposurelineM * CoarseShutter;
    WorkFLOAT = ExpTimeLowerLimit / FineShutter;
    MinTm = (UINT32)WorkFLOAT;

    ExpTimeUpperLimit = (FLOAT)MaxExposurelineS * CoarseShutter;
    WorkFLOAT = ExpTimeUpperLimit / FineShutter;
    MaxTs = (UINT32)WorkFLOAT;
    ExpTimeLowerLimit = (FLOAT)MinExposurelineS * CoarseShutter;
    WorkFLOAT = ExpTimeLowerLimit / FineShutter;
    MinTs = (UINT32)WorkFLOAT;

#endif

    if (TlTmp > MaxTl) {
        TlTmp = MaxTl;
    }

    if (TlTmp < MinTl) {
        TlTmp = MinTl;
    }

    /* calculated shuter time, in unit of fine shutter */
#if 0
    Ts = TlTmp / (Rlm * Rms);
    Tm = Ts * Rms;
    Tl = Tm * Rlm;
    IntegL.Coarse = Tl / 500U;
    IntegL.Fine = (UINT16)(Tl % 500U);
    pShutterCtrl[0] = Tl;
    pActualExpTime[0] = ((FLOAT)IntegL.Coarse * CoarseShutter) + ((FLOAT)IntegL.Fine * FineShutter);

    IntegM.Coarse = Tm / 500U;
    IntegM.Fine = (UINT16)(Tm % 500U);
    pShutterCtrl[1] = Tm;
    pActualExpTime[1] = ((FLOAT)IntegM.Coarse * CoarseShutter) + ((FLOAT)IntegM.Fine * FineShutter);

    IntegS.Coarse = Ts / 500U;
    IntegS.Fine = (UINT16)(Ts % 500U);
    pShutterCtrl[2] = Ts;
    pActualExpTime[2] = ((FLOAT)IntegS.Coarse * CoarseShutter) + ((FLOAT)IntegS.Fine * FineShutter);
#else
    Ts = TlTmp / (Rlm * Rms);
    if(Ts < MinTs) {
        Ts = MinTs;
    }
    if(Ts > MaxTs) {
        Ts = MaxTs;
    }
    Tm = TlTmp / Rlm;

    if(Tm < MinTm) {
        Tm = MinTm;
    }
    if(Tm > MaxTm) {
        Tm = MaxTm;
    }
    Tl = TlTmp;

    /*
        IntegL.Coarse = Tl / 500U;
        pShutterCtrl[0] = Tl;
        pActualExpTime[0] = ((FLOAT)IntegL.Coarse * CoarseShutter);

        IntegM.Coarse = Tm / 500U;
        pShutterCtrl[1] = Tm;
        pActualExpTime[1] = ((FLOAT)IntegM.Coarse * CoarseShutter);

        IntegS.Coarse = Ts / 500U;
        pShutterCtrl[2] = Ts;
        pActualExpTime[2] = ((FLOAT)IntegS.Coarse * CoarseShutter);
    */
    IntegL.Coarse = Tl / 500U;
    IntegL.Fine = (UINT16)(Tl % 500U);
    FineShutterD = (UINT16)IntegL.Fine;
    MX01_IMX424_CheckFineShutter(&FineShutterD, &FineShutterA);
    IntegL.Fine = (UINT16)FineShutterA;
    pShutterCtrl[0] = (IntegL.Coarse * 500U) + IntegL.Fine;//Tl;
    pActualExpTime[0] = ((FLOAT)IntegL.Coarse * CoarseShutter) + ((FLOAT)IntegL.Fine * FineShutter);

    IntegM.Coarse = Tm / 500U;
    IntegM.Fine = (UINT16)(Tm % 500U);
    FineShutterD = (UINT16)IntegM.Fine;
    MX01_IMX424_CheckFineShutter(&FineShutterD, &FineShutterA);
    IntegM.Fine = (UINT16)FineShutterA;
    pShutterCtrl[1] = (IntegM.Coarse * 500U) + IntegM.Fine;//Tm;
    pActualExpTime[1] = ((FLOAT)IntegM.Coarse * CoarseShutter) + ((FLOAT)IntegM.Fine * FineShutter);

    IntegS.Coarse = Ts / 500U;
    IntegS.Fine = (UINT16)(Ts % 500U);
    FineShutterD = (UINT16)IntegS.Fine;
    MX01_IMX424_CheckFineShutter(&FineShutterD, &FineShutterA);
    IntegS.Fine = (UINT16)FineShutterA;
    pShutterCtrl[2] = (IntegS.Coarse * 500U) + IntegS.Fine;//Ts;
    pActualExpTime[2] = ((FLOAT)IntegS.Coarse * CoarseShutter) + ((FLOAT)IntegS.Fine * FineShutter);
#endif

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_ConvertShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      ExposureTime:   Exposure time / shutter speed
 *
 *  @Output     ::
 *      pShutterCtrl:   Electronic shutter control
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &pMX01_IMX424Ctrl[pChan->VinID]->Status[0].ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API;
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            ConvertHdrShutterSpeed(pModeInfo, pDesiredExposureTime, pActualExposureTime, pShutterCtrl);
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_SetAnalogGainCtrl
 *
 *  @Description:: Set analog gain control
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pAnalogGainCtrl: Analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 i;
#if !defined(CONFIG_IMX424_RCCB_PSLIN_DISABLE)
    UINT32 Gain_thr = 30U; //20210708
    UINT8 WData[4];
#endif
    if ((pChan == NULL) || (pAnalogGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &pMX01_IMX424Ctrl[pChan->VinID]->Status[0].ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API; /* TBD */
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX01_IMX424_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);
            //AmbaPrint_PrintUInt5("Gain = %d", pAnalogGainCtrl[0], 0U, 0U, 0U, 0U);
#if !defined(CONFIG_IMX424_RCCB_PSLIN_DISABLE)
            if (pAnalogGainCtrl[0] > Gain_thr) {//20210615
                //AmbaPrint_PrintUInt5( "Long~~~~~~~~~~", 0U, 0U, 0U, 0U, 0U);

                /* LONG exp only */
                WData[0] = (UINT8)(0xFFU);
                WData[1] = (UINT8)(0x0FU);
                WData[2] = (UINT8)(0xFFU);
                WData[3] = (UINT8)(0x0FU);
                RetVal |= RegWrite(pChan, 0x814A, WData, 4);

                WData[0] = (UINT8)(0xFFU);
                WData[1] = (UINT8)(0x0FU);
                WData[2] = (UINT8)(0xFFU);
                WData[3] = (UINT8)(0x0FU);
                RetVal |= RegWrite(pChan, 0x815A, WData, 4);
            } else {//20210615
                //AmbaPrint_PrintUInt5("original", 0U, 0U, 0U, 0U, 0U);
                /* Original 3-exp */
                WData[0] = (UINT8)(0x6CU);
                WData[1] = (UINT8)(0x07U);
                WData[2] = (UINT8)(0xA0U);
                WData[3] = (UINT8)(0x0FU);
                RetVal |= RegWrite(pChan, 0x814A, WData, 4);

                WData[0] = (UINT8)(0x98U);
                WData[1] = (UINT8)(0x08U);
                WData[2] = (UINT8)(0xA0U);
                WData[3] = (UINT8)(0x0FU);
                RetVal |= RegWrite(pChan, 0x815A, WData, 4);
            }//20210615
#endif
            /* Update current analog gain control */
            for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    pMX01_IMX424Ctrl[pChan->VinID]->CurrentAEInfo[i].AgcCtrl = pAnalogGainCtrl[0];
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_SetWbGainCtrl
 *
 *  @Description:: Set white balance gain control
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pWbGainCtrl:     Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 i;

    if ((pChan == NULL) || (pWbGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &pMX01_IMX424Ctrl[pChan->VinID]->Status[0].ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API; /* TBD */
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX01_IMX424_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    pMX01_IMX424Ctrl[pChan->VinID]->CurrentAEInfo[i].WbCtrl = pWbGainCtrl[0];
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_SetShutterCtrl
 *
 *  @Description:: set shutter control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      ShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 i, k;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &pMX01_IMX424Ctrl[pChan->VinID]->Status[0].ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API; /* TBD */
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX01_IMX424_SetShutterReg(pChan, pShutterCtrl);

            /* Update current shutter control */
            for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pMX01_IMX424Ctrl[pChan->VinID]->CurrentAEInfo[i].ShutterCtrl[k] = pShutterCtrl[k];
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_SetSlowShrCtrl
 *
 *  @Description:: set slow shutter control
 *
 *  @Input      ::
 *      pChan:           Vin ID and sensor ID
 *      SlowShutterCtrl:   Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_SetSlowShrCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (SlowShutterCtrl < 1U)) {
        RetVal = SENSOR_ERR_ARG;
    } else if (pMX01_IMX424Ctrl[pChan->VinID]->Status[0].ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        // RetVal = SENSOR_ERR_INVALID_API;
        RetVal = MX01_IMX424_SetSlowShutterReg(SlowShutterCtrl);
    }

    return RetVal;
}

#ifdef MX01_IMX424_VC_IN_SLAVE_MODE
static UINT32 MX01_IMX424_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal;
    DOUBLE PeriodInDb;
    const MX01_IMX424_MODE_INFO_s *pModeInfo = &MX01_IMX424_ModeInfoList[ModeID];
    const MX01_IMX424_SENSOR_INFO_s *pSensorInfo = &MX01_IMX424_SensorInfo[ModeID];
    static AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncConfig = {
        .RefClk = 0,
        .HSync = {
            .Period     = 0,
            .PulseWidth = 0,
            .Polarity   = 0
        },
        .VSync = {
            .Period     = 0,
            .PulseWidth = 0,
            .Polarity   = 0
        },
        .HSyncDelayCycles = 0,
        .VSyncDelayCycles = 0,
        .ToggleHsyncInVblank = 1
    };
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_floor(((DOUBLE)pModeInfo->FrameRate.NumUnitsInTick * ((DOUBLE) MX01_IMX424_SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameRate.TimeScale)) + 0.5, &PeriodInDb);

    MasterSyncConfig.RefClk = pSensorInfo->InputClk;
    MasterSyncConfig.HSync.Period      = (UINT32)PeriodInDb;
    MasterSyncConfig.HSync.PulseWidth  = 8U;
    MasterSyncConfig.HSync.Polarity    = 0U;
    MasterSyncConfig.VSync.Period      = 1U;
    MasterSyncConfig.VSync.PulseWidth  = 1000U;
    MasterSyncConfig.VSync.Polarity    = 0U;
    MasterSyncConfig.ToggleHsyncInVblank  = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(MX01_IMX424MasterSyncChannel[pChan->VinID], &MasterSyncConfig);

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "MasterSync RefClk(%u) Hsync Period(%u)", pSensorInfo->InputClk, (UINT32)PeriodInDb, 0U, 0U, 0U);
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

#endif

static UINT32 MX01_IMX424_GetSerdesStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_SERDES_STATUS_s *pSerdesStatus)
{
    UINT8 i;
    UINT32 RetVal = SENSOR_ERR_NONE;

    MAX9295_96712_SERDES_STATUS_s SerdesStatus[MX01_IMX424_NUM_MAX_SENSOR_COUNT];
    RetVal |= Max9295_96712_GetSerdesStatus(pChan->VinID, MX01_IMX424_GetEnabledLinkID(pChan->SensorID), SerdesStatus);
    for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
        pSerdesStatus[i].LinkLock = SerdesStatus[i].LinkLock;
        pSerdesStatus[i].VideoLock = SerdesStatus[i].VideoLock;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX424_Config
 *
 *  @Description:: Set sensor to indicated mode
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Mode:   Sensor Readout Mode Number
 *      ElecShutterMode:    Operating Mode of the Electronic Shutter
 *
 *  @Output     ::
 *      pModeInfo: pointer to mode info of target mode
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX424_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
#if 0
    AMBA_VIN_MIPI_PAD_CONFIG_s MX01_IMX424PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    AMBA_VIN_MIPI_VC_CONFIG_s MipiVirtChanConfig = {
        .VirtChan     = 0x0U,
        .VirtChanMask = 0x0U,
    };
    const UINT8 LaneShift[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0]  = 0U,
        [AMBA_VIN_CHANNEL1]  = 0U,
        [AMBA_VIN_CHANNEL2]  = 0U,
        [AMBA_VIN_CHANNEL3]  = 0U,
        [AMBA_VIN_CHANNEL4]  = 4U,
        [AMBA_VIN_CHANNEL5]  = 4U,
        [AMBA_VIN_CHANNEL6]  = 4U,
        [AMBA_VIN_CHANNEL7]  = 4U,
        [AMBA_VIN_CHANNEL8]  = 0U,
        [AMBA_VIN_CHANNEL9]  = 0U,
        [AMBA_VIN_CHANNEL10] = 0U,
        [AMBA_VIN_CHANNEL11] = 4U,
        [AMBA_VIN_CHANNEL12] = 4U,
        [AMBA_VIN_CHANNEL13] = 4U,
    };
#endif

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo[MX01_IMX424_NUM_MAX_SENSOR_COUNT];
    UINT32 ModeID[MX01_IMX424_NUM_MAX_SENSOR_COUNT] = {0xffffffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU};
#if 0
    UINT32 FrameTimeInMs = 0U;
    UINT32 WorkUINT32 = 0U;
#endif
    UINT32 i;
    AMBA_SENSOR_CHANNEL_s Chan_t = {0};
#ifdef MX01_IMX424_VC_IN_SLAVE_MODE
//#ifdef CONFIG_VIN_ONE_MASTER_SYNC_SUPPORT
    static UINT8 Init = 0U;
//#endif
#endif
#if 0
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg = pMX01_IMX424Ctrl[pChan->VinID]->pSerdesCfg;
#endif

    ModeID[0] = pMode->ModeID;
    ModeID[1] = pMode->ModeID_1;
    ModeID[2] = pMode->ModeID_2;
    ModeID[3] = pMode->ModeID_3;

    if ((((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_A) != 0U) && (pMode->ModeID >= MX01_IMX424_NUM_MODE)) ||
        (((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_B) != 0U) && (pMode->ModeID_1 >= MX01_IMX424_NUM_MODE)) ||
        (((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_C) != 0U) && (pMode->ModeID_2 >= MX01_IMX424_NUM_MODE)) ||
        (((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_D) != 0U) && (pMode->ModeID_3 >= MX01_IMX424_NUM_MODE))) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  [ MAX9295_96712_IMX424_Config VC ]  ============", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d %d %d %d ============", pMode->ModeID, pMode->ModeID_1, pMode->ModeID_2, pMode->ModeID_3, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

#if 0
        RetVal |= MX01_IMX424_UpdateSerdesCSITxConfig(pChan, pMode);
#endif

        for (i = 0U; i < MX01_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
            pModeInfo[i] = &pMX01_IMX424Ctrl[pChan->VinID]->Status[i].ModeInfo;

            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                Chan_t.VinID = pChan->VinID;
                Chan_t.SensorID = (UINT32)1U << (4U * (i + 1U));

                /* update status */
                RetVal |= MX01_IMX424_PrepareModeInfo(&Chan_t, pMode, pModeInfo[i]);
#if 0
                WorkUINT32 = (1000U * pModeInfo[i]->FrameRate.NumUnitsInTick) / pModeInfo[i]->FrameRate.TimeScale;
                WorkUINT32++;

                if (FrameTimeInMs < WorkUINT32) {
                    FrameTimeInMs = WorkUINT32;
                }
#endif
            }
        }

#ifdef MX01_IMX424_VC_IN_SLAVE_MODE
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "MX01_IMX424_VC_IN_SLAVE_MODE", 0U, 0U, 0U, 0U, 0U);
//#ifdef CONFIG_VIN_ONE_MASTER_SYNC_SUPPORT
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "CONFIG_VIN_ONE_MASTER_SYNC_SUPPORT", 0U, 0U, 0U, 0U, 0U);
        if (Init == 0) {
//#endif
            (void)AmbaVIN_MasterSyncDisable(MX01_IMX424MasterSyncChannel[pChan->VinID]);
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX01_IMX424_MasterSyncDisable] Vin%d", pChan->VinID, 0U, 0U, 0U, 0U);
//#ifdef CONFIG_VIN_ONE_MASTER_SYNC_SUPPORT
        }
//#endif
#endif

#if 0
        /* Adjust mipi-phy parameters */
        MX01_IMX424PadConfig.DateRate = (UINT64) pSerdesCfg->CSITxSpeed[0] * 100000000U;
        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", MX01_IMX424PadConfig.DateRate, 0U, 0U, 0U, 0U);
        if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_A) != 0U) {
            MX01_IMX424PadConfig.EnabledPin = (((UINT32)1U << pModeInfo[0]->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
            RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX01_IMX424PadConfig);
        }
        if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_B) != 0U) {
            MX01_IMX424PadConfig.EnabledPin = (((UINT32)1U << pModeInfo[1]->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
            RetVal |= AmbaVIN_MipiReset(pChan->VinID+1U, &MX01_IMX424PadConfig);
        }
        if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_C) != 0U) {
            MX01_IMX424PadConfig.EnabledPin = (((UINT32)1U << pModeInfo[2]->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
            RetVal |= AmbaVIN_MipiReset(pChan->VinID+2U, &MX01_IMX424PadConfig);
        }
        if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_D) != 0U) {
            MX01_IMX424PadConfig.EnabledPin = (((UINT32)1U << pModeInfo[3]->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
            RetVal |= AmbaVIN_MipiReset(pChan->VinID+3U, &MX01_IMX424PadConfig);
        }

        /* After reset VIN, Set MAX96712A PWDNB high */
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"PWDN LOW", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(3);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"PWDN HIGH", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"SensorID 0x%x, LinkID 0x%x", pChan->VinID, MX01_IMX424_GetEnabledLinkID(pChan->SensorID), 0U, 0U, 0U);
        RetVal |= Max9295_96712_Init(pChan->VinID, MX01_IMX424_GetEnabledLinkID(pChan->SensorID));

        MX01_IMX424_ConfigSerDes(pChan, ModeID);
#endif

        (void)MX01_IMX424_SetSensorClock(pChan, pModeInfo);

        (void)MX01_IMX424_ResetSensor(pChan);
        // (void) AmbaKAL_TaskSleep(100);

        /* program sensor */
        // AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "program sensor", NULL, NULL, NULL, NULL, NULL);
        (void)MX01_IMX424_ChangeReadoutMode(pChan, ModeID);

#ifdef MX01_IMX424_VC_IN_SLAVE_MODE
        {
            UINT8 Data;
            Data = 0xC0U;
            RetVal |= RegWrite(pChan, 0x0091U, &Data, 1U);

            //Set MASK_FRMNUM_INIT1=1,to avoid invalid first 2 frame of IMX424 when using External pulse-based Sync
            Data = 0x01U;
            RetVal |= RegWrite(pChan, 0x0003U, &Data, 1U);
        }
#endif
        RetVal |= MX01_IMX424_StandbyOff(pChan);
#ifdef MX01_IMX424_VC_IN_SLAVE_MODE

//#ifdef CONFIG_VIN_ONE_MASTER_SYNC_SUPPORT
        if (Init == 0U) {
//#endif
            RetVal |= MX01_IMX424_ConfigMasterSync(pChan, ModeID[0]);
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX01_IMX424_ConfigMasterSync] Vin%d", pChan->VinID, 0U, 0U, 0U, 0U);
//#ifdef CONFIG_VIN_ONE_MASTER_SYNC_SUPPORT
            Init = 1U;
        }
//#endif

#endif

#if 0
        /* config vin */
        if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_A) != 0U) {
            RetVal |= MX01_IMX424_ConfigVin(pChan->VinID, pModeInfo[0]);
            MipiVirtChanConfig.VirtChan = 0x0U;
            RetVal |= AmbaVIN_MipiVirtChanConfig(pChan->VinID, &MipiVirtChanConfig);
        }
        if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_B) != 0U) {
            RetVal |= MX01_IMX424_ConfigVin(pChan->VinID+1U, pModeInfo[1]);
            MipiVirtChanConfig.VirtChan = 0x1U;
            RetVal |= AmbaVIN_MipiVirtChanConfig(pChan->VinID+1U, &MipiVirtChanConfig);
        }
        if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_C) != 0U) {
            RetVal |= MX01_IMX424_ConfigVin(pChan->VinID+2U, pModeInfo[2]);
            MipiVirtChanConfig.VirtChan = 0x2U;
            RetVal |= AmbaVIN_MipiVirtChanConfig(pChan->VinID+2U, &MipiVirtChanConfig);
        }
        if((pChan->SensorID & MX01_IMX424_SENSOR_ID_CHAN_D) != 0U) {
            RetVal |= MX01_IMX424_ConfigVin(pChan->VinID+3U, pModeInfo[3]);
            MipiVirtChanConfig.VirtChan = 0x3U;
            RetVal |= AmbaVIN_MipiVirtChanConfig(pChan->VinID+3U, &MipiVirtChanConfig);
        }
#endif
    }

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX01_IMX424_Config] RetVal: 0x%08x", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

#if 0
static UINT32 MX01_IMX424_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    MX01_IMX424_SetStandbyOff();

    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
static AMBA_SENSOR_OBJ_s MX01_IMX424_VC_Obj = {
    .SensorName             = "IMX424",
    .SerdesName             = "MAX9295_96712",
    .Init                   = MX01_IMX424_Init,
    .Enable                 = MX01_IMX424_Enable,
    .Disable                = MX01_IMX424_Disable,
    .Config                 = MX01_IMX424_Config,
    .GetStatus              = MX01_IMX424_GetStatus,
    .GetModeInfo            = MX01_IMX424_GetModeInfo,
    .GetDeviceInfo          = MX01_IMX424_GetDeviceInfo,
    .GetHdrInfo             = MX01_IMX424_GetHdrInfo,
    .GetCurrentGainFactor   = MX01_IMX424_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX01_IMX424_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX01_IMX424_ConvertGainFactor,
    .ConvertShutterSpeed    = MX01_IMX424_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX01_IMX424_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = NULL,
    .SetWbGainCtrl          = MX01_IMX424_SetWbGainCtrl,
    .SetShutterCtrl         = MX01_IMX424_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX01_IMX424_SetSlowShrCtrl,

#ifdef MX01_IMX424_VC_IN_SLAVE_MODE
    .SetMasterSyncCtrl      = NULL,
#else
    .SetMasterSyncCtrl      = NULL,
#endif

    .RegisterRead           = MX01_IMX424_RegisterRead,
    .RegisterWrite          = MX01_IMX424_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = MX01_IMX424_GetSerdesStatus,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};

static UINT32 MX01_IMX424_GetSensorInfo(UINT32 ModeID, MX01_WRAPPER_SENSOR_INFO_s *pMx01SensorInfo)
{
    const MX01_IMX424_SENSOR_INFO_s *pSensorInfo = &MX01_IMX424_SensorInfo[ModeID];

    pMx01SensorInfo->InputClk = pSensorInfo->InputClk;
    pMx01SensorInfo->DataRate = pSensorInfo->DataRate;
    pMx01SensorInfo->NumDataBits = pSensorInfo->NumDataBits;
    pMx01SensorInfo->NumDataLanes = pSensorInfo->NumDataLanes;

    return SENSOR_ERR_NONE;
}

static UINT32 MX01_IMX424_GetFrameRateInfo(UINT32 ModeID, AMBA_VIN_FRAME_RATE_s *pMx01FrameTime)
{
    pMx01FrameTime->Interlace = MX01_IMX424_ModeInfoList[ModeID].FrameRate.Interlace;
    pMx01FrameTime->TimeScale = MX01_IMX424_ModeInfoList[ModeID].FrameRate.TimeScale;
    pMx01FrameTime->NumUnitsInTick = MX01_IMX424_ModeInfoList[ModeID].FrameRate.NumUnitsInTick;

    return SENSOR_ERR_NONE;
}

static UINT32 MX01_IMX424_GetSerdesCfgAddr(const AMBA_SENSOR_CHANNEL_s *pChan, ULONG *pAddr)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    ULONG Addr;
    const MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pSerdesCfg = pMX01_IMX424Ctrl[pChan->VinID]->pSerdesCfg;
        AmbaMisra_TypeCast(&Addr, &pSerdesCfg);
        *pAddr = Addr;
    }

    return RetVal;
}

static UINT32 MX01_IMX424_ConfigVinPre(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pMode);

    return SENSOR_ERR_NONE;
}

static UINT32 MX01_IMX424_ConfigVinPost(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pMode);

    return SENSOR_ERR_NONE;
}

MX01_SENSOR_OBJ_s MX01_IMX424_Obj = {
    .pSensorObj = &MX01_IMX424_VC_Obj,
    .pOutputInfo = &MX01_IMX424_OutputInfo[0],
    .GetSensorInfo = MX01_IMX424_GetSensorInfo,
    .GetFrameRateInfo = MX01_IMX424_GetFrameRateInfo,
    .GetSerdesCfgAddr = MX01_IMX424_GetSerdesCfgAddr,
    .ConfigVinPre = MX01_IMX424_ConfigVinPre,
    .ConfigVin = MX01_IMX424_ConfigVin,
    .ConfigVinPost = MX01_IMX424_ConfigVinPost,
};
