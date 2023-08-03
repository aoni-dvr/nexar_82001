/**
 *  @file AmbaSensor_MAX96707_9286_AR0144.c
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
 *  @details Control APIs of MAXIM SerDes + AR0144 CMOS sensors with Parallel interface
 *
 */

#include "AmbaVIN.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"

#include "AmbaSensor.h"
#include "AmbaSensor_MAX96707_9286_AR0144.h"
#include "AmbaSbrg_Max96707_9286.h"

#include "bsp.h"

#define MX03_AR0144_IN_SLAVE_MODE

/*-----------------------------------------------------------------------------------------------*\
 * MX03_AR0144 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX03_AR0144_CTRL_s MX03_AR0144Ctrl = {0};

static UINT32 MX03_AR0144I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_I2C_CHANNEL_PIP2,
};

static UINT32 MX03_AR0144_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < MX03_AR0144_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return EnabledLinkID;
}

static UINT32 MX03_AR0144_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < MX03_AR0144_NUM_MAX_SENSOR_COUNT; i ++) {
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
 *  @RoutineName:: MX03_AR0144_PrepareModeInfo
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
static void MX03_AR0144_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 SensorMode = pMode->ModeID;
    const MX03_AR0144_SENSOR_INFO_s *pSensorInfo = &MX03_AR0144SensorInfo[SensorMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &MX03_AR0144OutputInfo[SensorMode];
    const AMBA_SENSOR_INPUT_INFO_s *pInputInfo = &MX03_AR0144InputInfo[SensorMode];
    UINT32 SensorNum = MX03_AR0144_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;

    (void)AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->FrameLengthLines         = pSensorInfo->FrameLengthLines * SensorNum;
    pModeInfo->NumExposureStepPerFrame  = pSensorInfo->FrameLengthLines;
    pModeInfo->RowTime                  = MX03_AR0144ModeInfoList[SensorMode].RowTime / (FLOAT)SensorNum;
    pModeInfo->InputClk                 = pSensorInfo->InputClk;
    (void) AmbaWrap_floor(((DOUBLE)MX03_AR0144ModeInfoList[SensorMode].PixelRate * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal);
    pModeInfo->LineLengthPck            = (UINT32)FloorVal;

    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &MX03_AR0144ModeInfoList[SensorMode].FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &MX03_AR0144ModeInfoList[SensorMode].FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    /* update for Max9286 CSI-2 output */
    //pModeInfo->OutputInfo.DataRate = (UINT32) ((UINT64) pSensorInfo->DataRate * pSensorInfo->NumDataBits);
    pModeInfo->OutputInfo.OutputHeight *= (UINT16)SensorNum;
    pModeInfo->OutputInfo.RecordingPixels.Height *= (UINT16)SensorNum;
    pModeInfo->OutputInfo.NumDataLanes = (UINT8)SensorNum;

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_ConfigVin
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
static UINT32 MX03_AR0144_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX03_AR0144VinConfig = {
        .Config = {
            .FrameRate          = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace         = 0U,
            .BayerPattern       = AMBA_SENSOR_BAYER_PATTERN_GR,
            .YuvOrder           = 0,
            .NumDataBits        = 0,
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
        .DataType               = 0x2C,
        .DataTypeMask           = 0x0,
        .VirtChanHDREnable      = 0U,
        .VirtChanHDRConfig      = {0},
    };

    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s  *pVinCfg = &MX03_AR0144VinConfig;

    pVinCfg->Config.NumDataBits   = pOutputInfo->NumDataBits;
    pVinCfg->NumActiveLanes       = pOutputInfo->NumDataLanes;
    pVinCfg->Config.BayerPattern  = pOutputInfo->BayerPattern;

    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;

    (void)AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    return AmbaVIN_MipiConfig(pChan->VinID, pVinCfg);
}

