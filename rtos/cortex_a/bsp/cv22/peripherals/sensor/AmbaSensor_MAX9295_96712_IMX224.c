/**
 *  @file AmbaSensor_MAX9295_96712_IMX224.c
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

#include <AmbaWrap.h>

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_96712_IMX224.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSbrg_Max9295_96712.h"

#include "bsp.h"

//#define DEBUG_MSG //enable if when need to print all sensor setting
#define MX01_IMX224_IN_SLAVE_MODE

/*-----------------------------------------------------------------------------------------------*\
 * MX01_IMX224 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX01_IMX224_CTRL_s MX01_IMX224Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};

static UINT32 MX01_IMX224I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_MAXIM_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_MAXIM_I2C_CHANNEL_PIP,
};

static UINT32 MX01_IMX224MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
};


static MAX9295_96712_SERDES_CONFIG_s MX01_IMX224SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX01_IMX224_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = 0x60U,
            [1] = 0x62U,
            [2] = 0x64U,
            [3] = 0x66U,
        },
        .DataType        = 0x2cU,  /* Default DataType: RAW12 */
        .DataType2       = 0xffU,  /* Default secondary DataType: not used */
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4, 4, 4},

        .CSITxSpeed      = {10, 10, 10, 10},
    },
    //[1] -> [3] TBD
    [1] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX01_IMX224_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)MX01_IMX224_I2C_SLAVE_ADDRESS + (UINT8)0x20,
            [1] = (UINT8)MX01_IMX224_I2C_SLAVE_ADDRESS + (UINT8)0x22,
            [2] = (UINT8)MX01_IMX224_I2C_SLAVE_ADDRESS + (UINT8)0x24,
            [3] = (UINT8)MX01_IMX224_I2C_SLAVE_ADDRESS + (UINT8)0x26,
        },
        .DataType        = 0x2cU,  /* Default DataType: RAW12 */
        .DataType2       = 0xffU,  /* Default secondary DataType: not used */
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4, 4, 4},

        .CSITxSpeed      = {10, 10, 10, 10},
    },

};

static UINT16 MX01_IMX224_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 i, EnabledLinkID = 0U;

    for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_GetNumActiveSensor
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
static UINT32 MX01_IMX224_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i ++) {
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
 *  @RoutineName:: MX01_IMX224_PrepareModeInfo
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
static UINT32 MX01_IMX224_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 SensorMode = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s *pInputInfo = &MX01_IMX224_InputInfo[SensorMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &MX01_IMX224_OutputInfo[SensorMode];
    const MX01_IMX224_SENSOR_INFO_s *pSensorInfo = &MX01_IMX224_SensorInfo[SensorMode];
    const MX01_IMX224_FRAME_TIMING_s *pFrameTime = &MX01_IMX224_ModeInfoList[SensorMode].FrameTime;
    UINT32 SensorNum = MX01_IMX224_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    pModeInfo->FrameLengthLines = pSensorInfo->FrameLengthLines * SensorNum;
    pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    pModeInfo->InternalExposureOffset = 0.0f;
    pModeInfo->RowTime = MX01_IMX224_ModeInfoList[SensorMode].RowTime / (FLOAT)SensorNum;
    if (AmbaWrap_floor((((DOUBLE)MX01_IMX224_ModeInfoList[SensorMode].PixelRate * (DOUBLE)pModeInfo->RowTime) + 0.5), &FloorVal) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    pModeInfo->LineLengthPck = (UINT32)FloorVal;
    pModeInfo->InputClk = pSensorInfo->InputClk;

    if (AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX01_IMX224_HdrInfo[SensorMode], sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) { // HDR information
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    /* update for MAX96712 CSI-2 output */
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
    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_ConfigSerDes
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
static UINT32 MX01_IMX224_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const MX01_IMX224_SENSOR_INFO_s *pSensorInfo = &MX01_IMX224_SensorInfo[SensorMode];
    MAX9295_96712_SERDES_CONFIG_s *pSerdesCfg = &MX01_IMX224SerdesConfig[pChan->VinID];
    DOUBLE FloorVal;
    UINT32 RetVal = SENSOR_ERR_NONE;

    pSerdesCfg->EnabledLinkID = MX01_IMX224_GetEnabledLinkID(pChan->SensorID);

    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;
    if (AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    RetVal |= Max9295_96712_Config(pChan->VinID, &MX01_IMX224SerdesConfig[pChan->VinID]);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_ConfigVin
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
static UINT32 MX01_IMX224_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX01_IMX224VinConfig = {
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
        .DataType = 0x00U,
        .DataTypeMask = 0x3fU,
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0},
    };
    //-----------------------------------------------------------------------------------------------//

    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &MX01_IMX224VinConfig;
    UINT32 RetVal = SENSOR_ERR_NONE;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;


    if (AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }

    RetVal |= AmbaVIN_MipiConfig(VinID, pVinCfg);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_ResetSensor
 *
 *  @Description:: Reset IMX224 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MX01_IMX224_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX224_ResetSensor] ", NULL, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "; # Link A: MAX9295A MFP4 H->L->H for IMX224", NULL, NULL, NULL, NULL, NULL);

    if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_A_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_A_ID, MAX9295_96712_GPIO_PIN_4, 1);
        (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start */
    }
    if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_B_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_B_ID, MAX9295_96712_GPIO_PIN_4, 1);
        (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start */
    }
    if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_C) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_C_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_C_ID, MAX9295_96712_GPIO_PIN_4, 1);
        (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start */
    }
    if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_D) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_D_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_D_ID, MAX9295_96712_GPIO_PIN_4, 1);
        (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start */
    }

    if (RetVal != MAX9295_96712_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX9295_96712_IMX224_RegRW
 *
 *  @Description:: Read/Write sensor registers through SPI bus
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
static UINT32 RegRW(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX01_IMX224_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;
#ifdef DEBUG_MSG
    UINT32 k;
#endif

    if ((pChan == NULL) || (Size > MX01_IMX224_SENSOR_I2C_MAX_SIZE)) {
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
        RetVal = AmbaI2C_MasterWrite(MX01_IMX224I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        if (pChan->SensorID == (MX01_IMX224_SENSOR_ID_CHAN_A | MX01_IMX224_SENSOR_ID_CHAN_B | MX01_IMX224_SENSOR_ID_CHAN_C | MX01_IMX224_SENSOR_ID_CHAN_D)) {
            I2cConfig.SlaveAddr = MX01_IMX224_I2C_BC_SLAVE_ADDRESS;

            RetVal = AmbaI2C_MasterWrite(MX01_IMX224I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD,
                                         &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Broadcast] Slave ID 0x%2x SensorID 0x%05x ", I2cConfig.SlaveAddr, pChan->SensorID, 0U, 0U, 0U);
            for (k = 0U; k < Size; k++) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
            }
#endif

        } else {
            for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX01_IMX224SerdesConfig[pChan->VinID].SensorAliasID[i];

                    RetVal |= AmbaI2C_MasterWrite(MX01_IMX224I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD,
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
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX224] Slave 0x%02x Addr 0x%02x Size %d I2C does not work!!!!!", I2cConfig.SlaveAddr, Addr, Size, 0U, 0U);
        }
    }
#if 0
    for (i = 0U; i < Size; i++) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX224][IO] Addr = 0x%04x, Data = 0x%02x", Addr+i, pTxData[i], 0U, 0U, 0U);
        TxDataBuf[0] = (UINT8)((Addr+i) & 0xff);
        TxDataBuf[1] = (UINT8)(((Addr+i)>>8) & 0xff);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "sendln 't drv i2c write 0 0x%02X 0x%02X 0x%02X 0x%02X", I2cConfig.SlaveAddr, TxDataBuf[1], TxDataBuf[0], pTxData[i], 0U);
    }
