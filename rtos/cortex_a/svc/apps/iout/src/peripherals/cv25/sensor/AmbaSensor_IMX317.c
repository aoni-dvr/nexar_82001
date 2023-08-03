/**
 *  [Engineering Version]
 *  @file AmbaSensor_IMX317.c
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
 *  @details Control APIs of SONY IMX317 CMOS sensor with LVDS interface
 *
 */

#include <AmbaWrap.h>

#include "AmbaVIN.h"

#include "AmbaSPI.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX317.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
//#include "bsp.h"

#if 0
static INT32 IMX317_ConfigPost(UINT32 *pVinID);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * IMX317 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX317_CTRL_s IMX317Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_GetPadConfig
 *
 *  @Description:: Get SLVS pad configuration
 *
 *  @Input      ::
 *      pMode:  Sensor readout mode number
 *
 *  @Output     ::
 *      pPadConfig:  Pad configuration
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX317_GetPadConfig(UINT32 VinID, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_VIN_SLVS_PAD_CONFIG_s *pPadConfig)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX317OutputInfo[ModeID];
    static AMBA_VIN_SLVS_PAD_CONFIG_s IMX317PadConfig[2][6] = {
        [AMBA_VIN_CHANNEL0] = {
            [IMX317_LVDS_10CH]  = {
                .EnabledPin = 0x3ffU,
            },
            [IMX317_LVDS_8CH]   = {
                .EnabledPin = 0xffU,
            },
            [IMX317_LVDS_6CH]   = {
                .EnabledPin = 0x7eU,
            },
            [IMX317_LVDS_4CH]   = {
                .EnabledPin = 0x3cU,
            },
            [IMX317_LVDS_2CH]   = {
                .EnabledPin = 0x18U,
            },
            [IMX317_LVDS_1CH]   = {
                .EnabledPin = 0x8U,
            },
        },
        [AMBA_VIN_CHANNEL1] = {
            [IMX317_LVDS_10CH]  = {
                .EnabledPin = 0x3ff000U,
            },
            [IMX317_LVDS_8CH]   = {
                .EnabledPin = 0xff000U,
            },
            [IMX317_LVDS_6CH]   = {
                .EnabledPin = 0x7e000U,
            },
            [IMX317_LVDS_4CH]   = {
                .EnabledPin = 0x3c000U,
            },
            [IMX317_LVDS_2CH]   = {
                .EnabledPin = 0x18000U,
            },
            [IMX317_LVDS_1CH]   = {
                .EnabledPin = 0x8000U,
            },
        },
    };

    switch (pOutputInfo->NumDataLanes) {
    case 10:
        pPadConfig->EnabledPin = IMX317PadConfig[VinID][IMX317_LVDS_10CH].EnabledPin;
        break;
    case 8:
        pPadConfig->EnabledPin = IMX317PadConfig[VinID][IMX317_LVDS_8CH].EnabledPin;
        break;
    case 6:
        pPadConfig->EnabledPin = IMX317PadConfig[VinID][IMX317_LVDS_6CH].EnabledPin;
        break;
    case 4:
        pPadConfig->EnabledPin = IMX317PadConfig[VinID][IMX317_LVDS_4CH].EnabledPin;
        break;
    case 2:
        pPadConfig->EnabledPin = IMX317PadConfig[VinID][IMX317_LVDS_2CH].EnabledPin;
        break;
    case 1:
    default:
        pPadConfig->EnabledPin = IMX317PadConfig[VinID][IMX317_LVDS_1CH].EnabledPin;
        break;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_PrepareModeInfo
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
static void IMX317_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &IMX317InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX317OutputInfo[ModeID];
    const IMX317_FRAME_TIMING_s         *pFrameTiming   = &IMX317ModeInfoList[ModeID].FrameTiming;

    (void)AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->LineLengthPck = (pFrameTiming->NumTickPerXhs * pFrameTiming->NumXhsPerH) *
                               (pOutputInfo->DataRate / pFrameTiming->InputClk) *
                               (pOutputInfo->NumDataLanes) / (pOutputInfo->NumDataBits);
    pModeInfo->FrameLengthLines = pFrameTiming->NumXhsPerV / pFrameTiming->NumXhsPerH;
    pModeInfo->NumExposureStepPerFrame = pFrameTiming->NumXhsPerV;
    pModeInfo->InputClk                = pFrameTiming->InputClk;
    pModeInfo->RowTime                 = IMX317ModeInfoList[ModeID].RowTime;
    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(const AMBA_SENSOR_CONFIG_s));
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX317HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_ConfigVin
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
static UINT32 IMX317_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_VIN_SLVS_CONFIG_s IMX317VinConfig = {
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
        .pLaneMapping   = NULL,
        .NumActiveLanes = 0,
        .SyncDetectCtrl = {
            .SyncInterleaving   = 0,
            .ITU656Type         = 0,
            .CustomSyncCode     = {
                .PatternAlign   = 1,        /* MSB */
                .SyncCodeMask   = 0x00ff,

                .SolDetectEnable = 1,
                .EolDetectEnable = 1,
                .SofDetectEnable = 0,
                .EofDetectEnable = 0,
                .SovDetectEnable = 1,
                .EovDetectEnable = 1,

                .PatternSol = 0x8000,
                .PatternEol = 0x9d00,
                .PatternSof = 0,
                .PatternEof = 0,
                .PatternSov = 0xab00,
                .PatternEov = 0xb600,
            },
        },
    };
    AMBA_VIN_MASTER_SYNC_CONFIG_s IMX317MasterSyncConfig = {
        .RefClk = 72000000,
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

    UINT8 IMX317LvdsLaneMux[2][6][10] = {
        [AMBA_VIN_CHANNEL0] = {
            [IMX317_LVDS_10CH]  = { 0, 1, 2, 8, 3, 4, 9, 5, 6, 7 },
            [IMX317_LVDS_8CH]   = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 0 },
            [IMX317_LVDS_6CH]   = { 1, 2, 3, 4, 5, 6, 0, 0, 0, 0 },
            [IMX317_LVDS_4CH]   = { 2, 3, 4, 5, 0, 0, 0, 0, 0, 0 },
            [IMX317_LVDS_2CH]   = { 3, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
            [IMX317_LVDS_1CH]   = { 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        },
        [AMBA_VIN_CHANNEL1] = {
            [IMX317_LVDS_10CH]  = { 12, 13, 14, 20, 15, 16, 21, 17, 18, 19},
            [IMX317_LVDS_8CH]   = { 12, 13, 14, 15, 16, 17, 18, 19, 0, 0 },
            [IMX317_LVDS_6CH]   = { 13, 14, 15, 16, 17, 18, 0, 0, 0, 0 },
            [IMX317_LVDS_4CH]   = { 14, 15, 16, 17, 0, 0, 0, 0, 0, 0 },
            [IMX317_LVDS_2CH]   = { 15, 16, 0, 0, 0, 0, 0, 0, 0, 0 },
            [IMX317_LVDS_1CH]   = { 15, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        },
    };


    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_SLVS_CONFIG_s *pVinCfg               = &IMX317VinConfig;
    AMBA_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncCfg = &IMX317MasterSyncConfig;
    const IMX317_FRAME_TIMING_s *pFrameTime       = &IMX317ModeInfoList[pModeInfo->Config.ModeID].FrameTiming;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;
    switch (pVinCfg->NumActiveLanes) {
    case 10:
        pVinCfg->pLaneMapping = IMX317LvdsLaneMux[VinID][IMX317_LVDS_10CH];
        break;
    case 8:
        pVinCfg->pLaneMapping = IMX317LvdsLaneMux[VinID][IMX317_LVDS_8CH];
        break;
    case 6:
        pVinCfg->pLaneMapping = IMX317LvdsLaneMux[VinID][IMX317_LVDS_6CH];
        break;
    case 4:
        pVinCfg->pLaneMapping = IMX317LvdsLaneMux[VinID][IMX317_LVDS_4CH];
        break;
    case 2:
        pVinCfg->pLaneMapping = IMX317LvdsLaneMux[VinID][IMX317_LVDS_2CH];
        break;
    case 1:
    default:
        pVinCfg->pLaneMapping = IMX317LvdsLaneMux[VinID][IMX317_LVDS_1CH];
        break;
    }

    (void)AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    RetVal |= AmbaVIN_SlvsConfig(VinID, pVinCfg);

    pMasterSyncCfg->RefClk= pFrameTime->InputClk;
    pMasterSyncCfg->HSync.Period = pFrameTime->NumTickPerXhs;
    pMasterSyncCfg->HSync.PulseWidth = 8;
    pMasterSyncCfg->VSync.Period = pFrameTime->NumXhsPerV / pFrameTime->NumXvsPerV;
    pMasterSyncCfg->VSync.PulseWidth = 8;

    RetVal |= AmbaVIN_MasterSyncEnable(AMBA_VIN_MSYNC0, pMasterSyncCfg);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_HardwareReset
 *
 *  @Description:: Reset IMX317 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 IMX317_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    (void)VinID;
#if 0  /* to be refined with bsp file */
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, 1U);
#endif
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_84, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(2);
    /* de-assert reset pin */
#if 0  /* to be refined with bsp file */
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, 0U);
#endif
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_84, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(2);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: RegWrite
 *
 *  @Description:: Read/Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      Chan:       Vin ID and sensor ID
 *      Addr:       Register Address
 *      pTxData:    Pointer to Write data buffer
 *      pRxData:    Pointer to Echo data buffer
 *      Size:       Number of Read/Write data
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT16 Addr, const UINT8 *pTxData, const UINT32 Size)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
    UINT8 IMX317SpiBuf[2][128];

    /*-----------------------------------------------------------------------------------------------*\
     * SPI configuration, {BaudRate, CsPolarity, ClkMode, ClkStretch, DataFrmSize, FrameBitOrder}
    \*-----------------------------------------------------------------------------------------------*/
    AMBA_SPI_CONFIG_s IMX317SpiConfig = {
        .BaudRate      = 10000000U,                         /* Transfer BaudRate in Hz */
        .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
        .ClkMode       = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
        .ClkStretch    = 0U,                                /* Stretch the duration (us) from the active edge of chip select to the first edge of clock */
        .DataFrameSize = 8U,                                /* Data Frame Size in Bit */
        .FrameBitOrder = AMBA_SPI_TRANSFER_LSB_FIRST,       /* Bit transfer order */
    };

    if ((Size + 3U) > sizeof(IMX317SpiBuf[0])) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        IMX317SpiBuf[0][0] = 0x81;
        IMX317SpiBuf[0][1] = (UINT8)(Addr >> 8U);
        IMX317SpiBuf[0][2] = (UINT8)(Addr & 0xffU);

        for (i = 0U; i < Size; i++) {
            IMX317SpiBuf[0][i+3U] = pTxData[i];
        }

        if (pChan->VinID == AMBA_VIN_CHANNEL0) {
            RetVal = AmbaSPI_MasterTransferD8(AMBA_SPI_MASTER1, 1U, &IMX317SpiConfig, Size + 3U, IMX317SpiBuf[0], NULL, NULL, 500U);
        } else {
            RetVal = AmbaSPI_MasterTransferD8(AMBA_SPI_MASTER2, 1U, &IMX317SpiConfig, Size + 3U, IMX317SpiBuf[0], NULL, NULL, 500U);
        }

        for (i = 0U; i < Size; i++) {
            AmbaPrint_PrintUInt5("Addr=0x%04x, Data=0x%02x", Addr + i, pTxData[i], 0, 0, 0);
            AmbaPrint_Flush();
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_RegisterWrite
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
static UINT32 IMX317_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT16 Addr, const UINT16 Data)
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

