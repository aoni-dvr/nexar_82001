/**
 *  @file AmbaSensor_MX00_OV2312.c
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
 *  @details Control APIs of MX00_OV2312 CMOS sensor with MIPI/DVP interface
 *
 */

#include <AmbaWrap.h>

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_OV2312_RGBIR.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSbrg_Max9295_9296.h"

#include "bsp.h"

#define MX00_OV2312_IN_SLAVE_MODE

/*-----------------------------------------------------------------------------------------------*\
 * MX00_OV2312_MIPI sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX00_OV2312_CTRL_s MX00_OV2312_Ctrl = {0};
static UINT32 GShutterCtrl = 0;
//Please noted VIN0 supports up to 8lane, PIP supports up to 4lane(shared with VIN0), PIP2 support up to 2lane(shared with PIP)
static MAX9295_9296_SERDES_CONFIG_s MX00_OV2312SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_OV2312_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] =  (UINT8)(MX00_OV2312_SENSOR_I2C_SLAVE_ADDR + 0x10U),
            [1] =  (UINT8)(MX00_OV2312_SENSOR_I2C_SLAVE_ADDR + 0x12U),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [1] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_OV2312_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] =  (UINT8)(MX00_OV2312_SENSOR_I2C_SLAVE_ADDR + 0x14U),
            [1] =  (UINT8)(MX00_OV2312_SENSOR_I2C_SLAVE_ADDR + 0x16U),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [2] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_OV2312_SENSOR_I2C_SLAVE_ADDR,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_OV2312_SENSOR_I2C_SLAVE_ADDR + 0x20U),
            [1] = (UINT8)(MX00_OV2312_SENSOR_I2C_SLAVE_ADDR + 0x22U),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    //[3] = {
    //    .EnabledLinkID   = 0x01,
    //    .SensorSlaveID   = MX00_OV2312_SENSOR_I2C_SLAVE_ADDR,
    //    .SensorAliasID   = {
    //        [0] = MX00_OV2312_SENSOR_I2C_SLAVE_ADDR + 0x24,
    //        [1] = MX00_OV2312_SENSOR_I2C_SLAVE_ADDR + 0x26,
    //    },
    //    .NumDataBits     = 12,
    //    .CSIRxLaneNum    = 4,
    //    .CSITxLaneNum    = {4, 4},
    //    .CSITxSpeed      = {10, 10},
    //},
};

static UINT32 MX00_OV2312I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_MAXIM_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_MAXIM_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_MAXIM_I2C_CHANNEL_PIP2,
    //[AMBA_VIN_CHANNEL3] = AMBA_MAXIM_I2C_CHANNEL_PIP3,
    //[AMBA_VIN_CHANNEL4] = AMBA_MAXIM_I2C_CHANNEL_PIP4,
    //[AMBA_VIN_CHANNEL5] = AMBA_MAXIM_I2C_CHANNEL_PIP5,
};

/*static UINT16 MX00_OV2312_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < MX00_OV2312_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9716_GetNumActiveSensor
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
static UINT32 MX00_OV2312_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < MX00_OV2312_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            SensorNum ++;
        }
    }

    if(SensorNum <= 1U) {
        SensorNum = 1U;
    }

    return SensorNum;
}


static UINT16 MX00_OV2312_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 i, EnabledLinkID = 0U;

    for (i = 0U; i < MX00_OV2312_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_PrepareModeInfo
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
static UINT32 FLL;
static void MX00_OV2312_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &MX00_OV2312_InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &MX00_OV2312_OutputInfo[ModeID];
    const MX00_OV2312_FRAME_TIMING_s        *pFrameTiming   = &MX00_OV2312_ModeInfoList[ModeID].FrameTiming;
    UINT32 SensorCount;
    UINT32 U32RVal = 0;

    SensorCount = MX00_OV2312_GetNumActiveSensor(pChan->SensorID);

    //AmbaPrint("Sensor Mode:%d, Input Mode:%d, Output Mode:%d",SensorMode,InputMode,OutputMode);

    //pModeInfo->LineLengthPck = (UINT32) ((double)(pOutputInfo->DataRate * pOutputInfo->NumDataBits * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits) *
    //                                        pFrameTiming->FrameRate.NumUnitsInTick / pFrameTiming->FrameRate.TimeScale / pFrameTiming->FrameLengthLines);
    pModeInfo->LineLengthPck            = pFrameTiming->LineLengthPck;
    pModeInfo->FrameLengthLines         = pFrameTiming->FrameLengthLines * SensorCount;
    FLL = pFrameTiming->FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines;
    pModeInfo->InputClk                 = pFrameTiming->InputClk;
    //pModeInfo->RowTime                  = (FLOAT)(((DOUBLE) pFrameTiming->LineLengthPck / (DOUBLE)pOutputInfo->DataRate) / (DOUBLE)SensorCount);
    pModeInfo->RowTime                  = MX00_OV2312_ModeInfoList[ModeID].RowTime;
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX00_OV2312_HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 2U;
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_ConfigSerDes
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
static void MX00_OV2312_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s *pImgInfo = &pModeInfo->OutputInfo;
    const MX00_OV2312_SENSOR_INFO_s *pSensorInfo = &MX00_OV2312SensorInfo[SensorMode];
    MAX9295_9296_SERDES_CONFIG_s *pSerdesCfg = &MX00_OV2312SerdesConfig[pChan->VinID];
    DOUBLE FloorVal;
    UINT32 U32RVal = 0;

    pSerdesCfg->EnabledLinkID = MX00_OV2312_GetEnabledLinkID(pChan->SensorID);
    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;

    U32RVal |= AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal);
    pSerdesCfg->CSITxSpeed[0] = (UINT8)(FloorVal);

    (void) Max9295_9296_Config(pChan->VinID, &MX00_OV2312SerdesConfig[pChan->VinID]);

    if(U32RVal != 0U) {
        /* MisraC */
    }
}

