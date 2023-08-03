/**
 *  @file AmbaSensor_B8N_IMX290.c
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
 *  @details Control APIs of SONY B8N_IMX290 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>

#include "AmbaVIN.h"

#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_B8N_IMX290.h"
#include "AmbaPrint.h"

#include "AmbaB8_I2C.h"
#include "AmbaB8_GPIO.h"
#include "AmbaB8_PLL.h"

#include "bsp.h"

//#define B8N_IMX290_ENABLE_B8_DUAL_VOUT

#if 0
static INT32 B8N_IMX290_ConfigPost(UINT32 *pVinID);
#endif

#define B8N_IMX290_I2C_WR_BUF_SIZE  32U

#define B8N_IMX290_B8N_GRP  0U
#define B8N_IMX290_B8F0_GRP 1U
#define B8N_IMX290_B8F1_GRP 2U
#define B8N_IMX290_NUM_GRP  3U

static UINT8 B8N_IMX290_SensorI2cChan[AMBA_NUM_VIN_CHANNEL][B8N_IMX290_NUM_GRP][B8_NUM_VIN_CHANNEL + 1U] = {
    /* i2c chan for sensors on VIN */
    [AMBA_VIN_CHANNEL0] = {
        [B8N_IMX290_B8N_GRP] = {
            [0] = (UINT8) B8_I2C_CHANNEL0_ALL,  /* broadcast mode */
            [1] = (UINT8) B8_I2C_CHANNEL0,      /* for VIN sensor, SensorId = 0x0001 */
            [2] = B8_I2C_CHANNEL0_2,            /* for PIP sensor, SensorId = 0x0002 */
        },
        [B8N_IMX290_B8F0_GRP] = {
            [0] = (UINT8) B8_I2C_CHANNEL0_ALL,  /* broadcast mode */
            [1] = (UINT8) B8_I2C_CHANNEL0,      /* for VIN sensor, SensorId = 0x0010 */
            [2] = (UINT8) B8_I2C_CHANNEL0_2,    /* for PIP sensor, SensorId = 0x0020 */
        },
        [B8N_IMX290_B8F1_GRP] = {
            [0] = 0xFF,                         /* broadcast mode */
            [1] = (UINT8) B8_I2C_CHANNEL0,      /* for VIN sensor, SensorId = 0x0100 */
            [2] = 0xFF,                         /* for PIP sensor, SensorId = 0x0200 */
        },
    },
    /* i2c chan for sensors on PIP */
    [AMBA_VIN_CHANNEL1] = {
        [B8N_IMX290_B8N_GRP] = {
            [0] = (UINT8) B8_I2C_CHANNEL0_ALL,  /* broadcast mode */
            [1] = (UINT8) B8_I2C_CHANNEL0,      /* for VIN sensor, SensorId = 0x0001 */
            [2] = B8_I2C_CHANNEL0_2,            /* for PIP sensor, SensorId = 0x0002 */
        },
        [B8N_IMX290_B8F0_GRP] = {
            [0] = (UINT8) B8_I2C_CHANNEL0_ALL,  /* broadcast mode */
            [1] = (UINT8) B8_I2C_CHANNEL0,      /* for VIN sensor, SensorId = 0x0010 */
            [2] = (UINT8) B8_I2C_CHANNEL0_2,    /* for PIP sensor, SensorId = 0x0020 */
        },
        [B8N_IMX290_B8F1_GRP] = {
            [0] = 0xFF,                         /* broadcast mode */
            [1] = (UINT8) B8_I2C_CHANNEL0,      /* for VIN sensor, SensorId = 0x0100 */
            [2] = 0xFF,                         /* for PIP sensor, SensorId = 0x0200 */
        },
    },
    [AMBA_VIN_CHANNEL8] = {
        [B8N_IMX290_B8N_GRP] = {
            [0] = (UINT8) B8_I2C_CHANNEL0_ALL,  /* broadcast mode */
            [1] = (UINT8) B8_I2C_CHANNEL0,      /* for VIN sensor, SensorId = 0x0001 */
            [2] = B8_I2C_CHANNEL0_2,            /* for PIP sensor, SensorId = 0x0002 */
        },
        [B8N_IMX290_B8F0_GRP] = {
            [0] = (UINT8) B8_I2C_CHANNEL0_ALL,  /* broadcast mode */
            [1] = (UINT8) B8_I2C_CHANNEL0,      /* for VIN sensor, SensorId = 0x0010 */
            [2] = (UINT8) B8_I2C_CHANNEL0_2,    /* for PIP sensor, SensorId = 0x0020 */
        },
        [B8N_IMX290_B8F1_GRP] = {
            [0] = 0xFF,                         /* broadcast mode */
            [1] = (UINT8) B8_I2C_CHANNEL0,      /* for VIN sensor, SensorId = 0x0100 */
            [2] = 0xFF,                         /* for PIP sensor, SensorId = 0x0200 */
        },
    },
};

/*-----------------------------------------------------------------------------------------------*\
 * B8N_IMX290 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static B8N_IMX290_CTRL_s B8N_IMX290Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};

static UINT32 B8N_IMX290_GetB8ChipID(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 B8ChipId = 0U;
    UINT8 i;

    /* B8N */
    B8ChipId |= (UINT32)1U << (pChan->VinID + 4U);

    /* B8F */
    for (i = 1U; i <= B8_MAX_NUM_B8F_ON_CHAN; i ++) {
        if ((pChan->SensorID & ((UINT32)0xfU << (i * 4U))) != 0U) {
            B8ChipId |= ((UINT32)1U << (i - 1U)) ;
        }
    }

    return B8ChipId;
}

