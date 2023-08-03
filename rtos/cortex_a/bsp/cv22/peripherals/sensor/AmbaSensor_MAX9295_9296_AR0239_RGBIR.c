/**
 *  @file AmbaSensor_MAX9295_9296_AR0239_RGBIR.c
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
 *  @details Control APIs of MAXIM 9295/9296 serdes plus OnSemi AR0239 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_AR0239_RGBIR.h"
#include "AmbaSbrg_Max9295_9296.h"

#include "bsp.h"


#define MX00_AR0239_IN_SLAVE_MODE

/*-----------------------------------------------------------------------------------------------*\
 * MX00_AR0239 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX00_AR0239_CTRL_s MX00_AR0239Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};



/*#ifdef MX00_AR0239_IN_SLAVE_MODE
static UINT32 MX00_AR0239MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP
};
#endif*/

static MAX9295_9296_SERDES_CONFIG_s MX00_AR0239SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_AR0239_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_AR0239_I2C_SLAVE_ADDRESS + 0x10U),
            [1] = (UINT8)(MX00_AR0239_I2C_SLAVE_ADDRESS + 0x12U),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [1] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_AR0239_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_AR0239_I2C_SLAVE_ADDRESS + 0x14U),
            [1] = (UINT8)(MX00_AR0239_I2C_SLAVE_ADDRESS + 0x16U),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
};

static UINT16 MX00_AR0239_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 i, EnabledLinkID = 0U;

    for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_GetNumActiveSensor
 *
 *  @Description:: Get active sensor numbers
 *
 *  @Input      ::
 *      SensorID:      sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32:    Sensor numbers
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U;
    UINT32 i;

    for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            SensorNum ++;
        }
    }

    return (SensorNum > 1U) ? SensorNum : 1U;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *      pSensorPrivate: Sensor frame time configuration
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_AR0239_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 SensorMode = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s *pInputInfo = &MX00_AR0239_InputInfo[SensorMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &MX00_AR0239_OutputInfo[SensorMode];
    const MX00_AR0239_SENSOR_INFO_s *pSensorInfo = &MX00_AR0239_SensorInfo[SensorMode];
    const MX00_AR0239_FRAME_TIMING_s *pFrameTime = &MX00_AR0239_ModeInfoList[SensorMode].FrameTime;
    UINT32 SensorNum = MX00_AR0239_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->FrameLengthLines = pSensorInfo->FrameLengthLines * SensorNum;
    if (MX00_AR0239_HdrInfo[SensorMode].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
        pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines/2U;
    } else {
        pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    }
    pModeInfo->InternalExposureOffset = 0.0f;
    pModeInfo->RowTime = MX00_AR0239_ModeInfoList[SensorMode].RowTime / (FLOAT)SensorNum;
    U32RVal |= AmbaWrap_floor((((DOUBLE)MX00_AR0239_ModeInfoList[SensorMode].PixelRate * (DOUBLE)pModeInfo->RowTime) + 0.5), &FloorVal);
    pModeInfo->LineLengthPck = (UINT32)FloorVal;
    pModeInfo->InputClk = pSensorInfo->InputClk;

    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX00_AR0239_HdrInfo[SensorMode], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information


    /* update for MAX9296 CSI-2 output */
    pModeInfo->OutputInfo.DataRate *= SensorNum;
    pModeInfo->OutputInfo.OutputHeight *= (UINT16)SensorNum;
    pModeInfo->OutputInfo.RecordingPixels.StartY *= (UINT16)SensorNum;
    pModeInfo->OutputInfo.RecordingPixels.Height *= (UINT16)SensorNum;

    /* Updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        pModeInfo->MinFrameRate.NumUnitsInTick *= 8U;
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }
}

#ifdef MX00_AR0239_IN_SLAVE_MODE
/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: MX00_AR0239_ConfigMasterSync
*
*  @Description:: Configure Master Sync generation
*
*  @Input      ::
*      pFrameTime: Sensor frame time configuration
*
*  @Output     :: none
*
*  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_ConfigMasterSync(UINT32 ModeID)
{
    UINT32 RetVal;
    DOUBLE PeriodInDb;
    const MX00_AR0239_MODE_INFO_s *pModeInfo = &MX00_AR0239_ModeInfoList[ModeID];
    const MX00_AR0239_SENSOR_INFO_s *pSensorInfo = &MX00_AR0239_SensorInfo[ModeID];
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncConfig = {0};
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_floor(((DOUBLE)pModeInfo->FrameTime.FrameRate.NumUnitsInTick * ((DOUBLE) MX00_AR0239_SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameTime.FrameRate.TimeScale)) + 0.5, &PeriodInDb);

    MasterSyncConfig.RefClk = pSensorInfo->InputClk;
    MasterSyncConfig.HSync.Period      = (UINT32)PeriodInDb;
    MasterSyncConfig.HSync.PulseWidth  = 8U;
    MasterSyncConfig.HSync.Polarity    = 1U;
    MasterSyncConfig.VSync.Period      = 1U;
    MasterSyncConfig.VSync.PulseWidth  = 1000U;
    MasterSyncConfig.VSync.Polarity    = 1U;
    MasterSyncConfig.ToggleHsyncInVblank  = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(AMBA_VIN_MSYNC0, &MasterSyncConfig);

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "MasterSync RefClk(%u) Hsync Period(%u)", pSensorInfo->InputClk, (UINT32)PeriodInDb, 0U, 0U, 0U);
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output frames of the new readout mode
 *
 *  @Input      ::
 *      VinChanNo:  Vin channel number
 *      pModeInfo:  Details of the specified readout mode
 *      pFrameTime: Sensor frame time configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX00_AR0239VinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace         = AMBA_VIN_COLOR_SPACE_RGB,
            .BayerPattern       = AMBA_VIN_BAYER_PATTERN_GR,
            .NumDataBits        = 0,
            .NumSkipFrame       = 1, /* wait for initialization period */
            .RxHvSyncCtrl = {
                .NumActivePixels    = 0,
                .NumActiveLines     = 0,
                .NumTotalPixels     = 0,
                .NumTotalLines      = 0,
            },
            .SplitCtrl          = {0},
            .DelayedVsync       = 0,
        },
        .NumActiveLanes         = 4U,
        .DataType               = 0x00U,
        .DataTypeMask           = 0x3fU,
