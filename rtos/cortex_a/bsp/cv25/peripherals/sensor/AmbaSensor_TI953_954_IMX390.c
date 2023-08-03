/**
 *  @file AmbaSensor_TI953_954_IMX390.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Control APIs of TI953_954 plus SONY IMX390 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaMisraFix.h"

#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_TI953_954_IMX390.h"

#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSbrg_TI953_954.h"

#include "bsp.h"

#define TI01_IMX390_IN_SLAVE_MODE
#define TI01_IMX390_I2C_WR_BUF_SIZE  64U

/*-----------------------------------------------------------------------------------------------*\
 * TI01_IMX390 sensor I2C and MSync channel info
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI01_IMX390I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_I2C_CHANNEL_PIP2,
};

/*-----------------------------------------------------------------------------------------------*\
 * TI01_IMX390 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static TI01_IMX390_CTRL_s TI01_IMX390Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};

/*-----------------------------------------------------------------------------------------------*\
 * TI01_IMX390 serdes config info
\*-----------------------------------------------------------------------------------------------*/
static TI953_954_SERDES_CONFIG_s TI01_IMX390SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = {
        .EnabledRxPortID = TI953_954_RX_PORT_0,
        .SensorSlaveID = TI01_IMX390_SENSOR_SLAVE_ADDR,
        .SensorAliasID = {
            [0] = 0x46U,
            [1] = 0x48U,
        },
    },

    [AMBA_VIN_CHANNEL1] = {
        .EnabledRxPortID = TI953_954_RX_PORT_0,
        .SensorSlaveID = TI01_IMX390_SENSOR_SLAVE_ADDR,
        .SensorAliasID = {
            [0] = 0x4AU,
            [1] = 0x4CU,
        },
    },

    [AMBA_VIN_CHANNEL2] = {
        .EnabledRxPortID = TI953_954_RX_PORT_0,
        .SensorSlaveID = TI01_IMX390_SENSOR_SLAVE_ADDR,
        .SensorAliasID = {
            [0] = 0x46U,
            [1] = 0x48U,
        },
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * TI01_IMX390 APIs
\*-----------------------------------------------------------------------------------------------*/

static UINT16 TI01_IMX390_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}