static UINT32 B8N_IMX290_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < 12U; i ++) {
        if ((SensorID & ((UINT32)1U << i)) != 0U) {
            SensorNum ++;
        }
    }

    return SensorNum;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_PrepareModeInfo
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
static void B8N_IMX290_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &B8N_IMX290InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &B8N_IMX290OutputInfo[ModeID];
    const B8N_IMX290_SENSOR_INFO_s      *pSensorInfo    = &B8N_IMX290SensorInfo[ModeID];
    UINT32 SensorNum = B8N_IMX290_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;

    //AmbaPrint("Sensor Mode:%d, Input Mode:%d, Output Mode:%d",SensorMode,InputMode,OutputMode);
    if (SensorNum != 0U) {
        pModeInfo->FrameLengthLines         = pSensorInfo->FrameLengthLines * SensorNum;
        pModeInfo->NumExposureStepPerFrame  = pSensorInfo->FrameLengthLines;
        pModeInfo->RowTime                  = B8N_IMX290ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
        pModeInfo->InputClk                 = pSensorInfo->InputClk;
        (void) AmbaWrap_floor((((DOUBLE)B8N_IMX290ModeInfoList[ModeID].PixelRate * (DOUBLE)SensorNum) * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal);
        pModeInfo->LineLengthPck            = (UINT32) FloorVal;
        (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
        (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &B8N_IMX290ModeInfoList[ModeID].FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
        (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
        (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &B8N_IMX290ModeInfoList[ModeID].FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        (void)AmbaWrap_memcpy(&pModeInfo->HdrInfo, &B8N_IMX290HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

        pModeInfo->OutputInfo.DataRate *= SensorNum;
        pModeInfo->OutputInfo.OutputHeight *= (UINT16)SensorNum;
        pModeInfo->OutputInfo.RecordingPixels.Height *= (UINT16)SensorNum;

        /* updated minimum frame rate limitation */
        if (pModeInfo->HdrInfo.HdrType == 0U) {
            pModeInfo->MinFrameRate.TimeScale /= 8U;
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_ConfigVin
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
static UINT32 B8N_IMX290_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID = pModeInfo->Config.ModeID;
    const AMBA_SENSOR_OUTPUT_INFO_s  *pOutputInfo = &B8N_IMX290OutputInfo[ModeID];
    AMBA_VIN_MIPI_CONFIG_s VinCfg;
    UINT32 SensorNum = B8N_IMX290_GetNumActiveSensor(pChan->SensorID);
    UINT32 HdrExpNum = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels;
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    AMBA_VIN_MIPI_CONFIG_s B8N_IMX290VinConfig = {
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

    (void)AmbaWrap_memcpy(&VinCfg, &B8N_IMX290VinConfig, sizeof(AMBA_VIN_MIPI_CONFIG_s));

    VinCfg.NumActiveLanes                 = pOutputInfo->NumDataLanes;
    VinCfg.Config.NumDataBits               = pOutputInfo->NumDataBits;
    VinCfg.Config.BayerPattern              = pOutputInfo->BayerPattern;

    if ((B8N_IMX290HdrInfo[ModeID].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) &&
        (B8N_IMX290ModeInfoList[ModeID].B8Ctrl.CompressionRatio == B8_USE_CFA_CODEC)) {
        VinCfg.Config.RxHvSyncCtrl.NumActivePixels   = pOutputInfo->RecordingPixels.Width * SensorNum * HdrExpNum;
        VinCfg.Config.RxHvSyncCtrl.NumActiveLines    = pOutputInfo->RecordingPixels.Height / HdrExpNum;
        VinCfg.Config.RxHvSyncCtrl.NumTotalPixels    = pOutputInfo->OutputWidth * SensorNum * HdrExpNum;
        VinCfg.Config.RxHvSyncCtrl.NumTotalLines     = pOutputInfo->OutputHeight / HdrExpNum;

        VinCfg.Config.SplitCtrl.NumSplits = (UINT8)(SensorNum * HdrExpNum);     /* note: should be <= 4 */
        VinCfg.Config.SplitCtrl.SplitWidth = pOutputInfo->OutputWidth;
    } else {
        VinCfg.Config.RxHvSyncCtrl.NumActivePixels   = pOutputInfo->RecordingPixels.Width * SensorNum;
        VinCfg.Config.RxHvSyncCtrl.NumActiveLines    = pOutputInfo->RecordingPixels.Height;
        VinCfg.Config.RxHvSyncCtrl.NumTotalPixels    = pOutputInfo->OutputWidth * SensorNum;
        VinCfg.Config.RxHvSyncCtrl.NumTotalLines     = pOutputInfo->OutputHeight;

        if (SensorNum > 1U) {
            VinCfg.Config.SplitCtrl.NumSplits = 0U;// mark it off since we use side by side mode temporarily. SensorNum;
            VinCfg.Config.SplitCtrl.SplitWidth = pOutputInfo->OutputWidth;
        }
    }

    (void)AmbaWrap_memcpy(&(VinCfg.Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    return AmbaVIN_MipiConfig(pChan->VinID, &VinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_HardwareReset
 *
 *  @Description:: Reset B8N_IMX290 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 B8N_IMX290_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_SerdesResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);

    (void)AmbaKAL_TaskSleep(100);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_RegRW
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
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
    UINT8  TxDataBuf[B8N_IMX290_I2C_WR_BUF_SIZE];
    UINT16 TxDataSize = (UINT16)Size + 2U;
    UINT32 ChipID = B8N_IMX290_GetB8ChipID(pChan);
    UINT32 SubChipID = 0U;
    /* sensor id for B8N / B8F0 /B8F1 */
    UINT32 SubSensorId[3];

    if ((TxDataSize > B8N_IMX290_I2C_WR_BUF_SIZE) || (pChan == NULL)) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "B8_i2c error: write size (%u) > buffer size (%u)", TxDataSize, B8N_IMX290_I2C_WR_BUF_SIZE, 0U, 0U, 0U);
        RetVal = SENSOR_ERR_ARG;
    } else {
        SubSensorId[B8N_IMX290_B8N_GRP] = (pChan->SensorID & 0x3U);
        SubSensorId[B8N_IMX290_B8F0_GRP] = ((pChan->SensorID >> 4U) & 0x3U);
        SubSensorId[B8N_IMX290_B8F1_GRP] = ((pChan->SensorID >> 8U) & 0x3U);

        TxDataBuf[0] = (UINT8) (Addr >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0xffU);

        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }
        /* Sensor on B8N and B8F */
        if ((SubSensorId[B8N_IMX290_B8N_GRP] != 0U) && ((SubSensorId[B8N_IMX290_B8F0_GRP] != 0U) || (SubSensorId[B8N_IMX290_B8F1_GRP] != 0U))) {

            /* TBD: B8NF_I2cWrite */

            /* Sensor on B8F0 and B8F1 and symmetry (I2C channel should be the same) */
        } else if ((SubSensorId[B8N_IMX290_B8F0_GRP] != 0U) && (SubSensorId[B8N_IMX290_B8F1_GRP] != 0U) &&
                   (SubSensorId[B8N_IMX290_B8F0_GRP] == SubSensorId[B8N_IMX290_B8F1_GRP])) {

            /* B8F boardcast channel */
            if ((SubSensorId[B8N_IMX290_B8F0_GRP] == 0x3U) && (B8N_IMX290_SensorI2cChan[pChan->VinID][B8N_IMX290_B8F0_GRP][0] != 0xFFU)) {
                RetVal |= AmbaB8_I2cWrite(ChipID,
                                          B8N_IMX290_SensorI2cChan[pChan->VinID][B8N_IMX290_B8F0_GRP][0], B8_I2C_SPEED_FAST,
                                          B8N_IMX290_SENSOR_I2C_SLAVE_ADDR, TxDataSize, TxDataBuf, 1);
#if 0
                for (k = 0; k < Size; k++) {
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290][IO][Write] B8ChipID: 0x%x, I2cChan: %d, Addr:0x%04x, Data:0x%02X",
                                               ChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][B8N_IMX290_B8F0_GRP][0], Addr + k, pTxData[k], 0U);
                }
#endif

                /* B8F VIN sensor or PIP sensor */
            } else {
                if ((SubSensorId[B8N_IMX290_B8F0_GRP] & 0x1U) != 0U) {
                    RetVal |= AmbaB8_I2cWrite(ChipID,
                                              B8N_IMX290_SensorI2cChan[pChan->VinID][B8N_IMX290_B8F0_GRP][1], B8_I2C_SPEED_FAST,
                                              B8N_IMX290_SENSOR_I2C_SLAVE_ADDR, TxDataSize, TxDataBuf, 1);
#if 0
                    for (k = 0; k < Size; k++) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290][IO][Write] B8ChipID: 0x%x, I2cChan: %d, Addr:0x%04x, Data:0x%02X",
                                                   ChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][B8N_IMX290_B8F0_GRP][1], Addr + k, pTxData[k], 0U);
                    }
#endif

                }
                if ((SubSensorId[B8N_IMX290_B8F0_GRP] & 0x2U) != 0U) {
                    RetVal |= AmbaB8_I2cWrite(ChipID,
                                              B8N_IMX290_SensorI2cChan[pChan->VinID][B8N_IMX290_B8F0_GRP][2], B8_I2C_SPEED_FAST,
                                              B8N_IMX290_SENSOR_I2C_SLAVE_ADDR, TxDataSize, TxDataBuf, 1);
#if 0
                    for (k = 0; k < Size; k++) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290][IO][Write] B8ChipID: 0x%x, I2cChan: %d, Addr:0x%04x, Data:0x%02X",
                                                   ChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][B8N_IMX290_B8F0_GRP][2], Addr + k, pTxData[k], 0U);
                    }
#endif

                }
            }

            /* Others */
        } else {
            /* send i2c cmd for sensors on B8N/B8F0/B8F1 */
            for (i = 0; i <= B8_MAX_NUM_B8F_ON_CHAN; i++) {
                if (SubSensorId[i] != 0U) {
                    /* get chip id */
                    if (i == 0U) {
                        SubChipID = (ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
                    } else {
                        SubChipID = ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) | ((UINT32)1U << (i - 1U)));
                    }

                    /* send i2c cmd */
                    if ((SubSensorId[i] == 0x3U) && (B8N_IMX290_SensorI2cChan[pChan->VinID][i][0] != 0xFFU)) { /* boardcast */
                        RetVal |= AmbaB8_I2cWrite(SubChipID,
                                                  B8N_IMX290_SensorI2cChan[pChan->VinID][i][0], B8_I2C_SPEED_FAST,
                                                  B8N_IMX290_SENSOR_I2C_SLAVE_ADDR, TxDataSize, TxDataBuf, 1);
#if 0
                        for (k = 0; k < Size; k++) {
                            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290][IO][Write] B8ChipID: 0x%x, I2cChan: %d, Addr:0x%04x, Data:0x%02X",
                                                       SubChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][i][0], Addr + k, pTxData[k], 0U);
                        }
