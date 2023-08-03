/**
 *  @file AmbaSensor_IMX390.c
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
 *  @details Control APIs of SONY IMX390 CMOS sensor with MIPI interface
 *
 */

#include ".svc_autogen"

#include <AmbaWrap.h>
#include "AmbaMisraFix.h"

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX390.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "bsp.h"

//#define DEBUG_MSG //enable if when need to print all sensor setting
//#define IMX390_SHARING_MASTER_SYNC
#define IMX390_IN_SLAVE_MODE
#define IMX390_I2C_WR_BUF_SIZE 64
#if !defined(CONFIG_SOC_H22)
static UINT32 IMX390_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);
#endif
static UINT32 IMX390I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
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

#ifdef IMX390_IN_SLAVE_MODE
static UINT32 IMX390MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
#ifdef IMX390_SHARING_MASTER_SYNC
    // TODO: use the same AMBA_SENSOR_MSYNC_CHANNEL on HW to make different VIN start together.
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL3]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL4]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL5]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL6]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL7]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL8]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL9]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL10] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL11] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL12] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL13] = AMBA_SENSOR_MSYNC_CHANNEL,
#else
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN2,
    [AMBA_VIN_CHANNEL3]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN3,
    [AMBA_VIN_CHANNEL4]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN4,
    [AMBA_VIN_CHANNEL5]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN5,
    [AMBA_VIN_CHANNEL6]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN6,
    [AMBA_VIN_CHANNEL7]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN7,
    [AMBA_VIN_CHANNEL8]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN8,
    [AMBA_VIN_CHANNEL9]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN9,
    [AMBA_VIN_CHANNEL10] = AMBA_SENSOR_MSYNC_CHANNEL_VIN10,
    [AMBA_VIN_CHANNEL11] = AMBA_SENSOR_MSYNC_CHANNEL_VIN11,
    [AMBA_VIN_CHANNEL12] = AMBA_SENSOR_MSYNC_CHANNEL_VIN12,
    [AMBA_VIN_CHANNEL13] = AMBA_SENSOR_MSYNC_CHANNEL_VIN13,
#endif
};
#endif

/*-----------------------------------------------------------------------------------------------*\
 * IMX390 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX390_CTRL_s IMX390Ctrl[IMX390_NUM_VIN_CHANNEL] = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_GetNumActiveSensor
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
static UINT32 IMX390_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
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
 *  @RoutineName:: IMX390_PrepareModeInfo
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
static void IMX390_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                               = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s              *pInputInfo     = &IMX390_InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s             *pOutputInfo    = &IMX390_OutputInfo[ModeID];
    const IMX390_SENSOR_INFO_s     *pSensorInfo    = &IMX390_SensorInfo[ModeID];
    const IMX390_FRAME_TIMING_s    *pFrameTime     = &IMX390ModeInfoList[ModeID].FrameTime;
    UINT32 SensorNum = IMX390_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;
    UINT32 RetVal;

    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)SensorNum  %u", __LINE__, SensorNum, 0U, 0U, 0U);
    if (SensorNum != 0U) {
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)FrameLengthLines  %u", __LINE__, pSensorInfo->FrameLengthLines, 0U, 0U, 0U);
        pModeInfo->FrameLengthLines         = pSensorInfo->FrameLengthLines * SensorNum;
        pModeInfo->NumExposureStepPerFrame  = pSensorInfo->FrameLengthLines;
        pModeInfo->InternalExposureOffset   = 0.0f;
        pModeInfo->RowTime = IMX390ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
        RetVal = AmbaWrap_floor((((DOUBLE)IMX390ModeInfoList[ModeID].PixelRate * (DOUBLE)SensorNum) * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
        if (RetVal == ERR_NONE) {
            pModeInfo->LineLengthPck            = (UINT32)FloorVal;
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)LineLengthPck  %u", __LINE__, pModeInfo->LineLengthPck , 0U, 0U, 0U);
            pModeInfo->InputClk                 = pSensorInfo->InputClk;
            RetVal = AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX390_HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }

        /* update for MAX9296 CSI-2 output */
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
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_ConfigVin
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
static UINT32 IMX390_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 Rval;
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s IMX390VinConfig = {
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
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &IMX390VinConfig;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)NumDataBits  %u", __LINE__, pOutputInfo->NumDataBits, 0U, 0U, 0U);
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;

    Rval = AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    if (Rval != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, Rval, 0U, 0U, 0U);
    }
    if (Rval == ERR_NONE) {
        Rval = AmbaVIN_MipiConfig(VinID, pVinCfg);
        if (Rval != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaVIN_MipiConfig return %u", __LINE__, Rval, 0U, 0U, 0U);
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_ResetSensor
 *
 *  @Description:: Reset IMX390 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 IMX390_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_SensorResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
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
        RetVal = AmbaUserGPIO_SensorResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
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

//TBD: boardcast only and not support burst write
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
static UINT32 RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[IMX390_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;
    #ifdef DEBUG_MSG
    UINT32 k;
    #endif

    if ((pChan == NULL) || (Size > IMX390_SENSOR_I2C_MAX_SIZE)) {
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
        RetVal = AmbaI2C_MasterWrite(IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                            &I2cConfig, &TxSize, 1000U);
#endif

        I2cConfig.SlaveAddr = IMX390_I2C_SLAVE_ADDRESS;
        /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
        RetVal = AmbaI2C_MasterWrite(IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                            &I2cConfig, &TxSize, 1000U);
        #ifdef DEBUG_MSG
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Broadcast] Slave ID 0x%2x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U);
        for (k = 0U; k < Size; k++) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
        }
        #endif

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] i2c chan %d slave 0x%x Addr 0x%04x Size %d I2C does not work!!!!!", IMX390I2cChannel[pChan->VinID], IMX390_I2C_SLAVE_ADDRESS, Addr, Size, 0U);
        }
    }

    //SENSOR_DBG_IO("[IMX390][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_RegisterWrite
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
static UINT32 IMX390_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
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

