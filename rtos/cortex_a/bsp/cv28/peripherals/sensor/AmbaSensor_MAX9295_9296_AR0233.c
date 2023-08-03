/**
 *  @file AmbaSensor_MAX9295_9296_AR0233.c
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

#include <AmbaWrap.h>
#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_AR0233.h"
#include "AmbaSbrg_Max9295_9296.h"

#include "bsp.h"

#define CG_RATIO           14.8f   /* CG brightness between HCG/LCG */
#define T1_T2_SHIFT_BIT       7U   /*128x*/

#define MX_AR0233_DRV_VERSION 0x20210125U
#define SERDES_ENABLE
#define MX00_AR0233_IN_SLAVE_MODE


/*-----------------------------------------------------------------------------------------------*\
 * MX00_AR0233 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX00_AR0233_CTRL_s MX00_AR0233Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};
//Please noted VIN0 supports up to 8lane, PIP supports up to 4lane(shared with VIN0), PIP2 support up to 2lane(shared with PIP)
static UINT32 MX00_AR0233I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_I2C_CHANNEL_PIP2,
};

#ifdef MX00_AR0233_IN_SLAVE_MODE
static UINT32 MX00_AR0233MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_MSYNC_CHANNEL_PIP2,
};
#endif
#ifdef SERDES_ENABLE
static MAX9295_9296_SERDES_CONFIG_s MX00_AR0233SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_AR0233_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS + 0x10U),
            [1] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS + 0x12U),
        },
        .NumDataBits     = 16,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [1] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_AR0233_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS + 0x14U),
            [1] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS + 0x16U),
        },
        .NumDataBits     = 16,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [2] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_AR0233_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS + 0x18U),
            [1] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS + 0x1AU),
        },
        .NumDataBits     = 16,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
};

static UINT16 MX00_AR0233_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 i, EnabledLinkID = 0U;

    for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}
#else
static MAX9295_9296_SERDES_CONFIG_s MX00_AR0233SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_AR0233_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS),
            [1] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS),
        },
        .NumDataBits     = 16,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
    },
    [1] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_AR0233_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS),
            [1] = (UINT8)(MX00_AR0233_I2C_SLAVE_ADDRESS),
        },
        .NumDataBits     = 16,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
    },
};
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_GetNumActiveSensor
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
static UINT32 MX00_AR0233_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U;
    UINT32 i;

    for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            SensorNum ++;
        }
    }

    return (SensorNum > 1U) ? SensorNum : 1U;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_PrepareModeInfo
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
static void MX00_AR0233_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID = pMode->ModeID;
    const MX00_AR0233_SENSOR_INFO_s *pSensorInfo = &MX00_AR0233SensorInfo[ModeID];
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo = &MX00_AR0233InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &MX00_AR0233OutputInfo[ModeID];
    UINT32 SensorNum = MX00_AR0233_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    pModeInfo->FrameLengthLines = pSensorInfo->FrameLengthLines * SensorNum;
    pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    pModeInfo->InternalExposureOffset = 0.0f;
    pModeInfo->InputClk = pSensorInfo->InputClk;
    pModeInfo->RowTime = MX00_AR0233ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
    U32RVal |= AmbaWrap_floor((((DOUBLE)MX00_AR0233ModeInfoList[ModeID].PixelRate * (DOUBLE)SensorNum) * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal);
    pModeInfo->LineLengthPck = (UINT32)FloorVal;

    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &(MX00_AR0233ModeInfoList[ModeID].FrameRate), sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &(MX00_AR0233ModeInfoList[ModeID].FrameRate), sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX00_AR0233HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    pModeInfo->OutputInfo.DataRate *= SensorNum;
    pModeInfo->OutputInfo.OutputHeight *= (UINT16)SensorNum;
    pModeInfo->OutputInfo.RecordingPixels.Height *= (UINT16)SensorNum;

    /* updated minimum frame rate limitation */
    /*
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
    */
    if(U32RVal != 0U) {
        /* MisraC */
    }
}

#ifdef MX00_AR0233_IN_SLAVE_MODE
static UINT32 MX00_AR0233_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    static AMBA_VIN_MASTER_SYNC_CONFIG_s MX00_AR0233MasterSyncConfig = {
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

    UINT32 RetVal;
    AMBA_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncCfg = &MX00_AR0233MasterSyncConfig;
    const MX00_AR0233_MODE_INFO_s *pModeInfo = &MX00_AR0233ModeInfoList[ModeID];
    DOUBLE PeriodInDb;
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_floor((((DOUBLE)pModeInfo->FrameRate.NumUnitsInTick * ((DOUBLE)MX00_AR0233SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameRate.TimeScale)) + 0.5), &PeriodInDb);
    pMasterSyncCfg->RefClk = MX00_AR0233SensorInfo[ModeID].InputClk;
    pMasterSyncCfg->HSync.Period = (UINT32)PeriodInDb;
    pMasterSyncCfg->HSync.PulseWidth = 8U;
    pMasterSyncCfg->HSync.Polarity   = 1U;
    pMasterSyncCfg->VSync.Period     = 1U;
    pMasterSyncCfg->VSync.PulseWidth = 1000U;
    pMasterSyncCfg->VSync.Polarity   = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(MX00_AR0233MasterSyncChannel[pChan->VinID], pMasterSyncCfg);

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
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_ConfigVin
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
static UINT32 MX00_AR0233_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
    * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX00_AR0233VinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .BayerPattern       = AMBA_SENSOR_BAYER_PATTERN_GR,
            .NumDataBits        = 0,
            .YuvOrder           = 0,
            .NumSkipFrame       = 1U,
            .RxHvSyncCtrl = {
                .NumActivePixels    = 0,
                .NumActiveLines     = 0,
                .NumTotalPixels     = 0,
                .NumTotalLines      = 0,
            },
            .SplitCtrl = {
                .NumSplits          = 0,
                .SplitWidth         = 0,
            },
            .DelayedVsync       = 0,
        },
        .DataTypeMask           = 0x3f,
        .DataType               = 0x2E,     /* Linear: 12bit */
        .NumActiveLanes         = 4U,
        .VirtChanHDREnable      = 0U,
        .VirtChanHDRConfig      = {0},
    };

    UINT32 RetVal;
    const AMBA_SENSOR_OUTPUT_INFO_s  *pOutputInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s *pVinCfg = &MX00_AR0233VinConfig;
    UINT32 U32RVal = 0;

    pVinCfg->Config.NumDataBits = pOutputInfo->NumDataBits;
    pVinCfg->Config.BayerPattern = pOutputInfo->BayerPattern;

    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels  = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines   = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    pVinCfg->NumActiveLanes = pOutputInfo->NumDataLanes;

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

