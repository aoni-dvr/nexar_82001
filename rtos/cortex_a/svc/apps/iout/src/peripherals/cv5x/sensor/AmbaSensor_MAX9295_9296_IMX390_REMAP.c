/**
 *  @file AmbaSensor_MAX9295_9296_IMX390_REMAP.c
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
 *  @details Control APIs of MAX9295_9296 plus SONY IMX390 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaMisraFix.h"

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_IMX390.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSbrg_Max9295_9296_REMAP.h"

#include "bsp.h"

//#define DEBUG_MSG //enable if when need to print all sensor setting
#define MX00_IMX390_I2C_WR_BUF_SIZE 64
#define CONFIG_MX00_IMX390_POSTCFG_ENABLED

static UINT32 MX00_IMX390I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
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

#ifdef CONFIG_MX00_IMX390_IN_SLAVE_MODE
static UINT32 MX00_IMX390MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_SENSOR_MSYNC_CHANNEL,
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
};
#endif
/*-----------------------------------------------------------------------------------------------*\
 * MX00_IMX390 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX00_IMX390_CTRL_s MX00_IMX390Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};

static MAX9295_9296_SERDES_CONFIG_s MX00_IMX390SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX00_IMX390_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_IMX390_I2C_SLAVE_ADDRESS + 0x10U),
            [1] = (UINT8)(MX00_IMX390_I2C_SLAVE_ADDRESS + 0x12U),
        },
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},

        .CSITxSpeed      = {10, 10},
    },
    //VC - VIN1/2/3
    [1] = {0},
    [2] = {0},
    [3] = {0},
    [4] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX00_IMX390_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_IMX390_I2C_SLAVE_ADDRESS + 0x14U),
            [1] = (UINT8)(MX00_IMX390_I2C_SLAVE_ADDRESS + 0x16U),
        },
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},

        .CSITxSpeed      = {10, 10},
    },
    //VC - VIN5/6/7
    [5] = {0},
    [6] = {0},
    [7] = {0},
    [8] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX00_IMX390_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_IMX390_I2C_SLAVE_ADDRESS + 0x20U),
            [1] = (UINT8)(MX00_IMX390_I2C_SLAVE_ADDRESS + 0x22U),
        },
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},

        .CSITxSpeed      = {10, 10},
    },
    //VC - VIN9/10
    [9] = {0},
    [10] = {0},
    [11] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX00_IMX390_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_IMX390_I2C_SLAVE_ADDRESS + 0x24U),
            [1] = (UINT8)(MX00_IMX390_I2C_SLAVE_ADDRESS + 0x26U),
        },
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},

        .CSITxSpeed      = {10, 10},
    },
    //VC - VIN12/13
    [12] = {0},
    [13] = {0},

};

static UINT16 MX00_IMX390_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX9295_9296_IMX224_GetNumActiveSensor
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
static UINT32 MX00_IMX390_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
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
 *  @RoutineName:: MX00_IMX390_PrepareModeInfo
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
static void MX00_IMX390_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                               = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s              *pInputInfo     = &MX00_IMX390_InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s             *pOutputInfo    = &MX00_IMX390_OutputInfo[ModeID];
    const MX00_IMX390_SENSOR_INFO_s     *pSensorInfo    = &MX00_IMX390_SensorInfo[ModeID];
    const MX00_IMX390_FRAME_TIMING_s    *pFrameTime     = &MX00_IMX390ModeInfoList[ModeID].FrameTime;
    UINT32 SensorNum = MX00_IMX390_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;

    if (SensorNum != 0U) {
        (void)AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

        pModeInfo->FrameLengthLines         = pSensorInfo->FrameLengthLines * SensorNum;
        pModeInfo->NumExposureStepPerFrame  = pSensorInfo->FrameLengthLines;
        pModeInfo->InternalExposureOffset   = 0.0f;
        pModeInfo->RowTime = MX00_IMX390ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
        (void) AmbaWrap_floor((((DOUBLE)MX00_IMX390ModeInfoList[ModeID].PixelRate * (DOUBLE)SensorNum) * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal);
        pModeInfo->LineLengthPck            = (UINT32)FloorVal;
        pModeInfo->InputClk                 = pSensorInfo->InputClk;

        (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
        (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
        (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
        (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        (void)AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX00_IMX390_HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information

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
 *  @RoutineName:: MX00_IMX390_ConfigSerDes
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
static void MX00_IMX390_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const MX00_IMX390_SENSOR_INFO_s *pSensorInfo = &MX00_IMX390_SensorInfo[SensorMode];
    MAX9295_9296_SERDES_CONFIG_s *pSerdesCfg = &MX00_IMX390SerdesConfig[pChan->VinID];
    DOUBLE FloorVal;

    pSerdesCfg->EnabledLinkID = MX00_IMX390_GetEnabledLinkID(pChan->SensorID);

    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;
    (void) AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal);
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    (void) Max9295_9296_Remap_Config(pChan->VinID, &MX00_IMX390SerdesConfig[pChan->VinID]);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_ConfigVin
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
static UINT32 MX00_IMX390_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX00_IMX390VinConfig = {
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
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &MX00_IMX390VinConfig;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;


    (void)AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    return AmbaVIN_MipiConfig(VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_ResetSensor
 *
 *  @Description:: Reset IMX390 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MX00_IMX390_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan->SensorID & MX00_IMX390_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_Remap_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    if ((pChan->SensorID & MX00_IMX390_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_Remap_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
    if ((pChan->SensorID & MX00_IMX390_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_Remap_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    if ((pChan->SensorID & MX00_IMX390_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_Remap_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start */

    if (RetVal != MAX9295_9296_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
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
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX00_IMX390_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;
#ifdef DEBUG_MSG
    UINT32 k;
#endif

    if ((pChan == NULL) || (Size > MX00_IMX390_SENSOR_I2C_MAX_SIZE)) {
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
        RetVal = AmbaI2C_MasterWrite(MX00_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST_PLUS,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        if (pChan->SensorID == (MX00_IMX390_SENSOR_ID_CHAN_A | MX00_IMX390_SENSOR_ID_CHAN_B)) {
            I2cConfig.SlaveAddr = MX00_IMX390SerdesConfig[pChan->VinID].SensorSlaveID;

            /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390] slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
            RetVal = AmbaI2C_MasterWrite(MX00_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST_PLUS,
                                         &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390][Broadcast] Slave ID 0x%2x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U);
            for (k = 0U; k < Size; k++) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390]Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
            }
