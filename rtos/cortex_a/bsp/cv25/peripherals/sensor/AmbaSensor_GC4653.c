/**
 *  @file AmbaSensor_GC4653.c
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
 *  @details Control APIs of OmniVision GC4653 CMOS sensor with MIPI interface
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
#include "AmbaSensor_GC4653.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "bsp.h"

#define GC4653_SENSOR_I2C_SLAVE_ADDR 0x52
#define GC4653_SENSOR_I2C_MAX_SIZE 64U

/*-----------------------------------------------------------------------------------------------*\
 * GC4653 sensor global info
\*-----------------------------------------------------------------------------------------------*/
//static UINT8 SensorSlaveID = 0x6e;

/*-----------------------------------------------------------------------------------------------*\
 * GC4653 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
GC4653_CTRL_s GC4653Ctrl = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_PrepareModeInfo
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
static void GC4653_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                 = pMode->ModeID;
    GC4653_READOUT_MODE_e           ReadoutMode   = GC4653ModeInfoList[ModeID].ReadoutMode;
    const GC4653_FRAME_TIMING_s     *pFrameTiming = &GC4653ModeInfoList[ModeID].FrameTiming;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo  = &GC4653OutputInfo[ReadoutMode];
    const AMBA_SENSOR_INPUT_INFO_s  *pInputInfo = &GC4653InputInfo[ReadoutMode];
    
    //pModeInfo->Mode.Data= Mode;
    pModeInfo->LineLengthPck = (UINT32)((UINT64)pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits *
                                                GC4653ModeInfoList[ModeID].FrameTiming.FrameRate.NumUnitsInTick /
                                                GC4653ModeInfoList[ModeID].FrameTiming.FrameRate.TimeScale /
                                                GC4653ModeInfoList[ModeID].FrameTiming.FrameLengthLines);
    pModeInfo->FrameLengthLines = GC4653ModeInfoList[ModeID].FrameTiming.FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame = GC4653ModeInfoList[ModeID].FrameTiming.FrameLengthLines;
    //pModeInfo->PixelRate = (float)pOutputInfo->DataRate * pOutputInfo->NumDataLanes / pOutputInfo->NumDataBits;
    pModeInfo->InputClk                 = pFrameTiming->InputClk;
    pModeInfo->RowTime = (pModeInfo->LineLengthPck * pOutputInfo->NumDataBits * 1.0) / (pOutputInfo->NumDataLanes * pOutputInfo->DataRate );
    //pModeInfo->FrameTime.InputClk = GC4653ModeInfoList[ModeID].FrameTiming.InputClk;
    (void)AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
    (void)AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    (void)AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
    
    /* HDR information */
    (void)AmbaWrap_memset(&pModeInfo->HdrInfo, 0, sizeof(AMBA_SENSOR_HDR_INFO_s)); // Not support HDR
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_ConfigVin
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
static UINT32 GC4653_ConfigVin(const UINT32 VinID, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s GC4653VinConfig = {
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
        .NumActiveLanes         = 2,
        .DataType               = 0x2B,
        .DataTypeMask           = 0x3f,
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0}
    };
    
    const AMBA_SENSOR_OUTPUT_INFO_s   *pOutputInfo  = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &GC4653VinConfig;
    
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
 *  @RoutineName:: GC4653_HardwareReset
 *
 *  @Description:: Reset GC4653 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 GC4653_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);

    if (RetVal == GPIO_ERR_NONE) {
        (void)AmbaKAL_TaskSleep(2);
        RetVal = AmbaUserGPIO_SensorResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
    }

    return RetVal;
}

static UINT32 GC4653_RegRead(UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    //UINT16 pTxData[4];
    //UINT8  RxData[2];
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};    
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};    
    UINT8 TxData[2];    
    UINT32 TxSize;    
    
    I2cTxConfig.SlaveAddr = GC4653_SENSOR_I2C_SLAVE_ADDR;    
    I2cTxConfig.DataSize  = 2U;    
    I2cTxConfig.pDataBuf  = TxData;    
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);   
   
    
    I2cRxConfig.SlaveAddr = ((UINT32)GC4653_SENSOR_I2C_SLAVE_ADDR | (UINT32)1U);    
    I2cRxConfig.DataSize  = Size;    
    I2cRxConfig.pDataBuf  = pRxData;