#ifdef SERDES_ENABLE
static void MX00_AR0233_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const MX00_AR0233_SENSOR_INFO_s *pSensorInfo = &MX00_AR0233SensorInfo[SensorMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &pModeInfo->OutputInfo;
    MAX9295_9296_SERDES_CONFIG_s *pSerdesConfig = &MX00_AR0233SerdesConfig[pChan->VinID];
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    pSerdesConfig->EnabledLinkID = MX00_AR0233_GetEnabledLinkID(pChan->SensorID);
    pSerdesConfig->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesConfig->CSITxLaneNum[0] = pOutputInfo->NumDataLanes;
    pSerdesConfig->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    U32RVal |= AmbaWrap_floor((DOUBLE)pOutputInfo->DataRate * 1e-8, &FloorVal);
    pSerdesConfig->CSITxSpeed[0] = (UINT8)FloorVal;

    (void)Max9295_9296_Config(pChan->VinID, &MX00_AR0233SerdesConfig[pChan->VinID]);
    if(U32RVal != 0U) {
        /* MisraC */
    }
}
#endif

static UINT32 MX00_AR0233_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
#ifdef SERDES_ENABLE
    if ((pChan->SensorID & MX00_AR0233_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    if ((pChan->SensorID & MX00_AR0233_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
    if ((pChan->SensorID & MX00_AR0233_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    if ((pChan->SensorID & MX00_AR0233_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start >= 20us*/

    if (RetVal != MAX9295_9296_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }
#else
    RetVal = AmbaUserGPIO_SensorResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    if (RetVal == GPIO_ERR_NONE) {
        (void)AmbaKAL_TaskSleep(2);
        RetVal = AmbaUserGPIO_SensorResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        //RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_113, AMBA_GPIO_LEVEL_HIGH);
    }
    (void)AmbaKAL_TaskSleep(30);
#endif
    return RetVal;
}

static UINT32 MX00_AR0233_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pRxData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8  TxDataBuf[2];
    UINT8  RxDataBuf[2];
    UINT32 TxSize;

    if ((pChan == NULL) ||
        ((pChan->SensorID != MX00_AR0233_SENSOR_ID_CHAN_A) && (pChan->SensorID != MX00_AR0233_SENSOR_ID_CHAN_B))) {
        RetVal = SENSOR_ERR_ARG;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Read SENSOR_ERR_ARG", 0, 0, 0, 0, 0);
    } else {
        I2cTxConfig.DataSize = 2U;
        I2cTxConfig.pDataBuf = TxDataBuf;

        TxDataBuf[0] = (UINT8)(Addr >> 8U);    /* Register Address [15:8] */
        TxDataBuf[1] = (UINT8)(Addr & 0xffU);  /* Register Address [7:0]  */

        I2cRxConfig.DataSize = 2U;
        I2cRxConfig.pDataBuf = RxDataBuf;

        if (pChan->SensorID == MX00_AR0233_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX00_AR0233SerdesConfig[pChan->VinID].SensorAliasID[0];
            I2cRxConfig.SlaveAddr = (MX00_AR0233SerdesConfig[pChan->VinID].SensorAliasID[0] | (UINT32)0x1U);
        } else {
            /* pChan->SensorID == MX00_AR0233_SENSOR_ID_CHAN_B */
            I2cTxConfig.SlaveAddr = MX00_AR0233SerdesConfig[pChan->VinID].SensorAliasID[1];
            I2cRxConfig.SlaveAddr = (MX00_AR0233SerdesConfig[pChan->VinID].SensorAliasID[1] | (UINT32)0x1U);
        }

        RetVal = AmbaI2C_MasterReadAfterWrite(MX00_AR0233I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U,
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
 *  @RoutineName:: MX00_AR0233_RegisterRead
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
static UINT32 MX00_AR0233_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16* Data)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_AR0233_RegRead(pChan, Addr, Data);
    }

    return RetVal;
}

static UINT32 MX00_AR0233_RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[4];
    UINT32 TxSize;
    UINT32 k;
    //UINT16 rData;

    if ((pChan == NULL) ||
        ((pChan->SensorID & (MX00_AR0233_SENSOR_ID_CHAN_A | MX00_AR0233_SENSOR_ID_CHAN_B)) == 0x0U)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.DataSize = 4U;
        I2cConfig.pDataBuf = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        TxDataBuf[2] = (UINT8) ((Data & 0xff00U) >> 8U);
        TxDataBuf[3] = (UINT8) (Data & 0x00ffU);

        if (pChan->SensorID == (MX00_AR0233_SENSOR_ID_CHAN_A | MX00_AR0233_SENSOR_ID_CHAN_B)) {     /* broadcast to all sensors */
            I2cConfig.SlaveAddr = MX00_AR0233SerdesConfig[pChan->VinID].SensorSlaveID;
            RetVal = AmbaI2C_MasterWrite(MX00_AR0233I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                         &I2cConfig, &TxSize, 1000U);
        } else {
            for(k = 0U; k < MX00_AR0233_NUM_MAX_SENSOR_COUNT; k++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (k + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_AR0233SerdesConfig[pChan->VinID].SensorAliasID[k];
                    RetVal = AmbaI2C_MasterWrite(MX00_AR0233I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                                 &I2cConfig, &TxSize, 1000U);
                }
            }
        }

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr:0x%02x, Data:%d, I2C does not work (SlaveID=0x%02x)!!!!!", Addr, Data, I2cConfig.SlaveAddr, 0U, 0U);
        } else {
            RetVal = SENSOR_ERR_NONE;
            /* I2C Debug (1-ch only) */
            //(void) MX00_AR0233_RegRead(pChan, Addr, &rData);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr=0x%04X, [Write]Data=0x%04X, [Read]Data=0x%04x", Addr, Data, rData, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_RegisterWrite
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
static UINT32 MX00_AR0233_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_AR0233_RegWrite(pChan, Addr, Data);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_SoftwareReset
 *
 *  @Description:: Reset (SW) MX00_AR0233 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_AR0233_SoftwareReset(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    /* R0x301A: software reset */
    RetVal = MX00_AR0233_RegWrite(pChan, 0x301A, 0x0058);

    /* Wait for internal initialization */
    (void) AmbaKAL_TaskSleep(200);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_ChangeReadoutMode
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
static UINT32 MX00_AR0233_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
    UINT16 Addr, Data;
    UINT16 RevData = 0U;
    UINT8  WFlag = 0U;
    if ((SensorMode == MX00_AR0233_1920_1080_A30P_MODE6) || (SensorMode == MX00_AR0233_1920_1080_A60P_MODE6)|| (SensorMode == MX00_AR0233_1920_1080_A27P6_M6)|| (SensorMode == MX00_AR0233_1920_1080_A27P6_M6_1) || (SensorMode == MX00_AR0233_1920_1280_A50P_MODE6)) {
        //RetVal |= MX00_AR0233_RegWrite(pChan, 0x301A, 0x0058);  //streaming off
        (void) MX00_AR0233_RegRead(pChan, 0x300E, &RevData);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== R0x300E : 0x%x ====",RevData,0, 0, 0, 0U);
        if(RevData >= 0x202DU) {
            WFlag = 0U;
        } else {
            WFlag = 1U;
        }
        for (i = 0U; i < AR0233_R3M6_NUM_PLL_REG; i++) {
            Addr = AR0233R3M6PLLRegTable[i].Addr;
            //Data = AR0233R3M6PLLRegTable[i].Data[0];
            Data = AR0233R3M6PLLRegTable[i].Data[SensorMode];
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        if(WFlag == 1U) {
            for (i = 0U; i < AR0233_R3M6_NUM_DESIGN_RECOM_REG; i++) {
                Addr = AR0233R3M6DesignRecomRegTable[i].Addr;
                Data = AR0233R3M6DesignRecomRegTable[i].Data;
                RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
            }
        }
        for (i = 0U; i < AR0233_R3M6_NUM_SE_T2_PHASE_REG; i++) {
            Addr = AR0233R3M6SET2PhaseRegTable[i].Addr;
            Data = AR0233R3M6SET2PhaseRegTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_R3M6_NUM_SE_MODE_REG; i++) {
            Addr = AR0233R3M6SEModeRegTable[i].Addr;
            Data = AR0233R3M6SEModeRegTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_R3M6_NUM_SE_HDR_REG; i++) {
            Addr = AR0233R3M6SEHdrRegTable[i].Addr;
            Data = AR0233R3M6SEHdrRegTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        if(WFlag == 1U) {
            for (i = 0U; i < AR0233_R3M6_NUM_PIXEL_CHAR_RECOM_REG; i++) {
                Addr = AR0233R3M6PixelCharReRegTable[i].Addr;
                Data = AR0233R3M6PixelCharReRegTable[i].Data;
                RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
            }
            for (i = 0U; i < AR0233_R3M6_NUM_SEQ_HIDY_REG; i++) {
                Addr = AR0233R3M6SeqHidyRegTable[i].Addr;
                Data = AR0233R3M6SeqHidyRegTable[i].Data;
                RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
            }
        }
        for (i = 0U; i < AR0233_R3M6_NUM_WORKING_MODE_REG; i++) {
            Addr = AR0233R3M6WorkingModeRegTable[i].Addr;
            Data = AR0233R3M6WorkingModeRegTable[i].Data[SensorMode];
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_R3M6_NUM_SEQ_REG; i++) {
            Addr = AR0233R3M6SeqRegTable[i].Addr;
            Data = AR0233R3M6SeqRegTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_R3M6_NUM_LUT24T16; i++) {
            Addr = AR0233R3M6Lut24to16Table[i].Addr;
            Data = AR0233R3M6Lut24to16Table[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_R3M6_NUM_SE_AGAIN_1X; i++) {
            Addr = AR0233R3M6SEAGin1XTable[i].Addr;
            Data = AR0233R3M6SEAGin1XTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_R3M6_NUM_TEMP_INIT_REG; i++) {
            Addr = AR0233R3M6TempInitRegTable[i].Addr;
            Data = AR0233R3M6TempInitRegTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        //RetVal |= MX00_AR0233_RegWrite(pChan, 0x301A, 0x005C);  //streaming on
        //Disabled embedded data
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x3064, 0x0000);
        //
    } else if (SensorMode == MX00_AR0233_2048_1280_A30P) { //Linear Mode
        UINT16 ModeIdx = SensorMode - MX00_AR0233_2048_1280_A30P;
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x3064, 0x0); //Embedded off
        for (i = 0U; i < AR0233_NUM_DESIGN_RECOM_REG; i++) {
            Addr = AR0233R3LinDesignRecomRegTable[i].Addr;
            Data = AR0233R3LinDesignRecomRegTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_NUM_RECOM_LFMHDR_REG; i++) {
            Addr = AR0233LinRecomLFMHDRTable[i].Addr;
            Data = AR0233LinRecomLFMHDRTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_NUM_SEQ_HIDY_REG; i++) {
            Addr = AR0233LinSeqHidyRegTable[i].Addr;
            Data = AR0233LinSeqHidyRegTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_R3M6_NUM_TEMP_INIT_REG; i++) {
            Addr = AR0233R3M6TempInitRegTable[i].Addr;
            Data = AR0233R3M6TempInitRegTable[i].Data;
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x3364, 0x0766);
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x3082, 0x0);     //num_exp = 0
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x30BA, 0x1120);  //Num_exp_max =0
        for (i = 0U; i < AR0233_NUM_PLL_REG; i++) {
            Addr = AR0233LinPLLRegTable[i].Addr;
            Data = AR0233LinPLLRegTable[i].Data[0];
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        for (i = 0U; i < AR0233_NUM_WORKING_MODE_REG; i++) {
            Addr = AR0233LinWorkingModeRegTable[i].Addr;
            Data = AR0233LinWorkingModeRegTable[i].Data[ModeIdx];
            RetVal |= MX00_AR0233_RegWrite(pChan, Addr, Data);
        }
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x3110, 0x0001);    //Pre_hdr_gain_disable
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x31D0, 0x0);       //non-companding
        //Disabled embedded data
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x3064, 0x0000);
        //
        //RetVal |= MX00_AR0233_RegWrite(pChan, 0x301A, 0x005C);  //streaming on
    } else { /* MisraC */ }

    return RetVal;
}

static void MX00_AR0233_PreCalculateModeInfo(void)
{
    UINT32 i;

    for (i = 0U; i < MX00_AR0233_NUM_MODE; i++) {
        MX00_AR0233ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)MX00_AR0233SensorInfo[i].LineLengthPck /
                                             ((DOUBLE)MX00_AR0233SensorInfo[i].DataRate *
                                              (DOUBLE)MX00_AR0233SensorInfo[i].NumDataLanes /
                                              (DOUBLE)MX00_AR0233SensorInfo[i].NumDataBits));
        MX00_AR0233ModeInfoList[i].PixelRate = (FLOAT)((DOUBLE)MX00_AR0233OutputInfo[i].DataRate *
                                               (DOUBLE)MX00_AR0233OutputInfo[i].NumDataLanes /
                                               (DOUBLE)MX00_AR0233OutputInfo[i].NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_Init
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
static UINT32 MX00_AR0233_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* The default vin reference clock frequency */
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, 27000000);
        MX00_AR0233_PreCalculateModeInfo();
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_Enable
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
static UINT32 MX00_AR0233_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX00_AR0233_ResetSensor(pChan);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "MX00_AR0233_Enable", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_Disable
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
static UINT32 MX00_AR0233_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* Set streaming off and reset */
        RetVal = MX00_AR0233_RegWrite(pChan, 0x301A, 0x0058);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_GetStatus
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
static UINT32 MX00_AR0233_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pStatus == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pStatus, &MX00_AR0233Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_GetModeInfo
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
static UINT32 MX00_AR0233_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }
        if (Config.ModeID >= MX00_AR0233_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            MX00_AR0233_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_GetDeviceInfo
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
static UINT32 MX00_AR0233_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &MX00_AR0233DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_GetHdrInfo
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
static UINT32 MX00_AR0233_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pHdrInfo, &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_GetCurrentGainFactor
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
static UINT32 MX00_AR0233_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 CurrentAgcIdx, CurrentDgc;
    UINT32 i, k;

    if ((pChan == NULL) ||(pGainFactor == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    CurrentAgcIdx = MX00_AR0233Ctrl[pChan->VinID].CurrentAgc[i][0];
                    CurrentDgc = MX00_AR0233Ctrl[pChan->VinID].CurrentDgc[i][0];
                    pGainFactor[0] = MX00_AR0233AgcRegTable[CurrentAgcIdx].Factor * ((FLOAT)CurrentDgc / 512.0f);

                    if (MX00_AR0233AgcRegTable[CurrentAgcIdx].ConvGain == MX00_AR0233_HIGH_CONVERSION_GAIN) {
                        pGainFactor[0] *= CG_RATIO;
                    }
                }
            }
        } else {
            /* HDR mode */
            for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k++) {
                        CurrentAgcIdx = MX00_AR0233Ctrl[pChan->VinID].CurrentAgc[i][k];
                        CurrentDgc = MX00_AR0233Ctrl[pChan->VinID].CurrentDgc[i][k];
                        pGainFactor[k] = MX00_AR0233HDRSEAgcRegTable[CurrentAgcIdx].Factor * ((FLOAT)CurrentDgc / 512.0f);
                    }
                }
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_GetCurrentShutterSpeed
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
static UINT32 MX00_AR0233_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if ((pChan == NULL) ||(pExposureTime == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    pExposureTime[0] = (MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.RowTime * (FLOAT)MX00_AR0233Ctrl[pChan->VinID].CurrentShtCtrl[i][0]);
                }
            }
        } else {
            /* HDR mode */
            for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k++) {
                        pExposureTime[k] = (MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.RowTime * (FLOAT)MX00_AR0233Ctrl[pChan->VinID].CurrentShtCtrl[i][k]);
                    }
                }
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_ConvertGainFactor
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
static UINT32 MX00_AR0233_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT  AgcGain, DgcGain;
    FLOAT  MaxGain, MinGain;
    FLOAT  DesiredFactor = pDesiredFactor->Gain[0];
    UINT32 HDRAgcTableIdx, AgcTableIdx;
    FLOAT  TotalAgain = 0.0f;
    UINT32 DgcGainReg = 0U;
    UINT32 SensorMode;// = MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;

    AMBA_SENSOR_WB_GAIN_FACTOR_s WbGain;
    FLOAT MaxWbFactor;
    DOUBLE WbGainInTmp;

    FLOAT FGainTmp;
    UINT16 FGainTmp512;
    UINT32 WBTmp;
    UINT32 U32RVal = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        SensorMode = MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        if(MX00_AR0233HdrInfo[SensorMode].HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) { //Mode6, SE+T2
            MaxGain = MX00_AR0233HDRSEAgcRegTable[0].Factor * (MX00_AR0233DeviceInfo.MaxDigitalGainFactor);
            MinGain = MX00_AR0233HDRSEAgcRegTable[0].Factor;
            if (DesiredFactor < MinGain) {
                DesiredFactor = MinGain;
            } else if (DesiredFactor > MaxGain) {
                DesiredFactor = MaxGain;
            } else {
                /* MisraC */ //(void)DesiredFactor;
            }
            HDRAgcTableIdx = 0U;

            AgcGain = MX00_AR0233HDRSEAgcRegTable[HDRAgcTableIdx].Factor;
            //DgcGain = DesiredFactor / AgcGain;
            /*=============================================================
            The setting of digital gain R0x3308: 2.9 format
            =============================================================*/
            FGainTmp = DesiredFactor * 512.0f / AgcGain;
            FGainTmp512 = (UINT16)FGainTmp;
            DgcGainReg = (UINT32)FGainTmp512;
            DgcGain = (FLOAT)FGainTmp512 / 512.0f;
            /* AnalogGainCtrl consists of Dgain(upper 16bits) and Again(lower 16bits) */
            pGainCtrl->AnalogGain[0] = (DgcGainReg << 16);/*+ HDRAgcTableIdx=0*/
            pGainCtrl->DigitalGain[0] = DgcGainReg;
            pActualFactor->Gain[0] = (AgcGain * DgcGain);
            pGainCtrl->AnalogGain[1] = (DgcGainReg << 16);/*+ HDRAgcTableIdx=0*/
            pGainCtrl->DigitalGain[1] = DgcGainReg;
            pActualFactor->Gain[1] = (AgcGain * DgcGain);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, " AgcGain : %d, DgcGain : %d",(UINT32)(AgcGain*1000.0f), (UINT32)(AgcGain*1000.0f), 0U, 0U, 0U);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, " DesiredFactor : %d",(UINT32)(DesiredFactor*1000.0f), 0U, 0U, 0U, 0U);
            /*AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[ConvertGainFactor]HDRAgcTableIdx=%d,ActualFactor=%d", DesiredFactor*1000000, (AgcGain * DgcGain)*1000000, 0U, 0U, 0U);*/
        } else { //Linear Mode
            MaxGain = MX00_AR0233AgcRegTable[MX00_AR0233_NUM_AGC_STEP - 1U].Factor * CG_RATIO * (MX00_AR0233DeviceInfo.MaxDigitalGainFactor);
            MinGain = MX00_AR0233AgcRegTable[0].Factor;
            if (DesiredFactor < MinGain) {
                DesiredFactor = MinGain;
            } else if (DesiredFactor > MaxGain) {
                DesiredFactor = MaxGain;
            } else {
                /* MisraC */ //(void)DesiredFactor;
            }
            for (AgcTableIdx = 0; AgcTableIdx < (MX00_AR0233_NUM_AGC_STEP - 1U); AgcTableIdx++) {
                if (MX00_AR0233AgcRegTable[AgcTableIdx + 1U].ConvGain == MX00_AR0233_HIGH_CONVERSION_GAIN) {
                    TotalAgain = MX00_AR0233AgcRegTable[AgcTableIdx + 1U].Factor * CG_RATIO;
                } else {
                    TotalAgain = MX00_AR0233AgcRegTable[AgcTableIdx + 1U].Factor;
                }
                if (DesiredFactor < TotalAgain) {
                    break;
                }
            }
            if (MX00_AR0233AgcRegTable[AgcTableIdx].ConvGain == MX00_AR0233_HIGH_CONVERSION_GAIN) {
                AgcGain = MX00_AR0233AgcRegTable[AgcTableIdx].Factor * CG_RATIO;
            } else {
                AgcGain = MX00_AR0233AgcRegTable[AgcTableIdx].Factor;
            }

            /*=============================================================
            The setting of digital gain R0x3308: 2.9 format
            =============================================================*/
            FGainTmp = DesiredFactor * 512.0f / AgcGain;
            FGainTmp512 = (UINT16)FGainTmp;
            DgcGainReg = (UINT32)FGainTmp512;
            DgcGain = (FLOAT)FGainTmp512 / 512.0f;
            /* AnalogGainCtrl consists of Dgain(upper 16bits) and Again(lower 16bits) */
            pGainCtrl->AnalogGain[0] = (DgcGainReg << 16) + AgcTableIdx;
            pGainCtrl->DigitalGain[0] = DgcGainReg;
            pActualFactor->Gain[0] = (AgcGain * DgcGain);
            /*AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "-----------SensorMode:%d------------- ", SensorMode, 0U, 0U, 0U, 0U);*/
        }
        /*====================================================
          The setting of Wb gain: 4.7 format
            The upper 4 bit is the integer part.
            The lower 7 bit is expressed as fractional part.
        ====================================================*/
        MaxWbFactor = (FLOAT)0x7ff / 128.0f;
        /* Bounding check */
        WbGain.R  = (pDesiredFactor->WbGain[0].R >= MaxWbFactor)  ? MaxWbFactor : pDesiredFactor->WbGain[0].R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr >= MaxWbFactor) ? MaxWbFactor : pDesiredFactor->WbGain[0].Gr;
        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb >= MaxWbFactor) ? MaxWbFactor : pDesiredFactor->WbGain[0].Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B >= MaxWbFactor)  ? MaxWbFactor : pDesiredFactor->WbGain[0].B;

        WbGain.R  = (pDesiredFactor->WbGain[0].R <= 1.0f)  ? 1.0f : WbGain.R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr <= 1.0f) ? 1.0f : WbGain.Gr;
        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb <= 1.0f) ? 1.0f : WbGain.Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B <= 1.0f)  ? 1.0f : WbGain.B;

        if((SensorMode == MX00_AR0233_1920_1080_A30P_MODE6)||(SensorMode == MX00_AR0233_1920_1080_A60P_MODE6)||(SensorMode == MX00_AR0233_1920_1080_A27P6_M6)||(SensorMode == MX00_AR0233_1920_1080_A27P6_M6_1)||(SensorMode == MX00_AR0233_1920_1280_A50P_MODE6)) {


            if(WbGain.R >= 2.0f) {
                WbGain.R/= (255.0f/128.0f);
                WBTmp = 0xFFU;
                U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.R * 128.0), &WbGainInTmp);
                pGainCtrl->WbGain[0].R = (WBTmp<<16U) | (UINT32)WbGainInTmp;
            } else {
                WBTmp = 0x80U;
                U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.R * 128.0), &WbGainInTmp);
                pGainCtrl->WbGain[0].R = ((UINT32)WbGainInTmp << 16U) | WBTmp;
            }
            pActualFactor->WbGain[0].R  = ((FLOAT)WbGainInTmp / 128.0f)* ((FLOAT) WBTmp / 128.0f);

            if(WbGain.Gr >= 2.0f) {
                WbGain.Gr/= (255.0f/128.0f);
                WBTmp = 0xFFU;
                U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gr * 128.0), &WbGainInTmp);
                pGainCtrl->WbGain[0].Gr = (WBTmp<<16U) | (UINT32)WbGainInTmp;
            } else {
                WBTmp = 0x80U;
                U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gr * 128.0), &WbGainInTmp);
                pGainCtrl->WbGain[0].Gr = ((UINT32)WbGainInTmp << 16U) | WBTmp;
            }
            pActualFactor->WbGain[0].Gr  = ((FLOAT)WbGainInTmp / 128.0f)* ((FLOAT) WBTmp / 128.0f);

            if(WbGain.Gb >= 2.0f) {
                WbGain.Gb/= (255.0f/128.0f);
                WBTmp = 0xFFU;
                U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gb * 128.0), &WbGainInTmp);
                pGainCtrl->WbGain[0].Gb = (WBTmp<<16U) | (UINT32)WbGainInTmp;
            } else {
                WBTmp = 0x80U;
                U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gb * 128.0), &WbGainInTmp);
                pGainCtrl->WbGain[0].Gb = ((UINT32)WbGainInTmp << 16U) | WBTmp;
            }
            pActualFactor->WbGain[0].Gb  = ((FLOAT)WbGainInTmp / 128.0f)* ((FLOAT) WBTmp / 128.0f);

            if(WbGain.B >= 2.0f) {
                WbGain.B/= (255.0f/128.0f);
                WBTmp = 0xFFU;
                U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.B * 128.0), &WbGainInTmp);
                pGainCtrl->WbGain[0].B = (WBTmp<<16U) | (UINT32)WbGainInTmp;
            } else {
                WBTmp = 0x80U;
                U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.B * 128.0), &WbGainInTmp);
                pGainCtrl->WbGain[0].B = ((UINT32)WbGainInTmp << 16U) | WBTmp;
            }
            pActualFactor->WbGain[0].B  = ((FLOAT)WbGainInTmp / 128.0f)* ((FLOAT) WBTmp / 128.0f);

        } else {
            U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.R * 128.0), &WbGainInTmp);
            pGainCtrl->WbGain[0].R = (UINT32)WbGainInTmp;
            U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gr * 128.0), &WbGainInTmp);
            pGainCtrl->WbGain[0].Gr = (UINT32)WbGainInTmp;
            U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gb * 128.0), &WbGainInTmp);
            pGainCtrl->WbGain[0].Gb = (UINT32)WbGainInTmp;
            U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.B * 128.0), &WbGainInTmp);
            pGainCtrl->WbGain[0].B = (UINT32)WbGainInTmp;
            pActualFactor->WbGain[0].R  = (FLOAT)pGainCtrl->WbGain[0].R  / 128.0f;
            pActualFactor->WbGain[0].Gr = (FLOAT)pGainCtrl->WbGain[0].Gr / 128.0f;
            pActualFactor->WbGain[0].Gb = (FLOAT)pGainCtrl->WbGain[0].Gb / 128.0f;
            pActualFactor->WbGain[0].B  = (FLOAT)pGainCtrl->WbGain[0].B  / 128.0f;
        }

        if(U32RVal != 0U) {
            /* MisraC */
        }
    }

    //AmbaPrint_PrintUInt5("WbGain[0].R = %d, WbGain[0].Gr = %d,WbGain.Gb = %d,WbGain.B = %d", 1000000U*pActualFactor->WbGain[0].R, 1000000U*pActualFactor->WbGain[0].Gb,1000000U*pActualFactor->WbGain[0].Gr ,1000000U*pActualFactor->WbGain[0].B,0U);
    /*AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "WbGain[0].R = %d, WbGain[0].Gr = %d,WbGain.Gb = %d,WbGain.B = %d", 1000000U*pActualFactor->WbGain[0].R, 1000000U*pActualFactor->WbGain[0].Gb,1000000U*pActualFactor->WbGain[0].Gr ,1000000U*pActualFactor->WbGain[0].B, 0U);*/
    /*AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "WbGain[0].R = %d, WbGain[0].Gr = %d,WbGain.Gb = %d,WbGain.B = %d", pDesiredFactor->WbGain[0].R*1000000, pDesiredFactor->WbGain[0].Gb*1000000,pDesiredFactor->WbGain[0].Gr*1000000 ,pDesiredFactor->WbGain[0].B*1000000, 0U);*/

    return RetVal;
}

