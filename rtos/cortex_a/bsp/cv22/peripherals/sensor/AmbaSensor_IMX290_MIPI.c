/**
 *  @file AmbaSensor_IMX290_MIPI.c
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
 *  @details Control APIs of SONY IMX290_MIPI CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX290_MIPI.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "bsp.h"

static UINT32 IMX290_MIPII2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
};

/*-----------------------------------------------------------------------------------------------*\
 * IMX290_MIPI sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX290_MIPI_CTRL_s IMX290_MIPICtrl[AMBA_NUM_VIN_CHANNEL] = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_PrepareModeInfo
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
static void IMX290_MIPI_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &IMX290_MIPIInputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX290_MIPIOutputInfo[ModeID];
    const IMX290_MIPI_FRAME_TIMING_s    *pFrameTiming   = &IMX290_MIPIModeInfoList[ModeID].FrameTiming;
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->LineLengthPck            = pFrameTiming->LineLengthPck;
    pModeInfo->FrameLengthLines         = pFrameTiming->FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines;
    pModeInfo->InputClk                 = pFrameTiming->InputClk;
    pModeInfo->RowTime                  = IMX290_MIPIModeInfoList[ModeID].RowTime;
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX290_MIPIHdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }

    if(U32RVal != 0U) {
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_ConfigVin
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
static UINT32 IMX290_MIPI_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s IMX290_MIPIVinConfig = {
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
        .VirtChanHDRConfig = {0},
    };
    //-----------------------------------------------------------------------------------------------//

    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &IMX290_MIPIVinConfig;

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
static inline UINT32 IMX290_MIPI_HardwareReset(UINT32 VinID)
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
 *  @RoutineName:: IMX290_MIPI_RegRW
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
static UINT32 IMX290_MIPI_RegRW(UINT32 VinID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[3];
    UINT32 TxSize;

    I2cConfig.SlaveAddr = IMX290_MIPI_SENSOR_I2C_SLAVE_ADDR;
    I2cConfig.DataSize  = 3U;
    I2cConfig.pDataBuf  = TxDataBuf;

    TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
    TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
    TxDataBuf[2] = Data;

    RetVal = AmbaI2C_MasterWrite(IMX290_MIPII2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                 &I2cConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX290_MIPI] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    //SENSOR_DBG_IO("[IMX290_MIPI][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_RegisterWrite
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
static UINT32 IMX290_MIPI_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)Data;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX290_MIPI_RegRW(pChan->VinID, Addr, WData);
    }

    return RetVal;
}

static UINT32 IMX290_MIPI_RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = IMX290_MIPI_SENSOR_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = IMX290_MIPI_SENSOR_I2C_SLAVE_ADDR | 1U;
    I2cRxConfig.DataSize  = 1U;
    I2cRxConfig.pDataBuf  = pRxData;

    RetVal = AmbaI2C_MasterReadAfterWrite(IMX290_MIPII2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX290_MIPI] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_RegisterRead
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
static UINT32 IMX290_MIPI_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX290_MIPI_RegRead(pChan->VinID, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_SetAnalogGainReg
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
static UINT32 IMX290_MIPI_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AGC)
{
    return IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_AGAIN, (UINT8)AGC);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_SetShutterReg
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
static UINT32 IMX290_MIPI_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 NumXhsEshrSpeed)
{
    UINT32 SHS1;
    UINT32 RetVal = SENSOR_ERR_NONE;

    /* Actual exp line = VMAX - (SHS1 + 1) */
    SHS1 = (IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - NumXhsEshrSpeed) - 1U;

    RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS1_LSB, (UINT8)(SHS1 & 0xffU));
    RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS1_MSB, (UINT8)((SHS1 >> 8U) & 0xffU));
    RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS1_HSB, (UINT8)((SHS1 >> 16U) & 0x03U));

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_SetHdrShutterReg
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
static UINT32 IMX290_MIPI_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pNumXhsEshrSpeed)
{
    UINT32 FSC, SHSX, RHS1, RHS2;
    UINT32 RetVal = SENSOR_ERR_NONE;

    FSC = IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame;
    RHS1 = IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].MaxExposureLine + IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels + 1U;
    RHS2 = IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[2].MaxExposureLine + RHS1 + 4U;

    if (IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels == 2U) {
        /* LEF */
        {   /* Actual exp line = FSC - (SHS2 + 1) */
            SHSX = (FSC - pNumXhsEshrSpeed[0U]) - 1U;

            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS2_LSB, (UINT8)(SHSX & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS2_MSB, (UINT8)((SHSX >> 8U) & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS2_HSB, (UINT8)((SHSX >> 16U) & 0x03U));
        }
        /* SEF1 */
        {
            /* Actual exp line = RHS1 - (SHS1 + 1) */
            SHSX = (RHS1 - pNumXhsEshrSpeed[1U]) - 1U;

            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS1_LSB, (UINT8)(SHSX & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS1_MSB, (UINT8)((SHSX >> 8U) & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS1_HSB, (UINT8)((SHSX >> 16U) & 0x03U));
        }
    } else if (IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels == 3U) {
        /* LEF */
        {
            /* Actual exp line = FSC - (SHS3 + 1) */
            SHSX = (FSC - pNumXhsEshrSpeed[0U]) - 1U;

            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS3_LSB, (UINT8)(SHSX & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS3_MSB, (UINT8)((SHSX >> 8U) & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS3_HSB, (UINT8)((SHSX >> 16U) & 0x03U));
        }
        /* SEF1 */
        {
            /* Actual exp line = RHS1 - (SHS1 + 1) */
            SHSX = (RHS1 - pNumXhsEshrSpeed[1U]) - 1U;

            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS1_LSB, (UINT8)(SHSX & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS1_MSB, (UINT8)((SHSX >> 8U) & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS1_HSB, (UINT8)((SHSX >> 16U) & 0x03U));
        }
        /* SEF2 */
        {
            /* Actual exp line = RHS2 - (SHS2 + 1) */
            SHSX = (RHS2 - pNumXhsEshrSpeed[2U]) - 1U;

            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS2_LSB, (UINT8)(SHSX & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS2_MSB, (UINT8)((SHSX >> 8U) & 0xffU));
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_SHS2_HSB, (UINT8)((SHSX >> 16U) & 0x03U));
        }
    } else {
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_SetSlowShutterReg
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
static UINT32 IMX290_MIPI_SetSlowShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 IntegrationPeriodInFrame)
{
    UINT32 TargetFrameLengthLines;
    UINT32 ModeID = IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (IntegrationPeriodInFrame < 1U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TargetFrameLengthLines = IMX290_MIPIModeInfoList[ModeID].FrameTiming.FrameLengthLines * IntegrationPeriodInFrame;

        if (TargetFrameLengthLines > 0x3ffffU) {
            TargetFrameLengthLines = 0x3ffffU;
        }

        RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_VMAX_LSB, (UINT8)(TargetFrameLengthLines & 0xffU));
        RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_VMAX_MSB, (UINT8)((TargetFrameLengthLines >> 8U) & 0xffU));
        RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_VMAX_HSB, (UINT8)((TargetFrameLengthLines >> 16U) & 0x03U));

        /* Update frame rate information */
        IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale = IMX290_MIPIModeInfoList[ModeID].FrameTiming.FrameRate.TimeScale / IntegrationPeriodInFrame;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX290_MIPI_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    RetVal = IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_STANDBY, 0x01);
    if (AmbaKAL_TaskSleep(2) == ERR_NONE) {
        RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_MASTERSTOP, 0x01);
        //    AmbaPrint("[IMX290_MIPI] IMX290_MIPI_SetStandbyOn");
    } else {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX290_MIPI_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    RetVal = IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_STANDBY, 0x00U);
    if (AmbaKAL_TaskSleep(2U) == ERR_NONE) {
        RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_MASTERSTOP, 0x00U);
    } else {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[IMX290_MIPI] IMX290_MIPI_SetStandbyOff");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_ChangeReadoutMode
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
static UINT32 IMX290_MIPI_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    for (i = 0U; i < IMX290_MIPI_NUM_INIT_REG; i++) {
        RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPIInitRegTable[i].Addr, IMX290_MIPIInitRegTable[i].Data);
    }

    if (AmbaKAL_TaskSleep(1U) == ERR_NONE) {
        for (i = 0; i < IMX290_MIPI_NUM_MODE_REG; i ++) {
            RetVal |= IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPIRegTable[i].Addr, IMX290_MIPIRegTable[i].Data[ModeID]);
            //        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX290_MIPIRegTable[i].Addr,
            //                             IMX290_MIPIRegTable[i].Data[ModeID], RetVal, 0U, 0U);
        }

        IMX290_MIPICtrl[pChan->VinID].CurrentGainCtrl = 0xffffffffU;
        IMX290_MIPICtrl[pChan->VinID].CurrentShutterCtrl[0]= 0xffffffffU;
        IMX290_MIPICtrl[pChan->VinID].CurrentShutterCtrl[1]= 0xffffffffU;
        IMX290_MIPICtrl[pChan->VinID].CurrentShutterCtrl[2]= 0xffffffffU;
    } else {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX290_MIPI_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < IMX290_MIPI_NUM_MODE; i++) {
        IMX290_MIPIModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX290_MIPIModeInfoList[i].FrameTiming.LineLengthPck /
                                             ((DOUBLE)IMX290_MIPIOutputInfo[i].DataRate
                                              * (DOUBLE)IMX290_MIPIOutputInfo[i].NumDataLanes
                                              / (DOUBLE)IMX290_MIPIOutputInfo[i].NumDataBits));

        //AmbaPrint("row time: %f", IMX290_MIPIModeInfoList[i].RowTime);

    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_Init
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
static UINT32 IMX290_MIPI_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const IMX290_MIPI_FRAME_TIMING_s *pFrameTime = &IMX290_MIPIModeInfoList[0U].FrameTiming;

