/**
 *  @file AmbaSensor_TI913_954_OV10635.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Control APIs of TI SerDes + OV10635 CMOS sensors with Parallel interface
 *
 */

#include "AmbaVIN.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_TI913_954_OV10635.h"
#include "AmbaSbrg_TI913_954.h"
#include "AmbaMisraFix.h"

#include "bsp.h"

#define OV10635_MANUAL_AEC

/*-----------------------------------------------------------------------------------------------*\
 * TI03_OV10635 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static TI03_OV10635_CTRL_s TI03_OV10635Ctrl = {0};

static UINT32 I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_I2C_CHANNEL_PIP2,
    [AMBA_VIN_CHANNEL3] = AMBA_SENSOR_I2C_CHANNEL_PIP3,
    [AMBA_VIN_CHANNEL4] = AMBA_SENSOR_I2C_CHANNEL_PIP4,
    [AMBA_VIN_CHANNEL5] = AMBA_SENSOR_I2C_CHANNEL_PIP5,
};

static TI913_954_SERDES_CONFIG_s SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = {
        .EnabledRxPortID = TI913_954_RX_PORT_0,
        .SensorSlaveID = TI03_OV10635_I2C_ADDR,
        .SensorBroadcastAliasID = TI03_OV10635_BROADCAST_I2C_ADDR,
        .SensorAliasID = {
            [0] = (UINT8)(TI03_OV10635_I2C_ADDR + 2U),
            [1] = 0U,
        },
        .TI913Alias = {
            [0] = TI03_IDC_ADDR_TI913_A_LINK0,
            [1] = 0U,
        },
        .TI954Alias = TI03_IDC_ADDR_TI954_A,
        .Fpd3Mode = TI913_954_RAW10_100MHz,
        .FvPolarity = 1,
        .LvPolarity = 0,
        .CSIOutputInfo = {
            .NumDataLanes = TI913_954_CSI_LANE_NUM_4,
            .DataRate = TI913_954_CSI_TX_SPEED_800M,
            .ForwardingMode = TI913_954_LINE_INTERLEAVE_FWD,
        },
        .FrameSyncSel = {
            .FrameSyncEnable = TI03_OV10635_FSYNC_MODE,
            .TI913Gpio = 0,
            .TI954Gpio = 2,
            .TimeScale = 30000,
            .NumUnitsInTick = 1001,
        },
    },
    [AMBA_VIN_CHANNEL1] = {
        .EnabledRxPortID = TI913_954_RX_PORT_0,
        .SensorSlaveID = TI03_OV10635_I2C_ADDR,
        .SensorBroadcastAliasID = TI03_OV10635_BROADCAST_I2C_ADDR,
        .SensorAliasID = {
            [0] = (UINT8)(TI03_OV10635_I2C_ADDR + 4U),
            [1] = 0U,
        },
        .TI913Alias = {
            [0] = TI03_IDC_ADDR_TI913_B_LINK0,
            [1] = 0U,
        },
        .TI954Alias = TI03_IDC_ADDR_TI954_A,
        .Fpd3Mode = TI913_954_RAW10_100MHz,
        .FvPolarity = 1,
        .LvPolarity = 0,
        .CSIOutputInfo = {
            .NumDataLanes = TI913_954_CSI_LANE_NUM_4,
            .DataRate = TI913_954_CSI_TX_SPEED_800M,
            .ForwardingMode = TI913_954_LINE_INTERLEAVE_FWD,
        },
        .FrameSyncSel = {
            .FrameSyncEnable = TI03_OV10635_FSYNC_MODE,
            .TI913Gpio = 0,
            .TI954Gpio = 2,
            .TimeScale = 30000,
            .NumUnitsInTick = 1001,
        },
    },
    [AMBA_VIN_CHANNEL2] = {0},
    [AMBA_VIN_CHANNEL3] = {0},
    [AMBA_VIN_CHANNEL4] = {0},
    [AMBA_VIN_CHANNEL5] = {0},
};

static UINT8 TI03_OV10635_GetEnabledLinkID(UINT32 SensorID)
{
    UINT8 EnabledLinkID = 0U, i;

    for (i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = (UINT8)(EnabledLinkID | ((UINT32)1U << i));
        }
    }

    return EnabledLinkID;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_GetNumActiveSensor
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
static UINT32 TI03_OV10635_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i ++) {
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
 *  @RoutineName:: TI03_OV10635_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      SensorID:   Sensor ID
 *      Mode:       Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void TI03_OV10635_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID = pMode->ModeID;
    const TI03_OV10635_SENSOR_INFO_s *pSensorInfo = &TI03_OV10635SensorInfo[ModeID];
    const AMBA_VIN_FRAME_RATE_s *pFrameRate          = &TI03_OV10635ModeInfoList[ModeID].FrameRate;
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo      = &TI03_OV10635InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo     = &TI03_OV10635OutputInfo[ModeID];
    UINT32 SensorNum = TI03_OV10635_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    pModeInfo->FrameLengthLines        = pSensorInfo->FrameLengthLines * SensorNum;
    pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    pModeInfo->InputClk                = pSensorInfo->InputClk;
    pModeInfo->RowTime                 = TI03_OV10635ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
    U32RVal |= AmbaWrap_floor(((DOUBLE)TI03_OV10635ModeInfoList[ModeID].PixelRate * ((DOUBLE)pModeInfo->RowTime * (DOUBLE)SensorNum)) + 0.5, &FloorVal);
    pModeInfo->LineLengthPck           = (UINT32)FloorVal;

    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, pFrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, pFrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &TI03_OV10635HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* Only support 1 stage slow shutter with none HDR modes */
    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        pModeInfo->MinFrameRate.TimeScale /= 2U;
    }

    /* update for TI954 CSI-2 output */
    pModeInfo->OutputInfo.DataRate *= SensorNum;
    pModeInfo->OutputInfo.OutputHeight *= (UINT16)SensorNum;
    pModeInfo->OutputInfo.RecordingPixels.StartY *= (UINT16)SensorNum;
    pModeInfo->OutputInfo.RecordingPixels.Height *= (UINT16)SensorNum;

    if(U32RVal != 0U) {
        /* MisraC */
    }

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_ConfigVinMasterSync
 *
 *  @Description:: Configure VIN master sync generation
 *
 *  @Input      ::
 *      Chan:       Vin ID and Sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_ConfigVinMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal;
    DOUBLE PeriodInDb;
    static const UINT32 TI03_OV10635MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
        [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
        [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_MSYNC_CHANNEL_PIP2,
        [AMBA_VIN_CHANNEL3] = AMBA_SENSOR_MSYNC_CHANNEL_PIP3,
        [AMBA_VIN_CHANNEL4] = AMBA_SENSOR_MSYNC_CHANNEL_PIP4,
        [AMBA_VIN_CHANNEL5] = AMBA_SENSOR_MSYNC_CHANNEL_PIP5,
    };

    const TI03_OV10635_MODE_INFO_s *pModeInfo      = &TI03_OV10635ModeInfoList[ModeID];
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg = {0};
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_floor(((((DOUBLE)pModeInfo->FrameRate.NumUnitsInTick * ((DOUBLE)TI03_OV10635SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameRate.TimeScale))) + 0.5), &PeriodInDb);
    MasterSyncCfg.RefClk             = TI03_OV10635SensorInfo[ModeID].InputClk;
    MasterSyncCfg.HSync.Period       = (UINT32) PeriodInDb;
    MasterSyncCfg.HSync.PulseWidth   = 8U;
    MasterSyncCfg.HSync.Polarity     = 1U;
    MasterSyncCfg.VSync.Period       = 1U;
    MasterSyncCfg.VSync.PulseWidth   = 1000U;
    MasterSyncCfg.VSync.Polarity     = 1U;
    MasterSyncCfg.HSyncDelayCycles     = 0U;
    MasterSyncCfg.VSyncDelayCycles     = 0U;
    MasterSyncCfg.ToggleHsyncInVblank  = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(TI03_OV10635MasterSyncChannel[pChan->VinID], &MasterSyncCfg);
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_ConfigVin
 *
 *  @Description:: Configure VIN to receive output frames of the new readout mode
 *
 *  @Input      ::
 *      Chan:       Vin ID and Sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s TI03_OV10635VinConfig = {
        .Config = {
            .FrameRate          = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace         = 0U,
            .BayerPattern       = AMBA_SENSOR_BAYER_PATTERN_BG,
            .YuvOrder           = 0,
            .NumDataBits        = 10,/* RAW10 DT */
            .NumSkipFrame       = 1U,
            .RxHvSyncCtrl           = {
                .NumActivePixels    = 0,
                .NumActiveLines     = 0,
                .NumTotalPixels     = 0,
                .NumTotalLines      = 0,
            },
            .SplitCtrl              = {
                .NumSplits          = 0,
                .SplitWidth         = 0,
            },
            .DelayedVsync       = 0,
        },
        .NumActiveLanes         = 4U,
        .DataType               = 0x2BU,/* RAW10 DT */
        .DataTypeMask           = 0x00U,
        .VirtChanHDREnable      = 0U,
        .VirtChanHDRConfig      = {0},
    };

    UINT32 RetVal;
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s            *pVinCfg = &TI03_OV10635VinConfig;
    UINT32 U32RVal = 0;

    pVinCfg->NumActiveLanes      = pImgInfo->NumDataLanes;
    pVinCfg->Config.BayerPattern = pImgInfo->BayerPattern;

    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels = pImgInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines  = pImgInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels  = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines   = pModeInfo->FrameLengthLines;

    U32RVal |= AmbaWrap_memcpy(&pVinCfg->Config.FrameRate, &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    RetVal = AmbaVIN_MipiConfig(pChan->VinID, pVinCfg);
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

static UINT32 TI03_OV10635_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    UINT32 i;
    AMBA_I2C_TRANSACTION_s I2cTxConfig;
    AMBA_I2C_TRANSACTION_s I2CRxConfig;
    UINT8 TxData[2];
    UINT32 TxSize;

    if ((pChan->SensorID & (TI03_OV10635_SENSOR_ID_CHAN_0 | TI03_OV10635_SENSOR_ID_CHAN_1)) == 0U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cTxConfig.DataSize = 2U;
        I2cTxConfig.pDataBuf = TxData;
        TxData[0] = (UINT8)(Addr >> 8U);
        TxData[1] = (UINT8)(Addr & 0xffU);

        I2CRxConfig.DataSize = 1U;
        I2CRxConfig.pDataBuf = pRxData;

        for (i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                I2cTxConfig.SlaveAddr = SerdesConfig[pChan->VinID].SensorAliasID[i];
                I2CRxConfig.SlaveAddr = (SerdesConfig[pChan->VinID].SensorAliasID[i] | (UINT32)0x1U);
                break;
            }
        }

        RetVal = AmbaI2C_MasterReadAfterWrite(I2cChannel[pChan->VinID], I2C_SPEED_FAST,
                                              1U, &I2cTxConfig, &I2CRxConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C read does not work!!!!! (SlaveID=0x%x, Addr=0x%04x)", I2cTxConfig.SlaveAddr, Addr, 0U, 0U, 0U);
        }
    }

    return RetVal;
}


