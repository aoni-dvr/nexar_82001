/**
 *  @file AmbaSensor_MAX9295_9296_VG1762_RGBIR.c
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
 *  @details Control APIs of MAX9295_9296 plus ST VG1762 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaMisraFix.h"

#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_VG1762_RGBIR.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSbrg_Max9295_9296.h"

#include "bsp.h"
#define MX00_VG1762_SERDES_ENABLE
//#define DEBUG_MSG //enable if when need to print all sensor setting
//#define MX00_VG1762_IN_SLAVE_MODE
//#define MX00_VG1762_I2C_WR_BUF_SIZE 64

#define TESTCONTX //test for multiple context
//#define IRLED_RGBIR

#if 0
static INT32 MX00_VG1762_ConfigPost(UINT32 *pVinID);
#endif

static UINT32 MX00_VG1762I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
};

#ifdef MX00_VG1762_IN_SLAVE_MODE
static UINT32 MX00_VG1762MasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
};
#endif

/*-----------------------------------------------------------------------------------------------*\
 * MX00_VG1762 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX00_VG1762_CTRL_s MX00_VG1762Ctrl[AMBA_NUM_VIN_CHANNEL] = {0};
#ifdef MX00_VG1762_SERDES_ENABLE
static MAX9295_9296_SERDES_CONFIG_s MX00_VG1762SerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX00_VG1762_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_VG1762_I2C_SLAVE_ADDRESS + 0x10U),
            [1] = (UINT8)(MX00_VG1762_I2C_SLAVE_ADDRESS + 0x12U),
        },
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},

        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [1] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX00_VG1762_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_VG1762_I2C_SLAVE_ADDRESS + 0x14U),
            [1] = (UINT8)(MX00_VG1762_I2C_SLAVE_ADDRESS + 0x16U),
        },
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},

        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },

};

static UINT16 MX00_VG1762_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}
static UINT32 RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size);
//////
static UINT32 MX00_VG1762_Setup_ContxReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT8 CurContxID, UINT8 NextContxID)
{
    /*
    #define MX00_VG1762_CONTX_RPTCNT_OFFSET         0x004AU
    #define MX00_VG1762_NEXT_CONTX_OFFSET           0x004CU
    #define MX00_VG1762_HDR_CTRL_OFFSET             0x0032U
    *//*
#define MX00_VG1762_CONTX_RPTCNT_OFFSET         0x004AU
#define MX00_VG1762_NEXT_CONTX_OFFSET           0x004CU
#define MX00_VG1762_HDR_CTRL_OFFSET             0x0032U
*/
    UINT32 RetVal = 0U;
    UINT32 RsvdMode = pChan->Reserved[0]; //0: Normal, 1: HDR subtraction
    UINT16 ContxRegBaseCur, ContxRegBaseNext, RegAddr;
    UINT8  WData;
//UINT32 CurContxID, UINT32 NextContxID)
//
//AmbaPrint_PrintUInt5(" MX00_VG1762_Setup_ContxReg, CurContxID : %d, NextContxID : %d, RsvdMode : %d", (UINT32)CurContxID, (UINT32)NextContxID, RsvdMode, 0U, 0U);//
///
    if(CurContxID == 0U) {
        ContxRegBaseCur  = MX00_VG1762_CONTX0_BASE;
    } else if(CurContxID == 1U) {
        ContxRegBaseCur  = MX00_VG1762_CONTX1_BASE;
    } else if(CurContxID == 2U) {
        ContxRegBaseCur  = MX00_VG1762_CONTX2_BASE;
    } else if(CurContxID == 3U) {
        ContxRegBaseCur  = MX00_VG1762_CONTX3_BASE;
    } else {
        ContxRegBaseCur  = MX00_VG1762_CONTX0_BASE;
    }
    if(NextContxID == 0U) {
        ContxRegBaseNext  = MX00_VG1762_CONTX0_BASE;
    } else if(NextContxID == 1U) {
        ContxRegBaseNext  = MX00_VG1762_CONTX1_BASE;
    } else if(NextContxID == 2U) {
        ContxRegBaseNext  = MX00_VG1762_CONTX2_BASE;
    } else if(NextContxID == 3U) {
        ContxRegBaseNext  = MX00_VG1762_CONTX3_BASE;
    } else {
        ContxRegBaseNext  = MX00_VG1762_CONTX0_BASE;
    }


    //Step1
    RegAddr = ContxRegBaseNext + MX00_VG1762_CONTX_RPTCNT_OFFSET;
    WData = 1U;
    RetVal |= RegWrite(pChan, RegAddr, &WData, 1);
    RegAddr = ContxRegBaseNext + MX00_VG1762_NEXT_CONTX_OFFSET;
    WData = NextContxID;
    RetVal |= RegWrite(pChan, RegAddr, &WData, 1);
    //Step2
    RegAddr = ContxRegBaseCur + MX00_VG1762_CONTX_RPTCNT_OFFSET;
    WData = 1U;
    RetVal |= RegWrite(pChan, RegAddr, &WData, 1);
    RegAddr = ContxRegBaseCur + MX00_VG1762_NEXT_CONTX_OFFSET;
    WData = NextContxID;
    RetVal |= RegWrite(pChan, RegAddr, &WData, 1);

    //Step3 HDRCtrl
    if(RsvdMode == 1U) {
        WData = 0x04U; //HDR Subtraction
    } else {
        WData = 0x01U; //HDR Linearize
    }
    RegAddr = ContxRegBaseNext + MX00_VG1762_HDR_CTRL_OFFSET;
    RetVal |= RegWrite(pChan, RegAddr, &WData, 1);

    //Step4 LED Ctrl
    if(RsvdMode == 1U) {
        WData = 0x11U; //HDR Subtraction
    } else {
#ifdef IRLED_RGBIR
        WData = 0x23U; //HDR Linearize
#else
        WData = 0x00U; //HDR Linearize
#endif
    }
    RegAddr = ContxRegBaseNext + MX00_VG1762_SIGNALS_CTRL_OFFSET;
    RetVal |= RegWrite(pChan, RegAddr, &WData, 1);

    return RetVal;
}
/////
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_ConfigSerDes
 *
 *  @Description:: Configure Serdes sync/video forwarding, I2C translation, and MIPI I/F
 *
 *  @Input      ::
 *      Chan:        Vin ID and sensor ID
 *      SensorMode:  Sensor mode
 *      pModeInfo:   Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_VG1762_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const MX00_VG1762_SENSOR_INFO_s *pSensorInfo = &MX00_VG1762_SensorInfo[SensorMode];
    MAX9295_9296_SERDES_CONFIG_s *pSerdesCfg = &MX00_VG1762SerdesConfig[pChan->VinID];
    DOUBLE FloorVal;
    UINT32 U32RVal = 0U;

    pSerdesCfg->EnabledLinkID = MX00_VG1762_GetEnabledLinkID(pChan->SensorID);

    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;
    U32RVal |= AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal);
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    (void) Max9295_9296_Config(pChan->VinID, &MX00_VG1762SerdesConfig[pChan->VinID]);

    if(U32RVal != 0U){
        /* MisraC */
    }
}

