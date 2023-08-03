/**
 *  [Engineering Version]
 *  @file AmbaSensor_IMX383.c
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
 *  @details Control APIs of SONY IMX383 CMOS sensor with SLVS-EC interface
 *
 */

#include <AmbaWrap.h>
#include <AmbaMisraFix.h>

#include "AmbaVIN.h"

#include "AmbaSPI.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX383.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSYS.h"
#include "AmbaDef.h"
#include "bsp.h"

#if 0
static UINT32 IMX383_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * IMX383 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX383_CTRL_s IMX383Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_GetPadConfig
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
static void IMX383_GetPadConfig(UINT32 VinID, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX383OutputInfo[ModeID];
    UINT8 DualLink                                      = IMX383ModeInfoList[ModeID].DualLink;
    UINT32 Link1LaneShift                               = 6U; /* up to 6 lanes for each link */
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
 *  @RoutineName:: IMX383_PrepareModeInfo
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
static void IMX383_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &IMX383InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX383OutputInfo[ModeID];
    const IMX383_FRAME_TIMING_s         *pFrameTiming   = &IMX383ModeInfoList[ModeID].FrameTiming;
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
    pModeInfo->RowTime                 = IMX383ModeInfoList[ModeID].RowTime;
    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(const AMBA_SENSOR_CONFIG_s));
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX383HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_ConfigMasterSync
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
static void IMX383_ConfigMasterSync(const IMX383_FRAME_TIMING_s *pFrameTime)
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
        .HSyncDelayCycles = 0,
        .VSyncDelayCycles = 0,
        .ToggleHsyncInVblank = 1
    };

    MasterSyncCfg.RefClk= pFrameTime->InputClk;
    MasterSyncCfg.HSync.Period = pFrameTime->NumTickPerXhs;
    MasterSyncCfg.VSync.Period = pFrameTime->NumXhsPerV / pFrameTime->NumXvsPerV;

    (void) AmbaVIN_MasterSyncEnable(AMBA_VIN_MSYNC0, &MasterSyncCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_ConfigVin
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
static UINT32 IMX383_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_VIN_SLVSEC_CONFIG_s IMX383VinConfig = {
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
    AMBA_VIN_SLVSEC_CONFIG_s *pVinCfg            = &IMX383VinConfig;
    UINT8 DualLink                                = IMX383ModeInfoList[pModeInfo->Config.ModeID].DualLink;

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
 *  @RoutineName:: IMX383_HardwareReset
 *
 *  @Description:: Reset IMX383 Image Sensor Device
 *
 *  @Input      ::
 *      VinID:      Vin channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 IMX383_HardwareReset(UINT32 VinID)
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
 *  @RoutineName:: IMX383_SerialComm
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
static UINT32 IMX383_SerialComm(const AMBA_SENSOR_CHANNEL_s *pChan, UINT8 CID, UINT16 Addr, const UINT8 *pTxData, UINT8 *pRxData, UINT32 Size, UINT32 BaudRate)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE, Size0 = 3U;
    UINT32 MasterID, SlaveMask;
    UINT8 IMX383SpiBuf[2][128];
    /*-----------------------------------------------------------------------------------------------*\
     * SPI configuration, {BaudRate, CsPolarity, ClkMode, ClkStretch, DataFrmSize, FrameBitOrder}
    \*-----------------------------------------------------------------------------------------------*/
    AMBA_SPI_CONFIG_s IMX383SpiConfig = {
        .BaudRate      = BaudRate,                          /* Transfer BaudRate in Hz */
        .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
        .ClkMode       = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
        .ClkStretch    = 0U,                                /* Stretch the duration (us) from the active edge of chip select to the first edge of clock */
        .DataFrameSize = 8U,                                /* Data Frame Size in Bit */
        .FrameBitOrder = AMBA_SPI_TRANSFER_LSB_FIRST,       /* Bit transfer order */
    };

    AmbaMisra_TouchUnused(&pChan);

    if ((Size + Size0) > sizeof(IMX383SpiBuf[0])) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        MasterID = AMBA_SENSOR_SPI_CHANNEL_SLVSEC;
#if AMBA_SENSOR_SPI_SLAVE_ID_SLVSEC == 0U
        SlaveMask = 1U;
#else
        SlaveMask = (UINT32)1U << AMBA_SENSOR_SPI_SLAVE_ID_SLVSEC;
#endif
        IMX383SpiBuf[0][0] = CID;
        IMX383SpiBuf[0][1] = (UINT8)(Addr >> 8U);
        IMX383SpiBuf[0][2] = (UINT8)(Addr & 0xffU);

        if (pTxData != NULL) {
            for (i = 0U; i < Size; i++) {
                IMX383SpiBuf[0][i + Size0] = pTxData[i];
                /* for debugging */
                //AmbaPrint_PrintUInt5("[IMX383] CID=0x%02x, Addr=0x%04x, Data=0x%02x", CID, Addr + i, pTxData[i], 0U, 0U);
                //AmbaPrint_Flush();
            }
        }

        RetVal = AmbaSPI_MasterTransferD8(MasterID, SlaveMask, &IMX383SpiConfig, Size + Size0, IMX383SpiBuf[0], IMX383SpiBuf[1], NULL, 500U);

        if (pRxData != NULL) {
            for (i = 0U; i < Size; i++) {
                pRxData[i] = IMX383SpiBuf[1][i + Size0];
            }
        }
    }

    return RetVal;
}