#endif
    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_RegisterWrite
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
static UINT32 MX01_IMX224_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)Data;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRW(pChan, Addr, &WData, 1U);
    }

    return RetVal;
}

static UINT32 MX01_IMX224_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if(pChan->SensorID == MX01_IMX224_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX01_IMX224SerdesConfig[pChan->VinID].SensorAliasID[0];
        } else if(pChan->SensorID == MX01_IMX224_SENSOR_ID_CHAN_B) {
            I2cTxConfig.SlaveAddr = MX01_IMX224SerdesConfig[pChan->VinID].SensorAliasID[1];
        } else if(pChan->SensorID == MX01_IMX224_SENSOR_ID_CHAN_C) {
            I2cTxConfig.SlaveAddr = MX01_IMX224SerdesConfig[pChan->VinID].SensorAliasID[2];
        } else if(pChan->SensorID == MX01_IMX224_SENSOR_ID_CHAN_D) {
            I2cTxConfig.SlaveAddr = MX01_IMX224SerdesConfig[pChan->VinID].SensorAliasID[3];
        } else {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX01_IMX224_RegRead] NG. incorrect sensor ID 0x%x", pChan->SensorID, 0U, 0U, 0U, 0U);
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

            RetVal = AmbaI2C_MasterReadAfterWrite(MX01_IMX224I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                                  &I2cRxConfig, &TxSize, 1000U);
        } else {
            //SENSOR_ERR
        }
    }
    if (RetVal != I2C_ERR_NONE) {
        RetVal = SENSOR_ERR_COMMUNICATE;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX224] Addr 0x%02x I2C does not work!!!!!", I2cTxConfig.SlaveAddr, Addr, 0U, 0U, 0U);
    } else {
        //SENSOR_ERR_NONE
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX9295_96712_IMX224_RegisterRead
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
static UINT32 MX01_IMX224_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX01_IMX224_RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      pChan:  pointer to Vin ID and sensor ID
 *      PGC:    Analog gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl)
{
    UINT8 WData[2];
    UINT32 RetVal;

    WData[0] = (UINT8)(AnalogGainCtrl & 0x00ffU);
    WData[1] = (UINT8)((AnalogGainCtrl & 0x0300U) >> 8U);

    RetVal = RegRW(pChan, MX01_IMX224_GAIN_LSB, WData, 2U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetConversionGainReg
 *
 *  @Description:: Configure sensor conversion gain setting
 *
 *  @Input      ::
 *      pChan:     pointer to Vin ID and sensor ID
 *      CG:        Conversion gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetConversionGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 CG)
{
    UINT8 RData = 0U, WData = 0U;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= MX01_IMX224_RegRead(pChan, MX01_IMX224_FRSEL, &RData);
    WData = ((UINT8)RData & (UINT8)~0x10U) | (UINT8)(CG << 4U);
    RetVal |= RegRW(pChan, MX01_IMX224_FRSEL, &WData, 1U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      pChan:              pointer to Vin ID and sensor ID
 *      NumXhsEshrSpeed:    Integration time in number of XHS period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 ShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 VMAX, SHS1;
    UINT8  WData[4];

    VMAX = MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame;
    SHS1 = VMAX - (ShutterCtrl + 1U);

    WData[0] = (UINT8)(SHS1 & 0x0000ffU);
    WData[1] = (UINT8)((SHS1 & 0x00ff00U) >> 8U);
    WData[2] = (UINT8)((SHS1 & 0x010000U) >> 16U);

    RetVal |= RegRW(pChan, MX01_IMX224_SHS1_LSB, WData, 3U);

    return RetVal;
}

#ifndef MX01_IMX224_IN_SLAVE_MODE
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetSlowShutterReg
 *
 *  @Description:: Apply slow shutter setting
 *
 *  @Input      ::
 *     pChan:                    pointer to Vin ID and sensor ID
 *     IntegrationPeriodInFrame: Number of frames in integation period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetSlowShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 FrameLengthLines)
{
    UINT8  TxData[4];

    TxData[0] = (FrameLengthLines & 0x0000ff) >> 0;
    TxData[1] = (FrameLengthLines & 0x00ff00) >> 8;
    TxData[2] = (FrameLengthLines & 0x010000) >> 16;

    RegRW(pChan, MX01_IMX224_VMAX_LSB, TxData, 3);
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetHdrShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      pChan:    pointer to Vin ID and sensor ID
 *      *pShutterCtrl:     Pointer to integration time in number of H period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 FSC, RHS1, RHS2, SHS1, SHS2, SHS3;
    UINT8  WData[4];
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels == 2U) {

        /* Long frame */
        FSC = MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame;
        SHS2 = FSC - (pShutterCtrl[0] + 1U);

        WData[0] = (UINT8)(SHS2 & 0x0000ffU);
        WData[1] = (UINT8)((SHS2 & 0x00ff00U) >> 8U);
        WData[2] = (UINT8)((SHS2 & 0x010000U) >> 16U);

        RetVal |= RegRW(pChan, MX01_IMX224_SHS2_LSB, WData, 3U);
        // AmbaPrint("[LEF] FSC: %u, SHS2: %u => %u H", FSC, SHS2, FSC - (SHS2 + 1));

        /* Short frame */
        RHS1 = (((UINT32)MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].EffectiveArea.StartY -
                 (UINT32)MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[0].EffectiveArea.StartY) * 2U) + 1U;
        SHS1 = RHS1 - (pShutterCtrl[1] + 1U);

        WData[0] = (UINT8)(SHS1 & 0x0000ffU);
        WData[1] = (UINT8)((SHS1 & 0x00ff00U) >> 8U);
        WData[2] = (UINT8)((SHS1 & 0x010000U) >> 16U);

        RetVal |= RegRW(pChan, MX01_IMX224_SHS1_LSB, WData, 3U);
        // AmbaPrint("[SEF1] RHS1: %u, SHS1: %u => %u H", RHS1, SHS1, RHS1 - (RHS1 + 1));

    } else if (MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels == 3U) {

        /* Long frame */
        FSC = MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame;
        SHS3 = FSC - (pShutterCtrl[0] + 1U);

        WData[0] = (UINT8)(SHS3 & 0x0000ffU);
        WData[1] = (UINT8)((SHS3 & 0x00ff00U) >> 8U);
        WData[2] = (UINT8)((SHS3 & 0x010000U) >> 16U);

        RetVal |= RegRW(pChan, MX01_IMX224_SHS3_LSB, WData, 3U);
        // AmbaPrint("[LEF] FSC: %u, SHS3: %u => %u H", FSC, SHS3, FSC - (SHS3 + 1));

        /* Short frame */
        RHS1 = (((UINT32)MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].EffectiveArea.StartY -
                 (UINT32)MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[0].EffectiveArea.StartY) * 3U) + 1U;
        SHS1 = RHS1 - (pShutterCtrl[1] + 1U);

        WData[0] = (UINT8)(SHS1 & 0x0000ffU);
        WData[1] = (UINT8)((SHS1 & 0x00ff00U) >> 8U);
        WData[2] = (UINT8)((SHS1 & 0x010000U) >> 16U);

        RetVal |= RegRW(pChan, MX01_IMX224_SHS1_LSB, WData, 3U);
        // AmbaPrint("[SEF1] RHS1: %u, SHS1: %u => %u H", RHS1, SHS1, RHS1 - (SHS1 + 1));

        /* Very short frame */
        RHS2 = (((UINT32)MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[2].EffectiveArea.StartY -
                 (UINT32)MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[0].EffectiveArea.StartY) * 3U) + 2U;
        SHS2 = RHS2 - (pShutterCtrl[2] + 1U);

        WData[0] = (UINT8)(SHS2 & 0x0000ffU);
        WData[1] = (UINT8)((SHS2 & 0x00ff00U) >> 8U);
        WData[2] = (UINT8)((SHS2 & 0x010000U) >> 16U);

        RetVal |= RegRW(pChan, MX01_IMX224_SHS2_LSB, WData, 3U);
        // AmbaPrint("[SEF2] RHS2: %u, SHS2: %u => %u H", RHS2, SHS2, RHS2 - (SHS2 + 1));
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[NG] Invalid HDR Active Channels: %d", MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels, 0U, 0U, 0U, 0U);
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_GetHdrInfo
 *
 *  @Description:: Get Sensor HDR information
 *
 *  @Input      ::
 *      pChan:            Vin ID and sensor ID
 *      pShutterCtrl:    Pointer to electronic shutter control
 *
 *  @Output     ::
 *      pHdrInfo:   senosr hdr information
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pHdrInfo, &MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal |= SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_StandbyOn
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
static UINT32 MX01_IMX224_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x01;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= RegRW(pChan, MX01_IMX224_STANDBY, &TxData, 1U);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_StandbyOff
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
static UINT32 MX01_IMX224_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x00;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRW(pChan, MX01_IMX224_STANDBY, &TxData, 1U);
    }

    return RetVal;
}