//#if defined(CONFIG_SENSOR_VCHDR_SUPPORT)
        .VirtChanHDREnable      = 0U,
        .VirtChanHDRConfig      = {0},
//#endif
    };

    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s *pVinCfg = &MX00_AR0239VinConfig;
    UINT32 RetVal;
    UINT32 U32RVal = 0;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Config.NumDataBits = pImgInfo->NumDataBits;
    pVinCfg->Config.BayerPattern = pImgInfo->BayerPattern;

    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    U32RVal |= AmbaWrap_memcpy(&pVinCfg->Config.FrameRate, &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    RetVal = AmbaVIN_MipiConfig(pChan->VinID, pVinCfg);

    if (RetVal == VIN_ERR_NONE) {
        RetVal = SENSOR_ERR_NONE;
    } else {
        RetVal = SENSOR_ERR_INVALID_API;
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

static void MX00_AR0239_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const MX00_AR0239_SENSOR_INFO_s *pSensorInfo = &MX00_AR0239_SensorInfo[SensorMode];
    MAX9295_9296_SERDES_CONFIG_s *pSerdesCfg = &MX00_AR0239SerdesConfig[pChan->VinID];
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    pSerdesCfg->EnabledLinkID = MX00_AR0239_GetEnabledLinkID(pChan->SensorID);
    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;

    U32RVal |= AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal);
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    (void) Max9295_9296_Config(pChan->VinID, &MX00_AR0239SerdesConfig[pChan->VinID]);
    if(U32RVal != 0U) {
        /* MisraC */
    }
}

