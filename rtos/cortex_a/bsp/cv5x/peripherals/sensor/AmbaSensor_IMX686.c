/**
 *  @file AmbaSensor_IMX686.c
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
 *  @details Control APIs of SONY IMX686 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX686.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "bsp.h"
#include "AmbaGPIO_Def.h"

//#define AE_DEBUG
//#define GAIN_DEBUG
//#define IMX686_ENABLE_QSC_SETTING

#ifdef IMX686_ENABLE_QSC_SETTING
#include "AmbaNVM_Partition.h"
#define QSC_CALIB_DATA_SIZE 3024
#define IMX686_MODULE_3FVLH3M0D2007J
//#define IMX686_MODULE_3FVLH3M0D2007P
//#define IMX686_MODULE_3FVLH470D2000F
//#define IMX686_MODULE_3FVOH6C0X200A1
//#define IMX686_MODULE_3FVOH6C0X200NG
//#define IMX686_MODULE_3FVOH6C0X200CE
//#define IMX686_MODULE_3FVOH6C0X20006
static UINT8 QSCData[QSC_CALIB_DATA_SIZE + 2];
#endif
static UINT32 IMX686I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
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

#ifdef CONFIG_IMX686_POSTCFG_ENABLED
static UINT32 IMX686_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * IMX686 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX686_CTRL_s IMX686Ctrl = {0};

static UINT32 MIN(UINT32 x, UINT32 y)
{
    UINT32 RetVal =y;
    if (x<y) {
        RetVal = x;
    }
    return RetVal;
}

static UINT32 IMX686_ConvertWbGainFactor(AMBA_SENSOR_WB_GAIN_FACTOR_s DesiredWbFactor, AMBA_SENSOR_WB_GAIN_FACTOR_s *pActualWbFactor, AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl);


#if 0
static UINT32 IMX686_OpenI2cChannel(void)
{
    UINT8 TxData = 0xff;
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT32 TxSize;

    I2cConfig.SlaveAddr = TCA9548A_I2C_SLAVE_ADDRESS;
    I2cConfig.DataSize  = 1U;
    I2cConfig.pDataBuf  = &TxData;

    RetVal = AmbaI2C_MasterWrite(AMBA_SENSOR_I2C_CHANNEL, AMBA_I2C_SPEED_FAST,
                                 &I2cConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_PrintUInt5("[TCA9548A] I2C does not work, RetVal:0x%x", RetVal, 0U, 0U, 0U, 0U);
    }

    return SENSOR_ERR_NONE;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_PrepareModeInfo
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
static void IMX686_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal                                       = ERR_NONE;
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &IMX686InputInfoNormalReadout[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &IMX686OutputInfo[ModeID];
    const IMX686_FRAME_TIMING_s    *pFrameTiming        = &IMX686ModeInfoList[ModeID].FrameTiming;

    pModeInfo->LineLengthPck            = pFrameTiming->Linelengthpck;
    pModeInfo->FrameLengthLines         = pFrameTiming->FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines;

    pModeInfo->InputClk = pFrameTiming->InputClk;

    pModeInfo->RowTime = (FLOAT)((DOUBLE)pFrameTiming->Linelengthpck / (DOUBLE)IMX686ModeInfoList[ModeID].FrameTiming.PixelRate);

    if ((AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE)) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    /* HDR information */
    if(IMX686ModeInfoList[ModeID].HdrType == AMBA_SENSOR_HDR_NONE) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
        pModeInfo->HdrInfo.HdrType = AMBA_SENSOR_HDR_NONE;
        pModeInfo->HdrInfo.ActiveChannels = 0;
    } else if (IMX686ModeInfoList[ModeID].HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
#if 1
        pModeInfo->HdrInfo.NotSupportIndividualGain = 0;  // individual D gain?  need to be fixed!!!!!
        AmbaPrint_PrintUInt5("ActiveChannels:%d, HdrType:%d", IMX686HdrInfo[ModeID].ActiveChannels, IMX686HdrInfo[ModeID].HdrType, 0U, 0U, 0U);
        if (AmbaWrap_memcpy(&pModeInfo->HdrInfo, &IMX686HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
#else
        pModeInfo->MinFrameRate.TimeScale /= 8;
        pModeInfo->HdrInfo.HdrType = AMBA_SENSOR_HDR_NONE;
        pModeInfo->HdrInfo.ActiveChannels = 0;
#endif
    } else {
        //MisraC
    }

    if (RetVal != ERR_NONE) {
        /* misra */
    }

    return;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_ConfigVin
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
static void IMX686_ConfigVin(const UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s IMX686VinConfig = {
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
        .DataTypeMask = 0x00,
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0}
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s *pVinCfg = &IMX686VinConfig;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Config.BayerPattern = pImgInfo->BayerPattern;
    pVinCfg->Config.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    if (AmbaWrap_memcpy(&pVinCfg->Config.FrameRate, &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) == ERR_NONE) {
#ifdef IMX686_CPHY
        RetVal = AmbaVIN_MipiCphyConfig(VinID, pVinCfg);
#else
        RetVal = AmbaVIN_MipiConfig(VinID, pVinCfg);
#endif
    }

    if (RetVal != ERR_NONE) {
        //misrac
    }
    return;
}

static UINT32 IMX686_RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = IMX686_I2C_SLAVE_ADDRESS;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = ((UINT32)IMX686_I2C_SLAVE_ADDRESS | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;
#if 0
    RetVal = AmbaI2C_MasterReadAfterWrite(AMBA_SENSOR_I2C_CHANNEL, AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);
#else
    RetVal = AmbaI2C_MasterReadAfterWrite(IMX686I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);
#endif
    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_PrintStr5("[IMX686] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

static UINT32 IMX686_RegWrite(UINT32 VinID, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[128U];
    UINT32 TxSize, i;

    if (Size > 126U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = IMX686_I2C_SLAVE_ADDRESS;
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

#if 0
        RetVal = AmbaI2C_MasterWrite(AMBA_SENSOR_I2C_CHANNEL, AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);
#else
        RetVal = AmbaI2C_MasterWrite(IMX686I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);

#endif


        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_PrintStr5("[IMX686] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }

    //AmbaPrint_PrintUInt5("[IMX686][IO] Addr = 0x%04x, Data = 0x%02x", Addr, pTxData[0], 0, 0, 0);
    //SENSOR_DBG_IO("[IMX686][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_RegisterRead
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
static UINT32 IMX686_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RxData;

    AmbaMisra_TouchUnused(&pChan);

    RetVal |= IMX686_RegRead(pChan->VinID, Addr, &RxData, 1U);
    *Data = RxData;

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
static UINT32 IMX686_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData = (UINT8) Data;

    AmbaMisra_TouchUnused(&pChan);

    RetVal = IMX686_RegWrite(pChan->VinID, Addr, &TxData, 1);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_HardwareReset
 *
 *  @Description:: Reset IMX377 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void IMX686_HardwareReset(UINT32 VinId)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinId, AMBA_GPIO_LEVEL_LOW);
    RetVal |= AmbaKAL_TaskSleep(2);               /* XCLR Low level pulse width >= 100ns */
    RetVal |= AmbaUserGPIO_SensorResetCtrl(VinId, AMBA_GPIO_LEVEL_HIGH);
    RetVal |= AmbaKAL_TaskSleep(2);
    if (RetVal != SENSOR_ERR_NONE) {
        //misrac
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX686_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RegData = 0x0;
    RetVal = IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_MODE_SEL_REG, &RegData, 1);

    AmbaMisra_TouchUnused(&RetVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void IMX686_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RegData = 0x1;
    RetVal = IMX686_RegWrite(pChan->VinID, (UINT16)IMX686_MODE_SEL_REG, &RegData, 1);

    AmbaMisra_TouchUnused(&RetVal);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_InitGolbalReg
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
static UINT32 IMX686_InitGolbalReg(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;

    for (i = 0U; i < IMX686_NUM_GOLBAL_REG; i++) {
        RetVal |= IMX686_RegWrite(pChan->VinID, IMX686GlobalRegTable[i].Addr, &IMX686GlobalRegTable[i].Data, 1);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_InitGolbalReg
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
static UINT32 IMX686_InitImageQualityReg(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
    for (i = 0U; i <IMX686_NUM_IMG_QUALITY_REG; i++) {
        RetVal |= IMX686_RegWrite(pChan->VinID, IMX686ImageQualityRegTable[i].Addr, &IMX686ImageQualityRegTable[i].Data, 1);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_ChangeReadoutMode
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
static UINT32 IMX686_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SensorMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxRegData[4];
    UINT32 i = 0;

    for (i = 0U; i < (UINT32) IMX686_NUM_READOUT_MODE_REG; i++) {
        RetVal |= IMX686_RegWrite(pChan->VinID, IMX686ModeRegTable[i].Addr, &IMX686ModeRegTable[i].Data[SensorMode], 1U);
    }

    TxRegData[0] = (UINT8)((IMX686ModeInfoList[SensorMode].FrameTiming.FrameLengthLines & 0xff00U) >> 8U);
    TxRegData[1] = (UINT8)((IMX686ModeInfoList[SensorMode].FrameTiming.FrameLengthLines & 0xffU));
    TxRegData[2] = (UINT8)((IMX686ModeInfoList[SensorMode].FrameTiming.Linelengthpck & 0xff00U) >> 8U);
    TxRegData[3] = (UINT8)((IMX686ModeInfoList[SensorMode].FrameTiming.Linelengthpck & 0xffU));
    RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_FRM_LENGTH_LINES_MSB_REG, &TxRegData[0], 4);

#ifdef IMX686_ENABLE_QSC_SETTING
    if((UINT32) AMBA_SENSOR_IMX686_9248_6944_30P == SensorMode) {
        /*Enable QSC*/
        UINT32 TxSize;
        AMBA_I2C_TRANSACTION_s I2cConfig = {0};
        TxRegData[0] = 0x1;
        IMX686_RegWrite(pChan->VinID, 0x32D6, TxRegData, 1);
        I2cConfig.SlaveAddr = IMX686_I2C_SLAVE_ADDRESS;
        I2cConfig.DataSize  = (2U + QSC_CALIB_DATA_SIZE);
        I2cConfig.pDataBuf  = (UINT8 *)QSCData;
        QSCData[0] = (UINT8) ((0xCA00 & 0xff00) >> 8);
        QSCData[1] = (UINT8) (0xCA00 & 0x00ff);

        if (AmbaI2C_MasterWrite(IMX686I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                &I2cConfig, &TxSize, 1000U) != I2C_ERR_NONE) {
            AmbaPrint_PrintUInt5("I2C does not work!!!!!", 0U, 0U, 0U, 0U, 0U);
        }
    }

#endif
    if (RetVal == ERR_NONE) {
        RetVal |= AmbaWrap_memset(IMX686Ctrl.CurrentAgc, 0, sizeof(IMX686Ctrl.CurrentAgc));
        RetVal |= AmbaWrap_memset(IMX686Ctrl.CurrentDgc, 0, sizeof(IMX686Ctrl.CurrentDgc));
        RetVal |= AmbaWrap_memset(IMX686Ctrl.CurrentShrCtrl, 0, sizeof(IMX686Ctrl.CurrentShrCtrl));
    }

    if (RetVal != ERR_NONE) {
        /* misra */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_Init
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
static UINT32 IMX686_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
#ifdef IMX686_ENABLE_QSC_SETTING
    UINT32 FileSize = 0;
    UINT32 NvmID = AMBA_NVM_NAND;
#endif
    const IMX686_FRAME_TIMING_s *pFrameTime = &IMX686ModeInfoList[0].FrameTiming;

//    IMX686_OpenI2cChannel();

    (void)AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);

#ifdef CONFIG_IMX686_POSTCFG_ENABLED
    /* TODO */
    //AmbaDSP_VinConfigCallBackRegister(AMBA_DSP_VIN_CONFIG_POST, (UINT32)Chan.Bits.VinID, (AMBA_DSP_VIN_OP_POST_f) IMX686_ConfigPost);
#endif

    // Init lens focus
    {
        UINT8 I2cDataBuf[2][128];
        UINT16 *pTxDataBuf = NULL;
        UINT32 TargetPos = 1500;
        UINT32 ReadSize = 0x0;
        AMBA_I2C_TRANSACTION_s TxI2cConfig = {0};
        AMBA_I2C_TRANSACTION_s RxI2cConfig = {0};

        AmbaMisra_TypeCast(pTxDataBuf, I2cDataBuf[0]);
        TxI2cConfig.SlaveAddr = 0x18;
        TxI2cConfig.DataSize  = 3U;
        TxI2cConfig.pDataBuf = I2cDataBuf[0];

        RxI2cConfig.SlaveAddr = 0x18U | 0x1U;
        RxI2cConfig.DataSize  = 1U;
        RxI2cConfig.pDataBuf = I2cDataBuf[1];

        // Change to active mode start
        pTxDataBuf[0] = 0x02;
        RetVal |= AmbaI2C_MasterReadAfterWrite(IMX686I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U, &TxI2cConfig, &RxI2cConfig, &ReadSize, 5000U);

        // Change to active mode end
        I2cDataBuf[0][0] = 0x02;
        I2cDataBuf[0][1] = 0x00;

        TxI2cConfig.SlaveAddr = 0x18;
        TxI2cConfig.DataSize  = 2U;
        TxI2cConfig.pDataBuf = I2cDataBuf[0];
        RetVal |= AmbaI2C_MasterWrite(IMX686I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, &TxI2cConfig, &ReadSize, 5000U);

        RetVal |= AmbaKAL_TaskSleep(10);
        // Move to TargetPos start
        I2cDataBuf[0][0] = 0x00;
        I2cDataBuf[0][1] = (UINT8)(TargetPos >> 4);
        RetVal |= AmbaI2C_MasterWrite(IMX686I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, &TxI2cConfig, &ReadSize, 5000U);
        I2cDataBuf[0][0] = 0x01;
        I2cDataBuf[0][1] = (UINT8)((TargetPos & 0x0FU) << 4);
        RetVal |= AmbaI2C_MasterWrite(IMX686I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, &TxI2cConfig, &ReadSize, 5000U);
        AmbaPrint_PrintUInt5("[IMX686] focus_init_done", 0U, 0U, 0U, 0U, 0U);
    }


#ifdef IMX686_ENABLE_QSC_SETTING
#if defined(IMX686_MODULE_3FVLH3M0D2007J)
    if (OK != AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVLH3M0D2007J_QSC.bin", &FileSize)) {
        AmbaPrint_PrintUInt5("Fail to get 3FVLH3M0D2007J_QSC.bin size %d (%d) ", 0U, 0U, 0U, 0U, 0U);
        return SENSOR_ERR_NONE;
    } else {
        AmbaPrint_PrintUInt5("3FVLH3M0D2007J_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);;
    }

    if(3024U != FileSize) {
        AmbaPrint_PrintUInt5("Abnormal 3FVLH3M0D2007J_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    /* Load QSC Data */
    if (OK != AmbaNVM_ReadRomFile(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVLH3M0D2007J_QSC.bin", 0, QSC_CALIB_DATA_SIZE, (UINT8 *)(&QSCData[2]), 5000U)) {
        AmbaPrint_PrintUInt5("Load Imx686 QSC data failed", 0U, 0U, 0U, 0U, 0U);
    }
#elif defined(IMX686_MODULE_3FVLH3M0D2007P)
    if (OK != AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVLH3M0D2007P_QSC.bin", &FileSize)) {
        AmbaPrint_PrintUInt5("Fail to get 3FVLH3M0D2007P_QSC.bin size", 0U, 0U, 0U, 0U, 0U);;
        return SENSOR_ERR_NONE;
    } else {
        AmbaPrint_PrintUInt5("3FVLH3M0D2007P_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    if(3024U != FileSize) {
        AmbaPrint_PrintUInt5("Abnormal 3FVLH3M0D2007P_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    /* Load QSC Data */
    if (OK != AmbaNVM_ReadRomFile(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVLH3M0D2007P_QSC.bin", 0, QSC_CALIB_DATA_SIZE, (UINT8 *)(&QSCData[2]), 5000U)) {
        AmbaPrint_PrintUInt5("Load Imx686 QSC data failed", 0U, 0U, 0U, 0U, 0U);
    }
#elif defined(IMX686_MODULE_3FVLH470D2000F)
    if (OK != AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVLH470D2000F_QSC.bin", &FileSize)) {
        AmbaPrint_PrintUInt5("Fail to get 3FVLH470D2000F_QSC.bin size", 0U, 0U, 0U, 0U, 0U);;
        return SENSOR_ERR_NONE;
    } else {
        AmbaPrint_PrintUInt5("3FVLH470D2000F_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    if(3024U != FileSize) {
        AmbaPrint_PrintUInt5("Abnormal 3FVLH470D2000F_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    /* Load QSC Data */
    if (OK != AmbaNVM_ReadRomFile(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVLH470D2000F_QSC.bin", 0, QSC_CALIB_DATA_SIZE, (UINT8 *)(&QSCData[2]), 5000U)) {
        AmbaPrint_PrintUInt5("Load Imx686 QSC data failed", 0U, 0U, 0U, 0U, 0U);
    }
#elif defined(IMX686_MODULE_3FVOH6C0X200A1)
    if (OK != AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVOH6C0X200A1_QSC.bin", &FileSize)) {
        AmbaPrint_PrintUInt5("Fail to get 3FVOH6C0X200A1_QSC.bin size", 0U, 0U, 0U, 0U, 0U);;
        return SENSOR_ERR_NONE;
    } else {
        AmbaPrint_PrintUInt5("3FVOH6C0X200A1_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    if(3024U != FileSize) {
        AmbaPrint_PrintUInt5("Abnormal 3FVOH6C0X200A1_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    /* Load QSC Data */
    if (OK != AmbaNVM_ReadRomFile(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVOH6C0X200A1_QSC.bin", 0, QSC_CALIB_DATA_SIZE, (UINT8 *)(&QSCData[2]), 5000U)) {
        AmbaPrint_PrintUInt5("Load Imx686 QSC data failed", 0U, 0U, 0U, 0U, 0U);
    }
#elif defined(IMX686_MODULE_3FVOH6C0X200NG)
    if (OK != AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVOH6C0X200NG_QSC.bin", &FileSize)) {
        AmbaPrint_PrintUInt5("Fail to get 3FVOH6C0X200NG_QSC.bin size", 0U, 0U, 0U, 0U, 0U);;
        return SENSOR_ERR_NONE;
    } else {
        AmbaPrint_PrintUInt5("3FVOH6C0X200NG_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    if(3024U != FileSize) {
        AmbaPrint_PrintUInt5("Abnormal 3FVOH6C0X200NG_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    /* Load QSC Data */
    if (OK != AmbaNVM_ReadRomFile(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVOH6C0X200NG_QSC.bin", 0, QSC_CALIB_DATA_SIZE, (UINT8 *)(&QSCData[2]), 5000U)) {
        AmbaPrint_PrintUInt5("Load Imx686 QSC data failed", 0U, 0U, 0U, 0U, 0U);
    }
#elif defined(IMX686_MODULE_3FVOH6C0X200CE)
    if (OK != AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVOH6C0X200CE_QSC.bin", &FileSize)) {
        AmbaPrint_PrintUInt5("Fail to get 3FVOH6C0X200CE_QSC.bin size", 0U, 0U, 0U, 0U, 0U);;
        return SENSOR_ERR_NONE;
    } else {
        AmbaPrint_PrintUInt5("3FVOH6C0X200CE_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    if(3024U != FileSize) {
        AmbaPrint_PrintUInt5("Abnormal 3FVOH6C0X200CE_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    /* Load QSC Data */
    if (OK != AmbaNVM_ReadRomFile(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVOH6C0X200CE_QSC.bin", 0, QSC_CALIB_DATA_SIZE, (UINT8 *)(&QSCData[2]), 5000U)) {
        AmbaPrint_PrintUInt5("Load Imx686 QSC data failed", 0U, 0U, 0U, 0U, 0U);
    }
#elif defined(IMX686_MODULE_3FVOH6C0X20006)
    if (OK != AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVOH6C0X20006_QSC.bin", &FileSize)) {
        AmbaPrint_PrintUInt5("Fail to get 3FVOH6C0X20006_QSC.bin size", 0U, 0U, 0U, 0U, 0U);;
        return SENSOR_ERR_NONE;
    } else {
        AmbaPrint_PrintUInt5("3FVOH6C0X20006_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    if(3024U != FileSize) {
        AmbaPrint_PrintUInt5("Abnormal 3FVOH6C0X20006_QSC.bin size :%d", FileSize, 0U, 0U, 0U, 0U);
    }

    /* Load QSC Data */
    if (OK != AmbaNVM_ReadRomFile(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, "3FVOH6C0X20006_QSC.bin", 0, QSC_CALIB_DATA_SIZE, (UINT8 *)(&QSCData[2]), 5000U)) {
        AmbaPrint_PrintUInt5("Load Imx686 QSC data failed", 0U, 0U, 0U, 0U, 0U);
    }

#else
    AmbaPrint_PrintUInt5("Error Imx686 sensor module define!!!!", 0U, 0U, 0U, 0U, 0U);
#endif
#endif

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_Enable
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
static UINT32 IMX686_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    AmbaMisra_TouchUnused(&pChan);

    /* Config mipi phy */
    // AmbaDSP_VinPhySetMIPI(pChan->VinID);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_Disable
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
static UINT32 IMX686_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    AmbaMisra_TouchUnused(&pChan);

    /* Sensor is at Standby mode */
    IMX686_SetStandbyOn(pChan);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_GetStatus
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
static UINT32 IMX686_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AmbaMisra_TouchUnused(&pChan);

    if (pStatus == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &IMX686Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_GetModeInfo
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
static UINT32 IMX686_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    AmbaMisra_TouchUnused(&pChan);

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = IMX686Ctrl.Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if (Config.ModeID >= (UINT32) AMBA_SENSOR_IMX686_NUM_MODE) {
            RetVal =SENSOR_ERR_ARG;
        } else {
            IMX686_PrepareModeInfo(&Config, pModeInfo);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_GetDeviceInfo
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
static UINT32 IMX686_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    if (pDeviceInfo == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pDeviceInfo, &IMX686DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    AmbaMisra_TouchUnused(&pChan);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_GetHdrInfo
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
static UINT32 IMX686_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pShutterCtrl);

    if (AmbaWrap_memcpy(pHdrInfo, &IMX686Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_GetCurrentGainFactor
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
static UINT32 IMX686_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    FLOAT AnalogGainFactor = 0.0f;
    FLOAT DigitalGainFactor = 0.0f;
    UINT32 i = 0U;
    UINT32 RetVal = SENSOR_ERR_NONE;

    AmbaMisra_TouchUnused(&pChan);

    if (pGainFactor == NULL) {
        RetVal =  SENSOR_ERR_ARG;
    } else {
        if (IMX686Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX686Ctrl.CurrentAgc[0]);
            DigitalGainFactor = (FLOAT)IMX686Ctrl.CurrentDgc[0] / 256.0f;
            *pGainFactor = AnalogGainFactor * DigitalGainFactor;
        } else if (IMX686Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);
            for (i = 0U; i < (IMX686Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX686Ctrl.CurrentAgc[i]);
                DigitalGainFactor = (FLOAT)IMX686Ctrl.CurrentDgc[i] / 256.0f;
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
 *  @RoutineName:: IMX686_GetCurrentShutterSpeed
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
static UINT32 IMX686_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX686Ctrl.Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 i;

    AmbaMisra_TouchUnused(&pChan);

    if (pExposureTime == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX686Ctrl.CurrentShrCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX686Ctrl.CurrentShrCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    if (DesiredFactor < 1.122807f) { /* 1.0 = IMX686DeviceInfo.MinAnalogGainFactor * IMX686DeviceInfo.MinDigitalGainFactor */
        FLOAT AnalogGainFactor;
        *pAnalogGainCtrl = 112U;
        *pDigitalGainCtrl = 0x100U;
        AnalogGainFactor = 1.122807f;
        *pActualFactor = AnalogGainFactor * 1024.0f * 1024.0f; /* .20 fixed point */
    } else {
        FLOAT AnalogGainFactor;
        FLOAT DigitalGainFactor;
        UINT32 SensorMode = IMX686Ctrl.Status.ModeInfo.Config.ModeID;
        UINT16 ReadoutMode = (UINT16)SensorMode;
        FLOAT MaxAnalogGainFactor = IMX686DeviceInfo.MaxAnalogGainFactor;
        UINT16 AnalogGainCtrlFactor = 0;

        DOUBLE DigitalGainCtrlInDb;
        DOUBLE AnalogGainCtrlInDb;

        if(((UINT16) AMBA_SENSOR_IMX686_9248_6944_30P == ReadoutMode) ||
           ((UINT16) AMBA_SENSOR_IMX686_9248_6944_15P == ReadoutMode)) {
            AnalogGainCtrlFactor = 960U;   //max: 24 dB
            MaxAnalogGainFactor = 16.0f;
        } else {
            AnalogGainCtrlFactor = 1008U;  //max: 36 dB
            MaxAnalogGainFactor = 64.0f;
        }

        DigitalGainFactor = DesiredFactor / MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0) {
            *pDigitalGainCtrl = 0x100U;
            AnalogGainFactor = DesiredFactor;

        } else {
            if (DigitalGainFactor > IMX686DeviceInfo.MaxDigitalGainFactor) {
                DigitalGainFactor = IMX686DeviceInfo.MaxDigitalGainFactor;
            }

            if (AmbaWrap_floor((DigitalGainFactor * 256.0), &DigitalGainCtrlInDb) == ERR_NONE) {
                *pDigitalGainCtrl = (UINT32) DigitalGainCtrlInDb;
                DigitalGainFactor = (FLOAT) DigitalGainCtrlInDb / 256.0f;
                AnalogGainFactor = DesiredFactor / DigitalGainFactor;
            } else {
                AnalogGainFactor = 1.0f;
            }
        }
        if (*pAnalogGainCtrl > AnalogGainCtrlFactor) {
            *pAnalogGainCtrl = AnalogGainCtrlFactor;
        }
        if (AmbaWrap_floor((1024.0 / AnalogGainFactor), &AnalogGainCtrlInDb) == ERR_NONE) {
            *pAnalogGainCtrl = 1024U - (UINT32)AnalogGainCtrlInDb;
        } else {
            *pAnalogGainCtrl = 1U;
        }
    }

    *pActualFactor = (FLOAT)(1024.0f / (1024.0f - (float)*pAnalogGainCtrl)) * ((float)*pDigitalGainCtrl / 256.0f);

#if defined(GAIN_DEBUG)
    //SENSOR_DBG_GAIN("[Vin%d][Hdr:%d][IMX686][GAINCONV] DesiredFactor:%lf, AnalogGainCtrl:%ld, DigitalGainCtrl:%ld, ActualFactor:%ld",
    //    Chan.Bits.VinID, Chan.Bits.HdrID, DesiredFactor, *pAnalogGainCtrl, *pDigitalGainCtrl, *pActualFactor);
    AmbaPrint_PrintUInt5("IMX686][GAINCONV] DesiredFactor:%ld, AnalogGainCtrl:%ld, DigitalGainCtrl:%ld, ActualFactor:%ld", DesiredFactor,  *pAnalogGainCtrl, *pDigitalGainCtrl, *pActualFactor, 0);
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_ConvertGainFactor
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
static UINT32 IMX686_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{

    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &IMX686Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertGainFactor(pDesiredFactor->Gain[0], &(pActualFactor->Gain[0]), &(pGainCtrl->AnalogGain[0]), &(pGainCtrl->DigitalGain[0]));
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                ConvertGainFactor(pDesiredFactor->Gain[i], &(pActualFactor->Gain[i]), &(pGainCtrl->AnalogGain[i]), &(pGainCtrl->DigitalGain[i]));
                RetVal|= IMX686_ConvertWbGainFactor(pDesiredFactor->WbGain[i],&(pActualFactor->WbGain[i]), &(pGainCtrl->WbGain[i]));
                pActualFactor->Gain[i] =(FLOAT)((pActualFactor->Gain[i]) / 1024.0f  / 1024.0f);
                //AmbaPrint_PrintUInt5("IMX686]AnalogGain[%d]: %d", i, pGainCtrl->AnalogGain[i], 0, 0, 0);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}


//static int IMX686_ConvertWbGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_WB_GAIN_s DesiredWbFactor, AMBA_SENSOR_WB_GAIN_s *pActualWbFactor, AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
static UINT32 IMX686_ConvertWbGainFactor(AMBA_SENSOR_WB_GAIN_FACTOR_s DesiredWbFactor, AMBA_SENSOR_WB_GAIN_FACTOR_s *pActualWbFactor, AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{

    //AmbaMisra_TouchUnused((void*)pChan);

    /* Convert to 16-bits iReal */
    pWbGainCtrl->R =  (UINT32) ((UINT32)DesiredWbFactor.R  * 256U);
    pWbGainCtrl->Gr = (UINT32) ((UINT32)DesiredWbFactor.Gr * 256U);
    pWbGainCtrl->Gb = (UINT32) ((UINT32)DesiredWbFactor.Gb * 256U);
    pWbGainCtrl->B =  (UINT32) ((UINT32)DesiredWbFactor.B  * 256U);

    /* Clamp maximal gain to 255.996 */
    pWbGainCtrl->R  = MIN(pWbGainCtrl->R,  0xFFFFU);
    pWbGainCtrl->Gr = MIN(pWbGainCtrl->Gr, 0xFFFFU);
    pWbGainCtrl->Gb = MIN(pWbGainCtrl->Gb, 0xFFFFU);
    pWbGainCtrl->B  = MIN(pWbGainCtrl->B,  0xFFFFU);

    /* Convert from iReal to .12 fixed point */
    pActualWbFactor->R  = (FLOAT) pWbGainCtrl->R  / 256.0f;
    pActualWbFactor->Gr = (FLOAT) pWbGainCtrl->Gr / 256.0f;
    pActualWbFactor->Gb = (FLOAT) pWbGainCtrl->Gb / 256.0f;
    pActualWbFactor->B  = (FLOAT) pWbGainCtrl->B  / 256.0f;


#if defined(GAIN_DEBUG)
    //SENSOR_DBG_GAIN("[Vin%d][IMX686][WB] DesireWBFactor:(%f,%f,%f,%f), ActualWBFactor:(%f,%f,%f,%f), WBGainCtrl:(0x%04X,0x%04x,0x%04x,0x%04x)",Chan.Bits.VinID,
    //          DesiredWbFactor.R, DesiredWbFactor.Gr, DesiredWbFactor.Gb, DesiredWbFactor.B,
    //          pActualWbFactor->R, pActualWbFactor->Gr, pActualWbFactor->Gb, pActualWbFactor->B,
    //          pWbGainCtrl->R, pWbGainCtrl->Gr, pWbGainCtrl->Gb, pWbGainCtrl->B);
    AmbaPrint_PrintUInt5("IMX686][WB] DesireWBFactor:(%d,%d,%d,%d)", DesiredWbFactor.R, DesiredWbFactor.Gr, DesiredWbFactor.Gb, DesiredWbFactor.B, 0);
    AmbaPrint_PrintUInt5("IMX686][WB] ActualWBFactor:(%d,%d,%d,%d)", pActualWbFactor->R, pActualWbFactor->Gr, pActualWbFactor->Gb, pActualWbFactor->B, 0);
    AmbaPrint_PrintUInt5("IMX686][WB] WBGainCtrl:((%d,%d,%d,%d))", pWbGainCtrl->R, pWbGainCtrl->Gr, pWbGainCtrl->Gb, pWbGainCtrl->B, 0);
#endif

    return OK;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_ConvertShutterSpeed
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
static UINT32 ConvertShutterSpeed(const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX686Ctrl.Status.ModeInfo;
    //UINT32 NumExposureStepPerFrame = pModeInfo->FrameLengthLines;
    //UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 MaxExposureLineNumber = 0;
    UINT32 MinExposureLineNumber = 0;
    UINT32 SensorMode = IMX686Ctrl.Status.ModeInfo.Config.ModeID;
    UINT16 ReadoutMode = (UINT16)SensorMode;
    DOUBLE                      CtrlInDb;

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        if (AmbaWrap_floor((DOUBLE)*pDesiredExposureTime/ pModeInfo-> RowTime, &CtrlInDb) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
        *pShutterCtrl = (UINT32) CtrlInDb;

        MaxExposureLineNumber = (UINT32)(((((*pShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - 48U);

        if(((UINT16) AMBA_SENSOR_IMX686_9248_6944_30P == ReadoutMode) ||
           ((UINT16) AMBA_SENSOR_IMX686_9248_6944_15P == ReadoutMode)) {
            MinExposureLineNumber = 5U;
        } else {
            MinExposureLineNumber = 6U;
        }
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

    if(((UINT16) AMBA_SENSOR_IMX686_9248_6944_30P != ReadoutMode) &&
       ((UINT16) AMBA_SENSOR_IMX686_9248_6944_15P != ReadoutMode)) {
        /*Non full frame and non HDR mode shutter should be even number*/
        *pShutterCtrl &= 0xfffffffeU;
    }

    *pActualExposureTime = (FLOAT)*pShutterCtrl * (pModeInfo->RowTime * (FLOAT)pModeInfo->FrameLengthLines / (FLOAT)NumExposureStepPerFrame);

    return RetVal;

}

static UINT32 IMX686_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &IMX686Ctrl.Status.ModeInfo;
    UINT32 i = 0;

    if ((pChan == NULL) || (pDesiredExposureTime == NULL) || (pActualExposureTime == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal |= ConvertShutterSpeed(pDesiredExposureTime, pActualExposureTime, pShutterCtrl);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                RetVal |= ConvertShutterSpeed(&pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_SetAnalogGainCtrl
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
#define MASK_SHUTTER_DGAIN  0
#define FIX_AE_TEST 0

static UINT32 IMX686_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX686Ctrl.Status.ModeInfo;
    UINT8 TxData[2U];

#if defined(AE_DEBUG)
    static UINT32 again0 = 0;
    static UINT32 again1 = 0;
    static UINT32 again2 = 0;
#endif

#if MASK_SHUTTER_DGAIN
    return RetVal;
#endif
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX686Ctrl.CurrentAgc[0] = pAnalogGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);

#if FIX_AE_TEST
            ////test+++
            /* For long exposure frame */
            TxData[0] = (UINT8)((960 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8)(960 & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentAgc[0] = 960;

            /* For middle exposure frame */
            TxData[0] = (UINT8)((960 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (960 & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_MID_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentAgc[1] = 960;

            /* For short exposure frame */
            TxData[0] = (UINT8)((960 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (960 & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_ST_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentAgc[2] = 960;
#else
            /* For long exposure frame */
            TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentAgc[0] = pAnalogGainCtrl[0];

            /* For middle exposure frame */
            TxData[0] = (UINT8)((pAnalogGainCtrl[1] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[1] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_MID_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentAgc[1] = pAnalogGainCtrl[1];

            /* For short exposure frame */
            TxData[0] = (UINT8)((pAnalogGainCtrl[2] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[2] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_ST_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentAgc[2] = pAnalogGainCtrl[2];
#endif

#if defined(AE_DEBUG)
            if ((again0 != pAnalogGainCtrl[0])||
                (again1 != pAnalogGainCtrl[1])||
                (again2 != pAnalogGainCtrl[2])) {
                AmbaPrint_PrintUInt5("IMX686 3-expo AGain: %d %d %d", pAnalogGainCtrl[0], pAnalogGainCtrl[1], pAnalogGainCtrl[2], 0, 0);
                again0 = pAnalogGainCtrl[0];
                again1 = pAnalogGainCtrl[1];
                again2 = pAnalogGainCtrl[2];
            }
#endif

        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_SetDigitalGainCtrl
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
static UINT32 IMX686_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX686Ctrl.Status.ModeInfo;
    UINT8 TxData[2U];

#if defined(AE_DEBUG)
    static UINT32 dgain0 = 0;
    static UINT32 dgain1 = 0;
    static UINT32 dgain2 = 0;
#endif

#if MASK_SHUTTER_DGAIN
    return RetVal;
#endif
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);

            /* Update current AGC control */
            IMX686Ctrl.CurrentDgc[0] = pDigitalGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);

#if FIX_AE_TEST
            /* For long exposure frame */
            TxData[0] = (UINT8)((256 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (256 & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentDgc[0] = 256;

            /* For middle exposure frame */
            TxData[0] = (UINT8)((256 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (256 & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_MID_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentDgc[1] = 256;

            /* For short exposure frame */
            TxData[0] = (UINT8)((256 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (256 & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_ST_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentDgc[2] = 256;
#else
            /* For long exposure frame */
            TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentDgc[0] = pDigitalGainCtrl[0];

            /* For middle exposure frame */
            TxData[0] = (UINT8)((pDigitalGainCtrl[1] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[1] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_MID_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentDgc[1] = pDigitalGainCtrl[1];

            /* For short exposure frame */
            TxData[0] = (UINT8)((pDigitalGainCtrl[2] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pDigitalGainCtrl[2] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16)IMX686_ST_DIG_GAIN_GLOBAL_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentDgc[2] = pDigitalGainCtrl[2];
#endif

#if defined(AE_DEBUG)
            if ((dgain0 != pDigitalGainCtrl[0])||
                (dgain1 != pDigitalGainCtrl[1])||
                (dgain2 != pDigitalGainCtrl[2])) {
                AmbaPrint_PrintUInt5("IMX686 3-expo DGain: %d %d %d", pDigitalGainCtrl[0], pDigitalGainCtrl[1], pDigitalGainCtrl[2], 0, 0);
                dgain0 = pDigitalGainCtrl[0];
                dgain1 = pDigitalGainCtrl[1];
                dgain2 = pDigitalGainCtrl[2];
            }
#endif

        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_SetShutterCtrl
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
static UINT32 IMX686_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX686Ctrl.Status.ModeInfo;
    UINT8 TxData[2U];

#if defined(AE_DEBUG)
    static UINT32 shutter0 = 0;
    static UINT32 shutter1 = 0;
    static UINT32 shutter2 = 0;
#endif

#if MASK_SHUTTER_DGAIN
    return RetVal;
#endif
    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_COARSE_INTEG_TIME_MSB_REG, TxData, 2);

            /* Update current shutter control */
            IMX686Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) { // fix me
            // AmbaPrint_PrintStr5("%s AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND", __func__, NULL, NULL, NULL, NULL);

#if FIX_AE_TEST
            /* For long exposure frame */
            TxData[0] = (UINT8)((6048 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (6048 & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentShrCtrl[0] = 6048;

            /* For middle exposure frame */
            TxData[0] = (UINT8)((6048 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (6048 & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_MID_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentShrCtrl[1] = 6048;

            /* For short exposure frame */
            TxData[0] = (UINT8)((6048 >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (6048 & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16) IMX686_ST_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentShrCtrl[2] = 6048;
#else
            /* For long exposure frame */
            TxData[0] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16)IMX686_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];

            /* For middle exposure frame */
            TxData[0] = (UINT8)((pShutterCtrl[1] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[1] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16)IMX686_MID_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentShrCtrl[1] = pShutterCtrl[1];

            /* For short exposure frame */
            TxData[0] = (UINT8)((pShutterCtrl[2] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pShutterCtrl[2] & 0x00ffU);
            RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16)IMX686_ST_COARSE_INTEG_TIME_MSB_REG, TxData, 2);
            IMX686Ctrl.CurrentShrCtrl[2] = pShutterCtrl[2];
#endif

#if defined(AE_DEBUG)
            if ((shutter0 != pShutterCtrl[0])||
                (shutter1 != pShutterCtrl[1])||
                (shutter2 != pShutterCtrl[2])) {
                AmbaPrint_PrintUInt5("IMX686 3-expo Shutter: %d %d %d", pShutterCtrl[0], pShutterCtrl[1], pShutterCtrl[2], 0, 0);
                shutter0 = pShutterCtrl[0];
                shutter1 = pShutterCtrl[1];
                shutter2 = pShutterCtrl[2];
            }
#endif

        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_SetSlowShutterCtrl
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
static UINT32 IMX686_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    static UINT32 IMX686_CitLshift = 0;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData[2];
    UINT32 i;

#if MASK_SHUTTER_DGAIN
    return SENSOR_ERR_NONE;
#endif
    AmbaMisra_TouchUnused(&pChan);

    for (i = 0U; i < 8U; i++) {
        if (SlowShutterCtrl <= ((UINT32) 1U << i)) {
            break;
        }
    }
    if (i == 8U) {
        i = 7U;
    }

    IMX686_CitLshift = i;
    TxData[0] = (UINT8) i;
    RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16)IMX686_CIT_LSHIFT_REG, TxData, 1);

    IMX686Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick = (UINT32)(
                IMX686ModeInfoList[IMX686Ctrl.Status.ModeInfo.Config.ModeID].FrameTiming.FrameRate.NumUnitsInTick * ((UINT32)1U << IMX686_CitLshift));

    return RetVal;
}

static UINT32 IMX686_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 TxData[8];

#if MASK_SHUTTER_DGAIN
    return RetVal;
#endif
    if ((pChan == NULL) || (pWbGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

#if FIX_AE_TEST
        TxData[0] = (UINT8)((4096U >> 8U) & 0x00ffU);
        TxData[1] = (UINT8)(4096U & 0x00ffU);
        TxData[2] = (UINT8)((4096U >> 8U) & 0x00ffU);
        TxData[3] = (UINT8)(4096U & 0x00ffU);
        TxData[4] = (UINT8)((4096U >> 8U) & 0x00ffU);
        TxData[5] = (UINT8)(4096U & 0x00ffU);
        TxData[6] = (UINT8)((4096U >> 8U) & 0x00ffU);
        TxData[7] = (UINT8)(4096U & 0x00ffU);
        Retal |= IMX686_RegWrite(pChan->VinID, (UINT16)IMX686_ABS_GAIN_GR_MSB_REG, TxData, 8);
#else
        TxData[0] = (UINT8)((pWbGainCtrl[0].Gr >> 8U) & 0x00ffU);
        TxData[1] = (UINT8)(pWbGainCtrl[0].Gr & 0x00ffU);
        TxData[2] = (UINT8)((pWbGainCtrl[0].R >> 8U) & 0x00ffU);
        TxData[3] = (UINT8)(pWbGainCtrl[0].R & 0x00ffU);
        TxData[4] = (UINT8)((pWbGainCtrl[0].B >> 8U) & 0x00ffU);
        TxData[5] = (UINT8)(pWbGainCtrl[0].B & 0x00ffU);
        TxData[6] = (UINT8)((pWbGainCtrl[0].Gb >> 8U) & 0x00ffU);
        TxData[7] = (UINT8)(pWbGainCtrl[0].Gb & 0x00ffU);
        RetVal |= IMX686_RegWrite(pChan->VinID, (UINT16)IMX686_ABS_GAIN_GR_MSB_REG, TxData, 8);
#endif
    }
    //AmbaPrint_PrintUInt5("IMX686 WBGain: Gr:%d R:%d B:%d Gb:%d", pWbGainCtrl[0].Gr, pWbGainCtrl[0].R, pWbGainCtrl[0].B, pWbGainCtrl[0].Gb, 0);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX686_Config
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
static UINT32 IMX686_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
#ifdef IMX686_CPHY
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
#ifdef IMX686_CPHY
    AMBA_VIN_MIPI_CPHY_PAD_CONFIG_s IMX686PadConfig = {
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };
#else
    AMBA_VIN_MIPI_PAD_CONFIG_s IMX686PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };
#endif

    UINT32 RetVal = SENSOR_ERR_NONE;

    UINT32 SensorMode = pMode->ModeID;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX686Ctrl.Status.ModeInfo;

    if (SensorMode >= (UINT32) AMBA_SENSOR_IMX686_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_PrintUInt5("[IMX686] Config Mode: %d(VinID: %d)", SensorMode, pChan->VinID, 0U, 0U, 0U);

        /* Update status & FrameTime */
        IMX686_PrepareModeInfo(pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        IMX686PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
#ifdef IMX686_CPHY
        RetVal |= AmbaVIN_MipiCphyReset(pChan->VinID, &IMX686PadConfig);
#else
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &IMX686PadConfig);
#endif
        /* INCK start */
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);

        RetVal |= AmbaKAL_TaskSleep(3);

        /* XCLR rising: hardware standby to software standy */
        /* Reset sensor */
        IMX686_HardwareReset(pChan->VinID);

//        IMX686_OpenI2cChannel();

        /* Write golbal reg table after hardware reset */
        RetVal |= IMX686_InitGolbalReg(pChan);

        /* Write image quality reg table after hardware reset */
        RetVal |= IMX686_InitImageQualityReg(pChan);

        /* Write registers of mode change to sensor */
        RetVal |= IMX686_ChangeReadoutMode(pChan, SensorMode);

#if 0
        {
            UINT8 TxData = 0;

            TxData = 0x00;
            IMX686_RegWrite(0x8943, &TxData, 1);
            TxData = 0x01;
            IMX686_RegWrite(0x0b00, &TxData, 1);
            TxData = 0x00;
            IMX686_RegWrite(0x380c, &TxData, 1);
            TxData = 0x80;
            IMX686_RegWrite(0x380d, &TxData, 1);
        }
#endif

        /* preset the parameters of shutter & gain */
        //IMX686_SetSlowShutterCtrl(pChan, 1);
        //IMX686_SetShutterCtrl(pChan, 8);
        //IMX686_SetAnalogGainCtrl(pChan, 112);
        //IMX686_SetDigitalGainCtrl(pChan, 0x100);

        /* Standby cancel */
        IMX686_SetStandbyOff(pChan);

        /* Add delay to wait MIPI data output from sending standby cancel */
        /* T8 = 4.0 ms + The delay of the coarse integration time value */
        RetVal |= AmbaKAL_TaskSleep(5);

        /* Config Vin */
        IMX686_ConfigVin(pChan->VinID, pModeInfo);

#ifdef CONFIG_IMX686_POSTCFG_ENABLED
        IMX686_SetStandbyOn(pChan);
#endif
    }

    return RetVal;
}

#ifdef CONFIG_IMX686_POSTCFG_ENABLED
static UINT32 IMX686_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 RegData = 0x1;

    AmbaMisra_TouchUnused(&pChan);
    IMX686_RegWrite(pChan->VinID, IMX686_MODE_SEL_REG, &RegData, 1);

    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX686Obj = {
    .Init                   = IMX686_Init,
    .Enable                 = IMX686_Enable,
    .Disable                = IMX686_Disable,
    .Config                 = IMX686_Config,
    .GetStatus              = IMX686_GetStatus,
    .GetModeInfo            = IMX686_GetModeInfo,
    .GetDeviceInfo          = IMX686_GetDeviceInfo,
    .GetCurrentGainFactor   = IMX686_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX686_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX686_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX686_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX686_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = IMX686_SetDigitalGainCtrl,
    .SetShutterCtrl         = IMX686_SetShutterCtrl,
    .SetWbGainCtrl          = IMX686_SetWbGainCtrl,
    .SetSlowShutterCtrl     = IMX686_SetSlowShutterCtrl,

    .RegisterRead           = IMX686_RegisterRead,
    .RegisterWrite          = IMX686_RegisterWrite,
    .GetHdrInfo             = IMX686_GetHdrInfo,
#ifdef CONFIG_IMX686_POSTCFG_ENABLED
    .ConfigPost             = IMX686_ConfigPost,
#else
    .ConfigPost             = NULL,
#endif

    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
