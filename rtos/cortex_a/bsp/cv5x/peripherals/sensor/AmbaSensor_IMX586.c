/**
 *  @file AmbaSensor_IMX586.c
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
 *  @details Control APIs of SONY IMX586 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX586.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "bsp.h"

static UINT32 IMX586I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
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

#ifdef CONFIG_IMX586_POSTCFG_ENABLED
static UINT32 IMX586_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);
#endif

#define MIN(x,y) ( ((x)<(y)) ? (x) : (y) )

/*-----------------------------------------------------------------------------------------------*\
 * IMX586 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX586_CTRL_s IMX586Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};
static UINT8 IMX586_CitLshift = 0U;

/*-----------------------------------------------------------------------------------------------*\
 * Configurations for Amba. VIN side
\*-----------------------------------------------------------------------------------------------*/
static AMBA_VIN_MIPI_CONFIG_s IMX586VinConfig = {
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
    .DataType = 0x2B,
#if 0
    .DataTypeMask = 0x3f,  /* receive all packets for debugging purpose */
#else
    .DataTypeMask = 0x0,
#endif
    .VirtChanHDREnable = 0U,
    .VirtChanHDRConfig = {0}
};


static UINT32 IMX586_ConvertWbGainFactor(AMBA_SENSOR_WB_GAIN_FACTOR_s DesiredWbFactor, AMBA_SENSOR_WB_GAIN_FACTOR_s *pActualWbFactor, AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_PrepareModeInfo
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
static void IMX586_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32                          RetVal          = ERR_NONE;
    UINT32                          ModeID          = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo     = &IMX586InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo    = &IMX586OutputInfo[ModeID];
    const IMX586_FRAME_TIMING_s     *pFrameTiming   = &IMX586ModeInfoList[ModeID].FrameTiming;

    if (AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    pModeInfo->LineLengthPck            = pFrameTiming->LineLengthPck;
    pModeInfo->FrameLengthLines         = pFrameTiming->FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines;
    pModeInfo->InputClk                 = pFrameTiming->InputClk;
    pModeInfo->RowTime                  = IMX586ModeInfoList[ModeID].RowTime;

    if ((AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX586HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE)) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    /* HDR information */
    if(pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }

    if (RetVal != ERR_NONE) {
        /* misra */
    }

    return;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_ConfigVin
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
static void IMX586_ConfigVin(const UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s *pVinCfg = &IMX586VinConfig;
    UINT32 RetVal = ERR_NONE;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Config.BayerPattern = pImgInfo->BayerPattern;
    pVinCfg->Config.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    if (AmbaWrap_memcpy(&pVinCfg->Config.FrameRate, &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) == ERR_NONE) {
#ifdef IMX586_CPHY
        RetVal = AmbaVIN_MipiCphyConfig(VinID, pVinCfg);
#else
        RetVal = AmbaVIN_MipiConfig(VinID, pVinCfg);
#endif
    }
    if (RetVal != ERR_NONE) {
        /* misra */
    }

    return;
}

static UINT32 IMX586_RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = IMX586_I2C_SLAVE_ADDRESS;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = ((UINT32)IMX586_I2C_SLAVE_ADDRESS | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;

    RetVal = AmbaI2C_MasterReadAfterWrite(IMX586I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_PrintStr5("[IMX586] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

static UINT32 IMX586_RegWrite(UINT32 VinID, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[128U];
    UINT32 TxSize, i;

    if (Size > 126U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = IMX586_I2C_SLAVE_ADDRESS;
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

        RetVal = AmbaI2C_MasterWrite(IMX586I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_PrintStr5("[IMX586] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }

//    AmbaPrint_PrintUInt5("[IMX586][IO] Addr = 0x%04x, Data = 0x%02x (I2C Chan: %d)", Addr, pTxData[0], IMX586I2CChan, 0, 0);
    //SENSOR_DBG_IO("[IMX586][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_RegisterRead
 *
 *  @Description:: Read sensor registers through I2C bus
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *
 *  @Output     ::
 *      pData:  Pointer to Read data buffer
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal;
    UINT8 RxData;

    RetVal = IMX586_RegRead(pChan->VinID, Addr, &RxData, 1U);

    if (RetVal == SENSOR_ERR_NONE) {
        *Data = RxData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX377_RegisterWrite
 *
 *  @Description:: Write sensor registers through I2C bus
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal;
    UINT8 TxData = (UINT8) Data;

    RetVal = IMX586_RegWrite(pChan->VinID, Addr, &TxData, 1);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_HardwareReset
 *
 *  @Description:: Reset IMX377 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void IMX586_HardwareReset(UINT32 VinId)
{
    if (AmbaUserGPIO_SensorResetCtrl(VinId, AMBA_GPIO_LEVEL_LOW) == ERR_NONE) {
        if (AmbaKAL_TaskSleep(2) == ERR_NONE) {   /* XCLR Low level pulse width >= 100ns */
            if (AmbaUserGPIO_SensorResetCtrl(VinId, AMBA_GPIO_LEVEL_HIGH) == ERR_NONE) {
                if (AmbaKAL_TaskSleep(2) == ERR_NONE) {
                    /* misra */
                }
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX586_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 RegData = 0x0;
    UINT32 RetVal;

    RetVal = IMX586_RegWrite(pChan->VinID, IMX586_MODE_SEL_REG, &RegData, 1);

    if (RetVal != ERR_NONE) {
        /* misra */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX586_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 RegData = 0x1;
    UINT32 RetVal;

    RetVal = IMX586_RegWrite(pChan->VinID, IMX586_MODE_SEL_REG, &RegData, 1);

    if (RetVal != ERR_NONE) {
        /* misra */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_InitGolbalReg
 *
 *  @Description:: init global register of sensor after reset.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void IMX586_InitGolbalReg(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i;
    UINT32 RetVal = ERR_NONE;

    for (i = 0; i < IMX586_NUM_GOLBAL_REG; i++) {
        RetVal |= IMX586_RegWrite(pChan->VinID, IMX586GlobalRegTable[i].Addr, &IMX586GlobalRegTable[i].Data, 1);
    }

    if (RetVal != ERR_NONE) {
        /* misra */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_InitGolbalReg
 *
 *  @Description:: init image quality register of sensor after reset.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void IMX586_InitImageQualityReg(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i;
    UINT32 RetVal = ERR_NONE;

    for (i = 0; i < IMX586_NUM_IMG_QUALITY_REG; i++) {
        RetVal |= IMX586_RegWrite(pChan->VinID, IMX586ImageQualityRegTable[i].Addr, &IMX586ImageQualityRegTable[i].Data, 1);
    }

    if (RetVal != ERR_NONE) {
        /* misra */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      ReadoutMode:   Sensor readout mode number
 *      pFrameTime:    Sensor frame time configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void IMX586_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SensorMode)
{
    UINT32 i;
    UINT32 RetVal = ERR_NONE;

    for (i = 0U; i < IMX586_NUM_READOUT_MODE_REG; i++) {
        RetVal |= IMX586_RegWrite(pChan->VinID, IMX586ModeRegTable[i].Addr, &IMX586ModeRegTable[i].Data[SensorMode], 1U);
    }

    if (RetVal == ERR_NONE) {
        RetVal |= AmbaWrap_memset(IMX586Ctrl[pChan->VinID].CurrentAgc, 0, sizeof(IMX586Ctrl[pChan->VinID].CurrentAgc));
        RetVal |= AmbaWrap_memset(IMX586Ctrl[pChan->VinID].CurrentDgc, 0, sizeof(IMX586Ctrl[pChan->VinID].CurrentDgc));
        RetVal |= AmbaWrap_memset(IMX586Ctrl[pChan->VinID].CurrentShrCtrl, 0, sizeof(IMX586Ctrl[pChan->VinID].CurrentShrCtrl));
    }

    if (RetVal != ERR_NONE) {
        /* misra */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX586_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < IMX586_NUM_MODE; i++) {
        IMX586ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX586ModeInfoList[i].FrameTiming.LineLengthPck /
                                                ((DOUBLE)IMX586OutputInfo[i].DataRate
                                                        * (DOUBLE)IMX586OutputInfo[i].NumDataLanes
                                                        / (DOUBLE)IMX586OutputInfo[i].NumDataBits));

        //AmbaPrint("row time: %f", IMX586ModeInfoList[i].RowTime);

    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const IMX586_FRAME_TIMING_s *pFrameTime = &IMX586ModeInfoList[0U].FrameTiming;

    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "IMX586 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        IMX586_PreCalculateModeInfo();
    }
#ifdef CONFIG_IMX586_POSTCFG_ENABLED
    /* TODO */
    //AmbaDSP_VinConfigCallBackRegister(AMBA_DSP_VIN_CONFIG_POST, (UINT32)Chan.Bits.VinID, (AMBA_DSP_VIN_OP_POST_f) IMX586_ConfigPost);
#endif

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const IMX586_FRAME_TIMING_s *pFrameTime = &IMX586ModeInfoList[0].FrameTiming;

    RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    /* Sensor is at Standby mode */
    IMX586_SetStandbyOn(pChan);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_GetStatus
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &IMX586Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_GetModeInfo
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s DesiredMode;

    if ((pChan == NULL) || (pMode ==NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(&DesiredMode, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) == ERR_NONE) {
            if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
                DesiredMode.ModeID = IMX586Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
            }

            if ((DesiredMode.ModeID >= IMX586_NUM_MODE) ||
                (pModeInfo == NULL)) {
                RetVal = SENSOR_ERR_ARG;
            } else {

                IMX586_PrepareModeInfo(&DesiredMode, pModeInfo);
            }
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }
    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_GetDeviceInfo
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pDeviceInfo, &IMX586DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_GetHdrInfo
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AmbaMisra_TouchUnused(&pShutterCtrl);

    if (AmbaWrap_memcpy(pHdrInfo, &IMX586Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_GetCurrentGainFactor
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32  RetVal              = SENSOR_ERR_NONE;
    float   AnalogGainFactor    = 0.0f;
    float   DigitalGainFactor   = 0.0f;
    UINT32  i                   = 0U;

    if (pGainFactor == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (IMX586Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX586Ctrl[pChan->VinID].CurrentAgc[0]);
            DigitalGainFactor = (FLOAT)IMX586Ctrl[pChan->VinID].CurrentDgc[0] / 256.0f;
            *pGainFactor = AnalogGainFactor * DigitalGainFactor;
        } else if (IMX586Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);
            for (i = 0U; i < (IMX586Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX586Ctrl[pChan->VinID].CurrentAgc[i]);
                DigitalGainFactor = (FLOAT)IMX586Ctrl[pChan->VinID].CurrentDgc[i] / 256.0f;
                pGainFactor[i] = AnalogGainFactor * DigitalGainFactor;
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        *pGainFactor = AnalogGainFactor * DigitalGainFactor;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_GetCurrentShutterSpeed
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32                          RetVal                  = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s*  pModeInfo;
    UINT32                          FrameLengthLines;
    UINT32                          NumExposureStepPerFrame;
    UINT8                           i;

    if (pExposureTime == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo               = &IMX586Ctrl[pChan->VinID].Status.ModeInfo;
        FrameLengthLines        = pModeInfo->FrameLengthLines;
        NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX586Ctrl[pChan->VinID].CurrentShrCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX586Ctrl[pChan->VinID].CurrentShrCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    return RetVal;
}

static void ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    FLOAT AnalogGainFactor;
    DOUBLE DigitalGainCtrlInDb;
    DOUBLE AnalogGainCtrlInDb;
    if (DesiredFactor < 1.122807) { /* 1.0 = IMX586DeviceInfo.MinAnalogGainFactor * IMX586DeviceInfo.MinDigitalGainFactor */
        *pAnalogGainCtrl = 112U;
        *pDigitalGainCtrl = 0x100U;
    } else {

        FLOAT DigitalGainFactor;
        UINT32 SensorMode = IMX586Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        FLOAT MaxAnalogGainFactor = IMX586ModeInfoList[SensorMode].MaxAGainFactor;

        DigitalGainFactor = DesiredFactor / MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0) {
            *pDigitalGainCtrl = 0x100U;
            AnalogGainFactor = DesiredFactor;
        } else {
            if (DigitalGainFactor > IMX586DeviceInfo.MaxDigitalGainFactor) {
                DigitalGainFactor = IMX586DeviceInfo.MaxDigitalGainFactor;
            }

            if (AmbaWrap_floor((DigitalGainFactor * 256.0), &DigitalGainCtrlInDb) == ERR_NONE) {
                *pDigitalGainCtrl = (UINT32) DigitalGainCtrlInDb;
                DigitalGainFactor = (FLOAT) DigitalGainCtrlInDb / 256.0f;
                AnalogGainFactor = DesiredFactor / DigitalGainFactor;
            } else {
                AnalogGainFactor = 1.0f;
            }
        }

        if (AmbaWrap_floor((1024.0 / AnalogGainFactor), &AnalogGainCtrlInDb) == ERR_NONE) {
            *pAnalogGainCtrl = 1024U - (UINT32)AnalogGainCtrlInDb;
        } else {
            *pAnalogGainCtrl = 1U;
        }
    }

    *pActualFactor = (FLOAT)(1024.0f / (1024.0f - (float)*pAnalogGainCtrl)) * ((float)*pDigitalGainCtrl / 256.0f);

#if 0
    //SENSOR_DBG_GAIN("[Vin%d][Hdr:%d][IMX586][GAINCONV] DesiredFactor:%lf, AnalogGainCtrl:%ld, DigitalGainCtrl:%ld, ActualFactor:%ld",
    //    Chan.Bits.VinID, Chan.Bits.HdrID, DesiredFactor, *pAnalogGainCtrl, *pDigitalGainCtrl, *pActualFactor);
    AmbaPrint_PrintUInt5("IMX586][GAINCONV] DesiredFactor:%ld, AnalogGainCtrl:%ld, DigitalGainCtrl:%ld, ActualFactor:%ld", DesiredFactor,  *pAnalogGainCtrl, *pDigitalGainCtrl, *pActualFactor, 0);
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_ConvertGainFactor
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{

    UINT32                          RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s*  pModeInfo;
    UINT8                           i;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &IMX586Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertGainFactor(pChan, pDesiredFactor->Gain[0], &(pActualFactor->Gain[0]), &(pGainCtrl->AnalogGain[0]), &(pGainCtrl->DigitalGain[0]));
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                ConvertGainFactor(pChan, pDesiredFactor->Gain[i], &(pActualFactor->Gain[i]), &(pGainCtrl->AnalogGain[i]), &(pGainCtrl->DigitalGain[i]));
                RetVal = IMX586_ConvertWbGainFactor(pDesiredFactor->WbGain[i],&(pActualFactor->WbGain[i]), &(pGainCtrl->WbGain[i]));
                pActualFactor->Gain[i] = ((pActualFactor->Gain[i]) / 1024.0f ) / 1024.0f;
                //AmbaPrint_PrintUInt5("IMX586]AnalogGain[%d]: %d", i, pGainCtrl->AnalogGain[i], 0, 0, 0);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}


static UINT32 IMX586_ConvertWbGainFactor(AMBA_SENSOR_WB_GAIN_FACTOR_s DesiredWbFactor, AMBA_SENSOR_WB_GAIN_FACTOR_s *pActualWbFactor, AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    DOUBLE CtrlInDb;
    UINT32 RetVal = ERR_NONE;
    //AmbaMisra_TouchUnused((void*)pChan);

    /* Convert to 16-bits iReal */
    if (AmbaWrap_floor((DesiredWbFactor.R  * 256.0), &CtrlInDb) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    pWbGainCtrl->R =  (UINT32) CtrlInDb;

    if (AmbaWrap_floor((DesiredWbFactor.Gr  * 256.0), &CtrlInDb) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    pWbGainCtrl->Gr =  (UINT32) CtrlInDb;

    if (AmbaWrap_floor((DesiredWbFactor.Gb  * 256.0), &CtrlInDb) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    pWbGainCtrl->Gb =  (UINT32) CtrlInDb;

    if (AmbaWrap_floor((DesiredWbFactor.B  * 256.0), &CtrlInDb) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    pWbGainCtrl->B =  (UINT32) CtrlInDb;

    /* Clamp maximal gain to 255.996 */
    pWbGainCtrl->R  = MIN(pWbGainCtrl->R,  0xFFFFU);
    pWbGainCtrl->Gr = MIN(pWbGainCtrl->Gr, 0xFFFFU);
    pWbGainCtrl->Gb = MIN(pWbGainCtrl->Gb, 0xFFFFU);
    pWbGainCtrl->B  = MIN(pWbGainCtrl->B,  0xFFFFU);

    /* Convert from iReal to .12 fixed point */
    pActualWbFactor->R  = (float)pWbGainCtrl->R  / 256.0f;
    pActualWbFactor->Gr = (float)pWbGainCtrl->Gr / 256.0f;
    pActualWbFactor->Gb = (float)pWbGainCtrl->Gb / 256.0f;
    pActualWbFactor->B  = (float)pWbGainCtrl->B  / 256.0f;


#if 0
    //SENSOR_DBG_GAIN("[Vin%d][IMX586][WB] DesireWBFactor:(%f,%f,%f,%f), ActualWBFactor:(%f,%f,%f,%f), WBGainCtrl:(0x%04X,0x%04x,0x%04x,0x%04x)",Chan.Bits.VinID,
    //          DesiredWbFactor.R, DesiredWbFactor.Gr, DesiredWbFactor.Gb, DesiredWbFactor.B,
    //          pActualWbFactor->R, pActualWbFactor->Gr, pActualWbFactor->Gb, pActualWbFactor->B,
    //          pWbGainCtrl->R, pWbGainCtrl->Gr, pWbGainCtrl->Gb, pWbGainCtrl->B);
    AmbaPrint_PrintUInt5("IMX586][WB] DesireWBFactor:(%d,%d,%d,%d)", DesiredWbFactor.R, DesiredWbFactor.Gr, DesiredWbFactor.Gb, DesiredWbFactor.B, 0);
    AmbaPrint_PrintUInt5("IMX586][WB] ActualWBFactor:(%d,%d,%d,%d)", pActualWbFactor->R, pActualWbFactor->Gr, pActualWbFactor->Gb, pActualWbFactor->B, 0);
    AmbaPrint_PrintUInt5("IMX586][WB] WBGainCtrl:((%d,%d,%d,%d))", pWbGainCtrl->R, pWbGainCtrl->Gr, pWbGainCtrl->Gb, pWbGainCtrl->B, 0);
#endif

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_ConvertShutterSpeed
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32                      RetVal                  = ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s*    pModeInfo         = &IMX586Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32                      SensorMode              = pModeInfo->Config.ModeID;
    UINT32                      NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32                      MaxExposureLineNumber   = 0;
    UINT32                      MinExposureLineNumber   = IMX586ModeInfoList[SensorMode].MinExpLineNum;
    DOUBLE                      CtrlInDb;

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        if (AmbaWrap_floor((DOUBLE)*pDesiredExposureTime / pModeInfo->RowTime, &CtrlInDb) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
        *pShutterCtrl = (UINT32) CtrlInDb;
        MaxExposureLineNumber = ((((*pShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - 48U;

    } else {
        if (AmbaWrap_floor((DOUBLE)*pDesiredExposureTime / pModeInfo->RowTime, &CtrlInDb) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
        *pShutterCtrl = (UINT32) CtrlInDb;
        if ((*pShutterCtrl % 2U) == 1U) {
            *pShutterCtrl = *pShutterCtrl - 1U;
        }
        MaxExposureLineNumber = NumExposureStepPerFrame - 48U;
    }

    if (*pShutterCtrl > MaxExposureLineNumber) {
        *pShutterCtrl = MaxExposureLineNumber;
    } else if (*pShutterCtrl < MinExposureLineNumber) {
        *pShutterCtrl = MinExposureLineNumber;
    } else {
        /* misra */
    }

    if (IMX586ModeInfoList[SensorMode].ExpStepVal == 2U) {
        *pShutterCtrl &= 0xfffffffeU;
    }

    if ((pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) && (*pShutterCtrl < 16U)) {
        /*HDR mode shutter should be odd number when shutter smaller than 16*/
        *pShutterCtrl |= 0x1U;
    }

    *pActualExposureTime = (FLOAT)*pShutterCtrl * (pModeInfo->RowTime * (FLOAT)pModeInfo->FrameLengthLines / (FLOAT)NumExposureStepPerFrame);

    return RetVal;

}

static UINT32 IMX586_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32                          RetVal      = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s*  pModeInfo;
    UINT32                          i;

    if ((pChan == NULL) || (pDesiredExposureTime == NULL) || (pActualExposureTime == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo   = &IMX586Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = ConvertShutterSpeed(pChan, pDesiredExposureTime, pActualExposureTime, pShutterCtrl);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                RetVal |= ConvertShutterSpeed(pChan, &pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_SetAnalogGainCtrl
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
//#define MASK_SHUTTER_DGAIN  0
//#define FIX_AE_TEST 0

static UINT32 IMX586_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32                          RetVal      = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s*  pModeInfo;
    UINT8                           TxData[2U];

#ifdef MASK_SHUTTER_DGAIN
    return RetVal;
#else
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo   = &IMX586Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal = IMX586_RegWrite(pChan->VinID, IMX586_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX586Ctrl[pChan->VinID].CurrentAgc[0] = pAnalogGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);

#ifdef FIX_AE_TEST
            ////test+++
            /* For long exposure frame */
            TxData[0] = (UINT8)((960 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8)(960 & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentAgc[0] = 960;

            /* For middle exposure frame */
            TxData[0] = (UINT8)((960 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (960 & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_MID_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentAgc[1] = 960;

            /* For short exposure frame */
            TxData[0] = (UINT8)((960 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (960 & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_ST_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentAgc[2] = 960;
#else
            /* For long exposure frame */
            TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentAgc[0] = pAnalogGainCtrl[0];

            /* For middle exposure frame */
            TxData[0] = (UINT8)((pAnalogGainCtrl[1] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[1] & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_MID_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentAgc[1] = pAnalogGainCtrl[1];

            /* For short exposure frame */
            TxData[0] = (UINT8)((pAnalogGainCtrl[2] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[2] & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_ST_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentAgc[2] = pAnalogGainCtrl[2];
#endif

        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_SetDigitalGainCtrl
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32                          RetVal      = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s*  pModeInfo;
    UINT8                           TxData[2U];

#ifdef MASK_SHUTTER_DGAIN
    return RetVal;
#else
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo   = &IMX586Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            RetVal = IMX586_RegWrite(pChan->VinID, IMX586_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX586Ctrl[pChan->VinID].CurrentDgc[0] = pDigitalGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);

#ifdef FIX_AE_TEST
            /* For long exposure frame */
            TxData[0] = (UINT8)((256 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (256 & 0x00ffU);
            RetVal = IMX586_RegWrite(pChan->VinID, IMX586_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentDgc[0] = 256;

            /* For middle exposure frame */
            TxData[0] = (UINT8)((256 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (256 & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_MID_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentDgc[1] = 256;

            /* For short exposure frame */
            TxData[0] = (UINT8)((256 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (256 & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_ST_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentDgc[2] = 256;
#else
            /* For long exposure frame */
            TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentDgc[0] = pDigitalGainCtrl[0];

            /* For middle exposure frame */
            TxData[0] = (UINT8)((pDigitalGainCtrl[1] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[1] & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_MID_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentDgc[1] = pDigitalGainCtrl[1];

            /* For short exposure frame */
            TxData[0] = (UINT8)((pDigitalGainCtrl[2] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[2] & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_ST_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentDgc[2] = pDigitalGainCtrl[2];
#endif
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_SetShutterCtrl
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32                          RetVal      = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s*  pModeInfo;
    UINT8                           TxData[2U];

#ifdef MASK_SHUTTER_DGAIN
    return RetVal;
#else
    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo   = &IMX586Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            RetVal = IMX586_RegWrite(pChan->VinID, IMX586_COARSE_INTEG_TIME_MSB_REG, TxData, 2);

            /* Update current shutter control */
            IMX586Ctrl[pChan->VinID].CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) { // fix me
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);

#ifdef FIX_AE_TEST
            /* For long exposure frame */
            TxData[0] = (UINT8)((6048 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (6048 & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentShrCtrl[0] = 6048;

            /* For middle exposure frame */
            TxData[0] = (UINT8)((6048 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (6048 & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_MID_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentShrCtrl[1] = 6048;

            /* For short exposure frame */
            TxData[0] = (UINT8)((6048 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (6048 & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_ST_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentShrCtrl[2] = 6048;
#else
            /* For long exposure frame */
            TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentShrCtrl[0] = pShutterCtrl[0];

            /* For middle exposure frame */
            TxData[0] = (UINT8)((pShutterCtrl[1] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[1] & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_MID_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentShrCtrl[1] = pShutterCtrl[1];

            /* For short exposure frame */
            TxData[0] = (UINT8)((pShutterCtrl[2] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[2] & 0x00ffU);
            RetVal |= IMX586_RegWrite(pChan->VinID, IMX586_ST_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX586Ctrl[pChan->VinID].CurrentShrCtrl[2] = pShutterCtrl[2];
#endif
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    return RetVal;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_SetSlowShutterCtrl
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT8  TxData[2];
    UINT8  i;
    UINT32 RetVal = SENSOR_ERR_NONE;

#ifdef MASK_SHUTTER_DGAIN
    return RetVal;
#else

    for (i = 0U; i < 8U; i++) {
        if (SlowShutterCtrl <= ((UINT32)1U << (UINT32)i)) {
            break;
        }
    }
    if (i == 8U) {
        i = 7U;
    }

    TxData[0] = i;
    IMX586_CitLshift = i;

    RetVal = IMX586_RegWrite(pChan->VinID, IMX586_CIT_LSHIFT_REG, TxData, 1);

    IMX586Ctrl[pChan->VinID].Status.ModeInfo.FrameRate.NumUnitsInTick =
        IMX586ModeInfoList[IMX586Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID].FrameTiming.FrameRate.NumUnitsInTick * ((UINT32)1U << (UINT32)IMX586_CitLshift);

    return RetVal;
#endif
}

static UINT32 IMX586_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32  RetVal      = SENSOR_ERR_NONE;
    UINT8   TxData[8];

#ifdef MASK_SHUTTER_DGAIN
    return RetVal;
#else
    if ((pChan == NULL) || (pWbGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        TxData[0] = (UINT8)((pWbGainCtrl[0].Gr >> 8U) & 0x00ffU);
        TxData[1] = (UINT8)(pWbGainCtrl[0].Gr & 0x00ffU);
        TxData[2] = (UINT8)((pWbGainCtrl[0].R >> 8U) & 0x00ffU);
        TxData[3] = (UINT8)(pWbGainCtrl[0].R & 0x00ffU);
        TxData[4] = (UINT8)((pWbGainCtrl[0].B >> 8U) & 0x00ffU);
        TxData[5] = (UINT8)(pWbGainCtrl[0].B & 0x00ffU);
        TxData[6] = (UINT8)((pWbGainCtrl[0].Gb >> 8U) & 0x00ffU);
        TxData[7] = (UINT8)(pWbGainCtrl[0].Gb & 0x00ffU);
        RetVal = IMX586_RegWrite(pChan->VinID, IMX586_ABS_GAIN_GR_MSB_REG, TxData, 8);
    }
    //AmbaPrint_PrintUInt5("IMX586 WBGain: Gr:%d R:%d B:%d Gb:%d", pWbGainCtrl[0].Gr, pWbGainCtrl[0].R, pWbGainCtrl[0].B, pWbGainCtrl[0].Gb, 0);

    return RetVal;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX586_Config
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
 *          UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX586_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
#ifdef IMX586_CPHY
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0]  = 0x07U,  /* DN/P_DATA_0~3 */
        [AMBA_VIN_CHANNEL1]  = 0x07U,  /* DN/P_DATA_0~3 */
        [AMBA_VIN_CHANNEL2]  = 0x07U,  /* DN/P_DATA_0~3 */
        [AMBA_VIN_CHANNEL3]  = 0x07U,  /* DN/P_DATA_0~3 */
        [AMBA_VIN_CHANNEL4]  = 0x70U,  /* DN/P_DATA_4~7 */
        [AMBA_VIN_CHANNEL5]  = 0x70U,  /* DN/P_DATA_4~7 */
        [AMBA_VIN_CHANNEL6]  = 0x70U,  /* DN/P_DATA_4~7 */
        [AMBA_VIN_CHANNEL7]  = 0x70U,  /* DN/P_DATA_4~7 */
        [AMBA_VIN_CHANNEL8]  = 0x00U,  /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL9]  = 0x00U,  /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL10] = 0x00U,  /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL11] = 0x00U,  /* SD_LVDS_4~7 */
        [AMBA_VIN_CHANNEL12] = 0x00U,  /* SD_LVDS_4~7 */
        [AMBA_VIN_CHANNEL13] = 0x00U,  /* SD_LVDS_4~7 */
    };
#else
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
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
#endif
#ifdef IMX586_CPHY
    AMBA_VIN_MIPI_CPHY_PAD_CONFIG_s IMX586PadConfig = {
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };
#else
    AMBA_VIN_MIPI_PAD_CONFIG_s IMX586PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };
#endif

    UINT32                      RetVal      = SENSOR_ERR_NONE;
    UINT32                      SensorMode  = pMode->ModeID;
    AMBA_SENSOR_MODE_INFO_s*    pModeInfo   = &IMX586Ctrl[pChan->VinID].Status.ModeInfo;

    if (SensorMode >= IMX586_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_PrintUInt5("[IMX586] Config Mode: %d(VinID: %d)", SensorMode, pChan->VinID, 0U, 0U, 0U);

        /* Update status & FrameTime */
        IMX586_PrepareModeInfo(pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        IMX586PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
#ifdef IMX586_CPHY
        RetVal = AmbaVIN_MipiCphyReset(pChan->VinID, &IMX586PadConfig);
#else
        RetVal = AmbaVIN_MipiReset(pChan->VinID, &IMX586PadConfig);
#endif
        /* INCK start */
        if (RetVal == ERR_NONE) {
            RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);

            if (RetVal == ERR_NONE) {
                RetVal = AmbaKAL_TaskSleep(3);

                if (RetVal == ERR_NONE) {
                    /* XCLR rising: hardware standby to software standy */
                    /* Reset sensor */
                    IMX586_HardwareReset(pChan->VinID);

                    /* Write golbal reg table after hardware reset */
                    IMX586_InitGolbalReg(pChan);

                    /* Write image quality reg table after hardware reset */
                    IMX586_InitImageQualityReg(pChan);

                    /* Write registers of mode change to sensor */
                    IMX586_ChangeReadoutMode(pChan, SensorMode);

#if 0
                    {
                        UINT8 TxData = 0;

                        TxData = 0x00;
                        IMX586_RegWrite(0x8943, &TxData, 1);
                        TxData = 0x01;
                        IMX586_RegWrite(0x0b00, &TxData, 1);
                        TxData = 0x00;
                        IMX586_RegWrite(0x380c, &TxData, 1);
                        TxData = 0x80;
                        IMX586_RegWrite(0x380d, &TxData, 1);
                    }
#endif

                    /* preset the parameters of shutter & gain */
                    //        IMX586_SetSlowShutterCtrl(pChan, 1);
                    //        IMX586_SetShutterCtrl(pChan, 8);
                    //        IMX586_SetAnalogGainCtrl(pChan, 112);
                    //        IMX586_SetDigitalGainCtrl(pChan, 0x100);

                    /* Standby cancel */
                    IMX586_SetStandbyOff(pChan);

                    /* Add delay to wait MIPI data output from sending standby cancel */
                    /* T8 = 4.0 ms + The delay of the coarse integration time value */
                    RetVal = AmbaKAL_TaskSleep(5);

                    if (RetVal == ERR_NONE) {
                        /* Config Vin */
                        IMX586_ConfigVin(pChan->VinID, pModeInfo);

#ifdef CONFIG_IMX586_POSTCFG_ENABLED
                        IMX586_SetStandbyOn(pChan);
#endif
                    }
                }
            }
        }
    }

    return RetVal;
}

#ifdef CONFIG_IMX586_POSTCFG_ENABLED
static UINT32 IMX586_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 RegData = 0x1;
    UINT32 RetVal;

    AmbaMisra_TouchUnused((void*)pChan);
    RetVal = IMX586_RegWrite(pChan->VinID, IMX586_MODE_SEL_REG, &RegData, 1);

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX586Obj = {
    .Init                   = IMX586_Init,
    .Enable                 = IMX586_Enable,
    .Disable                = IMX586_Disable,
    .Config                 = IMX586_Config,
    .GetStatus              = IMX586_GetStatus,
    .GetModeInfo            = IMX586_GetModeInfo,
    .GetDeviceInfo          = IMX586_GetDeviceInfo,
    .GetCurrentGainFactor   = IMX586_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX586_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX586_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX586_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX586_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX586_SetDigitalGainCtrl,
    .SetShutterCtrl         = IMX586_SetShutterCtrl,
    .SetWbGainCtrl          = IMX586_SetWbGainCtrl,
    .SetSlowShutterCtrl     = IMX586_SetSlowShutterCtrl,

    .RegisterRead           = IMX586_RegisterRead,
    .RegisterWrite          = IMX586_RegisterWrite,
    .GetHdrInfo             = IMX586_GetHdrInfo,
#ifdef CONFIG_IMX586_POSTCFG_ENABLED
    .ConfigPost             = IMX586_ConfigPost,
#else
    .ConfigPost             = NULL,
#endif

    .GetSerdesStatus        = NULL,
    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