static UINT32 TI01_IMX390_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            SensorNum++;
        }
    }

    if(SensorNum <= 1U) {
        SensorNum = 1U;
    }

    return SensorNum;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI01_IMX390_PrepareModeInfo
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
static void TI01_IMX390_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID = pMode->ModeID;
    const AMBA_VIN_FRAME_RATE_s *pFrameRate = &TI01_IMX390ModeInfoList[ModeID].FrameRate;
    const TI01_IMX390_SENSOR_INFO_s *pSensorInfo = &TI01_IMX390SensorInfo[ModeID];
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo = &TI01_IMX390InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &TI01_IMX390OutputInfo[ModeID];
    UINT32 SensorNum = TI01_IMX390_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;

    pModeInfo->FrameLengthLines        = pSensorInfo->FrameLengthLines * SensorNum;
    pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    pModeInfo->InputClk                = pSensorInfo->InputClk;
    pModeInfo->RowTime                 = TI01_IMX390ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
    (void) AmbaWrap_floor(((DOUBLE)TI01_IMX390ModeInfoList[ModeID].PixelRate * ((DOUBLE)pModeInfo->RowTime * (DOUBLE)SensorNum)) + 0.5, &FloorVal);
    pModeInfo->LineLengthPck           = (UINT32)FloorVal;

    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, pFrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, pFrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->HdrInfo, &TI01_IMX390HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* update for TI954 CSI-2 output */
    pModeInfo->OutputInfo.DataRate *= SensorNum;
    pModeInfo->OutputInfo.OutputHeight *= (UINT16)SensorNum;
    pModeInfo->OutputInfo.RecordingPixels.StartY *= (UINT16)SensorNum;
    pModeInfo->OutputInfo.RecordingPixels.Height *= (UINT16)SensorNum;

    /* Updated minimum frame rate limitation */
    /*
    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        pModeInfo->MinFrameRate.NumUnitsInTick *= 8U;
    }
    */
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI01_IMX390_ConfigVin
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
static UINT32 TI01_IMX390_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s TI01_IMX390VinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace         = AMBA_VIN_COLOR_SPACE_RGB,
            .BayerPattern       = AMBA_SENSOR_BAYER_PATTERN_RG,
            .NumDataBits        = 0,
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
        .NumActiveLanes         = 4U,
        .DataTypeMask           = 0x3fU,
        .DataType               = 0x00,
        .VirtChanHDREnable      = 0U,
        .VirtChanHDRConfig      = {0},
    };

    const AMBA_SENSOR_OUTPUT_INFO_s  *pOutputInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s *pVinCfg = &TI01_IMX390VinConfig;

    pVinCfg->NumActiveLanes = pOutputInfo->NumDataLanes;
    pVinCfg->Config.NumDataBits = pOutputInfo->NumDataBits;
    pVinCfg->Config.BayerPattern = pOutputInfo->BayerPattern;

    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels  = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines   = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    (void)AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    return AmbaVIN_MipiConfig(pChan->VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI01_IMX390_RegWrite
 *
 *  @Description:: Write sensor registers through I2C bus
 *
 *  @Input      ::
 *      Addr:      Register address
 *      Data:      Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI01_IMX390_RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal = SENSOR_ERR_ARG;
    UINT32 i;
    UINT8 TxDataBuf[TI01_IMX390_SENSOR_I2C_MAX_SIZE + 2U];
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT32 TxSize;

    if ((pChan == NULL) || (Size > TI01_IMX390_SENSOR_I2C_MAX_SIZE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.DataSize = (Size + 2U);
        I2cConfig.pDataBuf = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);

        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

#if 0
        /* Broadcast has not been verified on TI953/954 */
        if (pChan->SensorID == (TI01_IMX390_SENSOR_ID_CHAN_A | TI01_IMX390_SENSOR_ID_CHAN_B)) {
            I2cConfig.SlaveAddr = TI01_IMX390SerdesConfig[pChan->VinID].SensorSlaveID;
            RetVal = AmbaI2C_MasterWrite(TI01_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD, &I2cConfig, &TxSize, 1000U);

            if (RetVal != I2C_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work!!!!! (SlaveID=0x%02x)", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U);
            }

        } else {
#endif
            for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = TI01_IMX390SerdesConfig[pChan->VinID].SensorAliasID[i];
                    RetVal = AmbaI2C_MasterWrite(TI01_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD, &I2cConfig, &TxSize, 1000U);

                    if (RetVal != I2C_ERR_NONE) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work!!!!! (SlaveID=0x%02x)", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_RegRead
     *
     *  @Description:: Read sensor registers through I2C bus
     *
     *  @Input      ::
     *      Addr:      Register address
     *      pRxData:   Pointer to rx data buffer
     *
     *  @Output     :: none
     *
     *  @Return     :: none
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData) {
        UINT32 RetVal;
        UINT32 i;
        AMBA_I2C_TRANSACTION_s I2cTxConfig;
        AMBA_I2C_TRANSACTION_s I2CRxConfig;
        UINT8 TxData[2];
        UINT32 TxSize;

        if ((pChan->SensorID != TI01_IMX390_SENSOR_ID_CHAN_A) && (pChan->SensorID != TI01_IMX390_SENSOR_ID_CHAN_B)) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            I2cTxConfig.DataSize = 2U;
            I2cTxConfig.pDataBuf = TxData;
            TxData[0] = (UINT8)(Addr >> 8U);
            TxData[1] = (UINT8)(Addr & 0xffU);

            I2CRxConfig.DataSize = 1U;
            I2CRxConfig.pDataBuf = pRxData;

            for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cTxConfig.SlaveAddr = TI01_IMX390SerdesConfig[pChan->VinID].SensorAliasID[i];
                    I2CRxConfig.SlaveAddr = (TI01_IMX390SerdesConfig[pChan->VinID].SensorAliasID[i] | (UINT32)0x1U);
                    break;
                }
            }

            RetVal = AmbaI2C_MasterReadAfterWrite(TI01_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD,
                                                  1U, &I2cTxConfig, &I2CRxConfig, &TxSize, 1000U);

            if (RetVal != I2C_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C read does not work!!!!! (Addr=0x%04x)", Addr, 0U, 0U, 0U, 0U);
            }
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_RegisterWrite
     *
     *  @Description:: Write sensor registers API
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
    static UINT32 TI01_IMX390_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data) {
        UINT32 RetVal;
        UINT8 WriteData = (UINT8)Data;

        RetVal = TI01_IMX390_RegWrite(pChan, Addr, &WriteData, 1U);

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_RegisterRead
     *
     *  @Description:: Read sensor registers API
     *
     *  @Input      ::
     *      Chan:      Vin ID and sensor ID
     *      Addr:      Register address
     *      pData:     Pointer to data value
     *
     *  @Output     :: none
     *
     *  @Return     ::
     *      INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData) {
        UINT32 RetVal;
        UINT8 RData = 0U;

        RetVal = TI01_IMX390_RegRead(pChan, Addr, &RData);
        *pData = RData;

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_SetAnalogGainReg
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
    static UINT32 TI01_IMX390_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl) {
        UINT32 RetVal;
        UINT8 WData[2];

        /* SP1L */
        WData[0] = (UINT8)(AnalogGainCtrl & 0xffU);
        WData[1] = (UINT8)((AnalogGainCtrl >> 8U) & 0x7U);
        RetVal = TI01_IMX390_RegWrite(pChan, TI01_IMX390_AGAIN_SP1L, WData, 2U);

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_SetDigitalGainReg
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
    static UINT32 TI01_IMX390_SetDigitalGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 DigitalGainCtrl) {
        UINT32 RetVal;
        UINT8 WData[2];

        /* SP1L */
        WData[0] = (UINT8)(DigitalGainCtrl & 0xffU);
        WData[1] = (UINT8)((DigitalGainCtrl >> 8U) & 0x7U);
        RetVal = TI01_IMX390_RegWrite(pChan, TI01_IMX390_PGA_SP1H, WData, 2U);

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_SetWbGainReg
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
    static UINT32 TI01_IMX390_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl) {
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
        RetVal |= TI01_IMX390_RegWrite(pChan, TI01_IMX390_WBGAIN_R, WData, 8U);

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_SetShutterReg
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
    static UINT32 TI01_IMX390_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 ShutterCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        UINT32 SHS1;
        UINT8 WData[4];

        /* SP1H & SP1L */
        SHS1 = TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - ShutterCtrl;

        WData[0] = (UINT8)(SHS1 & 0xffU);
        WData[1] = (UINT8)((SHS1 >> 8U) & 0xffU);
        WData[2] = (UINT8)((SHS1 >> 16U) & 0xfU);
        RetVal |= TI01_IMX390_RegWrite(pChan, TI01_IMX390_SHS1, WData, 3U);

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_SetHdrAnalogGainReg
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
    static UINT32 TI01_IMX390_SetHdrAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        UINT8 WData[2];

        /* SP1H */
        WData[0] = (UINT8)(pAnalogGainCtrl[0] & 0xffU);
        WData[1] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x7U);
        RetVal |= TI01_IMX390_RegWrite(pChan, TI01_IMX390_AGAIN_SP1H, WData, 2U);

        /* SP1L */
        WData[0] = (UINT8)(pAnalogGainCtrl[1] & 0xffU);
        WData[1] = (UINT8)((pAnalogGainCtrl[1] >> 8U) & 0x7U);
        RetVal |= TI01_IMX390_RegWrite(pChan, TI01_IMX390_AGAIN_SP1L, WData, 2U);

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_SetHdrShutterReg
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
    static UINT32 TI01_IMX390_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        UINT32 SHSX;
        UINT8 WData[4];

        /* SP1H & SP1L */
        SHSX = TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - pShutterCtrl[0];

        WData[0] = (UINT8)(SHSX & 0xffU);
        WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
        WData[2] = (UINT8)((SHSX >> 16U) & 0xfU);
        RetVal |= TI01_IMX390_RegWrite(pChan, TI01_IMX390_SHS1, WData, 3U);

        /* SP2 */
        SHSX = TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - pShutterCtrl[2];

        WData[0] = (UINT8)(SHSX & 0xffU);
        WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
        WData[2] = (UINT8)((SHSX >> 16U) & 0xfU);
        RetVal |= TI01_IMX390_RegWrite(pChan, TI01_IMX390_SHS2, WData, 3U);

        return RetVal;
    }


    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_SetSlowShutterReg
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
    static UINT32 TI01_IMX390_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame) {
        AmbaMisra_TouchUnused(&IntegrationPeriodInFrame);
        return SENSOR_ERR_NONE;
#if 0
        UINT32 TargetFrameLengthLines;
        UINT32 ModeID = TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        UINT32 RetVal = SENSOR_ERR_NONE;
        UINT8 TxData[1];

        if (IntegrationPeriodInFrame < 1U) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            TargetFrameLengthLines = TI01_IMX390ModeInfoList[ModeID].FrameTime.VMAX * IntegrationPeriodInFrame;

            if (TargetFrameLengthLines > 0x3ffffU) {
                TargetFrameLengthLines = 0x3ffffU;
            }

            TxData[0] = IntegrationPeriodInFrame - 1;

            RetVal |= RegWrite(pChan, TI01_IMX390_FMAX, TxData, 1U);

            /* Update frame rate information */
            TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
            TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
            TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale = TI01_IMX390ModeInfoList[ModeID].FrameTime.FrameRate.TimeScale / IntegrationPeriodInFrame;
        }

        return RetVal;
