/**
 *  @file AmbaSensor_MAX9295_9296_IMX490.c
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
 *  @details Control APIs of MAX9295_9296 plus SONY IMX490 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaMisraFix.h"

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_IMX490.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSbrg_Max9295_9296.h"

#include "bsp.h"

//#define DEBUG_MSG //enable if when need to print all sensor setting
#define MX00_IMX490_IN_SLAVE_MODE
//#define MX00_IMX490_I2C_WR_BUF_SIZE 64
#if 0
static INT32 MX00_IMX490_ConfigPost(UINT32 *pVinID);
#endif

static UINT32 MX00_IMX490I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
};

static UINT32 MX00_IMX490MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
};


/*-----------------------------------------------------------------------------------------------*\
 * MX00_IMX490 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX00_IMX490_CTRL_s MX00_IMX490Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};

static MAX9295_9296_SERDES_CONFIG_s MX00_IMX490SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX00_IMX490_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_IMX490_I2C_SLAVE_ADDRESS + 0x10U),
            [1] = (UINT8)(MX00_IMX490_I2C_SLAVE_ADDRESS + 0x12U),
        },
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},

        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [1] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX00_IMX490_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_IMX490_I2C_SLAVE_ADDRESS + 0x14U),
            [1] = (UINT8)(MX00_IMX490_I2C_SLAVE_ADDRESS + 0x16U),
        },
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},

        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },

};

static UINT16 MX00_IMX490_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX9295_9296_IMX490_GetNumActiveSensor
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
static UINT32 MX00_IMX490_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            SensorNum ++;
        }
    }

    if(SensorNum <= 1U) {
        SensorNum = 1U;
    }

    return SensorNum;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_IMX490_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                               = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s              *pInputInfo     = &MX00_IMX490_InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s             *pOutputInfo    = &MX00_IMX490_OutputInfo[ModeID];
    const MX00_IMX490_SENSOR_INFO_s     *pSensorInfo    = &MX00_IMX490_SensorInfo[ModeID];
    const MX00_IMX490_FRAME_TIMING_s    *pFrameTime     = &MX00_IMX490ModeInfoList[ModeID].FrameTime;
    UINT32 SensorNum = MX00_IMX490_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    if (SensorNum != 0U) {
        pModeInfo->FrameLengthLines         = pSensorInfo->FrameLengthLines * SensorNum;
        pModeInfo->NumExposureStepPerFrame  = pSensorInfo->FrameLengthLines;
        pModeInfo->InternalExposureOffset   = 0.0f;
        pModeInfo->RowTime = MX00_IMX490ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
        U32RVal |= AmbaWrap_floor((((DOUBLE)MX00_IMX490ModeInfoList[ModeID].PixelRate * (DOUBLE)SensorNum) * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal);
        pModeInfo->LineLengthPck            = (UINT32)FloorVal;
        pModeInfo->InputClk                 = pSensorInfo->InputClk;

        U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX00_IMX490_HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information

        /* update for MAX9296 CSI-2 output */
        pModeInfo->OutputInfo.DataRate = pModeInfo->OutputInfo.DataRate * SensorNum;
        pModeInfo->OutputInfo.OutputHeight = (UINT16)(pModeInfo->OutputInfo.OutputHeight * SensorNum);
        pModeInfo->OutputInfo.RecordingPixels.StartY = (UINT16)(pModeInfo->OutputInfo.RecordingPixels.StartY * SensorNum);
        pModeInfo->OutputInfo.RecordingPixels.Height = (UINT16)(pModeInfo->OutputInfo.RecordingPixels.Height * SensorNum);

        /* Updated minimum frame rate limitation */
        /*
        if (pModeInfo->HdrInfo.HdrType == 0U) {
            pModeInfo->MinFrameRate.TimeScale /= 8U;
        }
        */
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_ConfigSerDes
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
static void MX00_IMX490_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const MX00_IMX490_SENSOR_INFO_s *pSensorInfo = &MX00_IMX490_SensorInfo[SensorMode];
    MAX9295_9296_SERDES_CONFIG_s *pSerdesCfg = &MX00_IMX490SerdesConfig[pChan->VinID];
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    pSerdesCfg->EnabledLinkID = MX00_IMX490_GetEnabledLinkID(pChan->SensorID);

    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;
    U32RVal |= AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal);
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    (void) Max9295_9296_Config(pChan->VinID, &MX00_IMX490SerdesConfig[pChan->VinID]);
    if(U32RVal != 0U) {
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_ConfigVin
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
static UINT32 MX00_IMX490_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX00_IMX490VinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace = AMBA_VIN_COLOR_SPACE_RGB,
            .BayerPattern = AMBA_SENSOR_BAYER_PATTERN_RG,
            .NumDataBits  = 0,
            .NumSkipFrame = 1U,
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
        .DataType = 0x20U,//*****
        .DataTypeMask = 0x1fU,
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0},
    };
    //-----------------------------------------------------------------------------------------------//

    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &MX00_IMX490VinConfig;
    UINT32 U32RVal = 0;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;


    U32RVal |= AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return AmbaVIN_MipiConfig(VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_ResetSensor
 *
 *  @Description:: Reset IMX490 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MX00_IMX490_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan->SensorID & MX00_IMX490_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    if ((pChan->SensorID & MX00_IMX490_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
    if ((pChan->SensorID & MX00_IMX490_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    if ((pChan->SensorID & MX00_IMX490_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start */

    if (RetVal != MAX9295_9296_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }

    return RetVal;
}

//TBD: boardcast only and not support burst write
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: RegWrite
 *
 *  @Description:: Read/Write sensor registers through SPI bus
 *
 *  @Input      ::
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
    UINT8 TxDataBuf[MX00_IMX490_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;
#ifdef DEBUG_MSG
    UINT32 k;
#endif

    if ((pChan == NULL) || (Size > MX00_IMX490_SENSOR_I2C_MAX_SIZE)) {
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
        RetVal = AmbaI2C_MasterWrite(MX00_IMX490I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        if (pChan->SensorID == (MX00_IMX490_SENSOR_ID_CHAN_A | MX00_IMX490_SENSOR_ID_CHAN_B)) {
            I2cConfig.SlaveAddr = MX00_IMX490SerdesConfig[pChan->VinID].SensorSlaveID;

            /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
            RetVal = AmbaI2C_MasterWrite(MX00_IMX490I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                         &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Broadcast] Slave ID 0x%2x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U);
            for (k = 0U; k < Size; k++) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
            }
#endif

        } else {
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_IMX490SerdesConfig[pChan->VinID].SensorAliasID[i];
                    /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
                    RetVal |= AmbaI2C_MasterWrite(MX00_IMX490I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                                  &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[SensorID 0x%02x] Slave ID 0x%2x", (1 << i), I2cConfig.SlaveAddr, 0U, 0U, 0U);
                    for (k = 0U; k < Size; k++) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
                    }
#endif
                }
            }
        }
        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX490] Addr 0x%04x Size %d I2C does not work!!!!!", Addr, Size, 0U, 0U, 0U);
        }
    }

    //SENSOR_DBG_IO("[MX00_IMX490][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_RegisterWrite
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
 *      INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
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

static UINT32 MX00_IMX490_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    if ((pChan->SensorID != MX00_IMX490_SENSOR_ID_CHAN_A) && (pChan->SensorID != MX00_IMX490_SENSOR_ID_CHAN_B)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cTxConfig.DataSize  = 2U;
        I2cTxConfig.pDataBuf  = TxData;
        TxData[0] = (UINT8)(Addr >> 8U);
        TxData[1] = (UINT8)(Addr & 0xffU);

        I2cRxConfig.DataSize  = 1U;
        I2cRxConfig.pDataBuf  = pRxData;

        if (pChan->SensorID == MX00_IMX490_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX00_IMX490SerdesConfig[pChan->VinID].SensorAliasID[0];
            I2cRxConfig.SlaveAddr = (MX00_IMX490SerdesConfig[pChan->VinID].SensorAliasID[0] | (UINT32)1U);
        } else {
            I2cTxConfig.SlaveAddr = MX00_IMX490SerdesConfig[pChan->VinID].SensorAliasID[1];
            I2cRxConfig.SlaveAddr = (MX00_IMX490SerdesConfig[pChan->VinID].SensorAliasID[1] | (UINT32)1U);
        }

        /*        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x, 0x%x", I2cTxConfig.SlaveAddr, I2cRxConfig.SlaveAddr, 0U, 0U, 0U); */
        RetVal = AmbaI2C_MasterReadAfterWrite(MX00_IMX490I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                              &I2cRxConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX00_IMX490] Addr 0x%04x I2C does not work!!!!!", Addr, 0U, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_RegisterRead
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
static UINT32 MX00_IMX490_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData = 0U;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_IMX490_RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_SetAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      PGC:    Analog gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl)
{
    UINT32 RetVal;
    UINT8 WData[2];

    /* SP1H */
    WData[0] = (UINT8)(AnalogGainCtrl & 0xffU);
    WData[1] = (UINT8)((AnalogGainCtrl >> 8U) & 0x7U);
    RetVal = RegWrite(pChan, MX00_IMX490_AGAIN_SP1H, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_SetDigitalGainReg
 *
 *  @Description:: Configure sensor conversion gain setting
 *
 *  @Input      ::
 *      Chan:      Vin ID and sensor ID
 *      CG:        Conversion gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_SetDigitalGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 DigitalGainCtrl)
{
    UINT32 RetVal;
    UINT8 WData[2];

    /* SP1H */
    WData[0] = (UINT8)(DigitalGainCtrl & 0xffU);
    WData[1] = (UINT8)((DigitalGainCtrl >> 8U) & 0x1U);
    RetVal = RegWrite(pChan, MX00_IMX490_DGAIN_SP1H, WData, 2);

    /* SP1L */
    //WData[0] = (UINT8)(DigitalGainCtrl & 0xffU);
    //WData[1] = (UINT8)((DigitalGainCtrl >> 8U) & 0x1U);
    //RetVal = RegWrite(pChan, MX00_IMX490_DGAIN_SP1L, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_SetWbGainReg
 *
 *  @Description:: Configure sensor wb gain setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pWbGainCtrl:     Digital gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[8];

    WData[0] = (UINT8)(pWbGainCtrl[0].R & 0xffU);
    WData[1] = (UINT8)((pWbGainCtrl[0].R >> 8U) & 0xfU);
    WData[2] = (UINT8)(pWbGainCtrl[0].Gr & 0xffU);
    WData[3] = (UINT8)((pWbGainCtrl[0].Gr >> 8U) & 0xfU);
    WData[4] = (UINT8)(pWbGainCtrl[0].Gb & 0xffU);
    WData[5] = (UINT8)((pWbGainCtrl[0].Gb >> 8U) & 0xfU);
    WData[6] = (UINT8)(pWbGainCtrl[0].B & 0xffU);
    WData[7] = (UINT8)((pWbGainCtrl[0].B >> 8U) & 0xfU);
    RetVal |= RegWrite(pChan, MX00_IMX490_WBGAIN_R, WData, 8);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      NumXhsEshrSpeed:    Integration time in number of XHS period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 ShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHS1;
    UINT8 WData[4];

    /* SP1H & SP1L */
    SHS1 = ShutterCtrl;

    WData[0] = (UINT8)(SHS1 & 0xffU);
    WData[1] = (UINT8)((SHS1 >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHS1 >> 16U) & 0x3U);
    RetVal |= RegWrite(pChan, MX00_IMX490_SHS1, WData, 3);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_SetHdrAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pAnalogGainCtrl: Pointer to analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_SetHdrAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[2];

    /* SP1H */
    WData[0] = (UINT8)(pAnalogGainCtrl[0] & 0xffU);
    WData[1] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x7U);
    RetVal |= RegWrite(pChan, MX00_IMX490_AGAIN_SP1H, WData, 2);

    /* SP1L, SP2H, SP2L are fixed. */

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_SetHdrShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      *pSHS:    Sensor SHS setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHSX;
    UINT8 WData[4];

    /* SP1H & SP1L */
    SHSX = pShutterCtrl[0];

    WData[0] = (UINT8)(SHSX & 0xffU);
    WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHSX >> 16U) & 0x3U);
    RetVal |= RegWrite(pChan, MX00_IMX490_SHS1, WData, 3);

    /* SP2 */
    SHSX = pShutterCtrl[0];

    WData[0] = (UINT8)(SHSX & 0xffU);
    WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHSX >> 16U) & 0x3U);
    RetVal |= RegWrite(pChan, MX00_IMX490_SHS2, WData, 3);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_SetSlowShutterReg
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
static UINT32 MX00_IMX490_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    AmbaMisra_TouchUnused(&IntegrationPeriodInFrame);
    return SENSOR_ERR_NONE;
