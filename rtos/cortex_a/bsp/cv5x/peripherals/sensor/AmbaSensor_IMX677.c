/**
 *  @file AmbaSensor_IMX677.c
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details Control APIs of SONY IMX677 CMOS sensor with SLVS-EC interface
 *
 */

#include <AmbaWrap.h>
#include <AmbaMisraFix.h>

#include "AmbaVIN.h"

#include "AmbaSPI.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX677.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSYS.h"
#include "AmbaDef.h"
#include "bsp.h"

#if 0
static UINT32 IMX677_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * IMX677 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX677_CTRL_s IMX677Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_GetPadConfig
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
static void IMX677_GetPadConfig(UINT32 VinID, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX677OutputInfo[ModeID];
    UINT8 DualLink                                      = IMX677ModeInfoList[ModeID].DualLink;
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
 *  @RoutineName:: IMX677_PrepareModeInfo
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
static void IMX677_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &IMX677InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX677OutputInfo[ModeID];
    const IMX677_FRAME_TIMING_s         *pFrameTiming   = &IMX677ModeInfoList[ModeID].FrameTiming;
    DOUBLE WorkDouble;
    UINT32 U32RVal = 0U;

    U32RVal = AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    WorkDouble = (DOUBLE)pFrameTiming->NumTickPerXhs * (DOUBLE)pFrameTiming->NumXhsPerH;
    WorkDouble *= (DOUBLE)pOutputInfo->DataRate / (DOUBLE)pFrameTiming->InputClk;
    WorkDouble *= (DOUBLE)pOutputInfo->NumDataLanes / (DOUBLE)pOutputInfo->NumDataBits;
    pModeInfo->LineLengthPck = (UINT32) WorkDouble;

    WorkDouble = (DOUBLE)pFrameTiming->NumXhsPerV * (DOUBLE)pFrameTiming->NumXvsPerV;
    WorkDouble /= (DOUBLE)pFrameTiming->NumXhsPerH;
    pModeInfo->FrameLengthLines = (UINT32)WorkDouble;

    pModeInfo->NumExposureStepPerFrame = pFrameTiming->NumXhsPerV * pFrameTiming->NumXvsPerV;
    pModeInfo->InputClk                = pFrameTiming->InputClk;
    pModeInfo->RowTime                 = IMX677ModeInfoList[ModeID].RowTime;
    pModeInfo->InternalExposureOffset  = 0.0f;
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(const AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX677HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }

    if (U32RVal != 0U) {
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_ConfigMasterSync
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
static void IMX677_ConfigMasterSync(const IMX677_FRAME_TIMING_s *pFrameTime)
{
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg = {
        .RefClk = 72000000U,
        .HSync = {
            .Period     = 0U,
            .PulseWidth = 16U,
            .Polarity   = 0U
        },
        .VSync = {
            .Period     = 0U,
            .PulseWidth = 8U + ((UINT32)pFrameTime->NumTickPerXhs >> 1U),
            .Polarity   = 0U
        },
        .HSyncDelayCycles = 4U,
        .VSyncDelayCycles = 0U,
        .ToggleHsyncInVblank = 1U
    };

    MasterSyncCfg.RefClk= pFrameTime->InputClk;
    MasterSyncCfg.HSync.Period = pFrameTime->NumTickPerXhs;
    MasterSyncCfg.VSync.Period = pFrameTime->NumXhsPerV / pFrameTime->NumXvsPerV;

    (void) AmbaVIN_MasterSyncEnable(AMBA_VIN_MSYNC0, &MasterSyncCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_ConfigVin
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
static UINT32 IMX677_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    AMBA_VIN_SLVSEC_CONFIG_s IMX677VinConfig = {
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
    AMBA_VIN_SLVSEC_CONFIG_s *pVinCfg            = &IMX677VinConfig;
    UINT8 DualLink                                = IMX677ModeInfoList[pModeInfo->Config.ModeID].DualLink;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes = pModeInfo->OutputInfo.NumDataLanes;
    pVinCfg->LinkType = (DualLink == 0U) ? AMBA_VIN_SLVSEC_LINK_TYPE_S: AMBA_VIN_SLVSEC_LINK_TYPE_D;

    if (AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        /* MisraC */
    }

    RetVal |= AmbaVIN_SlvsEcConfig(VinID, pVinCfg);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_HardwareReset
 *
 *  @Description:: Reset IMX677 Image Sensor Device
 *
 *  @Input      ::
 *      VinID:      Vin channel ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX677_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);
    /* de-assert reset pin */
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(10);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_SerialComm
 *
 *  @Description:: Read/Write sensor registers through SPI bus
 *
 *  @Input      ::
 *      pChan:      pointer to sensor channel ID
 *      CID:        Chip ID
 *      Addr:       Start Address
 *      pTxData:    Pointer to Write data buffer
 *      Size:       Number of Read/Write data
 *      BaudRate:   Desired baudrate
 *
 *  @Output     ::
 *      pRxData:    Pointer to Read data buffer
 *
 *  @Return     ::
 *          UINT32 : Error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX677_SerialComm(const AMBA_SENSOR_CHANNEL_s *pChan, UINT8 CID, UINT16 Addr, const UINT8 *pTxData, UINT8 *pRxData, UINT32 Size, UINT32 BaudRate)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE, Size0 = 3U;
    UINT32 MasterID, SlaveMask;
    UINT8 IMX677SpiBuf[2][128];
    UINT32 BufSize = (UINT32)(sizeof(IMX677SpiBuf[0]) / sizeof(UINT8));
    /*-----------------------------------------------------------------------------------------------*\
     * SPI configuration, {BaudRate, CsPolarity, ClkMode, ClkStretch, DataFrmSize, FrameBitOrder}
    \*-----------------------------------------------------------------------------------------------*/
    AMBA_SPI_CONFIG_s IMX677SpiConfig = {
        .BaudRate      = BaudRate,                          /* Transfer BaudRate in Hz */
        .CsPolarity    = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,   /* Slave select polarity */
        .ClkMode       = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,      /* SPI Protocol mode */
        .ClkStretch    = 0U,                                /* Stretch the duration (us) from the active edge of chip select to the first edge of clock */
        .DataFrameSize = 8U,                                /* Data Frame Size in Bit */
        .FrameBitOrder = AMBA_SPI_TRANSFER_LSB_FIRST,       /* Bit transfer order */
    };

    AmbaMisra_TouchUnused(&pChan);

    if ((Size + Size0) > BufSize) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        MasterID = AMBA_SENSOR_SPI_CHANNEL_SLVSEC;
#if AMBA_SENSOR_SPI_SLAVE_ID_SLVSEC == 0U
        SlaveMask = 1U;
#else
        SlaveMask = (UINT32)1U << AMBA_SENSOR_SPI_SLAVE_ID_SLVSEC;
#endif
        IMX677SpiBuf[0][0] = CID;
        IMX677SpiBuf[0][1] = (UINT8)(Addr >> 8U);
        IMX677SpiBuf[0][2] = (UINT8)(Addr & 0xffU);

        if (pTxData != NULL) {
            for (i = 0U; i < Size; i++) {
                IMX677SpiBuf[0][i + Size0] = pTxData[i];
                /* for debugging */
                //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID("[IMX677] CID=0x%02x, Addr=0x%04x, Data=0x%02x", CID, Addr + i, pTxData[i], 0U, 0U);
                //AmbaPrint_Flush();
            }
        }

        RetVal = AmbaSPI_MasterTransferD8(MasterID, SlaveMask, &IMX677SpiConfig, Size + Size0, IMX677SpiBuf[0], IMX677SpiBuf[1], NULL, 500U);

        if (pRxData != NULL) {
            for (i = 0U; i < Size; i++) {
                pRxData[i] = IMX677SpiBuf[1][i + Size0];
            }
        }
    }

    return RetVal;
}

static UINT32 RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    return IMX677_SerialComm(pChan, 0x81U, Addr, pTxData, NULL, Size, 10000000U);
}

static UINT32 RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    return IMX677_SerialComm(pChan, 0x82U, Addr, NULL, pRxData, Size, 5000000U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_RegisterWrite
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
static UINT32 IMX677_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
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
 *  @RoutineName:: IMX677_RegisterRead
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
static UINT32 IMX677_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
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
 *  @RoutineName:: IMX677_SetStandbyOn
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
static UINT32 IMX677_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    UINT8 WData = 0x01U;

    RetVal = RegWrite(pChan, IMX677_STB_AUTO_REG, &WData, 1U);

    return RetVal;
}

static UINT32 IMX677_SetPlusTimingRegs(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE;
    UINT8 WData;

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX677] Set PLSTMG settings - BEGIN", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U ; i < IMX677_NUM_PLSTMG_REG; i++) {
        WData = IMX677PlstmgRegTable[i].Data;
        RetVal |= RegWrite(pChan,  IMX677PlstmgRegTable[i].Addr, &WData, 1U);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX677] Set PLSTMG settings - END", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