#endif
                    } else {
                        if ((SubSensorId[i] & 0x1U) != 0U) { /* for B8F VIN sensor */
                            RetVal |= AmbaB8_I2cWrite(SubChipID,
                                                      B8N_IMX290_SensorI2cChan[pChan->VinID][i][1], B8_I2C_SPEED_FAST,
                                                      B8N_IMX290_SENSOR_I2C_SLAVE_ADDR, TxDataSize, TxDataBuf, 1);
#if 0
                            for (k = 0; k < Size; k++) {
                                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290][IO][Write] B8ChipID: 0x%x, I2cChan: %d, Addr:0x%04x, Data:0x%02X",
                                                           SubChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][i][1], Addr + k, pTxData[k], 0U);
                            }
#endif
                        }
                        if ((SubSensorId[i] & 0x2U) != 0U) { /* for B8F PIP sensor */
                            RetVal |= AmbaB8_I2cWrite(SubChipID,
                                                      B8N_IMX290_SensorI2cChan[pChan->VinID][i][2], B8_I2C_SPEED_FAST,
                                                      B8N_IMX290_SENSOR_I2C_SLAVE_ADDR, TxDataSize, TxDataBuf, 1);
#if 0
                            for (k = 0; k < Size; k++) {
                                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290][IO][Write] B8ChipID: 0x%x, I2cChan: %d, Addr:0x%04x, Data:0x%02X",
                                                           SubChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][i][2], Addr + k, pTxData[k], 0U);
                            }
#endif
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_RegisterWrite
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
static UINT32 B8N_IMX290_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT16 Addr, const UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)Data;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegWrite(pChan, Addr, &WData, 1U); //B8N_IMX290_RegRW(Addr, WData);
    }

    return RetVal;
}

