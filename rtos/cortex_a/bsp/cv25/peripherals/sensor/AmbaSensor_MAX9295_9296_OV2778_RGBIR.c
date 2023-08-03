/**
 *  @file AmbaSensor_MX00_OV2778.c
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
 *  @details Control APIs of SONY MX00_OV2778 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_OV2778_RGBIR.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "bsp.h"
#include "AmbaSbrg_Max9295_9296.h"
#define MX00_OV2778_SERDES_ENABLE
static UINT16 SMode;

//#include "AmbaUtility.h"
static UINT32 MX00_OV2778_I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
};



/*-----------------------------------------------------------------------------------------------*\
 * MX00_OV2778 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX00_OV2778_CTRL_s MX00_OV2778Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};



#ifdef MX00_OV2778_SERDES_ENABLE
static MAX9295_9296_SERDES_CONFIG_s MX00_OV2778SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR + 0x10,
            [1] = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR + 0x12,
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [1] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR + 0x14,
            [1] = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR + 0x16,
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    //[2] = {
    //    .EnabledLinkID   = 0x01,
    //    .SensorSlaveID   = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR,
    //    .SensorAliasID   = {
    //        [0] = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR + 0x20,
    //        [1] = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR + 0x22,
    //    },
    //    .NumDataBits     = 12,
    //    .CSIRxLaneNum    = 4,
    //    .CSITxLaneNum    = {4, 4},
    //    .CSITxSpeed      = {10, 10},
    //},
    //[3] = {
    //    .EnabledLinkID   = 0x01,
    //    .SensorSlaveID   = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR,
    //    .SensorAliasID   = {
    //        [0] = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR + 0x24,
    //        [1] = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR + 0x26,
    //    },
    //    .NumDataBits     = 12,
    //    .CSIRxLaneNum    = 4,
    //    .CSITxLaneNum    = {4, 4},
    //    .CSITxSpeed      = {10, 10},
    //},
};
#else
static MAX9295_9296_SERDES_CONFIG_s MX00_OV2778SerdesConfig[MX00_OV2778_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] =  (UINT8)(MX00_OV2778_SENSOR_I2C_SLAVE_ADDR),
            [1] =  (UINT8)(MX00_OV2778_SENSOR_I2C_SLAVE_ADDR),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
    },
};

#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_GetNumActiveSensor
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
static UINT32 MX00_OV2778_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 1U;
    //UINT32 i;

    //for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i ++) {
    //    if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
    //        SensorNum ++;
    //    }
    //}
    if(SensorID == 0U) { /* MisraC */}
    //return (SensorNum > 1U) ? SensorNum : 1U;
    return SensorNum;
}


