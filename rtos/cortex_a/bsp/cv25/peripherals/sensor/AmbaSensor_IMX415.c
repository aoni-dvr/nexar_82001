/**
 *  @file AmbaSensor_IMX415.c
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
 *  @details Control APIs of SONY IMX415 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
//#include <math.h>


#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_IMX415.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "bsp.h"


/*-----------------------------------------------------------------------------------------------*\
 * IMX415 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static IMX415_CTRL_s IMX415Ctrl = {0};
static UINT32 IMX415_IntegrationPeriodInXVS = 1;  /* current intergration period (unit: XVS) */



static UINT32 IMX415I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
       [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
	   [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
       //[AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP AMBA_SENSOR_I2C_CHANNEL
};


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_PrepareModeInfo
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
static void IMX415_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT16 SensorMode                             = pMode->ModeID;
    IMX415_READOUT_MODE_e           ReadoutMode   = IMX415ModeInfoList[SensorMode].ReadoutMode;
    const IMX415_FRAME_TIMING_s     *pFrameTiming = &IMX415ModeInfoList[SensorMode].FrameTiming;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo  = &IMX415OutputInfo[ReadoutMode];
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo   = &IMX415InputInfoNormalReadout[ReadoutMode];

    //pModeInfo->Mode = Mode;
    pModeInfo->LineLengthPck = (pFrameTiming->NumTickPerXhs * pFrameTiming->NumXhsPerH) *
                               (pOutputInfo->DataRate / (74.25 * 1000000)) *
                               (pOutputInfo->NumDataLanes) / (pOutputInfo->NumDataBits);
    pModeInfo->FrameLengthLines = pFrameTiming->NumXhsPerV / pFrameTiming->NumXhsPerH;
    pModeInfo->NumExposureStepPerFrame = pFrameTiming->NumXhsPerV;
    //pModeInfo->PixelRate        = (float) pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
	pModeInfo->InputClk = pFrameTiming->InputClk;
    pModeInfo->RowTime = (float) (pFrameTiming->NumTickPerXhs * pFrameTiming->NumXhsPerH) / (74.25 * 1000000); //sensor internal clk

	//(void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    /* HDR information */
    (void)AmbaWrap_memset(&pModeInfo->HdrInfo, 0, sizeof(AMBA_SENSOR_HDR_INFO_s)); // Not support HDR

    /* updated minimum frame rate limitation */
    pModeInfo->MinFrameRate.NumUnitsInTick *= 0xffff / pModeInfo->FrameLengthLines;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_ConfigVin
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
static UINT32 IMX415_ConfigVin(const UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s IMX415VinConfig = {
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
        .DataType = 0x2CU,
        .DataTypeMask = 0x00U,//0x3fU, //
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0}
    };
    //-----------------------------------------------------------------------------------------------//

    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &IMX415VinConfig;

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
 *  @RoutineName:: IMX415_HardwareReset
 *
 *  @Description:: Reset IMX335 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 IMX415_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);

    if (RetVal == GPIO_ERR_NONE) {
        (void)AmbaKAL_TaskSleep(2);
        RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
    }
	return RetVal;
}
//static UINT32 RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData, UINT32 Size)