#endif

        } else {
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_IMX390SerdesConfig[pChan->VinID].SensorAliasID[i];
                    /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
                    RetVal |= AmbaI2C_MasterWrite(MX00_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST_PLUS,
                                                  &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390][SensorID 0x%02x] Slave ID 0x%2x", (1 << i), I2cConfig.SlaveAddr, 0U, 0U, 0U);
                    for (k = 0U; k < Size; k++) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390]Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
                    }
#endif
                }
            }
        }
        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390] VinID %d I2C_Ch %d Slave 0x%x Addr 0x%04x Size %d I2C does not work!!!!!", pChan->VinID, MX00_IMX390I2cChannel[pChan->VinID], I2cConfig.SlaveAddr, Addr, Size);
        }
    }

    //SENSOR_DBG_IO("[MX00_IMX390][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_RegisterWrite
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
static UINT32 MX00_IMX390_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
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

static UINT32 MX00_IMX390_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    if ((pChan->SensorID != MX00_IMX390_SENSOR_ID_CHAN_A) && (pChan->SensorID != MX00_IMX390_SENSOR_ID_CHAN_B)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cTxConfig.DataSize  = 2U;
        I2cTxConfig.pDataBuf  = TxData;
        TxData[0] = (UINT8)(Addr >> 8U);
        TxData[1] = (UINT8)(Addr & 0xffU);

        I2cRxConfig.DataSize  = 1U;
        I2cRxConfig.pDataBuf  = pRxData;

        if (pChan->SensorID == MX00_IMX390_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX00_IMX390SerdesConfig[pChan->VinID].SensorAliasID[0];
            I2cRxConfig.SlaveAddr = (MX00_IMX390SerdesConfig[pChan->VinID].SensorAliasID[0] | (UINT32)1U);
        } else {
            I2cTxConfig.SlaveAddr = MX00_IMX390SerdesConfig[pChan->VinID].SensorAliasID[1];
            I2cRxConfig.SlaveAddr = (MX00_IMX390SerdesConfig[pChan->VinID].SensorAliasID[1] | (UINT32)1U);
        }

        /*        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x, 0x%x", I2cTxConfig.SlaveAddr, I2cRxConfig.SlaveAddr, 0U, 0U, 0U); */
        RetVal = AmbaI2C_MasterReadAfterWrite(MX00_IMX390I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST_PLUS, 1U, &I2cTxConfig,
                                              &I2cRxConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390] VinID %d Addr 0x%04x I2C does not work!!!!!", pChan->VinID, Addr, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_RegisterRead
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
static UINT32 MX00_IMX390_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_IMX390_RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_SetAnalogGainReg
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
static UINT32 MX00_IMX390_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl)
{
    UINT32 RetVal;
    UINT8 WData[2];

    /* SP1L */
    WData[0] = (UINT8)(AnalogGainCtrl & 0xffU);
    WData[1] = (UINT8)((AnalogGainCtrl >> 8U) & 0x7U);
    RetVal = RegWrite(pChan, MX00_IMX390_AGAIN_SP1L, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_SetDigitalGainReg
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
static UINT32 MX00_IMX390_SetDigitalGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 DigitalGainCtrl)
{
    UINT32 RetVal;
    UINT8 WData[2];

    /* SP1L */
    WData[0] = (UINT8)(DigitalGainCtrl & 0xffU);
    WData[1] = (UINT8)((DigitalGainCtrl >> 8U) & 0x7U);
    RetVal = RegWrite(pChan, MX00_IMX390_PGA_SP1H, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_SetWbGainReg
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
static UINT32 MX00_IMX390_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
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
    RetVal |= RegWrite(pChan, MX00_IMX390_WBGAIN_R, WData, 8);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_SetShutterReg
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
static UINT32 MX00_IMX390_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 ShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHS1;
    UINT8 WData[4];

    /* SP1H & SP1L */
    SHS1 = MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - ShutterCtrl;

    WData[0] = (UINT8)(SHS1 & 0xffU);
    WData[1] = (UINT8)((SHS1 >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHS1 >> 16U) & 0xfU);
    RetVal |= RegWrite(pChan, MX00_IMX390_SHS1, WData, 3);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_SetHdrAnalogGainReg
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
static UINT32 MX00_IMX390_SetHdrAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[2];

    /* SP1H */
    WData[0] = (UINT8)(pAnalogGainCtrl[0] & 0xffU);
    WData[1] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x7U);
    RetVal |= RegWrite(pChan, MX00_IMX390_AGAIN_SP1H, WData, 2);

    /* SP1L */
    WData[0] = (UINT8)(pAnalogGainCtrl[1] & 0xffU);
    WData[1] = (UINT8)((pAnalogGainCtrl[1] >> 8U) & 0x7U);
    RetVal |= RegWrite(pChan, MX00_IMX390_AGAIN_SP1L, WData, 2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_SetHdrShutterReg
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
static UINT32 MX00_IMX390_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHSX;
    UINT8 WData[4];

    /* SP1H & SP1L */
    SHSX = MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - pShutterCtrl[0];

    WData[0] = (UINT8)(SHSX & 0xffU);
    WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHSX >> 16U) & 0xfU);
    RetVal |= RegWrite(pChan, MX00_IMX390_SHS1, WData, 3);

    /* SP2 */
    SHSX = MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame - pShutterCtrl[2];

    WData[0] = (UINT8)(SHSX & 0xffU);
    WData[1] = (UINT8)((SHSX >> 8U) & 0xffU);
    WData[2] = (UINT8)((SHSX >> 16U) & 0xfU);
    RetVal |= RegWrite(pChan, MX00_IMX390_SHS2, WData, 3);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_SetSlowShutterReg
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
static UINT32 MX00_IMX390_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    AmbaMisra_TouchUnused(&IntegrationPeriodInFrame);
    return SENSOR_ERR_NONE;
#if 0
    UINT32 TargetFrameLengthLines;
    UINT32 ModeID = MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData[1];

    if (IntegrationPeriodInFrame < 1U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TargetFrameLengthLines = MX00_IMX390ModeInfoList[ModeID].FrameTime.VMAX * IntegrationPeriodInFrame;

        if (TargetFrameLengthLines > 0x3ffffU) {
            TargetFrameLengthLines = 0x3ffffU;
        }

        TxData[0] = IntegrationPeriodInFrame - 1;

        RetVal |= RegWrite(pChan, MX00_IMX390_FMAX, TxData, 1U);

        /* Update frame rate information */
        MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale = MX00_IMX390ModeInfoList[ModeID].FrameTime.FrameRate.TimeScale / IntegrationPeriodInFrame;
    }

    return RetVal;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_StandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX390_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x01;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= RegWrite(pChan, MX00_IMX390_STANDBY, &TxData, 1);
//    AmbaPrint("[MX00_IMX390] MX00_IMX390_StandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_StandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX390_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 TxData = 0x00;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= RegWrite(pChan, MX00_IMX390_STANDBY, &TxData, 1);

    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * MX00_IMX390Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / MX00_IMX390Ctrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[MX00_IMX390] MX00_IMX390_StandbyOff");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_Query_VerID
 *
 *  @Description:: Check sensor chip version
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pVerID:    pointer to MX00_IMX390 version ID
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX390_Query_VerID(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 *pVerID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 Reg0x3060 = 0xffU, Reg0x3067 = 0xffU, Reg0x3064 = 0xffU;

    if (pVerID == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        RetVal |= MX00_IMX390_RegRead(pChan, 0x3060U, &Reg0x3060);
        RetVal |= MX00_IMX390_RegRead(pChan, 0x3067U, &Reg0x3067);
        RetVal |= MX00_IMX390_RegRead(pChan, 0x3064U, &Reg0x3064);

        if (RetVal == SENSOR_ERR_NONE) {

            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390] Reg0x3060: 0x%02x, Reg0x3067: 0x%02x, Reg0x3064: 0x%02x", Reg0x3060, Reg0x3067, Reg0x3064, 0U, 0U);

            if ((Reg0x3060 == 0x70U) && ((Reg0x3067 >> 4U) == 0x5U)) {
                *pVerID = MX00_IMX390_CHIP_VERSION_60DEG;
            } else if ((Reg0x3060 == 0x70U) || (Reg0x3060 == 0x71U)) {
                *pVerID = MX00_IMX390_CHIP_VERSION_CASE5;
            } else if (((Reg0x3060 == 0x01U) && (Reg0x3067 == 0x10U)) || ((Reg0x3060 == 0x00U) && (Reg0x3067 == 0x30U))
                       || (((Reg0x3060 == 0x02U) && (Reg0x3067 == 0x30U)) && ((Reg0x3064 == 0x05U) || (Reg0x3064 == 0x06U) || (Reg0x3064 == 0x07U) || (Reg0x3064 == 0x14U) || (Reg0x3064 == 0x15U)))) {
                *pVerID = MX00_IMX390_CHIP_VERSION_CASE7;
            } else if (((Reg0x3060 == 0x00U) && (Reg0x3067 == 0x40U)) || (((Reg0x3060 == 0x02U) && (Reg0x3067 == 0x30U)) && ((Reg0x3064 == 0x08U) || (Reg0x3064 == 0x09U) || (Reg0x3064 == 0x0AU) || (Reg0x3064 == 0x0BU) || (Reg0x3064 == 0x0CU)))) {
                *pVerID = MX00_IMX390_CHIP_VERSION_CASE8;
            } else {
                *pVerID = MX00_IMX390_CHIP_VERSION_CASE8;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390] ============  Unknown chip version  ============", 0U, 0U, 0U, 0U, 0U);

            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_ChangeReadoutMode
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
static UINT32 MX00_IMX390_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;


    UINT16 i, j, DataSize;
    const MX00_IMX390_REG_s *pModeRegTable = &MX00_IMX390_RegTableCase5[0];
    const MX00_IMX390_SEQ_REG_s *pModeRegTableOTP = &MX00_IMX390_RegTableCase7_OTPM4[0];
    UINT16 FirstAddr, NextAddr;
    UINT8 FirstData, NextData;
    UINT8 TxData[64];
    UINT32 VerID = 0U;
    UINT32 RegNum = MX00_IMX390_NUM_REG_CASE5, RegNum_OTP = MX00_IMX390_NUM_REG_CASE7_OTPM4;
    UINT8 OTP_F=0;

    if (MX00_IMX390_Query_VerID(pChan, &VerID) != SENSOR_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_9296_IMX390] Unable to get chip version", 0U, 0U, 0U, 0U, 0U);
    } else {
        if (VerID == MX00_IMX390_CHIP_VERSION_60DEG) {
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_9296_IMX390] Not support 60 deg setting", 0U, 0U, 0U, 0U, 0U);
        } else if (VerID == MX00_IMX390_CHIP_VERSION_CASE5) {      //Case5
            pModeRegTable = &MX00_IMX390_RegTableCase5[0];
            RegNum = MX00_IMX390_NUM_REG_CASE5;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_9296_IMX390] IMX390 Ver: case5 (RegTableSize: %u)", RegNum, 0U, 0U, 0U, 0U);
        } else if (VerID == MX00_IMX390_CHIP_VERSION_CASE7) {      //Case 7                                             //Case7
            OTP_F=1;
            pModeRegTable = &MX00_IMX390_RegTableCase7[0];
            RegNum = MX00_IMX390_NUM_REG_CASE7;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_9296_IMX390] IMX390 Ver: case7 (RegTableSize: %u)", RegNum, 0U, 0U, 0U, 0U);
            if ((ModeID == 5U) || (ModeID == 8U)) {               //Case7 - M2 OTP
                pModeRegTableOTP = &MX00_IMX390_RegTableCase7_OTPM2[0];
                RegNum_OTP = MX00_IMX390_NUM_REG_CASE7_OTPM2;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_9296_IMX390] IMX390 Ver: case7M2OTP (RegTableSize: %u)", MX00_IMX390_NUM_REG_CASE7_OTPM2, 0U, 0U, 0U, 0U);
            } else {                                               //Case7 - M4 OTP
                pModeRegTableOTP = &MX00_IMX390_RegTableCase7_OTPM4[0];
                RegNum_OTP = MX00_IMX390_NUM_REG_CASE7_OTPM4;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_9296_IMX390] IMX390 Ver: case7M4OTP (RegTableSize: %u)", MX00_IMX390_NUM_REG_CASE7_OTPM4, 0U, 0U, 0U, 0U);
            }
        } else {                                                   //Case8 or unknown
            OTP_F=1;
            pModeRegTable = &MX00_IMX390_RegTableCase8[0];
            RegNum = MX00_IMX390_NUM_REG_CASE8;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_9296_IMX390] IMX390 Ver: case8 (RegTableSize: %u)", RegNum, 0U, 0U, 0U, 0U);
            if ((ModeID == 5U) || (ModeID == 8U)) {               //Case8 - M2 OTP
                pModeRegTableOTP = &MX00_IMX390_RegTableCase8_OTPM2[0];
                RegNum_OTP = MX00_IMX390_NUM_REG_CASE8_OTPM2;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_9296_IMX390] IMX390 Ver: case8M2OTP (RegTableSize: %u)", MX00_IMX390_NUM_REG_CASE8_OTPM2, 0U, 0U, 0U, 0U);
            } else {                                               //Case8 - M4 OTP
                pModeRegTableOTP = &MX00_IMX390_RegTableCase8_OTPM4[0];
                RegNum_OTP = MX00_IMX390_NUM_REG_CASE8_OTPM4;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAXIM9295_9296_IMX390] IMX390 Ver: case8M4OTP (RegTableSize: %u)", MX00_IMX390_NUM_REG_CASE8_OTPM4, 0U, 0U, 0U, 0U);
            }
        }
    }

    for (i = 0U; i < RegNum; i += DataSize) {
        DataSize = 0;
        FirstAddr = pModeRegTable[i].Addr;
        FirstData = pModeRegTable[i].Data[ModeID];

        TxData[DataSize] = FirstData;
        DataSize++;

        for (j = i + 1U; j < RegNum; j++) {
            NextAddr = pModeRegTable[j].Addr;
            NextData = pModeRegTable[j].Data[ModeID];

            if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)((UINT16)MX00_IMX390_I2C_WR_BUF_SIZE - 3U))) {
                break;
            } else {
                TxData[DataSize] = NextData;
                DataSize++;
            }
        }

        RetVal = RegWrite(pChan, FirstAddr, TxData, DataSize);
    }

    if (OTP_F == 1U) {
        for (i = 0U; i < RegNum_OTP; i += DataSize) {
            DataSize = 0;
            FirstAddr = pModeRegTableOTP[i].Addr;
            FirstData = pModeRegTableOTP[i].Data;

            TxData[DataSize] = FirstData;
            DataSize++;

            for (j = i + 1U; j < RegNum_OTP; j++) {
                NextAddr = pModeRegTableOTP[j].Addr;
                NextData = pModeRegTableOTP[j].Data;

                if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)((UINT16)MX00_IMX390_I2C_WR_BUF_SIZE - 3U))) {
                    break;
                } else {
                    TxData[DataSize] = NextData;
                    DataSize++;
                }
            }

            RetVal = RegWrite(pChan, FirstAddr, TxData, DataSize);
        }
    }

    /* Reset current AE information */
    (void) AmbaWrap_memset(MX00_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl, 0x0, sizeof(UINT32) * 3U * MX00_IMX390_NUM_MAX_SENSOR_COUNT);
    (void) AmbaWrap_memset(MX00_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl, 0x0, sizeof(UINT32) * 3U * MX00_IMX390_NUM_MAX_SENSOR_COUNT);
    (void) AmbaWrap_memset(MX00_IMX390Ctrl[pChan->VinID].CurrentWbCtrl, 0x0, sizeof(AMBA_SENSOR_WB_CTRL_s) * 3U * MX00_IMX390_NUM_MAX_SENSOR_COUNT);
    (void) AmbaWrap_memset(MX00_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl, 0x0, sizeof(UINT32) * 3U * MX00_IMX390_NUM_MAX_SENSOR_COUNT);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_IMX390_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;

    for (i = 0; i < MX00_IMX390_NUM_MODE; i++) {

        MX00_IMX390ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)MX00_IMX390_SensorInfo[i].LineLengthPck /
                                             ((DOUBLE)MX00_IMX390_SensorInfo[i].DataRate *
                                              (DOUBLE)MX00_IMX390_SensorInfo[i].NumDataLanes /
                                              (DOUBLE)MX00_IMX390_SensorInfo[i].NumDataBits));
        pOutputInfo = &MX00_IMX390_OutputInfo[i];
        MX00_IMX390ModeInfoList[i].PixelRate = (FLOAT) ((DOUBLE) pOutputInfo->DataRate *
                                               (DOUBLE)pOutputInfo->NumDataLanes /
                                               (DOUBLE)pOutputInfo->NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_Init
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
static UINT32 MX00_IMX390_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const MX00_IMX390_FRAME_TIMING_s *pFrameTime = &MX00_IMX390ModeInfoList[0U].FrameTime;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "MX00_IMX390 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        MX00_IMX390_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_Enable
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
static UINT32 MX00_IMX390_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;


    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX00_IMX390_ResetSensor(pChan);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390] MX00_IMX390_Enable", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_Disable
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
static UINT32 MX00_IMX390_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_IMX390_StandbyOn(pChan);
#ifdef CONFIG_MX00_IMX390_SHORTEN_SYNC_LATENCY
        if (RetVal == OK) {
            RetVal = Max9295_9296_Remap_DeInit(pChan->VinID);
        }
        if (RetVal == OK) {
            RetVal |= AmbaVIN_MasterSyncDisable(MX00_IMX390MasterSyncChannel[pChan->VinID]);
        }