#ifdef MX00_OV2312_IN_SLAVE_MODE
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_ConfigVinMasterSync
 *
 *  @Description:: Configure VIN master sync generation
 *
 *  @Input      ::
 *      Chan:       Vin ID and Sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2312_ConfigVinMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    DOUBLE PeriodInDb;
    static const UINT32 MX00_OV2312MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
        [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
        [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_MSYNC_CHANNEL_PIP2,
        //[AMBA_VIN_CHANNEL3] = AMBA_SENSOR_MSYNC_CHANNEL_PIP3,
        //[AMBA_VIN_CHANNEL4] = AMBA_SENSOR_MSYNC_CHANNEL_PIP4,
        //[AMBA_VIN_CHANNEL5] = AMBA_SENSOR_MSYNC_CHANNEL_PIP5,
    };

    //const OV9716_MODE_INFO_s *pModeInfo      = &OV9716ModeInfoList[ModeID];
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2312_Ctrl.Status.ModeInfo;
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg = {0};
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_floor(((DOUBLE)pModeInfo->FrameRate.NumUnitsInTick * ((DOUBLE) pModeInfo->InputClk / (DOUBLE)pModeInfo->FrameRate.TimeScale)) + 0.5, &PeriodInDb);
    MasterSyncCfg.RefClk             = pModeInfo->InputClk;
    MasterSyncCfg.HSync.Period       = (UINT32) PeriodInDb;
    MasterSyncCfg.HSync.PulseWidth   = 8U;
    MasterSyncCfg.HSync.Polarity     = 1U;
    MasterSyncCfg.VSync.Period       = 1U;
    MasterSyncCfg.VSync.PulseWidth   = 1000U;
    MasterSyncCfg.VSync.Polarity     = 1U;
    MasterSyncCfg.HSyncDelayCycles     = 0U;
    MasterSyncCfg.VSyncDelayCycles     = 0U;
    MasterSyncCfg.ToggleHsyncInVblank  = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(MX00_OV2312MasterSyncChannel[pChan->VinID], &MasterSyncCfg);

    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}
