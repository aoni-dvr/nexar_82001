/**
 *  @file AmbaSensor_MAX9295_96712_IMX390.c
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
 *  @details Control APIs of MAX9295_96712 plus SONY IMX390 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaMisraFix.h"

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_96712_IMX390.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSbrg_Max9295_96712.h"

#include "bsp.h"

//#define DEBUG_MSG //enable if when need to print all sensor setting
#define MX01_IMX390_IN_SLAVE_MODE
#define MX01_IMX390_I2C_WR_BUF_SIZE 64

static UINT32 MX01_IMX390_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);

//Please noted VIN0 supports up to 8lane, PIP supports up to 4lane(shared with VIN0), PIP2 support up to 2lane(shared with PIP)
static UINT32 MX01_IMX390I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_MAXIM_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_MAXIM_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_MAXIM_I2C_CHANNEL_PIP2,
};

#ifdef MX01_IMX390_IN_SLAVE_MODE
static UINT32 MX01_IMX390MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_MSYNC_CHANNEL_PIP2,
};

static UINT32 MX01_IMX390_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID, UINT32 SlowShutterCtrl);
#endif
/*-----------------------------------------------------------------------------------------------*\
 * MX01_IMX390 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX01_IMX390_CTRL_s MX01_IMX390Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};

static MAX9295_96712_SERDES_CONFIG_s MX01_IMX390SerdesConfig = {
    .EnabledLinkID   = 0x01U,
    .SensorSlaveID   = MX01_IMX390_I2C_SLAVE_ADDRESS,
    .SensorBCID      = MX01_IMX390_I2C_BC_SLAVE_ADDRESS,
    .SensorAliasID   = {
        [0] = 0x60U,
        [1] = 0x62U,
        [2] = 0x64U,
        [3] = 0x66U,
    },
    .DataType        = 0x2cU,  /* Default DataType: RAW12 */
    .DataType2       = 0xffU,  /* Default secondary DataType: not used */

    .CSIRxLaneNum    = 4,
    .CSITxLaneNum    = {4, 4, 4, 4},

    .CSITxSpeed      = {10, 10, 10, 10},

};

