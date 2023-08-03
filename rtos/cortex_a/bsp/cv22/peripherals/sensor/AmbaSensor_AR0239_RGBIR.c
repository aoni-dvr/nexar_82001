/**
 *  @file AmbaSensor_AR0239.c
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
 *  @details Control APIs of SONY AR0239 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_AR0239_RGBIR.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "bsp.h"

//#include "AmbaUtility.h"
static UINT32 AR0239_I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_I2C_CHANNEL_PIP,
};

#ifdef AR0239_IN_SLAVE_MODE
static UINT32 AR0239MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
#if 0//def IMX390_SHARING_MASTER_SYNC
    // TODO: use the same AMBA_SENSOR_MSYNC_CHANNEL on HW to make different VIN start together.
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_MSYNC_CHANNEL,
#else
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN2,
#endif
};
#endif

/*-----------------------------------------------------------------------------------------------*\
 * AR0239 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static AR0239_CTRL_s AR0239Ctrl[AR0239_NUM_VIN_CHANNEL] = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_GetNumActiveSensor
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
static UINT32 AR0239_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U;
    UINT32 i;

    for (i = 0U; i < AR0239_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            SensorNum ++;
        }
    }

    return (SensorNum > 1U) ? SensorNum : 1U;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_PrepareModeInfo
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
static void AR0239_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 SensorMode = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s *pInputInfo = &AR0239_InputInfo[SensorMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &AR0239_OutputInfo[SensorMode];
    const AR0239_SENSOR_INFO_s *pSensorInfo = &AR0239_SensorInfo[SensorMode];
    const AR0239_FRAME_TIMING_s *pFrameTime = &AR0239_ModeInfoList[SensorMode].FrameTime;
    UINT32 SensorNum = AR0239_GetNumActiveSensor(pChan->SensorID), RetVal;
    DOUBLE FloorVal;


    //pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;

    pModeInfo->FrameLengthLines = pSensorInfo->FrameLengthLines * SensorNum;
    if (AR0239_HdrInfo[SensorMode].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE)
    {
            pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines/2U;
    } else{
            pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    }
    pModeInfo->InternalExposureOffset = 0.0f;
    pModeInfo->RowTime = AR0239_ModeInfoList[SensorMode].RowTime / (FLOAT)SensorNum;
    RetVal = AmbaWrap_floor((((DOUBLE)AR0239_ModeInfoList[SensorMode].PixelRate * (DOUBLE)pModeInfo->RowTime) + 0.5), &FloorVal);
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    pModeInfo->LineLengthPck = (UINT32)FloorVal;
    pModeInfo->InputClk = pSensorInfo->InputClk;

    RetVal = AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    RetVal = AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    RetVal = AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    RetVal = AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    RetVal = AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    RetVal = AmbaWrap_memcpy(&pModeInfo->HdrInfo, &AR0239_HdrInfo[SensorMode], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }


    /* Updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        pModeInfo->MinFrameRate.NumUnitsInTick /= 8U;
    }
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_ConfigVin
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
static UINT32 AR0239_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s AR0239VinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
#if 1
            .ColorSpace         = AMBA_VIN_COLOR_SPACE_RGBIR,
            .BayerPattern       = AMBA_VIN_BAYER_PATTERN_GRIG,
#else
            .ColorSpace         = AMBA_VIN_COLOR_SPACE_RGB,
            .BayerPattern       = AMBA_VIN_BAYER_PATTERN_GR,
#endif
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
#if 1
        .DataType               = 0x20U,
        .DataTypeMask           = 0x1fU,
#else
        .DataType               = 0x00U,
        .DataTypeMask           = 0x3fU,
#endif
        .VirtChanHDREnable      = 0U,
        .VirtChanHDRConfig      = {0},
    };

    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s *pVinCfg = &AR0239VinConfig;
    UINT32 RetVal;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Config.NumDataBits = pImgInfo->NumDataBits;
    pVinCfg->Config.BayerPattern = pImgInfo->BayerPattern;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    RetVal = AmbaWrap_memcpy(&pVinCfg->Config.FrameRate, &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "NumActiveLanes=%d,NumDataBits=%d, NumActivePixels=%d, NumActiveLines=%d, NumTotalPixels=%d ",
    //                     pVinCfg->NumActiveLanes, pVinCfg->Config.NumDataBits, pVinCfg->Config.RxHvSyncCtrl.NumActivePixels, pVinCfg->Config.RxHvSyncCtrl.NumActiveLines, pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels);
    RetVal = AmbaVIN_MipiConfig(pChan->VinID, pVinCfg);

    if (RetVal == VIN_ERR_NONE) {
        RetVal = SENSOR_ERR_NONE;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaVIN_MipiConfig return %u", __LINE__, RetVal, 0U, 0U, 0U);
        RetVal = SENSOR_ERR_INVALID_API;
    }

    return RetVal;
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
static inline UINT32 AR0239_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaUserGPIO_SensorResetCtrl return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    if (RetVal == GPIO_ERR_NONE) {
        RetVal = AmbaKAL_TaskSleep(2);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaKAL_TaskSleep return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }
    if (RetVal == ERR_NONE) {
        RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaUserGPIO_SensorResetCtrl return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }
    if (RetVal == GPIO_ERR_NONE) {
        RetVal = AmbaKAL_TaskSleep(30);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaKAL_TaskSleep return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_RegRead
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
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0239_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pRxData)
{
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT8 RxData[2];
    UINT32 TxSize;
    UINT32 RetVal;

    if ((pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    }else{
        I2cTxConfig.SlaveAddr = AR0239_I2C_SLAVE_ADDRESS;
        I2cTxConfig.DataSize  = 2U;
        I2cTxConfig.pDataBuf  = TxData;
        TxData[0] = (UINT8)((Addr & 0xff00U) >> 8U);
        TxData[1] = (UINT8)(Addr & 0x00ffU);

        I2cRxConfig.SlaveAddr = AR0239_I2C_SLAVE_ADDRESS | 1U;
        I2cRxConfig.DataSize  = 2U;
        I2cRxConfig.pDataBuf  = RxData;
        RetVal = AmbaI2C_MasterReadAfterWrite(AR0239_I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,&I2cRxConfig, &TxSize, 1000U);
        *pRxData = (UINT16)RxData[0] << 8U;
        *pRxData |= (UINT16)RxData[1];

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX290_MIPI] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_RegWrite
 *
 *  @Description:: Write sensor registers
 *
 *  @Input      ::
 *      pChan:      Vin ID and sensor ID
 *      Addr:      Register Address
 *      pTxData:   Pointer to write data
 *      Size:      Number of registers to write
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0239_RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[4];
    UINT32 TxSize;
    UINT32 RetVal;
    /*UINT16 rData;*/
    if ((pChan == NULL)){
        RetVal = SENSOR_ERR_ARG;
    } else{
        I2cConfig.SlaveAddr = AR0239_I2C_SLAVE_ADDRESS;
        I2cConfig.DataSize = 4U;
        I2cConfig.pDataBuf = TxDataBuf;
        TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);
        TxDataBuf[2] = (UINT8)((Data & 0xff00U) >> 8U);
        TxDataBuf[3] = (UINT8)(Data & 0x00ffU);
        RetVal = AmbaI2C_MasterWrite(AR0239_I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,&I2cConfig, &TxSize, 1000U);
        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[AR0239] i2c chan %u SlaveID 0x%02x Addr 0x%02x Data 0x%02x I2C does not work!!!!!", AR0239_I2cChannel[pChan->VinID], I2cConfig.SlaveAddr, Addr, Data, 0U);
        } else{
            /*(void) AR0239_RegRead(pChan, Addr, &rData);*/
            /*AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,  "Addr=0x%04X, [Write]Data=0x%04X, [Read]Data=%d", Addr, Data, rData, 0U, 0U);*/
        }
    }
    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_RegisterRead
 *
 *  @Description:: Read sensor registers
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      Addr:   Register address
 *
 *  @Output     ::
 *      pData:  Pointer to Read data buffer
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0239_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{
    UINT16 RData = 0xFFFFU;
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AR0239_RegRead(pChan, Addr, &RData);
        *pData = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_RegisterWrite
 *
 *  @Description:: Write sensor registers
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0239_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AR0239_RegWrite(pChan, Addr, Data);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_SetAGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      PGC:    Programmable gain control setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0239_SetAGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl)
{
    UINT16 R3100Val, R3060Val;
    UINT32 AgcIdx = (AnalogGainCtrl & 0xffffU);
    UINT32 RetVal;

    RetVal = AR0239_RegRead(pChan, 0x3100, &R3100Val);

    if (RetVal == ERR_NONE) {
        if (AR0239_AgcRegTable[AgcIdx].GainConv == AR0239_GAIN_CONVERT_LOW) {
            R3100Val = R3100Val & 0xFFFBU;  /* Enable LCG */
        } else {
            R3100Val = R3100Val | 0x04U;    /* Enable HCG */
        }
        RetVal = AR0239_RegWrite(pChan, 0x3100, R3100Val);
    }
    /* Analog gain */

    if (RetVal == ERR_NONE) {
        R3060Val = ((UINT16)(AR0239_AgcRegTable[AgcIdx].HData << 4U)) | AR0239_AgcRegTable[AgcIdx].LData;
        RetVal = AR0239_RegWrite(pChan, 0x3060, R3060Val);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_SetShutterReg
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
static UINT32 AR0239_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ShutterCtrl)
{
    UINT16 Data = (UINT16)ShutterCtrl;
    UINT32 RetVal;

    RetVal = AR0239_RegWrite(pChan, 0x3012, Data);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_GetHdrInfo
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
static UINT32 AR0239_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT16 SEFShtCtrl;// = (UINT16)pShutterCtrl[1];

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        SEFShtCtrl = (UINT16)pShutterCtrl[1];
        pModeInfo = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE)
        {
            AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].EffectiveArea.StartY = SEFShtCtrl+2U;
            RetVal = AmbaWrap_memcpy(pHdrInfo, &AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        } else {
            RetVal = AmbaWrap_memcpy(pHdrInfo, &AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_SetStandbyOn
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
static UINT32 AR0239_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = AR0239_RegWrite(pChan, 0x301A, 0x0001);
        RetVal = AR0239_RegWrite(pChan, 0x301A, 0x2058);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_SetStandbyOff
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
static UINT32 AR0239_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        #ifdef AR0239_IN_SLAVE_MODE
            RetVal = AR0239_RegWrite(pChan, 0x30CE, 0x0120);
            if (RetVal == ERR_NONE) {
                RetVal = AR0239_RegWrite(pChan, 0x301A, 0x215C);
            }
        #else
            RetVal = AR0239_RegWrite(pChan, 0x301A, 0x205C);
        #endif
    }

    return RetVal;
}

#ifdef AR0239_IN_SLAVE_MODE
 /*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_ConfigMasterSync
 *
 *  @Description:: Configure Master Sync generation
 *
 *  @Input      ::
 *      pFrameTime: Sensor frame time configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0239_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
     UINT32 RetVal;
    DOUBLE PeriodInDb;
    const AR0239_MODE_INFO_s *pModeInfo = &AR0239_ModeInfoList[ModeID];
    const AR0239_SENSOR_INFO_s *pSensorInfo = &AR0239_SensorInfo[ModeID];
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncConfig = {0};

    RetVal = AmbaWrap_floor(((DOUBLE)pModeInfo->FrameTime.FrameRate.NumUnitsInTick * ((DOUBLE) AR0239_SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameTime.FrameRate.TimeScale)) + 0.5, &PeriodInDb);
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    if (RetVal == ERR_NONE) {
        MasterSyncConfig.RefClk = pSensorInfo->InputClk;
        MasterSyncConfig.HSync.Period      = (UINT32)PeriodInDb;
        MasterSyncConfig.HSync.PulseWidth  = 8U;
        MasterSyncConfig.HSync.Polarity    = 1U;
        MasterSyncConfig.VSync.Period      = 1U;
        MasterSyncConfig.VSync.PulseWidth  = 1000U;
        MasterSyncConfig.VSync.Polarity    = 1U;
        MasterSyncConfig.ToggleHsyncInVblank  = 1U;

        RetVal = AmbaVIN_MasterSyncEnable(AR0239MasterSyncChannel[pChan->VinID], &MasterSyncConfig);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaVIN_MasterSyncEnable vin %u sync_chan %u return %u", __LINE__, pChan->VinID, AR0239MasterSyncChannel[pChan->VinID], RetVal, 0U);
        }
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "MasterSync RefClk(%u) Hsync Period(%u)", pSensorInfo->InputClk, (UINT32)PeriodInDb, 0U, 0U, 0U);
    }
    return RetVal;
}
#endif

static UINT32 AR0239_ConfigLinearSeq(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;

    for (UINT32 i = 0U; i < AR0239_NUM_LINEAR_SEQ_REG; i++) {
        if (RetVal == SENSOR_ERR_NONE) {
            Addr = AR0239_LinearSeqRegTable[i].Addr;
            Data = AR0239_LinearSeqRegTable[i].Data;
            RetVal = AR0239_RegWrite(pChan, Addr, Data);
        }
    }

    return RetVal;
}

static UINT32 AR0239_ConfigRecomEsSeq(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;

    for (UINT32 i = 0U; i < AR0239_NUM_RECOM_ES_REG; i++) {
        if (RetVal == SENSOR_ERR_NONE) {
            Addr = AR0239_RecomESRegTable[i].Addr;
            Data = AR0239_RecomESRegTable[i].Data;
            RetVal = AR0239_RegWrite(pChan, Addr, Data);
        }
    }

    return RetVal;
}

static UINT32 AR0239_ConfigPllSeq(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;

    for (UINT32 i = 0U; i < AR0239_NUM_PLL_REG; i++) {
        if (RetVal == SENSOR_ERR_NONE) {
            Addr = AR0239_PllRegTable[i].Addr;
            Data = AR0239_PllRegTable[i].Data;
            RetVal = AR0239_RegWrite(pChan, Addr, Data);
        }
    }

    return RetVal;
}
static UINT32 AR0239_CFPNImp(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;

    for (UINT32 i = 0U; i < AR0239_NUM_CFPN_Impr_REG; i++) {
        if (RetVal == SENSOR_ERR_NONE) {
            Addr = AR0239_CFPNImpTable[i].Addr;
            Data = AR0239_CFPNImpTable[i].Data;
            RetVal = AR0239_RegWrite(pChan, Addr, Data);
        }
    }

    return RetVal;
}
static UINT32 AR0239_HDRSeq(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;

    for (UINT32 i = 0U; i < AR0239_NUM_HDR_Seq_REG; i++) {
        if (RetVal == SENSOR_ERR_NONE) {
            Addr = AR0239_HDRSeqTable[i].Addr;
            Data = AR0239_HDRSeqTable[i].Data;
            RetVal = AR0239_RegWrite(pChan, Addr, Data);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_ChangeReadoutMode
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
static UINT32 AR0239_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 i;
    UINT16 Addr;
    UINT16 Data;
    UINT32 RetVal = SENSOR_ERR_NONE;

    for (i = 0U; i < AR0239_NUM_READOUT_MODE_REG; i++) {
        if (RetVal == SENSOR_ERR_NONE) {
            Addr = AR0239_ModeRegTable[i].Addr;
            Data = AR0239_ModeRegTable[i].Data[SensorMode];
            RetVal = AR0239_RegWrite(pChan, Addr, Data);
        }
    }

    for (i = 0U; i < AR0239_NUM_MAX_SENSOR_COUNT; i++) {
        //if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
            /* reset gain/shutter ctrl information */
            AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][0] = 0xffffffffU;
            AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][1] = 0xffffffffU;
            AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = 0xffffffffU;
            AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][1] = 0xffffffffU;
            AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][2] = 0xffffffffU;
            AR0239Ctrl[pChan->VinID].CurrentDgcCtrl[i][0] = 0U;
            AR0239Ctrl[pChan->VinID].CurrentDgcCtrl[i][1] = 0U;
            AR0239Ctrl[pChan->VinID].CurrentDgcCtrl[i][2] = 0U;
        //}
    }

    return RetVal;
}