static UINT32 IMX415_RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
	UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = IMX415_SENSOR_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = ((UINT32)IMX415_SENSOR_I2C_SLAVE_ADDR | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;

#if 0  /* to be refined with bsp file */
    RetVal = AmbaI2C_MasterReadAfterWrite(IMX415I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                 &I2cRxConfig, &TxSize, 1000U);
#endif
    RetVal = AmbaI2C_MasterReadAfterWrite(IMX415I2cChannel[VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                 &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX415]READ I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_RegisterRead
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
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
		UINT32 RetVal = SENSOR_ERR_NONE;
		UINT8 RData;

		if (pChan == NULL) {
			RetVal = SENSOR_ERR_ARG;
		} else {
			RetVal = IMX415_RegRead(pChan->VinID, Addr, &RData, 1U);
			*Data = RData;
		}

		return RetVal;
}



static UINT32 IMX415_RegWrite(UINT32 VinID, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[IMX415_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;

    if (Size > IMX415_SENSOR_I2C_MAX_SIZE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = IMX415_SENSOR_I2C_SLAVE_ADDR;
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(IMX415I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                            &I2cConfig, &TxSize, 1000U);
#endif
        RetVal = AmbaI2C_MasterWrite(IMX415I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                            &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[IMX415] WRITE I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX283][IO] Addr = 0x%04x, Data = 0x%02x", Addr, pTxData[0], 0, 0, 0);
    //SENSOR_DBG_IO("[IMX283][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_RegisterWrite
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
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
		UINT32 RetVal = SENSOR_ERR_NONE;
		UINT8 WData = (UINT8)Data;

		if (pChan == NULL) {
			RetVal = SENSOR_ERR_ARG;
		} else {
			RetVal = IMX415_RegWrite(pChan->VinID, Addr, &WData, 1U);
		}

		return RetVal;
}




/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_SetStandbyOn(UINT32 VinID)
{
	UINT32 RetVal;
    UINT8 WData = 0x01U;
    RetVal = IMX415_RegWrite(VinID, IMX415_STANDBY_REG, &WData, 1U);

	return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_SetStandbyOff(UINT32 VinID)
{
    UINT32 RetVal;
    UINT8 WData = 0x00U;
    RetVal = IMX415_RegWrite(VinID,IMX415_STANDBY_REG, &WData, 1);
    //Wait for internal regulator stabilization: 18ms delay
    AmbaKAL_TaskSleep(18);
	return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *      ReadoutMode:   Sensor readout mode number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_ChangeReadoutMode(UINT32 VinID, UINT32 ModeID)
{
	IMX415_READOUT_MODE_e ReadoutMode = IMX415ModeInfoList[ModeID].ReadoutMode;
    UINT32 RetVal = SENSOR_ERR_NONE;
    int i;


    AmbaPrint_PrintUInt5("[IMX415_ChangeReadoutMode] VinID=%d, ModeID=%d", VinID, ModeID, 0, 0, 0);

	for (i = 0; i < IMX415_NUM_READOUT_MODE_REG; i++) {
		RetVal |= IMX415_RegWrite(VinID,IMX415ModeRegTable[i].Addr, &IMX415ModeRegTable[i].Data[ReadoutMode], 1);

		//AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX415_ChangeReadoutMode] IMX415ModeRegTable[%d].Addr: 0x%x, IMX415ModeRegTable[%d].Data: 0x%x",i, IMX415ModeRegTable[i].Addr, i, IMX415ModeRegTable[i].Data[ReadoutMode], 0U);
	}
    /* reset gain/shutter ctrl information */
    IMX415Ctrl.CurrentShutterCtrl[0]= 0xffffffff;
    IMX415Ctrl.CurrentPgc[0] = 0xffffffff;
    //reset exposure frame
    IMX415_IntegrationPeriodInXVS = 1;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
/*static void IMX415_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < AMBA_SENSOR_IMX415_NUM_MODE; i++) {

        IMX415ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)IMX415ModeInfoList[i].FrameTiming.NumTickPerXhs
                                           * IMX415ModeInfoList[i].FrameTiming.NumXhsPerH
                                           / (74.25 * 1000000));

        //AmbaPrint("row time: %f", IMX577ModeInfoList[i].RowTime);

    }
}*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX577_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
	UINT32 RetVal;
    const IMX415_FRAME_TIMING_s *pFrameTime = &IMX415ModeInfoList[0].FrameTiming;
	if (pChan == NULL) {
		RetVal = SENSOR_ERR_ARG;
    } else {
#if 0  /* to be refined with bsp file */
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
#endif
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
       	//IMX415_PreCalculateModeInfo();
    }


    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    }
	else {
        ;//RetVal = IMX283_HardwareReset(pChan->VinID);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      Chan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = IMX415_SetStandbyOn(pChan->VinID);
    }

    //AmbaPrint("[IMX415] IMX415_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_GetStatus
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &IMX415Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_GetModeInfo
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = IMX415Ctrl.Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= AMBA_SENSOR_IMX415_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            IMX415_PrepareModeInfo(pMode, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_GetDeviceInfo
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &IMX415DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_GetHdrInfo
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
static UINT32 IMX415_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pHdrInfo, &IMX415Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_GetCurrentGainFactor
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
		UINT32 RetVal = SENSOR_ERR_NONE;
		//FLOAT AnalogGainFactor = 0.0f;
		//FLOAT DigitalGainFactor = 0.0f;
		UINT8 i;
		DOUBLE ActualFactor64 = 0.0;

		if (pChan == NULL) {
			RetVal = SENSOR_ERR_ARG;
		} else {
			if (IMX415Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
				//AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX577Ctrl.CurrentAgcCtrl[0]);
				//DigitalGainFactor = (FLOAT)IMX577Ctrl.CurrentDgcCtrl[0] / 256.0f;
				//*pGainFactor = AnalogGainFactor * DigitalGainFactor;

				(void)AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)IMX415Ctrl.CurrentPgc[0]), &ActualFactor64);
				*pGainFactor = (float)ActualFactor64;

			} else if (IMX415Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
				for (i = 0U; i < (IMX415Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
					//AnalogGainFactor = 1024.0f / ( 1024.0f - (FLOAT)IMX577Ctrl.CurrentAgcCtrl[i]);
					//DigitalGainFactor = (FLOAT)IMX577Ctrl.CurrentDgcCtrl[i] / 256.0f;
					//pGainFactor[i] = AnalogGainFactor * DigitalGainFactor;
					(void)AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)IMX415Ctrl.CurrentPgc[i]), &ActualFactor64);
					pGainFactor[i] =(float)ActualFactor64;
				}
			} else {
				RetVal = SENSOR_ERR_INVALID_API;
			}
		}
		//AmbaPrint("GainFactor:%f", *pGainFactor);

		return RetVal;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_GetCurrentShutterSpeed
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
		const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX415Ctrl.Status.ModeInfo;
		UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
		UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
		UINT8 i;

		UINT32 RetVal = SENSOR_ERR_NONE;
		if (pChan == NULL) {
			RetVal = SENSOR_ERR_ARG;
		} else {
			if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
				pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX415Ctrl.CurrentShutterCtrl[0];
			} else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
				for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
					pExposureTime[i] = (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame) * (FLOAT)IMX415Ctrl.CurrentShutterCtrl[i];
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
	UINT32 Pgc = 1; //only PGC, not Agc or Dgc only
	DOUBLE LogDesiredFactor = 1.0;
	DOUBLE ActualFactor64 = 0.0;

    //if (pActualFactor == NULL || pAnalogGainCtrl == NULL || pDigitalGainCtrl == NULL)
        //return NG;

    //DB = 20*log(Factor), reg = DB*10/3

    if (DesiredFactor < 1.0) {
        //Pgc= (float)20.0 * log10(1) * 10 / 3;
        (void)AmbaWrap_log10(1, &LogDesiredFactor);
		Pgc=(float)20.0 * LogDesiredFactor * 10 / 3;

    } else {
    	(void)AmbaWrap_log10(DesiredFactor, &LogDesiredFactor);
		Pgc = (float)20.0 * LogDesiredFactor * 10 / 3;
        //Pgc = (float)20.0 * log10(DesiredFactor) * 10 / 3;
        if(Pgc > 0xf0){
            Pgc = 0xf0;
        }
    }

    *pAnalogGainCtrl = Pgc;
    *pDigitalGainCtrl = 1;

    //reg*3/10 = DB, factor = 10^(DB/20)

	(void)AmbaWrap_pow(10.0, 0.015 * ((DOUBLE)Pgc), &ActualFactor64);
    *pActualFactor =(FLOAT)( ActualFactor64); /* ActualFactor .20 fixed point */

    //AmbaPrint("DesiredFactor:%f, pActualFactor:0x%08x", DesiredFactor,  *pActualFactor);
    //AmbaPrint("Agc = 0x%08x, Dgc = 0x%08x", Agc, Dgc);
 }



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_ConvertGainFactor
 *
 *  @Description:: Convert gain factor to analog and digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      DesiredFactor:      Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      Achievable gain factor (12.20 fixed point)
 *      pAnalogGainCtrl:    Analog and Digital gain control for achievable gain factor
 *      pDigitalGainCtrl:   Digital gain is not used
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &IMX415Ctrl.Status.ModeInfo;
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
	//AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX415Ctrl.ModeInfo;
    //IMX415_READOUT_MODE_e ReadoutMode = IMX415ModeInfoList[pModeInfo->Mode.Bits.Mode].ReadoutMode;
    IMX415_READOUT_MODE_e SensorMode = IMX415Ctrl.Status.ModeInfo.Config.ModeID;
    const IMX415_FRAME_TIMING_s *pFrameTiming = &IMX415ModeInfoList[SensorMode].FrameTiming;
    UINT32 ExposureFrames       = 0;
    UINT32 ExpXhsNum            = 0;
    UINT32 MinExpXhsNum         = 1;
    UINT32 MaxExpXhsNum         = pModeInfo->NumExposureStepPerFrame - 4;
    UINT32 MaxFrameLengthLines  = 0xfffff;   //FrameLengthLines reg boundary
    UINT32 FrameLengthLines = pModeInfo->NumExposureStepPerFrame;

    //if (pShutterCtrl == NULL)
        //return NG;

    ExpXhsNum = (UINT32)(ExposureTime / pModeInfo->RowTime * pFrameTiming->NumXhsPerH);

	if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {

    	ExposureFrames = (ExpXhsNum / pModeInfo->NumExposureStepPerFrame);
    	ExposureFrames = (ExpXhsNum % pModeInfo->NumExposureStepPerFrame) ? ExposureFrames + 1 : ExposureFrames;

    	FrameLengthLines = ExposureFrames * pModeInfo->NumExposureStepPerFrame;

    	if(FrameLengthLines > MaxFrameLengthLines)
    	{
        	ExposureFrames = MaxFrameLengthLines / pModeInfo->NumExposureStepPerFrame;
    	}

        	MaxExpXhsNum = ExposureFrames * pModeInfo->NumExposureStepPerFrame - 8;
        	MinExpXhsNum = 4;
	}else if(pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {


        MaxExpXhsNum = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MaxExpXhsNum = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
    } else {
        /* Dummy value */
        ExpXhsNum = 1;
        MaxExpXhsNum = 1;
        MinExpXhsNum = 1;

        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[NG] Wrong HdrType!!!!!", NULL, NULL, NULL, NULL, NULL);
    }


    if (ExpXhsNum > MaxExpXhsNum)
    	{
        	*pShutterCtrl = MaxExpXhsNum;
    	}
    else if (ExpXhsNum < MinExpXhsNum)
    	{
        	*pShutterCtrl = MinExpXhsNum;
    	}
    else
    	{
        	*pShutterCtrl = ExpXhsNum;
    	}

	*pActualExptime = (FLOAT)*pShutterCtrl * (pModeInfo->RowTime * (FLOAT)FrameLengthLines / (FLOAT)pModeInfo->NumExposureStepPerFrame);

	//AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "ConvertShutterSpeed SensorMode: %d !!!!!", SensorMode, 0U, 0U, 0U, 0U);

}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_ConvertShutterSpeed
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
		UINT32 RetVal = SENSOR_ERR_NONE;
		const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX415Ctrl.Status.ModeInfo;
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
 *  @RoutineName:: IMX415_SetAnalogGainCtrl
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{

    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX415Ctrl.Status.ModeInfo;
    UINT8 TxData[1U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
			TxData[0] = (UINT8)(pAnalogGainCtrl[0] & 0x000000ff);
    		IMX415_RegWrite(pChan->VinID, IMX415_ANA_GAIN_GLOBAL_LSB_REG, TxData, 1);
            /* Update current AGC control */
            IMX415Ctrl.CurrentPgc[0] = pAnalogGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
        TxData[0] = (UINT8)(pAnalogGainCtrl[0] & 0x000000ff);
            RetVal = IMX415_RegWrite(pChan->VinID, IMX415_ANA_GAIN_GLOBAL_LSB_REG, TxData, 1);
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
        //SENSOR_DBG_GAIN("[Vin%d/Sensor%d ][IMX283][GAIN] AnalogGainCtrl = 0x%x",
        //                  pChan->VinID, pChan->SensorID,   AnalogGainCtrl);
    }


    return RetVal;

}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_SetShutterCtrl
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
#if 1
		UINT32 RetVal = SENSOR_ERR_NONE;
		const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX415Ctrl.Status.ModeInfo;
		//IMX415_READOUT_MODE_e SensorMode = IMX415Ctrl.Status.ModeInfo.Config.ModeID;
		UINT32 NumXhsPerXvs = IMX415Ctrl.ModeInfo.NumExposureStepPerFrame;
		UINT32 SVR; /* specifies the exposure end vertical period */
		UINT32 SHR; /* specifies the exposure start horizontal period */
		UINT8 TxData[3U];

		if ((pChan == NULL) || (pShutterCtrl == NULL)) {
			RetVal = SENSOR_ERR_ARG;
		} else {
			if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
				SVR = IMX415_IntegrationPeriodInXVS - 1;
    			SHR = (SVR + 1) * NumXhsPerXvs - pShutterCtrl[0];

    			TxData[0] = SHR & 0x000000ff;
    			TxData[1] = (SHR >> 8) & 0x000000ff;
    			TxData[2] = (SHR >> 16) & 0x0000000f;

    			IMX415_RegWrite(pChan->VinID,IMX415_COARSE_INTEG_TIME_LSB_REG, TxData, 3);

				/* Update current shutter control */
				IMX415Ctrl.CurrentShutterCtrl[0] = SHR;
			} else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
				RetVal |= IMX415_RegWrite(pChan->VinID, IMX415_COARSE_INTEG_TIME_LSB_REG, TxData, 3U);
			} else {
				RetVal = SENSOR_ERR_INVALID_API;
			}

			//AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "IMX283_SetShutterCtrl SensorMode: %d !!!!!", SensorMode, 0U, 0U, 0U, 0U);

		//	  SENSOR_DBG_SHR("[Vin%d/Sensor%d][IMX283][SHR] *pShutterCtrl = 0x%x, *(pShutterCtrl+1) = 0x%x, *(pShutterCtrl+2) = 0x%x",
		//						pChan->VinID, pChan->SensorID,	*pShutterCtrl, *(pShutterCtrl+1), *(pShutterCtrl+2));
		}