#else
static MAX9295_9296_SERDES_CONFIG_s MX00_VG1762SerdesConfig[MX00_VG1762_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_VG1762_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] =  (UINT8)(MX00_VG1762_I2C_SLAVE_ADDRESS),
            [1] =  (UINT8)(MX00_VG1762_I2C_SLAVE_ADDRESS),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
    },
};
#endif




/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX9295_9296_VG1762_GetNumActiveSensor
 *
 *  @Description:: Get active sensor numbers
 *
 *  @Input      ::
 *      SensorID:      sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32:    Sensor numbers
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_VG1762_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            SensorNum ++;
        }
    }

    if(SensorNum <= 1U) {
        SensorNum = 1U;
    }

    return SensorNum;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_PrepareModeInfo
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
static void MX00_VG1762_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 ModeID                                               = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s              *pInputInfo     = &MX00_VG1762_InputInfo[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s             *pOutputInfo    = &MX00_VG1762_OutputInfo[ModeID];
    const MX00_VG1762_SENSOR_INFO_s     *pSensorInfo    = &MX00_VG1762_SensorInfo[ModeID];
    const MX00_VG1762_FRAME_TIMING_s    *pFrameTime     = &MX00_VG1762ModeInfoList[ModeID].FrameTime;
    UINT32 SensorNum = MX00_VG1762_GetNumActiveSensor(pChan->SensorID);
    DOUBLE FloorVal;
    UINT32 U32RVal = 0U;

    if (SensorNum != 0U) {
        pModeInfo->FrameLengthLines         = pSensorInfo->FrameLengthLines * SensorNum;
        pModeInfo->NumExposureStepPerFrame  = pSensorInfo->FrameLengthLines;
        pModeInfo->InternalExposureOffset   = 0.0f;
        pModeInfo->RowTime = MX00_VG1762ModeInfoList[ModeID].RowTime / (FLOAT)SensorNum;
        U32RVal |= AmbaWrap_floor((((DOUBLE)MX00_VG1762ModeInfoList[ModeID].PixelRate * (DOUBLE)SensorNum) * (DOUBLE)pModeInfo->RowTime) + 0.5, &FloorVal);
        pModeInfo->LineLengthPck            = (UINT32)FloorVal;
        pModeInfo->InputClk                 = pSensorInfo->InputClk;

        U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX00_VG1762_HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information

        /* update for MAX9296 CSI-2 output */
        pModeInfo->OutputInfo.DataRate = pModeInfo->OutputInfo.DataRate * SensorNum;
        pModeInfo->OutputInfo.OutputHeight = (UINT16)(pModeInfo->OutputInfo.OutputHeight * SensorNum);
        pModeInfo->OutputInfo.RecordingPixels.StartY = (UINT16)(pModeInfo->OutputInfo.RecordingPixels.StartY * SensorNum);
        pModeInfo->OutputInfo.RecordingPixels.Height = (UINT16)(pModeInfo->OutputInfo.RecordingPixels.Height * SensorNum);

        /* Updated minimum frame rate limitation */
        /*
        if (pModeInfo->HdrInfo.HdrType == 0U) {
            pModeInfo->MinFrameRate.TimeScale /= 8U;
        }
        */
    }

    if(U32RVal != 0U){
        /* MisraC */
    }
}