#ifdef MX01_IMX224_IN_SLAVE_MODE
static UINT32 MX01_IMX224_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID, const MX01_IMX224_FRAME_TIMING_s *pFrameTime)
{
    UINT32 RetVal;



    AMBA_VIN_MASTER_SYNC_CONFIG_s MX01_IMX224MasterSyncConfig = {
        .RefClk = MX01_IMX224_SensorInfo[ModeID].InputClk,
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
        .HSyncDelayCycles = 1,
        .VSyncDelayCycles = 0,
        .ToggleHsyncInVblank = 1
    };

    AMBA_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncCfg = &MX01_IMX224MasterSyncConfig;

    /* tWLXHS >= 4/fINCK, tWHXHS >= 4/fINCK, tHFDLY >= 1/fINCK, tVRDLY >= 1/fINCK */
    pMasterSyncCfg->HSync.Period = pFrameTime->NumTickPerXhs;
    pMasterSyncCfg->HSync.PulseWidth = 6U;
    pMasterSyncCfg->VSync.Period = pFrameTime->NumXhsPerV / pFrameTime->NumXvsPerV;
    pMasterSyncCfg->VSync.PulseWidth = 8U;

    RetVal = AmbaVIN_MasterSyncEnable(MX01_IMX224MasterSyncChannel[pChan->VinID], pMasterSyncCfg);

    if (RetVal == VIN_ERR_NONE) {
        RetVal = SENSOR_ERR_NONE;
    } else {
        RetVal = SENSOR_ERR_INVALID_API;
    }

    return RetVal;
}
#else
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_StartMasterMode
 *
 *  @Description:: Start sensor master mode
 *
 *  @Input      ::
 *      pChan:                      Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_StartMasterMode(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x00U;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= RegRW(pChan, MX01_IMX224_XMSTA, &TxData, 1U);

    return RetVal;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetInitRegs
 *
 *  @Description:: Set initial register settings
 *
 *  @Input      ::
 *      pChan:      Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetInitRegs(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i;
    UINT16 Addr;
    UINT8  Data;
    UINT32 RetVal = SENSOR_ERR_NONE;

    for (i = 0U; i < MX01_IMX224_NUM_INIT_REG; i++) {
        Addr = MX01_IMX224_InitRegTable[i].Addr;
        Data = MX01_IMX224_InitRegTable[i].Data;
        RetVal |= RegRW(pChan, Addr, &Data, 1U);
    }

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      pChan:       pointer to Vin ID and sensor ID
 *      SensorMode : Sensor mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 i;
    UINT16 Addr;
    UINT8  Data;
    UINT32 RetVal = SENSOR_ERR_NONE;

    for (i = 0U; i < MX01_IMX224_NUM_READOUT_MODE_REG; i++) {
        Addr = MX01_IMX224_ModeRegTable[i].Addr;
        Data = MX01_IMX224_ModeRegTable[i].Data[SensorMode];
        RetVal |= RegRW(pChan, Addr, &Data, 1U);
    }

    for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i++) {
        if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
            /* reset gain/shutter ctrl information */
            MX01_IMX224Ctrl[pChan->VinID].CurrentShrCtrl[i][0] = 0xffffffffU;
            MX01_IMX224Ctrl[pChan->VinID].CurrentShrCtrl[i][1] = 0xffffffffU;
            MX01_IMX224Ctrl[pChan->VinID].CurrentShrCtrl[i][2] = 0xffffffffU;
            MX01_IMX224Ctrl[pChan->VinID].CurrentGainCtrl[i] = 0xffffffffU;
            MX01_IMX224Ctrl[pChan->VinID].CurrentCGCtrl[i] = 0U;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX01_IMX224_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;
#ifdef MX01_IMX224_IN_SLAVE_MODE
    const MX01_IMX224_FRAME_TIMING_s *pFrameTime = &MX01_IMX224_ModeInfoList[0].FrameTime;
#else
    const MX01_IMX224_SENSOR_INFO_s *pSensorInfo = NULL;
#endif

    for (i = 0U; i < MX01_IMX224_NUM_MODE; i++) {
#ifdef MX01_IMX224_IN_SLAVE_MODE
        pFrameTime = &MX01_IMX224_ModeInfoList[i].FrameTime;
        MX01_IMX224_ModeInfoList[i].RowTime = (FLOAT) ((DOUBLE) pFrameTime->NumTickPerXhs / (DOUBLE) pFrameTime->VinRefClk);
#else
        pSensorInfo = &MX01_IMX224_SensorInfo[i];
        MX01_IMX224_ModeInfoList[i].RowTime = (FLOAT) ((DOUBLE) pSensorInfo->LineLengthPck /
                                              ( (DOUBLE) pSensorInfo->DataRate *
                                                (DOUBLE) pSensorInfo->NumDataLanes /
                                                (DOUBLE) pSensorInfo->NumDataBits) );
#endif
        pOutputInfo = &MX01_IMX224_OutputInfo[i];
        MX01_IMX224_ModeInfoList[i].PixelRate = (FLOAT) ((DOUBLE) pOutputInfo->DataRate *
                                                (DOUBLE)pOutputInfo->NumDataLanes /
                                                (DOUBLE)pOutputInfo->NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_Init
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
static UINT32 MX01_IMX224_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const MX01_IMX224_FRAME_TIMING_s *pFrameTime = &MX01_IMX224_ModeInfoList[0U].FrameTime;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->VinRefClk);

        MX01_IMX224_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_Enable
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
static UINT32 MX01_IMX224_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX224] Enable", NULL, NULL, NULL, NULL, NULL);


    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX01_IMX224_ResetSensor(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_Disable
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
static UINT32 MX01_IMX224_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX224] Disable", NULL, NULL, NULL, NULL, NULL);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX01_IMX224_StandbyOn(pChan);
    }


    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_GetStatus
 *
 *  @Description:: Get current sensor status
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pStatus:    pointer to current sensor status
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pStatus == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &MX01_IMX224Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_GetModeInfo
 *
 *  @Description:: Get Mode Info of indicated mode
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      Mode:   Sensor Readout Mode Number
 *
 *  @Output     ::
 *      pModeInfo: pointer to requested Mode Info
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_SENSOR_CONFIG_s SensorConfig;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        SensorConfig = *pMode;
        if (SensorConfig.ModeID == AMBA_SENSOR_CURRENT_MODE) {
            SensorConfig.ModeID = MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        }

        if (SensorConfig.ModeID >= MX01_IMX224_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            RetVal |= MX01_IMX224_PrepareModeInfo(pChan, &SensorConfig, pModeInfo);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_GetDeviceInfo
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
static UINT32 MX01_IMX224_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pDeviceInfo, &MX01_IMX224_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_GetCurrentGainFactor
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
static UINT32 MX01_IMX224_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 i;
    UINT32 CurrentGainCtrl, CurrentCGCtrl, CG_Factor;
    DOUBLE GainFactor64 = 0.0;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) ||(pGainFactor == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                CurrentGainCtrl = MX01_IMX224Ctrl[pChan->VinID].CurrentGainCtrl[i];
                CurrentCGCtrl = MX01_IMX224Ctrl[pChan->VinID].CurrentCGCtrl[i];
                CG_Factor = (UINT32)1U << CurrentCGCtrl;
                if (AmbaWrap_pow(10.0, (DOUBLE)CurrentGainCtrl / 200.0, &GainFactor64) != ERR_NONE) {
                    RetVal |= SENSOR_ERR_UNEXPECTED;
                }
                *pGainFactor = (FLOAT)(GainFactor64 * (DOUBLE)CG_Factor);
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_GetCurrentShutterSpeed
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
static UINT32 MX01_IMX224_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 i, k;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo;
        for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k++) {
                    pExposureTime[k] = pModeInfo->RowTime * (FLOAT)MX01_IMX224Ctrl[pChan->VinID].CurrentShrCtrl[i][k];
                }
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_ConvertGainFactor
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
static UINT32 MX01_IMX224_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 HdrType;
    UINT32 ConversionGainCtrl; /* 0: LCG Mode, 1: HCG Mode */
    UINT32 CG_Factor;
    DOUBLE DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
    DOUBLE LogDesiredFactor = 0.0;
    DOUBLE AnalogGainCtrlInDb = 0.0;
    DOUBLE ActualFactor64 = 0.0;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        HdrType = MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
        if (DesiredFactor > MX01_IMX224_DeviceInfo.MaxAnalogGainFactor) {
            pGainCtrl->AnalogGain[0] = 720;
            ConversionGainCtrl = 1;
        } else {
            if (DesiredFactor <= 31.6227766 /* 30 dB */) {
                ConversionGainCtrl = 0;
            } else {
                ConversionGainCtrl = 1;
            }
            CG_Factor = (UINT32)1U << ConversionGainCtrl;
            if (AmbaWrap_log10((DesiredFactor / (DOUBLE)CG_Factor), &LogDesiredFactor) != ERR_NONE) {
                RetVal |= SENSOR_ERR_UNEXPECTED;
            }
            if (AmbaWrap_floor((200.0 * LogDesiredFactor), &AnalogGainCtrlInDb) != ERR_NONE) {
                RetVal |= SENSOR_ERR_UNEXPECTED;
            }
            pGainCtrl->AnalogGain[0] = (UINT32)AnalogGainCtrlInDb;
        }

        CG_Factor = (UINT32)1U << ConversionGainCtrl;
        pGainCtrl->DigitalGain[0] = ConversionGainCtrl;
        if (AmbaWrap_pow(10.0, (DOUBLE)pGainCtrl->AnalogGain[0] / 200.0, &ActualFactor64) != ERR_NONE) {
            RetVal |= SENSOR_ERR_UNEXPECTED;
        }
        pActualFactor->Gain[0] = (FLOAT)(ActualFactor64 * (DOUBLE)CG_Factor);

        // AmbaPrint("DesireFactor:%f, ActualFactor:%u, AnalogGainCtrl:0x%04X, DigitalGainCtrl: 0x%04X",
        //           DesiredFactor, *pActualFactor, *pAnalogGainCtrl, *pDigitalGainCtrl);

        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            pActualFactor->Gain[1] = pActualFactor->Gain[0];
            pActualFactor->Gain[2] = pActualFactor->Gain[0];

            pGainCtrl->AnalogGain[1] = pGainCtrl->AnalogGain[0];
            pGainCtrl->AnalogGain[2] = pGainCtrl->AnalogGain[0];

            pGainCtrl->DigitalGain[1] = pGainCtrl->DigitalGain[0];
            pGainCtrl->DigitalGain[2] = pGainCtrl->DigitalGain[0];
        }
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
static UINT32 ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 ExposureLineMaxMargin = 3U;
    UINT32 MaxExposureLine, MinExposureLine;
    DOUBLE ShutterCtrlInDb;
    UINT32 ShutterCtrl;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (AmbaWrap_floor((DOUBLE)ExposureTime / ((DOUBLE)pModeInfo->RowTime * ((DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame)), &ShutterCtrlInDb) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    /* For HDR mode (not support slow-shutter) */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MinExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
        MaxExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        /* For none-HDR mode (not support slow-shutter) */
    } else {
        MaxExposureLine = FrameLengthLines - ExposureLineMaxMargin;
        MinExposureLine = 1U;
    }

    if (ShutterCtrl > MaxExposureLine) {
        ShutterCtrl = MaxExposureLine;
    } else if (ShutterCtrl < MinExposureLine) {
        ShutterCtrl = MinExposureLine;
    } else {
        /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl = ShutterCtrl;
    *pActualExptime = (FLOAT)ShutterCtrl * (pModeInfo->RowTime * ((FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame));

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_ConvertShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      pChan:           Vin ID and sensor ID
 *      ExposureTime:   Exposure time / shutter speed
 *
 *  @Output     ::
 *      pShutterCtrl:   Electronic shutter control
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 i;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i++) {
                RetVal |= ConvertShutterSpeed(pModeInfo, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
        } else {
            RetVal |= ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        }
        // AmbaPrint("ExposureTime:%f, ShutterCtrl:0x%04X", ExposureTime, *pShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetAnalogGainCtrl
 *
 *  @Description:: Set analog gain control
 *
 *  @Input      ::
 *      pChan:               Vin ID and sensor ID
 *      AnalogGainCtrl:     Analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 i;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX01_IMX224_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

        /* Update current analog gain control */
        for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                MX01_IMX224Ctrl[pChan->VinID].CurrentGainCtrl[i] = pAnalogGainCtrl[0];
            }
        }
    }

    //SENSOR_DBG_GAIN("[Vin%d/Sensor%d/Hdr%d][IMX224][GAIN] AnalogGainCtrl = 0x%x",
    //                pChan->VinID, pChan->SensorID, pChan->HdrID, AnalogGainCtrl);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      pChan:            Vin ID and sensor ID
 *      pDigitalGainCtrl: Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 i;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX01_IMX224_SetConversionGainReg(pChan, pDigitalGainCtrl[0]);

        /* Update current digital gain control */
        for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                MX01_IMX224Ctrl[pChan->VinID].CurrentCGCtrl[i] = pDigitalGainCtrl[0];
            }
        }
    }

    //SENSOR_DBG_GAIN("[Vin%d/Sensor%d/Hdr%d][IMX224][GAIN] DigitalGainCtrl = 0x%x",
    //                pChan->VinID, pChan->SensorID, pChan->HdrID, DigitalGainCtrl);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetShutterCtrl
 *
 *  @Description:: Set shutter control
 *
 *  @Input      ::
 *      pChan:           Vin ID and sensor ID
 *      pShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 i, k;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo;
        /* HDR mode */
        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
            RetVal = MX01_IMX224_SetHdrShutterReg(pChan, pShutterCtrl);

            for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k++) {
                        MX01_IMX224Ctrl[pChan->VinID].CurrentShrCtrl[i][k] = pShutterCtrl[k];
                    }
                }
            }
            /* None-HDR mode */
        } else {
            RetVal = MX01_IMX224_SetShutterReg(pChan, pShutterCtrl[0]);

            for (i = 0U; i < MX01_IMX224_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    MX01_IMX224Ctrl[pChan->VinID].CurrentShrCtrl[i][0] = pShutterCtrl[0];
                }
            }
        }
    }


    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_SetSlowShutterCtrl
 *
 *  @Description:: set slow shutter control
 *
 *  @Input      ::
 *      pChan:             Vin ID and sensor ID
 *      SlowShutterCtrl:  Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX224_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    (void)pChan;
    (void)SlowShutterCtrl;

    return SENSOR_ERR_NONE;
