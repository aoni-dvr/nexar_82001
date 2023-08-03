/**
 *  [Engineering Version]
 *  @file AmbaSensor_AR0238_PARALLEL.c
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
 *  @details Control APIs of Aptina AR0238_PARALLEL CMOS sensor with HiSPi interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_AR0238_PARALLEL.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
//#include "bsp.h"


/*-----------------------------------------------------------------------------------------------*\
 * AR0238_PARALLEL sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static AR0238_PARALLEL_CTRL_s AR0238_PARALLELCtrl = {0};



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *      pSensorPrivate: Sensor frame time configuration
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void AR0238_PARALLEL_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                         = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s        *pInputInfo     = &AR0238_PARALLELInputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s       *pOutputInfo    = &AR0238_PARALLELOutputInfo[ModeID];
    const AR0238_PARALLEL_FRAME_TIMING_s  *pFrameTiming   = &AR0238_PARALLELModeInfoList[ModeID].FrameTiming;

    (void)AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->LineLengthPck = pFrameTiming->Linelengthpck;
    pModeInfo->FrameLengthLines = pFrameTiming->FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame = pFrameTiming->FrameLengthLines;
    pModeInfo->InputClk = pFrameTiming->InputClk;
    //pModeInfo->PixelRate = AR0238_PARALLELModeInfoList[ModeID].PixelRate;
    pModeInfo->RowTime = AR0238_PARALLELModeInfoList[ModeID].RowTime;

    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    //(void)AmbaWrap_memcpy(&pModeInfo->HdrInfo, &AR0238_PARALLELHdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output frames of the new readout mode
 *
 *  @Input      ::
 *      VinID:      Vin channel number
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_DVP_CONFIG_s AR0238_PARALLELVinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace = AMBA_VIN_COLOR_SPACE_RGB,
            .BayerPattern = AMBA_VIN_BAYER_PATTERN_GR,
            .NumDataBits = 12,
            .NumSkipFrame = 1U,
            .RxHvSyncCtrl = {
                .NumActivePixels = 0,
                .NumActiveLines = 0,
                .NumTotalPixels = 0,
                .NumTotalLines = 0,
            },
            .SplitCtrl = {0},
            .DelayedVsync = 0,
        },
        .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS,
        .DvpWide = AMBA_VIN_DVP_WIDE_1_PIXEL,
        .DataLatchEdge = AMBA_VIN_DVP_LATCH_EDGE_RISING,
        .SyncDetectCtrl = {
            .SyncType = AMBA_VIN_DVP_SYNC_TYPE_BT601,
            .SyncPinConfig = {
                .HsyncPinSelect = AMBA_VIN_DVP_SYNC_PIN_SPCLK_N_0,
                .VsyncPinSelect = AMBA_VIN_DVP_SYNC_PIN_SPCLK_N_1,
                .FieldPinSelect = AMBA_VIN_DVP_SYNC_PIN_SPCLK_N_0,
                .HsyncPolarity = AMBA_VIN_DVP_POLARITY_ACTIVE_HIGH,
                .VsyncPolarity = AMBA_VIN_DVP_POLARITY_ACTIVE_HIGH,
                .FieldPolarity = AMBA_VIN_DVP_POLARITY_ACTIVE_HIGH,
            },
            .EmbSyncConfig = {0},
        },
    };
    const AMBA_SENSOR_OUTPUT_INFO_s  *pOutputInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_DVP_CONFIG_s *pVinCfg = &AR0238_PARALLELVinConfig;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    (void)AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    return AmbaVIN_DvpConfig(VinID, pVinCfg);
}

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
static UINT32 RegWrite(UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[AR0238_PARALLEL_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;

    if (Size > AR0238_PARALLEL_SENSOR_I2C_MAX_SIZE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = AR0238_PARALLEL_SENSOR_I2C_SLAVE_ADDR;
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(AMBA_SENSOR_I2C_CHANNEL, AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        RetVal = AmbaI2C_MasterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_PrintStr5("[AR0238_PARALLEL] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }
    AmbaPrint_PrintUInt5("[AR0238_PARALLEL][IO] Addr = 0x%04x, Data = 0x%02x", Addr, pTxData[0], 0, 0, 0);
    //SENSOR_DBG_IO("[AR0238_PARALLEL][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_RegisterWrite
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
static UINT32 AR0238_PARALLEL_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[2];
    WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = (UINT8)(Data & 0x00ffU);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegWrite(Addr, &WData[0], 2U);
    }

    return RetVal;
}

static UINT32 RegRead(UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = AR0238_PARALLEL_SENSOR_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = ((UINT32)AR0238_PARALLEL_SENSOR_I2C_SLAVE_ADDR | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;

#if 0  /* to be refined with bsp file */
    RetVal = AmbaI2C_MasterReadAfterWrite(AMBA_SENSOR_I2C_CHANNEL, AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);