static UINT32 IMX390_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.DataSize  = 1U;
    I2cRxConfig.pDataBuf  = pRxData;

    I2cTxConfig.SlaveAddr = IMX390_I2C_SLAVE_ADDRESS;
    I2cRxConfig.SlaveAddr = (IMX390_I2C_SLAVE_ADDRESS | (UINT32)1U);

/*    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x, 0x%x", I2cTxConfig.SlaveAddr, I2cRxConfig.SlaveAddr, 0U, 0U, 0U); */
    RetVal = AmbaI2C_MasterReadAfterWrite(IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                 &I2cRxConfig, &TxSize, 1000U);
    if (RetVal != I2C_ERR_NONE) {
        RetVal = SENSOR_ERR_COMMUNICATE;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] Addr 0x%04x I2C does not work!!!!!", Addr, 0U, 0U, 0U, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_RegisterRead
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
static UINT32 IMX390_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX390_RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetAnalogGainReg
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
static UINT32 IMX390_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl)
{
    UINT32 RetVal;
    UINT8 WData[2];

    /* SP1L */
    WData[0] = (UINT8)(AnalogGainCtrl & 0xffU);
    WData[1] = (UINT8)((AnalogGainCtrl >> 8U) & 0x7U);
    RetVal = RegWrite(pChan, IMX390_AGAIN_SP1L, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetDigitalGainReg
 *
 *  @Description:: Configure sensor conversion gain setting
 *
 *  @Input      ::
 *      Chan:      Vin ID and sensor ID
 *      CG:        Conversion gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX390_SetDigitalGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 DigitalGainCtrl)
{
    UINT32 RetVal;
    UINT8 WData[2];

    /* SP1L */
    WData[0] = (UINT8)(DigitalGainCtrl & 0xffU);
    WData[1] = (UINT8)((DigitalGainCtrl >> 8U) & 0x7U);
    RetVal = RegWrite(pChan, IMX390_PGA_SP1H, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetWbGainReg
 *
 *  @Description:: Configure sensor wb gain setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pWbGainCtrl:     Digital gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX390_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[8];

    WData[0] = (UINT8)(pWbGainCtrl[0].R & 0xffU);
    WData[1] = (UINT8)((pWbGainCtrl[0].R >> 8U) & 0xfU);
    WData[2] = (UINT8)(pWbGainCtrl[0].Gr & 0xffU);
    WData[3] = (UINT8)((pWbGainCtrl[0].Gr >> 8U) & 0xfU);
    WData[4] = (UINT8)(pWbGainCtrl[0].Gb & 0xffU);
    WData[5] = (UINT8)((pWbGainCtrl[0].Gb >> 8U) & 0xfU);
    WData[6] = (UINT8)(pWbGainCtrl[0].B & 0xffU);
    WData[7] = (UINT8)((pWbGainCtrl[0].B >> 8U) & 0xfU);
    RetVal |= RegWrite(pChan, IMX390_WBGAIN_R, WData, 8);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetShutterReg
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
static UINT32 IMX390_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 ShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHS1;
    UINT8 WData[4];

    /* SP1H & SP1L */
    SHS1 = IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - ShutterCtrl;

    WData[0] = (UINT8)(SHS1 & 0xffU);
    WData[1] = (UINT8)((SHS1 >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHS1 >> 16U) & 0xfU);
    RetVal |= RegWrite(pChan, IMX390_SHS1, WData, 3);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetHdrAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pAnalogGainCtrl: Pointer to analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX390_SetHdrAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[2];

    /* SP1H */
    WData[0] = (UINT8)(pAnalogGainCtrl[0] & 0xffU);
    WData[1] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x7U);
    RetVal |= RegWrite(pChan, IMX390_AGAIN_SP1H, WData, 2);

    /* SP1L */
    WData[0] = (UINT8)(pAnalogGainCtrl[1] & 0xffU);
    WData[1] = (UINT8)((pAnalogGainCtrl[1] >> 8U) & 0x7U);
    RetVal |= RegWrite(pChan, IMX390_AGAIN_SP1L, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetHdrShutterReg
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
static UINT32 IMX390_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHSX;
    UINT8 WData[4];

    /* SP1H & SP1L */
    SHSX = IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - pShutterCtrl[0];

    WData[0] = (UINT8)(SHSX & 0xffU);
    WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHSX >> 16U) & 0xfU);
    RetVal |= RegWrite(pChan, IMX390_SHS1, WData, 3);

    /* SP2 */
    SHSX = IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - pShutterCtrl[2];

    WData[0] = (UINT8)(SHSX & 0xffU);
    WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHSX >> 16U) & 0xfU);
    RetVal |= RegWrite(pChan, IMX390_SHS2, WData, 3);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetSlowShutterReg
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
static UINT32 IMX390_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    AmbaMisra_TouchUnused(&IntegrationPeriodInFrame);
    return SENSOR_ERR_NONE;
#if 0
    UINT32 TargetFrameLengthLines;
    UINT32 ModeID = IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData[1];

    if (IntegrationPeriodInFrame < 1U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TargetFrameLengthLines = IMX390ModeInfoList[ModeID].FrameTime.VMAX * IntegrationPeriodInFrame;

        if (TargetFrameLengthLines > 0x3ffffU) {
            TargetFrameLengthLines = 0x3ffffU;
        }

        TxData[0] = IntegrationPeriodInFrame - 1;

        RetVal |= RegWrite(pChan, IMX390_FMAX, TxData, 1U);

        /* Update frame rate information */
        IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale = IMX390ModeInfoList[ModeID].FrameTime.FrameRate.TimeScale / IntegrationPeriodInFrame;
    }

    return RetVal;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_StandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX390_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x01;
    UINT32 RetVal;

    RetVal = RegWrite(pChan, IMX390_STANDBY, &TxData, 1);