#endif
    }
    //AmbaPrint("[MX00_IMX390] MX00_IMX390_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_GetStatus
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
static UINT32 MX00_IMX390_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &MX00_IMX390Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_GetModeInfo
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
static UINT32 MX00_IMX390_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= MX00_IMX390_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            MX00_IMX390_PrepareModeInfo(pChan, pMode, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_GetDeviceInfo
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
static UINT32 MX00_IMX390_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &MX00_IMX390_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_GetHdrInfo
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
static UINT32 MX00_IMX390_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pHdrInfo, &MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_GetCurrentGainFactor
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
static UINT32 MX00_IMX390_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k, TotalGainCtrl;
    DOUBLE GainFactor64 = 0.0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    TotalGainCtrl = (MX00_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] +
                                     MX00_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]);
                    (void)AmbaWrap_pow(10.0, 0.015 * (DOUBLE)TotalGainCtrl, &GainFactor64);
                    pGainFactor[0] = (FLOAT)GainFactor64;
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        TotalGainCtrl = (MX00_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] +
                                         MX00_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]);
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
 *  @RoutineName:: MX00_IMX390_GetCurrentShutterSpeed
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
static UINT32 MX00_IMX390_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    DOUBLE ShutterTimeUnit;
    UINT8 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    pExposureTime[0] = ((FLOAT)ShutterTimeUnit * (FLOAT)MX00_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][0]);
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pExposureTime[k] = ((FLOAT)ShutterTimeUnit * (FLOAT)MX00_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][k]);
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
 *  @RoutineName:: MX00_IMX390_ConvertGainFactor
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
static UINT32 MX00_IMX390_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType = MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
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
            Desire_dB_SP1H = (Desire_dB_SP1H >= MX00_IMX390_MAX_TOTAL_GAIN) ? MX00_IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1H;
            Desire_dB_SP1L = (Desire_dB_SP1L >= MX00_IMX390_MAX_TOTAL_GAIN) ? MX00_IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1L;

            /* Minimum check */
            Desire_dB_SP1H = (Desire_dB_SP1H <= 0.0f) ? 0.0f : Desire_dB_SP1H;
            Desire_dB_SP1L = (Desire_dB_SP1L <= 0.0f) ? 0.0f : Desire_dB_SP1L;

            /* Maximum ABS between SP1H and SP1L is 30dB */
            if ((Desire_dB_SP1H - Desire_dB_SP1L) >= MX00_IMX390_MAX_AGAIN) {
                Desire_dB_SP1L = Desire_dB_SP1H - MX00_IMX390_MAX_AGAIN;
            }
            if ((Desire_dB_SP1H - Desire_dB_SP1L) <= -MX00_IMX390_MAX_AGAIN) {
                Desire_dB_SP1L = Desire_dB_SP1H + MX00_IMX390_MAX_AGAIN;
            }

            /* Calculate Agc/Dgc for SP1H/SP1L */
            if ((Desire_dB_SP1H <= MX00_IMX390_MAX_AGAIN) && (Desire_dB_SP1L <= MX00_IMX390_MAX_AGAIN)) {
                Agc_dB_SP1H = Desire_dB_SP1H;
                Agc_dB_SP1L = Desire_dB_SP1L;
                Dgc_dB = 0.0f;
            } else if ((Desire_dB_SP1H >= MX00_IMX390_MAX_AGAIN) && (Desire_dB_SP1L >= MX00_IMX390_MAX_AGAIN)) {
                if (Desire_dB_SP1H > Desire_dB_SP1L) {
                    Agc_dB_SP1H = MX00_IMX390_MAX_AGAIN;
                    Agc_dB_SP1L = MX00_IMX390_MAX_AGAIN - (Desire_dB_SP1H - Desire_dB_SP1L);
                    Dgc_dB = Desire_dB_SP1H - MX00_IMX390_MAX_AGAIN;
                } else {
                    Agc_dB_SP1H = MX00_IMX390_MAX_AGAIN - (Desire_dB_SP1L - Desire_dB_SP1H);
                    Agc_dB_SP1L = MX00_IMX390_MAX_AGAIN;
                    Dgc_dB = Desire_dB_SP1L - MX00_IMX390_MAX_AGAIN;
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
            Desire_dB_SP1 = (Desire_dB_SP1 >= MX00_IMX390_MAX_TOTAL_GAIN) ? MX00_IMX390_MAX_TOTAL_GAIN : Desire_dB_SP1;

            /* Minimum check */
            Desire_dB_SP1 = (Desire_dB_SP1 <= 0.0f) ? 0.0f : Desire_dB_SP1;

            /* Calculate Agc/Dgc for SP1 */
            if (Desire_dB_SP1 <= MX00_IMX390_MAX_AGAIN) {
                Agc_dB_SP1 = Desire_dB_SP1;
                Dgc_dB = 0.0f;
            } else {
                Agc_dB_SP1 = MX00_IMX390_MAX_AGAIN;
                Dgc_dB = Desire_dB_SP1 - MX00_IMX390_MAX_AGAIN;
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
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
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

#ifdef CONFIG_MX00_IMX390_IN_SLAVE_MODE
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
 *  @RoutineName:: MX00_IMX390_ConvertShutterSpeed
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
static UINT32 MX00_IMX390_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
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
 *  @RoutineName:: MX00_IMX390_SetAnalogGainCtrl
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
static UINT32 MX00_IMX390_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_IMX390_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

            /* Update current analog gain control */
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = pAnalogGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX390_SetHdrAnalogGainReg(pChan, pAnalogGainCtrl);

            /* Update current analog gain control */
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_IMX390Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] = pAnalogGainCtrl[k];
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
 *  @RoutineName:: MX00_IMX390_SetDigitalGainCtrl
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
static UINT32 MX00_IMX390_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_IMX390_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]= pDigitalGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX390_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_IMX390Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]= pDigitalGainCtrl[k];
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
 *  @RoutineName:: MX00_IMX390_SetWbGainCtrl
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
static UINT32 MX00_IMX390_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_IMX390_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX390Ctrl[pChan->VinID].CurrentWbCtrl[i][0] = pWbGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX390_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_IMX390Ctrl[pChan->VinID].CurrentWbCtrl[i][k] = pWbGainCtrl[k];
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
 *  @RoutineName:: MX00_IMX390_SetShutterCtrl
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
static UINT32 MX00_IMX390_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_IMX390_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][0] = pShutterCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX390_SetHdrShutterReg(pChan, pShutterCtrl);

            /* Update current shutter control */
            for (i = 0U; i < MX00_IMX390_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_IMX390Ctrl[pChan->VinID].CurrentShutterCtrl[i][k] = pShutterCtrl[k];
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
 *  @RoutineName:: MX00_IMX390_SetSlowShrCtrl
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
static UINT32 MX00_IMX390_SetSlowShrCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        RetVal = MX00_IMX390_SetSlowShutterReg(SlowShutterCtrl);
    }

    return RetVal;
}

