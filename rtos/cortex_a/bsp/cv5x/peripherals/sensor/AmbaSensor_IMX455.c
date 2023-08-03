/**
 *  @file AmbaSensor_IMX455.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details Control APIs of SONY IMX455 CMOS sensor with SLVS-EC interface
 *
 */

#include <AmbaWrap.h>
#include <AmbaMisraFix.h>

#include "AmbaVIN.h"

#include "AmbaSPI.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX455.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSYS.h"
#include "AmbaDef.h"
#include "bsp.h"

#if 0
static UINT32 IMX455_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * IMX455 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX455_CTRL_s IMX455Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_GetPadConfig
 *
 *  @Description:: Get SLVS-EC pad configuration
 *
 *  @Input      ::
 *      pMode:  Sensor readout mode number
 *
 *  @Output     ::
 *      pPadConfig:  Pad configuration
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX455_GetPadConfig(UINT32 VinID, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX455OutputInfo[ModeID];
    UINT8 DualLink                                      = IMX455ModeInfoList[ModeID].DualLink;
    UINT32 Link1LaneShift                               = 8U; /* up to 8 lanes for each link */
    UINT32 EnabledPin;

    (void) VinID;

    if (DualLink == 0U) {
        EnabledPin = ((UINT32)1U << pOutputInfo->NumDataLanes) - 1U;
    } else {
        EnabledPin = ((UINT32)1U << (pOutputInfo->NumDataLanes >> 1U)) - 1U;
        EnabledPin |= EnabledPin << Link1LaneShift;
    }

    pPadConfig->EnabledPin = EnabledPin;
    pPadConfig->DataRate = pOutputInfo->DataRate;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      pMode:      pointer to desired sensor mode configuration
 *
 *  @Output     ::
 *      pModeInfo:  pointer to Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX455_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &IMX455InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX455OutputInfo[ModeID];
    const IMX455_FRAME_TIMING_s         *pFrameTiming   = &IMX455ModeInfoList[ModeID].FrameTiming;
    DOUBLE WorkDouble;

    (void)AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    WorkDouble = (DOUBLE) pFrameTiming->NumTickPerXhs * (DOUBLE) pFrameTiming->NumXhsPerH;
    WorkDouble *= (DOUBLE) pOutputInfo->DataRate / (DOUBLE) pFrameTiming->InputClk;
    WorkDouble *= (DOUBLE) pOutputInfo->NumDataLanes / (DOUBLE) pOutputInfo->NumDataBits;
    pModeInfo->LineLengthPck = (UINT32) WorkDouble;

    WorkDouble = (DOUBLE) pFrameTiming->NumXhsPerV / (DOUBLE) pFrameTiming->NumXhsPerH;
    pModeInfo->FrameLengthLines = (UINT32) WorkDouble;

    pModeInfo->NumExposureStepPerFrame = pFrameTiming->NumXhsPerV;
    pModeInfo->InputClk                = pFrameTiming->InputClk;
    pModeInfo->RowTime                 = IMX455ModeInfoList[ModeID].RowTime;
    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(const AMBA_SENSOR_CONFIG_s));
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX455HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 1U;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_ConfigMasterSync
 *
 *  @Description:: Configure Master Sync generation
 *
 *  @Input      ::
 *      pFrameTime: pointer to Sensor frame time configuration
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX455_ConfigMasterSync(const IMX455_FRAME_TIMING_s *pFrameTime)
{
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg = {
        .RefClk = 72000000,
        .HSync = {
            .Period     = 0,
            .PulseWidth = 8,
            .Polarity   = 0
        },
        .VSync = {
            .Period     = 0,
            .PulseWidth = 8,
            .Polarity   = 0
        },
        .HSyncDelayCycles = 3,
        .VSyncDelayCycles = 0,
        .ToggleHsyncInVblank = 1
    };

    MasterSyncCfg.RefClk= pFrameTime->InputClk;
    MasterSyncCfg.HSync.Period = pFrameTime->NumTickPerXhs;
    MasterSyncCfg.VSync.Period = pFrameTime->NumXhsPerV / pFrameTime->NumXvsPerV;

    (void) AmbaVIN_MasterSyncEnable(AMBA_SENSOR_MSYNC_CHANNEL, &MasterSyncCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_ConfigVin
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
static UINT32 IMX455_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_VIN_SLVSEC_CONFIG_s IMX455VinConfig = {
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
            .RxHvSyncCtrl = {
                .NumActivePixels = 0,
                .NumActiveLines  = 0,
                .NumTotalPixels  = 0,
                .NumTotalLines   = 0,
            },
            .SplitCtrl    = {0},
            .DelayedVsync = 0,
        },

        .NumActiveLanes = 0,
        .LinkType       = AMBA_VIN_SLVSEC_LINK_TYPE_S,
        .EccOption      = 2,
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_SLVSEC_CONFIG_s *pVinCfg            = &IMX455VinConfig;
    UINT8 DualLink                                = IMX455ModeInfoList[pModeInfo->Config.ModeID].DualLink;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes = pModeInfo->OutputInfo.NumDataLanes;
    pVinCfg->LinkType = (DualLink == 0U) ? AMBA_VIN_SLVSEC_LINK_TYPE_S: AMBA_VIN_SLVSEC_LINK_TYPE_D;

    (void)AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    RetVal |= AmbaVIN_SlvsEcConfig(VinID, pVinCfg);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_HardwareReset
 *
 *  @Description:: Reset IMX455 Image Sensor Device
 *
 *  @Input      ::
 *      VinID:      Vin channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 IMX455_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);
    /* de-assert reset pin */
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(1);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_SerialComm
 *
 *  @Description:: Read/Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      pChan:      pointer to sensor channel ID
 *      CID:        Chip ID
 *      Addr:       Start Address
 *      pTxData:    Pointer to Write data buffer
 *      Size:       Number of Read/Write data
 *
 *  @Output     ::
 *      pRxData:    Pointer to Read data buffer
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_SerialComm(const AMBA_SENSOR_CHANNEL_s *pChan, UINT8 CID, UINT16 Addr, const UINT8 *pTxData, UINT8 *pRxData, UINT32 Size, UINT32 BaudRate)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE, Size0 = 3U;
    UINT32 MasterID, SlaveMask;
    UINT8 IMX455SpiBuf[2][128];
    /*-----------------------------------------------------------------------------------------------*\
     * SPI configuration, {BaudRate, CsPolarity, ClkMode, ClkStretch, DataFrmSize, FrameBitOrder}
    \*-----------------------------------------------------------------------------------------------*/
    AMBA_SPI_CONFIG_s IMX455SpiConfig = {
        .BaudRate      = BaudRate,                          /* Transfer BaudRate in Hz */
        .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
        .ClkMode       = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
        .ClkStretch    = 0U,                                /* Stretch the duration (us) from the active edge of chip select to the first edge of clock */
        .DataFrameSize = 8U,                                /* Data Frame Size in Bit */
        .FrameBitOrder = AMBA_SPI_TRANSFER_LSB_FIRST,       /* Bit transfer order */
    };

    AmbaMisra_TouchUnused(&pChan);

    if ((Size + Size0) > (UINT32)sizeof(IMX455SpiBuf[0])) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        MasterID = AMBA_SENSOR_SPI_CHANNEL_SLVSEC;
#if AMBA_SENSOR_SPI_SLAVE_ID_SLVSEC == 0U
        SlaveMask = 1U;
#else
        SlaveMask = (UINT32)1U << AMBA_SENSOR_SPI_SLAVE_ID_SLVSEC;
#endif
        IMX455SpiBuf[0][0] = CID;
        IMX455SpiBuf[0][1] = (UINT8)(Addr >> 8U);
        IMX455SpiBuf[0][2] = (UINT8)(Addr & 0xffU);

        if (pTxData != NULL) {
            for (i = 0U; i < Size; i++) {
                IMX455SpiBuf[0][i + Size0] = pTxData[i];
                /* for debugging */
                //AmbaPrint_PrintUInt5("[IMX455] CID=0x%02x, Addr=0x%04x, Data=0x%02x", CID, Addr + i, pTxData[i], 0U, 0U);
                //AmbaPrint_Flush();
            }
        }

        RetVal = AmbaSPI_MasterTransferD8(MasterID, SlaveMask, &IMX455SpiConfig, Size + Size0, IMX455SpiBuf[0], IMX455SpiBuf[1], NULL, 500U);

        if (pRxData != NULL) {
            for (i = 0U; i < Size; i++) {
                pRxData[i] = IMX455SpiBuf[1][i + Size0];
            }
        }
    }

    return RetVal;
}