static UINT32 IMX677_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    const IMX677_FRAME_TIMING_s *pFrameTime = &IMX677ModeInfoList[ModeID].FrameTiming;
    UINT32 RetVal = SENSOR_ERR_NONE, i, DLR_HsWidth;
    UINT8 WData;

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX677] Set Mode settings - BEGIN", 0U, 0U, 0U, 0U, 0U);
    for (i = 0; i < IMX677_NUM_READOUT_MODE_REG; i ++) {
        WData = IMX677ModeRegTable[i].Data[ModeID];
        RetVal |= RegWrite(pChan, IMX677ModeRegTable[i].Addr, &WData, 1U);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX677] Set Mode settings - END", 0U, 0U, 0U, 0U, 0U);

    /* Set Internal pulse insert setting according to XHS period [XHS] */
    if ((IMX677ModeRegTable[0].Data[ModeID] == 5U) || (IMX677ModeRegTable[0].Data[ModeID] == 8U)) {
        DLR_HsWidth = pFrameTime->NumTickPerXhs * 2U;
    } else {
        DLR_HsWidth = pFrameTime->NumTickPerXhs;
    }

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX677] Set DLR_HSWIDTH register: %u", DLR_HsWidth, 0U, 0U, 0U, 0U);
    WData = (UINT8)(DLR_HsWidth & 0xffU);
    RetVal |= RegWrite(pChan, IMX677_DLR_HSWIDTH_LSB_REG, &WData, 1);
    WData = (UINT8)((DLR_HsWidth & 0xf00U) >> 8U);
    RetVal |= RegWrite(pChan, IMX677_DLR_HSWIDTH_MSB_REG, &WData, 1);

    IMX677Ctrl.CurrentAgcCtrl[0] = 0xffffffffU;
    IMX677Ctrl.CurrentAgcCtrl[1] = 0xffffffffU;
    IMX677Ctrl.CurrentDgcCtrl[0] = 0xffffffffU;
    IMX677Ctrl.CurrentDgcCtrl[1] = 0xffffffffU;
    IMX677Ctrl.CurrentShrCtrl[0] = 0xffffffffU;
    IMX677Ctrl.CurrentShrCtrl[1] = 0xffffffffU;

    return RetVal;
}