#ifdef CONFIG_MX00_IMX390_IN_SLAVE_MODE
static UINT32 MX00_IMX390_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = 0;
    DOUBLE PeriodInDb;
    const MX00_IMX390_MODE_INFO_s* pModeInfo = &MX00_IMX390ModeInfoList[ModeID];
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg  = {0};

    (void) AmbaWrap_floor(((DOUBLE)pModeInfo->FrameTime.FrameRate.NumUnitsInTick * ((DOUBLE) MX00_IMX390_SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameTime.FrameRate.TimeScale)) + 0.5, &PeriodInDb);
    MasterSyncCfg.RefClk            = MX00_IMX390_SensorInfo[ModeID].InputClk;
    MasterSyncCfg.HSync.Period      = (UINT32)PeriodInDb;
    MasterSyncCfg.HSync.PulseWidth  = 8U;
    MasterSyncCfg.HSync.Polarity    = 0U;
    MasterSyncCfg.VSync.Period      = 1U;
    MasterSyncCfg.VSync.PulseWidth  = 1000U;
    MasterSyncCfg.VSync.Polarity    = 0U;
    MasterSyncCfg.HSyncDelayCycles  = 1U;
    MasterSyncCfg.VSyncDelayCycles  = 0U;
    MasterSyncCfg.ToggleHsyncInVblank  = 1U;