static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 MinExpLine, MaxExpLine;
    DOUBLE ShutterTimeUnit, FloorVal;
    UINT32 ShutterCtrl;
    UINT32 U32RVal = 0;

    ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * ((DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame);
    U32RVal |= AmbaWrap_floor(((DOUBLE)ExposureTime / ShutterTimeUnit), &FloorVal);
    ShutterCtrl = (UINT32)FloorVal;
    /*AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "FLL = %d, ShutterCtrl = %d", FrameLengthLines, ShutterCtrl, 0U, 0U, 0U);*/

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        MaxExpLine = FrameLengthLines - 4U;
        MinExpLine = 1U;
    } else {
        /* HDR mode */
        MaxExpLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExpLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;

    }

    if (ShutterCtrl > MaxExpLine) {
        ShutterCtrl = MaxExpLine;
    } else if (ShutterCtrl < MinExpLine) {
        ShutterCtrl = MinExpLine;
    } else {
        /* MisraC */  /* Don't need to adjust ShutterCtrl */
    }
    /*AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "NumExposureStepPerFrame = %d, ShutterCtrl = %d,ShutterTimeUnit = %d", NumExposureStepPerFrame, ShutterCtrl, ShutterTimeUnit*1000000, 0U, 0U);*/
    *pShutterCtrl = ShutterCtrl;
    *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;

    if(U32RVal != 0U) {
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_ConvertShutterSpeed
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
static UINT32 MX00_AR0233_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else {
            /* HDR mode */
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i++) {
                ConvertShutterSpeed(pModeInfo, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_SetAnalogGainCtrl
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
static UINT32 MX00_AR0233_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    /*UINT32 HdrType = MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;*/
    UINT16 CoarseAGainData;
    UINT16 FineAGainData;
    /* AnalogGainCtrl consists of Dgain(upper 16bits) and Again(lower 16bits) */
    UINT16 AgcIndex = (UINT16)(pAnalogGainCtrl[0] & 0xffffU);
    UINT16 DGainCtrl = (UINT16)((pAnalogGainCtrl[0] >> 16) & 0xffffU);
    UINT32 i;
    UINT32 SensorMode;// = MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        SensorMode = MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        if ((SensorMode == MX00_AR0233_1920_1080_A60P_MODE6) || (SensorMode == MX00_AR0233_1920_1080_A30P_MODE6) || (SensorMode == MX00_AR0233_1920_1280_A50P_MODE6)) {
            RetVal |= MX00_AR0233_RegWrite(pChan,0x3022, 0x01);
            RetVal |= MX00_AR0233_RegWrite(pChan,0x3308, DGainCtrl);
            RetVal |= MX00_AR0233_RegWrite(pChan,0x3022, 0x00);
            for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_AR0233Ctrl[pChan->VinID].CurrentAgc[i][0] = AgcIndex;
                    MX00_AR0233Ctrl[pChan->VinID].CurrentDgc[i][0] = DGainCtrl;
                }
            }
            /*AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[SetAGainCtrl-SensorID:%d]AgcIndex = %d, CoarseAGainData = 0x%x,FineAGainData = 0x%x, DGainCtrl = 0x%x",pChan->SensorID,  AgcIndex, CoarseAGainData,FineAGainData, DGainCtrl);*/
        } else {
            CoarseAGainData = MX00_AR0233AgcRegTable[AgcIndex].CoarseData;
            FineAGainData = MX00_AR0233AgcRegTable[AgcIndex].FineData;
            FineAGainData &= 0x000fU;  /* Bit[3:0] is for fine gain setting */
            /* Linear mode */
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3022, 0x01);  /* GROUPED_PARAMETER_HOLD */
            if (MX00_AR0233AgcRegTable[AgcIndex].ConvGain == MX00_AR0233_LOW_CONVERSION_GAIN) {
                RetVal |= MX00_AR0233_RegWrite(pChan, 0x3362, 0x0000); /* Enable LCG */
            } else {
                RetVal |= MX00_AR0233_RegWrite(pChan, 0x3362, 0x0001); /* Enable HCG */
            }
            /* Analog gain */
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3366, CoarseAGainData);
            /* Analog gain */
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x336A, FineAGainData);
            /* Digital gain */
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3308, DGainCtrl);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3022, 0x00);  /* GROUPED_PARAMETER_HOLD off */
            for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_AR0233Ctrl[pChan->VinID].CurrentAgc[i][0] = AgcIndex;
                    MX00_AR0233Ctrl[pChan->VinID].CurrentDgc[i][0] = DGainCtrl;
                }
            }
        }
    }
    return RetVal;
}