#endif
static void MX00_OV2312_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < MX00_OV2312_NUM_MODE; i++) {
        MX00_OV2312_ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)MX00_OV2312SensorInfo[i].LineLengthPck /
                                              ((DOUBLE)MX00_OV2312SensorInfo[i].DataRate *
                                               (DOUBLE)MX00_OV2312SensorInfo[i].NumDataLanes /
                                               (DOUBLE)MX00_OV2312SensorInfo[i].NumDataBits));
        MX00_OV2312_ModeInfoList[i].PixelRate = (FLOAT)((DOUBLE)MX00_OV2312_OutputInfo[i].DataRate *
                                                (DOUBLE)MX00_OV2312_OutputInfo[i].NumDataLanes /
                                                (DOUBLE)MX00_OV2312_OutputInfo[i].NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_ConfigVin
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
static UINT32 MX00_OV2312_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX00_OV2312_VinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace = AMBA_VIN_COLOR_SPACE_RGB,
            .BayerPattern = AMBA_SENSOR_BAYER_PATTERN_BG,
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
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &MX00_OV2312_VinConfig;
    UINT32 U32RVal = 0;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;


    U32RVal |= AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return AmbaVIN_MipiConfig(VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_HardwareReset
 *
 *  @Description:: Reset MX00_OV2312 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2312_HardwareReset(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan->SensorID & MX00_OV2312_SENSOR_ID_CHAN_0) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    if ((pChan->SensorID & MX00_OV2312_SENSOR_ID_CHAN_1) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
    if ((pChan->SensorID & MX00_OV2312_SENSOR_ID_CHAN_0) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    if ((pChan->SensorID & MX00_OV2312_SENSOR_ID_CHAN_1) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start >= 20us*/

    if (RetVal != MAX9295_9296_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }

    return RetVal;
}


static UINT32 MX00_OV2312_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxDataBuf[2];
    UINT32 TxSize;

    if ((pChan == NULL) || ((pChan->SensorID != MX00_OV2312_SENSOR_ID_CHAN_0) && (pChan->SensorID != MX00_OV2312_SENSOR_ID_CHAN_1))) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        I2cTxConfig.DataSize = 2U;
        I2cTxConfig.pDataBuf = TxDataBuf;
        TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8);
        TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);

        I2cRxConfig.DataSize = 1U;
        I2cRxConfig.pDataBuf = pRxData;

        if (pChan->SensorID == MX00_OV2312_SENSOR_ID_CHAN_0) {
            I2cTxConfig.SlaveAddr = MX00_OV2312SerdesConfig[pChan->VinID].SensorAliasID[0];
            I2cRxConfig.SlaveAddr = (MX00_OV2312SerdesConfig[pChan->VinID].SensorAliasID[0] | (UINT32)0x1U);
        } else {
            /* (pChan->SensorID == MX00_OV2312_SENSOR_ID_CHAN_1) */
            I2cTxConfig.SlaveAddr = MX00_OV2312SerdesConfig[pChan->VinID].SensorAliasID[1];
            I2cRxConfig.SlaveAddr = (MX00_OV2312SerdesConfig[pChan->VinID].SensorAliasID[1] | (UINT32)0x1U);
        }

        RetVal = AmbaI2C_MasterReadAfterWrite(MX00_OV2312I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U,
                                              &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);
        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C(Read) does not work (SlaveID=0x%02x)!!!!!", I2cTxConfig.SlaveAddr, 0, 0, 0, 0);
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_RegRW
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
static UINT32 MX00_OV2312_RegRW(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr,const UINT8 *pTxData, UINT32 Size)
{
    UINT32 i, k;
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX00_OV2312_I2C_WR_BUF_SIZE + 2U];
    UINT32 TxSize;

    if ((pChan == NULL) || ((pChan->SensorID & (MX00_OV2312_SENSOR_ID_CHAN_0 | MX00_OV2312_SENSOR_ID_CHAN_1)) == 0x0U)
        || (Size > MX00_OV2312_I2C_WR_BUF_SIZE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        I2cConfig.DataSize  = Size + 2U;
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8);
        TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);

        for (i = 0; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

        if (pChan->SensorID == (MX00_OV2312_SENSOR_ID_CHAN_0 | MX00_OV2312_SENSOR_ID_CHAN_1)) { /* broadcast to all sensors */
            I2cConfig.SlaveAddr = MX00_OV2312_SENSOR_I2C_SLAVE_ADDR;
            RetVal |= AmbaI2C_MasterWrite(MX00_OV2312I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);

            if (RetVal != I2C_ERR_NONE) {
                RetVal = SENSOR_ERR_COMMUNICATE;
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "I2C does not work chan %u (SlaveID=0x%02x)!!!!!", MX00_OV2312I2cChannel[pChan->VinID], I2cConfig.SlaveAddr, 0, 0, 0);
            }
        } else {
            for (k = 0; k < MX00_OV2312_NUM_MAX_SENSOR_COUNT; k++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (k + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_OV2312SerdesConfig[pChan->VinID].SensorAliasID[k];
                    RetVal |= AmbaI2C_MasterWrite(MX00_OV2312I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);
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
 *  @RoutineName:: MX00_OV2312_RegisterWrite
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
static UINT32 MX00_OV2312_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)Data;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OV2312_RegRW(pChan, Addr, &WData, 1U);
    }

    return RetVal;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_RegisterRead
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
static UINT32 MX00_OV2312_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData = 0U;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OV2312_RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_SetAnalogGainReg
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
static UINT32 MX00_OV2312_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AGC)
{
    UINT32 RetVal = SENSOR_ERR_NONE;    //AGC = 0x10;    // real gain linear, 0x10 = 1x AGC/0x10 = x gain
    UINT32 HB,LB;
    FLOAT FloatTmp = (FLOAT)AGC / 16.0f;
    UINT32 AGC_tmp;

    /*if(RetVal != MX00_OV2312_SetStrobeReg(pChan, AGC)){
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SetStrobeReg Error", 0, 0, 0, 0, 0);
    }*/

    if(FloatTmp > 15.9375f) {    // max
        //AGC_tmp = (UINT32)(15.9375f * 16.0f);                // 0.9375 x 16
        FloatTmp = 15.9375f * 16.0f;
        AGC_tmp = (UINT32)FloatTmp;
    } else {
        AGC_tmp = AGC;
    }

    LB = (AGC_tmp & 0x0000000fU) << 4U;
    HB = (AGC_tmp & 0x000000f0U) >> 4U;

    RetVal |= MX00_OV2312_RegisterWrite(pChan, MX00_OV2312_GAIN_H, (UINT16)HB);
    RetVal |= MX00_OV2312_RegisterWrite(pChan, MX00_OV2312_GAIN_L, (UINT16)LB);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_SetShutterReg
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
static UINT32 MX00_OV2312_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8  Data[2];

    Data[0] = (UINT8)((ShutterCtrl >> 8U) & 0xffU);
    Data[1] = (UINT8)(ShutterCtrl & 0xffU);
    GShutterCtrl = ShutterCtrl;
    RetVal |= MX00_OV2312_RegRW(pChan,MX00_OV2312_EXPO_H,Data,2U);
    return RetVal;
}

static UINT32 MX00_OV2312_SetStrobeReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    static UINT32 GroupId = 0;
    UINT8  StrobeWidthData[2];
    UINT8  StrobeStartPointData[2];
    UINT8  Grouphold;
    UINT32 StrobeStartPoint = (FLL - GShutterCtrl) - 7U;
    UINT32 StrobeLength = ShutterCtrl;
    if(StrobeLength > GShutterCtrl) {
        StrobeLength = GShutterCtrl;
    } else {
        ///MisraC
    }

    StrobeWidthData[0] = (UINT8)((StrobeLength >> 8U) & 0xffU);
    StrobeWidthData[1] = (UINT8)(StrobeLength & 0xffU);

    StrobeStartPointData[0] = (UINT8)((StrobeStartPoint >> 8U) & 0xffU);
    StrobeStartPointData[1] = (UINT8)(StrobeStartPoint & 0xffU);

    Grouphold = (UINT8)GroupId;
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3208,Grouphold); //group hold for IR LED control

    if(StrobeLength == 0U) {
        RetVal |= MX00_OV2312_RegisterWrite(pChan, 0x3006, 0x0);
    } else {
        RetVal |= MX00_OV2312_RegisterWrite(pChan, 0x3006, 0x8);
        RetVal |= MX00_OV2312_RegRW(pChan, MX00_OV2312_IRLED_STROBE_WIDTH, StrobeWidthData, 2U);
        RetVal |= MX00_OV2312_RegRW(pChan, MX00_OV2312_IRLED_STROBE_START, StrobeStartPointData,2U);
    }
    Grouphold = (UINT8)(0x10U | GroupId);
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3208,Grouphold);
    Grouphold = (UINT8)(0xA0U | GroupId);
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3208,Grouphold);
    GroupId = (GroupId + 1U) % 2U;
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_MIPI_SetSlowShutterReg
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
#if 0
static UINT32 MX00_OV2312_MIPI_SetSlowShutterReg(UINT32 IntegrationPeriodInFrame)
{
    UINT32 TargetFrameLengthLines;
    UINT32 ModeID = MX00_OV2312_Ctrl.Status.ModeInfo.Config.ModeID;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (IntegrationPeriodInFrame < 1U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TargetFrameLengthLines = MX00_OV2312_ModeInfoList[ModeID].FrameTiming.FrameLengthLines * IntegrationPeriodInFrame;

        if (TargetFrameLengthLines > 0x3ffffU) {
            TargetFrameLengthLines = 0x3ffffU;
        }

        RetVal |= MX00_OV2312_MIPI_RegRW(MX00_OV2312_MIPI_VMAX_LSB, (UINT8)(TargetFrameLengthLines & 0xffU));
        RetVal |= MX00_OV2312_MIPI_RegRW(MX00_OV2312_MIPI_VMAX_MSB, (UINT8)((TargetFrameLengthLines >> 8U) & 0xffU));
        RetVal |= MX00_OV2312_MIPI_RegRW(MX00_OV2312_MIPI_VMAX_HSB, (UINT8)((TargetFrameLengthLines >> 16U) & 0x03U));

        /* Update frame rate information */
        MX00_OV2312_Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        MX00_OV2312_Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        MX00_OV2312_Ctrl.Status.ModeInfo.FrameRate.TimeScale = MX00_OV2312_ModeInfoList[ModeID].FrameTiming.FrameRate.TimeScale / IntegrationPeriodInFrame;
    }

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_MIPI_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2312_MIPI_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= MX00_OV2312_RegisterWrite(pChan, MX00_OV2312_SC_MODE_SELECT, 0x00U);
//    AmbaPrint("[MX00_OV2312_MIPI] MX00_OV2312_MIPI_SetStandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2312_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= MX00_OV2312_RegisterWrite(pChan, MX00_OV2312_SC_MODE_SELECT, 0x01U);
    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * MX00_OV2312_Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / MX00_OV2312_Ctrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[MX00_OV2312_MIPI] MX00_OV2312_SetStandbyOff");
}
#ifdef MX00_OV2312_IN_SLAVE_MODE
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_SetSlaveMode
 *
 *  @Description:: Configure sensor to sensor slave mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2312_SetSlaveMode(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3006, 0x00);// disable FSIN output
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x38b3, 0x07);//
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3885, 0x07);//
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x382b, 0x5a);//
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3670, 0x68);//
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3823, 0x30);// [5] ext_vs_en ;//[4] r_init_man_en
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3824, 0x00);// sclk -> cs counter reset value at vs_ext
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3825, 0x08);// set it to 8
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3826, 0x05);// sclk -> r counter reset value at vs_ext
    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3827, 0x8a);// vts = 'h5c2 as default, set r counter to vts-4
    return RetVal;

}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_ChangeReadoutMode
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
static UINT32 MX00_OV2312_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
    // init-sequence
    for (i = 0; i < MX00_OV2312_NUM_MODE_REG; i ++) {
        RetVal |= MX00_OV2312_RegisterWrite(pChan, MX00_OV2312_RegTable[i].Addr, MX00_OV2312_RegTable[i].Data);
    }

    RetVal |= MX00_OV2312_RegisterWrite(pChan,0x38b1, 0x00); // To prevent half image brightness difference at low light

    if (ModeID == MX00_OV2312_1280_800_A30P) {
        for (i = 0; i < MX00_OV2312_NUM_HD_MODE_REG; i ++) {
            RetVal |= MX00_OV2312_RegisterWrite(pChan, MX00_OV2312_HDRegTable[i].Addr, MX00_OV2312_HDRegTable[i].Data);
        }
    } else if((ModeID == MX00_OV2312_1280_720_A60P) || (ModeID == MX00_OV2312_1600_1300_A60P)) {
        RetVal |= MX00_OV2312_RegisterWrite(pChan,0x380c, 0x03 );
        RetVal |= MX00_OV2312_RegisterWrite(pChan,0x380d, 0xa8 );
        RetVal |= MX00_OV2312_RegisterWrite(pChan,0x380e, 0x05 );
        RetVal |= MX00_OV2312_RegisterWrite(pChan,0x380f, 0x8e );
        if(ModeID == MX00_OV2312_1280_720_A60P) {
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3800, 0x00 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3801, 0x00 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3802, 0x00 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3803, 0x00 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3804, 0x06 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3805, 0x4f );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3806, 0x05 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3807, 0x23 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3808, 0x05 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x3809, 0x00 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x380a, 0x02 );
            RetVal |= MX00_OV2312_RegisterWrite(pChan,0x380b, 0xd0 );
        }
    } else {
        ///
    }
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentAgcCtrl[0] = 1;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentAgcCtrl[1] = 2;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentDgcCtrl[0] = 0x15a;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentDgcCtrl[1] = 0x200;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentShutterCtrl[0] = 0x448;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentShutterCtrl[1] = 0x405;

    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentWbCtrl[0].R  = 0x100;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentWbCtrl[0].Gr = 0x100;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentWbCtrl[0].Gb = 0x100;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentWbCtrl[0].B  = 0x100;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentWbCtrl[1].R  = 0x100;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentWbCtrl[1].Gr = 0x100;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentWbCtrl[1].Gb = 0x100;
    MX00_OV2312_Ctrl.CurrentAEInfo.CurrentWbCtrl[1].B  = 0x100;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_Init
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
static UINT32 MX00_OV2312_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