static UINT32 RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    return IMX383_SerialComm(pChan, 0x81U, Addr, pTxData, NULL, Size, 10000000U);
}

static UINT32 RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    return IMX383_SerialComm(pChan, 0x82U, Addr, NULL, pRxData, Size, 5000000U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_RegisterWrite
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
static UINT32 IMX383_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT16 Addr, const UINT16 Data)
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
 *  @RoutineName:: IMX383_RegisterRead
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
static UINT32 IMX383_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT16 Addr, UINT16 *pData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(pChan, Addr, &RData, 1U);
        *pData = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_SetStandbyOn
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
static UINT32 IMX383_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    UINT8 WData = 0x13U;

    RetVal = RegWrite(pChan, IMX383_OP_REG, &WData, 1U);

    return RetVal;
}

static UINT32 IMX383_SetPlusTimingRegs(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE;
    UINT8 WData;

    AmbaPrint_PrintUInt5("[IMX383] Set PLSTMG settings - BEGIN", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U ; i < IMX383_NUM_PLSTMG_REG; i++) {
        WData = IMX383PlstmgRegTable[i].Data;
        RetVal |= RegWrite(pChan,  IMX383PlstmgRegTable[i].Addr, &WData, 1U);
    }
    AmbaPrint_PrintUInt5("[IMX383] Set PLSTMG settings - END", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

static UINT32 IMX383_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE, i;
    UINT8 WData;

    AmbaPrint_PrintUInt5("[IMX383] Set Mode settings - BEGIN", 0U, 0U, 0U, 0U, 0U);
    for (i = 0; i < IMX383_NUM_READOUT_MODE_REG; i ++) {
        WData = IMX383ModeRegTable[i].Data[ModeID];
        RetVal |= RegWrite(pChan, IMX383ModeRegTable[i].Addr, &WData, 1U);
    }
    AmbaPrint_PrintUInt5("[IMX383] Set Mode settings - END", 0U, 0U, 0U, 0U, 0U);

    IMX383Ctrl.CurrentAgcCtrl[0] = 0xffffffffU;
    IMX383Ctrl.CurrentAgcCtrl[1] = 0xffffffffU;
    IMX383Ctrl.CurrentDgcCtrl[0] = 0xffffffffU;
    IMX383Ctrl.CurrentDgcCtrl[1] = 0xffffffffU;
    IMX383Ctrl.CurrentShrCtrl[0] = 0xffffffffU;
    IMX383Ctrl.CurrentShrCtrl[1] = 0xffffffffU;

    return RetVal;
}

static UINT32 IMX383_SetAttrRegs(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE;
    UINT8 WData;

    for (i = 0U ; i < IMX383_NUM_ATTR_REG; i++) {
        WData = IMX383AttrRegTable[i].Data;
        RetVal |= RegWrite(pChan,  IMX383AttrRegTable[i].Addr, &WData, 1U);
    }

    return RetVal;
}

static UINT32 IMX383_PerformStandbyCancelSequence(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData;

    RetVal |= IMX383_SetPlusTimingRegs(pChan);
    RetVal |= IMX383_ChangeReadoutMode(pChan, ModeID);
    RetVal |= IMX383_SetAttrRegs(pChan);

    /* -------------------------------------------------- */
    /* 1. After performing the power-on start-up sequence */
    /* -------------------------------------------------- */
    // 1-1.
    WData = 0x0FU;
    RetVal |= RegWrite(pChan, IMX383_STBPL_REG, &WData, 1);
    // 1-2.
    WData = 0x82U;
    RetVal |= RegWrite(pChan, IMX383_OP_REG, &WData, 1);

    (void)AmbaKAL_TaskSleep(10);
    /* -------------------------------------------------------- */
    /* 2. After the 1st stabilization period of 10.0 ms or more */
    /* -------------------------------------------------------- */
    // 2-1.
    WData = 0x00U;
    RetVal |= RegWrite(pChan, IMX383_STBPL_REG, &WData, 1);


    (void)AmbaKAL_TaskSleep(1);
    /* ------------------------------------------------------- */
    /* 3. After the 2st stabilization period of 0.2 ms or more */
    /* ------------------------------------------------------- */
    // 3-1.
    WData = 0x80U;
    RetVal |= RegWrite(pChan, IMX383_OP_REG, &WData, 1);

    (void)AmbaKAL_TaskSleep(7);
    /* ------------------------------------------------------- */
    /* 4. After the 3rd stabilization period of 6.8 ms or more */
    /* ------------------------------------------------------- */
    // 4-1. Finish all the PLSTMG settings and mode settings.
    //      (These initialize communcation can precede above
    //       standby cancel registers 0000h or 04EBh)

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX383_PreCalculateModeInfo(void)
{
    UINT32 i;

    for (i = 0U; i < IMX383_NUM_MODE; i++) {
        IMX383ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX383ModeInfoList[i].FrameTiming.NumTickPerXhs *
                                                (DOUBLE)IMX383ModeInfoList[i].FrameTiming.NumXhsPerH /
                                                (DOUBLE)IMX383ModeInfoList[i].FrameTiming.InputClk);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_Init
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
static UINT32 IMX383_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const IMX383_FRAME_TIMING_s *pFrameTime = &IMX383ModeInfoList[0U].FrameTiming;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        IMX383_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_Enable
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
static UINT32 IMX383_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX383_HardwareReset(pChan->VinID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_Disable
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
static UINT32 IMX383_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX383_SetStandbyOn(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_GetStatus
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
static UINT32 IMX383_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &IMX383Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_GetModeInfo
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
static UINT32 IMX383_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = IMX383Ctrl.Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= IMX383_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            IMX383_PrepareModeInfo(pMode, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_GetDeviceInfo
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
static UINT32 IMX383_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &IMX383DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_GetHdrInfo
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
static UINT32 IMX383_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pHdrInfo, &IMX383Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_GetCurrentGainFactor
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
static UINT32 IMX383_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
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
        if (IMX383Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            AnalogGainFactor = 1024.0 / ( 1024.0 - (FLOAT)IMX383Ctrl.CurrentAgcCtrl[0]);
            DigitalGainFactor = (FLOAT)IMX383Ctrl.CurrentDgcCtrl[0] / 256.0;
            *pGainFactor = AnalogGainFactor * DigitalGainFactor;

        } else if (IMX383Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (IMX383Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                AnalogGainFactor = 1024.0 / ( 1024.0 - (FLOAT)IMX383Ctrl.CurrentAgcCtrl[i]);
                DigitalGainFactor = (FLOAT)IMX383Ctrl.CurrentDgcCtrl[i] / 256.0;
                pGainFactor[i] = AnalogGainFactor * DigitalGainFactor;
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
#endif
        *pGainFactor = 1.0f;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_GetCurrentShutterSpeed
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
static UINT32 IMX383_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX383Ctrl.Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT8 i;

    UINT32 RetVal = SENSOR_ERR_NONE;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX383Ctrl.CurrentShrCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX383Ctrl.CurrentShrCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

#if 0
static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    if (DesiredFactor < 1.0) { /* 1.0 = IMX383DeviceInfo.MinAnalogGainFactor * IMX383DeviceInfo.MinDigitalGainFactor */
        *pAnalogGainCtrl = 0U;
        *pDigitalGainCtrl = 0x100U;

        *pActualFactor = 1.0f;
    } else {
        FLOAT AnalogGainFactor;
        FLOAT DigitalGainFactor;

        DigitalGainFactor = DesiredFactor / IMX383DeviceInfo.MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0) {
            *pDigitalGainCtrl = 0x100U;
            DigitalGainFactor = 1.0f;

            AnalogGainFactor = DesiredFactor;
        } else {
            if (DigitalGainFactor > IMX383DeviceInfo.MaxDigitalGainFactor) {
                DigitalGainFactor = IMX383DeviceInfo.MaxDigitalGainFactor;
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
 *  @RoutineName:: IMX383_ConvertGainFactor
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
static UINT32 IMX383_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s* pModeInfo = &IMX383Ctrl.Status.ModeInfo;
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
 *  @RoutineName:: IMX383_ConvertShutterSpeed
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
static UINT32 IMX383_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX383Ctrl.Status.ModeInfo;
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
 *  @RoutineName:: IMX383_SetAnalogGainCtrl
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
static UINT32 IMX383_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX383Ctrl.Status.ModeInfo;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            /* TODO */

            /* Update current AGC control */
            IMX383Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            /* TODO */

            /* Update current AGC control */
            IMX383Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
            IMX383Ctrl.CurrentAgcCtrl[1] = pAnalogGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_SetDigitalGainCtrl
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
static UINT32 IMX383_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX383Ctrl.Status.ModeInfo;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            /* TODO */

            /* Update current AGC control */
            IMX383Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            /* TODO */

            /* Update current AGC control */
            IMX383Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
            IMX383Ctrl.CurrentDgcCtrl[1] = pDigitalGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_SetShutterCtrl
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
static UINT32 IMX383_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX383Ctrl.Status.ModeInfo;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            /* TODO */

            /* Update current shutter control */
            IMX383Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            /* TODO */

            /* Update current shutter control */
            IMX383Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
            IMX383Ctrl.CurrentShrCtrl[1] = pShutterCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_SetSlowShutterCtrl
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
static UINT32 IMX383_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = IMX383Ctrl.Status.ModeInfo.Config.ModeID;
    // UINT32 TargetFrameLengthLines;

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX383Ctrl.Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        // TargetFrameLengthLines = IMX383ModeInfoList[ModeID].FrameTiming.FrameLengthLines * SlowShutterCtrl;
        /* TODO */

        /* Update frame rate information */
        // IMX383Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        // IMX383Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        IMX383Ctrl.Status.ModeInfo.FrameRate.TimeScale = IMX383ModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;
    }

    return RetVal;
}

static UINT32 IMX383_PerformSlvsEcPhyCal(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig)
{
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX383Ctrl.Status.ModeInfo;
    const IMX383_FRAME_TIMING_s *pFrameTime = &IMX383ModeInfoList[pModeInfo->Config.ModeID].FrameTiming;
    UINT32 CortexClk, Delay, RetVal = SENSOR_ERR_NONE;
    DOUBLE WorkDouble;

    (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_CORTEX, &CortexClk);
    WorkDouble = (DOUBLE) pFrameTime->NumTickPerXhs / (DOUBLE) pFrameTime->InputClk;
    WorkDouble *= CortexClk;
    WorkDouble *= 20;
    WorkDouble += (910.22 * 1e-6) * CortexClk; /* low period */
    Delay = (UINT32) WorkDouble;

    AmbaPrint_PrintUInt5("[IMX383] Enable VIN Master sync", 0U, 0U, 0U, 0U, 0U);
    IMX383_ConfigMasterSync(pFrameTime);
    AmbaDelayCycles(Delay);

    if (AmbaVIN_SlvsEcCalib(pChan->VinID, pPadConfig) != OK) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    AmbaPrint_PrintUInt5("[IMX383] Phy setup is done (delay %u/%u seconds)", Delay, CortexClk, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX383_Config
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
static UINT32 IMX383_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX383Ctrl.Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    // const IMX383_FRAME_TIMING_s *pFrameTime = &IMX383ModeInfoList[ModeID].FrameTiming;
    AMBA_VIN_SLVSEC_PAD_CONFIG_s PadConfig = {0};

    if (ModeID >= IMX383_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_PrintUInt5("[IMX383] Config Mode: %d", ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        IMX383_PrepareModeInfo(pMode, pModeInfo);

        RetVal |= AmbaVIN_MasterSyncDisable(AMBA_VIN_MSYNC0);

        IMX383_GetPadConfig(pChan->VinID, pMode, &PadConfig);

        /* set pll */
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);

        (void)IMX383_HardwareReset(pChan->VinID);

        RetVal |= AmbaVIN_SlvsEcReset(pChan->VinID, &PadConfig);

        (void)IMX383_PerformStandbyCancelSequence(pChan, ModeID);

        RetVal |= IMX383_PerformSlvsEcPhyCal(pChan, &PadConfig);

        /* config vin */
        RetVal |= IMX383_ConfigVin(pChan->VinID, pModeInfo);

        AmbaPrint_PrintUInt5("[IMX383] Sensor Config Done", 0U, 0U, 0U, 0U, 0U);
    }
    return RetVal;
}

#if 0
static UINT32 IMX383_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX383Obj = {
    .SensorName             = "IMX383",
    .SerdesName             = "NA",
    .Init                   = IMX383_Init,
    .Enable                 = IMX383_Enable,
    .Disable                = IMX383_Disable,
    .Config                 = IMX383_Config,
    .GetStatus              = IMX383_GetStatus,
    .GetModeInfo            = IMX383_GetModeInfo,
    .GetDeviceInfo          = IMX383_GetDeviceInfo,
    .GetHdrInfo             = IMX383_GetHdrInfo,
    .GetCurrentGainFactor   = IMX383_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX383_GetCurrentShutterSpeed,

    .ConvertGainFactor      = NULL, /* IMX383_ConvertGainFactor */
    .ConvertShutterSpeed    = NULL, /* IMX383_ConvertShutterSpeed */

    .SetAnalogGainCtrl      = IMX383_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX383_SetDigitalGainCtrl,
    .SetWbGainCtrl          = NULL,
    .SetShutterCtrl         = IMX383_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX383_SetSlowShutterCtrl,

    .RegisterRead           = IMX383_RegisterRead,
    .RegisterWrite          = IMX383_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