#ifdef MX00_OV2778_SERDES_ENABLE
static UINT16 MX00_OV2778_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 i, EnabledLinkID = 0U;

    for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_OV2778_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 SensorMode = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s *pInputInfo = &MX00_OV2778_InputInfo[SensorMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &MX00_OV2778_OutputInfo[SensorMode];
    const MX00_OV2778_SENSOR_INFO_s *pSensorInfo = &MX00_OV2778_SensorInfo[SensorMode];
    const MX00_OV2778_FRAME_TIMING_s *pFrameTime = &MX00_OV2778_ModeInfoList[SensorMode].FrameTime;
    UINT32 SensorNum = MX00_OV2778_GetNumActiveSensor(pChan->SensorID);
    // UINT32 FloorVal;
    DOUBLE LLP;
    UINT32 U32RVal = 0U;

    //pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;

    pModeInfo->FrameLengthLines = pSensorInfo->FrameLengthLines * SensorNum;
    if (MX00_OV2778_HdrInfo[SensorMode].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
        pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines/2U;
    } else {
        pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    }
    //pModeInfo->PixelRate =  (FLOAT)((DOUBLE)pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits);
    LLP = ((DOUBLE)MX00_OV2778_ModeInfoList[SensorMode].PixelRate*(DOUBLE)pSensorInfo->LineLengthPck)/120000000.0f;
    pModeInfo-> LineLengthPck =  (UINT32)LLP;
    pModeInfo->InternalExposureOffset = 0.0f;
    pModeInfo->RowTime = (FLOAT) pModeInfo->LineLengthPck / MX00_OV2778_ModeInfoList[SensorMode].PixelRate;

    pModeInfo->InputClk = pSensorInfo->InputClk;

    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX00_OV2778_HdrInfo[SensorMode], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information


    /* Updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        pModeInfo->MinFrameRate.NumUnitsInTick /= 8U;
    }

    if(U32RVal != 0U){
        /* MisraC */
    }
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_ConfigVin
 *
 *  @Description:: Configure VIN to receive output frames of the new readout mode
 *
 *  @Input      ::
 *      pChan:       Vin ID and sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX00_OV2778VinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace         = AMBA_VIN_COLOR_SPACE_RGBIR,
            .BayerPattern       = AMBA_VIN_BAYER_PATTERN_BGGI,
            .NumDataBits        = 0,
            .NumSkipFrame       = 1U, /* wait for initialization period */
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
        .DataType               = 0x2EU,
        .DataTypeMask           = 0x3FU,//0x1fU,
        .VirtChanHDREnable      = 0U,
        .VirtChanHDRConfig      = {0},
    };

    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s *pVinCfg = &MX00_OV2778VinConfig;
    UINT32 RetVal;
    UINT32 U32RVal = 0U;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Config.NumDataBits = pImgInfo->NumDataBits;
    pVinCfg->Config.BayerPattern = pImgInfo->BayerPattern;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    U32RVal |= AmbaWrap_memcpy(&pVinCfg->Config.FrameRate, &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "NumActiveLanes=%d,NumDataBits=%d, NumActivePixels=%d, NumActiveLines=%d, NumTotalPixels=%d ",
    //                     pVinCfg->NumActiveLanes, pVinCfg->Config.NumDataBits, pVinCfg->Config.RxHvSyncCtrl.NumActivePixels, pVinCfg->Config.RxHvSyncCtrl.NumActiveLines, pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels);
    RetVal = AmbaVIN_MipiConfig(pChan->VinID, pVinCfg);

    if (RetVal == VIN_ERR_NONE) {
        RetVal = SENSOR_ERR_NONE;
    } else {
        RetVal = SENSOR_ERR_INVALID_API;
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_ConfigSerDes
 *
 *  @Description:: Configure Serdes sync/video forwarding, I2C translation, and MIPI I/F
 *
 *  @Input      ::
 *      pChan:        Vin ID and sensor ID
 *      SensorMode:  Sensor mode
 *      pModeInfo:   Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
#ifdef MX00_OV2778_SERDES_ENABLE
static void MX00_OV2778_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s *pImgInfo = &pModeInfo->OutputInfo;
    const MX00_OV2778_SENSOR_INFO_s *pSensorInfo = &MX00_OV2778_SensorInfo[SensorMode];
    MAX9295_9296_SERDES_CONFIG_s *pSerdesCfg = &MX00_OV2778SerdesConfig[pChan->VinID];
    DOUBLE FloorVal;
    UINT32 U32RVal = 0U;

    pSerdesCfg->EnabledLinkID = MX00_OV2778_GetEnabledLinkID(pChan->SensorID);
    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;

    U32RVal |= AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal);
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    (void) Max9295_9296_Config(pChan->VinID, &MX00_OV2778SerdesConfig[pChan->VinID]);

    if(U32RVal != 0U){
        /* MisraC */
    }
}
#endif


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_HardwareReset
 *
 *  @Description:: Reset IMX290_MIPI Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MX00_OV2778_HardwareReset(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

#ifdef MX00_OV2778_SERDES_ENABLE
    if ((pChan->SensorID & MX00_OV2778_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    if ((pChan->SensorID & MX00_OV2778_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
    if ((pChan->SensorID & MX00_OV2778_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    if ((pChan->SensorID & MX00_OV2778_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start >= 20us*/
#else
    RetVal = AmbaUserGPIO_SensorResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    if (RetVal == GPIO_ERR_NONE) {
        (void)AmbaKAL_TaskSleep(2);
        RetVal = AmbaUserGPIO_SensorResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        //RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_113, AMBA_GPIO_LEVEL_HIGH);
    }
    (void)AmbaKAL_TaskSleep(30);
#endif
    if (RetVal != MAX9295_9296_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_RegRead
 *
 *  @Description:: Read sensor registers
 *
 *  @Input      ::
 *      pChan:      Vin ID and sensor ID
 *      Addr:      Register Address
 *
 *  @Output     ::
 *      pRxData:   Pointer to Read data buffer
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*//*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_RegRW
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
    UINT32 i, k;
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX00_OV2778_I2C_WR_BUF_SIZE + 2U];
    UINT32 TxSize;

    if ((pChan == NULL) || ((pChan->SensorID & (MX00_OV2778_SENSOR_ID_CHAN_A | MX00_OV2778_SENSOR_ID_CHAN_B)) == 0x0U)
        || (Size > MX00_OV2778_I2C_WR_BUF_SIZE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        I2cConfig.DataSize  = Size + 2U;
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8);
        TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);

        for (i = 0; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

        if (pChan->SensorID == (MX00_OV2778_SENSOR_ID_CHAN_A | MX00_OV2778_SENSOR_ID_CHAN_B)) { /* broadcast to all sensors */
            I2cConfig.SlaveAddr = MX00_OV2778_SENSOR_I2C_SLAVE_ADDR;
            RetVal |= AmbaI2C_MasterWrite(MX00_OV2778_I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD, &I2cConfig, &TxSize, 1000U);

            if (RetVal != I2C_ERR_NONE) {
                RetVal = SENSOR_ERR_COMMUNICATE;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work (SlaveID=0x%02x)!!!!!", I2cConfig.SlaveAddr, 0, 0, 0, 0);
            }
        } else {
            for (k = 0; k < MX00_OV2778_NUM_MAX_SENSOR_COUNT; k++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (k + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_OV2778SerdesConfig[pChan->VinID].SensorAliasID[k];
                    RetVal |= AmbaI2C_MasterWrite(MX00_OV2778_I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD, &I2cConfig, &TxSize, 1000U);
                    break;
                }
            }

            if (RetVal != I2C_ERR_NONE) {
                RetVal = SENSOR_ERR_COMMUNICATE;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work (SlaveID=0x%02x)!!!!!", I2cConfig.SlaveAddr, 0, 0, 0, 0);
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_RegisterWrite
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
static UINT32 MX00_OV2778_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
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

static UINT32 RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxDataBuf[2];
    UINT32 TxSize;

    if ((pChan == NULL) || ((pChan->SensorID != MX00_OV2778_SENSOR_ID_CHAN_A) && (pChan->SensorID != MX00_OV2778_SENSOR_ID_CHAN_B))) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        I2cTxConfig.DataSize = 2U;
        I2cTxConfig.pDataBuf = TxDataBuf;
        TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8);
        TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);

        I2cRxConfig.DataSize = 1U;
        I2cRxConfig.pDataBuf = pRxData;

        if (pChan->SensorID == MX00_OV2778_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX00_OV2778SerdesConfig[pChan->VinID].SensorAliasID[0];
            I2cRxConfig.SlaveAddr = (MX00_OV2778SerdesConfig[pChan->VinID].SensorAliasID[0] | (UINT32)0x1U);
        } else {
            /* (pChan->SensorID == MX00_OV2778_SENSOR_ID_CHAN_B) */
            I2cTxConfig.SlaveAddr = MX00_OV2778SerdesConfig[pChan->VinID].SensorAliasID[1];
            I2cRxConfig.SlaveAddr = (MX00_OV2778SerdesConfig[pChan->VinID].SensorAliasID[1] | (UINT32)0x1U);
        }

        RetVal = AmbaI2C_MasterReadAfterWrite(MX00_OV2778_I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD, 1U,
                                              &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);
        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C(Read) does not work (SlaveID=0x%02x)!!!!!", I2cTxConfig.SlaveAddr, 0, 0, 0, 0);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_RegisterRead
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
static UINT32 MX00_OV2778_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData = 0U;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}


static inline UINT32 MX00_OV2778_SoftwareReset(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    RetVal |= MX00_OV2778_RegisterWrite(pChan, MX00_OV2778_SFW_CTRL2_SW_RST, 0x01U);// Sensor software reset
    (void) AmbaKAL_TaskSleep(10);// Delay 10ms after sensor software reset

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      pChan:            Vin ID and sensor ID
 *      NumXhsEshrSpeed: Integration time in number of H period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData[2] = {0U};
    TxData[0] = (UINT8)((pShutterCtrl[0] & 0xff00U) >> 8U);
    TxData[1] = (UINT8)(pShutterCtrl[0] & 0x00ffU);
    RetVal |= RegWrite(pChan,MX00_OV2778_CEXP_DCG_H,TxData,2U);
    if (SMode == MX00_OV2778_1920_1080_DCGVS_A30P) {
        TxData[0] = (UINT8)((pShutterCtrl[1] & 0xff00U) >> 8U);
        TxData[1] = (UINT8)(pShutterCtrl[1] & 0x00ffU);
        RetVal |=RegWrite(pChan, 0x30b8U,TxData, 2U);
    }
    return RetVal;
}

#if 0
static UINT32 MX00_OV2778_SetHdrWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl){
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_WB_GAIN_FACTOR_s CurrentWbFactor;
    UINT32 offsetR,offsetGr,offsetGb,offsetB;
    DOUBLE offsetD = 0.0f;
    UINT8 i;
    UINT8 TxData[60] = {0U};
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    RetVal |=MX00_OV2778_RegisterWrite(pChan, MX00_OV2778_GROUP_CTRL, 0x00U); //Select Group 0 to be defined
    UINT32 U32RVal = 0U;

    for (i=0U; i<(pModeInfo->HdrInfo.ActiveChannels); i ++){
        CurrentWbFactor.R  = ((FLOAT)pWbGainCtrl[i].R)/256.0f;
        CurrentWbFactor.Gr = ((FLOAT)pWbGainCtrl[i].Gr)/256.0f;
        CurrentWbFactor.Gb = ((FLOAT)pWbGainCtrl[i].Gb)/256.0f;
        CurrentWbFactor.B  = ((FLOAT)pWbGainCtrl[i].B)/256.0f;
        U32RVal |= AmbaWrap_floor(64.0f * 256.0f * ((DOUBLE)CurrentWbFactor.R - 1.0f),&offsetD);
        offsetR = (UINT32)offsetD;
        U32RVal |= AmbaWrap_floor(64.0f * 256.0f * ((DOUBLE)CurrentWbFactor.Gr - 1.0f),&offsetD);
        offsetGr = (UINT32)offsetD;
        U32RVal |= AmbaWrap_floor(64.0f * 256.0f * ((DOUBLE)CurrentWbFactor.Gb - 1.0f),&offsetD);
        offsetGb = (UINT32)offsetD;
        U32RVal |= AmbaWrap_floor(64.0f * 256.0f * ((DOUBLE)CurrentWbFactor.B - 1.0f),&offsetD);
        offsetB = (UINT32)offsetD;
        if(i==0U){
            TxData[0xb378U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetR & 0xff0000U) >> 16U);
            TxData[0xb379U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetR & 0x00ff00U) >> 8U);
            TxData[0xb37aU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetR & 0x0000ffU);
            TxData[0xb384U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetR & 0xff0000U) >> 16U);
            TxData[0xb385U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetR & 0x00ff00U) >> 8U);
            TxData[0xb386U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetR & 0x0000ffU);
            TxData[0xb37bU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGr & 0xff0000U) >> 16U);
            TxData[0xb37cU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGr & 0x00ff00U) >> 8U);
            TxData[0xb37dU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetGr & 0x0000ffU);
            TxData[0xb387U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGr & 0xff0000U) >> 16U);
            TxData[0xb388U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGr & 0x00ff00U) >> 8U);
            TxData[0xb389U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetGr & 0x0000ffU);
            TxData[0xb37eU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGb & 0xff0000U) >> 16U);
            TxData[0xb37fU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGb & 0x00ff00U) >> 8U);
            TxData[0xb380U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetGb & 0x0000ffU);
            TxData[0xb38aU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGb & 0xff0000U) >> 16U);
            TxData[0xb38bU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGb & 0x00ff00U) >> 8U);
            TxData[0xb38cU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetGb & 0x0000ffU);
            TxData[0xb381U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetB & 0xff0000U) >> 16U);
            TxData[0xb382U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetB & 0x00ff00U) >> 8U);
            TxData[0xb383U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetB & 0x0000ffU);
            TxData[0xb38DU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetB & 0xff0000U) >> 16U);
            TxData[0xb38EU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetB & 0x00ff00U) >> 8U);
            TxData[0xb38FU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetB & 0x0000ffU);
            TxData[0U] = (UINT8)((pWbGainCtrl[i].R  & 0xff00U) >> 8U);
            TxData[0xb361U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].R  & 0x00ffU) ;
            TxData[0xb368U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].R  & 0xff00U) >> 8U);
            TxData[0xb369U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].R  & 0x00ffU) ;
            TxData[0xb362U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].Gr  & 0xff00U) >> 8U);
            TxData[0xb363U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].Gr  & 0x00ffU) ;
            TxData[0xb36aU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].Gr  & 0xff00U) >> 8U);
            TxData[0xb36bU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].Gr  & 0x00ffU) ;
            TxData[0xb364U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].Gb  & 0xff00U) >> 8U);
            TxData[0xb365U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].Gb  & 0x00ffU) ;
            TxData[0xb36cU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].Gb  & 0xff00U) >> 8U);
            TxData[0xb36dU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].Gb  & 0x00ffU) ;
            TxData[0xb366U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].B  & 0xff00U) >> 8U);
            TxData[0xb367U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].B  & 0x00ffU) ;
            TxData[0xb36eU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].B  & 0xff00U) >> 8U);
            TxData[0xb36fU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].B  & 0x00ffU) ;
        }
        if(i == 1U){  //ForVS
            TxData[0xb390U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetR & 0xff0000U) >> 16U);
            TxData[0xb391U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetR & 0x00ff00U) >> 8U);
            TxData[0xb392U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetR & 0x0000ffU);
            TxData[0xb393U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGr & 0xff0000U) >> 16U);
            TxData[0xb394U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGr & 0x00ff00U) >> 8U);
            TxData[0xb395U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetGr & 0x0000ffU);
            TxData[0xb396U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGb & 0xff0000U) >> 16U);
            TxData[0xb397U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetGb & 0x00ff00U) >> 8U);
            TxData[0xb398U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetGb & 0x0000ffU);
            TxData[0xb399U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetB & 0xff0000U) >> 16U);
            TxData[0xb39aU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((offsetB & 0x00ff00U) >> 8U);
            TxData[0xb39bU-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)( offsetB & 0x0000ffU);
            TxData[0xb370U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].R  & 0xff00U) >> 8U);
            TxData[0xb371U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].R  & 0x00ffU) ;
            TxData[0xb372U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].Gr  & 0xff00U) >> 8U);
            TxData[0xb373U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].Gr  & 0x00ffU) ;
            TxData[0xb374U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].Gb  & 0xff00U) >> 8U);
            TxData[0xb375U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].Gb  & 0x00ffU) ;
            TxData[0xb376U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)((pWbGainCtrl[i].B  & 0xff00U) >> 8U);
            TxData[0xb377U-MX00_OV2778_WB_CTRL_START_ADDR] = (UINT8)(pWbGainCtrl[i].B  & 0x00ffU) ;
        }
    }
    RetVal |= RegWrite(pChan,MX00_OV2778_WB_CTRL_START_ADDR,TxData,60U);
    RetVal |=MX00_OV2778_RegisterWrite(pChan, MX00_OV2778_GROUP_CTRL, 0x10U); //Select launch post SOF, Group 0 for single start,
    RetVal |=MX00_OV2778_RegisterWrite(pChan, MX00_OV2778_OPERATION_CTRL, 0x01U); //Do single start

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_GetHdrInfo
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
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    ////const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    ////UINT16 SEFShtCtrl = (UINT16)pShutterCtrl[1];
    UINT32 U32RVal = 0U;

    if ((pChan == NULL) || (pShutterCtrl == NULL) || (pHdrInfo ==NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //pHdrInfo->ChannelInfo[1].EffectiveArea.StartY = (UINT16)pShutterCtrl[1] + 1U;
        //MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo
        U32RVal |= AmbaWrap_memcpy(pHdrInfo, &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
        if(SMode == MX00_OV2778_1920_1080_DCGVS_A30P) {
            pHdrInfo->ChannelInfo[1].EffectiveArea.StartY = (UINT16)pShutterCtrl[1] + 1U;
        }
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      ::
 *      pChan:      Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OV2778_RegisterWrite(pChan, MX00_OV2778_SFW_CTRL1_STANDBY, 0x00);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      ::
 *      pChan:      Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OV2778_RegisterWrite(pChan, MX00_OV2778_SFW_CTRL1_STANDBY, 0x01);
    }

    return RetVal;
}




/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *    pChan:        Vin ID and sensor ID
 *    SensorMode:  Sensor mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 i;
    UINT16 Addr;
    UINT8 Data;
    UINT32 RetVal = SENSOR_ERR_NONE;
    if (SensorMode == MX00_OV2778_1280_720_A30P) {
        for (i = 0U; i < MX00_OV2778_NUM_READOUT_MODE_REG; i++) {
            Addr = MX00_OV2778RegTable[i].Addr;
            Data = MX00_OV2778RegTable[i].Data;
            RetVal |= MX00_OV2778_RegisterWrite(pChan, Addr, Data);
        }
    } else if(SensorMode == MX00_OV2778_1920_1080_A30P) {
        for (i = 0U; i < MX00_OV2778_FHD_NUM_READOUT_MODE_REG; i++) {
            Addr = MX00_OV2778RegTableFHD[i].Addr;
            Data = MX00_OV2778RegTableFHD[i].Data;
            RetVal |= MX00_OV2778_RegisterWrite(pChan, Addr, Data);
        }
    } else if(SensorMode == MX00_OV2778_1920_1080_DCG_A30P) {
        for (i = 0U; i < MX00_OV2778_DCG_NUM_READOUT_MODE_REG; i++) {
            Addr = MX00_OV2778RegTableDCG[i].Addr;
            Data = MX00_OV2778RegTableDCG[i].Data;
            RetVal |= MX00_OV2778_RegisterWrite(pChan, Addr, Data);
        }
    } else if(SensorMode == MX00_OV2778_1920_1080_DCG12b_A30P) {
        for (i = 0U; i < MX00_OV2778_DCG_NUM_READOUT_MODE_REG; i++) {
            Addr = MX00_OV2778RegTableDCG[i].Addr;
            Data = MX00_OV2778RegTableDCG[i].Data;
            RetVal |= MX00_OV2778_RegisterWrite(pChan, Addr, Data);
        }
        RetVal |= MX00_OV2778_RegisterWrite(pChan, 0x3190, 0x02);
        RetVal |= MX00_OV2778_RegisterWrite(pChan, 0x31e7, 0x2c);
        RetVal |= MX00_OV2778_RegisterWrite(pChan, 0x3217, 0x2c);
        RetVal |= MX00_OV2778_RegisterWrite(pChan, 0x3197, 0x0a);
        RetVal |= MX00_OV2778_RegisterWrite(pChan, 0x3195, 0x29);
    } else if(SensorMode == MX00_OV2778_1920_1080_DCGVS_A30P) {
        for (i = 0U; i < MX00_OV2778_DCGVS_NUM_READOUT_MODE_REG; i++) {
            Addr = MX00_OV2778DCG_VSRegTable[i].Addr;
            Data = MX00_OV2778DCG_VSRegTable[i].Data;
            RetVal |= MX00_OV2778_RegisterWrite(pChan, Addr, Data);
        }
    } else {
        RetVal = SENSOR_ERR_ARG;
    }


    for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
        //if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
        /* reset gain/shutter ctrl information */
        MX00_OV2778Ctrl[pChan->VinID].CurrentShrCtrl[i][0] = 0xffffffffU;
        MX00_OV2778Ctrl[pChan->VinID].CurrentShrCtrl[i][1] = 0xffffffffU;
        MX00_OV2778Ctrl[pChan->VinID].CurrentAGCtrl[i][0] = 0xffffffffU;
        MX00_OV2778Ctrl[pChan->VinID].CurrentAGCtrl[i][1] = 0xffffffffU;
        MX00_OV2778Ctrl[pChan->VinID].CurrentAGCtrl[i][2] = 0xffffffffU;
        MX00_OV2778Ctrl[pChan->VinID].CurrentDGCtrl[i][0] = 0U;
        MX00_OV2778Ctrl[pChan->VinID].CurrentDGCtrl[i][1] = 0U;
        MX00_OV2778Ctrl[pChan->VinID].CurrentDGCtrl[i][2] = 0U;
        MX00_OV2778Ctrl[pChan->VinID].CurrentGainType[i][0] = 0U;
        MX00_OV2778Ctrl[pChan->VinID].CurrentGainType[i][1] = 0U;
        MX00_OV2778Ctrl[pChan->VinID].CurrentGainType[i][2] = 0U;
        //}
    }
    return RetVal;
}