static UINT32 MX00_AR0233_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
    UINT32 SensorMode;// = MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    //FLOAT TempWBGain = 0.0f;
    //AMBA_SENSOR_WB_CTRL_s WBPixelGain = {0};
    UINT32 R0x37B0,R0x37B6;
    UINT32 R0x3056,R0x3058,R0x305A,R0x305C;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        SensorMode = MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        if((SensorMode == MX00_AR0233_1920_1080_A30P_MODE6)||(SensorMode == MX00_AR0233_1920_1080_A60P_MODE6)||(SensorMode == MX00_AR0233_1920_1080_A27P6_M6)||(SensorMode == MX00_AR0233_1920_1080_A27P6_M6_1)||(SensorMode == MX00_AR0233_1920_1280_A50P_MODE6)) {
            R0x37B0 = (pWbGainCtrl->Gr &0xFFFF0000U) >> 16U;
            R0x3056 = (pWbGainCtrl->Gr &0x0000FFFFU);
            R0x37B6 = (pWbGainCtrl->B &0xFFFF0000U) >> 16U;
            R0x3058 = (pWbGainCtrl->B &0x0000FFFFU);
            R0x37B0 |= (pWbGainCtrl->R &0xFFFF0000U)>> 8U;
            R0x305A = (pWbGainCtrl->R &0x0000FFFFU);
            R0x37B6 |= (pWbGainCtrl->Gb &0xFFFF0000U)>> 8U;
            R0x305C = (pWbGainCtrl->Gb &0x0000FFFFU);

            RetVal |= MX00_AR0233_RegWrite(pChan, 0x37B0, (UINT16)R0x37B0);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x37B6, (UINT16)R0x37B6);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3056, (UINT16)R0x3056);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3058, (UINT16)R0x3058);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x305a, (UINT16)R0x305A);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x305c, (UINT16)R0x305C);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[HDR_WB_GAIN] R0x37B0 0x%4x, R0x37B6 0x%4x", R0x37B0, R0x37B6, 0, 0, 0);

        } else {
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3056, (UINT16)pWbGainCtrl->Gr);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3058, (UINT16)pWbGainCtrl->B);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x305a, (UINT16)pWbGainCtrl->R);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x305c, (UINT16)pWbGainCtrl->Gb);
        }
        for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                MX00_AR0233Ctrl[pChan->VinID].CurrentWbCtrl[i][0] = pWbGainCtrl[0];
            }
        }
    }
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[WB_GAIN]Gr = 0x%04x, B = 0x%04x, R = 0x%04x, Gb = 0x%04x", pWbGainCtrl[0].Gr, pWbGainCtrl[0].B, pWbGainCtrl[0].R, pWbGainCtrl[0].Gb, 0U);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_SetShutterCtrl
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
static UINT32 MX00_AR0233_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Data = (UINT16)pShutterCtrl[0];
    UINT32 i;
    UINT16 Data2 = (UINT16)(pShutterCtrl[0]>>T1_T2_SHIFT_BIT); //Manually control the shutter Ratio
    //UINT32 SensorMode = MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;
        if(pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_AR0233_RegWrite(pChan, 0x3012, Data);

            for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_AR0233Ctrl[pChan->VinID].CurrentShtCtrl[i][0] = Data;
                }
            }
        } else {
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3012, Data);  //T1
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3212, Data2); //T2 : T1 >> T1_T2_SHIFT_BIT

            for (i = 0U; i < MX00_AR0233_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_AR0233Ctrl[pChan->VinID].CurrentShtCtrl[i][0] = Data;
                }
            }
        }
    }
    /*AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "pShutterCtrl:%d, Data:0x%x ", pShutterCtrl[0], Data, 0U, 0U, 0U);*/
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_SetSlowShutterCtrl
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
static UINT32 MX00_AR0233_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    (void)pChan;
    (void)SlowShutterCtrl;

    return SENSOR_ERR_NONE;
}