#endif
		return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_SetSlowShutterCtrl
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
#if 1
	UINT32 RetVal = SENSOR_ERR_NONE;

	UINT32 TargetFrameLengthLines;
    UINT8 TxData[3];

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    //1H = 2*XHS
    TargetFrameLengthLines = IMX415Ctrl.ModeInfo.NumExposureStepPerFrame * SlowShutterCtrl;

    IMX415_IntegrationPeriodInXVS = SlowShutterCtrl;
    if(TargetFrameLengthLines > 0xfffff){

        IMX415_IntegrationPeriodInXVS =  0xfffff / IMX415Ctrl.ModeInfo.NumExposureStepPerFrame;
        TargetFrameLengthLines = IMX415_IntegrationPeriodInXVS * IMX415Ctrl.ModeInfo.NumExposureStepPerFrame;
    }

    TxData[0] = (UINT8)(TargetFrameLengthLines & 0x000000ff);
    TxData[1] = (UINT8)((TargetFrameLengthLines >> 8) & 0x000000ff);
    TxData[2] = (UINT8)((TargetFrameLengthLines >> 16) & 0x0000000f);

    RetVal = IMX415_RegWrite(pChan->VinID, 0x3024, TxData, 3U);;

    /* Update frame rate information */
    IMX415Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines * IMX415Ctrl.ModeInfo.FrameLengthLines / IMX415Ctrl.ModeInfo.NumExposureStepPerFrame;
    IMX415Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    IMX415Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick = IMX415Ctrl.ModeInfo.FrameRate.NumUnitsInTick * SlowShutterCtrl;

    //AmbaPrint("SlowShutterCtrl=%d, TargetFrameLengthLines=%d", SlowShutterCtrl, TargetFrameLengthLines);
    //AmbaPrint("[SS] Reg0x0340 = 0x%02x, Reg0x0341 = 0x%02x", TxData[0], TxData[1]);