static UINT32 RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    (void)pChan;
    (void)Addr;
    (void)pRxData;
    (void)Size;

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_RegisterRead
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
static UINT32 IMX317_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(pChan, Addr, &RData, 1U);
        // *Data = RData;
        *Data = 0;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      ::
 *      Chan:       Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX317_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    UINT8 WData = 0x13U;

    RetVal = RegWrite(pChan, IMX317_OP_REG, &WData, 1U);
//    AmbaPrint("[IMX317] IMX317_SetStandbyOn");

    return RetVal;
}

#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      ::
 *      Chan:       Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX317_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    UINT8 WData = 0x01U;

    RetVal = RegWrite(pChan, IMX317_MODE_SEL_REG, &WData, 1U);

    return RetVal;
    /* Wait 8 frames */
//    (void)AmbaKAL_TaskSleep(8 * 1000 * IMX317Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / IMX317Ctrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[IMX317] IMX317_SetStandbyOff");
}
#endif

static UINT32 IMX317_SetReadoutPlusTimingRegs(const AMBA_SENSOR_CHANNEL_s *pChan, const IMX317_FRAME_TIMING_s *pFrameTime)
{
    UINT32 i, XVS;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[2];

    XVS = pFrameTime->NumXhsPerV / pFrameTime->NumXvsPerV;

    WData[0] = (UINT8)((XVS - (UINT32)0x32U) & (UINT32)0xFFU);
    WData[1] = (UINT8)(((XVS - (UINT32)0x32U) & (UINT32)0xFF00U) >> (UINT32)8U);
    RetVal |= RegWrite(pChan, IMX317_PSLVDS1_LSB_REG, &WData[0], 2U);
    RetVal |= RegWrite(pChan, IMX317_PSLVDS2_LSB_REG, &WData[0], 2U);
    RetVal |= RegWrite(pChan, IMX317_PSLVDS3_LSB_REG, &WData[0], 2U);
    RetVal |= RegWrite(pChan, IMX317_PSLVDS4_LSB_REG, &WData[0], 2U);

    for (i = 0U ; i < IMX317_NUM_READOUT_PSTMG_REG; i++) {
        WData[0] = IMX317PlstmgRegTable[i].Data;
        RetVal |= RegWrite(pChan, IMX317PlstmgRegTable[i].Addr, &WData[0], 1U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      Chan:        Vin ID and sensor ID
 *      ReadoutMode: Sensor readout mode
 *      SensorMode : Sensor mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX317_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    for (i = 0; i < IMX317_NUM_READOUT_MODE_REG; i ++) {
        RetVal |= RegWrite(pChan, IMX317ModeRegTable[i].Addr, &IMX317ModeRegTable[i].Data[ModeID], 1U);
//        AmbaPrint_PrintUInt5("Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX317RegTable[i].Addr,
//                             IMX317RegTable[i].Data[ModeID], RetVal, 0U, 0U);
    }

    IMX317Ctrl.CurrentAgcCtrl[0] = 0xffffffffU;
    IMX317Ctrl.CurrentAgcCtrl[1] = 0xffffffffU;
    IMX317Ctrl.CurrentDgcCtrl[0] = 0xffffffffU;
    IMX317Ctrl.CurrentDgcCtrl[1] = 0xffffffffU;
    IMX317Ctrl.CurrentShrCtrl[0] = 0xffffffffU;
    IMX317Ctrl.CurrentShrCtrl[1] = 0xffffffffU;

    return RetVal;
}

static UINT32 IMX317_PerformStandbyCancelSequence(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData;

    /* ----------------------------- */
    /* [Analog stabilization period] */
    /* ----------------------------- */
    /* 1st stabilization period */
    WData = 0x1AU;
    RetVal |= RegWrite(pChan, IMX317_OP_REG, &WData, 1);
    WData = 0x00U;
    RetVal |= RegWrite(pChan, IMX317_SYS_MODE_REG, &WData, 1);

    WData = 0x80U;
    RetVal |= RegWrite(pChan, IMX317_PLRD1_LSB_REG, &WData, 1);
    WData = 0x00U;
    RetVal |= RegWrite(pChan, IMX317_PLRD1_MSB_REG, &WData, 1);
    WData = 0x03U;
    RetVal |= RegWrite(pChan, IMX317_PLRD2_REG, &WData, 1);
    WData = 0x68U;
    RetVal |= RegWrite(pChan, IMX317_PLRD3_REG, &WData, 1);
    WData = 0x03U;
    RetVal |= RegWrite(pChan, IMX317_PLRD4_REG, &WData, 1);
    WData = 0x02U;
    RetVal |= RegWrite(pChan, IMX317_PLRD5_REG, &WData, 1);
    WData = 0x00U;
    RetVal |= RegWrite(pChan, IMX317_STBPL_IF_AD_REG, &WData, 1);

    /* initialize communication */
    WData = 0x01U;
    RetVal |= RegWrite(pChan, IMX317_PSMOVEN_REG, &WData, 1);

    /* Write golbal reg table after hardware reset */
    (void)IMX317_SetReadoutPlusTimingRegs(pChan, &IMX317ModeInfoList[ModeID].FrameTiming);

    (void)AmbaKAL_TaskSleep(10U);

    /* 2nd stabilization period */
    WData = 0x01U;
    RetVal |= RegWrite(pChan, IMX317_PLL_CKEN_REG, &WData, 1);
    WData = 0x01U;
    RetVal |= RegWrite(pChan, IMX317_PACKEN_REG, &WData, 1);
    WData = 0x18U;
    RetVal |= RegWrite(pChan, IMX317_OP_REG, &WData, 1);

    (void)AmbaKAL_TaskSleep(7U);

    /* ------------------------- */
    /* [Standby cancel sequence] */
    /* ------------------------- */
    WData = 0x10U;
    RetVal |= RegWrite(pChan, IMX317_CLPSQRST_REG, &WData, 1);
    WData = 0x01U;
    RetVal |= RegWrite(pChan, IMX317_DCKRST_REG, &WData, 1);

    /* Write registers of mode change to sensor */
    (void)IMX317_ChangeReadoutMode(pChan, ModeID);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX317_PreCalculateModeInfo(void)
{
    UINT32 i;

    for (i = 0U; i < AMBA_SENSOR_IMX317_NUM_MODE; i++) {
        IMX317ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX317ModeInfoList[i].FrameTiming.NumTickPerXhs *
                                                (DOUBLE)IMX317ModeInfoList[i].FrameTiming.NumXhsPerH /
                                                (DOUBLE)IMX317ModeInfoList[i].FrameTiming.InputClk);

        //AmbaPrint("row time: %f", IMX317ModeInfoList[i].RowTime);

    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_Init
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
static UINT32 IMX317_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const IMX317_FRAME_TIMING_s *pFrameTime = &IMX317ModeInfoList[0U].FrameTiming;

//    AmbaPrint_PrintStr5("IMX290 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#if 0  /* to be refined with bsp file */
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
#endif
        RetVal = AmbaVIN_SensorClkEnable(AMBA_VIN_SENSOR_CLOCK0, pFrameTime->InputClk);
        IMX317_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_Enable
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
static UINT32 IMX317_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX317_HardwareReset(pChan->VinID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_Disable
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
static UINT32 IMX317_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX317_SetStandbyOn(pChan);
    }

    //AmbaPrint("[IMX317] IMX317_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_GetStatus
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
static UINT32 IMX317_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &IMX317Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_GetModeInfo
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
static UINT32 IMX317_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = IMX317Ctrl.Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= AMBA_SENSOR_IMX317_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            IMX317_PrepareModeInfo(pMode, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_GetDeviceInfo
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
static UINT32 IMX317_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &IMX317DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_GetHdrInfo
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pShutterCtrl:    Electronic shutter control
 *  @Output     ::
 *      pExposureTime:   senosr hdr information
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX317_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pHdrInfo, &IMX317Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_GetCurrentGainFactor
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
static UINT32 IMX317_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
#if 0
    FLOAT AnalogGainFactor = 0.0;
    FLOAT DigitalGainFactor = 0.0;
    UINT8 i;
#endif
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#if 0
        if (IMX317Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            AnalogGainFactor = 1024.0 / ( 1024.0 - (FLOAT)IMX317Ctrl.CurrentAgcCtrl[0]);
            DigitalGainFactor = (FLOAT)IMX317Ctrl.CurrentDgcCtrl[0] / 256.0;
            *pGainFactor = AnalogGainFactor * DigitalGainFactor;

        } else if (IMX317Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (IMX317Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                AnalogGainFactor = 1024.0 / ( 1024.0 - (FLOAT)IMX317Ctrl.CurrentAgcCtrl[i]);
                DigitalGainFactor = (FLOAT)IMX317Ctrl.CurrentDgcCtrl[i] / 256.0;
                pGainFactor[i] = AnalogGainFactor * DigitalGainFactor;
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
#endif
        *pGainFactor = 0.0f;
    }
    //AmbaPrint("GainFactor:%f", *pGainFactor);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_GetCurrentShutterSpeed
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
static UINT32 IMX317_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX317Ctrl.Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT8 i;

    UINT32 RetVal = SENSOR_ERR_NONE;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX317Ctrl.CurrentShrCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX317Ctrl.CurrentShrCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}

#if 0
static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    if (DesiredFactor < 1.0) { /* 1.0 = IMX317DeviceInfo.MinAnalogGainFactor * IMX317DeviceInfo.MinDigitalGainFactor */
        *pAnalogGainCtrl = 0U;
        *pDigitalGainCtrl = 0x100U;

        *pActualFactor = 1.0f;
    } else {
        FLOAT AnalogGainFactor;
        FLOAT DigitalGainFactor;

        DigitalGainFactor = DesiredFactor / IMX317DeviceInfo.MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0) {
            *pDigitalGainCtrl = 0x100U;
            DigitalGainFactor = 1.0f;

            AnalogGainFactor = DesiredFactor;
        } else {
            if (DigitalGainFactor > IMX317DeviceInfo.MaxDigitalGainFactor) {
                DigitalGainFactor = IMX317DeviceInfo.MaxDigitalGainFactor;
            }

            (void)AmbaWrap_floor((DigitalGainFactor * 256.0), pDigitalGainCtrl);

            DigitalGainFactor = (FLOAT)*pDigitalGainCtrl / 256.0;

            AnalogGainFactor = DesiredFactor / DigitalGainFactor;
        }

        (void)AmbaWrap_floor(((1024.0) - (1024.0 / AnalogGainFactor)), pAnalogGainCtrl);

        if (*pAnalogGainCtrl > 978U) {
            *pAnalogGainCtrl = 978U; /* Max. 26.951 dB */
        }

        AnalogGainFactor = 1024.0 / (1024.0 - (FLOAT)*pAnalogGainCtrl);

        *pActualFactor = AnalogGainFactor * DigitalGainFactor;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_ConvertGainFactor
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
static UINT32 IMX317_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s* pModeInfo = &IMX317Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertGainFactor(pDesiredFactor->Gain[0], &(pActualFactor->Gain[0]), &(pGainCtrl->AnalogGain[0]), &(pGainCtrl->DigitalGain[0]));
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                ConvertGainFactor(pDesiredFactor->Gain[i], &(pActualFactor->Gain[i]), &(pGainCtrl->AnalogGain[i]), &(pGainCtrl->DigitalGain[i]));
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
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
static void ConvertShutterSpeed(AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 MaxExposureLine, MinExposureLine;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ShutterCtrl, ExposureFrames;

    (void)AmbaWrap_floor((DOUBLE)(ExposureTime / (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame)), &ShutterCtrl);

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        /* Maximun value of FRM_LENGTH_LINES is 65535d */
        ExposureFrames = (ShutterCtrl - 1U) / NumExposureStepPerFrame + 1U;
        if ((FrameLengthLines * ExposureFrames) > 0xffffU) {
            ShutterCtrl = (0xffffU / FrameLengthLines) * NumExposureStepPerFrame;
        }

        /* Maximun value of COARSE_INTEG_TIME is (FRM_LENGTH_LINES - 20) */
        MaxExposureLine= ((ShutterCtrl - 1U) / NumExposureStepPerFrame + 1U) * NumExposureStepPerFrame - 20U;
        MinExposureLine= 1U;
    } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
        ShutterCtrl = ((ShutterCtrl % 2U) == 1U) ? (ShutterCtrl - 1U) : (ShutterCtrl);

        MaxExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
    } else {
        return;
    }

    if (ShutterCtrl > MaxExposureLine) {
        *pShutterCtrl = MaxExposureLine;
    } else if (ShutterCtrl < MinExposureLine) {
        *pShutterCtrl = MinExposureLine;
    } else {
        *pShutterCtrl = ShutterCtrl;
    }

    *pActualExptime = (FLOAT)*pShutterCtrl * (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame);
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_ConvertShutterSpeed
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
static UINT32 IMX317_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX317Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
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
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_SetAnalogGainCtrl
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
static UINT32 IMX317_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX317Ctrl.Status.ModeInfo;
    // UINT8 TxData[2U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            // TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            // RetVal = RegWrite(pChan, IMX317_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX317Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            // TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            // RetVal |= RegWrite(pChan, IMX317_DOL_ANA_GAIN_1ST_MSB_REG, TxData, 2);

            // TxData[0] = (UINT8)((pAnalogGainCtrl[1] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pAnalogGainCtrl[1] & 0x00ffU);
            // RetVal |= RegWrite(pChan, IMX317_DOL_ANA_GAIN_2ND_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX317Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
            IMX317Ctrl.CurrentAgcCtrl[1] = pAnalogGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][IMX317][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      DigitalGainCtrl:    Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX317_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX317Ctrl.Status.ModeInfo;
    // UINT8 TxData[2U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            // TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            // RetVal = RegWrite(pChan, IMX317_DIG_GAIN_GR_MSB_REG, TxData, 2U);

            /* Update current AGC control */
            IMX317Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            // TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            // RetVal |= RegWrite(pChan, IMX317_DOL_DIG_GAIN_1ST_MSB_REG, TxData, 2U);

            // TxData[0] = (UINT8)((pDigitalGainCtrl[1] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pDigitalGainCtrl[1] & 0x00ffU);
            // RetVal |= RegWrite(pChan, IMX317_DOL_DIG_GAIN_2ND_MSB_REG, TxData, 2U);

            /* Update current AGC control */
            IMX317Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
            IMX317Ctrl.CurrentDgcCtrl[1] = pDigitalGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][IMX317][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_SetShutterCtrl
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
static UINT32 IMX317_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX317Ctrl.Status.ModeInfo;
    // UINT8 TxData[2U];

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            // TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            // RetVal = RegWrite(pChan, IMX317_COARSE_INTEG_TIME_MSB_REG, TxData, 2U);

            /* Update current shutter control */
            IMX317Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            // TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            // RetVal = RegWrite(pChan, IMX317_DOL_CIT_1ST_MSB, TxData, 2U);

            // TxData[0] = (UINT8)((pShutterCtrl[1] >> 8U) & 0x00ffU);
            // TxData[1] = (UINT8) (pShutterCtrl[1] & 0x00ffU);
            // RetVal = RegWrite(pChan, IMX317_DOL_CIT_2ND_MSB, TxData, 2U);

            /* Update current shutter control */
            IMX317Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
            IMX317Ctrl.CurrentShrCtrl[1] = pShutterCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        //    SENSOR_DBG_SHR("[Vin%d/Sensor%d][IMX317][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
        //                      pChan->VinID, pChan->SensorID,  *pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_SetSlowShutterCtrl
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
static UINT32 IMX317_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = IMX317Ctrl.Status.ModeInfo.Config.ModeID;
    // UINT32 TargetFrameLengthLines = 0U;
    // UINT8 TxData[2U];

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX317Ctrl.Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        // TargetFrameLengthLines = IMX317ModeInfoList[ModeID].FrameTiming.FrameLengthLines * SlowShutterCtrl;

        // TxData[0] = (UINT8)((TargetFrameLengthLines >> 8U) & 0x00ffU);
        // TxData[1] = (UINT8) (TargetFrameLengthLines & 0x00ffU);
        // RetVal = RegWrite(pChan, IMX317_FRM_LENGTH_LINES_MSB_REG, TxData, 2U);


        /* Update frame rate information */
        // IMX317Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        // IMX317Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        IMX317Ctrl.Status.ModeInfo.FrameRate.TimeScale = IMX317ModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d ][IMX317][SHR] SlowShutterCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   SlowShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX317_Config
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
static UINT32 IMX317_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX317Ctrl.Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    AMBA_VIN_SLVS_PAD_CONFIG_s PadConfig = {0};

    if (ModeID >= AMBA_SENSOR_IMX317_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_PrintUInt5("[IMX317] Config Mode: %d",ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        IMX317_PrepareModeInfo(pMode, pModeInfo);

        RetVal |= AmbaVIN_MasterSyncDisable(AMBA_VIN_MSYNC0);

        IMX317_GetPadConfig(pChan->VinID, pMode, &PadConfig);

        RetVal |= AmbaVIN_SlvsReset(pChan->VinID, &PadConfig);

        /* set pll */
#if 0  /* to be refined with bsp file */

        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
#endif
        RetVal |= AmbaVIN_SensorClkEnable(AMBA_VIN_SENSOR_CLOCK0, pModeInfo->InputClk);

        (void)AmbaKAL_TaskSleep(3);

        /* reset sensor */
        (void)IMX317_HardwareReset(pChan->VinID);
        (void)AmbaKAL_TaskSleep(3);

        (void)IMX317_PerformStandbyCancelSequence(pChan, ModeID);

        // (void)IMX317_SetStandbyOff();

        /* config vin */
        RetVal |= IMX317_ConfigVin(pChan->VinID, pModeInfo);

        RetVal |= AmbaVIN_SlvsReset(pChan->VinID, &PadConfig);
    }

    return RetVal;
}

#if 0
static INT32 IMX317_ConfigPost(UINT32 *pVinID)
{
    IMX317_SetStandbyOff();

    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX317Obj = {
    .Init                   = IMX317_Init,
    .Enable                 = IMX317_Enable,
    .Disable                = IMX317_Disable,
    .Config                 = IMX317_Config,
    .GetStatus              = IMX317_GetStatus,
    .GetModeInfo            = IMX317_GetModeInfo,
    .GetDeviceInfo          = IMX317_GetDeviceInfo,
    .GetHdrInfo             = IMX317_GetHdrInfo,
    .GetCurrentGainFactor   = IMX317_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX317_GetCurrentShutterSpeed,

    .ConvertGainFactor      = NULL, /* IMX317_ConvertGainFactor */
    .ConvertShutterSpeed    = NULL, /*IMX317_ConvertShutterSpeed */

    .SetAnalogGainCtrl      = IMX317_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX317_SetDigitalGainCtrl,
    .SetWbGainCtrl          = NULL,
    .SetShutterCtrl         = IMX317_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX317_SetSlowShutterCtrl,

    .RegisterRead           = IMX317_RegisterRead,
    .RegisterWrite          = IMX317_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
