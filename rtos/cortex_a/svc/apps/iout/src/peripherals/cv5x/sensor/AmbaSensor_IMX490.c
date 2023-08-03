/**
 *  @file AmbaSensor_IMX490.c
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
 *  @details Control APIs of SONY IMX490 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX490.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "bsp.h"

// #define IMX490_IN_SLAVE_MODE

static UINT32 IMX490_I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
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

#ifdef IMX490_IN_SLAVE_MODE
static UINT32 IMX490_MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
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

#if 0
static INT32 IMX490_ConfigPost(UINT32 *pVinID);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * IMX490_MIPI sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX490_CTRL_s IMX490_Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_PrepareModeInfo
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
static void IMX490_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &IMX490_InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX490_OutputInfo[ModeID];
    const IMX490_FRAME_TIMING_s         *pFrameTiming   = &IMX490_ModeInfoList[ModeID].FrameTiming;

    (void)AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->LineLengthPck            = pFrameTiming->LineLengthPck;
    pModeInfo->FrameLengthLines         = pFrameTiming->FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines;
    pModeInfo->InputClk                 = pFrameTiming->InputClk;
    pModeInfo->RowTime                  = IMX490_ModeInfoList[ModeID].RowTime;
    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX490_HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
}

#ifdef IMX490_IN_SLAVE_MODE
static UINT32 IMX490_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal;
    DOUBLE PeriodInDb;
    const IMX490_MODE_INFO_s* pModeInfo = &IMX490_ModeInfoList[ModeID];
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg  = {0};

    (void) AmbaWrap_floor(((DOUBLE)pModeInfo->FrameTiming.FrameRate.NumUnitsInTick * ((DOUBLE) pModeInfo->FrameTiming.InputClk / (DOUBLE)pModeInfo->FrameTiming.FrameRate.TimeScale)) + 0.5, &PeriodInDb);
    MasterSyncCfg.RefClk            = pModeInfo->FrameTiming.InputClk;
    MasterSyncCfg.HSync.Period      = (UINT32)PeriodInDb;
    MasterSyncCfg.HSync.PulseWidth  = 8U;
    MasterSyncCfg.HSync.Polarity    = 0U;
    MasterSyncCfg.VSync.Period      = 1U;
    MasterSyncCfg.VSync.PulseWidth  = 1000U;
    MasterSyncCfg.VSync.Polarity    = 0U;
    MasterSyncCfg.HSyncDelayCycles  = 1U;
    MasterSyncCfg.VSyncDelayCycles  = 0U;
    MasterSyncCfg.ToggleHsyncInVblank  = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(IMX490_MasterSyncChannel[pChan->VinID], &MasterSyncCfg);

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_ConfigVin
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
static UINT32 IMX490_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s IMX490_VinConfig = {
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
        .DataType = 0x20U,//*****
        .DataTypeMask = 0x1fU,
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0},
    };
    //-----------------------------------------------------------------------------------------------//

    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &IMX490_VinConfig;

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
 *  @RoutineName:: IMX490_HardwareReset
 *
 *  @Description:: Reset IMX490 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 IMX490_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);

    if (RetVal == GPIO_ERR_NONE) {
        (void)AmbaKAL_TaskSleep(2);
        RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_RegRW
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
static UINT32 IMX490_RegRW(UINT32 VinID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[3];
    UINT32 TxSize;

    I2cConfig.SlaveAddr = IMX490_SENSOR_I2C_SLAVE_ADDR;
    I2cConfig.DataSize  = 3U;
    I2cConfig.pDataBuf  = TxDataBuf;

    TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
    TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
    TxDataBuf[2] = Data;

    RetVal = AmbaI2C_MasterWrite(IMX490_I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                                 &I2cConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX490] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    //SENSOR_DBG_IO("[IMX490][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_RegisterWrite
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
static UINT32 IMX490_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)Data;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX490_RegRW(pChan->VinID, Addr, WData);
    }

    return RetVal;
}

static UINT32 IMX490_RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = IMX490_SENSOR_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = IMX490_SENSOR_I2C_SLAVE_ADDR | 1U;
    I2cRxConfig.DataSize  = 1U;
    I2cRxConfig.pDataBuf  = pRxData;

    RetVal = AmbaI2C_MasterReadAfterWrite(IMX490_I2cChannel[VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX490] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_RegisterRead
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
static UINT32 IMX490_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX490_RegRead(pChan->VinID, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetAnalogGainReg
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
static UINT32 IMX490_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AGC)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP1H_LSB, (UINT8)(AGC & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP1H_HSB, (UINT8)((AGC >> 8U) & 0x7U));

    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "gain reg: %d", AGC, 0, 0, 0, 0);

    //return IMX490_RegRW(pChan->VinID, IMX490_MIPI_AGAIN_SP1H, (UINT8)AGC);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetHdrAnalogGainReg
 *
 *  @Description:: Configure sensor analog gain setting for HDR modes
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      pAnalogGainCtrl: pointer to Analog gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX490_SetHdrAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    /* SP1H */
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP1H_LSB, (UINT8)(pAnalogGainCtrl[0] & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP1H_HSB, (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x7U));

    /* SP1L */
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP1L_LSB, 0x0E);
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP1L_HSB, 0x00);

    /* SP2H */
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP2H_LSB, 0x62);
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP2H_HSB, 0x00);

    /* SP2L */
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP2L_LSB, 0x0E);
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_AGAIN_SP2L_HSB, 0x00);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetDigitalGainReg
 *
 *  @Description:: Configure sensor digital gain setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      DigitalGainCtrl: Digital gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX490_SetDigitalGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 DigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    /* SP1H */
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_DGAIN_SP1H_LSB, (UINT8)(DigitalGainCtrl & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_DGAIN_SP1H_HSB, (UINT8)((DigitalGainCtrl >> 8U) & 0x1U));

    /* SP1L */
    //RetVal |= IMX490_RegRW(pChan->VinID, IMX490_DGAIN_SP1L_LSB, (UINT8)(DigitalGainCtrl & 0xffU));
    //RetVal |= IMX490_RegRW(pChan->VinID, IMX490_DGAIN_SP1L_HSB, (UINT8)((DigitalGainCtrl >> 8U) & 0x1U));

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetHdrDigitalGainReg
 *
 *  @Description:: Configure sensor digital gain setting
 *
 *  @Input      ::
 *      Chan:            Vin ID and sensor ID
 *      DigitalGainCtrl: Digital gain setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX490_SetHdrDigitalGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 DigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    /* SP1H */
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_DGAIN_SP1H_LSB, (UINT8)(DigitalGainCtrl & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_DGAIN_SP1H_HSB, (UINT8)((DigitalGainCtrl >> 8U) & 0x1U));

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetWbGainReg
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
static UINT32 IMX490_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_WBGAIN_R_LSB, (UINT8)(pWbGainCtrl[0].R & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_WBGAIN_R_HSB, (UINT8)((pWbGainCtrl[0].R >> 8U) & 0xfU));

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_WBGAIN_GR_LSB, (UINT8)(pWbGainCtrl[0].Gr & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_WBGAIN_GR_HSB, (UINT8)((pWbGainCtrl[0].Gr >> 8U) & 0xfU));

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_WBGAIN_GB_LSB, (UINT8)(pWbGainCtrl[0].Gb & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_WBGAIN_GB_HSB, (UINT8)((pWbGainCtrl[0].Gb >> 8U) & 0xfU));

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_WBGAIN_B_LSB, (UINT8)(pWbGainCtrl[0].B & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_WBGAIN_B_HSB, (UINT8)((pWbGainCtrl[0].B >> 8U) & 0xfU));

    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "WBWBWB: R-%d Gr-%d Gb-%d B-%d", pWbGainCtrl[0].R, pWbGainCtrl[0].Gr, pWbGainCtrl[0].Gb, pWbGainCtrl[0].B, 0);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetShutterReg
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
static UINT32 IMX490_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 NumXhsEshrSpeed)
{
    UINT32 SHS1;
    UINT32 RetVal = SENSOR_ERR_NONE;

    SHS1 = NumXhsEshrSpeed;

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_SHS1_LSB, (UINT8)(SHS1 & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_SHS1_MSB, (UINT8)((SHS1 >> 8U) & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_SHS1_HSB, (UINT8)((SHS1 >> 16U) & 0x3U));

    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "XXXXXXXXXXXX: %d --- %d ## %d ## %d ## %d", SHS1, IMX490_Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame, NumXhsEshrSpeed, 0, 0);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetHdrShutterReg
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
static UINT32 IMX490_SetHdrShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    //UINT32 FSC, SHSX, RHS1, RHS2;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHSX;

    /* SP1H & SP1L */
    SHSX = pShutterCtrl[0];

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_SHS1_LSB, (UINT8)(SHSX & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_SHS1_MSB, (UINT8)((SHSX >> 8U) & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_SHS1_HSB, (UINT8)((SHSX >> 16U) & 0x3U));

    /* SP2 */
    SHSX = pShutterCtrl[0];

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_SHS2_LSB, (UINT8)(SHSX & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_SHS2_MSB, (UINT8)((SHSX >> 8U) & 0xffU));
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_SHS2_HSB, (UINT8)((SHSX >> 16U) & 0x3U));

    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "XXXXXXXXXXXX: %d --- %d ## %d ## %d ## %d", SHSX, IMX490_Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame, pShutterCtrl[0], pShutterCtrl[1], pShutterCtrl[2]);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetSlowShutterReg
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
static UINT32 IMX490_SetSlowShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 IntegrationPeriodInFrame)
{
    UINT32 TargetFrameLengthLines;
    UINT32 ModeID = IMX490_Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (IntegrationPeriodInFrame < 1U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TargetFrameLengthLines = IMX490_ModeInfoList[ModeID].FrameTiming.FrameLengthLines * IntegrationPeriodInFrame;

        if (TargetFrameLengthLines > 0x3ffffU) {
            TargetFrameLengthLines = 0x3ffffU;
        }

        RetVal |= IMX490_RegRW(pChan->VinID, IMX490_VMAX_LSB, (UINT8)(TargetFrameLengthLines & 0xffU));
        RetVal |= IMX490_RegRW(pChan->VinID, IMX490_VMAX_MSB, (UINT8)((TargetFrameLengthLines >> 8U) & 0xffU));
        RetVal |= IMX490_RegRW(pChan->VinID, IMX490_VMAX_HSB, (UINT8)((TargetFrameLengthLines >> 16U) & 0x03U));

        /* Update frame rate information */
        IMX490_Ctrl[pChan->VinID].Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        IMX490_Ctrl[pChan->VinID].Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        IMX490_Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale = IMX490_ModeInfoList[ModeID].FrameTiming.FrameRate.TimeScale / IntegrationPeriodInFrame;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX490_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_STANDBY, 0xFFU);
    (void)AmbaKAL_TaskSleep(2);
    //AmbaPrint("[IMX490] IMX490_SetStandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX490_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_STANDBY, 0x5CU);
    (void)AmbaKAL_TaskSleep(2);
    RetVal |= IMX490_RegRW(pChan->VinID, IMX490_STANDBY, 0xA3U);

    (void)AmbaKAL_TaskSleep(2U);

    return RetVal;
    /* Wait 8 frames */
    //AmbaKAL_TaskSleep(8 * 1000 * IMX490_Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.NumUnitsInTick
    //                  / IMX490_Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.TimeScale);
    //
    //AmbaPrint("[IMX490] IMX490_SetStandbyOff");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_ChangeReadoutMode
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
static UINT32 IMX490_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    (void)AmbaKAL_TaskSleep(1U);


    if (ModeID == 0U) {   //Load Reg for NML mode
        for (i = 0; i < IMX490_NUM_MODE_NML_REG; i ++) {
            RetVal |= IMX490_RegRW(pChan->VinID, IMX490_RegTable_NML[i].Addr, IMX490_RegTable_NML[i].Data);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_RegTable_NML[i].Addr, IMX490_RegTable_NML[i].Data[ModeID], RetVal, 0U, 0U);
        }
        for (i = 0; i < IMX490_NUM_CALIB; i ++) {
            RetVal |= IMX490_RegRW(pChan->VinID, IMX490_CalibTable[i].Addr, IMX490_CalibTable[i].Data[ModeID]);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }
    }

    if (ModeID == 1U) {   //Load Reg for HDR30P mode
        for (i = 0; i < IMX490_NUM_MODE_REG_HDR30P; i ++) {
            RetVal |= IMX490_RegRW(pChan->VinID, IMX490_RegTable_HDR30P[i].Addr, IMX490_RegTable_HDR30P[i].Data);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_RegTable_HDR30P[i].Addr, IMX490_RegTable_HDR30P[i].Data, RetVal, 0U, 0U);
        }
        for (i = 0; i < IMX490_NUM_REG; i ++) {
            RetVal |= IMX490_RegRW(pChan->VinID, IMX490_RegTable[i].Addr, IMX490_RegTable[i].Data[ModeID]);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }

        for (i = 0; i < IMX490_NUM_CALIB; i ++) {
            RetVal |= IMX490_RegRW(pChan->VinID, IMX490_CalibTable[i].Addr, IMX490_CalibTable[i].Data[ModeID]);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }
    }

    if (ModeID == 2U) {   //Load IQ Reg for HDR60P mode
        for (i = 0; i < IMX490_NUM_MODE_REG_HDR60P; i ++) {
            RetVal |= IMX490_RegRW(pChan->VinID, IMX490_RegTable_HDR60P[i].Addr, IMX490_RegTable_HDR60P[i].Data);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_RegTable_HDR60P[i].Addr, IMX490_RegTable_HDR60P[i].Data, RetVal, 0U, 0U);
        }
        for (i = 0; i < IMX490_NUM_REG; i ++) {
            RetVal |= IMX490_RegRW(pChan->VinID, IMX490_RegTable[i].Addr, IMX490_RegTable[i].Data[ModeID]);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }

        for (i = 0; i < IMX490_NUM_CALIB; i ++) {
            RetVal |= IMX490_RegRW(pChan->VinID, IMX490_CalibTable[i].Addr, IMX490_CalibTable[i].Data[ModeID]);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", IMX490_CalibTable_CAR28[i].Addr, IMX490_CalibTable_CAR28[i].Data[ModeID], RetVal, 0U, 0U);
        }
    }

    IMX490_Ctrl[pChan->VinID].CurrentGainCtrl[0] = 0xffffffffU;
    IMX490_Ctrl[pChan->VinID].CurrentGainCtrl[1] = 0xffffffffU;
    IMX490_Ctrl[pChan->VinID].CurrentGainCtrl[2] = 0xffffffffU;
    IMX490_Ctrl[pChan->VinID].CurrentGainCtrl[3] = 0xffffffffU;
    IMX490_Ctrl[pChan->VinID].CurrentShutterCtrl[0]= 0xffffffffU;
    IMX490_Ctrl[pChan->VinID].CurrentShutterCtrl[1]= 0xffffffffU;
    IMX490_Ctrl[pChan->VinID].CurrentShutterCtrl[2]= 0xffffffffU;
    IMX490_Ctrl[pChan->VinID].CurrentShutterCtrl[3]= 0xffffffffU;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX490_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < IMX490_NUM_MODE; i++) {
        IMX490_ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX490_ModeInfoList[i].FrameTiming.LineLengthPck /
                                         ((DOUBLE)IMX490_OutputInfo[i].DataRate
                                          * (DOUBLE)IMX490_OutputInfo[i].NumDataLanes
                                          / (DOUBLE)IMX490_OutputInfo[i].NumDataBits));

        //AmbaPrint("row time: %f", IMX490_ModeInfoList[i].RowTime);

    }
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "ZXXXXXXXXXXXXrow time: %d# %d# %d# %d# %d",IMX490_ModeInfoList[0].RowTime*100000000,
    //                                                                                         IMX490_ModeInfoList[0].FrameTiming.LineLengthPck,
    //                                                                                         IMX490_OutputInfo[0].DataRate,
    //                                                                                         IMX490_OutputInfo[0].NumDataLanes,
    //                                                                                         IMX490_OutputInfo[0].NumDataBits);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_Init
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
static UINT32 IMX490_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const IMX490_FRAME_TIMING_s *pFrameTime = &IMX490_ModeInfoList[0U].FrameTiming;

    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "IMX490 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        IMX490_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_Enable
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
static UINT32 IMX490_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX490_HardwareReset(pChan->VinID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_Disable
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
static UINT32 IMX490_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX490_SetStandbyOn(pChan);
    }

    //AmbaPrint("[IMX490] IMX490_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_GetStatus
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
static UINT32 IMX490_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &IMX490_Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_GetModeInfo
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
static UINT32 IMX490_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = IMX490_Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= IMX490_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            IMX490_PrepareModeInfo(pMode, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_GetDeviceInfo
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
static UINT32 IMX490_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &IMX490_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_GetHdrInfo
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
static UINT32 IMX490_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pHdrInfo, &IMX490_Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_GetCurrentGainFactor
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
static UINT32 IMX490_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
    DOUBLE GainFactor_D = 0.0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* SP1H */
        (void)AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)IMX490_Ctrl[pChan->VinID].CurrentGainCtrl[0] * 0.3) + ((DOUBLE)IMX490_Ctrl[pChan->VinID].DgcCtrl * 0.1) ), &GainFactor_D);
        pGainFactor[0] = (FLOAT)GainFactor_D;

        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentGain: i=%d, %d ", 0, pGainFactor[0]*1000, 0, 0, 0);

        if (IMX490_Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {

            for (i = 1U; i < (IMX490_Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                /* SP1L, SP2H, SP2L */
                (void)AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)IMX490_Ctrl[pChan->VinID].CurrentGainCtrl[i] * 0.3) + ((DOUBLE)IMX490_Ctrl[pChan->VinID].DgcCtrl * 0.1) ), &GainFactor_D);
                pGainFactor[i] = (FLOAT)GainFactor_D;

                //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentGain: i=%d, %d ", i, pGainFactor[i]*1000, 0, 0, 0);
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_GetCurrentShutterSpeed
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
static UINT32 IMX490_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX490_Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    DOUBLE ShutterTimeUnit;
    UINT32 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {

            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            pExposureTime[0] = ((FLOAT)ShutterTimeUnit * (FLOAT)IMX490_Ctrl[pChan->VinID].CurrentShutterCtrl[0]);

        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {

            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = (FLOAT)ShutterTimeUnit * (FLOAT)IMX490_Ctrl[pChan->VinID].CurrentShutterCtrl[i];
                //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentShutter: i=%d, %d ", i, pExposureTime[i]*1000000, 0, 0, 0);
            }

        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_ConvertGainFactor
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
static UINT32 IMX490_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType = IMX490_Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
    DOUBLE DesiredFactor;
    DOUBLE LogDesiredFactor = 1.0;
    DOUBLE ActualFactor64 = 0.0;
    FLOAT Desire_dB_SP1H, Desire_dB_SP1;
    FLOAT Agc_dB_SP1H = 0.0f, Agc_dB_SP1 = 0.0f;
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

            /************************************** Agc & Dgc **************************************/
            /* Maximum check */
            Desire_dB_SP1H = (Desire_dB_SP1H >= IMX490_MAX_TOTAL_GAIN) ? IMX490_MAX_TOTAL_GAIN : Desire_dB_SP1H;

            /* Minimum check */
            Desire_dB_SP1H = (Desire_dB_SP1H <= 18.0f) ? 18.0f : Desire_dB_SP1H;//*****

            /* Calculate Agc/Dgc for SP1H/SP1L */
            if (Desire_dB_SP1H <= IMX490_MAX_AGAIN) {
                Agc_dB_SP1H = Desire_dB_SP1H;
                Dgc_dB = 0.0f;
            } else if (Desire_dB_SP1H >= IMX490_MAX_AGAIN) {
                Agc_dB_SP1H = IMX490_MAX_AGAIN;
                Dgc_dB = Desire_dB_SP1H - IMX490_MAX_AGAIN;
            } else {
                AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "=== Invalid SP1H and SP1L ===", NULL, NULL, NULL, NULL, NULL);
            }

            (void) AmbaWrap_floor((DOUBLE)(Agc_dB_SP1H / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
            (void) AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
            (void)AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[0]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[0]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[0] = (FLOAT) ActualFactor64;

            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GGGGain: Desire_dB_SP1H=%d, DesiredFactor%d", Desire_dB_SP1H*1000, DesiredFactor*1000, 0, 0, 0);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GGGGain: again=%d(%ddB) dgain=%d(%ddB) pActualFactor=%d", (DOUBLE)pGainCtrl->AnalogGain[0], Agc_dB_SP1H, (DOUBLE)pGainCtrl->DigitalGain[0], Dgc_dB, pActualFactor->Gain[0]);

            //SP1L
            (void) AmbaWrap_floor((DOUBLE)(4.2 / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[1] = (UINT32)GainInDb;
            (void) AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[1] = (UINT32)GainInDb;
            (void)AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[1]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[1]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[1] = (FLOAT) ActualFactor64;

            //SP2H
            (void) AmbaWrap_floor((DOUBLE)(29.4 / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[2] = (UINT32)GainInDb;
            (void) AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[2] = (UINT32)GainInDb;
            (void)AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[2]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[2]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[2] = (FLOAT) ActualFactor64;

            //SP2L
            (void) AmbaWrap_floor((DOUBLE)(4.2 / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[3] = (UINT32)GainInDb;
            (void) AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[3] = (UINT32)GainInDb;
            (void)AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[3]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[3]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[3] = (FLOAT) ActualFactor64;

        } else {
            DesiredFactor = (DOUBLE)pDesiredFactor->Gain[0];
            (void)AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
            Desire_dB_SP1 = (FLOAT)(20.0 * LogDesiredFactor);

            /* Maximum check */
            Desire_dB_SP1 = (Desire_dB_SP1 >= IMX490_MAX_TOTAL_GAIN) ? IMX490_MAX_TOTAL_GAIN : Desire_dB_SP1;

            /* Minimum check */
            Desire_dB_SP1 = (Desire_dB_SP1 <= 0.0f) ? 0.0f : Desire_dB_SP1;

            /* Calculate Agc/Dgc for SP1 */
            if (Desire_dB_SP1 <= IMX490_MAX_AGAIN) {
                Agc_dB_SP1 = Desire_dB_SP1;
                Dgc_dB = 0.0f;
            } else {
                Agc_dB_SP1 = IMX490_MAX_AGAIN;
                Dgc_dB = Desire_dB_SP1 - IMX490_MAX_AGAIN;
            }

            (void) AmbaWrap_floor((DOUBLE)(Agc_dB_SP1 / 0.3), &GainInDb);
            pGainCtrl->AnalogGain[0] = (UINT32)GainInDb;
            (void) AmbaWrap_floor((DOUBLE)(Dgc_dB / 0.1), &GainInDb);
            pGainCtrl->DigitalGain[0] = (UINT32)GainInDb;
            (void)AmbaWrap_pow(10.0, 0.05 * ( ((DOUBLE)pGainCtrl->AnalogGain[0]*0.3) + ((DOUBLE)pGainCtrl->DigitalGain[0]*0.1) ), &ActualFactor64);
            pActualFactor->Gain[0] = (FLOAT) ActualFactor64;

            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GGGGain: Desire_dB_SP1=%d, DesiredFactor%d", Desire_dB_SP1*1000, DesiredFactor*1000, 0, 0, 0);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GGGGain: again=%d(%ddB) dgain=%d(%ddB) pActualFactor=%d", (DOUBLE)pGainCtrl->AnalogGain[0]*1000, Agc_dB_SP1*1000, (DOUBLE)pGainCtrl->DigitalGain[0], Dgc_dB, pActualFactor->Gain[0]*1000);
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
            (void) AmbaWrap_memcpy(&pGainCtrl->WbGain[3], &pGainCtrl->WbGain[0], sizeof(AMBA_SENSOR_WB_CTRL_s));

            (void) AmbaWrap_memcpy(&pActualFactor->WbGain[1], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
            (void) AmbaWrap_memcpy(&pActualFactor->WbGain[2], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));
            (void) AmbaWrap_memcpy(&pActualFactor->WbGain[3], &pActualFactor->WbGain[0], sizeof(AMBA_SENSOR_WB_GAIN_FACTOR_s));

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
    UINT32 MaxExposureline, MinExposureline;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ShutterCtrl;
    DOUBLE ShutterTimeUnit, ShutterCtrlInDb;

    ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

    (void)AmbaWrap_floor(((DOUBLE)ExposureTime / ShutterTimeUnit), &ShutterCtrlInDb);
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
        MaxExposureline = 2000U - 8U;//VMAX = 2000
        MinExposureline = 4U;
    }

    if (ShutterCtrl > MaxExposureline) {
        ShutterCtrl = MaxExposureline;
    } else if (ShutterCtrl < MinExposureline) {
        ShutterCtrl = MinExposureline;
    } else {
        (void)ShutterCtrl;  /* do not need to adjust ShutterCtrl */
    }

    if ( (ShutterCtrl % 2U) == 1U) {
        ShutterCtrl = ShutterCtrl - 1U;
    }

    *pShutterCtrl   = ShutterCtrl;
    *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_ConvertShutterSpeed
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
static UINT32 IMX490_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX490_Ctrl[pChan->VinID].Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                ConvertShutterSpeed(pModeInfo, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
                //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SSSShutter: i=%d pDesiredExposureTime=%d pActualExposureTime=%d pShutterCtrl=%d", i, pDesiredExposureTime[i]*1000000, pActualExposureTime[i]*1000000, pShutterCtrl[i], 0);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SSSShutter: i=%d pDesiredExposureTime=%d pActualExposureTime=%d pShutterCtrl=%d", 0, pDesiredExposureTime[0]*1000000, pActualExposureTime[0]*1000000, pShutterCtrl[0], 0);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetAnalogGainCtrl
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
static UINT32 IMX490_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX490_Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX490_SetAnalogGainReg(pChan, pAnalogGainCtrl[0]);

            /* Update current analog gain control */
            IMX490_Ctrl[pChan->VinID].CurrentGainCtrl[0] = pAnalogGainCtrl[0];
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Set again = %d", pAnalogGainCtrl[0]*1000, 0, 0, 0, 0);

        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = IMX490_SetHdrAnalogGainReg(pChan, pAnalogGainCtrl);

            /* Update current analog gain control */
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                IMX490_Ctrl[pChan->VinID].CurrentGainCtrl[i] = pAnalogGainCtrl[i];
            }

        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetDigitalGainCtrl
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
static UINT32 IMX490_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX490_Ctrl[pChan->VinID].Status.ModeInfo;
    //UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX490_SetDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            IMX490_Ctrl[pChan->VinID].DgcCtrl = pDigitalGainCtrl[0];

        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = IMX490_SetHdrDigitalGainReg(pChan, pDigitalGainCtrl[0]);

            /* Update current digital gain control */
            IMX490_Ctrl[pChan->VinID].DgcCtrl = pDigitalGainCtrl[0];

        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetWbGainCtrl
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
static UINT32 IMX490_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX490_Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX490_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            IMX490_Ctrl[pChan->VinID].WbCtrl[0] = pWbGainCtrl[0];

        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = IMX490_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                /* Update current shutter control */
                IMX490_Ctrl[pChan->VinID].WbCtrl[i] = pWbGainCtrl[0];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "WWWbgain-set: R=%d Gr=%d Gb=%d B=%d", pWbGainCtrl[0].R*1000, pWbGainCtrl[0].Gr*1000, pWbGainCtrl[0].Gb*1000, pWbGainCtrl[0].B*1000, 0);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetShutterCtrl
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
static UINT32 IMX490_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX490_Ctrl[pChan->VinID].Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = IMX490_SetShutterReg(pChan, pShutterCtrl[0]);

            /* Update current shutter control */
            IMX490_Ctrl[pChan->VinID].CurrentShutterCtrl[0] = pShutterCtrl[0];

        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = IMX490_SetHdrShutterReg(pChan, pShutterCtrl);

            /* Update current shutter control */
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                /* Update current shutter control */
                IMX490_Ctrl[pChan->VinID].CurrentShutterCtrl[i] = pShutterCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d][IMX490][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
        //                  pChan->VinID, pChan->SensorID,  *pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_SetSlowShutterCtrl
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
static UINT32 IMX490_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else if (IMX490_Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        RetVal = IMX490_SetSlowShutterReg(pChan, SlowShutterCtrl);

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d ][IMX490][SHR] SlowShutterCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   SlowShutterCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX490_Config
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
static UINT32 IMX490_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0]  = 0x0fU,  /* DN/P_DATA_0~3 */
        [AMBA_VIN_CHANNEL1]  = 0x0fU,  /* DN/P_DATA_0~3 */
        [AMBA_VIN_CHANNEL2]  = 0x0fU,  /* DN/P_DATA_0~3 */
        [AMBA_VIN_CHANNEL3]  = 0x0fU,  /* DN/P_DATA_0~3 */
        [AMBA_VIN_CHANNEL4]  = 0xf0U,  /* DN/P_DATA_4~7 */
        [AMBA_VIN_CHANNEL5]  = 0xf0U,  /* DN/P_DATA_4~7 */
        [AMBA_VIN_CHANNEL6]  = 0xf0U,  /* DN/P_DATA_4~7 */
        [AMBA_VIN_CHANNEL7]  = 0xf0U,  /* DN/P_DATA_4~7 */
        [AMBA_VIN_CHANNEL8]  = 0x0fU,  /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL9]  = 0x0fU,  /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL10] = 0x0fU,  /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL11] = 0xf0U,  /* SD_LVDS_4~7 */
        [AMBA_VIN_CHANNEL12] = 0xf0U,  /* SD_LVDS_4~7 */
        [AMBA_VIN_CHANNEL13] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s IMX490_PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX490_Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;

    if (ModeID >= IMX490_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX490] Config Mode: %d",ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        IMX490_PrepareModeInfo(pMode, pModeInfo);