#endif
    RetVal = AmbaI2C_MasterReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_PrintStr5("[AR0238_PARALLEL] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_RegisterRead
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
static UINT32 AR0238_PARALLEL_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData[2];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(Addr, &RData[0], 2U);
        *Data = (UINT16) (((UINT16)RData[0] << (UINT16)8U) | ((UINT16)RData[1]));
    }

    return RetVal;
}
#if 0
static UINT32 AR0238_PARALLEL_ConfirmStreamingMode(void)
{
    int PollingCnt = 200;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Data = 0xFFFF;

    /* Make sure that sensor is streaming */
    while (1) {
        /* Read frame count */
        RegRead(0x303A, &Data);

        //AmbaPrint("confirm_streaming: frame_count = 0x%04X", Data);

        if (Data != 0xFFFF) {
            break;
        }

        if (PollingCnt < 0) {
            RetVal = SENSOR_ERR_ARG;
            break;
        }
        AmbaKAL_TaskSleep(1);
        PollingCnt--;
    }

    return RetVal;
}

static UINT32 AR0238_PARALLEL_ConfirmStandbyMode(void)
{
    int PollingCnt = 200,
        UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Data = 0xFFFF;

    while (1) {
        /* Read frame status */
        AR0238_PARALLEL_RegRead(0x303C, &Data);

        //AmbaPrint("confirm_standby: R0x303C = 0x%04X", Data);

        if ((Data & 0x0002) && (Data != 0xFFFF)) {
            RetStatus = OK;
            break;
        }

        if (PollingCnt < 0) {
            RetStatus = NG;
            break;
        }
        AmbaKAL_TaskSleep(1);
        PollingCnt--;
    }
    return RetStatus;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_HardwareReset
 *
 *  @Description:: Reset (HW) AR0238_PARALLEL Image Sensor Device
 *
 *  @Input      ::
 *      VinChanNo:          Vin channel number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 AR0238_PARALLEL_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    (void)VinID;
#if 0  /* to be refined with bsp file */
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, 1U);
#endif
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_66, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(2);
    /* de-assert reset pin */