static UINT32 MX00_AR0239_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan->SensorID & MX00_AR0239_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    if ((pChan->SensorID & MX00_AR0239_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
    if ((pChan->SensorID & MX00_AR0239_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    if ((pChan->SensorID & MX00_AR0239_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start >= 20us*/

    if (RetVal != MAX9295_9296_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }

    return RetVal;
}

static UINT32 MX00_AR0239_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pRxData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8  TxDataBuf[2];
    UINT8  RxDataBuf[2];
    UINT32 TxSize;
    static const UINT32 MX00_AR0239I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
        [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP
    };

    if ((pChan == NULL) ||
        ((pChan->SensorID != MX00_AR0239_SENSOR_ID_CHAN_A) && (pChan->SensorID != MX00_AR0239_SENSOR_ID_CHAN_B))) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cTxConfig.DataSize = 2U;
        I2cTxConfig.pDataBuf = TxDataBuf;

        TxDataBuf[0] = (UINT8)(Addr >> 8U);    /* Register Address [15:8] */
        TxDataBuf[1] = (UINT8)(Addr & 0xffU);  /* Register Address [7:0]  */

        I2cRxConfig.DataSize = 2U;
        I2cRxConfig.pDataBuf = RxDataBuf;

        if (pChan->SensorID == MX00_AR0239_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX00_AR0239SerdesConfig[pChan->VinID].SensorAliasID[0];
            I2cRxConfig.SlaveAddr = (MX00_AR0239SerdesConfig[pChan->VinID].SensorAliasID[0] | (UINT32)0x1U);
        } else {
            /* pChan->SensorID == MX00_AR0239_SENSOR_ID_CHAN_B */
            I2cTxConfig.SlaveAddr = MX00_AR0239SerdesConfig[pChan->VinID].SensorAliasID[1];
            I2cRxConfig.SlaveAddr = (MX00_AR0239SerdesConfig[pChan->VinID].SensorAliasID[1] | (UINT32)0x1U);
        }

        RetVal = AmbaI2C_MasterReadAfterWrite(MX00_AR0239I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U,
                                              &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C read does not work (SlaveID=0x%02x)!!!!!", I2cTxConfig.SlaveAddr, 0, 0, 0, 0);
        } else {
            RetVal = SENSOR_ERR_NONE;
        }

        *pRxData = (UINT16)RxDataBuf[0] << 8U;
        *pRxData |= RxDataBuf[1];
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_RegisterRead
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
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16* Data)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_AR0239_RegRead(pChan, Addr, Data);
    }

    return RetVal;
}

static UINT32 MX00_AR0239_RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 k;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX00_AR0239_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || ((pChan->SensorID & (MX00_AR0239_SENSOR_ID_CHAN_A | MX00_AR0239_SENSOR_ID_CHAN_B)) == 0x0U)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.DataSize = 4U;
        I2cConfig.pDataBuf = TxDataBuf;

        TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);
        TxDataBuf[2] = (UINT8)((Data & 0xff00U) >> 8U);
        TxDataBuf[3] = (UINT8)(Data & 0x00ffU);

        if (pChan->SensorID == (MX00_AR0239_SENSOR_ID_CHAN_A | MX00_AR0239_SENSOR_ID_CHAN_B)) {    /* broadcast to all sensors */
            I2cConfig.SlaveAddr = MX00_AR0239SerdesConfig[pChan->VinID].SensorSlaveID;
            RetVal = AmbaI2C_MasterWrite(AMBA_SENSOR_I2C_CHANNEL, AMBA_I2C_SPEED_FAST,
                                         &I2cConfig, &TxSize, 1000U);
        } else {
            for (k = 0U; k < MX00_AR0239_NUM_MAX_SENSOR_COUNT; k++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (k + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_AR0239SerdesConfig[pChan->VinID].SensorAliasID[k];
                    RetVal = AmbaI2C_MasterWrite(AMBA_SENSOR_I2C_CHANNEL, AMBA_I2C_SPEED_FAST,
                                                 &I2cConfig, &TxSize, 1000U);
                }
            }
        }

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX00_AR0239] SlaveID 0x%02x Addr 0x%02x Data 0x%02x I2C does not work!!!!!", I2cConfig.SlaveAddr, Addr, Data, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_RegisterWrite
 *
 *  @Description:: Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_AR0239_RegWrite(pChan, Addr, Data);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_SoftwareReset
 *
 *  @Description:: Reset (SW) MX00_AR0239 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_SoftwareReset(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = 0;


#ifdef MX00_AR0239_IN_SLAVE_MODE
    RetVal |= MX00_AR0239_RegWrite(pChan, 0x301A, 0x2058);
#else
    RetVal |= MX00_AR0239_RegWrite(pChan, 0x301A, 0x0001);
#endif
    /* R0x301A: software reset */

    /* Wait for internal initialization */
    (void) AmbaKAL_TaskSleep(200);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 i;
    UINT16 Addr;
    UINT16 Data;
    UINT32 RetVal = SENSOR_ERR_NONE;

    for (i = 0U; i < MX00_AR0239_NUM_READOUT_MODE_REG; i++) {
        Addr = MX00_AR0239_ModeRegTable[i].Addr;
        Data = MX00_AR0239_ModeRegTable[i].Data[SensorMode];
        RetVal |= MX00_AR0239_RegWrite(pChan, Addr, Data);
    }

    for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i++) {
        //if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
        /* reset gain/shutter ctrl information */
        MX00_AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][0] = 0xffffffffU;
        MX00_AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][1] = 0xffffffffU;
        MX00_AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = 0xffffffffU;
        MX00_AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][1] = 0xffffffffU;
        MX00_AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][2] = 0xffffffffU;
        MX00_AR0239Ctrl[pChan->VinID].CurrentDgcCtrl[i][0] = 0U;
        MX00_AR0239Ctrl[pChan->VinID].CurrentDgcCtrl[i][1] = 0U;
        MX00_AR0239Ctrl[pChan->VinID].CurrentDgcCtrl[i][2] = 0U;
        //}
    }

    return RetVal;
}