static UINT32 RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ChipID = B8N_IMX290_GetB8ChipID(pChan);
    UINT8 TxDataBuf[3];
    UINT32 SubChipID = 0;
    UINT32 i;
    /* sensor id for B8N / B8F0 /B8F1 */
    UINT32 SubSensorId[3];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        SubSensorId[B8N_IMX290_B8N_GRP] = pChan->SensorID & 0x3U;
        SubSensorId[B8N_IMX290_B8F0_GRP] = (pChan->SensorID >> 4U) & 0x3U;
        SubSensorId[B8N_IMX290_B8F1_GRP] = (pChan->SensorID >> 8U) & 0x3U;

        TxDataBuf[0] = B8N_IMX290_SENSOR_I2C_SLAVE_ADDR;
        TxDataBuf[1] = (UINT8)(Addr >> 8U);
        TxDataBuf[2] = (UINT8)(Addr & 0xffU);

        for (i = 0U; i <= B8_MAX_NUM_B8F_ON_CHAN; i ++) {
            if (SubSensorId[i] != 0U) {
                /* get chip id */
                if (i == 0U) {
                    SubChipID = (ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
                } else {
                    SubChipID = ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) | ((UINT32)1U << (i - 1U)));
                }

                if ((SubSensorId[i] & 0x1U) != 0U) {
                    RetVal = AmbaB8_I2cReadAfterWrite(SubChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][i][1],
                                                      B8_I2C_SPEED_FAST, 3, TxDataBuf, 1, pRxData, 0);
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290][IO][Read] ChipID:0x%04x, i2cChan: %d, Addr:0x%04x, Data:0x%02X",
                                               SubChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][i][1], Addr, pRxData[0], 0U);
                }
                if ((SubSensorId[i] & 0x2U) != 0U) {
                    RetVal = AmbaB8_I2cReadAfterWrite(SubChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][i][2],
                                                      B8_I2C_SPEED_FAST, 3, TxDataBuf, 1, pRxData, 1);
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290][IO][Read] ChipID:0x%04x, i2cChan: %d, Addr:0x%04x, Data:0x%02X",
                                               SubChipID, B8N_IMX290_SensorI2cChan[pChan->VinID][i][2], Addr, pRxData[0], 0U);
                }
            }

        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_RegisterRead
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
static UINT32 B8N_IMX290_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_SetShutterReg
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
static UINT32 B8N_IMX290_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 NumXhsEshrSpeed)
{
    UINT32 SHS1;
    UINT32 RetVal;
    UINT8 TxData[3];

    /* Actual exp line = VMAX - (SHS1 + 1) */
    SHS1 = (B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - NumXhsEshrSpeed) - 1U;

    TxData[0] = (UINT8)(SHS1 & 0xffU);
    TxData[1] = (UINT8)((SHS1 >> 8U) & 0xffU);
    TxData[2] = (UINT8)((SHS1 >> 16U) & 0x03U);
    RetVal = RegWrite(pChan, B8N_IMX290_SHS1_LSB, TxData, 3U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_SetHdrShutterReg
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
static UINT32 B8N_IMX290_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pNumXhsEshrSpeed)
{
    UINT32 FSC, SHSX, RHS1, RHS2;
    UINT8 TxData[3];
    UINT32 RetVal = SENSOR_ERR_NONE;

    FSC = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame;
    RHS1 = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[1].MaxExposureLine + B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels + 1U;
    RHS2 = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ChannelInfo[2].MaxExposureLine + RHS1 + 4U;

    if (B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels == 2U) {
        /* LEF */
        {   /* Actual exp line = FSC - (SHS2 + 1) */
            SHSX = (FSC - pNumXhsEshrSpeed[0U]) - 1U;

            TxData[0] = (UINT8)(SHSX & 0xffU);
            TxData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
            TxData[2] = (UINT8)((SHSX >> 16U) & 0x03U);

            RetVal |= RegWrite(pChan, B8N_IMX290_SHS2_LSB, TxData, 3U);
        }
        /* SEF1 */
        {
            /* Actual exp line = RHS1 - (SHS1 + 1) */
            SHSX = (RHS1 - pNumXhsEshrSpeed[1U]) - 1U;

            TxData[0] = (UINT8)(SHSX & 0xffU);
            TxData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
            TxData[2] = (UINT8)((SHSX >> 16U) & 0x03U);

            RetVal |= RegWrite(pChan, B8N_IMX290_SHS1_LSB, TxData, 3U);
        }
    } else if (B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels == 3U) {
        /* LEF */
        {
            /* Actual exp line = FSC - (SHS3 + 1) */
            SHSX = (FSC - pNumXhsEshrSpeed[0U]) - 1U;

            TxData[0] = (UINT8)(SHSX & 0xffU);
            TxData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
            TxData[2] = (UINT8)((SHSX >> 16U) & 0x03U);

            RetVal |= RegWrite(pChan, B8N_IMX290_SHS3_LSB, TxData, 3U);
        }
        /* SEF1 */
        {
            /* Actual exp line = RHS1 - (SHS1 + 1) */
            SHSX = (RHS1 - pNumXhsEshrSpeed[1U]) - 1U;

            TxData[0] = (UINT8)(SHSX & 0xffU);
            TxData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
            TxData[2] = (UINT8)((SHSX >> 16U) & 0x03U);

            RetVal |= RegWrite(pChan, B8N_IMX290_SHS1_LSB, TxData, 3U);
        }
        /* SEF2 */
        {
            /* Actual exp line = RHS2 - (SHS2 + 1) */
            SHSX = (RHS2 - pNumXhsEshrSpeed[2U]) - 1U;

            TxData[0] = (UINT8)(SHSX & 0xffU);
            TxData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
            TxData[2] = (UINT8)((SHSX >> 16U) & 0x03U);

            RetVal |= RegWrite(pChan, B8N_IMX290_SHS2_LSB, TxData, 3U);
        }
    } else {
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_SetSlowShutterReg
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
static UINT32 B8N_IMX290_SetSlowShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 IntegrationPeriodInFrame)
{
    UINT32 TargetFrameLengthLines;
    UINT32 ModeID = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    UINT32 NumExposureStepPerFrame = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame;
    UINT32 FrameLengthLines = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData[3];

    if (IntegrationPeriodInFrame < 1U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TargetFrameLengthLines = B8N_IMX290SensorInfo[ModeID].FrameLengthLines * IntegrationPeriodInFrame;

        if (TargetFrameLengthLines > 0x3ffffU) {
            TargetFrameLengthLines = 0x3ffffU;
        }

        TxData[0] = (UINT8)(TargetFrameLengthLines & 0xffU);
        TxData[1] = (UINT8)((TargetFrameLengthLines >> 8U) & 0xffU);
        TxData[2] = (UINT8)((TargetFrameLengthLines >> 16U) & 0x03U);

        RetVal = RegWrite(pChan, B8N_IMX290_VMAX_LSB, TxData, 3U);

        /* Update frame rate information */
        B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines * (FrameLengthLines / NumExposureStepPerFrame);
        B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale = B8N_IMX290ModeInfoList[ModeID].FrameRate.TimeScale / IntegrationPeriodInFrame;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8N_IMX290_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData = 0x1U;

    RetVal |= RegWrite(pChan, B8N_IMX290_STANDBY, &TxData, 1U);
    (void)AmbaKAL_TaskSleep(2U);
    RetVal |= RegWrite(pChan, B8N_IMX290_MASTERSTOP, &TxData, 1U);
//    AmbaPrint("[B8N_IMX290] B8N_IMX290_SetStandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
#if 1
static UINT32 B8N_IMX290_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData = 0x0U;

    RetVal |= RegWrite(pChan, B8N_IMX290_STANDBY, &TxData, 1U);
    (void)AmbaKAL_TaskSleep(2U);
    RetVal |= RegWrite(pChan, B8N_IMX290_MASTERSTOP, &TxData, 1U);

    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[B8N_IMX290] B8N_IMX290_SetStandbyOff");
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_ChangeReadoutMode
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
static UINT32 B8N_IMX290_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 i, j, DataSize;
    const B8N_IMX290_SEQ_REG_s *pInitRegTable = &B8N_IMX290InitRegTable[0];
    const B8N_IMX290_REG_s *pModeRegTable = &B8N_IMX290RegTable[0];
    UINT16 FirstAddr, NextAddr;
    UINT8 FirstData, NextData, TxData[64];

    for (i = 0U; i < B8N_IMX290_NUM_INIT_REG; i += DataSize) {
        DataSize = 0;
        FirstAddr = pInitRegTable[i].Addr;
        FirstData = pInitRegTable[i].Data;

        TxData[DataSize] = FirstData;
        DataSize ++;

        for (j = i + 1U; j < B8N_IMX290_NUM_INIT_REG; j++) {
            NextAddr = pInitRegTable[j].Addr;
            NextData = pInitRegTable[j].Data;

            if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U   ) > (UINT16)((sizeof(TxData) / sizeof(TxData[0])) - 2U))) {
                break;
            } else {
                TxData[DataSize] = NextData;
                DataSize ++;
            }
        }

        RetVal |= RegWrite(pChan, FirstAddr, TxData, DataSize);
    }

    for (i = 0U; i < B8N_IMX290_NUM_MODE_REG; i += DataSize) {
        DataSize = 0U;
        FirstAddr = pModeRegTable[i].Addr;
        FirstData = pModeRegTable[i].Data[ModeID];

        TxData[DataSize] = FirstData;
        DataSize ++;

        for (j = i + 1U; j < B8N_IMX290_NUM_MODE_REG; j++) {
            NextAddr = pModeRegTable[j].Addr;
            NextData = pModeRegTable[j].Data[ModeID];

            if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)((sizeof(TxData) / sizeof(TxData[0])) - 2U))) {
                break;
            } else {
                TxData[DataSize] = NextData;
                DataSize++;
            }
        }

        RetVal |= RegWrite(pChan, FirstAddr, TxData, DataSize);
    }

    /* Reset current AE information */
    (void) AmbaWrap_memset(B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo, 0x0, sizeof(B8N_IMX290_AE_INFO_s) * 6U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8N_IMX290_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < B8N_IMX290_NUM_MODE; i++) {
        B8N_IMX290ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)B8N_IMX290SensorInfo[i].LineLengthPck /
                                            ((DOUBLE) B8N_IMX290SensorInfo[i].DataRate
                                             * (DOUBLE)B8N_IMX290SensorInfo[i].NumDataLanes
                                             / (DOUBLE)B8N_IMX290SensorInfo[i].NumDataBits));
        B8N_IMX290ModeInfoList[i].PixelRate = (FLOAT)((DOUBLE)B8N_IMX290OutputInfo[i].DataRate *
                                              (DOUBLE)B8N_IMX290OutputInfo[i].NumDataLanes /
                                              (DOUBLE)B8N_IMX290OutputInfo[i].NumDataBits);
        //AmbaPrint("row time: %f", B8N_IMX290ModeInfoList[i].RowTime);

    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_Init
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
static UINT32 B8N_IMX290_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

