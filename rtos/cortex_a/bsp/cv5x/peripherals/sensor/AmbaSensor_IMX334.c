/**
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

#include "AmbaWrap.h"
#include "AmbaMisraFix.h"

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX334.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "bsp.h"

static UINT32 IMX334I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
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
 * IMX334 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX334_CTRL_s IMX334Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      pMode:      pointer to desired sensor mode configuration
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
    const AMBA_SENSOR_HDR_INFO_s        *pHdrInfo       = &IMX334HdrInfo[ModeID];
    DOUBLE LinePeriod                                   = IMX334ModeInfoList[ModeID].LinePeriod;
    DOUBLE WorkDouble;
    UINT32 U32RVal = 0U;

    U32RVal |= AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->RowTime = (FLOAT)LinePeriod;
    WorkDouble = LinePeriod * (DOUBLE)pOutputInfo->DataRate;
    WorkDouble *= (DOUBLE)pOutputInfo->NumDataLanes;
    WorkDouble /= (DOUBLE)pOutputInfo->NumDataBits;
    pModeInfo->LineLengthPck = (UINT32)WorkDouble;
    pModeInfo->FrameLengthLines = pFrameTiming->NumXhsPerV * pFrameTiming->NumXvsPerV;
    pModeInfo->NumExposureStepPerFrame = pFrameTiming->NumXhsPerV * pFrameTiming->NumXvsPerV;
    pModeInfo->InternalExposureOffset = 0.0f;
    pModeInfo->InputClk = pFrameTiming->InputClk;

    if (pHdrInfo->HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
        pModeInfo->RowTime *= (FLOAT)pHdrInfo->ActiveChannels;
        pModeInfo->LineLengthPck *= (UINT32)pHdrInfo->ActiveChannels;
        pModeInfo->FrameLengthLines /= (UINT32)pHdrInfo->ActiveChannels;
    }

    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(const AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX334HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }

    if (U32RVal != 0U) {
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output frames of the new readout mode
 *
 *  @Input      ::
 *      VinID:      Vin channel ID
 *      pModeInfo:  pointer to Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
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

    if (AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        /* MisraC */
    }

    return AmbaVIN_MipiConfig(VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_HardwareReset
 *
 *  @Description:: Reset IMX334 Image Sensor Device
 *
 *  @Input      ::
 *      VinID:      Vin channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 IMX334_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

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
 *  @Description:: Write sensor registers
 *
 *  @Input      ::
 *      VinID:      Vin channel ID
 *      Addr:       Register Address
 *      pTxData:    Pointer to Write data buffer
 *      Size:       Number of Write data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 RegWrite(UINT32 VinID, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
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

        RetVal = AmbaI2C_MasterWrite(IMX334I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX334] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }

        for (i = 0U; i < Size; i++) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr=0x%04x, Data=0x%02x", Addr + i, pTxData[i], 0, 0, 0);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_RegisterWrite
 *
 *  @Description:: Write sensor registers API
 *
 *  @Input      ::
 *      pChan:  pointer to sensor channel ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
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

    I2cTxConfig.SlaveAddr = IMX334_SENSOR_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = ((UINT32)IMX334_SENSOR_I2C_SLAVE_ADDR | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;

    RetVal = AmbaI2C_MasterReadAfterWrite(IMX334I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
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
 *      pChan:  pointer to sensor channel ID
 *      Addr:   Register address
 *
 *  @Output     ::
 *      pData:  pointer to read Register value
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(pChan->VinID, Addr, &RData, 1U);
        *pData = (UINT16)RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      ::
 *      pChan:  pointer to sensor channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = 0x01U;

    RetVal |= RegWrite(pChan->VinID, IMX334_STANDBY, &WData, 1U);
    if (AmbaKAL_TaskSleep(2) == ERR_NONE) {
        RetVal |= RegWrite(pChan->VinID, IMX334_MASTERSTOP, &WData, 1U);

        WData = 0x04U;
        RetVal |= RegWrite(pChan->VinID, IMX334_RESTART, &WData, 1U);
        WData = 0x00U;
        RetVal |= RegWrite(pChan->VinID, IMX334_RESTART, &WData, 1U);
    } else {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      ::
 *      pChan:  pointer to sensor channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = 0x00U;

    RetVal |= RegWrite(pChan->VinID, IMX334_STANDBY, &WData, 1U);
    if (AmbaKAL_TaskSleep(18U) == ERR_NONE) {
        RetVal |= RegWrite(pChan->VinID, IMX334_MASTERSTOP, &WData, 1U);
    } else {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

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
 *
 *  @Input      ::
 *      pChan:       pointer to sensor channel ID
 *      ModeID:      Sensor mode ID
 *      Hrev:        Horizontal direction Readout inversion control flag
 *      Vrev:        Vertical direction Readout inversion control flag
 *      pHdrInfo:    pointer to HdrInfo
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID, UINT32 Hrev, UINT32 Vrev, const AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE, i;
    UINT8 WData, RData, Index = 0U;

    for (i = 0; i < IMX334_NUM_INIT_REG; i ++) {
        RetVal |= RegWrite(pChan->VinID, IMX334InitRegTable[i].Addr, &IMX334InitRegTable[i].Data, 1U);
    }

    for (i = 0; i < IMX334_NUM_READOUT_MODE_REG; i ++) {
        RetVal |= RegWrite(pChan->VinID, IMX334ModeRegTable[i].Addr, &IMX334ModeRegTable[i].Data[ModeID], 1U);
    }

    RetVal |= RegRead(pChan->VinID, IMX334_WINMODE, &RData, 1U);
    if ((RData & 0xfU) == 1U) {
        Index = 1U;
    } else {
        Index = 0U;
    }

    if (Hrev != 0U) {
        WData = 0x1U;
        RetVal |= RegWrite(pChan->VinID, IMX334_HREVERSE, &WData, 1U);
    }

    if (Vrev != 0U) {
        WData = 0x1U;
        RetVal |= RegWrite(pChan->VinID, IMX334_VREVERSE, &WData, 1U);
    }

    if (Vrev != 0U) {
        for (i = 0; i < IMX334_NUM_READOUT_DIRECTION_REG; i++) {
            RetVal |= RegWrite(pChan->VinID, IMX334InvertedVRegTable[Index][i].Addr, &IMX334InvertedVRegTable[Index][i].Data, 1U);
        }
    } else {
        for (i = 0; i < IMX334_NUM_READOUT_DIRECTION_REG; i++) {
            RetVal |= RegWrite(pChan->VinID, IMX334NormalVRegTable[Index][i].Addr, &IMX334NormalVRegTable[Index][i].Data, 1U);
        }
    }

    if ((pHdrInfo->HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
        (pHdrInfo->HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
        if (pHdrInfo->NotSupportIndividualGain == 0U) {
            RetVal |= RegRead(pChan->VinID, IMX334_FGAINEN, &RData, 1U);
            /* Set 0x3200[0]=0 to enable each frame gain adjustment */
            WData = RData & (UINT8)~0x1U;
            RetVal |= RegWrite(pChan->VinID, IMX334_FGAINEN, &WData, 1U);
        }
    }

    IMX334Ctrl.CurrentGainCtrl[0] = 0xffffffffU;
    IMX334Ctrl.CurrentGainCtrl[1] = 0xffffffffU;
    IMX334Ctrl.CurrentGainCtrl[2] = 0xffffffffU;
    IMX334Ctrl.CurrentShrCtrl[0] = 0xffffffffU;
    IMX334Ctrl.CurrentShrCtrl[1] = 0xffffffffU;
    IMX334Ctrl.CurrentShrCtrl[2] = 0xffffffffU;

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
        IMX334ModeInfoList[i].LinePeriod = (FLOAT)((DOUBLE)IMX334ModeInfoList[i].FrameTiming.NumTickPerXhs /
                                           ((DOUBLE)IMX334ModeInfoList[i].FrameTiming.RefClkMul *
                                            (DOUBLE)IMX334ModeInfoList[i].FrameTiming.InputClk));
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      pChan:  pointer to sensor channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const IMX334_FRAME_TIMING_s *pFrameTime = &IMX334ModeInfoList[0U].FrameTiming;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
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
 *      pChan:  pointer to sensor channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
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
 *      pChan:  pointer to sensor channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX334_SetStandbyOn(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetStatus
 *
 *  @Description:: Get current sensor status
 *
 *  @Input      ::
 *      pChan:   pointer to sensor channel ID
 *
 *  @Output     ::
 *      pStatus: pointer to current sensor status
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &IMX334Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetModeInfo
 *
 *  @Description:: Get Mode Info of indicated mode
 *
 *  @Input      ::
 *      pChan:  pointer to sensor channel ID
 *      pMode:  pointer to desired sensor mode configuration
 *
 *  @Output     ::
 *      pModeInfo: pointer to requested Mode Info
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s DesiredMode;

    if ((pChan == NULL) || (pMode == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(&DesiredMode, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) == ERR_NONE) {
            if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
                DesiredMode.ModeID = IMX334Ctrl.Status.ModeInfo.Config.ModeID;
            }

            if ((DesiredMode.ModeID >= IMX334_NUM_MODE) ||
                (pModeInfo == NULL)) {
                RetVal = SENSOR_ERR_ARG;
            } else {

                IMX334_PrepareModeInfo(&DesiredMode, pModeInfo);
            }
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
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
 *      pChan:  pointer to sensor channel ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to device info
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pDeviceInfo, &IMX334DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetHdrInfo
 *
 *  @Description:: Get HDR-related information
 *
 *  @Input      ::
 *      pChan:         pointer to sensor channel ID
 *      pShutterCtrl:  pointer to Electronic shutter control
 *
 *  @Output     ::
 *      pHdrInfo:      senosr hdr information
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL) || (pHdrInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pHdrInfo, &IMX334Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetCurrentGainFactor
 *
 *  @Description:: Get Sensor Current Gain Factor
 *
 *  @Input      ::
 *      pChan:         pointer to sensor channel ID
 *
 *  @Output     ::
 *      pGainFactor:   pointer to current gain factor
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    DOUBLE GainFactor_dB, GainFactor64 = 1.0f;
    UINT8 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (IMX334Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            GainFactor_dB = (DOUBLE)IMX334Ctrl.CurrentGainCtrl[0] * 0.3;
            if (AmbaWrap_pow(10.0, GainFactor_dB / 20.0, &GainFactor64) != ERR_NONE) {
                RetVal = SENSOR_ERR_UNEXPECTED;
            }
            *pGainFactor = (FLOAT)GainFactor64;

        } else if ((IMX334Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (IMX334Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            for (i = 0U; i < (IMX334Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                GainFactor_dB = (DOUBLE)IMX334Ctrl.CurrentGainCtrl[i] * 0.3;
                if (AmbaWrap_pow(10.0, GainFactor_dB / 20.0, &GainFactor64) != ERR_NONE) {
                    RetVal = SENSOR_ERR_UNEXPECTED;
                }
                pGainFactor[i] = (FLOAT)GainFactor64;
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      pChan:         pointer to sensor channel ID
 *
 *  @Output     ::
 *      pExposureTime: pointer to current exposure time
 *
 *  @Return     ::
 *          UINT32 : Error code
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

    return RetVal;
}

static UINT32 ConvertGainFactor(FLOAT DesiredFactor, FLOAT *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 DesiredGainCtrl, AGainCtrl, DGainCtrl, GainCtrl;
    DOUBLE LogDesiredFactor, DesiredFactor_dB, ActualFactor_dB, ActualFactor64, WorkDouble;

    if (AmbaWrap_log10((DOUBLE)DesiredFactor, &LogDesiredFactor) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    DesiredFactor_dB = (FLOAT)(20.0 * LogDesiredFactor);

    WorkDouble = DesiredFactor_dB / 0.3;
    DesiredGainCtrl = (UINT32)WorkDouble;

    if (DesiredGainCtrl == 0U) {
        AGainCtrl = 0;
        DGainCtrl = 0;
    } else if (DesiredGainCtrl < IMX334_MAX_AGAIN_CTRL) {
        AGainCtrl = DesiredGainCtrl;
        DGainCtrl = 0;
    } else if (DesiredGainCtrl < IMX334_MAX_TOTAL_GAIN_CTRL) {
        AGainCtrl = IMX334_MAX_AGAIN_CTRL;
        DGainCtrl = DesiredGainCtrl - IMX334_MAX_AGAIN_CTRL;
    } else {
        AGainCtrl = IMX334_MAX_AGAIN_CTRL;
        DGainCtrl = IMX334_MAX_DGAIN_CTRL;
    }

    GainCtrl = AGainCtrl + DGainCtrl;
    ActualFactor_dB = (DOUBLE)GainCtrl * 0.3;
    if (AmbaWrap_pow(10.0, ActualFactor_dB / 20.0, &ActualFactor64) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    *pActualFactor = (FLOAT)ActualFactor64;
    *pAnalogGainCtrl = AGainCtrl + DGainCtrl; /* set to GAIN register */
    *pDigitalGainCtrl = 0;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_ConvertGainFactor
 *
 *  @Description:: Convert gain factor to analog and digital gain control
 *
 *  @Input      ::
 *      pChan:              pointer to sensor channel ID
 *      pDesiredFactor:     pointer to Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      pointer to Achievable gain factor
 *      pGainCtrl:          pointer to Gain parameter for achievable gain factor
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal |= ConvertGainFactor(pDesiredFactor->Gain[0], &(pActualFactor->Gain[0]), &(pGainCtrl->AnalogGain[0]), &(pGainCtrl->DigitalGain[0]));
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                RetVal |= ConvertGainFactor(pDesiredFactor->Gain[i], &(pActualFactor->Gain[i]), &(pGainCtrl->AnalogGain[i]), &(pGainCtrl->DigitalGain[i]));
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
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
 *      pModeInfo:      Details of the specified readout mode
 *      HdrChannel:     0: LEF(HDR mode) or linear mdoe
 *                      1: SEF1(HDR mode),
 *                      2: SEF2(HDR mode),
 *      ExposureTime:   Desired Exposure time
 *
 *  @Output     ::
 *      pActualExptime: Actual exposure time
 *      pShutterCtrl:   Electronic shutter control
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 ModeID = pModeInfo->Config.ModeID;
    const IMX334_FRAME_TIMING_s *pFrameTiming = &IMX334ModeInfoList[ModeID].FrameTiming;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 MaxExposureLine, MinExposureLine;
    UINT32 ExposureLineMargin = 5U;
    UINT32 NumExposureStepPerFrame = pFrameTiming->NumXhsPerV * pFrameTiming->NumXvsPerV;
    UINT32 ShutterCtrl;
    DOUBLE ShutterTimeUnit = IMX334ModeInfoList[ModeID].LinePeriod;
    DOUBLE ShutterCtrlInDb;

    if (AmbaWrap_floor((DOUBLE)ExposureTime / ShutterTimeUnit, &ShutterCtrlInDb) == ERR_NONE) {
        ShutterCtrl = (UINT32)ShutterCtrlInDb;

        /* VMAX is 20 bits */
        if (ShutterCtrl > ((0xfffffU / NumExposureStepPerFrame) * NumExposureStepPerFrame)) {
            ShutterCtrl = ((0xfffffU / NumExposureStepPerFrame) * NumExposureStepPerFrame);
        }

        /* For HDR mode */
        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
            MaxExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
            MinExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
            /* For none-HDR mode */
        } else {
            MaxExposureLine = (((ShutterCtrl / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - ExposureLineMargin;
            MinExposureLine = 1U;
        }

        if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
            (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            ShutterCtrl -= (ShutterCtrl % (UINT32)pModeInfo->HdrInfo.ActiveChannels); /* must be multiple of n frames */
        }

        if (ShutterCtrl > MaxExposureLine) {
            ShutterCtrl = MaxExposureLine;
        } else if (ShutterCtrl < MinExposureLine) {
            ShutterCtrl = MinExposureLine;
        } else {
            /* do not need to adjust ShutterCtrl */
        }

        *pShutterCtrl   = ShutterCtrl;
        *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
    } else {
        *pShutterCtrl   = 0U;
        *pActualExptime = 0.0f;
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_ConvertShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      pChan:                  pointer to sensor channel ID
 *      pDesiredExposureTime:   pointer to Exposure time / shutter speed
 *
 *  @Output     ::
 *      pActualExposureTime:    pointer to Achievable gain factor
 *      pShutterCtrl:           pointer to Electronic shutter control
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;

    if ((pChan == NULL) || (pDesiredExposureTime == NULL) || (pActualExposureTime == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal |= ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                RetVal |= ConvertShutterSpeed(pModeInfo, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetAnalogGainCtrl
 *
 *  @Description:: Set analog gain control
 *
 *  @Input      ::
 *      pChan:              pointer to sensor channel ID
 *      pAnalogGainCtrl:    Analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    const UINT16 GainRegAddrLsb[3] = {
        [0] = IMX334_GAIN_LSB,
        [1] = IMX334_GAIN1_LSB,
        [2] = IMX334_GAIN2_LSB,
    };
    UINT8 TxData[2U];

    if ((pChan == NULL) || (pAnalogGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            TxData[1] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            RetVal = RegWrite(pChan->VinID, IMX334_GAIN_LSB, TxData, 2);

            /* Update current gain control */
            IMX334Ctrl.CurrentGainCtrl[0] = pAnalogGainCtrl[0];
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            if (pModeInfo->HdrInfo.NotSupportIndividualGain == 1U) {
                TxData[0] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
                TxData[1] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
                RetVal = RegWrite(pChan->VinID, GainRegAddrLsb[0], TxData, 2);

                for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                    /* Update current gain control */
                    IMX334Ctrl.CurrentGainCtrl[i] = pAnalogGainCtrl[0];
                }
            } else {
                for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                    TxData[0] = (UINT8) (pAnalogGainCtrl[i] & 0x00ffU);
                    TxData[1] = (UINT8)((pAnalogGainCtrl[i] >> 8U) & 0x00ffU);

                    RetVal = RegWrite(pChan->VinID, GainRegAddrLsb[i], TxData, 2);

                    /* Update current gain control */
                    IMX334Ctrl.CurrentGainCtrl[i] = pAnalogGainCtrl[i];
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      pChan:              pointer to sensor channel ID
 *      pDigitalGainCtrl:   pointer to Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pDigitalGainCtrl);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_MIPI_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      ShutterCtrl:    Integration time in number of XHS period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ShutterCtrl)
{
    UINT32 SHR0;
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    UINT8 TxData[3U];

    /* Actual exp line = VMAX - SHR0 */
    SHR0 = pModeInfo->NumExposureStepPerFrame - ShutterCtrl;

    TxData[0] = (UINT8) (SHR0 & 0x00ffU);
    TxData[1] = (UINT8)((SHR0 >> 8U) & 0x00ffU);
    TxData[2] = (UINT8)((SHR0 >> 16U) & 0x000fU);
    RetVal = RegWrite(pChan->VinID, IMX334_SHR0_LSB, TxData, 3U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetHdrShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      pShutterCtrl:    pointer to Integration time in number of XHS period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 FSC, SHRx, RHS1, RHS2;
    UINT32 RetVal = SENSOR_ERR_NONE;
    const UINT16 ShutterRegAddrLsb[3] = {
        [0] = IMX334_SHR0_LSB,
        [1] = IMX334_SHR1_LSB,
        [2] = IMX334_SHR2_LSB,
    };
    UINT8 TxData[3U];

    FSC = IMX334Ctrl.Status.ModeInfo.NumExposureStepPerFrame;

    if (IMX334Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels == 2U) {
        RHS1 = IMX334Ctrl.Status.ModeInfo.HdrInfo.ChannelInfo[1].MaxExposureLine + 9U;
        /* LEF */
        {   /* Actual exp line = FSC - SHR0 */
            SHRx = FSC - pShutterCtrl[0U];

            TxData[0] = (UINT8) (SHRx & 0x00ffU);
            TxData[1] = (UINT8)((SHRx >> 8U) & 0x00ffU);
            TxData[2] = (UINT8)((SHRx >> 16U) & 0x000fU);
            RetVal |= RegWrite(pChan->VinID, ShutterRegAddrLsb[0U], TxData, 3U);
        }
        /* SEF1 */
        {
            /* Actual exp line = RHS1 - SHR1 */
            SHRx = RHS1 - pShutterCtrl[1U];

            TxData[0] = (UINT8) (SHRx & 0x00ffU);
            TxData[1] = (UINT8)((SHRx >> 8U) & 0x00ffU);
            TxData[2] = (UINT8)((SHRx >> 16U) & 0x000fU);
            RetVal |= RegWrite(pChan->VinID, ShutterRegAddrLsb[1U], TxData, 3U);
        }
    } else if (IMX334Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels == 3U) {
        RHS1 = IMX334Ctrl.Status.ModeInfo.HdrInfo.ChannelInfo[1].MaxExposureLine + 13U;
        RHS2 = IMX334Ctrl.Status.ModeInfo.HdrInfo.ChannelInfo[2].MaxExposureLine + RHS1 + 13U;
        /* LEF */
        {
            /* Actual exp line = FSC - SHR0 */
            SHRx = FSC - pShutterCtrl[0U];

            TxData[0] = (UINT8) (SHRx & 0x00ffU);
            TxData[1] = (UINT8)((SHRx >> 8U) & 0x00ffU);
            TxData[2] = (UINT8)((SHRx >> 16U) & 0x000fU);
            RetVal |= RegWrite(pChan->VinID, ShutterRegAddrLsb[0U], TxData, 3U);
        }
        /* SEF1 */
        {
            /* Actual exp line = RHS1 - SHR1 */
            SHRx = RHS1 - pShutterCtrl[1U];

            TxData[0] = (UINT8) (SHRx & 0x00ffU);
            TxData[1] = (UINT8)((SHRx >> 8U) & 0x00ffU);
            TxData[2] = (UINT8)((SHRx >> 16U) & 0x000fU);
            RetVal |= RegWrite(pChan->VinID, ShutterRegAddrLsb[1U], TxData, 3U);
        }
        /* SEF2 */
        {
            /* Actual exp line = RHS2 - SHR2 */
            SHRx = RHS2 - pShutterCtrl[2U];

            TxData[0] = (UINT8) (SHRx & 0x00ffU);
            TxData[1] = (UINT8)((SHRx >> 8U) & 0x00ffU);
            TxData[2] = (UINT8)((SHRx >> 16U) & 0x000fU);
            RetVal |= RegWrite(pChan->VinID, ShutterRegAddrLsb[2U], TxData, 3U);
        }
    } else {
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetShutterCtrl
 *
 *  @Description:: set shutter control
 *
 *  @Input      ::
 *      pChan:          pointer to Vin ID and sensor ID
 *      ShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX334_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            IMX334Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if ((pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                   (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
            RetVal = IMX334_SetHdrShutterReg(pChan, &pShutterCtrl[0]);

            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i++) {
                /* Update current shutter control */
                IMX334Ctrl.CurrentShrCtrl[i] = pShutterCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_SetSlowShutterCtrl
 *
 *  @Description:: set slow shutter control
 *
 *  @Input      ::
 *      pChan:              pointer to sensor channel ID
 *      SlowShutterCtrl:    Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = IMX334Ctrl.Status.ModeInfo.Config.ModeID;
    UINT32 TargetFrameLengthLines = 0U;
    UINT8 TxData[3U];

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX334Ctrl.Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        TargetFrameLengthLines = ((IMX334ModeInfoList[ModeID].FrameTiming.NumXhsPerV *
                                   IMX334ModeInfoList[ModeID].FrameTiming.NumXvsPerV) * SlowShutterCtrl);

        TxData[0] = (UINT8) (TargetFrameLengthLines & 0x00ffU);
        TxData[1] = (UINT8)((TargetFrameLengthLines >> 8U) & 0x00ffU);
        TxData[2] = (UINT8)((TargetFrameLengthLines >> 16U) & 0x000fU);
        RetVal = RegWrite(pChan->VinID, IMX334_VMAX_LSB, TxData, 3U);

        /* Update frame rate information */
        IMX334Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        IMX334Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        IMX334Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick = IMX334ModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX334_Config
 *
 *  @Description:: Set sensor to indicated mode
 *
 *  @Input      ::
 *      pChan:     pointer to sensor channel ID
 *      pMode:     pointer to desired sensor mode configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX334_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 i, k;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX334Ctrl.Status.ModeInfo;
    static AMBA_VIN_MIPI_PAD_CONFIG_s IMX334PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    AMBA_VIN_MIPI_VC_CONFIG_s MipiVirtChanConfig = {0};
    const UINT8 VCToVinID[2][3] = {
        [0] = {
            [0] = AMBA_VIN_CHANNEL8,
            [1] = AMBA_VIN_CHANNEL9,
            [2] = AMBA_VIN_CHANNEL10,
        },
        [1] = {
            [0] = AMBA_VIN_CHANNEL11,
            [1] = AMBA_VIN_CHANNEL12,
            [2] = AMBA_VIN_CHANNEL13,
        },
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
    UINT8 WData;

    if ((ModeID >= IMX334_NUM_MODE) || ((pChan->VinID != AMBA_VIN_CHANNEL8) && (pChan->VinID != AMBA_VIN_CHANNEL11))) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX334] Config Mode: %d", ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        IMX334_PrepareModeInfo(pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        IMX334PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        IMX334PadConfig.EnabledPin = (((UINT32)1U << pModeInfo->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
        RetVal = AmbaVIN_MipiReset(pChan->VinID, &IMX334PadConfig);
        if (RetVal == ERR_NONE) {
            /* set pll */
            RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
            if (RetVal == ERR_NONE) {
                /* reset sensor */
                RetVal = IMX334_HardwareReset(pChan->VinID);
                if (RetVal == ERR_NONE) {
                    /* program sensor */
                    RetVal = IMX334_ChangeReadoutMode(pChan, ModeID, 0, 0, &pModeInfo->HdrInfo);
                    if (RetVal == ERR_NONE) {
                        RetVal = IMX334_SetStandbyOff(pChan);
                        if (RetVal == ERR_NONE) {
                            WData = 0x01U;
                            RetVal = RegWrite(pChan->VinID, IMX334_MASTERSTOP, &WData, 1U);
                            /* config vin */
                            if (RetVal == ERR_NONE) {
                                if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
                                    k = (pChan->VinID == AMBA_VIN_CHANNEL8) ? 0U : 1U;

                                    for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i++) {
                                        RetVal = IMX334_ConfigVin(VCToVinID[k][i], pModeInfo);
                                        if (RetVal == ERR_NONE) {
                                            MipiVirtChanConfig.VirtChanMask = 0U;
                                            MipiVirtChanConfig.VirtChan = (UINT8)i;
                                            RetVal = AmbaVIN_MipiVirtChanConfig(VCToVinID[k][i], &MipiVirtChanConfig);
                                            if (RetVal != ERR_NONE) {
                                                break;
                                            }
                                        }
                                    }
                                } else {
                                    RetVal = IMX334_ConfigVin(pChan->VinID, pModeInfo);
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

static UINT32 IMX334_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return IMX334_SetStandbyOff(pChan);
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