static UINT32 MX00_OV2778_SensorModeInit(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    RetVal |= MX00_OV2778_ChangeReadoutMode(pChan, SensorMode);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_OV2778_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;
    //const MX00_OV2778_SENSOR_INFO_s *pSensorInfo = NULL;
    //FLOAT LineLengthPck;

    for (i = 0U; i < MX00_OV2778_NUM_MODE; i++) {
        pOutputInfo = &MX00_OV2778_OutputInfo[i];
        //pSensorInfo = &MX00_OV2778_SensorInfo[i];
        //Need to check the RowTime calculation method
        /* MX00_OV2778_ModeInfoList[i].RowTime = (FLOAT) ((DOUBLE) pSensorInfo->LineLengthPck /
                                                                ( (DOUBLE) pSensorInfo->DataRate *
                                                                  (DOUBLE) pSensorInfo->NumDataLanes /
                                                                  (DOUBLE) pSensorInfo->NumDataBits) );*/
        MX00_OV2778_ModeInfoList[i].PixelRate = (FLOAT) ((DOUBLE) pOutputInfo->DataRate *
                                                (DOUBLE) pOutputInfo->NumDataLanes /
                                                (DOUBLE) pOutputInfo->NumDataBits);
        //LineLengthPck = ((((FLOAT)MX00_OV2778_ModeInfoList[i].PixelRate / (FLOAT)pSensorInfo->FrameLengthLines) * (FLOAT)pSensorInfo->LineLengthPck) * (FLOAT)pSensorInfo->FrameLengthLines) / 119799200.0f;
        //MX00_OV2778_ModeInfoList[i].RowTime = (FLOAT)  LineLengthPck / MX00_OV2778_ModeInfoList[i].PixelRate;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

//    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "OV2778 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, MX00_OV2778_SensorInfo[0U].InputClk);
        MX00_OV2778_PreCalculateModeInfo();

    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      pChan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX00_OV2778_HardwareReset(pChan);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      pChan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OV2778_StandbyOn(pChan);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_GetStatus
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
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0U;

    if ((pChan == NULL) || (pStatus == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pStatus, &MX00_OV2778Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_GetModeInfo
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
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if (Config.ModeID >= MX00_OV2778_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            MX00_OV2778_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }
    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_GetDeviceInfo
 *
 *  @Description:: Get Sensor Device Info
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to device info
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0U;

    if ((pChan == NULL) ||(pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &MX00_OV2778_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_GetCurrentGainFactor
 *
 *  @Description:: Get Sensor Current Gain Factor
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pGainFactor:    pointer to current gain factor
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    //const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;
    UINT32 CurrentGainCtrl, CurrentCGCtrl, Tmp;
    UINT32 RetVal = SENSOR_ERR_NONE;
    DOUBLE DoubleTmp;

    //(void)pModeInfo;
    if ((pChan == NULL) ||(pGainFactor == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
            //Need to have a if to check
            CurrentGainCtrl = MX00_OV2778Ctrl[pChan->VinID].CurrentAGCtrl[i][0];
            CurrentCGCtrl = MX00_OV2778Ctrl[pChan->VinID].CurrentDGCtrl[i][0];
            Tmp = CurrentGainCtrl * CurrentCGCtrl;
            DoubleTmp = ((DOUBLE)Tmp/64.0)/1024.0;
            *pGainFactor = (FLOAT)DoubleTmp;

            //(FLOAT) ((FLOAT)((CurrentGainCtrl * CurrentCGCtrl)/64.0)/1024.0);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pExposureTime:    pointer to current exposure time
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 i,k;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    //Need to have a if to check
                    pExposureTime[0] = pModeInfo->RowTime * (FLOAT)MX00_OV2778Ctrl[pChan->VinID].CurrentShrCtrl[i][0];
                }
            }
        } else {
            for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    //Need to have a if to check
                    for (k = 0U; k < (pModeInfo->HdrInfo.ActiveChannels); k ++) {
                        pExposureTime[k] =(FLOAT)MX00_OV2778Ctrl[pChan->VinID].CurrentShrCtrl[i][k]* pModeInfo->RowTime;

                    }
                }
            }
        }
    }
    return RetVal;
}
static void OV2778_ConvertGainToCtrl(FLOAT DesiredGain, UINT8 GainType, UINT32 *GainCtrl, FLOAT *ActualFactor)
{
    FLOAT TempAgc = 1.0f;
    FLOAT Agc = 1.0f;
    UINT16 Dgc = 1U;
    UINT32 Gain = 0U;
    DOUBLE DGainInDb = 0.0f;
    UINT32 U32RVal = 0U;

    if( GainType == 0U) {
        TempAgc = (DesiredGain / MX00_OV2778_GAIN_RATIO) / MX00_OV2778_MIN_DGAIN_HCG;
        if (TempAgc >= 8.0f) {
            Agc = 8.0f;
            Gain = 3U;
        } else if (TempAgc >= 4.0f) {
            Agc = 4.0f;
            Gain = 2U;
        } else if (TempAgc >= 2.0f) {
            Agc = 2.0f;
            Gain = 1U;
        } else {
            Agc = 1.0f;
            Gain = 0U;
        }
        U32RVal |= AmbaWrap_floor((((DOUBLE)DesiredGain /(DOUBLE)MX00_OV2778_GAIN_RATIO)/ (DOUBLE)Agc )*256.0f, &DGainInDb);
        Dgc =  (UINT16)DGainInDb;
        Dgc = (Dgc > 0x3fffU) ? 0x3fffU : Dgc;
        Dgc = (Dgc < 0x0140U) ? 0x0140U : Dgc;
        *GainCtrl =  (Gain << 16U) + Dgc;
        *ActualFactor =  Agc * (FLOAT)Dgc * MX00_OV2778_GAIN_RATIO / 256.0f;

    } else if(GainType ==1U) {
        TempAgc = DesiredGain / MX00_OV2778_MIN_DGAIN_LCG;
        if (TempAgc >= 8.0f) {
            Agc = 8.0f;
            Gain = 3U;
        } else if (TempAgc >= 4.0f) {
            Agc = 4.0f;
            Gain = 2U;
        } else {
            Agc = 2.0f;
            Gain = 1U;
        }
        U32RVal |= AmbaWrap_floor(((DOUBLE)DesiredGain / (DOUBLE)Agc )*256.0f, &DGainInDb);
        Dgc =  (UINT16)DGainInDb;
        Dgc = (Dgc > 0x3fffU) ? 0x3fffU : Dgc;
        Dgc = (Dgc < 0x0118U) ? 0x0118U : Dgc;
        *GainCtrl = (Gain << 16U) + Dgc;
        *ActualFactor =  Agc * (FLOAT) Dgc / 256.0f;
    } else {
        AmbaPrint_PrintUInt5("Not Support this GainType Convrt to GainCtrl", 0U, 0U, 0U, 0U, 0U);
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

}

static UINT32 MX00_OV2778_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32  HdrType = pModeInfo->HdrInfo.HdrType;
    FLOAT agc_tmp;
    AMBA_SENSOR_GAIN_FACTOR_s DesiredFactor = *pDesiredFactor;
    UINT32 CurrentAgcCtrl;
    UINT32 type = 1; //0: HCG, 1:LCG
    UINT32 Dgc = 1;
    UINT32 Agc = 1;
    UINT8 i = 0;
    UINT32 GainCtrl = 0U;
    FLOAT ActualFactor = 1.0f;
    DOUBLE Dgc_tmp = 0.0f;
    CurrentAgcCtrl = MX00_OV2778Ctrl[pChan->VinID].CurrentAGCtrl[pChan->VinID][0]; //////need to refine it
    if (HdrType == AMBA_SENSOR_HDR_NONE) {
        if(DesiredFactor.Gain[0] <= MX00_OV2778_MIN_AGAIN_DGAIN_LCG) {
            DesiredFactor.Gain[0] = MX00_OV2778_MIN_AGAIN_DGAIN_LCG;
        }
        if((DesiredFactor.Gain[0] >= MX00_OV2778_HCG_SWITCH)) { //HCG
            DesiredFactor.Gain[0] = DesiredFactor.Gain[0] / MX00_OV2778_GAIN_RATIO;
            type = 0;
            agc_tmp = DesiredFactor.Gain[0]/MX00_OV2778_MIN_DGAIN_HCG;
            if ( agc_tmp>= 8.0) {
                Agc = 8;
            } else if (agc_tmp >= 4.0) {
                Agc = 4;
            } else if (agc_tmp >= 2.0) {
                Agc = 2;
            } else {
                Agc = 1;
            }
        } else if(DesiredFactor.Gain[0]  <= MX00_OV2778_LCG_SWITCH) { //LCG
            type = 1;
            agc_tmp = DesiredFactor.Gain[0] / MX00_OV2778_MIN_DGAIN_LCG;
            if (agc_tmp >= 8.0) {
                Agc = 8;
            } else if (agc_tmp >= 4.0) {
                Agc = 4;
            } else {
                Agc = 2;
            }
        } else { //LCG_SWITCH~HCG_SWITCH
            if((CurrentAgcCtrl & 0x40U) == 0x40U) { //HCG
                DesiredFactor.Gain[0] = DesiredFactor.Gain[0] / MX00_OV2778_GAIN_RATIO;
                type = 0;
                agc_tmp = DesiredFactor.Gain[0]/MX00_OV2778_MIN_DGAIN_HCG;
                if (agc_tmp>= 8.0) {
                    Agc = 8;
                } else if (agc_tmp >= 4.0) {
                    Agc = 4;
                } else if (agc_tmp >= 2.0) {
                    Agc = 2;
                } else {
                    Agc = 1;
                }
            } else { //LCG
                type = 1;
                agc_tmp = DesiredFactor.Gain[0] / MX00_OV2778_MIN_DGAIN_LCG;
                if (agc_tmp >= 8.0) {
                    Agc = 8;
                } else if (agc_tmp >= 4.0) {
                    Agc = 4;
                } else {
                    Agc = 2;
                }
            }
        }

        Dgc_tmp = ((DOUBLE)DesiredFactor.Gain[0] * 256.0f) / (DOUBLE)Agc;
        Dgc = (UINT32)Dgc_tmp;
        /* For Dgc restriction */
        Dgc = (Dgc > 0x3fffU) ? 0x3fffU : Dgc;
        if(type == 0U) { //HCG
            Dgc = (Dgc < 0x0140U) ? 0x0140U : Dgc;
            pGainCtrl->AnalogGain[0] = ((Agc+0x40U) << 16U) + Dgc;
            pActualFactor->Gain[0] =   (((FLOAT)Agc * (FLOAT)Dgc) / 256.0f) * MX00_OV2778_GAIN_RATIO;
        } else { // this branch is (type == 1U) { //LCG
            Dgc = (Dgc < 0x0118U) ? 0x0118U : Dgc;
            pGainCtrl->AnalogGain[0] = (Agc << 16U) + Dgc;
            pActualFactor->Gain[0] =(FLOAT) Agc * (FLOAT)Dgc  / 256.0f;
        }
        // MisraC. type==0 or type==1
        //else {
        //    AmbaPrint_PrintUInt5("--- MX00_OV2778_ConvertGainFactor, Not support this gain type",0U,0U, 0U, 0U, 0U);
        //}
        //AmbaPrint_PrintUInt5("--- MX00_OV2778_ConvertGainFactor, Desire %d : Actual %d ---",(UINT32)(Desire*10000) ,(UINT32)(pActualFactor->Gain[0] *10000), 0U, 0U, 0U);
    } else if(HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
        if(DesiredFactor.Gain[0] < MX00_OV2778_MIN_AGAIN_DGAIN_LCG) {
            DesiredFactor.Gain[0] = MX00_OV2778_MIN_AGAIN_DGAIN_LCG;
        }
        OV2778_ConvertGainToCtrl(DesiredFactor.Gain[0],1U,&GainCtrl,&ActualFactor);
        pGainCtrl->AnalogGain[0] =  GainCtrl;
        pActualFactor->Gain[0] =  ActualFactor;
        pActualFactor->Gain[1] =  ActualFactor;
    } else if(HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) { //DCG + VS
        for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
            if(DesiredFactor.Gain[i] < MX00_OV2778_MIN_AGAIN_DGAIN_LCG) {
                DesiredFactor.Gain[i] = MX00_OV2778_MIN_AGAIN_DGAIN_LCG;
            }
            OV2778_ConvertGainToCtrl(DesiredFactor.Gain[i],1U,&GainCtrl,&ActualFactor);
            pGainCtrl->AnalogGain[i] =  GainCtrl;
            pActualFactor->Gain[i] =  ActualFactor;
        }
    } else {
        //
    }
    //AmbaPrint_PrintUInt5("--- MX00_OV2778_ConvertGainFactor, HdrType : %d ---", HdrType, 0U, 0U, 0U, 0U);
    return RetVal;
}
static void OV2778_ConvertCtrlToReg(const UINT32 GainCtrl, UINT8 *Agc, UINT8 *DgcMsb, UINT8 *DgcLsb)
{
    UINT8 CheckAgain = (UINT8)((GainCtrl & 0xffff0000U) >> 16U);
    *Agc = CheckAgain;
    *DgcMsb =(UINT8) ((GainCtrl & 0x0000ff00U) >> 8U);
    *DgcLsb =(UINT8) (GainCtrl & 0x000000ffU);
}

