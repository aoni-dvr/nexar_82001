/**
 *  @file AmbaSensor_MAX9295_9296_IMX424_RCCB.c
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
 *  @details Control APIs of MAXIM SerDes + SONY IMX424 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaVIN.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_IMX424_RCCB.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSbrg_Max9295_9296.h"

#include "bsp.h"

#define  MX_IMX424_IN_SLAVE_MODE


static UINT32 MX00_IMX424I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_MAXIM_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_MAXIM_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_MAXIM_I2C_CHANNEL_PIP2,
    [AMBA_VIN_CHANNEL3] = AMBA_MAXIM_I2C_CHANNEL_PIP3,
    [AMBA_VIN_CHANNEL4] = AMBA_MAXIM_I2C_CHANNEL_PIP4,
    [AMBA_VIN_CHANNEL5] = AMBA_MAXIM_I2C_CHANNEL_PIP5,
};

#ifdef MX_IMX424_IN_SLAVE_MODE
static UINT32 MX00_IMX424MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_MSYNC_CHANNEL_PIP2,
    [AMBA_VIN_CHANNEL3] = AMBA_SENSOR_MSYNC_CHANNEL_PIP3,
    [AMBA_VIN_CHANNEL4] = AMBA_SENSOR_MSYNC_CHANNEL_PIP4,
    [AMBA_VIN_CHANNEL5] = AMBA_SENSOR_MSYNC_CHANNEL_PIP5
};
#endif

/*-----------------------------------------------------------------------------------------------*\
 * MX00_IMX424 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX00_IMX424_CTRL_s MX00_IMX424Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};

static MAX9295_9296_SERDES_CONFIG_s MX00_IMX424SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x10,
            [1] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x12,
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [1] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x14,
            [1] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x16,
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [2] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x20,
            [1] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x22,
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [3] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x24,
            [1] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x26,
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [4] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x30,
            [1] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x32,
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [5] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x34,
            [1] = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR + 0x36,
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
};

static UINT16 MX00_IMX424_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 i, EnabledLinkID = 0U;

    for (i = 0U; i < MX00_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}

static UINT32 MX00_IMX424_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0, i;

    for (i = 0U; i < MX00_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            SensorNum ++;
        }
    }

    return SensorNum;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_PrepareModeInfo
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
static void MX00_IMX424_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                        = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s *pInputInfo           = &MX00_IMX424InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo         = &MX00_IMX424OutputInfo[ModeID];
    const MX00_IMX424_SENSOR_INFO_s *pSensorInfo = &MX00_IMX424SensorInfo[ModeID];
    UINT32 SensorNum = MX00_IMX424_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    if(ModeID >= MX00_IMX424_NUM_MODE) {
        ModeID = MX00_IMX424_NUM_MODE -1U;
    }

    //AmbaPrint("Sensor Mode:%d, Input Mode:%d, Output Mode:%d",SensorMode,InputMode,OutputMode);
    if(SensorNum != 0U) {
        U32RVal |= AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

        pModeInfo->FrameLengthLines         = pSensorInfo->FrameLengthLines * SensorNum;
        pModeInfo->NumExposureStepPerFrame  = pSensorInfo->VMAX * 500U;
        pModeInfo->RowTime                  = MX00_IMX424ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
        pModeInfo->InputClk                 = pSensorInfo->InputClk;
        U32RVal |= AmbaWrap_floor((((DOUBLE)MX00_IMX424ModeInfoList[ModeID].PixelRate * (DOUBLE)SensorNum) * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal);
        pModeInfo->LineLengthPck            = (UINT32)FloorVal;
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &MX00_IMX424ModeInfoList[ModeID].FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &MX00_IMX424ModeInfoList[ModeID].FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX00_IMX424HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

        /* updated minimum frame rate limitation */
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pModeInfo->MinFrameRate.TimeScale /= 8U;
        }

    }
    if(U32RVal != 0U) {
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_ConfigSerDes
 *
 *  @Description:: Configure Serdes sync/video forwarding, I2C translation, and MIPI I/F
 *
 *  @Input      ::
 *      pChan:        Vin ID and sensor ID
 *      SensorMode:  Sensor mode
 *      pModeInfo:   Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_IMX424_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s *pImgInfo = &pModeInfo->OutputInfo;
    const MX00_IMX424_SENSOR_INFO_s *pSensorInfo = &MX00_IMX424SensorInfo[SensorMode];
    MAX9295_9296_SERDES_CONFIG_s *pSerdesCfg = &MX00_IMX424SerdesConfig[pChan->VinID];
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    pSerdesCfg->EnabledLinkID = MX00_IMX424_GetEnabledLinkID(pChan->SensorID);
    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;

    U32RVal |= AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal);
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    (void) Max9295_9296_Config(pChan->VinID, &MX00_IMX424SerdesConfig[pChan->VinID]);
    if(U32RVal != 0U) {
        /* MisraC */
    }
}