#ifdef MX03_AR0144_IN_SLAVE_MODE
static UINT32 MX03_AR0144_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    static UINT32 MX03_AR0144MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
        [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
        [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_MSYNC_CHANNEL_PIP2,
    };

    static AMBA_VIN_MASTER_SYNC_CONFIG_s MX03_AR0144MasterSyncConfig = {
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
    AMBA_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncCfg = &MX03_AR0144MasterSyncConfig;
    const MX03_AR0144_MODE_INFO_s *pModeInfo = &MX03_AR0144ModeInfoList[ModeID];
    DOUBLE PeriodInDb;

    (void)AmbaWrap_floor((((DOUBLE)pModeInfo->FrameRate.NumUnitsInTick * ((DOUBLE)MX03_AR0144SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameRate.TimeScale)) + 0.5), &PeriodInDb);
    pMasterSyncCfg->RefClk = MX03_AR0144SensorInfo[ModeID].InputClk;
    pMasterSyncCfg->HSync.Period = (UINT32)PeriodInDb;
    pMasterSyncCfg->HSync.PulseWidth = 8U;
    pMasterSyncCfg->HSync.Polarity   = 1U;
    pMasterSyncCfg->VSync.Period     = 1U;
    pMasterSyncCfg->VSync.PulseWidth = 1000U;
    pMasterSyncCfg->VSync.Polarity   = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(MX03_AR0144MasterSyncChannel[pChan->VinID], pMasterSyncCfg);

    if (RetVal == VIN_ERR_NONE) {
        RetVal = SENSOR_ERR_NONE;
    } else {
        RetVal = SENSOR_ERR_INVALID_API;
    }

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_HardwareReset
 *
 *  @Description:: Reset MX03_AR0144 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX03_AR0144_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= AmbaUserGPIO_SerdesResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);
    (void) AmbaKAL_TaskSleep(2);

    RetVal |= AmbaUserGPIO_SerdesResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
    (void) AmbaKAL_TaskSleep(10);

    return RetVal;
}

static UINT32 MX03_AR0144_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};

    UINT8 TxData[2];
    UINT8 RxData[2];
    UINT32 TxSize;
    UINT32 i;
    UINT32 SensorID = pChan->SensorID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0; i < 4U; i++) {
            if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                break;
            }
        }

        I2cTxConfig.SlaveAddr = MX03_AR0144_I2C_ADDR + ((i + 1U) << 1U);
        I2cTxConfig.DataSize  = 2U;
        I2cTxConfig.pDataBuf  = TxData;
        TxData[0] = (UINT8)((Addr & 0xff00U) >> 8U);
        TxData[1] = (UINT8)(Addr & 0x00ffU);

        I2cRxConfig.SlaveAddr = (MX03_AR0144_I2C_ADDR | 1U) + ((i + 1U) << 1U);
        I2cRxConfig.DataSize  = 2U;
        I2cRxConfig.pDataBuf  = RxData;

        RetVal = AmbaI2C_MasterReadAfterWrite(MX03_AR0144I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD, 1U,
                                              &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);

        *pRxData = (UINT16)RxData[0] << 8U;
        *pRxData |= (UINT16)RxData[1];
    }

    return RetVal;
}