static void MX00_AR0233_SensorRevCheck(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT16 RevData     = 0U; //R0x300E, Rev2 : 0x2025, Rev3 : 0x202A
    UINT16 R0x3E18Data = 0U; //R0x3E18, Rev2 : 0x0E80, Rev3 :
    UINT16 R0x3364Data = 0U; //R0x3364, Rev2 : 0x068C, Rev3 :

    (void) MX00_AR0233_RegRead(pChan, 0x300E, &RevData);
    (void) MX00_AR0233_RegRead(pChan, 0x3E18, &R0x3E18Data);
    (void) MX00_AR0233_RegRead(pChan, 0x3364, &R0x3364Data);
    if((RevData == 0x202AU) && (R0x3E18Data != 0x0E80U) && (R0x3364Data != 0x068CU)) { //Rev3
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== Rev.3(Rev.2+OTP) sensor, SensorID:0x%x, R0x300E : 0x%x,  R0x3E18 : 0x%x, R0x3364 : 0x%x ====", pChan->SensorID, RevData, R0x3E18Data, R0x3364Data, 0U);
    } else if((RevData == 0x202BU) && (R0x3E18Data != 0x0E80U) && (R0x3364Data != 0x068CU)) { //Rev3
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== Rev.3(Rev.2+OTP) sensor, SensorID:0x%x, R0x300E : 0x%x,  R0x3E18 : 0x%x, R0x3364 : 0x%x ====", pChan->SensorID, RevData, R0x3E18Data, R0x3364Data, 0U);
    } else if((RevData == 0x202DU) && (R0x3E18Data != 0x0E80U) && (R0x3364Data != 0x068CU)) { //Rev3
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== Rev.3(Rev.2+OTP2) sensor, SensorID:0x%x, R0x300E : 0x%x,  R0x3E18 : 0x%x, R0x3364 : 0x%x ====", pChan->SensorID, RevData, R0x3E18Data, R0x3364Data, 0U);
    } else if((RevData == 0x2025U) && (R0x3E18Data == 0x0E80U) && (R0x3364Data == 0x068CU)) { //Rev2
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== Unsupport sensor version, Rev.2 sensor, SensorID:0x%x, R0x300E : 0x%x,  R0x3E18 : 0x%x, R0x3364 : 0x%x ====", pChan->SensorID, RevData, R0x3E18Data, R0x3364Data, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== Unsupport sensor version, Rev.2 sensor, SensorID:0x%x, R0x300E : 0x%x,  R0x3E18 : 0x%x, R0x3364 : 0x%x ====", pChan->SensorID, RevData, R0x3E18Data, R0x3364Data, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== Unsupport sensor version, Rev.2 sensor, SensorID:0x%x, R0x300E : 0x%x,  R0x3E18 : 0x%x, R0x3364 : 0x%x ====", pChan->SensorID, RevData, R0x3E18Data, R0x3364Data, 0U);
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== Unknow sensor version, SensorID:0x%x, R0x300E : 0x%x,  R0x3E18 : 0x%x, R0x3364 : 0x%x ====", pChan->SensorID, RevData, R0x3E18Data, R0x3364Data, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== Unknow sensor version, SensorID:0x%x, R0x300E : 0x%x,  R0x3E18 : 0x%x, R0x3364 : 0x%x ====", pChan->SensorID, RevData, R0x3E18Data, R0x3364Data, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "==== Unknow sensor version, SensorID:0x%x, R0x300E : 0x%x,  R0x3E18 : 0x%x, R0x3364 : 0x%x ====", pChan->SensorID, RevData, R0x3E18Data, R0x3364Data, 0U);
    }

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_AR0233_Config
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
static UINT32 MX00_AR0233_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    AMBA_VIN_MIPI_PAD_CONFIG_s MX00_AR0233PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    const UINT8 LaneShift[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0U,
        [AMBA_VIN_CHANNEL1] = 4U,
        [AMBA_VIN_CHANNEL2] = 6U,
    };

    AMBA_SENSOR_CHANNEL_s TempChan = {
        //.VinID = pChan->VinID,
        .SensorID = 0U,
        .Reserved = {
            [0] = 0U,
            [1] = 0U
        }
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;
    UINT16 SensorMode = (UINT16)pMode->ModeID;

    if ((pChan == NULL) || (SensorMode >= MX00_AR0233_NUM_MODE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TempChan.VinID = pChan->VinID;
        pModeInfo = &MX00_AR0233Ctrl[pChan->VinID].Status.ModeInfo;

#ifdef SERDES_ENABLE
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Add delay time to wait MAX9296/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power off", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(1);

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power on", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);
#endif

        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "============  [ MX00_AR0233 ]  ============", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "======  [ Only Support Rev2+OTP sensor ]  =====", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"=====  init mode: %d, DrvVer. : 0x%x =====", SensorMode, MX_AR0233_DRV_VERSION, 0U, 0U, 0U);

        MX00_AR0233_PrepareModeInfo(pChan, pMode, pModeInfo);

#ifdef MX00_AR0233_IN_SLAVE_MODE
        (void)AmbaVIN_MasterSyncDisable(MX00_AR0233MasterSyncChannel[pChan->VinID]);
#endif
        /* Adjust mipi-phy parameters */
        MX00_AR0233PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        MX00_AR0233PadConfig.EnabledPin = (((UINT32)1U << pModeInfo->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX00_AR0233PadConfig);
#ifdef SERDES_ENABLE

        /* Hardware reset for 9296 */
        /* Set MAX9296A PWDNB high */
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB low", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(1);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB high", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_9296_Init(pChan->VinID, MX00_AR0233_GetEnabledLinkID(pChan->SensorID));

        MX00_AR0233_ConfigSerDes(pChan, SensorMode, pModeInfo);
        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX00_AR0233_SENSOR_ID_CHAN_A) != 0U) {
            if(pModeInfo->InputClk == 27000000U) {
                (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_27M);
            } else {
                (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
            }
        }
        if ((pChan->SensorID & MX00_AR0233_SENSOR_ID_CHAN_B) != 0U) {
            if(pModeInfo->InputClk == 27000000U) {
                (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_27M);
            } else {
                (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
            }
        }
#else
        /* The default vin reference clock frequency */
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
#endif
        RetVal |= MX00_AR0233_ResetSensor(pChan);
        if (pChan->SensorID != (MX00_AR0233_SENSOR_ID_CHAN_A | MX00_AR0233_SENSOR_ID_CHAN_B)) {
            MX00_AR0233_SensorRevCheck(pChan);
        } else {
            /* 2-ch */
            TempChan.SensorID = MX00_AR0233_SENSOR_ID_CHAN_A;
            MX00_AR0233_SensorRevCheck(&TempChan);
            TempChan.SensorID = MX00_AR0233_SENSOR_ID_CHAN_B;
            MX00_AR0233_SensorRevCheck(&TempChan);
        }
        /* Program sensor registers */
        RetVal |= MX00_AR0233_SoftwareReset(pChan);
        RetVal |= MX00_AR0233_ChangeReadoutMode(pChan, SensorMode);
        /*MX00_AR0233_RegWrite(0x31bc, 0x8905);*/  //continuous mode

#ifdef MX00_AR0233_IN_SLAVE_MODE
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x301A, 0x0118); // RESET_REGISTER
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x340A, 0x00F3); // GPIO_CONTROL1
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x340A, 0x00B3); // GPIO_CONTROL1
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x340C, 0x0020); // GPIO_CONTROL2
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x30CE, 0x0120); // GRR_CONTROL1
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x301A, 0x011C); // RESET_REGISTER
        if ((SensorMode == MX00_AR0233_1920_1080_A60P_MODE6) || (SensorMode == MX00_AR0233_1920_1080_A30P_MODE6)|| (SensorMode == MX00_AR0233_1920_1080_A27P6_M6)|| (SensorMode == MX00_AR0233_1920_1080_A27P6_M6_1) || (SensorMode == MX00_AR0233_1920_1280_A50P_MODE6)) {
            (void)AmbaKAL_TaskSleep(70);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3E3E, 0x0042); //DISABLE_TILT_TRANSFER
        }
        RetVal |= MX00_AR0233_ConfigMasterSync(pChan, SensorMode);