#endif
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_SetStandbyOn
     *
     *  @Description:: Configure sensor into standby mode
     *
     *  @Input      :: none
     *
     *  @Output     :: none
     *
     *  @Return     :: none
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan) {
        UINT32 RetVal;
        UINT8 TxData = 0x01U;

        RetVal = TI01_IMX390_RegWrite(pChan, TI01_IMX390_STANDBY, &TxData, 1U);

        if (RetVal != SENSOR_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX390] TI01_IMX390_SetStandbyOn failed!!", NULL, NULL, NULL, NULL, NULL);
        } else {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX390] TI01_IMX390_SetStandbyOn", NULL, NULL, NULL, NULL, NULL);
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_SetStandbyOff
     *
     *  @Description:: Configure sensor to leave standby mode
     *
     *  @Input      :: none
     *
     *  @Output     :: none
     *
     *  @Return     :: none
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        UINT8 TxData = 0x00U;
        UINT32 i;
        AMBA_SENSOR_CHANNEL_s TempChan;

        (void)AmbaWrap_memcpy(&TempChan, pChan, sizeof(AMBA_SENSOR_CHANNEL_s));

        for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
            if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                TempChan.SensorID = (UINT32)1U << (4U * (i + 1U));
                RetVal |= TI01_IMX390_RegWrite(&TempChan, TI01_IMX390_STANDBY, &TxData, 1U);
                (void)AmbaKAL_TaskSleep(50);
            }
        }

        if (RetVal != SENSOR_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX390] TI01_IMX390_SetStandbyOff failed!!!", NULL, NULL, NULL, NULL, NULL);
        } else {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX390] TI01_IMX390_SetStandbyOff", NULL, NULL, NULL, NULL, NULL);
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_Query_VerID
     *
     *  @Description:: Check sensor chip version
     *
     *  @Input      :: none
     *
     *  @Output     ::
     *      pVerID:    pointer to TI01_IMX390 version ID
     *
     *  @Return     ::
     *      INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_Query_VerID(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 *pVerID) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        UINT8 Reg0x3060 = 0xffU, Reg0x3067 = 0xffU, Reg0x3064 = 0xffU;

        if (pVerID == NULL) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            RetVal |= TI01_IMX390_RegRead(pChan, 0x3060U, &Reg0x3060);
            RetVal |= TI01_IMX390_RegRead(pChan, 0x3067U, &Reg0x3067);
            RetVal |= TI01_IMX390_RegRead(pChan, 0x3064U, &Reg0x3064);

            if (RetVal == SENSOR_ERR_NONE) {

                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI01_IMX390] Reg0x3060: 0x%02x, Reg0x3067: 0x%02x, Reg0x3064: 0x%02x", Reg0x3060, Reg0x3067, Reg0x3064, 0U, 0U);

                if ((Reg0x3060 == 0x70U) && ((Reg0x3067 >> 4U) == 0x5U)) {
                    *pVerID = TI01_IMX390_CHIP_VERSION_60DEG;
                } else if ((Reg0x3060 == 0x70U) || (Reg0x3060 == 0x71U)) {
                    *pVerID = TI01_IMX390_CHIP_VERSION_CASE5;
                } else if (((Reg0x3060 == 0x01U) && (Reg0x3067 == 0x10U)) || ((Reg0x3060 == 0x00U) && (Reg0x3067 == 0x30U))
                           || (((Reg0x3060 == 0x02U) && (Reg0x3067 == 0x30U)) && ((Reg0x3064 == 0x05U) || (Reg0x3064 == 0x06U) || (Reg0x3064 == 0x07U) || (Reg0x3064 == 0x14U) || (Reg0x3064 == 0x15U)))) {
                    *pVerID = TI01_IMX390_CHIP_VERSION_CASE7;
                } else if (((Reg0x3060 == 0x00U) && (Reg0x3067 == 0x40U)) || (((Reg0x3060 == 0x02U) && (Reg0x3067 == 0x30U)) && ((Reg0x3064 == 0x08U) || (Reg0x3064 == 0x09U) || (Reg0x3064 == 0x0AU) || (Reg0x3064 == 0x0BU) || (Reg0x3064 == 0x0CU)))) {
                    *pVerID = TI01_IMX390_CHIP_VERSION_CASE8;
                } else {
                    *pVerID = TI01_IMX390_CHIP_VERSION_CASE8;
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  Unknown chip version  ============", 0U, 0U, 0U, 0U, 0U);

                }
            }
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_ChangeReadoutMode
     *
     *  @Description:: Switch sensor to requested mode.
     *                 It's for the case that sensor have been initialized.
     *
     *  @Input      ::
     *      SensorMode : Sensor mode number
     *
     *  @Output     :: none
     *
     *  @Return     ::
     *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        const TI01_IMX390_REG_s *pModeRegTable = &TI01_IMX390_RegTableCase5[0];
        const TI01_IMX390_SEQ_REG_s *pModeRegTableOTP = &TI01_IMX390_RegTableCase7_OTPM4[0];
        UINT32 VerID = 0U;
        UINT32 RegNum = TI01_IMX390_NUM_REG_CASE5, RegNum_OTP = TI01_IMX390_NUM_REG_CASE7_OTPM4;
        UINT8 OTP_F=0;

        UINT16 i, j, DataSize;
        UINT16 FirstAddr, NextAddr;
        UINT8 FirstData, NextData;
        UINT8 TxData[TI01_IMX390_I2C_WR_BUF_SIZE];


        if (TI01_IMX390_Query_VerID(pChan, &VerID) != SENSOR_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI953_954_IMX390] Unable to get chip version", 0U, 0U, 0U, 0U, 0U);
        } else {
            if (VerID == TI01_IMX390_CHIP_VERSION_60DEG) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI953_954_IMX390] Not support 60 deg setting", 0U, 0U, 0U, 0U, 0U);
            } else if (VerID == TI01_IMX390_CHIP_VERSION_CASE5) {      //Case5
                pModeRegTable = &TI01_IMX390_RegTableCase5[0];
                RegNum = TI01_IMX390_NUM_REG_CASE5;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI953_954_IMX390] IMX390 Ver: case5 (RegTableSize: %u)", RegNum, 0U, 0U, 0U, 0U);
            } else if (VerID == TI01_IMX390_CHIP_VERSION_CASE7) {      //Case 7
                OTP_F=1;
                pModeRegTable = &TI01_IMX390_RegTableCase7[0];
                RegNum = TI01_IMX390_NUM_REG_CASE7;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI953_954_IMX390] IMX390 Ver: case7 (RegTableSize: %u)", RegNum, 0U, 0U, 0U, 0U);
                if ((ModeID == 5U) || (ModeID == 8U)) {               //Case7 - M2 OTP
                    pModeRegTableOTP = &TI01_IMX390_RegTableCase7_OTPM2[0];
                    RegNum_OTP = TI01_IMX390_NUM_REG_CASE7_OTPM2;
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI953_954_IMX390] IMX390 Ver: case7M2OTP (RegTableSize: %u)", TI01_IMX390_NUM_REG_CASE7_OTPM2, 0U, 0U, 0U, 0U);
                } else {                                               //Case7 - M4 OTP
                    pModeRegTableOTP = &TI01_IMX390_RegTableCase7_OTPM4[0];
                    RegNum_OTP = TI01_IMX390_NUM_REG_CASE7_OTPM4;
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI953_954_IMX390] IMX390 Ver: case7M4OTP (RegTableSize: %u)", TI01_IMX390_NUM_REG_CASE7_OTPM4, 0U, 0U, 0U, 0U);
                }
            } else {                                                   //Case8 or unknown
                OTP_F=1;
                pModeRegTable = &TI01_IMX390_RegTableCase8[0];
                RegNum = TI01_IMX390_NUM_REG_CASE8;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI953_954_IMX390] IMX390 Ver: case8 (RegTableSize: %u)", RegNum, 0U, 0U, 0U, 0U);
                if ((ModeID == 5U) || (ModeID == 8U)) {               //Case8 - M2 OTP
                    pModeRegTableOTP = &TI01_IMX390_RegTableCase8_OTPM2[0];
                    RegNum_OTP = TI01_IMX390_NUM_REG_CASE8_OTPM2;
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI953_954_IMX390] IMX390 Ver: case8M2OTP (RegTableSize: %u)", TI01_IMX390_NUM_REG_CASE8_OTPM2, 0U, 0U, 0U, 0U);
                } else {                                               //Case8 - M4 OTP
                    pModeRegTableOTP = &TI01_IMX390_RegTableCase8_OTPM4[0];
                    RegNum_OTP = TI01_IMX390_NUM_REG_CASE8_OTPM4;
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[TI953_954_IMX390] IMX390 Ver: case8M4OTP (RegTableSize: %u)", TI01_IMX390_NUM_REG_CASE8_OTPM4, 0U, 0U, 0U, 0U);
                }
            }
        }

        for (i = 0U; i < (UINT16)RegNum; i += DataSize) {
            DataSize = 0;
            FirstAddr = pModeRegTable[i].Addr;
            FirstData = pModeRegTable[i].Data[ModeID];

            TxData[DataSize] = FirstData;
            DataSize++;

            for (j = i + 1U; j < (UINT16)RegNum; j++) {
                NextAddr = pModeRegTable[j].Addr;
                NextData = pModeRegTable[j].Data[ModeID];

                if (((NextAddr - FirstAddr) != (j - i)) || ((DataSize + 1U) > (UINT16)((UINT16)TI01_IMX390_I2C_WR_BUF_SIZE - 3U))) {
                    break;
                } else {
                    TxData[DataSize] = NextData;
                    DataSize++;
                }
            }

            RetVal = TI01_IMX390_RegWrite(pChan, FirstAddr, TxData, DataSize);
        }

        if (OTP_F == 1U) {
            for (i = 0U; i < (UINT16)RegNum_OTP; i += DataSize) {
                DataSize = 0;
                FirstAddr = pModeRegTableOTP[i].Addr;
                FirstData = pModeRegTableOTP[i].Data;

                TxData[DataSize] = FirstData;
                DataSize++;

                for (j = i + 1U; j < (UINT16)RegNum_OTP; j++) {
                    NextAddr = pModeRegTableOTP[j].Addr;
                    NextData = pModeRegTableOTP[j].Data;

                    if (((NextAddr - FirstAddr) != (j - i)) || ((DataSize + 1U) > (UINT16)((UINT16)TI01_IMX390_I2C_WR_BUF_SIZE - 3U))) {
                        break;
                    } else {
                        TxData[DataSize] = NextData;
                        DataSize++;
                    }
                }

                RetVal = TI01_IMX390_RegWrite(pChan, FirstAddr, TxData, DataSize);
            }
        }

        /* Reset current AE information */
        (void)AmbaWrap_memset(TI01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl, 0x0, sizeof(UINT32) * 3U * TI01_IMX390_NUM_MAX_SENSOR_COUNT);
        (void)AmbaWrap_memset(TI01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl, 0x0, sizeof(UINT32) * 3U * TI01_IMX390_NUM_MAX_SENSOR_COUNT);
        (void)AmbaWrap_memset(TI01_IMX390Ctrl[pChan->VinID].CurrentWbCtrl, 0x0, sizeof(AMBA_SENSOR_WB_CTRL_s) * 3U * TI01_IMX390_NUM_MAX_SENSOR_COUNT);
        (void)AmbaWrap_memset(TI01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl, 0x0, sizeof(UINT32) * 3U * TI01_IMX390_NUM_MAX_SENSOR_COUNT);

        return RetVal;
    }

    static void TI01_IMX390_PreCalculateModeInfo(void) {
        UINT32 i;
        const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;

        for (i = 0U; i < TI01_IMX390_NUM_MODE; i++) {
            TI01_IMX390ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)TI01_IMX390SensorInfo[i].LineLengthPck /
                                                 ((DOUBLE)TI01_IMX390SensorInfo[i].DataRate *
                                                  (DOUBLE)TI01_IMX390SensorInfo[i].NumDataLanes /
                                                  (DOUBLE)TI01_IMX390SensorInfo[i].NumDataBits));
            pOutputInfo = &TI01_IMX390OutputInfo[i];
            TI01_IMX390ModeInfoList[i].PixelRate = (FLOAT)((DOUBLE)pOutputInfo->DataRate * (DOUBLE)pOutputInfo->NumDataLanes / (DOUBLE)pOutputInfo->NumDataBits);
        }
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_Init
     *
     *  @Description:: Initialize sensor driver
     *
     *  @Input      ::
     *      Chan:      Vin ID and sensor ID
     *
     *  @Output     :: none
     *
     *  @Return     ::
     *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_Init(const AMBA_SENSOR_CHANNEL_s *pChan) {
        UINT32 RetVal = SENSOR_ERR_NONE;

        if (pChan == NULL) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            /* TI954 has external input clock source */
            //RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, TI01_IMX390SensorInfo[0U].InputClk);
            TI01_IMX390_PreCalculateModeInfo();
        }

        //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "=============TI01_IMX390_INIT ===========", NULL, NULL, NULL, NULL, NULL);

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_Enable
     *
     *  @Description:: Power on sensor
     *
     *  @Input      ::
     *      Chan:      Vin ID and sensor ID
     *
     *  @Output     :: none
     *
     *  @Return     ::
     *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_Enable(const AMBA_SENSOR_CHANNEL_s *pChan) {
        UINT32 RetVal = SENSOR_ERR_NONE;

        if (pChan == NULL) {
            RetVal = SENSOR_ERR_ARG;
        }

        //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "=============TI01_IMX390_ENABLE ===========", NULL, NULL, NULL, NULL, NULL);

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_Disable
     *
     *  @Description:: Power down sensor
     *
     *  @Input      ::
     *      Chan:      Vin ID and sensor ID
     *
     *  @Output     :: none
     *
     *  @Return     ::
     *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_Disable(const AMBA_SENSOR_CHANNEL_s *pChan) {
        UINT32 RetVal = SENSOR_ERR_NONE;

        if (pChan == NULL) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            RetVal = TI01_IMX390_SetStandbyOn(pChan);
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_GetStatus
     *
     *  @Description:: Get current sensor status
     *
     *  @Input      ::
     *      Chan:       Vin ID and sensor ID
     *
     *  @Output     ::
     *      pStatus:    Pointer to current sensor status
     *
     *  @Return     ::
     *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus) {
        UINT32 RetVal = SENSOR_ERR_NONE;

        if ((pStatus == NULL) || (pChan == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            (void)AmbaWrap_memcpy(pStatus, &TI01_IMX390Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_GetModeInfo
     *
     *  @Description:: Get Mode Info of indicated mode
     *
     *  @Input      ::
     *      Chan:      Vin ID and sensor ID
     *      Mode:      Sensor mode number
     *
     *  @Output     ::
     *      pModeInfo: Pointer to requested Mode Info
     *
     *  @Return     ::
     *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        AMBA_SENSOR_CONFIG_s Config = {0};

        if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
                Config.ModeID = TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
            } else {
                Config.ModeID = pMode->ModeID;
            }
            if (Config.ModeID >= TI01_IMX390_NUM_MODE) {
                RetVal = SENSOR_ERR_ARG;
            } else {
                TI01_IMX390_PrepareModeInfo(pChan, &Config, pModeInfo);
            }
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_GetDeviceInfo
     *
     *  @Description:: Get Sensor Device Info
     *
     *  @Input      ::
     *      Chan:        Vin ID and sensor ID
     *
     *  @Output     ::
     *      pDeviceInfo: Pointer to device info
     *
     *  @Return     ::
     *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo) {
        UINT32 RetVal = SENSOR_ERR_NONE;

        if ((pChan == NULL) || (pDeviceInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            (void)AmbaWrap_memcpy(pDeviceInfo, &TI01_IMX390DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_GetHdrInfo
     *
     *  @Description:: Get Sensor Current HDR information
     *
     *  @Input      ::
     *      Chan:         Vin ID and sensor ID
     *      pShutterCtrl: Pointer to electronic shutter control
     *  @Output     ::
     *      pHdrInfo:     Pointer to sensor HDR information
     *
     *  @Return     ::
     *      INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)

    {
        UINT32 RetVal = SENSOR_ERR_NONE;

        if ((pChan == NULL) || (pShutterCtrl == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            (void)AmbaWrap_memcpy(pHdrInfo, &TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_GetCurrentGainFactor
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
    static UINT32 TI01_IMX390_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        UINT32 i, k, TotalGainCtrl;
        DOUBLE GainFactor64 = 0.0;

        if (pChan == NULL) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        TotalGainCtrl = (TI01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] +
                                         TI01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]);
                        (void)AmbaWrap_pow(10.0, 0.015 * (DOUBLE)TotalGainCtrl, &GainFactor64);
                        pGainFactor[0] = (FLOAT)GainFactor64;
                    }
                }
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                            TotalGainCtrl = (TI01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] +
                                             TI01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]);
                            (void)AmbaWrap_pow(10.0, 0.015 * (DOUBLE)TotalGainCtrl, &GainFactor64);
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
     *  @RoutineName:: TI01_IMX390_GetCurrentShutterSpeed
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
    static UINT32 TI01_IMX390_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
        UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
        DOUBLE ShutterTimeUnit;
        UINT8 i, k;

        if (pChan == NULL) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        pExposureTime[0] = ((FLOAT)ShutterTimeUnit * (FLOAT)TI01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][0]);
                    }
                }
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                            pExposureTime[k] = ((FLOAT)ShutterTimeUnit * (FLOAT)TI01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][k]);
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
     *  @RoutineName:: TI01_IMX390_ConvertGainFactor
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
    static UINT32 TI01_IMX390_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        UINT32 HdrType = TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
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
                (void)AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
                Desire_dB_SP1H = (FLOAT)(20.0 * LogDesiredFactor);

                DesiredFactor = (DOUBLE)pDesiredFactor->Gain[1];
                (void)AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
                Desire_dB_SP1L = (FLOAT)(20.0 * LogDesiredFactor);

                /************************************** Agc & Dgc **************************************/
                /* Maximum check */
                Desire_dB_SP1H = (Desire_dB_SP1H >= TI01_IMX390_MAX_TOTAL_GAIN) ? TI01_IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1H;
                Desire_dB_SP1L = (Desire_dB_SP1L >= TI01_IMX390_MAX_TOTAL_GAIN) ? TI01_IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1L;

                /* Minimum check */
                Desire_dB_SP1H = (Desire_dB_SP1H <= 0.0f) ? 0.0f : Desire_dB_SP1H;
                Desire_dB_SP1L = (Desire_dB_SP1L <= 0.0f) ? 0.0f : Desire_dB_SP1L;

                /* Maximum ABS between SP1H and SP1L is 30dB */
                if ((Desire_dB_SP1H - Desire_dB_SP1L) >= TI01_IMX390_MAX_AGAIN) {
                    Desire_dB_SP1L = Desire_dB_SP1H - TI01_IMX390_MAX_AGAIN;
                }
                if ((Desire_dB_SP1H - Desire_dB_SP1L) <= -TI01_IMX390_MAX_AGAIN) {
                    Desire_dB_SP1L = Desire_dB_SP1H + TI01_IMX390_MAX_AGAIN;
                }

                /* Calculate Agc/Dgc for SP1H/SP1L */
                if ((Desire_dB_SP1H <= TI01_IMX390_MAX_AGAIN) && (Desire_dB_SP1L <= TI01_IMX390_MAX_AGAIN)) {
                    Agc_dB_SP1H = Desire_dB_SP1H;
                    Agc_dB_SP1L = Desire_dB_SP1L;
                    Dgc_dB = 0.0f;
                } else if ((Desire_dB_SP1H >= TI01_IMX390_MAX_AGAIN) && (Desire_dB_SP1L >= TI01_IMX390_MAX_AGAIN)) {
                    if (Desire_dB_SP1H > Desire_dB_SP1L) {
                        Agc_dB_SP1H = TI01_IMX390_MAX_AGAIN;
                        Agc_dB_SP1L = TI01_IMX390_MAX_AGAIN - (Desire_dB_SP1H - Desire_dB_SP1L);
                        Dgc_dB = Desire_dB_SP1H - TI01_IMX390_MAX_AGAIN;
                    } else {
                        Agc_dB_SP1H = TI01_IMX390_MAX_AGAIN - (Desire_dB_SP1L - Desire_dB_SP1H);
                        Agc_dB_SP1L = TI01_IMX390_MAX_AGAIN;
                        Dgc_dB = Desire_dB_SP1L - TI01_IMX390_MAX_AGAIN;
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

                (void) AmbaWrap_floor((DOUBLE)(Agc_dB_SP1H / 0.3), &GainInDb);
                pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
                (void) AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.3), &GainInDb);
                pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
                (void)AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)pGainCtrl->AnalogGain[0] + (DOUBLE)pGainCtrl->DigitalGain[0]), &ActualFactor64);
                pActualFactor->Gain[0] = (FLOAT) ActualFactor64;

                (void) AmbaWrap_floor((DOUBLE)(Agc_dB_SP1L / 0.3), &GainInDb);
                pGainCtrl->AnalogGain[1] = (UINT32)GainInDb;
                (void) AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.3), &GainInDb);
                pGainCtrl->DigitalGain[1] = (UINT32)GainInDb;
                (void)AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)pGainCtrl->AnalogGain[1] + (DOUBLE)pGainCtrl->DigitalGain[1]), &ActualFactor64);
                pActualFactor->Gain[1] = (FLOAT) ActualFactor64;

                pGainCtrl->AnalogGain[2] = pGainCtrl->AnalogGain[1];
                pGainCtrl->DigitalGain[2] = pGainCtrl->DigitalGain[1];
                pActualFactor->Gain[2] = pActualFactor->Gain[1];
            } else {
                DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
                (void)AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
                Desire_dB_SP1 = (FLOAT)(20.0 * LogDesiredFactor);

                /* Maximum check */
                Desire_dB_SP1 = (Desire_dB_SP1 >= TI01_IMX390_MAX_TOTAL_GAIN) ? TI01_IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1;

                /* Minimum check */
                Desire_dB_SP1 = (Desire_dB_SP1 <= 0.0f) ? 0.0f : Desire_dB_SP1;

                /* Calculate Agc/Dgc for SP1 */
                if (Desire_dB_SP1 <= TI01_IMX390_MAX_AGAIN) {
                    Agc_dB_SP1 = Desire_dB_SP1;
                    Dgc_dB = 0.0f;
                } else {
                    Agc_dB_SP1 = TI01_IMX390_MAX_AGAIN;
                    Dgc_dB = Desire_dB_SP1 - TI01_IMX390_MAX_AGAIN;
                }

                (void) AmbaWrap_floor((DOUBLE)(Agc_dB_SP1 / 0.3), &GainInDb);
                pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
                (void) AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.3), &GainInDb);
                pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
                (void)AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)pGainCtrl->AnalogGain[0] + (DOUBLE)pGainCtrl->DigitalGain[0]), &ActualFactor64);
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

            (void) AmbaWrap_floor((DOUBLE)(WbGain.R * 256.0), &WbGainInDb);
            pGainCtrl->WbGain[0].R = (UINT32)WbGainInDb;
            (void) AmbaWrap_floor((DOUBLE)(WbGain.Gr * 256.0), &WbGainInDb);
            pGainCtrl->WbGain[0].Gr = (UINT32)WbGainInDb;
            (void) AmbaWrap_floor((DOUBLE)(WbGain.Gb * 256.0), &WbGainInDb);
            pGainCtrl->WbGain[0].Gb = (UINT32)WbGainInDb;
            (void) AmbaWrap_floor((DOUBLE)(WbGain.B * 256.0), &WbGainInDb);
            pGainCtrl->WbGain[0].B = (UINT32)WbGainInDb;

            pActualFactor->WbGain[0].R  = (FLOAT)pGainCtrl->WbGain[0].R  / 256.0f;
            pActualFactor->WbGain[0].Gr = (FLOAT)pGainCtrl->WbGain[0].Gr / 256.0f;
            pActualFactor->WbGain[0].Gb = (FLOAT)pGainCtrl->WbGain[0].Gb / 256.0f;
            pActualFactor->WbGain[0].B  = (FLOAT)pGainCtrl->WbGain[0].B  / 256.0f;

            if (HdrType != AMBA_SENSOR_HDR_NONE) {
                (void) AmbaWrap_memcpy(&pGainCtrl->WbGain[1], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));
                (void) AmbaWrap_memcpy(&pGainCtrl->WbGain[2], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));

                (void) AmbaWrap_memcpy(&pActualFactor->WbGain[1], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
                (void) AmbaWrap_memcpy(&pActualFactor->WbGain[2], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
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
    static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl) {
        UINT32 MaxExposureline, MinExposureline;
        UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
        UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
        UINT32 ShutterCtrl;
        DOUBLE ShutterTimeUnit, ShutterCtrlInDb;

        ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

        (void)AmbaWrap_floor(((DOUBLE)ExposureTime / ShutterTimeUnit), &ShutterCtrlInDb);
        ShutterCtrl = (UINT32)ShutterCtrlInDb;

        /* VMAX is 20 bits */
        if (ShutterCtrl > ((0xfffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame)) {
            ShutterCtrl = ((0xfffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame);
        }

#ifdef TI01_IMX390_IN_SLAVE_MODE
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
     *  @RoutineName:: TI01_IMX390_ConvertShutterSpeed
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
    static UINT32 TI01_IMX390_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
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
     *  @RoutineName:: TI01_IMX390_SetAnalogGainCtrl
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
    static UINT32 TI01_IMX390_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        UINT32 i, k;

        if (pChan == NULL) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                RetVal = TI01_IMX390_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

                /* Update current analog gain control */
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        TI01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = pAnalogGainCtrl[0];
                    }
                }
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                RetVal = TI01_IMX390_SetHdrAnalogGainReg(pChan, pAnalogGainCtrl);

                /* Update current analog gain control */
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                            TI01_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] = pAnalogGainCtrl[k];
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
     *  @RoutineName:: TI01_IMX390_SetDigitalGainCtrl
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
    static UINT32 TI01_IMX390_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        UINT32 i, k;

        if (pChan == NULL) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                RetVal = TI01_IMX390_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

                /* Update current digital gain control */
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        TI01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]= pDigitalGainCtrl[0];
                    }
                }
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                RetVal = TI01_IMX390_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

                /* Update current digital gain control */
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                            TI01_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]= pDigitalGainCtrl[k];
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
     *  @RoutineName:: TI01_IMX390_SetWbGainCtrl
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
    static UINT32 TI01_IMX390_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        UINT32 i, k;

        if (pChan == NULL) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                RetVal = TI01_IMX390_SetWbGainReg(pChan, pWbGainCtrl);

                /* Update current wb gain control */
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        TI01_IMX390Ctrl[pChan->VinID].CurrentWbCtrl[i][0] = pWbGainCtrl[0];
                    }
                }
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                RetVal = TI01_IMX390_SetWbGainReg(pChan, pWbGainCtrl);

                /* Update current wb gain control */
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                            TI01_IMX390Ctrl[pChan->VinID].CurrentWbCtrl[i][k] = pWbGainCtrl[k];
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
     *  @RoutineName:: TI01_IMX390_SetShutterCtrl
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
    static UINT32 TI01_IMX390_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
        UINT32 i, k;

        if ((pChan == NULL) || (pShutterCtrl == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                RetVal = TI01_IMX390_SetShutterReg(pChan, pShutterCtrl[0]);

                /* Update current shutter control */
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        TI01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][0] = pShutterCtrl[0];
                    }
                }
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                RetVal = TI01_IMX390_SetHdrShutterReg(pChan, pShutterCtrl);

                /* Update current shutter control */
                for (i = 0U; i < TI01_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                            TI01_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][k] = pShutterCtrl[k];
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
     *  @RoutineName:: TI01_IMX390_SetSlowShrCtrl
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
    static UINT32 TI01_IMX390_SetSlowShrCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl) {
        UINT32 RetVal = SENSOR_ERR_NONE;

        if (pChan == NULL) {
            RetVal = SENSOR_ERR_ARG;
        } else if (TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API;
        } else {
            RetVal = TI01_IMX390_SetSlowShutterReg(SlowShutterCtrl);
        }

        return RetVal;
    }