static UINT32 MX00_AR0239_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ShutterCtrl)
{
    UINT16 Data = (UINT16)ShutterCtrl;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= MX00_AR0239_RegWrite(pChan, 0x3012, Data);

    return RetVal;
}

static UINT32 MX00_AR0239_ConfigLinearSeq(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;

    for (UINT32 i = 0U; i < MX00_AR0239_NUM_LINEAR_SEQ_REG; i++) {
        Addr = MX00_AR0239_LinearSeqRegTable[i].Addr;
        Data = MX00_AR0239_LinearSeqRegTable[i].Data;
        RetVal |= MX00_AR0239_RegWrite(pChan, Addr, Data);
    }

    return RetVal;
}

static UINT32 MX00_AR0239_ConfigRecomEsSeq(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;

    for (UINT32 i = 0U; i < MX00_AR0239_NUM_RECOM_ES_REG; i++) {
        Addr = MX00_AR0239_RecomESRegTable[i].Addr;
        Data = MX00_AR0239_RecomESRegTable[i].Data;
        RetVal |= MX00_AR0239_RegWrite(pChan, Addr, Data);
    }

    return RetVal;
}

static UINT32 MX00_AR0239_ConfigPllSeq(const AMBA_SENSOR_CHANNEL_s *pChan,UINT32 Mode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;
    if((Mode == MX00_AR0239_1920_1080_2LANE_A30P) || (Mode == MX00_AR0239_1920_1080_2LANE_A15P)) {
        for (UINT32 i = 0U; i < MX00_AR0239_NUM_PLL_REG; i++) {
            Addr = MX00_AR0239_2LANE_PllRegTable[i].Addr;
            Data = MX00_AR0239_2LANE_PllRegTable[i].Data;
            RetVal |= MX00_AR0239_RegWrite(pChan, Addr, Data);
        }
    } else {
        for (UINT32 i = 0U; i < MX00_AR0239_NUM_PLL_REG; i++) {
            Addr = MX00_AR0239_PllRegTable[i].Addr;
            Data = MX00_AR0239_PllRegTable[i].Data;
            RetVal |= MX00_AR0239_RegWrite(pChan, Addr, Data);
        }
    }
    return RetVal;
}

static UINT32 MX00_AR0239_CFPNImp(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;

    for (UINT32 i = 0U; i < MX00_AR0239_NUM_CFPN_Impr_REG; i++) {
        Addr = MX00_AR0239_CFPNImpTable[i].Addr;
        Data = MX00_AR0239_CFPNImpTable[i].Data;
        RetVal |= MX00_AR0239_RegWrite(pChan, Addr, Data);
    }

    return RetVal;
}
static UINT32 MX00_AR0239_HDRSeq(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;

    for (UINT32 i = 0U; i < MX00_AR0239_NUM_HDR_Seq_REG; i++) {
        Addr = MX00_AR0239_HDRSeqTable[i].Addr;
        Data = MX00_AR0239_HDRSeqTable[i].Data;
        RetVal |= MX00_AR0239_RegWrite(pChan, Addr, Data);
    }

    return RetVal;
}

static UINT32 MX00_AR0239_SensorModeInit(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((SensorMode == MX00_AR0239_1920_1080_A30P) ||(SensorMode == MX00_AR0239_1920_1080_2LANE_A15P) || (SensorMode == MX00_AR0239_1920_1080_2LANE_A30P)) {
        RetVal |= MX00_AR0239_ConfigLinearSeq(pChan);
        RetVal |= MX00_AR0239_ConfigRecomEsSeq(pChan);
        RetVal |= MX00_AR0239_ConfigPllSeq(pChan, SensorMode);
        RetVal |= MX00_AR0239_ChangeReadoutMode(pChan, SensorMode);
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x3082, 0x0001);/* ERS Linear Mode */
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x318E, 0x0000);/* HDR_MC_CTRL3 */
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x31D0, 0x0000); /* COMPANDING DISABLE */
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x3064, 0x1802); /* Embedded off */
    } else {
        RetVal |= MX00_AR0239_CFPNImp(pChan);
        RetVal |= MX00_AR0239_HDRSeq(pChan);
        RetVal |= MX00_AR0239_ConfigRecomEsSeq(pChan);
        RetVal |= MX00_AR0239_ConfigPllSeq(pChan,SensorMode);
        RetVal |= MX00_AR0239_ChangeReadoutMode(pChan, SensorMode);
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x3082, 0x4808); /* HDR 2exp arbitrary T1 and T2*/
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x318E, 0x9000); /* HDR_MC_CTRL3*/
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x3100, 0x0000); /* AECTRLREG*/
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x3786, 0x6204); /* num_exp_max=2*/
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x31D0, 0x0000); /* COMPANDING DISABLE */
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x3064, 0x1802); /* Embedded off */
    }



    return RetVal;
}