//    AmbaPrint("[IMX390] IMX390_StandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_StandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX390_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x00;
    UINT32 RetVal;

    RetVal = RegWrite(pChan, IMX390_STANDBY, &TxData, 1);

    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * IMX390Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / IMX390Ctrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[IMX390] IMX390_StandbyOff");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_Query_VerID
 *
 *  @Description:: Check sensor chip version
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pVerID:    pointer to IMX390 version ID
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX390_Query_VerID(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 *pVerID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 Reg0x3060 = 0xffU, Reg0x3067 = 0xffU, Reg0x3064 = 0xffU;

    if (pVerID == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        RetVal |= IMX390_RegRead(pChan, 0x3060U, &Reg0x3060);
        RetVal |= IMX390_RegRead(pChan, 0x3067U, &Reg0x3067);
        RetVal |= IMX390_RegRead(pChan, 0x3064U, &Reg0x3064);

        if (RetVal == SENSOR_ERR_NONE) {

            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] Reg0x3060: 0x%02x, Reg0x3067: 0x%02x, Reg0x3064: 0x%02x", Reg0x3060, Reg0x3067, Reg0x3064, 0U, 0U);

            if ((Reg0x3060 == 0x70U) && ((Reg0x3067 >> 4U) == 0x5U)) {
                *pVerID = IMX390_CHIP_VERSION_60DEG;
            } else if ((Reg0x3060 == 0x70U) || (Reg0x3060 == 0x71U)) {
                *pVerID = IMX390_CHIP_VERSION_CASE5;
            } else if (((Reg0x3060 == 0x01U) && (Reg0x3067 == 0x10U)) || ((Reg0x3060 == 0x00U) && (Reg0x3067 == 0x30U))
                      || (((Reg0x3060 == 0x02U) && (Reg0x3067 == 0x30U)) && ((Reg0x3064 == 0x05U) || (Reg0x3064 == 0x06U) || (Reg0x3064 == 0x07U) || (Reg0x3064 == 0x14U) || (Reg0x3064 == 0x15U)))) {
                *pVerID = IMX390_CHIP_VERSION_CASE7;
            } else if (((Reg0x3060 == 0x00U) && (Reg0x3067 == 0x40U)) || (((Reg0x3060 == 0x02U) && (Reg0x3067 == 0x30U)) && ((Reg0x3064 == 0x08U) || (Reg0x3064 == 0x09U) || (Reg0x3064 == 0x0AU) || (Reg0x3064 == 0x0BU) || (Reg0x3064 == 0x0CU)))) {
                    *pVerID = IMX390_CHIP_VERSION_CASE8;
            } else {
                *pVerID = IMX390_CHIP_VERSION_CASE8;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  Unknown chip version  ============", 0U, 0U, 0U, 0U, 0U);

            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_ChangeReadoutMode
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
static UINT32 IMX390_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;


    UINT16 i, j, DataSize;
    const IMX390_REG_s *pModeRegTable = &IMX390_RegTableCase5[0];
    const IMX390_SEQ_REG_s *pModeRegTableOTP = &IMX390_RegTableCase7_OTPM4[0];
    UINT16 FirstAddr, NextAddr;
    UINT8 FirstData, NextData;
    UINT8 TxData[64];
    UINT32 VerID = 0U;
    UINT32 RegNum = IMX390_NUM_REG_CASE5, RegNum_OTP = IMX390_NUM_REG_CASE7_OTPM4;
    UINT8 OTP_F=0;

    if (IMX390_Query_VerID(pChan, &VerID) != SENSOR_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] Unable to get chip version", 0U, 0U, 0U, 0U, 0U);
    } else {
        if (VerID == IMX390_CHIP_VERSION_60DEG) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] Not support 60 deg setting", 0U, 0U, 0U, 0U, 0U);
        } else if (VerID == IMX390_CHIP_VERSION_CASE5) {      //Case5
            pModeRegTable = &IMX390_RegTableCase5[0];
            RegNum = IMX390_NUM_REG_CASE5;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] IMX390 Ver: case5 (RegTableSize: %u)", RegNum, 0U, 0U, 0U, 0U);
        } else if (VerID == IMX390_CHIP_VERSION_CASE7) {      //Case 7                                             //Case7
                OTP_F=1;
                pModeRegTable = &IMX390_RegTableCase7[0];
                RegNum = IMX390_NUM_REG_CASE7;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] IMX390 Ver: case7 (RegTableSize: %u)", RegNum, 0U, 0U, 0U, 0U);
            if ((ModeID == 5U) || (ModeID == 8U)){                //Case7 - M2 OTP
                pModeRegTableOTP = &IMX390_RegTableCase7_OTPM2[0];
                RegNum_OTP = IMX390_NUM_REG_CASE7_OTPM2;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] IMX390 Ver: case7M2OTP (RegTableSize: %u)", IMX390_NUM_REG_CASE7_OTPM2, 0U, 0U, 0U, 0U);
            }else{                                                //Case7 - M4 OTP
                pModeRegTableOTP = &IMX390_RegTableCase7_OTPM4[0];
                RegNum_OTP = IMX390_NUM_REG_CASE7_OTPM4;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] IMX390 Ver: case7M4OTP (RegTableSize: %u)", IMX390_NUM_REG_CASE7_OTPM4, 0U, 0U, 0U, 0U);
            }
        } else {                                                   //Case8 or unknown
                OTP_F=1;
                pModeRegTable = &IMX390_RegTableCase8[0];
                RegNum = IMX390_NUM_REG_CASE8;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] IMX390 Ver: case8 (RegTableSize: %u)", RegNum, 0U, 0U, 0U, 0U);
            if ((ModeID == 5U) || (ModeID == 8U)){                //Case8 - M2 OTP
                pModeRegTableOTP = &IMX390_RegTableCase8_OTPM2[0];
                RegNum_OTP = IMX390_NUM_REG_CASE8_OTPM2;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] IMX390 Ver: case8M2OTP (RegTableSize: %u)", IMX390_NUM_REG_CASE8_OTPM2, 0U, 0U, 0U, 0U);
            }else{                                                //Case8 - M4 OTP
                pModeRegTableOTP = &IMX390_RegTableCase8_OTPM4[0];
                RegNum_OTP = IMX390_NUM_REG_CASE8_OTPM4;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX390] IMX390 Ver: case8M4OTP (RegTableSize: %u)", IMX390_NUM_REG_CASE8_OTPM4, 0U, 0U, 0U, 0U);
            }
        }
    }

    for (i = 0U; i < RegNum; i += DataSize) {
        if (RetVal == ERR_NONE) {
            DataSize = 0;
            FirstAddr = pModeRegTable[i].Addr;
            FirstData = pModeRegTable[i].Data[ModeID];

            TxData[DataSize] = FirstData;
            DataSize++;

            for (j = i + 1U; j < RegNum; j++) {
                NextAddr = pModeRegTable[j].Addr;
                NextData = pModeRegTable[j].Data[ModeID];

                if (((NextAddr - FirstAddr) != (j - i)) || ((DataSize + 1U) > (UINT16)((UINT16)IMX390_I2C_WR_BUF_SIZE - 3U))) {
                    break;
                } else {
                    TxData[DataSize] = NextData;
                    DataSize++;
                }
            }

            RetVal = RegWrite(pChan, FirstAddr, TxData, DataSize);
        }
    }

    if (OTP_F == 1U){
        for (i = 0U; i < RegNum_OTP; i += DataSize) {
            if (RetVal == ERR_NONE) {
                DataSize = 0;
                FirstAddr = pModeRegTableOTP[i].Addr;
                FirstData = pModeRegTableOTP[i].Data;

                TxData[DataSize] = FirstData;
                DataSize++;

                for (j = i + 1U; j < RegNum_OTP; j++) {
                    NextAddr = pModeRegTableOTP[j].Addr;
                    NextData = pModeRegTableOTP[j].Data;

                    if (((NextAddr - FirstAddr) != (j - i)) || ((DataSize + 1U) > (UINT16)((UINT16)IMX390_I2C_WR_BUF_SIZE - 3U))) {
                        break;
                    } else {
                        TxData[DataSize] = NextData;
                        DataSize++;
                    }
                }
                RetVal = RegWrite(pChan, FirstAddr, TxData, DataSize);
            }
        }
    }

   /* Reset current AE information */
    if (RetVal == ERR_NONE) {
        RetVal = AmbaWrap_memset(IMX390Ctrl[pChan->VinID].CurrentAgcCtrl, 0x0, sizeof(UINT32) * 3U * IMX390_NUM_MAX_SENSOR_COUNT);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memset return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }
    if (RetVal == ERR_NONE) {
        RetVal = AmbaWrap_memset(IMX390Ctrl[pChan->VinID].CurrentDgcCtrl, 0x0, sizeof(UINT32) * 3U * IMX390_NUM_MAX_SENSOR_COUNT);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memset return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }
    if (RetVal == ERR_NONE) {
        RetVal = AmbaWrap_memset(IMX390Ctrl[pChan->VinID].CurrentWbCtrl, 0x0, sizeof(AMBA_SENSOR_WB_CTRL_s) * 3U * IMX390_NUM_MAX_SENSOR_COUNT);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memset return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }
    if (RetVal == ERR_NONE) {
        RetVal = AmbaWrap_memset(IMX390Ctrl[pChan->VinID].CurrentShutterCtrl, 0x0, sizeof(UINT32) * 3U * IMX390_NUM_MAX_SENSOR_COUNT);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memset return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX390_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;

    for (i = 0; i < IMX390_NUM_MODE; i++) {

        IMX390ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX390_SensorInfo[i].LineLengthPck /
                                                      ((DOUBLE)IMX390_SensorInfo[i].DataRate *
                                                      (DOUBLE)IMX390_SensorInfo[i].NumDataLanes /
                                                      (DOUBLE)IMX390_SensorInfo[i].NumDataBits));
        pOutputInfo = &IMX390_OutputInfo[i];
        IMX390ModeInfoList[i].PixelRate = (FLOAT) ((DOUBLE) pOutputInfo->DataRate *
                                                                 (DOUBLE)pOutputInfo->NumDataLanes /
                                                                 (DOUBLE)pOutputInfo->NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_Init
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
static UINT32 IMX390_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const IMX390_FRAME_TIMING_s *pFrameTime = &IMX390ModeInfoList[0U].FrameTime;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "IMX390 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        IMX390_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_Enable
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
static UINT32 IMX390_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;


    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    //} else {
    //    RetVal = AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "IMX390_Enable", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_Disable
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
static UINT32 IMX390_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX390_StandbyOn(pChan);
#ifdef IMX390_IN_SLAVE_MODE
        if (RetVal == OK) {
            RetVal = AmbaVIN_MasterSyncDisable(IMX390MasterSyncChannel[pChan->VinID]);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaVIN_MasterSyncDisable return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
#endif
        //if (RetVal == OK) {
        //    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        //}
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "IMX390_Disable", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_GetStatus
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
static UINT32 IMX390_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaWrap_memcpy(pStatus, &IMX390Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_GetModeInfo
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
static UINT32 IMX390_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }
        if (Config.ModeID >= IMX390_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            IMX390_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_GetDeviceInfo
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
static UINT32 IMX390_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaWrap_memcpy(pDeviceInfo, &IMX390_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_GetHdrInfo
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
static UINT32 IMX390_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaWrap_memcpy(pHdrInfo, &IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_GetCurrentGainFactor
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
static UINT32 IMX390_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k, TotalGainCtrl;
    DOUBLE GainFactor64 = 0.0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    TotalGainCtrl = (IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] +
                                     IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]);
                    RetVal = AmbaWrap_pow(10.0, 0.015 * (DOUBLE)TotalGainCtrl, &GainFactor64);
                    if (RetVal != ERR_NONE) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_pow return %u", __LINE__, RetVal, 0U, 0U, 0U);
                    }
                    pGainFactor[0] = (FLOAT)GainFactor64;
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        TotalGainCtrl = (IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] +
                                         IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]);
                        RetVal = AmbaWrap_pow(10.0, 0.015 * (DOUBLE)TotalGainCtrl, &GainFactor64);
                        if (RetVal != ERR_NONE) {
                            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_pow return %u", __LINE__, RetVal, 0U, 0U, 0U);
                        }
                        pGainFactor[k] = (FLOAT)GainFactor64;
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_GetCurrentShutterSpeed
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
static UINT32 IMX390_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    DOUBLE ShutterTimeUnit;
    UINT8 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    pExposureTime[0] = ((FLOAT)ShutterTimeUnit * (FLOAT)IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][0]);
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pExposureTime[k] = ((FLOAT)ShutterTimeUnit * (FLOAT)IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][k]);
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_ConvertGainFactor
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
static UINT32 IMX390_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType = IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
    DOUBLE DesiredFactor;
    DOUBLE LogDesiredFactor = 1.0;
    DOUBLE ActualFactor64 = 0.0;
    FLOAT Desire_dB_SP1H, Desire_dB_SP1L, Desire_dB_SP1;
    FLOAT Agc_dB_SP1H = 0.0f, Agc_dB_SP1L = 0.0f, Agc_dB_SP1 = 0.0f;
    FLOAT Dgc_dB = 0.0f;
    AMBA_SENSOR_WB_GAIN_FACTOR_s WbGain;
    DOUBLE GainInDb, WbGainInDb;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
            RetVal = AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_log10 return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            Desire_dB_SP1H = (FLOAT)(20.0 * LogDesiredFactor);

            DesiredFactor = (DOUBLE)pDesiredFactor->Gain[1];
            RetVal = AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_log10 return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            Desire_dB_SP1L = (FLOAT)(20.0 * LogDesiredFactor);

            /************************************** Agc & Dgc **************************************/
            /* Maximum check */
            Desire_dB_SP1H = (Desire_dB_SP1H >= IMX390_MAX_TOTAL_GAIN) ? IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1H;
            Desire_dB_SP1L = (Desire_dB_SP1L >= IMX390_MAX_TOTAL_GAIN) ? IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1L;

            /* Minimum check */
            Desire_dB_SP1H = (Desire_dB_SP1H <= 0.0f) ? 0.0f : Desire_dB_SP1H;
            Desire_dB_SP1L = (Desire_dB_SP1L <= 0.0f) ? 0.0f : Desire_dB_SP1L;

            /* Maximum ABS between SP1H and SP1L is 30dB */
            if ((Desire_dB_SP1H - Desire_dB_SP1L) >= IMX390_MAX_AGAIN) {
                Desire_dB_SP1L = Desire_dB_SP1H - IMX390_MAX_AGAIN;
            }
            if ((Desire_dB_SP1H - Desire_dB_SP1L) <= -IMX390_MAX_AGAIN) {
                Desire_dB_SP1L = Desire_dB_SP1H + IMX390_MAX_AGAIN;
            }

            /* Calculate Agc/Dgc for SP1H/SP1L */
            if ((Desire_dB_SP1H <= IMX390_MAX_AGAIN) && (Desire_dB_SP1L <= IMX390_MAX_AGAIN)) {
                Agc_dB_SP1H = Desire_dB_SP1H;
                Agc_dB_SP1L = Desire_dB_SP1L;
                Dgc_dB = 0.0f;
            } else if ((Desire_dB_SP1H >= IMX390_MAX_AGAIN) && (Desire_dB_SP1L >= IMX390_MAX_AGAIN)) {
                if (Desire_dB_SP1H > Desire_dB_SP1L) {
                    Agc_dB_SP1H = IMX390_MAX_AGAIN;
                    Agc_dB_SP1L = IMX390_MAX_AGAIN - (Desire_dB_SP1H - Desire_dB_SP1L);
                    Dgc_dB = Desire_dB_SP1H - IMX390_MAX_AGAIN;
                } else {
                    Agc_dB_SP1H = IMX390_MAX_AGAIN - (Desire_dB_SP1L - Desire_dB_SP1H);
                    Agc_dB_SP1L = IMX390_MAX_AGAIN;
                    Dgc_dB = Desire_dB_SP1L - IMX390_MAX_AGAIN;
                }
            } else if (Desire_dB_SP1H > Desire_dB_SP1L) {
                Agc_dB_SP1H = Desire_dB_SP1H - Desire_dB_SP1L;
                Agc_dB_SP1L = 0.0f;
                Dgc_dB = Desire_dB_SP1L;
            } else if (Desire_dB_SP1L > Desire_dB_SP1H) {
                Agc_dB_SP1H = 0.0f;
                Agc_dB_SP1L = Desire_dB_SP1L - Desire_dB_SP1H;
                Dgc_dB = Desire_dB_SP1H;
            } else {
                AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "=== Invalid SP1H and SP1L ===", NULL, NULL, NULL, NULL, NULL);
            }

            RetVal = AmbaWrap_floor((DOUBLE)(Agc_dB_SP1H / 0.3), &GainInDb);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
            RetVal = AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.3), &GainInDb);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
            RetVal =AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)pGainCtrl->AnalogGain[0] + (DOUBLE)pGainCtrl->DigitalGain[0]), &ActualFactor64);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_pow return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            pActualFactor->Gain[0] = (FLOAT) ActualFactor64;

            RetVal = AmbaWrap_floor((DOUBLE)(Agc_dB_SP1L / 0.3), &GainInDb);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            pGainCtrl->AnalogGain[1] = (UINT32)GainInDb;
            RetVal = AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.3), &GainInDb);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            pGainCtrl->DigitalGain[1] = (UINT32)GainInDb;
            RetVal = AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)pGainCtrl->AnalogGain[1] + (DOUBLE)pGainCtrl->DigitalGain[1]), &ActualFactor64);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_pow return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            pActualFactor->Gain[1] = (FLOAT) ActualFactor64;

            pGainCtrl->AnalogGain[2] = pGainCtrl->AnalogGain[1];
            pGainCtrl->DigitalGain[2] = pGainCtrl->DigitalGain[1];
            pActualFactor->Gain[2] = pActualFactor->Gain[1];
        } else {
            DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
            RetVal = AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_log10 return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            Desire_dB_SP1 = (FLOAT)(20.0 * LogDesiredFactor);

            /* Maximum check */
            Desire_dB_SP1 = (Desire_dB_SP1 >= IMX390_MAX_TOTAL_GAIN) ? IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1;

            /* Minimum check */
            Desire_dB_SP1 = (Desire_dB_SP1 <= 0.0f) ? 0.0f : Desire_dB_SP1;

            /* Calculate Agc/Dgc for SP1 */
            if (Desire_dB_SP1 <= IMX390_MAX_AGAIN) {
                Agc_dB_SP1 = Desire_dB_SP1;
                Dgc_dB = 0.0f;
            } else {
                Agc_dB_SP1 = IMX390_MAX_AGAIN;
                Dgc_dB = Desire_dB_SP1 - IMX390_MAX_AGAIN;
            }

            RetVal = AmbaWrap_floor((DOUBLE)(Agc_dB_SP1 / 0.3), &GainInDb);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
            RetVal = AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.3), &GainInDb);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
            RetVal = AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)pGainCtrl->AnalogGain[0] + (DOUBLE)pGainCtrl->DigitalGain[0]), &ActualFactor64);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_pow return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            pActualFactor->Gain[0] = (FLOAT) ActualFactor64;
        }

        /************************************** Wb gain **************************************/
        /* Bounding check (SP1H&SP1L&SP2 share the same Wb gain) */
        WbGain.R  = (pDesiredFactor->WbGain[0].R >= 15.996094f)  ? 15.996094f : pDesiredFactor->WbGain[0].R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr >= 15.996094f) ? 15.996094f : pDesiredFactor->WbGain[0].Gr;
        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb >= 15.996094f) ? 15.996094f : pDesiredFactor->WbGain[0].Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B >= 15.996094f)  ? 15.996094f : pDesiredFactor->WbGain[0].B;

        WbGain.R  = (pDesiredFactor->WbGain[0].R <= 1.0f)  ? 1.0f : WbGain.R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr <= 1.0f) ? 1.0f : WbGain.Gr;
        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb <= 1.0f) ? 1.0f : WbGain.Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B <= 1.0f)  ? 1.0f : WbGain.B;

        RetVal = AmbaWrap_floor((DOUBLE)(WbGain.R * 256.0), &WbGainInDb);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
        pGainCtrl->WbGain[0].R = (UINT32)WbGainInDb;
        RetVal = AmbaWrap_floor((DOUBLE)(WbGain.Gr * 256.0), &WbGainInDb);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
        pGainCtrl->WbGain[0].Gr = (UINT32)WbGainInDb;
        RetVal = AmbaWrap_floor((DOUBLE)(WbGain.Gb * 256.0), &WbGainInDb);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
        pGainCtrl->WbGain[0].Gb = (UINT32)WbGainInDb;
        RetVal = AmbaWrap_floor((DOUBLE)(WbGain.B * 256.0), &WbGainInDb);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
        }
        pGainCtrl->WbGain[0].B = (UINT32)WbGainInDb;

        pActualFactor->WbGain[0].R  = (FLOAT)pGainCtrl->WbGain[0].R  / 256.0f;
        pActualFactor->WbGain[0].Gr = (FLOAT)pGainCtrl->WbGain[0].Gr / 256.0f;
        pActualFactor->WbGain[0].Gb = (FLOAT)pGainCtrl->WbGain[0].Gb / 256.0f;
        pActualFactor->WbGain[0].B  = (FLOAT)pGainCtrl->WbGain[0].B  / 256.0f;

        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            RetVal = AmbaWrap_memcpy(&pGainCtrl->WbGain[1], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            RetVal = AmbaWrap_memcpy(&pGainCtrl->WbGain[2], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }

            RetVal = AmbaWrap_memcpy(&pActualFactor->WbGain[1], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
            RetVal = AmbaWrap_memcpy(&pActualFactor->WbGain[2], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_memcpy return %u", __LINE__, RetVal, 0U, 0U, 0U);
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
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 MaxExposureline, MinExposureline;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ShutterCtrl;
    DOUBLE ShutterTimeUnit, ShutterCtrlInDb;
    UINT32 RetVal;

    ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

    RetVal = AmbaWrap_floor(((DOUBLE)ExposureTime / ShutterTimeUnit), &ShutterCtrlInDb);
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    /* VMAX is 20 bits */
    if (ShutterCtrl > ((0xfffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame)) {
        ShutterCtrl = ((0xfffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame);
    }

#ifdef IMX390_IN_SLAVE_MODE
    /* (ERRWID_FRONT / 2) < SHSx < (MODE_VMAX * (FMAX + 1)) - 1 - (ERRWID_BACK / 2) */

    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MaxExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine - 2U;
        MinExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine + 4U;
    /* For none-HDR mode */
    } else {
        MaxExposureline = ((((ShutterCtrl / pModeInfo->NumExposureStepPerFrame) + 1U) * pModeInfo->NumExposureStepPerFrame) - 2U) - 2U;
        MinExposureline = 1U + 4U;
    }
#else
    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MaxExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
    /* For none-HDR mode */
    } else {
        MaxExposureline = (((ShutterCtrl / pModeInfo->NumExposureStepPerFrame) + 1U) * pModeInfo->NumExposureStepPerFrame) - 2U;
        MinExposureline = 1U;
    }
#endif

    if (ShutterCtrl > MaxExposureline) {
        ShutterCtrl = MaxExposureline;
    } else if (ShutterCtrl < MinExposureline) {
        ShutterCtrl = MinExposureline;
    } else {
        (void)ShutterCtrl;  /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl   = ShutterCtrl;
    *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_ConvertShutterSpeed
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
static UINT32 IMX390_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
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
 *  @RoutineName:: IMX390_SetAnalogGainCtrl
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
static UINT32 IMX390_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX390_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

            /* Update current analog gain control */
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = pAnalogGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = IMX390_SetHdrAnalogGainReg(pChan, pAnalogGainCtrl);

            /* Update current analog gain control */
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] = pAnalogGainCtrl[k];
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pDigitalGainCtrl: Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX390_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX390_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]= pDigitalGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = IMX390_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]= pDigitalGainCtrl[k];
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetWbGainCtrl
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
static UINT32 IMX390_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX390_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    IMX390Ctrl[pChan->VinID].CurrentWbCtrl[i][0] = pWbGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = IMX390_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        IMX390Ctrl[pChan->VinID].CurrentWbCtrl[i][k] = pWbGainCtrl[k];
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetShutterCtrl
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
static UINT32 IMX390_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX390_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][0] = pShutterCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = IMX390_SetHdrShutterReg(pChan, pShutterCtrl);

            /* Update current shutter control */
            for (i = 0U; i < IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][k] = pShutterCtrl[k];
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_SetSlowShrCtrl
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
static UINT32 IMX390_SetSlowShrCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE){
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        RetVal = IMX390_SetSlowShutterReg(SlowShutterCtrl);
    }

    return RetVal;
}