static UINT32 TI03_OV10635_RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal = SENSOR_ERR_ARG;
    UINT32 i;
    UINT8 TxDataBuf[TI03_OV10635_I2C_DATA_MAX_SIZE + 2U];
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT32 TxSize;
    UINT32 Retry =100;

    if ((pChan == NULL) || (Size > TI03_OV10635_I2C_DATA_MAX_SIZE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.DataSize = (Size + 2U);
        I2cConfig.pDataBuf = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);

        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

        /* Broadcast */
        if (pChan->SensorID == TI03_OV10635Ctrl.EnabledSensorID) {
            while (Retry > 0U) {
                I2cConfig.SlaveAddr = SerdesConfig[pChan->VinID].SensorBroadcastAliasID;
                RetVal = AmbaI2C_MasterWrite(I2cChannel[pChan->VinID], I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);

                if (RetVal != I2C_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work!!!!! (SensorID=0x%x, SlaveID=0x%02x, Addr=0x%04x, Size=%d)", pChan->SensorID, I2cConfig.SlaveAddr, Addr, Size, 0U);
                } else {
                    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C Write (SensorID=0x%x, SlaveID=0x%02x, Addr=0x%04x, Data=0x%02x)", pChan->SensorID, I2cConfig.SlaveAddr, Addr, pTxData[0], 0U);
                    break;
                }
                Retry--;
            }

        } else {
            for (i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    while (Retry > 0U) {
                        I2cConfig.SlaveAddr = SerdesConfig[pChan->VinID].SensorAliasID[i];
                        RetVal = AmbaI2C_MasterWrite(I2cChannel[pChan->VinID], I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);

                        if (RetVal != I2C_ERR_NONE) {
                            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work!!!!! (SensorID=0x%x, SlaveID=0x%02x)", pChan->SensorID, I2cConfig.SlaveAddr, 0U, 0U, 0U);
                        } else {
                            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C Write (SensorID=0x%x, SlaveID=0x%02x, Addr=0x%04x, Data=0x%02x)", pChan->SensorID, I2cConfig.SlaveAddr, Addr, pTxData[0], 0U);
                            break;
                        }
                        Retry--;
                    }
                }
            }
        }

    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_RegisterRead
 *
 *  @Description:: Read sensor registers through I2C bus
 *
 *  @Input      ::
 *      Chan:   Vin ID and Sensor ID
 *      Addr:   Register address
 *
 *  @Output     ::
 *      pData:  Pointer to Read data buffer
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{
    UINT32 RetVal;
    UINT8  RxData = 0xffU;

    RetVal = TI03_OV10635_RegRead(pChan, Addr, &RxData);
    *pData = RxData;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_RegisterWrite
 *
 *  @Description:: Write sensor registers through I2C bus
 *
 *  @Input      ::
 *      Chan:   Vin ID and Sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal;
    UINT8  TxData[1];

    TxData[0] = (UINT8) Data;

    RetVal = TI03_OV10635_RegWrite(pChan, Addr, TxData, 1U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_DesReset
 *
 *  @Description:: Reset TI03_OV10635 Deserializer Device
 *
 *  @Input      ::
 *      VinID:         Vin ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_DesReset(UINT32 VinID)
{
    UINT32 RetVal = GPIO_ERR_NONE;
    static UINT8 VinInit[AMBA_NUM_VIN_CHANNEL] = {0};
    UINT8 VinInitNum = 0;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;

    } else {
        VinInit[VinID] = 1U;

        for (UINT8 i = 0; i < AMBA_NUM_VIN_CHANNEL; i++) {
            VinInitNum += VinInit[i];
        }
        AmbaPrint_PrintUInt5("TI03_OV10635_DesReset() VinID:%u", VinID, 0U, 0U, 0U, 0U);

        if (VinInitNum == 1U) {
            AmbaPrint_PrintUInt5("Reset and Pwer on Des", 0U, 0U, 0U, 0U, 0U);
            /* Des PDB=low and Power disable */
            RetVal |= AmbaUserGPIO_SerdesResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);
            RetVal |= AmbaUserGPIO_SerdesPowerCtrl(VinID, AMBA_GPIO_LEVEL_LOW);
            (void) AmbaKAL_TaskSleep(2);

            /* Power enable and Des PDB=high */
            RetVal |= AmbaUserGPIO_SerdesPowerCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
            RetVal |= AmbaUserGPIO_SerdesResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
            (void) AmbaKAL_TaskSleep(100);

        } else {
            //Can't reset to affect other DES
            RetVal = GPIO_ERR_NONE;

        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_SoftwareReset
 *
 *  @Description:: Reset TI03_OV10635 Image Sensor Device
 *
 *  @Input      ::
 *      SensorID:         Sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SoftwareReset(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    UINT8  TxData[1];

    TxData[0] = 0x01U;

    RetVal = TI03_OV10635_RegWrite(pChan, 0x0103U, TxData, 1U);
    (void) AmbaKAL_TaskSleep(10U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_SetAnalogGainReg
 *
 *  @Description:: Configure Sensor gain setting
 *
 *  @Input      ::
 *      SensorID:         Sensor ID
 *      AnalogGainCtrl:   Analog gain control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8  TxData[2];
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8) & 0x3U);
            TxData[1] = (UINT8)(pAnalogGainCtrl[0] & 0xffU);

            if (pModeInfo->Config.ModeID == TI03_OV10635_1280_800_30P_LONG) {
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_GAIN_LONG_H, TxData, 2U);
                TxData[0] = 0x01;
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AEC_MANUAL_DONE, TxData, 1U);
            } else if (pModeInfo->Config.ModeID == TI03_OV10635_1280_800_30P_SHORT) {
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_GAIN_SHORT_H, TxData, 2U);
                TxData[0] = 0x01;
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AEC_MANUAL_DONE, TxData, 1U);
            } else {
                RetVal = SENSOR_ERR_ARG;
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                TxData[0] = (UINT8)((pAnalogGainCtrl[i] >> 8) & 0x3U);
                TxData[1] = (UINT8)(pAnalogGainCtrl[i] & 0xffU);

                if (i == 0U) {
                    RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_GAIN_LONG_H, TxData, 2U);
                } else if (i == 1U) {
                    RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_GAIN_SHORT_H, TxData, 2U);
                } else {
                    RetVal = SENSOR_ERR_ARG;
                }
                TxData[0] = 0x01;
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AEC_MANUAL_DONE, TxData, 1U);
            }
        } else {
            RetVal = SENSOR_ERR_ARG;
        }

    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::TI03_OV10635_SetWbGainReg
 *
 *  @Description:: Set WB gain control
 *
 *  @Input      ::
 *      Chan:        Vin ID and sensor ID
 *      WBGainCtrl:  WB gain control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWBGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8  TxData[8];
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            // B
            TxData[0] = (UINT8)((pWBGainCtrl[0].B & 0x0300U) >> 8);
            TxData[1] = (UINT8)(pWBGainCtrl[0].B & 0xffU);
            // Gb
            TxData[2] = (UINT8)((pWBGainCtrl[0].Gb  & 0x0300U) >> 8);
            TxData[3] = (UINT8)(pWBGainCtrl[0].Gb  & 0xffU);

            // Gr
            TxData[4] = (UINT8)((pWBGainCtrl[0].Gr & 0x0300U) >> 8);
            TxData[5] = (UINT8)(pWBGainCtrl[0].Gr & 0xffU);

            // R
            TxData[6] = (UINT8)((pWBGainCtrl[0].R  & 0x0300U) >> 8);
            TxData[7] = (UINT8)(pWBGainCtrl[0].R  & 0xffU);

            if (pModeInfo->Config.ModeID == TI03_OV10635_1280_800_30P_LONG) {
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AWB_B_LONG_H, TxData, 8U);
            } else if (pModeInfo->Config.ModeID == TI03_OV10635_1280_800_30P_SHORT) {
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AWB_B_SHORT_H, TxData, 8U);
            } else {
                RetVal = SENSOR_ERR_ARG;
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                // B
                TxData[0] = (UINT8)((pWBGainCtrl[i].B & 0x0300U) >> 8);
                TxData[1] = (UINT8)(pWBGainCtrl[i].B & 0xffU);
                // Gb
                TxData[2] = (UINT8)((pWBGainCtrl[i].Gb  & 0x0300U) >> 8);
                TxData[3] = (UINT8)(pWBGainCtrl[i].Gb  & 0xffU);

                // Gr
                TxData[4] = (UINT8)((pWBGainCtrl[i].Gr & 0x0300U) >> 8);
                TxData[5] = (UINT8)(pWBGainCtrl[i].Gr & 0xffU);

                // R
                TxData[6] = (UINT8)((pWBGainCtrl[i].R  & 0x0300U) >> 8);
                TxData[7] = (UINT8)(pWBGainCtrl[i].R  & 0xffU);

                if (i == 0U) {
                    RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AWB_B_LONG_H, TxData, 8U);
                } else if (i == 1U) {
                    RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AWB_B_SHORT_H, TxData, 8U);
                } else {
                    RetVal = SENSOR_ERR_ARG;
                }
            }

        } else {
            RetVal = SENSOR_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      SensorID:    Sensor ID
 *      ShutterCtrl: Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8  TxData[2];
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pShutterCtrl[0] >> 8) & 0xffU);
            TxData[1] = (UINT8)(pShutterCtrl[0] & 0xffU);

            if (pModeInfo->Config.ModeID == TI03_OV10635_1280_800_30P_LONG) {
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_INT_EXP_LONG_H, TxData, 2U);
                TxData[0] = 0x01;
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AEC_MANUAL_DONE, TxData, 1U);
            } else if (pModeInfo->Config.ModeID == TI03_OV10635_1280_800_30P_SHORT) {
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_INT_EXP_SHORT_H, TxData, 2U);
                TxData[0] = 0x01;
                RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AEC_MANUAL_DONE, TxData, 1U);
            } else {
                RetVal = SENSOR_ERR_ARG;
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                TxData[0] = (UINT8)((pShutterCtrl[i] >> 8) & 0xffU);
                TxData[1] = (UINT8)(pShutterCtrl[i] & 0xffU);

                if (i == 0U) {
                    RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_INT_EXP_LONG_H, TxData, 2U);
                } else if (i == 1U) {
                    RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_INT_EXP_SHORT_H, TxData, 2U);
                } else {
                    RetVal = SENSOR_ERR_ARG;
                }
            }
            TxData[0] = 0x01;
            RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_AEC_MANUAL_DONE, TxData, 1U);
        } else {
            RetVal = SENSOR_ERR_ARG;
        }

    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_SetSlowShutterReg
 *
 *  @Description:: Apply slow shutter setting
 *
 *  @Input      ::
 *      SensorID:         Sensor ID
 *      SlowShutterCtrl:  Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SetSlowShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8  TxData[2];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        TxData[0] = (UINT8)((SlowShutterCtrl >> 8) & 0xffU);
        TxData[1] = (UINT8)(SlowShutterCtrl & 0xffU);
        RetVal |= TI03_OV10635_RegWrite(pChan, OV10635_VTS_H, TxData, 2U);
    } else {
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      SensorID:      Sensor ID
 *      ReadoutMode:   Sensor readout mode number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SensorMode)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 DataSize;
    UINT32 RegNum;
    UINT16 FirstAddr, NextAddr;
    UINT8 FirstData, NextData;
    UINT8 TxData[TI03_OV10635_I2C_DATA_MAX_SIZE];
    const TI03_OV10635_MODE_REG_s *pModeRegTable;
    const TI03_OV10635_LINEARMODE_REG_s *pLinearModeRegTable;
    const TI03_OV10635_SEQ_REG_s *pSHDRModeRegTable;
    UINT16 i = 0U, j;

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        pLinearModeRegTable = &TI03_OV10635ModeRegTable[0];
        RegNum = TI03_OV10635_NUM_READOUT_MODE_REG;
        /*
        for (i = 0U; i < RegNum; i += DataSize) {
            DataSize = 0;
            FirstAddr = pLinearModeRegTable[i].Addr;
            if (SensorMode == TI03_OV10635_1280_800_30P_SHORT) {
                FirstData = pLinearModeRegTable[i].Data[1];
            } else {
                FirstData = pLinearModeRegTable[i].Data[0];
            }

            TxData[DataSize] = FirstData;
            DataSize++;

            for (j = i + 1U; j < RegNum; j++) {
                NextAddr = pLinearModeRegTable[j].Addr;
                if (SensorMode == TI03_OV10635_1280_800_30P_SHORT) {
                    NextData = pLinearModeRegTable[j].Data[1];
                } else {
                    NextData = pLinearModeRegTable[j].Data[0];
                }

                if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)((UINT16)TI03_OV10635_I2C_DATA_MAX_SIZE - 3U))) {
                    break;
                } else {
                    TxData[DataSize] = NextData;
                    DataSize++;
                }
            }

            RetVal = TI03_OV10635_RegWrite(pChan, FirstAddr, TxData, DataSize);
        }*/
        i = 0U;
        while(i < RegNum) {
            DataSize = 0;
            FirstAddr = pLinearModeRegTable[i].Addr;
            if (SensorMode == TI03_OV10635_1280_800_30P_SHORT) {
                FirstData = pLinearModeRegTable[i].Data[1];
            } else {
                FirstData = pLinearModeRegTable[i].Data[0];
            }

            TxData[DataSize] = FirstData;
            DataSize++;

            for (j = i + 1U; j < RegNum; j++) {
                NextAddr = pLinearModeRegTable[j].Addr;
                if (SensorMode == TI03_OV10635_1280_800_30P_SHORT) {
                    NextData = pLinearModeRegTable[j].Data[1];
                } else {
                    NextData = pLinearModeRegTable[j].Data[0];
                }

                if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)((UINT16)TI03_OV10635_I2C_DATA_MAX_SIZE - 3U))) {
                    break;
                } else {
                    if(DataSize < TI03_OV10635_I2C_DATA_MAX_SIZE) {
                        TxData[DataSize] = NextData;
                        DataSize++;
                    }
                }
            }

            RetVal = TI03_OV10635_RegWrite(pChan, FirstAddr, TxData, DataSize);
            i += DataSize;
        }
    } else {
        pSHDRModeRegTable = &TI03_OV10635SHDRRegTable[0];
        RegNum = TI03_OV10635_NUM_SHDR_MODE_REG;
        /*
        for (i = 0U; i < RegNum; i += DataSize) {
            DataSize = 0;
            FirstAddr = pSHDRModeRegTable[i].Addr;
            FirstData = pSHDRModeRegTable[i].Data;

            TxData[DataSize] = FirstData;
            DataSize++;

            for (j = i + 1U; j < RegNum; j++) {
                NextAddr = pSHDRModeRegTable[j].Addr;
                NextData = pSHDRModeRegTable[j].Data;

                if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)((UINT16)TI03_OV10635_I2C_DATA_MAX_SIZE - 3U))) {
                    break;
                } else {
                    TxData[DataSize] = NextData;
                    DataSize++;
                }
            }

            RetVal = TI03_OV10635_RegWrite(pChan, FirstAddr, TxData, DataSize);
        }*/
        i = 0U;
        while(i < RegNum) {
            DataSize = 0;
            FirstAddr = pSHDRModeRegTable[i].Addr;
            FirstData = pSHDRModeRegTable[i].Data;

            TxData[DataSize] = FirstData;
            DataSize++;

            for (j = i + 1U; j < RegNum; j++) {
                NextAddr = pSHDRModeRegTable[j].Addr;
                NextData = pSHDRModeRegTable[j].Data;

                if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)((UINT16)TI03_OV10635_I2C_DATA_MAX_SIZE - 3U))) {
                    break;
                } else {
                    if(DataSize < TI03_OV10635_I2C_DATA_MAX_SIZE) {
                        TxData[DataSize] = NextData;
                        DataSize++;
                    }
                }
            }

            RetVal = TI03_OV10635_RegWrite(pChan, FirstAddr, TxData, DataSize);
            i += DataSize;
        }
    }

    pModeRegTable = &TI03_OV10635PureRawRegTable[0];
    RegNum = TI03_OV10635_NUM_PURERAW_MODE_REG;
    for (i = 0U; i < RegNum; i ++) {
        TxData[0] = pModeRegTable[i].Data[SensorMode];
        RetVal = TI03_OV10635_RegWrite(pChan, pModeRegTable[i].Addr, TxData, 1);
    }