#if 0
    UINT32 TargetFrameLengthLines;
    UINT32 ModeID = MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData[1];

    if (IntegrationPeriodInFrame < 1U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TargetFrameLengthLines = MX00_IMX490ModeInfoList[ModeID].FrameTime.VMAX * IntegrationPeriodInFrame;

        if (TargetFrameLengthLines > 0x3ffffU) {
            TargetFrameLengthLines = 0x3ffffU;
        }

        TxData[0] = IntegrationPeriodInFrame - 1;

        RetVal |= RegWrite(pChan, MX00_IMX490_FMAX, TxData, 1U);

        /* Update frame rate information */
        MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale = MX00_IMX490ModeInfoList[ModeID].FrameTime.FrameRate.TimeScale / IntegrationPeriodInFrame;
    }

    return RetVal;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_StandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0xFFU;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= RegWrite(pChan, MX00_IMX490_STANDBY, &TxData, 1);
//    AmbaPrint("[MX00_IMX490] MX00_IMX490_StandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_StandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x5CU;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= RegWrite(pChan, MX00_IMX490_STANDBY, &TxData, 1);
    (void)AmbaKAL_TaskSleep(2);

    TxData = 0xA3U;
    RetVal |= RegWrite(pChan, MX00_IMX490_STANDBY, &TxData, 1);
    (void)AmbaKAL_TaskSleep(2);

    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * MX00_IMX490Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / MX00_IMX490Ctrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[MX00_IMX490] MX00_IMX490_StandbyOff");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_ChangeReadoutMode
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
static UINT32 MX00_IMX490_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 i;
    UINT16 Addr;
    UINT8  Data;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if (ModeID == 0U) {
        for (i = 0U; i < MX00_IMX490_NUM_MODE_NML_REG; i++) {
            Addr = MX00_IMX490_RegTable_NML[i].Addr;
            Data = MX00_IMX490_RegTable_NML[i].Data;
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);
        }

        for (i = 0U; i < MX00_IMX490_NUM_CALIB; i ++) {
            Addr = MX00_IMX490_CalibTable_53[i].Addr;
            Data = MX00_IMX490_CalibTable_53[i].Data[ModeID];
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);

            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }
    }

    if (ModeID == 1U) {
        for (i = 0U; i < MX00_IMX490_NUM_MODE_REG_HDR30P; i++) {
            Addr = MX00_IMX490_RegTable_HDR30P[i].Addr;
            Data = MX00_IMX490_RegTable_HDR30P[i].Data;
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);
        }

        for (i = 0U; i < MX00_IMX490_NUM_REG; i ++) {
            Addr = MX00_IMX490_RegTable[i].Addr;
            Data = MX00_IMX490_RegTable[i].Data[ModeID];
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);
        }

        for (i = 0U; i < MX00_IMX490_NUM_CALIB; i ++) {
            Addr = MX00_IMX490_CalibTable_53[i].Addr;
            Data = MX00_IMX490_CalibTable_53[i].Data[ModeID];
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);

            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }
    }

    if (ModeID == 2U) {
        for (i = 0U; i < MX00_IMX490_NUM_MODE_REG_HDR60P; i++) {
            Addr = MX00_IMX490_RegTable_HDR60P[i].Addr;
            Data = MX00_IMX490_RegTable_HDR60P[i].Data;
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);
        }

        for (i = 0U; i < MX00_IMX490_NUM_REG; i ++) {
            Addr = MX00_IMX490_RegTable[i].Addr;
            Data = MX00_IMX490_RegTable[i].Data[ModeID];
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);
        }

        for (i = 0U; i < MX00_IMX490_NUM_CALIB; i ++) {
            Addr = MX00_IMX490_CalibTable_53[i].Addr;
            Data = MX00_IMX490_CalibTable_53[i].Data[ModeID];
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);

            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }
    }

    if (ModeID == 3U) {
        for (i = 0U; i < MX00_IMX490_NUM_MODE_REG_HDR30P; i++) {
            Addr = MX00_IMX490_RegTable_HDR30P[i].Addr;
            Data = MX00_IMX490_RegTable_HDR30P[i].Data;
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);
        }

        for (i = 0U; i < MX00_IMX490_NUM_REG; i ++) {
            Addr = MX00_IMX490_RegTable[i].Addr;
            Data = MX00_IMX490_RegTable[i].Data[ModeID];
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);
        }

        for (i = 0U; i < MX00_IMX490_NUM_CALIB; i ++) {
            Addr = MX00_IMX490_CalibTable_36[i].Addr;
            Data = MX00_IMX490_CalibTable_36[i].Data[ModeID];
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);

            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }
    }

    if (ModeID == 4U) {
        for (i = 0U; i < MX00_IMX490_NUM_MODE_REG_HDR60P; i++) {
            Addr = MX00_IMX490_RegTable_HDR60P[i].Addr;
            Data = MX00_IMX490_RegTable_HDR60P[i].Data;
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);
        }

        for (i = 0U; i < MX00_IMX490_NUM_REG; i ++) {
            Addr = MX00_IMX490_RegTable[i].Addr;
            Data = MX00_IMX490_RegTable[i].Data[ModeID];
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);
        }

        for (i = 0U; i < MX00_IMX490_NUM_CALIB; i ++) {
            Addr = MX00_IMX490_CalibTable_36[i].Addr;
            Data = MX00_IMX490_CalibTable_36[i].Data[ModeID];
            RetVal |= RegWrite(pChan, Addr, &Data, 1U);

            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }
    }

    /* Reset current AE information */
    U32RVal |= AmbaWrap_memset(MX00_IMX490Ctrl[pChan->VinID].CurrentAgcCtrl, 0x0, sizeof(UINT32) * 3U * MX00_IMX490_NUM_MAX_SENSOR_COUNT);
    U32RVal |= AmbaWrap_memset(MX00_IMX490Ctrl[pChan->VinID].CurrentDgcCtrl, 0x0, sizeof(UINT32) * 3U * MX00_IMX490_NUM_MAX_SENSOR_COUNT);
    U32RVal |= AmbaWrap_memset(MX00_IMX490Ctrl[pChan->VinID].CurrentWbCtrl, 0x0, sizeof(AMBA_SENSOR_WB_CTRL_s) * 3U * MX00_IMX490_NUM_MAX_SENSOR_COUNT);
    U32RVal |= AmbaWrap_memset(MX00_IMX490Ctrl[pChan->VinID].CurrentShutterCtrl, 0x0, sizeof(UINT32) * 3U * MX00_IMX490_NUM_MAX_SENSOR_COUNT);
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_IMX490_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;

    for (i = 0; i < MX00_IMX490_NUM_MODE; i++) {

        MX00_IMX490ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)MX00_IMX490_SensorInfo[i].LineLengthPck /
                                             ((DOUBLE)MX00_IMX490_SensorInfo[i].DataRate *
                                              (DOUBLE)MX00_IMX490_SensorInfo[i].NumDataLanes /
                                              (DOUBLE)MX00_IMX490_SensorInfo[i].NumDataBits));
        pOutputInfo = &MX00_IMX490_OutputInfo[i];
        MX00_IMX490ModeInfoList[i].PixelRate = (FLOAT) ((DOUBLE) pOutputInfo->DataRate *
                                               (DOUBLE)pOutputInfo->NumDataLanes /
                                               (DOUBLE)pOutputInfo->NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_Init
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
static UINT32 MX00_IMX490_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const MX00_IMX490_FRAME_TIMING_s *pFrameTime = &MX00_IMX490ModeInfoList[0U].FrameTime;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "MX00_IMX490 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        MX00_IMX490_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_Enable
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
static UINT32 MX00_IMX490_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;


    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX00_IMX490_ResetSensor(pChan);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "RESET sensor @ %x", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_Disable
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
static UINT32 MX00_IMX490_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_IMX490_StandbyOn(pChan);
    }

    //AmbaPrint("[MX00_IMX490] MX00_IMX490_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_GetStatus
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
static UINT32 MX00_IMX490_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pStatus, &MX00_IMX490Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_GetModeInfo
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
static UINT32 MX00_IMX490_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if (Config.ModeID >= MX00_IMX490_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            MX00_IMX490_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_GetDeviceInfo
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
static UINT32 MX00_IMX490_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &MX00_IMX490_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_GetHdrInfo
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
static UINT32 MX00_IMX490_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pHdrInfo, &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_GetCurrentGainFactor
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
static UINT32 MX00_IMX490_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k/*, TotalGainCtrl*/;
    DOUBLE GainFactor64 = 0.0;
    UINT32 U32RVal = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {

                    U32RVal |= AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)MX00_IMX490Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] * 0.3) + ((DOUBLE)MX00_IMX490Ctrl[pChan->VinID].CurrentDgcCtrl[i][0] * 0.1) ), &GainFactor64);
                    pGainFactor[0] = (FLOAT)GainFactor64;

                    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentGain: i=%d, %d ", 0, pGainFactor[0]*1000, 0, 0, 0);
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {

                        U32RVal |= AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)MX00_IMX490Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] * 0.3) + ((DOUBLE)MX00_IMX490Ctrl[pChan->VinID].CurrentDgcCtrl[i][k] * 0.1) ), &GainFactor64);
                        pGainFactor[k] = (FLOAT)GainFactor64;

                        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentGain: i=%d, %d ", k, pGainFactor[0]*1000, 0, 0, 0);
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_GetCurrentShutterSpeed
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
static UINT32 MX00_IMX490_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 FrameLengthLines;// = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame;// = pModeInfo->NumExposureStepPerFrame;
    DOUBLE ShutterTimeUnit;
    UINT8 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
        FrameLengthLines = pModeInfo->FrameLengthLines;
        NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    pExposureTime[0] = ((FLOAT)ShutterTimeUnit * (FLOAT)MX00_IMX490Ctrl[pChan->VinID].CurrentShutterCtrl[i][0]);

                    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentShutter: i=%d, %d ", 0, pExposureTime[0]*1000000, 0, 0, 0);
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pExposureTime[k] = ((FLOAT)ShutterTimeUnit * (FLOAT)MX00_IMX490Ctrl[pChan->VinID].CurrentShutterCtrl[i][k]);

                        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentShutter: i=%d, %d ", k, pExposureTime[k]*1000000, 0, 0, 0);
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_ConvertGainFactor
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
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType;// = MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
    DOUBLE DesiredFactor;
    DOUBLE LogDesiredFactor = 1.0;
    DOUBLE ActualFactor64 = 0.0;
    FLOAT Desire_dB_SP1H, /*Desire_dB_SP1L,*/ Desire_dB_SP1;
    FLOAT Agc_dB_SP1H = 0.0f, /*Agc_dB_SP1L = 0.0f,*/ Agc_dB_SP1 = 0.0f;
    FLOAT Dgc_dB = 0.0f;
    AMBA_SENSOR_WB_GAIN_FACTOR_s WbGain;
    DOUBLE GainInDb, WbGainInDb;
    UINT32 U32RVal = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        HdrType = MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
            U32RVal |= AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            Desire_dB_SP1H = (FLOAT)(20.0 * LogDesiredFactor);

            /************************************** Agc & Dgc **************************************/
            /* Maximum check */
            Desire_dB_SP1H = (Desire_dB_SP1H >= MX00_IMX490_MAX_TOTAL_GAIN) ? MX00_IMX490_MAX_TOTAL_GAIN : Desire_dB_SP1H;

            /* Minimum check */
            Desire_dB_SP1H = (Desire_dB_SP1H <= 18.0f) ? 18.0f : Desire_dB_SP1H;

            /* Calculate Agc/Dgc for SP1H/SP1L */
            if (Desire_dB_SP1H <= MX00_IMX490_MAX_AGAIN) {
                Agc_dB_SP1H = Desire_dB_SP1H;
                Dgc_dB = 0.0f;
            } else if (Desire_dB_SP1H >= MX00_IMX490_MAX_AGAIN) {
                Agc_dB_SP1H = MX00_IMX490_MAX_AGAIN;
                Dgc_dB = Desire_dB_SP1H - MX00_IMX490_MAX_AGAIN;
            } else {
                AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "=== Invalid SP1H and SP1L ===", NULL, NULL, NULL, NULL, NULL);
            }

            //SP1H
            U32RVal |= AmbaWrap_floor((DOUBLE)(Agc_dB_SP1H / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[0]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[0]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[0] = (FLOAT) ActualFactor64;

            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GGGGain: Desire_dB_SP1H=%d, DesiredFactor%d", Desire_dB_SP1H*1000, DesiredFactor*1000, 0, 0, 0);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GGGGain: again=%d(%ddB) dgain=%d(%ddB) pActualFactor=%d", (DOUBLE)pGainCtrl->AnalogGain[0], Agc_dB_SP1H, (DOUBLE)pGainCtrl->DigitalGain[0], Dgc_dB, pActualFactor->Gain[0]);

            //SP1L
            U32RVal |= AmbaWrap_floor((DOUBLE)(4.2 / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[1] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[1] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[1]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[1]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[1] = (FLOAT) ActualFactor64;

            //SP2H
            U32RVal |= AmbaWrap_floor((DOUBLE)(29.4 / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[2] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[2] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[2]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[2]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[2] = (FLOAT) ActualFactor64;

            //SP2L
            U32RVal |= AmbaWrap_floor((DOUBLE)(4.2 / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[3] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[3] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[3]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[3]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[3] = (FLOAT) ActualFactor64;

        } else {
            DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
            U32RVal |= AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            Desire_dB_SP1 = (FLOAT)(20.0 * LogDesiredFactor);

            /* Maximum check */
            Desire_dB_SP1 = (Desire_dB_SP1 >= MX00_IMX490_MAX_TOTAL_GAIN) ? MX00_IMX490_MAX_TOTAL_GAIN : Desire_dB_SP1;

            /* Minimum check */
            Desire_dB_SP1 = (Desire_dB_SP1 <= 0.0f) ? 0.0f : Desire_dB_SP1;

            /* Calculate Agc/Dgc for SP1 */
            if (Desire_dB_SP1 <= MX00_IMX490_MAX_AGAIN) {
                Agc_dB_SP1 = Desire_dB_SP1;
                Dgc_dB = 0.0f;
            } else {
                Agc_dB_SP1 = MX00_IMX490_MAX_AGAIN;
                Dgc_dB = Desire_dB_SP1 - MX00_IMX490_MAX_AGAIN;
            }

            U32RVal |= AmbaWrap_floor((DOUBLE)(Agc_dB_SP1 / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
            U32RVal |= AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[0]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[0]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[0] = (FLOAT) ActualFactor64;

            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GGGGain: Desire_dB_SP1=%d, DesiredFactor%d", Desire_dB_SP1*1000, DesiredFactor*1000, 0, 0, 0);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GGGGain: again=%d(%ddB) dgain=%d(%ddB) pActualFactor=%d", (DOUBLE)pGainCtrl->AnalogGain[0]*1000, Agc_dB_SP1*1000, (DOUBLE)pGainCtrl->DigitalGain[0], Dgc_dB, pActualFactor->Gain[0]*1000);
        }

        /************************************** Wb gain **************************************/
        /* Bounding check (SP1H&SP1L&SP2 share the same Wb gain) */
        WbGain.R  = (pDesiredFactor->WbGain[0].R >= 15.996094f)  ? 15.996094f : pDesiredFactor->WbGain[0].R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr >= 15.996094f) ? 15.996094f : pDesiredFactor->WbGain[0].Gr;
        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb >= 15.996094f) ? 15.996094f : pDesiredFactor->WbGain[0].Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B >= 15.996094f)  ? 15.996094f : pDesiredFactor->WbGain[0].B;

        WbGain.R  = (pDesiredFactor->WbGain[0].R <= 1.0f)  ? 1.0f : WbGain.R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr <= 1.0f) ? 1.0f : WbGain.Gr;
        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb <= 1.0f) ? 1.0f : WbGain.Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B <= 1.0f)  ? 1.0f : WbGain.B;

        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.R * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].R = (UINT32)WbGainInDb;
        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gr * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].Gr = (UINT32)WbGainInDb;
        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gb * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].Gb = (UINT32)WbGainInDb;
        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.B * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].B = (UINT32)WbGainInDb;

        pActualFactor->WbGain[0].R  = (FLOAT)pGainCtrl->WbGain[0].R  / 256.0f;
        pActualFactor->WbGain[0].Gr = (FLOAT)pGainCtrl->WbGain[0].Gr / 256.0f;
        pActualFactor->WbGain[0].Gb = (FLOAT)pGainCtrl->WbGain[0].Gb / 256.0f;
        pActualFactor->WbGain[0].B  = (FLOAT)pGainCtrl->WbGain[0].B  / 256.0f;

        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            U32RVal |= AmbaWrap_memcpy(&pGainCtrl->WbGain[1], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));
            U32RVal |= AmbaWrap_memcpy(&pGainCtrl->WbGain[2], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));
            U32RVal |= AmbaWrap_memcpy(&pGainCtrl->WbGain[3], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));

            U32RVal |= AmbaWrap_memcpy(&pActualFactor->WbGain[1], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
            U32RVal |= AmbaWrap_memcpy(&pActualFactor->WbGain[2], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
            U32RVal |= AmbaWrap_memcpy(&pActualFactor->WbGain[3], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
        }
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }
    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ConvertShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      HdrChannel:          0: LEF(HDR mode) or linear mdoe
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
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
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

    /* VMAX is 18 bits */
    if (ShutterCtrl > ((0x3ffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame)) {
        ShutterCtrl = ((0x3ffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame);
    }

    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MaxExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
        /* For none-HDR mode */
    } else {
        MaxExposureline = 2000U - 8U;//VMAX = 2000
        MinExposureline = 4U;
    }

    if (ShutterCtrl > MaxExposureline) {
        ShutterCtrl = MaxExposureline;
    } else if (ShutterCtrl < MinExposureline) {
        ShutterCtrl = MinExposureline;
    } else {
        /* MisraC */  /* do not need to adjust ShutterCtrl */
    }

    if ( (ShutterCtrl % 2U) == 1U) {
        ShutterCtrl = ShutterCtrl - 1U;
    }

    *pShutterCtrl   = ShutterCtrl;
    *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
    if(U32RVal != 0U) {
        /* MisraC */
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_ConvertShutterSpeed
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
static UINT32 MX00_IMX490_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                ConvertShutterSpeed(pModeInfo, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
                //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SSSShutter: i=%d pDesiredExposureTime=%d pActualExposureTime=%d pShutterCtrl=%d", i, pDesiredExposureTime[i]*1000000, pActualExposureTime[i]*1000000, pShutterCtrl[i], 0);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SSSShutter: i=%d pDesiredExposureTime=%d pActualExposureTime=%d pShutterCtrl=%d", 0, pDesiredExposureTime[0]*1000000, pActualExposureTime[0]*1000000, pShutterCtrl[0], 0);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_SetAnalogGainCtrl
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
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_IMX490_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

            /* Update current analog gain control */
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX490Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = pAnalogGainCtrl[0];
                    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Set again = %d", pAnalogGainCtrl[0]*1000, 0, 0, 0, 0);
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX490_SetHdrAnalogGainReg(pChan, pAnalogGainCtrl);

            /* Update current analog gain control */
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_IMX490Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] = pAnalogGainCtrl[k];
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
 *  @RoutineName:: MX00_IMX490_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pDigitalGainCtrl: Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_IMX490_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX490Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]= pDigitalGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX490_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_IMX490Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]= pDigitalGainCtrl[k];
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
 *  @RoutineName:: MX00_IMX490_SetWbGainCtrl
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
static UINT32 MX00_IMX490_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_IMX490_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX490Ctrl[pChan->VinID].CurrentWbCtrl[i][0] = pWbGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX490_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_IMX490Ctrl[pChan->VinID].CurrentWbCtrl[i][k] = pWbGainCtrl[k];
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "WWWbgain-set: R=%d Gr=%d Gb=%d B=%d", pWbGainCtrl[0].R*1000, pWbGainCtrl[0].Gr*1000, pWbGainCtrl[0].Gb*1000, pWbGainCtrl[0].B*1000, 0);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_SetShutterCtrl
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
static UINT32 MX00_IMX490_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_IMX490_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX490Ctrl[pChan->VinID].CurrentShutterCtrl[i][0] = pShutterCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX490_SetHdrShutterReg(pChan, pShutterCtrl);

            /* Update current shutter control */
            for (i = 0U; i < MX00_IMX490_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_IMX490Ctrl[pChan->VinID].CurrentShutterCtrl[i][k] = pShutterCtrl[k];
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
 *  @RoutineName:: MX00_IMX490_SetSlowShrCtrl
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
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX490_SetSlowShrCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        RetVal = MX00_IMX490_SetSlowShutterReg(SlowShutterCtrl);
    }

    return RetVal;
}