/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_ConfigVin
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
static UINT32 MX00_VG1762_ConfigVin(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX00_VG1762VinConfig = {
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
    AMBA_VIN_MIPI_CONFIG_s      *pVinCfg            = &MX00_VG1762VinConfig;
    UINT32 U32RVal = 0U;

    pVinCfg->Config.NumDataBits                     = pOutputInfo->NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels    = pOutputInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines     = pOutputInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels     = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines      = pModeInfo->FrameLengthLines;
    pVinCfg->Config.BayerPattern                    = pOutputInfo->BayerPattern;

    pVinCfg->NumActiveLanes                         = pModeInfo->OutputInfo.NumDataLanes;


    U32RVal |= AmbaWrap_memcpy(&(pVinCfg->Config.FrameRate), &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

    if(U32RVal != 0U){
        /* MisraC */
    }

    return AmbaVIN_MipiConfig(VinID, pVinCfg);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_ResetSensor
 *
 *  @Description:: Reset VG1762 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MX00_VG1762_ResetSensor(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
#ifdef MX00_VG1762_SERDES_ENABLE
    if ((pChan->SensorID & MX00_VG1762_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    if ((pChan->SensorID & MX00_VG1762_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
    if ((pChan->SensorID & MX00_VG1762_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    if ((pChan->SensorID & MX00_VG1762_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    (void) AmbaKAL_TaskSleep(30);  /* wait for communcation start */
#else
    RetVal = AmbaUserGPIO_SensorResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    if (RetVal == GPIO_ERR_NONE) {
        (void)AmbaKAL_TaskSleep(2);
        RetVal = AmbaUserGPIO_SensorResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        //RetVal = AmbaGPIO_SetFuncGPO(GPIO_PIN_113, AMBA_GPIO_LEVEL_HIGH);
    }
    (void)AmbaKAL_TaskSleep(10);
#endif
    if (RetVal != MAX9295_9296_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }

    return RetVal;
}

//TBD: boardcast only and not support burst write
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
static UINT32 RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX00_VG1762_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;
#ifdef DEBUG_MSG
    UINT32 k;
#endif

    if ((pChan == NULL) || (Size > MX00_VG1762_SENSOR_I2C_MAX_SIZE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(MX00_VG1762I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        if (pChan->SensorID == (MX00_VG1762_SENSOR_ID_CHAN_A | MX00_VG1762_SENSOR_ID_CHAN_B)) {
            I2cConfig.SlaveAddr = MX00_VG1762SerdesConfig[pChan->VinID].SensorSlaveID;

            /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
            RetVal = AmbaI2C_MasterWrite(MX00_VG1762I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                         &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Broadcast] Slave ID 0x%2x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U);
            for (k = 0U; k < Size; k++) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
            }
#endif

        } else {
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_VG1762SerdesConfig[pChan->VinID].SensorAliasID[i];
                    /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
                    RetVal |= AmbaI2C_MasterWrite(MX00_VG1762I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                                  &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
                    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[SensorID 0x%02x] Slave ID 0x%2x", (1 << i), I2cConfig.SlaveAddr, 0U, 0U, 0U);
                    for (k = 0U; k < Size; k++) {
                        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
                    }
#endif
                }
            }
        }
        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_VG1762] Addr 0x%04x Size %d I2C does not work!!!!!", Addr, Size, 0U, 0U, 0U);
        }
    }

    //SENSOR_DBG_IO("[MX00_VG1762][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;

}

#if 0
static UINT32 MX00_VG1762_RegWriteSeq(UINT32 VinID, UINT16 Addr, const UINT8 *pData, UINT32 Size)
{
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[54];//(max size: 52)+2
    UINT32 TxSize, i;
    UINT32 RetVal;

    I2cConfig.SlaveAddr = MX00_VG1762_I2C_SLAVE_ADDRESS;
    I2cConfig.DataSize = Size + 2U;
    I2cConfig.pDataBuf = TxDataBuf;

    TxDataBuf[0] = (UINT8)((Addr & 0xff00U) >> 8U);
    TxDataBuf[1] = (UINT8)(Addr & 0x00ffU);

    for (i = 0; i < Size; i++) {
        TxDataBuf[i + 2U] = pData[i];
    }

    RetVal = AmbaI2C_MasterWrite(MX00_VG1762I2cChannel[VinID], AMBA_I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        RetVal = SENSOR_ERR_COMMUNICATE;
    }

    return RetVal;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_RegisterWrite
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
static UINT32 MX00_VG1762_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
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

static UINT32 MX00_VG1762_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    if ((pChan->SensorID != MX00_VG1762_SENSOR_ID_CHAN_A) && (pChan->SensorID != MX00_VG1762_SENSOR_ID_CHAN_B)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cTxConfig.DataSize  = 2U;
        I2cTxConfig.pDataBuf  = TxData;
        TxData[0] = (UINT8)(Addr >> 8U);
        TxData[1] = (UINT8)(Addr & 0xffU);

        I2cRxConfig.DataSize  = 1U;
        I2cRxConfig.pDataBuf  = pRxData;

        if (pChan->SensorID == MX00_VG1762_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX00_VG1762SerdesConfig[pChan->VinID].SensorAliasID[0];
            I2cRxConfig.SlaveAddr = (MX00_VG1762SerdesConfig[pChan->VinID].SensorAliasID[0] | (UINT32)1U);
        } else {
            I2cTxConfig.SlaveAddr = MX00_VG1762SerdesConfig[pChan->VinID].SensorAliasID[1];
            I2cRxConfig.SlaveAddr = (MX00_VG1762SerdesConfig[pChan->VinID].SensorAliasID[1] | (UINT32)1U);
        }

        /*        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x, 0x%x", I2cTxConfig.SlaveAddr, I2cRxConfig.SlaveAddr, 0U, 0U, 0U); */
        RetVal = AmbaI2C_MasterReadAfterWrite(MX00_VG1762I2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                              &I2cRxConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX00_VG1762] Addr 0x%04x I2C does not work!!!!!", Addr, 0U, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_RegisterRead
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
static UINT32 MX00_VG1762_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 RData = 0U;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_VG1762_RegRead(pChan, Addr, &RData);
        *Data = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_SetAnalogGainReg
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
static UINT32 MX00_VG1762_SetAnalogGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 AnalogGainCtrl, UINT32 HDREnable)
{
    UINT32 RetVal= SENSOR_ERR_NONE;
    UINT8 WData[1];
    UINT8 WData2[2];
////
    //UINT32 RsvdMode = pChan->Reserved[0]; //0: Normal, 1: HDR subtraction
    UINT32 TmpU32;
    UINT8 NextContxID, CurContxID=0U;
    UINT16 ContxRegBase, RegAddr;
    TmpU32 = (pChan->Reserved[1] & 0x7FFFFFFFUL) % MX00_VG1762_MAX_CONTX_NUM;
    NextContxID = (UINT8)TmpU32;
    if(pChan->Reserved[1] != 0U) {
        if(NextContxID > 0U) {
            CurContxID = NextContxID - 1U;
        } else if(NextContxID == 0U) {
            CurContxID = (UINT8)(MX00_VG1762_MAX_CONTX_NUM - 1U);
        } else { /* MisraC */ }
    } else {
        CurContxID = NextContxID;
    }
////
    /* SP1H */
    WData[0] = (UINT8)(AnalogGainCtrl & 0xffU);
    WData2[0] = (UINT8)((AnalogGainCtrl & 0xff00U) >> 8U);
    WData2[1] = (UINT8)((AnalogGainCtrl & 0x070000U) >> 16U);
    if(HDREnable ==0U) {
        RetVal |= RegWrite(pChan, MX00_VG1762_AGAIN, WData, 1);
        RetVal |= RegWrite(pChan, MX00_VG1762_DGAIN_LONG, WData2, 2);
    } else {
////
#ifdef TESTCONTX
        if(NextContxID == 0U) {
            ContxRegBase = MX00_VG1762_CONTX0_BASE;
        } else if(NextContxID == 1U) {
            ContxRegBase = MX00_VG1762_CONTX1_BASE;
        } else if(NextContxID == 2U) {
            ContxRegBase = MX00_VG1762_CONTX2_BASE;
        } else { //this branch is (NextContxID == 3U)
            ContxRegBase = MX00_VG1762_CONTX3_BASE;
        }
        // MisraC. NextContxID in the range of [0,3]
        //else {
        //    ContxRegBase = MX00_VG1762_CONTX0_BASE;
        //}
        RegAddr = ContxRegBase + MX00_VG1762_AGAIN_OFFSET;
        RetVal |= RegWrite(pChan, RegAddr, WData, 1);
        RegAddr = ContxRegBase + MX00_VG1762_DGAIN_LONG_OFFSET;
        RetVal |= RegWrite(pChan, RegAddr, WData2, 2);
        RegAddr = ContxRegBase + MX00_VG1762_DGAIN_SHORT_OFFSET;
        RetVal |= RegWrite(pChan, RegAddr, WData2, 2);

        RetVal |= MX00_VG1762_Setup_ContxReg(pChan, CurContxID, NextContxID);
////
#else
        RetVal |= RegWrite(pChan, MX00_VG1762_AGAIN, WData, 1);
        RetVal |= RegWrite(pChan, MX00_VG1762_DGAIN_LONG, WData2, 2);
        RetVal |= RegWrite(pChan, MX00_VG1762_DGAIN_SHORT, WData2, 2);
#endif
    }

    return RetVal;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_SetShutterReg
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
static UINT32 MX00_VG1762_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 ShutterCtrl, UINT32 frameNum)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SHS1;
    UINT8 WData[2];
////
#ifdef TESTCONTX
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
    //UINT32 RsvdMode = pChan->Reserved[0]; //0: Normal, 1: HDR subtraction
    UINT32 TmpU32;
    UINT8 NextContxID, CurContxID=0U;
    UINT16 ContxRegBase, RegAddr;
    TmpU32 = (pChan->Reserved[1] & 0x7FFFFFFFUL) % MX00_VG1762_MAX_CONTX_NUM;
    NextContxID = (UINT8)TmpU32;
    if(pChan->Reserved[1] != 0U) {
        if(NextContxID > 0U) {
            CurContxID = NextContxID - 1U;
        } else if(NextContxID == 0U) {
            CurContxID = (UINT8)(MX00_VG1762_MAX_CONTX_NUM - 1U);
        } else { /* MisraC */ }
    } else {
        CurContxID = NextContxID;
    }

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
        /* Chopping Window & LONG exposure */
        SHS1 = ShutterCtrl;

        WData[0] = (UINT8)(SHS1 & 0xffU);
        WData[1] = (UINT8)((SHS1 >> 8U) & 0xffU);
        if(NextContxID == 0U) {
            ContxRegBase = MX00_VG1762_CONTX0_BASE;
        } else if(NextContxID == 1U) {
            ContxRegBase = MX00_VG1762_CONTX1_BASE;
        } else if(NextContxID == 2U) {
            ContxRegBase = MX00_VG1762_CONTX2_BASE;
        } else { //this branch is (NextContxID == 3U)
            ContxRegBase = MX00_VG1762_CONTX3_BASE;
        }
        // MisraC. NextContxID in the range of [0,3]
        //else {
        //    ContxRegBase = MX00_VG1762_CONTX0_BASE;
        //}
        if(frameNum == 0U) {
            RegAddr = ContxRegBase + MX00_VG1762_COARSE_EXPL_OFFSET;
            RetVal |= RegWrite(pChan, RegAddr, WData, 2);
        } else {
            RegAddr = ContxRegBase + MX00_VG1762_COARSE_EXPS_OFFSET;
            RetVal |= RegWrite(pChan, RegAddr, WData, 2);
        }
        RetVal |= MX00_VG1762_Setup_ContxReg(pChan, CurContxID, NextContxID);
    } else {
        /* Chopping Window & LONG exposure */
        SHS1 = ShutterCtrl;

        WData[0] = (UINT8)(SHS1 & 0xffU);
        WData[1] = (UINT8)((SHS1 >> 8U) & 0xffU);
        if(frameNum == 0U) {
            RetVal |= RegWrite(pChan, MX00_VG1762_COARSE_EXPOSURE_LONG, WData, 2);
        } else {
            RetVal |= RegWrite(pChan, MX00_VG1762_COARSE_EXPOSURE_SHORT, WData, 2);
        }
    }

#else
    /* Chopping Window & LONG exposure */
    SHS1 = ShutterCtrl;

    WData[0] = (UINT8)(SHS1 & 0xffU);
    WData[1] = (UINT8)((SHS1 >> 8U) & 0xffU);
    if(frameNum == 0U) {
        RetVal |= RegWrite(pChan, MX00_VG1762_COARSE_EXPOSURE_LONG, WData, 2);
    } else {
        RetVal |= RegWrite(pChan, MX00_VG1762_COARSE_EXPOSURE_SHORT, WData, 2);
    }
#endif
////
////




    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_StandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_VG1762_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= MX00_VG1762_RegisterWrite(pChan, MX00_VG1762_STANDBY, 0x0);
//    AmbaPrint("[MX00_VG1762] MX00_VG1762_StandbyOn");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_StandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_VG1762_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    //UINT8 TxData = 0x02U;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= MX00_VG1762_RegisterWrite(pChan, MX00_VG1762_STANDBY, 0x2);
    //(void)AmbaKAL_TaskSleep(2);

    //TxData = 0xA3U;
    //RetVal |= RegWrite(pChan, MX00_VG1762_STANDBY, &TxData, 1);
    (void)AmbaKAL_TaskSleep(10);

    return RetVal;
    /* Wait 8 frames */
//    AmbaKAL_TaskSleep(8 * 1000 * MX00_VG1762Ctrl.Status.ModeInfo.FrameRate.NumUnitsInTick
//                      / MX00_VG1762Ctrl.Status.ModeInfo.FrameRate.TimeScale);

//    AmbaPrint("[MX00_VG1762] MX00_VG1762_StandbyOff");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_ChangeReadoutMode
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
static UINT32 MX00_VG1762_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 i;
    UINT16 Addr, AddrBase;
    UINT32 RetVal = SENSOR_ERR_NONE;

    const UINT8* Data;
    UINT16 DataTmp;
    UINT32 DataSize;
    UINT32 U32RVal = 0U;

    (void)AmbaKAL_TaskSleep(20U);

    for (i = 0; i < MX00_VG1762_NUM_MODE_REG; i ++) {
        Addr = MX00_VG1762_RegTable[i].Addr;
        Data = &MX00_VG1762_RegTable[i].Data[0];
        DataSize = MX00_VG1762_RegTable[i].DataSize;
        RetVal |= RegWrite(pChan, Addr, Data, DataSize);
        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", MX00_VG1762_RegTable[i].Addr, MX00_VG1762_RegTable[i].Data, RetVal, 0U, 0U);
    }

    //(void)AmbaKAL_TaskSleep(10U);
    RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0201, 0x10);
    (void)AmbaKAL_TaskSleep(400U);
    RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0220, 0x00);
    RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0221, 0x36);
    RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0222, 0x6e);
    RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0223, 0x01);
    //RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0224, 0x02);
    RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0224, 0x04);
    RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0225, 0x86);
    RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0530, 0x00);
    if((ModeID == MX00_VG1762_1920_1080_SHDR_30P) || (ModeID == MX00_VG1762_1920_1080_SHDR_60P) || (ModeID == MX00_VG1762_1920_1080_SHDR_30P_1)) {
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0400, 0x0c);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0401, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0402, 0x3e);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0403, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0404, 0x8b);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0405, 0x07);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0406, 0x75);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0407, 0x04);
#ifndef TESTCONTX
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051c, 0x3e);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051d, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051e, 0x75);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051f, 0x04);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0532, 0x01);
#endif
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0a60, 0x04);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0305, 0x0c);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0304, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0306, 0x84);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0307, 0x68);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0302, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x030a, 0x2c);
#ifndef TESTCONTX
        if((ModeID == MX00_VG1762_1920_1080_SHDR_30P) || (ModeID == MX00_VG1762_1920_1080_SHDR_30P_1)) {
            RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051a, 0x43);
            RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051b, 0x0a);
        } else {
            RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051a, 0x21);
            RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051b, 0x05);
        }
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0500, 0x30);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0501, 0x01);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0504, 0x13);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0505, 0x00);
#endif
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0320, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0321, 0x00);
#ifndef TESTCONTX
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0522, 0x23);
#endif
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0478, 0x02);// user PWL lut
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0958, 0x0F);
        for (i = 0; i < MX00_VG1762_NUM_LUT16T12; i ++) {
            Addr = MX00_VG1762Lut16to12Table[i].Addr;
            Data = &MX00_VG1762Lut16to12Table[i].Data[0];
            DataSize = MX00_VG1762Lut16to12Table[i].DataSize;
            RetVal |= RegWrite(pChan, Addr, Data, DataSize);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", MX00_VG1762Lut16to12Table[i].Addr, MX00_VG1762Lut16to12Table[i].Data, RetVal, 0U, 0U);
        }