#if 0
    UINT32 TargetFrameLengthLines;
    UINT32 SensorMode = MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.Mode.Bits.Mode;
    UINT32 NumExposureStepPerFrame = MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame;
    UINT32 FrameLengthLines = MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines;
#ifdef MX01_IMX224_IN_SLAVE_MODE
    MX01_IMX224_FRAME_TIMING_s *pFrameTime = &MX01_IMX224Ctrl[pChan->VinID].FrameTime;
    MX01_IMX224_FRAME_TIMING_s TargetFrameTime;
#endif
    if (MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE)
        return NG;

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    TargetFrameLengthLines = MX01_IMX224_SensorInfo[SensorMode].FrameLengthLines * SlowShutterCtrl;

#ifdef MX01_IMX224_IN_SLAVE_MODE
    memcpy(&TargetFrameTime, pFrameTime, sizeof(MX01_IMX224_FRAME_TIMING_s));
    TargetFrameTime.NumXhsPerV *= SlowShutterCtrl;

    MX01_IMX224_ConfigMasterSync(&TargetFrameTime);
#else
    MX01_IMX224_SetSlowShutterReg(pChan, TargetFrameLengthLines);
#endif

    MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines * (FrameLengthLines / NumExposureStepPerFrame);
    MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo.FrameTime.FrameRate.NumUnitsInTick =
        MX01_IMX224_ModeInfoList[SensorMode].FrameTime.FrameRate.NumUnitsInTick * SlowShutterCtrl;

    SENSOR_DBG_SHR("[Vin%d/Sensor%d/Hdr%d][IMX224][SHR] SlowShutterCtrl = 0x%x",
                   pChan->VinID, pChan->SensorID, pChan->HdrID, SlowShutterCtrl);

    return OK;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX224_Config
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
static UINT32 MX01_IMX224_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s MX01_IMX224PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX01_IMX224Ctrl[pChan->VinID].Status.ModeInfo;
    UINT16 SensorMode = (UINT16)pMode->ModeID;
    MX01_IMX224_FRAME_TIMING_s *pFrameTime = &MX01_IMX224Ctrl[pChan->VinID].FrameTime;
    UINT32 RetVal = SENSOR_ERR_NONE;
    //power off and on power control
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"power off MAXIM", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"power on MAXIM", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(1);

    if (SensorMode >= MX01_IMX224_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  [ MAX9295_96712_IMX224 Config ] ============", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d ============", SensorMode, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

        if((pChan->SensorID != MX01_IMX224_SENSOR_ID_CHAN_A) && (pChan->SensorID != (MX01_IMX224_SENSOR_ID_CHAN_A | MX01_IMX224_SENSOR_ID_CHAN_B | MX01_IMX224_SENSOR_ID_CHAN_C | MX01_IMX224_SENSOR_ID_CHAN_D))) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Sensor_Config] NG, Sensor ID 0x%x. Only Support 1ch(A) and 4ch", pChan->SensorID, 0U, 0U, 0U, 0U);
            RetVal = SENSOR_ERR_ARG;
        } else {
            //supported SensorID
        }

        /* update status */
        RetVal |= MX01_IMX224_PrepareModeInfo(pChan, pMode, pModeInfo);
        if (AmbaWrap_memcpy(pFrameTime, &MX01_IMX224_ModeInfoList[SensorMode].FrameTime, sizeof(MX01_IMX224_FRAME_TIMING_s)) != ERR_NONE) {
            RetVal |= SENSOR_ERR_UNEXPECTED;
        }