#ifdef IMX490_IN_SLAVE_MODE
        (void)AmbaVIN_MasterSyncDisable(IMX490_MasterSyncChannel[pChan->VinID]);
#endif

        /* Adjust mipi-phy parameters */
        IMX490_PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &IMX490_PadConfig);

        /* set pll */
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);

        (void)AmbaKAL_TaskSleep(3);

        /* SW reset */
        //(void)IMX490_RegRW(pChan->VinID, IMX490_RESET, 0x01);

        /* program sensor */
        (void)IMX490_ChangeReadoutMode(pChan, ModeID);

#ifdef IMX490_IN_SLAVE_MODE
        RetVal |= IMX490_RegRW(pChan->VinID, 0x01f0U, 0x01U);//External Pulse-based sync
        RetVal |= IMX490_RegRW(pChan->VinID, 0x01f1U, 0x01U);//Active Low
#endif
        (void)IMX490_SetStandbyOff(pChan);
#ifdef IMX490_IN_SLAVE_MODE
        RetVal |= IMX490_ConfigMasterSync(pChan, ModeID);
#endif

        (void)IMX490_SetStandbyOn(pChan);
        /* config vin */
        RetVal |= IMX490_ConfigVin(pChan->VinID, pModeInfo);
    }

    return RetVal;
}

static UINT32 IMX490_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    (void)IMX490_SetStandbyOff(pChan);
    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX490Obj = {
    .SensorName             = "IMX490",
    .SerdesName             = "NA",
    .Init                   = IMX490_Init,
    .Enable                 = IMX490_Enable,
    .Disable                = IMX490_Disable,
    .Config                 = IMX490_Config,
    .GetStatus              = IMX490_GetStatus,
    .GetModeInfo            = IMX490_GetModeInfo,
    .GetDeviceInfo          = IMX490_GetDeviceInfo,
    .GetHdrInfo             = IMX490_GetHdrInfo,
    .GetCurrentGainFactor   = IMX490_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX490_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX490_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX490_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX490_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX490_SetDigitalGainCtrl,
    .SetWbGainCtrl          = IMX490_SetWbGainCtrl,
    .SetShutterCtrl         = IMX490_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX490_SetSlowShutterCtrl,

    .RegisterRead           = IMX490_RegisterRead,
    .RegisterWrite          = IMX490_RegisterWrite,

    .ConfigPost             = IMX490_ConfigPost,
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};

