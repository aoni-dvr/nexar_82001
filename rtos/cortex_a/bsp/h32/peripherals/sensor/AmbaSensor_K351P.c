/**
 *  @file AmbaSensor_K351P.c
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
 *  @details Control APIs of SOI K351P CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_K351P.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "bsp.h"


#ifdef CONFIG_K351P_POSTCFG_ENABLED
static UINT32 K351P_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * K351P sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static K351P_CTRL_s K351PCtrl = {0};

static UINT32 K351PI2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_PrepareModeInfo
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
static void K351P_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &K351PInputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &K351POutputInfo[ModeID];
    const K351P_FRAME_TIMING_s         *pFrameTiming   = &K351PModeInfoList[ModeID].FrameTiming;
    UINT32 U32RVal = 0U;

    U32RVal |= AmbaWrap_memset(pModeInfo, 0, sizeof(AMBA_SENSOR_MODE_INFO_s));

    pModeInfo->LineLengthPck            = pFrameTiming->LineLengthPck;
    pModeInfo->FrameLengthLines         = pFrameTiming->FrameLengthLines;
    if (K351PHdrInfo[pMode->ModeID].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
        pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines / 2U;
    } else {
        pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines;
    }
    pModeInfo->InputClk                 = pFrameTiming->InputClk;
    pModeInfo->RowTime                  = K351PModeInfoList[ModeID].RowTime;
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &K351PHdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s));

    /* updated minimum frame rate limitation */
    if (pModeInfo->HdrInfo.HdrType == 0U) {
        pModeInfo->MinFrameRate.TimeScale /= 8U;
    }
    if(U32RVal != 0U) {
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_ConfigVin
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
static UINT32 K351P_ConfigVin(const UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s K351PVinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace = AMBA_VIN_COLOR_SPACE_RGB,
            .BayerPattern = AMBA_SENSOR_BAYER_PATTERN_GR,
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
        .DataType = 0x2BU,
        .DataTypeMask = 0x1fU,
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0}
    };
    //-----------------------------------------------------------------------------------------------//

    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &K351PVinConfig;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;


    if (AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) {
        /* misrac */
    }

    return AmbaVIN_MipiConfig(VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_HardwareReset
 *
 *  @Description:: Reset K351P Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 K351P_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);

    if (RetVal == GPIO_ERR_NONE) {
        if (AmbaKAL_TaskSleep(2) == ERR_NONE) {
            RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}



static UINT32 RegRead(UINT32 VinID, UINT8 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData;
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = K351P_SENSOR_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 1U;
    I2cTxConfig.pDataBuf  = &TxData;
	TxData = Addr;

    I2cRxConfig.SlaveAddr = ((UINT32)K351P_SENSOR_I2C_SLAVE_ADDR | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;

#if 0  /* to be refined with bsp file */
    RetVal = AmbaI2C_MasterReadAfterWrite(K351PI2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);
#endif
    RetVal = AmbaI2C_MasterReadAfterWrite(K351PI2cChannel[VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_PrintStr5("[K351P] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_RegisterRead
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
static UINT32 K351P_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;
    UINT8 RegAddr = 0;

    RegAddr = (UINT8)(Addr & 0xff);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegRead(pChan->VinID, RegAddr, &RData, 1U);
        *Data = RData;
    }

    return RetVal;
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
static UINT32 RegWrite(UINT32 VinID, UINT8 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[K351P_SENSOR_I2C_MAX_SIZE + 1U];
    UINT32 TxSize, i;
	

    if (Size > K351P_SENSOR_I2C_MAX_SIZE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = K351P_SENSOR_I2C_SLAVE_ADDR;
        I2cConfig.DataSize  = (1U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

   
        TxDataBuf[0] = (UINT8) (Addr & 0xffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 1U] = pTxData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(K351PI2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        RetVal = AmbaI2C_MasterWrite(K351PI2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                                     &I2cConfig, &TxSize, 1000U);


#if 0
	{
		UINT8 RData;
		RegRead(VinID, Addr, &RData, 1U);
		AmbaPrint_PrintUInt5("[K351P][IO] Addr = 0x%02x, WData = 0x%02x, RData = 0x%02x", Addr, pTxData[0], RData, 0, 0);
	}
#endif


        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_PrintStr5("[K351P] write I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[K351P][IO] Addr = 0x%04x, Data = 0x%02x", Addr, pTxData[0], 0, 0, 0);
    //SENSOR_DBG_IO("[K351P][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_RegisterWrite
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
static UINT32 K351P_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)(Data & 0xff);
    UINT8 RegAddr = 0;

    RegAddr = (UINT8)(Addr & 0xff);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = RegWrite(pChan->VinID, RegAddr, &WData, 1U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 K351P_SetStandbyOn(UINT32 VinID)
{
    UINT32 RetVal;
    UINT8 WData = 0x40U;

    RetVal = RegWrite(VinID, K351P_MODE_SEL_REG, &WData, 1U);
//    AmbaPrint("[K351P] K351P_SetStandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 K351P_SetStandbyOff(UINT32 VinID)
{
    UINT32 RetVal;
    UINT8 WData = 0x00U;

    RetVal = RegWrite(VinID, K351P_MODE_SEL_REG, &WData, 1U);

    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * K351PCtrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / K351PCtrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[K351P] K351P_SetStandbyOff");
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_ChangeReadoutMode
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
static UINT32 K351P_ChangeReadoutMode(UINT32 VinID)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 i;
	//void*(ModeID);
    
    for (i = 0; i < K351P_NUM_READOUT_MODE_REG; i ++) {
        RetVal |= RegWrite(VinID, K351PRegTable[i].Addr, &K351PRegTable[i].Data, 1U);
//        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", K351PRegTable[i].Addr,
//                             K351PRegTable[i].Data[ModeID], RetVal, 0U, 0U);
    }

    K351PCtrl.CurrentAgcCtrl[0] = 0xffffffffU;
    K351PCtrl.CurrentAgcCtrl[1] = 0xffffffffU;
    K351PCtrl.CurrentDgcCtrl[0] = 0xffffffffU;
    K351PCtrl.CurrentDgcCtrl[1] = 0xffffffffU;
    K351PCtrl.CurrentShrCtrl[0] = 0xffffffffU;
    K351PCtrl.CurrentShrCtrl[1] = 0xffffffffU;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void K351P_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < K351P_NUM_MODE; i++) {
        K351PModeInfoList[i].RowTime = (FLOAT)((DOUBLE)K351PModeInfoList[i].FrameTiming.LineLengthPck /
                                                ((DOUBLE)K351POutputInfo[i].DataRate
                                                        * (DOUBLE)K351POutputInfo[i].NumDataLanes
                                                        / (DOUBLE)K351POutputInfo[i].NumDataBits));

        //AmbaPrint("row time: %f", K351PModeInfoList[i].RowTime);

    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_Init
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
static UINT32 K351P_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const K351P_FRAME_TIMING_s *pFrameTime = &K351PModeInfoList[0U].FrameTiming;

//    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "IMX290 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
#if 0  /* to be refined with bsp file */
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
#endif
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        K351P_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_Enable
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
static UINT32 K351P_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_Disable
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
static UINT32 K351P_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = K351P_SetStandbyOn(pChan->VinID);
    }

    //AmbaPrint("[K351P] K351P_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_GetStatus
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
static UINT32 K351P_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &K351PCtrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_GetModeInfo
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
static UINT32 K351P_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s DesiredMode;

    if ((pChan == NULL) || (pMode ==NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(&DesiredMode, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) == ERR_NONE) {

            if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
                DesiredMode.ModeID = K351PCtrl.Status.ModeInfo.Config.ModeID;
            }

            if ((DesiredMode.ModeID >= K351P_NUM_MODE) ||
                (pModeInfo == NULL)) {
                RetVal = SENSOR_ERR_ARG;
            } else {

                K351P_PrepareModeInfo(&DesiredMode, pModeInfo);
            }
        } else {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_GetDeviceInfo
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
static UINT32 K351P_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pDeviceInfo, &K351PDeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_GetHdrInfo
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
static UINT32 K351P_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pHdrInfo, &K351PCtrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_GetCurrentGainFactor
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
static UINT32 K351P_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 i;
	UINT32 uFactor, dFactor;
	DOUBLE uFactorGainCtrlInDb;
	DOUBLE dFactorGainCtrlInDb;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (K351PCtrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
			uFactor = ((K351PCtrl.CurrentAgcCtrl[0] >> 4) & 0x7);
			dFactor = (K351PCtrl.CurrentAgcCtrl[0] & 0xf);

			AmbaWrap_pow(2.0, uFactor, &uFactorGainCtrlInDb);
			AmbaWrap_floor( (1 + dFactor / 16.0), &dFactorGainCtrlInDb);
			*pGainFactor    = (FLOAT)(uFactorGainCtrlInDb * dFactorGainCtrlInDb);
        } else if (K351PCtrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (K351PCtrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
				uFactor = ((K351PCtrl.CurrentAgcCtrl[i] >> 4) & 0x7);
				dFactor = (K351PCtrl.CurrentAgcCtrl[i] & 0xf);

				AmbaWrap_pow(2.0, uFactor, &uFactorGainCtrlInDb);
				AmbaWrap_floor( (1 + dFactor / 16.0), &dFactorGainCtrlInDb);
				pGainFactor[i]    = (FLOAT)(uFactorGainCtrlInDb * dFactorGainCtrlInDb);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    //AmbaPrint("GainFactor:%f", *pGainFactor);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_GetCurrentShutterSpeed
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
static UINT32 K351P_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &K351PCtrl.Status.ModeInfo;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT8 i;

    UINT32 RetVal = SENSOR_ERR_NONE;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)K351PCtrl.CurrentShrCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)K351PCtrl.CurrentShrCtrl[i];
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}

static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    if (DesiredFactor < 1.0) { /* 1.0 = K351PDeviceInfo.MinAnalogGainFactor * K351PDeviceInfo.MinDigitalGainFactor */
        *pAnalogGainCtrl = 0U;
        *pDigitalGainCtrl = 0x1U;

        *pActualFactor = 1.0f;
    }else if(DesiredFactor > 31.0){
		*pAnalogGainCtrl = 0x4fU;
        *pDigitalGainCtrl = 0x1U;

        *pActualFactor = 31.0f;
	} else {
        FLOAT AnalogGainFactor;
        DOUBLE uFactorGainCtrlInDb;
		DOUBLE dFactorGainCtrlInDb;
		UINT32 uFactor, dFactor;

        AnalogGainFactor = DesiredFactor;

        if (AnalogGainFactor >= 16.0) {
            uFactor = 4;
			dFactor = (UINT32)((DesiredFactor - 16)*1);
        } else if(AnalogGainFactor >= 8.0){
            uFactor = 3;
			dFactor = (UINT32)((DesiredFactor - 8)*2);			
		} else if(AnalogGainFactor >= 4.0){
            uFactor = 2;
			dFactor = (UINT32)((DesiredFactor - 4)*4);			
		} else if(AnalogGainFactor >= 2.0){
            uFactor = 1;
			dFactor = (UINT32)((DesiredFactor - 2)*8);			
		}else {
			uFactor = 0;
			dFactor = (UINT32)((DesiredFactor - 1)*16);	
        }
		
		*pDigitalGainCtrl = 0x1U;
		*pAnalogGainCtrl  = (uFactor << 4) + dFactor;
		
		AmbaWrap_pow(2.0, uFactor, &uFactorGainCtrlInDb);
		AmbaWrap_floor( (1 + dFactor / 16.0), &dFactorGainCtrlInDb);
		*pActualFactor    = (FLOAT)(uFactorGainCtrlInDb * dFactorGainCtrlInDb);
		//*pActualFactor    = pow(2, uFactor) * (1 + dFactor / 16.0) ;

    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_ConvertGainFactor
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
static UINT32 K351P_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &K351PCtrl.Status.ModeInfo;
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
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 MaxExposureLine, MinExposureLine;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ShutterCtrl, ExposureFrames;
    DOUBLE ShutterCtrlInDb;

    if (AmbaWrap_floor((DOUBLE)(ExposureTime / ((DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame)), &ShutterCtrlInDb) == ERR_NONE) {
        ShutterCtrl = (UINT32)ShutterCtrlInDb;

        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            /* Maximun value of FRM_LENGTH_LINES is 65535d */
            ExposureFrames = ((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U;
            if ((FrameLengthLines * ExposureFrames) > 0xffffU) {
                ShutterCtrl = (0xffffU / FrameLengthLines) * NumExposureStepPerFrame;
            }

            /* Maximun value of COARSE_INTEG_TIME is (FRM_LENGTH_LINES - 22) */
            MaxExposureLine= ((((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U) * NumExposureStepPerFrame) - 1U;
            MinExposureLine= 1U;
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
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_ConvertShutterSpeed
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
static UINT32 K351P_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &K351PCtrl.Status.ModeInfo;
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
 *  @RoutineName:: K351P_SetAnalogGainCtrl
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
static UINT32 K351P_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &K351PCtrl.Status.ModeInfo;
    UINT8 TxData[2U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal = RegWrite(pChan->VinID, K351P_ANA_GAIN_GLOBAL_MSB_REG, TxData, 1);

            /* Update current AGC control */
            K351PCtrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            TxData[0] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, K351P_ANA_GAIN_GLOBAL_MSB_REG, TxData, 1);

            TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, K351P_ANA_GAIN_GLOBAL_MSB_REG, TxData, 1);

            /* Update current AGC control */
            K351PCtrl.CurrentAgcCtrl[0] = pAnalogGainCtrl[0];
            K351PCtrl.CurrentAgcCtrl[1] = pAnalogGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][K351P][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_SetDigitalGainCtrl
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
static UINT32 K351P_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &K351PCtrl.Status.ModeInfo;
    //UINT8 TxData[2U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            //TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            //TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            //RetVal = RegWrite(pChan->VinID, K351P_DIG_GAIN_GR_MSB_REG, TxData, 2U);

            /* Update current AGC control */
            K351PCtrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            //TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 8U) & 0x00ffU);
            //TxData[1] = (UINT8) (pDigitalGainCtrl[0] & 0x00ffU);
            //RetVal |= RegWrite(pChan->VinID, K351P_DOL_DIG_GAIN_1ST_MSB_REG, TxData, 2U);

            //TxData[0] = (UINT8)((pDigitalGainCtrl[1] >> 8U) & 0x00ffU);
            //TxData[1] = (UINT8) (pDigitalGainCtrl[1] & 0x00ffU);
            //RetVal |= RegWrite(pChan->VinID, K351P_DOL_DIG_GAIN_2ND_MSB_REG, TxData, 2U);

            /* Update current AGC control */
            K351PCtrl.CurrentDgcCtrl[0] = pDigitalGainCtrl[0];
            K351PCtrl.CurrentDgcCtrl[1] = pDigitalGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][K351P][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_SetShutterCtrl
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
static UINT32 K351P_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &K351PCtrl.Status.ModeInfo;
    UINT8 TxData[2U];

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
			TxData[1] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            RetVal = RegWrite(pChan->VinID, K351P_COARSE_INTEG_TIME_LSB_REG, TxData, 2U);

            /* Update current shutter control */
            K351PCtrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            TxData[0] = (UINT8) (pShutterCtrl[0] & 0x00ffU);
			TxData[1] = (UINT8)((pShutterCtrl[0] >> 8U) & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, K351P_COARSE_INTEG_TIME_LSB_REG, TxData, 2U);

            TxData[0] = (UINT8) (pShutterCtrl[1] & 0x00ffU);
			TxData[1] = (UINT8)((pShutterCtrl[1] >> 8U) & 0x00ffU);
            RetVal |= RegWrite(pChan->VinID, K351P_COARSE_INTEG_TIME_LSB_REG, TxData, 2U);

            /* Update current shutter control */
            K351PCtrl.CurrentShrCtrl[0] = pShutterCtrl[0];
            K351PCtrl.CurrentShrCtrl[1] = pShutterCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        //    SENSOR_DBG_SHR("[Vin%d/Sensor%d][K351P][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
        //                      pChan->VinID, pChan->SensorID,  *pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
    }
    return RetVal;
}
#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_SetSlowShutterCtrl
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
static UINT32 K351P_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
	UINT32 RetVal = SENSOR_ERR_NONE;
	AmbaMisra_TouchUnused(&pChan);
	AmbaMisra_TouchUnused(&SlowShutterCtrl);
#if 0
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = K351PCtrl.Status.ModeInfo.Config.ModeID;
    UINT32 TargetFrameLengthLines = 0U;
    UINT8 TxData[2U];

    if ((pChan == NULL) || (SlowShutterCtrl < 1U))  {
        RetVal = SENSOR_ERR_ARG;
    } else if (K351PCtrl.Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        RetVal = SENSOR_ERR_INVALID_API;
    } else {
        TargetFrameLengthLines = K351PModeInfoList[ModeID].FrameTiming.FrameLengthLines * SlowShutterCtrl;

        TxData[0] = (UINT8)((TargetFrameLengthLines >> 8U) & 0x00ffU);
        TxData[1] = (UINT8) (TargetFrameLengthLines & 0x00ffU);
        RetVal = RegWrite(pChan->VinID, K351P_FRM_LENGTH_LINES_MSB_REG, TxData, 2U);


        /* Update frame rate information */
        K351PCtrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
        K351PCtrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
        K351PCtrl.Status.ModeInfo.FrameRate.NumUnitsInTick = K351PModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick * SlowShutterCtrl;

        //SENSOR_DBG_SHR("[Vin%d/Sensor%d ][K351P][SHR] SlowShutterCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   SlowShutterCtrl);
    }
#endif
    return RetVal;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: K351P_Config
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
static UINT32 K351P_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s K351PPadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &K351PCtrl.Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;

    if (ModeID >= K351P_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_PrintUInt5("[K351P 2lane] yyue 2023061201 Config Mode: %d",ModeID, 0U, 0U, 0U, 0U);

        /* update status */
        K351P_PrepareModeInfo(pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        K351PPadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        RetVal = AmbaVIN_MipiReset(pChan->VinID, &K351PPadConfig);
        if (RetVal == ERR_NONE) {
            /* set pll */
            RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
            if (RetVal == ERR_NONE) {
                RetVal = AmbaKAL_TaskSleep(3);
                if (RetVal == ERR_NONE) {
                    /* reset sensor */
                    RetVal = K351P_HardwareReset(pChan->VinID);

                                if (RetVal == ERR_NONE) {
                                    /* Write registers of mode change to sensor */
                                    RetVal = K351P_ChangeReadoutMode(pChan->VinID);
                                    if (RetVal == ERR_NONE) {
                                        RetVal = K351P_SetStandbyOff(pChan->VinID);
                                        if (RetVal == ERR_NONE) {
                                            /* config vin */
                                            RetVal = K351P_ConfigVin(pChan->VinID, pModeInfo);
#ifdef CONFIG_K351P_POSTCFG_ENABLED
                                            if (RetVal == ERR_NONE) {
                                                RetVal = K351P_SetStandbyOn(pChan->VinID);
                                            }
#endif
                                        }
                                    }
                                }
                            
                        
                    
                }
            }
        }
    }

    return RetVal;
}

#ifdef CONFIG_K351P_POSTCFG_ENABLED
static UINT32 K351P_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    return K351P_SetStandbyOff(pChan->VinID);
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_K351PObj = {
    .SensorName             = "K351P",
    .SerdesName             = "NA",
    .Init                   = K351P_Init,
    .Enable                 = K351P_Enable,
    .Disable                = K351P_Disable,
    .Config                 = K351P_Config,
    .GetStatus              = K351P_GetStatus,
    .GetModeInfo            = K351P_GetModeInfo,
    .GetDeviceInfo          = K351P_GetDeviceInfo,
    .GetHdrInfo             = K351P_GetHdrInfo,
    .GetCurrentGainFactor   = K351P_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = K351P_GetCurrentShutterSpeed,

    .ConvertGainFactor      = K351P_ConvertGainFactor,
    .ConvertShutterSpeed    = K351P_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = K351P_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = K351P_SetDigitalGainCtrl,
    .SetShutterCtrl         = K351P_SetShutterCtrl,
    .SetSlowShutterCtrl     = NULL,//K351P_SetSlowShutterCtrl,

    .RegisterRead           = K351P_RegisterRead,
    .RegisterWrite          = K351P_RegisterWrite,

#ifdef CONFIG_K351P_POSTCFG_ENABLED
    .ConfigPost             = K351P_ConfigPost,
#else
    .ConfigPost             = NULL,
#endif
    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