static UINT32 IMX677_SetAttrRegs(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i, RetVal = SENSOR_ERR_NONE;
    UINT8 WData;

    for (i = 0U ; i < IMX677_NUM_ATTR_REG; i++) {
        WData = IMX677AttrRegTable[i].Data;
        RetVal |= RegWrite(pChan,  IMX677AttrRegTable[i].Addr, &WData, 1U);
    }

    return RetVal;
}

static UINT32 IMX677_PerformStandbyCancelSequence(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 BaudGrade = (IMX677Ctrl.Status.ModeInfo.OutputInfo.DataRate <= 2500000000ULL) ? 2U : 3U;
    UINT8 WData;

    /* SDO_ACT register = 1 */
    WData = 0x01U;
    RetVal |= RegWrite(pChan, IMX677_SDO_ACT_REG, &WData, 1);

    /* ----------------------------------------------------- */
    /* 1. After setting XCLR to high level for 10 ms or more */
    /* ----------------------------------------------------- */
    // Initialize communcation
    //   Set all registers of PLSTMG setting (1) and (2) in "Readout Drive Pulse Timing".
    RetVal |= IMX677_SetPlusTimingRegs(pChan);

    // - Set address 0054h to "00h" (STB_AUTO register = 0h).
    WData = 0x00U;
    RetVal |= RegWrite(pChan, IMX677_STB_AUTO_REG, &WData, 1);
    // - Set address 1A0Bh to "00h" or "01h" (FREQ register = 0h or 1h).
    WData = (BaudGrade == 2U) ? 0x01U : 0x00U;
    RetVal |= RegWrite(pChan, IMX677_FREQ_REG, &WData, 1);
    // - Set address 07E1h to "00h" or "01h" (FREQ_PLL register = 0h or 1h).
    WData = (BaudGrade == 2U) ? 0x01U : 0x00U;
    RetVal |= RegWrite(pChan, IMX677_FREQ_PLL_REG, &WData, 1);

    // CRC_ECC_MODE
    //WData = 0x00U;
    //RetVal |= RegWrite(pChan, 0x1F0A, &WData, 1);

    (void)AmbaKAL_TaskSleep(3U);

    /* --------------------------------------------------------------- */
    /* 2. After the 1st analog stabilization period for 2.2ms or more. */
    /* --------------------------------------------------------------- */
    // - Set address 0002h to "01h" (WAKEUP register = 1h) during the register communcation period before V1 period.
    WData = 0x01U;
    RetVal |= RegWrite(pChan, IMX677_WAKEUP_REG, &WData, 1);
    // - Set address 1900h to "01h" (CLPSQRST register = 1h) during the register communcation period before V1 period.
    WData = 0x01U;
    RetVal |= RegWrite(pChan, IMX677_CLPSQRST_REG, &WData, 1);
    // Readout drive mode registers
    //   Set the mode registers according to the separate document "IMX677-AAPH5-C Initial Register Setting".
    //   Furthermore, set the required shutter and gain registers.
    RetVal |= IMX677_ChangeReadoutMode(pChan, ModeID);
    RetVal |= IMX677_SetAttrRegs(pChan);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX677_PreCalculateModeInfo(void)
{
    UINT32 i;

    for (i = 0U; i < IMX677_NUM_MODE; i++) {
        IMX677ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX677ModeInfoList[i].FrameTiming.NumTickPerXhs *
                                                (DOUBLE)IMX677ModeInfoList[i].FrameTiming.NumXhsPerH /
                                                (DOUBLE)IMX677ModeInfoList[i].FrameTiming.InputClk);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_Init
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
static UINT32 IMX677_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const IMX677_FRAME_TIMING_s *pFrameTime = &IMX677ModeInfoList[0U].FrameTiming;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        IMX677_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_Enable
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
static UINT32 IMX677_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX677_HardwareReset(pChan->VinID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_Disable
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
static UINT32 IMX677_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX677_SetStandbyOn(pChan);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_GetStatus
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
static UINT32 IMX677_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &IMX677Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_GetModeInfo
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
static UINT32 IMX677_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s DesiredMode;

    if ((pChan == NULL) || (pMode == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(&DesiredMode, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) == ERR_NONE) {
            if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
                DesiredMode.ModeID = IMX677Ctrl.Status.ModeInfo.Config.ModeID;
            }

            if ((DesiredMode.ModeID >= IMX677_NUM_MODE) ||
                (pModeInfo == NULL)) {
                RetVal = SENSOR_ERR_ARG;
            } else {

                IMX677_PrepareModeInfo(&DesiredMode, pModeInfo);
            }
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_GetDeviceInfo
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
static UINT32 IMX677_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pDeviceInfo, &IMX677DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_GetHdrInfo
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
static UINT32 IMX677_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pHdrInfo, &IMX677Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

static FLOAT GetDigitalGain(UINT32 SettingVal)
{
    FLOAT Factor;

    /* 0h: 0[dB] / 1h: 6[dB] / 2h: 12[dB] / 3h: 18[dB] */
    if (SettingVal == 0U) {
        Factor = 1.0f;
    } else if (SettingVal == 1U) {
        Factor = 1.99526231496888f;
    } else if (SettingVal == 2U) {
        Factor = 3.98107170553497f;
    } else if (SettingVal == 3U) {
        Factor = 7.94328234724282f;
    } else {
        Factor = 1.0f;
    }

    return Factor;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_GetCurrentGainFactor
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
static UINT32 IMX677_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT AnalogGainFactor = 0.0f;
    FLOAT DigitalGainFactor = 0.0f;

    if ((pChan == NULL) || (pGainFactor == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (IMX677Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX677Ctrl.CurrentAgcCtrl[0]);
            DigitalGainFactor = GetDigitalGain(IMX677Ctrl.CurrentDgcCtrl[0]);
            *pGainFactor = AnalogGainFactor * DigitalGainFactor;
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_GetCurrentShutterSpeed
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
static UINT32 IMX677_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX677Ctrl.Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;

    UINT32 RetVal = SENSOR_ERR_NONE;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX677Ctrl.CurrentShrCtrl[0];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

static UINT32 ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    UINT32 i;

    if (DesiredFactor < 1.0f) { /* 1.0 = IMX677DeviceInfo.MinAnalogGainFactor * IMX677DeviceInfo.MinDigitalGainFactor */
        *pAnalogGainCtrl = 0U;
        *pDigitalGainCtrl = 0U;
        *pActualFactor = 1.0f;
    } else {
        FLOAT DigitalGainFactor;
        FLOAT AnalogGainFactor;
        FLOAT WorkFLOAT;

        DigitalGainFactor = DesiredFactor / IMX677DeviceInfo.MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0f) {
            *pDigitalGainCtrl = 0U;
            AnalogGainFactor = DesiredFactor;
        } else {
            for (i = 0U; i < IMX677_MAX_DGAIN; i++) {
                if (DigitalGainFactor < GetDigitalGain(i)) {
                    break;
                }
            }
            *pDigitalGainCtrl = i;
            DigitalGainFactor = GetDigitalGain(i);
            AnalogGainFactor = DesiredFactor / DigitalGainFactor;
        }

        WorkFLOAT = (1024.0f - (1024.0f / AnalogGainFactor));
        *pAnalogGainCtrl = (UINT32)WorkFLOAT;

        if (*pAnalogGainCtrl > IMX677_MAX_APGC) {
            *pAnalogGainCtrl = IMX677_MAX_APGC;
        }

        *pActualFactor = GetDigitalGain(*pDigitalGainCtrl);
        *pActualFactor *= 1024.0f / (1024.0f - (FLOAT)*pAnalogGainCtrl);
    }

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_ConvertGainFactor
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
static UINT32 IMX677_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &IMX677Ctrl.Status.ModeInfo;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = ConvertGainFactor(pDesiredFactor->Gain[0], &(pActualFactor->Gain[0]), &(pGainCtrl->AnalogGain[0]), &(pGainCtrl->DigitalGain[0]));
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
static UINT32 ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pModeInfo->Config.ModeID;
    const IMX677_FRAME_TIMING_s *pFrameTiming = &IMX677ModeInfoList[ModeID].FrameTiming;
    UINT32 NumExposureStepPerFrame = pFrameTiming->NumXhsPerV * pFrameTiming->NumXvsPerV;
    UINT32 MaxExposureLine, MinExposureLine; /* in unit of XHS period */
    UINT32 ExposureLineMargin = IMX677ModeInfoList[ModeID].MinSHR;
    UINT32 ShutterCtrl;
    DOUBLE ShutterTimeUnit = (DOUBLE)IMX677ModeInfoList[ModeID].RowTime / pFrameTiming->NumXhsPerH;
    DOUBLE ShutterCtrlInDb;

    AmbaMisra_TouchUnused(&HdrChannel);

    if (AmbaWrap_floor((DOUBLE)ExposureTime / ShutterTimeUnit, &ShutterCtrlInDb) == ERR_NONE) {
        ShutterCtrl = (UINT32)ShutterCtrlInDb;

        /* SVR is 16 bits */
        if (ShutterCtrl > ((0xffffU + 1U) * NumExposureStepPerFrame)) {
            ShutterCtrl = ((0xffffU + 1U) * NumExposureStepPerFrame);
        }

        if (ShutterCtrl == 0U) {
            MaxExposureLine = NumExposureStepPerFrame;
        } else {
            MaxExposureLine = (((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame;
        }
        MaxExposureLine -= ExposureLineMargin;
        MinExposureLine = 1U;

        if (ShutterCtrl > MaxExposureLine) {
            ShutterCtrl = MaxExposureLine;
        } else if (ShutterCtrl < MinExposureLine) {
            ShutterCtrl = MinExposureLine;
        } else {
            /* do not need to adjust ShutterCtrl */
        }

        *pShutterCtrl   = ShutterCtrl;
        *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
    } else {
        *pShutterCtrl   = 0U;
        *pActualExptime = 0.0f;
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_ConvertShutterSpeed
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
static UINT32 IMX677_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX677Ctrl.Status.ModeInfo;

    if ((pChan == NULL) || (pDesiredExposureTime == NULL) || (pActualExposureTime == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_SetAnalogGainCtrl
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
static UINT32 IMX677_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX677Ctrl.Status.ModeInfo;
    UINT8 WData[2U];

    if ((pChan == NULL) || (pAnalogGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            WData[0] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            WData[1] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x0003U);
            RetVal = RegWrite(pChan, IMX677_APGC_LT_LSB_REG, &WData[0], 2);

            /* Update current AGC control */
            IMX677Ctrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_SetDigitalGainCtrl
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
static UINT32 IMX677_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX677Ctrl.Status.ModeInfo;
    UINT8 WData;

    if ((pChan == NULL) || (pDigitalGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            WData = (UINT8)(pDigitalGainCtrl[0] & 0x000fU);
            RetVal = RegWrite(pChan, IMX677_DGAIN_LT_REG, &WData, 1);

            /* Update current AGC control */
            IMX677Ctrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_SetShutterCtrl
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
static UINT32 IMX677_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = IMX677Ctrl.Status.ModeInfo.Config.ModeID;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX677Ctrl.Status.ModeInfo;
    const IMX677_FRAME_TIMING_s *pFrameTiming = &IMX677ModeInfoList[ModeID].FrameTiming;
    UINT32 SHRVal;
    UINT8 WData[2U];

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            SHRVal = pFrameTiming->NumXhsPerV;
            SHRVal -= (pShutterCtrl[0] % pFrameTiming->NumXhsPerV);
            WData[0] = (UINT8) (SHRVal & 0x00ffU);
            WData[1] = (UINT8)((SHRVal >> 8U) & 0x00ffU);
            RetVal = RegWrite(pChan, IMX677_SHR_LT_LSB_REG, &WData[0], 2);

            /* Update current shutter control */
            IMX677Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_SetSlowShutterCtrl
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
static UINT32 IMX677_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = IMX677Ctrl.Status.ModeInfo.Config.ModeID;
    const IMX677_FRAME_TIMING_s *pFrameTiming = &IMX677ModeInfoList[ModeID].FrameTiming;
    UINT32 NumExposureStepPerFrame, TargetFrameLengthLines;
    UINT32 SVRVal;
    UINT8 WData[2U];
    DOUBLE WorkDouble;

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX677Ctrl.Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        NumExposureStepPerFrame = ((pFrameTiming->NumXhsPerV * pFrameTiming->NumXvsPerV) * SlowShutterCtrl);
        WorkDouble = ((DOUBLE)NumExposureStepPerFrame / pFrameTiming->NumXhsPerH);
        TargetFrameLengthLines = (UINT32)WorkDouble;

        SVRVal = (SlowShutterCtrl - 1U);
        WData[0] = (UINT8) (SVRVal & 0x00ffU);
        WData[1] = (UINT8)((SVRVal >> 8U) & 0x00ffU);
        RetVal = RegWrite(pChan, IMX677_SVR_LSB_REG, &WData[0], 2);

        /* Update frame rate information */
        IMX677Ctrl.Status.ModeInfo.NumExposureStepPerFrame = NumExposureStepPerFrame;
        IMX677Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        IMX677Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick = IMX677ModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;
    }

    return RetVal;
}

static UINT32 IMX677_PerformSlvsEcPhyCal(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_VIN_SLVSEC_PAD_CONFIG_s *pPadConfig)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX677Ctrl.Status.ModeInfo;
    const IMX677_FRAME_TIMING_s *pFrameTime = &IMX677ModeInfoList[pModeInfo->Config.ModeID].FrameTiming;
    UINT32 BaudGrade = (pModeInfo->OutputInfo.DataRate <= 2500000000ULL) ? 2U : 3U;
    UINT32 CortexClk, Delay, FramePeriod, RetVal = SENSOR_ERR_NONE;
    DOUBLE WorkDouble;

    (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_CORTEX, &CortexClk);
    WorkDouble = (DOUBLE) pFrameTime->NumTickPerXhs / (DOUBLE) pFrameTime->InputClk;
    WorkDouble *= (DOUBLE)CortexClk;
    WorkDouble *= 20.0;
    if (BaudGrade == 2U) {
        WorkDouble += (568.89 * 1e-6) * (DOUBLE)CortexClk; /* low period */
    } else {
        WorkDouble += (284.44 * 1e-6) * (DOUBLE)CortexClk; /* low period */
    }
    Delay = (UINT32) WorkDouble;

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX677] Enable VIN Master sync", 0U, 0U, 0U, 0U, 0U);
    IMX677_ConfigMasterSync(pFrameTime);
    AmbaDelayCycles(Delay);

    /* IMX677 will send one more training sequence after the second XVS pulse input.
     * Wait a frame period to ensure that the low period followed by the last training
     * sequence has been performed before AmbaVIN_SlvsEcCalib() is called. */
    FramePeriod = pFrameTime->FrameRate.NumUnitsInTick * 1000U;
    FramePeriod /= pFrameTime->FrameRate.TimeScale;
    FramePeriod += 1U;
    (void)AmbaKAL_TaskSleep(FramePeriod);

    if (AmbaVIN_SlvsEcCalib(pChan->VinID, pPadConfig) != OK) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX677] Phy setup is done (delay %u/%u seconds)", Delay, CortexClk, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX677_Config
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
static UINT32 IMX677_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX677Ctrl.Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    // const IMX677_FRAME_TIMING_s *pFrameTime = &IMX677ModeInfoList[ModeID].FrameTiming;
    AMBA_VIN_SLVSEC_PAD_CONFIG_s PadConfig = {0};

    if (ModeID >= IMX677_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX677] Config Mode: %d", ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        IMX677_PrepareModeInfo(pMode, pModeInfo);

        RetVal |= AmbaVIN_MasterSyncDisable(AMBA_VIN_MSYNC0);

        IMX677_GetPadConfig(pChan->VinID, pMode, &PadConfig);

        /* set pll */
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);

        (void)IMX677_HardwareReset(pChan->VinID);

        RetVal |= AmbaVIN_SlvsEcReset(pChan->VinID, &PadConfig);

        (void)IMX677_PerformStandbyCancelSequence(pChan, ModeID);

        RetVal |= IMX677_PerformSlvsEcPhyCal(pChan, &PadConfig);

        /* config vin */
        RetVal |= IMX677_ConfigVin(pChan->VinID, pModeInfo);

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX677] Sensor Config Done", 0U, 0U, 0U, 0U, 0U);
    }
    return RetVal;
}

#if 0
static UINT32 IMX677_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX677Obj = {
    .SensorName             = "IMX677",
    .SerdesName             = "NA",
    .Init                   = IMX677_Init,
    .Enable                 = IMX677_Enable,
    .Disable                = IMX677_Disable,
    .Config                 = IMX677_Config,
    .GetStatus              = IMX677_GetStatus,
    .GetModeInfo            = IMX677_GetModeInfo,
    .GetDeviceInfo          = IMX677_GetDeviceInfo,
    .GetHdrInfo             = IMX677_GetHdrInfo,
    .GetCurrentGainFactor   = IMX677_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX677_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX677_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX677_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX677_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX677_SetDigitalGainCtrl,
    .SetWbGainCtrl          = NULL,
    .SetShutterCtrl         = IMX677_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX677_SetSlowShutterCtrl,

    .RegisterRead           = IMX677_RegisterRead,
    .RegisterWrite          = IMX677_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