#ifdef OV10635_MANUAL_AEC
    pModeRegTable = &TI03_OV10635MANAECRegTable[0];
    RegNum = TI03_OV10635_NUM_MANAEC_MODE_REG;
    for (i = 0U; i < RegNum; i ++) {
        TxData[0] = pModeRegTable[i].Data[SensorMode];
        RetVal = TI03_OV10635_RegWrite(pChan, pModeRegTable[i].Addr, TxData, 1);
    }
#endif

    if (SerdesConfig[pChan->VinID].FrameSyncSel.FrameSyncEnable != TI913_954_NO_FSYNC) {
        pModeRegTable = &TI03_OV10635FSINRegTable[0];
        RegNum = TI03_OV10635_NUM_FSIN_MODE_REG;
        for (i = 0U; i < RegNum; i ++) {
            TxData[0] = pModeRegTable[i].Data[SensorMode];
            RetVal = TI03_OV10635_RegWrite(pChan, pModeRegTable[i].Addr, TxData, 1);
        }
    }

    /* reset gain/shutter ctrl information */
    for (i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i++) {
        if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentAgcCtrl[i][0]     = 0x18;
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentDgcCtrl[i][0]     = 0x01;
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentShutterCtrl[i][0] = 0x340;
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][0].R    = 0x80;
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][0].Gr   = 0x80;
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][0].Gb   = 0x80;
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][0].B    = 0x80;

            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                for (j = 0U; j < pModeInfo->HdrInfo.ActiveChannels; j ++) {
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentAgcCtrl[i][j]     = 0x18;
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentDgcCtrl[i][j]     = 0x01;
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentShutterCtrl[i][j] = 0x340;
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][j].R    = 0x80;
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][j].Gr   = 0x80;
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][j].Gb   = 0x80;
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][j].B    = 0x80;
                }

            } else {
                RetVal |= SENSOR_ERR_ARG;
            }
        } else {
            continue;
        }
    }

    return RetVal;
}