static UINT32 MX00_OV2778_SetAGainReg( const AMBA_SENSOR_CHANNEL_s *pChan,const UINT32 *pAnalogGainCtrl)
{
    //UINT32 DgcMsb = 0U;
    //UINT32 DgcLsb = 0U;
    UINT8 AgcR30BB = 0;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData[2] = {0U};
    UINT8 AgcLcg= 0U;
    UINT8 DgcMsb = 0U;
    UINT8 DgcLsb = 0U;
    UINT16 Tmp;
    UINT16 RxData = 0U;
    UINT8 Agc = 0U;
    UINT16 Dgain = 0U;
    //UINT8 AgcHcg=0; //HCG
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32  HdrType = pModeInfo->HdrInfo.HdrType;
    if (HdrType == AMBA_SENSOR_HDR_NONE) {
        switch((pAnalogGainCtrl[0] & 0x00ff0000U) >> 16U) {
        case 0x41:
            AgcR30BB = 0x40;
            break;
        case 0x2:
            AgcR30BB = 0x01;
            break;
        case 0x42:
            AgcR30BB = 0x41;
            break;
        case 0x4:
            AgcR30BB = 0x02;
            break;
        case 0x44:
            AgcR30BB = 0x42;
            break;
        case 0x8:
            AgcR30BB = 0x03;
            break;
        case 0x48:
            AgcR30BB = 0x43;
            break;
        default:
            AgcR30BB = 0xff;
            break;
        }
        //DgcMsb = (pAnalogGainCtrl[0]  & 0x0000ff00U) >> 8U;
        //DgcLsb = (pAnalogGainCtrl[0]  & 0x000000ffU);
        TxData[0] = (UINT8)((pAnalogGainCtrl[0]  & 0x0000ff00U) >> 8U);
        TxData[1] = (UINT8)(pAnalogGainCtrl[0]  & 0x000000ffU);
        RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_GROUP_CTRL, 0x00U); //Select Group 0 to be defined
        RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_CG_AGAIN_GROUP_HOLD, AgcR30BB);
        RetVal |= RegWrite(pChan,MX00_OV2778_DIG_GAIN_HCG_H_GROUP_HOLD,TxData,2U);
        //RetVal |= MX00_OV2778_RegisterWrite(pChan,0xb15aU, (UINT16)DgcMsb);
        //RetVal |= MX00_OV2778_RegisterWrite(pChan,0xb15bU, (UINT16)DgcLsb);
        RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_GROUP_CTRL, 0x40); //Select launch post SOF, Group 0 for single start,
        RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_OPERATION_CTRL, 0x01); //Do single start
    } else {
        if((SMode == MX00_OV2778_1920_1080_DCG_A30P) || (SMode == MX00_OV2778_1920_1080_DCG12b_A30P)) {
            RetVal |=MX00_OV2778_RegisterRead(pChan,0x30bbU,&RxData);

            RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_GROUP_CTRL, 0x00U); //Select Group 0 to be defined
            OV2778_ConvertCtrlToReg(pAnalogGainCtrl[0],&AgcLcg,&DgcMsb,&DgcLsb);;  //For LCG
            //Tmp2 = (UINT32)1U << AgcLcg;
            //LcgAgain = (UINT8)Tmp2;
            Dgain = (UINT16)(pAnalogGainCtrl[0] & 0x0000ffffU);
            Dgain = (Dgain < 0x0140U) ? 0x0140U : Dgain;
            Tmp = (UINT16)AgcLcg << 2;

            AgcLcg = (UINT8)Tmp;
            Agc = (UINT8)(RxData & 0x33U)+(AgcLcg & 0x0cU);
            RetVal |= RegWrite(pChan,0xb15cU, &DgcMsb,1U);
            RetVal |= RegWrite(pChan,0xb15dU, &DgcLsb,1U);

            Agc = (UINT8)((Agc & 0x003cU)) + (AgcLcg & 0x03U );
            DgcMsb =(UINT8) ((Dgain & 0x0000ff00U) >> 8U);
            DgcLsb =(UINT8) (Dgain & 0x000000ffU);
            RetVal |= RegWrite(pChan,0xb15aU, &DgcMsb,1U);
            RetVal |= RegWrite(pChan,0xb15bU, &DgcLsb, 1U);
            RetVal |= RegWrite(pChan,MX00_OV2778_CG_AGAIN_GROUP_HOLD, &Agc, 1U);
            RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_GROUP_CTRL, 0x40); //Select launch post SOF, Group 0 for single start,
            RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_OPERATION_CTRL, 0x01); //Do single start
        } else if(SMode == MX00_OV2778_1920_1080_DCGVS_A30P) {
            RetVal |=MX00_OV2778_RegisterRead(pChan,0x30bbU,&RxData);

            RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_GROUP_CTRL, 0x00U); //Select Group 0 to be defined
            OV2778_ConvertCtrlToReg(pAnalogGainCtrl[0],&AgcLcg,&DgcMsb,&DgcLsb);;  //For LCG
            //Tmp2 = (UINT32)1U << AgcLcg;
            //LcgAgain = (UINT8)Tmp2;
            Dgain = (UINT16)(pAnalogGainCtrl[0] & 0x0000ffffU);
            Dgain = (Dgain < 0x0140U) ? 0x0140U : Dgain;
            Tmp = (UINT16)AgcLcg << 2;

            AgcLcg = (UINT8)Tmp;
            Agc = (UINT8)(RxData & 0x33U)+(AgcLcg & 0x0cU);
            RetVal |= RegWrite(pChan,0xb15cU, &DgcMsb,1U);
            RetVal |= RegWrite(pChan,0xb15dU, &DgcLsb,1U);

            Agc = (UINT8)((Agc & 0x003cU)) + (AgcLcg & 0x03U );
            DgcMsb =(UINT8) ((Dgain & 0x0000ff00U) >> 8U);
            DgcLsb =(UINT8) (Dgain & 0x000000ffU);
            RetVal |= RegWrite(pChan,0xb15aU, &DgcMsb,1U);
            RetVal |= RegWrite(pChan,0xb15bU, &DgcLsb, 1U);


            OV2778_ConvertCtrlToReg(pAnalogGainCtrl[1],&AgcLcg,&DgcMsb,&DgcLsb);  //For VS
            Tmp = (UINT16)AgcLcg << 4;
            AgcLcg = (UINT8)Tmp;
            Agc = (Agc & 0x0fU) + (AgcLcg & 0x30U );
            RetVal |= RegWrite(pChan,0xb15eU, &DgcMsb, 1U);
            RetVal |= RegWrite(pChan,0xb15fU, &DgcLsb, 1U);
            RetVal |= RegWrite(pChan,MX00_OV2778_CG_AGAIN_GROUP_HOLD, &Agc, 1U);
            RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_GROUP_CTRL, 0x40); //Select launch post SOF, Group 0 for single start,
            RetVal |= MX00_OV2778_RegisterWrite(pChan,MX00_OV2778_OPERATION_CTRL, 0x01); //Do single start
        } else {
            ////Misra-C
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
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    //UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 ExposureLineMaxMargin = 10U;
    UINT32 MaxExposureLine, MinExposureLine;
    DOUBLE ShutterCtrlInDb;
    UINT32 ShutterCtrl;
    UINT32 U32RVal = 0U;

    U32RVal |= AmbaWrap_floor((DOUBLE)ExposureTime / (DOUBLE)pModeInfo->RowTime, &ShutterCtrlInDb);/***/
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
        /* MisraC */  /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl = ShutterCtrl;
    //AmbaPrint_PrintUInt5("--- @@@@@ConvertShutterSpeed, HdrChannel : %d pShutterCtrl:%d ExposureTime:%d", HdrChannel, *pShutterCtrl, ExposureTime*10000, 0U, 0U);
    //*pActualExptime = (FLOAT)ShutterCtrl * (pModeInfo->RowTime * ((FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame));
    *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)pModeInfo->RowTime ;

    if(U32RVal != 0U){
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_ConvertShutterSpeed
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
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* None HDR mode */
        pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
            //AmbaPrint_PrintUInt5("---AMBA_SENSOR_HDR_NONE, HdrType : %d ---", pModeInfo->HdrInfo.HdrType, 0U, 0U, 0U, 0U);
        } else {
            /*HDR mode */
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i++) {
                if(((SMode == MX00_OV2778_1920_1080_DCG_A30P) || (SMode == MX00_OV2778_1920_1080_DCG12b_A30P)) && (i==1U)) {  //LCG and HCG has the same shutter time under DCG mode
                    pActualExposureTime[1] = pActualExposureTime[0];
                    pShutterCtrl[1] = pShutterCtrl[0];
                    continue;
                }
                ConvertShutterSpeed(pModeInfo, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
        }
    }
    //AmbaPrint_PrintUInt5("--- MX00_OV2778_ConvertShutterSpeed, HdrType : %d ---", pModeInfo->HdrInfo.HdrType, 0U, 0U, 0U, 0U);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_SetAnalogGainCtrl
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
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 i,j;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    (void)pModeInfo;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* None-HDR mode */
        pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal |= MX00_OV2778_SetAGainReg(pChan, pAnalogGainCtrl);
            /* Update current analog gain control */
            for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    MX00_OV2778Ctrl[pChan->VinID].CurrentAGCtrl[i][0] = (pAnalogGainCtrl[0] & 0xffff0000U)>>16;
                    MX00_OV2778Ctrl[pChan->VinID].CurrentDGCtrl[i][0] = pAnalogGainCtrl[0] & 0x0000ffffU;
                }
            }
        } else { //HDR mode
            RetVal |= MX00_OV2778_SetAGainReg(pChan,pAnalogGainCtrl);
            for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (j=0U; j<(pModeInfo->HdrInfo.ActiveChannels); j ++) {
                        MX00_OV2778Ctrl[pChan->VinID].CurrentAGCtrl[i][j] = pAnalogGainCtrl[j];
                    }
                }
            }/* HDR mode */
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX224_SetShutterCtrl
 *
 *  @Description:: Set shutter control
 *
 *  @Input      ::
 *      pChan:           Vin ID and sensor ID
 *      ShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 i,j;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    /////UINT16 SFData, Data ;
    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* None-HDR mode */
        pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_OV2778_SetShutterReg(pChan, pShutterCtrl);
            for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    MX00_OV2778Ctrl[pChan->VinID].CurrentShrCtrl[i][0] = pShutterCtrl[0];
                }
            }
        } else {
            /* HDR mode */
            //Need to check this API, 20190613
            RetVal = MX00_OV2778_SetShutterReg(pChan, pShutterCtrl);
            for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (j = 0U; j < (pModeInfo->HdrInfo.ActiveChannels); j ++) {
                        MX00_OV2778Ctrl[pChan->VinID].CurrentShrCtrl[i][j] = pShutterCtrl[j];
                    }
                }
            }

        }
    }
    //AmbaPrint_PrintUInt5("--- MX00_OV2778_SetShutterCtrl, HdrType : %d ---", pModeInfo->HdrInfo.HdrType, 0U, 0U, 0U, 0U);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      pChan:               Vin ID and sensor ID
 *      DigitalGainCtrl:     Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    if(pChan ==NULL) {
        //MisraC
    }
    if(pDigitalGainCtrl==NULL) {
        //MisraC
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_SetWbGainCtrl
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
static UINT32 MX00_OV2778_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;//CurrentWbCtrl
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i,j;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            //RetVal = MX00_OV2778_SetHdrWbGainReg(pChan, pWbGainCtrl);
            /* Update current wb gain control */
            for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
                //if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {                    MX00_OV2778Ctrl[pChan->VinID].CurrentWbCtrl[i][0] = pWbGainCtrl[0];
                //}
            }

        } else {
            /* Update current wb gain control */
            //RetVal = MX00_OV2778_SetHdrWbGainReg(pChan, pWbGainCtrl);
            for (i = 0U; i < MX00_OV2778_NUM_MAX_SENSOR_COUNT; i++) {
                /*if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {*/
                for (j=0U; j<(pModeInfo->HdrInfo.ActiveChannels); j ++) {
                    /* Update current shutter control */
                    MX00_OV2778Ctrl[pChan->VinID].CurrentWbCtrl[i][j] = pWbGainCtrl[0];
                }
            }
        }
        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "WWWbgain-set: R=%d Gr=%d Gb=%d B=%d", pWbGainCtrl[0].R*1000, pWbGainCtrl[0].Gr*1000, pWbGainCtrl[0].Gb*1000, pWbGainCtrl[0].B*1000, 0);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_SetSlowShutterCtrl
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
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    (void)pChan;
    (void)SlowShutterCtrl;

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2778_Config
 *
 *  @Description:: Set sensor to indicated mode
 *
 *  @Input      ::
 *      pChan:             Vin ID and sensor ID
 *      Mode:             Sensor Readout Mode Number
 *      ElecShutterMode:  Operating Mode of the Electronic Shutter
 *
 *  @Output     ::
 *      pModeInfo:  pointer to mode info of target mode
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2778_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s MX00_OV2778PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2778Ctrl[pChan->VinID].Status.ModeInfo;
    UINT16 SensorMode = (UINT16)pMode->ModeID;
    //MX00_OV2778_FRAME_TIMING_s *pFrameTime = &MX00_OV2778Ctrl[pChan->VinID].FrameTime;
    UINT32 RetVal = SENSOR_ERR_NONE;