//    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "OV2312 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, MX00_OV2312SensorInfo[0U].InputClk);

        /* Set MAX9296A PWDNB high */
        /*RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(1);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_9296_Init(pChan->VinID, MX00_OV2312_GetEnabledLinkID(pChan->SensorID));*/
        MX00_OV2312_PreCalculateModeInfo();

    }
    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_Enable
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
static UINT32 MX00_OV2312_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        // RetVal = MX00_OV2312_HardwareReset(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_Disable
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
static UINT32 MX00_OV2312_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OV2312_MIPI_SetStandbyOn(pChan);
    }

    //AmbaPrint("[MX00_OV2312_MIPI] MX00_OV2312_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_GetStatus
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
static UINT32 MX00_OV2312_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pStatus, &MX00_OV2312_Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_GetModeInfo
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
static UINT32 MX00_OV2312_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = MX00_OV2312_Ctrl.Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if (Config.ModeID >= MX00_OV2312_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            MX00_OV2312_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_GetDeviceInfo
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
static UINT32 MX00_OV2312_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &MX00_OV2312_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_GetHdrInfo
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
static UINT32 MX00_OV2312_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pHdrInfo, &MX00_OV2312_Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_GetCurrentGainFactor
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
static UINT32 MX00_OV2312_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 i;
    UINT32 GainCtrl;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        GainCtrl = MX00_OV2312_Ctrl.CurrentAEInfo.CurrentAgcCtrl[0];
        pGainFactor[0]  =  ((FLOAT)GainCtrl)/16.0f;
        if (MX00_OV2312_Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 1U; i < (MX00_OV2312_Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                pGainFactor[i] = pGainFactor[0];
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_GetCurrentShutterSpeed
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
static UINT32 MX00_OV2312_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2312_Ctrl.Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = pModeInfo->RowTime * (FLOAT)MX00_OV2312_Ctrl.CurrentAEInfo.CurrentShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = pModeInfo->RowTime * (FLOAT)MX00_OV2312_Ctrl.CurrentAEInfo.CurrentShutterCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_ConvertGainFactor
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
static UINT32 MX00_OV2312_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    //UINT8 Agc = 1;
    // UINT8 Agc_f = 0;
    DOUBLE DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
    DOUBLE GainCtrlInDb;
    UINT32 U32RVal = 0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (DesiredFactor < 1.0f) {    // min
            DesiredFactor = 1.0f;
        } else if(DesiredFactor > 256.0f) { // max
            DesiredFactor = 256.0f;
        } else {
            //
        }


        //   else if(DesiredFactor > 15.9375f) {    // max
        //   DesiredFactor = 15.9375f;                // 0.9375 x 16
        //   } else {}

        U32RVal |= AmbaWrap_floor((DesiredFactor*16.0f), &GainCtrlInDb);
        pGainCtrl->AnalogGain[0] = (UINT32)GainCtrlInDb;
        pGainCtrl->DigitalGain[0] = 1;

        pActualFactor->Gain[0]  = ((FLOAT)(pGainCtrl->AnalogGain[0]))/16.0f;
    }
    if(U32RVal != 0U) {
        /* MisraC */
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
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl, UINT8 Strobe)
{
    DOUBLE ShutterCtrlInDb;
    //UINT32 ExposureFrames = 0;
    UINT32 ExposureTimeMaxMargin;
    UINT32 ShutterCtrl;
    UINT32 U32RVal = 0;

    U32RVal |= AmbaWrap_floor(((DOUBLE)ExposureTime / (DOUBLE)pModeInfo->RowTime), &ShutterCtrlInDb);
    ShutterCtrl = (UINT32) ((UINT64) ((FLOAT) ShutterCtrlInDb));

    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        /* For none-HDR mode */
    } else {
        //ExposureFrames = (ShutterCtrl / pModeInfo->NumExposureStepPerFrame);
        //ExposureFrames = (ShutterCtrl % pModeInfo->NumExposureStepPerFrame) ? ExposureFrames + 1 : ExposureFrames;
        ExposureTimeMaxMargin =  pModeInfo->NumExposureStepPerFrame - 12U;
        if (ShutterCtrl >= ExposureTimeMaxMargin) { /* Exposure lines needs to be smaller than VTS - 12 */
            *pShutterCtrl = ExposureTimeMaxMargin;
        } else if ((ShutterCtrl <= 1U) && (Strobe != 1U)) {             /* Exposure lines needs to be bigger 1 */
            *pShutterCtrl = 1U;
        } else {
            *pShutterCtrl = ShutterCtrl;
        }
    }
    *pActualExptime = (FLOAT) ShutterCtrl * pModeInfo->RowTime;

    if(U32RVal != 0U) {
        /* MisraC */
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_ConvertShutterSpeed
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
static UINT32 MX00_OV2312_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2312_Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                ConvertShutterSpeed(pModeInfo, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i], 0U);
            }
        } else {
            ConvertShutterSpeed(pModeInfo, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0],0U);
        }
    }

    return RetVal;
}