#ifdef IMX390_IN_SLAVE_MODE
static UINT32 IMX390_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal, i;
    DOUBLE PeriodInDb;
    const IMX390_MODE_INFO_s* pModeInfo = &IMX390ModeInfoList[ModeID];
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg  = {0};

    RetVal = AmbaWrap_floor(((DOUBLE)pModeInfo->FrameTime.FrameRate.NumUnitsInTick * ((DOUBLE) IMX390_SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameTime.FrameRate.TimeScale)) + 0.5, &PeriodInDb);
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaWrap_floor return %u", __LINE__, RetVal, 0U, 0U, 0U);
    }
    MasterSyncCfg.RefClk            = IMX390_SensorInfo[ModeID].InputClk;
    MasterSyncCfg.HSync.Period      = (UINT32)PeriodInDb;
    MasterSyncCfg.HSync.PulseWidth  = 8U;
    MasterSyncCfg.HSync.Polarity    = 0U;
    MasterSyncCfg.VSync.Period      = 1U;
    MasterSyncCfg.VSync.PulseWidth  = 1000U;
    MasterSyncCfg.VSync.Polarity    = 0U;
    MasterSyncCfg.HSyncDelayCycles  = 1U;
    MasterSyncCfg.VSyncDelayCycles  = 0U;
    MasterSyncCfg.ToggleHsyncInVblank  = 1U;