//    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "IMX290 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, B8N_IMX290SensorInfo[0U].InputClk);
        B8N_IMX290_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_Enable
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
static UINT32 B8N_IMX290_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = B8N_IMX290_HardwareReset(pChan->VinID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_Disable
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
static UINT32 B8N_IMX290_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = B8N_IMX290_SetStandbyOn(pChan);
    }

    //AmbaPrint("[B8N_IMX290] B8N_IMX290_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_GetStatus
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
static UINT32 B8N_IMX290_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &B8N_IMX290Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_GetModeInfo
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
static UINT32 B8N_IMX290_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= B8N_IMX290_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            B8N_IMX290_PrepareModeInfo(pChan, pMode, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_GetDeviceInfo
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
static UINT32 B8N_IMX290_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &B8N_IMX290DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_GetHdrInfo
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
static UINT32 B8N_IMX290_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pHdrInfo, &B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_GetCurrentGainFactor
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
static UINT32 B8N_IMX290_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
    UINT32 AgcCtrl = 0U;
    DOUBLE GainFactor_D;


    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0U; i < 3U; i ++) {
            if ((pChan->SensorID & ((UINT32)1U << (i * 4U))) != 0U) {
                AgcCtrl = B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[i * 2U].AgcCtrl;
            }
            if ((pChan->SensorID & ((UINT32)1U << ((i * 4U) + 1U) )) != 0U) {
                AgcCtrl = B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[(i * 2U) + 1U].AgcCtrl;
            }
            if (AgcCtrl != 0U) {
                break;
            }
        }
        //AmbaPrint("GainFactor:%f", *pGainFactor);
        (void)AmbaWrap_pow(10.0, 0.015 * (DOUBLE)AgcCtrl, &GainFactor_D);
        pGainFactor[0] = (FLOAT)GainFactor_D;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_GetCurrentShutterSpeed
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
static UINT32 B8N_IMX290_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 RetVal = SENSOR_ERR_NONE;
    const UINT32* pCurrentShutterCtrl = NULL;
    DOUBLE ShutterTimeUnit;
    UINT8 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0U; i < 3U; i ++) {
            if ((pChan->SensorID & ((UINT32)1U << (i * 4U))) != 0U) {
                pCurrentShutterCtrl = &B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[i * 2U].ShutterCtrl[0];
            }
            if ((pChan->SensorID & ((UINT32)1U << ((i * 4U) + 1U))) != 0U) {
                pCurrentShutterCtrl = &B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[(i * 2U) + 1U].ShutterCtrl[0];
            }
            if (pCurrentShutterCtrl != NULL) {
                break;
            }
        }
        if (pCurrentShutterCtrl != NULL) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
                pExposureTime[0] = (FLOAT)ShutterTimeUnit * (FLOAT)pCurrentShutterCtrl[0];
            } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
                for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                    pExposureTime[i] = (FLOAT)ShutterTimeUnit * (FLOAT)pCurrentShutterCtrl[i];
                }
            } else {
                RetVal = SENSOR_ERR_INVALID_API;
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_ConvertGainFactor
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
static UINT32 B8N_IMX290_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
    DOUBLE DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
    DOUBLE LogDesiredFactor = 0.0;
    FLOAT ActualFactor = 0.0f;
    DOUBLE ActualFactor64 = 0.0;
    DOUBLE AnalogGainCtrlInDb;
    UINT32 AnalogGainCtrl;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if(DesiredFactor > 3981.071706) {
            AnalogGainCtrl = 0xF0U;         //Max Again register value
            ActualFactor =  (FLOAT)3981.071706;     //Max Again
        } else if (DesiredFactor >= 1.0) {
            (void)AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            (void)AmbaWrap_floor((66.666666666666667 * LogDesiredFactor), &AnalogGainCtrlInDb);
            (void)AmbaWrap_pow(10.0, 0.015 * AnalogGainCtrlInDb, &ActualFactor64);

            AnalogGainCtrl = (UINT32)AnalogGainCtrlInDb;
            ActualFactor = (FLOAT)ActualFactor64;
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
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 MaxExposureline, MinExposureline;
    UINT32 ExposureLineMargin = 2U;
    DOUBLE ShutterTimeUnit;
    DOUBLE ShutterCtrlInDb;
    UINT32 ShutterCtrl;

    ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

    (void)AmbaWrap_floor((DOUBLE)ExposureTime / ShutterTimeUnit, &ShutterCtrlInDb);
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
        (void)ShutterCtrl;  /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl   = ShutterCtrl;
    *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_ConvertShutterSpeed
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
static UINT32 B8N_IMX290_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

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
 *  @RoutineName:: B8N_IMX290_SetAnalogGainCtrl
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
static UINT32 B8N_IMX290_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData, i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TxData = (UINT8)pAnalogGainCtrl[0];
        RetVal = RegWrite(pChan, B8N_IMX290_AGAIN, &TxData, 1);

        if (RetVal == SENSOR_ERR_NONE) {
            /* Update current AGC control */
            for (i = 0U; i < 3U; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (i * 4U))) != 0U) {
                    B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[i * 2U].AgcCtrl = pAnalogGainCtrl[0];
                }
                if ((pChan->SensorID & ((UINT32)1U << ((i * 4U) + 1U) )) != 0U) {
                    B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[(i * 2U) + 1U].AgcCtrl = pAnalogGainCtrl[0];
                }
            }
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][B8N_IMX290][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_SetShutterCtrl
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
static UINT32 B8N_IMX290_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo;
    UINT8 i, j;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
            RetVal = B8N_IMX290_SetHdrShutterReg(pChan, pShutterCtrl);

            /* update current status */
            if (RetVal == SENSOR_ERR_NONE) {
                for (i = 0U; i < 3U; i ++) {
                    if ((pChan->SensorID & ((UINT32)1U << (i * 4U))) != 0U) {
                        for (j = 0U; j < pModeInfo->HdrInfo.ActiveChannels; j ++) {
                            /* Update current shutter control */
                            B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[i * 2U].ShutterCtrl[j] = pShutterCtrl[j];
                        }
                    }
                    if ((pChan->SensorID & ((UINT32)1U << ((i * 4U) + 1U) )) != 0U) {
                        for (j = 0U; j < pModeInfo->HdrInfo.ActiveChannels; j ++) {
                            /* Update current shutter control */
                            B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[(i * 2U) + 1U].ShutterCtrl[j] = pShutterCtrl[j];
                        }
                    }
                }
            }
        } else {
            RetVal = B8N_IMX290_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            if (RetVal == SENSOR_ERR_NONE) {
                for (i = 0U; i < 3U; i ++) {
                    if ((pChan->SensorID & ((UINT32)1U << (i * 4U))) != 0U) {
                        B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[i * 2U].ShutterCtrl[0] = pShutterCtrl[0];
                    }
                    if ((pChan->SensorID & ((UINT32)1U << ((i * 4U) + 1U) )) != 0U) {
                        B8N_IMX290Ctrl[pChan->VinID].CurrentAEInfo[(i * 2U) + 1U].ShutterCtrl[0] = pShutterCtrl[0];
                    }
                }
            }
        }


        //    SENSOR_DBG_SHR("[Vin%d/Sensor%d][B8N_IMX290][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
        //                      pChan->VinID, pChan->SensorID,  *pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_SetSlowShutterCtrl
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
static UINT32 B8N_IMX290_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        RetVal = B8N_IMX290_SetSlowShutterReg(pChan, SlowShutterCtrl);

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d ][B8N_IMX290][SHR] SlowShutterCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   SlowShutterCtrl);
    }

    return RetVal;
}