#else
        RetVal |= MX00_AR0233_RegWrite(pChan, 0x301A, 0x005C);    //streaming on
///
        if ((SensorMode == MX00_AR0233_1920_1080_A60P_MODE6) || (SensorMode == MX00_AR0233_1920_1080_A30P_MODE6)|| (SensorMode == MX00_AR0233_1920_1080_A27P6_M6)|| (SensorMode == MX00_AR0233_1920_1080_A27P6_M6_1) || (SensorMode == MX00_AR0233_1920_1280_A50P_MODE6)) {
            (void)AmbaKAL_TaskSleep(70);
            RetVal |= MX00_AR0233_RegWrite(pChan, 0x3E3E, 0x0042); //DISABLE_TILT_TRANSFER
        }
//
#endif

        RetVal |= MX00_AR0233_ConfigVin(pChan, pModeInfo);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX00_AR0233Obj = {
    .SensorName             = "AR0233",
    .SerdesName             = "MAX9295_9296",

    .Init                   = MX00_AR0233_Init,
    .Enable                 = MX00_AR0233_Enable,
    .Disable                = MX00_AR0233_Disable,
    .Config                 = MX00_AR0233_Config,
    .GetStatus              = MX00_AR0233_GetStatus,
    .GetModeInfo            = MX00_AR0233_GetModeInfo,
    .GetDeviceInfo          = MX00_AR0233_GetDeviceInfo,
    .GetHdrInfo             = MX00_AR0233_GetHdrInfo,
    .GetCurrentGainFactor   = MX00_AR0233_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX00_AR0233_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX00_AR0233_ConvertGainFactor,
    .ConvertShutterSpeed    = MX00_AR0233_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX00_AR0233_SetAnalogGainCtrl,
    .SetWbGainCtrl          = MX00_AR0233_SetWbGainCtrl,
    //.SetDigitalGainCtrl     = MX00_AR0233_SetDigitalGainCtrl,
    .SetShutterCtrl         = MX00_AR0233_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX00_AR0233_SetSlowShutterCtrl,

    .RegisterRead           = MX00_AR0233_RegisterRead,
    .RegisterWrite          = MX00_AR0233_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};