static UINT32 MX00_OV2312_ConvertStrobeWidth(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredWidth, FLOAT *pActualWidth, UINT32 *pStrobeCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2312_Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pStrobeCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                ConvertShutterSpeed(pModeInfo, pDesiredWidth[i], &pActualWidth[i], &pStrobeCtrl[i],1U);
            }
        } else {
            ConvertShutterSpeed(pModeInfo, pDesiredWidth[0], &pActualWidth[0], &pStrobeCtrl[0],1U);
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_SetAnalogGainCtrl
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
static UINT32 MX00_OV2312_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OV2312_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

        /* Update current AGC control */
        MX00_OV2312_Ctrl.CurrentAEInfo.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_SetAnalogGainCtrl
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
/*
static UINT32 MX00_OV2312_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    return RetVal;
}
*/
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_SetShutterCtrl
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
static UINT32 MX00_OV2312_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2312_Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {

            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                /* Update current shutter control */
                MX00_OV2312_Ctrl.CurrentAEInfo.CurrentShutterCtrl[i] = pShutterCtrl[i];
            }
        } else {
            RetVal = MX00_OV2312_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            MX00_OV2312_Ctrl.CurrentAEInfo.CurrentShutterCtrl[0] = pShutterCtrl[0];
        }


        //    SENSOR_DBG_SHR("[Vin%d/Sensor%d][MX00_OV2312_MIPI][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
        //                      pChan->VinID, pChan->SensorID,  *pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
    }
    return RetVal;
}