#ifdef CONFIG_MX00_IMX390_SHARING_MASTER_SYNC
    // TODO: use the same AMBA_SENSOR_MSYNC_CHANNEL on HW to make different VIN start together.
    RetVal |= AmbaVIN_MasterSyncEnable(MX00_IMX390MasterSyncChannel[pChan->VinID], &MasterSyncCfg);
#else
    {
        UINT32 i;
        // enable all master sync together to make different VIN start close.
        for (i = 0; i < AMBA_NUM_VIN_CHANNEL; i ++) {
            RetVal |= AmbaVIN_MasterSyncEnable(MX00_IMX390MasterSyncChannel[i], &MasterSyncCfg);
        }
    }
    AmbaMisra_TouchUnused(&pChan);
#endif

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX390_Config
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
static UINT32 MX00_IMX390_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    AMBA_VIN_MIPI_PAD_CONFIG_s MX00_IMX390PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    UINT8 LaneShift[AMBA_NUM_VIN_CHANNEL] = {
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
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    UINT8 Data;
    AMBA_SENSOR_CHANNEL_s pChan_t;
    UINT32 EnableLink = 0U;

#if 1
    AMBA_VIN_LANE_REMAP_CONFIG_s pLaneRemapConfig = {0};
    UINT8 remap[4] = {0};
    UINT32 RetVal1 = SENSOR_ERR_NONE;
#endif

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] Add delay time to wait MAX9296/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] Power off", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] Power on", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(3);


    if (ModeID >= MX00_IMX390_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] ============  [ MX00_IMX390_Config ]  ============", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] ============ init mode:%d ============", ModeID, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] ======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

        /* update status */
        MX00_IMX390_PrepareModeInfo(pChan, pMode, pModeInfo);