#ifdef MX01_IMX224_IN_SLAVE_MODE
        (void)AmbaVIN_MasterSyncDisable(MX01_IMX224MasterSyncChannel[pChan->VinID]);
#endif

        /* Adjust mipi-phy parameters */
        MX01_IMX224PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", (UINT32)MX01_IMX224PadConfig.DateRate, 0U, 0U, 0U, 0U);
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX01_IMX224PadConfig);

        /* After reset VIN, Set MAX96712A PWDNB high for new MAX96712*/
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"PWDN low", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(1);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"PWDN HIGH", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_96712_Init(pChan->VinID, MX01_IMX224_GetEnabledLinkID(pChan->SensorID));

        RetVal |= MX01_IMX224_ConfigSerDes(pChan, SensorMode, pModeInfo);

        //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Max9295_96712_SetSensorClk", NULL, NULL, NULL, NULL, NULL);
        //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "; # Link A: MAX9295A MFP2 to output 27M for IMX224", NULL, NULL, NULL, NULL, NULL);
        //TBD: 37.125M Hz
        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_A) != 0U) {
            (void)Max9295_96712_SetSensorClk(pChan->VinID, MX01_MAX9295_A_ID, MAX9295_96712_RCLK_OUT_FREQ_37P125M);
        } else {
            //sensor ID not 0x1
        }
        if ((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_B) != 0U) {
            (void)Max9295_96712_SetSensorClk(pChan->VinID, MX01_MAX9295_B_ID, MAX9295_96712_RCLK_OUT_FREQ_37P125M);
        } else {
            //sensor ID not 0x2
        }
        if ((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_C) != 0U) {
            (void)Max9295_96712_SetSensorClk(pChan->VinID, MX01_MAX9295_C_ID, MAX9295_96712_RCLK_OUT_FREQ_37P125M);
        } else {
            //sensor ID not 0x4
        }
        if ((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_D) != 0U) {
            (void)Max9295_96712_SetSensorClk(pChan->VinID, MX01_MAX9295_D_ID, MAX9295_96712_RCLK_OUT_FREQ_37P125M);
        } else {
            //sensor ID not 0x8
        }
        RetVal |= MX01_IMX224_ResetSensor(pChan);

        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Sensor I2C translation 0x%02x", pChan->SensorID, 0, 0, 0, 0);
        //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "; # Sensor IMX224 I2C tanslation", NULL, NULL, NULL, NULL, NULL);
        /* Sensor I2C translation */

        if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_A) != 0U) {
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_A_ID, 0x0042U, MX01_IMX224SerdesConfig[pChan->VinID].SensorAliasID[0]);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_A_ID, 0x0043U, MX01_IMX224_I2C_SLAVE_ADDRESS);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_A_ID, 0x0044U, MX01_IMX224_I2C_BC_SLAVE_ADDRESS);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_A_ID, 0x0045U, MX01_IMX224_I2C_SLAVE_ADDRESS);
        }
        if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_B) != 0U) {
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_B_ID, 0x0042U, MX01_IMX224SerdesConfig[pChan->VinID].SensorAliasID[1]);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_B_ID, 0x0043U, MX01_IMX224_I2C_SLAVE_ADDRESS);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_B_ID, 0x0044U, MX01_IMX224_I2C_BC_SLAVE_ADDRESS);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_B_ID, 0x0045U, MX01_IMX224_I2C_SLAVE_ADDRESS);
        }
        if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_C) != 0U) {
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_C_ID, 0x0042U, MX01_IMX224SerdesConfig[pChan->VinID].SensorAliasID[2]);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_C_ID, 0x0043U, MX01_IMX224_I2C_SLAVE_ADDRESS);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_C_ID, 0x0044U, MX01_IMX224_I2C_BC_SLAVE_ADDRESS);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_C_ID, 0x0045U, MX01_IMX224_I2C_SLAVE_ADDRESS);
        }
        if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_D) != 0U) {
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_D_ID, 0x0042U, MX01_IMX224SerdesConfig[pChan->VinID].SensorAliasID[3]);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_D_ID, 0x0043U, MX01_IMX224_I2C_SLAVE_ADDRESS);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_D_ID, 0x0044U, MX01_IMX224_I2C_BC_SLAVE_ADDRESS);
            (void)Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_D_ID, 0x0045U, MX01_IMX224_I2C_SLAVE_ADDRESS);
        }



        /* program sensor */
        //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "program sensor", NULL, NULL, NULL, NULL, NULL);

        RetVal |= MX01_IMX224_SetInitRegs(pChan);
        RetVal |= MX01_IMX224_ChangeReadoutMode(pChan, SensorMode);

        RetVal |= MX01_IMX224_StandbyOff(pChan);
        (void)AmbaKAL_TaskSleep(20); /* wait for initial regulator stabilization period */