#ifdef MX_IMX424_IN_SLAVE_MODE
/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: MX00_IMX424_ConfigMasterSync
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
static UINT32 MX00_IMX424_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal;
    DOUBLE PeriodInDb;
    const MX00_IMX424_MODE_INFO_s *pModeInfo = &MX00_IMX424ModeInfoList[ModeID];
    const MX00_IMX424_SENSOR_INFO_s *pSensorInfo = &MX00_IMX424SensorInfo[ModeID];
    static AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncConfig = {
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
        .HSyncDelayCycles = 0,
        .VSyncDelayCycles = 0,
        .ToggleHsyncInVblank = 1
    };
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_floor(((DOUBLE)pModeInfo->FrameRate.NumUnitsInTick * ((DOUBLE) MX00_IMX424SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameRate.TimeScale)) + 0.5, &PeriodInDb);

    MasterSyncConfig.RefClk = pSensorInfo->InputClk;
    MasterSyncConfig.HSync.Period      = (UINT32)PeriodInDb;
    MasterSyncConfig.HSync.PulseWidth  = 8U;
    MasterSyncConfig.HSync.Polarity    = 0U;
    MasterSyncConfig.VSync.Period      = 1U;
    MasterSyncConfig.VSync.PulseWidth  = 1000U;
    MasterSyncConfig.VSync.Polarity    = 0U;
    MasterSyncConfig.ToggleHsyncInVblank  = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(MX00_IMX424MasterSyncChannel[pChan->VinID], &MasterSyncConfig);

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "MasterSync RefClk(%u) Hsync Period(%u)", pSensorInfo->InputClk, (UINT32)PeriodInDb, 0U, 0U, 0U);
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_ConfigVin
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
static UINT32 MX00_IMX424_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID = pModeInfo->Config.ModeID;
    const AMBA_SENSOR_OUTPUT_INFO_s  *pOutputInfo = &MX00_IMX424OutputInfo[ModeID];
    AMBA_VIN_MIPI_CONFIG_s VinCfg;
    UINT32 SensorNum = MX00_IMX424_GetNumActiveSensor(pChan->SensorID);
    UINT32 HdrExpNum = MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels;
    UINT32 U32RVal = 0;

    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    AMBA_VIN_MIPI_CONFIG_s MX00_IMX424_VinConfig = {
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

    U32RVal |= AmbaWrap_memcpy(&VinCfg, &MX00_IMX424_VinConfig, sizeof(AMBA_VIN_MIPI_CONFIG_s));

    VinCfg.NumActiveLanes       = pOutputInfo->NumDataLanes;
    VinCfg.Config.NumDataBits   = pOutputInfo->NumDataBits;
    VinCfg.Config.BayerPattern  = pOutputInfo->BayerPattern;

    if ((MX00_IMX424HdrInfo[ModeID].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE)) {
        VinCfg.Config.RxHvSyncCtrl.NumActivePixels   = pOutputInfo->RecordingPixels.Width * SensorNum * HdrExpNum;
        VinCfg.Config.RxHvSyncCtrl.NumActiveLines    = pOutputInfo->RecordingPixels.Height / HdrExpNum;
        VinCfg.Config.RxHvSyncCtrl.NumTotalPixels    = pOutputInfo->OutputWidth * SensorNum * HdrExpNum;
        VinCfg.Config.RxHvSyncCtrl.NumTotalLines     = pOutputInfo->OutputHeight / HdrExpNum;

        VinCfg.Config.SplitCtrl.NumSplits = (UINT8)(SensorNum * HdrExpNum);      /* note: should be <= 4 */
        VinCfg.Config.SplitCtrl.SplitWidth = pOutputInfo->OutputWidth;
    } else {
        VinCfg.Config.RxHvSyncCtrl.NumActivePixels   = pOutputInfo->RecordingPixels.Width * SensorNum;
        VinCfg.Config.RxHvSyncCtrl.NumActiveLines    = pOutputInfo->RecordingPixels.Height;
        VinCfg.Config.RxHvSyncCtrl.NumTotalPixels    = pOutputInfo->OutputWidth * SensorNum;
        VinCfg.Config.RxHvSyncCtrl.NumTotalLines     = pOutputInfo->OutputHeight;

        if (SensorNum > 1U) {
            VinCfg.Config.SplitCtrl.NumSplits = 0;// mark it off since we use side by side mode temporarily. SensorNum;
            VinCfg.Config.SplitCtrl.SplitWidth = pOutputInfo->OutputWidth;
        }
    }

    U32RVal |= AmbaWrap_memcpy(&(VinCfg.Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return AmbaVIN_MipiConfig(pChan->VinID, &VinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_RegRW
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
    UINT32 i, k;
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX00_IMX424_I2C_WR_BUF_SIZE + 2U];
    UINT32 TxSize;

    if ((pChan == NULL) || ((pChan->SensorID & (MX00_IMX424_SENSOR_ID_CHAN_A | MX00_IMX424_SENSOR_ID_CHAN_B)) == 0x0U)
        || (Size > MX00_IMX424_I2C_WR_BUF_SIZE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        I2cConfig.DataSize  = Size + 2U;
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8);
        TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);

        for (i = 0; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

        if (pChan->SensorID == (MX00_IMX424_SENSOR_ID_CHAN_A | MX00_IMX424_SENSOR_ID_CHAN_B)) { /* broadcast to all sensors */
            I2cConfig.SlaveAddr = MX00_IMX424_SENSOR_I2C_SLAVE_ADDR;
            RetVal |= AmbaI2C_MasterWrite(MX00_IMX424I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);

            if (RetVal != I2C_ERR_NONE) {
                RetVal = SENSOR_ERR_COMMUNICATE;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work (SlaveID=0x%02x)!!!!!", I2cConfig.SlaveAddr, 0, 0, 0, 0);
            }
        } else {
            for (k = 0; k < MX00_IMX424_NUM_MAX_SENSOR_COUNT; k++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (k + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_IMX424SerdesConfig[pChan->VinID].SensorAliasID[k];
                    RetVal |= AmbaI2C_MasterWrite(MX00_IMX424I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);
                    break;
                }
            }

            if (RetVal != I2C_ERR_NONE) {
                RetVal = SENSOR_ERR_COMMUNICATE;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work (SlaveID=0x%02x)!!!!!", I2cConfig.SlaveAddr, 0, 0, 0, 0);
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_RegisterWrite
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
static UINT32 MX00_IMX424_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
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

static UINT32 RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxDataBuf[2];
    UINT32 TxSize;

    if ((pChan == NULL) || ((pChan->SensorID != MX00_IMX424_SENSOR_ID_CHAN_A) && (pChan->SensorID != MX00_IMX424_SENSOR_ID_CHAN_B))) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        I2cTxConfig.DataSize = 2U;
        I2cTxConfig.pDataBuf = TxDataBuf;
        TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8);
        TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);

        I2cRxConfig.DataSize = 1U;
        I2cRxConfig.pDataBuf = pRxData;

        if (pChan->SensorID == MX00_IMX424_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX00_IMX424SerdesConfig[pChan->VinID].SensorAliasID[0];
            I2cRxConfig.SlaveAddr = (MX00_IMX424SerdesConfig[pChan->VinID].SensorAliasID[0] | (UINT32)0x1U);
        } else {
            /* (pChan->SensorID == MX00_IMX424_SENSOR_ID_CHAN_B) */
            I2cTxConfig.SlaveAddr = MX00_IMX424SerdesConfig[pChan->VinID].SensorAliasID[1];
            I2cRxConfig.SlaveAddr = (MX00_IMX424SerdesConfig[pChan->VinID].SensorAliasID[1] | (UINT32)0x1U);
        }

        RetVal = AmbaI2C_MasterReadAfterWrite(MX00_IMX424I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U,
                                              &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);
        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C(Read) does not work (SlaveID=0x%02x)!!!!!", I2cTxConfig.SlaveAddr, 0, 0, 0, 0);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_RegisterRead
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
static UINT32 MX00_IMX424_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData = 0U;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_SetAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      AGC:    Analog gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX424_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AGC)
{
    UINT8 WData[2];

    WData[0] = (UINT8)(AGC & 0xffU);
    WData[1] = (UINT8)((AGC >> 8U) & 0x01U);

    return RegWrite(pChan, MX00_IMX424_AGAIN, WData, 2U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_SetWbGainReg
 *
 *  @Description:: Configure sensor WB gain setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pWbGainCtrl:     WB gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX424_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[8];

    WData[0] = (UINT8)(pWbGainCtrl[0].Gb & 0xffU);
    WData[1] = (UINT8)((pWbGainCtrl[0].Gb >> 8U) & 0xfU);
    WData[2] = (UINT8)(pWbGainCtrl[0].B & 0xffU);
    WData[3] = (UINT8)((pWbGainCtrl[0].B >> 8U) & 0xfU);
    WData[4] = (UINT8)(pWbGainCtrl[0].R & 0xffU);
    WData[5] = (UINT8)((pWbGainCtrl[0].R >> 8U) & 0xfU);
    WData[6] = (UINT8)(pWbGainCtrl[0].Gr & 0xffU);
    WData[7] = (UINT8)((pWbGainCtrl[0].Gr >> 8U) & 0xfU);

    RetVal |= RegWrite(pChan, MX00_IMX424_WBGAIN_CF0, WData, 8);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void MX00_IMX424_CheckFineShutter(const UINT16 *pFineShutterD, UINT16 *pFineShutterA)
{
    UINT16 FineShutterD, FineShutterA;
    FineShutterD = *pFineShutterD;
    FineShutterA = FineShutterD;

    if(FineShutterD < 1U) {
        FineShutterA = 1U;
    } else if((FineShutterD > 26U) && (FineShutterD < 131U)) {
        FineShutterA = 26U;
    } else if((FineShutterD > 205U) && (FineShutterD < 481U)) {
        FineShutterA = 205U;
    } else { /* MisraC */ }
    *pFineShutterA = FineShutterA;
}

static UINT32 MX00_IMX424_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    MX00_IMX424_INTEG_REG_s Integ[3] = {0};
    UINT8 WData[6U];
    //UINT16 FineShutterA;

    /* Coarse shutter time (Long exposure) */
    Integ[0].Coarse = pShutterCtrl[0] / 500U;
    Integ[0].Fine = (UINT16)(pShutterCtrl[0] % 500U);
    WData[0] = (UINT8)(Integ[0].Coarse & 0xffU);
    WData[1] = (UINT8)((Integ[0].Coarse >> 8U) & 0xffU);
    WData[2] = (UINT8)((Integ[0].Coarse >> 16U) & 0x01U);
    RetVal |= RegWrite(pChan, MX00_IMX424_INTGL_COARSE, WData, 3U);

    /* Coarse shutter time (Medium exposure) */
    Integ[1].Coarse = pShutterCtrl[1] / 500U;
    Integ[1].Fine = (UINT16)(pShutterCtrl[1] % 500U);
    WData[0] = (UINT8)(Integ[1].Coarse & 0xffU);
    WData[1] = (UINT8)((Integ[1].Coarse >> 8U) & 0xffU);
    WData[2] = (UINT8)((Integ[1].Coarse >> 16U) & 0x01U);
    RetVal |= RegWrite(pChan, MX00_IMX424_INTGM_COARSE, WData, 3U);

    /* Coarse shutter time (Short exposure) */
    Integ[2].Coarse = pShutterCtrl[2] / 500U;
    Integ[2].Fine = (UINT16)(pShutterCtrl[2] % 500U);
    WData[0] = (UINT8)(Integ[2].Coarse & 0xffU);
    WData[1] = (UINT8)((Integ[2].Coarse >> 8U) & 0xffU);
    WData[2] = (UINT8)((Integ[2].Coarse >> 16U) & 0x01U);
    RetVal |= RegWrite(pChan, MX00_IMX424_INTGS_COARSE, WData, 3U);

    /* Fine shutter time (Long, Medium and Short exposures) */
#if 1
    WData[0] = (UINT8)(Integ[0].Fine & 0xffU);
    WData[1] = (UINT8)((Integ[0].Fine >> 8U) & 0x0fU);
    WData[2] = (UINT8)(Integ[1].Fine & 0xffU);
    WData[3] = (UINT8)((Integ[1].Fine >> 8U) & 0x0fU);
    WData[4] = (UINT8)(Integ[2].Fine & 0xffU);
    WData[5] = (UINT8)((Integ[2].Fine >> 8U) & 0x0fU);
    RetVal |= RegWrite(pChan, MX00_IMX424_INTGL_FINE, WData, 6U);
#else
    MX00_IMX424_CheckFineShutter(&Integ[0].Fine, &FineShutterA);
    WData[0] = (UINT8)(FineShutterA & 0xffU);
    WData[1] = (UINT8)((FineShutterA >> 8U) & 0x0fU);
    MX00_IMX424_CheckFineShutter(&Integ[1].Fine, &FineShutterA);
    WData[2] = (UINT8)(FineShutterA & 0xffU);
    WData[3] = (UINT8)((FineShutterA >> 8U) & 0x0fU);
    MX00_IMX424_CheckFineShutter(&Integ[1].Fine, &FineShutterA);
    WData[4] = (UINT8)(FineShutterA & 0xffU);
    WData[5] = (UINT8)((FineShutterA >> 8U) & 0x0fU);
    RetVal |= RegWrite(pChan, MX00_IMX424_INTGL_FINE, WData, 6U);
#endif
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_SetSlowShutterReg
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
static UINT32 MX00_IMX424_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    (void) IntegrationPeriodInFrame;
    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX424_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData = 0x1U;

    RetVal |= RegWrite(pChan, MX00_IMX424_STANDBY, &TxData, 1U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX424_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData = 0x0U;

    RetVal |= RegWrite(pChan, MX00_IMX424_STANDBY, &TxData, 1U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_ResetSensor
 *
 *  @Description:: Reset IMX424 Image Sensor Device
 *
 *  @Input      ::
 *      pChan:       Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX424_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan->SensorID & MX00_IMX424_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    if ((pChan->SensorID & MX00_IMX424_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
    if ((pChan->SensorID & MX00_IMX424_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    if ((pChan->SensorID & MX00_IMX424_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(1);  /* wait for communcation start >= 20us*/

    if (RetVal != MAX9295_9296_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_ChangeReadoutMode
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
static UINT32 MX00_IMX424_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 i, j, DataSize;
    const MX00_IMX424_MODE_REG_s *pModeRegTable = &MX00_IMX424RegTable[0];
    UINT16 FirstAddr, NextAddr;
    UINT8 FirstData, NextData, TxData[MX00_IMX424_I2C_WR_BUF_SIZE];
    UINT32 U32RVal = 0;

    /*
    for (i = 0U; i < MX00_IMX424_NUM_REGTABLE_REG; i += DataSize) {
        DataSize = 0;
        FirstAddr = pModeRegTable[i].Addr;
        FirstData = pModeRegTable[i].Data[ModeID];

        TxData[DataSize] = FirstData;
        DataSize++;

        for (j = i + 1U; j < MX00_IMX424_NUM_REGTABLE_REG; j++) {
            NextAddr = pModeRegTable[j].Addr;
            NextData = pModeRegTable[j].Data[ModeID];

            if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)(MX00_IMX424_I2C_WR_BUF_SIZE))) {
                break;
            } else {
                TxData[DataSize] = NextData;
                DataSize++;
            }
        }

        RetVal |= RegWrite(pChan, FirstAddr, TxData, DataSize);
    }
    */
    i = 0U;
    while(i < MX00_IMX424_NUM_REGTABLE_REG) {
        DataSize = 0;
        FirstAddr = pModeRegTable[i].Addr;
        FirstData = pModeRegTable[i].Data[ModeID];

        TxData[DataSize] = FirstData;
        DataSize++;

        for (j = i + 1U; j < MX00_IMX424_NUM_REGTABLE_REG; j++) {
            NextAddr = pModeRegTable[j].Addr;
            NextData = pModeRegTable[j].Data[ModeID];

            if (((NextAddr - FirstAddr) != (j - i)) || (((j - i) + 1U) > (UINT16)(MX00_IMX424_I2C_WR_BUF_SIZE))) {
                break;
            } else {
                if(DataSize < MX00_IMX424_I2C_WR_BUF_SIZE) {
                    TxData[DataSize] = NextData;
                    DataSize++;
                }
            }
        }

        RetVal |= RegWrite(pChan, FirstAddr, TxData, DataSize);

        i += DataSize;
    }

    /* Reset current AE information */
    if(pChan->VinID >= AMBA_NUM_VIN_CHANNEL) {
        U32RVal |= AmbaWrap_memset(MX00_IMX424Ctrl[AMBA_NUM_VIN_CHANNEL-1U].CurrentAEInfo, 0x0, sizeof(MX00_IMX424_AE_INFO_s) * MX00_IMX424_NUM_MAX_SENSOR_COUNT);
    } else {
        U32RVal |= AmbaWrap_memset(MX00_IMX424Ctrl[pChan->VinID].CurrentAEInfo, 0x0, sizeof(MX00_IMX424_AE_INFO_s) * MX00_IMX424_NUM_MAX_SENSOR_COUNT);
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_IMX424_PreCalculateModeInfo(void)
{
    const AMBA_VIN_FRAME_RATE_s *pFrameRate;
    UINT8 i;

    for (i = 0U; i < MX00_IMX424_NUM_MODE; i++) {
        pFrameRate = &MX00_IMX424ModeInfoList[i].FrameRate;

        MX00_IMX424ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)pFrameRate->NumUnitsInTick /
                                             ((DOUBLE)MX00_IMX424SensorInfo[i].FrameLengthLines *
                                              (DOUBLE)pFrameRate->TimeScale));

        MX00_IMX424ModeInfoList[i].PixelRate = (FLOAT)((DOUBLE)MX00_IMX424OutputInfo[i].DataRate *
                                               (DOUBLE)MX00_IMX424OutputInfo[i].NumDataLanes /
                                               (DOUBLE)MX00_IMX424OutputInfo[i].NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_Init
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
static UINT32 MX00_IMX424_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

//    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "IMX424 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, MX00_IMX424SensorInfo[0U].InputClk);
        MX00_IMX424_PreCalculateModeInfo();

    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_Enable
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
static UINT32 MX00_IMX424_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX00_IMX424_ResetSensor(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_Disable
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
static UINT32 MX00_IMX424_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_IMX424_SetStandbyOn(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_GetStatus
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
static UINT32 MX00_IMX424_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pStatus, &MX00_IMX424Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_GetModeInfo
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
static UINT32 MX00_IMX424_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if ((Config.ModeID >= MX00_IMX424_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            MX00_IMX424_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_GetDeviceInfo
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
static UINT32 MX00_IMX424_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &MX00_IMX424DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_GetHdrInfo
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
static UINT32 MX00_IMX424_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pHdrInfo, &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_GetCurrentGainFactor
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
static UINT32 MX00_IMX424_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i, k;
    DOUBLE GainFactor64 = 0.0;
    UINT32 U32RVal = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API;
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < MX00_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    U32RVal |= AmbaWrap_pow(10.0, 0.005 * (DOUBLE)MX00_IMX424Ctrl[pChan->VinID].CurrentAEInfo[i].AgcCtrl, &GainFactor64);
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pGainFactor[k] = (FLOAT)GainFactor64;
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_GetCurrentShutterSpeed
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
static UINT32 MX00_IMX424_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 FrameLengthLines;// = pModeInfo->FrameLengthLines;
    UINT32 VMAX;
    UINT32 RetVal = SENSOR_ERR_NONE;
    const UINT32 *pShutterCtrl;
    FLOAT CoarseShutter, FineShutter;
    MX00_IMX424_INTEG_REG_s Integ = {0};
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
        FrameLengthLines = pModeInfo->FrameLengthLines;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API;
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            VMAX = pModeInfo->NumExposureStepPerFrame / 500U;
            CoarseShutter = pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)VMAX;
            FineShutter = CoarseShutter / 500.0f;
            for (i = 0U; i < MX00_IMX424_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pShutterCtrl = &MX00_IMX424Ctrl[pChan->VinID].CurrentAEInfo[i].ShutterCtrl[k];
                        Integ.Coarse = (*pShutterCtrl) / 500U;
                        Integ.Fine = (UINT16)((*pShutterCtrl) % 500U);
                        pExposureTime[k] = ((FLOAT)Integ.Coarse * CoarseShutter) + ((FLOAT)Integ.Fine * FineShutter);
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
 *  @RoutineName:: MX00_IMX424_ConvertGainFactor
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
static UINT32 MX00_IMX424_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType;// = MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
    DOUBLE DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
    DOUBLE LogDesiredFactor = 1.0;
    FLOAT ActualFactor = 0.0f;
    DOUBLE ActualFactor64 = 0.0;
    DOUBLE AnalogGainCtrlInDb;
    UINT32 AnalogGainCtrl;
    AMBA_SENSOR_WB_GAIN_FACTOR_s WbGain;
    DOUBLE WbGainInDb;
    UINT32 U32RVal = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        HdrType = MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
        if (DesiredFactor > 31.622776) {
            AnalogGainCtrl = 300U;
            ActualFactor = (FLOAT) 31.622776;
        } else if (DesiredFactor >= 1.0) {
            U32RVal |= AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            U32RVal |= AmbaWrap_floor((200.0 * LogDesiredFactor), &AnalogGainCtrlInDb);
            U32RVal |= AmbaWrap_pow(10.0, 0.005 * AnalogGainCtrlInDb, &ActualFactor64);

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

        /* WB gain */
        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb >= 15.996094f) ? 15.996094f : pDesiredFactor->WbGain[0].Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B >= 15.996094f)  ? 15.996094f : pDesiredFactor->WbGain[0].B;
        WbGain.R  = (pDesiredFactor->WbGain[0].R >= 15.996094f)  ? 15.996094f : pDesiredFactor->WbGain[0].R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr >= 15.996094f) ? 15.996094f : pDesiredFactor->WbGain[0].Gr;

        WbGain.Gb = (pDesiredFactor->WbGain[0].Gb <= 1.0f) ? 1.0f : WbGain.Gb;
        WbGain.B  = (pDesiredFactor->WbGain[0].B <= 1.0f)  ? 1.0f : WbGain.B;
        WbGain.R  = (pDesiredFactor->WbGain[0].R <= 1.0f)  ? 1.0f : WbGain.R;
        WbGain.Gr = (pDesiredFactor->WbGain[0].Gr <= 1.0f) ? 1.0f : WbGain.Gr;

        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gb * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].Gb = (UINT32)WbGainInDb;
        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.B * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].B = (UINT32)WbGainInDb;
        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.R * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].R = (UINT32)WbGainInDb;
        U32RVal |= AmbaWrap_floor((DOUBLE)(WbGain.Gr * 256.0), &WbGainInDb);
        pGainCtrl->WbGain[0].Gr = (UINT32)WbGainInDb;

        pActualFactor->WbGain[0].Gb = (FLOAT)pGainCtrl->WbGain[0].Gb / 256.0f;
        pActualFactor->WbGain[0].B  = (FLOAT)pGainCtrl->WbGain[0].B  / 256.0f;
        pActualFactor->WbGain[0].R  = (FLOAT)pGainCtrl->WbGain[0].R  / 256.0f;
        pActualFactor->WbGain[0].Gr = (FLOAT)pGainCtrl->WbGain[0].Gr / 256.0f;

        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            U32RVal |= AmbaWrap_memcpy(&pGainCtrl->WbGain[1], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));
            U32RVal |= AmbaWrap_memcpy(&pGainCtrl->WbGain[2], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));

            U32RVal |= AmbaWrap_memcpy(&pActualFactor->WbGain[1], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
            U32RVal |= AmbaWrap_memcpy(&pActualFactor->WbGain[2], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
        }
        if(U32RVal != 0U) {
            /* MisraC */
        }
    }

    return RetVal;
}

#if 0
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
    UINT32 MaxExposureline, MinExposureline;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ShutterCtrl;
    DOUBLE ShutterTimeUnit, ShutterCtrlInDb;
    UINT32 U32RVal = 0;

    ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

    U32RVal |= AmbaWrap_floor(((DOUBLE)ExposureTime / ShutterTimeUnit), &ShutterCtrlInDb);
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    /* VMAX is 17 bits */
    if (ShutterCtrl > ((0x1ffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame)) {
        ShutterCtrl = ((0x1ffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame);
    }

    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MaxExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
        /* For none-HDR mode */
    } else {
        /* TBD */
        MaxExposureline = (((ShutterCtrl / pModeInfo->NumExposureStepPerFrame) + 1U) * pModeInfo->NumExposureStepPerFrame);
        MinExposureline = 1U;
    }

    if (ShutterCtrl > MaxExposureline) {
        ShutterCtrl = MaxExposureline;
    } else if (ShutterCtrl < MinExposureline) {
        ShutterCtrl = MinExposureline;
    } else {
        /* MisraC */  /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl   = ShutterCtrl;
    *pActualExptime = (FLOAT)((DOUBLE)ShutterCtrl * ShutterTimeUnit);
    if(U32RVal != 0U) {
        /* MisraC */
    }
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ConvertHdrShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      pModeInfo:       pointer to mode info of target mode
 *      pExposureTime:   Desired Exposure time / shutter speed
 *
 *  @Output     ::
 *      pActualExpTime:  Achievable exposure time
 *      pShutterCtrl:    Shutter control for achievable exposure time
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void ConvertHdrShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, const FLOAT *pExposureTime, FLOAT *pActualExpTime, UINT32 *pShutterCtrl)
{
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines, VMAX;
    UINT32 MaxExposurelineL, MinExposurelineL;
    UINT32 MaxExposurelineM, MinExposurelineM;
    UINT32 MaxExposurelineS, MinExposurelineS;
    FLOAT ExpTime = pExposureTime[0];
    FLOAT ExpTimeUpperLimit, ExpTimeLowerLimit, WorkFLOAT;
    FLOAT CoarseShutter, FineShutter;
    UINT32 Rlm, Rms;
    UINT32 TlTmp, Tl, Tm, Ts, MaxTl, MinTl, MaxTm, MinTm, MaxTs, MinTs; /* exposure time in unit of fine shutter */
    MX00_IMX424_INTEG_REG_s IntegL = {0}, IntegM = {0}, IntegS = {0};
    UINT16 FineShutterD, FineShutterA;

    Rlm = MX00_IMX424_EXP_RATE_L_M;
    Rms = MX00_IMX424_EXP_RATE_M_S;

    VMAX = pModeInfo->NumExposureStepPerFrame / 500U;
    CoarseShutter = pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)VMAX;
    FineShutter = CoarseShutter / 500.0f;

    /* desired shutter time, in unit of fine shutter */
    WorkFLOAT = ExpTime / FineShutter;
    TlTmp = (UINT32)WorkFLOAT;

    /* bounds checking */
#if 0
    ExpTimeUpperLimit = (((FLOAT)Rlm * 7.0f / 8.0f) + 1.0f) / 1000.0f;
    WorkFLOAT = ExpTimeUpperLimit / FineShutter;
    MaxTl = (UINT32)WorkFLOAT;
    MinTl = (Rlm * Rms * 500U);
#else
    MaxExposurelineL = pModeInfo->HdrInfo.ChannelInfo[0].MaxExposureLine;
    MinExposurelineL = pModeInfo->HdrInfo.ChannelInfo[0].MinExposureLine;
    MaxExposurelineM = pModeInfo->HdrInfo.ChannelInfo[1].MaxExposureLine;
    MinExposurelineM = pModeInfo->HdrInfo.ChannelInfo[1].MinExposureLine;
    MaxExposurelineS = pModeInfo->HdrInfo.ChannelInfo[2].MaxExposureLine;
    MinExposurelineS = pModeInfo->HdrInfo.ChannelInfo[2].MinExposureLine;

    ExpTimeUpperLimit = (FLOAT)MaxExposurelineL * CoarseShutter;
    WorkFLOAT = ExpTimeUpperLimit / FineShutter;
    MaxTl = (UINT32)WorkFLOAT;
    ExpTimeLowerLimit = (FLOAT)MinExposurelineL * CoarseShutter;
    WorkFLOAT = ExpTimeLowerLimit / FineShutter;
    MinTl = (UINT32)WorkFLOAT;

    ExpTimeUpperLimit = (FLOAT)MaxExposurelineM * CoarseShutter;
    WorkFLOAT = ExpTimeUpperLimit / FineShutter;
    MaxTm = (UINT32)WorkFLOAT;
    ExpTimeLowerLimit = (FLOAT)MinExposurelineM * CoarseShutter;
    WorkFLOAT = ExpTimeLowerLimit / FineShutter;
    MinTm = (UINT32)WorkFLOAT;

    ExpTimeUpperLimit = (FLOAT)MaxExposurelineS * CoarseShutter;
    WorkFLOAT = ExpTimeUpperLimit / FineShutter;
    MaxTs = (UINT32)WorkFLOAT;
    ExpTimeLowerLimit = (FLOAT)MinExposurelineS * CoarseShutter;
    WorkFLOAT = ExpTimeLowerLimit / FineShutter;
    MinTs = (UINT32)WorkFLOAT;

#endif

    if (TlTmp > MaxTl) {
        TlTmp = MaxTl;
    }

    if (TlTmp < MinTl) {
        TlTmp = MinTl;
    }

    /* calculated shuter time, in unit of fine shutter */
#if 0
    Ts = TlTmp / (Rlm * Rms);
    Tm = Ts * Rms;
    Tl = Tm * Rlm;
    IntegL.Coarse = Tl / 500U;
    IntegL.Fine = (UINT16)(Tl % 500U);
    pShutterCtrl[0] = Tl;
    pActualExpTime[0] = ((FLOAT)IntegL.Coarse * CoarseShutter) + ((FLOAT)IntegL.Fine * FineShutter);

    IntegM.Coarse = Tm / 500U;
    IntegM.Fine = (UINT16)(Tm % 500U);
    pShutterCtrl[1] = Tm;
    pActualExpTime[1] = ((FLOAT)IntegM.Coarse * CoarseShutter) + ((FLOAT)IntegM.Fine * FineShutter);

    IntegS.Coarse = Ts / 500U;
    IntegS.Fine = (UINT16)(Ts % 500U);
    pShutterCtrl[2] = Ts;
    pActualExpTime[2] = ((FLOAT)IntegS.Coarse * CoarseShutter) + ((FLOAT)IntegS.Fine * FineShutter);
#else
    Ts = TlTmp / (Rlm * Rms);
    if(Ts < MinTs) {
        Ts = MinTs;
    }
    if(Ts > MaxTs) {
        Ts = MaxTs;
    }
    Tm = TlTmp / Rlm;

    if(Tm < MinTm) {
        Tm = MinTm;
    }
    if(Tm > MaxTm) {
        Tm = MaxTm;
    }
    Tl = TlTmp;

    /*
        IntegL.Coarse = Tl / 500U;
        pShutterCtrl[0] = Tl;
        pActualExpTime[0] = ((FLOAT)IntegL.Coarse * CoarseShutter);

        IntegM.Coarse = Tm / 500U;
        pShutterCtrl[1] = Tm;
        pActualExpTime[1] = ((FLOAT)IntegM.Coarse * CoarseShutter);

        IntegS.Coarse = Ts / 500U;
        pShutterCtrl[2] = Ts;
        pActualExpTime[2] = ((FLOAT)IntegS.Coarse * CoarseShutter);
    */
    IntegL.Coarse = Tl / 500U;
    IntegL.Fine = (UINT16)(Tl % 500U);
    FineShutterD = (UINT16)IntegL.Fine;
    MX00_IMX424_CheckFineShutter(&FineShutterD, &FineShutterA);
    IntegL.Fine = (UINT16)FineShutterA;
    pShutterCtrl[0] = (IntegL.Coarse * 500U) + IntegL.Fine;//Tl;
    pActualExpTime[0] = ((FLOAT)IntegL.Coarse * CoarseShutter) + ((FLOAT)IntegL.Fine * FineShutter);

    IntegM.Coarse = Tm / 500U;
    IntegM.Fine = (UINT16)(Tm % 500U);
    FineShutterD = (UINT16)IntegM.Fine;
    MX00_IMX424_CheckFineShutter(&FineShutterD, &FineShutterA);
    IntegM.Fine = (UINT16)FineShutterA;
    pShutterCtrl[1] = (IntegM.Coarse * 500U) + IntegM.Fine;//Tm;
    pActualExpTime[1] = ((FLOAT)IntegM.Coarse * CoarseShutter) + ((FLOAT)IntegM.Fine * FineShutter);

    IntegS.Coarse = Ts / 500U;
    IntegS.Fine = (UINT16)(Ts % 500U);
    FineShutterD = (UINT16)IntegS.Fine;
    MX00_IMX424_CheckFineShutter(&FineShutterD, &FineShutterA);
    IntegS.Fine = (UINT16)FineShutterA;
    pShutterCtrl[2] = (IntegS.Coarse * 500U) + IntegS.Fine;//Ts;
    pActualExpTime[2] = ((FLOAT)IntegS.Coarse * CoarseShutter) + ((FLOAT)IntegS.Fine * FineShutter);
#endif

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_ConvertShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      Chan:                 Vin ID and sensor ID
 *      pExposureTime:        Desired Exposure time / shutter speed
 *
 *  @Output     ::
 *      pActualExposureTime:  Achievable exposure time
 *      pShutterCtrl:         Shutter control for achievable exposure time
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX424_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API;
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            ConvertHdrShutterSpeed(pModeInfo, pDesiredExposureTime, pActualExposureTime, pShutterCtrl);
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_SetAnalogGainCtrl
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
static UINT32 MX00_IMX424_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;
#if !defined(CONFIG_IMX424_RCCB_PSLIN_DISABLE)
    UINT32 Gain_thr = 30U; //20210708
    UINT8 WData[4];
#endif
    if ((pChan == NULL) || (pAnalogGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API; /* TBD */
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX424_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);
            //AmbaPrint_PrintUInt5("Gain = %d", pAnalogGainCtrl[0], 0U, 0U, 0U, 0U);
#if !defined(CONFIG_IMX424_RCCB_PSLIN_DISABLE)
            if (pAnalogGainCtrl[0] > Gain_thr) {//20210615
                //AmbaPrint_PrintUInt5( "Long~~~~~~~~~~", 0U, 0U, 0U, 0U, 0U);

                /* LONG exp only */
                WData[0] = (UINT8)(0xFFU);
                WData[1] = (UINT8)(0x0FU);
                WData[2] = (UINT8)(0xFFU);
                WData[3] = (UINT8)(0x0FU);
                RetVal |= RegWrite(pChan, 0x814A, WData, 4);

                WData[0] = (UINT8)(0xFFU);
                WData[1] = (UINT8)(0x0FU);
                WData[2] = (UINT8)(0xFFU);
                WData[3] = (UINT8)(0x0FU);
                RetVal |= RegWrite(pChan, 0x815A, WData, 4);
            } else {//20210615
                //AmbaPrint_PrintUInt5("original", 0U, 0U, 0U, 0U, 0U);
                /* Original 3-exp */
                WData[0] = (UINT8)(0x6CU);
                WData[1] = (UINT8)(0x07U);
                WData[2] = (UINT8)(0xA0U);
                WData[3] = (UINT8)(0x0FU);
                RetVal |= RegWrite(pChan, 0x814A, WData, 4);

                WData[0] = (UINT8)(0x98U);
                WData[1] = (UINT8)(0x08U);
                WData[2] = (UINT8)(0xA0U);
                WData[3] = (UINT8)(0x0FU);
                RetVal |= RegWrite(pChan, 0x815A, WData, 4);
            }//20210615
#endif
            /* Update current analog gain control */
            for (i = 0U; i < MX00_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX424Ctrl[pChan->VinID].CurrentAEInfo[i].AgcCtrl = pAnalogGainCtrl[0];
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_SetWbGainCtrl
 *
 *  @Description:: Set white balance gain control
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pWbGainCtrl:     Wb gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_IMX424_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;

    if ((pChan == NULL) || (pWbGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API; /* TBD */
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX424_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX00_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_IMX424Ctrl[pChan->VinID].CurrentAEInfo[i].WbCtrl = pWbGainCtrl[0];
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_SetShutterCtrl
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
static UINT32 MX00_IMX424_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = SENSOR_ERR_INVALID_API; /* TBD */
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_IMX424_SetShutterReg(pChan, pShutterCtrl);

            /* Update current shutter control */
            for (i = 0U; i < MX00_IMX424_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_IMX424Ctrl[pChan->VinID].CurrentAEInfo[i].ShutterCtrl[k] = pShutterCtrl[k];
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
 *  @RoutineName:: MX00_IMX424_SetSlowShutterCtrl
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
static UINT32 MX00_IMX424_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (SlowShutterCtrl < 1U)) {
        RetVal = SENSOR_ERR_ARG;
    } else if (MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        // RetVal = SENSOR_ERR_INVALID_API;
        RetVal = MX00_IMX424_SetSlowShutterReg(SlowShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX424_Config
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
static UINT32 MX00_IMX424_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0x00000fU,  /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0x00f000U,  /* SD_LVDS_12~15 */
        [AMBA_VIN_CHANNEL2] = 0x0000f0U,  /* SD_LVDS_4~7 */
        [AMBA_VIN_CHANNEL3] = 0x000f00U,  /* SD_LVDS_8~11 */
        [AMBA_VIN_CHANNEL4] = 0x0f0000U,  /* SD_LVDS_16~19 */
        [AMBA_VIN_CHANNEL5] = 0xf00000U,  /* SD_LVDS_20~23 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s MX00_IMX424PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_IMX424Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Add delay time to wait MAX9296/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power off", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power on", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(3);

    if (ModeID >= MX00_IMX424_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX00_IMX424] Config Mode: %d, SensorID: %d", ModeID, pChan->SensorID, 0U, 0U, 0U);

        /* update status */
        MX00_IMX424_PrepareModeInfo(pChan, pMode, pModeInfo);
#ifdef MX_IMX424_IN_SLAVE_MODE
        (void)AmbaVIN_MasterSyncDisable(MX00_IMX424MasterSyncChannel[pChan->VinID]);
#endif
        /* Adjust mipi-phy parameters */
        MX00_IMX424PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX00_IMX424PadConfig);
        /* After reset VIN, set MAX9296A PWDNB high */
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB low", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(3);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB high", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_9296_Init(pChan->VinID, MX00_IMX424_GetEnabledLinkID(pChan->SensorID));

        MX00_IMX424_ConfigSerDes(pChan, (UINT16)ModeID, pModeInfo);

        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX00_IMX424_SENSOR_ID_CHAN_A) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
        }
        if ((pChan->SensorID & MX00_IMX424_SENSOR_ID_CHAN_B) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
        }

        RetVal |= MX00_IMX424_ResetSensor(pChan);
        (void) AmbaKAL_TaskSleep(100);

        /* program sensor */
        RetVal |= MX00_IMX424_ChangeReadoutMode(pChan, ModeID);
#ifdef MX_IMX424_IN_SLAVE_MODE
        {
            UINT8 Data;
            Data = 0xC0U;
            RetVal |= RegWrite(pChan, 0x0091U, &Data, 1U);

            //Set MASK_FRMNUM_INIT1=1,to avoid invalid first 2 frame of IMX424 when using External pulse-based Sync
            Data = 0x01U;
            RetVal |= RegWrite(pChan, 0x0003U, &Data, 1U);
        }
#endif
        RetVal |= MX00_IMX424_SetStandbyOff(pChan);
#ifdef MX_IMX424_IN_SLAVE_MODE
        RetVal |= MX00_IMX424_ConfigMasterSync(pChan, ModeID);
#endif
        /* config vin */
        RetVal |= MX00_IMX424_ConfigVin(pChan, pModeInfo);
    }

    return RetVal;
}

#if 0
static INT32 MX00_IMX424_ConfigPost(UINT32 *pVinID)
{
    MX00_IMX424_SetStandbyOff();

    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX00_IMX424Obj = {
    .SensorName             = "IMX424",
    .SerdesName             = "MAX9295_9296",
    .Init                   = MX00_IMX424_Init,
    .Enable                 = MX00_IMX424_Enable,
    .Disable                = MX00_IMX424_Disable,
    .Config                 = MX00_IMX424_Config,
    .GetStatus              = MX00_IMX424_GetStatus,
    .GetModeInfo            = MX00_IMX424_GetModeInfo,
    .GetDeviceInfo          = MX00_IMX424_GetDeviceInfo,
    .GetHdrInfo             = MX00_IMX424_GetHdrInfo,
    .GetCurrentGainFactor   = MX00_IMX424_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX00_IMX424_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX00_IMX424_ConvertGainFactor,
    .ConvertShutterSpeed    = MX00_IMX424_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX00_IMX424_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = NULL,
    .SetWbGainCtrl          = MX00_IMX424_SetWbGainCtrl,
    .SetShutterCtrl         = MX00_IMX424_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX00_IMX424_SetSlowShutterCtrl,

    .RegisterRead           = MX00_IMX424_RegisterRead,
    .RegisterWrite          = MX00_IMX424_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};

