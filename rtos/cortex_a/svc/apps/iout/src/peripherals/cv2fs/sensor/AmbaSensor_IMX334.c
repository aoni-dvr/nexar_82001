/**
 *  [Engineering Version]
 *  @file AmbaSensor_IMX334.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Control APIs of SONY IMX334 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX334.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
//#include "bsp.h"

#if 0
static INT32 IMX334_ConfigPost(UINT32 *pVinID);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * IMX334 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX334_CTRL_s IMX334Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_PrepareModeInfo
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
static void IMX334_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &IMX334InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX334OutputInfo[ModeID];
    const IMX334_FRAME_TIMING_s         *pFrameTiming   = &IMX334ModeInfoList[ModeID].FrameTiming;
    UINT32 RefClk                                       = pFrameTiming->InputClk * pFrameTiming->RefClkMul;

    (void)AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->LineLengthPck = (pFrameTiming->NumTickPerXhs * pFrameTiming->NumXhsPerH) *
                               (pOutputInfo->DataRate / RefClk) *
                               (pOutputInfo->NumDataLanes) / (pOutputInfo->NumDataBits);
    pModeInfo->FrameLengthLines = pFrameTiming->NumXhsPerV / pFrameTiming->NumXhsPerH;
    pModeInfo->NumExposureStepPerFrame = pFrameTiming->NumXhsPerV;
    pModeInfo->InputClk = pFrameTiming->InputClk;
    // pModeInfo->RowTime = IMX334ModeInfoList[ModeID].RowTime;

    pModeInfo->RowTime = (FLOAT) ((FLOAT)pFrameTiming->NumTickPerXhs * (FLOAT)pFrameTiming->NumXhsPerH) / (FLOAT)RefClk;
    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(const AMBA_SENSOR_CONFIG_s));
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX334HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_ConfigVin
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
static UINT32 IMX334_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s IMX334VinConfig = {
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
        .DataType = 0x20U,
        .DataTypeMask = 0x1fU,
        .VirtChanHDREnable = 0,
        .VirtChanHDRConfig = {
            .VirtChanNum = 2,
            .VCPattern1stExp = 0,
            .VCPattern2ndExp = 1,
            .VCPattern3rdExp = 2,
            .VCPattern4thExp = 3,

            .Offset2ndExp = 0,
            .Offset3rdExp = 0,
            .Offset4thExp = 0,
        },
    };
    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &IMX334VinConfig;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
        pVinCfg->VirtChanHDREnable = 1;
        pVinCfg->VirtChanHDRConfig.VirtChanNum = pModeInfo->HdrInfo.ActiveChannels;
        pVinCfg->VirtChanHDRConfig.VCPattern1stExp = 0U;
        pVinCfg->VirtChanHDRConfig.VCPattern2ndExp = 1U;
        pVinCfg->VirtChanHDRConfig.Offset2ndExp = pModeInfo->HdrInfo.ChannelInfo[1].EffectiveArea.StartY;
    } else {
        pVinCfg->VirtChanHDREnable = 0;
    }

    (void)AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    return AmbaVIN_MipiConfig(VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_HardwareReset
 *
 *  @Description:: Reset IMX334 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 IMX334_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    (void)VinID;
#if 0  /* to be refined with bsp file */
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, 1U);
#endif
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_087, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(2);
    /* de-assert reset pin */