#ifdef CONFIG_MX00_IMX390_IN_SLAVE_MODE
#ifndef CONFIG_MX00_IMX390_SHORTEN_SYNC_LATENCY
        RetVal |= AmbaVIN_MasterSyncDisable(MX00_IMX390MasterSyncChannel[pChan->VinID]);
#endif
#endif
        /* Adjust mipi-phy parameters */
        MX00_IMX390PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        MX00_IMX390PadConfig.EnabledPin = ((UINT32) 0xC) << LaneShift[pChan->VinID];
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] ======  data rate: %d, Enable Pin: 0x%x  ======", MX00_IMX390PadConfig.DateRate, MX00_IMX390PadConfig.EnabledPin, 0U, 0U, 0U);
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX00_IMX390PadConfig);

        /* After reset VIN, set MAX9296A PWDNB high */
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] Set MAX9296A PWDNB low", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(3);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] Set MAX9296A PWDNB high", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_9296_Remap_Init(pChan->VinID, MX00_IMX390_GetEnabledLinkID(pChan->SensorID));

        MX00_IMX390_ConfigSerDes(pChan, ModeID, pModeInfo);

        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX00_IMX390_SENSOR_ID_CHAN_A) != 0U) {
            (void)Max9295_9296_Remap_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_27M);
        }
        if ((pChan->SensorID & MX00_IMX390_SENSOR_ID_CHAN_B) != 0U) {
            (void)Max9295_9296_Remap_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_27M);
        }

        (void)MX00_IMX390_ResetSensor(pChan);


        /* program sensor */
        (void)MX00_IMX390_ChangeReadoutMode(pChan, ModeID);