#ifndef TESTCONTX
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0534, 0x01);
#endif
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0300, 0xF8);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0301, 0x07);
#ifndef TESTCONTX
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0538, 0x00); //align the brightness to Short Exposure
#endif
////
#ifdef TESTCONTX
        ///Initialize contx0
        AddrBase = MX00_VG1762_CONTX0_BASE;
        for (i = 0; i < MX00_VG1762_NUM_CONTX_REG; i ++) {
            Addr = MX00_VG1762ContxTable[i].Addr + AddrBase;
            DataTmp = (UINT16)MX00_VG1762ContxTable[i].Data[ModeID];
            RetVal |= MX00_VG1762_RegisterWrite(pChan, Addr, DataTmp);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", Addr, Data, RetVal, 0U, 0U);
        }
        ///Initialize contx1
        AddrBase = MX00_VG1762_CONTX1_BASE;
        for (i = 0; i < MX00_VG1762_NUM_CONTX_REG; i ++) {
            Addr = MX00_VG1762ContxTable[i].Addr + AddrBase;
            DataTmp = (UINT16)MX00_VG1762ContxTable[i].Data[ModeID];
            RetVal |= MX00_VG1762_RegisterWrite(pChan, Addr, DataTmp);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", Addr, Data, RetVal, 0U, 0U);
        }
        ///Initialize contx2
        AddrBase = MX00_VG1762_CONTX2_BASE;
        for (i = 0; i < MX00_VG1762_NUM_CONTX_REG; i ++) {
            Addr = MX00_VG1762ContxTable[i].Addr + AddrBase;
            DataTmp = (UINT16)MX00_VG1762ContxTable[i].Data[ModeID];
            RetVal |= MX00_VG1762_RegisterWrite(pChan, Addr, DataTmp);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", Addr, Data, RetVal, 0U, 0U);
        }
        ///Initialize contx3
        AddrBase = MX00_VG1762_CONTX3_BASE;
        for (i = 0; i < MX00_VG1762_NUM_CONTX_REG; i ++) {
            Addr = MX00_VG1762ContxTable[i].Addr + AddrBase;
            DataTmp = (UINT16)MX00_VG1762ContxTable[i].Data[ModeID];
            RetVal |= MX00_VG1762_RegisterWrite(pChan, Addr, DataTmp);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr = 0x%04x, Data = 0x%02x, Ret: %d", Addr, Data, RetVal, 0U, 0U);
        }