static UINT16 MX01_IMX390_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_GetNumActiveSensor
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
static UINT32 MX01_IMX390_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
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
 *  @RoutineName:: MX01_IMX390_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Chan:     Vin ID and sensor ID
 *      Mode:     Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                               = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s              *pInputInfo     = &MX01_IMX390_InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s             *pOutputInfo    = &MX01_IMX390_OutputInfo[ModeID];
    const MX01_IMX390_SENSOR_INFO_s     *pSensorInfo    = &MX01_IMX390_SensorInfo[ModeID];
    const MX01_IMX390_FRAME_TIMING_s    *pFrameTime     = &MX01_IMX390ModeInfoList[ModeID].FrameTime;
    UINT32 SensorNum = MX01_IMX390_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (SensorNum != 0U) {
        if (AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }

        pModeInfo->FrameLengthLines = pSensorInfo->FrameLengthLines * SensorNum;
        pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
        pModeInfo->InternalExposureOffset = 0.0f;
        //pModeInfo->PixelRate = MAX9295_96712_IMX390ModeInfoList[ModeID].PixelRate * SensorNum;
        pModeInfo->RowTime = MX01_IMX390ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
        if (AmbaWrap_floor((((DOUBLE)MX01_IMX390ModeInfoList[ModeID].PixelRate  * (DOUBLE)SensorNum) * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
        pModeInfo->LineLengthPck =  (UINT32)FloorVal;
        pModeInfo->InputClk = pSensorInfo->InputClk;

        if (AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
        if (AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
        if (AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
        if (AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
        if (AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
        if (AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX01_IMX390_HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) { // HDR information
            RetVal = SENSOR_ERR_UNEXPECTED;
        }

        /* update for MAX96712 CSI-2 output */
        pModeInfo->OutputInfo.DataRate = pModeInfo->OutputInfo.DataRate * SensorNum;
        pModeInfo->OutputInfo.OutputHeight = (UINT16)(pModeInfo->OutputInfo.OutputHeight * SensorNum);
        pModeInfo->OutputInfo.RecordingPixels.StartY = (UINT16)(pModeInfo->OutputInfo.RecordingPixels.StartY * SensorNum);
        pModeInfo->OutputInfo.RecordingPixels.Height = (UINT16)(pModeInfo->OutputInfo.RecordingPixels.Height * SensorNum);

        /* Updated minimum frame rate limitation */
        if (pModeInfo->HdrInfo.HdrType == 0U) {
            pModeInfo->MinFrameRate.TimeScale /= 2U;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_ConfigSerDes
 *
 *  @Description:: Configure Serdes sync/video forwarding, I2C translation, and MIPI I/F
 *
 *  @Input      ::
 *      Chan:        Vin ID and sensor ID
 *      SensorMode:  Sensor mode
 *      pModeInfo:   Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const MX01_IMX390_SENSOR_INFO_s *pSensorInfo = &MX01_IMX390_SensorInfo[SensorMode];
    MAX9295_96712_SERDES_CONFIG_s *pSerdesCfg = &MX01_IMX390SerdesConfig;
    DOUBLE FloorVal;
    UINT32 RetVal = SENSOR_ERR_NONE;

    pSerdesCfg->EnabledLinkID = MX01_IMX390_GetEnabledLinkID(pChan->SensorID);

    pSerdesCfg->DataType = 0x2cU;  /* RAW12 */
    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;

    if (AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    (void) Max9295_96712_Config(pChan->VinID, &MX01_IMX390SerdesConfig);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_ConfigVin
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
static UINT32 MX01_IMX390_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX01_IMX390VinConfig = {
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
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &MX01_IMX390VinConfig;
    UINT32 RetVal = SENSOR_ERR_NONE;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;


    if (AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    RetVal |= AmbaVIN_MipiConfig(VinID, pVinCfg);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_ResetSensor
 *
 *  @Description:: Reset IMX390 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MX01_IMX390_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX390_ResetSensor] ", NULL, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "; # Link A: MAX9295A MFP4 H->L->H for IMX390", NULL, NULL, NULL, NULL, NULL);

    if((pChan->SensorID & MX01_IMX390_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_A_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_A_ID, MAX9295_96712_GPIO_PIN_4, 1);
    }
    if((pChan->SensorID & MX01_IMX390_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_B_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_B_ID, MAX9295_96712_GPIO_PIN_4, 1);
    }
    if((pChan->SensorID & MX01_IMX390_SENSOR_ID_CHAN_C) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_C_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_C_ID, MAX9295_96712_GPIO_PIN_4, 1);
    }
    if((pChan->SensorID & MX01_IMX390_SENSOR_ID_CHAN_D) != 0U) {
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_D_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MX01_MAX9295_D_ID, MAX9295_96712_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start */

    if (RetVal != MAX9295_96712_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX9295_96712_IMX390_RegRW
 *
 *  @Description:: Read/Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      pChan:      pointer to Vin ID and sensor ID
 *      Addr:       Register Address
 *      pTxData:    Pointer to Write data buffer
 *      pRxData:    Pointer to Echo data buffer
 *      Size:       Number of Read/Write data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 RegRW(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX01_IMX390_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;
#ifdef DEBUG_MSG
    UINT32 k;
#endif

    if ((pChan == NULL) || (Size > MX01_IMX390_SENSOR_I2C_MAX_SIZE)) {
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
        RetVal = AmbaI2C_MasterWrite(MX01_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        if ((pChan->SensorID == (MX01_IMX390_SENSOR_ID_CHAN_A | MX01_IMX390_SENSOR_ID_CHAN_B)) ||
            (pChan->SensorID == (MX01_IMX390_SENSOR_ID_CHAN_A | MX01_IMX390_SENSOR_ID_CHAN_B | MX01_IMX390_SENSOR_ID_CHAN_C)) ||
            (pChan->SensorID == (MX01_IMX390_SENSOR_ID_CHAN_A | MX01_IMX390_SENSOR_ID_CHAN_B | MX01_IMX390_SENSOR_ID_CHAN_D)) ||
            (pChan->SensorID == (MX01_IMX390_SENSOR_ID_CHAN_A | MX01_IMX390_SENSOR_ID_CHAN_B | MX01_IMX390_SENSOR_ID_CHAN_C | MX01_IMX390_SENSOR_ID_CHAN_D))) {

            I2cConfig.SlaveAddr = MX01_IMX390_I2C_BC_SLAVE_ADDRESS;

            RetVal = AmbaI2C_MasterWrite(MX01_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                         &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Broadcast] SensorID 0x%05x Slave ID 0x%2x", pChan->SensorID, I2cConfig.SlaveAddr, 0U, 0U, 0U);
            for (k = 0U; k < Size; k++) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
            }
#endif

        } else {
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX01_IMX390SerdesConfig.SensorAliasID[i];

                    RetVal |= AmbaI2C_MasterWrite(MX01_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                                  &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[SensorID 0x%02x] Slave ID 0x%2x", ((UINT32)1U << i), I2cConfig.SlaveAddr, 0U, 0U, 0U);
                    for (k = 0U; k < Size; k++) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
                    }
#endif
                }
            }
        }

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX390] I2C_Ch %d Slave 0x%02x Addr 0x%04x Size %d I2C does not work!!!!!", MX01_IMX390I2cChannel[pChan->VinID], I2cConfig.SlaveAddr, Addr, Size, 0U);
        }
    }
#if 0
    for (i = 0U; i < Size; i++) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX390][IO] Addr = 0x%04x, Data = 0x%02x", Addr+i, pTxData[i], 0U, 0U, 0U);
        TxDataBuf[0] = (UINT8)((Addr+i) & 0xff);
        TxDataBuf[1] = (UINT8)(((Addr+i)>>8) & 0xff);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "sendln 't drv i2c write 0 0x%02X 0x%02X 0x%02X 0x%02X", I2cConfig.SlaveAddr, TxDataBuf[1], TxDataBuf[0], pTxData[i], 0U);
    }
#endif
    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_RegisterWrite
 *
 *  @Description:: Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      pChan:  pointer to Vin ID and sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)Data;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRW(pChan, Addr, &WData, 1U);
    }

    return RetVal;
}

static UINT32 MX01_IMX390_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2] = {0};
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if(pChan->SensorID == MX01_IMX390_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX01_IMX390SerdesConfig.SensorAliasID[0];
        } else if(pChan->SensorID == MX01_IMX390_SENSOR_ID_CHAN_B) {
            I2cTxConfig.SlaveAddr = MX01_IMX390SerdesConfig.SensorAliasID[1];
        } else if(pChan->SensorID == MX01_IMX390_SENSOR_ID_CHAN_C) {
            I2cTxConfig.SlaveAddr = MX01_IMX390SerdesConfig.SensorAliasID[2];
        } else if(pChan->SensorID == MX01_IMX390_SENSOR_ID_CHAN_D) {
            I2cTxConfig.SlaveAddr = MX01_IMX390SerdesConfig.SensorAliasID[3];
        } else {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX01_IMX390_RegRead] NG. incorrect sensor ID 0x%x", pChan->SensorID, 0U, 0U, 0U, 0U);
            RetVal = SENSOR_ERR_ARG;
        }
        if (RetVal == SENSOR_ERR_NONE) {
            I2cTxConfig.DataSize  = 2U;
            I2cTxConfig.pDataBuf  = TxData;
            TxData[0] = (UINT8)(Addr >> 8U);
            TxData[1] = (UINT8)(Addr & 0xffU);

            I2cRxConfig.SlaveAddr = (I2cTxConfig.SlaveAddr | (UINT32)1U);
            I2cRxConfig.DataSize  = 1U;
            I2cRxConfig.pDataBuf  = pRxData;

            RetVal = AmbaI2C_MasterReadAfterWrite(MX01_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                                  &I2cRxConfig, &TxSize, 1000U);
        } else {
            //SENSOR_ERR
        }
    }
    if (RetVal != I2C_ERR_NONE) {
        RetVal = SENSOR_ERR_COMMUNICATE;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX390] i2c Slave Addr 0x%02x Addr 0x%04x I2C does not work!!!!!", I2cTxConfig.SlaveAddr, Addr, 0U, 0U, 0U);
    } else {
        //SENSOR_ERR_NONE
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX9295_96712_IMX390_RegisterRead
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
static UINT32 MX01_IMX390_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX01_IMX390_RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_SetAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      pChan:  pointer to Vin ID and sensor ID
 *      PGC:    Analog gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl)
{
    UINT32 RetVal;
    UINT8 WData[2];

    /* SP1L */
    WData[0] = (UINT8)(AnalogGainCtrl & 0xffU);
    WData[1] = (UINT8)((AnalogGainCtrl >> 8U) & 0x7U);
    RetVal = RegRW(pChan, MX01_IMX390_AGAIN_SP1L, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_SetDigitalGainReg
 *
 *  @Description:: Configure sensor conversion gain setting
 *
 *  @Input      ::
 *      pChan:     pointer to Vin ID and sensor ID
 *      CG:        Conversion gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetDigitalGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 DigitalGainCtrl)
{
    UINT32 RetVal;
    UINT8 WData[2];

    /* SP1L */
    WData[0] = (UINT8)(DigitalGainCtrl & 0xffU);
    WData[1] = (UINT8)((DigitalGainCtrl >> 8U) & 0x7U);
    RetVal = RegRW(pChan, MX01_IMX390_PGA_SP1H, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_SetWbGainReg
 *
 *  @Description:: Configure sensor wb gain setting
 *
 *  @Input      ::
 *      pChan:           pointer Vin ID and sensor ID
 *      pWbGainCtrl:     Digital gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
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
    RetVal |= RegRW(pChan, MX01_IMX390_WBGAIN_R, WData, 8);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      pChan:              pointer to Vin ID and sensor ID
 *      NumXhsEshrSpeed:    Integration time in number of XHS period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 ShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHS1;
    UINT8 WData[4];

    /* SP1H & SP1L */
    SHS1 = MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - ShutterCtrl;

    WData[0] = (UINT8)(SHS1 & 0xffU);
    WData[1] = (UINT8)((SHS1 >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHS1 >> 16U) & 0xfU);
    RetVal |= RegRW(pChan, MX01_IMX390_SHS1, WData, 3);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_SetHdrAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      pChan:           pointer to Vin ID and sensor ID
 *      pAnalogGainCtrl: Pointer to analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetHdrAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[2];

    /* SP1H */
    WData[0] = (UINT8)(pAnalogGainCtrl[0] & 0xffU);
    WData[1] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x7U);
    RetVal |= RegRW(pChan, MX01_IMX390_AGAIN_SP1H, WData, 2);

    /* SP1L */
    WData[0] = (UINT8)(pAnalogGainCtrl[1] & 0xffU);
    WData[1] = (UINT8)((pAnalogGainCtrl[1] >> 8U) & 0x7U);
    RetVal |= RegRW(pChan, MX01_IMX390_AGAIN_SP1L, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_SetHdrShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      pChan:    pointer to Vin ID and sensor ID
 *      *pSHS:    Sensor SHS setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHSX;
    UINT8 WData[4];

    /* SP1H & SP1L */
    SHSX = MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - pShutterCtrl[0];

    WData[0] = (UINT8)(SHSX & 0xffU);
    WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHSX >> 16U) & 0xfU);
    RetVal |= RegRW(pChan, MX01_IMX390_SHS1, WData, 3);

    /* SP2 */
    SHSX = MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - pShutterCtrl[2];

    WData[0] = (UINT8)(SHSX & 0xffU);
    WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHSX >> 16U) & 0xfU);
    RetVal |= RegRW(pChan, MX01_IMX390_SHS2, WData, 3);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_SetSlowShutterReg
 *
 *  @Description:: Apply slow shutter setting
 *
 *  @Input      ::
 *     pChan:                    pointer to Vin ID and sensor ID
 *     IntegrationPeriodInFrame: Number of frames in integation period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetSlowShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 IntegrationPeriodInFrame)
{
    UINT32 TargetFrameLengthLines;
    UINT32 SensorNum = MX01_IMX390_GetNumActiveSensor(pChan->SensorID);
    UINT32 ModeID = MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData[1];
    UINT32 i;

    if (IntegrationPeriodInFrame < 1U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TargetFrameLengthLines = MX01_IMX390ModeInfoList[ModeID].FrameTime.VMAX * IntegrationPeriodInFrame;

        if (TargetFrameLengthLines > 0x3ffffU) {
            TargetFrameLengthLines = 0x3ffffU;
        }

        TxData[0] = (UINT8)(IntegrationPeriodInFrame - 1U);

        RetVal |= RegRW(pChan, MX01_IMX390_FMAX, TxData, 1U);

        /* Update frame rate information */
        MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines * SensorNum;
        MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale = MX01_IMX390ModeInfoList[ModeID].FrameTime.FrameRate.TimeScale / IntegrationPeriodInFrame;

        if (MX01_IMX390_HdrInfo[ModeID].HdrType != AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX01_IMX390_HdrInfo[ModeID].ActiveChannels; i ++) {
                MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[i].MaxExposureLine = TargetFrameLengthLines - 2U;
                MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[i].NumExposureStepPerFrame = TargetFrameLengthLines;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_StandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      ::
 *     pChan:      pointer to Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x01;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= RegRW(pChan, MX01_IMX390_STANDBY, &TxData, 1);
//    AmbaPrint("[MAX9295_96712_IMX390] MX01_IMX390_SetStandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_StandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      ::
 *     pChan:      pointer to Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x00;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= RegRW(pChan, MX01_IMX390_STANDBY, &TxData, 1);

    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * MX01_IMX390Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / MX01_IMX390Ctrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[MX01_IMX390] MX01_IMX390_StandbyOff");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_Query_VerID
 *
 *  @Description:: Check sensor chip version
 *
 *  @Input      ::
 *      pChan:     pointer to Vin ID and sensor ID
 *  @Output     ::
 *      pVerID:    pointer to MX01_IMX390 version ID
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_Query_VerID(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 *pVerID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 Reg0x3060[4], Reg0x3067[4], Reg0x3064[4];
    AMBA_SENSOR_CHANNEL_s Chan_t;
    UINT8 i;
    UINT16 j;
    UINT8 CheckVer = 0U; //0: version is not in ChipVerAry
    const MX01_IMX390_CHIP_s ChipVerAry[] = {
        //r0x3060/r0x3067/r0x3064/ChipVer
        //0xFFFF -> don't care

        //60 degree
        {0x0070U, 0x0030U, 0xFFFFU, MX01_IMX390_CHIP_VERSION_60DEG},
        {0x0070U, 0x0050U, 0xFFFFU, MX01_IMX390_CHIP_VERSION_60DEG},
        {0x0070U, 0x0060U, 0xFFFFU, MX01_IMX390_CHIP_VERSION_60DEG},
        //CASE5
        {0x0070U, 0xFFFFU, 0xFFFFU, MX01_IMX390_CHIP_VERSION_CASE5},
        {0x0071U, 0xFFFFU, 0xFFFFU, MX01_IMX390_CHIP_VERSION_CASE5},
        //CASE7
        {0x0001U, 0x0010U, 0xFFFFU, MX01_IMX390_CHIP_VERSION_CASE7},
        {0x0000U, 0x0030U, 0xFFFFU, MX01_IMX390_CHIP_VERSION_CASE7},
        {0x0002U, 0x0030U, 0x0005U, MX01_IMX390_CHIP_VERSION_CASE7},
        {0x0002U, 0x0030U, 0x0006U, MX01_IMX390_CHIP_VERSION_CASE7},
        {0x0002U, 0x0030U, 0x0007U, MX01_IMX390_CHIP_VERSION_CASE7},
        {0x0002U, 0x0030U, 0x0014U, MX01_IMX390_CHIP_VERSION_CASE7},
        {0x0002U, 0x0030U, 0x0015U, MX01_IMX390_CHIP_VERSION_CASE7},
        //CASE8
        {0x0000U, 0x0040U, 0xFFFFU, MX01_IMX390_CHIP_VERSION_CASE8},
        {0x0002U, 0x0030U, 0x0008U, MX01_IMX390_CHIP_VERSION_CASE8},
        {0x0002U, 0x0030U, 0x0009U, MX01_IMX390_CHIP_VERSION_CASE8},
        {0x0002U, 0x0030U, 0x000AU, MX01_IMX390_CHIP_VERSION_CASE8},
        {0x0002U, 0x0030U, 0x000BU, MX01_IMX390_CHIP_VERSION_CASE8},
        {0x0002U, 0x0030U, 0x000CU, MX01_IMX390_CHIP_VERSION_CASE8},
    };

    Reg0x3060[0] = 0xff;
    Reg0x3060[1] = 0xff;
    Reg0x3060[2] = 0xff;
    Reg0x3060[3] = 0xff;

    Reg0x3067[0] = 0xff;
    Reg0x3067[1] = 0xff;
    Reg0x3067[2] = 0xff;
    Reg0x3067[3] = 0xff;

    Reg0x3064[0] = 0xff;
    Reg0x3064[1] = 0xff;
    Reg0x3064[2] = 0xff;
    Reg0x3064[3] = 0xff;

    if (pVerID == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        Chan_t.VinID = pChan->VinID;

        for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                Chan_t.SensorID = (UINT32)1U << (4U * (i + 1U));
                RetVal |= MX01_IMX390_RegRead(&Chan_t, 0x3060U, &Reg0x3060[i]);
                RetVal |= MX01_IMX390_RegRead(&Chan_t, 0x3067U, &Reg0x3067[i]);
                RetVal |= MX01_IMX390_RegRead(&Chan_t, 0x3064U, &Reg0x3064[i]);
            }
        }

        if (RetVal == SENSOR_ERR_NONE) {
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    CheckVer = 0U;
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX01_IMX390] SensorID 0x%x Reg0x3060: 0x%02x, Reg0x3067: 0x%02x", ((UINT32)1U << i), Reg0x3060[i], Reg0x3067[i], 0U, 0U);
                    for (j = 0U; j < (sizeof(ChipVerAry)/sizeof(ChipVerAry[0])); j++) {
                        if ((Reg0x3060[i] == ChipVerAry[j].Reg0x3060) &&
                            (((Reg0x3067[i] & 0xFFF0U) == ChipVerAry[j].Reg0x3064) || (ChipVerAry[j].Reg0x3064 == 0xFFFFU)) &&
                            ((Reg0x3064[i] == ChipVerAry[j].Reg0x3064) || (ChipVerAry[j].Reg0x3064 == 0xFFFFU))) {
                            pVerID[i] = ChipVerAry[j].ChipVer;
                            CheckVer = 1U;
                            break;
                        }
                    }
                    if (CheckVer == 0U) {
                        //not in ChipVerAry -> CASE8
                        pVerID[i] = MX01_IMX390_CHIP_VERSION_CASE8;
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  SenorID 0x%x : Unknown chip version  ============", ((UINT32)1U << (4U * (i + 1U))), 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_SetReadoutMode
 *
 *  @Description:: Set sensor mode setting.
 *
 *  @Input      ::
 *      pChan:       pointer to Vin ID and sensor ID
 *      ReadoutMode: Sensor readout mode
 *      SensorMode : Sensor mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID, const MX01_IMX390_REG_s *pModeRegTable, const MX01_IMX390_SEQ_REG_s *pModeRegTableOTP, UINT16 RegNum, UINT16 RegNum_OTP, UINT8 OTP_F)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    UINT16 i, j, DataSize;
    UINT16 FirstAddr, NextAddr;
    UINT8 FirstData, NextData;
    UINT8 TxData[64];

    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] SensorID 0x%02x IMX390 Ver: case5/7 Setting (RegTableSize: %u)", pChan->SensorID, RegNum, 0U, 0U, 0U);
    i = 0U;
    while (i < RegNum) {

        DataSize = 0;
        FirstAddr = pModeRegTable[i].Addr;
        FirstData = pModeRegTable[i].Data[ModeID];

        TxData[DataSize] = FirstData;
        DataSize++;

        for (j = i + 1U; j < RegNum; j++) {
            NextAddr = pModeRegTable[j].Addr;
            NextData = pModeRegTable[j].Data[ModeID];

            if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)((UINT16)MX01_IMX390_I2C_WR_BUF_SIZE - 3U))) {
                break;
            } else {
                if(DataSize < 64U) {
                    TxData[DataSize] = NextData;
                    DataSize++;
                }
            }
        }
        RetVal |= RegRW(pChan, FirstAddr, TxData, DataSize);
        i += DataSize;

    }

    if (OTP_F == 1U) {
        i = 0U;
        while (i < RegNum_OTP) {
            DataSize = 0;
            FirstAddr = pModeRegTableOTP[i].Addr;
            FirstData = pModeRegTableOTP[i].Data;

            TxData[DataSize] = FirstData;
            DataSize++;

            for (j = i + 1U; j < RegNum_OTP; j++) {
                NextAddr = pModeRegTableOTP[j].Addr;
                NextData = pModeRegTableOTP[j].Data;

                if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)((UINT16)MX01_IMX390_I2C_WR_BUF_SIZE - 3U))) {
                    break;
                } else {
                    TxData[DataSize] = NextData;
                    DataSize++;
                }
            }

            RetVal |= RegRW(pChan, FirstAddr, TxData, DataSize);
            i += DataSize;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      pChan:       pointer to Vin ID and sensor ID
 *      ReadoutMode: Sensor readout mode
 *      SensorMode : Sensor mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    UINT16 i;
    const MX01_IMX390_REG_s *pModeRegTable = &MX01_IMX390_RegTableCase5[0];
    const MX01_IMX390_SEQ_REG_s *pModeRegTableOTP = &MX01_IMX390_RegTableCase7_OTPM4[0];
    UINT32 VerID[4U] = {0U};
    AMBA_SENSOR_CHANNEL_s Chan_t;
    UINT16 RegNum = MX01_IMX390_NUM_REG_CASE5, RegNum_OTP = MX01_IMX390_NUM_REG_CASE7_OTPM4;
    UINT8 OTP_F = 0U;

    Chan_t.SensorID = 0;

    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] ChangeReadoutMode SensorID 0x%x ", pChan->SensorID, 0U, 0U, 0U, 0U);

    if (MX01_IMX390_Query_VerID(pChan, VerID) != SENSOR_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] Unable to get chip version", NULL, NULL, NULL, NULL, NULL);
    } else {
        for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                Chan_t.SensorID |= (UINT32)1U << (4U * (i + 1U));
                if (VerID[i] == MX01_IMX390_CHIP_VERSION_60DEG) {
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] SensorID 0x%x Not support 60 deg setting, apply 105 deg setting", ((UINT32)1U << (4U * (i + 1U))), 0U, 0U, 0U, 0U);
                } else if (VerID[i] == MX01_IMX390_CHIP_VERSION_CASE5) {    //Case5
                    if (i == 0U) {
                        pModeRegTable = &MX01_IMX390_RegTableCase5[i];
                        RegNum = MX01_IMX390_NUM_REG_CASE5;
                    }
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] SensorID 0x%x CASE5 version", ((UINT32)1U << (4U * (i + 1U))), 0U, 0U, 0U, 0U);
                } else if (VerID[i] == MX01_IMX390_CHIP_VERSION_CASE7) {      //Case 7
                    if (i == 0U) {
                        OTP_F=1;
                        pModeRegTable = &MX01_IMX390_RegTableCase7[i];
                        RegNum = MX01_IMX390_NUM_REG_CASE7;
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390]  SensorID 0x%x CASE7 (RegTableSize: %u)", ((UINT32)1U << (4U * (i + 1U))), RegNum, 0U, 0U, 0U);
                        if ((ModeID == MX01_IMX390_1920_1080_30P_HDR1) || (ModeID == MX01_IMX390_1920_1080_30P_HDR3)) {               //Case7 - M2 OTP
                            pModeRegTableOTP = &MX01_IMX390_RegTableCase7_OTPM2[i];
                            RegNum_OTP = MX01_IMX390_NUM_REG_CASE7_OTPM2;
                            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] SensorID 0x%x CASE7M2OTP (RegTableSize: %u)", ((UINT32)1U << (4U * (i + 1U))),MX01_IMX390_NUM_REG_CASE7_OTPM2, 0U, 0U, 0U);
                        } else {                                               //Case7 - M4 OTP
                            pModeRegTableOTP = &MX01_IMX390_RegTableCase7_OTPM4[i];
                            RegNum_OTP = MX01_IMX390_NUM_REG_CASE7_OTPM4;
                            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] SensorID 0x%x CASE7M4OTP (RegTableSize: %u)", ((UINT32)1U << (4U * (i + 1U))),MX01_IMX390_NUM_REG_CASE7_OTPM4, 0U, 0U, 0U);
                        }
                    } else {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390]  SensorID 0x%x CASE7", ((UINT32)1U << (4U * (i + 1U))), 0U, 0U, 0U, 0U);
                    }

                } else  {                                             //Case 8 or unknown
                    if (i == 0U) {
                        OTP_F=1;
                        pModeRegTable = &MX01_IMX390_RegTableCase8[i];
                        RegNum = MX01_IMX390_NUM_REG_CASE8;
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] SensorID 0x%x CASE8 (RegTableSize: %u)", ((UINT32)1U << (4U * (i + 1U))),RegNum, 0U, 0U, 0U);
                        if ((ModeID == MX01_IMX390_1920_1080_30P_HDR1) || (ModeID == MX01_IMX390_1920_1080_30P_HDR3)) {               //Case8 - M2 OTP
                            pModeRegTableOTP = &MX01_IMX390_RegTableCase8_OTPM2[i];
                            RegNum_OTP = MX01_IMX390_NUM_REG_CASE8_OTPM2;
                            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] SensorID 0x%x CASE8M2OTP (RegTableSize: %u)", ((UINT32)1U << (4U * (i + 1U))),MX01_IMX390_NUM_REG_CASE8_OTPM2, 0U, 0U, 0U);
                        } else {                                               //Case8 - M4 OTP
                            pModeRegTableOTP = &MX01_IMX390_RegTableCase8_OTPM4[i];
                            RegNum_OTP = MX01_IMX390_NUM_REG_CASE8_OTPM4;
                            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390] SensorID 0x%x CASE8M4OTP (RegTableSize: %u)",((UINT32)1U << (4U * (i + 1U))), MX01_IMX390_NUM_REG_CASE8_OTPM4, 0U, 0U, 0U);
                        }
                    } else {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_96712_IMX390]  SensorID 0x%x CASE8", ((UINT32)1U << (4U * (i + 1U))), 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
    }

    if(Chan_t.SensorID != 0U) {
        RetVal |= MX01_IMX390_SetReadoutMode(pChan, ModeID, pModeRegTable, pModeRegTableOTP, RegNum, RegNum_OTP, OTP_F);

    } else {
        RetVal |= SENSOR_ERR_INVALID_API;
    }

    /* Reset current AE information */
    if (AmbaWrap_memset(MX01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl, 0x0, sizeof(UINT32) * 3U * MX01_IMX390_NUM_MAX_SENSOR_COUNT) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memset(MX01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl, 0x0, sizeof(UINT32) * 3U * MX01_IMX390_NUM_MAX_SENSOR_COUNT) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memset(MX01_IMX390Ctrl[pChan->VinID].CurrentWbCtrl, 0x0, sizeof(AMBA_SENSOR_WB_CTRL_s) * 3U * MX01_IMX390_NUM_MAX_SENSOR_COUNT) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    if (AmbaWrap_memset(MX01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl, 0x0, sizeof(UINT32) * 3U * MX01_IMX390_NUM_MAX_SENSOR_COUNT) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX01_IMX390_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;

    for (i = 0; i < MX01_IMX390_NUM_MODE; i++) {

        MX01_IMX390ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)MX01_IMX390_SensorInfo[i].LineLengthPck /
                                             ((DOUBLE)MX01_IMX390_SensorInfo[i].DataRate *
                                              (DOUBLE)MX01_IMX390_SensorInfo[i].NumDataLanes /
                                              (DOUBLE)MX01_IMX390_SensorInfo[i].NumDataBits));
        pOutputInfo = &MX01_IMX390_OutputInfo[i];
        MX01_IMX390ModeInfoList[i].PixelRate = (FLOAT) ((DOUBLE) pOutputInfo->DataRate *
                                               (DOUBLE)pOutputInfo->NumDataLanes /
                                               (DOUBLE)pOutputInfo->NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_Init
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
static UINT32 MX01_IMX390_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const MX01_IMX390_FRAME_TIMING_s *pFrameTime = &MX01_IMX390ModeInfoList[0U].FrameTime;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);

        MX01_IMX390_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_Enable
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
static UINT32 MX01_IMX390_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX390] Enable", NULL, NULL, NULL, NULL, NULL);


    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX01_IMX390_ResetSensor(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_Disable
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
static UINT32 MX01_IMX390_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_96712_IMX390] Disable", NULL, NULL, NULL, NULL, NULL);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX01_IMX390_StandbyOn(pChan);
    }


    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_GetStatus
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
static UINT32 MX01_IMX390_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &MX01_IMX390Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_GetModeInfo
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
static UINT32 MX01_IMX390_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s SensorConfig;

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        SensorConfig = *pMode;
        if (SensorConfig.ModeID == AMBA_SENSOR_CURRENT_MODE) {
            SensorConfig.ModeID = MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        }

        if (SensorConfig.ModeID >= MX01_IMX390_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            RetVal = MX01_IMX390_PrepareModeInfo(pChan, &SensorConfig, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_GetDeviceInfo
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
static UINT32 MX01_IMX390_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pDeviceInfo, &MX01_IMX390_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_GetHdrInfo
 *
 *  @Description:: Get Sensor Current HDR Information
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      *ShutterCtrl:    Electronic shutter control
 *  @Output     ::
 *      pHdrInfo:        Sensor HDR information
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 MaxExposureLine, NumExposureStepPerFrame;
    UINT32 ModeID, i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pHdrInfo, &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        } else {
            ModeID = MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
            NumExposureStepPerFrame = ((pShutterCtrl[0] / MX01_IMX390_HdrInfo[ModeID].ChannelInfo[0].NumExposureStepPerFrame) + 1U) * MX01_IMX390_HdrInfo[ModeID].ChannelInfo[0].NumExposureStepPerFrame;
            MaxExposureLine = NumExposureStepPerFrame - 2U;

            /* update exp info */
            for (i = 0U; i < pHdrInfo->ActiveChannels; i++) {
                pHdrInfo->ChannelInfo[i].MaxExposureLine = MaxExposureLine;
                pHdrInfo->ChannelInfo[i].NumExposureStepPerFrame = NumExposureStepPerFrame;
            }
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_GetCurrentGainFactor
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
static UINT32 MX01_IMX390_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 i, k, TotalGainCtrl;
    DOUBLE GainFactor64 = 0.0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    TotalGainCtrl = (MX01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] +
                                     MX01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]);
                    if (AmbaWrap_pow(10.0, 0.015 * (DOUBLE)TotalGainCtrl, &GainFactor64) != ERR_NONE) {
                        RetVal = SENSOR_ERR_UNEXPECTED;
                    }
                    pGainFactor[0] = (FLOAT)GainFactor64;
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        TotalGainCtrl = (MX01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] +
                                         MX01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]);
                        if (AmbaWrap_pow(10.0, 0.015 * (DOUBLE)TotalGainCtrl, &GainFactor64) != ERR_NONE) {
                            RetVal = SENSOR_ERR_UNEXPECTED;
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
 *  @RoutineName:: MX01_IMX390_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pExposureTime:    pointer to current exposure time
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 FrameLengthLines;
    UINT32 NumExposureStepPerFrame;
    DOUBLE ShutterTimeUnit;
    UINT8 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        FrameLengthLines = pModeInfo->FrameLengthLines;
        NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    pExposureTime[0] = ((FLOAT)ShutterTimeUnit * (FLOAT)MX01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][0]);
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pExposureTime[k] = ((FLOAT)ShutterTimeUnit * (FLOAT)MX01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][k]);
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
 *  @RoutineName:: MX01_IMX390_ConvertHdrGain
 *
 *  @Description:: Convert gain factor to analog and digital gain control for Linear mode
 *
 *  @Input      ::
 *      DesiredFactor:      Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      Achievable gain factor (12.20 fixed point)
 *      pGainCtrl:          Analog/Digital/WB gain control for achievable gain factor
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_ConvertHdrGain(AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl, FLOAT Desire_dB_H, FLOAT Desire_dB_L)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT Agc_dB_SP1H = 0.0f, Agc_dB_SP1L = 0.0f;
    FLOAT Dgc_dB = 0.0f;
    DOUBLE GainInDb;
    DOUBLE ActualFactor64 = 0.0;
    FLOAT Desire_dB_SP1H, Desire_dB_SP1L;

    /************************************** Agc & Dgc **************************************/
    /* Maximum check */
    Desire_dB_SP1H = (Desire_dB_H >= (FLOAT)MX01_IMX390_MAX_TOTAL_GAIN) ? (FLOAT)MX01_IMX390_MAX_TOTAL_GAIN : Desire_dB_H;
    Desire_dB_SP1L = (Desire_dB_L >= (FLOAT)MX01_IMX390_MAX_TOTAL_GAIN) ? (FLOAT)MX01_IMX390_MAX_TOTAL_GAIN : Desire_dB_L;

    /* Minimum check */
    Desire_dB_SP1H = (Desire_dB_SP1H <= 0.0f) ? 0.0f : Desire_dB_SP1H;
    Desire_dB_SP1L = (Desire_dB_SP1L <= 0.0f) ? 0.0f : Desire_dB_SP1L;

    /* Maximum ABS between SP1H and SP1L is 30dB */
    if ((Desire_dB_SP1H - Desire_dB_SP1L) >= (FLOAT)MX01_IMX390_MAX_AGAIN) {
        Desire_dB_SP1L = Desire_dB_SP1H - (FLOAT)MX01_IMX390_MAX_AGAIN;
    }
    if ((Desire_dB_SP1H - Desire_dB_SP1L) <= -(FLOAT)MX01_IMX390_MAX_AGAIN) {
        Desire_dB_SP1L = Desire_dB_SP1H + (FLOAT)MX01_IMX390_MAX_AGAIN;
    }

    /* Calculate Agc/Dgc for SP1H/SP1L */
    if ((Desire_dB_SP1H <= (FLOAT)MX01_IMX390_MAX_AGAIN) && (Desire_dB_SP1L <= (FLOAT)MX01_IMX390_MAX_AGAIN)) {
        Agc_dB_SP1H = Desire_dB_SP1H;
        Agc_dB_SP1L = Desire_dB_SP1L;
        Dgc_dB = 0.0f;
    } else if ((Desire_dB_SP1H >= (FLOAT)MX01_IMX390_MAX_AGAIN) && (Desire_dB_SP1L >= (FLOAT)MX01_IMX390_MAX_AGAIN)) {
        if (Desire_dB_SP1H > Desire_dB_SP1L) {
            Agc_dB_SP1H = (FLOAT)MX01_IMX390_MAX_AGAIN;
            Agc_dB_SP1L = (FLOAT)MX01_IMX390_MAX_AGAIN - (Desire_dB_SP1H - Desire_dB_SP1L);
            Dgc_dB = Desire_dB_SP1H - (FLOAT)MX01_IMX390_MAX_AGAIN;
        } else {
            Agc_dB_SP1H = (FLOAT)MX01_IMX390_MAX_AGAIN - (Desire_dB_SP1L - Desire_dB_SP1H);
            Agc_dB_SP1L = (FLOAT)MX01_IMX390_MAX_AGAIN;
            Dgc_dB = Desire_dB_SP1L - (FLOAT)MX01_IMX390_MAX_AGAIN;
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

    if (AmbaWrap_floor((DOUBLE)(Agc_dB_SP1H / 0.3), &GainInDb) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
    if (AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.3), &GainInDb) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
    if (AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)pGainCtrl->AnalogGain[0] + (DOUBLE)pGainCtrl->DigitalGain[0]), &ActualFactor64) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pActualFactor->Gain[0] = (FLOAT) ActualFactor64;

    if (AmbaWrap_floor((DOUBLE)(Agc_dB_SP1L / 0.3), &GainInDb) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pGainCtrl->AnalogGain[1] = (UINT32)GainInDb;
    if (AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.3), &GainInDb) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pGainCtrl->DigitalGain[1] = (UINT32)GainInDb;
    if (AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)pGainCtrl->AnalogGain[1] + (DOUBLE)pGainCtrl->DigitalGain[1]), &ActualFactor64) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pActualFactor->Gain[1] = (FLOAT) ActualFactor64;

    pGainCtrl->AnalogGain[2] = pGainCtrl->AnalogGain[1];
    pGainCtrl->DigitalGain[2] = pGainCtrl->DigitalGain[1];
    pActualFactor->Gain[2] = pActualFactor->Gain[1];

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_ConvertWbGainFactor
 *
 *  @Description:: Convert Wbgain factor control
 *
 *  @Input      ::
 *      DesiredFactor:      Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      Achievable gain factor (12.20 fixed point)
 *      pGainCtrl:          Analog/Digital/WB gain control for achievable gain factor
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_ConvertWbGainFactor(const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl, UINT32 HdrType)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_WB_GAIN_FACTOR_s WbGain;
    DOUBLE WbGainInDb;

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

    if (AmbaWrap_floor((DOUBLE)(WbGain.R * 256.0), &WbGainInDb) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pGainCtrl->WbGain[0].R = (UINT32)WbGainInDb;
    if (AmbaWrap_floor((DOUBLE)(WbGain.Gr * 256.0), &WbGainInDb) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pGainCtrl->WbGain[0].Gr = (UINT32)WbGainInDb;
    if (AmbaWrap_floor((DOUBLE)(WbGain.Gb * 256.0), &WbGainInDb) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pGainCtrl->WbGain[0].Gb = (UINT32)WbGainInDb;
    if (AmbaWrap_floor((DOUBLE)(WbGain.B * 256.0), &WbGainInDb) != ERR_NONE) {
        RetVal |= SENSOR_ERR_UNEXPECTED;
    }
    pGainCtrl->WbGain[0].B = (UINT32)WbGainInDb;

    pActualFactor->WbGain[0].R  = (FLOAT)pGainCtrl->WbGain[0].R  / 256.0f;
    pActualFactor->WbGain[0].Gr = (FLOAT)pGainCtrl->WbGain[0].Gr / 256.0f;
    pActualFactor->WbGain[0].Gb = (FLOAT)pGainCtrl->WbGain[0].Gb / 256.0f;
    pActualFactor->WbGain[0].B  = (FLOAT)pGainCtrl->WbGain[0].B  / 256.0f;

    if (HdrType != AMBA_SENSOR_HDR_NONE) {
        if (AmbaWrap_memcpy(&pGainCtrl->WbGain[1], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s)) != ERR_NONE) {
            RetVal |= SENSOR_ERR_UNEXPECTED;
        }
        if (AmbaWrap_memcpy(&pGainCtrl->WbGain[2], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s)) != ERR_NONE) {
            RetVal |= SENSOR_ERR_UNEXPECTED;
        }

        if (AmbaWrap_memcpy(&pActualFactor->WbGain[1], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s)) != ERR_NONE) {
            RetVal |= SENSOR_ERR_UNEXPECTED;
        }
        if (AmbaWrap_memcpy(&pActualFactor->WbGain[2], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_ConvertGainFactor
 *
 *  @Description:: Convert gain factor to analog and digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      DesiredFactor:      Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      Achievable gain factor (12.20 fixed point)
 *      pGainCtrl:          Analog/Digital/WB gain control for achievable gain factor
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType = AMBA_SENSOR_HDR_NONE;
    FLOAT Desire_dB_SP1H, Desire_dB_SP1L, Desire_dB_SP1;
    DOUBLE DesiredFactor;
    DOUBLE LogDesiredFactor = 1.0;
    DOUBLE ActualFactor64 = 0.0;
    FLOAT Agc_dB_SP1 = 0.0f;
    FLOAT Dgc_dB = 0.0f;
    DOUBLE GainInDb;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        HdrType = MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
            if (AmbaWrap_log10(DesiredFactor, &LogDesiredFactor) != ERR_NONE) {
                RetVal |= SENSOR_ERR_UNEXPECTED;
            }
            Desire_dB_SP1H = (FLOAT)(20.0 * LogDesiredFactor);

            DesiredFactor = (DOUBLE)pDesiredFactor->Gain[1];
            if (AmbaWrap_log10(DesiredFactor, &LogDesiredFactor) != ERR_NONE) {
                RetVal |= SENSOR_ERR_UNEXPECTED;
            }
            Desire_dB_SP1L = (FLOAT)(20.0 * LogDesiredFactor);

            RetVal |= MX01_IMX390_ConvertHdrGain(pActualFactor, pGainCtrl, Desire_dB_SP1H, Desire_dB_SP1L);

        } else {
            DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
            if (AmbaWrap_log10(DesiredFactor, &LogDesiredFactor) != ERR_NONE) {
                RetVal |= SENSOR_ERR_UNEXPECTED;
            }
            Desire_dB_SP1 = (FLOAT)(20.0 * LogDesiredFactor);

            /* Maximum check */
            Desire_dB_SP1 = (Desire_dB_SP1 >= (FLOAT)MX01_IMX390_MAX_TOTAL_GAIN) ? (FLOAT)MX01_IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1;

            /* Minimum check */
            Desire_dB_SP1 = (Desire_dB_SP1 <= 0.0f) ? 0.0f : Desire_dB_SP1;

            /* Calculate Agc/Dgc for SP1 */
            if (Desire_dB_SP1 <= (FLOAT)MX01_IMX390_MAX_AGAIN) {
                Agc_dB_SP1 = Desire_dB_SP1;
                Dgc_dB = 0.0f;
            } else {
                Agc_dB_SP1 = (FLOAT)MX01_IMX390_MAX_AGAIN;
                Dgc_dB = Desire_dB_SP1 - (FLOAT)MX01_IMX390_MAX_AGAIN;
            }

            if (AmbaWrap_floor((DOUBLE)(Agc_dB_SP1 / 0.3), &GainInDb) != ERR_NONE) {
                RetVal |= SENSOR_ERR_UNEXPECTED;
            }
            pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
            if (AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.3), &GainInDb) != ERR_NONE) {
                RetVal |= SENSOR_ERR_UNEXPECTED;
            }
            pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
            if (AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)pGainCtrl->AnalogGain[0] + (DOUBLE)pGainCtrl->DigitalGain[0]), &ActualFactor64) != ERR_NONE) {
                RetVal |= SENSOR_ERR_UNEXPECTED;
            }
            pActualFactor->Gain[0] = (FLOAT) ActualFactor64;
        }
        RetVal |= MX01_IMX390_ConvertWbGainFactor(pDesiredFactor, pActualFactor, pGainCtrl, HdrType);
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
static UINT32 ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 SensorNum, MaxExposureline, MinExposureline, FrameLengthLines, NumExposureStepPerFrame, ShutterCtrl;
    FLOAT RowTime;
    DOUBLE ShutterTimeUnit, ShutterCtrlInDb;
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    const MX01_IMX390_SENSOR_INFO_s *pSensorInfo = &MX01_IMX390_SensorInfo[pModeInfo->Config.ModeID];

    SensorNum = MX01_IMX390_GetNumActiveSensor(pChan->SensorID);
    FrameLengthLines = pSensorInfo->FrameLengthLines * SensorNum;
    NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    RowTime = MX01_IMX390ModeInfoList[pModeInfo->Config.ModeID].RowTime / (FLOAT)SensorNum;
    ShutterTimeUnit = (DOUBLE)RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

    if (AmbaWrap_floor(((DOUBLE)ExposureTime / ShutterTimeUnit), &ShutterCtrlInDb) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    /* VMAX is 20 bits */
    if (ShutterCtrl > ((0xfffffU / NumExposureStepPerFrame) * NumExposureStepPerFrame)) {
        ShutterCtrl = ((0xfffffU / NumExposureStepPerFrame) * NumExposureStepPerFrame);
    } else if (ShutterCtrl < 1U) {
        ShutterCtrl = 1U;
    } else {
        /* do not need to adjust ShutterCtrl */
    }

#ifdef MX01_IMX390_IN_SLAVE_MODE
    /* (ERRWID_FRONT / 2) < SHSx < (MODE_VMAX * (FMAX + 1)) - 1 - (ERRWID_BACK / 2) */

    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MaxExposureline = (((((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - 2U) - 2U;
        MinExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine + 4U;
        /* For none-HDR mode */
    } else {
        MaxExposureline = (((((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - 2U) - 2U;
        MinExposureline = 1U + 4U;
    }
#else
    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MaxExposureline = ((((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - 2U;
        MinExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
        /* For none-HDR mode */
    } else {
        MaxExposureline = ((((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - 2U;
        MinExposureline = 1U;
    }
#endif

    if (ShutterCtrl > MaxExposureline) {
        ShutterCtrl = MaxExposureline;
    } else if (ShutterCtrl < MinExposureline) {
        ShutterCtrl = MinExposureline;
    } else {
        /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl   = ShutterCtrl;
    *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_ConvertShutterSpeed
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
static UINT32 MX01_IMX390_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = ConvertShutterSpeed(pChan, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                RetVal |= ConvertShutterSpeed(pChan, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        //AmbaPrint("ExposureTime:%f, RowTime: %f, pShutterCtrl:%d", ExposureTime, pModeInfo->RowTime, *pShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_SetAGainCtrl
 *
 *  @Description:: Set analog gain control
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pAnalogGainCtrl: Analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX01_IMX390_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

            /* Update current analog gain control */
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = pAnalogGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX01_IMX390_SetHdrAnalogGainReg(pChan, pAnalogGainCtrl);

            /* Update current analog gain control */
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] = pAnalogGainCtrl[k];
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
 *  @RoutineName:: MX01_IMX390_SetDigitalGainCtrl
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
static UINT32 MX01_IMX390_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX01_IMX390_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]= pDigitalGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX01_IMX390_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]= pDigitalGainCtrl[k];
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
 *  @RoutineName:: MX01_IMX390_SetWbGainCtrl
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
static UINT32 MX01_IMX390_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX01_IMX390_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX01_IMX390Ctrl[pChan->VinID].CurrentWbCtrl[i][0] = pWbGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX01_IMX390_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX01_IMX390Ctrl[pChan->VinID].CurrentWbCtrl[i][k] = pWbGainCtrl[k];
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
 *  @RoutineName:: MX01_IMX390_SetShutterCtrl
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
static UINT32 MX01_IMX390_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;
    UINT32 i, k;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX01_IMX390_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][0] = pShutterCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX01_IMX390_SetHdrShutterReg(pChan, pShutterCtrl);

            /* Update current shutter control */
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][k] = pShutterCtrl[k];
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
 *  @RoutineName:: MX01_IMX390_SetSlowShrCtrl
 *
 *  @Description:: set slow shutter control
 *
 *  @Input      ::
 *      pChan:           Vin ID and sensor ID
 *      SlowShutterCtrl:   Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX01_IMX390_SetSlowShrCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= MX01_IMX390_SetSlowShutterReg(pChan, SlowShutterCtrl);
    }

    return RetVal;
}

#ifdef MX01_IMX390_IN_SLAVE_MODE
static UINT32 MX01_IMX390_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    DOUBLE PeriodInDb;
    const MX01_IMX390_MODE_INFO_s* pModeInfo = &MX01_IMX390ModeInfoList[ModeID];
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg  = {0};

    if (AmbaWrap_floor(((DOUBLE)pModeInfo->FrameTime.FrameRate.NumUnitsInTick * ((DOUBLE) MX01_IMX390_SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameTime.FrameRate.TimeScale)) + 0.5, &PeriodInDb) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    MasterSyncCfg.RefClk            = MX01_IMX390_SensorInfo[ModeID].InputClk;
    MasterSyncCfg.HSync.Period      = (UINT32)PeriodInDb;
    MasterSyncCfg.HSync.PulseWidth  = 8U;
    MasterSyncCfg.HSync.Polarity    = 0U;
    MasterSyncCfg.VSync.Period      = SlowShutterCtrl;
    MasterSyncCfg.VSync.PulseWidth  = 1000U;
    MasterSyncCfg.VSync.Polarity    = 0U;
    MasterSyncCfg.HSyncDelayCycles  = 1U;
    MasterSyncCfg.VSyncDelayCycles  = 0U;
    MasterSyncCfg.ToggleHsyncInVblank  = 1U;

    RetVal |= AmbaVIN_MasterSyncEnable(MX01_IMX390MasterSyncChannel[pChan->VinID], &MasterSyncCfg);

    return RetVal;
}
#endif

#ifdef MX01_IMX390_IN_SLAVE_MODE
static UINT32 MX01_IMX390_SetMasterSyncCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    return MX01_IMX390_ConfigMasterSync(pChan, MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID, SlowShutterCtrl);
}
#endif

static UINT32 MX01_IMX390_GetSerdesStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_SERDES_STATUS_s *pSerdesStatus)
{
    UINT8 i;
    UINT32 RetVal = SENSOR_ERR_NONE;

    MAX9295_96712_SERDES_STATUS_s SerdesStatus[MX01_IMX390_NUM_MAX_SENSOR_COUNT];
    RetVal |= Max9295_96712_GetSerdesStatus(pChan->VinID, MX01_IMX390_GetEnabledLinkID(pChan->SensorID), SerdesStatus);
    for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
        pSerdesStatus[i].LinkLock = SerdesStatus[i].LinkLock;
        pSerdesStatus[i].VideoLock = SerdesStatus[i].VideoLock;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_IMX390_Config
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
static UINT32 MX01_IMX390_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    AMBA_VIN_MIPI_PAD_CONFIG_s MX01_IMX390PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    const UINT8 LaneShift[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0U,
        [AMBA_VIN_CHANNEL1] = 4U,
        [AMBA_VIN_CHANNEL2] = 6U,
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    UINT8 Data;
    AMBA_SENSOR_CHANNEL_s pChan_t;
    UINT32 EnableLink = 0U;
    UINT8 i = 0U;

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Add delay time to wait MAX96712/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"power off", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"power on", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(3);

    if (ModeID >= MX01_IMX390_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  [ MAX9295_96712_IMX390_Config ]  ============", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d ============", ModeID, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "======= Support 1ch only for MX01_IMX390_1936_1106_60P_HDR/MX01_IMX390_1936_1106_30P mode =======", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "======= Support 2ch(AB), 3ch(ABC/ABD), and 4ch(ABCD) for other modes =======", NULL, NULL, NULL, NULL, NULL);

        if(pChan->SensorID == MX01_IMX390_SENSOR_ID_CHAN_A) {
            if((ModeID != MX01_IMX390_1936_1106_60P_HDR) && (ModeID != MX01_IMX390_1936_1106_30P)) {
                AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[Sensor_Config] NG, Sensor ID should be 0x1 for only MX01_IMX390_1936_1106_60P_HDR/MX01_IMX390_1936_1106_30P", NULL, NULL, NULL, NULL, NULL);
            } else {
                //legal sensor ID
            }

        } else if((pChan->SensorID == (MX01_IMX390_SENSOR_ID_CHAN_A | MX01_IMX390_SENSOR_ID_CHAN_B)) ||
                  (pChan->SensorID == (MX01_IMX390_SENSOR_ID_CHAN_A | MX01_IMX390_SENSOR_ID_CHAN_B | MX01_IMX390_SENSOR_ID_CHAN_C)) ||
                  (pChan->SensorID == (MX01_IMX390_SENSOR_ID_CHAN_A | MX01_IMX390_SENSOR_ID_CHAN_B | MX01_IMX390_SENSOR_ID_CHAN_D)) ||
                  (pChan->SensorID == (MX01_IMX390_SENSOR_ID_CHAN_A | MX01_IMX390_SENSOR_ID_CHAN_B | MX01_IMX390_SENSOR_ID_CHAN_C | MX01_IMX390_SENSOR_ID_CHAN_D))) {
            if((ModeID == MX01_IMX390_1936_1106_60P_HDR) || (ModeID == MX01_IMX390_1936_1106_30P)) {
                AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[Sensor_Config] NG, Sensor ID should be 0x1 for MX01_IMX390_1936_1106_60P_HDR/MX01_IMX390_1936_1106_30P", NULL, NULL, NULL, NULL, NULL);
            } else {
                //legal sensor ID
            }
        } else {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Sensor_Config] NG, Sensor ID 0x%x should not supported\n \
                                                           Please use 1ch(A), 2ch(AB), 3ch(ABC or ABD), and 4ch(ABCD)", pChan->SensorID, 0U, 0U, 0U, 0U);
            RetVal = SENSOR_ERR_ARG;
        }

        /* update status */
        RetVal |= MX01_IMX390_PrepareModeInfo(pChan, pMode, pModeInfo);

#ifdef MX01_IMX390_IN_SLAVE_MODE
        (void)AmbaVIN_MasterSyncDisable(MX01_IMX390MasterSyncChannel[pChan->VinID]);
#endif
        /* Adjust mipi-phy parameters */
        MX01_IMX390PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", (UINT32)MX01_IMX390PadConfig.DateRate, 0U, 0U, 0U, 0U);
        MX01_IMX390PadConfig.EnabledPin = (((UINT32)1U << pModeInfo->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX01_IMX390PadConfig);

        /* After reset VIN, Set MAX96712A PWDNB high */
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"PWDN low", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(3);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"PWDN HIGH", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_96712_Init(pChan->VinID, MX01_IMX390_GetEnabledLinkID(pChan->SensorID));

        RetVal |= MX01_IMX390_ConfigSerDes(pChan, ModeID, pModeInfo);

        //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Max9295_96712_SetSensorClk", NULL, NULL, NULL, NULL, NULL);
        //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "; # Link A: MAX9295A MFP2 to output 27M for IMX390", NULL, NULL, NULL, NULL, NULL);
        /* set MAX9295 MFP2 to output sensor clock */
        for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                if (pModeInfo->InputClk == 26973027U) {
                    (void)Max9295_96712_SetSensorClk(pChan->VinID, i, MAX9295_96712_RCLK_OUT_FREQ_26973027);
                } else {
                    (void)Max9295_96712_SetSensorClk(pChan->VinID, i, MAX9295_96712_RCLK_OUT_FREQ_27M);
                }
            }
        }

        (void)MX01_IMX390_ResetSensor(pChan);

        /* program sensor */
        //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "program sensor", NULL, NULL, NULL, NULL, NULL);

        (void)MX01_IMX390_ChangeReadoutMode(pChan, ModeID);

#ifdef MX01_IMX390_IN_SLAVE_MODE
        {


            Data = 0x06U;
            RetVal |= RegRW(pChan, 0x365cU, &Data, 1U);
            Data = 0x06U;
            RetVal |= RegRW(pChan, 0x365eU, &Data, 1U);
            Data = 0x06U;
            RetVal |= RegRW(pChan, 0x23c2U, &Data, 1U);
            Data = 0x01U;
            RetVal |= RegRW(pChan, 0x3650U, &Data, 1U);
        }
#endif
        /* The Data Type value of valid Optical Black (OB) is determined by MIPI CSI-2 Data Format (10 lines) */
        Data = 0x01U;
        RetVal |= RegRW(pChan, 0x2e41, &Data, 1U);

        RetVal |= MX01_IMX390_StandbyOff(pChan);

        (void)AmbaKAL_TaskSleep(22); //To avoid i2c fail for current chip version

        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MX01_IMX390_Config] Please ignore following i2c fail to wait i2c communication ready.", NULL, NULL, NULL, NULL, NULL);
        while(EnableLink != pChan->SensorID) {
            for (i = 0U; i < MX01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if (((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) && ((EnableLink & ((UINT32)1U << (4U * (i + 1U)))) != ((UINT32)1U << (4U * (i + 1U))))) {
                    pChan_t.VinID = pChan->VinID;
                    pChan_t.SensorID = (UINT32)1U << (4U * (i + 1U));
                    (void)MX01_IMX390_RegRead(&pChan_t, 0x5001, &Data);
                    if(Data == 3U) {
                        EnableLink |= (UINT32)1U << (4U * (i + 1U));
                    } else {
                        //check again
                    }
                }
            }

            if(EnableLink == pChan->SensorID) {
                break;
            } else {
                (void)AmbaKAL_TaskSleep(1);
            }
        }
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MX01_IMX390_Config] i2c communication is ready.", NULL, NULL, NULL, NULL, NULL);

#if 0 //no need to do one shot reset per FAE's suggestion


        if(pChan->SensorID != MX01_IMX390_SENSOR_ID_CHAN_A) {
            //do Concatentation enable during MAXIM setting by FAE's suggestion
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Derserializer one-shot link reset", NULL, NULL, NULL, NULL, NULL);
            //Derserializer one-shot link reset
            RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, AMBA_SBRG_MAX96712_ID, 0x0018U, 0x0FU);
            (void)AmbaKAL_TaskSleep(100);

            //Also issue one-shot linke reset for MAX9295A
            if((pChan->SensorID & MX01_IMX390_SENSOR_ID_CHAN_A) != 0U) {
                RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_A_ID, 0x0010U, 0x21U);
                (void)AmbaKAL_TaskSleep(100);
            }
            if((pChan->SensorID & MX01_IMX390_SENSOR_ID_CHAN_B) != 0U) {
                RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_B_ID, 0x0010U, 0x21U);
                (void)AmbaKAL_TaskSleep(100);
            }
            if((pChan->SensorID & MX01_IMX390_SENSOR_ID_CHAN_C) != 0U) {
                RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_C_ID, 0x0010U, 0x21U);
                (void)AmbaKAL_TaskSleep(100);
            }
            if((pChan->SensorID & MX01_IMX390_SENSOR_ID_CHAN_D) != 0U) {
                RetVal |= Max9295_96712_RegisterWrite(pChan->VinID, MX01_MAX9295_D_ID, 0x0010U, 0x21U);
                (void)AmbaKAL_TaskSleep(100);
            }

        }
#endif

#ifdef MX01_IMX390_IN_SLAVE_MODE
        RetVal |= MX01_IMX390_ConfigMasterSync(pChan, ModeID, 1U);
#endif

        //mask the IMX390 MIPI frame output
        //set MUTE_VSYNC_MASK_EN=1
        Data = ((UINT8)1U << 2U);
        RetVal |= RegRW(pChan, 0x0088U, &Data, 1U);
        //set SM_MUTE_VSYNC_MASK_EN_APL=1
        Data = ((UINT8)1U << 6U);
        RetVal |= RegRW(pChan, 0x03C0U, &Data, 1U);

        /* config vin */
        RetVal |= MX01_IMX390_ConfigVin(pChan->VinID, pModeInfo);

        //waits two frame time until mask IMX390 mipi output
        (void)AmbaKAL_TaskSleep(2U * 1000U * MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.NumUnitsInTick
                                / MX01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "leave sensor config", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}


static UINT32 MX01_IMX390_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 Data;

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "MX01_IMX390_ConfigPost Vin %d Sensor 0x%x", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);
    //unmask the IMX390 MIPI frame output
    //set MUTE_VSYNC_MASK_EN=0
    Data = 0x0;
    RetVal |= RegRW(pChan, 0x0088U, &Data, 1U);
    //set SM_MUTE_VSYNC_MASK_EN_APL=0
    RetVal |= RegRW(pChan, 0x03C0U, &Data, 1U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX01_IMX390Obj = {
    .SensorName             = "IMX390",
    .SerdesName             = "MAX9295_96712",
    .Init                   = MX01_IMX390_Init,
    .Enable                 = MX01_IMX390_Enable,
    .Disable                = MX01_IMX390_Disable,
    .Config                 = MX01_IMX390_Config,
    .GetStatus              = MX01_IMX390_GetStatus,
    .GetModeInfo            = MX01_IMX390_GetModeInfo,
    .GetDeviceInfo          = MX01_IMX390_GetDeviceInfo,
    .GetHdrInfo             = MX01_IMX390_GetHdrInfo,
    .GetCurrentGainFactor   = MX01_IMX390_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX01_IMX390_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX01_IMX390_ConvertGainFactor,
    .ConvertShutterSpeed    = MX01_IMX390_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX01_IMX390_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MX01_IMX390_SetDigitalGainCtrl,
    .SetWbGainCtrl          = MX01_IMX390_SetWbGainCtrl,
    .SetShutterCtrl         = MX01_IMX390_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX01_IMX390_SetSlowShrCtrl,
#ifdef MX01_IMX390_IN_SLAVE_MODE
    .SetMasterSyncCtrl      = MX01_IMX390_SetMasterSyncCtrl,
#else
    .SetMasterSyncCtrl      = NULL,
#endif

    .RegisterRead           = MX01_IMX390_RegisterRead,
    .RegisterWrite          = MX01_IMX390_RegisterWrite,

    .ConfigPost             = MX01_IMX390_ConfigPost,
    .GetSerdesStatus        = MX01_IMX390_GetSerdesStatus,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};