#ifdef MX00_IMX490_IN_SLAVE_MODE
static UINT32 MX00_IMX490_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{


    UINT32 RetVal;
    DOUBLE PeriodInDb;
    const MX00_IMX490_MODE_INFO_s* pModeInfo = &MX00_IMX490ModeInfoList[ModeID];
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg  = {0};
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_floor(((DOUBLE)pModeInfo->FrameTime.FrameRate.NumUnitsInTick * ((DOUBLE) MX00_IMX490_SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameTime.FrameRate.TimeScale)) + 0.5, &PeriodInDb);
    MasterSyncCfg.RefClk            = MX00_IMX490_SensorInfo[ModeID].InputClk;
    MasterSyncCfg.HSync.Period      = (UINT32)PeriodInDb;
    MasterSyncCfg.HSync.PulseWidth  = 8U;
    MasterSyncCfg.HSync.Polarity    = 0U;
    MasterSyncCfg.VSync.Period      = 1U;
    MasterSyncCfg.VSync.PulseWidth  = 1000U;
    MasterSyncCfg.VSync.Polarity    = 0U;
    MasterSyncCfg.HSyncDelayCycles  = 1U;
    MasterSyncCfg.VSyncDelayCycles  = 0U;
    MasterSyncCfg.ToggleHsyncInVblank  = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(MX00_IMX490MasterSyncChannel[pChan->VinID], &MasterSyncCfg);

    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX490_Config
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
static UINT32 MX00_IMX490_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s MX00_IMX490PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX490Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Add delay time to wait MAX9296/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power off", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power on", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(3);

    if (ModeID >= MX00_IMX490_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  [ MX00_IMX490_Config ]  ============", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d ============", ModeID, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

        /* update status */
        MX00_IMX490_PrepareModeInfo(pChan, pMode, pModeInfo);

#ifdef MX00_IMX490_IN_SLAVE_MODE
        (void)AmbaVIN_MasterSyncDisable(MX00_IMX490MasterSyncChannel[pChan->VinID]);
#endif
        /* Adjust mipi-phy parameters */
        MX00_IMX490PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", (UINT32)MX00_IMX490PadConfig.DateRate, 0U, 0U, 0U, 0U);
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX00_IMX490PadConfig);



        /* After reset VIN, set MAX9296A PWDNB high */
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB low", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(3);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB high", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_9296_Init(pChan->VinID, MX00_IMX490_GetEnabledLinkID(pChan->SensorID));


        MX00_IMX490_ConfigSerDes(pChan, ModeID, pModeInfo);

        /* set pll */
//#ifdef MX00_IMX490_IN_SLAVE_MODE
//        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
//#endif

        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX00_IMX490_SENSOR_ID_CHAN_A) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
        }
        if ((pChan->SensorID & MX00_IMX490_SENSOR_ID_CHAN_B) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
        }

        (void)MX00_IMX490_ResetSensor(pChan);


        /* program sensor */
        (void)MX00_IMX490_ChangeReadoutMode(pChan, ModeID);