#if 0  /* to be refined with bsp file */
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinID, 0U);
#endif
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_66, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(6);

    AmbaPrint_PrintStr5("Sensor HW Reset", NULL, NULL, NULL, NULL, NULL);
    /*
        if (AR0238_PARALLEL_ConfirmStandbyMode() == NG) {
            AmbaPrint("cannot enter standby mode after HW reset!");
        }
    */
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_SoftwareReset
 *
 *  @Description:: Reset (SW) AR0238_PARALLEL Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_SoftwareReset(void)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;
    UINT8 WData[2];

    /* R0x301A[0]: software reset */
    Addr = 0x301A;
    Data = 0x10D9;
    WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = (UINT8)(Data & 0x00ffU);

    RetVal |= RegWrite(Addr, &WData[0], 2U);

    /* Wait for internal initialization */
    (void)AmbaKAL_TaskSleep(10);

    AmbaPrint_PrintStr5("Sensor SW Reset", NULL, NULL, NULL, NULL, NULL);
    /*if (AR0238_PARALLEL_ConfirmStandbyMode() == -1) {
        AmbaPrint_PrintStr5("cannot enter standby mode after SW reset!", NULL, NULL, NULL, NULL, NULL);
    }*/

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_SetStandbyOn(void)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;
    UINT8 WData[2];

    /* R0x301A[0]: software reset */
    Addr = 0x301A;
    Data = 0x10D8;
    WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = (UINT8)(Data & 0x00ffU);

    RetVal |= RegWrite(Addr, &WData[0], 2U);
    /*
        if (AR0238_PARALLEL_ConfirmStandbyMode() == -1)
            AmbaPrint_PrintStr5("cannot enter standby mode!", NULL, NULL, NULL, NULL, NULL);
    */
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_ConfigLinearSeq
 *
 *  @Description:: Parallel linear initialization: Linear Sequencer.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_ConfigLinearSeq(void)
{
    UINT32 i;
    UINT16 Addr, Data;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[2];

    for (i = 0U; i < AR0238_PARALLEL_NUM_SEQUENCER_LINEAR_REG; i++) {
        Addr = AR0238_PARALLELLinearSeqRegTbl[i].Addr;
        Data = AR0238_PARALLELLinearSeqRegTbl[i].Data;
        WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
        WData[1] = (UINT8)(Data & 0x00ffU);
        RetVal |= RegWrite(Addr, &WData[0], 2U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_ConfigLinearSeq
 *
 *  @Description:: Parallel linear initialization: Default Setting.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_ConfigDefaultSettings(void)
{
    UINT32 i;
    UINT16 Addr, Data;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[2];

    for (i = 0U; i < AR0238_PARALLEL_NUM_DEFAULT_SETTING_REG; i++) {
        Addr = AR0238_PARALLELDefSetRegTable[i].Addr;
        Data = AR0238_PARALLELDefSetRegTable[i].Data;
        WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
        WData[1] = (UINT8)(Data & 0x00ffU);
        RetVal |= RegWrite(Addr, &WData[0], 2U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_ConfigLinearSeq
 *
 *  @Description:: Parallel linear initialization: Parallel Setup.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_ConfigSetup(void)
{
    UINT32 i;
    UINT16 Addr, Data;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData[2];

    for (i = 0U; i < AR0238_PARALLEL_NUM_SETUP_REG; i++) {
        Addr = AR0238_PARALLELSetupRegTable[i].Addr;
        Data = AR0238_PARALLELSetupRegTable[i].Data;
        WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
        WData[1] = (UINT8)(Data & 0x00ffU);
        RetVal |= RegWrite(Addr, &WData[0], 2U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_ChangeReadoutMode(UINT32 ReadoutMode)
{
    UINT32 i;
    UINT16 Addr, Data;
    UINT8 WData[2];
    UINT32 RetVal = SENSOR_ERR_NONE;

    for (i = 0; i < AR0238_PARALLEL_NUM_READOUT_MODE_REG; i++) {
        Addr = AR0238_PARALLELModeRegTable[i].Addr;
        Data = AR0238_PARALLELModeRegTable[i].Data[ReadoutMode];
        WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
        WData[1] = (UINT8)(Data & 0x00ffU);
        RetVal |= RegWrite(Addr, &WData[0], 2U);
    }
    /*
        for(i = 0; i < AR0238_PARALLEL_NUM_READOUT_MODE_REG; i ++) {
            UINT8 Rval = 0;
            AR0238_PARALLEL_RegRead(AR0238_PARALLELRegTable[i].Addr, &Rval);
            AmbaPrint("Reg Read: Addr: 0x%x, data: 0x%x", AR0238_PARALLELRegTable[i].Addr, Rval);
        }
    */
    return RetVal;
}

static UINT32 AR0238_PARALLEL_PerformSequence(UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT16 Addr, Data;
    UINT8 WData[2];

    /* Sensor parallel linear initialization */
    RetVal |=  AR0238_PARALLEL_ConfigLinearSeq();
    RetVal |= AR0238_PARALLEL_ConfigDefaultSettings();
    RetVal |= AR0238_PARALLEL_ConfigSetup();

    Addr = 0x30B0;
    Data = 0x1120;  //0x0118;
    WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = (UINT8)(Data & 0x00ffU);
    RetVal |= RegWrite(Addr, &WData[0], 2U);

    Addr = 0x31AC;
    Data = 0x0C0C;
    WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = (UINT8)(Data & 0x00ffU);
    RetVal |= RegWrite(Addr, &WData[0], 2U);

    Addr = 0x318E;
    //Data = 0x0000;
    WData[0] = 0x00;  //(UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = 0x00;  //(UINT8)(Data & 0x00ffU);
    RetVal |= RegWrite(Addr, &WData[0], 2U);

    Addr = 0x3082;
    Data = 0x0009;
    WData[0] = 0x00;  //(UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = (UINT8)(Data & 0x00ffU);
    RetVal |= RegWrite(Addr, &WData[0], 2U);

    Addr = 0x30BA;
    Data = 0x762C;
    WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = (UINT8)(Data & 0x00ffU);
    RetVal |= RegWrite(Addr, &WData[0], 2U);

    Addr = 0x31D0;
    //Data = 0x0000;
    WData[0] = 0x00;  //(UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = 0x00;  //(UINT8)(Data & 0x00ffU);
    RetVal |= RegWrite(Addr, &WData[0], 2U);

    Addr = 0x30B4;
    Data = 0x0091;
    WData[0] = 0x00;  //(UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = (UINT8)(Data & 0x00ffU);
    RetVal |= RegWrite(Addr, &WData[0], 2U);


    RetVal |= AR0238_PARALLEL_ChangeReadoutMode(ModeID);

    /* Start streaming */
    Addr = 0x301A;
    Data = 0x10DC;
    WData[0] = (UINT8)((Data & 0xff00U) >> 8U);
    WData[1] = (UINT8)(Data & 0x00ffU);
    RetVal |= RegWrite(Addr, &WData[0], 2U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
#if 0
static void AR0238_PARALLEL_PreCalculateModeInfo(void)
{
    UINT32 i;

    for (i = 0U; i < AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE; i++) {
        //AR0238_PARALLELModeInfoList[i].PixelRate = (double)AR0238_PARALLELModeInfoList[i].FrameTiming.InputClk * 44 / 2 / 8;
        AR0238_PARALLELModeInfoList[i].RowTime =
            (double)AR0238_PARALLELModeInfoList[i].FrameTiming.Linelengthpck / ((double)AR0238_PARALLELModeInfoList[i].FrameTiming.InputClk * 44 / 2 / 8);
    }
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_Init
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
static UINT32 AR0238_PARALLEL_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AR0238_PARALLEL_FRAME_TIMING_s *pFrameTime = &AR0238_PARALLELModeInfoList[0U].FrameTiming;

//    AmbaPrint_PrintStr5("AR0238_PARALLEL INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#if 0  /* to be refined with bsp file */
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
#endif
        RetVal = AmbaVIN_SensorClkEnable(AMBA_VIN_SENSOR_CLOCK0, pFrameTime->InputClk);
        /*        AR0238_PARALLEL_PreCalculateModeInfo();  */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= AR0238_PARALLEL_HardwareReset(pChan->VinID);
        RetVal |= AR0238_PARALLEL_SoftwareReset();
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AR0238_PARALLEL_SetStandbyOn();
    }

    //AmbaPrint("[AR0238_PARALLEL] AR0238_PARALLEL_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_GetStatus
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
static UINT32 AR0238_PARALLEL_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &AR0238_PARALLELCtrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_GetModeInfo
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
static UINT32 AR0238_PARALLEL_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = AR0238_PARALLELCtrl.Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            AR0238_PARALLEL_PrepareModeInfo(pMode, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_GetDeviceInfo
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
static UINT32 AR0238_PARALLEL_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    }

    if (pDeviceInfo == NULL) {
        RetVal = SENSOR_ERR_ARG;
    }

    (void)AmbaWrap_memcpy(pDeviceInfo, &AR0238_PARALLELDeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));

    return RetVal;
}

#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_GetHdrInfo
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
static UINT32 AR0238_PARALLEL_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pHdrInfo, &AR0238_PARALLELCtrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_GetCurrentGainFactor
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
static UINT32 AR0238_PARALLEL_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    (void)pChan;
    (void)pGainFactor;
#if 0
    UINT32 CurrentAgcIdx, CurrentDgc;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        CurrentAgcIdx = AR0238_PARALLELCtrl.CurrentAgc;
        CurrentDgc = AR0238_PARALLELCtrl.CurrentDgc;
        *pGainFactor = AR0238_PARALLELAgcRegTable[CurrentAgcIdx].Factor * CurrentDgc / 133;
    }
#endif
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_GetCurrentShutterSpeed
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
static UINT32 AR0238_PARALLEL_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        *pExposureTime = (AR0238_PARALLELCtrl.Status.ModeInfo.RowTime * (FLOAT)AR0238_PARALLELCtrl.CurrentShtCtrl);
    }

    return RetVal;
}

#if 0  //TODO
static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    FLOAT AgcGain = 0.0;
    FLOAT DgcGain = 0.0;
    FLOAT MaxAgcGain = 43.2;
    FLOAT MinAgcGain = 1.52;
    UINT32 AgcTableIdx;
    //AmbaPrint("Original DesiredFactor:%f, ConvGain:%d,",DesiredFactor,ConvGain);


    if (DesiredFactor < MinAgcGain)
        DesiredFactor = MinAgcGain;
    else if (DesiredFactor > MaxAgcGain)
        DesiredFactor = MaxAgcGain;

    for (AgcTableIdx = 0; AgcTableIdx < (AR0238_PARALLEL_NUM_AGC_STEP - 1); AgcTableIdx ++) {
        if(DesiredFactor < AR0238_PARALLELAgcRegTable[AgcTableIdx + 1].Factor)
            break;
    }
#if 0
    /* linear mode does not support 32.4x */
    if (AR0238_PARALLELAgcRegTable[AgcTableIdx].Data == 0x003B) {
        AgcTableIdx --;
    }
#endif
    AgcGain = AR0238_PARALLELAgcRegTable[AgcTableIdx].Factor;
    DgcGain = (DesiredFactor / AgcGain);

    *pAnalogGainCtrl  = AgcTableIdx;   /* index of agc table */
    *pDigitalGainCtrl = DgcGain * 133; /* the setting of R0x305E */

    DgcGain = (FLOAT)*pDigitalGainCtrl / 133.0;
    *pActualFactor = AgcGain * DgcGain;
    //*pActualFactor = (UINT32)(AgcGain * DgcGain * 1024 * 1024);

    //AmbaPrint("DesiredFactor = %f, DgcR305E = 0x%08x, AgcR3060 = 0x%08x, AgcGain = %f, DgcGain = %f, ActualFactor = %u",
    //      DesiredFactor, *pDigitalGainCtrl, *pAnalogGainCtrl, AgcGain, DgcGain, *pActualFactor);

}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_ConvertGainFactor
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
static UINT32 AR0238_PARALLEL_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    //AMBA_SENSOR_MODE_INFO_s* pModeInfo = &AR0238_PARALLELCtrl.Status.ModeInfo;
    //UINT8 i;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#if 0  //TODO
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertGainFactor(pDesiredFactor->Gain[0], &(pActualFactor->Gain[0]), &(pGainCtrl->AnalogGain[0]), &(pGainCtrl->DigitalGain[0]));
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                ConvertGainFactor(pDesiredFactor->Gain[i], &(pActualFactor->Gain[i]), &(pGainCtrl->AnalogGain[i]), &(pGainCtrl->DigitalGain[i]));
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
#endif
    }
    //AmbaPrint("AnalogGain:%f", AnalogGain);
    //AmbaPrint("DesiredFactor = %f, AgainReg = 0x%08x, ActualFactor = %d",DesiredFactor, *pAnalogGainCtrl, *pActualFactor);
    (void)AmbaWrap_memset(pGainCtrl, 0, sizeof(AMBA_SENSOR_GAIN_CTRL_s));
    (void)AmbaWrap_memset(pActualFactor, 0, sizeof(AMBA_SENSOR_GAIN_FACTOR_s));

    return RetVal;
}

static void ConvertShutterSpeed(AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    (void)pModeInfo;
    (void)HdrChannel;
    (void)ExposureTime;
    (void)pActualExptime;
    (void)pShutterCtrl;

#if 0
    UINT32 MaxExposureLine, MinExposureLine;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ExposureFrames, ShutterCtrl;
    DOUBLE ShutterCtrlInDb;

    /* calculate exp. steps */
    //*pShutterCtrl = (UINT32)(ExposureTime / AR0238_PARALLELModeInfoList[Mode].RowTime);
    (void)AmbaWrap_floor((DOUBLE)(ExposureTime / ((DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame)), &ShutterCtrlInDb);
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    /* Maximun value of R0x3012(coarse_integration_time) and R0x300A(frame_length_lines) is 65535d*/
    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        /* Maximun value of FRM_LENGTH_LINES is 65535d */
        ExposureFrames = (ShutterCtrl - 1U) / NumExposureStepPerFrame + 1U;
        if ((FrameLengthLines * ExposureFrames) > 0xffffU) {
            ShutterCtrl = (0xffffU / FrameLengthLines) * NumExposureStepPerFrame;
        }

        /* limit exp. setting to a valid value */
        MaxExposureLine = (((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame - 4U;
        if (MaxExposureLine > 0xffff) {
            MaxExposureLine = 0xffff;
        }
        MinExposureLine = 1U;
        //AmbaPrint("[Linear]MaxShrWidth:%d, MinShrWidth,:%d, ShutterCtrl:%d",MaxShrWidth,MinShrWidth,ShutterCtrl);

//TODO
    } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
        ShutterCtrl = ((ShutterCtrl % 2U) == 1U) ? (ShutterCtrl - 1U) : (ShutterCtrl);

        MaxExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;

    } else {
        /* Dummy value */
        ShutterCtrl = 1;
        MaxExposureLine = 1;
        MinExposureLine = 1;

        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[NG] Wrong HdrType!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    if (ShutterCtrl > MaxExposureLine) {
        *pShutterCtrl = MaxExposureLine;
    } else if (ShutterCtrl < MinExposureLine) {
        *pShutterCtrl = MinExposureLine;
    } else {
        *pShutterCtrl = ShutterCtrl;
    }
    *pActualExptime = (FLOAT)*pShutterCtrl * (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame);
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_ConvertShutterSpeed
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
static UINT32 AR0238_PARALLEL_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &AR0238_PARALLELCtrl.Status.ModeInfo;
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

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_SetAnalogGainCtrl
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
static UINT32 AR0238_PARALLEL_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#if 0 //TODO
        if (AR0238_PARALLELAgcRegTable[AnalogGainCtrl].ConvGain == AR0238_PARALLEL_LOW_CONVERSION_GAIN)
            AR0238_PARALLEL_RegWrite(0x3100, 0x0000, 1U); /* Enable LCG */
        else
            AR0238_PARALLEL_RegWrite(0x3100, 0x0004, 1U); /* Enable HCG */

        /* Analog gain */
        AR0238_PARALLEL_RegWrite(0x3060, AR0238_PARALLELAgcRegTable[AnalogGainCtrl].Data, 1U);
#endif
    }

    AR0238_PARALLELCtrl.CurrentAgc = pAnalogGainCtrl[0];

    //SENSOR_DBG_GAIN("[Vin%d/Sensor%d/Hdr%d][AR0238_PARALLEL][GAIN] AnalogGainCtrl = 0x%x",
    //               pChan->VinID,  pChan->SensorID,  pChan->HdrID, pAnalogGainCtrl[0]);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_SetDigitalGainCtrl
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
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AR0238_PARALLEL_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    //UINT16 Data = (UINT16) DigitalGainCtrl;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#if 0 //TODO
        AR0238_PARALLEL_RegWrite(0x305E, Data, 1U); //TODO
#endif
    }

    AR0238_PARALLELCtrl.CurrentDgc = pDigitalGainCtrl[0];

    //SENSOR_DBG_GAIN("[Vin%d/Sensor%d/Hdr%d][AR0238_PARALLEL][GAIN] DigitalGainCtrl = 0x%x",
    //             pChan->VinID,  pChan->SensorID,  pChan->HdrID, pDigitalGainCtrl[0]);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_SetShutterCtrl
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
static UINT32 AR0238_PARALLEL_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    //UINT16 Data = (UINT16)ShutterCtrl;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //AR0238_PARALLEL_RegWrite(0x3012, Data, 1U); //TODO

        AR0238_PARALLELCtrl.CurrentShtCtrl = pShutterCtrl[0];
    }
    //SENSOR_DBG_SHR("[Vin%d/Sensor%d][AR0238_PARALLEL][SHR] ShutterCtrl = 0x%x",
    //           pChan->VinID, pChan->SensorID, *pShutterCtrl);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_SetSlowShutterCtrl
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
static UINT32 AR0238_PARALLEL_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = AR0238_PARALLELCtrl.Status.ModeInfo.Config.ModeID;

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else {
        UINT32 TargetFrameLengthLines = AR0238_PARALLELModeInfoList[ModeID].FrameTiming.FrameLengthLines * SlowShutterCtrl;
        //AR0238_PARALLEL_RegWrite(0x300A, (UINT16)TargetFrameLengthLines, 1U);     //TODO
        /* Update frame rate information */
        AR0238_PARALLELCtrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        AR0238_PARALLELCtrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        AR0238_PARALLELCtrl.Status.ModeInfo.FrameRate.TimeScale = AR0238_PARALLELModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d][AR0238_PARALLEL][SHR] FrameLengthLines:%d, SlowShutterCtrl:%d",
        //        pChan->VinID, pChan->SensorID, AR0238_PARALLELCtrl.Status.ModeInfo.FrameLengthLines, SlowShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AR0238_PARALLEL_Config
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
static UINT32 AR0238_PARALLEL_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &AR0238_PARALLELCtrl.Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
    AMBA_VIN_DVP_PAD_CONFIG_s AR0238_PARALLELPadConfig = {
        .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS,
    };


    if (ModeID >= AMBA_SENSOR_AR0238_PARALLEL_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_PrintUInt5("[AR0238_PARALLEL] Config Mode: %d",ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        AR0238_PARALLEL_PrepareModeInfo(pMode, pModeInfo);

        RetVal |= AmbaVIN_DvpReset(pChan->VinID, &AR0238_PARALLELPadConfig);

        /* set pll */
#if 0  /* to be refined with bsp file */

        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
#endif
        RetVal |= AmbaVIN_SensorClkEnable(AMBA_VIN_SENSOR_CLOCK0, pModeInfo->InputClk);

        (void)AmbaKAL_TaskSleep(3);

        /* reset sensor */
        (void)AR0238_PARALLEL_HardwareReset(pChan->VinID);
        (void)AR0238_PARALLEL_SoftwareReset();
        (void)AmbaKAL_TaskSleep(200);

        (void)AR0238_PARALLEL_PerformSequence(ModeID);

        /* Config Vin */
        RetVal |= AR0238_PARALLEL_ConfigVin(pChan->VinID, pModeInfo);

        // AR0238_PARALLELCtrl.Status.DevStat.Bits.Sensor0Standby = 0;
        AR0238_PARALLELCtrl.CurrentShtCtrl = 1095;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_AR0238_PARALLELObj = {
    .SensorName             = "AR0238",
    .SerdesName             = "NA",
    .Init                   = AR0238_PARALLEL_Init,
    .Enable                 = AR0238_PARALLEL_Enable,
    .Disable                = AR0238_PARALLEL_Disable,
    .Config                 = AR0238_PARALLEL_Config,
    .GetStatus              = AR0238_PARALLEL_GetStatus,
    .GetModeInfo            = AR0238_PARALLEL_GetModeInfo,
    .GetDeviceInfo          = AR0238_PARALLEL_GetDeviceInfo,
    .GetHdrInfo             = NULL,
    .GetCurrentGainFactor   = AR0238_PARALLEL_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = AR0238_PARALLEL_GetCurrentShutterSpeed,

    .ConvertGainFactor      = AR0238_PARALLEL_ConvertGainFactor,
    .ConvertShutterSpeed    = AR0238_PARALLEL_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = AR0238_PARALLEL_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = AR0238_PARALLEL_SetDigitalGainCtrl,
    .SetWbGainCtrl          = NULL,
    .SetShutterCtrl         = AR0238_PARALLEL_SetShutterCtrl,
    .SetSlowShutterCtrl     = AR0238_PARALLEL_SetSlowShutterCtrl,

    .RegisterRead           = AR0238_PARALLEL_RegisterRead,
    .RegisterWrite          = AR0238_PARALLEL_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};