#ifdef MX00_OV2778_SERDES_ENABLE
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Add delay time to wait MAX9296/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power off", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power on", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(3);
#endif
    if (SensorMode >= MX00_OV2778_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        SMode = SensorMode;
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "============  [ MX00_OV2778 Config ]  ============", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d ============", SensorMode, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

        /* update status */
        MX00_OV2778_PrepareModeInfo(pChan, pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        MX00_OV2778PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX00_OV2778PadConfig);
#ifdef MX00_OV2778_SERDES_ENABLE
        /* After reset VIN, set MAX9296A PWDNB high */
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB low", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(3);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB high", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_9296_Init(pChan->VinID, MX00_OV2778_GetEnabledLinkID(pChan->SensorID));

        MX00_OV2778_ConfigSerDes(pChan, (UINT16)SensorMode, pModeInfo);
        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX00_OV2778_SENSOR_ID_CHAN_A) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
        }
        if ((pChan->SensorID & MX00_OV2778_SENSOR_ID_CHAN_B) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
        }

#else
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
#endif
        RetVal |= MX00_OV2778_HardwareReset(pChan); // sensor hardware reset

        RetVal |= MX00_OV2778_SoftwareReset(pChan);

        RetVal |= MX00_OV2778_SensorModeInit(pChan, SensorMode);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  SensorMode: %d  ============", SensorMode, 0U, 0U, 0U, 0U);
        RetVal |= MX00_OV2778_StandbyOff(pChan); /* Stream on */
        RetVal |= MX00_OV2778_ConfigVin(pChan, pModeInfo);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX00_OV2778Obj = {
    .SensorName             = "OV2778",
    .SerdesName             = "MAX9295_9296",
    .Init                   = MX00_OV2778_Init,
    .Enable                 = MX00_OV2778_Enable,
    .Disable                = MX00_OV2778_Disable,
    .Config                 = MX00_OV2778_Config,

    .GetStatus              = MX00_OV2778_GetStatus,
    .GetModeInfo            = MX00_OV2778_GetModeInfo,
    .GetDeviceInfo          = MX00_OV2778_GetDeviceInfo,
    .GetHdrInfo             = MX00_OV2778_GetHdrInfo,
    .GetCurrentGainFactor   = MX00_OV2778_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX00_OV2778_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX00_OV2778_ConvertGainFactor,
    .ConvertShutterSpeed    = MX00_OV2778_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX00_OV2778_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MX00_OV2778_SetDigitalGainCtrl, //Skipped it first
    .SetWbGainCtrl          = MX00_OV2778_SetWbGainCtrl,
    .SetShutterCtrl         = MX00_OV2778_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX00_OV2778_SetSlowShutterCtrl, //Skipped it first

    .RegisterRead           = MX00_OV2778_RegisterRead,
    .RegisterWrite          = MX00_OV2778_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