static void B8N_IMX290_GetB8VinConfig(UINT32 ChipID, UINT32 ModeID, B8_VIN_MIPI_CONFIG_s *pB8VinCfg, B8_VIN_WINDOW_s *pB8CropWindow)
{
    const B8N_IMX290_SENSOR_INFO_s *pSensorInfo = &B8N_IMX290SensorInfo[ModeID];
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for B8 side VIN (receiving video from SONY IMX290)
    \*-----------------------------------------------------------------------------------------------*/
    B8_VIN_MIPI_CONFIG_s B8N_IMX290_B8VinConfig = {
        .NumDataBits            = 12,
        .DataTypeMask           = 0x1f,
        .DataType               = B8_VIN_MIPI_RGB444,
        .NumActiveLanes         = 4,
        .RxHvSyncCtrl           = {
            .NumActivePixels    = 0,
            .NumActiveLines     = 0,
            .NumTotalPixels     = 0,
            .NumTotalLines      = 0,
        },
        .MipiCtrl              = {
            .HsSettleTime  = 26U,
            .HsTermTime    = 10U,
            .ClkSettleTime = 12U,
            .ClkTermTime   = 4U,
            .ClkMissTime   = 8U,
            .RxInitTime    = 30U
        }
    };

    (void) ChipID;
    (void) AmbaWrap_memcpy(pB8VinCfg, &B8N_IMX290_B8VinConfig, sizeof(B8_VIN_MIPI_CONFIG_s));

    pB8VinCfg->RxHvSyncCtrl.NumActivePixels  = pSensorInfo->OutputWidth;
    pB8VinCfg->RxHvSyncCtrl.NumActiveLines   = pSensorInfo->OutputHeight;
    //pB8VinCfg->RxHvSyncCtrl.NumTotalPixels   = pSensorInfo->OutputWidth;
    //pB8VinCfg->RxHvSyncCtrl.NumTotalLines    = pSensorInfo->OutputHeight;

    pB8VinCfg->NumDataBits                   = pSensorInfo->NumDataBits;
    pB8VinCfg->NumActiveLanes                = pSensorInfo->NumDataLanes;
    pB8VinCfg->DataRate                      = pSensorInfo->DataRate;  /* For mipi parameter calculation */

    pB8CropWindow->StartX = pSensorInfo->RecordingPixels.StartX;
    pB8CropWindow->StartY = pSensorInfo->RecordingPixels.StartY;
    pB8CropWindow->EndX   = (pSensorInfo->RecordingPixels.Width + pSensorInfo->RecordingPixels.StartX) - 1U;
    pB8CropWindow->EndY   = (pSensorInfo->RecordingPixels.Height + pSensorInfo->RecordingPixels.StartY) - 1U;
}

static UINT32 B8N_IMX290_B8Init(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    const B8N_IMX290_MODE_INFO_s *pModeInfo = &B8N_IMX290ModeInfoList[ModeID];
    const B8N_IMX290_SENSOR_INFO_s *pSensorInfo = &B8N_IMX290SensorInfo[ModeID];
    B8_PIN_CONFIG_s PinConfig = {0};
    B8_VIN_MIPI_DPHY_CTRL_s VinMipiDphyCtrl = {0};
    UINT32 ChipID = B8N_IMX290_GetB8ChipID(pChan);
    UINT32 RetVal;

    /* Fill Pin Configure */
    PinConfig.SerDesRate = pModeInfo->B8Ctrl.SerdesDataRate;
    PinConfig.SerDesPinCtrl = B8_PIN_SERDES_CTRL_SINGLE;
    PinConfig.SensorPinCtrl = B8_PIN_SENSOR_CTRL_I2C0;
    PinConfig.VinInterface = B8_VIN_MIPI;
    PinConfig.pVinMipiDphyCtrl = &VinMipiDphyCtrl;
    PinConfig.SensorID = pChan->SensorID;
    VinMipiDphyCtrl.DataRate = pSensorInfo->DataRate;
    VinMipiDphyCtrl.ClockMode = B8_VIN_MIPI_CONTINUOUS_CLOCK_MODE;

    /* B8 Initialization */
    if (AmbaB8_Init(ChipID, &PinConfig) != B8_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    } else {
        RetVal = SENSOR_ERR_NONE;
    }

    return RetVal;
}

