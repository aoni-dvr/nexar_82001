/**
 *  @file AmbaSensor_GC2053.c
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
 *  @details Control APIs of OmniVision GC2053 CMOS sensor with MIPI interface
 *
 */
#include <AmbaWrap.h>

//#include <math.h>
//#include "AmbaSSP.h"

//#include "AmbaDSP_VIN.h"
#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_GC2053.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "bsp.h"

#define GC2053_SENSOR_I2C_SLAVE_ADDR 0x6e
#define GC2053_SENSOR_I2C_MAX_SIZE 64U

/*-----------------------------------------------------------------------------------------------*\
 * GC2053 sensor global info
\*-----------------------------------------------------------------------------------------------*/
//static UINT8 SensorSlaveID = 0x6e;

/*-----------------------------------------------------------------------------------------------*\
 * GC2053 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
GC2053_CTRL_s GC2053Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:      Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void GC2053_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
	UINT32 ModeID                                 = pMode->ModeID;
    GC2053_READOUT_MODE_e           ReadoutMode   = GC2053ModeInfoList[ModeID].ReadoutMode;
    const GC2053_FRAME_TIMING_s     *pFrameTiming = &GC2053ModeInfoList[ModeID].FrameTiming;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo  = &GC2053OutputInfo[ReadoutMode];
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo = &GC2053InputInfo[ReadoutMode];

    //pModeInfo->Mode.Data= Mode;
	pModeInfo->LineLengthPck = (UINT32)((UINT64)pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits *
                                                GC2053ModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick /
                                                GC2053ModeInfoList[ModeID].FrameTiming.FrameRate.TimeScale /
                                                GC2053ModeInfoList[ModeID].FrameTiming.FrameLengthLines);
    pModeInfo->FrameLengthLines = GC2053ModeInfoList[ModeID].FrameTiming.FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame = GC2053ModeInfoList[ModeID].FrameTiming.FrameLengthLines;
    //pModeInfo->PixelRate = (float)pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
	pModeInfo->InputClk                 = pFrameTiming->InputClk;
    pModeInfo->RowTime = (pModeInfo->LineLengthPck * pOutputInfo->NumDataBits * 1.0) / (pOutputInfo->NumDataLanes * pOutputInfo->DataRate);
	//pModeInfo->FrameTime.InputClk = GC2053ModeInfoList[ModeID].FrameTiming.InputClk;
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));

    /* HDR information */
    (void)AmbaWrap_memset(&pModeInfo->HdrInfo, 0, sizeof(AMBA_SENSOR_HDR_INFO_s)); // Not support HDR
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_ConfigVin
 *
 *  @Description:: Configure VIN to receive output frames of the new readout mode
 *
 *  @Input      ::
  *     Chan:       Vin ID and sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_ConfigVin(const UINT32 VinID, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
	/*-----------------------------------------------------------------------------------------------*\
	 * Configurations for Amba. VIN side
	\*-----------------------------------------------------------------------------------------------*/
	static AMBA_VIN_MIPI_CONFIG_s GC2053VinConfig = {
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
		.NumActiveLanes         = 4,
		.DataType               = 0x2B,
		.DataTypeMask           = 0x3f,
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0}
	};

	const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &GC2053VinConfig;

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
 *  @RoutineName:: GC2053_HardwareReset
 *
 *  @Description:: Reset GC2053 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 GC2053_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);

    if (RetVal == GPIO_ERR_NONE) {
        (void)AmbaKAL_TaskSleep(2);
        RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
    }

    return RetVal;
}

static UINT32 get_i2c_channel(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
    extern unsigned int get_sensor_i2c_channel(void);
    return get_sensor_i2c_channel();
#else
    return AMBA_I2C_CHANNEL1;
#endif
}