static UINT32 AR0239_SensorModeInit(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 RetVal;

    if (SensorMode == AR0239_1920_1080_30P)
    {
        RetVal = AR0239_ConfigLinearSeq(pChan);
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_ConfigRecomEsSeq(pChan);
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_ConfigPllSeq(pChan);
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_ChangeReadoutMode(pChan, SensorMode);
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x3082, 0x0001);/* ERS Linear Mode */
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x318E, 0x0000);/* HDR_MC_CTRL3 */
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x31D0, 0x0000); /* COMPANDING DISABLE */
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x3064, 0x1802); /* Embedded off */
        }
    } else {
        RetVal = AR0239_CFPNImp(pChan);
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_HDRSeq(pChan);
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_ConfigRecomEsSeq(pChan);
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_ConfigPllSeq(pChan);
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_ChangeReadoutMode(pChan, SensorMode);
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x3082, 0x4808);/* HDR 2exp arbitrary T1 and T2*/
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x318E, 0x9000);/* HDR_MC_CTRL3*/
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x3100, 0x0000);/* AECTRLREG*/
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x3786, 0x6204);/* num_exp_max=2*/
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x31D0, 0x0000); /* COMPANDING DISABLE */
        }
        if (RetVal == SENSOR_ERR_NONE) {
            RetVal = AR0239_RegWrite(pChan, 0x3064, 0x1802); /* Embedded off */
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0239_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;
    const AR0239_SENSOR_INFO_s *pSensorInfo = NULL;

    for (i = 0U; i < AR0239_NUM_MODE; i++) {
        pSensorInfo = &AR0239_SensorInfo[i];
        AR0239_ModeInfoList[i].RowTime = (FLOAT) ((DOUBLE) pSensorInfo->LineLengthPck /
                                                               ( (DOUBLE) pSensorInfo->DataRate *
                                                                 (DOUBLE) pSensorInfo->NumDataLanes /
                                                                 (DOUBLE) pSensorInfo->NumDataBits) );
        pOutputInfo = &AR0239_OutputInfo[i];
        AR0239_ModeInfoList[i].PixelRate = (FLOAT) ((DOUBLE) pOutputInfo->DataRate *
                                                                 (DOUBLE) pOutputInfo->NumDataLanes /
                                                                 (DOUBLE) pOutputInfo->NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_Init
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
static UINT32 AR0239_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const AR0239_SENSOR_INFO_s *pSensorInfo = &AR0239_SensorInfo[0];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* default clk */
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pSensorInfo->InputClk);
        AR0239_PreCalculateModeInfo();
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_Enable
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
static UINT32 AR0239_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AR0239_Enable", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_Disable
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
static UINT32 AR0239_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AR0239_StandbyOn(pChan);
        if (RetVal == OK) {
            RetVal = AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        }
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AR0239_Disable", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_GetStatus
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
static UINT32 AR0239_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal;

    if ((pChan == NULL) || (pStatus == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaWrap_memcpy(pStatus, &AR0239Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    if(RetVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_GetModeInfo
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
static UINT32 AR0239_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = AR0239Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if (Config.ModeID >= AR0239_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else{
            AR0239_PrepareModeInfo(pChan, &Config, pModeInfo);
            RetVal = SENSOR_ERR_NONE;
        }
    }
    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_GetDeviceInfo
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
static UINT32 AR0239_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal;

    if ((pChan == NULL) ||(pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaWrap_memcpy(pDeviceInfo, &AR0239_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    if(RetVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_GetCurrentGainFactor
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
static UINT32 AR0239_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;
    UINT32 CurrentAgcIdx;
    UINT32 RetVal = SENSOR_ERR_NONE;
    (void)pModeInfo;
    if ((pChan == NULL) ||(pGainFactor == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;

        //if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    CurrentAgcIdx = (AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] & 0xFFFFU);
                    *pGainFactor = AR0239_AgcRegTable[CurrentAgcIdx].Factor;
                }
            }

    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_GetCurrentShutterSpeed
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
static UINT32 AR0239_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 i,k;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    pExposureTime[0] = pModeInfo->RowTime * (FLOAT)AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][0];
                }
            }
        } else {
            for (i = 0U; i < AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (k = 0U; k < (pModeInfo->HdrInfo.ActiveChannels); k ++) {
                        pExposureTime[k] =(FLOAT)AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][k]* pModeInfo->RowTime*(FLOAT)(2U);
                    }
                }
            }
        }
    }
    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_ConvertGainFactor
 *
 *  @Description:: Convert gain factor to analog and digital gain control
 *
 *  @Input      ::
 *      pChan:               Vin ID and sensor ID
 *      DesiredFactor:      Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      Achievable gain factor (12.20 fixed point)
 *      pAnalogGainCtrl:    Analog gain control for achievable gain factor
 *      pDigitalGainCtrl:   Digital gain control for achievable gain factor
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0239_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT AgcGain = 0.0f;
    FLOAT DgcGainCtrl = 0.0f;
    FLOAT MinGain = AR0239_AgcRegTable[0].Factor;
    FLOAT MaxGain = AR0239_AgcRegTable[AR0239_NUM_AGC_STEP - 1U].Factor;// * AR0239_DeviceInfo.MaxDigitalGainFactor;
    FLOAT DesiredFactor =  pDesiredFactor->Gain[0];
    UINT32 AgcTableIdx;
    FLOAT TotalAgain = 0.0f;

    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE)
        {

            if (DesiredFactor < MinGain) {
                DesiredFactor = MinGain;
            } else if (DesiredFactor > MaxGain) {
                DesiredFactor = MaxGain;
            } else {
                /* Do nothing */
            }
            for (AgcTableIdx = 0; AgcTableIdx < (AR0239_NUM_AGC_STEP - 1U); AgcTableIdx ++) {
                TotalAgain = AR0239_AgcRegTable[AgcTableIdx + 1U].Factor;
                if(DesiredFactor < TotalAgain){
                    break;
                }
            }
            AgcGain = AR0239_AgcRegTable[AgcTableIdx].Factor;

                DgcGainCtrl = 128.0f;
            pGainCtrl->AnalogGain[0] = ((UINT32)DgcGainCtrl << 16U) + AgcTableIdx; //[31:16] DgcGain + AgcTableIdx
            pActualFactor->Gain[0] = AgcGain * (DgcGainCtrl / 128.0f);
        }else{
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE)
            {


                    if (DesiredFactor < MinGain) {
                         DesiredFactor = MinGain;
                    } else if (DesiredFactor > MaxGain) {
                        DesiredFactor = MaxGain;
                    } else {
                        /* Do nothing */
                    }
                    for (AgcTableIdx = 0; AgcTableIdx < (AR0239_NUM_AGC_STEP - 1U); AgcTableIdx ++) {
                        TotalAgain = AR0239_AgcRegTable[AgcTableIdx + 1U].Factor;
                        if(DesiredFactor < TotalAgain){
                            break;
                        }
                    }
                    AgcGain = AR0239_AgcRegTable[AgcTableIdx].Factor;

                    DgcGainCtrl = 128.0f;
                    pGainCtrl->AnalogGain[0] = ((UINT32)DgcGainCtrl << 16U) + AgcTableIdx; //[31:16] DgcGain + AgcTableIdx
                    pActualFactor->Gain[0] = AgcGain * (DgcGainCtrl / 128.0f);
                    pGainCtrl->AnalogGain[1] = ((UINT32)DgcGainCtrl << 16U) + AgcTableIdx; //[31:16] DgcGain + AgcTableIdx
                    pActualFactor->Gain[1] = AgcGain * (DgcGainCtrl / 128.0f);
                    pGainCtrl->AnalogGain[2] = ((UINT32)DgcGainCtrl << 16U) + AgcTableIdx; //[31:16] DgcGain + AgcTableIdx
                    pActualFactor->Gain[2] = AgcGain * (DgcGainCtrl / 128.0f);
               // }

            }
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
static void ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 ModeID = AR0239Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    UINT32 NumExposureStepPerFrame = AR0239Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame;
    UINT32 MinShrWidth;
    UINT32 MaxShrWidth;
    UINT32 ExposureFrames;
    FLOAT  ShutterCtrl;

    if ((ModeID == AR0239_1920_1080_30P_HDR) || (ModeID == AR0239_1920_1080_A30P_HDR)) {
        ShutterCtrl = ExposureTime / (AR0239_ModeInfoList[ModeID].RowTime*(FLOAT)(2U));
    } else{
        ShutterCtrl = ExposureTime / (AR0239_ModeInfoList[ModeID].RowTime);
    }
    *pShutterCtrl = (UINT32)ShutterCtrl;
    if (*pShutterCtrl > ((0xffffU / NumExposureStepPerFrame) * NumExposureStepPerFrame)) {
        *pShutterCtrl = ((0xffffU / NumExposureStepPerFrame) * NumExposureStepPerFrame);
    }
    ExposureFrames = *pShutterCtrl / NumExposureStepPerFrame;
    /*ExposureFrames = (*pShutterCtrl % NumExposureStepPerFrame) ? (ExposureFrames + 1U) : (ExposureFrames);*/
    if ((*pShutterCtrl % NumExposureStepPerFrame) > 0U) {
        ExposureFrames = ExposureFrames + 1U;
    }
    /* limit exp not over 1 frame time */
    if (ExposureFrames > 1U) {
        ExposureFrames = 1U;
    }
    /* limit exp. setting to a valid value */
    if(ModeID == AR0239_1920_1080_30P){
        MaxShrWidth = (ExposureFrames * NumExposureStepPerFrame) - 1U;
        MinShrWidth = 1U;
    } else{
        if (HdrChannel == 1U) {
            MinShrWidth = AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].MinExposureLine;
            MaxShrWidth = AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].MaxExposureLine;
        } else{
            MinShrWidth = AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[0].MinExposureLine;
            MaxShrWidth = AR0239Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[0].MaxExposureLine;
        }
    }
    if(*pShutterCtrl > MaxShrWidth) {
        *pShutterCtrl = MaxShrWidth;
    }
    if(*pShutterCtrl < MinShrWidth) {
        *pShutterCtrl = MinShrWidth;
    }
    if(ModeID == AR0239_1920_1080_30P){
        *pActualExptime = (FLOAT)*pShutterCtrl * AR0239_ModeInfoList[ModeID].RowTime;
    } else{
        *pActualExptime = (FLOAT)*pShutterCtrl * AR0239_ModeInfoList[ModeID].RowTime*(FLOAT)(2U);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_ConvertShutterSpeed
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
static UINT32 AR0239_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 j;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;

        /* None-HDR mode */
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pChan, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else {
            /* HDR mode */
            for (j = 0U; j < (pModeInfo->HdrInfo.ActiveChannels); j ++) {
                        ConvertShutterSpeed(pChan, j, pDesiredExposureTime[j], &pActualExposureTime[j], &pShutterCtrl[j]);

            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_SetAnalogGainCtrl
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
static UINT32 AR0239_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 i,j;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal;
    (void)pModeInfo;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;

        /* None-HDR mode */
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = AR0239_SetAGainReg(pChan, pAnalogGainCtrl[0]);
            /* Update current analog gain control */
            for (i = 0U; i < AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = pAnalogGainCtrl[0];
                }
            }
        } else {
            RetVal = AR0239_SetAGainReg(pChan, pAnalogGainCtrl[0]);
            for (i = 0U; i < AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                /*if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {*/
                for (j=0U; j<(pModeInfo->HdrInfo.ActiveChannels); j ++){
                    AR0239Ctrl[pChan->VinID].CurrentAgcCtrl[i][j] = pAnalogGainCtrl[0];
                }/*}*/
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
static UINT32 AR0239_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 i,j;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 SFData, Data ;
    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;

        /* None-HDR mode */
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = AR0239_SetShutterReg(pChan, pShutterCtrl[0]);

            for (i = 0U; i < AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][0] = pShutterCtrl[0];
                }
            }
        } else {
            /* HDR mode */
            SFData = (UINT16)(pShutterCtrl[1]);
            Data = (UINT16)(pShutterCtrl[0]);

            RetVal = AR0239_RegWrite(pChan, 0x3022, 0x01);
            if (RetVal == ERR_NONE) {
                RetVal = AR0239_RegWrite(pChan, 0x3212, SFData);
            }
            if (RetVal == ERR_NONE) {
                RetVal = AR0239_RegWrite(pChan, 0x3012, Data);
            }
            if (RetVal == ERR_NONE) {
                RetVal = AR0239_RegWrite(pChan, 0x3022, 0x00);

                for (i = 0U; i < AR0239_NUM_MAX_SENSOR_COUNT; i++) {
                    //if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                        for (j = 0U; j < (pModeInfo->HdrInfo.ActiveChannels); j ++){
                            AR0239Ctrl[pChan->VinID].CurrentShrCtrl[i][j] = pShutterCtrl[j];
                        }
                    //}
                }
            }

        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_SetSlowShutterCtrl
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
static UINT32 AR0239_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    (void)pChan;
    (void)SlowShutterCtrl;

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0239_Config
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
static UINT32 AR0239_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s AR0239PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &AR0239Ctrl[pChan->VinID].Status.ModeInfo;
    UINT16 SensorMode = (UINT16)pMode->ModeID;
    AR0239_FRAME_TIMING_s *pFrameTime = &AR0239Ctrl[pChan->VinID].FrameTime;
    UINT32 RetVal;

    if (SensorMode >= AR0239_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "============  [ AR0239 Config ]  ============", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d ============", SensorMode, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

        /* update status */
        AR0239_PrepareModeInfo(pChan, pMode, pModeInfo);
        RetVal = AmbaWrap_memcpy(pFrameTime, &AR0239_ModeInfoList[SensorMode].FrameTime, sizeof(AR0239_FRAME_TIMING_s));
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }

#ifdef AR0239_IN_SLAVE_MODE
        if (RetVal == ERR_NONE) {
            RetVal = AmbaVIN_MasterSyncDisable(AR0239MasterSyncChannel[pChan->VinID]);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaVIN_MasterSyncDisable return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
#endif

        /* Adjust mipi-phy parameters */
        if (RetVal == ERR_NONE) {
            AR0239PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", (UINT32)AR0239PadConfig.DateRate, 0U, 0U, 0U, 0U);
            RetVal = AmbaVIN_MipiReset(pChan->VinID, &AR0239PadConfig);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AmbaVIN_MipiReset vin %u return %u", pChan->VinID, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaKAL_TaskSleep(3);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaKAL_TaskSleep vin %u return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = AR0239_HardwareReset(pChan->VinID);
            /* clock for target mode */
            RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  InputClk: %d  ============", pModeInfo->InputClk, 0U, 0U, 0U, 0U);
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaKAL_TaskSleep(3);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaKAL_TaskSleep vin %u return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            //AR0239_HardwareReset(pChan->VinID);
            /* sw reset */
#ifdef AR0239_IN_SLAVE_MODE
            RetVal = AR0239_RegWrite(pChan, 0x301A, 0x2058);
#else
            RetVal = AR0239_RegWrite(pChan, 0x301A, 0x0001);
#endif
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaKAL_TaskSleep(20);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaKAL_TaskSleep vin %u return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = AR0239_SensorModeInit(pChan, SensorMode);
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  SensorMode: %d  ============", SensorMode, 0U, 0U, 0U, 0U);
        }
        if (RetVal == ERR_NONE) {
            RetVal = AR0239_StandbyOff(pChan); /* Stream on */
        }
#ifdef AR0239_IN_SLAVE_MODE
        if (RetVal == ERR_NONE) {
            RetVal = AR0239_ConfigMasterSync(pChan, SensorMode);
        }
#endif
        if (RetVal == ERR_NONE) {
            RetVal = AR0239_ConfigVin(pChan, pModeInfo);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_AR0239Obj = {
    .SensorName             = "AR0239",
    .SerdesName             = "NA",
    .Init                   = AR0239_Init,
    .Enable                 = AR0239_Enable,
    .Disable                = AR0239_Disable,
    .Config                 = AR0239_Config,

    .GetStatus              = AR0239_GetStatus,
    .GetModeInfo            = AR0239_GetModeInfo,
    .GetDeviceInfo          = AR0239_GetDeviceInfo,
    .GetHdrInfo             = AR0239_GetHdrInfo,
    .GetCurrentGainFactor   = AR0239_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = AR0239_GetCurrentShutterSpeed,

    .ConvertGainFactor      = AR0239_ConvertGainFactor,
    .ConvertShutterSpeed    = AR0239_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = AR0239_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = NULL, //IMX290_MIPI_SetDigitalGainCtrl,
    .SetShutterCtrl         = AR0239_SetShutterCtrl,
    .SetSlowShutterCtrl     = AR0239_SetSlowShutterCtrl,

    .RegisterRead           = AR0239_RegisterRead,
    .RegisterWrite          = AR0239_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