#endif
////
    } else {
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0400, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0401, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0402, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0403, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0404, 0x97);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0405, 0x07);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0406, 0xB3);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0407, 0x04);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051C, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051D, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051E, 0xB3);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051F, 0x04);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0532, 0x0A);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0320, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0321, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0478, 0x01);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0534, 0x01);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0a60, 0x04);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0305, 0x0C);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0304, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0306, 0x84);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0307, 0x68);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0302, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x030a, 0x2c);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051A, 0x43);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x051B, 0x0A);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0500, 0x90);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0501, 0x01);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0504, 0x19);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0505, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0300, 0xF8);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0301, 0x07);
    }

    if(ModeID == MX00_VG1762_1920_1080_SHDR_30P_1) {
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0532, 0x04);

        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0500, 0x13);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0501, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0504, 0x13);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0505, 0x00);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0522, 0x11);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0523, 0x11);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0478, 0x00);//PWL off
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x0534, 0x01);
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x054a, 0x00);// UI.STREAM_CTX0.CONTEXT_REPEAT_COUNT
        RetVal |= MX00_VG1762_RegisterWrite(pChan, 0x054c, 0x00);// UI.STREAM_CTX0.NEXT_CONTEXT

    } else { /* MisraC */ }

    (void)AmbaKAL_TaskSleep(10U);

    /* Reset current AE information */
    U32RVal |= AmbaWrap_memset(MX00_VG1762Ctrl[pChan->VinID].CurrentAgcCtrl, 0x0, sizeof(UINT32) * 3U * MX00_VG1762_NUM_MAX_SENSOR_COUNT);
    U32RVal |= AmbaWrap_memset(MX00_VG1762Ctrl[pChan->VinID].CurrentDgcCtrl, 0x0, sizeof(UINT32) * 3U * MX00_VG1762_NUM_MAX_SENSOR_COUNT);
    U32RVal |= AmbaWrap_memset(MX00_VG1762Ctrl[pChan->VinID].CurrentWbCtrl, 0x0, sizeof(AMBA_SENSOR_WB_CTRL_s) * 3U * MX00_VG1762_NUM_MAX_SENSOR_COUNT);
    U32RVal |= AmbaWrap_memset(MX00_VG1762Ctrl[pChan->VinID].CurrentShutterCtrl, 0x0, sizeof(UINT32) * 3U * MX00_VG1762_NUM_MAX_SENSOR_COUNT);

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_VG1762_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;

    for (i = 0; i < MX00_VG1762_NUM_MODE; i++) {

        MX00_VG1762ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)MX00_VG1762_SensorInfo[i].LineLengthPck /
                                             ((DOUBLE)MX00_VG1762_SensorInfo[i].DataRate *
                                              (DOUBLE)MX00_VG1762_SensorInfo[i].NumDataLanes /
                                              (DOUBLE)MX00_VG1762_SensorInfo[i].NumDataBits));
        pOutputInfo = &MX00_VG1762_OutputInfo[i];
        MX00_VG1762ModeInfoList[i].PixelRate = (FLOAT) ((DOUBLE) pOutputInfo->DataRate *
                                               (DOUBLE)pOutputInfo->NumDataLanes /
                                               (DOUBLE)pOutputInfo->NumDataBits);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_Init
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
static UINT32 MX00_VG1762_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const MX00_VG1762_FRAME_TIMING_s *pFrameTime = &MX00_VG1762ModeInfoList[0U].FrameTime;

    AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "MX00_VG1762 INIT", NULL, NULL, NULL, NULL, NULL);
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pFrameTime->InputClk);
        MX00_VG1762_PreCalculateModeInfo();
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_Enable
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
static UINT32 MX00_VG1762_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;


    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal = MX00_VG1762_ResetSensor(pChan);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "RESET sensor @ %x", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_Disable
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
static UINT32 MX00_VG1762_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_VG1762_StandbyOn(pChan);
    }

    //AmbaPrint("[MX00_VG1762] MX00_VG1762_Disable");

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_GetStatus
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
static UINT32 MX00_VG1762_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0U;

    if ((pStatus == NULL) || (pChan == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pStatus, &MX00_VG1762Ctrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_GetModeInfo
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
static UINT32 MX00_VG1762_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {

        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if (Config.ModeID >= MX00_VG1762_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {

            MX00_VG1762_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_GetDeviceInfo
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
static UINT32 MX00_VG1762_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
    //UINT32 ModeID = pMode->ModeID;
    UINT32 RsvdMode;// = pChan->Reserved[0]; //0: Normal, 1: context switching
    UINT32 U32RVal = 0U;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
        RsvdMode = pChan->Reserved[0];
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &MX00_VG1762_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
        if(RsvdMode == 1U) { //context switching mode
            if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                pDeviceInfo->FrameRateCtrlInfo.FirstReflectedFrame = 3U;
                pDeviceInfo->ShutterSpeedCtrlInfo.FirstReflectedFrame = 3U;
                pDeviceInfo->AnalogGainCtrlInfo.FirstReflectedFrame = 3U;
                pDeviceInfo->DigitalGainCtrlInfo.FirstReflectedFrame = 3U;
                pDeviceInfo->WbGainCtrlInfo.FirstReflectedFrame = 3U;
                pDeviceInfo->StrobeCtrlInfo.FirstReflectedFrame = 3U;
            }
        }
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_GetHdrInfo
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
static UINT32 MX00_VG1762_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0U;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pHdrInfo, &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_GetCurrentGainFactor
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
static UINT32 MX00_VG1762_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k/*, TotalGainCtrl*/;
    UINT32 AGAIN,DGAIN;
    //DOUBLE GainFactor64 = 0.0;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    AGAIN = (MX00_VG1762Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] & 0xffU);
                    DGAIN = MX00_VG1762Ctrl[pChan->VinID].CurrentDgcCtrl[i][0];
                    pGainFactor[0] = (32.0f / (32.0f - (FLOAT)AGAIN)) * (((FLOAT)DGAIN) / 256.0f);
                    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentGain: i=%d, %d ", 0, pGainFactor[0]*1000, 0, 0, 0);
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        AGAIN = (MX00_VG1762Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] & 0xffU);
                        DGAIN = MX00_VG1762Ctrl[pChan->VinID].CurrentDgcCtrl[i][0];
                        pGainFactor[k] = (32.0f / (32.0f - (FLOAT)AGAIN)) * (((FLOAT)DGAIN) / 256.0f);
                        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentGain: i=%d, %d ", k, pGainFactor[0]*1000, 0, 0, 0);
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_GetCurrentShutterSpeed
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
static UINT32 MX00_VG1762_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 FrameLengthLines;// = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame;// = pModeInfo->NumExposureStepPerFrame;
    DOUBLE ShutterTimeUnit;
    UINT8 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
        NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
        FrameLengthLines = pModeInfo->FrameLengthLines;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    pExposureTime[0] = ((FLOAT)ShutterTimeUnit * (FLOAT)MX00_VG1762Ctrl[pChan->VinID].CurrentShutterCtrl[i][0]);

                    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentShutter: i=%d, %d ", 0, pExposureTime[0]*1000000, 0, 0, 0);
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        pExposureTime[k] = ((FLOAT)ShutterTimeUnit * (FLOAT)MX00_VG1762Ctrl[pChan->VinID].CurrentShutterCtrl[i][k]);

                        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "GetCurrentShutter: i=%d, %d ", k, pExposureTime[k]*1000000, 0, 0, 0);
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    //AmbaPrint("ExposureTime:%f", *pExposureTime);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_ConvertGainFactor
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
static UINT32 MX00_VG1762_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 HdrType;// = MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
    FLOAT DesiredFactor;
    FLOAT Agc = 1.0f, Dgc = 1.0f;
    DOUBLE FloorVal;
    UINT32 Gain,DGAIN;
    UINT32 U32RVal = 0U;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        HdrType = MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType;
        DesiredFactor = pDesiredFactor->Gain[0];

        /* Maximum check */
        DesiredFactor = (DesiredFactor >= MX00_VG1762_MAX_TOTAL_GAIN) ? MX00_VG1762_MAX_TOTAL_GAIN : DesiredFactor;
        /* Minimum check */
        DesiredFactor = (DesiredFactor <= 1.0f) ? 1.0f : DesiredFactor;
        /* Calculate Agc/Dgc for SP1H/SP1L */
        if (DesiredFactor > MX00_VG1762_MAX_AGAIN) {
            Agc = MX00_VG1762_MAX_AGAIN;
        } else {
            Agc = DesiredFactor;
        }
        U32RVal |= AmbaWrap_floor((32.0f - (32.0f / (DOUBLE)Agc)), &FloorVal);
        Gain = (UINT32)FloorVal;
        Agc = 32.0f / (32.0f-(FLOAT)Gain);
        Dgc = DesiredFactor/Agc;
        if(Dgc > MX00_VG1762_MAX_DGAIN) {
            Dgc = MX00_VG1762_MAX_DGAIN;
        }
        U32RVal |= AmbaWrap_floor((DOUBLE)Dgc * 256.0f, &FloorVal);
        DGAIN = (UINT32)FloorVal;
        pGainCtrl->AnalogGain[0] = Gain + (DGAIN << 8U);
        pGainCtrl->DigitalGain[0] = DGAIN;
        pActualFactor->Gain[0] = Agc * Dgc;

        if (HdrType != AMBA_SENSOR_HDR_NONE) {
            pGainCtrl->AnalogGain[1] = Gain + (DGAIN << 8U);
            pGainCtrl->DigitalGain[1] = DGAIN;
            pActualFactor->Gain[1] = Agc * Dgc;
            //
        } else {
            /* For linear mode (TBD) */

        }

    }

    if(U32RVal != 0U){
        /* MisraC */
    }

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
static void ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    UINT32 MaxExposureline, MinExposureline;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 ShutterCtrl;
    DOUBLE ShutterTimeUnit, ShutterCtrlInDb;
    UINT32 RsvdMode = pChan->Reserved[0]; //0: Normal, 1: HDR subtraction
    UINT32 ModeID = pModeInfo->Config.ModeID;
    UINT32 U32RVal = 0U;

    ShutterTimeUnit = (DOUBLE)pModeInfo->RowTime * (DOUBLE)FrameLengthLines / (DOUBLE)NumExposureStepPerFrame;

    U32RVal |= AmbaWrap_floor(((DOUBLE)ExposureTime / ShutterTimeUnit), &ShutterCtrlInDb);
    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    /* VMAX is 16 bits */
    if (ShutterCtrl > ((0xffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame)) {
        ShutterCtrl = ((0xffffU / pModeInfo->NumExposureStepPerFrame) * pModeInfo->NumExposureStepPerFrame);
    }

    /* For HDR mode */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        if(RsvdMode == 1U) { //HDR subtraction mode
            if(ModeID == MX00_VG1762_1920_1080_SHDR_30P) {
                MaxExposureline = 1280U;
                MinExposureline = 10U;
            } else if(ModeID == MX00_VG1762_1920_1080_SHDR_60P) {
                MaxExposureline = 39U;
                MinExposureline = 10U;
            } else {
                MaxExposureline = pModeInfo->HdrInfo.ChannelInfo[1].MaxExposureLine >> 1U;
                MinExposureline = pModeInfo->HdrInfo.ChannelInfo[1].MinExposureLine;
            }
        } else { //HDR linearize mode
            MaxExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
            MinExposureline = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
        }
        /* For none-HDR mode */
    } else {
        MaxExposureline = FrameLengthLines - 12U;//VMAX = 1332
        MinExposureline = 19U;
    }

    if (ShutterCtrl > MaxExposureline) {
        ShutterCtrl = MaxExposureline;
    } else if (ShutterCtrl < MinExposureline) {
        ShutterCtrl = MinExposureline;
    } else {
        /* MisraC */  /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl   = ShutterCtrl;
    *pActualExptime = (FLOAT)ShutterCtrl * (FLOAT)ShutterTimeUnit;

    if(U32RVal != 0U){
        /* MisraC */
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_ConvertShutterSpeed
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
static UINT32 MX00_VG1762_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pChan, pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < pModeInfo->HdrInfo.ActiveChannels; i ++) {
                ConvertShutterSpeed(pChan, pModeInfo, i, pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i]);
            }
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SSSShutter: i=%d pDesiredExposureTime=%d pActualExposureTime=%d pShutterCtrl=%d", 0, pDesiredExposureTime[0]*1000000, pActualExposureTime[0]*1000000, pShutterCtrl[0], 0);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SSSShutter: i=%d pDesiredExposureTime=%d pActualExposureTime=%d pShutterCtrl=%d", 1, pDesiredExposureTime[1]*1000000, pActualExposureTime[1]*1000000, pShutterCtrl[1], 0);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SSSShutter: i=%d pDesiredExposureTime=%d pActualExposureTime=%d pShutterCtrl=%d", 2, pDesiredExposureTime[2]*1000000, pActualExposureTime[2]*1000000, pShutterCtrl[2], 0);
            //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SSSShutter: LONG=%d MEDIUM=%d SHORT=%d", pShutterCtrl[0], pShutterCtrl[1], pShutterCtrl[2], 0, 0);
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "SSSShutter: i=%d pDesiredExposureTime=%d pActualExposureTime=%d pShutterCtrl=%d", 0, pDesiredExposureTime[0]*1000000, pActualExposureTime[0]*1000000, pShutterCtrl[0], 0);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_SetAnalogGainCtrl
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
static UINT32 MX00_VG1762_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_VG1762_SetAnalogGainReg(pChan, pAnalogGainCtrl[0],0U);

            /* Update current analog gain control */
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_VG1762Ctrl[pChan->VinID].CurrentAgcCtrl[i][0] = pAnalogGainCtrl[0];
                    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Set again = %d", pAnalogGainCtrl[0]*1000, 0, 0, 0, 0);
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal = MX00_VG1762_SetAnalogGainReg(pChan, pAnalogGainCtrl[0],1U);
            /* Update current analog gain control */
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_VG1762Ctrl[pChan->VinID].CurrentAgcCtrl[i][k] = pAnalogGainCtrl[k];
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_SetDigitalGainCtrl
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
static UINT32 MX00_VG1762_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            /* Update current digital gain control */
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_VG1762Ctrl[pChan->VinID].CurrentDgcCtrl[i][0]= pDigitalGainCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            /* Update current digital gain control */
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                        MX00_VG1762Ctrl[pChan->VinID].CurrentDgcCtrl[i][k]= pDigitalGainCtrl[k];
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_SetShutterCtrl
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
static UINT32 MX00_VG1762_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 i, k;
    UINT32 RsvdMode;// = pChan->Reserved[0]; //0: Normal, 1: HDR subtraction

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
        RsvdMode = pChan->Reserved[0];
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_VG1762_SetShutterReg(pChan, pShutterCtrl[0],0U);

            /* Update current shutter control */
            for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    MX00_VG1762Ctrl[pChan->VinID].CurrentShutterCtrl[i][0] = pShutterCtrl[0];
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {

            /* Update current shutter control */
            if(RsvdMode == 1U) {
                for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                            RetVal = MX00_VG1762_SetShutterReg(pChan, pShutterCtrl[0],k);
                            MX00_VG1762Ctrl[pChan->VinID].CurrentShutterCtrl[i][k] = pShutterCtrl[0];
                        }
                    }
                }
            } else {
                for (i = 0U; i < MX00_VG1762_NUM_MAX_SENSOR_COUNT; i++) {
                    if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                        for (k = 0U; k < pModeInfo->HdrInfo.ActiveChannels; k ++) {
                            RetVal = MX00_VG1762_SetShutterReg(pChan, pShutterCtrl[k],k);
                            MX00_VG1762Ctrl[pChan->VinID].CurrentShutterCtrl[i][k] = pShutterCtrl[k];
                        }
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_SetSlowShrCtrl
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
//static UINT32 MX00_VG1762_SetSlowShrCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
//{
//    UINT32 RetVal = SENSOR_ERR_NONE;
//
//    if (pChan == NULL) {
//        RetVal = SENSOR_ERR_ARG;
//    } else if (MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo.HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE){
//        RetVal = SENSOR_ERR_INVALID_API;
//    } else {
//        RetVal = MX00_VG1762_SetSlowShutterReg(SlowShutterCtrl);
//    }
//
//    return RetVal;
//}

#ifdef MX00_VG1762_IN_SLAVE_MODE
static UINT32 MX00_VG1762_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{


    UINT32 RetVal;
    DOUBLE PeriodInDb;
    const MX00_VG1762_MODE_INFO_s* pModeInfo = &MX00_VG1762ModeInfoList[ModeID];
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg  = {0};
    UINT32 U32RVal = 0U;

    U32RVal |= AmbaWrap_floor(((DOUBLE)pModeInfo->FrameTime.FrameRate.NumUnitsInTick * ((DOUBLE) MX00_VG1762_SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameTime.FrameRate.TimeScale)) + 0.5, &PeriodInDb);
    MasterSyncCfg.RefClk            = MX00_VG1762_SensorInfo[ModeID].InputClk;
    MasterSyncCfg.HSync.Period      = (UINT32)PeriodInDb;
    MasterSyncCfg.HSync.PulseWidth  = 8U;
    MasterSyncCfg.HSync.Polarity    = 0U;
    MasterSyncCfg.VSync.Period      = 1U;
    MasterSyncCfg.VSync.PulseWidth  = 1000U;
    MasterSyncCfg.VSync.Polarity    = 0U;
    MasterSyncCfg.HSyncDelayCycles  = 1U;
    MasterSyncCfg.VSyncDelayCycles  = 0U;
    MasterSyncCfg.ToggleHsyncInVblank  = 1U;

    RetVal = AmbaVIN_MasterSyncEnable(MX00_VG1762MasterSyncChannel[pChan->VinID], &MasterSyncCfg);

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_VG1762_Config
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
static UINT32 MX00_VG1762_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = 0xfU,   /* SD_LVDS_0~3 */
        [AMBA_VIN_CHANNEL1] = 0xf0U,  /* SD_LVDS_4~7 */
    };
    AMBA_VIN_MIPI_PAD_CONFIG_s MX00_VG1762PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        //.EnabledPin = EnabledPin[pChan->VinID]
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_VG1762Ctrl[pChan->VinID].Status.ModeInfo;
    UINT32 ModeID = pMode->ModeID;
#ifdef MX00_VG1762_SERDES_ENABLE
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Add delay time to wait MAX9296/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power off", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power on", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(3);
#endif
    if (ModeID >= MX00_VG1762_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  [ MX00_VG1762_Config ]  ============", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d ============", ModeID, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

        MX00_VG1762PadConfig.EnabledPin = EnabledPin[pChan->VinID];

        /* update status */
        MX00_VG1762_PrepareModeInfo(pChan, pMode, pModeInfo);

#ifdef MX00_VG1762_IN_SLAVE_MODE
        (void)AmbaVIN_MasterSyncDisable(MX00_VG1762MasterSyncChannel[pChan->VinID]);
#endif
        /* Adjust mipi-phy parameters */
        MX00_VG1762PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", (UINT32)MX00_VG1762PadConfig.DateRate, 0U, 0U, 0U, 0U);
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX00_VG1762PadConfig);
#ifdef MX00_VG1762_SERDES_ENABLE
        /* After reset VIN, set MAX9296A PWDNB high */
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB low", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(3);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB high", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_9296_Init(pChan->VinID, MX00_VG1762_GetEnabledLinkID(pChan->SensorID));

        MX00_VG1762_ConfigSerDes(pChan, ModeID, pModeInfo);

        /* set pll */
//#ifdef MX00_VG1762_IN_SLAVE_MODE
//        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
//#endif

        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX00_VG1762_SENSOR_ID_CHAN_A) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
            RetVal |= Max9295_9296_RegisterWrite(pChan->VinID, MX00_MAX9295_A_ID, 0x0100, 0x68); //set max9295 BPP as auto
        }
        if ((pChan->SensorID & MX00_VG1762_SENSOR_ID_CHAN_B) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
        }