static UINT32 RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    return IMX455_SerialComm(pChan, 0x81U, Addr, pTxData, NULL, Size, 10000000U);
}

static UINT32 RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    return IMX455_SerialComm(pChan, 0x82U, Addr, NULL, pRxData, Size, 5000000U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_RegisterWrite
 *
 *  @Description:: Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      pChan:      pointer to sensor channel ID
 *      Addr:       Register address
 *      Data:       Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
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

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_RegisterRead
 *
 *  @Description:: Read sensor registers API
 *
 *  @Input      ::
 *      pChan:      pointer to sensor channel ID
 *      Addr:       Register address
 *
 *  @Output     ::
 *      pData:      pointer to Register value
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData = 0U;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(pChan, Addr, &RData, 1U);
        *pData = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_SetShutterReg
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
static UINT32 IMX455_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 ShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHR;
    UINT8 WData;

    SHR = (IMX455Ctrl.Status.ModeInfo.NumExposureStepPerFrame - ShutterCtrl) / 2U;

    WData = (UINT8)(SHR & 0xFFU);
    RetVal |= RegWrite(pChan, IMX455_SHR_L, &WData, 1U);

    WData = (UINT8)((SHR >> 8U) & 0x7FU);
    RetVal |= RegWrite(pChan, IMX455_SHR_H, &WData, 1U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_SetAnalogGainReg
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
static UINT32 IMX455_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData;

    if (AnalogGainCtrl >= 0xDFCU) {         /* 18dB to 24dB */
        WData = 0x4U;
        RetVal |= RegWrite(pChan, IMX455_APGC_ADD, &WData, 1U);
        WData = 0xCU;
        RetVal |= RegWrite(pChan, IMX455_CLPAPGC, &WData, 1U);
    } else if (AnalogGainCtrl >= 0x7FBU) {  /* 6dB to 18dB */
        WData = 0x4U;
        RetVal |= RegWrite(pChan, IMX455_APGC_ADD, &WData, 1U);
        WData = 0xAU;
        RetVal |= RegWrite(pChan, IMX455_CLPAPGC, &WData, 1U);
    } else {                                /* 0dB to 6dB */
        WData = 0x0U;
        RetVal |= RegWrite(pChan, IMX455_APGC_ADD, &WData, 1U);
        WData = 0x8U;
        RetVal |= RegWrite(pChan, IMX455_CLPAPGC, &WData, 1U);
    }

    WData = (UINT8)(AnalogGainCtrl & 0xFFU);
    RetVal |= RegWrite(pChan, IMX455_APGC_N_L, &WData, 1U);

    WData = (UINT8)((AnalogGainCtrl >> 8U) & 0xFU);
    RetVal |= RegWrite(pChan, IMX455_APGC_N_H, &WData, 1U);

    WData = (UINT8)(AnalogGainCtrl & 0xFFU);
    RetVal |= RegWrite(pChan, IMX455_APGC_S_L, &WData, 1U);

    WData = (UINT8)((AnalogGainCtrl >> 8U) & 0xFU);
    RetVal |= RegWrite(pChan, IMX455_APGC_S_H, &WData, 1U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_SetDigitalGainReg
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
static UINT32 IMX455_SetDigitalGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 DigitalGainCtrl)
{
    UINT32 RetVal;
    UINT8 WData;

    WData = (UINT8)((DigitalGainCtrl << 4U) & 0x70U);
    if (WData > 0x60U) {
        WData = 0x60U;
    }
    RetVal = RegWrite(pChan, IMX455_DGAIN, &WData, 1U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      ::
 *      pChan:      pointer to sensor channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = 0U;

    WData = 0x03U;  /* PWR_AUTO = 1h, SLP_CTRL = 1h */
    RetVal |= RegWrite(pChan, IMX455_PWR_CTRL_REG, &WData, 1U);

    WData = 0x05U;  /* STANDBY = 0h, WAKEUP = 1h */
    RetVal |= RegWrite(pChan, IMX455_STANDBY_WAKEUP_REG, &WData, 1U);

    WData = 0x04U;  /* STANDBY = 0h, WAKEUP = 1h */
    RetVal |= RegWrite(pChan, IMX455_STANDBY_WAKEUP_REG, &WData, 1U);

    WData = 0x01U;  /* PWR_AUTO = 1h, SLP_CTRL = 0h */
    RetVal |= RegWrite(pChan, IMX455_PWR_CTRL_REG, &WData, 1U);

    return RetVal;
}

static UINT32 IMX455_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal, i;
    UINT8 WData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        WData = (UINT8)ModeID;
        RetVal = RegWrite(pChan, IMX455_MODE_REG, &WData, 1U);
    };

    AmbaPrint_PrintUInt5("[IMX455] Set Initial Setting Registers - BEGIN", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U ; i < IMX455_NUM_INIT_REG; i++) {
        WData = IMX455InitRegTable[i].Data[ModeID];
        RetVal |= RegWrite(pChan, IMX455InitRegTable[i].Addr, &WData, 1U);
    }
    AmbaPrint_PrintUInt5("[IMX455] Set Initial Setting Registers - END", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

static UINT32 IMX455_SetAttrRegs(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE;
    UINT8 WData;

    for (i = 0U ; i < IMX455_NUM_ATTR_REG; i++) {
        WData = IMX455AttrRegTable[i].Data;
        RetVal |= RegWrite(pChan,  IMX455AttrRegTable[i].Addr, &WData, 1U);
    }

    return RetVal;
}

static UINT32 IMX455_PowerOnSequence(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if ((ModeID == IMX455_7696_4320_19P) || (ModeID == IMX455_7696_4320_30P)) {
            WData = 0x0EU;  /* Setting High Speed AD mode */
            RetVal |= RegWrite(pChan, IMX455_PLL_AD_SETTING_REG, &WData, 1U);
        } else {
            WData = 0x0CU;  /* Setting Normal AD mode */
            RetVal |= RegWrite(pChan, IMX455_PLL_AD_SETTING_REG, &WData, 1U);
        }

        WData = 0x01U;  /* PWR_AUTO = 1h, SLP_CTRL = 0h */
        RetVal |= RegWrite(pChan, IMX455_PWR_CTRL_REG, &WData, 1U);

        WData = 0x04U;  /* STANDBY = 0h, WAKEUP = 1h */
        RetVal |= RegWrite(pChan, IMX455_STANDBY_WAKEUP_REG, &WData, 1U);
    };

    return RetVal;
};

static UINT32 IMX455_StandbyCancelSequence(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= IMX455_ChangeReadoutMode(pChan, ModeID);
    RetVal |= IMX455_SetAttrRegs(pChan);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX455_PreCalculateModeInfo(void)
{
    UINT32 i;

    for (i = 0U; i < IMX455_NUM_MODE; i++) {
        IMX455ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX455ModeInfoList[i].FrameTiming.NumTickPerXhs *
                                                (DOUBLE)IMX455ModeInfoList[i].FrameTiming.NumXhsPerH /
                                                (DOUBLE)IMX455ModeInfoList[i].FrameTiming.InputClk);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      pChan:   pointer to sensor channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const IMX455_FRAME_TIMING_s *pFrameTime = &IMX455ModeInfoList[0U].FrameTiming;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        IMX455_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      pChan:   pointer to sensor channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX455_HardwareReset(pChan->VinID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      pChan:   pointer to sensor channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX455_SetStandbyOn(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_GetStatus
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
static UINT32 IMX455_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &IMX455Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_GetModeInfo
 *
 *  @Description:: Get Mode Info of indicated mode
 *
 *  @Input      ::
 *      pChan:     pointer to sensor channel ID
 *      pMode:     pointer to desired sensor mode configuration
 *
 *  @Output     ::
 *      pModeInfo: pointer to requested Mode Info
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s DesiredMode;

    if ((pChan == NULL) || (pMode ==NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(&DesiredMode, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) == ERR_NONE) {

            if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
                DesiredMode.ModeID = IMX455Ctrl.Status.ModeInfo.Config.ModeID;
            }

            if ((DesiredMode.ModeID >= IMX455_NUM_MODE) || (pModeInfo == NULL)) {
                RetVal = SENSOR_ERR_ARG;
            } else {

                IMX455_PrepareModeInfo(&DesiredMode, pModeInfo);
            }
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_GetDeviceInfo
 *
 *  @Description:: Get Sensor Device Info
 *
 *  @Input      ::
 *      pChan:       pointer to sensor channel ID
 *
 *  @Output     ::
 *      pDeviceInfo: pointer to device info
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &IMX455DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_GetHdrInfo
 *
 *  @Description:: Get HDR-related information
 *
 *  @Input      ::
 *      pChan:         pointer to sensor channel ID
 *      pShutterCtrl:  pointer to Electronic shutter control
 *  @Output     ::
 *      pHdrInfo:      senosr hdr information
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pHdrInfo, &IMX455Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return RetVal;
}

static FLOAT GetDigitalGain(UINT32 SettingVal)
{
    FLOAT Factor = 1.0f;

    /* Digital gain setting [6:4] */
    /* 0h: OFF / 1h: 6[dB] / 2h: 12[dB] / 3h: 18[dB] / 4h: 24[dB] / 5h: 30[dB] / 6h: 36[dB] / 7h: Prohibited */
    switch (SettingVal) {
        case 6U:
            Factor = 63.09573444801932f;
            break;
        case 5U:
            Factor = 31.62277660168379f;
            break;
        case 4U:
            Factor = 15.84893192461113f;
            break;
        case 3U:
            Factor = 7.94328234724282f;
            break;
        case 2U:
            Factor = 3.98107170553497f;
            break;
        case 1U:
            Factor = 1.99526231496888f;
            break;
        case 0U:
        default:
            Factor = 1.0f;
            break;
    }
    return Factor;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_GetCurrentGainFactor
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
static UINT32 IMX455_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT AnalogGainFactor = 0.0f;
    FLOAT DigitalGainFactor = 0.0f;
    UINT8 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (IMX455Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            AnalogGainFactor = 4095.0f / ( 4095.0f - (FLOAT)IMX455Ctrl.CurrentAgcCtrl[0]);
            DigitalGainFactor = (FLOAT)GetDigitalGain(IMX455Ctrl.CurrentDgcCtrl[0]);
            *pGainFactor = AnalogGainFactor * DigitalGainFactor;

        } else if (IMX455Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (IMX455Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                AnalogGainFactor = 4095.0f / ( 4095.0f - (FLOAT)IMX455Ctrl.CurrentAgcCtrl[i]);
                DigitalGainFactor = (FLOAT)GetDigitalGain(IMX455Ctrl.CurrentDgcCtrl[0]);
                pGainFactor[i] = AnalogGainFactor * DigitalGainFactor;
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_GetCurrentShutterSpeed
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
static UINT32 IMX455_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX455Ctrl.Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT8 i;

    UINT32 RetVal = SENSOR_ERR_NONE;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX455Ctrl.CurrentShrCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX455Ctrl.CurrentShrCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    UINT32 i;

    if (DesiredFactor < 1.0f) { /* 1.0 = IMX455DeviceInfo.MinAnalogGainFactor * IMX455DeviceInfo.MinDigitalGainFactor */
        *pAnalogGainCtrl = 0U;
        *pDigitalGainCtrl = 0U;
        *pActualFactor = 1.0f;
    } else {
        FLOAT DigitalGainFactor;
        FLOAT AnalogGainFactor;
        FLOAT AnalogGainCtrlInFloat;

        DigitalGainFactor = DesiredFactor / IMX455DeviceInfo.MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0f) {
            *pDigitalGainCtrl = 0U;
            AnalogGainFactor = DesiredFactor;
        } else {
            for (i = 0U; i < IMX455_MAX_DGAIN; i++) {
                if (DigitalGainFactor < GetDigitalGain(i)) {
                    break;
                }
            }
            *pDigitalGainCtrl = i;
            DigitalGainFactor = GetDigitalGain(i);
            AnalogGainFactor = DesiredFactor / DigitalGainFactor;
        }

        AnalogGainCtrlInFloat = (FLOAT)(4095.0 - (4095.0 / AnalogGainFactor));
        *pAnalogGainCtrl = (UINT32)AnalogGainCtrlInFloat;

        if (*pAnalogGainCtrl > IMX455_MAX_APGC) {
            *pAnalogGainCtrl = IMX455_MAX_APGC;
        }

        *pActualFactor = GetDigitalGain(*pDigitalGainCtrl);
        *pActualFactor *= 4095.0 / (4095.0 - (FLOAT)*pAnalogGainCtrl);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_ConvertGainFactor
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
static UINT32 IMX455_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &IMX455Ctrl.Status.ModeInfo;
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
 *
 *      ExposureTime:   Exposure time / shutter speed
 *
 *  @Output     ::
 *      pShutterCtrl:   Electronic shutter control
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 MaxExposureline, MinExposureline, FrameLengthLines, NumExposureStepPerFrame, ShutterCtrl;
    FLOAT RowTime;
    DOUBLE ShutterTimeUnit, ShutterCtrlInDb;

    FrameLengthLines = pModeInfo->FrameLengthLines;
    NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    RowTime = IMX455ModeInfoList[pModeInfo->Config.ModeID].RowTime;
    ShutterTimeUnit = (DOUBLE)RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

    (void)AmbaWrap_floor(((DOUBLE)ExposureTime / ShutterTimeUnit), &ShutterCtrlInDb);
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    /* VMAX is 20 bits */
    if (ShutterCtrl > ((0xFFFFFU / NumExposureStepPerFrame) * NumExposureStepPerFrame)) {
        ShutterCtrl = ((0xFFFFFU / NumExposureStepPerFrame) * NumExposureStepPerFrame);
    } else if (ShutterCtrl < 1U) {
        ShutterCtrl = 1U;
    } else {
        /* do not need to adjust ShutterCtrl */
    }

    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MaxExposureline = ((((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - 2U;
        MinExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
        /* For none-HDR mode */
    } else {
        MaxExposureline = NumExposureStepPerFrame - 4U;
        MinExposureline = 4U;
    }

    if (ShutterCtrl > MaxExposureline) {
        ShutterCtrl = MaxExposureline;
    } else if (ShutterCtrl < MinExposureline) {
        ShutterCtrl = MinExposureline;
    } else {
        /* MisraC */  /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl   = ShutterCtrl;
    *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_ConvertShutterSpeed
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
static UINT32 IMX455_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX455Ctrl.Status.ModeInfo;
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
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_SetAnalogGainCtrl
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
static UINT32 IMX455_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX455Ctrl.Status.ModeInfo;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX455_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

            /* Update current AGC control */
            IMX455Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            /* TODO */

            /* Update current AGC control */
            IMX455Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
            IMX455Ctrl.CurrentAgcCtrl[1] = pAnalogGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_SetDigitalGainCtrl
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
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX455Ctrl.Status.ModeInfo;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX455_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current AGC control */
            IMX455Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            /* TODO */

            /* Update current AGC control */
            IMX455Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
            IMX455Ctrl.CurrentDgcCtrl[1] = pDigitalGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_SetShutterCtrl
 *
 *  @Description:: set shutter control
 *
 *  @Input      ::
 *      pChan:              pointer to sensor channel ID
 *      pShutterCtrl:       pointer to Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX455Ctrl.Status.ModeInfo;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX455_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            IMX455Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            /* TODO */

            /* Update current shutter control */
            IMX455Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
            IMX455Ctrl.CurrentShrCtrl[1] = pShutterCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_SetSlowShutterCtrl
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
static UINT32 IMX455_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = IMX455Ctrl.Status.ModeInfo.Config.ModeID;
    // UINT32 TargetFrameLengthLines;

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX455Ctrl.Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        // TargetFrameLengthLines = IMX455ModeInfoList[ModeID].FrameTiming.FrameLengthLines * SlowShutterCtrl;
        /* TODO */

        /* Update frame rate information */
        // IMX455Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        // IMX455Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        IMX455Ctrl.Status.ModeInfo.FrameRate.TimeScale = IMX455ModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;
    }

    return RetVal;
}

static UINT32 IMX455_PerformSlvsEcPhyCal(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX455Ctrl.Status.ModeInfo;
    const IMX455_FRAME_TIMING_s *pFrameTime = &IMX455ModeInfoList[pModeInfo->Config.ModeID].FrameTiming;
    UINT32 CortexClk = 0U, Delay, RetVal = SENSOR_ERR_NONE;
    DOUBLE WorkDouble;

    (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_CORTEX, &CortexClk);
    WorkDouble = (DOUBLE) pFrameTime->NumTickPerXhs / (DOUBLE) pFrameTime->InputClk;
    WorkDouble *= (DOUBLE)CortexClk;
    WorkDouble *= 2340.0;
    WorkDouble += (14.22 * 1e-6) * (DOUBLE)CortexClk; /* low period */
    Delay = (UINT32) WorkDouble;

    AmbaPrint_PrintUInt5("[IMX455] Enable VIN Master sync", 0U, 0U, 0U, 0U, 0U);
    IMX455_ConfigMasterSync(pFrameTime);
    AmbaDelayCycles(Delay);
    (void)AmbaKAL_TaskSleep(40);

    if (AmbaVIN_SlvsEcCalib(pChan->VinID, pPadConfig) != OK) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    AmbaPrint_PrintUInt5("[IMX455] Phy setup is done (delay %u/%u seconds)", Delay, CortexClk, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX455_Config
 *
 *  @Description:: Set sensor to indicated mode
 *
 *  @Input      ::
 *      pChan:              pointer to sensor channel ID
 *      pMode:              pointer to desired sensor mode configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX455_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX455Ctrl.Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    AMBA_VIN_SLVSEC_PAD_CONFIG_s PadConfig = {0};

    if (ModeID >= IMX455_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_PrintUInt5("[IMX455] Config Mode: %d", ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        IMX455_PrepareModeInfo(pMode, pModeInfo);

        RetVal |= AmbaVIN_MasterSyncDisable(AMBA_SENSOR_MSYNC_CHANNEL);

        IMX455_GetPadConfig(pChan->VinID, pMode, &PadConfig);

        /* set pll */
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);

        (void)IMX455_HardwareReset(pChan->VinID);

        RetVal |= AmbaVIN_SlvsEcReset(pChan->VinID, &PadConfig);

        RetVal |= IMX455_PowerOnSequence(pChan, ModeID);

        RetVal |= IMX455_StandbyCancelSequence(pChan, ModeID);

        /* config vin */
        RetVal |= IMX455_ConfigVin(pChan->VinID, pModeInfo);

        RetVal |= IMX455_PerformSlvsEcPhyCal(pChan, &PadConfig);

        AmbaPrint_PrintUInt5("[IMX455] Sensor Config Done", 0U, 0U, 0U, 0U, 0U);
    }
    return RetVal;
}

#if 0
static UINT32 IMX455_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX455Obj = {
    .SensorName             = "IMX455",
    .SerdesName             = "NA",
    .Init                   = IMX455_Init,
    .Enable                 = IMX455_Enable,
    .Disable                = IMX455_Disable,
    .Config                 = IMX455_Config,
    .GetStatus              = IMX455_GetStatus,
    .GetModeInfo            = IMX455_GetModeInfo,
    .GetDeviceInfo          = IMX455_GetDeviceInfo,
    .GetHdrInfo             = IMX455_GetHdrInfo,
    .GetCurrentGainFactor   = IMX455_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX455_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX455_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX455_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX455_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX455_SetDigitalGainCtrl,
    .SetWbGainCtrl          = NULL,
    .SetShutterCtrl         = IMX455_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX455_SetSlowShutterCtrl,

    .RegisterRead           = IMX455_RegisterRead,
    .RegisterWrite          = IMX455_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