#if 0  /* to be refined with bsp file */
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, 0U);
#endif
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_087, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(2);

    return RetVal;
}

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
static UINT32 RegWrite(UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[IMX334_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;

    if (Size > IMX334_SENSOR_I2C_MAX_SIZE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = IMX334_SENSOR_I2C_SLAVE_ADDR;
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(AMBA_SENSOR_I2C_CHANNEL, AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        RetVal = AmbaI2C_MasterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_PrintStr5("[IMX334] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }

        for (i = 0U; i < Size; i++) {
            AmbaPrint_PrintUInt5("Addr=0x%04x, Data=0x%02x", Addr + i, pTxData[i], 0, 0, 0);
            AmbaPrint_Flush();
        }

    }

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_RegisterWrite
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
static UINT32 IMX334_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT16 Addr, const UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)Data;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegWrite(Addr, &WData, 1U);
    }

    return RetVal;
}

static UINT32 RegRead(UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = IMX334_SENSOR_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = ((UINT32)IMX334_SENSOR_I2C_SLAVE_ADDR | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;

#if 0  /* to be refined with bsp file */
    RetVal = AmbaI2C_MasterReadAfterWrite(AMBA_SENSOR_I2C_CHANNEL, AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);
#endif
    RetVal = AmbaI2C_MasterReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_PrintStr5("[IMX334] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_RegisterRead
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
static UINT32 IMX334_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(Addr, &RData, 1U);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetStandbyOn(void)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = 0x01U;

    RetVal |= RegWrite(IMX334_STANDBY, &WData, 1U);
    (void)AmbaKAL_TaskSleep(2);
    RetVal |= RegWrite(IMX334_MASTERSTOP, &WData, 1U);

    WData = 0x04U;
    RetVal |= RegWrite(IMX334_RESTART, &WData, 1U);
    WData = 0x00U;
    RetVal |= RegWrite(IMX334_RESTART, &WData, 1U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetStandbyOff(void)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = 0x00U;

    RetVal |= RegWrite(IMX334_STANDBY, &WData, 1U);
    (void)AmbaKAL_TaskSleep(18U);
    RetVal |= RegWrite(IMX334_MASTERSTOP, &WData, 1U);

    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * IMX334Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / IMX334Ctrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[IMX334] IMX334_SetStandbyOff");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      ReadoutMode: Sensor readout mode
 *      SensorMode : Sensor mode
 *      VerticalFlip: Vertical flip flag
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_ChangeReadoutMode(UINT32 ModeID, UINT32 Hrev, UINT32 Vrev)
{
    UINT32 RetVal = SENSOR_ERR_NONE, i;
    UINT8 WData, RData, Index = 0U;

    for (i = 0; i < IMX334_NUM_INIT_REG; i ++) {
        RetVal |= RegWrite(IMX334InitRegTable[i].Addr, &IMX334InitRegTable[i].Data, 1U);
    }

    for (i = 0; i < IMX334_NUM_READOUT_MODE_REG; i ++) {
        RetVal |= RegWrite(IMX334ModeRegTable[i].Addr, &IMX334ModeRegTable[i].Data[ModeID], 1U);
    }

    RetVal |= RegRead(IMX334_WINMODE, &RData, 1);
    if ((RData & 0xfU) == 1U) {
        Index = 1U;
    } else {
        Index = 0U;
    }

    if (Hrev != 0U) {
        WData = 0x1U;
        RetVal |= RegWrite(IMX334_HREVERSE, &WData, 1U);
    }

    if (Vrev != 0U) {
        WData = 0x1U;
        RetVal |= RegWrite(IMX334_VREVERSE, &WData, 1U);
    }

    if (Vrev != 0U) {
        for (i = 0; i < IMX334_NUM_READOUT_DIRECTION_REG; i++) {
            RetVal |= RegWrite(IMX334InvertedVRegTable[Index][i].Addr, &IMX334InvertedVRegTable[Index][i].Data, 1U);
        }
    } else {
        for (i = 0; i < IMX334_NUM_READOUT_DIRECTION_REG; i++) {
            RetVal |= RegWrite(IMX334NormalVRegTable[Index][i].Addr, &IMX334NormalVRegTable[Index][i].Data, 1U);
        }
    }

    IMX334Ctrl.CurrentAgcCtrl[0] = 0xffffffffU;
    IMX334Ctrl.CurrentAgcCtrl[1] = 0xffffffffU;
    IMX334Ctrl.CurrentDgcCtrl[0] = 0xffffffffU;
    IMX334Ctrl.CurrentDgcCtrl[1] = 0xffffffffU;
    IMX334Ctrl.CurrentShrCtrl[0] = 0xffffffffU;
    IMX334Ctrl.CurrentShrCtrl[1] = 0xffffffffU;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX334_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < IMX334_NUM_MODE; i++) {
        IMX334ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX334ModeInfoList[i].FrameTiming.NumTickPerXhs *
                                                (DOUBLE)IMX334ModeInfoList[i].FrameTiming.NumXhsPerH /
                                                (DOUBLE)IMX334ModeInfoList[i].FrameTiming.InputClk);

        //AmbaPrint("row time: %f", IMX334ModeInfoList[i].RowTime);

    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_Init
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
static UINT32 IMX334_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const IMX334_FRAME_TIMING_s *pFrameTime = &IMX334ModeInfoList[0U].FrameTiming;

//    AmbaPrint_PrintStr5("IMX290 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#if 0  /* to be refined with bsp file */
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
#endif
        RetVal = AmbaVIN_SensorClkEnable(AMBA_VIN_SENSOR_CLOCK0, pFrameTime->InputClk);
        IMX334_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_Enable
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
static UINT32 IMX334_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX334_HardwareReset(pChan->VinID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_Disable
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
static UINT32 IMX334_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX334_SetStandbyOn();
    }

    //AmbaPrint("[IMX334] IMX334_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetStatus
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
static UINT32 IMX334_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &IMX334Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetModeInfo
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
static UINT32 IMX334_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = IMX334Ctrl.Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= IMX334_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            IMX334_PrepareModeInfo(pMode, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetDeviceInfo
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
static UINT32 IMX334_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &IMX334DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetHdrInfo
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
static UINT32 IMX334_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pHdrInfo, &IMX334Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetCurrentGainFactor
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
static UINT32 IMX334_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT AnalogGainFactor = 0.0f;
    FLOAT DigitalGainFactor = 0.0f;
    UINT8 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (IMX334Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX334Ctrl.CurrentAgcCtrl[0]);
            DigitalGainFactor = (FLOAT)IMX334Ctrl.CurrentDgcCtrl[0] / 256.0f;
            *pGainFactor = AnalogGainFactor * DigitalGainFactor;

        } else if ((IMX334Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (IMX334Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            for (i = 0U; i < (IMX334Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX334Ctrl.CurrentAgcCtrl[i]);
                DigitalGainFactor = (FLOAT)IMX334Ctrl.CurrentDgcCtrl[i] / 256.0f;
                pGainFactor[i] = AnalogGainFactor * DigitalGainFactor;
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    //AmbaPrint("GainFactor:%f", *pGainFactor);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetCurrentShutterSpeed
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
static UINT32 IMX334_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT8 i;

    UINT32 RetVal = SENSOR_ERR_NONE;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX334Ctrl.CurrentShrCtrl[0];
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX334Ctrl.CurrentShrCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}

static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    if (DesiredFactor < 1.0) { /* 1.0 = IMX334DeviceInfo.MinAnalogGainFactor * IMX334DeviceInfo.MinDigitalGainFactor */
        *pAnalogGainCtrl = 0U;
        *pDigitalGainCtrl = 0x100U;

        *pActualFactor = 1.0f;
    } else {
        FLOAT AnalogGainFactor;
        FLOAT DigitalGainFactor;
        DOUBLE DigitalGainCtrlInDb;
        DOUBLE AnalogGainCtrlInDb;

        DigitalGainFactor = DesiredFactor / IMX334DeviceInfo.MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0) {
            *pDigitalGainCtrl = 0x100U;
            DigitalGainFactor = 1.0f;

            AnalogGainFactor = DesiredFactor;
        } else {
            if (DigitalGainFactor > IMX334DeviceInfo.MaxDigitalGainFactor) {
                DigitalGainFactor = IMX334DeviceInfo.MaxDigitalGainFactor;
            }

            (void)AmbaWrap_floor((DigitalGainFactor * 256.0), &DigitalGainCtrlInDb);
            *pDigitalGainCtrl = (UINT32) DigitalGainCtrlInDb;

            DigitalGainFactor = (FLOAT)*pDigitalGainCtrl / 256.0f;

            AnalogGainFactor = DesiredFactor / DigitalGainFactor;
        }

        (void)AmbaWrap_floor(((1024.0) - (1024.0 / AnalogGainFactor)), &AnalogGainCtrlInDb);
        *pAnalogGainCtrl = (UINT32)AnalogGainCtrlInDb;

        if (*pAnalogGainCtrl > 978U) {
            *pAnalogGainCtrl = 978U; /* Max. 26.951 dB */
        }

        AnalogGainFactor = 1024.0f / (1024.0f - (FLOAT)*pAnalogGainCtrl);

        *pActualFactor = AnalogGainFactor * DigitalGainFactor;
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_ConvertGainFactor
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
static UINT32 IMX334_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertGainFactor(pDesiredFactor->Gain[0], &(pActualFactor->Gain[0]), &(pGainCtrl->AnalogGain[0]), &(pGainCtrl->DigitalGain[0]));
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                ConvertGainFactor(pDesiredFactor->Gain[i], &(pActualFactor->Gain[i]), &(pGainCtrl->AnalogGain[i]), &(pGainCtrl->DigitalGain[i]));
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    //AmbaPrint("AnalogGain:%f", AnalogGain);
    //AmbaPrint("DesiredFactor = %f, AgainReg = 0x%08x, ActualFactor = %d",DesiredFactor, *pAnalogGainCtrl, *pActualFactor);
    return RetVal;
}


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
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 MaxExposureLine, MinExposureLine;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ShutterCtrl, ExposureFrames;
    DOUBLE ShutterCtrlInDb;

    (void)AmbaWrap_floor((DOUBLE)(ExposureTime / ((DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame)), &ShutterCtrlInDb);
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        /* Maximun value of FRM_LENGTH_LINES is 65535d */
        ExposureFrames = ((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U;
        if ((FrameLengthLines * ExposureFrames) > 0xffffU) {
            ShutterCtrl = (0xffffU / FrameLengthLines) * NumExposureStepPerFrame;
        }

        /* Maximun value of COARSE_INTEG_TIME is (FRM_LENGTH_LINES - 22) */
        MaxExposureLine= ((((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - 22U;
        MinExposureLine= 1U;
    } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
               (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
        ShutterCtrl = ((ShutterCtrl % 2U) == 1U) ? (ShutterCtrl - 1U) : (ShutterCtrl);

        MaxExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
    } else {
        /* Dummy value */
        ShutterCtrl = 1;
        MaxExposureLine = 1;
        MinExposureLine = 1;

        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[NG] Wrong HdrType!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    if (ShutterCtrl > MaxExposureLine) {
        *pShutterCtrl = MaxExposureLine;
    } else if (ShutterCtrl < MinExposureLine) {
        *pShutterCtrl = MinExposureLine;
    } else {
        *pShutterCtrl = ShutterCtrl;
    }

    *pActualExptime = (FLOAT)*pShutterCtrl * (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame);
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_ConvertShutterSpeed
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
static UINT32 IMX334_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                ConvertShutterSpeed(pModeInfo, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //AmbaPrint("ExposureTime:%f, RowTime: %f, pShutterCtrl:%d", ExposureTime, pModeInfo->RowTime, *pShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetAnalogGainCtrl
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
static UINT32 IMX334_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    // UINT8 TxData[2U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            // TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            // RetVal = RegWrite(IMX334_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX334Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            // TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            // RetVal |= RegWrite(IMX334_DOL_ANA_GAIN_1ST_MSB_REG, TxData, 2);

            // TxData[0] = (UINT8)((pAnalogGainCtrl[1] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pAnalogGainCtrl[1] & 0x00ffU);
            // RetVal |= RegWrite(IMX334_DOL_ANA_GAIN_2ND_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX334Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
            IMX334Ctrl.CurrentAgcCtrl[1] = pAnalogGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][IMX334][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      DigitalGainCtrl:    Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    // UINT8 TxData[2U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            // TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            // RetVal = RegWrite(IMX334_DIG_GAIN_GR_MSB_REG, TxData, 2U);

            /* Update current AGC control */
            IMX334Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            // TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            // RetVal |= RegWrite(IMX334_DOL_DIG_GAIN_1ST_MSB_REG, TxData, 2U);

            // TxData[0] = (UINT8)((pDigitalGainCtrl[1] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pDigitalGainCtrl[1] & 0x00ffU);
            // RetVal |= RegWrite(IMX334_DOL_DIG_GAIN_2ND_MSB_REG, TxData, 2U);

            /* Update current AGC control */
            IMX334Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
            IMX334Ctrl.CurrentDgcCtrl[1] = pDigitalGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][IMX334][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetShutterCtrl
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
static UINT32 IMX334_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    // UINT8 TxData[2U];

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            // TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            // RetVal = RegWrite(IMX334_COARSE_INTEG_TIME_MSB_REG, TxData, 2U);

            /* Update current shutter control */
            IMX334Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            // TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            // RetVal = RegWrite(IMX334_DOL_CIT_1ST_MSB, TxData, 2U);

            // TxData[0] = (UINT8)((pShutterCtrl[1] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pShutterCtrl[1] & 0x00ffU);
            // RetVal = RegWrite(IMX334_DOL_CIT_2ND_MSB, TxData, 2U);

            /* Update current shutter control */
            IMX334Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
            IMX334Ctrl.CurrentShrCtrl[1] = pShutterCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        //    SENSOR_DBG_SHR("[Vin%d/Sensor%d][IMX334][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
        //                      pChan->VinID, pChan->SensorID,  *pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetSlowShutterCtrl
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
static UINT32 IMX334_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = IMX334Ctrl.Status.ModeInfo.Config.ModeID;
    // UINT32 TargetFrameLengthLines = 0U;
    // UINT8 TxData[2U];

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX334Ctrl.Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        // TargetFrameLengthLines = IMX334ModeInfoList[ModeID].FrameTiming.FrameLengthLines * SlowShutterCtrl;

        // TxData[0] = (UINT8)((TargetFrameLengthLines >> 8U) & 0x00ffU);
        // TxData[1] = (UINT8) (TargetFrameLengthLines & 0x00ffU);
        // RetVal = RegWrite(IMX334_FRM_LENGTH_LINES_MSB_REG, TxData, 2U);


        /* Update frame rate information */
        // IMX334Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        // IMX334Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        IMX334Ctrl.Status.ModeInfo.FrameRate.TimeScale = IMX334ModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d ][IMX334][SHR] SlowShutterCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   SlowShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_Config
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
static UINT32 IMX334_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    static AMBA_VIN_MIPI_PAD_CONFIG_s IMX334PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    UINT8 WData;

    if (ModeID >= IMX334_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_PrintUInt5("[IMX334] Config Mode: %d",ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        IMX334_PrepareModeInfo(pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        IMX334PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        IMX334PadConfig.EnabledPin = ((UINT32)1U << (UINT32)pModeInfo->OutputInfo.NumDataLanes) - 1U;
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &IMX334PadConfig);

        /* set pll */
#if 0  /* to be refined with bsp file */

        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
#endif
        RetVal |= AmbaVIN_SensorClkEnable(AMBA_VIN_SENSOR_CLOCK0, pModeInfo->InputClk);

        (void)AmbaKAL_TaskSleep(3);

        /* reset sensor */
        (void)IMX334_HardwareReset(pChan->VinID);

        /* program sensor */
        (void)IMX334_ChangeReadoutMode(ModeID, 0, 0);

        (void)IMX334_SetStandbyOff();

        WData = 0x01U;
        (void)RegWrite(IMX334_MASTERSTOP, &WData, 1U);
        /* config vin */
        RetVal |= IMX334_ConfigVin(pChan->VinID, pModeInfo);
    }

    return RetVal;
}

static UINT32 IMX334_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    (void)pChan;
    (void)IMX334_SetStandbyOff();

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX334Obj = {
    .SensorName             = "IMX334",
    .SerdesName             = "NA",
    .Init                   = IMX334_Init,
    .Enable                 = IMX334_Enable,
    .Disable                = IMX334_Disable,
    .Config                 = IMX334_Config,
    .GetStatus              = IMX334_GetStatus,
    .GetModeInfo            = IMX334_GetModeInfo,
    .GetDeviceInfo          = IMX334_GetDeviceInfo,
    .GetHdrInfo             = IMX334_GetHdrInfo,
    .GetCurrentGainFactor   = IMX334_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX334_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX334_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX334_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX334_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX334_SetDigitalGainCtrl,
    .SetWbGainCtrl          = NULL,
    .SetShutterCtrl         = IMX334_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX334_SetSlowShutterCtrl,

    .RegisterRead           = IMX334_RegisterRead,
    .RegisterWrite          = IMX334_RegisterWrite,

    .ConfigPost             = IMX334_ConfigPost,
    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