static void TI03_OV10635_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;

    for (i = 0U; i < TI03_OV10635_NUM_MODE; i++) {
        TI03_OV10635ModeInfoList[i].RowTime = (FLOAT)(((DOUBLE)TI03_OV10635SensorInfo[i].LineLengthPck /
                                              ((DOUBLE)TI03_OV10635SensorInfo[i].DataRate *
                                               (DOUBLE)TI03_OV10635SensorInfo[i].NumDataLanes /
                                               (DOUBLE)TI03_OV10635SensorInfo[i].NumDataBits)) * 2.);
        pOutputInfo = &TI03_OV10635OutputInfo[i];
        TI03_OV10635ModeInfoList[i].PixelRate = (FLOAT)((DOUBLE)pOutputInfo->DataRate * (DOUBLE)pOutputInfo->NumDataLanes / (DOUBLE)pOutputInfo->NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      Chan: Vin ID and Sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    /* Note: Although Deserializer has internal oscillator providing the clock for configuration link, we still
     * need to set this clock for gclk_so_vin, so vin can have correct ref. clock to receive mipi data */
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, TI03_OV10635SensorInfo[0U].InputClk);
#if 0 /* no need to send out the clock */
        AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_OUTPUT);    /* Config clock output */
#endif
        TI03_OV10635_PreCalculateModeInfo();
        TI03_OV10635Ctrl.EnabledSensorID = pChan->SensorID;
    }
    AmbaPrint_PrintUInt5("TI03_OV10635_Init: EnabledSensorID=0x%x", TI03_OV10635Ctrl.EnabledSensorID, 0, 0, 0, 0);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and Sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* Reset Des (PDB) */
        RetVal |= TI03_OV10635_DesReset(pChan->VinID);
    }

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "TI03_OV10635_Enable", NULL, NULL, NULL, NULL, NULL);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and Sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= TI03_OV10635_SoftwareReset(pChan);
    }

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "TI03_OV10635_Disable", NULL, NULL, NULL, NULL, NULL);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_GetStatus
 *
 *  @Description:: Get current sensor status
 *
 *  @Input      ::
 *      Chan: Vin ID and Sensor ID
 *
 *  @Output     ::
 *      pStatus: pointer to current sensor status
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pStatus, &TI03_OV10635Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_GetModeInfo
 *
 *  @Description:: Get Mode Info of indicated mode
 *
 *  @Input      ::
 *      Chan:   Vin ID and Sensor ID
 *      Mode:   Sensor Readout Mode Number
 *
 *  @Output     ::
 *      pModeInfo: pointer to requested Mode Info
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "TI03_OV10635_GetModeInfo", NULL, NULL, NULL, NULL, NULL);

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = TI03_OV10635Ctrl.Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }
        if (Config.ModeID >= TI03_OV10635_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            TI03_OV10635_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_GetDeviceInfo
 *
 *  @Description:: Get Sensor Device Info
 *
 *  @Input      ::
 *      Chan: Vin ID and Sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo: pointer to device info
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &TI03_OV10635DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_GetCurrentGainFactor
 *
 *  @Description:: Get Sensor Current Gain Factor
 *
 *  @Input      ::
 *      Chan:   Vin ID and Sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to current gain factor
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 CurrentAgcCtrl;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (UINT32 i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                    CurrentAgcCtrl = TI03_OV10635Ctrl.CurrentAEInfo.CurrentAgcCtrl[i][0];
                    pGainFactor[0] = (FLOAT)((DOUBLE)CurrentAgcCtrl / 16.);
                } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                    for (UINT32 j = 0U; j < pModeInfo->HdrInfo.ActiveChannels; j ++) {
                        CurrentAgcCtrl = TI03_OV10635Ctrl.CurrentAEInfo.CurrentAgcCtrl[i][j];
                        pGainFactor[j] = (FLOAT)((DOUBLE)CurrentAgcCtrl / 16.);
                    }

                } else {
                    RetVal = SENSOR_ERR_INVALID_API;
                }
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:   Vin ID and Sensor ID
 *
 *  @Output     ::
 *      pExposureTime:    pointer to current exposure time
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    DOUBLE ShutterTimeUnit;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

        for (UINT32 i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                    pExposureTime[0] = ((FLOAT)ShutterTimeUnit * (FLOAT)TI03_OV10635Ctrl.CurrentAEInfo.CurrentShutterCtrl[i][0]);
                } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                    for (UINT32 j = 0U; j < pModeInfo->HdrInfo.ActiveChannels; j ++) {
                        pExposureTime[j] = ((FLOAT)ShutterTimeUnit * (FLOAT)TI03_OV10635Ctrl.CurrentAEInfo.CurrentShutterCtrl[i][j]);
                    }
                } else {
                    RetVal = SENSOR_ERR_INVALID_API;
                }
            }
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_ConvertGainFactor
 *
 *  @Description:: Convert gain factor to analog and digital gain control
 *
 *  @Input      ::
 *      Chan:             Vin ID and Sensor ID
 *      DesiredFactor:    Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:    Achievable gain factor (12.20 fixed point)
 *      pAnalogGainCtrl:  Analog gain control for achievable gain factor
 *      pDigitalGainCtrl: Digital gain control for achievable gain factor
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT  MaxGain, MinGain;
    FLOAT DesiredFactor;
    AMBA_SENSOR_WB_GAIN_FACTOR_s DesiredWbGain;
    DOUBLE GainInDb, WbGainInDb;
    UINT32 i;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {

            DesiredFactor = pDesiredFactor->Gain[0];
            MaxGain = TI03_OV10635DeviceInfo.MaxAnalogGainFactor;
            MinGain = TI03_OV10635DeviceInfo.MinAnalogGainFactor;
            if (DesiredFactor < MinGain) {
                DesiredFactor = MinGain;
            } else if (DesiredFactor > MaxGain) {
                DesiredFactor = MaxGain;
            } else {
                /* for misraC check */
            }

            U32RVal |= AmbaWrap_floor((DOUBLE)((DOUBLE)DesiredFactor * 16.0f), &GainInDb);
            pGainCtrl->DigitalGain[0] = 1;
            pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
            pActualFactor->Gain[0] = (FLOAT)pGainCtrl->AnalogGain[0] / 16.0f;

            /*********** WB gain ***********/
            DesiredWbGain.R  = (pDesiredFactor->WbGain[0].R >= 7.9921875f)  ? 7.9921875f : pDesiredFactor->WbGain[0].R;
            DesiredWbGain.Gr = (pDesiredFactor->WbGain[0].Gr >= 7.9921875f) ? 7.9921875f : pDesiredFactor->WbGain[0].Gr;
            DesiredWbGain.Gb = (pDesiredFactor->WbGain[0].Gb >= 7.9921875f) ? 7.9921875f : pDesiredFactor->WbGain[0].Gb;
            DesiredWbGain.B  = (pDesiredFactor->WbGain[0].B >= 7.9921875f)  ? 7.9921875f : pDesiredFactor->WbGain[0].B;

            DesiredWbGain.R  = (pDesiredFactor->WbGain[0].R <= 1.0f)  ? 1.0f : DesiredWbGain.R;
            DesiredWbGain.Gr = (pDesiredFactor->WbGain[0].Gr <= 1.0f) ? 1.0f : DesiredWbGain.Gr;
            DesiredWbGain.Gb = (pDesiredFactor->WbGain[0].Gb <= 1.0f) ? 1.0f : DesiredWbGain.Gb;
            DesiredWbGain.B  = (pDesiredFactor->WbGain[0].B <= 1.0f)  ? 1.0f : DesiredWbGain.B;

            U32RVal |= AmbaWrap_floor((DOUBLE)(DesiredWbGain.R * 128.0), &WbGainInDb);
            pGainCtrl->WbGain[0].R = (UINT32)WbGainInDb;
            U32RVal |= AmbaWrap_floor((DOUBLE)(DesiredWbGain.Gr * 128.0), &WbGainInDb);
            pGainCtrl->WbGain[0].Gr = (UINT32)WbGainInDb;
            U32RVal |= AmbaWrap_floor((DOUBLE)(DesiredWbGain.Gb * 128.0), &WbGainInDb);
            pGainCtrl->WbGain[0].Gb = (UINT32)WbGainInDb;
            U32RVal |= AmbaWrap_floor((DOUBLE)(DesiredWbGain.B * 128.0), &WbGainInDb);
            pGainCtrl->WbGain[0].B = (UINT32)WbGainInDb;

            pActualFactor->WbGain[0].R  = (FLOAT)pGainCtrl->WbGain[0].R  / 128.0f;
            pActualFactor->WbGain[0].Gr = (FLOAT)pGainCtrl->WbGain[0].Gr / 128.0f;
            pActualFactor->WbGain[0].Gb = (FLOAT)pGainCtrl->WbGain[0].Gb / 128.0f;
            pActualFactor->WbGain[0].B  = (FLOAT)pGainCtrl->WbGain[0].B  / 128.0f;
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {

                DesiredFactor = pDesiredFactor->Gain[i];
                MaxGain = TI03_OV10635DeviceInfo.MaxAnalogGainFactor;
                MinGain = TI03_OV10635DeviceInfo.MinAnalogGainFactor;
                if (DesiredFactor < MinGain) {
                    DesiredFactor = MinGain;
                } else if (DesiredFactor > MaxGain) {
                    DesiredFactor = MaxGain;
                }  else {
                    /* for misraC check */
                }

                U32RVal |= AmbaWrap_floor((DOUBLE)((DOUBLE)DesiredFactor * 16.0f), &GainInDb);
                pGainCtrl->DigitalGain[i] = 1;
                pGainCtrl->AnalogGain[i] = (UINT32)GainInDb;
                pActualFactor->Gain[i] = (FLOAT)pGainCtrl->AnalogGain[i] / 16.0f;

                /*********** WB gain ***********/
                DesiredWbGain.R  = (pDesiredFactor->WbGain[i].R >= 7.9921875f)  ? 7.9921875f : pDesiredFactor->WbGain[i].R;
                DesiredWbGain.Gr = (pDesiredFactor->WbGain[i].Gr >= 7.9921875f) ? 7.9921875f : pDesiredFactor->WbGain[i].Gr;
                DesiredWbGain.Gb = (pDesiredFactor->WbGain[i].Gb >= 7.9921875f) ? 7.9921875f : pDesiredFactor->WbGain[i].Gb;
                DesiredWbGain.B  = (pDesiredFactor->WbGain[i].B >= 7.9921875f)  ? 7.9921875f : pDesiredFactor->WbGain[i].B;

                DesiredWbGain.R  = (pDesiredFactor->WbGain[i].R <= 1.0f)  ? 1.0f : DesiredWbGain.R;
                DesiredWbGain.Gr = (pDesiredFactor->WbGain[i].Gr <= 1.0f) ? 1.0f : DesiredWbGain.Gr;
                DesiredWbGain.Gb = (pDesiredFactor->WbGain[i].Gb <= 1.0f) ? 1.0f : DesiredWbGain.Gb;
                DesiredWbGain.B  = (pDesiredFactor->WbGain[i].B <= 1.0f)  ? 1.0f : DesiredWbGain.B;

                U32RVal |= AmbaWrap_floor((DOUBLE)(DesiredWbGain.R * 128.0), &WbGainInDb);
                pGainCtrl->WbGain[i].R = (UINT32)WbGainInDb;
                U32RVal |= AmbaWrap_floor((DOUBLE)(DesiredWbGain.Gr * 128.0), &WbGainInDb);
                pGainCtrl->WbGain[i].Gr = (UINT32)WbGainInDb;
                U32RVal |= AmbaWrap_floor((DOUBLE)(DesiredWbGain.Gb * 128.0), &WbGainInDb);
                pGainCtrl->WbGain[i].Gb = (UINT32)WbGainInDb;
                U32RVal |= AmbaWrap_floor((DOUBLE)(DesiredWbGain.B * 128.0), &WbGainInDb);
                pGainCtrl->WbGain[i].B = (UINT32)WbGainInDb;

                pActualFactor->WbGain[i].R  = (FLOAT)pGainCtrl->WbGain[i].R  / 128.0f;
                pActualFactor->WbGain[i].Gr = (FLOAT)pGainCtrl->WbGain[i].Gr / 128.0f;
                pActualFactor->WbGain[i].Gb = (FLOAT)pGainCtrl->WbGain[i].Gb / 128.0f;
                pActualFactor->WbGain[i].B  = (FLOAT)pGainCtrl->WbGain[i].B  / 128.0f;
            }

        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    //AmbaPrint("AnalogGain:%f", AnalogGain);
    //AmbaPrint("DesiredFactor = %f, AgainReg = 0x%08x, ActualFactor = %d",DesiredFactor, *pAnalogGainCtrl, *pActualFactor);
    if(U32RVal != 0U) {
        /* MisraC */
    }


    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_ConvertShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      Chan:         Vin ID and Sensor ID
 *      ExposureTime: Exposure time / shutter speed
 *
 *  @Output     ::
 *      pShutterCtrl: Electronic shutter control
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 MaxShrWidth;
    UINT32 MinShrWidth;
    DOUBLE ShutterTimeUnit, FloorVal;
    UINT32 ShutterCtrl;
    UINT32 i = 0;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * ((DOUBLE)pModeInfo->FrameLengthLines / (DOUBLE)pModeInfo->NumExposureStepPerFrame);
            U32RVal |= AmbaWrap_floor((DOUBLE)pDesiredExposureTime[0] / ShutterTimeUnit, &FloorVal);
            ShutterCtrl = (UINT32)FloorVal;

            /* Exposure lines needs to be smaller than VTS - 8 */
            MaxShrWidth = pModeInfo->NumExposureStepPerFrame - 8U;
            /* Exposure lines needs to be bigger 1 */
            MinShrWidth = 1U;

            if (ShutterCtrl > MaxShrWidth) {
                ShutterCtrl = MaxShrWidth;
            } else if (ShutterCtrl < MinShrWidth) {
                ShutterCtrl = MinShrWidth;
            } else {
                /* MisraC */ //(void)ShutterCtrl;
            }
            pShutterCtrl[0] = ShutterCtrl;
            pActualExposureTime[0] = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * ((DOUBLE)pModeInfo->FrameLengthLines / (DOUBLE)pModeInfo->NumExposureStepPerFrame);
                U32RVal |= AmbaWrap_floor((DOUBLE)pDesiredExposureTime[i] / ShutterTimeUnit, &FloorVal);
                ShutterCtrl = (UINT32)FloorVal;

                /* Exposure lines needs to be smaller than VTS - 8 */
                MaxShrWidth = pModeInfo->NumExposureStepPerFrame - 8U;
                /* Exposure lines needs to be bigger 1 */
                MinShrWidth = 1U;

                if (ShutterCtrl > MaxShrWidth) {
                    ShutterCtrl = MaxShrWidth;
                } else if (ShutterCtrl < MinShrWidth) {
                    ShutterCtrl = MinShrWidth;
                } else {
                    /* MisraC */ //(void)ShutterCtrl;
                }
                pShutterCtrl[i] = ShutterCtrl;
                pActualExposureTime[i] = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
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
 *  @RoutineName:: TI03_OV10635_SetAnalogGainCtrl
 *
 *  @Description:: Set analog gain control
 *
 *  @Input      ::
 *      Chan:            Vin ID and Sensor ID
 *      AnalogGainCtrl:  Analog gain control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= TI03_OV10635_SetAnalogGainReg(pChan, pAnalogGainCtrl);

    /* Update current AGC control */
    for (UINT32 i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i++) {
        if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentAgcCtrl[i][0] = pAnalogGainCtrl[0];
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                for (UINT32 j = 0U; j < pModeInfo->HdrInfo.ActiveChannels; j ++) {
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentAgcCtrl[i][j] = pAnalogGainCtrl[j];
                }

            } else {
                RetVal |= SENSOR_ERR_ARG;
            }
        } else {
            continue;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      Chan:              Vin ID and Sensor ID
 *      DigitalGainCtrl:   Digital gain control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AmbaMisra_TouchUnused(&pDigitalGainCtrl);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName::TI03_OV10635_SetWbGainCtrl
 *
 *  @Description:: Set WB gain control
 *
 *  @Input      ::
 *      Chan:         Vin ID and sensor ID
 *      pWBGainCtrl:  HDR WB gain control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= TI03_OV10635_SetWbGainReg(pChan, pWbGainCtrl);

    /* Update current WBgain control */
    for (UINT32 i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i++) {
        if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][0] = pWbGainCtrl[0];
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                for (UINT32 j = 0U; j < pModeInfo->HdrInfo.ActiveChannels; j ++) {
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentWbCtrl[i][j] = pWbGainCtrl[j];
                }

            } else {
                RetVal |= SENSOR_ERR_ARG;
            }
        } else {
            continue;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_SetShutterCtrl
 *
 *  @Description:: set shutter control
 *
 *  @Input      ::
 *      Chan:        Vin ID and Sensor ID
 *      ShutterCtrl: Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= TI03_OV10635_SetShutterReg(pChan, pShutterCtrl);

    /* Update current shutter control */
    for (UINT32 i = 0U; i < TI03_OV10635_NUM_MAX_SENSOR_COUNT; i++) {
        if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                TI03_OV10635Ctrl.CurrentAEInfo.CurrentShutterCtrl[i][0] = pShutterCtrl[0];
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                for (UINT32 j = 0U; j < pModeInfo->HdrInfo.ActiveChannels; j ++) {
                    TI03_OV10635Ctrl.CurrentAEInfo.CurrentShutterCtrl[i][j] = pShutterCtrl[j];
                }
            } else {
                RetVal |= SENSOR_ERR_ARG;
            }
        } else {
            continue;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_SetSlowShutterCtrl
 *
 *  @Description:: set slow shutter control
 *
 *  @Input      ::
 *      Chan:            Vin ID and Sensor ID
 *      SlowShutterCtrl: Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI03_OV10635Ctrl.Status.ModeInfo;
    UINT32 ModeID = pModeInfo->Config.ModeID;
    UINT32 TargetFrameLengthLines = 0U;

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        TargetFrameLengthLines = TI03_OV10635SensorInfo[ModeID].FrameLengthLines * SlowShutterCtrl;

        RetVal = TI03_OV10635_SetSlowShutterReg(pChan, TargetFrameLengthLines);

        /* Update frame rate information */
        pModeInfo->FrameLengthLines = TargetFrameLengthLines;
        pModeInfo->NumExposureStepPerFrame = TargetFrameLengthLines;
        pModeInfo->FrameRate.NumUnitsInTick = pModeInfo->FrameRate.NumUnitsInTick * SlowShutterCtrl;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI03_OV10635_Config
 *
 *  @Description:: Set sensor to indicated mode
 *
 *  @Input      ::
 *      Chan:            Vin ID and Sensor ID
 *      Mode:            Sensor Readout Mode Number
 *      ElecShutterMode: Operating Mode of the Electronic Shutter
 *
 *  @Output     ::
 *      pModeInfo: pointer to mode info of target mode
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI03_OV10635_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0x00000fU,  /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0x00f000U,  /* SD_LVDS_12~15 */
        [AMBA_VIN_CHANNEL2] = 0x0000f0U,  /* SD_LVDS_4~7 */
        [AMBA_VIN_CHANNEL3] = 0x000f00U,  /* SD_LVDS_8~11 */
        [AMBA_VIN_CHANNEL4] = 0x0f0000U,  /* SD_LVDS_16~19 */
        [AMBA_VIN_CHANNEL5] = 0xf00000U,  /* SD_LVDS_20~23 */
    };

    AMBA_VIN_MIPI_PAD_CONFIG_s TI03_OV10635PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        //.EnabledPin = EnabledPin[pChan->VinID]
    };

    UINT32 RetVal                       = SENSOR_ERR_NONE;
    UINT32 SensorMode                   = pMode->ModeID & 0xFFFFFFFU;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo  = &TI03_OV10635Ctrl.Status.ModeInfo;

    if ((pChan == NULL) || (SensorMode >= TI03_OV10635_NUM_MODE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TI03_OV10635PadConfig.EnabledPin = EnabledPin[pChan->VinID];
        /* update status */
        AmbaPrint_PrintUInt5("TI03_OV10635_Config:: SensorID 0x%x, SensorMode %d", pChan->SensorID, SensorMode, 0U, 0U, 0U);

        TI03_OV10635_PrepareModeInfo(pChan, pMode, pModeInfo);

        TI03_OV10635PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &TI03_OV10635PadConfig);

        /* Prepare SerDes info */
        SerdesConfig[pChan->VinID].EnabledRxPortID = TI03_OV10635_GetEnabledLinkID(pChan->SensorID);
        SerdesConfig[pChan->VinID].CSIOutputInfo.NumDataLanes = TI03_OV10635OutputInfo[pMode->ModeID].NumDataLanes;
        SerdesConfig[pChan->VinID].FrameSyncSel.TimeScale = pModeInfo->FrameRate.TimeScale;
        SerdesConfig[pChan->VinID].FrameSyncSel.NumUnitsInTick = pModeInfo->FrameRate.NumUnitsInTick;

        /* Init SerDes */
        RetVal |= TI913_954_Init(pChan->VinID, &SerdesConfig[pChan->VinID]);

#if defined(CONFIG_BSP_CV2BUB_OPTION_A_V300) || defined(CONFIG_BSP_CV2DK)
        /* Power on / Reset Ser by Des GPIO */
        RetVal |= TI913_954_SetGpioOutput(pChan->VinID, TI954_CHIP_ID, TI954_GPIO_PIN_1, 1U);
        (void) AmbaKAL_TaskSleep(10);
#endif

        /* Config SerDes */
        (void) TI913_954_Config(pChan->VinID);

        /* Onboard sensor clock */

        /* reset all sensors */
        RetVal |= TI03_OV10635_SoftwareReset(pChan);

        /* write registers of mode change to sensor */
        RetVal |= TI03_OV10635_ChangeReadoutMode(pChan, SensorMode);

        /* config vin master sync generation for frame sync */
        if (SerdesConfig[pChan->VinID].FrameSyncSel.FrameSyncEnable == TI913_954_EXTERNAL_FSYNC) {
            RetVal |= TI03_OV10635_ConfigVinMasterSync(pChan, SensorMode);
        }

        /* Enable TI954 CSI-2 output */
        RetVal |= TI913_954_EnableCSIOutput(pChan->VinID);

        /* config vin main config */
        RetVal |= TI03_OV10635_ConfigVin(pChan, pModeInfo);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_TI03_OV10635Obj = {
    .SensorName             = "OV10635",
    .SerdesName             = "TI03",
    .Init                   = TI03_OV10635_Init,
    .Enable                 = TI03_OV10635_Enable,
    .Disable                = TI03_OV10635_Disable,
    .Config                 = TI03_OV10635_Config,
    .GetStatus              = TI03_OV10635_GetStatus,
    .GetModeInfo            = TI03_OV10635_GetModeInfo,
    .GetDeviceInfo          = TI03_OV10635_GetDeviceInfo,
    .GetCurrentGainFactor   = TI03_OV10635_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = TI03_OV10635_GetCurrentShutterSpeed,
    .ConvertGainFactor      = TI03_OV10635_ConvertGainFactor,
    .ConvertShutterSpeed    = TI03_OV10635_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = TI03_OV10635_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = TI03_OV10635_SetDigitalGainCtrl,
    .SetShutterCtrl         = TI03_OV10635_SetShutterCtrl,
    .SetSlowShutterCtrl     = TI03_OV10635_SetSlowShutterCtrl,
    .SetWbGainCtrl          = TI03_OV10635_SetWbGainCtrl,

    .RegisterRead           = TI03_OV10635_RegisterRead,
    .RegisterWrite          = TI03_OV10635_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};