#if 0 //no need to do one shot reset by FAE's suggestion
        if(pChan->SensorID != 0x10U) {
            //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Derserializer one-shot link reset", NULL, NULL, NULL, NULL, NULL);
            //Derserializer one-shot link reset
            RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, AMBA_SBRG_MAX96712_ID, 0x0018U, 0x0FU);
            (void)AmbaKAL_TaskSleep(100);

            //Also issue one-shot linke reset for MAX9295A
            if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_A) != 0U) {
                RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_A_ID, 0x0010U, 0x21U);
                (void)AmbaKAL_TaskSleep(100);
            }
            if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_B) != 0U) {
                RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_B_ID, 0x0010U, 0x21U);
                (void)AmbaKAL_TaskSleep(100);
            }
            if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_C) != 0U) {
                RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_C_ID, 0x0010U, 0x21U);
                (void)AmbaKAL_TaskSleep(100);
            }
            if((pChan->SensorID & MX01_IMX224_SENSOR_ID_CHAN_D) != 0U) {
                RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, MX01_MX01_MAX9295_D_ID, 0x0010U, 0x21U);
                (void)AmbaKAL_TaskSleep(100);
            }
            //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Derserializer Concatentation", NULL, NULL, NULL, NULL, NULL);
            //MAX96712 Concatentation: still debug
            //RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, AMBA_SBRG_MAX96712_ID, 0x0971U, MX01_IMX224SerdesConfig[pChan->VinID].EnabledLinkID); //[TBD] 11/19 set concate during MAXIM setting
        }