#ifdef IMX390_SHARING_MASTER_SYNC
    // TODO: use the same AMBA_SENSOR_MSYNC_CHANNEL on HW to make different VIN start together.
    RetVal = AmbaVIN_MasterSyncEnable(IMX390MasterSyncChannel[pChan->VinID], &MasterSyncCfg);
    if (RetVal != ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AmbaVIN_MasterSyncEnable return %u", RetVal, 0U, 0U, 0U, 0U);
    }
#else
    // enable all master sync together to make different VIN start close.
    for (i = 0; i < AMBA_NUM_VIN_CHANNEL; i ++) {
        RetVal = AmbaVIN_MasterSyncEnable(IMX390MasterSyncChannel[i], &MasterSyncCfg);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AmbaVIN_MasterSyncEnable for vin %u return %u", i, RetVal, 0U, 0U, 0U);
        }
    }
    AmbaMisra_TouchUnused(&pChan);
#endif

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX390_Config
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
static UINT32 IMX390_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
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
    AMBA_VIN_MIPI_PAD_CONFIG_s IMX390PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    UINT8  Data;

    if (ModeID >= IMX390_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  [ IMX390_Config ]  ============", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d ============", ModeID, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

        /* update status */
        IMX390_PrepareModeInfo(pChan, pMode, pModeInfo);

// moved to IMX390_Disable instead of here, to start different VIN close
//#ifdef IMX390_IN_SLAVE_MODE
//        RetVal |= AmbaVIN_MasterSyncDisable(IMX390MasterSyncChannel[pChan->VinID]);
//#endif
        /* Adjust mipi-phy parameters */
        IMX390PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", IMX390PadConfig.DateRate, 0U, 0U, 0U, 0U);
        IMX390PadConfig.EnabledPin = (((UINT32)1U << pModeInfo->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
        RetVal = AmbaVIN_MipiReset(pChan->VinID, &IMX390PadConfig);
        if (RetVal != ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AmbaVIN_MipiReset return %u", RetVal, 0U, 0U, 0U, 0U);
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AmbaUserVIN_SensorClkCtrl return %u", RetVal, 0U, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = AmbaKAL_TaskSleep(3);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaKAL_TaskSleep return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }
        if (RetVal == ERR_NONE) {
            RetVal = IMX390_ResetSensor(pChan);
        }
        if (RetVal == ERR_NONE) {
            /* program sensor */
            RetVal = IMX390_ChangeReadoutMode(pChan, ModeID);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "IMX390_ChangeReadoutMode return %u", RetVal, 0U, 0U, 0U, 0U);
            }
        }

#ifdef IMX390_IN_SLAVE_MODE
        if (RetVal == ERR_NONE) {
            Data = 0x0aU;
            RetVal = RegWrite(pChan, 0x365cU, &Data, 1U);
        }
        if (RetVal == ERR_NONE) {
            Data = 0x0aU;
            RetVal = RegWrite(pChan, 0x365eU, &Data, 1U);
        }
        if (RetVal == ERR_NONE) {
            Data = 0x06U;
            RetVal = RegWrite(pChan, 0x23c2U, &Data, 1U);
        }
        if (RetVal == ERR_NONE) {
            Data = 0x01U;
            RetVal = RegWrite(pChan, 0x3650U, &Data, 1U);
        }
#endif

        if (RetVal == ERR_NONE) {
            RetVal = IMX390_StandbyOff(pChan);
        }

#ifdef IMX390_IN_SLAVE_MODE
        if (RetVal == ERR_NONE) {
            RetVal = IMX390_ConfigMasterSync(pChan, ModeID);
        }
#endif

        if (RetVal == ERR_NONE) {
            RetVal = AmbaKAL_TaskSleep(22); //TBD: To avoid i2c fail for gain setting
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaKAL_TaskSleep return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }
        }

#if !defined(CONFIG_SOC_H22)
        // for config post
        //mask the IMX390 MIPI frame output
        //set MUTE_VSYNC_MASK_EN=1
        if (RetVal == ERR_NONE) {
            Data = ((UINT8)1U << 2U);
            RetVal = RegWrite(pChan, 0x0088U, &Data, 1U);
        }
        //set SM_MUTE_VSYNC_MASK_EN_APL=1
        if (RetVal == ERR_NONE) {
            Data = ((UINT8)1U << 6U);
            RetVal = RegWrite(pChan, 0x03C0U, &Data, 1U);
        }
#endif
        /* config vin */
        if (RetVal == ERR_NONE) {
            RetVal = IMX390_ConfigVin(pChan->VinID, pModeInfo);
        }

#if !defined(CONFIG_SOC_H22)
        // for config post
        if (RetVal == ERR_NONE) {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "wait two frames", NULL, NULL, NULL, NULL, NULL);

            //waits two frame time until mask IMX390 mipi output
            RetVal = AmbaKAL_TaskSleep(2U * 1000U * IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.NumUnitsInTick / IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale);
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)NumUnitsInTick  %u", __LINE__, IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.NumUnitsInTick, 0U, 0U, 0U);
            if (RetVal != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "(%u)AmbaKAL_TaskSleep return %u", __LINE__, RetVal, 0U, 0U, 0U);
            }

            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "leave sensor config", NULL, NULL, NULL, NULL, NULL);
        }