static UINT32 MX03_AR0144_RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};

    UINT8 TxDataBuf[4];
    UINT32 TxSize;
    UINT32 i;
    UINT32 SensorID = pChan->SensorID;

    I2cConfig.SlaveAddr = MX03_AR0144_I2C_ADDR;
    I2cConfig.DataSize  = 4U;
    I2cConfig.pDataBuf  = TxDataBuf;

    TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8U);
    TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);
    TxDataBuf[2] = (UINT8)((Data & 0xff00U) >> 8U);
    TxDataBuf[3] = (UINT8)(Data & 0x00ffU);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0; i < MX03_AR0144_NUM_MAX_SENSOR_COUNT; i++) {
            if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                I2cConfig.SlaveAddr = MX03_AR0144_I2C_ADDR + ((i + 1U) << 1U);
                RetVal |= AmbaI2C_MasterWrite(MX03_AR0144I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD, &I2cConfig, &TxSize, 1000U);

                if (RetVal != I2C_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work (SlaveID=0x%02x)!!!!!", MX03_AR0144_I2C_ADDR + ((i + 1U) << 1U), 0, 0, 0, 0);
                }

                //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX03_AR0144][IO] SensorID = 0x%x, Addr = 0x%04X, Data = 0x%04X", SensorID, Addr, Data, 0U, 0U);
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_RegisterRead
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
static UINT32 MX03_AR0144_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{
    UINT32 RetVal;
    UINT16 RxData = 0xffffU;

    RetVal = MX03_AR0144_RegRead(pChan, Addr, &RxData);
    *pData = RxData;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_RegisterWrite
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
static UINT32 MX03_AR0144_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal;

    RetVal = MX03_AR0144_RegWrite(pChan, Addr, Data);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_SoftwareReset
 *
 *  @Description:: Reset MX03_AR0144 Image Sensor Device
 *
 *  @Input      ::
 *      SensorID:         Sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX03_AR0144_SoftwareReset(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    RetVal = MX03_AR0144_RegWrite(pChan, 0x301A, 0x30D8);
    (void) AmbaKAL_TaskSleep(200);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_SetAnalogGainReg
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
static UINT32 MX03_AR0144_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 AGain, DGain;
    UINT16 index = (UINT16)(AnalogGainCtrl & 0x0000ffffU);

    AGain = MX03_AR0144AgcRegTable[index].Data;
    DGain = (UINT16)((AnalogGainCtrl & 0xffff0000U) >> 16U);

    RetVal |= MX03_AR0144_RegWrite(pChan, 0x3022, 0x01);  /* GROUPED_PARAMETER_HOLD */
    RetVal |= MX03_AR0144_RegWrite(pChan, 0x3060, AGain); /* ANALOG_GAIN */
    RetVal |= MX03_AR0144_RegWrite(pChan, 0x305E, DGain); /* DGain */
    RetVal |= MX03_AR0144_RegWrite(pChan, 0x3022, 0x00);  /* GROUPED_PARAMETER_HOLD off*/

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_SetShutterReg
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
static UINT32 MX03_AR0144_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ShutterCtrl)
{
    UINT32 RetVal;
    UINT16 Data = (UINT16)ShutterCtrl;

    RetVal = MX03_AR0144_RegWrite(pChan, 0x3012, Data);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_SetSlowShutterReg
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
static UINT32 MX03_AR0144_SetSlowShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    (void)pChan;
    (void)SlowShutterCtrl;
#if 0
    if (SlowShutterCtrl < 1U) {
        SlowShutterCtrlVal = 1U;
    }

    if(SlowShutterCtrl > 1U) {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[NG] Force to exposure within one frame", NULL, NULL, NULL, NULL, NULL);
        SlowShutterCtrlVal = 1U;
    }

    //Force to exposure within one frame
    TargetFrameLengthLines = MX03_AR0144Ctrl.FrameTime.FrameLengthLines; // * SlowShutterCtrl;
    Data = (UINT16)TargetFrameLengthLines;

    if(TargetFrameLengthLines > 0xffff) {
        Data = MX03_AR0144Ctrl.FrameTime.FrameLengthLines * (0xffff/MX03_AR0144Ctrl.FrameTime.FrameLengthLines);
    } else {
        Data = TargetFrameLengthLines;
    }
    (void) MX03_AR0144_RegWrite(pChan, 0x300A, Data);

    /* Update frame rate information */
    MX03_AR0144Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    MX03_AR0144Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    MX03_AR0144Ctrl.Status.ModeInfo.FrameRate.TimeScale = MX03_AR0144Ctrl.FrameTime.FrameRate.TimeScale / SlowShutterCtrlVal;
#endif

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_StandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      ::
 *      SensorID:       Sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX03_AR0144_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    RetVal = MX03_AR0144_RegWrite(pChan, 0x301A, 0x30D8);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_ChangeReadoutMode
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
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX03_AR0144_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SensorMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
    UINT16 Addr, Data;

    for (i = 0U; i < MX03_AR0144_NUM_RECOM_REG; i++) {
        Addr = MX03_AR0144RecomRegTable[i].Addr;
        Data = MX03_AR0144RecomRegTable[i].Data;
        RetVal |= MX03_AR0144_RegWrite(pChan, Addr, Data);
    }
    for (i = 0U; i < MX03_AR0144_NUM_PLL_REG; i++) {
        Addr = MX03_AR0144PLLRegTable[i].Addr;
        Data = MX03_AR0144PLLRegTable[i].Data;
        RetVal |= MX03_AR0144_RegWrite(pChan, Addr, Data);
    }
    for (i = 0U; i < MX03_AR0144_NUM_READOUT_MODE_REG; i++) {
        Addr = MX03_AR0144ModeRegTable[i].Addr;
        Data = MX03_AR0144ModeRegTable[i].Data[SensorMode];
        RetVal |= MX03_AR0144_RegWrite(pChan, Addr, Data);
    }

    /* disable embedded data (FAE suggests no need to enable it.) */
    RetVal |= MX03_AR0144_RegWrite(pChan, 0x3064, 0x1802);
    /* row speed */
    RetVal |= MX03_AR0144_RegWrite(pChan, 0x3028, 0x0010);
    /* Turn on IR-LED and auto tuned by exposure time */
    RetVal |= MX03_AR0144_RegWrite(pChan, 0x3270, 0x0100);
    RetVal |= MX03_AR0144_RegWrite(pChan, 0x3100, 0x0001);
    RetVal |= MX03_AR0144_RegWrite(pChan, 0x3102, 0x8000);
    RetVal |= MX03_AR0144_RegWrite(pChan, 0x311c, 0x0050);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX03_AR0144_PreCalculateModeInfo(void)
{
    UINT32 i;

    for (i = 0U; i < MX03_AR0144_NUM_MODE; i++) {
        MX03_AR0144ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)MX03_AR0144SensorInfo[i].LineLengthPck /
                                             ((DOUBLE)MX03_AR0144SensorInfo[i].DataRate
                                              * (DOUBLE)MX03_AR0144SensorInfo[i].NumDataLanes
                                              / (DOUBLE)MX03_AR0144SensorInfo[i].NumDataBits));
        MX03_AR0144ModeInfoList[i].PixelRate = (FLOAT)((DOUBLE)MX03_AR0144OutputInfo[i].DataRate *
                                               (DOUBLE)MX03_AR0144OutputInfo[i].NumDataLanes /
                                               (DOUBLE)MX03_AR0144OutputInfo[i].NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_Init
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
static UINT32 MX03_AR0144_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* Note: Although Max9286 has internal oscillator providing the clock for configuration link, we still
         * need to set this clock for gclk_so_vin, so vin can have correct ref. clock to receive mipi data */
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, MX03_AR0144SensorInfo[0].InputClk); /* The default sensor input clock frequency */
        MX03_AR0144Ctrl.EnabledSensorID = pChan->SensorID;
        MX03_AR0144_PreCalculateModeInfo();
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_Enable
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
static UINT32 MX03_AR0144_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    (void)pChan;

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_Disable
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
static UINT32 MX03_AR0144_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX03_AR0144_StandbyOn(pChan);
    }

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "MX03_AR0144_Disable", NULL, NULL, NULL, NULL, NULL);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_GetStatus
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
static UINT32 MX03_AR0144_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &MX03_AR0144Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_GetModeInfo
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
static UINT32 MX03_AR0144_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = MX03_AR0144Ctrl.Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if (Config.ModeID >= MX03_AR0144_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            MX03_AR0144_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_GetDeviceInfo
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
static UINT32 MX03_AR0144_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &MX03_AR0144DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_GetCurrentGainFactor
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
static UINT32 MX03_AR0144_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 AgcIdx;
    FLOAT AGain;
    FLOAT DGain;
    UINT32 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0U; i < MX03_AR0144_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                AgcIdx = MX03_AR0144Ctrl.CurrentAEInfo.CurrentAgcCtrl[i];
                AGain = MX03_AR0144AgcRegTable[AgcIdx].Factor;
                DGain = (FLOAT)(MX03_AR0144Ctrl.CurrentAEInfo.CurrentDgcCtrl[i]) / 128.0f;
                pGainFactor[0] = AGain * DGain;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_GetCurrentShutterSpeed
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
static UINT32 MX03_AR0144_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0U; i < MX03_AR0144_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                pExposureTime[0] = (FLOAT)MX03_AR0144Ctrl.CurrentAEInfo.CurrentShutterCtrl[i] * MX03_AR0144Ctrl.Status.ModeInfo.RowTime;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_ConvertGainFactor
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
static UINT32 MX03_AR0144_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 AgcTableIdx;
    FLOAT  TotalAgain;
    FLOAT  DesiredFactor = pDesiredFactor->Gain[0];
    FLOAT  MinAgcGain = MX03_AR0144AgcRegTable[0].Factor;
    FLOAT  MaxAgcGain = MX03_AR0144AgcRegTable[MX03_AR0144_NUM_AGC_STEP - 1U].Factor * MX03_AR0144DeviceInfo.MaxDigitalGainFactor;
    UINT32 MinDgcGainCtrl = 128U;
    UINT32 MaxDgcGainCtrl;
    DOUBLE AGain;
    DOUBLE DGain;
    UINT32 DgcGainCtrl;
    DOUBLE DgcGainCtrlInDb;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (DesiredFactor < MinAgcGain) {
            DesiredFactor = MinAgcGain;
        } else if (DesiredFactor > MaxAgcGain) {
            DesiredFactor = MaxAgcGain;
        } else {
            (void)DesiredFactor;
        }

        for (AgcTableIdx = 0U; AgcTableIdx < (MX03_AR0144_NUM_AGC_STEP - 1U); AgcTableIdx ++) {
            TotalAgain = MX03_AR0144AgcRegTable[AgcTableIdx + 1U].Factor;
            if(DesiredFactor < TotalAgain) {
                break;
            }
        }

        AGain = (DOUBLE)MX03_AR0144AgcRegTable[AgcTableIdx].Factor;
        (void) AmbaWrap_floor((DOUBLE)MX03_AR0144DeviceInfo.MaxDigitalGainFactor * 128.0, &DgcGainCtrlInDb);
        MaxDgcGainCtrl = (UINT32)DgcGainCtrlInDb;

        if (AGain < 4.0) {
            /* DGain fixed to 1.14x when AGain < 4x */
            DGain = 1.14;
        } else {
            DGain = (DOUBLE)DesiredFactor / AGain;
        }

        (void) AmbaWrap_floor(DGain * 128.0, &DgcGainCtrlInDb);
        DgcGainCtrl = (UINT32)DgcGainCtrlInDb;
        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AgcTableIdx=%d, AGain=%d, DgcGainCtrl=%d DGAIN=%d", AgcTableIdx, AGain, DgcGainCtrl, DGain, 0U);

        if (DgcGainCtrl < MinDgcGainCtrl) {
            DgcGainCtrl = MinDgcGainCtrl;
        } else if (DgcGainCtrl > MaxDgcGainCtrl) {
            DgcGainCtrl = MinDgcGainCtrl;
        } else {
            (void)DgcGainCtrl;
        }

        pGainCtrl->AnalogGain[0] = ((UINT32)DgcGainCtrl << 16) + AgcTableIdx; //[31:16] DgcGain + AgcTableIdx
        pActualFactor->Gain[0] = (FLOAT)(AGain * ((DOUBLE)DgcGainCtrl / 128.0));

        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AnalogGaincrtl = 0x%08x, Gain = %d, DgcGainCtrl = %d, ", pGainCtrl->AnalogGain[0], pActualFactor->Gain[0], DgcGainCtrl, 0U, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_ConvertShutterSpeed
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
static UINT32 MX03_AR0144_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX03_AR0144Ctrl.Status.ModeInfo;
    UINT32 FrameLengthLines = MX03_AR0144Ctrl.Status.ModeInfo.FrameLengthLines;
    UINT32 NumExpStepPerFrame = MX03_AR0144Ctrl.Status.ModeInfo.NumExposureStepPerFrame;
    UINT32 MaxShrWidth;
    UINT32 MinShrWidth;
    DOUBLE ShutterTimeUnit, FloorVal;
    UINT32 ShutterCtrl;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * ((DOUBLE)FrameLengthLines / (DOUBLE)NumExpStepPerFrame);
        (void) AmbaWrap_floor((DOUBLE)pDesiredExposureTime[0] / ShutterTimeUnit, &FloorVal);
        ShutterCtrl = (UINT32)FloorVal;

        /* Exposure lines needs to be smaller than VTS - 5 */
        MaxShrWidth = pModeInfo->NumExposureStepPerFrame - 5U;
        /* Exposure lines needs to be bigger 1 */
        MinShrWidth = 1U;

        if (ShutterCtrl > MaxShrWidth) {
            ShutterCtrl = MaxShrWidth;
        } else if (ShutterCtrl < MinShrWidth) {
            ShutterCtrl = MinShrWidth;
        } else {
            (void)ShutterCtrl;
        }

        pShutterCtrl[0] = ShutterCtrl;
        pActualExposureTime[0] = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_SetAnalogGainCtrl
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
static UINT32 MX03_AR0144_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 AgcIndex;
    UINT32 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX03_AR0144_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

        /* Update current gain control */
        for (i = 0U; i < MX03_AR0144_NUM_MAX_SENSOR_COUNT; i++) {
            AgcIndex = pAnalogGainCtrl[0] & 0x0000ffffU;
            MX03_AR0144Ctrl.CurrentAEInfo.CurrentAgcCtrl[i] = AgcIndex;
            MX03_AR0144Ctrl.CurrentAEInfo.CurrentDgcCtrl[i] = (pAnalogGainCtrl[0] & 0xffff0000U) >> 16U;
        }
    }

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_SetShutterCtrl
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
static UINT32 MX03_AR0144_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal;
    UINT32 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX03_AR0144_SetShutterReg(pChan, pShutterCtrl[0]);

        /* Update current shutter control */
        for (i = 0U; i < MX03_AR0144_NUM_MAX_SENSOR_COUNT; i++) {
            MX03_AR0144Ctrl.CurrentAEInfo.CurrentShutterCtrl[i] = pShutterCtrl[0];
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_SetSlowShutterCtrl
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
static UINT32 MX03_AR0144_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX03_AR0144_SetSlowShutterReg(pChan, SlowShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX03_AR0144_Config
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
static UINT32 MX03_AR0144_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SensorMode = pMode->ModeID;
    UINT32 FramePeriod;
    UINT32 EnabledLinkID = MX03_AR0144_GetEnabledLinkID(pChan->SensorID);
    DOUBLE VsHighWidthInDb;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX03_AR0144Ctrl.Status.ModeInfo;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &MX03_AR0144OutputInfo[SensorMode];
    MX03_AR0144_FRAME_TIMING_s *pFrameTime = &MX03_AR0144Ctrl.FrameTime;
    MAX96707_9286_SERDES_CONFIG_s SerdesConfig = {0};
    MAX96707_9286_RETIMING_s RetimingConfig = {0};

    UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0x30U,  /* SD_LVDS_4~5 */
        [AMBA_VIN_CHANNEL2] = 0xc0U,  /* SD_LVDS_6~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s MX03_AR0144PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    if ((pChan == NULL) || (SensorMode >= MX03_AR0144_NUM_MODE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX96707_9286_AR0144]SensorID:0x%x, SensorMode:%d", pChan->SensorID, SensorMode, 0U, 0U, 0U);
        MX03_AR0144_PrepareModeInfo(pChan, pMode, pModeInfo);

        pFrameTime->InputClk = MX03_AR0144SensorInfo[SensorMode].InputClk;
        pFrameTime->LineLengthPck = MX03_AR0144SensorInfo[SensorMode].LineLengthPck;
        pFrameTime->FrameLengthLines = MX03_AR0144SensorInfo[SensorMode].FrameLengthLines;
        (void)AmbaWrap_memcpy(&pFrameTime->FrameRate, &MX03_AR0144ModeInfoList[SensorMode].FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

        MX03_AR0144PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX03_AR0144PadConfig);

        /* Reset Max9286 via GPIO_reset */
        RetVal |= MX03_AR0144_HardwareReset(pChan->VinID);

        /* Maxim serdes config  */
        SerdesConfig.EnabledLinkID = EnabledLinkID;
        SerdesConfig.SensorSlaveID = MX03_AR0144_I2C_ADDR;
        SerdesConfig.NumDataBits = pOutputInfo->NumDataBits;
        SerdesConfig.PixelRate = MX03_AR0144SensorInfo[SensorMode].DataRate;
        SerdesConfig.FrameSyncPeriod = 0U; /* Use external framesync */
        SerdesConfig.VSyncPolarity = MAX96707_9286_ACTIVE_HIGH;
        SerdesConfig.HSyncPolarity = MAX96707_9286_ACTIVE_HIGH;

        RetVal |= Max96707_9286_Config(pChan->VinID, &SerdesConfig);

        /* Enable VSync retiming to meet MAX9286 MIPI timing */
        (void)AmbaWrap_floor((DOUBLE)MX03_AR0144SensorInfo[SensorMode].OutputHeight * (DOUBLE)pFrameTime->LineLengthPck * 1.05, &VsHighWidthInDb); /* Avoid margin that cause short line/frame */
        RetimingConfig.VsHighWidth = (UINT32)VsHighWidthInDb;
        RetimingConfig.VsLowWidth = (pFrameTime->FrameLengthLines * pFrameTime->LineLengthPck) - RetimingConfig.VsHighWidth;
        RetimingConfig.VsDelay = (pFrameTime->FrameLengthLines * pFrameTime->LineLengthPck) - MIPI_VS_DELAY_PCLK; /* Delay 200 PCLK backward */

        /* Enable crossbar switch to reverse data pin input */
        if ((pChan->SensorID & MX03_AR0144_SENSOR_ID_CHAN_0) != 0U) {
            RetVal |= Max96707_9286_EnableVsRetiming(pChan->VinID, MAX96707_LINK0_ID, &RetimingConfig);
            RetVal |= Max96707_9286_ReverseDataPin(pChan->VinID, MAX96707_LINK0_ID, pOutputInfo->NumDataBits);
        }
        if ((pChan->SensorID & MX03_AR0144_SENSOR_ID_CHAN_1) != 0U) {
            RetVal |= Max96707_9286_EnableVsRetiming(pChan->VinID, MAX96707_LINK1_ID, &RetimingConfig);
            RetVal |= Max96707_9286_ReverseDataPin(pChan->VinID, MAX96707_LINK1_ID, pOutputInfo->NumDataBits);
        }
        if ((pChan->SensorID & MX03_AR0144_SENSOR_ID_CHAN_2) != 0U) {
            RetVal |= Max96707_9286_EnableVsRetiming(pChan->VinID, MAX96707_LINK2_ID, &RetimingConfig);
            RetVal |= Max96707_9286_ReverseDataPin(pChan->VinID, MAX96707_LINK2_ID, pOutputInfo->NumDataBits);
        }
        if ((pChan->SensorID & MX03_AR0144_SENSOR_ID_CHAN_3) != 0U) {
            RetVal |= Max96707_9286_EnableVsRetiming(pChan->VinID, MAX96707_LINK3_ID, &RetimingConfig);
            RetVal |= Max96707_9286_ReverseDataPin(pChan->VinID, MAX96707_LINK3_ID, pOutputInfo->NumDataBits);
        }

        /* Reset all sensors and delay 200ms */
        RetVal |= MX03_AR0144_SoftwareReset(pChan);
        /* Program sensor registers */
        RetVal |= MX03_AR0144_ChangeReadoutMode(pChan, SensorMode);

        /* streaming on */
#ifdef MX03_AR0144_IN_SLAVE_MODE
        RetVal |= MX03_AR0144_RegWrite(pChan, 0x30CE, 0x0120); //SURROUND VIEW
        RetVal |= MX03_AR0144_RegWrite(pChan, 0x30B0, 0x0138); //DIGITAL_TEST (PIXCLK_ON)
        RetVal |= MX03_AR0144_RegWrite(pChan, 0x301A, 0x39DC); //slave mode

        RetVal |= MX03_AR0144_ConfigMasterSync(pChan, SensorMode);
#else
        RetVal |= MX03_AR0144_RegWrite(pChan, 0x301A, 0x30DC);
#endif

        FramePeriod = (UINT32)((pFrameTime->FrameRate.NumUnitsInTick * 1000U) /
                               pFrameTime->FrameRate.TimeScale) + 1U;
        /* ===== Max96707_9286_EnableCSIOutput ===== */
        RetVal |= Max96707_9286_EnableCSIOutput(pChan->VinID, FramePeriod);
        (void) AmbaKAL_TaskSleep(34);

        /* config vin main config */
        RetVal |= MX03_AR0144_ConfigVin(pChan, pModeInfo);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX03_AR0144Obj = {
    .SensorName             = "AR0144",
    .SerdesName             = "MAX96707_9286",
    .Init                   = MX03_AR0144_Init,
    .Enable                 = MX03_AR0144_Enable,
    .Disable                = MX03_AR0144_Disable,
    .Config                 = MX03_AR0144_Config,
    .GetStatus              = MX03_AR0144_GetStatus,
    .GetModeInfo            = MX03_AR0144_GetModeInfo,
    .GetDeviceInfo          = MX03_AR0144_GetDeviceInfo,
    .GetCurrentGainFactor   = MX03_AR0144_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX03_AR0144_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX03_AR0144_ConvertGainFactor,
    .ConvertShutterSpeed    = MX03_AR0144_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX03_AR0144_SetAnalogGainCtrl,
    .SetShutterCtrl         = MX03_AR0144_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX03_AR0144_SetSlowShutterCtrl,

    .RegisterRead           = MX03_AR0144_RegisterRead,
    .RegisterWrite          = MX03_AR0144_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