//    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "IMX290 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        IMX290_MIPI_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_Enable
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
static UINT32 IMX290_MIPI_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX290_MIPI_HardwareReset(pChan->VinID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_Disable
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
static UINT32 IMX290_MIPI_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX290_MIPI_SetStandbyOn(pChan);
    }

    //AmbaPrint("[IMX290_MIPI] IMX290_MIPI_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_GetStatus
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
static UINT32 IMX290_MIPI_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &IMX290_MIPICtrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_GetModeInfo
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
static UINT32 IMX290_MIPI_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s DesiredMode;

    if ((pChan == NULL) || (pMode ==NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(&DesiredMode, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) == ERR_NONE) {
            if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
                DesiredMode.ModeID = IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
            }

            if ((DesiredMode.ModeID >= IMX290_MIPI_NUM_MODE) ||
                (pModeInfo == NULL)) {
                RetVal = SENSOR_ERR_ARG;
            } else {

                IMX290_MIPI_PrepareModeInfo(&DesiredMode, pModeInfo);
            }
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_GetDeviceInfo
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
static UINT32 IMX290_MIPI_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pDeviceInfo, &IMX290_MIPIDeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_GetHdrInfo
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
static UINT32 IMX290_MIPI_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pHdrInfo, &IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_GetCurrentGainFactor
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
static UINT32 IMX290_MIPI_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 i;
    DOUBLE GainFactor_D = 0.0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_pow(10.0, 0.015 * (DOUBLE)IMX290_MIPICtrl[pChan->VinID].CurrentGainCtrl, &GainFactor_D) == ERR_NONE) {
            pGainFactor[0] = (FLOAT)GainFactor_D;
            if (IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
                for (i = 1U; i < (IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                    pGainFactor[i] = pGainFactor[0];
                }
            }
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }
    //AmbaPrint("GainFactor:%f", *pGainFactor);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_GetCurrentShutterSpeed
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
static UINT32 IMX290_MIPI_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = pModeInfo->RowTime * (FLOAT)IMX290_MIPICtrl[pChan->VinID].CurrentShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = pModeInfo->RowTime * (FLOAT)IMX290_MIPICtrl[pChan->VinID].CurrentShutterCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_ConvertGainFactor
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
static UINT32 IMX290_MIPI_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType;
    DOUBLE DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
    DOUBLE LogDesiredFactor = 0.0;
    FLOAT ActualFactor = 0.0f;
    DOUBLE ActualFactor64 = 0.0;
    DOUBLE AnalogGainCtrlInDb;
    UINT32 AnalogGainCtrl;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        HdrType = IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
        if(DesiredFactor > 3981.071706) {
            AnalogGainCtrl = 0xF0U;         //Max Again register value
            ActualFactor =  (FLOAT)3981.071706;     //Max Again
        } else if (DesiredFactor >= 1.0) {
            RetVal = AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            RetVal |= AmbaWrap_floor((66.666666666666667 * LogDesiredFactor), &AnalogGainCtrlInDb);
            RetVal |= AmbaWrap_pow(10.0, 0.015 * AnalogGainCtrlInDb, &ActualFactor64);

            if (RetVal == ERR_NONE) {
                AnalogGainCtrl = (UINT32)AnalogGainCtrlInDb;
                ActualFactor = (FLOAT)ActualFactor64;
            } else {
                AnalogGainCtrl = 0U;
                ActualFactor = 0.0f;
                RetVal = SENSOR_ERR_UNEXPECTED;
            }
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
    UINT32 ExposureLineMargin = 2U;
    DOUBLE ShutterCtrlInDb;
    UINT32 ShutterCtrl;

    if (AmbaWrap_floor(((DOUBLE)ExposureTime / (DOUBLE)pModeInfo->RowTime), &ShutterCtrlInDb) == ERR_NONE) {
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
            MaxExposureline = (((ShutterCtrl / pModeInfo->NumExposureStepPerFrame) + 1U) * pModeInfo->NumExposureStepPerFrame) - ExposureLineMargin;
            MinExposureline = 1U;
        }

        if (ShutterCtrl > MaxExposureline) {
            ShutterCtrl = MaxExposureline;
        } else if (ShutterCtrl < MinExposureline) {
            ShutterCtrl = MinExposureline;
        } else {
            /* do not need to adjust ShutterCtrl */
        }

        *pShutterCtrl   = ShutterCtrl;
        *pActualExptime = (FLOAT)ShutterCtrl * pModeInfo->RowTime;
    } else {
        *pShutterCtrl   = 0U;
        *pActualExptime = 0.0f;
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_ConvertShutterSpeed
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
static UINT32 IMX290_MIPI_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo;

        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                ConvertShutterSpeed(pModeInfo, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
        } else {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        }

        //AmbaPrint("ExposureTime:%f, RowTime: %f, pShutterCtrl:%d", ExposureTime, pModeInfo->RowTime, *pShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_SetAnalogGainCtrl
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
static UINT32 IMX290_MIPI_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX290_MIPI_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

        /* Update current AGC control */
        IMX290_MIPICtrl[pChan->VinID].CurrentGainCtrl = pAnalogGainCtrl[0];

        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][IMX290_MIPI][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_SetShutterCtrl
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
static UINT32 IMX290_MIPI_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo;

        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX290_MIPI_SetHdrShutterReg(pChan, pShutterCtrl);

            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                /* Update current shutter control */
                IMX290_MIPICtrl[pChan->VinID].CurrentShutterCtrl[i] = pShutterCtrl[i];
            }
        } else {
            RetVal = IMX290_MIPI_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            IMX290_MIPICtrl[pChan->VinID].CurrentShutterCtrl[0] = pShutterCtrl[0];
        }


        //    SENSOR_DBG_SHR("[Vin%d/Sensor%d][IMX290_MIPI][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
        //                      pChan->VinID, pChan->SensorID,  *pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_SetSlowShutterCtrl
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
static UINT32 IMX290_MIPI_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        RetVal = IMX290_MIPI_SetSlowShutterReg(pChan, SlowShutterCtrl);

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d ][IMX290_MIPI][SHR] SlowShutterCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   SlowShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_Config
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
static UINT32 IMX290_MIPI_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s IMX290_MIPIPadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX290_MIPICtrl[pChan->VinID].Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;

    if (ModeID >= IMX290_MIPI_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX290_MIPI] Config Mode: %d",ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        IMX290_MIPI_PrepareModeInfo(pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        IMX290_MIPIPadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        RetVal = AmbaVIN_MipiReset(pChan->VinID, &IMX290_MIPIPadConfig);

        if (RetVal == ERR_NONE) {
            /* set pll */
            RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
            if (RetVal == ERR_NONE) {
                RetVal = AmbaKAL_TaskSleep(3);
                if (RetVal == ERR_NONE) {
                    /* SW reset */
                    RetVal = IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_RESET, 0x01);
                    if (RetVal == ERR_NONE) {
                        /* program sensor */
                        RetVal = IMX290_MIPI_ChangeReadoutMode(pChan, ModeID);
                        if (RetVal == ERR_NONE) {
                            RetVal = IMX290_MIPI_SetStandbyOff(pChan);
                            if (RetVal == ERR_NONE) {
                                RetVal = IMX290_MIPI_RegRW(pChan->VinID, IMX290_MIPI_MASTERSTOP, 0x01);
                                if (RetVal == ERR_NONE) {
                                    /* config vin */
                                    RetVal = IMX290_MIPI_ConfigVin(pChan->VinID, pModeInfo);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 IMX290_MIPI_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return IMX290_MIPI_SetStandbyOff(pChan);
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX290_MIPIObj = {
    .SensorName             = "IMX290",
    .SerdesName             = "NA",
    .Init                   = IMX290_MIPI_Init,
    .Enable                 = IMX290_MIPI_Enable,
    .Disable                = IMX290_MIPI_Disable,
    .Config                 = IMX290_MIPI_Config,
    .GetStatus              = IMX290_MIPI_GetStatus,
    .GetModeInfo            = IMX290_MIPI_GetModeInfo,
    .GetDeviceInfo          = IMX290_MIPI_GetDeviceInfo,
    .GetHdrInfo             = IMX290_MIPI_GetHdrInfo,
    .GetCurrentGainFactor   = IMX290_MIPI_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX290_MIPI_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX290_MIPI_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX290_MIPI_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX290_MIPI_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = NULL, //IMX290_MIPI_SetDigitalGainCtrl,
    .SetShutterCtrl         = IMX290_MIPI_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX290_MIPI_SetSlowShutterCtrl,

    .RegisterRead           = IMX290_MIPI_RegisterRead,
    .RegisterWrite          = IMX290_MIPI_RegisterWrite,

    .ConfigPost             = IMX290_MIPI_ConfigPost,
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};