#endif
    }

    return RetVal;
}
#if !defined(CONFIG_SOC_H22)
static UINT32 IMX390_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    UINT8 Data;
    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "IMX390_ConfigPost", NULL, NULL, NULL, NULL, NULL);
    //unmask the IMX390 MIPI frame output
    //set MUTE_VSYNC_MASK_EN=0
    Data = 0x0;
    RetVal = RegWrite(pChan, 0x0088U, &Data, 1U);
    //set SM_MUTE_VSYNC_MASK_EN_APL=0
    if (RetVal == ERR_NONE) {
        RetVal = RegWrite(pChan, 0x03C0U, &Data, 1U);
    }
    return RetVal;
}
#endif
/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX390Obj = {
    .SensorName             = "IMX390",
    .Init                   = IMX390_Init,
    .Enable                 = IMX390_Enable,
    .Disable                = IMX390_Disable,
    .Config                 = IMX390_Config,
    .GetStatus              = IMX390_GetStatus,
    .GetModeInfo            = IMX390_GetModeInfo,
    .GetDeviceInfo          = IMX390_GetDeviceInfo,
    .GetHdrInfo             = IMX390_GetHdrInfo,
    .GetCurrentGainFactor   = IMX390_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX390_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX390_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX390_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX390_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX390_SetDigitalGainCtrl,
    .SetWbGainCtrl          = IMX390_SetWbGainCtrl,
    .SetShutterCtrl         = IMX390_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX390_SetSlowShrCtrl,

    .RegisterRead           = IMX390_RegisterRead,
    .RegisterWrite          = IMX390_RegisterWrite,
#if !defined(CONFIG_SOC_H22)
    .ConfigPost             = IMX390_ConfigPost,
#else
    .ConfigPost             = NULL,
#endif
};