static UINT32 MX00_OV2312_SetStrobeCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pStrobeCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OV2312_Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pStrobeCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {

            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                /* Update current shutter control */
                //MX00_OV2312_Ctrl.CurrentAEInfo.CurrentShutterCtrl[i] = pStrobeCtrl[i];
            }
        } else {
            RetVal = MX00_OV2312_SetStrobeReg(pChan, pStrobeCtrl[0]);

            /* Update current shutter control */
            //MX00_OV2312_Ctrl.CurrentAEInfo.CurrentShutterCtrl[0] = pShutterCtrl[0];
        }


        //    SENSOR_DBG_SHR("[Vin%d/Sensor%d][MX00_OV2312_MIPI][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
        //                      pChan->VinID, pChan->SensorID,  *pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
    }
    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OV2312_MIPI_SetSlowShutterCtrl
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
#if 0
static UINT32 MX00_OV2312_MIPI_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (MX00_OV2312_Ctrl.Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        RetVal = MX00_OV2312_MIPI_SetSlowShutterReg(SlowShutterCtrl);

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d ][MX00_OV2312_MIPI][SHR] SlowShutterCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   SlowShutterCtrl);
    }

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV9716_Config
 *
 *  @Description:: Set sensor to indicated mode
 *
 *  @Input      ::
 *      Chan:            Vin ID and Sensor ID
 *      Mode:            Sensor Readout Mode Number
 *      ElecShutterMode: Operating Mode of the Electronic Shutter
 *
 *  @Output     ::
 *      pModeInfo: pointer to mode info of target mode
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OV2312_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    AMBA_VIN_MIPI_PAD_CONFIG_s MX00_OV2312PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    const UINT8 LaneShift[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0U,
        [AMBA_VIN_CHANNEL1] = 4U,
        [AMBA_VIN_CHANNEL2] = 6U,
    };

    UINT32 RetVal     = SENSOR_ERR_NONE;
    UINT16 SensorMode = (UINT16)pMode->ModeID;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo           = &MX00_OV2312_Ctrl.Status.ModeInfo;
    MX00_OV2312_FRAME_TIMING_s *pFrameTime       = &MX00_OV2312_Ctrl.FrameTime;
    static UINT8 Init = 0;
    UINT32 U32RVal = 0U;

    if ((pChan == NULL) || (SensorMode >= MX00_OV2312_NUM_MODE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Add delay time to wait MAX9296/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power off", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(1);

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power on", 0U, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        MX00_OV2312_PrepareModeInfo(pChan, pMode, pModeInfo);
        U32RVal |= AmbaWrap_memcpy(pFrameTime, &MX00_OV2312_ModeInfoList[SensorMode].FrameTiming, sizeof(MX00_OV2312_FRAME_TIMING_s));
        MX00_OV2312PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        MX00_OV2312PadConfig.EnabledPin = (((UINT32)1U << pModeInfo->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];

        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX00_OV2312PadConfig);
        /* Set MAX9296A PWDNB high */
        if (Init == 0U) {


            /* After reset VIN, set MAX9296A PWDNB high */
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB low", NULL, NULL, NULL, NULL, NULL);
            RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
            (void)AmbaKAL_TaskSleep(3);
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB high", NULL, NULL, NULL, NULL, NULL);

            RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
            (void)AmbaKAL_TaskSleep(3);
        } else {
            //vin recovery, can't reset quad MAX9296 to affect other MAX9296
        }
        RetVal |= Max9295_9296_Init(pChan->VinID, MX00_OV2312_GetEnabledLinkID(pChan->SensorID));

        if(RetVal == SENSOR_ERR_NONE) {
            Init = 1;
        } else {
            //NG case
        }

        MX00_OV2312_ConfigSerDes(pChan, SensorMode, pModeInfo);

        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX00_OV2312_SENSOR_ID_CHAN_0) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
        }
        if ((pChan->SensorID & MX00_OV2312_SENSOR_ID_CHAN_1) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
        }
        RetVal |= MX00_OV2312_HardwareReset(pChan);
        /* reset all sensors */
        (void)MX00_OV2312_RegisterWrite(pChan, MX00_OV2312_SC_SOFTWARE_RESET, 0x01);
        /* write registers of mode change to sensor */
        RetVal |= MX00_OV2312_ChangeReadoutMode(pChan, SensorMode);