static UINT32 MX00_AR0239_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#ifdef MX00_AR0239_IN_SLAVE_MODE
        RetVal = MX00_AR0239_RegWrite(pChan, 0x30CE, 0x0120);
        RetVal |= MX00_AR0239_RegWrite(pChan, 0x301A, 0x215C);
#else
        RetVal = MX00_AR0239_RegWrite(pChan, 0x301A, 0x205C);
#endif
    }

    return RetVal;
}


static UINT32 MX00_AR0239_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX00_AR0239_RegWrite(pChan, 0x301A, 0x0001);
        RetVal = MX00_AR0239_RegWrite(pChan, 0x301A, 0x2058);
    }

    return RetVal;
}


static void MX00_AR0239_PreCalculateModeInfo(void)
{
    UINT32 i;

    for (i = 0U; i < MX00_AR0239_NUM_MODE; i++) {
        MX00_AR0239_ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)MX00_AR0239_SensorInfo[i].LineLengthPck /
                                              ((DOUBLE)MX00_AR0239_SensorInfo[i].DataRate *
                                               (DOUBLE)MX00_AR0239_SensorInfo[i].NumDataLanes /
                                               (DOUBLE)MX00_AR0239_SensorInfo[i].NumDataBits));
        MX00_AR0239_ModeInfoList[i].PixelRate = (FLOAT)((DOUBLE)MX00_AR0239_OutputInfo[i].DataRate *
                                                (DOUBLE)MX00_AR0239_OutputInfo[i].NumDataLanes /
                                                (DOUBLE)MX00_AR0239_OutputInfo[i].NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const MX00_AR0239_SENSOR_INFO_s *pSensorInfo = &MX00_AR0239_SensorInfo[0];
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID,  pSensorInfo->InputClk);     /* The default vin reference clock frequency */
        MX00_AR0239_PreCalculateModeInfo();
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal= SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX00_AR0239_ResetSensor(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal= SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* Set streaming off and reset */
        RetVal = MX00_AR0239_StandbyOn(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_GetStatus
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pStatus == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pStatus, &MX00_AR0239Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_GetModeInfo
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }


        if (Config.ModeID >= MX00_AR0239_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            MX00_AR0239_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_GetDeviceInfo
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &MX00_AR0239_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_GetHdrInfo
 *
 *  @Description:: Get Sensor HDR Info
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      ShutterCtrl:    pointer to sensor exposure line
 *
 *  @Output     ::
 *      pHdrInfo:       pointer to Hdr info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT16 SEFShtCtrl;// = (UINT16)pShutterCtrl[1];
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        SEFShtCtrl = (UINT16)pShutterCtrl[1];
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].EffectiveArea.StartY = SEFShtCtrl+2U;
            U32RVal |= AmbaWrap_memcpy(pHdrInfo, &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
        } else {
            U32RVal |= AmbaWrap_memcpy(pHdrInfo, &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
        }
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_GetCurrentGainFactor
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    //const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;
    UINT32 CurrentAgcIdx;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) ||(pGainFactor == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                CurrentAgcIdx = (MX00_AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] & 0xFFFFU);
                *pGainFactor = MX00_AR0239_AgcRegTable[CurrentAgcIdx].Factor;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_GetCurrentShutterSpeed
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 i,k;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    pExposureTime[0] = pModeInfo->RowTime * (FLOAT)MX00_AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][0];
                }
            }
        } else {
            for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (k = 0U; k < (pModeInfo->HdrInfo.ActiveChannels); k ++) {
                        pExposureTime[k] =(FLOAT)MX00_AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][k]* pModeInfo->RowTime*(FLOAT)(2U);

                    }
                }
            }
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_ConvertGainFactor
 *
 *  @Description:: Convert gain factor to analog and digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      DesiredFactor:      Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      Achievable gain factor (12.20 fixed point)
 *      pAnalogGainCtrl:    Analog gain control for achievable gain factor
 *      pDigitalGainCtrl:   Digital gain control for achievable gain factor
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT AgcGain = 0.0f;
    FLOAT DgcGainCtrl = 0.0f;
    FLOAT MinGain = MX00_AR0239_AgcRegTable[0].Factor;
    FLOAT MaxGain = MX00_AR0239_AgcRegTable[MX00_AR0239_NUM_AGC_STEP - 1U].Factor;// * AR0239_DeviceInfo.MaxDigitalGainFactor;
    FLOAT DesiredFactor =  pDesiredFactor->Gain[0];
    UINT32 AgcTableIdx;
    FLOAT TotalAgain = 0.0f;

    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {

            if (DesiredFactor < MinGain) {
                DesiredFactor = MinGain;
            } else if (DesiredFactor > MaxGain) {
                DesiredFactor = MaxGain;
            } else {
                /* Do nothing */
            }
            for (AgcTableIdx = 0; AgcTableIdx < (MX00_AR0239_NUM_AGC_STEP - 1U); AgcTableIdx ++) {
                TotalAgain = MX00_AR0239_AgcRegTable[AgcTableIdx + 1U].Factor;
                if(DesiredFactor < TotalAgain) {
                    break;
                }
            }
            AgcGain = MX00_AR0239_AgcRegTable[AgcTableIdx].Factor;

            DgcGainCtrl = 128.0f;
            pGainCtrl->AnalogGain[0] = ((UINT32)DgcGainCtrl << 16U) + AgcTableIdx; //[31:16] DgcGain + AgcTableIdx
            pActualFactor->Gain[0] = AgcGain * (DgcGainCtrl / 128.0f);
        } else {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {


                if (DesiredFactor < MinGain) {
                    DesiredFactor = MinGain;
                } else if (DesiredFactor > MaxGain) {
                    DesiredFactor = MaxGain;
                } else {
                    /* Do nothing */
                }
                for (AgcTableIdx = 0; AgcTableIdx < (MX00_AR0239_NUM_AGC_STEP - 1U); AgcTableIdx ++) {
                    TotalAgain = MX00_AR0239_AgcRegTable[AgcTableIdx + 1U].Factor;
                    if(DesiredFactor < TotalAgain) {
                        break;
                    }
                }
                AgcGain = MX00_AR0239_AgcRegTable[AgcTableIdx].Factor;

                DgcGainCtrl = 128.0f;
                pGainCtrl->AnalogGain[0] = ((UINT32)DgcGainCtrl << 16U) + AgcTableIdx; //[31:16] DgcGain + AgcTableIdx
                pActualFactor->Gain[0] = AgcGain * (DgcGainCtrl / 128.0f);
                pGainCtrl->AnalogGain[1] = ((UINT32)DgcGainCtrl << 16U) + AgcTableIdx; //[31:16] DgcGain + AgcTableIdx
                pActualFactor->Gain[1] = AgcGain * (DgcGainCtrl / 128.0f);
                pGainCtrl->AnalogGain[2] = ((UINT32)DgcGainCtrl << 16U) + AgcTableIdx; //[31:16] DgcGain + AgcTableIdx
                pActualFactor->Gain[2] = AgcGain * (DgcGainCtrl / 128.0f);
                // }

            }
        }


    }

    return RetVal;
}