#endif

		return RetVal;

}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX415_Config
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IMX415_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
	UINT8 Data = 0;

    UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s IMX415PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &IMX415Ctrl.Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;

	AmbaPrint_ModuleSetAllowList(SENSOR_MODULE_ID, 1);

    if (ModeID >= AMBA_SENSOR_IMX415_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[IMX415] Config Mode: %d, VinID: %d, SensorID: %d",ModeID, pChan->VinID, pChan->SensorID, 0U, 0U);

        /* update status */
        IMX415_PrepareModeInfo(pMode, pModeInfo);
		(void)AmbaWrap_memcpy(&IMX415Ctrl.ModeInfo, pModeInfo, sizeof(AMBA_SENSOR_MODE_INFO_s));

        /* Adjust mipi-phy parameters */
        IMX415PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
		AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", IMX415PadConfig.DateRate, 0U, 0U, 0U, 0U);
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &IMX415PadConfig);

        /* set pll */
#if 0  /* to be refined with bsp file */

        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
#endif
		AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  InputClk: %d  ============", pModeInfo->InputClk, 0U, 0U, 0U, 0U);
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);

        (void)AmbaKAL_TaskSleep(30);

        /* reset sensor */
        (void)IMX415_HardwareReset(pChan->VinID);

        /* program sensor */

        /* Write golbal reg table after hardware reset */
        //(void)IMX283_InitGolbalReg(pChan->VinID);

		//AmbaKAL_TaskSleep(1);
		//Data = 0x00;
		//RegWrite(pChan->VinID,0x3000, &Data, 1);
		AmbaKAL_TaskSleep(19);

        /* Write registers of mode change to sensor */
        (void)IMX415_ChangeReadoutMode(pChan->VinID, ModeID);

        (void)IMX415_SetStandbyOff(pChan->VinID);

		//5. set MASTER mode
    	Data = 0;
    	IMX415_RegWrite(pChan->VinID,IMX415_XMSTA_REG, &Data, 1);


        /* config vin */
        RetVal |= IMX415_ConfigVin(pChan->VinID, pModeInfo);

		AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  RetVal: %d  ============", RetVal, 0U, 0U, 0U, 0U);

    }

    return RetVal;
}