#else
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
#endif
        (void)MX00_VG1762_ResetSensor(pChan);


        /* program sensor */
        (void)MX00_VG1762_ChangeReadoutMode(pChan, ModeID);

#ifdef MX00_VG1762_IN_SLAVE_MODE
        {
            UINT8 Data;
            Data = 0x00U;
            RetVal |= RegWrite(pChan, 0x01f0U, &Data, 1U);//External Pulse-based sync
            Data = 0x01U;
            RetVal |= RegWrite(pChan, 0x01f1U, &Data, 1U);//Active Low
        }

#endif
        (void)MX00_VG1762_StandbyOff(pChan);

#ifdef MX00_VG1762_IN_SLAVE_MODE
        RetVal |= MX00_VG1762_ConfigMasterSync(pChan, ModeID);
#endif

        //(void)AmbaKAL_TaskSleep(22); //TBD: To avoid i2c fail for gain setting

        /* config vin */
        RetVal |= MX00_VG1762_ConfigVin(pChan->VinID, pModeInfo);
    }

    return RetVal;
}

#if 0
static INT32 MX00_VG1762_ConfigPost(UINT32 *pVinID)
{
    MX00_VG1762_StandbyOff();

    return SENSOR_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX00_VG1762Obj = {
    .SensorName             = "VG1762",
    .SerdesName             = "MAX9295_9296",
    .Init                   = MX00_VG1762_Init,
    .Enable                 = MX00_VG1762_Enable,
    .Disable                = MX00_VG1762_Disable,
    .Config                 = MX00_VG1762_Config,
    .GetStatus              = MX00_VG1762_GetStatus,
    .GetModeInfo            = MX00_VG1762_GetModeInfo,
    .GetDeviceInfo          = MX00_VG1762_GetDeviceInfo,
    .GetHdrInfo             = MX00_VG1762_GetHdrInfo,
    .GetCurrentGainFactor   = MX00_VG1762_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX00_VG1762_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX00_VG1762_ConvertGainFactor,
    .ConvertShutterSpeed    = MX00_VG1762_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX00_VG1762_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MX00_VG1762_SetDigitalGainCtrl,
    .SetWbGainCtrl          = NULL,
    .SetShutterCtrl         = MX00_VG1762_SetShutterCtrl,
    //.SetSlowShutterCtrl     = MX00_VG1762_SetSlowShrCtrl,

    .RegisterRead           = MX00_VG1762_RegisterRead,
    .RegisterWrite          = MX00_VG1762_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus    = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};