#endif

#ifdef MX01_IMX224_IN_SLAVE_MODE
        RetVal |= MX01_IMX224_ConfigMasterSync(pChan, SensorMode, pFrameTime);
#else
        RetVal |= MX01_IMX224_StartMasterMode(pChan);
#endif

        /* config vin */
        RetVal |= MX01_IMX224_ConfigVin(pChan->VinID, pModeInfo);
    }

    return RetVal;
}

#if 0
static INT32 MX01_IMX224_ConfigPost(UINT32 *pVinID)
{
    MX01_IMX224_StandbyOff();

    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX01_IMX224Obj = {
    .SensorName             = "IMX224",
    .SerdesName             = "MAX9295_96712",
    .Init                   = MX01_IMX224_Init,
    .Enable                 = MX01_IMX224_Enable,
    .Disable                = MX01_IMX224_Disable,
    .Config                 = MX01_IMX224_Config,
    .GetStatus              = MX01_IMX224_GetStatus,
    .GetModeInfo            = MX01_IMX224_GetModeInfo,
    .GetDeviceInfo          = MX01_IMX224_GetDeviceInfo,
    .GetHdrInfo             = MX01_IMX224_GetHdrInfo,
    .GetCurrentGainFactor   = MX01_IMX224_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX01_IMX224_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX01_IMX224_ConvertGainFactor,
    .ConvertShutterSpeed    = MX01_IMX224_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX01_IMX224_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MX01_IMX224_SetDigitalGainCtrl,
    .SetShutterCtrl         = MX01_IMX224_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX01_IMX224_SetSlowShutterCtrl,

    .RegisterRead           = MX01_IMX224_RegisterRead,
    .RegisterWrite          = MX01_IMX224_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,

};

