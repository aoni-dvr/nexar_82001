/**
 *  @file AmbaSensor_IMX577.c
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
 *  @details Control APIs of SONY IMX577 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX577.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "bsp.h"

#ifdef CONFIG_IMX577_POSTCFG_ENABLED
static UINT32 IMX577_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * IMX577 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX577_CTRL_s IMX577Ctrl = {0};

static UINT32 IMX577I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_I2C_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_I2C_CHANNEL_VIN2,
    [AMBA_VIN_CHANNEL3]  = AMBA_SENSOR_I2C_CHANNEL_VIN3,
    [AMBA_VIN_CHANNEL4]  = AMBA_SENSOR_I2C_CHANNEL_VIN4,
    [AMBA_VIN_CHANNEL5]  = AMBA_SENSOR_I2C_CHANNEL_VIN5,
    [AMBA_VIN_CHANNEL6]  = AMBA_SENSOR_I2C_CHANNEL_VIN6,
    [AMBA_VIN_CHANNEL7]  = AMBA_SENSOR_I2C_CHANNEL_VIN7,
    [AMBA_VIN_CHANNEL8]  = AMBA_SENSOR_I2C_CHANNEL_VIN8,
    [AMBA_VIN_CHANNEL9]  = AMBA_SENSOR_I2C_CHANNEL_VIN9,
    [AMBA_VIN_CHANNEL10] = AMBA_SENSOR_I2C_CHANNEL_VIN10,
    [AMBA_VIN_CHANNEL11] = AMBA_SENSOR_I2C_CHANNEL_VIN11,
    [AMBA_VIN_CHANNEL12] = AMBA_SENSOR_I2C_CHANNEL_VIN12,
    [AMBA_VIN_CHANNEL13] = AMBA_SENSOR_I2C_CHANNEL_VIN13,
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_PrepareModeInfo
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
static void IMX577_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &IMX577InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX577OutputInfo[ModeID];
    const IMX577_FRAME_TIMING_s         *pFrameTiming   = &IMX577ModeInfoList[ModeID].FrameTiming;
    UINT32 U32RVal = 0U;

    U32RVal |= AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->LineLengthPck            = pFrameTiming->LineLengthPck;
    pModeInfo->FrameLengthLines         = pFrameTiming->FrameLengthLines;
    if (IMX577HdrInfo[pMode->ModeID].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
        pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines / 2U;
    } else {
        pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines;
    }
    pModeInfo->InputClk                 = pFrameTiming->InputClk;
    pModeInfo->RowTime                  = IMX577ModeInfoList[ModeID].RowTime;
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX577HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_ConfigVin
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
static UINT32 IMX577_ConfigVin(const UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s IMX577VinConfig = {
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
    //-----------------------------------------------------------------------------------------------//

    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &IMX577VinConfig;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;


    if (AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        /* misrac */
    }

    return AmbaVIN_MipiConfig(VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_HardwareReset
 *
 *  @Description:: Reset IMX577 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 IMX577_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);

    if (RetVal == GPIO_ERR_NONE) {
        if (AmbaKAL_TaskSleep(2) == ERR_NONE) {
            RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

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
static UINT32 RegWrite(UINT32 VinID, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[IMX577_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;

    if (Size > IMX577_SENSOR_I2C_MAX_SIZE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = IMX577_SENSOR_I2C_SLAVE_ADDR;
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(IMX577I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        RetVal = AmbaI2C_MasterWrite(IMX577I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                                     &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX577] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX577][IO] Addr = 0x%04x, Data = 0x%02x", Addr, pTxData[0], 0, 0, 0);
    //SENSOR_DBG_IO("[IMX577][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_RegisterWrite
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
static UINT32 IMX577_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)Data;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegWrite(pChan->VinID, Addr, &WData, 1U);
    }

    return RetVal;
}

static UINT32 RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = IMX577_SENSOR_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = ((UINT32)IMX577_SENSOR_I2C_SLAVE_ADDR | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;

#if 0  /* to be refined with bsp file */
    RetVal = AmbaI2C_MasterReadAfterWrite(IMX577I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);
#endif
    RetVal = AmbaI2C_MasterReadAfterWrite(IMX577I2cChannel[VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX577] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_RegisterRead
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
static UINT32 IMX577_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(pChan->VinID, Addr, &RData, 1U);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX577_SetStandbyOn(UINT32 VinID)
{
    UINT32 RetVal;
    UINT8 WData = 0x00U;

    RetVal = RegWrite(VinID, IMX577_MODE_SEL_REG, &WData, 1U);
//    AmbaPrint("[IMX577] IMX577_SetStandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX577_SetStandbyOff(UINT32 VinID)
{
    UINT32 RetVal;
    UINT8 WData = 0x01U;

    RetVal = RegWrite(VinID, IMX577_MODE_SEL_REG, &WData, 1U);

    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * IMX577Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / IMX577Ctrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[IMX577] IMX577_SetStandbyOff");
}

static UINT32 IMX577_InitExtClklReg(UINT32 VinID, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    for (i = 0U; i < IMX577_NUM_EXTCLK_REG; i++) {
        RetVal |= RegWrite(VinID, IMX577ExtClkRegTable[i].Addr, &IMX577ExtClkRegTable[i].Data[ModeID], 1U);
    }

    return RetVal;
}

static UINT32 IMX577_InitGolbalReg(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    for (i = 0U; i < IMX577_NUM_GOLBAL_REG; i++) {
        RetVal |= RegWrite(VinID, IMX577GlobalRegTable[i].Addr, &IMX577GlobalRegTable[i].Data, 1U);
    }

    return RetVal;
}

static UINT32 IMX577_InitImageQualityReg(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    for (i = 0U; i < IMX577_NUM_IMG_QUALITY_REG; i++) {
        RetVal |= RegWrite(VinID, IMX577ImageQualityRegTable[i].Addr, &IMX577ImageQualityRegTable[i].Data, 1U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_ChangeReadoutMode
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
static UINT32 IMX577_ChangeReadoutMode(UINT32 VinID, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    for (i = 0; i < IMX577_NUM_READOUT_MODE_REG; i ++) {
        RetVal |= RegWrite(VinID, IMX577ModeRegTable[i].Addr, &IMX577ModeRegTable[i].Data[ModeID], 1U);
//        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX577RegTable[i].Addr,
//                             IMX577RegTable[i].Data[ModeID], RetVal, 0U, 0U);
    }

    IMX577Ctrl.CurrentAgcCtrl[0] = 0xffffffffU;
    IMX577Ctrl.CurrentAgcCtrl[1] = 0xffffffffU;
    IMX577Ctrl.CurrentDgcCtrl[0] = 0xffffffffU;
    IMX577Ctrl.CurrentDgcCtrl[1] = 0xffffffffU;
    IMX577Ctrl.CurrentShrCtrl[0] = 0xffffffffU;
    IMX577Ctrl.CurrentShrCtrl[1] = 0xffffffffU;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX577_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < IMX577_NUM_MODE; i++) {
        IMX577ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX577ModeInfoList[i].FrameTiming.LineLengthPck /
                                                ((DOUBLE)IMX577OutputInfo[i].DataRate
                                                        * (DOUBLE)IMX577OutputInfo[i].NumDataLanes
                                                        / (DOUBLE)IMX577OutputInfo[i].NumDataBits));

        //AmbaPrint("row time: %f", IMX577ModeInfoList[i].RowTime);

    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_Init
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
static UINT32 IMX577_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const IMX577_FRAME_TIMING_s *pFrameTime = &IMX577ModeInfoList[0U].FrameTiming;

//    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "IMX290 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#if 0  /* to be refined with bsp file */
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
#endif
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        IMX577_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_Enable
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
static UINT32 IMX577_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_Disable
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
static UINT32 IMX577_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX577_SetStandbyOn(pChan->VinID);
    }

    //AmbaPrint("[IMX577] IMX577_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_GetStatus
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
static UINT32 IMX577_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &IMX577Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_GetModeInfo
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
static UINT32 IMX577_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s DesiredMode;

    if ((pChan == NULL) || (pMode ==NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(&DesiredMode, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) == ERR_NONE) {

            if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
                DesiredMode.ModeID = IMX577Ctrl.Status.ModeInfo.Config.ModeID;
            }

            if ((DesiredMode.ModeID >= IMX577_NUM_MODE) ||
                (pModeInfo == NULL)) {
                RetVal = SENSOR_ERR_ARG;
            } else {

                IMX577_PrepareModeInfo(&DesiredMode, pModeInfo);
            }
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_GetDeviceInfo
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
static UINT32 IMX577_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pDeviceInfo, &IMX577DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_GetHdrInfo
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
static UINT32 IMX577_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pHdrInfo, &IMX577Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_GetCurrentGainFactor
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
static UINT32 IMX577_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT AnalogGainFactor = 0.0f;
    FLOAT DigitalGainFactor = 0.0f;
    UINT8 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (IMX577Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX577Ctrl.CurrentAgcCtrl[0]);
            DigitalGainFactor = (FLOAT)IMX577Ctrl.CurrentDgcCtrl[0] / 256.0f;
            *pGainFactor = AnalogGainFactor * DigitalGainFactor;

        } else if (IMX577Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (IMX577Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX577Ctrl.CurrentAgcCtrl[i]);
                DigitalGainFactor = (FLOAT)IMX577Ctrl.CurrentDgcCtrl[i] / 256.0f;
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
 *  @RoutineName:: IMX577_GetCurrentShutterSpeed
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
static UINT32 IMX577_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX577Ctrl.Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT8 i;

    UINT32 RetVal = SENSOR_ERR_NONE;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX577Ctrl.CurrentShrCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX577Ctrl.CurrentShrCtrl[i];
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
    if (DesiredFactor < 1.0) { /* 1.0 = IMX577DeviceInfo.MinAnalogGainFactor * IMX577DeviceInfo.MinDigitalGainFactor */
        *pAnalogGainCtrl = 0U;
        *pDigitalGainCtrl = 0x100U;

        *pActualFactor = 1.0f;
    } else {
        FLOAT AnalogGainFactor;
        FLOAT DigitalGainFactor;
        DOUBLE DigitalGainCtrlInDb;
        DOUBLE AnalogGainCtrlInDb;

        DigitalGainFactor = DesiredFactor / IMX577DeviceInfo.MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0) {
            *pDigitalGainCtrl = 0x100U;
            DigitalGainFactor = 1.0f;

            AnalogGainFactor = DesiredFactor;
        } else {
            if (DigitalGainFactor > IMX577DeviceInfo.MaxDigitalGainFactor) {
                DigitalGainFactor = IMX577DeviceInfo.MaxDigitalGainFactor;
            }

            if (AmbaWrap_floor((DigitalGainFactor * 256.0), &DigitalGainCtrlInDb) == ERR_NONE) {
                *pDigitalGainCtrl = (UINT32) DigitalGainCtrlInDb;

                DigitalGainFactor = (FLOAT)*pDigitalGainCtrl / 256.0f;

                AnalogGainFactor = DesiredFactor / DigitalGainFactor;
            } else {
                DigitalGainFactor = 0.0f;
                AnalogGainFactor = 1.0f;
            }
        }

        if (AmbaWrap_floor(((1024.0) - (1024.0 / AnalogGainFactor)), &AnalogGainCtrlInDb) == ERR_NONE) {
            *pAnalogGainCtrl = (UINT32)AnalogGainCtrlInDb;

            if (*pAnalogGainCtrl > 978U) {
                *pAnalogGainCtrl = 978U; /* Max. 26.951 dB */
            }

            AnalogGainFactor = 1024.0f / (1024.0f - (FLOAT)*pAnalogGainCtrl);

            *pActualFactor = AnalogGainFactor * DigitalGainFactor;
        } else {
            *pActualFactor = 0.0f;
        }
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_ConvertGainFactor
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
static UINT32 IMX577_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &IMX577Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertGainFactor(pDesiredFactor->Gain[0], &(pActualFactor->Gain[0]), &(pGainCtrl->AnalogGain[0]), &(pGainCtrl->DigitalGain[0]));
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
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

    if (AmbaWrap_floor((DOUBLE)(ExposureTime / ((DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame)), &ShutterCtrlInDb) == ERR_NONE) {
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
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
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
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_ConvertShutterSpeed
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
static UINT32 IMX577_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX577Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
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
 *  @RoutineName:: IMX577_SetAnalogGainCtrl
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
static UINT32 IMX577_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX577Ctrl.Status.ModeInfo;
    UINT8 TxData[2U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal = RegWrite(pChan->VinID, IMX577_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX577Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, IMX577_DOL_ANA_GAIN_1ST_MSB_REG, TxData, 2);

            TxData[0] = (UINT8)((pAnalogGainCtrl[1] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[1] & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, IMX577_DOL_ANA_GAIN_2ND_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX577Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
            IMX577Ctrl.CurrentAgcCtrl[1] = pAnalogGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][IMX577][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_SetDigitalGainCtrl
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
static UINT32 IMX577_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX577Ctrl.Status.ModeInfo;
    UINT8 TxData[2U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            RetVal = RegWrite(pChan->VinID, IMX577_DIG_GAIN_GR_MSB_REG, TxData, 2U);

            /* Update current AGC control */
            IMX577Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, IMX577_DOL_DIG_GAIN_1ST_MSB_REG, TxData, 2U);

            TxData[0] = (UINT8)((pDigitalGainCtrl[1] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[1] & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, IMX577_DOL_DIG_GAIN_2ND_MSB_REG, TxData, 2U);

            /* Update current AGC control */
            IMX577Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
            IMX577Ctrl.CurrentDgcCtrl[1] = pDigitalGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][IMX577][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_SetShutterCtrl
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
static UINT32 IMX577_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX577Ctrl.Status.ModeInfo;
    UINT8 TxData[2U];

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            RetVal = RegWrite(pChan->VinID, IMX577_COARSE_INTEG_TIME_MSB_REG, TxData, 2U);

            /* Update current shutter control */
            IMX577Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, IMX577_DOL_CIT_1ST_MSB, TxData, 2U);

            TxData[0] = (UINT8)((pShutterCtrl[1] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[1] & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, IMX577_DOL_CIT_2ND_MSB, TxData, 2U);

            /* Update current shutter control */
            IMX577Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
            IMX577Ctrl.CurrentShrCtrl[1] = pShutterCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        //    SENSOR_DBG_SHR("[Vin%d/Sensor%d][IMX577][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
        //                      pChan->VinID, pChan->SensorID,  *pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_SetSlowShutterCtrl
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
static UINT32 IMX577_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = IMX577Ctrl.Status.ModeInfo.Config.ModeID;
    UINT32 TargetFrameLengthLines = 0U;
    UINT8 TxData[2U];

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX577Ctrl.Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        TargetFrameLengthLines = IMX577ModeInfoList[ModeID].FrameTiming.FrameLengthLines * SlowShutterCtrl;

        TxData[0] = (UINT8)((TargetFrameLengthLines >> 8U) & 0x00ffU);
        TxData[1] = (UINT8) (TargetFrameLengthLines & 0x00ffU);
        RetVal = RegWrite(pChan->VinID, IMX577_FRM_LENGTH_LINES_MSB_REG, TxData, 2U);


        /* Update frame rate information */
        IMX577Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        IMX577Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        IMX577Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick = IMX577ModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d ][IMX577][SHR] SlowShutterCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   SlowShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_Config
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
static UINT32 IMX577_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    AMBA_VIN_MIPI_PAD_CONFIG_s IMX577PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
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

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX577Ctrl.Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;

    if (ModeID >= IMX577_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX577] Config Mode: %d",ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        IMX577_PrepareModeInfo(pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        IMX577PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        IMX577PadConfig.EnabledPin = (((UINT32)1U << pModeInfo->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
        RetVal = AmbaVIN_MipiReset(pChan->VinID, &IMX577PadConfig);
        if (RetVal == ERR_NONE) {
            if (pMode->KeepState == 0U) {
                /* set pll */
                RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
                if (RetVal == ERR_NONE) {
                    RetVal = AmbaKAL_TaskSleep(3);
                    if (RetVal == ERR_NONE) {
                        /* reset sensor */
                        RetVal = IMX577_HardwareReset(pChan->VinID);
                        if (RetVal == ERR_NONE) {
                            /* program sensor */
                            /* Write external clock setting reg */
                            RetVal = IMX577_InitExtClklReg(pChan->VinID, ModeID);
                            if (RetVal == ERR_NONE) {
                                /* Write golbal reg table after hardware reset */
                                RetVal = IMX577_InitGolbalReg(pChan->VinID);
                                if (RetVal == ERR_NONE) {
                                    /* Write image quality reg table after hardware reset */
                                    RetVal = IMX577_InitImageQualityReg(pChan->VinID);
                                    if (RetVal == ERR_NONE) {
                                        /* Write registers of mode change to sensor */
                                        RetVal = IMX577_ChangeReadoutMode(pChan->VinID, ModeID);
                                        if (RetVal == ERR_NONE) {
                                            RetVal = IMX577_SetStandbyOff(pChan->VinID);
                                            if (RetVal == ERR_NONE) {
                                                /* config vin */
                                                RetVal = IMX577_ConfigVin(pChan->VinID, pModeInfo);
#ifdef CONFIG_IMX577_POSTCFG_ENABLED
                                                if (RetVal == ERR_NONE) {
                                                    RetVal = IMX577_SetStandbyOn(pChan->VinID);
                                                }
#endif
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                IMX577Ctrl.CurrentAgcCtrl[0] = 0xffffffffU;
                IMX577Ctrl.CurrentAgcCtrl[1] = 0xffffffffU;
                IMX577Ctrl.CurrentDgcCtrl[0] = 0xffffffffU;
                IMX577Ctrl.CurrentDgcCtrl[1] = 0xffffffffU;
                IMX577Ctrl.CurrentShrCtrl[0] = 0xffffffffU;
                IMX577Ctrl.CurrentShrCtrl[1] = 0xffffffffU;

                /* config vin */
                RetVal = IMX577_ConfigVin(pChan->VinID, pModeInfo);
            }
        }
    }

    return RetVal;
}

#ifdef CONFIG_IMX577_POSTCFG_ENABLED
static UINT32 IMX577_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return IMX577_SetStandbyOff(pChan->VinID);
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX577Obj = {
    .SensorName             = "IMX577",
    .SerdesName             = "NA",
    .Init                   = IMX577_Init,
    .Enable                 = IMX577_Enable,
    .Disable                = IMX577_Disable,
    .Config                 = IMX577_Config,
    .GetStatus              = IMX577_GetStatus,
    .GetModeInfo            = IMX577_GetModeInfo,
    .GetDeviceInfo          = IMX577_GetDeviceInfo,
    .GetHdrInfo             = IMX577_GetHdrInfo,
    .GetCurrentGainFactor   = IMX577_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX577_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX577_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX577_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX577_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX577_SetDigitalGainCtrl,
    .SetShutterCtrl         = IMX577_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX577_SetSlowShutterCtrl,

    .RegisterRead           = IMX577_RegisterRead,
    .RegisterWrite          = IMX577_RegisterWrite,

#ifdef CONFIG_IMX577_POSTCFG_ENABLED
    .ConfigPost             = IMX577_ConfigPost,
#else
    .ConfigPost             = NULL,
#endif
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