#ifdef MX00_OV2312_IN_SLAVE_MODE
        RetVal |= MX00_OV2312_SetSlaveMode(pChan);
        RetVal |= MX00_OV2312_ConfigVinMasterSync(pChan);
#endif
        RetVal |= MX00_OV2312_SetStandbyOff(pChan);

        RetVal |= MX00_OV2312_ConfigVin(pChan->VinID, pModeInfo);
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}


#if 0
static INT32 MX00_OV2312_ConfigPost(UINT32 *pVinID)
{
    MX00_OV2312_SetStandbyOff();

    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX00_OV2312Obj = {
    .SensorName             = "OV2312",
    .SerdesName             = "MAX9295_9296",
    .Init                   = MX00_OV2312_Init,
    .Enable                 = MX00_OV2312_Enable,
    .Disable                = MX00_OV2312_Disable,
    .Config                 = MX00_OV2312_Config,

    .GetStatus              = MX00_OV2312_GetStatus,
    .GetModeInfo            = MX00_OV2312_GetModeInfo,
    .GetDeviceInfo          = MX00_OV2312_GetDeviceInfo,
    .GetHdrInfo             = MX00_OV2312_GetHdrInfo,
    .GetCurrentGainFactor   = MX00_OV2312_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX00_OV2312_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX00_OV2312_ConvertGainFactor,
    .ConvertShutterSpeed    = MX00_OV2312_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX00_OV2312_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = NULL, //MX00_OV2312_SetDigitalGainCtrl,
    .SetShutterCtrl         = MX00_OV2312_SetShutterCtrl,
    .SetSlowShutterCtrl     = NULL,//MX00_OV2312_MIPI_SetSlowShutterCtrl,

    .RegisterRead           = MX00_OV2312_RegisterRead,
    .RegisterWrite          = MX00_OV2312_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = MX00_OV2312_ConvertStrobeWidth,
    .SetStrobeCtrl          = MX00_OV2312_SetStrobeCtrl,
};