#ifdef CONFIG_MX00_IMX390_IN_SLAVE_MODE
        Data = 0x0aU;
        RetVal |= RegWrite(pChan, 0x365cU, &Data, 1U);
        Data = 0x0aU;
        RetVal |= RegWrite(pChan, 0x365eU, &Data, 1U);
        Data = 0x06U;
        RetVal |= RegWrite(pChan, 0x23c2U, &Data, 1U);
        Data = 0x01U;
        RetVal |= RegWrite(pChan, 0x3650U, &Data, 1U);
#endif
        (void)MX00_IMX390_StandbyOff(pChan);

#ifdef CONFIG_MX00_IMX390_IN_SLAVE_MODE
        RetVal |= MX00_IMX390_ConfigMasterSync(pChan, ModeID);
#endif

        (void)AmbaKAL_TaskSleep(22); //TBD: To avoid i2c fail for gain setting

        // for new IMX390 version
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[MAX9295_9296_IMX390_REMAP] Config: Please ignore following \"I2C does not work\" msg to wait i2c communication ready.", NULL, NULL, NULL, NULL, NULL);
        while(EnableLink != pChan->SensorID) {
            if(((pChan->SensorID & MX00_IMX390_SENSOR_ID_CHAN_A) != 0U) && ((EnableLink & MX00_IMX390_SENSOR_ID_CHAN_A) != MX00_IMX390_SENSOR_ID_CHAN_A)) {
                pChan_t.VinID = pChan->VinID;
                pChan_t.SensorID = MX00_IMX390_SENSOR_ID_CHAN_A;
                (void)MX00_IMX390_RegRead(&pChan_t, 0x5001, &Data);
                if(Data == 3U) {
                    EnableLink |= MX00_IMX390_SENSOR_ID_CHAN_A;
                } else {
                    //check again
                }
            }
            if(((pChan->SensorID & MX00_IMX390_SENSOR_ID_CHAN_B) != 0U) && ((EnableLink & MX00_IMX390_SENSOR_ID_CHAN_B) != MX00_IMX390_SENSOR_ID_CHAN_B)) {
                pChan_t.VinID = pChan->VinID;
                pChan_t.SensorID = MX00_IMX390_SENSOR_ID_CHAN_B;
                (void)MX00_IMX390_RegRead(&pChan_t, 0x5001, &Data);
                if(Data == 3U) {
                    EnableLink |= MX00_IMX390_SENSOR_ID_CHAN_B;
                } else {
                    //check again
                }
            }

            if(EnableLink == pChan->SensorID) {
                break;
            } else {
                (void)AmbaKAL_TaskSleep(1);
            }
        }