static UINT32 B8N_IMX290_B8Config(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    const B8N_IMX290_MODE_INFO_s *pModeInfo = (B8N_IMX290_MODE_INFO_s *)&B8N_IMX290ModeInfoList[ModeID];
    const B8N_IMX290_SENSOR_INFO_s *pSensorInfo = &B8N_IMX290SensorInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &B8N_IMX290OutputInfo[ModeID];
    B8_CONFIG_s B8Config = {0};
    B8_VIN_MIPI_CONFIG_s B8VinConfig = {0};
    UINT32 ChipID = B8N_IMX290_GetB8ChipID(pChan);
    UINT32 SensorNum = B8N_IMX290_GetNumActiveSensor(pChan->SensorID);

    /* Input Info */
    B8Config.InputInfo.SensorID = pChan->SensorID;                                               /* Sensor Id */
    B8Config.InputInfo.SensorInfo.DataRate = pSensorInfo->DataRate;                              /* Sensor Info */
    B8Config.InputInfo.SensorInfo.NumDataLanes = pSensorInfo->NumDataLanes;
    B8Config.InputInfo.SensorInfo.NumDataBits = pSensorInfo->NumDataBits;
    B8Config.InputInfo.SensorInfo.LineLengthPck = (UINT16)pSensorInfo->LineLengthPck;
    B8Config.InputInfo.SensorInfo.FrameLengthLines = (UINT16)pSensorInfo->FrameLengthLines;
    B8Config.InputInfo.SensorInfo.OutputWidth = (UINT16)pSensorInfo->OutputWidth;
    B8Config.InputInfo.SensorInfo.OutputHeight = (UINT16)pSensorInfo->OutputHeight;
    if (B8N_IMX290HdrInfo[ModeID].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
        B8Config.InputInfo.SensorInfo.NumPicturesPerFrame = B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels;
    } else {
        B8Config.InputInfo.SensorInfo.NumPicturesPerFrame = 1;
    }
    B8Config.InputInfo.SensorInfo.BayerPattern = pOutputInfo->BayerPattern;
    B8Config.InputInfo.SensorInfo.RowTime = pModeInfo->RowTime;
    B8Config.InputInfo.VinInterface = B8_VIN_MIPI;                                                  /* Vin Info */
    B8Config.InputInfo.pVinConfig = &B8VinConfig;                                                   /* Vin config */
    B8N_IMX290_GetB8VinConfig(ChipID, ModeID, &B8VinConfig, &B8Config.InputInfo.VinCropWindow);     /* Vin crop window */
    B8Config.InputInfo.PrescalerWidth = pModeInfo->B8Ctrl.CfaOutputWidth;                           /* Frame width after Pre-scaler */

    /* Output Info  */
    B8Config.OutputInfo[B8_VOUT_CHANNEL0].OutputMode = B8_VOUT_MODE_CSI;                            /* Vout0 Info */
    B8Config.OutputInfo[B8_VOUT_CHANNEL0].DataRate = pOutputInfo->DataRate * SensorNum;
    B8Config.OutputInfo[B8_VOUT_CHANNEL0].NumDataLanes = pOutputInfo->NumDataLanes;
    B8Config.OutputInfo[B8_VOUT_CHANNEL0].NumDataBits = pOutputInfo->NumDataBits;

#ifdef B8N_IMX290_ENABLE_B8_DUAL_VOUT
    B8Config.OutputInfo[B8_VOUT_CHANNEL1].OutputMode = B8_VOUT_MODE_CSI;                            /* Vout1 Info */
    B8Config.OutputInfo[B8_VOUT_CHANNEL1].DataRate = pOutputInfo->DataRate * SensorNum;
    B8Config.OutputInfo[B8_VOUT_CHANNEL1].NumDataLanes = pOutputInfo->NumDataLanes;
    B8Config.OutputInfo[B8_VOUT_CHANNEL1].NumDataBits = pOutputInfo->NumDataBits;
#endif

    /* Serdes Info. */
    B8Config.SerdesInfo.SerDesRate = pModeInfo->B8Ctrl.SerdesDataRate;                              /* Serdes bit rate */
    B8Config.SerdesInfo.CompressCtrl.Ratio = pModeInfo->B8Ctrl.CompressionRatio;                    /* Compression */
    B8Config.SerdesInfo.CompressCtrl.Offset = 0;
    B8Config.SerdesInfo.CompressCtrl.DitherCtrl.Enable = 0;

    /* B8 Program */
    if (AmbaB8_Config(ChipID, &B8Config) != B8_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8N_IMX290_Config
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
static UINT32 B8N_IMX290_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &B8N_IMX290Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    UINT32 B8ChipId = B8N_IMX290_GetB8ChipID(pChan);
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

    AMBA_VIN_MIPI_PAD_CONFIG_s B8N_IMX290PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0u,
    };

    if (ModeID >= B8N_IMX290_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

#ifdef B8N_IMX290_ENABLE_B8_DUAL_VOUT
        if (pChan->VinID == 0U) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290] Config Mode: %d, SensorID: 0x%x", ModeID, pChan->SensorID, 0U, 0U, 0U);

            /* update status */
            B8N_IMX290_PrepareModeInfo(pChan, pMode, pModeInfo);

            /* Adjust mipi-phy parameters */
            B8N_IMX290PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
            RetVal |= AmbaVIN_MipiReset(pChan->VinID, &B8N_IMX290PadConfig);

            /* B8 init */
            if (B8N_IMX290_B8Init(pChan, ModeID) != SENSOR_ERR_NONE) {
                RetVal = SENSOR_ERR_SERDES;
            } else {
                /* Sensor Clock from B8  */
                if ((pChan->SensorID & (UINT32)0xf) != 0U) {
                    (void) AmbaUserVIN_SensorClkCtrl(0U, B8N_IMX290SensorInfo[ModeID].InputClk);
                } else {

                    (void) AmbaB8_PllSetSensorClk(B8ChipId, B8N_IMX290SensorInfo[ModeID].InputClk);
                }

                /* Sensor HW reset from B8 GPIO_0 */
                (void) AmbaB8_GpioSetOutput(B8ChipId, B8_GPIO_PIN2, B8_GPIO_LEVEL_LOW);
                (void) AmbaKAL_TaskSleep(3);
                (void) AmbaB8_GpioSetOutput(B8ChipId, B8_GPIO_PIN2, B8_GPIO_LEVEL_HIGH);
                (void) AmbaKAL_TaskSleep(3);

                (void)B8N_IMX290_ChangeReadoutMode(pChan, ModeID);

                (void)B8N_IMX290_SetStandbyOff(pChan);

                /* B8 VIN mipi HS clk locked */
                if ((pChan->SensorID & (UINT16)0x111U) != 0U) {
                    if (AmbaB8_VinWaitMipiSignalLock(B8ChipId, B8_VIN_CHANNEL0, 10000) != B8_ERR_NONE) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AmbaB8_VinWaitMipiSignalLock err @ ChipID 0x%x, Vin0", B8ChipId, 0U, 0U, 0U, 0U);
                    }
                }
                if ((pChan->SensorID & (UINT16)0x222U) != 0U) {
                    if (AmbaB8_VinWaitMipiSignalLock(B8ChipId, B8_VIN_CHANNEL1, 10000) != B8_ERR_NONE) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AmbaB8_VinWaitMipiSignalLock err @ ChipID 0x%x, Vin1", B8ChipId, 0U, 0U, 0U, 0U);
                    }
                }

                /* Sensor stop streaming (vin post config) */
                (void) B8N_IMX290_SetStandbyOn(pChan);


                /* Config B8 */
                if (B8N_IMX290_B8Config(pChan, ModeID) != SENSOR_ERR_NONE) {
                    RetVal = SENSOR_ERR_SERDES;
                } else {

                    (void) B8N_IMX290_SetStandbyOff(pChan);

                    /* config vin */
                    RetVal |= B8N_IMX290_ConfigVin(pChan, pModeInfo);
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290] Config Mode: %d, SensorID: 0x%x", ModeID, pChan->SensorID, 0U, 0U, 0U);

            /* update status */
            B8N_IMX290_PrepareModeInfo(pChan, pMode, pModeInfo);

            /* Adjust mipi-phy parameters */
            B8N_IMX290PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
            RetVal |= AmbaVIN_MipiReset(pChan->VinID, &B8N_IMX290PadConfig);

            /* config vin */
            RetVal |= B8N_IMX290_ConfigVin(pChan, pModeInfo);

        }