static void ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 ModeID = MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    UINT32 NumExposureStepPerFrame = MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame;
    UINT32 MinShrWidth;
    UINT32 MaxShrWidth;
    UINT32 ExposureFrames;
    FLOAT  ShutterCtrl;

    if (ModeID == MX00_AR0239_1920_1080_30P_HDR) {
        ShutterCtrl = ExposureTime / (MX00_AR0239_ModeInfoList[ModeID].RowTime*(FLOAT)(2U));
    } else {
        ShutterCtrl = ExposureTime / (MX00_AR0239_ModeInfoList[ModeID].RowTime);
    }
    *pShutterCtrl = (UINT32)ShutterCtrl;
    if (*pShutterCtrl > ((0xffffU / NumExposureStepPerFrame) * NumExposureStepPerFrame)) {
        *pShutterCtrl = ((0xffffU / NumExposureStepPerFrame) * NumExposureStepPerFrame);
    }
    ExposureFrames = *pShutterCtrl / NumExposureStepPerFrame;
    /*ExposureFrames = (*pShutterCtrl % NumExposureStepPerFrame) ? (ExposureFrames + 1U) : (ExposureFrames);*/
    if ((*pShutterCtrl % NumExposureStepPerFrame) > 0U) {
        ExposureFrames = ExposureFrames + 1U;
    }
    /* limit exp not over 1 frame time */
    if (ExposureFrames > 1U) {
        ExposureFrames = 1U;
    }
    /* limit exp. setting to a valid value */
    if((ModeID == MX00_AR0239_1920_1080_A30P) || (ModeID == MX00_AR0239_1920_1080_2LANE_A30P) || (ModeID == MX00_AR0239_1920_1080_2LANE_A15P)) {
        MaxShrWidth = (ExposureFrames * NumExposureStepPerFrame) - 1U;
        MinShrWidth = 1U;
    } else {
        if (HdrChannel == 1U) {
            MinShrWidth = MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].MinExposureLine;
            MaxShrWidth = MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].MaxExposureLine;
        } else {
            MinShrWidth = MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[0].MinExposureLine;
            MaxShrWidth = MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[0].MaxExposureLine;
        }
    }
    if(*pShutterCtrl > MaxShrWidth) {
        *pShutterCtrl = MaxShrWidth;
    }
    if(*pShutterCtrl < MinShrWidth) {
        *pShutterCtrl = MinShrWidth;
    }
    if((ModeID == MX00_AR0239_1920_1080_A30P) || (ModeID == MX00_AR0239_1920_1080_2LANE_A30P) || (ModeID == MX00_AR0239_1920_1080_2LANE_A15P)) {
        *pActualExptime = (FLOAT)*pShutterCtrl * MX00_AR0239_ModeInfoList[ModeID].RowTime;
    } else {
        *pActualExptime = (FLOAT)*pShutterCtrl * MX00_AR0239_ModeInfoList[ModeID].RowTime*(FLOAT)(2U);
    }

}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_ConvertShutterSpeed
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 j;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        /* None-HDR mode */
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pChan, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else {
            /* HDR mode */
            for (j = 0U; j < (pModeInfo->HdrInfo.ActiveChannels); j ++) {
                ConvertShutterSpeed(pChan, j, pDesiredExposureTime[j], &pActualExposureTime[j], &pShutterCtrl[j]);

            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_SetAGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      PGC:    Programmable gain control setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_SetAGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl)
{
    UINT16 R3100Val = 0U, R3060Val = 0U;
    UINT32 AgcIdx = (AnalogGainCtrl & 0xffffU);
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= MX00_AR0239_RegRead(pChan, 0x3100, &R3100Val);

    if (MX00_AR0239_AgcRegTable[AgcIdx].GainConv == MX00_AR0239_GAIN_CONVERT_LOW) {
        R3100Val = R3100Val & 0xFFFBU;  /* Enable LCG */
    } else {
        R3100Val = R3100Val | 0x04U;    /* Enable HCG */
    }
    RetVal |= MX00_AR0239_RegWrite(pChan, 0x3100, R3100Val);

    /* Analog gain */

    R3060Val = ((UINT16)(MX00_AR0239_AgcRegTable[AgcIdx].HData << 4U)) | MX00_AR0239_AgcRegTable[AgcIdx].LData;
    RetVal |= MX00_AR0239_RegWrite(pChan, 0x3060, R3060Val);

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_SetAnalogGainCtrl
 *
 *  @Description:: Set analog gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      AnalogGainCtrl:     Analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 i,j;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    //(void)pModeInfo;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        /* None-HDR mode */
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_AR0239_SetAGainReg(pChan, pAnalogGainCtrl[0]);
            /* Update current analog gain control */
            for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    MX00_AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = pAnalogGainCtrl[0];
                }
            }
        } else {
            RetVal = MX00_AR0239_SetAGainReg(pChan, pAnalogGainCtrl[0]);
            for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                /*if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {*/
                for (j=0U; j<(pModeInfo->HdrInfo.ActiveChannels); j ++) {
                    MX00_AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][j] = pAnalogGainCtrl[0];
                }/*}*/
            }/* HDR mode */
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_SetShutterCtrl
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 i,j;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 SFData, Data ;
    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        /* None-HDR mode */
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_AR0239_SetShutterReg(pChan, pShutterCtrl[0]);

            for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    MX00_AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][0] = pShutterCtrl[0];
                }
            }
        } else {
            /* HDR mode */
            SFData = (UINT16)(pShutterCtrl[1]);
            Data = (UINT16)(pShutterCtrl[0]);
            RetVal |= MX00_AR0239_RegWrite(pChan, 0x3022, 0x01);
            RetVal |= MX00_AR0239_RegWrite(pChan, 0x3212, SFData);
            RetVal |= MX00_AR0239_RegWrite(pChan, 0x3012, Data);
            RetVal |= MX00_AR0239_RegWrite(pChan, 0x3022, 0x00);

            for (i = 0U; i < MX00_AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                //if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                for (j = 0U; j < (pModeInfo->HdrInfo.ActiveChannels); j ++) {
                    MX00_AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][j] = pShutterCtrl[j];
                }
                //}
            }
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_SetSlowShutterCtrl
 *
 *  @Description:: set slow shutter control
 *
 *  @Input      ::
 *      Chan:           Vin ID and sensor ID
 *      pSlowShutterCtrl:   Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    (void)pChan;
    (void)SlowShutterCtrl;

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0239_Config
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0239_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s MX00_AR0239PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        //.EnabledPin = EnabledPin[pChan->VinID]
    };

    AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT16 SensorMode = (UINT16)pMode->ModeID;
    MX00_AR0239_FRAME_TIMING_s *pFrameTime;// = &MX00_AR0239Ctrl[pChan->VinID].FrameTime;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;


    if ((pChan == NULL) || (SensorMode >= MX00_AR0239_NUM_MODE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        pFrameTime = &MX00_AR0239Ctrl[pChan->VinID].FrameTime;
        MX00_AR0239PadConfig.EnabledPin = EnabledPin[pChan->VinID];

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Add delay time to wait MAX9296/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power off", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(1);

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power on", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "============  [ MX00_AR0239 Config ]  ============", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d ============", SensorMode, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

        MX00_AR0239_PrepareModeInfo(pChan, pMode, pModeInfo);
        U32RVal |= AmbaWrap_memcpy(pFrameTime, &MX00_AR0239_ModeInfoList[SensorMode].FrameTime, sizeof(MX00_AR0239_FRAME_TIMING_s));

#ifdef MX00_AR0239_IN_SLAVE_MODE
        (void)AmbaVIN_MasterSyncDisable(AMBA_VIN_MSYNC0);
#endif
        /* Adjust mipi-phy parameters */
        MX00_AR0239PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", (UINT32)MX00_AR0239PadConfig.DateRate, 0U, 0U, 0U, 0U);
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX00_AR0239PadConfig);



        /* Hardware reset for 9296 */
        /* Set MAX9296A PWDNB high */
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB low", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(1);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB high", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_9296_Init(pChan->VinID, MX00_AR0239_GetEnabledLinkID(pChan->SensorID));
        MX00_AR0239_ConfigSerDes(pChan, SensorMode, pModeInfo);

        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX00_AR0239_SENSOR_ID_CHAN_A) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_27M);

        }
        if ((pChan->SensorID & MX00_AR0239_SENSOR_ID_CHAN_B) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_27M);
        }
        RetVal |= MX00_AR0239_ResetSensor(pChan);
        /* Program sensor registers */
        RetVal |= MX00_AR0239_SoftwareReset(pChan);
        RetVal |= MX00_AR0239_SensorModeInit(pChan, SensorMode);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  SensorMode: %d  ============", SensorMode, 0U, 0U, 0U, 0U);

        RetVal |= MX00_AR0239_StandbyOff(pChan); /* Stream on */