static UINT32 IMX415_SetRotation(const AMBA_SENSOR_CHANNEL_s *pChan, UINT8 Rotaion)
{
    UINT16 Value = 0;

    IMX415_RegisterRead(pChan, 0x3030, &Value);
    if (Rotaion == 0) {
        Value = (Value & 0xFC) | 0x00;
    } else if (Rotaion == 1) {
        Value = (Value & 0xFC) | 0x01;
    } else if (Rotaion == 2) {
        Value = (Value & 0xFC) | 0x02;
    } else if (Rotaion == 3) {
        Value = (Value & 0xFC) | 0x03;
    }
    IMX415_RegisterWrite(pChan, 0x3030, Value);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_IMX415Obj = {
    .SensorName             = "IMX415",
    .SerdesName             = "NA",
    .Init                   = IMX415_Init,
    .Enable                 = IMX415_Enable,
    .Disable                = IMX415_Disable,
    .Config                 = IMX415_Config,
    .GetStatus              = IMX415_GetStatus,
    .GetModeInfo            = IMX415_GetModeInfo,
    .GetDeviceInfo          = IMX415_GetDeviceInfo,
	.GetHdrInfo             = IMX415_GetHdrInfo,
    .GetCurrentGainFactor   = IMX415_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = IMX415_GetCurrentShutterSpeed,

    .ConvertGainFactor      = IMX415_ConvertGainFactor,
    .ConvertShutterSpeed    = IMX415_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = IMX415_SetAnalogGainCtrl,
    .SetShutterCtrl         = IMX415_SetShutterCtrl,
    .SetSlowShutterCtrl     = IMX415_SetSlowShutterCtrl,

    .RegisterRead           = IMX415_RegisterRead,
    .RegisterWrite          = IMX415_RegisterWrite,

    .SetRotation            = IMX415_SetRotation
};