#else
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290] Config Mode: %d, SensorID: 0x%x", ModeID, pChan->SensorID, 0U, 0U, 0U);

        /* update status */
        B8N_IMX290_PrepareModeInfo(pChan, pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        B8N_IMX290PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        B8N_IMX290PadConfig.EnabledPin = (((UINT32)1U << pModeInfo->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &B8N_IMX290PadConfig);

        /* B8 init */
        if (B8N_IMX290_B8Init(pChan, ModeID) != SENSOR_ERR_NONE) {
            RetVal = SENSOR_ERR_SERDES;
        } else {
            /* Sensor Clock from B8  */
            if ((pChan->SensorID & (UINT32)0xf) != 0U) {
                (void) AmbaUserVIN_SensorClkCtrl(0U, B8N_IMX290SensorInfo[ModeID].InputClk);
            } else {

                (void) AmbaB8_PllSetSensorClk(B8ChipId, B8N_IMX290SensorInfo[ModeID].InputClk);
            }

            /* Sensor HW reset from B8 GPIO_0 */
            (void) AmbaB8_GpioSetOutput(B8ChipId, B8_GPIO_PIN2, B8_GPIO_LEVEL_LOW);
            (void) AmbaKAL_TaskSleep(3);
            (void) AmbaB8_GpioSetOutput(B8ChipId, B8_GPIO_PIN2, B8_GPIO_LEVEL_HIGH);
            (void) AmbaKAL_TaskSleep(3);

            (void)B8N_IMX290_ChangeReadoutMode(pChan, ModeID);
            (void)B8N_IMX290_SetStandbyOff(pChan);

            /* B8 VIN mipi HS clk locked */
            if ((pChan->SensorID & (UINT16)0x111U) != 0U) {
                if (AmbaB8_VinWaitMipiSignalLock(B8ChipId, B8_VIN_CHANNEL0, 10000) != B8_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AmbaB8_VinWaitMipiSignalLock err @ ChipID 0x%x, Vin0", B8ChipId, 0U, 0U, 0U, 0U);
                }
            }
            if ((pChan->SensorID & (UINT16)0x222U) != 0U) {
                if (AmbaB8_VinWaitMipiSignalLock(B8ChipId, B8_VIN_CHANNEL1, 10000) != B8_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "AmbaB8_VinWaitMipiSignalLock err @ ChipID 0x%x, Vin1", B8ChipId, 0U, 0U, 0U, 0U);
                }
            }

            AmbaB8_VinDPhyReset(B8ChipId, 1);
            AmbaKAL_TaskSleep(2);
            AmbaB8_VinDPhyReset(B8ChipId, 0);

            /* Sensor stop streaming (vin post config) */
            (void) B8N_IMX290_SetStandbyOn(pChan);


            /* Config B8 */
            if (B8N_IMX290_B8Config(pChan, ModeID) != SENSOR_ERR_NONE) {
                RetVal = SENSOR_ERR_SERDES;
            } else {

                (void) B8N_IMX290_SetStandbyOff(pChan);

                /* config vin */
                RetVal |= B8N_IMX290_ConfigVin(pChan, pModeInfo);
            }
        }

#endif
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[B8N_IMX290] Return: %d, B8ChipId: 0x%x", RetVal, B8ChipId, 0U, 0U, 0U);

    return RetVal;
}

#if 0
static INT32 B8N_IMX290_ConfigPost(UINT32 *pVinID)
{
    B8N_IMX290_SetStandbyOff();

    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_B8N_IMX290Obj = {
    .SensorName             = "IMX290",
    .SerdesName             = "B8N",
    .Init                   = B8N_IMX290_Init,
    .Enable                 = B8N_IMX290_Enable,
    .Disable                = B8N_IMX290_Disable,
    .Config                 = B8N_IMX290_Config,
    .GetStatus              = B8N_IMX290_GetStatus,
    .GetModeInfo            = B8N_IMX290_GetModeInfo,
    .GetDeviceInfo          = B8N_IMX290_GetDeviceInfo,
    .GetHdrInfo             = B8N_IMX290_GetHdrInfo,
    .GetCurrentGainFactor   = B8N_IMX290_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = B8N_IMX290_GetCurrentShutterSpeed,

    .ConvertGainFactor      = B8N_IMX290_ConvertGainFactor,
    .ConvertShutterSpeed    = B8N_IMX290_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = B8N_IMX290_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = NULL, //B8N_IMX290_SetDigitalGainCtrl,
    .SetShutterCtrl         = B8N_IMX290_SetShutterCtrl,
    .SetSlowShutterCtrl     = B8N_IMX290_SetSlowShutterCtrl,

    .RegisterRead           = B8N_IMX290_RegisterRead,
    .RegisterWrite          = B8N_IMX290_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