#if 0  /* to be refined with bsp file */    
    RetVal = AmbaI2C_MasterReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,                                 
                                        &I2cRxConfig, &TxSize, 1000U);
#endif    
    RetVal = AmbaI2C_MasterReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,                                 
                                        &I2cRxConfig, &TxSize, 1000U);    

    if (RetVal != I2C_ERR_NONE) {        
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[GC4653] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);    
    }
    
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_RegisterRead
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
static UINT32 GC4653_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{   
    UINT32 RetVal = SENSOR_ERR_NONE; 
    UINT8 RData;  

    
    if (pChan == NULL) {  
        RetVal = SENSOR_ERR_ARG;   
    } else {   
        RetVal = GC4653_RegRead(Addr, &RData, 1U);   
        *pData = RData;  
    } 
    return RetVal;
}


static UINT32 GC4653_RegWrite(UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal = I2C_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[GC4653_SENSOR_I2C_MAX_SIZE + 1U];
    UINT32 TxSize, i;

    if (Size > GC4653_SENSOR_I2C_MAX_SIZE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = GC4653_SENSOR_I2C_SLAVE_ADDR;
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;
        
        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                            &I2cConfig, &TxSize, 1000U);
#endif
        RetVal = AmbaI2C_MasterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                            &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "[GC4653] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }
    
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[GC4653][IO] Addr = 0x%04x, Data = 0x%02x", Addr, pTxData[0], 0, 0, 0);
    //SENSOR_DBG_IO("[GC4653][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_RegisterWrite
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
static UINT32 GC4653_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 WData = (UINT8)(Data & 0xff);


    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = GC4653_RegWrite(Addr, &WData, 1U);
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_SetAnalogGainReg
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
static void GC4653_SetAnalogGainReg(UINT32 AnalogGainCtrl)
{
    GC4653_RegWrite(0x02b3,&Gc4653RegValTable[AnalogGainCtrl][0],1);
    GC4653_RegWrite(0x02b4,&Gc4653RegValTable[AnalogGainCtrl][1],1);
    GC4653_RegWrite(0x02b8,&Gc4653RegValTable[AnalogGainCtrl][2],1);
    GC4653_RegWrite(0x02b9,&Gc4653RegValTable[AnalogGainCtrl][3],1);
    GC4653_RegWrite(0x0515,&Gc4653RegValTable[AnalogGainCtrl][4],1);
    GC4653_RegWrite(0x0519,&Gc4653RegValTable[AnalogGainCtrl][5],1);
    GC4653_RegWrite(0x02d9,&Gc4653RegValTable[AnalogGainCtrl][6],1);


}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_SetDigitalGainReg
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
static void GC4653_SetDigitalGainReg(UINT32 DigitalGainCtrl)
{

    UINT8 DigitalGainVal[2];

    DigitalGainVal[0] = (DigitalGainCtrl>>6);
    DigitalGainVal[1] = ((DigitalGainCtrl&0x3f)<<2);
    
        
    GC4653_RegWrite(0x020e,&DigitalGainVal[0],1);
    GC4653_RegWrite(0x020f,&DigitalGainVal[1],1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_SetShutterReg
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
static void GC4653_SetShutterReg(UINT32 ShutterCtrl)
{

    UINT8 ShutterCtrlVal[2];

    ShutterCtrlVal[0] = (ShutterCtrl >> 8) & 0x3f;
    ShutterCtrlVal[1] = ShutterCtrl & 0xff;

    GC4653_RegWrite(0x0202, &ShutterCtrlVal[0],1);
    GC4653_RegWrite(0x0203, &ShutterCtrlVal[1],1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_SetSlowShutterReg
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
static void GC4653_SetSlowShutterReg(UINT32 SlowShutterCtrl)
{

    UINT32 TargetFrameLengthLines;
    UINT8 TargetFrameLengthLinesVal_H;
    UINT8 TargetFrameLengthLinesVal_L;
    
    if (SlowShutterCtrl < 1)
        SlowShutterCtrl = 1;

    TargetFrameLengthLines = GC4653ModeInfoList[GC4653Ctrl.Status.ModeInfo.Config.ModeID].FrameTiming.FrameLengthLines * SlowShutterCtrl;
    
    TargetFrameLengthLinesVal_H = (UINT8)((TargetFrameLengthLines >> 8) & 0xff);
    TargetFrameLengthLinesVal_L = (UINT8)(TargetFrameLengthLines & 0xff);
    

    GC4653_RegWrite(0x0340, &TargetFrameLengthLinesVal_H, 1);
    GC4653_RegWrite(0x0341, &TargetFrameLengthLinesVal_L, 1);
    
    /* Update frame rate information */
    GC4653Ctrl.Status.ModeInfo.FrameLengthLines = TargetFrameLengthLines;
    GC4653Ctrl.Status.ModeInfo.NumExposureStepPerFrame = TargetFrameLengthLines;
    GC4653Ctrl.Status.ModeInfo.FrameRate.TimeScale = GC4653Ctrl.ModeInfo.FrameRate.TimeScale / SlowShutterCtrl;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void GC4653_SetStandbyOn(void)
{
    UINT8 RegVal = 0x08;

  
    GC4653_RegWrite(0x0100, &RegVal, 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void GC4653_SetStandbyOff(void)
{
    UINT8 RegVal = 0x09;
    GC4653_RegWrite(0x0100, &RegVal, 1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_Init
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
static UINT32 GC4653_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    const GC4653_FRAME_TIMING_s *pFrameTime = &GC4653ModeInfoList[0].FrameTiming;
    
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "================[GC4653][IO] INIT Vin(%d)================", pChan->VinID, 0, 0, 0, 0);
    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_Enable
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
static UINT32 GC4653_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        //GC4653_HardwareReset();

    }
    /* config mipi phy*/
    //RetVal = AmbaDSP_VinPhySetMIPI(pChan->VinID);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_Disable
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
static UINT32 GC4653_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {        
        RetVal = SENSOR_ERR_ARG;    
    } 


    GC4653_SetStandbyOn();



    //GC4653Ctrl.Status.DevStat.Bits.Sensor0Power = 0;

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_GetStatus
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
static UINT32 GC4653_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    
    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {        
        (void)AmbaWrap_memcpy(pStatus, &GC4653Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));  
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_GetModeInfo
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
static UINT32 GC4653_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ModeID = pMode->ModeID;
    

    
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (ModeID == AMBA_SENSOR_CURRENT_MODE) {
            ModeID = GC4653Ctrl.Status.ModeInfo.Config.ModeID;
        }

        if ((ModeID >= AMBA_SENSOR_GC4653_NUM_MODE) ||
            (pModeInfo == NULL)) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            GC4653_PrepareModeInfo(pMode, pModeInfo);
        }
    }    
    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_GetDeviceInfo
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
static UINT32 GC4653_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    
    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        (void)AmbaWrap_memcpy(pDeviceInfo, &GC4653DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_GetCurrentGainFactor
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
static UINT32 GC4653_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, float *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    
    if ((pChan == NULL) || (pGainFactor == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {    
        *pGainFactor = (float)Gc4653GainLevelTable[GC4653Ctrl.CurrentAgcCtrl] / 64 * GC4653Ctrl.CurrentDgcCtrl / 64;
    }
    
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_GetCurrentShutterSpeed
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
static UINT32 GC4653_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, float *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    
    if (pChan == NULL || pExposureTime == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        *pExposureTime = GC4653Ctrl.Status.ModeInfo.RowTime * (float)(GC4653Ctrl.CurrentShutterCtrl);
    }
    return RetVal;
}

static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{    
    float AgcGain = 1.0; 
    float DgcGain = 1.0; 
    UINT32 MaxAgcGain = Gc4653GainLevelTable[GC4653_NUM_AGC_STEP - 1]*16;
    UINT32 MinAgcGain = Gc4653GainLevelTable[0];
    UINT32 GainFactor;
    UINT32 AgcTableIdx;
    UINT16 DgcGainReg = 0;
    
    GainFactor = DesiredFactor * 64;
    
    if (GainFactor < MinAgcGain)
        GainFactor = MinAgcGain;
    else if (GainFactor > MaxAgcGain)
        GainFactor = MaxAgcGain;

    for (AgcTableIdx = 0; AgcTableIdx < GC4653_NUM_AGC_STEP; AgcTableIdx ++) {
        if((Gc4653GainLevelTable[AgcTableIdx] <= GainFactor)&&(GainFactor < Gc4653GainLevelTable[AgcTableIdx+1]))
            break;
    }

    AgcGain = (float)(Gc4653GainLevelTable[AgcTableIdx]);
    DgcGain = GainFactor / AgcGain;
    *pAnalogGainCtrl  = AgcTableIdx;
    
    DgcGainReg = (UINT16 )(DgcGain * 64);
    *pDigitalGainCtrl = DgcGainReg;
    
    DgcGain = (float)(DgcGainReg / 64.0);
    
    *pActualFactor = (FLOAT)(AgcGain / 64 * DgcGain);

}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_ConvertGainFactor
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
static UINT32 GC4653_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &GC4653Ctrl.Status.ModeInfo;
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
        MaxExposureLine=  NumExposureStepPerFrame - 8;
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

    //AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID,"ExposureTime:%f, Actual:%f, ShutterCtrl:%d, rowtime:%f", ExposureTime, *pActualExptime, *pShutterCtrl, pModeInfo->RowTime,NULL);
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
 *          INT32 : SENSOR_ERR_NONE(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 GC4653_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &GC4653Ctrl.Status.ModeInfo;
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
 *  @RoutineName:: GC4653_SetAnalogGainCtrl
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
static UINT32 GC4653_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *AnalogGainCtrl)
{
    if (pChan == NULL)  
        return SENSOR_ERR_ARG;   
    
    GC4653_SetAnalogGainReg(*AnalogGainCtrl);

    /* Update current AGC control */
    GC4653Ctrl.CurrentAgcCtrl = *AnalogGainCtrl;
    
    //SENSOR_DBG_GAIN("[Vin%d/Sensor%d/Hdr%d][GC4653][GAIN] AnalogGain[%d] = %lf X",
    //                      Chan.Bits.VinID, Chan.Bits.SensorID, Chan.Bits.HdrID, GC4653Ctrl.CurrentAgcCtrl, Gc4653GainLevelTable[AnalogGainCtrl]);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_SetDigitalGainCtrl
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
static UINT32 GC4653_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *DigitalGainCtrl)
{
    if (pChan == NULL)  
        return SENSOR_ERR_ARG;
    
    GC4653_SetDigitalGainReg(*DigitalGainCtrl);

    // Update current DGC control 
    GC4653Ctrl.CurrentDgcCtrl = *DigitalGainCtrl;
    
    //SENSOR_DBG_GAIN("[Vin%d/Sensor%d/Hdr%d][GC4653][GAIN] DigitalGainCtrl = 0x%x",
    //                      Chan.Bits.VinID, Chan.Bits.SensorID, Chan.Bits.HdrID, GC4653Ctrl.CurrentDgcCtrl);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_SetShutterCtrl
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
static UINT32 GC4653_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *ShutterCtrl)
{
    if (pChan == NULL)  
        return SENSOR_ERR_ARG;
    
    GC4653_SetShutterReg(*ShutterCtrl);

    /* Update current shutter control */
    GC4653Ctrl.CurrentShutterCtrl = *ShutterCtrl;

    //SENSOR_DBG_SHR("[Vin%d/Sensor%d/Hdr%d][GC4653][SHR] ShutterCtrl = 0x%x",
    //               Chan.Bits.VinID, Chan.Bits.SensorID, Chan.Bits.HdrID, ShutterCtrl);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_SetSlowShutterCtrl
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
static UINT32 GC4653_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    if (pChan == NULL)  
            return SENSOR_ERR_ARG;

    if (GC4653Ctrl.Status.ModeInfo.HdrInfo.HdrType != 0)
        return SENSOR_ERR_ARG;

    GC4653_SetSlowShutterReg(SlowShutterCtrl);

    //SENSOR_DBG_SHR("[Vin%d/Sensor%d/Hdr%d][GC4653][SHR] SlowShutterCtrl = 0x%x",
    //                  Chan.Bits.VinID, Chan.Bits.SensorID, Chan.Bits.HdrID, SlowShutterCtrl);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_ChangeReadoutMode
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
static UINT32 GC4653_ChangeReadoutMode(UINT16 Mode)
{
    int i;
    UINT16 Addr;
	UINT8 Data;

    for (i = 0; i < GC4653_NUM_READOUT_MODE_REG; i++) {
        Addr = GC4653RegTable[i].Addr;
        Data = GC4653RegTable[i].Data[Mode];
        GC4653_RegWrite(Addr, &Data, 1);
    }
    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GC4653_Config
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
static UINT32 GC4653_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)//AMBA_SENSOR_MODE_ID_u Mode, AMBA_SENSOR_ESHUTTER_TYPE_e ElecShutterMode)
{
    AMBA_VIN_MIPI_PAD_CONFIG_s GC4653PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = NULL;
    UINT16 SensorMode = pMode->ModeID;//Mode.Bits.Mode;
    GC4653_READOUT_MODE_e ReadoutMode = GC4653ModeInfoList[SensorMode].ReadoutMode;


    AmbaPrint_ModuleSetAllowList(SENSOR_MODULE_ID, 1);

    if (SensorMode >= AMBA_SENSOR_GC4653_NUM_MODE){
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[GC4653]yyue Sensor Mode %d ReadOut Mode %d", SensorMode, ReadoutMode, 0U, 0U, 0U);



        pModeInfo  = &GC4653Ctrl.Status.ModeInfo;
        (void)GC4653_PrepareModeInfo(pMode, pModeInfo);
        (void)AmbaWrap_memcpy(&GC4653Ctrl.ModeInfo, pModeInfo, sizeof(AMBA_SENSOR_MODE_INFO_s));

		GC4653PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &GC4653PadConfig);
        AmbaKAL_TaskSleep(3);

        GC4653_HardwareReset(pChan->VinID);

        //AmbaPLL_SetSensor1Clk(pModeInfo->FrameTime.InputClk);
        AmbaKAL_TaskSleep(3);
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
        AmbaKAL_TaskSleep(5);
        GC4653_ChangeReadoutMode(ReadoutMode);
        
        GC4653_SetStandbyOff();

        AmbaKAL_TaskSleep(10); /* wait for initial regulator stabilization period */

        {
            UINT8 data[2] = {0};
            GC4653_RegRead(0x03F0, data, 2);
            AmbaPrint_PrintInt5("GC4653 Chip ID: 0x%.2X%.2X", data[0], data[1], 0, 0, 0);AmbaPrint_Flush();
        }
        //AmbaKAL_TaskSleep(5);

        GC4653_ConfigVin(pChan->VinID, pModeInfo);


    }
    return OK;
}

static UINT32 GC4653_SetRotation(const AMBA_SENSOR_CHANNEL_s *pChan, UINT8 Rotaion)
{
    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_GC4653Obj = {
    .Init                   = GC4653_Init,
    .Enable                 = GC4653_Enable,
    .Disable                = GC4653_Disable,
    .Config                 = GC4653_Config,
    .GetStatus              = GC4653_GetStatus,
    .GetModeInfo            = GC4653_GetModeInfo,
    .GetDeviceInfo          = GC4653_GetDeviceInfo,
    .GetCurrentGainFactor   = GC4653_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = GC4653_GetCurrentShutterSpeed,

    .ConvertGainFactor      = GC4653_ConvertGainFactor,
    .ConvertShutterSpeed    = GC4653_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = GC4653_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = GC4653_SetDigitalGainCtrl,
    .SetShutterCtrl         = GC4653_SetShutterCtrl,
    .SetSlowShutterCtrl     = GC4653_SetSlowShutterCtrl,

    .RegisterRead           = GC4653_RegisterRead,
    .RegisterWrite          = GC4653_RegisterWrite,
    .SetRotation            = GC4653_SetRotation,
};