#ifdef MX00_IMX490_IN_SLAVE_MODE
        {
            UINT8 Data;
            Data = 0x01U;
            RetVal |= RegWrite(pChan, 0x01f0U, &Data, 1U);//External Pulse-based sync
            Data = 0x01U;
            RetVal |= RegWrite(pChan, 0x01f1U, &Data, 1U);//Active Low
        }

#endif
        (void)MX00_IMX490_StandbyOff(pChan);

#ifdef MX00_IMX490_IN_SLAVE_MODE
        RetVal |= MX00_IMX490_ConfigMasterSync(pChan, ModeID);
#endif

        (void)AmbaKAL_TaskSleep(22); //TBD: To avoid i2c fail for gain setting

        /* config vin */
        RetVal |= MX00_IMX490_ConfigVin(pChan->VinID, pModeInfo);
    }

    return RetVal;
}

#if 0
static INT32 MX00_IMX490_ConfigPost(UINT32 *pVinID)
{
    MX00_IMX490_StandbyOff();

    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX00_IMX490Obj = {
    .SensorName             = "IMX490",
    .SerdesName             = "MAX9295_9296",
    .Init                   = MX00_IMX490_Init,
    .Enable                 = MX00_IMX490_Enable,
    .Disable                = MX00_IMX490_Disable,
    .Config                 = MX00_IMX490_Config,
    .GetStatus              = MX00_IMX490_GetStatus,
    .GetModeInfo            = MX00_IMX490_GetModeInfo,
    .GetDeviceInfo          = MX00_IMX490_GetDeviceInfo,
    .GetHdrInfo             = MX00_IMX490_GetHdrInfo,
    .GetCurrentGainFactor   = MX00_IMX490_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX00_IMX490_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX00_IMX490_ConvertGainFactor,
    .ConvertShutterSpeed    = MX00_IMX490_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX00_IMX490_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MX00_IMX490_SetDigitalGainCtrl,
    .SetWbGainCtrl          = MX00_IMX490_SetWbGainCtrl,
    .SetShutterCtrl         = MX00_IMX490_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX00_IMX490_SetSlowShrCtrl,

    .RegisterRead           = MX00_IMX490_RegisterRead,
    .RegisterWrite          = MX00_IMX490_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus    = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};