#ifdef MX00_AR0239_IN_SLAVE_MODE
        RetVal |= MX00_AR0239_ConfigMasterSync(SensorMode);
#endif

        RetVal |= MX00_AR0239_ConfigVin(pChan, pModeInfo);
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX00_AR0239Obj = {
    .SensorName             = "AR0239",
    .SerdesName             = "MAX9295_9296",

    .Init                   = MX00_AR0239_Init,//
    .Enable                 = MX00_AR0239_Enable,//
    .Disable                = MX00_AR0239_Disable,//
    .Config                 = MX00_AR0239_Config,
    .GetStatus              = MX00_AR0239_GetStatus,//
    .GetModeInfo            = MX00_AR0239_GetModeInfo,//
    .GetDeviceInfo          = MX00_AR0239_GetDeviceInfo,//
    .GetHdrInfo             = MX00_AR0239_GetHdrInfo,//
    .GetCurrentGainFactor   = MX00_AR0239_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX00_AR0239_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX00_AR0239_ConvertGainFactor,
    .ConvertShutterSpeed    = MX00_AR0239_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX00_AR0239_SetAnalogGainCtrl,
    //.SetWbGainCtrl          = MX00_AR0239_SetWbGainCtrl,
    //.SetDigitalGainCtrl     = MX00_AR0239_SetDigitalGainCtrl,
    .SetShutterCtrl         = MX00_AR0239_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX00_AR0239_SetSlowShutterCtrl,

    .RegisterRead           = MX00_AR0239_RegisterRead,
    .RegisterWrite          = MX00_AR0239_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};