#ifdef TI01_IMX390_IN_SLAVE_MODE
    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_ConfigMasterSync
     *
     *  @Description:: Configure VIN master sync generation
     *
     *  @Input      ::
     *      ModeID:    Sensor mode
     *
     *  @Output     :: none
     *
     *  @Return     :: none
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID) {
        static UINT32 TI01_IMX390MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
            [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
            [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
            [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_MSYNC_CHANNEL_PIP2,
        };

        static AMBA_VIN_MASTER_SYNC_CONFIG_s TI01_IMX390MasterSyncConfig = {
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
            .HSyncDelayCycles = 1U,
            .VSyncDelayCycles = 0,
            .ToggleHsyncInVblank = 1U
        };

        UINT32 RetVal;
        DOUBLE PeriodInDb;
        AMBA_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncCfg = &TI01_IMX390MasterSyncConfig;
        const TI01_IMX390_MODE_INFO_s *pModeInfo = &TI01_IMX390ModeInfoList[ModeID];

        (void)AmbaWrap_floor(((((DOUBLE)pModeInfo->FrameRate.NumUnitsInTick * ((DOUBLE)TI01_IMX390SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameRate.TimeScale))) + 0.5), &PeriodInDb);
        pMasterSyncCfg->RefClk = TI01_IMX390SensorInfo[ModeID].InputClk;
        pMasterSyncCfg->HSync.Period = (UINT32)PeriodInDb;
        pMasterSyncCfg->HSync.PulseWidth  = 8U;
        pMasterSyncCfg->HSync.Polarity    = 0U;
        pMasterSyncCfg->VSync.Period      = 1U;
        pMasterSyncCfg->VSync.PulseWidth  = 1000U;
        pMasterSyncCfg->VSync.Polarity    = 0U;

        RetVal = AmbaVIN_MasterSyncEnable(TI01_IMX390MasterSyncChannel[pChan->VinID], pMasterSyncCfg);

        return RetVal;
    }
#endif

    /*-----------------------------------------------------------------------------------------------*\
     *  @RoutineName:: TI01_IMX390_Config
     *
     *  @Description:: Set sensor to indicated mode
     *
     *  @Input      ::
     *      Chan:   Vin ID and sensor ID
     *      Mode:   Sensor Readout Mode Number
     *
     *  @Output     :: none
     *
     *  @Return     ::
     *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
    \*-----------------------------------------------------------------------------------------------*/
    static UINT32 TI01_IMX390_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode) {
        UINT32 RetVal = SENSOR_ERR_NONE;
        UINT32 SensorMode = pMode->ModeID;
        AMBA_SENSOR_MODE_INFO_s *pModeInfo = &TI01_IMX390Ctrl[pChan->VinID].Status.ModeInfo;

#if 0
        UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
            [AMBA_VIN_CHANNEL0] = 0x00000fU,  /* SD_LVDS_0~3 */
            [AMBA_VIN_CHANNEL1] = 0x00f000U,  /* SD_LVDS_12~15 */
            [AMBA_VIN_CHANNEL2] = 0x0000f0U,  /* SD_LVDS_4~7 */
        };
#else
        UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
            [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
            [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
        };
#endif

        AMBA_VIN_MIPI_PAD_CONFIG_s TI01_IMX390PadConfig = {
            .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
            .DateRate   = 0U,
            .EnabledPin = EnabledPin[pChan->VinID]
        };

        static TI953_954_SERDES_OUTPUT_CONFIG_s  TI01_IMX390SerdesOutputConfig[AMBA_NUM_VIN_CHANNEL] = {
            [AMBA_VIN_CHANNEL0] = {
                .CSILaneNum         = TI953_954_CSI_LANE_NUM_4,
                .CSITxSpeed         = TI953_954_CSI_TX_SPEED_800M,
                .ForwardingMode     = TI953_954_LINE_INTERLEAVE_FWD,
            },
            [AMBA_VIN_CHANNEL1] = {
                .CSILaneNum         = TI953_954_CSI_LANE_NUM_4,
                .CSITxSpeed         = TI953_954_CSI_TX_SPEED_800M,
                .ForwardingMode     = TI953_954_LINE_INTERLEAVE_FWD,
            },
            [AMBA_VIN_CHANNEL2] = {
                .CSILaneNum         = TI953_954_CSI_LANE_NUM_4,
                .CSITxSpeed         = TI953_954_CSI_TX_SPEED_800M,
                .ForwardingMode     = TI953_954_LINE_INTERLEAVE_FWD,
            },
        };

        if (SensorMode >= TI01_IMX390_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "============  [ TI01_IMX390 ]  ============", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d, VinID:%d ============", SensorMode, pChan->VinID, 0U, 0U, 0U);

            TI01_IMX390Ctrl[pChan->VinID].EnabledSensorID = TI01_IMX390_GetEnabledLinkID(pChan->SensorID);
            TI01_IMX390_PrepareModeInfo(pChan, pMode, pModeInfo);

            /* POC power reset */
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "POC power reset: Power off", 0U, 0U, 0U, 0U, 0U);
            RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaKAL_TaskSleep(1);
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "POC power reset: Power on", 0U, 0U, 0U, 0U, 0U);
            RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
            (void)AmbaKAL_TaskSleep(3);

            /* Adjust mipi-phy parameters */
            TI01_IMX390PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
            RetVal |= AmbaVIN_MipiReset(pChan->VinID, &TI01_IMX390PadConfig);
            (void) AmbaKAL_TaskSleep(50);

            /* Set TI954 PDB high */
            RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
            (void) AmbaKAL_TaskSleep(1);
            RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
            (void) AmbaKAL_TaskSleep(3);

            /* Config TI serdes */
            TI01_IMX390SerdesConfig[pChan->VinID].EnabledRxPortID = (UINT16)TI01_IMX390Ctrl[pChan->VinID].EnabledSensorID;
            RetVal |= TI953_954_Init(pChan->VinID, TI01_IMX390SerdesConfig[pChan->VinID].EnabledRxPortID);
            (void) AmbaKAL_TaskSleep(10);
            RetVal |= TI953_954_Config(pChan->VinID, &TI01_IMX390SerdesConfig[pChan->VinID]);
            (void) AmbaKAL_TaskSleep(10);
            RetVal |= TI953_954_SetSensorClk(pChan->VinID, TI01_IMX390SerdesConfig[pChan->VinID].EnabledRxPortID, TI953_954_RCLK_OUT_FREQ_27M);
            (void) AmbaKAL_TaskSleep(650);

            /* program sensor */
            RetVal |= TI01_IMX390_ChangeReadoutMode(pChan, SensorMode);

#ifdef TI01_IMX390_IN_SLAVE_MODE
            {
                UINT8 Data;
                Data = 0x0aU;
                RetVal |= TI01_IMX390_RegWrite(pChan, 0x365cU, &Data, 1U);
                Data = 0x0aU;
                RetVal |= TI01_IMX390_RegWrite(pChan, 0x365eU, &Data, 1U);
                Data = 0x06U;
                RetVal |= TI01_IMX390_RegWrite(pChan, 0x23c2U, &Data, 1U);
                Data = 0x01U;
                RetVal |= TI01_IMX390_RegWrite(pChan, 0x3650U, &Data, 1U);
            }
#endif

            RetVal |= TI01_IMX390_SetStandbyOff(pChan);

#ifdef TI01_IMX390_IN_SLAVE_MODE
            RetVal |= TI01_IMX390_ConfigMasterSync(pChan, SensorMode);
#endif

            TI01_IMX390SerdesOutputConfig[pChan->VinID].CSILaneNum = (UINT32)pModeInfo->OutputInfo.NumDataLanes;
            RetVal |= TI953_954_EnableCSIOutput(pChan->VinID, &TI01_IMX390SerdesOutputConfig[pChan->VinID]);

            /* Config VIN */
            RetVal |= TI01_IMX390_ConfigVin(pChan, pModeInfo);
        }

        return RetVal;
    }

    /*-----------------------------------------------------------------------------------------------*/
    AMBA_SENSOR_OBJ_s AmbaSensor_TI01_IMX390Obj = {
        .SensorName             = "IMX390",
        .SerdesName             = "TI953_954",
        .Init                   = TI01_IMX390_Init,
        .Enable                 = TI01_IMX390_Enable,
        .Disable                = TI01_IMX390_Disable,
        .Config                 = TI01_IMX390_Config,
        .GetStatus              = TI01_IMX390_GetStatus,
        .GetModeInfo            = TI01_IMX390_GetModeInfo,
        .GetDeviceInfo          = TI01_IMX390_GetDeviceInfo,
        .GetHdrInfo             = TI01_IMX390_GetHdrInfo,
        .GetCurrentGainFactor   = TI01_IMX390_GetCurrentGainFactor,
        .GetCurrentShutterSpeed = TI01_IMX390_GetCurrentShutterSpeed,

        .ConvertGainFactor      = TI01_IMX390_ConvertGainFactor,
        .ConvertShutterSpeed    = TI01_IMX390_ConvertShutterSpeed,

        .SetAnalogGainCtrl      = TI01_IMX390_SetAnalogGainCtrl,
        .SetDigitalGainCtrl     = TI01_IMX390_SetDigitalGainCtrl,
        .SetWbGainCtrl          = TI01_IMX390_SetWbGainCtrl,
        .SetShutterCtrl         = TI01_IMX390_SetShutterCtrl,
        .SetSlowShutterCtrl     = TI01_IMX390_SetSlowShrCtrl,

        .RegisterRead           = TI01_IMX390_RegisterRead,
        .RegisterWrite          = TI01_IMX390_RegisterWrite,

        .ConfigPost             = NULL,
        .GetSerdesStatus    = NULL,

        .ConvertStrobeWidth     = NULL,
        .SetStrobeCtrl          = NULL,
    };