#ifdef CONFIG_MX00_IMX390_POSTCFG_ENABLED
        {
            //mask the IMX390 MIPI frame output
            //set MUTE_VSYNC_MASK_EN=1
            Data = ((UINT8)1U << 2U);
            RetVal |= RegWrite(pChan, 0x0088U, &Data, 1U);
            //set SM_MUTE_VSYNC_MASK_EN_APL=1
            Data = ((UINT8)1U << 6U);
            RetVal |= RegWrite(pChan, 0x03C0U, &Data, 1U);
        }
#endif
        /* config vin */
        RetVal |= MX00_IMX390_ConfigVin(pChan->VinID, pModeInfo);
#if 1
        //after AmbaVIN_MipiConfig
        pLaneRemapConfig.NumActiveLanes = pModeInfo->OutputInfo.NumDataLanes;
        remap[0] = 3 + LaneShift[pChan->VinID];
        remap[1] = 2 + LaneShift[pChan->VinID];
        remap[2] = 1 + LaneShift[pChan->VinID];
        remap[3] = 0 + LaneShift[pChan->VinID];
        pLaneRemapConfig.pPhyLaneMapping = remap;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Bf LaneRemap] RetVal1 = 0x%x", RetVal1, 0U, 0U, 0U, 0U);
        RetVal1 = AmbaVIN_DataLaneRemap(pChan->VinID, &pLaneRemapConfig);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Af LaneRemap] RetVal1 = 0x%x", RetVal1, 0U, 0U, 0U, 0U);
#endif

#ifdef CONFIG_MX00_IMX390_POSTCFG_ENABLED
        {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "wait two frames", NULL, NULL, NULL, NULL, NULL);

            //waits two frame time until mask IMX390 mipi output
            (void)AmbaKAL_TaskSleep(2U * 1000U * MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.NumUnitsInTick / MX00_IMX390Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale);

            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "leave sensor config", NULL, NULL, NULL, NULL, NULL);
        }
#endif
    }

    return RetVal;
}
#ifdef CONFIG_MX00_IMX390_POSTCFG_ENABLED
static UINT32 MX00_IMX390_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 Data;
    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "MX01_IMX390_ConfigPost_REMAP", NULL, NULL, NULL, NULL, NULL);
    //unmask the IMX390 MIPI frame output
    //set MUTE_VSYNC_MASK_EN=0
    Data = 0x0;
    RetVal |= RegWrite(pChan, 0x0088U, &Data, 1U);
    //set SM_MUTE_VSYNC_MASK_EN_APL=0
    RetVal |= RegWrite(pChan, 0x03C0U, &Data, 1U);

    return RetVal;
}
#endif
/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX00_IMX390_REMAPObj = {
    .SensorName             = "IMX390REMAP",
    .SerdesName             = "MAX9295_9296REMAP",
    .Init                   = MX00_IMX390_Init,
    .Enable                 = MX00_IMX390_Enable,
    .Disable                = MX00_IMX390_Disable,
    .Config                 = MX00_IMX390_Config,
    .GetStatus              = MX00_IMX390_GetStatus,
    .GetModeInfo            = MX00_IMX390_GetModeInfo,
    .GetDeviceInfo          = MX00_IMX390_GetDeviceInfo,
    .GetHdrInfo             = MX00_IMX390_GetHdrInfo,
    .GetCurrentGainFactor   = MX00_IMX390_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX00_IMX390_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX00_IMX390_ConvertGainFactor,
    .ConvertShutterSpeed    = MX00_IMX390_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX00_IMX390_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MX00_IMX390_SetDigitalGainCtrl,
    .SetWbGainCtrl          = MX00_IMX390_SetWbGainCtrl,
    .SetShutterCtrl         = MX00_IMX390_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX00_IMX390_SetSlowShrCtrl,

    .RegisterRead           = MX00_IMX390_RegisterRead,
    .RegisterWrite          = MX00_IMX390_RegisterWrite,

#ifdef CONFIG_MX00_IMX390_POSTCFG_ENABLED
    .ConfigPost             = MX00_IMX390_ConfigPost,
#else
    .ConfigPost             = NULL,
#endif
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};