static UINT32 GC2053_RegRead(UINT8 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    //UINT16 pTxData[4];
    //UINT8  RxData[2];
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData;
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = GC2053_SENSOR_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 1U;
    I2cTxConfig.pDataBuf  = &TxData;
    //TxData[0] = (UINT8)(Addr >> 8U);
    TxData = Addr;

    I2cRxConfig.SlaveAddr = ((UINT32)GC2053_SENSOR_I2C_SLAVE_ADDR | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;

    //pTxData[0] = AMBA_I2C_RESTART_FLAG | GC2053_SENSOR_I2C_SLAVE_ADDR;
    //pTxData[1] = (Addr & 0xff);  /* Register Address [7:0]  */
    //pTxData[2] = AMBA_I2C_RESTART_FLAG | (GC2053_SENSOR_I2C_SLAVE_ADDR + 1);

    //RetVal = AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL1, AMBA_I2C_SPEED_FAST,
    //                              3, (UINT16 *) pTxData, 1, RxData, AMBA_KAL_WAIT_FOREVER);

    //*pRxData = RxData[0];

#if 0  /* to be refined with bsp file */
    RetVal = AmbaI2C_MasterReadAfterWrite(AMBA_I2C_CHANNEL1, AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                        &I2cRxConfig, &TxSize, 1000U);
#endif
    RetVal = AmbaI2C_MasterReadAfterWrite(get_i2c_channel(), AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                        &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[GC2053] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_RegisterRead
 *
 *  @Description:: Read sensor registers API
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      pData:  Pointer to register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData;
    UINT8 RegAddr = 0;

    RegAddr = (UINT8)(Addr & 0xff);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = GC2053_RegRead(RegAddr, &RData, 1U);
        *pData = RData;
    }
    return RetVal;
}


static UINT32 GC2053_RegWrite(UINT8 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal = I2C_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[GC2053_SENSOR_I2C_MAX_SIZE + 1U];
    UINT32 TxSize, i;

    if (Size > GC2053_SENSOR_I2C_MAX_SIZE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = GC2053_SENSOR_I2C_SLAVE_ADDR;
        I2cConfig.DataSize  = (1U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = Addr;

        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 1U] = pTxData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(AMBA_I2C_CHANNEL1, AMBA_I2C_SPEED_STANDARD,
                            &I2cConfig, &TxSize, 1000U);
#endif
        RetVal = AmbaI2C_MasterWrite(get_i2c_channel(), AMBA_I2C_SPEED_STANDARD,
                            &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[GC2053] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }

    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[GC2053][IO] Addr = 0x%04x, Data = 0x%02x", Addr, pTxData[0], 0, 0, 0);
    //SENSOR_DBG_IO("[GC2053][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_RegisterWrite
 *
 *  @Description:: Write sensor registers API
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
static UINT32 GC2053_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)(Data & 0xff);
    UINT8 RegAddr = 0;

    RegAddr = (UINT8)(Addr & 0xff);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = GC2053_RegWrite(RegAddr, &WData, 1U);
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_SetAnalogGainReg
 *
 *  @Description:: Apply electronic analog gain setting
 *
 *  @Input      ::
 *      AnalogGainCtrl:    Electronic analog gain control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void GC2053_SetAnalogGainReg(UINT32 AnalogGainCtrl)
{
    UINT8 RegData = 0x00;

	GC2053_RegWrite(0xfe,&RegData,1);
	GC2053_RegWrite(0xb4,&Gc2053RegValTable[AnalogGainCtrl][0],1);
	GC2053_RegWrite(0xb3,&Gc2053RegValTable[AnalogGainCtrl][1],1);
	GC2053_RegWrite(0xb8,&Gc2053RegValTable[AnalogGainCtrl][2],1);
	GC2053_RegWrite(0xb9,&Gc2053RegValTable[AnalogGainCtrl][3],1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_SetDigitalGainReg
 *
 *  @Description:: Apply electronic Digital gain setting
 *
 *  @Input      ::
 *      AnalogGainCtrl:    Electronic Digital gain control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void GC2053_SetDigitalGainReg(UINT32 DigitalGainCtrl)
{
    UINT8 RegData = 0x00;
    UINT8 DigitalGainVal[2];

    DigitalGainVal[0] = (DigitalGainCtrl>>6);
    DigitalGainVal[1] = ((DigitalGainCtrl&0x3f)<<2);

	GC2053_RegWrite(0xfe,&RegData,1);
	GC2053_RegWrite(0xb1,&DigitalGainVal[0],1);
	GC2053_RegWrite(0xb2,&DigitalGainVal[1],1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      ShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void GC2053_SetShutterReg(UINT32 ShutterCtrl)
{
    UINT8 RegData = 0x00;
    UINT8 ShutterCtrlVal[2];

    ShutterCtrlVal[0] = (ShutterCtrl >> 8) & 0x3f;
    ShutterCtrlVal[1] = ShutterCtrl & 0xff;
	GC2053_RegWrite(0xfe, &RegData,1);
	GC2053_RegWrite(0x03, &ShutterCtrlVal[0],1);
	GC2053_RegWrite(0x04, &ShutterCtrlVal[1],1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_SetSlowShutterReg
 *
 *  @Description:: Apply slow shutter setting
 *
 *  @Input      ::
 *      SlowShutterCtrl:    Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void GC2053_SetSlowShutterReg(UINT32 SlowShutterCtrl)
{
    UINT8 RegData = 0x00;
    UINT32 TargetFrameLengthLines;
    UINT8 TargetFrameLengthLinesVal_H;
    UINT8 TargetFrameLengthLinesVal_L;

    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    TargetFrameLengthLines = GC2053ModeInfoList[GC2053Ctrl.Status.ModeInfo.Config.ModeID].FrameTiming.FrameLengthLines * SlowShutterCtrl;

    TargetFrameLengthLinesVal_H = (UINT8)((TargetFrameLengthLines >> 8) & 0xff);
    TargetFrameLengthLinesVal_L = (UINT8)(TargetFrameLengthLines & 0xff);

	GC2053_RegWrite(0xfe, &RegData, 1);
    GC2053_RegWrite(0x41, &TargetFrameLengthLinesVal_H, 1);
	GC2053_RegWrite(0x42, &TargetFrameLengthLinesVal_L, 1);

    /* Update frame rate information */
    GC2053Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    GC2053Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    GC2053Ctrl.Status.ModeInfo.FrameRate.TimeScale = GC2053Ctrl.ModeInfo.FrameRate.TimeScale / SlowShutterCtrl;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void GC2053_SetStandbyOn(void)
{
    UINT8 RegVal[2];

    RegVal[0] = 0x00;
    RegVal[1] = 0x81;

	GC2053_RegWrite(0xfe, &RegVal[0], 1);
    GC2053_RegWrite(0x3e, &RegVal[1], 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void GC2053_SetStandbyOff(void)
{
    UINT8 RegVal[2];

    RegVal[0] = 0x00;
    RegVal[1] = 0x91;

	GC2053_RegWrite(0xfe, &RegVal[0], 1);
    GC2053_RegWrite(0x3e, &RegVal[1], 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_Init
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
static UINT32 GC2053_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
	UINT32 RetVal;
    const GC2053_FRAME_TIMING_s *pFrameTime = &GC2053ModeInfoList[0].FrameTiming;

	if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
		//AmbaPLL_SetSensor0Clk(pFrameTime->InputClk);                 /* The default sensor input clock frequency */
		//AmbaPLL_SetSensor0ClkDir(AMBA_PLL_SENSOR_REF_CLK_OUTPUT);    /* Config clock output to sensor */
		RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
	}
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "================[GC2053][IO] INIT Vin(%d)================", pChan->VinID, 0, 0, 0, 0);
    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
	UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
		//if (!GC2053Ctrl.Status.DevStat.Bits.Sensor0Power) {
        //GC2053_HardwareReset();
        //GC2053Ctrl.Status.DevStat.Bits.Sensor0Power   = 1;
        //GC2053Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;
	}
    /* config mipi phy*/
    //RetVal = AmbaDSP_VinPhySetMIPI(pChan->VinID);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    }

    //if (GC2053Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
    //    return OK;

    GC2053_SetStandbyOn();

    //GC2053Ctrl.Status.DevStat.Bits.Sensor0Standby = 1;  /* Sensor is at Standby mode */

    //GC2053Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_GetStatus
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
static UINT32 GC2053_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pStatus, &GC2053Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }
    //if (pStatus == NULL || GC2053Ctrl.Status.DevStat.Bits.Sensor0Power == 0)
    //    return NG;

    //GC2053Ctrl.Status.TimeToAllPelExposed = 0;  /* TODO: for flash light or mechanical shutter control */

    //memcpy(pStatus, &GC2053Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_GetModeInfo
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
static UINT32 GC2053_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;

    //UINT16 SensorMode = Mode.Bits.Mode;

    //if (SensorMode == AMBA_SENSOR_CURRENT_MODE)
    //    SensorMode = GC2053Ctrl.Status.ModeInfo.Mode.Bits.Mode;

    //if (SensorMode >= AMBA_SENSOR_GC2053_NUM_MODE || pModeInfo == NULL)
    //    return NG;

    //GC2053_PrepareModeInfo(SensorMode, pModeInfo);

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = GC2053Ctrl.Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= AMBA_SENSOR_GC2053_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            GC2053_PrepareModeInfo(pMode, pModeInfo);
        }
    }
    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_GetDeviceInfo
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
static UINT32 GC2053_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &GC2053DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_GetCurrentGainFactor
 *
 *  @Description:: Get Sensor Current Gain Factor
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pGainFactor:    pointer to current gain factor
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, float *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if ((pChan == NULL) || (pGainFactor == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        *pGainFactor = (float)Gc2053GainLevelTable[GC2053Ctrl.CurrentAgcCtrl] / 64 * GC2053Ctrl.CurrentDgcCtrl / 64;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      Chan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pExposureTime:    pointer to current exposure time
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, float *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL || pExposureTime == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        *pExposureTime = GC2053Ctrl.Status.ModeInfo.RowTime * (float)(GC2053Ctrl.CurrentShutterCtrl);
    }
    return RetVal;
}
#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_ConvertGainFactor
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
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, float DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl, UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    float AgcGain = 1.0;
    float DgcGain = 1.0;
	UINT32 MaxAgcGain = Gc2053GainLevelTable[GC2053_NUM_AGC_STEP - 1]*16;
    UINT32 MinAgcGain = Gc2053GainLevelTable[0];
	UINT32 GainFactor;
	UINT32 AgcTableIdx;
	UINT16 DgcGainReg = 0;

	GainFactor = DesiredFactor * 64;

	if (GainFactor < MinAgcGain)
        GainFactor = MinAgcGain;
    else if (GainFactor > MaxAgcGain)
        GainFactor = MaxAgcGain;

    for (AgcTableIdx = 0; AgcTableIdx < GC2053_NUM_AGC_STEP; AgcTableIdx ++) {
		if((Gc2053GainLevelTable[AgcTableIdx] <= GainFactor)&&(GainFactor < Gc2053GainLevelTable[AgcTableIdx+1]))
            break;
    }

    AgcGain = (float)(Gc2053GainLevelTable[AgcTableIdx]);
    DgcGain = GainFactor / AgcGain;
    *pAnalogGainCtrl  = AgcTableIdx;

	DgcGainReg = (UINT16 )(DgcGain * 64);
    *pDigitalGainCtrl = DgcGainReg;

    DgcGain = (float)(DgcGainReg / 64.0);

    *pActualFactor = (UINT32)(AgcGain / 64 * DgcGain * 1024 * 1024);


    //AmbaPrint("DesiredFactor = %f, DgcR305E = 0x%08x, *pDigitalGainCtrl = 0x%08x, AgcGain = %f, ActualFactor = %u",
               //DesiredFactor, *pAnalogGainCtrl, *pDigitalGainCtrl, AgcGain, *pActualFactor);

    return RetVal;
}
#else
static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
	float AgcGain = 1.0;
    float DgcGain = 1.0;
	UINT32 MaxAgcGain = Gc2053GainLevelTable[GC2053_NUM_AGC_STEP - 1]*16;
    UINT32 MinAgcGain = Gc2053GainLevelTable[0];
	UINT32 GainFactor;
	UINT32 AgcTableIdx;
	UINT16 DgcGainReg = 0;

	GainFactor = DesiredFactor * 64;

	if (GainFactor < MinAgcGain)
        GainFactor = MinAgcGain;
    else if (GainFactor > MaxAgcGain)
        GainFactor = MaxAgcGain;

    for (AgcTableIdx = 0; AgcTableIdx < GC2053_NUM_AGC_STEP; AgcTableIdx ++) {
		if((Gc2053GainLevelTable[AgcTableIdx] <= GainFactor)&&(GainFactor < Gc2053GainLevelTable[AgcTableIdx+1]))
            break;
    }

    AgcGain = (float)(Gc2053GainLevelTable[AgcTableIdx]);
    DgcGain = GainFactor / AgcGain;
    *pAnalogGainCtrl  = AgcTableIdx;

	DgcGainReg = (UINT16 )(DgcGain * 64);
    *pDigitalGainCtrl = DgcGainReg;

    DgcGain = (float)(DgcGainReg / 64.0);

    *pActualFactor = (UINT32)(AgcGain / 64 * DgcGain);







#if 0
    if (DesiredFactor < 1.0) { /* 1.0 = IMX415DeviceInfo.MinAnalogGainFactor * IMX415DeviceInfo.MinDigitalGainFactor */
        *pAnalogGainCtrl = 0U;
        *pDigitalGainCtrl = 0x100U;

        *pActualFactor = 1.0f;
    } else {
        FLOAT AnalogGainFactor;
        FLOAT DigitalGainFactor;
        DOUBLE DigitalGainCtrlInDb;
        DOUBLE AnalogGainCtrlInDb;

        DigitalGainFactor = DesiredFactor / GC2053DeviceInfo.MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0) {
            *pDigitalGainCtrl = 0x100U;
            DigitalGainFactor = 1.0f;

            AnalogGainFactor = DesiredFactor;
        } else {
            if (DigitalGainFactor > GC2053DeviceInfo.MaxDigitalGainFactor) {
                DigitalGainFactor = GC2053DeviceInfo.MaxDigitalGainFactor;
            }

            (void)AmbaWrap_floor((DigitalGainFactor * 256.0), &DigitalGainCtrlInDb);
            *pDigitalGainCtrl = (UINT32) DigitalGainCtrlInDb;

            DigitalGainFactor = (FLOAT)*pDigitalGainCtrl / 256.0f;

            AnalogGainFactor = DesiredFactor / DigitalGainFactor;
        }

        (void)AmbaWrap_floor(((1024.0) - (1024.0 / AnalogGainFactor)), &AnalogGainCtrlInDb);
        *pAnalogGainCtrl = (UINT32)AnalogGainCtrlInDb;

        if (*pAnalogGainCtrl > 978U) {
            *pAnalogGainCtrl = 978U; /* Max. 26.951 dB */
        }

        AnalogGainFactor = 1024.0f / (1024.0f - (FLOAT)*pAnalogGainCtrl);

        *pActualFactor = AnalogGainFactor * DigitalGainFactor;
    }
#endif
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_ConvertGainFactor
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
static UINT32 GC2053_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &GC2053Ctrl.Status.ModeInfo;
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
#endif
#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_ConvertShutterSpeed
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
static UINT32 GC2053_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, float ExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &GC2053Ctrl.ModeInfo;
    UINT32 ExposureFrames, ExposureTimeMaxMargin;
    UINT32 MaxFrameLengthLines  = 0x3fff;
    UINT32 FrameLengthLines;

    if (pShutterCtrl == NULL)
        return SENSOR_ERR_ARG;;

    *pShutterCtrl = (UINT32)(ExposureTime/ pModeInfo->RowTime * pModeInfo->NumExposureStepPerFrame / pModeInfo->FrameLengthLines );

    ExposureFrames = (*pShutterCtrl / pModeInfo->NumExposureStepPerFrame);
    ExposureFrames = (*pShutterCtrl % pModeInfo->NumExposureStepPerFrame) ? ExposureFrames + 1 : ExposureFrames;
    FrameLengthLines = ExposureFrames * GC2053ModeInfoList[pModeInfo->Config.ModeID].FrameTiming.FrameLengthLines;

    if(FrameLengthLines > MaxFrameLengthLines)
        ExposureFrames = MaxFrameLengthLines / GC2053ModeInfoList[pModeInfo->Config.ModeID].FrameTiming.FrameLengthLines;

    ExposureTimeMaxMargin = ExposureFrames * pModeInfo->NumExposureStepPerFrame;

    /* Exposure lines needs to be smaller than VTS */
    if (*pShutterCtrl > ExposureTimeMaxMargin)
        *pShutterCtrl = ExposureTimeMaxMargin;

    /* Exposure lines needs to be bigger 1 */
    if (*pShutterCtrl < 1)
        *pShutterCtrl = 1;

    //AmbaPrint("ExposureTime:%f, Actual:%f, ShutterCtrl:%d", ExposureTime, *pShutterCtrl*pModeInfo->RowTime*pModeInfo->FrameLengthLines/pModeInfo->NumExposureStepPerFrame, *pShutterCtrl);

    return RetVal;
}
#else
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 MaxExposureLine, MinExposureLine;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ShutterCtrl, ExposureFrames;
    DOUBLE ShutterCtrlInDb;

    (void)AmbaWrap_floor((DOUBLE)(ExposureTime / ((DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame)), &ShutterCtrlInDb);
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
         /* Maximun value of FRM_LENGTH_LINES is 65535d */
        ExposureFrames = ((ShutterCtrl - 1U) / NumExposureStepPerFrame) + 1U;
        if ((FrameLengthLines * ExposureFrames) > 0xffffU) {
            ShutterCtrl = (0xffffU / FrameLengthLines) * NumExposureStepPerFrame;
        }

        /* Maximun value of COARSE_INTEG_TIME is (FRM_LENGTH_LINES - 22) */
        MaxExposureLine=  NumExposureStepPerFrame;
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
#endif


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
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &GC2053Ctrl.Status.ModeInfo;
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
 *  @RoutineName:: GC2053_SetAnalogGainCtrl
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
static UINT32 GC2053_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *AnalogGainCtrl)
{
    if (pChan == NULL)
        return SENSOR_ERR_ARG;

    GC2053_SetAnalogGainReg(*AnalogGainCtrl);

    /* Update current AGC control */
    GC2053Ctrl.CurrentAgcCtrl = *AnalogGainCtrl;

    //SENSOR_DBG_GAIN("[Vin%d/Sensor%d/Hdr%d][GC2053][GAIN] AnalogGain[%d] = %lf X",
    //                      Chan.Bits.VinID, Chan.Bits.SensorID, Chan.Bits.HdrID, GC2053Ctrl.CurrentAgcCtrl, Gc2053GainLevelTable[AnalogGainCtrl]);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      Chan:               Vin ID and sensor ID
 *      DigitalGainCtrl:     Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *DigitalGainCtrl)
{
    if (pChan == NULL)
        return SENSOR_ERR_ARG;

    GC2053_SetDigitalGainReg(*DigitalGainCtrl);

    // Update current DGC control
    GC2053Ctrl.CurrentDgcCtrl = *DigitalGainCtrl;

    //SENSOR_DBG_GAIN("[Vin%d/Sensor%d/Hdr%d][GC2053][GAIN] DigitalGainCtrl = 0x%x",
    //                      Chan.Bits.VinID, Chan.Bits.SensorID, Chan.Bits.HdrID, GC2053Ctrl.CurrentDgcCtrl);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_SetShutterCtrl
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
static UINT32 GC2053_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *ShutterCtrl)
{
    if (pChan == NULL)
        return SENSOR_ERR_ARG;

    GC2053_SetShutterReg(*ShutterCtrl);

    /* Update current shutter control */
    GC2053Ctrl.CurrentShutterCtrl = *ShutterCtrl;

    //SENSOR_DBG_SHR("[Vin%d/Sensor%d/Hdr%d][GC2053][SHR] ShutterCtrl = 0x%x",
    //               Chan.Bits.VinID, Chan.Bits.SensorID, Chan.Bits.HdrID, ShutterCtrl);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_SetSlowShutterCtrl
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
static UINT32 GC2053_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    if (pChan == NULL)
            return SENSOR_ERR_ARG;

    if (GC2053Ctrl.Status.ModeInfo.HdrInfo.HdrType != 0)
        return SENSOR_ERR_ARG;

    GC2053_SetSlowShutterReg(SlowShutterCtrl);

    //SENSOR_DBG_SHR("[Vin%d/Sensor%d/Hdr%d][GC2053][SHR] SlowShutterCtrl = 0x%x",
    //                  Chan.Bits.VinID, Chan.Bits.SensorID, Chan.Bits.HdrID, SlowShutterCtrl);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC2053_ChangeReadoutMode(UINT16 Mode)
{
    int i;
    UINT8 Addr, Data;

    for (i = 0; i < GC2053_NUM_READOUT_MODE_REG; i++) {
        Addr = GC2053RegTable[i].Addr;
        Data = GC2053RegTable[i].Data[Mode];
        GC2053_RegWrite(Addr, &Data, 1);
    }
    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC2053_Config
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
static UINT32 GC2053_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)//AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
	AMBA_VIN_MIPI_PAD_CONFIG_s GC2053PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    UINT8 LaneShift[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0U,
        [AMBA_VIN_CHANNEL1] = 4U,
        [AMBA_VIN_CHANNEL2] = 6U,
    };
	UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    UINT16 SensorMode = pMode->ModeID;//Mode.Bits.Mode;
    GC2053_READOUT_MODE_e ReadoutMode = GC2053ModeInfoList[SensorMode].ReadoutMode;
    //UINT8 Data = 0;

	AmbaPrint_ModuleSetAllowList(SENSOR_MODULE_ID, 1);

    if (SensorMode >= AMBA_SENSOR_GC2053_NUM_MODE){
        RetVal = SENSOR_ERR_ARG;
	} else {
		AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[GC2053]yyue Sensor Mode %d ReadOut Mode %d", SensorMode, ReadoutMode, 0U, 0U, 0U);

        //GC2053Ctrl.Status.ElecShutterMode = ElecShutterMode;
    	//AmbaPrint("============ GC2053: %d ============", SensorMode);

        pModeInfo  = &GC2053Ctrl.Status.ModeInfo;
        (void)GC2053_PrepareModeInfo(pMode, pModeInfo);
        (void)AmbaWrap_memcpy(&GC2053Ctrl.ModeInfo, pModeInfo, sizeof(AMBA_SENSOR_MODE_INFO_s));

        //GC2053VinConfig.NumActiveLanes = pModeInfo->OutputInfo.NumDataLanes;

        /* Adjust MIPI-Phy parameters */
        //AmbaPrint("DataRate=%d", pModeInfo->OutputInfo.DataRate);
        //AmbaVIN_CalculateMphyConfig(pModeInfo->OutputInfo.DataRate, &GC2053VinConfig.MipiCtrl.VinMipiTiming);

        /* Make VIN not to capture bad frames during readout mode transition */
        //AmbaVIN_Reset((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, AMBA_VIN_MIPI, &GC2053VinConfig.MipiCtrl);
        GC2053PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        GC2053PadConfig.EnabledPin = (((UINT32)1U << pModeInfo->OutputInfo.NumDataLanes) - 1U) << LaneShift[pChan->VinID];
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &GC2053PadConfig);
        AmbaKAL_TaskSleep(3);

        GC2053_HardwareReset(pChan->VinID);

        //AmbaPLL_SetSensor1Clk(pModeInfo->FrameTime.InputClk);
        AmbaKAL_TaskSleep(3);
		RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
    	AmbaKAL_TaskSleep(5);
        GC2053_ChangeReadoutMode(ReadoutMode);

    	GC2053_SetStandbyOff();

        AmbaKAL_TaskSleep(10); /* wait for initial regulator stabilization period */

        {
            UINT8 data[2] = {0};
            GC2053_RegRead(0xF0, data, 2);
            AmbaPrint_PrintInt5("GC2053 Chip ID: 0x%.2X%.2X", data[0], data[1], 0, 0, 0);AmbaPrint_Flush();
        }

    	//if (AmbaVIN_ResetMipiRx((AMBA_VIN_CHANNEL_e)Chan.Bits.VinID, 1000) != OK) {
            //AmbaPrint("Can't lock MIPI signal to reset MIPI Rx");
        //}

    	//AmbaKAL_TaskSleep(5);

        GC2053_ConfigVin(pChan->VinID, pModeInfo);

        //GC2053Ctrl.Status.DevStat.Bits.Sensor0Standby = 0;
    }
    return OK;
}

static UINT32 GC2053_SetRotation(const AMBA_SENSOR_CHANNEL_s *pChan, UINT8 Rotaion)
{
    UINT16 Value = 0;

    GC2053_RegisterWrite(pChan, 0xfe, 0);
    GC2053_RegisterRead(pChan, 0x17, &Value);
    if (Rotaion == 0) {
        Value = (Value & 0xFC) | 0x00;
    } else if (Rotaion == 1) {
        Value = (Value & 0xFC) | 0x01;
    } else if (Rotaion == 2) {
        Value = (Value & 0xFC) | 0x02;
    } else if (Rotaion == 3) {
        Value = (Value & 0xFC) | 0x03;
    }
    GC2053_RegisterWrite(pChan, 0x17, Value);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_GC2053Obj = {
    .Init                   = GC2053_Init,
    .Enable                 = GC2053_Enable,
    .Disable                = GC2053_Disable,
    .Config                 = GC2053_Config,
    .GetStatus              = GC2053_GetStatus,
    .GetModeInfo            = GC2053_GetModeInfo,
    .GetDeviceInfo          = GC2053_GetDeviceInfo,
    .GetCurrentGainFactor   = GC2053_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = GC2053_GetCurrentShutterSpeed,

    .ConvertGainFactor      = GC2053_ConvertGainFactor,
    .ConvertShutterSpeed    = GC2053_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = GC2053_SetAnalogGainCtrl,
	.SetDigitalGainCtrl     = GC2053_SetDigitalGainCtrl,
    .SetShutterCtrl         = GC2053_SetShutterCtrl,
    .SetSlowShutterCtrl     = GC2053_SetSlowShutterCtrl,

    .RegisterRead           = GC2053_RegisterRead,
    .RegisterWrite          = GC2053_RegisterWrite,

    .SetRotation            = GC2053_SetRotation,
};
